
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  Session Manager API

GENERAL DESCRIPTION
  This module makes up the Position Determination Session Manager of the subscriber
  station software.

  The Session Manager API file is used by MGP module to interface with external
  world


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2013 - 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/sm/common/src/sm_api.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
29/04/20   ska     Added changes for asking less proc duration when EPH is valid
10/30/19   nath    Nhz positioning
08/21/19   ska     Added support for NMEA GSV 
05/15/19   nath    Added qualified enablement related changes
04/01/19   nath    Added debug NMEA related code.
10/17/17   rn      Added support for XTRA Integrity
08/10/16   mj      Handle Leap Second Updates from ME
06/06/16   mj      Add GERA support for AP-MP
07/09/15   rh      Keep sm_UpdateCellIDInfo() API for SMART even with CM CID change
07/03/15   jv      Added QZSS Support
06/04/15   wtr     Added initializer (of Unc's to high values) for
                   sm_GnssNavPosition
06/04/15   jyw     Added CPE Timing Control
07/18/14   jyw     Added API for CPE measurements
05/09/14   jv      Galio GNSS Support
06/07/13   rh      Removed NO_FIX report handling (now BestAvailPos)
01/28/13   mj      Update sm_RegisterApiNotifier to include new parm clientId
01/12/13   sk      ECID for LPP API changes
03/14/12   ss      Reset Location Service Changes
02/27/12   rh      Added sm_SendEngineErrorRecoveryReport() API
05/18/11   rc      Lowering priority of some standalone messages from MSG_HIGH to MSG_LOW
02/12/10   vp      Added Sv polynomial function for OEM DRE enablement
06/10/09   gk      Added Get UTC offset from NV
01/12/09   atien   XTRA-T support

09/15/08   gk      Rename the on-demand struct to sm_ from cgps_
04/28/08   gk      On-Demand Changes
08/01/07   rw      Add GPS report time info for XTRA feature
12/01/06   jw      Made changes to support modified struct sm_GpsMeasRptStructType
04/04/06   sk      Initial version

============================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "gps_variation.h"
#include "comdef.h"     /* Definition for basic types and macros */
#include "customer.h"   /* Customer configuration file */

#include "smdbg.h"      /* Interface to SM debug services */
#include "gps_common.h"
#include "sm_api.h"     /* Internal interface to pdsm.c */
#include "aries_os_api.h"
#include "aries_os_globals.h"
#ifndef ARIES_DLF_CONVERT
   #include "tm_api.h"
#endif /* End - !ARIES_DLF_CONVERT */

#include "sm_util.h"
#include "gnss_common.h"
#include "mgp_api.h"
#include "location_common.h"
#include "sm_nv_efs.h"
#include "gps_fs_api.h"

/*===========================================================================
FUNCTION sm_send_robust_message

DESCRIPTION
  Function to send an IPC message to SM threads

RETURN VALUE
 ipc_msg_ptr = If IPC message was send successfully

DEPENDENCIES
  None

=============================================================================*/
/* KGC TODO: change return type for sm_send_message. the returned
   os_IpcMsgType is only being used to determine if a message was
   successfully allocated and sent. it is dangerous to access the message
   after it has been queued for another thread.

   Return type may be changed to bool. Also, in the current implementation,
   only action taken on the return is to emit an F3, which is a repeat of the
   one already being done in sm_send_message.

*/
os_IpcMsgType *sm_send_robust_message
(
uint32 msg_id,
uint32 size,
uint32 thread,
const void *data
)
{
   os_IpcMsgType   *ipc_msg_ptr  = NULL;
   boolean status;
   MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW,"SM_API: Sending msg=%d,size=%d,thread=%d",msg_id,size,thread);

   /* All messages to SM are dynamic, so just allocate non-static buffer.
   */
   ipc_msg_ptr = os_RobustIpcCreate(size,IPC_ALLOCATION_DYNAMIC,thread);

   if(ipc_msg_ptr != NULL)
   {
      /* Copy out the data message portion.
      */
      if(data != NULL)
      {
         memscpy( ipc_msg_ptr->p_Data, size, data, size);
      }
      ipc_msg_ptr->q_MsgId = msg_id;

      /* Check if IPC is able to send the message to thread.
      */
      status    = os_IpcSend(ipc_msg_ptr, thread);

      if(status == FALSE)
      {
         MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"SM_API: Error in sending msg_id=%d to thread=%d",msg_id,thread);
         (void)os_IpcDelete(ipc_msg_ptr);
         ipc_msg_ptr = NULL;
      } /* End status */
      else
      {
         MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_LOW,"SM_API:sending msg_id=%d to thread=%d",msg_id,thread);
      } /* end else status */
   } /* end ipc_msg_ptr */

   return ipc_msg_ptr;
} /* End sm_send_message() */

/*===========================================================================
  sm_ReportGnssQualInd

  Description:

     This function is used by MGP to report GNSS Quality indications to SM.

 Parameters:

   e_GnssQualityInd:  GNSS Quality indication enum

  Return value: 
    void

=============================================================================*/
void sm_ReportGnssQualInd(const gnss_QualIndEnumType e_GnssQualityInd)
{}
/*=============================================================================

FUNCTION
  sm_UpdateCPETimingControl

DESCRIPTION
  This updates SM about the CPE timing control that is read by NV

DEPENDENCIES
  None.

PARAMETERS
  q_CPEControl - CPE Control Value

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_UpdateCPETimingControl(uint32 q_CPEControl)
{}
/*=============================================================================

       Functions

=============================================================================*/

/**
 * This function sends the GNSS Measurements obtained by ME to
 * SM. PS Measurements are sent out periodically for NMEA
 * sentence generation and for diagnostic reasons (periodicity
 * is set by SM). They are also sent when measurement processing
 * is complete in 1x MS-Assisted mode of operation. The
 * measurement status report indicates the reason for sending
 * the measurements.
 * @param e_MeasRptReason
 * @param meas_ptr
 */
