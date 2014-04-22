/*===========================================================================

                         D S _ Q M I _W D S . C

DESCRIPTION

 The Data Services Qualcomm Wireless Data Services MSM Interface source file

EXTERNALIZED FUNCTIONS

  qmi_wds_init()
    Initialize the QMI wds service

Copyright (c) 2017-2020 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

              EDIT HISTORY FOR FILE

  $Header: 
  $DateTime: 2023/04/19 01:09:38 $ 

when        who    what, where, why 
--------    ---    ----------------------------------------------------------
08/11/20    psahithi    Added support for adding msisdn flag TLV in modify_profile_lite.
5/17/20     mkumarsa    apn bearer converted to proper value for WDS_MASK_APN_BEARER_ANY_V01 
12/29/17    mkumarsa    First version of the WDS module
===========================================================================*/

/*===========================================================================
                          INCLUDE FILES FOR MODULE
                          
===========================================================================*/

#include "ds_headers.h"
#include "qmi_csi.h"
#include "qmi_csi_target_ext.h"
#include "mqcsi_log.h"
#include "ds_qmi_wds.h"
#include "wireless_data_service_v01.h"
#include "qmi_sap.h"
#include "ds_qmi_defs.h"
#include "qmi_framework.h"
#include "ds_qmi_if.h"
#include "ds_rmnet_meta_sm.h"
#include "ds_rmnet_xport.h"
#include "ds_profile.h"
#include "ds_pdn_manager.h"
#include "ps_addri_mgmt.h"
#include "ps_addr_v6.h"
#include "ds_rmnet_smi.h"
#include "ds_pdn_cp_data_hdlr.h"
#include "ps_stat_logging.h"
#include "ds_pdn_apn_rate_ctrl.h"
#include "ds_fwki.h"


/*---------------------------------------------------------------------------
  Service configuration
- WDS service version is 1.0
---------------------------------------------------------------------------*/
#define WDSI_BASE_VER_MAJOR  (1)
#define WDSI_BASE_VER_MINOR  (1)

#define WDSI_ADDENDUM_VER_MAJOR  (0)
#define WDSI_ADDENDUM_VER_MINOR  (0)

#define WDSI_MAX_CLIDS    (QMI_FRAMEWORK_SVC_MAX_CLIENTS - 1)
#define WDSI_QCSI_OS_SIGNAL ((rex_sigs_type)(1 << DS_WDS_RDR_SIGNAL))

/*---------------------------------------------------------------------------
  QMI wds Client state info
---------------------------------------------------------------------------*/
typedef struct qmi_wdsi_client_state_s
{
    /*< Data Structure required by the queue 
         library to add this as part of a queue */
  q_link_type                            link;  
  qmi_client_handle                      clnt;
  qmi_req_handle                         req_handle;
  struct qmi_wdsi_client_state_s        *next;        // Link to next client
  struct qmi_wdsi_client_state_s        *sni_pending_next;
  int8                                   wds_inst;   // Current  wds_inst
  qmi_ip_family_e_type                   bound_ip_type;  // Current ip type
  boolean                                net_bound;      // Net bound
}qmi_wdsi_client_info_type;

/*---------------------------------------------------------------------------
  QMI wds service state info
---------------------------------------------------------------------------*/
typedef struct
{
  /*< Information about all the clients that have connected to the
      service. This is queue     of type qmi_wdsi_client_info_type */
  q_type                 client_q; 
  qmi_csi_service_handle service_handle;
  qmi_sap_client_handle  sap_client_handle;
  uint16                 num_of_clients;
} qmi_wdsi_state_info_type;

static qmi_wdsi_state_info_type      qmi_wds_state;
static qmi_csi_os_params             os_params;
static boolean                       qmi_wdsi_inited = FALSE;

#define WDS_MAX_INSTANCE  15
typedef struct
{
  qmi_wdsi_client_info_type               *client_list;
  wds_connection_status_enum_v01           pkt_svc_state;
  qmi_wds_call_end_reason_e_type           call_end_reason;
  uint32                                   call_end_reason_verbose;  
  qmi_ip_family_e_type                     ip_call_type;
  qmi_wdsi_client_info_type               *stop_nw_client;
  qmi_wdsi_client_info_type               *sni_pending_list;
  uint64                                   gateway_iid;
  struct
  {
    uint32                           ep_id;
    uint8                            mux_id;
    rmnet_instance_e_type            rmnet_inst;
  } binding;
}qmi_wds_client_state_map;

static qmi_wds_client_state_map *qmi_wds_cl_sp_map[WDS_MAX_INSTANCE];

/*===========================================================================

                               INTERNAL DATA

===========================================================================*/
/*---------------------------------------------------------------------------
  QMI service command handlers
  forward declarations & cmd handler dispatch table definition
---------------------------------------------------------------------------*/

/*===========================================================================
  FUNCTION QMI_WDSI_START_NETWORK_INTERFACE()

  DESCRIPTION
   Starts a data call and posts command to Rmnet

  PARAMETERS
     req_c_struct  :            Request Message with all TLV
     req_handle  :              Request Handle
     clnt_info   :              Client Info Handle
     msg_id             :       Message ID
     req_c_struct_len   :       length of request message
     service_handle     :       wds service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

static qmi_csi_cb_error qmi_wdsi_start_network_interface
(
  qmi_wdsi_client_info_type *clnt_info,
  qmi_req_handle             req_handle,
  int                        msg_id,
  void                      *req_c_struct,
  int                        req_c_struct_len,
  void                      *service_handle
);

/*===========================================================================
  FUNCTION QMI_WDSI_STOP_NETWORK_INTERFACE()

  DESCRIPTION
   Stops a data call and posts command to Rmnet

  PARAMETERS
     req_c_struct  :            Request Message with all TLV
     req_handle  :              Request Handle
     clnt_info   :              Client Info Handle
     msg_id             :       Message ID
     req_c_struct_len   :       length of request message
     service_handle     :       wds service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

static qmi_csi_cb_error  qmi_wdsi_stop_network_interface
(
  qmi_wdsi_client_info_type        *clnt_info,
  qmi_req_handle                    req_handle,
  int                               msg_id,
  void                             *req_c_struct,
  int                               req_c_struct_len,
  void                             *service_handle
);

/*===========================================================================
  FUNCTION QMI_WDSI_GET_PKT_SRVC_STATUS()

  DESCRIPTION
   This message is been used to fetch the current wds connection state
   
  PARAMETERS
     req_c_struct  :            Request Message with all TLV
     req_handle  :              Request Handle
     clnt_info   :              Client Info Handle
     msg_id             :       Message ID
     req_c_struct_len   :       length of request message
     service_handle     :       wds service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_wdsi_get_pkt_srvc_status
(
  qmi_wdsi_client_info_type        *clnt_info,
  qmi_req_handle                    req_handle,
  int                               msg_id,
  void                             *req_c_struct,
  int                               req_c_struct_len,
  void                             *service_handle

);

/*===========================================================================
  FUNCTION QMI_WDSI_SEND_EVENT_REPORT_IND_MSG()

  DESCRIPTION
    This api is been used to post indication to clients about the packet 
    statistics when the call is going down.
    
  PARAMETERS
     wds_sp  :                  Wds state 
     clnt_info  :               Client info 

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_wdsi_send_event_report_ind_msg
( 
  uint32                                      event_mask,
  ds_stats_type                              *stats,
  qmi_wdsi_client_info_type                  *clnt_info
);

/*===========================================================================
  FUNCTION QMI_WDSI_PKT_SRVC_STATUS_IND_MSG()

  DESCRIPTION
    This api is been used to post indication to clients about the connection 
    state and ip parameters also if the state is connected.
    
  PARAMETERS
     wds_sp  :                  Wds state 
     clnt_info  :               Client info 
     pkt_svc_state   :          Packet state
     reconfig_flag       :      Reconfig flag
     changed_config   :         Changde mask

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_wdsi_pkt_srvc_status_ind_msg
(
  qmi_wds_client_state_map                  *wds_sp,  
  qmi_wdsi_client_info_type                 *clnt_info,  
  wds_connection_status_enum_v01             pkt_svc_state,  
  boolean                                    reconfig_flag,
  ds_ioctl_extended_ip_config_ex_type        changed_config
);

/*===========================================================================
  FUNCTION QMI_WDSI_GET_PROFILE_SETTINGS_LITE()

  DESCRIPTION
    Gets the profile settings based on the APN and IP type passed in req
    
  PARAMETERS
     req_c_struct  :            Request Message with all TLV
     req_handle  :              Request Handle
     clnt_info   :              Client Info Handle
     msg_id             :       Message ID
     req_c_struct_len   :       length of request message
     service_handle     :       wds service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_wdsi_get_profile_settings_lite
(
  qmi_wdsi_client_info_type        *clnt_info,
  qmi_req_handle                    req_handle,
  int                               msg_id,
  void                             *req_c_struct,
  int                               req_c_struct_len,
  void                             *service_handle
); 

/*===========================================================================
  FUNCTION QMI_WDSI_ABORT())

  DESCRIPTION
    Aborts an ongoing data session
    
  PARAMETERS

     req_c_struct  :            Request Message with all TLV
     req_handle  :              Request Handle
     clnt_info   :              Client Info Handle
     msg_id             :       Message ID
     req_c_struct_len   :       length of request message
     service_handle     :       wds service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_wdsi_abort
(
  qmi_wdsi_client_info_type        *clnt_info,
  qmi_req_handle                    req_handle,
  int                               msg_id,
  void                             *req_c_struct,
  int                               req_c_struct_len,
  void                             *service_handle
);

/*===========================================================================
  FUNCTION QMI_WDSI_GET_AVAIL_EXTERNAL_IPV6_ADDR()

  DESCRIPTION

    Gets all the extrenal ipv6 address allocated for a data call
    
  PARAMETERS
     req_c_struct  :            Request Message with all TLV
     req_handle  :              Request Handle
     clnt_info   :              Client Info Handle
     msg_id             :       Message ID
     req_c_struct_len   :       length of request message
     service_handle     :       wds service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_wdsi_get_avail_external_ipv6_addr
(
  qmi_wdsi_client_info_type        *clnt_info,
  qmi_req_handle                    req_handle,
  int                               msg_id,
  void                             *req_c_struct,
  int                               req_c_struct_len,
  void                             *service_handle
);

/*===========================================================================
  FUNCTION QMI_WDSI_CREATE_PROFILE_LITE()

  DESCRIPTION
     Creates a data profile which is used for calls
     
  PARAMETERS
     req_c_struct  :            Request Message with all TLV
     req_handle  :              Request Handle
     clnt_info   :              Client Info Handle
     msg_id             :       Message ID
     req_c_struct_len   :       length of request message
     service_handle     :       wds service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_wdsi_create_profile_lite
(
  qmi_wdsi_client_info_type        *clnt_info,
  qmi_req_handle                    req_handle,
  int                               msg_id,
  void                             *req_c_struct,
  int                               req_c_struct_len,
  void                             *service_handle
);

/*===========================================================================
  FUNCTION QMI_WDSI_MODIFY_PROFILE_SETTINGS_LITE()

  DESCRIPTION
   Modifies the parameters of an already configured profile based on APN and IP type
   
  PARAMETERS
     req_c_struct  :            Request Message with all TLV
     req_handle  :              Request Handle
     clnt_info   :              Client Info Handle
     msg_id             :       Message ID
     req_c_struct_len   :       length of request message
     service_handle     :       wds service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_wdsi_modify_profile_settings_lite
(
  qmi_wdsi_client_info_type        *clnt_info,
  qmi_req_handle                    req_handle,
  int                               msg_id,
  void                             *req_c_struct,
  int                               req_c_struct_len,
  void                             *service_handle
);

/*===========================================================================
  FUNCTION QMI_WDSI_DELETE_PROFILE_LITE()

  DESCRIPTION

    Deletes an already existing profile 
    
  PARAMETERS
     req_c_struct  :            Request Message with all TLV
     req_handle  :              Request Handle
     clnt_info   :              Client Info Handle
     msg_id             :       Message ID
     req_c_struct_len   :       length of request message
     service_handle     :       wds service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_wdsi_delete_profile_lite
(
  qmi_wdsi_client_info_type        *clnt_info,
  qmi_req_handle                    req_handle,
  int                               msg_id,
  void                             *req_c_struct,
  int                               req_c_struct_len,
  void                             *service_handle
);

/*===========================================================================
  FUNCTION QMI_WDSI_GET_PROFILE_LIST_LITE()

  DESCRIPTION
     Creates a data profile which is used for calls
     
  PARAMETERS
     req_c_struct  :            Request Message with all TLV
     req_handle  :              Request Handle
     clnt_info   :              Client Info Handle
     msg_id             :       Message ID
     req_c_struct_len   :       length of request message
     service_handle     :       wds service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_wdsi_get_profile_list_lite
(
  qmi_wdsi_client_info_type        *clnt_info,
  qmi_req_handle                    req_handle,
  int                               msg_id,
  void                             *req_c_struct,
  int                               req_c_struct_len,
  void                             *service_handle
);



/*===========================================================================
  FUNCTION QMI_WDSI_GET_PKT_STATISTICS()

  DESCRIPTION
    Get the packet statistics

  PARAMETERS
     req_c_struct  :            Request Message with all TLV
     req_handle  :              Request Handle
     clnt_info   :              Client Info Handle
     msg_id             :       Message ID
     req_c_struct_len   :       length of request message
     service_handle     :       wds service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error    qmi_wdsi_get_pkt_statistics
(
  qmi_wdsi_client_info_type        *clnt_info,
  qmi_req_handle                    req_handle,
  int                               msg_id,
  void                             *req_c_struct,
  int                               req_c_struct_len,
  void                             *service_handle
);
 /* qmi_wdsi_get_pkt_statistics() */

 
 /*===========================================================================
 FUNCTION QMI_WDSI_RESET_PKT_STATISTICS()
 
   DESCRIPTION
     Reset packet statistics
 
   PARAMETERS
       req_c_struct  :            Request Message with all TLV
       req_handle  :              Request Handle
       clnt_info   :              Client Info Handle
       msg_id             :       Message ID
       req_c_struct_len   :       length of request message
       service_handle     :       wds service handle

   RETURN VALUE
     qmi_csi_cb_error types.
 
   DEPENDENCIES
     None
 
   SIDE EFFECTS
     None
 ===========================================================================*/
 static qmi_csi_cb_error  qmi_wdsi_reset_pkt_statistics
 (
   qmi_wdsi_client_info_type        *clnt_info,
   qmi_req_handle                    req_handle,
   int                               msg_id,
   void                             *req_c_struct,
   int                               req_c_struct_len,
   void                             *service_handle
 );
  /* qmi_wdsi_reset_pkt_statistics() */

/*===========================================================================
  FUNCTION QMI_WDSI_GET_DATA_BEARER_TECHNOLOGY_EX()

  DESCRIPTION
    Retrive the current data bearer technology in new format

  PARAMETERS
    req_c_struct  :            Request Message with all TLV
     req_handle  :              Request Handle
     clnt_info   :              Client Info Handle
     msg_id             :       Message ID
     req_c_struct_len   :       length of request message
     service_handle     :       wds service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_wdsi_get_data_bearer_technology_ex
(
  qmi_wdsi_client_info_type        *clnt_info,
  qmi_req_handle                    req_handle,
  int                               msg_id,
  void                             *req_c_struct,
  int                               req_c_struct_len,
  void                             *service_handle
);

/*===========================================================================
  FUNCTION QMI_WDSI_SUBSEQUENT_DATA_TRANSFER_STATUS()

  DESCRIPTION
    Request the modem to set subsequent data transfer status
 
  PARAMETERS
   PARAMETERS
     req_c_struct  :            Request Message with all TLV
     req_handle  :              Request Handle
     clnt_info   :              Client Info Handle
     msg_id             :       Message ID
     req_c_struct_len   :       length of request message
     service_handle     :       wds service handle
     
  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_wdsi_subsequent_data_transfer_status
(
  qmi_wdsi_client_info_type        *clnt_info,
  qmi_req_handle                    req_handle,
  int                               msg_id,
  void                             *req_c_struct,
  int                               req_c_struct_len,
  void                             *service_handle
);

/*===========================================================================
  FUNCTION QMI_WDSI_VALIDATE_IP_FAMILY_PREF()

  DESCRIPTION
    Validates the ip family with allowed values
    
  PARAMETERS
     ip_family_pref  :        Ip family

  RETURN VALUE
    qmi_error_type_v01 types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_type_v01 qmi_wdsi_validate_ip_family_pref
(
  uint8  ip_family_pref
);

/*===========================================================================
  FUNCTION QMI_WDSI_MSG_HDLR)CB()

  DESCRIPTION
    It is been registerd with QCCI module and invoked when any QMI 
    message comes dor this service

  PARAMETERS
     sig  :                    Signal
     svc_ptr :             QMI WDS service pointer

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_wdsi_msg_hdlr_cb
(
  ds_sig_enum_type  sig,
  void              *svc_ptr
);

/*===========================================================================
  FUNCTION QMI_WDSI_SVC_CONNECT_CB()

  DESCRIPTION
    Used by QCSI modele to create WDS clients to not already created
    
  PARAMETERS
     client_handle  :           client handle     
     service_handle     :       wds service handle
     connection_handle   :      Client Info Handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

static qmi_csi_cb_error qmi_wdsi_svc_connect_cb
(
  qmi_client_handle          client_handle,
  void                      *service_handle,
  void                     **connection_handle
);

/*===========================================================================
  FUNCTION QMI_WDSI_SVC_DISCONNECT_CB()

  DESCRIPTION
     Used by the qcci modeul to deallocate a client
    
  PARAMETERS
     connection_handle   :      Client Info Handle
     service_handle     :       wds service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_wdsi_svc_disconnect_cb
(
  void                      *connection_handle,
  void                      *service_handle
);

/*===========================================================================
  FUNCTION QMI_WDSI_HANDLE_CLIENT_REQ_CB()

  DESCRIPTION
    Handles the QMI messages from qcci module and calls related message

  PARAMETERS  
     connection_handle   :      Client Info Handle     
     req_handle  :              Request Handle
     msg_id             :       Message ID     
     req_c_struct  :            Request Message with all TLV
     req_c_struct_len   :       length of request message
     service_handle     :       wds service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_wdsi_handle_client_req_cb
(
  void                     *connection_handle,
  qmi_req_handle           req_handle,
  unsigned int             msg_id,
  void                     *req_c_struct,
  unsigned int             req_c_struct_len,
  void                     *service_handle
);

/*===========================================================================
  FUNCTION QMI_GET_WDS_STATE()

  DESCRIPTION
    Return the wds state to which the client is linked
    
  PARAMETERS
     clnt_info  :            Client Info 

  RETURN VALUE
    qmi_wds_client_state_map     .

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_wds_client_state_map *qmi_get_wds_state
(
  qmi_wdsi_client_info_type * client_info
);

/*===========================================================================
  FUNCTION QMI_WDSI_STOP_NET_IFACE_CLIENT())

  DESCRIPTION
   Stops  a data call and posts command to Rmnet

  PARAMETERS
     cl_sp  :                  Client info 
     force  :                  Forceful stop or default 
     wdsi_sp   :               Wds state pointer to which this client belong
     pending_resp      :       Output param if resp is pending 
     req_handle   :            req handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_type_v01 qmi_wdsi_stop_net_iface_client
(
  qmi_wdsi_client_info_type   *cl_sp,
  qmi_wdsi_force_type          force,
  qmi_wds_client_state_map    *wdsi_sp,
  boolean                     *pending_resp,  
  qmi_req_handle               req_handle
);

/*===========================================================================
  FUNCTION QMI_WDSI_GET_CAPABILITIES()

  DESCRIPTION
    Gives the modem capabilities whether Mux is been given by modem
    
  PARAMETERS
     req_c_struct  :            Request Message with all TLV
     req_handle  :              Request Handle
     clnt_info   :              Client Info Handle
     msg_id             :       Message ID
     req_c_struct_len   :       length of request message
     service_handle     :       wds service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_wdsi_get_capabilities
(
  qmi_wdsi_client_info_type        *clnt_info,
  qmi_req_handle          req_handle,
  int                     msg_id,
  void                   *req_c_struct,
  int                     req_c_struct_len,
  void                   *service_handle
);

/*===========================================================================
  FUNCTION QMI_WDSI_BIND_MUX_DATA_PORT()

  DESCRIPTION
    Binds a muxid and data port 
    
  PARAMETERS
     req_c_struct  :            Request Message with all TLV
     req_handle  :              Request Handle
     clnt_info   :              Client Info Handle
     msg_id             :       Message ID
     req_c_struct_len   :       length of request message
     service_handle     :       wds service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_wdsi_bind_mux_data_port
(
   qmi_wdsi_client_info_type        *clnt_info,
   qmi_req_handle           req_handle,
   int                      msg_id,
   void                    *req_c_struct,
   int                      req_c_struct_len,
   void                     *service_handle 
);

/*===========================================================================
  FUNCTION QMI_WDSI_SPLMN_RATE_CONTROL_INFO()

  DESCRIPTION
    Gives the modem capabilities whether Mux is been given by modem
    
  PARAMETERS
     req_c_struct  :            Request Message with all TLV
     req_handle  :              Request Handle
     clnt_info   :              Client Info Handle
     msg_id             :       Message ID
     req_c_struct_len   :       length of request message
     service_handle     :       wds service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error    qmi_wdsi_splmn_rate_control_info
(
  qmi_wdsi_client_info_type        *clnt_info,
  qmi_req_handle                    req_handle,
  int                               msg_id,
  void                             *req_c_struct,
  int                               req_c_struct_len,
  void                             *service_handle
);

/*===========================================================================
  FUNCTION QMI_WDSI_APN_RATE_CONTROL_INFO()

  DESCRIPTION
    Gives the modem capabilities whether Mux is been given by modem
    
  PARAMETERS
     req_c_struct  :            Request Message with all TLV
     req_handle  :              Request Handle
     clnt_info   :              Client Info Handle
     msg_id             :       Message ID
     req_c_struct_len   :       length of request message
     service_handle     :       wds service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error    qmi_wdsi_apn_rate_control_info
(
  qmi_wdsi_client_info_type        *clnt_info,
  qmi_req_handle                    req_handle,
  int                               msg_id,
  void                             *req_c_struct,
  int                               req_c_struct_len,
  void                             *service_handle
);

static wds_ds_extended_error_code_enum_v01 qmi_wdsi_map_profile_error
(
  ds_profile_status_etype profile_status
);


/*===========================================================================
                       FORWARD FUNCTION DECLARATIONS          
===========================================================================*/

/* Define service handle table for wds messages */
static qmi_csi_cb_error (* const req_handle_table[])
(
 qmi_wdsi_client_info_type        *clnt_info,
 qmi_req_handle           req_handle,
 int                      msg_id,
 void                    *req_c_struct,
 int                      req_c_struct_len,
 void                     *service_handle
) = 
{

 NULL,NULL,                                          /* Request handler for message ID 0x00 -0x01 */
  qmi_wdsi_abort,                                     /* Request handler for message ID 0x02 */
  NULL,NULL,                                          /* Request handler for message ID 0x03- 0X04 */                               
  NULL,                                               /* Request handler for message ID 0x05 */
  NULL,                                               /* Request handler for message ID 0x06 */
  NULL,                                               /* Request handler for message ID 0x07 */
  NULL,                                               /* Request handler for message ID 0x08 */ 
  NULL,NULL,                                          /* Request handler for message ID 0x09 -0x0A */
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  /* Request handler for message ID 0x0B -0x14 */
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  /* Request handler for message ID 0x15 -0x1E */
  NULL,                                               /* Request handler for message ID 0x1F */
  qmi_wdsi_start_network_interface,                   /* Request handler for message ID 0x20 */
  qmi_wdsi_stop_network_interface,                    /* Request handler for message ID 0x21 */
  qmi_wdsi_get_pkt_srvc_status,                       /* Request handler for message ID 0x22 */
  NULL,                                               /* Request handler for message ID 0x23 */
  qmi_wdsi_get_pkt_statistics ,                       /* Request handler for message ID 0x24 */
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,            /* Request handler for message ID 0x25 -0x2c */
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  /* Request handler for message ID 0x2d -0x36 */
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  /* Request handler for message ID 0x37 -0x40 */
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  /* Request handler for message ID 0x41 -0x4A */
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  /* Request handler for message ID 0x4B -0x54 */
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  /* Request handler for message ID 0x55 -0x5E */
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  /* Request handler for message ID 0x5F -0x68 */
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  /* Request handler for message ID 0x69 -0x72 */
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  /* Request handler for message ID 0x73 -0x7C */
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,       /* Request handler for message ID 0x7D -0x85 */
  qmi_wdsi_reset_pkt_statistics,                       /* Request handler for message ID 0x86 */
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  /* Request handler for message ID 0x87 -0x90 */
  qmi_wdsi_get_data_bearer_technology_ex,             /* Request handler for message ID 0x91   */
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,       /* Request handler for message ID 0x92 -0x9A */
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,qmi_wdsi_bind_mux_data_port,NULL,NULL,  /* Request handler for message ID 0x9B -0xA4 */
  NULL,NULL,NULL,                                     /* Request handler for message ID 0xA5 -0xA7 */
  NULL,                                               /* Request handler for message ID 0xA8 */
  qmi_wdsi_get_capabilities,                          /* Request handler for message ID 0xA9 */
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  //179/* Request handler for message ID 0xAA -0xB3 */
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  /* Request handler for message ID 0xB4 -0xBD */
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  /* Request handler for message ID 0xBE -0xC7 */
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  /* Request handler for message ID 0xC8 -0xD1 */
  NULL,NULL,NULL,                                          /* Request handler for message ID 0xD2 -0xD4 */
  qmi_wdsi_subsequent_data_transfer_status ,          /* Request handler for message ID 0xD5 */
  NULL,NULL,NULL,NULL,NULL,NULL,                       /* Request handler for message ID 0xD6 -0xDB */
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,                 /* Request handler for message ID 0xDC -0xE2 */
  qmi_wdsi_get_avail_external_ipv6_addr,              /* Request handler for message ID 0xE3*/
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,       /* Request handler for message ID 0xE4 -0xEC*/ 
  qmi_wdsi_create_profile_lite,                       /* Request handler for message ID 0xED */
  qmi_wdsi_modify_profile_settings_lite,              /* Request handler for message ID 0xEE */
  qmi_wdsi_delete_profile_lite,                       /* Request handler for message ID 0xEF */
  qmi_wdsi_get_profile_settings_lite,                 /* Request handler for message ID 0xF0 */
  qmi_wdsi_get_profile_list_lite,                          /* Request handler for message ID 0xF1 */
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,                 /* Request handler for message ID 0xF2 -0xF8 */
  qmi_wdsi_splmn_rate_control_info,                    /* Request handler for message ID 0xF9 */ 
  qmi_wdsi_apn_rate_control_info                       /* Request handler for message ID 0xFA */ 

  };

