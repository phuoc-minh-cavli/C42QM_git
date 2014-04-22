/*!
   @file
   rfc_card_gnss.cpp

   @brief
   This file contains all the gnss mode specific implementations to support RFC.
   These are platform independent.

*/

/*===========================================================================

Copyright (c) 2008-2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2020/04/16 12:19:05 $ $Author: pwbldsvc $
$Header: //components/rel/rftech_gnss.mpss/2.10/gen8/rf/rfc/src/rfc_card_gnss.cpp#1 $

when       who     what, where, why
------------------------------------------------------------------------------- 
04/12/19   aj      Added API rfc_gnss_is_mb_path_supported to check for mb port in GNSS
02/05/19   Om      Expose standalone API to GNSS to GRFC signal info as well for GNSS path
08/27/18   aj      Expose standalone API to GNSS to return rfc signal info for WGR
10/31/13   shb     Converted TRX common device interface to C++
05/06/13   pv      Triton Support for multiple transceivers for GPS.
07/17/12   jr      Added support for non-ELNA configuration
07/10/12   rmb     Removed critical section enter/exit and added these in rfc_common.c 
                   for wakeup/sleep function.
02/28/12   sr      made changes to put all the GPIOs into low power mode during sleep.
11/05/11    aak     Enable NPA calls from RFSW for power management on 9x15  
08/25/11   aak     Enable RF NPA voting only for MSM8960 
07/15/11   gy      Enabled command dispatch
05/24/11   dbz     Initial version.

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
extern "C" {
#include "rfc_card.h"
#include "rfc_card_gnss.h"
#include "rfm_path_types.h"
}

#include "rfdevice_trx_common_intf_class.h"
#include "rfc_gnss.h"
#include "rfc_gnss_data.h"
#include "rfc_common_data.h"
#include "modem_mem.h"
#include "sdr105_typedef_ag.h" 
/* ----------------------------------------------------------------------- */
/*!
  @brief
  Sets the specified path to the technology mode of operation.

  @details
  This funtion will call the card specific version of the set mode API.

  @parameter
  path RF signal path

*/
void rfc_gnss_mode_config( rf_path_enum_type path )
{
  rfc_gnss *rfc_obj = rfc_gnss::get_instance();

  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"gnss RFC Obj is NOT created. ",0);
    return;
  }

  rfc_obj->config_mode_data(path);

  return;
} /* rfc_gnss_mode_config() */

void rfc_gnss_manage_vregs
( 
  rf_path_enum_type path, 
  rfcom_mode_enum_type mode, 
  rfm_path_state state
)
{
  rfc_gnss *rfc_obj = rfc_gnss::get_instance();
  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"gnss RFC Obj is NOT created. ",0);
    return;
  }

  if (  state == RF_PATH_SLEEP_STATE)
  {
    /* disable the rf signals, put the signals into low power mode */
    rfc_common_sleep(path, RFM_GNSS_MODE);
  }
  else
  {
    /* enable the rf signals, if the signals are in low power mode */
    rfc_common_wakeup(path, RFM_GNSS_MODE);
  }

  rfc_obj->manage_vregs(path, mode, state);

  return;
}

int32 rfc_gnss_command_dispatch
( 
  rf_path_enum_type path, 
  rfcs_gnss_cmd_type cmd, 
  void *data
) 
{
  rfc_gnss *rfc_obj = rfc_gnss::get_instance();
  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"gnss RFC Obj is NOT created. ",0);
    return 0;
  }

  rfc_obj->command_dispatch(path, cmd, data);

  return 1;

}

extern "C" {
rfc_msm_signal_info_type *rfc_msm_signal_info_tbl2=NULL;
uint32 rfc_msm_num_signals_2;

/* rfc Signal info table for the specific RF CARD get initialized once during the init. */
rfc_signal_info_type *rfc_signal_info_tbl2 = NULL;
uint32 rfc_num_signals = 0;

}

