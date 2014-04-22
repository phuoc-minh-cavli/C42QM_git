/*===========================================================================

                         D S _ Q M I _ W D A . C

DESCRIPTION

  The Data Services QMI Wireless Data Administrative Service source file.

EXTERNALIZED FUNCTIONS

  qmi_wda_init()
    Initialize the QMI WDA service

Copyright (c) 2012-2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include <stdio.h>
#include <string.h>

#include "dsm.h"
#include "nv.h"
#include "msg.h"
#include "err.h"
#include "amssassert.h"
#include "queue.h"
#include "ds_qmi_defs.h"
#include "ds_qmi_task.h"
#include "ps_in.h"
#include "ds_system_heap.h"
#include "data_msg.h"

#include "qmi_idl_lib.h"
#include "qmi_idl_lib_internal.h"
#include "qmi_csi.h"
#include "qmi_csi_target_ext.h"
#include "common_v01.h"
#include "mqcsi_log.h"
#include "qmi_sap.h"

#include "qmi_svc_defs.h"
#include "qmi_svc_utils.h"
#include "qmi_framework.h"
#include "ds_qmi_wda.h"
#include "ds_rmnet_meta_sm.h"
#include "ds_rmnet_xport.h"

#include "ds_system_heap.h"
#include "modem_mem.h"
#include "qmi_si.h"
#include "qmi_idl_lib.h"
#include "wireless_data_administrative_service_v01.h"
#include "wireless_data_administrative_service_impl_v01.h"
#include "ps_utils.h"

/*---------------------------------------------------------------------------
  Service management
---------------------------------------------------------------------------*/
#define WDAI_MAX_CLIDS 5
#define WDAI_INVALID_INSTANCE    -1

/*---------------------------------------------------------------------------
  Major and Minor Version Nos for WDA
---------------------------------------------------------------------------*/
#define WDAI_BASE_VER_MAJOR    (1)
#define WDAI_BASE_VER_MINOR    (16)

#define WDAI_ADDENDUM_VER_MAJOR  (0)
#define WDAI_ADDENDUM_VER_MINOR  (0)

/*---------------------------------------------------------------------------
  Macro used in command handlers (common)
---------------------------------------------------------------------------*/

#define WDAI_QCSI_OS_SIGNAL ((rex_sigs_type)(1 << DS_WDA_RDR_SIGNAL))
#define EP_TYPE_BAM_DEMUX 83886080 

/*===========================================================================

                                DATA TYPES

===========================================================================*/

/*---------------------------------------------------------------------------
  QMI WDA Client state info
---------------------------------------------------------------------------*/
typedef struct qmi_wdai_client_state_s{
  q_link_type                       link;           /*< Data Structure required by the queue 
                                                             library to add this as part of a 
                                                             queue */
  qmi_client_handle                 clnt; 															 
}qmi_wdai_client_info_type;


/*---------------------------------------------------------------------------
  QMI WDA instance state definition &
  WDA client state definition
---------------------------------------------------------------------------*/

typedef struct
{
  uint16 cmd_id;
} qmi_wdai_cmd_buf_type;

/*---------------------------------------------------------------------------
  QMI AT Service state info
---------------------------------------------------------------------------*/
typedef struct
{
  q_type                 client_q; /*< Information about all the clients that
                                       have connected to the service. This is queue 
                                       of type qmi_dsdi_client_info_type */
  qmi_csi_service_handle service_handle;
  qmi_sap_client_handle  sap_client_handle;
  uint16                 num_of_clients;
} qmi_wdai_state_info_type;

static qmi_wdai_state_info_type       qmi_wda_state;
static qmi_csi_os_params              os_params;
static boolean                        qmi_wdai_inited = FALSE;

/*===========================================================================

                               INTERNAL DATA

===========================================================================*/

/*---------------------------------------------------------------------------
  QMI service command handlers
  forward declarations & cmd handler dispatch table definition
---------------------------------------------------------------------------*/


static qmi_csi_cb_error qmi_wdai_set_data_format
(
  qmi_wdai_client_info_type		 	 *clnt_info,
  qmi_req_handle			          req_handle,
  int 					              msg_id,
  void					             *req_c_struct,
  int 					              req_c_struct_len,
  void					             *service_handle  
);
static qmi_csi_cb_error qmi_wdai_get_data_format
(
  qmi_wdai_client_info_type	     	 *clnt_info,
  qmi_req_handle 		              req_handle,
  int					              msg_id,
  void					             *req_c_struct,
  int					              req_c_struct_len,
  void					             *service_handle 
);

static qmi_csi_cb_error qmi_wdai_set_qmap_settings
(
  qmi_wdai_client_info_type	     	 *clnt_info,
  qmi_req_handle 		              req_handle,
  int					              msg_id,
  void					             *req_c_struct,
  int					              req_c_struct_len,
  void					             *service_handle 
);

static qmi_csi_cb_error qmi_wdai_get_qmap_settings
(
   qmi_wdai_client_info_type	 	 *clnt_info,
   qmi_req_handle 		              req_handle,
   int					              msg_id,
   void					             *req_c_struct,
   int					              req_c_struct_len,
   void					             *service_handle 
);

static boolean qmi_wdai_msg_hdlr_cb
(
  ds_sig_enum_type  sig,
  void              *     svc_ptr
);

static qmi_csi_cb_error qmi_wdai_svc_connect_cb
(
  qmi_client_handle          client_handle,
  void                      *service_handle,
  void                     **connection_handle
);