void sm_ReportGnssMeasInfo(gnss_MeasRptReasonEnumType    e_MeasRptReason,
                           const gnss_MeasBlkStructType *meas_ptr)
{
  sm_GpsMeasRptStructType z_SmMeas, *sm_meas_ptr = &z_SmMeas;
  os_IpcMsgType   *ipc_msg_ptr  = NULL;
  uint32          msg_id        = 0;
  uint32          size          = 0,u_I;
  if (NULL == meas_ptr)
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_API: Null pointer access in sm_ReportGnssMeasInfo()");
    return;
  }
  MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"SM_API: Meas update for SM");

  /* Copy the GPS Meas Blk into SM meas structure */
  sm_meas_ptr->e_MeasRptReason = e_MeasRptReason;

  sm_meas_ptr->z_MeasBlk = *meas_ptr;	

  msg_id = (uint32)ARIES_MSG_ID_MGP_LM_REPORT_GPS_MEAS;

  size = sizeof(sm_GpsMeasRptStructType);

  /* Send the MSG to SM LM thread
   */
  ipc_msg_ptr = sm_send_message(msg_id, size, (uint32)THREAD_ID_SM_LM, (void *)sm_meas_ptr);

  if(ipc_msg_ptr != NULL)   
  {
     MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"SM_API: IPC msg send to SM, id = %d",ipc_msg_ptr->q_MsgId,0,0);
  }
  return;
} /* end sm_ReportGnssMeasInfo() */
/*=============================================================================

FUNCTION
  sm_ReportGpsEphemeris

DESCRIPTION
  This function sends a GPS ephemeride from PE to SM.

DEPENDENCIES
  None.

PARAMETERSa
  w_Prn    - PRN of ephemeride being returned
  pz_GpsEph - Pointer to the ephemeris information

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportGpsEphemeris(const uint16 w_Prn, const gps_EphStructType *pz_GpsEph)
{}

/*=============================================================================

FUNCTION
  sm_ReportGloEphemeris

DESCRIPTION
  This function sends a GLO ephemeride from PE to SM.

DEPENDENCIES
  None.

PARAMETERSa
  w_SlotId  - Slot ID of ephemeride being returned
  pz_GloEph - Pointer to the ephemeris information

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportGloEphemeris(const uint16 w_SlotId, const glo_EphStructType *pz_GloEph)
{}

/*=============================================================================

FUNCTION
  sm_ReportBdsEphemeris

DESCRIPTION
  This function sends a BDS ephemeride from PE to SM.

DEPENDENCIES
  None.

PARAMETERSa
  w_Sv      - SV ID of ephemeride being returned
  pz_BdsEph - Pointer to the ephemeris information

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportBdsEphemeris(const uint16 w_Sv, const bds_EphStructType *pz_BdsEph)
{}

/*=============================================================================

FUNCTION
  sm_ReportGalEphemeris

DESCRIPTION
  This function sends a GAL ephemeride from PE to SM.

DEPENDENCIES
  None.

PARAMETERSa
  w_Sv      - SV ID of ephemeride being returned
  pz_GalEph - Pointer to the ephemeris information

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportGalEphemeris(const uint16 w_Sv, const gal_EphStructType *pz_GalEph)
{}
/*===========================================================================
  sm_ReportPedAlignAvailInd

  Description:

     This function is used by MGP to report Pedestrian Alignement availibility to SM.

 Parameters:

   v_PedAlignSensorAvailable: whether Pedestrian alignement is available or not

  Return value: 
    void

=============================================================================*/
void sm_ReportPedAlignAvailInd(boolean v_PedAlignSensorAvailable)
{}
/*===========================================================================
  sm_ReportMgpPedStatus

  Description:

     This function is used by MGP to report Pedestrian status to SM.

 Parameters:

   e_PedStatus:  Pedestrian status enum

  Return value: 
    void

=============================================================================*/
void sm_ReportMgpPedStatus(gnss_MgpPedStatusEnumType e_PedStatus)
{}
/*
******************************************************************************
* sm_GloStringPut
*
* Function description:
*
*  This function is called by PE to provide a decoded GLONASS string to PE.
*
* Parameters:
*
*  pz_Str -  input, Pointer to a GLONASS string
*
* Return value:
*
*  void
*
* ******************************************************************************
*/
void sm_GloStringPut(const gnss_GloStringStruct *pz_Str)
{}
/*
******************************************************************************
* sm_SvGalPagePut
*
* Function description:
*
*    This function is called by PE to provide a decoded Galileo Page to PE.
*
* Parameters:
*
*  p_SvSframe -  Pointer to SV GAL Page structure
*
* Return value:
*
*  void
*
* ******************************************************************************
*/
void sm_SvGalPagePut(const gnss_GalPageStruct *p_SvGalPage)
{}
/*
******************************************************************************
* sm_SvSframePut
*
* Function description:
*
*    This function is called by PE to provide a decoded subframe to PE.
*
* Parameters:
*
*  p_SvSframe -  Pointer to SV subframe structure
*
* Return value:
*
*  void
*
* ******************************************************************************
*/
void sm_SvSubframePut(const gnss_SvSubframeStruct *p_SvSubframe)
{}

/*===========================================================================
  sm_ReportPedDevContext

  Description:

     This function is used by MGP to report Pedestrian Device Context to SM.

 Parameters:

   e_PedDevCtx:  Pedestrian Device Context enum

  Return value: 
    void

=============================================================================*/
void sm_ReportPedDevContext(const slimPedDevCxtTypeEnumT e_PedDevCtx)
{}
/*=============================================================================

FUNCTION
  sm_ReportGnssLEStatus

DESCRIPTION
  This function is to send GNSS LE fix reporting status to SM

DEPENDENCIES
  None.

PARAMETERS
  p_GnssLERptStatus -Pointer to GNSS LE Reporting Status struct

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportGnssLEStatus(sm_GnssLERptStatusStructType *p_GnssLERptStatus)
{}
/*=============================================================================

FUNCTION
  sm_ReportSensorLEStatus

DESCRIPTION
  This function is to report Sensor LE State Change

DEPENDENCIES
  None.

PARAMETERS
  p_SensorLEStatus -Pointer to Sensor LE Status struct

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportSensorLEStatus(const sm_SensorLEStatusStructType *const p_SensorLEStatus)
{}
/*=============================================================================

FUNCTION
  sm_ReportSensorLERelDisplacement

DESCRIPTION
  This function is to report Sensor Relative Displacement from Sensor LE

DEPENDENCIES
  None.

PARAMETERS
  p_RelDisp -Pointer to Relative Displacement Struct.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportSensorLERelDisplacement(const sm_SensorLERelDispStructType *const p_RelDisp)
{}

/*===========================================================================
  sm_loc_lte_ecid_recv_measurements

  Description:

     This function is used by LOC_LTE_ECID module to send the ECID measurements
 Parameters:
    Reason: stop reason

  Return value: 
    boolean: SUCCESS or FAIL
=============================================================================*/
boolean sm_loc_lte_ecid_recv_measurements(
   gnss_lpp_ecid_meas_info_type *p_ecid_measurement_info)
     {
return FALSE;
   }

/**
* @brief  ME calls this function to request SA (assistance) data
*         from the network
  
* @return      void
  @sideeffects None
  @see


*/
void sm_RequestSAData(void)
{}
/**
* @brief  PE indicates to SM the status of current assistance
*         data and wish-list using this API
  
  @return    
  @sideeffects None
  @see 

*/
void sm_ReportGnssAssistDataStatus(const sm_GnssAssistDataStatusStructType *p_AssistDataInfo)
{}
//TODO  [an][del]  delete till here
/*-------------------------------------------------------------------------------*/

