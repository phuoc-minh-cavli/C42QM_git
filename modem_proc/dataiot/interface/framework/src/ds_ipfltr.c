/*===========================================================================

                      DS _ I P F L T R . C

DESCRIPTION
  This file provides functions to use IP filters 
  processing of IP packets received on that framework IP instance.

Copyright (c) 2018 - 2022 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/dataiot.mpss/2.0/interface/framework/src/ds_ipfltr.c#4 $
  $DateTime: 2022/07/05 22:10:54 $ $Author: pwbldsvc $

when        who         what, where, why
--------    ---    ----------------------------------------------------------
02/19/18   svaka      File created.
===========================================================================*/

/*===========================================================================

                       INCLUDE FILES FOR THE MODULE

===========================================================================*/
#include "ds_headers.h"
#include "ps_mem.h"
#include "ds_ipfltr.h"
#include "ps_in.h"
#include "ps_ip6_addr.h"
#include "ps_iputil.h"
#include "ps_byte.h"
#include "ps_pkt_info.h"
#include "ps_utils.h"
#include "ds_crit_sect.h"
#include <stringl/stringl.h>
#include "sio.h" 
#include "ds_ipfltr_defs.h"
#include "ds_fwki.h"
#include "ds_defsi.h"
#include "ds_pdn_defs.h"


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------------
  Defines maximum port that can be opened on a mobile
---------------------------------------------------------------------------*/
#define MAX_PORT  0xFFFF

/*---------------------------------------------------------------------------
  The IP filter information : global
---------------------------------------------------------------------------*/
q_type global_ipfltr_info[FLTR_CLIENT_MAX];

/*---------------------------------------------------------------------------
  Optimized global filter array for storing socket filters
---------------------------------------------------------------------------*/
ds_dl_opt_filter_spec_type *ds_global_dl_opt_fltr_arr = NULL;

/*---------------------------------------------------------------------------
  Optimized global filter array to filter uplink packets on RM_GROUP
---------------------------------------------------------------------------*/
ds_dl_opt_filter_spec_type *ds_global_ul_opt_fltr_arr = NULL;

/*---------------------------------------------------------------------------
  Tuning the number of ip filter buffers needed by this module
---------------------------------------------------------------------------*/
#define DS_IPFLTR_BUF_SIZE ((sizeof(ds_ipfilteri_type) + 3) & ~3)

/*---------------------------------------------------------------------------
  PS_IPFLTR_TYPE_IS_ESP macro definition
---------------------------------------------------------------------------*/
#define PS_IPFLTR_TYPE_IS_ESP(ipfltr)                                        \
      (((ipfltr.ip_vsn == IP_V4) &&                                       \
       (ipfltr.ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_NEXT_HDR_PROT) &&   \
       (ipfltr.ip_hdr.v4.next_hdr_prot == PS_IPPROTO_ESP)) ||             \
      ((ipfltr.ip_vsn == IP_V6) &&                                        \
       (ipfltr.ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_NEXT_HDR_PROT) &&   \
       (ipfltr.ip_hdr.v6.next_hdr_prot == PS_IPPROTO_ESP)))


#define DS_IPFLTR_BUF_NUM       120
#define DS_IPFLTR_BUF_HIGH_WM    80
#define DS_IPFLTR_BUF_LOW_WM     30

/*----------------------------------------------------------------------------
  Allocate memory to hold ds_ipfltr along with ps_mem header
----------------------------------------------------------------------------*/
static int ds_ipfltr_buf_mem[PS_MEM_GET_TOT_SIZE_OPT
                                   (
                                     DS_IPFLTR_BUF_NUM,
                                     DS_IPFLTR_BUF_SIZE
                                   )];

#ifdef FEATURE_DATA_PS_MEM_DEBUG
/*----------------------------------------------------------------------------
  Array of pointers used to facilitate easy debugging.  The first one points
  to the ps_mem header and the latter points to ds_ipfltr_buf
----------------------------------------------------------------------------*/
static ps_mem_buf_hdr_type     * ds_ipfltr_buf_hdr[DS_IPFLTR_BUF_NUM];
static ds_ipfilteri_type       * ds_ipfltr_buf_ptr[DS_IPFLTR_BUF_NUM];
#endif /* FEATURE_DATA_PS_MEM_DEBUG */

/*---------------------------------------------------------------------------
DS ipfltr error codes
---------------------------------------------------------------------------*/
#define DS_IPFLTR_ERR_NONE               0
#define DS_IPFLTR_ERR_INVALID_CLIENT_ID  1
#define DS_IPFLTR_ERR_INVALID_HANLDE     2
#define DS_IPFLTR_ERR_INVALID_OP         3
#define DS_IPFLTR_ERR_DELETE_FAIL        4
#define DS_IPFLTR_ERR_INALID_PS_ERR_PTR  5

/*===========================================================================

                                    MACROS

===========================================================================*/
/*===========================================================================
MACRO IS_PORT_RANGE_VALID()

DESCRIPTION
  Validates if given range of ports is within maximum permissible port number
  i.e within 65535

  NOTE : port and range needs to be converted to uint32 since otherwise
         check will always be TRUE as values wraparound when it overflows

PARAMETERS
  port  : starting port number in network order
  range : range of ports. port + range gives ending port number

RETURN VALUE
   TRUE  : if port is <= 65535
   FALSE : otherwise
===========================================================================*/
#define IS_PORT_RANGE_VALID(port, range)                                 \
  (((((uint32) ps_ntohs(port)) + ((uint32) (range))) > MAX_PORT) ? FALSE : TRUE)

/*===========================================================================
FUNCTION DS_IPFLTRI_ALLOC_BUF()

DESCRIPTION
  This function allocates a memory buffer to contain the IP filter being
  added to the Framework instance.

DEPENDENCIES
  None.

RETURN VALUE
  Ptr to the allocated buffer, NULL if the buffer could not be allocated

SIDE EFFECTS
  None.

===========================================================================*/
#define ds_ipfltri_alloc_buf()                                \
        ps_mem_get_buf(PS_MEM_DS_IPFLTER_TYPE)

/*===========================================================================
FUNCTION DS_IPFLTRI_FREE_BUF()

DESCRIPTION
  This function frees up the memory buffer coantaining the IP
  filter in the Framework instance.

DEPENDENCIES
  The buffer should have been created using ds_ipfltri_alloc_buf()
  and it should not be on a queue.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
#define ds_ipfltri_free_buf( mi_ptr_ptr )                          \
        PS_MEM_FREE(mi_ptr_ptr)


/*===========================================================================

                      INTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION DS_IPFLTRI_VALIDATE_IPV4_PARAM()

DESCRIPTION
  Validates IPV4 parameters of a filter

PARAMETERS
  client_id     : Filtering client id
  fltr_ptr      : ptr to a filter
  next_hdr_prot : OUT param indicating the higher level protocol

RETURN VALUE
  TRUE  : if parameters are valid
  FALSE : otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  err_mask is updated with appropriate error codes for values and enums
  which failed validation. Because variables are only validated if the enums
  are set in the field masks the only error masks that will be updated will
  be those corresponding to the values set within the field masks.

  If field mask is IPFLTR_MASK_IP4_NONE, errmask is set to
  IPFLTR_MASK_IP4_ALL

  If a bit mask is specified, which is outside the supported range, errmask
  is updated to those bits
===========================================================================*/
static boolean ds_ipfltri_validate_ipv4_param
(
  ds_ipfltr_client_id_enum_type          client_id,
  ip_filter_type                       * fltr_ptr,
  ps_ip_protocol_enum_type             * next_hdr_prot
)
{
  boolean  is_fltr_valid = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  fltr_ptr->ip_hdr.v4.err_mask = 0;

  if (fltr_ptr->ip_hdr.v4.field_mask == IPFLTR_MASK_IP4_NONE)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"ds_ipfltri_validate_ipv4_param(): "
                    "Invalid IP_V4 field mask: 0x%x",
                    fltr_ptr->ip_hdr.v4.field_mask);
    fltr_ptr->ip_hdr.v4.err_mask = IPFLTR_MASK_IP4_ALL;
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    If a bit mask is specified, which is outside the supported range, flag
    error and continue with validation so that other parameters are validated
  -------------------------------------------------------------------------*/
  if (fltr_ptr->ip_hdr.v4.field_mask & ~IPFLTR_MASK_IP4_ALL)
  {
    fltr_ptr->ip_hdr.v4.err_mask =
      (fltr_ptr->ip_hdr.v4.field_mask & ~IPFLTR_MASK_IP4_ALL);
    is_fltr_valid = FALSE;
  }

  /*-----------------------------------------------------------------------
    If Source address is used in a Tx filter or if Destination address is
    used in a Rx filter, make sure that only a single address value is
    specified. Filters are installed per framework instance 
    CLAT is an exception since the filters can match a range of addresses
  -----------------------------------------------------------------------*/
  if (client_id != FLTR_CLIENT_CLAT)
  {
    if (client_id >= FLTR_CLIENT_INPUT_MAX)
    {
      if ((fltr_ptr->ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_SRC_ADDR) &&
          fltr_ptr->ip_hdr.v4.src.subnet_mask.ps_s_addr != 0xFFFFFFFFUL)
      {
        fltr_ptr->ip_hdr.v4.err_mask |= IPFLTR_MASK_IP4_SRC_ADDR;
        is_fltr_valid = FALSE;
      }
    }
    else
    {
      if ((fltr_ptr->ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_DST_ADDR) &&
          (fltr_ptr->ip_hdr.v4.dst.subnet_mask.ps_s_addr != 0xFFFFFFFFUL))
      {
        fltr_ptr->ip_hdr.v4.err_mask |= IPFLTR_MASK_IP4_DST_ADDR;
        is_fltr_valid = FALSE;
      }
    }
  }

  /*-------------------------------------------------------------------------
    Make sure that only one of TCP, UDP, or ICMP is specified as higher layer
    protocol in a filter. Also make sure that ICMP filters are not specified
    by QoS clients
  -------------------------------------------------------------------------*/
  if (fltr_ptr->ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_NEXT_HDR_PROT)
  {
    if (fltr_ptr->ip_hdr.v4.next_hdr_prot == PS_IPPROTO_TCP ||
        fltr_ptr->ip_hdr.v4.next_hdr_prot == PS_IPPROTO_UDP ||
        fltr_ptr->ip_hdr.v4.next_hdr_prot == PS_IPPROTO_ICMP ||
        fltr_ptr->ip_hdr.v4.next_hdr_prot == PS_IPPROTO_ICMP6 ||
        fltr_ptr->ip_hdr.v4.next_hdr_prot == PS_IPPROTO_IGMP ||
        fltr_ptr->ip_hdr.v4.next_hdr_prot == PS_IPPROTO_ESP ||
        fltr_ptr->ip_hdr.v4.next_hdr_prot == PS_IPPROTO_AH ||
        fltr_ptr->ip_hdr.v4.next_hdr_prot == PS_IPPROTO_TCP_UDP)
    {
      *next_hdr_prot =
        (ps_ip_protocol_enum_type) fltr_ptr->ip_hdr.v4.next_hdr_prot;
    }
    else
    {
      fltr_ptr->ip_hdr.v4.err_mask |= IPFLTR_MASK_IP4_NEXT_HDR_PROT;
      is_fltr_valid = FALSE;
    }
  }
  else if (client_id == FLTR_CLIENT_SOCKETS)
  {
    fltr_ptr->ip_hdr.v4.err_mask |= IPFLTR_MASK_IP4_NEXT_HDR_PROT;
    is_fltr_valid = FALSE;
  }

  if ((fltr_ptr->ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_TOS) &&
      fltr_ptr->ip_hdr.v4.tos.mask == 0)
  {
    fltr_ptr->ip_hdr.v4.err_mask |= IPFLTR_MASK_IP4_TOS;
    is_fltr_valid = FALSE;
  }

  if (is_fltr_valid == FALSE)
  {
    DS_MSG2(MSG_LEGACY_HIGH,"ds_ipfltri_validate_ipv4_param(): "
                    "IP_FLTR V4 field_mask = 0x%x, IP_FLTR V4 err_mask = 0x%x",
                    fltr_ptr->ip_hdr.v4.field_mask,
                    fltr_ptr->ip_hdr.v4.err_mask);
  }

  return is_fltr_valid;

} /* ds_ipfltri_validate_ipv4_param() */

