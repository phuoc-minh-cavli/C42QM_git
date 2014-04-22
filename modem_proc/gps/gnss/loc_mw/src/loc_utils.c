/*============================================================================
  FILE:         loc_utils.c

  OVERVIEW:     Utility functions for loc_slim and loc_sdp modules.

  DEPENDENCIES: None.
 
                Copyright (c) 2014 QUALCOMM Atheros, Inc.
                All Rights Reserved.
                Qualcomm Atheros Confidential and Proprietary
                Copyright (c) 2015 - 2019 Qualcomm Technologies, Inc. 
                All Rights Reserved.
                Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/loc_mw/src/loc_utils.c#1 $
  $DateTime: 2020/07/21 21:23:44 $
  $Author: pwbldsvc $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  08/21/19   ska   Added support for NMEA GSV
  01/11/16    yh   LocMW refactoring
  2014-04-17  lv   Initial revision.

============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "loc_api_internal.h"
#include "loc_utils.h"
#include "loc_qmi_shim.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
/**
 * Create an ipc message and send to LocMW task
 *
 * @param msg_id    ipc message id
 * @param loc_client_handle
 *                  LocAPI client handle
 * @param transaction_id
 *                  will be duplicated into the resulting indication
 * @param p_payload QMI request, usually data structure created by the IDL compiler
 * @param payload_size
 *                  size of the QMI request
 *
 * @return 0 if ipc has been sent successfully
 */
uint32 loc_send_ipc_to_locmw
(
   uint32 msg_id,
   int32 loc_client_handle,
   uint32 transaction_id,
   const void *p_payload,
   uint32 payload_size
)
{
  uint32 error_code = 1;
  uint32 msg_size = sizeof(loc_client_handle) + sizeof(transaction_id) + payload_size;

  if (NULL != p_payload)
  {
    os_IpcMsgType *ipc_msg_ptr =
      os_IpcCreate(msg_size, IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_LOC_MIDDLEWARE);
    if (NULL != ipc_msg_ptr)
    {
      // casting to void * first to avoid lint error of memory size
      byte *p_insertion_point = (byte *)(void *)ipc_msg_ptr->p_Data;
      int32 *p_client_handle = NULL;
      uint32 *p_transaction_id = NULL;
      void *p_payload_store = NULL;

      // calculate offset
      p_client_handle = (int32 *)p_insertion_point;
      p_insertion_point += sizeof(int32);
      p_transaction_id = (uint32 *)p_insertion_point;
      p_insertion_point += sizeof(uint32);
      p_payload_store = (void *)p_insertion_point;

      // first is client handle
      *p_client_handle = (uint32)loc_client_handle;
      // copy the transaction id
      *p_transaction_id = transaction_id;
      // make a shallow copy of the request
      (void)memscpy(p_payload_store, payload_size, p_payload, payload_size);

      ipc_msg_ptr->q_MsgId = msg_id;
      if (TRUE == os_IpcSend(ipc_msg_ptr, THREAD_ID_LOC_MIDDLEWARE))
      {
        // ipc sent
        error_code = 0;
      }
      else
      {
        // ipc not sent
        (void)os_IpcDelete(ipc_msg_ptr);
        error_code = 2;
      }
    }
    else
    {
      // ipc memory allocation failure, ipc not sent
      error_code = 3;
    }
  }
  else
  {
    // null pointer for payload. this shall never happen
    error_code = 4;
  }

  if (0 != error_code)
  {
    LOC_MSG_ERROR("[LOC] loc_send_ipc_to_locmw: "
      "error code: %u", error_code, 0, 0);
  }
  return error_code;
}


/**
 * Unpack an ipc message sent to LocMW.
 *
 * @param p_ipc_msg_in
 *                  [in] ipc message
 * @param p_loc_client_handle
 *                  [out] LocAPI client handle
 * @param p_transaction_id
 *                  [out] transaction id in the request
 * @param p_payload [out] QMI request. structure created by IDL compiler.
 *                  size must be enough to hold payload_size
 * @param payload_size
 *                  [in] size of the QMI request
 *
 * @return 0 if ipc has been unpacked successfully
 */
