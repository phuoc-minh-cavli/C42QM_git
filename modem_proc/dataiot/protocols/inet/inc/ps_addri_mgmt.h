#ifndef PS_ADDRI_MGMT_H
#define PS_ADDRI_MGMT_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        P S _ A D D R I _ M G M T . H

GENERAL DESCRIPTION
  Interface IP Address Management Layer

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c)2008-2018, 2020 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                           EDIT HISTORY FOR MODULE

  $Header: //components/rel/dataiot.mpss/2.0/protocols/inet/inc/ps_addri_mgmt.h#2 $
  $DateTime: 2020/09/25 03:12:23 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/28/18    ds     Code changes for chiltepin(9x05).
03/25/15    skc    Provided new API to create unique address
08/18/14    cx     Added API to get v6 address from handle
04/25/14    ash    Added api to compare prefixes
02/18/14    vl     Modified IPV6_EXT_ADDR_INTERVAL_VERIFY_TIME from 15 mins to 60 mins
11/01/13    rk     Removed feature mobileap.
03/28/12    ash    Modified IPV6_EXT_ADDR_INTERVAL_VERIFY_TIME to 15 mins
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#ifdef FEATURE_DATA_PS_ADDR_MGMT
#include "ps_addri_v6.h"
#include "ds_defs.h"
#include "ds_fwk.h"
/*===========================================================================

                         EXTERNAL DATA DECLARATIONS

===========================================================================*/

typedef enum
{
  PS_ADDR_MGMT_MASK_MIN            = 0x0000,
  PS_ADDR_MGMT_MASK_PREFIX         = 0x0001,
  PS_ADDR_MGMT_MASK_PREFIX_LEN     = 0x0002,
  PS_ADDR_MGMT_MASK_IID            = 0x0004,
  PS_ADDR_MGMT_MASK_GATEWAY_IID    = 0x0008,
  PS_ADDR_MGMT_MASK_PREF_LIFETIME  = 0x0010,
  PS_ADDR_MGMT_MASK_VALID_LIFETIME = 0x0020,
  PS_ADDR_MGMT_MASK_ADDR_STATE     = 0x0040,
  PS_ADDR_MGMT_MASK_ADDR_TYPE      = 0x0080,
  PS_ADDR_MGMT_MASK_REF_CNT        = 0x0100,
  PS_ADDR_MGMT_MASK_INTERNAL_ONLY  = 0x01C0,
  PS_ADDR_MGMT_MASK_DAD_RETRIES    = 0x0200,
  PS_ADDR_MGMT_MASK_ALL            = 0x01FF 
} ps_addr_mgmt_addr_mask_enum_type;


typedef enum 
{
  ADDR_MGMT_DAD_SUCCESS        = 0,
  ADDR_MGMT_DAD_DUP_ADDR       = 1,
  ADDR_MGMT_DAD_NO_MEM         = 2,
  ADDR_MGMT_DAD_NETWORK_REJECT = 3,
  ADDR_MGMT_DAD_INTERNAL_ERR   = 4,
  ADDR_MGMT_DAD_TIMEOUT        = 5
} ps_addr_mgmt_dad_enum_type;

typedef struct
{
  ps_addr_mgmt_addr_mask_enum_type addr_mask;
  uint64  prefix;
  uint64  iid;
  uint64  gateway_iid;
  uint32  pref_lifetime;
  uint32  valid_lifetime;
  uint8   prefix_len;
  uint8   dad_retries;
  
  ds_ipv6_addr_state_enum_type  addr_state;
  ds_ipv6_addr_type_enum_type   addr_type;
} ps_addr_mgmt_addr_info_type;

typedef struct
{
  ps_ip_addr_type                   ip_addr;
  ds_ipv6_addr_type_enum_type       addr_type;
  boolean                           is_psm_restore;
} ps_addr_mgmt_alloc_type;

typedef void ps_addr_mgmt_free_type;

typedef int32 ps_addr_mgmt_handle_type;

/*---------------------------------------------------------------------------
  The time interval in which to verify external addresses to ensure they
  are still being used.
---------------------------------------------------------------------------*/
#define IPV6_EXT_ADDR_INTERVAL_VERIFY_TIME 3600000 /* 60 mins */
#define IPV6_EXT_ADDR_WAIT_TIME            60000  /* 1 min   */

#define IPV6_MAX_ADDR_DAD_RETRIES          1

/*----------------------------------------------------------------------------
  Prefix management Configuration Macros
----------------------------------------------------------------------------*/
/* Hashtable size for prefix mgmt */
#define IPV6_PREFIX_MGMT_HASH_TBL_SIZE     20

