#ifndef DSS_PING_COMM_MGR_H
#define DSS_PING_COMM_MGR_H
/*===========================================================================

         P I N G   C O M M U N I C A T I O N   M A N A G E R   
                         H E A D E R   F I L E
                   
DESCRIPTION

 This header file manages communication functions related to ping - 
 creating and closing sockets, sending pings and reading the responses.

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: $
  $Header: //components/rel/dataiot.mpss/2.0/interface/dss/inc/dss_ping_comm_mgr.h#1 $ $DateTime: 2020/03/17 08:03:16 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/22/07    ss     Created.
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"

//#include "ps_iface_defs.h"
#include "ps_svc.h"
#include "pstimer.h"
#include "dss.h"
#include "list.h"
#include "dss_ping.h"
#include "dssdns.h"
#include "ds_fwk.h"
#include "dssocki.h"



/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
#define   DSS_PING_COOKIE                           0xEC0EC0ECUL
#define   DSS_PING_INFINITE_NUM_PINGS               0xFF
#define   DSS_PING_MAX_PING_DATA_BYTES              256
#define   DSS_PING_MIN_PING_DATA_BYTES              4
#define   DSS_PING_DEFAULT_TIMES_TO_PING            4
#define   DSS_PING_DEFAULT_PING_DATA_BYTES          64
#define   DSS_PING_DEFAULT_TTL                      255

#define   DSS_PING_DEFAULT_PING_RETRANSMIT_TIME     2000   /* ms */
#define   DSS_PING_MIN_PING_RETRANSMIT_TIME          200   /* ms */
#define   DSS_PING_MAX_PING_RETRANSMIT_TIME         5000   /* ms */

#define   DSS_PING_DEFAULT_PING_RESPONSE_TIME_OUT   10000  /* ms */
#define   DSS_PING_MIN_PING_RESPONSE_TIME_OUT       5000   /* ms */
#define   DSS_PING_MAX_PING_RESPONSE_TIME_OUT       10000  /* ms */


typedef enum
{
  DSSICMP_ICMP_V4  = 0,
  DSSICMP_ICMP_V6  = 1
} dssicmp_icmp_ver_type;


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/



/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION DSS_PING_SOCKET()

DESCRIPTION
  This function creates an ICMP socket. NOTE: This function is cannot be
  mapped by the protocol specific function pointer. To create an ICMP
  socket, this function must be invoked directly (instead of through
  dss_socket()).

DEPENDENCIES
  None.

PARAMETERS
  uint8  type       -  ICMP socket type
  uint8  code       -  ICMP socket code
  dss_sock_cb_fcn sock_cb - socket callback function
  void * sock_cb_user_data - User data for socket call back
  dss_net_lib_info_type_s * policy_info_ptr-  Network policy info
  sint15 *dss_errno -  error condition value

RETURN VALUE
  dss_errno Values
  ----------------
  DS_EBADAPP          invalid application ID
  DS_EMFILE           too many descriptors open.  A socket is already open or
                      has not closed compeletely.
  DS_EPROTOTYPE       specified protocol invalid for socket type.


SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_ping_socket
(
  dssicmp_icmp_ver_type  icmp_ver,
  uint8 type,
  uint8 code,
  void  *ping_sess_ptr,
  sint15 *dss_errno
);


/*===========================================================================
FUNCTION DSS_PING_COMM_MGR_INIT_PING_ENGINE

DESCRIPTION
  This function initializes the ping session info structures, and sets
  the command handler for handling outgoing pings.

DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.
===========================================================================*/
void dss_ping_comm_mgr_init_ping_engine
(
  void
);

/*===========================================================================
FUNCTION DSS_PING_COMM_MGR_SEND_PING_CMD_HANDLER

DESCRIPTION
  This function handles the PS_DSS_PING_SEND_PING_CMD command.

DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.
===========================================================================*/

void dss_ping_comm_mgr_send_ping_cmd_handler
(
  ps_cmd_enum_type  cmd,
  void              *user_data_ptr
);

