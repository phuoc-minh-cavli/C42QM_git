/*===========================================================================

                             D S S O C K E T . C

DESCRIPTION
 The Data Services mobile socket API File. Contains basic API functions for
 socket manipulation.

Copyright (c) 2018 - 2019 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
==============================================================================
06/20/19        ds              Code changes to increment REF count for IPv6 
                                address used for binding by application.
08/08/18        sr              Initial Version
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ds_headers.h"
#include "target.h"
#include "dss.h"
#include "dssocki.h"
#include "dsm.h"
#include "ps_svc.h"
#include "ps_udp.h"
#include "dstask.h"
#include "dss_ps.h"
#include "assert.h"
#include "ps_meta_info.h"
#include "ds_sys_ioctl.h"
#include "ps_mem.h"
#include "ps_utils.h"
#include "ds_flow.h"
#include "dss_config.h"


/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================
FUNCTION DSS_SOCKET2()

DESCRIPTION
 Create a socket and related data structures, and return a socket descriptor.

 The mapping to actual protocols is as follows:

 ADDRESS FAMILY         Stream          Datagram

 DSS_AF_INET                TCP             UDP

  Note this function must be called to obtain a valid socket descriptor, for
  use with all other socket-related functions.  Thus, before any socket
  functions can be used (e.g. I/O, asynchronous notification, etc.), this
  call must have successfully returned a valid socket descriptor.  The
  application must also have made a call to dss_open_netlib() to obtain
  a valid application ID, and to put the Data Services task into "sockets"
  mode.

  Note:  This implementation version has no support for Raw IP sockets, and
         will return an error, if the application attempts to create one.

         Sockets created using socket2 are not bound to any particular dss_nethandle.

DEPENDENCIES
  Netpolicy structure needs to be initialized by calling dss_init_netpolicy.

RETURN VALUE
  On successful creation of a socket, this function returns socket file
  descriptor which is a sint15 value between 0x1000 and 0x1FFF.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EAFNOSUPPORT     address family not supported
  DS_EBADAPP          invalid application ID
  DS_EPROTOTYPE       specified protocol invalid for socket type
  DS_ESOCKNOSUPPORT   invalid or unsupported socket parameter specified
  DS_EPROTONOSUPPORT  specified protocol not supported
  DS_EMFILE           too many descriptors open.  A socket is already open or
                      has not closed compeletely.

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_socket_create
(
  byte   family,                                         /* Address family */
  byte   type,                                              /* socket type */
  byte   protocol,                                        /* protocol type */
  dss_sock_cb_fcn sock_cb,                     /* socket callback function */
  void * sock_cb_user_data,              /* User data for socket call back */
  sint15 *dss_errno                               /* error condition value */
)
{
  struct scb_s *scb_ptr;       /* ptr to socket control block allocated in */
//  dss_net_policy_info_type default_policy; /* temp var to store def policy */
  sint15   result;                    /* tmp return value from called fcns */
//  ps_route_scope_type      rscope;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Check that a valid ptr to a dss_errno variable is specified
  -------------------------------------------------------------------------*/
  if (dss_errno == NULL)
  {
    DS_ERROR_LOG_0( "dss_errno is NULL");
    return(DSS_ERROR);
  }
  *dss_errno = DSS_SUCCESS;

  DS_MSG3( MSG_LEGACY_HIGH,  "Creating socket;family = %d  prot=%d type=%d",
           family, protocol, type);

  /*-------------------------------------------------------------------------
    Only support Internet address family
   ------------------------------------------------------------------------*/
  if ((family != (byte)DSS_AF_INET) && (family != (byte)DSS_AF_INET6))
  {
    *dss_errno = DS_EAFNOSUPPORT;
    DS_ERROR_LOG_0("Incorrect Address family in dss_socket() call");
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Ensure that we support this socket type - TCP or UDP only
  -------------------------------------------------------------------------*/
  if ( (type != DSS_SOCK_STREAM) && (type != DSS_SOCK_DGRAM) )
  {
    *dss_errno = DS_ESOCKNOSUPPORT;
    return (DSS_ERROR);
  }

 /*-------------------------------------------------------------------------
    Set the protocol field. Return an error if the type and protocol do
    not match.
  -------------------------------------------------------------------------*/
  if (protocol == 0)
  {
    protocol = (byte)((type == (byte)DSS_SOCK_STREAM) ? PS_IPPROTO_TCP:PS_IPPROTO_UDP);
  }

  if((protocol != (byte)PS_IPPROTO_UDP) && (protocol != (byte)PS_IPPROTO_TCP) &&
      (protocol != (byte)PS_IPPROTO_ICMP) && (protocol != (byte)PS_IPPROTO_ICMP6))
  {
    *dss_errno = DS_EPROTONOSUPPORT;
    return (DSS_ERROR);
  }

  if((type == (byte)DSS_SOCK_STREAM && protocol != (byte)PS_IPPROTO_TCP) ||
     (type == (byte)DSS_SOCK_DGRAM && ((protocol != (byte)PS_IPPROTO_UDP) && 
     (protocol != (byte)PS_IPPROTO_ICMP)&& (protocol != (byte)PS_IPPROTO_ICMP6))))
  {
    *dss_errno = DS_EPROTOTYPE;
    return DSS_ERROR;
  }


  /*-------------------------------------------------------------------------
    Allocate a socket control block and fill in
  -------------------------------------------------------------------------*/
  if ( (scb_ptr = dssocki_allocscb(protocol))== NULL)
  {
    *dss_errno = DS_EMFILE;
    DS_ERROR_LOG_0("Socket not allocated in dss_socket() call");
    return (DSS_ERROR);
  }

  scb_ptr->type = type;
  scb_ptr->protocol = protocol;
  scb_ptr->family = family;
  scb_ptr->sock_cb = sock_cb;
  scb_ptr->sock_cb_user_data = sock_cb_user_data;

  /*-------------------------------------------------------------------------
    Set up protocol specific function pointer table
  -------------------------------------------------------------------------*/
  result = dssocki_fcnptr_setup(scb_ptr, dss_errno);
  if (result == DSS_ERROR)
  {
    dssocki_freescb(scb_ptr);
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Set socket_state to NULL. Note, this must be done before the protocol
    specific function which changes the state.
  -------------------------------------------------------------------------*/
  dssocki_set_sock_state(scb_ptr, DSSOCKI_NULL);

  /*-------------------------------------------------------------------------
    Call protocol specific socket function
  -------------------------------------------------------------------------*/
  if (scb_ptr->fcn_ptr_table->socket != NULL)
  {
    result = scb_ptr->fcn_ptr_table->socket(scb_ptr, dss_errno);
    if (result == DSS_ERROR)
    {
      dssocki_freescb(scb_ptr);
      return (DSS_ERROR);
    }
  }

  /*-------------------------------------------------------------------------
    Register the handler to receive information change notifications sent
    by the lower layers
  -------------------------------------------------------------------------*/
  if (scb_ptr->fcn_ptr_table->register_upcall != NULL)
  {
    scb_ptr->fcn_ptr_table->register_upcall(scb_ptr,
                                            dssocki_info_change_handler);
  }

  /*-------------------------------------------------------------------------
    Success! - return valid socket descriptor
   ------------------------------------------------------------------------*/
  DS_MSG1(MSG_LEGACY_HIGH, "Created socket;  sockfd=%d ", scb_ptr->sockfd);
  return (scb_ptr->sockfd);

} /* dss_socket_create() */





/*===========================================================================
FUNCTION DSS_SOCKET()

DESCRIPTION
 Create a socket and related data structures,and return a socket descriptor.

 The mapping to actual protocols is as follows:

 ADDRESS FAMILY         Stream          Datagram

 DSS_AF_INET                TCP             UDP

  Note this function must be called to obtain a valid socket descriptor, for
  use with all other socket-related functions.  Thus, before any socket
  functions can be used (e.g. I/O, asynchronous notification, etc.), this
  call must have successfully returned a valid socket descriptor.  The
  application must also have made a call to dss_open_netlib() to obtain
  a valid application ID, and to put the Data Services task into "sockets"
  mode.

  Note:  This implementation version has no support for Raw IP sockets, and
         will return an error, if the application attempts to create one.

         Sockets created using this call are bound to the dss_nethandle used in
         creating this socket.

DEPENDENCIES
  The function dss_open_netlib() must be called to open the network library
  and put the DS/PS managers into sockets mode.

RETURN VALUE
  On successful creation of a socket, this function returns socket file
  descriptor which is a sint15 value between 0x1000 and 0x1FFF.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EAFNOSUPPORT     address family not supported
  DS_EBADAPP          invalid application ID
  DS_ESOCKNOSUPPORT   invalid or unsupported socket parameter specified
  DS_EPROTONOSUPPORT  invalid or unsupported protocol
  DS_EMFILE           too many descriptors open.  A socket is already open
                      or has not closed compeletely.

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_socket
(
  sint15 dss_nethandle,
  byte   family,                          /* Address family - DSS_AF_INET only */
  byte   type,                                              /* socket type */
  byte   protocol,                                        /* protocol type */
  sint15 *dss_errno                               /* error condition value */
)
{
  acb_type     * acb_ptr;              /* ptr to application control block */
  struct scb_s *scb_ptr;       /* ptr to socket control block allocated in */
  sint15 sockfd;                                      /* socket descriptor */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG1(MSG_LEGACY_HIGH,"Creating socket for application dss_nethandle=%d ",
          dss_nethandle);

 /*-------------------------------------------------------------------------
    Invalid application ID
  -------------------------------------------------------------------------*/
  if ( (acb_ptr=dssocki_dss_nethandletoacb(dss_nethandle)) == NULL)
  {
    *dss_errno = DS_EBADAPP;
    return (DSS_ERROR);
  }

  sockfd = dss_socket_create(family,
                       type,
                       protocol,
                       acb_ptr->sock_cb,
                       acb_ptr->sock_cb_user_data,
                       dss_errno);

  /*-------------------------------------------------------------------------
    Set the acb_ptr and increase the SCB ref count
  -------------------------------------------------------------------------*/
  if ( (scb_ptr=dssocki_sockfdtoscb(sockfd)) != NULL)
  {
    DSSOCKI_BIND_ACB_TO_SCB(scb_ptr, acb_ptr);
  }
  return sockfd;
} /* dss_socket() */



/*===========================================================================
FUNCTION DSS_CONNECT()

DESCRIPTION
  For TCP, attempts to establish the TCP connection.  Upon
  successful connection, calls the socket callback function asserting that
  the DS_WRITE_EVENT is TRUE.  For udp it fills in the peers ipaddr in the
  socket control block and binds the socket to an interface.
  The function must receive (as a parameter) a valid socket descriptor,
  implying a previous successful call to dss_socket().

DEPENDENCIES
  Network subsystem must be established and available.

RETURN VALUE
  Return DSS_ERROR and places the error condition value in *dss_errno.

  dss_errno Values
  ----------------
  DS_EWOULDBLOCK      operation would block
  DS_EBADF            invalid socket descriptor is specfied
  DS_ECONNREFUSED     connection attempt refused
  DS_ETIMEDOUT        connection attempt timed out
  DS_EFAULT           addrlen parameter is invalid
  DS_EIPADDRCHANGED   IP address changed due to PPP resync
  DS_EINPROGRESS      connection establishment in progress
  DS_EISCONN          a socket descriptor is specified that is already
                      connected
  DS_ENETDOWN         network subsystem unavailable
  DS_EOPNOTSUPP       invalid server address specified
  DS_EADDRREQ         destination address is required.
  DS_NOMEM            not enough memory to establish connection

SIDE EFFECTS
  For TCP, initiates active open for connection.
===========================================================================*/
sint15 dss_connect
(
  sint15 sockfd,                                      /* Socket descriptor */
  struct ps_sockaddr *servaddr,                        /* destination address */
  uint16 addrlen,                                    /* length of servaddr */
  sint15 *dss_errno                               /* error condition value */
)
{
   struct scb_s *scb_ptr;                   /* Ptr to socket control block */
   sint15 result;                                          /* return value */
   struct ps_sockaddr_in6 v6_sockaddr;      /* protocol specific dest address */
   struct socket servname;                      /* servname from SCB cache */
   boolean flow_available = FALSE;
   ip_pkt_info_type pkt_info;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check that a valid ptr to a dss_errno variable is specified
  -------------------------------------------------------------------------*/
  if (dss_errno == NULL)
  {
    DS_ERROR_LOG_0("dss_errno is NULL");
    return(DSS_ERROR);
  }
  *dss_errno = DSS_SUCCESS;

  /*-------------------------------------------------------------------------
    Invalid socket descriptor
  -------------------------------------------------------------------------*/
  if ( (scb_ptr=dssocki_sockfdtoscb(sockfd)) == NULL)
  {
    *dss_errno = DS_EBADF;
    return (DSS_ERROR);
  }
  /*-------------------------------------------------------------------------
    If there is no connect setup function defined then connect on this socket
    is unsupported.
  -------------------------------------------------------------------------*/
  if(!scb_ptr->fcn_ptr_table->connect_setup)
  {
    *dss_errno = DS_EOPNOTSUPP;
    return DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    Convert ps_sockaddr to ps_sockaddr_in6.
  -------------------------------------------------------------------------*/
  if( dssocki_get_sockaddr_in6(&v6_sockaddr, servaddr, addrlen, dss_errno) ==
      DSS_ERROR )
  {
    DS_ERROR_LOG_1( "Invalid Server address: ERRNO %d", *dss_errno);
    return DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    If address is ps_in6addr_any return error and it is a udp socket then
    disconnect an earlier association, else return DSS_ERROR.
  -------------------------------------------------------------------------*/
  if(PS_IN6_ARE_ADDR_EQUAL(&v6_sockaddr.ps_sin6_addr, &ps_in6addr_any))
  {
    if(scb_ptr->protocol == (byte)PS_IPPROTO_UDP)
    {
      return scb_ptr->fcn_ptr_table->connect_setup(scb_ptr, NULL, dss_errno);
    }
    else
    {
      *dss_errno = DS_EADDRREQ;
      return DSS_ERROR;
    }
  }

  /*-------------------------------------------------------------------------
    Invalid destination port.
  -------------------------------------------------------------------------*/
  if(v6_sockaddr.ps_sin6_port == 0)
  {
    *dss_errno = DS_EINVAL;
    return DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    Determine state and return appropriate error code
  -------------------------------------------------------------------------*/
  result = dssocki_get_err_val(scb_ptr, DSSOCKI_CONNECT_ROUTINE, dss_errno);
  if ( (result == DSS_ERROR) && (*dss_errno != DS_EWOULDBLOCK) )
  {
    DSSOCKI_CACHE_GET_REMOTE( scb_ptr, servname );

    /*-----------------------------------------------------------------------
      We return EINVAL if the socket is already connected but to a different
      endpoint than passed in as servaddr.
    -----------------------------------------------------------------------*/
    if ( *dss_errno == DS_EISCONN &&
         ( !PS_IN6_ARE_ADDR_EQUAL(&v6_sockaddr.ps_sin6_addr, &servname.address) ||
	       servname.port != v6_sockaddr.ps_sin6_port) )
    {
      *dss_errno = DS_EINVAL;
    }
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Do early routing and add the result to the SCB's routing cache.
  -------------------------------------------------------------------------*/

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
    *dss_errno = DS_EWOULDBLOCK;
    return DSS_ERROR;
  }

  if (scb_ptr->flow_ptr == NULL)
  {
    DS_ERROR_LOG_0("Flow pointer is NULL. Cant route packet");
    *dss_errno = DS_EWOULDBLOCK;
    return DSS_ERROR;
  }

  DS_MSG1(MSG_LEGACY_HIGH, "dss_connect Flow pointer to route "
          " packet %p",scb_ptr->flow_ptr);

  servname.address = v6_sockaddr.ps_sin6_addr;
  servname.port    = v6_sockaddr.ps_sin6_port;

  DSSOCKI_CACHE_SET_REMOTE(scb_ptr, servname);
  DSSOCKI_CACHE_SET_FLOW_LABEL(scb_ptr, v6_sockaddr.ps_sin6_flowinfo);
  DSSOCKI_CACHE_SET_TX_FWK_INST(scb_ptr,scb_ptr->acb_ptr->fw_inst);

  /*-------------------------------------------------------------------------
    Setup protocol specific connection requirements
  -------------------------------------------------------------------------*/
  result = scb_ptr->fcn_ptr_table->connect_setup(scb_ptr,
                                                 &v6_sockaddr,
                                                 dss_errno);

  return (result);

}  /* dss_connect() */



/*===========================================================================
FUNCTION DSS_LISTEN()

DESCRIPTION

  For TCP, this starts a passive open for connections.  Upon a
  sucessful connection, the socket callback function is invoked
  asserting DS_ACCEPT_EVENT as TRUE.  The application should respond
  with a call to dss_accept(). If a connection is recieved and there
  are no free queue slots the new connection is rejected
  (ECONNREFUSED).  The backlog queue is for ALL unaccepted sockets
  (half-open, or completely established).

  A listening UDP doesn't make sense, and as such isn't supported.
  DS_EOPNOTSUPP is returned.

  The sockfd parameter is a created (dss_socket) and bound (dss_bind)
  socket that will become the new listening socket.  The backlog
  parameter indiates the maximum length for the queue of pending
  sockets.  If backlog is larger than the maximum, it will be
  reduced to the maximum (see DSS_SOMAXCONN). This is the BSD behavior.

  The argument dss_error should point to a memory location in which
  error conditions can be recorded.

DEPENDENCIES

  Network subsystem must be established and available.

  The sockfd should get a valid socket descriptor (implying a
  previously successful call to dss_socket) This socket should be
  bound to a specific port number (implying a previously successful
  call to dss_bind) .

RETURN VALUE

  Returns DSS_SUCCESS on success.  If the backlog was truncated
  DS_EFAULT will be set in errno, but the call will still be
  successful.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EWOULDBLOCK      operation would block (PJ: I don't think this CAN happen)
  DS_EBADF            invalid socket descriptor is specfied
  DS_EOPNOTSUPP       The socket is not capable of listening (UDP)
  DS_EFAULT           backlog parameter is invalid
  DS_ENETNONET        network subsystem unavailable for some unknown reason
  DS_ENETINPROGRESS   network subsystem establishment currently in progress
  DS_ENETCLOSEINPROGRESS network subsystem close in progress.
  DS_NOMEM            not enough memory to establish backlog connections.
  DS_EINVAL           Socket already open, closed, unbound or not one
                      you can listen on.

SIDE EFFECTS
  For TCP, initiates passive open for new connections.
===========================================================================*/
sint15 dss_listen
(
  sint15 sockfd,                                      /* Socket descriptor */
  sint15 backlog,                      /* Number of connections to backlog */
  sint15 *dss_errno                               /* error condition value */
)
{
   struct scb_s *scb_ptr;                   /* Ptr to socket control block */
   sint15 result;                                          /* return value */
   struct socket localname;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check that a valid ptr to a dss_errno variable is specified
  -------------------------------------------------------------------------*/
  if (dss_errno == NULL)
  {
    DS_ERROR_LOG_0( "dss_errno is NULL");
    return(DSS_ERROR);
  }
  *dss_errno = DSS_SUCCESS;

  DS_MSG1(MSG_LEGACY_HIGH, "dss_listen on socket %d", sockfd);

  /*-------------------------------------------------------------------------
    Invalid socket descriptor
  -------------------------------------------------------------------------*/
  if ( (scb_ptr=dssocki_sockfdtoscb(sockfd)) == NULL)
  {
    *dss_errno = DS_EBADF;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Only support TCP - no listening UDP sockets
  -------------------------------------------------------------------------*/
  if (scb_ptr->protocol != (byte)PS_IPPROTO_TCP)
  {
    *dss_errno = DS_EOPNOTSUPP;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Check to see that socket has already been bound to a local address.  If
    not, return error.
  -------------------------------------------------------------------------*/
  DSSOCKI_CACHE_GET_LOCAL(scb_ptr, localname);

  if (localname.port == 0)
  {
    *dss_errno = DS_EINVAL;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Check for errors in backlog parameter - It must be greater than 0
  -------------------------------------------------------------------------*/
  if (backlog <= 0)
  {
    *dss_errno = DS_EFAULT;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Determine state and return appropriate error code
  -------------------------------------------------------------------------*/
  result = dssocki_get_err_val(scb_ptr, DSSOCKI_LISTEN_ROUTINE, dss_errno);

  if (result == DSS_ERROR)
  {
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Silently truncate queue to SOMAXCONN
  -------------------------------------------------------------------------*/
  if (backlog > DSS_SOMAXCONN)
  {
    *dss_errno = DS_EFAULT;
    backlog = DSS_SOMAXCONN;
  }

  /*-------------------------------------------------------------------------
    Setup the protocol specific listen
  -------------------------------------------------------------------------*/
  if (scb_ptr->fcn_ptr_table->listen_setup != NULL)
  {
    *dss_errno = DS_ENOMEM;
    return scb_ptr->fcn_ptr_table->listen_setup(scb_ptr,
                        backlog,
                        dss_errno);
  }
  else
  {
    *dss_errno = DS_EOPNOTSUPP;
    return (result);
  }
}  /* dss_listen() */



/*===========================================================================
FUNCTION DSS_ACCEPT()

DESCRIPTION

  The accept function is used on listening sockets to respond when
  DS_ACCEPT_EVENT is asserted.  The first backlog queued connection is
  removed from the queue, and bound to a new connected socket (as if
  you called dss_socket).  The newly created socket is in the
  connected state.  The listening socket is unaffect the queue size is
  maintained (ie. there is not need to call listen again.)

  The argument sockfd is the file descriptor of the listening socket

  The argument remote addr is a pointer to a struct ps_sockaddr.  This
  structure is populated with the address information for the remote
  end of the new connection. addrlen should initially contain the
  length of the struct ps_sockaddr passed in.  The length of the real
  address is placed in this location when the struct is populated.

  The argument dss_error should point to a memory location in which
  error conditions can be recorded.

DEPENDENCIES

  Network subsystem must be established and available.

  The sockfd should get a valid socket descriptor (implying a
  previously successful call to dss_socket) This socket should be
  bound to a specific port number (implying a previously successful
  call to dss_bind).  The socket should be listening (implying a
  previously successful call to dss_listen).

RETURN VALUE
  Returns the socket descriptor of the new socket on success.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EWOULDBLOCK      operation would block
  DS_EBADF            invalid socket descriptor is specfied
  DS_EOPNOTSUPP       The socket is not of type DSS_SOCK_STREAM
  DS_EINVAL           Socket is not listening.
  DS_EFAULT           The addr parameter is bogus.
  DS_ENETNONET        network subsystem unavailable for some unknown reason
  DS_ENETINPROGRESS   network subsystem establishment currently in progress
  DS_ENETCLOSEINPROGRESS network subsystem close in progress.
  DS_NOMEM            not enough memory to establish backlog connections.

SIDE EFFECTS

  The head backlog item from the queue of the listening socket is
  removed from that queue.
===========================================================================*/
sint15 dss_accept
(
  sint15 sockfd,                                      /* Socket descriptor */
  struct ps_sockaddr *remoteaddr,                       /* new remote address */
  uint16 *addrlen,                                   /* length of servaddr */
  sint15 *dss_errno                               /* error condition value */
)

{
   struct scb_s *scb_ptr;                   /* Ptr to socket control block */
   sint15 result;                                          /* return value */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check that a valid ptr to a dss_errno variable is specified
  -------------------------------------------------------------------------*/
  if (dss_errno == NULL)
  {
    DS_ERROR_LOG_0( "dss_errno is NULL");
    return(DSS_ERROR);
  }

  DS_MSG1(MSG_LEGACY_HIGH,"dss_accept called for sockfd %d",sockfd);

  *dss_errno = DSS_SUCCESS;

  /*-------------------------------------------------------------------------
    Invalid socket descriptor
  -------------------------------------------------------------------------*/
  if ((scb_ptr=dssocki_sockfdtoscb(sockfd)) == NULL)
  {
    *dss_errno = DS_EBADF;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Only support TCP - can't accept on non-TCP sockets.
  -------------------------------------------------------------------------*/
  if (scb_ptr->protocol != (byte)PS_IPPROTO_TCP)
  {
    *dss_errno = DS_EOPNOTSUPP;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Verify remote addr and addr len are legit.
  -------------------------------------------------------------------------*/
  if ((remoteaddr == NULL) ||
      (addrlen == NULL))
  {
    *dss_errno = DS_EFAULT;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Determine state and return appropriate error code
  -------------------------------------------------------------------------*/
  result = dssocki_get_err_val(scb_ptr, DSSOCKI_ACCEPT_ROUTINE, dss_errno);

  if ( (result == DSS_ERROR) && (*dss_errno != DS_EWOULDBLOCK) )
  {
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Do the protocol specific accept
  -------------------------------------------------------------------------*/
  if (scb_ptr->fcn_ptr_table->ds_accept != NULL)
  {
    *dss_errno = DS_ENOMEM;
    return scb_ptr->fcn_ptr_table->ds_accept(scb_ptr,
                      remoteaddr,
                      addrlen,
                      dss_errno);
  }
  else
  {
    *dss_errno = DS_EOPNOTSUPP;
    return (result);
  }

}  /* dss_accept() */





/*===========================================================================
FUNCTION DSS_BIND()

DESCRIPTION
  For all client sockets, attaches a local address and port value to the
  socket.  If the call is not explicitly issued, the socket will implicitly
  bind in calls to dss_connect() or dss_sendto().  Note that this function
  does not support binding to a local IPv4 address, but rather ONLY a local
  port number.  The local IPv4 address is assigned automatically by the sockets
  library.  This function does support binding to a local IPv6 address,
  however, as this is required for using IPv6 Privacy Extensions (RFC 3041).
  The function must receive (as a parameter) a valid socket descriptor,
  implying a previous successful call to dss_socket().

DEPENDENCIES
  None.

RETURN VALUE
  Returns DSS_SUCCESS on success.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor is specfied
  DS_EAFNOSUPPORT     address family not supported
  DS_EOPNOTSUPP       operation not supported
  DS_EADDRINUSE       the local address is already in use.
  DS_EINVAL           the socket is already attached to a local name
  DS_EFAULT           invalid address and/or invalid address length

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_bind
(
  sint15 sockfd,                                      /* socket descriptor */
  struct ps_sockaddr *localaddr,                             /* local address */
  uint16 addrlen,                                     /* length of address */
  sint15 *dss_errno                               /* error condition value */
)
{
  struct scb_s *scb_ptr;      /*               ptr to socket control block */
  struct ps_sockaddr_in6 v6_sockaddr;
  sint15 result;
  struct socket localname;
  dssocki_app_to_ps_cmd_type *ps_cmd;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check that a valid ptr to a dss_errno variable is specified
  -------------------------------------------------------------------------*/
  if (dss_errno == NULL)
  {
    DS_ERROR_LOG_0( "dss_errno is NULL");    
    return (DSS_ERROR);
  }
  *dss_errno = DSS_SUCCESS;

  DS_MSG1(MSG_LEGACY_HIGH, " DSS_BIND called on sock %d",sockfd);

  ps_cmd = dssocki_alloc_ps_cmd_buffer(APP_TO_PS_SOCKET_BIND);
  if(NULL == ps_cmd)
  {
    DS_ERROR_LOG_0( "Out of mem for PS cmd");
    *dss_errno = DS_ENOMEM;
    return DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    Convert socket descriptor to SCB
  -------------------------------------------------------------------------*/
  if ( (scb_ptr=dssocki_sockfdtoscb(sockfd)) == NULL)
  {
    *dss_errno = DS_EBADF;
    PS_MEM_FREE(ps_cmd);
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Determine state and return appropriate error code
  -------------------------------------------------------------------------*/
  result = dssocki_get_err_val(scb_ptr, DSSOCKI_BIND_ROUTINE, dss_errno);

  if (result == DSS_ERROR)
  {
    PS_MEM_FREE(ps_cmd);
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Check to see if socket has already been bound to a local address.  If
    so, return error.
  -------------------------------------------------------------------------*/
  DSSOCKI_CACHE_GET_LOCAL(scb_ptr, localname);

  /*-------------------------------------------------------------------------
    Check to see if localname.addr is not a multicast addr and not already
    been bound to a local address.  If so, return error. This check only
    done for old bcmcs setsockopt compatibility.
  -------------------------------------------------------------------------*/
  if(localaddr != NULL)
  {
/*---------------------------------------------------------------------------
  Only if its a TCP connect socket, then we shud not allow this but otherwise we can allow..
---------------------------------------------------------------------------*/
      if (!PS_IN6_ARE_ADDR_EQUAL(&ps_in6addr_any, &localname.address))
      {
        *dss_errno = DS_EINVAL;
        PS_MEM_FREE(ps_cmd);
        return (DSS_ERROR);
      }

    if (localname.port != 0)
    {
      *dss_errno = DS_EINVAL;
      PS_MEM_FREE(ps_cmd);
      return (DSS_ERROR);
    }
  }
  else
  {
  /*-------------------------------------------------------------------------
    If not previously bound this is invalid. Otherwise a NULL localaddr will
    unbind the socket.
  -------------------------------------------------------------------------*/
    /*-----------------------------------------------------------------------
      Unbinding. Call protocol specific function to complete the unbind.
    -----------------------------------------------------------------------*/
    DS_MSG1(MSG_LEGACY_HIGH, "Unbinding sock %d", sockfd);

/*---------------------------------------------------------------------------
  Check if rebinding is allowed by BSD
---------------------------------------------------------------------------*/
    if(DSS_ERROR == dssocki_bind(scb_ptr, NULL))
    {
      DS_ERROR_LOG_0( "dssocki_bind call failed");
      *dss_errno = DS_EINVAL;
      return DSS_ERROR;
    }

    /*-----------------------------------------------------------------------
      Call protocol specific function to complete the unbind.
    -----------------------------------------------------------------------*/
    dssocki_send_ps_cmd_buffer( scb_ptr,
                                NULL,
                                NULL,
                                ps_cmd);
    return (DSS_SUCCESS);
  }

  /*-------------------------------------------------------------------------
    Convert ps_sockaddr to ps_sockaddr_in6.
  -------------------------------------------------------------------------*/
  if( dssocki_get_sockaddr_in6(&v6_sockaddr, localaddr, addrlen, dss_errno) ==
      DSS_ERROR)
  {
    DS_ERROR_LOG_1( "Invalid Local address: ERRNO %d", *dss_errno);
    PS_MEM_FREE(ps_cmd);
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Check to see that no other sockets are using this IP address/port
    combination for the particular protocol.  We do not support local
    address re-use.
  -------------------------------------------------------------------------*/
  if ( dssocki_is_addr_in_use(scb_ptr,
                              scb_ptr->protocol,
                              &v6_sockaddr) )
  {
    DS_ERROR_LOG_1("Port %d already in use",v6_sockaddr.ps_sin6_port);
    *dss_errno = DS_EADDRINUSE;
    PS_MEM_FREE((ps_cmd));
    return DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    Autoassign port if not set.
  -------------------------------------------------------------------------*/
  if (v6_sockaddr.ps_sin6_port == 0)
  {
    v6_sockaddr.ps_sin6_port = dssocki_getephem_port(scb_ptr->protocol);
  }

  localname.port = v6_sockaddr.ps_sin6_port;
  if(localaddr->ps_sa_family == (uint16)DSS_AF_INET)
  {
    ((struct ps_sockaddr_in*)localaddr)->ps_sin_port = localname.port;
  }
  else if(localaddr->ps_sa_family == (uint16)DSS_AF_INET6)
  {
    ((struct ps_sockaddr_in6*)localaddr)->ps_sin6_port = localname.port;
  }

  DS_MSG1(MSG_LEGACY_HIGH, "Binding to port %d", dss_ntohs(localname.port));

  if(-1 == dssocki_bind(scb_ptr, &v6_sockaddr))
  {
    *dss_errno = DS_EINVAL;
    PS_MEM_FREE(ps_cmd);
    return (DSS_ERROR);
  }

  /* Once Bind is successful, Call V6 Proto API to increament the ref cnt for 
     the v6 address used for binding. */
  if (localaddr->ps_sa_family == (uint16)DSS_AF_INET6 && 
      (-1 == ps_ip_addr_ipv6_priv_addr_inc_ref_cnt(&(v6_sockaddr.ps_sin6_addr))))
  {
    DS_MSG0(MSG_LEGACY_ERROR, "Error during ref count increament.");
  }
 
  /*-------------------------------------------------------------------------
    Call protocol specific function to complete the bind.
  -------------------------------------------------------------------------*/
  dssocki_send_ps_cmd_buffer( scb_ptr, NULL, &v6_sockaddr, ps_cmd);

  /*-------------------------------------------------------------------------
    Return result code.
  -------------------------------------------------------------------------*/
  return (DSS_SUCCESS);

} /* dss_bind() */


/*===========================================================================
FUNCTION DSS_CLOSE()

DESCRIPTION
  Non-blocking close of a socket.  Performs all necessary clean-up of data
  structures and frees the socket for re-use.  For TCP initiates the active
  close for connection termination.  Once TCP has closed, the DS_CLOSE_EVENT
  will become TRUE, and the application can call dss_close() again to free
  the socket for re-use.  UDP sockets also need to call this to
  clean-up the socket and free it for re-use.

DEPENDENCIES
  None.

RETURN VALUE
  Returns DSS_SUCCESS on success.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EWOULDBLOCK      operation would block - TCP close in progress
  DS_EBADF            invalid socket descriptor is specfied

SIDE EFFECTS
  Initiates active close for TCP connections.
===========================================================================*/
sint15 dss_close
(
  sint15 sockfd,                                      /* socket descriptor */
  sint15 *dss_errno                               /* error condition value */
)
{
  struct scb_s *scb_ptr;                    /* ptr to socket control block */
  struct scb_s *lscb_ptr;                   /* ptr to socket control block */
  sint15 result = DSS_SUCCESS;              /* error checking return value */
  dssocki_state_enum_type socket_state;
  int16       scb_free_flag = 0;
  dssocki_app_to_ps_cmd_type  * ps_cmd;
  struct socket localname;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_MSG1( MSG_LEGACY_MED," DSS_CLOSE called by Sock%d ", sockfd);

  /*-------------------------------------------------------------------------
    If the passed dss_errno address is NULL return ERROR.
  -------------------------------------------------------------------------*/
  if (dss_errno == NULL)
  {
    DS_ERROR_LOG_0("dss_errno is NULL");
    return(DSS_ERROR);
  }

  DS_MSG1(MSG_LEGACY_HIGH,"dss_close for socket %d",sockfd);

  *dss_errno = DSS_SUCCESS;

  /*-------------------------------------------------------------------------
    Convert socket descriptor to SCB
  -------------------------------------------------------------------------*/
  if ( (scb_ptr=dssocki_sockfdtoscb(sockfd)) == NULL)
  {
    *dss_errno = DS_EBADF;
    DS_ERROR_LOG_1( "dss_close: Bad sockfd %d", sockfd);
    return (DSS_ERROR);
  }
  DS_ENTER_CRIT_SECTION_NO_TASKLOCK(&global_ps_crit_section);

  ps_cmd = dssocki_alloc_ps_cmd_buffer(APP_TO_PS_SOCKET_CLOSE);
  if(NULL == ps_cmd)
  {
    DS_ERROR_LOG_0( "Command buffers could not be allocated");
    *dss_errno = DS_ENOMEM;
    DS_LEAVE_CRIT_SECTION_NO_TASKLOCK(&global_ps_crit_section);
    return DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    Modify the state flags to indicate that this socket can neither send
    nor receive anything now. Needs to be done in the application context.
  -------------------------------------------------------------------------*/
  DSSOCKI_SOCKET_CANTSENDMORE( scb_ptr );
  DSSOCKI_SOCKET_CANTRECVMORE( scb_ptr );

  /*-------------------------------------------------------------------------
    Obtain the socket state
  -------------------------------------------------------------------------*/
  socket_state = scb_ptr->socket_state;

  switch(socket_state)
  {
    case DSSOCKI_NULL:
      PS_MEM_FREE(ps_cmd);
      *dss_errno = DS_EBADF;
      result = DSS_ERROR;
      break;

    case DSSOCKI_INIT:
      dssocki_send_ps_cmd_buffer(scb_ptr, (void *)TRUE, NULL, ps_cmd);
      scb_free_flag = 1;
      result = DSS_SUCCESS;
      break;

    case DSSOCKI_LISTEN:

      PS_MEM_FREE(ps_cmd);
      while((lscb_ptr = scb_ptr->so_partial) != NULL)
      {
        if(DSS_ERROR ==
             dssocki_send_to_ps( lscb_ptr,
                                 (void *) TRUE,
                                  NULL,
                                  APP_TO_PS_SOCKET_CLOSE ) )
        {
          *dss_errno = DS_ENOMEM;
          DS_LEAVE_CRIT_SECTION_NO_TASKLOCK(&global_ps_crit_section);
          return DSS_ERROR;
        }

        scb_ptr->so_partial = lscb_ptr->so_partial;
        dssocki_freescb(lscb_ptr);
      }

      scb_free_flag = 1;
      result = DSS_SUCCESS;
      break;

    case DSSOCKI_OPENING:
    case DSSOCKI_OPEN:
      /*---------------------------------------------------------------------
        If it is a stream socket, send the close command to PS. This ensures
        that the close happens in the context of the PS task.
      ---------------------------------------------------------------------*/
      if ( scb_ptr->type == DSS_SOCK_STREAM )
      {
        /*-------------------------------------------------------------------
          If the SO_LINGER socket option is set and the timeout is
          non-zero then start the linger timer. The case of zero timeout is
          handled by the TCP code.
        -------------------------------------------------------------------*/
        if ( (IS_SOCKOPT_SET(scb_ptr->options_mask, (uint64)DSS_SO_LINGER) ||
                IS_SOCKOPT_SET(scb_ptr->options_mask, (uint64)DSS_SO_LINGER_RESET)) &&
	           scb_ptr->so_linger != 0 )
        {
          if( PS_TIMER_SUCCESS != ps_timer_start( scb_ptr->linger_timer,
                                                  (int64)scb_ptr->so_linger ))
          {
            DS_ERROR_LOG_0( "Cant start ps timer for so_linger");
            DS_ASSERT(0);
          }
        }

        if ( IS_SOCKOPT_SET(scb_ptr->options_mask, (uint64)DSS_SO_SILENT_CLOSE) ||
             ((IS_SOCKOPT_SET(scb_ptr->options_mask, (uint64)DSS_SO_LINGER) ||
              IS_SOCKOPT_SET(scb_ptr->options_mask, (uint64)DSS_SO_LINGER_RESET)) &&
              scb_ptr->so_linger == 0))
        {
          dssocki_send_ps_cmd_buffer(scb_ptr, (void *)TRUE, NULL, ps_cmd);
          scb_free_flag = 1;
          result = DSS_SUCCESS;
        }
        else
        {
          if(scb_ptr->socket_state != DSSOCKI_CLOSED)
          {
            dssocki_set_sock_state( scb_ptr, DSSOCKI_CLOSING );
          }

          dssocki_send_ps_cmd_buffer( scb_ptr, (void *)FALSE, NULL, ps_cmd);
          *dss_errno = DS_EWOULDBLOCK;
          result = DSS_ERROR;
        }

      }
      /*--------------------------------------------------------------------
        If it is a datagram socket, close the scb and send a command to ps
        to close the protocol conrtol block
      ---------------------------------------------------------------------*/
      else if ( scb_ptr->type == DSS_SOCK_DGRAM )
      {
        dssocki_send_ps_cmd_buffer(scb_ptr, (void *)TRUE, NULL, ps_cmd);
        scb_free_flag = 1;
        result = DSS_SUCCESS;
      }
      /*--------------------------------------------------------------------
        Unknown/unsupported socket type.
      ---------------------------------------------------------------------*/
      else
      {
        PS_MEM_FREE(ps_cmd);
        DS_ERROR_LOG_1("Unknown socket type: %d", scb_ptr->type);
        *dss_errno = DS_EBADF;
        result = DSS_ERROR;
        DS_ASSERT(0);
      }
      break;

    case DSSOCKI_CLOSING:
      /*-------------------------------------------------------------------
        If the socket is lingering then return SUCCESS right away.
        The socket will be closed in the background.
      -------------------------------------------------------------------*/
      if ( scb_ptr->state_flags & (uint16)DSSOCKI_LINGER )
      {
        /*-------------------------------------------------------------------
          Decrement ref cnt now as old acb_ptr is lost from here on.
        -------------------------------------------------------------------*/
        PS_MEM_FREE(ps_cmd);
        scb_ptr->acb_ptr->scb_cnt--;
        scb_ptr->acb_ptr = scb_ptr->linger_acb_ptr;
        result = DSS_SUCCESS;
      }
      else
      {
        if ( IS_SOCKOPT_SET(scb_ptr->options_mask, (uint64)DSS_SO_SILENT_CLOSE))
        {
          dssocki_send_ps_cmd_buffer(scb_ptr, (void *)TRUE, NULL, ps_cmd);
          scb_free_flag = 1;
          result = DSS_SUCCESS;
        }
        else
        {
          dssocki_send_ps_cmd_buffer(scb_ptr, (void *)FALSE, NULL, ps_cmd);
          *dss_errno = DS_EWOULDBLOCK;
          result = DSS_ERROR;
        }
      }
      break;

    case DSSOCKI_WAIT_FOR_CLOSE:
    case DSSOCKI_CLOSED:
      PS_MEM_FREE(ps_cmd);
      scb_free_flag = 1;
      result = DSS_SUCCESS;
      break;

    default:
      PS_MEM_FREE(ps_cmd);
      DS_ERROR_LOG_1("Unknown socket state: SOCKET_STATE %d", socket_state);
      result = DSS_ERROR;
      DS_ASSERT(0);
  } /* end of switch */

  DS_LEAVE_CRIT_SECTION_NO_TASKLOCK(&global_ps_crit_section);
  if(scb_free_flag == 1)
  {
    /*-------------------------------------------------------------------------
      Ensure that the socket is unbound to the local address.
    -------------------------------------------------------------------------*/
    (void) dssocki_bind(scb_ptr, NULL);
    dssocki_unlink_scb_from_pcb(scb_ptr);
    /*------------------------------------------------------------------------
    Before Freeing the SCB, check if socket was bound to V6 address and it is 
    not a multicast address then call API to decreament the ref count.
    ------------------------------------------------------------------------*/
    DSSOCKI_CACHE_GET_LOCAL(scb_ptr, localname);
    if(!PS_IN_IS_ADDR_MULTICAST(&(localname.address)) &&
        PS_IN6_IS_ADDR_V6(&(localname.address)))
    {
       if (-1 == ps_ip_addr_ipv6_priv_addr_dec_ref_cnt(&(localname.address)))
       {
         DS_MSG0(MSG_LEGACY_ERROR, "Error during ref count decreament.");
       }
    }

    dssocki_freescb(scb_ptr);
  }

  return (result);
}  /* dss_close() */



/*===========================================================================
FUNCTION DSS_NETSTATUS()

DESCRIPTION
  Provides status of network subsystem.  Called in response to DS_ENETDOWN
  errors.  Note that origination status is based on the last attempted
  origination.

DEPENDENCIES
  None.

RETURN VALUE

  Returns DSS_ERROR and places the error condition value in *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADAPP               invalid application ID specified
  DS_ENETNONET             network subsystem unavailable for some unknown
                           reason
  DS_ENETISCONN            network subsystem is connected and available
  DS_ENETINPROGRESS        network subsystem establishment currently in
                           progress
  DS_ENETCLOSEINPROGRESS   network subsystem close in progress.

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_netstatus
(
  sint15 dss_nethandle,                                  /* application ID */
  sint15 *dss_errno                               /* error condition value */
)
{
  acb_type               *acb_ptr = NULL;         /* ptr to application control block structure */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Check that a valid ptr to a dss_errno variable is specified
  -------------------------------------------------------------------------*/
  if (dss_errno == NULL)
  {
    DS_ERROR_LOG_0( "dss_errno is NULL");
    return(DSS_ERROR);
  }

  DS_MSG1(MSG_LEGACY_HIGH," dss_netstatus() called by App %d", dss_nethandle);

  /*-------------------------------------------------------------------------
    Invalid application ID
  -------------------------------------------------------------------------*/
  if ((acb_ptr = dssocki_dss_nethandletoacb(dss_nethandle)) == NULL)
  {
    DS_MSG0(MSG_LEGACY_HIGH,"NULL ACB indicates clean up is successful and call,"
                            " does not exist, return DS_ENETNONET");
    *dss_errno = DS_ENETNONET;
    return (DSS_ERROR);
  }

   if(acb_ptr->fw_inst == NULL)
  {
    DS_ERROR_LOG_0("DS_FWK instance is  NULL, return ENETNONET");
    *dss_errno = DS_ENETNONET;
    return(DSS_ERROR);
  }
  else
  {
    DS_MSG0(MSG_LEGACY_HIGH,"DS_FWK instance is valid, return ENETISCONN");
    *dss_errno = DS_ENETISCONN;
     return DSS_SUCCESS;	
  }

  return (DSS_ERROR);

} /* dss_netstatus() */



/*===========================================================================
FUNCTION DSS_ASYNC_SELECT()

DESCRIPTION
  Enables the events to be notified about through the asynchronous
  notification mechanism.  Application specifies a bitmask of events that it
  is interested in, for which it will receive asynchronous notification via
  its application callback function. This function also performs a real-time
  check to determine if any of the events have already occurred, and if so
  invokes the application callback.

DEPENDENCIES
  None.

RETURN VALUE

  Returns DSS_SUCCESS on success.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor is specfied

SIDE EFFECTS
  Sets the relevant event mask in the socket control block.  Will also
  notify the application via the callback function.
===========================================================================*/
sint31 dss_async_select
(
  sint15 sockfd,                                      /* socket descriptor */
  sint31 event_mask,                        /* bitmask of events to set */
  sint15 *dss_errno                               /* error condition value */
)
{
  struct scb_s *scb_ptr;                    /* Ptr to socket control block */
  uint32        event_happened_mask;               /* event happenend mask */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Check that a valid ptr to a dss_errno variable is specified
  -------------------------------------------------------------------------*/
  if (dss_errno == NULL)
  {
    DS_ERROR_LOG_0( "dss_errno is NULL");
    return(DSS_ERROR);
  }
  *dss_errno = DSS_SUCCESS;

  DS_MSG2( MSG_LEGACY_HIGH, " In dss_async_select(): sockfd=%d, event_mask=%d",
          sockfd, event_mask);

  /*-------------------------------------------------------------------------
    Convert socket descriptor to SCB
  -------------------------------------------------------------------------*/
  if ( (scb_ptr=dssocki_sockfdtoscb(sockfd)) == NULL)
  {
    *dss_errno = DS_EBADF;
    return (DSS_ERROR);
  }

  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  /*-------------------------------------------------------------------------
    Reset the notified callback flag, so that the callback can be called
    again.
  -------------------------------------------------------------------------*/
  if(scb_ptr->acb_ptr != NULL)
  {
    scb_ptr->acb_ptr->notified_cb_flag = FALSE;
  }
  /*-------------------------------------------------------------------------
    Update requested event mask for the socket. Add it to existing requested
    mask.
  -------------------------------------------------------------------------*/
  scb_ptr->requested_event_mask |= event_mask;

  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  /*-------------------------------------------------------------------------
    Do a realtime check to see if any of the events are true. If there are
    any active events, call the callbacks.
  -------------------------------------------------------------------------*/
  event_happened_mask = (uint32)
    dssocki_socket_event_check( scb_ptr, scb_ptr->requested_event_mask);
  if (event_happened_mask != 0)
  {
    dssocki_notify_app( scb_ptr, event_happened_mask );
  }

  return (DSS_SUCCESS);

} /* dss_async_select() */



/*===========================================================================
FUNCTION DSS_ASYNC_DESELECT()

DESCRIPTION
  Clears events of interest in the socket control block interest mask.  The
  application specifies a bitmask of events that it wishes to clear; events
  for which it will no longer receive notification.

DEPENDENCIES
  None.

RETURN VALUE
  Returns DSS_SUCCESS on success.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor is specfied


SIDE EFFECTS
  Clears specified events from the relevant event mask.
===========================================================================*/
sint15 dss_async_deselect
(
  sint15 sockfd,                                      /* socket descriptor */
  sint31 event_mask,                         /* bitmask of events to clear */
  sint15 *dss_errno                               /* error condition value */
)
{
  struct scb_s *scb_ptr;                    /* Ptr to socket control block */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Check that a valid ptr to a dss_errno variable is specified
  -------------------------------------------------------------------------*/
  if (dss_errno == NULL)
  {
    DS_ERROR_LOG_0( "dss_errno is NULL");
    return(DSS_ERROR);
  }

  DS_MSG2( MSG_LEGACY_HIGH, " In dss_async_select(): sockfd=%d, event_mask=%d",
          sockfd, event_mask);

  *dss_errno = DSS_SUCCESS;

  /*-------------------------------------------------------------------------
    Convert socket descriptor to SCB
  -------------------------------------------------------------------------*/
  if ( (scb_ptr=dssocki_sockfdtoscb(sockfd)) == NULL)
  {
    *dss_errno = DS_EBADF;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Update requested event mask for the socket. Clear out any of the bits
    specified.
  -------------------------------------------------------------------------*/

  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);
  scb_ptr->requested_event_mask &= ~((uint32)event_mask);
  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  return (DSS_SUCCESS);
} /* dss_async_deselect() */


/*===========================================================================
FUNCTION DSS_WRITE()

DESCRIPTION
  Writes upto nbytes of data from buffer over the transport specified
  by the socket descriptor.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes written, which could be less than the number of
      bytes specified.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor is specfied
  DS_ENOTCONN         socket not connected
  DS_ECONNRESET       TCP connection reset by server
  DS_ECONNABORTED     TCP connection aborted due to timeout or other failure
  DS_EIPADDRCHANGED   IP address changed, causing TCP connection reset
  DS_EPIPE            broken pipe
  DS_EADDRREQ         destination address required
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           bad memory address
  DS_EWOULDBLOCK      operation would block
  DS_EINVAL           can't write to a listen socket

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_write
(
  sint15 sockfd,                                      /* socket descriptor */
  const void *buffer,               /* user buffer from which to copy data */
  uint16 nbytes,                /* number of bytes to be written to socket */
  sint15 *dss_errno                               /* error condition value */
)
{

  struct dss_msghdr     msg;  /* Header containing data and dest addr      */
  struct ps_iovec          iov;  /* encapsulated data array                   */
  int                   flags = 0 ; /* local flags field for dss_sendmsg   */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_MSG2( MSG_LEGACY_HIGH, "dss_write: fd=%d, nbytes=%d", sockfd, nbytes);

  iov.ps_iov_base = (byte *)buffer;
  iov.ps_iov_len  = nbytes;

  msg.msg_name       = NULL;
  msg.msg_namelen    = 0;
  msg.msg_iov        = & iov;
  msg.msg_iovlen     = 1;
  msg.msg_control    = NULL;
  msg.msg_controllen = 0;
  msg.msg_flags      = 0;

  return dssocki_sendmsg(sockfd, &msg, NULL, flags, dss_errno);
} /* dss_write() */



/*===========================================================================
FUNCTION DSS_WRITEV()

DESCRIPTION
  Provides the gather write variant of the dss_write() call, which
  allows the application to write from non-contiguous buffers.    Sends
  specified number of bytes in the buffer over the TCP transport.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes written, which could be less than the number of
      bytes specified.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor is specfied
  DS_ENOTCONN         socket not connected
  DS_ECONNRESET       TCP connection reset by server
  DS_ECONNABORTED     TCP connection aborted due to timeout or other failure
  DS_EIPADDRCHANGED   IP address changed, causing TCP connection reset
  DS_EPIPE            broken pipe
  DS_EADDRREQ         destination address required
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           bad memory address
  DS_EWOULDBLOCK      operation would block
  DS_EINVAL           Cant write to a listen socket

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_writev
(
  sint15 sockfd,                                      /* socket descriptor */
  struct ps_iovec iov[],     /* array of data buffers from which to copy data */
  sint15 iovcount,                                /* number of array items */
  sint15 *dss_errno                               /* error condition value */
)
{

  struct dss_msghdr     msg;  /* Msg header containing data and destaddr   */
  int                   flags = 0; /* flags for dss_sendmsg call           */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_MSG2( MSG_LEGACY_HIGH, "dss_writev: fd=%d, iovcount=%d", sockfd, iovcount);
  msg.msg_name         = NULL;
  msg.msg_namelen      = 0;
  msg.msg_iov          = iov;
  msg.msg_iovlen       = (uint16)iovcount;
  msg.msg_control      = NULL;
  msg.msg_controllen   = 0;
  msg.msg_flags        = 0;

  return dssocki_sendmsg(sockfd, &msg, NULL, flags, dss_errno);
} /* dss_writev() */



/*===========================================================================
FUNCTION DSS_READ()

DESCRIPTION
  Reads upto nbytes of data into buffer from the transport specified
  by the socket descriptor

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes read, which could be less than the number of
      bytes specified. A return of 0 indicates that an End-of-File condition
      has occurred.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor is specfied
  DS_ENOTCONN         socket not connected
  DS_ECONNRESET       TCP connection reset by server
  DS_ECONNABORTED     TCP connection aborted due to timeout or other failure
  DS_EIPADDRCHANGED   IP address changed, causing TCP connection reset
  DS_EPIPE            broken pipe
  DS_EADDRREQ         destination address required
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           bad memory address
  DS_EWOULDBLOCK      operation would block
  DS_EINVAL           Can't read a listen socket.

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_read
(
  sint15 sockfd,                                      /* socket descriptor */
  void   *buffer,                     /* user buffer to which to copy data */
  uint16 nbytes,                 /* number of bytes to be read from socket */
  sint15 *dss_errno                               /* error condition value */
)
{
  struct dss_msghdr     msg; /* msg header containing data and dest addr   */
  int                   flags = 0; /* flags for dss_recvmsg                */
  struct ps_iovec          iov;  /* encapsulated data array                   */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_MSG2(MSG_LEGACY_HIGH, "dss_read: fd=%d, nbytes=%d", sockfd, nbytes );

  iov.ps_iov_base = (byte *) buffer;
  iov.ps_iov_len  = nbytes;

  msg.msg_name       = NULL;
  msg.msg_namelen    = 0;
  msg.msg_iov        = &iov;
  msg.msg_iovlen     = 1;
  msg.msg_control    = NULL;
  msg.msg_controllen = 0;
  msg.msg_flags      = 0;

  return dssocki_recvmsg(sockfd, &msg, NULL, flags, dss_errno);
} /* dss_read() */



/*===========================================================================
FUNCTION DSS_READV()

DESCRIPTION
  Provides the scatter read variant of the dss_read() call, which
  allows the application to read into non-contiguous buffers.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes read, which could be less than the number of
      bytes specified. A return of 0 indicates that an End-of-File condition
      has occurred.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ------------
  DS_EBADF            invalid socket descriptor is specfied
  DS_ENOTCONN         socket not connected
  DS_ECONNRESET       TCP connection reset by server
  DS_ECONNABORTED     TCP connection aborted due to timeout or other failure
  DS_EIPADDRCHANGED   IP address changed, causing TCP connection reset
  DS_EPIPE            broken pipe
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           bad memory address
  DS_EWOULDBLOCK      operation would block
  DS_EINVAL           Can't read from a listen socket

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_readv
(
  sint15 sockfd,                                      /* socket descriptor */
  struct ps_iovec iov[],           /* array of data buffers to copy data into */
  sint15 iovcount,               /* number of bytes to be read from socket */
  sint15 *dss_errno                               /* error condition value */
)
{

  struct dss_msghdr     msg; /* msg header containing data and dest addr   */
  int                   flags = 0; /* flags for dss_recvmsg                */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_MSG2(MSG_LEGACY_HIGH, "dss_readv: fd=%d, iovcount=%d", sockfd, iovcount);
  msg.msg_name       = NULL;
  msg.msg_namelen    = 0;
  msg.msg_iov        = iov;
  msg.msg_iovlen     = (uint16)iovcount;
  msg.msg_control    = NULL;
  msg.msg_controllen = 0;
  msg.msg_flags      = 0;

  return dssocki_recvmsg(sockfd, &msg, NULL, flags, dss_errno);
} /* dss_readv() */



/*===========================================================================
FUNCTION DSS_SENDTO()

DESCRIPTION
  Sends 'nbytes' bytes in the buffer over the transport specified by the
  socket descriptor.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes to be written, which could be less than the number
      of bytes specified.

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

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_sendto
(
  sint15 sockfd,                /* socket descriptor                       */
  const void *buffer,           /* user buffer from which to copy the data */
  uint16 nbytes,                /* number of bytes to be written           */
  uint32 flags,                 /* used for SDB (if enabled)               */
  struct ps_sockaddr *toaddr,      /* destination address                     */
  uint16 addrlen,               /* address length                          */
  sint15 *dss_errno             /* error condition value                   */
)
{
  struct dss_msghdr     msg;   /* Header containing data and dest addr     */
  struct ps_iovec          iov;   /* encapsulated data array                  */

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DS_MSG3(MSG_LEGACY_HIGH, "dss_sendto: fd=%d, nbytes=%d, flags=0x%x",
          sockfd, nbytes, flags);

  iov.ps_iov_base = (byte *) buffer;
  iov.ps_iov_len  = nbytes;

  msg.msg_name       = toaddr;
  msg.msg_namelen    = addrlen;
  msg.msg_iov        = &iov;
  msg.msg_iovlen     = 1;
  msg.msg_control    = NULL;
  msg.msg_controllen = 0;
  msg.msg_flags      = 0;

  return dssocki_sendmsg(sockfd, &msg, NULL, (int)flags, dss_errno);
} /* dss_sendto() */



/*===========================================================================
FUNCTION DSS_RECVFROM()

DESCRIPTION
  Reads 'nbytes' bytes in the buffer from the transport specified by the
  socket descriptor.  Fills in address structure with values from who sent
  the data.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes to be written, which could be less than the number
      of bytes specified.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

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
sint15 dss_recvfrom
(
  sint15 sockfd,                                      /* socket descriptor */
  void   *buffer,               /* user buffer from which to copy the data */
  uint16 nbytes,                          /* number of bytes to be written */
  uint32 flags,                                                  /* unused */
  struct ps_sockaddr *fromaddr,                        /* destination address */
  uint16 *addrlen,                                       /* address length */
  sint15 *dss_errno                               /* error condition value */
)
{
  struct dss_msghdr     msg; /* Header containing data and dest address    */
  struct ps_iovec          iov; /* Data encapsulation array                   */
  int                   bytes_read; /* Bytes read                          */
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DS_MSG3(MSG_LEGACY_HIGH,  "dss_recvfrom: fd=%d, nbytes=%d, flags=0x%x",
           sockfd,
           nbytes,
           flags);

  iov.ps_iov_base = (byte *) buffer;
  iov.ps_iov_len = nbytes;

  msg.msg_name       = fromaddr;
  msg.msg_namelen    = (addrlen != NULL) ? *addrlen : 0;
  msg.msg_iov        = &iov;
  msg.msg_iovlen     = 1;
  msg.msg_control    = NULL;
  msg.msg_controllen = 0;
  msg.msg_flags      = 0;

  bytes_read = dssocki_recvmsg(sockfd, &msg, NULL, (int)flags, dss_errno);

  if(addrlen != NULL)
  {
    *addrlen = msg.msg_namelen;
  }

  return (int16)bytes_read;
} /* dss_recvfrom() */

/*===========================================================================
FUNCTION DSS_RECVMSG()
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
  *dss_errno.

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
sint15 dss_recvmsg
(
  sint15                  sockfd,   /* socket descriptor                   */
  struct dss_msghdr     * msg,      /* Message header for filling in data  */
  int                     flags,    /* flags from dss_recvfrom             */
  sint15                * dss_errno /* error condition value               */
)
{
  /*-------------------------------------------------------------------------
    There must be a valid msg ptr and dss_errno. Prevents dereferencing NULL
    ptrs.
  -------------------------------------------------------------------------*/
  if (dss_errno == NULL)
  {
    DS_ERROR_LOG_0( "DSS_RECVMSG : NULL pointer passed for dss_errno");
    return(DSS_ERROR);
  }

  if(msg == NULL)
  {
    DS_ERROR_LOG_0( "There must be a valid msg ptr passed to dss_recvmsg.");
    *dss_errno = DS_EFAULT;
    return(DSS_ERROR);
  }

  return dssocki_recvmsg(sockfd, msg, NULL, flags, dss_errno);
} /* dss_recvmsg() */

/*===========================================================================
FUNCTION DSS_SENDMSG()

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

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_sendmsg
(
  sint15                  sockfd,  /* socket descriptor                    */
  struct dss_msghdr     * msg,     /* Header containing data and dest addr */
  int                     flags,   /* flags used for SDB (if enabled)      */
  sint15                * dss_errno /* error condition value               */
)
{
  /*-------------------------------------------------------------------------
    There must be a valid msg ptr and dss_errno. Prevents dereferencing NULL
    ptrs.
  -------------------------------------------------------------------------*/
  if (dss_errno == NULL)
  {
    DS_ERROR_LOG_0( "DSS_SENDMSG : NULL pointer passed for dss_errno");
    return(DSS_ERROR);
  }

  if(msg == NULL)
  {
    DS_ERROR_LOG_0( "There must be a valid msg ptr passed to dss_sendmsg.");
    *dss_errno = DS_EFAULT;
    return(DSS_ERROR);
  }

  return dssocki_sendmsg(sockfd, msg, NULL, flags, dss_errno);
}


/*===========================================================================
FUNCTION DSS_SETSOCKOPT()

DESCRIPTION
  Set the options associated with a socket. This fuction expects the
  following parameters:

DEPENDENCIES
  None.

PARAMETERS
  int sockfd        -     Socket file descriptor.
  int level         -     Socket option level.
  int optname,      -     Option name.
  void *optval      -     Pointer to the option value.
  uint32 *optlen    -     Pointer to the size of the option value.
  sint15 *errno     -     Error condition value.

RETURN VALUE
  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF                invalid socket descriptor is specfied
  DS_ENOPROTOOPT          the option is unknown at the level indicated
  DS_EINVAL               invalid option name or invalid option value
  DS_EFAULT               Invalid buffer or argument

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_setsockopt
(
  int sockfd,                            /* socket descriptor              */
  int level,                             /* socket option level            */
  int optname,                           /* option name                    */
  void *optval,                          /* value of the option            */
  uint32 *optlen,                        /* size of the option value       */
  sint15 *dss_errno                      /* error condition value          */
)
{
  struct scb_s *scb_ptr;                 /* Ptr to socket control block    */
  struct scb_s *lscb_ptr;                /* local scb_ptr for traversing   */

#ifdef FEATURE_DATA_PS_QOS
  uint32                    share_handle;
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Check that a valid ptr to a dss_errno variable is specified
  -------------------------------------------------------------------------*/
  if(dss_errno == NULL)
  {
    DS_ERROR_LOG_0("dss_errno is NULL");
    return(DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Assert all arguments are valid - Prevents dereferencing of NULL ptrs.
  -------------------------------------------------------------------------*/
  if((optval == NULL) || (optlen == NULL) )
  {
    DS_ERROR_LOG_2( "Invalid optlen=0x%x or optval=0x%x", optlen, optval);
    *dss_errno = DS_EFAULT;
    return(DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Initialize dss_errno.
  -------------------------------------------------------------------------*/
  *dss_errno = DSS_SUCCESS;

  /*-------------------------------------------------------------------------
    Invalid socket descriptor
  -------------------------------------------------------------------------*/
  if((scb_ptr = dssocki_sockfdtoscb(sockfd)) == NULL)
  {
    *dss_errno = DS_EBADF;
    return (DSS_ERROR);
  }

  DS_MSG3(MSG_LEGACY_HIGH,"dss_setsockopt :Opt Name %d, opt value %u,"
          " sockfd %d",optname,(*(int*)optval),scb_ptr->sockfd);

  /*-------------------------------------------------------------------------
    If socket state is higher than CLOSING then don't set the option.
  -------------------------------------------------------------------------*/
  if(scb_ptr->socket_state > DSSOCKI_CLOSING)
  {
    *dss_errno = DS_EBADF;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Check if the optname is valid to prevent illegal indexings into the
    socket option table.
  -------------------------------------------------------------------------*/
  if ((optname <= (int)DSS_SOCKOPT_MIN) || (optname >= (int)DSS_SOCKOPT_MAX))
  {
    *dss_errno = DS_EINVAL;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Check level/option correspondence. Return DS_ENOPROTOOPT if the
    requested option is unknown at the level specified, or if the level
    if unknown.
  -------------------------------------------------------------------------*/
  if (level != (int)(dss_sock_opts_default[optname].level))
  {
    *dss_errno = DS_ENOPROTOOPT;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Check if the socket belongs to desired option level.
  -------------------------------------------------------------------------*/
  switch (level)
  {
    case DSS_IPPROTO_IP:
      /*---------------------------------------------------------------------
        IPv4 options can be set on any type of socket (DSS_AF_INET or DSS_AF_INET6)
      ---------------------------------------------------------------------*/
      break;


    case DSS_IPPROTO_IPV6:
      if ((byte)DSS_AF_INET6 != scb_ptr->family)
      {
        DS_ERROR_LOG_2( "Option, %d, can't be set for a %d socket",
                  optname,
                  scb_ptr->family);
        *dss_errno = DS_ENOPROTOOPT;
        return DSS_ERROR;
      }

      break;


    case DSS_IPPROTO_TCP:
      if ((byte)PS_IPPROTO_TCP != scb_ptr->protocol)
      {
         DS_ERROR_LOG_2( "Option, %d, can't be set for a %d socket",
                  optname,
                  scb_ptr->protocol);
        *dss_errno = DS_ENOPROTOOPT;
        return DSS_ERROR;
      }

      break;

    case DSS_SOCK:
      break;

    case DSS_IPPROTO_ICMP:
    case DSS_IPPROTO_ICMP6:
      if ((byte)PS_IPPROTO_ICMP != scb_ptr->protocol && (byte)PS_IPPROTO_ICMP6 != scb_ptr->protocol)
      {
         DS_ERROR_LOG_2( "Option, %d, can't be set for a %d socket",
                  optname,
                  scb_ptr->protocol);
        *dss_errno = DS_ENOPROTOOPT;
        return DSS_ERROR;
      }
      break;

    default:
       DS_ERROR_LOG_1("Invalid option level(%d)",level);
      *dss_errno = DS_EINVAL;
      return DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    Validate the value
  -------------------------------------------------------------------------*/
  switch (dss_sock_opts_default[optname].optvaltype)
  {
    case DSSI_SO_BOOL:
      /*---------------------------------------------------------------------
        Size checking
      ---------------------------------------------------------------------*/
      if (*optlen < dss_sock_opts_default[optname].optlen)
      {
        *dss_errno = DS_EINVAL;
        return DSS_ERROR;
      }
      /*---------------------------------------------------------------------
        We need not do any range checking for boolean types. Any non-zero
        value is treated as TRUE and zero as FALSE
      ---------------------------------------------------------------------*/
      break;

    case DSSI_SO_INT:
      /*---------------------------------------------------------------------
         Size checking
      ---------------------------------------------------------------------*/
      if (*optlen < dss_sock_opts_default[optname].optlen)
      {
        *dss_errno = DS_EINVAL;
        return DSS_ERROR;
      }
      /*---------------------------------------------------------------------
         Range checking.
      ---------------------------------------------------------------------*/
      if(((*(int*)optval) < dss_sock_opts_default[optname].minval) ||
         ((*(int*)optval) > dss_sock_opts_default[optname].maxval ))
      {
        *dss_errno = DS_EINVAL;
        return (DSS_ERROR);
      }
      break;

    case DSSI_SO_LINGER:
    case DSSI_SO_NETPOLICY_TYPE:
    case DSSI_SO_IFACE_ID_TYPE:
    case DSSI_SO_CB_FCN_TYPE:
      /*---------------------------------------------------------------------
         Size checking
      ---------------------------------------------------------------------*/
      if (*optlen != dss_sock_opts_default[optname].optlen)
      {
        *dss_errno = DS_EINVAL;
        return DSS_ERROR;
      }

      /*---------------------------------------------------------------------
        No range checking needed
      ---------------------------------------------------------------------*/
      break;

    default:
      /*---------------------------------------------------------------------
        Option value type not supported.
      ---------------------------------------------------------------------*/
      *dss_errno = DS_ENOPROTOOPT;
      return (DSS_ERROR);
  } /* switch(optval type) */

  if ( level == (int)DSS_SOL_SOCKET )
  {
    switch (optname)
    {
      case DSS_SO_SYS_SOCK:
        return dssocki_cache_setopt( scb_ptr,
                                     level,
                                     optname,
                                     optval,
                                     optlen,
                                     dss_errno);
      case DSS_SO_SILENT_CLOSE:
        if (*((int *)optval))
        {
          SOCKOPT_SET(scb_ptr->options_mask, optname);
        }
        else
        {
          SOCKOPT_RESET(scb_ptr->options_mask, optname);
        }
        break;

      case DSS_SO_RCVBUF:
        return dssocki_cache_setopt( scb_ptr,
                                     level,
                                     optname,
                                     optval,
                                     optlen,
                                     dss_errno);

      case DSS_SO_SNDBUF:
        scb_ptr->sndbuf = *((unsigned int *) optval);
        break;

      case DSS_SO_LINGER:

        if(IS_SOCKOPT_SET(scb_ptr->options_mask, (uint64)DSS_SO_LINGER_RESET))
        {
          *dss_errno = DS_EINVAL;
          return DSS_ERROR;
        }
        if ( ((dss_so_linger_type_s*)optval)->l_onoff )
        {
          SOCKOPT_SET(scb_ptr->options_mask, optname);
        }
        else
        {
          SOCKOPT_RESET(scb_ptr->options_mask, optname);
        }
        scb_ptr->so_linger = ((dss_so_linger_type_s *)optval)->l_linger;
        if (scb_ptr->so_linger != 0)
        {
          if (DSS_SUCCESS != dssocki_set_linger_timer(scb_ptr,dss_errno))
          {
            return DSS_ERROR;
          }
        }
        break;

      case DSS_SO_LINGER_RESET:
        if(IS_SOCKOPT_SET(scb_ptr->options_mask, (uint64)DSS_SO_LINGER))
        {
          *dss_errno = DS_EINVAL;
          return DSS_ERROR;
        }
        if ( ((dss_so_linger_type_s *)optval)->l_onoff )
        {
          SOCKOPT_SET(scb_ptr->options_mask, optname);
        }
        else
        {
          SOCKOPT_RESET(scb_ptr->options_mask, optname);
        }
        scb_ptr->so_linger = ((dss_so_linger_type_s *)optval)->l_linger;

        if (scb_ptr->so_linger != 0)
        {
          if (DSS_SUCCESS != dssocki_set_linger_timer(scb_ptr,dss_errno))
          {
            return DSS_ERROR;
          }
        }
        break;

      case DSS_SO_KEEPALIVE:
        return dssocki_cache_setopt( scb_ptr,
                                     level,
                                     optname,
                                     optval,
                                     optlen,
                                     dss_errno);
        break;

      case DSS_SO_CB_FCN:
        if(scb_ptr->acb_ptr != NULL)
        {
          /*--------------------------------------------------------------
            Socket not created using socket2 function call.
          --------------------------------------------------------------*/
          *dss_errno = DS_EOPNOTSUPP;
          return (DSS_ERROR);
        }
        DS_ENTER_CRIT_SECTION_NO_TASKLOCK(&global_ps_crit_section);
        scb_ptr->sock_cb =
          ((dss_sock_cb_fcn_type_s *)optval)->sock_cb_fcn;
        scb_ptr->sock_cb_user_data =
          ((dss_sock_cb_fcn_type_s *)optval)->sock_cb_user_data;
        DS_LEAVE_CRIT_SECTION_NO_TASKLOCK(&global_ps_crit_section);
        break;

      case DSS_SO_ERROR_ENABLE:
      case DSS_SO_DISABLE_FLOW_FWDING:
        if( *((int *) optval))
        {
          SOCKOPT_SET(scb_ptr->options_mask, optname);
        }
        else
        {
          SOCKOPT_RESET(scb_ptr->options_mask, optname);
        }
        break;

#ifdef FEATURE_DATA_PS_QOS
      case DSS_SO_QOS_SHARE_HANDLE:
        if(scb_ptr->acb_ptr == NULL)
        {
          share_handle = *((unsigned int *) optval);
          scb_ptr->qos_subset_id = (share_handle);
          DS_MSG2(MSG_LEGACY_HIGH, "Setting DSS_SO_QOS_SHARE_HANDLE to %d on sockfd %d",
                   scb_ptr->qos_subset_id, sockfd);
        }
        else
        {
          DS_ERROR_LOG_0("QOS_SHARE_HANDLE option: invalid for this kind of socket");
          *dss_errno = DS_EINVAL;
          return DSS_ERROR;
        }
        break;
#endif /* FEATURE_DATA_PS_QOS */

      case DSS_SO_REUSEADDR:
        if (scb_ptr->protocol == (byte)PS_IPPROTO_TCP)
        {
           DS_ERROR_LOG_0("DSS_SO_REUSEADDR cannot be set for TCP socket");
          *dss_errno = DS_ENOPROTOOPT;
          return DSS_ERROR;
        }
        if (*((int *)optval))
        {
          SOCKOPT_SET(scb_ptr->options_mask, optname);
        }
        else
        {
          SOCKOPT_RESET(scb_ptr->options_mask, optname);
        }
        /*-------------------------------------------------------------------
          The port number check during BIND is done at SCB level. So there
          is no need to pass this socket option info to underlying PCB.
        -------------------------------------------------------------------*/
        break;

      default:
        /*-------------------------------------------------------------------
          Socket option not supported.
        -------------------------------------------------------------------*/
        *dss_errno = DS_ENOPROTOOPT;
        return (DSS_ERROR);
    }
  } /* if ( level == DSS_SOL_SOCKET ) */
  else
  {
    /*-------------------------------------------------------------------
      For Listening TCP sockets, set the option for all the tcp sockets in
      partial queue.
    -------------------------------------------------------------------*/
    if(scb_ptr->socket_state == DSSOCKI_LISTEN)
    {
      lscb_ptr = scb_ptr->so_partial;
      while(lscb_ptr != NULL)
      {
        (void)
        dssocki_cache_setopt( lscb_ptr,
                              level,
                              optname,
                              optval,
                              optlen,
                              dss_errno);
        lscb_ptr = lscb_ptr->so_partial;
      }
      return DSS_SUCCESS;
    }

    return dssocki_cache_setopt( scb_ptr,
                                 level,
                                 optname,
                                 optval,
                                 optlen,
                                 dss_errno);
  }

  return (DSS_SUCCESS);
} /* dss_setsockopt() */



/*===========================================================================
FUNCTION DSS_GETSOCKOPT()

DESCRIPTION
  Return an option associated with a socket. This fuction expects the
  following parameters:

DEPENDENCIES
  None.

PARAMETERS
  int sockfd        -     Socket file descriptor.
  int level         -     Socket option level.
  int optname,      -     Option name.
  void *optval      -     Pointer to the option value.
  uint32 *optlen    -     Pointer to the size of the option value.
  sint15 *errno     -     Error condition value.

RETURN VALUE
  optlen is a value-result parameter, initially containing the size of
  the buffer pointed to by optval, and modified on return to indicate the
  actual  size  of the value returned. On error, return DSS_ERROR and places
  the error condition value in *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF                invalid socket descriptor is specfied
  DS_ENOPROTOOPT          the option is unknown at the level indicated
  DS_EINVAL               invalid option name or invalid option value
  DS_EFAULT               Invalid buffer or argument

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_getsockopt
(
  int sockfd,                            /* socket descriptor              */
  int level,                             /* socket option level            */
  int optname,                           /* option name                    */
  void *optval,                          /* value of the option            */
  uint32 *optlen,                        /* size of the option value       */
  sint15 *dss_errno                      /* error condition value          */
)
{
  struct scb_s *scb_ptr;                    /* Ptr to socket control block */
  struct icmp_cb *icmp_cb_ptr = NULL;
  struct udp_cb  *ucb_ptr = NULL;
  struct tcb *tcb_ptr = NULL; 
  dsm_item_type *item_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Check that a valid ptr to a dss_errno variable is specified
  -------------------------------------------------------------------------*/
  if(dss_errno == NULL)
  {
    DS_ERROR_LOG_0( "dss_errno is NULL");
    return(DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Assert all arguments are valid - Prevents dereferencing of NULL ptrs.
  -------------------------------------------------------------------------*/
  if((optval == NULL) || (optlen == NULL))
  {
    DS_ERROR_LOG_2( "Invalid optlen=0x%x or optval=0x%x", optlen,optval );
    *dss_errno = DS_EFAULT;
    return(DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Initialize dss_errno.
  -------------------------------------------------------------------------*/
  *dss_errno = DSS_SUCCESS;

  /*-------------------------------------------------------------------------
    Invalid socket descriptor
  -------------------------------------------------------------------------*/
  if((scb_ptr=dssocki_sockfdtoscb(sockfd)) == NULL)
  {
    *dss_errno = DS_EBADF;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Check if the optname is valid to prevent illegal indexings into the
    socket option table.
  -------------------------------------------------------------------------*/
  if((optname <= (int)DSS_SOCKOPT_MIN) || (optname >= (int)DSS_SOCKOPT_MAX))
  {
    *dss_errno = DS_EINVAL;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Check level/option correspondence. Return DS_ENOPROTOOPT if the
    requested option is unknown at the level specified, or if the level
    if unknown.
  -------------------------------------------------------------------------*/
  if(level != (int)(dss_sock_opts_default[optname].level))
  {
    *dss_errno = DS_ENOPROTOOPT;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Check if the socket belongs to desired option level.
  -------------------------------------------------------------------------*/
  switch (level)
  {
    case DSS_IPPROTO_IP:
      if ((byte)DSS_AF_INET != scb_ptr->family)
      {
        DS_ERROR_LOG_2(  "Option, %d, is not supported for a %d socket",
                  optname,
                  scb_ptr->family);
        *dss_errno = DS_ENOPROTOOPT;
        return DSS_ERROR;
      }

      break;


    case DSS_IPPROTO_IPV6:
      if ((byte)DSS_AF_INET6 != scb_ptr->family)
      {
        DS_ERROR_LOG_2( "Option, %d, is not supported for a %d socket",
                  optname,
                  scb_ptr->family);
        *dss_errno = DS_ENOPROTOOPT;
        return DSS_ERROR;
      }

      break;


    case DSS_IPPROTO_TCP:
      if ((byte)PS_IPPROTO_TCP != scb_ptr->protocol)
      {
        DS_ERROR_LOG_2( "Option, %d, is not supported for a %d socket",
                  optname,
                  scb_ptr->protocol);
        *dss_errno = DS_ENOPROTOOPT;
        return DSS_ERROR;
      }

      break;

    case DSS_SOCK:
      break;

    default:
      DS_ERROR_LOG_1("Invalid option level(%d)", level);
      *dss_errno = DS_EINVAL;
      return DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    Returning the queried option.  optlen is a value-result
    parameter, initially containing the size of the buffer pointed to
    by optval, and modified on return to indicate the  actual  size  of
    the value returned.
  -------------------------------------------------------------------------*/
  switch(dss_sock_opts_default[optname].optvaltype)
  {
    case DSSI_SO_BOOL:
    case DSSI_SO_INT:
      /*---------------------------------------------------------------------
         Integer or boolean type socket option value.
      ---------------------------------------------------------------------*/
      if ( *optlen < dss_sock_opts_default[optname].optlen )
      {
        *dss_errno = DS_EINVAL;
        return (DSS_ERROR);
      }
      else
      {
        *optlen = dss_sock_opts_default[optname].optlen;
      }
      break;

    case DSSI_SO_LINGER:
    case DSSI_SO_NETPOLICY_TYPE:
    case DSSI_SO_IFACE_ID_TYPE:
    case DSSI_SO_CB_FCN_TYPE:
      /*---------------------------------------------------------------------
        The specified optlen should be same as the optlen stored in the
        opts_default.
      ---------------------------------------------------------------------*/
      if ( *optlen != dss_sock_opts_default[optname].optlen )
      {
        *dss_errno = DS_EINVAL;
        return (DSS_ERROR);
      }
      break;

    default:
      /*---------------------------------------------------------------------
        Socket option value type not supported.
      ---------------------------------------------------------------------*/
      *dss_errno = DS_ENOPROTOOPT;
      return (DSS_ERROR);
  } /* switch(optval type) */

  if( (int)DSS_SOL_SOCKET == level )
  {
    switch (optname)
    {
      case DSS_SO_SYS_SOCK:
      case DSS_SO_SILENT_CLOSE:
      case DSS_SO_KEEPALIVE:
      case DSS_SO_ERROR_ENABLE:
      case DSS_SO_DISABLE_FLOW_FWDING:
        *((int *) optval) = IS_SOCKOPT_SET(scb_ptr->options_mask, optname);
        break;

      case DSS_SO_RCVBUF:
        *((unsigned int*)optval) = scb_ptr->rcvbuf;
        break;

      case DSS_SO_SNDBUF:
        *((unsigned int *)optval) = scb_ptr->sndbuf;
        break;

      case DSS_SO_LINGER:
      case DSS_SO_LINGER_RESET:
        ((dss_so_linger_type_s *)optval)->l_onoff =
            IS_SOCKOPT_SET(scb_ptr->options_mask, optname);
        ((dss_so_linger_type_s *)optval)->l_linger = scb_ptr->so_linger;
        break;

      case DSS_SO_CB_FCN:
        if(scb_ptr->acb_ptr != NULL)
        {
          /*-----------------------------------------------------------------
            Socket not created using socket2 function call.
          -----------------------------------------------------------------*/
          *dss_errno = DS_EOPNOTSUPP;
          return (DSS_ERROR);
        }
        ((dss_sock_cb_fcn_type_s *)optval)->sock_cb_fcn = scb_ptr->sock_cb;
        ((dss_sock_cb_fcn_type_s *)optval)->sock_cb_user_data =
          scb_ptr->sock_cb_user_data;
        break;

      case DSS_SO_ERROR:
        if(IS_SOCKOPT_SET(scb_ptr->options_mask, (uint64)DSS_SO_ERROR_ENABLE))
        {
          DS_ENTER_CRIT_SECTION(&global_ps_crit_section);
          *((int *) optval) = scb_ptr->so_error;
          scb_ptr->so_error = 0;
          DSSOCKI_RESET_ERROR_AVAILABLE(scb_ptr);
          DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
        }
        else
        {
          *dss_errno = DS_EFAULT;
          return DSS_ERROR;
        }
        break;

#ifdef FEATURE_DATA_PS_QOS
      case DSS_SO_QOS_SHARE_HANDLE:
        if(scb_ptr->acb_ptr == NULL)
        {
          *((unsigned int *) optval) = (uint32)scb_ptr->qos_subset_id;
        }
        else
        {
          DS_MSG0(MSG_LEGACY_HIGH,
                  "QOS_SHARE_HANDLE option is invalid for this kind of "
                   "socket");
          *dss_errno = DS_EINVAL;
          return DSS_ERROR;
        }
        break;
#endif /* FEATURE_DATA_PS_QOS */

      case DSS_SO_REUSEADDR:
        if (scb_ptr->protocol == (byte)PS_IPPROTO_TCP)
        {
          DS_ERROR_LOG_0( "DSS_SO_REUSEADDR not valid for TCP socket");
          *dss_errno = DS_ENOPROTOOPT;
          return DSS_ERROR;
        }
        *((int *) optval) = IS_SOCKOPT_SET(scb_ptr->options_mask, optname);
        break;

      case DSS_SO_FIONREAD:
        if (scb_ptr->protocol == PS_IPPROTO_TCP)
        {
          tcb_ptr = scb_ptr->protocol_ctl_blk.tcb;
          if (tcb_ptr != NULL)
          {
            (*(uint32*)optval)= dsm_length_packet(scb_ptr->protocol_ctl_blk.tcb->rcvq);
          }
          else
          {
            *dss_errno = DS_EINVAL;
            return DSS_ERROR;
          }
        }
        else if (scb_ptr->protocol == PS_IPPROTO_UDP)
        {
          ucb_ptr =scb_ptr->protocol_ctl_blk.ucb;
          if (ucb_ptr != NULL)
          {
            item_ptr = (dsm_item_type *) q_check( &ucb_ptr->rcvq);
            while(item_ptr != NULL)
            {
              (*(uint32*)optval) += dsm_length_packet(item_ptr);
              (*(uint32*)optval) -= DSS_PROTO_UDP_HDR;
              item_ptr = (dsm_item_type *)q_next(&ucb_ptr->rcvq,
                                                 &item_ptr->link);
            }
          }
          else
          {
            *dss_errno = DS_EINVAL;
            return DSS_ERROR;
          }
        }
        else if (scb_ptr->protocol == PS_IPPROTO_ICMP || 
                 scb_ptr->protocol == PS_IPPROTO_ICMP6)
        {
          icmp_cb_ptr = scb_ptr->protocol_ctl_blk.icb;
          if (icmp_cb_ptr != NULL)
          {
            item_ptr = (dsm_item_type *) q_check( &icmp_cb_ptr->rcvq);
            while(item_ptr != NULL)
            {
              (*(uint32*)optval) += dsm_length_packet(item_ptr);
              (*(uint32*)optval) -= DSS_PROTO_ICMP_HDR;
              item_ptr = (dsm_item_type *)q_next(&icmp_cb_ptr->rcvq,
                                                 &item_ptr->link);
            }
          }
          else
          {
            *dss_errno = DS_EINVAL;
            return DSS_ERROR;
          }
        }
      break;

      default:
        /*-------------------------------------------------------------------
          Socket option not supported.
        -------------------------------------------------------------------*/
        *dss_errno = DS_ENOPROTOOPT;
        return (DSS_ERROR);
    }
  }
  else
  {
    return dssocki_cache_getopt(scb_ptr,level,optname,optval,optlen,dss_errno);
  }

  return (DSS_SUCCESS);

} /* dss_getsock_opt ()*/



/*===========================================================================
FUNCTION DSS_GETSOCKNAME

DESCRIPTION
  Returns the current local address assigned to the specified socket.

DEPENDENCIES
  None.

PARAMETERS
  sockfd    -  socket file descriptor
  addr      -  local address currently associated with the socket
  addrlen   -  address length. This parameter is initialized to indicate
               the amount of space pointed by addr and on return, it
               contains the actual size of the address returned.
  dss_errno -  error number

RETURN VALUE
  Returns DSS_SUCCESS upon successful completion and places the socket
  address and the address length in addr and addrlen parameters, resp.

  If the address is larger than the supplied buffer then it is silently
  truncated. The value returned in addrlen indicates the size prior to
  truncation, if any.

  On error, returns DSS_ERROR and places the error number in dss_errno.

  Errno Values
  ------------
  DS_EBADF                invalid socket descriptor is specfied
  DS_EFAULT               addr parameter points to an invalid memory
                          location

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_getsockname
(
  sint15           sockfd,                  /* socket descriptor           */
  struct ps_sockaddr* addr,                    /* address of the socket       */
  uint16*          addrlen,                 /* address length              */
  sint15*          dss_errno                /* error number                */
)
{
  struct scb_s *scb_ptr;                    /* ptr to socket control block */
  struct socket localname;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG1(MSG_LEGACY_HIGH, "dss_getsockname called, sockfd=%d", sockfd);
  /*-------------------------------------------------------------------------
    Check that a valid ptr to a dss_errno variable is specified
  -------------------------------------------------------------------------*/
  if (dss_errno == NULL)
  {
    DS_ERROR_LOG_0( "dss_errno is NULL");
    return(DSS_ERROR);
  }
  *dss_errno = DSS_SUCCESS;

  /*-------------------------------------------------------------------------
    Convert socket descriptor to SCB
  -------------------------------------------------------------------------*/
  if ( (scb_ptr=dssocki_sockfdtoscb(sockfd)) == NULL)
  {
    DS_ERROR_LOG_1( "Bad sockfd=%d", sockfd);
    *dss_errno = DS_EBADF;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Invalid local address specified
  -------------------------------------------------------------------------*/
  if ( addr == NULL )
  {
    DS_ERROR_LOG_0( "Bad memory address: addr is NULL");
    *dss_errno = DS_EFAULT;
    return ( DSS_ERROR );
  }

  /*-------------------------------------------------------------------------
    Invalid address length specified
  -------------------------------------------------------------------------*/
  if ( addrlen == NULL )
  {
    DS_ERROR_LOG_0( "Bad memory address: addrlen is NULL");
    *dss_errno = DS_EFAULT;
    return ( DSS_ERROR );
  }

  DSSOCKI_CACHE_GET_LOCAL(scb_ptr, localname);
  /*-------------------------------------------------------------------------
    Fill up the local address from the scb. Note that the scb stores the
    port, address in host byte order and it needs to be converted to network
    byte order.
  -------------------------------------------------------------------------*/
  dssocki_fill_sockaddr(addr, addrlen, localname,scb_ptr->family);

  return DSS_SUCCESS;

} /* dss_getsockname() */



/*===========================================================================
FUNCTION DSS_GETPEERNAME

DESCRIPTION
  Returns the address of the peer connected to the specified socket.

DEPENDENCIES
  None.

PARAMETERS
  sockfd    -  socket file descriptor
  addr      -  address of the peer connected with the socket
  addrlen   -  address length. This parameter is initialized to indicate
               the amount of space pointed by addr and on return, it
               contains the actual size of the address returned.
  dss_errno -  error number

RETURN VALUE
  Returns DSS_SUCCESS upon successful completion and places the peer
  address and the address length in addr and addrlen parameters, resp.

  If the address is larger than the supplied buffer then it is silently
  truncated. The value returned in addrlen indicates the size prior to
  truncation, if any.

  On error, returns DSS_ERROR and places the error number in dss_errno.

  Errno Values
  ------------
  DS_EBADF                invalid socket descriptor is specfied
  DS_EFAULT               addr parameter points to an invalid memory
                          location
  DS_ENOTCONN             the socket is not connected

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_getpeername
(
  sint15           sockfd,                  /* socket descriptor           */
  struct ps_sockaddr* addr,                    /* address of the socket       */
  uint16*          addrlen,                 /* address length              */
  sint15*          dss_errno                /* error number                */
)
{
  struct scb_s *scb_ptr;                    /* ptr to socket control block */
  struct socket servname;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG1( MSG_LEGACY_HIGH, "dss_getpeername called, sockfd=%d", sockfd);
  /*-------------------------------------------------------------------------
    Check that a valid ptr to a dss_errno variable is specified
  -------------------------------------------------------------------------*/
  if (dss_errno == NULL)
  {
    DS_ERROR_LOG_0( "dss_errno is NULL");
    return(DSS_ERROR);
  }
  *dss_errno = DSS_SUCCESS;

  /*-------------------------------------------------------------------------
    Convert socket descriptor to SCB
  -------------------------------------------------------------------------*/
  if ( (scb_ptr=dssocki_sockfdtoscb(sockfd)) == NULL)
  {
    DS_ERROR_LOG_1( "Bad sockfd=%d", sockfd);
    *dss_errno = DS_EBADF;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Invalid local address specified
  -------------------------------------------------------------------------*/
  if ( addr == NULL )
  {
    DS_ERROR_LOG_0( "Bad memory address: addr is NULL");
    *dss_errno = DS_EFAULT;
    return ( DSS_ERROR );
  }

  /*-------------------------------------------------------------------------
    Invalid address length specified
  -------------------------------------------------------------------------*/
  if ( addrlen == NULL )
  {
    DS_ERROR_LOG_0( "Bad memory address: addrlen is NULL");
    *dss_errno = DS_EFAULT;
    return ( DSS_ERROR );
  }

  DSSOCKI_CACHE_GET_REMOTE( scb_ptr, servname );

  /*-------------------------------------------------------------------------
    Check if it is a connected socket, i.e., it is a stream socket AND it
    is in OPEN state
  -------------------------------------------------------------------------*/
  if ( (scb_ptr->socket_state != DSSOCKI_OPEN)  ||
       (PS_IN6_ARE_ADDR_EQUAL(&servname.address, &ps_in6addr_any )) ||
       (servname.port    == 0) )
  {
    DS_ERROR_LOG_1( "Socket is not connected, sockfd=%d", sockfd);
    *dss_errno = DS_ENOTCONN;
    return ( DSS_ERROR );
  }

  /*-------------------------------------------------------------------------
    Fill up the peer address from the scb. Note that the scb stores the
    port, address in host byte order and it needs to be converted to network
    byte order.
  -------------------------------------------------------------------------*/
  dssocki_fill_sockaddr(addr,addrlen,servname,scb_ptr->family);

  return DSS_SUCCESS;
} /* dss_getpeername() */



/*===========================================================================
FUNCTION DSS_SHUTDOWN

DESCRIPTION
  Shuts down the connection of the specified socket depending on the
  'how' parameter as follows:

  DSS_SHUT_RD:   Disallow subsequent calls to recv function
  DSS_SHUT_WR:   Disallow subsequent calls to send function
  DSS_SHUT_RDWR: Disallow subseuqnet calls to both recv and send functions

DEPENDENCIES
  None.

PARAMETERS
  sockfd    -  socket file descriptor
  how       -  action to be performed: shutdown read-half, write-half or
               both
  dss_errno -  error number

RETURN VALUE
  In case of successful completion, returns DSS_SUCCESS. Otherwise, returns
  DSS_ERROR and places the error number in dss_errno.

  Errno Values
  ------------
  DS_EBADF                invalid socket descriptor is specfied
  DS_ENOTCONN             the socket is not connected
  DS_EINVAL               invalid operation (e.g., how parameter is invalid)
  DS_ENOMEM               insufficient memory available to complete the
                          operation

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_shutdown
(
  sint15           sockfd,                  /* socket descriptor           */
  uint16           how,                     /* what action to perform      */
  sint15*          dss_errno                /* error number                */
)
{
  struct scb_s  *scb_ptr;                   /* ptr to socket control block */
  sint15        result = 0;                 /* error checking return value */
  dssocki_app_to_ps_cmd_type  * ps_cmd;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_MSG1(MSG_LEGACY_HIGH,  "dss_shutdown called, sockfd=%d", sockfd);

  /*-------------------------------------------------------------------------
    Check that a valid ptr to a dss_errno variable is specified
  -------------------------------------------------------------------------*/
  if (dss_errno == NULL)
  {
    DS_ERROR_LOG_0( "dss_errno is NULL");
    return(DSS_ERROR);
  }
  *dss_errno = DSS_SUCCESS;

  /*-------------------------------------------------------------------------
    Convert socket descriptor to SCB
  -------------------------------------------------------------------------*/
  if ( (scb_ptr=dssocki_sockfdtoscb(sockfd)) == NULL)
  {
    DS_ERROR_LOG_1("Bad file descriptor, sockfd=%d", sockfd);
    *dss_errno = DS_EBADF;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Validate 'how' parameter
  -------------------------------------------------------------------------*/
  if ( how != DSS_SHUT_RD && how != DSS_SHUT_WR && how != DSS_SHUT_RDWR )
  {
    DS_ERROR_LOG_1( "Invalid operation, how=%d", how);
    *dss_errno = DS_EINVAL;
    return (DSS_ERROR);
  }

  result = dssocki_get_err_val(scb_ptr, DSSOCKI_SHUTDOWN_ROUTINE, dss_errno);

  if (result == DSS_ERROR && *dss_errno != DS_ESHUTDOWN)
  {
    return (result);
  }

  if(*dss_errno == DS_ESHUTDOWN &&
     (how == DSS_SHUT_WR || how == DSS_SHUT_RDWR))
  {
    *dss_errno = DS_ENOTCONN;
    return (result);
  }

  ps_cmd = dssocki_alloc_ps_cmd_buffer(APP_TO_PS_SOCKET_SHUTDOWN);
  if(NULL == ps_cmd)
  {
    DS_ERROR_LOG_0("Command buffers could not be allocated");
    *dss_errno = DS_ENOMEM ;
    return DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    Mark the scb to disallow specified operation. Note that this needs to
    be done in the application context.
  -------------------------------------------------------------------------*/
  if ( how == DSS_SHUT_RD || how == DSS_SHUT_RDWR )
  {
    DSSOCKI_SOCKET_CANTRECVMORE( scb_ptr );
  }

  if ( how == DSS_SHUT_WR || how == DSS_SHUT_RDWR )
  {
    DSSOCKI_SOCKET_CANTSENDMORE( scb_ptr );
  }

  dssocki_send_ps_cmd_buffer(scb_ptr, (void *)&how, NULL, ps_cmd);

  return (DSS_SUCCESS);

} /* dss_shutdown */

