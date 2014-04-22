/*===========================================================================

                         D S _ Q M I _ DFS . C

DESCRIPTION

  The Data Services QMI Data Filter service source file.

EXTERNALIZED FUNCTIONS

  qmi_dfs_init()
    Initialize the QMI DFS service

Copyright (c) 2013-2020 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/interface/qmidata/src/ds_qmi_dfs.c#1 $
  $DateTime: 2020/03/17 08:03:16 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/06/17    ss     Return appropriate error code when message is not suppoted
09/25/13    bh     Created module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "amssassert.h"

#include "ds_qmi_dfs.h"
#include "ds_system_heap.h"
#include "ds_sys.h"
#include "data_msg.h"
#include "qmi_idl_lib.h"
#include "qmi_idl_lib_internal.h"
#include "qmi_csi.h"
#include "qmi_csi_target_ext.h"
#include "mqcsi_log.h"
#include "common_v01.h"
#include "data_filter_service_v01.h"
#include "qmi_sap.h"
#include "queue.h"
#include "qmi_framework.h"
#include "ds_rmnet_defs.h"
#include "ds_qmi_if.h"
#include "ds_qmux.h"
#include "ds_rmnet_meta_sm.h"
#include "ds_rmnet_xport.h"
#include <stringl/stringl.h>
#include "ps_utils.h"
#include "ds_sys_event.h"
#include "ds_qmi_defs.h"
#include "ds_sys_ioctl.h"

/*---------------------------------------------------------------------------
  Service configuration
  - DPM service version is 1.0
---------------------------------------------------------------------------*/
#define DFSI_BASE_VER_MAJOR  (1)
#define DFSI_BASE_VER_MINOR  (4)

#define DFSI_ADDENDUM_VER_MAJOR  (0)
#define DFSI_ADDENDUM_VER_MINOR  (0)

#define DFSI_MAX_CLIDS    (QMI_FRAMEWORK_SVC_MAX_CLIENTS - 1)
#define DFSI_QCSI_OS_SIGNAL ((rex_sigs_type)(1 << DS_DFS_RDR_SIGNAL))

#define DFSI_MAX_REV_IP_FLTRS   8

/*To Verify whether the call is up or not on passed rmnet_instance*/
#define QMI_DFSI_IS_CALL_UP(rmnet_instance) \
                        if(rmnet_meta_sm_in_call(rmnet_instance) == FALSE)\
                         {\
                           DS_MSG1(MSG_LEGACY_ERROR,"Invalid rmnet_instance id %d", rmnet_instance );\
                           resp_msg.resp.error = QMI_ERR_OUT_OF_CALL_V01;\
                           resp_msg.resp.result = QMI_RESULT_FAILURE_V01;\
                           goto bail;\
                         }
#define QMI_DFSI_GET_RMNET_INSTANCE_FROM_EP_AND_MUX_ID(bound_ip_type,ep_id,mux_id,rmnet_instance)\
                         if (bound_ip_type == DFS_IP_FAMILY_IPV6_V01)\
                         {\
                           rmnet_instance = rmnet_inst_from_ep_and_mux_id(ep_id, mux_id, 0,QMI_AF_INET6);\
                         }\
                         else\
                         {\
                           rmnet_instance = rmnet_inst_from_ep_and_mux_id(ep_id, mux_id, 1,QMI_AF_INET);\
                         }\
                         if (rmnet_instance >= RMNET_INSTANCE_MAX)\
                         {\
                           DS_MSG2(MSG_LEGACY_ERROR,"Invalid EP id 0x%x and mux_id %d", ep_id, mux_id);\
                           resp_msg.resp.error  = QMI_ERR_OUT_OF_CALL_V01;\
                           resp_msg.resp.result = QMI_RESULT_FAILURE_V01;\
                           goto bail;\
                         }

/*---------------------------------------------------------------------------
  QMI DFS Client state info
---------------------------------------------------------------------------*/
typedef struct {
  q_link_type              link;           /*< Data Structure required by the queue 
                                              library to add this as part of a 
                                              queue */
  qmi_client_handle        clnt;
  dfs_ip_family_enum_v01   bound_ip_type;
  uint32                   ep_id;          // Current binding physical end point
  uint8                    mux_id;         // Current (binding) Mux Id
  dfs_bind_subscription_enum_v01 subscription;
  boolean                  is_already_bound;
}qmi_dfsi_client_info_type;

/*---------------------------------------------------------------------------
  QMI DFS Service state info
---------------------------------------------------------------------------*/
typedef struct
{
  q_type                 client_q; /*< Information about all the clients that
                                       have connected to the service. This is queue 
                                       of type qmi_dfsi_client_info_type */
  qmi_csi_service_handle service_handle;
  qmi_sap_client_handle  sap_client_handle;
  uint16                 num_of_clients;
} qmi_dfsi_state_info_type;

static qmi_dfsi_state_info_type      qmi_dfs_state;
static qmi_csi_os_params             os_params;
static boolean                       qmi_dfsi_inited = FALSE;

/*===========================================================================

                               INTERNAL DATA

===========================================================================*/

/*---------------------------------------------------------------------------
  QMI service command handlers
  forward declarations & cmd handler dispatch table definition
---------------------------------------------------------------------------*/
static qmi_csi_cb_error  qmi_dfsi_bind_client(
  qmi_dfsi_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);
static qmi_csi_cb_error  qmi_dfsi_get_client_binding(
  qmi_dfsi_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle
);

static boolean qmi_dfsi_msg_hdlr_cb
(
  ds_sig_enum_type  sig,
  void              *svc_ptr
);

static qmi_csi_cb_error qmi_dfsi_svc_connect_cb
(
  qmi_client_handle          client_handle,
  void                      *service_handle,
  void                     **connection_handle
);

static void qmi_dfsi_svc_disconnect_cb
(
  void                      *connection_handle,
  void                      *service_handle
);

static qmi_csi_cb_error qmi_dfsi_handle_client_req_cb
(
  void                     *connection_handle,
  qmi_req_handle           req_handle,
  unsigned int             msg_id,
  void                     *req_c_struct,
  unsigned int             req_c_struct_len,
  void                     *service_handle
);
static qmi_csi_cb_error qmi_dfsi_add_mo_excp_filters
(
  qmi_dfsi_client_info_type *clnt_info,
  qmi_req_handle             req_handle,
  int                        msg_id,
  void                      *req_c_struct,
  int                        req_c_struct_len,
  void                      *service_handle
);
static qmi_csi_cb_error qmi_dfsi_remove_mo_excp_filters
(
  qmi_dfsi_client_info_type *clnt_info,
  qmi_req_handle             req_handle,
  int                        msg_id,
  void                      *req_c_struct,
  int                        req_c_struct_len,
  void                      *service_handle
);

static void qmi_dfsi_fill_v4_filter_spec
(
  dfs_filter_rule_type_v01*   filter_rules,
  ip_filter_type*             fltr_spec
);

static void qmi_dfsi_fill_v6_filter_spec
(
  dfs_filter_rule_type_v01*   filter_rules,
  ip_filter_type*             fltr_spec
);

static void qmi_dfsi_fill_fltr_xport_info
(
  dfs_xport_header_type_v01*  filter_rules,
  ip_filter_type*             fltr_spec
);

static void qmi_dfsi_fill_filter_error_mask
(
  ip_filter_type*                  filter_spec,
  dfs_filter_param_error_mask_v01* error_mask
);

static qmi_csi_cb_error qmi_dfsi_add_pdn_sharing_filters
(
  qmi_dfsi_client_info_type        *clnt_info,
  qmi_req_handle                    req_handle,
  int                               msg_id,
  void                             *req_c_struct,
  int                               req_c_struct_len,
  void                             *service_handle
);

static qmi_csi_cb_error qmi_dfsi_remove_pdn_sharing_filters
(
  qmi_dfsi_client_info_type        *clnt_info,
  qmi_req_handle                    req_handle,
  int                               msg_id,
  void                             *req_c_struct,
  int                               req_c_struct_len,
  void                             *service_handle
);


/*===========================================================================

                       FORWARD FUNCTION DECLARATIONS 

===========================================================================*/


/* Define service handle table for DFS messages */
static qmi_csi_cb_error (* const req_handle_table[])
(
 qmi_dfsi_client_info_type        *clnt_info,
 qmi_req_handle           req_handle,
 int                      msg_id,
 void                    *req_c_struct,
 int                      req_c_struct_len,
 void                     *service_handle
) = 
{
  NULL,NULL,NULL,                            /* Request handler for message ID 0x00 - 0x02*/
  NULL,							             /* Request handler for message ID 0x03 */
  NULL,NULL,NULL,NULL,                       /* Request handler for message ID 0x04 - 0x07*/
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,   /* Request handler for message ID 0x08 - 0x0F*/
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,   /* Request handler for message ID 0x10 - 0x17*/
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,   /* Request handler for message ID 0x18 - 0x1F*/
  NULL,							             /* Request handler for message ID 0x20 */
  qmi_dfsi_bind_client,                      /* Request handler for message ID 0x21 */
  qmi_dfsi_get_client_binding,               /* Request handler for message ID 0x22 */
  NULL,								         /* Request handler for message ID 0x23 */
  NULL,    									 /* Request handler for message ID 0x24 */
  NULL,									     /* Request handler for message ID 0x25 */
  qmi_dfsi_add_pdn_sharing_filters,          /* Request handler for message ID 0x26 */
  qmi_dfsi_remove_pdn_sharing_filters,       /* Request handler for message ID 0x27 */
  NULL,							             /* Request handler for message ID 0x28 */
  NULL,								         /* Request handler for message ID 0x29 */
  NULL,								         /* Request handler for message ID 0x2A */
  NULL,										 /* Request handler for message ID 0x2B */
  NULL,                                      /* Request handler for message ID 0x2C */
  NULL,									     /* Request handler for message ID 0x2D */
  NULL, 
  NULL,
  NULL,										 /* Request handler for message ID 0x30 */ 
  NULL,
  NULL,
  NULL,								         /* Request handler for message ID 0x33 */          
  NULL,
  NULL,
  NULL,
  qmi_dfsi_remove_mo_excp_filters,           /* Request handler for message ID 0x37 */
  qmi_dfsi_add_mo_excp_filters,              /* Request handler for message ID 0x38 */
};

