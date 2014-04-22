#ifndef DSSI_H
#define DSSI_H
/*============================================================================

                  D S S I . H

  DESCRIPTION
     This files has structure and API declarations which are internal to DSS
     module

  Copyright (c) 2021 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  ============================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "queue.h"
#include "ps_in.h"
#include "ds_defs.h"
#include "ps_socket_defs.h"



#define  SO_KEEPALIVE_DEF_VAL         FALSE
#define  SO_REUSEADDR_DEF_VAL         FALSE
#define  SO_ERROR_ENABLE_DEF_VAL      FALSE
#define  TCP_NODELAY_DEF_VAL          FALSE
#define  TCP_DELAYED_ACK_DEF_VAL      FALSE
#define  TCP_SACK_DEF_VAL             FALSE
#define  TCP_TIMESTAMP_DEF_VAL        FALSE
#define  UDP_ENCAPS_DEF_VAL           FALSE
#define  IP_RECVIF_DEF_VAL            FALSE
#define  IP_RECVERR_DEF_VAL           FALSE
#define  IPV6_RECVERR_DEF_VAL         FALSE
#define  IP_MCAST_LOOP_DEF_VAL        FALSE
#define  SO_SKIP_ROUTE_SCOPE_DEF_VAL  FALSE
#define  SO_SNDBUF_DEF_VAL            (10 * 1024)
#define  SO_SNDBUF_MIN_VAL            536
#define  SO_SNDBUF_MAX_VAL            (256 * 1024)
#define  IP_TOS_DEF_VAL               0
#define  IP_TTL_DEF_VAL               255
#define  IP_MCAST_TTL_DEF_VAL         1
#define  IPV6_TCLASS_DEF_VAL          0
#define  TCP_MAXSEG_DEF_VAL           1460
#define  TCP_MAXSEG_MIN_VAL           536
#define  TCP_MAXSEG_MAX_VAL           (18 * 1024)

#define  SO_RCVBUF_DEF_VAL            (16 * 1024)
#define  SO_RCVBUF_MIN_VAL            536
#define  SO_RCVBUF_MAX_VAL            (256 * 1024)

#define  MAX_SOCKET_FACTORY           1

/*==================================================================================*/

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  Value used to abstract the actual primary phys link instance at DSS level
---------------------------------------------------------------------------*/
#define DSSOCKI_PRIMARY_PHYS_LINK_ID  0xFF

/*---------------------------------------------------------------------------
  Socket allocation constants.
---------------------------------------------------------------------------*/
#define SOCKAVAIL            0                   /* socket is free for use */
#define SOCKFDMAX       0x1FFF                    /* socket descriptor max */
#define APPAVAIL             0              /* application is free for use */
#define DSS_NETHANDLEMAX        0x2FFF            /* socket descriptor max */
#define PORTBASE        (1025)                      /* ephemeral port base */
#define PORTOFFSET      (0xFFFF  - PORTBASE)      /* ephemeral port offset */

/*---------------------------------------------------------------------------
  Socket Half Open Interval
---------------------------------------------------------------------------*/
#define DSS_HALF_OPEN_INTERVAL 15000          /* Max time to ack a syn/ack */

/*---------------------------------------------------------------------------
  Socket structure
---------------------------------------------------------------------------*/
struct socket
{
  struct ps_in6_addr address;                                /* IP address */
  uint16 port;                                              /* port number */
};

/*---------------------------------------------------------------------------
  Connection structure (two sockets)
---------------------------------------------------------------------------*/
struct connection
{
  struct socket local;
  struct socket remote;
};

/*---------------------------------------------------------------------------
  Definitions for pkt_read_info
---------------------------------------------------------------------------*/
typedef struct
{
  uint32 ifaceId;
  uint32 length;
  struct ps_sockaddr_in6 v6RemoteSockAddr;
} ps_pkt_read_info;

/*---------------------------------------------------------------------------
  Definitions for meta data for ICMP err Q
---------------------------------------------------------------------------*/
typedef struct
{
  struct ps_in6_addr           v6_offender;
  struct ps_sock_extended_err  extended_err;
  struct ps_sockaddr_in6       original_dgram_dest_addr;
} ps_icmp_errq_meta_info;

/*---------------------------------------------------------------------------
  Socket Config Structure
---------------------------------------------------------------------------*/
extern struct dss_socket_config_s
{
  /*-------------------------------------------------------------------------
    Flag to determine the any network policy for sockets.
  -------------------------------------------------------------------------*/
  ds_addr_family_type any_network_policy;

  /*-------------------------------------------------------------------------
    Flag to determine whether to do a graceful close or abort the
    connection when the associated interface is dormant.
  -------------------------------------------------------------------------*/
  boolean                   tcp_graceful_dormant_close;

  /*-------------------------------------------------------------------------
    Idle time (in milliseconds) for a TCP connection before TCP starts sending
    keepalive probes.
  -------------------------------------------------------------------------*/
  uint32                    tcp_keepalive_idle_time;

  /*-------------------------------------------------------------------------
    Default Round trip time (in milliseconds) for a TCP connection
  -------------------------------------------------------------------------*/
  uint32                    tcp_default_round_trip_time;

  q_type sock_cb_q;
  q_type tcp_cb_q;
  q_type udp_cb_q;
  q_type icmp_cb_q;
  q_type rsvp_cb_q;
  q_type app_cb_q;
  uint8  tcp_count;
} sock_config_cb;

/*---------------------------------------------------------------------------
  protocol control block union type - contains pointers to protocol
  specific control blocks
---------------------------------------------------------------------------*/
struct icmp_cb;
struct tcb;
struct udp_cb;

/*=================================================================================*/

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
typedef enum
{
  PS_SOCKET_EVENT_MIN                 = 0,
  PS_SOCKET_TCP_CONNECTED_EV          = PS_SOCKET_EVENT_MIN,
  PS_SOCKET_PCB_CLOSED_EV             = PS_SOCKET_EVENT_MIN + 1,
  PS_SOCKET_ICMP_ERROR_EV             = PS_SOCKET_EVENT_MIN + 2,
  PS_SOCKET_DSM_ITEM_NOT_AVAILABLE_EV = PS_SOCKET_EVENT_MIN + 3,
  PS_SOCKET_DSM_ITEM_AVAILABLE_EV     = PS_SOCKET_EVENT_MIN + 4,
  PS_SOCKET_UDP_CONNECTED_EV          = PS_SOCKET_EVENT_MIN + 5,
  PS_SOCKET_EVENT_MAX
} ps_socket_event_enum_type;

typedef struct
{
  void *                    scb_ptr;

  union
  {
    struct
    {
      struct  connection    conn;
      int32                 options_mask;
      uint32                mss;
    } tcp_connected_event;

    struct  connection      conn;
    uint8                   pcb_closed_reason;
    int16                   icmp_error;

  } data;
} ps_socket_event_info_type;


#endif
