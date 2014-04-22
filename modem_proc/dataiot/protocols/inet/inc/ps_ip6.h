#ifndef PS_IP6_H
#define PS_IP6_H
/*===========================================================================

                                P S _ I P 6 . H

DESCRIPTION
 This file contains definitions and function prototypes specific to IPv6.

Copyright (c) 1995-2020 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ip.h_v   1.1   30 Aug 2002 20:59:24   omichael  $
  $Header: //components/rel/dataiot.mpss/2.0/protocols/inet/inc/ps_ip6.h#2 $ $DateTime: 2020/04/15 22:43:35 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/14/20    ds     Code changes added for FRAG support in DUN+AP PDN sharing 
                   use case..
09/28/18    ds     Code changes for IOT.
09/12/08    pp     Metainfo optimizations.
04/18/05    mct    Added ip6_route() to support v6 loopback.
03/01/05    sv     Added support to pass metainfo to IP layer on rx_path
06/11/04    vp     Removed inclusion of ps_ip_defs.h and included ps_ip.h
04/05/04    sv     Featurized the header file.
03/02/04    rc     Removed tx interface cache entry from ip6_send() signature.
01/09/04    rc     Initial Version

===========================================================================*/


/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"       /* Customer Specific Features */
#include "comdef.h"
#if defined (FEATURE_DATA_PS) && defined (FEATURE_DATA_PS_IPV6)



#include "ps_ip.h"
#include "ds_fwk.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/



/*===========================================================================

FUNCTION IP6_SEND()

DESCRIPTION
  This function is used by the upper layers (TCP/UDP/ICMP) to send
  an IPv6 datagram. 

DEPENDENCIES
  None

RETURN VALUE
  0 for success and -1 for failure (ps_errno contains error code)

SIDE EFFECTS
  None
===========================================================================*/
int ip6_send
(
  ip_cb_type         *ip_cb,          /* IP control block                  */
  struct ps_in6_addr dest,            /* Destination address               */
  uint8              protocol,        /* Protocol                          */
  dsm_item_type*     send_dg_ptr,     /* Pointer to incoming datagram      */
  uint16             length,          /* Optional length of data portion   */
  ps_tx_meta_info_type* meta_info_ptr, /* Metainfo about packet             */
  int16*             ps_errno         /* Error code for failure case       */
);

/*===========================================================================

FUNCTION IP6_INPUT()

DESCRIPTION
  This function accepts received IPv6 packet(-fragment)s from network
  interfaces and applies ipfilter for the DMSS inbound ACL.
 
DEPENDENCIES
  None

RETURN VALUE
  0 for success, -1 for failure

SIDE EFFECTS
  None
===========================================================================*/
int ip6_input
(
  ds_fwk_s_type *ds_fwk_inst_ptr,           /* Input interface                   */
  dsm_item_type *bp,                  /* Input packet                      */
  uint8         *source,              /* Media source address, if any      */
  int            broadcast,           /* Packet had link broadcast address */
  ps_rx_meta_info_type * meta_info_ptr,   /* Meta info pointer             */
  boolean              is_concurrency_on, /* Boolean for concurreny check  */
  dsm_item_type        **pkt_ptr,            /* Reassembled output packet  */
  uint16               *v6_offset          /* Offset of the packet(v6)     */
);

#endif /* FEATURE_DATA_PS || FEATURE_DATA_PS_IPV6 */
#endif /* PS_IP6_H */
