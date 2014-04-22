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

 $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/loc_mw/src/loc_sensor_rmd.c#1 $
 $DateTime: 2020/07/21 21:23:44 $
 $Author: pwbldsvc $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------
 11/29/12   ss      Initial version

=============================================================================*/
#include "customer.h"

#if (defined(FEATURE_GEOFENCE_SENSOR_SUPPORT) && (!defined(FEATURE_SMART_OFFLINE_COMPILATION)))
#include "msg.h"
#include "gm_api.h"

typedef enum
{
  RMD_STATE_CLOSED,
  RMD_STATE_OPENING,
  RMD_STATE_OPEN,
  RMD_STATE_CLOSING
} rmd_state_e_type;

static uint32 loc_geofence_rmd_report_period = 0;
static rmd_state_e_type loc_geofence_rmd_state = RMD_STATE_CLOSED;

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

/** loc_geofence_sensor_rmd_handle_enable_resp
 *  Handle the RMD Enable Resp from ADSP. This
 *  should be invoked when RMD Enable request was 
     sent to ADSP previosuly.
 *  @param [in] enable_resp_p
 * */
void loc_geofence_sensor_rmd_handle_enable_resp(void)
{
  loc_geofence_rmd_ind rmd_indicaiton = {0};

  memset((void*)&rmd_indicaiton, 0, sizeof(rmd_indicaiton)); 
  rmd_indicaiton.rmd_indication_type = GEOFENCE_RMD_IND_TYPE_ENABLE_RESP;
  //gm_inject_rmd_indication(&rmd_indicaiton);
}



/** loc_geofence_sensor_rmd_handle_disable_resp
 *  Handle the RMD Disable Resp from ADSP. This
 *  should be invoked when RMD Disable request was 
     sent to ADSP previosuly.
 *  @param [in] disable_resp_p
 * */
void loc_geofence_sensor_rmd_handle_disable_resp(void)
{
  loc_geofence_rmd_ind rmd_indicaiton = {0};

  memset((void*)&rmd_indicaiton, 0, sizeof(rmd_indicaiton)); 
  rmd_indicaiton.rmd_indication_type = GEOFENCE_RMD_IND_TYPE_DISABLE_RESP;
  //gm_inject_rmd_indication(&rmd_indicaiton);
}



/** loc_geofence_sensor_rmd_handle_async_report
 *  Handle the RMD Async Report from Slim. This carries
     ADSP report indications
 *  @param [in] report_ind_p
 * */
void loc_geofence_sensor_rmd_handle_async_report(slimQmdDataStructT *report_ind_p)
{
  loc_geofence_rmd_ind rmd_indicaiton;

  if(report_ind_p == NULL)
  {
    SHIM_MSG_ERROR("rmd Report Indication pointer NULL",0,0,0);
    return;  
  }
  
  SHIM_MSG_HIGH("RMD Report Indication Timestamp %d ",report_ind_p->timestamp,0,0);

  SHIM_MSG_HIGH("RMD Report State %d ",report_ind_p->motionState,0,0);

  memset((void*)&rmd_indicaiton, 0, sizeof(rmd_indicaiton)); 
  rmd_indicaiton.rmd_indication_type = GEOFENCE_RMD_IND_TYPE_REPORT;
  memscpy((void*)&rmd_indicaiton.u.rmd_report_ind, sizeof(rmd_indicaiton.u.rmd_report_ind),
    (void*)report_ind_p, sizeof(*report_ind_p));
  
  //gm_inject_rmd_indication(&rmd_indicaiton);
}


/** loc_geofence_sensor_rmd_handle_err_ind
 *    Handle the RMD Error Indication from Slim.
 *  @param [in] err_ind_p
 * */
void loc_geofence_sensor_rmd_handle_err_ind(slimErrorEnumT e_msgError)
{
  loc_geofence_rmd_ind rmd_indicaiton;

  SHIM_MSG_HIGH("RMD Error Indication Error %d ",e_msgError,0,0);

  memset((void*)&rmd_indicaiton, 0, sizeof(rmd_indicaiton)); 
  rmd_indicaiton.rmd_indication_type = GEOFENCE_RMD_IND_TYPE_ERROR;
  rmd_indicaiton.u.rmd_err_type = e_msgError;

  //gm_inject_rmd_indication(&rmd_indicaiton);
}


/** loc_geofence_sensor_rmd_cb
 *    RMD callback registered when enabling RMD.
 * */
