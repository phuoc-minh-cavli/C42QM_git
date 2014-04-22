/*============================================================================
 @file loc_sensor_client.c

 loc MW sensor client implementation

 GENERAL DESCRIPTION

 This file contains the loc middleware sensor client implementation.

 Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
 All Rights Reserved
 Qualcomm Confidential and Proprietary
 
 Copyright (c) 2013-2014 Qualcomm Atheros, Inc.
 All Rights Reserved. 
 Qualcomm Atheros Confidential and Proprietary.

 Export of this technology or software is regulated by the U.S. Government.
 Diversion contrary to U.S. law prohibited.

 =============================================================================*/

/*============================================================================

 EDIT HISTORY FOR FILE

 This section contains comments describing changes made to the module.

 $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/loc_mw/src/loc_sensor_ped.c#1 $
 $DateTime: 2020/07/21 21:23:44 $
 $Author: pwbldsvc $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------
 11/29/12   ss      Initial version

=============================================================================*/
#include "customer.h"

#if (defined(FEATURE_GEOFENCE_SENSOR_SUPPORT) && (!defined(FEATURE_SMART_OFFLINE_COMPILATION)))
#include "msg.h"
#include "loc_sensor_ped.h"
#include "gm_api.h"

typedef enum
{
  PED_STATE_CLOSED,
  PED_STATE_OPENING,
  PED_STATE_OPEN,
  PED_STATE_CLOSING
} ped_state_e_type;

static uint32 loc_geofence_ped_report_period = 0;
static ped_state_e_type loc_geofence_ped_state = PED_STATE_CLOSED;

#define SHIM_MSG_LOW( str, a, b, c )                                          \
                  MSG_3(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_MED, str,  a, b, c )
#define SHIM_MSG_MED( str, a, b, c )                                          \
                  MSG_3(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_MED, str, a, b, c )
#define SHIM_MSG_HIGH( str, a, b, c )                                         \
                  MSG_3(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_HIGH, str, a, b, c )
#define SHIM_MSG_ERROR( str, a, b, c )                                        \
                  MSG_3(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_ERROR, str, a, b, c )
#define SHIM_ERR_FATAL( str, a, b, c )                                        \
                  MSG_3(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_FATAL, str, a, b, c )

/** loc_geofence_sensor_ped_handle_enable_resp
 *  Handle the PED Enable Resp from ADSP. This
 *  should be invoked when PED Enable request was 
     sent to ADSP previosuly.
 *  @param [in] enable_resp_p
 * */
void loc_geofence_sensor_ped_handle_enable_resp(void)
{
 loc_geofence_ped_ind ped_indicaiton = {0};

 memset((void*)&ped_indicaiton, 0, sizeof(ped_indicaiton)); 
 ped_indicaiton.ped_indication_type = GEOFENCE_SMD_IND_TYPE_ENABLE_RESP;
 
 //gm_inject_ped_indication(&ped_indicaiton);
}



/** loc_geofence_sensor_ped_handle_disable_resp
 *  Handle the PED Disable Resp from ADSP. This
 *  should be invoked when PED Disable request was 
     sent to ADSP previosuly.
 *  @param [in] disable_resp_p
 * */
void loc_geofence_sensor_ped_handle_disable_resp(void)
{
 loc_geofence_ped_ind ped_indicaiton = {0};

 memset((void*)&ped_indicaiton, 0, sizeof(ped_indicaiton)); 
 ped_indicaiton.ped_indication_type = GEOFENCE_SMD_IND_TYPE_DISABLE_RESP; 
 //gm_inject_ped_indication(&ped_indicaiton);
}


/** loc_geofence_sensor_ped_handle_async_report
 *  Handle the PED Async Report from ADSP. This carries
     ADSP report indications
 *  @param [in] report_ind_p
 * */
