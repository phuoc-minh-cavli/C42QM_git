/*===========================================================================

                         Q M I _ M M C _ S V C . C

DESCRIPTION

 The QMI MMODE Services Source file. Contains routines common to
 all QMI services.

 Note this library assumes caller is running in QMI-MMODE task context.

EXTERNALIZED FUNCTIONS

   qmi_svc_prepend_msg_hdr()
     Append the message header type and length to the provided QMI service
     message payload

Copyright (c) 2005-2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/mmcp.mpss/6.1.10/mmode/qmi/src/qmi_mmode_svc.c#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/08/12     sk     Initial changes

===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "amssassert.h"
#include "dsm.h"
#include "task.h"
#include "modem_mem.h"
#include "qmi_mmode_svc.h"
#include "qmi_voice.h"
#include "ps_in.h"
#include "qmi_mmode_task_cmd.h"
#include "qmi_nas_internal.h"
#include "qmi_idl_lib.h"
#include "common_v01.h"
#include "qm_nv.h"
#include "qm_cbpq.h"
#include "qm_si.h"
#include "qm_nas_clnt.h"
#include "qm_meas.h"
#include "qm_mstats.h"
#include "qm_timer.h"
#include "qm_efs.h"
#include "dsm_item.h"
#include "cmtaski.h"
#ifdef CM_DEBUG
#error code not present
#endif
#include "qmi_psm.h"
#include "mqcsi_log.h"
/*===========================================================================
FUNCTION QMI_MMODE_SVC_INIT()

DESCRIPTION
  This function initializes various qmi services in qmi mmode task.
 
PARAMETERS 
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_mmode_svc_init
(
  void
)
{
  /*-------------------------------------------------------------------------
    Initialize QM-Modules here
  -------------------------------------------------------------------------*/
  #if !(defined(FEATURE_MMODE_QMI_LTE_ONLY) || defined(FEATURE_MMODE_QMI_GPRS_LTE))
  qm_nv_init();
  #endif
  qm_cbpq_init();
  qm_si_init();
  qm_meas_init();
#ifdef FEATURE_MODEM_STATISTICS
  qm_mstats_init();
#endif
#ifndef FEATURE_MMODE_QMI_DISABLE_VOICE
  qm_timer_init();
#endif
  qm_efs_init();

  /* Initialize qmi services here */
#ifndef FEATURE_MMODE_QMI_DISABLE_VOICE
  qmi_voice_init();
#endif
  qmi_nas_init();
  qmi_psm_init();

}/* qmi_mmode_svc_init */