/*===========================================================================
FUNCTION sm_send_message

DESCRIPTION
  Function to send an IPC message to SM threads

RETURN VALUE
 ipc_msg_ptr = If IPC message was send successfully

DEPENDENCIES
  None

=============================================================================*/
/* KGC TODO: change return type for sm_send_message. the returned
   os_IpcMsgType is only being used to determine if a message was
   successfully allocated and sent. it is dangerous to access the message
   after it has been queued for another thread.

   Return type may be changed to bool. Also, in the current implementation,
   only action taken on the return is to emit an F3, which is a repeat of the
   one already being done in sm_send_message.

*/
os_IpcMsgType *sm_send_message
(
uint32 msg_id,
uint32 size,
uint32 thread,
const void *data
)
{
   os_IpcMsgType   *ipc_msg_ptr  = NULL;
   boolean status;
   MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW,"SM_API: Sending msg=%d,size=%d,thread=%d",msg_id,size,thread);

   /* All messages to SM are dynamic, so just allocate non-static buffer.
   */
   ipc_msg_ptr = os_IpcCreate(size,IPC_ALLOCATION_DYNAMIC,thread);

   if(ipc_msg_ptr != NULL)
   {
      /* Copy out the data message portion.
      */
      if(data != NULL)
      {
         memscpy( ipc_msg_ptr->p_Data, size, data, size);
      }
      ipc_msg_ptr->q_MsgId = msg_id;

      /* Check if IPC is able to send the message to thread.
      */
      status    = os_IpcSend(ipc_msg_ptr, thread);

      if(status == FALSE)
      {
         MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"SM_API: Error in sending msg_id=%d to thread=%d",msg_id,thread,0);
         (void)os_IpcDelete(ipc_msg_ptr);
         ipc_msg_ptr = NULL;
      } /* End status */
      else
      {
         MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW,"SM_API:sending msg_id=%d to thread=%d",msg_id,thread,0);
      } /* end else status */
   } /* end ipc_msg_ptr */

   return ipc_msg_ptr;
} /* End sm_send_message() */
/**
  @brief   This function sends the GPS Measurements obtained by ME to SM.

  This function sends the Receiver State change event report to
  SM

  @return
  @sideeffects None
  @see
*/
void sm_ReportRcvrStateChange(
                             uint32 q_GpsRtcMs,
                             /**< GNSS RTC msec at which this report was generated.  */
                             gps_RcvrStateEnumType e_CurRcvrState,
                             /**< new receiver state */
                             const gnss_RcvrSubStateStructType* pz_CurRcvrSubState
                             /**< new receiver sub-state */
                             )
{
   os_IpcMsgType   *ipc_msg_ptr  = NULL;
   uint32          msg_id        = 0;
   uint32          size          = 0;
   sm_RcvrStateChangeStructType  state;

   MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW,"SM_API: MGP=>SM state change RTC=%d, state=%d",q_GpsRtcMs,e_CurRcvrState,0);

   if(NULL == pz_CurRcvrSubState)
   {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"Null Rcvr Sub State passed",0,0,0);
      return;
   }

   /* Depending on meas report, set the corsp msg id for sm.
   */
   msg_id      = (uint32)ARIES_MSG_ID_MGP_LM_REPORT_GENERAL_STATUS;
   size        = sizeof(sm_RcvrStateChangeStructType);

   /* Init the state and RTC value in local variable for IPC send
   */
   state.e_CurrentRcvrState = e_CurRcvrState;
   state.z_currentRcvrSubState = *(pz_CurRcvrSubState);
   state.q_GpsRtcMs = q_GpsRtcMs;
   /* Send the MSG to SM LM thread
   */
   ipc_msg_ptr = sm_send_message(msg_id, size, (uint32)THREAD_ID_SM_LM, (void *)&state);

   if(ipc_msg_ptr != NULL)
   {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW,"SM_API: IPC msg send to SM, id = %d",ipc_msg_ptr->q_MsgId,0,0);
   }
} /* End sm_ReportRcvrStateChange() */

/**
  @brief  This function sends the GPS Fix report calculated by
          PE to SM. GPS FIX/NO-FIX reports are sent periodically
          once MGP is turned ON

  @return
  @sideeffects None
  @see

*/
void sm_ReportFix(const sm_FixReportStructType *p_GnssFixRpt)
/**< Pointer to the structure containing the GPS Fix data */
{

   os_IpcMsgType   *ipc_msg_ptr  = NULL;
   uint32          msg_id        = 0;
   uint32          size          = 0;

   MSG(MSG_SSID_GPSSM, MSG_LEGACY_LOW,"SM_API: Fix report for SM");
   if (NULL == p_GnssFixRpt)
   {
     MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"SM_API: Null pointer access in sm_ReportGnssFixInfo()");
     return;
   }


   /* Depending on meas report, set the corsp msg id for sm.
   */
   msg_id      = (uint32)ARIES_MSG_ID_MGP_LM_REPORT_FIX;
   size        = sizeof(*p_GnssFixRpt); /* sizeof(sm_FixReportStructType) */

   /* Send the MSG to SM LM thread
   */
   /* If fix is not valid or it is full GNSS Fix use legacy IPC create API */
   if ((TRUE == p_GnssFixRpt->z_FixInfo.z_NavPos.z_PosFlags.b_NHzGnssUsedFixRpt) || 
       (TRUE == p_GnssFixRpt->z_FixInfo.z_NavPos.z_PosFlags.b_PVAPropagationUsedFixRpt) )
   {     
     ipc_msg_ptr = sm_send_robust_message(msg_id, size, (uint32)THREAD_ID_SM_LM, 
       (void *)p_GnssFixRpt);
   }
   else
   {
     ipc_msg_ptr = sm_send_message(msg_id, size, (uint32)THREAD_ID_SM_LM, (void *)p_GnssFixRpt);
   }
   if(ipc_msg_ptr != NULL)
   {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW,"SM_API: IPC msg send to SM, id = %d",ipc_msg_ptr->q_MsgId,0,0);
   }

} /* end sm_ReportFix */


/*=============================================================================

FUNCTION
  sm_ReportUtcInfo

DESCRIPTION
  This function sends the UTC data decoded by PE off the air to SM.

DEPENDENCIES
  None.

PARAMETERS
  p_UtcInfo - Pointer to the structure containing the UTC info

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportUtcInfo(const sm_UtcInfoRptStructType *p_UtcInfo)
{
   os_IpcMsgType   *ipc_msg_ptr  = NULL;
   uint32          msg_id        = 0;
   uint32          size          = 0;

   MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"SM_API: UTC data update...",0,0,0);

   /* Depending on meas report, set the corsp msg id for sm.
   */
   msg_id      = (uint32)ARIES_MSG_ID_MGP_LM_REPORT_UTC_INFO;
   size        = sizeof(sm_UtcInfoRptStructType);

   /* Send the MSG to SM LM thread
   */
   ipc_msg_ptr = sm_send_message(msg_id, size, (uint32)THREAD_ID_SM_LM, (void *)p_UtcInfo);

   if(ipc_msg_ptr != NULL)
   {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"SM_API: IPC msg send to SM, id = %d",ipc_msg_ptr->q_MsgId,0,0);
   }
}

