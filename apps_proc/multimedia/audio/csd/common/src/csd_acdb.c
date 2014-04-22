
/*============================================================================

   FILE:       csd_acdb.c

   DESCRIPTION: Contains Core Sound Driver's dispatcher.

   PUBLIC CLASSES:  Not Applicable

   INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright © 2010 - 2012, 2016, 2017 QUALCOMM Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
 ===========================================================================*/

/*===========================================================================
                      EDIT HISTORY FOR MODULE

This section contains comments describing changes made to this module.
Notice that changes are listed in reverse chronological order.



when       who      what, where, why
--------   ---      ----------------------------------------------------------
03/13/17   vk       Fix QACT crash issue
11/15/16   rk       Abstracting OSAL layer
09/01/14   rp       enhanced the csd init debug buffer 
02/06/14   vk       modified code for proper csd/acdb/apr error mapping
07/23/13   vk       Added entry and exit logs for acdb_ioctl() processing
07/03/13   vk       Changes to remove compiler warnings
06/04/13   unni     Using heap for AcdbInitCmdType variable due to large size

============================================================================*/

#include "mmdefs.h"  
#include "csd_os_dependencies.h"
#include "csd_debug_msg.h"    
#include "csd.h"    
#include "csd_intf.h"
#include "csd_handle.h"
#include "acdb.h"
#include "acph.h"
#include "csd_debug_info.h"
#include "acph_rtc_dsp.h"
#include "acph_rtc_adie.h"
#include "acph_rtc_fts.h"
#include "acph_rtc_mcs.h"
#include "csd_error_map.h"
#include "flash_ftl.h"
#include "qurt_memory.h"
#include "qurt_error.h"


qurt_mem_region_t  acdb_mem_region;
qurt_mem_region_attr_t  acdb_mem_region_attr;
qurt_mem_pool_t  acdb_qurt_pool_id;

int32_t csd_acdb_map_memory_region( void )
{
  int rc = QURT_EOK;
  int32_t rc_ftl_open = 0 ;
  int32_t rc_ftl_read = 0;
  int32_t rc_ftl_close = 0;

  flash_ftl_client_t acdb_handle;
  
  qurt_mem_region_attr_init(&acdb_mem_region_attr);
  acdb_mem_region_attr.mapping_type = QURT_MEM_MAPPING_PHYS_CONTIGUOUS;
  acdb_mem_region_attr.physaddr = (qurt_addr_t)IMAGE_KEY_ACDB_IMG_DEST_ADDR;
   
  

  rc = qurt_mem_pool_attach("STATIC_POOL", &acdb_qurt_pool_id);
  if (rc != QURT_EOK)
  {
    CSD_MSG( CSD_PRIO_FATAL, "csd_acdb_map_memory_region: ACDB qurt_mem_pool_attach failed! rc=%d.", rc );
    return CSD_EFAILED;
  }

  rc = qurt_mem_region_create(&acdb_mem_region, (qurt_size_t)IMAGE_KEY_ACDB_SIZE,
                               acdb_qurt_pool_id, &acdb_mem_region_attr);
  if (rc != QURT_EOK)
  {
    CSD_MSG( CSD_PRIO_FATAL,"csd_acdb_map_memory_region: ACDB mem_region create failed! paddr=%x, "
              "size=%x, rc = %d", acdb_mem_region_attr.physaddr, IMAGE_KEY_ACDB_SIZE, rc );
    return CSD_EFAILED;
  }

  /* Reretrieve attributes struct since qurt_mem_region_create may have altered
   * the base addresses or size to fit page alignment (hopefully alignment is 
   * unnecessary). */
  rc = qurt_mem_region_attr_get( acdb_mem_region, &acdb_mem_region_attr );
  if (rc != QURT_EOK)
  {
    CSD_MSG( CSD_PRIO_FATAL, "csd_acdb_map_memory_region: ACDB qurt_mem_pool_attach failed! rc=%d.", rc );
    return CSD_EFAILED;
  }
   
  rc_ftl_open = flash_ftl_open(&acdb_handle, "acdb");
  if(rc_ftl_open != 0)
  {
    CSD_MSG( CSD_PRIO_FATAL, "csd_acdb_map_memory_region: ftl_open failed with rc=%d.", rc_ftl_open );
    return CSD_EFAILED;
  }

  rc_ftl_read = flash_ftl_read_lpa(acdb_handle, 0, 64, (uint8*)(acdb_mem_region_attr.virtaddr)); 
  if(rc_ftl_read != 0)
  {
    CSD_MSG( CSD_PRIO_FATAL, "csd_acdb_map_memory_region: ftl_read failed with rc=%d.", rc_ftl_read );
    return CSD_EFAILED;
  }

  rc_ftl_close = flash_ftl_close(&acdb_handle);
  if(rc_ftl_close != 0)
  {
    CSD_MSG( CSD_PRIO_FATAL, "csd_acdb_map_memory_region: ftl_close failed with rc=%d.", rc_ftl_close );
    return CSD_EFAILED;
  }

  return CSD_EOK;
}
  


