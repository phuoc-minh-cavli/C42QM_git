/*===========================================================================

                         D S _ Q M I _ NIPD . C

DESCRIPTION

  The Data Services QMI NON IP DATA service source file.

EXTERNALIZED FUNCTIONS

  qmi_nipd_init()
    Initialize the QMI NIPD service

Copyright (c) 2017 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: 
  $DateTime: 2023/05/09 08:36:53 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/11/18    mks    NIPD fixers
09/25/13    bh     Created module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "amssassert.h"
#include "ds_qmi_nipd.h"
#include "ds_system_heap.h"
#include "ps_sys.h"
#include "data_msg.h"

#include "qmi_idl_lib.h"
#include "qmi_idl_lib_internal.h"
#include "qmi_csi.h"
#include "qmi_csi_target_ext.h"
#include "common_v01.h"
#include "mqcsi_log.h"

#include "non_ip_data_service_v01.h"
#include "qmi_sap.h"
#include "queue.h"
#include "qmi_framework.h"
#include "ds_rmnet_defs.h"
#include "ds_qmi_if.h"
#include "ds_qmux.h"
#include "ds_rmnet_meta_sm.h"
#include "ds_rmnet_xport.h"
#include "ds_bearer_manager.h"
#include "ds_qmi_defs.h"
#include "ds_sys_event.h"
#include <stringl/stringl.h>
#include "ps_metai_info.h"

/*---------------------------------------------------------------------------
  Service configuration
  - NIPD service version is 1.0
---------------------------------------------------------------------------*/
#define NIPDI_BASE_VER_MAJOR  (1)
#define NIPDI_BASE_VER_MINOR  (1)

#define NIPDI_ADDENDUM_VER_MAJOR  (0)
#define NIPDI_ADDENDUM_VER_MINOR  (0)

#define NIPDI_MAX_CLIDS    (QMI_FRAMEWORK_SVC_MAX_CLIENTS - 1)
#define NIPDI_QCSI_OS_SIGNAL ((rex_sigs_type)(1 << DS_NIPD_RDR_SIGNAL))

#define NIPDI_MAX_PAYLOAD_LEN  2000

/*To Verify whether the call is up or not on passed rmnet_instance*/
#define QMI_NIPDI_IS_CALL_UP(rmnet_instance) \
                        if(rmnet_meta_sm_in_call(rmnet_instance) == FALSE)\
                         {\
                           resp_msg.resp.error = QMI_ERR_OUT_OF_CALL_V01;\
                           DS_MSG1(MSG_LEGACY_ERROR,"Call is not Up on rmnet instance %d", rmnet_instance);\
                           goto bail;\
                         }

#define QMI_NIPDI_GET_RMNET_INSTANCE_FROM_EP_AND_MUX_ID(ep_id,mux_id,rmnet_instance)\
                         rmnet_instance = rmnet_inst_from_ep_and_mux_id(ep_id, mux_id, 1,QMI_AF_NIPD);\
                         if (rmnet_instance >= RMNET_INSTANCE_MAX)\
                         {\
                           resp_msg.resp.error = QMI_ERR_OUT_OF_CALL_V01;\
                           DS_MSG2(MSG_LEGACY_ERROR,"Invalid EP id 0x%x and mux_id %d", ep_id, mux_id);\
                           goto bail;\
                         }

/*---------------------------------------------------------------------------
  QMI NIPD Client state info
---------------------------------------------------------------------------*/
typedef struct {
  q_link_type                   link;           /*< Data Structure required by the queue 
                                              library to add this as part of a 
                                              queue */
  qmi_client_handle             clnt;
  uint32                        ep_id;          // Current binding physical end point
  uint8                         mux_id;         // Current (binding) Mux Id
  rmnet_instance_e_type         rmnet_instance; // Current associated rmnet port
  ps_sys_subscription_enum_type subscription;
  boolean                       is_already_bound;
  boolean                       report_down_link_data_ind; //indication registration value  
  boolean                       report_data_delivery_ind; //indication registration value
}qmi_nipdi_client_info_type;

/*---------------------------------------------------------------------------
  QMI NIPD Service state info
---------------------------------------------------------------------------*/
typedef struct
{
  q_type                 client_q; /*< Information about all the clients that
                                       have connected to the service. This is queue 
                                       of type qmi_nipdi_client_info_type */
  qmi_csi_service_handle service_handle;
  qmi_sap_client_handle  sap_client_handle;
  uint16                 num_of_clients;
} qmi_nipdi_state_info_type;


/*---------------------------------------------------------------------------
  NIPD commands
---------------------------------------------------------------------------*/
typedef enum
{
  QMI_NIPDI_CMD_MIN_VAL = -1, 
  QMI_CMD_NIPD_UL_DATA_DELIVERY_EVT = 0,
  QMI_NIPDI_CMD_MAX_VAL = 0x7FFFFFFF
} qmi_nipdi_cmd_type;


static qmi_nipdi_state_info_type     qmi_nipd_state;
static qmi_csi_os_params             os_params;
static boolean                       qmi_nipdi_inited = FALSE;
static ps_tx_meta_info_type         *tx_nipd_meta_info_ptr = NULL;

/*===========================================================================

                               INTERNAL DATA

===========================================================================*/

/*---------------------------------------------------------------------------
  QMI service command handlers
  forward declarations & cmd handler dispatch table definition
---------------------------------------------------------------------------*/
static qmi_csi_cb_error qmi_nipdi_indication_register
(
  qmi_nipdi_client_info_type *clnt_info,
  qmi_req_handle             req_handle,
  int                        msg_id,
  void                      *req_c_struct,
  int                        req_c_struct_len,
  void                      *service_handle
);
static qmi_csi_cb_error  qmi_nipdi_bind_client(
  qmi_nipdi_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);