/*===========================================================================
  FUNCTION QMI_WDSI_COMPARE_CLIENT_HANDLE()

  DESCRIPTION
     Compares the qmi client handle     
     
  PARAMETERS
     item_ptr  :                item_ptr to compare
     compare_val  :              Client handle

  RETURN VALUE
    integer

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static int qmi_wdsi_compare_client_handle
( 
  void *item_ptr,
 void *compare_val 
)
{
  qmi_wdsi_client_info_type *clnt_info = (qmi_wdsi_client_info_type*)item_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ( (clnt_info->clnt == 
                          (qmi_client_handle)compare_val) ? 1 : 0 );
}

static int wdsi_compare_connection_info_handle
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
  FUNCTION QMI_WDS_INIT()

  DESCRIPTION
    Register the wds service with QCSI 

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_wds_init
(
  void
)
{
  qmi_csi_error         rc;
  qmi_sap_error         src;  
  qmi_csi_options       options;  
  uint8                 i =0 ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_MSG1(MSG_LEGACY_MED,"QMI WDS service init %d", qmi_wdsi_inited);
  if (!qmi_wdsi_inited)
  {
    /*-------------------------------------------------------------------------
      Setting DS signal handler for handling QMI messages
    -------------------------------------------------------------------------*/
    (void)reg_set_sig_handler(DS_WDS_RDR_SIGNAL, 
                              qmi_wdsi_msg_hdlr_cb, &qmi_wds_state);
    /* Set the signal to init again */
    (void)rex_set_sigs( ds_get_ds_task_ptr(),
                   (rex_sigs_type) 1 << (rex_sigs_type)(DS_WDS_RDR_SIGNAL) ); 
    return;
  }
  os_params.tcb = ds_get_ds_task_ptr();
  os_params.sig = WDSI_QCSI_OS_SIGNAL;
  QMI_CSI_OPTIONS_INIT(options);
  (void)mqcsi_register_log_options(&options);

  /*-------------------------------------------------------------------------
    Register QMI WDS service with QCSI
  -------------------------------------------------------------------------*/
  rc = qmi_csi_register_with_options(wds_get_service_object_v01(),
                        qmi_wdsi_svc_connect_cb,
                        qmi_wdsi_svc_disconnect_cb, 
                        qmi_wdsi_handle_client_req_cb, 
                        &qmi_wds_state, &os_params,
                        &options,
                        &qmi_wds_state.service_handle);
  
  if(rc != QMI_CSI_NO_ERR)
  {
    ERR_FATAL("Unable to register wds service! Error %d", rc, 0, 0);
    return;
  }

  //register with SAP
  src = qmi_sap_register(wds_get_service_object_v01(), NULL,
                        &qmi_wds_state.sap_client_handle);
  
  if (src != QMI_SAP_NO_ERR)
  {
    ERR_FATAL("Unable to register wds service with QSAP! Error %d", src, 0, 0);
    return;
  }

  /* Initialize the queue which will hold all handles of clients */
  (void) q_init(&qmi_wds_state.client_q);
  qmi_wds_state.num_of_clients = 0;
  
  for(i =0 ; i< WDS_MAX_INSTANCE; i++)
  {
      /* Initialize the queue which will hold all handles of clients */
      qmi_wds_cl_sp_map[i] = NULL;
  }
  rmnet_wds_reg_events(&wdsi_reg_rmnet_pdn_ev ,NULL);
} /* qmi_wds_init */

qmi_wds_client_state_map *qmi_get_wds_state
(
  qmi_wdsi_client_info_type *client_info
)
{
  if((client_info != NULL) &&(client_info->wds_inst != QMI_WDS_INVALID_INST))
  {
    return qmi_wds_cl_sp_map[client_info->wds_inst];
  }
  return NULL;
}

/*===========================================================================
  FUNCTION QMI_WDSI_NET_IF_BOUND()

  DESCRIPTION
    Checks if any client has started the call on WDS instance 
    
  PARAMETERS
    wdsi_sp  :                     WDS state 
     
  RETURN VALUE
    TRUE if started the call.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

boolean qmi_wdsi_net_if_bound
(
  qmi_wds_client_state_map * wdsi_sp
)
{
  qmi_wdsi_client_info_type *client_info = NULL;
/*--------------------------------------------------------------------------*/
  
  client_info = wdsi_sp->client_list;
  while(client_info != NULL)
  {
   if(client_info->net_bound == TRUE)
   {
	 return TRUE;
   }
   client_info = client_info->next;
  }
  return FALSE;
}

static boolean qmi_wdsi_msg_hdlr_cb
(
  ds_sig_enum_type  sig,
  void              *svc_ptr
)
{
  qmi_wdsi_state_info_type *wds_sp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_ASSERT((sig == DS_WDS_RDR_SIGNAL) && svc_ptr);

  if (!qmi_wdsi_inited)
  {
    qmi_wdsi_inited = TRUE;
    /* Call init again to register with QCSI now
     * Wait for all tasks to be inited */
	qmi_wds_init();
	return TRUE;
  }
  
  wds_sp = (qmi_wdsi_state_info_type*) svc_ptr;
  qmi_csi_handle_event(wds_sp->service_handle, &os_params);
  return TRUE;
} /* qmi_nipdi_msg_hdlr_cb */

static qmi_csi_cb_error qmi_wdsi_svc_connect_cb
(
  qmi_client_handle          client_handle,
  void                      *service_handle,
  void                     **connection_handle
)
{
  qmi_wdsi_client_info_type    *clnt_info_ptr;
  qmi_wdsi_state_info_type     *wds_svc_ptr;
  qmi_csi_cb_error               rc;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_ASSERT(connection_handle != NULL);
  DS_ASSERT(service_handle != NULL);

  wds_svc_ptr = (qmi_wdsi_state_info_type *) service_handle;
 
  if ( NULL != q_linear_search( &wds_svc_ptr->client_q,
                                qmi_wdsi_compare_client_handle,
                                client_handle ) )
  {
    DS_MSG1(MSG_LEGACY_ERROR,"wdsi_svc_connect_cb: wdsi client 0x%p present",
	 client_handle);
	rc = QMI_CSI_CB_CONN_REFUSED;
    goto bail;
  }

  if (WDSI_MAX_CLIDS == wds_svc_ptr->num_of_clients)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"qmi_wdsi_svc_connect_cb:"
            "Maximum number of clients already allocated %d",
                  WDSI_MAX_CLIDS);
    rc = QMI_CSI_CB_CONN_REFUSED;
    goto bail;
  }

  clnt_info_ptr = ds_system_heap_mem_alloc(
                    sizeof(qmi_wdsi_client_info_type),__FILENAME__ , __LINE__);
  if(!clnt_info_ptr)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"qmi_wdsi_svc_connect_cb: Out of mem");
    rc = QMI_CSI_CB_CONN_REFUSED;
    goto bail;
  }
  memset(clnt_info_ptr, 0, sizeof(qmi_wdsi_client_info_type));
  clnt_info_ptr->wds_inst  = QMI_WDS_INVALID_INST;

  (void) q_link ( clnt_info_ptr, &clnt_info_ptr->link );
  clnt_info_ptr->clnt = client_handle;
  /* Add it to the queue of clients */
  q_put( &(wds_svc_ptr->client_q), &(clnt_info_ptr->link) );
  wds_svc_ptr->num_of_clients++;

  *connection_handle = clnt_info_ptr;
  DS_MSG1(MSG_LEGACY_MED,"qmi_wdsi_svc_connect_cb: Alloc client 0x%p",
                          clnt_info_ptr);
  rc = QMI_CSI_CB_NO_ERR;
  
 bail:
  return rc;

} /* qmi_wdsi_svc_connect_cb() */

static void qmi_wdsi_svc_disconnect_cb
(
  void                      *connection_handle,
  void                      *service_handle
)
{
  qmi_wdsi_state_info_type         *wds_svc_ptr =NULL;
  qmi_wdsi_client_info_type        *clnt_info_ptr  =NULL;
  qmi_wdsi_client_info_type        *prev_client =NULL;
  qmi_wdsi_client_info_type        *client = NULL;
  qmi_wds_client_state_map         *wds_sp = NULL;
  uint8                             wds_inst = QMI_WDS_INVALID_INST;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  DS_ASSERT(connection_handle != NULL);
  DS_ASSERT(service_handle != NULL);
  wds_svc_ptr = (qmi_wdsi_state_info_type *)service_handle;

  DS_MSG1(MSG_LEGACY_MED,"qmi_wds_svc_disconnect_cb: Releasing client 0x%p", 
                connection_handle);

  clnt_info_ptr = q_linear_search( &wds_svc_ptr->client_q,
                                   wdsi_compare_connection_info_handle,
                                   connection_handle );
  if(NULL == clnt_info_ptr)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"qmi_wdsi_svc_disconnect_cb: wds client 0x%p"
            "not found",connection_handle);
    return;

  }
  wds_sp = qmi_get_wds_state(clnt_info_ptr); 
  if(wds_sp != NULL)
  {  
   wds_inst = clnt_info_ptr->wds_inst;
   client = wds_sp->client_list;
   while(client != NULL)
   {
     if (qmi_wdsi_compare_client_handle(clnt_info_ptr, client->clnt))
	 {
       DS_MSG4(MSG_LEGACY_ERROR,"create_cl_sp_map: Client already present %p and"
               "%p at wds_inst %d wds_sp %p",clnt_info_ptr->clnt,client->clnt ,wds_inst,wds_sp);

      // Unlink the client from client list
       if (prev_client == NULL)
       {
         wds_sp->client_list = client->next;
       }
       else
       {
         prev_client->next = client->next;
       }
       break;
     }
     prev_client = client;
     client = client->next;
    }
    if(wds_sp->client_list == NULL)
    {
      DS_SYSTEM_HEAP_MEM_FREE(wds_sp);
      if(wds_inst < WDS_MAX_INSTANCE)
         qmi_wds_cl_sp_map[wds_inst] = NULL;
      DS_MSG0(MSG_LEGACY_ERROR,"All the clients are realesed so memset the wds_sp");
    }
  }
  if(TRUE != q_delete_ext( &wds_svc_ptr->client_q,
                             &clnt_info_ptr->link ) )
  {
    DS_MSG1(MSG_LEGACY_ERROR,"qmi_wdsi_svc_disconnect_cb: wds client 0x%p "
            "could not be deleted from queue",
                    clnt_info_ptr);
    return;
  }
  DS_SYSTEM_HEAP_MEM_FREE(clnt_info_ptr);
  wds_svc_ptr->num_of_clients--;
} /* qmi_wdsi_svc_disconnect_cb() */

/*===========================================================================
  FUNCTION QMI_WDS_PROCESS_CMD()

  DESCRIPTION
    This function processes a QMI wds command

    It is called by the QMI command handler and will dispatch the
    associated command/event handler function.

  PARAMETERS
    cmd_ptr:  private data buffer containing the QMI wds command
              information.

  RETURN VALUE
    None

  DEPENDENCIES
    QMI wds must already have been initialized

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_wdsi_process_cmd
(
  ds_command_enum_type  cmd,
  void              *cmd_ptr
)
{
  qmi_wdsi_cmd_buf_type *cmd_buf_ptr = NULL;
/*-------------------------------------------------------------------------*/
  DS_ASSERT((int)cmd_ptr);
  cmd_buf_ptr = (qmi_wdsi_cmd_buf_type *)cmd_ptr;

  DS_MSG1(MSG_LEGACY_MED,"qmi_wds_process_cmd: cmdbuf's cmd_id:%d",
                          cmd_buf_ptr->cmd_id);
  switch(cmd_buf_ptr->cmd_id)
  {
   break;
  }
  
} /* qmi_wdsi_process_cmd() */

static qmi_csi_cb_error qmi_wdsi_handle_client_req_cb
(
  void                     *connection_handle,
  qmi_req_handle            req_handle,
  unsigned int              msg_id,
  void                     *req_c_struct,
  unsigned int              req_c_struct_len,
  void                     *service_handle
)
{
  qmi_csi_cb_error                rc;
  qmi_wdsi_client_info_type     *clnt_info_ptr;
  qmi_wdsi_state_info_type      *wds_svc_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_ASSERT((int)connection_handle);
  DS_ASSERT((int)service_handle);

  rc = QMI_CSI_CB_NO_ERR;
  wds_svc_ptr = (qmi_wdsi_state_info_type*) service_handle;

  clnt_info_ptr = q_linear_search( &wds_svc_ptr->client_q,
                                   wdsi_compare_connection_info_handle,
                                   connection_handle );
  if(NULL == clnt_info_ptr)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"qmi_wdsi_handle_client_req_cb: Invalid clnt 0x%p",
                  connection_handle);
	rc = QMI_CSI_CB_INTERNAL_ERR;
    return rc;
  }
  
  DS_MSG1(MSG_LEGACY_ERROR,"qmi_wdsi_handle_client_req_cb: message ID: %d",
                            msg_id);

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
      DS_MSG1(MSG_LEGACY_ERROR,"qmi_wdsi_handle_client_req_cb: "
              "NULL message ID handler: %d",
                    msg_id);
	  rc = QMI_CSI_CB_UNSUPPORTED_ERR;
    }
  }
  else
  {
    DS_MSG1(MSG_LEGACY_ERROR,"qmi_wdsi_handle_client_req_cb:"
            "Invalid message ID: %d", msg_id);
    rc = QMI_CSI_CB_UNSUPPORTED_ERR;
  }
  return rc;
} /* qmi_nipdi_handle_client_req_cb() */

/*===========================================================================
  FUNCTION QMI_WDSI_GET_AVAIL_EXTERNAL_IPV6_ADDR()

  DESCRIPTION

  This function retrieves exteranl IPV6 information.

  PARAMETERS
     req_c_struct  :            Request Message with all TLV
     req_handle  :              Request Handle
     clnt_info   :              Client Info Handle
     msg_id             :       Message ID
     req_c_struct_len   :       length of request message
     service_handle     :       wds service handle

  RETURN VALUE
    qmi_csi_cb_error 

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_wdsi_get_avail_external_ipv6_addr
(
   qmi_wdsi_client_info_type 	   *clnt_info,
   qmi_req_handle                   req_handle,
   int                              msg_id,
   void                            *req_c_struct,
   int                              req_c_struct_len,
   void                            *service_handle
)
{
  qmi_wds_client_state_map                   *wdsi_sp = NULL;
  qmi_error_type_v01                          errval;
  struct ps_in6_addr                          temp_ipv6_addr;
  wds_get_avail_ext_ipv6_addr_resp_msg_v01    resp_msg;
  boolean                                     info_avail = FALSE;
  ds_fwk_clnt_info_s_type                     clnt_info_ptr ={0};  
  ds_fwk_v6_addr_s_type                      *external_addr =NULL;  
  uint8                                       i =1;
  uint8                                       prefix_len =0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_ASSERT(req_c_struct &&(req_c_struct_len > 0)&&clnt_info && service_handle);
  memset(&resp_msg, 0, sizeof(wds_get_avail_ext_ipv6_addr_resp_msg_v01));

  errval = QMI_ERR_NONE_V01;
  memset(&resp_msg,0,sizeof(resp_msg));
  //memset(&ds_v6_addr_list, 0 , sizeof(ds_v6_addr_list));
  wdsi_sp = qmi_get_wds_state(clnt_info);
  if((wdsi_sp ==NULL)||
     (wdsi_sp->pkt_svc_state != WDS_CONNECTION_STATUS_CONNECTED_V01))
  {
    DS_MSG0(MSG_LEGACY_MED,"Call not up. Cannot get prefix");
    errval = QMI_ERR_OUT_OF_CALL_V01;
    goto send_result;
  }

  ds_fwk_get_clnt_info_from_handle(DS_FWK_CLIENT_RMNET,
                                   wdsi_sp->binding.rmnet_inst,
                                  &clnt_info_ptr);
  if(clnt_info_ptr.fw_inst == NULL)
  { 	
    DS_ERR_FATAL("No framework instance could be found for this"
                 "call and state is connected");
  }   
  if((clnt_info_ptr.ip_type != IPV6_ADDR)||
    (FALSE == ds_fwk_inst_is_valid(clnt_info_ptr.fw_inst,
                                   clnt_info_ptr.ip_type)))
  {
     DS_MSG0(MSG_LEGACY_MED,"Call not up. Cannot get prefix");
     errval = QMI_ERR_OUT_OF_CALL_V01;
    goto send_result;
  }
  prefix_len = clnt_info_ptr.fw_inst->v6_net_info.ipv6_addrs[0]->prefix_len;
  while(TRUE)
  {
    if(i == DS_MAX_IPV6_ADDRS)
     break;
    external_addr = clnt_info_ptr.fw_inst->v6_net_info.ipv6_addrs[i];
    if((external_addr != NULL) && (external_addr->addr_type == DS_IPV6_ADDR_TYPE_EXTERNAL))
    {
       DS_MSG2(MSG_LEGACY_ERROR,"qmi_wdsi_get_avail_exteranl_ipv6_addr index %d"
             "external_addr->addr_type %d",i ,external_addr->addr_type);
       if((external_addr->client_id == DS_FWK_CLIENT_RMNET)&&
          (resp_msg.ext_ipv6_addr_list_len < WDS_EXT_IPV6_ADDRS_MAX_V01))
       {
         info_avail =TRUE;
	     resp_msg.ext_ipv6_addr_list_valid = TRUE;
         temp_ipv6_addr.ps_s6_addr64[0] = external_addr->prefix;
         temp_ipv6_addr.ps_s6_addr64[1] = external_addr->iid;
         memscpy(resp_msg.ext_ipv6_addr_list[resp_msg.ext_ipv6_addr_list_len].\
                 ipv6_addr, 
                 sizeof(resp_msg.\
                 ext_ipv6_addr_list[resp_msg.ext_ipv6_addr_list_len].ipv6_addr),
                 temp_ipv6_addr.in6_u.u6_addr64,
                 sizeof(temp_ipv6_addr.in6_u.u6_addr64));
         
          resp_msg.ext_ipv6_addr_list[resp_msg.ext_ipv6_addr_list_len].\
            ipv6_prefix_length = prefix_len;

        IPV6_ADDR_MSG(temp_ipv6_addr.in6_u.u6_addr64);
        resp_msg.ext_ipv6_addr_list_len++;
      }
   }
   i++;
 }
 if (!info_avail)
 {
   errval = QMI_ERR_INFO_UNAVAILABLE_V01;
 }
  
  send_result:
  resp_msg.response.result = (errval == QMI_ERR_NONE_V01 ?
                  QMI_RESULT_SUCCESS_V01 : QMI_RESULT_FAILURE_V01);
  resp_msg.response.error  = errval;  

  DS_MSG2(MSG_LEGACY_MED,"qmi_wdsi_get_avail_exteranl_ipv6_addr result %d"
          "errval %d",resp_msg.response.result, errval);
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(wds_get_avail_ext_ipv6_addr_resp_msg_v01));  
  return QMI_CSI_CB_NO_ERR;  
  
} /* qmi_wdsi_get_avail_exteranl_ipv6_addr */

