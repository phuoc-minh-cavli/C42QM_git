/*===========================================================================

                         D S _ Q M I _ D S D . C

DESCRIPTION

  The Data Services QMI Data System Determination service source file.

EXTERNALIZED FUNCTIONS

  qmi_dsd_init()
    Initialize the QMI DSD service

Copyright (c) 2013-2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/interface/qmidata/src/ds_qmi_dsd.c#1 $
  $DateTime: 2020/03/17 08:03:16 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/06/17    ss     Return appropriate error code when message is not suppoted
03/14/14    sah    Added support for limit_so_mask_change_ind in system status
                   change request.
11/07/12    sj     Created module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "amssassert.h"

#include "ds_qmi_dsd.h"
#include "ds_system_heap.h"
#include "ds_sys.h"
#include "ds_sys_event.h"
#include "ds_sys_ioctl.h"
#include "data_msg.h"
#include "qmi_idl_lib.h"
#include "qmi_idl_lib_internal.h"
#include "qmi_csi.h"
#include "qmi_csi_target_ext.h"
#include "mqcsi_log.h"
#include "common_v01.h"
#include "data_system_determination_v01.h"
#include "qmi_sap.h"
#include "queue.h"
#include "qmi_framework.h"
#include <stringl/stringl.h>

#include "event_defs.h"
#include "event.h"           /* For logging DIAG event and event codes      */

/*===========================================================================

                            CONSTANT DEFINITIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Service management
---------------------------------------------------------------------------*/
#define DSDI_MAX_CLIDS    (QMI_FRAMEWORK_SVC_MAX_CLIENTS - 1)
#define DSDI_INVALID_INSTANCE    -1

/*---------------------------------------------------------------------------
  Major and Minor Version Nos for DSD
---------------------------------------------------------------------------*/
#define DSD_BASE_VER_MAJOR    (1)
#define DSD_BASE_VER_MINOR    (7)

#define DSD_ADDENDUM_VER_MAJOR  (0)
#define DSD_ADDENDUM_VER_MINOR  (0)

#define DSDI_QCSI_OS_SIGNAL ((rex_sigs_type)(1 << DS_DSD_RDR_SIGNAL))

/*===========================================================================

                                DATA TYPES

===========================================================================*/

/*---------------------------------------------------------------------------
  QMI DSD Client state info
---------------------------------------------------------------------------*/
typedef struct {
  q_link_type                  link; /*< Data Structure required by the queue 
                                         library to add this as part of a 
                                         queue */
  qmi_client_handle clnt; 
  struct
  {
    boolean     report_system_status;
    boolean     limit_so_mask_change_report_ind;
  } report_status;
}qmi_dsdi_client_info_type;

/* Global variable required to track last avail sys pref network */
ds_sys_status_type  qmi_dsdi_last_avail_sys_pref_network;

/*---------------------------------------------------------------------------
  QMI DSD Service state info
---------------------------------------------------------------------------*/
typedef struct
{
  q_type                 client_q; /*< Information about all the clients that
                                       have connected to the service. This is queue 
                                       of type qmi_dsdi_client_info_type */
  qmi_csi_service_handle service_handle;
  qmi_sap_client_handle  sap_client_handle;
  uint16                 num_of_clients;
} qmi_dsdi_state_info_type;

/*---------------------------------------------------------------------------
  DSD commands
---------------------------------------------------------------------------*/
typedef enum
{
  QMI_DSDI_CMD_MIN_VAL = -1,	
  QMI_CMD_DSD_SYSTEM_STATUS_CHANGE_EVT = 0,
  QMI_DSDI_CMD_MAX_VAL = 0x7FFFFFFF
} qmi_dsdi_cmd_type;

static qmi_dsdi_state_info_type      qmi_dsd_state;
static qmi_csi_os_params             os_params;
static boolean                       qmi_dsdi_inited = FALSE;


/* This is the default QMI_DSD command buffer. Make sure that it does not grow
 * beyond 512 bytes. In case you need a command buffer larger than 512 bytes,
 * declare a separate structure. */
typedef struct
{
  uint16  cmd_id; /* qmi_cmd_id_e_type */
    dsd_system_status_ind_msg_v01 system_status;
} qmi_dsdi_cmd_buf_type;



/*===========================================================================

                               INTERNAL DATA

===========================================================================*/

/*---------------------------------------------------------------------------
  QMI service command handlers
  forward declarations & cmd handler dispatch table definition
---------------------------------------------------------------------------*/
static boolean qmi_dsdi_msg_hdlr_cb
(
  ds_sig_enum_type  sig,
  void              *svc_ptr
);

static qmi_csi_cb_error qmi_dsdi_svc_connect_cb
(
  qmi_client_handle          client_handle,
  void                      *service_handle,
  void                     **connection_handle
);

static void qmi_dsdi_svc_disconnect_cb
(
  void                      *connection_handle,
  void                      *service_handle
);

static qmi_csi_cb_error qmi_dsdi_handle_client_req_cb
(
  void                     *connection_handle,
  qmi_req_handle           req_handle,
  unsigned int             msg_id,
  void                     *req_c_struct,
  unsigned int             req_c_struct_len,
  void                     *service_handle
);

static qmi_csi_cb_error  qmi_dsdi_get_system_status
( 
  qmi_dsdi_client_info_type   * clnt_info,
  qmi_req_handle                req_handle,
  int                           msg_id,
  void                        * req_c_struct,
  int                           req_c_struct_len,
  void                        * service_handle
);  

static qmi_csi_cb_error  qmi_dsdi_system_status_change_req
(
  qmi_dsdi_client_info_type   * clnt_info,
  qmi_req_handle                req_handle,
  int                           msg_id,
  void                        * req_c_struct,
  int                           req_c_struct_len,
  void                        * service_handle
);


static void qmi_dsdi_system_status_ind
(
  void                                  * client,
  dsd_system_status_ind_msg_v01         * system_status
);

/*===========================================================================

                       FORWARD FUNCTION DECLARATIONS 

===========================================================================*/


/* Define service handle table for DSD messages */
static qmi_csi_cb_error (* const req_handle_table[])
(
 qmi_dsdi_client_info_type        *clnt_info,
 qmi_req_handle           req_handle,
 int                      msg_id,
 void                    *req_c_struct,
 int                      req_c_struct_len,
 void                     *service_handle
) = 
{
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, /* Request handler for message ID 0x00 - 0x07*/
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, /* Request handler for message ID 0x08 - 0x0F*/
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, /* Request handler for message ID 0x10 - 0x17*/
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, /* Request handler for message ID 0x18 - 0x1F*/
  NULL,NULL,NULL,NULL,					   /* Request handler for message ID 0x20 - 0x23*/
  qmi_dsdi_get_system_status,              /* Request handler for message ID 0x24 */
  qmi_dsdi_system_status_change_req,       /* Request handler for message ID 0x25 */
  NULL,
  NULL,						              /* Request handler for message ID 0x27 */
  NULL,							          /* Request handler for message ID 0x28 */
  NULL,								      /* Request handler for message ID 0x29 */
  NULL,						             /* Request handler for message ID 0x2A */
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,                   				/* Request handler for message ID 0x32 */  
  NULL,                   				/* Request handler for message ID 0x33 */ 
  NULL,           						/* Request handler for message ID 0x34 */ 
  NULL,              					/* Request handler for message ID 0x35 */
  NULL,                                 /* Request handler for message ID 0x36 */ 
  NULL,
  NULL,            						/* Request handler for message ID 0x38 */ 
  NULL,                                    /* Request handler for message ID 0x39 */ 
  NULL,                                    /* Request handler for message ID 0x3A */
  NULL,NULL,
  NULL,              						/* Request handler for message ID 0x3D */
  NULL,
  NULL,                                    /* Request handler for message ID 0x3F */
  NULL,                                    /* Request handler for message ID 0x40 */
  NULL,                                    /* Request handler for message ID 0x41 */
  NULL,
  NULL,NULL,
  NULL                                     /* Request handler for message ID 0x45 */
};

