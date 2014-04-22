/*===========================================================================

        D A T A  S E R V I C E S  S O C K E T  I N T E R N A L   F I L E

DESCRIPTION

 The Data Services sockets internal file. This contains all the functions
 used by the network library internally.

Copyright (c) 2018 - 2021 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
==============================================================================
10/12/22        nk              Handling of KEEPALIVE sock opt in clone socket api
01/02/20        ds              Removed the code to reset the IPSEC Info as it ia taken care in 
                                IP layer.
10/10/19        ds              Code changes for TCP Congestion control 
                                mechanism using TCP SendQ limit.
07/08/19        ds              Code changes to reset cached flow ptr in IP 
                                control block's meta info ptr.
10/12/18        ds              Removed dsstcp_is_writeable() from TCP 
                                function pointer table.
08/08/18        sr              Initial Version
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "ds_headers.h"
#include "target.h"
#include "time_svc.h"
#include <string.h>

#include "dss.h"
#include "dssocki.h"
#include "dsstcp.h"
#include "dssudp.h"
#include "dsm.h"
#include "dssicmp.h"
#include "dss_ps.h"
#include "ps_svc.h"
#include "ps_tcp.h"
#include "ds_ipfltr.h"
#include "ds_defs.h"
#include "pstimer.h"
#include "ps_mem.h"
#include "ran.h"
#include "ps_socket_defs.h"
#include "ps_stat_sock.h"
#include "ps_utils.h"
#include "dss_config.h"
#include "ps_meta_info.h"
#include "ds_system_heap.h"
#include "ds_flow.h"
#include "ds_bearer_manager.h"



/*===========================================================================
            REGIONAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/
/*---------------------------------------------------------------------------
                        DEFINES & CONSTANTS
---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
  Threshold size to determine if a large or small item should be used in the
  dssocki_copy_buf_to_dsm.
---------------------------------------------------------------------------*/
#define DSSOCKI_SMALL_ITEM_THRESHOLD   (3 * (DSM_DS_SMALL_ITEM_SIZ))

/*---------------------------------------------------------------------------
  SDB related valid bits for the sendto() flags.
---------------------------------------------------------------------------*/
#define MSG_SDB_VALID_BITS (0x0)


/*---------------------------------------------------------------------------
  The reserved bits for the sendto() flags. Whenever a new flag is to be
  added, it should be defined in dssocket.h and ORed with MSG_SDB_VALID_BITS
  below so that the reserved bits mask is properly built.
---------------------------------------------------------------------------*/
#define MSG_VALID_BITS    (MSG_SDB_VALID_BITS)
#define MSG_RESERVED_BITS (~MSG_VALID_BITS)
/*---------------------------------------------------------------------------
                        Variable Definitions
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  Socket configuration control block
---------------------------------------------------------------------------*/
struct dss_socket_config_s sock_config_cb;

/*---------------------------------------------------------------------------
  Global loopback and INADDR_ANY constants
---------------------------------------------------------------------------*/
const struct ps_in6_addr ps_in6addr_any = PS_IN6ADDR_ANY_INIT;                /* :: */
const struct ps_in6_addr ps_in6addr_loopback = PS_IN6ADDR_LOOPBACK_INIT ;    /* ::1 */


/*---------------------------------------------------------------------------
  Socket function pointer table.  The functions correspond to the
  protocol specific function calls.  The format of this table is as
  follows:

  type,
  socket,     connect setup,           connect,
  listen_setup, listen,
  accept,
  write,      read,                    read_dsm_chain,
  shutdown,
  close,      abort,                   bind,
  is_writeable

  with the final entry in the table being DSS_ERROR.
  In UDP, abort behaves the same as close.
---------------------------------------------------------------------------*/
struct dss_fcn_ptr_table dssock_fcn_ptr_table[] =
{

  /*-------------------------------------------------------------------------
    TCP functions - found in dsstcp.c
  -------------------------------------------------------------------------*/
    {(int)PS_IPPROTO_TCP,
    dsstcp_socket,
    dsstcp_connect_setup,
    dsstcp_connect,
    dsstcp_listen_setup,
    dsstcp_listen,
    dsstcp_accept,
    dsstcp_write,
    dsstcp_read,
    dsstcp_read_dsm_chain,
    dsstcp_shutdown,
    dsstcp_close,   dsstcp_bind,
    dsstcp_is_writeable,
    dsstcp_register_upcall,
    dsstcp_setopt,
    dsstcp_window_reopen,
    dsstcp_flow_enabled_ind,
    dsstcp_set_route_meta_info,
    dsstcp_set_linger_reset},

  /*-------------------------------------------------------------------------
    UDP functions - found in dssudp.c
  -------------------------------------------------------------------------*/
    {(int)PS_IPPROTO_UDP,
    dssudp_socket,  dssudp_connect_setup,  dssudp_connect,
    NULL,           NULL,
    NULL,
    dssudp_write,   dssudp_read,           dssudp_read_dsm_chain,
    dssudp_shutdown,
    dssudp_close,   dssudp_bind,
    NULL,
    dssudp_register_upcall, dssudp_setopt, NULL, NULL,
    dssudp_set_route_meta_info,NULL},


  /*-------------------------------------------------------------------------
    ICMP functions - found in dssicmp.c
  -------------------------------------------------------------------------*/
    {(int)PS_IPPROTO_ICMP,
    dssicmp_socket,           NULL,                  NULL,
    NULL,           NULL,
    NULL,
    dssicmp_write,  dssicmp_read,          NULL,
    NULL,
    dssicmp_close,  NULL,
    NULL,
    dssicmp_register_upcall,  dssicmp_setopt, NULL, NULL,
    dssicmp_set_route_meta_info, NULL},

  /*-------------------------------------------------------------------------
    ICMPv6 functions - found in dssicmp.c
  -------------------------------------------------------------------------*/
    {(int)PS_IPPROTO_ICMP6,
    dssicmp_socket,           NULL,                  NULL,
    NULL,           NULL,
    NULL,
    dssicmp_write,  dssicmp_read,          NULL,
    NULL,
    dssicmp_close,  NULL,
    NULL,
    dssicmp_register_upcall,  dssicmp_setopt, NULL, NULL,
    dssicmp_set_route_meta_info, NULL},

  /*-------------------------------------------------------------------------
    End of table
  -------------------------------------------------------------------------*/
    {DSS_ERROR,
    NULL,           NULL,                  NULL,
    NULL,           NULL,
    NULL,
    NULL,           NULL,                  NULL,
    NULL,
    NULL,           NULL,
    NULL,
    NULL,           NULL, NULL, NULL},
};

/*---------------------------------------------------------------------------
  Socket File describtor Base.
  This variable is used to allocate the socket file descriptor for each
  socket.

  sockfd = dss_sockfdbase + index; This ensures that each sockfd can now
  have a value SOCKFDBASE_MIN_VAL to 2^15. So, dss_sockfdbase can have a
  value from SOCKFDBASE_MIN_VAL to 2^15 Minus maximum value of index.
  Hence, this variable should be as same type as sockfd (i.e. sint15).

  dss_sockfdbase is made as static because the earlier value should be
  remembered to give a new sockfd value.

  Initialize it to SOCKFDBASE_MIN_VAL, to start with. This will be the
  sockfd allotted to the first opened socket. Note that we choose
  SOCKFDBASE_MIN_VAL, arbitrarily and no special reason is attributed
  for that.

  The values that this variable will have belong to the following set:
  {
     SOCKFDBASE_MIN_VAL + DSS_MAX_SOCKS,
     SOCKFDBASE_MIN_VAL + 2*DSS_MAX_SOCKS,
     SOCKFDBASE_MIN_VAL + 3*DSS_MAX_SOCKS...
     SOCKFDBASE_MIN_VAL + N * DSS_MAX_SOCKS
  }
  where N is given by : ____________
  It should be a multiple of DSS_MAX_SOCKS

  Assuming we have SOCKFDBASE_MIN_VAL as 100 and DSS_MAX_SOCKS as 3,
  here is the set of the values that each index can get:

     Index         Possible sockfd's
   ---------   --------------------------
  |    0    |   100, 103, 106,..
   --------- -----------------------------
  |    1    |   101, 104, 107
   --------- -----------------------------
  |    2    |   102, 105, 108 ...
   --------- -----------------------------

Relation between sockfdbase and sockfd:
   sockfd = sockfdbase + index
   sockfdbase = SOCKFDBASE_MIN_VAL + (n * MAX_SOCKS)

Ranges of sockfd and sockfdbase:
   Minimum value of sockfd:      SOCKFDBASE_MIN_VAL
   Maximum value of sockfd:      SOCKFD_BASE_MAX_VAL
   Minimum value of sockfd_base: SOCKFDBASE_MIN_VAL
   Maximum value of sockfd_base: SOCKFD_BASE_MAX_VAL


---------------------------------------------------------------------------*/
static sint15 dss_sockfdbase = SOCKFDBASE_MIN_VAL;

/*---------------------------------------------------------------------------
  Application Identifier Base.
  This is calculated same way as sockfdbase.

---------------------------------------------------------------------------*/
static sint15 dss_dss_nethandlebase = DSS_NETHANDLEBASE_MIN_VAL;


/*---------------------------------------------------------------------------
  Initial Socket Options Values.
  The following initial values will be copied to the socket option
  table in the socket control block. The copying takes place upon
  the allocation of a scb.

  When expanding the repertoire of supported socket options, all the
  related definitions in dssocki.h as well as the table before have
  to be updated. The table below follows this format:

    opt  |  opt value type |  opt   | optval |  init     |    range
   name  |    indicator    | level  |  size  | opt value |  min - max
 --------+-----------------+--------+--------+-----------+-------------

  PLEASE NOTE THAT ANY NEW ENTRIES SHOULD BE ORDERED ACCORDING TO THE
  DEFINITION OF "optname" given by dss_sockopt_names_type.

  Note: The initial and min-max value of the SO_LINGER option below
        corresponds only to the first field of the structure, i.e., l_onoff.
---------------------------------------------------------------------------*/
const dssocki_sockopt_type dss_sock_opts_default[DSS_SOCKOPT_MAX] =
{
  { (int)DSS_IP_TTL, DSSI_SO_INT, DSS_IPPROTO_IP, sizeof(int), IP_DEF_TTL,
    1, 255 },

  { (int)DSS_SO_SYS_SOCK, DSSI_SO_BOOL, DSS_SOCK, sizeof(int), 0, 0, 1 },

  { (int)DSS_SO_SILENT_CLOSE, DSSI_SO_BOOL, DSS_SOCK, sizeof(int), 0, 0, 1 },

  { (int)DSS_SO_RCVBUF, DSSI_SO_INT, DSS_SOCK, sizeof(int), DEF_WND,
    DSS_MIN_RCVBUF, DSS_MAX_RCVBUF },

  { (int)DSS_SO_LINGER, DSSI_SO_LINGER, DSS_SOCK, sizeof(dss_so_linger_type_s), 0,
    0, 1 },

  { (int)DSS_SO_SNDBUF, DSSI_SO_INT, DSS_SOCK, sizeof(int), DSS_DEF_SNDBUF,
    DSS_MIN_SNDBUF, DSS_MAX_SNDBUF },

  { (int)DSS_TCP_MAXSEG, DSSI_SO_INT, DSS_IPPROTO_TCP, sizeof(int), DEF_MSS,
    DSS_TCP_MAXSEG_LO, DSS_TCP_MAXSEG_HI },

  { (int)DSS_TCP_NODELAY, DSSI_SO_BOOL, DSS_IPPROTO_TCP, sizeof(int), 0, 0, 1 },

  { (int)DSS_SO_KEEPALIVE, DSSI_SO_BOOL, DSS_SOCK, sizeof(int), 0, 0, 1 },

  { (int)DSS_TCP_DELAYED_ACK, DSSI_SO_BOOL, DSS_IPPROTO_TCP,
    sizeof(int), 0, 0, 1 },

  { (int)DSS_TCP_SACK, DSSI_SO_BOOL, DSS_IPPROTO_TCP, sizeof(int), 0, 0, 1 },

  { (int)DSS_TCP_TIME_STAMP, DSSI_SO_BOOL, DSS_IPPROTO_TCP, sizeof(int),
    0, 0, 1 },


  { (int)DSS_IP_RECVIF, DSSI_SO_BOOL, DSS_IPPROTO_IP, sizeof(int), 0, 0, 1 },

  { (int)DSS_IP_TOS, DSSI_SO_INT, DSS_IPPROTO_IP, sizeof(int), 0, 0, 255 },

  { (int)DSS_IPV6_TCLASS, DSSI_SO_INT, DSS_IPPROTO_IPV6, sizeof(int), 0, 0, 255 },

  { (int)DSS_SO_CB_FCN, DSSI_SO_CB_FCN_TYPE, DSS_SOCK,
    sizeof(dss_sock_cb_fcn_type_s), 0, 0, 0 },

  { (int)DSS_SO_ERROR_ENABLE, DSSI_SO_BOOL, DSS_SOCK, sizeof(int), 0, 0, 1 },

  { (int)DSS_SO_ERROR, DSSI_SO_INT, DSS_SOCK, sizeof(int), 0, 0, 255 },

  { (int)DSS_SO_LINGER_RESET, DSSI_SO_LINGER, DSS_SOCK,
    sizeof(dss_so_linger_type_s), 0, 0, 1 },

  { (int)DSS_IP_RECVERR, DSSI_SO_BOOL, DSS_IPPROTO_IP, sizeof(int), 0, 0, 1 },

  { (int)DSS_IPV6_RECVERR, DSSI_SO_BOOL, DSS_IPPROTO_IPV6, sizeof(int), 0, 0, 1 },

  { (int)DSS_TCP_EIFEL, DSSI_SO_BOOL, DSS_IPPROTO_TCP, sizeof(int), 0, 0, 1 },

  { (int)DSS_SO_QOS_SHARE_HANDLE, DSSI_SO_INT, DSS_SOCK, sizeof(int), 0,
    0, 0x40000000},

  { (int)DSS_SO_REUSEADDR, DSSI_SO_BOOL, DSS_SOCK, sizeof(int), 0, 0, 1 },

  { (int)DSS_SO_DISABLE_FLOW_FWDING, DSSI_SO_BOOL, DSS_SOCK, sizeof(int), 0, 0, 1},

  { (int)DSS_ICMP_ECHO_ID, DSSI_SO_INT, DSS_IPPROTO_ICMP, sizeof(int), 0, 0, (1<<15) },

  { (int)DSS_ICMP_ECHO_SEQ_NUM, DSSI_SO_INT, DSS_IPPROTO_ICMP, sizeof(int), 0, 0, (1<<15) },

  { (int)DSS_ICMP_TYPE,DSSI_SO_INT, DSS_IPPROTO_ICMP, sizeof(int), 0,0, 255 },

  {(int)DSS_ICMP_CODE,DSSI_SO_INT, DSS_IPPROTO_ICMP, sizeof(int), 0,0, 255 },

  { (int)DSS_SO_FIONREAD, DSSI_SO_INT, DSS_SOCK, sizeof(int), 0,
    0, DSS_MAX_RCVBUF },

  { (int)DSS_TCP_MAX_BACKOFF_TIME, DSSI_SO_INT,DSS_IPPROTO_TCP, sizeof(int), 0,
    0, TCP_MAX_TOTAL_BACKOFF_TIME},

  { (int)DSS_UDP_ENCAPS, DSSI_SO_INT, DSS_IPPROTO_UDP, sizeof(int), 0,0, 
  0x7FFFFFFF},

};

/*---------------------------------------------------------------------------
  Initial value for the l_linger field, i.e., the duration in ms to
  linger for, of the dss_linger_type for SO_LINGER option.
---------------------------------------------------------------------------*/
static sint31 so_linger_initval = 0;

/*---------------------------------------------------------------------------
  Structures for notifying ICMP errors to applications
---------------------------------------------------------------------------*/
typedef struct
{
  struct ps_sock_extended_err ee;
  struct ps_sockaddr_in    sin_addr;
} dssocki_err_msg_in;

typedef struct
{
  struct ps_sock_extended_err ee;
  struct ps_sockaddr_in6   sin6_addr;
} dssocki_err_msg_in6;



/*===========================================================================

                      LOCAL FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================
FUNCTION DSSOCKI_LINGER_TIMEOUT_CB()

DESCRIPTION
  The callback function associated with the pstimer for the linger timeout.
  This callback is called when the linger timeout on a socket expires.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void
dssocki_linger_timeout_cb
(
  void* in_scb_ptr
)
{
  struct scb_s* scb_ptr = (struct scb_s *) in_scb_ptr; /* ptr to the scb   */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( IS_SOCKOPT_SET(scb_ptr->options_mask, (uint64)DSS_SO_LINGER_RESET))
  {
  #if 0
    (void) dsstcp_close(scb_ptr->protocol_ctl_blk,
                        TRUE,
                        &dss_errno);
  #endif
    dssocki_set_sock_state(scb_ptr, DSSOCKI_CLOSED);
    return;
  }

  /*-------------------------------------------------------------------------
    Call socket_event_occurred() which will assert the CLOSE_EVENT
  -------------------------------------------------------------------------*/
  dssocki_socket_event_occurred(scb_ptr, NULL);

  return;
}


/*===========================================================================
FUNCTION DSSOCKI_PUT_CMSG()

DESCRIPTION
  This function fills up the passed cmsg structure with the passed data after
  aligning it.

DEPENDENCIES
  None

RETURN VALUE
  Length of the cmsg if properly formed. The actual cmsg formed my be smaller
  than this value.

SIDE EFFECTS
  None.
===========================================================================*/
uint16 dssocki_put_cmsg
(
  struct dss_msghdr  *msg,
  struct dss_cmsghdr *cmsg,
  int16              level,
  int16              type,
  uint16             len,
  void               *data
)
{
  uint16 cmlen = DSS_CMSG_LEN(len);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if((((unsigned char *)msg->msg_control) + msg->msg_controllen) <
     (((unsigned char *)cmsg) + cmlen))
  {
    /*-----------------------------------------------------------------------
      Message is truncated.
    -----------------------------------------------------------------------*/
    msg->msg_flags |= DSS_MSG_TRUNC;
    cmlen = ((unsigned char*)msg->msg_control) + msg->msg_controllen -
            ((unsigned char*)cmsg);
  }

  cmsg->cmsg_level = level;
  cmsg->cmsg_type  = type;
  cmsg->cmsg_len   = cmlen;

  memscpy(DSS_CMSG_DATA(cmsg), 
          cmlen - sizeof(struct dss_cmsghdr),
          data,
          cmlen - sizeof(struct dss_cmsghdr));

  return(DSS_CMSG_SPACE(len));
} /* dssocki_put_cmsg() */

/*===========================================================================
FUNCTION DSSOCKI_PROCESS_ANCILLARY_DATA()
DESCRIPTION
  This function processes all ancillary data to be returned to the app.
  It then verifies the space allocated for it and stores the data within
  the cmsg structure.

DEPENDENCIES


RETURN VALUE
  dss_errno

SIDE EFFECTS
  Stores the data requested by the app in the ancillary data struct.
===========================================================================*/
sint15 dssocki_process_ancillary_data
(
  struct dss_msghdr * msg,           /* Message header for filling in data */
  struct scb_s      * scb_ptr,              /* Ptr to socket control block */
  int                 flags,
  sint15            * dss_errno                   /* error condition value */
)
{
  struct dss_cmsghdr   * cmsg_ptr    = NULL;
  dss_in_pktinfo_type_s  in_pktinfo;
  dss_in6_pktinfo_type_s in6_pktinfo;
  dssocki_err_msg_in     err_info;
  dssocki_err_msg_in6    err6_info;
  uint16                 controllen  = 0;
  uint64                 option_mask = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Loop through all cmsghdrs within dss and populate with the appropriate
    ancillary data.
  -------------------------------------------------------------------------*/

  for( cmsg_ptr = DSS_CMSG_FIRSTHDR(msg); cmsg_ptr != NULL;
       cmsg_ptr = DSS_CMSG_NXTHDR(msg, cmsg_ptr))
  {

    if(IS_SOCKOPT_SET(scb_ptr->cached_info.ip.options_mask, (uint64)DSS_IP_RECVIF) &&
       (scb_ptr->rx_iface_id != 0) &&
       ((option_mask & ((uint64)1 << (uint64)DSS_IP_RECVIF)) == 0))
    {
      option_mask |= ((uint64)1 << (uint64)DSS_IP_RECVIF);

      if(scb_ptr->family == (byte)DSS_AF_INET)
      {
        memset(&in_pktinfo, 0, sizeof(in_pktinfo));
        in_pktinfo.if_index = (int)scb_ptr->rx_iface_id;
        controllen += dssocki_put_cmsg(msg, cmsg_ptr, (int16)DSS_IPPROTO_IP,
                                       (int16)DSS_IP_RECVIF, sizeof(in_pktinfo),
                                       (void*)&in_pktinfo);
      }
      else if(scb_ptr->family == (byte)DSS_AF_INET6)
      {
        memset(&in6_pktinfo, 0, sizeof(in6_pktinfo));
        in6_pktinfo.if_index = (int)scb_ptr->rx_iface_id;
        controllen += dssocki_put_cmsg(msg, cmsg_ptr, (int16)DSS_IPPROTO_IP,
                                       (int16)DSS_IP_RECVIF, sizeof(in6_pktinfo),
                                       (void*)&in6_pktinfo);
      }
    }
    else if((flags & DSS_MSG_ERRQUEUE)                 &&
            (IS_SOCKOPT_SET(scb_ptr->cached_info.ip.options_mask,
                            (uint64)DSS_IP_RECVERR) ||
             IS_SOCKOPT_SET(scb_ptr->cached_info.ip.options_mask,
                            (uint64)DSS_IPV6_RECVERR))         &&
            (option_mask & ((uint64)1 << (uint64)DSS_IP_RECVERR)) == 0 &&
            (option_mask & ((uint64)1 << (uint64)DSS_IPV6_RECVERR)) == 0)
    {
      if(scb_ptr->family == (byte)DSS_AF_INET)
      {
        option_mask |= ((uint64)1 << (uint64)DSS_IP_RECVERR);

        memset(&err_info, 0, sizeof(err_info));
        err_info.ee                       = scb_ptr->sock_err;
        err_info.sin_addr.ps_sin_family      = (uint16)DSS_AF_INET;
        err_info.sin_addr.ps_sin_port        = 0;
        err_info.sin_addr.ps_sin_addr.ps_s_addr =
                         PS_IN6_GET_V4_FROM_V4_MAPPED_V6(&scb_ptr->v6_offender);
        memset(err_info.sin_addr.ps_sin_zero, 0,
               sizeof(err_info.sin_addr.ps_sin_zero));

        controllen += dssocki_put_cmsg(msg, cmsg_ptr, (int16)DSS_IPPROTO_IP,
                                       (int16)DSS_IP_RECVERR, sizeof(err_info),
                                       (void*)&err_info);
      }
      else if(scb_ptr->family == (byte)DSS_AF_INET6)
      {
        option_mask |= ((uint64)1 << (uint64)DSS_IPV6_RECVERR);

        memset(&err6_info, 0, sizeof(err6_info));
        err6_info.ee                      = scb_ptr->sock_err;
        err6_info.sin6_addr.ps_sin6_family   = (uint16)DSS_AF_INET6;
        err6_info.sin6_addr.ps_sin6_port     = 0;
        err6_info.sin6_addr.ps_sin6_flowinfo = 0;
        err6_info.sin6_addr.ps_sin6_scope_id = 0;
        memscpy(&err6_info.sin6_addr.ps_sin6_addr,
                sizeof(struct ps_in6_addr),
                &scb_ptr->v6_offender,
               sizeof(struct ps_in6_addr));
        if(err6_info.ee.ee_data != 0)
        {
          /*-----------------------------------------------------------------
            Address is linklocal so ee_data stores the scope_id
          -----------------------------------------------------------------*/
          err6_info.sin6_addr.ps_sin6_scope_id = err6_info.ee.ee_data;
          err6_info.ee.ee_data = 0;
        }

        controllen += dssocki_put_cmsg(msg, cmsg_ptr, (int16)DSS_IPPROTO_IP,
                                       (int16)DSS_IP_RECVERR, sizeof(err6_info),
                                       (void*)&err6_info);
      }
    }
  } /* end for loop */

  if(controllen < msg->msg_controllen)
  {
    msg->msg_controllen = controllen;
  }
  return(DSS_SUCCESS);
} /* dssocki_process_ancillary_data() */




