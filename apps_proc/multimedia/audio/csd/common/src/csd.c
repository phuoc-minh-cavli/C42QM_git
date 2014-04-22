
/*============================================================================

   FILE:       csd.c

   DESCRIPTION: Contains Core Sound Driver's dispatcher.

   PUBLIC CLASSES:  Not Applicable

   INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright © 2010 - 2013, 2016 QUALCOMM Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
 ===========================================================================*/

/*===========================================================================
                      EDIT HISTORY FOR MODULE

This section contains comments describing changes made to this module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/audio.tx/2.0/audio/csd/common/src/csd.c#4 $
$DateTime: 2019/06/21 00:18:23 $
$Author: pwbldsvc $
$Change: 19615600 $
$Revision: #4 $

when       who      what, where, why
--------   ---      ----------------------------------------------------------
09/01/14    rp      enhanced the csd init debug buffer
04/16/14    jk      added support for voice session manager init/deinit
02/06/14    vk      modified code for proper csd/acdb/apr error mapping
10/23/12    jk      added dynamic module enablement based on config file
09/28/12    rp      added fix for csd init reference count overflow
06/27/12    jk      added support for csd cleanup 
08/24/11    sr      Updated Copyright and added History/Author.
                    Removed csd_voc_common_init
============================================================================*/

#include "mmdefs.h"  
#include "csd_os_dependencies.h"
#include "csd_debug_msg.h"    
#include "csd.h"    
#include "csd_intf.h"
#include "csd_handle.h"
#include "DALSys.h"
#include "DDIChipInfo.h"
#include "csd_resourcemanager.h"
#include "apr_errcodes.h"
#include "apr_api.h"
#include "csd_debug_info.h"
#include "csd_cleanup.h"
#include "csd_vsm.h"
#include "csd_voc_common.h"
#include "csd_error_map.h"
#include "pm_gpio.h"
#include "pm_err_flags.h"

#define CSD_DRIVER_VER ((CSD_API_VER_MAIN<<24)+(CSD_API_VER_MINOR<<16)+CSD_API_VER_REV)

#define CSD_HANDLE_SIZE         128     //64     //1024

#define CSD_INIT_OVERFLOW  0xFFFFFFFF

typedef struct {
  uint32_t    ref_count;
  struct csd_vtable *fn_tbl[CSD_OPEN_MAX_NUM];
} csd_main_data_type;
static csd_main_data_type csd_main_data; 
static int acdb_init_count =0,voc_init_count=0;
/*! note: Any addition of new module to init/deinit sequence should make sure two things:
    1. Increment the CSD_INIT_STAGE_MAX accordingly, + comemnt addition in enum update
    2. Insert the DeInit appropriately in the exact (revere)order as followed in the Init chain.
. 
*/
#if 0
typedef enum csd_init_senquence_stages
{
  CSD_INIT_STAGE_0    = 0,    // NONE
  CSD_INIT_STAGE_1    = 1,    // csd_os_dep_dal_init
  CSD_INIT_STAGE_2    = 2,    // csd_pmem_init
  CSD_INIT_STAGE_3    = 3,    // csd_handle_init
  CSD_INIT_STAGE_4    = 4,    // csd_rm_init 
  CSD_INIT_STAGE_5    = 5,    // csd_atl_init
  CSD_INIT_STAGE_6    = 6,    // csd_acdb_init
  CSD_INIT_STAGE_7    = 7,    // csd_dev_init
  CSD_INIT_STAGE_8    = 8,    // csd_dev_debug_init
  CSD_INIT_STAGE_9    = 9,    // csd_ssr_init
  CSD_INIT_STAGE_10   = 10,   // csd_as_init
  CSD_INIT_STAGE_11   = 11,   // csd_as_debug_init 
  CSD_INIT_STAGE_12   = 12,   // csd_ac_init
  CSD_INIT_STAGE_13   = 13,   // csd_ac_debug_init
  CSD_INIT_STAGE_14   = 14,   // csd_voc_common_init
  CSD_INIT_STAGE_15   = 15,   // csd_vs_init
  CSD_INIT_STAGE_16   = 16,   // csd_vc_init
  CSD_INIT_STAGE_17   = 17,   // csd_vm_init
  CSD_INIT_STAGE_18   = 18,   // csd_voice_debug_init
  CSD_INIT_STAGE_19   = 19,   // csd_alm_init
  CSD_INIT_STAGE_20   = 20,   // csd_ocmem_init
  CSD_INIT_STAGE_21   = 21,   // csd_vsm_init
  CSD_INIT_STAGE_MAX  = 22,   // MAX_FOR_ALL
}csd_init_senquence_stages_e;
#else
#define CSD_INIT_STAGE_0    0
#define CSD_INIT_STAGE_MAX  22  //(21+1)
#endif

