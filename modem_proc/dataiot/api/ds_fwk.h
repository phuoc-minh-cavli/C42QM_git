#ifndef _DS_FWK_H
#define _DS_FWK_H
/*============================================================

                         D S _ F W K . H

DESCRIPTION

This file exposes DS Framework functionality through API'S and data structures to MH and 
clients such as RMNET,RMSM,DSS.
Framework is an Intermeditery module to bringup/teardown/events/ioctls related to calls.

Copyright (c) 2017-2019, 2021 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
============================================================*/
/*============================================================

                      EDIT HISTORY FOR FILE
  when        who        what, where, why
--------    ---    ----------------------------------------------------------------
04/25/19  svaka     Fixed issue V4 DL ping request
11/23/17    svaka     Created Framework Module as part of Chiltepin rewrite
============================================================*/

#include "ds_headers.h"
#include "ps_logging_defs.h"
#include "ds_ipfltr.h"
#include "ds_defsi.h"
#include "ds_pdn_ioctl.h"
#include "ps_meta_info.h"
#include "ps_ip6_addr.h"
#include "ds_pdn_defs.h"
#include "ds_sys.h"

/*MAX instances supported on 9x05 is 4*/
#define DS_FWK_MAX_INST 4
/*by default FW instance is invalid*/
#define DS_FWK_INST_INVALID -1
/*invalid client handle*/
#define DS_FWK_INVALID_CLNT_HANDLE -1
/*Min client handle*/
#define DS_FWK_MIN_CLNT_HANDLE 0
/*Max client handle*/
#define DS_FWK_MAX_CLNT_HANDLE 15


typedef int ds_fwk_clnt_handle_type;
typedef int ds_fwk_index_type;

/*===========================================================
MACRO IPV6_ADDR_MSG()
============================================================*/
/**
  Prints an IPv6 address.

  @param[in] ip_addr   IPv6 address in network byte order.

  @return
  None.

  @dependencies 
  None.
*/
#define IPV6_ADDR_MSG(ip_addr) MSG_8(MSG_SSID_DS, \
                        MSG_LEGACY_HIGH, \
                        "IPV6 Address %x:%x:%x:%x:%x:%x:%x:%x", \
                        (uint16)(ps_ntohs(ip_addr[0])), \
                        (uint16)(ps_ntohs(ip_addr[0] >> 16)), \
                        (uint16)(ps_ntohs(ip_addr[0] >> 32)) , \
                        (uint16)(ps_ntohs(ip_addr[0] >> 48)), \
                        (uint16)(ps_ntohs(ip_addr[1])), \
                        (uint16)(ps_ntohs(ip_addr[1] >> 16)), \
                        (uint16)(ps_ntohs(ip_addr[1] >> 32)) , \
                        (uint16)(ps_ntohs(ip_addr[1] >> 48)))

/*============================================================
MACRO IPV4_ADDR_MSG()

DESCRIPTION
  This macro prints an IPV4 address to F3.

PARAMETERS
  ip_addr: The IPV4 address in host byte order.

RETURN VALUE
  none
============================================================*/
#define IPV4_ADDR_MSG(ip_addr) MSG_4(MSG_SSID_DS, \
                        MSG_LEGACY_HIGH, \
                        "IPV4 Address is %d.%d.%d.%d", \
                        (unsigned char)(ip_addr), \
                        (unsigned char)(ip_addr >> 8), \
                        (unsigned char)(ip_addr >> 16) , \
                        (unsigned char)(ip_addr >> 24))


/*===========================================================
DS Client info: Client handle to be maintained with FW - this is passed from clients.
The client handle is passed as part of events incocation to clients.
though clients can use any handle , in general: 
incase of RMSM the client handle is rmsm instance.
incase of RMNET the client handle is rmnet instance
incase of DSS the client handle is dss_handle
============================================================*/
typedef struct 
{
  ds_fwk_clnt_handle_type   v4_client_handle;
  ds_fwk_clnt_handle_type   v6_client_handle;
  ds_fwk_clnt_handle_type   nipd_client_handle;
}ds_fwk_client_info_s_type;

/*===========================================================
DS Framework strcuture: This structure stores the PDN and its clients information.
============================================================*/
typedef enum
{
   DS_FWK_MH_3GPP,
   DS_FWK_MH_MAX
}ds_fwk_mh_e_type;

/*===========================================================
DS Framework strcuture: This structure stores the IP TYPE enum used for DPL and stats.
============================================================*/
typedef enum
{
   DS_FWK_IP_TYPE_V4,
   DS_FWK_IP_TYPE_V6,
   DS_FWK_IP_TYPE_MAX
}ds_fwk_iptype_e_type;

/*===========================================================
DS Framework PDN info strcuture: This structure stores the PDN info and its IP information.
============================================================*/
typedef struct
{
  ds_pdn_mgr_s      *pdn_ptr;
  ip_addr_enum_type ip_type;
}ds_fwk_pdn_info;

/*===========================================================
DS Framework client handle information structure
============================================================*/
typedef struct
{
  ds_fwk_client_type       clnt_type;
  ds_fwk_clnt_handle_type  clnt_handle;
}ds_fwk_clnt_handle_info_s_type;


/*===========================================================
DS Framework response info: MH fills this info as part of pdn operations to update 
the DS Framework strcutures.
============================================================*/
typedef struct
{
  ds_pdn_mgr_s                 *pdn_ptr;
  ip_addr_enum_type             ip_type;
  uint8                         netdown_reason_type;
  ds_net_down_reason_type       netdown_reason;
}ds_fwk_response_info_s_type;

