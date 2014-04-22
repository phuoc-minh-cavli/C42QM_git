
/*============================================================================
  @file:       loc_qmi_shim.c

  OVERVIEW:    This module is adapts the loc-api to QCSI framework. This
               file glues loc-api enabling it to be a provider of the 
               QMI-Loc Service. It has a few sections, any changes to the 
               file should be done in the appropriate sections to maintain 
               readability of the file. 

               UTILITY FUNCTIONS
                 All utility functions that are needed shall go here
                   
               MESSAGE CONVERTORS QMI --> Loc-Api
                 All functions that are used to convert QMI to Loc-Api data
                 structures shall be added to this section.
                   
               MESSAGE CONVERTORS Loc-Api --> QMI
                 All functions that are used to convert Loc-Api to QMI data
                 structures shall be added to this section.

               LOC-API CALLBACKS
                 All functions that are registered with the Loc-API shall be 
                 defined in this section
 
               MESSAGE HANDLERS 
                 All the messages handlers should be added to this section. 
                
               QCSI CALLBACKS
                 All functions that are registered with QCSI framework shall be
                 defined in this section
                
               EXTERNALIZED Functions
                 All functions that are exposed through the header file shall be
                 implemented here.
  
                
  DEPENDENCIES: QCSI Framework
 
                Copyright (c) 2011-2012, 2013 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Copyright (c) 2013-2015 QUALCOMM Atheros, Inc.
                All Rights Reserved. 
                Qualcomm Confidential and Proprietary
                Copyright (c) 2015-2020 Qualcomm Technologies, Inc. 
                All Rights Reserved.
                Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/loc_mw/src/loc_qmi_shim.c#1 $
  $DateTime: 2020/07/21 21:23:44 $
  $Author: pwbldsvc $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  04/03/20    ska  Added support for System Info
  08/21/19    ska  Added support for NMEA GSV
  04/17/19    nath Added debug NMEA related code.
  04/16/19    nath Added ERR_FATAL in case of decoding failure in pos QMI send ind
  04/06/18     sum Added code to clear in_progress flag in LocMw context
  09/14/17     sum Added support for GPDTM NMEAs
  02/17/17     skm BLE and Barometer support for LPPE Phase 2
  06/08/16     yh  Lppe
  05/19/16     yh  LocMW refactoring
  03/16/16     yh  XTRA PCID
  02/29/16     muk Add support to output numClockResets
  02/26/16     yh  Add 3 timeSrc enum parsing
  01/21/16     yh  LocWifi cleanup
  10/06/15     yh  QZSS
  04/06/15     sj  LB 2.0 integration
  05/28/15     jv  Disabled QMI_LOC_[SET/GET]_SBAS_CONFIG_REQ_V02
  05/21/15     jv  Support for SBAS EPH & ALM Deletion
  11/07/14   ssu   GM LOWI Integration
  02/19/15     sj  Added reqId to status indications sent back by DBT
  11/06/14     ah  Fixed QMI_LOC_GET_LOW_POWER_MODE Indication failure
  04/15/14     yh  XTRA version check added
  03/10/14     ah  Added support for Set Premium Services Cfg & Get Avail WWAN Pos
  01/24/14     ah  Added GLO support for Get/Set NMEA Type Requests in QMI-LOC
  02/25/13    ssu  Adding a new WWAN type used for AGNSS Emergency connections.
  01/28/12      mj   Update sm_RegisterApiNotifier to include client thread id
  2011-03-08  spn  Initial revision.

============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "gps_variation.h"
#include "customer.h"
#include "comdef.h"

/* Module Header include */
#include "loc_qmi_shim.h"

/* Non-GPS Includes */
#include "qmi_csi_target_ext.h"
#include "qmi_csi.h"
#include "qmi_sap.h"
#include "qmi_idl_lib.h"
#include "mqcsi_log.h"
#include "msg.h"
#include "queue.h"
#include "qmi_si.h"
#include "location_service_iot_v02.h"
#include "location_service_impl_v02.h"
#include "common_v01.h"

/* GPS Includes */
#include "aries_os_api.h"
#include "loc_api_2.h"
#include "loc_client.h"
#include "loc_api_internal.h"
#include "gnss_calendar_util.h"
#include "loc_geofence.h"
#include "cgps_api.h"
#include "gnss_cqsvc.h"
#include "loc_batching.h"
#include "loc_aon.h"
#include "loc_gtp.h"
#include "loc_sm_interface.h"
#include "loc_nv_efs.h"
#include "loc_utils.h"
#include"loc_for_gtp.h"
/* PDAPI - Include PDAPI to convert newly added APIs to PDAPI directly */
#include "pdapi_lite.h"
#ifdef __GRIFFON__
#include "mgp_api.h"
#endif /* __GRIFFON__ */
#include "gnss_common.h"
#include "loc_batching.h"
#include "loc_constrained_tunc_client.h"

/*****************************************************************************
 * Preprocessor Definitions and Constants
 * **************************************************************************/

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

/* -----------------------------------------------------------------------*//**
@brief
Signal that is used by the QMI Framework to notify the task in case there 
are pending messages for the task to handle. 
*/ /* ------------------------------------------------------------------------*/
#define LOC_QMI_SHIM_SIGNAL             C_OS_FLAG_QMI_EVENT

/* -----------------------------------------------------------------------*//**
@brief
Macro definition to set the value of a simple data type and its validity
to TRUE. Simple macro-utility to update the QMI-Loc data structures for 
simple data-types.

@param[in]  variable    the variable which needs to be set
@param[in]  value       the value that it needs to be set to
*/ /* ------------------------------------------------------------------------*/
#define LOC_QMI_SHIM_VARIABLE_VALID(variable) variable ## _valid
#define LOC_QMI_SHIM_SET_VALUE(variable, value)                               \
           LOC_QMI_SHIM_VARIABLE_VALID(variable) = TRUE;                      \
           (variable)= (value)

/*****************************************************************************
 * Type Declarations
 * *************************************************************************/

/* -----------------------------------------------------------------------*//**
@brief 
The information required for handling messages is same as that of indication.
Hence reusing the same structure. 
*/ /* ------------------------------------------------------------------------*/

/* Variables to Cache the position report separately for pass and failed QMI Ind*/
qmiLocEventPositionReportIndMsgT_v02 z_PassCachedQmiPosInfo;
qmiLocEventPositionReportIndMsgT_v02 z_FailCachedQmiPosInfo;

/*****************************************************************************
 * Global Data Definitions
 * *************************************************************************/

/*****************************************************************************
 * Static Variable Definitions
 * *************************************************************************/

/* -----------------------------------------------------------------------*//**
@brief
A Single instance of the module context information. This instance is also
provided as a service cookie to the QCSI framework. 

This information SHALL NOT BE EXPOSED OUT OF THIS FILE  
*/ /* ------------------------------------------------------------------------*/
 locQmiShimInfoStructT z_LocQmiShim;

/* -----------------------------------------------------------------------*//**
@brief
 QMI-Loc Supported Feature Table
 1. The table sequence must follow enum sequence,
 enum 0 at table{0], enum 1 at table[1], etc
 l. QmiLocFeature is only for tracking feature enum purpose.
 2. Never remove any record from this table, if feature is not supported, set 
 to FALSE
 3. This table must updated when location_service_v02.h ->
    qmiLocSupportedFeatureEnumT_v02 is updated
 4. eQMI_LOC_SUPPORTED_FEATURE max value shall not exceed 799

 This is a singleton, there is no need to define another instance of this
*/ /* ------------------------------------------------------------------------*/
static const struct {
  qmiLocSupportedFeatureEnumT_v02     e_QmiLocFeature;
  boolean                             v_Support;
}z_LocQmiShimSupportedFeatureTable[] =
{
  {  eQMI_LOC_SUPPORTED_FEATURE_ODCPI_2_V02,                        TRUE },
  {  eQMI_LOC_SUPPORTED_FEATURE_WIFI_AP_DATA_INJECT_2_V02,          TRUE },
  {  eQMI_LOC_SUPPORTED_FEATURE_DEBUG_NMEA_V02,                     TRUE },
  {  eQMI_LOC_SUPPORTED_FEATURE_GNSS_ONLY_POSITION_REPORT_V02,      FALSE},
  {  eQMI_LOC_SUPPORTED_FEATURE_FDCL_V02,                           TRUE },
  {  eQMI_LOC_SUPPORTED_FEATURE_CONSTELLATION_ENABLEMENT_V02,      FALSE },
  {  eQMI_LOC_SUPPORTED_FEATURE_AGPM_V02,                          FALSE },
  {  eQMI_LOC_SUPPORTED_FEATURE_XTRA_INTEGRITY_V02,                FALSE },
  {  eQMI_LOC_SUPPORTED_FEATURE_FDCL_2_V02,                        TRUE  }
};


/* For Griffon*/
#ifdef __GRIFFON__
#include "qmi_csi_common.h"
#include "upos.h"
extern qmi_csi_xport_ops_type udp_ops;
static qmi_csi_os_params g_z_OsParams;
static UPOS_thread_t upos_thread;
static void read_udp_and_signal()
{
  fd_set fds;

  printf("created thread: in read_udp_and_signal\n");
  /* This loop calls a blocking select to read from the UDP port that was
         provided to QCSI during service registration */
  while (1)
  {
    //Wait for a signal from the mother task to resume
    UPOS_ThreadWait(&upos_thread);
    fds = g_z_OsParams.fds;
    printf("read_udp_and_signal: waiting for fd signal\n");
    select(g_z_OsParams.max_fd + 1, &fds, NULL, NULL, NULL);

    // signal the "mother" thread
    printf("read_udp_and_signal: signaling the main thread\n");
    UPOS_ThreadSignalTask(&upos_thread, C_OS_FLAG_QMI_EVENT);
  }
}
#endif /* __GRIFFON__ */

/* -----------------------------------------------------------------------*//**
@brief
A helper utility provided to the queue library. This function is intended to 
see if the  ShimCltInfo represented by "item_ptr" holds information for a 
QMI-Client represented by "p_compareVal" which is also a ShimCltInfo 
structure.

@param[in]   p_ItemPtr         A potential item of z_LocQmiShim.z_ClientQ
@param[in]   p_CompareVal      Qmi-Client handle that is being considered

@retval    0          Item was not the one we are searching for
@retval    1          Item was the one we are searching for
*/ /* ------------------------------------------------------------------------*/
static int locQmiShimCompareConnectionInfoHandle
(
   void *p_ItemPtr,
   void *p_CompareVal
   )
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* item_ptr is an element of the Client Q i.e. Connection Handle, 
     and compare val is also a Connection Handle */
  return ((p_ItemPtr == p_CompareVal) ? 1 : 0);
}

/* -----------------------------------------------------------------------*//**
@brief
A helper utility provided to the queue library. This function is intended to 
see if the  ShimCltInfo represented by "item_ptr" holds information for a 
QMI-Client represented by "p_compareVal" which is 
ShimCltInfo->p_QmiClientHandle

@param[in]   p_ItemPtr         A potential item of z_LocQmiShim.z_ClientQ
@param[in]   p_CompareVal      Qmi-Client handle that is being considered

@retval    0          Item was not the one we are searching for
@retval    1          Item was the one we are searching for
*/ /* ------------------------------------------------------------------------*/
static int locQmiShimCompareQmiClientHandle
(
   void *p_ItemPtr,
   void *p_CompareVal
   )
{
  locQmiShimCltInfoStructT *pz_CltInfo = (locQmiShimCltInfoStructT *)p_ItemPtr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* item_ptr is an element of the Client Q i.e. Connection Handle, 
     and compare val is also a Connection Handle */
  return ((pz_CltInfo->p_QmiClientHandle == p_CompareVal) ? 1 : 0);
}


/* -----------------------------------------------------------------------*//**
@brief
A helper utility provided to the queue library. This function is intended to 
see if the  ShimCltInfo represented by "item_ptr" holds information for a 
Loc-Api Client Handle represented by "p_compareVal".

@param[in]   p_ItemPtr         A potential item of z_LocQmiShim.z_ClientQ
@param[in]   p_CompareVal      Loc-Api Client handle that is being considered

@retval    0          Item was not the one we are searching for
@retval    1          Item was the one we are searching for
*/ /* ------------------------------------------------------------------------*/
static int locQmiShimCompareLocApiHandle
(
   void *p_ItemPtr,
   void *p_CompareVal
   )
{
  locQmiShimCltInfoStructT *pz_Item = (locQmiShimCltInfoStructT *)p_ItemPtr;
  loc_client_handle_type   z_locHandle;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  z_locHandle = (loc_client_handle_type)p_CompareVal;
  /* item_ptr is an element of the Client Q i.e. Connection Handle, 
     and compare val is also a Connection Handle */
  return ((pz_Item->l_LocClientHandle == z_locHandle) ? 1 : 0);
}


/*============================================================================
 *                    L O C - A P I    C A L L B A C K S 
 * =========================================================================*/

