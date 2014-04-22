/*!
  @file rfdevice_lte_interface.h

  @brief
  This file is to provide interface functions to configure the RTR device
  for LTE mode.

  @details
  A RF card can have multiple RF devices. This layer provides transparent
  device independent interface to Main Control, Core and Feature Modules, 
  so that clients of this layer doesn't need to know how many and what specific
  RF devices are installed. Generic LTE interface to RF device is provided using
  function pointers, which will be initialized by the specific RF device.

  RF Configuration file which is specific to RF CARD and the MDM will maintain 
  data regarding what specific rf devices are present in the installed RF Card. 
  Also, it is resposible for maintaining what specific RF Device to use when 
  there is change in mode(technology) of operation or when band changes.   
  Whenever a mode or band changes the RF Configuration module will request the 
  specific RF device such as RTR8600 to initialize the generic RF device function 
  pointer struct. After the function ptr struct is initialized by the specific 
  rf device, the Main Control, Core and Feature can use the initialized function 
  pointers to setup/query the RF Device.

*/

/*===========================================================================

  Copyright (c) 2009 - 2019 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rfa.mpss/3.10/device/rfdev_intf/inc/rfdevice_lte_interface.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
02/06/19   as      Non Anchor Rx retune support
01/29/19   gk      XO DCC Calibration support changes
08/09/18   bm      Updated get_rgi0_trigger_script api to provide timer token info
08/08/18   up      Removed redundant interfaces:rfdevice_lte_rx_get_ffh_reg_bank_sel
                   and rfdevice_lte_tx_get_ffh_reg_bank_sel
07/26/18   up      dflo_offset_khz in FFH data fixup API for LTE RX and TX 
                   is made as int32
07/25/18   up      Modified the APIs
                   - rfdevice_lte_tx_toggle_pll_and_sigpath
                   - rfdevice_lte_tx_get_rgi0_trigger_script
                   - rfdevice_lte_tx_get_ffh_data
07/19/18   up      Added new API rfdevice_lte_tx_get_rgi0_script.
07/02/18   up      Added new APIs rfdevice_lte_rx_get_ffh_data and 
                   rfdevice_lte_tx_get_ffh_data
06/27/18   up      Modified API rfdevice_lte_rx_set_band_data and 
                   rfdevice_lte_tx_set_band_data
06/25/18   up      Updated APIs rfdevice_lte_tx_get_ffh_reg_bank_sel and 
                   rfdevice_lte_tx_get_ffh_reg_bank_sel
06/25/18   up      Added the APIs rfdevice_lte_tx_get_ffh_reg_bank_sel and 
                   rfdevice_lte_tx_get_ffh_reg_bank_sel
06/20/18   up      Modified the API rfdevice_lte_tx_toggle_sigpath to take
                   lte_cat as input param.
06/18/18   up      Modified rfdevice_lte_tx_get_bbf_resp to tale lte_cat as input
04/25/18   up      Added RX FFH API: rfdevice_lte_rx_get_ffh_template 
                   and removed the LTE TX API rfdevice_lte_tx_get_ffh_parameters
02/14/18   up      Removed the API rfdevice_lte_tx_get_txagc_data from LTE 
                   interface, it is available in common interface as 
                   rfdevice_cmn_get_tx_lut_scripts
02/13/2018 up      nominal_freq_hz in rfdevice_lte_rx_get_afc_script is
                   renamed to nominal_freq_khz
01/20/15   up      Modified for SDR105, renamed rfdevice_rxtx_common_class to 
                   rfdevice_trx_common_intf_class
01/23/18   up      Added/Modified SDR105 LTE RX and TX APIs
                   FR 46405: SDR105 TRX (IOTRF) device driver addition 
05/30/17   smi     TxPLL setting changes for NBIOT
01/11/17   ry      Tx PLL fast lock changes for LTE CatM
12/02/16   cv      Merging MTPL and AFC for UL Hopping
11/10/16   ry      Timing updates for LTE CatM1
11/10/16   smi     Changes to support PLL fast lock for LTE CATM
08/12/16   ry      Added APIs to build the Rx and Tx PLL scripts
08/05/15   bm      Added rfdevice_lte_tx_get_dac_backoff() API
                   Renamed rfdevice_lte_tx_get_single_rb_data API
05/19/15   bm      Added get_single_rb_data() API interface
04/21/15   sma     Scell tear down changes for CA NC
04/01/15   zg      Added power voting api
03/18/15   pkg     Changes to support Script based Tx Tune.
01/23/15   kab     RSB NV support
01/08/15   aks     Updated get_gain_default_pwr_val to accept operation_mode as an input 
12/16/14   aks     Updated exit and get_tune_script APIs to accept an array 
                   through which they can enforce retune of an active Rx chain
12/10/14   aks     Added API to get max BW supported by Tx port
11/13/14   zg      Added new API of get_vco_path
09/11/14   vb      RSB/IQMC per LNA gain state support
08/28/14   shb     Deleted unused get_critical_section API
08/15/14   aks     Added API to get Tx tune as preload and trigger script 
08/14/14   aks     Added API to get the default gain offsets for each gain state
08/14/14   aks     Updated Doxygen Comments 
07/23/14   sma     Added commit to efs for writing RSB data once
07/17/14   aks     Added call-back API param to tune-to-chan.
08/05/14   sma     Rx RSB cal per gain state efs support 
07/31/14   ndb     Added support for extended EARFCN for LTE
07/30/14   gvn     Intra-band fixes: pass intraband flag to get sca lna script
07/30/14   aks     Added API to return tune Script as separate Preload and Trigger Scripts 
07/17/14   aks     Added call-back API param to tune-to-chan.
07/16/14   aks     Added API to get AFC adjusted Notch Frequency 
06/12/14   aks     Added API to get AFC adjusted Notch Frequency 
4/29/14    xsh     Update the fbrx rsb cal API to ensure calibrated rsb cal can pass back
03/28/14   npi     Obtain protocol from common instance
03/17/14   npi     Get common device pointer from tech device instance
03/12/14   npi     Check for Rx intraband CA support
12/16/13   aks     Added new API rfdevice_lte_rx_enable
11/15/13   shb     Added arg in device IRAT API for startup vs cleanup
10/26/13   shb     Updated IRAT APIs to support preload
09/30/13   npi     Support for determining Bus Type
09/26/13   kab     Fix for using stg_is_bad_fuse value from actual STG 
                   involved in RSB Cal
09/18/13   php     Pick LO Div ratio based Type 1 Spur present
08/21/13   aks     Modified function rfdevice_lte_rx_tune_to_chan to accept
                   spur_mitigations_params pointer as an argument 
08/20/13   aka     Added FB path STG APIs
07/24/13   gvn     Change name of RSB funtion
07/03/13   php     Add EFS support to save and load dcoc value
06/28/13   vbh     Added an include file 'rfdevice_cmn_intf.h'
06/26/13   gvn     Write RSB Cal params to NV and other RSB cal updates
06/12/13   shb     Added API to get ET delay compensation
05/03/13   gvn     STG Hook-up for RSB Cal
04/17/13   cri     Support for TxLM dynamic Tx filter coefficient programming 
03/26/13   vss     Add support for send intraband ca status to rx tune function
03/12/13   sn      Added rfdevice_lte_tx_tune_to_default_port
01/29/13   sbm     Remove coupler Hack.
01/10/13   pl      Added LTE common table write
12/02/12   pv      Added Set Rx Gain settings API.
11/30/12   pv      Added RSB Get Coefficients' API.
11/21/12   pv      Added support for Get Rx Gain state table setting API.
11/20/12   gvn     Featurize LTE for Triton 
11/20/12   sbm     Add HDET Events support
10/31/12   vss     Add API's for get rx and tx timing info 
10/05/12   pl      Added support for meas script generation API
08/31/12   pv      Added support for another parameter for tx_set_band_port
08/22/12   shb     Deleted rfdevice_interface.h 
08/15/12   shb     Moved all LTE data structs from rfdevice_lte_interface.h to 
                   rfdevice_lte_type_defs.h 
08/08/12   vss     Update set_rx_port and set_tx_port to use device instance instead of RFM device 
08/07/12   vss     Add bands to all device intf since it is required to query RFC
08/06/12   php     Add dpd state support
08/02/12   shb     Added rfdevice_lte_rx/tx_set_band_data
07/31/12   pv      Use BB Filter response for compensating Tx Power.  
07/27/12   sr      renamed rfdevice related enum types.
07/23/12   php     Add FTM mode option to Device tune to chan
07/12/12   sr      Add device interface to push port info from rfc. 
07/11/12   shb     Updated script to be of rf_buffer_intf* type instead of 
                   void ptr
07/10/12   vss     Remove unused path field in rx and tx cmd dispatch
06/21/12   vss     Add support for scripting 
06/06/12   pl      Update LTE device interface to take RFM_DEVICE enum
03/20/12   aca     Implemented XO correction during Tx pll tune
07/25/11   aca     CW Fix- device pointer updated to be const
07/18/11   aca     Interface Update- WTR 1605 development
07/07/11   aca     Multi device support
07/06/11   aca     WTR1605 bringup
04/01/11   bmg     Added extern "C" for C++ safety.
01/13/11   pl      Use proper BW type
09/15/10   jyu     Fixed Compiler Warnings
07/06/10   pl      enable Dynamic band switching with RTR reset
06/27/10   pl      enable Dynamic band switching in LTE
05/12/10   qma     LTE exit support for state cleanup
05/02/10   qma     Added LTE RTR8600 sleep/wakeup and TX disable support
04/14/09   qma     Initial Check-in

===========================================================================*/