int32_t csd_acdb_init()
{
  struct csd_acdb_files *pacdb_files = NULL;
  void *pacdb_files_handle = NULL;
  AcdbInitCmdType* cmdInit = NULL;
  int32_t rc = CSD_EOK;
  int32_t acdb_rc = ACDB_SUCCESS;
  size_t size_check = 0;

  if ( CSD_EOK != csd_acdb_map_memory_region() )
  {
    return CSD_EFAILED;
  }
  size_check = sizeof(AcdbInitCmdType);
  csd_malloc(sizeof(AcdbInitCmdType),(void**)&cmdInit);
  if (cmdInit == NULL )
  {
    CSD_MSG( CSD_PRIO_ERROR, "csd_acdb_init: Unable to allocate memory of size [%d]", size_check );
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,CSD_ENORESOURCE,CSD_ACDB_INIT_STAGE,
                    //   CSD_INIT_INFO_ACDB_DAL_MALLOC,0,0); 
    return CSD_ENORESOURCE;
  }

  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,CSD_ACDB_INIT_STAGE,
                    // CSD_INIT_INFO_ACDB_DAL_MALLOC,0,0); 

  csd_mem_alloc( sizeof(struct csd_acdb_files), &pacdb_files_handle, (uint8_t **)&pacdb_files );
  if( pacdb_files == NULL )
  {
    csd_free(cmdInit);
    CSD_MSG( CSD_PRIO_ERROR, "csd_acdb_init: Unable to allocate memory" );
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,CSD_ENORESOURCE,CSD_ACDB_INIT_STAGE,
                      // CSD_INIT_INFO_ACDB_CSD_MEM_MALLOC,0,0); 
    return CSD_ENORESOURCE;
  }

  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,CSD_ACDB_INIT_STAGE,
                     //CSD_INIT_INFO_ACDB_CSD_MEM_MALLOC,0,0); 
  rc = csd_query_acdb_files( pacdb_files );
  if( rc != CSD_EOK )
  {
    CSD_MSG( CSD_PRIO_ERROR, "csd_acdb_init: Unable to query ACDB file, rc[0x%x]", rc );
  }
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,CSD_ACDB_INIT_STAGE,
                      // CSD_INIT_INFO_ACDB_QUERY_ACDB_FILE,0,0); 

  if( rc == CSD_EOK )
  {
    uint16_t i = 0;
    cmdInit->nNoOfFiles = pacdb_files->num_files;
    for( i = 0; i < pacdb_files->num_files; i++ )
    {
      cmdInit->acdbFiles[i].fileNameLen = pacdb_files->file_list[i].filenamelen;
      csd_memscpy( cmdInit->acdbFiles[i].fileName, cmdInit->acdbFiles[i].fileNameLen,
                   pacdb_files->file_list[i].filename, pacdb_files->file_list[i].filenamelen );
    }
  }

  csd_mem_free( pacdb_files_handle );

  if( rc == CSD_EOK )
  {
    CSD_MSG( CSD_PRIO_MEDIUM, "csd_acdb_init: Invoking ACDB_CMD_INITIALIZE_V2, NoOfFiles[%d]",
    cmdInit->nNoOfFiles );
    acdb_rc = acdb_ioctl( ACDB_CMD_INITIALIZE_V2, (uint8_t*)cmdInit, sizeof(AcdbInitCmdType), NULL, 0 );
    if( acdb_rc != ACDB_SUCCESS )
    {
      CSD_MSG( CSD_PRIO_ERROR, "csd_acdb_init: ACDB_CMD_INITIALIZE_V2 failed, acdb_rc[%d]",acdb_rc );
      rc = csd_map_acdb_error_to_csd( acdb_rc ); 
    }
    else
    {
      CSD_MSG( CSD_PRIO_MEDIUM, "csd_acdb_init: ACDB_CMD_INITIALIZE_V2 success" );
    }
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,acdb_rc,CSD_ACDB_INIT_STAGE,
                      // CSD_INIT_INFO_ACDB_IOCTL_INITIALIZE,0,0); 

  }
   
  if( rc == CSD_EOK )
  {
    acdb_rc = acph_init();
    if( ACPH_SUCCESS == acdb_rc  )
    {
        CSD_MSG( CSD_PRIO_MEDIUM, "csd_acdb_init: acph_init success" );
    }
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,acdb_rc,CSD_ACDB_INIT_STAGE,
             //CSD_INIT_INFO_ACDB_ACPH,0,0); 

    acdb_rc = acph_rtc_dsp_init();
    if( ACPH_SUCCESS == acdb_rc  )
    {
      CSD_MSG( CSD_PRIO_MEDIUM, "csd_acdb_init: acph_rtc_dsp_init success" );
    }
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,acdb_rc,CSD_ACDB_INIT_STAGE,
                      //CSD_INIT_INFO_ACDB_ACPH_RTC_DSP,0,0); 

    acdb_rc = acph_rtc_adie_init();
    if( ACPH_SUCCESS == acdb_rc )
    {
      CSD_MSG( CSD_PRIO_MEDIUM, "csd_acdb_init: acph_rtc_adie_init success" );
    }
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,acdb_rc,CSD_ACDB_INIT_STAGE,
                      //CSD_INIT_INFO_ACDB_ACPH_RTC_ADIE,0,0); 

    acdb_rc = acph_rtc_fts_init();
    if( ACPH_SUCCESS == acdb_rc )
    {
      CSD_MSG( CSD_PRIO_MEDIUM, "csd_acdb_init: acph_rtc_fts_init success" );
    }
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,acdb_rc,CSD_ACDB_INIT_STAGE,
                         //CSD_INIT_INFO_ACDB_ACPH_RTC_FTS,0,0); 

    acdb_rc = acph_rtc_mcs_init();
    if( ACPH_SUCCESS == acdb_rc  )
    {
      CSD_MSG( CSD_PRIO_MEDIUM, "csd_acdb_init: acph_rtc_mcs_init success" );
    }
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,acdb_rc,CSD_ACDB_INIT_STAGE,
                      //CSD_INIT_INFO_ACDB_ACPH_RTC_MCS,0,0); 
  }

  csd_free(cmdInit);

  return rc;
}