/*===========================================================================
  FUNCTION QMI_MMODE_IDL_MESSAGE_DECODE()

  DESCRIPTION
    Decode QMI NAS message into c structure from incoming dsm item

  PARAMETERS
    svc         The QMI IDL service object.
    message_id  QMI message id
    msg_ptr     Pointer to dsm item that contains TLVs 
    p_req       Pointer to C structure to contain decoded message
    p_req_size  Size of C structure to contain decoded data

  RETURN VALUE
    QMI_ERR_NONE_V01 is returned if decode is successful, otherwise relevant error is returned

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
 qmi_error_type_v01 qmi_mmode_idl_message_decode
 ( 
   qmi_idl_service_object_type svc, 
   uint16_t message_id, 
   dsm_item_type **msg_ptr, 
   void *p_req, 
   uint32_t p_req_size
 )
 {
   qmi_error_type_v01 errval = QMI_ERR_NONE_V01;
   int32_t            retval = QMI_IDL_LIB_NO_ERR;
   uint32             size;
   uint8              *buf = NULL;
    
 
   if( msg_ptr != NULL && *msg_ptr != NULL ) // We reutrn QMI_ERR_NONE_V01 if msg_ptr is NULL. The check() function is going to verify the malformed message in case there is no TLVs present.
   {
     size = dsm_length_packet( *msg_ptr );
     QM_MSG_MED_1("qmi_mmode_idl_message_decode dsm_pullup size %d", size);

     if ( size > 0 )
     {
       buf = (uint8 *)modem_mem_alloc((size_t)size, MODEM_MEM_CLIENT_QMI);
     
       if ( buf != NULL )
       {
         if ( size == dsm_pullup( msg_ptr, buf, (uint16)size ) )
         {
           retval = qmi_idl_message_decode( svc,
                                            QMI_IDL_REQUEST,     // IDL message type
                                            message_id,          // QMI NAS cmd type
                                            (void*)buf,          // pointer to incoming byte stream
                                            (uint32_t)size,      // pointer to no of bytes to be read
                                            p_req,               // pointer to c structure
                                            p_req_size           // max size of request message, don't get this from IDL_ACCESSOR_MAX_LEN function
                                          );
         }
         else
         {
           retval = QMI_IDL_LIB_LENGTH_INCONSISTENCY;
           QM_MSG_HIGH_1("qmi_mmode_idl_message_decode dsm_pullup failed size %d", size);
         }
     
         modem_mem_free( (uint8*)buf, MODEM_MEM_CLIENT_QMI );
       }
       else
       {
         retval = QMI_IDL_LIB_LENGTH_INCONSISTENCY;
         QM_MSG_HIGH("qmi_mmode_idl_message_decode no memory for buffer");
       }
     }
     else
     {
       retval = qmi_idl_message_decode( svc,
                                 QMI_IDL_REQUEST,     // IDL message type
                                 message_id,          // QMI NAS cmd type
                                 (*msg_ptr)->data_ptr,   // pointer to incoming byte stream
                                 (*msg_ptr)->used,      // pointer to no of bytes to be read
                                 p_req,               // pointer to c structure
                                 p_req_size           // max size of request message, don't get this from IDL_ACCESSOR_MAX_LEN function
                               );
     }
  
     switch( retval )
     {  
       case QMI_IDL_LIB_NO_ERR:
         errval = QMI_ERR_NONE_V01;
         break;
    
       case QMI_IDL_LIB_ARRAY_TOO_BIG:
         errval = QMI_ERR_ARG_TOO_LONG_V01;
         break;
    
       case QMI_IDL_LIB_MESSAGE_ID_NOT_FOUND:
         errval = QMI_ERR_NOT_SUPPORTED_V01;
         break;
    
       case QMI_IDL_LIB_TLV_DUPLICATED :
       case QMI_IDL_LIB_LENGTH_INCONSISTENCY:
       case QMI_IDL_LIB_MISSING_TLV:
       case QMI_IDL_LIB_UNKNOWN_MANDATORY_TLV:
         errval = QMI_ERR_MALFORMED_MSG_V01;
         break;
    
       case QMI_IDL_LIB_EXTENDED_ERR:
       case QMI_IDL_LIB_BUFFER_TOO_SMALL:
       case QMI_IDL_LIB_PARAMETER_ERROR:
       case QMI_IDL_LIB_UNRECOGNIZED_SERVICE_VERSION:
       case QMI_IDL_LIB_INCOMPATIBLE_SERVICE_VERSION:
       default:
         errval = QMI_ERR_INTERNAL_V01;
         break;
     }
  
     if ( retval != QMI_IDL_LIB_NO_ERR )
     {
       QM_MSG_HIGH_2("qmi_mmode_idl_message_decode failed %d %d", retval, errval);
     }
   }
 
   return errval;
 }

/*===========================================================================
  FUNCTION QMI_MMODE_IDL_MESSAGE_ENCODE()

  DESCRIPTION
    Encode QMI NAS message into dsm item from incoming c structure

  PARAMETERS
    svc          The QMI IDL service object.
    type         QMI IDL type ( QMI_IDL_RESPONSE or QMI_IDL_INDICATION )
    message_id   QMI message id
    p_src        Pointer to C structure containing message data
    p_src_size   Size of C structure that contains message data
    msg_ptr      Address of pointer to dsm item for storing encoded message data

  RETURN VALUE
    TRUE if success, FALSE if failed

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_mmode_idl_message_encode
( 
  qmi_idl_service_object_type  svc, 
  qmi_idl_type_of_message_type type, 
  uint16_t                     message_id, 
  void                         *p_src, 
  uint32_t                     p_src_size, 
  dsm_item_type                **p_response 
)
{
  qmi_error_type_v01  eval        = QMI_IDL_LIB_NO_ERR;
  int32_t             eval2       = QMI_IDL_LIB_NO_ERR;
  uint32_t            rsp_buf_len = 0;
  uint8 *             rsp_buf     = NULL;
  uint32_t            size_buf    = 0;
  boolean             retval      = FALSE;

  *p_response = NULL;
 
  /*--------------------------------------------------------------------------------
    Accessor function for getting the maximum message length of a response
    message. Message length includes c struct size, size of type and size of length.
  ----------------------------------------------------------------------------------*/
  eval = qmi_idl_get_max_message_len( svc, type, message_id, &rsp_buf_len );
  QM_MSG_HIGH_1("qmi_mmode_idl_message_encode: Message being encoded is %d", message_id);
  if ( eval == QMI_IDL_LIB_NO_ERR )
  {
    rsp_buf = (uint8 *)modem_mem_alloc(rsp_buf_len, MODEM_MEM_CLIENT_QMI);

    if ( rsp_buf != NULL )
    {
      memset(rsp_buf, 0x00, rsp_buf_len);
      eval2 = qmi_idl_message_encode( svc,
                                      type,         // IDL message type
                                      message_id,   // QMI NAS cmd type
                                      p_src,        // pointer to c structure
                                      p_src_size,   // size of c structure which is to be encoded
                                      rsp_buf,      // buffer to contain encoded message data
                                      rsp_buf_len,  // size of buffer, should be maximum size of the encoded c structure
                                      &size_buf     // pointer to store length of the encoded message
                                    );

      if ( eval2 == QMI_IDL_LIB_NO_ERR )
      {
        if ( size_buf != dsm_pushdown_packed( p_response, &rsp_buf[0], (uint16)size_buf, DSM_DS_SMALL_ITEM_POOL) )
        {
          QM_MSG_ERROR_1("Pushdown Failed for qmi_nas_idl_message_encode %d", size_buf);
          dsm_free_packet(p_response);
        }
        else
        {
          retval = TRUE;
        }
      }
      else
      {
        QM_MSG_ERROR_2("qmi_idl_message_encode failed eval2 = %d,  src_size = %d", eval2, p_src_size);
      }
    }
    else
    {
      QM_MSG_ERROR_1("Not enough memory to allocate for QMI_NAS_IDL_MESSAGE_ENCODE cmd_type = %d", message_id);
    }
  }
  else
  {
    QM_MSG_ERROR_1("qmi_idl_get_max_message_len failed %d", eval);
  }
  
  if ( rsp_buf != NULL ) { modem_mem_free((uint8*) rsp_buf, MODEM_MEM_CLIENT_QMI); }

  return retval;
}

