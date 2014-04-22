#ifndef DSS_DNS_PRIV_H
#define DSS_DNS_PRIV_H
/*===========================================================================

    D A T A   S E R V I C E S   D N S   P R I V A T E   A P I   F I L E

DESCRIPTION

 Contains APIs that are used by other modules (STA testing, for example) but
 have not yet been released for general use to api/datacommon.

EXTERNALIZED FUNCTIONS
  dss_dns_add_server_addr()
    Add a session-specific DNS server address.

  dss_dns_clear_server_queue()
    Clear a session-specific DNS server queue.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2021 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#include "dssdns.h"


/*===========================================================================

                            PUBLIC DATA DECLARATIONS

===========================================================================*/


/*===========================================================================
FUNCTION  DSS_DNS_GET_SERVER_ADDR

DESCRIPTION
  This function get a server address from the DNS servers configured by
  the user for this session. 

PARAMETERS
  session_handle  - Session identifier
  server_index    - Requested server index in the queue
  dns_sa_ptr      - Server address. This is an input and output parameter.
                    Client is responsible for allocating the memory.
                    Client shall also set the ss_family field in 
                    dns_sa_ptr->ss_family. The family will be used to 
                    determine if an IPV6 or IPV4 is requested.                   
  dss_errno       - Error code in case of error.

RETURN VALUE
  DSS_SUCCESS on success.
  DSS_ERROR on failure and sets the error code in dss_errno.

  dss_errno values
  ------------
  DS_EBADF         - Wrong session handle
  DS_EAFNOSUPPORT  - Address family not supported
  DS_EFAULT        - Invalid arguments
  DS_EINVAL        - Invalid operation in case no server exists in this index

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
int16 dss_dns_get_server_addr
(
  dss_dns_session_mgr_handle_type   session_handle,
  uint16                            server_index,
  struct ps_sockaddr_storage      * dns_sa_ptr,
  int16                           * dss_errno
);

/*===========================================================================
FUNCTION  DSS_DNS_SESSION_MGR_GET_SERVERS_COUNT

DESCRIPTION
  Get the count of servers in the session's DNS servers queue.
  The count of the IPV4 queue or IPV6 is returned based on the 
  address family parameter.

PARAMETERS
  session_handle     -  Session handle.
  addr_family        -  Address family. 
  count              -  Returns the count.

RETURN VALUE
  DSS_SUCCESS on success.
  DSS_ERROR on error.

  dss_errno values
  ----------------
  DS_EFAULT          - Invalid arguments.
  DS_EAFNOSUPPORT    - Address family is not IPV4 or IPV6.
  DS_EBADF           - Invalid session handle.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
int16 dss_dns_session_mgr_get_servers_count
(
  dss_dns_session_mgr_handle_type   session_handle,
  uint16                            addr_family,
  uint16                          * count,
  int16                           * dss_errno
);

/*===========================================================================
FUNCTION DSS_DNS_SESSION_MGR_IS_NET_POLICY_SET

DESCRIPTION
  This function determines if app has set network policy or iface id for
  a particular session.

PARAMETERS
  session_handle    - Session manager control block handle

RETURN VALUE
  TRUE - if network policy or iface id is set.
  FALSE - otherwise.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
boolean dss_dns_session_mgr_is_net_policy_set
(
  dss_dns_session_mgr_handle_type      session_handle
);

#endif /* DSS_DNS_PRIV_H */