uint32 loc_unpack_ipc_to_locmw
(
  const os_IpcMsgType *p_ipc_msg_in,
  int32 *p_loc_client_handle,
  uint32 *p_transaction_id,
  void *p_payload,
  uint32 payload_size
)
{
  uint32 error_code = 1;
  uint32 msg_size = sizeof(int32) + sizeof(uint32) + payload_size;

  if (NULL != p_ipc_msg_in &&
      NULL != p_loc_client_handle &&
      NULL != p_transaction_id &&
      NULL != p_payload &&
      msg_size <= p_ipc_msg_in->q_Size)
  {
    byte *p_extraction_point = (byte *)(void *)p_ipc_msg_in->p_Data;
    int32 *p_client_handle = NULL;
    uint32 *p_transaction_id_store = NULL;
    void *p_payload_store = NULL;

    // calculate offset
    p_client_handle = (int32 *)p_extraction_point;
    p_extraction_point += sizeof(int32);
    p_transaction_id_store = (uint32 *)p_extraction_point;
    p_extraction_point += sizeof(uint32);
    p_payload_store = (void *)p_extraction_point;

    // retrieve client id
    *p_loc_client_handle = *p_client_handle;
    // retrieve transaction id
    *p_transaction_id = *p_transaction_id_store;
    // retrieve payload
    (void)memscpy(p_payload, payload_size, p_payload_store, payload_size);

    error_code = 0;
  }
  else
  {
    // null pointer for something. this shall never happen
    error_code = 2;
  }

  if (0 != error_code)
  {
    LOC_MSG_ERROR("[LOC] loc_unpack_ipc_to_locmw: "
      "error code: %u", error_code, 0, 0);
  }
  return error_code;
}

/**
 * get a somewhat unique transaction id for debugging and message matching purposes
 *
 * @return qmiloc transaction id
 */
uint32 loc_qmiloc_get_transaction_id
(
  void
)
{
  // it might be better if transaction can be globally unique and time varying so we can survive
  // state machine resets and the like
  // however, it's a bit difficult to acquire true time varying source from higher level code
  static uint32 transaction_id = 0;
  ++transaction_id;

  return transaction_id;
}

/*---------------------------------------------------------------------------
@brief
  Function to generate an IPC to locMW task

@param[in] e_MsgId       : LocMW IPC message ID
@param[in] q_PayloadSize : Payload size
@param[in] p_PayloadData : Pointer to the payload data

@retval    TRUE    if IPC posted to LocMW successfully
@retval    FALSE   if IPC posted to LocMW unsuccessfully
---------------------------------------------------------------------------*/
boolean locMW_SendIpcMsg
(
  loc_middleware_msg_id_e_type e_MsgId,
  uint32                       q_PayloadSize,
  const void *                 p_PayloadData
)
{
  os_IpcMsgType* p_IpcMsg = os_IpcCreate(q_PayloadSize, IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_LOC_MIDDLEWARE);

  LOC_MSG_LOW("locMW_SendIpcMsg: Sending IPC MsgId=%d to LocMW", e_MsgId, 0, 0);

  if (NULL != p_IpcMsg)
  {
    /* Copy the payload */
    (void)memscpy(p_IpcMsg->p_Data, q_PayloadSize, p_PayloadData, q_PayloadSize);

    p_IpcMsg->q_MsgId = e_MsgId;
    if(FALSE == os_IpcSend(p_IpcMsg, THREAD_ID_LOC_MIDDLEWARE))
    {
      LOC_MSG_ERROR("locMW_SendIpcMsg: Sending IPC MsgId=%d to LocMW failed", e_MsgId, 0, 0);
      os_IpcDelete(p_IpcMsg);
      return FALSE;
    }
    return TRUE;
  }
  return FALSE;
}