/*! \fn uint32_t csd_deinit_stage(uint16_t nMaxInitStage)
    \brief This is a private function to handle De-Init() in sequence 
    de-initializes the Core Sound Driver.
    note calling function should be either csd_init() or csd_dinit() 
    that acquires the mutex lock
    \return int32_t
*/

uint32_t csd_deinit_stage(uint16_t nMaxInitStage)
{
  uint32_t rc             = CSD_EOK;
  uint16_t nDeInitStage   = CSD_INIT_STAGE_MAX;

  if( CSD_INIT_STAGE_MAX < nMaxInitStage )
  {
    CSD_MSG( CSD_PRIO_ERROR, "csd_deinit_stage: Input stage[%d] greater than MAX stage[%d]", nMaxInitStage, CSD_INIT_STAGE_MAX );
    /* Better continue with DeInit_All mode: revert once all the modules can handle the calls gracefully, till then return  */
    return rc; 
  }
  if( 0 == nMaxInitStage )
  {
    CSD_MSG( CSD_PRIO_ERROR, "csd_deinit_stage: Input stage[%d]: nothing to deinit", nMaxInitStage );
    return rc;
  }
  else
  {
    (nDeInitStage> 0)? (nDeInitStage--):(nDeInitStage);
    CSD_MSG( CSD_PRIO_LOW, "csd_deinit_stage: Start@deinit_Stage[%d]", nDeInitStage );
  }
  if( nDeInitStage <= nMaxInitStage )
  {
    rc = csd_vsm_deinit();
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,rc,nDeInitStage,0,0,0);	
    CSD_MSG( CSD_PRIO_MEDIUM, "csd_deinit_stage: stage[%d]: csd_vsm_deinit() completed, rc[0x%x]", nDeInitStage, rc );
  }
  (nDeInitStage> 0)? (nDeInitStage--):(nDeInitStage);
  if( nDeInitStage <= nMaxInitStage )
  {
    rc = csd_ocmem_dinit();
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,rc,nDeInitStage,0,0,0);		
    CSD_MSG( CSD_PRIO_MEDIUM, "csd_deinit_stage: stage[%d]: csd_ocmem_deinit() completed, rc[0x%x]", nDeInitStage, rc );
  }
  (nDeInitStage> 0)? (nDeInitStage--):(nDeInitStage);
  if( nDeInitStage <= nMaxInitStage )
  {
    rc = csd_alm_deinit();
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,rc,nDeInitStage,0,0,0);		
    CSD_MSG( CSD_PRIO_MEDIUM, "csd_deinit_stage: stage[%d]: csd_alm_deinit() completed, rc[0x%x]", nDeInitStage, rc );
  }
  (nDeInitStage> 0)? (nDeInitStage--):(nDeInitStage);
  if( nDeInitStage <= nMaxInitStage )
  {
    rc = csd_voice_debug_deinit();
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,rc,nDeInitStage,0,0,0);		
    CSD_MSG( CSD_PRIO_MEDIUM, "csd_deinit_stage: stage[%d]: csd_voice_debug_deinit() completed, rc[0x%x]", nDeInitStage, rc );
  }
  (nDeInitStage> 0)? (nDeInitStage--):(nDeInitStage);
  if(nDeInitStage <= nMaxInitStage)   
  {
    rc = csd_vm_deinit();
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,rc,nDeInitStage,0,0,0);		
    CSD_MSG( CSD_PRIO_MEDIUM, "csd_deinit_stage: stage[%d]: csd_vm_deinit() completed, rc[0x%x]", nDeInitStage, rc );
  }
  (nDeInitStage> 0)? (nDeInitStage--):(nDeInitStage);
  if( nDeInitStage <= nMaxInitStage )   
  {
    rc = csd_vc_deinit();
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,rc,nDeInitStage,0,0,0);		
    CSD_MSG( CSD_PRIO_MEDIUM, "csd_deinit_stage: stage[%d]: csd_vc_deinit() completed, rc[0x%x]",nDeInitStage, rc );
  }
  (nDeInitStage> 0)? (nDeInitStage--):(nDeInitStage);
  if(nDeInitStage <= nMaxInitStage)   
  {
    rc = csd_vs_deinit();
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,rc,nDeInitStage,0,0,0);			
    CSD_MSG( CSD_PRIO_MEDIUM, "csd_deinit_stage: stage[%d]: csd_vs_deinit() completed, rc[0x%x]", nDeInitStage, rc );
  }
  (nDeInitStage> 0)? (nDeInitStage--):(nDeInitStage);
  if( nDeInitStage <= nMaxInitStage ) 

  { /*TODO-Removed for Twizy */
    //rc = csd_voc_common_deinit(); 
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,rc,nDeInitStage,0,0,0);			
    CSD_MSG( CSD_PRIO_MEDIUM, "csd_deinit_stage: stage[%d]: csd_voc_common_deinit() completed, rc[0x%x]", nDeInitStage, rc );
  }
  (nDeInitStage> 0)? (nDeInitStage--):(nDeInitStage);
  if( nDeInitStage <= nMaxInitStage )   
  {
    rc = csd_ac_debug_dinit();
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,rc,nDeInitStage,0,0,0);			
    CSD_MSG( CSD_PRIO_MEDIUM, "csd_deinit_stage: stage[%d]: csd_ac_debug_dinit() completed, rc[0x%x]",nDeInitStage, rc );
  }
  (nDeInitStage> 0)? (nDeInitStage--):(nDeInitStage);
  if( nDeInitStage <= nMaxInitStage )   
  {
    rc = csd_ac_dinit();
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,rc,nDeInitStage,0,0,0);			
    CSD_MSG( CSD_PRIO_MEDIUM, "csd_deinit_stage: stage[%d]: csd_ac_dinit() completed, rc[0x%x]", nDeInitStage, rc );
  }
  (nDeInitStage> 0)? (nDeInitStage--):(nDeInitStage);
  if( nDeInitStage <= nMaxInitStage )   
  {
    rc = csd_as_debug_dinit();
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,rc,nDeInitStage,0,0,0);			
    CSD_MSG( CSD_PRIO_MEDIUM, "csd_deinit_stage: stage[%d]: csd_as_debug_dinit() completed, rc[0x%x]", nDeInitStage, rc );
  }
  (nDeInitStage> 0)? (nDeInitStage--):(nDeInitStage);
  if( nDeInitStage <= nMaxInitStage )   
  {
    rc = csd_as_dinit();
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,rc,nDeInitStage,0,0,0);			
    CSD_MSG( CSD_PRIO_MEDIUM, "csd_deinit_stage: stage[%d]: csd_as_dinit() completed, rc[0x%x]", nDeInitStage, rc );
  }
  (nDeInitStage> 0)? (nDeInitStage--):(nDeInitStage);
  if( nDeInitStage <= nMaxInitStage )   
  {
    rc = csd_ssr_deinit();
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,rc,nDeInitStage,0,0,0);			
    CSD_MSG( CSD_PRIO_MEDIUM, "csd_deinit_stage: stage[%d]: csd_ssr_deinit() completed, rc[0x%x]", nDeInitStage, rc );
  }
  (nDeInitStage> 0)? (nDeInitStage--):(nDeInitStage);
  if( nDeInitStage <= nMaxInitStage )   
  {
    rc = csd_dev_debug_dinit();
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,rc,nDeInitStage,0,0,0);			
    CSD_MSG( CSD_PRIO_MEDIUM, "csd_deinit_stage: stage[%d]: csd_dev_debug_dinit() completed, rc[0x%x]", nDeInitStage, rc );
  }
  (nDeInitStage> 0)? (nDeInitStage--):(nDeInitStage);
  if(nDeInitStage <= nMaxInitStage)   
  {
    rc = csd_dev_dinit();
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,rc,nDeInitStage,0,0,0);			
    CSD_MSG( CSD_PRIO_MEDIUM, "csd_deinit_stage: stage[%d]: csd_dev_dinit() completed, rc[0x%x]", nDeInitStage, rc );
  }
  (nDeInitStage> 0)? (nDeInitStage--):(nDeInitStage);
  if( nDeInitStage <= nMaxInitStage )  

  { /*TODO -  Removed for Twizy */
    //rc = csd_acdb_dinit(); 
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,rc,nDeInitStage,0,0,0);			
    CSD_MSG( CSD_PRIO_MEDIUM, "csd_deinit_stage: stage[%d]: csd_acdb_dinit() completed, rc[0x%x]", nDeInitStage, rc );
  }    
  (nDeInitStage> 0)? (nDeInitStage--):(nDeInitStage);
  if( nDeInitStage <= nMaxInitStage )   
  {
    rc = csd_atl_dinit();
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,rc,nDeInitStage,0,0,0);			
    CSD_MSG( CSD_PRIO_MEDIUM, "csd_deinit_stage: stage[%d]: csd_atl_dinit() completed, rc[0x%x]", nDeInitStage, rc );
  }
  (nDeInitStage> 0)? (nDeInitStage--):(nDeInitStage);
  if( nDeInitStage <= nMaxInitStage )   
  {
    rc = csd_rm_dinit();
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,rc,nDeInitStage,0,0,0);			
    CSD_MSG( CSD_PRIO_MEDIUM, "csd_deinit_stage: stage[%d]: csd_rm_dinit() completed, rc[0x%x]", nDeInitStage, rc );
  }
  (nDeInitStage> 0)? (nDeInitStage--):(nDeInitStage);
  if( nDeInitStage <= nMaxInitStage )   
  {
    rc = csd_handle_dinit();
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,rc,nDeInitStage,0,0,0);			
    CSD_MSG( CSD_PRIO_MEDIUM, "csd_deinit_stage: stage[%d]: csd_handle_dinit() completed, rc[0x%x]", nDeInitStage, rc );
  }
  (nDeInitStage> 0)? (nDeInitStage--):(nDeInitStage);
  if( nDeInitStage <= nMaxInitStage )   
  {
    rc = csd_pmem_deinit();
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,rc,nDeInitStage,0,0,0);			
    CSD_MSG( CSD_PRIO_MEDIUM, "csd_deinit_stage: stage[%d]: csd_pmem_deinit() completed, rc[0x%x]", nDeInitStage, rc );
  }
  (nDeInitStage> 0)? (nDeInitStage--):(nDeInitStage);
  if( 0 == nDeInitStage )
  {
    CSD_MSG( CSD_PRIO_LOW, "csd_deinit_stage: stage[%d]: nothing to deinit", nDeInitStage );
  }
  return rc;
}