/* -----------------------------------------------------------------------*//**
@brief
Function registered with Loc-Api, which will be called to notify the shim of 
events from loc-api

@param[in]   z_LocHandle           Loc-Client Handle
@param[in]   e_LocEvent            Event identifier
@param[in]   pz_LocEventPayload    Payload of the event

@retval    LOC_API_SUCCESS        Indication handled
@retval    <Anything Else>        Indication could not be handled. Defined in 
                                loc_api.h
                                
*/ /* ------------------------------------------------------------------------*/
static boolean locQmiShimEventCb
(
   loc_client_handle_type           l_LocClientHandle,
   qmiLocEventRegMaskT_v02            e_LocEvent,
   const void *p_LocEventPayload
   )
{
  qmi_csi_error               e_QmiErr;
  locQmiShimCltInfoStructT   *pz_CltInfo = NULL;
  locQmiShimMsgInfoStructT    z_IndInfo = { 0 };
  boolean                     v_SendInd = TRUE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  pz_CltInfo = q_linear_search(&z_LocQmiShim.z_ClientQ,
                               locQmiShimCompareLocApiHandle,
                               (void *)l_LocClientHandle);

  /* If the client was not found */
  if ((NULL == pz_CltInfo) ||
      (NULL == p_LocEventPayload))
  {
    LOC_MSG_ERROR("locQmiShimEventCb Error NULL param: locQmiShimClt = 0x%p for ClientHandle = %d, p_LocEventPayload = 0x%p",
                  pz_CltInfo, l_LocClientHandle, p_LocEventPayload);
    return FALSE;
  }

  /* Message assignment is common */
  z_IndInfo.p_Msg = (void *)p_LocEventPayload;
  switch (e_LocEvent)
  {
    case QMI_LOC_EVENT_MASK_POSITION_REPORT_V02:
      {
        qmiLocEventPositionReportIndMsgT_v02 *pz_PositionReportIndMsg = (qmiLocEventPositionReportIndMsgT_v02 *)p_LocEventPayload;
        z_IndInfo.q_Id = QMI_LOC_EVENT_POSITION_REPORT_IND_V02;
        z_IndInfo.q_MsgLen = sizeof(qmiLocEventPositionReportIndMsgT_v02);

        if ((NULL != pz_PositionReportIndMsg) &&
            (eQMI_LOC_SESS_STATUS_SUCCESS_V02 == pz_PositionReportIndMsg->sessionStatus))
        {
          uint64 t_positionReportTime = cgps_TimeTickGetMsec();
          gnss_UpdateQmiPosRptTime(pz_PositionReportIndMsg->gpsTime.gpsWeek,
                                   pz_PositionReportIndMsg->gpsTime.gpsTimeOfWeekMs,
                                   t_positionReportTime);
          LOC_MSG_ERROR("QMI_LOC_EVENT_MASK_POSITION_REPORT_V02 gpsWeek=%u, gpsTimeOfWeekMs=%u,position report time=%u,",
                      pz_PositionReportIndMsg->gpsTime.gpsWeek,
                      pz_PositionReportIndMsg->gpsTime.gpsTimeOfWeekMs,
                      t_positionReportTime);

        }
      }
      break;
	case QMI_LOC_EVENT_MASK_GNSS_SV_INFO_V02:
		{  
		   	z_IndInfo.q_Id = QMI_LOC_EVENT_GNSS_SV_INFO_IND_V02;
            z_IndInfo.q_MsgLen = sizeof(qmiLocEventGnssSvInfoIndMsgT_v02);
			
		}
	    break;

    case QMI_LOC_EVENT_MASK_NMEA_V02:
      {
        z_IndInfo.q_Id = QMI_LOC_EVENT_NMEA_IND_V02;
        z_IndInfo.q_MsgLen = sizeof(qmiLocEventNmeaIndMsgT_v02);
      }
      break;
     /*9x05: LOC_EVENT_NI_NOTIFY_VERIFY_REQUEST  not supported*/


    case QMI_LOC_EVENT_MASK_INJECT_TIME_REQ_V02:
      {
        z_IndInfo.q_Id = QMI_LOC_EVENT_INJECT_TIME_REQ_IND_V02;
        z_IndInfo.q_MsgLen = sizeof(qmiLocEventInjectTimeReqIndMsgT_v02);
      }
      break;

    case QMI_LOC_EVENT_MASK_INJECT_PREDICTED_ORBITS_REQ_V02:
      {
        z_IndInfo.q_Id = QMI_LOC_EVENT_INJECT_PREDICTED_ORBITS_REQ_IND_V02;
        z_IndInfo.q_MsgLen = sizeof(qmiLocEventInjectPredictedOrbitsReqIndMsgT_v02);
      }
      break;
#if 0 //Temp removal needs to be brought back
    case QMI_LOC_EVENT_MASK_ENGINE_STATE_V02:
      {
        z_IndInfo.q_Id = QMI_LOC_EVENT_ENGINE_STATE_IND_V02;
        z_IndInfo.q_MsgLen = sizeof(qmiLocEventEngineStateIndMsgT_v02);
      }
      break;
#endif 
    case QMI_LOC_EVENT_MASK_FIX_SESSION_STATE_V02:
      {
        z_IndInfo.q_Id = QMI_LOC_EVENT_FIX_SESSION_STATE_IND_V02;
        z_IndInfo.q_MsgLen = sizeof(qmiLocEventFixSessionStateIndMsgT_v02);
      }
      break;

     /*9x05: Wifi not supported*/


    default:
      LOC_MSG_ERROR("locQmiShimEventCb Unhandled locEvent 0x%x ", e_LocEvent, 0, 0);
      v_SendInd = FALSE;
      break;
  }

  if (v_SendInd)
  {
    e_QmiErr = mqcsi_send_ind(pz_CltInfo->p_QmiClientHandle,
                              z_IndInfo.q_Id,
                              z_IndInfo.p_Msg,
                              z_IndInfo.q_MsgLen);

    if (QMI_CSI_NO_ERR != e_QmiErr)
    {
      if((e_LocEvent == QMI_LOC_EVENT_MASK_POSITION_REPORT_V02)&&(5 == e_QmiErr))
      {
        /*store the position report in z_FailCachedQmiPosInfo in cases when indication is not successful*/
        memset(&z_FailCachedQmiPosInfo,0,sizeof(z_FailCachedQmiPosInfo));
        z_FailCachedQmiPosInfo = *(qmiLocEventPositionReportIndMsgT_v02 *)p_LocEventPayload;
        ERR_FATAL("Error dut to decoding failure while sending QMI Ind", 0, 0, 0);
      }
      
      /* Could not send an event out, some error in QCSI framework */
      LOC_MSG_ERROR("locQmiShimEventCb: <<< QMI_LOC Msg 0x%04x ind failed for LocClient %d, QMI_CSI_ERR=%d ",
                    z_IndInfo.q_Id, l_LocClientHandle, e_QmiErr);
    }
    else
    {
      LOC_MSG_ERROR("locQmiShimEventCb: <<< QMI_LOC Msg 0x%04x ind succeed for LocClient %d",
                  z_IndInfo.q_Id, l_LocClientHandle, 0);
      
      if(e_LocEvent & QMI_LOC_EVENT_MASK_POSITION_REPORT_V02)
      {
      /*store the position report in z_PassCachedQmiPosInfo in cases when indication is successful*/
        memset(&z_PassCachedQmiPosInfo,0,sizeof(z_PassCachedQmiPosInfo));
        z_PassCachedQmiPosInfo = *(qmiLocEventPositionReportIndMsgT_v02 *)p_LocEventPayload;
    }
    }
  }
  else
  {
    /* Could not send an event out, some error in translation */
    LOC_MSG_ERROR("locQmiShimEventCb: Not Send QMI_LOC_EVENT_IND locEvent 0x%x for LocClient %d",
                  e_LocEvent, l_LocClientHandle, 0);
  }

  return v_SendInd;
}

/*============================================================================
 *                       M E S S A G E    H A N D L E R S 
 * =========================================================================*/