/*===========================================================================
  FUNCTION QMI_WDSI_PROFILE_MASK()

  DESCRIPTION
    Api provides the mask value of the profile parameters changed

  PARAMETERS
     in_req_msg  :              Request Message with all TLV
     payload  :                 Profile DB payload
     ident_info   :             Identity  Info
     profile_op             :   Operation Create or modify

  RETURN VALUE
    int64 

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
uint64 qmi_wdsi_get_profile_mask
(
 void                                           *in_req_msg ,
 ds_profile_type_s                              *payload,
 ds_profile_ident_info                          *ident_info, 
 qmi_wdsi_profile_op_type                        profile_op
)
{
  wds_create_profile_lite_req_msg_v01             *create_req_msg = NULL;
  wds_modify_profile_settings_lite_req_msg_v01    *req_msg = NULL;
  uint64                                          mask =0;
 /*---------------------------------------------------------------------------*/
  if(profile_op == WDSI_PROFILE_CREATE)
  {
    create_req_msg = (wds_create_profile_lite_req_msg_v01 *)in_req_msg;        
    if(create_req_msg->apn_name_valid)
    {
      memscpy(payload->context.apn ,DS_PROFILE_APN_STRING_LEN,create_req_msg->apn_name,
              DS_PROFILE_APN_STRING_LEN);
      mask = mask|DS_PROFILE_APN_NAME;
    }
    payload->context.pdp_type   = create_req_msg->pdp_type;      
    mask = mask|DS_PROFILE_TYPE;
 
    if(create_req_msg->pdp_hdr_compression_type_valid)
    {
      payload->context.h_comp = create_req_msg->pdp_hdr_compression_type;  
      mask = mask|DS_PROFILE_CONTEXT_H_COMP;
    }
    if(create_req_msg->pdp_data_compression_type_valid)
    {
      payload->context.d_comp = create_req_msg->pdp_data_compression_type;
      mask = mask|DS_PROFILE_CONTEXT_D_COMP;
    }
    if(create_req_msg->primary_DNS_IPv4_address_preference_valid)
    {
      payload->dns_addr.ip_vsn = DS_PROFILE_IP_V4;
      payload->dns_addr.primary_dns_addr.pdp_addr.pdp_addr_ipv4 = 
                 create_req_msg->primary_DNS_IPv4_address_preference;
      mask = mask|DS_PROFILE_DNS_ADDR_V4_PRIMARY;
    }
    if(create_req_msg->secondary_DNS_IPv4_address_preference_valid)
    { 
      payload->dns_addr.ip_vsn = DS_PROFILE_IP_V4;
      payload->dns_addr.secondary_dns_addr.pdp_addr.pdp_addr_ipv4 =
               create_req_msg->secondary_DNS_IPv4_address_preference;
      mask = mask|DS_PROFILE_DNS_ADDR_V4_SECONDARY;
    }
    if(create_req_msg->username_valid)
    {
      strlcpy(payload->auth.username,create_req_msg->username,
             (QMI_WDS_USER_NAME_MAX_V01+1));
       mask = mask|DS_PROFILE_AUTH_USERNAME;
    }
    if(create_req_msg->password_valid)
    {
      strlcpy(payload->auth.password,create_req_msg->password,
             (QMI_WDS_PASSWORD_MAX_V01+1));
      mask = mask|DS_PROFILE_AUTH_PASSWORD;
    }
    if(create_req_msg->authentication_preference_valid)
    {
       payload->auth.auth_type = create_req_msg->authentication_preference;
       mask = mask|DS_PROFILE_AUTH_TYPE;
    }
    if(create_req_msg->pcscf_addr_using_pco_valid)
    {
      payload->request_pcscf_address_flag = create_req_msg->pcscf_addr_using_pco;
      mask = mask|DS_PROFILE_PCFCH_ADDRESS_USING_DHCP_FLAG;
    }
    if(create_req_msg->pdp_access_control_flag_valid)
    {
      payload->context.access_ctrl_flag = create_req_msg->pdp_access_control_flag;
      mask = mask|DS_PROFILE_ACCESS_CTRL_FLAG;
    }
    if(create_req_msg->im_cn_flag_valid)
    {
      payload->im_cn_flag = create_req_msg->im_cn_flag;
      mask = mask|DS_PROFILE_IM_CN_FLAG;
    }
    if(create_req_msg->primary_dns_ipv6_address_preference_valid)
    { 
       payload->dns_addr.ip_vsn = DS_PROFILE_IP_V6;
       memscpy((void *)&payload->dns_addr.primary_dns_addr.
               pdp_addr.pdp_addr_ipv6.in6_u, sizeof(struct ps_in6_addr),
              (void *)&create_req_msg->primary_dns_ipv6_address_preference,
              sizeof(struct ps_in6_addr));
       mask = mask|DS_PROFILE_DNS_ADDR_V6_PRIMARY;
    }
    if(create_req_msg->secondary_dns_ipv6_address_preference_valid)
    { 
      payload->dns_addr.ip_vsn = DS_PROFILE_IP_V6;
      memscpy((void *)&payload->dns_addr.secondary_dns_addr.
               pdp_addr.pdp_addr_ipv6.in6_u, sizeof(struct ps_in6_addr),
               (void *)&create_req_msg->secondary_dns_ipv6_address_preference,
               sizeof(struct ps_in6_addr)); 
      mask = mask|DS_PROFILE_DNS_ADDR_V6_SECONDARY;
    }
    if(create_req_msg->apn_disabled_flag_valid)
    {
       payload->apn_disable_flag = create_req_msg->apn_disabled_flag;
       mask = mask|DS_PROFILE_APN_DISABLE_FLAG;
    }
    if(create_req_msg->pdn_inactivity_timeout_valid)
    {
      payload->inactivity_timer_val = create_req_msg->pdn_inactivity_timeout;
      mask = mask|DS_PROFILE_PDN_INACTIVITY_TIMEOUT;
    }
    if(create_req_msg->apn_class_valid)
    {
       payload->apn_class = create_req_msg->apn_class;
       mask = mask|DS_PROFILE_APN_CLASS;
    }
    if(create_req_msg->apn_bearer_valid)
    {
       if (create_req_msg->apn_bearer & WDS_MASK_APN_BEARER_ANY_V01) 
       {
         payload->apn_bearer = 0xFF;
       }
       else
       {
         payload->apn_bearer = create_req_msg->apn_bearer;
       }
       mask = mask|DS_PROFILE_APN_BEARER;
    }
    if(create_req_msg->op_pco_id_valid)
    {
       payload->operator_reserved_pco = create_req_msg->op_pco_id;
       mask = mask|DS_PROFILE_OPERATOR_RESERVED_PCO;
    }
    if(create_req_msg->pco_mcc_valid)
    {
      payload->mcc = create_req_msg->pco_mcc;
      mask = mask|DS_PROFILE_MCC;
    }
    if(create_req_msg->pco_mnc_includes_pcs_digit_valid)
    {
       payload->mnc.mnc_digits = create_req_msg->pco_mnc_includes_pcs_digit.mnc;
       payload->mnc.mnc_includes_pcs_digit = 
         create_req_msg->pco_mnc_includes_pcs_digit.mnc_includes_pcs_digit;
        mask = mask|DS_PROFILE_MNC;
    }
    if(create_req_msg->max_pdn_conn_per_block_valid)
    {
       payload->max_pdn_conn_per_blk = create_req_msg->max_pdn_conn_per_block;
       mask = mask|DS_PROFILE_MAX_PDN_CONN_PER_BLOCK;    

    }
    if(create_req_msg->max_pdn_conn_timer_valid)
    {
      payload->max_pdn_conn_time = create_req_msg->max_pdn_conn_timer;
      mask = mask|DS_PROFILE_MAX_PDN_CONN_TIMER;
    }
    if(create_req_msg->pdn_req_wait_interval_valid)
    {
       payload->pdn_req_wait_time = create_req_msg->pdn_req_wait_interval;

       mask = mask|DS_PROFILE_PDN_REQ_WAIT_TIMER;
    }
    if(create_req_msg->roaming_disallowed_valid)
    {
      payload->roaming_disallowed = create_req_msg->roaming_disallowed;
      mask = mask|DS_PROFILE_ROAMING_DISALLOWED;
    }
    if(create_req_msg->pdn_discon_wait_timer_valid)
    {
      payload->pdn_discon_wait_time = create_req_msg->pdn_discon_wait_timer;
      mask = mask|DS_PROFILE_PDN_DISCON_WAIT_TIME;
    }
    if(create_req_msg->lte_roaming_pdp_type_valid)
    {
       payload->context.lte_roaming_pdp_type = 
              create_req_msg->lte_roaming_pdp_type;
      mask = mask|DS_PROFILE_PARAM_LTE_ROAMING_PDP_TYPE;
    }
    if(create_req_msg->umts_roaming_pdp_type_valid)
    {
       payload->context.umts_roaming_pdp_type = 
                create_req_msg->umts_roaming_pdp_type;
      mask = mask|DS_PROFILE_PARAM_UMTS_ROAMING_PDP_TYPE;
    }
    if(create_req_msg->override_home_pdp_type_valid)
    {
       payload->context.override_home_pdp_type = 
                create_req_msg->override_home_pdp_type;
       mask = mask|DS_PROFILE_PARAM_OVERRIDE_HOME_PDP_TYPE;
    }
    if(create_req_msg->attach_wo_pdn_valid)
    {
       payload->attach_wo_pdn = create_req_msg->attach_wo_pdn;
       mask = mask|DS_PROFILE_PARAM_ATTACH_WO_PDN;
    }
    if(create_req_msg->clat_enabled_valid)
    {
       payload->clat_enabled = create_req_msg->clat_enabled;
       mask = mask|DS_PROFILE_CLAT_ENABLED;
    }
    if(create_req_msg->failure_timer_3gpp_valid)
    {
       memscpy((payload->failure_timers), sizeof(payload->failure_timers),
               (create_req_msg->failure_timer_3gpp),sizeof(create_req_msg->failure_timer_3gpp));
       mask = mask|DS_PROFILE_FAILURE_TIMERS;
    }
    if(create_req_msg->msisdn_flag_valid)
    {
      payload->msisdn_flag = create_req_msg->msisdn_flag;
      mask = mask|DS_PROFILE_MSISDN_FLAG;
    }
  }
  else if(profile_op == WDSI_PROFILE_MODIFY)
  {
    req_msg = (wds_modify_profile_settings_lite_req_msg_v01 *)in_req_msg;
    if(req_msg->changed_apn_name_valid)
    {
       memscpy(payload->context.apn,DS_PROFILE_APN_STRING_LEN,req_msg->changed_apn_name,
                   DS_PROFILE_APN_STRING_LEN);         
       mask = mask|DS_PROFILE_APN_NAME;
    }
    if(req_msg->changed_pdp_type_valid)
    {
       payload->context.pdp_type   = req_msg->changed_pdp_type;	   
       mask = mask|DS_PROFILE_TYPE;
    }
    if(req_msg->pdp_hdr_compression_type_valid)
    {
      payload->context.h_comp = req_msg->pdp_hdr_compression_type;	
      mask = mask|DS_PROFILE_CONTEXT_H_COMP;
    }
    if(req_msg->pdp_data_compression_type_valid)
    {
      payload->context.d_comp = req_msg->pdp_data_compression_type;
      mask = mask|DS_PROFILE_CONTEXT_D_COMP;
    }
    if(req_msg->primary_DNS_IPv4_address_preference_valid)
    {
       payload->dns_addr.ip_vsn = DS_PROFILE_IP_V4;
       payload->dns_addr.primary_dns_addr.pdp_addr.pdp_addr_ipv4 = 
            req_msg->primary_DNS_IPv4_address_preference;
       mask = mask|DS_PROFILE_DNS_ADDR_V4_PRIMARY;
    }
    if(req_msg->secondary_DNS_IPv4_address_preference_valid)
    { 
       payload->dns_addr.ip_vsn = DS_PROFILE_IP_V4;
       payload->dns_addr.secondary_dns_addr.pdp_addr.pdp_addr_ipv4 = 
             req_msg->primary_DNS_IPv4_address_preference;
       mask = mask|DS_PROFILE_DNS_ADDR_V4_SECONDARY;
    }
    if(req_msg->username_valid)
    {
      strlcpy(payload->auth.username,req_msg->username,
             (QMI_WDS_USER_NAME_MAX_V01+1));
      mask = mask|DS_PROFILE_AUTH_USERNAME;
    }
    if(req_msg->password_valid)
    {
       strlcpy(payload->auth.password,req_msg->password,
              (QMI_WDS_PASSWORD_MAX_V01+1));
       mask = mask|DS_PROFILE_AUTH_PASSWORD;
    }
    if(req_msg->authentication_preference_valid)
    {
      payload->auth.auth_type = req_msg->authentication_preference;
      mask = mask|DS_PROFILE_AUTH_TYPE;
    }
    if(req_msg->pcscf_addr_using_pco_valid)
    {
      payload->request_pcscf_address_flag = req_msg->pcscf_addr_using_pco;
      mask = mask|DS_PROFILE_PCFCH_ADDRESS_USING_DHCP_FLAG;
    }
    if(req_msg->pdp_access_control_flag_valid)
    {
       payload->context.access_ctrl_flag = req_msg->pdp_access_control_flag;
       mask = mask|DS_PROFILE_ACCESS_CTRL_FLAG;
    }
    if(req_msg->im_cn_flag_valid)
    {
       payload->im_cn_flag = req_msg->im_cn_flag;
       mask = mask|DS_PROFILE_IM_CN_FLAG;
    }
    if(req_msg->primary_dns_ipv6_address_preference_valid)
    { 
      payload->dns_addr.ip_vsn = DS_PROFILE_IP_V6;
      memscpy((void *)&payload->dns_addr.primary_dns_addr.
              pdp_addr.pdp_addr_ipv6.in6_u, sizeof(struct ps_in6_addr),
              (void *)&req_msg->primary_dns_ipv6_address_preference,
             sizeof(struct ps_in6_addr));
       mask = mask|DS_PROFILE_DNS_ADDR_V6_PRIMARY;
    }
    if(req_msg->secondary_dns_ipv6_address_preference_valid)
    { 
       payload->dns_addr.ip_vsn = DS_PROFILE_IP_V6;
       memscpy((void *)&payload->dns_addr.secondary_dns_addr.
                pdp_addr.pdp_addr_ipv6.in6_u, sizeof(struct ps_in6_addr),
               (void *)&req_msg->primary_dns_ipv6_address_preference,
                sizeof(struct ps_in6_addr));
       mask = mask|DS_PROFILE_DNS_ADDR_V6_SECONDARY;
    }
    if(req_msg->apn_disabled_flag_valid)
    {
       payload->apn_disable_flag = req_msg->apn_disabled_flag;
       mask = mask|DS_PROFILE_APN_DISABLE_FLAG;
    }
    if(req_msg->pdn_inactivity_timeout_valid)
    {
       payload->inactivity_timer_val = req_msg->pdn_inactivity_timeout;
       mask = mask|DS_PROFILE_PDN_INACTIVITY_TIMEOUT;
    }
    if(req_msg->apn_class_valid)
    {
       payload->apn_class = req_msg->apn_class;
       mask = mask|DS_PROFILE_APN_CLASS;
    }
    if(req_msg->apn_bearer_valid)
    {
      if (req_msg->apn_bearer & WDS_MASK_APN_BEARER_ANY_V01) 
      {
        payload->apn_bearer = 0xFF;
      }
      else
      {
        payload->apn_bearer = req_msg->apn_bearer;

      }
       mask = mask|DS_PROFILE_APN_BEARER;
    }
    if(req_msg->op_pco_id_valid)
    {
       payload->operator_reserved_pco = req_msg->op_pco_id;
       mask = mask|DS_PROFILE_OPERATOR_RESERVED_PCO;
    }
    if(req_msg->pco_mcc_valid)
    {
      payload->mcc = req_msg->pco_mcc;
      mask = mask|DS_PROFILE_MCC;
    }
    if(req_msg->pco_mnc_includes_pcs_digit_valid)
    {
        payload->mnc.mnc_digits = req_msg->pco_mnc_includes_pcs_digit.mnc;
        payload->mnc.mnc_includes_pcs_digit = 
               req_msg->pco_mnc_includes_pcs_digit.mnc_includes_pcs_digit;
       mask = mask|DS_PROFILE_MNC;
    }
    if(req_msg->max_pdn_conn_per_block_valid)
    {
      payload->max_pdn_conn_per_blk = req_msg->max_pdn_conn_per_block;
      mask = mask|DS_PROFILE_MAX_PDN_CONN_PER_BLOCK;
    }
    if(req_msg->max_pdn_conn_timer_valid)
    {
       payload->max_pdn_conn_time = req_msg->max_pdn_conn_timer;
       mask = mask|DS_PROFILE_MAX_PDN_CONN_TIMER;
    }
    if(req_msg->pdn_req_wait_interval_valid)
    {
       payload->pdn_req_wait_time = req_msg->pdn_req_wait_interval;
       mask = mask|DS_PROFILE_PDN_REQ_WAIT_TIMER;
    }
    if(req_msg->roaming_disallowed_valid)
    {
      payload->roaming_disallowed = req_msg->roaming_disallowed;
      mask = mask|DS_PROFILE_ROAMING_DISALLOWED;
    }
    if(req_msg->pdn_discon_wait_timer_valid)
    {
       payload->pdn_discon_wait_time = req_msg->pdn_discon_wait_timer;
       mask = mask|DS_PROFILE_PDN_DISCON_WAIT_TIME;
    }
    if(req_msg->lte_roaming_pdp_type_valid)
    {
       payload->context.lte_roaming_pdp_type = req_msg->lte_roaming_pdp_type;
      mask = mask|DS_PROFILE_PARAM_LTE_ROAMING_PDP_TYPE;
    }
    if(req_msg->umts_roaming_pdp_type_valid)
    {
       payload->context.umts_roaming_pdp_type = req_msg->umts_roaming_pdp_type;
       mask = mask|DS_PROFILE_PARAM_UMTS_ROAMING_PDP_TYPE;
    }
    if(req_msg->override_home_pdp_type_valid)
    {
       payload->context.override_home_pdp_type = req_msg->override_home_pdp_type;
       mask = mask|DS_PROFILE_PARAM_OVERRIDE_HOME_PDP_TYPE;
    }
    if(req_msg->attach_wo_pdn_valid)
    {
       payload->attach_wo_pdn = req_msg->attach_wo_pdn;
       mask = mask|DS_PROFILE_PARAM_ATTACH_WO_PDN;
    }
    if(req_msg->clat_enabled_valid)
    {
      payload->clat_enabled = req_msg->clat_enabled;
      mask = mask|DS_PROFILE_CLAT_ENABLED;
    }
    if(req_msg->failure_timer_3gpp_valid)
    {
       memscpy(payload->failure_timers,sizeof(payload->failure_timers),
               req_msg->failure_timer_3gpp,sizeof(req_msg->failure_timer_3gpp));
       mask = mask|DS_PROFILE_FAILURE_TIMERS;
    }
    if(req_msg->msisdn_flag_valid)
    {
      payload->msisdn_flag = req_msg->msisdn_flag;
      mask = mask|DS_PROFILE_MSISDN_FLAG;
    }
  } 
  return mask;
}
/*===========================================================================
  FUNCTION QMI_WDSI_CREATE_CL_SP_MAP()

  DESCRIPTION
    Api creates the wds state for the client or links the client to already allocated
      wds state

    PARAMETERS
     index  :                   Index in the wds state map
     ip_family_pref  :          Ip famliy
     clnt_info_ptr   :          Client pointer
     ep_id             :        Endpoint

  RETURN VALUE
    qmi_wds_client_state_map 

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_wds_client_state_map *qmi_wdsi_create_cl_sp_map
(
  uint8                      index,
  uint8                      ip_family_pref,
  qmi_wdsi_client_info_type *clnt_info_ptr,
  uint32                     ep_id
)
{
  boolean alloc_inst =FALSE;
  qmi_wdsi_client_info_type * client;
 /*-----------------------------------------------------------------*/ 
 DS_MSG2(MSG_LEGACY_HIGH,"qmi_wdsi_create_cl_sp_map:  index %d %d",
                           index,ip_family_pref);
  if(ip_family_pref == QMI_AF_NIPD)
  {
    if(qmi_wds_cl_sp_map[index -1] == NULL)
      alloc_inst = TRUE;  
      index = index -1;
  }
  else if(ip_family_pref == QMI_AF_INET6)
  {
    if(qmi_wds_cl_sp_map[index -2] == NULL)
      alloc_inst = TRUE;
    index = index -2;
  }
  else
  {
     if(qmi_wds_cl_sp_map[index -3] == NULL)
       alloc_inst = TRUE;
     index = index -3;
  }
  
  if(alloc_inst == TRUE)
  {
     DS_SYSTEM_HEAP_MEM_ALLOC_ASSERT(qmi_wds_cl_sp_map[index],
                              sizeof(qmi_wds_client_state_map),
                              qmi_wds_client_state_map *);

    memset(qmi_wds_cl_sp_map[index], 0, sizeof(qmi_wds_client_state_map));
    qmi_wds_cl_sp_map[index]->pkt_svc_state = 
                    WDS_CONNECTION_STATUS_DISCONNECTED_V01;
    qmi_wds_cl_sp_map[index]->ip_call_type  = ip_family_pref;
  }
  
  clnt_info_ptr->bound_ip_type = ip_family_pref;

  DS_MSG4(MSG_LEGACY_HIGH,"qmi_wdsi_create_cl_sp_map: alloc_inst %d "
          "client info %p and wds_inst %p actual index %d",alloc_inst, clnt_info_ptr,qmi_wds_cl_sp_map[index],index);

  //Check if client already exists 
   client = qmi_wds_cl_sp_map[index]->client_list;
   while(client != NULL)
   {
     if(qmi_wdsi_compare_client_handle(client ,clnt_info_ptr->clnt))
   	{
        DS_MSG2(MSG_LEGACY_ERROR,"qmi_wdsi_create_cl_sp_map: Client already"
                      "present so returning %p and %p",
                      clnt_info_ptr->clnt,client->clnt);
        
       //qmi_wds_cl_sp_map[index]->binding.ep_id = ep_id;
      return  qmi_wds_cl_sp_map[index];
    }
    client = client->next;
   }
   clnt_info_ptr->next = qmi_wds_cl_sp_map[index]->client_list;
   qmi_wds_cl_sp_map[index]->client_list = clnt_info_ptr;
   qmi_wds_cl_sp_map[index]->binding.rmnet_inst = index;
   clnt_info_ptr->wds_inst = index;
   clnt_info_ptr->net_bound = FALSE;
   qmi_wds_cl_sp_map[index]->binding.ep_id = ep_id;
   
 return  qmi_wds_cl_sp_map[index];
}

/*===========================================================================
  FUNCTION QMI_WDSI_GET_PROFILE_SETTINGS_LITE()

  DESCRIPTION
    Gets the profile settings based on the APN and IP type passed in req
    
  PARAMETERS
     req_c_struct  :            Request Message with all TLV
     req_handle  :              Request Handle
     clnt_info   :              Client Info Handle
     msg_id             :       Message ID
     req_c_struct_len   :       length of request message
     service_handle     :       wds service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_wdsi_get_profile_settings_lite
(
  qmi_wdsi_client_info_type    *clnt_info,
  qmi_req_handle                req_handle,
  int                           msg_id,
  void                         *req_c_struct,
  int                           req_c_struct_len,
  void                         *service_handle
)
{
  wds_get_profile_settings_lite_req_msg_v01        *req_msg = NULL;
  wds_get_profile_settings_lite_resp_msg_v01        resp_msg;
  qmi_error_type_v01                               errval;
  ds_profile_ident_info                            ident_info;
  ds_profile_type_s                               *payload =NULL;
  ds_profile_status_etype                          profile_status = DS_PROFILE_RESULT_SUCCESS;  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  errval         = QMI_ERR_NONE_V01;
  memset(&resp_msg,0,sizeof(resp_msg));  
  req_msg  = (wds_get_profile_settings_lite_req_msg_v01*)req_c_struct;

  /*-------------------------------------------------------------------------
    Validate the mandatory and optional parameters of profile 
   -------------------------------------------------------------------------*/
  if(req_msg->apn_name_valid)
  {
   if (strlen(req_msg->apn_name) >= (QMI_WDS_APN_NAME_MAX_V01+1))
   {
      errval = QMI_ERR_ARG_TOO_LONG_V01;
      goto send_result;
   }
  }
   
  memset(&ident_info ,0,sizeof(ds_profile_ident_info));

  DS_SYSTEM_HEAP_MEM_ALLOC(payload,
                           sizeof(ds_profile_type_s),
                           ds_profile_type_s *); 
  if (NULL == payload)
  {
    errval = QMI_ERR_NO_MEMORY_V01;
    goto send_result;
  }
  
  memset(payload,0,sizeof(ds_profile_type_s));  
  if(req_msg->apn_name_valid)		
    memscpy(ident_info.apn,DS_PROFILE_APN_STRING_LEN,req_msg->apn_name,DS_PROFILE_APN_STRING_LEN);
  
  if(req_msg->pdp_type != WDS_PDP_TYPE_PDP_PPP_V01)
  {
    ident_info.pdp_type = (ds_profile_pdp_type_e)req_msg->pdp_type;
  }
  else
  {
     errval = QMI_ERR_INVALID_PDP_TYPE_V01;
     goto send_result;
  }
  
  profile_status = ds_profile_get_param(ident_info,payload);
  if(profile_status == DS_PROFILE_RESULT_SUCCESS)
  {     
   if(payload->mask & DS_PROFILE_TYPE)
   {
     resp_msg.pdp_type = (wds_pdp_type_enum_v01)payload->context.pdp_type;
     resp_msg.pdp_type_valid = TRUE;
   }
   if(payload->mask & DS_PROFILE_APN_NAME)
   {
     strlcpy(resp_msg.apn_name,payload->context.apn,sizeof(payload->context.apn));
     resp_msg.apn_name_valid = TRUE;
   }
   if(payload->mask & DS_PROFILE_CONTEXT_H_COMP)
   {
     resp_msg.pdp_hdr_compression_type = 
              (wds_pdp_hdr_compr_type_enum_v01)payload->context.h_comp; 
    resp_msg.pdp_hdr_compression_type_valid = TRUE;
   }
   if(payload->mask & DS_PROFILE_CONTEXT_D_COMP)
   {
     resp_msg.pdp_data_compression_type = 
              (wds_pdp_data_compr_type_enum_v01)payload->context.d_comp; 
    resp_msg.pdp_data_compression_type_valid = TRUE;
   }

   if(payload->mask & DS_PROFILE_DNS_ADDR_V4_PRIMARY)
   {
     resp_msg.primary_DNS_IPv4_address_preference =
                payload->dns_addr.primary_dns_addr.pdp_addr.pdp_addr_ipv4; 
    resp_msg.primary_DNS_IPv4_address_preference_valid = TRUE;
   }
   if(payload->mask & DS_PROFILE_DNS_ADDR_V4_SECONDARY)
   {
     resp_msg.secondary_DNS_IPv4_address_preference = 
              payload->dns_addr.secondary_dns_addr.pdp_addr.pdp_addr_ipv4; 
    resp_msg.secondary_DNS_IPv4_address_preference_valid = TRUE;
   }
   if(payload->mask & DS_PROFILE_AUTH_USERNAME)
   {
     strlcpy(resp_msg.username ,payload->auth.username,
            (QMI_WDS_USER_NAME_MAX_V01+1));
    resp_msg.username_valid = TRUE;
   }

   if(payload->mask & DS_PROFILE_AUTH_TYPE)
   {
    resp_msg.authentication_preference = payload->auth.auth_type;
    resp_msg.authentication_preference_valid = TRUE;
   }
   if(payload->mask & DS_PROFILE_PCFCH_ADDRESS_USING_DHCP_FLAG)
   {
    resp_msg.pcscf_addr_using_pco = payload->request_pcscf_address_flag;
    resp_msg.pcscf_addr_using_pco_valid = TRUE;
   }
   if(payload->mask & DS_PROFILE_ACCESS_CTRL_FLAG)
   {
     resp_msg.pdp_access_control_flag = 
            (wds_pdp_access_control_enum_v01)payload->context.access_ctrl_flag;
    resp_msg.pdp_access_control_flag_valid = TRUE;
   }
   if(payload->mask & DS_PROFILE_IM_CN_FLAG)
   {
    resp_msg.im_cn_flag = payload->im_cn_flag;
    resp_msg.im_cn_flag_valid = TRUE;
   }
   if(payload->mask & DS_PROFILE_DNS_ADDR_V6_PRIMARY)
   {
     memscpy(resp_msg.primary_dns_ipv6_address_preference,
             sizeof(struct ps_in6_addr),
            (void *)&payload->dns_addr.primary_dns_addr.\
             pdp_addr.pdp_addr_ipv6.in6_u,sizeof(struct ps_in6_addr));
    resp_msg.primary_dns_ipv6_address_preference_valid = TRUE;
   }
   if(payload->mask & DS_PROFILE_DNS_ADDR_V6_SECONDARY)
   {
     memscpy(resp_msg.secondary_dns_ipv6_address_preference,
             sizeof(struct ps_in6_addr),
            (void *)&payload->dns_addr.secondary_dns_addr.\
             pdp_addr.pdp_addr_ipv6.in6_u,
            sizeof(struct ps_in6_addr));
														 
    resp_msg.secondary_dns_ipv6_address_preference_valid = TRUE;
   }
   if(payload->mask & DS_PROFILE_APN_DISABLE_FLAG)
   {
    resp_msg.apn_disabled_flag = payload->apn_disable_flag;
    resp_msg.apn_disabled_flag_valid = TRUE;
   }
   if(payload->mask & DS_PROFILE_PDN_INACTIVITY_TIMEOUT)
   {
    resp_msg.pdn_inactivity_timeout = payload->inactivity_timer_val;
    resp_msg.pdn_inactivity_timeout_valid = TRUE;
   }
   if(payload->mask & DS_PROFILE_APN_CLASS)
   {
    resp_msg.apn_class = payload->apn_class;
    resp_msg.apn_class_valid = TRUE;
   }
   if(payload->mask & DS_PROFILE_APN_BEARER)
   {
      if (payload->apn_bearer == 0xFF)
      {
        resp_msg.apn_bearer = WDS_MASK_APN_BEARER_ANY_V01;
      }
      else
      {
        resp_msg.apn_bearer = payload->apn_bearer;
      }
      resp_msg.apn_bearer_valid = TRUE;
   }
   if(payload->mask & DS_PROFILE_OPERATOR_RESERVED_PCO)
   {
    resp_msg.op_pco_id = payload->operator_reserved_pco;
    resp_msg.op_pco_id_valid = TRUE;
   }
   if(payload->mask & DS_PROFILE_MCC)
   {
    resp_msg.pco_mcc = payload->mcc;
    resp_msg.pco_mcc_valid = TRUE;
   }
   if(payload->mask & DS_PROFILE_MNC)
   {
    resp_msg.pco_mnc_includes_pcs_digit.mnc = payload->mnc.mnc_digits;
     resp_msg.pco_mnc_includes_pcs_digit.mnc_includes_pcs_digit = 
                    payload->mnc.mnc_includes_pcs_digit;
    resp_msg.pco_mnc_includes_pcs_digit_valid = TRUE;
   }
   if(payload->mask & DS_PROFILE_MAX_PDN_CONN_PER_BLOCK)
   {
    resp_msg.max_pdn_conn_per_block = payload->max_pdn_conn_per_blk;
    resp_msg.max_pdn_conn_per_block_valid = TRUE;
   }
   if(payload->mask & DS_PROFILE_MAX_PDN_CONN_TIMER)
   {
    resp_msg.max_pdn_conn_timer = payload->max_pdn_conn_time;
    resp_msg.max_pdn_conn_timer_valid = TRUE;
   }
   if(payload->mask & DS_PROFILE_PDN_REQ_WAIT_TIMER)
   {
    resp_msg.pdn_req_wait_interval = payload->pdn_req_wait_time;
    resp_msg.pdn_req_wait_interval_valid = TRUE;
   }
   if(payload->mask & DS_PROFILE_ROAMING_DISALLOWED)
   {
    resp_msg.roaming_disallowed = payload->roaming_disallowed;
    resp_msg.roaming_disallowed_valid = TRUE;
   }
   if(payload->mask & DS_PROFILE_PDN_DISCON_WAIT_TIME)
   {
    resp_msg.pdn_discon_wait_timer = payload->pdn_discon_wait_time;
    resp_msg.pdn_discon_wait_timer_valid = TRUE;
   }
   if(payload->mask & DS_PROFILE_PARAM_LTE_ROAMING_PDP_TYPE)
   {
     resp_msg.lte_roaming_pdp_type = 
          (wds_common_pdp_type_enum_v01)payload->context.lte_roaming_pdp_type;
    resp_msg.lte_roaming_pdp_type_valid = TRUE;
   }
   if(payload->mask & DS_PROFILE_PARAM_UMTS_ROAMING_PDP_TYPE)
   {
     resp_msg.umts_roaming_pdp_type = 
         (wds_common_pdp_type_enum_v01)payload->context.umts_roaming_pdp_type;
    resp_msg.umts_roaming_pdp_type_valid = TRUE;
   }
   if(payload->mask & DS_PROFILE_PARAM_OVERRIDE_HOME_PDP_TYPE)
   {
     resp_msg.override_home_pdp_type = 
         (wds_common_pdp_type_enum_v01)payload->context.override_home_pdp_type;
    resp_msg.override_home_pdp_type_valid = TRUE;
   }
   if(payload->mask & DS_PROFILE_PARAM_ATTACH_WO_PDN)
   {
    resp_msg.attach_wo_pdn = payload->attach_wo_pdn;
    resp_msg.attach_wo_pdn_valid = TRUE;
   }
   if(payload->mask & DS_PROFILE_CLAT_ENABLED)
   {
    resp_msg.clat_enabled = payload->clat_enabled;
    resp_msg.clat_enabled_valid = TRUE;
   }
   resp_msg.persistent_valid = TRUE;
   resp_msg.persistent = (payload->is_non_persistent)?FALSE:TRUE;
   
   if(payload->mask & DS_PROFILE_FAILURE_TIMERS)
   {
      memscpy(resp_msg.failure_timer_3gpp,sizeof(resp_msg.failure_timer_3gpp), 

	          payload->failure_timers,sizeof(payload->failure_timers));
	  resp_msg.failure_timer_3gpp_valid = TRUE;
    }
    if(payload->mask & DS_PROFILE_MSISDN_FLAG)
    {
      resp_msg.msisdn_flag = payload->msisdn_flag;
      resp_msg.msisdn_flag_valid = TRUE;
    }
  }
  else
    goto send_result;

  send_result:
  if (profile_status != DS_PROFILE_RESULT_SUCCESS)
  {
    resp_msg.extended_error_code_valid = TRUE;
    resp_msg.extended_error_code = 
           qmi_wdsi_map_profile_error(profile_status);
    errval = QMI_ERR_EXTENDED_INTERNAL_V01;
  }
  
  resp_msg.resp.result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01
                                   : QMI_RESULT_FAILURE_V01);
  resp_msg.resp.error  = errval; 
  DS_SYSTEM_HEAP_MEM_FREE(payload);
  DS_MSG2(MSG_LEGACY_MED,"qmi_wdsi_get_profile_settings result %d errval %d",
                   resp_msg.resp.result, errval);
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(wds_get_profile_settings_lite_resp_msg_v01));
 return QMI_CSI_CB_NO_ERR;
}