/*===========================================================================
FUNCTION DS_IPFLTRI_VALIDATE_IPV6_PARAM()

DESCRIPTION
  Validates IPV6 parameters of a filter

PARAMETERS
  client_id     : Filtering client id
  fltr_ptr      : ptr to a filter
  next_hdr_prot : OUT param indicating the higher level protocol

RETURN VALUE
  TRUE  : if parameters are valid
  FALSE : otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  err_mask is updated with appropriate error codes for values and enums
  which failed validation. Because variables are only validated if the enums
  are set in the field masks the only error masks that will be updated will
  be those corresponding to the values set within the field masks.

  If field mask is IPFLTR_MASK_IP6_NONE, errmask is set to
  IPFLTR_MASK_IP6_ALL

  If a bit mask is specified, which is outside the supported range, errmask
  is updated to those bits
===========================================================================*/
static boolean ds_ipfltri_validate_ipv6_param
(
  ds_ipfltr_client_id_enum_type    client_id,
  ip_filter_type                       * fltr_ptr,
  ps_ip_protocol_enum_type             * next_hdr_prot
)
{
  boolean  is_fltr_valid = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  fltr_ptr->ip_hdr.v6.err_mask = 0;

  if (fltr_ptr->ip_hdr.v6.field_mask == IPFLTR_MASK_IP6_NONE)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"ds_ipfltri_validate_ipv6_param(): "
                    "Invalid IP_V6 field mask: 0x%x",
                    fltr_ptr->ip_hdr.v6.field_mask);
    fltr_ptr->ip_hdr.v6.err_mask = IPFLTR_MASK_IP6_ALL;
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    If a bit mask is specified, which is outside the supported range, flag
    error and continue with validation so that other parameters are validated
  -------------------------------------------------------------------------*/
  if (fltr_ptr->ip_hdr.v6.field_mask & ~IPFLTR_MASK_IP6_ALL)
  {
    fltr_ptr->ip_hdr.v6.err_mask =
      (fltr_ptr->ip_hdr.v6.field_mask & ~IPFLTR_MASK_IP6_ALL);
    is_fltr_valid = FALSE;
  }

  /*-------------------------------------------------------------------------
    Fail if either of source or destination address is V4 mapped V6
  -------------------------------------------------------------------------*/
  if ((fltr_ptr->ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_SRC_ADDR) &&
      PS_IN6_IS_ADDR_V4MAPPED(&fltr_ptr->ip_hdr.v6.src.addr))
  {
    fltr_ptr->ip_hdr.v6.err_mask |= IPFLTR_MASK_IP6_SRC_ADDR;
    is_fltr_valid = FALSE;
  }

  if ((fltr_ptr->ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_DST_ADDR) &&
      PS_IN6_IS_ADDR_V4MAPPED(&fltr_ptr->ip_hdr.v6.dst.addr))
  {
    fltr_ptr->ip_hdr.v6.err_mask |= IPFLTR_MASK_IP6_DST_ADDR;
    is_fltr_valid = FALSE;
  }

  /*-----------------------------------------------------------------------
    If Source address is used in a Tx filter or if Destination address is
    used in a Rx filter, make sure that only a single address value is
    specified. Filters are installed per framework IP instance
  -----------------------------------------------------------------------*/
  
  if ((fltr_ptr->ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_SRC_ADDR) &&
      ! (fltr_ptr->ip_hdr.v6.src.prefix_len > 0  && 
         fltr_ptr->ip_hdr.v6.src.prefix_len <= 128))
  {
    fltr_ptr->ip_hdr.v6.err_mask |= IPFLTR_MASK_IP6_SRC_ADDR;
    is_fltr_valid = FALSE;
  }

  if ((fltr_ptr->ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_DST_ADDR) &&
      ! (fltr_ptr->ip_hdr.v6.dst.prefix_len > 0  && 
         fltr_ptr->ip_hdr.v6.dst.prefix_len <= 128))
  {
    fltr_ptr->ip_hdr.v6.err_mask |= IPFLTR_MASK_IP6_DST_ADDR;
    is_fltr_valid = FALSE;
  }


  /*-------------------------------------------------------------------------
    Make sure that only one of TCP, UDP, or ICMP is specified as higher layer
    protocol in a filter
  -------------------------------------------------------------------------*/
  if (fltr_ptr->ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_NEXT_HDR_PROT)
  {
    if (fltr_ptr->ip_hdr.v6.next_hdr_prot == PS_IPPROTO_TCP ||
        fltr_ptr->ip_hdr.v6.next_hdr_prot == PS_IPPROTO_UDP ||
        fltr_ptr->ip_hdr.v6.next_hdr_prot == PS_IPPROTO_ICMP ||
        fltr_ptr->ip_hdr.v6.next_hdr_prot == PS_IPPROTO_ICMP6 ||
        fltr_ptr->ip_hdr.v6.next_hdr_prot == PS_IPPROTO_ESP ||
        fltr_ptr->ip_hdr.v6.next_hdr_prot == PS_IPPROTO_AH ||
        fltr_ptr->ip_hdr.v6.next_hdr_prot == PS_IPPROTO_TCP_UDP)
    {
      *next_hdr_prot =
        (ps_ip_protocol_enum_type) fltr_ptr->ip_hdr.v6.next_hdr_prot;
    }
    else
    {
      fltr_ptr->ip_hdr.v6.err_mask |= IPFLTR_MASK_IP6_NEXT_HDR_PROT;
      is_fltr_valid = FALSE;
    }
  }
  else if (client_id == FLTR_CLIENT_SOCKETS)
  {
    fltr_ptr->ip_hdr.v6.err_mask |= IPFLTR_MASK_IP6_NEXT_HDR_PROT;
    is_fltr_valid = FALSE;
  }

  if ((fltr_ptr->ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_TRAFFIC_CLASS) &&
      fltr_ptr->ip_hdr.v6.trf_cls.mask == 0)
  {
    fltr_ptr->ip_hdr.v6.err_mask |= IPFLTR_MASK_IP6_TRAFFIC_CLASS;
    is_fltr_valid = FALSE;
  }

  /*-------------------------------------------------------------------------
    IPV6 flow label already specifies flow ID. No need to further classify
    this packet
  -------------------------------------------------------------------------*/
  if ((fltr_ptr->ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_FLOW_LABEL) &&
      (*next_hdr_prot != PS_NO_NEXT_HDR))
  {
    fltr_ptr->ip_hdr.v6.err_mask |= (IPFLTR_MASK_IP6_NEXT_HDR_PROT |
                                     IPFLTR_MASK_IP6_FLOW_LABEL);
    is_fltr_valid = FALSE;
  }

  if (is_fltr_valid == FALSE)
  {
    DS_MSG2(MSG_LEGACY_HIGH,"ds_ipfltri_validate_ipv6_param(): "
                    "IP_FLTR V6 field_mask = 0x%x, IP_FLTR V6 err_mask = 0x%x",
                    fltr_ptr->ip_hdr.v6.field_mask,
                    fltr_ptr->ip_hdr.v6.err_mask);
  }

  return is_fltr_valid;

} /* ds_ipfltri_validate_ipv6_param() */

/*===========================================================================
FUNCTION DS_IPFLTRI_VALIDATE_TCP_PARAM()

DESCRIPTION
  Validates TCP parameters of a filter

PARAMETERS
  client_id : Filtering client id
  fltr_ptr  : ptr to a filter

RETURN VALUE
  TRUE  : if parameters are valid
  FALSE : otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  err_mask is updated with appropriate error codes for values and enums
  which failed validation. Because variables are only validated if the enums
  are set in the field masks the only error masks that will be updated will
  be those corresponding to the values set within the field masks.

  If a bit mask is specified, which is outside the supported range, errmask
  is updated to those bits
===========================================================================*/
static boolean ds_ipfltri_validate_tcp_param
(
  ds_ipfltr_client_id_enum_type    client_id,
  ip_filter_type                       * fltr_ptr
)
{
  boolean  is_fltr_valid     = TRUE;
  boolean  is_port_specified = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  fltr_ptr->next_prot_hdr.tcp.err_mask = 0;

  /*-------------------------------------------------------------------------
    If a bit mask is specified, which is outside the supported range, flag
    error and continue with validation so that other parameters are validated
  -------------------------------------------------------------------------*/
  if (fltr_ptr->next_prot_hdr.tcp.field_mask & ~IPFLTR_MASK_TCP_ALL)
  {
    fltr_ptr->next_prot_hdr.tcp.err_mask =
      (fltr_ptr->next_prot_hdr.tcp.field_mask & ~IPFLTR_MASK_TCP_ALL);
    is_fltr_valid = FALSE;
  }

  /*-------------------------------------------------------------------------
    TCP ports must be within the range (0, 65535]
  -------------------------------------------------------------------------*/
  if (fltr_ptr->next_prot_hdr.tcp.field_mask & IPFLTR_MASK_TCP_SRC_PORT)
  {
    is_port_specified = TRUE;

    if (fltr_ptr->next_prot_hdr.tcp.src.port == 0 ||
        !IS_PORT_RANGE_VALID(fltr_ptr->next_prot_hdr.tcp.src.port,
                             fltr_ptr->next_prot_hdr.tcp.src.range))
    {
      fltr_ptr->next_prot_hdr.tcp.err_mask |= IPFLTR_MASK_TCP_SRC_PORT;
      is_fltr_valid = FALSE;
    }
  }

  if (fltr_ptr->next_prot_hdr.tcp.field_mask & IPFLTR_MASK_TCP_DST_PORT)
  {
    is_port_specified = TRUE;

    if (fltr_ptr->next_prot_hdr.tcp.dst.port == 0 ||
        !IS_PORT_RANGE_VALID(fltr_ptr->next_prot_hdr.tcp.dst.port,
                             fltr_ptr->next_prot_hdr.tcp.dst.range))
    {
      fltr_ptr->next_prot_hdr.tcp.err_mask |= IPFLTR_MASK_TCP_DST_PORT;
      is_fltr_valid = FALSE;
    }
  }

  if ( FALSE == is_port_specified && 
       FLTR_CLIENT_SOCKETS == client_id )
  {
    fltr_ptr->next_prot_hdr.tcp.err_mask |= IPFLTR_MASK_TCP_SRC_PORT;
    fltr_ptr->next_prot_hdr.tcp.err_mask |= IPFLTR_MASK_TCP_DST_PORT;
    is_fltr_valid = FALSE;
  }

  if (is_fltr_valid == FALSE)
  {
    DS_MSG3(MSG_LEGACY_HIGH,"ds_ipfltri_validate_tcp_param(): "
                    "IP_FLTR TCP field_mask = 0x%x on IP ver: %d, "
                    "IP_FLTR TCP err_mask = 0x%x",
                    fltr_ptr->next_prot_hdr.tcp.field_mask,
                    fltr_ptr->ip_vsn,
                    fltr_ptr->next_prot_hdr.tcp.err_mask);
  }

  return is_fltr_valid;

} /* ds_ipfltri_validate_tcp_param() */

/*===========================================================================
FUNCTION DS_IPFLTRI_VALIDATE_UDP_PARAM()

DESCRIPTION
  Validates UDP parameters of a filter

PARAMETERS
  client_id : Filtering client id
  fltr_ptr  : ptr to a filter

RETURN VALUE
  TRUE  : if parameters are valid
  FALSE : otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  err_mask is updated with appropriate error codes for values and enums
  which failed validation. Because variables are only validated if the enums
  are set in the field masks the only error masks that will be updated will
  be those corresponding to the values set within the field masks.

  If a bit mask is specified, which is outside the supported range, errmask
  is updated to those bits
===========================================================================*/
static boolean ds_ipfltri_validate_udp_param
(
  ds_ipfltr_client_id_enum_type    client_id,
  ip_filter_type                       * fltr_ptr
)
{
  boolean  is_fltr_valid     = TRUE;
  boolean  is_port_specified = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  fltr_ptr->next_prot_hdr.udp.err_mask = 0;

  /*-------------------------------------------------------------------------
    If a bit mask is specified, which is outside the supported range, flag
    error and continue with validation so that other parameters are validated
  -------------------------------------------------------------------------*/
  if (fltr_ptr->next_prot_hdr.udp.field_mask & ~IPFLTR_MASK_UDP_ALL)
  {
    fltr_ptr->next_prot_hdr.udp.err_mask =
      (fltr_ptr->next_prot_hdr.udp.field_mask & ~IPFLTR_MASK_UDP_ALL);
    is_fltr_valid = FALSE;
  }

  /*-------------------------------------------------------------------------
    UDP ports must be within the range (0, 65535]
  -------------------------------------------------------------------------*/
  if (fltr_ptr->next_prot_hdr.udp.field_mask & IPFLTR_MASK_UDP_SRC_PORT)
  {
    is_port_specified = TRUE;

    if (fltr_ptr->next_prot_hdr.udp.src.port == 0 ||
        !IS_PORT_RANGE_VALID(fltr_ptr->next_prot_hdr.udp.src.port,
                             fltr_ptr->next_prot_hdr.udp.src.range))
    {
      fltr_ptr->next_prot_hdr.udp.err_mask |= IPFLTR_MASK_UDP_SRC_PORT;
      is_fltr_valid = FALSE;
    }
  }

  if (fltr_ptr->next_prot_hdr.udp.field_mask & IPFLTR_MASK_UDP_DST_PORT)
  {
    is_port_specified = TRUE;

    if (fltr_ptr->next_prot_hdr.udp.dst.port == 0 ||
        !IS_PORT_RANGE_VALID(fltr_ptr->next_prot_hdr.udp.dst.port,
                             fltr_ptr->next_prot_hdr.udp.dst.range))
    {
      fltr_ptr->next_prot_hdr.udp.err_mask |= IPFLTR_MASK_UDP_DST_PORT;
      is_fltr_valid = FALSE;
    }
  }

  if ( FALSE == is_port_specified && 
       FLTR_CLIENT_SOCKETS == client_id )
  {
    fltr_ptr->next_prot_hdr.udp.err_mask |= IPFLTR_MASK_UDP_SRC_PORT;
    fltr_ptr->next_prot_hdr.udp.err_mask |= IPFLTR_MASK_UDP_DST_PORT;
    is_fltr_valid = FALSE;
  }

  if (is_fltr_valid == FALSE)
  {
    DS_MSG3(MSG_LEGACY_HIGH,"ds_ipfltri_validate_udp_param(): "
                    "IP_FLTR UDP field_mask = 0x%x on IP ver: %d, "
                    "IP_FLTR UDP err_mask = 0x%x",
                    fltr_ptr->next_prot_hdr.udp.field_mask,
                    fltr_ptr->ip_vsn,
                    fltr_ptr->next_prot_hdr.udp.err_mask);
  }

  return is_fltr_valid;

} /* ds_ipfltri_validate_udp_param() */