static void qmi_wdai_svc_disconnect_cb
(
  void                      *connection_handle,
  void                      *service_handle
);

static qmi_csi_cb_error qmi_wdai_handle_client_req_cb
(
  void                      *connection_handle,
  qmi_req_handle             req_handle,
  unsigned int               msg_id,
  void                      *req_c_struct,
  unsigned int               req_c_struct_len,
  void                      *service_handle
);

static uint32 qmi_wdai_parse_data_format_req
(
  wda_set_data_format_req_msg_v01* req_msg,
  rmnet_data_format_type* data_format
);

static int wdai_compare_qmi_client_handle
( 
  void *item_ptr,
  void *compare_val 
)
{
  qmi_wdai_client_info_type *clnt_info = (qmi_wdai_client_info_type*)item_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ( (clnt_info->clnt == 
                          (qmi_client_handle)compare_val) ? 1 : 0 );
}

static int wdai_compare_connection_info_handle
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


/*===========================================================================

                       FORWARD FUNCTION DECLARATIONS 

===========================================================================*/


/* Define service handle table for WDA messages */
static qmi_csi_cb_error (* const req_handle_table[])
(
 qmi_wdai_client_info_type        *clnt_info,
 qmi_req_handle                    req_handle,
 int                               msg_id,
 void                    		  *req_c_struct,
 int                      		   req_c_struct_len,
 void                     		  *service_handle
) = 
{
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, /* Request handler for message ID 0x00 - 0x07*/
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, /* Request handler for message ID 0x08 - 0x0F*/
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, /* Request handler for message ID 0x10 - 0x17*/
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, /* Request handler for message ID 0x18 - 0x1F*/
  qmi_wdai_set_data_format,                /* Request handler for message ID 0x20 */
  qmi_wdai_get_data_format,                /* Request handler for message ID 0x21 */
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, /* Request handler for message ID 0x22 - 0x29*/
  NULL,			 						   /* Request handler for message ID 0x2A */
  qmi_wdai_set_qmap_settings,		       /* Request handler for message ID 0x2B */
  qmi_wdai_get_qmap_settings,			   /* Request handler for message ID 0x2C */
};

/*===========================================================================

                       EXTERNAL FUNCTION DEFINTIONS

===========================================================================*/

/*===========================================================================
  FUNCTION QMI_WDA_INIT()

  DESCRIPTION
    Register the WDA service with QMI Framework 

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

void qmi_wda_init
(
  void
)
{
  qmi_csi_error         rc;
  qmi_sap_error         src;    
  qmi_csi_options       options;  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG1(MSG_LEGACY_MED,"QMI WDA service init %d", qmi_wdai_inited);

  if (!qmi_wdai_inited)
  {
    /*-------------------------------------------------------------------------
      Setting DCC signal handler for handling QMI messages
    -------------------------------------------------------------------------*/
    (void)reg_set_sig_handler(DS_WDA_RDR_SIGNAL, 
                              qmi_wdai_msg_hdlr_cb, &qmi_wda_state);
    
    (void)rex_set_sigs( ds_get_ds_task_ptr(),
                    (rex_sigs_type) 1 << (rex_sigs_type)(DS_WDA_RDR_SIGNAL) );
    return;
  }

  os_params.tcb = ds_get_ds_task_ptr();
  os_params.sig = WDAI_QCSI_OS_SIGNAL;
  QMI_CSI_OPTIONS_INIT(options);
  (void)mqcsi_register_log_options(&options);

  /*-------------------------------------------------------------------------
    Register QMI WDA service with QCSI
  -------------------------------------------------------------------------*/
  rc = qmi_csi_register_with_options(wda_get_service_object_v01(),
                        qmi_wdai_svc_connect_cb,
                        qmi_wdai_svc_disconnect_cb, 
                        qmi_wdai_handle_client_req_cb, 
                        &qmi_wda_state, &os_params,
                        &options,
                        &qmi_wda_state.service_handle);
  
  if(rc != QMI_CSI_NO_ERR)
  {
    ERR_FATAL("Unable to register wda service! Error %d", rc, 0, 0);
    return;
  }

  //register with SAP
  src = qmi_sap_register(wda_get_service_object_v01(), NULL, &qmi_wda_state.sap_client_handle);
  if (src != QMI_SAP_NO_ERR)
  {
    ERR_FATAL("Unable to register WDA service with QSAP! Error %d", src, 0, 0);
    return;
  }


  /* Initialize the queue which will hold all handles of clients */
  (void) q_init(&qmi_wda_state.client_q);
  qmi_wda_state.num_of_clients = 0;

} /* qmi_wda_init */

/*===========================================================================
  FUNCTION qmi_wdai_process_cmd()

  DESCRIPTION
    This function processes a QMI WDA command

    It is called by the QMI command handler and will dispatch the
    associated command/event handler function.

  PARAMETERS
    cmd_ptr:  private data buffer containing the QMI WDA command
              information.

  RETURN VALUE
    None

  DEPENDENCIES
    QMI WDA must already have been initialized

  SIDE EFFECTS
    None
===========================================================================*/
 void qmi_wdai_process_cmd