/*---------------------------------------------------------------------------
    Provides statistics adjustment to the current iface stats
---------------------------------------------------------------------------*/
typedef struct
{
  uint32  pkts_rx;             /* rx pkt count adjustment */
  uint32  pkts_tx;             /* tx pkt count adjustment */
  uint64  bytes_rx;            /* rx byte count adjustment */
  uint64  bytes_tx;            /* tx byte count adjustment */
} ds_fwk_stat_adjustment_s_type;

/*---------------------------------------------------------------------------
  SIP address list
---------------------------------------------------------------------------*/
typedef struct
{
  list_link_type    link;
  ps_ip_addr_type   sip_addr;
} ds_fwk_sip_serv_addr_type;

typedef struct
{
  uint64                         prefix;
    /**< IPv6 prefix.           */
  uint64                         iid;
    /**< IPv6 IID.              */
  ps_timer_handle_type           pref_lifetimer_handle;    
    /**< Preferred lifetime.    */
  ps_timer_handle_type           valid_lifetimer_handle;   
    /**< Valid lifetime.        */
  ps_timer_handle_type           unused_addr_timer_handle; 
    /**< Timer for the duration of time a privacy address can remain unused. If 
         no application binds before the timer expires, the address is freed. */
  ds_ipv6_addr_state_enum_type   addr_state; 
    /**< IPv6 address state.    */
  ds_ipv6_addr_type_enum_type    addr_type;  
    /**< IPv6 address type.     */
  uint8                          ref_cnt;
    /**< Privacy extensions reference count. */
  uint8                          prefix_len;
    /**< Length of the prefix.  */
  uint8                          dad_retries;
    /**< Number of DAD attempts remaining. */
  ds_fwk_client_type             client_id;
    /**< points  to the "linked" client ID */
  uint32                         alloc_timestamp;
    /**< The timestamp when the address is allocated>**/
  boolean                        is_psm_restore;
    /**< Indicates whether the address was restored as part of PSM>**/
} ds_fwk_v6_addr_s_type;

/*---------------------------------------------------------------------------
  IP_V4_CONFIG_INFO
---------------------------------------------------------------------------*/
typedef struct
{
  /** Gateway address. */
  struct ps_in_addr         gateway;
  /** Primary DNS address. */
  struct ps_in_addr         primary_dns; 
  /** Secondary DNS address. */
  struct ps_in_addr         secondary_dns;  
  /** Net mask. */
  struct ps_in_addr         net_mask;       
  /** SIP Serv addr**/
  list_type                 sip_serv_addr;
  /* IPv4 address       */
  uint32                    ipv4_addr; 
  /* MTU size */
  uint16                    mtu;              
    /* for WINS support */
  struct ps_in_addr         primary_nbns; 
  struct ps_in_addr         secondary_nbns;
}ds_fwk_v4_net_info_type;

/*---------------------------------------------------------------------------
  IP_V6_CONFIG_INFO
---------------------------------------------------------------------------*/
typedef struct
{
  /** Primary DNS address. */
  struct ps_in6_addr        primary_dns; 
  /** Secondary DNS address. */
  struct ps_in6_addr        secondary_dns;  
  uint64                    gateway_iid;    /* IID of gateway. */
  uint8                     curr_hop_limit; /* Current hop limit. */
  /** SIP Serv addr**/
  list_type                 sip_serv_addr;
  /*IPV6 address allocated to clients*/
  ds_fwk_v6_addr_s_type*	ipv6_addrs[DS_MAX_IPV6_ADDRS];
    /* MTU size */
  uint16                    mtu;
  void*                     dhcp6_hdl;
}ds_fwk_v6_net_info_type;

/*---------------------------------------------------------------------------
  NON_IP_CONFIG_INFO
---------------------------------------------------------------------------*/
typedef struct
{
  /* MTU size */
  uint16                     mtu;              
}ds_fwk_non_ip_net_info_type;

/*============================================================
TYPEDEF DS_FWK_STAT_ADJUSTMENT_F_PTR_TYPE()

DESCRIPTION
  Definition of the function called when iface wants to obtain stat
  adjustment from client

PARAMETERS
 ds_fwk_index on which adjustment needs to be calculated
  adjustment: Pointer to the adjustment result

RETURN VALUE
  none

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================*/
typedef void (*ds_fwk_stat_adjustment_f_ptr_type)
(
  ds_fwk_index_type               ds_fwk_index,
  ds_fwk_stat_adjustment_s_type * adjustment
);


/*============================================================
DS Framework strcuture: This structure stores the PDN and its clients information.
============================================================*/
/*DS FWK Global strcuture to capture all the clients information*/
struct ds_fwk_s
{
  /*Clients to register for bearer and pdn events*/ 
  q_type	                       event_handle_q;
  /*references to PDN strctures*/
  ds_pdn_mgr_s                   * v4_pdn_ptr;
  ds_pdn_mgr_s                   * v6_pdn_ptr;
  ds_pdn_mgr_s                   * non_ip_pdn_ptr;

  /*Clients bound to this pdn(RMNET/RMSM/DSS)*/
  ds_fwk_client_info_s_type        client_info[DS_FWK_CLIENT_MAX]; 

  /*to route the packet in data path*/
  //ds_fwk_tx_cmd_f_ptr_type	tx_cmd;
  
  uint32                           tx_flow_mask;

  /*The Tx IP filter information that is associated with this instance*/
  q_type                           ipfltr_info[FLTR_CLIENT_MAX];

  /*The Rx IP filter information that is associated with this instance*/
  list_type                        rx_qos_fltr_node_list;

