/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        I P v 4    S O U R C E     F I L E

GENERAL DESCRIPTION
 Upper half of IP, consisting of send/receive primitives,
  Not needed when running as a standalone gateway.
  Copyright 1991 Phil Karn, KA9Q
  Additional material Copyright 1992 William Allen Simpson
  fragment reassembly removed.


EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 1995-2021 QUALCOMM Technology Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ip.c_v   1.8   19 Feb 2003 14:33:04   ubabbar  $
  $Header: //components/rel/dataiot.mpss/2.0/protocols/inet/src/ps_ip4.c#10 $ $DateTime: 2021/06/22 06:10:32 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/22/21    ds     Code changes to fix double free of meta info ptr in case of
                   UL packet is not sent due to flow disabled.
04/07/21    ds     Code changes to use correct structure data type to fetch
                   CLAT SM pointer from CLAT PDN context.
03/23/21    ds     Code changes to free RX META INFO ptr incase clat 
                   translation fails.
12/30/20    ds     Added XLAT feature.
07/29/20    sr     Changes to convert IPv4 source address to IPv6 format and 
                   use Ipv6 to Ipv4 conversion
03/07/20    sr     Fill correct Ipv4 address when IPCB has a valid address set
04/17/20    ds     Code changes to use MUTILS to efficiently utilize DSM items.
04/14/20    ds     Code changes for FRAG and ICMP support in DUN+AP call 
                   concurrency.
02/04/20    sr     Added handling to drop packet in case path MTU is 0
01/27/20    ds     Enhanced defense mechanism to validate IPSEC SA ptr.
01/02/20    ds     Updated the logic to reset the IPSEC info within RT meta info
                   while sending packet in UL.
08/29/19    ds     Code changes to allocate memory for meta info ex ptr only in
                   case of non- frag scenarios.
07/03/19    ds     Code changes to reset the IPSEC info within meta info ptr
                   to make sure that IPSEC filters are executed.
05/15/19    ds     Code changes to free meta info allocated in case of any
                   failures while processing the packet.
04/02/19    ds     Fixed Compiler warnings.
08/13/18    ds     Code changes as per new FWK design for IOT.
12/09/14    ds     Fixed KW errors.
03/12/14    ss     Changes to generate IPSEC info in IP layer.
11/20/13    fn     Update pkt info after parsing secip hdr and route pkt to
                   IPSec iface if it is the next iface
11/11/13    rk     Removed feature mobileap.
04/23/13    ash    Set Qos filter result only if it is not NOMATCH.
02/25/13    op     Fix to reset the IP filter in ip4_input appropriately
02/11/13    ds     Added do_reassembly validation before doing ipsec 
                   processing.
04/06/12    pgm    Resetting filter result and filter mask for socket client
                   in ip4_input().
03/19/12    ss     Drop IP pkts with IP hdr length greater than or equal to
                   dsm item length.
12/02/11    ss     Added index validation while accessing IPSEC iface list
                   array.
05/18/11    pp     Pkt's DestIP, IFACE addr validation moved to embedded pkts
                   to support LTE Mode-B.
03/16/11    ss     ip4_input: Move the check for local packet to the last.
03/10/11    cp     ip4_send: fragment offset field is initialized to 0.
02/24/11    cp     Fixed compiler warnings.
02/22/11    cp     Generate PktInfo in IP layer, if its not generated.
01/07/11    cp     Made changes to calculate the path mtu value used for
                   IP packet fragmentaion, using ps_ip_calc_path_mtu() API.
12/23/10    ss     Cleaning up of Globals for thread safety in Q6 Free
                   Floating environment.
10/18/10    pp     Generate transport protocol information, and set
                   is_pkt_info_valid to TRUE to avoid Pkt Info re-generation.
10/01/10    sa     Added appropriate log messages before ASSERT(0).
04/18/10    ss     KW fixes.
11/14/09    ss     IP Multicast feature addition.
11/14/09    sp     Removed dependency on internal ipsec structure.
09/24/09    ss     KW warning fixes.
05/22/09    kk     Q6 compiler warning fixes.
04/14/09    sp     ip4_send: If the IP address type is invalid, the packet
                   is dropped.
03/27/09    ssh    ip4_send: Fixed possible NULL dereference
09/12/08    pp     Metainfo optimizations.
06/26/07    rs     Added code to use Multicast TTL value if the address is
                   multicast. Also added code to support router alert option.
04/25/07    vp     Using meta_info's pkt_info_ptr in ip4_send
03/29/07    rt     Removed IP_MAXTTL. Use IP_DEF_TTL instead.
02/09/07    rt     Initialized local var errno in ip4_input()
02/06/07    scb    Fixed Klockwork Critical and high errors in ip4_input()
04/20/06    mct    Optimized IPv4 loopback processing.
04/19/06    rt     Modified ip4_send to take generic ip pkt info in
                   secipsah_hdr_create().
03/28/06    msr    Fixed compilation error caused when LOG_MSG_INFO3 is compiled
09/26/05    msr    Fixed ip4_send() to not populate IPCB's metainfo if it is
                   NULL
08/15/05    mct    Fixed naming causing some issues w/C++ compilers.
08/15/05    vp     Allow packets if the dst_addr does no match the iface IP
                   addr only if iface state is CONFIGURING and iface IP addr
                   is zero.
