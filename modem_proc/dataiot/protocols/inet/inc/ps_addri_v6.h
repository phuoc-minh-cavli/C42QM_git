#ifndef PS_ADDRI_V6_H
#define PS_ADDRI_V6_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        P S _ A D D R I _ V 6 . H

GENERAL DESCRIPTION
 Internet Protocol Version 6 - Interface Layer Functionality

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2008-2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR MODULE

  $Header: //components/rel/dataiot.mpss/2.0/protocols/inet/inc/ps_addri_v6.h#1 $
  $DateTime: 2020/03/17 08:03:16 $

  when      who    what, where, why
--------    ---    ----------------------------------------------------------
08/13/18    ds     Code changes as per new FWK design for IOT.
08/22/14    skc    Changed default probe behavior
08/18/14    cx     Add support to not probe the recent added ext addresses
07/27/14    cx     Add support for reporting NA/NS transaction to diag
01/19/14    cx     Support for unused external addresses probe optimization
02/11/09    pp     NULL check for ipv6_addrs array elements before accessing
                   them from iface_ptr.
12/14/08    pp     Created module as part of Common Modem Interface:
                   Public/Private API split.
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */

#if defined (FEATURE_DATA_PS) && defined (FEATURE_DATA_PS_IPV6)
#include "ps_ip6_addr.h"
#include "ds_Utils_DebugMsg.h"
#include "event_defs.h"
#include "ds_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                              TYPE DEFINITIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  Converting a value in seconds to mili seconds
---------------------------------------------------------------------------*/
#define SEC_TO_MSEC(val_sec) ((val_sec) * 1000)

/*---------------------------------------------------------------------------
TYPEDEF PS_IPV6_ADDR_TYPE_MASK_ENUM_TYPE

DESCRIPTION
  The bitmask for the IPv6 address types. Public/Private, etc.
---------------------------------------------------------------------------*/
typedef enum
{
  // This mask is for public address
  IPV6_ADDR_TYPE_MASK_PUBLIC           = 1 << DS_IPV6_ADDR_TYPE_PUBLIC,
  // This mask is for in use private shareable address
  IPV6_ADDR_TYPE_MASK_PRIV_SHARED      = 1 << DS_IPV6_ADDR_TYPE_PRIV_SHARED,
  // This mask is for in use private unique address
  IPV6_ADDR_TYPE_MASK_PRIV_UNIQUE      = 1 << DS_IPV6_ADDR_TYPE_PRIV_UNIQUE,
  // This mask is for in use by an external device
  IPV6_ADDR_TYPE_MASK_EXTERNAL         = 1 << DS_IPV6_ADDR_TYPE_EXTERNAL,
  // This mask is for all address type
  IPV6_ADDR_TYPE_ALL                   = 0xFFFF
} ps_ipv6_addr_type_mask_enum_type;

/*---------------------------------------------------------------------------
TYPEDEF IPV6_EXT_ADDR_PROBE_CONFIG_ITEMS_TYPE

DESCRIPTION
  Configuration items for IPv6 external address probe 
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  // Threshold between probing phase in percentage 
  uint8     cache_threshold;  
  // The interval in sec when MDM should probe in pre-threhold phase
  uint16    pre_threshold_probe_interval;    
  // The period in sec when MDM should not probe in post-threshold phase
  uint16    post_threshold_probe_silent_period;        
} ipv6_ext_addr_probe_config_items_type;

/*----------------------------------------------------------------------------
  Default configuration for IPv6 external address probe  
  All time is in second, threshold is in percentage
---------------------------------------------------------------------------*/
#define IPV6_EXT_ADDR_DEFAULT_PRE_THRESHOLD_PROBE_INTERVAL  0
#define IPV6_EXT_ADDR_DEFAULT_POST_THRESHOLD_SILENT_PERIOD  30
#define IPV6_EXT_ADDR_DEFAULT_THRESHOLD_PERCENT             80

/*----------------------------------------------------------------------------
  Modem will not probe the external address who are allocated in the following
  seconds
---------------------------------------------------------------------------*/
#define IPV6_EXT_ADDR_TENTATIVE_TIME 3
/*----------------------------------------------------------------------------
  Check if it is at pre-threshold stage or post-threshold stage
---------------------------------------------------------------------------*/
#define PS_V6_EXT_ADDRS_THRESHOLD_TRIGGERED(fwk_inst_ptr) \
        (ps_v6_get_cache_count(fwk_inst_ptr) >\
        ps_v6_get_cache_threshold())