/*===========================================================================
FUNCTION DSSOCKI_GENERATE_PKT_INFO()

DESCRIPTION
  Generates pkt info containing header and misc info

PARAMETERS
  scb_ptr        : socket which is transmitting data
  v6_sockaddr    : remote socket's address
  pkt_info_ptr   : OUT PARAM. pkt info is populated in this parameter
  dss_errno      : errno passed back if pkt info can't be generated

RETURN VALUE
  On error, DSS_ERROR is returned; DSS_SUCCESS otherwise.

DEPENDENCIES
  pointers must be valid

SIDE EFFECTS
  None
===========================================================================*/
int32 dssocki_generate_pkt_info
(
  scb_type             * scb_ptr,
  struct ps_sockaddr_in6  * v6_sockaddr,
  ip_pkt_info_type     * pkt_info_ptr,
  int16                * dss_errno
)
{
  struct socket  localname;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(pkt_info_ptr, 0, sizeof(ip_pkt_info_type));

  if (!PS_IN6_IS_ADDR_V4MAPPED(&v6_sockaddr->ps_sin6_addr))
  {
    pkt_info_ptr->ip_vsn             = IP_V6;
    pkt_info_ptr->ip_hdr.v6.hdr_type = scb_ptr->protocol;

    pkt_info_ptr->ip_hdr.v6.hdr_body.base_hdr.version  = (uint32)IP_V6;
    pkt_info_ptr->ip_hdr.v6.hdr_body.base_hdr.next_hdr = scb_ptr->protocol;

    pkt_info_ptr->ip_hdr.v6.hdr_body.base_hdr.dst_addr =
      v6_sockaddr->ps_sin6_addr;
    pkt_info_ptr->ip_hdr.v6.hdr_body.base_hdr.trf_cls  =
      scb_ptr->cached_info.ip.tclass;
  }
  else
  {
    pkt_info_ptr->ip_vsn             = IP_V4;
    pkt_info_ptr->ip_hdr.v4.tos      = scb_ptr->cached_info.ip.tos;
    pkt_info_ptr->ip_hdr.v4.protocol = scb_ptr->protocol;

    pkt_info_ptr->ip_hdr.v4.dest.ps_s_addr =
      PS_IN6_GET_V4_FROM_V4_MAPPED_V6(&v6_sockaddr->ps_sin6_addr);
  }

  if (PS_IN6_IS_ADDR_MC_LINKLOCAL(&v6_sockaddr->ps_sin6_addr) ||
      PS_IN6_IS_ADDR_LINKLOCAL(&v6_sockaddr->ps_sin6_addr))
  {
    /*-----------------------------------------------------------------------
      If dest addr is linklocal then there should be an iface id as the
      scope id in ps_sockaddr_in6 for dest. Else return error.
    -----------------------------------------------------------------------*/
    if (scb_ptr->acb_ptr->fw_inst)
    {
      pkt_info_ptr->if_ptr = scb_ptr->acb_ptr->fw_inst;
    }
    else
    {
      DS_ERROR_LOG_0("FW Instance is NULL corresponding to passed scb");
      *dss_errno = DS_ENOROUTE;
      return DSS_ERROR;
    }
  }

  DSSOCKI_CACHE_GET_LOCAL(scb_ptr, localname);
  switch (scb_ptr->protocol)
  {
    case PS_IPPROTO_ICMP6:
    case PS_IPPROTO_ICMP:
      pkt_info_ptr->ptcl_info.icmp.type = scb_ptr->cached_info.pcb.icmp.type;
      pkt_info_ptr->ptcl_info.icmp.code = scb_ptr->cached_info.pcb.icmp.code;
      break;

    case PS_IPPROTO_TCP:
      pkt_info_ptr->ptcl_info.tcp.src_port = localname.port;
      pkt_info_ptr->ptcl_info.tcp.dst_port = v6_sockaddr->ps_sin6_port;
      break;

    case PS_IPPROTO_UDP:
      pkt_info_ptr->ptcl_info.udp.src_port = localname.port;
      pkt_info_ptr->ptcl_info.udp.dst_port = v6_sockaddr->ps_sin6_port;
      break;

    default:
      break;
  }

  return DSS_SUCCESS;
} /* dssocki_generate_pkt_info() */



/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =

                    EXTERNAL FUNCTION DEFINTIONS

= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/
/*===========================================================================
FUNCTION DSSOCKI_ALLOCSCB()

DESCRIPTION
  Allocates socket control block from SCB array.  Calculates socket
  descriptor and sets it in the SCB.

DEPENDENCIES
  None.

PARAMETERS
  protocol        : The kind of protocol used.TCP/UDP/ICMP

RETURN VALUE
  Returns pointer to socket control block structure.  If none are
  available, returns NULL.

SIDE EFFECTS
  None.
===========================================================================*/
struct scb_s *dssocki_allocscb
(
  byte    protocol
)
{
  static uint16 max_sockfd = 0;              /* maximum sockfd seen so far */
  struct scb_s *scb_ptr = NULL;   /* ptr to socket control block structure */
  int index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( ((byte)PS_IPPROTO_TCP == protocol) &&
          (sock_config_cb.tcp_count >= DSS_MAX_TCP_SOCKS))
  {
    DS_ERROR_LOG_0("  tcp_count >= DSS_MAX_TCP_SOCKS");
    SOCK_INC_STATS(scb_alloc_failures, 1) ;
    return NULL;
  }

  scb_ptr = (scb_type *) ps_mem_get_buf(PS_MEM_DSS_SCB_TYPE);

  /*------------------------------------------------------------------------
    If buffer not available, return NULL
  -------------------------------------------------------------------------*/
  if (scb_ptr == NULL)
  {
    DS_ERROR_LOG_0( "Memory allocation for SCB failed");
    SOCK_INC_STATS(scb_alloc_failures, 1) ;
    return (NULL);
  }

  memset(scb_ptr, 0, sizeof(struct scb_s));

  /*---------------------------------------------------------------------
    Assign socket descriptor value in the valid range between
    SOCKFDBASE_MIN_VAL to SOCKFDBASE_MAX_VAL.  This is the socket
    descriptor (handle) specified in all socket function calls.
    Socket descriptor monotonically increases each time.
  ---------------------------------------------------------------------*/
  if (-1 == (index = ps_mem_buf_to_index(scb_ptr)))
  {
    SOCK_INC_STATS(scb_alloc_failures, 1) ;
    DS_ERROR_LOG_0("Invalid buffer passed to ps_mem_buf_to_index");
  }

  /*-------------------------------------------------------------------------
    Update the value of the dss_sockfdbase when current sockfd is less than
    or equal to maximum sockfd seen so far. If it is greater than the
    Maximum value it can hold, bring it back to initial value.
  -------------------------------------------------------------------------*/
  index = (uint16) index;
  if ((index + dss_sockfdbase) <= max_sockfd)
  {
    dss_sockfdbase += DSS_MAX_SOCKS;

    if (dss_sockfdbase >= SOCKFDBASE_MAX_VAL)
    {
      DS_MSG2( MSG_LEGACY_MED, "Changing sockfdbase from %d to %d",
              dss_sockfdbase, SOCKFDBASE_MIN_VAL);
      dss_sockfdbase = SOCKFDBASE_MIN_VAL;
    }
  }

  /*-------------------------------------------------------------------------
    max_sockfd should reflect the recent sockfd returned to any socket
    client.
  -------------------------------------------------------------------------*/
  max_sockfd = (uint16)(index + dss_sockfdbase);

  scb_ptr->sockfd = (sint15) (dss_sockfdbase + index);

  /*-------------------------------------------------------------------------
    Initialize the scb
  -------------------------------------------------------------------------*/
  if (dssocki_init_scb( scb_ptr ) == FALSE)
  {
    SOCK_INC_STATS(scb_alloc_failures, 1) ;
    PS_MEM_FREE(scb_ptr);
    return (NULL);
  }

  scb_ptr->sock_cb = NULL;
  scb_ptr->sock_cb_user_data = NULL;

  if( (byte)PS_IPPROTO_TCP == protocol )
  {
    sock_config_cb.tcp_count ++;
  }

  /*-------------------------------------------------------------------------
    Add to the queue
  -------------------------------------------------------------------------*/
  (void) q_link(scb_ptr, &scb_ptr->link);
  q_put(&sock_config_cb.sock_cb_q, &scb_ptr->link);

  SOCK_INC_STATS(total_socks_allocated, 1);
  SOCK_INC_STATS(open_socks, 1);
  return (scb_ptr);
} /* dssocki_allocscb() */

/*===========================================================================
FUNCTION DSSOCKI_INIT_SCB()

DESCRIPTION
  Initializes the given scb

DEPENDENCIES
  None.

RETURN VALUE
  TRUE:  If the initialization was successful
  FALSE: Otherwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean dssocki_init_scb
(
  struct scb_s *scb_ptr           /* ptr to socket control block structure */
)
{
  int i;
  ps_timer_handle_type timer_handle;  /* Return value from timer fn. calls */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  scb_ptr->fcn_ptr_table        = NULL;
  scb_ptr->type                 = 0;
  scb_ptr->protocol             = 0;

  scb_ptr->requested_event_mask = 0;
  scb_ptr->data_available       = FALSE;
  scb_ptr->closed_state_reason  = DS_ENOTCONN;

  /*-------------------------------------------------------------------------
    zero out queue lengths
  -------------------------------------------------------------------------*/
  scb_ptr->so_head              = NULL;
  scb_ptr->so_partial           = NULL;
  scb_ptr->so_qlimit            = 0;

  /*-------------------------------------------------------------------------
    Set socket state to INIT and reset the state_flags
  -------------------------------------------------------------------------*/
  scb_ptr->socket_state         = DSSOCKI_INIT;
  scb_ptr->state_flags          = 0x0;

  /*-------------------------------------------------------------------------
    Set socket options to default values. Certain fields should never
    be changed once initilized, these fields include option name, option
    level and option value data type. Only option value and the size of the
    option value are subject to assignments.
  -------------------------------------------------------------------------*/
  scb_ptr->options_mask = 0x0;

  for (i=(int)DSS_SOCKOPT_MIN+1; i < (int)DSS_SOCKOPT_MAX; i++)
  {
    if (dss_sock_opts_default[i].initval)
    {
      SOCKOPT_SET(scb_ptr->options_mask, i);
    }
  }

  scb_ptr->cached_info.ip.ttl =
    (uint8)dss_sock_opts_default[DSS_IP_TTL].initval;
  scb_ptr->cached_info.ip.tos =
    (uint8)dss_sock_opts_default[DSS_IP_TOS].initval;
  scb_ptr->cached_info.pcb.tcp.mss =
    (uint16)dss_sock_opts_default[DSS_TCP_MAXSEG].initval;
  scb_ptr->cached_info.pcb.tcp.cwind = PS_TCP_DEF_CWIND;

  scb_ptr->rcvbuf    = (uint32)dss_sock_opts_default[DSS_SO_RCVBUF].initval;
  scb_ptr->sndbuf    = (uint32)dss_sock_opts_default[DSS_SO_SNDBUF].initval;
  scb_ptr->so_linger = so_linger_initval;


  /*-------------------------------------------------------------------------
    Allocate the linger timer
  -------------------------------------------------------------------------*/
  timer_handle = ps_timer_alloc( dssocki_linger_timeout_cb, scb_ptr );
  if( timer_handle == (uint32)PS_TIMER_FAILURE)
  {
    DS_ERROR_LOG_1("Failed to allocate linger timer, sockfd=%d",scb_ptr->sockfd);
    return( FALSE );
  }
  else
  {
    scb_ptr->linger_timer = timer_handle;
  }

  memset (&scb_ptr->fltr_info, 0, sizeof(dssocki_fltr_info_type));

  scb_ptr->flow_ptr = NULL;
  scb_ptr->is_write_blocked = FALSE;
  return( TRUE );

} /* dssocki_init_scb() */



/*===========================================================================
FUNCTION DSSOCKI_FREESCB()

DESCRIPTION
  Frees a socket control block in the SCB array for re-use.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dssocki_freescb
(
  struct scb_s *scb_ptr           /* ptr to socket control block structure */
)
{
  int16                           dss_errno;
  ds_ipfltr_delete_param_type     fltr_delete_param_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( scb_ptr == NULL )
  {
    DS_ERROR_LOG_0("NULL scb_ptr passed");
    DS_ASSERT( 0 );
    return;
  }

  DS_ENTER_CRIT_SECTION_NO_TASKLOCK(&global_ps_crit_section);

  /*-------------------------------------------------------------------------
    Cleanup any filters installed for this socket
  -------------------------------------------------------------------------*/

  if (scb_ptr->fltr_info.fw_inst!= NULL)
  {
    memset (&fltr_delete_param_ptr, 0, sizeof (ds_ipfltr_delete_param_type));

    fltr_delete_param_ptr.client_type = DS_FWK_CLIENT_SOCKETS;
    fltr_delete_param_ptr.client_handle = scb_ptr->acb_ptr->dss_handle;
    fltr_delete_param_ptr.client_id = FLTR_CLIENT_SOCKETS;
    fltr_delete_param_ptr.fi_handle = scb_ptr->fltr_info.ipfltr_handle;
	
    (void ) ds_ipfltr_delete(&fltr_delete_param_ptr, &dss_errno);
  }

  /*-------------------------------------------------------------------------
    Free the linger timer
  -------------------------------------------------------------------------*/
  PS_TIMER_FREE_HANDLE( scb_ptr->linger_timer );
  
  /*------------------------------------------------------------------------
    Decrement the SCB ref count only if there is a binding ACB
  ------------------------------------------------------------------------*/
  if (NULL != scb_ptr->acb_ptr)
  {
    scb_ptr->acb_ptr->scb_cnt--;
  }

  /*------------------------------------------------------------------------
    Decrement the tcp_count.
  ------------------------------------------------------------------------*/
  if((byte)PS_IPPROTO_TCP == scb_ptr->protocol)
  {
    sock_config_cb.tcp_count--;
  }

  /*-------------------------------------------------------------------------
    Free the socket control block and set everything to NULL - then
    make it available for re-use. We are using memset here also so that
    "scb_ptr->sockfd != sockfd" condition in dssocki_sockfdtoscb function
    will not pass because of garbage stored from previous allocation.
  -------------------------------------------------------------------------*/
  q_delete(&sock_config_cb.sock_cb_q, (q_link_type *) scb_ptr);
  memset( scb_ptr, 0, sizeof(struct scb_s) );
  DS_LEAVE_CRIT_SECTION_NO_TASKLOCK(&global_ps_crit_section);
  PS_MEM_FREE(scb_ptr);


  SOCK_DEC_STATS(open_socks, 1);
} /* dssocki_freescb() */

/*===========================================================================
FUNCTION DSSOCKI_SOCKFDTOSCB()

DESCRIPTION
  Given a socket descriptor, returns a socket control block from SCB array.

DEPENDENCIES
  None.

RETURN VALUE
  Returns pointer to socket control block structure.

SIDE EFFECTS
  None.
===========================================================================*/
struct scb_s *dssocki_sockfdtoscb
(
  int sockfd                                          /* socket descriptor */
)
{
  struct scb_s *scb_ptr;          /* ptr to socket control block structure */
  uint16        index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( sockfd < SOCKFDBASE_MIN_VAL ||
       sockfd >= SOCKFDBASE_MAX_VAL + DSS_MAX_SOCKS )
  {
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Find the matching SCB by indexing into the corresponding ps_mem buffer
    pool.
  -------------------------------------------------------------------------*/
  index = ((uint16)sockfd - SOCKFDBASE_MIN_VAL) % DSS_MAX_SOCKS;
  scb_ptr = (struct scb_s *) ps_mem_index_to_buf(index, PS_MEM_DSS_SCB_TYPE);

  /*-------------------------------------------------------------------------
    Make sure that NULL is returned when invalid sockfd is passed
  -------------------------------------------------------------------------*/
  if (scb_ptr == NULL || scb_ptr->sockfd != sockfd)
  {
    return NULL;
  }

  return (scb_ptr);

} /* dssocki_sockfdtoscb() */


/*===========================================================================
FUNCTION DSSOCKI_INDEXTOSCB()

DESCRIPTION
  Given an index, returns a socket control block from SCB array.

DEPENDENCIES
  None.

RETURN VALUE
  Returns pointer to socket control block structure.

SIDE EFFECTS
  None.
===========================================================================*/
struct scb_s *dssocki_indextoscb
(
  uint16 index
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Find the matching SCB by indexing into the corresponding ps_mem buffer
    pool.
  -------------------------------------------------------------------------*/
  return ((struct scb_s *) ps_mem_index_to_buf(index, PS_MEM_DSS_SCB_TYPE));

} /* dssocki_indextoscb() */



/*===========================================================================
FUNCTION DSSOCKI_ALLOCACB()

DESCRIPTION
  Allocates application control block from ACB array.  Assigns application
  ID and sets the network and socket callback functions.

DEPENDENCIES
  None.

RETURN VALUE
  Returns pointer to application control block structure.  If none are
  available, returns NULL.

SIDE EFFECTS
  None.
===========================================================================*/
acb_type *dssocki_allocacb
(
  void
)
{
  static uint16 max_dss_nethandle = 0;              /* maximum sockfd seen so far */
  int           index;
  int           loop;
  acb_type *acb_ptr = NULL;  /* ptr to application control block structure */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  acb_ptr = (acb_type *) ps_mem_get_buf(PS_MEM_DSS_ACB_TYPE);

  /*-------------------------------------------------------------------------
    Return NULL if buffer not available
  -------------------------------------------------------------------------*/
  if (acb_ptr == NULL)
  {
    DS_ERROR_LOG_0( "Memory allocation for ACB failed");
    return (NULL);
  }

  memset(acb_ptr, 0, sizeof(acb_type));

  /*-------------------------------------------------------------------------
    The variable, "max_index" is made as static because , whenever an app is
    opened the index given is one higher than the previous one. This method
    of index allocation allows us to allocate different indices every time a
    new app is opened and closed.
  -------------------------------------------------------------------------*/
  if (-1 == (index = ps_mem_buf_to_index(acb_ptr)))
  {
    DS_ERR_FATAL("Invalid buffer passed to ps_mem_buf_to_index");
  }

  /*-------------------------------------------------------------------------
    Update the value of the dss_dss_nethandlebase. If it is greater than the
    maximum value it can hold, bring it back to initial value.
  -------------------------------------------------------------------------*/
  index = (uint16) index;
  if ((index + dss_dss_nethandlebase) <= max_dss_nethandle)
  {
    dss_dss_nethandlebase += DSS_MAX_APPS;

    if (dss_dss_nethandlebase >= DSS_NETHANDLEBASE_MAX_VAL)
    {
      DS_MSG2(MSG_LEGACY_MED, "Changing dss_nethandlebase from %d to %d",
              dss_dss_nethandlebase, DSS_NETHANDLEBASE_MIN_VAL);
      dss_dss_nethandlebase = DSS_NETHANDLEBASE_MIN_VAL;
    }
  }

  /*-------------------------------------------------------------------------
    max_dss_nethandle should reflect the recent handle returned to any
    client.
  -------------------------------------------------------------------------*/
  max_dss_nethandle = (uint16)(index + dss_dss_nethandlebase);

  acb_ptr->dss_nethandle = (sint15) (dss_dss_nethandlebase + index );

  /*------------------------------------------------------------------------
    Initialize ACB
  ------------------------------------------------------------------------*/
  acb_ptr->scb_cnt                      = 0;
  acb_ptr->net_cb                       = NULL;
  acb_ptr->sock_cb                      = NULL;
  acb_ptr->notified_cb_flag             = FALSE;
  acb_ptr->fw_inst                      = NULL;
  acb_ptr->qos_requested = FALSE;

    /*---------------------------------------------------------------------
      Initialize the event queue structure.
    ---------------------------------------------------------------------*/
  for(loop = 0; loop < PDN_EVENT_MAX; loop++)
  {
    (void)q_init((acb_ptr->event_q_array) + loop);
  }

  /*-------------------------------------------------------------------------
    Initialize the link and add ACB to the queue
  -------------------------------------------------------------------------*/
  (void) q_link(acb_ptr, &acb_ptr->link);
  q_put(&sock_config_cb.app_cb_q, &acb_ptr->link);

  return (acb_ptr);

} /* dssocki_allocacb() */

/*===========================================================================
FUNCTION DSSOCKI_FREEACB()

DESCRIPTION
  Frees an application control block in the ACB array for re-use.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dssocki_freeacb
(
  acb_type *acb_ptr          /* ptr to application control block structure */
)
{
  int           loop;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( acb_ptr == NULL )
  {
    DS_ERROR_LOG_0("NULL acb_ptr passed");
    //ASSERT( 0 );
    return;
  }

  DS_ENTER_CRIT_SECTION_NO_TASKLOCK(&global_ps_crit_section);
  /*---------------------------------------------------------------------
    Cleanup the event queue structure.
  ---------------------------------------------------------------------*/
  for(loop = 0; loop < PDN_EVENT_MAX; loop++)
  {
    (void)q_destroy((acb_ptr->event_q_array) + loop);
  }
  
  /*------------------------------------------------------------------------
    Delete from the queue and make the buffer available for reuse.
    We are using memset here also so that "acb_ptr->dss_nethandle != dss_nethandle"
    condition in dssocki_dss_nethandletoacb function will not pass because of
    garbage stored from previous allocation.
  ------------------------------------------------------------------------*/
  q_delete(&sock_config_cb.app_cb_q,(q_link_type*)acb_ptr);
  //memset(acb_ptr, 0, sizeof(acb_type));
  PS_MEM_FREE(acb_ptr);
  DS_LEAVE_CRIT_SECTION_NO_TASKLOCK(&global_ps_crit_section);

} /* dssocki_freeacb() */

/*===========================================================================
FUNCTION DSSOCKI_DSS_NETHANDLETOACB()

DESCRIPTION
  Given an application ID, returns an application control block from ACB
  array.

DEPENDENCIES
  None.

RETURN VALUE
  Returns pointer to application control block structure.

SIDE EFFECTS
  None.
===========================================================================*/
acb_type *dssocki_dss_nethandletoacb
(
  sint15 dss_nethandle                                   /* application ID */
)
{
  acb_type *acb_ptr;         /* ptr to application control block structure */
  uint16    index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Do range checking on input param.
  -------------------------------------------------------------------------*/
  if ( dss_nethandle >= (DSS_NETHANDLEBASE_MAX_VAL + DSS_MAX_APPS) ||
       dss_nethandle < DSS_NETHANDLEBASE_MIN_VAL )
  {
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Find the matching ACB by indexing into the corresponding ps_mem buffer
    pool.

    dss_nethandle = dss_nethandlebase + index. We need to find the index.
    This can also be written as,
    dss_nethandle = ( DSS_NETHANDLEBASE_MIN_VAL + n * MAX_APPS) + index.
    So, index = (dss_nethandle - DSS_NETHANDLEBASE_MIN_VAL) mod MAX_APPS
  -------------------------------------------------------------------------*/
  index = ((uint16)dss_nethandle - DSS_NETHANDLEBASE_MIN_VAL) % DSS_MAX_APPS;
  acb_ptr = (acb_type *) ps_mem_index_to_buf(index, PS_MEM_DSS_ACB_TYPE);

  /*-------------------------------------------------------------------------
    Make sure that NULL is returned when an invalid dss_nethandle is passed
  -------------------------------------------------------------------------*/
  if (acb_ptr == NULL || acb_ptr->dss_nethandle != dss_nethandle)
  {
    return NULL;
  }

  return acb_ptr;

} /* dssocki_dss_nethandletoacb() */

/*===========================================================================
FUNCTION DSSOCKI_IS_ADDR_IN_USE()

DESCRIPTION
  Determines if the specified address, in network byte order, is in use for
  the given protocol.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - The address is in use
  FALSE - The address is not in use

SIDE EFFECTS
  None.
===========================================================================*/
boolean dssocki_is_addr_in_use
(
  struct scb_s *       scb_ptr,
  byte                 proto,
  struct ps_sockaddr_in6* laddr
)
{
  struct scb_s *tmp_scb_ptr;  /* tmp socket control block used for compare */
  struct socket localname;
//  ps_route_scope_type  lscope, rscope, rscope_intersect;
  boolean              reuse_addr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);
  tmp_scb_ptr = (scb_type *) q_check( &sock_config_cb.sock_cb_q );

  reuse_addr = FALSE;
  if (scb_ptr)
  {
    reuse_addr = IS_SOCKOPT_SET(scb_ptr->options_mask, (uint64)DSS_SO_REUSEADDR );
  }

  while (tmp_scb_ptr != NULL)
  {
    DS_ASSERT(SOCKAVAIL != tmp_scb_ptr->sockfd);

      /*---------------------------------------------------------------------
        If sockets are using a different protocol, this is not a problem.
        Just continue.
      ---------------------------------------------------------------------*/
    if (proto == tmp_scb_ptr->protocol)
    {
      DSSOCKI_CACHE_GET_LOCAL(tmp_scb_ptr, localname);

      /*---------------------------------------------------------------------
        If socket has same local address as specified parameter, then we
        need to check against the local port number.  Note that INADDR_ANY
        will result in the same generated local IP address.
      ---------------------------------------------------------------------*/
      if( localname.port == laddr->ps_sin6_port &&
          ( PS_IN6_ARE_ADDR_EQUAL(&localname.address,&laddr->ps_sin6_addr) ||
            PS_IN6_ARE_ADDR_EQUAL(&laddr->ps_sin6_addr,&ps_in6addr_any) ))
      {
        /*-------------------------------------------------------------------
          Check if the address is a multicast address and the SO_REUSEADDR
          socket option is set. We allow address,port reuse for Multicast.
          MCAST then INADDR_ANY or MCAST is okay.
          INADDR_ANY then MCAST is okay.
        -------------------------------------------------------------------*/
        if( reuse_addr == TRUE &&
            ( PS_IN6_IS_V4_MAPPED_V6_ADDR_MULTICAST(&(laddr->ps_sin6_addr)) ||
             PS_IN6_IS_V4_MAPPED_V6_ADDR_MULTICAST(&(localname.address)) ||
             PS_IN6_IS_ADDR_MULTICAST(&(laddr->ps_sin6_addr))               ||
             PS_IN6_IS_ADDR_MULTICAST(&(localname.address) )) )
        {
          tmp_scb_ptr =
            (scb_type *) q_next(&sock_config_cb.sock_cb_q, &tmp_scb_ptr->link);
          continue;
        }

        if(scb_ptr && IS_SOCKOPT_SET(scb_ptr->options_mask, (uint64)DSS_SO_SYS_SOCK))
        {
          DS_MSG2(MSG_LEGACY_HIGH,"Skip checking for dhcp6 socket current scb: 0x%x passed scb: 0x%x",tmp_scb_ptr,scb_ptr);
          tmp_scb_ptr =
            (scb_type *) q_next(&sock_config_cb.sock_cb_q, &tmp_scb_ptr->link);
          continue;
        }

        /*-------------------------------------------------------------------
          Port and address are equal, so the address may already be in use.
          If the port number is not a wildcard (0), return an error.
        -------------------------------------------------------------------*/
        if (localname.port != 0)
        {
          DS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
          return TRUE;
        }
      } /* if address and port are equal */
    } /* if protocols are equal */

    tmp_scb_ptr
        = (scb_type *) q_next(&sock_config_cb.sock_cb_q, &tmp_scb_ptr->link);

  } /* while */
  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  return FALSE;
}

