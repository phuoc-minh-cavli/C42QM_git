/*===========================================================================

                           D S S U D P . C

DESCRIPTION

 The Data Services sockets UDP interface file. This contains all the
 functions used by the sockets library to access UDP socket function calls.

EXTERNALIZED FUNCTIONS
  dssudp_socket()
    UDP specific socket() function.  Sets up the UDP socket and creates
    the UDP control block.

  dssudp_write()
    UDP specific write() function.  Sends specified number of bytes out
    the UDP datagram transport.

  dssudp_read()
    UDP specific read() function.  Reads specified number of bytes from
    the UDP datagram transport.

  dssudp_close()
    UDP specific close() function.  Simply cleans up socket control
    block and makes the socket available for re-use.

  dssudp_bind()
    This is the UDP specific bind() function. It writes the port value into
    local.conn.port of the socket control block structure.

  dssudp_connect_setup()
    This is the UDP specific connect function. It fills in the peers ipaddr
    and binds the socket to a particular interface.

  dssudp_shutdown()
    This is the UDP specific shutdown function. It clears the receive queue
    of the socket and generates a socket event.

  dssudp_lookup_conn()
    Given a connection structure, looks up the connection to determine
    if this is a valid connection.

  dssudp_process_rx_pkt()
    Processes incoming UDP packet to see which socket it belongs to.

  dssudp_new_ip_upcall()
    If a new IP address is negotiated with the Base Station, then update
    udp ctrl blocks of the sockets using the interface with the new address

dssudp_set_rscope()
    UDP specific set_rscope function. Sets the incoming route scope in
    the UCB control block.

Copyright (c) 1998--2020 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dssudp.c_v   1.1   08 Oct 2002 23:16:52   akuzhiyi  $
  $Header: //components/rel/dataiot.mpss/2.0/protocols/inet/src/dssudp.c#3 $ $DateTime: 2020/08/04 12:06:05 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
07/29/20    sr     Changes to convert IPv4 source address to IPv6 format and 
                   use Ipv6 to Ipv4 conversion
04/28/20    ds     Code changes to set the DNE level for UDP recv watermark.
04/02/19    ds     Fixed Compiler warnings.
10/12/18    ds     Code changes to reset the meta info ptr within UDP control
                   block.
08/13/18    ds     Code changes as per new FWK design for IOT.
06/23/11    bd     Clean up pkt_meta_info_ptr() in the ip_cb control block 
                   after each packet write
10/01/10    sa     Added appropriate log messages before ASSERT(0).
05/22/09    kk     Q6 compiler warning fixes.
03/17/03    mct    Ensure we clear the source ip addr when our ip changes.
12/05/06    msr    Passing sockfd in ps_event_info_type
11/22/06    sv     Added support for IPSEC transport mode.
11/04/06    rt     Set the data_available flag as FALSE if the rcvcnt is 0
                   in UDP read functions.
11/03/06    hm     Using dssocki_alloc_ps_cmd_buffer and dssocki_send_ps_cmd_buffer
                   instead of dssocki_send_to_ps in dssudp_connect_setup.
11/02/06    mct    Added support for IPv6 Privacy Extensions.
07/31/06    rt     Route Scope modifications.
04/24/06    mct    Drop UDP packets if routing_cache is null.
04/17/06    rt     Changed comparison of uint in dssudp_read().
03/27/06    rt     L4 TASKLOCK review changes.
02/22/06    rt     Using single critical section
02/07/06    rt     L4 TASKLOCK changes.
01/31/05    rt     Added new F3 message in BIND function.
12/03/05    rt     Added a new scb_ptr field in dssocki_send_to_ps() fn and
                   got rid of pcb and protocol fields.
11/02/05    msr    Freeing metainfo in IPCB when dssudpi_free_ucb() is called
08/29/05    vp     Fixed bug in dssudp_read.
08/16/05    vp     Cleanup for propagating sys sock info to ipcb.
08/15/05    mct    Removed bcmcs featurization for multicast support.
08/15/05    vp     Propagating sys sock info to ipcb.
08/15/05    kr     Added support for stats update
08/15/05    vp     Support for IP_RECVERR.
08/12/05    sv     Cache ipcb routing cache from metainfo.
08/05/05    sv     Cache meta_info_ptr in IPCB.
04/18/05    vp     Changes for enforcing netpolicy on input.
04/18/05    msr    Fixed incorrect loop traversal in dssudp_lookup_conn().
04/17/05    msr    Using new signature of dssocki_generate_meta_info().
03/01/05    vp     Changes due to change in signature of dssocki_fill_sockaddr
02/22/05    ssh    Replaced INTLOCK()s with TASKLOCK()s
02/15/05    msr    Calling ip_setopt() for IPV6 level options.
02/11/05    msr    Moved binding a non-zero port for ephemeral ports to
                   dssocki_sendmsg().
12/27/04    ifk    Replaced setting of tracer field with DSM_TOUCH_ITEM
11/24/04    msr    memsetting UCB to 0 after it is freed in
                   dssudpi_free_ucb().
11/10/04    mct    Pull iface id off packet if IP_RECVIF option is set.
10/13/04    vp     Changes in dssudp_write() as port numbers are expected in
                   network order.
08/16/04    ks/msr Integrated UCBs in to ps_mem memory allocation.
                   Maintaining allocated UCBs in a list instead of array.
06/11/04    vp     Removal of included files psglobal.h, netuser.h. Included
                   files dssocket_defs.h and ps_socket_defs.h. Changes for
                   representation of IP addresses as struct ps_in_addr or struct
                   ps_in6_addr
05/25/04    sv     Modified dssudp_init_ucb to not to use high watermark for
                   rcvq of UDP socket.
05/17/04    sv     Modified dssudp_read to read a maximum of DSS_READ_MAX_BYTES
                   in a single read call.
04/028/04   aku    Added support for BCMCS feature.
03/30/04    vp     Merged changes from June04Dev_05.00.00 PS branch.
03/18/04    vp     Changes for removing direct scb access from PS context.
                   All such events are posted through info_change_upcall.
03/10/04    vp     Changes for dssocki_socket_event_occurred() to be executed
                   through the upcall mechanism.
02/24/04    sv     Added Null pointer check in dssudp_connect_setup.
02/04/04    vp     Replaced use of PS_IFACE_IP_V4_ADDR with ps_iface_get_addr
02/02/04    sv     Fixed host byte order bug in new_ip_upcall.
01/26/04    sv     Modified dssudp_read and dssudp_read_dsm_chain to use
                   the new watermark rcvq for UDP datagrams.
01/14/04    vp     Added function dssudp_new_ip_upcall() to take care of IP
                   address change due to handoff for udp sockets.
10/02/03    sv     Changed protocol specific function to take pcb instead
                   of SCB.
10/01/03    sv     Modified code so that udp_cb is accessed and freed only
                   in the PS task.
08/27/03    sv     Added TASKLOCK to udp connect code to avoid potential
                   race condition.
08/18/03    sv     Modified the code to use IPv6 addresses internally.
08/01/03    ss/sv  Restructured the code to support IPv6
07/22/03    sv     Modified the code to use new DSM macros for getting the
                   DSM declarations.
07/17/03    ss     Decremented the rcvcnt in dssudp_read() while discarding
                   data due to the supplied buffer being too small
05/28/03    ss     Changed code to handle rcvq as a dsm item chain instead of
                   queue. Added support for rcvbuf socket option.
05/27/03    om     Added dssudp_read_dsm_chain().
05/13/03    ss     Removed an unused variable declaration from dssudp_close()
05/08/03    ss     Added dssudpi_free_rcvq() and INTLOCK'ed queue accesses
05/05/03    ss     Protected q_put() and q_get() within INTLOCK's
04/15/03    sv     Added dssudp_connect_setup and dssudp_shutdown functions.
                   Modified dssudp_read to accept iov and iocount instead of
                   buffer and bufferlen as arguments.
04/15/03    sv     Modified dssudp_process_rx_pkt to support connected udp
                   sockets and to accept ucb_ptr and dsm_item as arguments.
                   Added dssudp_lookup_conn function.
04/10/03    ss     Modified dssudp_write() to change the parameter to
                   getephem_port()
10/08/02    aku    Removed FEATURE_DATA_MM featurization.
08/04/02    aku    Merged in changes for multimode ps under FEATURE_DATA_MM
07/21/02    na     Fixed comment
07/15/02    na     Added dssudp_process_rx_pkt() (used to be ps_process_udp
                   in old psmgr.c)
07/14/02    na     Fixed ip_addr_type to support FEATURE_DATA_MM.
05/02/02    mvl    File clean up, added meta_info_ptr to write()
04/08/02    ss     Modified dssudp_read() to truncate the datagram if the
                   supplied buffer is too small.
12/18/01    rsl    Updated tracer field.
07/23/01    pjb    Return type of dsm_pushdown changed.
06/27/01    na     Replaced DSSUDPI_MAX_UDP_SOCKS with DSS_MAX_UDP_SOCKS
                   making the usage consistent.
03/27/01    mt     Changed type of second argument in dssudp_read() from
                   byte * to void *.
03/27/01    js     Added extra parameter to function dssudp_write(). The
                   extra parameter holds a pointer to the structure holding
                   the destination information. This avoids using the
                   destination information in scb, which may be stale.
02/14/01    js     Modified call to udp_output() to support DSS_IP_TTL.
11/10/00    snn    Renamed scb_type as scb_s
08/25/00    rc     Changed dsm_pushdown() to take take pool id as fourth
                   parameter.Made changes to handle dsm_pushdown()
                   returning FALSE.
08/03/00    mvl/js Added function dssudp_bind() for the protocol specific
                   assignment of local.conn.port.
06/07/00    rc     Removed ovld of link ptr in dsm items.
05/11/00    mvl    Modified DSSUDPI_MAX_UDP_SOCKS to support multiple UDP
                   sockets (the current number is 4).
12/23/99    na     Changed q_inserts to use the ovld of link ptr in dsm items.
04/09/99    hcg    Added FEATURE_Q_NO_SELF_QPTR to support queue services memory
                   optimization.
04/08/99    hcg    Added host to network byte conversion for dssudp_read() to
                   convert the incoming IP address to network byte ordering.
04/05/99    hcg    Incorporated code review changes and comments.  Changed
                   some of the diagnostic message levels.  Modified use of
                   INTLOCK()/INTFREE() macros to ensure protection of access
                   to global data.
12/09/98   na/hcg Created module.
===========================================================================*/

