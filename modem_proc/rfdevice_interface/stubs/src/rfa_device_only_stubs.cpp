/*!
   @file
   rfa_device_only_stubs.cpp

   @brief

*/

/*===========================================================================

  Copyright (c) 2015 - 2018 Qualcomm Technologies, Inc. All Rights Reserved

  Qualcomm Technologies Proprietary and Confidential.

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.


                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //components/rel/rfdevice_interface.mpss/1.40/stubs/src/rfa_device_only_stubs.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------------
10/26/18   up      Mainlined SDR105 OFT support
09/23/15   px      Updated rfhal rffe execute stub for OFT compilation
08/27/15   hj      Added LTE stubs to fix compilation
08/11/15   avi     Added empty stubs for sbi_simulator api
07/20/15   avi     Added Apis to compile trx 
07/12/15   avi     Initial version

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "rfcommon_locks.h"
#include "rfcommon_efs.h" 
#include "rfm.h"
#include "rfgsm_nv_cmn.h"
#include "rfm_device_config_type.h"
#include "rfc_class.h"
#include "DALSys.h"

#include "rfcommon_core.h"
#include "rfcommon_time_profile.h"
#include "rflte_ext_mc.h"
#include "SbiSimulator.h"
#include "rflte_ext_mc.h"
extern "C"
{
#include "ftm_gsm.h"
#include "rf_hal_rffe.h"
}

#include "amssassert.h"
#include "rfc_bsp.h"
#include "DDITlmm.h"
#include "rfd_grfc.h"
#if 0
#include "rfc_card_wcdma.h"
#include "rf_tdscdma_core_util.h"
#include "rf_tdscdma_mc.h"
#include "rfc_card_tdscdma.h"
#include "rfc_tdscdma.h"

#include "rf_tdscdma.h"
#include "rfnv_tdscdma.h"
#include "rf_tdscdma_core_txlin.h"
#include "rf_tdscdma_core_txplim.h"
#include "rfc_card_tdscdma.h"
#endif
#include "rfc_common.h"
#include "rfcommon_core.h"


boolean is_sawless_enabled = FALSE;
boolean prx_port_is_sawless = FALSE;
uint8 device_process[2] = {0,0};
rfgsm_card_type rfgsm_card;

/***************************** COMMON ***********************************************************/


boolean
rf_common_init_critical_section
(
  rf_lock_data_type *crit_section
)
{
  return TRUE;
}




boolean
rf_common_deinit_critical_section
(
  rf_lock_data_type *crit_section
)
{
  return TRUE;
}



void
rf_common_enter_critical_section
(
  rf_lock_data_type *crit_section
)
{
} 



void
rf_common_leave_critical_section
(
  rf_lock_data_type *crit_section
)
{
} 

#if 0

boolean rfcommon_efs_put(const char *rfc_efs_file_name, void *data, int32 data_max_size, boolean append_flag)
{
  return TRUE;

}


int32 rfcommon_efs_get(const char *file_name, void *data, int32 data_max_size)
{
 
  return 1;

}


int32 rfcommon_efs_get_size( const char *file_name, 
                             rfcommon_efs_fstat_type *fs_buf )
{
  
  return 1;

}
#endif


boolean
rfm_get_calibration_state
(
  void
)
{

  return FALSE;

}



uint16 rfgsm_nv_get_spare(void)
{
  return 0x0000FFFF;
}





int32 ftm_gsm_get_tx_frequency_offset( void  )
{
 
    return 0;
}







boolean
rfm_get_devices_configuration
(
  rfm_devices_configuration_type *dev_cfg
)
{
  return TRUE;
}


boolean
rfm_inform_device_allocation
(
  rfm_device_allocation_type *dev_allocation
)
{
    return TRUE;
}



rfm_device_allocation_status_type* rfm_verify_rf_concurrency
(
  rfm_device_client_info_type *dev_client
)
{
    return NULL;
}






system_clock_enum rfc_card_get_sys_clk_type(void)
{

  return SYS_CLK_XO;
}




rf_hal_bus_result_type rfhal_rffe_read_byte
(
  uint8 channel,
  uint8 slave_id,
  uint16 reg,
  uint8* data,
  rf_hal_bus_resource_script_settings_type* settings
)
{
  return RF_HAL_BUS_SUCCESS;
}



rf_hal_bus_result_type rfhal_rffe_write_byte
(
  uint8 channel,
  uint8 slave_id,
  uint16 reg,
  uint8 data,
  rf_hal_bus_resource_script_settings_type* settings
)
{
  return RF_HAL_BUS_SUCCESS;
}



rf_hal_bus_result_type rfhal_rffe_execute_rf_buf_ext
(
  rf_buffer_intf* rf_buff, 
  rf_hal_bus_rffe_type* rffe_return_script,
  uint8 *return_read_size,
  rf_hal_bus_resource_script_settings_type* settings,
  uint8 max_read_size,
  boolean async_mode
)
{
  return RF_HAL_BUS_SUCCESS;
}



rf_hal_bus_result_type rfhal_rffe_write_bytes_ext 
(
  rf_hal_bus_rffe_type* script_ptr,
  uint8 channel,
  uint32 count,
  rf_hal_bus_resource_script_settings_type* settings
)
{
	return RF_HAL_BUS_SUCCESS;
}

rf_hal_bus_result_type rfhal_rffe_write_cmn
(
 rf_hal_bus_rffe_type* script_ptr,
 uint8 channel,
 boolean extended_cmd,
 uint32 count,
 rf_hal_bus_resource_script_settings_type* settings
 )
{
  return RF_HAL_BUS_SUCCESS;
}



void DALSYS_BusyWait(uint32 pause_time_us)
{
  return;
}



