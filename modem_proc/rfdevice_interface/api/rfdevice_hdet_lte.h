#ifndef RFDEVICE_HDET_LTE_H
#define RFDEVICE_HDET_LTE_H
/*!
   @file
   rfdevice_hdet_lte.h

   @brief
   APIs for various LTE HDET functionality supported by HDET device driver
   Enabling/Disabling/reading HDET, returning hdet parameters for device,
   generating HDET operation scripts for firmware operation.
*/

/*===========================================================================

Copyright (c) 2012-2019 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rfdevice_interface.mpss/1.40/api/rfdevice_hdet_lte.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
01/15/19   am      Added NB tone_spacing as a parameter to required functions.
09/11/18   up      Added new API process_read_script
02/16/18   up      Modified for SDR105, FR 46604: NBPDET support on SDR105
                   do_hdet_read is modified to take band as input param
06/16/14   sbm     HDET online brinup
01/15/13   pv      API updates for Transceiver compatibility.
11/29/12   av      Using proper featurization and removing featurization 
                    that is not needed
11/20/12   gvn     Featurize LTE for Triton 
10/20/12   kai     Initial version
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "rf_buffer_intf.h"
#include "rfdevice_class.h"

#ifdef FEATURE_LTE
#include "rfdevice_hdet_lte_intf.h"
#endif

extern "C"
{
#include "rfdevice_type_defs.h"
}

#ifdef FEATURE_LTE

class rfdevice_hdet_lte:public rfdevice_class
{
public:

/*----------------------------------------------------------------------------*/
/*!
  @brief
  function initializes HDET circuitry in LTE mode 

  @details
  This function initializes HDET circuitry in LTE mode i.e. programs the HW 
  with the required bandwidth

  @param buff_obj_ptr
  void pointer to CCS buffer object

  @param dev_action
  indicates if it is immediate write to hw or build scripts    

  @param tx_freq
  Indicates Tx freq.

  @param device
  Indicates Logical RFM device

  @param band
  Indicates RF LTE Band for Tx.

  @return
  boolean indicating pass or fail
*/

virtual boolean hdet_init
(
  rf_buffer_intf *buff_obj_ptr, 
  rf_device_execution_type dev_action,
  uint32 tx_freq,
  rfm_device_enum_type device,
  rfcom_lte_band_type band
) =0;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enable the HDET power detector circuit.

  @details
  This function is used if the driver wants to manually manage the HDET
  circuit to reduce overhead in readings or to insure the power detector
  has sufficient time to stabilize.
 
  @param buff_obj_ptr
  void pointer to CCS buffer object

  @param dev_action
  Indicates if it is immediate write to hw or build scripts
 
  @param hdet_data
  Indicates hdet params needed to enable HDET circuitry

  @param device
  Indicates Logical RFM device

  @param band
  Indicates RF LTE Band for Tx.
 
  @return
  boolean indicating pass or fail
*/
virtual boolean enable_hdet
(
  rf_buffer_intf *buff_obj_ptr, 
  rf_device_execution_type dev_action,
  rfdevice_lte_hdet_enable_type *hdet_data,
  rfm_device_enum_type device,
  rfcom_lte_band_type band,
  rfcom_lte_cat_type  lte_cat,
  uint32  tx_freq_khz,
  rfdevice_lte_nb_tone_spacing_type tone_spacing
) =0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Disable the HDET power detector circuit.

  @details
  This function is used if the driver wants to manually manage the HDET
  circuit to reduce overhead in readings or to insure the power detector
  has sufficient time to stabilize.
 
  @param buff_obj_ptr
  void pointer to CCS buffer object

  @param dev_action
  Indicates if it is immediate write to hw or build scripts
 
  @param device
  Indicates Logical RFM device

  @param band
  Indicates RF LTE Band for Tx.
 
  @return
  boolean indicating pass or fail
