#ifndef _DS_FWKI_H
#define _DS_FWKI_H
/*============================================================

                         DS_FWKI . H

DESCRIPTION

This file exposes DS Framework functionality through API'S and data structures to MH 
and clients such as RMNET,RMSM,DSS.

Copyright (c) 2017 - 2021 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=============================================================*/
/*============================================================

                      EDIT HISTORY FOR FILE
when        who    what, where, why
--------    ---    ----------------------------------------
11/23/17   svaka     Created module
============================================================*/
#include "ds_headers.h"
#include "ps_logging_defs.h"
#include "ds_defsi.h"
#include "ds_pdn_ioctl.h"
#include "ps_meta_info.h"
#include "ds_fwk.h"
#include "ds_pdn_defs.h"
#include "ps_svc.h"
#include "ds_pdn_manager.h"
#include "ps_logging.h"
#include "ps_stat_logging.h"
#include "ds_rmnet_smi.h"
#include "ULogFront.h"

typedef struct 
{
  ds_bearer_mgr_s  * bearer_ptr;
  ps_sig_enum_type   signal; 
  boolean            in_use;
}ds_fwk_bearer_info_type;

/**
  @brief PDN specific clat configuration data for both 4to6 and 
         6to4 translation.
*/
typedef struct
{
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
} ds_fwk_clat_pdn_cntxt_info_type;

#define DS_FWK_INVALID_HW_ADDR       0x00000000

/**
  @brief Data type that holds global stats which consists total packets count
         including uplink and downlink on all the RAT's.
         also holds a variable which will have the current number
         clients registered for data activity resumed event.
*/
typedef struct
{
  ds_sys_ioctl_global_stats_type global_stats;
  uint8                          reg_clnt_cnt;
}ds_fwk_global_stats_info_type;

/**
  @brief Data type that holds global Fwk info for all existing RAT's.
*/
typedef struct
{
  ds_fwk_global_stats_info_type   fwk_global_stats;
} ds_fwk_global_info_type;

#define DS_FWK_GLOBAL_STATS_INC ds_fwk_global_stats_inc()

/*===========================================================================
  DESCRIPTION
    function pointer to register for transmit function

  PARAMETERS
    client_handle : handle associated with the data call.
    dsm_item               : payload to transmit
    user_data_ptr: user data passed as part of registration

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
typedef void (*ds_fwk_tx_cback_type)
(
  ds_fwk_clnt_handle_type    client_handle,
  dsm_item_type              *dsm_item,
  void                      *user_data_ptr

);

/*===========================================================================
  Event buffer to store clients callback and user data that needs to be passed as part of 
  transmiting data
===========================================================================*/

typedef struct
{
  ds_fwk_tx_cback_type       tx_cback_f_ptr;
  void                      *user_data_ptr;
} ds_fwk_tx_buf_type;

/*===========================================================================
  Event buffer to store clients callback and user data that needs to be passed as part of 
  transmiting non ip data
===========================================================================*/

typedef struct
{
  ds_fwk_tx_cback_type       tx_cback_f_ptr;
  void                      *user_data_ptr;
} ds_fwk_tx_nipd_buf_type;

typedef enum
{
  DS_FWK_SUCCESS = 0,
  DS_FWK_ERROR_1 = 1,
  DS_FWK_ERROR_2 = 2,
  DS_FWK_ERROR_3 = 3,
  DS_FWK_ERROR_4 = 4,
  DS_FWK_ERROR_5 = 5,
  DS_FWK_ERROR_6 = 6,
  DS_FWK_ERROR_7 = 7,
  DS_FWK_ERROR_8 = 8,
  DS_FWK_ERROR_9 = 9
}ds_fwk_error_enum_type;
/*===========================================================================
FUNCTION DS_FWK_GET_CLIENT_INFO_FROM_HANDLE()

DESCRIPTION
   fills the framework instance and ip version of passed client handle
  
PARAMETERS
  clnt_type - type of client
  clnt_handle - client handle
  clnt_info_ptr - information of client
  
RETURN VALUE
  void

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================*/
void ds_fwk_get_clnt_info_from_handle
(
  ds_fwk_client_type             clnt_type,
  ds_fwk_clnt_handle_type        clnt_handle,
  ds_fwk_clnt_info_s_type       *clnt_info_ptr
);

/*============================================================
FUNCTION DS_FWKI_GET_FWK_INSTANCE_FROM_BEARER()

DESCRIPTION
  returns associated framework instance ptr for bearer.
  
PARAMETERS
  bearer_ptr
  
RETURN VALUE
  framework instance ptr

DEPENDENCIES
  None

SIDE EFFECTS
  None
=============================================================*/
ds_fwk_s_type* ds_fwki_get_fwk_instance_from_bearer
(
  ds_bearer_mgr_s * bearer_ptr
);

/*============================================================
FUNCTION DS_FWK_GET_ALL_DNS_ADDRS

DESCRIPTION
  This function returns all the DNS addresses stored on the interface.

PARAMETERS
  ds_pdn_mgr_s:       ptr to PDN control block on which to operate on.
  dns_addrs_ptr:      The ptr to the DNS addrs
  num_dns_addrs_ptr:  The number of DNS addresses space is allocated for.
                      This is an output param as well and will be set to the
                      # of valid DNS addresses actually returned.
  pdn_instance        Instance of the PDN V4 or V6

RETURN VALUE
  None.

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================*/
void ds_fwk_get_all_dns_addrs
(
  ds_fwk_s_type                 *fwk_inst,
  ip_addr_type                  *dns_addrs_ptr,
  uint8                         *num_dns_addrs_ptr,
  ds_pdn_mgr_instance_type_e	 pdn_instance
);