  /*QOS request list*/
  list_type                        net_initiated_qos_req_list;
   
  /*index of ds_fwk*/
  ds_fwk_index_type                ds_fwk_index;
   
  /*proc_id being used for this interface*/
  uint32                           proc_id;
  
  /*ps_errno for previous action*/
  int16                            ps_errno;
  uint8                            type_of_action;
  
  /*dns cache*/
  q_type                           dns_cache_q;
  
  /*DL opt configuration with filters*/
  ds_dl_opt_filter_spec_type     * dl_opt_fltr_arr;

  /*indicate PDN is bridged or unbridged; if there is any socket call PDN is always unbridged*/
  boolean                          is_unbridged;

  /* IPv4 configuration information. */
  ds_fwk_v4_net_info_type          v4_net_info;       
  
  /* IPv6 configuration information. */
  ds_fwk_v6_net_info_type          v6_net_info;       
  
  /*non-ip net information*/
  ds_fwk_non_ip_net_info_type      non_ip_net_info;

  /*MSISDN Info*/
  ds_sys_msisdn_info_type          msisdn_info;
  
  /*to probe external addresses*/
  ps_timer_handle_type             ipv6_ext_addr_silent_timer; 
	
  /*Stats information and function pointers*/
  ds_fwk_stat_adjustment_s_type     stat_adjustment_cache;
  ds_fwk_stat_adjustment_f_ptr_type	ds_fwk_stat_get_adjustment_f_ptr;
  
  /*Client mask for PDN sharing */
  uint32                            client_mask;

  boolean                           attach_inst;

  /* UM Statistics for DSFWK instance*/
  ds_stats_type                     um_stats[DS_FWK_IP_TYPE_MAX]; //V4 ,V6

  /* RM Statistics for DSFWK instance*/
  ds_stats_type                     rm_stats[DS_FWK_IP_TYPE_MAX]; //V4 ,V6

  dpl_net_cb         dpl_rm[DS_FWK_IP_TYPE_MAX]; //V4 ,V6
  boolean                           is_clat_capable;
  dpl_net_cb         dpl_um[DS_FWK_IP_TYPE_MAX]; //V4 ,V6

  void*              dhcp6_server_handle;
  boolean            dhcp6_inited;
  void                             *clat_pdn_cntxt;
};

typedef struct
{
  ds_fwk_s_type       * fw_inst;
  ip_addr_enum_type     ip_type;
}ds_fwk_clnt_info_s_type;


typedef int (*ds_fwk_tx_cmd_f_ptr_type)
(
  ds_fwk_s_type        *ds_fwk_inst_ptr,
  dsm_item_type       **pkt_chain_ptr,
  ps_tx_meta_info_type *meta_info_ptr,
  void                 *tx_cmd_info
);

/*============================================================
  DESCRIPTION
    function pointer to register for pdn events

  PARAMETERS
    client_handle : handle associated with the pdn.
    event: pdn event.
    event-info: event payload
    user_data_ptr: user data passed as part of registration

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/

typedef void (*ds_fwk_event_cback_type)
(
  ds_fwk_clnt_handle_type    client_handle,
  ds_pdn_event_enum_type     event,
  ds_pdn_event_info_u_type   event_info,
  void                      *user_data_ptr
);

/*===========================================================
  Event buffer to store clients callback and user data that needs to be passed 
  as part of events notification
============================================================*/

typedef struct
{
  ds_fwk_event_cback_type    event_cback_f_ptr;
  void                      *user_data_ptr;
} ds_fwk_event_buf_type;

/*============================================================
  DESCRIPTION
    function pointer which points to PDN lookup API of MH

  PARAMETERS
    apn, ip_type

  RETURN VALUE
     valid framework index -- if matches with any of the existing pdn
     -1 -- if not matching

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
typedef ds_fwk_index_type (*ds_fwk_lookup_pdn_f_ptr_type)
(
  ds_policy_info_type  policy_info
); 

/*============================================================
  DESCRIPTION
    function pointer which points to PDN bringup API of MH.
    this functions returns immediately with the info such as pdn_ptr 
    and cause codes(incase of failure).
    pdn_ptr will be updated in ds_fwk strcutures for further operations.
    however clients needs to wait for PDN events for PDN UP,DOWN status.

  PARAMETERS
    ds_fwk_idx,apn, ip_type,info

  RETURN VALUE
      0  -- if bringup is successful.(pdn_ptr will be updated in info out param)
     -1 -- if bringup is failed.(failure reason will be updated in info out param)
      102 -Operation would block.(pdn_ptr will be updated in info out param)
      
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
typedef int32 (*ds_fwk_bringup_pdn_f_ptr_type)
(
  ds_fwk_index_type               ds_fwk_idx,
  ds_policy_info_type             policy_info,
  ds_fwk_response_info_s_type*    response_info
);

/*============================================================
  DESCRIPTION
    function pointer which points to PDN teardown API of MH.
    teardown function returns immediately from MH and clients needs to wait 
    for pdn_down events for cleanup.
    as part of PDN down events ds framework posts events to its clients and 
    deletes the ds_framework instance incase of v4,v6,non-ip pdn's are invalid.
    
  PARAMETERS
    apn, ip_type

  RETURN VALUE
      0  -- if teardown is successful.
     -1 -- if teardown is failed.
      102 -Operation would block.


  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
typedef int32 (*ds_fwk_teardown_pdn_f_ptr_type)
(
  ds_pdn_mgr_s                  *pdn_ptr,
  ip_addr_enum_type             ip_type,
  ds_fwk_response_info_s_type  *response_info
);

/*===========================================================
  DESCRIPTION
    function pointer which points to MH IOCTL function

  PARAMETERS
    pdn_ptr,ioctl_name,arg_val_ptr,ps_errno

  RETURN VALUE
      0  --  if successful.
     -1 -- if failed.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/

typedef int (*ds_fwk_ioctl_f_ptr_type)
(
  ds_pdn_mgr_s             *pdn_ptr,
  ip_addr_enum_type                ip_type,
  ds_pdn_ioctl_type        ioctl_name,
  void                    *argval_ptr,
  sint15                  *ps_errno
);

/*============================================================
TYPEDEF DS_FWK_STAT_ADJUSTMENT_F_PTR_TYPE()

DESCRIPTION
  Definition of the function called when iface wants to obtain stat
  adjustment from client

PARAMETERS
 ds_fwk_index on which adjustment needs to be calculated
  adjustment: Pointer to the adjustment result

RETURN VALUE
  none

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================*/
typedef void (*ds_fwk_stat_adjustment_f_ptr_type)
(
  ds_fwk_index_type               ds_fwk_index,
  ds_fwk_stat_adjustment_s_type * adjustment
);

