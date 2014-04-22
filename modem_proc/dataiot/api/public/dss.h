#ifndef _DSS_H
#define _DSS_H
/*===========================================================================

   D A T A   S E R V I C E S   A P I   H E A D E R   F I L E

DESCRIPTION

 The Data Services Header File. Contains shared variables and enums,
 as well as declarations for functions.

Copyright (c) 2017 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.

===========================================================================*/
#include "comdef.h"
#include "dsm.h"
#include "ds_errno.h"
#include "ps_in.h"
#include "ds_defs.h"
#include "ds_profile.h"


/*===========================================================================
                            TYPEDEFS                                          
===========================================================================*/

typedef uint32 dss_qos_handle_type;


/*===========================================================================
                            MACROS                                          
===========================================================================*/

/*---------------------------------------------------------------------------
                    Return values indicating error status
---------------------------------------------------------------------------*/
#define DSS_SUCCESS         0                      /* successful operation */
#define DSS_ERROR          -1                    /* unsuccessful operation */
#define DSS_NOT_SUPPORTED  DSS_ERROR            /*  Unsupported operation  */

/*---------------------------------------------------------------------------
                    DSS NET Handle related MACROS
---------------------------------------------------------------------------*/
#define DSS_MAX_HANDLE         8            /*4PDN's *2(IPv4/v6) = 8*/
#define DSS_HANDLE_ASSIGNED    1            /*DSS Net handle is assigned*/
#define DSS_HANDLE_FREE        0            /*DSS Net handle is free*/
#define DSS_INVALID_HANDLE    -1


/*---------------------------------------------------------------------------
                      Asynchronous Socket Events
---------------------------------------------------------------------------*/
#define DS_WRITE_EVENT  0x01         /* associated with a writeable socket */
#define DS_READ_EVENT   0x02          /* associated with a readable socket */
#define DS_CLOSE_EVENT  0x04         /* associated with a closeable socket */
#define DS_ACCEPT_EVENT 0x08        /* associated with an accetable socket */

/*---------------------------------------------------------------------------
  Maximum bytes you can read using a single read call.
---------------------------------------------------------------------------*/
#define DSS_READ_MAX_BYTES ( (1 << 15) - 1)
#define DSS_WRITE_MAX_BYTES ( (1 << 15) - 1)

/*---------------------------------------------------------------------------
  Range values for various socket options
---------------------------------------------------------------------------*/
#define DSS_MIN_RCVBUF (536)
#define DSS_MAX_RCVBUF (256 * 1024)

#define DSS_MIN_SNDBUF (536)
#define DSS_DEF_SNDBUF (10 * 1024)
#define DSS_MAX_SNDBUF (256 * 1024)

/*===========================================================================
  Define flags passed to dss_sendto() calls here. Any time a new flag is to
  be added, define it here and OR it with the existing flags in
  MSG_VALID_BITS macro definition in dssocki.c so that the reserved bit
  mask can be properly built.

  Note: Refer to the SDB related flags defined in dssocket_defs.h when
        adding a new flag to avoid assigning the same value.
===========================================================================*/
#define DSS_MSG_ERRQUEUE 0x2000

#define DSS_MSG_TRUNC    8

/*---------------------------------------------------------------------------
  Socket extended error structure for ICMP(v6) error reporting and related
  constants.
---------------------------------------------------------------------------*/
#define DSS_SO_EE_ORIGIN_LOCAL PS_SO_EE_ORIGIN_LOCAL
#define DSS_SO_EE_ORIGIN_ICMP  PS_SO_EE_ORIGIN_ICMP
#define DSS_SO_EE_ORIGIN_ICMP6 PS_SO_EE_ORIGIN_ICMP6

/*---------------------------------------------------------------------------
  Definition for invalid handle
---------------------------------------------------------------------------*/
#define DSS_INVALID_HANDLE_TYPE     0

#define DSS_INVALID_SCOPE_ID 0


/*---------------------------------------------------------------------------
  Flags passed to dss_sendto() call for SDB. Non-SDB related flags are
  defined in dssocket.h. Any time a SDB related flags is added here, OR it
  with the existing flags in MSG_VALID_BITS macro definition in dssocki.c
  so that the reserved bit mask can be properly built.

  SDB flags will cause short data burst to be used if the mobile is not on
  traffic. Don't set any bit if one doesn't want SDB and traffic is to be
  brought up. Set both bits if one doesn't care which channel is used for
  SDB transmission, else set the appropriate bit.
---------------------------------------------------------------------------*/
#define MSG_EXPEDITE      0x00000001   /* use ACH/REACH/TCH                */
#define MSG_FAST_EXPEDITE 0x00000002   /* use     REACH/TCH                */
#define MSG_WAKEUP        0x00000004   /* Bring up      TCH                */


/*---------------------------------------------------------------------------
  The values for the 'how' argument of the shutdown() call. Specifies what
  action to perform:
---------------------------------------------------------------------------*/
#define  DSS_SHUT_RD   0                      /* disallow further receives */
#define  DSS_SHUT_WR   1                         /* disallow further sends */
#define  DSS_SHUT_RDWR 2     /* disallow further receives as well as sends */


