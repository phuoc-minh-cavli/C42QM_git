/*============================================================================
 @file loc_geofence.c

 loc MW geofencing module implementation

 GENERAL DESCRIPTION

 This file contains the loc middleware geofencing module implementation.

 Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
 All Rights Reserved
 Qualcomm Confidential and Proprietary

 Copyright (c) 2013-2014 QUALCOMM Atheros, Inc.
 All Rights Reserved.
 QCA Proprietary and Confidential.
 
 Copyright (c) 2015 Qualcomm Techonologies, Inc.  
 Qualcomm Techonologies Confidential and Proprietary. All Rights Reserved. 
 
 Export of this technology or software is regulated by the U.S. Government.
 Diversion contrary to U.S. law prohibited.

 =============================================================================*/

/*============================================================================

 EDIT HISTORY FOR FILE

 This section contains comments describing changes made to the module.

 $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/loc_mw/src/loc_geofence.c#1 $
 $DateTime: 2020/07/21 21:23:44 $
 $Author: pwbldsvc $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------
11/15/16    yh    Local variable scope and init value correction.
05/22/15    ssu   Resolve a bug in setting inside dwell type.
04/02/15    ssu   Batch Dwell notification for geofence.
01/26/15    ssu   Adding geofence custom responsiveness.
11/12/14   sj     Make changes for GF Config items. 
04/03/14   ssu      Additional Fix details parameters for Geofence Breach reports
 03/24/12   ss       Supporting operation mode settting to effect Geofencing operation mode
 12/12/11   nks     Initial version

=============================================================================*/

#include "customer.h"
#include "comdef.h"

#include "msg.h"
#include "loc_geofence.h"
#include "gfc_qmi_api.h"
#include "location_service_iot_v02.h"
#include "loc_api_internal.h"
#include "loc_qmi_shim.h"
#include "loc_sm_interface.h"
#include "gnss_calendar_util.h"
#include "loc_utils.h"
#include "gm_api.h"

/** the maximum number of outstanding requests per geofence.
 */
#define MAX_NUM_CONCURRENT_REQ (1)
#define LOC_GEOFENCE_NON_QMI_TRXN_ID 0xABABABAB

/**
 * Structure tying a client to a particular transaction id
 * identifies the client to whom the response for the
 * transaction should be sent.
 */

typedef struct
{
  /** loc MW client who issued the request, if the handle
     is set to LOC_CLIENT_HANDLE_INVALID then this
     entry is availabe to use.
  */
  int locClientHandle;

  /** identifies if the txn ID is valid, a valid txn Id indicates
      that a request has been sent to gfc but the response has
     not been receievd.
   */
  boolean txnId_valid;

  /** txnId of the request, txnId is used for start and edit
   * geofence requests.
   * */
  uint32 txnId;

}locGeofenceClientReqStructT;



/**==================================GLOBAL VARIABLES========================================================*/

/** global array to store outstanding delete geofence requests */
static locGeofenceClientReqStructT g_deleteReqClientList[MAX_NUM_CONCURRENT_REQ];

/** global array to store outstanding add geofence requests */
static locGeofenceClientReqStructT g_addReqClientList[MAX_NUM_CONCURRENT_REQ];

/** global array to store outstanding geofence set engine
 *  config requests */
static locGeofenceClientReqStructT g_setEngineConfigReqClientList[MAX_NUM_CONCURRENT_REQ];

/** global array to store outstanding geofence get engine config
 *  requests */
static locGeofenceClientReqStructT g_getEngineConfigReqClientList[MAX_NUM_CONCURRENT_REQ];


/**==================================INTERNAL FUNCTION DEFINITIONS==============================================*/

/** locGeofenceResetState
 *  reset the state of all the geofence global variables */

static void locGeofenceResetState(void)
{
  uint32 i = 0;
  for( i=0; i<MAX_NUM_CONCURRENT_REQ;i++)
  {
    g_addReqClientList[i].locClientHandle = LOC_CLIENT_HANDLE_INVALID;
    g_addReqClientList[i].txnId_valid = 0;
    g_deleteReqClientList[i].locClientHandle = LOC_CLIENT_HANDLE_INVALID;
    g_deleteReqClientList[i].txnId_valid = 0;
    g_setEngineConfigReqClientList[i].locClientHandle = LOC_CLIENT_HANDLE_INVALID;
    g_setEngineConfigReqClientList[i].txnId_valid = 0;
    g_getEngineConfigReqClientList[i].locClientHandle = LOC_CLIENT_HANDLE_INVALID;
    g_getEngineConfigReqClientList[i].txnId_valid = 0;
  }
}

/** addTxnIdToClientReq
 *  checks if there is a free entry in the client req list and
 *  adds a txn Id to it.
 *  @param [in] pReqClient : pointer to the request client
 *         list where txn id has to be added
 *  @param [in] : locClientHandle : handle of the request sender
 *  @param [in] : transactionId   : txn id to be populated in
 *         the request list.
 *  @return TRUE -- The handle/txn Id were stored in the table.
 *          FALSE -- The handle/txn Id could not be stored in
 *          the table.
 */

static boolean addTxnIdToClientReq(
  locGeofenceClientReqStructT *pReqClient,
  int locClientHandle, uint32 transactionId)
{
  uint32 i = 0;

  if(NULL == pReqClient)
  {
    return FALSE;
  }

  /** check if there is a entry free in the request table */
  for( i=0; i<MAX_NUM_CONCURRENT_REQ ; i++)
  {
    if(LOC_CLIENT_HANDLE_INVALID == pReqClient[i].locClientHandle)
    {
      /** found an empty slot */
      pReqClient[i].locClientHandle = locClientHandle;
      /** store the transaction id */
      pReqClient[i].txnId_valid = TRUE;
      pReqClient[i].txnId = transactionId;
      break;
    }
  }

  /** if max requests exceed, return error */
  if(i >= MAX_NUM_CONCURRENT_REQ)
  {
    LOC_MSG_ERROR("addTxnIdToClientReq : max concurrent requests exceeded \n",
                    0,0,0);
    return FALSE;
  }

  return TRUE;
}

/** clearTxnIdFromClientReq
 *  Clears the entry corresponding to the txnd ID in the
 *  clientReqList
 *  @param [in] pReqClient : pointer to the request client list
 *         where an entry has to be cleared.
 *  @param [in] : locClientHandle : handle of the request sender
 *  @param [in] : transactionId   : txn id
 *  @return TRUE -- entry was cleared.
 *          FALSE -- entry could not be cleared.
 */

static boolean clearTxnIdFromClientReq(
  locGeofenceClientReqStructT *pReqClient,
  int locClientHandle, uint32 transactionId)
{
  uint32 i = 0;

  if(NULL == pReqClient)
  {
    return FALSE;
  }

  /** find the entry in the request table */
  for( i=0; i<MAX_NUM_CONCURRENT_REQ ; i++)
  {
    if(transactionId == pReqClient[i].txnId &&
       locClientHandle == pReqClient[i].locClientHandle )
    {
      /** found the entry, invalidate it  */
      pReqClient[i].locClientHandle = LOC_CLIENT_HANDLE_INVALID ;
      pReqClient[i].txnId_valid = FALSE;
      break;
    }
  }

  /** if max requests exceed, return error */
  if(i >= MAX_NUM_CONCURRENT_REQ)
  {
    LOC_MSG_ERROR("clearTxnIdFromClientReq : Did not find the entry "
                  "handle = %d, txnID = %u \n", locClientHandle,transactionId,0);
    return FALSE;
  }

  return TRUE;
}

