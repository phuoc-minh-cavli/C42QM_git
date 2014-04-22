#ifndef RFDEVICE_CMN_INTF_H
#define RFDEVICE_CMN_INTF_H

/*!
  @file
  rfdevice_cmn_intf.h

  @brief
  This file provides common interface functions to configure the RTR/WTR device.
*/

/*==============================================================================

  Copyright (c) 2010 - 2021 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/rfa.mpss/3.10/device/rfdev_intf/inc/rfdevice_cmn_intf.h#2 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
04/27/21   up      B66 HDET spur issue Fix
04/01/20   up      Reverted CR: 2634827 changes
03/04/20   up      Reverted IOTFE therm read template preload changes
09/09/19   up      IOTFE therm read support in INIT and SLEEP state.
07/02/19   up      Featurized GSM related functions
04/19/19   dh      Support for IOTFE therm reads
02/12/19   up      Added API for IOTFE therm read rfdevice_trx_cmn_read_iotfe_therm
01/29/19   gk      XO DCC Calibration support changes
10/26/18   up      Added rfdevice_cmn_ustmr_sync support
10/26/18   up      Mainlined sdr105 bu common intf changes
09/06/18   tv      Added rfdevice_cmn_get_tx_dummy_script_trigger() API
08/14/18   up      Modified rfdevice_cmn_ustmr_sync
08/10/18   ssr     Added interface APIs to return LTE/GSM script sizes
06/27/18   up      Modified the API rfdevice_cmn_get_tx_lut_scripts()
05/30/18   up      Modified the API rfdevice_cmn_get_tx_lut_scripts to 
                   include freq as input param
05/25/18   up      Added new APIs for booutup init and ustmr sync
02/14/18   nm      Updated rfdevice_cmn_get_tx_lut_scripts() interface
10/20/15   up      Modified for SDR105, renamed rfdevice_rxtx_common_class to 
                   rfdevice_trx_common_intf_class
10/20/15   sd      Added API to get WTR capability for Autopin PAR captures
10/06/15   sd      Added support for AutoPin PAR
05/05/14   sma     Added ULCA 20+20 Tx LUT power offset
02/24/15   aks     Device driver updates to enable ADC Vcm Cal 
02/10/15   dps     Added support for Tx RSB calibration on WTR devices
01/05/15   aks     Added a new API for BBRx DC Cal
12/08/14   hs      Added API to set the JDET reading timer.
11/24/14   aks     Updated get_tx_lut_scripts to accept bw union as an input
10/08/14   sd      Added support for Therm enable, therm read and hdet calculation
09/03/14   rsr     Added support for FTM command to retrieve manufacture and product id of transceiver
08/15/14   sd      Added tech argument to FBRX Enable API
08/14/14   shb     Updated fbrx_setup/disable APIs to support slow_ramp ON and 
                   OFF sequence for LDO 26
08/05/14   sd      Added suppport for ILPC
07/25/14   sd      Added frequency arg to FBRX Enable API to handle band
                   dependent sequences
06/24/14   sma     Added LNA gain state for FBRX RSB  
05/29/14   shb     Added frequency arg to FBRX APIs to handle band dependent 
                   sequences
05/23/14   hm      Added API support to indicate PMIC 1V rail shutoff 
05/22/14   bm      Addded API to support RF device light sleep
04/14/14   bm      Changes to decouple GPS and WWAN device ON/OFF state changes 
03/10/14   hm      FBRX enable API enhancement to add delay after LDO enable, if configured
02/14/14   shb     Added API to retrieve FBRX RSB coefficients
02/05/14   hm      Added FBRX DC Cal enable and disable API
02/05/14   shb     Added read_therm API in WTR common class interface
12/16/13   tks     Added prototype for stg enable and disable
10/31/13   shb     Switch to C++ TRX common interface
10/11/13   shb     Added FBRX setup API. Updated FBRx enable/disable for 
                   immediate execution support
10/03/13   APU     Add support for getting new common Tx LUT.
10/01/13   cd      Renamed TxAGC APIs and added execution type param
09/30/13   shb     Added new common TXAGC APIs
08/26/13   shb     Added API to get tech obj ptr in new format
05/20/13   kai     Added new feedback path attenuation state enum
04/18/13   sty/fh  Added RFDEVICE_NO_TIME_OFFSET
12/04/12   dps     Added rfdevice_cmn_cmd_dispatch()
12/03/12   aca     DIME Dac cal
09/24/12   sr      Added initernal_device_cal interface api.  
08/15/12   shb     Moved all function pointer and data types to new file 
                   rfdevice_cmn_typedef.h. This file now contains function
                   prototypes only
08/02/12   kb      Ported CL 2585079 from nikel  
07/27/12   sr      renamed rfdevice related enum types.
07/10/12   vss     Add function ptr for retrieving device type. 
07/09/12   aak     Move Quiet mode API from device to RFC 
06/20/12   shb     Added rf_device_execution_type
06/07/12   shb     Added rfdevice_dpd_enum_type
04/11/12   shb     Added support for shadow backup/restore
03/26/12   vb      Added support for new RF device function to program DAC Iref 
03/15/12   shb     Added arg to return chip process during device creation
02/24/12   shb     Created internal dev cal struct to handle all int device cal
                   and a HDET autocal status type
02/16/12   pv      RF Common Core Device Manager Implementation.  
02/14/12   shb     Removed process/version check interface
01/20/12   tks     Added support for WTR1605 process/version check  
01/20/12   shb     Created generic rfdevice_script_type that can be used by all
                   tech APIs that need to return a SSBI script
01/16/12   aro     PA state as an argument to enable ET path function
01/13/12   aro     Added support for ET path enable for self calibration
01/13/12   aro     Documentation update
11/21/11   vb      Added RFDEVICE_IREF_INVALID in rfdevice_dac_iref_val
11/17/11   aak     Changes to implement Quiet mode  
11/07/11   aak     Changes to implement Quiet mode 
11/02/11   vb      DAC cal should not be performed for Iref less than 90uA,
                   hence reducing the number of DAC Cal points.
10/20/11   shb     Added rfdevice_ssbi_write_type for WTR1605 scripting support
09/27/11   aro     Added interface to load RC tuner calibration data
09/25/11   vb      Support for DAC cal using different Irefs
09/19/11   aro     Changed the Internal calibration error code definition
09/18/11   aro     Updated RC tuner interface to pass generic RC tuner
                   calibration data to remove RTR8600 specific interface
09/14/11   aro     Added error codes for internal calibration
09/14/11   aro     Updated RC tuner calibration interface to include pointer
                   to cal data as function argument
09/14/11   aro     Added doxygen documentation for RC tuner calibration intf
09/14/11   aro     Added internal calibration status enumeration
09/14/11   aro     Added documentation to make the file doxygen compatible
09/14/11   aro     Deleted unused variables
09/14/11   aro     Added Common device interface to perform RC tuner
                   calibration
09/13/11   aro     Updated common interface to have common function table
                   and common device data
09/13/11   aro     Updated the file with doxygen documentation
07/11/11   shb     Updated constness of rfdevice_cmn_type members for 
                   robustness
09/22/10   shb     Added pointer to common data in common device structure
09/22/10   shb     Make common device struct rfdevice_cmn an array of pointers
                   of max supported paths size
09/22/10   shb     Added rfdevice_cmn_cmd_dispatch()
09/22/10   shb     Make common command dispatch function ptr accept a ptr to 
                   rfdevice_cmn_type to access global data
08/13/10   vb      Changed datatype of rfdevice_cmn_type() for merging RTR9K device
07/27/10   shb     rfdevice_cmd_dispatch_type definition added
07/01/10   av      Initial version.

==============================================================================*/