/*============================================================
FUNCTION DS_FWK_GET_OPERATOR_RESERVED_PCO

DESCRIPTION
  This function returns the Operator reserved PCO information in the iface.

PARAMETERS
  ds_pdn_mgr_s:               ptr to PDN control block on which to operate on.
  operator_reserved_pco_info_ptr: pointer to the operator reserved PCO info
 
RETURN VALUE
  0 if successful
 -1 if fails

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================*/
int ds_fwk_get_op_reserved_pco
(
  ds_pdn_mgr_s                       *pdn_ptr_info,
  ds_pdn_op_reserved_pco_type        *operator_reserved_pco_info_ptr
);

/*============================================================
FUNCTION DS_FWK_GET_PEER_ADDR()

DESCRIPTION
  This function will return the address of the interfaces peer (e.g. the
  GGSN, or the PDSN) if this is a point-to-point link.  If the interface
  isn't point-to-point the invalid address will be returned.

PARAMETERS
  ds_pdn_mgr_s:       ptr to PDN control block on which to operate on.
  ip_addr_ptr:        value return - the address will be stored here
  pdn_instance		  Instance of the PDN V4 or V6

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================*/
void ds_fwk_get_peer_addr
(
  ds_pdn_mgr_s                    *pdn_ptr_info,
  ps_ip_addr_type                 *ip_addr_ptr,
  ds_pdn_mgr_instance_type_e	   pdn_instance,  
  ds_fwk_s_type                  *ds_fwk_inst_ptr 
);

/*============================================================
FUNCTION DS_FWK_GET_STATS_PTR()

DESCRIPTION
  This function will return the address of the stats structure 

PARAMETERS
  fw_inst:             ptr to fwk control block on which to operate on.
  ip_type:             the address will be stored here
  client_id	       client_id RM or UM

RETURN VALUE
  dpl_net_cb

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================*/
#define DS_FWK_GET_STATS_PTR ds_fwk_get_stats_ptr
INLINE ds_stats_type *ds_fwk_get_stats_ptr
(
  ds_fwk_s_type        *fw_inst,
  ip_version_enum_type  ip_type,
  ds_stats_client_type    client_id
)
{
  if((fw_inst != NULL)&&(client_id == DS_STATS_CLIENT_UM))
  {
   if(ip_type == IP_V4)
    return &(fw_inst->um_stats[DS_FWK_IP_TYPE_V4]);
   else if(ip_type == IP_V6)
    return &(fw_inst->um_stats[DS_FWK_IP_TYPE_V6]);
  }
  else if((fw_inst != NULL)&&(client_id == DS_STATS_CLIENT_RM))
  {
    if(ip_type == IP_V4)
      return &(fw_inst->rm_stats[DS_FWK_IP_TYPE_V4]);
   else if(ip_type == IP_V6)
     return &(fw_inst->rm_stats[DS_FWK_IP_TYPE_V6]);
  }
  return NULL;
}

/*===========================================================================
MACRO DS_FWK_INC_INSTANCE_STATS()

DESCRIPTION
  This macro increments the iface statistic (field) by the necessary quantity
  (inc).
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define DS_FWK_INC_INSTANCE_STATS(PTR,FIELD,INC)    \
  if(PTR != NULL)\
  {                                                                          \
     if ((sizeof(PTR->FIELD)) == 4)                                          \
     {                                                                       \
        (PTR->FIELD) += (INC);                                               \
     }                                                                       \
     else                                                                    \
     {                                                                       \
        DS_ENTER_CRIT_SECTION(&global_ps_crit_section);                      \
        (PTR->FIELD) += (INC);                                               \
        DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);                      \
     }                                                                       \
  }

/*=======================================================
  FUNCTION DS_FWK_REGISTER_TX_CB()

  DESCRIPTION
   regsiters client for all the PDN events. this callback
   is invoked for all the PDN events if the client is active.
      
  PARAMETERS
    client_id: client type
    callback_type: user callback 
    user-data_ptr: data that needs to be passed to client as part
                            of PDN event invocation.

  RETURN VALUE
      0  -- if reg is success
     -1 -- if failed

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================*/
int ds_fwk_reg_tx_cb
(
  ds_fwk_client_type       client_id,
  ds_fwk_tx_cback_type     callback_type,
  void                    *user_data_ptr
);


/*===========================================================================
  FUNCTION DS_FWK_REGISTER_NIPD_TX_CB()

  DESCRIPTION
   regsiters client for all the PDN events. this callback is invoked for all the PDN events if the client is active.
      
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
===========================================================================*/
int ds_fwk_reg_nipd_tx_cb
(
  ds_fwk_client_type       client_id,
  ds_fwk_tx_cback_type     callback_type,
  void                    *user_data_ptr
);

