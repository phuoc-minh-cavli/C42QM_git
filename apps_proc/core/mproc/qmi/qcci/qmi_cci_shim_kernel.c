/*=============================================================================

  Q M I   C L I E N T   S H I M   M O D U L E   -   K E R N E L   S P A C E 
              
GENERAL DESCRIPTION
  Implements kernel handler functions for QMI Client User space module


REGIONAL FUNCTIONS 
  None
  
EXTERNALIZED FUNCTIONS
  qmi_client_shim_init - Should be called before any other function call
    
INITIALIZATION AND SEQUENCING REQUIREMENTS
  qmi_client_shim_init - Should be called before any other function call

Copyright (c) 2017, 2020 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/mproc/qmi/qcci/qmi_cci_shim_kernel.c#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------

01/23/17   ab      File created.

=============================================================================*/

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/

#include "qmi_common.h"
#include "qmi_client.h"
#include "qmi_cci_target.h"
#include "qmi_client_shim.h"
#include "qmi_cci_shim_kernel.h"
#include "qapi_txm_base.h"
#include "qurt_txm_qapi_fwk.h"
#include "qmi_txm_shim.h"
#include "timer.h"

/*===========================================================================
                      MACROS AND DEFINES
===========================================================================*/
#define INVALID_HANDLE 0

/*===========================================================================
                      GLOBAL VARIABLES
===========================================================================*/
LIST(qmi_client_user_module_handle, qmi_cci_shim_module_handle_list);
qmi_cci_lock_type qmi_cci_shim_module_handle_list_lock;
volatile int deinit_timeout = 10;


/*===========================================================================
                      LOCAL FUNCTIONS DECLARATIONS
===========================================================================*/
qmi_client_error_type qmi_cci_shim_free_client_obj(
      qmi_client_shim_handle_type               *qmi_shim_handle
);

qmi_client_error_type qmi_cci_shim_alloc_client_obj(
      qmi_client_user_module_handle             *handle,
      qmi_idl_service_object_type               service_obj,
      qmi_client_tx_os_params                   *tx_os_params,
      qmi_client_shim_handle_type               **shim_handle
);

/*===========================================================================
                      LOCAL FUNCTIONS
===========================================================================*/

/*===========================================================================
  FUNCTION  qmi_cci_shim_get_buf
  Description: Helper function to retrieve buffer from kernel space to user space.
  The kernel buffer passed should be first placed in msg_q of qmi channel using
  qmi_cci_shim_copy_put_buf before calling this api.
===========================================================================*/
int qmi_cci_shim_get_buf(qmi_client_user_module_handle *module_handle, qmi_client_shim_handle_type* qmi_shim_handle,
                      void *kernel_buf, uint32 kernel_buf_len,
                      void *user_buf, uint32 user_buf_len)
  {
    qmi_client_shim_buf_type *iter;
    
    if(qmi_shim_handle == NULL || qmi_shim_handle->magic != QMI_CLIENT_SHIM_MAGIC)
    {
      QMI_CCI_LOG_ERR("qmi_shim_handle(0x%x) is NULL or invalid", qmi_shim_handle, 0, 0);
      return QMI_CLIENT_INVALID_CLNT;
    }
    
    //Check if the provided kernel buf addr is present in our buf_q
    LOCK(&qmi_shim_handle->mutex);
    
    LIST_FIND(qmi_shim_handle->buf_q, iter, link, (iter == kernel_buf && iter->len == kernel_buf_len));
    
    if(iter == NULL)
    {
      QMI_CCI_LOG_ERR("qmi_cci_shim_get_buf: couldn't get the buf link", (uint32)kernel_buf, kernel_buf_len, 0);
      return 1; //Todo: return proper err code
    }
  
    //Remove iter from list
    LIST_REMOVE(qmi_shim_handle->buf_q, iter, link);
    
    UNLOCK(&qmi_shim_handle->mutex);
    
    //Note: This may need to change if we need to use special api to access user memory
    QMI_MEM_COPY(user_buf, user_buf_len, iter->buf, iter->len);
    
    //Free the buffer
    FREE(iter);
    
    return 0;
  } /* qmi_cci_shim_get_buf */

/*===========================================================================
  FUNCTION  qmi_cci_shim_copy_put_buf
  Description: Does local copy of buffer and queues into qmi channel's msg q.
  The buf struct address is returned which is passed to user space.
  Then user space needs to allocate memory of same size and jumps into kernel
  to copy the data to user space memory.
===========================================================================*/
qmi_client_error_type qmi_cci_shim_copy_put_buf(qmi_client_shim_handle_type *qmi_shim_handle, void *src_buf, int buf_len, void **dest_buf)
{
  qmi_client_shim_buf_type *buf_link;
  
  buf_link = MALLOC(sizeof(qmi_client_shim_buf_type) + buf_len);
  if(buf_link == NULL)
  {
    QMI_CCI_LOG_ERR("copy_put_buf alloc failed for %d bytes", sizeof(qmi_client_shim_buf_type) + buf_len, qmi_shim_handle, 0);
    return QMI_CLIENT_ALLOC_FAILURE;
  }

  //take backup of ind_buf for user space to read and return
  LINK_INIT(buf_link->link);
  buf_link->buf = (uint8 *)buf_link + sizeof(qmi_client_shim_buf_type);
  buf_link->len = buf_len;
  
  //copy the data to buffer
  QMI_MEM_COPY(buf_link->buf, buf_len, src_buf, buf_len);
  
  //Add it to buf_q
  LOCK(&qmi_shim_handle->mutex);
  LIST_ADD(qmi_shim_handle->buf_q, buf_link, link);
  UNLOCK(&qmi_shim_handle->mutex);

  *dest_buf = (void *)buf_link;
  
  return QMI_NO_ERR;
} /* qmi_cci_shim_copy_put_buf */


/*===========================================================================
                      CALLBACK HANDLERS
===========================================================================*/

/*===========================================================================
  FUNCTION  qmi_cci_shim_callback_handler
  Description: Helper function to post callback details to MsgQ for user thread
               to read and call in user space
===========================================================================*/
static void qmi_cci_shim_callback_handler(qmi_client_user_module_handle *module_handle, qmi_cci_shim_cb_type cb_type, void *cb, 
                                          ULONG a, ULONG b, ULONG c, ULONG d, ULONG e, ULONG f, ULONG g, ULONG h, ULONG i, ULONG j)
{
  qmi_cci_shim_cb_info cb_info;
  UINT status;
  
  cb_info.cb_type = cb_type;
  cb_info.cb = cb;
  cb_info.a = a;
  cb_info.b = b;
  cb_info.c = c;
  cb_info.d = d;
  cb_info.e = e;
  cb_info.f = f;
  cb_info.g = g;
  cb_info.h = h;
  cb_info.i = i;
  cb_info.j = j;
  
  //Wait for 10 timer ticks before returning.. which happens when
  // queue is full case otherwise returns immediately.
  status = tx_queue_send(&module_handle->msg_q, &cb_info, 10);
  if(status != TX_SUCCESS)
  {
    QMI_CCI_LOG_ERR("tx_queue_send failed with status %d for module_handle: 0x%x, cb: 0x%x", status, (uint32)module_handle, (uint32)cb);
  }
  
  return;
} /* qmi_cci_shim_callback_handler */


/*===========================================================================
  FUNCTION  qmi_cci_shim_get_callback_details
  Description: Helper function where user space thread will call into and 
               reads callback details from MsgQ and calls them in user space
===========================================================================*/
UINT qmi_cci_shim_get_callback_details(qmi_client_user_module_handle *module_handle, qmi_cci_shim_cb_info *cb_info)
{
  UINT status ;
  
  if(module_handle->magic != QMI_SHIM_MODULE_COOKIE)
    return TX_DELETED; //In case module got deinited and later trying to get cb details
  
  status = tx_queue_receive(&module_handle->msg_q, cb_info, TX_WAIT_FOREVER);
   if( TX_SUCCESS != status)
   {  
     QMI_CCI_LOG_ERR("tx_queue_receive failed with status %d for module 0x%x", status, (uint32)module_handle, 0);
     return status;
   }

  return status; 
} /* qmi_cci_shim_get_callback_details */