/*! \fn uint32_t csd_init(void)
    \brief This function initializes the Sound Routing Driver. The second call only adds ref count
    ote When adding new module/s, 
        need to maintain the same ordering in init and deInit functions
        Account to the count of new module in the CSD_INIT_STAGE_MAX defn/enum.
    
    \return int32_t
*/
uint32_t csd_init(void)
{
  uint32_t rc             = CSD_EOK;
  uint32_t apr_rc         = APR_EOK;
  uint16_t nMaxInitStage  = CSD_INIT_STAGE_0;

  /* this critical section si created in dll attach so that 
   * we can safely use it now */
  csd_criticalsection_enter();
  if( csd_main_data.ref_count == CSD_INIT_OVERFLOW )
  {
    /* csd init reached max client numbers, send
    Success to client with out incrementing the reference
    count*/
    CSD_MSG( CSD_PRIO_ERROR, "csd_init: Max number of client[%d] registered", csd_main_data.ref_count );
    rc = CSD_ENORESOURCE;
    goto end;
  }

  if(++csd_main_data.ref_count > 1)
  {
    /* init already. no op */
	CSD_MSG( CSD_PRIO_HIGH, "csd_init(): Already initialized! ref_count=[%d]", csd_main_data.ref_count );
    goto end;
  }

  apr_rc = apr_init();

  if(apr_rc != APR_EOK)
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_init(): apr_init() failed. apr_rc=0x%x", apr_rc);
    rc = csd_map_apr_error_to_csd( apr_rc );
  }
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,apr_rc,CSD_APR_INIT_STAGE,0,0,0);

  /* invoke csd sub modules initilalization routines */
  /* init pmem */
  if(CSD_EOK == rc)
  {
    nMaxInitStage++;
    rc = csd_pmem_init();
  }
  /* Initialise the CSD Handle manager */
  if(CSD_EOK == rc)
  {
    nMaxInitStage++;
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,nMaxInitStage,0,0,0);		
    rc = csd_handle_init(CSD_HANDLE_SIZE);
  }
  /* Initialize the Resource Manager */
  if(CSD_EOK == rc)
  {
    nMaxInitStage++;
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,nMaxInitStage,0,0,0);		
    csd_rm_init(CSD_HANDLE_SIZE);
  }
  /* init atl first. If atl init failed no need to init rest */
  if(CSD_EOK == rc)
  {
    nMaxInitStage++;
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,nMaxInitStage,0,0,0);		
    rc = csd_atl_init();
  }
  /* init acdb */
  if(CSD_EOK == rc)
  {
    nMaxInitStage++;  
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,nMaxInitStage,0,0,0);
     /*TODO - Taking only one Initialization */		
    if(!acdb_init_count) 
    { rc = csd_acdb_init();
      acdb_init_count++ ;
    }
  }
  /* init audio device control route module */
  if(CSD_EOK == rc)
  {
    nMaxInitStage++;
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,nMaxInitStage,0,0,0);		
    rc = csd_dev_init(&csd_main_data.fn_tbl[CSD_OPEN_DEVICE_CONTROL]);
  }
  if(CSD_EOK == rc)
  {
    nMaxInitStage++;
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,nMaxInitStage,0,0,0);		
    rc = csd_dev_debug_init(&csd_main_data.fn_tbl[CSD_OPEN_DEBUG_DEVICE_CONTROL]);
  }
  /* Initialise the SSR registration to Policy manager */
  if(CSD_EOK == rc)
  {
    nMaxInitStage++;
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,nMaxInitStage,0,0,0);		
    csd_ssr_init();
  }
  /* init audio stream module */
  if(CSD_EOK == rc)
  {
    nMaxInitStage++;
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,nMaxInitStage,0,0,0);		
    rc = csd_as_init(&csd_main_data.fn_tbl[CSD_OPEN_AUDIO_STREAM]);
        if (CSD_EOK == rc)
        {
          csd_main_data.fn_tbl[CSD_OPEN_AUDIO_STREAM_V2] = csd_main_data.fn_tbl[CSD_OPEN_AUDIO_STREAM];
        }
  }
  if(CSD_EOK == rc)
  {
    nMaxInitStage++;
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,nMaxInitStage,0,0,0);		
    rc = csd_as_debug_init(&csd_main_data.fn_tbl[CSD_OPEN_DEBUG_AUDIO_STREAM_CONTROL]);
  }
  /* init audio context route module */
  if(CSD_EOK == rc)
  {
    nMaxInitStage++;
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,nMaxInitStage,0,0,0);		
    rc = csd_ac_init(&csd_main_data.fn_tbl[CSD_OPEN_AUDIO_CONTEXT]);
        if (CSD_EOK == rc)
        {
          csd_main_data.fn_tbl[CSD_OPEN_AUDIO_CONTEXT_V2] = csd_main_data.fn_tbl[CSD_OPEN_AUDIO_CONTEXT];
        }
  }
  if(CSD_EOK == rc)
  {
    nMaxInitStage++;
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,nMaxInitStage,0,0,0);		
    rc = csd_ac_debug_init(&csd_main_data.fn_tbl[CSD_OPEN_DEBUG_AUDIO_CONTEXT_CONTROL]);
  }
  /* init voice modules */
  if(CSD_EOK == rc)
  {
    nMaxInitStage++;
    /*TODO-Twizzy-Only One Initialization*/
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,nMaxInitStage,0,0,0);		
    if(!voc_init_count) 
    { rc = csd_voc_common_init();
      voc_init_count++ ;
    }
  }
  if(CSD_EOK == rc)
  {
    nMaxInitStage++;
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,nMaxInitStage,0,0,0);		
    rc = csd_vs_init(&csd_main_data.fn_tbl[CSD_OPEN_VOICE_STREAM]);
  }
  if(CSD_EOK == rc)
  {
    nMaxInitStage++;
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,nMaxInitStage,0,0,0);		
    rc = csd_vc_init(&csd_main_data.fn_tbl[CSD_OPEN_VOICE_CONTEXT]);
  }
  if(CSD_EOK == rc)
  {
    nMaxInitStage++;
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,nMaxInitStage,0,0,0);		
    rc = csd_vm_init(&csd_main_data.fn_tbl[CSD_OPEN_VOICE_MANAGER]);
  }
  if(CSD_EOK == rc)
  {
    nMaxInitStage++;
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,nMaxInitStage,0,0,0);		
    rc = csd_voice_debug_init(&csd_main_data.fn_tbl[CSD_OPEN_DEBUG_VOICE_CONTROL]);
  }
  if(CSD_EOK == rc)
  {
    nMaxInitStage++;
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,nMaxInitStage,0,0,0);		
    rc = csd_alm_init(&csd_main_data.fn_tbl[CSD_OPEN_AUDIO_LICENSE_MANAGER]);
  }
  if(CSD_EOK == rc)
  {
    nMaxInitStage++;
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,nMaxInitStage,0,0,0);		
    //rc = csd_ocmem_init();
    csd_ocmem_init();           // It is OK to continue, Even if it OCMEM errors out: Not a Must!
  }

  if(CSD_EOK == rc)
  {
    nMaxInitStage++;
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,nMaxInitStage,0,0,0);		
    rc = csd_vsm_init();
  }

  /* Upon Failure Clean-up the csd modules that were already initialized */
  if(CSD_EOK != rc)
  {
    uint32_t rc1 = CSD_EOK;

    CSD_MSG(CSD_PRIO_ERROR, "csd_init: Failed at stage[%d] Wtih Err=[0x%x]!! Continuing with DeInit ", nMaxInitStage, rc);
    /* set the CSD sub module failed*/
    if(nMaxInitStage)
    {
      //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,(nMaxInitStage+1),0,0,0);		
    }
    csd_main_data.ref_count = 0;
    rc1 = csd_deinit_stage(nMaxInitStage);
    CSD_MSG(CSD_PRIO_MEDIUM, "CSD:INIT: csd_deinit_Stage() Completed with rc=[0x%x] !!", rc1);
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,CSD_DEBUG_INIT_STAGE_MAX,0,0,0);			
  }
  else
  {
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,(nMaxInitStage+1),0,0,0);
    /* Set the CSD init ok*/
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,CSD_DEBUG_INIT_STAGE_MAX,0,0,0);			
  }