/*=============================================================================

FUNCTION
  sm_ReportEvent

DESCRIPTION
  This function sends an event notification from PE to SM.

DEPENDENCIES
  None.

PARAMETERS
  p_ReportEvent - Report event parameters.
  Contains the following fields:
     e_EventType - type of event
     q_EventData - data related to the event
     q_EventTime - time stamp related to event(GPS RTC)

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportEvent(const sm_ReportEventStructType* p_ReportEvent)
{
   os_IpcMsgType   *ipc_msg_ptr  = NULL;
   uint32          msg_id        = 0;
   uint32          size          = 0;

   MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW,"SM_API: Report event to SM...",0,0,0);

   /* Depending on meas report, set the corsp msg id for sm.
   */
   msg_id      = (uint32)ARIES_MSG_ID_MGP_LM_REPORT_EVENT;
   size        = sizeof(sm_ReportEventStructType);

   /* Send the MSG to SM LM thread
   */
   ipc_msg_ptr = sm_send_message(msg_id, size, (uint32)THREAD_ID_SM_LM, (void *)p_ReportEvent);

   if(ipc_msg_ptr != NULL)
   {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW,"SM_API: IPC msg send to SM, id = %d",ipc_msg_ptr->q_MsgId,0,0);
   }

}

/*=============================================================================

FUNCTION
  sm_ReportRfInfo

DESCRIPTION
  This function sends RF Info data from PGI to SM.

DEPENDENCIES
  None.

PARAMETERS
  p_ReportRfInfo - Pointer to the structure containing RF Info data

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportRfInfo(const sm_ReportRfInfoStructType * p_ReportRfInfo)
{}

/*=============================================================================

FUNCTION
  sm_ReportWBIQInfo

DESCRIPTION
  This function sends Wide Band IQ Info data from MC to SM.

DEPENDENCIES
  None.

PARAMETERS
  p_ReportWBIQInfo - Pointer to the structure containing WBIQ params

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportWBIQInfo (const sm_ReportWBIQInfoStructType * p_ReportWBIQInfo)
{
  os_IpcMsgType   *ipc_msg_ptr  = NULL;
  uint32          msg_id        = 0;
  uint32          size          = 0;

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW,"SM_API: Report WBIQ Info to SM...",0,0,0);

  msg_id      = (uint32)ARIES_MSG_ID_MGP_LM_REPORT_WB_IQ_PARAMS;
  size        = sizeof(sm_ReportWBIQInfoStructType);

  /* Send the MSG to SM LM thread
  */
  ipc_msg_ptr = sm_send_message(msg_id, size, (uint32)THREAD_ID_SM_LM, (void *)p_ReportWBIQInfo);

  if (ipc_msg_ptr != NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW,"SM_API: IPC msg send to SM, id = %d",ipc_msg_ptr->q_MsgId,0,0);
  }

}

/*=============================================================================

FUNCTION
  sm_ReportXOOffsetInfo

DESCRIPTION
  This function sends XO offset data to SM

DEPENDENCIES
  None.

PARAMETERS

  l_RefOffsetPpb - Ref osc offset in PPB
  l_ClkFreqBiasPpb - Clock freq bias in PPB
  l_ClkFreqBiasUncPpb - Clock freq bias uncertainty in PPB

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportXOOffsetInfo (int32 l_RefOffsetPpb, 
                            int32 l_ClkFreqBiasPpb, 
                            int32 l_ClkFreqBiasUncPpb)
{
  os_IpcMsgType   *p_IpcMsg  = NULL;
  sm_ReportXOOffsetInfoStructType z_ReportXOOffsetInfo = {0};

  z_ReportXOOffsetInfo.l_RefOffsetPpb = l_RefOffsetPpb;
  z_ReportXOOffsetInfo.l_ClkFreqBiasPpb = l_ClkFreqBiasPpb;
  z_ReportXOOffsetInfo.l_ClkFreqBiasUncPpb = l_ClkFreqBiasUncPpb;
  z_ReportXOOffsetInfo.v_IsDataValid = TRUE;

  /* Send the MSG to SM LM thread */
  p_IpcMsg = sm_send_message((uint32) ARIES_MSG_ID_MGP_LM_XO_OFFSET_INFO, 
                             sizeof(z_ReportXOOffsetInfo), 
                                (uint32)THREAD_ID_SM_LM, 
                             (void *) &z_ReportXOOffsetInfo);

  if (p_IpcMsg == NULL)
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "sm_ReportXOOffsetInfo: IPC send failed");
  }
}

/*=============================================================================

FUNCTION
  sm_NavConfigChangeResp

DESCRIPTION
  This function sends MGP's response to the Nav config change diag command.

DEPENDENCIES
  None.

PARAMETERS
  u_CurrGnssConfig - Current GNSS configuration.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_NavConfigChangeResp (const uint8 u_CurrGnssConfig)
{
  os_IpcMsgType   *p_IpcMsg  = NULL;

  /* Send the MSG to SM LM thread */
  p_IpcMsg = sm_send_message((uint32)ARIES_MSG_ID_MGP_LM_NAV_CONFIG_CHANGE_RESP,
                             sizeof(uint8), 
                             (uint32)THREAD_ID_SM_LM, (void *)&u_CurrGnssConfig);

  if (p_IpcMsg != NULL)
  {
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_LOW, "SM_API: IPC msg sent to SM, id = %d", p_IpcMsg->q_MsgId);
  }
}


/*=============================================================================

FUNCTION
  sm_ReportNBIQInfo

DESCRIPTION
  This function sends Narrow Band IQ Info data from MC to SM.

DEPENDENCIES
  None.

PARAMETERS
  p_ReportNBIQInfo - Pointer to the structure containing NBIQ raw IQ data

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportNBIQInfo (const sm_ReportNBIQInfoStructType * p_ReportNBIQInfo)
{
  os_IpcMsgType   *ipc_msg_ptr  = NULL;
  uint32          msg_id        = 0;
  uint32          size          = 0;

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW,"SM_API: Report NBIQ Info to SM...",0,0,0);

  msg_id      = (uint32)SM_MSG_ID_MGP_LM_REPORT_NBIQ_PARAMS;
  size        = sizeof(sm_ReportNBIQInfoStructType);

  /* Send the MSG to SM LM thread
  */
  ipc_msg_ptr = sm_send_message(msg_id, size, (uint32)THREAD_ID_SM_LM, (void *)p_ReportNBIQInfo);

  if (ipc_msg_ptr != NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW,"SM_API: IPC msg send to SM, id = %d",ipc_msg_ptr->q_MsgId,0,0);
  }

}