/*===========================================================================

                       INCLUDE FILES FOR THE MODULE

===========================================================================*/

#include "comdef.h"
#include "target.h"
#include "customer.h"
#include "amssassert.h"
#include "msg.h"
#include "ps.h"
#include "dsm.h"
#include "ps_ip.h"
#include "memory.h"
#include "msg.h"
#include "err.h"
#include "ps_tx_meta_info.h"
#include "ps_socket_defs.h"
#include "dssi.h"
#include "ps_pcb_close_reason_codes.h"

#include "ps_udp_io.h"
#include "dssudp.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_in.h"
#include "ps_metai_info.h"
#include "ds_system_heap.h"

/*===========================================================================

                      LOCAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION DSSUDPI_CREATE_UCB()

DESCRIPTION
  This function creates a new UDP control block.

DEPENDENCIES
  None.

RETURN VALUE
  Returns pointer to UDP control block structure.  If none are
  available, returns NULL.

SIDE EFFECTS
  None.
===========================================================================*/
static struct udp_cb *dssudpi_create_ucb
(
struct scb_s *scb_ptr      /* Ptr to socket ctl block. */
)
{
  struct udp_cb* ucb_ptr = NULL;               /* Ptr to UCB to be created */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == scb_ptr)
    {
    LOG_MSG_ERROR_0( "NULL scb_ptr passed" );
    DS_ASSERT( 0 );
    return NULL;
  }

  DS_SYSTEM_HEAP_MEM_ALLOC(ucb_ptr,sizeof(struct udp_cb),struct udp_cb*);
  /*-------------------------------------------------------------------------
    If no UDP control block is available, return NULL.
   ------------------------------------------------------------------------*/
  if (NULL == ucb_ptr)
  {
    LOG_MSG_ERROR_0("Memory allocation for UCB failed");
    return (NULL);
  }

  memset(ucb_ptr, 0, sizeof(struct udp_cb));
  ucb_ptr->scb_ptr = scb_ptr;
  ucb_ptr->sockfd = scb_ptr->sockfd;
  
  /*-------------------------------------------------------------------------
    Initialize link and add UCB to queue
  -------------------------------------------------------------------------*/
  (void) q_link(ucb_ptr, &(ucb_ptr->link));
  q_put(&sock_config_cb.udp_cb_q, &(ucb_ptr->link));
  
  return (ucb_ptr);
} /* dssudpi_create_ucb() */

/*===========================================================================
FUNCTION DSSUDPI_INIT_UCB()

DESCRIPTION
  This function creates a new UDP control block.

DEPENDENCIES
  None.

RETURN VALUE
  Returns pointer to UDP control block structure.  If none are
  available, returns NULL.

SIDE EFFECTS
  None.
===========================================================================*/
static void dssudpi_init_ucb
(
  struct udp_cb * ucb_ptr                      /* Ptr to UDP control block */
)
{
  ucb_ptr->rcvbuf = DEF_WND;

  dsm_queue_init(&(ucb_ptr->rcvq_wm),
                 0x7FFFFFFF, /* Maxint */
                 &(ucb_ptr->rcvq));
  dsm_set_dne(&(ucb_ptr->rcvq_wm), ucb_ptr->rcvbuf);
 
  dsm_queue_init(&(ucb_ptr->errq_wm),
                 0x7FFFFFFF, /* Maxint */
                 &(ucb_ptr->errq));
  ip_init_ipcb(&ucb_ptr->ipcb);
  return;
}