/*===========================================================================
  FUNCTION qmi_mmode_svc_free_pending_transaction()

  DESCRIPTION
    Deletes the pending transaction provided and Frees the memory allocated

  PARAMETERS
    svc_ptr : Service pointer thats associated with QMI Service
    pending_trans : Pointer to pending transaction

  RETURN VALUE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_mmode_svc_free_pending_transaction
(
  qmi_mmode_svc_info_type          *svc_ptr,
  qmi_mmode_qcsi_transaction_type **pending_trans
)
{
  qmi_mmode_qcsi_transaction_type *trans;

  /* Identify handle from queue of pending requests and delete from queue */
  trans = q_linear_delete_new( &svc_ptr->pending_req,
                               qmi_mmode_qcsi_compare_pending_trans,
                               (void *)(*pending_trans)->req_handle,
                               NULL,
                               NULL );

  if( trans != NULL )
  {
    modem_mem_free((void *)trans, MODEM_MEM_CLIENT_QMI);
    *pending_trans = NULL;
  }
  
}/* qmi_mmode_svc_free_pending_transaction */

/*===========================================================================
  FUNCTION qmi_mmode_qcsi_compare_client_handle()

  DESCRIPTION
    Compare function for QCSI client handle 

  PARAMETERS
    item_ptr : Pointer to corresponding client Q
    compare_val : Client handle to compare

  RETURN VALUE
        TRUE if Cleint handle is found in the client Q

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
int qmi_mmode_qcsi_compare_client_handle
( 
  void *item_ptr,
  void *compare_val 
)
{

  return ( (item_ptr == compare_val) ? TRUE : FALSE );
} /* qmi_mmode_qcsi_compare_client_handle */

/*===========================================================================
  FUNCTION qmi_mmode_qcsi_compare_pending_trans()

  DESCRIPTION
    Compare function for QCSI request handle 

  PARAMETERS
    item_ptr : Pointer to corresponding Pending_req Q
    compare_val : Request handle to compare

  RETURN VALUE
        TRUE if Request handle is found in the Pending_req Q

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
int qmi_mmode_qcsi_compare_pending_trans
( 
  void *item_ptr,
  void *compare_val 
)
{
  qmi_mmode_qcsi_transaction_type *req_node = (qmi_mmode_qcsi_transaction_type*)item_ptr;
  qmi_req_handle req_handle = (qmi_req_handle)compare_val;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ( (req_node->req_handle == req_handle)? TRUE : FALSE );
} /* qmi_mmode_qcsi_compare_pending_trans */