/*===========================================================================
FUNCTION DS_IPFLTRI_VALIDATE_ICMP_PARAM()

DESCRIPTION
  Validates the ICMP parameters of a filter

PARAMETERS
  client_id : Filtering client id
  fltr_ptr  : ptr to a filter

RETURN VALUE
   TRUE  : if parameters are valid
   FALSE : otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  err_mask is updated with the appropriate error codes for values and enums
  which failed validation.

  If a bit mask, which is outside the supported range, is specified, errmask
  is updated to those bits
===========================================================================*/
static boolean ds_ipfltri_validate_icmp_param
(
  ds_ipfltr_client_id_enum_type    client_id,
  ip_filter_type                 * fltr_ptr
)
{
  boolean  is_fltr_valid = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  fltr_ptr->next_prot_hdr.icmp.err_mask = 0;

  /*-------------------------------------------------------------------------
    If a bit mask, which is outside the supported range, is specified flag
    error continue with validation so that other parameters are validated
  -------------------------------------------------------------------------*/
  if (fltr_ptr->next_prot_hdr.icmp.field_mask & ~IPFLTR_MASK_ICMP_ALL)
  {
    fltr_ptr->next_prot_hdr.icmp.err_mask =
      (fltr_ptr->next_prot_hdr.icmp.field_mask & ~IPFLTR_MASK_ICMP_ALL);
    is_fltr_valid = FALSE;
  }

  if (is_fltr_valid == FALSE)
  {
    DS_MSG3(MSG_LEGACY_HIGH,"ds_ipfltri_validate_icmp_param(): "
                    "IP_FLTR ICMP field_mask = 0x%x on IP ver: %d, "
                    "IP_FLTR ICMP err_mask = 0x%x",
                    fltr_ptr->next_prot_hdr.icmp.field_mask,
                    fltr_ptr->ip_vsn,
                    fltr_ptr->next_prot_hdr.icmp.err_mask);
  }

  return is_fltr_valid;

} /* ds_ipfltri_validate_icmp_param() */


/*===========================================================================
FUNCTION DS_IPFLTRI_VALIDATE_TCP_UDP_PARAM()

DESCRIPTION
  Validates Transport parameters of a filter when the next header is not
  specified.

PARAMETERS
  client_id : Filtering client id
  fltr_ptr  : ptr to a filter

RETURN VALUE
  TRUE  : if parameters are valid
  FALSE : otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  err_mask is updated with appropriate error codes for values and enums
  which failed validation. Because variables are only validated if the enums
  are set in the field masks the only error masks that will be updated will
  be those corresponding to the values set within the field masks.

  If a bit mask is specified, which is outside the supported range, errmask
  is updated to those bits
===========================================================================*/
static boolean ds_ipfltri_validate_tcp_udp_param
(
  ds_ipfltr_client_id_enum_type    client_id,
  ip_filter_type                       * fltr_ptr
)
{
  boolean  is_fltr_valid     = TRUE;
  boolean  is_port_specified = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  fltr_ptr->next_prot_hdr.tcp_udp_port_range.err_mask = 0;

  /*-------------------------------------------------------------------------
    If a bit mask is specified, which is outside the supported range, flag
    error and continue with validation so that other parameters are validated
  -------------------------------------------------------------------------*/
  if (fltr_ptr->next_prot_hdr.tcp_udp_port_range.field_mask &
        ~IPFLTR_MASK_TCP_UDP_ALL)
  {
    fltr_ptr->next_prot_hdr.tcp_udp_port_range.err_mask =
      (fltr_ptr->next_prot_hdr.tcp_udp_port_range.field_mask &
       ~IPFLTR_MASK_TCP_UDP_ALL);
    is_fltr_valid = FALSE;
  }

  /*-------------------------------------------------------------------------
    All ports must be within the range (0, 65535]
  -------------------------------------------------------------------------*/
  if (fltr_ptr->next_prot_hdr.tcp_udp_port_range.field_mask &
        IPFLTR_MASK_TCP_UDP_SRC_PORT)
  {
    is_port_specified = TRUE;

    if (fltr_ptr->next_prot_hdr.tcp_udp_port_range.src.port == 0 ||
        !IS_PORT_RANGE_VALID(
           fltr_ptr->next_prot_hdr.tcp_udp_port_range.src.port,
           fltr_ptr->next_prot_hdr.tcp_udp_port_range.src.range))
    {
      fltr_ptr->next_prot_hdr.tcp_udp_port_range.err_mask |=
        IPFLTR_MASK_TCP_UDP_SRC_PORT;
      is_fltr_valid = FALSE;
    }
  }

  if (fltr_ptr->next_prot_hdr.tcp_udp_port_range.field_mask &
        IPFLTR_MASK_TCP_UDP_DST_PORT) 
  {  
    is_port_specified = TRUE;

    if (fltr_ptr->next_prot_hdr.tcp_udp_port_range.dst.port == 0 ||
        !IS_PORT_RANGE_VALID(
           fltr_ptr->next_prot_hdr.tcp_udp_port_range.dst.port,
           fltr_ptr->next_prot_hdr.tcp_udp_port_range.dst.range))
    {
      fltr_ptr->next_prot_hdr.tcp_udp_port_range.err_mask |=
        IPFLTR_MASK_TCP_UDP_DST_PORT;
      is_fltr_valid = FALSE;
    }
  }

  if ( FALSE == is_port_specified && 
       FLTR_CLIENT_SOCKETS == client_id )
  {
    fltr_ptr->next_prot_hdr.tcp_udp_port_range.err_mask |=
      IPFLTR_MASK_TCP_UDP_SRC_PORT;
    fltr_ptr->next_prot_hdr.tcp_udp_port_range.err_mask |=
      IPFLTR_MASK_TCP_UDP_DST_PORT;
    is_fltr_valid = FALSE;
  }

  if (is_fltr_valid == FALSE)
  {
    DS_MSG3(MSG_LEGACY_HIGH,"ds_ipfltri_validate_tcp_udp_param(): "
                    "IP_FLTR TRANSPORT PROTOCOL field_mask = 0x%x on IP ver: %d, "
                    "IP_FLTR TRANSPORT PROTOCOL err_mask = 0x%x",
                    fltr_ptr->next_prot_hdr.tcp_udp_port_range.field_mask,
                    fltr_ptr->ip_vsn,
                    fltr_ptr->next_prot_hdr.tcp_udp_port_range.err_mask);
  }

  return is_fltr_valid;

} /* ds_ipfltri_validate_tcp_udp_param() */