/*=========================================================
  FUNCTION DS_FWK_INVOKE_TX_CB()

  DESCRIPTION
   invokes tx function pointer fo the client id passed.
      
  PARAMETERS
    client_id: client type
    callback_type: user callback 
    user-data_ptr: data that needs to be passed to client 
                           as part of PDN event invocation.

  RETURN VALUE
      0  -- if reg is success
     -1 -- if failed

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
int ds_fwk_invoke_tx_cb
(
  ds_fwk_client_type          client_id,  
  ds_fwk_clnt_handle_type     client_handle,  
  dsm_item_type              *dsm_item
);

/*===========================================================================
  FUNCTION DS_FWK_INVOKE_TX_NIPD_CB()

  DESCRIPTION
    Invokes the nipd callback for client       
    
  PARAMETERS
    client_id: client type
    client_handle: user callback 
    user-dsm_item: data that needs to be passed to client as part of PDN event invocation.

  RETURN VALUE
      0  -- if invocation is success
     -1 -- if failed

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

int ds_fwk_invoke_tx_nipd_cb
(
  ds_fwk_client_type          client_id,  
  ds_fwk_clnt_handle_type     client_handle,  
  dsm_item_type              *dsm_item
);

/*============================================================
  FUNCTION DS_FWK_DEALLO_IID()

  DESCRIPTION
    Deallocs the unique iid assigne to thefwk instance

  PARAMETERS
    dpl_cb
    family 

  RETURN VALUE
   None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
void ds_fwk_dealloc_iid
(
  dpl_net_cb            *dpl_cb,
  ip_addr_enum_type      family
);

/*============================================================
  FUNCTION DS_FWK_FREE_IIDS()

  DESCRIPTION
    Deallocs the unique iid assigne to thefwk instance

  PARAMETERS
  fw_inst

  RETURN VALUE
   None

  DEPENDENCIES
    None

  SIDE EFFECTS

  None
============================================================*/

void ds_fwk_free_iids
(
  ds_fwk_s_type  *fw_inst
);

/*============================================================
  FUNCTION DS_FWK_ASSIGN_IID()

  DESCRIPTION
    Assigns the unique iid to a fwk instance

  PARAMETERS
    dpl_cb
    family 
    client_id

  RETURN VALUE
   None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
void ds_fwk_assign_iid
(
  dpl_net_cb       *dpl_cb,
  ip_addr_enum_type family,
  ds_dpl_client_type client_id
);

/*===========================================================
  FUNCTION DS_FWK_ASSIGN_DPL_DESC()

  DESCRIPTION
   deallocates attach framework instance when invoked from MH

  PARAMETERS
    ds_fwk_index
    client_id
    policy_info

  RETURN VALUE
    none

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
void ds_fwk_assign_dpl_desc
(
  uint8                        fwk_inst,
  ds_dpl_client_type           client_id,
  ds_policy_info_type          policy_info
);

/*============================================================

FUNCTION DS_FWK_GET_SIP_SERV_ADDR_EX

DESCRIPTION
  This function returns the list of sip server addresses stored in the iface.

PARAMETERS
  ds_pdn_mgr_s:        ptr to PDN control block on which to operate on.
  sip_serv_addr_ptr_array: pointer to the SIP server addresses array.
  count : as input value it contains the count of the SIP server
          addresses asked for and as a return value it contains the
          actual number of addresses returned.   
  pdn_instance		Instance of the PDN V4 or V6

RETURN VALUE
  0 Success
 -1 Failure

DEPENDENCIES
  None

SIDE EFFECTS
  None

  ============================================================*/
int ds_fwk_get_sip_serv_addr_ex
(
  ds_fwk_s_type                    *fwk_inst,
  ip_addr_type                     *sip_serv_addr_ptr_array,
  uint32                           *count,  
  ds_pdn_mgr_instance_type_e	    pdn_instance
);

/*===========================================================================
FUNCTION DS_FWKI_GET_LINKLOCAL_ADDR()

DESCRIPTION
  This function is used to get the link local address of the V6 interface.
  The function sets the addr type to invalid if the call fails.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  ip_addr_ptr:    value return - the address will be will be stored here

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  Copies the ipv6 addr from ps_iface_ptr to ip_addr_ptr.
===========================================================================*/
void ds_fwki_get_linklocal_addr
(
  ds_fwk_s_type         *fwk_inst,
  ps_ip_addr_type       *ip_addr_ptr,
  ip_addr_enum_type      ip_ver
);

/*===========================================================================
FUNCTION DS_FWK_GET_IPV6_ADDR()

DESCRIPTION
  This function will return IPv6 address for the interface to be used by
  transport layers. If the interface is a v6 interface and IPv6 is enabled
  then this function returns the source address of the interface with
  appropriate scope based on the passed destination address.
  Otherwise it just returns a v4 mapped v6 address by making a call to
  ds_fwk_get_addr().

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  dest_ip_ptr:    Destination IPv6 address.
  ip_addr_ptr:    value return - the address will be  will be stored here

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_fwk_get_ipv6_addr
(
  ds_fwk_s_type              *fw_ptr,  
  ip_addr_enum_type           ip_ver,
  const struct ps_in6_addr *dest_ip_ptr,
  ps_ip_addr_type           *ip_addr_ptr 
);

/*============================================================
FUNCTION DS_FWK_GET_PDN_STATS()

DESCRIPTION
  This function supplies the required fwk statistics. The first
  argument passed to the function is an enum that denotes the metric of
  interest.The second argument will be  a pointer to the address of the
  fwk control block. The third argument points to the memory location
  where the results will be copied.The result (statistic) will be copied to
  this memory address. The fourth argument passes the ip type of the statistic requested.

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  stat_type:   -     IN stats type to be queried from the fwk control block.
  fwk_inst           IN fwk control block from where the stats are fetched
  ip_type            IN- ip version for the stats   
  stats            - OUT- pointer for return value

SIDE EFFECTS
  None
============================================================*/
int ds_fwk_get_pdn_stats
(
  ps_stat_fwk_enum_type      stat_type,
  ds_fwk_s_type             *fwk_inst,
  ip_addr_enum_type          ip_type,
  ds_stats_type             *stats
);