/*===========================================================================

MACRO DSS_HTONL()

DESCRIPTION
  Converts host-to-network long integer.  Handles potential byte order
  differences between different computer architectures and different network
  protocols.

PARAMETERS
  x     unsigned long integer value to be converted.

DEPENDENCIES
  None.

RETURN VALUE
  The network byte-ordered value.

SIDE EFFECTS
  None.

===========================================================================*/
#define dss_htonl(x) ps_htonl(x)

/*===========================================================================

MACRO DSS_HTONLL()

DESCRIPTION
  Converts host-to-network long integer.  Handles potential byte order
  differences between different computer architectures and different network
  protocols.

PARAMETERS
  x     unsigned long integer value to be converted.

DEPENDENCIES
  None.

RETURN VALUE
  The host byte-ordered value.

SIDE EFFECTS
  None.

===========================================================================*/
#define dss_htonll(x) ps_htonll(x)

/*===========================================================================

MACRO DSS_HTONS()

DESCRIPTION
 Converts host-to-network short integer.  Handles potential byte order
 differences between different computer architectures and different network
 protocols.

PARAMETERS
  x     unsigned short integer value to be converted.

DEPENDENCIES
  None.

RETURN VALUE
  The network byte-ordered value.

SIDE EFFECTS
  None.

===========================================================================*/
#define dss_htons(x) ps_htons(x)

/*===========================================================================

MACRO DSS_NTOHL()

DESCRIPTION
  Converts network-to-host long integer.  Handles potential byte order
  differences between different computer architectures and different network
  protocols.

PARAMETERS
  x     unsigned long integer value to be converted.

DEPENDENCIES
  None.

RETURN VALUE
  The host byte-ordered value.

SIDE EFFECTS
  None.

===========================================================================*/
#define dss_ntohl(x) ps_ntohl(x)

/*===========================================================================

MACRO DSS_NTOHLL()

DESCRIPTION
  Converts network-to-host long integer.  Handles potential byte order
  differences between different computer architectures and different network
  protocols.

PARAMETERS
  x     unsigned long integer value to be converted.

DEPENDENCIES
  None.

RETURN VALUE
  The host byte-ordered value.

SIDE EFFECTS
  None.

===========================================================================*/
#define dss_ntohll(x) ps_ntohll(x)

/*===========================================================================

MACRO DSS_NTOHS()

DESCRIPTION
  Converts network-to-host short integer.  Handles potential byte order
  differences between different computer architectures and different network
  protocols.

PARAMETERS
  x     unsigned short integer value to be converted.

DEPENDENCIES
  None.

RETURN VALUE
  The host byte-ordered value.

SIDE EFFECTS
  None.

===========================================================================*/
#define dss_ntohs(x) ps_ntohs(x)



/*===========================================================================
                            ENUMS                                          
===========================================================================*/

/*---------------------------------------------------------------------------
                          Address/Protocol Family
---------------------------------------------------------------------------*/

enum
{
  DSS_AF_INET    = IPV4_ADDR,            /* Address Family - Internet    */
  DSS_AF_INET6   = IPV6_ADDR,           /* Address Family - Internet v6 */
  DSS_AF_UNSPEC  = 0,                      /* Unspecified address family   */
  DSS_AF_ANY     = 4                       /* Any address family           */
};


/*---------------------------------------------------------------------------
                             Socket types
---------------------------------------------------------------------------*/
enum
{
  DSS_SOCK_STREAM = 0,                             /* TCP - streaming socket */
  DSS_SOCK_DGRAM  = 1                               /* UDP - datagram socket */
};


typedef enum
{
  DSS_SOCKOPT_MIN     = -1,           /* lower bound                          */
  DSS_IP_TTL,                         /* time-to-live                         */
  DSS_SO_SYS_SOCK,                    /* bool: is this a system socket?       */
  DSS_SO_SILENT_CLOSE,                /* bool: close() call causes conn reset */
  DSS_SO_RCVBUF,                      /* set the receive window size          */
  DSS_SO_LINGER,                      /* linger on close                      */
  DSS_SO_SNDBUF,                      /* set/get the sndbuf queue size        */
  DSS_TCP_MAXSEG,                     /* set/get the TCP maximum segement size*/
  DSS_TCP_NODELAY,                    /* Disable Nagle's algorithm            */
  DSS_SO_KEEPALIVE,                   /* Send keepalive probes?               */
  DSS_TCP_DELAYED_ACK,                /* Enable delayed ack                   */
  DSS_TCP_SACK,                       /* Enable SACK                          */
  DSS_TCP_TIME_STAMP,                 /* Enable TCP time stamp option         */
  DSS_IP_RECVIF,                      /* Get incoming packet's interface      */
  DSS_IP_TOS,                         /* Type of Service                      */
  DSS_IPV6_TCLASS,                    /* Traffic class for V6 sockets         */
  DSS_SO_CB_FCN,                      /* set the socket callback function     */
  DSS_SO_ERROR_ENABLE,                /* Enable storage of ICMP err in so_err */
  DSS_SO_ERROR,                       /* Get value of ICMP so_error           */
  DSS_SO_LINGER_RESET,                /* Linger and reset on timeout          */
  DSS_IP_RECVERR,                     /* Enable getting ICMP error pkts       */
  DSS_IPV6_RECVERR,                   /* Enable getting ICMPv6 error pkts     */
  DSS_TCP_EIFEL,                      /* Enable EIFEL Algorithm               */
  DSS_SO_QOS_SHARE_HANDLE,            /* QOS group handle                     */
  DSS_SO_REUSEADDR,                   /* Enable Socket Reuse                  */
  DSS_SO_DISABLE_FLOW_FWDING,         /* Disable forwarding data on best effort
                                         flow if QoS flow can't be used       */
  DSS_ICMP_ECHO_ID,                   /* ICMP ECHO_REQ message, identifier    */
  DSS_ICMP_ECHO_SEQ_NUM,              /* ICMP ECHO_REQ message, sequence num  */
  DSS_ICMP_TYPE,
  DSS_ICMP_CODE,
  DSS_SO_FIONREAD,                    /* To get the TCP/UDP/ICMP socket rcvq 
                                                               length. */
  DSS_TCP_MAX_BACKOFF_TIME,          /*  Max TCP backoff time */
  DSS_UDP_ENCAPS,                    /* Enable UDP encapsulation */
  DSS_SOCKOPT_MAX                    /* determine upper bound and array size */
} dss_sockopt_names_type_e;

