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

 $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/loc_mw/src/loc_sensor_smd.c#2 $
 $DateTime: 2020/12/20 23:10:13 $
 $Author: pwbldsvc $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------
 11/29/12   ss      Initial version

=============================================================================*/
#include "customer.h"
#include "msg.h"

#ifdef FEATURE_REF_TRACKER_LOC
#if (defined(FEATURE_GEOFENCE_SENSOR_SUPPORT) && (!defined(FEATURE_SMART_OFFLINE_COMPILATION)))
#include "loc_sensor_smd.h"
#include "gm_api.h"

typedef enum
{
  SMD_STATE_CLOSED,
  SMD_STATE_OPENING,
  SMD_STATE_OPEN,
  SMD_STATE_CLOSING
} smd_state_e_type;

static uint32 loc_geofence_smd_report_period = 0;
static smd_state_e_type loc_geofence_smd_state = SMD_STATE_CLOSED;

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

/** loc_geofence_sensor_smd_handle_enable_resp
 *  Handle the SMD Enable Resp from ADSP. This
 *  should be invoked when SMD Enable request was 
     sent to ADSP previosuly.
 *  @param [in] enable_resp_p
 * */
void loc_geofence_sensor_smd_handle_enable_resp(void)
{
  loc_geofence_smd_ind smd_indicaiton = {0};

  memset((void*)&smd_indicaiton, 0, sizeof(smd_indicaiton)); 
  smd_indicaiton.smd_indication_type = GEOFENCE_SMD_IND_TYPE_ENABLE_RESP;
  gm_inject_smd_indication(&smd_indicaiton);
}




/** loc_geofence_sensor_smd_handle_disable_resp
 *  Handle the SMD Disable Resp from ADSP. This
 *  should be invoked when SMD Disable request was 
     sent to ADSP previosuly.
 *  @param [in] disable_resp_p
 * */
void loc_geofence_sensor_smd_handle_disable_resp(void)
{
  loc_geofence_smd_ind smd_indicaiton = {0};


  memset((void*)&smd_indicaiton, 0, sizeof(smd_indicaiton)); 
  smd_indicaiton.smd_indication_type = GEOFENCE_SMD_IND_TYPE_DISABLE_RESP; 
  gm_inject_smd_indication(&smd_indicaiton);
}



/** loc_geofence_sensor_smd_handle_async_report
 *  Handle the SMD Async Report from ADSP. This carries
     ADSP report indications
 *  @param [in] report_ind_p
 * */
void loc_geofence_sensor_smd_handle_async_report(slimSmdDataStructT *report_ind_p)
{
  loc_geofence_smd_ind smd_indicaiton = {0};

  if(report_ind_p == NULL)
  {
    SHIM_MSG_ERROR("smd Report Indication pointer NULL",0,0,0);
    return;  
  }
  
  SHIM_MSG_HIGH("SMD Report Indication Timestamp %d ",report_ind_p->timestamp,0,0);
  SHIM_MSG_HIGH("SMD Report State %d ",report_ind_p->motionState,0,0);

  memset((void*)&smd_indicaiton, 0, sizeof(smd_indicaiton)); 
  smd_indicaiton.smd_indication_type = GEOFENCE_SMD_IND_TYPE_REPORT;
  memscpy((void*)&smd_indicaiton.u.smd_report_ind, sizeof(smd_indicaiton.u.smd_report_ind),
    (void*)report_ind_p, sizeof(*report_ind_p));
  
  gm_inject_smd_indication(&smd_indicaiton);
}

/** loc_geofence_sensor_smd_handle_err_ind
 *    Handle the SMD Error Indication from ADSP.
 *  @param [in] err_ind_p
 * */