/*===========================================================================
  FUNCTION qmi_mmode_qcsi_process_raw_msg()

  DESCRIPTION
    Function to process received QMI Request and call the appropriate req handler

  PARAMETERS
    svc_ptr : Service pointer thats associated with QMI Service
    trans : Pointer to current Transaction 
    raw_msg : Pointer to raw message
    raw_msg_len : Length of raw message

  RETURN VALUE
    QMI_CSI_CB_REQ_HANDLED on SUCCESS, appropriate failure code otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_mmode_qcsi_process_raw_msg
(
  qmi_mmode_svc_info_type            *svc_ptr,
  qmi_mmode_qcsi_transaction_type    **trans,
  void                               *raw_msg,
  unsigned int                        raw_msg_len
)
{

  qmi_csi_cb_error                 ret = QMI_CSI_CB_REQ_HANDLED;
  qmi_mmode_svc_cmd_hdlr_type     *cmd_hdlr;
  uint16                           cmd;
  uint16                           msg_id = (*trans)->msg_id;
  qmi_mmode_svc_config_type       *svc_cfg;
  dsm_item_type                   *sdu_in = NULL;
  dsm_item_type                   *response_ptr = NULL;

  ASSERT(svc_ptr);
  
  svc_cfg = (qmi_mmode_svc_config_type *)svc_ptr->svc_config;

  do
  {
    if ( raw_msg_len != dsm_pushdown_packed( &sdu_in, raw_msg, raw_msg_len, DSM_DS_SMALL_ITEM_POOL) )
    {
      QM_MSG_ERROR_1("Pushdown Failed for qmi_nas_idl_message_encode %d", raw_msg_len);
      ret = QMI_CSI_CB_NO_MEM;
      break;
    }
    
    if( svc_cfg != NULL )
    {
      cmd_hdlr = svc_cfg->cmd_hdlr_array;
      for (cmd = 0; cmd < svc_cfg->cmd_num_entries; cmd++, cmd_hdlr++)
      {
        if (msg_id == cmd_hdlr->msg_id)
        {
          break; 
        }
      }
    
      if( cmd == svc_cfg->cmd_num_entries || cmd_hdlr->raw_msg_request_hdlr == NULL )
      {
        QM_MSG_MED_2("Unrecognized msg_id (=%d) for QMI svc %d Return error", msg_id, svc_ptr->service_type);
        ret = QMI_CSI_CB_UNSUPPORTED_ERR;
        break;
      }
      else
      {
    
        QM_MSG_MED_1( "Handling %s", cmd_hdlr->msg_desc);
        response_ptr = cmd_hdlr->raw_msg_request_hdlr( svc_ptr, *trans, (*trans)->clnt_info_ptr, &sdu_in );
      }
    }
    else
    {
      ret = QMI_CSI_CB_INTERNAL_ERR;
      QM_MSG_ERROR_1("no service_configuration found for service %d", svc_ptr->service_type);
      break;
    }
  
    if (response_ptr == NULL)
    {
      QM_MSG_MED("(Service generated no response)");
      ret = QMI_CSI_CB_INTERNAL_ERR;
    }
    else if (response_ptr == QMI_SVC_RESPONSE_PENDING)
    {
      QM_MSG_MED_1("Response pending for msg_id %d", msg_id);
    }
    else
    {
      if( qmi_mmode_qcsi_send_resp_raw(svc_ptr, trans, response_ptr)
            != QMI_CSI_NO_ERR )
      {
        ret = QMI_CSI_CB_INTERNAL_ERR;
      }
    }
  }while(0);

  dsm_free_packet(&sdu_in);
  return ret;
  
}/* qmi_mmode_qcsi_process_raw_msg */