/** sendIndicationByTxnId
 *  Matches the transaction Id and sends an indication to
 *  the corresponding client
 *  @param [in/out] pReqClient -- pointer to client/request
 *         table. The table entry is cleared after the
 *         indication is sent.
 *  @param [in] transactionId
 *  @param[in]  pShimIndication -- pointer to the SHIM
 *        indication to send out
 *  @return -- TRUE : If the indication was sent to SHIM
 *          -- FALSE : If indication could not be sent   */

boolean sendIndicationByTxnId (
  locGeofenceClientReqStructT * const pClientReqInfo,
  uint32 transactionId,
  const locQmiShimIndInfoStructT * const pShimIndication )
{
  uint32 i = 0;
  /** sanity check the input */
  if(NULL == pClientReqInfo || NULL == pShimIndication)
  {
    LOC_MSG_ERROR("sendIndicationByTxnId: Null input params\n", 0,0,0);
    return FALSE;
  }
  /** traverse the client list and send edit indications to the
   *  clients who issued the corresponding request
   */
  for(i = 0; i < MAX_NUM_CONCURRENT_REQ; i++)
  {
    /** if txnId matches the indication txnInd send the
     *  indication back to to the corresponding client */
    if((LOC_CLIENT_HANDLE_INVALID != pClientReqInfo[i].locClientHandle) &&
       (pClientReqInfo[i].txnId_valid) &&
       (pClientReqInfo[i].txnId == transactionId ))
    {
      if( FALSE == locQmiShimSendInd(pClientReqInfo[i].locClientHandle,
                                     pShimIndication) )
      {
            LOC_MSG_ERROR("sendIndicationByTxnId: could not send Ind %u"
                         "for client %d\n", pShimIndication->q_Id,
                         pClientReqInfo[i].locClientHandle,0);
      }

      /** invalidate the entry */
      pClientReqInfo[i].txnId_valid = FALSE;
      pClientReqInfo[i].locClientHandle = LOC_CLIENT_HANDLE_INVALID;

      /** done; break out of the for loop */
      break;
    }
  }

  if(i >= MAX_NUM_CONCURRENT_REQ)
  {
    LOC_MSG_ERROR("sendIndicationByTxnId: did not find txn ID %u for client %d\n",
                 transactionId, pClientReqInfo[i].locClientHandle, 0);
    return FALSE;
  }
  return TRUE;
}


/** locGeofenceGenerateIPCtolocMW
 *  Generate an IPC to loc MW
 *  @param [in] msgId -- loc MW IPC message id
 *  @param [in] payloadSize -- payload size
 *  @param [in] pPayload --  pointer to the payload
 *  @return -- TRUE is IPC was sent successfully
 *          -- FALSE otherwise.
 **/
static boolean locGeofenceGenerateIPCtolocMW(
  loc_middleware_msg_id_e_type msgId,
  size_t payloadSize, const void * const pPayload )
{
  os_IpcMsgType * const pIpcMsg =
  os_IpcCreate(payloadSize, IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_LOC_MIDDLEWARE);
  if(NULL != pIpcMsg )
  {
    /** copy the payload*/
    (void)memscpy(pIpcMsg->p_Data, payloadSize, pPayload, payloadSize);

    pIpcMsg->q_MsgId = msgId;
    return(os_IpcSend(pIpcMsg, THREAD_ID_LOC_MIDDLEWARE) );
  }
  return FALSE;
}

/******************* CONVERSION FUNCTIONS BETWEEN QMI_LOC and GFC*************************************************/

/** convertGmStatusToQmiLocResp
 *  @param [in] status : GFC operation status
 *  @param [in] returnCode : GFC detailed return code
 *  @param [out] pResp:  QMI_LOC response type
 *  @return -- TRUE : If GFC status contained no error
 *          -- FALSE : If GFC status contained an error
 * */

static boolean convertGmStatusToQmiLocResp(
  gm_operation_status_type status,
  uint8 returnCode,
  qmiLocGenRespMsgT_v02 * const pResp)
{
  boolean b_Retval;
  if(FAILED == status )
  {
	pResp->resp.result = QMI_RESULT_FAILURE_V01;
	switch(returnCode)
	{
	  case GEOFENCE_ERROR_NONE:
	    {
	       pResp->resp.error = QMI_ERR_NONE_V01;
	       break;
	    }
	  case GEOFENCE_ERROR_MEMORY_ALLOC_FAIL:
	  {
	    pResp->resp.error = QMI_ERR_NO_MEMORY_V01;
	    break;
	  }
	  case GEOFENCE_ERROR_GEOFENCE_NOT_FOUND:
	  {
	    pResp->resp.error = QMI_ERR_INVALID_ARG_V01;
	    break;
	  }
	  case GEOFENCE_ERROR_IPC_SEND_FAIL:
	  default:
	  {
        pResp->resp.error = QMI_ERR_GENERAL_V01;
	    break;
	  }
	}

   /** The GM status contained an error   */
    b_Retval = FALSE;
  }
  else //PENDING OR SUCCESS
  {
	pResp->resp.error = 0;
    pResp->resp.result = QMI_RESULT_SUCCESS_V01;

    /** the GM status contained no error */
    b_Retval = TRUE;
  }
  return b_Retval;
}

/** convertAddCircularGeofenceReqToGm
 *  convert an add circular geofence request from QMI_LOC to GM
 *  format
 *  @param [in] pAddGeofenceReq : QMI LOC add request
 *  @param [out]  pGmAddGeofenceReq : GFC add geofence request
 *  @return  TRUE -- if the conversion succeeded
 *           FALSE -- if there were errors, invalid input
 **/