/*============================================================
ds_fwk_pdn_operations_s_type: MH registers functions for below operations.
============================================================*/
typedef struct
{
  ds_fwk_bringup_pdn_f_ptr_type     bringup_f_ptr;
  ds_fwk_teardown_pdn_f_ptr_type    teardown_f_ptr;
  ds_fwk_lookup_pdn_f_ptr_type      look_up_f_ptr;
  ds_fwk_ioctl_f_ptr_type           ioctl_f_ptr;
}ds_fwk_pdn_operations_s_type;


/*============================================================
DS Framework MH strcuture: This structure stores the MH information registered with FW.
============================================================*/
struct ds_fw_mh_info_s
{
  ds_fwk_pdn_operations_s_type pdn_operations;
};

/**
  @brief PDN specific clat configuration data for both 4to6 and 
         6to4 translation.
*/
typedef struct
{
  ds_fwk_s_type                    * fwk_inst_ptr;
  uint32                             v4_client_addr;
  uint8                              v6_dev_prefix_len;
  uint8                              v6_plat_prefix_len;
  uint8                              v6_own_addr_len;
  uint8                              v6_te_addr_len;
  struct ps_in6_addr                 v6_dev_prefix;
  struct ps_in6_addr                 v6_plat_prefix;
  struct ps_in6_addr                 v6_own_addr;
  struct ps_in6_addr                 v6_te_addr;
  void                             * client_pdn_handle;
  uint8                              tos_ignore_bit_flag ;
  uint8                              tos_override_val;
} ds_fwk_clat_reg_pdn_cntxt_info_type;

/**
  @brief PDN specific clat configuration dereg.
*/

typedef struct
{
  ds_fwk_s_type                    * fwk_inst_ptr;
  void                             * client_pdn_handle;
} ds_fwk_clat_dereg_pdn_cntxt_info_type;

typedef struct ds_fw_mh_info_s ds_fw_mh_info_type;

#define DS_FWK_NUM_FILTERS( ds_fwk_inst_ptr, client )                      \
	ds_fwk_get_num_filters( ds_fwk_inst_ptr, client )

#define DS_FWK_GET_MTU( ds_fwk_inst_ptr,ip_ver )\
        ds_fwk_get_mtu(ds_fwk_inst_ptr,ip_ver)

#define DS_FWK_INST_IS_VALID( ds_fwk_inst_ptr,ip_ver)\
        ds_fwk_inst_is_valid(ds_fwk_inst_ptr,ip_ver)

	
/*============================================================
FUNCTION DS_FWK_INST_IS_VALID_EX()

DESCRIPTION
  returns whether the pdn instance of fraemwork instance pointer passed is valid or not
  
PARAMETERS
  ds_fwk_inst_ptr - framework instance
  ip_ver - ip version
  
RETURN VALUE
  TRUE if instance is valid
  FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================*/
boolean ds_fwk_inst_is_valid_ex
(
  ds_fwk_s_type*          ds_fwk_inst_ptr,
  ip_addr_enum_type       ip_ver	
);

/*============================================================
FUNCTION DS_FWK_INST_IS_VALID()

DESCRIPTION
  returns whether the pdn instance of fraemwork instance pointer passed is valid or not
  
PARAMETERS
  ds_fwk_inst_ptr - framework instance
  ip_ver - ip version
  
RETURN VALUE
  TRUE if instance is valid
  FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================*/
boolean ds_fwk_inst_is_valid
(
  ds_fwk_s_type*          ds_fwk_inst_ptr,
  ip_addr_enum_type       ip_ver
);

/*============================================================
FUNCTION DS_FWK_GET_NUM_FILTERS()

DESCRIPTION
   returns the number of filters on the framework instance
  
PARAMETERS
  ds_fwk_inst_ptr - framework instance
  client_type - type of ip filter
  
RETURN VALUE
  no of filters

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================*/
uint8 ds_fwk_get_num_filters
(
  ds_fwk_s_type                 * ds_fwk_inst_ptr,
  ds_ipfltr_client_id_enum_type   client_type
);

/*============================================================
FUNCTION DS_FWK_GET_INST_PTR_FROM_INDEX()

DESCRIPTION
  gets the framework instance ptr of passed index.
  
PARAMETERS
  framework index
  
RETURN VALUE
  pointer to framework instance if instance is allocated
  NULL otherwise

DEPENDENCIES
  none

SIDE EFFECTS
  none
============================================================*/
ds_fwk_s_type* ds_fwk_get_inst_ptr_from_index
(
  ds_fwk_index_type index
);