/*===========================================================================
FUNCTION DS_IPFLTRI_PROCESS_DEFAULT()

DESCRIPTION
  This function executes a specified filter on a specified IP pkt information
  block using the default set of rules.

DEPENDENCIES
  - Filter and pakcet must have the same IP version.
  - If a field in the next hdr is specified as a filter parameter,
    next_hdr_prot field in the IP hdr must be set. This means that ip hdr
    field mask will always be non null since filter should have at least
    one parameter specified.

RETURN VALUE
  TRUE for a successful match
  FALSE for no match

SIDE EFFECTS
  None.

===========================================================================*/
static boolean ds_ipfltri_process_default
(
  ip_filter_type       *fi_ptr,          /* ptr to filter to process       */
  ip_pkt_info_type     *info_ptr         /* ptr to info to apply filter on */
)
{
  uint32 tmp_mask;
  uint8  next_hdr_prot = PS_NO_NEXT_HDR;
  uint8 tmp_prot;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == fi_ptr || NULL == info_ptr)
  {
    DS_MSG2(MSG_LEGACY_ERROR,"ds_ipfltri_process_default(): "
                    "Bad args: fi_ptr 0x%p, info_ptr 0x%p", fi_ptr, info_ptr);
    return FALSE;
  }

  if (fi_ptr->ip_vsn != (ip_addr_enum_type)info_ptr->ip_vsn)
  {
    DS_MSG2(MSG_LEGACY_ERROR,"ds_ipfltri_process_default(): "
                    "Filter address family %d differs from pkt address family %d",
                    fi_ptr->ip_vsn, info_ptr->ip_vsn);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Algorithm:
    - Based on the IP version process corresponding IP hdr parameters.
    - If parameters for higher level protocol are specified, v4 protocol
    field or v6 next header field must be set in the filter.  Extract next
    header protocol value and amke sure it matches with that of the packet.
    - Next process the next header protocol parameters

    If any parameter specified in the filter fails a match, fail the filter
    execution.  If all specified filter parametrs match the pkt, filter
    is passed.
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    Note the while loop over a tmp_mask for IP hdr processing as an
    optimization.  More likely parameters should be checked first inside
    the while loop.
  -------------------------------------------------------------------------*/

  switch (fi_ptr->ip_vsn)
  {
    case IP_V4:
    {
      tmp_mask = fi_ptr->ip_hdr.v4.field_mask;

      while (tmp_mask)
      {
        if (tmp_mask & IPFLTR_MASK_IP4_SRC_ADDR)
        {
          if ((fi_ptr->ip_hdr.v4.src.addr.ps_s_addr &
              fi_ptr->ip_hdr.v4.src.subnet_mask.ps_s_addr) !=
             (info_ptr->ip_hdr.v4.source.ps_s_addr &
              fi_ptr->ip_hdr.v4.src.subnet_mask.ps_s_addr))
          {
            return FALSE;
          }

          tmp_mask &= ~IPFLTR_MASK_IP4_SRC_ADDR;
          continue;
        }

        if (tmp_mask & IPFLTR_MASK_IP4_DST_ADDR)
        {
          if ((fi_ptr->ip_hdr.v4.dst.addr.ps_s_addr &
              fi_ptr->ip_hdr.v4.dst.subnet_mask.ps_s_addr) !=
             (info_ptr->ip_hdr.v4.dest.ps_s_addr &
              fi_ptr->ip_hdr.v4.dst.subnet_mask.ps_s_addr))
          {
            return FALSE;
          }

          tmp_mask &= ~IPFLTR_MASK_IP4_DST_ADDR;
          continue;
        }

        if (tmp_mask & IPFLTR_MASK_IP4_NEXT_HDR_PROT)
        {
          if (fi_ptr->ip_hdr.v4.next_hdr_prot == PS_IPPROTO_TCP_UDP)
          {
            if (info_ptr->ip_hdr.v4.protocol != PS_IPPROTO_TCP &&
                info_ptr->ip_hdr.v4.protocol != PS_IPPROTO_UDP)
            {
              return FALSE;
            }
          }
          else if (fi_ptr->ip_hdr.v4.next_hdr_prot !=
                     info_ptr->ip_hdr.v4.protocol)
          {
            return FALSE;
          }

          next_hdr_prot = fi_ptr->ip_hdr.v4.next_hdr_prot;

          tmp_mask &= ~IPFLTR_MASK_IP4_NEXT_HDR_PROT;
          continue;
        }

        if (tmp_mask & IPFLTR_MASK_IP4_TOS)
        {
          if ((fi_ptr->ip_hdr.v4.tos.val & fi_ptr->ip_hdr.v4.tos.mask) !=
             (info_ptr->ip_hdr.v4.tos & fi_ptr->ip_hdr.v4.tos.mask))
          {
            return FALSE;
          }

          tmp_mask &= ~IPFLTR_MASK_IP4_TOS;
          continue;
        }

        if (0 != tmp_mask)
        {
          DS_MSG1(MSG_LEGACY_ERROR,"ds_ipfltri_process_default(): "
                          "Unknown v4 filter param in mask 0x%x", tmp_mask);
        }
      }
    }
    break;

#ifdef FEATURE_DATA_PS_IPV6
    case IP_V6:
    {
      tmp_mask = fi_ptr->ip_hdr.v6.field_mask;
      while (tmp_mask)
      {
        if (tmp_mask & IPFLTR_MASK_IP6_SRC_ADDR)
        {
          if (FALSE == IN6_ARE_PREFIX_EQUAL
                       (
                         &fi_ptr->ip_hdr.v6.src.addr,
                         &info_ptr->ip_hdr.v6.hdr_body.base_hdr.src_addr,
                         fi_ptr->ip_hdr.v6.src.prefix_len)
                       )
          {
            return FALSE;
          }

          tmp_mask &= ~IPFLTR_MASK_IP6_SRC_ADDR;
          continue;
        }

        if (tmp_mask & IPFLTR_MASK_IP6_DST_ADDR)
        {
          if (FALSE == IN6_ARE_PREFIX_EQUAL
                       (
                         &fi_ptr->ip_hdr.v6.dst.addr,
                         &info_ptr->ip_hdr.v6.hdr_body.base_hdr.dst_addr,
                         fi_ptr->ip_hdr.v6.dst.prefix_len)
                       )
          {
            return FALSE;
          }

          tmp_mask &= ~IPFLTR_MASK_IP6_DST_ADDR;
          continue;
        }

        /*-------------------------------------------------------------------
          Next hdr for IPv6 is transport protocol type
        -------------------------------------------------------------------*/
        if (tmp_mask & IPFLTR_MASK_IP6_NEXT_HDR_PROT)
        {
          if (fi_ptr->ip_hdr.v6.next_hdr_prot == PS_IPPROTO_TCP_UDP)
          {
            if (info_ptr->ip_hdr.v6.hdr_type != PS_IPPROTO_TCP &&
                info_ptr->ip_hdr.v6.hdr_type != PS_IPPROTO_UDP)
            {
              return FALSE;
            }
          }
          else if (fi_ptr->ip_hdr.v6.next_hdr_prot !=
                     info_ptr->ip_hdr.v6.hdr_type)
          {
            return FALSE;
          }

          next_hdr_prot = fi_ptr->ip_hdr.v6.next_hdr_prot;
          tmp_mask &= ~IPFLTR_MASK_IP6_NEXT_HDR_PROT;
          continue;
        }

        if (tmp_mask & IPFLTR_MASK_IP6_TRAFFIC_CLASS)
        {
          if ((fi_ptr->ip_hdr.v6.trf_cls.val &
                 fi_ptr->ip_hdr.v6.trf_cls.mask) !=
              (info_ptr->ip_hdr.v6.hdr_body.base_hdr.trf_cls &
                 fi_ptr->ip_hdr.v6.trf_cls.mask))
          {
            return FALSE;
          }

          tmp_mask &= ~IPFLTR_MASK_IP6_TRAFFIC_CLASS;
          continue;
        }

        if (tmp_mask & IPFLTR_MASK_IP6_FLOW_LABEL)
        {
          if (fi_ptr->ip_hdr.v6.flow_label !=
               info_ptr->ip_hdr.v6.hdr_body.base_hdr.flow_cls)
          {
            return FALSE;
          }

          tmp_mask &= ~IPFLTR_MASK_IP6_FLOW_LABEL;
          continue;
        }

        if (0 != tmp_mask)
        {
          DS_MSG1(MSG_LEGACY_ERROR,"ds_ipfltri_process_default(): "
                          "Unknown v6 filter param in mask 0x%x", tmp_mask);
        }
      }
    }
    break;
#endif /* FEATURE_DATA_PS_IPV6 */

    default:
      /* unsupported IP version */
      DS_ASSERT(0);
      return FALSE;
  }

  if (next_hdr_prot == PS_NO_NEXT_HDR)
  {
    return TRUE;
  }

  /*-------------------------------------------------------------------------
    Optimization done for IP hdrs above may also be used for protocol hdr
    processing when more than 2 parameters can be specified for a protocol.
    For 2 or less parameters the scheme is less optimal and hence avoided.
  -------------------------------------------------------------------------*/
  switch (next_hdr_prot)
  {
    case PS_IPPROTO_TCP:
    {
      if (0 == (tmp_mask = fi_ptr->next_prot_hdr.tcp.field_mask))
      {
        break;
      }

      if ((tmp_mask & IPFLTR_MASK_TCP_SRC_PORT) &&

         ((ps_ntohs(fi_ptr->next_prot_hdr.tcp.src.port)) >
            (ps_ntohs(info_ptr->ptcl_info.tcp.src_port)) ||

          (ps_ntohs(fi_ptr->next_prot_hdr.tcp.src.port)) +
            fi_ptr->next_prot_hdr.tcp.src.range <
            (ps_ntohs(info_ptr->ptcl_info.tcp.src_port))))
      {
        return FALSE;
      }

      if ((tmp_mask & IPFLTR_MASK_TCP_DST_PORT) &&

         ((ps_ntohs(fi_ptr->next_prot_hdr.tcp.dst.port)) >
            (ps_ntohs(info_ptr->ptcl_info.tcp.dst_port)) ||

          (ps_ntohs(fi_ptr->next_prot_hdr.tcp.dst.port)) +
            fi_ptr->next_prot_hdr.tcp.dst.range <
            (ps_ntohs(info_ptr->ptcl_info.tcp.dst_port))))
      {
        return FALSE;
      }
    }

    break;

    case PS_IPPROTO_UDP:
    {
      if (0 == (tmp_mask = fi_ptr->next_prot_hdr.udp.field_mask))
      {
        break;
      }

      if ((tmp_mask & IPFLTR_MASK_UDP_SRC_PORT) &&

         ((ps_ntohs(fi_ptr->next_prot_hdr.udp.src.port)) >
            (ps_ntohs(info_ptr->ptcl_info.udp.src_port)) ||

          (ps_ntohs(fi_ptr->next_prot_hdr.udp.src.port)) +
            fi_ptr->next_prot_hdr.udp.src.range <
            (ps_ntohs(info_ptr->ptcl_info.udp.src_port))))
      {
        return FALSE;
      }

      if ((tmp_mask & IPFLTR_MASK_UDP_DST_PORT) &&

         ((ps_ntohs(fi_ptr->next_prot_hdr.udp.dst.port)) >
            (ps_ntohs(info_ptr->ptcl_info.udp.dst_port)) ||

          (ps_ntohs(fi_ptr->next_prot_hdr.udp.dst.port)) +
            fi_ptr->next_prot_hdr.udp.dst.range <
            (ps_ntohs(info_ptr->ptcl_info.udp.dst_port))))
      {
        return FALSE;
      }
    }

    break;

    case PS_IPPROTO_ICMP:
    case PS_IPPROTO_ICMP6:
    {
      if (0 == (tmp_mask = fi_ptr->next_prot_hdr.icmp.field_mask))
      {
        break;
      }

      if (tmp_mask & IPFLTR_MASK_ICMP_MSG_TYPE &&
         fi_ptr->next_prot_hdr.icmp.type != info_ptr->ptcl_info.icmp.type)
      {
        return FALSE;
      }

      if (tmp_mask & IPFLTR_MASK_ICMP_MSG_CODE &&
         fi_ptr->next_prot_hdr.icmp.code != info_ptr->ptcl_info.icmp.code)
      {
        return FALSE;
      }
      
      if (tmp_mask & IPFLTR_MASK_ICMP_MSG_ID &&
         fi_ptr->next_prot_hdr.icmp.id != info_ptr->ptcl_info.icmp.id)
      {
        return FALSE;
      }
    }

    break;

    case PS_IPPROTO_ESP:
    {
      if (IPFLTR_MASK_ESP_NONE == fi_ptr->next_prot_hdr.esp.field_mask)
      {
        break;
      }

      tmp_mask = fi_ptr->next_prot_hdr.esp.field_mask;

      if (tmp_mask & IPFLTR_MASK_ESP_SPI &&
          fi_ptr->next_prot_hdr.esp.spi != info_ptr->esp_hdr.spi)
      {
        return FALSE;
      }
    }

    break;

    case PS_IPPROTO_AH:
    {
      if (IPFLTR_MASK_AH_NONE == fi_ptr->next_prot_hdr.ah.field_mask)
      {
        break;
      }

      tmp_mask = fi_ptr->next_prot_hdr.ah.field_mask;

      if (tmp_mask & IPFLTR_MASK_AH_SPI &&
          fi_ptr->next_prot_hdr.ah.spi != info_ptr->ah_hdr.spi)
      {
        return FALSE;
      }
    }

    break;

    case PS_IPPROTO_TCP_UDP:
    {
      if (0 == (tmp_mask = fi_ptr->next_prot_hdr.tcp_udp_port_range.field_mask))
      {
        break;
      }

      if (fi_ptr->ip_vsn == IP_V4)
      {
        tmp_prot = info_ptr->ip_hdr.v4.protocol;
      }
      else
      {
        tmp_prot = info_ptr->ip_hdr.v6.hdr_type;
      }

      switch (tmp_prot)
      {
        case PS_IPPROTO_TCP:
          if ((tmp_mask & IPFLTR_MASK_TCP_UDP_SRC_PORT) &&

             ((ps_ntohs(fi_ptr->next_prot_hdr.tcp_udp_port_range.src.port)) >
                (ps_ntohs(info_ptr->ptcl_info.tcp.src_port)) ||

              (ps_ntohs(fi_ptr->next_prot_hdr.tcp_udp_port_range.src.port)) +
                fi_ptr->next_prot_hdr.tcp_udp_port_range.src.range <
                (ps_ntohs(info_ptr->ptcl_info.tcp.src_port))))
          {
            return FALSE;
          }

          if ((tmp_mask & IPFLTR_MASK_TCP_UDP_DST_PORT) &&

             ((ps_ntohs(fi_ptr->next_prot_hdr.tcp_udp_port_range.dst.port)) >
              (ps_ntohs(info_ptr->ptcl_info.tcp.dst_port)) ||

              (ps_ntohs(fi_ptr->next_prot_hdr.tcp_udp_port_range.dst.port)) +
                fi_ptr->next_prot_hdr.tcp_udp_port_range.dst.range <
                (ps_ntohs(info_ptr->ptcl_info.tcp.dst_port))))
          {
            return FALSE;
          }
        break;

        case PS_IPPROTO_UDP:
          if ((tmp_mask & IPFLTR_MASK_TCP_UDP_SRC_PORT) &&

             ((ps_ntohs(fi_ptr->next_prot_hdr.tcp_udp_port_range.src.port)) >
                (ps_ntohs(info_ptr->ptcl_info.udp.src_port)) ||

              (ps_ntohs(fi_ptr->next_prot_hdr.tcp_udp_port_range.src.port)) +
                fi_ptr->next_prot_hdr.tcp_udp_port_range.src.range <
                (ps_ntohs(info_ptr->ptcl_info.udp.src_port))))
          {
            return FALSE;
          }

          if ((tmp_mask & IPFLTR_MASK_TCP_UDP_DST_PORT) &&

             ((ps_ntohs(fi_ptr->next_prot_hdr.tcp_udp_port_range.dst.port)) >
                (ps_ntohs(info_ptr->ptcl_info.udp.dst_port)) ||

              (ps_ntohs(fi_ptr->next_prot_hdr.tcp_udp_port_range.dst.port)) +
                fi_ptr->next_prot_hdr.tcp_udp_port_range.dst.range <
                (ps_ntohs(info_ptr->ptcl_info.udp.dst_port))))
          {
            return FALSE;
          }
        break;

        default:
          break;
      }
    }
  break;

  default:
    /* unsupported protocol */
    DS_ASSERT(0);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    If we reach here, all the specified parameters matached
  -------------------------------------------------------------------------*/
  return TRUE;
} /* ds_ipfltri_process_default() */


/*===========================================================================
FUNCTION DS_IPFLTRI_ADD_OPT_FILTERS()

DESCRIPTION
  This function adds filters for optimized data path.

PARAMETERS
  fw_inst_ptr     : instance to add filters to
  ip_type..........: ip version
  filter_group    : group
  fltr_arr           : Filtering specification
  num_filters        : Number of filters to be added
  fi_handle          : Handle associated with this filter

RETURN VALUE
  TRUE if success, FALSE on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static int32 ds_ipfltri_add_opt_filter
(
  ds_fwk_s_type*                   fw_inst_ptr,
  ip_addr_enum_type                ip_type,
  ds_filter_group_enum_type        filter_group,
  ip_filter_type                 * fltr_arr,
  uint32                           num_filters,
  ds_ipfltr_handle_type            fi_handle
)
{
  ds_dl_opt_filter_spec_type     * opt_fltr_ptr;
  int32                            opt_fltr_arr_size;
  uint32                           max_fltr;
  uint32                           fltr_index = 0;
  uint8                            opt_fltr_count;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get address of opt filter array
      1. If iface is NULL, address points to global opt filter array
      2. Else, address points to opt filter array of the base iface of input
         iface. It must be base iface as DL opt handler runs on physical iface
         and not logical iface
  -------------------------------------------------------------------------*/
  if (fw_inst_ptr == NULL)
  {
    if (filter_group == DS_RM_GROUP)
    {
      opt_fltr_ptr = ds_global_ul_opt_fltr_arr;
    }
    else
    {    
      opt_fltr_ptr = ds_global_dl_opt_fltr_arr;
    }
    max_fltr     = MAX_GLOBAL_OPT_FILTER_ENTRIES;
  }
  else
  {
    opt_fltr_ptr = fw_inst_ptr->dl_opt_fltr_arr;
    max_fltr     = DS_MAX_OPT_FILTER_ENTRIES;
  }

  /*-------------------------------------------------------------------------
    If opt fltr array is not already allocated, allocate it
  -------------------------------------------------------------------------*/
  if (opt_fltr_ptr == NULL)
  {
    opt_fltr_arr_size = sizeof(ds_dl_opt_filter_spec_type) * max_fltr;

    DS_SYSTEM_HEAP_MEM_ALLOC(opt_fltr_ptr, opt_fltr_arr_size, 
                             ds_dl_opt_filter_spec_type*);
    if (opt_fltr_ptr == NULL)
    {
      return -1;
    }

    memset(opt_fltr_ptr, 0, opt_fltr_arr_size);

    if (NULL == fw_inst_ptr)
    {
      if (filter_group == DS_RM_GROUP)
      {
        ds_global_ul_opt_fltr_arr = opt_fltr_ptr;
      }
      else
      {      
      ds_global_dl_opt_fltr_arr = opt_fltr_ptr;
    }
    }
    else
    {
      fw_inst_ptr->dl_opt_fltr_arr = opt_fltr_ptr;
    }
  }

  /*-------------------------------------------------------------------------
    Find the first empty slot in the opt filter array and use it to add
    opt filter
  -------------------------------------------------------------------------*/
  for (opt_fltr_count = 0;
       opt_fltr_count < max_fltr && fltr_index < num_filters;
       opt_fltr_count++, opt_fltr_ptr++)
  {
    if (opt_fltr_ptr->fi_handle != 0)
    {
      continue;
    }

    /*-----------------------------------------------------------------------
      Add TCP or UDP or TCP_UDP based filters to opt filter array
    -----------------------------------------------------------------------*/
    if (fltr_arr[fltr_index].ip_vsn == IP_V4)
    {
      opt_fltr_ptr->protocol = fltr_arr[fltr_index].ip_hdr.v4.next_hdr_prot;
    }
    else
    {
      opt_fltr_ptr->protocol = fltr_arr[fltr_index].ip_hdr.v6.next_hdr_prot;
    }

    if (opt_fltr_ptr->protocol == PS_IPPROTO_TCP)
    {
      if (fltr_arr[fltr_index].next_prot_hdr.tcp.field_mask &
            IPFLTR_MASK_TCP_SRC_PORT)
      {
        opt_fltr_ptr->src_port =
          fltr_arr[fltr_index].next_prot_hdr.tcp.src.port;
        opt_fltr_ptr->src_port_range =
          fltr_arr[fltr_index].next_prot_hdr.tcp.src.range;
      }
      else
      {
        opt_fltr_ptr->src_port = 0;
        opt_fltr_ptr->src_port_range = 0;
      }

      if (fltr_arr[fltr_index].next_prot_hdr.tcp.field_mask &
            IPFLTR_MASK_TCP_DST_PORT)
      {
        opt_fltr_ptr->dst_port =
          fltr_arr[fltr_index].next_prot_hdr.tcp.dst.port;
        opt_fltr_ptr->dst_port_range =
          fltr_arr[fltr_index].next_prot_hdr.tcp.dst.range;
      }
      else
      {
        opt_fltr_ptr->dst_port = 0;
        opt_fltr_ptr->dst_port_range = 0;
      }
    }
    else if (opt_fltr_ptr->protocol == PS_IPPROTO_UDP)
    {
      if (fltr_arr[fltr_index].next_prot_hdr.udp.field_mask &
            IPFLTR_MASK_UDP_SRC_PORT)
      {
        opt_fltr_ptr->src_port =
          fltr_arr[fltr_index].next_prot_hdr.udp.src.port;
        opt_fltr_ptr->src_port_range =
          fltr_arr[fltr_index].next_prot_hdr.udp.src.range;
      }
      else
      {
        opt_fltr_ptr->src_port = 0;
        opt_fltr_ptr->src_port_range = 0;
      }

      if (fltr_arr[fltr_index].next_prot_hdr.udp.field_mask &
            IPFLTR_MASK_UDP_DST_PORT)
      {
        opt_fltr_ptr->dst_port =
          fltr_arr[fltr_index].next_prot_hdr.udp.dst.port;
        opt_fltr_ptr->dst_port_range =
          fltr_arr[fltr_index].next_prot_hdr.udp.dst.range;
      }
      else
      {
        opt_fltr_ptr->dst_port = 0;
        opt_fltr_ptr->dst_port_range = 0;
      }
    }
    else if (opt_fltr_ptr->protocol == PS_IPPROTO_TCP_UDP)
    {
      if (fltr_arr[fltr_index].next_prot_hdr.tcp_udp_port_range.field_mask &
            IPFLTR_MASK_TCP_UDP_SRC_PORT)
      {
        opt_fltr_ptr->src_port =
          fltr_arr[fltr_index].next_prot_hdr.tcp_udp_port_range.src.port;
        opt_fltr_ptr->src_port_range =
          fltr_arr[fltr_index].next_prot_hdr.tcp_udp_port_range.src.range;
      }
      else
      {
        opt_fltr_ptr->src_port = 0;
        opt_fltr_ptr->src_port_range = 0;
      }

      if (fltr_arr[fltr_index].next_prot_hdr.tcp_udp_port_range.field_mask &
            IPFLTR_MASK_TCP_UDP_DST_PORT)
      {
        opt_fltr_ptr->dst_port =
          fltr_arr[fltr_index].next_prot_hdr.tcp_udp_port_range.dst.port;
        opt_fltr_ptr->dst_port_range =
          fltr_arr[fltr_index].next_prot_hdr.tcp_udp_port_range.dst.range;
      }
      else
      {
        opt_fltr_ptr->dst_port = 0;
        opt_fltr_ptr->dst_port_range = 0;
      }
    }
    else if ( (opt_fltr_ptr->protocol == PS_IPPROTO_ICMP) ||
              (opt_fltr_ptr->protocol == PS_IPPROTO_IGMP) )
    {
      opt_fltr_ptr->src_port = 0;
      opt_fltr_ptr->src_port_range = 0;
      opt_fltr_ptr->dst_port = 0;
      opt_fltr_ptr->dst_port_range = 0;
    }
    else
    {
      fltr_index++;
      /*---------------------------------------------------------------------
        Only TCP, UDP, TCP_UDP, ICMP, IGMP  based filters are supported 
        currently inoptimized path. So, reset protocol and move to next filter
      ---------------------------------------------------------------------*/
      opt_fltr_ptr->protocol = 0;
      continue;
    }

    fltr_index++;
    opt_fltr_ptr->fi_handle = fi_handle;
  } /* end for all filters */

  if (fltr_index < num_filters)
  {
    DS_MSG2(MSG_LEGACY_ERROR,"Out of filter space in opt path. "
                    "Added (%d) out of (%d)", fltr_index, num_filters);
    return -1;
  }

  return 0;

} /* ds_ipfltri_add_opt_filter() */