static boolean convertAddCircularGeofenceReqToGm (
  const qmiLocAddCircularGeofenceReqMsgT_v02* const pAddGeofenceReq,
  sm_gm_add_request_type* const pGmAddGeofenceReq)
{
  double d_lat_local = 0;
  double d_long_local = 0;
  if(NULL == pAddGeofenceReq || NULL == pGmAddGeofenceReq )
  {
    LOC_MSG_ERROR("convertAddCircularGeofenceReqToGm: NULL param(s)\n",
                 0,0,0);
    return FALSE;
  }

  if(FALSE == gfc_qmi_get_gm_client_handle(&pGmAddGeofenceReq->client_handle))
  {
    LOC_MSG_ERROR("convertAddCircularGeofenceReqToGm: NULL client handle\n",
                 0,0,0);
    return FALSE;
  }

  /** copy the transaction Id */
  pGmAddGeofenceReq->q_transaction_id = gfc_qmi_getnext_gm_trxn_id(pAddGeofenceReq->transactionId);

  if(pGmAddGeofenceReq->q_transaction_id == GFC_QMI_INVALID_TRXN_ID)
  {
    /* Failed to get the GM transaction ID . Report error  */
    return FALSE;
  }


  /** copy the mandatory fields */

  /** convert the breach mask*/
  if(pAddGeofenceReq->breachMask & QMI_LOC_GEOFENCE_BREACH_ENTERING_MASK_V02 )
  {
    pGmAddGeofenceReq->geofence.event_type|=GEOFENCE_EVENT_ENTERING_AREA;
  }
  if(pAddGeofenceReq->breachMask & QMI_LOC_GEOFENCE_BREACH_LEAVING_MASK_V02 )
  {
    pGmAddGeofenceReq->geofence.event_type|=GEOFENCE_EVENT_LEAVING_AREA;
  }

  /** convert the target area */
  pGmAddGeofenceReq->geofence.target_area.target_area_type = 
  GEOFENCE_TARGET_AREA_CIRCULAR_TYPE;

  // Factor to convert degrees ( referenced to WGS-84 ) into radians = ((1<<25)/180)
  d_lat_local = pAddGeofenceReq->circularGeofenceArgs.latitude;
  d_long_local = pAddGeofenceReq->circularGeofenceArgs.longitude;
  pGmAddGeofenceReq->geofence.target_area.u.circ_target_area.latitude = (sint31)((d_lat_local*(1<<25))/180);
  pGmAddGeofenceReq->geofence.target_area.u.circ_target_area.longitude = (sint31)((d_long_local*(1<<25))/180);
  pGmAddGeofenceReq->geofence.target_area.u.circ_target_area.radius = pAddGeofenceReq->circularGeofenceArgs.radius;

  if( TRUE == pAddGeofenceReq->includePosition)
  {
    pGmAddGeofenceReq->geofence.location_estimate = TRUE;
  }

  /** copy the optional fields */

  /** copy confidence */
  if(pAddGeofenceReq->confidence_valid)
  {
    switch(pAddGeofenceReq->confidence)
    {
      case eQMI_LOC_GEOFENCE_CONFIDENCE_LOW_V02:
        pGmAddGeofenceReq->geofence.confidence = GEOFENCE_CONFIDENCE_LOW;
        break;
      case eQMI_LOC_GEOFENCE_CONFIDENCE_MED_V02:
        pGmAddGeofenceReq->geofence.confidence = GEOFENCE_CONFIDENCE_MEDIUM;
        break;
      case eQMI_LOC_GEOFENCE_CONFIDENCE_HIGH_V02:
        pGmAddGeofenceReq->geofence.confidence = GEOFENCE_CONFIDENCE_HIGH;
        break;
      default:
        //error return with an error
        return FALSE;
    }
  }
  else
  {
	/* Set default confidence level */
    pGmAddGeofenceReq->geofence.confidence = GEOFENCE_CONFIDENCE_HIGH; 
  }

  if(GEOFENCE_CONFIDENCE_LOW != pGmAddGeofenceReq->geofence.confidence)
  {
    /* Enable dynamic responsiveness only if the geofence is not a LOW_CONFIDENCE geofence */
    pGmAddGeofenceReq->geofence.dynamic_responsiveness = TRUE;
  }

  /** Copy the custom responsiveness value if present. If the
   *  custom responsiveness value is set then consider only
   *  this value and disregard the other responsiveness setting */
  if(pAddGeofenceReq->customResponsivenessValue_valid)
  {
    /** Old HLOS/AP will send only responsiveness as High, Medium
        and Low. While new HLOS/AP will send both responsiveness
        set to High, Medium or Low and also set the value in custom
        responsiveness. This is for backward compatibility to take
        care of older modems which wont have custom responsiveness
        support*/
    pGmAddGeofenceReq->geofence.q_custom_responsiveness_secs = pAddGeofenceReq->customResponsivenessValue;
    pGmAddGeofenceReq->geofence.responsiveness = GEOFENCE_RESPONSIVENESS_CUSTOM;
  }
  else if(pAddGeofenceReq->responsiveness_valid)
  {
    /** Consider the other responsiveness setting   */
    switch(pAddGeofenceReq->responsiveness)
    {
      case eQMI_LOC_GEOFENCE_RESPONSIVENESS_LOW_V02:
        pGmAddGeofenceReq->geofence.responsiveness = GEOFENCE_RESPONSIVENESS_LOW;
        break;
      case eQMI_LOC_GEOFENCE_RESPONSIVENESS_MED_V02:
        pGmAddGeofenceReq->geofence.responsiveness = GEOFENCE_RESPONSIVENESS_MEDIUM;
        break;
      case eQMI_LOC_GEOFENCE_RESPONSIVENESS_HIGH_V02:
        pGmAddGeofenceReq->geofence.responsiveness = GEOFENCE_RESPONSIVENESS_HIGH;
        break;
      case eQMI_LOC_GEOFENCE_RESPONSIVENESS_ULTRA_HIGH_V02:
        pGmAddGeofenceReq->geofence.responsiveness = GEOFENCE_RESPONSIVENESS_ULTRA_HIGH;
        break;
      case eQMI_LOC_GEOFENCE_RESPONSIVENESS_CUSTOM_V02:
        pGmAddGeofenceReq->geofence.responsiveness = GEOFENCE_RESPONSIVENESS_CUSTOM;
        LOC_MSG_ERROR("Custom resp value not specified\n",
                 0,0,0);
        return FALSE;
      default:
        //error return with an error
        return FALSE;
    }
  }

  /** duration is INFINITY and start time is NOW */
  pGmAddGeofenceReq->geofence.stop_time = 0xFFFFFFFF;
  pGmAddGeofenceReq->geofence.start_time = 0;

  /*Have context usage enabled by default*/
  pGmAddGeofenceReq->geofence.context_usage = TRUE;
  /*Enable Hystersis*/
  pGmAddGeofenceReq->geofence.hysteresis_required = TRUE;

  // ToDo: No flag from QMI for this . We hardcode it for now.
  pGmAddGeofenceReq->geofence.target_area_provided = TRUE;

  // ToDo:: Hardcoding as of now.
  pGmAddGeofenceReq->geofence.num_of_area_ids = 0;
  pGmAddGeofenceReq->geofence.area_id_list = NULL;
  pGmAddGeofenceReq->geofence.max_num_reports = 0xFFFFFFFF;

  return TRUE;
}


/** convertSetGeofenceEngineConfigReqToGm
 *  convert set geofence engine config request from QMI_LOC to
 *  GFC format
 *  @param [in] pEditGeofenceReq : QMI LOC Edit request ptr
 *  @param [in] pGfcEditGeofenceReq : GFC edit geofence request
 *         ptr
 *  @return  TRUE -- if the conversion succeeded
 *           FALSE -- if there were errors, invalid input
 **/