extern rex_tcb_type dcc_tcb;

static int dfsi_compare_qmi_client_handle
( 
  void *item_ptr,
  void *compare_val 
)
{
  qmi_dfsi_client_info_type *clnt_info = (qmi_dfsi_client_info_type*)item_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ( (clnt_info->clnt == 
                          (qmi_client_handle)compare_val) ? 1 : 0 );
}

static int dfsi_compare_connection_info_handle
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

                       EXTERNAL FUNCTION DEFINTIONS

===========================================================================*/

/*===========================================================================
  FUNCTION QMI_DFS_INIT()

  DESCRIPTION
    Register the DFS service with QCSI 

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_dfs_init
(
  void
)
{
  qmi_csi_error         rc;
  qmi_sap_error         src;
  qmi_csi_options       options;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG1(MSG_LEGACY_MED,"QMI DFS service init %d", qmi_dfsi_inited);

  if (!qmi_dfsi_inited)
  {
    /*-------------------------------------------------------------------------
      Setting DCC signal handler for handling QMI messages
    -------------------------------------------------------------------------*/
    (void)reg_set_sig_handler(DS_DFS_RDR_SIGNAL, 
                              qmi_dfsi_msg_hdlr_cb, &qmi_dfs_state);
    //ds_enable_sig_ex(DS_DFS_RDR_SIGNAL);

    /* Set the signal to init again */
    //DS_SET_SIGNAL(DS_DFS_RDR_SIGNAL);
    (void)rex_set_sigs( ds_get_ds_task_ptr(),
                      (rex_sigs_type) 1 << (rex_sigs_type)(DS_DFS_RDR_SIGNAL) ); 
    return;
  }
  
  os_params.tcb = ds_get_ds_task_ptr();
  os_params.sig = DFSI_QCSI_OS_SIGNAL;
  
  QMI_CSI_OPTIONS_INIT(options);
  (void)mqcsi_register_log_options(&options);
  
  /*-------------------------------------------------------------------------
    Register QMI dfs service with QCSI
  -------------------------------------------------------------------------*/
  rc = qmi_csi_register_with_options(dfs_get_service_object_v01(),
                        qmi_dfsi_svc_connect_cb,
                        qmi_dfsi_svc_disconnect_cb, 
                        qmi_dfsi_handle_client_req_cb, 
                        &qmi_dfs_state, &os_params, 
                        &options,
                        &qmi_dfs_state.service_handle);
  
  if(rc != QMI_CSI_NO_ERR)
  {
    ERR_FATAL("Unable to register dfs service! Error %d", rc, 0, 0);
    DS_ASSERT(0);
    return;
  }

  //register with SAP
  src = qmi_sap_register(dfs_get_service_object_v01(), NULL, &qmi_dfs_state.sap_client_handle);
  if (src != QMI_SAP_NO_ERR)
  {
    ERR_FATAL("Unable to register dfs service with QSAP! Error %d", src, 0, 0);
    DS_ASSERT(0);
    return;
  }


  /* Initialize the queue which will hold all handles of clients */
  (void)q_init(&qmi_dfs_state.client_q);
  qmi_dfs_state.num_of_clients = 0;
  
} /* qmi_dfs_init */

static boolean qmi_dfsi_msg_hdlr_cb
(
  ds_sig_enum_type  sig,
  void              *svc_ptr
)
{
  qmi_dfsi_state_info_type *dfs_sp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_ASSERT((sig == DS_DFS_RDR_SIGNAL) && svc_ptr);

  if (!qmi_dfsi_inited)
  {
    qmi_dfsi_inited = TRUE;
    /* Call init again to register with QCSI now
     * Wait for all tasks to be inited */
    qmi_dfs_init();
    return TRUE;
  }

  dfs_sp = (qmi_dfsi_state_info_type*) svc_ptr;

  qmi_csi_handle_event(dfs_sp->service_handle, &os_params);
  return TRUE;
} /* qmi_dfsi_msg_hdlr_cb */

static qmi_csi_cb_error qmi_dfsi_svc_connect_cb
(
  qmi_client_handle          client_handle,
  void                      *service_handle,
  void                     **connection_handle
)
{
  qmi_dfsi_client_info_type    *clnt_info_ptr;
  qmi_dfsi_state_info_type     *dfs_svc_ptr;
  qmi_csi_cb_error             rc;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  DS_ASSERT(connection_handle !=NULL);
  DS_ASSERT(service_handle != NULL);

  dfs_svc_ptr = (qmi_dfsi_state_info_type *) service_handle;
 
  if ( NULL != q_linear_search( &dfs_svc_ptr->client_q,
                                dfsi_compare_qmi_client_handle,
                                client_handle ) )
  {
    DS_MSG1(MSG_LEGACY_ERROR,"dfsi_svc_connect_cb: dfs client 0x%p present",
                  client_handle);
    rc = QMI_CSI_CB_CONN_REFUSED;
    goto bail;
  }

  if (DFSI_MAX_CLIDS == dfs_svc_ptr->num_of_clients)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"dfsi_svc_connect_cb: Maximum number of clients already allocated %d",
                  DFSI_MAX_CLIDS);
    rc = QMI_CSI_CB_CONN_REFUSED;
    goto bail;
  }

  DS_SYSTEM_HEAP_MEM_ALLOC(clnt_info_ptr, sizeof(qmi_dfsi_client_info_type),
                           qmi_dfsi_client_info_type*);
  if(!clnt_info_ptr)
  {
    rc = QMI_CSI_CB_CONN_REFUSED;
    goto bail;
  }

  memset(clnt_info_ptr, 0, sizeof(qmi_dfsi_client_info_type));
  clnt_info_ptr->bound_ip_type = DFS_IP_FAMILY_IPV4_V01;
  clnt_info_ptr->subscription =  DFS_DONT_CARE_SUBS_V01; 
  clnt_info_ptr->is_already_bound = FALSE;

  (void) q_link ( clnt_info_ptr, &clnt_info_ptr->link );
  clnt_info_ptr->clnt = client_handle;
  /* Add it to the queue of clients */
  q_put( &(dfs_svc_ptr->client_q), &(clnt_info_ptr->link) );
  dfs_svc_ptr->num_of_clients++;

  *connection_handle = clnt_info_ptr;
  DS_MSG1(MSG_LEGACY_MED,"dfsi_svc_connect_cb: Alloc client 0x%p", clnt_info_ptr);
  rc = QMI_CSI_CB_NO_ERR;
bail:
  return rc;
} /* qmi_dfsi_svc_connect_cb() */

static void qmi_dfsi_svc_disconnect_cb
(
  void                      *connection_handle,
  void                      *service_handle
)
{
  qmi_dfsi_state_info_type    *dfs_svc_ptr;
  qmi_dfsi_client_info_type    *clnt_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ASSERT(connection_handle != NULL);
  DS_ASSERT(service_handle != NULL);
  dfs_svc_ptr = (qmi_dfsi_state_info_type *)service_handle;

  DS_MSG1(MSG_LEGACY_MED,"dfsi_svc_disconnect_cb: Releasing client 0x%p", 
                connection_handle);

  clnt_info_ptr = q_linear_search( &dfs_svc_ptr->client_q,
                                   dfsi_compare_connection_info_handle,
                                   connection_handle );
  if(NULL == clnt_info_ptr)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"dfsi_svc_connect_cb: dfs client 0x%p not found",
                  connection_handle);
    return;

  }

  if(TRUE != q_delete_ext( &dfs_svc_ptr->client_q,
                             &clnt_info_ptr->link ) )
  {
    DS_MSG1(MSG_LEGACY_ERROR,"dfsi_svc_connect_cb: dfs client 0x%p could not be deleted from queue",
                    clnt_info_ptr);
    return;
  }
  DS_SYSTEM_HEAP_MEM_FREE(clnt_info_ptr);
  dfs_svc_ptr->num_of_clients--;
} /* qmi_dfsi_svc_disconnect_cb() */