/* Hashtable bucket depth for prefix mgmt */
#define IPV6_PREFIX_MGMT_HASH_BUCKET_DEPTH 20 

/* Max Prefix LENGTH*/
#define IPV6_PREFIX_MGMT_MAX_PREFIX_LENGTH 64 

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                         PUBLIC FUNCTION DEFINITIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION PS_ADDR_MGMT_GET_HANDLE_FROM_IP()

DESCRIPTION
  This function retrieves the handle given the IPv6 address structure.

PARAMETERS
  ip_addr_ptr:  The ptr to the ip address

RETURN VALUE
  handle if successful
 -1      if failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ps_addr_mgmt_handle_type ps_addr_mgmt_get_handle_from_ip
(
  ds_fwk_s_type       * fwk_inst_ptr,
  struct ps_in6_addr  * ip_addr_ptr
);

/*===========================================================================
FUNCTION PS_ADDR_MGMT_GET_HANDLE_FROM_IP_EX()

DESCRIPTION
  This function retrieves the handle given the IPv6 address structure.
  While looking for the address it only searches through IP address
  of type specified in addr_type_mask.

PARAMETERS
  ip_addr_ptr:  The ptr to the ip address
  addr_type_mask: IP address type mask.

RETURN VALUE
  handle if successful
 -1      if failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ps_addr_mgmt_handle_type ps_addr_mgmt_get_handle_from_ip_ex
(
  ds_fwk_s_type                         * fwk_inst_ptr,
  struct ps_in6_addr                    *ip_addr_ptr,
  ps_ipv6_addr_type_mask_enum_type      addr_type_mask
);

/*===========================================================================
FUNCTION PS_ADDR_MGMT_GET_HANDLE_FROM_ADDR()

DESCRIPTION
  This function retrieves the handle given the IPv6 address structure.

PARAMETERS
  v6_addr_ptr:  The ptr to the address structure.
  handle_ptr:   The handle from which to derive the IP address structure

RETURN VALUE
  handle if successful
  -1     if failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ps_addr_mgmt_handle_type ps_addr_mgmt_get_handle_from_addr
(
  ds_fwk_v6_addr_s_type        * v6_addr_ptr
);


/*===========================================================================
FUNCTION PS_ADDR_MGMTI_SET_ADDR_INFO()

DESCRIPTION
  This internal function is used to set all address information in the 
  IP address structure. Anything not allowed to be set externally is filtered
  out in the externalized function.

PARAMETERS
  fwk_ptr:     Interface on which the address exists.
  handle_ptr:    The handle to the IP address structure to update.
  addr_info_ptr: The address information structure from which to populate.
  ps_errno:      The error code to return in case of a problem.

RETURN VALUE
  0 success
 -1 failure

DEPENDENCIES
  None

SIDE EFFECTS
  Copies the IP address iformation from the client's IP address information 
  structure to the IP address structure.
===========================================================================*/
int ps_addr_mgmti_set_addr_info
(
  ds_fwk_s_type                     * fwk_ptr,
  ps_addr_mgmt_handle_type          * handle_ptr,
  ps_addr_mgmt_addr_info_type       * addr_info_ptr,
  int16                             * ps_errno
);

/*===========================================================================
FUNCTION PS_ADDR_MGMT_EXT_ADDR_PRESENT()

DESCRIPTION
  This function cancels the timer associated with verifying that an
  external address is still present and in use.

PARAMETERS
  handle_ptr:  The handle to the IP address structure.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_addr_mgmt_ext_addr_present
(
  ds_fwk_s_type                     * fwk_ptr,
  ps_addr_mgmt_handle_type          * handle_ptr
);

/*===========================================================================
FUNCTION PS_ADDR_MGMTI_ALLOC_ADDR()

DESCRIPTION
  This function allocates an address buffer and returns the handle to the 
  caller.

PARAMETERS
  fwk_ptr:       Interface on which the address exists.
  handle_ptr:      The handle to the newly alloc'd IP address structure.
  alloc_info:      Required information to allocate the address;
  ps_errno:        The error code to return in case of a problem.

RETURN VALUE
  0 success
 -1 failure

DEPENDENCIES
  None

SIDE EFFECTS
  Allocates a ps mem buffer for the address.
===========================================================================*/
int ps_addr_mgmti_alloc_addr
(
  ds_fwk_s_type                     * fwk_ptr,
  ps_addr_mgmt_handle_type          * handle_ptr,
  ps_addr_mgmt_alloc_type           * create_info,
  ds_fwk_client_type                  client_type,
  int16                             * ps_errno
);

