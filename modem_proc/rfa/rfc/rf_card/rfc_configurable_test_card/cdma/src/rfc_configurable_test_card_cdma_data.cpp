/*!
   @file
   rfc_configurable_test_card_cdma_data.cpp

   @brief
   This file contains implementation the rfc_configurable_test_card_cdma_data class, which can be used to
   configure the rf-card related data as required.

*/

/*===========================================================================

Copyright (c) 2011 - 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2020/04/16 12:27:00 $ $Author: pwbldsvc $
$Header: //components/rel/rfa.mpss/3.10/rfc/rf_card/rfc_configurable_test_card/cdma/src/rfc_configurable_test_card_cdma_data.cpp#1 $

when       who     what, where, why
------------------------------------------------------------------------------- 
03/13/13   aro     Updated Data Config query function to return right
                   table
03/11/13   sar     Updated file for APQ, GNSS only target.
03/05/13   vrb     Replacing RFC accessor param init_req boolean by req enum
02/09/13   sr      Added Destructor.
11/07/12   sr      Initial version.

============================================================================*/
/*=============================================================================
                           INCLUDE FILES
=============================================================================*/
#include "comdef.h"
#ifdef FEATURE_CDMA1X
#include "rfc_configurable_test_card_cmn_data.h" 
#include "rfc_common.h" 
#include "rfc_configurable_test_card_cdma_data.h"
#include "rfm_cdma_band_types.h" 


static rfc_sig_info_type *rfc_configurable_test_card_cdma_init_data[RFC_CONFIG_RXTX_MAX] = {0};
static rfc_sig_info_type *rfc_configurable_test_card_cdma_band_data[RFM_MAX_DEVICES][RFC_CONFIG_RXTX_MAX][RFCOM_NUM_1X_BANDS] = {{{0}}};
static rfc_device_info_type *rfc_configurable_test_card_cdma_init_device_info[RFC_CONFIG_RXTX_MAX] = {0};
static rfc_device_info_type *rfc_configurable_test_card_cdma_band_device_info[RFM_MAX_DEVICES][RFC_CONFIG_RXTX_MAX][RFCOM_NUM_1X_BANDS] = {{{0}}};


rfc_cdma_data * rfc_configurable_test_card_cdma_data::get_instance()
{
  if (rfc_cdma_data_ptr == NULL)
  {
    rfc_cdma_data_ptr = (rfc_cdma_data *)new rfc_configurable_test_card_cdma_data();
  }
  return( (rfc_cdma_data *)rfc_cdma_data_ptr);
}

//constructor
rfc_configurable_test_card_cdma_data::rfc_configurable_test_card_cdma_data()
  :rfc_cdma_data()
{
}