/*===========================================================================
  FUNCTION qmi_mmode_qcsi_process_msg()

  DESCRIPTION
    Function to process received QMI Request and call the appropriate req handler

  PARAMETERS
    svc_ptr : Service pointer thats associated with QMI Service
    trans : Pointer to current Transaction 
    msg : Pointer to message
    msg_len : Length of message

  RETURN VALUE
    QMI_CSI_CB_REQ_HANDLED on SUCCESS, appropriate failure code otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_mmode_qcsi_process_msg(
  qmi_mmode_svc_info_type            *svc_ptr,
  qmi_mmode_qcsi_transaction_type    **trans,
  void                               *req_c_struct,
  unsigned int                        req_c_struct_len
)
{

  qmi_csi_cb_error                 ret = QMI_CSI_CB_REQ_HANDLED;
  qmi_mmode_svc_cmd_hdlr_type     *cmd_hdlr;
  uint16                           cmd;
  uint16                           msg_id = (*trans)->msg_id;
  qmi_mmode_svc_config_type       *svc_cfg;

  ASSERT(svc_ptr);
  
  svc_cfg = (qmi_mmode_svc_config_type *)svc_ptr->svc_config;
    
  if( svc_cfg != NULL )
  {
    cmd_hdlr = svc_cfg->cmd_hdlr_array;
    for (cmd = 0; cmd < svc_cfg->cmd_num_entries; cmd++, cmd_hdlr++)
    {
      if (msg_id == cmd_hdlr->msg_id)
      {
        break; 
      }
    }
  
    if( cmd == svc_cfg->cmd_num_entries || cmd_hdlr->request_hdlr == NULL )
    {
      QM_MSG_MED_2("Unrecognized msg_id (=%d) for QMI svc %d Return error", msg_id, svc_ptr->service_type);
      ret = QMI_CSI_CB_UNSUPPORTED_ERR;
    }
    else
    {
  
      QM_MSG_MED_1( "Handling %s", cmd_hdlr->msg_desc);
      ret = cmd_hdlr->request_hdlr( svc_ptr, 
                                    *trans, 
                                    (*trans)->clnt_info_ptr, 
                                    req_c_struct, 
                                    req_c_struct_len);
    }
  }
  else
  {
    ret = QMI_CSI_CB_INTERNAL_ERR;
    QM_MSG_ERROR_1("no service_configuration found for service %d", svc_ptr->service_type);
  }
  
  return ret;
  
}/* qmi_mmode_qcsi_process_raw_msg */

/*===========================================================================
  FUNCTION qmi_mmode_qcsi_svc_handle_req_cb()

  DESCRIPTION
    CallBack function to be registered with QCSI for handling raw client request

  PARAMETERS
    connection_handle : Connection handle for client
    req_handle : Handle identifying request
    msg_id : Message ID
    raw_msg : Pointer to raw message
    raw_msg_len : Length of raw message
    service_handle : Service handle

  RETURN VALUE
    QMI_CSI_CB_REQ_HANDLED on SUCCESS, appropriate failure code otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_cb_error qmi_mmode_qcsi_svc_handle_req_cb
(
  void                     *connection_handle,
  qmi_req_handle           req_handle,
  unsigned int             msg_id,
  void                    *req,
  unsigned int             req_len,
  void                     *service_handle
)
{

  void                            *clnt_info_ptr;
  qmi_mmode_svc_info_type         *svc_ptr;
  qmi_csi_cb_error                 err = QMI_CSI_CB_REQ_HANDLED;
  qmi_mmode_qcsi_transaction_type *curr_trans = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(connection_handle && service_handle);

  svc_ptr = (qmi_mmode_svc_info_type *) service_handle;

  do
  {
    if( svc_ptr == NULL || (svc_ptr->service_type != QMI_MMODE_SERVICE_NAS &&
        svc_ptr->service_type != QMI_MMODE_SERVICE_VOICE &&
        svc_ptr->service_type != QMI_MMODE_SERVICE_PSM) )
    {
      err = QMI_CSI_CB_UNSUPPORTED_ERR;
      QM_MSG_HIGH("Invalid service received");
      break;
    }
    
    clnt_info_ptr = q_linear_search( &svc_ptr->client_q,
                                     qmi_mmode_qcsi_compare_client_handle,
                                     connection_handle );
    if (NULL == clnt_info_ptr)
    {
      err = QMI_CSI_CB_CONN_REFUSED;
      break;
    }
      
    /*----------------------------------------------------------------------
       Client alloc has already been successfully processed by QMI service,
       Process the request message further
       ---------------------------------------------------------------------*/
    /* store the request to into pending requests queue */
    curr_trans = modem_mem_alloc( sizeof(qmi_mmode_qcsi_transaction_type), MODEM_MEM_CLIENT_QMI );
    if(!curr_trans)
    {
      err = QMI_CSI_CB_NO_MEM;
      break;
    }
  
    memset(curr_trans, 0, sizeof(qmi_mmode_qcsi_transaction_type));
    (void) q_link ( curr_trans, &curr_trans->link );
    curr_trans->req_handle = req_handle;
    curr_trans->msg_id = (uint16)msg_id;
    curr_trans->clnt_info_ptr = clnt_info_ptr;
    q_put( &(svc_ptr->pending_req), &(curr_trans->link) );
    
    if( svc_ptr->uses_raw_msgs == TRUE )
    {
    err = qmi_mmode_qcsi_process_raw_msg(svc_ptr,
                                       &curr_trans,
                                            req,
                                            req_len);
    }
    else
    {
      err = qmi_mmode_qcsi_process_msg(svc_ptr,
                                         &curr_trans,
                                         req,
                                         req_len);
    }

  }while(0);
  

  if (err != QMI_CSI_CB_REQ_HANDLED && err != QMI_CSI_CB_NO_ERR )
  {  
    QM_MSG_ERROR_4("qcsi_handle_req_cb: Error processing err = %d connection handle %p svc handle %p msg id %d",
                    err, connection_handle, service_handle, msg_id);

    /* Delete and Free curr_trans from queue */
    if( curr_trans != NULL )
    {
      q_delete(&(svc_ptr->pending_req), &(curr_trans->link));
      modem_mem_free((void *) curr_trans, MODEM_MEM_CLIENT_QMI);
    }
  }
  else
  {
    QM_MSG_HIGH_3("qcsi_handle_req_cb: Handled req connection handle %p req_handle %p msg id %d ",
                    connection_handle, req_handle, msg_id);
  }

  return err;
  
} /* qmi_mmode_qcsi_svc_handle_req_cb() */