/*============================================================
FUNCTION DS_FWK_RESET_PDN_STATS()

DESCRIPTION
  This function resets the statistics on the specified interface.

PARAMETERS
  fwk_inst           IN fwk control block from where the stats are fetched
  ip_type            IN- ip version for the stats   
  return_value	  -  OUT- return value (statistics)

RETURN VALUE
  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================*/
int ds_fwk_reset_pdn_stats
(
  ds_fwk_s_type              *fwk_inst,
  ip_addr_enum_type           ip_type,  
  sint15                     *return_value
);


/*============================================================
FUNCTION DS_FWK_GET_SIP_DOMAIN_NAMES()

DESCRIPTION
  This function returns the list of sip server addresses stored in the iface.

PARAMETERS
  ds_pdn_mgr_s:   - IN ptr to PDN control block on which to operate on.
  sip_serv_addr_ptr_array: pointer to the SIP server addresses array.
  count : as input value it contains the count of the SIP server
          addresses asked for and as a return value it contains the
          actual number of addresses returned.

RETURN VALUE
  0 Success
 -1 Failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================*/

int ds_fwk_get_sip_domain_names
(

  ds_fwk_s_type      *fwk_inst,
  ds_domain_name_type      *sip_serv_addr_ptr_array,
 uint8             *count
);
                  
/*============================================================
FUNCTION DS_FWK_GET_SUBNET_MASK()

DESCRIPTION
  This function supplies the required iface layer statistics. The first
  argument passed to the function is an enum that denotes the metric of
  interest.The second argument will be  a pointer to the address of the
  iface control block. The third argument points to the memory location
  where the results will be copied.The result (statistic) will be copied to
  this memory address. The fourth argument passes the length of memory
  allocated for the return value and should be  greater than or equal to
  the memory size of the statistic requested.

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  ds_pdn_mgr_s:   - IN ptr to PDN control block on which to operate on.
  subnet_info     - IN - ptr to subnet mask
  pdn_instance		Instance of the PDN V4 or V6
  return_value    - IN - pointer for return value
                  - OUT- return value erro code

SIDE EFFECTS
  None
============================================================*/
int ds_fwk_get_subnet_mask
(
  ds_pdn_mgr_s                         *pdn_ptr_info,
  ds_pdn_ioctl_subnet_mask_type         *subnet_info,
  ds_pdn_mgr_instance_type_e             pdn_instance,  
  ds_fwk_s_type                         *fwk_inst,
  sint15                                *return_value
);


/*============================================================
FUNCTION DS_FWK_GENERATE_PRIV_IPV6_ADDR()

DESCRIPTION
  This function will generate a new private IPv6 as per RFC 3041. Based on
  the input to the function it will create a private shared (interface
  based) or private unique (owned and useable only by a particular socket).

PARAMETERS
  ds_pdn_mgr_s:   - IN ptr to PDN control block on which to operate on.
  ip_addr_ptr:    value return - the address will be stored here
  iid_param_ptr     IID params ptr
  ps_errno          OUT errno to be filled in this ptr

RETURN VALUE
  0 on successful generation of private ipv6 address
 -1 on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================*/
int ds_fwk_generate_priv_ipv6_addr
(
  ds_pdn_mgr_s             *pdn_ptr_info,
  ps_ip_addr_type          *ip_addr_ptr,
  ps_ipv6_iid_params_type *iid_param_ptr,
  int16                   *ps_errno

);

/*============================================================
FUNCTION DS_FWK_GET_PDN_PTR_FROM_FWK_INST()

DESCRIPTION
  This function will return the PDN ptr corresponding to the fwk instance and 
  IP type passed 

PARAMETERS
  ds_fwk_s_type:   - IN ptr to FWK control block on which to operate on.
  ip_ver:          - IN IP version for which the PDN ptr is needed

RETURN VALUE
  ds_pdn_mgr_s       OUT PDN ptr to be filled in case of success
  NULL               In case of failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================*/

ds_pdn_mgr_s * ds_fwk_get_pdn_ptr_from_fwk_inst
(
  ds_fwk_s_type        *fwk_inst,
  ip_addr_enum_type     ip_ver
);