/*===========================================================================
FUNCTION PS_ADDR_MGMT_FREE_ADDR()

DESCRIPTION
  This function frees the address buffer associated with the passed handle.

PARAMETERS
  fwk_ptr:         Fwk instance on which the address exists.
  handle_ptr:      The handle to the newly alloc'd IP address structure.
  free_info_ptr:   Any additional required information needed.
  ps_errno:        The error code to return in case of a problem.

RETURN VALUE
  0 success
 -1 failure

DEPENDENCIES
  None

SIDE EFFECTS
  Allocates a ps mem buffer for the address.
===========================================================================*/
int ps_addr_mgmt_free_addr
(
  ds_fwk_s_type                     * fwk_ptr,
  ps_addr_mgmt_handle_type          * handle_ptr,
  ps_addr_mgmt_free_type            * free_info,
  int16                             * ps_errno
);

/*===========================================================================
FUNCTION PS_ADDR_MGMT_GET_ADDR_INFO()

DESCRIPTION
  This function is used to retrieve information about an address to the
  client.

PARAMETERS
  handle_ptr:    The handle to the address structure from which to retrieve
                 the information.
  addr_info_ptr: The address information structure to populate.
  ps_errno:      The error code to return in case of a problem.

RETURN VALUE
  0 success
 -1 failure

DEPENDENCIES
  None

SIDE EFFECTS
  Copies the IP address iformation from the IP address structure to the 
  client's address information structure.
===========================================================================*/
int ps_addr_mgmt_get_addr_info
(
  ps_addr_mgmt_handle_type    * handle_ptr,
  ps_addr_mgmt_addr_info_type * addr_info_ptr,
  int16                       * ps_errno
);

/*===========================================================================
FUNCTION PS_ADDR_MGMT_SET_ADDR_INFO()

DESCRIPTION
  This function is used to set address information in the IP address
  structure.

PARAMETERS
  handle_ptr:    The handle to the IP address structure to update.
  addr_info_ptr: The address information structure from which to populate.
  ps_errno:      The error code to return in case of a problem.

RETURN VALUE
  0 success
 -1 failure

DEPENDENCIES
  None

SIDE EFFECTS
  Copies the IP address iformation from the client's IP address information 
  structure to the IP address structure.
===========================================================================*/
int ps_addr_mgmt_set_addr_info
(
  ds_fwk_s_type                     * fwk_ptr,
  ps_addr_mgmt_handle_type          * handle_ptr,
  ps_addr_mgmt_addr_info_type       * addr_info_ptr,
  int16                             * ps_errno
);

/*===========================================================================
FUNCTION PS_ADDR_MGMT_FREE_EXT_V6_ADDRESSES()

DESCRIPTION
  This function will delete all external IPv6 address from um_iface

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_addr_mgmt_free_ext_v6_addresses
(
  ds_fwk_s_type                     * fwk_ptr
);

/*===========================================================================
FUNCTION PS_ADDR_MGMT_GET_ADDR_FROM_HANDLE()

DESCRIPTION
  This function retrieves the IPv6 address structure for the given handle.

PARAMETERS
  handle_ptr:   The handle from which to derive the IP address structure

RETURN VALUE
  IPv6 address structure if successful
  NULL                   if failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ds_fwk_v6_addr_s_type * ps_addr_mgmt_get_addr_from_handle
(
  ps_addr_mgmt_handle_type  *  handle_ptr
);

/*===========================================================================
FUNCTION PS_ADDR_MGMT_ALLOC_UNIQUE_ADDR()

DESCRIPTION
  This function allocates an address buffer as well as do dad() for
  the new allocated address.

PARAMETERS
  
  fwk_inst_ptr:    Fwk instance on which the address exists.
  handle_ptr:      The handle to the newly alloc'd IP address structure.
  alloc_info:      Required information to allocate the address;
  ps_errno:        The error code to return in case of a problem.
  

RETURN VALUE
  0: create address successfully
  -1: create address failed

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_addr_mgmt_alloc_unique_addr
(
  ds_fwk_s_type            * fwk_inst_ptr,
  ps_addr_mgmt_handle_type * handle_ptr,
  ps_addr_mgmt_alloc_type  * create_info_ptr,
  ds_fwk_client_type         client_type,
  int16                    * ps_errno
);

#endif /* FEATURE_DATA_PS_ADDR_MGMT */
#endif /* PS_ADDRI_MGMT_H */