/*---------------------------------------------------------------------------
  Socket option level specifies the code in the system to interpret the
  option: the general socket code or some protocol-specific code.  If
  an option is unknown at a given level, an error will be generated.
---------------------------------------------------------------------------*/
typedef enum
{
  DSS_IPPROTO_IP   = 1,               /* IP protocol level                 */
  DSS_SOL_SOCKET,                     /* socket level                      */
  DSS_SOCK         = DSS_SOL_SOCKET,  /* another alias for socket level    */
  DSS_IPPROTO_TCP,                    /* TCP protocol level                */
  DSS_IPPROTO_IPV6,                   /* IPV6 protocol level               */
  DSS_IPPROTO_ICMP,                   /* ICMP protocol level              */
  DSS_IPPROTO_ICMP6,                  /* ICMPv6 protocol level            */
  DSS_IPPROTO_UDP                     /* UDP prptocol level                */
} dss_sockopt_levels_type_e;


/*===========================================================================
                            STRUCTURES                                       
===========================================================================*/


/*---------------------------------------------------------------------------
  The data type used for SO_LINGER socket option. Note that l_linger is in
  ms, not in seconds as on UNIX systems
---------------------------------------------------------------------------*/
typedef struct
{
  int l_onoff;                                     /* linger active or not */
  int l_linger;                   /* how many milli-secondss to linger for */
} dss_so_linger_type_s;

/*---------------------------------------------------------------------------
  Structures defined for the IP_RECVIF and IP_RECVSTADDR options.
---------------------------------------------------------------------------*/
typedef struct
{
  struct ps_in_addr ip_addr;                   /* destination IPv4 address */
  uint32            if_index;                  /* received interface index */
} dss_in_pktinfo_type_s;

typedef struct
{
  struct ps_in6_addr ip6_addr;                 /* destination IPv6 address */
  uint32             if_index;                 /* received interface index */
} dss_in6_pktinfo_type_s;

/*---------------------------------------------------------------------------
  Structures defined for the DSS Net and Sock CallBack parameters
---------------------------------------------------------------------------*/
typedef struct
{
  sint15                          dss_nethandle;        /* Application id */
  sint15                          dss_errno;            /* type of network error, ENETISCONN, ENETNONET.*/
  ds_net_down_event_info_type     pdn_down_info;        /* Net Down Reason*/
  void                            *net_cb_user_data;    /* Call back User data  */
  
}dss_net_callback_info_s;

typedef struct
{
  sint15 dss_nethandle;              /* Application id */
  sint15 sockfd;                     /* socket descriptor */
  uint32 event_mask;                 /* Event occurred */
  void * sock_cb_user_data;          /* User data specfied during registration */
}dss_sock_callback_info_s;

/*---------------------------------------------------------------------------
  Message header structure for the common I/o functions.
---------------------------------------------------------------------------*/
struct dss_msghdr
{
  void               * msg_name;            /* protocol address            */
  uint16               msg_namelen;         /* size of protocol address    */
  struct ps_iovec    * msg_iov;             /* scatter/gather array        */
  uint16               msg_iovlen;          /* # elements in msg_iov       */
  void               * msg_control;         /* ancillary data              */
  uint16               msg_controllen;      /* length of ancillary data    */
  int                  msg_flags;           /* flags returned by recvmsg   */
};

/*---------------------------------------------------------------------------
  Message header structure for ancillary data.
---------------------------------------------------------------------------*/
struct dss_cmsghdr
{
  uint32   cmsg_len;                  /* data byte count, including header */
  int32    cmsg_level;                             /* originating protocol */
  int32    cmsg_type;                            /* protocol-specific type */
};


/*---------------------------------------------------------------------------
  Macros required to process and control the ancillary data struct cmsghdr.
---------------------------------------------------------------------------*/


#define DSS_CMSG_DATA(cmsg) ((void*)((unsigned char *)(cmsg) + \
                             DSS_CMSG_ALIGN(sizeof(struct dss_cmsghdr))))

#define DSS_CMSG_NXTHDR(msg, cmsg) cmsg_nxthdr (msg, cmsg)

#define DSS_CMSG_FIRSTHDR(msg) \
  ((uint32) (msg)->msg_controllen >= sizeof (struct dss_cmsghdr)          \
   ? (struct dss_cmsghdr *) (msg)->msg_control : NULL)