void rfc_gnss_get_rfc_signal_info_signal_list(rfgnss_rfc_signal_list_type *rf_signal_list)
{

  uint16 i = 0;
  int32 signal_num = -1;
  rfc_cfg_params_type cfg;
  rfc_sig_cfg_type *rf_rfc_signals = NULL;
  rfc_gnss_data *rfc_gnss_data = rfc_gnss_data::get_instance();

  if (rfc_gnss_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfc_gnss_data == NULL!", 0);
    return;
  }

  rfc_common_data *rfc_data = NULL;
  rfc_data = rfc_common_data::get_instance();

  if (rfc_data == NULL)
  {
    ERR_FATAL("Cannot Create RFC COMMON DATA Object",0,0,0);
  }
  // Retrive RF_GRFC/GRFC/GPIO table
  rfc_num_signals = rfc_data->sig_info_table_get(&rfc_signal_info_tbl2);

  cfg.alternate_path = 0;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;
  rfc_gnss_data->sig_cfg_data_get(&cfg, &rf_rfc_signals);
  if (rf_rfc_signals == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rf_rfc_signals == NULL!", 0);
    return;
  }
  int32 rf_sig_name = RFC_SIG_LIST_END; 
  rfc_msm_signal_type msm_signal = RFC_MSM_SIG_NUM;

  rfc_msm_num_signals_2 = rfc_msm_sig_info_table_get(&rfc_msm_signal_info_tbl2);
 
  // Count number of RFC signals
  while (rf_rfc_signals[i].sig_name != RFC_SIG_LIST_END)
  {
     i++;
  }
  rf_signal_list->num_rfc_signals=i;
  rf_signal_list->rfc_signals = (rfgnss_rfc_signal_info_type *)modem_mem_alloc((rf_signal_list->num_rfc_signals)*sizeof(rfgnss_rfc_signal_info_type),MODEM_MEM_CLIENT_RFA);

  if (rf_signal_list->rfc_signals == NULL )
  {
 	 MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR," Memory allocation failed rf_signal_list->rfc_signals is %x", rf_signal_list->rfc_signals);
     return;
  }


  for (i=0;i<rf_signal_list->num_rfc_signals ;i++)
  {
    rf_sig_name = rf_rfc_signals[i].sig_name; /* get the RF-CARD Signal name */
    msm_signal = rfc_signal_info_tbl2[rf_sig_name].msm_sig_name; /* get the MSM Signal name */
    rf_signal_list->rfc_signals[i].sig_type = rfc_msm_signal_info_tbl2[msm_signal].output_type;
    rf_signal_list->rfc_signals[i].gpio_id = rfc_msm_signal_info_tbl2[msm_signal].msm_gpio;
    rf_signal_list->rfc_signals[i].grfc_id= rfc_msm_signal_info_tbl2[msm_signal].grfc_num;	
    rf_signal_list->rfc_signals[i].start.logic=(rfm_rfc_logic_type)rf_rfc_signals[i].start.logic;
    rf_signal_list->rfc_signals[i].start.time_offset=rf_rfc_signals[i].start.time_offset;
    rf_signal_list->rfc_signals[i].stop.logic=(rfm_rfc_logic_type)rf_rfc_signals[i].stop.logic;
    rf_signal_list->rfc_signals[i].stop.time_offset=rf_rfc_signals[i].stop.time_offset;
  }

}
/* ----------------------------------------------------------------------- */
/*!
  @brief
  check for port details 

  @details
  This funtion will call the card to check port configuration .

  @parameter
  void

*/
boolean rfc_gnss_is_mb_path_supported (void)
{

  rfc_cfg_params_type cfg;
  int32 tx_port=0;
  uint32 device_num=0;
  rfc_device_info_type *device_info = NULL;
  rfcom_band_type_u gnss_band;
  rfdevice_type_enum_type dev_type = RFDEVICE_TYPE_INVALID;
  int32 *rfc_data_ptr =NULL;
  
  
  rfc_gnss_data *rfc_gnss_data = rfc_gnss_data::get_instance();

  if (rfc_gnss_data == NULL)
  {
    RF_MSG( RF_ERROR, "rfc_gnss_is_mb_path_supported. Null rfc_gnss_data !");
	return FALSE;
  }
  
  cfg.alternate_path = 0;
  cfg.req =RFC_REQ_DEFAULT_GET_DATA ;
  rfc_gnss_data->devices_cfg_data_get(&cfg, &device_info);
  
  if (device_info == NULL)
  {
    RF_MSG( RF_ERROR, "rfc_gnss_is_mb_path_supported. Null device info data !");
    return FALSE;
  }
  device_num= device_info->num_devices;
  for(uint16 device_itr=0; device_itr< device_num;device_itr++)
  {
	 dev_type = device_info->rf_asic_info[device_itr].device_type;
     if(dev_type == RFDEVICE_TRANSCEIVER)
     {
		MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "rfc_gnss_is_mb_path_supported. in device loop device_itr%d!",device_itr);
        rfc_data_ptr=device_info->rf_asic_info[device_itr].data;
        tx_port=*(rfc_data_ptr+1);
		break;
	 }
  }

   if(tx_port==(int)SDR105_GNSS_MB_LNA_IN)
   {
	  RF_MSG( RF_HIGH, "rfc_gnss_is_mb_path_supported. Success !");
	  return TRUE;
		  
   }
   else
   {
	  RF_MSG( RF_ERROR, "rfc_gnss_is_mb_path_supported. Failure !");
      return FALSE;
	}
		
  
}
/* ----------------------------------------------------------------------- */
/*!
  @brief
  gets LNA configuration

  @details
  This funtion will call the card to get LNA configuration.

  @parameter
  lna config type

*/

void rfc_gnss_get_LNA_config
( 
  rfc_gnss_lna_config_type *lna
) 
{
  rfc_gnss *rfc_obj = rfc_gnss::get_instance();
  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"gnss RFC Obj is NOT created. ",0);
    return;
  }

  rfc_obj->get_LNA_config(lna);

  return;

}

/* ----------------------------------------------------------------------- */
/*!
  @brief
  gets All the device configuration needed to operate on GNSS.

  @details
  This funtion will call the card to get device configuration for a specific type of device.

  @parameter
  rfdevice_type_enum_type
 
  @return
  array of pointers to the devices.

*/

rfdevice_class* rfc_gnss_get_dev_obj( rfdevice_type_enum_type dev_type, uint8 instance) 
{
  rfc_gnss *rfc_obj = rfc_gnss::get_instance();
  rfdevice_class *temp_gnss_dev_obj = NULL;
  
  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"gnss RFC Obj is NOT created. ",0);
    return temp_gnss_dev_obj;
  }

  temp_gnss_dev_obj = rfc_obj->get_rf_device_obj(dev_type, instance);

  return temp_gnss_dev_obj;
}