void loc_geofence_sensor_ped_handle_async_report(slimPedometerDataStructT *report_ind_p)
{
  loc_geofence_ped_ind ped_indicaiton;

  if(report_ind_p == NULL)
  {
    SHIM_MSG_ERROR("ped Report Indication pointer NULL",0,0,0);
    return;  
  }
  
  SHIM_MSG_HIGH("PED Report Indication Timestamp %d ",report_ind_p->timestamp,0,0);

  SHIM_MSG_HIGH("PED Report Step Count %d ", report_ind_p->stepCount, 0, 0);
  SHIM_MSG_HIGH("PED Report Step Rate %d Valid %d", report_ind_p->stepRate, report_ind_p->stepRate_valid, 0);
  SHIM_MSG_HIGH("PED Report Step Confidence %d Valid %d ", report_ind_p->stepConfidence, report_ind_p->stepConfidence_valid, 0);

  memset((void*)&ped_indicaiton, 0, sizeof(ped_indicaiton)); 
  ped_indicaiton.ped_indication_type = GEOFENCE_SMD_IND_TYPE_REPORT;
  memscpy((void*)&ped_indicaiton.u.ped_report_ind, sizeof(ped_indicaiton.u.ped_report_ind),
    (void*)report_ind_p, sizeof(*report_ind_p));
  
  //gm_inject_ped_indication(&ped_indicaiton);
}

/** loc_geofence_sensor_ped_handle_err_ind
 *    Handle the PED Error Indication from ADSP.
 *  @param [in] err_ind_p
 * */
void loc_geofence_sensor_ped_handle_err_ind(slimErrorEnumT e_msgError)
{
  loc_geofence_ped_ind ped_indicaiton = {0};

  SHIM_MSG_HIGH("PED Error Indication Error %d ",e_msgError,0,0);

  memset((void*)&ped_indicaiton, 0, sizeof(ped_indicaiton)); 
  ped_indicaiton.ped_indication_type = GEOFENCE_AMD_IND_TYPE_ERROR;
  ped_indicaiton.u.ped_err_type = e_msgError;

  //gm_inject_ped_indication(&ped_indicaiton);
}


/** loc_geofence_sensor_ped_cb
 *    PED callback registered when enabling PED.
 * */
void loc_geofence_sensor_ped_cb
(
  uint64_t callbackData,
  /**<[in] Callback cookie, provided with #slimClientOpen call.*/
  const slimMessageHeaderStructT *messageHeader,
  /**<[in] Message header. */
  void *message
  /**<[in] Message payload, when mesageHeader.size is not 0. */
)
{
  (void)callbackData;

  if( messageHeader == NULL )
  {
    SHIM_MSG_ERROR("loc_geofence_sensor_ped_cb(): NULL header pointer",0,0,0);
    return;
  }

  if( messageHeader->msgError != eSLIM_SUCCESS )
  {
    loc_geofence_sensor_ped_handle_err_ind(messageHeader->msgError);
    return;
  }

  SHIM_MSG_HIGH("GF_SLIM: callback PED: %d",messageHeader->msgId,0,0);

  switch( messageHeader->msgId )
  {
    case eSLIM_MESSAGE_ID_SERVICE_STATUS_IND:
    {
      slimEnablePedometerRequestStructT ped_req;
      slimServiceStatusEventStructT *service_status = (slimServiceStatusEventStructT *) message;

      if (service_status->service != eSLIM_SERVICE_PEDOMETER || loc_geofence_ped_state == PED_STATE_OPEN)
        break;

      /*------------------------------------------------------------------
        Check if the Sensor  service is up, if not return. Else,
        Get addressing information for accessing wds service
      ------------------------------------------------------------------*/  
      if(!slim_ServiceAvailable(service_status, eSLIM_SERVICE_PEDOMETER))
      {
        SHIM_MSG_ERROR("loc_geofence_sensor_ped_get_service(): PED not available", 0, 0, 0);
        slim_Close(&slim_GFPedHandle);
        loc_geofence_ped_state = PED_STATE_CLOSED;
        return;
      }

      memset((void*)&ped_req, 0, sizeof(ped_req));
      ped_req.enableConf.enable = TRUE;
      ped_req.enableConf.providerFlags = eSLIM_SERVICE_PROVIDER_SSC;
      ped_req.reportPeriod = loc_geofence_ped_report_period;

      if( slim_EnablePedometer(&slim_GFPedHandle, &ped_req, 0) != eSLIM_SUCCESS )
      {
        SHIM_MSG_ERROR("loc_geofence_sensor_start_ped: Unable to enable PED Sensor Service\n",0, 0,0);
        slim_Close(&slim_GFPedHandle);
        loc_geofence_ped_state = PED_STATE_CLOSED;
        return;
      }
      break;
    }

    case eSLIM_MESSAGE_ID_PEDOMETER_ENABLE_RESP:
    {
      loc_geofence_ped_state = PED_STATE_OPEN;
      loc_geofence_sensor_ped_handle_enable_resp();
      break;
    }

    case eSLIM_MESSAGE_ID_PEDOMETER_DISABLE_RESP:
    {
      slim_Close(&slim_GFPedHandle);
      loc_geofence_ped_state = PED_STATE_CLOSED;
      loc_geofence_sensor_ped_handle_disable_resp();
      break;
    }

    case eSLIM_MESSAGE_ID_PEDOMETER_IND:
    {
      slimPedometerDataStructT *ped_data = (slimPedometerDataStructT *) message;
      
      if (!ped_data->stepConfidence_valid || !ped_data->stepCountUncertainty_valid
        || !ped_data->stepRate_valid)
      {
        SHIM_MSG_ERROR("loc_geofence_sensor_ped_cb(): not all required SLIM parameters valid", 0, 0, 0);
        return;
      }
      loc_geofence_sensor_ped_handle_async_report(ped_data);
      break;
    }

    default:
      break;
  }
}

