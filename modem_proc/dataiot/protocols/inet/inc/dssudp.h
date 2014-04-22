#ifndef DSSUDP_NEW_H
#define DSSUDP_NEW_H
/*===========================================================================

        D A T A  S E R V I C E S  S O C K E T  U D P  F I L E

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
    Processes rxed UDP packet and checks if any socket is waiting for it.

  dssudp_new_ip_upcall()
    If a new IP address is negotiated with the Base Station, then update
    udp ctrl blocks of the sockets using the interface with the new address

dssudp_set_rscope()
    UDP specific set_rscope function. Sets the incoming route scope in
    the UCB control block.
  dssudp_set_route_meta_info()
    This function sets routing meta info inside IPCB.

Copyright (c) 1998-2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dssudp.h_v   1.1   08 Oct 2002 23:17:18   akuzhiyi  $
  $Header: //components/rel/dataiot.mpss/2.0/protocols/inet/inc/dssudp.h#1 $ $DateTime: 2020/03/17 08:03:16 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/28/18   ds      Code changes for chiltepin(9x05).
03/13/08   pp      Metainfo optimizations.
07/31/06   rt      Route Scope modifications.
08/15/05   vp      Support for IP_RECVERR.
04/18/05   vp      Change in signature of bind and listen functions.
03/30/04   vp      Merged changes from June04Dev_05.00.00 PS branch.
01/14/04   vp      Added declaration of function dssudp_new_ip_upcall()
10/17/03   sv      Modified protocol specific functions to take PCB instead
                   of SCB.
08/18/03   sv      Modified the code to use IPv6 addresses internally.
08/01/03   ss/sv   Restructured the code to support IPv6
05/28/03   ss      Changed the prototype of process_rx_pkt() to include
                   payload length
05/27/03   om      Added dssudp_read_dsm_chain().
04/15/03   sv      Added dssudp_shutdown, dssudp_connect_setup and
                   dssudp_lookup_conn function declarations. Added UDP
                   Header length (UDPHLEN) constant.
04/15/03   sv      Modified dssudp_read to accept iov and iovcount as
                   arguments. Modified dssudp_rx_packet to accept ucb_ptr
                   and data item pointer as arguments.
10/08/02   aku     Removed FEATURE_DATA_MM featurization.
08/04/02   aku     Merged in changes for multimode ps under FEATURE_DATA_MM
07/15/02   na      Added prototype for dssudp_process_rx_pkt().
05/02/02   mvl     File clean up, added meta_info_ptr to write() prototype
03/27/01   mt      Changed type of second argument in dssudp_read() from
                   byte * to void *.
03/27/01   js      Added extra parameter to function dssudp_write(). The
                   extra parameter holds a pointer to the structure holding
                   the destination information. This avoids using the
                   destination information in scb, which may be stale.
11/10/00    snn    Renamed scb_type as scb_s
08/03/00   mvl/js  Added prototype for dssudp_bind() function.
12/09/98   na/hcg  Created module.
===========================================================================*/
/*===========================================================================

                       INCLUDE FILES FOR THE MODULE

===========================================================================*/
//#include "ds_Sock_Def.h"
#include "ps_in.h"
#include "ps_udp_io.h"
//#include "ps_iface_defs.h"
#include "ps_socket_defs.h"
#include "ps_rt_meta_info.h"
#include "ps_pkt_meta_info.h"
#include "dssocki.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
#define UDPHLEN 8


#ifdef __cplusplus
extern "C" {
#endif
/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
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

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dssudp_socket
(
  struct scb_s* scb_ptr,     /* Ptr to socket control block for the socket */
  sint15 *dss_errno                                   /* error condition value */
);

/*===========================================================================
FUNCTION DSSUDP_WRITE()

DESCRIPTION
  Sends dsm memory item out to UDP using udp_output found in udp.c.

  This function is called from the context of the PS task.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dssudp_write
(
  protocol_ctl_block_type pcb,             /* protocol control block       */
  struct ps_sockaddr_in6 *ta_ptr,             /* destination socket info      */
  dsm_item_type *item_ptr,                 /* ptr to head of dsm pkt chain */
  ps_meta_info_type *meta_info_ptr         /* ptr to meta info             */
);

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
sint15 dssudp_close
(
  protocol_ctl_block_type  pcb,   /* Protocol control block for the socket */
  boolean silent_close_flag,                          /* silent close flag */
  sint15 *dss_errno                                   /* error condition value */
);

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
);

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
);

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
);

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
);

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
);


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
);

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
);

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
);

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
);




#if 0
/*===========================================================================
FUNCTION DSSUDP_SET_ROUTE_SCOPE()

DESCRIPTION
  This is the UDP specific set_route_scope() function. It writes the route
  scope in the UCB's IPCB

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dssudp_set_route_scope
(
  int32                  pcb_handle,
  int32                  sockfd,
  ps_route_scope_type  * route_scope_ptr
);

#endif
#ifdef __cplusplus
}
#endif

#endif /* DSSUDP_NEW_H */
