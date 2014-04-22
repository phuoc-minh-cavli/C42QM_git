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

 $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/loc_mw/src/loc_sensor_amd.c#1 $
 $DateTime: 2020/07/21 21:23:44 $
 $Author: pwbldsvc $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------
 11/29/12   ss      Initial version

=============================================================================*/
#include "customer.h"
#include "msg.h"

#ifdef FEATURE_REF_TRACKER_LOC
#if (defined(FEATURE_GEOFENCE_SENSOR_SUPPORT) && (!defined(FEATURE_SMART_OFFLINE_COMPILATION)))
#include "loc_sensor_amd.h"
#include "gm_api.h"

typedef enum
{
  AMD_STATE_CLOSED,
  AMD_STATE_OPENING,
  AMD_STATE_OPEN,
  AMD_STATE_CLOSING
} amd_state_e_type;

static uint32 loc_geofence_amd_report_period = 0;
static amd_state_e_type loc_geofence_amd_state = AMD_STATE_CLOSED;

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

/** loc_geofence_sensor_amd_handle_enable_resp
 *  Handle the AMD Enable Resp from ADSP. This
 *  should be invoked when AMD Enable request was 
     sent to Slim previosuly.
 *  @param [in] void
 * */
void loc_geofence_sensor_amd_handle_enable_resp(void)
{
  loc_geofence_amd_ind amd_indicaiton = {0};

  memset((void*)&amd_indicaiton, 0, sizeof(amd_indicaiton)); 
  amd_indicaiton.amd_indication_type = GEOFENCE_AMD_IND_TYPE_ENABLE_RESP;
  gm_inject_amd_indication(&amd_indicaiton);
}



/** loc_geofence_sensor_amd_handle_disable_resp
 *  Handle the AMD Disable Resp from ADSP. This
 *  should be invoked when AMD Disable request was 
     sent to ADSP previosuly.
 *  @param [in] disable_resp_p
 * */
void loc_geofence_sensor_amd_handle_disable_resp(void)
{
 loc_geofence_amd_ind amd_indicaiton = {0};


 memset((void*)&amd_indicaiton, 0, sizeof(amd_indicaiton)); 
 amd_indicaiton.amd_indication_type = GEOFENCE_AMD_IND_TYPE_DISABLE_RESP; 
 gm_inject_amd_indication(&amd_indicaiton);
}

/** loc_geofence_sensor_amd_handle_async_report
 *  Handle the AMD Async Report from ADSP. This carries
     ADSP report indications
 *  @param [in] report_ind_p
 * */
void loc_geofence_sensor_amd_handle_async_report(slimQmdDataStructT *report_ind_p)
{
  loc_geofence_amd_ind amd_indicaiton = {0};

  if(report_ind_p == NULL)
  {
    SHIM_MSG_ERROR("amd Report Indication pointer NULL",0,0,0);
    return;  
  }
  
  SHIM_MSG_HIGH("AMD Report Indication Timestamp %d ",report_ind_p->timestamp,0,0);

  SHIM_MSG_HIGH("AMD Report State %d ",report_ind_p->motionState,0,0);

  memset((void*)&amd_indicaiton, 0, sizeof(amd_indicaiton)); 
  amd_indicaiton.amd_indication_type = GEOFENCE_AMD_IND_TYPE_REPORT;
  memscpy((void*)&amd_indicaiton.u.amd_report_ind, sizeof(amd_indicaiton.u.amd_report_ind),
    (void*)report_ind_p, sizeof(*report_ind_p));
  
  gm_inject_amd_indication(&amd_indicaiton);
}

/** loc_geofence_sensor_amd_handle_err_ind
 *    Handle the AMD Error Indication from ADSP.
 *  @param [in] err_ind_p
 * */
