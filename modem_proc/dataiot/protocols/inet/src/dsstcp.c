/*===========================================================================

        D A T A  S E R V I C E S  S O C K E T  T C P   F I L E

DESCRIPTION

 The Data Services sockets TCP interface file. This contains all the
 functions used by the sockets library to access TCP socket function calls.

EXTERNALIZED FUNCTIONS
  dsstcp_socket()
    TCP specific socket() function.  Sets socket state to INIT.

  dsstcp_connect()
    Connects a TCP socket.

  dsstcp_listen()
    Does the passive open for listener socket child sockets.

  dsstcp_accept()
    Removes and return the a child socket from a listener sockets
    ready queue. This will also try to refill the half open
    connections queue.

  dsstcp_write()
    TCP specific write() function.  Sends the dsm memory item out
    the TCP transport.

  dsstcp_read()
    TCP specific read() function.  Reads specified number of bytes from
    the TCP transport.

  dsstcp_read_dsm_chain()
    TCP specific read_dsm_chain() function.  Reads a DSM item chain from
    the TCP transport.

  dsstcp_close()
    TCP specific close() function.  Initiates the TCP active close.
    If the connection is already closed, cleans up the TCP socket and
    associated resources.

  dsstcp_bind()
    This is the TCP specific bind() function.  It writes the port number into
    the local.conn.port of the socket control block structure.

  dsstcp_setopt
    This function sets the tcp option value to the user specified value.

  dsstcp_register_upcall
    This function registers upcall function and user data with the TCB.

  dsstcp_flow_enabled_ind
    This function calls tcp_output() to send data out once socket is
    flow enabled

Copyright (c) 1998-2019 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsstcp.c_v   1.9   13 Feb 2003 12:20:46   ssinghai  $
  $Header: //components/rel/dataiot.mpss/2.0/protocols/inet/src/dsstcp.c#1 $ $DateTime: 2020/03/17 08:03:16 $ $Author: pwbldsvc $


when        who    what, where, why
--------    ---    ----------------------------------------------------------
12/09/19    sr     Added support for dsstcp_get_conn_error
10/10/19    ds     Added API dsstcp_is_writeable() for TCP congestion control.
10/12/18    ds     Code changes to reset the meta info ptr within TCP control
                   block.
10/12/18    ds     Fixed the KW errors.
08/13/18    ds     Code changes as per new FWK design for IOT.
01/20/15    ss     Add IP family check before picking up TCP server socket
01/02/13    ssh    Local TCP connections should use IP layer-supplied MTU
10/01/10    sa     Added appropriate log messages before ASSERT(0).
06/15/10    vm     IDLE event related fixes
05/22/09    kk     Q6 compiler warning fixes.
03/17/03    mct    Ensure we clear the source ip addr when our ip changes.
11/08/07    hm     Fixed a bug in TCP RTT computation
03/16/07    scb    Fixed Klocwork High errors
02/07/07    scb    Fixed Klocworks high errors
02/05/07    rt     Removed EIFEL code.
12/05/06    msr    Passing sockfd in ps_event_info_type
11/22/06    sv     Added support for IPSEC transport mode support.
11/03/06    hm     Checking the return value of dssocki_send_to_ps. Using
                   dssocki_alloc_ps_cmd_buffer and dssocki_send_ps_cmd_buffer
                   instead of dssocki_send_to_ps in many places.
11/02/06    mct    Updates for allowing binding to local IPv6 addresses.
10/19/06    msr    Freeing TCB's rcvq in dsstcp_close() as well
10/13/06    msr    Handling APP_TO_PS_SOCKET_FLOW_ENABLED_IND
10/13/06    hm     RTT related code cleanup
09/27/06    rt     Setting data_available flag as FALSE if tcb rcvcnt is 0
                   in dsstcp_read functions.
08/02/06    ks     Merded Fix for RCVBUF casting issue in open_tcp call.
07/31/06    rt     Route Scope modifications.
06/13/06    msr    Making sure that timers are non-NULL before manipulating
                   them. Async TCB is not creating all the timers
04/24/06    mct    Drop TCP packets if routing_cache is null.
03/27/06    rt     L4 TASKLOCK review changes.
03/13/06    msr    Fixed bug in dsstcp_accept() where metainfo is not freed
03/13/06    msr    Fixed bug in dsstcp_bind() where metainfo is not freed
02/22/06    rt     Using single critical section
02/07/06    rt     L4 TASKLOCK changes.
01/31/05    rt     Added new F3 message in BIND function.
12/03/05    rt     Added code for new tcp option DSS_TCP_EIFEL.
12/03/05    rt     Added a new scb_ptr field in dssocki_send_to_ps() fn and
                   got rid of pcb and protocol fields.
09/26/05    msr    Fixed a compilation warning
08/30/05    vp     Check for backlog before generating meta info.
08/16/05    vp     Cleanup for propagating sys sock info to ipcb.
08/15/05    vp     Propagating sys sock info to ipcb.
08/15/05    kr     Added support for stats update
08/15/05    vp     Addition of flags argument to dsstcp_read(). Resetting
                   err_available flag when so_error is returned.
08/05/05    sv     Update metainfo when we get a new metainfo ptr in write.
06/13/05    vp     Support for SO_ERROR and SO_ERROR_SNABLE socket options.
04/22/05    vp     Send the tcb localname to open_tcp in dsstcp_listen().
04/18/05    vp     Changes for enforcing netpolicy on input.
04/17/05    msr    Using new signature of dssocki_generate_meta_info().
04/16/05    ks     Not checking Iface state any more, during dormant close.
04/04/05    rt     Fixed the problem of backlogs not freed if client sends a
                   reset after receiving the SYN/ACK from server.
03/21/05    ssh    Changes pursuant to the new typedef ps_timer_handle_type
03/01/05    vp     Changes due to change in signature of dssocki_fill_sockaddr
02/22/05    ssh    Replaced INTLOCK()s with TASKLOCK()s
02/15/05    msr    Calling ip_setopt() for IPV6 level options.
01/17/05    ks     removed call to PS_META_INFO_FREE macro from dsstcp_connect
01/10/05    sv     Merged IPSEC changes.
12/20/04    msr    Fixed dsstcpi_persist_timer_cb() to use
                   numb_bytes_in_transit instead of snd_ptr.
12/09/04    msr    Replaced ASSERTS in dsstcp_lookup_serv() with if check.
11/24/04    msr    memsetting tcb to 0 after it is freed in
                   dsstcpi_free_tcb().
10/25/04    msr    Added support for persist timer and added
                   dsstcpi_persist_timer_cb()
10/11/04    sv     Fixed dsstcpi_free_tcb to free SACK BLOCKS.
10/13/04    vp     Changes in dsstcpi_send_keepalive probe() for port byte
                   order and addition of tcp_hdr_create() instead of htontcp().
08/16/04    ks/msr Integrated TCBs in to ps_mem memory allocation. Maintaining
                   allocated TCBs in a list instead of array. Removed
                   dsstcp_init_tcb_array function.
08/30/04    sv     Moved graceful dormant close flag and keep alive idle time
                   constants to socket config control block.
08/19/04    vp     Changes for freeing up the tcb's only in PS context in
                   dsstcp_listen_setup().
08/19/04    vp     Set the local address and port in dsstcp_bind only when
                   tcb_ptr->sockfd != 0.
06/21/04    sv     Modified dsstcp_connect to set the user specified flow label
                   in IPCB.
06/11/04    vp     Removal of inclusion of psglobal.h, netuser.h ps_tcp.h.
                   Included files dssocket_defs.h, ps_tcp.h and ps_socket_defs,h
                   Changes for representation of IP addresses in struct ps_in_addr
                   or struct ps_in6_addr. Replaced use of TCP_PTCL with PS_IPPROTO_TCP
05/17/04    sv     Modified dsstcp_read to read a maximum of DSS_READ_MAX_BYTES
                   in a single read call.
05/14/04    sv     Updated init_tcb to reset the sndcnt and rcvcnt of TCB.
04/30/04    sv     Added TCP timestamp and SACK option. Changed the tcp
                   options type from uint32 to struct tcp_options_type.
04/28/04    mct    Changed max # of TCBs to DSS_MAX_TCP_SOCKS + 10.
04/26/04    vp     Removed unnecessary local variable if_v6addr in
                   dsstcp_connect
04/22/04    vp     Removal of the access of partial and established queues of
                   the scb from state_change_upcall.
04/14/04    vp     Changes for KEEPALIVE and DELAYED ACK as socket options.
03/30/04    vp     Merged changes from June04Dev_05.00.00 PS branch.
03/18/04    vp     Changes for removing direct scb access from PS context.
                   All such events are posted through info_change_upcall.
03/10/04    vp     Changes for dssocki_socket_event_occurred() to be executed
                   through upcall mechanism.
02/26/04    vp     Changed the name of setstate and close_self to tcp_setstate
                   and tcp_close_self
02/26/04    vp     Cleanup of dsstcpi_init_tcb()
02/04/04    vp     Replaced use of PS_IFACE_IP_V4_ADDR with ps_iface_get_addr
01/29/04    mct    Replaced DSS_LONG_CLOSE_INTERVAL with TCP_TIME_WAIT_INTERVAL
01/28/04    vp     Modified the if condition for abort during dormancy in
                   dsstcp_close
01/28/04    sv     Increased the initial TCP congestion window to 2*mss
01/27/04    vp     Modified dsstcp_is_writeable to enqueue minimum of SNDBUF
                   and Congestion window into sndq before PS task runs.
                   Modified dsstcpi_data_tx_upcall to update the cached cwind
                   in scb.
01/27/04    vp     Added definition of tcp_graceful_dormant_close flag.
                   Added check to close the dormant connection gracefully if
                   tcp_graceful_dormant_close flag is TRUE.
01/26/04    mct    Added additional check in read, read_dsm_chain, write,
                   shutdown, and setopt to check if tcb_ptr->sockfd == 0.
01/20/04    mct    Modified dsstcp_close() to return success if tcb->sockfd
                   is already equal to zero.
01/19/04    sv     Modified alloc_listener to get sock_cb from the scb
                   instead of acb.
01/14/04    vp     Replaced the call to reset_tcp with a call to close_self
                   in case of ip address change in dsstcp_new_ip_upcall()
01/07/04    vp     Modified dsstcpi_half_open_timer_cb to remove scb access
                   from ps context and use dsstcp_reset_tcb() instead.
12/02/03    sv     Changed tcp close reason to NETWORK if we abort the
                   connection because of network failure/down.
11/12/03    sv     Trigger socket event when we receive a fin from the peer
11/12/03    sv     Added check to make sure we don't enqueue more than
                   sndbuf size of data in tcp sndq.
10/20/03    sv     Modified the code in state_change_upcall to trigger the
                   socket closed event in TIME_WAIT state.
10/17/03    sv     Modified protocol specific functions to take PCB instead of
                   SCB.
10/01/03    sv     Modified dsstcp_close so that TCB is freed in PS context
                   and SCB is freed in application context. Removed tcp_abort.
09/24/03    sv     Changed bytes_available variable in dsstcp_read to uint16
                   to fix the larger window size problem.
09/09/03    sv     Added dsstcp_reset function to reset tcb. Changed
                   create_tcb to accept SCB as an argument.
08/18/03    sv     Modified the code to use IPv6 addresses internally.
08/08/03    sv     Fixed the duplicate syn problem.
08/08/03    ss/sv  Restructured the socket code.
06/27/03    sv     Modified dsstcp_is_writeable to use sndbuf in scb to
                   determine the number of bytes available in sndq.
06/16/03    ss     Modified dsstcp_close() to return immediately if the
                   socket state is null, wait for close or closed.
06/04/03    om     Fixed MSS setting to adjust for IPSec ESP overhead.
05/28/03    ss     Removed SNDQ_LIMIT from dsstcp_is_writeable()
05/13/03    ss     Modified code to insert the scb at the end of the estab
                   queue instead of at front for SYN_RECD->EST transition
05/04/03    ss     Modified state_change_upcall() to support SO_LINGER option
04/24/03    usb    Fixed bug in tcp_read where bytes read were not
                   getting updated correctly.
04/23/03    ss     Modified code to reflect new socket options handling
04/15/03    sv     Modify dsstcp_read to accept iov and iovcount instead of
                   buffer and length. Added support for tcp half open timer.
04/15/03    ss     Modified dsstcp_close() to transition socket state to
                   CLOSING only if the socket state is OPEN after close_tcp()
04/10/03    ss     Modified dsstcp_connect() to change the parameter to
                   getephem_port(). Modified dsstcp_shutdown() to correctly
                   test the value of how parameter.
03/07/03    js     Added event reporting for TCP.
02/13/03    ss     Added code to support socket shutdown
02/12/03    om     Use SCB's routing cache only.
12/22/02    aku    Changed dsstcp_new_ip_upcall() signature to take
                   ps_iface_ptr as argument instead of IP addresses.
11/19/02    mvl    removed JCDMA from window size socket option.
11/12/02    ss     Modified dsstcp_abort() so that the clean-up is done when
                   the socket is CLOSING also.
11/05/02    mvl    Added support for changing default window size.
10/30/02    ss     Moved the ASSERT() later in the code in dsstcp_close()
                   since tcb_ptr may be NULL for listener socket
10/17/02    ss     When in SynSent state, dsstcp_close() closes the socket
                   right away. data_available flag is reset when TCB is freed
                   so that the user cannot read data from a socket after TCP
                   reset.
10/08/02    aku    Removed FEATURE_DATA_MM featurization.
09/25/02    om     Added setting of routing cache for LISTEN sockets.
09/19/02    na     Fixed returning of EEOF when in CLOSE_WAIT state.
09/10/02    aku    Removed global Ip_addr if FEATURE_DATA_MM is defined.
08/16/02    ss     Modified dsstcp_close() to support silent close option
08/04/02    aku    Merged in changes for multimode ps under FEATURE_DATA_MM
07/19/02    ss     Modified dsstcp_abort() to call dsstcp_close() in all the
                   states except OPEN and OPENING for necessary clean-up.
07/16/02    aku    Fixed the dsm kind names.
05/02/02    mvl    File clean up, added meta_info_ptr to write()
04/17/02    rc     Fixed bug in dsstcp_close() where INTFREE_SAV() was not
                   being called before returning from the function.
03/13/02    pjb    alloc_tcb was leaving tcb in funny state when timer
                   alloc failed.
03/05/02    pjb    Finished Accept Listen Code Review Items
02/22/02    pjb    Added accept and listen
02/11/02    ss     Added dsstcpi_data_tx_upcall() to notify application when
                   sndq can accept more data. Added dsstcp_is_writeable(),
                   accessed via fcn_ptr_table, to find out if sndq has space.
02/01/02    ss     The user now gets an EOF in case of passive close when
                   there is no more data to read.
12/18/01    rsl    Updated tracer field.
12/07/01    ss     Modified dsstcpi_state_change_upcall() not to call
                   close_tcp() when transitioning to CLOSED_WAIT state
12/07/01    ss     Modified dsstcp_abort() to not to free up scb
11/15/01    aku    dssocki_socket_event_occurred() takes an additonal
                   acb_ptr arg
11/09/01    ss     Changed dsstcpi_state_change_upcall() so that close_timer
                   is started only in FINWAIT2 and TIME_WAIT states. Modified
                   dsstcp_close() to perform TCP close in the PS context.
10/11/01    mt     Add dss_errno argument to dsstcp_connect_setup() and set
                   its contents accordingly.
07/23/01    pjb    Change location/prefix of command names.
05/18/01    yll    Removed an unnecessary assert statement in
                   dsstcp_read_dsm_chain().
04/16/01    js     Added a checking for TCP socket state in
                   dsstcp_new_ip_upcall() to avoid resetting the socket in
                   inappropriate states.
03/27/01    mt     Changed type of second argument in dsstcp_read() from
                   byte * to void *.
03/27/01    js     Added extra parameter to function dsstcp_write(). The
                   extra parameter holds a pointer to the structure holding
                   the destination information. Though not used, the addition
                   is necessary to comply with dss_fcn_ptr_table format.
02/08/01    yll    Added dsstcp_read_dsm_chain().
11/10/00    snn    Modified dword to uint32 and word to uint16.
                   Modified some comments.
                   Renamed callback functions with a postscript of _cb.
                   Removed errno as the parameter for timer functions,
                     because they are never used.

10/10/00    na     Added support for Multiple TCP sockets. Added include
                   files. Removed dsstcp_tcb_ptr because the TCB is now
                   accessed as an array.
                   Added a new array dsstcpi_arr.
08/25/00    rc     Changed dsm_new_buffer() calls to take pool id as input
                   parameter. So, a new macro DSM_DS_POOL_SIZE is used to
                   return pool id based on pool size.
08/03/00    mvl/js Added function dsstcp_bind() to support the protocol
                   specific assignment of local.conn.port.
03/22/00    rc/na  Added function dsstcp_new_ip_upcall() for the case when
                   ppp resyncs and the IP address changed. In this case, the
                   tcp connection is reset.
11/04/99    rc     Modified function dsstcp_close() to close TCP and clean up
                   if socket is already closed.
09/22/99    rc     Incorporated code review changes and comments.
08/19/99    ak     Removed ARM2.5 compiler warnings.
08/11/99    ak/rc  Added new_ip_upcall, for the case where we do ppp
                   re-negotiation and get a new IP.  Affects TCP esp.
                   in dormant mode re-originations.  Code for aborting
                   a TCP socket.
04/25/99    hcg    Added ASSERT to dsstcp_read() to guard against NULL tcp
                   receive queue.
04/05/99    hcg    Incorporated code review changes and comments.  Changed
                   some of the diagnostic message levels.
12/09/98    na/hcg Created module.
===========================================================================*/