static boolean convertSetGeofenceEngineConfigReqToGm (
  const qmiLocSetGeofenceEngineConfigReqMsgT_v02* const pSetGeofenceEngineConfigReq,
  sm_gm_set_geofence_engine_config_request_type* const pGmSetGeofenceEngineConfigReq,
  sm_gm_set_geofence_client_config_request_type* const pGmSetGeofenceClientConfigReq)
{
  boolean b_retval = FALSE;

  if((NULL == pSetGeofenceEngineConfigReq) ||
     (NULL == pGmSetGeofenceEngineConfigReq ) ||
	 (NULL == pGmSetGeofenceClientConfigReq))
  {
    LOC_MSG_ERROR("convertSetGeofenceEngineConfigReqToGm: NULL param(s)\n",
                 0,0,0);
    return FALSE;
  }

  pGmSetGeofenceEngineConfigReq->q_transaction_id = gfc_qmi_getnext_gm_trxn_id(pSetGeofenceEngineConfigReq->transactionId);

  pGmSetGeofenceClientConfigReq->q_transaction_id = gfc_qmi_getnext_gm_trxn_id(pSetGeofenceEngineConfigReq->transactionId);

  if((GFC_QMI_INVALID_TRXN_ID == pGmSetGeofenceEngineConfigReq->q_transaction_id)||
	 (GFC_QMI_INVALID_TRXN_ID == pGmSetGeofenceClientConfigReq->q_transaction_id))
  {
    MSG_1(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_ERROR,"loc_gf:Set engine config invalid TID %d", pSetGeofenceEngineConfigReq->transactionId);
    return FALSE;
  }

   /*************** Populate GM engine config *************************/
  if(pSetGeofenceEngineConfigReq->gnssUnavailableIndicationTimeout_valid)
  {
    b_retval = TRUE;
	pGmSetGeofenceEngineConfigReq->q_mask |= SM_GM_ENGINE_CONFIG_GNSS_UNAVAIL_TIMEOUT_MASK;
    pGmSetGeofenceEngineConfigReq->q_gnss_unavailable_indication_timeout = pSetGeofenceEngineConfigReq->gnssUnavailableIndicationTimeout;
  }
  else if(pSetGeofenceEngineConfigReq->enableCpiUsage_valid)
  {
    b_retval = TRUE;
	pGmSetGeofenceEngineConfigReq->q_mask |= SM_GM_ENGINE_CONFIG_ENABLE_POSITION_INJECTION_MASK;
    pGmSetGeofenceEngineConfigReq->q_enable_position_injection = pSetGeofenceEngineConfigReq->enableCpiUsage;
  }
  else if(pSetGeofenceEngineConfigReq->enableMotionDetectionSources_valid)
  {
    /* Start by setting all the sources to disabled , then enable based on the setting */
    pGmSetGeofenceEngineConfigReq->q_enable_sensor_motion_detection = FALSE;

    if(pSetGeofenceEngineConfigReq->enableMotionDetectionSources&QMI_LOC_MOTION_DETECTION_SOURCE_SENSORS_V02)
    {
	  pGmSetGeofenceEngineConfigReq->q_mask |= SM_GM_ENGINE_CONFIG_ENABLE_SENSOR_MOTION_DETECTION_MASK;
      pGmSetGeofenceEngineConfigReq->q_enable_sensor_motion_detection  = TRUE;
    }
    b_retval = TRUE;
  }
  else if(pSetGeofenceEngineConfigReq->gnssPositionMaxPuncAcceptable_valid)
  {
	pGmSetGeofenceEngineConfigReq->q_mask |= SM_GM_ENGINE_CONFIG_GNSS_MAX_POS_UNC_ACCEPTED_MASK;
    pGmSetGeofenceEngineConfigReq->q_gnss_max_pos_unc_accepted =  pSetGeofenceEngineConfigReq->gnssPositionMaxPuncAcceptable;
    b_retval = TRUE;
  }

  if(0 == pGmSetGeofenceEngineConfigReq->q_mask)
  {
    /*Release the TxID*/
    (void)gfc_qmi_get_qmi_trxn_id(pGmSetGeofenceEngineConfigReq->q_transaction_id);
  }

  /********** Populate GM client config ******************/
  if(pSetGeofenceEngineConfigReq->gnssPositionSessionTimeout_valid)
  {
    pGmSetGeofenceClientConfigReq->config_request.q_mask |= SM_GM_CLIENT_CONFIG_GNSS_POS_QOS_SESS_TIMEOUT_MASK;
	pGmSetGeofenceClientConfigReq->config_request.q_gnss_pos_qos_sess_timeout = pSetGeofenceEngineConfigReq->gnssPositionSessionTimeout;
    b_retval = TRUE;
  }

  else if(pSetGeofenceEngineConfigReq->maxGeofences_valid)
  {
    b_retval = TRUE;
    pGmSetGeofenceClientConfigReq->config_request.q_mask |= SM_GM_CLIENT_CONFIG_MAX_GEOFENCES_MASK;
    pGmSetGeofenceClientConfigReq->config_request.q_max_geofences = pSetGeofenceEngineConfigReq->maxGeofences;
  }

  else if(pSetGeofenceEngineConfigReq->chalGnssEnvMinCpiWaitInterval_valid)
  {
	pGmSetGeofenceClientConfigReq->config_request.q_mask |= SM_GM_CLIENT_CONFIG_CPI_REQUEST_RATE_MASK;
	pGmSetGeofenceClientConfigReq->config_request.q_cpi_request_rate =  pSetGeofenceEngineConfigReq->chalGnssEnvMinCpiWaitInterval;
    b_retval = TRUE;
  }

  if(0 == pGmSetGeofenceClientConfigReq->config_request.q_mask)
  {
    /*Release the TxID*/
    (void)gfc_qmi_get_qmi_trxn_id(pGmSetGeofenceClientConfigReq->q_transaction_id);
  }

  MSG_2(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_HIGH,"loc_gf: Set GM engine config mask-0x%x GM TID-%d",
	                                         pGmSetGeofenceEngineConfigReq->q_mask,
											 pGmSetGeofenceEngineConfigReq->q_transaction_id);

  MSG_2(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_HIGH,"loc_gf: Set GM client config mask-0x%x GM TID-%d",
	                                         pGmSetGeofenceClientConfigReq->config_request.q_mask,
											 pGmSetGeofenceClientConfigReq->q_transaction_id);

  return b_retval;
}


/*********************************** CALLBACKS invoked by GFC ******************************************/

/** locGeofenceAddRespCb
 *  Add response callback called in SM context. The callback
 *  checks if the origin is NI or MO, converts the payload from
 *  GFC to QMI LOC format, and generates the appropriate IPC msg
 *  to loc MW.
 *  @param [in] addGeofenceInd
 *   */

static void locGeofenceAddRespCb(
  const qmiLocAddCircularGeofenceIndMsgT_v02 * const pAddGeofenceInd)
{
  void *pPayload = NULL;
  size_t payloadSize = 0;
  loc_middleware_msg_id_e_type msgId = 0;  //enum value starts from 10
  boolean sendIPCFlag = FALSE;

  qmiLocAddCircularGeofenceIndMsgT_v02 geofenceAddInd = {0};

  if(NULL != pAddGeofenceInd)
  {
    memscpy(&geofenceAddInd, sizeof(geofenceAddInd), pAddGeofenceInd, sizeof(*pAddGeofenceInd));

    /** send IPC to loc MW for MO geofence add indication */
    pPayload = (void *)&geofenceAddInd;
    msgId = LM_MIDDLEWARE_MSG_ID_GEOFENCE_ADD_IND;
    payloadSize = sizeof(geofenceAddInd);
    sendIPCFlag = TRUE;

    if( TRUE == sendIPCFlag)
    {
      /** Generate an IPC to loc MW*/
      if(FALSE == locGeofenceGenerateIPCtolocMW(msgId, payloadSize,pPayload ))
      {
        LOC_MSG_ERROR("locGeofenceAddRespCb: could not send IPC to loc MW\n",
                     0,0,0);
      }
    }
  }

  return;

}

/** locGeofenceDeleteRespCb
 *  Delete indication callback called in SM context. The
 *  callback checks if the origin is NI or MO, converts the
 *  payload from GFC to QMI LOC format, and generates the
 *  appropriate IPC msg to loc MW.
 *  @param [in] deleteGeofenceInd
 *   */

