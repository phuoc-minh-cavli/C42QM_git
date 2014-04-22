#ifndef RFDEVICE_TRX_LTE_RX__H
#define RFDEVICE_TRX_LTE_RX__H

/*!
  @file
  rfdevice_trx_lte_rx_intf_class.h

  @brief
  Base class for LTE RF device interface 

*/

/*===========================================================================

Copyright (c) 2013 - 2019 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //components/rel/rfdevice_interface.mpss/1.40/api/rfdevice_trx_lte_rx_intf_class.h#1 $
when       who    what, where, why
--------   ---   -------------------------------------------------------------------
02/06/19   up    Input frequency unit is changed to hz in get_rx_pll_freq_offset_script
08/13/18   up    Added frequency param to the API get_num_pll_option
08/08/18   up    Removed redundant interface:get_ffh_reg_bank_sel
07/26/18   up    dflo_offset_khz in FFH data fixup API is made as int32, added API: 
                 get_num_pll_option
07/25/18   up    Modified get_ffh_reg_bank_sel
07/02/18   up    Added new API get_ffh_data
06/27/18   uo    Modified the API set_band_data
06/25/18   up    Updated API get_ffh_reg_bank_sel
06/25/18   up    Added API get_ffh_reg_bank_sel for LTE RX
06/20/18   up    Modified the API toggle_sigpath to take lte_cat as input param.
04/25/18   up    Added RX FFH API: get_ffh_template
02/13/2018 up    nominal_freq_hz in get_afc_script is renamed to nominal_freq_khz
02/06/18   up    Mofified for SDR105, FR 46405: SDR105 TRX (IOTRF) 
                 renamed rfdevice_rxtx_common_class to rfdevice_trx_common_intf_class
11/10/16   ry    Timing updates for LTE Cat M1
11/10/16   smi   Changes to support PLL fast lock for LTE CATM
08/12/16   ry    Added APIs to build the Rx and Tx PLL scripts
09/16/15   sd    Interface changes for Rx AGC reload
05/20/15   dps   Added support for power logging using rfdevice_wtr_config_bitmask
05/15/15   ry    Device interface updates for RFC re-design for split Rx/Tx
02/25/15   sma   Scell tear down changes for CA NC
01/08/15   aks   Updated get_gain_default_pwr_val to accept operation_mode as an input  
12/16/14   aks   Updated exit and get_tune_script APIs to accept an array 
                 through which they can enforce retune of an active Rx chain
12/04/14   dps   Added support for RxRSB cal data from NV
11/13/14   zg    Added new API of get_vco_path
11/04/14   aks   Updates to LTE CB API for Spur Mitigation
08/29/14   aks   Compiler Warning fix 
08/28/14   shb   Deprecated API to return critical_section_ptr
08/26/14   aks   Updated get_tune_script to accept cell_idx as an argument
08/26/14   aks   Added API to check if addition of an Rx Chain requires retune of an existing
                 Rx/Tx Chain
08/14/14   aks   Added API to get the default gain offsets for each gain state
08/14/14   aks   Updated Doxygen Comments 
08/14/14   aks   Added API to get the default offsets for each gain state
08/08/14   sma   Added commit to efs for writing RSB data once 
08/05/14   sma   Changed the default gain state to gain state 2 for Rx RSB 
08/01/14   ndb   Added support for extended EARFCN for LTE
07/30/14   aks   Added API to return tune Script as separate Preload and Trigger Scripts 
07/16/14   aks   Added API to get AFC adjusted Notch Frequency
07/07/14   aks   Updated Rx Cmd Dispatch to accept rf_band as an input argument
07/07/14   aks   Updated tune_to_chan to accept Call Back API from tech driver 
                 for LTE CA Spur Mitigation
07/01/14   aks   Updated tune_to_chan to accept a flag to disable spur mitigation
07/01/14   aks   Added rx_agc_gain_state as an argument to get_rsb_coeff and calc_rsb_nv_params
06/12/14   aks   Added API to get AFC adjusted Notch Frequency
06/06/14   aks   Updated API get_lna_gain_script to accept intra_band_ca_flag 
                 as an argument
03/12/14   npi   Check for Rx intraband CA support
12/17/13   rp    Added an interface to retrieve common device pointer associated with 
                 given tech trx class.
12/16/13   aks   Added new API to return RX_EN script 
11/24/13   aks   Added arg in device IRAT API for startup vs cleanup
11/19/13   aks   Overloaded function build_meas_script to support IRAT on Bolt+WTR3925
11/18/13   aks   Added a new API to return LNA gain script
11/07/13   rp    Added and API to return common device class pointer.
09/26/13   kab   Fix for using stg_is_bad_fuse value from actual STG involved in RSB Cal
09/18/13   php   Pick LO Div ratio based Type 1 Spur present
09/17/13   aks   Added function rfdevice_lte_rx_get_critical_section_ptr
08/20/13   aks   Initial version
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "comdef.h"

#ifdef FEATURE_LTE
#include "rf_buffer_intf.h"
#include "rfdevice_lte_type_defs.h"
#include "rfdevice_type_defs.h"
#include "rfc_common.h"

#ifdef __cplusplus
#include "rfdevice_class.h"
#include "rfdevice_trx_common_intf_class.h"

class rfdevice_trx_lte_rx_intf_class: public rfdevice_class 
{
public:

  /* Constructor */
  rfdevice_trx_lte_rx_intf_class();
  rfdevice_trx_lte_rx_intf_class(  uint16 manufacturer_id, uint16 product_id, uint16 product_rev);


  /* Destructor */
  ~rfdevice_trx_lte_rx_intf_class();