/*===========================================================================
FUNCTION DSSUDPI_FREE_UCB()

DESCRIPTION
  This function frees UDP control block, making it available for re-use.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void dssudpi_free_ucb
(
  struct udp_cb* ucb_ptr                         /* Ptr to UCB to be freed */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( NULL == ucb_ptr )
  {
    LOG_MSG_ERROR_0( "NULL UCB passed" );
    DS_ASSERT( 0 );
    return;
  }

  if (ucb_ptr->ipcb.meta_info_ptr != NULL)
  {
    PS_TX_META_INFO_FREE(&(ucb_ptr->ipcb.meta_info_ptr));
  }

  /*---------------------------------------------------------------------------------------------
    Destroy the rcvq and errq DSM queues
  --------------------------------------------------------------------------------------------*/

  dsm_queue_destroy(&(ucb_ptr->rcvq_wm));

  dsm_queue_destroy(&(ucb_ptr->errq_wm));
  /*-------------------------------------------------------------------------
    Remove UCB from the queue and free it.

    NOTE : UCB must be memset to 0 because application could call dss_close()
    multiple times with same sockfd. By memsetting to 0, sockfd's status
    would become SOCKAVAIL and control block is not freed again because
    dssudp_close() frees a control block only if sockfd's status is not
    SOCKAVAIL.
   ------------------------------------------------------------------------*/
  q_delete(&sock_config_cb.udp_cb_q, (q_link_type *) ucb_ptr);
  memset(ucb_ptr, 0, sizeof(struct udp_cb));
  DS_SYSTEM_HEAP_MEM_FREE(ucb_ptr);

} /* dssudpi_free_ucb() */



/*===========================================================================

                      GLOBAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION DSSUDP_CONNECT()

DESCRIPTION
  This function sets the remote address for the udp socket.

  This function is called from the context of the PS  task.

DEPENDENCIES
  Before this function is called, the destination IP address/port number
  should have been put in the scb in the name and peername fields.

RETURN VALUE
  On success, returns DSS_SUCCESS.
  On failure returns DSS_ERROR

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dssudp_connect
(
  protocol_ctl_block_type  pcb,   /* Protocol control block for the socket */
  struct ps_sockaddr_in6 *dest_ptr,                /* destination socket addr */
  ps_meta_info_type *meta_info_ptr                     /* meta information */
)
{
  struct udp_cb              * ucb_ptr = pcb.ucb;
  ps_ip_addr_type              ip_addr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == ucb_ptr)
  {
    LOG_MSG_ERROR_0( "NULL UCB" );
    DS_ASSERT( 0 );
    return DSS_ERROR;
  }

  ucb_ptr->conn.remote.address = dest_ptr->ps_sin6_addr;
  ucb_ptr->conn.remote.port    = dest_ptr->ps_sin6_port;

  /*-------------------------------------------------------------------------
    Update flow_label from dest_ptr in IPCB
  -------------------------------------------------------------------------*/
  ucb_ptr->ipcb.flow_label = dest_ptr->ps_sin6_flowinfo;

  if (!PS_IN6_IS_ADDR_UNSPECIFIED(&( ucb_ptr->conn.remote.address)))
  {
    if(PS_IN6_IS_ADDR_UNSPECIFIED(&( ucb_ptr->conn.local.address)))
    {
      /*---------------------------------------------------------------------
        Check if routing cache is present
      ---------------------------------------------------------------------*/
      if (ucb_ptr->ipcb.meta_info_ptr == NULL ||
          PS_TX_META_GET_RT_META_INFO_PTR(ucb_ptr->ipcb.meta_info_ptr) == NULL)
      {
        LOG_MSG_ERROR_0("IPCB meta_info_ptr is NULL in dssudp_connect");
        DS_ASSERT(0);
        return DSS_ERROR;
      }

      /*---------------------------------------------------------------------
        Get the source address with appropriate scope based on destination
        addr.  This must be done for ICMPv6 messages.
      ---------------------------------------------------------------------*/
      ds_fwk_get_ip_addr(
      (ds_fwk_s_type *) PS_TX_META_GET_ROUTING_CACHE(ucb_ptr->ipcb.meta_info_ptr),
      IPV6_ADDR, &ip_addr);
      if(ip_addr.type == IP_ADDR_INVALID)
      {
        LOG_MSG_ERROR_0("Error");
        return DSS_ERROR;
      }

      ucb_ptr->conn.local.address = ip_addr.addr.v6;
      ucb_ptr->ipcb.src_ip_addr   = ip_addr.addr.v6;
    }
  }

  return DSS_SUCCESS;

} /* dssudp_connect() */

/*===========================================================================
FUNCTION DSSUDP_SET_ROUTE_META_INFO()

DESCRIPTION
  This function processes setting of routing meta info inside IPCB.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dssudp_set_route_meta_info
(
  protocol_ctl_block_type pcb_handle,
  int32                   sockfd,
  ps_rt_meta_info_type *  rt_mi_ptr
)
{
  struct udp_cb         * ucb_ptr = pcb_handle.ucb;
  ps_tx_meta_info_type  * tx_mi_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (rt_mi_ptr == NULL)
  {
    LOG_MSG_ERROR_0("NULL rt_mi_ptr");
    DS_ASSERT(0);
    return;
  }

  if (NULL == ucb_ptr)
  {
    PS_RT_META_INFO_FREE(&rt_mi_ptr);
    LOG_MSG_ERROR_0( "NULL UCB" );
    DS_ASSERT( 0 );
    return;
  }

  if (ucb_ptr->sockfd != sockfd)
  {
    PS_RT_META_INFO_FREE(&rt_mi_ptr);
    LOG_MSG_ERROR_2("Sockfd %d != UCB's sockfd %d", sockfd, ucb_ptr->sockfd);
    return;
  }

  /*-------------------------------------------------------------------------
    Get TX meta info and update incoming routing meta info!
  -------------------------------------------------------------------------*/
  PS_TX_META_INFO_GET(tx_mi_ptr);
  if (tx_mi_ptr == NULL)
  {
    LOG_MSG_ERROR_0("Out of mem for TX_META_INFO");
    PS_RT_META_INFO_FREE(&rt_mi_ptr);
    DS_ASSERT(0);
    return;
  }

  PS_TX_META_SET_RT_META_INFO_PTR(tx_mi_ptr, rt_mi_ptr);

  /*-------------------------------------------------------------------------
    Update IPCB TX meta info!
    - Clear any existing meta info before assigning new TX meta info in IPCB
  -------------------------------------------------------------------------*/
  if (ucb_ptr->ipcb.meta_info_ptr != NULL)
  {
    PS_TX_META_INFO_FREE(&ucb_ptr->ipcb.meta_info_ptr);
  }

  ucb_ptr->ipcb.meta_info_ptr = tx_mi_ptr;

  ps_ip_update_routing_cache_in_ipcb
  (
    &(ucb_ptr->ipcb),
    (ds_fwk_s_type*)
      PS_TX_META_GET_ROUTING_CACHE(ucb_ptr->ipcb.meta_info_ptr)
  );

  return;
} /* dssudp_set_route_meta_info() */