static void locGeofenceDeleteRespCb(
  const qmiLocDeleteGeofenceIndMsgT_v02 * const pDeleteGeofenceInd)
{
  void *pPayload = NULL;
  size_t payloadSize = 0;
  loc_middleware_msg_id_e_type msgId = 0;
  boolean sendIPCFlag = FALSE;
  qmiLocDeleteGeofenceIndMsgT_v02 geofenceDelInd = {0};

  if(NULL != pDeleteGeofenceInd)
  {
    memscpy(&geofenceDelInd, sizeof(geofenceDelInd), pDeleteGeofenceInd, sizeof(*pDeleteGeofenceInd));

    /** loc MW always sets duration to infinity so the only
       *  reason handled is if MO client deleted the geofence */

    /** send IPC to loc MW for MO geofence delete indication */
    pPayload = (void *)&geofenceDelInd;
    msgId = LM_MIDDLEWARE_MSG_ID_GEOFENCE_DELETE_IND;
    payloadSize = sizeof(geofenceDelInd);
    sendIPCFlag = TRUE;

    if( TRUE == sendIPCFlag)
    {
      /** Generate an IPC to loc MW*/
      if(FALSE == locGeofenceGenerateIPCtolocMW(msgId, payloadSize,pPayload ))
      {
        LOC_MSG_ERROR("locGeofenceDeleteRespCb: could not send IPC to loc MW\n",
                     0,0,0);
      }
    }
  }
  else
  {
    LOC_MSG_ERROR("locGeofenceDeleteRespCb: NULL payload\n",
                 0,0,0);
  }

  return;
}


/** locGeofencePurgeRespCb Purge indication callback called in
 *  SM context. No IPC is generated since this is an internal
 *  function, called only when a geofence client disconnects.
 *  @param [in]  gfcResult : result of the purge operation
 */

static void locGeofencePurgeRespCb(
  const  gfc_qmi_operation_result_e_type gfcResult)
{
  LOC_MSG_MED("locGeofencePurgeRespCb: Purge result = %d\n",
                 gfcResult,0,0);

  if(GFC_QMI_ERROR_NONE != gfcResult )
  {
    LOC_MSG_ERROR("locGeofencePurgeRespCb: Purge failed error = %d\n",
                 gfcResult,0,0);
  }
  return;
}

/** locGeofenceGenAlertRespCb
 *  General Alert indication callback called in SM context. The
 *  callback converts the payload from GFC to QMI LOC format,
 *  and generates the appropriate IPC msg to loc MW.
 *  @param [in] pGeofenceGenAlertInd
 */
static void locGeofenceGenAlertRespCb(
  const qmiLocEventGeofenceGenAlertIndMsgT_v02 *geofenceGenAlertInd)
{
  qmiLocEventGeofenceGenAlertIndMsgT_v02 genAlertInd = {0};

  if(NULL != geofenceGenAlertInd)
  {
    genAlertInd.geofenceAlert = geofenceGenAlertInd->geofenceAlert;
    /** Generate an IPC to loc MW*/
    if(FALSE == locGeofenceGenerateIPCtolocMW(
      LM_MIDDLEWARE_MSG_ID_GEOFENCE_GEN_ALERT_IND,
      sizeof(genAlertInd),
      (void *)&genAlertInd))
    {
      LOC_MSG_ERROR("locGeofenceGenAlertRespCb: could not send IPC to loc MW\n",
                    0,0,0);
    }
  }
  return;
}

/** locGeofenceBreachRespCb
 *  Breach indication callback called in SM context. The
 *  callback converts the payload from GFC to QMI LOC format,
 *  and generates the appropriate IPC msg to loc MW.
 *  @param [in] pGeofenceBreachInd
 */

static void locGeofenceBreachRespCb(
  const qmiLocEventGeofenceBreachIndMsgT_v02 * const pGeofenceBreachInd)
{
  qmiLocEventGeofenceBreachIndMsgT_v02 breachInd = {0};
  if(NULL != pGeofenceBreachInd)
  {
	memscpy(&breachInd, sizeof(breachInd), pGeofenceBreachInd, sizeof(*pGeofenceBreachInd));
    /** Generate an IPC to loc MW*/
    if(FALSE == locGeofenceGenerateIPCtolocMW(
      LM_MIDDLEWARE_MSG_ID_GEOFENCE_BREACH_IND,
      sizeof(breachInd),
      (void *)&breachInd))
    {
      LOC_MSG_ERROR("locGeofenceBreahcRespCb: could not send IPC to loc MW\n",
                   0,0,0);
    }
    
  }
  else
  {
    LOC_MSG_ERROR("locGeofenceBreahcRespCb: NULL payload\n",
                 0,0,0);
  }
  return;
}


/** locGeofenceSetEngineConfigRespCb
 *  Set geofence engine config indication callback called in SM
 *  context. The callback converts the payload from GFC to QMI
 *  LOC format, and generates the appropriate IPC msg to loc MW.
 *  @param [in] pGfcSetEngineConfigInd
 */

static void locGeofenceSetEngineConfigRespCb(
  const qmiLocSetGeofenceEngineConfigIndMsgT_v02 * const pGfcSetEngineConfigInd)
{
  void *pPayload = NULL;
  size_t payloadSize = 0;
  loc_middleware_msg_id_e_type msgId = 0;
  qmiLocSetGeofenceEngineConfigIndMsgT_v02 geofenceSetEngineConfigInd = {0};

  if(NULL != pGfcSetEngineConfigInd)
  {
    memscpy(&geofenceSetEngineConfigInd, sizeof(geofenceSetEngineConfigInd), pGfcSetEngineConfigInd, sizeof(*pGfcSetEngineConfigInd));
    /** send IPC to loc MW for set engine config indication */
    pPayload = (void *)&geofenceSetEngineConfigInd;
    msgId = LM_MIDDLEWARE_MSG_ID_GEOFENCE_SET_ENGINE_CONFIG_IND;
    payloadSize = sizeof(geofenceSetEngineConfigInd);

    /** Generate an IPC to loc MW*/
    if(FALSE == locGeofenceGenerateIPCtolocMW(msgId, payloadSize,pPayload ))
    {
      LOC_MSG_ERROR("locGeofenceSetEngineConfigRespCb: could not send IPC to loc MW\n",
                   0,0,0);
    }  
  }
  else
  {
    LOC_MSG_ERROR("locGeofenceSetEngineConfigRespCb: NULL payload\n",
                 0,0,0);
  }
  return;
}

/**============================= FUNCTION DEFINITIONS=====================================================*/

/** locGeofenceHandleAddInd
 *  Handle the add indication sent by GFC QMI module. This
 *  should be invoked when: a) loc_geofence module had sent an
 *  add geofence request previously.
 *  @param [in] pIpcMsg
 * */

void locGeofenceHandleAddInd(const os_IpcMsgType* const pIpcMsg)
{
  if((NULL != pIpcMsg) &&
     (pIpcMsg->q_Size == sizeof(qmiLocAddCircularGeofenceIndMsgT_v02)))
  {
    locQmiShimIndInfoStructT shimIndInfo = {0};
    qmiLocAddCircularGeofenceIndMsgT_v02 geofenceAddInd = {0};

    shimIndInfo.q_MsgLen = memscpy(&geofenceAddInd, sizeof(geofenceAddInd),
                                   pIpcMsg->p_Data, pIpcMsg->q_Size);

    /** send indication to the SHIM */
    shimIndInfo.p_Msg = &geofenceAddInd;
    shimIndInfo.q_Id = QMI_LOC_ADD_CIRCULAR_GEOFENCE_IND_V02;
    (void)sendIndicationByTxnId(g_addReqClientList,
                                geofenceAddInd.transactionId,
                                &shimIndInfo);
  }
  else
  {
    LOC_MSG_ERROR("locGeofenceHandleAddInd: Invalid IPC message\n", 0,0,0);
  }

  return;
}

/** locGeofenceHandleDeleteInd
 *  Handle the delete indication sent by GFC QMI module. This
 *  should be invoked when loc_geofence module had sent an
 *  delete geofence request previously.
 *  @param [in] pIpcMsg
 */

