#ifndef PS_IP4_H
#define PS_IP4_H
/*===========================================================================

                   I P V 4    H E A D E R    F I L E

DESCRIPTION
 This file contains definitions and function prototypes specific to IPv4.

Copyright (c) 1995-2020 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ip.h_v   1.1   30 Aug 2002 20:59:24   omichael  $
  $Header: //components/rel/dataiot.mpss/2.0/protocols/inet/inc/ps_ip4.h#2 $ $DateTime: 2020/04/15 22:43:35 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/14/20    ds     Code changes for FRAG support in DUN+AP call concurrency.
09/28/18    ds     Code changes for IOT.
09/12/08    pp     Metainfo optimizations.
03/29/07    rt     Removed IP_MAXTTL. Use IP_DEF_TTL instead.
03/01/05    sv     Added support to pass metainfo to IP layer on rx_path
06/11/04    vp     Removed inclusion of ps_ip_defs.h. Included files ps_ip.h
                   and ps_in.h. Definition of constants IP_MAXTTL, TLB, 
                   IP_CS_OLD, IP_CS_NEW. Changes for representation of IP 
                   addresses as struct ps_in_addr
04/21/04    usb    Passing ptr instead of ref ptr to metainfo thru the stack.
03/16/04    mct    Renamed acl_ipfilter_info_type to ip_pkt_info_type.
03/02/04    rc     Removed tx interface cache entry from ip4_send() signature.
01/09/04    rc     Created File.

===========================================================================*/


/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "customer.h"       /* Customer Specific Features */
#include "comdef.h"


#include "ps_ip.h"
#include "ps_in.h"
#include "ds_fwk.h"

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION IP4_SEND()

DESCRIPTION
  This function is used by the upper layers (UDP and TCP) to send
  an IPv4 datagram. 
  (Modeled after the example interface on p 32 of RFC 791.)

DEPENDENCIES
  None

RETURN VALUE
  0 for success and -1 for failure (ps_errno contains error code)

SIDE EFFECTS
  None
===========================================================================*/
extern int ip4_send
(
  ip_cb_type         *ip_cb,          /* IP control block                  */
  struct ps_in_addr  dest,            /* Destination address               */
  uint8              protocol,        /* Protocol                          */
  dsm_item_type*     send_dg_ptr,     /* Pointer to incoming datagram      */
  uint16             length,          /* Optional length of data portion   */
  ps_tx_meta_info_type* meta_info_ptr, /* Metainfo about packet             */
  int16*             ps_errno         /* Error code for failure case       */
);


/*===========================================================================
FUNCTION IP4_ROUTE()

DESCRIPTION
  This function determines which interface a pecket to send on
  given its IPv4 address info and any policy info.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
int ip4_route
(
  ip_pkt_info_type*        ip_pkt_info_ptr,   /* Pkt and policy info       */
  ps_tx_meta_info_type*    meta_info_ptr,     /* Meta info */
  dsm_item_type**          ip_pkt_ptr,        /* Input packet              */
  ds_fwk_s_type*           tx_if_cache,       /* cached Tx route           */
  int16*                   ps_errno           /* Error code for faillure   */
);

/*===========================================================================

FUNCTION IP4_INPUT

DESCRIPTION
  This function accepts received IPv4 packet(-fragment)s from network
  interfaces and applies ipfilter for the DMSS inbound ACL.
  It then forwards the packet to ip_route for local delivery
  respectively forwarding.

DEPENDENCIES
  None

RETURN VALUE
  0 for success, -1 for failure

SIDE EFFECTS
  None
===========================================================================*/
int ip4_input
(
  ds_fwk_s_type *ds_fwk_inst_ptr,  /* Input interface */
  struct dsm_item_s  *bp,         /* Input packet */
  uint8              *source,     /* Media source address, if any */
  int                 broadcast,  /* Packet had link broadcast address */
  ps_rx_meta_info_type *meta_info_ptr, /* RX meta info ptr                */
  boolean              is_concurrency_on, /* Boolean for concurreny check */
  dsm_item_type        **pkt_ptr           /* Reassembled output packet   */
);


#endif /* PS_IP4_H */
