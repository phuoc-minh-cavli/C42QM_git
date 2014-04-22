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

 $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/loc_mw/src/loc_sensor_cmc_db.c#1 $
 $DateTime: 2020/07/21 21:23:44 $
 $Author: pwbldsvc $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------
 11/29/12   ss      Initial version

=============================================================================*/
#include "customer.h"

#if (defined(FEATURE_GEOFENCE_SENSOR_SUPPORT) && (!defined(FEATURE_SMART_OFFLINE_COMPILATION)))
#include "msg.h"
#include "loc_sensor_cmc_db.h"
#include "gm_api.h"

typedef enum
{
  CMC_DB_STATE_CLOSED,
  CMC_DB_STATE_OPENING,
  CMC_DB_STATE_OPEN,
  CMC_DB_STATE_CLOSING
} cmc_db_state_e_type;

static loc_geofence_cmc_db_state_speed_bound speed_bound_cmc_db;
static uint8_t loc_geofence_sensor_cmc_db_instance_id = 0;
static boolean loc_geofence_sensor_cmc_db_instance_id_valid = FALSE;
static cmc_db_state_e_type cmc_db_state = CMC_DB_STATE_CLOSED;

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

/** loc_geofence_sensor_cmc_db_handle_get_report_resp
*    Handle the CMC Get Report Resp from ADSP.
*  @param [in] set_bound_resp
* */
void loc_geofence_sensor_cmc_db_handle_get_report_resp(slimDistanceBoundDataStructT *get_report_resp)
{
  loc_geofence_cmc_db_ind cmc_db_indicaiton = {0};

  if (get_report_resp == NULL)
  {
    SHIM_MSG_ERROR("CMC DB Get Report Response pointer NULL", 0, 0, 0);
    return;
  }

  memset((void*)&cmc_db_indicaiton, 0, sizeof(cmc_db_indicaiton));
  cmc_db_indicaiton.cmc_db_indication_type = GEOFENCE_CMC_DB_IND_TYPE_GET_REPORT_RESP;
  memscpy((void*)&cmc_db_indicaiton.u.cmc_db_get_report_resp_ind, sizeof(cmc_db_indicaiton.u.cmc_db_get_report_resp_ind),
    (void*)get_report_resp, sizeof(*get_report_resp));

  //gm_inject_cmc_db_indication(&cmc_db_indicaiton);
}

/** loc_geofence_sensor_cmc_db_get_report
*    Handles CMC Get Report request from Geofence module
*  @param [in] void
* */
boolean loc_geofence_sensor_cmc_db_get_report(void)
{
  SHIM_MSG_HIGH("GF_SLIM: get report CMC DB",0,0,0);

  if (cmc_db_state != CMC_DB_STATE_OPEN)
  {
    SHIM_MSG_ERROR("loc_geofence_sensor_cmc_db_get_report server handle invalid", 0, 0, 0);
    return FALSE;
  }

  slim_GetDistanceBoundReport(&slim_GFCmcDbHandle, loc_geofence_sensor_cmc_db_instance_id);

  return TRUE;
}

/** loc_geofence_sensor_cmc_db_set_bound
*    Handles CMC Set Bound request from Geofence module
*  @param [in] distance_to_check
* */
boolean loc_geofence_sensor_cmc_db_set_bound(float distance_to_check)
{
  slimSetDistanceBoundRequestStructT request;

  if (cmc_db_state != CMC_DB_STATE_OPEN)
  {
    SHIM_MSG_ERROR("loc_geofence_sensor_cmc_db_set_bound service not opened", 0, 0, 0);
    return FALSE;
  }

  request.distanceBound = distance_to_check;
  request.sessionKey = 0;

  slim_SetDistanceBound(&slim_GFCmcDbHandle, &request, loc_geofence_sensor_cmc_db_instance_id);

  return TRUE;
}

/** loc_geofence_sensor_cmc_db_handle_enable_resp
 *  Handle the CMC_DB Enable Resp from ADSP. This
 *  should be invoked when CMC_DB Enable request was 
     sent to ADSP previosuly.
 *  @param [in] enable_resp_p
 * */
