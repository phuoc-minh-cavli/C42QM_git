#ifndef DSSTCP_NEW_H
#define DSSTCP_NEW_H

/*===========================================================================

     D A T A  S E R V I C E S  S O C K E T  T C P  H E A D E R  F I L E

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

  dsstcp_close()
    TCP specific close() function.  Initiates the TCP active close.
    If the connection is already closed, cleans up the TCP socket and
    associated resources.

  dsstcp_bind()
    This is the TCP specific bind() function. It writes the port value into
    local.conn.port of the socket control block structure.

  dsstcp_setopt
    This function sets the tcp option value to the user specified value.

  dsstcp_flow_enabled_ind
    This function calls tcp_output() to send data out once socket is
    flow enabled

  dsstcp_lookup_conn()
    Given a connection structure, looks up the connection to determine
    if this is a valid connection.

  dsstcp_lookup_serv()
    Given a connection structure, looks up to determine if there are
    any valid listener sockets(server socket)

  dsstcp_new_ip_upcall()
    If a new IP address is negotiated with the Base Station, then send a
    reset to the peer, and then close down sockets and notify the user.


Copyright (c) 1998-2019 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsstcp.h_v   1.2   13 Feb 2003 12:21:28   ssinghai  $
  $Header: //components/rel/dataiot.mpss/2.0/protocols/inet/inc/dsstcp.h#1 $ $DateTime: 2020/03/17 08:03:16 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/09/19   sr      Added support for dsstcp_get_conn_error
10/10/19   ds      Added prototype for dsstcp_is_writeable().
10/12/18   ds      Removed prototype for dsstcp_is_writeable() as it is no 
                   needed.
09/28/18   ds      Memory optimization changes for low tier chipsets.
10/13/06   msr     Added APP_TO_PS_SOCKET_FLOW_ENABLED_IND
07/31/06   rt      Route Scope modifications.
03/27/06   rt      Added a new function dsstcp_window_reopen.
08/15/05   vp      Addition of flags argument to dsstcp_read().
04/18/05   vp      Change in signature of bind and listen functions.
08/16/04   ks/msr  Removed dsstcp_init_tcb_array function.
08/30/04   sv      Moved graceful dormant close flag and keep alive idle time
                   constants to socket config control block.
04/14/04   vp      Added declaration for tcp_keepalive_idle_time.
03/30/04   vp      Merged changes from June04Dev_05.00.00 PS branch.
01/27/04   vp      Added declaration of tcp_graceful_dormant_close flag
11/11/03   sv      Modified is_writeable to take scb_ptr instead of pcb.
10/17/03   sv      Modified protocol specific functions to take PCB instead
                   of SCB. Removed dsstcp_abort as it is no longer used.
09/09/03   sv      Added dsstcp_reset function to reset tcb.
08/18/03   sv      Modified the code to use IPv6 addresses internally.
08/08/03   ss/sv   Restructured the socket code.
06/27/03   sv      Modified dsstcp_is_writeable to return integer.
04/15/03   sv      Modified dsstcp_read to accept iov and iovcount as
                   arguments instead of buffer and bufferlen.
02/13/03   ss      Added dsstcp_shutdown()
12/22/02   aku     Changed dsstcp_new_ip_upcall() signature to take
                   ps_iface_ptr as argument instead of IP addresses.
05/02/02   mvl     File clean up, added meta_info_ptr to write() prototype
02/22/02   pjb     Added accept and listen
02/12/02   ss      Added prototype for dsstcp_cleanup_all_reseq().
02/11/02   ss      Added prototype for dsstcp_is_writeable().
10/11/01   mt      Add dss_errno argument to dsstcp_connect_setup().
03/27/01   mt      Changed type of second argument in dsstcp_read() from
                   byte * to void *.
03/27/01   js      Added extra parameter to function dsstcp_write(). The
                   extra parameter holds a pointer to the structure holding
                   the destination information. Though not used, the addition
                   is necessary to comply with the dss_fcn_ptr_table format.
02/08/01   yll     Added dsstcp_read_dsm_chain().
11/10/00   snn     Changed scb_type to scb_s
10/10/00   na      Removed tcp_ptr which is not longer used. Added prototype
                   for dsstcp_init_tcb_array().
08/03/00   mvl/js  Added prototype for dsstcp_bind() function.
03/22/00   rc/na   Added prototype for function dsstcp_new_ip_upcall().
09/22/99   rc      Incorporated code review changes and comments.
08/11/99   rc/ak   Extern for tcp abort function.
12/14/98   na/hcg  Created module.
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
//#include "ds_Sock_Def.h"
#include "ps_in.h"
#include "ps_socket_defs.h"
#include "dssi.h"
#include "ps_rt_meta_info.h"
#include "ps_pkt_meta_info.h"
#include "dssocki.h"
#include "ps_in_alias.h"

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION DSSTCP_SOCKET()

DESCRIPTION
  This function is the protocol-specific socket function.  For TCP, it
  just sets the socket state to INIT.

  This function is called from the context of the application task.

DEPENDENCIES
  None.

RETURN VALUE
  On success, returns DSS_SUCCESS

  On error, return DSS_ERROR and places the error condition value in *errno.

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dsstcp_socket
(
  struct scb_s* scb_ptr,	 /* Ptr to socket control block for the socket */
  sint15 *dss_errno                /* error condition value */
);

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

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dsstcp_connect
(
  protocol_ctl_block_type  pcb,                  /* Protocol control block */
  struct ps_sockaddr_in6 *dest_ptr,                /* destination socket addr */
  ps_meta_info_type *meta_info_ptr                     /* meta information */
);