#define DSS_CMSG_ALIGN(len) ( ((len) + sizeof(uint32) - 1) \
                              & (uint32) ~(sizeof(uint32) - 1) )

#define DSS_CMSG_SPACE(len) ( DSS_CMSG_ALIGN(len) \
                              + DSS_CMSG_ALIGN(sizeof(struct dss_cmsghdr)) )

#define DSS_CMSG_LEN(len) ( DSS_CMSG_ALIGN (sizeof(struct dss_cmsghdr)) + \
                            (len) )

static __inline struct dss_cmsghdr * cmsg_nxthdr
(
  struct dss_msghdr  * msg,
  struct dss_cmsghdr * cmsg
)
{
  if ((uint32) cmsg->cmsg_len < sizeof (struct dss_cmsghdr))
    return NULL;

  cmsg = (struct dss_cmsghdr *) ((unsigned char *) cmsg
                   + DSS_CMSG_ALIGN (cmsg->cmsg_len));
  if((unsigned char *) (cmsg + 1) > ((unsigned char *) msg->msg_control
                    + msg->msg_controllen))
    return NULL;
  return cmsg;
}



typedef void (*dss_net_cb_fcn)
(
  dss_net_callback_info_s *net_callback_info
);

typedef void (*dss_sock_cb_fcn)
(
  dss_sock_callback_info_s *sock_callback_info
);


typedef struct  dss_profile_auth_type_s
{
  boolean auth_param_valid;
  ds_profile_auth_type auth_params;
}dss_profile_auth_type_t;

typedef struct
{
  dss_net_cb_fcn       net_cb;                    /* network callback function */
  void                 *net_cb_user_data;        /* User data for network call back */
  dss_sock_cb_fcn      sock_cb;                   /* socket callback function */
  void                 *sock_cb_user_data;       /*User data for socket call back */
  uint8                family;                    /*IP family such as DSS_AF_INET ,DSS_AF_INET*/
  byte                 *name;                     /*apn string */
  ds_apn_enum_type     apn_type;                  /*APN type */
  dss_profile_auth_type_t  authentication_params; /*Authentication Params*/
} dss_net_lib_info_type_s;


/*---------------------------------------------------------------------------
  The data type used for changing the socket call back function using the
  socket option DSS_SO_CB_FN.
---------------------------------------------------------------------------*/
typedef struct
{
  dss_sock_cb_fcn  sock_cb_fcn;
  void*            sock_cb_user_data;
} dss_sock_cb_fcn_type_s;



/*===========================================================================
                            APIS/FUNCTIONS                                
===========================================================================*/




/*=====================================================================
FUNCTION DSS_BRINGUP_CALL()
DESCRIPTION
  Opens up the network library.  Assigns application ID and sets the
  application-defined callback functions to be called when library and
  socket calls would make progress. Stores the network policy info and
  brings up the data call with the policy info.
  Puts data services manager into "socket" mode.
  This function is called from the context of the socket client's task.
DEPENDENCIES
  None.
RETURN VALUE
  Returns application ID on success.
  On error,  places the error condition value in *dss_errno.
  dss_errno Values
  DS_EMAPP      no more applications available - max apps exceeded.
  DS_EFAULT     Policy structure is uninitialized.
  DS_WOULDBLOCK    Call bringup is in process events will be notified
  DSS_SUCCESS      Call bringup successful(This will be set when there is
                   already a call present on the APN and IP type being requested)
=====================================================================*/
sint15 dss_bringup_call
(
  dss_net_lib_info_type_s *net_params,
  sint15                  *dss_errno        /* error condition value */
);

/*===========================================================================
FUNCTION DSS_BRINGDOWN_CALL()
DESCRIPTION
  Closes the data call for the application.  All sockets must have
  been closed for the application, prior to closing. 
  This function is called from the context of the socket client's task.
  
DEPENDENCIES
  None.

RETURN VALUE
  On success, returns DSS_SUCCESS.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADAPP        invalid application ID
  DS_SOCKEXIST     there are existing sockets
 

SIDE EFFECTS
 None
===========================================================================*/
sint15 dss_bringdown_call
(
  sint15 dss_nethandle,                         /* application ID */
  sint15 *dss_errno                               /* error condition value */
);

sint15 dss_socket2
(
  byte   family,                                         /* Address family */
  byte   type,                                              /* socket type */
  byte   protocol,                                        /* protocol type */
  dss_sock_cb_fcn sock_cb,                     /* socket callback function */
  void * sock_cb_user_data,              /* User data for socket call back */
  sint15 *dss_errno                               /* error condition value */
);
/*===========================================================================

FUNCTION DSS_SOCKET()

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
  DS_EPROTOTYPE       specified protocol invalid for socket type
  DS_ESOCKNOSUPPORT   invalid or unsupported socket parameter specified
  DS_EPROTONOSUPPORT  specified protocol not supported
  DS_EMFILE           too many descriptors open.  A socket is already open or
                      has not closed compeletely.

SIDE EFFECTS
  None.

===========================================================================*/
sint15 dss_socket
(
  sint15 dss_nethandle,                                   /* application ID */
  byte   family,                       /* Address family - DSS_AF_INET only */
  byte   type,                                              /* socket type */
  byte   protocol,                                        /* protocol type */
  sint15 *dss_errno                               /* error condition value */
);