static qmi_csi_cb_error  qmi_nipdi_get_client_binding(
  qmi_nipdi_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_nipdi_get_capability(
  qmi_nipdi_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_nipdi_uplink_data(
  qmi_nipdi_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);


static boolean qmi_nipdi_msg_hdlr_cb
(
  ds_sig_enum_type  sig,
  void              *svc_ptr
);

static qmi_csi_cb_error qmi_nipdi_svc_connect_cb
(
  qmi_client_handle          client_handle,
  void                      *service_handle,
  void                     **connection_handle
);

static void qmi_nipdi_svc_disconnect_cb
(
  void                      *connection_handle,
  void                      *service_handle
);

static qmi_csi_cb_error qmi_nipdi_handle_client_req_cb
(
  void                     *connection_handle,
  qmi_req_handle           req_handle,
  unsigned int             msg_id,
  void                     *req_c_struct,
  unsigned int             req_c_struct_len,
  void                     *service_handle
);


/*===========================================================================
  FUNCTION nipdi_ul_data_delivery_ind_cb()

  DESCRIPTION
    This the callback that we have registered with framewok layer for
    data delivery event. 

  PARAMETERS
    tech_type      : technology for which system status has changed
    event_name     : Event name
    event_info_ptr : System status event info
    user_data_ptr  : Optional user data to be sent.

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void nipdi_ul_data_delivery_ind_cb
(
  ds_sys_tech_enum_type                          tech_type,
  uint64                                         event_name,
  void                                         * event_info_ptr,  
  void                                         * user_data_ptr
);

/*===========================================================================
  FUNCTION qmi_nipdi_generate_ul_data_delivery_status_ind()

  DESCRIPTION
    This is the function to generate the daa delivery indication

  PARAMETERS
    ul_data_ind     : delivery info 

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_nipdi_generate_ul_data_delivery_status_ind
(
  ds_sys_3gpp_ul_data_delivery_ind_info_type      *ul_data_ind
);

/*===========================================================================

                       FORWARD FUNCTION DECLARATIONS 

===========================================================================*/


/* Define service handle table for NIPD messages */
static qmi_csi_cb_error (* const req_handle_table[])
(
 qmi_nipdi_client_info_type        *clnt_info,
 qmi_req_handle           req_handle,
 int                      msg_id,
 void                    *req_c_struct,
 int                      req_c_struct_len,
 void                     *service_handle
) = 
{
  NULL,NULL,NULL,                          /* Request handler for message ID 0x00 - 0x02*/
  qmi_nipdi_indication_register,           /* Request handler for message ID 0x03 */
  NULL,NULL,NULL,NULL,                     /* Request handler for message ID 0x04 - 0x07*/
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, /* Request handler for message ID 0x08 - 0x0F*/
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, /* Request handler for message ID 0x10 - 0x17*/
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, /* Request handler for message ID 0x18 - 0x1F*/
  qmi_nipdi_bind_client,                   /* Request handler for message ID 0x20 */
  qmi_nipdi_get_client_binding,            /* Request handler for message ID 0x21 */
  qmi_nipdi_get_capability,                /* Request handler for message ID 0x22 */
  NULL,                                    /* Request handler for message ID 0x23 */
  qmi_nipdi_uplink_data,                   /* Request handler for message ID 0x24 */  
  NULL,                                    /* Request handler for message ID 0x25 */
};

extern rex_tcb_type dcc_tcb;

/* This is the default QMI_NIPD command buffer */
typedef struct
{
  uint16  cmd_id; /* qmi_cmd_id_e_type */
  ds_sys_3gpp_ul_data_delivery_ind_info_type  ul_data_ind;

} qmi_nipdi_cmd_buf_type;

static int nipdi_compare_qmi_client_handle
( 
  void *item_ptr,
  void *compare_val 
)
{
  qmi_nipdi_client_info_type *clnt_info = (qmi_nipdi_client_info_type*)item_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ( (clnt_info->clnt == 
                          (qmi_client_handle)compare_val) ? 1 : 0 );
}

static int nipdi_compare_connection_info_handle
( 
  void *p_ItemPtr,
  void *p_CompareVal 
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* item_ptr is an element of the Client Q i.e. Connection Handle, 
     and compare val is also a Connection Handle */
  return ( (p_ItemPtr == p_CompareVal) ? 1 : 0 );
}

static void qmi_nipdi_reg_sys_events(void);

/*===========================================================================

                       EXTERNAL FUNCTION DEFINTIONS

===========================================================================*/

/*===========================================================================
  FUNCTION QMI_NIPD_INIT()

  DESCRIPTION
    Register the NIPD service with QCSI 

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_nipd_init
(
  void
)
{
  qmi_csi_error         rc;
  qmi_sap_error         src;
  qmi_csi_options       options;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG1(MSG_LEGACY_MED,"QMI NIPD service init %d", qmi_nipdi_inited);

  if (!qmi_nipdi_inited)
  {
    /*-------------------------------------------------------------------------
      Setting DCC signal handler for handling QMI messages
    -------------------------------------------------------------------------*/
    (void)reg_set_sig_handler(DS_NIPD_RDR_SIGNAL, 
                              qmi_nipdi_msg_hdlr_cb, &qmi_nipd_state);
    //ds_enable_sig_ex(DS_NIPD_RDR_SIGNAL);

    /* Set the signal to init again */
    //DS_SET_SIGNAL(DS_NIPD_RDR_SIGNAL);
    (void)rex_set_sigs( ds_get_ds_task_ptr(),
                     (rex_sigs_type) 1 << (rex_sigs_type)(DS_NIPD_RDR_SIGNAL) ); 
    return;
  }

  os_params.tcb = ds_get_ds_task_ptr();
  os_params.sig = NIPDI_QCSI_OS_SIGNAL;
  QMI_CSI_OPTIONS_INIT(options);
  (void)mqcsi_register_log_options(&options);

  /*-------------------------------------------------------------------------
    Register QMI NIPD service with QCSI
  -------------------------------------------------------------------------*/
  rc = qmi_csi_register_with_options(nipd_get_service_object_v01(),
                        qmi_nipdi_svc_connect_cb,
                        qmi_nipdi_svc_disconnect_cb, 
                        qmi_nipdi_handle_client_req_cb, 
                        &qmi_nipd_state, &os_params,
                        &options,
                        &qmi_nipd_state.service_handle);
  
  if(rc != QMI_CSI_NO_ERR)
  {
    ERR_FATAL("Unable to register nipd service! Error %d", rc, 0, 0);
    DS_ASSERT(0);
    return;
  }

  //register with SAP
  src = qmi_sap_register(nipd_get_service_object_v01(), NULL, &qmi_nipd_state.sap_client_handle);
  if (src != QMI_SAP_NO_ERR)
  {
    ERR_FATAL("Unable to register nipd service with QSAP! Error %d", src, 0, 0);
    DS_ASSERT(0);
    return;
  }

  /* Initialize the queue which will hold all handles of clients */
  (void) q_init(&qmi_nipd_state.client_q);
  qmi_nipd_state.num_of_clients = 0;

/*-------------------------------------------------------------------------
   Setting QMI NIPD service command handler with DCC task process
-------------------------------------------------------------------------*/
  //dcc_set_cmd_handler(DS_NIPD_SVC_CMD, qmi_nipdi_process_cmd);

  /*-------------------------------------------------------------------------
    Pre-allocate meta info at powerup so that it is not allocated every time
    in data path. This meta info is DUPed and sent along with each NON-IP pkt in
    UL QMI path
  -------------------------------------------------------------------------*/
  if(NULL == tx_nipd_meta_info_ptr)
  {
    PS_TX_META_INFO_GET_ALL(tx_nipd_meta_info_ptr);
    
    if (NULL == tx_nipd_meta_info_ptr)
    {
      DS_ERR_FATAL("qmi_nipd_init(): Couldn't alloc meta info");
      return;
    }
  }
  qmi_nipdi_reg_sys_events();
  
} /* qmi_nipd_init */

static boolean qmi_nipdi_msg_hdlr_cb
(
  ds_sig_enum_type  sig,
  void              *svc_ptr
)
{
  qmi_nipdi_state_info_type *nipd_sp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_ASSERT((sig == DS_NIPD_RDR_SIGNAL) && svc_ptr);

  if (!qmi_nipdi_inited)
  {
    qmi_nipdi_inited = TRUE;
    /* Call init again to register with QCSI now
     * Wait for all tasks to be inited */
    qmi_nipd_init();
    return TRUE;
  }

  nipd_sp = (qmi_nipdi_state_info_type*) svc_ptr;

  qmi_csi_handle_event(nipd_sp->service_handle, &os_params);
  return TRUE;
} /* qmi_nipdi_msg_hdlr_cb */