/*===========================================================================
FUNCTION DSSOCKI_GETEPHEM_PORT()

DESCRIPTION
  Returns an ephemeral port, for the specified protocol, between PORTBASE and
  PORTBASE+PORTOFFSET after checking that it is not in use by another socket.
  Currently, the port number returned is unique across TCP/UDP and across
  interfaces.

DEPENDENCIES
  None.

RETURN VALUE
  Port number: in network byte order, not in use by another socket
  0: In case there is no ephemeral port available

SIDE EFFECTS
  None.
===========================================================================*/
uint16 dssocki_getephem_port
(
  byte proto      /* protocol for which the caller wants an ephemeral port */
)
{
  struct ps_sockaddr_in6 tmp_addr;              /* tmp variable to store addr */
  int iter = 0;                               /* number of loop iterations */
  uint16 rand_num;                          /* just to suppress lint error */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Initialize the address structure. s_addr is initialized to 0x0 so that
    we return a port number not in use on any interface.
  -------------------------------------------------------------------------*/
  tmp_addr.ps_sin6_family = (uint16)DSS_AF_INET6;
  tmp_addr.ps_sin6_addr   = ps_in6addr_any;

  /*-------------------------------------------------------------------------
    Set local port to a random number between PORTBASE and PORTBASE+PORTOFFSET.
    This is the range of ephemeral port numbers generated when the
    application has not bound to any particular local port (preferred for
    client applications).
  -------------------------------------------------------------------------*/
  for (iter=0; iter<=PORTOFFSET; iter++)
  {
    rand_num = (uint16) ran_dist( ran_next(), PORTBASE, PORTBASE + PORTOFFSET);
    tmp_addr.ps_sin6_port = dss_htons(rand_num);

    if (dssocki_is_addr_in_use(NULL, proto, &tmp_addr) == FALSE)
    {
      break;
    }
  }

  if ( iter > PORTOFFSET )
  {
    tmp_addr.ps_sin6_port = 0x0;
  }

  return (tmp_addr.ps_sin6_port);

} /* dssocki_genephem_port() */

/*===========================================================================
FUNCTION DSSOCKI_FCNPTR_SETUP()

DESCRIPTION
  Sets up the protocol specific function pointer table.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Function pointer table is assinged.
===========================================================================*/
sint15 dssocki_fcnptr_setup
(
  struct scb_s *scb_ptr,                 /* socket control block structure */
  sint15 *dss_errno                               /* error condition value */
)
{
  struct dss_fcn_ptr_table *sp;             /* function pointer table item */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Look for entry in protocol table - break when socket protocol type
    matches the protocol type of the function pointer table.
  -------------------------------------------------------------------------*/

  for(sp=dssock_fcn_ptr_table; sp->protocol!= DSS_ERROR; sp++)
  {
    if(scb_ptr->protocol == sp->protocol)
    {
      break;
    }
  }

  /*-------------------------------------------------------------------------
    End of table, with no entries found - return error.
  -------------------------------------------------------------------------*/
  if(sp->protocol == DSS_ERROR)
  {
    *dss_errno = DS_ESOCKNOSUPPORT;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Entry was found, so assign table in socket control block structure
  -------------------------------------------------------------------------*/
  scb_ptr->fcn_ptr_table = sp;

  return (DSS_SUCCESS);

} /* dssocki_fcnptr_setup() */

/*===========================================================================
FUNCTION DSSOCKI_GET_FCNPTR_TABLE

DESCRIPTION
  Returns the protocol specific function pointer table.

DEPENDENCIES
  None.

RETURN VALUE
  function pointer table

SIDE EFFECTS
  Function pointer table is assinged.
===========================================================================*/
struct dss_fcn_ptr_table * dssocki_get_fcnptr_table
(
 int protocol                                        /* transport protocol */
)
{
  struct dss_fcn_ptr_table *sp;             /* function pointer table item */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Look for entry in protocol table - break when socket protocol type
    matches the protocol type of the function pointer table.
  -------------------------------------------------------------------------*/

  for(sp=dssock_fcn_ptr_table; sp->protocol!= DSS_ERROR; sp++)
  {
    if(protocol == sp->protocol)
    {
      break;
    }
  }


  /*-------------------------------------------------------------------------
    End of table, with no entries found - return error.
  -------------------------------------------------------------------------*/
  if(sp->protocol == DSS_ERROR)
  {
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Entry was found, so return the table
  -------------------------------------------------------------------------*/
  return sp;

} /* dssocki_get_fcnptr_table() */


/*===========================================================================
FUNCTION DSSOCKI_SEND_TO_PS()

DESCRIPTION
  This function allocates memory PS command using dssocki_alloc_ps_cmd_buffer
  and if that passes, it will call dssocki_send_ps_cmd_buffer

DEPENDENCIES
  None.

RETURN VALUE
  DSS_ERROR if no memory can be allocated for ps commands
  else returns DSS_SUCCESS

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dssocki_send_to_ps
(
  struct scb_s  *scb_ptr,                   /* ptr to socket control block */
  void * user_data,                           /* Ptr to payload to be sent */
  struct ps_sockaddr_in6            * ta_ptr,    /* Ptr to structure containing
                                                 dest info                 */
  dssocki_app_to_ps_cmd_kind_type kind                  /* PS command kind */
)
{
  dssocki_app_to_ps_cmd_type * ps_cmd;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ps_cmd = dssocki_alloc_ps_cmd_buffer(kind);
  if(ps_cmd == NULL)
  {
   /*-------------------------------------------------------------------
      No memory could be allocated for the command buffers.
    ------------------------------------------------------------------*/
    DS_ERROR_LOG_0("No memory could be allocated for ps command buffers");
    return DSS_ERROR;
  }

  /*----------------------------------------------------------------------
    Send the ps command buffer.
  ---------------------------------------------------------------------*/
  dssocki_send_ps_cmd_buffer(scb_ptr, user_data, ta_ptr, ps_cmd);
  return DSS_SUCCESS;
} /* dssocki_send_to_ps() */

/*===========================================================================
FUNCTION DSSOCKI_ALLOC_PS_CMD_BUFFER()

DESCRIPTION
  Allocates memory for the PS command buffer and sets its kind.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the ps command buffer or NULL

SIDE EFFECTS
  None.
===========================================================================*/
dssocki_app_to_ps_cmd_type *
dssocki_alloc_ps_cmd_buffer
(
  dssocki_app_to_ps_cmd_kind_type kind                 /* PS command kind */
)
{
  dssocki_app_to_ps_cmd_type     *ps_cmd;     /* ps command buffer         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Verify if there are free ps mem items to enqueue write cmd.
  -------------------------------------------------------------------------*/
  if( kind == APP_TO_PS_SOCKET_PAYLOAD &&
      ps_mem_less_than_few_free_items(PS_MEM_APP_TO_PS_CMD_TYPE))
  {
    return NULL;
  }

  /*-------------------------------------------------------------------------
    To send the data to the PS task, we initialize the ps_cmd structure
    and enqueue that into the ps_socket_send_q and signal
    the PS task that there is data available it's the RX queue.
  -------------------------------------------------------------------------*/
  ps_cmd = (dssocki_app_to_ps_cmd_type *)
    ps_mem_get_buf(PS_MEM_APP_TO_PS_CMD_TYPE);

  if(ps_cmd == NULL)
  {
    DS_ERROR_LOG_0("Memory allocation for PS command failed");
    return NULL;
  }

  ps_cmd->kind      = kind;
  return ps_cmd;
}/* dssocki_alloc_ps_cmd_buffer() */

/*===========================================================================
FUNCTION DSSOCKI_SEND_PS_CMD_BUFFER()

DESCRIPTION
  Sends the application's dsm_item data to the PS task. This function
  is called from the context of the application task.

DEPENDENCIES
  The PS cmd has to be allocated memory and its kind has to be set using
  dssocki_alloc_ps_cmd_buffer

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dssocki_send_ps_cmd_buffer
(
  struct scb_s                   *scb_ptr,  /* ptr to socket control block */
  void                           *user_data,/* Ptr to payload to be sent   */
  struct ps_sockaddr_in6            *ta_ptr,   /* Ptr to destination info     */
  dssocki_app_to_ps_cmd_type     *ps_cmd    /* ps command buffer           */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( NULL == ps_cmd || NULL == scb_ptr ||
      ps_cmd->kind <= APP_TO_PS_SOCKET_CMD_MIN_VAL ||
      ps_cmd->kind >= APP_TO_PS_SOCKET_CMD_MAX_VAL )
  {
    //ASSERT(0);
    DS_ERROR_LOG_0( "Invalid parameters to dssocki_send_ps_cmd_buffer");
    PS_MEM_FREE(ps_cmd);
    return;
  }

  ps_cmd->pcb       = scb_ptr->protocol_ctl_blk;
  ps_cmd->protocol  = scb_ptr->protocol;
  ps_cmd->user_data = user_data;

  ps_cmd->sockfd    = scb_ptr->sockfd;

  if( ta_ptr != NULL )
  {
    ps_cmd->ta = *ta_ptr;
  }
  else
  {
    /*-----------------------------------------------------------------------
      If no destination information is passed in, use 0 for address and port
    -----------------------------------------------------------------------*/
    memset(&ps_cmd->ta, 0, sizeof(ps_cmd->ta));
  }

  q_put( &ps_socket_send_q, &ps_cmd->link);
  ps_set_signal( (uint32)PS_SOCKET_RX_Q_SIGNAL);
}/* dssocki_send_ps_cmd_buffer() */

/*===========================================================================
FUNCTION DSSOCKI_COPY_BUF_TO_DSM()

DESCRIPTION
  Copies application data buffer vector to data services memory pool. The
  copying is done intelligently. If the number of bytes to copy is less than
  the DSSOCKI_SMALL_ITEM_THRESHOLD, the bytes are copied into a small dsm
  item otherwise they are copied into a large dsm item.
  Copies the data from each array item, and concatenates the data into a
  packet chain.  For efficiency, each dsm memory item is completely filled,
  thus a new dsm item for each array item is not allocated.

DEPENDENCIES
  None.

RETURN VALUE
  Total byte count.

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dssocki_copy_buf_to_dsm
(
  struct ps_iovec *iov,      /* array of data buffers from which to copy data */
  sint15 iovcount,                                /* number of array items */
  sint15 max_bytes,                     /* maximum number of bytes to copy */
  dsm_item_type **buf_ptr_ptr          /* pointer to dsm item to copy into */
)
{
  dsm_item_type     *item_ptr = NULL;       /* pointer to current dsm item */
  dsm_item_type     *last_ptr = NULL;          /* pointer to last dsm item */
  byte              *data_ptr;                   /* pointer to data buffer */
  byte              *tmp_data_ptr = NULL;      /* temporary scratch buffer */
  int               i;                                       /* loop index */

  sint15            buf_leftover = 0;    /* bytes left over in single item */

  uint16            data_len;   /* length to be copied in current dsm item */
  sint15            length;               /* specified length to be copied */
  sint15            remaining_length = 0; /* remaining bytes to be copied  */
  sint15            byte_count= 0;            /* total returned byte count */
  dsm_mempool_id_enum_type pool_id;  /* the pool to allocate DSM item from */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    If there are no bytes to copy, return an empty dsm item as appfield
    of dsm item is used to send metainfo.
  -------------------------------------------------------------------------*/
  if(max_bytes == 0)
  {
    *buf_ptr_ptr = dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL);
    return 0;
  }

  remaining_length   = max_bytes;

  /*-------------------------------------------------------------------------
    Loop across all array items, copying the data from each array item, and
    concatenate the data into a packet chain.  For efficiency, each dsm
    memory item is completely filled, thus a new dsm item for each array
    item is not allocated.
  -------------------------------------------------------------------------*/
  for (i=0; (i < iovcount) && (remaining_length > 0) ; i++)
  {
    length       = MIN(iov[i].ps_iov_len, remaining_length);
    data_ptr     = iov[i].ps_iov_base;

    /*-----------------------------------------------------------------------
      For efficiency determine if there are still free bytes left over in
      the buffer before allocating a new one.
    -----------------------------------------------------------------------*/
    if (buf_leftover > 0)
    {
      data_len = (uint16) MIN(length, buf_leftover);
    }
    else
    {
      /*---------------------------------------------------------------------
        No free bytes are left, so allocate a new dsm item.

        First, determine an appropriate DSM item pool. If the remaining
        length is more than a threshold then choose large item pool. Small
        item pool, otherwise.
      ---------------------------------------------------------------------*/
      if ( remaining_length > DSSOCKI_SMALL_ITEM_THRESHOLD )
      {
        pool_id = DSM_DS_LARGE_ITEM_POOL;
      }
      else
      {
        pool_id = DSM_DS_SMALL_ITEM_POOL;
      }

      /*---------------------------------------------------------------------
        If the preferred pool does not have sufficient free items left,
        try to use the other pool. If both of them are running out of
        free items, just return the current number of bytes already copied.
      ---------------------------------------------------------------------*/
      if ( DSM_LESS_THAN_FEW_FREE_ITEMS(pool_id) )
      {
        pool_id = DSM_DS_OTHER_POOL_ID(pool_id);

        if ( DSM_LESS_THAN_FEW_FREE_ITEMS(pool_id) )
        {
          DS_ERROR_LOG_0(" Running out of buffers");
          return (byte_count);
        }
      }

      /*---------------------------------------------------------------------
        Copy the minimum of the current buffer length and the size of the
        retrieved data item.
      ---------------------------------------------------------------------*/
      data_len = (uint16) MIN(length, DSM_POOL_ITEM_SIZE(pool_id));
      DS_MSG2(MSG_LEGACY_LOW, "Using pool %d: %d bytes",
              (int) pool_id, data_len);

      if (last_ptr != NULL)
      {
        /*-------------------------------------------------------------------
          There are already existing items in the packet chain, so create
          and insert the dsm item.  Note, the call to dsm_new_buffer() will
          never return NULL, if the resource checks (above) work correctly.
        -------------------------------------------------------------------*/
        last_ptr = item_ptr;

        if ( (item_ptr = dsm_new_buffer(pool_id)) == NULL )
        {
          DS_ERROR_LOG_0(" Running out of buffers");
          return (byte_count);
        }

        /*-------------------------------------------------------------------
          Insert pointer into packet chain
        -------------------------------------------------------------------*/
        if (last_ptr != NULL)
        {
          last_ptr->pkt_ptr = item_ptr;
        }
      }
      else
      {
        /*-------------------------------------------------------------------
          This is the initial item for the packet chain.  Create a new item,
          and then save the head of the packet chain, which will be the
          return value of this function.
        -------------------------------------------------------------------*/
        if ( (item_ptr = dsm_new_buffer(pool_id)) == NULL )
        {
          DS_ERROR_LOG_0("Running out of buffers");
          return (byte_count);
        }

        last_ptr = item_ptr;
        /*-------------------------------------------------------------------
          Save head of packet chain
        -------------------------------------------------------------------*/
        *buf_ptr_ptr = item_ptr;
      }
    } /* buf_leftover > 0 */

    /*-----------------------------------------------------------------------
      Copy data - possibly split over more than one dsm item
    -----------------------------------------------------------------------*/
    while (data_len != 0)
    {
      /*---------------------------------------------------------------------
        Offset into data array to be copied.  Need to assign temp variable,
        as item_ptr->data always needs to be pointing to beginning of dsm
        item.  Ensure (for sanity checking) that item_ptr can never be
        NULL.
      ---------------------------------------------------------------------*/

      if( item_ptr == NULL )
      {
        ASSERT( 0 );
        return byte_count;
      }

      tmp_data_ptr        = item_ptr->data_ptr;
      tmp_data_ptr       += item_ptr->used;

      memscpy(tmp_data_ptr,
              (uint16)data_len,
              data_ptr,
              (uint16)data_len);
      item_ptr->used     += (uint16)data_len;
      data_ptr            = (byte *) data_ptr + data_len;
      byte_count         += (sint15) data_len;

      /*---------------------------------------------------------------------
        Now calculate the length for the next part of data buffer.  ASSERT
        that more than the size of the dsm item has not been used.
      ---------------------------------------------------------------------*/
      DS_ASSERT( item_ptr->used <= item_ptr->size );
      buf_leftover  = (sint15) ((item_ptr->size - item_ptr->used));

      length           -= (sint15) data_len;
      remaining_length -= (sint15) data_len;

      DS_ASSERT(remaining_length >= 0);

      /*---------------------------------------------------------------------
        More data to copy
      ---------------------------------------------------------------------*/
      if (buf_leftover == 0 && length > 0)
      {
        /*-------------------------------------------------------------------
          First, determine an appropriate DSM item pool. If the remaining
          length is more than a threshold then choose large item pool. Small
          item pool, otherwise.
        -------------------------------------------------------------------*/

        if ( remaining_length > DSSOCKI_SMALL_ITEM_THRESHOLD )
        {
          pool_id = DSM_DS_LARGE_ITEM_POOL;
        }
        else
        {
          pool_id = DSM_DS_SMALL_ITEM_POOL;
        }

        /*-------------------------------------------------------------------
          If the preferred pool does not have sufficient free items left,
          we try to use the other pool. If both of them are running out of
          free items, just return the current number of bytes already copied.
        -------------------------------------------------------------------*/
        if ( DSM_LESS_THAN_FEW_FREE_ITEMS(pool_id) )
        {
          pool_id = DSM_DS_OTHER_POOL_ID(pool_id);

          if ( DSM_LESS_THAN_FEW_FREE_ITEMS(pool_id) )
          {
            DS_ERROR_LOG_0(" Running out of buffers");
            return (byte_count);
          }
        }

        data_len = (uint16) MIN(length, DSM_POOL_ITEM_SIZE(pool_id));
        DS_MSG2(MSG_LEGACY_MED, "Using pool %d: %d bytes", (int) pool_id, data_len);

        last_ptr = item_ptr;
        if ((item_ptr = dsm_new_buffer(pool_id)) == NULL)
        {
          DS_ERROR_LOG_0("Running out of buffers");
          return (byte_count);
        }
        /*-------------------------------------------------------------------
          Insert pointer into packet chain
        -------------------------------------------------------------------*/
        last_ptr->pkt_ptr = item_ptr;
      }
      else
      {
        data_len = 0;
      }

      /*---------------------------------------------------------------------
        Sanity check - This should never happen.
      ---------------------------------------------------------------------*/
      if ( length < 0)
      {
        DS_ERR_FATAL("Sanity check");
      }

    } /* while */

  } /* for number of array items */

  return (byte_count);

} /* dssocki_copy_buf_to_dsm() */


/*===========================================================================
FUNCTION DSSOCKI_GENERATE_META_INFO()

DESCRIPTION
  Generates PS meta info packet for TX flags and socket opetions
  set and attaches it to the packet.

DEPENDENCIES
  Packet head pointer has to include all headers above IP.

RETURN VALUE
  DSS_SUCCESS or DSS_ERROR (dss_errno contains cause).

SIDE EFFECTS
  None.
===========================================================================*/
ps_tx_meta_info_type * dssocki_generate_meta_info
(
  scb_type*       scb_ptr,
  ds_flow_type*   flow_ptr,
  uint32          tx_flags,
  boolean         *ipcb_meta_info_valid,
  sint15*         dss_errno
)
{
  ps_tx_meta_info_type             *tx_meta_info_ptr = NULL; // This is a DUP Meta TX ptr
  time_type                         timestamp;
  ip_cb_type                        ipcb;
  ds_flow_type                      *ds_flow_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( scb_ptr == NULL || dss_errno == NULL )
  {
    DS_ERROR_LOG_2("args: scb_ptr 0x%x, dss_errno 0x%x", scb_ptr, dss_errno);
    //ASSERT( 0 );
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Don't ASSERT if flow_ptr is NULL since metainfo is generated in bind()
    and listen() and routing is not performed in those functiosn resulting
    in flow_ptr being NULL
  -------------------------------------------------------------------------*/

  if (flow_ptr == NULL )
  {
    DS_ERROR_LOG_0("Flow Pointer is NULL, unable to generate Meta info");
    return NULL;
  }

  if (ds_fwk_is_flow_enabled(flow_ptr) == FALSE)
  {
    DS_ERROR_LOG_1("Flow is Disabled for flow 0x%x. Dropping the packet",
                   flow_ptr);
    return NULL;
  }

  if (scb_ptr->protocol == PS_IPPROTO_TCP)
  {
    if (scb_ptr->protocol_ctl_blk.tcb)
    {
      ipcb = scb_ptr->protocol_ctl_blk.tcb->ipcb;
    }
    else
    {
      DS_MSG1(MSG_LEGACY_ERROR,"TCB is NULL for sockfd %d",scb_ptr->sockfd);
      return NULL;
    }		
  }
  else if (scb_ptr->protocol == PS_IPPROTO_UDP)
  {
    if (scb_ptr->protocol_ctl_blk.ucb)
    {
      ipcb = scb_ptr->protocol_ctl_blk.ucb->ipcb;
    }
    else
    {
      DS_MSG1(MSG_LEGACY_ERROR,"UCB is NULL for sockfd %d",scb_ptr->sockfd);
      return NULL;
    }
  }
  else if ((scb_ptr->protocol == PS_IPPROTO_ICMP) ||
           (scb_ptr->protocol == PS_IPPROTO_ICMP6))
  {
    if (scb_ptr->protocol_ctl_blk.icb)
    {
      ipcb = scb_ptr->protocol_ctl_blk.icb->ipcb;
    }
    else
    {
      DS_MSG1(MSG_LEGACY_ERROR,"ICB is NULL for sockfd %d",scb_ptr->sockfd);
      return NULL;
    }
  }
  else
  {
    DS_MSG0(MSG_LEGACY_ERROR,"Unsupported protocol for socket");
    return NULL;
  }

  /*
    If the Meta info pointer in the IPCB is not populated, we DUP the meta info
    in DSS. This META info pointer is freed when the UCB/TCP  is freed
    dssudpi_free_ucb for UDP and dsstcpi_free_tcb for TCP
    
    If the IPCB meta info pointer is populated, protocols does the DUP of meta
    info pointer in ip_send/ip6_send and freed by DS_FWK when the packet
    is transffered to PDN/Bearer manager
  */

  if (ipcb.meta_info_ptr == NULL)
  {
    PS_TX_META_INFO_DUP(dss_tx_meta_info_ptr, &tx_meta_info_ptr);
    DS_MSG1(MSG_LEGACY_HIGH,"Dupping META Info dss_tx_meta_info_ptr 0x%p",
            dss_tx_meta_info_ptr);
  }
  else
  {
    tx_meta_info_ptr = ipcb.meta_info_ptr;
    *ipcb_meta_info_valid = TRUE;
  }

  if (tx_meta_info_ptr == NULL)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"META Info is NULL");
    return NULL;
  }

  DS_MSG3(MSG_LEGACY_HIGH, "tx_meta_info_ptr %p, pkt_meta_info_ptr %p,"
          "rt_meta_info_ptr %p",tx_meta_info_ptr,
          tx_meta_info_ptr->pkt_meta_info_ptr,
          tx_meta_info_ptr->rt_meta_info_ptr);
  /*-----------------------------------------------------------------------
    Set the transmit flags and socket file descriptor and time stamp
  -----------------------------------------------------------------------*/
  if(tx_meta_info_ptr->pkt_meta_info_ptr != NULL)
  {
    PS_PKT_META_SET_TX_FLAGS( tx_meta_info_ptr->pkt_meta_info_ptr,
                             tx_flags);
    PS_PKT_META_SET_DOS_ACK_HANDLE( tx_meta_info_ptr->pkt_meta_info_ptr,
                                   scb_ptr->sockfd);

    time_get_ms( timestamp);
    PS_PKT_META_SET_TIMESTAMP( tx_meta_info_ptr->pkt_meta_info_ptr,
                               timestamp);
  }

  if (tx_meta_info_ptr->rt_meta_info_ptr != NULL)
  {
    /*-------------------------------------------------------------------------
      Set the routing cache information in the meta info
    -----------------------------------------------------------------------*/
    DSSOCKI_CACHE_GET_TX_FWK_INST( scb_ptr,
                                  PS_META_GET_ROUTING_CACHE( tx_meta_info_ptr));

    PS_TX_META_SET_FILTER_RESULT(tx_meta_info_ptr,
                                 (uint32)FLTR_CLIENT_QOS_OUTPUT,
                                 ds_flow_ptr);

  
    /*------------------------------------------------------------------------
      Identity the IP version and store it in the META TX Info
    -------------------------------------------------------------------------*/
    if (scb_ptr->family == IPV4_ADDR)
    {
      tx_meta_info_ptr->rt_meta_info_ptr->pkt_info.ip_vsn = IP_V4;
    }
    else if (scb_ptr->family == IPV6_ADDR)
    {
      tx_meta_info_ptr->rt_meta_info_ptr->pkt_info.ip_vsn = IP_V6;
    }

    /*-------------------------------------------------------------------------
      Use special subset ID for apps using socket2
    -------------------------------------------------------------------------*/
    if (scb_ptr->acb_ptr != NULL)
    {
      PS_TX_META_SET_SUBSET_ID(tx_meta_info_ptr,
                               scb_ptr->acb_ptr->dss_nethandle);
    }
    else
    {
      PS_TX_META_SET_SUBSET_ID(tx_meta_info_ptr, 0);
    }
  }

  return tx_meta_info_ptr;

} /* dssocki_generate_meta_info() */