void loc_geofence_sensor_cmc_db_handle_enable_resp(void)
{
 loc_geofence_cmc_db_ind cmc_db_indicaiton = {0};

 memset((void*)&cmc_db_indicaiton, 0, sizeof(cmc_db_indicaiton)); 
 cmc_db_indicaiton.cmc_db_indication_type = GEOFENCE_CMC_DB_IND_TYPE_ENABLE_RESP;
 
 //gm_inject_cmc_db_indication(&cmc_db_indicaiton);
}



/** loc_geofence_sensor_cmc_db_handle_disable_resp
 *  Handle the CMC_DB Disable Resp from ADSP. This
 *  should be invoked when CMC_DB Disable request was 
     sent to ADSP previosuly.
 *  @param [in] disable_resp_p
 * */
void loc_geofence_sensor_cmc_db_handle_disable_resp(void)
{
 loc_geofence_cmc_db_ind cmc_db_indicaiton = {0};

 memset((void*)&cmc_db_indicaiton, 0, sizeof(cmc_db_indicaiton)); 
 cmc_db_indicaiton.cmc_db_indication_type = GEOFENCE_CMC_DB_IND_TYPE_DISABLE_RESP;
 
 //gm_inject_cmc_db_indication(&cmc_db_indicaiton);
}

/** loc_geofence_sensor_cmc_db_handle_set_bound_resp
 *    Handle the CMC Set Bound Resp from ADSP.
 *  @param [in] set_bound_resp
 * */
void loc_geofence_sensor_cmc_db_handle_set_bound_resp(void)
{
  loc_geofence_cmc_db_ind cmc_db_indicaiton = {0};
  
  memset((void*)&cmc_db_indicaiton, 0, sizeof(cmc_db_indicaiton)); 
  cmc_db_indicaiton.cmc_db_indication_type = GEOFENCE_CMC_DB_IND_TYPE_SET_BOUND_RESP;

  //gm_inject_cmc_db_indication(&cmc_db_indicaiton);
}

/** loc_geofence_sensor_cmc_db_handle_async_report
 *  Handle the CMC_DB Async Report from ADSP. This carries
     ADSP report indications
 *  @param [in] report_ind_p
 * */
void loc_geofence_sensor_cmc_db_handle_async_report(slimDistanceBoundDataStructT *report_ind_p)
{
  loc_geofence_cmc_db_ind cmc_db_indicaiton = {0};

  if(report_ind_p == NULL)
  {
    SHIM_MSG_ERROR("cmc_db Report Indication pointer NULL",0,0,0);
    return;  
  }
  
  SHIM_MSG_HIGH("CMC_DB Report Indication Timestamp %d ",report_ind_p->timestamp,0,0);

  memset((void*)&cmc_db_indicaiton, 0, sizeof(cmc_db_indicaiton)); 
  cmc_db_indicaiton.cmc_db_indication_type = GEOFENCE_CMC_DB_IND_TYPE_REPORT;
  memscpy((void*)&cmc_db_indicaiton.u.cmc_db_report_ind, sizeof(cmc_db_indicaiton.u.cmc_db_report_ind),
    (void*)report_ind_p, sizeof(*report_ind_p));
  
  //gm_inject_cmc_db_indication(&cmc_db_indicaiton);
}

/** loc_geofence_sensor_cmc_db_handle_err_ind
 *    Handle the CMC_DB Error Indication from ADSP.
 *  @param [in] err_ind_p
 * */
void loc_geofence_sensor_cmc_db_handle_err_ind(slimErrorEnumT err_ind_type)
{
  loc_geofence_cmc_db_ind cmc_db_indicaiton = {0};

  SHIM_MSG_HIGH("CMC_DB Error Indication Error %d ",err_ind_type,0,0);

  memset((void*)&cmc_db_indicaiton, 0, sizeof(cmc_db_indicaiton)); 
  cmc_db_indicaiton.cmc_db_indication_type = GEOFENCE_CMC_DB_IND_TYPE_ERROR;

  //gm_inject_cmc_db_indication(&cmc_db_indicaiton);
}

/** loc_geofence_sensor_cmc_db_cb
 *    CMC_DB callback registered when enabling CMC_DB.
 * */