void loc_geofence_sensor_smd_handle_err_ind(slimErrorEnumT e_msgError)
{
  loc_geofence_smd_ind smd_indicaiton;

  SHIM_MSG_HIGH("SMD Error Indication Error %d ",e_msgError,0,0);

  memset((void*)&smd_indicaiton, 0, sizeof(smd_indicaiton)); 
  smd_indicaiton.smd_indication_type = GEOFENCE_SMD_IND_TYPE_ERROR;
  smd_indicaiton.u.smd_err_type = e_msgError;

  gm_inject_smd_indication(&smd_indicaiton);
}


/** loc_geofence_sensor_smd_cb
 *    SMD callback registered when enabling SMD.
 * */
void loc_geofence_sensor_smd_cb
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
    SHIM_MSG_ERROR("loc_geofence_sensor_smd_cb(): NULL header pointer",0,0,0);
    return;
  }

  if( messageHeader->msgError != eSLIM_SUCCESS )
  {
    loc_geofence_sensor_smd_handle_err_ind(messageHeader->msgError);
    return;
  }

  SHIM_MSG_HIGH("GF_SLIM: callback SMD: %d",messageHeader->msgId,0,0);

  switch( messageHeader->msgId )
  {
    case eSLIM_MESSAGE_ID_SERVICE_STATUS_IND:
    {
      slimEnableSmdDataRequestStructT smd_req;
      slimServiceStatusEventStructT *service_status = (slimServiceStatusEventStructT *) message;

      if (service_status->service != eSLIM_SERVICE_SMD || loc_geofence_smd_state == SMD_STATE_OPEN)
        break;

      /*------------------------------------------------------------------
        Check if the Sensor  service is up, if not return. Else,
        Get addressing information for accessing wds service
      ------------------------------------------------------------------*/  
      if(!slim_ServiceAvailable(service_status, eSLIM_SERVICE_SMD))
      {
        SHIM_MSG_ERROR("loc_geofence_sensor_smd_get_service(): SMD not available", 0, 0, 0);
        slim_Close(&slim_GFSmdHandle);
        loc_geofence_smd_state = SMD_STATE_CLOSED;
        return;
      }

      memset((void*)&smd_req, 0, sizeof(smd_req));
      smd_req.enableConf.enable = TRUE;
      smd_req.enableConf.providerFlags = eSLIM_SERVICE_PROVIDER_SSC;
	  smd_req.service = eSLIM_SERVICE_SMD;

      if( slim_EnableSmdData(&slim_GFSmdHandle, &smd_req, 0) != eSLIM_SUCCESS )
      {
        SHIM_MSG_ERROR("loc_geofence_sensor_start_smd: Unable to disable SMD Sensor Service\n",0, 0,0);
        slim_Close(&slim_GFSmdHandle);
        loc_geofence_smd_state = SMD_STATE_CLOSED;        
        return;
      }
      break;
    }

    case eSLIM_MESSAGE_ID_SMD_DATA_ENABLE_RESP:
    {
      loc_geofence_smd_state = SMD_STATE_OPEN;
	    loc_geofence_sensor_smd_handle_enable_resp();
      break;
    }

	case eSLIM_MESSAGE_ID_SMD_DATA_DISABLE_RESP:
	{
      slim_Close(&slim_GFSmdHandle);
      loc_geofence_smd_state = SMD_STATE_CLOSED;
      loc_geofence_sensor_smd_handle_disable_resp();
      break;
	}

    case eSLIM_MESSAGE_ID_SMD_DATA_IND:
    {
      loc_geofence_sensor_smd_handle_async_report((slimSmdDataStructT*)message);
      break;
    }

    default:
      break;
  }
}

/** loc_geofence_sensor_smd_get_service
 *    Handles SMD Get service request. This funciton
      will find and get the SMD service handle.
 * */