/*============================================================
  FUNCTION DS_FWK_REG_BEARER_WITH_HW()

  DESCRIPTION
    registers bearer with HW for data path.
    
  PARAMETERS
   bearer ptr
   
  RETURN VALUE
   0 - if succses
   -1 - otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
int ds_fwk_reg_bearer_with_hw
(
  ds_bearer_mgr_s * bearer_ptr

);

/*============================================================
  FUNCTION DS_FWK_DEREG_BEARER_WITH_HW()

  DESCRIPTION
    deregisters bearer with HW for data path.
    
  PARAMETERS
   bearer ptr
   
  RETURN VALUE
   0 - if succses
   -1 - otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
int ds_fwk_dereg_bearer_with_hw
(
  ds_bearer_mgr_s * bearer_ptr
);

/*============================================================
  FUNCTION DS_FWK_ALLOC_INST()

  DESCRIPTION
   allocates free framework instance when invoked from MH

  PARAMETERS
    none

  RETURN VALUE
    fwk instance index

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
ds_fwk_index_type ds_fwk_alloc_inst
(
);

/*===========================================================================
FUNCTION DS_IPFLTRI_DELETE()

DESCRIPTION
  This function deletes all the existing IP filter rules for the specified
  filter handle for a given client from the specified iface.

DEPENDENCIES
  None

PARAMETERS
  iface_ptr : Iface to delete filters from
  client_id : Filtering client id
  fi_handle : filter handle associated with the filter set which was added
              on this iface by this client.
  ps_errno  : error returned to the caller if operation fails

RETURN VALUE
   0 : on success
  -1 : on failure

SIDE EFFECTS
  Some packets may not get filtered once these filters are deleted
===========================================================================*/
int ds_ipfltri_delete
(
  ds_fwk_s_type*                   fw_inst_ptr,
  ip_addr_enum_type                ip_type,
  ds_ipfltr_client_id_enum_type    client_id,
  ds_ipfltr_handle_type            fi_handle,
  int16                          * ps_errno
);

/*===========================================================================
FUNCTION DS_FWK_SET_IP_ADDR

DESCRIPTION
  This function returns the ip address of the call to ds_fwk 

PARAMETERS
  fwk_inst    : ptr to framework instance.
  ip_ver      : IP version to get the IP address
  ip_addr_ptr : pbuffer where the ip address will be returned 

RETURN VALUE
  None
  
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/

void ds_fwk_set_ip_addr
(
  ds_fwk_s_type*          ds_fwk_inst_ptr,
  ip_addr_enum_type       ip_ver,
  ps_ip_addr_type         *ip_addr_ptr
);

/*===========================================================================
FUNCTION DS_FWK_GET_PRIVATE_IP6_CONFIG()

DESCRIPTION
  This function copies secondary ip addresses into the v6 address list 
  from the given iface.

PARAMETERS 
  fwk_inst                  fwk_instance ptr
  ps_v6_addr_list           IPv6 address list

DEPENDENCIES
  None

RETURN VALUE
   0 on successful
  -1 on failure

SIDE EFFECTS
  None
===========================================================================*/

int ds_fwk_get_private_v6_config
(
  ds_fwk_s_type  *fwk_inst,
  q_type         *ps_v6_addr_list
);



/*===========================================================================
FUNCTION PS_FWK_SET_PRIVATE_IP6_CONFIG()

DESCRIPTION
  Set secondary ip addresses to the given fwk inst ptr.

PARAMETERS 
  fwk_inst                  fwk_instance ptr
  ps_v6_addr_list           IPv6 address list
  is_psm_restore            PSM restoration
  
DEPENDENCIES
  None

RETURN VALUE
   0 on successful
  -1 on failure

SIDE EFFECTS
  None
===========================================================================*/
int ds_fwk_set_private_v6_config
(
  ds_fwk_s_type    *fwk_inst,
  q_type           *ps_v6_addr_list,
  boolean           is_psm_restore
);

/*===========================================================================
FUNCTION PS_FWK_GET_IP_CONFIG()

DESCRIPTION
  Set secondary ip addresses to the given iface.

PARAMETERS 
  ds_fwk_inst_ptr      fwk_inst_ptr
  ip_ver               IP version
  ip_config_ptr        IP config ptr


DEPENDENCIES
  None

RETURN VALUE
   0 on successful
  -1 on failure

SIDE EFFECTS
  None
===========================================================================*/
int ds_fwk_get_ip_config
(
  ds_fwk_s_type*          ds_fwk_inst_ptr,
  ip_addr_enum_type       ip_ver,
  ds_ip_config_type       *ip_config_ptr
);

/*===========================================================================
FUNCTION PS_FWK_SET_IP_CONFIG()

DESCRIPTION
  Set secondary ip addresses to the given fwk inst ptr.

PARAMETERS 
  ds_fwk_inst_ptr      fwk_inst_ptr
  ip_ver               IP version
  ip_config_ptr        IP config ptr


DEPENDENCIES
  None

RETURN VALUE
   0 on successful
  -1 on failure

SIDE EFFECTS
  None
===========================================================================*/
int ds_fwk_set_ip_config
(
  ds_fwk_s_type          *ds_fwk_inst_ptr,
  ip_addr_enum_type      ip_ver,
  ds_ip_config_type      *ip_config_ptr
);

