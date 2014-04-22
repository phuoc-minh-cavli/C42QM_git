/*=============================================================================

FILE:         qdss_etm_arm.c

DESCRIPTION:

================================================================================
              Copyright (c) 2013 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
==============================================================================*/
#include "DALFramework.h"
#include "HALcomdef.h"
#include "qdss_etm_utils.h"
#include "halqdss_etm_arm.h"
#include "qdss_etm_config.h"
#include "qdss_throw.h"


#define QDSS_ETM_EVENT_ALWAYS_TRUE  0x406F

struct qdss_etm_struct {
   boolean         bInitialized;
   DALSYSMemHandle mem_handle;
   QDSSMemInfo   mem_info;
   uint8          version;
   uint8          num_addr_comparators;
   uint8          num_counters;
   uint8          num_ctxtid_comparators;
   uint8          num_data_comparators;
   uint8          num_external_inputs;
   uint8          num_external_outputs;
   uint32         control;
   boolean        bDataTraceSupported;
};

struct qdss_etm_struct qdss_etm;

#define QDSS_ETM_VERSION_V1_0    (0x00)
#define QDSS_ETM_VERSION_V1_2    (0x02)
#define QDSS_ETM_VERSION_V3_3    (0x23)
#define QDSS_ETM_VERSION_V3_5    (0x25)
#define QDSS_PFT_VERSION_MAJOR      (0x30)
#define QDSS_PFT_VERSION_V1_1    (0x31)



static boolean qdss_etm_version_check(uint8 version, uint8 atleast)
{
   if ( QDSS_PFT_VERSION_MAJOR == (version & QDSS_PFT_VERSION_MAJOR) ) {
      return FALSE;
   }
   return (version >= atleast);
}



static void qdss_etm_init_defaults(void)
{
   uint32 etm_config_reg;
   uint32 etm_ctl_reg;

   HAL_qdss_etmarm_AccessUnLock();
   HAL_qdss_etm_set_power_up();
   HAL_qdss_etm_clear_power_down();
   HAL_qdss_etm_clear_power_up();
   HAL_qdss_etm_set_prog();

   etm_config_reg = HAL_qdss_etmarm_Get_CCR();
   qdss_etm.num_addr_comparators = bit_field_value(etm_config_reg, 0, 3) * 2;
   qdss_etm.num_counters = bit_field_value(etm_config_reg, 13, 15);
   qdss_etm.num_external_inputs = bit_field_value(etm_config_reg, 17, 19);
   qdss_etm.num_external_outputs = bit_field_value(etm_config_reg, 20, 22);
   qdss_etm.num_ctxtid_comparators = bit_field_value(etm_config_reg, 24, 25);
   qdss_etm.num_data_comparators = bit_field_value(etm_config_reg, 4, 7);

   qdss_etm.version = bit_field_value( HAL_qdss_etmarm_Get_IDR(), 4, 11);

   qdss_etm.bDataTraceSupported = FALSE;
   if (qdss_etm_version_check(qdss_etm.version, QDSS_ETM_VERSION_V1_0)) {
      etm_ctl_reg = HAL_qdss_etmarm_Get_CR();
      etm_ctl_reg |= (bit(2) | bit(3));
      HAL_qdss_etmarm_Set_CR(etm_ctl_reg);
      etm_ctl_reg = HAL_qdss_etmarm_Get_CR();
      if (bit_value(etm_ctl_reg, 2) || bit_value(etm_ctl_reg, 3)) {
         qdss_etm.bDataTraceSupported = TRUE;
      }
   }

   qdss_etm.control = 0x0;
   if (qdss_etm_version_check(qdss_etm.version, QDSS_ETM_VERSION_V1_0)) {
      qdss_etm.control |= bit(11);
   }

   if (qdss_etm.bDataTraceSupported == TRUE) {
      qdss_etm.control |= bit(2) | bit(3);
   }

   HAL_qdss_etm_set_power_down();
   HAL_qdss_etmarm_AccessLock();
}

#define QDSS_ETM_CFG_SIZE      4096

static void qdss_etm_init(void)
{
   int nErr = 0;

   if (qdss_etm.bInitialized) {
      //already initialized
      return;
   }


   TRY(nErr,qdss_dal_memmap(QDSS_ETM_ARM_BASE_PHYS,
                            QDSS_ETM_CFG_SIZE,
                            0,
                            &qdss_etm.mem_handle,
                            &qdss_etm.mem_info));

   HAL_qdss_etmarm_HalConfigInit(qdss_etm.mem_info.VirtualAddr);

   qdss_etm_init_defaults();

   qdss_etm.bInitialized  =  TRUE;

   CATCH(nErr) {}
   return;
}

#define QDSS_APPS_ETM_ATID  64

/**
  @brief  Enables apps core ETM tracing with default settings
          By default all code is traced.
 */

