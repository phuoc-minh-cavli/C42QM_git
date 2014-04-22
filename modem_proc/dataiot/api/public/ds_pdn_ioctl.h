#ifndef DS_PDN_IOCTL_H
#define DS_PDN_IOCTL_H

/*===========================================================================

                          P S _ I F A C E _ I O C T L. H

DESCRIPTION

  The Protocol Services interface file. This contains variables and
  definitions used by the various modules to access interface definitions.

Copyright (c) 2018-19 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE


  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds_pdn_ioctl.h_v   1.1   12 Feb 2003 12:08:04   akuzhiyi  $
  $Header: //components/rel/dataiot.mpss/2.0/api/public/ds_pdn_ioctl.h#1 $ $DateTime: 2020/03/17 08:03:16 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/01/18   sk     created file
===========================================================================*/

#include "comdef.h"
#include "ds_defs.h"
#include "ps_in.h"
#include "ps_qos_defs.h"
#include "ds_ipfltr_defs.h"
#include "dss.h"
#include "ds_profile.h"
#include "list.h"


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                      REGIONAL DATA DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                          IOCTL Definitions

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/


/*---------------------------------------------------------------------------
  Definition of various interface control operations. Please do NOT change
  the values of the existing enumerated types when adding new ones (to ensure
  that object code released with previous versions of the code do not break).
  Each of the following control operations have associated tyepdef (with
  corresponding names) that are passed in or returned for get/set operations.

  Note:
    GET operations are assigned even numbers
    SET operations are assigned odd numbers.

    Also ensure that symetric operations are grouped.
---------------------------------------------------------------------------*/
typedef enum
{
  DS_PDN_IOCTL_MIN                      = 0,
  DS_PDN_IOCTL_GET_ALL_DNS_ADDR, /* Retrieve all DNS addresses of the PDN  */
  DS_PDN_IOCTL_GET_MTU, /* Get MTU of PDN        */
  DS_PDN_IOCTL_GET_IP_ADDR , /* Get IPADDR of PDN     */
  DS_PDN_IOCTL_REG_EVENT_CB,/* Register callback for events*/
  DS_PDN_IOCTL_DEREG_EVENT_CB ,/* Deregister event callback*/
  DS_PDN_IOCTL_GET_IMS_PARAMS, /*Get SIP Server Addresses & SIP Server Domain Names.*/
  DS_PDN_IOCTL_GET_OP_RES_PCO,/*operator reserved pco */
  DS_PDN_IOCTL_GENERATE_PRIV_IPV6_ADDR, /*Generate a new private IPv6 address             */
  DS_PDN_IOCTL_GET_PDN_STATS,  /* Get the PDN stats    */
  DS_PDN_IOCTL_RESET_PDN_STATS,  /* Reset the pdn stats  */
  DS_PDN_IOCTL_QOS_NET_INITIATED_REQUEST,  /** Request to monitor Network Initiated QoS (per specific qos filters)  */
  DS_PDN_IOCTL_QOS_NET_INITIATED_RELEASE,  /** Request to stop monitoring Network Initiated QoS*/
  DS_PDN_FLOW_IOCTL_QOS_GET_GRANTED_FLOW_SPEC,/*remove once BIP removes*/
  DS_PDN_IOCTL_QOS_GET_GRANTED_FLOW_SPEC,/*equiliant of DSS_PDN_IOCTL_QOS_GET_GRANTED_FLOW_SPEC2*/
  DS_PDN_IOCTL_GET_CHANGED_APN_PARAM_INFO , /* Get Changed APN parameter info*/
  DS_PDN_IOCTL_SET_SILENT_REDIAL, /* Do Silent redial by MH*/
  DS_PDN_IOCTL_GET_GATEWAY_ADDR,/**< Get gateway ip address.*/
  DS_PDN_IOCTL_GET_SUBNET_MASK,/**< Get IPv4 subnet mask.  */
  DS_PDN_IOCTL_GET_IPV6_REMAINING_ROUTER_LIFETIME, /* Get IPv6 Router lifetime of public address on PDN */
  DS_PDN_IOCTL_FILTER_MATCH_REQUEST, /* Filter match ioctl */
  DS_PDN_IOCTL_3GPP_GET_SESSION_PARAMS, /* Get session params */
  DS_PDN_IOCTL_3GPP_GET_LTE_REESTAB_CALLTYPE, /* Get calltype */
  DS_PDN_IOCTL_3GPP_SET_LTE_REESTAB_CALLTYPE, /* Set calltype */
  DS_PDN_IOCTL_3GPP_FORCE_PDN_DISCONNECT, /* Force PDN disconnect */
  DS_PDN_IOCTL_SET_SUBSEQUENT_DATA_TRANSFER_STATUS,
  DS_PDN_IOCTL_GET_SUBSEQUENT_DATA_TRANSFER_STATUS,
  DS_PDN_IOCTL_3GPP_MO_EXCP_IN_USE, /* Set MO Exception call state */

  
  DS_PDN_IOCTL_GET_SPLMN_RATE_CONTROL_INFO, /* Get     SPLMN     rate control info  */  
  DS_PDN_IOCTL_GET_APN_RATE_CONTROL_INFO, /* Get apn rate control info */
  DS_PDN_IOCTL_GET_PATH_MTU, /* Get Path MTU of PDN        */
  DS_PDN_IOCTL_MAX
} ds_pdn_ioctl_type;