end:
  csd_criticalsection_leave();
  return rc;
}

/*! \fn uint32_t csd_deinit(void)
    \brief This function first reduce the ref count. If ref count reaches zero it 
    de-initializes the Sound Routing Driver. 
    note When adding new module/s, 
        need to maintain the same sequence in init and deInit functions
        Account to the count of new module in the CSD_INIT_STAGE_MAX defn/enum.
    \return int32_t
*/
uint32_t csd_deinit(void)
{
  uint32_t rc = CSD_EOK;
  uint32_t apr_rc = APR_EOK;
  if(csd_main_data.ref_count == 0) 
  {
    /*  no op */
    CSD_MSG(CSD_PRIO_ERROR, "CSD:DE_INIT <refCount=[0] => CSD Not Yet Initialized!!");
    return rc;
  }

  CSD_MSG(CSD_PRIO_LOW, "CSD:DE_INIT <Performance>start</Performance>.");
  csd_criticalsection_enter();
  //Count > 0
  if(csd_main_data.ref_count > 1)
  {
    CSD_MSG(CSD_PRIO_MEDIUM, "CSD:DE_INIT <refCount=[%d] => Active clients exist!! CSD Cannot be De-Initialized!!", csd_main_data.ref_count);
    csd_cleanup_session(csd_main_data.ref_count);
    csd_main_data.ref_count--;
  }
  else
  {
    /* if multiple threads are waiting at criticalsection_enter() with refCount=[1], we want to avoid decrementing below zero */
    if ( 1 == csd_main_data.ref_count )
    {
      csd_cleanup_session(csd_main_data.ref_count);
      csd_main_data.ref_count--;
      rc = csd_deinit_stage(CSD_INIT_STAGE_MAX);
      CSD_MSG(CSD_PRIO_MEDIUM, "CSD:DE_INIT Completed with rc=[0x%x] !!", rc);

      apr_rc = apr_deinit();

      if(apr_rc != APR_EOK)
      {
        CSD_MSG(CSD_PRIO_ERROR, "csd_deinit(): apr_deinit() failed. apr_rc=0x%x", rc);
        rc = csd_map_apr_error_to_csd( apr_rc );
      }
      //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,apr_rc,CSD_APR_INIT_STAGE,0,0,0);

      /* Set the De Init state    */
      //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,rc,CSD_DEBUG_INIT_STAGE_MAX,0,0,0);
    }
    else
    {
      CSD_MSG(CSD_PRIO_ERROR, "csd_deinit(): Trying to deinit when refCount=[0], multiple threads trying to deinit");
    }
  }
  csd_criticalsection_leave();
  CSD_MSG(CSD_PRIO_LOW, "CSD:DE_INIT <Performance>stop</Performance>.");
  return rc;
}