static wds_ds_extended_error_code_enum_v01 qmi_wdsi_map_profile_error
(
  ds_profile_status_etype profile_status
)
{
  DS_MSG1(MSG_LEGACY_MED,"qmi_wdsi_map_profile_error profile_status %d",profile_status);
  if(profile_status == DS_PROFILE_RESULT_FAIL)
    return WDS_EEC_DS_PROFILE_REG_RESULT_FAIL_V01;
  else if (profile_status == DS_PROFILE_RESULT_ERR_INVAL_IDENT)
    return WDS_EEC_DS_PROFILE_REG_RESULT_ERR_INVAL_IDENT_V01;
  else if (profile_status == DS_PROFILE_RESULT_ERR_INVAL)
    return WDS_EEC_DS_PROFILE_REG_RESULT_ERR_INVAL_V01;
  else if (profile_status == DS_PROFILE_RESULT_ERR_OUT_OF_PROFILES)
   return WDS_EEC_DS_PROFILE_REG_RESULT_ERR_OUT_OF_PROFILES_V01;
  else if (profile_status == DS_PROFILE_RESULT_ERR_OUT_OF_MEMORY)
   return WDS_EEC_DS_PROFILE_REG_RESULT_ERR_OUT_OF_MEMORY_V01;
  else if(profile_status == DS_PROFILE_RESULT_ERR_FILE_ACCESS)
   return WDS_EEC_DS_PROFILE_REG_RESULT_ERR_FILE_ACCESS_V01;
  else if (profile_status == DS_PROFILE_RESULT_ERR_EOF)
   return WDS_EEC_DS_PROFILE_REG_RESULT_ERR_EOF_V01;
  else if (profile_status == DS_PROFILE_ALREADY_PRESENT)
   return WDS_EEC_DS_PROFILE_REG_RESULT_PROFILE_ALREADY_PRESENT_V01;
  else 
   return WDS_EEC_DS_PROFILE_REG_RESULT_FAIL_V01;
}
/*===========================================================================
  FUNCTION QMI_WDSI_CREATE_PROFILE_LITE()

  DESCRIPTION
    Create a Data profile

  PARAMETERS
   req_c_struct       :       Request Message with all TLV
   req_handle  : Request Handle
   clnt_info   : Client Info Handle
   msg_id             :       Message ID
   req_c_struct_len   :       length of request message
   service_handle     :       wds service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_wdsi_create_profile_lite
(
  qmi_wdsi_client_info_type       *clnt_info,
  qmi_req_handle                   req_handle,
  int                              msg_id,
  void                            *req_c_struct,
  int                              req_c_struct_len,
  void                            *service_handle
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  wds_create_profile_lite_req_msg_v01         *req_msg = NULL;
  wds_create_profile_lite_resp_msg_v01         resp_msg;
  qmi_error_type_v01                         errval;
  ds_profile_ident_info                      ident_info;
  ds_profile_type_s                         *payload =NULL;
  uint64                                     mask =0;
  ds_profile_status_etype                    profile_status = DS_PROFILE_RESULT_SUCCESS;  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  errval         = QMI_ERR_NONE_V01;
  memset(&resp_msg,0,sizeof(resp_msg));    
  req_msg  = (wds_create_profile_lite_req_msg_v01*)req_c_struct;

  /*-------------------------------------------------------------------------
    Validate the mandatory and optional parameters of profile 
   -------------------------------------------------------------------------*/
   if(req_msg->apn_name_valid)
   {
    if (strlen(req_msg->apn_name) >= (QMI_WDS_APN_NAME_MAX_V01+1))
    {
       errval = QMI_ERR_ARG_TOO_LONG_V01;
       goto send_result;
    }
   }	

   if(req_msg->username_valid)
   {
    if(strlen(req_msg->username) >= (QMI_WDS_USER_NAME_MAX_V01+1))
    {
      errval = QMI_ERR_ARG_TOO_LONG_V01;
      goto send_result;
    }
   }

  if(req_msg->password_valid)
  { 
    if(strlen(req_msg->password) >= (QMI_WDS_PASSWORD_MAX_V01+1))
    {
      errval = QMI_ERR_ARG_TOO_LONG_V01;
      goto send_result;
    }
  }
 
  /*-------------------------------------------------------------------------
    Create the mask for the profile TLV to create profile
  -------------------------------------------------------------------------*/
  memset(&ident_info ,0,sizeof(ds_profile_ident_info));
  DS_SYSTEM_HEAP_MEM_ALLOC(payload,
                           sizeof(ds_profile_type_s),
                           ds_profile_type_s *); 
  
  if (NULL == payload)
  {
    errval = QMI_ERR_NO_MEMORY_V01;
    goto send_result;
  }
  
  memset(payload,0,sizeof(ds_profile_type_s));  
  if(req_msg->apn_name_valid)		
    memscpy(ident_info.apn,DS_PROFILE_APN_STRING_LEN,req_msg->apn_name,DS_PROFILE_APN_STRING_LEN);
  if(req_msg->pdp_type != WDS_PDP_TYPE_PDP_PPP_V01)
  {
    ident_info.pdp_type = (ds_profile_pdp_type_e)req_msg->pdp_type;
  }
  else
  {
     errval = QMI_ERR_INVALID_PDP_TYPE_V01;
     goto send_result;
  }
  
  mask = qmi_wdsi_get_profile_mask(req_msg,
                                  payload,
                                 &ident_info,
                                  WDSI_PROFILE_CREATE
                                  );
  if(req_msg->persistent_valid)
  {
    payload->is_non_persistent = (req_msg->persistent)?FALSE:TRUE;
  }
  /*-------------------------------------------------------------------------
    Create the new profile based on the profile ident and the mask which 
    is been retrueved from the get_profile_mask 
    The payload contains the TLV values in the DS profile format
    The ident_info and payload is freed in success as well as fail case.
  -------------------------------------------------------------------------*/
   profile_status = ds_profile_create(ident_info,
                                      mask,
                                      payload
                                     );
send_result:
  if (profile_status != DS_PROFILE_RESULT_SUCCESS)
  {
    resp_msg.extended_error_code_valid = TRUE;
    resp_msg.extended_error_code = 
           qmi_wdsi_map_profile_error(profile_status);
    errval  = QMI_ERR_EXTENDED_INTERNAL_V01;
  }
  resp_msg.resp.result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01
                                   : QMI_RESULT_FAILURE_V01);
  resp_msg.resp.error  = errval; 
  DS_SYSTEM_HEAP_MEM_FREE(payload);
  DS_MSG2(MSG_LEGACY_MED,"qmi_wdsi_create_profile_lite result %d errval %d",
                   resp_msg.resp.result, errval);
  
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(wds_create_profile_lite_resp_msg_v01));
  
  return QMI_CSI_CB_NO_ERR;
} /* qmi_wdsi_create_profile() */
/*===========================================================================
  FUNCTION QMI_WDSI_MODIFY_PROFILE_SETTINGS_LITE()

  DESCRIPTION 
    Modifies the parameters of an already configured profile based on APN and IP type

  PARAMETERS
    req_c_struct       :       Request Message with all TLV
    req_handle         :       Request Handle
    clnt_info          :       Client Info Handle
    msg_id             :       Message ID
    req_c_struct_len   :       length of request message
    service_handle     :       wds service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    qmi_wds_init has to be called.

  SIDE EFFECTS
      None
===========================================================================*/

static qmi_csi_cb_error qmi_wdsi_modify_profile_settings_lite
(
   qmi_wdsi_client_info_type	              *clnt_info,
   qmi_req_handle                              req_handle,
   int                                         msg_id,
   void                                       *req_c_struct,
   int                                         req_c_struct_len,
   void                                       *service_handle
)
{
  wds_modify_profile_settings_lite_req_msg_v01  *req_msg;
  wds_modify_profile_settings_lite_resp_msg_v01  resp_msg;
  qmi_error_type_v01                            errval;  
  ds_profile_ident_info                         ident_info;
  ds_profile_type_s                            *payload = NULL;
  uint64                                        mask =0; 
  ds_profile_status_etype                       profile_status = DS_PROFILE_RESULT_SUCCESS;  
   
/*----------------------------------------------------------------------------*/  
   errval= QMI_ERR_NONE_V01;
   memset(&resp_msg,0,sizeof(resp_msg));  

   req_msg= (wds_modify_profile_settings_lite_req_msg_v01*)req_c_struct;
  
   /*-------------------------------------------------------------------------
      Validate the mandatory and optional parameters of profile 
  -------------------------------------------------------------------------*/
  if(req_msg->apn_name_valid)
  {
    if (strlen(req_msg->apn_name) >= (QMI_WDS_APN_NAME_MAX_V01+1))
    {
       errval = QMI_ERR_ARG_TOO_LONG_V01;
       goto send_result;
    }
  }
  
  if(req_msg->username_valid)
  {
   if(strlen(req_msg->username) >= (QMI_WDS_USER_NAME_MAX_V01+1))
   {
     errval = QMI_ERR_ARG_TOO_LONG_V01;
     goto send_result;
   }
  }
  
  if(req_msg->password_valid)
  { 
    if(strlen(req_msg->password) >= (QMI_WDS_PASSWORD_MAX_V01+1))
    {
      errval = QMI_ERR_ARG_TOO_LONG_V01;
      goto send_result;
    }
  }
 
  /*-------------------------------------------------------------------------
    Create the mask for the profile TLV to create profile
  -------------------------------------------------------------------------*/

  memset(&ident_info ,0,sizeof(ds_profile_ident_info));
  
  DS_SYSTEM_HEAP_MEM_ALLOC(payload,
                           sizeof(ds_profile_type_s),
                           ds_profile_type_s *); 
  
  if (NULL == payload)
  {
    errval = QMI_ERR_NO_MEMORY_V01; 
    goto send_result;
  }
  
  memset(payload,0,sizeof(ds_profile_type_s));  
  if(req_msg->apn_name_valid)
    memscpy(ident_info.apn,DS_PROFILE_APN_STRING_LEN,req_msg->apn_name,DS_PROFILE_APN_STRING_LEN);

  if(req_msg->pdp_type != WDS_PDP_TYPE_PDP_PPP_V01)
  {
    ident_info.pdp_type = (ds_profile_pdp_type_e)req_msg->pdp_type;
  }
  else
  {
     errval = QMI_ERR_INVALID_PDP_TYPE_V01;
     goto send_result;
  }

  mask = qmi_wdsi_get_profile_mask(req_msg,
                                  payload,
                                 &ident_info,
                                  WDSI_PROFILE_MODIFY
                                 );
 /*-------------------------------------------------------------------------
    Create the new profile based on the profile ident and the mask which 
    is been retrueved from the get_profile_mask 
    The payload contains the TLV values in the DS profile format
    The ident_info and payload is freed in success as well as fail case.
  -------------------------------------------------------------------------*/

 profile_status =  ds_profile_modify(ident_info,
                                     mask,
                                     payload
                                     );
 
  DS_MSG2(MSG_LEGACY_MED,"qmi_wdsi_create_profile result %d errval %d",profile_status,0);
send_result: 
  if (profile_status != DS_PROFILE_RESULT_SUCCESS)
  {
    resp_msg.extended_error_code_valid = TRUE;
    resp_msg.extended_error_code = 
           qmi_wdsi_map_profile_error(profile_status);
    errval = QMI_ERR_EXTENDED_INTERNAL_V01;
  }
  
  resp_msg.resp.result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01
                                   : QMI_RESULT_FAILURE_V01);
  resp_msg.resp.error  = errval;
  
  DS_SYSTEM_HEAP_MEM_FREE(payload);
  DS_MSG2(MSG_LEGACY_MED,"qmi_wdsi_modify_profile_settings_lite result %d errval %d",
                   resp_msg.resp.result, errval);
  
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(wds_modify_profile_settings_lite_resp_msg_v01));
	return QMI_CSI_CB_NO_ERR;


}
/*===========================================================================
  FUNCTION QMI_WDSI_DELETE_PROFILE_LITE()

  DESCRIPTION
    Delete the given profile with specified apn namme

  PARAMETERS
    req_c_struct       :       Request Message with all TLV
    req_handle         :       Request Handle
    clnt_info          :       Client Info Handle
    msg_id             :       Message ID
    req_c_struct_len   :       length of request message
    service_handle     :       wds service handle

  RETURN VALUE
    qmi_csi_cb_error types.

DEPENDENCIES
   qmi_wds_init has to be called

SIDE EFFECTS
    None
===========================================================================*/

static qmi_csi_cb_error  qmi_wdsi_delete_profile_lite
(
  qmi_wdsi_client_info_type        *clnt_info,
  qmi_req_handle                    req_handle,
  int                               msg_id,
  void                             *req_c_struct,
  int                               req_c_struct_len,
  void                             *service_handle
)
{
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  qmi_error_type_v01                  errval;
  wds_delete_profile_lite_req_msg_v01  *req_msg;
  wds_delete_profile_lite_resp_msg_v01  resp_msg;
  ds_profile_status_etype             profile_status = DS_PROFILE_RESULT_SUCCESS;   
  ds_profile_ident_info               ident_info;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  errval = QMI_ERR_NONE_V01;	
  memset(&resp_msg,0,sizeof(wds_delete_profile_lite_resp_msg_v01));
  req_msg  = (wds_delete_profile_lite_req_msg_v01*)req_c_struct;

  /*--------------------------------------------------------------------------
     Validate the mandatory and optional parameters of profile 
  -------------------------------------------------------------------------*/
   if(req_msg->apn_name_valid)
   {
     if (strlen(req_msg->apn_name) >= (QMI_WDS_APN_NAME_MAX_V01+1))
     {
        errval = QMI_ERR_ARG_TOO_LONG_V01;
        goto send_result;
     }
   }
  /*-------------------------------------------------------------------------
     Call the DS Profile API to reset the profile to default values
  -------------------------------------------------------------------------*/
  
  memset(&ident_info ,0,sizeof(ds_profile_ident_info));
   
  if(req_msg->apn_name_valid)
    memscpy(ident_info.apn,DS_PROFILE_APN_STRING_LEN,req_msg->apn_name,DS_PROFILE_APN_STRING_LEN);
  
  if(req_msg->pdp_type != WDS_PDP_TYPE_PDP_PPP_V01)
  {
    ident_info.pdp_type = (ds_profile_pdp_type_e)req_msg->pdp_type;
  }
  else
  {
     errval = QMI_ERR_INVALID_PDP_TYPE_V01;
     goto send_result;
  }
  
  profile_status = ds_profile_delete(ident_info);
  if (profile_status != DS_PROFILE_RESULT_SUCCESS)
  {
    DS_MSG3(MSG_LEGACY_ERROR,"Cannot delete profile (%d) pdp (%d) Error (%d)",
                     0, req_msg->pdp_type, profile_status);
    errval = QMI_ERR_EXTENDED_INTERNAL_V01;
    goto send_result;
  }
send_result:
  resp_msg.resp.result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01
                                          : QMI_RESULT_FAILURE_V01);
  resp_msg.resp.error  = errval;
  
  if (profile_status != DS_PROFILE_RESULT_SUCCESS)
  {
    resp_msg.extended_error_code_valid = TRUE;
    resp_msg.extended_error_code = 
           qmi_wdsi_map_profile_error(profile_status);
  }
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(wds_delete_profile_lite_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION QMI_WDSI_GET_PROFILE_LIST_LITE()

  DESCRIPTION
     Creates a data profile which is used for calls
     
  PARAMETERS
     req_c_struct  :            Request Message with all TLV
     req_handle  :              Request Handle
     clnt_info   :              Client Info Handle
     msg_id             :       Message ID
     req_c_struct_len   :       length of request message
     service_handle     :       wds service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_wdsi_get_profile_list_lite
(
  qmi_wdsi_client_info_type        *clnt_info,
  qmi_req_handle                    req_handle,
  int                               msg_id,
  void                             *req_c_struct,
  int                               req_c_struct_len,
  void                             *service_handle
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  wds_get_profile_list_lite_req_msg_v01         *req_msg = NULL;
  wds_get_profile_list_lite_resp_msg_v01         *resp_msg;
  qmi_error_type_v01                             errval;
  ds_profile_list_stype                         *profile_list =NULL;
  uint8                                          profile_count =0;
  uint8                                          i;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  errval = QMI_ERR_NONE_V01;    
  req_msg  = (wds_get_profile_list_lite_req_msg_v01*)req_c_struct;
  DS_SYSTEM_HEAP_MEM_ALLOC(resp_msg,
                            sizeof(wds_get_profile_list_lite_resp_msg_v01),
                            wds_get_profile_list_lite_resp_msg_v01*);
  if(resp_msg == NULL)
  {
    DS_ASSERT(0);
  }
  else
  {
    memset(resp_msg,0,sizeof(wds_get_profile_list_lite_resp_msg_v01));
  }
   
  DS_SYSTEM_HEAP_MEM_ALLOC(profile_list, (sizeof(ds_profile_list_stype)*DS_PROFILE_MAX_PROFILE_NUM),
                                        ds_profile_list_stype*);
 
  if(profile_list ==NULL)
  {
    DS_ASSERT(0);
  }
  memset(profile_list,0,(sizeof(ds_profile_list_stype)*DS_PROFILE_MAX_PROFILE_NUM));
  profile_count = ds_profile_get_profile_list(profile_list);
  if(profile_count == 0)
  {
      //Return error
      resp_msg->profile_list_len = profile_count;
      goto send_result;
  }
 
  resp_msg->profile_list_len = profile_count;
  resp_msg->profile_list_valid = TRUE;
  for(i =0 ;i<profile_count ;i++)
  { 
    if((wds_pdp_type_enum_v01)profile_list[i].pdp_type != WDS_PDP_TYPE_PDP_PPP_V01)
    {
      resp_msg->profile_list[i].pdp_type = (wds_pdp_type_enum_v01)profile_list[i].pdp_type;
    }
    else
    {
       errval = QMI_ERR_INVALID_PDP_TYPE_V01;
       goto send_result;
    }  
    if(profile_list[i].apn[0] !='\0')
    {
      memscpy(&(resp_msg->profile_list[i].apn_name), DS_PROFILE_APN_STRING_LEN+1, 
               profile_list[i].apn, sizeof(profile_list[i].apn ));
     }
  }
   
  send_result:  
  DS_SYSTEM_HEAP_MEM_FREE(profile_list);      
  resp_msg->resp.result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01
                                          : QMI_RESULT_FAILURE_V01);
  resp_msg->resp.error  = errval;
  qmi_csi_send_resp(req_handle, msg_id, resp_msg, 
                    sizeof(wds_get_profile_list_lite_resp_msg_v01));
   
  DS_SYSTEM_HEAP_MEM_FREE(resp_msg);      

  return QMI_CSI_CB_NO_ERR;
}
/*===========================================================================
   FUNCTION QMI_WDSI_ABORT()

   DESCRIPTION
   Abort an outstanding WDS command specified by transaction ID

   PARAMETERS
   req_c_struct       :       Request Message with all TLV
   req_handle         :       Request Handle
   clnt_info          :       Client Info Handle
   msg_id             :       Message ID
   req_c_struct_len   :       length of request message
   service_handle     :       wds service handle

   RETURN VALUE
   qmi_csi_cb_error types.
  
   DEPENDENCIES
     None
  
   SIDE EFFECTS
     None
===========================================================================*/
 static qmi_csi_cb_error  qmi_wdsi_abort
(
   qmi_wdsi_client_info_type        *clnt_info,
   qmi_req_handle                    req_handle,
   int                               msg_id,
   void                             *req_c_struct,
   int                               req_c_struct_len,
   void                             *service_handle 
)
{
  qmi_error_type_v01            errval;
  wds_abort_resp_msg_v01        resp_msg;
  qmi_wds_client_state_map     *wdsi_sp;
  boolean                       pending_resp = FALSE;
  rmnet_smi_info_type          *rm_info = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_ASSERT(clnt_info && service_handle);
  memset(&resp_msg,0,sizeof(wds_abort_resp_msg_v01));	
  errval      = QMI_ERR_NONE_V01;

  wdsi_sp = qmi_get_wds_state(clnt_info);  
  if(wdsi_sp  ==NULL)
  {
     errval = QMI_ERR_INTERNAL_V01;
     goto send_result;
  }
   /*-------------------------------------------------------------------------
     Check if the net_bound is true or not to check if the rmnet call was
     initaited 
   -------------------------------------------------------------------------*/
  if(clnt_info->net_bound == TRUE)
  {
     DS_MSG1(MSG_LEGACY_MED,"Found pending SNI with in the client %p",
                     clnt_info);
     
      /*Check state of the RMNET if it is in EP_CONFIG 
        then don't abort the call*/
      rm_info = (rmnet_smi_info_type *)rmnet_get_smi_info_ptr
                                       (
                                         wdsi_sp->binding.rmnet_inst
                                       ); 
      if((rm_info != NULL)&& 
         (rm_info->meta_sm.state == RMNET_META_SM_EP_CONFIGURING_STATE)) 
      { 
        DS_MSG0(MSG_LEGACY_HIGH,"Cannot abort the call as command already is" 
                                "given for A2 to open ports "); 
        errval = QMI_ERR_UNABORTABLE_TRANSACTION_V01; 
        goto send_result; 
      }

     errval = qmi_wdsi_stop_net_iface_client(
               (qmi_wdsi_client_info_type *)clnt_info,
                WDSI_FORCE_REASON_ABORT,wdsi_sp,&pending_resp,NULL);
  }
  else
  {
     DS_MSG0(MSG_LEGACY_MED,"No SNI found!");
     errval = QMI_ERR_OUT_OF_CALL_V01;
     goto send_result;
  }

send_result:
    resp_msg.resp.result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01
                                          : QMI_RESULT_FAILURE_V01);
    resp_msg.resp.error  = errval;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(wds_abort_resp_msg_v01));
   return QMI_CSI_CB_NO_ERR;
} /* qmi_wdsi_abort() */

/*===========================================================================
  FUNCTION QMI_WDSI_GET_COMPANION_IP_TYPE_CALL()

  DESCRIPTION
    Delete the given profile with specified apn namme

  PARAMETERS
    apn_name       :           Apn name  
    apn_length         :       Length of the apn name

    ip_type          :         Ip version

  RETURN VALUE
   ds_fwk_clnt_handle_type

DEPENDENCIES
   qmi_wds_init has to be called

SIDE EFFECTS
    None
===========================================================================*/

ds_fwk_clnt_handle_type qmi_wdsi_get_companion_ip_type_call
(
  char * apn_name,
  uint8  apn_length,
  uint8  ip_type
)
{
  ds_policy_info_type      policy_info ={0};
  ds_fwk_clnt_handle_type  client_handle = DS_FWK_INVALID_CLNT_HANDLE;
  /*-----------------------------------------------------------------------------*/
  strlcpy(policy_info.name , apn_name, apn_length+1);
  policy_info.length = apn_length;

  DS_MSG_SPRINTF_3(MSG_LEGACY_HIGH,"qmi_wdsi_get_companion_ip_type_call"
                   "APN Name: %s, APN Length %d IP Type %d",
                    policy_info.name, apn_length,ip_type);
  
  if(ip_type == QMI_AF_INET)
  {
    //Check the call if it is present with QMI_AF_INET6 or QMI_AF_NIPD handles    
    policy_info.family = IPV6_ADDR;
    client_handle = ds_fwk_look_up_pdn_by_policy(DS_FWK_CLIENT_RMNET, 
                                                 policy_info);
    if(client_handle != DS_FWK_INVALID_CLNT_HANDLE)
    {
      DS_MSG0(MSG_LEGACY_MED,"Found a companion IPV6 call");
    }
	else
    {
      policy_info.family = NON_IP_ADDR;
      client_handle = ds_fwk_look_up_pdn_by_policy(DS_FWK_CLIENT_RMNET,
                                                   policy_info);
    }
  }
  else if(ip_type == QMI_AF_INET6)
  {
    //Check the call if it is present with QMI_AF_INET or QMI_AF_NIPD handles    
    policy_info.family = IPV4_ADDR;
    client_handle = ds_fwk_look_up_pdn_by_policy(DS_FWK_CLIENT_RMNET,
                                                 policy_info);
	if(client_handle != DS_FWK_INVALID_CLNT_HANDLE)
    {
      DS_MSG0(MSG_LEGACY_MED,"Found a companion IPV4 call");
    }
	else
    {
      policy_info.family = NON_IP_ADDR;
      client_handle = ds_fwk_look_up_pdn_by_policy(DS_FWK_CLIENT_RMNET,
                                                   policy_info);
    }
  }
  else if(ip_type == QMI_AF_NIPD)
  {
    //Check the call if it is present with QMI_AF_INET or QMI_AF_INET6 handles    
    policy_info.family = IPV4_ADDR;
    client_handle = ds_fwk_look_up_pdn_by_policy(DS_FWK_CLIENT_RMNET,
                                                 policy_info);
	if(client_handle != DS_FWK_INVALID_CLNT_HANDLE)
    {
      DS_MSG0(MSG_LEGACY_MED,"Found a companion NIPD call");
    }
	else
    {
      policy_info.family = IPV6_ADDR;
     client_handle = ds_fwk_look_up_pdn_by_policy(DS_FWK_CLIENT_RMNET,
                                                  policy_info);
    }
  } 
  if(client_handle != DS_FWK_INVALID_CLNT_HANDLE)
     DS_MSG2(MSG_LEGACY_HIGH,"Found a companion call for v%d call,"
                            " client handle %d",ip_type,client_handle);
  return client_handle;
}

/*===========================================================================

  FUNCTION QMI_WDSI_VALIDATE_IP_FAMILY_PREF()

  DESCRIPTION
    Validate the IP family preferences

  PARAMETERS
    ip_family_pref - IP pref

  RETURN VALUE
    QMI_ERR_NONE_v01: If valid IP preference.
    QMI_ERR_INVALID_IP_FAMILY_PREF_v01: If invalid IP preference.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_type_v01 qmi_wdsi_validate_ip_family_pref
(
  uint8  ip_family_pref
)
{
  switch (ip_family_pref)
  {
    case QMI_AF_NIPD:          /*IP NIPD */
    case QMI_AF_INET:         /* IPv4        */
    case QMI_AF_INET6:        /* IPv6        */
      return QMI_ERR_NONE_V01;    
    case QMI_AF_UNSPEC:       /* UNSPECIFIED */
    default:
      DS_MSG1(MSG_LEGACY_MED,"Invalid value of IP family preference:%d ",
                      ip_family_pref);
      return QMI_ERR_INVALID_IP_FAMILY_PREF_V01;
  }
} /* qmi_wdsi_validate_ip_family_pref */

/*===========================================================================
  FUNCTION QMI_WDSI_GEN_START_NETWORK_INTERFACE_RESPONSE()

  DESCRIPTION
   Generates the SNI response to the clients
   
  PARAMETERS
     errval  :                  Errval 
     req_handle  :              Request Handle
     clnt_info   :              Client Info Handle
     msg_id             :       Message ID
     muxid   :                 Muxid of the call 
     call_end_reason            Call end reason 
     call_end_reason_verbose verbose reason

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

static qmi_csi_cb_error wdsi_gen_start_network_interface_response
(
  qmi_error_type_v01              errval,
  qmi_req_handle                  req_handle,
  int                             msg_id,
  qmi_wds_call_end_reason_e_type  call_end_reason,
  uint32                          call_end_reason_verbose,
  uint8                           mux_id,
  void *                          pkt_data_handle
)
{
  wds_start_network_interface_resp_msg_v01 resp_msg;
  memset(&resp_msg,0,sizeof(resp_msg));
  DS_MSG1(MSG_LEGACY_ERROR,"start network response the req_handle is :%p ",
           req_handle);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( errval == QMI_ERR_CALL_FAILED_V01)
  {
     if ( (call_end_reason_verbose ==
           DS_NET_DOWN_REASON_INTERNAL_CALL_ALREADY_PRESENT) &&
           (mux_id > 0) )
     {
        resp_msg.mux_id_valid = TRUE;
        resp_msg.mux_id       = mux_id;
        /* resp_msg.ep_id_valid  = 1;
        resp_msg.ep_id   = (net_down_ep_id & 0xFF000000) >> 24;*/
        resp_msg.pkt_data_handle = (uint32)pkt_data_handle;
     }
     resp_msg.verbose_call_end_reason_valid = TRUE;
     resp_msg.verbose_call_end_reason.call_end_reason_type = 
               (call_end_reason_verbose >> 16);
     resp_msg.verbose_call_end_reason.call_end_reason = 
                  (0x0000FFFF&call_end_reason_verbose);
     resp_msg.call_end_reason_valid = TRUE;
     resp_msg.call_end_reason = (wds_call_end_reason_enum_v01)call_end_reason;
  }
  if(errval == QMI_ERR_NONE_V01)
  {
    resp_msg.mux_id_valid = TRUE;
    resp_msg.mux_id       = mux_id;
   /* resp_msg.ep_id_valid  = 1;
    resp_msg.ep_id    = (net_down_ep_id & 0xFF000000) >> 24;*/
    resp_msg.pkt_data_handle = (uint32)pkt_data_handle;
  }
  resp_msg.resp.result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01
                                                     : QMI_RESULT_FAILURE_V01);
  resp_msg.resp.error  = errval;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(wds_start_network_interface_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
}

