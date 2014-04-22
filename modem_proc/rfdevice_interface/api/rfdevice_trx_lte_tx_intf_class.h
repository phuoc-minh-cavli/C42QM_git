#ifndef RFDEVICE_TRX_LTE_TX_H
#define RFDEVICE_TRX_LTE_TX_H

/*!
  @file
  rfdevice_trx_lte_tx_intf_class.h

  @brief
  Base class for LTE RF device interface 

*/

/*===========================================================================

Copyright (c) 2012 - 2018 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //source/qcom/qct/modem/rfdevice/interface/main/latest/api/rfdevice_trx_lte_tx_intf_class.h#7

when       who    what, where, why
--------   ---   -------------------------------------------------------------------
08/09/18   bm     Updated get_rgi0_trigger_script api to provide timer token info
08/08/18   up     Removed redundant interfaces:get_ffh_reg_bank_sel
08/07/18   up     Modified API get_num_pll_option
07/26/18   up     dflo_offset_khz in FFH data fixup API is made as int32, added API: 
                  get_num_pll_option
07/25/18   up     Modified get_ffh_reg_bank_sel
07/19/18   up     Added new API rfdevice_lte_tx_get_rgi0_script.
07/02/18   up     Added new API get_ffh_data
06/27/18   uo     Modified the API set_band_data
06/25/18   up     Updated API get_ffh_reg_bank_sel
06/25/18   up     Added API get_ffh_reg_bank_sel for LTE RX
06/20/18   up     Modified the API toggle_sigpath to take lte_cat as input param.
06/18/18   up     Modified get_bbf_resp to tale lte_cat as input
04/25/18   up     Removed the LTE TX FFH API get_ffh_parameters
02/14/18   bm     Updated interface for get_txagc_data()
02/06/18   up     Mofified for SDR105, FR 46405: SDR105 TRX (IOTRF) 
                  renamed rfdevice_rxtx_common_class to rfdevice_trx_common_intf_class
05/30/17   smi    TxPLL setting changes for NBIOT
01/11/17   ry     Changes for Tx PLL fast hop for LTE CatM
12/02/16   cv     Merging MTPL and AFC for UL Hopping
11/08/16   ry     Timing updates for LTE Cat M1
07/20/16   ry     Added APIs to build the Rx and Tx PLL scripts
07/27/15   bm     Renamed get_single_rb_scripts() to get_4fmod_scripts()
                  Added new API get_dac_backoff_value() 
06/01/15   bm     Added API to get single_rb_scripts from WTR device
05/20/15   dps    Added support for power logging using rfdevice_wtr_config_bitmask
12/10/14   aks    Added API to get max BW supported by Tx port
08/29/14   aks    Compiler Warning fix 
08/26/14   aks    Updated get_tune_script to accept cell_idx as an argument
08/18/14   aks    Added API to get Tx tune as preload and trigger script 
08/14/14   aks    Updated Doxygen Comments 
08/01/14   ndb    Added support for extended EARFCN for LTE
08/01/14   aks    Updated Tx Disable to accept rf_band as an input argument
07/07/14   aks    Updated cmd_dispatch to accept rf_band as an input argument
07/01/14   aks    Updated tune_to_chan to accept a flag to disable spur mitigation 
4/29/14    xsh    Modify the fbrx rsb cal API to ensure calibrated rsb cal can pass back
01/02/14   aks    Removed set_port from rfdevice_trx_lte_tx class 
12/09/13   rp     Added an interface to retrieve common device pointer associated with 
                  given tech trx class.
11/20/13   aks    Device driver updates to enable AFC on all bands
11/15/13   aks    Device driver updates to enable AFC
11/07/13   rp     Added and API to return common device class pointer.
10/25/13   aka    LTE FB path RSB cal
08/20/13   aks    Initial version
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "comdef.h"
#include "rf_buffer_intf.h"

#ifdef  FEATURE_LTE
#include "rfdevice_lte_type_defs.h"
#include "rfdevice_type_defs.h"
#include "rflte_ext_mc.h"
#include "rfc_common.h"
#ifdef __cplusplus
#include "rfdevice_class.h"
#include "rfdevice_trx_common_intf_class.h"


class rfdevice_trx_lte_tx_intf_class:public rfdevice_class 
{
  public:

  /* Constructor */
  rfdevice_trx_lte_tx_intf_class();
  rfdevice_trx_lte_tx_intf_class(  uint16 manufacturer_id, uint16 product_id, uint16 product_rev);



  /*----------------------------------------------------------------------------*/
  /* Destructor */
  ~rfdevice_trx_lte_tx_intf_class();
  

  virtual boolean get_tune_script
  (
    rfm_device_enum_type                                rfm_device,
    uint32                                              tx_freq_khz,
    int32                                               rx_freq_error_hz,
    rfcom_lte_band_type                                 rf_band,
    rfcom_lte_bw_type                                   rf_bw,
    rf_buffer_intf*                                     buf,
    rf_device_execution_type                            exec_type,
    rfcom_lte_cat_type                                  lte_cat
  ) = 0;

  /*----------------------------------------------------------------------------*/
  virtual rfdevice_tx_saw_port_type get_port_type(
    rfm_device_enum_type                                rfm_device,
    rfcom_lte_band_type                                 band 
  )=0;

  /*----------------------------------------------------------------------------*/
  virtual boolean toggle_pll_and_sigpath
  (
    rfm_device_enum_type                                rfm_device,
    rfdevice_trx_lte_tx_path_info_type*                 current_path_info,
    boolean                                             on_off,
    rf_buffer_intf*                                     script_buf,
    rf_device_execution_type                            exec_type,
    rfdevice_trx_timer_fixup_type                       (&timer_fixup_token_arr)[RFDEVICE_TRX_MAX_TIMER_FIXUPS_PER_SCRIPT],
    uint8&                                              num_timer_fixups
  ) = 0;

  /*----------------------------------------------------------------------------*/
  virtual boolean toggle_sigpath
  (
    rfm_device_enum_type                                rfm_device,
    rfcom_lte_band_type                                 rf_band,
    boolean                                             on_off,
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
    uint32                                              tx_pa_lut_map
  )=0;

  /*----------------------------------------------------------------------------*/
  virtual const int8* get_bbf_resp(
    rfcom_lte_band_type                                 rf_band,
    rfcom_lte_bw_type                                   rfcom_bw,
    uint16*                                             bbf_resp_arr_size,
    rfcom_lte_cat_type                                  lte_cat
  )=0;

  /*----------------------------------------------------------------------------*/
  virtual boolean get_timing_info
  (
    rfcom_lte_bw_type                                   tx_bw,
    rfdevice_trx_lte_tx_event_timing_info_type&         timing_info
  ) = 0;

  /*----------------------------------------------------------------------------*/
  virtual boolean get_tx_pll_script
  (
    rfm_device_enum_type                                device,
    rfcom_lte_band_type                                 rf_band,
    int32                                               tx_freq_error_in_hz,
    uint64                                              tx_freq_hz, 
    rfcom_lte_cat_type                                  lte_cat,
    rfdevice_lte_script_data_type*                      script_data_ptr
  ) = 0;

  /*----------------------------------------------------------------------------*/
  virtual  boolean get_tx_pll_freq_offset_script(
    uint64                                              tx_freq_hz,	
    int32                                               tx_freq_error_in_hz, 
    rfcom_lte_band_type                                 rf_band,
    boolean                                             is_afc_flag,
    rfcom_lte_cat_type                                  lte_cat,
    rfdevice_lte_script_data_type*                      script_data_ptr
   ) = 0;

  /*----------------------------------------------------------------------------*/
  virtual boolean get_max_bw_supported(
    rfm_device_enum_type                                rfm_device,
    rfcom_lte_band_type                                 rf_band,
    uint32*                                             max_bw_khz
  )=0;

  /*----------------------------------------------------------------------------*/
  virtual rfdevice_trx_common_intf_class* get_common_device_pointer() = 0;

  virtual boolean get_rgi0_script
  (
    rfm_device_enum_type                                rfm_device,
    rfdevice_trx_lte_tx_path_info_type*                 current_path_info,
    rf_buffer_intf*                                     script_buf,
    rf_device_execution_type                            exec_type
  ) = 0;

  virtual boolean get_rgi0_trigger_script
  (
    rfcom_lte_band_type                                   rf_band,
    rf_buffer_intf*                                       script_buf,
    rf_device_execution_type                              exec_type,
    rfcom_lte_cat_type                                    lte_cat,
    rfdevice_trx_timer_fixup_type 			  (&timer_fixup_token_arr)[RFDEVICE_TRX_MAX_TIMER_FIXUPS_PER_SCRIPT],
    uint8&						  num_timer_fixups
  ) = 0;

  virtual boolean get_ffh_template
  (
    rfm_device_enum_type                                rfm_device,
    rfcom_lte_band_type                                 rf_band,
    rfdevice_trx_lte_ffh_template_type                  template_type,
    rf_buffer_intf*                                     script_buf, 
    rfcom_lte_cat_type                                  lte_cat
  ) = 0;

  virtual boolean get_spectral_inversion
  (
      rfm_device_enum_type                              rfm_device,
      rfdevice_lte_tx_spectral_inversion_type*          data
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

};
#else

struct rfdev_lte_tx;
typedef struct rfdev_lte_rx rfdevice_trx_lte_tx_intf_class;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get rfdevice_rxtx_common_class pointer associated with this instance of device.
  
  @return 
  Pointer to rxtx common device associated with current instance of device.
*/

rfdevice_trx_common_intf_class* 
rfdevice_lte_tx_get_common_device_pointer(rfdevice_trx_lte_tx_intf_class* tx_device);


#endif /* #ifdef __cplusplus */
#endif /* #ifdef FEATURE_LTE */
#endif /* __RFDEVICE_TRX_LTE_TX_H_INCL__ */