/*===========================================================================
FUNCTION DS_IPFLTRI_DELETE_OPT_FILTERS()

DESCRIPTION
  This function deletes filters from optimized data path.

PARAMETERS
  iface_ptr          : Iface to add filters to
  fi_handle          : Handle associated with this filter

RETURN VALUE
  TRUE if success, FALSE on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void ds_ipfltri_delete_opt_filter
(
  ds_fwk_s_type*                   fw_inst_ptr,
  ip_addr_enum_type                ip_type,
  ds_filter_group_enum_type        filter_group,
  ds_ipfltr_handle_type            fi_handle
)
{
  ds_dl_opt_filter_spec_type * opt_fltr_ptr;
  ds_dl_opt_filter_spec_type * nxt_opt_fltr_ptr;
  ds_dl_opt_filter_spec_type * tmp_opt_fltr_ptr;
  ds_dl_opt_filter_spec_type   tmp_opt_fltr;
  uint32                       max_fltr;
  uint8                        opt_fltr_count;
  uint8                        swap_fltr_count;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get address of opt filter array
      1. If iface is NULL, address points to global opt filter array
      2. Else, address points to opt filter array of the base iface of input
         iface. It must be base iface as DL opt handler runs on physical iface
         and not logical iface
  -------------------------------------------------------------------------*/
  if (fw_inst_ptr == NULL)
  {
    if (filter_group == DS_RM_GROUP)
    {
      opt_fltr_ptr = ds_global_ul_opt_fltr_arr;
    }
    else
    {    
    opt_fltr_ptr = ds_global_dl_opt_fltr_arr;
    }
    max_fltr     = MAX_GLOBAL_OPT_FILTER_ENTRIES;
  }
  else
  {
    opt_fltr_ptr = fw_inst_ptr->dl_opt_fltr_arr;
    max_fltr     = DS_MAX_OPT_FILTER_ENTRIES;
  }

  if (opt_fltr_ptr != NULL)
  {
    /*-----------------------------------------------------------------------
      Delete all the filters with filter handle as "fi_handle". Remove hole
      in opt filter array by shifting entries to left so that one doesn't have
      to traverse the whole array to look at valid opt filter entries
    -----------------------------------------------------------------------*/
    for (opt_fltr_count = 0;
         opt_fltr_count < max_fltr && opt_fltr_ptr->fi_handle != 0;
         opt_fltr_count++)
    {
      if (opt_fltr_ptr->fi_handle == fi_handle ||
          DS_IPFLTR_DELETE_ALL_HANDLE == fi_handle)
      {
        swap_fltr_count  = opt_fltr_count;
        tmp_opt_fltr_ptr = opt_fltr_ptr;
        nxt_opt_fltr_ptr = opt_fltr_ptr + 1;

        while (swap_fltr_count < (max_fltr - 1) &&
               nxt_opt_fltr_ptr->fi_handle != 0)
        {
          /*-----------------------------------------------------------------
            Swap the entries
          -----------------------------------------------------------------*/
          memscpy(&tmp_opt_fltr,
                  sizeof(ds_dl_opt_filter_spec_type),
                  nxt_opt_fltr_ptr,
                  sizeof(ds_dl_opt_filter_spec_type));
          memscpy(nxt_opt_fltr_ptr,
                  sizeof(ds_dl_opt_filter_spec_type),
                  tmp_opt_fltr_ptr,
                  sizeof(ds_dl_opt_filter_spec_type));
          memscpy(tmp_opt_fltr_ptr,
                  sizeof(ds_dl_opt_filter_spec_type),
                  &tmp_opt_fltr,
                  sizeof(ds_dl_opt_filter_spec_type));

          tmp_opt_fltr_ptr = nxt_opt_fltr_ptr;
          swap_fltr_count++;
          nxt_opt_fltr_ptr++;
        }

        memset(tmp_opt_fltr_ptr, 0, sizeof(ds_dl_opt_filter_spec_type));

        /*-------------------------------------------------------------------
          Since hole is filled by shifting entry left and since this entry
          hasn't been processed yet, don't increment opt_fltr_ptr
        -------------------------------------------------------------------*/
      } /* end if matched */
      else
      {
        opt_fltr_ptr++;
      }
    } /* end for all filters */
  } /* end if opt filters exist on this iface */

  return;
} /* ds_ipfltri_delete_opt_filter() */


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
)
{
  ds_ipfilteri_type          *  filter_buf_ptr;
  ds_ipfilteri_type          *  next_filter_buf_ptr;
  q_type                     *  ipfltr_q_ptr;
  int                           prev_cnt;
  ds_filter_group_enum_type     filter_group = DS_DEFAULT_GROUP;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG3(MSG_LEGACY_HIGH,"ds_ipfltr_delete(): FLTR DELETE called "
                  "by client %d, filter_id %d, fw_inst_ptr 0x%p",
                  client_id, fi_handle, fw_inst_ptr);

  if (ps_errno == NULL)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"ds_ipfltr_delete(): "
                    "NULL parameter is passed");
    return -1;
  }

  if (client_id >= FLTR_CLIENT_MAX)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"ds_ipfltr_delete(): "
                    "Invalid filtering client id %d", client_id);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  if (fi_handle == DS_IPFLTR_INVALID_HANDLE)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"ds_ipfltr_delete(): "
                    "Invalid filter handle is passed by client, %d",
                    client_id);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  /*-------------------------------------------------------------------------
    Figure out if filter needs to be deleted from global fltr Q or iface
    specific Q
  -------------------------------------------------------------------------*/
  if (fw_inst_ptr == NULL)
  {
    DS_MSG0(MSG_LEGACY_HIGH,"ds_ipfltr_delete(): "
                    "Deleting fltrs from global ipfltr q");
    ipfltr_q_ptr = &(global_ipfltr_info[client_id]);
  }
  else
  {
   DS_MSG2(MSG_LEGACY_HIGH,"ds_ipfltr_delete(): "
                    "Deleting fltrs from iface 0x%x:%d",
                    fw_inst_ptr, fw_inst_ptr->ds_fwk_index);
   ipfltr_q_ptr = &(fw_inst_ptr->ipfltr_info[client_id]);
  }
  
  prev_cnt    = q_cnt(ipfltr_q_ptr);
  filter_buf_ptr = q_check(ipfltr_q_ptr);
  while (filter_buf_ptr != NULL)
  {
    next_filter_buf_ptr = q_next(ipfltr_q_ptr, &(filter_buf_ptr->link));
    /*-----------------------------------------------------------------------
      If generic filter handle is passed, it means that delete operation needs
      to be applied  on all filters
    -----------------------------------------------------------------------*/
    if (filter_buf_ptr->fi_handle == fi_handle ||
        DS_IPFLTR_DELETE_ALL_HANDLE == fi_handle)
    {
      filter_group = filter_buf_ptr->filter_group;
      if (FLTR_CLIENT_PDN_SHARING == client_id ) 
      {            
        DS_MSG1(MSG_LEGACY_HIGH,"ds_ipfltri_delete pdn_sharing filter, ip_type is %d",ip_type);
        if((filter_buf_ptr->fi_default.ip_vsn != IP_ANY_ADDR)&&
           (filter_buf_ptr->fi_default.ip_vsn != ip_type))
        {
          filter_buf_ptr = next_filter_buf_ptr;
          continue;
        }
      }
      /*---------------------------------------------------------------------
          Delete this filter
      ---------------------------------------------------------------------*/
#ifdef FEATURE_Q_NO_SELF_QPTR
      q_delete(ipfltr_q_ptr, &(filter_buf_ptr->link));
#else
      q_delete(&(filter_buf_ptr->link));
#endif

      ds_ipfltri_free_buf(filter_buf_ptr);
    }

    filter_buf_ptr = next_filter_buf_ptr;
  }

  if (FLTR_CLIENT_SOCKETS == client_id)
  {
    ds_ipfltri_delete_opt_filter(fw_inst_ptr,ip_type, filter_group, fi_handle);
  }

  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  DS_MSG4(MSG_LEGACY_HIGH,"ds_ipfltri_delete"
          "fwk 0x%p, client %d, n_filters %d -> %d",
          fw_inst_ptr,client_id,prev_cnt, q_cnt(ipfltr_q_ptr));
  return 0;

} /* ds_ipfltri_delete() */