08/12/05    kr     Removed existing stats code and added code for updating
                   the new statistics framework
08/05/05    sv     Genreate meta_info and cache it in IPCB if metainfo is NULL.
                   so that filtering results done at lower layers get cached.
05/11/05    sv     Lint changes.
04/22/05    ks     Fixed checking of IPV4 version no. in ip4_input.
04/20/05    sv     optimize ip4_input to use passed pkt information.
04/19/05    mct    Only check for loopback if iface is a v4 iface.
04/19/05    jd     Reverted ip4_route to only generate metainfo for broadcast
                   interfaces to optimize non-broadcast iface performance.
04/18/05    vp     Verify that the address of the input iface is equal to
                   dest addr of the incoming packet in ip4_input().
03/15/05    lyr    Change the byte ordering for next hop addr meta info in
                   ipv4_route to host byte order - ARP currently expects this
03/10/05    jd     Ensure ipv4_route adds metainfo for non-broadcast
                   interfaces to ensure correct operation with lan_llc
03/02/05    vp     Support for ICMP error generation.
01/26/05    sv     Renamed esp/ah header parsing functions.
01/10/05    sv     Merged IPSEC changes.
01/06/05    msr    Not freeing meta info and DSM item in ip4_route() when
                   ps_iface_tx_cmd() fails.
12/27/04    ifk    Replaced setting of tracer field with DSM_TOUCH_ITEM
12/23/04    lyr    Added meta info population in ip4_route() for bcast ifaces
12/07/04    msr    Updating IP header with user provided TOS.
10/30/04    ks     Changed ptr to ptr of ps_meta_info_type to single pointer
                   in the call to lo_tx_cmd().
10/15/04    ifk    Added call to fragmentation routine in ip4_route()
10/13/04    vp     Removed inclusion of ps_iphdr.h and included ps_ip4_hdr.h.
                   Changes due to restructuring of struct ip and replacement
                   of ntohip() and htonip() with ps_ip4_hdr_parse() and
                   ps_ip4_hdr_create().
08/30/04    usb    Fixed if check in ip4_route() to loopback to vaid iface.
07/22/04    mct    Fixed issue seen when compiling with RVDS 21 compiler.
07/01/04    ifk    Removed setting of DSM kind field.
06/11/04    vp     Removed inclusion of psglobal.h and internet.h. Included
                   files ps_pkt_info.h and ps_stas.h.Changes for representation
                   of IP addresses as struct ps_in_addr or struct ps_in6_addr.
                   Replaced use of TCP_PTCL etc with PS_IPPROTO_TCP etc.
                   Replaced use of IPVERSION with IP_V4.
04/30/04    vp     Removed FEATURE_PS_UNSUPPORTED and changes for that.
04/30/04    vp     Code for correct handling of packets destined to itself.
04/30/04    usb    Passing IP pkt up the stack without stripping the IP hdr.
04/21/04    usb    Passing ptr instead of ref ptr to metainfo thru the stack.
04/16/04    sv     Removed assert in ip4_send when we get an invalid IP address
                   from the ps_iface.
03/16/04    mct    Renamed acl_ipfilter_info_type to ip_pkt_info_type.
03/02/04    rc     Removed tx interface cache entry from ip4_send() signature.
                   Fixed ip4_send() to always use the routing cache from ip_cb.
01/09/04    rc     Initial Version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */

#include "msg.h"
#include "err.h"
#include "amssassert.h"
#include "memory.h"
#include "IxErrno.h"

#include "dsm.h"

#include "ps_tcp.h"
#include "ps_udp_io.h"
#include "ps_icmp.h"

#include "ps_ip4.h"
#include "ps_iputil.h"
#include "ps_ip4_hdr.h"
#include "ps_pkt_info.h"
#include "ps_ip4_frag.h"
#include "ps_icmp_error.h"
#include "ps_pkt_info_utils.h"
#include "ps_ip_fraghdl.h"

#ifdef FEATURE_SEC_IPSEC
#include "secips.h"
#endif