/*===========================================================================
FUNCTION DSSUDP_SOCKET()

DESCRIPTION
  This function sets up the UDP socket and creates the UDP control block.

  This function is called from the context of the application task.

DEPENDENCIES
  None.

RETURN VALUE
  On success, returns DSS_SUCCESS

  On error, return DSS_ERROR and places the error condition value in *errno.

  Errno Values
  ------------
  DS_EMFILE           too many descriptors open.  A socket is already open or
                      has not closed compeletely.
  DS_EPROTOTYPE       specified protocol invalid for socket type.

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dssudp_socket
(
  struct scb_s* scb_ptr,     /* Ptr to socket control block for the socket */
  sint15 *dss_errno                                   /* error condition value */
)
{
  struct udp_cb *ucb_ptr;                    /* UDP protocol control block */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check to see that protocol matches socket type.
   ------------------------------------------------------------------------*/
  if ( (scb_ptr->protocol != (byte)PS_IPPROTO_UDP) && (scb_ptr->protocol != 0) )
  {
    *dss_errno = DS_EPROTOTYPE;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Create UDP control block.
   ------------------------------------------------------------------------*/
  if ( (ucb_ptr = dssudpi_create_ucb( scb_ptr)) == NULL)
  {
    LOG_MSG_ERROR_0(" Could not create a UCB");
    *dss_errno = DS_EMFILE;
    return (DSS_ERROR);
  }
  dssudpi_init_ucb( ucb_ptr);

  /*-------------------------------------------------------------------------
    Set socket state to OPEN
   ------------------------------------------------------------------------*/
  dssocki_set_sock_state(scb_ptr, DSSOCKI_OPEN);

  /*-------------------------------------------------------------------------
    Associate the ucb with the scb through the pcb ptr in the scb
   ------------------------------------------------------------------------*/
  scb_ptr->protocol_ctl_blk.ucb = ucb_ptr;

  return (DSS_SUCCESS);

} /* dssudp_socket() */


/*===========================================================================
FUNCTION DSSUDP_WRITE()

DESCRIPTION
  Sends dsm memory item out to UDP using udp_output found in udp.c.

  This function is called from the context of the PS task.

DEPENDENCIES
  None.

RETURN VALUE
  The number of bytes written.

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dssudp_write
(
  protocol_ctl_block_type pcb,             /* protocol control block       */
  struct ps_sockaddr_in6 *ta_ptr,             /* destination socket info      */
  dsm_item_type *item_ptr,                 /* ptr to head of dsm pkt chain */
  ps_tx_meta_info_type *meta_info_ptr         /* ptr to meta info             */
)
{
  struct udp_cb *ucb_ptr;                    /* UDP protocol control block */
  struct ps_in6_addr  *dest_ip_ptr;                 /* Ptr to dest IP address */
  udp_hdr_type  udp_hdr;                                     /* UDP header */
  sint15 length;                              /* length of payload written */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Setup the socket and UDP control block pointers.
   ------------------------------------------------------------------------*/
  ucb_ptr = pcb.ucb;
  dest_ip_ptr = &(ucb_ptr->conn.remote.address);

  /*-------------------------------------------------------------------------
    Cache the meta_info ptr
   ------------------------------------------------------------------------*/
  if(meta_info_ptr != NULL)
  {
    ucb_ptr->ipcb.meta_info_ptr = meta_info_ptr;
    ucb_ptr->ipcb.routing_cache = PS_META_GET_ROUTING_CACHE( meta_info_ptr);
  }

  /*-------------------------------------------------------------------------
    Drop the UDP packet if the iface is NULL. This can occur because there
    are already packets queued to be sent, but the iface down event is
    processed first and clears the routing_cache in the ipcb.
  -------------------------------------------------------------------------*/
  if(ucb_ptr->ipcb.routing_cache == NULL)
  {
    //META info would be freed when socket is closed
    LOG_MSG_ERROR_0("NULL routing_cache has gone down.");
    dsm_free_packet(&item_ptr);
    return DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    Use destination information from dsm_item chain. If not possible.
    use the information in scb (may be stale).
  -------------------------------------------------------------------------*/
  /**/
  if (ta_ptr != NULL && !PS_IN6_ARE_ADDR_EQUAL(&ta_ptr->ps_sin6_addr, &ps_in6addr_any))
  /**/
  {
    dest_ip_ptr = &(ta_ptr->ps_sin6_addr);
  }

  /*-------------------------------------------------------------------------
    Ephemeral ports are taken care of in dssocki_sendmsg()
  -------------------------------------------------------------------------*/
  DS_ASSERT(0 != ucb_ptr->conn.local.port);

  /*-------------------------------------------------------------------------
    Fill in the known UDP header fields, except the checksum.
  -------------------------------------------------------------------------*/
  udp_hdr.source = ucb_ptr->conn.local.port;

  /*-------------------------------------------------------------------------
    Use the non-zero port number from dsm_item chain. If not possible.
    use the information in scb (may be stale).
  -------------------------------------------------------------------------*/
  if (( ta_ptr != NULL ) && ( ta_ptr->ps_sin6_port != 0 ))
  {
    udp_hdr.dest = ta_ptr->ps_sin6_port;
  }
  else
  {
    udp_hdr.dest = ucb_ptr->conn.remote.port;
  }

  length         = (sint15) dsm_length_packet(item_ptr);
  udp_hdr.length = (word) ((uint16) length + sizeof(udp_hdr_type));

  /*-------------------------------------------------------------------------
    Call udp_output, which will make up the UDP header and send the packet to
    IP.
   ------------------------------------------------------------------------*/
  DSM_TOUCH_ITEM(item_ptr);
  udp_output(dest_ip_ptr, &udp_hdr, item_ptr, ucb_ptr, meta_info_ptr);

  /*-------------------------------------------------------------------------
    Return number of bytes sent out.
   ------------------------------------------------------------------------*/
  return (length);

} /* dssudp_write() */

/*===========================================================================
FUNCTION DSSUDP_CONNECT_SETUP()

DESCRIPTION
  This function connects a udp socket by filling up the peer address and port
  in the udp control block.

DEPENDENCIES
  Before this function is called, the destination IP address/port number
  should have been put in the scb in the name and peername fields.

RETURN VALUE
  On success, returns DSS_SUCCESS.
  On failure returns DSS_ERROR

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dssudp_connect_setup
(
  struct scb_s* scb_ptr,     /* Ptr to socket control block for the socket */
  struct ps_sockaddr_in6* dest_ptr,                    /* destination address */
  sint15 *dss_errno                        /* address of error condition value */
)
{
  ps_ip_addr_type if_v6addr;                    /* Address of the interface */
  struct socket localname;                /* local address and port number */
  struct socket remotename;              /* remote address and port number */
  struct ps_sockaddr_in6 localaddr;                          /* local address */
  ps_tx_meta_info_type * tx_meta_info_ptr;                       /* PS meta info */
  dssocki_app_to_ps_cmd_type  * ps_bind_cmd;
  dssocki_app_to_ps_cmd_type  * ps_connect_cmd;
  boolean               update_binding = FALSE;
  ds_fwk_s_type       * ds_fwk_ptr;
  ip_addr_enum_type     ip_ver;
  boolean               ipcb_meta_info_valid = FALSE;
  struct ps_sockaddr_in v4_addr;
  struct ps_sockaddr_in6 v6_sockaddr;
  /*- - - - - - - - - - - - - - - - - -  - - - - - - - - -- - - - - - - - -*/
  LOG_MSG_INFO2_0(" Connecting UDP ");

  /*------------------------------------------------------------------------
    Check arguments.
  -------------------------------------------------------------------------*/
  if (scb_ptr == NULL)
  {
    *dss_errno = DS_EFAULT;
    return (DSS_ERROR);
  }

  DSSOCKI_CACHE_GET_LOCAL(scb_ptr, localname);

  ps_connect_cmd = dssocki_alloc_ps_cmd_buffer(APP_TO_PS_SOCKET_CONNECT);
  if(NULL == ps_connect_cmd)
  {
    LOG_MSG_ERROR_0("Cannot alloc memory for ps connect command");
    *dss_errno = DS_ENOMEM;
    return DSS_ERROR;
  }

  if ((tx_meta_info_ptr = dssocki_generate_meta_info( scb_ptr,
                                                   scb_ptr->flow_ptr,
                                                   0,
                                                   &ipcb_meta_info_valid,
                                                   dss_errno )) == NULL )
  {
    LOG_MSG_ERROR_1("MetaInfo gen. failed (%d)", *dss_errno);
    PS_MEM_FREE(ps_connect_cmd);
    *dss_errno = DS_ENOMEM;
    return DSS_ERROR;
  }

  /*--------------------------------------------------------------------------
    if server address is null disconnect the socket.
  --------------------------------------------------------------------------*/
  if(dest_ptr == NULL)
  {
    localname.address = ps_in6addr_any;
    remotename.address = ps_in6addr_any;
    remotename.port    = 0;

    /*-----------------------------------------------------------------------
      Set the route scope as now the socket2 can use any interface satisfying
      the network policy. Socket1's can now use the app's ps_iface_ptr or
      the loopback ifaces.
    -----------------------------------------------------------------------*/
  }
  else
  {

    remotename.address = dest_ptr->ps_sin6_addr;
    remotename.port    = dest_ptr->ps_sin6_port;
    /*-----------------------------------------------------------------------
      If address not bound to socket, assign it the local IP. Till we support
      binding to a particular interface we keep binding to new local ipaddress
      for each connect call.
    -----------------------------------------------------------------------*/
    if(PS_IN6_IS_ADDR_UNSPECIFIED(&localname.address))
    {
      ds_fwk_ptr = scb_ptr->cached_info.ip.routing_cache;

      if (ds_fwk_ptr == NULL)
      {
        DS_ERROR_LOG_0("Fwk instance is NULL");
        PS_MEM_FREE(ps_connect_cmd);
        PS_TX_META_INFO_FREE(&tx_meta_info_ptr);
        *dss_errno = DS_ENOSRCADDR;
        return DSS_ERROR;
      }

      if (scb_ptr->family == IPV4_ADDR)
      {
        ip_ver = IPV4_ADDR;
      }
      else if (scb_ptr->family == IPV6_ADDR)
      {
       ip_ver = IPV6_ADDR;
      }
      else
      {
        DS_ERROR_LOG_0("Invalid IP family");
        return DSS_ERROR;
      }

      ds_fwk_get_ip_addr(ds_fwk_ptr,ip_ver, &if_v6addr);

      if (if_v6addr.type == IP_ADDR_INVALID)
      {
        DS_ERROR_LOG_0("IP address type fetched from fwk inst is invalid");
        PS_MEM_FREE(ps_connect_cmd);
        PS_TX_META_INFO_FREE(&tx_meta_info_ptr);
        *dss_errno = DS_ENOSRCADDR;
        return DSS_ERROR;
      }

      if(scb_ptr->family == DSS_AF_INET)
      {
        v4_addr.ps_sin_family = DSS_AF_INET;
        v4_addr.ps_sin_addr.ps_s_addr = if_v6addr.addr.v4.ps_s_addr;
        v4_addr.ps_sin_port = localname.port;

/*-------------------------------------------------------------------------
    Convert ps_sockaddr to ps_sockaddr_in6.
-------------------------------------------------------------------------*/
        if( dssocki_get_sockaddr_in6(&v6_sockaddr, 
            (struct ps_sockaddr*)&v4_addr, 
            sizeof(struct ps_sockaddr_in), dss_errno) == DSS_ERROR )
        {
          DS_MSG1(MSG_LEGACY_ERROR, "Invalid Source address: ERRNO %d", *dss_errno);
          return DSS_ERROR;
        }
        localname.address = v6_sockaddr.ps_sin6_addr;
      }
      else if (scb_ptr->family == DSS_AF_INET6)
      {
        memscpy(localname.address.ps_s6_addr,
                sizeof(struct ps_in6_addr),
                if_v6addr.addr.v6.ps_s6_addr,
                sizeof(struct ps_in6_addr));
      }

      update_binding = TRUE;
    }

    /*-------------------------------------------------------------------------
      If socket is not already attached to a local address port, assign
      ephemeral port.
    -------------------------------------------------------------------------*/
    if (localname.port == 0)
    {
      localname.port = dssocki_getephem_port(scb_ptr->protocol);
      update_binding = TRUE;
    }

    if(update_binding)
    {
      localaddr.ps_sin6_family = (uint16)DSS_AF_INET6;
      localaddr.ps_sin6_port = localname.port;
      localaddr.ps_sin6_addr = localname.address;

      ps_bind_cmd = dssocki_alloc_ps_cmd_buffer(APP_TO_PS_SOCKET_BIND);
      if(NULL == ps_bind_cmd)
      {
        *dss_errno = DS_ENOMEM;
        if (!ipcb_meta_info_valid)
        {
          PS_TX_META_INFO_FREE(&tx_meta_info_ptr);
        }
        PS_MEM_FREE(ps_connect_cmd);
        LOG_MSG_ERROR_0("Not enough memory for PS commands");
        return DSS_ERROR;
      }

      DSSOCKI_CACHE_SET_LOCAL(scb_ptr, localname);
      dssocki_send_ps_cmd_buffer( scb_ptr,
                                  NULL,
                                  &localaddr,
                                  ps_bind_cmd);
    }
  }
  DSSOCKI_CACHE_SET_REMOTE(scb_ptr, remotename);
  dssocki_send_ps_cmd_buffer( scb_ptr,
                              (void*)tx_meta_info_ptr,
                              dest_ptr,
                              ps_connect_cmd);

  return(DSS_SUCCESS);
} /* dssudp_connect_setup() */