/*===========================================================================
  FUNCTION qmi_mmode_qcsi_send_resp_raw()

  DESCRIPTION
    Function to send raw response QCSI framework

  PARAMETERS
    svc_ptr : Service pointer thats associated with QMI Service
    trans : Pointer to current/Pending Transaction 
    resp_ptr: DSM pointer to response to be sent

  RETURN VALUE
    QMI_CSI_NO_ERR on SUCCESS, appropriate failure code otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_error qmi_mmode_qcsi_send_resp_raw
(
  qmi_mmode_svc_info_type            *svc_ptr,
  qmi_mmode_qcsi_transaction_type     **trans,
  dsm_item_type                      *resp_ptr
  
)
{

  qmi_csi_error                    err    = QMI_CSI_NO_ERR;
  uint8                              *raw_rsp;
  uint32                              raw_rsp_len;
  qmi_mmode_qcsi_transaction_type    *pending_trans;

  ASSERT(trans);
  ASSERT(resp_ptr);

  raw_rsp_len = dsm_length_packet(resp_ptr);

  if ( raw_rsp_len > 0 )
  {

    raw_rsp = (uint8 *)modem_mem_alloc((size_t)raw_rsp_len, MODEM_MEM_CLIENT_QMI);

    if ( raw_rsp != NULL )
    {
      if ( raw_rsp_len == dsm_pullup( &resp_ptr, (void *)raw_rsp, raw_rsp_len ) )
      {

        /* Identify handle from queue of pending requests and delete from queue */
        pending_trans = q_linear_delete_new( &svc_ptr->pending_req,
                                       qmi_mmode_qcsi_compare_pending_trans,
                                       (void *)(*trans)->req_handle,
                                       NULL,
                                       NULL );
        
        if ( pending_trans != NULL )
        {
          err = qmi_csi_send_resp_raw((*trans)->req_handle,
                                  (*trans)->msg_id,
                                  (void *)raw_rsp,
                                  (unsigned int)raw_rsp_len);
        }
        else
        {
          err = QMI_CSI_INVALID_HANDLE;
        }
      }
      else
      {
        err = QMI_CSI_INTERNAL_ERR;
      }
  
      modem_mem_free( (void *)raw_rsp, MODEM_MEM_CLIENT_QMI );
    }
    else
    {
      err = QMI_CSI_NO_MEM;
    }
    
  }

  QM_MSG_HIGH_3("qmi_mmode_qcsi_send_resp_raw err = %d for msg_id = %d, service %d",err, (*trans)->msg_id, svc_ptr->service_type);

  modem_mem_free((void *) (*trans), MODEM_MEM_CLIENT_QMI);
  *trans = NULL;
  dsm_free_packet(&resp_ptr);
  
  return err;
  
}/* qmi_mmode_qcsi_send_resp_raw */