/*===========================================================================
FUNCTION DSSTCP_LISTEN()

DESCRIPTION
  This starts a passive open on a tcp socket.
  This is called from the context of the protocol stack
 // PJB Fill in from .c file....
DEPENDENCIES
  Before this is called the source port should be put in the socket
  control block with bind.
 // PJB Fill in from .c file....
RETURN VALUE
  Returns success on success, or error on error.  If there
  is an error dss_errno is filled in to indicate the error.

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dsstcp_listen
(
  protocol_ctl_block_type  pcb,        /* Protocol control block */
  uint16 rcv_window                    /* receive window size */
);


sint15 dsstcp_accept
(
  struct scb_s* scb_ptr,     /* Ptr to socket control block for the socket */
  struct ps_sockaddr* remote_addr,
  uint16 * addrlen,
  sint15 * dss_errno
);

/*===========================================================================
FUNCTION DSSTCP_WRITE()

DESCRIPTION
  Sends dsm memory item out to TCP.

  This function is called from the context of the PS task.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dsstcp_write
(
  protocol_ctl_block_type pcb,              /* protocol control block      */
  struct ps_sockaddr_in6 *ta_ptr,              /* destination socket info     */
  dsm_item_type *dsm_item,                  /* ptr to dsm item chain       */
  ps_meta_info_type *meta_info_ptr          /* meta information - ignored  */
);

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
);

/*===========================================================================
FUNCTION DSSTCP_BIND()

DESCRIPTION
  This is the TCP specific bind() function. It writes the port value into
  local.conn.port of the socket control block structure.

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
  struct ps_sockaddr_in6  *la                                 /* local address */
);

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
sint15 dsstcp_shutdown
(
  protocol_ctl_block_type pcb,                   /* protocol control block */
  int  how,                                     /* what action to perform  */
  sint15* dss_errno                                    /* dss error number */
);

/*===========================================================================
FUNCTION DSSTCP_SETOPT()

DESCRIPTION
  This function sets the option value to the user specified value.

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
);

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
);

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
  protocol_ctl_block_type  pcb_handle,
  int32                   sockfd,
  ps_rt_meta_info_type *  rt_meta_info_ptr
);

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
void dsstcp_set_linger_reset
(
  protocol_ctl_block_type pcb,
  int32    sockfd,
  boolean  is_linger_on,
  int32    time_in_sec
);

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
void dsstcp_cleanup_all_reseq
(
  void
);

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
);

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
);

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
sint15 dsstcp_read
(
  struct scb_s* scb_ptr,     /* Ptr to socket control block for the socket */
  struct ps_iovec  * iov,        /* scatter buffer to which to copy data      */
  uint16 iovcount,               /* length of iovec array                  */
  struct ps_sockaddr *fromaddr,              /* destination address - ignored */
  uint16 * addrlen,                            /* address length - ignored */
  int    flags,                                   /* flags from app        */
  sint15 *dss_errno                                   /* error condition value */
);

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
);

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
);

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
);

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
);

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
);

#endif /* DSSTCP_NEW_H */