/*===========================================================================
FUNCTION DSS_PING_COMM_MGR_NET_CB_CMD_HANDLER

DESCRIPTION
  This function handles the PS_DSS_PING_NET_CB_CMD command.

DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.
===========================================================================*/
void dss_ping_comm_mgr_net_cb_cmd_handler
(
  ps_cmd_enum_type  cmd,
  void              *user_data_ptr
);

/*===========================================================================
FUNCTION DSS_PING_COMM_MGR_SOCK_CB_CMD_HANDLER

DESCRIPTION
  This function handles the PS_DSS_PING_SOCK_CB_CMD command.

DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.
===========================================================================*/
void dss_ping_comm_mgr_sock_cb_cmd_handler
(
  ps_cmd_enum_type  cmd,
  void              *user_data_ptr
);

/*===========================================================================
FUNCTION DSS_PING_COMM_MGR_RSP_TIMEOUT_CB_CMD_HANDLER

DESCRIPTION
  This function handles the PS_DSS_PING_RSP_TIMEOUT_CB_CMD command.

DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.
===========================================================================*/
void dss_ping_comm_mgr_ping_rsp_timeout_cb_cmd_handler
(
  ps_cmd_enum_type  cmd,
  void              *timeout_data
);


/*===========================================================================
FUNCTION DSS_PING_COMM_MGR_CREATE_PING_SESSION

DESCRIPTION
  This function creates a ping session. Ping retransmit and ping
  response timeout handles are created. No sockets are created here.

DEPENDENCIES
  None.

RETURN VALUE
  On success, a valid handle is returned.
  On error, NULL is returned and the error condition value is placed in 
  *errno_ptr. 
  
  errno_ptr Values
  ----------------
  DS_EFAULT           bad memory address
  DS_ENOMEM           out of memory

SIDE EFFECTS
  None.
===========================================================================*/
dss_ping_handle dss_ping_comm_mgr_create_ping_session
(
  dss_net_lib_info_type_s                *net_policy_ptr,
  dss_ping_config_type                    *user_ping_options,
  dss_ping_callback_fn_type               app_callback_fn,
  dss_ping_sess_summary_callback_fn_type  app_ping_summary_callback_fn,
  void                                    *app_user_data,
  int16                                   *dss_errno
);

/*===========================================================================
FUNCTION DSS_PING_COMM_MGR_ABORT_PING

DESCRIPTION
  This function ends the ping session. If a non-NULL reason_ptr and non-NULL
  errno_ptr are specified, it also calls the application's summary callback 
  function before bringing down the ping session.

DEPENDENCIES
  None.

RETURN VALUE
  DSS_SUCCESS if session was closed successfully.
  DSS_ERROR if an invalid handle was specified.

SIDE EFFECTS
  None.
===========================================================================*/
int16 dss_ping_comm_mgr_abort_ping
(
  dss_ping_handle                     ping_handle,
  dss_ping_session_close_reason_type  *reason_ptr,
  int16                               dss_errno
);

/*===========================================================================
FUNCTION DSS_PING_COMM_MGR_SEND_PING

DESCRIPTION
  This function sends out an ICMP ECHO_REQUEST packet. It then starts a 
  retransmit timer. When this timer expires, the next ping will be sent.

DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.
===========================================================================*/
void dss_ping_comm_mgr_send_ping
(
  dss_ping_handle   ping_handle
);


/*===========================================================================
FUNCTION DSS_PING_COMM_MGR_UPDATE_PING_DEST_ADDR

DESCRIPTION
  This function updates the destination ping address for the specified 
  ping session (indicated through ping_handle) with the information contained 
  in the dss_dns_addrinfo structure.

DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.
===========================================================================*/
void dss_ping_comm_mgr_update_ping_dest_addr
(
  dss_ping_handle     ping_handle,
  struct dss_dns_addrinfo  *addrinfo_ptr
);

/*===========================================================================
FUNCTION DSS_PING_COMM_MGR_PERFORM_DNS_LOOKUP

DESCRIPTION
  This function performs the DNS lookup.

DEPENDENCIES
  None.

PARAMETERS

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
int32 dss_ping_comm_mgr_perform_dns_lookup
(
  dss_ping_handle             ping_handle,
  char *                      dest_addr_ptr,
  int16                       addr_family,
  int16 *                     dss_errno
);


#endif /* DSS_PING_COMM_MGR_H */