static qmi_csi_cb_error qmi_dfsi_handle_client_req_cb
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
  qmi_dfsi_client_info_type     *clnt_info_ptr;
  qmi_dfsi_state_info_type      *dfs_svc_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ASSERT(connection_handle != NULL);
  DS_ASSERT(service_handle != NULL);

  rc = QMI_CSI_CB_NO_ERR;
  dfs_svc_ptr = (qmi_dfsi_state_info_type*) service_handle;

  clnt_info_ptr = q_linear_search( &dfs_svc_ptr->client_q,
                                   dfsi_compare_connection_info_handle,
                                   connection_handle );
  if(NULL == clnt_info_ptr)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"dfsi_handle_req_cb: Invalid clnt 0x%p",
                  connection_handle);
    rc = QMI_CSI_CB_INTERNAL_ERR;
    return rc;
  }

  if(msg_id < (sizeof(req_handle_table) / sizeof(*req_handle_table)))
  {
    if(req_handle_table[msg_id])
    {
      DS_MSG2(MSG_LEGACY_MED,"Handling msg id %d for client 0x%p", 
                msg_id, clnt_info_ptr);
      rc = req_handle_table[msg_id] (clnt_info_ptr, req_handle, msg_id,
                                     req_c_struct, req_c_struct_len,
                                     service_handle);
    }
    else
    {
      DS_MSG1(MSG_LEGACY_ERROR,"dfsi_handle_req_cb: NULL message ID handler: %d",
                    msg_id);
      rc = QMI_CSI_CB_UNSUPPORTED_ERR;
    }
  }
  else
  {
    DS_MSG1(MSG_LEGACY_ERROR,"dfsi_handle_req_cb: Invalid message ID: %d", msg_id);
    rc = QMI_CSI_CB_UNSUPPORTED_ERR;
  }

  return rc;
} /* qmi_dfsi_handle_client_req_cb() */

/*===========================================================================
  FUNCTION QMI_DFS_BIND_CLIENT

  DESCRIPTION
    Bind the client to a given data port.

  PARAMETERS
    req_c_struct: Request Message with all TLV
    req_handle  : Request Handle
    clnt_info   : Client Info Handle

  RETURN VALUE
    QMI_CSI_CB_ERROR types.

  DEPENDENCIES
    qmi_dfs_init has to be called.

  SIDE EFFECTS
    None
===========================================================================*/

static qmi_csi_cb_error  qmi_dfsi_bind_client
(
  qmi_dfsi_client_info_type *clnt_info,
  qmi_req_handle            req_handle,
  int                       msg_id,
  void                      *req_c_struct,
  int                       req_c_struct_len,
  void                      *service_handle
)
{
  dfs_bind_client_req_msg_v01   *req_msg;
  dfs_bind_client_resp_msg_v01  resp_msg;
  uint32                        ep_id;
  uint8                         mux_id;
  dfs_ip_family_enum_v01        ip_preference;
  int                           errval = QMI_ERR_NONE_V01;
  boolean                       is_bound;
//------------------------------------------------------------------------------
  DS_ASSERT(req_c_struct && (req_c_struct_len > 0) && clnt_info && service_handle);
  memset(&resp_msg, 0, sizeof(dfs_bind_client_resp_msg_v01));

  ep_id  = clnt_info->ep_id;
  mux_id = clnt_info->mux_id;
  ip_preference = clnt_info->bound_ip_type;
  is_bound = clnt_info->is_already_bound;

  req_msg = (dfs_bind_client_req_msg_v01 *)req_c_struct;

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
      DS_MSG3(MSG_LEGACY_ERROR,"Invalid EP id 0x%x:0x%x for mux id %d", req_msg->ep_id.ep_type, req_msg->ep_id.iface_id, req_msg->mux_id);
	  errval = QMI_ERR_INVALID_ARG;
	  goto bail;
    }
    mux_id = (uint8)req_msg->mux_id;
  }
  if (req_msg->ip_preference_valid == TRUE)
  {
    if ((req_msg->ip_preference == DFS_IP_FAMILY_IPV4_V01)
       || (req_msg->ip_preference == DFS_IP_FAMILY_IPV6_V01))
    {
      ip_preference = req_msg->ip_preference;
    }
    else
    {
      DS_MSG1(MSG_LEGACY_MED,"Unknown IP Preference received %d",
                      req_msg->ip_preference);
      errval = QMI_ERR_INVALID_ARG;
      goto bail;
    }
  }

  if (req_msg->bind_subs_valid == TRUE)
  {
    if ( (req_msg->bind_subs < DFS_DEFAULT_SUBS_V01 || 
          req_msg->bind_subs > DFS_TERTIARY_SUBS_V01) &&
          req_msg->bind_subs != DFS_DONT_CARE_SUBS_V01 ) 
      {
        DS_MSG1(MSG_LEGACY_MED,"Unknown Subscription received %d",
                        req_msg->bind_subs);
        errval = QMI_ERR_INVALID_ARG;
        goto bail;
    }
  }