static int dsdi_compare_qmi_client_handle
( 
  void *item_ptr,
  void *compare_val 
)
{
  qmi_dsdi_client_info_type *clnt_info = (qmi_dsdi_client_info_type*)item_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ( (clnt_info->clnt == (qmi_client_handle)compare_val) ? 1 : 0 );
}

static int dsdi_compare_connection_info_handle
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

static void qmi_dsdi_system_status_ind
(
  void                                  * client,
  dsd_system_status_ind_msg_v01         * system_status
);

static void dsdi_system_status_change_cb
(
  ds_sys_tech_enum_type                          tech_type,
  uint64                                         event_name,
  void                                         * event_info_ptr,
  void                                         * user_data_ptr
);

static void qmi_dsdi_generate_system_status_ind
(
  dsd_system_status_ind_msg_v01  * system_status
);

static void qmi_dsdi_reg_sys_events(void);

/*===========================================================================

                       EXTERNAL FUNCTION DEFINTIONS

===========================================================================*/

/*===========================================================================
  FUNCTION QMI_DSD_INIT()

  DESCRIPTION
    Register the DSD service with QCSI 

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_dsd_init
(
  void
)
{
  qmi_csi_error         rc;
  qmi_sap_error         src;
  qmi_csi_options       options;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG1(MSG_LEGACY_MED,"QMI DSD service init %d", qmi_dsdi_inited);

  if (!qmi_dsdi_inited)
  {
    /*-------------------------------------------------------------------------
      Setting DCC signal handler for handling QMI messages
    -------------------------------------------------------------------------*/
    (void)reg_set_sig_handler(DS_DSD_RDR_SIGNAL, 
                              qmi_dsdi_msg_hdlr_cb, &qmi_dsd_state);
    (void)rex_set_sigs( ds_get_ds_task_ptr(),
                       (rex_sigs_type) 1 << (rex_sigs_type)( DS_DSD_RDR_SIGNAL ) ); 
    return;
  }

  os_params.tcb = ds_get_ds_task_ptr();
  os_params.sig = DSDI_QCSI_OS_SIGNAL;
  QMI_CSI_OPTIONS_INIT(options);
  (void)mqcsi_register_log_options(&options);
  
  /*-------------------------------------------------------------------------
    Register QMI DSD service with QCSI
  -------------------------------------------------------------------------*/
  rc = qmi_csi_register_with_options(dsd_get_service_object_v01(),
                        qmi_dsdi_svc_connect_cb,
                        qmi_dsdi_svc_disconnect_cb, 
                        qmi_dsdi_handle_client_req_cb, 
                        &qmi_dsd_state, &os_params, 
                        &options,
                        &qmi_dsd_state.service_handle);
  
  DS_MSG1(MSG_LEGACY_MED,"qmi_csi_register %d", rc);
  if(rc != QMI_CSI_NO_ERR)
  {
    ERR_FATAL("Unable to register DSD service! Error %d", rc, 0, 0);
    return;
  }

  //register with SAP
  src = qmi_sap_register(dsd_get_service_object_v01(), NULL, &qmi_dsd_state.sap_client_handle);
  if (src != QMI_SAP_NO_ERR)
  {
    ERR_FATAL("Unable to register DSD service with QSAP! Error %d", src, 0, 0);
    return;
  }

  /*-------------------------------------------------------------------------
    Register at power up for interested System Events.
  -------------------------------------------------------------------------*/
  qmi_dsdi_reg_sys_events();

  /* Initialize the queue which will hold all handles of clients */
  (void) q_init(&qmi_dsd_state.client_q);
  qmi_dsd_state.num_of_clients = 0;
} /* qmi_dsd_init */


static boolean qmi_dsdi_msg_hdlr_cb
(
  ds_sig_enum_type  sig,
  void              *svc_ptr
)
{
  qmi_dsdi_state_info_type *dsd_sp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_ASSERT((sig == DS_DSD_RDR_SIGNAL) && svc_ptr);

  if (!qmi_dsdi_inited)
  {
    qmi_dsdi_inited = TRUE;
    /* Call init again to register with QCSI now
     * Wait for all tasks to be inited */
    qmi_dsd_init();
    return TRUE;
  }

  dsd_sp = (qmi_dsdi_state_info_type*) svc_ptr;

  qmi_csi_handle_event(dsd_sp->service_handle, &os_params);
  return TRUE;
} /* qmi_dsdi_msg_hdlr_cb */

static qmi_csi_cb_error qmi_dsdi_svc_connect_cb
(
  qmi_client_handle          client_handle,
  void                      *service_handle,
  void                     **connection_handle
)
{
  qmi_dsdi_client_info_type    *clnt_info_ptr;
  qmi_dsdi_state_info_type     *dsd_svc_ptr;
  qmi_csi_cb_error              rc;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  DS_ASSERT(connection_handle && service_handle );

  dsd_svc_ptr = (qmi_dsdi_state_info_type *) service_handle;
 
  if ( NULL != q_linear_search( &dsd_svc_ptr->client_q,
                                dsdi_compare_qmi_client_handle,
                                client_handle ) )
  {
    DS_MSG1(MSG_LEGACY_ERROR,"dsdi_svc_connect_cb: DSD client 0x%p present",
                    client_handle);
    rc = QMI_CSI_CB_CONN_REFUSED;
    goto bail;
  }

  if (DSDI_MAX_CLIDS == dsd_svc_ptr->num_of_clients)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"dsdi_svc_connect_cb: Maximum number of clients already allocated %d",
                    DSDI_MAX_CLIDS);
    rc = QMI_CSI_CB_CONN_REFUSED;
    goto bail;
  }

  DS_SYSTEM_HEAP_MEM_ALLOC(clnt_info_ptr, sizeof(qmi_dsdi_client_info_type),
                           qmi_dsdi_client_info_type*);
  if(!clnt_info_ptr)
  {
    rc = QMI_CSI_CB_CONN_REFUSED;
    goto bail;
  }

  memset(clnt_info_ptr, 0, sizeof(qmi_dsdi_client_info_type));
  (void) q_link ( clnt_info_ptr, &clnt_info_ptr->link );
  clnt_info_ptr->clnt = client_handle;
  /* Add it to the queue of clients */
  q_put( &(dsd_svc_ptr->client_q), &(clnt_info_ptr->link) );
  dsd_svc_ptr->num_of_clients++;

  *connection_handle = clnt_info_ptr;
  DS_MSG1(MSG_LEGACY_MED,"dsdi_svc_connect_cb: Alloc client 0x%p", clnt_info_ptr);
  rc = QMI_CSI_CB_NO_ERR;