static qmi_csi_cb_error qmi_wdsi_start_network_interface
(
  qmi_wdsi_client_info_type      *clnt_info_ptr,
  qmi_req_handle                  req_handle,
  int                             msg_id,
  void                           *req_c_struct,
  int                             req_c_struct_len,
  void                           *service_handle
)
{
/*--------------declarations --------------------------------------------*/
  rmnet_meta_sm_network_info_type         *sni_params = NULL;
  qmi_error_type_v01                       errval = QMI_ERR_NONE_V01;
  wds_start_network_interface_req_msg_v01 *req_msg = NULL;
  qmi_wds_client_state_map                *wds_sp =NULL;
  uint8                                    mux_id =0;
  uint8                                    mux_multiplier = 3;
  uint8                                    tmp_mux_id =0;
  uint8                                    index =0;   
  ds_policy_info_type                      policy_info;   
  ds_fwk_clnt_handle_type                  client_handle = 
                                              DS_FWK_INVALID_CLNT_HANDLE;
  uint32                                   call_end_reason_verbose =0;
  qmi_wds_call_end_reason_e_type           call_end_reason = 
                                              QMI_WDS_CALL_END_REASON_MIN;
  uint32                                   ep_id,tmp_ep_id;   
  qmi_wdsi_client_info_type                *prev_client = NULL;
  rmnet_smi_info_type                      *rm_info = NULL;
  qmi_wdsi_client_info_type                *tmp_clnt = NULL;

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  req_msg = (wds_start_network_interface_req_msg_v01 *)req_c_struct;
  if(((req_msg->bring_up_by_apn_name_valid ==TRUE) &&
      (req_msg->bring_up_by_apn_name != TRUE))||
      (req_msg->bring_up_by_apn_name_valid == FALSE))
  {
      DS_MSG0(MSG_LEGACY_ERROR,"Bringup call by apn needs "
              "to be set for chiltepin");
      errval = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
      goto bail;
  }
  if (clnt_info_ptr->net_bound)
  {
     DS_MSG2(MSG_LEGACY_MED,"Network interface already started by clid %p "
             "netbound %d!", clnt_info_ptr,clnt_info_ptr->net_bound);
     errval = QMI_ERR_NO_EFFECT_V01;
     goto bail;
  }
  /*-------------------------------------------------------------------------
    Initialize sni_params
  -------------------------------------------------------------------------*/
  DS_SYSTEM_HEAP_MEM_ALLOC(sni_params,
                           sizeof(rmnet_meta_sm_network_info_type),
                           rmnet_meta_sm_network_info_type*); 
  if (NULL == sni_params)
  {
     errval = QMI_ERR_NO_MEMORY_V01;
     goto bail;
  }
  memset(sni_params, 0, sizeof(rmnet_meta_sm_network_info_type));

  /*------------------------------------------------------------------------
    Basic input validation
  -------------------------------------------------------------------------*/
  if(req_msg->apn_name_valid)
  {
    if (strlen(req_msg->apn_name) <= QMI_WDS_APN_NAME_MAX_V01)
    {
      sni_params->apn.length = (uint8)strlen(req_msg->apn_name);
      strlcpy(sni_params->apn.name,
              req_msg->apn_name,
             (QMI_WDS_APN_NAME_MAX_V01 +1));
    }
    else
    {
      errval = QMI_ERR_ARG_TOO_LONG_V01;
      goto bail;
    }
  }
  if(req_msg->ext_technology_preference_valid)
  {
    req_msg->ip_family_preference_valid = TRUE;
    req_msg->ip_family_preference = WDS_IP_FAMILY_PREF_NON_IP_V01;
  }
  
  if(req_msg->ip_family_preference_valid)
  {

   sni_params->ip_family_pref = req_msg->ip_family_preference;
  /* validate the IP family preference */
    if ((errval = qmi_wdsi_validate_ip_family_pref(sni_params->ip_family_pref))
                                                     != QMI_ERR_NONE_V01) 
    {
       goto bail;
    }
  }
  else
  {
     sni_params->ip_family_pref = QMI_AF_INET;
  }
  if(req_msg->apn_type_valid)
  {
    sni_params->apn_type = req_msg->apn_type;
  }  
  if(req_msg->call_type_valid)
  {
    sni_params->call_type = req_msg->call_type;
  }
  if(req_msg->authentication_preference_valid)
  {
    sni_params->auth_param_valid = TRUE;
    sni_params->auth_params.auth_type = (ds_profile_auth_enum_type)\
		                                req_msg->authentication_preference;
  }
  if(req_msg->username_valid)
  {
    if(strlen(req_msg->username) <= 
       sizeof(sni_params->auth_params.username))
    {
      strlcpy(sni_params->auth_params.username,
              req_msg->username,DS_PROFILE_MAX_AUTH_STRING_LEN+1);
    }
    else
    {
      errval = QMI_ERR_ARG_TOO_LONG_V01;
      goto bail;
    }
  }
  if(req_msg->password_valid)
  { 
    if(strlen(req_msg->password) <= 
              sizeof(sni_params->auth_params.password))
    {
      strlcpy(sni_params->auth_params.password,
              req_msg->password,DS_PROFILE_MAX_AUTH_STRING_LEN+1);
    }
    else
    {
      errval = QMI_ERR_ARG_TOO_LONG_V01;
      goto bail;
    }
  }
  if(req_msg->ep_id_valid)
  {
    if(QMI_EP_ID_VALID(req_msg->ep_id))
    {
      ep_id = QMI_EP_ID_TO_INTERNAL_EP_ID(req_msg->ep_id);
    }
    else
    {
       DS_MSG2(MSG_LEGACY_ERROR,"Invalid EP id 0x%x:0x%x",
               req_msg->ep_id.ep_type, req_msg->ep_id.iface_id);
       errval = QMI_ERR_INVALID_ARG_V01;
       goto bail;
    }
  }
  else
     ep_id   = EP_TYPE_BAM_DEMUX;   

  if(req_msg->mo_exceptional_data_capability_valid)
   {
     sni_params->mo_excp_data_cap = req_msg->mo_exceptional_data_capability;
   }

  if(rmnet_phys_xport_from_ep_id(ep_id) == NULL)
  {
      DS_MSG2(MSG_LEGACY_ERROR,"Invalid EP id 0x%x:0x%x",
              req_msg->ep_id.ep_type, req_msg->ep_id.iface_id);
       errval = QMI_ERR_INVALID_ARG_V01;
      goto bail;
  }  
  
  wds_sp = qmi_get_wds_state(clnt_info_ptr);
  if(wds_sp != NULL)
  {  
   /*-------------------------------------------------------------------------
       Check if a Stop is already in process. Return Close in progress error.
     -------------------------------------------------------------------------*/
   if (wds_sp->stop_nw_client != NULL)
   {
      DS_MSG0(MSG_LEGACY_ERROR,"Stop Network Command already in Queue");
      errval = QMI_ERR_CALL_FAILED_V01;
      call_end_reason = QMI_WDS_CALL_END_REASON_CLOSE_IN_PROGRESS;
      call_end_reason_verbose = DS_NET_DOWN_REASON_CLOSE_IN_PROGRESS;
   }
  /*----------------------------------------------------------------------------
    The Wds_sp could not be NULL only in two cases 
    1. The client has started the call and again trying to start the call without 
        any STOP.
        But this case is handled above only by checking net_bound 
        Ideally we wont come here.
    2. Second case could be client started the call and then stopped the call also and 
        again trying to bring the call on same or different APN.
        So in this case the client could be binded to another wds_inst(if call on 
        different APN and based on free_mux_id)
        So remove the referencse of the client from the older wds_inst.
------------------------------------------------------------------------------  */
   tmp_clnt = wds_sp->client_list;
   while(tmp_clnt !=NULL)
   {
     if(qmi_wdsi_compare_client_handle(tmp_clnt, clnt_info_ptr->clnt))
     {
        DS_MSG2(MSG_LEGACY_ERROR,"qmi_wdsi_start_network_interface:"
                "Client present %p and %p at index %d",
                 prev_client,clnt_info_ptr->clnt);
        if (prev_client == NULL)
        {
          wds_sp->client_list = tmp_clnt->next;
        }
        else
        {
          prev_client->next = tmp_clnt->next;
        }
        break;
     }
   prev_client = tmp_clnt;
   tmp_clnt = tmp_clnt->next;
   }
  }

 /*-------------------------------------------------------------------------------
   Check if the call is already active by doing the policy look up to
   find any call is active on the PDN
  ---------------------------------------------------------------------------------*/

  memscpy(policy_info.name ,(DS_SYS_MAX_APN_LEN -1),sni_params->apn.name,(DS_SYS_MAX_APN_LEN -1)); 
  policy_info.length = sni_params->apn.length;
  policy_info.family = sni_params->ip_family_pref;

  client_handle = ds_fwk_look_up_pdn_by_policy(DS_FWK_CLIENT_RMNET,
                                               policy_info);
  
  DS_MSG1(MSG_LEGACY_ERROR,"Start network:  client handle %d,",client_handle);
  if( client_handle != DS_FWK_INVALID_CLNT_HANDLE )
  {
    DS_MSG1(MSG_LEGACY_HIGH,"Start network:Already a call is present with"
           "same APN and IPTYPE client handle %d,",client_handle);
    // Check for the endpoint here it should be same as earlier call
    tmp_ep_id = rmnet_get_ep_id_from_rmnet_inst(client_handle);

    //Check the ep_id here and if the ep_id mismatches 
    //it means that new client is trying to bring
    //a call with different ep_id on same APN and same IPTYPE so reject call
    if(tmp_ep_id != ep_id)
    {
      errval =  QMI_ERR_INTERNAL_V01;
      DS_ERROR_LOG_2("Start network Failed:EP ID mismatch passed ep_id %d "
                     "and wds_inst ep_id %d,",
                       ep_id ,tmp_ep_id);
       goto bail;
    }
    mux_id = rmnet_get_mux_id_from_inst(client_handle);
    DS_MSG2(MSG_LEGACY_MED," mux_id %d, RMNET inst %d",mux_id, client_handle);

    wds_sp = qmi_wdsi_create_cl_sp_map((mux_id*mux_multiplier) , 
                               sni_params->ip_family_pref,
                               clnt_info_ptr,ep_id);
    wds_sp->binding.mux_id = mux_id;
     clnt_info_ptr->net_bound = TRUE;     
     
    //Check if the call is already up or not
    rm_info = (rmnet_smi_info_type *)rmnet_get_smi_info_ptr(client_handle);
    if((rm_info !=NULL)&&
      (rm_info->meta_sm.state >RMNET_META_SM_UM_CONFIGURING_STATE)&&
      (rm_info->meta_sm.state  <=RMNET_META_SM_RM_RECONFIGURING_STATE))
    {
      DS_MSG2(MSG_LEGACY_HIGH," RMNET info %p, RMNET state %d",rm_info, rm_info->meta_sm.state);
      errval =  QMI_ERR_CALL_FAILED_V01;
      call_end_reason_verbose = DS_NET_DOWN_REASON_INTERNAL_CALL_ALREADY_PRESENT;
      goto bail;
  }
  else
    {
       //Call might be in the progress so queue this requret also       
        clnt_info_ptr->req_handle = req_handle;
        clnt_info_ptr->sni_pending_next  =  wds_sp->sni_pending_list;
        wds_sp->sni_pending_list = clnt_info_ptr;
        DS_SYSTEM_HEAP_MEM_FREE(sni_params);   
        return QMI_CSI_CB_NO_ERR;
    }
  }
  else
  { 
    /*----------------------------------------------------------------------
    There could be a chance that call was UP on v4 and we are now requesting
    call on V6 instance on same APN .
    In this case just find out the same mux_id used earlier.
    Else Find a free mux id and intiate call on that mux_id
    ------------------------------------------------------------------------*/ 
    client_handle = qmi_wdsi_get_companion_ip_type_call(sni_params->apn.name,
                                       sni_params->apn.length,
                                       sni_params->ip_family_pref);
    
    if(client_handle != DS_FWK_INVALID_CLNT_HANDLE)
    {     
      //Check the ep_id here and if the ep_id mismatches it 
      //means that client is trying to bring
      //a call with different ep_id on same APN 
      //and different IPTYPE so reject call
      tmp_ep_id = rmnet_get_ep_id_from_rmnet_inst(client_handle);
      if(tmp_ep_id != ep_id)
      {
         errval =  QMI_ERR_INTERNAL_V01;
         DS_ERROR_LOG_2("Start network Failed : EP ID mismatch"
                        "passed ep_id %d and wds_inst ep_id %d,",
                         ep_id ,tmp_ep_id);
         goto bail;
      }
     //Means the call is already up on same apn and different IP
       mux_id = rmnet_get_mux_id_from_inst(client_handle);
       tmp_mux_id = mux_id;
    }
    else
    {
  /*-----------------------------------------------------------------------
      For dual ip call types it may happen that v4 and v6 call comes at 
      same time in that case we will end up generating two differnt mux_id 
      for v4 and v6 call and the first call would have been posted to RMNET 
      and while processing second call it might happen that the companion ip 
      call is not updated in the framework and companion check fails and we 
      end up treating it as a new differnt call and give new miux id 
  ---------------------------------------------------------------------------*/
       mux_id = rmnet_meta_sm_get_free_mux_id(sni_params->call_type,
                                              sni_params->apn.name);
       tmp_mux_id = mux_id;  
    }
    if(mux_id == 0)
    {     
       DS_MSG1(MSG_LEGACY_ERROR,"Start network failed as mux_id exhausted "
               "mux_id %d,",mux_id);
       errval = QMI_ERR_INVALID_OPERATION_V01;
       goto bail;
    }
    if(mux_id == TETHERED_CALL_MUXID)
    { 
     tmp_mux_id = MAX_MUX_ID_POOL_SIZE;
    }
    index = (tmp_mux_id * mux_multiplier); 
   /*-------------------------------------------------------------------------
    All Validations are successfull now add the client info to the cl_sp_map
    and assign a rmnet instance also in 
   ---------------------------------------------------------------------------*/
    DS_MSG3(MSG_LEGACY_HIGH,"Start network:cl_sp index  %d temp mux_id %d"
                            "Mux Id %d,", index, tmp_mux_id, mux_id);

    wds_sp = qmi_wdsi_create_cl_sp_map(index , sni_params->ip_family_pref,
                              clnt_info_ptr,ep_id);
    
    clnt_info_ptr->req_handle = req_handle;
    DS_MSG2(MSG_LEGACY_HIGH,"Start network req_handle is %p "
               " %p,",clnt_info_ptr->req_handle,req_handle);
    wds_sp->binding.mux_id     = mux_id;
  /*-----------------------------------------------------------------------
     This is the first client to start the network interface.
     Or it could be the other client starting the call on some APN and IP type 
     and call is already in progress ,In that case look up will return fail and 
     we can come here so add the client to the sni_pending_list and when PDN_UP 
     comes send the SNI response for all clients added to this list.
   -----------------------------------------------------------------------*/   
    clnt_info_ptr->net_bound = TRUE;
    clnt_info_ptr->sni_pending_next  =  wds_sp->sni_pending_list;
    wds_sp->sni_pending_list = clnt_info_ptr;
    
    if(FALSE == rmnet_meta_sm_bind(wds_sp->binding.ep_id, mux_id))
    {
       errval =  QMI_ERR_INTERNAL_V01; 
       DS_ERR_FATAL("Failed to start a rmnet instance ,Can not start a call");
    }
  
    rmnet_meta_sm_set_network_cfg_params(wds_sp->binding.rmnet_inst,
                                          sni_params);
    (void)rmnet_meta_sm_post_event(wds_sp->binding.rmnet_inst,
                            RMNET_META_SM_RM_DEV_START_PKT_EV);
  }
  DS_SYSTEM_HEAP_MEM_FREE(sni_params);   
  return QMI_CSI_CB_NO_ERR;
  bail:

  if (sni_params != NULL)
  {
    DS_SYSTEM_HEAP_MEM_FREE(sni_params);
  }

   return wdsi_gen_start_network_interface_response(errval,
                                                    req_handle,
                                                    msg_id,
                                                    call_end_reason,
                                                    call_end_reason_verbose,
                                                    mux_id,
                                                    clnt_info_ptr);
}

/*===========================================================================
  FUNCTION QMI_WDSI_STOP_NET_IFACE_CLIENT()

  DESCRIPTION
    Release the WWAN packet data connection (unless other control points are
    using it)

  PARAMETERS
    cl_sp  :                  Client info 
    force  :                  Forceful stop or default 
    wdsi_sp   :               Wds state pointer to which this client belong
    pending_resp      :       Output param if resp is pending 
    req_handle   :            req handle

  RETURN VALUE
    qmi_error_type_v01

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_type_v01 qmi_wdsi_stop_net_iface_client
(
  qmi_wdsi_client_info_type          *cl_sp,
  qmi_wdsi_force_type                 force,
  qmi_wds_client_state_map           *wdsi_sp,
  boolean                            *pending_resp,  
  qmi_req_handle                      req_handle
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  boolean retvalue = FALSE;
  DS_ASSERT((int)cl_sp);
  DS_MSG0(MSG_LEGACY_MED,"Client stop the network interface");
  *pending_resp = FALSE;
  if ((!cl_sp->net_bound) && (force == WDSI_FORCE_REASON_DEFAULT))
  {
    DS_MSG0(MSG_LEGACY_MED,"NW interface never started or already stopped "
            "by clid!");
    return QMI_ERR_NO_EFFECT_V01;
  }
  else
  {
    /*-----------------------------------------------------------------------
      Release client's hold on net interface
    -----------------------------------------------------------------------*/
    cl_sp->net_bound = FALSE;
    /*-----------------------------------------------------------------------
      If no other client has the net interface started
    -----------------------------------------------------------------------*/
    if (!qmi_wdsi_net_if_bound( wdsi_sp ))
    {
      if (rmnet_meta_sm_in_call(wdsi_sp->binding.rmnet_inst) || 
         (force == WDSI_FORCE_REASON_ABORT))
      {
        DS_MSG0(MSG_LEGACY_MED,"Last client releases RmNet call");

        /* --------------------------------------------------------------
                 Post event to WWAN RMSM to end call.
        ----------------------------------------------------------------*/
      retvalue = rmnet_meta_sm_post_event(wdsi_sp->binding.rmnet_inst,
                              RMNET_META_SM_RM_LINK_DOWN_EV);
      if(retvalue == FALSE)
      {
        DS_MSG0(MSG_LEGACY_HIGH,"Failure in posting the command to Rmnet state");
        return QMI_ERR_OUT_OF_CALL_V01;
      }
      *pending_resp = TRUE;
        if(req_handle !=NULL)
          cl_sp->req_handle = req_handle;
      }
      else
      {
        DS_MSG0(MSG_LEGACY_MED,"Rm is already down");
      }
      return QMI_ERR_NONE_V01;
    }
    else
    {
      DS_MSG0(MSG_LEGACY_MED,"Network interface left up for other clients");
      return QMI_ERR_NONE_V01;
    }
  }
}


static qmi_csi_cb_error  qmi_wdsi_stop_network_interface
(
  qmi_wdsi_client_info_type        *clnt_info,
  qmi_req_handle                    req_handle,
  int                               msg_id,
  void                             *req_c_struct,
  int                               req_c_struct_len,
  void                             *service_handle
)
{
  qmi_wds_client_state_map                *wdsi_sp;
  qmi_error_type_v01                       errval;
  wds_stop_network_interface_req_msg_v01  *req_msg;
  wds_stop_network_interface_resp_msg_v01  resp_msg;
  qmi_result_type_v01                      result;  
  boolean                                  pending_resp  = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  req_msg = (wds_stop_network_interface_req_msg_v01 *)req_c_struct;
  memset(&resp_msg,0,sizeof(wds_stop_network_interface_resp_msg_v01));
  DS_MSG0(MSG_LEGACY_MED,"Client end the rmnet call");

  errval   = QMI_ERR_NONE_V01;
  wdsi_sp = qmi_get_wds_state(clnt_info);
  
  if(wdsi_sp  ==NULL)
  {
    errval = QMI_ERR_INTERNAL_V01;
    goto send_result;
  }
  if(req_msg->pkt_data_handle == 0)
  {
    errval = QMI_ERR_INVALID_HANDLE_V01;
    goto send_result;
  }
  
  if (errval == QMI_ERR_NONE_V01)
  {
    if(((void*)req_msg->pkt_data_handle == clnt_info)&&
        (wdsi_sp->stop_nw_client == NULL))
    {
     /*---------------------------------------------------------------------
        Stop the network interface for this client
     ---------------------------------------------------------------------*/
      errval = qmi_wdsi_stop_net_iface_client(clnt_info,
                                              WDSI_FORCE_REASON_DEFAULT,
                                              wdsi_sp,&pending_resp,
                                              req_handle
                                              );
     if(pending_resp == FALSE)
        goto send_result;
    }
    else
    {
       errval = QMI_ERR_OUT_OF_CALL_V01;
       goto send_result;
    }
    wdsi_sp->stop_nw_client = clnt_info;
    return QMI_CSI_CB_NO_ERR;
  }
  
send_result:
  result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01
                                          : QMI_RESULT_FAILURE_V01);
  resp_msg.resp.error = errval; 
  resp_msg.resp.result = result;
  
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
					  sizeof(wds_stop_network_interface_resp_msg_v01));
 return QMI_CSI_CB_NO_ERR;
}