/*! \fn uint32_t csd_query_driver_version(void)
    \brief This function returns the CSD driver's version
    \return uint32_t
*/
uint32_t csd_query_driver_version(void)
{     
  CSD_MSG(CSD_PRIO_MEDIUM, "csd_query_driver_version: DRIVER VERSION: 0x%x", CSD_DRIVER_VER);
  return CSD_DRIVER_VER;
}

/*! \fn uint32_t csd_open(csd_open_code code, void* params, uint32_t size)
    \return uint32_t
*/
uint32_t csd_open(enum csd_open_code code, void* params, uint32_t size)
{
  uint32_t handle = 0;
  /* reject if invalid open code or if sub-system reset occured with associated open code */
  if( ( code != CSD_OPEN_NOT_USED ) &&
        ( code < CSD_OPEN_MAX_NUM ) &&
      ( FALSE == csd_handle_is_type_ssr( code ) ) )
  {
    if ( ( csd_main_data.fn_tbl[code] != NULL ) &&
         ( csd_main_data.fn_tbl[code]->open_fn != NULL ) )
    {
      csd_rm_process_csd_command(CSD_CMD_OPEN, 0, code, handle, CSD_RM_CMD_MODE_PREPROCESS);
      handle =  csd_main_data.fn_tbl[code]->open_fn(code,params, size);
      csd_rm_process_csd_command(CSD_CMD_OPEN, handle, code, ((handle>0)?(CSD_EOK):(CSD_EFAILED)), CSD_RM_CMD_MODE_POSTPROCESS);
    }
  }
  else
  {
    CSD_MSG( CSD_PRIO_ERROR, "csd_open: Cannot process csd_open due to SSR or invalid Open Code[0x%x] ", code );
  }
  return handle;
}