/*===========================================================================
FUNCTION DSSOCKI_SET_SOCK_STATE()

DESCRIPTION
  This function sets the socket state. It can be NULL, INIT, OPENING, OPEN,
  CLOSING, WAIT_FOR_CLOSE, or CLOSED.

  NULL: Is a pseudo-state, that is set prior to initial socket
           creation.  A NULL socket cannot be accessed by any API
           function.

  INIT: Is specific to TCP, and is set when a TCP socket is first
           created, through a call to socket().  Note this is an
           invalid state for UDP sockets.

  LISTEN: This socket it listening.  This is specific to TCP, as such
           is an invalid state for UDP.  accept() may be called on
           this socket.

  OPENING: the socket is not writable and read() and write() would
           return DS_EWOULDBLOCK.
           A connected socket is in the OPENING state after dss_connect()
           has been called and before the socket is completly connected.
           A connectionless socket is never in this state.

  OPEN:    means the socket is writable if there is enough memory available.
           A connected socket is in the OPEN state when TCP is in the
           ESTABLISHED state.
           A connectionless socket is in OPEN state in the dss_socket()
           call.

  CLOSED:  means the socket is writable and readable  and read() and write()
           would return DS_ECONNABORTED, DS_ECONNRESET, DS_IPADDRCHANGED or
           DS_ENETDOWN. A connected socket is in the CLOSED state after it
           lost its connection to the server or the network connectivity was
           lost any time after the dss_connect() was called.
           A connectionless socket is in this state, only when the network
           subsystem has experienced a failure.

  CLOSING: means that a TCP socket has issued a close(), but the TCP close
           is still in progress.  The initial call to close() for TCP
           sockets would return DS_EWOULDBLOCK.  This corresponds to the
           active close, and the socket is waiting for TCP to fully
           negotiate the close.
           This state is invalid for connectionless sockets.

  WAIT_FOR_CLOSE:  means that TCP has fully closed, or the TCP close timer
                   has expired.  In this state, the application may call
                   close() again to clean up the socket and free it for
                   re-use.
                   This state is invalid for connectionless sockets.

  This should be called whenever the state changes and is the only function
  which modifies the socket_state member of the struct scb_s.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dssocki_set_sock_state
(
  struct scb_s*           scb_ptr,     /* Ptr to socket control block for
                                          the socket */
  dssocki_state_enum_type new_state                        /* Socket state */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Change socket state to the new state.  Protect access to since the
    SCB socket state can be modified through another task.
  -------------------------------------------------------------------------*/
  DS_MSG3(MSG_LEGACY_MED, "Sockfd %d oldstate=%d newstate=%d",
          scb_ptr->sockfd,
          scb_ptr->socket_state,
          new_state);
  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  scb_ptr->socket_state = new_state;
  switch ( new_state )
  {
    case DSSOCKI_NULL:
    case DSSOCKI_INIT:
    case DSSOCKI_LISTEN:
    case DSSOCKI_OPENING:
    case DSSOCKI_OPEN:
    case DSSOCKI_CLOSING:
      /*---------------------------------------------------------------------
        Nothing special needs to be done
      ---------------------------------------------------------------------*/
      break;

    case DSSOCKI_WAIT_FOR_CLOSE:
    case DSSOCKI_CLOSED:
      /*---------------------------------------------------------------------
        If the so_linger option is set, cancel the linger_timer
      ---------------------------------------------------------------------*/
      if ( IS_SOCKOPT_SET(scb_ptr->options_mask, (uint64)DSS_SO_LINGER) )
      {
        if( PS_TIMER_SUCCESS != ps_timer_cancel(scb_ptr->linger_timer) )
        {
          DS_ERROR_LOG_1( "Cant free timer 0x%x", scb_ptr->linger_timer);
        }
      }
      break;

    default:
      break;
  }
  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  /*-------------------------------------------------------------------------
    Do realtime check for socket events for the scb.
  -------------------------------------------------------------------------*/
  dssocki_socket_event_occurred(scb_ptr, NULL);

} /* dssocki_set_sock_state() */



/*===========================================================================

FUNCTION DSSOCKI_SOCKET_EVENT_CHECK()

DESCRIPTION
  This function performs a real-time check, to determine if any of the socket
  events of interest have occurred.  If so, it returns the event mask of the'
  events that have occurred.

DEPENDENCIES
  None.

RETURN VALUE
  sint31 event mask of the socket events that have occurred.

SIDE EFFECTS
  None.

===========================================================================*/
sint31 dssocki_socket_event_check
(
  struct scb_s* scb_ptr,          /* ptr to socket control block structure */
  sint31 requested_event_mask        /* Events the socket is interested in */
)
{
  sint31  event_mask = 0;    /* returned mask of events that have occurred */
  ds_flow_type      * flow_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   DS_ENTER_CRIT_SECTION_NO_TASKLOCK(&global_ps_crit_section);
   DS_MSG1(MSG_LEGACY_HIGH, "dssocki_socket_event_check(): %d ",requested_event_mask);
  /*-------------------------------------------------------------------------
    Check to see if the write event has occurred.  If so, set it in the
    returned event mask.
    WRITE event is TRUE when dss_write/dss_sendto, if called, will return
    either SUCCESS or an error other than EWOULDBLOCK.
    WRITE event is FALSE when dss_write/dss_sendto, if called, will return
    EWOULDBLOCK.
  -------------------------------------------------------------------------*/
  if( (requested_event_mask & DS_WRITE_EVENT) == DS_WRITE_EVENT)
  {

    /*-----------------------------------------------------------------------
      Use default ps_flow if secondary ps_flow is not ACTIVATED. Data would be
      forwarded on default ps_flow in this case, and we need to make sure
      that default ps_flow is not flow controlled. Otherwise app may run in to
      infinite loop (app gets EWOULDBLOCK when data can not be forwarded ->
      app asserts WRITE EVENT -> app writes data again)

      NOTE : DO NOT use internal macros as ps_iface_ptr can be NULL
    -----------------------------------------------------------------------*/
    flow_ptr = scb_ptr->flow_ptr;
    if (ds_flow_is_valid(flow_ptr) &&
        flow_ptr->state != DS_FLOW_STATE_UP)
    {
      if(!ds_fwk_get_default_bearer_flow(scb_ptr->acb_ptr->fw_inst,
                                                scb_ptr->family,
                                                &scb_ptr->flow_ptr))
    {
        DS_MSG1(MSG_LEGACY_ERROR,
                "No flow available to transmit data sockfd %d",scb_ptr->sockfd);
    }
    }
    flow_ptr = scb_ptr->flow_ptr;

    /*----------------------------------------------------------------------
      If CANTSENDMORE flag is set or if the socket is in closed state then
      we need not check anything more. A write call is bound to return an
      error other than EWOULDBLOCK if either of either of these conditions
      is met.
    ----------------------------------------------------------------------*/
    if ( (scb_ptr->state_flags & (uint16)DSSOCKI_CANTSENDMORE) ||
         (scb_ptr->socket_state == DSSOCKI_CLOSED) ||
         (scb_ptr->so_error != 0))
    {
      event_mask |= DS_WRITE_EVENT;
    }
    /*-----------------------------------------------------------------------
      Check if the flow that this socket is using is enabled for TX traffic
    -----------------------------------------------------------------------*/
    else if (ds_flow_is_valid(flow_ptr) && ((!ds_fwk_is_flow_enabled(flow_ptr))||
	        (!ds_bearer_mgr_is_bearer_writable(flow_ptr->bearer_ptr))))
    {
      DS_MSG2(MSG_LEGACY_ERROR, "Socket %d: flow, 0x%p, is disabled for Tx traffic",
               scb_ptr->sockfd, flow_ptr);
    }
    /*----------------------------------------------------------------------
      If the mobile is out of memory then WRITE event is FALSE and app
      should get EWOULDBLOCK.
    ----------------------------------------------------------------------*/
    else if( DSM_LESS_THAN_FEW_FREE_ITEMS(DSM_DS_LARGE_ITEM_POOL) &&
             DSM_LESS_THAN_FEW_FREE_ITEMS(DSM_DS_SMALL_ITEM_POOL) )
    {
      DS_MSG2(MSG_LEGACY_ERROR,  "NO dsm items, small=%d large=%d",
                DSM_POOL_FREE_CNT(DSM_DS_SMALL_ITEM_POOL),
                DSM_POOL_FREE_CNT(DSM_DS_LARGE_ITEM_POOL));
    }
    else if( ps_mem_less_than_few_free_items(PS_MEM_APP_TO_PS_CMD_TYPE))
    {
      DS_MSG0(MSG_LEGACY_ERROR, "LOW in APP_TO_PS_CMD buffers");
    }
    else if( ps_mem_less_than_few_free_items(PS_MEM_TX_META_INFO_TYPE))
    {
      DS_MSG0(MSG_LEGACY_ERROR, "LOW in Meta info items");
    }
    /*----------------------------------------------------------------------
      A socket in init state is writeable only when the above checks pass.

      Note that if the call is dormant and CONNECT is being done on a new
      TCP socket, i.e., the socket is in INIT state, connect() will be
      called a few times by app. The first call will cause the re-orig, the
      second will actually cause the connect to occur and the third will
      provide the connect attempt result. The first connect will keep the
      TCP socket in the INIT state, but it does need to look for a write
      event, and so check for INIT state also.
    ----------------------------------------------------------------------*/
    else if (scb_ptr->socket_state == DSSOCKI_INIT)
    {
      event_mask |= DS_WRITE_EVENT;
    }

    /*-----------------------------------------------------------------------
      If connected, make the socket writeable only if there is enough memory
      available and the socket is writeable.
    -----------------------------------------------------------------------*/
    else if ( scb_ptr->socket_state == DSSOCKI_OPEN )
    {
      if ((scb_ptr->fcn_ptr_table)->is_writeable != NULL &&
           scb_ptr->is_write_blocked)
      {
        DS_MSG1( MSG_LEGACY_ERROR, "Socket %d has enough data queued", scb_ptr->sockfd);
      }
      else
      {
        /*------------------------------------------------------------------
          Everything's okay, enabling write event.
        ------------------------------------------------------------------*/
        event_mask |= DS_WRITE_EVENT;
      }
    } /* if socket_state == DSSOCKI_OPEN */
  } /* if DS_WRITE_EVENT */

  /*-------------------------------------------------------------------------
    Check to see if the read event has occurred, making the socket readable.
    A socket is readable when socket_state is OPEN and there is data to be
    read or when the socket was CLOSED by the other side.
  -------------------------------------------------------------------------*/
  if( (requested_event_mask & DS_READ_EVENT) == DS_READ_EVENT)
  {
    if (((scb_ptr->socket_state   == DSSOCKI_OPEN ||
          scb_ptr->socket_state   == DSSOCKI_CLOSING)  &&
          scb_ptr->data_available == TRUE &&
          !(scb_ptr->state_flags & (uint16)DSSOCKI_CANTRECVMORE)) ||
        (scb_ptr->error_available == TRUE) ||
        (scb_ptr->state_flags & (uint16)DSSOCKI_CANTRECVMORE)  ||
        (scb_ptr->socket_state == DSSOCKI_INIT) ||
        (scb_ptr->socket_state == DSSOCKI_CLOSED) ||
        (scb_ptr->socket_state == DSSOCKI_WAIT_FOR_CLOSE) ||
        (scb_ptr->socket_state == DSSOCKI_LISTEN))
    {
      /*---------------------------------------------------------------------
        Conditions are true - socket is readable - set event in event_mask.
      ---------------------------------------------------------------------*/
      event_mask |= DS_READ_EVENT;

    } /* if readable */

  } /* if DS_READ_EVENT */

  /*------------------------------------------------------------------------
    Check to see if the accept event has occured, makeing the socket
    accpetable.  A socket is acceptable when socket_state is LISTEN
    and there is data to be read.
  ------------------------------------------------------------------------*/
  if ( (requested_event_mask & DS_ACCEPT_EVENT) == DS_ACCEPT_EVENT )
  {
    if ( ((scb_ptr->socket_state == DSSOCKI_LISTEN) &&
         ((scb_ptr->data_available == TRUE) ||
          (scb_ptr->so_partial == NULL))) ||
          (scb_ptr->socket_state == DSSOCKI_CLOSED) ||
          (scb_ptr->socket_state == DSSOCKI_INIT))
    {
      event_mask |= DS_ACCEPT_EVENT;
    } /* if acceptable */
  }

  /*-------------------------------------------------------------------------
    Check to see if the close event has occurred, making the socket
    closeable.  A socket is closeable when socket_state is in WAIT_FOR_CLOSE
    or CLOSED or if the socket is lingering.
  -------------------------------------------------------------------------*/
  if( (requested_event_mask & DS_CLOSE_EVENT) == DS_CLOSE_EVENT)
  {
    if ( scb_ptr->socket_state == DSSOCKI_WAIT_FOR_CLOSE ||
         scb_ptr->socket_state == DSSOCKI_CLOSED ||
         (scb_ptr->state_flags & (uint16)DSSOCKI_LINGER) )
    {
      /*---------------------------------------------------------------------
        Conditions are true - socket is closeable - set event in event_mask
      ---------------------------------------------------------------------*/
      event_mask |= DS_CLOSE_EVENT;

    } /* if closeable */

  } /* if DS_CLOSE_EVENT */

  DS_LEAVE_CRIT_SECTION_NO_TASKLOCK(&global_ps_crit_section);

  DS_MSG1(MSG_LEGACY_HIGH, "Event mask %d", event_mask);
  return (event_mask);

} /* dssocki_socket_event_check() */

/*===========================================================================
FUNCTION DSSOCKI_NOTIFY_APP()

DESCRIPTION
  This function calls the application callback function to alert the
  application that a socket event has occurred.  Note that the application
  callback will not be called if the application has already been notified.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The application callback function is invoked.
===========================================================================*/
void dssocki_notify_app
(
  scb_type *scb_ptr,                        /* ptr to socket control block */
  uint32    event_happenend_mask                        /* Happenend event */
)
{
  boolean notified_cb_flag;  /* indicates if socket cb fcn has been called */
  acb_type * acb_ptr;
  sint15 dss_nethandle;
  dss_sock_callback_info_s *sock_cb_info = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_MSG1(MSG_LEGACY_HIGH, "Notify App Called %d ",event_happenend_mask);
  /*-----------------------------------------------------------------------
    sock_cb can be changed using setsockopt in App context and notify_app
    can be called in PS context. So protect using Critical Section.
   -----------------------------------------------------------------------*/
  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  acb_ptr = scb_ptr->acb_ptr;

  if(scb_ptr->sock_cb != NULL)
  {
    /*----------------------------------------------------------------------
      Check to see if the socket call back is a system level socket call back
      then we use old call back mechanism where the application has to call
      getnextevent to get the event occurred.
      If it is a user specified call back then in the new call back mechanism
      we inform each event independently, clear the event mask  and pass the
      event happenend mask as an argument to the call back function.
    ----------------------------------------------------------------------*/
    if(scb_ptr->sock_cb !=  dssocki_sock_cb)
    {
      scb_ptr->requested_event_mask &= ~event_happenend_mask;
      notified_cb_flag = FALSE;
      if(acb_ptr != NULL)
      {
        dss_nethandle = acb_ptr->dss_nethandle;
      }
      else
      {
        dss_nethandle = 0;
      }
    }
    else
    {
      if( acb_ptr == NULL )
      {
        DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
        DS_ASSERT( 0 );
        return;
      }
      dss_nethandle = acb_ptr->dss_nethandle;
      /*-------------------------------------------------------------------
         Get flag which indicates whether or not the application has been
         notified or not.  Since this is part of a global structure, need to
         protect access to it through TASKLOCK()/TASKFREE().
      -------------------------------------------------------------------*/

      notified_cb_flag = acb_ptr->notified_cb_flag;

      /*---------------------------------------------------------------------
        Application has not yet been notified
      ---------------------------------------------------------------------*/
      if (notified_cb_flag == FALSE)
      {
        /*------------------------------------------------------------------
          Change status of application notification, again protecting through
          TASKLOCK()/TASKFREE().
        -------------------------------------------------------------------*/
        acb_ptr->notified_cb_flag = TRUE;
      }
    }

    /*----------------------------------------------------------------------
      Call the callback function
    -----------------------------------------------------------------------*/
    if(notified_cb_flag == FALSE)
    {
      DS_SYSTEM_HEAP_MEM_ALLOC(sock_cb_info,
                               sizeof(dss_sock_callback_info_s),
                               dss_sock_callback_info_s*);

      if(sock_cb_info == NULL)
      {
        DS_ERROR_LOG_1("Unable to allocate memory for Invoking Sock Callback"
                       " For Sockfd %d",scb_ptr->sockfd);
        DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
        DS_ASSERT(0);
      }
      memset(sock_cb_info, 0, sizeof(dss_sock_callback_info_s));
      sock_cb_info->dss_nethandle = dss_nethandle;
      sock_cb_info->sockfd = scb_ptr->sockfd;
      sock_cb_info->sock_cb_user_data = scb_ptr->sock_cb_user_data;

      if ((event_happenend_mask & DS_WRITE_EVENT) == DS_WRITE_EVENT)
      {
        sock_cb_info->event_mask = DS_WRITE_EVENT;
        scb_ptr->sock_cb(sock_cb_info);
      }

      if ((event_happenend_mask & DS_READ_EVENT) == DS_READ_EVENT)
      {
        sock_cb_info->event_mask = DS_READ_EVENT;
        scb_ptr->sock_cb(sock_cb_info);
      }

      if ((event_happenend_mask & DS_CLOSE_EVENT) == DS_CLOSE_EVENT)
      {
        sock_cb_info->event_mask = DS_CLOSE_EVENT;
        scb_ptr->sock_cb(sock_cb_info);
      }

      if ((event_happenend_mask & DS_ACCEPT_EVENT) == DS_ACCEPT_EVENT)
      {
        sock_cb_info->event_mask = DS_ACCEPT_EVENT;
        scb_ptr->sock_cb(sock_cb_info);
      }

      if (sock_cb_info != NULL)
      {
        DS_SYSTEM_HEAP_MEM_FREE(sock_cb_info);
      }
    }
  }
  else
  {
      DS_ERROR_LOG_0("Socket cback null, so not called");
  }
  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

} /* dssocki_notify_app() */

/*===========================================================================
FUNCTION DSSOCKI_SOCKET_EVENT_OCCURRED()

DESCRIPTION
  This function notifies all interested sockets that a socket event
  has occurred.  If a scb_ptr is specified, this function notifies only that
  socket. If scb_ptr is NULL and acb_ptr is specified, this function notifies
  all sockets for that app. If both scb_ptr and acb_ptr are not specified,
  the function assumes it should provide notification for all sockets. The
  function then loops through the socket control block array and checks each
  that may have occurred.  The application is notified upon any events
  having occurred.  If a valid scb_ptr is specified, the function checks
  that particular socket for any events that may have occurred.  Again,
  the application is notified if any of the events that it is interested
  in, have occurred.

DEPENDENCIES

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dssocki_socket_event_occurred
(
  struct scb_s *scb_ptr,                    /* ptr to socket control block */
  acb_type *acb_ptr                         /* ptr to app control block    */
)
{
  uint32 event_mask;                                    /* Happenend event */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   DS_MSG0(MSG_LEGACY_HIGH, "dssocki_socket_event_occurred");
  /*-------------------------------------------------------------------------
    A NULL scb_ptr indicates that we should notify across all sockets.
    In other words, it is a global event.
   ------------------------------------------------------------------------*/
  if ((scb_ptr == NULL)  && (acb_ptr == NULL))
  {
      /*---------------------------------------------------------------------
        Check for events and notify application
      ---------------------------------------------------------------------*/
    DS_ENTER_CRIT_SECTION(&global_ps_crit_section);
    scb_ptr = (scb_type *) q_check( &sock_config_cb.sock_cb_q );

    while (scb_ptr != NULL)
    {
      DS_ASSERT(SOCKAVAIL != scb_ptr->sockfd);

      event_mask = (uint32)
        dssocki_socket_event_check(scb_ptr,
                                   (scb_ptr->requested_event_mask& DS_WRITE_EVENT));
      if (event_mask != 0)
      {
        dssocki_notify_app(scb_ptr, event_mask);
      }

      scb_ptr
        = (scb_type *) q_next(&sock_config_cb.sock_cb_q, &scb_ptr->link);

    } /* while */
    DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  } /* scb_ptr == NULL && acb_ptr == NULL */
  else if (scb_ptr != NULL) /* Also covers the case where acb_ptr != NULL */
  {
   /*-----------------------------------------------------------------------
      Check for events and notify application
    -----------------------------------------------------------------------*/
    event_mask = (uint32)
        dssocki_socket_event_check(scb_ptr, scb_ptr->requested_event_mask);
    if (event_mask != 0)
    {
      dssocki_notify_app(scb_ptr, event_mask);
    }
  } /* scb_ptr != NULL */
  else if(acb_ptr != NULL)
  {
    /*-----------------------------------------------------------------------
      Check for events and notify application
    -----------------------------------------------------------------------*/
    DS_ENTER_CRIT_SECTION(&global_ps_crit_section);
    scb_ptr = (scb_type *) q_check( &sock_config_cb.sock_cb_q );

    while (scb_ptr != NULL)
    {
      ASSERT(SOCKAVAIL != scb_ptr->sockfd);

      if (scb_ptr->acb_ptr == acb_ptr)
      {
        event_mask = (uint32)
        dssocki_socket_event_check(scb_ptr, scb_ptr->requested_event_mask);
        if (event_mask != 0)
        {
          DS_MSG1(MSG_LEGACY_MED, "Notifying app %d",acb_ptr->dss_nethandle);
          dssocki_notify_app(scb_ptr, event_mask);
        }

        /*-------------------------------------------------------------------
          Bail out after specified application has been notified
        -------------------------------------------------------------------*/
        if (scb_ptr->sock_cb == dssocki_sock_cb)
        {
          break;
        }
      }

      scb_ptr = (scb_type *) q_next(&sock_config_cb.sock_cb_q, &scb_ptr->link);

    } /* while */
    DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  } /* acb_ptr != NULL */
  else
  {
    /*-----------------------------------------------------------------------
      Both scb_ptr and acb_ptr are NULL!
    -----------------------------------------------------------------------*/
    DS_ASSERT(0);
  }

} /* dssocki_socket_event_occured() */

/*===========================================================================
FUNCTION DSSOCKI_SOCK_SET_STATE_CLOSED()

DESCRIPTION
  This function sets the socket state to DSSOCKI_CLOSED. If a NULL parameter
  is passed in, it will change all sockets to DSSOCKI_CLOSED.  Note this is
  only called from the network state routines when the network subsystem has
  gone down.  This sets the closed_state_reason to DS_ENETDOWN.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Sets socket state to DSSOCKI_CLOSED.
===========================================================================*/
void  dssocki_sock_set_state_closed
(
  struct scb_s *scb_ptr           /* ptr to socket control block structure */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    A NULL scb_ptr indicates that we should close all sockets.
    In other words, it is a global close.
  ------------------------------------------------------------------------*/
  if (scb_ptr == NULL)
  {
    DS_ENTER_CRIT_SECTION(&global_ps_crit_section);
    scb_ptr = (scb_type *) q_check( &sock_config_cb.sock_cb_q );
    DS_ASSERT(NULL != scb_ptr);

    while (scb_ptr != NULL)
    {
      DS_ASSERT(SOCKAVAIL != scb_ptr->sockfd);

      /*--------------------------------------------------------------------
        Check state and change if necessary - This is tail recursion.
        This can only happend in the case where scb_ptr is null, and
        since this makes the call with scb_ptr != NULL the call can
        only go 1 deep.  This avoid having all the code that is in
        the next section
        duplicated.
      --------------------------------------------------------------------*/
      dssocki_sock_set_state_closed(scb_ptr);

      scb_ptr
        = (scb_type *) q_next(&sock_config_cb.sock_cb_q, &scb_ptr->link);
    }/* while */
    DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  } /* scb_ptr == NULL */
  else
  {
    /*----------------------------------------------------------------------
      Individual SCB - change state
    ----------------------------------------------------------------------*/
    dssocki_set_closed_state_reason(scb_ptr, NETWORK);
    dssocki_set_sock_state(scb_ptr, DSSOCKI_CLOSED);

  } /* scb_ptr != NULL */

} /* dssocki_sock_set_state_closed() */

/*===========================================================================
FUNCTION DSSOCKI_SET_CLOSED_STATE_REASON()

DESCRIPTION
  Sets the closed_state_reason field in the SCB with the appropriate
  reason for the socket transitioning into the CLOSED state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Sets the closed_state_reason field.
===========================================================================*/
void  dssocki_set_closed_state_reason
(
  struct scb_s *scb_ptr,          /* ptr to socket control block structure */
  sint15   reason                /* reason for socket tranistion to CLOSED */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
     set the error condition - SCB is a part of a global array, so
     need to protect access through TASKLOCK()/TASKFREE().
   ------------------------------------------------------------------------*/
  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  scb_ptr->closed_state_reason = reason;

  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

} /* dssocki_set_closed_state_reason() */

