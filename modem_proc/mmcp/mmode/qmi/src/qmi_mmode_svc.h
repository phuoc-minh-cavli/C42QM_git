#ifndef _QMI_MMODE_SVC_H
#define _QMI_MMODE_SVC_H
/*===========================================================================

                         Q M I _ M M C P _ S V C . H

DESCRIPTION

 The Data Services QMI Definitions header file. 
 Contains definitions common to all QMI services.


Copyright (c) 2004-2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE
  $Header: //components/rel/mmcp.mpss/6.1.10/mmode/qmi/src/qmi_mmode_svc.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/08/12    sk     Initial version
===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "dsm.h"
#include "queue.h"     /* Interface to queue services */
#include "qmi_mmode_task_svc.h"
#include "qmi_csi.h"
#include "qmi_idl_lib.h"
#include "common_v01.h"
#include "qmi_sap.h"

/*===========================================================================
MACRO QMI_MMODE_SVC_MSG_HDLR()

DESCRIPTION
  Returns a qmi_svc_handler initializer given command type and handler
  function.

PARAMETERS
  cmdval : The command type value of the message
  hdlr   : The name of the handler function

RETURNS
  An initializer of qmi_svc_handler type.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
#define QMI_MMODE_SVC_MSG_HDLR(cmdval,hdlr)  { hdlr, NULL, #hdlr, cmdval }

/*===========================================================================
MACRO QMI_MMODE_SVC_RAW_MSG_HDLR()

DESCRIPTION
  Returns a qmi_svc_handler initializer given command type and handler
  function.

PARAMETERS
  cmdval : The command type value of the message
  hdlr   : The name of the handler function

RETURNS
  An initializer of qmi_svc_handler type.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
#define QMI_MMODE_SVC_RAW_MSG_HDLR(cmdval,hdlr)  { NULL, hdlr, #hdlr, cmdval }

/*===========================================================================
TYPEDEF QMI_MMODE_SERVICE_ENUM_TYPE
 
DESCRIPTION 
  This enum represents the different services QMI_MMODE handles
 
  
===========================================================================*/
typedef enum
{

  QMI_MMODE_SERVICE_NAS      = 0,
  QMI_MMODE_SERVICE_VOICE    = 1,
  QMI_MMODE_SERVICE_PSM      = 2,

  QMI_MMODE_SERVICE_MAX

} qmi_mmode_service_e_type;

/*===========================================================================
TYPEDEF qmi_mmode_svc_info_type

DESCRIPTION
  This type can be used for each qmi msg command req/response.
===========================================================================*/
typedef struct qmi_mmode_svc_info_s
{
  q_type                  client_q; /* Q of all service clients */
  q_type                  pending_req;/* Q of all pending requests */
  qmi_csi_os_params       os_params;      /* Task paramets to provide to QCSI */
  qmi_csi_service_handle  service_handle; /* QCSI handle */
  qmi_sap_client_handle  sap_client_handle;
  void                   *svc_config;
  qmi_mmode_service_e_type service_type;   /* Service type    */
  qmi_mmode_sig_enum_type svc_sig;        /* Specific signal being used for the service */                                   
  boolean                 uses_raw_msgs;  /* TRUE if the service sends/receives raw_msgs from QCSI */
  
} qmi_mmode_svc_info_type;


/*===========================================================================
TYPEDEF qmi_mmode_qcsi_transaction_type

DESCRIPTION
  This type can be used for each qmi msg command req/response.
===========================================================================*/

typedef struct
{
  q_link_type                    link; /*< Data Structure required by the queue 
                                                              library to add this as part of a queue */
  qmi_req_handle                 req_handle; /*< Req handle pertaining to the pending req */
  uint16                         msg_id; 
  void                          *clnt_info_ptr;
  
} qmi_mmode_qcsi_transaction_type;