void loc_geofence_sensor_amd_handle_err_ind(slimErrorEnumT e_msgError)
{
  loc_geofence_amd_ind amd_indicaiton;

  SHIM_MSG_HIGH("AMD Error Indication Error %d ",e_msgError,0,0);

  memset((void*)&amd_indicaiton, 0, sizeof(amd_indicaiton)); 
  amd_indicaiton.amd_indication_type = GEOFENCE_AMD_IND_TYPE_ERROR;
  amd_indicaiton.u.amd_err_type = e_msgError;

  gm_inject_amd_indication(&amd_indicaiton);
}

/** loc_geofence_sensor_amd_cb
 *    AMD callback registered when enabling AMD.
 * */
void loc_geofence_sensor_amd_cb
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
    SHIM_MSG_ERROR("loc_geofence_sensor_amd_cb(): NULL header pointer",0,0,0);
    return;
  }

  if( messageHeader->msgError != eSLIM_SUCCESS )
  {
    loc_geofence_sensor_amd_handle_err_ind(messageHeader->msgError);
    return;
  }

  SHIM_MSG_HIGH("GF_SLIM: Callback AMD: %d",messageHeader->msgId,0,0);

  switch( messageHeader->msgId )
  {
    case eSLIM_MESSAGE_ID_SERVICE_STATUS_IND:
    {
      slimEnableQmdDataRequestStructT qmd_req = {0};
      slimServiceStatusEventStructT *service_status = (slimServiceStatusEventStructT *) message;

      if (service_status->service != eSLIM_SERVICE_AMD || loc_geofence_amd_state == AMD_STATE_OPEN)
        break;

      /*------------------------------------------------------------------
        Check if the Sensor  service is up, if not return. Else,
        Get addressing information for accessing wds service
      ------------------------------------------------------------------*/  
      if(!slim_ServiceAvailable(service_status, eSLIM_SERVICE_AMD))
      {
        SHIM_MSG_ERROR("loc_geofence_sensor_amd_get_service(): AMD not available", 0, 0, 0);
        slim_Close(&slim_GFAmdHandle);
        loc_geofence_amd_state = AMD_STATE_CLOSED;
        return;
      }

      memset((void*)&qmd_req, 0, sizeof(qmd_req));
      qmd_req.enableConf.enable = TRUE;
      qmd_req.enableConf.providerFlags = eSLIM_SERVICE_PROVIDER_SSC;
      qmd_req.reportPeriod = loc_geofence_amd_report_period;
      qmd_req.service = eSLIM_SERVICE_AMD;

      if( slim_EnableQmdData(&slim_GFAmdHandle, &qmd_req, 0) != eSLIM_SUCCESS )
      {
        SHIM_MSG_ERROR("loc_geofence_sensor_start_amd: Unable to disable AMD Sensor Service\n",0, 0,0);
        slim_Close(&slim_GFAmdHandle);
        loc_geofence_amd_state = AMD_STATE_CLOSED;
        return;
      }
      break;
    }

    case eSLIM_MESSAGE_ID_QMD_DATA_ENABLE_RESP:
    {
      loc_geofence_amd_state = AMD_STATE_OPEN;
      loc_geofence_sensor_amd_handle_enable_resp();
      break;
    }

    case eSLIM_MESSAGE_ID_QMD_DATA_DISABLE_RESP:
    {
      slim_Close(&slim_GFAmdHandle);
      loc_geofence_amd_state = AMD_STATE_CLOSED;

      loc_geofence_sensor_amd_handle_disable_resp();
      break;
    }

    case eSLIM_MESSAGE_ID_QMD_DATA_IND:
    {
      loc_geofence_sensor_amd_handle_async_report((slimQmdDataStructT*)message);
      break;
    }

    default:
      break;
  }
}

/** loc_geofence_sensor_amd_get_service
 *    Handles AMD Get service request. This funciton
      will find and get the AMD service handle.
 * */