/*! \fn int32_t csd_close(uint32_t handle)
    \return int32_t
*/
uint32_t csd_close(uint32_t handle)
{
  int32_t rc = CSD_EOK;
  uint32_t type = csd_handle_get_type(handle);
  if( type == CSD_OPEN_NOT_USED || type >= CSD_OPEN_MAX_NUM )
  {
    CSD_MSG( CSD_PRIO_ERROR, "csd_close: Cannot process csd_close  on Handle=[0x%x] due to BadType[0x%x] ", handle, type );
    rc = CSD_EBADPARAM;
  }
  if( CSD_EOK == rc &&
    csd_main_data.fn_tbl[type] != NULL &&
    csd_main_data.fn_tbl[type]->close_fn != NULL ) 
  {
    csd_rm_process_csd_command(CSD_CMD_CLOSE, handle, type, (uint32_t)rc, CSD_RM_CMD_MODE_PREPROCESS);
    rc = csd_main_data.fn_tbl[type]->close_fn(handle);
    csd_rm_process_csd_command(CSD_CMD_CLOSE, handle, type, (uint32_t)rc, CSD_RM_CMD_MODE_POSTPROCESS);
  }
  else
  {
    rc = CSD_EBADPARAM;
  }
  return rc;
}

/*! \fn int32_t csd_write(uint32_t handle, void* params, uint32_t size)
    \return int32_t
*/
int32_t csd_write(uint32_t handle, void* params, uint32_t size)
{
  int32_t rc = -1;
  uint32_t type = csd_handle_get_type(handle);
  if( type == CSD_OPEN_NOT_USED || type >= CSD_OPEN_MAX_NUM || TRUE == csd_handle_is_ssr(handle) )
  {
    CSD_MSG( CSD_PRIO_ERROR, "csd_write: Cannot process csd_write due to SSR or invalid Open Code[0x%x] ", type );
    rc = -1;
  }
  else if( csd_main_data.fn_tbl[type] != NULL &&
           csd_main_data.fn_tbl[type]->write_fn != NULL )
  {
    CSD_MSG(CSD_PRIO_HIGH, "CSD:WRITE <Performance>start</Performance>.");
    rc = csd_main_data.fn_tbl[type]->write_fn(handle, params, size);
    CSD_MSG(CSD_PRIO_HIGH, "CSD:WRITE <Performance>stop</Performance>.");
  }
  return rc;
}
/*! \fn int32_t csd_read(uint32_t handle, void* params, uint32_t size)
    \return int32_t
*/
int32_t csd_read(uint32_t handle, void* params, uint32_t size)
{
  int32_t rc = -1;
  uint32_t type = csd_handle_get_type(handle);
  if( type == CSD_OPEN_NOT_USED || type >= CSD_OPEN_MAX_NUM || TRUE == csd_handle_is_ssr(handle) )
  {
    CSD_MSG( CSD_PRIO_ERROR, "csd_read: Cannot process csd_read due to SSR or invalid Open Code[0x%x] ", type );
    rc = -1;
  }
  else if( csd_main_data.fn_tbl[type] != NULL &&
           csd_main_data.fn_tbl[type]->read_fn != NULL )
  {
    CSD_MSG(CSD_PRIO_HIGH, "CSD:READ <Performance>start</Performance>.");
    rc = csd_main_data.fn_tbl[type]->read_fn(handle, params, size);
    CSD_MSG(CSD_PRIO_HIGH, "CSD:READ <Performance>stop</Performance>.");
  }
  return rc;
}