/*===========================================================================

                       INCLUDE FILES FOR THE MODULE

===========================================================================*/
#include "comdef.h"
#include "target.h"
#include "customer.h"

#include "amssassert.h"
#include "ps_tcp.h"
#include "ps.h"
#include "dsstcp.h"
#include "dsm.h"
#include "err.h"
#include "msg.h"
#include "memory.h"
#include "ps_utils.h"
#include "ds_crit_sect.h"
#include "ps_pcb_close_reason_codes.h"
#include "dssi.h"
#include "ps_socket_defs.h"
#include "dsstcpi.h"
#include "ps_mem.h"
#include "ps_tx_meta_info.h"
#include "ps_rt_meta_info.h"
#include "ps_metai_info.h"
#include "ds_Utils_DebugMsg.h"

#include "ps_in.h"
#include "ps_in_alias.h"
#include "ds_fwk.h"
#include "dss_config.h"


/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION DSSTCP_SOCKET()

DESCRIPTION
  This function is the protocol-specific socket function.  For TCP, it
  just sets the socket state to INIT.

  This function is called from the context of the application task.

DEPENDENCIES
  None

RETURN VALUE
  On success, returns DSS_SUCCESS
  On error, return DSS_ERROR and places the error condition value in *errno.

  Errno Values
  ------------
  DS_EPROTOTYPE       specified protocol invalid for socket type
  DS_EMFILE           too many descriptors open.  A socket is already open or
                      has not closed compeletely.

SIDE EFFECTS
  None.