/*----------------------------------------------------------------------------
  Check if the silent timer is allocated
---------------------------------------------------------------------------*/
#define PS_V6_EXT_ADDRS_SILENT_TIMER_IS_ALLOC(fwk_inst_ptr) \
        (fwk_inst_ptr->ipv6_ext_addr_silent_timer != \
        PS_TIMER_INVALID_HANDLE)

/*----------------------------------------------------------------------------
  Check if the silent timer is running
---------------------------------------------------------------------------*/
#define PS_V6_EXT_ADDRS_SILENT_TIMER_IS_RUNNING(fwk_inst_ptr) \
        (ps_timer_is_running\
        (fwk_inst_ptr->ipv6_ext_addr_silent_timer))

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                         PUBLIC MACRO DEFINITIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*===========================================================================
MACRO PS_IPV6_IS_PRIV_IID_UNIQUE()

DESCRIPTION
  This macro checks to see if the iid is locally unique to the interface.

PARAMETERS
  fwk_ptr:   Target fwk ptr
  uint64:      IID to be verified as unique

RETURN VALUE
  TRUE  if the IID is unique on the interface
  FALSE if the IID is not unique.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IPV6_IS_PRIV_IID_UNIQUE ps_ipv6_is_priv_iid_unique
INLINE boolean ps_ipv6_is_priv_iid_unique
(
  ds_fwk_s_type        *fwk_ptr,
  uint64               *priv_iid
)
{
  uint8 addr_index;
  ds_fwk_v6_addr_s_type  *v6_addr_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( !DS_FWK_INST_IS_VALID( fwk_ptr, IPV6_ADDR ) )
  {
    LOG_MSG_ERROR_0("Invalid fwk_ptr in ps_ipv6_is_priv_iid_unique");
    DS_ASSERT(0);
    return FALSE;
  }

  /*Verify the IID doesn't match with the default one also*/
  v6_addr_ptr = fwk_ptr->v6_net_info.ipv6_addrs[DS_DEFAULT_V6_INDEX];
  
  if( (v6_addr_ptr->iid == *priv_iid) ||
      (fwk_ptr->v6_net_info.gateway_iid ==
      *priv_iid))
  {
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Verify this IID does not already exist on this fwk instance as well.
  -------------------------------------------------------------------------*/
  for(addr_index = 0; addr_index < DS_MAX_IPV6_ADDRS; addr_index++)
  {
    if( fwk_ptr->v6_net_info.ipv6_addrs[addr_index] == NULL )
    {
      continue;
    }

    if( (fwk_ptr->v6_net_info.ipv6_addrs[addr_index]->iid == *priv_iid) ||
        (fwk_ptr->v6_net_info.gateway_iid == *priv_iid))
    {
      return FALSE;
    }
  } /* end for */    
  
  return TRUE;
}


/*===========================================================================
MACRO PS_IPV6_IS_ADDR_SPACE_AVAIL()

DESCRIPTION
  This macro checks to see if there is space available on the interface for
  an additional IPv6 address.

PARAMETERS
  fwk_inst_ptr:   Target fwk instance ptr

RETURN VALUE
  TRUE  if space is available.
  FALSE if there is no space available.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IPV6_IS_ADDR_SPACE_AVAIL ps_ipv6_is_addr_space_avail
INLINE boolean ps_ipv6_is_addr_space_avail
(
  ds_fwk_s_type        *fwk_inst_ptr,
  uint8                *index
)
{
  uint8  addr_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    If we hit NULL there is space available. Start from 1 as 0 is always the
    public addr.
  -------------------------------------------------------------------------*/
  for(addr_index = DS_MAX_IPV6_PREFIXES; addr_index < DS_MAX_IPV6_ADDRS; addr_index++)
  {

    if(fwk_inst_ptr->v6_net_info.ipv6_addrs[addr_index] == NULL)
    {
        *index = addr_index;
        return TRUE;
    }
  } /* end for */

  return FALSE;
}