(
  ds_command_enum_type  cmd,
  void              *cmd_ptr
)
{
  qmi_wdai_cmd_buf_type *cmd_buf_ptr = NULL;
/*-------------------------------------------------------------------------*/

  DS_ASSERT(cmd_ptr != NULL);
  cmd_buf_ptr = (qmi_wdai_cmd_buf_type *)cmd_ptr;

  DS_MSG1(MSG_LEGACY_MED,"QMI WDA process cmd: cmd_id:%d", cmd_buf_ptr->cmd_id);
} /* qmi_wdai_process_cmd() */

static boolean qmi_wdai_msg_hdlr_cb
(
  ds_sig_enum_type  sig,
  void              *svc_ptr
)
{
  qmi_wdai_state_info_type *wda_sp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_ASSERT((sig == DS_WDA_RDR_SIGNAL) && svc_ptr);

  if (!qmi_wdai_inited)
  {
    qmi_wdai_inited = TRUE;
    /* Call init again to register with QCSI now
     * Wait for all tasks to be inited */
    qmi_wda_init();
    return TRUE;
  }

  wda_sp = (qmi_wdai_state_info_type*) svc_ptr;

  qmi_csi_handle_event(wda_sp->service_handle, &os_params);
  return TRUE;
} /* qmi_wdai_msg_hdlr_cb */

static qmi_csi_cb_error qmi_wdai_svc_connect_cb
(
  qmi_client_handle          client_handle,
  void                      *service_handle,
  void                     **connection_handle
)
{
  qmi_wdai_client_info_type    *clnt_info_ptr;
  qmi_wdai_state_info_type     *wda_svc_ptr;
  qmi_csi_cb_error              rc;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  DS_ASSERT(connection_handle && service_handle );

  wda_svc_ptr = (qmi_wdai_state_info_type *) service_handle;
 
  if ( NULL != q_linear_search( &wda_svc_ptr->client_q,
                                wdai_compare_qmi_client_handle,
                                client_handle ) )
  {
    DS_MSG1(MSG_LEGACY_ERROR,"wdai_svc_connect_cb: DSD client 0x%p present",
                    client_handle);
    rc = QMI_CSI_CB_CONN_REFUSED;
    goto bail;
  }

  if (WDAI_MAX_CLIDS == wda_svc_ptr->num_of_clients)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"wdai_svc_connect_cb: "
            "Maximum number of clients already allocated %d",
                    WDAI_MAX_CLIDS);
    rc = QMI_CSI_CB_CONN_REFUSED;
    goto bail;
  }

  DS_SYSTEM_HEAP_MEM_ALLOC(clnt_info_ptr, sizeof(qmi_wdai_client_info_type),
                           qmi_wdai_client_info_type*);
  if(!clnt_info_ptr)
  {
    rc = QMI_CSI_CB_CONN_REFUSED;
    goto bail;
  }

  memset(clnt_info_ptr, 0, sizeof(qmi_wdai_client_info_type));
  (void) q_link ( clnt_info_ptr, &clnt_info_ptr->link );
  clnt_info_ptr->clnt = client_handle;
  /* Add it to the queue of clients */
  q_put( &(wda_svc_ptr->client_q), &(clnt_info_ptr->link) );
  wda_svc_ptr->num_of_clients++;

  *connection_handle = clnt_info_ptr;
  DS_MSG1(MSG_LEGACY_MED,"wdai_svc_connect_cb: Alloc client 0x%p", 
          clnt_info_ptr);
  rc = QMI_CSI_CB_NO_ERR;
bail:
  return rc;
} /* qmi_wdai_svc_connect_cb() */

static void qmi_wdai_svc_disconnect_cb
(
  void                      *connection_handle,
  void                      *service_handle
)
{
  qmi_wdai_state_info_type    *wda_svc_ptr;
  qmi_wdai_client_info_type    *clnt_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ASSERT(connection_handle && service_handle);
  wda_svc_ptr = (qmi_wdai_state_info_type *)service_handle;

  DS_MSG1(MSG_LEGACY_MED,"wdai_svc_disconnect_cb: Releasing client 0x%p",
                  connection_handle);

  clnt_info_ptr = q_linear_search( &wda_svc_ptr->client_q,
                                   wdai_compare_connection_info_handle,
                                   connection_handle );
  if (NULL == clnt_info_ptr)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"wdai_svc_connect_cb: WDA client 0x%p not found",
                    connection_handle);
    return;

  }

  if ( TRUE != q_delete_ext( &wda_svc_ptr->client_q,
                             &clnt_info_ptr->link ) )
  {
    DS_MSG1(MSG_LEGACY_ERROR,"wdai_svc_connect_cb: "
            "WDA client 0x%p could not be deleted from queue",
                    clnt_info_ptr);
    return;
  }
  DS_SYSTEM_HEAP_MEM_FREE(clnt_info_ptr);
  wda_svc_ptr->num_of_clients--;
} /* qmi_wdai_svc_disconnect_cb() */


