


/*!
  @file
  rfdevice_grfc_pa.cpp

  @brief
  Common interface for GRFC 3rd party PA devices

*/

/*==============================================================================

  Copyright (c) 2018 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //components/rel/rfdevice_pa.mpss/1.30/src/rfdevice_grfc_pa.cpp#1 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
08/28/18   vp      Initial version

==============================================================================*/

#include <stringl/stringl.h>

#ifdef __cplusplus
extern "C"
{
#endif

// Include C specific header files
#include "rfm_internal.h"

#ifdef __cplusplus
}/* extern "C"*/
#endif


#include "rf_buffer_intf.h"
#include "rf_hal_bus_types.h"
#include "rfcommon_msg.h"
#include "rfdevice_msg_log.h"
#include "rfcommon_time_profile.h"
#include "rf_rffe_common.h"
#include "rfdevice_grfc_pa.h"

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Constructor overlaoded for physical device support

*/
rfdevice_grfc_pa::rfdevice_grfc_pa
(
  rfdevice_physical_device* phy_obj_ptr,
  rfc_phy_device_info_type* phy_device_info, 
  rfc_logical_device_info_type* logical_device_info
)
{
  init_status = FALSE;

  if( 
      (NULL == phy_obj_ptr ) ||
      (NULL == phy_device_info) ||
      (NULL == logical_device_info ) ||
      (RFDEVICE_PA != logical_device_info->rf_device_type)
    )
  {
    RF_MSG( RF_ERROR, "rfdevice_grfc_pa(): PA device construction failed. Inavlid parameters passed" );
    init_status = FALSE;
    return;
  }

  //is_using_efs_data = FALSE;

  /* Copy over the config info */
  grfc_pa_device_info.rf_device_id             = logical_device_info->rf_device_id;
  grfc_pa_device_info.rf_device_type           = logical_device_info->rf_device_type;
  grfc_pa_device_info.rf_asic_id               = logical_device_info->rf_asic_id;
  grfc_pa_device_info.rf_device_comm_protocol  = phy_device_info->rf_device_comm_protocol;
  grfc_pa_device_info.bus[0]                   = phy_device_info->bus[0];
  grfc_pa_device_info.default_usid_range_start = phy_device_info->default_usid_range_start;
  grfc_pa_device_info.assigned_usid            = phy_device_info->assigned_usid;
  grfc_pa_device_info.group_id                 = phy_device_info->group_id;
  grfc_pa_device_info.manufacturer_id          = phy_device_info->manufacturer_id;
  grfc_pa_device_info.product_id               = phy_device_info->product_id;
  grfc_pa_device_info.product_rev              = phy_device_info->product_rev;
  grfc_pa_device_info.init_required            = phy_device_info->init_required;
  grfc_pa_device_info.associated_dac           = phy_device_info->associated_dac;

  /* Init device header */
  this->mfg_id = (uint16)grfc_pa_device_info.manufacturer_id;
  this->prd_id = (uint16)grfc_pa_device_info.product_id;
  this->rev_id = (uint16)grfc_pa_device_info.product_rev;
  this->rf_device_id = grfc_pa_device_info.rf_device_id;
  this->rf_device_type = grfc_pa_device_info.rf_device_type;
  this->instance_num  = logical_device_info->rf_asic_id;

  /*! Initialize default PA band data for all techs */
  memset(&this->band_map_grfc_tbl.cdma,    0xFF, sizeof(rfdevice_grfc_pa_rfc_tbl_type)*RFM_CDMA_MAX_BAND);
  memset(&this->band_map_grfc_tbl.gsm,     0xFF, sizeof(rfdevice_grfc_pa_rfc_tbl_type)*RFCOM_NUM_GSM_BANDS);
  memset(&this->band_map_grfc_tbl.lte,     0xFF, sizeof(rfdevice_grfc_pa_rfc_tbl_type)*RFCOM_NUM_LTE_BANDS);
  memset(&this->band_map_grfc_tbl.wcdma,   0xFF, sizeof(rfdevice_grfc_pa_rfc_tbl_type)*RFCOM_NUM_WCDMA_BANDS);
  memset(&this->band_map_grfc_tbl.tdscdma, 0xFF, sizeof(rfdevice_grfc_pa_rfc_tbl_type)*RFCOM_NUM_TDSCDMA_BANDS);

  /*initialize the pa_on and pa_gain flag to FALSE, pa_gain to 0*/
  set_pa_on_flag = FALSE;
  set_pa_gain_flag = FALSE;
  pa_gain = 0;
  pa_on_off = 0; 

  //On successfull pa device object construction, update the init flag
  init_status = TRUE;
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Destructor
*/
rfdevice_grfc_pa::~rfdevice_grfc_pa()
{
  
}

/*----------------------------------------------------------------------------*/
/*!
@brief
config() configures the GRFC PA.

@param pa_config_p
rfdevice_pa_config_type pointer to structure containing the tech, band and
antenna switch path to configure

@param buff_obj_ptr
void pointer to pass address of buffer script

@param execution_type
rf_device_execution_type indicates execute internally versus returning a script

@param script_timing
the timing used for script execution

@return
boolean indicating pass or fail
*/
boolean rfdevice_grfc_pa::config
(
  rfdevice_pa_cfg* pa_params_p,
  rf_buffer_intf *buff_obj_ptr,
  rf_device_execution_type dev_action,
  int16 script_timing
)
{
  RFDEVICE_LOG_MSG(LOG_RFA_DEVICE_PA_C, MSG_LEGACY_LOW, "rfdevice_grfc_pa::config API not implemented for GRFC PA component");
  return TRUE;
}/* rfdevice_grfc_pa::config */


/*----------------------------------------------------------------------------*/
/*!
@brief
set_pa_txagc() redirects to the set_txagc API depending on trigger type
because GRFC writes are non-trigger and need to go out at the time of trigger

@param pa_config_p
rfdevice_pa_config_type pointer to structure containing the tech, band and
antenna switch path to configure

@param buff_obj_ptr
void pointer to pass address of buffer script

@param execution_type
rf_device_execution_type indicates execute internally versus returning a script

@param script_timing
the timing used for script execution

@return
boolean indicating pass or fail
*/
boolean rfdevice_grfc_pa::set_pa_txagc
(
  rfdevice_pa_cfg* pa_params_p,
  rf_buffer_intf *script_buffer,
  rf_device_execution_type execution_type,
  int16 script_timing
)
{ 
  boolean status = TRUE;
  if (NULL != pa_params_p)
  {
    set_pa_gain_flag = TRUE;
    pa_gain = pa_params_p->pa_gain_range; // store the gain value in the class variable;
    //make sure the gain state value doesn't exceed the total number of states(exceeding the array alloation)
    if (RFDEVICE_MAX_PA_RX_GAIN_STATES<= pa_gain)
    {
      RF_MSG_1(RF_ERROR, "rfdevice_grfc_pa::set_pa_txagc: pa_gain value %d >= RFDEVICE_MAX_PA_RX_GAIN_STATES",
               pa_gain); 
      return FALSE;   
    }
    if(RFDEVICE_PA_TRIGGER_NONE != pa_params_p->trigger_type) 
    {   
      status &= this->set_pa_trigger(pa_params_p, script_buffer, execution_type, script_timing);
    }
  }
  else
  { 
     RF_MSG(RF_ERROR, "rfdevice_grfc_pa::set_pa_txagc  pa_params_p is null");
     status = FALSE;
  }
  return status;
}/* rfdevice_grfc_pa::set_pa_txagc */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  set_pa_on_off() redirects to the set_txagc API depending on trigger type
  because GRFC writes are non-trigger and need to go out at the time of trigger

  @param pa_config_p
  rfdevice_pa_config_type pointer to structure containing the tech, band and 
  antenna switch path to configure

  @param buff_obj_ptr
  void pointer to pass address of buffer script
 
  @param execution_type
  rf_device_execution_type indicates execute internally versus returning a script
 
  @param script_timing
  the timing used for script execution

  @return
  boolean indicating pass or fail
*/
boolean rfdevice_grfc_pa::set_pa_on_off
(
  rfdevice_pa_cfg* pa_params_p,
  rf_buffer_intf* script_buffer,
  rf_device_execution_type  execution_type,
  int16 script_timing
)
{
  boolean status = TRUE;
  
  if (NULL == pa_params_p)
  {
    RF_MSG(RF_ERROR, "rfdevice_grfc_pa::set_pa_on_off() pa_params_p is null");
    return FALSE;
  }
  
  RFDEVICE_LOG_MSG_3(LOG_RFA_DEVICE_PA_C, MSG_LEGACY_LOW, "[PA] set_pa_on_off(): Called with tech=%d, band=%d, rfm_device=%d",
                     pa_params_p->mode,
                     pa_params_p->band.lte_band,
                     pa_params_p->rfm_device);

  set_pa_on_flag = TRUE;
  pa_on_off = pa_params_p->on_off;
  if(RFDEVICE_PA_TRIGGER_NONE != pa_params_p->trigger_type) 
  {
    status &= this->set_pa_trigger(pa_params_p, script_buffer, execution_type, script_timing);
  }
  
  return status;
}/* rfdevice_grfc_pa::set_pa_on_off */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  GRFC PA set_pa_trigger() redirects to the other APIs depending on the internal state machine

  @param pa_config_p
  rfdevice_pa_config_type pointer to structure containing the tech, band and 
  antenna switch path to configure

  @param buff_obj_ptr
  void pointer to pass address of buffer script
 
  @param dev_action
  rf_device_execution_type indicates execute internally versus returning a script
 
  @param script_timing
  the timing used for script execution

	@return
	boolean indicating pass or fail
*/
boolean rfdevice_grfc_pa::set_pa_trigger
(
  rfdevice_pa_cfg* pa_params_p,
  rf_buffer_intf* script_buffer,
  rf_device_execution_type execution_type,
  int16 script_timing
)
{
  boolean status = TRUE;
  
  if (NULL == pa_params_p)
  {
    RF_MSG(RF_ERROR, "rfdevice_grfc_pa::set_pa_trigger() pa_params_p is null");
    return FALSE;
  }

  // Print API enter log
  RFDEVICE_LOG_MSG_3(LOG_RFA_DEVICE_PA_C, MSG_LEGACY_LOW, "[PA] trigger(): Called with tech=%d, band=%d, rfm_device=%d",
                     pa_params_p->mode,
                     pa_params_p->band.lte_band,
                     pa_params_p->rfm_device);

  if ((NULL != pa_params_p) && (RFDEVICE_PA_TRIGGER_NONE != pa_params_p->trigger_type)) 
  { 
    rf_time_tick_type start_tick = rf_time_get_tick();

    rfdevice_grfc_pa_rfc_tbl_type* rfc_grfc_tbl_ptr = NULL;
  /*==========================================================================*/
  /* CHOOSE SETTINGS */
  /*==========================================================================*/
    if(!get_band_map_tbl(pa_params_p->mode, pa_params_p->band, &rfc_grfc_tbl_ptr))
    { 
      /* use LTE band as LTE has the most bands */
      RF_MSG_3(RF_ERROR, "set_pa_trigger():GRFC PA %d Unable to get band config map info for tech %d band %d", 
                       this->grfc_pa_device_info.rf_device_id, pa_params_p->mode, pa_params_p->band.lte_band);
      status = FALSE;
    }
    else if (NULL !=rfc_grfc_tbl_ptr) 
    {
      if (FALSE != set_pa_on_flag || FALSE !=set_pa_gain_flag) 
      {
        if(!set_grfc(pa_params_p, script_buffer, execution_type, script_timing, rfc_grfc_tbl_ptr))
        { 
          RF_MSG_1(RF_ERROR, "set_pa_trigger(): GRFC PA %d failed to process gain settings", this->grfc_pa_device_info.rf_device_id);
          status = FALSE;
        }
      }
    }
  }
  else
  {
    RFDEVICE_LOG_MSG(LOG_RFA_DEVICE_PA_C, MSG_LEGACY_LOW, "[PA] trigger(): No valid trigger type requested");
    status = FALSE;
  }
  set_pa_on_flag = FALSE;
  set_pa_gain_flag = FALSE;
  return status;
}/* rfdevice_grfc_pa::set_pa_trigger */

/*----------------------------------------------------------------------------*/
/*!
@brief
Set the GRFC PA to its sleep logic

@param pa_config_p
rfdevice_pa_config_type pointer to structure containing the tech, band and
antenna switch path to configure

@param script_buffer
void pointer to pass address of buffer script

@param execution_type
rf_device_execution_type indicates execute internally versus returning a script

@param script_timing
the timing used for script execution

@return
boolean indicating pass or fail
*/
boolean rfdevice_grfc_pa::sleep
(
  rfdevice_pa_cfg* pa_params_p,
  rf_buffer_intf* script_buffer,
  rf_device_execution_type execution_type,
  int16 script_timing
)
{
  RFDEVICE_LOG_MSG(LOG_RFA_DEVICE_PA_C, MSG_LEGACY_LOW, "rfdevice_grfc_pa::sleep API not implemented for GRFC PA component");
  return TRUE;
}/* rfdevice_grfc_pa::sleep */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set the GRFC PA to its wakeup logic

  @param pa_config_p
  rfdevice_pa_config_type pointer to structure containing the tech, band and 
  antenna switch path to configure

  @param script_buffer
  void pointer to pass address of buffer script
 
  @param execution_type
  rf_device_execution_type indicates execute internally versus returning a script
 
  @param script_timing
  the timing used for script execution

  @return
  boolean indicating pass or fail
*/
boolean rfdevice_grfc_pa::wakeup
(
  rfdevice_pa_cfg* pa_params_p,
  rf_buffer_intf* script_buffer,
  rf_device_execution_type execution_type,
  int16 script_timing
)
{
  RFDEVICE_LOG_MSG(LOG_RFA_DEVICE_PA_C, MSG_LEGACY_LOW, "rfdevice_grfc_pa::wakeup API not implemented for GRFC PA component");
  return TRUE;
}/* rfdevice_grfc_pa::wakeup */


/*----------------------------------------------------------------------------*/
/*!
@brief
API to return the script size

@return
uint16 to indicate script size
*/
uint16 rfdevice_grfc_pa::get_script_size(void)
{
  RFDEVICE_LOG_MSG(LOG_RFA_DEVICE_PA_C, MSG_LEGACY_LOW, "rfdevice_grfc_pa::get_script_size API not implemented for GRFC PA component");
  return TRUE;
}/* rfdevice_grfc_pa::get_script_size */


/*----------------------------------------------------------------------------*/
/*!
   @brief
   Configure the pa band map for a given tech/band/sig_path
  
	 @param pa_config_p
	 rfdevice_pa_config_type pointer to structure containing the tech, band and
	 antenna switch path to configure

	 @params data_ptr
	 array of bitmasks to be populated in the band map

	 @params size
	 size of data_ptr array

	 @return
	 boolean indicating pass or fail
*/
boolean rfdevice_grfc_pa::set_band_map
(
  rfcom_mode_enum_type mode,
  rfcom_band_type_u band,
  int32* data_ptr,
  uint8 size
)
{
  boolean status = TRUE;
  rfdevice_grfc_pa_rfc_tbl_type* rfc_grfc_tbl_ptr = NULL;
  uint32 rf_gain_idx = 0;
  uint32 bitshift = RFDEVICE_GRFC_PA_GAIN0_BITSHIFT;
  
  // Validity check
  if ((mode == RFM_INVALID_MODE) || (data_ptr == NULL) || (size == 0))
  {
    RF_MSG_4(RF_ERROR, "[PA] set_band_map(): Invalid parameters passed: mode=%d, band=%d, data_ptr=0x%x, size=%d. Returning FALSE.",
             mode, band.lte_band, data_ptr, size);
    return FALSE;
  }

  // Print API enter log
  RFDEVICE_LOG_MSG_2(LOG_RFA_DEVICE_PA_C, MSG_LEGACY_LOW, "[PA] set_band_map(): Called with tech=%d, band=%d",
    mode,
    band.lte_band);

  if(!get_band_map_tbl(mode, band, &rfc_grfc_tbl_ptr))
  {
    /* use LTE band as LTE has the most bands */
    RF_MSG_3(RF_ERROR, "set_band_map():GRFC pa %d Unable to get band config map info for device id %d tech %d band %d", 
                       this->grfc_pa_device_info.rf_device_id, mode, band.lte_band);
    status = FALSE;
  }
  else if(rfc_grfc_tbl_ptr != NULL)
  {
    rfc_grfc_tbl_ptr->num_grfc = 0;
    while ( (data_ptr != NULL) &&
          (RFDEVICE_GRFC_PA_VALID_FLAG_GET(data_ptr[rfc_grfc_tbl_ptr->num_grfc]) != 0) &&
          (rfc_grfc_tbl_ptr->num_grfc< RFDEVICE_GRFC_PA_MAX_NUM_GRFCS))
    {
      rfc_grfc_tbl_ptr->rfc_grfc_pa_data[rfc_grfc_tbl_ptr->num_grfc].valid_flag    = RFDEVICE_GRFC_PA_VALID_FLAG_GET(data_ptr[rfc_grfc_tbl_ptr->num_grfc]);
      rfc_grfc_tbl_ptr->rfc_grfc_pa_data[rfc_grfc_tbl_ptr->num_grfc].grfc_type     = RFDEVICE_GRFC_PA_GRFC_TYPE_GET(data_ptr[rfc_grfc_tbl_ptr->num_grfc]);
      rfc_grfc_tbl_ptr->rfc_grfc_pa_data[rfc_grfc_tbl_ptr->num_grfc].grfc_num      = RFDEVICE_GRFC_PA_GRFC_NUMBER_GET(data_ptr[rfc_grfc_tbl_ptr->num_grfc]);
      rfc_grfc_tbl_ptr->rfc_grfc_pa_data[rfc_grfc_tbl_ptr->num_grfc].macro_select  = RFDEVICE_GRFC_PA_MACRO_SELECT_GET(data_ptr[rfc_grfc_tbl_ptr->num_grfc]);
      if (TRUE == RFDEVICE_GRFC_PA_MACRO_SELECT_GET(data_ptr[rfc_grfc_tbl_ptr->num_grfc]))
      {
        rfc_grfc_tbl_ptr->rfc_grfc_pa_data[rfc_grfc_tbl_ptr->num_grfc].pa_on_logic = RFDEVICE_GRFC_PA_ON_GET(data_ptr[rfc_grfc_tbl_ptr->num_grfc]);
        rfc_grfc_tbl_ptr->rfc_grfc_pa_data[rfc_grfc_tbl_ptr->num_grfc].pa_off_logic = RFDEVICE_GRFC_PA_OFF_GET(data_ptr[rfc_grfc_tbl_ptr->num_grfc]);
      }
      else
      {
        bitshift = RFDEVICE_GRFC_PA_GAIN0_BITSHIFT;
        for (rf_gain_idx = 0; rf_gain_idx< RFDEVICE_MAX_PA_RX_GAIN_STATES; rf_gain_idx++) 
        {
          rfc_grfc_tbl_ptr->rfc_grfc_pa_data[rfc_grfc_tbl_ptr->num_grfc].gain_states[rf_gain_idx] = RFDEVICE_GRFC_PA_GAIN_STATE_GET(data_ptr[rfc_grfc_tbl_ptr->num_grfc], bitshift);
          bitshift -= RFDEVICE_BITS_PER_GRFC_PA_GAIN_STATE;
        }
      }
      rfc_grfc_tbl_ptr->num_grfc++;
    }
  }
  return status;
}/* rfdevice_grfc_pa::set_band_map */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  The function that gets the pointer to the correct tech/band/sig_path in the band_map

	@param pa_config_p
	rfdevice_pa_config_type pointer to structure containing the tech, band and
	antenna switch path to configure

  @param rfc_grfc_tbl
  rfdevice_grfc_pa_rfc_tbl_type pointer to the tech/band/sig_path in the band map

  @return
  boolean indicating pass or fail
*/
boolean rfdevice_grfc_pa::get_band_map_tbl
(
  rfcom_mode_enum_type mode,
  rfcom_band_type_u band,
  rfdevice_grfc_pa_rfc_tbl_type **rfc_grfc_tbl
)
{
  boolean status = TRUE;
  switch (mode)
  {
#ifdef FEATURE_WCDMA
    case RFCOM_WCDMA_MODE:
      if(band.wcdma_band < RFCOM_NUM_WCDMA_BANDS)
      {
        *rfc_grfc_tbl = &(this->band_map_grfc_tbl.wcdma[band.wcdma_band]);
      }
      else
      {
        RF_MSG_1(RF_ERROR, "rfdevice_grfc_pa_get_band_map_grfc_tbl(): "
                           "GRFC pa rfdevice_grfc_pa_get_band_map_grfc_tbl invalid WCDMA band %d",
                           band.wcdma_band);    
        status = FALSE;
      }
      break;
#endif

#ifdef FEATURE_CDMA1X
    case RFCOM_1X_MODE:
    case RFCOM_1XEVDO_MODE:
      if (band.cdma_band < RFM_CDMA_MAX_BAND)
      {
        *rfc_grfc_tbl = &(this->band_map_grfc_tbl.cdma[band.cdma_band]);
      }
      else
      {
        RF_MSG_1(RF_ERROR, "rfdevice_grfc_pa_get_band_map_grfc_tbl():"
                           "GRFC pa rfdevice_grfc_pa_get_band_map_grfc_tbl invalid CDMA band %d",
                           band.cdma_band);  
        status = FALSE;
      }
      break;
#endif

#ifdef FEATURE_GSM
    case RFCOM_GSM_MODE:
      if (band.gsm_band < RFCOM_NUM_GSM_BANDS)
      {
        *rfc_grfc_tbl = &(this->band_map_grfc_tbl.gsm[band.gsm_band]);
      }
      else
      {
        RF_MSG_1(RF_ERROR, "rfdevice_grfc_pa_get_band_map_grfc_tbl():"
                           "GRFC pa rfdevice_grfc_pa_get_band_map_grfc_tbl invalid GSM band %d",
                           band.gsm_band); 
        status = FALSE;
      }
      break;      
#endif

#ifdef FEATURE_LTE
    case RFCOM_LTE_MODE:
      if (band.lte_band < RFCOM_NUM_LTE_BANDS)
      {
        *rfc_grfc_tbl = &(this->band_map_grfc_tbl.lte[band.lte_band]);
      }
      else
      {
        RF_MSG_1(RF_ERROR, "rfdevice_grfc_pa_get_band_map_grfc_tbl():"
                           "GRFC pa rfdevice_grfc_pa_get_band_map_grfc_tbl invalid LTE band %d", 
                           band.lte_band); 
        status = FALSE;
      }
      break;
#endif

#ifdef FEATURE_TDSCDMA
    case RFCOM_TDSCDMA_MODE:
      if (band.tdscdma_band < RFCOM_NUM_TDSCDMA_BANDS)
      {
        *rfc_grfc_tbl = &(this->band_map_grfc_tbl.tdscdma[band.tdscdma_band]);     
      }
      else
      {
        RF_MSG_1(RF_ERROR, "rfdevice_grfc_pa_get_band_map_grfc_tbl():"
                           "GRFC pa rfdevice_grfc_pa_get_band_map_grfc_tbl invalid TDSCDMA band %d",
                           band.tdscdma_band);    
        status = FALSE;
      }
      break;
#endif

    default:
      RF_MSG_1(RF_ERROR, "rfdevice_grfc_pa_get_band_map_grfc_tbl():"
                         "GRFC pa rfdevice_grfc_pa_get_band_map_grfc_tbl invalid tech %d",mode);
      status = FALSE;
      break;
  }
  return status;
}/* rfdevice_grfc_pa::get_band_map_tbl */


/*----------------------------------------------------------------------------*/
/*!
@brief
set_grfc() sets the GRFC pins to the correct state:
1. GRFC pin that controls the PA on
2. GRFC pins that controls the PA gain 

@param pa_config_p
rfdevice_pa_config_type pointer to structure containing the tech, band and
antenna switch path to configure

@param buff_obj_ptr
void pointer to pass address of buffer script

@param execution_type
rf_device_execution_type indicates execute internally versus returning a script

@param script_timing
the timing used for script execution

@param rfc_grfc_tbl_ptr
rfdevice_grfc_pa_rfc_tbl_type pointer to the tech/band/sig_path in the band map

@return
boolean indicating pass or fail
*/
boolean rfdevice_grfc_pa::set_grfc
(
  rfdevice_pa_cfg* pa_params_p,
  rf_buffer_intf* script_buffer,
  rf_device_execution_type execution_type,
  int16 script_timing,
  rfdevice_grfc_pa_rfc_tbl_type* rfc_grfc_tbl_ptr
)
{
  boolean status = TRUE;
  int32 signal_num = -1;
  uint8 grfc_count = 0;
  uint8 macro_select = 0;
  uint8 logic = 0;
  
  // ========[ Pre-Verification ]===============================================
  if ((execution_type != RFDEVICE_EXECUTE_IMMEDIATE) && 
      (execution_type != RFDEVICE_CREATE_SCRIPT))
  {
    RF_MSG(RF_ERROR,"[PA] rfdevice_grfc_pa::process_grfc_tbl: Invalid execution type");
    return FALSE;
  }

  if (pa_params_p == NULL)
  {
    RF_MSG(RF_ERROR,"[PA] rfdevice_grfc_pa::process_grfc_tbl: PA config parameter is NULL");
    return FALSE;
  }

  if (pa_params_p->rfm_device >= RFM_MAX_WAN_DEVICES)
  {
    RF_MSG_2(RF_ERROR, "[PA] rfdevice_grfc_pa::process_grfc_tbl(): RFM device number %d exceeds max of %d. Returning FALSE.", 
             pa_params_p->rfm_device, RFM_MAX_WAN_DEVICES);
    return FALSE;
  }

  // ========[ Get GRFC data ]==================================================
  // Get GRFC configuration data for this antenna switch path

  if (NULL == rfc_grfc_tbl_ptr)
  {
    RF_MSG(RF_ERROR, "[PA] rfdevice_grfc_pa::process_grfc_tbl(): rfc_grfc_tbl_ptr is NULL ");
    return FALSE;
  }

  // ========[ Set GRFC or prepare script to return ]===========================
  for (grfc_count = 0; grfc_count < rfc_grfc_tbl_ptr->num_grfc; grfc_count++)
  {
    // Skip invalid GRFC settings
    if (TRUE != rfc_grfc_tbl_ptr->rfc_grfc_pa_data[grfc_count].valid_flag)
    {
      RF_MSG_1(RF_ERROR,"[PA] rfdevice_grfc_pa::process_grfc_tbl(): GRFC valid flag returned false for MDM grfc %d",
                           rfc_grfc_tbl_ptr->rfc_grfc_pa_data[grfc_count].grfc_num );
      continue;
    }
    
    // Setting grfc pins iff:
    // 1. set_pat_on_flag returns true and the current grfc is used for pa on control(macro_select returns true)
    // 2. set_pa_gain_flag returns ture and the current grfc is used for pa gain control(macro_select returns false)
    macro_select = rfc_grfc_tbl_ptr->rfc_grfc_pa_data[grfc_count].macro_select;
    if ((TRUE == set_pa_on_flag && TRUE == macro_select) || (TRUE == set_pa_gain_flag && FALSE == macro_select )) 
    {
      if (TRUE == set_pa_on_flag && TRUE == macro_select) 
      {
        if (TRUE == pa_on_off) 
        {
          logic = rfc_grfc_tbl_ptr->rfc_grfc_pa_data[grfc_count].pa_on_logic;
        }
        else
        {
          logic = rfc_grfc_tbl_ptr->rfc_grfc_pa_data[grfc_count].pa_off_logic;
        }
      }
      else //TRUE == set_pa_gain_flag && FALSE == macro_select 
      {
        //make sure the gain state value doesn't exceed the total number of states(exceeding the array alloation)
        if (RFDEVICE_MAX_PA_RX_GAIN_STATES<= pa_gain)
        {
          RF_MSG_1(RF_ERROR, "rfdevice_grfc_pa::set_grfc: pa_gain value %d >= RFDEVICE_MAX_PA_RX_GAIN_STATES",
               pa_gain); 
          return FALSE;   
        }
        else
        {  
          logic = rfc_grfc_tbl_ptr->rfc_grfc_pa_data[grfc_count].gain_states[pa_gain];
        }
      }

      if (RFDEVICE_MDM_GRFC_TYPE == (rfdevice_grfc_pin_type)(rfc_grfc_tbl_ptr->rfc_grfc_pa_data[grfc_count].grfc_type))
      {
        // Convert GRFC num to signal num
        signal_num = rfc_common_get_grfc_num(rfc_grfc_tbl_ptr->rfc_grfc_pa_data[grfc_count].grfc_num);

        if (signal_num < 0)
        {
          RF_MSG_3(RF_ERROR,"[PA] set_grfc(): Invalid signal number %d returned for MDM grfc %d, grfc count %d.",
                 signal_num, rfc_grfc_tbl_ptr->rfc_grfc_pa_data[grfc_count].grfc_num, grfc_count);
          return FALSE;
        }

        // Set GRFC directly
        if (RFDEVICE_EXECUTE_IMMEDIATE == execution_type)
        {
          status &= rfc_common_set_grfc(signal_num, (rfc_logic_type)logic);
        }
        // Append GRFC to script
        else if (RFDEVICE_CREATE_SCRIPT == execution_type)
        {
          status &= script_buffer->append_grfc(signal_num,
                                             (rfc_logic_type)logic,
                                             script_timing,
                                             TRUE);
        }
      }

      else
      {
        // Unsupported GRFC type
        RF_MSG(RF_ERROR, "[PA] set_grfc(): Invalid GRFC type");
        status =  FALSE;
      }
    } 
  }
  return status;
}/* rfdevice_grfc_pa::set_grfc */