void loc_geofence_sensor_cmc_db_cb
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
    SHIM_MSG_ERROR("loc_geofence_sensor_cmc_db_cb(): NULL header pointer",0,0,0);
    return;
  }

  if( messageHeader->msgError != eSLIM_SUCCESS )
  {
    loc_geofence_sensor_cmc_db_handle_err_ind(messageHeader->msgError);
    return;
  }

  SHIM_MSG_HIGH("GF_SLIM: callback CMC DB: %d",messageHeader->msgId,0,0);

  switch( messageHeader->msgId )
  {
    case eSLIM_MESSAGE_ID_SERVICE_STATUS_IND:
    {
      slimEnableDistanceBoundRequestStructT req;   
      slimServiceStatusEventStructT *service_status = (slimServiceStatusEventStructT *) message;

      if (service_status->service != eSLIM_SERVICE_DISTANCE_BOUND || cmc_db_state == CMC_DB_STATE_OPEN)
        break;

      /*------------------------------------------------------------------
        Check if the Sensor  service is up, if not return. Else,
        Get addressing information for accessing wds service
      ------------------------------------------------------------------*/  
      if(!slim_ServiceAvailable(service_status, eSLIM_SERVICE_DISTANCE_BOUND))
      {
        SHIM_MSG_ERROR("loc_geofence_sensor_cmc_db_get_service(): CMC DB not available", 0, 0, 0);
        slim_Close(&slim_GFCmcDbHandle);
        cmc_db_state = CMC_DB_STATE_CLOSED;
        return;
      }

      memset((void*)&req, 0, sizeof(slimEnableDistanceBoundRequestStructT));

      req.enableConf.enable = TRUE;
      req.enableConf.providerFlags = eSLIM_SERVICE_PROVIDER_SSC;
      req.motionStateSpeedBounds_valid = TRUE;
      req.motionStateSpeedBoundsLength = 8;
      req.motionStateSpeedBounds[SLIM_DB_MS_UNKNOWN] = speed_bound_cmc_db.cmc_db_unknwon_speed_bound;
      req.motionStateSpeedBounds[SLIM_DB_MS_STATIONARY] = speed_bound_cmc_db.cmc_db_stationary_speed_bound;
      req.motionStateSpeedBounds[SLIM_DB_MS_MOVE] = speed_bound_cmc_db.cmc_db_in_motion_speed_bound;
      req.motionStateSpeedBounds[SLIM_DB_MS_FIDDLE] = speed_bound_cmc_db.cmc_db_fiddle_speed_bound;
      req.motionStateSpeedBounds[SLIM_DB_MS_PEDESTRIAN] = speed_bound_cmc_db.cmc_db_pedestrian_speed_bound;
      req.motionStateSpeedBounds[SLIM_DB_MS_VEHICLE] = speed_bound_cmc_db.cmc_db_vehicle_speed_bound;
      req.motionStateSpeedBounds[SLIM_DB_MS_WALK] = speed_bound_cmc_db.cmc_db_walk_speed_bound;
      req.motionStateSpeedBounds[SLIM_DB_MS_RUN] = speed_bound_cmc_db.cmc_db_run_speed_bound;

      if( slim_EnableDistanceBound(&slim_GFCmcDbHandle, &req, loc_geofence_sensor_cmc_db_instance_id) != eSLIM_SUCCESS )
      {
        SHIM_MSG_ERROR("loc_geofence_sensor_start_cmc_db: Unable to enable CMC_DB Sensor Service\n",0, 0,0);
        slim_Close(&slim_GFCmcDbHandle);
        cmc_db_state = CMC_DB_STATE_CLOSED;
        return;
      }
      break;
    }

    case eSLIM_MESSAGE_ID_DISTANCE_BOUND_DATA_GET_IND:
    {
      loc_geofence_sensor_cmc_db_handle_get_report_resp((slimDistanceBoundDataStructT*)message);
      break;
    }

    case eSLIM_MESSAGE_ID_DISTANCE_BOUND_DATA_SET_RESP:
    {
      loc_geofence_sensor_cmc_db_handle_set_bound_resp();
      break;
    }

    case eSLIM_MESSAGE_ID_DISTANCE_BOUND_DATA_ENABLE_RESP:
    {
      cmc_db_state = CMC_DB_STATE_OPEN;
      loc_geofence_sensor_cmc_db_handle_enable_resp();
      break;
	}

    case eSLIM_MESSAGE_ID_DISTANCE_BOUND_DATA_DISABLE_RESP:
    {
      slim_Close(&slim_GFCmcDbHandle);
      cmc_db_state = CMC_DB_STATE_CLOSED;
      loc_geofence_sensor_cmc_db_handle_disable_resp();
      break;
	}
	
    case eSLIM_MESSAGE_ID_DISTANCE_BOUND_DATA_IND:
    {
      loc_geofence_sensor_cmc_db_handle_async_report((slimDistanceBoundDataStructT *) message);
      break;
    }

    default:
      break;
  }
}