#include "rfdevice_cmn_type_defs.h"
#include "rfdevice_trx_common_intf_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
  This is a special value that tells the device API to NOT add any time offset
  to the settings being retrieved
*/
#define RFDEVICE_NO_TIME_OFFSET                                          0X7FFF

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cmn_set_dac_iref
(
  rfdevice_trx_common_intf_class*                       cmn_dev,
  rfdevice_dac_iref_val                                 iref_val,
  boolean                                               program_iref,
  uint32*                                               agc_gainword  
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cmn_bootup_init
(
  rfdevice_trx_common_intf_class*                       cmn_dev,
  rf_buffer_intf*                                       script_buf,
  rf_device_execution_type                              execution_type
);

/*----------------------------------------------------------------------------*/
boolean 
rfdevice_cmn_ustmr_sync
(
  rfdevice_trx_common_intf_class*                       cmn_dev,
  rf_buffer_intf*                                       script_buf,
  rf_device_execution_type                              execution_type
);

/*----------------------------------------------------------------------------*/

rfdev_cmn_status_type 
rfdevice_cmn_power_up
(
  rfdevice_trx_common_intf_class*                       cmn_dev,
  rfdev_cmn_vote_status_type                            rfdev_vote,
  rfdev_cmn_voting_type                                 rfdev_voting
);


/*----------------------------------------------------------------------------*/
boolean 
rfdevice_cmn_power_reset
(
  rfdevice_trx_common_intf_class *cmn_dev,
  rfdev_cmn_power_down_type rfdev_power_down_vote
);

#if 1
/*----------------------------------------------------------------------------*/
void*
rfdevice_cmn_get_tech_instance
(
  rfdevice_cmn_type*                                    cmn_dev,
  rfm_mode_enum_type                                    rfm_mode,
  rfdevice_chain_enum_type                              chain 
);
#endif

/*----------------------------------------------------------------------------*/

boolean
rfdevice_cmn_do_internal_device_cal
(
  rfdevice_trx_common_intf_class*                       cmn_device,
  rfdevice_cmn_int_dev_cal_data_type*                   cal_data
);


/*----------------------------------------------------------------------------*/
void*
rfdevice_cmn_get_tech_device
(
  rfdevice_trx_common_intf_class*                       cmn_dev,
  rfdevice_trx_tech_type                                tech,
  uint8                                                 phy_path
);


/*----------------------------------------------------------------------------*/
boolean rfdevice_cmn_get_txagc_template
(
  rfm_device_enum_type                                  rfm_device,
  rfdevice_trx_tech_type                                tech,
  rfcom_band_type_u                                     band_u,
  rfdevice_trx_common_intf_class*                       cmn_device,
  rf_buffer_intf*                                       script_buf,
  uint32*                                               data_fixup_token_id,
  rfdevice_trx_txagc_template_type                      template_type
);
/*---------------------------------------------------------------------------*/
boolean rfdevice_cmn_get_tx_lut_scripts
(
  rfdevice_trx_common_intf_class*                       cmn_device,
  rfdevice_tx_lut_data_type*                            lut_data,  
  rfm_device_enum_type                                  rfm_device,
  rfm_mode_enum_type                                    tech,
  rfdevice_trx_lte_tx_path_info_type*                   current_path_info,
  rf_device_execution_type                              execution_type,
  rfdevice_trx_txagc_template_type                      template_type,
  uint8                                                 pa_state,
  uint8                                                 rgi,
  boolean                                               get_txrsb_params
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_cmn_get_rxagc_template
(
  rfdevice_trx_common_intf_class*                       cmn_device,
  rf_buffer_intf*                                       script_buf
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_cmn_get_rxagc_trigger
(
  rfdevice_trx_common_intf_class*                       cmn_device,
  rf_buffer_intf*                                       script_buf
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_cmn_get_tx_dummy_script_trigger
(
  rfdevice_trx_common_intf_class*                       cmn_device,
  rf_buffer_intf*                                       script_buf
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cmn_is_cmn_tx_intf_supported
(
  rfdevice_trx_common_intf_class*                       device
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_trx_cmn_read_therm
(
  rfdevice_trx_common_intf_class*                       cmn_device,
  uint16*                                               therm_read_raw,
  int16*                                                therm_read_in_degC
);


/*----------------------------------------------------------------------------*/

boolean rfdevice_trx_cmn_read_pa_therm
(
  uint8                                               therm_handle,
  uint16*                                             therm_read_raw,
  int16*                                              therm_read_in_degC
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cmn_reg_dump
(
 rfdevice_trx_common_intf_class*                        cmn_device,
 rfdevice_reg_dump_cmd_type                             reg_dump_type,
 boolean                                                log_on_off
);

/*----------------------------------------------------------------------------*/
boolean rf_device_get_mfg_prd_ids 
(  
   rfdevice_trx_common_intf_class*                      cmn_device, 
   uint16*                                              mfg_id, 
   uint16*                                              prd_id,
   uint16*                                              revision_id
);

/*----------------------------------------------------------------------------*/
boolean
rfdevice_cmn_calculate_hdet_reading
(
  rfdevice_trx_common_intf_class*                       cmn_device,
  int32                                                 sum_i,
  int32                                                 sum_q,
  uint64                                                sum_i_q_sqr,
  uint32                                                fbrx_accum,
  uint8                                                 fbrx_accum_overflow,
  uint16*                                               hdet_reading
);

#ifdef FEATURE_GSM
/*----------------------------------------------------------------------------*/
boolean
rfdevice_cmn_rxsig_jdet_enable_set_timer
(
  rfdevice_trx_common_intf_class*                       cmn_device,
  uint16                                                holdoff_time_us,
  rf_device_execution_type                              exec_type,
  rf_buffer_intf*                                       script_buf
);
#endif /* #ifdef FEATURE_GSM */
/*----------------------------------------------------------------------------*/
boolean rfdevice_cmn_rx_adc_vcm_cal_setup
(
  rfdevice_trx_common_intf_class*                       cmn_device,
  rfdevice_rx_vco_path_enum_type                        rx_vco
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_get_lte_script_size
(
  rfdevice_trx_common_intf_class*                       cmn_dev,
  rfdevice_trx_lte_rx_event_size_type*                  rx_events,
  rfdevice_trx_lte_tx_event_size_type*                  tx_events
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_get_gsm_script_size
(
  rfdevice_trx_common_intf_class*                       cmn_dev,
  rfdevice_trx_gsm_rx_event_size_type*                  rx_events,
  rfdevice_trx_gsm_tx_event_size_type*                  tx_events
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_cmn_get_xogen_dcc_val
(
  rfdevice_trx_common_intf_class*                       cmn_dev,
  rf_buffer_intf*                                       script_buf,
  rf_device_execution_type                              execution_type, 
  rfm_dcc_info_type*                                    dcc_info
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_trx_cmn_read_iotfe_therm
(
  rfdevice_trx_common_intf_class*                     cmn_device,
  uint16*                                             therm_read_raw,
  int16*                                              therm_read_in_degC
);

/*----------------------------------------------------------------------------*/
void rfdevice_cmn_reset_iotfe_template_preload_flag
(
  rfdevice_trx_common_intf_class*                     cmn_device
);

/*----------------------------------------------------------------------------*/
void rfdevice_cmn_set_rfc_power_class
(
  rfdevice_trx_common_intf_class*                     cmn_device,
  boolean                                             is_cur_card_pc3
);

#ifdef __cplusplus
}
#endif

#endif /* RFDEVICE_CMN_INTF_H */