/*===========================================================================
MACRO PS_FIND_IPV6_ADDR()

DESCRIPTION
  This macro looks for a particular type of privacy address on an
  interface.

PARAMETERS
  fwk_inst_ptr:   Target fwk ptr
  addr_state:
  addr_type:

RETURN VALUE
  TRUE         If an address match is found.
  FALSE        If no privacy address match is found.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_FIND_IPV6_ADDR_TYPE ps_find_ipv6_addr_type
INLINE boolean ps_find_ipv6_addr_type
(
  ds_fwk_s_type                      *fwk_inst_ptr,
  ds_ipv6_addr_state_enum_type        addr_state,
  ds_ipv6_addr_type_enum_type         addr_type,
  ds_fwk_v6_addr_s_type            **v6_addr_ptr_ptr
)
{
  uint8 addr_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( !DS_FWK_INST_IS_VALID( fwk_inst_ptr, IPV6_ADDR ) )
  {
    LOG_MSG_ERROR_0("Invalid FWK instance in ps_find_ipv6_addr_type");
    DS_ASSERT(0);
    return FALSE ;
  }

  /*-------------------------------------------------------------------------
    The default index will always be a public address. Return that if no
    available address is found.
  -------------------------------------------------------------------------*/
  for(addr_index = 0; addr_index < DS_MAX_IPV6_ADDRS; addr_index++)
  {
    if( fwk_inst_ptr->v6_net_info.ipv6_addrs[addr_index] == NULL )
  {
      continue;
    }

    if( (fwk_inst_ptr->v6_net_info.ipv6_addrs[addr_index]->addr_state ==
           addr_state) &&
        (fwk_inst_ptr->v6_net_info.ipv6_addrs[addr_index]->addr_type ==
           addr_type) )
    {
      *v6_addr_ptr_ptr = fwk_inst_ptr->v6_net_info.ipv6_addrs[addr_index];
      return TRUE;
    }
  } /* end for */

  return FALSE;
}

/*===========================================================================
MACRO PS_IPV6_DEFAULT_ADDR_STATE_VALID()

DESCRIPTION
  This macro checks to see if the default prefix of the interface is in
  a valid state for sending. (ie. VALID state, not DEPRECATED)

PARAMETERS
  fwk_inst_ptr: Target fwk ptr

RETURN VALUE
  TRUE  - the default prefix is in valid state
  FALSE - the default prefix is not valid

DEPENDENCIES
  None

SIDE EFFECTS
  None
================================================== =========================*/
#define PS_IPV6_DEFAULT_ADDR_STATE_VALID ps_ipv6_default_addr_state_valid
INLINE boolean ps_ipv6_default_addr_state_valid
(
  ds_fwk_s_type * fwk_inst_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( !DS_FWK_INST_IS_VALID( fwk_inst_ptr, IPV6_ADDR ) )
  {
    LOG_MSG_ERROR_0("Invalid fwk_ptr in ps_ipv6_default_addr_state_valid");
    DS_ASSERT(0);
    return FALSE ;
  } 

  if( ( fwk_inst_ptr->v6_net_info.ipv6_addrs[DS_DEFAULT_V6_INDEX] != NULL ) &&
      ( fwk_inst_ptr->v6_net_info.ipv6_addrs[DS_DEFAULT_V6_INDEX]->addr_state == DS_IPV6_ADDR_STATE_VALID)
    )
  {
    return TRUE;
  }

  return FALSE;
}