/*============================================================
FUNCTION DS_FWK_GET_IP_ADDRESS()

DESCRIPTION
  returns the IP address for the specified ip version and instance
  
PARAMETERS
  framework instance
  ip version
  ip_addr_ptr(out)
  
RETURN VALUE
  void
  
DEPENDENCIES
  none

SIDE EFFECTS
  none
============================================================*/
void ds_fwk_get_ip_addr
(
  ds_fwk_s_type*          ds_fwk_inst_ptr,
  ip_addr_enum_type       ip_ver,
  ps_ip_addr_type 		 *ip_addr_ptr
);

/*============================================================
  FUNCTION DS_FWK_LOOK_UP_PDN_BY_POLICY()

  DESCRIPTION
    Invokes corresponding Mode handler look up api to find the mathcing PDN for given policy.

  PARAMETERS
    client_type
    apn 
    ip type

  RETURN VALUE
    Client handle - if matches to any existing policy
    -1 - if not matches with any existing PDN policy.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
ds_fwk_clnt_handle_type  ds_fwk_look_up_pdn_by_policy
(
  ds_fwk_client_type       client_id,
  ds_policy_info_type      policy_info
);


/*============================================================
  FUNCTION DS_FWK_ALLOC_ATTACH_INST()

  DESCRIPTION
   allocated attach framework instance when invoked from MH

  PARAMETERS
    pdn_ptr 
    ip type

  RETURN VALUE
    fwk instance index

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
ds_fwk_index_type ds_fwk_alloc_attach_inst
(
  ds_pdn_mgr_s* 	 pdn_ptr,
  ip_addr_enum_type  ip_type,
  boolean            is_default
);

/*============================================================
  FUNCTION DS_FWK_DEALLOC_ATTACH_INST()

  DESCRIPTION
   deallocates attach framework instance when invoked from MH

  PARAMETERS
    ds_fwk_index

  RETURN VALUE
    none

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
void ds_fwk_dealloc_attach_inst
(
  ds_fwk_index_type ds_fwk_index
);


/*============================================================
  FUNCTION DS_FWK_BRINGUP_PDN_BY_POLICY()

  DESCRIPTION
    Invokes corresponding Mode handler bringup up api and bringups the PDN.

  PARAMETERS
    client_type
    apn 
    ip type
    client_handle - 
    we use this as client handle to identiy and send as part of further indications.
    clients can be RMNET/RMSM/DSS.incase of PDN sharing scenario to indicate only to the 
    requested client dsfwk passes this user_data to pdn mansger, PDN manager provides this 
    info as part of event so that DSFWK can invoke client call back with this user data.

  RETURN VALUE
      0  -- if bringup is successful.
     -1 -- if bringup is failed.
      102 -Operation would block.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
int  ds_fwk_bringup_pdn_by_policy
(
  ds_fwk_client_type             client_id,
  ds_policy_info_type            policy_info,
  ds_fwk_clnt_handle_type        client_handle,
  ds_fwk_response_info_s_type   *response_info
);

/*============================================================
  FUNCTION DS_FWK_TEARDOWN_PDN()

  DESCRIPTION
    Invokes corresponding Mode handler bringup up api and bringups the PDN.

  PARAMETERS
    client_type

  RETURN VALUE
      0  -- if bringup is successful.
     -1 -- if bringup is failed.
      102 -Operation would block.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
int  ds_fwk_tear_down_pdn
(
  ds_fwk_client_type              client_id,
  ds_fwk_clnt_handle_type         client_handle,
  ds_fwk_response_info_s_type    *response_info
);

/*============================================================
  FUNCTION DS_FWK_GET_CURRENT_MH()

  DESCRIPTION
   gets the current MH
   
  PARAMETERS
   none

  RETURN VALUE
     MH index

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
uint8 ds_fwk_get_current_mh();

/*============================================================
  FUNCTION DS_FWK_REGISTER_MH()

  DESCRIPTION
   regsiters MH operations with ds_fwk
   
  PARAMETERS
    client_type

  RETURN VALUE
      0  -- if reg is success
     -1 -- if failed

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
int ds_fwk_register_mh
(
  ds_fwk_mh_e_type               mh_id,
  ds_fwk_pdn_operations_s_type  *operations
);

/*============================================================
  FUNCTION DS_FWK_REGISTER_EVENTS()

  DESCRIPTION
   regsiters client for all the PDN events. this callback is invoked for
   all the PDN events if the client is active.
      
  PARAMETERS
    client_id: client type
    callback_type: user callback 
    user-data_ptr: data that needs to be passed to client as part of PDN event invocation.

  RETURN VALUE
      0  -- if reg is success
     -1 -- if failed

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/

int ds_fwk_reg_events
(
  ds_fwk_client_type       client_id,
  ds_fwk_event_cback_type  callback_type,
  void                    *user_data_ptr
);

/*============================================================
  FUNCTION ds_fwk_notify_events()

  DESCRIPTION
    invokes event callbacks of all the client present on the framework instance
      
  PARAMETERS
    ds_fwk_inst: instance of ds_fwk.
    event: event name
    event_info: event_payload

  RETURN VALUE
       0  -- if events invocation is successful
     -1 -- if failed

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/

int ds_fwk_notify_events
(
  ds_fwk_index_type            ds_fwk_inst,
  ip_addr_enum_type            family,
  ds_pdn_event_enum_type       event,
  ds_pdn_event_info_u_type     event_info
);

/*============================================================
  FUNCTION ds_fwk_notify_events_ex()

  DESCRIPTION
    invokes event callbacks of all the client present on the framework instance
      
  PARAMETERS
    ds_fwk_inst: instance of ds_fwk.
    family: ip type
    client id: client due to which callback is invoked.
    event: event name
    event_info: event_payload

  RETURN VALUE
       0  -- if events invocation is successful
     -1 -- if failed

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/

int ds_fwk_notify_events_ex
(
  ds_fwk_index_type            ds_fwk_inst,
  ip_addr_enum_type            family,
  int8                         client_id,
  ds_pdn_event_enum_type       event,
  ds_pdn_event_info_u_type     event_info
);

/*============================================================
 Utils
============================================================*/
/*============================================================
FUNCTION DS_FWK_GET_CLNT_INFO_FROM_HANDLE

  DESCRIPTION
   returns client info such as pdn info and ip version from the passed handle.
      
  PARAMETERS
   flow_ptr

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
void ds_fwk_get_clnt_info_from_handle
(
  ds_fwk_client_type       clnt_type,
  ds_fwk_clnt_handle_type  clnt_handle,
  ds_fwk_clnt_info_s_type *clnt_info_ptr
);


/*============================================================
  FUNCTION DS_FWK_IS_FLOW_ENABLED()

  DESCRIPTION
   checks whether the flow is enabled or not on the given flow ptr.
      
  PARAMETERS
   flow_ptr

  RETURN VALUE
       0  -- if flow is disabled
       1 -- when flow is enabled

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
boolean ds_fwk_is_flow_enabled
(
  ds_flow_type*  flow_ptr
);

/*============================================================
  FUNCTION DS_FWK_IS_FLOW_ENABLED_EX()

  DESCRIPTION
   checks whether the flow is enabled or not on the given flow ptr.
      
  PARAMETERS
   flow_ptr
   is_mox_data_pkt

  RETURN VALUE
       0  -- if flow is disabled
       1 -- when flow is enabled

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
boolean ds_fwk_is_flow_enabled_ex
(
  ds_flow_type*  flow_ptr,
  boolean        is_mox_data_pkt
);

/*===========================================================================
  FUNCTION DS_FWK_GET_DATA_FLOW_EX

  DESCRIPTION
   checks if packet matches with any of the bearers and verifies whether flow is enabled or not.
      
  PARAMETERS
   ds_fwk_inst_ptr -- instance on which the packet needs to be sent
   ip_type -- ip type of client
   pkt_info -- packet which needs to be sent
   flow_ptr -- out param - flow ptr on which the packet will be sent
   is_mox_data_pkt -- mo exception packet or not.

  RETURN VALUE
       0  -- if flow is disabled for the packet
       1 -- when flow is enabled for the packet

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

boolean ds_fwk_get_data_flow_ex
(
  ds_fwk_s_type             *ds_fwk_inst_ptr,
  ip_addr_enum_type          ip_type,
  ip_pkt_info_type          *pkt_info,
  ds_flow_type             **flow_ptr,
  boolean                    is_mox_data_pkt
);


/*===========================================================================
  FUNCTION DS_FWK_GET_DATA_FLOW

  DESCRIPTION
   checks if packet matches with any of the bearers and verifies whether flow is enabled or not.
      
  PARAMETERS
   ds_fwk_inst_ptr -- instance on which the packet needs to be sent
   ip_type -- ip type of client
   pkt_info -- packet which needs to be sent
   flow_ptr -- out param - flow ptr on which the packet will be sent

  RETURN VALUE
       0  -- if flow is disabled for the packet
       1 -- when flow is enabled for the packet

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

boolean ds_fwk_get_data_flow
(
  ds_fwk_s_type             *ds_fwk_inst_ptr,
  ip_addr_enum_type          ip_type,
  ip_pkt_info_type          *pkt_info,
  ds_flow_type             **flow_ptr
);

/*===========================================================================
  FUNCTION DS_FWK_GET_DATA_FLOW_EXT

  DESCRIPTION
   Checks if packet matches with any of the bearers and verifies whether flow 
   is enabled or not.
      
  PARAMETERS
   ds_fwk_inst_ptr -- instance on which the packet needs to be sent
   ip_type -- ip type of client
   pkt_info -- packet which needs to be sent
   flow_ptr -- out param - flow ptr on which the packet will be sent

  RETURN VALUE
       0  -- if flow is disabled for the packet
       1 -- when flow is enabled for the packet

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean ds_fwk_get_data_flow_ext
(
  ds_fwk_s_type             *ds_fwk_inst_ptr,
  ip_addr_enum_type          ip_type,
  ip_pkt_info_type          *pkt_info,
  ds_flow_type             **flow_ptr
);

/*===========================================================================
  FUNCTION DS_FWK_GET_DEFAULT_BEARER_FLOW_EX

  DESCRIPTION
   Checks if packet matches with any of the bearers and verifies whether flow
   is enabled or not.
      
  PARAMETERS
   ds_fwk_inst_ptr -- instance on which the packet needs to be sent
   ip_type -- ip type of client
   flow_ptr -- flow pointer 
   is_mox_data_pkt -- is mo exception packet


  RETURN VALUE
       0  -- if flow is disabled for the packet
       1 -- when flow is enabled for the packet

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean ds_fwk_get_default_bearer_flow_ex
(
  ds_fwk_s_type            *ds_fwk_inst_ptr,
  ip_addr_enum_type         ip_type,
  ds_flow_type            **flow_ptr,
  boolean                   is_mox_data_pkt
);


/*===========================================================================
  FUNCTION DS_FWK_GET_DEFAULT_BEARER_FLOW_EXT

  DESCRIPTION
   Checks if packet matches with any of the bearers and verifies whether flow 
   is enabled or not.

   This api does not check the DS_FLOW_CAPABILITY_DATA_DISALLOWED 
   mask on the flows
      
  PARAMETERS
   ds_fwk_inst_ptr -- instance on which the packet needs to be sent
   ip_type -- ip type of client
   flow_ptr -- flow pointer 
   is_mox_data_pkt -- is mo exception packet


  RETURN VALUE
       0  -- if flow is disabled for the packet
       1 -- when flow is enabled for the packet

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean ds_fwk_get_default_bearer_flow_ext
(
  ds_fwk_s_type            *ds_fwk_inst_ptr,
  ip_addr_enum_type         ip_type,
  ds_flow_type            **flow_ptr,
  boolean                   is_mox_data_pkt
);

/*===========================================================================
  FUNCTION DS_FWK_GET_DEFAULT_BEARER_FLOW

  DESCRIPTION
   checks if packet matches with any of the bearers and verifies whether flow is enabled or not.
      
  PARAMETERS
   ds_fwk_inst_ptr -- instance on which the packet needs to be sent
   ip_type -- ip type of client


  RETURN VALUE
       0  -- if flow is disabled for the packet
       1 -- when flow is enabled for the packet

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean ds_fwk_get_default_bearer_flow
(
  ds_fwk_s_type             *ds_fwk_inst_ptr,
  ip_addr_enum_type         ip_type,
  ds_flow_type              **flow_ptr
);


/*============================================================
 All Data Path related functions
============================================================*/