virtual boolean get_tune_script(
    rfm_device_enum_type rfm_device,
    rfcom_lte_band_type                                 rf_band,
    rfcom_lte_bw_type                                   rf_bw,
    uint32                                              freq_khz,
    rf_buffer_intf*                                     script_buf,
    rf_device_execution_type                            exec_type,
    rfcom_lte_cat_type                                  lte_cat,
    rfdevice_lte_carrier_info_type*                     rf_lte_carrier_info,
    rfdevice_pll_truncation_error_info_type*            pll_trunc_error_info
  )=0;

virtual boolean get_tune_trigger_script
  (
    rfm_device_enum_type				rfm_device,
    rfcom_lte_band_type 				rf_band,
    rf_buffer_intf* 					script_buf,
    rf_device_execution_type				exec_type,
    rfcom_lte_cat_type					lte_cat
  )=0;

  /*----------------------------------------------------------------------------*/
  virtual boolean get_rx_pll_freq_offset_script(
    uint64                                              rx_freq_hz,
    rfcom_lte_band_type                                 rf_band,
    rfdevice_lte_script_data_type*                      script_data_ptr,
    rfcom_lte_cat_type                                  lte_cat
  ) = 0;
  #if 0
  {
    RF_MSG( RF_ERROR, "get_rx_pll_freq_offset_script: "
                      "Derived class does not support this API!" );
    return FALSE;
  }
#endif

  /*----------------------------------------------------------------------------*/
  virtual boolean toggle_pll_and_sigpath
  (
    rfcom_lte_band_type                                 rf_band,
    rfdevice_trx_lte_rx_toggle_type                     toggle_type,
    rf_buffer_intf*                                     script_buf,
    rf_device_execution_type                            exec_type,
    rfdevice_trx_timer_fixup_type                       (&timer_fixup_token_arr)[RFDEVICE_TRX_MAX_TIMER_FIXUPS_PER_SCRIPT],
    uint8&                                              num_timer_fixups,
    rfcom_lte_cat_type                                  lte_cat
  ) = 0;

  /*----------------------------------------------------------------------------*/
  virtual boolean set_band_data (
    rfcom_lte_band_type                                 band, 
    uint16                                              rfc_port,
    uint32                                              rx_lna_gain_lut_map
  ) = 0;

  /*----------------------------------------------------------------------------*/
  virtual boolean get_timing_info
  (
    rfm_device_enum_type rfm_device,
    rfcom_lte_band_type                                 rf_band,
    rfdevice_trx_lte_rx_event_timing_info_type&         timing_info
  ) = 0;
  


  /*----------------------------------------------------------------------------*/
  virtual boolean get_lna_gain_script(
    rfcom_lte_band_type                                 rf_band,
    uint8                                               rx_agc_gain_state,
    rfdevice_lte_script_data_type*                      script_data_ptr
  ) = 0;

  /*----------------------------------------------------------------------------*/
  virtual boolean get_rsb_coeff(
    rfdevice_lte_rx_rsb_offset_type*                    rsb_data,
    rfm_device_enum_type                                rfm_device,
    uint8                                               rx_agc_gain_state=2
  ) = 0;
  

  /*----------------------------------------------------------------------------*/
  //UMESP: keep this for the time being, requires revisit once the sdr105 spreadsheets are avaialbe
  virtual boolean calc_rsb_nv_params( 
    rfm_device_enum_type                                rfm_dev,
    rfcom_lte_band_type                                 rf_band,
    rfcom_lte_bw_type                                   rx_bw,
    uint64                                              i_sq,
    uint64                                              q_sq,
    int64                                               i_q,
    uint16                                              therm_data,
    int16*                                              sin_theta_final,
    int16*                                              cos_theta_final, 
    int16*                                              gain_inv_final,
    uint8                                               stg_is_bad_fuse,
    uint8                                               rx_agc_gain_state
  )=0;

  /*----------------------------------------------------------------------------*/
   virtual boolean commit_rsb_data_to_efs (
    rfm_device_enum_type                                rfm_dev,
    rfcom_lte_band_type                                 rf_band, 
    rfdevice_rsb_cal_verfication_data_type*             calibrated_list
  ) = 0;

  /*----------------------------------------------------------------------------*/
  virtual rfdevice_trx_common_intf_class* get_common_device_pointer() = 0;

  /*----------------------------------------------------------------------------*/