/*===========================================================================
LOCAL FUNCTION DS_FWK_CALC_GATEWAY_ADDR()

DESCRIPTION
  This function calculates the netmask and server/router IP address
  from the client IP address. This function tries to figure out the
  smallest netmask without handing out the broadcast address.

DEPENDENCIES
  Client IP address must be valid
  Currently only supports IPV4 addrs.  Of course, with IPV6, this
  whole thing is obsolete.

PARAMETERS
  dhcp_am_ps_iface_info * - Handle to the glue needed to get at the connection
                   details.

RETURN VALUE
  None.
  Calculations put into handle structure.

SIDE EFFECTS
  None
===========================================================================*/

void ds_fwk_calc_gateway_addr
(
  uint32    client_ip,
  uint32   *gateway_ip,
  uint32   *netmask
);

/*============================================================
FUNCTION DS_FWK_GET_DPL_PTR()

DESCRIPTION
  This function will return the address of the dpl structure 

PARAMETERS
  fw_inst:             ptr to fwk control block on which to operate on.
  ip_type:             the address will be stored here
  client_id	       client_id RM or UM

RETURN VALUE
  dpl_net_cb

DEPENDENCIES
  None

SIDE EFFECTS
  None

============================================================*/

dpl_net_cb *ds_fwk_get_dpl_ptr
(
  ds_fwk_s_type        *fw_inst,
  ip_version_enum_type  ip_type,
  ds_dpl_client_type    client_id
);

/*===========================================================
FUNCTION DS_FWK_GET_IP_ADDR

DESCRIPTION
  This function returns the primary IP address based on the ip 
  version

PARAMETERS
  ds_fwk_inst_ptr: ptr to fwk instance control block on which to
                   operate on.
  ip_ver:          version of the ip address to be looked 

RETURN VALUE
  ip_addr_ptr.  Ip address pointr in which the address will be
                 stored

DEPENDENCIES
  None

SIDE EFFECTS
  None
=============================================================*/
void ds_fwk_get_ip_addr
(
  ds_fwk_s_type*          ds_fwk_inst_ptr,
  ip_addr_enum_type       ip_ver,
  ps_ip_addr_type        *ip_addr_ptr
);

/*============================================================
  FUNCTION DS_FWK_CHECK_IF_TEARDOWN_ALLOWED()

  DESCRIPTION
   Checks if the network teardown  is allowed for the client 
   
  PARAMETERS
   client_type                fwk client 
   ip_type                    ip type of the call 
   fw_index                   index to fw inst
 
  RETURN VALUE
     Boolean

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
boolean ds_fwk_check_if_teardown_allowed
(
  ds_fwk_client_type             client_type,
  ip_addr_enum_type	         ip_type,
  ds_fwk_index_type              fw_index
);

/*===========================================================
FUNCTION DS_FWK_GET_ALL_STATS

DESCRIPTION
  This function returns the stats based on the ip type of the client
  version

PARAMETERS
  stat_type       Type of the stats needed 
  fwk_inst:       ptr to fwk instance control block on which to
                   operate on.
  return_value  : return value
  ret_len       the length of the retrn 
  ip_ver:          version of the ip address to be looked 

RETURN VALUE
    0 on successful generation of stats 

DEPENDENCIES
  None

SIDE EFFECTS
  None
=============================================================*/
errno_enum_type   ds_fwk_get_all_stats
(
  ps_stat_fwk_enum_type      stat_type,
  ds_fwk_s_type             *fwk_inst,
  void                      *return_value,
  uint16                     ret_len,
  ip_addr_enum_type          call_type
);