/*===========================================================================

FUNCTION DSS_CONNECT()

DESCRIPTION
  For TCP, attempts to establish the TCP connection.  Upon
  successful connection, calls the socket callback function asserting that
  the DS_WRITE_EVENT is TRUE.  The implementation does not support connected
  UDP sockets and will return an error.  The function must receive
  (as a parameter) a valid socket descriptor, implying a previous successful
  call to dss_socket().

DEPENDENCIES
  Network subsystem must be established and available.

RETURN VALUE
  Returns DSS_SUCCESS on success.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

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
  struct ps_sockaddr *servaddr,                     /* destination address */
  uint16 addrlen,                                    /* length of servaddr */
  sint15 *dss_errno                               /* error condition value */
);

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
  reduced to the maximum (see DSS_SOMAXCONN).

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
  DS_ENETDOWN         network subsystem unavailable
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
);

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
  DS_ENETDOWN         network subsystem unavailable
  DS_NOMEM            not enough memory to establish backlog connections.

SIDE EFFECTS

  The head backlog item from the queue of the listening socket is
  removed from that queue.

===========================================================================*/
sint15 dss_accept
(
  sint15 sockfd,                                      /* Socket descriptor */
  struct ps_sockaddr *remoteaddr,                    /* new remote address */
  uint16 *addrlen,                                   /* length of servaddr */
  sint15 *dss_errno                               /* error condition value */
);


/*===========================================================================

FUNCTION DSS_BIND()

DESCRIPTION
  For all client sockets, attaches a local address and port value to the
  socket.  If the call is not explicitly issued, the socket will implicitly
  bind in calls to dss_connect() or dss_sendto().  Note that this function
  does not support binding a local IP address, but rather ONLY a local port
  number.  The local IP address is assigned automatically by the sockets
  library.  The function must receive (as a parameter) a valid socket
  descriptor, implying a previous successful call to dss_socket().

DEPENDENCIES
  None.

RETURN VALUE
  Returns DSS_SUCCESS on success.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor is specfied
  DS_EOPNOTSUPP       operation not supported
  DS_EADDRINUSE       the local address is already in use.
  DS_EINVAL           the socket is already attached to a local name
  DS_EFAULT           invalid address parameter has been specified

SIDE EFFECTS
  None.

===========================================================================*/
sint15 dss_bind
(
  sint15 sockfd,                                      /* socket descriptor */
  struct ps_sockaddr *localaddr,                          /* local address */
  uint16 addrlen,                                     /* length of address */
  sint15 *dss_errno                               /* error condition value */
);


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
);


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
  sint15 dss_nethandle,                                         /* application ID */
  sint15 *dss_errno                               /* error condition value */
);


/*===========================================================================

FUNCTION DSS_ASYNC_SELECT()

DESCRIPTION
  Enables the events to be notified about through the asynchronous
  notification mechanism.  Application specifies a bitmask of events that it
  is interested in, for which it will receive asynchronous notification via
  its application callback function.  This function also performs a real-time
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
  sint31 event_mask,                           /* bitmask of events to set */
  sint15 *dss_errno                               /* error condition value */
);


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
);


/*===========================================================================

FUNCTION DSS_GETNEXTEVENT()

DESCRIPTION
  This function performs a real-time check to determine if any of the events
  of interest specified in the socket control block's event mask have
  occurred.  It also clears any bits in the event mask that have occurred.
  The application must re-enable these events through a subsequent call to
  dss_async_select().  The application may pass in a pointer to a single
  socket descriptor to determine if any events have occurred for that socket.

  Alternatively, the application may set this pointer's value to NULL (0)
  (note, not to be confused with a NULL pointer, but rather a pointer whose
  value is 0) in which case the function will return values for the next
  available socket.  The next available socket's descriptor will be placed
  in the socket descriptor pointer, and the function will return.  If no
  sockets are available (no events have occurred across all sockets for
  that application) the pointer value will remain NULL (originally value
  passed in), and the function will return 0, indicating that no events
  have occurred.

DEPENDENCIES
  None.

RETURN VALUE
  Returns an event mask of the events that were asserted.  A value of 0
  indicates that no events have occurred.

  On passing a pointer whose value is NULL into the function for
  the socket descriptor (not to be confused with a NULL pointer), places
  the next available socket descriptor in *sockfd_ptr and returns the
  event mask for that socket. If no sockets are available (no events have
  occurred across all sockets for that application) the pointer value
  will remain NULL (originally value passed in), and the function will
  return 0, indicating that no events have occurred.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADAPP           invalid app descriptor is specfied
  DS_EBADF             invalid socket descriptor is specfied

SIDE EFFECTS
  Clears the bits in the socket control block event mask, corresponding to
  the events that have occurred.

===========================================================================*/
sint31 dss_getnextevent
(
  sint15 dss_nethandle,                                         /* application ID */
  sint15 *sockfd_ptr,                                 /* socket descriptor */
  sint15 *dss_errno                               /* error condition value */
);