/*=============================================================================

FUNCTION
  sm_ReportGNSSRfStatusInfo

DESCRIPTION
  This function sends GNSS RF status Info data from MC to SM.

DEPENDENCIES
  None.

PARAMETERS
  p_ReportGNSSRfInfo - Pointer to the structure containing GNSS RF status
  information

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportGNSSRfStatusInfo (const sm_ReportGNSSRfStatusInfoStructType * p_ReportGNSSRfInfo)
{}

//TBD
// Aries41 merge. Following 2 functions needs to be removed once mc_msg is integrated.
/*void tm_api_update_pos_punc_tunc ( const sm_OnDemandPosPuncTuncStruct* const p_PosInfo )
{
}*/


/*=============================================================================

FUNCTION
  sm_GetGpsUtcOffset

DESCRIPTION
  This function gets UTC offset from SM.

DEPENDENCIES
  None.
PARAMETERS
  p_UtcOffset - Pointer to the UTC offset value
RETURN VALUE
  None.
SIDE EFFECTS
  None.

=============================================================================*/
void  sm_GetGpsUtcOffset(uint8 *p_UtcOffset)
{
  tm_api_report_utc_offset(p_UtcOffset);
}

/**
  @brief  Delivers estimate of current time to SM. PE calls this
          API in response to a request from SM via
          mgp_ReqGnssTime (to be defined by MGP).

  @return
  @sideeffects None
  @see

*/
void sm_ReportGnssTime(const sm_ReportGnssTimeStructType *p_ReportGnssTime)/**< gnss time struct */
{
   os_IpcMsgType   *ipc_msg_ptr  = NULL;
   uint32          msg_id        = 0;
   uint32          size          = 0;
   sm_ReportGpsTimeStructType gpsTime = {0};

   /* we translate to older gps struct so that internal code doesn't have to change just yet.
   Also all layers below SM API use only GPS time*/
   if(sm_translate_gnss_time_to_gps_time(&gpsTime, p_ReportGnssTime))
   {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW,"sm_ReportGnssInfo: Report GPS time to SM...",0,0,0);

      msg_id      = (uint32)ARIES_MSG_ID_MGP_LM_REPORT_GPS_TIME;
      size        = sizeof(gpsTime); /* sizeof(sm_ReportGpsTimeStructType) */

      /* Send the MSG to SM LM thread
      */
      ipc_msg_ptr = sm_send_message(msg_id, size, (uint32)THREAD_ID_SM_LM, (void *)&gpsTime);

      if(ipc_msg_ptr != NULL)
      {
         MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW,"SM_API: IPC msg send to SM, id = %d",ipc_msg_ptr->q_MsgId,0,0);
      }
   }
   else
   {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"SM_API: failed when translating gnss time to internal gps time ",0,0,0);
   }
} /* end sm_ReportGnssTime */

/*=============================================================================
FUNCTION
  sm_ReportPrescDwellResult

DESCRIPTION
  This function gets called when a presc dwell log is available.

DEPENDENCIES
  None.

PARAMETERS
  p_PrescDwellResult - Pointer to the structure containing presc dwell result

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportPrescDwellResult( const sm_PrescDwellLogType *p_PrescDwellResult )
{
  /* Display a few values for quick F3 confirmation */
  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW,"SM_API: Presc Dwell result Flags %u CNo %u PostD %u",
            p_PrescDwellResult->z_PrescDwellResult.u_Flags,
            p_PrescDwellResult->z_PrescDwellResult.w_CnoDb,
            p_PrescDwellResult->z_PrescDwellResult.w_PostDetN);
}

/*=============================================================================

FUNCTION
  sm_ReportDebugNMEA

DESCRIPTION
  Accepts formatted proprietary NMEA string for debug purpose.
  NMEA string is sent as IPC to the LM task by this API

DEPENDENCIES
  None.

PARAMETERS
  sm_DebugNMEA

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportDebugNMEA(sm_DebugNMEA* pDebugNMEA)
{/*output via port is not supported anymore*/}

/*=============================================================================

FUNCTION
  sm_RegisterApiNotifierCb

DESCRIPTION
  Called by potential clients to register their callbacks. The callbacks are
  used to notify the clients that the API/Engine is now ready to accept clients

  If the API/Engine is ready to accept clients, the callback will be called
  inline.  The caller should be ready to handle this and queue an IPC to itself.

DEPENDENCIES
  None.

PARAMETERS
  pNotifierCb - The callback of the client to be registered
  clientThreadId - ThreadId used to index the cb into an array

RETURN VALUE
  TRUE:  registration has succeeded, or if Engine is ready the callback is
         called immediately;
  FALSE:  registration failed

SIDE EFFECTS
  None.

=============================================================================*/
boolean sm_RegisterApiNotifier(sm_ApiInitNotifierCb pNotifierCb, os_ThreadIdEnumType clientThreadId)
{
  return FALSE;
#if 0
  return  tm_api_reg_task_init_notifier(pNotifierCb, clientThreadId);
#endif
}


/*=============================================================================

FUNCTION
  sm_ReportXtraInjDone

DESCRIPTION
  Indication from CD that xtra injection completed successfully. 

DEPENDENCIES
  None.

PARAMETERS
   q_AcceptedMask: The mask to indicate the updated constellations.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportXtraInjDone( gnss_XtraInjMaskType q_AcceptedMask )
{
  os_IpcMsgType   *ipc_msg_ptr  = NULL;
  uint32          msg_id        = 0;
  uint32          size          = 0;

  MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_LOW,"sm_ReportXtraInjDone q_AcceptedMask:0x%x",q_AcceptedMask);

  msg_id = (uint32)SM_MSG_ID_MGP_LM_REPORT_XTRA_INJ_DONE;
  size   = sizeof(gnss_XtraInjMaskType);

  /* Send the MSG to SM LM thread */
  ipc_msg_ptr = sm_send_message(msg_id, size, 
    (uint32)THREAD_ID_SM_LM, (void*)&q_AcceptedMask);

  if(ipc_msg_ptr != NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW,"SM_API: IPC msg send to SM, id = %d",ipc_msg_ptr->q_MsgId,0,0);
  }
}