/*===========================================================================
MACRO PS_IPV6_EXTRA_ADDR_IS_AVAIL()

DESCRIPTION
  This macro checks to see if there are available unassigned IPv6
  privacy addresses.
  
PARAMETERS
  fwk_inst_ptr  : Target fwk ptr
  v6_addr_ptr   : Ptr to the v6 address structure

RETURN VALUE
  TRUE is there is an extra address available
  FALSE if there is no extra address already available

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IPV6_EXTRA_ADDR_IS_AVAIL(fwk_inst_ptr,v6_addr_ptr)      \
     (PS_FIND_IPV6_ADDR_TYPE(fwk_inst_ptr,                        \
                                    DS_IPV6_ADDR_STATE_UNASSIGNED,            \
                                    DS_IPV6_ADDR_TYPE_INVALID,                \
                                    v6_addr_ptr) ||                        \
      (PS_FIND_IPV6_ADDR_TYPE(fwk_inst_ptr,                      \
                              DS_IPV6_ADDR_STATE_TENTATIVE,           \
                              DS_IPV6_ADDR_TYPE_INVALID,              \
                              v6_addr_ptr) ))

/*===========================================================================
FUNCTION PS_IPV6_IID_TIMER_HANDLER()

DESCRIPTION
  This function is a callback for the privacy extension lifetime timers. It
  is called when an IPv6 private IID timer expires.  It will take the
  appropriate action depedning upon if the timer that expired was the
  preferred or valid lifetime.

PARAMETERS
  user_data_ptr: the callback information which will be the IPv6 address
                 information structure

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_ipv6_iid_timer_handler
(
  void *user_data_ptr
);

/*===========================================================================
FUNCTION PS_IPV6_UNUSED_IID_HANDLER()

DESCRIPTION
  This function serves as a callback for three purposes:
  1)The retry timer for DAD verification expires. In this case (only for
    internal addresses) the timer is restarted (if tries remain) and the
    dad function called.
  2)The mobile needs to check that an external address is still being used.
  2)The privacy extension unsed timer to indicate when an IPv6 private address
    has not been bound by a socket in the required time. The function will
    notify the application(s) and free the address.

PARAMETERS
  user_data_ptr: the callback information which will be the IPv6 address
                 information structure

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_ipv6_unused_iid_handler
(
  void *user_data_ptr
);

/*===========================================================================
PS_ADDR_V6_INIT

DESCRIPTION
  Initializes ps_addr_v6 module

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_addr_v6_init
(
  void
);

/*===========================================================================
FUNCTION PS_FIND_IPV6_ADDR()

DESCRIPTION
  This function locates an IPv6 address on any UP interface.

PARAMETERS
  ip_addr_ptr:       Pointer to the ip address
  v6_addr_ptr_ptr:   Pointer to the v6_addr_ptr to fill if the addr is found
  fwk_inst_ptr_ptr:  Pointer to the fwk_inst_ptr to return if the address
                     matches one owned by that interface.

RETURN VALUE
  TRUE  if the address is located
  FALSE if the address cannot be found

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_find_ipv6_addr
(
  struct ps_in6_addr                      *ip_addr_ptr,
  ds_fwk_v6_addr_s_type                 **v6_addr_ptr_ptr,
  ds_fwk_s_type                         **fwk_inst_ptr_ptr,
  ps_ipv6_addr_type_mask_enum_type      addr_type_mask
);

/*===========================================================================
FUNCTION PS_CHECK_IPV6_IID_CONFLICT()

DESCRIPTION
  This function checks for a conflict of passed IID with any of the
  associated iid's of an fwk ptr.

PARAMETERS
  *fwk_inst_ptr  - Pointer to the fwk instance to operate on.
   iid            - IID to check for conflict.
   exclude_primary_iid - if set dont include primary IID 
                         while checking for conflict

RETURN VALUE 
  -1 in case of any error. 
   1 in case of conflict
   0 otherwise.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
int ps_check_ipv6_iid_conflict
(
  ds_fwk_s_type *fwk_inst_ptr,  /* Pointer to the interface to operate on */
  uint64         iid,            /* IID to check for conflict */
  boolean        exclude_primary_iid /* if set dont include primary IID 
                                        while checking for conflict */
);

