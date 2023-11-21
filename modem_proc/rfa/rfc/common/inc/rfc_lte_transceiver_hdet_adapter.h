#ifndef RFC_LTE_TRANSCEIVER_HDET_ADAPTER_H
#define RFC_LTE_TRANSCEIVER_HDET_ADAPTER_H
/*!
   @file
   rfc_lte_transceiver_hdet_adapter.h

   @brief
   LTE HDET Adapter implementation for Transceivers
   lte HDET driver header file

*/

/*===========================================================================

Copyright (c) 2013- 2019 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: 

when       who   what, where, why
--------   ---   -------------------------------------------------------------------
01/15/19   am    Added NB tone_spacing as a parameter to required functions and function calls
09/11/18   up    Added function process_read_script
09/05/18   am    Added parameters lte_category and tx_chan_freq to APIs 
02/22/18   up    Modified for SDR105, FR 46604: NBPDET support on SDR105 
                 Updated do_hdet_read to take band as input
01/29/13   pv    Helper API for Execution type to boolean
01/15/13   pv    Initial version

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#ifdef FEATURE_LTE

#include "rfdevice_hdet_lte.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

class rfc_lte_transceiver_hdet_adapter: public rfdevice_hdet_lte
{
  public:

  rfc_lte_transceiver_hdet_adapter(const rfdevice_lte_tx_type* tx_device);
  
  // Destructor
  ~rfc_lte_transceiver_hdet_adapter();

  void init();
  
  boolean hdet_init
  (
    rf_buffer_intf *buff_obj_ptr, 
    rf_device_execution_type dev_action,
    uint32 tx_freq,
    rfm_device_enum_type device,
    rfcom_lte_band_type band
  );

  boolean enable_hdet
  (
    rf_buffer_intf *buff_obj_ptr, 
    rf_device_execution_type dev_action,
    rfdevice_lte_hdet_enable_type *hdet_data,
    rfm_device_enum_type device,
    rfcom_lte_band_type band,
    rfcom_lte_cat_type lte_cat,
    uint32 tx_freq_khz,
    rfdevice_lte_nb_tone_spacing_type tone_spacing
  );

  boolean disable_hdet
  (
    rf_buffer_intf *buff_obj_ptr, 
    rf_device_execution_type dev_action,
    rfm_device_enum_type device,
    rfcom_lte_band_type band,
    rfcom_lte_cat_type lte_cat,
    uint32 tx_freq_khz,
    rfdevice_lte_nb_tone_spacing_type tone_spacing
  );

  boolean do_full_hdet
  (
    rf_buffer_intf *buff_obj_ptr, 
    rf_device_execution_type dev_action,
    rfdevice_lte_hdet_settings_type *hdet_data,
    rfm_device_enum_type device,
    rfcom_lte_band_type band,
    rfcom_lte_cat_type lte_cat,
    uint32 tx_freq_khz,
    rfdevice_lte_nb_tone_spacing_type tone_spacing
  );

  boolean do_hdet_read
  (
    rf_buffer_intf *buff_obj_ptr, 
    rf_device_execution_type dev_action,
    rfdevice_lte_hdet_settings_type *hdet_data,
    rfcom_lte_band_type band,
    rfcom_lte_cat_type lte_cat,
    rfdevice_lte_nb_tone_spacing_type tone_spacing
  );

  boolean get_hdet_setting
  (
    rf_buffer_intf *buff_obj_ptr, 
    rf_device_execution_type dev_action,
    rfdevice_lte_hdet_therm_data_type* lte_hdet_therm_settings,
    rfdevice_lte_script_data_type* script_data_ptr,
    rfm_device_enum_type device,
    rfcom_lte_band_type band,
    rfcom_lte_cat_type lte_cat,
    uint32 tx_freq_khz,
    rfdevice_lte_nb_tone_spacing_type tone_spacing
  );
  boolean get_hdet_val
  (
    rf_buffer_intf *buff_obj_ptr, 
    rf_device_execution_type dev_action,
    rfdevice_lte_hdet_therm_data_type* lte_hdet_therm_settings,
    rfm_device_enum_type device,
    rfcom_lte_band_type band
  );

  boolean process_read_script
  (
    rf_hal_bus_rffe_type* raw_script,
    rfcom_lte_cat_type lte_cat,
    rfdevice_lte_hdet_settings_type *hdet_data,
    rfdevice_lte_nb_tone_spacing_type tone_spacing
  );

  protected:
     /* Helper functions for Rfdevice Script data conversion */
  boolean helper (rf_device_execution_type dev_action);
 
  private:

  /*! Parameter to hold the HDET mode */
  rfdevice_lte_hdet_power_mode_type current_hdet_mode;

  /*! common LTE Tx Instance */
  const rfdevice_lte_tx_type *tx_device;
  
};

#endif /* FEATURE_LTE*/
#endif /* RFC_LTE_TRANSCEIVER_HDET_ADAPTER_H */