/*---------------------------------------------------------------------------
@brief
  Function to send IPC to LocMW task for servicing Indication to previous
  QMI_LOC request

@param[in] l_ClientHandle : Loc client handle
@param[in] q_QmiMsgId     : QMI_LOC Msg Id
@param[in] p_QmiIndMsg    : QMI_LOC Indication Msg Data
@param[in] q_LocApiStatus : Parameter Status from Loc API

@retval    TRUE    if IPC posted to LocMW successfully
@retval    FALSE   if IPC posted to LocMW unsuccessfully
---------------------------------------------------------------------------*/
boolean locMW_SendQmiIndicationProxy
(
   loc_client_handle_type     l_ClientHandle,
   uint32                     q_QmiMsgId,
   void*                      p_QmiIndMsg,
   uint32                     q_LocApiStatus
)
{
   os_IpcMsgType*                pz_IpcMsg = NULL;
   locMW_QmiLocIndMsgStructT*    pz_LocIndMsg = NULL;
   boolean                       v_RetVal = FALSE;

   do
   {
      /* Generate IPC message to LocMW */
      pz_IpcMsg = os_IpcCreate(sizeof(*pz_LocIndMsg), IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_LOC_MIDDLEWARE);
      if(NULL == pz_IpcMsg)
      {
         LOC_MSG_ERROR("locMW_SendQmiIndicationProxy: os_IpcCreate failed", 0, 0, 0);
         break;
      }

      LOC_MSG_MED("locMW_SendQmiIndicationProxy: ClientHandle=%d, q_QmiMsgId=0x%04x, ParamStatus=%d",
                  l_ClientHandle, q_QmiMsgId, q_LocApiStatus);

      pz_IpcMsg->q_MsgId = LM_MIDDLEWARE_MSG_ID_QMI_LOC_MW_IND;
      pz_LocIndMsg = (locMW_QmiLocIndMsgStructT*)(pz_IpcMsg->p_Data);
      pz_LocIndMsg->l_ClientHandle = l_ClientHandle;
      pz_LocIndMsg->q_QmiIndMsgId  = q_QmiMsgId;
      pz_LocIndMsg->p_QmiIndMsg    = p_QmiIndMsg;
      pz_LocIndMsg->q_LocApiStatus = q_LocApiStatus;

      /* Generate IPC message to LocMW */
      if(FALSE == os_IpcSend(pz_IpcMsg, THREAD_ID_LOC_MIDDLEWARE))
      {
         LOC_MSG_ERROR("locMW_SendQmiIndicationProxy: Sending IPC to LocMW failed", 0, 0, 0);
         os_IpcDelete(pz_IpcMsg);
      }
      else
      {
         v_RetVal = TRUE;
      }
   }while(0);

   /* The caller of locMW_SendQmiIndicationProxy allocated pz_QmiIndMsg,
   free the heap memory for IpcSend failure case */
   if((FALSE == v_RetVal) && (NULL != p_QmiIndMsg))
   {
      loc_free(p_QmiIndMsg);
   }

   return v_RetVal;
}