/*===========================================================================
FUNCTION PS_GENERATE_PRIV_IPV6_ADDR()

DESCRIPTION
  This function will generate a new private IPv6 as per RFC 3041. Based on
  the input to the function it will create a private shared (interface
  based) or private unique (owned and useable only by a particular socket).

PARAMETERS
  fwk_inst_ptr : ptr to fwk instance control block on which to operate on.
  ip_addr_ptr:    value return - the address will be stored here

RETURN VALUE
  0 on successful generation of private ipv6 address
 -1 on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_generate_priv_ipv6_addr
(
  ds_fwk_s_type           *fwk_inst_ptr,
  ps_ip_addr_type         *ip_addr_ptr,
  ps_ipv6_iid_params_type *iid_param_ptr,
  int16                   *ps_errno
);

/*===========================================================================
FUNCTION PS_ALLOC_PRIV_IPV6_ADDR()

DESCRIPTION
  This function will verify address space is available, allocate a ps mem
  item for a new private IPv6 address, allocate timers for the address,
  and populate the address with the necessary state information. In addition
  this function will start the DAD process for broadcast interfaces.

PARAMETERS
  fwk_inst_ptr  : ptr to fwk inst control block on which to operate on.
  ip_addr_ptr:    return value - the address will be stored here
  iid_param_ptr:  The type of address to create as well as whether it was
                  an application that requested the address or not.
  ps_errno:       the returned error code

RETURN VALUE
  0 for success
 -1 for failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_alloc_priv_ipv6_addr
(
  ds_fwk_s_type           *fwk_inst_ptr,
  ds_fwk_v6_addr_s_type   **v6_addr_ptr_ptr,
  ps_ipv6_iid_params_type *iid_param_ptr,
  boolean                  extra_addr,
  boolean                  store_addr,
  int16                   *ps_errno
);

/*===========================================================================
FUNCTION PS_DELETE_PRIV_IPV6_ADDR()

DESCRIPTION
  This function will delete an old private IPv6 address. Once the ref_cnt
  has gone to zero this function will take care of deleting and cleaning up
  the interface's ipv6 addresses.

PARAMETERS
  fwk_inst_ptr: ptr to fwk inst control block on which to operate on.
  v6_addr_ptr:    the ipv6 address location to be removed from the interface

RETURN VALUE
  0 for success
 -1 for failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_delete_priv_ipv6_addr
(
  ds_fwk_s_type           *fwk_inst_ptr,
  ds_fwk_v6_addr_s_type   *v6_addr_ptr
);

#ifdef __cplusplus
}
#endif

/*===========================================================================
FUNCTION PS_V6_GET_PROBE_CONFIG_ITEMS

DESCRIPTION
  This function returns the ipv6 external address probe config information

PARAMETERS

RETURN VALUE
  The pointer point to config items

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ipv6_ext_addr_probe_config_items_type *
ps_v6_get_probe_config_items
(
  void
);

/*===========================================================================
FUNCTION PS_V6_GET_CACHE_COUNT

DESCRIPTION
  This function retunrs the number of entries in v6 fwk instance 
  address cache

PARAMETERS
  The pointer point to Fwk instance
  
RETURN VALUE
 The number of entries in v6 address cache

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_v6_get_cache_count
(
  ds_fwk_s_type * fwk_ptr
);

/*===========================================================================
FUNCTION PS_V6_GET_CACHE_THRESHOLD()

DESCRIPTION
  This function returns the probing threshold in number

PARAMETERS
  None

RETURN VALUE
  The probing threshold
  
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_v6_get_cache_threshold
( 
  void
);

/*===========================================================================
FUNCTION PS_V6_PROBE_ALL_EXT_ADDRS()

DESCRIPTION
  Probes all valid external IPv6 addresses

PARAMETERS
  fwk_inst_ptr: ptr to fwk inst on which to operate on.

RETURN VALUE
  None
  
DEPENDENCIES
  None

SIDE EFFECTS
  It will cancel all the unused iid timers
===========================================================================*/
void ps_v6_probe_all_ext_addrs
(
   ds_fwk_s_type * fwk_inst_ptr
);

/*===========================================================================
FUNCTION PS_V6_RESTART_ALL_UNUSED_IID_TIMER()

DESCRIPTION
  This function will restart all external addresses's unused iid timer  

PARAMETERS
  fwk_inst_ptr: ptr to fwk inst on which to operate on.

RETURN VALUE
  None
  
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_v6_restart_all_unused_iid_timer
(
  ds_fwk_s_type * fwk_inst_ptr
);

/*===========================================================================
FUNCTION PS_V6_SILENT_TIMER_HANDLER()

DESCRIPTION
  A dummy call back function of silent timer 

PARAMETERS
  user_data_ptr: ptr to argument

RETURN VALUE
  None
  
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_ipv6_silent_timer_handler
(
  void *user_data_ptr
);

#endif /* FEATURE_DATA_PS && FEATURE_DATA_PS_IPV6 */
#endif /* PS_ADDRI_V6_H */