/*! \fn uint32_t csd_ioctl(uint32_t handle, uint32_t cmd, unsigned char* params, uint32_t size)
    \brief IOCTL command
    \return int32_t
*/
uint32_t csd_ioctl(uint32_t handle, uint32_t cmd, void* params, uint32_t size)
{
  int32_t rc = CSD_EBADPARAM;
  uint32_t type = csd_handle_get_type(handle);
  if( type == CSD_OPEN_NOT_USED || type >= CSD_OPEN_MAX_NUM )
  {
      CSD_MSG( CSD_PRIO_ERROR, "csd_ioctl: Cannot process csd_ioctl[0%x] on Handle[0x%x] of Invaid Type[0x%x] ", cmd, handle, type );
      rc = CSD_EBADPARAM;
  }
  else if( (TRUE == csd_handle_is_ssr(handle)) && 
    (FALSE == csd_ioctl_allow_status_in_ssr(cmd)) )
  {                                                                        
      CSD_MSG( CSD_PRIO_ERROR, "csd_ioctl: Cannot process csd_ioctl[0x%x] due to SSR  ", cmd );
      rc = CSD_ESSRESET;
  }
  else if( csd_main_data.fn_tbl[type] != NULL &&
           csd_main_data.fn_tbl[type]->ioctl_fn != NULL ) 
  {
      csd_rm_process_csd_command(CSD_CMD_IOCTL, handle, cmd, CSD_EOK, CSD_RM_CMD_MODE_PREPROCESS);
      rc = csd_main_data.fn_tbl[type]->ioctl_fn(handle, cmd, params, size);
      csd_rm_process_csd_command(CSD_CMD_IOCTL, handle, cmd, (uint32_t)rc, CSD_RM_CMD_MODE_POSTPROCESS);
  }
  return rc;
}


/* ==============================================================================*/
void csd_pmic_gpiotwo_on(void)
{
     pm_err_flag_type return_val ;
     return_val = pm_gpio_set_current_source_pulls(0x0, PM_GPIO_2 ,PM_GPIO_I_SOURCE_PULL_UP_30uA);
     CSD_MSG(CSD_PRIO_ERROR,"PMIC GPIO Pull Up API Called , returned with val = %d",return_val);
     CSD_SLEEP(10);
}
/*=============================================================================*/

void csd_pmic_gpiotwo_off(void)
{  
     pm_err_flag_type return_val ;
     return_val = pm_gpio_set_current_source_pulls(0x0, PM_GPIO_2 ,PM_GPIO_I_SOURCE_PULL_DOWN_10uA);
     if(return_val) 
     {
       CSD_MSG(CSD_PRIO_LOW,"PMIC GPIO Pull Down returned failed with  val = %d",return_val);
     }
     else 
     {
       CSD_MSG(CSD_PRIO_LOW,"PMIC GPIO Pull Down success val = %d",return_val);
     }     

}
/*============================================================================= */