static qmi_csi_cb_error  qmi_wdsi_get_pkt_srvc_status
(
  qmi_wdsi_client_info_type        *clnt_info,
  qmi_req_handle                    req_handle,
  int                               msg_id,
  void                             *req_c_struct,
  int                               req_c_struct_len,
  void                             *service_handle
)
{
  qmi_error_type_v01                        errval ;
  wds_get_pkt_srvc_status_req_msg_v01      *req_msg =NULL;
  wds_get_pkt_srvc_status_resp_msg_v01      resp_msg;  
  qmi_wds_client_state_map                 *wds_sp;  
  qmi_result_type_v01                       result;  
  uint32                                    req_settings =  QMI_WDS_MASK_DNS_ADDR |QMI_WDS_MASK_IP_ADDR|
                                                 QMI_WDS_MASK_GATEWAY|QMI_WDS_MASK_MTU
                                                 |QMI_WDS_MASK_IP_FAMILY|QMI_WDS_MASK_OP_RES_PCO;
  boolean                                   family =FALSE;
  ip_addr_type                              pri_dns = {IP_ANY_ADDR, {0}};
  ip_addr_type                              sec_dns = {IP_ANY_ADDR, {0}};
  uint32                                    ipv4_addr;
  ds_fwk_s_type                            *fwk_inst   = NULL;
  ds_fwk_clnt_info_s_type                   clnt_info_ptr ={0};  
  ps_ip_addr_type                           if_addr= {0};
  ip_addr_type                              dns_addr[2]={{IP_ANY_ADDR, {0}},{IP_ANY_ADDR, {0}}};  
  int16                                     ps_errno_addr_mgmt = 0;  
  uint64                                   *iid = NULL;
  ds_pdn_ioctl_get_all_dns_addrs_type       dns_addrs={0};
  ds_pdn_op_reserved_pco_type               op_reserved_pco={0};  
  sint15                                    ps_errno_iid;
  ps_ip_addr_type                           gateway_ip_addr={0}; 
  ps_addr_mgmt_handle_type                  handle;
  ds_pdn_ioctl_subnet_mask_type             subnet_mask={0};  
  int                                       num_retry = 0;
  sint15                                    ds_errno;
  ip_addr_type                              v6_pri_dns_man = {IP_ANY_ADDR, {0}};
  ip_addr_type                              v6_sec_dns_man = {IP_ANY_ADDR, {0}};  
  uint32                                    mtu =0;  
  struct ps_in6_addr                        ipv6_addr = {{{0},}};  
  ps_addr_mgmt_alloc_type                   alloc_info;
/*--------------------------------------------------------------*/  
  req_msg = (wds_get_pkt_srvc_status_req_msg_v01 *)req_c_struct;
  
  errval = QMI_ERR_NONE_V01;
  memset(&resp_msg,0,sizeof(resp_msg));

  if(req_msg->ip_config_req_mask_valid)
  {
    req_settings = req_msg->ip_config_req_mask;
  }
  /*-------------------------------------------------------------------------
    Check whether the call is up or not
  -------------------------------------------------------------------------*/
  wds_sp = qmi_get_wds_state(clnt_info);
  
  if((wds_sp  == NULL) ||
    (wds_sp->pkt_svc_state != WDS_CONNECTION_STATUS_CONNECTED_V01))
  {  
     DS_MSG0(MSG_LEGACY_MED,"Not in a call, so no runtime settings available");
   errval = QMI_ERR_OUT_OF_CALL_V01;
   goto send_result;
  }

  resp_msg.connection_status = wds_sp->pkt_svc_state;
  dns_addrs.num_dns_addrs =2;
  dns_addrs.dns_addrs_ptr = dns_addr;

  ds_fwk_get_clnt_info_from_handle(DS_FWK_CLIENT_RMNET,wds_sp->binding.rmnet_inst,
                                  &clnt_info_ptr);
  if(clnt_info_ptr.fw_inst == NULL)
  {   
    DS_MSG0(MSG_LEGACY_ERROR,"No framework "
           "instance could be found for this call");
    DS_ASSERT(0);

  }
  fwk_inst = clnt_info_ptr.fw_inst;
  //Call IOCTLS to get the required data

  ds_pdn_ioctl(DS_FWK_CLIENT_RMNET,wds_sp->binding.rmnet_inst,
               DS_PDN_IOCTL_GET_IP_ADDR,&if_addr,&ds_errno);

  //ds_pdn_ioctl(DS_FWK_CLIENT_RMNET,wds_sp->binding.rmnet_inst,
     //          DS_PDN_IOCTL_GET_GATEWAY_ADDR,&gateway_ip_addr,&ds_errno);

//  ds_pdn_ioctl(DS_FWK_CLIENT_RMNET,wds_sp->binding.rmnet_inst,
   //            DS_PDN_IOCTL_GET_SUBNET_MASK,&subnet_mask,&ds_errno);

  ds_pdn_ioctl(DS_FWK_CLIENT_RMNET,wds_sp->binding.rmnet_inst,
               DS_PDN_IOCTL_GET_ALL_DNS_ADDR,&dns_addrs,&ds_errno);

  ds_pdn_ioctl(DS_FWK_CLIENT_RMNET,wds_sp->binding.rmnet_inst,
               DS_PDN_IOCTL_GET_MTU,&mtu,&ds_errno);

  if(wds_sp->ip_call_type == QMI_AF_NIPD )
  {
    if(req_settings & QMI_WDS_MASK_MTU)
    { 
       resp_msg.mtu_valid = TRUE;
       resp_msg.mtu = mtu;//(fwk_inst, NON_IP_ADDR);
    }
    else
    {
      errval = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
    }
    goto send_result;
  }
  if(req_settings & QMI_WDS_MASK_IP_FAMILY)
    family =TRUE;

  if(wds_sp->ip_call_type == QMI_AF_INET)
  {
    if(family ==TRUE)
    {
      resp_msg.ip_family_valid =TRUE;
      resp_msg.ip_family = WDS_IP_FAMILY_PREF_IPV4_V01;
    }
    memset(&pri_dns, 0, sizeof(ip_addr_type));
    memset(&sec_dns, 0, sizeof(ip_addr_type));
   /* Convert DNS addrs to little endian to send to host */
    pri_dns.addr.v4 = ps_ntohl(dns_addr[0].addr.v4);
    sec_dns.addr.v4 = ps_ntohl(dns_addr[1].addr.v4);

    if((req_settings & QMI_WDS_MASK_IP_ADDR) &&(if_addr.addr.v4.ps_s_addr != 0))
    { 
      ipv4_addr = ps_ntohl(if_addr.addr.v4.ps_s_addr);
      resp_msg.ipv4_address_valid = TRUE;
      resp_msg.ipv4_address = ipv4_addr;
    }

    if(req_settings & QMI_WDS_MASK_DNS_ADDR)
    {
      if(0 != dns_addr[1].addr.v4)
      {
        resp_msg.secondary_DNS_IPv4_address_valid = TRUE;
        resp_msg.secondary_DNS_IPv4_address = sec_dns.addr.v4;
      }
      if(0 != dns_addr[0].addr.v4)
      {
        resp_msg.primary_DNS_IPv4_address_valid = TRUE;
        resp_msg.primary_DNS_IPv4_address = pri_dns.addr.v4;
      }
    }
    if(req_settings & QMI_WDS_MASK_GATEWAY)
    {
      ds_fwk_calc_gateway_addr(if_addr.addr.v4.ps_s_addr,&(gateway_ip_addr.addr.v4.ps_s_addr),
                                      &(subnet_mask.addr.v4));
      if(gateway_ip_addr.addr.v4.ps_s_addr !=0)
      {
         resp_msg.ipv4_subnet_mask_valid = TRUE;
         resp_msg.ipv4_subnet_mask       =  ps_ntohl(subnet_mask.addr.v4);
  
         resp_msg.ipv4_gateway_addr_valid =  TRUE;
         resp_msg.ipv4_gateway_addr = ps_ntohl(gateway_ip_addr.addr.v4.ps_s_addr);
      } 
    }
    if((req_settings & QMI_WDS_MASK_MTU)&&(mtu !=0))
    { 
      resp_msg.mtu_valid = TRUE;
      resp_msg.mtu       = mtu;
    }
  }
  else if(wds_sp->ip_call_type == QMI_AF_INET6)
  {
    memset(&v6_pri_dns_man, 0, sizeof(ip_addr_type));
    memset(&v6_sec_dns_man, 0, sizeof(ip_addr_type));	  
   /*-----------------------------------------------------------------------
     Query the 'manual' DNS addresses and over-ride auto-configured values if 
     set
     -----------------------------------------------------------------------*/
    rmnet_meta_sm_get_ipv6_manual_dns_addrs(wds_sp->binding.rmnet_inst,
                                           &v6_pri_dns_man,
                                           &v6_sec_dns_man);
    if (IPV6_ADDR == v6_pri_dns_man.type)
    {
      if ((0 == v6_pri_dns_man.addr.v6[0]) && (0 == v6_pri_dns_man.addr.v6[1]))
      {
         DS_MSG0(MSG_LEGACY_MED,"Manual IPv6 primary DNS, Ignoring IPv6"
                                "primary   DNS address");
      }
      else
      {
         DS_MSG0(MSG_LEGACY_MED,"Using manual IPv6 primary DNS address");
          memscpy(&dns_addr[0],
                  sizeof(dns_addr[0]), 
                 &v6_pri_dns_man,
                  sizeof(ip_addr_type));
      }
    }
    else
    {
       DS_MSG0(MSG_LEGACY_MED,"Manual IPv6 primary DNS address not provisioned");
    }
    if (IPV6_ADDR == v6_sec_dns_man.type)
    {
      if ((0 == v6_sec_dns_man.addr.v6[0]) &&
          (0 == v6_sec_dns_man.addr.v6[1]))
      {
         DS_MSG0(MSG_LEGACY_MED,"Manual IPv6 secondary DNS, Ignoring"
                 "manual IPv6 secondary DNS address");
      }
      else
      {
         DS_MSG0(MSG_LEGACY_MED,"Using manual IPv6 secondary DNS address");
        memscpy(&dns_addr[1],
                sizeof(dns_addr[1]), 
                &v6_sec_dns_man, 
                 sizeof(ip_addr_type)); 
      }
    }
    else
    { 
        DS_MSG0(MSG_LEGACY_MED,"Manual IPv6 secondary DNS address not provisioned");
    }
  
    if(family ==TRUE)
    {
      resp_msg.ip_family_valid =TRUE;
      resp_msg.ip_family = WDS_IP_FAMILY_PREF_IPV6_V01;
    } 
    if((req_settings & QMI_WDS_MASK_MTU)&&(mtu != 0))
    {
       resp_msg.mtu_valid = TRUE;
       resp_msg.mtu = mtu;//DS_FWK_GET_MTU(fwk_inst,IPV6_ADDR);
    }
    if((req_settings & QMI_WDS_MASK_IP_ADDR)&&
      (if_addr.addr.v6.ps_s6_addr64[0] !=0))
    { 
      ipv6_addr.ps_s6_addr64[0] = if_addr.addr.v6.ps_s6_addr64[0];  
      if(FALSE == ds_fwk_get_external_ipv6_addrs(clnt_info_ptr.fw_inst ,&ipv6_addr))
      {        
        iid = &ipv6_addr.ps_s6_addr64[1];
        if (-1 == ps_generate_ipv6_iid(clnt_info_ptr.fw_inst, iid, &ps_errno_iid))
        {
           DS_MSG0(MSG_LEGACY_ERROR,"Failed to generate a random IPv6 IID");
        }
        /* Add the address to UM database
         If there is a conflict re-generate IID up to 3 times
        */
        num_retry = 0;
        while (TRUE)
        {
           handle = ps_addr_mgmt_get_handle_from_ip(clnt_info_ptr.fw_inst, &ipv6_addr);
           if (-1 == handle)
           {
            /* No match found, add to database */
             memset(&alloc_info, 0, sizeof(alloc_info));
             alloc_info.ip_addr.type    = IPV6_ADDR;
             alloc_info.ip_addr.addr.v6 = ipv6_addr;
             alloc_info.addr_type       = DS_IPV6_ADDR_TYPE_EXTERNAL;
             if (-1 == ps_addr_mgmt_alloc_unique_addr(clnt_info_ptr.fw_inst, 
                                                      &handle,
                                                      &alloc_info,
                                                       DS_FWK_CLIENT_RMNET,
                                                      &ps_errno_addr_mgmt
                                                       ))
              {
                DS_MSG1(MSG_LEGACY_ERROR,"Failed to add IPv6 addr to pdn  database."
                        "errno [%d]",ps_errno_addr_mgmt);
                 DS_ASSERT(0);
              }
              else
              {
                 DS_MSG1(MSG_LEGACY_HIGH,"Added IPv6 addr to pdn database "
                         "%lld",ipv6_addr.ps_s6_addr64[1]);
              }
              break;
             }
             else
             {
               IPV6_ADDR_MSG(ipv6_addr.ps_s6_addr64);
               /* Generate another address since there is a conflict */
                if (num_retry < 3)
                {
                   DS_MSG0(MSG_LEGACY_MED,"IPv6 addr already in database, re-generate IID");
                   if (-1 == ps_generate_ipv6_iid(clnt_info_ptr.fw_inst, iid, &
                                                  ps_errno_iid))
                    {
                       DS_MSG0(MSG_LEGACY_ERROR,"Failed to generate a random IPv6 IID");              
                       DS_ASSERT(0);
                    }
                    num_retry++;
                 }
                 else
                 {
                   DS_MSG1(MSG_LEGACY_ERROR,"Max retry [%d], give up writing to PDN DB.",
                        num_retry);
                    DS_ASSERT(0);
                    goto send_result;
                 }
               }
            }
       }
       resp_msg.ipv6_addr_valid =  TRUE;	
       resp_msg.ipv6_addr.ipv6_prefix_length = clnt_info_ptr.fw_inst->v6_net_info.ipv6_addrs[0]->prefix_len;
       memscpy(resp_msg.ipv6_addr.ipv6_addr,
               sizeof(resp_msg.ipv6_addr.ipv6_addr),
                ipv6_addr.ps_s6_addr64,
                sizeof(ipv6_addr.ps_s6_addr64)); 
    }
    if(req_settings & QMI_WDS_MASK_GATEWAY)
    {
      if(0 != gateway_ip_addr.addr.v6.ps_s6_addr64[0])
      {
        resp_msg.ipv6_gateway_addr_valid = TRUE;
        resp_msg.ipv6_gateway_addr.ipv6_prefix_length =
                        WDSI_IPV6_DEFAULT_PREFIX_LEN;
        memscpy(resp_msg.ipv6_gateway_addr.ipv6_addr,
                sizeof(resp_msg.ipv6_gateway_addr.ipv6_addr),
                gateway_ip_addr.addr.v6.ps_s6_addr64,
                sizeof(gateway_ip_addr.addr.v6.ps_s6_addr64));
      }
    }
    if(req_settings & QMI_WDS_MASK_DNS_ADDR)
    {
      if(0 != dns_addr[0].addr.v6[0])
      {
        resp_msg.primary_dns_IPv6_address_valid = TRUE;
        memscpy(resp_msg.primary_dns_IPv6_address,
                sizeof(resp_msg.primary_dns_IPv6_address),
                dns_addr[0].addr.v6,
                sizeof(dns_addr[0].addr.v6));
      } 
      else
      {
        DS_MSG0(MSG_LEGACY_MED,"v6_pri_dns.addr.v6[0]=0, omitting TLV");
      }  
      if(0 != dns_addr[1].addr.v6[0])
      {
        resp_msg.secondary_dns_IPv6_address_valid = TRUE;
        memscpy(resp_msg.secondary_dns_IPv6_address,
                sizeof(resp_msg.secondary_dns_IPv6_address),
                dns_addr[1].addr.v6,
                sizeof(dns_addr[1].addr.v6));
      }
      else
      {
        DS_MSG0(MSG_LEGACY_MED,"v6_sec_dns.addr.v6[0]=0, omitting TLV");
      }
    }
  }
  
  if(req_settings & QMI_WDS_MASK_OP_RES_PCO)
  {
    ds_pdn_ioctl(DS_FWK_CLIENT_RMNET,wds_sp->binding.rmnet_inst,
                DS_PDN_IOCTL_GET_OP_RES_PCO,&op_reserved_pco,&ds_errno);
   if(ds_errno == DS_EINVAL)
   {
       DS_MSG0(MSG_LEGACY_MED,"PCO information is unavailable !!!");
   }
   else
   {
      /*-----------------------------------------------------------------------
        Construct the PCO packet
      -----------------------------------------------------------------------*/
      /* Retrieve mcc field and copy into buffer*/
      resp_msg.operator_reserved_pco_valid = TRUE;
      resp_msg.operator_reserved_pco.mcc = op_reserved_pco.mcc;
     /* Retrieve mnc field and copy into buffer*/
      resp_msg.operator_reserved_pco.mnc = op_reserved_pco.mnc;
      /* Retrieve mnc_includes_pcs_digit field and copy into buffer*/
      resp_msg.operator_reserved_pco.mnc_includes_pcs_digit = 
               op_reserved_pco.mnc_includes_pcs_digit;
      /* Retrieve app_specific_info_len field and copy into buffer*/
      resp_msg.operator_reserved_pco.app_specific_info_len = 
               op_reserved_pco.app_specific_info_len;
      /* Retrieve app_specific_info field and copy into buffer*/
      memscpy(resp_msg.operator_reserved_pco.app_specific_info,
              sizeof(resp_msg.operator_reserved_pco.app_specific_info),
              (void *)&op_reserved_pco.app_specific_info,
              op_reserved_pco.app_specific_info_len);
      /* Retrieve container_id field and copy into buffer*/
      resp_msg.operator_reserved_pco.container_id = 
               op_reserved_pco.container_id;
  	}
  }
 
send_result:
  result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01
                      : QMI_RESULT_FAILURE_V01);
   resp_msg.resp.error = errval; 
   resp_msg.resp.result = result;
   qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                      sizeof(wds_get_pkt_srvc_status_resp_msg_v01));
 return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION QMI_WDSI_MAP_WDS_CALL_END_REASON()

  DESCRIPTION
    To obtain wds call end reason from pdn call end reason

  PARAMETERS
    pdn_call_end_reason

  RETURN VALUE
    wds_call_end_reason

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_wds_call_end_reason_e_type  qmi_wdsi_map_wds_call_end_reason
(
  ds_net_down_reason_type          call_end_reason
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch ( call_end_reason )
  {
    case DS_NET_DOWN_REASON_CLIENT_END:
      return QMI_WDS_CALL_END_REASON_CLIENT_END;

    case DS_NET_DOWN_REASON_NO_SRV:
      return QMI_WDS_CALL_END_REASON_NO_SRV;

    case DS_NET_DOWN_REASON_FADE:
      return QMI_WDS_CALL_END_REASON_FADE;

    case DS_NET_DOWN_REASON_REL_NORMAL:
      return QMI_WDS_CALL_END_REASON_REL_NORMAL;

    case DS_NET_DOWN_REASON_ACC_IN_PROG:
      return QMI_WDS_CALL_END_REASON_ACC_IN_PROG;

    case DS_NET_DOWN_REASON_ACC_FAIL:
      return QMI_WDS_CALL_END_REASON_ACC_FAIL;

    case DS_NET_DOWN_REASON_REDIR_OR_HANDOFF:
      return QMI_WDS_CALL_END_REASON_REDIR_OR_HANDOFF;

    case DS_NET_DOWN_REASON_CLOSE_IN_PROGRESS:
      return QMI_WDS_CALL_END_REASON_CLOSE_IN_PROGRESS;

    case DS_NET_DOWN_REASON_AUTH_FAILED:
      return QMI_WDS_CALL_END_REASON_AUTH_FAILED;

    case DS_NET_DOWN_REASON_INTERNAL_CALL_ENDED:
      return QMI_WDS_CALL_END_REASON_INTERNAL_CALL_END;

    case DS_NET_DOWN_REASON_INTERNAL_ERROR:
      return QMI_WDS_CALL_END_REASON_INTERNAL_ERROR;

    case DS_NET_DOWN_REASON_INTERNAL_UNKNOWN_CAUSE_CODE:
      return QMI_WDS_CALL_END_REASON_INTERNAL_UNKNOWN_CAUSE_CODE;
      
    case DS_NET_DOWN_REASON_CONF_FAILED:
      return QMI_WDS_CALL_END_REASON_CONF_FAILED;

    case DS_NET_DOWN_REASON_INCOM_REJ:
      return QMI_WDS_CALL_END_REASON_INCOM_REJ;

    case DS_NET_DOWN_REASON_NW_INITIATED_TERMINATION:
      return QMI_WDS_CALL_END_REASON_NETWORK_END;

    case DS_NET_DOWN_REASON_NO_GW_SRV:
      return QMI_WDS_CALL_END_REASON_NO_GW_SRV;

    case DS_NET_DOWN_REASON_LLC_SNDCP_FAILURE:
      return QMI_WDS_CALL_END_REASON_LLC_SNDCP_FAILURE;

    case DS_NET_DOWN_REASON_INSUFFICIENT_RESOURCES:
      return QMI_WDS_CALL_END_REASON_INSUFFICIENT_RESOURCES;

    case DS_NET_DOWN_REASON_OPTION_TEMP_OOO:
      return QMI_WDS_CALL_END_REASON_OPTION_TEMP_OOO;

    case DS_NET_DOWN_REASON_NSAPI_ALREADY_USED:
      return QMI_WDS_CALL_END_REASON_NSAPI_ALREADY_USED;

    case DS_NET_DOWN_REASON_REGULAR_DEACTIVATION:
      return QMI_WDS_CALL_END_REASON_REGULAR_DEACTIVATION;

    case DS_NET_DOWN_REASON_NETWORK_FAILURE:
      return QMI_WDS_CALL_END_REASON_NETWORK_FAILURE;

    case DS_NET_DOWN_REASON_UMTS_REATTACH_REQ:
      return QMI_WDS_CALL_END_REASON_UMTS_REATTACH_REQ;

    case DS_NET_DOWN_REASON_PROTOCOL_ERROR:
      return QMI_WDS_CALL_END_REASON_PROTOCOL_ERROR;

    case DS_NET_DOWN_REASON_OPERATOR_DETERMINED_BARRING:
      return QMI_WDS_CALL_END_REASON_OPERATOR_DETERMINED_BARRING;

    case DS_NET_DOWN_REASON_UNKNOWN_APN:
      return QMI_WDS_CALL_END_REASON_UNKNOWN_APN;

    case DS_NET_DOWN_REASON_UNKNOWN_PDP:
      return QMI_WDS_CALL_END_REASON_UNKNOWN_PDP;

    case DS_NET_DOWN_REASON_GGSN_REJECT:
      return QMI_WDS_CALL_END_REASON_GGSN_REJECT;

    case DS_NET_DOWN_REASON_ACTIVATION_REJECT:
      return QMI_WDS_CALL_END_REASON_ACTIVATION_REJECT;

    case DS_NET_DOWN_REASON_OPTION_NOT_SUPPORTED:
      return QMI_WDS_CALL_END_REASON_OPTION_NOT_SUPPORTED;

    case DS_NET_DOWN_REASON_OPTION_UNSUBSCRIBED:
      return QMI_WDS_CALL_END_REASON_OPTION_UNSUBSCRIBED;

    case DS_NET_DOWN_REASON_QOS_NOT_ACCEPTED:
      return QMI_WDS_CALL_END_REASON_QOS_NOT_ACCEPTED;

    case DS_NET_DOWN_REASON_TFT_SEMANTIC_ERROR:
      return QMI_WDS_CALL_END_REASON_TFT_SEMANTIC_ERROR;

    case DS_NET_DOWN_REASON_TFT_SYNTAX_ERROR:
      return QMI_WDS_CALL_END_REASON_TFT_SYNTAX_ERROR;

    case DS_NET_DOWN_REASON_UNKNOWN_PDP_CONTEXT:
      return QMI_WDS_CALL_END_REASON_UNKNOWN_PDP_CONTEXT;

    case DS_NET_DOWN_REASON_FILTER_SEMANTIC_ERROR:
      return QMI_WDS_CALL_END_REASON_FILTER_SEMANTIC_ERROR;

    case DS_NET_DOWN_REASON_FILTER_SYNTAX_ERROR:
      return QMI_WDS_CALL_END_REASON_FILTER_SYNTAX_ERROR;

    case DS_NET_DOWN_REASON_PDP_WITHOUT_ACTIVE_TFT:
      return QMI_WDS_CALL_END_REASON_PDP_WITHOUT_ACTIVE_TFT;

    case DS_NET_DOWN_REASON_INVALID_TRANSACTION_ID:
      return QMI_WDS_CALL_END_REASON_INVALID_TRANSACTION_ID;

    case DS_NET_DOWN_REASON_MESSAGE_INCORRECT_SEMANTIC:
      return QMI_WDS_CALL_END_REASON_MESSAGE_INCORRECT_SEMANTIC;

    case DS_NET_DOWN_REASON_INVALID_MANDATORY_INFO:
      return QMI_WDS_CALL_END_REASON_INVALID_MANDATORY_INFO;

    case DS_NET_DOWN_REASON_MESSAGE_TYPE_UNSUPPORTED:
      return QMI_WDS_CALL_END_REASON_MESSAGE_TYPE_UNSUPPORTED;

    case DS_NET_DOWN_REASON_MSG_TYPE_NONCOMPATIBLE_STATE:
      return QMI_WDS_CALL_END_REASON_MSG_TYPE_NONCOMPATIBLE_STATE;

    case DS_NET_DOWN_REASON_UNKNOWN_INFO_ELEMENT:
      return QMI_WDS_CALL_END_REASON_UNKNOWN_INFO_ELEMENT;

    case DS_NET_DOWN_REASON_CONDITIONAL_IE_ERROR:
      return QMI_WDS_CALL_END_REASON_CONDITIONAL_IE_ERROR;

    case DS_NET_DOWN_REASON_MSG_AND_PROTOCOL_STATE_UNCOMPATIBLE:
      return QMI_WDS_CALL_END_REASON_MSG_AND_PROTOCOL_STATE_UNCOMPATIBLE;

    case DS_NET_DOWN_REASON_APN_TYPE_CONFLICT:
      return QMI_WDS_CALL_END_REASON_APN_TYPE_CONFLICT;

    case DS_NET_DOWN_REASON_NO_GPRS_CONTEXT:
      return QMI_WDS_CALL_END_REASON_NO_GPRS_CONTEXT;

    case DS_NET_DOWN_REASON_FEATURE_NOT_SUPPORTED:
      return QMI_WDS_CALL_END_REASON_FEATURE_NOT_SUPPORTED;

    case DS_NET_DOWN_REASON_ILLEGAL_MS:
      return QMI_WDS_CALL_END_REASON_ILLEGAL_MS;

    case DS_NET_DOWN_REASON_ILLEGAL_ME:
      return QMI_WDS_CALL_END_REASON_ILLEGAL_ME;

    case DS_NET_DOWN_REASON_GPRS_SERVICES_AND_NON_GPRS_SERVICES_NOT_ALLOWED:
      return QMI_WDS_CALL_END_REASON_GPRS_SERVICES_AND_NON_GPRS_SERVICES_NOT_ALLOWED;

    case DS_NET_DOWN_REASON_GPRS_SERVICES_NOT_ALLOWED:
      return QMI_WDS_CALL_END_REASON_GPRS_SERVICES_NOT_ALLOWED;

    case DS_NET_DOWN_REASON_MS_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK:
      return QMI_WDS_CALL_END_REASON_MS_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK;

    case DS_NET_DOWN_REASON_IMPLICITLY_DETACHED:
      return QMI_WDS_CALL_END_REASON_IMPLICITLY_DETACHED;

    case DS_NET_DOWN_REASON_PLMN_NOT_ALLOWED:
      return QMI_WDS_CALL_END_REASON_PLMN_NOT_ALLOWED;

    case DS_NET_DOWN_REASON_LA_NOT_ALLOWED:
      return QMI_WDS_CALL_END_REASON_LA_NOT_ALLOWED;

    case DS_NET_DOWN_REASON_GPRS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN:
      return QMI_WDS_CALL_END_REASON_GPRS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN;

    case DS_NET_DOWN_REASON_PDP_DUPLICATE:
      return QMI_WDS_CALL_END_REASON_PDP_DUPLICATE;

    case DS_NET_DOWN_REASON_UE_RAT_CHANGE:
      return QMI_WDS_CALL_END_REASON_UE_RAT_CHANGE;

    case DS_NET_DOWN_REASON_APP_PREEMPTED:
      return QMI_WDS_CALL_END_REASON_APP_PREEMPTED;

    case DS_NET_DOWN_REASON_CONGESTION:
      return QMI_WDS_CALL_END_REASON_CONGESTION;

    case DS_NET_DOWN_REASON_NO_PDP_CONTEXT_ACTIVATED:
      return QMI_WDS_CALL_END_REASON_NO_PDP_CONTEXT_ACTIVATED;

    case DS_NET_DOWN_REASON_ACCESS_CLASS_DSAC_REJECTION:
      return QMI_WDS_CALL_END_REASON_ACCESS_CLASS_DSAC_REJECTION;

    case DS_NET_DOWN_REASON_MAX_NB_PDN_REACHED:
     return QMI_WDS_CALL_END_REASON_MAX_NB_PDN_REACHED;

    default:
      return QMI_WDS_CALL_END_REASON_UNSPECIFIED;  
  } /* switch ( call_end_reason ) */
}

static qmi_csi_error wdsi_gen_stop_network_interface_response
(
  qmi_error_type_v01             errval,
  qmi_req_handle                 req_handle
)
{
  wds_stop_network_interface_resp_msg_v01  resp_msg; 
  qmi_csi_error        return_val  =QMI_CSI_NO_ERR;
/*---------------------------------------------------------------------------*/
  memset(&resp_msg,0,sizeof(resp_msg));
  DS_MSG1(MSG_LEGACY_ERROR,"wdsi_gen_stop_network_interface_response "
                              "Sending stop network resp %p",req_handle);

  resp_msg.resp.result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01
                                                    : QMI_RESULT_FAILURE_V01);
  resp_msg.resp.error = errval;
  return_val = qmi_csi_send_resp(req_handle, QMI_WDS_STOP_NETWORK_INTERFACE_RESP_V01,
                   &resp_msg, sizeof(wds_stop_network_interface_resp_msg_v01));
  return return_val;
}