bail:
  return rc;
} /* qmi_dsdi_svc_connect_cb() */

static void qmi_dsdi_svc_disconnect_cb
(
  void                      *connection_handle,
  void                      *service_handle
)
{
  qmi_dsdi_state_info_type    *dsd_svc_ptr;
  qmi_dsdi_client_info_type    *clnt_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ASSERT(connection_handle && service_handle);
  dsd_svc_ptr = (qmi_dsdi_state_info_type *)service_handle;

  DS_MSG1(MSG_LEGACY_MED,"dsdi_svc_disconnect_cb: Releasing client 0x%p",
                  connection_handle);

  clnt_info_ptr = q_linear_search( &dsd_svc_ptr->client_q,
                                   dsdi_compare_connection_info_handle,
                                   connection_handle );
  if (NULL == clnt_info_ptr)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"dsdi_svc_connect_cb: DSD client 0x%p not found",
                    connection_handle);
    return;

  }

  if ( TRUE != q_delete_ext( &dsd_svc_ptr->client_q,
                             &clnt_info_ptr->link ) )
  {
    DS_MSG1(MSG_LEGACY_ERROR,"dsdi_svc_connect_cb: DSD client 0x%p could not be deleted from queue",
                    clnt_info_ptr);
    return;
  }
  DS_SYSTEM_HEAP_MEM_FREE(clnt_info_ptr);
  dsd_svc_ptr->num_of_clients--;
} /* qmi_dsdi_svc_disconnect_cb() */


static qmi_csi_cb_error qmi_dsdi_handle_client_req_cb
(
  void                     *connection_handle,
  qmi_req_handle           req_handle,
  unsigned int             msg_id,
  void                     *req_c_struct,
  unsigned int             req_c_struct_len,
  void                     *service_handle
)
{
  qmi_csi_cb_error               rc;
  qmi_dsdi_client_info_type     *clnt_info_ptr;
  qmi_dsdi_state_info_type      *dsd_svc_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ASSERT(connection_handle && service_handle);

  rc = QMI_CSI_CB_NO_ERR;
  dsd_svc_ptr = (qmi_dsdi_state_info_type*) service_handle;

  clnt_info_ptr = q_linear_search( &dsd_svc_ptr->client_q,
                                   dsdi_compare_connection_info_handle,
                                   connection_handle );
  if (NULL == clnt_info_ptr)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"dsdi_handle_req_cb: Invalid clnt 0x%p",
                    connection_handle);
    rc = QMI_CSI_CB_INTERNAL_ERR;
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
      rc = QMI_CSI_CB_UNSUPPORTED_ERR;
      DS_MSG1(MSG_LEGACY_ERROR,"dsdi_handle_req_cb: NULL message ID handler: %d",
                      msg_id);
    }
  }
  else
  {
    rc = QMI_CSI_CB_UNSUPPORTED_ERR;
    DS_MSG1(MSG_LEGACY_ERROR,"dsdi_handle_req_cb: Invalid message ID: %d", msg_id);
  }

  return rc;
} /* qmi_dsdi_handle_client_req_cb() */


/*===========================================================================
  FUNCTION QMI_DSDI_SYSTEM_STATUS_CHANGE_REQ()

  DESCRIPTION
    Registers system status event for interested control point.

  PARAMETERS
    clnt_info          : Client info pointer
    req_handle         : Request handle
    msg_id             : Message Id
    req_c_struct       : Incoming request 
    req_c_struct_len   : incoming request length
    service_handle     : Service hadle

  RETURN VALUE
    QMI_RESULT_SUCCESS in case of suscess and QMI_RESULT_FAILURE in case
    of failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_dsdi_system_status_change_req
(
  qmi_dsdi_client_info_type       * clnt_info,
  qmi_req_handle                    req_handle,
  int                               msg_id,
  void                            * req_c_struct,
  int                               req_c_struct_len,
  void                            * service_handle
)
{
  qmi_dsdi_state_info_type                * dsd_sp;
  dsd_system_status_change_req_msg_v01    * req_ptr;
  dsd_system_status_change_resp_msg_v01    resp_msg;
  qmi_error_type_v01     errval;
  qmi_result_type_v01    result;
  qmi_dsdi_client_info_type  * clnt_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_MSG1(MSG_LEGACY_MED,"qmi_dsdi_system_status_change_req: msg_id %d ", msg_id);

  DS_ASSERT(clnt_info && service_handle );
  
  errval = QMI_ERR_NONE_V01;
  memset(&resp_msg, 0, sizeof(dsd_system_status_change_resp_msg_v01));  
  req_ptr = (dsd_system_status_change_req_msg_v01 *)req_c_struct;
  
  dsd_sp = (qmi_dsdi_state_info_type *) service_handle;
  clnt_info_ptr = q_linear_search( &dsd_sp->client_q,
                                   dsdi_compare_connection_info_handle,
                                   clnt_info );
                                   
  if(clnt_info_ptr == NULL || req_ptr == NULL)
  {
    DS_MSG2(MSG_LEGACY_ERROR,"No client or req found ,clnt_info_ptr=0x%x req_ptr=0x%x",
                         clnt_info_ptr,req_ptr);
    errval = QMI_ERR_INTERNAL_V01;
    goto send_result;
  }               

  if ( req_ptr->report_data_system_status_changes_valid == TRUE)
  {
    if(req_ptr->report_data_system_status_changes)
    {
      clnt_info_ptr->report_status.report_system_status = TRUE;
    }
    else
    {
      clnt_info_ptr->report_status.report_system_status = FALSE;
    }
  }
  else /*Legacy behavior to send status change by default*/
  {
    clnt_info_ptr->report_status.report_system_status = TRUE;
  }

  if(req_ptr->limit_so_mask_change_ind_valid)
  {
    if(req_ptr->limit_so_mask_change_ind)
    {
      clnt_info_ptr->report_status.limit_so_mask_change_report_ind = TRUE;
    }
    else
    {
      clnt_info_ptr->report_status.limit_so_mask_change_report_ind = FALSE;
    }
  }
  else
  {
    /*Default value of limit_so_mask_change_report_ind is false*/
    clnt_info_ptr->report_status.limit_so_mask_change_report_ind = FALSE;
  }

send_result:
  result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01
                                   : QMI_RESULT_FAILURE_V01);
  resp_msg.resp.result = result;
  resp_msg.resp.error  = errval;  

  DS_MSG2(MSG_LEGACY_MED,"qmi_dsdi_system_status_change_req result %d errval %d",
                   result, errval);     

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(dsd_system_status_change_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;

} /* qmi_dsdi_system_status_change_req() */