/*===========================================================================
FUNCTION DSSUDP_READ()

DESCRIPTION
  Reads 'nbytes' bytes in the buffer from the UDP transport.  Fills in
  address structure with values from who sent the data in fromaddr.
  This function assumes that fromaddr is not NULL.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes to be written, which could be less than the number
      of bytes specified.

  On error, return DSS_ERROR and places the error condition value in *errno.

  Errno Values
  ------------
  DS_EWOULDBLOCK      operation would block

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dssudp_read
(
  struct scb_s* scb_ptr,     /* Ptr to socket control block for the socket */
  struct ps_iovec *iov,     /* scattered user buffer from where to copy data */
  uint16 iovcount,                                /* lenght of iovec array */
  struct ps_sockaddr *fromaddr,                        /* destination address */
  uint16 * addrlen,                                      /* address length */
  int    flags,                                          /* flags from app */
  sint15 *dss_errno                                   /* error condition value */
)
{
  struct udp_cb *ucb_ptr;                    /* UDP protocol control block */
  uint16 bytes_read =0;                    /* # of bytes to read from rcvq */
  uint16 bytes_requested =0;  /* # of bytes to requested in each iteration */
  uint16 payload_len;                             /* Length of UDP payload */
  uint16 remaining_len;   /* remaining length of the datagram to be copied */
  uint16 read_cnt=0;                  /* # of bytes read in each iteration */
  uint16 cnt=0;                                /* tmp # of bytes retrieved */
  uint32 iface_id;                         /* interface packet was rx'd on */
  int i;                                          /* Local loop variable   */
  struct socket src_addr;            /* source address  of received packet */
  dsm_item_type * item_ptr;                           /* recieved datagram */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Ensure that the "from address" is not NULL.  This function fills in this
    structure, thus, needs to ASSERT against dereferencing a NULL pointer.
   ------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    Set the socket and UDP control block pointers, and set the family to
    AF_INET.
  -------------------------------------------------------------------------*/
  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  ucb_ptr = scb_ptr->protocol_ctl_blk.ucb;
  LOG_MSG_INFO1_3("flags %d ucb_ptr 0x%p dss_errno 0x%p",flags, ucb_ptr, 
                  dss_errno);
  if((flags & DSS_MSG_ERRQUEUE) == 0)
  {
    /*-----------------------------------------------------------------------
      Check if there is anything in the UDP receive queue. If not, return
      DS_EWOULDBLOCK.
    -----------------------------------------------------------------------*/
    if (ucb_ptr->rcv_pktcnt == 0)
    {
      *dss_errno = DS_EWOULDBLOCK;
      scb_ptr->data_available = FALSE;
      LOG_MSG_INFO1_0("return ewould block, Data Not availble");
      DS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      return (DSS_ERROR);
    }

    item_ptr = dsm_dequeue(&(ucb_ptr->rcvq_wm));
    DS_ASSERT(item_ptr != NULL);
    LOG_MSG_INFO1_1("packet count %d",ucb_ptr->rcv_pktcnt);
    ucb_ptr->rcv_pktcnt --;

    /*-----------------------------------------------------------------------
      If IP_RECVIF option is set record iface pkt was received on.
    -----------------------------------------------------------------------*/
    if(item_ptr->kind == 1)
    {
      cnt = dsm_pullup(&(item_ptr), &iface_id, sizeof(iface_id));
      if( sizeof(iface_id) != cnt )
      {
        LOG_MSG_ERROR_0("DSM Pullup error");
        dsm_free_packet(&item_ptr);
        return DSS_ERROR;
      }

      if(IS_SOCKOPT_SET(ucb_ptr->ipcb.options_mask, (uint64)DSS_IP_RECVIF))
      {
        scb_ptr->rx_iface_id = iface_id;
      }
      else
      {
        scb_ptr->rx_iface_id = 0;
      }
    }
    /*--------------------------------------------------------------------
      Extract the payload length, server's IP address, and port number
      and store in fromaddr.  ASSERT that bytes to read, are equal to the
      number of bytes pulled up.
    -----------------------------------------------------------------------*/


    cnt = dsm_pullup(&(item_ptr), &payload_len, sizeof(payload_len));
    if(cnt != sizeof(payload_len)) goto bail;

    cnt = dsm_pullup(&(item_ptr), &(src_addr.port),
                     sizeof(src_addr.port));
    if(cnt != sizeof(src_addr.port)) goto bail;

    cnt = dsm_pullup(&(item_ptr), &(src_addr.address),
                     sizeof(src_addr.address));
    if(cnt != sizeof(src_addr.address)) goto bail;


  }
  else  /* flags == DSS_MSG_ERRQUEUE */
  {
    if (ucb_ptr->err_pktcnt == 0)
    {
      *dss_errno = DS_EWOULDBLOCK;
      LOG_MSG_INFO1_0("Data Not availblae");
      DS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      return (DSS_ERROR);
    }

    item_ptr = dsm_dequeue(&(ucb_ptr->errq_wm));
    DS_ASSERT(item_ptr != NULL);

    ucb_ptr->err_pktcnt --;

    /*-----------------------------------------------------------------------
      Pull out the destination address of original datagram that generated
      this error.
    -----------------------------------------------------------------------*/
    cnt = dsm_pullup(&(item_ptr), &(src_addr.port),
                     sizeof(src_addr.port));
    if(cnt != sizeof(src_addr.port)) goto bail;

    /*-----------------------------------------------------------------------
      Pull out the destination address of original datagram that generated
      this error.
    -----------------------------------------------------------------------*/
    cnt = dsm_pullup(&(item_ptr), &(src_addr.address),
                     sizeof(src_addr.address));
    if(cnt != sizeof(src_addr.address)) goto bail;

    /*-----------------------------------------------------------------------
      Pull out the extended_err structure and address of the node which
      generated this error.
    -----------------------------------------------------------------------*/
    cnt = dsm_pullup(&(item_ptr), &(scb_ptr->sock_err),
                     sizeof(struct ps_sock_extended_err));
    if(cnt != sizeof(struct ps_sock_extended_err)) goto bail;

    cnt = dsm_pullup(&(item_ptr), &(scb_ptr->v6_offender),
                     sizeof(struct ps_in6_addr));
    if(cnt != sizeof(struct ps_in6_addr)) goto bail;

    payload_len = (uint16)dsm_length_packet(item_ptr);

    if(ucb_ptr->err_pktcnt == 0)
    {
      scb_ptr->error_available = FALSE;
    }
  }
  LOG_MSG_INFO1_1("packet extracted %d",payload_len);
  dssocki_fill_sockaddr(fromaddr,addrlen,src_addr,scb_ptr->family);

  remaining_len = MIN(payload_len, DSS_READ_MAX_BYTES);

  for(i=0; i < iovcount && remaining_len != 0; i++)
  {
    /*-----------------------------------------------------------------------
      Extract the number of bytes which the application wants to read.
    -----------------------------------------------------------------------*/
    bytes_requested = MIN(remaining_len, iov[i].ps_iov_len);
    if(bytes_requested > 0)
    {
      read_cnt = dsm_pullup( &(item_ptr),
                             iov[i].ps_iov_base,
                             bytes_requested );

      if(read_cnt != bytes_requested) goto bail;

      remaining_len   -= read_cnt;
      payload_len     -= read_cnt;
      bytes_read      += read_cnt;
    }
  }

  /*-------------------------------------------------------------------------
    If the supplied buffer is not large enough to hold the datagram, discard
    the remainder of the datagram and set dss_errno.
  -------------------------------------------------------------------------*/
  LOG_MSG_INFO1_1("packet extracted %d",payload_len);
  if (payload_len > 0)
  {
    dsm_free_packet(&(item_ptr));

    LOG_MSG_INFO1_2("Truncating %d bytes, sockfd=%d, supplied buffer too small",
              remaining_len, scb_ptr->sockfd);
    *dss_errno = DS_EMSGTRUNC;
  }

  /*-------------------------------------------------------------------------
    Check if there are any remaining UDP packets in the receive queue. Set
    the data_available flag to FALSE, if there are no remaining UDP packets.
    Protect the access to the queue through TASKLOCK()/TASKFREE().
  -------------------------------------------------------------------------*/
  LOG_MSG_INFO1_1("recv pkt count %d",ucb_ptr->rcv_pktcnt);

  if ( ucb_ptr->rcv_pktcnt == 0 )
  {
    scb_ptr->data_available = FALSE;
  }

  if(ucb_ptr->err_pktcnt != 0)
  {
    scb_ptr->error_available = TRUE;
  }
  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  /*-------------------------------------------------------------------------
    Return the number of bytes read from the buffer.
  -------------------------------------------------------------------------*/
  return ( (sint15) bytes_read);

bail:
  {
    dsm_free_packet(&item_ptr);
    return DSS_ERROR;
   }

} /* dssudp_read() */