bail:
  if (errval == QMI_ERR_NONE_V01)
  {
    clnt_info->bound_ip_type = ip_preference;
    clnt_info->ep_id = ep_id;
    clnt_info->mux_id = mux_id;
    clnt_info->is_already_bound = is_bound;
    clnt_info->subscription =req_msg->bind_subs;
    DS_MSG3(MSG_LEGACY_MED,"The bound ep_id %d, mux_id %d, Ip_pref %d", ep_id, mux_id, ip_preference);
  }
  resp_msg.resp.result = (errval == QMI_ERR_NONE_V01) ? QMI_RESULT_SUCCESS_V01 : QMI_RESULT_FAILURE_V01;
  resp_msg.resp.error  = errval;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, sizeof(dfs_bind_client_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION QMI_DFS_GET_CLIENT_BINDING

  DESCRIPTION
    Get the Client Binding data (Client Ip_pref and Client SIO Data port)

  PARAMETERS
    req_c_struct: Request Message with all TLV
    req_handle  : Request Handle
    clnt_info   : Client Info Handle

  RETURN VALUE
    QMI_CSI_CB_ERROR types.

  DEPENDENCIES
    qmi_dfs_init has to be called.

  SIDE EFFECTS
    None
===========================================================================*/

static qmi_csi_cb_error  qmi_dfsi_get_client_binding
(
  qmi_dfsi_client_info_type *clnt_info,
  qmi_req_handle            req_handle,
  int                       msg_id,
  void                      *req_c_struct,
  int                       req_c_struct_len,
  void                      *service_handle
)
{
  dfs_get_client_binding_resp_msg_v01  resp_msg;
//------------------------------------------------------------------------------
  DS_ASSERT(req_c_struct && (req_c_struct_len > 0) && clnt_info && service_handle);
  memset(&resp_msg, 0, sizeof(dfs_get_client_binding_resp_msg_v01));
  resp_msg.resp.error = QMI_ERR_NONE_V01;
  resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
  if (clnt_info->is_already_bound)
  {
    resp_msg.bound_ep_id_valid = TRUE;
    resp_msg.bound_mux_id_valid = TRUE;
    resp_msg.bound_ep_id = qmi_internal_ep_id_to_qmi_ep_id(clnt_info->ep_id);
    resp_msg.bound_mux_id = clnt_info->mux_id;
  }
  resp_msg.ip_preference_valid = TRUE;
  resp_msg.ip_preference = clnt_info->bound_ip_type;
  resp_msg.bound_subs_valid = TRUE;
  resp_msg.bound_subs = ( dfs_bind_subscription_enum_v01 )clnt_info->subscription;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, sizeof(dfs_get_client_binding_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
}

/*==========================================================================
  FUNCTION qmi_dfsi_process_cmd()

  DESCRIPTION
    This function processes a QMI DFS command

    It is called by the QMI command handler and will dispatch the
    associated command/event handler function.

  PARAMETERS
    cmd_ptr:  private data buffer containing the QMI DFS command
              information.

  RETURN VALUE
    None

  DEPENDENCIES
    QMI DFS must already have been initialized

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_dfsi_process_cmd
(
  ds_command_enum_type  cmd,
  void              *cmd_ptr
)
{
}/* qmi_dfsi_process_cmd() */

/*===========================================================================
  FUNCTION QMI_DFSI_FILL_V4_FILTER_SPEC()

  DESCRIPTION
   fills filter_spec based on the masks set in filter rules.
    Values are filled based on valid parameters value.
    
  PARAMETERS
    filter_rules - filter rules sent from client
    fltr_spec - filter rules which will be passed to pdn

  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

static void qmi_dfsi_fill_v4_filter_spec
(
  dfs_filter_rule_type_v01* filter_rules,
  ip_filter_type*           fltr_spec
)
{
  DS_ASSERT(filter_rules && fltr_spec);
  
  if(filter_rules->ip_info.v4_info.valid_params == QMI_DFS_IPV4_FILTER_MASK_NONE_V01)
  {
    fltr_spec->ip_hdr.v4.field_mask = IPFLTR_MASK_IP4_NONE;	
  }
  else
  {
    if(filter_rules->ip_info.v4_info.valid_params & 
                      QMI_DFS_IPV4_FILTER_MASK_SRC_ADDR_V01)
    {
      fltr_spec->ip_hdr.v4.field_mask |= IPFLTR_MASK_IP4_SRC_ADDR;
      fltr_spec->ip_hdr.v4.src.addr.ps_s_addr = 
              ps_htonl(filter_rules->ip_info.v4_info.src_addr.ipv4_addr);
      fltr_spec->ip_hdr.v4.src.subnet_mask.ps_s_addr =
              ps_htonl(filter_rules->ip_info.v4_info.src_addr.subnet_mask);
      IPV4_ADDR_MSG(fltr_spec->ip_hdr.v4.src.addr.ps_s_addr);
      IPV4_ADDR_MSG(fltr_spec->ip_hdr.v4.src.subnet_mask.ps_s_addr);
    }
    if(filter_rules->ip_info.v4_info.valid_params & 
                      QMI_DFS_IPV4_FILTER_MASK_DEST_ADDR_V01)
    {
      fltr_spec->ip_hdr.v4.field_mask |= IPFLTR_MASK_IP4_DST_ADDR;
      fltr_spec->ip_hdr.v4.dst.addr.ps_s_addr = 
              ps_htonl(filter_rules->ip_info.v4_info.dest_addr.ipv4_addr);
      fltr_spec->ip_hdr.v4.dst.subnet_mask.ps_s_addr = 
              ps_htonl(filter_rules->ip_info.v4_info.dest_addr.subnet_mask);			  
      IPV4_ADDR_MSG(fltr_spec->ip_hdr.v4.dst.addr.ps_s_addr);
      IPV4_ADDR_MSG(fltr_spec->ip_hdr.v4.dst.subnet_mask.ps_s_addr);
    }
    if(filter_rules->ip_info.v4_info.valid_params & 
                      QMI_DFS_IPV4_FILTER_MASK_TOS_V01)
    {
      fltr_spec->ip_hdr.v4.field_mask |= IPFLTR_MASK_IP4_TOS;
      fltr_spec->ip_hdr.v4.tos.val     = filter_rules->ip_info.v4_info.tos.val;
      fltr_spec->ip_hdr.v4.tos.mask    = filter_rules->ip_info.v4_info.tos.mask;
      DS_MSG2(MSG_LEGACY_MED,"IP4  TOS value %d mask %d ",
                       fltr_spec->ip_hdr.v4.tos.val,
                       fltr_spec->ip_hdr.v4.tos.mask);
    }
  }
    if(filter_rules->xport_info.xport_protocol != DFS_PROTO_NONE_V01) 
    {
      fltr_spec->ip_hdr.v4.field_mask |= IPFLTR_MASK_IP4_NEXT_HDR_PROT;
      fltr_spec->ip_hdr.v4.next_hdr_prot = 
                 filter_rules->xport_info.xport_protocol;
      qmi_dfsi_fill_fltr_xport_info(&(filter_rules->xport_info),fltr_spec);
    }
  }

/*===========================================================================
  FUNCTION QMI_DFSI_FILL_V6_FILTER_SPEC()

  DESCRIPTION
   fills V6 filter_spec based on the masks set in filter rules.
   Values are filled based on valid parameters value.
    
  PARAMETERS
    filter_rules - filter rules sent from client
    fltr_spec - filter rules which will be passed to pdn

  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_dfsi_fill_v6_filter_spec
(
  dfs_filter_rule_type_v01* filter_rules,
  ip_filter_type*           fltr_spec
)
{
  DS_ASSERT(filter_rules && fltr_spec);

  if(filter_rules->ip_info.v6_info.valid_params == 
                          QMI_DFS_IPV6_FILTER_MASK_NONE_V01)
  {
    fltr_spec->ip_hdr.v6.field_mask = IPFLTR_MASK_IP6_NONE;		    
  }
  else
  {
    if(filter_rules->ip_info.v6_info.valid_params & 
                           QMI_DFS_IPV6_FILTER_MASK_SRC_ADDR_V01)
    {
      fltr_spec->ip_hdr.v6.field_mask |= IPFLTR_MASK_IP6_SRC_ADDR;
      memscpy(&fltr_spec->ip_hdr.v6.src.addr,sizeof(fltr_spec->ip_hdr.v6.src.addr),
    	&filter_rules->ip_info.v6_info.src_addr.ipv6_address,
    	sizeof(filter_rules->ip_info.v6_info.src_addr.ipv6_address));
      fltr_spec->ip_hdr.v6.src.prefix_len = 
         filter_rules->ip_info.v6_info.src_addr.prefix_len;
      IPV6_ADDR_MSG(fltr_spec->ip_hdr.v6.src.addr.in6_u.u6_addr64);
      DS_MSG1(MSG_LEGACY_MED,"IPV6 prefix len %d",fltr_spec->ip_hdr.v6.src.prefix_len );
    }
    if(filter_rules->ip_info.v6_info.valid_params & 
                            QMI_DFS_IPV6_FILTER_MASK_DEST_ADDR_V01)
    {
      fltr_spec->ip_hdr.v6.field_mask |= IPFLTR_MASK_IP6_DST_ADDR;
      memscpy(&fltr_spec->ip_hdr.v6.dst.addr,sizeof(fltr_spec->ip_hdr.v6.dst.addr),
         &filter_rules->ip_info.v6_info.dest_addr.ipv6_address,
    	 sizeof(filter_rules->ip_info.v6_info.dest_addr.ipv6_address));
      fltr_spec->ip_hdr.v6.dst.prefix_len = 
         filter_rules->ip_info.v6_info.dest_addr.prefix_len;			  
      IPV6_ADDR_MSG(fltr_spec->ip_hdr.v6.dst.addr.in6_u.u6_addr64);
      DS_MSG1(MSG_LEGACY_MED,"IPV6 prefix len %d",fltr_spec->ip_hdr.v6.dst.prefix_len );
    }
    if(filter_rules->ip_info.v6_info.valid_params & 
                            QMI_DFS_IPV6_FILTER_MASK_TRAFFIC_CLASS_V01)
    {
      fltr_spec->ip_hdr.v6.field_mask |= IPFLTR_MASK_IP6_TRAFFIC_CLASS;
      fltr_spec->ip_hdr.v6.trf_cls.val = 
         filter_rules->ip_info.v6_info.trf_cls.val;
      fltr_spec->ip_hdr.v6.trf_cls.mask = 
         filter_rules->ip_info.v6_info.trf_cls.mask;
      DS_MSG2(MSG_LEGACY_MED,"IPV6 trf class value %d mask %d",
                        fltr_spec->ip_hdr.v6.trf_cls.val,
                        fltr_spec->ip_hdr.v6.trf_cls.mask);
    }
    if(filter_rules->ip_info.v6_info.valid_params & 
                             QMI_DFS_IPV6_FILTER_MASK_FLOW_LABEL_V01)
    {
      fltr_spec->ip_hdr.v6.field_mask |= IPFLTR_MASK_IP6_FLOW_LABEL;
      fltr_spec->ip_hdr.v6.flow_label = 
          ps_htonl(filter_rules->ip_info.v6_info.flow_label);
    }
  }
  if(filter_rules->xport_info.xport_protocol != DFS_PROTO_NONE_V01) 
  {
    fltr_spec->ip_hdr.v6.field_mask |= IPFLTR_MASK_IP6_NEXT_HDR_PROT;
    fltr_spec->ip_hdr.v6.next_hdr_prot = 
       filter_rules->xport_info.xport_protocol;
    qmi_dfsi_fill_fltr_xport_info(&(filter_rules->xport_info),fltr_spec);
  }    			
}

/*===========================================================================
  FUNCTION QMI_DFSI_FILL_FLTR_XPORT_INFO()

  DESCRIPTION
   fills protocols xport info based on masks set in filter rules.
   Filterspec will be filled based on valid parameters in the xport_info.
    
  PARAMETERS
    xport_info - protocol inforamtion
    fltr_spec - filter rules which will be passed to pdn

  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_dfsi_fill_fltr_xport_info
(
  dfs_xport_header_type_v01* xport_info,
  ip_filter_type*            fltr_spec
)
{
  DS_ASSERT(xport_info && fltr_spec);
  
  switch( xport_info->xport_protocol )
  {
    case DFS_PROTO_ICMP_V01:
    case DFS_PROTO_ICMP6_V01:
        {
          if(xport_info->icmp_info.valid_params != 
                        QMI_DFS_ICMP_FILTER_MASK_NONE_V01)
          {
            if(xport_info->icmp_info.valid_params & 
                        QMI_DFS_ICMP_FILTER_MASK_MSG_TYPE_V01)
            {
              fltr_spec->next_prot_hdr.icmp.field_mask |= IPFLTR_MASK_ICMP_MSG_TYPE;
              fltr_spec->next_prot_hdr.icmp.type = xport_info->icmp_info.type;
            }
            if(xport_info->icmp_info.valid_params & 
                        QMI_DFS_ICMP_FILTER_MASK_MSG_CODE_V01)
            {
              fltr_spec->next_prot_hdr.icmp.field_mask |= IPFLTR_MASK_ICMP_MSG_CODE;
              fltr_spec->next_prot_hdr.icmp.code = xport_info->icmp_info.code;
            }
            DS_MSG4(MSG_LEGACY_MED,"protocol = %d ICMP valid params (%d) type (%d) code (%d)",
				      xport_info->xport_protocol,
				      xport_info->icmp_info.valid_params,
				      xport_info->icmp_info.type,
				      xport_info->icmp_info.code);
          }
    	}
        break;
    case DFS_PROTO_TCP_V01:
        {
          if(xport_info->tcp_info.valid_params != 
                         QMI_DFS_PORT_INFO_FILTER_MASK_NONE_V01)
          {
            DS_MSG2(MSG_LEGACY_MED,"protocol = %d tcp valid params (%d)",xport_info->xport_protocol,
				      xport_info->tcp_info.valid_params);
            if(xport_info->tcp_info.valid_params & 
                         QMI_DFS_PORT_INFO_FILTER_MASK_SRC_PORT_V01)
            {
              fltr_spec->next_prot_hdr.tcp.field_mask |= IPFLTR_MASK_TCP_SRC_PORT;
              fltr_spec->next_prot_hdr.tcp.src.port  = 
         			  ps_htons(xport_info->tcp_info.src_port_info.port);
              fltr_spec->next_prot_hdr.tcp.src.range = 
         			  xport_info->tcp_info.src_port_info.range;
              DS_MSG2(MSG_LEGACY_MED,"tcp src port (%d) range (%d)",
                               fltr_spec->next_prot_hdr.tcp.src.port,
                               fltr_spec->next_prot_hdr.tcp.src.range);
            }
            if(xport_info->tcp_info.valid_params & 
                         QMI_DFS_PORT_INFO_FILTER_MASK_DEST_PORT_V01)
            {
              fltr_spec->next_prot_hdr.tcp.field_mask |= IPFLTR_MASK_TCP_DST_PORT;
              fltr_spec->next_prot_hdr.tcp.dst.port  = 
                                  ps_htons(xport_info->tcp_info.dest_port_info.port);
              fltr_spec->next_prot_hdr.tcp.dst.range = 
                                  xport_info->tcp_info.dest_port_info.range;
               DS_MSG2(MSG_LEGACY_MED,"tcp dst port (%d) range (%d)",
                                fltr_spec->next_prot_hdr.tcp.dst.port,
                                fltr_spec->next_prot_hdr.tcp.dst.range);
            }
          }	
        }
        break;
    case DFS_PROTO_UDP_V01:
        {
          if(xport_info->udp_info.valid_params != 
                                QMI_DFS_PORT_INFO_FILTER_MASK_NONE_V01)
          {
            DS_MSG2(MSG_LEGACY_MED,"protocol = %d udp valid params (%d)",xport_info->xport_protocol,
                             xport_info->udp_info.valid_params);
            if(xport_info->udp_info.valid_params & 
                             QMI_DFS_PORT_INFO_FILTER_MASK_SRC_PORT_V01)
            {
              fltr_spec->next_prot_hdr.udp.field_mask |= 
                                   IPFLTR_MASK_UDP_SRC_PORT;
              fltr_spec->next_prot_hdr.udp.src.port  = 
            			   ps_htons(xport_info->udp_info.src_port_info.port);
              fltr_spec->next_prot_hdr.udp.src.range = 
            		       xport_info->udp_info.src_port_info.range;
              DS_MSG2(MSG_LEGACY_MED,"udp src port (%d) range (%d)",
                               fltr_spec->next_prot_hdr.udp.src.port,
                               fltr_spec->next_prot_hdr.udp.src.range);
            }
            if(xport_info->udp_info.valid_params &
                               QMI_DFS_PORT_INFO_FILTER_MASK_DEST_PORT_V01)
            {
              fltr_spec->next_prot_hdr.udp.field_mask |= IPFLTR_MASK_UDP_DST_PORT;
              fltr_spec->next_prot_hdr.udp.dst.port  = 
            				   ps_htons(xport_info->udp_info.dest_port_info.port);
              fltr_spec->next_prot_hdr.udp.dst.range = 
            			       xport_info->udp_info.dest_port_info.range;
              DS_MSG2(MSG_LEGACY_MED,"udp dst port (%d) range (%d)",
                               fltr_spec->next_prot_hdr.udp.dst.port,
                               fltr_spec->next_prot_hdr.udp.dst.range);
            }  
          }	
        }
    	break;
    case DFS_PROTO_ESP_V01:
        {
          if(xport_info->esp_info.valid_params != QMI_DFS_IPSEC_FILTER_MASK_NONE_V01)
          {
            if(xport_info->esp_info.valid_params & QMI_DFS_IPSEC_FILTER_MASK_SPI_V01)
            {
              fltr_spec->next_prot_hdr.esp.field_mask |= IPFLTR_MASK_ESP_SPI;
              fltr_spec->next_prot_hdr.esp.spi  = xport_info->esp_info.spi;
              DS_MSG2(MSG_LEGACY_MED,"protocol = %d esp spi (%d)",xport_info->xport_protocol,
			  	      fltr_spec->next_prot_hdr.esp.spi);			  
            }
          }
        }
        break;
    case DFS_PROTO_AH_V01:
        {
          if(xport_info->ah_info.valid_params != QMI_DFS_IPSEC_FILTER_MASK_NONE_V01)
          {
            if(xport_info->ah_info.valid_params & QMI_DFS_IPSEC_FILTER_MASK_SPI_V01)
            {
              fltr_spec->next_prot_hdr.ah.field_mask |= IPFLTR_MASK_ESP_SPI;
              fltr_spec->next_prot_hdr.ah.spi  = xport_info->ah_info.spi;
              DS_MSG2(MSG_LEGACY_MED,"protocol = %d ah spi (%d)",xport_info->xport_protocol,
			  	       fltr_spec->next_prot_hdr.ah.spi);			  
            }
          }
        }
        break;
    default:
        break;
  }
}

/*===========================================================================
  FUNCTION QMI_DFSI_FILL_FILTER_ERROR_MASK()

  DESCRIPTION
  This fills Filter mask to be reported in error case of add_powersave_filters_req.
  Error mask will be filled based on the error mask in filterspec which is updated 
  by pdn in case of validation failes.  

  PARAMETERS
   ip_filter_type - Filter rule 
   dfs_filter_param_error_mask_v01 - error mask to be filled for passed filter rule.
   
  RETURN VALUE
    boolean(true or false)

  DEPENDENCIES

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_dfsi_fill_filter_error_mask
(
  ip_filter_type* filter_spec,
  dfs_filter_param_error_mask_v01* error_mask
)
{
  uint8    next_hdr_prot = DFS_PROTO_NONE_V01;

  DS_ASSERT(filter_spec && error_mask);
  if(filter_spec->ip_vsn == IP_V4)
  {
    if(filter_spec->ip_hdr.v4.err_mask != IPFLTR_MASK_IP4_NONE)
    {
      if(filter_spec->ip_hdr.v4.err_mask & IPFLTR_MASK_IP4_SRC_ADDR)
      {
        *error_mask |= QMI_DFS_FILTER_PARAM_IPV4_SRC_ADDR_V01;
      }
      if(filter_spec->ip_hdr.v4.err_mask & IPFLTR_MASK_IP4_DST_ADDR)
      {
        *error_mask |= QMI_DFS_FILTER_PARAM_IPV4_DEST_ADDR_V01;
      }
      if(filter_spec->ip_hdr.v4.err_mask & IPFLTR_MASK_IP4_TOS)
      {
        *error_mask |= QMI_DFS_FILTER_PARAM_IPV4_TOS_V01;
      }  
      if(filter_spec->ip_hdr.v4.err_mask & IPFLTR_MASK_IP4_NEXT_HDR_PROT)
      {
        *error_mask |= QMI_DFS_FILTER_PARAM_XPORT_PROT_V01;
      }
    }
    next_hdr_prot = filter_spec->ip_hdr.v4.next_hdr_prot;
  }
  else if(filter_spec->ip_vsn == IP_V6)
  {
    if(filter_spec->ip_hdr.v6.err_mask != IPFLTR_MASK_IP6_NONE)
    {
      if(filter_spec->ip_hdr.v6.err_mask & IPFLTR_MASK_IP6_SRC_ADDR)
      {
        *error_mask |= QMI_DFS_FILTER_PARAM_IPV6_SRC_ADDR_V01;
      }
      if(filter_spec->ip_hdr.v6.err_mask & IPFLTR_MASK_IP6_DST_ADDR)
      {
        *error_mask |= QMI_DFS_FILTER_PARAM_IPV6_DEST_ADDR_V01;
      }
      if(filter_spec->ip_hdr.v6.err_mask & IPFLTR_MASK_IP6_TRAFFIC_CLASS)
      {
        *error_mask |= QMI_DFS_FILTER_PARAM_IPV6_TRF_CLS_V01;
      }	  
      if(filter_spec->ip_hdr.v6.err_mask & IPFLTR_MASK_IP6_FLOW_LABEL)
      {
        *error_mask |= QMI_DFS_FILTER_PARAM_IPV6_FLOW_LABEL_V01;
      }
    }
    next_hdr_prot = filter_spec->ip_hdr.v6.next_hdr_prot;;
  }
  else
  {
    *error_mask |= QMI_DFS_FILTER_PARAM_IP_VERSION_V01;
  }
  switch(next_hdr_prot)
  {
    case DFS_PROTO_ICMP_V01:
    case DFS_PROTO_ICMP6_V01:
    {
      if(filter_spec->next_prot_hdr.icmp.err_mask != IPFLTR_MASK_ICMP_NONE)
      {
        if(filter_spec->next_prot_hdr.icmp.err_mask & IPFLTR_MASK_ICMP_MSG_TYPE)
        {
          *error_mask |= QMI_DFS_FILTER_PARAM_ICMP_TYPE_V01;
        }
        if(filter_spec->next_prot_hdr.icmp.err_mask & IPFLTR_MASK_ICMP_MSG_CODE)
        {
          *error_mask |= QMI_DFS_FILTER_PARAM_ICMP_CODE_V01;
        }			  
      }
    }
    break;
    case DFS_PROTO_TCP_V01:
    {
      if(filter_spec->next_prot_hdr.tcp.err_mask != IPFLTR_MASK_TCP_NONE)
      {
        if(filter_spec->next_prot_hdr.tcp.err_mask & IPFLTR_MASK_TCP_SRC_PORT)
        {
          *error_mask |= QMI_DFS_FILTER_PARAM_TCP_SRC_PORT_V01;
        }
        if(filter_spec->next_prot_hdr.tcp.err_mask & IPFLTR_MASK_TCP_DST_PORT)
        {
          *error_mask |= QMI_DFS_FILTER_PARAM_TCP_DEST_PORT_V01;
        }			  
      }
    }
    break;
    case DFS_PROTO_UDP_V01:
    {
      if(filter_spec->next_prot_hdr.udp.err_mask != IPFLTR_MASK_UDP_NONE)
      {
        if(filter_spec->next_prot_hdr.udp.err_mask & IPFLTR_MASK_UDP_SRC_PORT)
        {
          *error_mask |= QMI_DFS_FILTER_PARAM_UDP_SRC_PORT_V01;
        }
        if(filter_spec->next_prot_hdr.udp.err_mask & IPFLTR_MASK_UDP_DST_PORT)
        {
          *error_mask |= QMI_DFS_FILTER_PARAM_UDP_DEST_PORT_V01;
        }			  
      }			
    }
    break;
    case DFS_PROTO_ESP_V01:
    {
      if(filter_spec->next_prot_hdr.esp.err_mask != IPFLTR_MASK_ESP_NONE)
      {
        if(filter_spec->next_prot_hdr.esp.err_mask & IPFLTR_MASK_ESP_SPI)
        {
          *error_mask |= QMI_DFS_FILTER_PARAM_ESP_SPI_V01;
        }
      }			
    }
    break;
    case DFS_PROTO_AH_V01:
    {
      if(filter_spec->next_prot_hdr.ah.err_mask != IPFLTR_MASK_AH_NONE)
      {
        if(filter_spec->next_prot_hdr.ah.err_mask & IPFLTR_MASK_AH_SPI)
        {
          *error_mask |= QMI_DFS_FILTER_PARAM_AH_SPI_V01;
        }
      }			
    }
    break;
    default:
      break;
  }
  DS_MSG1(MSG_LEGACY_MED,"error mask info (%d)",*error_mask);
}

/*===========================================================================
  FUNCTION QMI_DFSI_ADD_MO_EXCP_FILTERS()

  DESCRIPTION
    This function adds MO Exceptional filters and returns handles for each filter added.

  PARAMETERS
    req_c_struct: Request Message with all TLV
    req_handle  : Request Handle
    clnt_info   : Client Info Handle
    msg_id      : Message ID
    req_c_struct_len : length of request message
    service_handle :DFS service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    qmi_dfs_init has to be called.

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_dfsi_add_mo_excp_filters
(
  qmi_dfsi_client_info_type        *clnt_info,
  qmi_req_handle                    req_handle,
  int                               msg_id,
  void                             *req_c_struct,
  int                               req_c_struct_len,
  void                             *service_handle
)
{
  /*--------------declarations -------------------------------*/
  dfs_add_mo_exceptional_filters_req_msg_v01 *req_msg;
  dfs_add_mo_exceptional_filters_resp_msg_v01 resp_msg;
  ip_filter_type                             *fltr_spec = NULL;
  ds_ipfltr_add_param_type                    filter_param;
  ds_ipfltr_handle_type                       *ipfltr_handle_arr =NULL;  
  ds_ipfltr_handle_type                       tmp_ipfltr_handle;
  int8                                        cnt = 0;
  int16                                       ps_errno = 0;
  int16                                       error_no = 0;
  uint32                                      max_allowed = 0;
  uint32                                      used        = 0;
  rmnet_instance_e_type                       rmnet_instance = RMNET_INSTANCE_MAX;  
  ds_pdn_ioctl_mo_excp_usage_enum_type        mo_call_state;    
  ds_ipfltr_delete_param_type           delete_param;  
  boolean                               status = TRUE;
  /*-------------------------------------------------------*/

  DS_ASSERT(req_c_struct &&(req_c_struct_len > 0)&& clnt_info && service_handle);
  memset(&resp_msg, 0, sizeof(dfs_add_mo_exceptional_filters_resp_msg_v01));
  memset(&mo_call_state, 0, sizeof(ds_pdn_ioctl_mo_excp_usage_enum_type));
  req_msg = (dfs_add_mo_exceptional_filters_req_msg_v01*)req_c_struct;

  /*return if arguments are not passed*/
  if(req_msg->filter_rules_valid != TRUE)
  {
    resp_msg.resp.error = QMI_ERR_MISSING_ARG_V01;
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    goto bail;
  }
  QMI_DFSI_GET_RMNET_INSTANCE_FROM_EP_AND_MUX_ID(
                                        clnt_info->bound_ip_type,
                                        clnt_info->ep_id,
                                        clnt_info->mux_id,
                                        rmnet_instance);
  QMI_DFSI_IS_CALL_UP(rmnet_instance);
  
  if(ds_ipfltr_stats(DS_FWK_CLIENT_RMNET,rmnet_instance,
                     FLTR_CLIENT_MO_EXCP,
                     &max_allowed,&used,&ps_errno) != 0)
  {
    DS_MSG2(MSG_LEGACY_ERROR,"Getting Stats Failed max_allowed = (%d) used = (%d)",
                     max_allowed, used);
    resp_msg.resp.error  = QMI_ERR_INTERNAL_V01;
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    goto bail;
  }
                     
  ps_errno = 0;
  if(req_msg->filter_rules_len > 0 && 
     req_msg->filter_rules_len  <= ( max_allowed - used ))
  {
    memset(&filter_param, 0, sizeof(ds_ipfltr_add_param_type));  
    DS_SYSTEM_HEAP_MEM_ALLOC(fltr_spec, 
                 sizeof(ip_filter_type)*(req_msg->filter_rules_len),
                 ip_filter_type*);

     DS_SYSTEM_HEAP_MEM_ALLOC(ipfltr_handle_arr,
            sizeof(ds_ipfltr_handle_type)*(req_msg->filter_rules_len),
                   ds_ipfltr_handle_type*);

    if(fltr_spec == NULL ||ipfltr_handle_arr == NULL)
    {
      resp_msg.resp.error  = QMI_ERR_NO_MEMORY_V01;
      resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
      goto bail;
    }
    memset(fltr_spec, 0, 
           sizeof(ip_filter_type)*(req_msg->filter_rules_len));

	memset(ipfltr_handle_arr, 0, 
           sizeof(ds_ipfltr_handle_type)*(req_msg->filter_rules_len));

    /*fill filter spec with values from request struct*/
    //while(cnt < req_msg->filter_rules_len)
	for(cnt =0;cnt < req_msg->filter_rules_len;cnt++)
    {
      tmp_ipfltr_handle = DS_IPFLTR_INVALID_HANDLE;
      fltr_spec[cnt].ip_vsn = req_msg->filter_rules[cnt].ip_info.ip_version;
      if(req_msg->filter_rules[cnt].ip_info.ip_version == 
                                     DFS_IP_FAMILY_IPV6_V01)
      {
        qmi_dfsi_fill_v6_filter_spec(&(req_msg->filter_rules[cnt]),
                                     &fltr_spec[cnt]); 
      }
      else if(req_msg->filter_rules[cnt].ip_info.ip_version == 
                                     DFS_IP_FAMILY_IPV4_V01)
      {
        qmi_dfsi_fill_v4_filter_spec(&(req_msg->filter_rules[cnt]),
                                     &fltr_spec[cnt]); 
      }    
    filter_param.fi_ptr_arr         = &fltr_spec[cnt];
    //filter_param.num_filters        = (uint8)req_msg->filter_rules_len;
    filter_param.num_filters        = 1;
    filter_param.is_validated       = FALSE;
    filter_param.fltr_priority      = DS_IPFLTR_PRIORITY_DEFAULT;
    filter_param.fi_result          = (ds_ipfltr_result_type)TRUE;
    filter_param.client_type        = DS_FWK_CLIENT_RMNET;
    filter_param.client_handle      = rmnet_instance;
    filter_param.client_id          = FLTR_CLIENT_MO_EXCP;
   
    /* call ds fwk API to add filters - ipfltr_handle_arr contains handles for all filters added*/
	tmp_ipfltr_handle = ds_ipfltr_add(&filter_param,&ps_errno);
	if(tmp_ipfltr_handle == DS_IPFLTR_INVALID_HANDLE)
	{
	  status = FALSE;
	  break;
	}
	else
	{
	   ipfltr_handle_arr[cnt] = tmp_ipfltr_handle;
	}
  }
  if(status != FALSE)
  {  
    DS_MSG0(MSG_LEGACY_MED,"qmi_dfsi_add_mo_excp_filters success");
    resp_msg.filter_handles_valid = TRUE;
    resp_msg.filter_handles_len = req_msg->filter_rules_len;
    memscpy(&(resp_msg.filter_handles),
              sizeof(uint32)*(req_msg->filter_rules_len),
              ipfltr_handle_arr,
              sizeof(uint32)*(req_msg->filter_rules_len));
    resp_msg.resp.error = QMI_ERR_NONE_V01;
    resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;

      //call ds_pdn_ioctl to set APN rate control if first time adding filter
    if(used == 0)
    {
      mo_call_state = DS_PDN_IOCTL_MO_EXCP_IN_USE;
      if (ds_pdn_ioctl(DS_FWK_CLIENT_RMNET, rmnet_instance,DS_PDN_IOCTL_3GPP_MO_EXCP_IN_USE,
                       &mo_call_state, &error_no) != 0)
      {
          DS_MSG1(MSG_LEGACY_ERROR,"DS_PDN_IOCTL_3GPP_MO_EXCP_IN_USE failed with error_no (%d)",
                                                            error_no);
      }
    }
  }
  else
  {
     cnt -= 1;
      for(;cnt >= 0; cnt--)
      {
       delete_param.client_handle = rmnet_instance;
	   delete_param.client_id    =  FLTR_CLIENT_MO_EXCP;
	   delete_param.client_type = DS_FWK_CLIENT_RMNET;
       delete_param.fi_handle   = ipfltr_handle_arr[cnt];   
       ds_ipfltr_delete(&delete_param,
                        &ps_errno);
      }
      cnt = 0;
      DS_MSG1(MSG_LEGACY_ERROR,"qmi_dfsi_add_mo_excp_filters failed with ps_errno (%d)",
                                                              ps_errno);
      /*if ps_errno is EINVAL,we need to fill error mask and report in response*/
      if(ps_errno == DS_EINVAL)
      {
        resp_msg.filter_rule_error_valid = TRUE;
        resp_msg.filter_rule_error_len = req_msg->filter_rules_len;
        while(cnt < req_msg->filter_rules_len)
        {
          qmi_dfsi_fill_filter_error_mask(&(fltr_spec[cnt]),
                                          &(resp_msg.filter_rule_error[cnt]));
          cnt++;
        }
        resp_msg.resp.error  = QMI_ERR_INVALID_ARG_V01;
        resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
      }
      else
      {
        resp_msg.resp.error  = QMI_ERR_INTERNAL_V01;
        resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
      }
    }		      
    
  }
  else
  {
    resp_msg.resp.error = QMI_ERR_REQUESTED_NUM_UNSUPPORTED_V01;
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    goto bail;
  }
  
bail:
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(dfs_add_mo_exceptional_filters_resp_msg_v01));

	DS_SYSTEM_HEAP_MEM_FREE(fltr_spec);
   if(ipfltr_handle_arr != NULL)
  {
    DS_SYSTEM_HEAP_MEM_FREE(ipfltr_handle_arr);
   }
  return QMI_CSI_CB_NO_ERR;
}/* qmi_dfsi_add_mo_excp_filters */