/*===========================================================================
  FUNCTION QMI_DSDI_SYSTEM_STATUS_IND()

  DESCRIPTION
    This function sends system status indication to registered control point.

  PARAMETERS
    clid             : clientID
    system_status    : System Status

  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_dsdi_system_status_ind
(
  void                                  * client,
  dsd_system_status_ind_msg_v01         * system_status
)
{
  qmi_dsdi_client_info_type               * client_info;
  qmi_csi_error                             rc;
  uint32                                       cnt_network;
  uint32                                       cnt_apn;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_MSG0(MSG_LEGACY_MED,"qmi_dsdi_system_status_ind ");
  
  rc = QMI_CSI_NO_ERR;
  client_info = (qmi_dsdi_client_info_type *) client;
  if (client_info == NULL)
  {
    DS_ERR_FATAL("qmi_dsdi_system_status_ind : Null client_info");
    return;
  }
  
  DS_MSG4(MSG_LEGACY_MED,"qmi_dsdi_system_status_ind ,"
                  "num of rats %d num of apns %d"
                  "avail_sys_valid %d apn_avail_sys_info_valid %d",
                  system_status->avail_sys_len,
                  system_status->apn_avail_sys_info_len,
                  system_status->avail_sys_valid,
                  system_status->apn_avail_sys_info_valid);
                  
  for (cnt_network=0; cnt_network < system_status->avail_sys_len; 
       cnt_network++)
  {
    DS_MSG3(MSG_LEGACY_MED,"qmi_dsdi_system_status_ind "
                    "technology %d rat value %d so mask 0x%x",
                    system_status->avail_sys[cnt_network].technology,
                    system_status->avail_sys[cnt_network].rat_value,
                    system_status->avail_sys[cnt_network].so_mask);
  }
  
  for (cnt_apn=0; cnt_apn < system_status->apn_avail_sys_info_len; 
       cnt_apn++)
  { 
    DS_MSG2(MSG_LEGACY_MED,"qmi_dsdi_system_status_ind , apn name %s apn avail sys len %d ",
                    system_status->apn_avail_sys_info[cnt_apn].apn_name,
                    system_status->apn_avail_sys_info[cnt_apn].apn_avail_sys_len);

    for (cnt_network=0; cnt_network < system_status-> \
         apn_avail_sys_info[cnt_apn].apn_avail_sys_len; cnt_network++)
    {
      DS_MSG4(MSG_LEGACY_MED,"qmi_dsdi_system_status_ind apn sys info[%d]"
                      "technology %d rat value %d so mask 0x%x",
                      cnt_network,
                      system_status->apn_avail_sys_info[cnt_apn].apn_avail_sys[cnt_network].technology,
                      system_status->apn_avail_sys_info[cnt_apn].apn_avail_sys[cnt_network].rat_value,
                      system_status->apn_avail_sys_info[cnt_apn].apn_avail_sys[cnt_network].so_mask);
    }    
  }
            
  rc = qmi_csi_send_ind(client_info->clnt, 
                        QMI_DSD_SYSTEM_STATUS_IND_V01,
                        system_status, 
                        sizeof(dsd_system_status_ind_msg_v01));
  
  if ( rc != QMI_CSI_NO_ERR )
  {
    DS_MSG1(MSG_LEGACY_ERROR,"qmi_dsdi_system_status_ind error %d", rc);
  }

} /* qmi_dsdi_system_status_ind() */