#include "ds_Utils_DebugMsg.h"
#include "ps_pkt_info_utils.h"
#include "ds_fwk.h"
#include "ps_metai_info.h"
#ifdef FEATURE_DATA_PS_464XLAT
#include "ds_fwki.h"
#include "ps_clat_sm.h"
#include "ps_clat.h"
#endif /* #ifdef FEATURE_DATA_PS_464XLAT */


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*-------------------------------------------------------------------------*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/



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
int ip4_send
(
  ip_cb_type         *ip_cb,          /* IP control block                  */
  struct ps_in_addr  dest,            /* Destination address               */
  uint8              protocol,        /* Protocol                          */
  dsm_item_type*     send_dg_ptr,     /* Pointer to incoming datagram      */
  uint16             length,          /* Optional length of data portion   */
  ps_tx_meta_info_type* meta_info_ptr, /* Metainfo about packet             */
  int16*             ps_errno         /* Error code for failure case       */
)
{
  static uint16 Id_cntr = 0;
  ip_pkt_info_type   *ip_pkt_info;         /* IP info (hdr + policiy info ) */
  ps_ip_addr_type ip_addr;
#ifdef FEATURE_SEC_IPSEC
  void* esp_sa_ptr = NULL;
  void* ah_sa_ptr = NULL;
  ip_pkt_info_type temp_ip_pkt_info;
  ps_ipsec_info_type ipsec_info;
  int result = 0;
#endif /* FEATURE_SEC_IPSEC */
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Check for UDP or ICMP packets which contain per-packet routing info
  -------------------------------------------------------------------------*/
  if ( ip_cb == NULL )
  {
    dsm_free_packet(&send_dg_ptr);
    PS_TX_META_INFO_FREE(&meta_info_ptr);
    LOG_MSG_ERROR_0(" ip4_send(): ip_cb ptr is NULL, Dropping the pkt");
    DS_ASSERT(0);
    return -1;
  }

  if(meta_info_ptr == NULL)
  {
    if(ip_cb->meta_info_ptr != NULL)
    {
      PS_TX_META_INFO_DUP(ip_cb->meta_info_ptr, &meta_info_ptr);
      /*---------------------------------------------------------------------
        Once Meta info ptr is DUPed from saved IP control block then reset 
        the IPSEC info to NULL so that any previous IPSEC info is set to 
        NULL and IPSEC filters are executed to identify whether packet needs
        to be encrypted or not. 
      ---------------------------------------------------------------------*/
      memset(&ipsec_info, 0x0, sizeof(ps_ipsec_info_type));
      PS_TX_META_SET_IPSEC_INFO(meta_info_ptr, ipsec_info);
    }
    else
    {
      /*---------------------------------------------------------------------
        Don't populate IPCB's metainfo as there are couple of cases, where
        IPCB passed in to this function is not part of TCB, or UCB etc. If
        it is part of TCB, or UCB, its metainfo would have already been
        populated
      ---------------------------------------------------------------------*/
      PS_TX_META_INFO_AND_RT_META_INFO_GET(meta_info_ptr);
      if(meta_info_ptr != NULL &&
         PS_TX_META_GET_RT_META_INFO_PTR(meta_info_ptr) != NULL)
      {
        PS_TX_META_SET_ROUTING_CACHE(meta_info_ptr, ip_cb->routing_cache);
      }
    }
  }

  /* Meta info cannot be NULL here! */
  if((meta_info_ptr == NULL) ||
     (PS_TX_META_GET_RT_META_INFO_PTR(meta_info_ptr) == NULL))
  {
    dsm_free_packet(&send_dg_ptr);
    PS_TX_META_INFO_FREE(&meta_info_ptr);
    LOG_MSG_ERROR_0("ip4_send(): meta info ptr is NULL, Dropping the pkt");
    DS_ASSERT(0);
    return -1;
  }

  ip_pkt_info = &PS_TX_META_GET_PKT_INFO(meta_info_ptr);

  /* Moved it above IPSEC generation to fetch source address */
  /*-----------------------------------------------------------------------
    Retrieve the new IP addr from psiface.
  -----------------------------------------------------------------------*/
  if(PS_IN6_IS_ADDR_UNSPECIFIED(&ip_cb->src_ip_addr))
  {
    /*-------------------------------------------------------------------------
      Get the source address if src_ip_addr is NULL. This must be done for
      ICMP messages.
    -------------------------------------------------------------------------*/
    ip_addr.type = IPV4_ADDR;
    ds_fwk_get_ip_addr(ip_cb->routing_cache, IPV4_ADDR , &ip_addr);
    if(ip_addr.type == IP_ADDR_INVALID)
    {
      LOG_MSG_ERROR_0("IP Address Invalid, Dropping the packet");
      dsm_free_packet(&send_dg_ptr);
      PS_META_INFO_FREE(&meta_info_ptr);
      return -1;
    }
    else
    {
      ip_pkt_info->ip_hdr.v4.source = ip_addr.addr.v4;
    }
  }
  else
  {
    ip_pkt_info->ip_hdr.v4.source.ps_s_addr =
      PS_IN6_GET_V4_FROM_V4_MAPPED_V6(&ip_cb->src_ip_addr);
  }

#ifdef FEATURE_SEC_IPSEC
  /* Generate IPSEC info if not yet generated */
  if( PS_TX_META_GET_IPSEC_INFO(meta_info_ptr).esp_sa_ptr == NULL &&
      PS_TX_META_GET_IPSEC_INFO(meta_info_ptr).ah_sa_ptr == NULL )
  {
    /* Populate temp ip pkt info with IP hdr details */
    memset( &temp_ip_pkt_info, 0, sizeof( temp_ip_pkt_info ) );

    temp_ip_pkt_info.ip_vsn = IP_V4;
    temp_ip_pkt_info.ip_hdr.v4.dest.ps_s_addr = dest.ps_s_addr;
    temp_ip_pkt_info.ip_hdr.v4.source.ps_s_addr = 
      ip_pkt_info->ip_hdr.v4.source.ps_s_addr;
    
    /* Generate IPSEC info */
    ps_ip_udpate_ipsec_info_in_metainfo( meta_info_ptr, send_dg_ptr, 
                                         &temp_ip_pkt_info, protocol );
  }

  /* Get esp_sa_ptr, ah_sa_ptr from Meta info */
  esp_sa_ptr = PS_TX_META_GET_IPSEC_INFO(meta_info_ptr).esp_sa_ptr;
  ah_sa_ptr = PS_TX_META_GET_IPSEC_INFO(meta_info_ptr).ah_sa_ptr;

  /*-------------------------------------------------------------------------
    Add ESP header.
  -------------------------------------------------------------------------*/
  if(esp_sa_ptr != NULL && send_dg_ptr != NULL)
  {
    ip_pkt_info->ip_hdr.v4.protocol = PS_IPPROTO_ESP;
    ip_pkt_info->ip_vsn = IP_V4;

    result = secipsesp_hdr_create(esp_sa_ptr,
                                  &protocol,
                                  &send_dg_ptr);
								
    if( 0!= result )
    {
      LOG_MSG_ERROR_0("secipsesp_hdr_create returned -1. Dropping packet");
      dsm_free_packet(&send_dg_ptr);
      PS_TX_META_INFO_FREE(&meta_info_ptr);
      return -1;
    }

    if(length != 0)
    {
      length = (uint16)dsm_length_packet(send_dg_ptr);
    }

    /* ----------------------------------------------------------------------
      Update ESP Protocol Pkt information into ip_pkt_info.
    ------------------------------------------------------------------------*/
    if (-1 == ps_transport_ptcl_pkt_info_generate ( send_dg_ptr,
                                                    ip_pkt_info,
                                                    PS_IPPROTO_ESP,
                                                    0 /* dsm_offset */
                                                   )
       )
    {
      LOG_MSG_ERROR_0("transport_ptcl_pkt_info_gen() returned -1");
      dsm_free_packet(&send_dg_ptr);
      PS_TX_META_INFO_FREE(&meta_info_ptr);
      return -1;
    }

    /*-------------------------------------------------------------------------
      QoS filter execution would happen at iface layer.
    -------------------------------------------------------------------------*/
  }  
#endif

  /*-------------------------------------------------------------------------
    Use implicit length of DSM item chain
  -------------------------------------------------------------------------*/
  if(length == 0 && send_dg_ptr != NULL)
  {
    length = (uint16)dsm_length_packet(send_dg_ptr);
  }

  if (ip_cb->ttl == 0)
  {
    ip_cb->ttl = IP_DEF_TTL;
  }

  if (ip_cb->mcast_ttl == 0)
  {
    ip_cb->mcast_ttl = IP_DEF_MCAST_TTL;
  }

  /*-------------------------------------------------------------------------
    Fill in IP header
  -------------------------------------------------------------------------*/
  ip_pkt_info->ip_vsn                  = IP_V4;
  ip_pkt_info->ip_hdr.v4.version       = IP_V4;
  ip_pkt_info->ip_hdr.v4.length        = IPLEN + length;

  ip_pkt_info->ip_hdr.v4.id            = Id_cntr++;

  ip_pkt_info->ip_hdr.v4.off           = 0;
  ip_pkt_info->ip_hdr.v4.offset        = 0;
  ip_pkt_info->ip_hdr.v4.mf            = 0;

  /*-------------------------------------------------------------------------
    Update MCAST TTL - if the address is multicast address.
  -------------------------------------------------------------------------*/
  if( PS_IN_IS_ADDR_MULTICAST(ip_pkt_info->ip_hdr.v4.dest.ps_s_addr) )
  {
    if(ip_cb->mcast_ttl_set == TRUE)
    {
      ip_pkt_info->ip_hdr.v4.ttl           = ip_cb->mcast_ttl;
    }
    else
    {
      ip_pkt_info->ip_hdr.v4.ttl = IP_DEF_MCAST_TTL;
    }
  }
  else
  {
    ip_pkt_info->ip_hdr.v4.ttl           = ip_cb->ttl;
  }

  if (ip_cb->ip_cb_router_alert == TRUE)
  {
    ip_pkt_info->ip_hdr.v4.router_alert = TRUE;
  }
  else
  {
    ip_pkt_info->ip_hdr.v4.router_alert = FALSE;
  }

  ip_pkt_info->ip_hdr.v4.tos           = ip_cb->tos;
  ip_pkt_info->ip_hdr.v4.df            = ip_cb->df;

  ip_pkt_info->ip_hdr.v4.congest       = 0;
  ip_pkt_info->ip_hdr.v4.protocol      = protocol;
  ip_pkt_info->ip_hdr.v4.dest          = dest;
  ip_pkt_info->ip_hdr.v4.optlen        = 0;

  /*-------------------------------------------------------------------------
    Set filter interface to NULL indicating it's a locally
    generated packet.
  -------------------------------------------------------------------------*/
  ip_pkt_info->if_ptr = NULL;

#ifdef FEATURE_SEC_IPSEC
  if(ah_sa_ptr != NULL)
  {
    ip_pkt_info->ip_hdr.v4.protocol = protocol = PS_IPPROTO_AH;
    ip_pkt_info->ip_hdr.v4.length += sizeof(secips_ah_hdr);
    (void ) secipsah_hdr_create(ah_sa_ptr,
                                protocol,
                                (*ip_pkt_info),
                                &send_dg_ptr);
    /* ----------------------------------------------------------------------
      Update AH Protocol Pkt information into ip_pkt_info.
    ------------------------------------------------------------------------*/
    if (-1 == ps_transport_ptcl_pkt_info_generate ( send_dg_ptr,
                                                    ip_pkt_info,
                                                    protocol,
                                                    0 /* dsm_offset */
                                                   )
       )
    {
      LOG_MSG_ERROR_0("transport_ptcl_pkt_info_gen() returned -1");
      dsm_free_packet(&send_dg_ptr);
      PS_TX_META_INFO_FREE(&meta_info_ptr);
      return -1;
    }

    /*-------------------------------------------------------------------------
      QoS filter execution would happen at iface layer.
      -------------------------------------------------------------------------*/
  }
#endif

  /* ----------------------------------------------------------------------
    Update Transport Protocol Pkt information into ip_pkt_info, to avoid
    re-generation of ip_pkt_info in IFACE I/O routines dueto filtering.
  ------------------------------------------------------------------------*/
  if (-1 == ps_transport_ptcl_pkt_info_generate ( send_dg_ptr,
                                                  ip_pkt_info,
                                                  protocol,
                                                  0 /* dsm_offset */
                                                 )
     )
  {
    LOG_MSG_ERROR_0("transport_ptcl_pkt_info_gen() returned -1");
    dsm_free_packet(&send_dg_ptr);
    PS_TX_META_INFO_FREE(&meta_info_ptr);
    return -1;
  }

  /* ----------------------------------------------------------------------
    Update is_pkt_info_valid flag to TRUE. This will make sure  to avoid pkt_info
    re-generation.
  ------------------------------------------------------------------------*/
  ip_pkt_info->is_pkt_info_valid = TRUE;

  /*-------------------------------------------------------------------------
    Fill in IP header in network byte order. NOTE: The passed in 'send_dg_ptr'
    field may be changed to account for IP header bytes needed.
  -------------------------------------------------------------------------*/
  if(ps_ip4_hdr_create_ex( &send_dg_ptr, &(ip_pkt_info->ip_hdr.v4), IP_CS_NEW)
      == FALSE)
  {
    dsm_free_packet(&send_dg_ptr);
    PS_TX_META_INFO_FREE(&meta_info_ptr);
    return -1;
  }
  if (send_dg_ptr == NULL)
  {
    return 0;
  }

  /*------------------------------------------------------------------------
    Removed the tcp_ppp_wm - so directly calling function instead of queueing
    item up in a watermark.
  -------------------------------------------------------------------------*/
  DSM_TOUCH_ITEM(send_dg_ptr);

  /*-------------------------------------------------------------------------
    Find out which interface this should go to
  -------------------------------------------------------------------------*/
  return ip4_route( ip_pkt_info,
                    meta_info_ptr,
                    &send_dg_ptr,
                    ip_cb->routing_cache,
                    ps_errno );

} /* ip4_send() */