// UMESP::based on Atuls input, handle this
  virtual boolean get_gain_default_pwr_val(
    rfm_device_enum_type                                rfm_device,
    rfcom_lte_band_type                                 rf_band,
    rfcom_lte_bw_type                                   rfcom_bw,
    rfdevice_lte_get_rx_gain_settings_type*             gain_settings
  )=0;


  /*----------------------------------------------------------------------------*/
  virtual rfdevice_rx_vco_path_enum_type get_vco_path(
    rfcom_lte_band_type                                 band
   ) = 0;

virtual boolean get_gain_fixup_data
(
  rfm_device_enum_type rfm_device,
  rfcom_lte_band_type                                   rf_band,
  rfcom_lte_bw_type                                     rf_bw,
  rfdevice_trx_rx_agc_data_per_gs_type                  (&gain_data_arr)[RFDEVICE_LTE_MAX_LNA_STATES], 
  uint16&                                               data_fixup_token,
  uint8&                                                data_fixup_lut_size_in_uint16,
  uint8&                                                num_wtr_gain_states,
  rfcom_lte_cat_type                                    lte_cat
)= 0;

virtual boolean get_afc_script
(
  rfm_device_enum_type                                  rfm_device,
  rfcom_lte_band_type                                   rf_band,
  uint32                                                nominal_freq_khz,
  int32                                                 rx_freq_offset_hz,
  rf_buffer_intf*                                       script_buf,
  rf_device_execution_type                              exec_type,
  rfcom_lte_cat_type                                    lte_cat
) = 0;

virtual boolean get_lna_phase_comp_data
(
  rfm_device_enum_type                                  rfm_device,
  rfcom_lte_band_type                                   rf_band,
  uint8                                                 num_gain_states,
  int16*                                                phase_arr_in_degree
) = 0;

virtual boolean get_wtr_capabilities
(
  rfcom_lte_band_type                                   rf_band,
  rfdevice_trx_lte_rx_capabilities&                     trx_capabilities
) = 0;


virtual boolean toggle_sigpath
(
  rfm_device_enum_type                                  rfm_device,
  rfcom_lte_band_type                                   rf_band,
  boolean                                               on_off,
  rf_buffer_intf*                                       script_buf,
  rf_device_execution_type                              exec_type,
  rfdevice_trx_timer_fixup_type                         (&timer_fixup_token_arr)[RFDEVICE_TRX_MAX_TIMER_FIXUPS_PER_SCRIPT],
  uint8&                                                num_timer_fixups,
  rfcom_lte_cat_type                                    lte_cat
) = 0;

virtual boolean get_ffh_template
(
  rfm_device_enum_type                                  rfm_device,
  rfcom_lte_band_type                                   rf_band,
  rfdevice_trx_lte_ffh_template_type                    template_type,
  rf_buffer_intf*                                       script_buf,
  rfcom_lte_cat_type                                    lte_cat
) = 0;

virtual boolean get_ffh_data
(
  rfcom_lte_band_type                                   band,
  uint32                                                freq_khz,
  int32                                                 dflo_offset_khz,
  rfcom_lte_cat_type                                    lte_cat,
  rfdevice_lte_ffh_data_type*                           ffh_data
) = 0;

virtual boolean get_num_pll_option
(
  rfcom_lte_band_type                                 band,
  uint32                                              freq_khz,
  rfcom_lte_cat_type                                  lte_cat,
  uint8&                                              num_pll_option,
  rfdevice_trx_pll_option_info_type*                  pll_option = NULL
) = 0;

private:
  rfdevice_lte_rx_type *rx_device;
};

#else

struct rfdev_lte_rx;
typedef struct rfdev_lte_rx rfdevice_trx_lte_rx_intf_class;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get rfdevice_rxtx_common_class pointer associated with this instance of device.
  
  @return 
  Pointer to rxtx common device associated with current instance of device.
*/
rfdevice_trx_common_intf_class* 
rfdevice_lte_rx_get_common_device_pointer(rfdevice_trx_lte_rx_intf_class* rx_device);


#endif /* #ifdef __cplusplus */
#endif /* %ifdef FEATURE_LTE */
#endif /* RFDEVICE_TRX_LTE_RX_H */