/*===========================================================================
  FUNCTION QMI_DSDI_GET_SYSTEM_STATUS()

  DESCRIPTION
    Retrieves current system status and returns it to requesting control 
    point

  PARAMETERS
    clnt_info          : Client info pointer
    req_handle         : Request handle
    msg_id             : Message Id
    req_c_struct       : Incoming request 
    req_c_struct_len   : incoming request length
    service_handle     : Service hadle

  RETURN VALUE
    QMI_RESULT_SUCCESS in case of suscess and QMI_RESULT_FAILURE in case
    of failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_dsdi_get_system_status
( 
  qmi_dsdi_client_info_type   * clnt_info,
  qmi_req_handle                req_handle,
  int                           msg_id,
  void                        * req_c_struct,
  int                           req_c_struct_len,
  void                        * service_handle
)
{
  qmi_error_type_v01            errval;
  qmi_result_type_v01           result;
  ds_sys_system_status_type     *conf_system_status = NULL;
  int16                         ps_errno = 0;
  uint32                           cnt_network;
  dsd_get_system_status_resp_msg_v01 *resp_msg;
  uint32                           cnt_apn;
  uint8                            recommended_pref;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_MSG1(MSG_LEGACY_MED,"qmi_dsdi_get_system_status : msg_id %d ", msg_id);

  DS_ASSERT(clnt_info && service_handle);

  DS_SYSTEM_HEAP_MEM_ALLOC(resp_msg,
                          sizeof(dsd_get_system_status_resp_msg_v01),
                          dsd_get_system_status_resp_msg_v01*);
   if (NULL == resp_msg)
   {
     return QMI_CSI_CB_NO_MEM;
   }
  
  memset(resp_msg, 0, sizeof(dsd_get_system_status_resp_msg_v01));
  
  DS_SYSTEM_HEAP_MEM_ALLOC(conf_system_status,
                           sizeof(ds_sys_system_status_type),
                           ds_sys_system_status_type*);
  
   if (NULL == conf_system_status)
   {
      errval = QMI_ERR_NO_MEMORY_V01;
      goto send_result;
   }

  memset(conf_system_status, 0, sizeof(ds_sys_system_status_type));
  
  errval = QMI_ERR_NONE_V01;

  /*-------------------------------------------------------------------------
    Retrieve data system status by calling into PS Sys Layer
  -------------------------------------------------------------------------*/
  /*To do cache the system status value and call the api. Todo */
  
  //ds_sys_get_system_status(conf_system_status);
   ds_sys_ioctl(DS_SYS_TECH_ALL, DS_SYS_IOCTL_SYS_STATUS,
                conf_system_status,
                &ps_errno);
  
  
  DS_MSG2(MSG_LEGACY_MED,"qmi_dsdi_get_system_status ,"
                  "num of rats %d num of apns %d",
                  conf_system_status->num_avail_sys,
                  conf_system_status->num_avail_apns);
  
  resp_msg->avail_sys_len =
    (uint32)conf_system_status->num_avail_sys;  

  if(resp_msg->avail_sys_len > 0)
  {
    resp_msg->avail_sys_valid = TRUE; 
    resp_msg->global_pref_sys_valid = TRUE;
  }
  else
  {
    resp_msg->avail_sys_valid = FALSE; 
    resp_msg->global_pref_sys_valid = FALSE;
  }  
  
  recommended_pref = 
    conf_system_status->recommended_pref_system_index;
  if (recommended_pref >
        conf_system_status->num_avail_sys)
  {
    DS_MSG2(MSG_LEGACY_ERROR,"Recommended Index is out of bounds %d, num sys %d", 
                    recommended_pref, conf_system_status->num_avail_sys);
    errval = QMI_ERR_INTERNAL_V01;
    goto send_result;
  }  
    
  resp_msg->apn_avail_sys_info_len = 
    (uint32)conf_system_status->num_avail_apns;  
  resp_msg->apn_pref_sys_len = 
    (uint32)conf_system_status->num_avail_apns; 


  if(resp_msg->apn_avail_sys_info_len > 0)
  {
    resp_msg->apn_avail_sys_info_valid = TRUE; 
    resp_msg->apn_pref_sys_valid = TRUE;
  }
  else
  {
    resp_msg->apn_avail_sys_info_valid = FALSE; 
    resp_msg->apn_pref_sys_valid = FALSE;
  } 
                  
  for (cnt_network=0; cnt_network < conf_system_status->num_avail_sys; 
       cnt_network++)
  {
    resp_msg->avail_sys[cnt_network].technology = (dsd_sys_network_enum_v01)
      conf_system_status->avail_sys[cnt_network].technology;
    resp_msg->avail_sys[cnt_network].rat_value = (dsd_sys_rat_ex_enum_v01)
      conf_system_status->avail_sys[cnt_network].rat_value;
    resp_msg->avail_sys[cnt_network].so_mask =
      conf_system_status->avail_sys[cnt_network].so_mask;

    DS_MSG3(MSG_LEGACY_MED,"qmi_dsdi_get_system_status "
                    "technology %d rat value %d so mask 0x%x",
                    resp_msg->avail_sys[cnt_network].technology,
                    resp_msg->avail_sys[cnt_network].rat_value,
                    resp_msg->avail_sys[cnt_network].so_mask);
  }
  
  resp_msg->global_pref_sys.curr_pref_sys.technology = (dsd_sys_network_enum_v01)
    conf_system_status->avail_sys[0].technology;
  resp_msg->global_pref_sys.curr_pref_sys.rat_value = (dsd_sys_rat_ex_enum_v01)
    conf_system_status->avail_sys[0].rat_value;
  resp_msg->global_pref_sys.curr_pref_sys.so_mask =
    conf_system_status->avail_sys[0].so_mask;

   DS_MSG3(MSG_LEGACY_MED,"qmi_dsdi_get_system_status global current"
                    "pref technology %d rat value %d so mask 0x%x",
                    resp_msg->global_pref_sys.curr_pref_sys.technology,
                    resp_msg->global_pref_sys.curr_pref_sys.rat_value,
                    resp_msg->global_pref_sys.curr_pref_sys.so_mask);

  resp_msg->global_pref_sys.recommended_pref_sys.technology = 
    (dsd_sys_network_enum_v01)
    conf_system_status->avail_sys[recommended_pref].technology;
  resp_msg->global_pref_sys.recommended_pref_sys.rat_value = (dsd_sys_rat_ex_enum_v01)
    conf_system_status->avail_sys[recommended_pref].rat_value;
  resp_msg->global_pref_sys.recommended_pref_sys.so_mask =
    conf_system_status->avail_sys[recommended_pref].so_mask;

   DS_MSG3(MSG_LEGACY_MED,"qmi_dsdi_get_system_status global recommended"
                    "pref technology %d rat value %d so mask 0x%x",
                    resp_msg->global_pref_sys.recommended_pref_sys.technology,
                    resp_msg->global_pref_sys.recommended_pref_sys.rat_value,
                    resp_msg->global_pref_sys.recommended_pref_sys.so_mask);
  
  for (cnt_apn=0; cnt_apn < conf_system_status->num_avail_apns; 
       cnt_apn++)
  { 
    memscpy(&(resp_msg->apn_avail_sys_info[cnt_apn].apn_name),
            sizeof(conf_system_status->apn_pref_sys_info[cnt_apn].apn_name),
           &(conf_system_status->apn_pref_sys_info[cnt_apn].apn_name),
           sizeof(conf_system_status->apn_pref_sys_info[cnt_apn].apn_name));        

    resp_msg->apn_avail_sys_info[cnt_apn].apn_avail_sys_len =
      (uint32)conf_system_status->apn_pref_sys_info[cnt_apn].num_avail_sys;

    DS_MSG2(MSG_LEGACY_MED,"qmi_dsdi_get_system_status , apn name %s apn avail sys len %d",
                    conf_system_status->apn_pref_sys_info[cnt_apn].apn_name,
                    conf_system_status->apn_pref_sys_info[cnt_apn].num_avail_sys);

    for (cnt_network=0; cnt_network < conf_system_status->apn_pref_sys_info[cnt_apn].num_avail_sys; cnt_network++)
    {
      resp_msg->apn_avail_sys_info[cnt_apn].apn_avail_sys[cnt_network].technology = 
        (dsd_sys_network_enum_v01) 
        conf_system_status->apn_pref_sys_info[cnt_apn].avail_sys[cnt_network].technology;

      resp_msg->apn_avail_sys_info[cnt_apn].apn_avail_sys[cnt_network].rat_value = 
        (dsd_sys_rat_ex_enum_v01)
        conf_system_status->apn_pref_sys_info[cnt_apn].avail_sys[cnt_network].rat_value;

      resp_msg->apn_avail_sys_info[cnt_apn].apn_avail_sys[cnt_network].so_mask =
        conf_system_status->apn_pref_sys_info[cnt_apn].avail_sys[cnt_network].so_mask;          

      DS_MSG4(MSG_LEGACY_MED,"qmi_dsdi_get_system_status apn sys info[%d]"
                      "technology %d rat value %d so mask 0x%x",
                      cnt_network,
                      resp_msg->apn_avail_sys_info[cnt_apn].apn_avail_sys[cnt_network].technology,
                      resp_msg->apn_avail_sys_info[cnt_apn].apn_avail_sys[cnt_network].rat_value,
                      resp_msg->apn_avail_sys_info[cnt_apn].apn_avail_sys[cnt_network].so_mask);
    }    
  }

  for (cnt_apn=0; cnt_apn < conf_system_status->num_avail_apns; 
       cnt_apn++)
  { 
    memscpy(&(resp_msg->apn_pref_sys[cnt_apn].apn_name),
            sizeof(conf_system_status->apn_pref_sys_info[cnt_apn].apn_name),
           &(conf_system_status->apn_pref_sys_info[cnt_apn].apn_name),
           sizeof(conf_system_status->apn_pref_sys_info[cnt_apn].apn_name));        

    //Current preferred system topmost element
    resp_msg->apn_pref_sys[cnt_apn].curr_pref_sys.technology = 
        (dsd_sys_network_enum_v01) 
        conf_system_status->apn_pref_sys_info[cnt_apn].avail_sys[0].technology;

      resp_msg->apn_pref_sys[cnt_apn].curr_pref_sys.rat_value = 
        (dsd_sys_rat_ex_enum_v01)
        conf_system_status->apn_pref_sys_info[cnt_apn].avail_sys[0].rat_value;

      resp_msg->apn_pref_sys[cnt_apn].curr_pref_sys.so_mask =
        conf_system_status->apn_pref_sys_info[cnt_apn].avail_sys[0].so_mask;  
      
      recommended_pref = 
        conf_system_status->apn_pref_sys_info[cnt_apn].recommended_pref_system_index;

    if (recommended_pref >
          conf_system_status->apn_pref_sys_info[cnt_apn].num_avail_sys)
    {
      DS_MSG2(MSG_LEGACY_ERROR,"Recommended Index is out of bounds %d, num sys %d", 
                      recommended_pref, 
                      conf_system_status->apn_pref_sys_info[cnt_apn].num_avail_sys);
      errval = QMI_ERR_INTERNAL_V01;
      goto send_result;
    }

    //Recommended preferred system.
     resp_msg->apn_pref_sys[cnt_apn].recommended_pref_sys.technology = 
        (dsd_sys_network_enum_v01) 
        conf_system_status->apn_pref_sys_info[cnt_apn].\
       avail_sys[recommended_pref].technology;

      resp_msg->apn_pref_sys[cnt_apn].recommended_pref_sys.rat_value = 
        (dsd_sys_rat_ex_enum_v01)
        conf_system_status->apn_pref_sys_info[cnt_apn].\
        avail_sys[recommended_pref].rat_value;

      resp_msg->apn_pref_sys[cnt_apn].recommended_pref_sys.so_mask =
        conf_system_status->apn_pref_sys_info[cnt_apn].\
        avail_sys[recommended_pref].so_mask;  
  }