static qmi_csi_cb_error qmi_nipdi_svc_connect_cb
(
  qmi_client_handle          client_handle,
  void                      *service_handle,
  void                     **connection_handle
)
{
  qmi_nipdi_client_info_type    *clnt_info_ptr;
  qmi_nipdi_state_info_type     *nipd_svc_ptr;
  qmi_csi_cb_error               rc;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  DS_ASSERT(connection_handle != NULL);
  DS_ASSERT(service_handle != NULL);

  nipd_svc_ptr = (qmi_nipdi_state_info_type *) service_handle;
 
  if ( NULL != q_linear_search( &nipd_svc_ptr->client_q,
                                nipdi_compare_qmi_client_handle,
                                client_handle ) )
  {
    DS_MSG1(MSG_LEGACY_ERROR,"nipdi_svc_connect_cb: nipd client 0x%p present",
                  client_handle);
    rc = QMI_CSI_CB_CONN_REFUSED;
    goto bail;
  }

  if (NIPDI_MAX_CLIDS == nipd_svc_ptr->num_of_clients)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"qmi_nipdi_svc_connect_cb: Maximum number of clients already allocated %d",
                  NIPDI_MAX_CLIDS);
    rc = QMI_CSI_CB_CONN_REFUSED;
    goto bail;
  }

  clnt_info_ptr = ds_system_heap_mem_alloc(
                    sizeof(qmi_nipdi_client_info_type),__FILENAME__ , __LINE__);
  if(!clnt_info_ptr)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"qmi_nipdi_svc_connect_cb: Out of mem");
    rc = QMI_CSI_CB_CONN_REFUSED;
    goto bail;
  }

  memset(clnt_info_ptr, 0, sizeof(qmi_nipdi_client_info_type));

  clnt_info_ptr->rmnet_instance = RMNET_INSTANCE_MIN;
  clnt_info_ptr->subscription = (ps_sys_subscription_enum_type) NIPD_PRIMARY_SUBS_V01;
  clnt_info_ptr->is_already_bound = FALSE;

  (void) q_link ( clnt_info_ptr, &clnt_info_ptr->link );
  clnt_info_ptr->clnt = client_handle;
  /* Add it to the queue of clients */
  q_put( &(nipd_svc_ptr->client_q), &(clnt_info_ptr->link) );
  nipd_svc_ptr->num_of_clients++;

  *connection_handle = clnt_info_ptr;
  DS_MSG1(MSG_LEGACY_MED,"qmi_nipdi_svc_connect_cb: Alloc client 0x%p", clnt_info_ptr);
  rc = QMI_CSI_CB_NO_ERR;
bail:
  return rc;
} /* qmi_nipdi_svc_connect_cb() */

static void qmi_nipdi_svc_disconnect_cb
(
  void                      *connection_handle,
  void                      *service_handle
)
{
  qmi_nipdi_state_info_type     *nipd_svc_ptr;
  qmi_nipdi_client_info_type    *clnt_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ASSERT(connection_handle !=NULL);
  DS_ASSERT(service_handle != NULL);
  nipd_svc_ptr = (qmi_nipdi_state_info_type *)service_handle;

  DS_MSG1(MSG_LEGACY_MED,"qmi_nipdi_svc_disconnect_cb: Releasing client 0x%p", 
                connection_handle);

  clnt_info_ptr = q_linear_search( &nipd_svc_ptr->client_q,
                                   nipdi_compare_connection_info_handle,
                                   connection_handle );
  if(NULL == clnt_info_ptr)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"qmi_nipdi_svc_disconnect_cb: nipd client 0x%p not found",
                  connection_handle);
    return;

  }

  if(TRUE != q_delete_ext( &nipd_svc_ptr->client_q,
                             &clnt_info_ptr->link ) )
  {
    DS_MSG1(MSG_LEGACY_ERROR,"qmi_nipdi_svc_disconnect_cb: nipd client 0x%p could not be deleted from queue",
                    clnt_info_ptr);
    return;
  }
  DS_SYSTEM_HEAP_MEM_FREE(clnt_info_ptr);
  nipd_svc_ptr->num_of_clients--;
} /* qmi_nipdi_svc_disconnect_cb() */


/*===========================================================================
  FUNCTION QMI_NIPDI_PROCESS_CMD()

  DESCRIPTION
    This function processes a QMI NIPD command

    It is called by the QMI command handler and will dispatch the
    associated command/event handler function.

  PARAMETERS
    cmd_ptr:  private data buffer containing the QMI NIPD command
              information.

  RETURN VALUE
    None

  DEPENDENCIES
    QMI NIPD must already have been initialized

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_nipdi_process_cmd
(
  ds_command_enum_type  cmd,
  void              *cmd_ptr
)
{

  qmi_nipdi_cmd_buf_type *cmd_buf_ptr = NULL;
  /*-------------------------------------------------------------------------*/

  DS_ASSERT(cmd_ptr != NULL);
  cmd_buf_ptr = (qmi_nipdi_cmd_buf_type *)cmd_ptr;

  DS_MSG1(MSG_LEGACY_HIGH,"qmi_nipdi_process_cmd: cmdbuf's cmd_id:%d",cmd_buf_ptr->cmd_id);

  switch(cmd_buf_ptr->cmd_id)
  {
    case QMI_CMD_NIPD_UL_DATA_DELIVERY_EVT:
      qmi_nipdi_generate_ul_data_delivery_status_ind
      (
        &(cmd_buf_ptr->ul_data_ind)
      );
      break;


   default:
   {
     DS_ASSERT(0); 
   }   
   break;
      
  }
} /* qmi_nipdi_process_cmd() */

static qmi_csi_cb_error qmi_nipdi_handle_client_req_cb
(
  void                     *connection_handle,
  qmi_req_handle           req_handle,
  unsigned int             msg_id,
  void                     *req_c_struct,
  unsigned int             req_c_struct_len,
  void                     *service_handle
)
{
  qmi_csi_cb_error                rc;
  qmi_nipdi_client_info_type     *clnt_info_ptr;
  qmi_nipdi_state_info_type      *nipd_svc_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ASSERT(connection_handle != NULL);
  DS_ASSERT(service_handle !=NULL);

  rc = QMI_CSI_CB_INTERNAL_ERR;
  nipd_svc_ptr = (qmi_nipdi_state_info_type*) service_handle;

  clnt_info_ptr = q_linear_search( &nipd_svc_ptr->client_q,
                                   nipdi_compare_connection_info_handle,
                                   connection_handle );
  if(NULL == clnt_info_ptr)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"qmi_nipdi_handle_client_req_cb: Invalid clnt 0x%p",
                  connection_handle);
    return rc;
  }

  if(msg_id < (sizeof(req_handle_table) / sizeof(*req_handle_table)))
  {
    if(req_handle_table[msg_id])
    {
      rc = req_handle_table[msg_id] (clnt_info_ptr, req_handle, msg_id,
                                     req_c_struct, req_c_struct_len,
                                     service_handle);
    }
    else
    {
      DS_MSG1(MSG_LEGACY_ERROR,"qmi_nipdi_handle_client_req_cb: NULL message ID handler: %d",
                    msg_id);
    }
  }
  else
  {
    DS_MSG1(MSG_LEGACY_ERROR,"qmi_nipdi_handle_client_req_cb: Invalid message ID: %d", msg_id);
  }

  return rc;
} /* qmi_nipdi_handle_client_req_cb() */