//Need to replace sm_ReportXtraInjDone with sm_ReportInjDone. MGP making calls
//to sm_ReportXtraInjDone.
#if 1 /* APIs called from PE */
/*=============================================================================

FUNCTION
  sm_ReportInjDone

DESCRIPTION
  Indication from CD that xtra or xtra integrity injection completed 
  successfully. 

DEPENDENCIES
  None.

PARAMETERS
   q_AcceptedMask: The mask to indicate the updated constellations.
   uint8:          0 - Xtra or 1 - Xtra Integrity.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportInjDone( gnss_XtraInjMaskType q_AcceptedMask, 
                       uint8 u_XtraInjectType )
{
  os_IpcMsgType        *pz_ipc_msg = NULL;
  gnss_XtraInjectType  z_XtraInject = {0};
  uint32               q_msg_id = 0;
  uint32               q_size = 0;
  void                 *a_ptr = NULL;

  MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED,
        "sm_ReportInjDone q_AcceptedMask:0x%x, u_XtraInjectType:%u",
        q_AcceptedMask,u_XtraInjectType);

  /* Fill structure */
  z_XtraInject.q_AcceptMask = q_AcceptedMask;
  z_XtraInject.u_XtraInjType = u_XtraInjectType;

  if (INJECTION_TYPE_XTRA == u_XtraInjectType)
  {
    q_msg_id = (uint32)SM_MSG_ID_MGP_LM_REPORT_XTRA_INJ_DONE;
    q_size = sizeof(q_AcceptedMask);
    a_ptr = &(z_XtraInject.q_AcceptMask);
  }
  else if (INJECTION_TYPE_XTRA_INTEGRITY == u_XtraInjectType)
  {
    q_msg_id = (uint32)SM_MSG_ID_MGP_LM_REPORT_XTRA_INTEGRITY_INJ_DONE;
    q_size = sizeof(gnss_XtraInjectType);
    a_ptr = &z_XtraInject;
  }
  else 
  {
     MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED,
        "sm_ReportInjDone unknown u_XtraInjectType:%u IPC not sent",
         u_XtraInjectType);
     return;
  }

  /* Send the MSG to SM LM thread */
  pz_ipc_msg = sm_send_message(q_msg_id, q_size, 
    (uint32)THREAD_ID_SM_LM, a_ptr);

  if(pz_ipc_msg != NULL)
  {
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_LOW,"SM_API: IPC msg send to SM, id = %d",
          pz_ipc_msg->q_MsgId);
  }
}
#endif  /* #if 0 */

/*=============================================================================

FUNCTION
  sm_SendEngineErrorRecoveryReport

DESCRIPTION
  ME will call this API to send engine error recovery report to SM so it'll be
  forwarded to QMI

DEPENDENCIES
  None.

PARAMETERS
 None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_SendEngineErrorRecoveryReport( gnss_engine_err_recovery_rpt *pErrReport )
{
  tm_api_send_engine_error_recovery_report(pErrReport);
}


/*=============================================================================

FUNCTION
  sm_DpoStatusReport

DESCRIPTION
  This function sends a GLO ephemeride from PE to SM.

DEPENDENCIES
  None.

PARAMETERS
  Information about DPO disengagement from MGP to SM .

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_DpoStatusReport( sm_DpoStatusReportStructType *p_DpoStatusReport )
{
  // Send the DPO status report to TM using IPC .
  tm_api_send_dpo_status_report(p_DpoStatusReport);
}


/*===========================================================================
  sm_InitGnssNavPosition

  Description:

     Initalizes a sm_GnssNavPosition safely

 Parameters:

   pz_Pos:  pointer to the struct to initialize

  Return value:
    boolean: true if successfully initialized

=============================================================================*/
boolean sm_InitGnssNavPosition(sm_NavPositionStructType * pz_Pos)
{
  if (NULL == pz_Pos)
  {
    return FALSE;
  }

  // zero most things - including the valid flags
  memset(pz_Pos, 0, sizeof(*pz_Pos));

  // but set error estiamtes, aka uncertainties, high

  // position
  pz_Pos->f_ErrorEllipse[1] = LOC_UNKNOWN_UNC;
  pz_Pos->f_ErrorEllipse[2] = LOC_UNKNOWN_UNC;
  pz_Pos->f_Punc3dMeters = LOC_UNKNOWN_UNC;
  pz_Pos->f_PuncLatLonMeters[0] = LOC_UNKNOWN_UNC;
  pz_Pos->f_PuncLatLonMeters[1] = LOC_UNKNOWN_UNC;

  // altitude
  pz_Pos->f_FilteredAltUnc = LOC_ALT_MAX_UNC_METERS;
  pz_Pos->f_PuncVertMeters = LOC_ALT_MAX_UNC_METERS;

  // speed
  pz_Pos->f_Vunc3dMps = LOC_MAX_SIGMA_MPS;
  pz_Pos->f_VuncEastNorthMps[0] = LOC_MAX_SIGMA_MPS;
  pz_Pos->f_VuncEastNorthMps[1] = LOC_MAX_SIGMA_MPS;
  pz_Pos->f_VuncVertMps = LOC_MAX_SIGMA_MPS;

  // heading
  pz_Pos->f_GnssHeadingUncRad = (FLT)C_PI;
  pz_Pos->f_HeadingUncRad     = (FLT)C_PI;

  // clock
  pz_Pos->f_ClockBiasUncMs       = (FLT)C_MAX_CLK_TIME_UNC;

  pz_Pos->f_ClockDriftRateUncMps = (FLT)C_MAX_CLK_FREQ_UNC;

  // times
  pz_Pos->w_GpsWeek = C_GPS_WEEK_UNKNOWN;
  pz_Pos->u_GloNumFourYear = C_GLO_FOUR_YEAR_UNKNOWN;
  pz_Pos->w_GloNumDaysInFourYear = C_GLO_DAY_UNKNOWN;

  return TRUE;
};
#if 0
/*===========================================================================
  sm_ReportMgpLppmStatus

  Description:

     This function is used by MGP to report LPPM status to SM.

 Parameters:

   pz_LppmStatus:  pointer to the LPPM status struct

  Return value:
    void

=============================================================================*/
void sm_ReportMgpLppmStatus(const sm_LppmStatusStructType *pz_LppmStatus)
{
  tm_handle_lppm_status(pz_LppmStatus);
}
#endif

/*=============================================================================

FUNCTION
  sm_ReportLeapSecondInfo

DESCRIPTION
  This function sends a Leap Second Info from ME to SM.

DEPENDENCIES
  None.

PARAMETERS
  mgp_LeapSecondInfoStructType - Pointer to leap second info

RETURN VALUE
  None

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportLeapSecondInfo(const mgp_LeapSecondInfoStructType *pz_LeapSecInfo)
{
  tm_api_leap_second_report(pz_LeapSecInfo);
}

/*=============================================================================

FUNCTION
  sm_ReportNMEAForDebug

DESCRIPTION
  Accepts formatted proprietary NMEA string for debug purpose.
  NMEA string is sent as IPC to the TM task by this API, for forwarding to HLOS
  for debugging purpose.

DEPENDENCIES
  None.

PARAMETERS
  sm_DebugNMEA

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportNMEAForDebug(sm_DebugNMEA* pDebugNMEA)
{
  os_IpcMsgType   *ipc_msg_ptr  = NULL;
  uint32          msg_id        = 0;
  uint32          size          = 0;
  MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"sm_ReportNMEAForDebug()");

  msg_id = (uint32)TM_CORE_MSG_ID_NMEA_DEBUG_INFO;
  size   = sizeof(sm_DebugNMEA);

  /* Send the MSG to SM TM thread */
  ipc_msg_ptr = sm_send_message(msg_id, size,
    (uint32)THREAD_ID_SM_TM, (void *)pDebugNMEA);

  if(ipc_msg_ptr != NULL)
  {
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_LOW,"SM_API: IPC msg send to SM_TM, id = %d",ipc_msg_ptr->q_MsgId);
  }

}