send_result:
  result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01
                                   : QMI_RESULT_FAILURE_V01);

  resp_msg->resp.result = result;
  resp_msg->resp.error  = errval; 

  DS_MSG2(MSG_LEGACY_MED,"qmi_dsdi_get_system_status result %d errval %d ",
                  result, errval);
                 
  qmi_csi_send_resp(req_handle, 
                    msg_id, 
                    resp_msg, 
                    sizeof(dsd_get_system_status_resp_msg_v01));
                    
  if(conf_system_status != NULL)
  {
    DS_SYSTEM_HEAP_MEM_FREE(conf_system_status);
  }

  if(resp_msg != NULL)
  {
    DS_SYSTEM_HEAP_MEM_FREE(resp_msg);
  }

  return QMI_CSI_CB_NO_ERR;
} /* qmi_dsdi_get_system_status() */

/*===========================================================================
  FUNCTION dsdi_system_status_change_cb()

  DESCRIPTION
    This the callback that we have registered with PS Sys layer for
    system status change event. 

  PARAMETERS
    tech_type      : technology for which system status has changed
    event_name     : Event name
    subscription_id: Subscription id
    event_info_ptr : System status event info
    user_data_ptr  : Optional user data to be sent.

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void dsdi_system_status_change_cb
(
  ds_sys_tech_enum_type                          tech_type,
  uint64                                         event_name,
  void                                         * event_info_ptr,  
  void                                         * user_data_ptr
)
{
  qmi_dsdi_cmd_buf_type                 *cmd_payload = NULL;
  dsd_system_status_ind_msg_v01         *dsd_system_status;
  ds_sys_system_status_type             *sys_system_status;
  uint32                               cnt_network;
  uint32                               cnt_apn;
  uint8                             recommend_index;
  ds_command_type                  *cmd_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ASSERT((int)event_info_ptr);

  sys_system_status = (ds_sys_system_status_type *)event_info_ptr;

  /*-----------------------------------------------------------------------
   Get a DS command buffer
   -----------------------------------------------------------------------*/
   cmd_ptr = ds_allocate_cmd_buf(sizeof(qmi_dsdi_cmd_buf_type));
   if ((cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL))
   {
     DS_ERR_FATAL("Out of memory");
     return;
    }
    cmd_ptr->hdr.cmd_id = DS_QMI_DSD_SVC_CMD;
    cmd_payload = (qmi_dsdi_cmd_buf_type*)cmd_ptr->cmd_payload_ptr;
	
    dsd_system_status = &cmd_payload->system_status;
  
  DS_MSG5(MSG_LEGACY_MED,"dsdi_system_status_change_cb tech_type %d event_name %d"
                           "event_info_ptr 0x%x"
                  "num of rats %d num of apns %d",
                  tech_type, event_name, event_info_ptr,
                  sys_system_status->num_avail_sys,
                  sys_system_status->num_avail_apns);
  
  dsd_system_status->avail_sys_len =
    (uint32)sys_system_status->num_avail_sys;  

  if(dsd_system_status->avail_sys_len > 0)
  {
    dsd_system_status->avail_sys_valid = TRUE; 
    dsd_system_status->global_pref_sys_valid = TRUE;
  }
  else
  {
    dsd_system_status->avail_sys_valid = FALSE; 
    dsd_system_status->global_pref_sys_valid = FALSE;
  }  
    
  dsd_system_status->apn_avail_sys_info_len = 
    (uint32)sys_system_status->num_avail_apns;  

  if(dsd_system_status->apn_avail_sys_info_len > 0)
  {
    dsd_system_status->apn_avail_sys_info_valid = TRUE; 
  }
  else
  {
    dsd_system_status->apn_avail_sys_info_valid = FALSE; 
  } 
                  

  dsd_system_status->apn_pref_sys_len = 
    (uint32)sys_system_status->num_avail_apns; 
 
  if(dsd_system_status->apn_pref_sys_len > 0)
  {
    dsd_system_status->apn_pref_sys_valid = TRUE; 
  }
  else
  {
    dsd_system_status->apn_pref_sys_valid = FALSE; 
  } 
                  
  recommend_index = sys_system_status->recommended_pref_system_index;

  if (recommend_index > sys_system_status->num_avail_sys)
  {
    DS_MSG2(MSG_LEGACY_MED,"Recommended Index is out of bounds %d, num sys %d", 
                    recommend_index,
                    sys_system_status->num_avail_sys);
    goto bail;
  }
                  
  for (cnt_network=0; cnt_network < sys_system_status->num_avail_sys; 
       cnt_network++)
  {
    DS_MSG3(MSG_LEGACY_MED,"dsdi_system_status_change_cb "
                    "technology %d rat value %d so mask 0x%x",
                    sys_system_status->avail_sys[cnt_network].technology,
                    sys_system_status->avail_sys[cnt_network].rat_value,
                    sys_system_status->avail_sys[cnt_network].so_mask);
                  
    dsd_system_status->avail_sys[cnt_network].technology = (dsd_sys_network_enum_v01)
      sys_system_status->avail_sys[cnt_network].technology;

    dsd_system_status->avail_sys[cnt_network].rat_value = (dsd_sys_rat_ex_enum_v01)
      sys_system_status->avail_sys[cnt_network].rat_value;

    dsd_system_status->avail_sys[cnt_network].so_mask =
      sys_system_status->avail_sys[cnt_network].so_mask;
  }
  
  dsd_system_status->global_pref_sys.curr_pref_sys.technology = 
    (dsd_sys_network_enum_v01)
      sys_system_status->avail_sys[0].technology;
  dsd_system_status->global_pref_sys.curr_pref_sys.rat_value = 
    (dsd_sys_rat_ex_enum_v01)
      sys_system_status->avail_sys[0].rat_value;
  dsd_system_status->global_pref_sys.curr_pref_sys.so_mask = 
      sys_system_status->avail_sys[0].so_mask;


  dsd_system_status->global_pref_sys.recommended_pref_sys.technology = 
    (dsd_sys_network_enum_v01)
      sys_system_status->avail_sys[recommend_index].technology;
  dsd_system_status->global_pref_sys.recommended_pref_sys.rat_value = 
    (dsd_sys_rat_ex_enum_v01)
      sys_system_status->avail_sys[recommend_index].rat_value;
  dsd_system_status->global_pref_sys.recommended_pref_sys.so_mask = 
      sys_system_status->avail_sys[recommend_index].so_mask;
  
  for (cnt_apn=0; cnt_apn < sys_system_status->num_avail_apns; 
       cnt_apn++)
  { 
    DS_MSG1(MSG_LEGACY_MED,"dsdi_system_status_change_cb , apn name %s ",
                    sys_system_status->apn_pref_sys_info[cnt_apn].apn_name);
      
    memscpy(&dsd_system_status->apn_avail_sys_info[cnt_apn].apn_name,
            sizeof(dsd_system_status->apn_avail_sys_info[cnt_apn].apn_name),
           &sys_system_status->apn_pref_sys_info[cnt_apn].apn_name,
           strlen(sys_system_status->apn_pref_sys_info[cnt_apn].apn_name));           

    dsd_system_status->apn_avail_sys_info[cnt_apn].apn_avail_sys_len =
      sys_system_status->apn_pref_sys_info[cnt_apn].num_avail_sys;

    DS_MSG1(MSG_LEGACY_MED,"dsdi_system_status_change_cb , apn avail sys len %d ",
                    sys_system_status->apn_pref_sys_info[cnt_apn].num_avail_sys);

    for (cnt_network=0; cnt_network < sys_system_status->apn_pref_sys_info[cnt_apn].num_avail_sys; cnt_network++)
    {
      DS_MSG4(MSG_LEGACY_MED,"dsdi_system_status_change_cb apn sys info[%d]"
                      "technology %d rat value %d so mask 0x%x",
                      cnt_network,
                      sys_system_status->apn_pref_sys_info[cnt_apn].avail_sys[cnt_network].technology,
                      sys_system_status->apn_pref_sys_info[cnt_apn].avail_sys[cnt_network].rat_value,
                      sys_system_status->apn_pref_sys_info[cnt_apn].avail_sys[cnt_network].so_mask);
                    
      dsd_system_status->apn_avail_sys_info[cnt_apn].apn_avail_sys[cnt_network].technology =
        (dsd_sys_network_enum_v01)
        sys_system_status->apn_pref_sys_info[cnt_apn].avail_sys[cnt_network].technology;

      dsd_system_status->apn_avail_sys_info[cnt_apn].apn_avail_sys[cnt_network].rat_value =
        (dsd_sys_rat_ex_enum_v01)
        sys_system_status->apn_pref_sys_info[cnt_apn].avail_sys[cnt_network].rat_value;

      dsd_system_status->apn_avail_sys_info[cnt_apn].apn_avail_sys[cnt_network].so_mask =
        sys_system_status->apn_pref_sys_info[cnt_apn].avail_sys[cnt_network].so_mask;          
    }    
  }

  for (cnt_apn=0; cnt_apn < sys_system_status->num_avail_apns; 
       cnt_apn++)
  { 
    DS_MSG1(MSG_LEGACY_MED,"dsdi_system_status_change_cb , apn name %s ",
                    sys_system_status->apn_pref_sys_info[cnt_apn].apn_name);
      
    memscpy(&dsd_system_status->apn_pref_sys[cnt_apn].apn_name,
            sizeof(dsd_system_status->apn_pref_sys[cnt_apn].apn_name),
           &sys_system_status->apn_pref_sys_info[cnt_apn].apn_name,
           strlen(sys_system_status->apn_pref_sys_info[cnt_apn].apn_name));  

    //Current
    dsd_system_status->apn_pref_sys[cnt_apn].curr_pref_sys.technology =
      (dsd_sys_network_enum_v01)
      sys_system_status->apn_pref_sys_info[cnt_apn].avail_sys[0].technology;

    dsd_system_status->apn_pref_sys[cnt_apn].curr_pref_sys.rat_value =
      (dsd_sys_rat_ex_enum_v01)
      sys_system_status->apn_pref_sys_info[cnt_apn].avail_sys[0].rat_value;

    dsd_system_status->apn_pref_sys[cnt_apn].curr_pref_sys.so_mask =
      sys_system_status->apn_pref_sys_info[cnt_apn].avail_sys[0].so_mask; 


    
    recommend_index = 
      sys_system_status->apn_pref_sys_info[cnt_apn].recommended_pref_system_index;

    if (recommend_index > 
          sys_system_status->apn_pref_sys_info[cnt_apn].num_avail_sys)
    {
      DS_MSG2(MSG_LEGACY_MED,"Recommended Index is out of bounds %d, num_sys %d", 
                      recommend_index,
                      sys_system_status->apn_pref_sys_info[cnt_apn].num_avail_sys);
      goto bail;
    }

    dsd_system_status->apn_pref_sys[cnt_apn].recommended_pref_sys.technology =
        (dsd_sys_network_enum_v01)
        sys_system_status->apn_pref_sys_info[cnt_apn].avail_sys[recommend_index].technology;

       dsd_system_status->apn_pref_sys[cnt_apn].recommended_pref_sys.rat_value =
        (dsd_sys_rat_ex_enum_v01)
        sys_system_status->apn_pref_sys_info[cnt_apn].avail_sys[recommend_index].rat_value;

      dsd_system_status->apn_pref_sys[cnt_apn].recommended_pref_sys.so_mask =
        sys_system_status->apn_pref_sys_info[cnt_apn].avail_sys[recommend_index].so_mask; 
  }

  cmd_payload->cmd_id = QMI_CMD_DSD_SYSTEM_STATUS_CHANGE_EVT;
  ds_put_cmd(cmd_ptr);
  return;

