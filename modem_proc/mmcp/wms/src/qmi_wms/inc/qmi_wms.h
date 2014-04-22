#ifndef QMI_WMS_H
#define QMI_WMS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
           W I R E L E S S    M E S S A G I N G   S E R V I C E S
           qmi_wms.h --

  The  QMI Wireless Message Service header file.


  -------------

    Copyright (c) 2007,2009-2012 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

  $Header: //components/rel/mmcp.mpss/6.1.10/wms/src/qmi_wms/inc/qmi_wms.h#1 $
  $DateTime: 2023/04/25 14:39:18 $
  $Author: pwbldsvc $
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
EXTERNALIZED FUNCTIONS

   qmi_wms_init
     Register the WMS service with QMUX

===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "ds_qmi_defs.h"
#include "ds_qmi_if_ext.h"
#include "qmi_wms_task_svc.h"
#include "qmi_csi.h"
#include "qmi_sap.h"

/*===========================================================================
TYPEDEF QMI_MMODE_SERVICE_ENUM_TYPE
 
DESCRIPTION 
  This enum represents the different services QMI_WMS handles
 
  
===========================================================================*/

typedef enum
{

  QMI_WMS_SERVICE     = 0,
  QMI_WMS_SERVICE_MAX

} qmi_wms_service_e_type;

/*===========================================================================
TYPEDEF qmi_wms_svc_info_s

DESCRIPTION
  This type can be used for each qmi msg command req/response.
===========================================================================*/

typedef struct qmi_wms_svc_info_s
{
  qmi_wms_service_e_type  service_id;    /* Service type    */
  qmi_csi_service_handle  service_handle; /* QCSI handle */
  qmi_wms_sig_enum_type   svc_sig;        /* Specific signal being used for the service */
  qmi_csi_os_params       os_params;      /* Task paramets to provide to QCSI */

  q_type                  client_q; /* Q of all service clients */

  q_type                  pending_req; /* Q of all pending requests */
  void                   *svc_config;

  qmi_sap_client_handle  sap_client_handle;
  
}qmi_wms_svc_info_type;


/*===========================================================================
TYPEDEF qmi_wms_qcsi_transaction_type

DESCRIPTION
  This type can be used for each qmi msg command req/response.
===========================================================================*/

typedef struct
{
  q_link_type                    link; /*< Data Structure required by the queue 
                                                              library to add this as part of a queue */
  qmi_req_handle                 req_handle; /*< Req handle pertaining to the pending req */
  uint16                         msg_id; 
  //dsm_item_type                 *sdu_in;
  void                          *clnt_info_ptr;
  
} qmi_wms_qcsi_transaction_type;


/*===========================================================================
TYPEDEF QMI_SVC_HDLR_FTYPE

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
typedef dsm_item_type* (* qmi_wms_svc_hdlr_ftype)
                          ( 
                            qmi_wms_svc_info_type          *svc_ptr,
                            qmi_wms_qcsi_transaction_type  *trans_type,
                            void                             *cl_sp,
                            dsm_item_type                   **sdu
                          );

/*===========================================================================
TYPEDEF qmi_wms_svc_cmd_hdlr_type

DESCRIPTION
  This type can be used for each qmi msg command req/response.
===========================================================================*/
typedef struct
{
  uint16                     msg_id;
  char                      *msg_desc;
  qmi_wms_svc_hdlr_ftype   request_hdlr;
} qmi_wms_svc_cmd_hdlr_type;

/*===========================================================================
TYPEDEF qmi_wms_svc_cfg_type

DESCRIPTION
  This type can be used for each qmi msg command req/response.
===========================================================================*/

typedef struct
{
  qmi_wms_svc_cmd_hdlr_type *          cmd_hdlr_array;   
  uint16                               cmd_num_entries;  
} qmi_wms_svc_cfg_type;


/*===========================================================================
MACRO QMI_SVC_HDLR()

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


#define QMI_WMS_SVC_HDLR(cmdval,hdlr)  { cmdval, #hdlr, hdlr }


/*===========================================================================
  FUNCTION qmi_wms_init()

  DESCRIPTION
    Register the WMS service with QCSI

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_wms_init(
  void
);

/*===========================================================================
  FUNCTION qmi_wms_qcsi_compare_pending_trans()

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

int qmi_wms_qcsi_compare_pending_trans
( 
  void *item_ptr,
  void *compare_val 
);


/*===========================================================================
FUNCTION qmi_wms_task_set_stop_sig_rcvd

DESCRIPTION
  SET the global which tell if TASK_STOP has come or not

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_wms_task_set_stop_sig_rcvd(
  boolean val
);

/*===========================================================================
  FUNCTION qmi_wms_svc_free_pending_transaction()

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
void qmi_wms_svc_free_pending_transaction
(
  qmi_wms_svc_info_type          *svc_ptr,
  qmi_wms_qcsi_transaction_type **pending_trans
);

/*===========================================================================
  FUNCTION qmi_wms_qcsi_compare_client_handle()

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

int qmi_wms_qcsi_compare_client_handle
( 
  void *item_ptr,
  void *compare_val 
);

/*===========================================================================
  FUNCTION qmi_wms_qcsi_compare_pending_trans()

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

int qmi_wms_qcsi_compare_pending_trans
( 
  void *item_ptr,
  void *compare_val 
);

/*===========================================================================
  FUNCTION qmi_wms_qcsi_svc_handle_raw_req_cb()

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

qmi_csi_cb_error qmi_wms_qcsi_svc_handle_raw_req_cb
(
  void                     *connection_handle,
  qmi_req_handle           req_handle,
  unsigned int             msg_id,
  void                     *raw_msg,
  unsigned int             raw_msg_len,
  void                     *service_handle
);

/*===========================================================================
  FUNCTION qmi_wms_qcsi_send_resp_raw()

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

qmi_csi_error qmi_wms_qcsi_send_resp_raw
(
  qmi_wms_svc_info_type            *svc_ptr,
  qmi_wms_qcsi_transaction_type    *trans,
  dsm_item_type                    *resp_ptr
  
);

/*===========================================================================
  FUNCTION qmi_wms_qcsi_send_ind_raw()

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

qmi_csi_error qmi_wms_qcsi_send_ind_raw
(
  qmi_client_handle                   clnt,
  unsigned int                        msg_id,
  dsm_item_type                      *ind_ptr
);

/*===========================================================================
  FUNCTION qmi_wms_qcsi_event_hdlr_cb()

  DESCRIPTION
    CallBack function invoked when signal is set by QCSI

  PARAMETERS

  RETURN VALUE
    TRUE - SUCCESS

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/


boolean qmi_wms_qcsi_event_hdlr_cb();

/*===========================================================================
  FUNCTION qmi_wms_svc_qcsi_register()

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

qmi_csi_error qmi_wms_svc_qcsi_register
(
  qmi_wms_service_e_type                  service,
  qmi_csi_connect                           service_connect,
  qmi_csi_disconnect                        service_disconnect,
  qmi_csi_process_req                       service_process_req,
  qmi_wms_svc_info_type                   *service_cookie
);

extern boolean qmi_wms_task_stop_sig_rcvd; 
#define QMI_WMS_TASK_STOP_SIG_IS_RECEIVED (TRUE == qmi_wms_task_stop_sig_rcvd)

#endif /* QMI_WMS_H */