/*--------------------------------------------------------------------------
IOCTL Data strucutres start
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  DS_PDN_IOCTL_GET_ALL_DNS_ADDR parameter type
  Retrieve both primary and secondary DNS addresses of an PDN
---------------------------------------------------------------------------*/
typedef struct
{
  /* This is an input/output param that specifies the # of addresses
     requested by the app/as well as the acutal # of addresses returned.   */
  uint8         num_dns_addrs;
  /* OUTPUT param */
  ip_addr_type *dns_addrs_ptr;  /* Ptr to client's allocated ip_addr array */
} ds_pdn_ioctl_get_all_dns_addrs_type;

/*---------------------------------------------------------------------------
  DS_PDN_IOCTL_GET_MTU parameter type
  - PDN  MTU
---------------------------------------------------------------------------*/
typedef uint32 ds_pdn_ioctl_mtu_type;

/*---------------------------------------------------------------------------
  DS_PDN_IOCTL_GET_IP_ADDR parameter type
---------------------------------------------------------------------------*/
typedef ip_addr_type ds_pdn_ioctl_ip_addr_type;

/*---------------------------------------------------------------------------
  Typedef for event callback function
  (DS_PDN_IOCTL_REG_EVENT_CB/ DS_PDN_IOCTL_DEREG_EVENT_CB)
---------------------------------------------------------------------------*/
typedef void (*ds_pdn_ioctl_event_cb)
(
  ds_pdn_event_enum_type          event,
  ds_pdn_event_info_u_type        event_info,
  void                           *user_data,
  sint15                          net_handle
);

/*---------------------------------------------------------------------------
  DS_PDN_IOCTL_GET_IMS_PARAMS paramameter type
---------------------------------------------------------------------------*/
typedef struct
{
  uint16  mcc;
  uint16  mnc;
  boolean mnc_includes_pcs_digit;
  uint8 app_specific_info_len;
  uint8  app_specific_info[MAX_OP_RES_PCO_APP_SPECIFIC_LEN];
  uint16 container_id;
}ds_pdn_op_reserved_pco_type;

 typedef struct
{
  uint32                          sip_addr_cnt;
  ip_addr_type                   *sip_addr_array;
  uint8                           domain_name_count;
  ds_domain_name_type            *domain_name_array;
} ds_pdn_ioctl_ims_params_info_type;

typedef ds_pdn_op_reserved_pco_type ds_pdn_ioctl_op_res_pco_type;

/*---------------------------------------------------------------------------
  DS_PDN_IOCTL_GENERATE_PRIV_IPV6_ADDR parameter type
---------------------------------------------------------------------------*/
typedef struct
{
  sint15                            dss_nethandle;
  boolean                           is_unique; /*unique or sharable*/
  ds_pdn_ioctl_event_cb             event_cb;         /* event callback */
  void                             *user_data_ptr;/* User data to cback */
  /* OUTPUT PARAMETER */
  ip_addr_type                         *ip_addr;
} ds_pdn_ioctl_priv_ipv6_addr_type;

/*---------------------------------------------------------------------------
  Data type used for DS_PDN_IOCTL_QOS_NET_INITIATED_REQUEST
---------------------------------------------------------------------------*/
typedef struct
{
  net_initiated_qos_spec_type               net_initiated_qos_spec;
  /**< QOS specification */
  ds_net_initiated_qos_req_handle_type  req_handle;
  /**< [out] Handle to this request. It can be used in subsequent operations
             on this request */
} ds_pdn_ioctl_qos_net_initiated_request_type;