void locGeofenceHandleDeleteInd(const os_IpcMsgType* const pIpcMsg)
{
  if((NULL != pIpcMsg) &&
     (pIpcMsg->q_Size == sizeof(qmiLocDeleteGeofenceIndMsgT_v02)))
  {
    locQmiShimIndInfoStructT shimIndInfo = {0};
    qmiLocDeleteGeofenceIndMsgT_v02 geofenceDeleteInd = {0};
    shimIndInfo.q_MsgLen = memscpy(&geofenceDeleteInd, sizeof(geofenceDeleteInd),
                                   pIpcMsg->p_Data, pIpcMsg->q_Size);

    /** send indication to the SHIM */
    shimIndInfo.p_Msg = &geofenceDeleteInd;
    shimIndInfo.q_Id = QMI_LOC_DELETE_GEOFENCE_IND_V02;
    (void)sendIndicationByTxnId(g_deleteReqClientList,
                                geofenceDeleteInd.transactionId,
                                &shimIndInfo);
  }
  else
  {
    LOC_MSG_ERROR("locGeofenceHandleDeleteInd: Invalid IPC message\n", 0,0,0);
  }

  return;
}


/** locGeofenceHandleGenAlertInd
 *  Handle the geofence general alert indication sent by GFC QMI
 *  module. This indication should be sent as an event to the
 *  client.
 *  @param [in] pIpcMsg
 * */

void locGeofenceHandleGenAlertInd(const os_IpcMsgType* const pIpcMsg)
{
  if((NULL != pIpcMsg) &&
     (pIpcMsg->q_Size == sizeof(qmiLocEventGeofenceGenAlertIndMsgT_v02)))
  {
    qmiLocEventGeofenceGenAlertIndMsgT_v02 genAlertInd = {0};
    locQmiShimIndInfoStructT shimIndInfo = {0};

    shimIndInfo.q_MsgLen = memscpy(&genAlertInd, sizeof(genAlertInd),
                                   pIpcMsg->p_Data, pIpcMsg->q_Size);

    // TBD: send a unicast message
    shimIndInfo.p_Msg = &genAlertInd;
    shimIndInfo.q_Id = QMI_LOC_EVENT_GEOFENCE_GEN_ALERT_IND_V02;
    // Broadcast to all registered clients for now
    if(FALSE == locQmiShimBroadcastInd(
      QMI_LOC_EVENT_MASK_GEOFENCE_GEN_ALERT_V02,
      &shimIndInfo))
    {
      LOC_MSG_ERROR("locGeofenceHandleGenAlertInd: Could not send indication\n",
                    0,0,0);
    }
  }
  else
  {
    LOC_MSG_ERROR("locGeofenceHandleGenAlertInd: Invalid IPC message\n", 0,0,0);
  }

  return;
}

/** locGeofenceHandleBreachInd
 *  Handle the breach indication sent by GFC QMI module. This
 *  indicatio should be sent as an event to the client.
 *  @param [in] pIpcMsg
 * */

void locGeofenceHandleBreachInd(const os_IpcMsgType* const pIpcMsg)
{
  if((NULL != pIpcMsg) &&
     (pIpcMsg->q_Size == sizeof(qmiLocEventGeofenceBreachIndMsgT_v02)))
  {
    qmiLocEventGeofenceBreachIndMsgT_v02 breachInd = {0};
    locQmiShimIndInfoStructT shimIndInfo = {0};

    shimIndInfo.q_MsgLen = memscpy(&breachInd, sizeof(breachInd), pIpcMsg->p_Data, pIpcMsg->q_Size);

    shimIndInfo.p_Msg = &breachInd;
    shimIndInfo.q_Id = QMI_LOC_EVENT_GEOFENCE_BREACH_NOTIFICATION_IND_V02;

    /** Broadcast to all registered clients */
    if(FALSE == locQmiShimBroadcastInd(
      QMI_LOC_EVENT_MASK_GEOFENCE_BREACH_NOTIFICATION_V02,
      &shimIndInfo))
    {
      LOC_MSG_ERROR("locGeofenceHandleBreachInd: Could not send indication\n",
                    0,0,0);
    }
  }
  else
  {
    LOC_MSG_ERROR("locGeofenceHandleBreachInd: Invalid IPC message\n", 0,0,0);
  }
  return;
}


/** locGeofenceHandleSetEngineConfigInd
 *  Handle the set engine configuration indication sent by GFC
 *  QMI module. This will be invoked when: a) loc_geofence
 *  module had sent a set engine configuration request
 *  previously.
 *  @param [in] pIpcMsg
 * */

void locGeofenceHandleSetEngineConfigInd(const os_IpcMsgType* const pIpcMsg)
{
  if((NULL != pIpcMsg) &&
     (pIpcMsg->q_Size == sizeof(qmiLocSetGeofenceEngineConfigIndMsgT_v02)))
  {
    locQmiShimIndInfoStructT shimIndInfo = {0};
    qmiLocSetGeofenceEngineConfigIndMsgT_v02 geofenceSetEngineConfigInd = {0};

    shimIndInfo.q_MsgLen = memscpy(&geofenceSetEngineConfigInd, sizeof(geofenceSetEngineConfigInd),
		pIpcMsg->p_Data, pIpcMsg->q_Size);

    /** send indication to the SHIM */
    shimIndInfo.p_Msg = &geofenceSetEngineConfigInd;
    shimIndInfo.q_Id = QMI_LOC_SET_GEOFENCE_ENGINE_CONFIG_IND_V02;
    (void)sendIndicationByTxnId(g_setEngineConfigReqClientList,
                                geofenceSetEngineConfigInd.transactionId,
                                &shimIndInfo);
  }
  else
  {
    LOC_MSG_ERROR("locGeofenceHandleAddInd: Invalid IPC message\n", 0,0,0);
  }

  return;
}

/** locGeofenceInit
 * initialize loc MW goefence module and register the callbacks with the gfc_qmi
 * module.
 * @return : TRUE ; loc geofence module was initialized correctly
 *         : FALSE; loc geofence module could not be initialized.
 */
boolean locGeofenceInit(void)
{
  gfc_qmi_cb_rsp_s_type locGeofenceCallbacks = {
    locGeofenceAddRespCb,
    locGeofenceDeleteRespCb,
    locGeofencePurgeRespCb,
    locGeofenceBreachRespCb,
    locGeofenceGenAlertRespCb,
    NULL,
    locGeofenceSetEngineConfigRespCb,
  };
  /** reset the global variable state */
  locGeofenceResetState();
  /** register the callbacks with gfc_qmi*/
  return( gfc_qmi_register(&locGeofenceCallbacks));
}

/** locGeofenceAdd
 * process an add geofence request
 * @param [in] locClientHandle : loc MW client handle
 * @param [in] addGeofenceReq :  add geofence request
 * @return : TRUE -- GFC_QMI accepted the operation to add the
 *           geofence.\n
 *           FALSE -- GFC_QMI did not accept the operation to
 *           add the geofence.
 */