/*============================================================
Fucntions related to downlink
============================================================*/

/*============================================================
FUNCTION DS_FWK_BEARER_INPUT()

DESCRIPTION
   Transfers the DL packet from bearer to respective client.

PARAMETERS
  bearer_ptr:    on which the packet recieved.
  pkt_ref_ptr:   ref to dsm item ptr received

RETURN VALUE
  0: data was transfered
 -1: data was not transferred, but memory was freed

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================*/
int ds_fwk_bearer_input
(
  ds_bearer_mgr_s       *bearer_ptr,
  dsm_item_type        **pkt_ref_ptr
);

/*============================================================
FUNCTION DS_FWK_INPUT_LEGACY()

DESCRIPTION
   This function is invoked when packet needs to be routed to legacy path
   takes decision whether to route the packet to RMNET/RMSM/SOCKET client
   by running the socket filters.
   it invokes either ip_input of respective ip_type if the packet is intended for
   modem stack otherwise it calls respective clients tranmit command.

PARAMETERS
  ds_fwk_inst_ptr               : framework instance
  pkt_ref_ptr                   : ref to dsm item ptr received
  ds_fwk_clnt_handle_info_s_type: client handle info
  meta_info_ptr                 : ptr to meta info (TX/RX meta information)

RETURN VALUE
  0: data was transfered
 -1: data was not transferred, but memory was freed

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================*/
int ds_fwk_input_legacy
(
  ds_fwk_s_type*                  ds_fwk_inst_ptr,
  dsm_item_type                  **pkt_ref_ptr,
  ds_fwk_clnt_handle_info_s_type  *clnt_handle_info, 
  ps_meta_info_type_ex            *meta_info_ex_ptr
);