/*===========================================================================
FUNCTION DSSOCKI_GET_ERR_VAL()

DESCRIPTION
  Gets the appropriate error value based on current socket state and
  type of operation (connect(), close(), read(), write()).  This function
  follows the TCP/UDP socket state machines.

DEPENDENCIES
  None.

RETURN VALUE
  Error condition.

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dssocki_get_err_val
(
  struct scb_s *scb_ptr,               /* ptr to socket control block type */
  dssocki_caller_enum_type  type,      /* type of function that is calling
                                          this function.                   */
  sint15 *dss_errno                               /* error condition value */
)
{
  sint15 result;                                           /* return value */
  dssocki_state_enum_type state;                   /* current socket state */
  struct socket servname;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Assert that a valid ptr to a dss_errno variable was specified - Prevents
    dereferencing of NULL ptrs.
  -------------------------------------------------------------------------*/
  if( dss_errno == NULL )
  {
    DS_ERROR_LOG_0("NULL dss_errno passed");
    return DSS_ERROR;
  }
  *dss_errno = DSS_SUCCESS;

  result = DSS_ERROR;

  /*-------------------------------------------------------------------------
    Determine state and return appropriate error code - SCB is part of
    global array, so need to protect access through TASKLOCK()/TASKFREE().
  -------------------------------------------------------------------------*/
  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);
  state = scb_ptr->socket_state;
  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  switch (state)
  {
    /*-----------------------------------------------------------------------
      This should never happen.  NULL socket state implies that the socket
      has not been allocated.
    -----------------------------------------------------------------------*/
    case DSSOCKI_NULL:
      DS_ERROR_LOG_1("Invalid State for sockfd %d", (int) (scb_ptr->sockfd));
      break;

    /*-----------------------------------------------------------------------
      Init state - ASSERT protocol type, since it is only valid for TCP.
    -----------------------------------------------------------------------*/
    case DSSOCKI_INIT:
      DS_ASSERT (scb_ptr->type == DSS_SOCK_STREAM);
      switch(type)
      {
        case DSSOCKI_CONNECT_ROUTINE:
          /*-----------------------------------------------------------------
            Operation would block while TCP opens
          -----------------------------------------------------------------*/
          *dss_errno = DS_EWOULDBLOCK;
          break;

        case DSSOCKI_CLOSE_ROUTINE:
          /*-----------------------------------------------------------------
            Closed - Go back to NULL.
          -----------------------------------------------------------------*/
          result = DSS_SUCCESS;
          break;

        case DSSOCKI_INPUT_ROUTINE:
        case DSSOCKI_OUTPUT_ROUTINE:
          /*-----------------------------------------------------------------
            Socket has never connected.
          -----------------------------------------------------------------*/
          *dss_errno = DS_ENOTCONN;
          break;

        case DSSOCKI_BIND_ROUTINE:
          /*-----------------------------------------------------------------
            Operation ok.
          -----------------------------------------------------------------*/
          result = DSS_SUCCESS;
          break;

        case DSSOCKI_LISTEN_ROUTINE:
          /*-----------------------------------------------------------------
            Operation ok.
          -----------------------------------------------------------------*/
          result = DSS_SUCCESS;
          break;

        case DSSOCKI_ACCEPT_ROUTINE:
          /*-----------------------------------------------------------------
            Socket is not listening
          -----------------------------------------------------------------*/
          *dss_errno = DS_EINVAL;
          break;

        case DSSOCKI_SHUTDOWN_ROUTINE:
          *dss_errno = DS_ENOTCONN;
          break;

        default:
          /*-----------------------------------------------------------------
            Sanity Check - This should never happen.
          -----------------------------------------------------------------*/
          ASSERT(0);
      } /* switch (type) */

      break; /* DSSOCKI_INIT */

    /*-----------------------------------------------------------------------
      Listen state - Only valid for TCP - Passive Open
    -----------------------------------------------------------------------*/
    case DSSOCKI_LISTEN:
      DS_ASSERT (scb_ptr->type == DSS_SOCK_STREAM);
      switch(type)
      {
        case DSSOCKI_CONNECT_ROUTINE:
          *dss_errno = DS_EINVAL;
          break;

        case DSSOCKI_CLOSE_ROUTINE:
          result = DSS_SUCCESS;
          break;

        case DSSOCKI_INPUT_ROUTINE:
        case DSSOCKI_OUTPUT_ROUTINE:
          *dss_errno = DS_EINVAL;
          break;

        case DSSOCKI_BIND_ROUTINE:
          *dss_errno = DS_EINVAL;
          break;

        case DSSOCKI_LISTEN_ROUTINE:
          *dss_errno = DS_EINVAL;
          break;

        case DSSOCKI_ACCEPT_ROUTINE:
          *dss_errno = DS_EWOULDBLOCK;
          break;

        case DSSOCKI_SHUTDOWN_ROUTINE:
          *dss_errno = DS_ENOTCONN;
          break;

        default:
          /*-----------------------------------------------------------------
            Sanity Check - This should never happen.
          -----------------------------------------------------------------*/
          ASSERT(0);
      } /* switch (type) */
      break; /* DSSOCKI_LISTEN */

    /*-----------------------------------------------------------------------
      Opening state - Only valid for TCP - Connection attempt in progress -
    -----------------------------------------------------------------------*/
    case DSSOCKI_OPENING:
      DS_ASSERT (scb_ptr->type == DSS_SOCK_STREAM);
      switch(type)
      {
        case DSSOCKI_CONNECT_ROUTINE:
          /*-----------------------------------------------------------------
            TCP connection is already in progress.
          -----------------------------------------------------------------*/
          *dss_errno = DS_EINPROGRESS;
          break;

        case DSSOCKI_CLOSE_ROUTINE:
        case DSSOCKI_INPUT_ROUTINE:
        case DSSOCKI_OUTPUT_ROUTINE:
          /*-----------------------------------------------------------------
            The operations would block.
          -----------------------------------------------------------------*/
          *dss_errno = DS_EWOULDBLOCK;
          break;

        case DSSOCKI_BIND_ROUTINE:
        case DSSOCKI_LISTEN_ROUTINE:
        case DSSOCKI_ACCEPT_ROUTINE:
          /*-----------------------------------------------------------------
            Can't accept or listen on a connected socket.
          -----------------------------------------------------------------*/
          *dss_errno = DS_EINVAL;
          break;

        case DSSOCKI_SHUTDOWN_ROUTINE:
          *dss_errno = DS_ENOTCONN;
          break;

        default:
          /*-----------------------------------------------------------------
            Sanity Check - This should never happen.
          -----------------------------------------------------------------*/
          DS_ASSERT(0);
      } /* switch (type) */

      break; /* DSSOCKI_OPENING */

    /*-----------------------------------------------------------------------
      Socket is OPEN - For TCP, closing would issue and DS_EWOULDBLOCK,
      whereas for UDP, returns immediately with success.
    -----------------------------------------------------------------------*/
    case DSSOCKI_OPEN:
      switch(type)
      {
        case DSSOCKI_CONNECT_ROUTINE:
          if (scb_ptr->type == DSS_SOCK_STREAM)
          {
            /*---------------------------------------------------------------
              TCP is already connected.
            ---------------------------------------------------------------*/
            *dss_errno = DS_EISCONN;
          }
          else
          {
            /*---------------------------------------------------------------
              UDP always successful.
            ---------------------------------------------------------------*/
            result = DSS_SUCCESS;
          }
          break;

        case DSSOCKI_CLOSE_ROUTINE:
          if (scb_ptr->type == DSS_SOCK_STREAM)
          {
            /*---------------------------------------------------------------
              TCP initiates an active close.
            ---------------------------------------------------------------*/
            *dss_errno = DS_EWOULDBLOCK;
          }
          else
          {
            /*---------------------------------------------------------------
              UDP closes immediately
            ---------------------------------------------------------------*/
            result = DSS_SUCCESS;
          }
          break;

        case DSSOCKI_INPUT_ROUTINE:
          if (scb_ptr->state_flags & (uint16)DSSOCKI_CANTRECVMORE )
          {
            *dss_errno = DS_EEOF;
          }
          else
          {
            result = DSS_SUCCESS;
          }
          break;

        case DSSOCKI_OUTPUT_ROUTINE:
          /*-----------------------------------------------------------------
            If CANTSENDMORE flag is set then it is an error to try to send
            data.
          -----------------------------------------------------------------*/
          if (scb_ptr->state_flags & (uint16)DSSOCKI_CANTSENDMORE )
          {
            *dss_errno = DS_ESHUTDOWN;
          }
          else
          {
            result = DSS_SUCCESS;
          }
          break;

        case DSSOCKI_BIND_ROUTINE:
          if (scb_ptr->type == DSS_SOCK_STREAM)
          {
            /*---------------------------------------------------------------
              TCP is already connected.
            ---------------------------------------------------------------*/
            *dss_errno = DS_EINVAL;
          }
          else
          {
            /*---------------------------------------------------------------
              UDP always successful.
            ---------------------------------------------------------------*/
            result = DSS_SUCCESS;
          }
          break;

        case DSSOCKI_LISTEN_ROUTINE:
        case DSSOCKI_ACCEPT_ROUTINE:
          /*-----------------------------------------------------------------
            Can't listen or accept on an open socket.
          -----------------------------------------------------------------*/
          *dss_errno = DS_EINVAL;
          break;

        case DSSOCKI_SHUTDOWN_ROUTINE:
          /*-----------------------------------------------------------------
            shutdown() is permitted only for connected sockets
          -----------------------------------------------------------------*/
          if (scb_ptr->type == DSS_SOCK_STREAM)
          {
            result = DSS_SUCCESS;
          }
          else
          {
            DSSOCKI_CACHE_GET_REMOTE( scb_ptr, servname );

            if(!(PS_IN6_ARE_ADDR_EQUAL(&servname.address, &ps_in6addr_any )) &&
                (servname.port != 0))
            {
              *dss_errno = DSS_SUCCESS;
            }
            else
            {
              *dss_errno = DS_ENOTCONN;
            }
          }
          break;

        default:
        /*-------------------------------------------------------------------
          Sanity Check - This should never happen.
        -------------------------------------------------------------------*/
          DS_ASSERT(0);

      } /* switch (type) */
      break; /* DSSOCKI_OPEN */

    case DSSOCKI_CLOSED:
      /*---------------------------------------------------------------------
      Socket is closed - no TCP connection exists or network went down
      ---------------------------------------------------------------------*/
      switch(type)
      {
        case DSSOCKI_CONNECT_ROUTINE:
          if (scb_ptr->type == DSS_SOCK_STREAM)
          {
            /*---------------------------------------------------------------
              Determine connection failure status.
            ------ ---------------------------------------------------------*/
            *dss_errno = dsstcp_get_conn_error(scb_ptr,
                                               DSSOCKI_CONNECT_ROUTINE);
          }
          else
          {
            /*---------------------------------------------------------------
              UDP is only closed through network failure.
            ---------------------------------------------------------------*/
            *dss_errno = DS_ENETDOWN;
          }
          break;

        case DSSOCKI_CLOSE_ROUTINE:
          /*-----------------------------------------------------------------
            No error.
          -----------------------------------------------------------------*/
          result = DSS_SUCCESS;
          break;

        case DSSOCKI_INPUT_ROUTINE:
        case DSSOCKI_OUTPUT_ROUTINE:
          if (scb_ptr->type == DSS_SOCK_STREAM)
          {
            /*---------------------------------------------------------------
              I/O-specific connection failure status.
            ---------------------------------------------------------------*/
            *dss_errno = dsstcp_get_conn_error(scb_ptr, type);
          }
          else
          {
            /*---------------------------------------------------------------
              UDP is only closed through network failure.
            ---------------------------------------------------------------*/
            *dss_errno = DS_ENETDOWN;
          }
          break;

        case DSSOCKI_BIND_ROUTINE:
        case DSSOCKI_LISTEN_ROUTINE:
        case DSSOCKI_ACCEPT_ROUTINE:
          /*-----------------------------------------------------------------
            Can't listen or accept on a closed socket.
          -----------------------------------------------------------------*/
          *dss_errno = DS_EINVAL;
          break;

        case DSSOCKI_SHUTDOWN_ROUTINE:
          *dss_errno = DS_ENOTCONN;
          break;

        default:
          /*-----------------------------------------------------------------
          Sanity Check - This should never happen.
          -----------------------------------------------------------------*/
          DS_ASSERT(0);

      } /* switch (type) */

      break; /* DSSOCKI_CLOSED */

    /*-----------------------------------------------------------------------
      CLOSING state reached by a close() issued on TCP socket - This state
      is only valid for TCP sockets, thus ASSERT socket state.
    -----------------------------------------------------------------------*/
    case DSSOCKI_CLOSING:
      DS_ASSERT (scb_ptr->type == DSS_SOCK_STREAM);
      switch(type)
      {
        case DSSOCKI_CONNECT_ROUTINE:
          /*-----------------------------------------------------------------
            No connection.
          -----------------------------------------------------------------*/
          *dss_errno = DS_EINVAL;
          break;

        case DSSOCKI_CLOSE_ROUTINE:
          /*-----------------------------------------------------------------
            Waiting for TCP to fully close.
          -----------------------------------------------------------------*/
          *dss_errno = DS_EWOULDBLOCK;
          break;

        case DSSOCKI_INPUT_ROUTINE:
          /*-----------------------------------------------------------------
            If cantrecvmore flag is not set, it is okay to read, otherwise
            return EEOF.
          -----------------------------------------------------------------*/
          if (scb_ptr->state_flags & (uint16)DSSOCKI_CANTRECVMORE)
          {
            *dss_errno = DS_EEOF;
          }
          else
          {
            result = DSS_SUCCESS;
          }
          break;

        case DSSOCKI_OUTPUT_ROUTINE:
          /*-----------------------------------------------------------------
            Whether we reached CLOSING state due to shutdown() or close()
            call, it is an error to try to write() because TCP has sent a
            FIN to the peer indicating that it won't send any more data
          -----------------------------------------------------------------*/
          *dss_errno = DS_ESHUTDOWN;
          break;

        case DSSOCKI_BIND_ROUTINE:
        case DSSOCKI_LISTEN_ROUTINE:
        case DSSOCKI_ACCEPT_ROUTINE:
          /*-----------------------------------------------------------------
            Can't listen or accept on a closed socket.
          -----------------------------------------------------------------*/
          *dss_errno = DS_EINVAL;
          break;

        case DSSOCKI_SHUTDOWN_ROUTINE:
          *dss_errno = DS_ESHUTDOWN;
          break;

        default:
          /*-----------------------------------------------------------------
            Sanity Check - This should never happen.
          -----------------------------------------------------------------*/
          DS_ASSERT(0);

      } /* switch (type) */

      break; /* DSSOCKI_CLOSING */

    /*-----------------------------------------------------------------------
      WAIT_FOR_CLOSE state reached by a close() issued on TCP socket -
      TCP has fully closed, and is waiting for application to make another
      call to close() for clean-up.  This state is only valid for TCP
      sockets, thus ASSERT socket state.
    -----------------------------------------------------------------------*/
    case DSSOCKI_WAIT_FOR_CLOSE:
      ASSERT (scb_ptr->type == DSS_SOCK_STREAM);
      switch(type)
      {
        case DSSOCKI_CONNECT_ROUTINE:
          /*-----------------------------------------------------------------
            No connection.
          -----------------------------------------------------------------*/
          *dss_errno = DS_ENOTCONN;
          break;

        case DSSOCKI_CLOSE_ROUTINE:           /* no error - just cleans up */
          /*-----------------------------------------------------------------
            No error - just cleans up.
          -----------------------------------------------------------------*/
          result = DSS_SUCCESS;
          break;

        case DSSOCKI_INPUT_ROUTINE:
          /*-------------------------------------------------------------
            Half way socket close - return EOF
           ------------------------------------------------------------*/
          *dss_errno = DS_EEOF;
          break;

        case DSSOCKI_OUTPUT_ROUTINE:
          /*-----------------------------------------------------------------
            For I/O routines, we are no longer connected.
          -----------------------------------------------------------------*/
          *dss_errno = DS_ENOTCONN;
          break;

        case DSSOCKI_BIND_ROUTINE:
        case DSSOCKI_LISTEN_ROUTINE:
        case DSSOCKI_ACCEPT_ROUTINE:
          /*-----------------------------------------------------------------
            Can't listen or accept on a closed socket.
          -----------------------------------------------------------------*/
          *dss_errno = DS_EINVAL;
          break;

        case DSSOCKI_SHUTDOWN_ROUTINE:
          *dss_errno = DS_ENOTCONN;
          break;

        default:
          /*-----------------------------------------------------------------
            Sanity Check - This should never happen.
          -----------------------------------------------------------------*/
          DS_ASSERT(0);

      } /* switch (type) */

      break; /* DSSOCKI_WAIT_FOR_CLOSE */

    default:
    /*-----------------------------------------------------------------------
      Sanity Check - This should never happen.
    -----------------------------------------------------------------------*/
      DS_ASSERT(0);

  } /* switch (state) */

  return (result);

} /* dssocki_get_err_val() */

/*===========================================================================
FUNCTION DSSOCKI_CHECK_SOCK_EXISTS()

DESCRIPTION

  This API check's if there are any sockets linked with the ACB

DEPENDENCIES
  None.

RETURN VALUE
      0  -- if all sockets for the ACB are closed
     -1  -- if sockets are not closed

SIDE EFFECTS
  None
==============================================================================*/
int dssocki_check_sock_exists
(
  acb_type *acb_ptr 
)
{
  struct scb_s                 * scb_ptr;
  boolean                      open_socket = FALSE;

  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  /*-------------------------------------------------------------------------
    Ensure that all sockets are closed for the application being closed.
  -------------------------------------------------------------------------*/
  scb_ptr = (scb_type *) q_check( &sock_config_cb.sock_cb_q );
  while (NULL != scb_ptr)
  {
    if ((scb_ptr->acb_ptr != NULL) &&
         scb_ptr->acb_ptr->dss_nethandle == acb_ptr->dss_nethandle)
    {
      DS_MSG2(MSG_LEGACY_ERROR,"Socket (sockfd %d)is open for this ACB %p",
              scb_ptr->sockfd, acb_ptr);
      open_socket = TRUE;
    }

    scb_ptr
        = (scb_type *) q_next(&sock_config_cb.sock_cb_q, &scb_ptr->link);
  }

  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  if (open_socket == TRUE)
    return DSS_ERROR;
  else
    return DSS_SUCCESS;
}

/*===========================================================================*/
boolean dssocki_reg_ev
(
  sint15                           dss_nethandle,
  ds_pdn_ioctl_event_cb            f_ptr,
  ds_pdn_event_enum_type            event,
  void                             *user_info_ptr
)
{
  dssocki_event_buf_type                 * event_buf_ptr;
  q_type                                 * working_q_ptr;
  acb_type                               * acb_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ASSERT(event < PDN_EVENT_MAX);

  acb_ptr = dssocki_dss_nethandletoacb(dss_nethandle);
  if(acb_ptr == NULL)
  {
    DS_ERROR_LOG_1("Invalid dss_nethandle %d",dss_nethandle);
    return FALSE;
  }

  if(f_ptr == NULL)
  {
    DS_ERROR_LOG_1("App %d passed in Null callback function",dss_nethandle);
    return FALSE;
  }

  DS_ENTER_CRIT_SECTION_NO_TASKLOCK(&global_ps_crit_section);

  /*-------------------------------------------------------------------------
    Check to see if there is a callback already registered for the
    specified event by this application.

    Applications should not register for PDN_UP_EV and PDN_DOWN_EV using the
    DS_PDN_IOCTL_REG_EVENT_CB IOCTL
  -------------------------------------------------------------------------*/
  working_q_ptr = (acb_ptr->event_q_array) + (uint32)event;

  event_buf_ptr = q_check(working_q_ptr);
  if (event == PDN_UP_EV || event == PDN_DOWN_EV)
  {
    if (event_buf_ptr != NULL)
    {
      DS_LEAVE_CRIT_SECTION_NO_TASKLOCK(&global_ps_crit_section);
      DS_MSG2(MSG_LEGACY_HIGH,"App %d already has callback for ev %d",
               dss_nethandle, event);
      return FALSE;
    }
  }
  else
  {
    while(event_buf_ptr != NULL)
    {
      if((event_buf_ptr->fw_inst == acb_ptr->fw_inst))
      {
        DS_LEAVE_CRIT_SECTION_NO_TASKLOCK(&global_ps_crit_section);
        DS_MSG2(MSG_LEGACY_HIGH,"App %d already has callback for ev%d",
                 dss_nethandle, event);
        return FALSE;
      }
      else
      {
        event_buf_ptr = q_next(working_q_ptr,&(event_buf_ptr->link));
      }
    }
  }

  /*-------------------------------------------------------------------------
    Allocate a memory buffer for dssocki_event_buf_type
  -------------------------------------------------------------------------*/
  event_buf_ptr = (dssocki_event_buf_type *)
                                ps_mem_get_buf(PS_MEM_DSS_IOCTL_EVT_CB_TYPE);
  if(event_buf_ptr == NULL)
  {
     DS_LEAVE_CRIT_SECTION_NO_TASKLOCK(&global_ps_crit_section);
     DS_ERROR_LOG_0("Event buf allocation failed");
     return FALSE;
  }

  /*-------------------------------------------------------------------------
    Initialize the event_buf_ptr.
  -------------------------------------------------------------------------*/
  (void)q_link(event_buf_ptr, &(event_buf_ptr->link));

  event_buf_ptr->f_ptr         = f_ptr;
  event_buf_ptr->dss_nethandle = dss_nethandle;
  event_buf_ptr->user_info_ptr = user_info_ptr;
  event_buf_ptr->fw_inst       = acb_ptr->fw_inst;

  /*-------------------------------------------------------------------------
    Enqueue the event callback.
  -------------------------------------------------------------------------*/
  q_put(working_q_ptr, &(event_buf_ptr->link));
  DS_MSG2(MSG_LEGACY_MED, "Ev%d callback registered for app %d",
          event,dss_nethandle);

  DS_LEAVE_CRIT_SECTION_NO_TASKLOCK(&global_ps_crit_section);

  return TRUE;

} /* dssocki_reg_ev() */

/*===========================================================================
FUNCTION DSSOCKI_DEREG_EV()

DESCRIPTION
  This function deregisters the application callback for the specified event.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE      If deregistration is successful.
  FALSE     If deregistration failed.

SIDE EFFECTS
  None.

===========================================================================*/
boolean dssocki_dereg_ev
(
  sint15                           dss_nethandle,
  ds_pdn_ioctl_event_cb            f_ptr,
  ds_pdn_event_enum_type           event,
  void                             *user_info_ptr
)
{

  dssocki_event_buf_type *event_buf_ptr=NULL;
  acb_type *acb_ptr=NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ASSERT(event < PDN_EVENT_MAX);

  acb_ptr = dssocki_dss_nethandletoacb(dss_nethandle);
  if(acb_ptr == NULL)
  {
    DS_ERROR_LOG_1("Invalid dss_nethandle %d",dss_nethandle);
    return FALSE;
  }

  /*------------------------------------------------------------------------
    Check to see if there is a callback already registered for the
    specified event by this application.
  ------------------------------------------------------------------------*/
  DS_ENTER_CRIT_SECTION_NO_TASKLOCK(&global_ps_crit_section);

  event_buf_ptr = q_check((acb_ptr->event_q_array) + (uint32)event);
  while(event_buf_ptr != NULL)
  {
    if(event_buf_ptr->fw_inst== acb_ptr->fw_inst)
    {
      break;
    }
    else
    {
      event_buf_ptr = q_next((acb_ptr->event_q_array) + (uint32)event,
                             &(event_buf_ptr->link));
    }
  }

  if(event_buf_ptr == NULL)
  {
    DS_LEAVE_CRIT_SECTION_NO_TASKLOCK(&global_ps_crit_section);
    DS_ERROR_LOG_1("Could not find callback for ev%d",event);
    return FALSE;
  }

  /*------------------------------------------------------------------------
    Remove this callback from the event queue.
  ------------------------------------------------------------------------*/
#ifdef FEATURE_Q_NO_SELF_QPTR
  q_delete((acb_ptr->event_q_array) + (uint32)event, &(event_buf_ptr->link));
#else
  q_delete(&(event_buf_ptr->link));
#endif

  DS_LEAVE_CRIT_SECTION_NO_TASKLOCK(&global_ps_crit_section);
  /*------------------------------------------------------------------------
    Free the associated buffer.
  ------------------------------------------------------------------------*/
  DS_MSG2(MSG_LEGACY_MED,"Ev%d callback deregistered for app %d",
          event,dss_nethandle);
  PS_MEM_FREE(event_buf_ptr);

  return TRUE;

} /* dssocki_dereg_ev() */



/*===========================================================================
FUNCTION      DSSOCKI_PROCESS_PDN_EV_CBACK()

DESCRIPTION   Called when the PDN event happens. This calls any app
              registered callbacks for this event.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void dssocki_process_pdn_ev_cback
(
  ds_pdn_event_enum_type     event,
  ds_pdn_event_info_u_type   event_info
)
{
  acb_type *curr_acb_ptr = NULL;

  DS_MSG1(MSG_LEGACY_HIGH,"dssocki_process_pdn_ev_cback event %d",event);

/*-----------------------------------------------------------------------
    Loop through all the apps and notify apps that have registered for
    events on the ps_iface on which the event occured.
-----------------------------------------------------------------------*/
  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  curr_acb_ptr = (acb_type *) q_check( &sock_config_cb.app_cb_q );
  while ( curr_acb_ptr != NULL)
  {

    DS_ASSERT(APPAVAIL != curr_acb_ptr->dss_nethandle);

    dssocki_invoke_app_ev_cbacks(curr_acb_ptr,
                                 event,
                                 event_info);

    curr_acb_ptr
        = (acb_type *) q_next(&sock_config_cb.app_cb_q, &curr_acb_ptr->link);
  } /* while */

  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
}


 /*===========================================================================
 FUNCTION	   DSSOCKI_INVOKE_APP_EV_CBACKS()
 
 DESCRIPTION   This function invokes callbacks registered by the app (using
			   the IOCTL interface) for the specified event.
 
 DEPENDENCIES  None.
 
 RETURN VALUE  None.
 
 SIDE EFFECTS  None.
 ===========================================================================*/