/*===========================================================================
  FUNCTION QMI_NIPD_GET_CAPABILITY()

  DESCRIPTION
    Get the capability for the client.

  PARAMETERS
    req_c_struct: Request Message with all TLV
    req_handle  : Request Handle
    clnt_info   : Client Info Handle

  RETURN VALUE
    QMI_CSI_CB_ERROR types.

  DEPENDENCIES
    qmi_nipd_init has to be called.

  SIDE EFFECTS
    None
===========================================================================*/

static qmi_csi_cb_error  qmi_nipdi_get_capability
(
  qmi_nipdi_client_info_type        *clnt_info,
  qmi_req_handle                     req_handle,
  int                                msg_id,
  void                              *req_c_struct,
  int                                req_c_struct_len,
  void                              *service_handle
)
{
  nipd_get_capability_resp_msg_v01   resp_msg;
  uint32                             max_payload_len = NIPDI_MAX_PAYLOAD_LEN;
/*-----------------------------------------------------------------------------*/
  DS_ASSERT(req_c_struct && (req_c_struct_len > 0) && clnt_info && service_handle);
  memset(&resp_msg, 0, sizeof(nipd_get_capability_resp_msg_v01));

  resp_msg.allowed_data_payload_length_valid = TRUE;
  resp_msg.allowed_data_payload_length = max_payload_len;

  resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
  resp_msg.resp.error = QMI_ERR_NONE_V01;

  DS_MSG1(MSG_LEGACY_MED,"Sending max_payload_len %d", max_payload_len);
   
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(nipd_get_capability_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION QMI_NIPDI_BIND_DATA_PORT

  DESCRIPTION
    Bind the client to a given data port.

  PARAMETERS
    req_c_struct: Request Message with all TLV
    req_handle  : Request Handle
    clnt_info   : Client Info Handle

  RETURN VALUE
    QMI_CSI_CB_ERROR types.

  DEPENDENCIES
    qmi_nipd_init has to be called.

  SIDE EFFECTS
    None
===========================================================================*/

static qmi_csi_cb_error  qmi_nipdi_bind_client
(
  qmi_nipdi_client_info_type *clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void                       *req_c_struct,
  int                         req_c_struct_len,
  void                       *service_handle
)
{
  nipd_bind_client_req_msg_v01   *req_msg;
  nipd_bind_client_resp_msg_v01   resp_msg;
  uint32                          ep_id;
  uint8                           mux_id;
  ps_sys_subscription_enum_type   subscription;
  int                             errval = QMI_ERR_NONE_V01;
  boolean                         is_bound;
//------------------------------------------------------------------------------
  DS_ASSERT(req_c_struct && (req_c_struct_len > 0) && clnt_info && service_handle);

  ep_id  = clnt_info->ep_id;
  mux_id = clnt_info->mux_id;
  subscription = clnt_info->subscription;
  is_bound = clnt_info->is_already_bound;
  
  req_msg = (nipd_bind_client_req_msg_v01 *)req_c_struct;
  
  if (req_msg->ep_id_valid == TRUE)
  {
    if (clnt_info->is_already_bound == TRUE)
    {
      errval = QMI_ERR_NO_EFFECT_V01;
      goto bail;
    }
    if (QMI_EP_ID_VALID(req_msg->ep_id))
    {
        ep_id = QMI_EP_ID_TO_INTERNAL_EP_ID(req_msg->ep_id);
    }
    else
    {
      DS_MSG2(MSG_LEGACY_ERROR,"Invalid EP id 0x%x:0x%x", req_msg->ep_id.ep_type, req_msg->ep_id.iface_id);
      errval = QMI_ERR_INVALID_ARG;
      goto bail;
    }
      if (rmnet_phys_xport_from_ep_id(ep_id) == NULL)
      {
        DS_MSG2(MSG_LEGACY_ERROR,"Invalid EP id 0x%x:0x%x", req_msg->ep_id.ep_type, req_msg->ep_id.iface_id);
      errval = QMI_ERR_INVALID_ARG;
      goto bail;
    }
    if (req_msg->mux_id_valid == TRUE)
    {
      mux_id = (uint8)req_msg->mux_id;
    }
    else
    {
      mux_id = 0;
    }
    is_bound = TRUE;
  }
  if (req_msg->mux_id_valid == TRUE)
  {
    if((rmnet_phys_xport_from_ep_id(ep_id) == NULL))
    {
      DS_MSG3(MSG_LEGACY_ERROR,"Invalid EP id 0x%x:0x%x for mux id %d", 
        req_msg->ep_id.ep_type, req_msg->ep_id.iface_id, req_msg->mux_id);
      errval = QMI_ERR_INVALID_ARG;
      goto bail;
    }
    mux_id = (uint8)req_msg->mux_id;
  }
  
  if (req_msg->bind_subs_valid == TRUE)
  {
    switch (req_msg->bind_subs)
    {
      case NIPD_PRIMARY_SUBS_V01:
      {
        subscription = PS_SYS_PRIMARY_SUBS;
        break;
      }
    
      case NIPD_SECONDARY_SUBS_V01:
      {
        subscription = PS_SYS_SECONDARY_SUBS;
        break;
      }
      case  NIPD_TERTIARY_SUBS_V01:
      {
        subscription = PS_SYS_TERTIARY_SUBS;
        break;
      }
      default:
      {
        DS_MSG1(MSG_LEGACY_MED,"Unknown Subscription received %d",
                        req_msg->bind_subs);
        errval = QMI_ERR_INVALID_ARG;
        break;
      }
    }
  }
bail:
  if (errval == QMI_ERR_NONE_V01)
  {
    clnt_info->ep_id = ep_id;
    clnt_info->mux_id = mux_id;
    clnt_info->is_already_bound = is_bound;
    clnt_info->subscription = subscription;
    DS_MSG2(MSG_LEGACY_MED,"The bound ep_id %d, mux_id %d ", ep_id, mux_id);
  }
  resp_msg.resp.result = (errval == QMI_ERR_NONE_V01) ? QMI_RESULT_SUCCESS_V01 : QMI_RESULT_FAILURE_V01;
  resp_msg.resp.error  = errval;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, sizeof(nipd_bind_client_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
}/*qmi_nipdi_bind_client*/

/*===========================================================================
  FUNCTION QMI_NIPDI_GET_CLIENT_BINDING

  DESCRIPTION
    Get the Client Binding data (Client Ip_pref and Client SIO Data port)

  PARAMETERS
    req_c_struct: Request Message with all TLV
    req_handle  : Request Handle
    clnt_info   : Client Info Handle

  RETURN VALUE
    QMI_CSI_CB_ERROR types.

  DEPENDENCIES
    qmi_nipd_init has to be called.

  SIDE EFFECTS
    None
===========================================================================*/

static qmi_csi_cb_error  qmi_nipdi_get_client_binding
(
  qmi_nipdi_client_info_type *clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void                       *req_c_struct,
  int                         req_c_struct_len,
  void                       *service_handle
)
{
  nipd_get_client_binding_resp_msg_v01  resp_msg;
//------------------------------------------------------------------------------
  DS_ASSERT(req_c_struct && (req_c_struct_len > 0) && clnt_info && service_handle);
  memset(&resp_msg, 0, sizeof(nipd_get_client_binding_resp_msg_v01));
  resp_msg.resp.error = QMI_ERR_NONE_V01;
  resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
  if (clnt_info->is_already_bound)
  {
    resp_msg.bound_ep_id_valid = TRUE;
    resp_msg.bound_mux_id_valid = TRUE;
    resp_msg.bound_ep_id = qmi_internal_ep_id_to_qmi_ep_id(clnt_info->ep_id);
    resp_msg.bound_mux_id = clnt_info->mux_id;
  }
  resp_msg.bound_subs_valid = TRUE;
  resp_msg.bound_subs = ( nipd_bind_subscription_enum_v01 )clnt_info->subscription;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, sizeof(nipd_get_client_binding_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION QMI_NIPDI_INDICATION_REGISTER()

  DESCRIPTION
    To Register for nipdi indications.
    report_powersave_filter_ind flag will be updated 
    in the client info based on input.

  PARAMETERS
    req_c_struct       :       Request Message with all TLV
    req_handle         :       Request Handle
    clnt_info          :       Client Info Handle
    msg_id             :       Message ID
    req_c_struct_len   :       length of request message
    service_handle     :       NIPD service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    qmi_nipd_init has to be called.

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_nipdi_indication_register
(
  qmi_nipdi_client_info_type       *clnt_info,
  qmi_req_handle                    req_handle,
  int                               msg_id,
  void                             *req_c_struct,
  int                               req_c_struct_len,
  void                             *service_handle
)
{
  /*--------------declarations --------------------*/
  nipd_indication_register_req_msg_v01   *req_msg;  
  nipd_indication_register_resp_msg_v01   resp_msg;
  boolean                                 is_already_conn = FALSE;
  qmi_nipdi_client_info_type*             client_info_ptr = NULL;
  /*--------------------------------------------------------*/
  DS_ASSERT(req_c_struct &&(req_c_struct_len > 0)&& clnt_info && service_handle);
  memset(&resp_msg, 0, sizeof(nipd_indication_register_resp_msg_v01));

  req_msg = (nipd_indication_register_req_msg_v01*)req_c_struct;
  
  if(req_msg->report_downlink_data_valid)
  {
    if(req_msg->report_downlink_data )
    {
      /*---------------------------------------------------------------------
      Traverse queue of clients to check if there is already a client registered for downlink
      data indication for same MUX ID and EP ID
      ----------------------------------------------------------------------*/
      client_info_ptr = (qmi_nipdi_client_info_type*)q_check(&qmi_nipd_state.client_q);
     
      while(client_info_ptr != NULL)
      {
        if ( client_info_ptr->report_down_link_data_ind == TRUE &&
             client_info_ptr->ep_id == clnt_info->ep_id &&
             client_info_ptr->mux_id == clnt_info->mux_id &&
             client_info_ptr != clnt_info )
        {
          is_already_conn = TRUE;
          DS_MSG4(MSG_LEGACY_MED," qmi_nipdi_indication_register client "
                          " is already register for downlink data ind prev "
                          " client ep id %d mux id %d curr client ep id %d mux id %d",
                          client_info_ptr->ep_id, client_info_ptr->mux_id,
                          clnt_info->ep_id, clnt_info->mux_id);
          break;
        }
        client_info_ptr = q_next(&qmi_nipd_state.client_q,
                               &(client_info_ptr->link));
      }
      if( FALSE == is_already_conn)
      {
        clnt_info->report_down_link_data_ind = TRUE;
      }
    }
    else
    {
      clnt_info->report_down_link_data_ind = FALSE;
    }
  }
    DS_MSG3(MSG_LEGACY_MED,"qmi_nipdi_indication_register valid flag %d  val %d is_already_req %d ", 
     req_msg->report_downlink_data_valid, req_msg->report_downlink_data, is_already_conn
     );
  if(req_msg->report_data_delivery_valid)
  {
    if(req_msg->report_data_delivery )
    {
      /*---------------------------------------------------------------------
      Traverse queue of clients to check if there is already a client registered for data 
      delivery indication for same MUX ID and EP ID
      ----------------------------------------------------------------------*/
      client_info_ptr = (qmi_nipdi_client_info_type*)q_check(&qmi_nipd_state.client_q);
     
      while(client_info_ptr != NULL)
      {
        if ( client_info_ptr->report_data_delivery_ind == TRUE &&
             client_info_ptr->ep_id == clnt_info->ep_id &&
             client_info_ptr->mux_id == clnt_info->mux_id &&
             client_info_ptr != clnt_info )
        {
          is_already_conn = TRUE;
          DS_MSG5(MSG_LEGACY_MED," qmi_nipdi_indication_register client "
                          " is already register for data delivery  ind prev "
                          " client ep id %d mux id %d curr client ep id %d mux id %d"
                          "client_info_ptr->report_data_delivery_ind %d",
                          client_info_ptr->ep_id, client_info_ptr->mux_id,
                          clnt_info->ep_id, clnt_info->mux_id,
                          client_info_ptr->report_data_delivery_ind);
          break;
        }
        client_info_ptr = q_next(&qmi_nipd_state.client_q,
                               &(client_info_ptr->link));
      }
      if( FALSE == is_already_conn)
      {
        clnt_info->report_data_delivery_ind = TRUE;
      }
    }
    else
    {
      clnt_info->report_data_delivery_ind = FALSE;
    }

  }

  DS_MSG3(MSG_LEGACY_MED,"qmi_nipdi_indication_register data delivery_valid %d, data deliver %d is_already_req %d", 
      req_msg->report_data_delivery_valid, req_msg->report_data_delivery,is_already_conn);

  if( FALSE == is_already_conn )
  {
    resp_msg.resp.error = QMI_ERR_NONE_V01;
  }
  else
  {
    resp_msg.resp.error = QMI_ERR_EISCONN_V01;
  }

  if (resp_msg.resp.error == QMI_ERR_NONE_V01)
  {
    resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
  }
  else
  {
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
  }
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(nipd_indication_register_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
}/*qmi_nipdi_indication_register*/

/*===========================================================================
  FUNCTION QMI_NIPDI_UPLINK_DATA

  DESCRIPTION
   client send uplink data

  PARAMETERS
    req_c_struct: Request Message with all TLV
    req_handle  : Request Handle
    clnt_info   : Client Info Handle

  RETURN VALUE
    QMI_CSI_CB_ERROR types.

  DEPENDENCIES
    qmi_nipd_init has to be called.

  SIDE EFFECTS
    None
===========================================================================*/

static qmi_csi_cb_error  qmi_nipdi_uplink_data(
  qmi_nipdi_client_info_type        *clnt_info,
  qmi_req_handle                     req_handle,        
  int                                msg_id,
  void                              *req_c_struct,
  int                                req_c_struct_len,
  void                              *service_handle
)
{
  /*--------------declarations --------------------*/
  nipd_uplink_data_req_msg_v01           *req_msg;         
  nipd_uplink_data_resp_msg_v01           resp_msg;
  rmnet_instance_e_type                   rmnet_instance     = RMNET_INSTANCE_MAX;
  ps_tx_meta_info_type                   *tx_meta_info_ptr   = NULL;
  dsm_item_type                          *rx_pkt             = NULL;
  dsm_item_type                         **rx_ptr_ptr         = NULL;
  ds_fwk_s_type                          *fwk_inst           = NULL;
  ds_bearer_mgr_s                        *def_bearer         = NULL;  
  ds_fwk_clnt_info_s_type                 clnt_info_ptr      = {0};  
  uint16                                  mtu_size;  
  boolean                                 is_mox_data_pkt    = FALSE;       
  /*--------------------------------------------------------*/
  DS_ASSERT(req_c_struct &&(req_c_struct_len > 0)&& clnt_info && service_handle);
  memset(&resp_msg, 0, sizeof(nipd_uplink_data_resp_msg_v01));

  req_msg = (nipd_uplink_data_req_msg_v01*)req_c_struct;
  /*---------------------------------------------------------------------
    Validate payload length
  ----------------------------------------------------------------------*/
  if( 0 == req_msg->payload_len || req_msg->payload_len > QMI_NIPD_MAX_PAYLOAD_SIZE_V01)
  {
    resp_msg.resp.error = QMI_ERR_MALFORMED_MSG_V01;
    goto bail;
  }
  /*---------------------------------------------------------------------
    Below mentioned optional TLVs are not supported
  ----------------------------------------------------------------------*/
  if( req_msg->segment_number_valid )
  {
    if ( req_msg->segment_number)
    {
       resp_msg.resp.error = QMI_ERR_NOT_SUPPORTED_V01;
       goto bail;
    }
  }
  if( req_msg->is_final_segment_valid )
  {
    if ( req_msg->is_final_segment)
    {
       resp_msg.resp.error = QMI_ERR_NOT_SUPPORTED_V01;
       goto bail;
    }
  }
  /*-----------------------------------------------------------------------
        1. Get RmNet instance 
        2. Data call should be Up
        3. NON-IP data call should be Up
        4. Allocate DSM item and copy payload
        5. Check the MTU size
        6. Get default flow ptr and check the flow status
        7. Dup TX meta info and set the filter results 
        8. Transmit the packet into NON-IP physical iface
  -----------------------------------------------------------------------*/ 

  /*---------------------------------------------------------------------
    Step 1 Get RmNet instance 
  ----------------------------------------------------------------------*/
  QMI_NIPDI_GET_RMNET_INSTANCE_FROM_EP_AND_MUX_ID(
                                        clnt_info->ep_id,
                                        clnt_info->mux_id,
                                        rmnet_instance);

  DS_MSG1(MSG_LEGACY_MED,"qmi_nipdi_uplink_data rmnet_instance %d", rmnet_instance);
  
  /*---------------------------------------------------------------------
    Step 2 Data call should be Up
  ----------------------------------------------------------------------*/
  QMI_NIPDI_IS_CALL_UP(rmnet_instance);

  ds_fwk_get_clnt_info_from_handle(
				DS_FWK_CLIENT_RMNET,rmnet_instance,&clnt_info_ptr);

  if(clnt_info_ptr.fw_inst == NULL)
  {   
     DS_MSG0(MSG_LEGACY_ERROR,"No framework instance could be found for this call");
     goto bail;
  }
  fwk_inst = clnt_info_ptr.fw_inst;
      
  /*---------------------------------------------------------------------
    Step 2 NON-IP PDNs(Ifaces) can use QMI path for data transfer
  ----------------------------------------------------------------------*/
  
  if ((NULL == fwk_inst->non_ip_pdn_ptr) ||(clnt_info_ptr.ip_type != NON_IP_ADDR))
  {
    resp_msg.resp.error = QMI_ERR_OUT_OF_CALL_V01;
    goto bail;
  }
  
  DS_MSG2(MSG_LEGACY_MED,"qmi_nipdi_uplink_data fwk_inst %x  IP TYPE is  %d", 
    fwk_inst,clnt_info_ptr.ip_type);

  /*---------------------------------------------------------------------
    Step 4 Check the MTU size. if packet size is greater then MTU size then we will discard 
    the packet
  ----------------------------------------------------------------------*/
  mtu_size = DS_FWK_GET_MTU(fwk_inst,clnt_info_ptr.ip_type);
  
  if ( mtu_size < req_msg->payload_len)
  {
    resp_msg.resp.error = QMI_ERR_INVALID_ARG_V01;
    goto bail;
  }
  /*---------------------------------------------------------------------
    Step 5 Get default flow ptr , check flow AND Mo_ex flow
  ----------------------------------------------------------------------*/  
  def_bearer = ds_fwk_get_default_bearer(fwk_inst,NON_IP_ADDR);
  if(def_bearer != NULL)
    {
      if((req_msg->is_mo_exceptional_data_valid) &&
         (req_msg->is_mo_exceptional_data))
      {
         is_mox_data_pkt = TRUE;
      }
      if(!ds_bearer_mgr_is_tx_flow_enabled_ex(def_bearer,is_mox_data_pkt))
      {
         resp_msg.resp.error = QMI_ERR_FLOW_SUSPENDED_V01;
         goto bail;
      }
    }
    else
    {
      resp_msg.resp.error = QMI_ERR_FLOW_SUSPENDED_V01;
      goto bail;
    }
  /*---------------------------------------------------------------------
    Step 3 Allocate DSM item for data payload
  ----------------------------------------------------------------------*/
   if ( NULL == ( rx_pkt = dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL)))
   {
     resp_msg.resp.error = QMI_ERR_ENOMEM_V01;
     goto bail;
   }  
   /*---------------------------------------------------------------------
     Step 4 Copy payload into DSM iteam
   ----------------------------------------------------------------------*/
  rx_ptr_ptr = &rx_pkt;

  if ( req_msg->payload_len != dsm_pushdown_tail( rx_ptr_ptr,
          &req_msg->payload, req_msg->payload_len, DSM_DS_LARGE_ITEM_POOL))
  {
    resp_msg.resp.error = QMI_ERR_INTERNAL_V01;
    goto bail;
  }

  DS_MSG0(MSG_LEGACY_MED,"qmi_nipdi_uplink_data flow ptr ");//ps_flow_ptr);
  /*---------------------------------------------------------------------
    Step 6 Dup TX meta info and set the filter results 
  ----------------------------------------------------------------------*/
   PS_TX_META_INFO_DUP(tx_nipd_meta_info_ptr, &tx_meta_info_ptr);

  /*-----------------------------------------------------------------------
    Step 6 Update meta info with routing cache and QoS filter result
  -----------------------------------------------------------------------*/
  PS_TX_META_SET_ROUTING_CACHE(tx_meta_info_ptr, fwk_inst);
  PS_TX_META_SET_FILTER_RESULT(tx_meta_info_ptr,
                               FLTR_CLIENT_QOS_OUTPUT,
                               (uint32) NULL);
 
  tx_meta_info_ptr->rt_meta_info_ptr->pkt_info.ip_vsn = NON_IP;
  /*---------------------------------------------------------------------
     Step 6 Set the filter result if data is MO exceptiona data.
  ----------------------------------------------------------------------*/
  if( req_msg->is_mo_exceptional_data_valid && req_msg->is_mo_exceptional_data)
  {
    DS_MSG0(MSG_LEGACY_MED,"qmi_nipdi_uplink_data MO Exceptional flag is set");
    PS_TX_META_SET_FILTER_RESULT( tx_meta_info_ptr, FLTR_CLIENT_MO_EXCP, TRUE);
  }
  else
  {
    PS_TX_META_SET_FILTER_RESULT( tx_meta_info_ptr, FLTR_CLIENT_MO_EXCP, FALSE);
  }
  /*---------------------------------------------------------------------
    Step 7 Transmit the packet into NON-IP physical iface
  ----------------------------------------------------------------------*/
  ds_fwk_default_tx_cmd( fwk_inst, 
                            rx_ptr_ptr, 
                            tx_meta_info_ptr
                            );

  resp_msg.resp.error = QMI_ERR_NONE_V01; 
  resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
  
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(nipd_uplink_data_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;

  bail:
  DS_MSG2(MSG_LEGACY_ERROR,"Unable to transmit the data rmnet_instance %d fwk_inst %x", rmnet_instance, fwk_inst);

  if ( NULL != rx_pkt)
  {
    dsm_free_packet( &rx_pkt );
  }
  
  resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
  
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(nipd_uplink_data_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
}/* qmi_nipdi_uplink_data */

/*===========================================================================
  FUNCTION QMI_NIPDI_DOWN_LINK_DATA_IND()

  DESCRIPTION
    Generate downlink indication and send to registered client

  PARAMETERS
    tx_pkt    : Pakcet to be transmitted
   um_iface_ptr : NON IP um iface ptr
   
  RETURN VALUE
    void.

  DEPENDENCIES
    qmi_nipdi_init has to be called.

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_nipdi_down_link_data_indication
(
  dsm_item_type                          **tx_pkt,
  ds_fwk_clnt_handle_type                 fwk_client
)
{
  /*--------------declarations ---------------------------------*/
  nipd_downlink_data_ind_msg_v01        * ind = NULL;
  qmi_csi_error                           rc;
  qmi_nipdi_client_info_type*             client_info_ptr = NULL;
  uint32                                  payload_len = 0;
  //ps_iface_type*                          temp_um_iface_ptr       = NULL;
  rmnet_instance_e_type                   rmnet_instance     = RMNET_INSTANCE_MAX;
  //ds_fwk_s_type                           *tmp_fwk_inst;
  /*----------------------------------------------------------*/
  if ( NULL == tx_pkt|| NULL == *tx_pkt || 0 == dsm_length_packet(*tx_pkt))
  {
    DS_MSG1(MSG_LEGACY_ERROR,"Nipd DL Ind Invalid Packet or length tx_pkt 0x%x ", tx_pkt);
    goto bail;
  }

  /*-----------------------------------------------------------------------
        1. Allocate memory for indication
        2. Copy payload from DSM item into indication
        3. Traverse queue of clients to report to the registered ones
        4. Get RmNet Instance and Um Iface ptr
        5. Sned indication on matched UM iface 
  -----------------------------------------------------------------------*/ 

  payload_len = dsm_length_packet(*tx_pkt);

   DS_MSG1(MSG_LEGACY_MED,"Nipd DL Ind Payload length  0x%d ", payload_len);
  
  if( NIPDI_MAX_PAYLOAD_LEN < payload_len)
  {
    //TODO Segment
     DS_MSG1(MSG_LEGACY_ERROR,"Nipd DL Ind Exceed MAX packet length tx_pkt 0x%x", payload_len);
     goto bail;
  }
  /*---------------------------------------------------------------------
    Step 1 Allocate memory for indication 
  ----------------------------------------------------------------------*/
  DS_SYSTEM_HEAP_MEM_ALLOC(ind, sizeof(nipd_downlink_data_ind_msg_v01),
         nipd_downlink_data_ind_msg_v01*);
  
  if (ind == NULL) 
  {
    DS_MSG0(MSG_LEGACY_ERROR,"Nipd DL Ind Unable to allocate memory ");
    goto bail;
  }

  ind->payload_len = payload_len;
  /*---------------------------------------------------------------------
    Step 2 Copy payload from DSM item into indication payload
  ----------------------------------------------------------------------*/
  if ( ind->payload_len != dsm_pullup_long ( tx_pkt, &ind->payload, payload_len) )
  {
    DS_MSG2(MSG_LEGACY_MED,"Nipd DL Ind Unable to pull long all bytes pulled byte %d payload_len %d",
    ind->payload_len, payload_len);
    goto bail;
  }
  /*---------------------------------------------------------------------
    Step 3 Traverse queue of clients to report to the registered ones
  ----------------------------------------------------------------------*/
  client_info_ptr = (qmi_nipdi_client_info_type*)q_check(&qmi_nipd_state.client_q);
  
  while(client_info_ptr != NULL)
  {
    /*---------------------------------------------------------------------
       Step 4 Get RmNet instance 
    ----------------------------------------------------------------------*/
   rmnet_instance = rmnet_inst_from_ep_and_mux_id(client_info_ptr->ep_id, client_info_ptr->mux_id, 
   1,QMI_AF_NIPD);

   DS_MSG1(MSG_LEGACY_MED," Nipd DL Ind sent data to rmnet_instance = %d", 
    rmnet_instance);

    if (rmnet_instance >= RMNET_INSTANCE_MAX)
    {
      client_info_ptr = q_next(&qmi_nipd_state.client_q,
                           &(client_info_ptr->link));
      continue;
    }
    if (client_info_ptr->report_down_link_data_ind == TRUE &&
        fwk_client == rmnet_instance )
    {
      /*---------------------------------------------------------------------
        Step 5 Send indication to registered client( Only one client can register)
      ----------------------------------------------------------------------*/
      rc = qmi_csi_send_ind(client_info_ptr->clnt, 
                          QMI_NIPD_DOWN_LINK_DATA_IND_V01,
                          (void*)ind, 
                          sizeof(nipd_downlink_data_ind_msg_v01));
      
      DS_MSG1(MSG_LEGACY_MED," Nipd DL Ind sent data to client = %0x", client_info_ptr);

      if ( rc != QMI_CSI_NO_ERR )
      {
        DS_MSG1(MSG_LEGACY_ERROR,"qmi_nipdi_down_link_data_indication error %d", rc);
        break;
      }
      
      break;
    }
    client_info_ptr = q_next(&qmi_nipd_state.client_q,
                           &(client_info_ptr->link));
  }
  if(ind != NULL)
  { 
    DS_SYSTEM_HEAP_MEM_FREE(ind);
  }
  return; 
  
  bail:

  DS_MSG1(MSG_LEGACY_ERROR,"qmi_nipdi_down_link_data_indication : "
                  "Unable to send downlink indication %d", payload_len);

  if(ind != NULL)
  { 
    DS_SYSTEM_HEAP_MEM_FREE(ind);
  }

  if(tx_pkt != NULL && *tx_pkt != NULL )
  { 
    dsm_free_packet( tx_pkt );
  }
  return;
}/*qmi_nipdi_down_link_data_indication*/



/*===========================================================================
FUNCTION  QMI_NIPDI_GENERATE_UL_DATA_DELIVERY_STATUS_IND

DESCRIPTION
  This callback is called to notify clients about data delivery indication .

PARAMETERS
  ul_data_ind - Data delovery ind 
  
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
static void qmi_nipdi_generate_ul_data_delivery_status_ind
(
  ds_sys_3gpp_ul_data_delivery_ind_info_type      *ul_data_ind
)
{
  qmi_nipdi_client_info_type                   * client_info_ptr = NULL;
  qmi_nipdi_state_info_type                    *nipd_sp;
  uint8                                         fwk_index = 0xFF;
  rmnet_instance_e_type                         rmnet_instance     = RMNET_INSTANCE_MAX;
  ds_fwk_s_type                                *fwk_inst           = NULL;
  ds_fwk_clnt_info_s_type                       clnt_info_ptr      = {0};      
  qmi_csi_error                                 rc;
  nipd_ul_data_delivery_ind_msg_v01             ind_msg;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(ul_data_ind == NULL)
  {
    DS_ASSERT(0);
  }
  DS_MSG2(MSG_LEGACY_HIGH,"qmi_nipdi_generate_ul_data_delivery_status_ind ,"
                  "fwk index %d,ul status %d ",
                  ul_data_ind->fwk_index,
                  ul_data_ind->delivery_status);

  nipd_sp         = &qmi_nipd_state;
  client_info_ptr = (qmi_nipdi_client_info_type*)q_check(&qmi_nipd_state.client_q);

  while(client_info_ptr != NULL)
  {
    if (TRUE != client_info_ptr->report_data_delivery_ind)
    {
       DS_MSG1(MSG_LEGACY_HIGH,"Capability invalid %d",client_info_ptr->report_data_delivery_ind);
       client_info_ptr = q_next(&qmi_nipd_state.client_q,
                           &(client_info_ptr->link));
       continue;
    }
    /*---------------------------------------------------------------------
      Step 1 Get RmNet instance 
  ----------------------------------------------------------------------*/
    rmnet_instance = rmnet_inst_from_ep_and_mux_id(client_info_ptr->ep_id, client_info_ptr->mux_id, 
                            1,QMI_AF_NIPD);
    if ((rmnet_instance >= RMNET_INSTANCE_MAX)||(rmnet_meta_sm_in_call(rmnet_instance) == FALSE))
    {
      DS_MSG3(MSG_LEGACY_HIGH,"Invalid EP id 0x%x and mux_id %d or rmnet instance %d", client_info_ptr->ep_id, client_info_ptr->mux_id,rmnet_instance);
      client_info_ptr = q_next(&qmi_nipd_state.client_q,
                           &(client_info_ptr->link));
      continue;
    }

    ds_fwk_get_clnt_info_from_handle(
                    DS_FWK_CLIENT_RMNET,rmnet_instance,&clnt_info_ptr);

    if(clnt_info_ptr.fw_inst == NULL)
    {   
      DS_ASSERT(0);
    }

    fwk_inst = clnt_info_ptr.fw_inst;

    fwk_index =  ul_data_ind->fwk_index;

  //Check if fwk inst is same as index 
    if(fwk_inst->ds_fwk_index != fwk_index)
    {
       DS_MSG2(MSG_LEGACY_HIGH,"Fwk index is not matching fwk_index %d fwk_inst->index %d",fwk_index, fwk_inst->ds_fwk_index);
	   client_info_ptr = q_next(&qmi_nipd_state.client_q,
                           &(client_info_ptr->link));
       continue;
    }                  
    else
    {
       if(ul_data_ind->delivery_status == DS_SYS_3GPP_DATA_ACK)
       {
          ind_msg.delivery_status = NIPD_DATA_DELIVERY_ACK_V01;
       }

	   else if(ul_data_ind->delivery_status == DS_SYS_3GPP_DATA_NACK)
       {
          ind_msg.delivery_status = NIPD_DATA_DELIVERY_NACK_V01;
       }
       else
       {
         return;
       }

       rc = qmi_csi_send_ind(client_info_ptr->clnt, 
                             QMI_NIPD_UL_DATA_DELIVERY_IND_V01,
                             &ind_msg, 
                              sizeof(nipd_ul_data_delivery_ind_msg_v01));
        
       if ( rc != QMI_CSI_NO_ERR )
       {
          DS_MSG1(MSG_LEGACY_HIGH,"qmi_dsdi_system_status_ind error %d", rc);
       }       
      client_info_ptr = q_next(&qmi_nipd_state.client_q,
                             &(client_info_ptr->link));
     }
  }
}
 
/*===========================================================================
  FUNCTION QMI_NIPDI_REG_SYS_EVENTS()

  DESCRIPTION
    Registers for framework level events

  PARAMETERS
    None

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_nipdi_reg_sys_events
( 
  void
)
{
  uint64                   event_id   = DS_SYS_EVENT_3GPP_UL_DATA_DELIVERY_INFO;
  int16                    result = 0;
  int16                    dss_errno = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  result = ds_system_event_reg (DS_SYS_TECH_3GPP,
                                DS_SYS_EVENT_3GPP_UL_DATA_DELIVERY_INFO,
                                nipdi_ul_data_delivery_ind_cb,
                                NULL,
                                &dss_errno);
  if(result < 0)
  {
    DS_MSG2(MSG_LEGACY_ERROR,"Couldn't reg for nipd ul data delivery = %d Err : (%d)",
            event_id, dss_errno);
    DS_ASSERT(0);
    return;
  }
}/* qmi_dsdi_reg_sys_events */


/*===========================================================================
  FUNCTION nipdi_ul_data_delivery_ind_cb()

  DESCRIPTION
    This the callback that we have registered with PS Sys layer for
    system status change event. 

  PARAMETERS
    tech_type      : technology for which system status has changed
    event_name     : Event name
    event_info_ptr : System status event info
    user_data_ptr  : Optional user data to be sent.

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void nipdi_ul_data_delivery_ind_cb
(
  ds_sys_tech_enum_type                          tech_type,
  uint64                                         event_name,
  void                                         * event_info_ptr,  
  void                                         * user_data_ptr
)
{
  qmi_nipdi_cmd_buf_type                       *cmd_payload = NULL;
  ds_sys_3gpp_ul_data_delivery_ind_info_type   *ul_system_status;  
  ds_sys_3gpp_ul_data_delivery_ind_info_type   *ul_data_status;
  ds_command_type                             *cmd_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ASSERT((int)event_info_ptr);

  ul_system_status = (ds_sys_3gpp_ul_data_delivery_ind_info_type *)event_info_ptr;

  /*-----------------------------------------------------------------------
   Get a DS command buffer
   -----------------------------------------------------------------------*/
   cmd_ptr = ds_allocate_cmd_buf(sizeof(qmi_nipdi_cmd_buf_type));
   if ((cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL))
   {
     DS_ERR_FATAL("Out of memory");
     return;
    }              
    cmd_ptr->hdr.cmd_id = DS_QMI_NIPD_SVC_CMD;
    cmd_payload = (qmi_nipdi_cmd_buf_type*)cmd_ptr->cmd_payload_ptr;
	
    ul_data_status = &cmd_payload->ul_data_ind;
    ul_data_status->delivery_status = ul_system_status->delivery_status;
    ul_data_status->fwk_index = ul_system_status->fwk_index;
	
  DS_MSG5(MSG_LEGACY_HIGH,"nipdi_ul_data_delivery_ind_cb cmd_id  %d ul_system_status %d , fwk_index %d %d,%d",
  	                      cmd_ptr->hdr.cmd_id,ul_system_status->delivery_status,
  	                      ul_system_status->fwk_index,ul_data_status->delivery_status,
  	                      ul_data_status->fwk_index);

  cmd_payload->cmd_id = QMI_CMD_NIPD_UL_DATA_DELIVERY_EVT;
  ds_put_cmd(cmd_ptr);
  return;
} /* nipdi_ul_data_delivery_ind_cb() */