static qmi_csi_cb_error qmi_wdai_handle_client_req_cb
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
  qmi_wdai_client_info_type     *clnt_info_ptr;
  qmi_wdai_state_info_type      *wda_svc_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ASSERT(connection_handle && service_handle);

  rc = QMI_CSI_CB_NO_ERR;
  wda_svc_ptr = (qmi_wdai_state_info_type*) service_handle;

  clnt_info_ptr = q_linear_search( &wda_svc_ptr->client_q,
                                   wdai_compare_connection_info_handle,
                                   connection_handle );
  if (NULL == clnt_info_ptr)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"wdai_handle_req_cb: Invalid clnt 0x%p",
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
      DS_MSG1(MSG_LEGACY_ERROR,"wdai_handle_req_cb: "
              "NULL message ID handler: %d",
                      msg_id);
    }
  }
  else
  {
    rc = QMI_CSI_CB_UNSUPPORTED_ERR;
    DS_MSG1(MSG_LEGACY_ERROR,"wdai_handle_req_cb: Invalid message ID: %d", 
            msg_id);
  }

  return rc;
} /* qmi_wdai_handle_client_req_cb() *

/*===========================================================================
  FUNCTION QMI_WDAI_SET_DATA_FORMAT()

  DESCRIPTION
    Handle the QMI WDS Admin set data format message.

    Dispatches a request to the specified service to set the data format.
    
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
    qmi_wda_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_wdai_set_data_format
(
  qmi_wdai_client_info_type 	   *clnt_info,
  qmi_req_handle		            req_handle,
  int					            msg_id,
  void					           *req_c_struct,
  int					            req_c_struct_len,
  void					           *service_handle  
)
{
  qmi_error_type_v01                errval;
  rmnet_data_format_type            data_format;
  uint32                            data_format_mask = 0;
  uint32                            ep_id;
  wda_set_data_format_req_msg_v01*  req_msg = NULL;
  wda_set_data_format_resp_msg_v01* resp_msg = NULL;  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG0(MSG_LEGACY_MED,"SET_DATA_Format");
  errval = QMI_ERR_NONE_V01;

  req_msg = (wda_set_data_format_req_msg_v01 *)req_c_struct;
  memset((void *)&data_format, 0, sizeof(rmnet_data_format_type));

  DS_SYSTEM_HEAP_MEM_ALLOC(resp_msg,
                           sizeof(wda_set_data_format_resp_msg_v01),
                           wda_set_data_format_resp_msg_v01*);
  if(resp_msg == NULL)
  {
    DS_ASSERT(0);
  }
  memset(resp_msg,0,sizeof(wda_set_data_format_resp_msg_v01));
  
  data_format_mask = qmi_wdai_parse_data_format_req(req_msg,&data_format);
  
  /*-------------------------------------------------------------------------
    Check ep_id TLV
  -------------------------------------------------------------------------*/
  ep_id = EP_TYPE_BAM_DEMUX;
  
  if (req_msg->ep_id_valid)
  {
    // Converting to uint32
    if (QMI_EP_ID_VALID(req_msg->ep_id))
    {
      ep_id = QMI_EP_ID_TO_INTERNAL_EP_ID(req_msg->ep_id);
    }
    else
    {
      DS_MSG2(MSG_LEGACY_ERROR,"Invalid ep_id 0x%x:0x%x",
                      req_msg->ep_id.ep_type, req_msg->ep_id.iface_id);
      errval = QMI_ERR_INVALID_ARG_V01;
      goto send_result;
    }
  }
  
  /*-------------------------------------------------------------------------
    Perform validity checks for QoS header format
  -------------------------------------------------------------------------*/

  if (data_format_mask & RMNET_DATA_FORMAT_MASK_QOS_FORMAT)
  {
    if (data_format.qos_format != RMNET_QOS_FORMAT_6_BYTE &&
        data_format.qos_format != RMNET_QOS_FORMAT_8_BYTE)
    {
      DS_MSG1(MSG_LEGACY_MED,"Invalid QoS header format %d",
              data_format.qos_format);
      errval = QMI_ERR_INVALID_ARG_V01;
      goto send_result;
    }
  }
  

  /*-------------------------------------------------------------------------
    Perform validity checks for Link Protocol Mode
  -------------------------------------------------------------------------*/
  if( (data_format_mask & RMNET_DATA_FORMAT_MASK_LINK_PROT) && 
      ((data_format.link_prot == RMNET_INVALID_MODE) ||
       (data_format.link_prot & (~RMNET_ALL_MODE))) )
  {
    DS_MSG1(MSG_LEGACY_MED,"Invalid link protocol(s) specified: 0x%x!",
                     data_format.link_prot);
    errval = QMI_ERR_INVALID_ARG_V01;
    goto send_result;
  }

  
   /*-------------------------------------------------------------------------
    Perform action/gather required information for response
    - get the service configuration block of the specified QMI service
    - allocate a client ID from the specified QMI service
  -------------------------------------------------------------------------*/
  DS_MSG6(MSG_LEGACY_MED,"Mask:%d, QoS format[%d], Link Prot[%d], "
                  "UL data agg[%d], DL data agg[%d], NDP sig[%d]",
                  data_format_mask,
                  data_format.qos,
                  data_format.link_prot,
                  data_format.ul_data_agg_protocol,
                  data_format.dl_data_agg_protocol,
                  data_format.ndp_signature );
  DS_MSG4(MSG_LEGACY_MED,"DL data agg max num[%d], DL data agg max size[%d], "
                  "DL min padding[%d], flow cntl[%d]",
                  data_format.dl_data_agg_max_num,
                  data_format.dl_data_agg_max_size,
                  data_format.dl_min_padding,
                  data_format.te_flow_control);

  if (RMNET_SET_DATA_FORMAT_QMI_CTL ==
      rmnet_phys_xport_data_format_client(ep_id))
  {
    DS_MSG0(MSG_LEGACY_ERROR,"Data format already set by QMI CTL");
    errval = QMI_ERR_INVALID_OPERATION_V01;
    goto send_result;
  }

  if (rmnet_phys_xport_set_data_format(ep_id,
                                   &data_format,
                                   data_format_mask,
                                   RMNET_SET_DATA_FORMAT_QMI_WDA) == FALSE)
  {
    errval = QMI_ERR_INVALID_OPERATION_V01;
    goto send_result;
  }

  /*-------------------------------------------------------------------------
    Attach the flow control TLV
  -------------------------------------------------------------------------*/
  if(data_format.qos == FALSE)
  {
    resp_msg->flow_control = data_format.te_flow_control;
    resp_msg->flow_control_valid = TRUE;
  }
  else
  {
    DS_MSG0(MSG_LEGACY_MED,"ignoring flow_control TLV"
            "as qos format is set and TRUE");
  }
  /*-------------------------------------------------------------------------
    DL minimum padding
  -------------------------------------------------------------------------*/
  resp_msg->dl_minimum_padding = data_format.dl_min_padding;
  resp_msg->dl_minimum_padding_valid = TRUE;

  /*-------------------------------------------------------------------------
    Attach the QoS header format TLV
  -------------------------------------------------------------------------*/
  if (data_format.qos)
  {
    resp_msg->qos_header_format_valid = TRUE;
    resp_msg->qos_header_format = 
                (wda_qos_header_format_enum_v01)data_format.qos_format;
  }
  
  if (data_format.ul_data_agg_max_num != 0)
  {
    /*-------------------------------------------------------------------------
      Attach the UL data aggregation max size TLV response
    -------------------------------------------------------------------------*/
    resp_msg->ul_data_aggregation_max_size = data_format.ul_data_agg_max_size;
	resp_msg->ul_data_aggregation_max_size_valid = TRUE;
    /*-------------------------------------------------------------------------
      Attach the UL data aggregation max datagrams TLV response
    -------------------------------------------------------------------------*/
    resp_msg->ul_data_aggregation_max_datagrams = 
                                  data_format.ul_data_agg_max_num;
	resp_msg->ul_data_aggregation_max_datagrams_valid = TRUE;
  }

  /*-------------------------------------------------------------------------
    Attach the DL data aggregation max size TLV response
  -------------------------------------------------------------------------*/
  resp_msg->dl_data_aggregation_max_size = data_format.dl_data_agg_max_size;
  resp_msg->dl_data_aggregation_max_size_valid = TRUE;

  /*-------------------------------------------------------------------------
    Attach the DL data aggregation max datagrams TLV response
  -------------------------------------------------------------------------*/
  resp_msg->dl_data_aggregation_max_datagrams = 
                                 data_format.dl_data_agg_max_num;
  resp_msg->dl_data_aggregation_max_datagrams_valid = TRUE;

  /*-------------------------------------------------------------------------
    Attach the NDP signature TLV response
  -------------------------------------------------------------------------*/
  resp_msg->ndp_signature = data_format.ndp_signature;
  resp_msg->ndp_signature_valid = TRUE;

  /*-------------------------------------------------------------------------
    Attach the DL data aggregation protocol TLV response
  -------------------------------------------------------------------------*/
  resp_msg->dl_data_aggregation_protocol = 
         (wda_dl_data_agg_protocol_enum_v01)data_format.dl_data_agg_protocol;
  resp_msg->dl_data_aggregation_protocol_valid = TRUE;

  /*-------------------------------------------------------------------------
    Attach the UL data aggregation protocol TLV response
  -------------------------------------------------------------------------*/
  resp_msg->ul_data_aggregation_protocol = 
         (wda_ul_data_agg_protocol_enum_v01)data_format.ul_data_agg_protocol;
  resp_msg->ul_data_aggregation_protocol_valid = TRUE;
  
  /*-------------------------------------------------------------------------
    Attach the Link protocol TLV response
  -------------------------------------------------------------------------*/
  resp_msg->link_prot = 
                   (wda_link_layer_protocol_enum_v01)data_format.link_prot;
  resp_msg->link_prot_valid =  TRUE;
  
  /*-------------------------------------------------------------------------
    Attach the QoS TLV response
  -------------------------------------------------------------------------*/
  resp_msg->qos_format = data_format.qos;
  resp_msg->qos_format_valid = TRUE;
  
  /*-------------------------------------------------------------------------
    build response
  -------------------------------------------------------------------------*/