void loc_geofence_sensor_rmd_cb
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
    SHIM_MSG_ERROR("loc_geofence_sensor_rmd_cb(): NULL header pointer",0,0,0);
    return;
  }

  if( messageHeader->msgError != eSLIM_SUCCESS )
  {
    loc_geofence_sensor_rmd_handle_err_ind(messageHeader->msgError );
    return;
  }

  SHIM_MSG_HIGH("GF_SLIM: callback RMD: %d",messageHeader->msgId,0,0);

  switch( messageHeader->msgId )
  {
    case eSLIM_MESSAGE_ID_SERVICE_STATUS_IND:
    {
      slimEnableQmdDataRequestStructT qmd_req;
      slimServiceStatusEventStructT *service_status = (slimServiceStatusEventStructT *) message;

      if (service_status->service != eSLIM_SERVICE_RMD || loc_geofence_rmd_state == RMD_STATE_OPEN)
        break;

      /*------------------------------------------------------------------
        Check if the Sensor  service is up, if not return. Else,
        Get addressing information for accessing wds service
      ------------------------------------------------------------------*/  
      if(!slim_ServiceAvailable(service_status, eSLIM_SERVICE_RMD))
      {
        SHIM_MSG_ERROR("loc_geofence_sensor_rmd_get_service(): RMD not available", 0, 0, 0);
        slim_Close(&slim_GFRmdHandle);
        loc_geofence_rmd_state = RMD_STATE_CLOSED;
        return;
      }

      memset((void*)&qmd_req, 0, sizeof(qmd_req));
      qmd_req.enableConf.enable = TRUE;
      qmd_req.enableConf.providerFlags = eSLIM_SERVICE_PROVIDER_SSC;
      qmd_req.reportPeriod = loc_geofence_rmd_report_period;
      qmd_req.service = eSLIM_SERVICE_RMD;

      if( slim_EnableQmdData(&slim_GFRmdHandle, &qmd_req, 0) != eSLIM_SUCCESS )
      {
        SHIM_MSG_ERROR("loc_geofence_sensor_start_rmd: Unable to disable RMD Sensor Service\n",0, 0,0);
        slim_Close(&slim_GFRmdHandle);
        loc_geofence_rmd_state = RMD_STATE_CLOSED;
        return;
      }
      break;
    }

    case eSLIM_MESSAGE_ID_QMD_DATA_ENABLE_RESP:
    {
      loc_geofence_rmd_state = RMD_STATE_OPEN;
      loc_geofence_sensor_rmd_handle_enable_resp();
      break;
    }

    case eSLIM_MESSAGE_ID_QMD_DATA_DISABLE_RESP:
    {
      slim_Close(&slim_GFRmdHandle);
      loc_geofence_rmd_state = RMD_STATE_CLOSED;

      loc_geofence_sensor_rmd_handle_disable_resp();
      break;
    }

    case eSLIM_MESSAGE_ID_QMD_DATA_IND:
    {
      loc_geofence_sensor_rmd_handle_async_report((slimQmdDataStructT *)message);
      break;
    }

    default:
      break;
  }
}

/** loc_geofence_sensor_rmd_get_service
 *    Handles RMD Get service request. This funciton
      will find and get the RMD service handle.
 * */
boolean loc_geofence_sensor_rmd_get_service(void)
{

  SHIM_MSG_HIGH("GF_SLIM: get service RMD",0,0,0);

  if(loc_geofence_rmd_state == RMD_STATE_OPEN)
  {
    SHIM_MSG_MED("RMD Serice already discovered\n", 0,0,0);
    return TRUE;
  }

  /*-----------------------------------------------------------------------
     Open SLIM connection
  -----------------------------------------------------------------------*/
  if( slim_Open(&slim_GFRmdHandle, loc_geofence_sensor_rmd_cb, 0, 0) 
        != eSLIM_SUCCESS)
  {
    SHIM_MSG_ERROR("loc_geofence_sensor_rmd_get_service(): Open failed",0,0,0);
    loc_geofence_rmd_state = RMD_STATE_CLOSED;
    return FALSE;
  }

  return TRUE;
}

/** loc_geofence_sensor_start_rmd
 *    Handles RMD Start request from Geofence module. 
 *  @param [in] report_period
 * */
boolean loc_geofence_sensor_start_rmd(uint32 report_period)
{

  SHIM_MSG_HIGH("GF_SLIM: start RMD",0,0,0);

  if(loc_geofence_rmd_state == RMD_STATE_CLOSED)
  {
    loc_geofence_rmd_state = RMD_STATE_OPENING;
    loc_geofence_rmd_report_period = report_period;
    if(loc_geofence_sensor_rmd_get_service() == FALSE)
    {      
      SHIM_MSG_ERROR("loc_geofence_sensor_start_rmd: Unable to get RMD Sensor Service\n",0, 0,0);
      loc_geofence_rmd_state = RMD_STATE_CLOSED;
      return FALSE;
    }
  }
  else
  {
    SHIM_MSG_ERROR("loc_geofence_sensor_start_rmd: RMD Sensor Service not closed\n",0, 0,0);
    return FALSE;
  }

  return TRUE;
}

/** loc_geofence_sensor_stop_rmd
 *    Handles RMD Stop request from Geofence module. 
 *  @param [in] void
 * */
boolean loc_geofence_sensor_stop_rmd(void)
{
  slimEnableQmdDataRequestStructT qmd_req;

  SHIM_MSG_HIGH("GF_SLIM: stop RMD",0,0,0);

  if(slim_GFRmdHandle == 0)
  {
    SHIM_MSG_ERROR("loc_geofence_sensor_disable_rmd: Client pointer NULL\n",0, 0,0);
    return FALSE;
  }

  memset((void*)&qmd_req, 0, sizeof(qmd_req));
  qmd_req.enableConf.enable = FALSE;
  qmd_req.service = eSLIM_SERVICE_RMD;
  qmd_req.enableConf.providerFlags = eSLIM_SERVICE_PROVIDER_SSC;

  loc_geofence_rmd_state = RMD_STATE_CLOSING;
  if( slim_EnableQmdData(&slim_GFRmdHandle, &qmd_req, 0) != eSLIM_SUCCESS )
  {
    SHIM_MSG_ERROR("loc_geofence_sensor_start_rmd: Unable to disable RMD Sensor Service\n",0, 0,0);
    slim_Close(&slim_GFRmdHandle);
    loc_geofence_rmd_state = RMD_STATE_CLOSED;
    return FALSE;
  }

  return TRUE;
}

#endif /*FEATURE_GEOFENCE_SENSOR_SUPPORT*/