/*===========================================================================
FUNCTION DS_FWK_GET_MODEM_RESERVED_PCO

DESCRIPTION
  This function returns the modem reserved PCO information in the pdn_ptr.

PARAMETERS
  ds_pdn_mgr_s:               ptr to PDN control block on which to operate on.
  operator_reserved_pco_info_ptr: pointer to the operator reserved PCO info
 
RETURN VALUE
  0 if successful
 -1 if fails

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/

int ds_fwk_get_modem_reserved_pco
(
  ds_pdn_mgr_s                       *pdn_ptr_info,
  ds_pdn_mgr_modem_reserved_pco_type *modem_reserved_pco_info_ptr
);

/*===========================================================================
FUNCTION DS_FWK_GET_V4_DNS_ADDRS

DESCRIPTION
  This function returns the primary and secondary DNS addr's on the
  IPV4 PDN.

PARAMETERS
  pdn_ptr_info: ptr to interface control block on which to operate on.
  prim_dns_ptr:  storage for primary dns address
  sec_dns_ptr:  storage for secondary dns address

RETURN VALUE
  None.  However, if the addr family is not IPV4, then the input
  parameters are stored with zero.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_fwk_get_v4_dns_addrs
(
  ds_fwk_s_type         *fwk_inst,
  ip_addr_type          *prim_dns_ptr,
  ip_addr_type          *sec_dns_ptr
);

#if  defined (FEATURE_DATA_PS_IPV6)
/*===========================================================================
FUNCTION DS_FWK_GET_V6_DNS_ADDRS

DESCRIPTION
  This function returns the primary and secondary DNS addr's on the
  IPV6 PDN.

PARAMETERS
  pdn_ptr_info: ptr to interface control block on which to operate on.
  prim_dns_ptr:  storage for primary dns address
  sec_dns_ptr:  storage for secondary dns address

RETURN VALUE
  None.  However, if the addr family is not IPV6, then the input
  parameters are stored with zero.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_fwk_get_v6_dns_addrs
(
  ds_fwk_s_type         *fwk_inst,
  ip_addr_type          *prim_dns_ptr,
  ip_addr_type          *sec_dns_ptr
);
#endif  /*FEATURE_DATA_PS_IPV6*/
/*============================================================
  FUNCTION DS_FWK_GET_GLOBAL_STATS()

  DESCRIPTION 
     Gets the global stats from framework

  PARAMETERS
   global_stats    global stats pointer to get the stats
 
  RETURN VALUE
     None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
void ds_fwk_get_global_stats
(
  ds_sys_ioctl_global_stats_type  *global_stats
);
/*===========================================================
  FUNCTION DS_FWK_GET_CLIENT_INFO()

  DESCRIPTION
    gets active client info from instance
      
  PARAMETERS
   clnt_type
   client_handle

  RETURN VALUE
   boolean

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
boolean ds_fwk_get_client_info
(
  ds_fwk_s_type             * fw_inst,
  ip_addr_enum_type           ip_type,
  ds_fwk_client_type        * clnt_type,
  ds_fwk_clnt_handle_type   * client_handle
);

/*============================================================
  FUNCTION DS_FWK_DATA_ACTIVITY_CLIENT_STATUS_IND()

  DESCRIPTION 
     Sets client status information in FWK Global stats structure.
     based on client status FWK decides whether
     to invoke data activity resumed system event or not.   

  PARAMETERS
   client_status    cient status if enalbe or not 
 
  RETURN VALUE
     None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
void ds_fwk_data_activity_client_status_ind
(
  boolean  client_status
);

/*===========================================================
FUNCTION DS_FWK_DEREG_DS_PDCP_WM_CB


DESCRIPTION
  This function sets the signal to DS task to clear the watermarks

PARAMETERS
  epsid     The epsid on which this watermark should be deregistered. 

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
=============================================================*/
void  ds_fwk_dereg_ds_pdcp_wm_cb
(
  uint8 epsid
);


/*============================================================
  FUNCTION DS_FWK_GLOBAL_STATS_INC()

  DESCRIPTION 
    increments global stats count in fwk global info

  PARAMETERS
   None   
 
  RETURN VALUE
     None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/

void ds_fwk_global_stats_inc
(
);

/*===========================================================================
FUNCTION DS_FWK_IS_PDN_SHARED()

DESCRIPTION
   Checks if the framework instance is shared between A7 and DUN clients
  
PARAMETERS
  fwk_inst    -  Framework instance
  pkt_version -  Pkt version
  
RETURN VALUE
  Boolean

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================*/
boolean ds_fwk_is_pdn_shared
(
  ds_fwk_s_type *fwk_inst,
  uint8          pkt_version
);

/*===========================================================================
FUNCTION DS_FWK_DELETE_PDN_SHARING_FILTERS()

DESCRIPTION
  Deletes the pdn sharing filters from the framework instance 
  
PARAMETERS
  fwk_index    -  Framework index to fetch the framework instance
  ip_ver       -  Ip address verion of the filter to delete

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================*/
void ds_fwk_delete_pdn_sharing_filters
(
  uint8               fwk_index,
  ip_addr_enum_type   ip_ver
);

/*===========================================================================
MACRO DS_FWK_HW_ADDR_PTR()

DESCRIPTION
  This macro returns the pointer to the Rmnet instance's private hardware address

PARAMETERS
  fw_inst: pointer to the interface in question.
  ip_ver : ip version to get the hardware address

RETURN VALUE
  HW address, 0 if interface pointer is NULL

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define DS_FWK_GET_HW_PTR ds_fwki_get_hw_ptr

INLINE uint8 *ds_fwki_get_hw_ptr
(
  ds_fwk_s_type         *fw_inst,  
  ip_addr_enum_type      ip_ver	
)
{
  rmnet_smi_info_type *info = NULL;

  if(DS_FWK_INST_IS_VALID(fw_inst, ip_ver))
  {
    if(ip_ver == IPV4_ADDR)
    {
      if(fw_inst->client_info[DS_FWK_CLIENT_RMNET].v4_client_handle != 
          DS_FWK_INST_INVALID)
      {  
        info = rmnet_get_smi_info_ptr(fw_inst->client_info[DS_FWK_CLIENT_RMNET].v4_client_handle);
      }
      if(info != NULL)
      {
        return info->hw_addr;
      }
    }
    else if(ip_ver == IPV6_ADDR)
    {
      if(fw_inst->client_info[DS_FWK_CLIENT_RMNET].v6_client_handle != 
          DS_FWK_INST_INVALID)
      {  
        info = rmnet_get_smi_info_ptr(fw_inst->client_info[DS_FWK_CLIENT_RMNET].v6_client_handle);
      }
      if(info != NULL)
      {
        return info->hw_addr;
      }
    }
  }
  return DS_FWK_INVALID_HW_ADDR;
}

/*===========================================================================
FUNCTION ds_fwk_get_ulog_handle

DESCRIPTION
  Returns ULOG Handle for DS FWK handler
  
DEPENDENCIES
  None

RETURN VALUE
  ulog_handle

SIDE EFFECTS
  None
===========================================================================*/
ULogHandle ds_fwk_get_ulog_handle();