send_result:
   resp_msg->resp.result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01
											  : QMI_RESULT_FAILURE_V01);
   resp_msg->resp.error = errval; 

   qmi_csi_send_resp(req_handle, msg_id, resp_msg, 
						  sizeof(wda_set_data_format_resp_msg_v01));

  if(resp_msg!= NULL) DS_SYSTEM_HEAP_MEM_FREE(resp_msg);

  return QMI_CSI_CB_NO_ERR;
} /* qmi_wdai_set_data_format */

/*===========================================================================
  FUNCTION QMI_WDA_GET_DATA_FORMAT()

  DESCRIPTION
    Handle the QMI WDA Admin get data format message.

    Dispatches a request to the specified service to get the data format.

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
    qmi_wda_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_wdai_get_data_format
(
  qmi_wdai_client_info_type	  *clnt_info,
  qmi_req_handle 		       req_handle,
  int					       msg_id,
  void					      *req_c_struct,
  int					       req_c_struct_len,
  void					      *service_handle 
)
{
  qmi_error_type_v01                errval;
  rmnet_data_format_type            data_format;
  uint32                            ep_id;
  wda_get_data_format_resp_msg_v01 *resp_msg = NULL;
  wda_get_data_format_req_msg_v01  *req_msg;  

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_MSG0(MSG_LEGACY_MED,"GET_DATA_Format");

  errval = QMI_ERR_NONE_V01;
  memset((void *)&data_format, 0, sizeof(rmnet_data_format_type));

  DS_SYSTEM_HEAP_MEM_ALLOC(resp_msg,
                           sizeof(wda_get_data_format_resp_msg_v01),
                           wda_get_data_format_resp_msg_v01*);
  if (resp_msg == NULL)
  {
    return QMI_CSI_CB_NO_MEM;
  }
  memset(resp_msg,0,sizeof(wda_get_data_format_resp_msg_v01));
  req_msg = (wda_get_data_format_req_msg_v01 *)req_c_struct;
  
  /*-------------------------------------------------------------------------
    Check ep_id TLV
  -------------------------------------------------------------------------*/
  ep_id = EP_TYPE_BAM_DEMUX;

  if (req_msg->ep_id_valid)
  {
    // Converting to uint32
    if (QMI_EP_ID_VALID(req_msg->ep_id))
    {
      ep_id = QMI_EP_ID_TO_INTERNAL_EP_ID(req_msg->ep_id);
    }
    else
    {
      DS_MSG2(MSG_LEGACY_ERROR,"Invalid ep_id 0x%x:0x%x",
                      req_msg->ep_id.ep_type, req_msg->ep_id.iface_id);
      errval = QMI_ERR_INVALID_ARG_V01;
      goto send_result;
    }
  }
  if (!rmnet_phys_xport_get_data_format(ep_id, &data_format))
  {
    errval = QMI_ERR_INVALID_ARG_V01;
    goto send_result;
  }
  /*-------------------------------------------------------------------------
    Attach QoS format TLV
  -------------------------------------------------------------------------*/
  if (data_format.qos)
  {
    resp_msg->qos_header_format_valid = TRUE;
    resp_msg->qos_header_format = 
                (wda_qos_header_format_enum_v01)data_format.qos_format;
  }
  /*-------------------------------------------------------------------------
    Attach the flow control TLV
  -------------------------------------------------------------------------*/
  resp_msg->flow_control = data_format.te_flow_control;
  resp_msg->flow_control_valid = TRUE;
  /*-------------------------------------------------------------------------
    DL minimum padding
  -------------------------------------------------------------------------*/
  resp_msg->dl_minimum_padding = data_format.dl_min_padding;
  resp_msg->dl_minimum_padding_valid = TRUE;

  if (data_format.ul_data_agg_max_num != 0)
  {
    /*-------------------------------------------------------------------------
      Attach the UL data aggregation max size TLV response
    -------------------------------------------------------------------------*/
    resp_msg->ul_data_aggregation_max_size = 
                                    data_format.ul_data_agg_max_size;
    resp_msg->ul_data_aggregation_max_size_valid = TRUE;
    /*-------------------------------------------------------------------------
      Attach the UL data aggregation max datagrams TLV response
    -------------------------------------------------------------------------*/
    resp_msg->ul_data_aggregation_max_datagrams =
                                     data_format.ul_data_agg_max_num;
    resp_msg->ul_data_aggregation_max_datagrams_valid = TRUE;
  }

  /*-------------------------------------------------------------------------
    Attach the DL data aggregation max size TLV response
  -------------------------------------------------------------------------*/
  resp_msg->dl_data_aggregation_max_size = 
                               data_format.dl_data_agg_max_size;
  resp_msg->dl_data_aggregation_max_size_valid = TRUE;

  /*-------------------------------------------------------------------------
    Attach the DL data aggregation max datagrams TLV response
  -------------------------------------------------------------------------*/
  resp_msg->dl_data_aggregation_max_datagrams =
                                data_format.dl_data_agg_max_num;
  resp_msg->dl_data_aggregation_max_datagrams_valid = TRUE;

  /*-------------------------------------------------------------------------
    Attach the NDP signature TLV response
  -------------------------------------------------------------------------*/
  resp_msg->ndp_signature = data_format.ndp_signature;
  resp_msg->ndp_signature_valid = TRUE;

  /*-------------------------------------------------------------------------
    Attach the DL data aggregation protocol TLV response
  -------------------------------------------------------------------------*/
  resp_msg->dl_data_aggregation_protocol = 
          (wda_dl_data_agg_protocol_enum_v01)data_format.dl_data_agg_protocol;
  resp_msg->dl_data_aggregation_protocol_valid = TRUE;

  /*-------------------------------------------------------------------------
    Attach the UL data aggregation protocol TLV response
  -------------------------------------------------------------------------*/
  resp_msg->ul_data_aggregation_protocol = 
          (wda_ul_data_agg_protocol_enum_v01)data_format.ul_data_agg_protocol;
  resp_msg->ul_data_aggregation_protocol_valid = TRUE;
  
  /*-------------------------------------------------------------------------
    Attach the Link protocol TLV response
  -------------------------------------------------------------------------*/
  resp_msg->link_prot = 
                   (wda_link_layer_protocol_enum_v01)data_format.link_prot;
  resp_msg->link_prot_valid =  TRUE;
  
  /*-------------------------------------------------------------------------
    Attach the QoS TLV response
  -------------------------------------------------------------------------*/
  resp_msg->qos_format = data_format.qos;
  resp_msg->qos_format_valid = TRUE;