/*===========================================================================
  FUNCTION WDSI_REG_RMNET_PDN_EV()

  DESCRIPTION
   Api handles all the PDN events occured on rmnet inst
   
  PARAMETERS
     client_handle  :           client handle     
     event     :                pdn event
     event_info   :             pdn event 
     user_data_ptr :           data regsitered at the time of callback registration
  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void wdsi_reg_rmnet_pdn_ev
(  
  ds_fwk_clnt_handle_type    client_handle,
  ds_pdn_event_enum_type     event,
  ds_pdn_event_info_u_type   event_info,
  void                      *user_data_ptr
)
{
 qmi_wds_client_state_map   *wdsi_sp = NULL;
 qmi_wdsi_client_info_type  *client_info =NULL;
 boolean  send_pkt_svc_ind = FALSE; 
  ds_stats_type              stats={0};
 qmi_wdsi_client_info_type  *sni_client_info;
 int                         msg_id  = QMI_WDS_START_NETWORK_INTERFACE_RESP_V01;
 uint32                      req_mask  = QMI_WDS_MASK_DNS_ADDR |QMI_WDS_MASK_IP_ADDR|
                                         QMI_WDS_MASK_GATEWAY|QMI_WDS_MASK_MTU
                                         |QMI_WDS_MASK_IP_FAMILY|QMI_WDS_MASK_OP_RES_PCO;
 rmnet_smi_info_type         *rm_info = NULL;
  
/*------------------------------------------------------------------*/ 
   if((client_handle == DS_FWK_INVALID_CLNT_HANDLE)||
      (qmi_wds_cl_sp_map[client_handle] == NULL))
  {
      return;
  }
    DS_MSG2(MSG_LEGACY_ERROR,"wdsi_reg_rmnet_iface_ev called with event %d "
                             "client_handle %d",event,client_handle);
  /*------------------------------------------------------------
    Find the wds_sp from the client handle as there is 1-1 mapping of 
     rmnet_instance and wds_sp instance
  -------------------------------------------------------------*/
  wdsi_sp = qmi_wds_cl_sp_map[client_handle];
 /*------------------------------------------------------------------------
   Process flow control events first since they may get posted during data
   transfer
 -------------------------------------------------------------------------*/
  client_info = wdsi_sp->client_list;
  sni_client_info = wdsi_sp->sni_pending_list;
 
 if(wdsi_sp->pkt_svc_state != WDS_CONNECTION_STATUS_DISCONNECTED_V01)
     send_pkt_svc_ind = TRUE;

  switch( event )
  {
    case PDN_DOWN_EV:
    {
      wdsi_sp->call_end_reason = qmi_wdsi_map_wds_call_end_reason(
                                   event_info.pdn_down_info.netdown_reason);

      wdsi_sp->call_end_reason_verbose = 
             (uint32) event_info.pdn_down_info.netdown_reason;

      DS_MSG1(MSG_LEGACY_MED,"PDN down event  reason [%d]",
                        event_info.pdn_down_info.netdown_reason );
      rm_info = (rmnet_smi_info_type *)rmnet_get_smi_ptr(client_handle);
      if(rm_info != NULL)
      {
         memscpy(&stats,
                  sizeof(ds_stats_type),
                 &(rm_info->meta_sm.call_stats),
                  sizeof(ds_stats_type) 
                 );
      }
    /*---------------------------------------------------------------------------
       Send start network response to the 1st client which have started the SNI call 
       Send stop network response to the last client which have called stop network 
       Interface
       Send Packet service indication to 1st client which started the data call.
       Send Packet service indication to last client which stoped the data call
    ----------------------------------------------------------------------------*/
     while(sni_client_info != NULL)
     {
       DS_MSG1(MSG_LEGACY_HIGH,"wdsi_reg_rmnet_iface_ev net bound" 
                   "%d ",sni_client_info->net_bound);
         wdsi_gen_start_network_interface_response(QMI_ERR_CALL_FAILED_V01,
                                                   sni_client_info->req_handle,
                                                   msg_id,
                                                    wdsi_sp->call_end_reason,
                                                   wdsi_sp->\
                                                   call_end_reason_verbose,
                                                    0,NULL);

           DS_MSG1(MSG_LEGACY_HIGH,"wdsi_reg_rmnet_iface_ev Call failed" 
                   "Sending start n/w response %p ",sni_client_info);

           sni_client_info->net_bound = FALSE; 
           sni_client_info = sni_client_info->sni_pending_next;
     }
     wdsi_sp->sni_pending_list = NULL;
/*------------------------------------------------------------------------------
       When SNI passed and now stop is called send STOP response and PKT 
       service indication
       Handles both client initiated and network initiated teardown,as in 
       network initiated only we send pkt service indication
 --------------------------------------------------------------------------------------*/
       if((wdsi_sp->stop_nw_client !=NULL)&&
          (client_info == wdsi_sp->stop_nw_client))
       {       
         wdsi_sp->stop_nw_client = NULL;
         if(QMI_CSI_NO_ERR == wdsi_gen_stop_network_interface_response(
                                 QMI_ERR_NONE_V01,client_info->req_handle))
          {
            DS_MSG1(MSG_LEGACY_HIGH,"wdsi_reg_rmnet_iface_ev Stop passed "
                              "Sending stop n/w response %p ",client_info);
          }
      }
      if(send_pkt_svc_ind  == TRUE)
      {
        while(client_info !=NULL)
        {
           qmi_wdsi_send_event_report_ind_msg(WDSI_REPORT_STATUS_EV_PKT_STATS,
                                             &stats,
                                              client_info);
          
           qmi_wdsi_pkt_srvc_status_ind_msg(wdsi_sp,client_info,
                                       WDS_CONNECTION_STATUS_DISCONNECTED_V01,
                                       FALSE,0);
           DS_MSG1(MSG_LEGACY_HIGH,"wdsi_reg_rmnet_iface_ev Sending packet "
                   "service indiaction with disconnected status to client %p "
                   ,client_info);
            client_info = client_info->next;
        }
      }
      break;
    }
    case PDN_UP_EV:
    {
    /*---------------------------------------------------------------------
       Traffic channel on UM has come up. This event is used for Relay and
       network model call setup.
    ---------------------------------------------------------------------*/
      while(sni_client_info != NULL)
      {
        //Check if client exists in sni_pending_list
        if(sni_client_info->net_bound == TRUE)
        {
          wdsi_gen_start_network_interface_response(QMI_ERR_NONE_V01,
                                                    sni_client_info->req_handle,
                                                    msg_id,
                                                    wdsi_sp->call_end_reason,
                                                    wdsi_sp->\
                                                    call_end_reason_verbose,
                                                    wdsi_sp->binding.mux_id,
                                                    sni_client_info);
          DS_MSG1(MSG_LEGACY_HIGH,"wdsi_reg_rmnet_iface_ev Call passed" 
                   "Sending start n/w response %p ",sni_client_info);
        } 
        sni_client_info = sni_client_info->sni_pending_next;
      }
      wdsi_sp->sni_pending_list = NULL;
   
      while(client_info != NULL)
      {

        //Send packet service indication to only client whose SNI was sent to fwk. 
           qmi_wdsi_pkt_srvc_status_ind_msg(wdsi_sp,client_info,
                                           WDS_CONNECTION_STATUS_CONNECTED_V01,
                                           FALSE,req_mask);
           DS_MSG1(MSG_LEGACY_HIGH,"wdsi_reg_rmnet_iface_ev Sending packet "
                  "service indiaction with connected status to client %p "
                  ,client_info);
           client_info = client_info->next;
      }
      break;
    }
    case PDN_ADDR_CHANGED_EV:
    case PDN_PREFIX_UPDATE_EV:
    {
      if(send_pkt_svc_ind == TRUE)
      {
        while(client_info != NULL)
        {
           qmi_wdsi_pkt_srvc_status_ind_msg(wdsi_sp,client_info,
                                            WDS_CONNECTION_STATUS_CONNECTED_V01,TRUE,
                                            QMI_WDS_MASK_REQ_SETTINGS_IP_ADDR_V01);           
           client_info = client_info->next;
        }
      }
      break;
    }
    case PDN_EXTENDED_IP_CONFIG_EX_EV:
    {
      if(send_pkt_svc_ind == TRUE)
      {
        while(client_info != NULL)
        {
           qmi_wdsi_pkt_srvc_status_ind_msg(wdsi_sp,client_info,
                                            WDS_CONNECTION_STATUS_CONNECTED_V01,FALSE,
                                            event_info.extended_ip_config_change_mask);           
          client_info = client_info->next;
        }
        
      }
      break;
    }
    default:
    {
      DS_MSG1(MSG_LEGACY_ERROR,"Unregistered event %d recvd, ignoring", event);
      break;
    }
 }
}

static qmi_csi_cb_error qmi_wdsi_send_event_report_ind_msg
( 
  uint32                                      event_mask,
  ds_stats_type                              *stats,
  qmi_wdsi_client_info_type                  *clnt_info
)
{
  wds_event_report_ind_msg_v01 *indication = NULL;
  /*-----------------------------------------------------
      Build indication from the back to the front
    ----------------------------------------------------*/
  if(event_mask & WDSI_REPORT_STATUS_EV_PKT_STATS)
  {
    if(clnt_info == NULL)
    {
      return QMI_CSI_CB_INTERNAL_ERR;
    }

    if(stats == NULL)
    {
      goto send_result;
    }
  
    DS_SYSTEM_HEAP_MEM_ALLOC(indication,sizeof(wds_event_report_ind_msg_v01),
                             wds_event_report_ind_msg_v01*);
    if(indication == NULL)
    {
      goto send_result;
    }

    memset(indication,0,sizeof(wds_event_report_ind_msg_v01));

    DS_MSG6(MSG_LEGACY_ERROR,"Indicate outgoing ok pkts: %d, incoming ok pkts:%d"
           "RX ok bytes: %d,TX ok bytes: %d,"
           "RX pkts dropped: %d, TX pkts dropped: %d",                       
             stats->pkts_rx,
             stats->pkts_tx,
             stats->bytes_rx,
             stats->bytes_tx,
             stats->pkts_dropped_rx,
             stats->pkts_dropped_tx);
  
    indication->tx_ok_count_valid = TRUE;
    indication->tx_ok_count = stats->pkts_rx;
  
    indication->rx_ok_count_valid = TRUE;
    indication->rx_ok_count = stats->pkts_tx;
    
    indication->tx_ok_bytes_count_valid = TRUE;
    indication->tx_ok_bytes_count = stats->bytes_rx;
  
    indication->rx_ok_bytes_count_valid = TRUE;
    indication->rx_ok_bytes_count = stats->bytes_tx;
  
    indication->tx_dropped_count_valid = TRUE;
    indication->tx_dropped_count= stats->pkts_dropped_rx;
  
    indication->rx_dropped_count_valid = TRUE;
    indication->rx_dropped_count = stats->pkts_dropped_tx;
  } 
  send_result:  
  qmi_csi_send_ind(clnt_info->clnt, 
                   QMI_WDS_EVENT_REPORT_IND_V01,
                   indication, 
                   sizeof(wds_event_report_ind_msg_v01));
  DS_SYSTEM_HEAP_MEM_FREE(indication);
  return QMI_CSI_CB_NO_ERR;
}

static qmi_csi_cb_error  qmi_wdsi_pkt_srvc_status_ind_msg
(
 qmi_wds_client_state_map                   *wds_sp,  
 qmi_wdsi_client_info_type                  *clnt_info, 
 wds_connection_status_enum_v01              pkt_svc_state,
 boolean                                     reconfig_flag,
 ds_ioctl_extended_ip_config_ex_type         changed_config_mask
)
{
  wds_pkt_srvc_status_ind_msg_v01           indication;
  ip_addr_type                              pri_dns = {IP_ANY_ADDR, {0}};
  ip_addr_type                              sec_dns = {IP_ANY_ADDR, {0}};
  uint32	                                ipv4_addr;
  ds_fwk_clnt_info_s_type                   clnt_info_ptr ={0};  
  ps_ip_addr_type                           if_addr;  
  uint64                                  * iid = NULL;
  ip_addr_type                              dns_addr[2]={{0}};
  ds_pdn_ioctl_get_all_dns_addrs_type       dns_addrs={0};  
  int                                       num_retry = 0;
  ds_pdn_op_reserved_pco_type               op_reserved_pco={0};  
  ps_ip_addr_type                           gateway_ip_addr ={0};
  ds_pdn_ioctl_subnet_mask_type             subnet_mask ={0};
  sint15                                    ds_errno;  
  int16                                     ps_errno_addr_mgmt = 0;
  ip_addr_type                              v6_pri_dns_man = {IP_ANY_ADDR, {0}};
  ip_addr_type                              v6_sec_dns_man = {IP_ANY_ADDR, {0}};
  ps_addr_mgmt_handle_type                  handle;
  ps_addr_mgmt_alloc_type                   alloc_info;
  uint32                                    mtu =0;  
  struct ps_in6_addr                        ipv6_addr = {{{0},}};
  sint15                                    ps_errno_iid;
  boolean                                   family = FALSE;
  ip_addr_type		                        pri_dns_ip_addr = {IP_ANY_ADDR, {0}};
  ip_addr_type		                        sec_dns_ip_addr = {IP_ANY_ADDR, {0}};

/*--------------------------------------------------------------------------------*/  
  memset(&indication, 0, sizeof(wds_pkt_srvc_status_ind_msg_v01));
  wds_sp->pkt_svc_state =  pkt_svc_state;
  
  DS_MSG2(MSG_LEGACY_ERROR,"qmi_wdsi_pkt_srvc_status_ind_msg "
                              "Sending packet service indicatin client info %p "
                              "state  %d ",clnt_info,pkt_svc_state);
  if (WDS_CONNECTION_STATUS_DISCONNECTED_V01 == wds_sp->pkt_svc_state)
  {
    indication.verbose_call_end_reason_valid = TRUE;
    indication.verbose_call_end_reason.call_end_reason_type = 
                        (wds_verbose_call_end_reason_type_enum_v01)
                         (wds_sp->call_end_reason_verbose >> 16);
     
    indication.verbose_call_end_reason.call_end_reason = 
     (uint16)(0x0000FFFF&wds_sp->call_end_reason_verbose);
    indication.call_end_reason_valid = TRUE;
    indication.call_end_reason = 
     (wds_call_end_reason_enum_v01)wds_sp->call_end_reason;

    indication.status.connection_status = wds_sp->pkt_svc_state;
    goto send_result;
  }

  //connection status
  indication.status.connection_status = wds_sp->pkt_svc_state;
  indication.status.reconfiguration_required  = reconfig_flag;

  if(changed_config_mask & QMI_WDS_MASK_IP_FAMILY)
     family = TRUE;
  if(wds_sp->pkt_svc_state == WDS_CONNECTION_STATUS_CONNECTED_V01)
  {  
    dns_addrs.num_dns_addrs =2;
    dns_addrs.dns_addrs_ptr = dns_addr;

  ds_fwk_get_clnt_info_from_handle(DS_FWK_CLIENT_RMNET,wds_sp->binding.rmnet_inst,
                                  &clnt_info_ptr);
  if(clnt_info_ptr.fw_inst == NULL)
  {   
    DS_MSG0(MSG_LEGACY_ERROR,"No framework "
           "instance could be found for this call");
    goto send_result;
  }
   //Call IOCTLS to get the required data
    ds_pdn_ioctl(DS_FWK_CLIENT_RMNET,wds_sp->binding.rmnet_inst,
                 DS_PDN_IOCTL_GET_IP_ADDR,&if_addr,&ds_errno);

    //ds_pdn_ioctl(DS_FWK_CLIENT_RMNET,wds_sp->binding.rmnet_inst,
      //           DS_PDN_IOCTL_GET_GATEWAY_ADDR,&gateway_ip_addr,&ds_errno);
	 
    //ds_pdn_ioctl(DS_FWK_CLIENT_RMNET,wds_sp->binding.rmnet_inst,
      //           DS_PDN_IOCTL_GET_SUBNET_MASK,&subnet_mask,&ds_errno);
	 
    ds_pdn_ioctl(DS_FWK_CLIENT_RMNET,wds_sp->binding.rmnet_inst,
                 DS_PDN_IOCTL_GET_ALL_DNS_ADDR,&dns_addrs,&ds_errno);
	 
    ds_pdn_ioctl(DS_FWK_CLIENT_RMNET,wds_sp->binding.rmnet_inst,
                 DS_PDN_IOCTL_GET_MTU,&mtu,&ds_errno);

  if(wds_sp->ip_call_type == QMI_AF_NIPD )
  {
       if(changed_config_mask & QMI_WDS_MASK_MTU)
       {
          indication.mtu_valid = TRUE;
          indication.mtu = mtu;
          indication.changed_ip_config_mask_valid = TRUE;
          indication.changed_ip_config_mask = changed_config_mask;
          goto send_result;
      }
  }
 
  if(wds_sp->ip_call_type == QMI_AF_INET)
  {
     if(family == TRUE)
     {
       indication.ip_family_valid =TRUE;
     indication.ip_family = WDS_IP_FAMILY_IPV4_V01;
     }

     if((changed_config_mask & QMI_WDS_MASK_IP_ADDR) &&(if_addr.addr.v4.ps_s_addr != 0))
     {
       ipv4_addr = ps_ntohl(if_addr.addr.v4.ps_s_addr);
         indication.ipv4_address_valid = TRUE;
         indication.ipv4_address = ipv4_addr;
     }
     else
       changed_config_mask  &= (~QMI_WDS_MASK_IP_ADDR);
      
       memset(&pri_dns, 0, sizeof(ip_addr_type));
       memset(&sec_dns, 0, sizeof(ip_addr_type));
       memset(&pri_dns_ip_addr, 0, sizeof(ip_addr_type));
       memset(&sec_dns_ip_addr, 0, sizeof(ip_addr_type));

    /* Convert DNS addrs to little endian to send to host */
       pri_dns.addr.v4 = ps_ntohl(dns_addr[0].addr.v4);
       sec_dns.addr.v4 = ps_ntohl(dns_addr[1].addr.v4);

	   rmnet_meta_sm_get_ipv4_manual_dns_addrs(
                                             wds_sp->binding.rmnet_inst, 
                                             &pri_dns_ip_addr, 
                                             &sec_dns_ip_addr);
	 
       /* if the pri is set (type==IPV4) and non-zero, save ptr for later */
       if ((IPV4_ADDR == pri_dns_ip_addr.type) && 
            (0 != pri_dns_ip_addr.addr.v4))
       {
          pri_dns.type     = pri_dns_ip_addr.type;
          pri_dns.addr.v4  = ps_htonl(pri_dns_ip_addr.addr.v4);
       }
       if ((IPV4_ADDR == sec_dns_ip_addr.type) && 
           (0 != sec_dns_ip_addr.addr.v4))
       {
          sec_dns.type     = sec_dns_ip_addr.type;
          sec_dns.addr.v4  = ps_htonl(sec_dns_ip_addr.addr.v4);
       }
		  
       if(changed_config_mask & QMI_WDS_MASK_DNS_ADDR)
       {
         if(0 != sec_dns.addr.v4)
         {
           indication.secondary_DNS_IPv4_address_valid = TRUE;
           indication.secondary_DNS_IPv4_address = sec_dns.addr.v4;
         }
         if(0 != pri_dns.addr.v4)
         {
           indication.primary_DNS_IPv4_address_valid = TRUE;
           indication.primary_DNS_IPv4_address = pri_dns.addr.v4;
         }
       }
       if((0 == pri_dns.addr.v4)&&(0 == sec_dns.addr.v4))
	   	 changed_config_mask &= (~QMI_WDS_MASK_DNS_ADDR);

      
      if(changed_config_mask & QMI_WDS_MASK_GATEWAY)
      { 
         ds_fwk_calc_gateway_addr(if_addr.addr.v4.ps_s_addr,&(gateway_ip_addr.addr.v4.ps_s_addr),
                               &(subnet_mask.addr.v4));
        indication.ipv4_subnet_mask_valid = TRUE; 
        indication.ipv4_subnet_mask 	  =  ps_ntohl(subnet_mask.addr.v4);
        indication.ipv4_gateway_addr_valid =  TRUE;
        indication.ipv4_gateway_addr = 
                       ps_ntohl(gateway_ip_addr.addr.v4.ps_s_addr);
      }
      else
        changed_config_mask  &= (~QMI_WDS_MASK_GATEWAY);
      
      if(changed_config_mask & QMI_WDS_MASK_MTU)
      {
        if(mtu != 0)
        {
         indication.mtu_valid = TRUE;
         indication.mtu = mtu;
        }        
        else 
         changed_config_mask &= (~QMI_WDS_MASK_MTU);
     }
  }
    
  else if(wds_sp->ip_call_type == QMI_AF_INET6)
  {
    memset(&v6_pri_dns_man, 0, sizeof(ip_addr_type));
    memset(&v6_sec_dns_man, 0, sizeof(ip_addr_type));
    /*-----------------------------------------------------------------------
      Query the 'manual' DNS addresses and over-ride auto-configured values if 
     set
    -----------------------------------------------------------------------*/
    rmnet_meta_sm_get_ipv6_manual_dns_addrs(wds_sp->binding.rmnet_inst,
                                            &v6_pri_dns_man,
                                            &v6_sec_dns_man);
    if (IPV6_ADDR == v6_pri_dns_man.type)
    {
      if ((0 == v6_pri_dns_man.addr.v6[0]) &&
          (0 == v6_pri_dns_man.addr.v6[1]))
      {
         DS_MSG0(MSG_LEGACY_MED,"Manual IPv6 primary DNS,"
                 "Ignoring IPv6 primary DNS address");
      }
      else
      {
         DS_MSG0(MSG_LEGACY_MED,"Using manual IPv6 primary DNS address");
         memscpy(&dns_addr[0], sizeof(dns_addr[0]), &v6_pri_dns_man,
                 sizeof(ip_addr_type));
      }
    }
    else
    {
       DS_MSG0(MSG_LEGACY_MED,"Manual IPv6 primary DNS address not provisioned");
    }  
    if (IPV6_ADDR == v6_sec_dns_man.type)
    {
      if((0 == v6_sec_dns_man.addr.v6[0]) && (0 == v6_sec_dns_man.addr.v6[1]))
      {
         DS_MSG0(MSG_LEGACY_MED,"Manual IPv6 secondary DNS,"
                 "Ignoring manual IPv6 secondary DNS address");
      }
      else
      {
         DS_MSG0(MSG_LEGACY_MED,"Using manual IPv6 secondary DNS address");
         memscpy(&dns_addr[1], sizeof(dns_addr[1]),
                 &v6_sec_dns_man, sizeof(ip_addr_type));
      }
    }
    else
    {
       DS_MSG0(MSG_LEGACY_MED,"Manual IPv6 secondary"
               "DNS address not provisioned");
    } 
    if(changed_config_mask & QMI_WDS_MASK_DNS_ADDR)
    {
        if(0 != dns_addr[0].addr.v6[0])
        {
            indication.primary_dns_IPv6_address_valid = TRUE;
            memscpy(indication.primary_dns_IPv6_address,
                    sizeof(indication.primary_dns_IPv6_address),
                    dns_addr[0].addr.v6,
                    sizeof(dns_addr[0].addr.v6));
        } 
        else
        {
           DS_MSG0(MSG_LEGACY_MED,"v6_pri_dns.addr.v6[0]=0, omitting TLV");
        }
        if(0 != dns_addr[1].addr.v6[0])
        { 
          indication.secondary_dns_IPv6_address_valid = TRUE;
           memscpy(indication.secondary_dns_IPv6_address,
                    sizeof(indication.secondary_dns_IPv6_address),
                    dns_addr[1].addr.v6,
                   sizeof(dns_addr[1].addr.v6));
        }
        else
        {
           DS_MSG0(MSG_LEGACY_MED,"v6_sec_dns.addr.v6[0]=0, omitting TLV");
        }
     }
     if((0 == dns_addr[0].addr.v6[0])&&(0 == dns_addr[1].addr.v6[0]))
	   changed_config_mask &= (~QMI_WDS_MASK_DNS_ADDR);
    
     if(family == TRUE)
     {
        indication.ip_family_valid =TRUE;
        indication.ip_family = WDS_IP_FAMILY_IPV6_V01;
     }
     if(changed_config_mask & QMI_WDS_MASK_MTU)
     {
      if(mtu !=0)
      {
         indication.mtu_valid = TRUE;
         indication.mtu       = mtu;
      }        
      else                    
        changed_config_mask &= (~QMI_WDS_MASK_MTU);
            
     }

    if((changed_config_mask & QMI_WDS_MASK_IP_ADDR )&&(if_addr.addr.v6.ps_s6_addr64[0] !=0))
    {
    /*-----------------------------------------------------------------------
      Get Um Iface Address and send to host in network byte order.
    -----------------------------------------------------------------------*/
     ipv6_addr.ps_s6_addr64[0] = if_addr.addr.v6.ps_s6_addr64[0];
     if(FALSE == ds_fwk_get_external_ipv6_addrs(clnt_info_ptr.fw_inst ,&ipv6_addr))
     { 
       iid = &ipv6_addr.ps_s6_addr64[1];
       if (-1 == ps_generate_ipv6_iid(clnt_info_ptr.fw_inst, iid, &ps_errno_iid))
       {
         DS_MSG0(MSG_LEGACY_ERROR,"Failed to generate a random IPv6 IID");
       }          
       /* Add the address to UM database
          If there is a conflict re-generate IID up to 3 times
       */
       num_retry = 0;
       while (TRUE)
       {
         handle = ps_addr_mgmt_get_handle_from_ip(clnt_info_ptr.fw_inst, & ipv6_addr);
         if (-1 == handle)
         {
           /* No match found, add to database */
           memset(&alloc_info, 0, sizeof(alloc_info));
           alloc_info.ip_addr.type    = IPV6_ADDR;
           alloc_info.ip_addr.addr.v6 = ipv6_addr;
           alloc_info.addr_type       = DS_IPV6_ADDR_TYPE_EXTERNAL;
           if (-1 == ps_addr_mgmt_alloc_unique_addr(clnt_info_ptr.fw_inst, 
                                                    &handle,
                                                    &alloc_info,
                                                    DS_FWK_CLIENT_RMNET,
                                                    &ps_errno_addr_mgmt
                                                    ))
           {
             DS_MSG1(MSG_LEGACY_ERROR,"Failed to add IPv6 addr to pdn database."
                     "errno [%d]",ps_errno_addr_mgmt);
             /*Clean tlvs added till now.Send failure*/
             DS_ASSERT(0);
           }
           else
           {
             DS_MSG1(MSG_LEGACY_HIGH,"Added IPv6 addr to pdn database %x ",
                                      ipv6_addr.ps_s6_addr64[1]);
             
             IPV6_ADDR_MSG(ipv6_addr.ps_s6_addr64);
           }
           break;
         }
         else
         {
           IPV6_ADDR_MSG(ipv6_addr.ps_s6_addr64);
           /* Generate another address since there is a conflict */
           if (num_retry < 3)
           {
             DS_MSG0(MSG_LEGACY_MED,"IPv6 addr already in database, re-generate IID");
             if (-1 == ps_generate_ipv6_iid(clnt_info_ptr.fw_inst, iid, &
                                            ps_errno_iid))
             {
               DS_MSG0(MSG_LEGACY_ERROR,"Failed to generate a random IPv6  IID");              
               DS_ASSERT(0);
             }
             num_retry++;
           }
           else
           {
             DS_MSG1(MSG_LEGACY_ERROR,"Max retry [%d], give up writing to PDN DB.",
                             num_retry);
             DS_ASSERT(0);
             goto send_result;
           }
         }
       } /* while */
      }
      indication.ipv6_addr_valid =  TRUE;  
      indication.ipv6_addr.ipv6_prefix_length = clnt_info_ptr.fw_inst->v6_net_info.ipv6_addrs[0]->prefix_len;
      memscpy(indication.ipv6_addr.ipv6_addr,sizeof(indication.ipv6_addr.ipv6_addr),
             ipv6_addr.ps_s6_addr64,sizeof(ipv6_addr.ps_s6_addr64));
      } 
    
      if(changed_config_mask & QMI_WDS_MASK_GATEWAY) 
      {
        if(wds_sp->gateway_iid == 0)
        {
           DS_MSG0(MSG_LEGACY_MED,"Generating iid for the gateway");
           if (-1 == ps_generate_ipv6_iid(clnt_info_ptr.fw_inst, &(wds_sp->gateway_iid), &
                                         ps_errno_iid))
            {
                 DS_MSG0(MSG_LEGACY_ERROR,"Failed to generate a random IPv6 IID");              
                 DS_ASSERT(0);
            }
          }
          gateway_ip_addr.addr.v6.ps_s6_addr64[0] = ipv6_addr.ps_s6_addr64[0];
          gateway_ip_addr.addr.v6.ps_s6_addr64[1] = wds_sp->gateway_iid;
        indication.ipv6_gateway_addr_valid = TRUE;
        indication.ipv6_gateway_addr.ipv6_prefix_length =
                 WDSI_IPV6_DEFAULT_PREFIX_LEN;
        memscpy(indication.ipv6_gateway_addr.ipv6_addr,
                sizeof(indication.ipv6_gateway_addr.ipv6_addr),
                gateway_ip_addr.addr.v6.ps_s6_addr64,
                sizeof(gateway_ip_addr.addr.v6.ps_s6_addr64));
      }
      else
         changed_config_mask &= (~QMI_WDS_MASK_GATEWAY);
    
     }
     if(changed_config_mask & QMI_WDS_MASK_OP_RES_PCO)
     {
        ds_pdn_ioctl(DS_FWK_CLIENT_RMNET,wds_sp->binding.rmnet_inst,
                     DS_PDN_IOCTL_GET_OP_RES_PCO,&op_reserved_pco,&ds_errno);
       if(ds_errno == DS_EINVAL)
       {
           DS_MSG0(MSG_LEGACY_MED,"PCO information is unavailable !!!");
               changed_config_mask &= (~QMI_WDS_MASK_OP_RES_PCO);
       }
       else   
       {
       /*--------------------------------------------
        Construct the PCO packet
       ----------------------------------------------*/
      /* Retrieve mcc field and copy into buffer*/
       indication.operator_reserved_pco_valid = TRUE;
       indication.operator_reserved_pco.mcc = op_reserved_pco.mcc;
      /* Retrieve mnc field and copy into buffer*/
       indication.operator_reserved_pco.mnc = op_reserved_pco.mnc;
      /* Retrieve mnc_includes_pcs_digit field and copy into buffer*/
       indication.operator_reserved_pco.mnc_includes_pcs_digit = 
                     op_reserved_pco.mnc_includes_pcs_digit;
	   
       /* Retrieve app_specific_info_len field and copy into buffer*/

        indication.operator_reserved_pco.app_specific_info_len = 

	                        op_reserved_pco.app_specific_info_len;
					/*   Retrieve app_specific_info field and copy into buffer*/
       memscpy(indication.operator_reserved_pco.app_specific_info,
               sizeof(indication.operator_reserved_pco.app_specific_info),
               (void *)&op_reserved_pco.app_specific_info,
               op_reserved_pco.app_specific_info_len);
      /* Retrieve container_id field and copy into buffer*/
       indication.operator_reserved_pco.container_id = 
                op_reserved_pco.container_id;
       }              
    }   
  }
  indication.changed_ip_config_mask_valid = TRUE;
  indication.changed_ip_config_mask     = changed_config_mask;

 send_result:  
  qmi_csi_send_ind(clnt_info->clnt, 
                   QMI_WDS_PKT_SRVC_STATUS_IND_V01,
                   &indication, 
                    sizeof(wds_pkt_srvc_status_ind_msg_v01));
  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION QMI_WDSI_GET_CAPABILITIES()

  DESCRIPTION
    Queries the modem capabilities

  PARAMETERS
      req_c_struct: Request Message with all TLV
      req_handle  : Request Handle
      clnt_info   : Client Info Handle
  
    RETURN VALUE  
      qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS

  None
===========================================================================*/
static qmi_csi_cb_error qmi_wdsi_get_capabilities
(
  qmi_wdsi_client_info_type        *clnt_info,
  qmi_req_handle          req_handle,
  int                     msg_id,
  void                   *req_c_struct,
  int                     req_c_struct_len,
  void                   *service_handle
)
{
  wds_get_capabilities_req_msg_v01 *req_msg;
  wds_get_capabilities_resp_msg_v01 resp_msg;
  qmi_error_type_v01                    errval;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  errval = QMI_ERR_NONE_V01;
  memset(&resp_msg,0,sizeof(resp_msg));
  req_msg  = (wds_get_capabilities_req_msg_v01*)req_c_struct;

  if((req_msg->mux_id_support_capability_valid)&&(req_msg->mux_id_support_capability))  
  {
    resp_msg.mux_id_support_valid = TRUE;
    resp_msg.mux_id_support = WDS_MUX_ID_SUPPORT_BY_MODEM_V01;
  }
  
  if((req_msg->bring_up_capability_valid)&&(req_msg->bring_up_capability))
  {
    resp_msg.bring_up_capability_valid  = TRUE;
    resp_msg.bring_up_capability        =WDS_BRING_UP_BY_APN_NAME_V01;
  }

    resp_msg.resp.result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01
                                          : QMI_RESULT_FAILURE_V01);
    resp_msg.resp.error  = errval;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(wds_get_capabilities_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
 FUNCTION QMI_WDSI_GET_PKT_STATISTICS()


 DESCRIPTION
   Get the packet statistics

  PARAMETERS
     req_c_struct  :            Request Message with all TLV
     req_handle  :              Request Handle
     clnt_info   :              Client Info Handle
     msg_id             :       Message ID
     req_c_struct_len   :       length of request message
     service_handle     :       wds service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error    qmi_wdsi_get_pkt_statistics
(
  qmi_wdsi_client_info_type        *clnt_info,
  qmi_req_handle                    req_handle,
  int                               msg_id,
  void                             *req_c_struct,
  int                               req_c_struct_len,
  void                             *service_handle
)
{
  qmi_wds_client_state_map                *wds_sp =NULL;
  qmi_error_type_v01                       errval = QMI_ERR_NONE_V01;
  wds_get_pkt_statistics_req_msg_v01      *req_msg;
  wds_get_pkt_statistics_resp_msg_v01      resp_msg;
  ds_fwk_clnt_info_s_type                  clnt_info_ptr ={0};
  ds_stats_type                            wds_stats={0};
  ds_fwk_s_type                           *fwk_inst=NULL;
  ip_addr_enum_type                        call_type;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset(&resp_msg,0,sizeof(wds_get_pkt_statistics_resp_msg_v01));
  wds_sp = qmi_get_wds_state(clnt_info);
  if(wds_sp == NULL)
  {
    errval = QMI_ERR_OUT_OF_CALL_V01;
    goto send_result;
  }
  req_msg= (wds_get_pkt_statistics_req_msg_v01 *)req_c_struct;

  if(req_msg->stats_mask == 0)
  {
    errval = QMI_ERR_MISSING_ARG_V01;
    goto send_result;
  }
  DS_MSG1(MSG_LEGACY_MED,"Got statistics require for mask %x",
          req_msg->stats_mask);

  if (!rmnet_meta_sm_in_call(wds_sp->binding.rmnet_inst))
  {
    errval = QMI_ERR_OUT_OF_CALL_V01;
    DS_MSG1(MSG_LEGACY_MED,"Got statistics require for mask %x",
            req_msg->stats_mask);
    goto send_result;
  }

  ds_fwk_get_clnt_info_from_handle(DS_FWK_CLIENT_RMNET,wds_sp->binding.rmnet_inst,
                                                 &clnt_info_ptr);
  if(clnt_info_ptr.fw_inst == NULL)
  {   
    DS_MSG0(MSG_LEGACY_ERROR,"No framework "
           "instance could be found for this call");
    errval = QMI_ERR_OUT_OF_CALL_V01;
    goto send_result;
  }
  call_type = clnt_info_ptr.ip_type;
  fwk_inst = clnt_info_ptr.fw_inst;
  //Get all the stats
  errval = QMI_ERR_NONE_V01;

  if((E_SUCCESS != ds_fwk_get_all_stats
                   (PS_STAT_FWK_PKTS_RX,fwk_inst,&wds_stats.pkts_tx,4,call_type))
    ||(E_SUCCESS != ds_fwk_get_all_stats(
                     PS_STAT_FWK_PKTS_TX,fwk_inst,&wds_stats.pkts_rx ,4,call_type))
    ||(E_SUCCESS !=  ds_fwk_get_all_stats(
                     PS_STAT_FWK_BYTES_RX,fwk_inst,&wds_stats.bytes_tx,8,call_type))
    ||(E_SUCCESS !=  ds_fwk_get_all_stats(
                     PS_STAT_FWK_BYTES_TX,fwk_inst,&wds_stats.bytes_rx,8,call_type))
    ||(E_SUCCESS !=  ds_fwk_get_all_stats(
                     PS_STAT_FWK_PKTS_DROPPED_RX,fwk_inst,&wds_stats.pkts_dropped_tx,4,call_type))
    ||(E_SUCCESS != ds_fwk_get_all_stats(
                    PS_STAT_FWK_PKTS_DROPPED_TX,fwk_inst,&wds_stats.pkts_dropped_rx,4,call_type)))
  {
      DS_MSG1(MSG_LEGACY_ERROR,"Unable to get stats for fwk_inst 0x%x!",
                    fwk_inst);
      errval = QMI_ERR_INTERNAL_V01;
      goto send_result;
     
  }
  /*-------------------------------------------------------------------------
    Build the response from back to front
    Note the QMI message is communicated from the host's perspective, i.e.
    tx packet count is the number of packets rx'ed by the Rm interface.
    rx packet count is the number of packets tx'ed by the Rm interface.
  -------------------------------------------------------------------------*/
  if(req_msg->stats_mask  & WDSI_STAT_MASK_TX_GOOD_NW)
  {
    resp_msg.tx_ok_count_valid = TRUE;
    resp_msg.tx_ok_count = wds_stats.pkts_rx;
  }

  if(req_msg->stats_mask  & WDSI_STAT_MASK_RX_GOOD_NW)
  {
    resp_msg.rx_ok_count_valid = TRUE;
    resp_msg.rx_ok_count = wds_stats.pkts_tx;
  }

  if(req_msg->stats_mask  & WDSI_STAT_MASK_TX_OK_BYTES)
  {
    resp_msg.tx_ok_bytes_count_valid = TRUE;
    resp_msg.tx_ok_bytes_count = wds_stats.bytes_rx;
  }

  if(req_msg->stats_mask  & WDSI_STAT_MASK_RX_OK_BYTES)
  {
    resp_msg.rx_ok_bytes_count_valid = TRUE;
    resp_msg.rx_ok_bytes_count = wds_stats.bytes_tx;
  }

  if(req_msg->stats_mask  & WDSI_STAT_MASK_TX_DROPPED)
  {
    resp_msg.tx_dropped_count_valid = TRUE;
    resp_msg.tx_dropped_count = wds_stats.pkts_dropped_rx;
  }

  if(req_msg->stats_mask  & WDSI_STAT_MASK_RX_DROPPED)
  {
    resp_msg.rx_dropped_count_valid = TRUE;
    resp_msg.rx_dropped_count = wds_stats.pkts_dropped_tx;
  }

send_result:
  resp_msg.resp.result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01
                             : QMI_RESULT_FAILURE_V01);

  resp_msg.resp.error  = errval;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(wds_get_pkt_statistics_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
} /* qmi_wdsi_get_pkt_statistics() */

/*===========================================================================
FUNCTION QMI_WDSI_RESET_PKT_STATISTICS()

  DESCRIPTION
    Reset packet statistics

  PARAMETERS
      req_c_struct  :            Request Message with all TLV
      req_handle  :              Request Handle
      clnt_info   :              Client Info Handle
      msg_id             :       Message ID
      req_c_struct_len   :       length of request message
      service_handle     :       wds service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error    qmi_wdsi_reset_pkt_statistics
(
  qmi_wdsi_client_info_type        *clnt_info,
  qmi_req_handle                    req_handle,
  int                               msg_id,
  void                             *req_c_struct,
  int                               req_c_struct_len,
  void                             *service_handle
)
{
  qmi_wds_client_state_map                *wds_sp =NULL;
  wds_reset_pkt_statistics_resp_msg_v01    resp_msg;
  qmi_error_type_v01                       errval = QMI_ERR_NONE_V01;
  ds_fwk_clnt_info_s_type                  clnt_info_ptr ={0};
  sint15                                   ps_errno=0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&resp_msg,0,sizeof(wds_reset_pkt_statistics_resp_msg_v01));
  wds_sp = qmi_get_wds_state(clnt_info);
  if(wds_sp ==NULL)
  {
    errval = QMI_ERR_OUT_OF_CALL_V01;
    goto send_result;
  }
  // Check if call is up
  if (!rmnet_meta_sm_in_call(wds_sp->binding.rmnet_inst))
  {
    errval = QMI_ERR_OUT_OF_CALL_V01;
    goto send_result;
  }

  ds_fwk_get_clnt_info_from_handle(DS_FWK_CLIENT_RMNET,wds_sp->binding.rmnet_inst,
                                                 &clnt_info_ptr);
  if(clnt_info_ptr.fw_inst == NULL)
  {   
    DS_MSG0(MSG_LEGACY_ERROR,"No framework "
           "instance could be found for this call");
    errval = QMI_ERR_OUT_OF_CALL_V01;
    goto send_result;
  }
  ds_pdn_ioctl(DS_FWK_CLIENT_RMNET,wds_sp->binding.rmnet_inst,
               DS_PDN_IOCTL_RESET_PDN_STATS,NULL,&ps_errno);

send_result:
  resp_msg.resp.result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01
                             : QMI_RESULT_FAILURE_V01);

  resp_msg.resp.error  = errval;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(wds_reset_pkt_statistics_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
} /* qmi_wdsi_reset_pkt_statistics() */

/*===========================================================================
  FUNCTION QMI_WDSI_BIND_MUX_DATA_PORT()

  DESCRIPTION
    Bindes the mux and ep_id to physical port 

 PARAMETERS
    req_c_struct: Request Message with all TLV
    req_handle  : Request Handle
    clnt_info   : Client Info Handle

  RETURN VALUE  
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS

  None
===========================================================================*/
static qmi_csi_cb_error  qmi_wdsi_bind_mux_data_port
(
  qmi_wdsi_client_info_type        *clnt_info,
  qmi_req_handle                    req_handle,
  int                               msg_id,
  void                             *req_c_struct,
  int                              req_c_struct_len,
  void                            *service_handle 
)
{
   qmi_error_type_v01                  errval = QMI_ERR_NONE_V01;

   wds_bind_mux_data_port_req_msg_v01  *req_msg;
   wds_bind_mux_data_port_resp_msg_v01 resp_msg;
   req_msg = (wds_bind_mux_data_port_req_msg_v01 *)req_c_struct;
   resp_msg.resp.result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01
                                          : QMI_RESULT_FAILURE_V01);
    resp_msg.resp.error  = errval;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(wds_bind_mux_data_port_resp_msg_v01));
    return QMI_CSI_CB_NO_ERR;

}

  /*===========================================================================

  FUNCTION QMI_WDSI_SUBSEQUENT_DATA_TRANSFER_STATUS()

  DESCRIPTION
    Request the modem to set subsequent data transfer status
 
  PARAMETERS
   PARAMETERS
     req_c_struct  :            Request Message with all TLV
     req_handle  :              Request Handle
     clnt_info   :              Client Info Handle
     msg_id             :       Message ID
     req_c_struct_len   :       length of request message
     service_handle     :       wds service handle
     
  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error    qmi_wdsi_subsequent_data_transfer_status
(
  qmi_wdsi_client_info_type        *clnt_info,
  qmi_req_handle                    req_handle,
  int                               msg_id,
  void                             *req_c_struct,
  int                               req_c_struct_len,
  void                             *service_handle
)
{
  qmi_error_type_v01      errval ;
  sint15                  ps_err_no = 0;  
  qmi_wds_client_state_map   *wdsi_sp = NULL;
  wds_subsequent_data_transfer_status_req_msg_v01     *req_msg;
  wds_subsequent_data_transfer_status_resp_msg_v01     resp_msg;
  ds_pdn_cp_data_subseq_data_info_s                    subsequent_data;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  wdsi_sp = qmi_get_wds_state(clnt_info);
  req_msg  = (wds_subsequent_data_transfer_status_req_msg_v01*)req_c_struct;
  errval = QMI_ERR_NONE_V01;
  memset(&resp_msg,0,sizeof(wds_subsequent_data_transfer_status_resp_msg_v01));
  memset(&subsequent_data,0,sizeof(ds_pdn_cp_data_subseq_data_info_s));
  
  // Check if call is UP
  if((wdsi_sp ==NULL)||(wdsi_sp->pkt_svc_state != WDS_CONNECTION_STATUS_CONNECTED_V01))
  {
    errval = QMI_ERR_OUT_OF_CALL_V01;
    goto send_result;
  }
  
  subsequent_data.rai_val = 
  (ds_pdn_rai_ind_enum_type)req_msg->subsequent_data_transfer_status_type_mask;

  subsequent_data.subseq_data_mask = DS_PDN_CP_DATA_RAI;
  if(req_msg->next_activity_time_valid)
  {
    subsequent_data.next_activity_timer = req_msg->next_activity_time;
    subsequent_data.subseq_data_mask |=  DS_PDN_CP_DATA_NAT;
  }
  
  // Call DS ioctl
  if (ds_pdn_ioctl(DS_FWK_CLIENT_RMNET,wdsi_sp->binding.rmnet_inst,
                     DS_PDN_IOCTL_SET_SUBSEQUENT_DATA_TRANSFER_STATUS,
                     &subsequent_data,
                     &ps_err_no) != 0)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"Couldn't set subsequent data transfer , err: %d", ps_err_no);
    switch (ps_err_no)
    {
      case DS_EFAULT:
        errval = QMI_ERR_INVALID_ARG_V01;
        break;

      case DS_EOPNOTSUPP:
        errval = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
        break;

      case DS_EINVAL:
      case DS_ENETDOWN:
        errval = QMI_ERR_INVALID_OPERATION_V01;
        break;

      default:
        errval = QMI_ERR_INTERNAL_V01;
        break;
    }
  }

send_result:
 
  resp_msg.resp.result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01
                                   : QMI_RESULT_FAILURE_V01);
  resp_msg.resp.error  = errval;
    
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(wds_subsequent_data_transfer_status_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
}/* qmi_wdsi_subsequent_data_transfer_status*/