/*===========================================================================
  FUNCTION QMI_DFSI_REMOVE_MO_EXCP_FILTERS()

  DESCRIPTION
    To remove MO Exceptional filters for passed handles .

  PARAMETERS
    req_c_struct       :       Request Message with all TLV
    req_handle         :       Request Handle
    clnt_info          :       Client Info Handle
    msg_id             :       Message ID
    req_c_struct_len   :       length of request message
    service_handle     :       DFS service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    qmi_dfs_init has to be called.

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_dfsi_remove_mo_excp_filters
(
  qmi_dfsi_client_info_type        *clnt_info,
  qmi_req_handle                    req_handle,
  int                               msg_id,
  void                             *req_c_struct,
  int                               req_c_struct_len,
  void                             *service_handle
)
{
  /*--------------declarations ----------------------------------------*/
  dfs_remove_mo_exceptional_filters_req_msg_v01    *req_msg;  
  dfs_remove_mo_exceptional_filters_resp_msg_v01    resp_msg;
  uint32                                            filter_count = 0;
  int16                                             ps_errno;
  rmnet_instance_e_type                             rmnet_instance = RMNET_INSTANCE_MAX;
  ds_pdn_ioctl_mo_excp_usage_enum_type            mo_call_state;
  uint32                                            used        = 0;
  uint32                                            max_allowed = 0;
  int16                                             error_no    = 0;
  ds_ipfltr_delete_param_type                       delete_param;
  /*-----------------------------------------------------------------*/
  
  DS_ASSERT(req_c_struct &&(req_c_struct_len > 0)&& clnt_info && service_handle);
  memset(&resp_msg, 0, sizeof(dfs_remove_mo_exceptional_filters_resp_msg_v01));
  memset(&mo_call_state, 0, sizeof(ds_pdn_ioctl_mo_excp_usage_enum_type));
  req_msg = (dfs_remove_mo_exceptional_filters_req_msg_v01*)req_c_struct;

  QMI_DFSI_GET_RMNET_INSTANCE_FROM_EP_AND_MUX_ID(
                                        clnt_info->bound_ip_type,
                                        clnt_info->ep_id,
                                        clnt_info->mux_id,
                                        rmnet_instance);
  QMI_DFSI_IS_CALL_UP(rmnet_instance);

  if(req_msg->filter_handles_len >= QMI_DFS_MAX_FILTERS_V01)
  {
    resp_msg.resp.error = QMI_ERR_INTERNAL_V01;
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    goto bail;
  }

  delete_param.client_type = DS_FWK_CLIENT_RMNET;
  delete_param.client_handle = rmnet_instance;
  delete_param.client_id     = FLTR_CLIENT_MO_EXCP;

  /* Validate all filter handles before passing to pdn*/
  while(filter_count < req_msg->filter_handles_len)
  {
    if(req_msg->filter_handles[filter_count] == DS_IPFLTR_INVALID_HANDLE)
    {
      resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
      resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
      goto bail;
    }
    delete_param.fi_handle    = req_msg->filter_handles[filter_count];
    if(ds_ipfltr_delete(&delete_param,
                      &ps_errno) != 0)
    {
      resp_msg.resp.error = QMI_ERR_INTERNAL_V01;
      resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
      goto bail;
    }
    filter_count++;
  }

  if(ds_ipfltr_stats(DS_FWK_CLIENT_RMNET,rmnet_instance,
                     FLTR_CLIENT_MO_EXCP,
                     &max_allowed,&used,&error_no) == 0)
  {
  
  //call ds_pdn_ioctl to set APN rate control if all filters are deleted
  if(used == 0)
  {
      mo_call_state = DS_PDN_IOCTL_MO_EXCP_NOT_IN_USE;
      if (ds_pdn_ioctl(DS_FWK_CLIENT_RMNET, rmnet_instance,DS_PDN_IOCTL_3GPP_MO_EXCP_IN_USE,
                       &mo_call_state, &error_no) != 0)
      {
          DS_MSG1(MSG_LEGACY_ERROR,"DS_PDN_IOCTL_3GPP_MO_EXCP_IN_USE failed with error_no (%d)",
                                                            error_no);
      }
    }
  }
  resp_msg.resp.error = QMI_ERR_NONE_V01;
  resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