boolean locGeofenceAdd(
  int locClientHandle,
  const qmiLocAddCircularGeofenceReqMsgT_v02* const pAddGeofenceReq,
  qmiLocGenRespMsgT_v02 * const pResp)
{
  sm_gm_add_request_type gmAddGeofenceReq = {0};
  uint8 returnCode;
  gm_operation_status_type status;
  boolean v_ReqAllowed = TRUE;
  qmiLocStatusEnumT_v02  e_qmiLocStatus = eQMI_LOC_SUCCESS_V02;

  /** Reject add GF request if Gps is locked **/
  if (pdsm_get_gps_lock_status() != PDSM_GPS_LOCK_NONE)
  {
    e_qmiLocStatus = eQMI_LOC_GNSS_DISABLED_V02;
    v_ReqAllowed = FALSE;
    MSG(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_ERROR,"locGeofenceAdd: GPS Locked ");
  }

  if (FALSE == v_ReqAllowed)
  {
    qmiLocAddCircularGeofenceIndMsgT_v02 z_GeofenceAddInd = { 0 };
    void *pPayload = NULL;

    z_GeofenceAddInd.status = e_qmiLocStatus;
    z_GeofenceAddInd.transactionId_valid = TRUE;
    z_GeofenceAddInd.transactionId = pAddGeofenceReq->transactionId;

    pResp->resp.result = QMI_RESULT_SUCCESS_V01;
    pResp->resp.error =  QMI_ERR_NONE_V01;
	
	/** If txn Id cannot be added send error back   */
    if(TRUE != addTxnIdToClientReq(g_addReqClientList,
                                   locClientHandle,
                                   pAddGeofenceReq->transactionId))
    {
      pResp->resp.result =  QMI_RESULT_FAILURE_V01;
      pResp->resp.error = QMI_ERR_DEVICE_IN_USE_V01;
      return FALSE;
    }

    /* send IND message */
    if (FALSE == locGeofenceGenerateIPCtolocMW(
          LM_MIDDLEWARE_MSG_ID_GEOFENCE_ADD_IND,
          sizeof(z_GeofenceAddInd),
          (void *)&z_GeofenceAddInd))
    {
      MSG(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_ERROR,"locGeofenceAdd: could not send IPC to loc MW\n");
	  (void)clearTxnIdFromClientReq(g_addReqClientList,
                                    locClientHandle,
                                    pAddGeofenceReq->transactionId);
	   return FALSE;								
    }
          
    return TRUE;
  }

  /** convert to GFC_QMI format */
  if( FALSE == convertAddCircularGeofenceReqToGm(
     pAddGeofenceReq, &gmAddGeofenceReq))
  {
    pResp->resp.result =  QMI_RESULT_FAILURE_V01;
    pResp->resp.error = QMI_ERR_INVALID_ARG_V01;
    MSG(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_ERROR,"locGeofenceAdd: Invalid params");
    return FALSE ;
  }

  /** If txn Id cannot be added send error back   */
  if(TRUE != addTxnIdToClientReq(g_addReqClientList,
                                 locClientHandle,
                                 pAddGeofenceReq->transactionId))
  {
    pResp->resp.result =  QMI_RESULT_FAILURE_V01;
    pResp->resp.error = QMI_ERR_DEVICE_IN_USE_V01;
    return FALSE;
  }

  /** call gfc api to add the geofence */
  status = gm_add_geofence(&gmAddGeofenceReq, &returnCode);

  if( TRUE != convertGmStatusToQmiLocResp(status, returnCode, pResp))
  {
    MSG_1(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_ERROR,"loc_gf:Add Geofence error_code %d \n", returnCode);
    (void)clearTxnIdFromClientReq(g_addReqClientList,
                                 locClientHandle,
                                 pAddGeofenceReq->transactionId);
    return FALSE;
  }
  MSG_1(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_HIGH,"loc_gf: Add geofence request sent, GM TID:: %d",gmAddGeofenceReq.q_transaction_id);
  return TRUE;
}

/** locGeofenceDelete
 * process a delete geofence request
 * @param [in] locClientHandle : loc MW client handle
 * @param [in] deleteGeofenceReq :  delete geofence request
 * @param [out] pResp            : QMI LOC response payload
 * @return : TRUE -- GFC_QMI accepted the operation to delete
 *           the geofence.\n
 *           FALSE -- GFC_QMI did not accept the operation to
 *           delete the geofence.
 */
boolean locGeofenceDelete(
  int locClientHandle,
  const qmiLocDeleteGeofenceReqMsgT_v02* const pDeleteGeofenceReq,
  qmiLocGenRespMsgT_v02 * const pResp)
{
  uint8 returnCode;
  gm_operation_status_type status;

  sm_gm_delete_request_type gm_delete_req = {0};

  /** sanity check the input */
  if(NULL == pDeleteGeofenceReq )
  {
    /** invalid parameters */
    pResp->resp.result =  QMI_RESULT_FAILURE_V01;
    pResp->resp.error = QMI_ERR_INVALID_ARG_V01;
    MSG(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_ERROR,"locGeofenceDelete: Invalid params\n");
    return FALSE;
  }

  gm_delete_req.q_transaction_id = gfc_qmi_getnext_gm_trxn_id(pDeleteGeofenceReq->transactionId); 
  if((GFC_QMI_INVALID_TRXN_ID == gm_delete_req.q_transaction_id)||
	 (FALSE == gfc_qmi_get_gm_client_handle(&gm_delete_req.client_handle)))
  {
    pResp->resp.result =  QMI_RESULT_FAILURE_V01;
    pResp->resp.error = QMI_ERR_DEVICE_IN_USE_V01;
	MSG_1(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_ERROR,"locGeofenceDelete: Invalid tid %d or GM client handle\n", 
		  gm_delete_req.q_transaction_id);
    return FALSE;
  }


  /** If txn Id cannot be added send error back   */
  if( TRUE != addTxnIdToClientReq( g_deleteReqClientList,
                                   locClientHandle,
                                   pDeleteGeofenceReq->transactionId))
  {
    pResp->resp.result =  QMI_RESULT_FAILURE_V01;
    pResp->resp.error = QMI_ERR_DEVICE_IN_USE_V01;
    return FALSE;
  }

  gm_delete_req.geofence_id = pDeleteGeofenceReq->geofenceId;


  /** call gfc api to delete the geofence */
  status = gm_delete_geofence(&gm_delete_req, &returnCode);

  /** convert delete status to response and fill in the
   *  response code for QMI_LOC Gen resp */
  if(TRUE != convertGmStatusToQmiLocResp(status, returnCode, pResp))
  {
    MSG_1(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_ERROR,"loc_gf:Delete Geofence error_code %d \n", returnCode);
    (void)clearTxnIdFromClientReq(g_deleteReqClientList,
                                  locClientHandle,
                                  pDeleteGeofenceReq->transactionId);
    return FALSE;
  }
  MSG_2(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_HIGH,"loc_gf: Del Geofence %d request sent, GM TID:: %d",
	                                          gm_delete_req.geofence_id,
	                                          gm_delete_req.q_transaction_id);

  return TRUE;
}

/** locGeofenceSetEngineConfig
 * process a Set Geofence Engine config request.
 * @param [in] locClientHandle : loc MW client handle
 * @param [in] pQueryGeofenceReq :  Query geofence request ptr
 * @param [out] pResp            : QMI LOC response payload
 * @return : TRUE -- GFC_QMI accepted the operation to query
 *           the geofence.\n
 *           FALSE -- GFC_QMI did not accept the operation to
 *           query the geofence.
 */