/*===========================================================================
  FUNCTION QMI_WDSI_SPLMN_RATE_CONTROL_INFO()

  DESCRIPTION
    Gives the modem capabilities whether Mux is been given by modem
    
  PARAMETERS
     req_c_struct  :            Request Message with all TLV
     req_handle  :              Request Handle
     clnt_info   :              Client Info Handle
     msg_id             :       Message ID
     req_c_struct_len   :       length of request message
     service_handle     :       wds service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error    qmi_wdsi_splmn_rate_control_info
(
  qmi_wdsi_client_info_type        *clnt_info,
  qmi_req_handle                    req_handle,
  int                               msg_id,
  void                             *req_c_struct,
  int                               req_c_struct_len,
  void                             *service_handle
)
{
  qmi_wds_client_state_map                      *wdsi_sp = NULL;  
  ds_pdn_cp_data_splmn_rate_ctrl_info_s          splmn_rate_ctrl_ioctl ={0};
  wds_get_splmn_rate_control_info_req_msg_v01   *req_msg; 
  wds_get_splmn_rate_control_info_resp_msg_v01   resp_msg;  
  qmi_error_type_v01                             errval = QMI_ERR_NONE_V01;  
  sint15                                         ps_errno = 0;
  /*----------------------------------------------------*/
  wdsi_sp = qmi_get_wds_state(clnt_info);
  req_msg  = (wds_get_splmn_rate_control_info_req_msg_v01*)req_c_struct;
  errval = QMI_ERR_NONE_V01;
  memset(&resp_msg,0,sizeof(wds_get_splmn_rate_control_info_resp_msg_v01));
 
  // Check if call is UP
  if((wdsi_sp == NULL)||(wdsi_sp->pkt_svc_state != WDS_CONNECTION_STATUS_CONNECTED_V01))
  {
    errval = QMI_ERR_OUT_OF_CALL_V01;
    goto send_result;
  }
 // Call DS ioctl
  if (ds_pdn_ioctl(DS_FWK_CLIENT_RMNET,
                   wdsi_sp->binding.rmnet_inst,
                   DS_PDN_IOCTL_GET_SPLMN_RATE_CONTROL_INFO,
                   &splmn_rate_ctrl_ioctl,
                   &ps_errno) != 0)
  {
     DS_MSG1(MSG_LEGACY_ERROR,"Couldn't set qmi_wdsi_splmn_rate_control_info  , err: %d", ps_errno);
     errval = QMI_ERR_INTERNAL_V01;
     goto send_result; 
  }
  if(ps_errno == DSS_SUCCESS)
  {
    resp_msg.splmn_enabled_valid = TRUE;
    resp_msg.splmn_enabled    = splmn_rate_ctrl_ioctl.valid;
    if(splmn_rate_ctrl_ioctl.valid == TRUE)
    {
      resp_msg.splmn_rate_valid = TRUE;
      resp_msg.splmn_rate = splmn_rate_ctrl_ioctl.rate_ctrl_info.threshold;
     
      resp_msg.splmn_pkt_counter_valid = TRUE;
      resp_msg.splmn_pkt_counter  = (uint16)splmn_rate_ctrl_ioctl.pkt_counter;
     
      resp_msg.splmn_rate_exceeded_valid = TRUE;
      resp_msg.splmn_rate_exceeded =  splmn_rate_ctrl_ioctl.rate_ctrl_info.threshold_exceeded;
    }
    errval = QMI_ERR_NONE_V01;
  }
  
 send_result:
  resp_msg.resp.result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01
                                   : QMI_RESULT_FAILURE_V01);
  resp_msg.resp.error  = errval;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(wds_get_splmn_rate_control_info_resp_msg_v01));  
  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION QMI_WDSI_APN_RATE_CONTROL_INFO()

  DESCRIPTION
    Gives the modem capabilities whether Mux is been given by modem
    
  PARAMETERS
     req_c_struct  :            Request Message with all TLV
     req_handle  :              Request Handle
     clnt_info   :              Client Info Handle
     msg_id             :       Message ID
     req_c_struct_len   :       length of request message
     service_handle     :       wds service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error    qmi_wdsi_apn_rate_control_info
(
  qmi_wdsi_client_info_type        *clnt_info,
  qmi_req_handle                    req_handle,
  int                               msg_id,
  void                             *req_c_struct,
  int                               req_c_struct_len,
  void                             *service_handle
)
{
  ds_apn_rate_ctrl_context_s                   apn_rate_ctrl_ioctl ={{0}};
  qmi_wds_client_state_map                    *wdsi_sp = NULL;
  wds_get_apn_rate_control_info_req_msg_v01   *req_msg; 
  wds_get_apn_rate_control_info_resp_msg_v01   resp_msg;  
  qmi_error_type_v01                           errval = QMI_ERR_NONE_V01;  
  sint15                                       ps_errno = 0;
  /*----------------------------------------------------*/
  wdsi_sp = qmi_get_wds_state(clnt_info);
  req_msg  = (wds_get_apn_rate_control_info_req_msg_v01*)req_c_struct;
  errval = QMI_ERR_NONE_V01;
  memset(&resp_msg,0,sizeof(wds_get_apn_rate_control_info_resp_msg_v01));
 
  // Check if call is UP
  if((wdsi_sp == NULL)||(wdsi_sp->pkt_svc_state != WDS_CONNECTION_STATUS_CONNECTED_V01))
  {
    errval = QMI_ERR_OUT_OF_CALL_V01;
    goto send_result;
  }
 // Call DS ioctl
  if (ds_pdn_ioctl(DS_FWK_CLIENT_RMNET,
                   wdsi_sp->binding.rmnet_inst,
                   DS_PDN_IOCTL_GET_APN_RATE_CONTROL_INFO,
                   &apn_rate_ctrl_ioctl,
                   &ps_errno) != 0)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"Couldn't get qmi_wdsi_apn_rate_control_info , err: %d", ps_errno);
    errval = QMI_ERR_INTERNAL_V01;
    goto send_result;
  }
  if(ps_errno == DSS_SUCCESS)
  {
    resp_msg.applies_to_mo_ex_valid = TRUE;
    resp_msg.applies_to_mo_ex    = apn_rate_ctrl_ioctl.config.applies_to_mo_ex;
        
    resp_msg.uplink_time_valid = TRUE;
    resp_msg.uplink_time = apn_rate_ctrl_ioctl.config.uplink_time_unit;
     
    resp_msg.max_uplink_rate_valid = TRUE;
    resp_msg.max_uplink_rate  = apn_rate_ctrl_ioctl.config.max_uplink_rate;
    
    resp_msg.pkt_counter_valid = TRUE;
    resp_msg.pkt_counter =  (uint16)apn_rate_ctrl_ioctl.pkt_counter;

    resp_msg.apn_rate_exceeded_valid = TRUE;
    resp_msg.apn_rate_exceeded = apn_rate_ctrl_ioctl.rate_ctrl_info.threshold_exceeded;

    resp_msg.num_bearers_valid = TRUE;
    resp_msg.num_bearers =  apn_rate_ctrl_ioctl.num_bearers;
    if(apn_rate_ctrl_ioctl.apn[0] != '\0')
    {
      memscpy(resp_msg.apn_name,DS_PROFILE_APN_STRING_LEN,
               apn_rate_ctrl_ioctl.apn,DS_PROFILE_APN_STRING_LEN);
      resp_msg.apn_name_valid = TRUE;
    }
    errval = QMI_ERR_NONE_V01;
  }
  
 send_result:
  resp_msg.resp.result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01
                                   : QMI_RESULT_FAILURE_V01);
  resp_msg.resp.error  = errval;
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(wds_get_apn_rate_control_info_resp_msg_v01));  
  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION QMI_WDSI_GET_DATA_BEARER_TECHNOLOGY_EX()

  DESCRIPTION
    Retrive the current data bearer technology in new format

  PARAMETERS
     req_c_struct  :            Request Message with all TLV
     req_handle  :              Request Handle
     clnt_info   :              Client Info Handle
     msg_id             :       Message ID
     req_c_struct_len   :       length of request message
     service_handle     :       wds service handle
     
  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_wdsi_get_data_bearer_technology_ex
(
  qmi_wdsi_client_info_type        *clnt_info,
  qmi_req_handle                    req_handle,
  int                               msg_id,
  void                             *req_c_struct,
  int                               req_c_struct_len,
  void                             *service_handle
)
{
  qmi_wds_client_state_map   *wdsi_sp = NULL;
  wds_get_data_bearer_technology_ex_req_msg_v01  *req_msg; 
  wds_get_data_bearer_technology_ex_resp_msg_v01 resp_msg;
  qmi_error_type_v01 errval = QMI_ERR_NONE_V01;  
  ds_sys_system_status_type     *conf_system_status = NULL;
  int16                         ps_errno = 0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset(&resp_msg,0,sizeof(wds_get_data_bearer_technology_ex_resp_msg_v01));
  req_msg  = (wds_get_data_bearer_technology_ex_req_msg_v01*)req_c_struct;

  wdsi_sp = qmi_get_wds_state(clnt_info);
  
  if ((wdsi_sp ==NULL )||(wdsi_sp->pkt_svc_state != WDS_CONNECTION_STATUS_CONNECTED_V01 ))
  {
    errval = QMI_ERR_OUT_OF_CALL_V01;
    goto send_result;
  }
  DS_SYSTEM_HEAP_MEM_ALLOC(conf_system_status,
                             sizeof(ds_sys_system_status_type),
                             ds_sys_system_status_type*);    
  if (NULL == conf_system_status)
  {
     errval = QMI_ERR_NO_MEMORY_V01;
     goto send_result;
  }

  memset(conf_system_status, 0, sizeof(ds_sys_system_status_type));
  
  ds_sys_ioctl(DS_SYS_TECH_ALL, DS_SYS_IOCTL_SYS_STATUS,
                  conf_system_status,
                  &ps_errno);
  if(ps_errno != 0)
  {
     errval = QMI_ERR_INTERNAL_V01;
     goto send_result;
  }

  DS_MSG2(MSG_LEGACY_MED,"qmi_wdsi_get_data_bearer_technology_ex ,"
          "num of rats %d num of apns %d",
            conf_system_status->num_avail_sys,
            conf_system_status->num_avail_apns);

  /*get the cached current data bearer data*/
  resp_msg.bearer_tech_valid = TRUE;
  
  resp_msg.bearer_tech.technology = (wds_bearer_tech_network_enum_v01)
                   conf_system_status->avail_sys[0].technology;
  
  resp_msg.bearer_tech.rat_value = (wds_bearer_tech_rat_ex_enum_v01)
      conf_system_status->avail_sys[0].rat_value;
 
  resp_msg.bearer_tech.so_mask = conf_system_status->avail_sys[0].so_mask;

send_result:

  if(conf_system_status != NULL)
  {
     DS_SYSTEM_HEAP_MEM_FREE(conf_system_status);
  }
  resp_msg.resp.result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01
                                   : QMI_RESULT_FAILURE_V01);
  resp_msg.resp.error  = errval;
    
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(wds_get_data_bearer_technology_ex_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
} /* qmi_wdsi_get_data_bearer_technology_ex() */