/*===========================================================================
  FUNCTION qmi_mmode_qcsi_send_ind_raw()

  DESCRIPTION
    Function to send raw response QCSI framework

  PARAMETERS
    clnt : Client handle to send the indication
    msg_id : Message ID
    ind_ptr: DSM pointer to response to be sent

  RETURN VALUE
    QMI_CSI_NO_ERR on SUCCESS, appropriate failure code otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_error qmi_mmode_qcsi_send_ind_raw
(
  qmi_client_handle                   clnt,
  unsigned int                        msg_id,
  dsm_item_type                      *ind_ptr
)
{

  qmi_csi_error                       err    = QMI_CSI_NO_ERR;
  uint8                              *raw_ind=NULL;
  uint32                              raw_ind_len=0;

  ASSERT(clnt);

  raw_ind_len = dsm_length_packet(ind_ptr);

  if ( raw_ind_len > 0 )
  {

    raw_ind = (uint8 *)modem_mem_alloc((size_t)raw_ind_len, MODEM_MEM_CLIENT_QMI);

    if ( raw_ind != NULL )
    {
      if ( raw_ind_len == dsm_pullup( &ind_ptr, (void *)raw_ind, raw_ind_len ) )
      {
        err = qmi_csi_send_ind_raw(clnt,
                                   msg_id,
                                  (void *)raw_ind,
                                  (unsigned int)raw_ind_len);
      }
      else
      {
        err = QMI_CSI_DECODE_ERR;
      }
  
      modem_mem_free( (uint8*)raw_ind, MODEM_MEM_CLIENT_QMI );
    }
    else
    {
      err = QMI_CSI_NO_MEM;
    }
    
  }
  else
  {
    err = qmi_csi_send_ind_raw(clnt,
                             msg_id,
                            (void *)raw_ind,
                            (unsigned int)raw_ind_len);

  }

  QM_MSG_HIGH_3("qmi_mmode_qcsi_send_ind_raw err = %d for msg_id = %d, clnt %p",err, msg_id, clnt);

  dsm_free_packet(&ind_ptr);
  
  return err;
  
}/* qmi_mmode_qcsi_send_ind_raw */

/*===========================================================================
  FUNCTION qmi_mmode_qcsi_send_resp()

  DESCRIPTION
  sends response using QCSI API.
    
  PARAMETERS
    req_handle         :       Request Handle
    clnt_info          :       Client Info Handle
    msg_id             :       Message ID
    c_struct_len       :       length of response


  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_error qmi_mmode_qcsi_send_resp
(
  qmi_mmode_svc_info_type            *svc_ptr,
  qmi_mmode_qcsi_transaction_type     **trans,
  void*                               resp_msg,
  unsigned int                        resp_len
)
{
  qmi_csi_error                       err = QMI_CSI_NO_ERR;
  qmi_mmode_qcsi_transaction_type    *pending_trans;

  /* Identify handle from queue of pending requests and delete from queue */
  pending_trans = q_linear_delete_new( &svc_ptr->pending_req,
                                 qmi_mmode_qcsi_compare_pending_trans,
                                 (void *)(*trans)->req_handle,
                                 NULL,
                                 NULL );
  
  if ( pending_trans != NULL )
  {
    err = qmi_csi_send_resp((*trans)->req_handle,
                            (*trans)->msg_id,
                            resp_msg,
                            resp_len);
  }
  else
  {
    err = QMI_CSI_INVALID_HANDLE;
  }
        
  QM_MSG_HIGH_2("qmi_mmode_qcsi_send_resp err = %d for msg_id = %d",err, (*trans)->msg_id);
  modem_mem_free((void *) (*trans), MODEM_MEM_CLIENT_QMI);
  *trans = NULL;
  
  return err;
}

/*===========================================================================
  FUNCTION qmi_mmode_qcsi_send_ind()

  DESCRIPTION
  sends response using QCSI API.
    
  PARAMETERS
    req_handle         :       Request Handle
    clnt_info          :       Client Info Handle
    msg_id             :       Message ID
    c_struct_len       :       length of response


  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_error qmi_mmode_qcsi_send_ind
(
  qmi_client_handle                   clnt,
  unsigned int                        msg_id,
  void*                               ind_msg,
  unsigned int                        ind_len
)
{
  qmi_csi_error err = QMI_CSI_NO_ERR;
  
  err = qmi_csi_send_ind(clnt, msg_id, ind_msg, ind_len);
  QM_MSG_HIGH_2("qmi_mmode_qcsi_send_ind err = %d for msg_id = %d",err, msg_id);

  return err;
}

/*===========================================================================
  FUNCTION qmi_mmode_qcsi_event_hdlr_cb()

  DESCRIPTION
    CallBack function invoked when signal is set by QCSI

  PARAMETERS
    sig : Signal set
    svc_ptr : Service pointer (user data ptr) thats associated with it

  RETURN VALUE
    TRUE - SUCCESS

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_mmode_qcsi_event_hdlr_cb
(
  qmi_mmode_sig_enum_type      sig,
  void                        *svc_ptr
)
{
  qmi_mmode_svc_info_type *svc_info = NULL;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( (sig == QMI_MMODE_QCSI_NAS_EVENT_SIGNAL) || 
          (sig == QMI_MMODE_QCSI_VOICE_EVENT_SIGNAL) ||
          (sig == QMI_MMODE_QCSI_PSM_EVENT_SIGNAL) );
  ASSERT( svc_ptr );

  svc_info = (qmi_mmode_svc_info_type *)svc_ptr;

  qmi_csi_handle_event(svc_info->service_handle, &svc_info->os_params);
  return TRUE;
} /* qmux_qcsi_shim_msg_hdlr_cb */