bail:
  if (cmd_ptr)
  {
    DS_SYSTEM_HEAP_MEM_FREE(cmd_ptr);
  }
} /* dsdi_system_status_change_cb() */


/*===========================================================================
FUNCTION  QMI_WDSI_GENERATE_SYSTEM_STATUS_IND

DESCRIPTION
  This callback is called to notify clients about data system status changes.

PARAMETERS
  system_status - System status info like preferred network, rat mask,
                  so mask.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
static void qmi_dsdi_generate_system_status_ind
(
  dsd_system_status_ind_msg_v01 * system_status
)
{
  qmi_dsdi_client_info_type                   * cl_sp;
  uint8                                         cnt_sys;
  uint8                                         cnt_apn;
  q_type                                      * client_q_ptr = NULL;
  void                                        * client_buf_ptr;
  void                                        * next_client_buf_ptr;
  qmi_dsdi_state_info_type                    * dsd_sp;
  boolean                                       is_tech_rat_same = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  //memset(&system_status_log, 0, sizeof(system_status_log));
  
  if(system_status == NULL)
  {
    DS_ASSERT(0);
  }
  DS_MSG4(MSG_LEGACY_MED,"qmi_dsdi_generate_system_status_ind ,"
                  "num of rats %d num of apns %d"
                  "avail_sys_valid %d apn_avail_sys_info_valid %d",
                  system_status->avail_sys_len,
                  system_status->apn_avail_sys_info_len,
                  system_status->avail_sys_valid,
                  system_status->apn_avail_sys_info_valid);
                  
  
    /* check if technology or rat value has changed*/ 
   if((qmi_dsdi_last_avail_sys_pref_network.technology != 
                 (ds_sys_network_enum_type)system_status->avail_sys[0].technology)||
       (qmi_dsdi_last_avail_sys_pref_network.rat_value != 
                  (ds_sys_rat_ex_enum_type)system_status->avail_sys[0].rat_value ))
    {
      is_tech_rat_same = FALSE;
    }
    else
    {
      DS_MSG2(MSG_LEGACY_MED,"SO mask changed from %x to %x",
      qmi_dsdi_last_avail_sys_pref_network.so_mask,
      system_status->avail_sys[0].so_mask);
    }

  /* Copy the last preferred network tuple into global variable */
  qmi_dsdi_last_avail_sys_pref_network.technology = 
             (ds_sys_network_enum_type)system_status->avail_sys[0].technology;
  qmi_dsdi_last_avail_sys_pref_network.rat_value  = 
              (ds_sys_rat_ex_enum_type)system_status->avail_sys[0].rat_value;
  qmi_dsdi_last_avail_sys_pref_network.so_mask    = 
                               (uint64)system_status->avail_sys[0].so_mask;

  for (cnt_apn=0; cnt_apn < system_status->apn_avail_sys_info_len; 
       cnt_apn++)
  { 
    DS_MSG2(MSG_LEGACY_MED,"qmi_dsdi_generate_system_status_ind , apn name %s apn avail sys len %d",
                    system_status->apn_avail_sys_info[cnt_apn].apn_name,
                    system_status->apn_avail_sys_info[cnt_apn].apn_avail_sys_len);

    for (cnt_sys=0; cnt_sys < system_status->apn_avail_sys_info[cnt_apn].apn_avail_sys_len; cnt_sys++)
    {
      DS_MSG4(MSG_LEGACY_MED,"qmi_dsdi_generate_system_status_ind apn sys info[%d]"
                      "technology %d rat value %d so mask 0x%x",
                      cnt_sys,
                      system_status->apn_avail_sys_info[cnt_apn].apn_avail_sys[cnt_sys].technology,
                      system_status->apn_avail_sys_info[cnt_apn].apn_avail_sys[cnt_sys].rat_value,
                      system_status->apn_avail_sys_info[cnt_apn].apn_avail_sys[cnt_sys].so_mask);
    }    
  }

  dsd_sp         = &qmi_dsd_state;
  client_q_ptr   = &dsd_sp->client_q;
  client_buf_ptr = q_check(client_q_ptr);
  
  while(client_buf_ptr != NULL)
  {
    cl_sp = (qmi_dsdi_client_info_type *) client_buf_ptr;
    
    if (TRUE == cl_sp->report_status.report_system_status)
    {    
      /* If limit_so_mask_change_report_ind is FALSE, send the indication, 
         If limit_so_mask_change_report_ind is TRUE, send the indication only
         when technology value or rat mask has changed */
      if((FALSE == cl_sp->report_status.limit_so_mask_change_report_ind) ||
        ((TRUE ==  cl_sp->report_status.limit_so_mask_change_report_ind) &&
        (FALSE == is_tech_rat_same)))
      {

        /* This flag is checked to send the diag event only once for all the clients */
	  /*
        if(diag_event_reported == FALSE)
        {
          /* Diag event for system status ex indication 
          system_status_log.pref_network = system_status->avail_sys[0].technology;
          system_status_log.rat_value    = system_status->avail_sys[0].rat_value;
          system_status_log.so_mask      = system_status->avail_sys[0].so_mask;
          system_status_log.subs_id      = subscription_id;
          /* Set the flag to true so that the event is not generated for all the clients 
          diag_event_reported            = TRUE;
          event_report_payload( EVENT_QMI_SYSTEM_STATUS_EX, 
                                sizeof(ps_sys_system_status_diag_event_logging_type),
                                (void*) &system_status_log);
        }*/
        qmi_dsdi_system_status_ind(cl_sp,
                                   system_status
                                  );
      }
    }
    
    next_client_buf_ptr = 
      q_next(client_q_ptr, &((q_generic_item_type *) client_buf_ptr)->link);
    client_buf_ptr = next_client_buf_ptr;
  }
  
} /* qmi_dsdi_generate_system_status_ind */