void qdss_etm_enable(void)
{
   int i;
   uint32 etm_ctl_reg;

   qdss_etm_init(); //init if not initialized

   HAL_qdss_etmarm_AccessUnLock();
   HAL_qdss_etm_set_power_up();
   HAL_qdss_etm_clear_power_down();
   HAL_qdss_etm_clear_power_up();
   HAL_qdss_etm_set_prog();


   etm_ctl_reg = HAL_qdss_etmarm_Get_CR();
   etm_ctl_reg &= (bit(10) | bit(0));
   HAL_qdss_etmarm_Set_CR(qdss_etm.control | etm_ctl_reg);

   HAL_qdss_etmarm_Set_TRACEIDR(QDSS_APPS_ETM_ATID);

   HAL_qdss_etmarm_Set_TRIGGER(QDSS_ETM_EVENT_ALWAYS_TRUE);
   HAL_qdss_etmarm_Set_TSSCR(0x0);
   if (qdss_etm_version_check(qdss_etm.version, QDSS_ETM_VERSION_V1_2)) {
      HAL_qdss_etmarm_Set_TECR2(0x0);
   }
   HAL_qdss_etmarm_Set_TEEVR(0x6F);
   HAL_qdss_etmarm_Set_TECR1( 0x1000000);
   HAL_qdss_etmarm_Set_FFLR(0x30);

   if (qdss_etm.bDataTraceSupported == TRUE) {
      HAL_qdss_etmarm_Set_VDEVR(0x6F);
      HAL_qdss_etmarm_Set_VDCR1(0x0);
      HAL_qdss_etmarm_Set_VDCR3(0x10000);
   }

   for (i = 0; i < qdss_etm.num_addr_comparators; i++) {
      HAL_qdss_etmarm_Set_ACVRn(i, 0x0);
      HAL_qdss_etmarm_Set_ACTRn(i, 0x0);
   }
   for (i = 0; i < qdss_etm.num_data_comparators; i++) {
      HAL_qdss_etmarm_Set_DCVRn(i, 0x0);
      HAL_qdss_etmarm_Set_DCMRn(i, ~(0x0));
   }
   for (i = 0; i < qdss_etm.num_counters; i++) {
      HAL_qdss_etmarm_Set_CNTRLDVRn(i, 0x0);
      HAL_qdss_etmarm_Set_CNTENRn(i,QDSS_ETM_EVENT_ALWAYS_TRUE);
      HAL_qdss_etmarm_Set_CNTRLDVRn(i,QDSS_ETM_EVENT_ALWAYS_TRUE);
      HAL_qdss_etmarm_Set_CNTVRn(i, 0x0);
   }
   HAL_qdss_etmarm_Set_SQ12EVR(QDSS_ETM_EVENT_ALWAYS_TRUE);
   HAL_qdss_etmarm_Set_SQ21EVR(QDSS_ETM_EVENT_ALWAYS_TRUE);
   HAL_qdss_etmarm_Set_SQ23EVR(QDSS_ETM_EVENT_ALWAYS_TRUE);
   HAL_qdss_etmarm_Set_SQ31EVR(QDSS_ETM_EVENT_ALWAYS_TRUE);
   HAL_qdss_etmarm_Set_SQ32EVR(QDSS_ETM_EVENT_ALWAYS_TRUE);
   HAL_qdss_etmarm_Set_SQ13EVR(QDSS_ETM_EVENT_ALWAYS_TRUE);
   HAL_qdss_etmarm_Set_SQR(0x0);
   for (i = 0; i < qdss_etm.num_external_outputs; i++) {
      HAL_qdss_etmarm_Set_EXTOUTEVRn(i,QDSS_ETM_EVENT_ALWAYS_TRUE);
   }
   for (i = 0; i < qdss_etm.num_ctxtid_comparators; i++) {
      HAL_qdss_etmarm_Set_CIDCVRn(i,0x0);
   }
   HAL_qdss_etmarm_Set_CIDCMR(0x0);
   HAL_qdss_etmarm_Set_SYNCFR(0x80);
   HAL_qdss_etmarm_Set_EXTINSELR(0x00000000);
   HAL_qdss_etmarm_Set_TSEVR(QDSS_ETM_EVENT_ALWAYS_TRUE);
   HAL_qdss_etmarm_Set_AUXCR(0x0);
   HAL_qdss_etmarm_Set_VMIDCVR(0x0);

   HAL_qdss_etm_clear_prog();
   HAL_qdss_etmarm_AccessLock();

}

/**
  @brief  Disables APPS core etm tracing
 */
void qdss_etm_disable(void)
{
   if (!qdss_etm.bInitialized) {
      //not initialized or enabled return;
      return;
   }

   HAL_qdss_etmarm_AccessUnLock();
   HAL_qdss_etm_set_prog();
   HAL_qdss_etmarm_Set_TEEVR(0x6F | bit(14));
   HAL_qdss_etm_set_power_down();
   HAL_qdss_etmarm_AccessLock();
}