/*===========================================================================
TYPEDEF QMI_SVC_HDLR_RAW_MSG_HDLR_FTYPE

DESCRIPTION
  QMI service command handler type.
 
  Request Handler Parameters:
    sp         : Service provided state pointer.
    cmd_buf_p  : Command buffer.
    cl_sp      : Client state pointer.
    sdu        : DSM item containing request to be processed.

  The sp and cl_sp should be appropriately casted to the service state 
  and client state structures respectively in the command handlers.
---------------------------------------------------------------------------*/
typedef dsm_item_type* (* qmi_mmode_svc_raw_msg_hdlr_ftype)
                          ( 
                            qmi_mmode_svc_info_type          *svc_ptr,
                            qmi_mmode_qcsi_transaction_type  *trans_type,
                            void                             *cl_sp,
                            dsm_item_type                   **sdu
                          );

/*===========================================================================
TYPEDEF QMI_SVC_HDLR_MSG_HDLR_FTYPE

DESCRIPTION
  QMI service command handler type.
 
  Request Handler Parameters:
    sp         : Service provided state pointer.
    cmd_buf_p  : Command buffer.
    cl_sp      : Client state pointer.
    sdu        : DSM item containing request to be processed.

  The sp and cl_sp should be appropriately casted to the service state 
  and client state structures respectively in the command handlers.
---------------------------------------------------------------------------*/
typedef qmi_csi_cb_error (* qmi_mmode_svc_msg_hdlr_ftype)
                          ( 
                            qmi_mmode_svc_info_type          *svc_ptr,
                            qmi_mmode_qcsi_transaction_type  *trans_type,
                            void                             *cl_sp,
                            void                             *req,
                            unsigned int                      req_len
                          );

/*===========================================================================
TYPEDEF qmi_mmode_svc_cmd_hdlr_type

DESCRIPTION
  This type can be used for each qmi msg command req/response.
===========================================================================*/
typedef struct
{
  qmi_mmode_svc_msg_hdlr_ftype       request_hdlr;   
  /* if the service receives decoded requests from QCSI in c_struct */
  qmi_mmode_svc_raw_msg_hdlr_ftype   raw_msg_request_hdlr;
  /* if the service receives raw requests from QCSI  */
  char                      *msg_desc;
  uint16                     msg_id;
} qmi_mmode_svc_cmd_hdlr_type;

/*===========================================================================
TYPEDEF qmi_mmode_svc_config_type

DESCRIPTION
  This type can be used for each qmi msg command req/response.
===========================================================================*/
typedef struct
{

  qmi_mmode_svc_cmd_hdlr_type *cmd_hdlr_array;
  uint16 cmd_num_entries;
} qmi_mmode_svc_config_type;


/*===========================================================================
  FUNCTION QMI_MMODE_IDL_MESSAGE_DECODE()

  DESCRIPTION
    Decode QMI NAS message into c structure from incoming dsm item

  PARAMETERS
    svc          The QMI IDL service object.
    message_id   QMI message id
    msg_ptr      Pointer to dsm item that contains TLVs 
    p_req        Pointer to C structure to contain decoded message
    p_req_size   Size of C structure to contain decoded data

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
);

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
   qmi_idl_service_object_type svc, 
   qmi_idl_type_of_message_type type, 
   uint16_t message_id, 
   void *p_src, 
   uint32_t p_src_size, 
   dsm_item_type **p_response 
 );

/*===========================================================================
  FUNCTION qmi_mmode_svc_free_pending_transaction()

  DESCRIPTION
    Deletes the pending transaction provided and Frees the memory allocated

  PARAMETERS
    svc_ptr : Service pointer thats associated with QMI Service
    pending_trans : Pointer to pening transaction

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
);

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
);

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
  void                     *req,
  unsigned int             req_len,
  void                     *service_handle
);

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
);

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
  qmi_mmode_qcsi_transaction_type    **trans,
  dsm_item_type                      *resp_ptr
  
);

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
);

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
);


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
);

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
);

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
);

#endif /* _QMI_MMODE_SVC_H */