/*===========================================================================

FUNCTION DSS_WRITE()

DESCRIPTION
  Sends specified number of bytes in the buffer over the TCP transport.

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
  DS_EADDRREQ         destination address required - connectionless socket
                      did not call dss_connect()
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           application buffer no valid part of address space
  DS_EWOULDBLOCK      operation would block

SIDE EFFECTS
  None.

===========================================================================*/
sint15 dss_write
(
  sint15 sockfd,                                      /* socket descriptor */
  const void *buffer,               /* user buffer from which to copy data */
  uint16 nbytes,                /* number of bytes to be written to socket */
  sint15 *dss_errno                               /* error condition value */
);


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
  DS_EADDRREQ         destination address required - connectionless socket
                      did not call dss_connect()
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           application buffer no valid part of address space
  DS_EWOULDBLOCK      operation would block

SIDE EFFECTS
  None.

===========================================================================*/
sint15 dss_writev
(
  sint15 sockfd,                                      /* socket descriptor */
  struct ps_iovec iov[],  /* array of data buffers from which to copy data */
  sint15 iovcount,                                /* number of array items */
  sint15 *dss_errno                               /* error condition value */
);


/*===========================================================================

FUNCTION DSS_READ()

DESCRIPTION
  Reads specified number of bytes into buffer from the TCP transport.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes read, which could be less than the number of
      bytes specified.  A return of 0 indicates that an End-of-File condition
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
  DS_EADDRREQ         destination address required - connectionless socket
                      did not call dss_connect()
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           application buffer no valid part of address space
  DS_EWOULDBLOCK      operation would block

SIDE EFFECTS
  None.

===========================================================================*/
sint15 dss_read
(
  sint15 sockfd,                                      /* socket descriptor */
  void   *buffer,                     /* user buffer to which to copy data */
  uint16 nbytes,                 /* number of bytes to be read from socket */
  sint15 *dss_errno                               /* error condition value */
);


/*===========================================================================

FUNCTION DSS_READV()

DESCRIPTION
  Provides the scatter read variant of the dss_read() call, which
  allows the application to read into non-contiguous buffers.    Reads
  specified number of bytes into the buffer from the TCP transport.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes read, which could be less than the number of
      bytes specified.  A return of 0 indicates that an End-of-File condition
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
  DS_EADDRREQ         destination address required - connectionless socket
                      did not call dss_connect()
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           application buffer no valid part of address space
  DS_EWOULDBLOCK      operation would block

SIDE EFFECTS
  None.

===========================================================================*/
sint15 dss_readv
(
  sint15 sockfd,                                      /* socket descriptor */
  struct ps_iovec iov[],        /* array of data buffers to copy data into */
  sint15 iovcount,                                /* number of array items */
  sint15 *dss_errno                               /* error condition value */
);


/*===========================================================================

FUNCTION DSS_SENDTO()

DESCRIPTION
  Sends 'nbytes' bytes in the buffer over the UDP transport.

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
  DS_EADDRREQ         destination address required
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           application buffer no valid part of address space
  DS_EOPNOSUPPORT     option not supported

SIDE EFFECTS
  None.

===========================================================================*/
sint15 dss_sendto
(
  sint15 sockfd,                                      /* socket descriptor */
  const void *buffer,           /* user buffer from which to copy the data */
  uint16 nbytes,                          /* number of bytes to be written */
  uint32 flags,                                                  /* unused */
  struct ps_sockaddr *toaddr,                       /* destination address */
  uint16 addrlen,                                        /* address length */
  sint15 *dss_errno                               /* error condition value */
);


/*===========================================================================

FUNCTION DSS_RECVFROM()

DESCRIPTION
  Reads 'nbytes' bytes in the buffer from the UDP transport.  Fills in
  address structure with values from who sent the data.

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
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           application buffer no valid part of address space
  DS_EOPNOSUPPORT     option not supported

SIDE EFFECTS
  None.

===========================================================================*/
sint15 dss_recvfrom
(
  sint15 sockfd,                                      /* socket descriptor */
  void   *buffer,               /* user buffer from which to copy the data */
  uint16 nbytes,                          /* number of bytes to be written */
  uint32 flags,                                                  /* unused */
  struct ps_sockaddr *fromaddr,                     /* destination address */
  uint16 *addrlen,                                       /* address length */
  sint15 *dss_errno                               /* error condition value */
);

/*===========================================================================

FUNCTION DSS_SETSOCKOPT

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
  sint15 *dss_errno     -     Error condition value.


RETURN VALUE
  On error, return DSS_ERROR and places the error condition value in *errno.

  Errno Values
  ------------
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
  sint15 *dss_errno                       /* error condition value          */
);


/*===========================================================================
FUNCTION DSS_GETSOCKOPT

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
  sint15 *dss_errno     -     Error condition value.

RETURN VALUE
  optlen is a value-result parameter, initially containing the size of
  the buffer pointed to by optval, and modified on return to indicate the
  actual  size  of the value returned. On error, return DSS_ERROR and places
  the error condition value in *errno.

  Errno Values
  ------------
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
);

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
  sint15              sockfd,               /* socket descriptor           */
  struct ps_sockaddr* addr,                 /* address of the socket       */
  uint16*             addrlen,              /* address length              */
  sint15*             dss_errno             /* error number                */
);

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
  sint15              sockfd,               /* socket descriptor           */
  struct ps_sockaddr* addr,                 /* address of the socket       */
  uint16*             addrlen,              /* address length              */
  sint15*             dss_errno             /* error number                */
);

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
);

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
);

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
);