/*---------------------------------------------------------------------------
@brief
  Function to send QMI_LOC indication to client for previous QMI_LOC request.

@param[in] p_IpcMsg : IPC Message containing the QMI_LOC indication data

@retval    TRUE    if IPC message is handled successfully
@retval    FALSE   if IPC message is handled unsuccessfully
---------------------------------------------------------------------------*/
boolean locMW_HandleQmiIndication
(
   const os_IpcMsgType* p_IpcMsg
)
{
   qmiLocStatusEnumT_v02 e_IndStatus;
   boolean               v_RetVal = FALSE;
   qmiLocStatusEnumT_v02 *p_Temp = NULL;

   if (NULL == p_IpcMsg)
   {
      LOC_MSG_ERROR("locMW_HandleQmiIndication: Invalid IPC message", 0, 0, 0);
      return v_RetVal;
   }
   else
   {
      locMW_QmiLocIndMsgStructT* pz_LocIndMsg = (locMW_QmiLocIndMsgStructT *)(p_IpcMsg->p_Data);

      if(NULL == pz_LocIndMsg)
      {
         LOC_MSG_ERROR("locMW_HandleQmiIndication: NULL pz_LocIndMsg",0,0,0);
         return v_RetVal;
      }

      /* Translate Indication Status from LocMW to QMI_LOC type */
      switch(pz_LocIndMsg->q_LocApiStatus)
      {
         case LOC_API_SUCCESS:
            e_IndStatus = eQMI_LOC_SUCCESS_V02;
            break;

         case LOC_API_INVALID_PARAMETER:
            e_IndStatus = eQMI_LOC_INVALID_PARAMETER_V02;
            break;

         case LOC_API_UNSUPPORTED:
            e_IndStatus = eQMI_LOC_UNSUPPORTED_V02;
            break;

         case LOC_API_ENGINE_BUSY:
            e_IndStatus = eQMI_LOC_ENGINE_BUSY_V02;
            break;

         case LOC_API_PHONE_OFFLINE:
            e_IndStatus = eQMI_LOC_PHONE_OFFLINE_V02;
            break;

         case LOC_API_TIMEOUT:
            e_IndStatus = eQMI_LOC_TIMEOUT_V02;
            break;

         case LOC_API_XTRA_VERSION_CHECK_FAILURE:
            e_IndStatus = eQMI_LOC_XTRA_VERSION_CHECK_FAILURE_V02;
            break;

         case LOC_API_GNSS_DISABLED:
         case LOC_API_GNSS_SERVICE_NOT_AVAILABLE:
            e_IndStatus = eQMI_LOC_GNSS_DISABLED_V02;
            break;

         default:
            e_IndStatus = eQMI_LOC_GENERAL_FAILURE_V02;
            break;
      }

      if(NULL != pz_LocIndMsg->p_QmiIndMsg)
      {
         p_Temp = (qmiLocStatusEnumT_v02 *)pz_LocIndMsg->p_QmiIndMsg;
         *p_Temp = e_IndStatus;
      }

      v_RetVal = locQmi_ProcessInd(pz_LocIndMsg->l_ClientHandle,
                                 pz_LocIndMsg->q_QmiIndMsgId,
                                 pz_LocIndMsg->p_QmiIndMsg,
                                 e_IndStatus);
      /* p_QmiIndMsg is allocated by the caller of locMW_SendQmiIndicationProxy
         free here */
      if(NULL != pz_LocIndMsg->p_QmiIndMsg)
      {
         loc_free(pz_LocIndMsg->p_QmiIndMsg);
         pz_LocIndMsg->p_QmiIndMsg = NULL;
      }
   }

   return v_RetVal;
}

#define LOC_CMD_TYPE_STRING_LEN 40

static const struct
{
   LocMW_LocCmdEnumT        e_LocCmdType;
   char                     u_LocCmdString[LOC_CMD_TYPE_STRING_LEN];
}LocMW_CmdTypeTable[]= {

   { LOC_CMD_TYPE_PD_SCHEDULE, 
     "LOC_CMD_TYPE_PD_SCHEDULE" },
   { LOC_CMD_TYPE_REPORT_POSITION, 
     "LOC_CMD_TYPE_REPORT_POSITION" },
   { LOC_CMD_TYPE_NOTIFY_CLIENT, 
     "LOC_CMD_TYPE_NOTIFY_CLIENT" },
   { LOC_CMD_TYPE_INJECT_XTRA_DATA, 
     "LOC_CMD_TYPE_INJECT_XTRA_DATA" },
   { LOC_CMD_TYPE_SILENTLY_DENY_NI_REQUEST, 
     "LOC_CMD_TYPE_SILENTLY_DENY_NI_REQUEST" },
};

/*---------------------------------------------------------------------------
@brief
  The helper function to print out LOC_CMD_TYPE for easy debugging

@param[in] e_LocCmdType : Loc command type

@retval    LOC_CMD_TYPE in string
---------------------------------------------------------------------------*/
static const char* locMW_LocCmdTypeString
(
   LocMW_LocCmdEnumT        e_LocCmdType
)
{
   uint8 i;
   for(i = 0; i< (sizeof(LocMW_CmdTypeTable)/sizeof(LocMW_CmdTypeTable[0])); i++)
   {
      if(e_LocCmdType == LocMW_CmdTypeTable[i].e_LocCmdType)
         return LocMW_CmdTypeTable[i].u_LocCmdString;
   }
   return "LOC_CMD_TYPE UNKNOWN";
}