/*===========================================================================
  FUNCTION  qmi_cci_shim_notify_cb_handler
  Description: Notify callback helper function that passes qmi clients 
               notify_cb cb details to user space
===========================================================================*/
void qmi_cci_shim_notify_cb_handler
(
 qmi_client_type                user_handle,
 qmi_idl_service_object_type    service_obj,
 qmi_client_notify_event_type   service_event,
 void                           *notify_cb_data
)
{
  qmi_client_shim_handle_type *qmi_shim_handle = (qmi_client_shim_handle_type *)notify_cb_data;
  qmi_client_notify_cb notify_cb;
  
  if(qmi_shim_handle->qmi_handle != user_handle)
  {
    ERR_FATAL("qmi_handle should match", qmi_shim_handle->qmi_handle, user_handle, 0);
    return;
  }
  
  //Copy details to qmi_shim_handle
  LOCK(&qmi_shim_handle->mutex);
  qmi_shim_handle->last_service_event = service_event;

  //Make a note now to call notify_cb of client if they register for notify_cb in future
  notify_cb = qmi_shim_handle->notify_cb;
  if(notify_cb == NULL)
    qmi_shim_handle->notify_received = 1;
  UNLOCK(&qmi_shim_handle->mutex);
  
  //Signal the client via callback handler
  //QMI_CCI_TX_OS_SIGNAL_SET(qmi_shim_handle->tx_os_params);
  if(qmi_shim_handle->tx_os_params->ext_signal)
    qmi_cci_shim_callback_handler(qmi_shim_handle->module_handle, QMI_CCI_SIGNAL_TYPE_CB, (void *)qmi_shim_handle->tx_os_params->ext_signal, (ULONG)qmi_shim_handle->tx_os_params->sig, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  
  //call client's notify_cb with qmi shim handle
  //if(notify_cb)
    //notify_cb(qmi_shim_handle, service_obj, service_event, qmi_shim_handle->notify_cb_data);
  if(notify_cb)
    qmi_cci_shim_callback_handler(qmi_shim_handle->module_handle, QMI_CCI_NOTIFY_CB, notify_cb, (ULONG)qmi_shim_handle, (ULONG)service_obj, (ULONG)service_event, (ULONG)qmi_shim_handle->notify_cb_data, 0, 0, 0, 0, 0, 0);
  
  return;
} /* qmi_cci_shim_notify_cb_handler */


/*===========================================================================
  FUNCTION  qmi_cci_shim_log_cb_handler
  Description: Log callback helper function that passes qmi clients log_cb
               cb details to user space
===========================================================================*/
void qmi_cci_shim_log_cb_handler
(
 qmi_client_type                user_handle,
 qmi_idl_type_of_message_type   msg_type,
 unsigned int                   msg_id,
 unsigned int                   txn_id,
 const void                     *raw_msg,
 unsigned int                   raw_msg_len,
 qmi_client_error_type          status,
 void                           *log_cb_data
)
{
  qmi_client_shim_handle_type *qmi_shim_handle = (qmi_client_shim_handle_type *)log_cb_data;
  qmi_client_error_type err = QMI_NO_ERR;
  void *shim_buf = NULL;
  
  if(qmi_shim_handle->qmi_handle != user_handle)
  {
    ERR_FATAL("qmi_handle should match", qmi_shim_handle->qmi_handle, user_handle, 0);
  }

  if(raw_msg_len != 0)
  {
    err = qmi_cci_shim_copy_put_buf(qmi_shim_handle, (void *)raw_msg, raw_msg_len, &shim_buf);
    if(err != QMI_NO_ERR)
    {
      //todo: Check if we can anyway inform to client about this packet.. if its required?
      QMI_CCI_LOG_ERR("copy_put_buf failed with err %d", err, qmi_shim_handle, 0);
      //Null will be sent in this case, atleast to notify client something
      shim_buf = NULL;
      raw_msg_len = 0;
    }
  }
  
  //call client's log_cb with qmi shim handle
  //if(qmi_shim_handle->log_cb)
    //qmi_shim_handle->log_cb(qmi_shim_handle, msg_type, msg_id, txn_id, raw_msg, raw_msg_len, status, qmi_shim_handle->log_cb_data);
  
  if(qmi_shim_handle->log_cb)
    qmi_cci_shim_callback_handler(qmi_shim_handle->module_handle, QMI_CCI_LOG_CB, qmi_shim_handle->log_cb, (ULONG)qmi_shim_handle, (ULONG)msg_type, (ULONG)msg_id, (ULONG)txn_id, (ULONG)shim_buf, (ULONG)raw_msg_len, (ULONG)status, (ULONG)qmi_shim_handle->log_cb_data, 0, 0);
  
  return;
} /* qmi_cci_shim_log_cb_handler */


/*===========================================================================
  FUNCTION  qmi_cci_shim_ind_cb_handler
  Description: Indication callback helper function that passes qmi clients 
               ind_cb cb details to user space
===========================================================================*/
void qmi_cci_shim_ind_cb_handler
(
 qmi_client_type                user_handle,
 unsigned int                   msg_id,
 void                           *ind_buf,
 unsigned int                   ind_buf_len,
 void                           *ind_cb_data
)
{
  qmi_client_shim_handle_type *qmi_shim_handle = (qmi_client_shim_handle_type *)ind_cb_data;
  qmi_client_error_type err = QMI_NO_ERR;
  void *shim_buf = NULL;
  
  if(qmi_shim_handle->qmi_handle != user_handle)
  {
    ERR_FATAL("qmi_handle should match", qmi_shim_handle->qmi_handle, user_handle, 0);
  }
  
  if(ind_buf_len != 0)
  {
    err = qmi_cci_shim_copy_put_buf(qmi_shim_handle, ind_buf, ind_buf_len, &shim_buf);
    if(err != QMI_NO_ERR)
    {
      //todo: Check if we can anyway inform to client about this packet.. if its required?
      QMI_CCI_LOG_ERR("copy_put_buf failed with err %d", err, qmi_shim_handle, 0);
      //Null will be sent in this case, atleast to notify client something
      shim_buf = NULL;
      ind_buf_len = 0;
    }
  }
  
  //call client's ind_cb with qmi shim handle
  //if(qmi_shim_handle->ind_cb)
    //qmi_shim_handle->ind_cb(qmi_shim_handle, msg_id, ind_buf, ind_buf_len, qmi_shim_handle->ind_cb_data);
  
  if(qmi_shim_handle->ind_cb)
    qmi_cci_shim_callback_handler(qmi_shim_handle->module_handle, QMI_CCI_INDICATION_CB, qmi_shim_handle->ind_cb, (UINT)qmi_shim_handle, (UINT)msg_id, (UINT)shim_buf, (UINT)ind_buf_len, (UINT)qmi_shim_handle->ind_cb_data, 0, 0, 0, 0, 0);
  
  return;
} /* qmi_cci_shim_ind_cb_handler */


/*===========================================================================
  FUNCTION  qmi_cci_shim_err_cb_handler
  Description: Error callback helper function that passes qmi clients 
               err_cb cb details to user space
===========================================================================*/
void qmi_cci_shim_err_cb_handler
(
 qmi_client_type        user_handle,
 qmi_client_error_type  error,
 void                  *err_cb_data
)
{
  qmi_client_shim_handle_type *qmi_shim_handle = (qmi_client_shim_handle_type *)err_cb_data;
  
  if(qmi_shim_handle->qmi_handle != user_handle)
  {
    ERR_FATAL("qmi_handle should match", qmi_shim_handle->qmi_handle, user_handle, 0);
  }
  
  //call client's err_cb with qmi shim handle
  //if(qmi_shim_handle->err_cb)
   //qmi_shim_handle->err_cb(qmi_shim_handle, error, qmi_shim_handle->err_cb_data);
  
  if(qmi_shim_handle->err_cb)
    qmi_cci_shim_callback_handler(qmi_shim_handle->module_handle, QMI_CCI_ERR_CB, qmi_shim_handle->err_cb, (UINT)qmi_shim_handle, (UINT)error, (UINT)qmi_shim_handle->err_cb_data, 0, 0, 0, 0, 0, 0, 0);
  
  return;
} /* qmi_cci_shim_err_cb_handler */


/*===========================================================================
  FUNCTION  qmi_cci_shim_release_cb_handler
  Description: Release callback helper function that passes qmi clients 
               release_cb cb details to user space
===========================================================================*/
void qmi_cci_shim_release_cb_handler
(
 void   *release_cb_data
)
{
  qmi_client_shim_handle_type *qmi_shim_handle = (qmi_client_shim_handle_type *)release_cb_data;
  qmi_client_user_module_handle *handle = qmi_shim_handle->module_handle;
  qmi_client_error_type err = QMI_NO_ERR;
  
  if(qmi_shim_handle->release_cb)
    qmi_cci_shim_callback_handler(qmi_shim_handle->module_handle, QMI_CCI_RELEASE_CB, qmi_shim_handle->release_cb, (UINT)qmi_shim_handle->release_cb_data, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  
 //remove the client handle from shim client list
 LOCK(&handle->clnt_list_lock);
 LIST_REMOVE(handle->clnt_list, qmi_shim_handle, link);
 UNLOCK(&handle->clnt_list_lock);
 
 err = qmi_cci_shim_free_client_obj(qmi_shim_handle);
 if(err != QMI_NO_ERR)
 {
   QMI_CCI_LOG_ERR("free_client_obj failed in release_cb_handler with err %d for handle 0x%x", err, (uint32)qmi_shim_handle, 0);
 }
  
  return;
} /* qmi_cci_shim_release_cb_handler */


/*===========================================================================
  FUNCTION  qmi_cci_shim_raw_msg_async_cb_handler
  Description: Raw msg async callback helper function that passes qmi clients 
               raw_msg_async_cb cb details to user space
===========================================================================*/
void qmi_cci_shim_raw_msg_async_cb_handler
(
 qmi_client_type                user_handle,
 unsigned int                   msg_id,
 void                           *resp_buf,
 unsigned int                   resp_buf_len,
 void                           *resp_cb_data,
 qmi_client_error_type          transp_err
)
{
  qmi_client_shim_handle_type *qmi_shim_handle = (qmi_client_shim_handle_type *)resp_cb_data;
  
  if(qmi_shim_handle->qmi_handle != user_handle)
  {
    ERR_FATAL("qmi_handle should match", qmi_shim_handle->qmi_handle, user_handle, 0);
  }
  
  //call client's raw_msg_async_cb with qmi shim handle
  //if(qmi_shim_handle->raw_msg_async_cb)
    //qmi_shim_handle->raw_msg_async_cb(qmi_shim_handle, msg_id, resp_buf, resp_buf_len, qmi_shim_handle->raw_msg_async_cb_data, transp_err);
  
  if(qmi_shim_handle->raw_msg_async_cb)
    qmi_cci_shim_callback_handler(qmi_shim_handle->module_handle, QMI_CCI_RAW_MSG_ASYNC_CB, qmi_shim_handle->raw_msg_async_cb, (UINT)qmi_shim_handle, (UINT)msg_id, (UINT)resp_buf, (UINT)resp_buf_len, (UINT)qmi_shim_handle->raw_msg_async_cb_data, (UINT)transp_err, 0, 0, 0, 0);
  
  return;
} /* qmi_cci_shim_raw_msg_async_cb_handler */


/*===========================================================================
  FUNCTION  qmi_cci_shim_struct_msg_async_cb_handler
  Description: Struct msg async callback helper function that passes qmi clients 
               struct_msg_async_cb cb details to user space
===========================================================================*/
void qmi_cci_shim_struct_msg_async_cb_handler
(
 qmi_client_type                user_handle,
 unsigned int                   msg_id,
 void                           *resp_c_struct,
 unsigned int                   resp_c_struct_len,
 void                           *resp_cb_data,
 qmi_client_error_type          transp_err
)
{
  qmi_client_shim_handle_type *qmi_shim_handle = (qmi_client_shim_handle_type *)resp_cb_data;
  
  if(qmi_shim_handle->qmi_handle != user_handle)
  {
    ERR_FATAL("qmi_handle should match", qmi_shim_handle->qmi_handle, user_handle, 0);
  }
  
  //call client's struct_msg_async_cb with qmi shim handle
  //if(qmi_shim_handle->struct_msg_async_cb)
    //qmi_shim_handle->struct_msg_async_cb(qmi_shim_handle, msg_id, resp_c_struct, resp_c_struct_len, qmi_shim_handle->struct_msg_async_cb_data, transp_err);
  
  if(qmi_shim_handle->struct_msg_async_cb)
    qmi_cci_shim_callback_handler(qmi_shim_handle->module_handle, QMI_CCI_STRUCT_MSG_ASYNC_CB, qmi_shim_handle->struct_msg_async_cb, (UINT)qmi_shim_handle, (UINT)msg_id, (UINT)resp_c_struct, (UINT)resp_c_struct_len, (UINT)qmi_shim_handle->struct_msg_async_cb_data, (UINT)transp_err, 0, 0, 0, 0);
  
  return;
} /* qmi_cci_shim_struct_msg_async_cb_handler */



/*===========================================================================
  FUNCTION  qmi_cci_shim_alloc_client_obj
  Description: Allocates QMI Client Shim struct and fills required details
===========================================================================*/
qmi_client_error_type qmi_cci_shim_alloc_client_obj(
      qmi_client_user_module_handle             *handle,
      qmi_idl_service_object_type               service_obj,
      qmi_client_tx_os_params                   *tx_os_params,
      qmi_client_shim_handle_type               **shim_handle
)
{
  qmi_client_os_params *os_params;
  qmi_client_shim_handle_type *qmi_shim_handle;
  qmi_client_error_type err;
  
  os_params = CALLOC(1, sizeof(qmi_client_os_params));
  if(os_params == NULL)
  {
    QMI_CCI_LOG_ERR("Couldnt allocate %d bytes for os_params",sizeof(qmi_client_os_params), 0, 0 );
    return QMI_CLIENT_ALLOC_FAILURE;
  }
  
  //initialize os_params
  os_params->ext_signal = CALLOC(1, sizeof(qurt_signal_t));
  if(os_params->ext_signal == NULL)
  {
    FREE(os_params);
    QMI_CCI_LOG_ERR("Couldnt allocate %d bytes for qurt sig obj",sizeof(qurt_signal_t), 0, 0 );
    return QMI_CLIENT_ALLOC_FAILURE;
  }
  
  qurt_signal_init(os_params->ext_signal);
  
  //Currently os_params signal is not been waited anywhere and thus no use
  if(tx_os_params != NULL)
  {
    os_params->sig = tx_os_params->sig; 
    os_params->timer_sig = tx_os_params->timer_sig;
  }
  else
  {
    os_params->sig = 0; 
    os_params->timer_sig = 0;
  }
  
  qmi_shim_handle = CALLOC(1, sizeof(qmi_client_shim_handle_type));
  if(qmi_shim_handle == NULL)
  {
    qurt_signal_destroy(os_params->ext_signal);
    FREE(os_params->ext_signal);
    FREE(os_params);
    QMI_CCI_LOG_ERR("Couldnt allocate %d bytes for qmi_shim_handle",sizeof(qmi_client_shim_handle_type), 0, 0 );
    return QMI_CLIENT_ALLOC_FAILURE;
  }
  
  qmi_shim_handle->magic = QMI_CLIENT_SHIM_MAGIC;
  qmi_shim_handle->tx_os_params = tx_os_params;
  qmi_shim_handle->os_params = os_params;
  qmi_shim_handle->module_handle = handle;
  qmi_shim_handle->notify_received = 0;
  qmi_shim_handle->service_obj = service_obj;
  
  LOCK_INIT(&qmi_shim_handle->mutex);
  
  *shim_handle = qmi_shim_handle;
  
  return QMI_NO_ERR;
} /* qmi_cci_shim_alloc_client_obj */

/*===========================================================================
  FUNCTION  qmi_cci_shim_free_client_obj
  Description: Cleansup and Frees QMI Client Shim struct
===========================================================================*/
qmi_client_error_type qmi_cci_shim_free_client_obj(
      qmi_client_shim_handle_type               *qmi_shim_handle
)
{
  qmi_client_shim_buf_type *iter;

  qmi_shim_handle->magic = 0;
  
  //Free memory entries in buf_q
  LOCK(&qmi_shim_handle->mutex);
  iter = LIST_HEAD(qmi_shim_handle->buf_q);
  while(iter)
  {
    LIST_REMOVE(qmi_shim_handle->buf_q, iter, link);
    FREE(iter);
    iter = LIST_HEAD(qmi_shim_handle->buf_q);
  }
  UNLOCK(&qmi_shim_handle->mutex);
  
  LOCK_DEINIT(&qmi_shim_handle->mutex);
  qurt_signal_destroy(qmi_shim_handle->os_params->ext_signal);
  FREE(qmi_shim_handle->os_params->ext_signal);
  FREE(qmi_shim_handle->os_params);
  FREE(qmi_shim_handle);
  
  return QMI_NO_ERR;
} /* qmi_cci_shim_free_client_obj */


/*===========================================================================
                    SHIM FUNCTIONS
===========================================================================*/
/*===========================================================================
  FUNCTION  _qmi_client_notifier_init
  DESCRIPTION: Notifier init shim function
===========================================================================*/
qmi_client_error_type _qmi_client_notifier_init(
      qmi_client_user_module_handle             *handle,
      qmi_idl_service_object_type               service_obj,
      qmi_client_tx_os_params                   *tx_os_params,
      qmi_client_type                           *user_handle
)
{
  qmi_client_shim_handle_type *qmi_shim_handle;
  qmi_client_error_type err;
  
  if(user_handle == NULL)
  {
    QMI_CCI_LOG_ERR("user_handle is NULL", 0, 0, 0);
    return QMI_CLIENT_PARAM_ERR;
  }
  
  err = qmi_cci_shim_alloc_client_obj(handle, service_obj, tx_os_params, &qmi_shim_handle);
  if(err != QMI_NO_ERR)
  {
    QMI_CCI_LOG_ERR("qmi_cci_shim_alloc_client_obj failed with err %d", err, (uint32)service_obj, 0);
    return err;
  }
  
  //Call actual qmi_client_notifier_init for registering for qmi notifier
  err = qmi_client_notifier_init(service_obj, qmi_shim_handle->os_params, &qmi_shim_handle->qmi_handle);
  if(err != QMI_NO_ERR)
  {
    QMI_CCI_LOG_ERR("qmi_client_notifier_init failed with err %d", err, (uint32)service_obj, 0);
    qmi_cci_shim_free_client_obj(qmi_shim_handle);
    return err;
  }

  //Rest of Initialisation
  qmi_shim_handle->category = QMI_CCI_NOTIFIER_CLIENT;
  LINK_INIT(qmi_shim_handle->link);
  
  //Put new qmi_shim_handle to module clnt_list
  LOCK(&handle->clnt_list_lock);
  LIST_ADD(handle->clnt_list, qmi_shim_handle, link);
  UNLOCK(&handle->clnt_list_lock);
  
  /* Need to register notify callback even if client registers or not for notify_cb,
     as we will signal to tx sig obj from user space on server coming up. */
  err = qmi_client_register_notify_cb(qmi_shim_handle->qmi_handle, qmi_cci_shim_notify_cb_handler, (void *)qmi_shim_handle);
  if(err != QMI_NO_ERR)
  {
    QMI_CCI_LOG_ERR("qmi_client_register_notify_cb failed with err %d", err, (uint32)service_obj, 0);
    _qmi_client_release(handle, qmi_shim_handle);
    return err;
  }
  
  *user_handle = (qmi_client_type) qmi_shim_handle;
  
  return err;
} /* _qmi_client_notifier_init */


/*===========================================================================
  FUNCTION  _qmi_client_get_service_list
  DESCRIPTION: Get Service list shim function
===========================================================================*/
qmi_client_error_type _qmi_client_get_service_list
(
 qmi_client_user_module_handle     *module_handle,
 qmi_idl_service_object_type        service_obj,
 qmi_service_info                  *service_info_array,
 unsigned int                      *num_entries,
 unsigned int                      *num_services
)
{
  return qmi_client_get_service_list(service_obj, service_info_array, num_entries, num_services);
} /* _qmi_client_get_service_list */


/*===========================================================================
  FUNCTION  _qmi_client_get_service_instance
  DESCRIPTION: Get Service instance shim function
===========================================================================*/
qmi_client_error_type _qmi_client_get_service_instance
(
 qmi_client_user_module_handle    *module_handle,
 qmi_idl_service_object_type       service_obj,
 qmi_service_instance              instance_id,
 qmi_service_info                 *service_info
)
{
  return qmi_client_get_service_instance(service_obj, instance_id, service_info);
} /* _qmi_client_get_service_instance */


/*===========================================================================
  FUNCTION  _qmi_client_get_instance_id
  DESCRIPTION: Get Instance ID shim function
===========================================================================*/
qmi_client_error_type _qmi_client_get_instance_id
(
 qmi_client_user_module_handle *module_handle,
 qmi_service_info              *service_info,
 qmi_service_instance          *instance_id
)
{
  return qmi_client_get_instance_id(service_info, instance_id);
} /* _qmi_client_get_instance_id */

/*===========================================================================
  FUNCTION  _qmi_client_init
  DESCRIPTION: Qmi Client Init shim function
===========================================================================*/
qmi_client_error_type _qmi_client_init(
                            qmi_client_user_module_handle             *handle,
                            qmi_service_info                          *service_info,
                            qmi_idl_service_object_type               service_obj,
                            qmi_client_ind_cb                         ind_cb,
                            void                                      *ind_cb_data,
                            qmi_client_tx_os_params                   *tx_os_params,
                            qmi_client_type                           *user_handle)
{
  qmi_client_shim_handle_type *qmi_shim_handle = NULL;
  qmi_client_error_type err = QMI_NO_ERR;

  if(user_handle == NULL)
  {
    QMI_CCI_LOG_ERR("user_handle is NULL in _qmi_client_init", 0, 0, 0);
    return QMI_CLIENT_PARAM_ERR;
  }
 
  err = qmi_cci_shim_alloc_client_obj(handle, service_obj, tx_os_params, &qmi_shim_handle);
  if(err != QMI_NO_ERR)
  {
    QMI_CCI_LOG_ERR("qmi_cci_shim_alloc_client_obj failed with err %d", err, (uint32)service_obj, 0);
    return err;
  }
  
  qmi_shim_handle->ind_cb = ind_cb;
  qmi_shim_handle->ind_cb_data = ind_cb_data;
  
  err = qmi_client_init(service_info, service_obj, qmi_cci_shim_ind_cb_handler, (void *)qmi_shim_handle, qmi_shim_handle->os_params, &qmi_shim_handle->qmi_handle);
  if(err != QMI_NO_ERR)
  {
    QMI_CCI_LOG_ERR("qmi_client_init failed with err %d", err, (uint32)service_obj, (uint32)service_info);
    qmi_cci_shim_free_client_obj(qmi_shim_handle);
    return err;
  }
  
  //Fill rest of details
  qmi_shim_handle->category = QMI_CCI_CONNECTED_CLIENT;
  LINK_INIT(qmi_shim_handle->link);
  
  //Add this new qmi_shim_handle in module clnt list
  LOCK(&handle->clnt_list_lock);
  LIST_ADD(handle->clnt_list, qmi_shim_handle, link);
  UNLOCK(&handle->clnt_list_lock);

  *user_handle = (qmi_client_type) qmi_shim_handle;
  
  return err;
} /* _qmi_client_init */


/*===========================================================================
  FUNCTION  _qmi_client_init_instance
  DESCRIPTION: Qmi Client Init Instance shim function
===========================================================================*/
qmi_client_error_type _qmi_client_init_instance
(
 qmi_client_user_module_handle   *handle,
 qmi_idl_service_object_type      service_obj,
 qmi_service_instance             instance_id,
 qmi_client_ind_cb                ind_cb,
 void                            *ind_cb_data,
 qmi_client_tx_os_params         *tx_os_params,
 uint32_t                         timeout,
 qmi_client_type                 *user_handle
)
{
  qmi_client_shim_handle_type *qmi_shim_handle = NULL;
  qmi_client_error_type err = QMI_NO_ERR;
  
  if(user_handle == NULL)
  {
    QMI_CCI_LOG_ERR("user_handle is NULL in _qmi_client_init_instance", 0, 0, 0);
    return QMI_CLIENT_PARAM_ERR;
  }
  
  *user_handle = INVALID_HANDLE;
  
  //Allocate qmi shim structure
  err = qmi_cci_shim_alloc_client_obj(handle, service_obj, tx_os_params, &qmi_shim_handle);
  if(err != QMI_NO_ERR)
  {
    QMI_CCI_LOG_ERR("qmi_cci_shim_alloc_client_obj returned err %d", err, (uint32)service_obj, instance_id);
    return err;
  }
  
  //call qmi_client_init_instance for opening actual qmi handle
  err = qmi_client_init_instance(service_obj, instance_id, qmi_cci_shim_ind_cb_handler, (void *)qmi_shim_handle, 
                                   qmi_shim_handle->os_params, timeout, &qmi_shim_handle->qmi_handle);
  if(err != QMI_NO_ERR)
  {
    QMI_CCI_LOG_ERR("qmi_client_init_instance returned err %d", err, (uint32)service_obj, instance_id);
    qmi_cci_shim_free_client_obj(qmi_shim_handle);
    return err;
  }
  
  qmi_shim_handle->category = QMI_CCI_CONNECTED_CLIENT;
  qmi_shim_handle->ind_cb = ind_cb;
  qmi_shim_handle->ind_cb_data = ind_cb_data;
  LINK_INIT(qmi_shim_handle->link);
  
  //Add this new qmi_shim_handle in module clnt list
  LOCK(&handle->clnt_list_lock);
  LIST_ADD(handle->clnt_list, qmi_shim_handle, link);
  UNLOCK(&handle->clnt_list_lock);

  *user_handle = (qmi_client_type) qmi_shim_handle;
  
  return err;
} /* _qmi_client_init_instance */


/*===========================================================================
  FUNCTION  _qmi_client_register_notify_cb
  DESCRIPTION: notify_cb registration shim function
===========================================================================*/
qmi_client_error_type _qmi_client_register_notify_cb
(
    qmi_client_user_module_handle             *handle,
    qmi_client_type                            user_handle,
    qmi_client_notify_cb                       notify_cb,
    void                                      *notify_cb_data
)
{
  qmi_client_shim_handle_type *qmi_shim_handle;
  uint32 notify_received;
  
  qmi_shim_handle = (qmi_client_shim_handle_type *)user_handle;
  
  if(qmi_shim_handle == NULL || qmi_shim_handle->magic != QMI_CLIENT_SHIM_MAGIC)
  {
    QMI_CCI_LOG_ERR("qmi_shim_handle(0x%x) is NULL or invalid", qmi_shim_handle, 0, 0);
    return QMI_CLIENT_INVALID_CLNT;
  }
  
  LOCK(&qmi_shim_handle->mutex);
  qmi_shim_handle->notify_cb = notify_cb;
  qmi_shim_handle->notify_cb_data = notify_cb_data;
  notify_received = qmi_shim_handle->notify_received;
  UNLOCK(&qmi_shim_handle->mutex);
  
  if(notify_received == 1)
  {
    qmi_cci_shim_notify_cb_handler(qmi_shim_handle->qmi_handle, qmi_shim_handle->service_obj, 
                                     qmi_shim_handle->last_service_event, (void *)qmi_shim_handle);
  }
  
  return QMI_NO_ERR;
} /* _qmi_client_register_notify_cb */


/*===========================================================================
  FUNCTION  _qmi_client_register_error_cb
  DESCRIPTION: error_cb registration shim function
===========================================================================*/
qmi_client_error_type _qmi_client_register_error_cb
(
    qmi_client_user_module_handle             *handle,
    qmi_client_type                            user_handle,
    qmi_client_error_cb                        error_cb,
    void                                      *err_cb_data
)
{
  qmi_client_shim_handle_type *qmi_shim_handle;
  qmi_shim_handle = (qmi_client_shim_handle_type *)user_handle;
  
  if(qmi_shim_handle == NULL || qmi_shim_handle->magic != QMI_CLIENT_SHIM_MAGIC)
  {
    QMI_CCI_LOG_ERR("qmi_shim_handle(0x%x) is NULL or invalid", qmi_shim_handle, 0, 0);
    return QMI_CLIENT_INVALID_CLNT;
  }

  qmi_shim_handle->err_cb = error_cb;
  qmi_shim_handle->err_cb_data = err_cb_data;
  
  return qmi_client_register_error_cb(qmi_shim_handle->qmi_handle, qmi_cci_shim_err_cb_handler, (void *)qmi_shim_handle);
} /* _qmi_client_register_error_cb */


/*===========================================================================
  FUNCTION  _qmi_client_register_log_cb
  DESCRIPTION: log_cb registration shim function
===========================================================================*/
qmi_client_error_type _qmi_client_register_log_cb
(
    qmi_client_user_module_handle    *handle,
    qmi_client_type                   user_handle,
    qmi_client_log_cb                 log_cb,
    void                             *log_cb_data
)
{
  qmi_client_shim_handle_type *qmi_shim_handle;
  qmi_shim_handle = (qmi_client_shim_handle_type *)user_handle;
  
  if(qmi_shim_handle == NULL || qmi_shim_handle->magic != QMI_CLIENT_SHIM_MAGIC)
  {
    QMI_CCI_LOG_ERR("qmi_shim_handle(0x%x) is NULL or invalid", qmi_shim_handle, 0, 0);
    return QMI_CLIENT_INVALID_CLNT;
  }

  qmi_shim_handle->log_cb = log_cb;
  qmi_shim_handle->log_cb_data = log_cb_data;
  
  return qmi_client_register_log_cb(qmi_shim_handle->qmi_handle, qmi_cci_shim_log_cb_handler, (void *)qmi_shim_handle);
} /* _qmi_client_register_log_cb */


/*===========================================================================
  FUNCTION  _qmi_client_send_raw_msg_async
  DESCRIPTION: send_raw_msg_async shim function
===========================================================================*/
qmi_client_error_type _qmi_client_send_raw_msg_async
(
 qmi_client_user_module_handle     *handle,
 qmi_client_type                   user_handle,
 unsigned int                      msg_id,
 void                              *req_buf,
 unsigned int                      req_buf_len,
 void                              *resp_buf,
 unsigned int                      resp_buf_len,
 qmi_client_recv_raw_msg_async_cb  resp_cb,
 void                              *resp_cb_data,
 qmi_txn_handle                    *txn_handle
)
{
  qmi_client_shim_handle_type *qmi_shim_handle;
  qmi_shim_handle = (qmi_client_shim_handle_type *)user_handle;
  
  if(qmi_shim_handle == NULL || qmi_shim_handle->magic != QMI_CLIENT_SHIM_MAGIC)
  {
    QMI_CCI_LOG_ERR("qmi_shim_handle(0x%x) is NULL or invalid", qmi_shim_handle, 0, 0);
    return QMI_CLIENT_INVALID_CLNT;
  }
  
  qmi_shim_handle->raw_msg_async_cb = resp_cb;
  qmi_shim_handle->raw_msg_async_cb_data = resp_cb_data;

  //Note: This needs to be changed when user space buf cant be accessed directly by kernel
  return qmi_client_send_raw_msg_async(qmi_shim_handle->qmi_handle, msg_id, req_buf, req_buf_len, resp_buf, resp_buf_len, qmi_cci_shim_raw_msg_async_cb_handler, (void *)qmi_shim_handle, txn_handle);
} /* _qmi_client_send_raw_msg_async */


/*===========================================================================
  FUNCTION  _qmi_client_send_msg_async
  DESCRIPTION: send_msg_async shim function
===========================================================================*/
qmi_client_error_type _qmi_client_send_msg_async
(
 qmi_client_user_module_handle   *handle,
 qmi_client_type                 user_handle,
 unsigned int                    msg_id,
 void                            *req_c_struct,
 unsigned int                    req_c_struct_len,
 void                            *resp_c_struct,
 unsigned int                    resp_c_struct_len,
 qmi_client_recv_msg_async_cb    resp_cb,
 void                            *resp_cb_data,
 qmi_txn_handle                  *txn_handle
)
{
  qmi_client_shim_handle_type *qmi_shim_handle;
  qmi_shim_handle = (qmi_client_shim_handle_type *)user_handle;
  
  if(qmi_shim_handle == NULL || qmi_shim_handle->magic != QMI_CLIENT_SHIM_MAGIC)
  {
    QMI_CCI_LOG_ERR("qmi_shim_handle(0x%x) is NULL or invalid", qmi_shim_handle, 0, 0);
    return QMI_CLIENT_INVALID_CLNT;
  }
  
  qmi_shim_handle->struct_msg_async_cb = resp_cb;
  qmi_shim_handle->struct_msg_async_cb_data = resp_cb_data;

  //Note: This needs to be changed when user space buf cant be accessed directly by kernel
  return qmi_client_send_msg_async(qmi_shim_handle->qmi_handle, msg_id, req_c_struct, req_c_struct_len, resp_c_struct, resp_c_struct_len, qmi_cci_shim_struct_msg_async_cb_handler, (void *)qmi_shim_handle, txn_handle);
} /* _qmi_client_send_msg_async */


/*===========================================================================
  FUNCTION  _qmi_client_delete_async_txn
  DESCRIPTION: delete transaction async shim function
===========================================================================*/
qmi_client_error_type _qmi_client_delete_async_txn
(
 qmi_client_user_module_handle    *handle,
 qmi_client_type                   user_handle,
 qmi_txn_handle                    async_txn_handle
)
{
  qmi_client_shim_handle_type *qmi_shim_handle;
  qmi_shim_handle = (qmi_client_shim_handle_type *)user_handle;
  
  if(qmi_shim_handle == NULL || qmi_shim_handle->magic != QMI_CLIENT_SHIM_MAGIC)
  {
    QMI_CCI_LOG_ERR("qmi_shim_handle(0x%x) is NULL or invalid", qmi_shim_handle, 0, 0);
    return QMI_CLIENT_INVALID_CLNT;
  }
  
  return qmi_client_delete_async_txn(qmi_shim_handle->qmi_handle, async_txn_handle);
} /* _qmi_client_delete_async_txn */


/*===========================================================================
  FUNCTION  _qmi_client_send_raw_msg_sync
  DESCRIPTION: send raw msg sync shim function
===========================================================================*/
qmi_client_error_type _qmi_client_send_raw_msg_sync
(
 qmi_client_user_module_handle    *handle,
 qmi_client_type                   user_handle,
 unsigned int                      msg_id,
 void                             *req_buf,
 unsigned int                      req_buf_len,
 void                             *resp_buf,
 unsigned int                      resp_buf_len,
 unsigned int                     *resp_buf_recv_len,
 unsigned int                      timeout_msecs
)
{
  qmi_client_shim_handle_type *qmi_shim_handle;
  qmi_shim_handle = (qmi_client_shim_handle_type *)user_handle;
  
  if(qmi_shim_handle == NULL || qmi_shim_handle->magic != QMI_CLIENT_SHIM_MAGIC)
  {
    QMI_CCI_LOG_ERR("qmi_shim_handle(0x%x) is NULL or invalid", qmi_shim_handle, 0, 0);
    return QMI_CLIENT_INVALID_CLNT;
  }

  //Note: This needs to be changed when user space buf cant be accessed directly by kernel
  return qmi_client_send_raw_msg_sync(qmi_shim_handle->qmi_handle, msg_id, req_buf, req_buf_len, resp_buf, resp_buf_len, resp_buf_recv_len, timeout_msecs);
} /* _qmi_client_send_raw_msg_sync */


/*===========================================================================
  FUNCTION  _qmi_client_send_msg_sync
  DESCRIPTION: send msg sync shim function
===========================================================================*/
qmi_client_error_type _qmi_client_send_msg_sync
(
 qmi_client_user_module_handle    *handle,
 qmi_client_type                   user_handle,
 unsigned int                      msg_id,
 void                             *req_c_struct,
 unsigned int                      req_c_struct_len,
 void                             *resp_c_struct,
 unsigned int                      resp_c_struct_len,
 unsigned int                      timeout_msecs
)
{
  qmi_client_shim_handle_type *qmi_shim_handle;
  qmi_shim_handle = (qmi_client_shim_handle_type *)user_handle;
  
  if(qmi_shim_handle == NULL || qmi_shim_handle->magic != QMI_CLIENT_SHIM_MAGIC)
  {
    QMI_CCI_LOG_ERR("qmi_shim_handle(0x%x) is NULL or invalid", qmi_shim_handle, 0, 0);
    return QMI_CLIENT_INVALID_CLNT;
  }

  //Note: This needs to be changed when user space buf cant be accessed directly by kernel
  return qmi_client_send_msg_sync(qmi_shim_handle->qmi_handle, msg_id, req_c_struct, req_c_struct_len, resp_c_struct, resp_c_struct_len, timeout_msecs);
} /* _qmi_client_send_msg_sync */


/*===========================================================================
  FUNCTION  _qmi_client_release_async
  DESCRIPTION: release async shim function
===========================================================================*/
qmi_client_error_type _qmi_client_release_async
(
 qmi_client_user_module_handle    *handle,
 qmi_client_type                   user_handle,
 qmi_client_release_cb             release_cb,
 void                             *release_cb_data
)
{
  qmi_client_shim_handle_type *qmi_shim_handle;
  qmi_shim_handle = (qmi_client_shim_handle_type *)user_handle;
  
  if(qmi_shim_handle == NULL || qmi_shim_handle->magic != QMI_CLIENT_SHIM_MAGIC)
  {
    QMI_CCI_LOG_ERR("qmi_shim_handle(0x%x) is NULL or invalid", qmi_shim_handle, 0, 0);
    return QMI_CLIENT_INVALID_CLNT;
  }
  
  qmi_shim_handle->release_cb = release_cb;
  qmi_shim_handle->release_cb_data = release_cb_data;
  
  return qmi_client_release_async(qmi_shim_handle->qmi_handle, qmi_cci_shim_release_cb_handler, (void *)qmi_shim_handle);  
} /* _qmi_client_release_async */


/*===========================================================================
  FUNCTION  _qmi_client_release
  DESCRIPTION: qmi client release shim function
===========================================================================*/
qmi_client_error_type _qmi_client_release
(
 qmi_client_user_module_handle    *handle,
 qmi_client_type                   user_handle
)
{
  qmi_client_error_type err;
  qmi_client_shim_handle_type *qmi_shim_handle;
  qmi_shim_handle = (qmi_client_shim_handle_type *)user_handle;
 
  if(qmi_shim_handle == NULL || qmi_shim_handle->magic != QMI_CLIENT_SHIM_MAGIC)
  {
    QMI_CCI_LOG_ERR("qmi_shim_handle(0x%x) is NULL or invalid", qmi_shim_handle, 0, 0);
    return QMI_CLIENT_INVALID_CLNT;
  }
 
  err = qmi_client_release(qmi_shim_handle->qmi_handle);
  if(err != QMI_NO_ERR)
  {
    QMI_CCI_LOG_ERR("qmi_client_release failed with %d", err, qmi_shim_handle, 0);
    return err;
  }
 
  //remove the client handle from shim client list
  LOCK(&handle->clnt_list_lock);
  LIST_REMOVE(handle->clnt_list, qmi_shim_handle, link);
  UNLOCK(&handle->clnt_list_lock);
 
  err = qmi_cci_shim_free_client_obj(qmi_shim_handle);
  if(err != QMI_NO_ERR)
  {
    QMI_CCI_LOG_ERR("free_client_obj failed with err %d for shim handle 0x%x", err, qmi_shim_handle, 0);
    return err;
  }
 
  return err; 
} /* _qmi_client_release */


/*===========================================================================
  FUNCTION  _qmi_client_message_encode
  DESCRIPTION: message encode shim function
===========================================================================*/
qmi_client_error_type _qmi_client_message_encode
(
 qmi_client_user_module_handle        *handle,
 qmi_client_type                      user_handle,
 qmi_idl_type_of_message_type         req_resp_ind,
 unsigned int                         message_id,
 const void                           *p_src,
 unsigned int                         src_len,
 void                                 *p_dst,
 unsigned int                         dst_len,
 unsigned int                         *dst_encoded_len
)
{
  qmi_client_shim_handle_type *qmi_shim_handle;
  qmi_shim_handle = (qmi_client_shim_handle_type *)user_handle;
 
  if(qmi_shim_handle == NULL || qmi_shim_handle->magic != QMI_CLIENT_SHIM_MAGIC)
  {
    QMI_CCI_LOG_ERR("qmi_shim_handle(0x%x) is NULL or invalid", qmi_shim_handle, 0, 0);
    return QMI_CLIENT_INVALID_CLNT;
  }
 
  return qmi_client_message_encode(qmi_shim_handle->qmi_handle, req_resp_ind, message_id, p_src, src_len, p_dst, dst_len, dst_encoded_len);
} /* _qmi_client_message_encode */


/*===========================================================================
  FUNCTION  _qmi_client_message_decode
  DESCRIPTION: message decode shim function
===========================================================================*/
qmi_client_error_type _qmi_client_message_decode
(
 qmi_client_user_module_handle        *handle,
 qmi_client_type                      user_handle,
 qmi_idl_type_of_message_type         req_resp_ind,
 unsigned int                         message_id,
 const void                           *p_src,
 unsigned int                         src_len,
 void                                 *p_dst,
 unsigned int                         dst_len
)
{
  qmi_client_shim_handle_type *qmi_shim_handle;
  qmi_shim_handle = (qmi_client_shim_handle_type *)user_handle;
 
  if(qmi_shim_handle == NULL || qmi_shim_handle->magic != QMI_CLIENT_SHIM_MAGIC)
  {
    QMI_CCI_LOG_ERR("qmi_shim_handle(0x%x) is NULL or invalid", qmi_shim_handle, 0, 0);
    return QMI_CLIENT_INVALID_CLNT;
  }
 
  return qmi_client_message_decode(qmi_shim_handle->qmi_handle, req_resp_ind, message_id, p_src, src_len, p_dst, dst_len);
} /* _qmi_client_message_decode */


/*===========================================================================
  FUNCTION  _qmi_client_user_module_deinit
  DESCRIPTION: qmi client module deinit function
===========================================================================*/
qmi_client_error_type _qmi_client_user_module_deinit
(
 qmi_cci_user_module_handle *user_module_handle
)
{
  qmi_client_user_module_handle *module_handle = (qmi_client_user_module_handle *)*user_module_handle;
  qmi_client_shim_handle_type *qmi_shim_handle;
  UINT status;

  if(module_handle == NULL || module_handle->magic != QMI_SHIM_MODULE_COOKIE)
  {
    QMI_CCI_LOG_ERR("module_handle(0x%x) is invalid", module_handle, 0, 0);
    return QMI_CLIENT_INVALID_CLNT;
  }
  
  *user_module_handle = INVALID_HANDLE;
  
  //Close all open qmi clients
  LOCK(&module_handle->clnt_list_lock);
  
  do{
    qmi_shim_handle = LIST_HEAD(module_handle->clnt_list);
    
    //qmi_shim_handle will be removed from clnt_list by _qmi_client_release
    _qmi_client_release(module_handle, (qmi_client_type)qmi_shim_handle);
  }while(LIST_HEAD(module_handle->clnt_list) != NULL);
  
  UNLOCK(&module_handle->clnt_list_lock);
  
  //Note: Below QMI_CCI_EXIT_CB_HANDLER and timer_sleep is workaround since if we are deleting
  // tx q while on other side if worker thread is waiting on msgQ, its causing exception
  
  //Indicate user space worker thread to get exited
  qmi_cci_shim_callback_handler(module_handle, QMI_CCI_EXIT_CB_HANDLER, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  
  //Wait for sometime for user space thread to get exited
  timer_sleep(deinit_timeout, T_MSEC, TRUE);
  
  //cleanup module handle internals and obj itself
  status = tx_queue_delete(&module_handle->msg_q);
  if(status != TX_SUCCESS)
  {
    QMI_CCI_LOG_ERR("tx_queue_delete returned %d error", status, 0, 0);
  }
  
  FREE(module_handle->msg_q_buf);
  
  LOCK_DEINIT(&module_handle->clnt_list_lock);
  
  LOCK(&qmi_cci_shim_module_handle_list_lock);
  LIST_REMOVE(qmi_cci_shim_module_handle_list, module_handle, link);
  UNLOCK(&qmi_cci_shim_module_handle_list_lock);
  
  module_handle->magic = INVALID_HANDLE;
  
  FREE(module_handle);
  
  return QMI_NO_ERR;
}/* _qmi_client_user_module_deinit */


/*===========================================================================
  FUNCTION  _qmi_client_user_module_init
  DESCRIPTION: qmi client module init function
===========================================================================*/
qmi_client_error_type _qmi_client_user_module_init
(
 qmi_cci_user_module_handle *user_module_handle
)
{
  qmi_client_user_module_handle *module_handle;
  uint32 message_size;
  UINT status;
  
  if(user_module_handle == NULL)
  {
    QMI_CCI_LOG_ERR("user_module_handle is NULL", 0, 0, 0);
    return QMI_CLIENT_PARAM_ERR;
  }
  
  message_size = sizeof(qmi_cci_shim_cb_info)/sizeof(uint32);
  
  module_handle = CALLOC(1, sizeof(qmi_client_user_module_handle));
  if(module_handle == NULL)
  {
    QMI_CCI_LOG_ERR("failed to alloc module_handle structure of size %d", sizeof(qmi_client_user_module_handle), 0, 0);
    return QMI_CLIENT_ALLOC_FAILURE;
  }
  
  module_handle->magic = QMI_SHIM_MODULE_COOKIE;
  
  module_handle->msg_q_buf = MALLOC(QMI_MSG_Q_MAX_NUM_CB_INFOS * sizeof(qmi_cci_shim_cb_info));
  if(module_handle->msg_q_buf == NULL)
  {
    FREE(module_handle);
    QMI_CCI_LOG_ERR("failed to alloc buffer for msgQ of size %d", QMI_MSG_Q_MAX_NUM_CB_INFOS * sizeof(qmi_cci_shim_cb_info), 0, 0);
    return QMI_CLIENT_ALLOC_FAILURE;
  }
  
  status = tx_queue_create(&module_handle->msg_q, "qmi_cci_shim", message_size, module_handle->msg_q_buf, 
                            QMI_MSG_Q_MAX_NUM_CB_INFOS * sizeof(qmi_cci_shim_cb_info));
  if (TX_SUCCESS != status)
  {
    QMI_CCI_LOG_ERR("tx_queue_create failed with status %d", status, 0, 0);
    FREE(module_handle->msg_q_buf);
    FREE(module_handle);
    return QMI_INTERNAL_ERR;
  }
    
  LOCK_INIT(&module_handle->clnt_list_lock);
  LIST_INIT(module_handle->clnt_list);
  LINK_INIT(module_handle->link);
  
  LOCK(&qmi_cci_shim_module_handle_list_lock);
  LIST_ADD(qmi_cci_shim_module_handle_list, module_handle, link);
  UNLOCK(&qmi_cci_shim_module_handle_list_lock);
  
  *user_module_handle = (qmi_cci_user_module_handle) module_handle;
  return QMI_NO_ERR;
} /* _qmi_client_user_module_init */

/*===========================================================================
  FUNCTION  qmi_client_shim_kernel_handler
  DESCRIPTION: qmi client shim kernel handler function
===========================================================================*/
UINT qmi_client_shim_kernel_handler
(
   UINT request_ID, UINT a, UINT b, UINT c, 
   UINT d, UINT e, UINT f, UINT g, UINT h, 
   UINT i, UINT j, UINT k, UINT l
)
{
  qmi_client_error_type ret;
  
  switch(request_ID)
  {
    case TXM_QCCI_USER_MODULE_INIT_CMD:
        ret = _qmi_client_user_module_init((qmi_cci_user_module_handle *)a);
    break;
    
    case TXM_QCCI_USER_MODULE_DEINIT_CMD:
        ret = _qmi_client_user_module_deinit((qmi_cci_user_module_handle *)a);
    break;
    
    case TXM_QCCI_NOTIFIER_INIT_CMD:
        ret = _qmi_client_notifier_init((qmi_client_user_module_handle *)a, (qmi_idl_service_object_type) b, (qmi_client_tx_os_params *) c, (qmi_client_type *) d);
    break;
      
    case TXM_QCCI_REGISTER_NOTIFY_CB_CMD:
        ret = _qmi_client_register_notify_cb((qmi_client_user_module_handle *)a, (qmi_client_type) b, (qmi_client_notify_cb) c, (void *)d);
    break;
    
    case TXM_QCCI_REGISTER_ERROR_CB_CMD:
        ret = _qmi_client_register_error_cb((qmi_client_user_module_handle *)a, (qmi_client_type) b, (qmi_client_error_cb) c, (void *)d);
    break;
    
    case TXM_QCCI_REGISTER_LOG_CB_CMD:
        ret = _qmi_client_register_log_cb((qmi_client_user_module_handle *)a, (qmi_client_type) b, (qmi_client_log_cb) c, (void *)d);
    break;
    
    case TXM_QCCI_CLIENT_INIT_CMD:
        ret = _qmi_client_init((qmi_client_user_module_handle *)a, (qmi_service_info *) b, (qmi_idl_service_object_type) c, (qmi_client_ind_cb) d, (void *)e, (qmi_client_tx_os_params *) f, (qmi_client_type *)g);
    break;
    
    case TXM_QCCI_INIT_INSTANCE_CMD:
        ret = _qmi_client_init_instance((qmi_client_user_module_handle *)a, (qmi_idl_service_object_type) b, (qmi_service_instance) c, (qmi_client_ind_cb) d, (void *)e, (qmi_client_tx_os_params *) f, (uint32_t)g, (qmi_client_type *)h);
    break;
    
    case TXM_QCCI_GET_SERVICE_LIST_CMD:
        ret = _qmi_client_get_service_list((qmi_client_user_module_handle *)a, (qmi_idl_service_object_type) b, (qmi_service_info *) c, (unsigned int *)d, (unsigned int *)e);
    break;
    
    case TXM_QCCI_GET_SERVICE_INSTANCE_CMD:
        ret = _qmi_client_get_service_instance((qmi_client_user_module_handle *)a, (qmi_idl_service_object_type) b, (qmi_service_instance) c, (qmi_service_info *)d);
    break;
    
    case TXM_QCCI_GET_INSTANCE_ID_CMD:
        ret = _qmi_client_get_instance_id((qmi_client_user_module_handle *)a, (qmi_service_info *) b, (qmi_service_instance *) c);
    break;
    
    case TXM_QCCI_SEND_RAW_MSG_ASYNC_CMD:
        ret = _qmi_client_send_raw_msg_async((qmi_client_user_module_handle *)a, (qmi_client_type)b, (unsigned int)c, (void *)d, (unsigned int)e, (void *)f, (unsigned int)g, (qmi_client_recv_raw_msg_async_cb)h, (void *)i, (qmi_txn_handle*)j);
    break;
    
    case TXM_QCCI_SEND_MSG_ASYNC_CMD:
        ret = _qmi_client_send_msg_async((qmi_client_user_module_handle *)a, (qmi_client_type)b, (unsigned int)c, (void *)d, (unsigned int)e, (void *)f, (unsigned int)g, (qmi_client_recv_msg_async_cb)h, (void *)i, (qmi_txn_handle*)j);
    break;
    
    case TXM_QCCI_DELETE_ASYNC_TXN_CMD:
        ret = _qmi_client_delete_async_txn((qmi_client_user_module_handle *)a, (qmi_client_type)b, (qmi_txn_handle)c);
    break;
    
    case TXM_QCCI_SEND_RAW_MSG_SYNC_CMD:
        ret = _qmi_client_send_raw_msg_sync((qmi_client_user_module_handle *)a, (qmi_client_type)b, (unsigned int)c, (void *)d, (unsigned int)e, (void *)f, (unsigned int)g, (unsigned int *)h, (unsigned int)i);
    break;
    
    case TXM_QCCI_SEND_MSG_SYNC_CMD:
        ret = _qmi_client_send_msg_sync((qmi_client_user_module_handle *)a, (qmi_client_type)b, (unsigned int)c, (void *)d, (unsigned int)e, (void *)f, (unsigned int)g, (unsigned int)h);
    break;
    
    case TXM_QCCI_RELEASE_ASYNC_CMD:
        ret = _qmi_client_release_async((qmi_client_user_module_handle *)a, (qmi_client_type)b, (qmi_client_release_cb)c, (void *)d);
    break;
    
    case TXM_QCCI_RELEASE_CMD:
        ret = _qmi_client_release((qmi_client_user_module_handle *)a, (qmi_client_type)b);
    break;
    
    case TXM_QCCI_MESSAGE_ENCODE_CMD:
        ret = _qmi_client_message_encode((qmi_client_user_module_handle *)a, (qmi_client_type)b, (qmi_idl_type_of_message_type)c, (unsigned int)d, (const void *)e, (unsigned int)f, (void *)g, (unsigned int)h, (unsigned int *)i);
    break;
    
    case TXM_QCCI_MESSAGE_DECODE_CMD:
        ret = _qmi_client_message_decode((qmi_client_user_module_handle *)a, (qmi_client_type)b, (qmi_idl_type_of_message_type)c, (unsigned int)d, (const void *)e, (unsigned int)f, (void *)g, (unsigned int)h);
    break;
    
    case TXM_QCCI_GET_CB_INFO_CMD:
        ret = qmi_cci_shim_get_callback_details((qmi_client_user_module_handle *)a, (qmi_cci_shim_cb_info *)b);
    break;
    
    case TXM_QCCI_GET_BUF_CMD:
        ret = qmi_cci_shim_get_buf((qmi_client_user_module_handle *)a, (qmi_client_shim_handle_type *)b, (void *)c, d, (void *)e, f);
    break;
    
    default:
        ret = QMI_INTERNAL_ERR;
    break;
  }
 
  return (UINT)ret;
} /* qmi_client_shim_kernel_handler */


/*===========================================================================
  FUNCTION  qmi_client_shim_init
  DESCRIPTION: qmi client shim init function
===========================================================================*/
void qmi_client_shim_init(void)
{
  qurt_qapi_register(QMI_SHIM_QCCI_BASE, QMI_SHIM_QCCI_NUM_IDS, qmi_client_shim_kernel_handler);

  LOCK_INIT(&qmi_cci_shim_module_handle_list_lock);  
  
  //Global list that contains all registered module handles
  LIST_INIT(qmi_cci_shim_module_handle_list);
} /* qmi_client_shim_init */