rf_hal_bus_result_type rfhal_rffe_read_bytes_ext
(
  rf_hal_bus_rffe_type* script_ptr,
  uint8 channel,
  uint32 count,
  rf_hal_bus_resource_script_settings_type* settings
)
{
  return RF_HAL_BUS_SUCCESS;
}

boolean rf_hal_rffe_reg_read_extln4
(
  uint8 channel, 
  uint8 slave_id, 
  boolean rd_delay, 
  boolean half_rate, 
  uint8 num_bytes, 
  uint16 addr, 
  uint32 *data
)
{
  return TRUE;
}



rf_time_type
rf_time_get_elapsed
(
  rf_time_tick_type start,
  rf_time_unit_type unit
)
{

  return 0;

} /* rf_time_get_elapsed */



rf_time_tick_type
rf_time_get_tick
(
  void
)
{
  rf_time_tick_type rf_timetick;

  rf_timetick.fine_timetick = 0;
  rf_timetick.sratch_pad = 0;
  return rf_timetick;

} /* rf_time_get_tick */


/****************************************************************************************************/


#if 0
void** rfc_wcdma_get_device
(
  rfm_device_enum_type rfm_device, 
  rfc_rxtx_enum_type rx_tx,
  rfcom_wcdma_band_type band, 
  rfdevice_type_enum_type dev_type
)
{
  
  return NULL;
}


uint8 rf_tdscdma_core_util_get_pa_range_map_nv_index(rfcom_tdscdma_band_type band, uint8 pa_range)
{
  return 0;
}
#endif

rf_path_enum_type rfcommon_core_device_to_path( rfcom_device_enum_type device )
{
  rf_path_enum_type temp_path;
  /* Hard coded for bring-up. Needs to be fixed.*/

  switch (device)
  {
    case RFM_DEVICE_0:
    case RFM_DEVICE_2:
    case RFM_DEVICE_4:
      temp_path = RF_PATH_0;
      break;

    case RFM_DEVICE_1:
    case RFM_DEVICE_3:
    case RFM_DEVICE_5:
      temp_path = RF_PATH_1;
      break;

    default :
      RF_MSG_1(RF_ERROR, "Invalid device: no path found for Device: %d",device);
      temp_path = RF_PATH_1;
      break;
  }

  return temp_path;
}

extern "C"
void
rf_ic_task
(
  dword parm
)
{
return ;
}


#if 0
boolean rf_tdscdma_mc_is_rx_sawless(void)
{
  boolean status = FALSE;
  status = prx_port_is_sawless;
  is_sawless_enabled = status;

  MSG_1(MSG_SSID_RF, MSG_LVL_HIGH, "rf_tdscdma_mc_is_rx_sawless port: status =%d", status);

  return (status);
}


rfc_tdscdma * rfc_tdscdma::get_instance(void)
{
  return NULL;
}


sys_lte_band_mask_e_type rfm_get_lte_band_mask ( void )
{
  sys_lte_band_mask_e_type rfm_lte_band_mask;
  memset(&rfm_lte_band_mask, 0, sizeof(rfm_lte_band_mask));
  return rfm_lte_band_mask;
}
#endif

uint8 
rflte_mc_single_rx_ca_get_intra_band_ca_support(
    rfcom_lte_band_type      band )
{
  return 0;
}

#if 0
sys_lte_band_mask_e_type rflte_mc_get_lte_ca_band_mask
(
  rfcom_lte_band_type pcell_band
)
{
  sys_lte_band_mask_e_type rfm_lte_band_mask;
  memset(&rfm_lte_band_mask, 0, sizeof(rfm_lte_band_mask));
  return rfm_lte_band_mask;
}
#endif

void rflte_mc_get_lte_ca_band_combo_support(rflte_mc_rrc_ca_band_combos_s *ca_band_combo)
{
  return;
}

#if 0
boolean rfm_wcdma_get_dbdc_band_support( uint16 *num_band_comb_supported, const rfm_wcdma_dual_band_type** band_comb_list)
{
  return FALSE;
}
#endif

boolean
rfm_get_rfcard_dynamic_properties
(
  rfm_rfcard_dynamic_properties_type *rfc_cfg
)
{
  return FALSE;
}

#if 0
void rfc_tdscdma_get_band_pa_lut_type
(
  rfm_device_enum_type  device, 
  rfcom_tdscdma_band_type band, 
  uint32* rf_asic_band_pa_lut_map
)
{
  return;
}

void *rfc_tdscdma_tx_band_config_get(rfm_device_enum_type  device, rfcom_tdscdma_band_type band,
                                 rfc_band_config_data_enum_type config_data)
{
  return NULL;	
}

void *rfc_tdscdma_rx_band_config_get(rfm_device_enum_type device, rfcom_tdscdma_band_type band,
                                 rfc_band_config_data_enum_type config_data)
{
  return NULL;
}
#endif

void sbi_simulator_teardown()
{
  return;
}


void sbi_simulator_dump_transactions_and_registers_to_file(char* file_name)
{
  return;
}


void sbi_simulator_init(rfdevice_id_enum_type device_id)
{
  return;
}

#if 0
boolean
rfm_lte_dealloc_bandwidth_class_combos(
  rflte_mc_bw_class_client_type client
)
{
  return FALSE;
}


rflte_mc_bw_class_combos_s*
rfm_lte_get_lte_ca_bandwidth_class_combos_support(
  rflte_mc_bw_class_client_type client
)
{
  return NULL;
}
#endif

extern "C"
{
  int reclaim_memory_to_heap(void* base_address, uint32 size)
  {
    return 0;
  }
}//extern "C"

unsigned int __rf_reclaim_data_start__ = 0;
unsigned int __rf_reclaim_data_end__ = 0;