#ifdef FEATURE_LOCTECH_NHZ
/*===========================================================================
  sm_NhzConfigPut

  Description:

     Get the NHz config from SM

 Parameters:

   pq_NhzConfig:  pointer to the config

  Return value: 
    None

=============================================================================*/
void sm_NhzConfigPut(uint32 * pq_NhzConfig)
{
  if(NULL == pq_NhzConfig)
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"Null pointer access in sm_NhzConfigPut()");
    return;
  }

  if (!sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_NHZ_CONFIG, sizeof(uint32), pq_NhzConfig) )
  {
    *pq_NhzConfig = GNSS_NV_EFS_SM_NHZ_CONFIG_DEFAULT;
  }
}

/*===========================================================================
  sm_NhzDefaultConfigPut

  Description:  Get the NHz default config from SM
  
  Parameters :  None

  Return value: Default config

=============================================================================*/
uint32 sm_NhzDefaultConfigPut()
{
  return(GNSS_NV_EFS_SM_NHZ_CONFIG_DEFAULT);
}
#endif /* FEATURE_LOCTECH_NHZ */


/*=============================================================================

FUNCTION
  sm_ReportGpsASBit

DESCRIPTION
  This function is used to send AS Bit value from GPS SV decode.

DEPENDENCIES
  None.

PARAMETERS
  cu_ASBitSv - AS bit indicates simulated GPS. Value 1 means AS bit was decoded to be 0 (which would
  indicate from GPS Simulator) in the last subframe.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportGpsASBit( uint8 const cu_ASBitSv )
{
  os_IpcMsgType   *ipc_msg_ptr  = NULL;
  uint32          msg_id        = 0;
  uint32          size          = 0;

  MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_LOW, "sm_ReportGpsASBit %u", cu_ASBitSv );

  msg_id = (uint32)TM_CORE_MSG_ID_GPS_AS_BIT;
  size   = sizeof(cu_ASBitSv);

  /* Send the MSG to SM TM thread */
  ipc_msg_ptr = sm_send_message( msg_id, size, (uint32)THREAD_ID_SM_TM, (void *)&cu_ASBitSv );

  if(ipc_msg_ptr != NULL)
  {
    MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_LOW, "SM_API: IPC msg send to SM, id = %d", ipc_msg_ptr->q_MsgId );
  }
}

/*=============================================================================
 FUNCTION
  sm_GpsFsTaskNotification

DESCRIPTION
  This function is a callback function used to report back EFS operation status 
  to 

DEPENDENCIES
  None.

PARAMETERS
  q_FileOperationResult - EFS operation status code 
  u_PathFilename[] - File name 
  u_FileOp - File operation 
RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
void sm_GpsFsTaskNotification(uint32 q_FileOperationResult, 
                              const uint8 u_PathFilename[], 
                              uint8 u_FileOp)
{
  os_IpcMsgType   *ipc_msg_ptr  = NULL;
  sm_GpsFsTaskNotificationDataType  z_NotificationData = {0};
  uint32 q_msg_id = 0;
  uint32 q_size = 0;
  uint16 w_pathfilelen = 0;
  char u_string[256];

  memset(u_string, 0, sizeof(u_string));

  /* Set filename length */
  w_pathfilelen = strlen((char *)&u_PathFilename[0]);
  /* Check for valid input parameters */
  if ( (u_FileOp > GPS_FS_DELETE_FILE_LIST) || 
       ((w_pathfilelen == 0) || (w_pathfilelen >= GPS_FS_MAX_PATH_LEN)) )
  {
    MSG_2( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, 
           "sm_GpsFsTaskNotification Invalid Params: FileOp:%u, PathFileLen:%u",
           u_FileOp, w_pathfilelen);
    return;
  }


  (void) snprintf(u_string, sizeof(u_string), "sm_GpsFsTaskNotification FileOp:%u OpResult:%u PathFile:%s PathFileLen:%u",
                      u_FileOp, (unsigned int)q_FileOperationResult, (char *)u_PathFilename, w_pathfilelen);
   MSG_SPRINTF_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "%s", u_string);

  q_msg_id = (uint32)TM_CORE_MSG_ID_XTRA_EFS_GPS_FS_TASK_DONE;
  q_size   = sizeof(z_NotificationData);
  z_NotificationData.u_FileOp = u_FileOp;
  memscpy((void *)&z_NotificationData.u_Filename[0],w_pathfilelen,
          (const void *)&u_PathFilename[0],w_pathfilelen);
  z_NotificationData.q_EfsStatus = q_FileOperationResult;

  /* Send the MSG to SM TM thread */
  ipc_msg_ptr = sm_send_message( q_msg_id, q_size, (uint32)THREAD_ID_SM_TM, (void *)&z_NotificationData );

  if(ipc_msg_ptr != NULL)
  {
    MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_LOW, "SM_API: IPC msg send to SM, id = %d", ipc_msg_ptr->q_MsgId );
  }
}
/*=======================================​======================================
FUNCTION     sm_GetXtraConstelType

DESCRIPTION   This function used to get the constellation type for which the XTRA download  
              is needed. XTRA shall be requested for GPS and the other constellation 
              returned by this API. This API should only be used for XTRA.

DEPENDENCIES None

PARAMETERS   pe_ConstelType: The constellation type for which XTRA is needed.
             This value shall be other than GPS . 

RETURN VALUE TRUE if the constellation type is determined.
             FALSE if the constellation type cannot be determined.

SIDE EFFECTS None.

=========================================​====================================*/
boolean sm_GetXtraConstelType(sm_XtraConstelEnumType* pe_ConstelType)
{
  *pe_ConstelType =(sm_XtraConstelEnumType) tm_core_get_curr_sec_gnss_constellation();
  MSG_2( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, 
           "SM_API: Secondary constellation id = %d", *pe_ConstelType, 0);
  

  if(SM_XTRA_CONSTEL_TYPE_MAX == *pe_ConstelType)
  {
     return FALSE;
  }
  return TRUE;
}