/*===========================================================================

FUNCTION IP4_INPUT()

DESCRIPTION
  This function accepts received IP packet(-fragment)s from network
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
  ds_fwk_s_type *ds_fwk_inst_ptr,           /* Input interface                   */
  dsm_item_type *bp,                  /* Input packet                      */
  uint8         *source,              /* Media source address, if any      */
  int            broadcast,           /* Packet had link broadcast address */
  ps_rx_meta_info_type *meta_info_ptr,     /* Meta info ptr                */
  boolean               is_concurrency_on, /* Boolean for concurreny check */
  dsm_item_type        **pkt_ptr           /* Reassembled output packet    */
)
{
  ip_pkt_info_type       *pkt_info_ptr;      /* IP header being processed  */
  uint16                  ip_len;            /* IP header length */
  uint8                  ip_ver;             /* IP version                 */
  errno_enum_type        error_num = E_SUCCESS;  /* PS errno               */
  uint16                 offset = 0;        /* current offset to proto hdr */
  uint8                  buf[8];            /* temp buffer for extracting  */
  boolean                do_reassembly = TRUE;  
  ds_fwk_s_type          *saved_fwk_ptr = NULL;
  void                  *fraghdl_ptr;
  (void)source;
  (void)broadcast;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*---------------------------------------------------------------------
    Get and verify IP version of packet
  ---------------------------------------------------------------------*/
  if (sizeof(uint8) != dsm_extract(bp, 0, &ip_ver, sizeof(uint8)))
  {
    LOG_MSG_ERROR_0("unable to get IP version");
    PS_RX_META_INFO_FREE(&meta_info_ptr);
    dsm_free_packet(&bp);
    return -1;
  }

  if (IP_V4 != (ip_ver >> 4))
  {
    LOG_MSG_ERROR_1("Incorrect IP version : (%d)", (ip_ver >> 4));
    PS_RX_META_INFO_FREE(&meta_info_ptr);
    dsm_free_packet(&bp);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Sneak a peek at the IP header's IHL field to find its length
  -------------------------------------------------------------------------*/
  ip_len = (bp->data_ptr[0] & 0xf) << 2;
  if(ip_len < IPLEN)
  {
    /*-----------------------------------------------------------------------
      The IP header length field is too small
    -----------------------------------------------------------------------*/
    dsm_free_packet(&bp);
    PS_RX_META_INFO_FREE(&meta_info_ptr);
    return -1;
  }

  if(dsm_length_packet(bp) <= ip_len)
  {
    /*-----------------------------------------------------------------------
      The packet is shorter than or equal to header length.
      Even packets equal to IP hdr must be dropped. No payload to be passed
      on to upper protocols. Better to drop here itself.
    -----------------------------------------------------------------------*/
    dsm_free_packet(&bp);
    PS_RX_META_INFO_FREE(&meta_info_ptr);
    return -1;
  }

  if(cksum(NULL,bp,ip_len, 0) != 0)
  {
    /*-------------------------------------------------------------------------
      Bad IP header checksum; discard
    -------------------------------------------------------------------------*/
    dsm_free_packet(&bp);
    PS_RX_META_INFO_FREE(&meta_info_ptr);
    return -1;
  }

  /*-----------------------------------------------------------------------
      Generate packet information.
    -----------------------------------------------------------------------*/
  if( meta_info_ptr == NULL ||
      PS_RX_META_GET_PKT_INFO(meta_info_ptr).is_pkt_info_valid == FALSE )
  {
    if( ps_rx_pkt_info_generate(&bp,&meta_info_ptr, &error_num, FALSE)== -1)
    {
      PS_RX_META_INFO_FREE(&meta_info_ptr);
      if(error_num == E_WOULD_BLOCK)
      {
        return 0;
      }
      dsm_free_packet(&bp);
      return -1;
    }
  }

  pkt_info_ptr = &PS_RX_META_GET_PKT_INFO(meta_info_ptr);
  pkt_info_ptr->if_ptr = ds_fwk_inst_ptr;

  /*---------------------------------------------------------------------
     Get the saved iface from fragmentation queue to compare with 
     current context iface.
  ---------------------------------------------------------------------*/

  fraghdl_ptr = pkt_info_ptr->fraghdl;

  saved_fwk_ptr = ps_ip_fraghdl_get_fwk_instance(fraghdl_ptr);

  if( NULL != saved_fwk_ptr )
  {
    if( (PS_RX_META_GET_FILTER_RESULT(meta_info_ptr, FLTR_CLIENT_SOCKETS) != 
         DS_IPFLTR_NOMATCH) &&
        (saved_fwk_ptr != ds_fwk_inst_ptr) )
    {
      PS_RX_META_RESET_FILTER_RESULT(meta_info_ptr,
                                     (int)FLTR_CLIENT_SOCKETS);
    }
  }

  /*-------------------------------------------------------------------------
    Check if we received a fragment.
    The following table shows what should be done for handling fragments:

    ip.flags.mf   ip.offset    Comments
    0              0           This is a complete datagram with no fragments.
                               So, no need to handle these fragments.
    1              0           We received the starting fragment of the
                               datagram. Handle it. Store the ip header of
                               this fragment and pass it to upper layer.
    0              !0          We received the last fragment of the datagram
                               Handle it. Use this to calculate the total
                               length of the incoming datagram.
    1              !0          We received some intermittent fragment of the
                               datagram. Handle it.

    Note that following cases are not taken care by the implementation:
    1. We donot handle any overlapping of the data in different fragments.
       It is left to upper layers to thrash it off.
    2. We donot have any time outs for fragments. If we donot receive any
       fragment of a datagram, we wait until the entire reassembly array is
       full. We donot time out , but leave it to the application to take
       care of it.
  -------------------------------------------------------------------------*/
  if ( do_reassembly && pkt_info_ptr->fraghdl != NULL )
  {
    /*-----------------------------------------------------------------------
      Received IP fragments process them before giving it to other layers
    -----------------------------------------------------------------------*/
    DSM_TOUCH_ITEM(bp);
    ip_len = 0;
    if((bp = ip4_reassemble(pkt_info_ptr, bp, &ip_len, is_concurrency_on)) == NULL)
    {
      PS_RX_META_INFO_FREE(&meta_info_ptr);
      *pkt_ptr = NULL;
      return 0;
    }
    /* Check if concurrency is ON and packet is reassembled,then return the
       packet to framework so that it can decide where to route the packet
       instead of packet getting routed to local stack. 
    */
    else if(bp != NULL && is_concurrency_on)
    {
      *pkt_ptr = bp; 
      return 0;
    }
  }

  ip_len = pkt_info_ptr->ip_hdr.v4.optlen+IPLEN;
  offset = ip_len;

#ifdef FEATURE_SEC_IPSEC
  /* skip sec processing, if the packet is outbound! */
  if (do_reassembly)
  { 
    if(pkt_info_ptr->ip_hdr.v4.protocol == PS_IPPROTO_UDP
       && pkt_info_ptr->esp_hdr.spi != 0)
    {
      /* use offset to ignore UDP hdr, rfc3948 section 3.2*/
      offset += sizeof(udp_hdr_type);
      if (secipsesp_hdr_parse(&bp,&offset,pkt_info_ptr) == -1)
      {
        LOG_MSG_ERROR_0("Error in parsing UDP ENCAPS ESP header");
        dsm_free_packet(&bp);
        PS_RX_META_INFO_FREE(&meta_info_ptr);
        return -1;
      }
    }
    else
    {
      while(pkt_info_ptr->ip_hdr.v4.protocol == PS_IPPROTO_ESP ||
            pkt_info_ptr->ip_hdr.v4.protocol == PS_IPPROTO_AH)
      {
        if(pkt_info_ptr->ip_hdr.v4.protocol == PS_IPPROTO_ESP)
        {

          if( secipsesp_hdr_parse(&bp,&offset,pkt_info_ptr) == -1)
          {
            LOG_MSG_ERROR_0("Error in parsing ESP header");
            dsm_free_packet(&bp);
            PS_RX_META_INFO_FREE(&meta_info_ptr);
            return -1;
          }
        }
        if(pkt_info_ptr->ip_hdr.v4.protocol == PS_IPPROTO_AH)
        {
          if( secipsah_hdr_parse(&bp,&offset,pkt_info_ptr) == -1)
          {
            LOG_MSG_ERROR_0("Error in parsing AH header");
            dsm_free_packet(&bp);
            PS_RX_META_INFO_FREE(&meta_info_ptr);
            return -1;
          }
        }
      }
    }
    /*-----------------------------------------------------------------------
      Update packet info with the protocol specific paramaters like ports
    -----------------------------------------------------------------------*/
    switch (pkt_info_ptr->ip_hdr.v4.protocol)
    {
      case PS_IPPROTO_TCP:
        /* extract src and dst port */
        (void)dsm_extract(bp, offset, buf, 2*sizeof(uint16));

         pkt_info_ptr->ptcl_info.tcp.src_port = *((uint16*)&buf[0]);
         pkt_info_ptr->ptcl_info.tcp.dst_port = *((uint16*)&buf[2]);
         break;

     case PS_IPPROTO_UDP:
       /* extract src and dst port */
       (void)dsm_extract(bp, offset, buf, 2*sizeof(uint16));

       pkt_info_ptr->ptcl_info.udp.src_port = *((uint16*)&buf[0]);
       pkt_info_ptr->ptcl_info.udp.dst_port = *((uint16*)&buf[2]);
       break;

     case PS_IPPROTO_ICMP:
       /* extract type and code fields */
       (void)dsm_extract(bp, offset, buf, 8*sizeof(uint8));

       pkt_info_ptr->ptcl_info.icmp.type = buf[0];
       pkt_info_ptr->ptcl_info.icmp.code = buf[1];
       if (pkt_info_ptr->ptcl_info.icmp.type == ICMP_ECHO_REPLY ||
          pkt_info_ptr->ptcl_info.icmp.type == ICMP_ECHO_REQ )
       {
         /* Store the ID */
         pkt_info_ptr->ptcl_info.icmp.id =  *((uint16*)&buf[4]);
         /* Store the sequence number */
         pkt_info_ptr->ptcl_info.icmp.sequence_num = *((uint16*)&buf[6]);	
       }
       break;

    default:
      /* filter with IP packet information*/
      break;
    }

  }
#endif
  /*-----------------------------------------------------------------------
    Deliver the packet to the local stack
  -----------------------------------------------------------------------*/
  ip_receive(bp, offset, meta_info_ptr);
  if(is_concurrency_on == TRUE)
    *pkt_ptr = bp; 

  return 0;
}  /* ip4_input() */

/*===========================================================================

FUNCTION IP4_ROUTE()

DESCRIPTION
  This function finds the appropriate interface to send a
  given IPv4 packet on and forwards it appropriately.

DEPENDENCIES
  If this function calls another function, say X, then X is responsible for
  freeing up memory in ALL cases.

RETURN VALUE
  0 for success, -1 for failure (ps_errno contains error code)

   ps_errno can contain:
   E_NOTALLOWED - Pkt forwarding is disabled
   E_NOROUTE    - No route to destination could be found
   E_NONET      - Route to destination is cuurently not accessible

SIDE EFFECTS
  None
===========================================================================*/
int ip4_route
(
  ip_pkt_info_type*        ip_pkt_info_ptr,   /* Pkt and policy info       */
  ps_tx_meta_info_type*    meta_info_ptr,     /* Policy info e.g. from SCB */
  dsm_item_type**          ip_pkt_ptr,        /* Input packet              */
  ds_fwk_s_type*           tx_if_cache,       /* cached Tx route           */
  int16*                   ps_errno           /* Error code for faillure   */
)
{
  int             ret_val = 0;
  int16           path_mtu = 0;
  ps_meta_info_type_ex  *meta_info_ex_ptr = NULL;
  boolean         is_meta_valid = TRUE;
#ifdef FEATURE_DATA_PS_464XLAT
  void            *instance = NULL;
  ds_fwk_clat_pdn_cntxt_info_type *clat_pdn_context = NULL;
  uint8           buf[4] = {0};
  uint16          dsm_offset = 0;
  uint16          src_port = 0;
  uint16          dst_port = 0;
  ps_clat_sm_type *clat_sm_ptr = NULL;
#endif
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  do
  {
    /*-----------------------------------------------------------------------
      If packet length is larger then the PATH MTU then try fragmenting
    -----------------------------------------------------------------------*/
    /* Get the path mtu value */
    path_mtu = ps_ip_calc_path_mtu( IPV4_ADDR,
                                    tx_if_cache);
    if ( path_mtu == -1 || path_mtu == 0)
    {
      LOG_MSG_ERROR_2("Path mtu %d on iface 0x%x", path_mtu, tx_if_cache);
      dsm_free_packet( ip_pkt_ptr );
      PS_TX_META_INFO_FREE( &meta_info_ptr );
      ret_val = -1;
      break;
    }

    if(ip_pkt_info_ptr->ip_hdr.v4.length > (uint16)path_mtu )
    {
      if( ip_pkt_info_ptr->ip_hdr.v4.df == 0 )
      {
        /*-------------------------------------------------------------------
          NOTE:  Meta Info and DSM Item are freed by tx_cmd. No need to free
          anything.
        -------------------------------------------------------------------*/
        /*-----------------------------------------------------------------
          Statistics collection : Increment frag_ok if fragmentation is
          successful; else increment frag_fails
        -----------------------------------------------------------------*/

        ret_val = ip4_fragment(ip_pkt_ptr,
                               &ip_pkt_info_ptr->ip_hdr.v4,
                               tx_if_cache,
                               meta_info_ptr,
                               path_mtu,
                               &is_meta_valid);
        if (-1 == ret_val)
        {
          LOG_MSG_ERROR_2("Unable to fragment IP packet, 0x%x, on iface 0x%x",
                          ip_pkt_ptr, tx_if_cache);
          if(is_meta_valid)
            PS_TX_META_INFO_FREE( &meta_info_ptr );				  
          
          PS_META_INFO_FREE_EX( &meta_info_ex_ptr );				  
          break;
        }
      }
      else
      {
        LOG_MSG_INFO1_3("As packet's (0x%x) size (%d) is greater "
                 "than PATH MTU (%d) and DF (don't fragment) bit is set on it, "
                 "packet is dropped",
                 ip_pkt_ptr,
                 ip_pkt_info_ptr->ip_hdr.v4.length,
                 path_mtu);
        dsm_free_packet( ip_pkt_ptr );
        PS_TX_META_INFO_FREE( &meta_info_ptr );
        PS_META_INFO_FREE_EX( &meta_info_ex_ptr );
        ret_val = -1;
        break;
      }
    }
    else
    {
      /*---------------------------------------------------------------------
        Call the interface's write function w/ the next hop's address

        NOTE:  Meta Info and DSM Item are freed by tx_cmd. No need to free
        anything.
      ---------------------------------------------------------------------*/
      /*-----------------------------------------------------------------------
        ds_fwk_input takes ps_meta_info_type_ex as a param. The meta_info_ex_ptr
        is allocated here and freed in ds_fwk_input
      ------------------------------------------------------------------------*/
      if (meta_info_ptr != NULL)
      {
        PS_META_INFO_GET_EX(meta_info_ex_ptr);
        if (meta_info_ex_ptr != NULL)
        {
          meta_info_ex_ptr->tx_meta_info_ptr = meta_info_ptr;
        }
        else
        {
          LOG_MSG_ERROR_0( "Unable to allocate memory for meta_info_ex_ptr");
          dsm_free_packet( ip_pkt_ptr );
          PS_TX_META_INFO_FREE( &meta_info_ptr );
          ret_val = -1;
          break;
        }
      }
	  
#ifdef FEATURE_DATA_PS_464XLAT
	  /* Check if this is CLAT Call then call API to translate the packets
         before giving it to FWK for Tx only if IPv4 protocol is not ESP. */
      if(tx_if_cache->clat_pdn_cntxt != NULL && 
         ip_pkt_info_ptr->ip_hdr.v4.protocol != PS_IPPROTO_ESP)
      {
        clat_pdn_context = (ds_fwk_clat_pdn_cntxt_info_type*)tx_if_cache->clat_pdn_cntxt;
        instance = (void*)clat_pdn_context->client_pdn_handle;

        /* Peek into the IPv4 packet and fetch the Dst port and if it matches the 
           DNS server port then set the boolean accordingly. */
        dsm_offset = IPV4_HEADER_LENGTH_OFFSET;

        (void)dsm_extract(*ip_pkt_ptr, dsm_offset, buf, 2*sizeof(uint16));
        src_port = *((uint16*)&buf[0]);
        dst_port = *((uint16*)&buf[2]);
        LOG_MSG_INFO1_2("ip4_route(): src port = 0x%x, dst port = 0x%x,",src_port, dst_port);
        if(dst_port == PS_DNS_PROXY_SERVER_PORT)
        {
           clat_sm_ptr  = (ps_clat_sm_type*)instance;
            LOG_MSG_INFO1_0( "UL DNS query with XLAT: set the flag ");
           clat_sm_ptr->is_dns_query = TRUE; 
        }
        LOG_MSG_INFO1_1("Translate the packet: CLAT inst 0x%x",instance);
        if(-1 == ps_clat_ip_to_ip6(ip_pkt_ptr, tx_if_cache, instance ))
        {
        
          LOG_MSG_ERROR_0( "Unable to Translate the IPv4 packet");
          dsm_free_packet( ip_pkt_ptr );
          PS_TX_META_INFO_FREE( &meta_info_ptr );
          PS_META_INFO_FREE_EX( &meta_info_ex_ptr );
          ret_val = -1;
          break;
        }
      }
#endif
  
      LOG_MSG_INFO1_1("Sending Packet fw_inst 0x%p",tx_if_cache);
      if(ds_fwk_inst_is_valid_ex(tx_if_cache,IPV4_ADDR) || (tx_if_cache->is_clat_capable && 
         ds_fwk_inst_is_valid_ex(tx_if_cache,IPV6_ADDR)))
      {
          ret_val = ds_fwk_input(tx_if_cache, ip_pkt_ptr, meta_info_ex_ptr);
          if (-1 == ret_val)
          {
            LOG_MSG_ERROR_1("Failed to transmit data on iface 0x%x", tx_if_cache);
            break;
          }
      }
      else
      {
        dsm_free_packet(ip_pkt_ptr);
        PS_TX_META_INFO_FREE(&meta_info_ptr);
        PS_META_INFO_FREE_EX(&meta_info_ex_ptr);
        LOG_MSG_ERROR_0("Invalid v4 FWK instance..Freeing packet");
        ret_val = -1;
        break;
      }
    }
  } while( 0 );

  return ret_val;

}  /* ip4_route() */