/*===========================================================================
FUNCTION DSSUDP_READ_DSM_CHAIN()

DESCRIPTION
  Reads 'nbytes' bytes in the buffer from the UDP transport.  Fills in
  address structure with values from who sent the data in fromaddr.
  This function assumes that fromaddr is not NULL.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes to be written, which could be less than the number
      of bytes specified.

  On error, return DSS_ERROR and places the error condition value in *errno.

  Errno Values
  ------------
  DS_EWOULDBLOCK      operation would block

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dssudp_read_dsm_chain
(
  struct scb_s* scb_ptr,     /* Ptr to socket control block for the socket */
  dsm_item_type** msg_ptr,
  struct ps_sockaddr *fromaddr,                        /* destination address */
  uint16 * addrlen,                                      /* address length */
  sint15 *dss_errno                                   /* error condition value */
)
{
  struct udp_cb *ucb_ptr;                    /* UDP protocol control block */
  uint16 payload_len;                             /* Length of UDP payload */
  uint32 iface_id;                              /* id for IP_RECVIF option */
  uint16 cnt=0;                                /* tmp # of bytes retrieved */
  struct socket src_addr;                  /* source address of the packet */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Ensure that the dsm item is not NULL.
  -------------------------------------------------------------------------*/
  if( msg_ptr == NULL )
  {
    LOG_MSG_ERROR_0("NULL dsm_item_passed");
    DS_ASSERT( 0 );
    *dss_errno = DS_EFAULT;
    return DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    Set the socket and UDP control block pointers, and set the family to
    AF_INET.
   ------------------------------------------------------------------------*/
  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  ucb_ptr = scb_ptr->protocol_ctl_blk.ucb;

  /*-------------------------------------------------------------------------
    Check if there is anything in the UDP receive queue. If not, return
    DS_EWOULDBLOCK.
  -------------------------------------------------------------------------*/
  if ( ucb_ptr->rcv_pktcnt == 0 )
  {
    scb_ptr->data_available = FALSE;
    DS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    *dss_errno = DS_EWOULDBLOCK;
    return (DSS_ERROR);
  }

  *msg_ptr = dsm_dequeue(&(ucb_ptr->rcvq_wm));
  DS_ASSERT(*msg_ptr != NULL);

  ucb_ptr->rcv_pktcnt --;

  /*-------------------------------------------------------------------------
    If IP_RECVIF option is set record iface pkt was received on.
  -------------------------------------------------------------------------*/
  if((*msg_ptr)->kind == 1)
  {
    cnt = dsm_pullup(msg_ptr, &iface_id, sizeof(iface_id));
    if( sizeof(iface_id) != cnt )
    {
      LOG_MSG_ERROR_0("DSM Pullup error");
      dsm_free_packet(msg_ptr);
      return (DSS_ERROR);
    }

    if(IS_SOCKOPT_SET(ucb_ptr->ipcb.options_mask, (uint64)DSS_IP_RECVIF))
    {
      scb_ptr->rx_iface_id = iface_id;
    }
    else
    {
      scb_ptr->rx_iface_id = 0;
    }
  }

  /*-------------------------------------------------------------------------
    Extract the payload length, server's IP address, and port number
    and store in fromaddr.  ASSERT that bytes to read, are equal to the
    number of bytes pulled up.
  -------------------------------------------------------------------------*/
  cnt = dsm_pullup(msg_ptr, &payload_len, sizeof(payload_len));
  if(cnt != sizeof(payload_len)) goto bail;

  cnt = dsm_pullup(msg_ptr, &(src_addr.port), sizeof(src_addr.port));
  if(cnt != sizeof(src_addr.port)) goto bail;

  cnt = dsm_pullup(msg_ptr, &(src_addr.address), sizeof(src_addr.address));
  if(cnt != sizeof(src_addr.address)) goto bail;

  dssocki_fill_sockaddr(fromaddr,addrlen,src_addr,scb_ptr->family);

  /*-------------------------------------------------------------------------
    Extract the number of bytes which the application wants to read.
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    Check if there are any remaining UDP packets in the receive queue. Set
    the readable flag to FALSE, if there are no remaining UDP packets.
    Access to global SCB array item is protected through
    TASKLOCK()/TASKFREE().
  -------------------------------------------------------------------------*/
  if ( ucb_ptr->rcv_pktcnt == 0 )
  {
    scb_ptr->data_available = FALSE;
  }

  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  /*-------------------------------------------------------------------------
    Return the number of bytes read from the buffer.
  -------------------------------------------------------------------------*/
  return (int16)(payload_len);
  bail:
  {
    dsm_free_packet(msg_ptr);
    return DSS_ERROR;
  }

} /* dssudp_read_dsm_chain() */

/*===========================================================================
FUNCTION DSSUDP_SHUTDOWN()

DESCRIPTION
  This function implements the half-close functionality for udp sockets.

DEPENDENCIES
  None.

PARAMETERS
  scb_ptr - pointer to scb which needs to be shut down
  item_ptr - a dsm item containing information about what kind of shutdown
             needs to be done (read, write or both)
  dss_errno - error number returned in case of error

RETURN VALUE
  On success, returns DSS_SUCCESS otherwise returns DSS_ERROR and places
  the error number in dss_errno.

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dssudp_shutdown
(
  protocol_ctl_block_type  pcb,   /* Protocol control block for the socket */
  int  how,                                     /* what action to perform  */
  sint15* dss_errno                                    /* dss error number */
)
{
  struct udp_cb *ucb_ptr;
  ps_event_info_type info;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( dss_errno == NULL )
  {
    LOG_MSG_ERROR_0("NULL dss_errno passed" );
    return DSS_ERROR;
  }

  *dss_errno = DSS_SUCCESS;

  ucb_ptr = (struct udp_cb *) pcb.ucb;

  /*-----------------------------------------------------------------------
    If the read-half needs to be shut down, clean-up the receive and the
    resequencing queues. Note that shut down of read-half does not
    result in any communication with the peer.
  -----------------------------------------------------------------------*/
  if ( how == DSS_SHUT_RD || how == DSS_SHUT_RDWR  )
  {
    ucb_ptr->cant_recv_more = 1;
    /*---------------------------------------------------------------------
      Free the receive queue for the UDP control block.
    ---------------------------------------------------------------------*/
    DS_ENTER_CRIT_SECTION(&global_ps_crit_section);

    dsm_empty_queue(&(ucb_ptr->rcvq_wm));

    DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  }
  /*-----------------------------------------------------------------------
    Inform the application of the socket event that the read-half/
    write-half is closed.
  -----------------------------------------------------------------------*/
  info.id     = PS_SOCKET_EVENT;
  info.sockfd = ucb_ptr->sockfd;

  ucb_ptr->info_change_upcall(ucb_ptr->scb_ptr, &info);

  return (DSS_SUCCESS);

} /* dssudp_shutdown() */