/*===========================================================================

FUNCTION DSS_WRITE_DSM_CHAIN

DESCRIPTION
  Sends the specified DSM item chain over the TCP transport.  Lower layers
  will be responsible for deallocating the DSM item chain.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes written.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  Errno Values
  ------------
  DS_EBADF            invalid socket descriptor is specfied
  DS_ENOTCONN         socket not connected
  DS_ECONNRESET       TCP connection reset by server
  DS_ECONNABORTED     TCP connection aborted due to timeout or other failure
  DS_EIPADDRCHANGED   IP address changed, causing TCP connection reset
  DS_EPIPE            broken pipe
  DS_EADDRREQ         destination address required
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           application buffer no valid part of address space
  DS_EWOULDBLOCK      operation would block
  DS_EOPNOTSUPP       invalid server address specified

SIDE EFFECTS
  The pointer to the DSM item chain is set to NULL.

===========================================================================*/
sint15 dss_write_dsm_chain
(
  sint15          sockfd,            /* socket descriptor                  */
  dsm_item_type **item_ptr,          /* DSM item chain containing the data */
  sint15         *dss_errno          /* error condition value              */
);


/*===========================================================================

FUNCTION DSS_READ_DSM_CHAIN

DESCRIPTION
  Reads a DSM item chain from the TCP transport.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes read.  A return of 0 indicates that an End-of-File
      condition has occurred.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  Errno Values
  ------------
  DS_EBADF            invalid socket descriptor is specfied
  DS_ENOTCONN         socket not connected
  DS_ECONNRESET       TCP connection reset by server
  DS_ECONNABORTED     TCP connection aborted due to timeout or other failure
  DS_EIPADDRCHANGED   IP address changed, causing TCP connection reset
  DS_EPIPE            broken pipe
  DS_EADDRREQ         destination address required
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           application buffer no valid part of address space
  DS_EWOULDBLOCK      operation would block

SIDE EFFECTS
  None.

===========================================================================*/
sint15 dss_read_dsm_chain
(
  sint15           sockfd,            /* socket descriptor                 */
  dsm_item_type  **item_ptr,          /* ptr to item chain containing data */
  sint15          *dss_errno          /* error condition value             */
);


/*===========================================================================
FUNCTION DSS_SENDTO_DSM_CHAIN()

DESCRIPTION
  Sends the specified DSM item chain over the TCP transport.  Lower layers
  will be responsible for deallocating the DSM item chain.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes written.

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
  DS_EOPNOTSUPP       invalid server address specified

SIDE EFFECTS
  The pointer to the DSM item chain is set to NULL.
===========================================================================*/
sint15 dss_sendto_dsm_chain
(
  sint15 sockfd,                                      /* socket descriptor */
  dsm_item_type **item_ptr,          /* DSM item chain containing the data */
  uint32 flags,                 /* used for SDB (if enabled)               */
  struct ps_sockaddr *toaddr,   /* destination address                     */
  uint16 addrlen,               /* address length                          */
  sint15 *dss_errno                               /* error condition value */
);

/*===========================================================================
FUNCTION DSS_RECVFROM_DSM_CHAIN()

DESCRIPTION
  Reads a DSM item chain from the TCP transport.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes read.  A return of 0 indicates that an End-of-File
      condition has occurred.

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
  DS_EINVAL           can't read from a listen socket

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_recvfrom_dsm_chain
(
  sint15 sockfd,                                      /* socket descriptor */
  dsm_item_type  **item_ptr,          /* ptr to item chain containing data */
  uint32 flags,                                                  /* unused */
  struct ps_sockaddr *fromaddr,                     /* destination address */
  uint16 *addrlen,                                       /* address length */
  sint15 *dss_errno                               /* error condition value */
);


/*===========================================================================
FUNCTION DSS_REG_IP_FILTER()

DESCRIPTION
  This function registers IP filters for the specified socket and iface. The
  filters are validated and any error cases detected.

  If this function is called on a bound socket it verifies that the binding
  matches the passed filter.  It also requires that for tcp socket the
  filter be registered before a connect.

  Next header protocol must be specified. Otherwise all IP traffic will be
  snooped and sent up the stack starving laptop of all traffic.

  It is recommended that filter specifies Source address, Source port and
  Destination port to make the filter as narrow as possible. This way filter
  doesn't snoop in to the data destined for the laptop. Here destination
  is the mobile and source is the peer as sockets filters are inbound.

  Right now it is not enforced that Source address, Source port and
  Destination port are specified in the filter, but we need to revisit this
  API when it is externalized.

DEPENDENCIES
  None.

PARAMETERS
  sockfd      - Descriptor of the Socket requesting filtering
  num_filters - Number of filters passed in
  filters     - Ptr to list of filters (total num_filters) where each
  filter is of type specified above -
  => ip_filter_type for IP_FILTER_DEFAULT_TYPE
  filter_handle - Handled returned on successful creation of filter
  dss_errno   - Ptr to contain error code on the return

RETURN VALUE
  Filter handle added successfully, or
  DSS_ERROR for error (sets dss_errno appropriately)

SIDE EFFECTS
  Adds socket client filters 

===========================================================================*/

int dss_reg_ip_filter
(
  sint15                     sockfd,
  uint8                      num_filters,
  void                       *filters,
  uint32                     *filter_handle,
  sint15                     *dss_errno
);