void dssocki_invoke_app_ev_cbacks
 (
   acb_type                   *acb_ptr,
   ds_pdn_event_enum_type     event,
   ds_pdn_event_info_u_type   event_info
 )
{
   dssocki_event_buf_type *event_buf_ptr = NULL;
   q_type *working_q_ptr=NULL;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ASSERT(acb_ptr != NULL);

  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);
  /*------------------------------------------------------------------------
     Notify app callbacks for this event. Note that the app could
     potentially be using a different interface currently.
  ------------------------------------------------------------------------*/
  if(acb_ptr->dss_nethandle != APPAVAIL)
  {
    working_q_ptr = (acb_ptr->event_q_array) + (uint32)event;

    event_buf_ptr = q_check(working_q_ptr);
    while(event_buf_ptr != NULL)
    {
      DS_ASSERT(event_buf_ptr->fw_inst != NULL);

      if( event_buf_ptr->fw_inst== acb_ptr->fw_inst)
      {
        DS_MSG2(MSG_LEGACY_MED, "Calling callback for ev%d for app%d",
                event,
                acb_ptr->dss_nethandle);
         DS_ASSERT(event_buf_ptr->f_ptr != NULL);

  /*------------------------------------------------------------------
     Callback's should be called under TASKLOCK's. In the code without
     L4 feature these TASKLOCK's are taken care by DS_ENTER_CRIT_SECTION
     above. Using ifdef here so that with L4 feature callback is under
     TASKLOCK.
  ------------------------------------------------------------------*/
         event_buf_ptr->f_ptr(event,
                              event_info,
                              event_buf_ptr->user_info_ptr,
                              acb_ptr->dss_nethandle);
      }
  /*---------------------------------------------------------------------
    Since a phys link could be shared among multiple ifaces, events may
    have mutiple clients waiting for it.  Hence continue scanning the
    queue.
  ---------------------------------------------------------------------*/
      event_buf_ptr = q_next(working_q_ptr,&(event_buf_ptr->link));
    }
  }
   DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
} /* dssocki_invoke_app_ev_cbacks() */



/*===========================================================================

FUNCTION DSSOCKI_REG_IP_FILTER()

DESCRIPTION
  This function registers IP filters for the specified iface. The filters are
  passed down to the iface to be added to iface control block while a mapping
  of the iface is added to SCB of the specified socket. This mapping is used
  to deregister these filters when the socket is closed.

DEPENDENCIES
  None.

PARAMETERS
  sockfd      - Descriptor of the Socket requesting filtering
  iface_ptr   - Iface to add filters to
  filter_type - Type of filter
  filters     - Ptr to list of filters (total num_filters) where each
                filter is of type specified above -
                  => ip_filter_type for IP_FILTER_DEFAULT_TYPE
  num_filters - Number of filters passed in
  dss_errno   - Ptr to contain error code on the return

RETURN VALUE
   0 on success
  -1 for error (sets dss_errno appropriately)

SIDE EFFECTS
  None.

===========================================================================*/
int dssocki_reg_ip_filter
(
  sint15                     sockfd,
  uint8                      num_filters,
  void                       *filters,
  uint32                     *filter_handle,
  sint15                     *dss_errno
)
{
  ds_ipfltr_add_param_type        fltr_add_param;
  struct scb_s                    * scb_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(dss_errno == NULL)
  {
    DS_ERROR_LOG_0("dss_errno is NULL");
    return DSS_ERROR;
  }

  *dss_errno = DSS_SUCCESS;

  if(filters == NULL)
  {
    *dss_errno = DS_EFAULT;
    DS_ERROR_LOG_0("Null filter to reg_ip_filter");
    return DSS_ERROR;
  }

  if((scb_ptr = dssocki_sockfdtoscb(sockfd)) == NULL)
  {
    *dss_errno = DS_EBADF;
    DS_ERROR_LOG_1("Bad sockfd %d to reg_ip_filter", sockfd);
    return DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    Add filters to the FWK
  -------------------------------------------------------------------------*/
  fltr_add_param.client_type = DS_FWK_CLIENT_SOCKETS;
  fltr_add_param.client_id = FLTR_CLIENT_SOCKETS;
  fltr_add_param.fi_ptr_arr = filters;
  fltr_add_param.fi_result = (ds_ipfltr_result_type)(uint16) sockfd;
  fltr_add_param.fltr_priority = DS_IPFLTR_PRIORITY_DEFAULT;
  fltr_add_param.is_validated = FALSE;
  fltr_add_param.num_filters = num_filters;

  if (scb_ptr->acb_ptr != NULL)
  {
    fltr_add_param.client_handle = scb_ptr->acb_ptr->dss_handle;
    fltr_add_param.subset_id = (uint32)(uint16)scb_ptr->acb_ptr->dss_nethandle;
   /*-------------------------------------------------------------------------
    Keep a mapping of the FWK Instance in the SCB to cleanup filters when the
    socket is closed
  -------------------------------------------------------------------------*/
    scb_ptr->fltr_info.fw_inst = scb_ptr->acb_ptr->fw_inst;
  }
  else
  {
    fltr_add_param.client_handle = DSS_INVALID_HANDLE;
    fltr_add_param.subset_id = DS_IPFLTR_SUBSET_ID_DEFAULT;
    scb_ptr->fltr_info.fw_inst = NULL;
  }


  scb_ptr->fltr_info.ipfltr_handle = ds_ipfltr_add(&fltr_add_param,  dss_errno);

  if (scb_ptr->fltr_info.ipfltr_handle == DS_IPFLTR_INVALID_HANDLE)
  {
    return DSS_ERROR;
  }
  else
  {
    *filter_handle = scb_ptr->fltr_info.ipfltr_handle;
  }

  return 0;

} /* dssocki_reg_ip_filter() */



/*===========================================================================
FUNCTION DSSOCKI_PKTINFO_DIFFER

DESCRIPTION
  Diff the packet info.
DEPENDENCIES
  None.

RETURN VALUE
 Retruns True if the pktinfo differs from the cached pkt info
 else returns FALSE

SIDE EFFECTS
  None.
===========================================================================*/
boolean dssocki_pkt_info_differ
(
  scb_type            * scb_ptr,
  struct ps_sockaddr_in6 * v6_sockaddr
)
{
  struct socket              localname;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!PS_IN6_IS_ADDR_V4MAPPED(&v6_sockaddr->ps_sin6_addr))
  {
    if(!PS_IN6_ARE_ADDR_EQUAL(
               &scb_ptr->pkt_info_cache.ip_hdr.v6.hdr_body.base_hdr.dst_addr,
               &v6_sockaddr->ps_sin6_addr))
    {
      return TRUE;
    }
  }
  else
  {
    if (scb_ptr->pkt_info_cache.ip_hdr.v4.dest.ps_s_addr !=
          PS_IN6_GET_V4_FROM_V4_MAPPED_V6(&v6_sockaddr->ps_sin6_addr))
    {
      return TRUE;
    }
  }

  DSSOCKI_CACHE_GET_LOCAL(scb_ptr, localname);
  switch (scb_ptr->protocol)
  {
    case PS_IPPROTO_TCP:
      if((scb_ptr->pkt_info_cache.ptcl_info.tcp.src_port != localname.port) ||
         (scb_ptr->pkt_info_cache.ptcl_info.tcp.dst_port !=
          v6_sockaddr->ps_sin6_port))
      {
        return TRUE;
      }
      break;

    case PS_IPPROTO_UDP:
      if((scb_ptr->pkt_info_cache.ptcl_info.udp.src_port != localname.port) ||
         (scb_ptr->pkt_info_cache.ptcl_info.udp.dst_port !=
          v6_sockaddr->ps_sin6_port))
      {
        return TRUE;
      }
      break;

    default:
      break;
  }

  return FALSE;

} /* dssocki_pkt_info_differ() */