/*---------------------------------------------------------------------------
  Argument to be passed with DSS_PDN_IOCTL_QOS_NET_INITIATED_RELEASE
---------------------------------------------------------------------------*/
typedef struct
{
  ds_net_initiated_qos_req_handle_type  req_handle;
  /**< Handle, which application had obtained obtained from
       DS_PDN_IOCTL_QOS_NET_INITIATED_REQUEST */
} ds_pdn_ioctl_qos_net_initiated_release_type;

/*---------------------------------------------------------------------------
  Data type used for retrieving the QOS flow spec
  (DS_PDN_IOCTL_QOS_GET_GRANTED_FLOW_SPEC)
---------------------------------------------------------------------------*/
typedef enum
{
  QOS_STATE_INVALID = 0x00,
  QOS_UNAVAILABLE   = 0x01,
  QOS_ACTIVATING    = 0x02,
  QOS_AVAILABLE     = 0x04,
  QOS_SUSPENDING    = 0x08,
  QOS_DEACTIVATED   = 0x10,
  QOS_RELEASING     = 0x20,
  QOS_CONFIGURING   = 0x40
} ds_pdn_ioctl_qos_status_type;

// Remove later once BIP removes.
typedef struct
{
  dss_qos_handle_type              handle;       /* Handle to QOS instance */

  /* OUTPUT Parameters */
  ip_flow_type                     rx_flow;      /* Rx Flow spec           */
  ip_flow_type                     tx_flow;      /* Tx Flow spec           */
  ds_pdn_ioctl_qos_status_type  qos_status;   /* status of QOS instance */
} ds_pdn_ioctl_qos_get_granted_flow_spec2_type;

typedef struct
{
  ds_qos_handle_type               qos_handle;       /* Handle to QOS instance */

  /* OUTPUT Parameters */
  ip_flow_type                     rx_flow;      /* Rx Flow spec           */
  ip_flow_type                     tx_flow;      /* Tx Flow spec           */
  ds_pdn_ioctl_qos_status_type     qos_status;   /* status of QOS instance */
  uint8                            bearer_id;    /* EPS bearer id          */
} ds_pdn_ioctl_qos_get_granted_flow_spec_type;

/*---------------------------------------------------------------------------
 DS_PDN_IOCTL_APN_PARAM_CHANGE_EV parameter type
  -Retrieve the data specifying the changed apn parameters.
---------------------------------------------------------------------------*/
typedef ds_apn_param_change_info_type ds_pdn_ioctl_changed_apn_param_info_type;

/*---------------------------------------------------------------------------
  Argument for DS_PDN_IOCTL_SET_SILENT_REDIAL
---------------------------------------------------------------------------*/
typedef struct
{
  boolean set_silent_redial;
} ds_pdn_ioctl_set_silent_redial_type;

/*---------------------------------------------------------------------------
  Argument to be passed with  DS_PDN_IOCTL_GET_GATEWAY_ADDR
---------------------------------------------------------------------------*/
typedef ip_addr_type ds_pdn_ioctl_gateway_addr_type;

/*---------------------------------------------------------------------------
  Argument to be passed with  DS_PDN_IOCTL_GET_SUBNET_MASK
---------------------------------------------------------------------------*/
typedef ip_addr_type ds_pdn_ioctl_subnet_mask_type;

/*---------------------------------------------------------------------------
  Argument to be passed with DS_PDN_IOCTL_3GPP_GET_SESSION_PARAMS
---------------------------------------------------------------------------*/
typedef struct{
  /* profile number for the profile used in the call */
  uint32                       profile_number;
  /* Profile name in ascii string format */
  byte                         profile_name[DS_PROFILE_MAX_AUTH_STRING_LEN+1];
  /* Authentication info */
  ds_profile_auth_enum_type        auth;
  /* APN string  */
  byte                         apn[DS_PROFILE_APN_STRING_LEN+1];
  /* PDP type (IP/PPP)  */
  ds_profile_pdp_type_e            pdp_type;
  /* Network alloc GPRS QoS params */
//  ds_gprs_qos_params_type gprs_qos;

  /* Network alloc LTE QoS params */
//  ds_pdn_lte_qos_params_type  lte_qos;
  boolean                      request_pcscf_address_flag;
  boolean                      is_psm_restored;
} ds_pdn_ioctl_3gpp_session_params_info_type;

/*---------------------------------------------------------------------------
  Argument to be passed with the ioctls
  DS_PDN_IOCTL_3GPP_GET_LTE_REESTAB_CALLTYPE
  DS_PDN_IOCTL_3GPP_SET_LTE_REESTAB_CALLTYPE
  The IOCTLs are valid only when pdn is up.
---------------------------------------------------------------------------*/