===========================================================================*/
sint15 dsstcp_socket
(
  struct scb_s* scb_ptr,     /* Ptr to socket control block for the socket */
  sint15 *dss_errno                                   /* error condition value */
)
{

  struct tcb *tcb_ptr;                           /* TCP  control block ptr */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check arguments.
  -------------------------------------------------------------------------*/
  if (scb_ptr == NULL)
  {
    *dss_errno = DS_EFAULT;
    return (DSS_ERROR);
  }
  /*-------------------------------------------------------------------------
    Check to see that protocol matches socket type.
  -------------------------------------------------------------------------*/
  if ( (scb_ptr->protocol != (byte)PS_IPPROTO_TCP) && (scb_ptr->protocol != 0) )
  {
    *dss_errno = DS_EPROTOTYPE;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Create TCP control block.
  -------------------------------------------------------------------------*/
  if ( (tcb_ptr = dsstcpi_create_tcb(scb_ptr)) == NULL)
  {
    LOG_MSG_ERROR_0( " Could not create a TCB");
    *dss_errno = DS_EMFILE;
    return (DSS_ERROR);
  }
  scb_ptr->protocol_ctl_blk.tcb = tcb_ptr;

  /*Update the IP family in TCB so that it can be fetched during MSS/MTU calculation. */
  tcb_ptr->family = (uint16)scb_ptr->family;

  /*-------------------------------------------------------------------------
    Set socket state to INIT
  -------------------------------------------------------------------------*/
  dssocki_set_sock_state(scb_ptr, DSSOCKI_INIT);

  return (DSS_SUCCESS);

}  /* dsstcp_socket() */

/*===========================================================================
FUNCTION DSSTCP_CONNECT()

DESCRIPTION
  This function connects a TCP socket and should only be called when
  PPP/traffic channel is established.

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
sint15 dsstcp_connect
(
  protocol_ctl_block_type  pcb,                  /* Protocol control block */
  struct ps_sockaddr_in6 *dest_ptr,                /* destination socket addr */
  ps_meta_info_type *meta_info_ptr                     /* meta information */
)
{
  struct tcb              * tcb_ptr = pcb.tcb; /* Ptr to TCP control block */
  uint16                   mtu = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_0(" Connecting socket TCP");

  /*-------------------------------------------------------------------------
    Verify that tcb_ptr is non NULL
  -------------------------------------------------------------------------*/
  if( NULL == tcb_ptr )
  {
    LOG_MSG_ERROR_0("NULL tcb_ptr passed");
    DS_ASSERT( 0 );
    return DSS_ERROR;
  }

  if(meta_info_ptr == NULL)
  {
    LOG_MSG_ERROR_0("meta_info_ptr is NULL in dsstcp_connect");
    DS_ASSERT(0);
    return DSS_ERROR;
  }

  tcb_ptr->conn.remote.address = dest_ptr->ps_sin6_addr;
  tcb_ptr->conn.remote.port    = dest_ptr->ps_sin6_port;

  /*-------------------------------------------------------------------------
    Open an active TCP connection.
  -------------------------------------------------------------------------*/
  LOG_MSG_INFO2_1( "connect() using 0x%p",
           PS_META_GET_ROUTING_CACHE( meta_info_ptr));

  tcb_ptr->ipcb.routing_cache = PS_META_GET_ROUTING_CACHE( meta_info_ptr);
  tcb_ptr->ipcb.meta_info_ptr = meta_info_ptr;
  
  if (tcb_ptr->family == DSS_AF_INET)
  {
    mtu = ps_ip_calc_path_mtu( IFACE_IPV4_ADDR_FAMILY, 
                               tcb_ptr->ipcb.routing_cache ) - IPLEN - TCPLEN;
  }
  else if (tcb_ptr->family == DSS_AF_INET6)
  {
    mtu = ps_ip_calc_path_mtu( IFACE_IPV6_ADDR_FAMILY, 
                               tcb_ptr->ipcb.routing_cache ) - IP6LEN - TCPLEN;
  }

  tcb_ptr->mss = MIN(tcb_ptr->mss, mtu);

  open_tcp(&(tcb_ptr->conn.local),
           &(tcb_ptr->conn.remote),
           (int)TCP_ACTIVE,
           tcb_ptr->rcvbuf,
           (uint16)tcb_ptr->mss,
           0,
           -1,
           tcb_ptr);

  return(DSS_SUCCESS);

} /* dsstcp_connect() */

/*===========================================================================
FUNCTION DSSTCP_LISTEN()

DESCRIPTION
  This starts a passive open on a tcp socket.
  This is called from the context of the protocol stack

DEPENDENCIES
  Before this is called the source port should be put in the socket
  control block with bind.

RETURN VALUE
  Returns success on success, or error on error.  If there
  is an error dss_errno is filled in to indicate the error.

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dsstcp_listen
(
  protocol_ctl_block_type  pcb,                  /* Protocol control block */
  uint16 rcv_window                                 /* receive window size */
)
{
  struct tcb* tcb_ptr;                       /* Ptr. to TCP control block */
  struct socket localname;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2_0(" Listening socket TCP");

  /*-------------------------------------------------------------------------
    Get the TCP control block (TCB).
  -------------------------------------------------------------------------*/
  tcb_ptr = pcb.tcb;
  if( NULL == tcb_ptr )
  {
    LOG_MSG_ERROR_0("NULL tcb_ptr passed");
    return DSS_ERROR;
  }

  localname.port = tcb_ptr->conn.local.port;
  localname.address = tcb_ptr->conn.local.address;

  /*-------------------------------------------------------------------------
    Open an passive TCP connection.
  -------------------------------------------------------------------------*/
  open_tcp(&localname,
           NULL,
           (int)TCP_PASSIVE,
           rcv_window,
           (uint16)tcb_ptr->mss,
           0,
           -1,
           tcb_ptr);

  /*
    We are cloning the socket options set on the listener socket
    tcb_ptr->scb_ptr - will give the listener socket created in 
                       dsstcpi_alloc_listener
    tcb_ptr->scb_ptr->so_head - will give the original socket created by client
  */
  dssocki_clone_socket_options(((scb_type*)tcb_ptr->scb_ptr)->so_head ,
                                (scb_type*)tcb_ptr->scb_ptr);
  return (DSS_SUCCESS);
} /* dsstcp_listen() */

/*===========================================================================
FUNCTION DSSTCP_SET_ROUTE_META_INFO()

DESCRIPTION
  This function processes setting of routing meta info inside IPCB.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsstcp_set_route_meta_info
(
  protocol_ctl_block_type pcb,
  int32                   sockfd,
  ps_rt_meta_info_type *  rt_mi_ptr             /* Routing Meta info */
)
{
  struct tcb            * tcb_ptr = pcb.tcb;
  ps_tx_meta_info_type  * tx_mi_ptr = NULL;        /* Ptr to TX meta info */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (rt_mi_ptr == NULL)
  {
    LOG_MSG_ERROR_0("NULL rt_mi_ptr");
    DS_ASSERT(0);
    return;
  }

  if (NULL == tcb_ptr )
  {
    PS_RT_META_INFO_FREE(&rt_mi_ptr);
    LOG_MSG_ERROR_0( "NULL TCB");
    DS_ASSERT(0);
    return;
  }

  if (tcb_ptr->sockfd != sockfd)
  {
    PS_RT_META_INFO_FREE(&rt_mi_ptr);
    LOG_MSG_ERROR_2("Sockfd %d != TCB's sockfd %d", sockfd, tcb_ptr->sockfd);
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
  if (tcb_ptr->ipcb.meta_info_ptr != NULL)
  {
    PS_TX_META_INFO_FREE(&tcb_ptr->ipcb.meta_info_ptr);
  }

  tcb_ptr->ipcb.meta_info_ptr = tx_mi_ptr;

  ps_ip_update_routing_cache_in_ipcb
  (
    &(tcb_ptr->ipcb),
    (ds_fwk_s_type*)
      PS_TX_META_GET_ROUTING_CACHE(tcb_ptr->ipcb.meta_info_ptr)
  );

  return;
} /* dsstcp_set_route_meta_info() */

void dsstcp_set_linger_reset
(
  protocol_ctl_block_type pcb,
  int32    sockfd,
  boolean  is_linger_on,
  int32    time_in_sec
)
{
  struct tcb * tcb_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  tcb_ptr = pcb.tcb;

  if (NULL == tcb_ptr )
  {
    LOG_MSG_ERROR_0( "NULL TCB");
    DS_ASSERT(0);
    return;
  }

  if (tcb_ptr->sockfd != sockfd)
  {
    LOG_MSG_ERROR_2("Sockfd %d != TCB's sockfd %d", sockfd, tcb_ptr->sockfd);
    return;
  }

  tcb_ptr->is_linger_on = is_linger_on;
  tcb_ptr->time_in_sec  = time_in_sec;

  return;
} /* dsstcp_set_linger_reset() */


/*===========================================================================
FUNCTION DSSTCP_CLEANUP_ALL_RESEQ()

DESCRIPTION
  This function cleans up the resequencing queue of all the TCBs. This
  fucntion should be called to free up memory potentially consumed by the
  segments lying in the resequencing queues.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dsstcp_cleanup_all_reseq()
{
  struct tcb *tcb_ptr;                                   /* Pointer to TCB */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  tcb_ptr = (struct tcb *) q_check(&sock_config_cb.tcp_cb_q);
  while (NULL != tcb_ptr)
  {
    DS_ASSERT(0 != tcb_ptr->sockfd);

    tcp_cleanup_reseq(tcb_ptr);

    tcb_ptr =
      (struct tcb *) q_next(&sock_config_cb.tcp_cb_q, (q_link_type *) tcb_ptr);
  } /* while */

} /* dsstcp_cleanup_all_reseq() */