/* !!!!!!!!!!!!!!!!!!!!!!APIs called from PE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
/*=======================================​======================================
FUNCTION     sm_MgpDeInitAckStatus

DESCRIPTION   De-initialize Ack status from MGP

DEPENDENCIES None

PARAMETERS  TRUE/FALSE

RETURN VALUE None

SIDE EFFECTS None.

=========================================​====================================*/
void sm_MgpDeInitAckStatus(boolean v_AckStatus)
{
  os_IpcMsgType   *ipc_msg_ptr  = NULL;

  if (v_AckStatus == FALSE)
  {
    ERR_FATAL("MGP deinit failed", 0, 0, 0);
  }

  /* Send the MSG to SM TM thread */
  ipc_msg_ptr = sm_send_message( (uint32)C_SM_LM_MSG_MGP_DEINITED_IND, sizeof(boolean), (uint32)THREAD_ID_SM_TM, (void *)&v_AckStatus );

  if(ipc_msg_ptr != NULL)
  {
    MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_LOW, "SM_API: IPC msg send to SM - C_SM_LM_MSG_MGP_DEINITED_IND, id = %d", ipc_msg_ptr->q_MsgId );
  }
}

/*=======================================​======================================
FUNCTION     sm_MgpInitAckStatus

DESCRIPTION   Initialize Ack status from MGP

DEPENDENCIES None

PARAMETERS  TRUE/FALSE

RETURN VALUE None

SIDE EFFECTS None.

=========================================​====================================*/
void sm_MgpInitAckStatus(boolean v_AckStatus)
{
  os_IpcMsgType   *ipc_msg_ptr  = NULL;
 
  if (v_AckStatus == FALSE)
  {
    ERR_FATAL("MGP init failed", 0, 0, 0);
  }
 
  /* Send the MSG to SM TM thread */
  ipc_msg_ptr = sm_send_message( (uint32)C_SM_LM_MSG_MGP_INITED_IND, sizeof(boolean), (uint32)THREAD_ID_SM_TM, (void *)&v_AckStatus );
 
  if(ipc_msg_ptr != NULL)
  {
    MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_LOW, "SM_API: IPC msg send to SM - C_SM_LM_MSG_MGP_INITED_IND, id = %d", ipc_msg_ptr->q_MsgId );
  }
}

/*=======================================​======================================
FUNCTION     sm_DeleteXtraData

DESCRIPTION   API to delete XTRA bin file

DEPENDENCIES None

PARAMETERS  None

RETURN VALUE None

SIDE EFFECTS None.

=========================================​====================================*/
void sm_DeleteXtraData(void)
{
  os_IpcMsgType  *ipc_msg_ptr  = NULL;
  
  /* Send the MSG to SM TM thread */
  ipc_msg_ptr = sm_send_message( (uint32)TM_CORE_MSG_ID_DELETE_XTRA_DATA,0 , (uint32)THREAD_ID_SM_TM, NULL );
  
  if(ipc_msg_ptr != NULL)
  {
    MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_LOW, "SM_API: IPC msg send to SM - DELETE_XTRA_DATA, id = %d", ipc_msg_ptr->q_MsgId );
  }
}
/*=======================================​======================================
FUNCTION     sm_DeleteXtraIntegrity

DESCRIPTION   API to delete XTRA integrity bin file

DEPENDENCIES None

PARAMETERS  None

RETURN VALUE None

SIDE EFFECTS None.

=========================================​====================================*/
void sm_DeleteXtraIntegrity(void)
{
  os_IpcMsgType  *ipc_msg_ptr  = NULL;
  
  /* Send the MSG to SM TM thread */
  ipc_msg_ptr = sm_send_message( (uint32)TM_CORE_MSG_ID_DELETE_XTRA_INTEGRITY_DATA,0 , (uint32)THREAD_ID_SM_TM, NULL );
  
  if(ipc_msg_ptr != NULL)
  {
    MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_LOW, "SM_API: IPC msg send to SM - DELETE_XTRA_INTEGRITY_DATA, id = %d", ipc_msg_ptr->q_MsgId );
  }

}
/* !!!!!!!!!!!!!!!!!!!!!!APIs called from PE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */


/**========================================================================================================

!!!!!!!!!!!!!!!!!!!!!!IMPORTANT!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
THIS IS A DUMMY SECTION WITH STUBS.

ALL THIS HAS TO BE REMOVED ONCE THE RESPECTIVE MODULES CHECK-IN THE CODE

**=========================================================================================================*/
/*Stubs for 9x05 compilation*/
void sm_StoreFtcalTTData (cgps_FtcalTTStruct* p_TTData)
{

}

/*===========================================================================

FUNCTION sm_RcvrInUsStatus()

DESCRIPTION
  This function will be called to say if receiver is inside/outside US.
  Called every second and only when PE generated fixes are available.

DEPENDENCIES

RETURN VALUE  

SIDE EFFECTS

===========================================================================*/
void sm_RcvrInUsStatus(boolean v_RcvrInUS)
{
  os_IpcMsgType  *ipc_msg_ptr  = NULL;

  if(FALSE == SM_IS_RCVR_CONFIG_UPDATE_NEEDED(v_RcvrInUS) )
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_MED, "sm_RcvrInUsStatus: No change in RCVR state , do not send msg");
    return;
  }
  

  /* Send the MSG to SM TM thread only when there is a change in RCVR state and secondary contel is BDS */
  ipc_msg_ptr = sm_send_message( (uint32)TM_CORE_MSG_ID_RCVR_IN_US_STATUS,sizeof(v_RcvrInUS) , (uint32)THREAD_ID_SM_TM, (void*)&v_RcvrInUS );
  
  if(ipc_msg_ptr != NULL)
  {
    MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "SM_API: IPC msg send to SM - sm_RcvrInUsStatus, id = %d", ipc_msg_ptr->q_MsgId );
  }
}

/*=============================================================================

FUNCTION
  sm_ReportCPEMeas

DESCRIPTION
  Report CPE meas from ME 

DEPENDENCIES
  None.

PARAMETERS
 p_CPEMeas - Pointer to CPE Measurement Block

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportCPEMeas(gnss_CPEMeasBlockStructType *p_CPEMeas)
{/*OEM related*/}

/*=============================================================================

FUNCTION
  sm_ReportGnssEphExpTime

DESCRIPTION
  Report Eph data from PE

DEPENDENCIES
  None.

PARAMETERS
 p_CPEMeas - Pointer to Eph data

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/


void sm_ReportGnssEphExpTime(const gnss_EphExpTimeStructType *pz_GnssEphAvailInfo)
{
   os_IpcMsgType	*ipc_msg_ptr  = NULL;
   uint32		  msg_id		= 0;
   uint32		  size			= 0;
   MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"sm_ReportGnssEphExpTime()");
	
   msg_id = (uint32)TM_CORE_MSG_ID_EPH_EXPIRY_TIME;
   size	 = sizeof(gnss_EphExpTimeStructType);
	
   /* Send the MSG to SM TM thread */
   ipc_msg_ptr = sm_send_message(msg_id, size, (uint32)THREAD_ID_SM_TM, (void *)pz_GnssEphAvailInfo);
	
   if (ipc_msg_ptr != NULL)
   {
   	  MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_LOW,"SM_API: IPC msg send to SM_TM, id = %d",ipc_msg_ptr->q_MsgId);
   }

}