/*===========================================================================
  FUNCTION qmi_mmode_svc_qcsi_register()

  DESCRIPTION
    CallBack function invoked when signal is set by QCSI

  PARAMETERS
    service : QMI service ID
    service_connect : CallBack function to be registered with QCSI for connecting a client
    service_disconnect: CallBack function to be registered with QCSI for disconnecting a client
    service_process_req: CallBack function to be registered with QCSI for handling raw client request
   service_cookie: Service pointer thats associated with QMI service
   
  RETURN VALUE
    QMI_CSI_NO_ERR - SUCCESS

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_error qmi_mmode_svc_qcsi_register
(
  qmi_mmode_service_e_type                  service,
  qmi_csi_connect                           service_connect,
  qmi_csi_disconnect                        service_disconnect,
  qmi_csi_process_req                       service_process_req,
  qmi_mmode_svc_info_type                   *service_cookie
)
{

  qmi_csi_error                 rc;
  qmi_csi_options               options;
  qmi_idl_service_object_type   service_obj;

  ASSERT(service_cookie);

  /* Service Obj related information */
  if( service == QMI_MMODE_SERVICE_NAS )
  {
    service_obj = nas_get_service_object_v01();
    service_cookie->svc_sig = QMI_MMODE_QCSI_NAS_EVENT_SIGNAL;
  }
  else if ( service == QMI_MMODE_SERVICE_PSM )
  {
    service_obj = psm_get_service_object_v01();
    service_cookie->svc_sig = QMI_MMODE_QCSI_VOICE_EVENT_SIGNAL;
  }
  else/* if ( service == QMI_MMODE_SERVICE_VOICE ) commenting to silence KW */
  {
    service_obj = voice_get_service_object_v02();
    service_cookie->svc_sig = QMI_MMODE_QCSI_PSM_EVENT_SIGNAL;
  }

  /* os_params for QCSI to raise a signal */
  service_cookie->os_params.tcb = get_cm_tcb();
  service_cookie->os_params.sig = ((rex_sigs_type)(1 << (rex_sigs_type)service_cookie->svc_sig));
  
  /*-------------------------------------------------------------------------
    Setting QMI MMODE TASK signal handler for handling QCSI messages
  -------------------------------------------------------------------------*/

  (void)qmi_mmode_set_sig_handler(service_cookie->svc_sig, 
                                  qmi_mmode_qcsi_event_hdlr_cb, 
                                  service_cookie);

    
  qmi_mmode_enable_sig(service_cookie->svc_sig);
  
  /*-------------------------------------------------------------------------
  Register QMI service with QCSI with option to handle message raw and 
  asynchronously inform result of client connect 
  -------------------------------------------------------------------------*/
  QMI_CSI_OPTIONS_INIT(options);
  if( service_cookie->uses_raw_msgs == TRUE )
  {
    QMI_CSI_OPTIONS_SET_PRE_REQUEST_CB(options, service_process_req);
  }
  (void)mqcsi_register_log_options(&options);
  rc = qmi_csi_register_with_options(service_obj,
                        service_connect,
                        service_disconnect, 
                        service_process_req, 
                        (void *)service_cookie, 
                        &service_cookie->os_params,
                        &options, 
                        &service_cookie->service_handle);  

  if( rc != QMI_CSI_NO_ERR )
  {
    ERR_FATAL("Unable to register QCSI service %d Error %d", service, rc, 0);
  }
  else
  {
    QM_MSG_HIGH_2("QMI service %d registered with QCSI, service_handle %p",
        service, service_cookie->service_handle);
  }

  /* Initialize the queue which will hold all handles of clients */
  (void)q_init(&service_cookie->client_q);
  /* Initialize the queue which will hold requests for which the resp is pending */
  (void)q_init(&service_cookie->pending_req);

  return rc;
  
}