/* -----------------------------------------------------------------------*//**
@brief
Message handler to handle the stop-request from the client

@param[in]  pz_CltInfo            Client for whom this message is intended
@param[in]  p_MsgInfo             Information regarding to the message
@param[out] pe_QmiErr             if Failure, this will be populated to 
                                identify the reason for error

@retval    TRUE                   Message successfully handled
@retval    FALSE                  Message could not be handled successfully
*/ /* ------------------------------------------------------------------------*/
static boolean locQmiShimStopReqHdlr
(
   const locQmiShimCltInfoStructT         *pz_CltInfo,
   const locQmiShimMsgInfoStructT         *pz_MsgInfo,
   qmi_error_type_v01                     *pe_QmiErr
   )
{
  boolean                     v_RetVal = TRUE;
  int32                       i_LocApiErr = LOC_API_SUCCESS;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Not doing check of inputs as this is called from within the module,
     and this will be called with proper parameters */

  /* Forward call to loc-api */
  i_LocApiErr = loc_stop_fix(pz_CltInfo->l_LocClientHandle);

  if (LOC_API_SUCCESS != i_LocApiErr)
  {
    v_RetVal = FALSE;

    switch (i_LocApiErr)
    {
      case LOC_API_INVALID_HANDLE:
        (*pe_QmiErr) = QMI_ERR_INVALID_HANDLE_V01;
        break;

        /* For all other values relay INTERNAL error */
      default:
        (*pe_QmiErr) = QMI_ERR_INTERNAL_V01;
        break;
    }
  }
  return v_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
Message handler to store and handle the client revision that is given 
by the application

@param[out]  pz_CltInfo            Client for whom this message is intended
@param[in]   p_MsgInfo             Information regarding to the message
@param[out]  pe_QmiErr             if Failure, this will be populated to 
                                 identify the reason for error

@retval    TRUE                   Message successfully handled
@retval    FALSE                  Message could not be handled successfully
*/ /* ------------------------------------------------------------------------*/
static boolean locQmiShimHandleClientRevision
(
   locQmiShimCltInfoStructT               *pz_CltInfo,
   const locQmiShimMsgInfoStructT         *pz_MsgInfo,
   qmi_error_type_v01                     *pe_QmiErr
   )
{
  boolean                     v_RetVal = TRUE;
  const qmiLocInformClientRevisionReqMsgT_v02 *pz_Clt = pz_MsgInfo->p_Msg;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Not doing check of inputs as this is called from within the module,
     and this will be called with proper parameters */

  /* Store the revision of the client revision  */
  pz_CltInfo->q_CltRev = pz_Clt->revision;

  return v_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
Message handler for the get service revision req_v02 API from the 
application. 

@param[out]  pz_CltInfo            Client for whom this message is intended
@param[in]   p_MsgInfo             Information regarding to the message
@param[out]  pe_QmiErr             if Failure, this will be populated to 
                                 identify the reason for error

@retval    TRUE                   Message successfully handled
@retval    FALSE                  Message could not be handled successfully
*/ /* ------------------------------------------------------------------------*/
static boolean locQmiShimHandleGetServRevisionReq_v02
(
   locQmiShimCltInfoStructT               *pz_CltInfo,
   const locQmiShimMsgInfoStructT         *pz_MsgInfo,
   qmi_error_type_v01                     *pe_QmiErr
   )
{
  qmiLocGetServiceRevisionIndMsgT_v02 *pz_GetSvcRevInd = NULL;
  boolean     v_RetVal = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((NULL == pz_CltInfo) || (NULL == pz_MsgInfo) ||
      (NULL == pe_QmiErr))
  {
    /* These NULL condition shall never happen */
    LOC_MSG_ERROR("locQmiShimHandleGetServRevisionReq_v02 pz_CltInfo=%p, pz_MsgInfo = %p, pe_QmiErr=%p",
                  pz_CltInfo, pz_MsgInfo, pe_QmiErr);
    if (NULL != pe_QmiErr)
    {
      (*pe_QmiErr) = QMI_ERR_MALFORMED_MSG_V01;
    }
    return FALSE;
  }

  pz_GetSvcRevInd = (qmiLocGetServiceRevisionIndMsgT_v02 *)loc_calloc(sizeof(*pz_GetSvcRevInd));
  if (NULL == pz_GetSvcRevInd)
  {
    LOC_MSG_ERROR("locQmiShimHandleGetServRevisionReq_v02: calloc return NULL", 0, 0, 0);
    return v_RetVal;
  }

  pz_GetSvcRevInd->status   = eQMI_LOC_SUCCESS_V02;
  pz_GetSvcRevInd->revision = LOC_IOT_V02_IDL_MINOR_VERS;

#ifdef __GRIFFON__
  p_GetSvcRevInd->gnssMeFWVerString_valid   =
     mgp_GetGNSSSWVersion(
     C_MGP_VER_STR_ME,
     p_GetSvcRevInd->gnssMeFWVerString,
     sizeof(p_GetSvcRevInd->gnssMeFWVerString));

  p_GetSvcRevInd->gnssHostSWVerString_valid =
     mgp_GetGNSSSWVersion(
     C_MGP_VER_STR_HOST,
     p_GetSvcRevInd->gnssHostSWVerString,
     sizeof(p_GetSvcRevInd->gnssHostSWVerString));

  p_GetSvcRevInd->gnssSWVerString_valid     =
     mgp_GetGNSSSWVersion(
     C_MGP_VER_STR_FULL,
     p_GetSvcRevInd->gnssSWVerString,
     sizeof(p_GetSvcRevInd->gnssSWVerString));
#else
  pz_GetSvcRevInd->gnssMeFWVerString_valid   = FALSE;
  pz_GetSvcRevInd->gnssHostSWVerString_valid = FALSE;
  pz_GetSvcRevInd->gnssSWVerString_valid     = FALSE;
#endif /* __GRIFFON__ */

  v_RetVal = locMW_SendQmiIndicationProxy(pz_CltInfo->l_LocClientHandle,
                                          QMI_LOC_GET_SERVICE_REVISION_IND_V02,
                                          pz_GetSvcRevInd, LOC_API_SUCCESS);

  return v_RetVal;

}


/* -----------------------------------------------------------------------*//**
@brief
Message handler to handle the Get Supported Feature QMI_LOC message .
Ind message sent back directly from this handler function.

@param[in|out]  pz_CltInfo        Client for whom this message is intended
@param[in]      p_MsgInfo         Information regarding to the message
@param[out]     pe_QmiErr         if Failure, this will be populated to 
                                identify the reason for error

@retval    TRUE                   Message successfully handled. It means that 
                                Ind message will be sent back to the client.
@retval    FALSE                  Message could not be handled successfully
*/ /* ------------------------------------------------------------------------*/
static boolean locQmiShimHandleGetSupportedFeatureReq
(
   locQmiShimCltInfoStructT        *pz_CltInfo,
   const locQmiShimMsgInfoStructT  *pz_MsgInfo,
   qmi_error_type_v01                     *pe_QmiErr
   )
{
  qmiLocGetSupportedFeatureIndMsgT_v02   *pz_GetSupportedFeatureInd = NULL;
  uint32                                  q_Count = 0;
  uint32                                  q_NumFeature = 0;
  uint8                                   u_Index = 0;
  uint8                                   u_FeatureBit = 0;
  boolean v_RetVal = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((NULL == pz_CltInfo) || (NULL == pz_MsgInfo) ||
      (NULL == pe_QmiErr))
  {
    /* These NULL condition shall never happen */
    LOC_MSG_ERROR("locQmiShimHandleGetServRevisionReq_v02 pz_CltInfo=%p, pz_MsgInfo = %p, pe_QmiErr=%p",
                  pz_CltInfo, pz_MsgInfo, pe_QmiErr);
    if (NULL != pe_QmiErr)
    {
      (*pe_QmiErr) = QMI_ERR_MALFORMED_MSG_V01;
    }
    return FALSE;
  }

  pz_GetSupportedFeatureInd = (qmiLocGetSupportedFeatureIndMsgT_v02 *)loc_calloc(sizeof(*pz_GetSupportedFeatureInd));
  if (NULL == pz_GetSupportedFeatureInd)
  {
    LOC_MSG_ERROR("locQmiShimHandleGetSupportedFeatureReq: calloc return NULL", 0, 0, 0);
    return v_RetVal;
  }

  pz_GetSupportedFeatureInd->status = eQMI_LOC_SUCCESS_V02;
  q_NumFeature = sizeof(z_LocQmiShimSupportedFeatureTable) / sizeof(z_LocQmiShimSupportedFeatureTable[0]);
  if (QMI_LOC_SUPPORTED_FEATURE_LENGTH_V02 * 8 < q_NumFeature)
  {
    LOC_MSG_ERROR("locQmiShimHandleGetSupportedFeatureReq, check feature size %d", q_NumFeature, 0, 0);
    q_NumFeature = QMI_LOC_SUPPORTED_FEATURE_LENGTH_V02 * 8;
  }
  pz_GetSupportedFeatureInd->feature_len = q_NumFeature / 8 + 1;  //feature bit is set in unit8
  for (q_Count = 0; q_Count < q_NumFeature; q_Count++)
  {
    u_Index = q_Count / 8;
    u_FeatureBit = q_Count % 8;
    if (TRUE == z_LocQmiShimSupportedFeatureTable[q_Count].v_Support)
    {
      pz_GetSupportedFeatureInd->feature[u_Index] |= (1 << u_FeatureBit);
    }
  }

    /* Queue up the indication that needs to be sent */
    v_RetVal = locMW_SendQmiIndicationProxy(pz_CltInfo->l_LocClientHandle,
                                          QMI_LOC_GET_SUPPORTED_FEATURE_IND_V02,
                                          pz_GetSupportedFeatureInd, LOC_API_SUCCESS);

  return v_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
 Message handler to handle QMI_LOC_GET_REGISTERED_EVENTS_REQ message .
 
@param[in|out]  pz_CltInfo        Client for whom this message is intended
@param[in]      p_MsgInfo         Information regarding to the message
@param[out]     pe_QmiErr         if Failure, this will be populated to 
                                 identify the reason for error

@retval    TRUE                   Message successfully handled. It means that 
                                 Ind message will be sent back to the client.
@retval    FALSE                  Message could not be handled successfully
*/ /* ------------------------------------------------------------------------*/
static boolean locQmiShimHandleGetRegisteredEventsReq
(
   locQmiShimCltInfoStructT               *pz_CltInfo,
   const locQmiShimMsgInfoStructT         *pz_MsgInfo,
   qmi_error_type_v01                     *pe_QmiErr
   )
{
  qmiLocGetRegisteredEventsIndMsgT_v02 *pz_GetRegMaskInd = NULL;
  boolean                                 v_RetVal = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((NULL == pz_CltInfo) || (NULL == pz_MsgInfo) ||
      (NULL == pe_QmiErr))
  {
    /* These NULL condition shall never happen */
    LOC_MSG_ERROR("locQmiShimHandleGetRegisteredEventsReqpz_CltInfo=%p, pz_MsgInfo = %p, pe_QmiErr=%p",
                  pz_CltInfo, pz_MsgInfo, pe_QmiErr);
    return v_RetVal;
  }

  pz_GetRegMaskInd = (qmiLocGetRegisteredEventsIndMsgT_v02 *)loc_calloc(sizeof(*pz_GetRegMaskInd));
  if (NULL == pz_GetRegMaskInd)
  {
    LOC_MSG_ERROR("locQmiShimHandleGetRegisteredEventsReqcalloc return NULL", 0, 0, 0);
    return v_RetVal;
  }

  LOC_QMI_SHIM_SET_VALUE(pz_GetRegMaskInd->eventRegMask, pz_CltInfo->t_EventMask);

    /* Queue up the indication that needs to be sent */
    v_RetVal = locMW_SendQmiIndicationProxy(pz_CltInfo->l_LocClientHandle,
                                          QMI_LOC_GET_REGISTERED_EVENTS_IND_V02,
                                          pz_GetRegMaskInd, LOC_API_SUCCESS);

  return v_RetVal;
}
#if 0
/* -----------------------------------------------------------------------*//**
@brief
 *  Message handler to handle QMI_LOC_START_LISTEN_POSITION_REQ message 
 *
 * @param pz_ClientInfo LocAPI client handle
 * @param pz_Req        QMI request
 * @param pz_Resp       QMI response
 *
 * @return TRUE if the message has been handled successfully, else
 *         FALSE
*/ /* ------------------------------------------------------------------------*/
static boolean locQmiShimHandleStartListenPositionReq
(
  locQmiShimCltInfoStructT*                pz_ClientInfo,
  qmiLocStartListenPositionReqMsgT_v02*  pz_Req,
  qmiLocGenRespMsgT_v02*                 pz_Resp
)
{
  boolean                                 v_RetVal = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == pz_Resp)
  {
    LOC_MSG_ERROR("locQmiShimHandleStartListenPositionReq: pResp is NULL \n", 0, 0, 0);
    return v_RetVal;
  }

  if ((NULL == pz_ClientInfo) || (NULL == pz_Req))
  {
    /** invalid parameters */
    pz_Resp->resp.result = QMI_RESULT_FAILURE_V01;
    pz_Resp->resp.error = QMI_ERR_INVALID_ARG_V01;
    LOC_MSG_ERROR("locQmiShimHandleStartListenPositionReq: Invalid params\n", 0, 0, 0);
  return v_RetVal;
}
#if 0
  pz_ClientInfo->v_PositionListener = TRUE;
#endif
  v_RetVal = TRUE;

  return v_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
 *  Message handler to handle QMI_LOC_STOP_LISTEN_POSITION_REQ message 
 *
 * @param pz_ClientInfo LocAPI client handle
 * @param pz_Req        QMI request
 * @param pz_Resp       QMI response
 *
 * @return TRUE if the message has been handled successfully, else
 *         FALSE
*/ /* ------------------------------------------------------------------------*/
static boolean locQmiShimHandleStopListenPositionReq
(
  loc_client_info_s_type*                pz_ClientInfo,
  qmiLocStopListenPositionReqMsgT_v02*   pz_Req,
  qmiLocGenRespMsgT_v02*                 pz_Resp
)
{
  boolean                                 v_RetVal = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == pz_Resp)
  {
    LOC_MSG_ERROR("locQmiShimHandleStopListenPositionReq: pResp is NULL \n", 0, 0, 0);
    return v_RetVal;
  }

  if ((NULL == pz_ClientInfo) || (NULL == pz_Req))
  {
    /** invalid parameters */
    pz_Resp->resp.result = QMI_RESULT_FAILURE_V01;
    pz_Resp->resp.error = QMI_ERR_INVALID_ARG_V01;
    LOC_MSG_ERROR("locQmiShimHandleStopListenPositionReq: Invalid params\n", 0, 0, 0);
  return v_RetVal;
}
#if 0
  pz_ClientInfo->v_PositionListener = FALSE;
#endif
  v_RetVal = TRUE;

  return v_RetVal;
}
#endif
/* -----------------------------------------------------------------------*//**
@brief
Message handler to handle all new APIs added after Loc-Api 1.x. The 
expectation from these APIs is that no translation is required and they 
will handle the QMI-LOC data types directly.

@param[out]  pz_CltInfo            Client for whom this message is intended
@param[in]   p_MsgInfo             Information regarding to the message
@param[out]  pe_QmiErr             if Failure, this will be populated to 
                                 identify the reason for error

@retval    TRUE                   Message successfully handled
@retval    FALSE                  Message could not be handled successfully
*/ /* ------------------------------------------------------------------------*/
static boolean locQmiShimRouteMessages
(
   locQmiShimCltInfoStructT               *pz_CltInfo,
   const locQmiShimMsgInfoStructT         *pz_MsgInfo,
   qmi_error_type_v01                     *pe_QmiErr
   )
{
  boolean                     v_RetVal = TRUE;
  qmiLocGenRespMsgT_v02       z_QmiResp = { 0 };
  locQmiShimCltInfoStructT *pz_LocClientPtr = NULL;
  
  if (NULL == pz_CltInfo || NULL == pz_MsgInfo || NULL == pe_QmiErr)
  {
    LOC_MSG_ERROR("locQmiShimRouteMessages NULL paramter",
                  0, 0, 0);
    return FALSE;
  }

  pz_LocClientPtr = locClnt_FindHandle(pz_CltInfo->l_LocClientHandle);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Not doing check of inputs as this is called from within the module,
     and this will be called with proper parameters */

  switch (pz_MsgInfo->q_Id)
  {
    /*#9x05: SPI and SLIM not supported*/

#if 0 //#9x05:*Handling of the message is still to be  decided 
    case QMI_LOC_SET_EXTERNAL_POWER_CONFIG_REQ_V02:
      {
        cgps_BatteryChargerStatusEnumType e_ExtChargerStatus;
        qmiLocSetExternalPowerConfigReqMsgT_v02 *p_ConfigMsg;
        qmiLocExternalPowerConfigEnumT_v02 externalPowerState;

        (void)loc_qmiloc_set_external_power_config_req(
           pz_LocClientPtr,
           (qmiLocSetExternalPowerConfigReqMsgT_v02 *)pz_MsgInfo->p_Msg,
           &z_QmiResp);

        /* Set the Charger status in CGPS Api */
        /* Translate the QMI Loc charger status to internal version */
        p_ConfigMsg =
           (qmiLocSetExternalPowerConfigReqMsgT_v02 *)pz_MsgInfo->p_Msg;
        /* Send the configuration to CSM */
        locCsm_HandleSetExternalPowerConfigReq(p_ConfigMsg);
        externalPowerState = p_ConfigMsg->externalPowerState;
        switch (externalPowerState)
        {
          case eQMI_LOC_EXTERNAL_POWER_NOT_CONNECTED_V02:
            e_ExtChargerStatus = CGPS_BATTERY_CHARGER_NOT_CONNECTED;
            break;
          case eQMI_LOC_EXTERNAL_POWER_CONNECTED_V02:
            /* Assume it is connected and charged */
            e_ExtChargerStatus = CGPS_BATTERY_CHARGER_CONNECTED_CHARGED;
            break;

          case eQMI_LOC_EXTERNAL_POWER_UNKNOWN_V02:
          default:
            /* Default to charger state unknown */
            e_ExtChargerStatus = CGPS_BATTERY_CHARGER_STATE_UNKNOWN;
            break;
        }
        cgps_SetExternalChargerStatus(e_ExtChargerStatus);
      }
      break;

    case QMI_LOC_GET_EXTERNAL_POWER_CONFIG_REQ_V02:
      {
        (void)loc_qmiloc_get_external_power_config_req(
           pz_LocClientPtr,
           &z_QmiResp);
      }
      break;
#endif


    case QMI_LOC_ADD_CIRCULAR_GEOFENCE_REQ_V02:
      {
        v_RetVal = locGeofenceAdd(
           pz_CltInfo->l_LocClientHandle,
           (qmiLocAddCircularGeofenceReqMsgT_v02 *)pz_MsgInfo->p_Msg,
           &z_QmiResp);
      }
      break;

    case QMI_LOC_DELETE_GEOFENCE_REQ_V02:
      {
        v_RetVal = locGeofenceDelete(
           pz_CltInfo->l_LocClientHandle,
           (qmiLocDeleteGeofenceReqMsgT_v02 *)pz_MsgInfo->p_Msg,
           &z_QmiResp);
      }
      break;



    case QMI_LOC_SET_GEOFENCE_ENGINE_CONFIG_REQ_V02:
      {
        v_RetVal = locGeofenceSetEngineConfig(
           pz_CltInfo->l_LocClientHandle,
           (qmiLocSetGeofenceEngineConfigReqMsgT_v02 *)pz_MsgInfo->p_Msg,
           &z_QmiResp);
      }
      break;


    case QMI_LOC_INJECT_GTP_CLIENT_DOWNLOADED_DATA_REQ_V02:
      {
        pz_CltInfo->v_IsTdpClient = TRUE;
        (void)loc_gtp_InjectGtpClientDownloadedDataReq(pz_LocClientPtr,
                                                       (qmiLocInjectGtpClientDownloadedDataReqMsgT_v02 *)pz_MsgInfo->p_Msg,
                                                       &z_QmiResp);
      }
      break;

    /*#9x05: GDT not supported  */

    case QMI_LOC_GET_FDCL_BS_LIST_REQ_V02:
      {
        v_RetVal = loc_tle_GetFdclBsListReq(pz_LocClientPtr,
                                            (const qmiLocGetFdclBsListReqMsgT_v02 *)pz_MsgInfo->p_Msg,
                                 &z_QmiResp);
      }
      break;
    case QMI_LOC_INJECT_FDCL_DATA_REQ_V02:
      {
        v_RetVal=loc_tle_InjectFdclDataReq(pz_LocClientPtr,
                                           (const qmiLocInjectFdclDataReqMsgT_v02 *)pz_MsgInfo->p_Msg,
                                    &z_QmiResp);
      }
      break;

    case QMI_LOC_GET_SUPPORTED_FEATURE_REQ_V02:
      {
        v_RetVal = locQmiShimHandleGetSupportedFeatureReq(pz_CltInfo,
                                                          pz_MsgInfo,
                                                          pe_QmiErr);
      }
      break;
#if 0
    case QMI_LOC_START_LISTEN_POSITION_REQ_V02:
      {
        v_RetVal = locQmiShimHandleStartListenPositionReq(pz_LocClientPtr,
                                        (qmiLocStartListenPositionReqMsgT_v02 *)pz_MsgInfo->p_Msg,
                                        &z_QmiResp);
      }
      break;

    case QMI_LOC_STOP_LISTEN_POSITION_REQ_V02:
      {
        v_RetVal = locQmiShimHandleStopListenPositionReq(pz_LocClientPtr,
                                        (qmiLocStopListenPositionReqMsgT_v02 *)pz_MsgInfo->p_Msg,
                                            &z_QmiResp );
      }
      break;
#endif
     
    case QMI_LOC_GET_BS_OBS_DATA_REQ_V02:
    {
      v_RetVal = loc_tle_Fdcl2_GetBsObsDataReq(pz_LocClientPtr,
                                               (const qmiLocGetBsObsDataReqMsgT_v02 *)pz_MsgInfo->p_Msg,
                                               &z_QmiResp);
    }
    break; 
    
    case QMI_LOC_QUERY_GNSS_ENERGY_CONSUMED_REQ_V02:
    {
      v_RetVal = loc_mw_query_gnss_energy_consumed(pz_LocClientPtr,
                                    (const qmiLocQueryGNSSEnergyConsumedReqMsgT_v02 *)pz_MsgInfo->p_Msg,
                                    &z_QmiResp);
    }
       break;

    case QMI_LOC_GET_WWAN_DATA_REQ_V02:
    {
      v_RetVal = loc_tle_GetWwanDataReq(pz_LocClientPtr,
                                               (const qmiLocGetWwanDataReqMsgT_v02 *)pz_MsgInfo->p_Msg,
                                               &z_QmiResp);
    }
    break; 

    default:
      {
        LOC_MSG_ERROR("This message:0x:%x has no known handler",
                      pz_MsgInfo->q_Id, 0, 0);
        v_RetVal = FALSE;
      }
      break;
  }

  /* Set this anyways, this will be read only if v_RetVal = TRUE, to 
     send out the response to the client */
  (*pe_QmiErr) = z_QmiResp.resp.error;

  return v_RetVal;
}

extern boolean loc_initialize_modules (const qmiLocEventRegMaskT_v02 t_EventRegMask);

/*============================================================================
 *                       Q C S I    C A L L B A C K S 
 * =========================================================================*/
/* -----------------------------------------------------------------------*//**
@brief
Callback function provided to QCSI, which notify the shim layer of a 
new client connecting. 

@param[in]   pz_ClientHandle   Handle of client that is connecting allocated by
                             QCSI
@param[in]   pz_ServiceCookie  Handle of service, as provided by SHIM layer to
                             QCSI during registration of service. 
@param[out]  pz_ConHandle      Connection handle allocated by shim given back
                             given to QCSI for reference. 

@retval    QMI_CSI_CB_NO_ERR        Client Connect was handled with success 
@retval    QMI_CSI_CB_CONN_REFUSED  If client connect cannot be accepted
@retval    QMI_CSI_CB_INTERNAL_ERR  For all other errors

@warning
A new client handle is accepted only if it is not present in the client 
queue.
*/ /* ------------------------------------------------------------------------*/
static qmi_csi_cb_error locQmiShimClientConnectCb
(
   void                     *p_ClientHandle,
   void                     *pz_ServiceCookie,
   void                    **pz_ConHandle
   )

{
  locQmiShimCltInfoStructT *pz_CltInfo;
  qmi_csi_cb_error e_RetVal = QMI_CSI_CB_INTERNAL_ERR;
  static boolean loc_modules_initialised = FALSE;
  if(FALSE == loc_modules_initialised)
  {
     loc_modules_initialised = loc_initialize_modules((qmiLocEventRegMaskT_v02) 0x0FFFFF);
  }
  
  if(loc_middleware_initialized() && loc_modules_initialised != TRUE)
   {
      e_RetVal = QMI_CSI_CB_CONN_REFUSED;
   }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Check if the Client Handle is Non-Null, and is not a duplicate that 
     has already been used */
  else if ((NULL != p_ClientHandle) &&
      (pz_ServiceCookie == &z_LocQmiShim) &&
      (NULL == q_linear_search(&z_LocQmiShim.z_ClientQ,
                               locQmiShimCompareQmiClientHandle,
                               p_ClientHandle)))
  {
    pz_CltInfo =
       (locQmiShimCltInfoStructT *)os_MemAlloc(
       sizeof(locQmiShimCltInfoStructT),
       OS_MEM_SCOPE_TASK);

    if (pz_CltInfo != NULL)
    {
      /* Memset to "0" for first use, it might have garbage and cause 
         trouble */
      memset(pz_CltInfo,
             0,
             sizeof(*pz_CltInfo));

      /* Initialize the link, return value is a pointer to the link, 
         not needed */
      (void)q_link(pz_CltInfo, &pz_CltInfo->z_Link);

      /* Store the client handle allocated by QCSI */
      pz_CltInfo->p_QmiClientHandle = p_ClientHandle;

      pz_CltInfo->v_IsTdpClient    = FALSE;

      /* Trigger Loc Open with a IOCTL Event Registration Mask, Event 
         Registration Mask is expected to be updated by client later on */
      locClnt_Init(locQmiShimEventCb, pz_CltInfo);

      if (pz_CltInfo->l_LocClientHandle == LOC_CLIENT_HANDLE_INVALID)
      {
        LOC_MSG_ERROR(" Could not accept a new client loc_open() ",
                      0, 0, 0);

        /* Free the memory in case of error */
        os_MemFree((void **)&pz_CltInfo);

        /* Connection is refused because of Loc-Api error */
        e_RetVal = QMI_CSI_CB_CONN_REFUSED;
      }
      else
      {
        /* Add it to the queue of clients */
        q_put(&z_LocQmiShim.z_ClientQ,
              &pz_CltInfo->z_Link);

        /* Give the Client Info as a handle to the QCSI Framework */
        (*pz_ConHandle) = pz_CltInfo;

        e_RetVal = QMI_CSI_CB_NO_ERR;
      }
    } /* if pz_CltInfo == NULL */
    else
    {
      LOC_MSG_ERROR(" Could not allocate memory for a new client",
                    0, 0, 0);
    }
  }
  else
  {
    LOC_MSG_ERROR(" Invalid input parameters rejecting the connect ",
                  0, 0, 0);
  }
  return e_RetVal;
}



/* -----------------------------------------------------------------------*//**
@brief
Callback function provided to QCSI, which notifies the shim layer of an  
existing client disconnecting. At this point loc-close is called to 
release the client handle with loc-api

@param[in]   pz_ConHandle     QMI-Handle of client that is Disconnecting. 
@param[in]   pz_ServiceCookie  Handle of service, as provided by SHIM layer to
                             QCSI during registration of service. 

*/ /* ------------------------------------------------------------------------*/
static void  locQmiShimClientDisconnectCb
(
   void                      *pz_ConHandle,
   void                      *pz_ServiceCookie
   )
{
  locQmiShimCltInfoStructT *pz_QmiShimCltInfo = NULL;
  LOC_MSG_HIGH("locQmiShimClientDisconnectCb : locQmiShimClnt 0x%x", pz_ConHandle, 0, 0);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL != pz_ConHandle)
  {
    /* Find the Connection Handle in the queue */
    pz_QmiShimCltInfo =
       (locQmiShimCltInfoStructT *)q_linear_search(&z_LocQmiShim.z_ClientQ,
                                                   locQmiShimCompareConnectionInfoHandle,
                                                   pz_ConHandle);
    if (NULL != pz_QmiShimCltInfo)
    {
      LOC_MSG_HIGH("locQmiShimClientDisconnectCb : ClientHandle = %d",
                   pz_QmiShimCltInfo->l_LocClientHandle, 0, 0);

      /* If the client registered for Geofence indication, purge
      geofences. ASSUMPTION: Only one client shall register for
      geofences and when this client deregisters or is killed, 
      all geofences in the system shall be deleted */
      if ((pz_QmiShimCltInfo->t_EventMask & QMI_LOC_EVENT_MASK_GEOFENCE_BREACH_NOTIFICATION_V02) ||
          (pz_QmiShimCltInfo->t_EventMask & QMI_LOC_EVENT_MASK_GEOFENCE_BATCH_BREACH_NOTIFICATION_V02))
      {
        /* no error checking done, errors are logged inside the
        function */
        locGeofencePurge();
      }

      if (LOC_API_SUCCESS != loc_close(pz_QmiShimCltInfo->l_LocClientHandle))
      {
        LOC_MSG_ERROR("loc_close returned error", 0, 0, 0);
      }

      /* Remove from Queue */
      if (TRUE != q_delete_ext(&z_LocQmiShim.z_ClientQ,
                               &pz_QmiShimCltInfo->z_Link))
      {
        LOC_MSG_ERROR("Could not delete the client info from queue ", 0, 0, 0);
      }
      else
      {
      /* Free the memory in case of error */
      os_MemFree((void **)&pz_QmiShimCltInfo);
        loc_middleware_data.z_locClnt_Data.l_MaxNumClients--;
      }
    }
  }
}

/* -----------------------------------------------------------------------*//**
@brief
 This callback is invoked when the infrastructure receives an incoming 
 message. The infrastructure decodes the data and gives it to the services 

@param[in]  pz_ConHandle           Service handle used by the infrastructure
                                 to communicate to each individual service.
@param[in]  z_ReqHandle            Opaque handle provided by the infrastructure
                                 to specify a particular transaction and
                                 message.

@param[in]  i_MsgId                Message Id pertaining to a particular
                                 message. 
@param[in]  pz_Msg                 C struct with the decoded data.
@param[in]  i_MsgLen               Length of the c struct.
@param[in]  pz_ServiceCookie         Service specific data.Service cookie is 
                                 registered with the infrastructure during 
                                 service registration(qmi_csi_register).


@retval    QMI_CSI_NO_ERR          Success
@retval    QMI_CSI_CB_INTERNAL_ERR Error, Could not handle the message
*/ /* ------------------------------------------------------------------------*/
static qmi_csi_cb_error locQmiShimHandleRequestCb
(
   void                    *pz_ConHandle,
   qmi_req_handle           z_ReqHandle,
   uint32                   q_MsgId,
   void                    *pz_Msg,
   uint32                   q_MsgLen,
   void                    *pz_ServiceCookie
   )
{
  locQmiShimMsgInfoStructT        z_MsgInfo;
  qmiLocGenRespMsgT_v02           z_QmiLocResp;
  qmi_csi_cb_error                rc = QMI_CSI_CB_INTERNAL_ERR;
  boolean                         v_RetVal = FALSE;
  locQmiShimCltInfoStructT       *pz_CltInfo = NULL;
  qmi_error_type_v01              e_QmiErr = QMI_ERR_NONE_V01;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* NULL pz_ConHandle is handled */
  pz_CltInfo = q_linear_search(&z_LocQmiShim.z_ClientQ,
                               locQmiShimCompareConnectionInfoHandle,
                               pz_ConHandle);

  LOC_MSG_HIGH("Enter locQmiShimHandleRequestCb", 0, 0, 0);
  if ((NULL != pz_CltInfo) &&
      (NULL != z_ReqHandle) &&
      (((0 == q_MsgLen) && (NULL == pz_Msg)) ||
       ((0 < q_MsgLen) && (NULL != pz_Msg))) &&
      (pz_ServiceCookie == &z_LocQmiShim))
  {
    LOC_MSG_MED("locQmiShimHandleRequestCb: >>> QMI_LOC Msg 0x%04x req, LocClientHandle %d z_ReqHandle 0x%x", q_MsgId, pz_CltInfo->l_LocClientHandle, z_ReqHandle);
    LOC_MSG_MED("Received q_MsgLen:%d pz_Msg 0x%x pz_ServiceCookie 0x%x", q_MsgLen, pz_Msg, pz_ServiceCookie);
    z_MsgInfo.q_Id = q_MsgId;
    z_MsgInfo.p_Msg = pz_Msg;
    z_MsgInfo.q_MsgLen = q_MsgLen;

    /* Handle all Batching Requests here by sending immediate Indication after processing.
     Otherwise, handle all other Requests normally */
    switch (q_MsgId)
    {
      case QMI_LOC_GET_BATCH_SIZE_REQ_V02:
        {
          v_RetVal = locGetBatchSizeReq(pz_CltInfo, z_ReqHandle, &z_MsgInfo);
          return (v_RetVal == TRUE ? QMI_CSI_CB_NO_ERR : QMI_CSI_CB_INTERNAL_ERR);
        }
      case QMI_LOC_READ_FROM_BATCH_REQ_V02:
        {
          v_RetVal = locReadFromBatchReq(pz_CltInfo, z_ReqHandle, &z_MsgInfo);
          return (v_RetVal == TRUE ? QMI_CSI_CB_NO_ERR : QMI_CSI_CB_INTERNAL_ERR);
        }
      case QMI_LOC_RELEASE_BATCH_REQ_V02:
        {
          v_RetVal = locReleaseBatchReq(pz_CltInfo, z_ReqHandle, &z_MsgInfo);
          return (v_RetVal == TRUE ? QMI_CSI_CB_NO_ERR : QMI_CSI_CB_INTERNAL_ERR);
        }
      case QMI_LOC_STOP_BATCHING_REQ_V02:
        {
          v_RetVal = locStopBatchingReq(pz_CltInfo, z_ReqHandle, &z_MsgInfo);
          return (v_RetVal == TRUE ? QMI_CSI_CB_NO_ERR : QMI_CSI_CB_INTERNAL_ERR);
        }
        /* If it is the Start Batching Request, check the batching parameters inputed by client. */
      case QMI_LOC_START_BATCHING_REQ_V02:
        {
          v_RetVal = locStartBatchingReq(pz_CltInfo,
                                         z_ReqHandle,
                                         &z_MsgInfo);

          LOC_MSG_MED("locQmiShimHandleRequestCb: "
                      "locStartBatchingReq returned %d", v_RetVal, 0, 0);
          rc = (v_RetVal == TRUE ? QMI_CSI_CB_NO_ERR : QMI_CSI_CB_INTERNAL_ERR);
          return rc;
        }
      case QMI_LOC_START_DBT_REQ_V02:
        {
          v_RetVal = locStartDbtReq(pz_CltInfo, z_ReqHandle, &z_MsgInfo);
          return (v_RetVal == TRUE ? QMI_CSI_CB_NO_ERR : QMI_CSI_CB_INTERNAL_ERR);
        }
      case QMI_LOC_STOP_DBT_REQ_V02:
        {
          v_RetVal = locStopDbtReq(pz_CltInfo, z_ReqHandle, &z_MsgInfo);
          return (v_RetVal == TRUE ? QMI_CSI_CB_NO_ERR : QMI_CSI_CB_INTERNAL_ERR);
        }
      case QMI_LOC_QUERY_AON_CONFIG_REQ_V02:
        {
          v_RetVal = locQueryAonConfigReq(pz_CltInfo,
                                          z_ReqHandle,
                                          &z_MsgInfo);

          rc = (v_RetVal == TRUE ? QMI_CSI_CB_NO_ERR : QMI_CSI_CB_INTERNAL_ERR);
          return rc;
        }
      case QMI_LOC_START_OUTDOOR_TRIP_BATCHING_REQ_V02:
        {
          return rc;
        }

      case QMI_LOC_QUERY_OTB_ACCUMULATED_DISTANCE_REQ_V02:
        {
          break;
        }

        /* Multiple Case Statements handled by the same function */

      /*9x05: NMEA not supported*/
      case QMI_LOC_SET_XTRA_T_SESSION_CONTROL_REQ_V02:
      case QMI_LOC_GET_XTRA_T_SESSION_CONTROL_REQ_V02:
      case QMI_LOC_SET_POSITION_ENGINE_CONFIG_PARAMETERS_REQ_V02:
      case QMI_LOC_GET_POSITION_ENGINE_CONFIG_PARAMETERS_REQ_V02:
      case QMI_LOC_SET_XTRA_VERSION_CHECK_REQ_V02:

      case QMI_LOC_DELETE_GNSS_SERVICE_DATA_REQ_V02:
      case QMI_LOC_INJECT_XTRA_PCID_REQ_V02:
        {

            /* Set up global PA module structure with QMI LOC specific info */
            v_RetVal = locPa_ProcessQmiRequest(pz_CltInfo->l_LocClientHandle,
                                               q_MsgId, pz_Msg, q_MsgLen, &e_QmiErr);
            if ((TRUE == v_RetVal) && (NULL !=  pz_Msg))
            {
              /* Store the request to set/get PE Config for later access (setting failure mask for send indication) */
              if (QMI_LOC_SET_POSITION_ENGINE_CONFIG_PARAMETERS_REQ_V02 == q_MsgId)
              {
                pz_CltInfo->z_PaReqUnion.z_SetPeCfg = *((qmiLocSetPositionEngineConfigParametersReqMsgT_v02 *)pz_Msg);
              }
              else if (QMI_LOC_GET_POSITION_ENGINE_CONFIG_PARAMETERS_REQ_V02 == q_MsgId)
              {
                pz_CltInfo->z_PaReqUnion.z_GetPeCfg = *((qmiLocGetPositionEngineConfigParametersReqMsgT_v02 *)pz_Msg);
              }
            }
          }
        break;

      case QMI_LOC_REG_EVENTS_REQ_V02:
      case QMI_LOC_START_REQ_V02:
      case QMI_LOC_GET_FIX_CRITERIA_REQ_V02:
      case QMI_LOC_GET_LOCATION_BACKOFF_PERIOD_REQ_V02:
      case QMI_LOC_SYSTEM_INFO_REQ_V02:
        {
          /* Set up global PD module structure with QMI LOC specific info */
          v_RetVal = locClnt_ProcessQmiRequest(pz_CltInfo->l_LocClientHandle,
                                               q_MsgId, pz_Msg, q_MsgLen);
          if (TRUE == v_RetVal)
          {
            if (QMI_LOC_REG_EVENTS_REQ_V02 == q_MsgId)
            {
              /* Cache the EventMask for future use to filter events when generated from LocAPI */
              pz_CltInfo->t_EventMask = ((qmiLocRegEventsReqMsgT_v02 *)pz_Msg)->eventRegMask;
            }
            else if (QMI_LOC_START_REQ_V02 == q_MsgId)
            {
              qmiLocStartReqMsgT_v02 *p_StartReq = (qmiLocStartReqMsgT_v02 *)pz_Msg;

              /* Cache the session ID to be used when relaying the position Reports.
                 NOTE: When stop request is received, this session ID is not deleted,
                       as a new start request will overwrite the old session ID. */
              pz_CltInfo->u_SessionId = p_StartReq->sessionId;

              /* Reset the fix ID to zero whenever there is a start request */
              pz_CltInfo->q_FixId = 0;

              /* Check altitude assumed configuration from client */
              pz_CltInfo->e_CfgAltAssumed = eQMI_LOC_ALTITUDE_ASSUMED_IN_GNSS_SV_INFO_ENABLED_V02; /* Set default */
              if (p_StartReq->configAltitudeAssumed_valid)
              {
                pz_CltInfo->e_CfgAltAssumed = p_StartReq->configAltitudeAssumed;
              }
            }
          }
          else
          {
            e_QmiErr = QMI_ERR_MALFORMED_MSG_V01;
          }
        }
        break;

      case QMI_LOC_GET_REGISTERED_EVENTS_REQ_V02:
        {
          v_RetVal = locQmiShimHandleGetRegisteredEventsReq(pz_CltInfo,
                                                            &z_MsgInfo,
                                                            &e_QmiErr);
          break;
        }

      case QMI_LOC_INJECT_POSITION_REQ_V02:
      case QMI_LOC_GET_BEST_AVAILABLE_POSITION_REQ_V02:
      case QMI_LOC_GET_AVAILABLE_WWAN_POSITION_REQ_V02:
      case QMI_LOC_INJECT_GSM_CELL_INFO_REQ_V02:

        {
          /* Set up global PD module structure with QMI LOC specific info */
          v_RetVal = locPd_ProcessQmiRequest(pz_CltInfo->l_LocClientHandle,
                                             q_MsgId, pz_Msg, q_MsgLen);

          if (FALSE == v_RetVal)
          {
            e_QmiErr = QMI_ERR_MALFORMED_MSG_V01;
          }
        }
        break;

      case QMI_LOC_GET_PREDICTED_ORBITS_DATA_SOURCE_REQ_V02:
      case QMI_LOC_GET_PREDICTED_ORBITS_DATA_VALIDITY_REQ_V02:
      case QMI_LOC_INJECT_UTC_TIME_REQ_V02:
      case QMI_LOC_INJECT_XTRA_DATA_REQ_V02:
      case QMI_LOC_QUERY_XTRA_INFO_REQ_V02:
        {
          /* Set up global XTRA module structure with QMI LOC specific info */
          v_RetVal = locXtra_ProcessQmiRequest(pz_CltInfo->l_LocClientHandle,
                                               q_MsgId, pz_Msg, q_MsgLen);
          if (FALSE == v_RetVal)
          {
            e_QmiErr = QMI_ERR_MALFORMED_MSG_V01;
          }
        }
        break;

      /*#9x05: loc_wifi loc_ni not supported*/



      case QMI_LOC_STOP_REQ_V02:
        v_RetVal = locQmiShimStopReqHdlr(pz_CltInfo,
                                         &z_MsgInfo,
                                         &e_QmiErr);
        break;


      case QMI_LOC_INFORM_CLIENT_REVISION_REQ_V02:
        v_RetVal = locQmiShimHandleClientRevision(pz_CltInfo,
                                                  &z_MsgInfo,
                                                  &e_QmiErr);
        break;

      case QMI_LOC_GET_SERVICE_REVISION_REQ_V02:
        v_RetVal = locQmiShimHandleGetServRevisionReq_v02(pz_CltInfo,
                                                          &z_MsgInfo,
                                                          &e_QmiErr);
        break;

        /* Multiple Case Statements handled by the same function */
      case QMI_LOC_ADD_CIRCULAR_GEOFENCE_REQ_V02:
      case QMI_LOC_DELETE_GEOFENCE_REQ_V02:
      case QMI_LOC_SET_GEOFENCE_ENGINE_CONFIG_REQ_V02:
      case QMI_LOC_INJECT_GTP_CLIENT_DOWNLOADED_DATA_REQ_V02:
      case QMI_LOC_GET_FDCL_BS_LIST_REQ_V02:
      case QMI_LOC_INJECT_FDCL_DATA_REQ_V02:
      case QMI_LOC_GET_BS_OBS_DATA_REQ_V02:
      case QMI_LOC_GET_SUPPORTED_FEATURE_REQ_V02:
      case QMI_LOC_QUERY_GNSS_ENERGY_CONSUMED_REQ_V02:
      case QMI_LOC_GET_WWAN_DATA_REQ_V02:
#if 0
      case QMI_LOC_START_LISTEN_POSITION_REQ_V02:
      case QMI_LOC_STOP_LISTEN_POSITION_REQ_V02:
#endif 
        v_RetVal = locQmiShimRouteMessages(pz_CltInfo,
                                           &z_MsgInfo,
                                           &e_QmiErr);
        break;

      default:
        LOC_MSG_ERROR("locQmiShimHandleRequestCb: Not supported QMI_LOC Msg: 0x%04x",
                      q_MsgId, 0, 0);
        e_QmiErr = QMI_ERR_NOT_SUPPORTED_V01;
        break;
    }

    /* Fill and send the response back to the client */
    if (TRUE == v_RetVal)
    {
      z_QmiLocResp.resp.result = QMI_RESULT_SUCCESS_V01;
    }
    else
    {
      z_QmiLocResp.resp.result = QMI_RESULT_FAILURE_V01;
      LOC_MSG_ERROR("locQmiShimHandleRequestCb: Could not handle QMI_LOC request Msg 0x%04x", q_MsgId, 0, 0);
    }
    z_QmiLocResp.resp.error  = e_QmiErr;

    /* Msg Id for Req and Resp are the same, hence sending response to the 
       incoming message id */
    if (QMI_CSI_NO_ERR != mqcsi_send_rsp(
          pz_CltInfo->p_QmiClientHandle,
          z_ReqHandle,
          q_MsgId,
          &z_QmiLocResp,
          sizeof(z_QmiLocResp)))
    {
      LOC_MSG_ERROR("locQmiShimHandleRequestCb: <<< QMI_LOC Msg 0x%04x resp failed for locClient %d",
                    q_MsgId, pz_CltInfo->l_LocClientHandle, 0);
      v_RetVal = FALSE;
    }
    else
    {
      LOC_MSG_MED("locQmiShimHandleRequestCb: <<< QMI_LOC Msg 0x%04x resp succeed,error:0x%04x,result:%d ",
                  q_MsgId, z_QmiLocResp.resp.error, z_QmiLocResp.resp.result);
    }
  }
  else
  {
    LOC_MSG_ERROR("locQmiShimHandleRequestCb: Invalid Connection Info Handle from QCSI",
                  0, 0, 0);
  }

  rc = (v_RetVal == TRUE ? QMI_CSI_CB_NO_ERR : QMI_CSI_CB_INTERNAL_ERR);
  LOC_MSG_MED("Eixt locQmiShimHandleRequestCb", 0, 0, 0);
  return rc;
} /* handle_req */

/*****************************************************************************
 *****************************************************************************
 *
 *                   Externalized Function Definitions
 *
 *****************************************************************************
 * ***************************************************************************/

/* -----------------------------------------------------------------------*//**
@brief
This function needs to be called to initialize the QMI-Loc module. It
initializes the internal data structures of the module and registers with
SM to receive a callback letting QMI-Loc know when PDAPI is initialized. 

No other QMI related initialization, specifically no registration of QMI-Loc
service with the QMI framework is performed here.

@retval  TRUE  - Successful initialization of the module
@retval  FALSE - Module initialization was not successful

@warning
This function does not gaurd itself against being called multiple times. 
Once called it can be called again only if "cleanup" is called. 
*/ /* ------------------------------------------------------------------------*/
boolean locQmiShimInit
(
   void
   )
{
  boolean             v_RetVal = TRUE;
  uint8               u_Cntr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Initialize the Service Context Information */
  memset(&z_LocQmiShim,
         0,
         sizeof(z_LocQmiShim));


  /* Initialize the queue which will hold all handles of clients and maintain 
     the mapping of QMI-Client-handles and the loc-api-client handles */
  (void)q_init(&z_LocQmiShim.z_ClientQ);

  /*9x05: initialize QMI interface*/
  locQmi_ProcessCmdRequest();

  return v_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
This function is called to reset and disconnect the shim layer from the 
QMI-Loc Service. After this function call, the loc-middleware task will need 
to call locQmiShimInit() to be able to communicate over the QMI-framework. 
*/ /* ------------------------------------------------------------------------*/
void locQmiShimCleanup
(
   void
   )
{
  locQmiShimCltInfoStructT *pz_CltInfo = NULL;
  qmi_csi_error             rc = QMI_CSI_INTERNAL_ERR;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*  Continue Stopping only if we have a valid service handle */
  if (z_LocQmiShim.p_ServiceHandle != NULL)
  {
    /** Unregister with QSAP */
    if (z_LocQmiShim.z_QSAPUserHandle)
    {
      if (QMI_SAP_NO_ERR != qmi_sap_deregister(z_LocQmiShim.z_QSAPUserHandle))
      {
        LOC_MSG_ERROR("Could not deregister the service with QSAP",
                      0, 0, 0);
      }
    }

    /* Unregister with framework, so no more messages arrive */
    rc = mqcsi_unregister(z_LocQmiShim.p_ServiceHandle);
    if (rc != QMI_CSI_NO_ERR)
    {
      LOC_MSG_ERROR("Could not deregister the service successfully",
                    0, 0, 0);
    }

    /* Reset to initial values */
    memset(&z_LocQmiShim.p_ServiceHandle,
           0,
           sizeof(z_LocQmiShim.p_ServiceHandle));

    /* For each client call the disconnect callback. The same actions need to 
       be taken. Hence q_check() instead of q_get() */
    pz_CltInfo =
       (locQmiShimCltInfoStructT *)q_check(&z_LocQmiShim.z_ClientQ);
    while (NULL != pz_CltInfo)
    {

      /* Trigger the function that handles disconnect client callback, 
         as at this point the same functionality needs to be performed 
         NOTE: Memory is freed inside this call */
      locQmiShimClientDisconnectCb((void *)pz_CltInfo,
                                   &z_LocQmiShim);

      /* Get the next connection handle. 
         NOTE: it is q_check() not q_get() */
      pz_CltInfo =
         (locQmiShimCltInfoStructT *)q_check(&z_LocQmiShim.z_ClientQ);
    }

    if (0 != q_cnt(&z_LocQmiShim.z_ClientQ))
    {
      LOC_MSG_ERROR("Cleanup was not proper, still some clients dangling ",
                    0, 0, 0);
    }

    /* Destroy the queue. It is expected to call Init() again before using this 
       module */
    q_destroy(&z_LocQmiShim.z_ClientQ);

  }
}

/* -----------------------------------------------------------------------*//**
@brief
This function is called by the Service owner indicating that there is a 
QMI-Event that is queued up in QCSI that needs to be handled.
*/ /* ------------------------------------------------------------------------*/
void locQmiShimHandleEvent
(
   void
   )
{
#ifndef __GRIFFON__
  qmi_csi_os_params   z_OsParams;
#endif /* __GRIFFON__ */
  qmi_csi_error       e_Err;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the TCB for the task in which initialization of shim was called 
     Assumption is that this function will be called in the service provider's
     context, and hence os_Self() should return the service provider's 
     context. If called in someone else's context, qmi_csi_handle_event() 
     should return an error */

#ifndef __GRIFFON__
  z_OsParams.tcb = os_Self();
  z_OsParams.sig = LOC_QMI_SHIM_SIGNAL;
#endif /* __GRIFFON__ */

  LOC_MSG_HIGH("Handle QMI Event", 0, 0, 0);
  e_Err = mqcsi_handle_event(z_LocQmiShim.p_ServiceHandle,
#ifdef __GRIFFON__
                             &g_z_OsParams
#else /* __GRIFFON__ */
                             & z_OsParams
#endif /* __GRIFFON__ */
                             );

  if (QMI_CSI_NO_ERR != e_Err)
  {
    LOC_MSG_ERROR("Could not handle the event properly Err:%d ",
                  e_Err, 0, 0);
  }
#ifdef __GRIFFON__

  //Unblock the "select" thread, ok to "select" the UDP socket now
  UPOS_ThreadActivate(&upos_thread);
#endif  /* __GRIFFON__ */
}
/* -----------------------------------------------------------------------*//**
@brief
Function to process the locQmiShimInitNotifierCb. This function checks with SM
which service is enabled, QMI-LOC/QMI-PDS/etc. and if QMI-LOC is enabled, 
registers the QMI-LOC service with QCSI.
*/ /* ------------------------------------------------------------------------*/
void locQmi_ProcessCmdRequest
(
   
   )
{
#ifndef __GRIFFON__
  qmi_csi_os_params   z_OsParams;
#endif /* __GRIFFON__ */
  qmi_csi_error                rc = QMI_CSI_INTERNAL_ERR;
  qmi_idl_service_object_type  z_QmiLocServiceObject = NULL;
  uint32_t                     q_SvcId;
  pdsm_lite_QMIServiceConfig        qmi_config;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (FALSE == pdsm_lite_read_qmi_services_config(&qmi_config))
  {
    LOC_ERR_FATAL("Could not read QMI Config!!", 0, 0, 0);
    return;
  }

  if (qmi_config.enableMask & PDSM_LITE_QMI_CONFIG_ENABLE_LOC)
  {
    loc_middleware_data.qmi_loc_enabled = TRUE;
  }
  else
  {
    /* Do not register with QMI framework, as QMI-LOC disabled in NV */
    LOC_MSG_HIGH("QMI-LOC Disabled!!", 0, 0, 0);
    return;
  }
#ifdef __GRIFFON__
  /* start the UDP transport */
  qmi_csi_xport_start(&udp_ops, NULL);

#else /* __GRIFFON__ */
  /* Get the TCB for the task in which initialization of shim was called */
  z_OsParams.tcb = os_Self();
  z_OsParams.sig = LOC_QMI_SHIM_SIGNAL;
#endif /* __GRIFFON__ */

  z_QmiLocServiceObject =  loc_iot_get_service_object_v02();

  if (QMI_IDL_LIB_NO_ERR == qmi_idl_get_service_id(z_QmiLocServiceObject,
                                                   &q_SvcId))
  {

    /* Register with QMI SI for logging of supported interfaces */
    qmi_si_register_object(z_QmiLocServiceObject,
                           0,
                           loc_get_service_impl_v02());

#ifdef __GRIFFON__
    printf("QMI-LOC trying to register with QCSI\n");
#endif /* __GRIFFON__ */
    /* Register the service with QCSI Framework */
    rc = mqcsi_register(q_SvcId,
                        LOC_IOT_V02_IDL_MAJOR_VERS,
                        LOC_IOT_V02_IDL_MINOR_VERS,
                        z_QmiLocServiceObject,
                        locQmiShimClientConnectCb,
                        locQmiShimClientDisconnectCb,
                        locQmiShimHandleRequestCb,
                        (void *)&z_LocQmiShim,
#ifdef __GRIFFON__
                        &g_z_OsParams,
#else /* __GRIFFON__ */
                        &z_OsParams,
#endif /* __GRIFFON__ */
                        &(z_LocQmiShim.p_ServiceHandle));


    if (QMI_CSI_NO_ERR == rc)
    {
      qmi_sap_error sapErr;
      LOC_MSG_HIGH("QMI-LOC Registered Successfully with QCSI", 0, 0, 0);

      /** Register the service with QSAP*/
      sapErr = qmi_sap_register(z_QmiLocServiceObject,
                                NULL,
                                &(z_LocQmiShim.z_QSAPUserHandle));

      if (QMI_SAP_NO_ERR == sapErr)
      {
        LOC_MSG_HIGH("QMI-LOC Registered Successfully with QSAP", 0, 0, 0);
      }
      else
      {
        LOC_MSG_ERROR("Was not able to register successfully with QSAP Error:%d",
                      sapErr, 0, 0);
      }
#ifdef __GRIFFON__
      printf("QMI-LOC Registered Successfully with QCSI\n");

      // create a thread to read this UDP socket
      if (OK == UPOS_ThreadCreate(&upos_thread, UPOS_FindTask(NULL),
                                  read_udp_and_signal, 1, 1, 8192, "uposThread"))
      {
        printf("udp xport_open: created UPOS thread\n");
        //Activate the Thread
        UPOS_ThreadActivate(&upos_thread);
      }
      else
      {
        printf("udp xport_open: Could not create UPOS thread\n");
      }
#endif /* __GRIFFON__ */
    }
    else
    {
#ifdef __GRIFFON__
      printf("Was not able to register successfully with QCSI Error:%d\n", rc);
#endif /* __GRIFFON__ */
      LOC_MSG_ERROR("Was not able to register successfully with QCSI Error:%d",
                    rc, 0, 0);
    }
  }
  else
  {
#ifdef __GRIFFON__
    printf("Was not able to get Service ID: %d\n", rc);
#endif /* __GRIFFON__ */
    LOC_MSG_ERROR("Was not able to get Service ID",
                  0, 0, 0);
  }
}

/* -----------------------------------------------------------------------*//**
@brief
This function is to be called by the module that intends to send out an 
indication to the client of the QMI-LOC, Service. 

@return TRUE    The indication was successfully sent over QCSI
@return FALSE   The indication was not successfully sent over QCSI

@warning
This function assumes that it is called in the context of the service 
providers task. ( Loc-MW task )
*/ /* ------------------------------------------------------------------------*/
boolean locQmiShimSendInd
(
   loc_client_handle_type                  l_LocClientHandle,
   const locQmiShimIndInfoStructT         *pz_IndInfo
   )
{
  boolean v_RetVal = TRUE;
  locQmiShimCltInfoStructT   *pz_CltInfo = NULL;
  qmi_csi_error               e_QmiErr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  pz_CltInfo = q_linear_search(&z_LocQmiShim.z_ClientQ,
                               locQmiShimCompareLocApiHandle,
                               (void *)l_LocClientHandle);
  if ((NULL != pz_CltInfo) &&
      (NULL != pz_IndInfo))
  {
    e_QmiErr = mqcsi_send_ind(pz_CltInfo->p_QmiClientHandle,
                              pz_IndInfo->q_Id,
                              pz_IndInfo->p_Msg,
                              pz_IndInfo->q_MsgLen);

    if (QMI_CSI_NO_ERR != e_QmiErr)
    {
      /* Could not send an event out, some error in QCSI framework */
      LOC_MSG_ERROR("locQmiShimSendInd: <<< QMI_LOC Msg 0x%04x ind failed for locClient %d QMI_CSI_NO_ERR=%d",
                    pz_IndInfo->q_Id, l_LocClientHandle, e_QmiErr);
      v_RetVal = FALSE;
    }
    else
    {
      LOC_MSG_MED("locQmiShimSendInd: <<< QMI_LOC Msg 0x%04x ind succeed for LocClient %d",
                  pz_IndInfo->q_Id, l_LocClientHandle, 0);
    }
  }
  else
  {
    LOC_MSG_ERROR("locQmiShimSendInd: Could not find the QMI-client for LocClient %d",
                  l_LocClientHandle, 0, 0);
    v_RetVal = FALSE;
  }
  return v_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
This function is to be called by the module that intends to broadcast an 
indication to multiple clients of the QMI-LOC, Service. 

@return TRUE    The indication was successfully sent over QCSI
@return FALSE   The indication was not successfully sent over QCSI

@warning
This function assumes that it is called in the context of the service 
providers task. ( Loc-MW task )
*/ /* ------------------------------------------------------------------------*/
boolean locQmiShimBroadcastInd
(
   uint64                           t_EventMask,
   const locQmiShimIndInfoStructT  *pz_IndInfo
   )
{
  boolean v_RetVal = TRUE;
  locQmiShimCltInfoStructT   *pz_CltInfo = NULL;
  uint32 error_code = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL != pz_IndInfo)
  {
    LOC_MSG_HIGH("locQmiShimBroadcastInd QMI_LOC_EVENT_MASK:0x%08x, QMI_LOC Msg 0x%04x, len:%u",
                 (uint32)(t_EventMask & 0x00000000FFFFFFFFULL), pz_IndInfo->q_Id, pz_IndInfo->q_MsgLen);

    for (pz_CltInfo = (locQmiShimCltInfoStructT *)q_check(&z_LocQmiShim.z_ClientQ);
         pz_CltInfo != NULL;
         pz_CltInfo = (locQmiShimCltInfoStructT *)q_next(&(z_LocQmiShim.z_ClientQ), &(pz_CltInfo->z_Link)))
    {
      if (0 != (t_EventMask & pz_CltInfo->t_EventMask))
      {
        /* this client has registered for something matching this event 
           (mask) */
        if (TRUE != locQmiShimSendInd(pz_CltInfo->l_LocClientHandle,
                                      pz_IndInfo))
        {
          error_code = 1;
        }
      }
      else
      {
        LOC_MSG_HIGH_0("QMI_LOC Msg 0x%04x is NOT REGISTERED");
      }
    }
  }
  else
  {
    error_code = 2;
  }

  if (0 != error_code)
  {
    v_RetVal = FALSE;
    LOC_MSG_ERROR("locQmiShimBroadcastInd error: %u", error_code, 0, 0);
  }
  return v_RetVal;
}

/* -----------------------------------------------------------------------*//**
@brief
Untility function to send Response mesasge to Control point 

@param[in]   pz_CltInfo            Client Info Handle 
@param[in]   z_ReqHandle           Opaque handle provided by the infrastructure
                                 to specify a particular transaction and message.
@param[in]   pz_MsgInfo            Shim info struct containing the response message
@param[out]  pResp                 Pointer to send the response
@param[in]   resp_value            Response status being sent to control point
@param[in]   e_QmiErr              QMI Error type being sent to control point


@retval    TRUE           Sending Response to control point success
@retval    FALSE          Sending Response to control point fail
*/ /* ------------------------------------------------------------------------*/
 boolean locBatchingOperationSendResp(locQmiShimCltInfoStructT    *pz_CltInfo,
                                            qmi_req_handle              z_ReqHandle,
                                            locQmiShimMsgInfoStructT    *pz_MsgInfo,
                                            boolean                     resp_value,
                                            qmi_error_type_v01          e_QmiErr)
{
  boolean v_RetVal = TRUE;
  qmiLocGenRespMsgT_v02           z_QmiLocResp;

  memset(&z_QmiLocResp, 0, sizeof(z_QmiLocResp));

  /* Fill and send the response back to the client */
  if (TRUE == resp_value)
  {
    z_QmiLocResp.resp.result = QMI_RESULT_SUCCESS_V01;
    z_QmiLocResp.resp.error = QMI_ERR_NONE_V01;
  }
  else
  {
    z_QmiLocResp.resp.result = QMI_RESULT_FAILURE_V01;
    z_QmiLocResp.resp.error  = e_QmiErr;
  }

  /* Msg Id for Req and Resp are the same, hence sending response to the 
     incoming message id */
  if (QMI_CSI_NO_ERR != mqcsi_send_rsp(
        pz_CltInfo->p_QmiClientHandle,
        z_ReqHandle,
        pz_MsgInfo->q_Id,
        &z_QmiLocResp,
        sizeof(qmiLocGenRespMsgT_v02)))
  {
    LOC_MSG_ERROR("locBatchingOperationSendResp: <<< Send QMI_RESP 0X%04x failed for LocClient %d",
                  pz_MsgInfo->q_Id, pz_CltInfo->l_LocClientHandle, 0);
    v_RetVal = FALSE;
  }
  else
  {
    LOC_MSG_MED("locBatchingOperationSendResp: <<< Send QMI_RESP 0X%04x succeed,error:%d,result:%d ",
                pz_MsgInfo->q_Id, z_QmiLocResp.resp.error, z_QmiLocResp.resp.result);
  }
  return v_RetVal;
}


 







/*---------------------------------------------------------------------------
@brief
  Function to process QMI_LOC Indication for PA Set commands
---------------------------------------------------------------------------*/
boolean locQmi_ProcessPaSetInd
(
   loc_client_handle_type l_ClientHandle,
   pdsm_lite_pa_e_type         e_ParamType,
   pdsm_lite_pa_info_s_type *p_PdsmPaInfo,
   qmiLocStatusEnumT_v02  e_IndStatus
   )
{
  locQmiShimIndInfoStructT   z_IndInfo  = { 0 };
  locQmiShimCltInfoStructT  *pz_CltInfo  = NULL;
  boolean                    v_RetVal = FALSE;

  LOC_MSG_HIGH("locQmi_ProcessPaSetInd: ParamType=%d, ClientHandle=%d, IndStatus=%d", e_ParamType, l_ClientHandle, e_IndStatus);

  do
  {
    pz_CltInfo = q_linear_search(&z_LocQmiShim.z_ClientQ, locQmiShimCompareLocApiHandle, (void *)l_ClientHandle);
    if (NULL == pz_CltInfo)
    {
      LOC_MSG_ERROR("locQmi_ProcessPaSetInd: Invalid Client Info for Handle=%d", l_ClientHandle, 0, 0);
      break;
    }

    /* Function parameters sanity check only applies to the following Param Type: PDSM_LITE_PA_PE_CONFIG_PARAM
       All other Param Types will ignore p_PdsmPaInfo, so no check required here. */

    /* Process all PDSM Set Parameter commands here */
    switch (e_ParamType)
    {
      case PDSM_LITE_PA_GPS_LOCK:
        {
#if 0 /*Set Get GPS lock not supported on 9x05 but GPS lock may be used for QWES*/
          qmiLocSetEngineLockIndMsgT_v02 z_SetEngLockInd = { 0 };
          z_IndInfo.q_Id          = QMI_LOC_SET_ENGINE_LOCK_IND_V02;
          z_IndInfo.q_MsgLen      = sizeof(z_SetEngLockInd);
          z_SetEngLockInd.status  = e_IndStatus;
          z_IndInfo.p_Msg         = &z_SetEngLockInd;
          v_RetVal = locQmiShimSendInd(pz_CltInfo->l_LocClientHandle, &z_IndInfo);
#endif
        }
        break;

      /*9x05: NMEA not supported */



      case PDSM_LITE_PA_XTRAT_USER_SESSION_CONTROL:
        {
          qmiLocSetXtraTSessionControlIndMsgT_v02 z_SetXtraTInd = { 0 };
          z_IndInfo.q_Id        = QMI_LOC_SET_XTRA_T_SESSION_CONTROL_IND_V02;
          z_IndInfo.q_MsgLen    = sizeof(z_SetXtraTInd);
          z_SetXtraTInd.status  = e_IndStatus;
          z_IndInfo.p_Msg       = &z_SetXtraTInd;
          v_RetVal = locQmiShimSendInd(pz_CltInfo->l_LocClientHandle, &z_IndInfo);
        }
        break;

      case PDSM_LITE_PA_PE_CONFIG_PARAM:
          {
          qmiLocPositionEngineConfigParamMaskT_v02 q_FailedPeCfgMask = 0;

          qmiLocSetPositionEngineConfigParametersIndMsgT_v02  z_SetPeCfgInd = { 0 };
          z_IndInfo.q_Id        = QMI_LOC_SET_POSITION_ENGINE_CONFIG_PARAMETERS_IND_V02;
          z_IndInfo.q_MsgLen    = sizeof(z_SetPeCfgInd);
          z_SetPeCfgInd.status  = e_IndStatus;
          z_IndInfo.p_Msg       = &z_SetPeCfgInd;

          /* Function parameters sanity check */
          if (NULL == p_PdsmPaInfo)
          {
            LOC_MSG_ERROR("locQmi_ProcessPaSetInd: PDSM_LITE_PA_PE_CONFIG_PARAM NULL parameters", 0, 0, 0);
            v_RetVal = FALSE;
          }
          else
          {
            if (eQMI_LOC_SUCCESS_V02 != e_IndStatus)
            {
              /* Handle unsuccessful case from PA command callback */
              q_FailedPeCfgMask |= (pz_CltInfo->z_PaReqUnion.z_SetPeCfg.injectedPositionControl_valid) ?
                 QMI_LOC_POSITION_ENGINE_CONFIG_PARAM_MASK_INJECTED_POSITION_CONTROL_V02 : 0;
              q_FailedPeCfgMask |= (pz_CltInfo->z_PaReqUnion.z_SetPeCfg.filterSvUsage_valid) ?
                 QMI_LOC_POSITION_ENGINE_CONFIG_PARAM_MASK_FILTER_SV_USAGE_V02 : 0;
              q_FailedPeCfgMask |= (pz_CltInfo->z_PaReqUnion.z_SetPeCfg.storeAssistData_valid) ?
                 QMI_LOC_POSITION_ENGINE_CONFIG_PARAM_MASK_STORE_ASSIST_DATA_V02 : 0;
              LOC_QMI_SHIM_SET_VALUE(z_SetPeCfgInd.failedPositionEngineConfigParamMask, q_FailedPeCfgMask);
            }
            else
            {
              /* If a bit mask is set in the PDAPI structure, then parameter was successfully set */
              if (pz_CltInfo->z_PaReqUnion.z_SetPeCfg.injectedPositionControl_valid)
              {
                q_FailedPeCfgMask |= (p_PdsmPaInfo->pa_info.pe_config_info.peConfigMask &
                                         PDSM_LITE_PA_PE_CONFIG_PARAM_MASK_USE_INJECTED_POSITION) ?
                   0 : QMI_LOC_POSITION_ENGINE_CONFIG_PARAM_MASK_INJECTED_POSITION_CONTROL_V02;
              }
              if (pz_CltInfo->z_PaReqUnion.z_SetPeCfg.filterSvUsage_valid)
              {
                q_FailedPeCfgMask |= (p_PdsmPaInfo->pa_info.pe_config_info.peConfigMask &
                                         PDSM_LITE_PA_PE_CONFIG_PARAM_MASK_FILTER_SV_USED) ?
                   0 : QMI_LOC_POSITION_ENGINE_CONFIG_PARAM_MASK_FILTER_SV_USAGE_V02;
              }

              /* If there is at least one mask that failed, then the set did not succeed */
              if (q_FailedPeCfgMask != 0)
              {
                z_SetPeCfgInd.failedPositionEngineConfigParamMask_valid = TRUE;
                z_SetPeCfgInd.status = eQMI_LOC_GENERAL_FAILURE_V02;
              }
              else
              {
                z_SetPeCfgInd.status = eQMI_LOC_SUCCESS_V02;
              }
            }
            v_RetVal = locQmiShimSendInd(pz_CltInfo->l_LocClientHandle, &z_IndInfo);
          }
        }
        break;



      case PDSM_LITE_PA_XTRA_VERSION_CHECK:
        {
          qmiLocSetXtraVersionCheckIndMsgT_v02   z_SetXtraVersInd = { 0 };
          z_IndInfo.q_Id           = QMI_LOC_SET_XTRA_VERSION_CHECK_IND_V02;
          z_IndInfo.q_MsgLen       = sizeof(z_SetXtraVersInd);
          z_SetXtraVersInd.status  = e_IndStatus;
          z_IndInfo.p_Msg          = &z_SetXtraVersInd;
          v_RetVal = locQmiShimSendInd(pz_CltInfo->l_LocClientHandle, &z_IndInfo);
        }
        break;

      case PDSM_LITE_PA_DELETE_PARAMS_EX1:
        {
          qmiLocDeleteGNSSServiceDataIndMsgT_v02 z_DeleteGNSSServiceDataInd = { 0 };
          z_IndInfo.q_Id             = QMI_LOC_DELETE_GNSS_SERVICE_DATA_IND_V02;
          z_IndInfo.q_MsgLen         = sizeof(z_DeleteGNSSServiceDataInd);
          z_DeleteGNSSServiceDataInd.status = e_IndStatus;
          z_IndInfo.p_Msg            = &z_DeleteGNSSServiceDataInd;
          v_RetVal = locQmiShimSendInd(pz_CltInfo->l_LocClientHandle, &z_IndInfo);
        }
        break;


      default:
        {
          LOC_MSG_ERROR("locQmi_ProcessPaSetInd: Invalid Set Param=%d", e_ParamType, 0, 0);
          v_RetVal = FALSE;
        }
        break;
    }
  }while (0);

  return v_RetVal;
}

/*---------------------------------------------------------------------------
@brief
  Function to process QMI_LOC Indication for PA Get commands
---------------------------------------------------------------------------*/
boolean locQmi_ProcessPaGetInd
(
   loc_client_handle_type l_ClientHandle,
   pdsm_lite_pa_e_type         e_ParamType,
   pdsm_lite_pa_info_s_type *p_PdsmPaInfo,
   qmiLocStatusEnumT_v02  e_IndStatus
   )
{
  locQmiShimIndInfoStructT  z_IndInfo  = { 0 };
  locQmiShimCltInfoStructT *pz_CltInfo  = NULL;
  boolean                   v_RetVal = FALSE;

  LOC_MSG_HIGH("locQmi_ProcessPaGetInd: ParamType=%d, ClientHandle=%d, IndStatus=%d", e_ParamType, l_ClientHandle, e_IndStatus);

  pz_CltInfo = q_linear_search(&z_LocQmiShim.z_ClientQ, locQmiShimCompareLocApiHandle, (void *)l_ClientHandle);
  if (NULL == pz_CltInfo)
  {
    LOC_MSG_ERROR("locQmi_ProcessPaGetInd: Invalid Client Info for Handle=%d", l_ClientHandle, 0, 0);
    return v_RetVal;
  }

  /* Function parameters sanity check */
  if ((NULL == p_PdsmPaInfo) && (PDSM_LITE_PA_PE_CONFIG_PARAM != e_ParamType))
  {
    LOC_MSG_ERROR("locQmi_ProcessPaGetInd: NULL parameters", 0, 0, 0);
    return v_RetVal;
  }

  /* Process all PDSM Get Parameter commands here */
  switch (e_ParamType)
  {
    case PDSM_LITE_PA_GPS_LOCK:
      {
#if 0
        qmiLocGetEngineLockIndMsgT_v02   z_GetEngLockInd = { 0 };
        z_IndInfo.q_Id          = QMI_LOC_GET_ENGINE_LOCK_IND_V02;
        z_IndInfo.q_MsgLen      = sizeof(z_GetEngLockInd);
        z_GetEngLockInd.status  = e_IndStatus;
        z_IndInfo.p_Msg         = &z_GetEngLockInd;

        switch (p_PdsmPaInfo->pa_info.gps_lock)
        {
          case PDSM_LITE_GPS_LOCK_NONE:
            {
              LOC_QMI_SHIM_SET_VALUE(z_GetEngLockInd.lockType, eQMI_LOC_LOCK_NONE_V02);
              break;
            }
          case PDSM_LITE_GPS_LOCK_MI:
            {
              LOC_QMI_SHIM_SET_VALUE(z_GetEngLockInd.lockType, eQMI_LOC_LOCK_MI_V02);
              break;
            }
          case PDSM_LITE_GPS_LOCK_MT:
            {
              LOC_QMI_SHIM_SET_VALUE(z_GetEngLockInd.lockType, eQMI_LOC_LOCK_MT_V02);
              break;
            }
          case PDSM_LITE_GPS_LOCK_ALL:
            {
              LOC_QMI_SHIM_SET_VALUE(z_GetEngLockInd.lockType, eQMI_LOC_LOCK_ALL_V02);
              break;
            }
          default:
            {
              LOC_MSG_ERROR("locQmi_ProcessPaGetInd: Invalid GPS Lock Type=%d", p_PdsmPaInfo->pa_info.gps_lock, 0, 0);
              return FALSE;
            }
        }

        LOC_MSG_MED("locQmi_ProcessPaGetInd: GPS Lock Type=%d", p_PdsmPaInfo->pa_info.gps_lock, 0, 0);
        v_RetVal = locQmiShimSendInd(pz_CltInfo->l_LocClientHandle, &z_IndInfo);
#endif
      }
      break;

     /*9x05: NMEA sentence type*/

    case PDSM_LITE_PA_XTRAT_USER_SESSION_CONTROL:
        {
        qmiLocGetXtraTSessionControlIndMsgT_v02   z_GetXtraTInd = { 0 };
        z_IndInfo.q_Id        = QMI_LOC_GET_XTRA_T_SESSION_CONTROL_IND_V02;
        z_IndInfo.q_MsgLen    = sizeof(z_GetXtraTInd);
        z_GetXtraTInd.status  = e_IndStatus;
        z_IndInfo.p_Msg       = &z_GetXtraTInd;

        LOC_QMI_SHIM_SET_VALUE(z_GetXtraTInd.xtraTSessionControl, p_PdsmPaInfo->pa_info.session_control);

        LOC_MSG_MED("locQmi_ProcessPaGetInd: XTRA-T User Session Ctrl=%d", p_PdsmPaInfo->pa_info.session_control, 0, 0);
        v_RetVal = locQmiShimSendInd(pz_CltInfo->l_LocClientHandle, &z_IndInfo);
      }
      break;



    case PDSM_LITE_PA_PE_CONFIG_PARAM:
                    {
        qmiLocGetPositionEngineConfigParametersIndMsgT_v02 z_GetPeCfgInd = { 0 };
        z_IndInfo.q_Id        = QMI_LOC_GET_POSITION_ENGINE_CONFIG_PARAMETERS_IND_V02;
        z_IndInfo.q_MsgLen    = sizeof(z_GetPeCfgInd);
        z_GetPeCfgInd.status  = e_IndStatus;
        z_IndInfo.p_Msg       = &z_GetPeCfgInd;

        if (eQMI_LOC_SUCCESS_V02 != e_IndStatus)
        {
          z_GetPeCfgInd.injectedPositionControl_valid = FALSE;
          z_GetPeCfgInd.filterSvUsage_valid           = FALSE;
          z_GetPeCfgInd.storeAssistData_valid         = FALSE;
        }
        else if (NULL != p_PdsmPaInfo)
        {
          qmiLocPositionEngineConfigParamMaskT_v02 q_GetPeCfgMask = 0;
          if (p_PdsmPaInfo->pa_info.pe_config_info.peConfigMask & PDSM_LITE_PA_PE_CONFIG_PARAM_MASK_USE_INJECTED_POSITION)
          {
            LOC_QMI_SHIM_SET_VALUE(z_GetPeCfgInd.injectedPositionControl, p_PdsmPaInfo->pa_info.pe_config_info.useInjectedPosition);
            q_GetPeCfgMask |= QMI_LOC_POSITION_ENGINE_CONFIG_PARAM_MASK_INJECTED_POSITION_CONTROL_V02;
          }

          if (p_PdsmPaInfo->pa_info.pe_config_info.peConfigMask & PDSM_LITE_PA_PE_CONFIG_PARAM_MASK_FILTER_SV_USED)
          {
            LOC_QMI_SHIM_SET_VALUE(z_GetPeCfgInd.filterSvUsage, p_PdsmPaInfo->pa_info.pe_config_info.filterSvUsed);
            q_GetPeCfgMask |= QMI_LOC_POSITION_ENGINE_CONFIG_PARAM_MASK_FILTER_SV_USAGE_V02;
          }

          
          

          /* If the info received is not equal to what was requested, notify a failure */
          if (q_GetPeCfgMask != pz_CltInfo->z_PaReqUnion.z_GetPeCfg.getPositionEngineConfigParamMask)
          {
            LOC_MSG_ERROR("locQmi_ProcessPaGetInd: Invalid Get PE Config Mask=%d=%d, Status=%d",
                          q_GetPeCfgMask, pz_CltInfo->z_PaReqUnion.z_GetPeCfg.getPositionEngineConfigParamMask, e_IndStatus);
            z_GetPeCfgInd.status = eQMI_LOC_GENERAL_FAILURE_V02;
          }
          else
          {
            LOC_MSG_MED("locQmi_ProcessPaGetInd: Get PE Config Mask=%d, Status=%d",
                        p_PdsmPaInfo->pa_info.pe_config_info.peConfigMask, e_IndStatus, 0);
            z_GetPeCfgInd.status = eQMI_LOC_SUCCESS_V02;
          }
        }
        v_RetVal = locQmiShimSendInd(pz_CltInfo->l_LocClientHandle, &z_IndInfo);
      }
      break;

    default:
      {
        LOC_MSG_ERROR("locQmi_ProcessPaGetInd: Invalid Get Param=%d", e_ParamType, 0, 0);
      }
  }

  return v_RetVal;
}

typedef struct {

  qmiLocStatusEnumT_v02 status;

}qmiLocGeneralIndMsgT_v02;  /* Message */

/*---------------------------------------------------------------------------
@brief
  Function to process QMI_LOC Indication for commands
---------------------------------------------------------------------------*/
boolean locQmi_ProcessInd
(
   loc_client_handle_type       l_ClientHandle,
   uint32                       q_QmiIndMsgId,
   void                        *p_QmiIndMsg,
   qmiLocStatusEnumT_v02        e_QmiIndMsgStatus
   )
{
  locQmiShimIndInfoStructT   z_IndInfo = { 0 };
  locQmiShimCltInfoStructT  *pz_CltInfo = NULL;
  boolean                    v_RetVal = FALSE;
  qmiLocGeneralIndMsgT_v02   z_QmiLocGeneralInd = { 0 };

  LOC_MSG_HIGH("locQmi_ProcessInd: QMI_LOC Msg=0x%04x, ClientHandle=%d, QmiIndMsgStatus=%d",
               q_QmiIndMsgId, l_ClientHandle, e_QmiIndMsgStatus);
  z_QmiLocGeneralInd.status = e_QmiIndMsgStatus;

  pz_CltInfo = (locQmiShimCltInfoStructT *)q_linear_search(&z_LocQmiShim.z_ClientQ, locQmiShimCompareLocApiHandle, (void *)l_ClientHandle);
  if (NULL == pz_CltInfo)
  {
    LOC_MSG_ERROR("locQmi_ProcessInd: Invalid Client Info for Handle=%d", l_ClientHandle, 0, 0);
    return v_RetVal;
  }

  v_RetVal = TRUE;

  switch (q_QmiIndMsgId)
  {
    /* Process of loc_pd messages start here */
    case QMI_LOC_INJECT_POSITION_IND_V02:
      {
        locPd_ModuleDataType* pz_LocPdData = &(loc_middleware_data.z_locPd_Data);

        LOC_MW_ENTER_CRIT_SECTION(&loc_middleware_data.pd_crit_sec);
        pz_LocPdData->z_InjPos.v_InProgress = FALSE;
        LOC_MW_LEAVE_CRIT_SECTION(&loc_middleware_data.pd_crit_sec);

        z_IndInfo.q_Id      = QMI_LOC_INJECT_POSITION_IND_V02;
        z_IndInfo.q_MsgLen   = sizeof(z_QmiLocGeneralInd);
        z_IndInfo.p_Msg      = &z_QmiLocGeneralInd;
        break;
      }

    case QMI_LOC_GET_BEST_AVAILABLE_POSITION_IND_V02:
      {
        if (NULL == p_QmiIndMsg)
        {
          LOC_MSG_ERROR("locQmi_ProcessInd: NULL p_QmiIndMsg for QMI_LOC_GET_BEST_AVAILABLE_POSITION_IND", 0, 0, 0);
          v_RetVal = FALSE;
        }
        else
        {
          z_IndInfo.q_Id               = QMI_LOC_GET_BEST_AVAILABLE_POSITION_IND_V02;
          z_IndInfo.q_MsgLen           = sizeof(qmiLocGetBestAvailablePositionIndMsgT_v02);
          z_IndInfo.p_Msg              = p_QmiIndMsg;
        }
        break;
      }

    case QMI_LOC_GET_AVAILABLE_WWAN_POSITION_IND_V02:
      {
        if (NULL == p_QmiIndMsg)
        {
          LOC_MSG_ERROR("locQmi_ProcessInd: NULL p_QmiIndMsg for QMI_LOC_GET_AVAILABLE_WWAN_POSITION_IND", 0, 0, 0);
          v_RetVal = FALSE;
        }
        else
        {
          z_IndInfo.q_Id               = QMI_LOC_GET_AVAILABLE_WWAN_POSITION_IND_V02;
          z_IndInfo.q_MsgLen           = sizeof(qmiLocGetAvailWwanPositionIndMsgT_v02);
          z_IndInfo.p_Msg              = p_QmiIndMsg;
        }
        break;
      }



    case QMI_LOC_INJECT_GSM_CELL_INFO_IND_V02:
      {
        z_IndInfo.q_Id       = QMI_LOC_INJECT_GSM_CELL_INFO_IND_V02;
        z_IndInfo.q_MsgLen   = sizeof(z_QmiLocGeneralInd);
        z_IndInfo.p_Msg      = &z_QmiLocGeneralInd;
        break;
      }

      /*9x05: Wifi not supported*/

      /* Process of loc_client messages starts here */


    case QMI_LOC_START_REQ_V02:
      {
        boolean b_IndFailure = FALSE;

        if (eQMI_LOC_SUCCESS_V02 == e_QmiIndMsgStatus)
        {
          if (LOC_API_SUCCESS != loc_start_fix(pz_CltInfo->l_LocClientHandle))
          {
            LOC_MSG_ERROR("locQmi_ProcessClntInd: loc_start_fix failed", 0, 0, 0);
            b_IndFailure = TRUE;
          }
        }
        else
        {
          LOC_MSG_ERROR("locQmi_ProcessClntInd: Set Fix Criteria failed, could not start fix", 0, 0, 0);
          b_IndFailure = TRUE;
        }

        if (b_IndFailure)
        {
          /* Manufacture a Position Report with Session Failure */
          qmiLocEventPositionReportIndMsgT_v02   z_PosRptInd = { 0 };

          z_IndInfo.q_Id             = QMI_LOC_EVENT_POSITION_REPORT_IND_V02;
          z_IndInfo.q_MsgLen         = sizeof(z_PosRptInd);
          z_IndInfo.p_Msg            = &z_PosRptInd;
          z_PosRptInd.sessionStatus  = eQMI_LOC_SESS_STATUS_GENERAL_FAILURE_V02;

          locQmiShimSendInd(pz_CltInfo->l_LocClientHandle, &z_IndInfo);
        }

        /* Nothing more to do here. There is no indication that needs to be sent. */
        v_RetVal = FALSE;
        break;
      }

    case QMI_LOC_GET_FIX_CRITERIA_IND_V02:
      {
        if (NULL == p_QmiIndMsg)
        {
          LOC_MSG_ERROR("locQmi_ProcessInd: NULL p_QmiIndMsg for QMI_LOC_GET_FIX_CRITERIA_IND", 0, 0, 0);
          v_RetVal = FALSE;
        }
        else
        {
          z_IndInfo.q_Id          = QMI_LOC_GET_FIX_CRITERIA_IND_V02;
          z_IndInfo.q_MsgLen      = sizeof(qmiLocGetFixCriteriaIndMsgT_v02);
          z_IndInfo.p_Msg         = p_QmiIndMsg;
        }
        break;
      }

    case QMI_LOC_GET_REGISTERED_EVENTS_IND_V02:
      {
        if (NULL == p_QmiIndMsg)
        {
          LOC_MSG_ERROR("locQmi_ProcessInd: NULL p_QmiIndMsg for QMI_LOC_GET_REGISTERED_EVENTS_IND", 0, 0, 0);
          v_RetVal = FALSE;
        }
        else
        {
          z_IndInfo.q_Id          = QMI_LOC_GET_REGISTERED_EVENTS_IND_V02;
          z_IndInfo.q_MsgLen      = sizeof(qmiLocGetRegisteredEventsIndMsgT_v02);
          z_IndInfo.p_Msg         = p_QmiIndMsg;
        }
        break;
      }

      /* Process of XTRA messages starts here */
    case QMI_LOC_INJECT_XTRA_DATA_IND_V02:
      {
        if (NULL == p_QmiIndMsg)
        {
          LOC_MSG_ERROR("locQmi_ProcessInd: NULL p_QmiIndMsg for QMI_LOC_INJECT_XTRA_DATA_IND", 0, 0, 0);
          v_RetVal = FALSE;
        }
        else
        {
          z_IndInfo.q_Id       = QMI_LOC_INJECT_XTRA_DATA_IND_V02;
          z_IndInfo.q_MsgLen   = sizeof(qmiLocInjectXtraDataIndMsgT_v02);
          z_IndInfo.p_Msg      = p_QmiIndMsg;
        }
        break;
      }

    case QMI_LOC_GET_PREDICTED_ORBITS_DATA_SOURCE_IND_V02:
      {
        if (NULL == p_QmiIndMsg)
        {
          LOC_MSG_ERROR("locQmi_ProcessInd: NULL p_QmiIndMsg for QMI_LOC_GET_PREDICTED_ORBITS_DATA_SOURCE_REQ_V02", 0, 0, 0);
          v_RetVal = FALSE;
        }
        else
        {
          z_IndInfo.q_Id       = QMI_LOC_GET_PREDICTED_ORBITS_DATA_SOURCE_IND_V02;
          z_IndInfo.q_MsgLen   = sizeof(qmiLocGetPredictedOrbitsDataSourceIndMsgT_v02);
          z_IndInfo.p_Msg      = p_QmiIndMsg;
        }
        break;
      }

    case QMI_LOC_GET_PREDICTED_ORBITS_DATA_VALIDITY_IND_V02:
      {
        if (NULL == p_QmiIndMsg)
        {
          LOC_MSG_ERROR("locQmi_ProcessInd: NULL p_QmiIndMsg for QMI_LOC_GET_PREDICTED_ORBITS_DATA_SOURCE_REQ_V02", 0, 0, 0);
          v_RetVal = FALSE;
        }
        else
        {
          z_IndInfo.q_Id       = QMI_LOC_GET_PREDICTED_ORBITS_DATA_VALIDITY_IND_V02;
          z_IndInfo.q_MsgLen   = sizeof(qmiLocGetPredictedOrbitsDataValidityIndMsgT_v02);
          z_IndInfo.p_Msg      = p_QmiIndMsg;
        }
        break;
      }

    case QMI_LOC_INJECT_UTC_TIME_IND_V02:
      {
        z_IndInfo.q_Id       = QMI_LOC_INJECT_UTC_TIME_IND_V02;
        z_IndInfo.q_MsgLen   = sizeof(z_QmiLocGeneralInd);
        z_IndInfo.p_Msg      = &z_QmiLocGeneralInd;
        break;
      }

    case QMI_LOC_QUERY_XTRA_INFO_IND_V02:
      {
        if (NULL == p_QmiIndMsg)
        {
          LOC_MSG_ERROR("locQmi_ProcessInd: NULL p_QmiIndMsg for QMI_LOC_QUERY_XTRA_INFO_IND", 0, 0, 0);
          v_RetVal = FALSE;
        }
        else
        {
          z_IndInfo.q_Id       = QMI_LOC_QUERY_XTRA_INFO_IND_V02;
          z_IndInfo.q_MsgLen   = sizeof(qmiLocQueryXtraInfoIndMsgT_v02);
          z_IndInfo.p_Msg      = p_QmiIndMsg;
        }
        break;
      }

      /* Process of loc_ni messages starts here */


      /* Process of loc_conn messages starts here */

      /* Process of loc_qmi_shim messages starts here */
    case QMI_LOC_GET_SERVICE_REVISION_IND_V02:
      {
        if (NULL == p_QmiIndMsg)
        {
          LOC_MSG_ERROR("locQmi_ProcessInd: NULL p_QmiIndMsg for QMI_LOC_GET_SERVICE_REVISION_IND", 0, 0, 0);
          v_RetVal = FALSE;
        }
        else
        {
          z_IndInfo.q_Id       = QMI_LOC_GET_SERVICE_REVISION_IND_V02;
          z_IndInfo.q_MsgLen   = sizeof(qmiLocGetServiceRevisionIndMsgT_v02);
          z_IndInfo.p_Msg      = p_QmiIndMsg;
        }
        break;
      }
    case QMI_LOC_GET_SUPPORTED_FEATURE_IND_V02:
      {
        if (NULL == p_QmiIndMsg)
        {
          LOC_MSG_ERROR("locQmi_ProcessInd: NULL p_QmiIndMsg for QMI_LOC_GET_SUPPORTED_FEATURE_IND", 0, 0, 0);
          v_RetVal = FALSE;
        }
        else
      {
          z_IndInfo.q_Id       = QMI_LOC_GET_SUPPORTED_FEATURE_IND_V02;
          z_IndInfo.q_MsgLen   = sizeof(qmiLocGetSupportedFeatureIndMsgT_v02);
          z_IndInfo.p_Msg      = p_QmiIndMsg;
        }
        break;
      }

      case QMI_LOC_GET_LOCATION_BACKOFF_PERIOD_IND_V02:
      {
        if (NULL == p_QmiIndMsg)
        {
          LOC_MSG_ERROR("locQmi_ProcessInd: NULL p_QmiIndMsg for QMI_LOC_GET_LOCATION_BACKOFF_PERIOD_IND_V02", 0, 0, 0);
          v_RetVal = FALSE;
        }
        else
        {
          z_IndInfo.q_Id         = QMI_LOC_GET_LOCATION_BACKOFF_PERIOD_IND_V02;
          z_IndInfo.q_MsgLen     = sizeof(qmiLocGetLocationBackoffPeriodIndMsgT_v02);
          z_IndInfo.p_Msg      = p_QmiIndMsg;
        }
        break;
      }

    case QMI_LOC_QUERY_GNSS_ENERGY_CONSUMED_IND_V02:
      {
        if (NULL == p_QmiIndMsg)
        {
          LOC_MSG_ERROR("locQmi_ProcessInd: NULL p_QmiIndMsg for QMI_LOC_QUERY_GNSS_ENERGY_CONSUMED_IND", 0, 0, 0);
          v_RetVal = FALSE;
        }
        else
        {
          z_IndInfo.q_Id = QMI_LOC_QUERY_GNSS_ENERGY_CONSUMED_IND_V02;
          z_IndInfo.q_MsgLen = sizeof(qmiLocQueryGNSSEnergyConsumedIndMsgT_v02);
          z_IndInfo.p_Msg = p_QmiIndMsg;
          break;
        }
      }
	 case QMI_LOC_SYSTEM_INFO_IND_V02:
	 {
	    if (NULL == p_QmiIndMsg)
	    {
	       LOC_MSG_ERROR("locQmi_ProcessInd: NULL p_QmiIndMsg for QMI_LOC_SYSTEM_INFO_IND_V02", 0, 0, 0);
           v_RetVal = FALSE;
	    }
		else
		{
		  z_IndInfo.q_Id = QMI_LOC_SYSTEM_INFO_IND_V02;
          z_IndInfo.q_MsgLen = sizeof(qmiLocSystemInfoIndMsgT_v02);
          z_IndInfo.p_Msg = p_QmiIndMsg;
          break;
		}
	 }

    default:
      {
        LOC_MSG_ERROR("locQmi_ProcessInd: Invalid QmiMsgId=0x%04x", q_QmiIndMsgId, 0, 0);
        v_RetVal = FALSE;
        break;
      }
  }

  if (TRUE == v_RetVal)
  {
    v_RetVal = locQmiShimSendInd(pz_CltInfo->l_LocClientHandle, &z_IndInfo);
  }

  return v_RetVal;
}