/*===========================================================================
FUNCTION DS_IPFLTRI_ADD()

DESCRIPTION Internal function to add filters on iface or global ip filter
            based on group

DEPENDENCIES

RETURN VALUE
  ds_ipfltr_handle on successful filter addition
  DS_IPFLTR_INVALID_HANDLE on failure 

SIDE EFFECTS
  None.

===========================================================================*/
static ds_ipfltr_handle_type ds_ipfltri_add
(
  ds_fwk_s_type*                   fw_inst_ptr,
  ip_addr_enum_type                ip_type,
  ds_filter_group_enum_type        filter_group,
  const ds_ipfltr_add_param_type * fltr_param_ptr,
  int16                          * ps_errno
)
{
  q_type                         * ipfltr_q_ptr;
  ds_ipfilteri_type              * new_filter_buf_ptr = NULL;
  ds_ipfilteri_type              * tmp_tx_fltr_buf = NULL;
  ds_ipfltr_handle_type            fltr_handle;
  int                              prev_cnt = 0;
  uint8                            fltr_index;
  ip_filter_type                 * fltr_ptr = NULL;
  struct ps_in6_addr               v6_addr;  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG3(MSG_LEGACY_HIGH,"ds_ipfltri_add(): "
                  "FLTR ADD called by client %d, instance 0x%p group 0x%x",
                  fltr_param_ptr->client_id, fw_inst_ptr, filter_group);

  /*-------------------------------------------------------------------------
    Validate all the parameters
  -------------------------------------------------------------------------*/
  if (ps_errno == NULL)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"ds_ipfltri_add(): "
                    "NULL parameter is passed");
    return DS_IPFLTR_INVALID_HANDLE;
  }

  if (fltr_param_ptr->client_id >= FLTR_CLIENT_MAX)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"ds_ipfltri_add(): "
                    "Invalid filtering client id %d", fltr_param_ptr->client_id);
    *ps_errno = DS_EFAULT;
    return DS_IPFLTR_INVALID_HANDLE;
  }

  if (fltr_param_ptr->fi_ptr_arr == NULL || fltr_param_ptr->num_filters == 0)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"ds_ipfltri_add(): "
                    "No filter specified by client %d", fltr_param_ptr->client_id);
    *ps_errno = DS_EFAULT;
    return DS_IPFLTR_INVALID_HANDLE;
  }

  if (fltr_param_ptr->fi_result == DS_IPFLTR_NOMATCH)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"ds_ipfltri_add(): Invallid filter_result"
                    "specified by client %d", fltr_param_ptr->client_id);
    *ps_errno = DS_EFAULT;
    DS_ASSERT(0);
    return DS_IPFLTR_INVALID_HANDLE;
  }

  if (fltr_param_ptr->fltr_priority != DS_IPFLTR_PRIORITY_FCFS)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"ds_ipfltri_add(): Invalid fltr add style %d",
                    fltr_param_ptr->fltr_priority);
    DS_ASSERT(0);
    *ps_errno = DS_EFAULT;
    return DS_IPFLTR_INVALID_HANDLE;
  }

  if (filter_group < DS_DEFAULT_GROUP || filter_group > DS_RM_GROUP )
  {
    *ps_errno = DS_EFAULT;
     DS_MSG1(MSG_LEGACY_ERROR,"ds_ipfltri_add(): incorrect iface group %D",
                      filter_group);
    return DS_IPFLTR_INVALID_HANDLE;
  }

  *ps_errno = DS_ENOERR;

  if (!ds_ipfltr_validate_fltr_param(fltr_param_ptr->client_id,
                                     fltr_param_ptr->fi_ptr_arr,
                                     fltr_param_ptr->num_filters))
  {
    DS_MSG1(MSG_LEGACY_HIGH,"ds_ipfltri_add(): Invalid fltr spec is specified "
                     "by client %d", fltr_param_ptr->client_id);
    *ps_errno = DS_EINVAL;
     return DS_IPFLTR_INVALID_HANDLE;
  }
  
  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  do
  {
    if (fw_inst_ptr == NULL)
    {
      DS_MSG0(MSG_LEGACY_HIGH,"ds_ipfltri_add(): "
                      "Adding fltrs to global ipfltr q");
      ipfltr_q_ptr = &(global_ipfltr_info[fltr_param_ptr->client_id]);
    }
    else 
    {
      DS_MSG2(MSG_LEGACY_HIGH,"ds_ipfltri_add(): Adding fltrs to FWK 0x%x:%d",
                      fw_inst_ptr, fw_inst_ptr->ds_fwk_index);
      ipfltr_q_ptr = &(fw_inst_ptr->ipfltr_info[fltr_param_ptr->client_id]);
    }

    prev_cnt    = q_cnt(ipfltr_q_ptr);
    fltr_handle = DS_IPFLTR_INVALID_HANDLE;

    /*-------------------------------------------------------------------------
      Now add the new filters to this iface
    -------------------------------------------------------------------------*/
    for (fltr_index = 0; fltr_index < fltr_param_ptr->num_filters; fltr_index++)
    {
      fltr_ptr = (((ip_filter_type *) fltr_param_ptr->fi_ptr_arr) + fltr_index);

      /*-------------------------------------------------------------------------
        Check if someone is adding destination based filter for client sockets 
        on iface for which v6 ip address doesnt exist on that iface
      -------------------------------------------------------------------------*/
      if(FLTR_CLIENT_SOCKETS == fltr_param_ptr->client_id)
      {
        if ((IP_V6 == fltr_ptr->ip_vsn) && 
            (0 != (IPFLTR_MASK_IP6_DST_ADDR & fltr_ptr->ip_hdr.v6.field_mask)))
        {        
          v6_addr.ps_s6_addr64[0] = fltr_ptr->ip_hdr.v6.dst.addr.ps_s6_addr64[0];
          v6_addr.ps_s6_addr64[1] = fltr_ptr->ip_hdr.v6.dst.addr.ps_s6_addr64[1];
         //TODO:need to migrate to new API. 
         /* retval = ds_find_ipv6_addr( &v6_addr,
                                            &v6_iface_addr_ptr,
                                            &iface_ptr);    
  
          if ((FALSE == retval) || (NULL == v6_iface_addr_ptr))
          {        
            continue;
          } */
        }        
      }
      
      if ((new_filter_buf_ptr = (ds_ipfilteri_type *)ds_ipfltri_alloc_buf()) == NULL)
      {
        DS_MSG0(MSG_LEGACY_HIGH,"ds_ipfltri_add(): "
                        "Failed to create buffer for IP filter");

        *ps_errno = DS_ENOMEM;
        break;
      }

      if (fltr_index == 0)
      {
        fltr_handle = (ds_ipfltr_handle_type) new_filter_buf_ptr;
      }

      new_filter_buf_ptr->fi_default = *fltr_ptr;
     (void) q_link(new_filter_buf_ptr, &(new_filter_buf_ptr->link));

      new_filter_buf_ptr->fi_handle        = fltr_handle;
      new_filter_buf_ptr->fi_result        = fltr_param_ptr->fi_result;
      new_filter_buf_ptr->filter_group     = filter_group;
      new_filter_buf_ptr->disabled         = FALSE;
     /*-------------------------------------------------------------------
         Add the new filters to the framework sorted by precedence
    -------------------------------------------------------------------*/
     tmp_tx_fltr_buf = q_check(ipfltr_q_ptr);

     while (tmp_tx_fltr_buf != NULL &&
            new_filter_buf_ptr->fi_default.ipfltr_aux_info.fi_precedence
            >= tmp_tx_fltr_buf->fi_default.ipfltr_aux_info.fi_precedence)
     {
       tmp_tx_fltr_buf = q_next(ipfltr_q_ptr, &(tmp_tx_fltr_buf->link));
     }
   
     if (NULL == tmp_tx_fltr_buf)
     {
       q_put(ipfltr_q_ptr, &new_filter_buf_ptr->link);
     }
     else
     {
       q_insert(ipfltr_q_ptr, &new_filter_buf_ptr->link, &tmp_tx_fltr_buf->link);
     }
    }

    /*-------------------------------------------------------------------------
      If error occured during install filter then dont install filter 
      in software
    -------------------------------------------------------------------------*/ 
    if (DS_ENOERR != *ps_errno)    
    {
      DS_MSG0 (MSG_LEGACY_ERROR,"Error occured during install filter");
      break;
    }
    
    /*-----------------------------------------------------------------------
       we need to install ul opt SW filters to send some specific traffic 
       to legacy path.
      -----------------------------------------------------------------------*/
      if ( (FLTR_CLIENT_SOCKETS == fltr_param_ptr->client_id)           
        #ifdef FEATURE_DATA_PS_464XLAT 
          ||  FLTR_CLIENT_CLAT == fltr_param_ptr->client_id
        #endif /* FEATURE_DATA_PS_464XLAT */ 
         )
      {
        if (-1 == ds_ipfltri_add_opt_filter(      fw_inst_ptr,
                                                  ip_type,
                                                  filter_group,
                                                  fltr_param_ptr->fi_ptr_arr,
                                                  fltr_param_ptr->num_filters,
                                                  fltr_handle))
        {
          DS_MSG2(MSG_LEGACY_ERROR,"ds_ipfltri_add():"
                          "Couldnt add Opt filters to iface 0x%x:%d",
                          fw_inst_ptr,
                          fw_inst_ptr?fw_inst_ptr->ds_fwk_index:-1);
          break;
        }
      }
    DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    DS_MSG4(MSG_LEGACY_HIGH,"ds_ipfltri_add"
            "fwk 0x%p, client %d, n_filters %d -> %d",
            fw_inst_ptr,fltr_param_ptr->client_id,prev_cnt, q_cnt(ipfltr_q_ptr));
    return fltr_handle;
  } while (0);

  /*-------------------------------------------------------------------------
    Filter failed to install so do the clean up
  -------------------------------------------------------------------------*/  
  (void) ds_ipfltri_delete(fw_inst_ptr,ip_type, fltr_param_ptr->client_id, fltr_handle, ps_errno);
  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  return DS_IPFLTR_INVALID_HANDLE;
} /* ds_ipfltri_add() */


/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION DS_IPFLTR_POWERUP()

DESCRIPTION
  Initialize the global IP filter queue.

DEPENDENCIES
  None

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void ds_ipfltr_powerup
(
  void
)
{
  uint8 i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Initialize Pool
  -------------------------------------------------------------------------*/
  if (PS_MEM_POOL_INIT_OPT(PS_MEM_DS_IPFLTER_TYPE,
                           ds_ipfltr_buf_mem,
                           DS_IPFLTR_BUF_SIZE,
                           DS_IPFLTR_BUF_NUM,
                           DS_IPFLTR_BUF_HIGH_WM,
                           DS_IPFLTR_BUF_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) ds_ipfltr_buf_hdr,
                           (int *) ds_ipfltr_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"ds_ipfltr_powerup(): Can't power up the module");
  }

  for (i = 0; i < FLTR_CLIENT_MAX; i++)
  {
    (void) q_init(&(global_ipfltr_info[i]));
  }
} /* ds_ipfltr_powerup() */