/*-------------------------------------------------------------------------
    build response
  -------------------------------------------------------------------------*/
send_result:
   resp_msg->resp.result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01
												: QMI_RESULT_FAILURE_V01);
   resp_msg->resp.error = errval;
		
   qmi_csi_send_resp(req_handle, msg_id, resp_msg, 
							sizeof(wda_set_data_format_resp_msg_v01));

  if(resp_msg!= NULL) DS_SYSTEM_HEAP_MEM_FREE(resp_msg);
  return QMI_CSI_CB_NO_ERR;
} /* qmi_wdslitei_get_data_format*/

/*===========================================================================
  FUNCTION QMI_WDAI_SET_QMAP_SETTINGS()

  DESCRIPTION
    Handle the QMI WDA set qmap settings message.

    Dispatches a request to the specified service to
    set qmap settings.

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
    qmi_wda_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_wdai_set_qmap_settings
(
  qmi_wdai_client_info_type	  *clnt_info,
  qmi_req_handle			   req_handle,
  int 					       msg_id,
  void				          *req_c_struct,
  int 					       req_c_struct_len,
  void					      *service_handle 
)
{
  qmi_error_type_v01                 errval;
  rmnet_qmap_settings_type           qmap_settings;
  uint32                             ep_id;
  wda_set_qmap_settings_req_msg_v01 *req_msg;
  wda_set_qmap_settings_resp_msg_v01 resp_msg;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_MSG0(MSG_LEGACY_MED,"SET_QMAP_Settings");

  memset(&qmap_settings, 0, sizeof(qmap_settings));
  memset(&resp_msg,0,sizeof(wda_set_qmap_settings_resp_msg_v01));
  
  errval            = QMI_ERR_NONE_V01;

  req_msg = (wda_set_qmap_settings_req_msg_v01 *)req_c_struct;

  /*-------------------------------------------------------------------------
    Check ep_id TLV
  -------------------------------------------------------------------------*/
  ep_id = EP_TYPE_BAM_DEMUX;

  if (req_msg->ep_id_valid)
  {
    // Converting to uint32
    if (QMI_EP_ID_VALID(req_msg->ep_id))
    {
      ep_id = QMI_EP_ID_TO_INTERNAL_EP_ID(req_msg->ep_id);
    }
    else
    {
      DS_MSG2(MSG_LEGACY_ERROR,"Invalid ep_id 0x%x:0x%x",
                      req_msg->ep_id.ep_type, req_msg->ep_id.iface_id);
      errval = QMI_ERR_INVALID_ARG_V01;
      goto send_result;
    }
  }
  if ( !rmnet_phys_xport_get_qmap_settings(ep_id, &qmap_settings) )
  {
    DS_MSG1(MSG_LEGACY_ERROR,"Failed to get QMAP settings for EP 0x%x", ep_id);
    errval = QMI_ERR_INTERNAL_V01;
    goto send_result;
  }

  if (req_msg->in_band_flow_control_valid)
  {
    qmap_settings.in_band_fc = (req_msg->in_band_flow_control)? TRUE : FALSE;
  }

  if ( !rmnet_phys_xport_set_qmap_settings(ep_id, &qmap_settings) )
  {
    DS_MSG1(MSG_LEGACY_ERROR,"Failed to set QMAP settings for EP 0x%x", ep_id);
    errval = QMI_ERR_NOT_SUPPORTED_V01;
    goto send_result;
  }
  
  /*---------------------------------------------------------------------
    Send response
  ---------------------------------------------------------------------*/
  memset(&qmap_settings, 0, sizeof(qmap_settings));
  if ( !rmnet_phys_xport_get_qmap_settings(ep_id, &qmap_settings) )
  {
    DS_MSG1(MSG_LEGACY_ERROR,"Failed to get QMAP settings for EP 0x%x", ep_id);
    errval = QMI_ERR_INTERNAL_V01;
    goto send_result;
  }
  resp_msg.in_band_flow_control_valid = TRUE;
  resp_msg.in_band_flow_control = qmap_settings.in_band_fc ? 1 : 0;