/** loc_geofence_sensor_cmc_db_get_service
 *    Handles CMC_DB Get service request. This funciton
      will find and get the CMC_DB service handle.
 * */
boolean loc_geofence_sensor_cmc_db_get_service(void)
{
  SHIM_MSG_HIGH("GF_SLIM: get service CMC DB",0,0,0);

  if(cmc_db_state == CMC_DB_STATE_OPEN)
  {
    SHIM_MSG_MED("CMC DB Serice already discovered\n", 0,0,0);
    return TRUE;
  }

  /*-----------------------------------------------------------------------
     Open SLIM connection
  -----------------------------------------------------------------------*/
  if( slim_Open(&slim_GFCmcDbHandle, loc_geofence_sensor_cmc_db_cb, 0, 0) 
        != eSLIM_SUCCESS)
  {
    SHIM_MSG_ERROR("loc_geofence_sensor_cmc_db_get_service(): Open failed",0,0,0);
    cmc_db_state = CMC_DB_STATE_CLOSED;
    return FALSE;
  }

  return TRUE;
}


/** loc_geofence_sensor_start_cmc_db
 *    Handles CMC_DB Start request from Geofence module. 
 *  @param [in] report_period
 * */
boolean loc_geofence_sensor_start_cmc_db(loc_geofence_cmc_db_state_speed_bound *p_speed_bound)
{  
  SHIM_MSG_HIGH("GF_SLIM: start CMC DB",0,0,0);

  if(cmc_db_state == CMC_DB_STATE_CLOSED)
  {
    cmc_db_state = CMC_DB_STATE_OPENING;
    speed_bound_cmc_db = *p_speed_bound;
    if(loc_geofence_sensor_cmc_db_get_service() == FALSE)
    {      
      SHIM_MSG_ERROR("loc_geofence_sensor_start_cmc_db: Unable to get CMC DB Sensor Service\n",0, 0,0);
      cmc_db_state = CMC_DB_STATE_CLOSED;
      return FALSE;
    }
  }
  else
  {
    SHIM_MSG_ERROR("loc_geofence_sensor_start_cmc_db: CMC DB Sensor Service not closed\n",0, 0,0);
    return FALSE;
  }

  return TRUE;
}


/** loc_geofence_sensor_stop_cmc_db
 *    Handles CMC_DB Stop request from Geofence module. 
 *  @param [in] void
 * */
boolean loc_geofence_sensor_stop_cmc_db(void)
{
  slimEnableDistanceBoundRequestStructT cmc_db_req;

  SHIM_MSG_HIGH("GF_SLIM: stop CMC DB",0,0,0);

  if(cmc_db_state == CMC_DB_STATE_CLOSED)
  {
    SHIM_MSG_ERROR("loc_geofence_sensor_disable_cmc_db: cmc db already closed\n",0, 0,0);
    return FALSE;
  }

  memset((void*)&cmc_db_req, 0, sizeof(cmc_db_req));
  cmc_db_req.enableConf.enable = FALSE;
  cmc_db_req.enableConf.providerFlags = eSLIM_SERVICE_PROVIDER_SSC;

  cmc_db_state = CMC_DB_STATE_CLOSING;
  if( slim_EnableDistanceBound(&slim_GFCmcDbHandle, &cmc_db_req, 0) != eSLIM_SUCCESS )
  {
    SHIM_MSG_ERROR("loc_geofence_sensor_start_cmc_db: Unable to disable CMC_DB Sensor Service\n",0, 0,0);
    slim_Close(&slim_GFCmcDbHandle);
    cmc_db_state = CMC_DB_STATE_CLOSED;
    return FALSE;
  }

  return TRUE;
}

#endif /*FEATURE_GEOFENCE_SENSOR_SUPPORT*/