/*===========================================================================
FUNCTION DS_IPFLTR_INIT()

DESCRIPTION
  Adds  global TCP Ack filters, v4 fragmentation fitlers.

DEPENDENCIES
  None

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void ds_ipfltr_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_ipfltr_powerup();

  return;
} /* ds_ipfltr_init() */


/*===========================================================================
FUNCTION DS_IPFLTR_ADD()

DESCRIPTION
  This function adds a set of IP filters to the specified iface. The
  filter set is tied to a filter handle, which uniquely idenifies a set of
  filters added by the given client for the given iface. The filter handle
  is used to manipulate the filter set. A client needs to provide a filter
  result which is returned when a filter successfully match during the filter
  execution.
  If filters are added in disabled state they won't be executed until
  they are enabled.

PARAMETERS
  fltr_add_param_ptr : ptr to structure containing all the necessary info to
                       add filters to an fwk instance
  ps_errno           : error returned to the caller if operation fails

RETURN VALUE
  A handle to filters            : on success
  DS_IPFLTR_INVALID_HANDLE : on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ds_ipfltr_handle_type ds_ipfltr_add
(
  ds_ipfltr_add_param_type        * fltr_param_ptr,
  int16                           * ps_errno
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_fwk_clnt_info_s_type      clnt_info_ptr;
  ds_ipfltr_handle_type        handle = DS_IPFLTR_INVALID_HANDLE;

  if( fltr_param_ptr == NULL )
  {
    DS_MSG0(MSG_LEGACY_ERROR,"fltr_param_ptr is invalid");
    return DS_IPFLTR_INVALID_HANDLE;
  }
  memset(&clnt_info_ptr,0x00,sizeof(ds_fwk_clnt_info_s_type));

  if( fltr_param_ptr->client_id != FLTR_CLIENT_QOS_OUTPUT 
       && fltr_param_ptr->client_id != FLTR_CLIENT_QOS_INPUT)
  {
    ds_fwk_get_clnt_info_from_handle(
           fltr_param_ptr->client_type,fltr_param_ptr->client_handle,&clnt_info_ptr);
    if(clnt_info_ptr.fw_inst == NULL)
    {
      DS_MSG1(MSG_LEGACY_ERROR,"invalid client_handle %d",fltr_param_ptr->client_handle);
    }
  }
  if( fltr_param_ptr->client_id == FLTR_CLIENT_QOS_OUTPUT )
  {
    clnt_info_ptr.fw_inst = fltr_param_ptr->fw_inst_ptr;    
  }

  handle = ds_ipfltri_add( clnt_info_ptr.fw_inst,
                           clnt_info_ptr.ip_type,
                              DS_DEFAULT_GROUP, 
                              fltr_param_ptr,
                              ps_errno);
  return handle;
} /* ds_ipfltr_add() */

/*===========================================================================
FUNCTION DS_IPFLTR_ADD_BY_GROUP()

DESCRIPTION
  This function adds a set of IP filters to the global ip filter array
  based on group. The filter set is tied to a filter handle, which uniquely
  idenifies a set of filters added by the client for the given iface
  group. The filter handle is used to manipulate the filter set. A client
  needs to provide a filter result which is returned when a filter
  successfully match during the filter execution.

  If filters are added in disabled state they won't be executed until
  they are enabled.

PARAMETERS
  filter_group        : filter group for which filters need to be executed
  client_id          : Filtering client id
  fltr_param_ptr     : ptr to structure containing all the necessary info
                       to add filters to an iface
  ps_errno           : error returned to the caller if operation fails

RETURN VALUE
  A handle to filters            : on success
  DS_IPFLTR_INVALID_HANDLE : on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ds_ipfltr_handle_type ds_ipfltr_add_by_group
(
  ds_filter_group_enum_type               filter_group,
  ds_ipfltr_client_id_enum_type           client_id,
  const ds_ipfltr_add_param_type        * fltr_param_ptr,
  int16                                 * ps_errno
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return ds_ipfltri_add(      NULL,
                              IP_ANY_ADDR,
                              filter_group,
                              fltr_param_ptr,
                              ps_errno);

} /* ds_ipfltr_add_by_group() */


/*===========================================================================
FUNCTION DS_IPFLTR_ADD_QOS_RULES()

DESCRIPTION
  This function adds a set of IP filters to the specified fwk instance. The
  filter set is tied to a filter handle, which uniquely idenifies a set of
  filters added by the given client for the given iface. The filter handle
  is used to manipulate the filter set. A client needs to provide a filter
  result which is returned when a filter successfully match during the filter
  execution.
  If filters are added in disabled state they won't be executed until
  they are enabled.

PARAMETERS
  fltr_add_param_ptr : ptr to structure containing all the necessary info to
                       add filters to an iface
  ps_errno           : error returned to the caller if operation fails

RETURN VALUE
  A handle to filters            : on success
  DS_IPFLTR_INVALID_HANDLE : on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ds_ipfltr_handle_type ds_ipfltr_add_qos_rules
(
  int                               fwk_index,
  ip_addr_enum_type                 ip_type,
  ds_ipfltr_add_param_type        * fltr_param_ptr,
  int16                           * ps_errno
)
{
  ds_fwk_s_type * fw_inst = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( fwk_index > DS_FWK_INVALID_ID && fwk_index < DS_FWK_MAX_INST)
  {
    fw_inst = ds_fwk_get_inst_ptr_from_index(fwk_index);
    if( fw_inst != NULL)
    {
      return ds_ipfltri_add(  fw_inst,
                              ip_type,
                              DS_DEFAULT_GROUP, 
                              fltr_param_ptr,
                              ps_errno);
    }
  }
  return DS_IPFLTR_INVALID_HANDLE;
} /* ds_ipfltr_add() */

/*===========================================================================
FUNCTION DS_IPFLTR_DELETE()

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
int ds_ipfltr_delete
(
  ds_ipfltr_delete_param_type    * fltr_delete_param_ptr,
  int16                          * ps_errno
)
{
  ds_fwk_clnt_info_s_type clnt_info_ptr;
  
  if( fltr_delete_param_ptr == NULL )
  {
    DS_MSG0(MSG_LEGACY_ERROR,"fltr_param_ptr is invalid");
    return DS_IPFLTR_INVALID_HANDLE;
  }
  
  memset(&clnt_info_ptr,0x00,sizeof(ds_fwk_clnt_info_s_type));
  if( fltr_delete_param_ptr->client_id != FLTR_CLIENT_QOS_OUTPUT 
       && fltr_delete_param_ptr->client_id != FLTR_CLIENT_QOS_INPUT)
  {
    ds_fwk_get_clnt_info_from_handle(
            fltr_delete_param_ptr->client_type,
            fltr_delete_param_ptr->client_handle,
            &clnt_info_ptr);
    if(clnt_info_ptr.fw_inst == NULL)
    {
      DS_MSG1(MSG_LEGACY_ERROR,"invalid client_handle %d",
            fltr_delete_param_ptr->client_handle);
    }
  }
  if( fltr_delete_param_ptr->client_id == FLTR_CLIENT_QOS_OUTPUT )
  {
    clnt_info_ptr.fw_inst = fltr_delete_param_ptr->fw_inst_ptr;    
  }

  return ds_ipfltri_delete( clnt_info_ptr.fw_inst,clnt_info_ptr.ip_type,
     fltr_delete_param_ptr->client_id, fltr_delete_param_ptr->fi_handle,ps_errno);
}

/*===========================================================================
FUNCTION DS_IPFLTR_CONTROL()

DESCRIPTION
  This function allows a control operation on all IP filters currently
  associated with the specified filter handle of the client for the Fwk instance.
  Only operation supported for now is disabling or enabling the filter
  execution state. Disabled filters are not used during filter execution.

DEPENDENCIES
  None

PARAMETERS
  fwk_ptr     : fwk instance on which filters are added
  client_id    : Filtering client id
  fi_handle   : filter handle which identifies a specific filter set added on
                    this instance by this client.
  enable       : Whether to enable or disable already installed filters
  ps_errno    : error returned to the caller if operation fails

RETURN VALUE
   0 : on success
  -1 : on failure

SIDE EFFECTS
  Some packets may not get filtered once a filter set is disabled and they
  may get filtered if a filter set is enabled
===========================================================================*/
int ds_ipfltr_control
(
  ds_ipfltr_ctrl_param_type           * fltr_ctrl_param_ptr,
  int16                               * ps_errno
)
{
  q_type                   * q_ptr;
  ds_ipfilteri_type        * filter_buf_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Validate all the parameters
  -------------------------------------------------------------------------*/
  if (ps_errno == NULL || fltr_ctrl_param_ptr == NULL)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"ds_ipfltr_control(): "
                    "NULL parameter is passed");
    return -1;
  }
  DS_MSG3(MSG_LEGACY_HIGH,"ds_ipfltr_control(): "
          "FLTR CONTROL called by client %d, filter_id %d, fltrs enabled=%d",
           fltr_ctrl_param_ptr->client_id,
           fltr_ctrl_param_ptr->fi_handle,
           fltr_ctrl_param_ptr->enable);

  if (fltr_ctrl_param_ptr->client_id >= FLTR_CLIENT_MAX)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"ds_ipfltr_control(): "
              "Invalid filtering client id %d", fltr_ctrl_param_ptr->client_id);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  if (fltr_ctrl_param_ptr->fi_handle == DS_IPFLTR_INVALID_HANDLE)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"ds_ipfltr_control(): "
                    "Invalid filter handle is passed by client, %d",
                     fltr_ctrl_param_ptr->client_id);
   *ps_errno = DS_EFAULT;
    return -1;
  }

  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (fltr_ctrl_param_ptr->ds_fwk_inst_ptr == NULL)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"ds_ipfltr_control(): "
                    "Controlling fltrs on global ipfltr q");
    q_ptr = &(global_ipfltr_info[fltr_ctrl_param_ptr->client_id]);
  }
  else
  {
    DS_MSG1(MSG_LEGACY_ERROR,"ds_ipfltr_control(): "
                    "Controlling fltrs on ds_fwk_inst_ptr 0x%x",
                    fltr_ctrl_param_ptr->ds_fwk_inst_ptr);
    q_ptr = &(fltr_ctrl_param_ptr->ds_fwk_inst_ptr->\
            ipfltr_info[fltr_ctrl_param_ptr->client_id]);
  }

  /*-------------------------------------------------------------------------
    Perform the control operation on all the filters that match the handle
  -------------------------------------------------------------------------*/
  filter_buf_ptr = q_check(q_ptr);
  while (filter_buf_ptr != NULL)
  {
    if ((filter_buf_ptr->fi_handle == fltr_ctrl_param_ptr->fi_handle) ||
         (DS_IPFLTR_DELETE_ALL_HANDLE == fltr_ctrl_param_ptr->fi_handle))
    {
      filter_buf_ptr->disabled = !fltr_ctrl_param_ptr->enable;
      if (FLTR_CLIENT_SOCKETS == fltr_ctrl_param_ptr->client_id || 
          FLTR_CLIENT_QOS_OUTPUT == fltr_ctrl_param_ptr->client_id)
      {
        if (fltr_ctrl_param_ptr->enable)
        {
          if (FLTR_CLIENT_SOCKETS == fltr_ctrl_param_ptr->client_id)
          {
            if (FALSE == ds_ipfltri_add_opt_filter
                         (
                           fltr_ctrl_param_ptr->ds_fwk_inst_ptr,
                           IP_ANY_ADDR,
                           DS_DEFAULT_GROUP,
                           &(filter_buf_ptr->fi_default),
                           1,
                           fltr_ctrl_param_ptr->fi_handle
                         ))
            {
              DS_MSG0(MSG_LEGACY_ERROR,"Couldn't add Opt filters ");
            }
          }
        }
        else
        {
          ds_ipfltri_delete_opt_filter(fltr_ctrl_param_ptr->ds_fwk_inst_ptr,
                                       IP_ANY_ADDR,
                                       DS_DEFAULT_GROUP,
                                       fltr_ctrl_param_ptr->fi_handle);
        }
      }
    }
    filter_buf_ptr = q_next(q_ptr, &(filter_buf_ptr->link));
  }

  /*As of now indication is not needed, we will enable if required in future*/
  //ds_ipfltr_updated_ind(iface_ptr, client_id, q_cnt(q_ptr), q_cnt(q_ptr));

  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  return 0;
}
/*============================================================
FUNCTION DS_IPFLTR_EXECUTE()

DESCRIPTION
  This function executes all the filters in an iface to see if any of
  those passes criteria specified in the information passed in. Processing
  is done until the first match is found and filter id associated
  with that filter is returned. For DEFAULT filter type, each field set in
  the filter is compared against the input info using a fixed set of rules.
  For ACL type filters, the ACL function is called which can contain more
  complex and variant types of rules.

DEPENDENCIES
  None

PARAMETERS
  iface_ptr          - Iface to pick filters from for execution
  client_id          - Filtering client id
  subset_id          - ID which identified a subset of all filters installed
                       on iface. Only these filters are considered for
                       matching a packet
  ip_filter_info_ptr - Ptr to IP pkt information block to apply filter on

RETURN VALUE
  filter id associated with the filter : on a successful filter match
  DS_IPFLTR_NOMATCH              : for no match

SIDE EFFECTS
  None
===========================================================================*/
ds_ipfltr_result_type ds_ipfltr_execute
(
  int                           fwk_index,
  ds_ipfltr_client_id_enum_type client_id,
  ds_ipfltr_subset_id_type      subset_id,
  ip_pkt_info_type             *ip_pkt_info_ptr
)
{
  q_type                    * q_ptr;
  ds_ipfilteri_type         * filter_buf_ptr;
  ds_ipfltr_result_type       result = DS_IPFLTR_NOMATCH;
  ds_filter_group_enum_type   filter_group = DS_DEFAULT_GROUP;
  ds_addr_family_type         ip_type;
  ds_fwk_s_type*              fw_inst_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG3(MSG_LEGACY_HIGH,"ds_ipfltr_execute(): "
                  "Executing fltrs fw_inst_ptr 0x%p, client %d, group 0x%x",
                  fw_inst_ptr, client_id, filter_group);

  /*-------------------------------------------------------------------------
    Validate all the parameters
  -------------------------------------------------------------------------*/
  if (client_id >= FLTR_CLIENT_MAX )
  {
    DS_ASSERT(0);
    DS_MSG1(MSG_LEGACY_ERROR,"ds_ipfltr_execute(): "
                    "Invalid filtering client id %d", client_id);
    return result;
  }

  if (ip_pkt_info_ptr == NULL)
  {
    DS_MSG2(MSG_LEGACY_ERROR,"ds_ipfltr_execute(): "
                    "fw_inst_ptr 0x%p client %d null pkt info", fw_inst_ptr, client_id);
    return result;
  }
  
  if( fwk_index != -1 )
    fw_inst_ptr = ds_fwk_get_inst_ptr_from_index(fwk_index);
  
  ip_type = (ds_addr_family_type) ip_pkt_info_ptr->ip_vsn;
  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (fw_inst_ptr == NULL)
  {
    q_ptr = &(global_ipfltr_info[client_id]);
  }
  else 
  {
    q_ptr = &(fw_inst_ptr->ipfltr_info[client_id]);
  }
  
  for (filter_buf_ptr = q_check(q_ptr);
       filter_buf_ptr != NULL;
       filter_buf_ptr = q_next(q_ptr, &(filter_buf_ptr->link)))
  {
    /*-----------------------------------------------------------------------
      Skip the filters which are disabled.
      Skip the filters that doesn't belong to same subset_id. If subset_id is
      DS_IPFLTR_SUBSET_ID_DEFAULT, go through all filters
    -----------------------------------------------------------------------*/
    if ( !filter_buf_ptr->disabled &&
         (subset_id == filter_buf_ptr->subset_id ||
          subset_id == DS_IPFLTR_SUBSET_ID_DEFAULT))
    {
        /*-------------------------------------------------------------------
          For default filter, process the filter info to see if required
          fields match.  Only execute the filter which actually matches the
          IP version of the received packet, skip others.
        -------------------------------------------------------------------*/
        if (filter_buf_ptr->fi_default.ip_vsn != (ip_addr_enum_type)ip_pkt_info_ptr->ip_vsn)
        {
          continue;
        }

        if (ds_ipfltri_process_default(
             &(filter_buf_ptr->fi_default),
             ip_pkt_info_ptr) == TRUE)
        {
          result = filter_buf_ptr->fi_result;
          DS_MSG3(MSG_LEGACY_HIGH,"ds_ipfltr_execute(): "
                          "Fltr passed fw_inst_ptr 0x%p, client %d, fi_result 0x%x",
                          fw_inst_ptr, client_id, result);
        }
      if (result != DS_IPFLTR_NOMATCH)
      {
        break;
      }
    }
  }

  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return result;
} 