send_result:

  resp_msg.resp.result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01
                                  : QMI_RESULT_FAILURE_V01);
  resp_msg.resp.error = errval;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(wda_set_qmap_settings_resp_msg_v01));

  return QMI_CSI_CB_NO_ERR;
} /* qmi_wdai_set_qmap_settings()*/

/*===========================================================================

 FUNCTION QMI_WDAI_GET_QMAP_SETTINGS()
  DESCRIPTION
    Handle the QMI WDA get qmap settings message.

    Dispatches a request to the specified service to
    get current qmap settings.

  PARAMETERS
    req_c_struct       :       Request Message with all TLV
    req_handle         :       Request Handle
    clnt_info          :       Client Info Handle
    msg_id             :       Message ID
    req_c_struct_len   :       length of request message
    service_handle     :       WDA service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    qmi_wda_init() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_wdai_get_qmap_settings
(
   qmi_wdai_client_info_type  *clnt_info,
   qmi_req_handle 		       req_handle,
   int				           msg_id,
   void				  		  *req_c_struct,
   int				   		   req_c_struct_len,
   void				   		  *service_handle 
)
{
  qmi_error_type_v01       		   	 errval;
  uint32                           	 ep_id;
  rmnet_qmap_settings_type         	 qmap_settings;
  wda_get_qmap_settings_req_msg_v01 *req_msg;
  wda_get_qmap_settings_resp_msg_v01 resp_msg;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_MSG0(MSG_LEGACY_MED,"GET_QMAP_Settings");
  memset(&qmap_settings, 0, sizeof(qmap_settings));
  errval = QMI_ERR_NONE_V01;

  memset(&resp_msg,0,sizeof(wda_set_qmap_settings_resp_msg_v01));
  req_msg = (wda_get_qmap_settings_req_msg_v01 *)req_c_struct;
  
  /*-------------------------------------------------------------------------
    Check ep_id TLV
  -------------------------------------------------------------------------*/
  ep_id = EP_TYPE_BAM_DEMUX;
  
  if (req_msg->ep_id_valid)
  {
    // Converting to uint32
    if (QMI_EP_ID_VALID(req_msg->ep_id))
    {
      ep_id = QMI_EP_ID_TO_INTERNAL_EP_ID(req_msg->ep_id);
    }
    else
    {
      DS_MSG2(MSG_LEGACY_ERROR,"Invalid ep_id 0x%x:0x%x",
                      req_msg->ep_id.ep_type, req_msg->ep_id.iface_id);
      errval = QMI_ERR_INVALID_ARG_V01;
      goto send_result;
    }
  }
  if ( !rmnet_phys_xport_get_qmap_settings(ep_id, &qmap_settings) )
  {
    DS_MSG1(MSG_LEGACY_ERROR,"Failed to get QMAP settings for EP 0x%x", ep_id);
    errval = QMI_ERR_INTERNAL_V01;
  }
  resp_msg.in_band_flow_control_valid = TRUE;
  resp_msg.in_band_flow_control = qmap_settings.in_band_fc ? TRUE : FALSE;