// Destructor
rfc_configurable_test_card_cdma_data::~rfc_configurable_test_card_cdma_data()
{
  // nothing to do for now
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the rfc_data for cdma tech based on specified configuration parameters.

  @details
  This function returns the rfc_data for cdma tech based on specified configuration parameters.
 
  @param
  'cfg' : configuration parameters for which rfc_data to be retrived.
  'ptr' : ptr to the rfc_data.
 
  @retval
  TRUE - if the rfc_data is available or FALSE if error.
*/
boolean rfc_configurable_test_card_cdma_data::sig_cfg_data_get( rfc_cfg_params_type *cfg, rfc_sig_cfg_type **ptr )
{

  boolean ret_val = FALSE;

  if (NULL==ptr)
  {
    return FALSE;
  }

  *ptr = NULL;

  if (NULL==cfg)
  {
    return FALSE;
  }

  if ( ( cfg->rx_tx < RFC_CONFIG_RXTX_INVALID ) && ( cfg->req == RFC_REQ_INIT ) )
  { 
    if (rfc_configurable_test_card_cdma_init_data[cfg->rx_tx] != NULL)
    {
      *ptr = (rfc_sig_cfg_type *)rfc_configurable_test_card_cdma_init_data[cfg->rx_tx]->cfg_sig_list;  
      ret_val = TRUE; 
    }
  }
  else if ( (cfg->logical_device < RFM_MAX_DEVICES) && (cfg->rx_tx < RFC_CONFIG_RXTX_INVALID) && (cfg->band < RFCOM_NUM_1X_BANDS) )
  {
    if (rfc_configurable_test_card_cdma_band_data[cfg->logical_device][cfg->rx_tx][cfg->band] != NULL)
    {
      *ptr = (rfc_sig_cfg_type *)(rfc_configurable_test_card_cdma_band_data[cfg->logical_device][cfg->rx_tx][cfg->band]->cfg_sig_list);
      ret_val = TRUE;
    }
  }

  return ret_val;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the device_info table for cdma tech based on specified configuration
  parameters.

  @details
  This function returns the device_info_table for cdma tech based on specified configuration
  parameters.
 
  @param
  'cfg' : configuration parameters for which rfc_data to be retrived.
  'ptr' : ptr to the device_info_table.
 
  @retval
  TRUE - if the rfc_data is available or FALSE if error.
*/
boolean rfc_configurable_test_card_cdma_data::devices_cfg_data_get( rfc_cfg_params_type *cfg, rfc_device_info_type **ptr )
{

  boolean ret_val = FALSE;

  if (NULL==ptr)
  {
    return FALSE;
  }

  *ptr = NULL;

  if (NULL==cfg)
  {
    return FALSE;
  }

  if ( ( cfg->rx_tx < RFC_CONFIG_RXTX_INVALID ) && 
       ( cfg->req == RFC_REQ_INIT ) )
  { 
    if (rfc_configurable_test_card_cdma_init_device_info[cfg->rx_tx] != NULL)
    {
      *ptr = rfc_configurable_test_card_cdma_init_device_info[cfg->rx_tx];  
      ret_val = TRUE; 
    }
  }
  else if ( (cfg->logical_device < RFM_MAX_DEVICES) && 
            (cfg->rx_tx < RFC_CONFIG_RXTX_INVALID) && 
            (cfg->band < RFCOM_NUM_1X_BANDS) )
  {
    if (rfc_configurable_test_card_cdma_band_device_info[cfg->logical_device][cfg->rx_tx][cfg->band] != NULL)
    {
      *ptr = (rfc_configurable_test_card_cdma_band_device_info[cfg->logical_device][cfg->rx_tx][cfg->band]);
      ret_val = TRUE;
    }
  }

  return ret_val;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function sets the rfc_data for cdma tech based on specified configuration parameters.

  @details
  This function sets the rfc_data for cdma tech based on specified configuration parameters.
 
  @param
  'cfg' : configuration parameters for which rfc_data to be stored.
  'ptr' : ptr to the rfc_data.
 
  @retval
  TRUE - if the rfc_data is stored or FALSE if error.
*/
boolean rfc_configurable_test_card_cdma_data::sig_cfg_data_set( rfc_cfg_params_type *cfg, rfc_sig_info_type *ptr )
{

  boolean ret_val = FALSE;

  if (NULL==cfg)
  {
    return FALSE;
  }

  if ( ( cfg->rx_tx < RFC_CONFIG_RXTX_INVALID ) && ( cfg->req == RFC_REQ_INIT ) )
  { 
    rfc_configurable_test_card_cdma_init_data[cfg->rx_tx] = ptr;
    ret_val = TRUE; 
  }
  else if ( (cfg->logical_device < RFM_MAX_DEVICES) && (cfg->rx_tx < RFC_CONFIG_RXTX_INVALID) && (cfg->band < RFCOM_NUM_1X_BANDS) )
  {
    rfc_configurable_test_card_cdma_band_data[cfg->logical_device][cfg->rx_tx][cfg->band] = ptr;
    ret_val = TRUE;
  }

  return ret_val;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function sets the device info. table for cdma tech based on specified configuration
  parameters.

  @details
  This function sets the device info. table for cdma tech based on specified configuration
  parameters.
 
  @param
  'cfg' : configuration parameters for which device info. table to be stored.
  'ptr' : ptr to the rfc_data.
 
  @retval
  TRUE - if the device info table is stored or FALSE if error.
*/
boolean rfc_configurable_test_card_cdma_data::devices_cfg_data_set( rfc_cfg_params_type *cfg, rfc_device_info_type *ptr )
{

  boolean ret_val = FALSE;

  if (NULL==cfg)
  {
    return FALSE;
  }

  if ( ( cfg->rx_tx < RFC_CONFIG_RXTX_INVALID ) && ( cfg->req == RFC_REQ_INIT ) )
  { 
    rfc_configurable_test_card_cdma_init_device_info[cfg->rx_tx] = ptr;
    ret_val = TRUE; 
  }
  else if ( (cfg->logical_device < RFM_MAX_DEVICES) && (cfg->rx_tx < RFC_CONFIG_RXTX_INVALID) && (cfg->band < RFCOM_NUM_1X_BANDS) )
  {
    rfc_configurable_test_card_cdma_band_device_info[cfg->logical_device][cfg->rx_tx][cfg->band] = ptr;
    ret_val = TRUE;
  }

  return ret_val;
}

#endif /* FEATURE_CDMA1X */