typedef enum
{
  DS_PDN_3GPP_LTE_REESTAB_CALLTYPE_NORMAL,
  DS_PDN_3GPP_LTE_REESTAB_CALLTYPE_VOLTE,
  DS_PDN_3GPP_LTE_REESTAB_CALLTYPE_MAX
} ds_pdn_ioctl_3gpp_lte_reestab_calltype_enum_type;

/*---------------------------------------------------------------------------
  Argument for DS_PDN_IOCTL_3GPP_GET_LTE_REESTAB_CALLTYPE
---------------------------------------------------------------------------*/
typedef struct
{
  ds_pdn_ioctl_3gpp_lte_reestab_calltype_enum_type calltype;
} ds_pdn_ioctl_3gpp_get_lte_reestab_calltype_type;

/*---------------------------------------------------------------------------
  Argument for DS_PDN_IOCTL_3GPP_SET_LTE_REESTAB_CALLTYPE
---------------------------------------------------------------------------*/
typedef struct
{
  ds_pdn_ioctl_3gpp_lte_reestab_calltype_enum_type calltype;
} ds_pdn_ioctl_3gpp_set_lte_reestab_calltype_type;

/*---------------------------------------------------------------------------
  Argument for DS_PDN_IOCTL_3GPP_SET_LTE_REESTAB_CALLTYPE
--------------------------------------------------------------------------*/
typedef enum
{
  DS_PDN_IOCTL_MO_EXCP_NOT_IN_USE = 0, 
  DS_PDN_IOCTL_MO_EXCP_IN_USE     = 1
}ds_pdn_ioctl_mo_excp_usage_enum_type;

/*---------------------------------------------------------------------------
  Typedef for struct used with DS_PDN_IOCTL_REG_EVENT_CB and
  DS_PDN_IOCTL_DEREG_EVENT_CB. Note that the application must specify a
  valid APP_ID when registering/deregistering for event callbacks.
---------------------------------------------------------------------------*/
typedef struct
{
  ds_pdn_ioctl_event_cb  event_cb;
  ds_pdn_event_enum_type    event;
  void                     *user_data_ptr;
} dss_iface_ioctl_ev_cb_type;

/*--------------------------------------------------------------------------
IOCTL Data strucutres end
---------------------------------------------------------------------------*/
/*===========================================================================

                           EXTERNALIZED FUNCTIONS

===========================================================================*/
/*===========================================================================
FUNCTION DS_PDN_IOCTL()

DESCRIPTION
  This function performs various operations on the given ps interface.
  Typically, these operations are to get or set a value.

DEPENDENCIES
  None.

PARAMETERS
  client_type .............client type can be either DSS,RMNET,RMSM

  ds_pdn_ptr              - ds_pdn on which the specified operations
                              is to be performed

  ioctl_name                - The operation name

  argval_ptr                - Pointer to operation specific structure

  ps_errno                  - Error code returned in case of failure (Error
                              values are those defined in dserrno.h)

                              DS_EINVAL - Returned when the specified IOCTL
                              does not belong to the common set of IOCTLs
                              and there is no IOCTL mode handler registered
                              for the specified interface.

                              DS_EOPNOTSUPP - Returned by the lower level
                              IOCTL mode handler when specified IOCTL is not
                              supported by the interface. For instance, this
                              would be returned by interfaces that do not
                              support a certain "pdn specific common IOCTL"
                              (i.e. these are common IOCTLs, but the
                              implementation is mode specific, for example,
                              GO_DORMANT).

                              DS_EFAULT - This error code is returned if the
                              specified arguments for the IOCTL are correct
                              but an error is encountered while executing
                              the IOCTL. For instance, if the 1X interface
                              cannot "GO_DORMANT" it would return this error.

                              DS_NOMEMORY - This error code is returned if we
                              run out of buffers during execution.

RETURN VALUE
  0 - on success
  -1 - on failure

SIDE EFFECTS
  None.

===========================================================================*/
int ds_pdn_ioctl
(
  uint8                  client_type,
  sint15                 client_handle,
  ds_pdn_ioctl_type      ioctl_name,
  void                  *argval_ptr,
  sint15                *ps_errno
);

/*---------------------------------------------------------------------------
  Typedef for struct used with DS_PDN_IOCTL_REG_EVENT_CB and
  DS_PDN_IOCTL_DEREG_EVENT_CB. 
---------------------------------------------------------------------------*/
typedef struct
{
  ds_pdn_ioctl_event_cb     event_cb;
  ds_pdn_event_enum_type    event;
  void                     *user_data_ptr;
} ds_pdn_ioctl_ev_cb_type;


#endif /* DS_PDN_IOCTL_H */