/*===========================================================================
FUNCTION ds_fwk_get_ulog_handle2

DESCRIPTION
  Returns ULOG Handle2 for DS FWK handler
  
DEPENDENCIES
  None

RETURN VALUE
  ulog_handle

SIDE EFFECTS
  None
===========================================================================*/
ULogHandle ds_fwk_get_ulog_handle2();

/*===========================================================================
FUNCTION ds_fwk_ulog_register

DESCRIPTION
  This function call registers DS FWK handler with ULOG framework
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_fwk_ulog_register(void);

/*===========================================================================
FUNCTION DS_FWK_IS_RMNET_EMBEDDED_ICMP_PACKET()

DESCRIPTION
  Retrieves the ICMP packet information from RMNET

PARAMETERS
  meta_pkt_info_ptr  : Meta info of the icmp
  ip_vsn             : Ip version of the icmp packet
  pkt_ptr            : Packet pointer
  rm_client_handle   : Rmnet client

RETURN VALUE
 Boolean  

DEPENDENCIES  
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_fwk_is_rmnet_embedded_icmp_packet
(
  ip_pkt_info_type      *meta_pkt_info_ptr,
  ip_version_enum_type   ip_vsn,
  dsm_item_type         *pkt_ptr,
  uint8                  rm_client_handle
);

/*===========================================================================
FUNCTION DS_FWK_SET_RMNET_EMBEDDED_ICMP_PACKET_INFO()

DESCRIPTION
  Stores the ICMP packet information from RMNET

PARAMETERS
  meta_pkt_info_ptr  : Meta info of the icmp
  ip_vsn             : Ip version of the icmp packet
  pkt_ptr            : Packet pointer
  rm_client_handle   : Rmnet client

RETURN VALUE
 Boolean  
 
DEPENDENCIES
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_fwk_set_rmnet_embedded_icmp_packet_info
(
  ip_pkt_info_type      *meta_pkt_info_ptr,
  ip_version_enum_type   ip_vsn,
  dsm_item_type         *pkt_ptr,
  uint8                  rm_client_handle
);

/*============================================================
FUNCTION DS_FWKI_INPUT_COMMON_PROC_EX()

DESCRIPTION
   Interface client gives the protocol stack a packet to transmit.

PARAMETERS
  ds_fwk_inst_ptr:  framework instance.
  pkt_ref_ptr:       ref to dsm item ptr received
  rx_meta_info_ptr:  ptr to RX meta meta information
  tx_meta_info_ptr:  ptr to TX meta meta information

RETURN VALUE
  0: data was transfered
 -1: data was not transferred, but memory was freed

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================*/
int ds_fwki_input_common_proc_ex
(
  ds_fwk_s_type*                   ds_fwk_inst_ptr,
  dsm_item_type                  ** pkt_ref_ptr,
  ps_rx_meta_info_type            * rx_meta_info_ptr,
  ps_tx_meta_info_type            * tx_meta_info_ptr
);

/*============================================================
FUNCTION DS_FWK_GET_EXTERNAL_IPV6_ADDR()

DESCRIPTION
  This function will get the external address
  
PARAMETERS
  ds_fwk_inst_ptr:   - IN ptr to PDN control block on which to operate on.
  ip_addr_ptr:    value return - the address will be stored here

RETURN VALUE
  0 on successful get of ipv6 extrenal address
 -1 on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================*/
boolean ds_fwk_get_external_ipv6_addrs
(
  ds_fwk_s_type           *ds_fwk_inst_ptr,
  struct ps_in6_addr      *ip_addr_ptr
);

#ifdef TEST_FRAMEWORK
#error code not present
#endif

/*============================================================
  FUNCTION DS_FWK_GET_DL_THROUGHPUT_STATUS()

  DESCRIPTION
   Returns the DL throughput status.
      
  PARAMETERS
   None

  RETURN VALUE
    boolean
    0 - DL Throughput enhancement is enabled.
    1 - DL Throughput enhancement is disabled.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
boolean ds_fwk_get_dl_throughput_status ();

/*============================================================
  FUNCTION DS_FWK_SET_DL_THROUGHPUT_STATUS()

  DESCRIPTION
   Sets DL throughput status.
      
  PARAMETERS
   boolean

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
void ds_fwk_set_dl_throughput_status
(
  boolean dl_tpt_status
);

/*============================================================
  FUNCTION DS_FWK_IS_MODEM_EMBEDDED_CALL_ACTIVE()

  DESCRIPTION
   This API determines if there is a modem embedded call active
   on the fwk instance.
      
  PARAMETERS
   fwk_inst - Framework instance
   ip_type  - IP type of the call

  RETURN VALUE
    boolean

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
boolean ds_fwk_is_modem_embedded_call_active
(
  ds_fwk_s_type     *fwk_inst,
  ip_addr_enum_type ip_type
);


/*============================================================
  FUNCTION DS_FWK_GET_TX_CB()

  DESCRIPTION
   This API retrieves the TX callback registered for a particular framework client
      
  PARAMETERS
   Client Id

  RETURN VALUE
    Function pointer of the registered callback

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
ds_fwk_tx_buf_type*  ds_fwk_get_tx_cb( ds_fwk_client_type   client_id);

#endif 