/** loc_geofence_sensor_ped_get_service
 *    Handles PED Get service request. This funciton
      will find and get the PED service handle.
 * */
boolean loc_geofence_sensor_ped_get_service(void)
{
  SHIM_MSG_HIGH("GF_SLIM: get service PED",0,0,0);

  if(loc_geofence_ped_state == PED_STATE_OPEN)
  {
    SHIM_MSG_MED("PED Service already discovered\n", 0,0,0);
    return TRUE;
  }

  /*-----------------------------------------------------------------------
     Open SLIM connection
  -----------------------------------------------------------------------*/
  if( slim_Open(&slim_GFPedHandle, loc_geofence_sensor_ped_cb, 0, 0) 
        != eSLIM_SUCCESS)
  {
    SHIM_MSG_ERROR("loc_geofence_sensor_ped_get_service(): Open failed",0,0,0);
    loc_geofence_ped_state = PED_STATE_CLOSED;
    return FALSE;
  }

  return TRUE;
}


/** loc_geofence_sensor_start_ped
 *    Handles PED Start request from Geofence module. 
 *  @param [in] report_period
 * */
boolean loc_geofence_sensor_start_ped(uint32 report_period)
{
  SHIM_MSG_HIGH("GF_SLIM: start PED",0,0,0);

  if(loc_geofence_ped_state == PED_STATE_CLOSED)
  {
    loc_geofence_ped_state = PED_STATE_OPENING;
    loc_geofence_ped_report_period = report_period;
    if(loc_geofence_sensor_ped_get_service() == FALSE)
    {      
      SHIM_MSG_ERROR("loc_geofence_sensor_start_ped: Unable to get PED Sensor Service\n",0, 0,0);
      loc_geofence_ped_state = PED_STATE_CLOSED;
      return FALSE;
    }
  }
  else
  {
    SHIM_MSG_ERROR("loc_geofence_sensor_start_ped: PED Sensor Service not closed\n",0, 0,0);
    return FALSE;
  }

  return TRUE;
}


/** loc_geofence_sensor_stop_ped
 *    Handles PED Stop request from Geofence module. 
 *  @param [in] void
 * */
boolean loc_geofence_sensor_stop_ped(void)
{
  slimEnablePedometerRequestStructT ped_req;

  SHIM_MSG_HIGH("GF_SLIM: stop PED",0,0,0);

  if(loc_geofence_ped_state == PED_STATE_CLOSED)
  {
    SHIM_MSG_ERROR("loc_geofence_sensor_disable_ped: ped already closed\n",0, 0,0);
    return FALSE;
  }

  memset((void*)&ped_req, 0, sizeof(ped_req));
  ped_req.enableConf.enable = FALSE;
  ped_req.enableConf.providerFlags = eSLIM_SERVICE_PROVIDER_SSC;

  loc_geofence_ped_state = PED_STATE_CLOSING;
  if( slim_EnablePedometer(&slim_GFPedHandle, &ped_req, 0) != eSLIM_SUCCESS )
  {
    SHIM_MSG_ERROR("loc_geofence_sensor_start_ped: Unable to disable PED Sensor Service\n",0, 0,0);
    slim_Close(&slim_GFPedHandle);
    loc_geofence_ped_state = PED_STATE_CLOSED;
    return FALSE;
  }

  return TRUE;
}

#endif /*FEATURE_GEOFENCE_SENSOR_SUPPORT*/