boolean locGeofenceSetEngineConfig(
  int locClientHandle,
  const qmiLocSetGeofenceEngineConfigReqMsgT_v02* const pSetGeofenceEngineConfigReq,
  qmiLocGenRespMsgT_v02 * const pResp)
{
  gfc_qmi_operation_result_e_type returnCode;
  /* The return value from this api */
  boolean retVal = FALSE;
  uint8 u_CntTlv = 0;
  gfc_qmi_operation_status_e_type status;
  uint8 u_error_code;

  sm_gm_set_geofence_engine_config_request_type z_gm_set_geofence_engine_config_request = {0};
  sm_gm_set_geofence_client_config_request_type z_gm_set_client_config_request = {0};


  u_CntTlv += ((pSetGeofenceEngineConfigReq->enableCpiUsage_valid)?1:0);
  u_CntTlv += ((pSetGeofenceEngineConfigReq->enableMotionDetectionSources_valid)?1:0);
  u_CntTlv += ((pSetGeofenceEngineConfigReq->gnssPositionSessionTimeout_valid)?1:0);
  u_CntTlv += ((pSetGeofenceEngineConfigReq->gnssUnavailableIndicationTimeout_valid)?1:0);
  u_CntTlv += ((pSetGeofenceEngineConfigReq->maxGeofences_valid)?1:0);
  do
  {
    if ( u_CntTlv > 1 )
    {
      /** invalid parameters */
      pResp->resp.result =  QMI_RESULT_FAILURE_V01;
      pResp->resp.error = QMI_ERR_NOT_SUPPORTED_V01;
      LOC_MSG_ERROR(" locGeofenceSetEngineConfig : "
                     "More than one configuration specified",0,0,0);
      retVal = FALSE;
      break;
    }

    if(FALSE == gfc_qmi_get_gm_client_handle(&z_gm_set_geofence_engine_config_request.client_handle))
    {
      pResp->resp.result =  QMI_RESULT_FAILURE_V01;
      pResp->resp.error = QMI_ERR_NOT_SUPPORTED_V01;
      retVal = FALSE;
      break;
    }


    /** convert the request to GM format from QMI_LOC */
    if(FALSE == convertSetGeofenceEngineConfigReqToGm(pSetGeofenceEngineConfigReq, 
		                                              &z_gm_set_geofence_engine_config_request,
													  &z_gm_set_client_config_request))
    {
      /** invalid parameters */
      pResp->resp.result =  QMI_RESULT_FAILURE_V01;
      pResp->resp.error = QMI_ERR_INVALID_ARG_V01;
      MSG(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_ERROR,"locGeofenceSetEngineConfig: Invalid params\n");
      retVal = FALSE;
      break;
    }
  
    if((0 == z_gm_set_geofence_engine_config_request.q_mask)&&
	   (0 == z_gm_set_client_config_request.config_request.q_mask))
    {
      MSG(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_ERROR,"locGeofenceSetEngineConfig: No config to set\n");
	  pResp->resp.result =  QMI_RESULT_FAILURE_V01;
      pResp->resp.error = QMI_ERR_INVALID_ARG_V01;
      retVal = FALSE;
      break;
    }

    /** If txn Id cannot be added send error back   */
    if (TRUE != addTxnIdToClientReq(g_setEngineConfigReqClientList,
                                  locClientHandle,
                                  pSetGeofenceEngineConfigReq->transactionId))
    {
      pResp->resp.result =  QMI_RESULT_FAILURE_V01;
      pResp->resp.error = QMI_ERR_DEVICE_IN_USE_V01;
      retVal = FALSE;
      break;
    }


    /* Set GM engine params first */
    if(0 != z_gm_set_geofence_engine_config_request.q_mask)
    {
      if(gm_set_geofence_engine_config(&z_gm_set_geofence_engine_config_request, &u_error_code) == FAILED)
      {
        MSG_1(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_ERROR,"loc_gf::Set geofence engine config request failed error_code %d", u_error_code);
        retVal = FALSE;
	    /**clear the entry from client req list */
        (void)clearTxnIdFromClientReq(g_setEngineConfigReqClientList,
                                    locClientHandle,
                                    pSetGeofenceEngineConfigReq->transactionId);
        break;
      }
    }
  
	
    /* Set GM client config next */
	gfc_qmi_get_gm_client_handle(&z_gm_set_client_config_request.config_request.client_handle);

    if(0 != z_gm_set_client_config_request.config_request.q_mask)
    {
      if(gm_set_geofence_client_config(&z_gm_set_client_config_request, &u_error_code) == FAILED)
      {
        MSG_1(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_ERROR,"loc_gf::Set geofence client config request failed error_code %d", u_error_code);
        retVal = FALSE;
	    /**clear the entry from client req list */
        (void)clearTxnIdFromClientReq(g_setEngineConfigReqClientList,
                                      locClientHandle,
                                      pSetGeofenceEngineConfigReq->transactionId);
	    break;
      }
    }
    retVal = TRUE;
  }while(0);

  return retVal;

}


/** locGeofencePurge
 * Purge all the geofences from the system. This
 * function is called when the geofence client deregisters
 * or the service deInitializes for any reasons
 * The assumption is that only one geofence client is ever
 * present in the system.
 *
 */

void locGeofencePurge(void)
{
  uint8 returnCode;
  gm_operation_status_type status;

  sm_gm_purge_request_type z_gm_purge_request = {0};

  z_gm_purge_request.q_transaction_id = gfc_qmi_getnext_gm_trxn_id(0xFF);

  if((GFC_QMI_INVALID_TRXN_ID == z_gm_purge_request.q_transaction_id)||
	 (FALSE == gfc_qmi_get_gm_client_handle(&z_gm_purge_request.client_handle)))
  {
	MSG(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_ERROR,"loc_gf:Purge invalid TID or GM handle");
    return;
  }

  status = gm_purge_geofence(&z_gm_purge_request, &returnCode);

  if( GEOFENCE_ERROR_NONE != status)
  {
    /** Log an error, nothing else can be done */
    MSG_1(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_ERROR,"locGeofencePurge: failed return code = %d\n",
                                                 returnCode);
  }

  MSG_1(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_HIGH,"loc_gf: Purge geofence request sent, GM TID:: %d",z_gm_purge_request.q_transaction_id);
  /** reset the global variable state */
  locGeofenceResetState();

  return;
}

/** locGeofenceSetOperationMode
 *  Handle the operation mode request sent by control point
 *
 *  @param [in] operation mode
 * */

void locGeofenceSetOperationMode(void)
{
  pdsm_lite_pd_session_operation_e_type pd_operation_mode = PDSM_LITE_SESSION_OPERATION_MIN;

  sm_gm_set_geofence_engine_config_request_type z_gm_request = {0};
    uint8 e_return_code;

  pd_operation_mode = PDSM_LITE_SESSION_OPERATION_STANDALONE_ONLY ;
    

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED,"Setting operation mode %d to GM",
            pd_operation_mode,0,0);
  z_gm_request.q_mask |= SM_GM_ENGINE_CONFIG_POS_OPERATION_MODE_MASK;
  z_gm_request.q_gm_pos_operation_mode = pd_operation_mode;


  /* Get the transaction ID for geofence client config  operation */
  if ((z_gm_request.q_transaction_id = gfc_qmi_getnext_gm_trxn_id(LOC_GEOFENCE_NON_QMI_TRXN_ID))
      == GFC_QMI_INVALID_TRXN_ID)
  {
    /* The mandatory parametrs are not specified , report error. */
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "locGeofenceSetOperationMode Invalid transaction ID \n",
          0, 0, 0);
    return;
  }
  gfc_qmi_get_gm_client_handle(&z_gm_request.client_handle);

  gm_set_geofence_engine_config(&z_gm_request, &e_return_code);

}