/*===========================================================================
FUNCTION DSS_DEREG_IP_FILTER()

DESCRIPTION
  This function deregisters all the IP filters previosuly added to an iface
  associated with the specified socket.

DEPENDENCIES
  None.

PARAMETERS
  sockfd     - Descriptor of the Socket requesting filtering
  filter_handle - The handle for the filter to be deregistered
  dss_errno  - Ptr to contain error code on the return

RETURN VALUE
  0 for success, DSS_ERROR for error (sets dss_errno appropriately)

SIDE EFFECTS
  Removes socket client filters from the passed iface
===========================================================================*/
int dss_dereg_ip_filter
(
  sint15             sockfd,
  uint32             filter_handle,
  sint15            *dss_errno
);

/*===========================================================================
FUNCTION DSS_INET_ATON

DESCRIPTION
  Convert internet address from dotted string to network ordered struct
  ps_in_addr.

DEPENDENCIES
  None.

RETURN VALUE
  DSS_ERROR    For failure
  DSS_SUCCESS  For success

  The IP address is returned in struct ps_in_addr *addr

SIDE EFFECTS
  None.
===========================================================================*/
int32 dss_inet_aton
(
  const  char       *cp,               /* IPv4 addr in dotted quad string  */
  struct ps_in_addr *addr              /* Returned IPv4 address            */
);

/*===========================================================================
FUNCTION  DSS_INET_NTOA

DESCRIPTION
  Convert network order internet address to dotted string form.

DEPENDENCIES
  None.

RETURN VALUE
  DSS_SUCCESS   If successful
  DSS_ERROR     If an error occured

SIDE EFFECTS
  None.
===========================================================================*/
int32 dss_inet_ntoa
(
  const struct ps_in_addr  in,     /* IPv4 address to be converted         */
  uint8                   *buf,    /* Buffer to hold the converted address */
  int32                    buflen  /* Length of buffer                     */
);

/*===========================================================================
FUNCTION  DSS_INET_PTON

DESCRIPTION
  This function is called with a presentation (printable or ASCII) format
  address to be converted to its network address (binary) format.  The af
  argument can be either DSS_AF_INET if the address to be converted is an IPv4
  address or DSS_AF_INET6 if the address is an IPv6 address.  In case of error
  the error code is returned in the dss_errno argument.

DEPENDENCIES
  The dst argument should have sufficient memory for the network address
  of the appropriate family.  For IPv4 it should be at least
  sizeof(struct ps_in_addr) while for IPv6 it should be at least
  sizeof(struct ps_in6_addr).

RETURN VALUE
  DSS_SUCCESS in case of success with the network format address
              returned in the dst argument.
  DSS_ERROR   in case of error with the error code returned in dss_errno
              argument.

              dss_errno values returned:
              DS_EFAULT         invalid arguments passed to function
              DS_EAFNOSUPPORT   invalid value for the address family
                                argument
              DS_NAMEERR        Malformed address passed to be converted
              DS_EMSGTRUNC      Insufficient buffer space in return argument

SIDE EFFECTS
  Returns the converted printable format IPv6 address in the dst argument.
  Any errors are returned in dss_errno argument.
===========================================================================*/
int32 dss_inet_pton
(
  const char *src,       /* String containing presentation form IP address */
  int32       af,        /* Address family of address in src argument      */
  void       *dst,       /* Memory for returning address in network format */
  uint32      dst_size,  /* Size of memory passed in dst argument          */
  int16      *dss_errno  /* Error code returned in case of DSS_ERROR return*/
);

/*===========================================================================
FUNCTION  DSS_INET_NTOP

DESCRIPTION
  This function is called with an IPv4 or IPv6 address in network (binary)
  format to be converted into the presentation (dotted decimal for IPv4
  or colon seperated for IPv6) format.  The af argument can be either
  DSS_AF_INET if the address to be converted is an IPv4 address or DSS_AF_INET6 if
  the address is an IPv6 address.  In case of error the error code is
  returned in the dss_errno argument.

DEPENDENCIES
  The dst argument should have sufficient memory for the string address.
  To convert an IPv4 address it is suggested to be PS_IN_ADDRSTRLEN
  in size while to convert an IPv6 address it is suggested to be 
  PS_IN6_ADDRSTRLEN.

RETURN VALUE
  DSS_SUCCESS in case of success with the presentation format address
              returned in the dst argument.
  DSS_ERROR   in case of error with the error code returned in dss_errno
              argument.

              dss_errno values returned:
              DS_EFAULT         invalid arguments passed to function
              DS_EAFNOSUPPORT   invalid value for the address family
                                argument
              DS_NAMEERR        Malformed address passed to be converted
              DS_EMSGTRUNC      Insufficient buffer space in return argument
              DS_ENORECOVERY    Failed to convert

SIDE EFFECTS
  Returns the converted printable format IPv6 address in the dst argument.
  Any errors are returned in dss_errno argument.
===========================================================================*/
int32 dss_inet_ntop
(
  const char *src,       /* Memory containing network form IP address      */
  int32       af,        /* Address family of address in src argument      */
  void       *dst,       /* Memory to return address in presentation format*/
  uint32      dst_size,  /* Size of memory passed in dst argument          */
  int16      *dss_errno  /* Error code returned in case of DSS_ERROR return*/
);

#endif /*_DSS_H*/