/*============================================================
FUNCTION DS_FWK_LEGACY_INPUT_LEGACY()

DESCRIPTION
  this function is invoked when packet needs to be routed to RMNET/RMSM legacy path.
  
PARAMETERS
  dsm_item_ptr:     ref to dsm item ptr received
  meta_info_ptr:     ptr to meta info (TX/RX meta information)
  client_handle:      the client which it needs to be sent.

RETURN VALUE
   0: data was transfered
 -1: data was not transferred, but memory was freed

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================*/
int ds_fwk_legacy_tx_cmd
(
  dsm_item_type           * dsm_item_ptr,
  ps_meta_info_type       * meta_info_ptr,
  ds_fwk_clnt_handle_type   client_handle
);

/*============================================================
FUNCTION DS_FWK_DEFAULT_TX_CMD()

DESCRIPTION
  This function is the default TX function.  
  It frees the data passed in.

PARAMETERS
  fwk_inst_ptr:      instance on which it is being called
  pkt_ref_ptr:       ref to dsm item ptr for Tx - freed
  meta_info_ptr:     pointer to meta info type - also freed.

RETURN VALUE
  -1: indicating error

DEPENDENCIES
  Should not be called for logical ifaces

SIDE EFFECTS
  None
============================================================*/
int ds_fwk_default_tx_cmd
(
  ds_fwk_s_type        *ds_fwk_inst_ptr,
  dsm_item_type       **pkt_ref_ptr,
  ps_tx_meta_info_type *meta_info_ptr
);

/*============================================================
Fucntions related to uplink
============================================================*/

/*============================================================
FUNCTION DS_FWK_GET_BEARER_ID()

DESCRIPTION
  this function returns the bearer ID by running QOS Output filters.
  
PARAMETERS
  dsm_item_ptr:     ref to dsm item ptr received
  client_handle:      the client which it needs to be sent.

RETURN VALUE
   bearer ID of matching bearer

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================*/
uint8 ds_fwk_get_bearer_id
(
  ds_fwk_clnt_handle_type   client_handle,
  dsm_item_type           * dsm_item_ptr
);