/*===========================================================================
FUNCTION DSSTCP_CONNECT_SETUP()

DESCRIPTION
  This function sets up specific parameters for TCP sockets, and should
  be called prior to making a call to the TCP connect function.  Specifically
  it sets the appropriate state of the socket, within the context of the
  calling application, so that the correct error responses will be generated
  as needed.
  This function is called from the App task context.

DEPENDENCIES
  None.

RETURN VALUE
  DSS_SUCCESS if connection setup succeeds.
  DSS_ERROR if arguments are invalid of if connection setup fails.

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dsstcp_connect_setup
(
  struct scb_s* scb_ptr,     /* Ptr to socket control block for the socket */
  struct ps_sockaddr_in6* dest_ptr,                    /* destination address */
  sint15 *dss_errno                    /* address of error condition value */
)
{
  ps_tx_meta_info_type * tx_meta_info_ptr;                       /* PS meta info */
  struct socket localname,remotename;
  struct ps_sockaddr_in6 localaddr;
  ps_ip_addr_type     if_v6addr;
  boolean             update_binding = FALSE;
  dssocki_app_to_ps_cmd_type  * ps_bind_cmd;
  dssocki_app_to_ps_cmd_type  * ps_connect_cmd;
  ds_fwk_s_type               * ds_fwk_ptr;
  struct ps_sockaddr_in      v4_addr;
  struct ps_sockaddr_in6 v6_sockaddr;      /* protocol specific dest address */
  boolean                 ipcb_meta_info_valid = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
     Check arguments.
  -------------------------------------------------------------------------*/
  if ( (scb_ptr == NULL) || (dss_errno == NULL) )
  {
    return (DSS_ERROR);
  }

  ps_connect_cmd = dssocki_alloc_ps_cmd_buffer(APP_TO_PS_SOCKET_CONNECT);
  if(NULL == ps_connect_cmd)
  {
    LOG_MSG_ERROR_0("Mem alloc for PS connect command failed");
    *dss_errno = DS_ENOMEM;
    return DSS_ERROR;
  }
  /*-------------------------------------------------------------------------
    If in INIT state, set the state to OPENING
  -------------------------------------------------------------------------*/
  if (scb_ptr->socket_state == DSSOCKI_INIT)
  {
    dssocki_set_sock_state( scb_ptr, DSSOCKI_OPENING);
  }

  DSSOCKI_CACHE_GET_LOCAL(scb_ptr, localname);

  if ((tx_meta_info_ptr = dssocki_generate_meta_info( scb_ptr,
                                                   scb_ptr->flow_ptr,
                                                   0,
                                                   &ipcb_meta_info_valid,
                                                   dss_errno )) == NULL )
  {
    LOG_MSG_ERROR_1( "MetaInfo gen. failed (%d)", *dss_errno);
    PS_MEM_FREE(ps_connect_cmd);
    *dss_errno = DS_ENOMEM;
    return DSS_ERROR;
  }

  remotename.address = dest_ptr->ps_sin6_addr;
  remotename.port    = dest_ptr->ps_sin6_port;
  /*-------------------------------------------------------------------------
    If address not bound to socket, assign it the local IP. Till we support
    binding to a particular interface we keep binding to new local ipaddress
    for each connect call.
  -------------------------------------------------------------------------*/
  if(PS_IN6_IS_ADDR_UNSPECIFIED(&localname.address))
  {

    ds_fwk_ptr = scb_ptr->cached_info.ip.routing_cache;
    
    ds_fwk_get_ip_addr( ds_fwk_ptr,
                        scb_ptr->family,
                        &if_v6addr);

    if(scb_ptr->family == DSS_AF_INET)
    {
      v4_addr.ps_sin_family = DSS_AF_INET;
      v4_addr.ps_sin_addr.ps_s_addr = if_v6addr.addr.v4.ps_s_addr;
      v4_addr.ps_sin_port = localname.port;

/*-------------------------------------------------------------------------
    Convert ps_sockaddr to ps_sockaddr_in6.
-------------------------------------------------------------------------*/
      if( dssocki_get_sockaddr_in6(&v6_sockaddr, (struct ps_sockaddr*)&v4_addr, sizeof(struct ps_sockaddr_in), dss_errno) ==
           DSS_ERROR )
      {
        DS_MSG1(MSG_LEGACY_ERROR, "Invalid Server address: ERRNO %d", *dss_errno);
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
      LOG_MSG_ERROR_0("Mem alloc for PS bind command failed");
      if(!ipcb_meta_info_valid)
      {
        PS_TX_META_INFO_FREE(&tx_meta_info_ptr);
      }
      PS_MEM_FREE(ps_connect_cmd);
      *dss_errno = DS_ENOMEM;
      return DSS_ERROR;
    }

    DSSOCKI_CACHE_SET_LOCAL(scb_ptr, localname);

    dssocki_send_ps_cmd_buffer( scb_ptr,
                                NULL,
                                &localaddr,
                                ps_bind_cmd);
  }

  DSSOCKI_CACHE_SET_REMOTE(scb_ptr, remotename);

  dssocki_send_ps_cmd_buffer( scb_ptr,
                              (void *) tx_meta_info_ptr,
                              dest_ptr,
                              ps_connect_cmd);

  *dss_errno = DS_EWOULDBLOCK;
  return (DSS_ERROR);

} /* dsstcp_connect_setup() */


/*===========================================================================
FUNCTION DSSTCPI_ALLOC_LISTENER()

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
static struct scb_s * dsstcpi_alloc_listener
(
  struct scb_s* scb_ptr
)
{
  struct scb_s* lscb_ptr;       /* Ptr to new scb's as they are allocated */
  struct socket localname;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if((lscb_ptr = dssocki_allocscb((byte)PS_IPPROTO_TCP)) == NULL)
  {
    return NULL;
  }
  lscb_ptr->so_head = scb_ptr;
  /**/
  DSSOCKI_BIND_ACB_TO_SCB(lscb_ptr, scb_ptr->acb_ptr);
  /**/
  lscb_ptr->fcn_ptr_table = scb_ptr->fcn_ptr_table;

  DSSOCKI_CACHE_GET_LOCAL(scb_ptr, localname);
  DSSOCKI_CACHE_SET_LOCAL(lscb_ptr, localname);

  lscb_ptr->family = scb_ptr->family;
  lscb_ptr->type = DSS_SOCK_STREAM;
  lscb_ptr->protocol = (byte)PS_IPPROTO_TCP;
  lscb_ptr->sock_cb = scb_ptr->sock_cb;
  lscb_ptr->sock_cb_user_data = scb_ptr->sock_cb_user_data;
  dssocki_set_sock_state(lscb_ptr, DSSOCKI_INIT);

  /*----------------------------------------------------------------------
    Allocate TCB
  ----------------------------------------------------------------------*/
  if((lscb_ptr->protocol_ctl_blk.tcb = dsstcpi_create_tcb(lscb_ptr)) == NULL)
  {
    dssocki_freescb(lscb_ptr);
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Register the handler to receive information change notifications sent
    by the lower layers
  -------------------------------------------------------------------------*/
  if (lscb_ptr->fcn_ptr_table->register_upcall != NULL)
  {
    lscb_ptr->fcn_ptr_table->register_upcall(lscb_ptr,
                                             dssocki_info_change_handler);
  }
  return lscb_ptr;
} /* dsstcpi_alloc_listener() */


/*===========================================================================
FUNCTION DSSTCP_LISTEN_SETUP()

DESCRIPTION
  This starts a passive open on a tcp socket.
  This is called from the context of the application.

  This allocates TCB and SCB's for each item in the backlog, then
  tells the TCP stack to passively open for each.

DEPENDENCIES
  Before this is called the source port should be put in the socket
  control block with bind.

RETURN VALUE
  Returns success on success, or error on error.  If there
  is an error dss_errno is filled in to indicate the error.

  dss_errno Values
  ----------------
  DS_EINVAL     This indicates that scb_ptr is not set.  This is a
                        violation of the preconditions for calling this
            function.
  DS_ENOMEM     There are not enough socket control blocks or tcp
                        control blocks to fill the back log.

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dsstcp_listen_setup
(
  struct scb_s* scb_ptr,     /* Ptr to socket control block for the socket */
  sint15 backlog,
  sint15 * dss_errno
)
{
  struct ps_sockaddr_in6 src_addr;
  struct scb_s* lscb_ptr;       /* Ptr to new scb's as they are allocated */
  struct scb_s* llscb_ptr;    /* Used for freeing lscb_ptr's on mem failure*/
  struct socket localname;
  dssocki_app_to_ps_cmd_type      *ps_close_cmd;
  dssocki_app_to_ps_cmd_type      *ps_bind_cmd;
  dssocki_app_to_ps_cmd_type      *ps_listen_cmd;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
     Check arguments.
  -------------------------------------------------------------------------*/
  if(NULL == dss_errno)
  {
    LOG_MSG_ERROR_0("dss_errno is null");
    return DSS_ERROR;
  }

  if (NULL == scb_ptr)
  {
    *dss_errno = DS_EINVAL;
    return (DSS_ERROR);
  }

  ps_close_cmd = dssocki_alloc_ps_cmd_buffer(APP_TO_PS_SOCKET_CLOSE);
  if(NULL == ps_close_cmd)
  {
     LOG_MSG_ERROR_0("Alloc failed for PS close command");
     *dss_errno = DS_ENOMEM;
     return DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    If in INIT state, set the state to LISTENing
  -------------------------------------------------------------------------*/
  if (scb_ptr->socket_state == DSSOCKI_INIT)
  {
    dssocki_set_sock_state( scb_ptr, DSSOCKI_LISTEN);
  }

  /*-------------------------------------------------------------------------
    Set QUEUE length
  -------------------------------------------------------------------------*/
  scb_ptr->so_qlimit = backlog;

  /*-------------------------------------------------------------------------
    Release the TCB as this is now a listener socket and doesn't have
    its own tcp context.
  -------------------------------------------------------------------------*/
  dssocki_send_ps_cmd_buffer(scb_ptr, (void*)TRUE, NULL, ps_close_cmd);

  scb_ptr->protocol_ctl_blk.tcb = NULL;

  /*-------------------------------------------------------------------------
    Loop backlog times allocating scb's and tcb's and item_ptrs.
  -------------------------------------------------------------------------*/
  while(backlog > 0)
  {
    if(NULL == (lscb_ptr = dsstcpi_alloc_listener(scb_ptr)))
    {
      break;
    }

    lscb_ptr->so_partial = scb_ptr->so_partial;
    scb_ptr->so_partial = lscb_ptr;

    backlog--;
  }

  /*-------------------------------------------------------------------------
    If backlog is not 0 then a memory allocation failed somewhere. -
    We need to free the resources we just  allocated.
  -------------------------------------------------------------------------*/
  if(backlog != 0)
  {
    while(scb_ptr->so_partial)
    {
      lscb_ptr = scb_ptr->so_partial;
      scb_ptr->so_partial = lscb_ptr->so_partial;
      if(lscb_ptr->protocol_ctl_blk.tcb != NULL)
      {
        dsstcpi_free_tcb(lscb_ptr->protocol_ctl_blk.tcb);
      }
      dssocki_freescb(lscb_ptr);
    }

    *dss_errno = DS_ENOMEM;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Put the scb in an item and send it to the PS task. This ensures that the
    listen happens in the context of the PS task.
  -------------------------------------------------------------------------*/
  lscb_ptr = scb_ptr->so_partial;

  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  while(lscb_ptr != NULL)
  {
    DSSOCKI_CACHE_GET_LOCAL( lscb_ptr, localname );
    src_addr.ps_sin6_family      = (uint16)DSS_AF_INET6;
    src_addr.ps_sin6_addr        = localname.address;
    src_addr.ps_sin6_port        = localname.port;

    ps_bind_cmd = dssocki_alloc_ps_cmd_buffer(APP_TO_PS_SOCKET_BIND);
    ps_listen_cmd = dssocki_alloc_ps_cmd_buffer(APP_TO_PS_SOCKET_LISTEN);
    if(NULL == ps_listen_cmd || NULL == ps_bind_cmd)
    {
      /*--------------------------------------------------------------------
        Free the memeory allocated for listeners and for ps_bind_cmd
      ---------------------------------------------------------------------*/
      while(lscb_ptr != NULL)
      {
        llscb_ptr = lscb_ptr->so_partial;
        if(lscb_ptr->protocol_ctl_blk.tcb != NULL)
        {
          dsstcpi_free_tcb(lscb_ptr->protocol_ctl_blk.tcb);
        }
        dssocki_freescb(lscb_ptr);
        lscb_ptr = llscb_ptr;
      }

      DS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      PS_MEM_FREE(ps_bind_cmd);
      PS_MEM_FREE(ps_listen_cmd); /* just in case check */

      LOG_MSG_ERROR_0("Not enough memory for PS command buffers");
      *dss_errno = DS_ENOMEM;
      return (DSS_ERROR);
    }

    dssocki_send_ps_cmd_buffer( lscb_ptr,
                                NULL,
                                &src_addr,
                                ps_bind_cmd);

    dssocki_send_ps_cmd_buffer( lscb_ptr,
                                (void *)scb_ptr->rcvbuf,
                                NULL,
                                ps_listen_cmd);

    lscb_ptr = lscb_ptr->so_partial;
  }
  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  *dss_errno = DS_EEOF;
  return (DSS_SUCCESS);

} /* dsstcp_listen_setup() */

/*===========================================================================
FUNCTION DSSTCP_ACCEPT()

DESCRIPTION
  This is called from the context of the application to accept a new
  connection.

  Verify that there is a connection waiting, otherwise return
  EWOULDBLOCK.  Take the first scb from
  the queue return it.  Allocate another item for the
  queue.  Return this item to the user.


DEPENDENCIES

  The scb that this is called with should be in the LISTEN state, and
  the ACCEPT_EVENT should have been posted.

RETURN VALUE
  The file descriptor of the new connection is returned on success.

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dsstcp_accept
(
  struct scb_s* scb_ptr,     /* Ptr to socket control block for the socket */
  struct ps_sockaddr* remote_addr,
  uint16 * addrlen,
  sint15 * dss_errno
)
{
  struct scb_s* lscb_ptr = NULL;        /* Ptr to new scb's to be returned */
  struct scb_s** lscb_ptr_ptr;          /* Ptr ptr to hold refill position */
  uint16 backlog;                /* A copy of qlimit (ie. backlog) */
  struct ps_sockaddr_in6 src_addr;
  struct socket localname, servname;
  struct scb_s* tempscb_ptr = NULL;        /* temp pointer to scb. */
  dssocki_app_to_ps_cmd_type  *ps_bind_cmd;
  dssocki_app_to_ps_cmd_type  *ps_listen_cmd;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
     Check arguments.
  -------------------------------------------------------------------------*/
  if (scb_ptr == NULL)
  {
    LOG_MSG_ERROR_0( " SCB pointer is bad");
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
     Try to fill the listen queue again.  First find the # we need to
     allocate.
  -------------------------------------------------------------------------*/
  backlog = (uint16)scb_ptr->so_qlimit;

  /*-------------------------------------------------------------------------
    Now we parse the partial queue to see if there are any scb's with state
    DSSOCKI_OPEN (sockets with established/unaccepted connection in partial
    queue).We delete the first such scb in partial queue and assign it to the
    lscb_ptr which has the socket to be returned by this function. For the
    rest of the scb's with state DSSOCKI_OPEN we just count the number with
    the help of backlog so that we can later allocate that many number of
    listener scb's to be attached to the partial queue so that the total
    number of passive open scb's in partial queue remain equal to backlog.
  -------------------------------------------------------------------------*/
  lscb_ptr_ptr = &(scb_ptr->so_partial);
  while((backlog > 0) && (*lscb_ptr_ptr) != NULL)
  {
    /*-----------------------------------------------------------------------
     If the socket_state is DSSOCKI_CLOSED then remove it from the so_partial
     list and do not decrement backlog value. In the subsequent code backlog
     number of PASSIVE connections are allocated amd sent to the PS task.
    -----------------------------------------------------------------------*/
    if((*lscb_ptr_ptr)->socket_state == DSSOCKI_CLOSED)
    {
      tempscb_ptr = *lscb_ptr_ptr;
      (*lscb_ptr_ptr) = (*lscb_ptr_ptr)->so_partial;
      dssocki_freescb(tempscb_ptr);

      /*---------------------------------------------------------------------
       If we do not have any socket in DSSOCKI_OPEN state then set
       the data_available as FALSE.
      ---------------------------------------------------------------------*/
      if(lscb_ptr == NULL)
      {
        scb_ptr->data_available = FALSE;
      }
      continue;
    }

    if((*lscb_ptr_ptr)->socket_state == DSSOCKI_OPEN)
    {
      if(lscb_ptr == NULL)
      {
        /*-------------------------------------------------------------------
          Found the scb to be returned. Delete this scb from the partial
          queue.  Reset the data_available flag of main listener socket.
        -------------------------------------------------------------------*/
        lscb_ptr = *lscb_ptr_ptr;
        (*lscb_ptr_ptr) = (*lscb_ptr_ptr)->so_partial;
        lscb_ptr->so_partial = NULL;

        scb_ptr->data_available = FALSE;
        /*-------------------------------------------------------------------
          Now since after deleting the scb to be returned from partial queue
          we also need to analyse the scb which replaced it to see whether its
          state is OPEN. So instead of going down we continue with while loop.
        -------------------------------------------------------------------*/
        continue;
      }
      else
      {
        /*-------------------------------------------------------------------
          There is another connection to be accepted so set data_available
          flag. Also this scb is not a part of backlog of listening socket.
        -------------------------------------------------------------------*/
        scb_ptr->data_available = TRUE;
      }
      /*---------------------------------------------------------------------
        Count the number of scb's with established connection.
      ---------------------------------------------------------------------*/
      backlog++;
    }
    lscb_ptr_ptr = &((*lscb_ptr_ptr)->so_partial);
    backlog --;
  }

  DS_ASSERT(backlog <= DSS_SOMAXCONN);

  /*-------------------------------------------------------------------------
    Now we allocate scb's to fill backlog number of passive open connections.
  -------------------------------------------------------------------------*/
  while(backlog > 0)
  {
    if(((*lscb_ptr_ptr) = dsstcpi_alloc_listener(scb_ptr)) != NULL)
    {
      DSSOCKI_CACHE_GET_LOCAL( *lscb_ptr_ptr, localname );
      src_addr.ps_sin6_family = (uint16)DSS_AF_INET6;
      src_addr.ps_sin6_addr   = localname.address;
      src_addr.ps_sin6_port   = localname.port;

      ps_bind_cmd = dssocki_alloc_ps_cmd_buffer(APP_TO_PS_SOCKET_BIND);
      ps_listen_cmd = dssocki_alloc_ps_cmd_buffer(APP_TO_PS_SOCKET_LISTEN);

      if(NULL == ps_bind_cmd || NULL == ps_listen_cmd)
      {
        PS_MEM_FREE(ps_bind_cmd);
        PS_MEM_FREE(ps_listen_cmd); //just in case check
        dssocki_freescb((*lscb_ptr_ptr));
        LOG_MSG_ERROR_0( "Not enough memory for PS commands");
        *dss_errno = DS_ENOMEM;
        return DSS_ERROR;
      }

      dssocki_send_ps_cmd_buffer( *lscb_ptr_ptr,
                                   NULL,
                                   &src_addr,
                                   ps_bind_cmd);

      dssocki_send_ps_cmd_buffer( *lscb_ptr_ptr,
                                  (void *)scb_ptr->rcvbuf,
                                  NULL,
                                  ps_listen_cmd);
    }
    else
    {
      break;
    }
    lscb_ptr_ptr = &((*lscb_ptr_ptr)->so_partial);
    backlog--;
  }

  /*-------------------------------------------------------------------------
     If we have nothing to return: If the partial queue is empty return
     ENOMEM, otherwise return EWOULDBLOCK.
  -------------------------------------------------------------------------*/
  if (lscb_ptr == NULL)
  {
    *dss_errno = (scb_ptr->so_partial == NULL) ? DS_ENOMEM : DS_EWOULDBLOCK;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    ipcb->routing_cache is set in the tcpin code and IP_INFO_CHANGE_UPCALL
    is called to update the cached info of the socket.
    Similarly, the connection information is also set as part of tcpin
    processing and TCP_INFO_CHANGE_UPCALL is called.
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
     If remote_addr/addrlen is set extract that from the tcb and return it
  -------------------------------------------------------------------------*/
  if((remote_addr != NULL) && (addrlen != NULL))
  {
    DSSOCKI_CACHE_GET_REMOTE(lscb_ptr, servname);
    dssocki_fill_sockaddr(remote_addr, addrlen, servname,lscb_ptr->family);
  }

  return lscb_ptr->sockfd;
} /* dsstcp_accept() */

/*===========================================================================
FUNCTION DSSTCP_WRITE()

DESCRIPTION
  Sends dsm memory item out to TCP.
  This function is called from the context of the PS task.

DEPENDENCIES
  None.

RETURN VALUE
  Uses return values from send_tcp().  On error, the function returns -1.
  Otherwise, it returns the number of bytes appended to TCP send queue.

SIDE EFFECTS
  None.

===========================================================================*/
/**/
sint15 dsstcp_write
(
  protocol_ctl_block_type pcb,              /* protocol control block      */
  struct ps_sockaddr_in6 *ta_ptr,              /* destination socket info     */
  dsm_item_type *dsm_item,                  /* ptr to dsm item chain       */
  ps_tx_meta_info_type *meta_info_ptr          /* meta information - ignored  */
)
{
  struct tcb *tcb_ptr;                         /* ptr to TCP control block */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  tcb_ptr = pcb.tcb;

  /*-------------------------------------------------------------------------
    Since the meta infomation is not used, free the item
  -------------------------------------------------------------------------*/
  if(meta_info_ptr != NULL)
  {
    tcb_ptr->ipcb.meta_info_ptr = meta_info_ptr;
    tcb_ptr->ipcb.routing_cache = PS_META_GET_ROUTING_CACHE( meta_info_ptr);
  }

  /*-------------------------------------------------------------------------
    Drop the TCP packet if the iface is NULL. This can occur because there
    are already packets queued to be sent, but the iface down event is
    processed first and clears the routing_cache in the ipcb.
  -------------------------------------------------------------------------*/
  if(tcb_ptr->ipcb.routing_cache == NULL)
  {
    LOG_MSG_ERROR_0("NULL routing_cache/iface has gone down.");
    dsm_free_packet(&dsm_item);
    return DSS_ERROR;
  }

  if(tcb_ptr->sockfd == SOCKAVAIL)
  {
    LOG_MSG_ERROR_0( "Sockfd in TCB is 0!!");
    dsm_free_packet(&dsm_item);
    return DSS_SUCCESS;
  }

  if(dsm_length_packet(dsm_item) == 0 )
  {
    dsm_free_packet(&dsm_item);
    return DSS_SUCCESS;
  }
 
  return (send_tcp(tcb_ptr, dsm_item));
} /* dsstcp_write() */

/*===========================================================================
FUNCTION DSSTCP_READ()

DESCRIPTION
  Reads specified number of bytes into buffer from the TCP transport.

  This function does no sanity checking. The sanity checking should be done
  in dss_read().

  This function is called from the context of the socket application task.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes read, which could be less than the number of
      bytes specified.

  If there are no bytes to be read in the rcvq, the return value is
  DSS_ERROR and DS_EWOULDBLOCK is returned in errno.

SIDE EFFECTS
  None.

===========================================================================*/
/**/
sint15 dsstcp_read
(
  struct scb_s* scb_ptr,     /* Ptr to socket control block for the socket */
  struct ps_iovec  * iov,        /* scatter buffer to which to copy data      */
  uint16 iovcount,               /* length of iovec array                  */
  struct ps_sockaddr *fromaddr,              /* destination address - ignored */
  uint16 * addrlen,                            /* address length - ignored */
  int    flags,                                   /* flags from app        */
  sint15 *dss_errno                                   /* error condition value */
)
{
  struct tcb*   tcb_ptr;       /* Ptr. to TCP control block                */
  uint32        initial_wnd;   /* holds initial Receive Q byte count       */
  sint15        remove_cnt;    /* down counter for removing bytes          */
  sint15        read_cnt=0;    /* # of bytes read from TCP rcvq            */
  uint16        bytes_available;/* # of bytes in the TCP rcvq              */
  sint15        bytes_read = 0; /* total bytes read from TCP rcvq          */
  int           i;              /* local loop variable                     */
  int32         wnd_threshold;  /* wnd size at which to send wnd update    */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  /*-------------------------------------------------------------------------
    Get the TCP control block pointer.
  -------------------------------------------------------------------------*/
  tcb_ptr = scb_ptr->protocol_ctl_blk.tcb;
  if( NULL == tcb_ptr )
  {
    DS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_0("NULL tcb_ptr passed");
    *dss_errno = DS_EFAULT;
    return DSS_ERROR;
  }


  if(tcb_ptr->sockfd == SOCKAVAIL)
  {
    DS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_INFO1_0( "Sockfd in TCB is 0!!");
    return DSS_SUCCESS;
  }

  /*-------------------------------------------------------------------------
    Check if there is anything in the TCP receive queue. If not, return
    DS_EWOULDBLOCK. However, we return EOF indication if there is nothing to
    read and tcb is in CLOSE_WAIT state.
  -------------------------------------------------------------------------*/
  if ( tcb_ptr->rcvcnt == 0)
  {
    if (tcb_ptr->state == TCP_CLOSE_WAIT)
    {
      DS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      *dss_errno = DS_EEOF;
      return DSS_SUCCESS;
    }
    else
    {
      scb_ptr->data_available = FALSE;
      DS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      *dss_errno = DS_EWOULDBLOCK;
      return (DSS_ERROR);
    }
  }

  /*-------------------------------------------------------------------------
    There is data in the receive queue, remove the number of bytes req'd.
  -------------------------------------------------------------------------*/
  LOG_MSG_INFO3_1( "rcv.cnt=%d", tcb_ptr->rcvcnt);

  initial_wnd = tcb_ptr->rcv.wnd;
  bytes_available = MIN(tcb_ptr->rcvcnt, DSS_READ_MAX_BYTES);
  /*-------------------------------------------------------------------------
    Use dsm_pullup to remove the number of bytes that the application wants
    to read. The buffers will be freed by dsm_pullup().
  -------------------------------------------------------------------------*/
  DS_ASSERT(tcb_ptr->rcvq != NULL);

  for(i=0;i<iovcount && bytes_available > 0;i++)
  {
    remove_cnt = (sint15) MIN( bytes_available, iov[i].ps_iov_len);
    if(remove_cnt > 0)
    {
      read_cnt = (sint15) dsm_pullup( &(tcb_ptr->rcvq), iov[i].ps_iov_base,
                 (uint16) remove_cnt);
      tcb_ptr->rcvcnt  -= (uint16) read_cnt;
      tcb_ptr->rcv.wnd += (uint16) read_cnt;
    }
    DS_ASSERT(read_cnt == remove_cnt);

    bytes_read += (int16)read_cnt;
    bytes_available -= (uint16)read_cnt;
    read_cnt = 0;
  }

  LOG_MSG_INFO3_2(" Read from TCP, bytes=%d, rcv.wnd=%d", bytes_read, tcb_ptr->rcv.wnd);

  /*-------------------------------------------------------------------------
    If there is no more data in TCP rcvq, set data_available to FALSE. Note
    that we should not modify the data_available flag if we are in
    CLOSE_WAIT state so that application performs another read and gets the
    EOF indication.
  -------------------------------------------------------------------------*/

  if ( tcb_ptr->rcvcnt == 0 && tcb_ptr->state != TCP_CLOSE_WAIT )
  {
      scb_ptr->data_available = FALSE;
  }
  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  /*-------------------------------------------------------------------------
    Send a window update only if the window size exceeds the threshold value.
    (in compliance with RFC 1122).
  -------------------------------------------------------------------------*/
  wnd_threshold = MIN((0.5*tcb_ptr->rcvbuf), (2*tcb_ptr->mss));
  if( initial_wnd < wnd_threshold && tcb_ptr->rcv.wnd >= wnd_threshold )
  {
    if(DSS_ERROR ==  dssocki_send_to_ps( scb_ptr,
                        NULL,
                        NULL,
                       APP_TO_PS_SOCKET_WINDOW_REOPEN )
      )
    {
      LOG_MSG_ERROR_0("Error in dssocki_send_to_ps: no memory for command buffers");
      *dss_errno = DS_ENOMEM;
      return DSS_ERROR;
    }
  }

  /*-------------------------------------------------------------------------
    Return the number of bytes that were read.
  -------------------------------------------------------------------------*/
  return(bytes_read);

} /* dsstcp_read() */
/**/

/*===========================================================================
FUNCTION DSSTCP_READ_DSM_CHAIN()

DESCRIPTION
  Reads a DSM item chain from the TCP transport.

  This function does no sanity checking. The sanity checking should be done
  in dss_read_dsm_chain().

  This function is called from the context of the socket application task.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes read from the DSM item chain.

  If there are no items to be read in the rcvq, the return value is
  DSS_ERROR and DS_EWOULDBLOCK is returned in errno.

SIDE EFFECTS
  None.

===========================================================================*/
/**/
sint15 dsstcp_read_dsm_chain
(
  struct scb_s* scb_ptr,     /* Ptr to socket control block for the socket */
  dsm_item_type **item_ptr,         /* Ptr to DSM item chain to be read in */
  struct ps_sockaddr *fromaddr,              /* destination address - ignored */
  uint16 * addrlen,                             /* address length - ignored */
  sint15 *dss_errno                                   /* error condition value */
)
{
  struct tcb*   tcb_ptr;       /* Ptr. to TCP control block                */
  dword         initial_wnd;   /* holds initial Receive Q byte count       */
  sint15        read_cnt=0;    /* # of bytes read from TCP rcvq            */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  /*-------------------------------------------------------------------------
    Get the TCP control block pointer.
  -------------------------------------------------------------------------*/
  tcb_ptr = scb_ptr->protocol_ctl_blk.tcb;
  if( NULL == tcb_ptr )
  {
    DS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_0("NULL tcb_ptr passed");
    *dss_errno = DS_EFAULT;
    return DSS_ERROR;
  }


  if(tcb_ptr->sockfd == SOCKAVAIL)
  {
    DS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_INFO2_0("Sockfd in TCB is 0!!");
    return DSS_SUCCESS;
  }

  /*-------------------------------------------------------------------------
    Check if there is anything in the TCP receive queue. If not, return
    DS_EWOULDBLOCK.
  -------------------------------------------------------------------------*/
  if ( tcb_ptr->rcvcnt == 0)
  {
    if (tcb_ptr->state == TCP_CLOSE_WAIT)
    {
      DS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      *dss_errno = DS_EEOF;
      return DSS_SUCCESS;
    }
    else
    {
      scb_ptr->data_available = FALSE;
      DS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      *dss_errno = DS_EWOULDBLOCK;
      return (DSS_ERROR);
    }
  }

  initial_wnd = tcb_ptr->rcv.wnd;

  /*-------------------------------------------------------------------------
    The TCP receive queue is simply a chain of DSM items.  Pass this chain
    in its entirety to the calling layer.  Reset the receive queue to NULL.
  -------------------------------------------------------------------------*/
  DS_ASSERT(tcb_ptr->rcvq != NULL);

  *item_ptr = (dsm_item_type *)tcb_ptr->rcvq;
  read_cnt = (sint15)dsm_length_packet(*item_ptr);

  /*-------------------------------------------------------------------------
    There should be no data left on the queue.
  -------------------------------------------------------------------------*/
  tcb_ptr->rcvcnt  -= (uint16) read_cnt;
  DS_ASSERT(tcb_ptr->rcvcnt == 0);

  tcb_ptr->rcv.wnd += (uint16) read_cnt;
  tcb_ptr->rcvq = NULL;

  LOG_MSG_INFO3_2(" Read from TCP, bytes=%d, rcv.wnd=%d", read_cnt, tcb_ptr->rcv.wnd);

  /*-------------------------------------------------------------------------
    If there is no more data in TCP rcvq, set data_available to FALSE.
  -------------------------------------------------------------------------*/

  if ( (tcb_ptr->rcvcnt == 0) && (tcb_ptr->state != TCP_CLOSE_WAIT) )
  {
    scb_ptr->data_available = FALSE;
  }
  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  /*-------------------------------------------------------------------------
    Check for a just re-opened window.
  -------------------------------------------------------------------------*/
  if( initial_wnd == 0 && read_cnt != 0)
  {
    if( DSS_ERROR == dssocki_send_to_ps( scb_ptr,
                        NULL,
                        NULL,
                       APP_TO_PS_SOCKET_WINDOW_REOPEN )
      )
    {
      LOG_MSG_ERROR_0("Error in dssocki_send_to_ps: no memory for command buffers");
      *dss_errno = DS_ENOMEM;
      return DSS_ERROR;
    }
  }

  /*-------------------------------------------------------------------------
    Return the number of bytes that were read.
  -------------------------------------------------------------------------*/
  return(read_cnt);

} /* dsstcp_read_dsm_chain() */

/*===========================================================================
FUNCTION DSSTCP_SHUTDOWN()

DESCRIPTION
  This function implements the half-close functionality for tcp sockets.

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
/**/
sint15 dsstcp_shutdown
(
  protocol_ctl_block_type pcb,                   /* protocol control block */
  int  how,                                     /* what action to perform  */
  sint15* dss_errno                                    /* dss error number */
)
{
  ps_event_info_type info;
  struct tcb *tcb_ptr = pcb.tcb;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (tcb_ptr == NULL )
  {
    LOG_MSG_ERROR_0("dsstcp_shutdown() called for NULL tcb_ptr");
    return DSS_ERROR;
  }

  if(tcb_ptr->sockfd == SOCKAVAIL)
  {
    LOG_MSG_INFO1_0("Sockfd in TCB is 0!!");
    return DSS_SUCCESS;
  }

  /*-----------------------------------------------------------------------
    If the read-half needs to be shut down, clean-up the receive and the
    resequencing queues. Note that shut down of read-half does not
    result in any communication with the peer.
  -----------------------------------------------------------------------*/
  if ( how == DSS_SHUT_RD || how == DSS_SHUT_RDWR  )
  {
    dsm_free_packet( &(tcb_ptr->rcvq) );
    tcb_ptr->rcvcnt = 0;
    //tcb_ptr->flags.cant_recv_more = 1;

    DS_ENTER_CRIT_SECTION(&global_ps_crit_section);

    if(tcb_ptr->scb_ptr != NULL)
    {
      info.id                  = PS_DATA_AVAILABLE_EVENT;
      info.sockfd              = tcb_ptr->sockfd;
      info.data.data_available = FALSE;

      tcb_ptr->info_change_upcall(tcb_ptr->scb_ptr, &info);
    }

    DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

    tcp_cleanup_reseq( tcb_ptr );
  }

  if(how == DSS_SHUT_WR)
  {
    tcb_ptr->flags.half_close = TRUE;
  }

  /*-----------------------------------------------------------------------
    If the write-half needs to be shut down, call close_tcp() which will
    send a FIN after sending out all the queued up data.
  -----------------------------------------------------------------------*/
  if ( how == DSS_SHUT_WR || how == DSS_SHUT_RDWR)
  {
    if ( close_tcp(tcb_ptr) == 0 )
    {
      info.id                   = PS_SOCK_STATE_CHANGE_EVENT;
      info.sockfd               = tcb_ptr->sockfd;
      info.data.state.new_state = DSSOCKI_CLOSING;

      tcb_ptr->info_change_upcall(tcb_ptr->scb_ptr, &info);
    }
  }

  return (DSS_SUCCESS);
}

/*===========================================================================
FUNCTION DSSTCP_CLOSE()

DESCRIPTION
  Initiates the active close for TCP.  If the connection is already closed,
  cleans up the TCP socket and associated resources.

DEPENDENCIES
  None.

RETURN VALUE
  On success, returns DSS_SUCCESS.

SIDE EFFECTS
  Initiates active close for TCP connections.

===========================================================================*/
sint15 dsstcp_close
(
  protocol_ctl_block_type pcb,                   /* protocol control block */
  boolean silent_close_flag,                        /* silent close option */
  sint15 *dss_errno                                   /* error condition value */
)
{
  struct tcb* tcb_ptr;                        /* Ptr. to TCP control block */
  int unack_data;               /* Number of bytes of un-acknowledged data */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_0("dsstcp_close() called, ");

 /*-------------------------------------------------------------------------
    Get the TCP control block pointer
  -------------------------------------------------------------------------*/
  if((tcb_ptr= pcb.tcb) == NULL)
  {
    *dss_errno = 0;
    return DSS_SUCCESS;
  }

  if(tcb_ptr->sockfd == SOCKAVAIL)
  {
    LOG_MSG_INFO1_0("Sockfd in TCB is 0!!");
    return DSS_SUCCESS;
  }

  if(tcb_ptr->state == TCP_CLOSED)
  {
    dsstcpi_free_tcb(tcb_ptr);
    return DSS_SUCCESS;
  }

  unack_data = tcb_ptr->sndcnt;
  /*-------------------------------------------------------------------------
    sndcnt is inclusive of FIN. So, decrement it if FIN is sent.
  -------------------------------------------------------------------------*/
  if(tcb_ptr->state == TCP_FINWAIT1 ||
     tcb_ptr->state == TCP_CLOSING ||
     tcb_ptr->state ==TCP_LAST_ACK)
  {
    unack_data --;
  }

  DS_ASSERT(unack_data >= 0);

  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);
  dsm_free_packet(&(tcb_ptr->rcvq));
  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  /*
    Applications can call dss_close and not wait for DS_CLOSE_EV.
	In such cases scb for the socket is already freed in dss_close. Hence
	we need to set the scb_ptr stored in TCB to NULL and avoid access of 
	a already freed memory
    This case is handled when silent_close_flag is set to TRUE
  */
  if ( silent_close_flag == TRUE )
  {
    tcb_ptr->scb_ptr = NULL;
    reset_tcp(tcb_ptr, PS_PCB_CLOSE_REASON_CODE_RESET);
    *dss_errno = 0;
  }
  else
  {
    if (tcb_ptr->is_linger_on == TRUE && tcb_ptr->time_in_sec != 0)
    {
      (void) ps_timer_start(tcb_ptr->linger_timer, tcb_ptr->time_in_sec);
    }

    (void) close_tcp(tcb_ptr);
  }

  return( DSS_SUCCESS);
} /* dsstcp_close() */

/*===========================================================================
FUNCTION DSSTCP_BIND()

DESCRIPTION
  This is the TCP specific bind() function. It writes the port number into
  the local.conn.port of the socket control block structure.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dsstcp_bind
(
  protocol_ctl_block_type pcb,                   /* protocol control block */
  struct ps_sockaddr_in6 *la                                 /* local address */
)
{
  struct tcb * tcb_ptr = pcb.tcb;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(tcb_ptr->sockfd != SOCKAVAIL)
  {
    /*-----------------------------------------------------------------------
      If tcb_ptr is valid.
      Set the conn.local.port value to the port number passed in. For a NULL
      sockaddr 0's are automatically passed for address/port information from
      the sockets layer.
    -----------------------------------------------------------------------*/
    LOG_MSG_INFO2_1("Called TCP BIND func with port %d", la->ps_sin6_port);
    tcb_ptr->conn.local.port = la->ps_sin6_port;
    tcb_ptr->conn.local.address = la->ps_sin6_addr;
    tcb_ptr->ipcb.src_ip_addr   = la->ps_sin6_addr;
  }

} /* dsstcp_bind() */


/*===========================================================================
FUNCTION DSSTCP_REGISTER_UPCALL()

DESCRIPTION
  This function registers upcall function and user data with the TCB.
  PS calls this function if any TCP specific information has changed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Sets the upcall_fcn and user_data field in TCB
===========================================================================*/
void
dsstcp_register_upcall
(
  struct scb_s *scb_ptr,
  ps_upcall_fcn_ptr upcall_fcn
)
{
  struct tcb *tcb_ptr;                       /* ptr to TCP control block   */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (scb_ptr == NULL)
  {
    return;
  }

  tcb_ptr = scb_ptr->protocol_ctl_blk.tcb;
  if (tcb_ptr == NULL)
  {
    return;
  }

  tcb_ptr->info_change_upcall = upcall_fcn;
  return;
}

/*===========================================================================
FUNCTION DSSTCP_SETOPT()

DESCRIPTION
  This function sets the option to the user specified value.

DEPENDENCIES
  None.

PARAMETERS
  tcb_ptr - pointer to tcb

RETURN VALUE
  NONE.

SIDE EFFECTS
  None.
===========================================================================*/
void dsstcp_setopt
(
  protocol_ctl_block_type pcb,                   /* protocol control block */
  int        level,                                        /* option level */
  int        optname,                                       /* option name */
  void     * optval,                                /* value of the option */
  int        optlen                               /* length of option data */
)
{
  struct tcb * tcb_ptr = pcb.tcb;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(tcb_ptr->sockfd == SOCKAVAIL)
  {
    LOG_MSG_ERROR_0("Sockfd in TCB is 0!!");
    return;
  }

  if(level == (int)DSS_IPPROTO_IP || level == (int)DSS_IPPROTO_IPV6)
  {
    ip_setopt(&tcb_ptr->ipcb, level, optname, optval, optlen);
    return;
  }

  switch(optname)
  {

    case DSS_SO_RCVBUF:
      tcb_ptr->rcvbuf = *((unsigned int *) optval);
      break;

    case DSS_SO_KEEPALIVE:
      tcb_ptr->options.tcp_keepalive = *((int *)optval) ? TRUE : FALSE;
      break;

    case DSS_TCP_MAXSEG:
      if(tcb_ptr->state == TCP_CLOSED)
      {
        tcb_ptr->mss = *((unsigned int *) optval);
      }
      else
      {
        if(tcb_ptr->mss > *((unsigned int *) optval))
        {
          tcb_ptr->mss = *((unsigned int *) optval);
        }
        else
        {
          LOG_MSG_INFO2_3("NEGLECTING MSS SET REQUEST", tcb_ptr->state,
                  tcb_ptr->mss, *((int *) optval));
        }
      }
      break;

    case DSS_TCP_NODELAY:
      tcb_ptr->options.tcp_nodelay = *((int *)optval) ? TRUE : FALSE;
      break;

    case DSS_TCP_DELAYED_ACK:
      tcb_ptr->options.tcp_delayed_ack = *((int *)optval) ? TRUE : FALSE;
      break;

    case DSS_TCP_SACK:
      tcb_ptr->options.tcp_sack_permitted = *((int *)optval) ? TRUE : FALSE;
      break;

    case DSS_TCP_TIME_STAMP:
      tcb_ptr->options.tcp_ts_permitted = *((int *)optval) ? TRUE : FALSE;
      break;
    case DSS_TCP_MAX_BACKOFF_TIME:
      if (*((int *)optval) == 0)
      {
        tcb_ptr->max_backoff_time = TCP_MAX_TOTAL_BACKOFF_TIME;
      }
      else
      {
        tcb_ptr->max_backoff_time = *((int *)optval);
      }
      break;

    default:
      break;
  }
} /* dsstcp_setopt() */

/*===========================================================================
FUNCTION DSSTCP_WINDOW_REOPEN()

DESCRIPTION
    This function reopens a previously closed window.

DEPENDENCIES
  None.

RETURN VALUE
  On success, returns DSS_SUCCESS.

SIDE EFFECTS
None
===========================================================================*/
int16 dsstcp_window_reopen
(
  protocol_ctl_block_type pcb,                   /* protocol control block */
  int16                    *dss_errno               /* error condition value  */
)
{
  struct tcb* tcb_ptr;                        /* Ptr. to TCP control block */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

 /*-------------------------------------------------------------------------
    Get the TCP control block pointer
  -------------------------------------------------------------------------*/
  if((tcb_ptr= pcb.tcb) == NULL)
  {
    *dss_errno = 0;
    return DSS_SUCCESS;
  }

  if(tcb_ptr->sockfd == SOCKAVAIL)
  {
    LOG_MSG_INFO2_0("Sockfd in TCB is 0!!");
    return DSS_SUCCESS;
  }

  LOG_MSG_INFO2_0("TCP RX window re-opened");

  /*-------------------------------------------------------------------------
    Window was closed. Force back open.
  -------------------------------------------------------------------------*/
  tcb_ptr->flags.force = 1;
  tcp_output( tcb_ptr);

  return( DSS_SUCCESS);

} /* dsstcp_window_reopen() */

/*===========================================================================
FUNCTION DSSTCP_FLOW_ENABLED_IND()

DESCRIPTION
  This function processes flow_enabld_ind on socket's routing and flow cache

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsstcp_flow_enabled_ind
(
  protocol_ctl_block_type  pcb                   /* protocol control block */
)
{
  struct tcb  * tcb_ptr;                      /* Ptr. to TCP control block */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get the TCP control block pointer
  -------------------------------------------------------------------------*/
  if ((tcb_ptr = pcb.tcb) == NULL)
  {
    return;
  }

  if (tcb_ptr->sockfd == SOCKAVAIL)
  {
    LOG_MSG_INFO2_0("Sockfd in TCB is 0!!");
    return;
  }

  /*-------------------------------------------------------------------------
    TCP is allowed to write data again
  -------------------------------------------------------------------------*/
  tcp_output(tcb_ptr);
  return;

} /* dsstcp_flow_enabled_ind() */