#ifndef RFDEVICE_LTE_INTERFACE_H
#define RFDEVICE_LTE_INTERFACE_H


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "comdef.h"
#include "rfcom.h"
#include "rfcommon_locks.h"
#include "rf_buffer_intf.h"
#include "rfdevice_cmn_intf.h"
#include "rf_hal_bus_types.h"
#include "rfdevice_class.h"

#ifdef FEATURE_LTE
#include "rfdevice_lte_type_defs.h"
#include "rfdevice_trx_lte_rx_intf_class.h"
#include "rfdevice_trx_lte_tx_intf_class.h"


#ifdef __cplusplus
extern "C" {
#endif


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
#define rfdevice_lte_config(device, rxtx, path) rfdevice_lte_##device##_##rxtx##_config(path);

/*! Structure in RFDevice interface to use this PA lut across RF Devices */
typedef struct
{
  /*! Flag to turn ON/OFF the feature from RFC */
  boolean active;

  /*! Band to PA State to LUT mapping*/
  uint8 band_pa_lut [RFCOM_NUM_LTE_BANDS][RFDEVICE_MAX_PA_STATES];

} rfc_based_lut_mapping_type;

/*============================================================================== 
 
                             FUNCTION DECLARATIONS
 
==============================================================================*/
/*----------------------------------------------------------------------------*/
boolean rfdevice_lte_rx_set_band_data(
  rfdevice_trx_lte_rx_intf_class*                   rx_device,
  rfcom_lte_band_type                               band,
  int32*                                            data_array,
  uint8                                             array_size,
  rfm_device_enum_type                              device
  
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_lte_tx_set_band_data(
  rfdevice_trx_lte_tx_intf_class*                   tx_device,
  rfcom_lte_band_type                               band,
  int32*                                            data_array,
  uint8                                             array_size,
  rfm_device_enum_type                              device
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_lte_rx_get_tune_script(
  rfcom_lte_band_type                               rf_band,
  uint32                                            freq_khz,
  rfcom_lte_bw_type                                 rfcom_bw,
  rfm_device_enum_type                              rfm_device,
  rfdevice_lte_script_data_type*                    script_data_ptr,
  rf_hal_bus_resource_type*                         script_type,
  rfdevice_lte_carrier_info_type*                   rf_lte_carrier_info,
  rfcom_lte_cat_type                                lte_cat,
  rfdevice_pll_truncation_error_info_type*          pll_trunc_error_info
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_lte_rx_get_tune_trigger_script(
  rfm_device_enum_type                                  rfm_device,
  rfcom_lte_band_type  					rf_band,
  rfdevice_lte_script_data_type*                        script_data_ptr,
  rf_hal_bus_resource_type*                             script_type,
  rfcom_lte_cat_type                                    lte_cat
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_lte_tx_get_tune_script
  (
    rfm_device_enum_type                            device,
    uint32                                          freq_khz,
    int32                                           rx_freq_error,
    rfcom_lte_band_type                             rf_band,
    rfcom_lte_bw_type                               rf_bw,
    rfdevice_lte_script_data_type*                  script_data_ptr,
    rfcom_lte_cat_type                              lte_cat
  );

/*----------------------------------------------------------------------------*/
boolean rfdevice_lte_rx_get_afc_script
(
  rfm_device_enum_type                              rfm_device,
  rfcom_lte_band_type                               rf_band,
  uint32                                            nominal_freq_khz,
  int32                                             rx_freq_offset_hz,
  rf_buffer_intf*                                   script_buf,
  rf_device_execution_type                          exec_type, 
  rfcom_lte_cat_type                                lte_cat
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_lte_tx_get_max_bw_supported(
  rfm_device_enum_type                              rfm_device, 
  rfcom_lte_band_type                               rf_band, 
  uint32*                                           max_bw_khz
);

/*----------------------------------------------------------------------------*/
const int8* rfdevice_lte_tx_get_bbf_resp(
  rfm_device_enum_type                              device,
  rfcom_lte_band_type                               rf_band,
  rfcom_lte_bw_type                                 rfcom_bw,
  uint16*                                           bbf_resp_arr_size,
  rfcom_lte_cat_type                                lte_cat
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_lte_tx_get_timing_info(
  rfm_device_enum_type device,
  rfcom_lte_band_type                               rf_band,
  rfcom_lte_bw_type                                 tx_bw,
  rfdevice_trx_lte_tx_event_timing_info_type*       tx_timing_info
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_lte_rx_get_timing_info(
  rfm_device_enum_type                              device,
  rfcom_lte_band_type                               rf_band,
  rfdevice_trx_lte_rx_event_timing_info_type*       timing_info
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_lte_rx_get_lna_gain_script(
  rfm_device_enum_type                              device, 
  rfcom_lte_band_type                               rf_band,
  uint8                                             rx_agc_gain_state,
  rfdevice_lte_script_data_type*                    script_data_ptr,
  rf_hal_bus_resource_type*                         script_type
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_lte_rx_get_rsb_coeff(
  rfm_device_enum_type                              device,
  rfcom_lte_band_type                               rf_band,
  rfdevice_lte_rx_rsb_offset_type*                  rsb_data,
  uint8                                             rx_lna_gain_state
);

boolean rfdevice_lte_get_tx_pll_script(
  rfm_device_enum_type                              device,
  rfcom_lte_band_type                               rf_band,
  int32                                             tx_freq_error_in_hz,
  uint64                                            tx_freq_hz,
  uint8*                                            tx_pll_tuned_flag,
  rfcom_lte_cat_type                                lte_cat,
  rfdevice_lte_script_data_type*                    script_data_ptr
);


/*----------------------------------------------------------------------------*/
boolean rfdevice_lte_get_rx_pll_offset_script(
  rfm_device_enum_type                              device,
  rfcom_lte_band_type                               rf_band,
  uint64                                            rx_freq_hz, 
  rfdevice_lte_script_data_type *                   script_data_ptr, 
  rfcom_lte_cat_type                                lte_cat
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_lte_get_tx_pll_offset_script(
  rfm_device_enum_type                              device,
  rfcom_lte_band_type                               rf_band,
  uint64                                            tx_freq_hz, 
  int32                                             tx_freq_error_in_hz,
  boolean                                           is_afc_flag,
  rfcom_lte_cat_type                                lte_cat,
  uint8*                                            tx_pll_tuned_flag,
  rfdevice_lte_script_data_type*                    script_data_ptr
);

/*----------------------------------------------------------------------------*/
rfdevice_tx_saw_port_type rfdevice_lte_tx_get_port_type(
  rfcom_lte_band_type                               rf_band, 
  rfm_device_enum_type                              rfm_device 
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_lte_rx_calc_rsb_nv_params( 
  rfm_device_enum_type                              rfm_dev,
  rfcom_lte_band_type                               rf_band, 
  rfcom_lte_bw_type                                 rx_bw,
  uint64                                            i_sq,
  uint64                                            q_sq,
  int64                                             i_q,
  int16                                             therm_data, 
  int16*                                            sin_theta_final, 
  int16*                                            cos_theta_final, 
  int16*                                            gain_inv_final,
  uint8                                             stg_is_bad_fuse,
  uint8                                             rx_agc_gain_state
);

boolean rfdevice_lte_rx_commit_rsb_data_to_efs(
  rfm_device_enum_type                              rfm_device,
  rfcom_lte_band_type                               rf_band, 
  rfdevice_rsb_cal_verfication_data_type*           calibrated_list 
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_lte_rx_get_lna_phase_comp_data
(
  rfm_device_enum_type                              rfm_device,
  rfcom_lte_band_type                               rf_band,
  uint8                                             num_gain_states,
  int16*                                            phase_arr_in_degree
);

/*----------------------------------------------------------------------------*/

boolean rfdevice_lte_rx_get_capability
(
  rfm_device_enum_type                              rfm_device,
  rfcom_lte_band_type                               rf_band,
  rfdevice_trx_lte_rx_capabilities*                 trx_capabilities
);

/*----------------------------------------------------------------------------*/
rf_hal_bus_resource_type rfdevice_lte_get_bus_type( 
  rfdevice_trx_common_intf_class*                   dev_ptr
);
/*----------------------------------------------------------------------------*/
rfdevice_trx_common_intf_class* rfdevice_lte_tx_get_common_dev_ptr( 
  rfm_device_enum_type                              rfm_device,
  rfcom_lte_band_type                               rf_band
);

/*----------------------------------------------------------------------------*/
rfdevice_trx_common_intf_class* rfdevice_lte_rx_get_common_dev_ptr( 
  rfm_device_enum_type                              rfm_device,
  rfcom_lte_band_type                               rf_band
);

/*----------------------------------------------------------------------------*/
rf_hal_bus_resource_type rfdevice_lte_get_bus_type_from_rx_device_inst(
  rfdevice_trx_lte_rx_intf_class*                   rx_device
);

/*----------------------------------------------------------------------------*/
rf_hal_bus_resource_type rfdevice_lte_get_bus_type_from_tx_device_inst(
  rfdevice_trx_lte_tx_intf_class*                   tx_device
);

/*----------------------------------------------------------------------------*/
rf_hal_bus_resource_type rfdevice_lte_get_rx_bus_type(
  rfm_device_enum_type                              rfm_device,
  rfcom_lte_band_type                               rf_band
);

/*----------------------------------------------------------------------------*/
rf_hal_bus_resource_type rfdevice_lte_get_tx_bus_type(
  rfm_device_enum_type                              rfm_device,
  rfcom_lte_band_type                               rf_band
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_lte_rx_get_gain_default_pwr_val(
  rfm_device_enum_type                              rfm_device,
  rfcom_lte_band_type                               rf_band,
  rfcom_lte_bw_type                                 rfcom_bw,
  rfdevice_lte_get_rx_gain_settings_type*           gain_settings
);

/*----------------------------------------------------------------------------*/
rfdevice_rx_vco_path_enum_type rfdevice_lte_rx_get_vco_path(
  rfdevice_trx_lte_rx_intf_class*                   rx_device,
  rfcom_lte_band_type                               band
);

boolean rfdevice_lte_rx_get_gain_fixup_data
(
  rfm_device_enum_type                              rfm_device,
  rfcom_lte_band_type                               rf_band,
  rfcom_lte_bw_type                                 rf_bw,
  rfdevice_trx_rx_agc_data_per_gs_type              (*ptr_gain_data_arr)[RFDEVICE_LTE_MAX_LNA_STATES], 
  uint16*                                           data_fixup_token,
  uint8*                                            data_fixup_lut_size_in_uint16,
  uint8*                                            num_wtr_gain_states, 
  rfcom_lte_cat_type                                lte_cat
);

/*----------------------------------------------------------------------------*/

// to toggle sigpath only
boolean rfdevice_lte_rx_toggle_sigpath
(
  rfm_device_enum_type                              rfm_device,
  rfcom_lte_band_type                               rf_band,
  boolean                                           on_off,
  rf_buffer_intf*                                   script_buf,
  rf_device_execution_type                          exec_type,
  rfdevice_trx_timer_fixup_type                     (*ptr_timer_fixup_token_arr)[RFDEVICE_TRX_MAX_TIMER_FIXUPS_PER_SCRIPT],
  uint8*                                            num_timer_fixups,
  rfcom_lte_cat_type                                lte_cat
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_lte_tx_toggle_sigpath
(
  rfm_device_enum_type                              rfm_device,
  rfcom_lte_band_type                               rf_band,
  boolean                                           on_off,
  rf_buffer_intf*                                   script_buf,
  rf_device_execution_type                          exec_type,
  rfdevice_trx_timer_fixup_type                     (*ptr_timer_fixup_token_arr)[RFDEVICE_TRX_MAX_TIMER_FIXUPS_PER_SCRIPT],
  uint8*                                            num_timer_fixups,
  rfcom_lte_cat_type                                lte_cat

);

/*----------------------------------------------------------------------------*/
boolean rfdevice_lte_rx_toggle_pll_and_sigpath
(
  rfm_device_enum_type                              device,
  rfcom_lte_band_type                               rf_band,
  rfdevice_trx_lte_rx_toggle_type                   toggle_type,
  rf_buffer_intf*                                   script_buf,
  rf_device_execution_type                          exec_type,
  rfdevice_trx_timer_fixup_type                     (*ptr_timer_fixup_token_arr)[RFDEVICE_TRX_MAX_TIMER_FIXUPS_PER_SCRIPT],
  uint8*                                            num_timer_fixups,
  rf_hal_bus_resource_type*                         script_type, 
  rfcom_lte_cat_type                                lte_cat
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_lte_tx_toggle_pll_and_sigpath
(
  rfm_device_enum_type                              device,
  rfdevice_trx_lte_tx_path_info_type*               current_path_info,
  boolean                                           on_off,
  rf_buffer_intf*                                   script_buf,
  rf_device_execution_type                          exec_type,
  rfdevice_trx_timer_fixup_type                     (*ptr_timer_fixup_token_arr)[RFDEVICE_TRX_MAX_TIMER_FIXUPS_PER_SCRIPT],
  uint8*                                            num_timer_fixups
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_lte_tx_get_rgi0_script
(
  rfm_device_enum_type                              rfm_device,
  rfdevice_trx_lte_tx_path_info_type*               current_path_info,
  rf_buffer_intf*                                   script_buf,
  rf_device_execution_type                          exec_type
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_lte_tx_get_rgi0_trigger_script
(
  rfm_device_enum_type                                  rfm_device,
  rfdevice_trx_lte_tx_path_info_type*                   current_path_info,
  rf_buffer_intf*                                       script_buf,
  rf_device_execution_type                              exec_type,
  rfdevice_trx_timer_fixup_type				(*ptr_timer_fixup_token_arr)[RFDEVICE_TRX_MAX_TIMER_FIXUPS_PER_SCRIPT],
  uint8*						num_timer_fixups
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_lte_tx_get_ffh_template
(
  rfm_device_enum_type                              rfm_device,
  rfcom_lte_band_type                               rf_band,
  rf_buffer_intf*                                   script_buf,
  rfdevice_trx_lte_ffh_template_type                template_type, 
  rfcom_lte_cat_type                                lte_cat
);

/*----------------------------------------------------------------------------*/
boolean rfdevice_lte_rx_get_ffh_template
(
  rfm_device_enum_type                              rfm_device,
  rfcom_lte_band_type                               rf_band,
  rf_buffer_intf*                                   script_buf,
  rfdevice_trx_lte_ffh_template_type                template_type, 
  rfcom_lte_cat_type                                lte_cat
);

boolean rfdevice_lte_tx_get_spectral_inversion
(
  rfm_device_enum_type                              rfm_device,
  rfdevice_lte_tx_spectral_inversion_type*          data
);

boolean rfdevice_lte_rx_get_ffh_data
(
  rfm_device_enum_type                                rfm_device,
  rfcom_lte_band_type                                 band,
  uint32                                              freq_khz,
  int32                                               dflo_offset_khz,
  rfcom_lte_cat_type                                  lte_cat,
  rfdevice_lte_ffh_data_type*                         ffh_data
);

boolean rfdevice_lte_tx_get_ffh_data
(
  rfm_device_enum_type                                rfm_device,
  rfdevice_trx_lte_tx_path_info_type*                 current_path_info,
  int32                                               dflo_offset_khz,
  rfdevice_lte_ffh_data_type*                         ffh_data
);

boolean rfdevice_lte_get_xogen_dcc_val
(
  rfdevice_trx_common_intf_class *cmn_dev,
  rf_buffer_intf* script,
  rfm_dcc_info_type *rfm_dcc_read_back
);


#ifdef __cplusplus
}
#endif 

#endif /*FEATURE_LTE*/
#endif /* RFDEVICE_INTERFACE_LTE_H */