/*============================================================
FUNCTION DS_FWK_GET_DEFAULT_BEARER()

DESCRIPTION
   gets the defualt bearer from the given framework instance
  
PARAMETERS
   ds_fwk_inst_ptr
   ip_type

RETURN VALUE
  bearer_ptr

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================*/
ds_bearer_mgr_s* ds_fwk_get_default_bearer
(
  ds_fwk_s_type*     ds_fwk_inst_ptr,
  ip_addr_enum_type  ip_type
);

/*============================================================
FUNCTION DS_FWK_IS_NON_IP_BEARER()

DESCRIPTION
   checks whether the bearer is ip or non_ip type
  
PARAMETERS
  bearer_ptr

RETURN VALUE
  TRUE - if bearer is non_ip
  FALSE - otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================*/
boolean ds_fwk_is_non_ip_bearer
(
  ds_bearer_mgr_s       *bearer_ptr
);

/*============================================================
FUNCTION DS_FWK_INPUT()

DESCRIPTION
   client gives packet to transmit over the network.

PARAMETERS
  pkt_ref_ptr:        ref to dsm item ptr received
  meta_info_ptr:     ptr to meta info (TX/RX meta information)

RETURN VALUE
  0: data was transfered
 -1: data was not transferred, but memory was freed

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================*/
int ds_fwk_input
(	
  ds_fwk_s_type*               ds_fwk_inst_ptr,
  dsm_item_type             ** pkt_ref_ptr,
  ps_meta_info_type_ex	    *  meta_info_ex_ptr
);

/*============================================================
FUNCTION DS_FWK_TX_CMD()

DESCRIPTION
  Sends the packet to respective bearer by invoking bearer TX command.
  bearer id would be extracted from meta_info_ptr.
  
PARAMETERS
  dsm_item_ptr:     ref to dsm item ptr received
  meta_info_ptr:     ptr to meta info (TX meta information)

RETURN VALUE
   0: data was transfered
 -1: data was not transferred, but memory was freed

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================*/
int ds_fwk_tx_cmd
(
  ds_fwk_s_type		  * ds_fwk_inst_ptr,
  dsm_item_type      ** dsm_item_ptr,
  ps_meta_info_type   * meta_info_ptr
);

/*============================================================
FUNCTION DS_FWK_INIT()

DESCRIPTION
  This function initializes the framework related varibales as part of bootup.
  
PARAMETERS
  none
  
RETURN VALUE
  void

DEPENDENCIES
  none

SIDE EFFECTS
  none
============================================================*/
void ds_fwk_init();

/*============================================================
  FUNCTION DS_FWK_QTF_GET_HW_TO_PS_WM()

  DESCRIPTION
    gets the hw_to_ps wm for QTF
      
  PARAMETERS
   clnt_type
   client_handle

  RETURN VALUE
   watermark

  DEPENDENCIES
  None

  SIDE EFFECTS
  None
============================================================*/
dsm_watermark_type* ds_fwk_qtf_get_hw_to_ps_wm
(
  ds_fwk_client_type       clnt_type,
  ds_fwk_clnt_handle_type  client_handle
);

/*============================================================
  FUNCTION DS_FWK_GET_MH()

  DESCRIPTION
   gets the mode handler ptr from passed ID
      
  PARAMETERS
   mh_id

  RETURN VALUE
    ds_fw_mh_info_type

  DEPENDENCIES
  None

  SIDE EFFECTS
  None
============================================================*/
ds_fw_mh_info_type * ds_fwk_get_mh(uint8 mh_id);

/*============================================================
  FUNCTION DS_FWK_GET_MTU()

  DESCRIPTION
   returns MTU from the framework instance based on the ip version passed.
      
  PARAMETERS
   mh_id

  RETURN VALUE
    ds_fw_mh_info_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
uint16  ds_fwk_get_mtu
(
  ds_fwk_s_type      * fwk_inst,
  ip_addr_enum_type    ip_type
);

#ifdef FEATURE_DATA_PS_464XLAT
/*============================================================
  FUNCTION DS_FWK_CLAT_REG_PDN_CONTEXT()

  DESCRIPTION
   Reg the CLAT PDN context
      
  PARAMETERS
   reg_info_ptr  : CLAT PDN context

  RETURN VALUE
    int

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
int ds_fwk_clat_reg_pdn_context 
(
  ds_fwk_clat_reg_pdn_cntxt_info_type *reg_info_ptr
);

/*============================================================
  FUNCTION DS_FWK_CLAT_DEREG_PDN_CONTEXT()

  DESCRIPTION
   Deregs the CLAT PDN context
  
  PARAMETERS
   dereg_info_ptr : Pointer to clat pdn contxt

  RETURN VALUE
    int

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
int ds_fwk_clat_dereg_pdn_context
(
  ds_fwk_clat_dereg_pdn_cntxt_info_type    *dereg_info_ptr
);

#endif

/*============================================================
  FUNCTION DS_FWK_LOOK_UP_PDN_BY_POLICY()

  DESCRIPTION
    Invokes corresponding Mode handler look up api to find the mathcing PDN for given policy.

  PARAMETERS
    client_id        Fwk client type
    policy_info      Policy on which the loop up is performed

  RETURN VALUE
    Client handle - if matches to any existing policy
    -1 - if not matches with any existing PDN policy.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
ds_fwk_clnt_handle_type  ds_fwk_get_fwk_inst_by_policy
(
  ds_fwk_client_type       client_id,
  ds_policy_info_type      policy_info
);

#endif /* _DS_FWK_H */