bail:
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(dfs_remove_mo_exceptional_filters_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
}/* qmi_dfsi_remove_mo_excp_filters */


/*===========================================================================
  FUNCTION QMI_DFSI_ADD_PDN_SHARING_FILTERS()

  DESCRIPTION
    This function adds PDN sharing filters and returns handles for each filter added.

  PARAMETERS
    req_c_struct: Request Message with all TLV
    req_handle  : Request Handle
    clnt_info   : Client Info Handle
    msg_id      : Message ID
    req_c_struct_len : length of request message
    service_handle :DFS service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    qmi_dfs_init has to be called.

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_dfsi_add_pdn_sharing_filters
(
  qmi_dfsi_client_info_type        *clnt_info,
  qmi_req_handle                    req_handle,
  int                               msg_id,
  void                             *req_c_struct,
  int                               req_c_struct_len,
  void                             *service_handle
)
{
  /*--------------declarations -------------------------------*/
  dfs_add_pdn_sharing_filters_req_msg_v01   *req_msg = NULL;
  dfs_add_pdn_sharing_filters_resp_msg_v01   resp_msg;
  ip_filter_type                            *fltr_spec = NULL;
  ds_ipfltr_add_param_type                   filter_param;
  ds_ipfltr_handle_type                     *ipfltr_handle_arr =NULL;  
  ds_ipfltr_handle_type                      tmp_ipfltr_handle;
  int8                                       cnt = 0;
  int16                                      ps_errno = 0;
  uint32                                     max_allowed = 0;
  uint32                                     used        = 0;
  rmnet_instance_e_type                      rmnet_instance = RMNET_INSTANCE_MAX;  
  ds_ipfltr_delete_param_type                delete_param;  
  boolean                                    status = TRUE;
  /*-------------------------------------------------------*/

  DS_ASSERT(req_c_struct &&(req_c_struct_len > 0)&& clnt_info && service_handle);
  memset(&resp_msg, 0, sizeof(dfs_add_pdn_sharing_filters_resp_msg_v01));
  req_msg = (dfs_add_pdn_sharing_filters_req_msg_v01*)req_c_struct;

  /*return if arguments are not passed*/
  if(req_msg->filter_rules_valid != TRUE)
  {
    resp_msg.resp.error = QMI_ERR_MISSING_ARG_V01;
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    goto bail;
  }
  QMI_DFSI_GET_RMNET_INSTANCE_FROM_EP_AND_MUX_ID(
                                        clnt_info->bound_ip_type,
                                        clnt_info->ep_id,
                                        clnt_info->mux_id,
                                        rmnet_instance);
  QMI_DFSI_IS_CALL_UP(rmnet_instance);
  
  if(ds_ipfltr_stats(DS_FWK_CLIENT_RMNET,rmnet_instance,
                     FLTR_CLIENT_PDN_SHARING,
                     &max_allowed,&used,&ps_errno) != 0)
  {
    DS_MSG2(MSG_LEGACY_ERROR,"Getting Stats Failed max_allowed = (%d) used = (%d)",
            max_allowed, used);
    resp_msg.resp.error  = QMI_ERR_INTERNAL_V01;
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    goto bail;
  }
                     
  ps_errno = 0;
  if(req_msg->filter_rules_len > 0 && 
     req_msg->filter_rules_len  <= ( max_allowed - used ))
  {
    memset(&filter_param, 0, sizeof(ds_ipfltr_add_param_type));  
    DS_SYSTEM_HEAP_MEM_ALLOC(fltr_spec, 
                 sizeof(ip_filter_type)*(req_msg->filter_rules_len),
                 ip_filter_type*);

     DS_SYSTEM_HEAP_MEM_ALLOC(ipfltr_handle_arr,
                              sizeof(ds_ipfltr_handle_type)*\
                              (req_msg->filter_rules_len),
                              ds_ipfltr_handle_type*);

    if(fltr_spec == NULL ||ipfltr_handle_arr == NULL)
    {
      resp_msg.resp.error  = QMI_ERR_NO_MEMORY_V01;
      resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
      goto bail;
    }
    memset(fltr_spec, 0, 
           sizeof(ip_filter_type)*(req_msg->filter_rules_len));

	memset(ipfltr_handle_arr, 0, 
           sizeof(ds_ipfltr_handle_type)*(req_msg->filter_rules_len));

    /*fill filter spec with values from request struct*/
	for(cnt =0;cnt < req_msg->filter_rules_len;cnt++)
    {
      tmp_ipfltr_handle = DS_IPFLTR_INVALID_HANDLE;
      fltr_spec[cnt].ip_vsn = req_msg->filter_rules[cnt].ip_info.ip_version;
      if(req_msg->filter_rules[cnt].ip_info.ip_version == 
                                     DFS_IP_FAMILY_IPV6_V01)
      {
        qmi_dfsi_fill_v6_filter_spec(&(req_msg->filter_rules[cnt]),
                                     &fltr_spec[cnt]); 
      }
      else if(req_msg->filter_rules[cnt].ip_info.ip_version == 
                                     DFS_IP_FAMILY_IPV4_V01)
      {
        qmi_dfsi_fill_v4_filter_spec(&(req_msg->filter_rules[cnt]),
                                     &fltr_spec[cnt]); 
      }    
      filter_param.fi_ptr_arr         = &fltr_spec[cnt];
      filter_param.num_filters        = 1;
      filter_param.is_validated       = FALSE;
      filter_param.fltr_priority      = DS_IPFLTR_PRIORITY_DEFAULT;
      filter_param.fi_result          = (ds_ipfltr_result_type)TRUE;
      filter_param.client_type        = DS_FWK_CLIENT_RMNET;
      filter_param.client_handle      = rmnet_instance;
      filter_param.client_id          = FLTR_CLIENT_PDN_SHARING;
   
      /* call ds fwk API to add filters - ipfltr_handle_arr contains handles for all filters added*/
	  tmp_ipfltr_handle = ds_ipfltr_add(&filter_param,&ps_errno);
      if(tmp_ipfltr_handle == DS_IPFLTR_INVALID_HANDLE)
      {
        status = FALSE;
        break;
      }
      else
      {
	    ipfltr_handle_arr[cnt] = tmp_ipfltr_handle; 
      }
    }
    if(status != FALSE)
    {  
      DS_MSG0(MSG_LEGACY_ERROR,"qmi_dfsi_add_pdn_sharing_filters success");
      resp_msg.filter_handles_valid = TRUE;
      resp_msg.filter_handles_len = req_msg->filter_rules_len;
      memscpy(&(resp_msg.filter_handles),
                sizeof(uint32)*(req_msg->filter_rules_len),
                ipfltr_handle_arr,
                sizeof(uint32)*(req_msg->filter_rules_len));
      resp_msg.resp.error = QMI_ERR_NONE_V01;
      resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
    }
    else
    {
       cnt -= 1;
       for(;cnt >= 0; cnt--)
       {
         delete_param.client_handle = rmnet_instance;
	     delete_param.client_id = FLTR_CLIENT_PDN_SHARING;
	     delete_param.client_type = DS_FWK_CLIENT_RMNET;
         delete_param.fi_handle = ipfltr_handle_arr[cnt];   
         ds_ipfltr_delete(&delete_param,
                          &ps_errno);
       }
       cnt = 0;
       DS_MSG1(MSG_LEGACY_ERROR,"qmi_dfsi_add_pdn_sharing_filters failed with ps_errno (%d)",
               ps_errno);
       /*if ps_errno is EINVAL,we need to fill error mask and report in response*/
       if(ps_errno == DS_EINVAL)
       {
         resp_msg.filter_rule_error_valid = TRUE;
         resp_msg.filter_rule_error_len = req_msg->filter_rules_len;
         while(cnt < req_msg->filter_rules_len)
         {
           qmi_dfsi_fill_filter_error_mask(&(fltr_spec[cnt]),
                                           &(resp_msg.filter_rule_error[cnt]));
           cnt++;
         }
         resp_msg.resp.error  = QMI_ERR_INVALID_ARG_V01;
         resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
       }
       else
       {
         resp_msg.resp.error  = QMI_ERR_INTERNAL_V01;
         resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
       }
     }		          
   }
   else
   {
     resp_msg.resp.error = QMI_ERR_REQUESTED_NUM_UNSUPPORTED_V01;
     resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
     goto bail;
   }
  