boolean loc_geofence_sensor_amd_get_service(void)
{
  SHIM_MSG_HIGH("GF_SLIM: Get Service AMD",0,0,0);

  if(loc_geofence_amd_state == AMD_STATE_OPEN)
  {
    SHIM_MSG_MED("AMD Serice already discovered\n", 0,0,0);
    return TRUE;
  }

  /*-----------------------------------------------------------------------
     Open SLIM connection
  -----------------------------------------------------------------------*/
  if( slim_Open(&slim_GFAmdHandle, loc_geofence_sensor_amd_cb, 0, 0) 
        != eSLIM_SUCCESS)
  {
    SHIM_MSG_ERROR("loc_geofence_sensor_amd_get_service(): Open failed",0,0,0);
    loc_geofence_amd_state = AMD_STATE_CLOSED;
    return FALSE;
  }

  return TRUE;
}


/** loc_geofence_sensor_start_amd
 *    Handles AMD Start request from Geofence module. 
 *  @param [in] report_period
 * */
boolean loc_geofence_sensor_start_amd(uint32 report_period)
{
  SHIM_MSG_HIGH("GF_SLIM: Start AMD",0,0,0);

  if(loc_geofence_amd_state == AMD_STATE_CLOSED)
  {
    loc_geofence_amd_state = AMD_STATE_OPENING;
    loc_geofence_amd_report_period = report_period;
    if(loc_geofence_sensor_amd_get_service() == FALSE)
    {      
      SHIM_MSG_ERROR("loc_geofence_sensor_start_amd: Unable to get AMD Sensor Service\n",0, 0,0);
      loc_geofence_amd_state = AMD_STATE_CLOSED;
      return FALSE;
    }

  }
  else
  {
    SHIM_MSG_ERROR("loc_geofence_sensor_start_amd: AMD Sensor Service not closed\n",0, 0,0);
    return FALSE;
  }

  return TRUE;
}


/** loc_geofence_sensor_stop_amd
 *    Handles AMD Stop request from Geofence module. 
 *  @param [in] void
 * */
boolean loc_geofence_sensor_stop_amd(void)
{
  slimEnableQmdDataRequestStructT qmd_req;

  SHIM_MSG_HIGH("GF_SLIM: Stop AMD",0,0,0);

  if(loc_geofence_amd_state == AMD_STATE_CLOSED)
  {
    SHIM_MSG_ERROR("loc_geofence_sensor_disable_amd: service not open\n",0, 0,0);
    return FALSE;
  }

  memset((void*)&qmd_req, 0, sizeof(qmd_req));
  qmd_req.enableConf.enable = FALSE;
  qmd_req.service = eSLIM_SERVICE_AMD;

  loc_geofence_amd_state = AMD_STATE_CLOSING;
  if( slim_EnableQmdData(&slim_GFAmdHandle, &qmd_req, 0) != eSLIM_SUCCESS )
  {
    SHIM_MSG_ERROR("loc_geofence_sensor_stop_amd: Unable to disable AMD Sensor Service\n",0, 0,0);
    slim_Close(&slim_GFAmdHandle);
    loc_geofence_amd_state = AMD_STATE_CLOSED;
    return FALSE;
  }

  return TRUE;
}

#endif /*FEATURE_GEOFENCE_SENSOR_SUPPORT*/
#else

/** loc_geofence_sensor_start_amd
 *    Handles AMD Start request from Geofence module. 
 *  @param [in] report_period
 * */
boolean loc_geofence_sensor_start_amd(uint32 report_period)
{
  MSG(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_ERROR,"loc_geofence_sensor_start_amd: AMD not enabled in this build!");
  return FALSE;
}
/** loc_geofence_sensor_stop_amd
 *    Handles AMD Stop request from Geofence module. 
 *  @param [in] void
 * */
boolean loc_geofence_sensor_stop_amd(void)
{
  MSG(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_ERROR,"loc_geofence_sensor_stop_amd: AMD not enabled in this build!");
  return FALSE;
}

#endif /* FEATURE_REF_TRACKER_LOC*/