/*===========================================================================
FUNCTION DSSTCP_ISWRITEABLE()

DESCRIPTION
  This function determines if one can write more data to the specified tcb.
  The objective is that we not only want to limit the number of
  outstanding segments but also the amount of data queued up by the
  application.

DEPENDENCIES
  None.

RETURN VALUE
 Return the number of bytes available in sndq.

SIDE EFFECTS
  None.
===========================================================================*/
int dsstcp_is_writeable
(
  struct scb_s * scb_ptr                    /* socket control block */
)
{
  uint32 min_send_bytes;   /* minimum between sndbuf and congestion window */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  min_send_bytes = MIN(scb_ptr->sndbuf,scb_ptr->cached_info.pcb.tcp.cwind);
  if(min_send_bytes > scb_ptr->sndcnt)
  {
	return (int)(min_send_bytes - scb_ptr->sndcnt);
  }

  return 0;
} /* dsstcp_is_writeable() */

/*===========================================================================
FUNCTION DSSTCP_GET_CONN_ERROR()

DESCRIPTION
  Gets the appropriate error code from the closed_state_reason.  Depending
  on the type of call (connect(), or read()/write()), different error
  values can be returned.

DEPENDENCIES
  None.

RETURN VALUE
  Error condition for connection state.

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dsstcp_get_conn_error
(
  struct scb_s *scb_ptr,                    /* ptr to socket control block */
  dssocki_caller_enum_type type             /* type of the caller function */
)
{
  sint15 reason;                            /* reason for connection error */
  sint15 dss_errno = DSS_ERROR;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Reason is set in the SCB whenever TCP state changes to CLOSED.
    Based on reason for close - return appropriate error condition based
    on the type of call it is being returned for.
  -------------------------------------------------------------------------*/
  reason = scb_ptr->closed_state_reason;

  DS_MSG1(MSG_LEGACY_HIGH, "TCP Closed Reason: %d", reason);

  switch (reason)
  {
    case PS_PCB_CLOSE_REASON_CODE_NETWORK:
      dss_errno = DS_ENETDOWN;
      break;

    case PS_PCB_CLOSE_REASON_CODE_NORMAL:
      /*---------------------------------------------------------------------
        The normal TCP close of connection - No connection exists
      ---------------------------------------------------------------------*/
      if (type == DSSOCKI_CONNECT_ROUTINE)
      {
        dss_errno = DS_ENOTCONN;
      }
      else if ( type == DSSOCKI_INPUT_ROUTINE ||
                type == DSSOCKI_OUTPUT_ROUTINE )
      {
        /*-------------------------------------------------------------------
          We have received an EOF, so assume it was a passive close
        -------------------------------------------------------------------*/
        dss_errno = DS_EEOF;
      }
      break;

    case PS_PCB_CLOSE_REASON_CODE_RESET:
      /*---------------------------------------------------------------------
        Connection was reset
      ---------------------------------------------------------------------*/
      if (type == DSSOCKI_CONNECT_ROUTINE)
      {
        if(IS_SOCKOPT_SET(scb_ptr->options_mask, (uint64)DSS_SO_ERROR_ENABLE) &&
           scb_ptr->so_error)
        {
          DS_ENTER_CRIT_SECTION(&global_ps_crit_section);
          dss_errno = (sint15)scb_ptr->so_error;
          scb_ptr->so_error = 0;
          DSSOCKI_RESET_ERROR_AVAILABLE(scb_ptr);
          DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
        }
        else
        {
          dss_errno = DS_ECONNREFUSED;
        }
      }
      else if ( type == DSSOCKI_INPUT_ROUTINE ||
                type == DSSOCKI_OUTPUT_ROUTINE )
      {
        dss_errno = DS_ECONNRESET;
      }
      else
      {
        DS_MSG0(MSG_LEGACY_ERROR,"Invalid type for reason");
      }
      break;

    case PS_PCB_CLOSE_REASON_CODE_TIMEOUT:
      /*---------------------------------------------------------------------
        Connection was aborted
      ---------------------------------------------------------------------*/
      if(IS_SOCKOPT_SET(scb_ptr->options_mask, (uint64)DSS_SO_ERROR_ENABLE) &&
         scb_ptr->so_error)
      {
        DS_ENTER_CRIT_SECTION(&global_ps_crit_section);
        dss_errno = (sint15)scb_ptr->so_error;
        scb_ptr->so_error = 0;
        DSSOCKI_RESET_ERROR_AVAILABLE(scb_ptr);
        DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      }
      else if (type == DSSOCKI_CONNECT_ROUTINE)
      {
        dss_errno = DS_ETIMEDOUT;
      }
      else if ( type == DSSOCKI_INPUT_ROUTINE ||
                type == DSSOCKI_OUTPUT_ROUTINE )
      {
        dss_errno = DS_ECONNABORTED;
      }
      else
      {
        DS_MSG0(MSG_LEGACY_ERROR,"Invalid type for reason");
      }
      break;

    default:
      /*---------------------------------------------------------------------
        Invalid closed state reason.
      ---------------------------------------------------------------------*/
      DS_MSG1(MSG_LEGACY_ERROR,"Invalid closed state reason %d",reason);
      break;
  }

  return (dss_errno);

} /* dsstcp_get_conn_error() */