bail:
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(dfs_add_pdn_sharing_filters_resp_msg_v01));
  DS_SYSTEM_HEAP_MEM_FREE(fltr_spec);
  DS_SYSTEM_HEAP_MEM_FREE(ipfltr_handle_arr);
  return QMI_CSI_CB_NO_ERR;
}/* qmi_dfsi_add_mo_excp_filters */


/*===========================================================================
  FUNCTION QMI_DFSI_REMOVE_PDN_SHARING_FILTERS()

  DESCRIPTION
    To remove PDN sharing filters for passed handles .

  PARAMETERS
    req_c_struct       :       Request Message with all TLV
    req_handle         :       Request Handle
    clnt_info          :       Client Info Handle
    msg_id             :       Message ID
    req_c_struct_len   :       length of request message
    service_handle     :       DFS service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    qmi_dfs_init has to be called.

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_dfsi_remove_pdn_sharing_filters
(
  qmi_dfsi_client_info_type        *clnt_info,
  qmi_req_handle                    req_handle,
  int                               msg_id,
  void                             *req_c_struct,
  int                               req_c_struct_len,
  void                             *service_handle
)
{
  /*--------------declarations ----------------------------------------*/
  dfs_remove_filters_req_msg_v01                   *req_msg = NULL;  
  dfs_remove_filters_resp_msg_v01                   resp_msg;
  uint32                                            filter_count = 0;
  int16                                             ps_errno;
  rmnet_instance_e_type                             rmnet_instance = RMNET_INSTANCE_MAX;
  uint32                                            used        = 0;
  uint32                                            max_allowed = 0;
  int16                                             error_no    = 0;
  ds_ipfltr_delete_param_type                       delete_param;
  /*-----------------------------------------------------------------*/
  
  DS_ASSERT(req_c_struct &&(req_c_struct_len > 0)&& clnt_info && service_handle);
  memset(&resp_msg, 0, sizeof(dfs_remove_filters_resp_msg_v01));
  req_msg = (dfs_remove_filters_req_msg_v01*)req_c_struct;

  QMI_DFSI_GET_RMNET_INSTANCE_FROM_EP_AND_MUX_ID(
                                        clnt_info->bound_ip_type,
                                        clnt_info->ep_id,
                                        clnt_info->mux_id,
                                        rmnet_instance);
  QMI_DFSI_IS_CALL_UP(rmnet_instance);

  if(req_msg->filter_handles_len >= QMI_DFS_MAX_FILTERS_V01)
  {
    resp_msg.resp.error = QMI_ERR_INTERNAL_V01;
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    goto bail;
  }

  delete_param.client_type = DS_FWK_CLIENT_RMNET;
  delete_param.client_handle = rmnet_instance;
  delete_param.client_id     = FLTR_CLIENT_PDN_SHARING;

  /* Validate all filter handles before passing to pdn*/
  while(filter_count < req_msg->filter_handles_len)
  {
    if(req_msg->filter_handles[filter_count] == DS_IPFLTR_INVALID_HANDLE)
    {
      resp_msg.resp.error = QMI_ERR_INVALID_HANDLE_V01;
      resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
      goto bail;
    }
    delete_param.fi_handle    = req_msg->filter_handles[filter_count];
    if(ds_ipfltr_delete(&delete_param,
                        &ps_errno) != 0)
    {
      resp_msg.resp.error = QMI_ERR_INTERNAL_V01;
      resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
      goto bail;
    }
    filter_count++;
  }

  if(ds_ipfltr_stats(DS_FWK_CLIENT_RMNET,rmnet_instance,
                     FLTR_CLIENT_PDN_SHARING,
                     &max_allowed,&used,&error_no) == 0)
  {
     DS_MSG1(MSG_LEGACY_ERROR,"qmi_dfsi_remove_pdn_sharing_filters failed with"
             "error_no (%d)", error_no);
  }
  resp_msg.resp.error = QMI_ERR_NONE_V01;
  resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
  
bail:
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(dfs_remove_filters_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
}/* qmi_dfsi_remove_mo_excp_filters */