*/
virtual boolean disable_hdet
(
  rf_buffer_intf *buff_obj_ptr, 
  rf_device_execution_type dev_action,
  rfm_device_enum_type device,
  rfcom_lte_band_type band,
  rfcom_lte_cat_type  lte_cat,
  uint32  tx_freq_khz,
  rfdevice_lte_nb_tone_spacing_type tone_spacing
) =0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  do_full_hdet.
  
  @param buff_obj_ptr
  void pointer to CCS buffer object

  @param dev_action
  Indicates if it is immediate write to hw or build scripts
 
  @param hdet_data
  Indicates hdet params needed to enable HDET circuitry

  @param device
  Indicates Logical RFM device

  @param band
  Indicates RF LTE Band for Tx.
 
  @return
  boolean indicating pass or fail
*/
virtual boolean do_full_hdet
(
  rf_buffer_intf *buff_obj_ptr, 
  rf_device_execution_type dev_action,
  rfdevice_lte_hdet_settings_type *hdet_data,
  rfm_device_enum_type device,
  rfcom_lte_band_type band,
  rfcom_lte_cat_type lte_cat,
  uint32  tx_freq_khz,
  rfdevice_lte_nb_tone_spacing_type tone_spacing
) =0;

virtual boolean do_hdet_read
(
  rf_buffer_intf *buff_obj_ptr, 
  rf_device_execution_type dev_action,
  rfdevice_lte_hdet_settings_type *hdet_data,
  rfcom_lte_band_type band,
  rfcom_lte_cat_type lte_cat,
   rfdevice_lte_nb_tone_spacing_type tone_spacing
) =0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  get_hdet_setting
  
  @param buff_obj_ptr
  void pointer to CCS buffer object

  @param dev_action
  Indicates if it is immediate write to hw or build scripts
 
  @param lte_hdet_therm_settings
  Indicates hdet params for specific settings
 
  @param script_data_ptr
  Script data pointer which will return the script from RF device.

  @param device
  Indicates Logical RFM device

  @param band
  Indicates RF LTE Band for Tx.

  @return
  boolean indicating pass or fail
*/
virtual boolean get_hdet_setting
(
  rf_buffer_intf *buff_obj_ptr, 
  rf_device_execution_type dev_action,
  rfdevice_lte_hdet_therm_data_type* lte_hdet_therm_settings,
  rfdevice_lte_script_data_type* script_data_ptr,
  rfm_device_enum_type device,
  rfcom_lte_band_type band,
  rfcom_lte_cat_type  lte_cat,
  uint32  tx_freq_khz,
  rfdevice_lte_nb_tone_spacing_type tone_spacing
) =0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  get_hdet_val

  @param buff_obj_ptr
  void pointer to CCS buffer object

  @param dev_action
  Indicates if it is immediate write to hw or build scripts
 
   @param lte_hdet_therm_settings
  Indicates hdet params for specific settings

  @param device
  Indicates Logical RFM device

  @param band
  Indicates RF LTE Band for Tx.
 
  @return
  boolean indicating pass or fail
*/
virtual boolean get_hdet_val
(
  rf_buffer_intf *buff_obj_ptr, 
    rf_device_execution_type dev_action,
    rfdevice_lte_hdet_therm_data_type* lte_hdet_therm_settings,
  rfm_device_enum_type device,
  rfcom_lte_band_type band
) =0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  process_read_script

  @param raw_script
  Ponter to the read data script pointer

  @param lte_cat
  catm1/NB1

  @param hdet_data
  Holds the calculated HDET value

  @return
  boolean indicating pass or fail
*/

virtual boolean process_read_script
(
  rf_hal_bus_rffe_type* raw_script,
  rfcom_lte_cat_type lte_cat,
  rfdevice_lte_hdet_settings_type *hdet_data,
  rfdevice_lte_nb_tone_spacing_type tone_spacing
) = 0;

};
#endif /*FEATURE_LTE */
#endif /*RFDEVICE_HDET_LTE_H*/