/*===========================================================================
FUNCTION DSSOCKI_SENDMSG()

DESCRIPTION
  This function is a common write function for all the socket output
  functions. The message header contains an array of scattered buffers, a
  socket descriptor and destination address for unconnected udp sockets.
  The function writes data from the scattered buffers over the transport
  specified by the socket descriptor.
DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes to be written, in case of tcp it could be less
  than the number of bytes specified.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ---------------
  DS_EBADF            invalid socket descriptor is specfied
  DS_EAFNOSUPPORT     address family not supported
  DS_EWOULDBLOCK      operation would block
  DS_EADDRREQ         destination address required
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           bad memory address
  DS_EOPNOTSUPP       option not supported
  DS_EMSGSIZE         the msg is too large to be sent all at once
  DS_EISCONN          if the socket is connected and the destination
                      address is other than it is connected to.
  DS_ENOMEM           Out of memory (when the mobile runs out of metainfo
                      items)

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dssocki_sendmsg
(
  sint15                  sockfd,  /* socket descriptor                    */
  struct dss_msghdr     * msg,     /* Header containing data and dest addr */
  dsm_item_type        ** msg_ptr, /* Optional payload already in DSMs     */
  int                     flags,   /* flags used for SDB (if enabled)      */
  sint15                * dss_errno /* error condition value               */
)
{
  struct scb_s          * scb_ptr;      /* Ptr to socket control block     */
  sint15                  count;        /* count of number of bytes copied */
  sint15                  result = 0;   /* error checking return value     */
  dsm_item_type         * item_ptr=NULL;/* data services memory pool item  */
  sint15                  i;            /* local counter variable          */
  uint32                  nbytes = 0;   /* total number of bytes specified */
  struct ps_sockaddr_in6     v6_sockaddr;  /* destination adddress struct     */
  struct ps_sockaddr_in6     local_sockaddr; /* source address struct         */
  struct ps_sockaddr_in6   * ta_ptr     ;  /* pointer to v6 struct            */
  ps_tx_meta_info_type     * tx_meta_info_ptr=NULL;/* PS meta info               */
  struct socket           servname;     /* servname from SCB cache         */
  struct socket           localname;    /* localname from SCB cache        */
  ds_fwk_s_type          * routing_cache; /* routing cache from SCB         */
  ds_flow_type          * flow_ptr = NULL;     /* routing cache from SCB          */
  dssocki_app_to_ps_cmd_type  * ps_cmd;
  boolean                 flow_available = FALSE;
  ip_pkt_info_type        pkt_info;
  boolean                 ipcb_meta_info_valid = FALSE;
  int                     sndq_limit = 0;   /* Limit on send queue size        */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /*-------------------------------------------------------------------------
    Assert that a valid ptr to a dss_errno variable was specified - Prevents
    dereferencing of NULL ptrs.
  -------------------------------------------------------------------------*/
  if (dss_errno == NULL)
  {
    DS_MSG0(MSG_LEGACY_HIGH,
            "DSSOCKI_SENDMSG: Null pointer passed for dss_errno\n");
    return(DSS_ERROR);
  }
  *dss_errno = DSS_SUCCESS;

  /*-------------------------------------------------------------------------
    Make sure that the flags that are set are within the range that we
    support - if no return an error.  Otherwise get a meta_info block and put
    the flags in there.
    NOTE: the value of MSG_RESERVED bits changes based on the IS200o_SDB
          feature.
  -------------------------------------------------------------------------*/
  if((flags & MSG_RESERVED_BITS) != 0)
  {
    *dss_errno = DS_EOPNOTSUPP;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Invalid socket descriptor specified.
  -------------------------------------------------------------------------*/
  if ( (scb_ptr = dssocki_sockfdtoscb(sockfd)) == NULL)
  {
    *dss_errno = DS_EBADF;
    DS_MSG1(MSG_LEGACY_HIGH, "Bad sockfd %d to WRITEcheck", sockfd);
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Check for any error conditions - if it will fail, return immediately
  -------------------------------------------------------------------------*/
  result = dssocki_get_err_val(scb_ptr, DSSOCKI_OUTPUT_ROUTINE, dss_errno);
  if (result == DSS_ERROR)
  {
    DS_MSG2(MSG_LEGACY_ERROR, "sockfd %d ERR %d in WRITEcheck", sockfd, *dss_errno);
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    At this point iovcount is non-zero. Hence, iov should be non-NULL.
  -------------------------------------------------------------------------*/
  if ( msg->msg_iov == NULL  && msg->msg_iovlen != 0)
  {
    *dss_errno = DS_EFAULT;
    return DSS_ERROR;
  }


  if(IS_SOCKOPT_SET(scb_ptr->options_mask, (uint64)DSS_SO_ERROR_ENABLE))
  {
    DS_ENTER_CRIT_SECTION(&global_ps_crit_section);
    if(scb_ptr->so_error)
    {
      *dss_errno = scb_ptr->so_error;
      scb_ptr->so_error = 0;
      DSSOCKI_RESET_ERROR_AVAILABLE(scb_ptr);
      DS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      return (DSS_ERROR);
    }
    DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  }

  DSSOCKI_CACHE_GET_REMOTE( scb_ptr, servname );

  /*-------------------------------------------------------------------------
    Validate the specified server address - if it is NULL,or the value is 0,
    check to see if server address has already been specified - if not,
    return error as DS_EADDRREQ.
  -------------------------------------------------------------------------*/
  if( dssocki_get_sockaddr_in6(&v6_sockaddr, msg->msg_name, msg->msg_namelen,
                               dss_errno) == DSS_ERROR)
  {
    DS_MSG1(MSG_LEGACY_HIGH, "Invalid Local address: ERRNO %d", *dss_errno);
    return DSS_ERROR;
  }

  if( PS_IN6_ARE_ADDR_EQUAL(&v6_sockaddr.ps_sin6_addr, &ps_in6addr_any))
  {
    /*---------------------------------------------------------------------
      Valid to_addr pointer, but address is 0 - check to see if server
      address already specified - If not, return error.
    ---------------------------------------------------------------------*/
    if (PS_IN6_ARE_ADDR_EQUAL(&servname.address, &ps_in6addr_any) )
    {
      *dss_errno = DS_EADDRREQ;
      return (DSS_ERROR);
    }
    ta_ptr   = NULL;
    v6_sockaddr.ps_sin6_addr = servname.address;
    v6_sockaddr.ps_sin6_port = servname.port;
    v6_sockaddr.ps_sin6_flowinfo = DSSOCKI_CACHE_GET_FLOW_LABEL( scb_ptr);
  }
  else
  {
    /*-----------------------------------------------------------------------
      Since the address is specified, it is an error if the socket is
      already connected
    -----------------------------------------------------------------------*/
    if(!PS_IN6_ARE_ADDR_EQUAL(&servname.address, &ps_in6addr_any))
    {
      *dss_errno = DS_EISCONN;
      return DSS_ERROR;
    }
    ta_ptr   = &v6_sockaddr;
  }

  /*-------------------------------------------------------------------------
    Determine if a non-zero number of bytes have been specified. If so,
    then continue, otherwise, return 0 bytes written.
  -------------------------------------------------------------------------*/
  if ( msg_ptr == NULL )
  {
    for (i=0; i< msg->msg_iovlen && nbytes < DSS_WRITE_MAX_BYTES; i++)
    {
      /*---------------------------------------------------------------------
        Return an error if the length is non-zero but the address is NULL.
      ---------------------------------------------------------------------*/
      if ((msg->msg_iov[i].ps_iov_len != 0 && msg->msg_iov[i].ps_iov_base == NULL))
      {
        *dss_errno = DS_EFAULT;
        return (DSS_ERROR);
      }
      nbytes += msg->msg_iov[i].ps_iov_len;
    }
  }
  else
  {
    if ( *msg_ptr )
    {
      nbytes = dsm_length_packet( *msg_ptr );
    }
    else
    {
      nbytes = 0;
    }
  }
  nbytes = MIN(nbytes, DSS_WRITE_MAX_BYTES);

  /*-------------------------------------------------------------------------
    Bind an address if ephemeral port is used or routing cache is null.
  -------------------------------------------------------------------------*/
  DSSOCKI_CACHE_GET_TX_FWK_INST(scb_ptr, routing_cache);
  DSSOCKI_CACHE_GET_LOCAL(scb_ptr, localname);
  if ((byte)PS_IPPROTO_UDP == scb_ptr->protocol && 0 == localname.port)
  {
    ps_cmd = dssocki_alloc_ps_cmd_buffer(APP_TO_PS_SOCKET_BIND);
    if(NULL == ps_cmd)
    {
      DS_ERROR_LOG_0("No memory for APP_TO_PS_SOCKET_BIND command");
      *dss_errno = DS_ENOMEM ;
      return DSS_ERROR;
    }

    /*-----------------------------------------------------------------------
      Ephemeral port.
    -----------------------------------------------------------------------*/
    local_sockaddr.ps_sin6_family = (uint16)DSS_AF_INET6;
    localname.port = local_sockaddr.ps_sin6_port
                   = dssocki_getephem_port(scb_ptr->protocol);
    local_sockaddr.ps_sin6_addr = localname.address;

    DSSOCKI_CACHE_SET_LOCAL(scb_ptr, localname);

    dssocki_send_ps_cmd_buffer(scb_ptr, NULL, &local_sockaddr, ps_cmd);
  }

  if( routing_cache == NULL || scb_ptr->flow_ptr == NULL ||
      (scb_ptr->protocol != (byte)PS_IPPROTO_TCP &&
       dssocki_pkt_info_differ(scb_ptr, &v6_sockaddr)))
  {

    memset(&pkt_info, 0, sizeof(ip_pkt_info_type));

    if(dssocki_generate_pkt_info(scb_ptr, &v6_sockaddr,
                                 &pkt_info, dss_errno) == DSS_ERROR)
    {
      return DSS_ERROR;
    }

    if (scb_ptr->acb_ptr == NULL)
    {
      /* 
        To have 1-1 mapping of ACB and SCB, it is mandatory to
        create socket using dss_socket API 
      */
 
      DS_ERROR_LOG_0("Socket is not created using dss_socket API");
      *dss_errno = DS_EINVAL;
      return DSS_ERROR;
    }

    
    if (ds_fwk_inst_is_valid_ex(scb_ptr->acb_ptr->fw_inst,
                                scb_ptr->family) == FALSE)
    {
      DS_ERROR_LOG_1("Fwk instance 0x%p is not valid",
                     scb_ptr->acb_ptr->fw_inst);
      *dss_errno = DS_EWOULDBLOCK;
      return DSS_ERROR;
    }
	
    flow_available = ds_fwk_get_data_flow_ext( scb_ptr->acb_ptr->fw_inst,
                                           scb_ptr->family,
                                           &pkt_info,
                                           &scb_ptr->flow_ptr);
	
    if(!flow_available)
    {
      DS_MSG0(MSG_LEGACY_ERROR,"Flow unavailable for first packet. "
      	                        "Cant route packet");
      *dss_errno = DS_EWOULDBLOCK;
      return DSS_ERROR;
    }
    scb_ptr->dirty_cache_flag = TRUE;
  }

  if (!ds_flow_is_valid(scb_ptr->flow_ptr))
  {
    DS_MSG0(MSG_LEGACY_ERROR,"Flow pointer is NULL. Cant route packet");
    *dss_errno = DS_EWOULDBLOCK;
    return DSS_ERROR;
  }
  else
  {
    flow_ptr = scb_ptr->flow_ptr;

    if (!ds_fwk_is_flow_enabled(scb_ptr->flow_ptr))
    {
      DS_MSG1(MSG_LEGACY_ERROR,"Flow is Disable for flow ptr %p"
                               " Cant route packet",scb_ptr->flow_ptr);
      *dss_errno = DS_EWOULDBLOCK;
      return DSS_ERROR;
    }

    if (ds_bearer_mgr_is_dormant_reorig_required(flow_ptr->bearer_ptr))
    {
       DS_MSG1(MSG_LEGACY_ERROR,"Bearer re-orig req for flow ptr %p"
                               " Cant route packet",scb_ptr->flow_ptr);
       ds_bearer_mgr_reorig_dorm_call_cb(NULL, flow_ptr->bearer_ptr);
      *dss_errno = DS_EWOULDBLOCK;
      return DSS_ERROR;
    }
  }

  /*-------------------------------------------------------------------------
    If we need not reoriginate then register the sdb ack status handler
    function with the ps_iface so
  -------------------------------------------------------------------------*/
  DSSOCKI_CACHE_GET_TX_FWK_INST(scb_ptr, routing_cache);

  /*-------------------------------------------------------------------------
    Make sure socket is writeable if there are any bytes to write, before
    proceeding further. A (TCP) socket is writeable when its sendq is less
    than the offered RX window. Non-TCP sockets are always writeable.
  -------------------------------------------------------------------------*/
  if ( nbytes > 0 &&
       scb_ptr->fcn_ptr_table->is_writeable != NULL &&
       ((sndq_limit = scb_ptr->fcn_ptr_table->is_writeable(scb_ptr)) < nbytes))
  {
    DS_MSG3( MSG_LEGACY_ERROR, "Sockfd %d NOT Writeable - blocked, SendQ limit"
             " = %d, packet size = %d", sockfd, sndq_limit, nbytes );
    scb_ptr->is_write_blocked = TRUE;
    *dss_errno= DS_EWOULDBLOCK;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Non-zero amount of data specified.  Copy array into data services
    memory pool
  -------------------------------------------------------------------------*/
  if ( msg_ptr && *msg_ptr )
  {
    count = (int16)nbytes;
    item_ptr = *msg_ptr;
    *msg_ptr = NULL;
  }
  else
  {
    count = dssocki_copy_buf_to_dsm(msg->msg_iov,
                                    (sint15)msg->msg_iovlen,
                                    (sint15)nbytes,
                                    &item_ptr);
  }

  /*-------------------------------------------------------------------------
    For Stream sockets, ensure that bytes were actually copied and that
    item_ptr is non-NULL,before dispatching it to PS.  Return DS_EWOULDBLOCK
    since the only way for this function to fail is by running out of buffer
    space.
  -------------------------------------------------------------------------*/
  if (item_ptr == NULL)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"NO DSM available for packet transfer, EWOULDBLOCK.");
     *dss_errno = DS_EWOULDBLOCK;
     return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    For datagram sockets, ensure that bytes were actually copied and that
    item_ptr is non-NULL,before dispatching it to PS.  Release item_ptr and
    return DS_EWOULDBLOCK since the only way for this function to fail is by
    running out of buffer space.
  -------------------------------------------------------------------------*/
  if ( scb_ptr->type == DSS_SOCK_DGRAM && (count != (int16)nbytes) )
  {
    DS_MSG0(MSG_LEGACY_ERROR,"NO DSM available for UDP packet transfer, EWOULDBLOCK.");
    dsm_free_packet( &item_ptr);
    DSSOCKI_CACHE_SET_TX_FWK_INST(scb_ptr, NULL);
    *dss_errno = DS_EWOULDBLOCK;
    return (DSS_ERROR);
  }

  /*-----------------------------------------------------------------------
    Update socket's cache with new values before generating metainfo
  -----------------------------------------------------------------------*/

  DSSOCKI_CACHE_SET_FLOW_LABEL( scb_ptr, v6_sockaddr.ps_sin6_flowinfo);
  DSSOCKI_CACHE_SET_TX_FLAGS( scb_ptr, flags);

  if(scb_ptr->acb_ptr != NULL)
  {
    DSSOCKI_CACHE_SET_TX_FWK_INST(scb_ptr,scb_ptr->acb_ptr->fw_inst);
  }

  /*
    DSS should generate the META info for all data packets.
  */
  if ((tx_meta_info_ptr = dssocki_generate_meta_info(scb_ptr,
                                                     flow_ptr,
                                                     (unsigned int) flags,
                                                     &ipcb_meta_info_valid,
                                                     dss_errno )) == NULL )
  {
    DS_ERROR_LOG_1("MetaInfo gen. failed (%d)", *dss_errno);

    if ( msg_ptr )
    {
      *msg_ptr = item_ptr;
    }
    else
    {
      dsm_free_packet( &item_ptr);
    }

    *dss_errno = DS_EWOULDBLOCK;
    return DSS_ERROR;
  }

  item_ptr->app_field = (uint32) tx_meta_info_ptr;

  /*-------------------------------------------------------------------------
    Call function to put the item on the queue to PS.
  -------------------------------------------------------------------------*/
  DSM_TOUCH_ITEM(item_ptr);

  /*-------------------------------------------------------------------------
    FEATURE_DSM_MEM_CHK
  -------------------------------------------------------------------------*/
  scb_ptr->sndcnt += count;
  result = dssocki_send_to_ps( scb_ptr,
                               item_ptr,
                               ta_ptr,
                               APP_TO_PS_SOCKET_PAYLOAD);
  if( result == DSS_ERROR)
  {
    /*
      If the META info pointer is not set in the IPCB corresponding to the
      socket, we need to free the META info pointer. The META info pointer
      when the socket is closed only if the IPCB contains a valid meta info 
      pointer.
    */
    if (!ipcb_meta_info_valid)
    {
      if(tx_meta_info_ptr != NULL)
      {
        PS_TX_META_INFO_FREE(&tx_meta_info_ptr);
      }
    }

    if ( msg_ptr )
    {
      *msg_ptr = item_ptr;
    }
    else
    {
      dsm_free_packet( &item_ptr);
    }

    *dss_errno = DS_EWOULDBLOCK;
    return DSS_ERROR;
  }

  scb_ptr->dirty_cache_flag = FALSE;
  SOCK_INC_STATS(bytes_tx, count);
  return (count);

} /* dssocki_sendmsg() */

/*===========================================================================
FUNCTION DSSOCKI_RECVMSG()
DESCRIPTION
  This function is a common read function for all the socket input
  functions. The message header contains an array of scattered buffers, a
  socket descriptor and an address field for filling the source address
  of the received packet.The function reads data into the scattered buffers
  over the transport specified by the socket descriptor

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes to be written, which could be less than the number
      of bytes specified.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.  Caller must free dsm in msg_ptr in this case.

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor is specfied
  DS_EAFNOSUPPORT     address family not supported
  DS_EWOULDBLOCK      operation would block
  DS_EFAULT           bad memory address
  DS_EOPNOTSUPP       option not supported
  DS_EINVAL           can't recv from a listen socket.

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dssocki_recvmsg
(
  sint15                  sockfd,   /* socket descriptor                   */
  struct dss_msghdr     * msg,      /* Message header for filling in data  */
  dsm_item_type        ** msg_ptr,  /* Optional ptr to recv data in DSMs   */
  int                     flags,    /* flags from dss_recvfrom             */
  sint15                * dss_errno /* error condition value               */
)
{
  struct scb_s      * scb_ptr;       /* Ptr to socket control block        */
  sint15              num_read;      /* number of bytes read               */
  sint15              result = 0;    /* error checking return value        */
  int                 i;             /* local loop index                   */
  struct socket       servname;      /* peer's address                     */
  ds_fwk_s_type       *routing_cache; /* routing cache                      */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Assert that a valid ptr to a dss_errno variable was specified - Prevents
    dereferencing of NULL ptrs.
  -------------------------------------------------------------------------*/
  if (dss_errno == NULL)
  {
    DS_ERROR_LOG_0("DSS_RECVMSG : NULL pointer passed for dss_errno");
    return(DSS_ERROR);
  }
  *dss_errno = DSS_SUCCESS;
   DS_MSG2(MSG_LEGACY_HIGH,"dss_errno 0x%p *dss_errno %d",dss_errno,*dss_errno);
  /*-------------------------------------------------------------------------
    Convert socket descriptor to SCB
  -------------------------------------------------------------------------*/
  if ( (scb_ptr=dssocki_sockfdtoscb(sockfd)) == NULL)
  {
    *dss_errno = DS_EBADF;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Flags are not supported
  -------------------------------------------------------------------------*/

  if(flags != 0 && (flags & ~DSS_MSG_ERRQUEUE) != 0)
  {
    *dss_errno = DS_EOPNOTSUPP;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Check for any error conditions - if it will fail, return immediately
  -------------------------------------------------------------------------*/
  result = dssocki_get_err_val(scb_ptr, DSSOCKI_INPUT_ROUTINE, dss_errno);
  if (result == DSS_ERROR)
  {
    if (*dss_errno == DS_EEOF)
    {
      return (DSS_SUCCESS);
    }
    else
    {
      return (result);
    }
  }

  if ( msg_ptr == NULL )
  {
    /*-----------------------------------------------------------------------
      If we reach this point then iovcount is non-zero. Hence, it is an error
      if iov is NULL.
    -----------------------------------------------------------------------*/
    if( (msg == NULL) || ((msg->msg_iov == NULL) && (msg->msg_iovlen != 0)) )
    {
      *dss_errno = DS_EFAULT;
      return DSS_ERROR;
    }


    for(i=0;i< msg->msg_iovlen ; i++)
    {
      /*---------------------------------------------------------------------
        Verify that valid buffer and length values were passed
      ---------------------------------------------------------------------*/
      if((msg->msg_iov[i].ps_iov_base == NULL && msg->msg_iov[i].ps_iov_len != 0))
      {
        *dss_errno=DS_EFAULT;
        return DSS_ERROR;
      }
    }

    /*-----------------------------------------------------------------------
      Call protocol specific function to read bytes into an array.
    -----------------------------------------------------------------------*/
    num_read = scb_ptr->fcn_ptr_table->read(scb_ptr,
                                            msg->msg_iov,
                                            msg->msg_iovlen,
                                            msg->msg_name,
                                            &(msg->msg_namelen),
                                            flags,
                                            dss_errno);
  }
  else
  {
    /* Keep receive data in a DSM chain */
    num_read = scb_ptr->fcn_ptr_table->read_dsm_chain( scb_ptr,
                                                       msg_ptr,
                                                       msg->msg_name,
                                                       &(msg->msg_namelen),
                                                       dss_errno );
  }

  DSSOCKI_CACHE_GET_REMOTE( scb_ptr, servname );
  DSSOCKI_CACHE_GET_TX_FWK_INST(scb_ptr, routing_cache);
//  #endif
  
  if(scb_ptr->error_available == TRUE &&
     (IS_SOCKOPT_SET(scb_ptr->cached_info.ip.options_mask, (uint64)DSS_IP_RECVERR) ||
      IS_SOCKOPT_SET(scb_ptr->cached_info.ip.options_mask, (uint64)DSS_IPV6_RECVERR))
    )
  {
    /*-----------------------------------------------------------------------
      Set msg_flags to DSS_MSG_ERRQUEUE
    -----------------------------------------------------------------------*/
    msg->msg_flags |= DSS_MSG_ERRQUEUE;
  }

  /*-------------------------------------------------------------------------
    Read was not successful...
  -------------------------------------------------------------------------*/

  if ((num_read == DSS_ERROR) &&
      (*dss_errno == DS_EWOULDBLOCK) &&
      (!PS_IN6_ARE_ADDR_EQUAL(&servname.address, &ps_in6addr_any)) &&
      (!ds_fwk_inst_is_valid_ex(routing_cache,scb_ptr->family)))
  {
    /*-----------------------------------------------------------------------
      ENETDOWN because no network...set dss_errno to reflect this.
    -----------------------------------------------------------------------*/
    *dss_errno = DS_ENETDOWN;
  }
  else if(num_read == DSS_ERROR &&
          *dss_errno == DS_EWOULDBLOCK &&
          IS_SOCKOPT_SET(scb_ptr->options_mask, (uint64)DSS_SO_ERROR_ENABLE))
  {
    DS_ENTER_CRIT_SECTION(&global_ps_crit_section);
    if(scb_ptr->so_error)
    {
      *dss_errno = scb_ptr->so_error;
      scb_ptr->so_error = 0;
      DSSOCKI_RESET_ERROR_AVAILABLE(scb_ptr);
      DS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      return(num_read);
    }
    DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  }

  if( (msg->msg_control != NULL) && (msg->msg_controllen > 0) )
  {
    if( -1 == dssocki_process_ancillary_data(msg, scb_ptr, flags, dss_errno) )
    {
      return(DSS_ERROR);
    }
  }

  if (num_read != DSS_ERROR)
  {
    SOCK_INC_STATS(bytes_rx, num_read);
  }
  return (num_read);

} /* dssocki_recvmsg() */


/*===========================================================================
FUNCTION DSSOCKI_SOCK_CB()
DESCRIPTION
  This function is a common socket call back function for application
  using old opennetlib function.

DEPENDENCIES
  NOTE: This function must be called in a TASKLOCK().

RETURN VALUE
  None

SIDE EFFECTS
  Calls the user specified socket call back which is passed in user_data_ptr.
===========================================================================*/
void dssocki_sock_cb
(
  dss_sock_callback_info_s *sock_callback_info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*lint -save -e611 we need to cast one function type to another, for that we first cast net/socket_callback_fcn to "void*" and then cast it back to another
    function type, using helper functions dssocki_net/sock_cb */
  if(sock_callback_info->sock_cb_user_data != NULL)
  {
    ((void (*)(void *))sock_callback_info->sock_cb_user_data)((void *)&(sock_callback_info->dss_nethandle));
  }
  /*lint -restore Restore lint error 611*/
} /* dssocki_sock_cb() */

/*===========================================================================
FUNCTION DSSOCKI_PROCESS_LINGERING_CLOSE()

DESCRIPTION
  This function is called to when a lingering socket is closed. It brings
  down the interface if no other application is using it and frees up the
  scb and the associated acb (a lingering socket's acb is allocated
  internally when the socket enters the lingering state)

DEPENDENCIES
  Need to be called under TASKLOCK()

RETURN VALUE
  None

SIDE EFFECTS
  Frees up the acb and the scb, passed as a parameter
===========================================================================*/
void dssocki_process_lingering_close
(
  scb_type** scb_ptr_ptr
)
{
  scb_type  * scb_ptr;
  acb_type  * tmp_acb_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( scb_ptr_ptr == NULL || *scb_ptr_ptr == NULL )
  {
    DS_ERROR_LOG_0("NULL scb passed");
    DS_ASSERT( 0 );
    return;
  }
  scb_ptr = *scb_ptr_ptr;


  DS_MSG1(MSG_LEGACY_MED, "process_lingering_close for sockfd=%d",
          scb_ptr->sockfd);

  tmp_acb_ptr = scb_ptr->linger_acb_ptr;
  if (scb_ptr->acb_ptr == scb_ptr->linger_acb_ptr)
  {
    dssocki_freescb(scb_ptr);
    *scb_ptr_ptr = NULL;
  }

  /*-------------------------------------------------------------------------
    Note that linger_acb_ptr could be NULL if SCB is created using socket2()
  -------------------------------------------------------------------------*/
  if (tmp_acb_ptr != NULL)
  {
    dssocki_freeacb(tmp_acb_ptr);
  }

  return;

} /* dssocki_process_lingering_close() */



/*===========================================================================
FUNCTION DSSOCKI_INFO_CHANGE_HANDLER()

DESCRIPTION
  Common info_change_upcall function for the socket's layer. This function is
  called by the PS, when information cached by the socket's layer
  has changed. (Executed in PS context)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Updates the cached values.
===========================================================================*/
void dssocki_info_change_handler
(
  void * user_data,
  ps_event_info_type* ps_event_info
)
{
  scb_type * scb_ptr = (scb_type *) user_data;
  scb_type * head_scb_ptr = NULL;
  boolean    socket_event = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(ps_event_info == NULL)
  {
    DS_ERROR_LOG_0("dssocki_info_change_handler called with no info");
    //ASSERT(0);
    return;
  }

  if (scb_ptr == NULL)
  {
    DS_MSG1(MSG_LEGACY_HIGH,"Socket is already closed: Discarding the event %d",
             ps_event_info->id);
    return;
  }

  /*-------------------------------------------------------------------------
    Access to SCB MUST BE tasklocked.
  -------------------------------------------------------------------------*/
  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  /*-------------------------------------------------------------------------
    Check that SCB is still valid after the critical section has been
    acquired. Also protect against the case where SCB is freed but is
    reassigned to another socket. If this is not checked, events reported by
    PCB will be processed on wrong SCB
  -------------------------------------------------------------------------*/
  if (scb_ptr->sockfd != ps_event_info->sockfd)
  {
    DS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    DS_MSG2( MSG_LEGACY_HIGH, "Socket %d is closed: Discarding the event %d",
             ps_event_info->sockfd, ps_event_info->id);
    return;
  }

  /*-------------------------------------------------------------------------
    F3 message is not printed here as printing a msg in data path causes lot
    of CPU overhead and PS_DATA_AVAILABLE_EVENT is posted in data path
  -------------------------------------------------------------------------*/
  switch (ps_event_info->id)
  {
    case PS_IP_EVENT:
      DS_MSG1( MSG_LEGACY_MED, "PS_IP_EVENT on sockfd=%d", scb_ptr->sockfd);

      if (scb_ptr->cached_info.ip.routing_cache == NULL)
      {
        scb_ptr->cached_info.ip.routing_cache =
          ps_event_info->data.ip.routing_cache;
      }

      scb_ptr->cached_info.ip.ttl          = ps_event_info->data.ip.ttl;
//      scb_ptr->cached_info.ip.route_scope  = ps_event_info->data.ip.route_scope;
      scb_ptr->cached_info.ip.options_mask = ps_event_info->data.ip.options_mask;
      break;

    case PS_UDP_EVENT:
      DS_MSG1( MSG_LEGACY_MED, "PS_UDP_EVENT on sockfd=%d", scb_ptr->sockfd);

      ASSERT(scb_ptr->protocol == (byte)PS_IPPROTO_UDP);
      scb_ptr->cached_info.pcb.udp = ps_event_info->data.udp;
      break;

    case PS_TCP_EVENT:
      DS_MSG1( MSG_LEGACY_MED, "PS_TCP_EVENT on sockfd=%d", scb_ptr->sockfd);

      DS_ASSERT(scb_ptr->protocol == (byte)PS_IPPROTO_TCP);
      scb_ptr->cached_info.pcb.tcp = ps_event_info->data.tcp;
      break;

    case PS_SOCKET_EVENT:
      DS_MSG1( MSG_LEGACY_MED, "PS_SOCKET_EVENT on sockfd=%d", scb_ptr->sockfd);

      socket_event = TRUE;
      break;

    case PS_DATA_AVAILABLE_EVENT:
      DS_MSG1( MSG_LEGACY_MED, "PS_DATA_AVAILABLE_EVENT on sockfd=%d", scb_ptr->sockfd);

      scb_ptr->data_available = ps_event_info->data.data_available;
      if(ps_event_info->data.data_available == TRUE)
      {
        socket_event = TRUE;
      }
      break;

    case PS_TX_UPCALL_EVENT:
      DS_MSG1( MSG_LEGACY_MED, "PS_TX_UPCALL_EVENT on sockfd=%d", scb_ptr->sockfd);

      /*---------------------------------------------------------------------
        Update the cached congestion window value in scb_ptr
      ---------------------------------------------------------------------*/
      scb_ptr->cached_info.pcb.tcp.cwind = ps_event_info->data.t_upcall.cwind;
      if(ps_event_info->data.t_upcall.sndcnt > 0)
      {
        scb_ptr->sndcnt -= ps_event_info->data.t_upcall.sndcnt;
        scb_ptr->is_write_blocked = FALSE;
      }
      break;

    case PS_SOCK_STATE_CHANGE_EVENT:
      DS_MSG1( MSG_LEGACY_MED, "PS_SOCK_STATE_CHANGE_EVENT on sockfd=%d",
              scb_ptr->sockfd);

      /*---------------------------------------------------------------------
        TCP_CLOSED or TCP_TIME_WAIT event. Here socket is in the process of
        close.
      ---------------------------------------------------------------------*/
      if(ps_event_info->data.state.new_state == DSSOCKI_WAIT_FOR_CLOSE ||
                       ps_event_info->data.state.new_state == DSSOCKI_CLOSED)
      {
        /*-------------------------------------------------------------------
          TCP is closed now set the appropriate reson for close
        -------------------------------------------------------------------*/
        dssocki_set_closed_state_reason(scb_ptr,
                                        ps_event_info->data.state.reason);

        /*-------------------------------------------------------------------
          If socket is CLOSING, set state to WAIT_FOR_CLOSE.
          Otherwise set it to CLOSED.
        -------------------------------------------------------------------*/
        if(scb_ptr->socket_state == DSSOCKI_CLOSING)
        {
          dssocki_set_sock_state(scb_ptr, DSSOCKI_WAIT_FOR_CLOSE);
        }
        else if((scb_ptr->socket_state == DSSOCKI_OPEN) ||
                (scb_ptr->socket_state == DSSOCKI_OPENING))
        {
          dssocki_set_sock_state(scb_ptr, DSSOCKI_CLOSED);
        }
        else
        {
          DS_MSG1( MSG_LEGACY_HIGH, " TCP ERROR: TCP closed from socket state %d",
                     scb_ptr->socket_state);
        }

        if(scb_ptr)
        {
          scb_ptr->protocol_ctl_blk.tcb = NULL;
        }
      }
      else
      {
        /*-------------------------------------------------------------------
          Some other state change.
        -------------------------------------------------------------------*/
        if(scb_ptr->socket_state == DSSOCKI_INIT &&
           ps_event_info->data.state.new_state == DSSOCKI_OPEN)
        {
          /*-----------------------------------------------------------------
            If the socket is changing state from LISTEN to OPEN this means
            that we are having a scb with newly established connection.
            In this case we need to set the data_available flag of the
            so_head of this scb_ptr to TRUE and call
            dssocki_socket_event_occurred on so_head
          -----------------------------------------------------------------*/
          ASSERT(scb_ptr->so_head != NULL);
          head_scb_ptr = scb_ptr->so_head;
        }

        dssocki_set_sock_state(scb_ptr,ps_event_info->data.state.new_state);

        if(head_scb_ptr != NULL)
        {
          head_scb_ptr->data_available = TRUE;
          socket_event = TRUE;
          /*--------------------------------------------------------------
            The function dssocki_socket_event_occurred at the end is
            called with scb_ptr. So point scb_ptr to head_scb_ptr.
           -------------------------------------------------------------*/
          scb_ptr = head_scb_ptr;
        }
      }

      break;

    case PS_ICMP_ERROR_EVENT:
      DS_MSG1( MSG_LEGACY_MED, "PS_ICMP_ERROR_EVENT on sockfd=%d", scb_ptr->sockfd);

      if(IS_SOCKOPT_SET(scb_ptr->options_mask, (uint64)DSS_SO_ERROR_ENABLE) ||
         IS_SOCKOPT_SET(scb_ptr->cached_info.ip.options_mask,
                        (uint64)DSS_IP_RECVERR) ||
         IS_SOCKOPT_SET(scb_ptr->cached_info.ip.options_mask,
                        (uint64)DSS_IPV6_RECVERR))
      {
        if(IS_SOCKOPT_SET(scb_ptr->options_mask, (uint64)DSS_SO_ERROR_ENABLE))
        {
          /*-----------------------------------------------------------------
            If SO_ERROR_ENABLE is not set write event should not be asserted
          -----------------------------------------------------------------*/
          scb_ptr->so_error = ps_event_info->data.icmp_error;
        }
        scb_ptr->error_available = TRUE;
        socket_event = TRUE;
      }
      break;

    default:
      DS_ERROR_LOG_0( "Unknown ps_event_info.ptcl");
      break;
  }

  if(socket_event == TRUE)
  {
    dssocki_socket_event_occurred(scb_ptr, NULL);
  }
  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  return;
}

/*===========================================================================
FUNCTION DSSOCKI_CACHE_GETOPT()

DESCRIPTION
  This function returns the requested option value from the cache.

DEPENDENCIES
  None

RETURN VALUE
  Returns the option value in the pointer

SIDE EFFECTS
  None
===========================================================================*/
sint15 dssocki_cache_getopt
(
  scb_type * scb_ptr,
  int        level,
  int        optname,
  void     * optval,
  uint32   * optlen,
  sint15   * dss_errno
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sint15 rc = DSS_SUCCESS;

  /*
    enter critical section before performing the NULL pointer check to avoid
    changes of this pointer when it is finally accessed in this function.
  */
  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if( level == (int)DSS_IPPROTO_TCP && scb_ptr->protocol_ctl_blk.tcb == NULL)
  {
    *dss_errno = DS_EOPNOTSUPP;
    rc = DSS_ERROR;
  }
  else
  switch(optname)
  {

    case DSS_IPV6_TCLASS:
      *((int *) optval) =  scb_ptr->cached_info.ip.tclass;
      break;


    case DSS_IP_TOS:
      *((int *) optval) =  scb_ptr->cached_info.ip.tos;
      break;

    case DSS_IP_TTL:
      *((int *)optval) =  scb_ptr->cached_info.ip.ttl;
      break;

    case DSS_IP_RECVIF:
    case DSS_IP_RECVERR:
    case DSS_IPV6_RECVERR:
      if ((byte)PS_IPPROTO_UDP != scb_ptr->protocol)
      {
        DS_ERROR_LOG_2("Option, %d, is not supported for a %d socket",
                  optname,
                  scb_ptr->protocol);
        *dss_errno = DS_EINVAL;
        rc = DSS_ERROR;
        break;
      }

      *((int *)optval) =  IS_SOCKOPT_SET(scb_ptr->cached_info.ip.options_mask,
                                         optname);
      break;

    case DSS_TCP_MAXSEG:
      *((int *)optval) = scb_ptr->cached_info.pcb.tcp.mss;
      break;

    case DSS_TCP_NODELAY:
      (*(int *) optval) = scb_ptr->cached_info.pcb.tcp.options.tcp_nodelay;
      break;

    case DSS_TCP_DELAYED_ACK:
      (*(int*)optval) = scb_ptr->cached_info.pcb.tcp.options.tcp_delayed_ack;
      break;

    case DSS_TCP_SACK:
      (*(int*)optval)= scb_ptr->cached_info.pcb.tcp.options.tcp_sack_permitted;
      break;

    case DSS_TCP_TIME_STAMP:
      (*(int*)optval)= scb_ptr->cached_info.pcb.tcp.options.tcp_ts_permitted;
      break;

    case DSS_TCP_MAX_BACKOFF_TIME:
      (*(int*)optval) = scb_ptr->protocol_ctl_blk.tcb->max_backoff_time;
      break;

    case DSS_TCP_EIFEL:
      *dss_errno = DS_EOPNOTSUPP;
      rc = DSS_ERROR;
      break;

    default:
      *dss_errno = DS_EOPNOTSUPP;
      rc = DSS_ERROR;
      break;
  }


 DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  return rc;

}

/*===========================================================================
FUNCTION DSSOCKI_CACHE_SETOPT()

DESCRIPTION
  This function updates the option value in the cache and sends a command
  to PS to change the option value.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Option value is updated in the cache.
===========================================================================*/
sint15 dssocki_cache_setopt
(
  scb_type * scb_ptr,
  int        level,
  int        optname,
  void     * optval,
  uint32   * optlen,
  sint15   * dss_errno
)
{
 dsm_item_type* item_ptr;                     /* Ptr to payload to be sent */
 dssocki_app_to_ps_cmd_type  * ps_cmd;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( level == (int)DSS_IPPROTO_TCP && scb_ptr->protocol_ctl_blk.tcb == NULL)
  {
    *dss_errno = DS_EOPNOTSUPP;
    return DSS_ERROR;
  }

  if(NULL == dss_errno)
  {
    DS_ERROR_LOG_0("Invalid parameters to dssocki_cache_setopt");
    return DSS_ERROR;
  }

  ps_cmd = dssocki_alloc_ps_cmd_buffer(APP_TO_PS_SOCKET_SETOPT);
  if(NULL == ps_cmd)
  {
    DS_ERROR_LOG_0( "Command buffers could not be allocated");
    *dss_errno = DS_ENOMEM ;
    return DSS_ERROR;
  }

  switch (optname)
  {

    case DSS_IPV6_TCLASS:
      DS_ENTER_CRIT_SECTION(&global_ps_crit_section);
      scb_ptr->cached_info.ip.tclass = (uint8) *((int *) optval);
      DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      break;


    case DSS_IP_TOS:
      DS_ENTER_CRIT_SECTION(&global_ps_crit_section);
      scb_ptr->cached_info.ip.tos = (uint8) *((int *) optval);
      DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      break;

    case DSS_IP_TTL:
      DS_ENTER_CRIT_SECTION(&global_ps_crit_section);
      scb_ptr->cached_info.ip.ttl = (uint8)*((int *)optval);
      DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      break;

    case DSS_IP_RECVIF:
    case DSS_IP_RECVERR:
    case DSS_IPV6_RECVERR:
      /*---------------------------------------------------------------------
        Support IP_RECVIF and IP_RECVERR only through UDP sockets for now.
      ---------------------------------------------------------------------*/
      if ((byte)PS_IPPROTO_UDP != scb_ptr->protocol)
      {
        DS_ERROR_LOG_2("Option, %d, can't be set for a %d socket",
                  optname,
                  scb_ptr->protocol);
        *dss_errno = DS_EINVAL;
        PS_MEM_FREE(ps_cmd);
        return DSS_ERROR;
      }

      DS_ASSERT(NULL != scb_ptr->protocol_ctl_blk.ucb);

      if ( *(int *)optval )
      {
        SOCKOPT_SET(scb_ptr->cached_info.ip.options_mask, optname);
      }
      else
      {
        SOCKOPT_RESET(scb_ptr->cached_info.ip.options_mask, optname);
      }

      break;

    case DSS_SO_SYS_SOCK:
      if ((*((uint32*)optval)) == 1)
      {
        SOCKOPT_SET(scb_ptr->options_mask, optname);
      }
      else if ((*((uint32*)optval)) == 0)
      {
        SOCKOPT_RESET(scb_ptr->options_mask, optname);
      }
      else
      {
        DS_MSG1(MSG_LEGACY_ERROR,"dssocki_cache_setopt :DSS_SO_SYS_SOCK: Invalid optval %u",*(( uint32*)optval));
        *dss_errno = DS_EINVAL;
        PS_MEM_FREE(ps_cmd);
        return DSS_ERROR;
      }
      /*-------------------------------------------------------------------
        Now since we need to propage this info upto the ipcb we change the
        level of the socket option to IP_LEVEL so that the level checks
        in lower layers pass.
      -------------------------------------------------------------------*/
      level = (int)DSS_IPPROTO_IP;
      break;

    case DSS_SO_RCVBUF:
      scb_ptr->rcvbuf = *((unsigned int *) optval);
      break;

    case DSS_SO_KEEPALIVE:
      if(scb_ptr->protocol != (byte)PS_IPPROTO_TCP ||
         scb_ptr->protocol_ctl_blk.tcb == NULL)
      {
        *dss_errno = DS_EINVAL;
        PS_MEM_FREE(ps_cmd);
        return DSS_ERROR;
      }
      if( *((int *) optval))
      {
        SOCKOPT_SET(scb_ptr->options_mask, optname);
      }
      else
      {
        SOCKOPT_RESET(scb_ptr->options_mask, optname);
      }
      break;

    case DSS_TCP_MAXSEG:
      DS_ENTER_CRIT_SECTION(&global_ps_crit_section);
      if( scb_ptr->socket_state == DSSOCKI_INIT)
      {
        scb_ptr->cached_info.pcb.tcp.mss = (uint16)*((int *)optval);
      }
      else
      {
        if(scb_ptr->cached_info.pcb.tcp.mss > (uint16)*((int *)optval))
        {
          scb_ptr->cached_info.pcb.tcp.mss = (uint16)*((int *)optval);
        }
        else
        {
          *dss_errno = DS_EINVAL;
          PS_MEM_FREE(ps_cmd);
          DS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
          return DSS_ERROR;
        }
      }
      DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      break;

    case DSS_TCP_NODELAY:
      DS_ENTER_CRIT_SECTION(&global_ps_crit_section);
      scb_ptr->cached_info.pcb.tcp.options.tcp_nodelay =
        (*(int *) optval) ? TRUE : FALSE;
      DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      break;

    case DSS_TCP_DELAYED_ACK:
      DS_ENTER_CRIT_SECTION(&global_ps_crit_section);
      scb_ptr->cached_info.pcb.tcp.options.tcp_delayed_ack =
        (*(int *) optval) ? TRUE : FALSE;
      DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      break;

    case DSS_TCP_SACK:
      DS_ENTER_CRIT_SECTION(&global_ps_crit_section);
      scb_ptr->cached_info.pcb.tcp.options.tcp_sack_permitted =
        (*(int *) optval) ? TRUE : FALSE;
      DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      break;

    case DSS_TCP_TIME_STAMP:
      DS_ENTER_CRIT_SECTION(&global_ps_crit_section);
      scb_ptr->cached_info.pcb.tcp.options.tcp_ts_permitted =
        (*(int *) optval) ? TRUE : FALSE;
      DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      break;

    case DSS_TCP_EIFEL:
      PS_MEM_FREE(ps_cmd);
      *dss_errno = DS_EOPNOTSUPP;
      return DSS_ERROR;

    case DSS_TCP_MAX_BACKOFF_TIME:
      DS_ENTER_CRIT_SECTION(&global_ps_crit_section);
      scb_ptr->max_backoff_time = (uint32)*((int *)optval);
      DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      break;

    case DSS_ICMP_CODE:
      DS_ENTER_CRIT_SECTION(&global_ps_crit_section);
      scb_ptr->icmp_code = (uint32)*((int *)optval);
      DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      break;

    case DSS_ICMP_TYPE:
      DS_ENTER_CRIT_SECTION(&global_ps_crit_section);
      scb_ptr->icmp_type = (uint32)*((int *)optval);
      DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      break;

    case DSS_ICMP_ECHO_ID:
    case DSS_ICMP_ECHO_SEQ_NUM:
      /* Nothing to save in cache */
      break;

    default:
      *dss_errno = DS_EOPNOTSUPP;
      PS_MEM_FREE(ps_cmd);
      return DSS_ERROR;
  }

  item_ptr = dsm_new_buffer( DSM_DS_SMALL_ITEM_POOL );
  if(item_ptr == NULL)
  {
    DS_ERROR_LOG_0("Running out of items");
    *dss_errno = DS_ENOMEM;
    PS_MEM_FREE(ps_cmd);
    return DSS_ERROR;
  }

  /*-----------------------------------------------------------------------
     pushdown the option data
  -----------------------------------------------------------------------*/
  if (dsm_pushdown( &item_ptr,
                    optval,
                    (uint16)*optlen,
                    DSM_DS_SMALL_ITEM_POOL) < *optlen)
  {
    DS_ERROR_LOG_0("Running out of items");
    dsm_free_packet(&item_ptr);
    *dss_errno = DS_ENOMEM;
    PS_MEM_FREE(ps_cmd);
    return DSS_ERROR;
  }

  /*-----------------------------------------------------------------------
     pushdown the option length
  -----------------------------------------------------------------------*/
  if (dsm_pushdown( &item_ptr,
                    optlen,
                    sizeof( int),
                    DSM_DS_SMALL_ITEM_POOL) < sizeof ( int))
  {
    DS_ERROR_LOG_0("Running out of items");
    dsm_free_packet(&item_ptr);
    *dss_errno = DS_ENOMEM;
    PS_MEM_FREE(ps_cmd);
    return DSS_ERROR;
  }

  /*-----------------------------------------------------------------------
     pushdown the option level
  -----------------------------------------------------------------------*/
  if (dsm_pushdown( &item_ptr,
                    &level,
                    sizeof( int),
                    DSM_DS_SMALL_ITEM_POOL) < sizeof ( int))
  {
    DS_ERROR_LOG_0("Running out of items");
    dsm_free_packet(&item_ptr);
    *dss_errno = DS_ENOMEM;
    PS_MEM_FREE(ps_cmd);
    return DSS_ERROR;
  }

  /*-----------------------------------------------------------------------
     pushdown the option name
  -----------------------------------------------------------------------*/
  if (dsm_pushdown( &item_ptr,
                    &optname,
                    sizeof( int),
                    DSM_DS_SMALL_ITEM_POOL) < sizeof ( int))
  {
    DS_ERROR_LOG_0("Running out of items");
    dsm_free_packet(&item_ptr);
    *dss_errno = DS_ENOMEM;
    PS_MEM_FREE(ps_cmd);
    return DSS_ERROR;
  }

  dssocki_send_ps_cmd_buffer(scb_ptr, item_ptr, NULL, ps_cmd);
  return DSS_SUCCESS;
}

/*===========================================================================
FUNCTION DSSOCKI_GET_SOCKADDR_IN6()

DESCRIPTION
  This function check for validity of sockaddr and if it is of type
  ps_sockaddr_in or ps_sockaddr_in6 it converts them into ps_sockaddr_in6.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Initializes the ps_sockaddr_in6 from sockaddr
===========================================================================*/
sint15 dssocki_get_sockaddr_in6
(
  struct ps_sockaddr_in6 * v6addr,                            /* V6 sockaddr */
  struct ps_sockaddr     * sockaddr,                     /* Generic sockaddr */
  uint16                addrlen,                    /* length of sockaddr */
  sint15              * dss_errno                                /* errno */
)
{
  struct ps_sockaddr_in * tmp_addr;             /* tmp pointer to ps_sockaddr_in */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(v6addr, 0 , sizeof(struct ps_sockaddr_in6));
  v6addr->ps_sin6_family = (uint16)DSS_AF_INET6;

  if (sockaddr == NULL)
  {
    return DSS_SUCCESS;
  }

  switch(sockaddr->ps_sa_family)
  {
    case DSS_AF_INET:
      if(addrlen != sizeof(struct ps_sockaddr_in))
      {
        *dss_errno = DS_EFAULT;
        return DSS_ERROR;
      }
      tmp_addr = (struct ps_sockaddr_in *)sockaddr;
      v6addr->ps_sin6_port = tmp_addr->ps_sin_port;
      PS_IN6_GET_V4_MAPPED_V6_FROM_V4(&v6addr->ps_sin6_addr,
                                   tmp_addr->ps_sin_addr.ps_s_addr);
    break;

    case DSS_AF_INET6:
      if(addrlen != sizeof(struct ps_sockaddr_in6))
      {
        *dss_errno = DS_EFAULT;
        return DSS_ERROR;
      }
      *v6addr = *(struct ps_sockaddr_in6 *)sockaddr;
      break;

    default:
      *dss_errno = DS_EAFNOSUPPORT;
      return DSS_ERROR;
  }

  return DSS_SUCCESS;
}
/*===========================================================================
FUNCTION DSSOCKI_FILL_SOCKADDR()

DESCRIPTION
  This function check for validity of sockaddr and fills the structure with
  either ps_sockaddr_in or ps_sockaddr_in6
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Fills the sockaddr with the 2 tuple(IPaddr and port).
===========================================================================*/
void dssocki_fill_sockaddr
(
  struct ps_sockaddr     * sockaddr,                      /* Generic sockaddr */
  uint16              * addrlen,                     /* length of sockaddr */
  struct socket         localname,             /* IP addr and port 2 tuple */
  uint8                 family                           /* IP addr family */
)
{
  struct ps_sockaddr_in6 v6addr ;                  /* local copy of sockaddr6 */
  uint16 sockaddr_len;                     /* local copy of address length */
  struct ps_sockaddr_in * tmp_addr ;            /* tmp pointer to ps_sockaddr_in */
/*- - - - - - - - - - - -- - - - - - - - -  - - - - - - - - - - - - - - - -*/

  if(sockaddr == NULL || addrlen == NULL)
  {
    return;
  }
  tmp_addr = (struct ps_sockaddr_in *) &v6addr;
  memset(&v6addr, 0, sizeof(v6addr));

  if(PS_IN6_IS_ADDR_V4MAPPED(&localname.address) ||
     (family == (uint8)DSS_AF_INET &&
      PS_IN6_ARE_ADDR_EQUAL(&localname.address, &ps_in6addr_any)))
  {
    tmp_addr->ps_sin_family = (uint16)DSS_AF_INET;
    tmp_addr->ps_sin_port = localname.port;
    tmp_addr->ps_sin_addr.ps_s_addr =
      PS_IN6_GET_V4_FROM_V4_MAPPED_V6(&localname.address);
    sockaddr_len = sizeof(struct ps_sockaddr_in);
  }
  else
  {
    v6addr.ps_sin6_family = (uint16)DSS_AF_INET6;
    v6addr.ps_sin6_port = localname.port;
    v6addr.ps_sin6_addr = localname.address;
    sockaddr_len = sizeof(struct ps_sockaddr_in6);
  }

  memscpy(sockaddr, 
          MIN(*addrlen,sockaddr_len),
          tmp_addr,
          MIN(*addrlen,sockaddr_len));
  *addrlen = sockaddr_len;

  return;
}


/*===========================================================================
FUNCTION DSSOCKI_PS_MEM_LOW_WM_CBACK

DESCRIPTION
  Low watermark call back for ps cmd items.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Triggers socket events.
===========================================================================*/
void dssocki_ps_mem_low_wm_cback
(
  int16 wm_value //Sid
)
{
  dssocki_socket_event_occurred(NULL, NULL);
}



/*===========================================================================
FUNCTION DSSOCKI_BIND()

DESCRIPTION
  This function either binds or unbinds the the socket appropriately
  based on the the passed ps_sockaddr_in6 structure.

PARAMETERS
  *scb_ptr      - Pointer to the socket control block.
  *v6_sockaddr  - Pointer to the address to bind to.

RETURN VALUE
  0  Success
  -1 Failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
sint15 dssocki_bind
(
  scb_type            *scb_ptr,
  struct ps_sockaddr_in6 *v6_sockaddr
)
{
  struct socket        localname;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(NULL == scb_ptr)
  {
    //ASSERT(0);
    DS_ERROR_LOG_0( "scb_ptr is NULL");
    return DSS_ERROR;
  }

  DSSOCKI_CACHE_GET_LOCAL(scb_ptr, localname);

  if(NULL != v6_sockaddr)
  {


    if(PS_IN6_IS_ADDR_V6(&v6_sockaddr->ps_sin6_addr) && scb_ptr->family != (byte)DSS_AF_INET6)
    {
      DS_ERROR_LOG_1( "Can't bind to a v6 addr on socket w/family %d!",
                scb_ptr->family);
      return DSS_ERROR;
    }

    localname.address = v6_sockaddr->ps_sin6_addr;
    localname.port    = v6_sockaddr->ps_sin6_port;
    DSSOCKI_CACHE_SET_LOCAL(scb_ptr, localname);
  }
  else
  {

    localname.address = ps_in6addr_any;
    localname.port    = 0;
    DSSOCKI_CACHE_SET_LOCAL(scb_ptr, localname);
  }
  return 0;
}



/*===========================================================================
FUNCTION DSSOCKI_UPDATE_WRITE_EV()

DESCRIPTION
  This function iterates through all the sockets/scb's and invokes the socket
  callback if client who opened the socket is interested on DS_WRITE_EV

PARAMETERS

RETURN VALUE
  void

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void dssocki_update_write_ev()
{
  struct scb_s   * curr_scb_ptr = NULL;            /* Socket Ctl Block Ptr */
  uint32           event_mask;                          /* Happenend event */
  dssocki_app_to_ps_cmd_type  *ps_cmd = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  curr_scb_ptr = (scb_type *) q_check(&sock_config_cb.sock_cb_q);
  while (curr_scb_ptr != NULL)
  {
    DS_ASSERT(SOCKAVAIL != curr_scb_ptr->sockfd);
    if((ds_flow_is_valid(curr_scb_ptr->flow_ptr)) && 
       (ds_fwk_is_flow_enabled(curr_scb_ptr->flow_ptr)))
    {
/*---------------------------------------------------------------------
  Check if WRITE event needs to be posted so that apps, which got
  DS_EWOULDBLOCK previously, can start writing data again
---------------------------------------------------------------------*/
      event_mask = (uint32)dssocki_socket_event_check
                   (
                     curr_scb_ptr,
                     (curr_scb_ptr->requested_event_mask & DS_WRITE_EVENT)
                   );
      if (event_mask != 0)
      {
        dssocki_notify_app(curr_scb_ptr, event_mask);
      }

      /*
        Post flow enabled IND for TCP sockets so that TCP Retxn will happen
        for sockets which have packets pending in their SendQ
      */
      if ((curr_scb_ptr->protocol == PS_IPPROTO_TCP)&&
          (curr_scb_ptr->socket_state == DSSOCKI_OPEN))
      {
        ps_cmd = dssocki_alloc_ps_cmd_buffer(APP_TO_PS_SOCKET_FLOW_ENABLED_IND);
        if(NULL == ps_cmd)
        {
          DS_ERROR_LOG_0( "Command buffers could not be allocated");
          DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
          return;
        }
        dssocki_send_ps_cmd_buffer( curr_scb_ptr, (void *)FALSE, NULL, ps_cmd);
      }

    }
    curr_scb_ptr
      = (scb_type *) q_next(&sock_config_cb.sock_cb_q, &curr_scb_ptr->link);
  } /* while there is SCB in the queue */

  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  return;
}


/*===========================================================================
FUNCTION DSSOCKI_CLONE_SOCKET_OPTIONS()

DESCRIPTION
  This function clones the socket options set on the original TCP server socket to the sockets 
   created internally when dss_listen is invoked by the clients

PARAMETERS
    scb_ptr - The parent socket control block created when client calls dss_socket to create a TCP server socket
    lscb_ptr - Listner socket created internally by DS when dss_listen is called
RETURN VALUE
  void

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void dssocki_clone_socket_options
(
  struct scb_s *scb_ptr,
  struct scb_s *lscb_ptr
)
{
  uint32 socket_opt_value = 0;
  uint32 socket_opt_len = sizeof(uint32);
  sint15 dss_errno = 0;

  if(scb_ptr == NULL || lscb_ptr == NULL)
  {
    DS_MSG0(MSG_LEGACY_ERROR, "scb_ptr or lscb_ptr is NULL, can clone sock opt");
    return;
  }

  DS_MSG2(MSG_LEGACY_HIGH, "Cloning socket option for listner sock fd %d from"
                           " parent sock fd %d", lscb_ptr->sockfd, scb_ptr->sockfd);
	


  socket_opt_value = scb_ptr->max_backoff_time;
  if(dss_sock_opts_default[DSS_TCP_MAX_BACKOFF_TIME].initval != socket_opt_value)
  {
    if (DSS_ERROR == dss_setsockopt(lscb_ptr->sockfd,
                                    DSS_IPPROTO_TCP,DSS_TCP_MAX_BACKOFF_TIME,
                                    (void*)&socket_opt_value,
                                    &socket_opt_len,&dss_errno))
    {
      DS_MSG1(MSG_LEGACY_ERROR, "Cannot set DSS_TCP_MAX_BACKOFF_TIME "
                                "socket opt for listener sockfd %d",
                                lscb_ptr->sockfd);
      return;
    }
  }


  socket_opt_value = scb_ptr->cached_info.pcb.tcp.mss;
  if(dss_sock_opts_default[DSS_TCP_MAXSEG].initval != socket_opt_value)
  {
    if (DSS_ERROR == dss_setsockopt(lscb_ptr->sockfd,
                                    DSS_IPPROTO_TCP,DSS_TCP_MAXSEG,
                                    (void*)&socket_opt_value,
                                    &socket_opt_len,&dss_errno))
    {
      DS_MSG2(MSG_LEGACY_ERROR, "Cannot set DSS_TCP_MAXSEG "
                                "socket opt for listener sockfd %d errno %d",
                                lscb_ptr->sockfd, dss_errno);
      return;
    }
    DS_MSG3(MSG_LEGACY_HIGH, "Cloning DSS_TCP_MAXSEG socket option for listner sock fd %d from"
                             "parent sock fd %d, value %d",
                             lscb_ptr->sockfd, scb_ptr->sockfd,
                             lscb_ptr->cached_info.pcb.tcp.mss );
  }


  socket_opt_value = scb_ptr->cached_info.ip.tclass;
  if(dss_sock_opts_default[DSS_IPV6_TCLASS].initval != socket_opt_value)
  {
    if (DSS_ERROR == dss_setsockopt(lscb_ptr->sockfd,
                                    DSS_IPPROTO_IPV6,DSS_IPV6_TCLASS,
                                    (void*)&socket_opt_value,
                                    &socket_opt_len,&dss_errno))
    {
      DS_MSG1(MSG_LEGACY_ERROR, "Cannot set DSS_IPV6_TCLASS "
                                "socket opt for listener sockfd %d",
                                lscb_ptr->sockfd);
      return;
    }
  }



  socket_opt_value = scb_ptr->icmp_type;
  if(dss_sock_opts_default[DSS_ICMP_TYPE].initval != socket_opt_value)
  {
    if (DSS_ERROR == dss_setsockopt(lscb_ptr->sockfd,
                                    DSS_IPPROTO_ICMP,DSS_ICMP_TYPE,
                                    (void*)&socket_opt_value,
                                    &socket_opt_len,&dss_errno))
    {
      DS_MSG1(MSG_LEGACY_ERROR, "Cannot set DSS_ICMP_TYPE "
                                "socket opt for listener sockfd %d",
                                lscb_ptr->sockfd);
      return;
    }
  }


  socket_opt_value = scb_ptr->icmp_code;
  if(dss_sock_opts_default[DSS_ICMP_CODE].initval != socket_opt_value)
  {
    if (DSS_ERROR == dss_setsockopt(lscb_ptr->sockfd,
                                    DSS_IPPROTO_ICMP,DSS_ICMP_CODE,
                                    (void*)&socket_opt_value,
                                    &socket_opt_len,&dss_errno))
    {
      DS_MSG1(MSG_LEGACY_ERROR, "Cannot set DSS_ICMP_CODE "
                                "socket opt for listener sockfd %d",
                                lscb_ptr->sockfd);
      return;
    }
  }



  socket_opt_value = scb_ptr->rcvbuf;
  if(dss_sock_opts_default[DSS_SO_RCVBUF].initval != socket_opt_value)
  {
    if (DSS_ERROR == dss_setsockopt(lscb_ptr->sockfd,
                                    DSS_SOCK,DSS_SO_RCVBUF,
                                    (void*)&socket_opt_value,
                                    &socket_opt_len,&dss_errno))
    {
      DS_MSG1(MSG_LEGACY_ERROR, "Cannot set DSS_SO_RCVBUF "
                                "socket opt for listener sockfd %d",
                                lscb_ptr->sockfd);
      return;
    }
  }

  socket_opt_value = scb_ptr->sndbuf;
  if(dss_sock_opts_default[DSS_SO_SNDBUF].initval != socket_opt_value)
  {
    if (DSS_ERROR == dss_setsockopt(lscb_ptr->sockfd,
                                    DSS_SOCK,DSS_SO_SNDBUF,
                                    (void*)&socket_opt_value,
                                    &socket_opt_len,&dss_errno))
    {
      DS_MSG1(MSG_LEGACY_ERROR, "Cannot set DSS_SO_SNDBUF "
                                "socket opt for listener sockfd %d",
                                lscb_ptr->sockfd);
      return;
    }
  }

  socket_opt_value = scb_ptr->cached_info.ip.tos;
  if(dss_sock_opts_default[DSS_IP_TOS].initval != socket_opt_value)
  {
    if (DSS_ERROR == dss_setsockopt(lscb_ptr->sockfd,
                                    DSS_IPPROTO_IP,DSS_IP_TOS,
                                    (void*)&socket_opt_value,
                                    &socket_opt_len,&dss_errno))
    {
      DS_MSG1(MSG_LEGACY_ERROR, "Cannot set DSS_IP_TOS "
                                "socket opt for listener sockfd %d",
                                lscb_ptr->sockfd);
      return;
    }
  }


  if (IS_SOCKOPT_SET(scb_ptr->options_mask, (uint64)DSS_SO_LINGER))
  {
    SOCKOPT_SET(lscb_ptr->options_mask,DSS_SO_LINGER);
  }

  if (IS_SOCKOPT_SET(scb_ptr->options_mask, (uint64)DSS_SO_KEEPALIVE))
  {
    socket_opt_value = 1;
    if (DSS_ERROR == dss_setsockopt(lscb_ptr->sockfd,
                                    DSS_SOCK,DSS_SO_KEEPALIVE,
                                    (void*)&socket_opt_value,
                                    &socket_opt_len,&dss_errno))
    {
      DS_MSG1(MSG_LEGACY_ERROR, "Cannot set DSS_SO_KEEPALIVE "
                                "socket opt for listener sockfd %d",
                                lscb_ptr->sockfd);
    }
  }
   
  if (IS_SOCKOPT_SET(scb_ptr->cached_info.pcb.tcp.options.tcp_delayed_ack, (uint64)DSS_TCP_DELAYED_ACK))
  {
    socket_opt_value = scb_ptr->cached_info.pcb.tcp.options.tcp_delayed_ack;
    if (DSS_ERROR == dss_setsockopt(lscb_ptr->sockfd,
                                    DSS_IPPROTO_TCP,DSS_TCP_DELAYED_ACK,
                                    (void*)&socket_opt_value,
                                    &socket_opt_len,&dss_errno))
    {
      DS_MSG1(MSG_LEGACY_ERROR, "Cannot set DSS_TCP_DELAYED_ACK "
                                "socket opt for listener sockfd %d",
                                lscb_ptr->sockfd);
    }
  }

  if (IS_SOCKOPT_SET(scb_ptr->options_mask, (uint64)DSS_TCP_TIME_STAMP))
  {
    SOCKOPT_SET(lscb_ptr->options_mask,DSS_TCP_TIME_STAMP);
  }

  if (IS_SOCKOPT_SET(scb_ptr->options_mask, (uint64)DSS_IP_RECVIF))
  {
    SOCKOPT_SET(lscb_ptr->options_mask,DSS_IP_RECVIF);
  }

  if (IS_SOCKOPT_SET(scb_ptr->options_mask, (uint64)DSS_IP_RECVERR))
  {
    SOCKOPT_SET(lscb_ptr->options_mask,DSS_IP_RECVERR);
  }

  if (IS_SOCKOPT_SET(scb_ptr->options_mask, (uint64)DSS_IPV6_RECVERR))
  {
    SOCKOPT_SET(lscb_ptr->options_mask,DSS_IPV6_RECVERR);
  }

  if (IS_SOCKOPT_SET(scb_ptr->options_mask, (uint64)DSS_UDP_ENCAPS))
  {
    SOCKOPT_SET(lscb_ptr->options_mask,DSS_UDP_ENCAPS);
  }

  if (IS_SOCKOPT_SET(scb_ptr->options_mask, (uint64)DSS_TCP_NODELAY))
  {
    SOCKOPT_SET(lscb_ptr->options_mask,DSS_TCP_NODELAY);
  }

  if (IS_SOCKOPT_SET(scb_ptr->options_mask, (uint64)DSS_TCP_SACK))
  {
    SOCKOPT_SET(lscb_ptr->options_mask,DSS_TCP_SACK);
  }

  if (IS_SOCKOPT_SET(scb_ptr->options_mask, (uint64)DSS_SO_REUSEADDR))
  {
    SOCKOPT_SET(lscb_ptr->options_mask,DSS_SO_REUSEADDR);
  }
}




/*===========================================================================
FUNCTION DSSOCKI_UNLINK_SCB_FROM_PCB()

DESCRIPTION
  When the socket is closed and the scb is being freed, we need to unlink the
  scb set in the UCB/TCB/IPCB

PARAMETERS
  scb_ptr - Socket Control Block

RETURN VALUE
  void

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void dssocki_unlink_scb_from_pcb(scb_type *scb_ptr)
{
  struct tcb *tcb_ptr = NULL;
  struct icmp_cb *icb_ptr = NULL;
  struct udp_cb *ucb_ptr = NULL;

  if(scb_ptr == NULL )
  {
    DS_MSG0(MSG_LEGACY_ERROR, "scb_ptr is NULL");
    return;
  }

  DS_MSG2(MSG_LEGACY_HIGH, "dssocki_unlink_scb_from_pcb scb_ptr 0x%p sockfd %d",
          scb_ptr,scb_ptr->sockfd);

  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (scb_ptr->protocol == PS_IPPROTO_TCP)
  {
    tcb_ptr = scb_ptr->protocol_ctl_blk.tcb;
    if (tcb_ptr == NULL)
    {
      DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      DS_MSG0(MSG_LEGACY_ERROR, "No TCB found for SCB");
      return;
    }
    tcb_ptr->scb_ptr = NULL;
  }
  else if(scb_ptr->protocol == PS_IPPROTO_UDP)
  {
    ucb_ptr = scb_ptr->protocol_ctl_blk.ucb;
    if (ucb_ptr == NULL)
    {
      DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      DS_MSG0(MSG_LEGACY_ERROR, "No UCB found for SCB");
      return;
    }
    ucb_ptr->scb_ptr= NULL;
  }
  else if ((scb_ptr->protocol == PS_IPPROTO_ICMP) ||
           (scb_ptr->protocol == PS_IPPROTO_ICMP6))
  {
    icb_ptr = scb_ptr->protocol_ctl_blk.icb;
    if (icb_ptr == NULL)
    {
      DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      DS_MSG0(MSG_LEGACY_ERROR, "No ICB found for SCB");
      return;
    }
    icb_ptr->scb_ptr = NULL;
  }
  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
}

/*===========================================================================
FUNCTION DSSOCKI_SET_LINGER_TIMER()

DESCRIPTION
  This API sets the linger timer for the TCP socket when DSS_SO_LINGER or
  DSS_SO_LINGER_RESET with non-zero is set from the application

PARAMETERS
  scb_ptr - Socket Control Block

RETURN VALUE
  void

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
sint15 dssocki_set_linger_timer(scb_type* scb_ptr, sint15 *dss_errno )
{
  dssocki_app_to_ps_cmd_type *ps_cmd;

  DS_MSG2(MSG_LEGACY_HIGH, " Setting linger timer %d msecs for sock %d",
          scb_ptr->so_linger, scb_ptr->sockfd);

  ps_cmd = dssocki_alloc_ps_cmd_buffer(APP_TO_PS_SOCKET_LINGER_RESET);
  if(NULL == ps_cmd)
  {
    DS_ERROR_LOG_0( "Out of mem for PS cmd");
    *dss_errno = DS_ENOMEM;
    return DSS_ERROR;
  }


  ps_cmd->is_linger_on = TRUE;
  ps_cmd->time_in_sec = scb_ptr->so_linger;
  
  dssocki_send_ps_cmd_buffer( scb_ptr,
                              NULL,
                              NULL,
                              ps_cmd);

  return DSS_SUCCESS;
}