boolean loc_geofence_sensor_smd_get_service(void)
{

  SHIM_MSG_HIGH("GF_SLIM: get service SMD",0,0,0);

  if(loc_geofence_smd_state == SMD_STATE_OPEN)
  {
    SHIM_MSG_MED("SMD Serice already discovered\n", 0,0,0);
    return TRUE;
  }

  /*-----------------------------------------------------------------------
     Open SLIM connection
  -----------------------------------------------------------------------*/
  if( slim_Open(&slim_GFSmdHandle, loc_geofence_sensor_smd_cb, 0, 0) 
        != eSLIM_SUCCESS)
  {
    SHIM_MSG_ERROR("loc_geofence_sensor_smd_get_service(): Open failed",0,0,0);
    loc_geofence_smd_state = SMD_STATE_CLOSED;
    return FALSE;
  }

  return TRUE;
}


/** loc_geofence_sensor_start_smd
 *    Handles SMD Start request from Geofence module. 
 *  @param [in] report_period
 * */
boolean loc_geofence_sensor_start_smd(uint32 report_period)
{  

  SHIM_MSG_HIGH("GF_SLIM: start SMD",0,0,0);

  if(loc_geofence_smd_state == SMD_STATE_CLOSED)
  {
    loc_geofence_smd_state = SMD_STATE_OPENING;
    loc_geofence_smd_report_period = report_period;
    if(loc_geofence_sensor_smd_get_service() == FALSE)
    {      
      SHIM_MSG_ERROR("loc_geofence_sensor_start_smd: Unable to get SMD Sensor Service\n",0, 0,0);
      loc_geofence_smd_state = SMD_STATE_CLOSED;
      return FALSE;
    }
  }
  else
  {
    SHIM_MSG_ERROR("loc_geofence_sensor_start_smd: SMD Sensor Service not closed\n",0, 0,0);
    return FALSE;
  }

  return TRUE;
}


/** loc_geofence_sensor_stop_smd
 *    Handles SMD Stop request from Geofence module. 
 *  @param [in] void
 * */
boolean loc_geofence_sensor_stop_smd(void)
{
  slimEnableSmdDataRequestStructT smd_req;

  SHIM_MSG_HIGH("GF_SLIM: stop SMD",0,0,0);

  if(loc_geofence_smd_state == SMD_STATE_CLOSED)
  {
    SHIM_MSG_ERROR("loc_geofence_sensor_disable_smd: already disabled",0, 0,0);
    return FALSE;
  }

  memset((void*)&smd_req, 0, sizeof(smd_req));
  smd_req.enableConf.enable = FALSE;
  smd_req.enableConf.providerFlags = eSLIM_SERVICE_PROVIDER_SSC;
  smd_req.service = eSLIM_SERVICE_SMD;

  loc_geofence_smd_state = SMD_STATE_CLOSING;
  if( slim_EnableSmdData(&slim_GFSmdHandle, &smd_req, 0) != eSLIM_SUCCESS )
  {
    SHIM_MSG_ERROR("loc_geofence_sensor_start_smd: Unable to disable SMD Sensor Service\n",0, 0,0);
    slim_Close(&slim_GFSmdHandle);
    loc_geofence_smd_state = SMD_STATE_CLOSED;
    return FALSE;
  }

  return TRUE;
}

#endif /*FEATURE_GEOFENCE_SENSOR_SUPPORT*/
#else

/** loc_geofence_sensor_start_smd
 *    Handles SMD Start request from Geofence module. 
 *  @param [in] report_period
 * */
boolean loc_geofence_sensor_start_smd(uint32 report_period)
{
  MSG(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_ERROR,"loc_geofence_sensor_start_smd: SMD not enabled in this build!");
  return FALSE;
}

/** loc_geofence_sensor_stop_smd
 *    Handles SMD Stop request from Geofence module. 
 *  @param [in] void
 * */
boolean loc_geofence_sensor_stop_smd(void)
{
  MSG(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_ERROR,"loc_geofence_sensor_stop_smd: SMD not enabled in this build!");
  return FALSE;
}

#endif /* FEATURE_REF_TRACKER_LOC*/