/*===========================================================================
FUNCTION DS_IPFLTR_VALIDATE_FLTR_PARAM()

DESCRIPTION
  Validates parameters of a filter

PARAMETERS
  client_id : Filtering client id
  fltr_arr  : Array of ptr to filters
  num_fltr  : Number of filters in above array

RETURN VALUE
  TRUE  : if parameters are valid
  FALSE : otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  the appropriate error masks will be set with the appropriate error codes
  for values and enums which failed validation. Because variables are only
  validated if the enums are set in the field masks the only error masks
  that will be updated will be those corresponding to the values set within
  the field masks.
===========================================================================*/
boolean ds_ipfltr_validate_fltr_param
(
  ds_ipfltr_client_id_enum_type        client_id,
  ip_filter_type                       fltr_arr[],
  uint8                                num_fltr
)
{
  ip_filter_type         * fltr_ptr;
  uint8                    fltr_index;
  ps_ip_protocol_enum_type    next_hdr_prot;
  boolean                  is_fltr_valid = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (client_id >= FLTR_CLIENT_MAX)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"ds_ipfltr_validate_fltr_param(): "
                    "Invalid client_id, %d, is passed", client_id);
    return FALSE;
  }

  if (fltr_arr == NULL)
  {
    DS_ERR_FATAL("ds_ipfltr_validate_fltr_param(): NULL parameter is passed");
    return FALSE;
  }

  DS_MSG1(MSG_LEGACY_HIGH,"ds_ipfltr_validate_fltr_param(): "
                  "Validating filters of client %d", client_id);

  for (fltr_index = 0; fltr_index < num_fltr; fltr_index++)
  {
    next_hdr_prot = PS_NO_NEXT_HDR;
    fltr_ptr = &fltr_arr[fltr_index];

    /*----------------------------------------------------------------------
      Validate all fltr enums and variables. Validation fails if any of the
      parameters are invalid
    ----------------------------------------------------------------------*/
    if (fltr_ptr->ip_vsn != IP_V4 && fltr_ptr->ip_vsn != IP_V6)
    {
      DS_MSG2(MSG_LEGACY_ERROR,"ds_ipfltr_validate_fltr_param(): "
                      "Incorrect IP version specified %d by client %d",
                      fltr_ptr->ip_vsn, client_id);
      return FALSE;
    }

    /*-------------------------------------------------------------------------
      Filter spec is invalid if any of the protocol params are invalid and
      hence "is_valid &=" is used below.
    -------------------------------------------------------------------------*/
    if (fltr_ptr->ip_vsn == IP_V4)
    {
      is_fltr_valid &= ds_ipfltri_validate_ipv4_param(client_id,
                                                            fltr_ptr,
                                                            &next_hdr_prot);
    }
    else /* IP version is IPV6 */
    {
      is_fltr_valid &= ds_ipfltri_validate_ipv6_param(client_id,
                                                            fltr_ptr,
                                                            &next_hdr_prot);
    }

    if (next_hdr_prot != PS_NO_NEXT_HDR)
    {
      switch (next_hdr_prot)
      {
        case PS_IPPROTO_TCP:
          is_fltr_valid &=
            ds_ipfltri_validate_tcp_param(client_id, fltr_ptr);
          break;

        case PS_IPPROTO_UDP:
          is_fltr_valid &=
            ds_ipfltri_validate_udp_param(client_id, fltr_ptr);
          break;

        case PS_IPPROTO_ICMP:
        case PS_IPPROTO_ICMP6:
          is_fltr_valid &=
            ds_ipfltri_validate_icmp_param(client_id, fltr_ptr);
          break;

        case PS_IPPROTO_IGMP:
        case PS_IPPROTO_ESP:
        case PS_IPPROTO_AH:
          /* Do nothing */
          break;

        case PS_IPPROTO_TCP_UDP:
          is_fltr_valid &=
            ds_ipfltri_validate_tcp_udp_param(client_id, fltr_ptr);
          break;

        default:
          is_fltr_valid = FALSE;
          break;
      }
    } /* end if (next_prot_hdr != PS_NO_NEXT_HDR) */
  }

  return is_fltr_valid;

} /* ds_ipfltr_validate_fltr_param() */


/*===========================================================================
FUNCTION DS_IPFLTRI_STATS()

DESCRIPTION
  gets the stats on the specified instance

DEPENDENCIES
  None

PARAMETERS
  fw_inst_ptr -- fwk instance
  ip_type -- ip version
  client -- type of filter
  max_allowed -- max number of allowed filters
  used -- number of filters already used
  ps_errono - error number

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

int ds_ipfltri_stats
(
  ds_fwk_s_type                         * fw_inst_ptr,
  ip_addr_enum_type                       ip_type,
  ds_ipfltr_client_id_enum_type           client,
  uint32                                * max_allowed,
  uint32                                * used,
  int16                                 * ps_errno
)
{
  q_type                   * ipfltr_q_ptr;
  int                        ret_val = 0;
  /*---------------------------------------------------------------------------------------------*/

  do
  {
    if (ps_errno == NULL)
    {
      ret_val = -1;
      break;
    }          
              
    if ( ( max_allowed == NULL ) || ( used == NULL ) || ( fw_inst_ptr == NULL )
          || ( client >= FLTR_CLIENT_MAX ))
    {
      ret_val = -1;
      *ps_errno = DS_EFAULT;
      break;
    }

    if (fw_inst_ptr == NULL || (ip_type != IPV4_ADDR && ip_type != IPV6_ADDR))
    {
      DS_MSG0(MSG_LEGACY_HIGH,"ds_ipfltr_stats(): "
                      "Adding fltrs to global ipfltr q");
      ipfltr_q_ptr = &(global_ipfltr_info[client]);
    }
    else if (fw_inst_ptr != NULL )
    {
      DS_MSG2(MSG_LEGACY_HIGH,"ds_ipfltr_stats(): Adding fltrs to iface 0x%x:%d",
                      fw_inst_ptr, fw_inst_ptr->ds_fwk_index);
      ipfltr_q_ptr = &(fw_inst_ptr->ipfltr_info[client]);
    }
    switch(client)
    {
      case FLTR_CLIENT_MO_EXCP:
        *max_allowed = DS_MO_EXCP_IPFLTR_MAX;
        break;
      case FLTR_CLIENT_PDN_SHARING:
        *max_allowed = DS_PDN_SHARING_IPFLTR_MAX;
        break;  
      default:
        *ps_errno = DS_EINVAL;
    }
  
    if( (NULL != ps_errno) && (0 != *ps_errno) )
    {
      ret_val = -1;
    }
    else
    {
      *used = q_cnt(ipfltr_q_ptr);  
    }
  }while(0);

  DS_MSG5(MSG_LEGACY_HIGH, "ds_ipfltr_stats: ps_errno=%d, max %d "
                   " used %d  client %d fw_inst_ptr 0x%x", 
                  ((NULL != ps_errno)?*ps_errno: DS_EFAULT), 
                  ((NULL != max_allowed)?*max_allowed: 0),
                  ((NULL != used)?*used: 0), 
                   client,
                   fw_inst_ptr);
  
  return ret_val;
}/* ds_ipfltr_stats */

/*===========================================================================
FUNCTION DS_IPFLTR_STATS()

DESCRIPTION
  gets the stats on the specified instance

DEPENDENCIES
  None

PARAMETERS
  client type -- type of client
  client handle  -- unique client hadle
  client -- type of filter
  max_allowed -- max number of allowed filters
  used -- number of filters already used
  ps_errono - error number

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
int ds_ipfltr_stats
(
  ds_fwk_client_type                      client_type,
  uint8                                   client_handle,
  ds_ipfltr_client_id_enum_type           filter_client,
  uint32                                * max_allowed,
  uint32                                * used,
  int16                                 * ps_errno
)
{
  ds_fwk_clnt_info_s_type clnt_info_ptr;
  memset(&clnt_info_ptr,0x00,sizeof(ds_fwk_clnt_info_s_type));
  ds_fwk_get_clnt_info_from_handle(client_type,client_handle,&clnt_info_ptr);
  return ds_ipfltri_stats(clnt_info_ptr.fw_inst,
                          clnt_info_ptr.ip_type,
                          filter_client,
                          max_allowed,
                          used,
                          ps_errno);
  
}

/*===========================================================================
FUNCTION DS_FWK_EXECUTE_PDN_SHARING_FILTERS()

DESCRIPTION
  Executes the PDN sharing filters
  
DEPENDENCIES
  fwk_inst  : Framework Instance
  pi_ptr    : Pkt info pointer which contains packet meta info

PARAMETERS
  None.

RETURN VALUE
  Uint32  

SIDE EFFECTS
  None.

===========================================================================*/
uint32 ds_fwk_execute_pdn_sharing_filters
(
 ds_fwk_s_type          *fwk_inst,
 ip_pkt_info_type		*pi_ptr
)
{

  ds_ipfltr_result_type       filter_result = DS_IPFLTR_NOMATCH;  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  filter_result = ds_ipfltr_execute(fwk_inst->ds_fwk_index ,
                                    FLTR_CLIENT_PDN_SHARING, 
                                    DS_FWK_IPFLTR_SUBSET_ID_DEFAULT,
                                    pi_ptr);  
  DS_MSG1(MSG_LEGACY_HIGH,"ds_fwk_execute_pdn_sharing_filters filter %d",filter_result);
  return filter_result;
}