/*===========================================================================
FUNCTION DSSUDP_CLOSE()

DESCRIPTION
  This function is the UDP specific close() function.  It simply sets the
  socket state to DSSOCKI_NULL and frees up the socket control block and
  related data structures.

DEPENDENCIES
  None.

RETURN VALUE
  On success, returns DSS_SUCCESS.

SIDE EFFECTS
  Socket state is set to NULL.  Socket is freed and made available for
  re-use.
===========================================================================*/
/**/
sint15 dssudp_close
(
  protocol_ctl_block_type  pcb,   /* Protocol control block for the socket */
  boolean silent_close_flag,                          /* silent close flag */
  sint15 *dss_errno                                   /* error condition value */
)
{
  struct udp_cb *ucb_ptr;                    /* UDP protocol control block */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( dss_errno == NULL )
  {
    LOG_MSG_ERROR_0( "NULL errno passed" );
    return DSS_ERROR;
  }
  *dss_errno = DSS_SUCCESS;

  ucb_ptr = (struct udp_cb *) pcb.ucb;

  /*-------------------------------------------------------------------------
    Free the receive queue, ucb and free the scb
  -------------------------------------------------------------------------*/
  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);
  dsm_empty_queue(&(ucb_ptr->rcvq_wm));
  dsm_empty_queue(&(ucb_ptr->errq_wm));
  dssudpi_free_ucb(ucb_ptr);
  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  return (DSS_SUCCESS);

} /* dssudp_close() */