/*---------------------------------------------------------------------------
@brief
  Function to send IPC to LocMW task for LocMW command

@param[in] l_ClientHandle : Loc client handle
@param[in] e_LocCmdType   : LocMW command ID
@param[in] t_EventType    : LocMW event type
@param[in] p_LocCmdData   : LocMW command data

@retval    TRUE    if IPC posted to LocMW successfully
@retval    FALSE   if IPC posted to LocMW unsuccessfully
---------------------------------------------------------------------------*/
boolean locMW_SendLocCmdProxy
(
   loc_client_handle_type     l_ClientHandle,
   LocMW_LocCmdEnumT          e_LocCmdType,
   qmiLocEventRegMaskT_v02      t_EventType,
   void*                      p_LocCmdData
)
{
   os_IpcMsgType*             pz_IpcMsg = NULL;
   LocMW_LocCmdStructT*       pz_CmdData = NULL;
   boolean                    v_RetVal = FALSE;

   do
   {
   
      /* Generate IPC message to LocMW */
      pz_IpcMsg = os_IpcCreate(sizeof(*pz_CmdData), IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_LOC_MIDDLEWARE);
      if(NULL == pz_IpcMsg)
      {
         LOC_MSG_ERROR("locMW_SendQmiIndicationProxy: os_IpcCreate failed", 0, 0, 0);
         break;
      }

      LOC_MSG_HIGH("locMW_SendLocCmdProxy %s", locMW_LocCmdTypeString(e_LocCmdType), 0, 0);

      pz_IpcMsg->q_MsgId = LM_MIDDLEWARE_MSG_ID_PDAPI;
      pz_CmdData = (LocMW_LocCmdStructT*)(pz_IpcMsg->p_Data);
      pz_CmdData->l_ClientHandle   = l_ClientHandle;
      pz_CmdData->e_CmdType        = e_LocCmdType;
      pz_CmdData->t_EventType      = t_EventType;
      pz_CmdData->p_QmiEventIndMsg = p_LocCmdData;

      /* Generate IPC message to LocMW */
      if(FALSE == os_IpcSend(pz_IpcMsg, THREAD_ID_LOC_MIDDLEWARE))
      {
         LOC_MSG_ERROR("locMW_SendLocCmdProxy: Sending IPC to LocMW failed", 0, 0, 0);
         os_IpcDelete(pz_IpcMsg);
      }
      else
      {
         v_RetVal = TRUE;
      }
   }while(0);

   /* The caller of locMW_SendLocCmdProxy allocated p_LocData,
   free the heap memory for IpcSend failure case */
   if((FALSE == v_RetVal) && (NULL != p_LocCmdData))
   {
      loc_free(p_LocCmdData);
   }

   return v_RetVal;
}

/*---------------------------------------------------------------------------
@brief
  Function to handle LocMW command

@param[in] p_IpcMsg : IPC Message containing the LocMW command data

@retval    TRUE    if IPC message is handled successfully
@retval    FALSE   if IPC message is handled unsuccessfully
---------------------------------------------------------------------------*/
boolean locMW_HandleLocCmd
(
  const os_IpcMsgType* p_IpcMsg
)
{
   if (NULL == p_IpcMsg)
   {
      LOC_MSG_ERROR("locMW_HandleLocCmd: Invalid IPC message", 0, 0, 0);
      return FALSE;
   }
   else
   {
      LocMW_LocCmdStructT*   pz_CmdData = (LocMW_LocCmdStructT*)(p_IpcMsg->p_Data);

      LOC_MSG_HIGH("locMW_HandleLocCmd %s", locMW_LocCmdTypeString(pz_CmdData->e_CmdType), 0, 0);
      switch (pz_CmdData->e_CmdType)
      {
      case LOC_CMD_TYPE_PD_SCHEDULE:
      case LOC_CMD_TYPE_REPORT_POSITION:
	  case LOC_CMD_TYPE_REPORT_SV_INFO: 
         locPd_ProcessCmdRequest (pz_CmdData);
         break;

      case LOC_CMD_TYPE_NOTIFY_CLIENT:
         locClnt_ProcessCmdRequest (pz_CmdData);
         break;

      case LOC_CMD_TYPE_INJECT_XTRA_DATA:
         locXtra_ProcessCmdRequest (pz_CmdData);
         break;
      /*
        #9x05: LOC_CMD_TYPE_SILENTLY_DENY_NI_REQUEST no longer supported 
        */


      default:
         break;
      }

      if(NULL != pz_CmdData->p_QmiEventIndMsg)
      {
         loc_free(pz_CmdData->p_QmiEventIndMsg);
         pz_CmdData->p_QmiEventIndMsg = NULL;
      }
   }
   return TRUE;
}