/*===========================================================================
  FUNCTION qmi_dsdi_process_cmd()

  DESCRIPTION
    This function processes a QMI DSD command

    It is called by the QMI command handler and will dispatch the
    associated command/event handler function.

  PARAMETERS
    cmd_ptr:  private data buffer containing the QMI DSD command
              information.

  RETURN VALUE
    None

  DEPENDENCIES
    QMI DSD must already have been initialized

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_dsdi_process_cmd
(
  ds_command_enum_type  cmd,
  void              *cmd_ptr
)
{
  qmi_dsdi_cmd_buf_type *cmd_buf_ptr = NULL;
/*-------------------------------------------------------------------------*/

  DS_ASSERT((int)cmd_ptr);
  cmd_buf_ptr = (qmi_dsdi_cmd_buf_type *)cmd_ptr;

  DS_MSG2(MSG_LEGACY_MED,"qmi_dsdi_process_cmd: cmdbuf's cmd_id:%d  cmd %d",
                  cmd_buf_ptr->cmd_id, cmd);

  switch(cmd_buf_ptr->cmd_id)
  {

    case QMI_CMD_DSD_SYSTEM_STATUS_CHANGE_EVT:
      qmi_dsdi_generate_system_status_ind
      (
        &(cmd_buf_ptr->system_status)
      );
      break;

    default:
      DS_ASSERT(0);
      
  }
  return;
} /* qmi_dsdi_process_cmd() */

/*===========================================================================
  FUNCTION QMI_DSDI_REG_SYS_EVENTS()

  DESCRIPTION
    Registers for PS system level events

  PARAMETERS
    None

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_dsdi_reg_sys_events
( 
  void
)
{
  uint64                   event_id   = DS_SYS_EVENT_COMMON_MIN_EX;
  int16                    result = 0;
  int16                    dss_errno = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
     result = ds_system_event_reg (DS_SYS_TECH_ALL,
                                    DS_SYS_EVENT_SYSTEM_STATUS,
                                    dsdi_system_status_change_cb,
                                    NULL,
                                    &dss_errno);
     if(result < 0)
     {
       event_id = DS_SYS_EVENT_SYSTEM_STATUS;
     }
    if(result < 0)
    {
      DS_MSG2(MSG_LEGACY_ERROR,"Couldn't reg for ds sys event_id = %d Err : (%d)",
                      event_id, dss_errno);
      DS_ASSERT(0);
      return;
    }
}/* qmi_dsdi_reg_sys_events */