/*===========================================================================
FUNCTION DSSUDP_BIND()

DESCRIPTION
  This is the UDP specific bind() function. It writes the port value into
  local.conn.port of the socket control block structure.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dssudp_bind
(
  protocol_ctl_block_type  pcb,   /* Protocol control block for the socket */
  struct ps_sockaddr_in6 *la /* Ptr to the structure contains the local addr */
)
{
  struct udp_cb * ucb_ptr = pcb.ucb;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Set the conn.local.port value to the port number passed in. For a NULL
    sockaddr 0's are automatically passed for address/port information from
    the sockets layer.
  -------------------------------------------------------------------------*/
  LOG_MSG_INFO2_1("Called UDP BIND func with port %d", la->ps_sin6_port);
  ucb_ptr->conn.local.port = la->ps_sin6_port;
  ucb_ptr->conn.local.address = la->ps_sin6_addr;
  ucb_ptr->ipcb.src_ip_addr   = la->ps_sin6_addr;

}

/*===========================================================================
FUNCTION DSSUDP_REGISTER_UPCALL()

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dssudp_register_upcall
(
  struct scb_s *scb_ptr,
  ps_upcall_fcn_ptr upcall_fcn
)
{
  struct udp_cb *ucb_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (scb_ptr == NULL)
  {
    return;
  }

  ucb_ptr = scb_ptr->protocol_ctl_blk.ucb;
  if (ucb_ptr == NULL)
  {
    return;
  }

  ucb_ptr->info_change_upcall = upcall_fcn;
  return;
}

/*===========================================================================
FUNCTION DSSUDP_SETOPT()

DESCRIPTION
  This function sets the option to the user specified value.

DEPENDENCIES
  None.

PARAMETERS
  ucb_ptr - pointer to udp_cb

RETURN VALUE
  NONE.

SIDE EFFECTS
  None.
===========================================================================*/
void dssudp_setopt
(
  protocol_ctl_block_type  pcb,   /* Protocol control block for the socket */
  int        level,                                        /* option level */
  int        optname,                                       /* option name */
  void     * optval,                                /* value of the option */
  int        optlen                               /* length of option data */
)
{

  struct udp_cb   *ucb_ptr = pcb.ucb;
  boolean         option_processed = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (optname)
  {

    case DSS_SO_RCVBUF:
      ucb_ptr->rcvbuf = *((unsigned int *) optval);
      /* Set the DSM DNE level as per received value. */
      dsm_set_dne(&(ucb_ptr->rcvq_wm), ucb_ptr->rcvbuf);
      option_processed = TRUE;
      break;
    default:
      LOG_MSG_INFO2_1( "UDP Setopt called for socket option %d", optname );

  } /* end switch() */

  /*-------------------------------------------------------------------------
    If option is not yet processed and it is an IP level option, call
    IP function to process the option.
  -------------------------------------------------------------------------*/
  if((level == (int)DSS_IPPROTO_IP || level == (int)DSS_IPPROTO_IPV6) &&
     option_processed == FALSE)
  {
    ip_setopt(&ucb_ptr->ipcb, level, optname, optval, optlen);
    return;
  }
} /* dssudp_setopt() */