int32_t csd_acdb_dinit(void)
{
  int32_t rc = CSD_EOK;
  int32_t acdb_rc = ACDB_SUCCESS;
   
  acdb_rc = acph_rtc_mcs_deinit();
  if( acdb_rc != ACDB_SUCCESS )
  {
    CSD_MSG( CSD_PRIO_ERROR, "csd_acdb_dinit: acph_rtc_mcs_deinit() failed, acdb_rc[%d]",acdb_rc );
  }
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,acdb_rc,CSD_ACDB_INIT_STAGE,
                    //CSD_INIT_INFO_ACDB_ACPH_RTC_MCS,0,0); 

  acdb_rc = acph_rtc_fts_deinit();
  if( acdb_rc != ACDB_SUCCESS )
  {
    CSD_MSG( CSD_PRIO_ERROR, "csd_acdb_dinit: acph_rtc_fts_deinit()failed, acdb_rc[%d]",acdb_rc );
  }
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,acdb_rc,CSD_ACDB_INIT_STAGE,
                    //CSD_INIT_INFO_ACDB_ACPH_RTC_FTS,0,0); 

  CSD_MSG( CSD_PRIO_MEDIUM, "csd_acdb_dinit: Invoking ACDB_CMD_RESET" );

  /* perform ACDB deinit */
  acdb_rc = acdb_ioctl( ACDB_CMD_RESET, NULL, 0, NULL, 0 );
  if( acdb_rc != ACDB_SUCCESS )
  {
    CSD_MSG( CSD_PRIO_ERROR, "csd_acdb_dinit: ACDB_CMD_RESET failed, acdb_rc[%d]",acdb_rc );
    rc = csd_map_acdb_error_to_csd( acdb_rc );
  }
  else
  {
    CSD_MSG( CSD_PRIO_MEDIUM, "csd_acdb_dinit: ACDB_CMD_RESET success" );
  }
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,acdb_rc,CSD_ACDB_INIT_STAGE,
                  //CSD_INIT_INFO_ACDB_RESET,0,0); 

  return rc; 
}