send_result:

  resp_msg.resp.result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01
                                   : QMI_RESULT_FAILURE_V01);
  resp_msg.resp.error = errval;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(wda_get_qmap_settings_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
}/* qmi_wdai_get_qmap_settings() */

/*===========================================================================
  FUNCTION QMI_WDAI_PARSE_DATA_FORMAT_REQ()

  DESCRIPTION
    Fills rmnet data format type from set_data_format request message

  PARAMETERS
    req_msg -- request message struct from set_data_format
    data-format -- rmnet structure to be filled

  RETURN VALUE
    uint32 - mask for valid parameters

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
static uint32 qmi_wdai_parse_data_format_req
(
  wda_set_data_format_req_msg_v01* req_msg,
  rmnet_data_format_type* data_format
)
{
  
  uint32 data_format_mask = 0;

  if(req_msg->qos_format_valid)
  {
    data_format_mask |= RMNET_DATA_FORMAT_MASK_QOS;
    data_format->qos = req_msg->qos_format;
  }
  if(req_msg->link_prot_valid)
  {
    data_format_mask |= RMNET_DATA_FORMAT_MASK_LINK_PROT;
    data_format->link_prot = (rmnet_sm_link_prot_e_type)req_msg->link_prot;
  }
  if(req_msg->ul_data_aggregation_protocol_valid)
  {
    data_format_mask |= RMNET_DATA_FORMAT_MASK_UL_DATA_AGG;
    data_format->ul_data_agg_protocol = 
                (rmnet_data_agg_enum_type)req_msg->ul_data_aggregation_protocol;
  }
  if(req_msg->dl_data_aggregation_protocol_valid)
  {
    data_format_mask |= RMNET_DATA_FORMAT_MASK_DL_DATA_AGG;
    data_format->dl_data_agg_protocol = 
                (rmnet_data_agg_enum_type)req_msg->dl_data_aggregation_protocol;
  }
  if(req_msg->ndp_signature_valid)
  {
    data_format_mask |= RMNET_DATA_FORMAT_MASK_NDP_SIGNATURE;
    data_format->ndp_signature = req_msg->ndp_signature;
  }
  if(req_msg->dl_data_aggregation_max_datagrams_valid)
  {
    data_format_mask |= RMNET_DATA_FORMAT_MASK_DL_DATA_AGG_NUM;
    data_format->dl_data_agg_max_num =
             req_msg->dl_data_aggregation_max_datagrams;
  }
  if(req_msg->dl_data_aggregation_max_size_valid)
  {
    data_format_mask |= RMNET_DATA_FORMAT_MASK_DL_DATA_AGG_SIZE;
    data_format->dl_data_agg_max_size = req_msg->dl_data_aggregation_max_size;
  }
  if(req_msg->dl_minimum_padding_valid)
  {
    data_format_mask |= RMNET_DATA_FORMAT_MASK_DL_MIN_PADDING;
    data_format->dl_min_padding = req_msg->dl_minimum_padding;
  }
  if(req_msg->qos_header_format_valid)
  {
    data_format_mask |= RMNET_DATA_FORMAT_MASK_QOS_FORMAT;
    data_format->qos_format =
                (rmnet_qos_format_e_type)req_msg->qos_header_format;
  }
  if(req_msg->flow_control_valid)
  {
    data_format_mask |= RMNET_DATA_FORMAT_MASK_FLOW_CONTROL;
    /* Honor the flow_control TLV only if qos is not present or 0 */
    if (data_format->qos == FALSE)
    {
      data_format->te_flow_control = (req_msg->flow_control)? TRUE : FALSE;
    }
    else
    {
       DS_MSG0(MSG_LEGACY_MED,"Ignoring flow_control TLV "
               "since qos_format is present and TRUE");
    }
  }
  return data_format_mask;
}/* qmi_wdai_parse_data_format_req */

