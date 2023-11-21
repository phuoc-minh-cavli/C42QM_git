/*===========================================================================

                      P S _ D N S I _ R E S O L V E R . C

DESCRIPTION
  The Data Services DNS Subsystem Resolver module. Contains
  definitions of functions, variables, macros, structs and enums
  used by DNS subsystem resolver.

  The following functions are exported to the DNS subsystem.

EXTERNALIZED FUNCTIONS
  ps_dnsi_resolve()
    This function takes the query type, query class and query data, and
    returns query identifier corresponding to the query.

INTIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2019 - 2022 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
//TODO Clean up all buf_index etc
//Add ps_handle_is_valid call to handle manager.
//delete and free instance should be one function.
//create and get should be one function.
//Resolver state manager function should contain only things that are reused.

/*===========================================================================

                             EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Note that changes are listed in reverse chronological order.

  $Header: //components/rel/dataiot.mpss/2.0/interface/dns/src/ps_dnsi_resolver.c#12 $$DateTime: 2022/09/19 01:44:54 $$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/27/19   ks      Fixed KW issues.
05/21/19   sr      Create Module

===========================================================================*/

/*===========================================================================

                            INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include <stringl/stringl.h>
#include "dssdns.h"
#include "dss_dns_config.h"
#include "dss_dns_priv.h"
#include "ps_handle_mgr.h"
#include "ps_dnsi_resolver.h"
#include "ps_dnsi_msg_mgr.h"
#include "ps_mem.h"
#include "ps_dnsi_mem_mgr.h"
#include "dss.h"
#include "ps_svc.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_utils.h"
#include "dssocki.h"
#include "ps_dns_cache.h"
#include "ds_fwk.h"
#include "ps_dnsi_io_mgr.h"

#ifdef FEATURE_DATAMODEM_DNS

/*===========================================================================

                                LOCAL DATA DEFINITIONS

===========================================================================*/
#define RESOLVER_CLIENT (PS_HANDLE_MGR_CLIENT_DNS_RESOLVER)
static q_type  res_instance_q;

/*---------------------------------------------------------------------------
  Tuning the number of ps dnsi resolver buffers needed by this module
---------------------------------------------------------------------------*/
#define PS_DNSI_RESOLVER_BUF_SIZE  ((sizeof (ps_dnsi_resolver_cb_type) + 3) & ~3)

#ifndef FEATURE_DATA_PS_LOW_MEM_CHIPSET

#define PS_DNSI_RESOLVER_BUF_NUM        DSS_DNS_MAX_QUERIES
#define PS_DNSI_RESOLVER_BUF_HIGH_WM    ((PS_DNSI_RESOLVER_BUF_NUM) - 1)
#define PS_DNSI_RESOLVER_BUF_LOW_WM     1

#else

#define PS_DNSI_RESOLVER_BUF_NUM        1
#define PS_DNSI_RESOLVER_BUF_HIGH_WM    1
#define PS_DNSI_RESOLVER_BUF_LOW_WM     0
#endif /* FEATURE_DATA_PS_LOW_MEM_CHIPSET */

/*----------------------------------------------------------------------------
  Allocate memory to hold ps_dnsi_resolver along with ps_mem header
----------------------------------------------------------------------------*/
static int ps_dnsi_resolver_buf_mem[PS_MEM_GET_TOT_SIZE_OPT
                                    (
                                      PS_DNSI_RESOLVER_BUF_NUM,
                                      PS_DNSI_RESOLVER_BUF_SIZE
                                   )];

#ifdef FEATURE_DATA_PS_MEM_DEBUG
/*----------------------------------------------------------------------------
  Array of pointers used to facilitate easy debugging.  The first one points
  to the ps_mem header and the latter points to ps_dnsi_resolver_buf
----------------------------------------------------------------------------*/
static ps_mem_buf_hdr_type       * ps_dnsi_resolver_buf_hdr[PS_DNSI_RESOLVER_BUF_NUM];
static ps_dnsi_resolver_cb_type  * ps_dnsi_resolver_buf_ptr[PS_DNSI_RESOLVER_BUF_NUM];
#endif /* FEATURE_DATA_PS_MEM_DEBUG */

boolean dns_reserved_bits_config = 0;

typedef struct
{
   ps_dnsi_resolver_cb_type * res_cb_ptr;
   sint15                     netlib_handle;
//   dss_iface_id_type          iface_id;
  ds_fwk_s_type               *fwk_inst;
} iface_event_cb_user_data;
/*===========================================================================

                         EXTERN FUNCTION DECLARATIONS

===========================================================================*/

extern ps_dnsi_api_cb_type * ps_dnsi_api_is_handle_valid
(
  dss_dns_query_handle_type  api_cb_handle
);

/*===========================================================================

                         INTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  Forward declarations for network event, IO manager event and iface event 
  notification functions. These are required because the Resolver state-event
  manager function and these notification function recursively call each other.
---------------------------------------------------------------------------*/
static void ps_dnsi_resolver_net_event_notify
(
  dss_net_callback_info_s *net_callback_info
);

static void ps_dnsi_resolver_io_event_notify
(
  ps_dnsi_io_mgr_handle_type         io_mgr_handle,
  ps_dnsi_io_mgr_event_enum_type     io_mgr_event,
  dsm_item_type                    * dns_response_ptr,
  void                             * user_data_ptr,
  int16                              ps_errno
);

static void ps_dnsi_resolveri_iface_event_cmd_handler
(
  ps_cmd_enum_type                   ps_cmd,
  void                             * res_data
);

/*===========================================================================
FUNCTION PS_DNSI_RESOLVERI_IS_HANDLE_VALID

DESCRIPTION
  Checks if the specified handle is valid.

PARAMETERS
  res_handle  - Resolver control block handle

RETURN VALUE
  Pointer to resolver control block corresponding to the handle
  if the specified handle is valid.
  NULL if the specified handle is not valid.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static ps_dnsi_resolver_cb_type  * ps_dnsi_resolveri_is_handle_valid
(
  ps_dnsi_resolver_handle_type  res_handle
)
{
  ps_dnsi_resolver_cb_type    * res_cb_ptr;
  int16                         buf_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (PS_DNSI_RESOLVER_INVALID_HANDLE == res_handle)
  {
    LOG_MSG_ERROR_1 ("Invalid resolver handle %d", res_handle);
    return NULL;
  }

  buf_index = ps_handle_mgr_get_index(RESOLVER_CLIENT, res_handle);
  if (PS_HANDLE_MGR_INVALID_INDEX == buf_index)
  {
    LOG_MSG_ERROR_1 ("Invalid resolver handle %d", res_handle);
    return NULL;
  }

  res_cb_ptr =
    (ps_dnsi_resolver_cb_type *)
      ps_mem_index_to_buf(buf_index, PS_MEM_DNSI_RESOLVER_CB_TYPE);
  if (NULL == res_cb_ptr || res_cb_ptr->handle != res_handle)
  {
    LOG_MSG_ERROR_2 ("Inv res CB 0x%p from handle %d", res_cb_ptr, res_handle);
    return NULL;
  }

  return res_cb_ptr;
} /* ps_dnsi_resolveri_is_handle_valid() */

/*===========================================================================
FUNCTION PS_DNSI_RESOLVERI_CALC_MAX_SERVERS

DESCRIPTION
  Return the maximum configured DNS servers based on the iface family  
  and the session's parameters and queue. The value is not the actual configured servers 
  but only an upper limit.
  If iface_id or session handle are not valid this function will return 0 as the
  max configured servers.

PARAMETERS
  iface_id         - Interface Identifier.
  session_handle   - Session handle.

DEPENDENCIES
  None.
RETURN VALUE
  Max number of configured servers

SIDE EFFECTS
  None.
===========================================================================*/
static int16 ps_dnsi_resolveri_calc_max_servers
(
  uint16                            addr_family,
  dss_dns_session_mgr_handle_type   session_handle
)
{
  int16                                      retval = DSS_ERROR;
  int16                                      ps_errno; 
  uint16                                     max_configured_servers = 0;
  dss_dns_client_specified_servers_enum_type client_specified_servers_behavior = USE_LAST;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
  Get the number of DNS servers on the session's queue
  -------------------------------------------------------------------------*/
  retval = 
    dss_dns_session_mgr_get_servers_count(session_handle,
                                          ((IPV4_ADDR == addr_family)?
                                          DSS_AF_INET:
                                          DSS_AF_INET6),
                                          &max_configured_servers,
                                          &ps_errno);

  if (DSS_SUCCESS != retval)
  {
    LOG_MSG_ERROR_1 ("Failed to get servers count from session err %d",
                     ps_errno);
    return 0;
  }

  /*-----------------------------------------------------------------------
  Check if iface servers should be added to the count
  -------------------------------------------------------------------------*/
  retval = 
    dss_dns_get_config_params(session_handle,
                              DSS_DNS_CONFIG_PARAMS_CLIENT_SPECIFIED_SERVERS,
                              &client_specified_servers_behavior,
                              sizeof (client_specified_servers_behavior),
                              &ps_errno);
  if (DSS_ERROR == retval)
  {
    LOG_MSG_ERROR_0 ("Get session info for CLIENT_SPECIFIED_SERVERS failed");
    return 0;    
  }
  
  if (USE_EXCLUSIVELY != client_specified_servers_behavior)
  {/*if servers defined by the iface layer should be used then
   add them to the count*/
    max_configured_servers += (int)PS_DNSI_SERVER_TYPE_MAX_IFACE_SERVERS;
  }
  return max_configured_servers;
}

/*===========================================================================
FUNCTION PS_DNSI_RESOLVERI_CALC_SERVER_SOURCE

DESCRIPTION
  Calculate based on the server_index and the iface if the 
  server address should be retrieved from iface or from the session's DNS servers 
  address queue.
  In addition it calculates the relative index to use for retrieving it
  (from iface or from the session).
  For example if based on the session parameters the iface servers should be used 
  first then to get the correct relative index into the session servers queue
  we need to subtract PS_DNSI_SERVER_TYPE_MAX_IFACE_SERVERS.
  
PARAMETERS
  iface_id              - Interface Identifier.
  session_handle        - Session handle.
  server_index          - Server_index (input parameter). 
  server_is_from_iface  - if TRUE indicates if the server should be retrieved from 
                          the iface layer. (output parameter)
  relative_server_index - Relative server index that should be used to 
                          retrieve the server address.(output parameter)
  addr_family           - address family (output parameter).

  
DEPENDENCIES

RETURN VALUE
  DSS_SUCCESS if the server address could be found.
  DSS_ERROR   on error and sets the errno to error code.

SIDE EFFECTS
None.
===========================================================================*/
static int16 ps_dnsi_resolveri_calc_server_source
(
 int16                             netlib_handle,
 dss_dns_session_mgr_handle_type   session_handle,
 uint16                            server_index,
 boolean                          *server_is_from_iface,
 uint16                           *relative_server_index,
 dss_iface_ioctl_ip_family_type   *addr_family,
 int16                            *ps_errno
)
{
  int16         retval = DSS_ERROR;
  dss_dns_client_specified_servers_enum_type client_specified_servers_behavior = USE_LAST;
  uint16        session_dns_server_index = server_index;
  uint16        iface_dns_server_index = server_index;
  uint16        max_configured_servers;
  ds_pdn_ioctl_ip_addr_type ip_addr;

  if ( NULL == ps_errno )
  {
    LOG_MSG_ERROR_0 ("ps_dnsi_resolveri_calc_server_source(): NULL errno");
    return DSS_ERROR;
  }
  if ( NULL == server_is_from_iface ||
       NULL == relative_server_index ||
       NULL == addr_family)
  {
    LOG_MSG_ERROR_0 ("ps_dnsi_resolveri_calc_server_source(): NULL parameters");
    *ps_errno = DS_EFAULT;
    return DSS_ERROR;
  }


  /*-------------------------------------------------------------------------
    Get the IP family corresponding to the net handle
  -------------------------------------------------------------------------*/
  retval = ds_pdn_ioctl(DS_FWK_CLIENT_SOCKETS,
                        netlib_handle,
                        DS_PDN_IOCTL_GET_IP_ADDR, 
                        (void *) &ip_addr, 
                         ps_errno);
  if (DSS_SUCCESS != retval)
  {
    return DSS_ERROR;
  }

  *addr_family = ip_addr.type;
  *server_is_from_iface = FALSE;

  /*-----------------------------------------------------------------------
  Check if iface servers should be added to the count
  -------------------------------------------------------------------------*/
  retval = 
    dss_dns_get_config_params(session_handle,
                              DSS_DNS_CONFIG_PARAMS_CLIENT_SPECIFIED_SERVERS,
                              &client_specified_servers_behavior,
                              sizeof (client_specified_servers_behavior),
                              ps_errno);
  if (DSS_ERROR == retval)
  {
    LOG_MSG_ERROR_0 ("Get session info for CLIENT_SPECIFIED_SERVERS failed");
    return DSS_ERROR;    
  }

  if (USE_EXCLUSIVELY != client_specified_servers_behavior)
  {
    /*Use iface DNS servers. Check if they should be used last or first*/
    if (USE_LAST == client_specified_servers_behavior)
    {
      /*if the iface DNS servers need to be used first then 
      the counting into the session's DNS servers queue is starting from
      PS_DNSI_SERVER_TYPE_MAX_IFACE_SERVERS. This means the first indexes belong to
      the iface DNS servers*/
      if (server_index < (int)PS_DNSI_SERVER_TYPE_MAX_IFACE_SERVERS)
      {
        *server_is_from_iface = TRUE;
      }
      else
      {
        session_dns_server_index = server_index -
                                   (int)PS_DNSI_SERVER_TYPE_MAX_IFACE_SERVERS;
      }
    }
    else
    {
      /*index of iface_dns_server_index should start as offset
      from the session's DNS server count*/
      retval = 
        dss_dns_session_mgr_get_servers_count(session_handle,
                                              ((IPV4_ADDR == *addr_family)?
                                                DSS_AF_INET:
                                                DSS_AF_INET6),
                                                &max_configured_servers,
                                                ps_errno);
      if (DSS_SUCCESS != retval)
      {
        LOG_MSG_ERROR_1 ("Failed to get servers count from session err %d",
                         ps_errno);
        return DSS_ERROR;
      }
      if ( server_index >= max_configured_servers )
      {
        iface_dns_server_index = server_index - max_configured_servers;
        *server_is_from_iface = TRUE;
      }
    }/* else of if (USE_LAST == client_specified_servers_behavior)*/

  }/*if (USE_EXCLUSIVELY != client_specified_servers_behavior)*/

  *relative_server_index = (*server_is_from_iface)?
                           iface_dns_server_index:session_dns_server_index;
  return DSS_SUCCESS;
}

/*===========================================================================
FUNCTION PS_DNSI_RESOLVERI_GET_SERVER_ADDR

DESCRIPTION
  Gets the DNS Server address associated with the interface.

PARAMETERS
  server_index   - Primary or secondary DNS server address.
  iface_id       - Interface Identifier.
  session_handle - Session handle.
  dns_sa_ptr     - Pointer to socket address structure where the
                   results should be returned.

DEPENDENCIES
  Memory for dns_sa_ptr (sizeof (struct ps_sockaddr_storage)) should be
  provided by the caller.

RETURN VALUE
  DSS_SUCCESS if the server address could be found
  DSS_ERROR   on error and sets the errno to error code.

SIDE EFFECTS
  None.
===========================================================================*/
static int16 ps_dnsi_resolveri_get_server_addr
(
  uint16                             server_index,
  int16                              netlib_handle,
  dss_dns_session_mgr_handle_type   session_handle,
  struct ps_sockaddr_storage      * dns_sa_ptr,
  int16                           * ps_errno
)
{
  int16                                      retval = DSS_ERROR;
  ip_addr_type                               dns_addr[2];
  ds_pdn_ioctl_get_all_dns_addrs_type        dns_addrs={0};
  struct ps_sockaddr_in                    * temp_sin_ptr;
  struct ps_sockaddr_in6                   * temp_sin6_ptr;
  dss_iface_ioctl_ip_family_type             addr_family;
  boolean                                    server_is_from_iface;
  uint16                                     relative_server_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( NULL == dns_sa_ptr)
  {
    *ps_errno = DS_EFAULT;
    goto bail;
  }
  
  retval = ps_dnsi_resolveri_calc_server_source(netlib_handle,
                                                session_handle,
                                                server_index,
                                                &server_is_from_iface,
                                                &relative_server_index,
                                                &addr_family,
                                                ps_errno);
 
  if (DSS_SUCCESS != retval)
  {
    return retval;
  }

  if (!server_is_from_iface)
  {/*requested server is from the session queue*/
    dns_sa_ptr->ps_ss_family = (addr_family == IPV4_ADDR)?
                                      DSS_AF_INET:DSS_AF_INET6;

    retval = dss_dns_get_server_addr(session_handle,
                                     relative_server_index,
                                     dns_sa_ptr,
                                     ps_errno);
    if (DSS_SUCCESS != retval)
    {
      goto bail;
    }      

  if (addr_family == DSS_AF_INET6)
  {
    temp_sin6_ptr = (struct ps_sockaddr_in6 *) dns_sa_ptr;
    temp_sin6_ptr->ps_sin6_family = DSS_AF_INET6;
    temp_sin6_ptr->ps_sin6_port   = ps_htons(DNS_LOOKUP_PORT);

    temp_sin6_ptr->ps_sin6_addr.ps_s6_addr64[0] = ((struct ps_sockaddr_in6 *)(dns_sa_ptr))->ps_sin6_addr.ps_s6_addr64[0] ;
    temp_sin6_ptr->ps_sin6_addr.ps_s6_addr64[1] = ((struct ps_sockaddr_in6 *)(dns_sa_ptr))->ps_sin6_addr.ps_s6_addr64[1] ;
  }
  else if (addr_family == DSS_AF_INET)
  {
    temp_sin_ptr = (struct ps_sockaddr_in *) dns_sa_ptr;
    temp_sin_ptr->ps_sin_family      = DSS_AF_INET;
    temp_sin_ptr->ps_sin_port        = ps_htons(DNS_LOOKUP_PORT);

    temp_sin_ptr->ps_sin_addr.ps_s_addr = ((struct ps_sockaddr_in *)(dns_sa_ptr))->ps_sin_addr.ps_s_addr;

    LOG_MSG_INFO2_1("ps_dnsi_resolveri_get_server_addr"
                  "Port %d and Addr is ",
                  temp_sin_ptr->ps_sin_port);
    IPV4_ADDR_MSG(temp_sin_ptr->ps_sin_addr.ps_s_addr);

    }
    else
    {
      *ps_errno = DS_EINVAL;
       goto bail;
    }
    return DSS_SUCCESS;
  }/*requested server is from the session queue*/
  
  /*requested server is from iface*/
  if ((int)PS_DNSI_SERVER_TYPE_PRIMARY != relative_server_index &&
      (int)PS_DNSI_SERVER_TYPE_SECONDARY != relative_server_index)
  {
    *ps_errno = DS_EINVAL;
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Perform the IOCTL operation to get the DNS server address.
  -------------------------------------------------------------------------*/
  dns_addrs.num_dns_addrs =2;
  dns_addrs.dns_addrs_ptr = dns_addr;

  retval = ds_pdn_ioctl(DS_FWK_CLIENT_SOCKETS,
                        netlib_handle,
                        DS_PDN_IOCTL_GET_ALL_DNS_ADDR,
                        &dns_addrs,
                        ps_errno);
  if (DSS_SUCCESS != retval)
  {
    goto bail;
  }

  if ((IPV4_ADDR == dns_addr[0].type && 0 == dns_addr[0].addr.v4)&&
      (IPV4_ADDR == dns_addr[1].type && 0 == dns_addr[1].addr.v4))
  {
    LOG_MSG_ERROR_1("Error getting v4 DNS server address on NetHandle %d",
                    netlib_handle);
    *ps_errno = DS_EINVAL;
    goto bail;
  } 

  if ((IPV6_ADDR == dns_addr[0].type && 
       (0 == dns_addr[0].addr.v6[0] && 0 == dns_addr[0].addr.v6[1]))&&
       (IPV6_ADDR == dns_addr[1].type && 
       (0 == dns_addr[1].addr.v6[0] && 0 == dns_addr[1].addr.v6[1])))
  {
    LOG_MSG_ERROR_1("Error getting v6 DNS server address on NetHandle %d",
                    netlib_handle);
    *ps_errno = DS_EINVAL;
    goto bail;
  }

  if (addr_family == DSS_AF_INET6)
  {
    temp_sin6_ptr = (struct ps_sockaddr_in6 *) dns_sa_ptr;
    memset(temp_sin6_ptr, 0, sizeof (struct ps_sockaddr_in6));
    temp_sin6_ptr->ps_sin6_family = DSS_AF_INET6;
    temp_sin6_ptr->ps_sin6_port   = ps_htons(DNS_LOOKUP_PORT);

    if ((0 != dns_addr[0].addr.v6[0]) &&
        (PS_DNSI_SERVER_TYPE_PRIMARY == relative_server_index))
    {
      temp_sin6_ptr->ps_sin6_addr.ps_s6_addr64[0] = dns_addr[0].addr.v6[0];
      temp_sin6_ptr->ps_sin6_addr.ps_s6_addr64[1] = dns_addr[0].addr.v6[1];
    }
    else if ((0 != dns_addr[1].addr.v6[0])&&
             (PS_DNSI_SERVER_TYPE_SECONDARY == relative_server_index))
    {
      temp_sin6_ptr->ps_sin6_addr.ps_s6_addr64[0] = dns_addr[1].addr.v6[0];
      temp_sin6_ptr->ps_sin6_addr.ps_s6_addr64[1] = dns_addr[1].addr.v6[1];
    }
  }
  else if (addr_family == DSS_AF_INET)
  {
    temp_sin_ptr = (struct ps_sockaddr_in *) dns_sa_ptr;
    memset(temp_sin_ptr, 0, sizeof (struct ps_sockaddr_in));
    temp_sin_ptr->ps_sin_family      = DSS_AF_INET;
    temp_sin_ptr->ps_sin_port        = ps_htons(DNS_LOOKUP_PORT);
    if ((0 != dns_addr[0].addr.v4) &&
        (PS_DNSI_SERVER_TYPE_PRIMARY == relative_server_index))
    {
    temp_sin_ptr->ps_sin_addr.ps_s_addr = dns_addr[0].addr.v4;
  }
    else if ((0 != dns_addr[1].addr.v4) &&
             (PS_DNSI_SERVER_TYPE_SECONDARY == relative_server_index))
    {
      temp_sin_ptr->ps_sin_addr.ps_s_addr = dns_addr[1].addr.v4;
    }	
  }
  else
  {
    *ps_errno = DS_EINVAL;
    goto bail;
  }

  return DSS_SUCCESS;

bail:
  LOG_MSG_ERROR_1 ("Err %d getting DNS server addr", *ps_errno);
  return DSS_ERROR;

} /* ps_dnsi_resolveri_get_server_addr() */

/*===========================================================================
FUNCTION PS_DNSI_RESOLVERI_IFACE_EV_CBACK()

DESCRIPTION
  This callback function handles the iface events posted by DSS module.

PARAMETERS
  iface_event       - Interface event.
  event_info        - Info code attached to the event.
  user_data_ptr     - Resolver user data (Pointer to resolver control block).
  dss_nethandle     - Handle to network manager on which the event occurred.
  iface_id          - The id of the interface in use.

RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void ps_dnsi_resolveri_iface_ev_cback
(
  ds_pdn_event_enum_type                   pdn_event,
  ds_pdn_event_info_u_type                 event_info,
  void                                   * user_data_ptr,
  sint15                                   dss_nethandle
)
{
  iface_event_cb_user_data  * cmd_data = NULL;
  ps_cmd_enum_type            cmd;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2_2("ps_dnsi_resolveri_iface_ev_cback(): "
                  "Recvd event %d for Resolver 0x%p ",
                  pdn_event, user_data_ptr);

  if (NULL == user_data_ptr)
  {
    LOG_MSG_ERROR_0("ps_dnsi_resolveri_iface_ev_cback(): "
                    "NULL user data");
    return;
  }

  if (!ps_mem_is_valid(user_data_ptr, PS_MEM_DNSI_RESOLVER_CB_TYPE))
  {
    LOG_MSG_ERROR_1("ps_dnsi_resolveri_iface_ev_cback(): "
                    "Invalid user data 0x%p", user_data_ptr);
    return;
  }

  cmd_data = (iface_event_cb_user_data *) ps_dnsi_mem_alloc(sizeof(iface_event_cb_user_data));
  if (NULL == cmd_data)
  {
    LOG_MSG_ERROR_0("ps_dnsi_resolveri_iface_ev_cback(): "
                    "Can't allocate cmd_data");
    return;
  }
  cmd_data->res_cb_ptr = (ps_dnsi_resolver_cb_type *) user_data_ptr;
  cmd_data->netlib_handle = dss_nethandle;
  //cmd_data->fwk_inst = fwk_inst;

  if (PDN_UP_EV == pdn_event)
  {
    cmd = PS_DNS_RESOLVER_IFACE_DOWN_CMD;
  }
  else if (PDN_ADDR_CHANGED_EV == pdn_event)
  {
    cmd = PS_DNS_RESOLVER_IFACE_ADDR_CHANGED_CMD;
  }
  else
  {
    LOG_MSG_ERROR_1("ps_dnsi_resolveri_iface_ev_cback(): "
                    "Unexpected event %d", pdn_event);
    return;
  }

  ps_send_cmd(cmd, cmd_data);
  return;

} /* ps_dnsi_resolveri_iface_ev_cback() */

/*===========================================================================
FUNCTION PS_DNSI_RESOLVERI_RUN

DESCRIPTION
  The Resolver state-event manager function.

  This function can be invoked in:

  START_STATE         by PS_DNS_RESOLVE_CMD command handler.
  NETWORK_WAIT_STATE  by NET_MGR_NETUP_EVENT/NET_MGR_ERROR_EVENT.
  IO_WAIT_STATE       by IO_MGR_READ_EVENT/IO_MGR_ERROR_EVENT/NET_ERROR_EVENT.
  FAIL_STATE          by One of the ERROR_EVENTS.

PARAMETERS
  res_cb_ptr     - Pointer to the resolver control block.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void ps_dnsi_resolveri_run
(
  ps_dnsi_resolver_cb_type      * res_cb_ptr
)
{
  boolean                          iterate = TRUE;
  int16                            ps_errno = DSS_SUCCESS;
  struct ps_sockaddr_storage       dns_serv_addr;
  boolean                          retry_flag = FALSE;
  boolean                          add_to_cache = FALSE;
  int16                            retval;
  int32                            net_timeout_interval = 0;
  int32                            max_net_retries = 0;
  dss_dns_linear_backoff_retry_info_type
                                   query_retry_params;
  int64                            query_timeout;
  int32                            get_server_attempts_in_try_iteration;
  int32                            max_get_server_attempts_in_try_iteration;
  boolean                          is_recursion_desired = TRUE;
  boolean                          do_not_retry_network = FALSE;
  ds_pdn_ioctl_ev_cb_type          pdn_ev_cb;
  ps_dnsi_io_mgr_cb_type         * io_mgr_cb_ptr = NULL;
  ps_dnsi_generic_rr_type        * rr_node = NULL;
  ps_dnsi_api_cb_type            * api_cb_ptr = NULL;
  dss_net_lib_info_type_s        net_params;
  uint8                          apn_length = 0;
  dss_net_callback_info_s        *net_callback_info = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Validate resolver ptr.
  -------------------------------------------------------------------------*/
  if (NULL == res_cb_ptr)
  {
    LOG_MSG_ERROR_0 ("Resolver control block is NULL");
    return;
  }

  if (res_cb_ptr != ps_dnsi_resolveri_is_handle_valid(res_cb_ptr->handle))
  {
    LOG_MSG_ERROR_0 ("Invalid resolver control block 0x%p");
    return;
  }

  if (PS_DNSI_RESOLVER_EVENT_MAX <= res_cb_ptr->event)
  {
    LOG_MSG_ERROR_1 ("Invalid resolver event %d", res_cb_ptr->event);
    ASSERT (0);
    return;
  }

  if (PS_DNSI_RESOLVER_STATE_MAX <= res_cb_ptr->state)
  {
    LOG_MSG_ERROR_1 ("Invalid resolver state %d", res_cb_ptr->event);
    ASSERT (0);
    return;
  }

  while (TRUE == iterate)
  {
    iterate = FALSE;
    switch (res_cb_ptr->state)
    {
      case  PS_DNSI_RESOLVER_STATE_START:
        /*-------------------------------------------------------------------
          We should come here when event mask is RESOLVER_EVENT_NONE.
          If the iface id parameter is set, go to NETWORK_OPEN state.
          If not, try to bring up network
            if FAILURE and EWOULDBLOCK, go to NETWORK_WAIT state
            else go to FAIL state
        -------------------------------------------------------------------*/

        DS_ASSERT (PS_DNSI_RESOLVER_EVENT_NONE == res_cb_ptr->event);

        /*-----------------------------------------------------------------
          Bring up the data call.
        -----------------------------------------------------------------*/

        if (dss_dns_session_mgr_is_net_policy_set(res_cb_ptr->session_handle))
        {
          retval = dss_dns_get_config_params (res_cb_ptr->session_handle,
                                              DSS_DNS_CONFIG_PARAMS_NET_POLICY,
                                              &res_cb_ptr->net_policy,
                                              sizeof (dss_net_lib_info_type_s),
                                              &ps_errno);
          if (DSS_ERROR == retval)
          {
            LOG_MSG_ERROR_1 ("Get session info for session %d failed",
                             res_cb_ptr->session_handle);
            res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_FAIL;
            res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_ERROR_SESSION_MGR;
            iterate = TRUE;
            break;
          }
	    }

        api_cb_ptr = (ps_dnsi_api_cb_type*)res_cb_ptr->user_data_ptr;

        if (api_cb_ptr == NULL)
        {
          LOG_MSG_ERROR_0 ("api_cb_ptr is NULL");
          res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_FAIL;
          res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_ERROR_NET_MGR;
          iterate = TRUE;
          break;
        }

        memset(&net_params, 0 ,sizeof(dss_net_lib_info_type_s));

        net_params.family = api_cb_ptr->addr_family;
        net_params.net_cb = ps_dnsi_resolver_net_event_notify;
        net_params.net_cb_user_data = res_cb_ptr;
        net_params.sock_cb = ps_dnsi_io_mgri_sock_event_cback;
        net_params.sock_cb_user_data = res_cb_ptr;
        net_params.apn_type = DS_APN_TYPE_UNSPECIFIED;

        if (res_cb_ptr->net_policy.name != NULL)
        {
          apn_length = strlen((const char*)res_cb_ptr->net_policy.name);
          if (apn_length)
          {
            DS_SYSTEM_HEAP_MEM_ALLOC((net_params.name),
                                     apn_length+1,
                                     byte*);
            if (net_params.name == NULL)
            {
              LOG_MSG_ERROR_0 ("Unable to allocate memory for APN Name.");
              res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_FAIL;
              res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_ERROR_NET_MGR;
              iterate = FALSE;
              break;
            }
            memset (net_params.name, 0 , apn_length+1);
            strlcpy((char*)net_params.name,
                     (const char*)res_cb_ptr->net_policy.name,apn_length+1);
          }
        }

        res_cb_ptr->net_mgr_handle = dss_bringup_call(&net_params,
                                                      &ps_errno);

        if (net_params.name)
        {
          DS_SYSTEM_HEAP_MEM_FREE(net_params.name);
        }

        if (DSS_ERROR == res_cb_ptr->net_mgr_handle)
        {
          LOG_MSG_ERROR_0 ("Network bringup interface failed.");
          res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_FAIL;
          res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_ERROR_NET_MGR;
          iterate = TRUE;
          break;
        }

        res_cb_ptr->state   = PS_DNSI_RESOLVER_STATE_NETWORK_WAIT;
        res_cb_ptr->event   = PS_DNSI_RESOLVER_EVENT_NONE;

        if(ps_errno == DS_EWOULDBLOCK)
        {
          /*-----------------------------------------------------------------
            Start the network wait timer.
          -----------------------------------------------------------------*/
          LOG_MSG_INFO2_0 ("Starting timer for net event.");
          if (DSS_ERROR == dss_dns_getconst (DSS_DNS_CONST_NET_TIMEOUT,
                                             &net_timeout_interval,
                                             sizeof (int32)))
          {
            LOG_MSG_ERROR_0 ("Cant get const for NET TIMEOUT");
            res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_FAIL;
            res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_ERROR_UNKNOWN;
            iterate = TRUE;
            break;
          }

          if (PS_TIMER_FAILURE ==
                ps_timer_start (res_cb_ptr->pstimer,
                                (int64) net_timeout_interval))

          {
            LOG_MSG_ERROR_1 ("Timer start failed for: %d", res_cb_ptr->pstimer);
            res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_FAIL;
            res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_ERROR_UNKNOWN;
            iterate = TRUE;
            break;
          }
        }
        else if (ps_errno == DSS_SUCCESS)
        {
          DS_SYSTEM_HEAP_MEM_ALLOC(net_callback_info,
                                   sizeof(dss_net_callback_info_s),
                                   dss_net_callback_info_s*);

          if (net_callback_info == NULL)
          {
            LOG_MSG_ERROR_0 ("Cant get memory for  for net_callback_info");
            res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_FAIL;
            res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_ERROR_UNKNOWN;
            iterate = TRUE;
            break;
          }
          net_callback_info->dss_nethandle = res_cb_ptr->net_mgr_handle;
          net_callback_info->dss_errno = DS_ENETISCONN;
          net_callback_info->net_cb_user_data = res_cb_ptr;

          ps_dnsi_resolver_net_event_notify(net_callback_info);

          DS_SYSTEM_HEAP_MEM_FREE(net_callback_info);

        }
        iterate = FALSE;

        break;

      case PS_DNSI_RESOLVER_STATE_NETWORK_WAIT:
        /*-------------------------------------------------------------------
          We should come here only when the following events occur.
          PS_DNSI_RESOLVER_EVENT_NET_UP         - Go to NETWORK_OPEN state.
          PS_DNSI_RESOLVER_EVENT_ERROR_NET_MGR  - Go to FAIL state.
          PS_DNSI_RESOLVER_TIMEOUT_EVENT        - Go to FAIL state.
        -------------------------------------------------------------------*/
        LOG_MSG_INFO2_2 ("Recvd resolver event %d in state %d",
                         res_cb_ptr->event, res_cb_ptr->state);
        switch (res_cb_ptr->event)
        {
          case PS_DNSI_RESOLVER_EVENT_NET_UP:
            res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_NETWORK_OPEN;
            res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_NONE;
            break;

          case PS_DNSI_RESOLVER_EVENT_ERROR_NET_MGR:
            res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_FAIL;
            res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_ERROR_NET_MGR;
            break;

          default:
            LOG_MSG_ERROR_2 ("Invalid event %d in net wait for resolver 0x%p",
                             res_cb_ptr->event, res_cb_ptr);
            res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_FAIL;
            res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_ERROR_UNKNOWN;
            break;
        }

        iterate = TRUE;
        break;

      case PS_DNSI_RESOLVER_STATE_NETWORK_OPEN:
        /*-------------------------------------------------------------------
          We should come here when event mask is RESOLVER_EVENT_NONE.
          Get the DNS server address.
          If necessary, create IO manager instance.
            If ERROR goto FAIL state.
          Call IO manager to send query.
            If IO manager returns EWOULDBLOCK, go to IO_WAIT state.
            If ERROR goto FAIL state.
        -------------------------------------------------------------------*/

        DS_ASSERT (PS_DNSI_RESOLVER_EVENT_NONE == res_cb_ptr->event);

        /*-------------------------------------------------------------------------
          register to PDN_ADDR_CHANGED_EV event
        -------------------------------------------------------------------------*/
        pdn_ev_cb.event_cb = ps_dnsi_resolveri_iface_ev_cback;
        pdn_ev_cb.user_data_ptr = res_cb_ptr;

        /*-------------------------------------------------------------------------
          register to IFACE_ADDR_CHANGED event. If iface_id was configured we
          don't need this, since in this case the application is responsible
          for network changes.
        -------------------------------------------------------------------------*/
        pdn_ev_cb.event = PDN_ADDR_CHANGED_EV;

        if (DSS_SUCCESS != ds_pdn_ioctl(DS_FWK_CLIENT_SOCKETS, 
                                          res_cb_ptr->net_mgr_handle,
                                          DS_PDN_IOCTL_REG_EVENT_CB,
                                          &pdn_ev_cb,
                                          &ps_errno))
          {
            LOG_MSG_ERROR_1 ("ps_dnsi_resolveri_run(): "
                             "Failed to register to iface_addr_changed event, errno = %d",
                             ps_errno);
            res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_FAIL;
            res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_ERROR_NET_MGR;
            iterate = TRUE;
            break;
          }

        /*-------------------------------------------------------------------
        Get the DNS server address. If ERROR try to get the other 
        DNS (primary/secondary). if get failed for both then go to fail state.
        -------------------------------------------------------------------*/

        api_cb_ptr = (ps_dnsi_api_cb_type*)res_cb_ptr->user_data_ptr;

        if (api_cb_ptr == NULL)
        {
          LOG_MSG_ERROR_0 ("api_cb_ptr is NULL");
          res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_FAIL;
          res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_ERROR_NET_MGR;
          iterate = TRUE;
          break;
        }

        
        max_get_server_attempts_in_try_iteration = 
          ps_dnsi_resolveri_calc_max_servers(api_cb_ptr->addr_family,
                                             res_cb_ptr->session_handle);
        /*Initialize retval to error so the logic below will cover also the case
        where there are no servers*/
        retval = DSS_ERROR;
        for ( get_server_attempts_in_try_iteration = 0;
              get_server_attempts_in_try_iteration < 
              max_get_server_attempts_in_try_iteration;
              get_server_attempts_in_try_iteration++ )
        {

          /*if server_index exceeds the max configured go again to the first */
          if (res_cb_ptr->server_index >= max_get_server_attempts_in_try_iteration)
          {
            res_cb_ptr->server_index = 0;
          }

          retval = 
            ps_dnsi_resolveri_get_server_addr( res_cb_ptr->server_index,
                                               (int16)res_cb_ptr->net_mgr_handle,
                                               res_cb_ptr->session_handle,
                                               &dns_serv_addr,
                                               &ps_errno );
          if ( DSS_SUCCESS == retval )
          {
            LOG_MSG_INFO1_1("Got DNS server index %d",
                            res_cb_ptr->server_index);
            break;/* Got the DNS addr. No need to check the other DNS addr 
                     so break the while*/
          }
          LOG_MSG_INFO1_1("Failed to get DNS server index %d",
                          res_cb_ptr->server_index);
          /* Try the next DNS address */
          res_cb_ptr->server_index++;
        }/* for ( get_server_attempts_in_try_iteration < 
                        max_get_server_attempts_in_try_iteration )*/
        
        if ( DSS_ERROR == retval )/* Failed to get DNS server */
        {
          LOG_MSG_ERROR_1("Cannot find DNS server,error=%d", ps_errno);
          res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_FAIL;
          res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_ERROR_IO_MGR;
          iterate        = TRUE;
          break;
        }
        /*-------------------------------------------------------------------
          If an IO manager instance already exists, then send query using
          that instance.
          Else create IO manager Instance. If ERROR goto FAIL state.
        -------------------------------------------------------------------*/
        if (PS_DNSI_IO_MGR_INVALID_HANDLE == res_cb_ptr->io_mgr_handle)
        {

          res_cb_ptr->io_mgr_handle =
            ps_dnsi_io_mgr_create_instance (dns_serv_addr.ps_ss_family,
                                            &res_cb_ptr->net_policy,
                                            ps_dnsi_resolver_io_event_notify,
                                            (void *) res_cb_ptr,
                                            &ps_errno);

          if (PS_DNSI_IO_MGR_INVALID_HANDLE == res_cb_ptr->io_mgr_handle)
          {
            LOG_MSG_ERROR_1 ("IO manager create instance error=%d", ps_errno);
            res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_FAIL;
            res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_ERROR_IO_MGR;
            iterate        = TRUE;
            break;
          }
        }

        /*-------------------------------------------------------------------
          Call IO manager to send query.
            If IO manager returns EWOULDBLOCK, go to IO_WAIT state.
            If ERROR goto FAIL state.
        -------------------------------------------------------------------*/
        
        if (res_cb_ptr->max_pending_servers_index > DSS_DNS_DEFAULT_MAX_QUERY_RETRIES)
        {
          LOG_MSG_ERROR_0 ("Internal error");
          res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_FAIL;
          res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_ERROR_SESSION_MGR;
          iterate = TRUE;
          break;
        }
        memscpy (&(res_cb_ptr->pending_servers[res_cb_ptr->max_pending_servers_index]),
          sizeof(dns_serv_addr), &dns_serv_addr, sizeof(dns_serv_addr));
        res_cb_ptr->max_pending_servers_index++;

        io_mgr_cb_ptr = ps_dnsi_io_mgri_is_handle_valid( res_cb_ptr->io_mgr_handle );
        if (NULL != io_mgr_cb_ptr)
        {
          io_mgr_cb_ptr->p_pending_servers = res_cb_ptr->pending_servers;
          io_mgr_cb_ptr->max_pending_servers_index = res_cb_ptr->max_pending_servers_index;
        }

        retval = ps_dnsi_io_mgr_send_query (res_cb_ptr->io_mgr_handle,
                                            res_cb_ptr->query_msg,
                                            (struct ps_sockaddr *)&dns_serv_addr,
                                            &ps_errno);
        if ((DSS_ERROR != retval || DS_EWOULDBLOCK != ps_errno) &&
             DSS_SUCCESS != retval)
        {
          LOG_MSG_ERROR_1 ("IO manager send query error %d", ps_errno);
          res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_FAIL;
          res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_ERROR_IO_MGR;
          iterate        = TRUE;
          break;
        }

        /*-----------------------------------------------------------------
          Start timer to wait for IO manager callback.
        -----------------------------------------------------------------*/
        retval = 
           dss_dns_get_config_params (
                           res_cb_ptr->session_handle,
                           DSS_DNS_CONFIG_PARAMS_LINEAR_BACKOFF_RETRY_INFO,
                           &query_retry_params,
                           sizeof (dss_dns_linear_backoff_retry_info_type),
                           &ps_errno);

        if (DSS_ERROR == retval)
        {
          LOG_MSG_ERROR_0 ("Get session info RETRY_MECHANISM_INFO failed");
          res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_FAIL;
          res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_ERROR_SESSION_MGR;
          iterate = TRUE;
          break;
        }
        /*
        Calculate the timeout value based on the query_retry_params
        */

        query_timeout = ( query_retry_params.backoff * ( int64 )res_cb_ptr->io_retries )
                          + query_retry_params.first_retry_time;
   
        LOG_MSG_INFO2_1 ("Starting timer for IO mgr event for %d ms.",
                         query_timeout);

        if (PS_TIMER_FAILURE ==
              ps_timer_start (res_cb_ptr->pstimer,
                              query_timeout))

        {
          LOG_MSG_ERROR_1 ("Timer start failed for: %d", res_cb_ptr->pstimer);
          res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_FAIL;
          res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_ERROR_UNKNOWN;
          iterate        = TRUE;
          break;
        }

        res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_IO_WAIT;
        res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_NONE;
        iterate        = FALSE;

        break;

      case PS_DNSI_RESOLVER_STATE_IO_WAIT:
        /*-------------------------------------------------------------------
          We should come here only when the following events occur.
          PS_DNSI_RESOLVER_IO_READ_EVENT    - Parse the response.
            If ERROR go to FAIL state.
            Else go to IO_COMPLETE state.
          PS_DNSI_RESOLVER_EVENT_ERROR_IO_MGR  - Go to FAIL state.
          PS_DNSI_RESOLVER_EVENT_ERROR_TIMEOUT - Go to FAIL state.
          PS_DNSI_RESOLVER_EVENT_ERROR_NET_MGR - Go to FAIl state.
        -------------------------------------------------------------------*/
        LOG_MSG_INFO2_2 ("Recvd resolver event %d in state %d",
                         res_cb_ptr->event, res_cb_ptr->state);
        switch (res_cb_ptr->event)
        {
          case PS_DNSI_RESOLVER_EVENT_IO_READ:
            retval = ps_dnsi_msg_mgr_parse_response (res_cb_ptr->query_type,
                                                     res_cb_ptr->query_class,
                                                     res_cb_ptr->query_data_ptr,
                                                     &(res_cb_ptr->cname_data_ptr),
                                                     res_cb_ptr->dns_response_ptr,
                                                     &(res_cb_ptr->result_q),
                                                     &ps_errno);
            /*---------------------------------------------------------
              Search whether the query_id matches any of the query_ids
                of the queries we sent earlier
            ---------------------------------------------------------*/
            {
              int i = 0;
              for (i = res_cb_ptr->max_query_index-1 ; i>=0 ; i--)
              {
                if (retval == (int16)(res_cb_ptr->query_id[i]) 
                  && (res_cb_ptr->query_id_valid[i])) // Match was found
                {
                  /* Remove the matching query_id from the list */
                  res_cb_ptr->query_id_valid[i] = FALSE;
                  break;
                }
              }
              /*-------------------------------------------------------------
              if we didn't send any query with query_id == retval --> error
              -------------------------------------------------------------*/
              if (-1 == i) 
              {
                LOG_MSG_ERROR_2 ("Parse error %d for resolver 0x%p",
                                 ps_errno, res_cb_ptr);
                res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_FAIL;
                res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_ERROR_MSG_MGR_PARSE_RESP;
                break;
              }
            }

            rr_node = q_check(&(res_cb_ptr->result_q));

            if(((res_cb_ptr->query_type == PS_DNSI_QUERY_TYPE_AAAA) || 
                (res_cb_ptr->query_type == PS_DNSI_QUERY_TYPE_A)) && 
                (rr_node == NULL))
            {
              LOG_MSG_INFO2_0("Still No Records");
              res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_ALIAS;
              res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_ERROR_MSG_MGR_PARSE_RESP;
              break;
            }
            res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_IO_COMPLETE;
            res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_NONE;
            break;

          case PS_DNSI_RESOLVER_EVENT_ERROR_IO_MGR:
          case PS_DNSI_RESOLVER_EVENT_ERROR_NET_MGR:
            res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_FAIL;
            break;

          default:
            LOG_MSG_ERROR_2 ("Invalid event %d in IO wait for resolver 0x%p",
                             res_cb_ptr->event, res_cb_ptr);
            res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_FAIL;
            res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_ERROR_UNKNOWN;
            break;
        }

        iterate = TRUE;
        break;

      case PS_DNSI_RESOLVER_STATE_IO_COMPLETE:
        /*-------------------------------------------------------------------
          We should come here only when the event mask is RESOLVER_EVENT_NONE.
          Check the session configuration whether to add results to cache.
          If so, add the results to the cache and goto SUCCESS state.
        -------------------------------------------------------------------*/
        DS_ASSERT (PS_DNSI_RESOLVER_EVENT_NONE == res_cb_ptr->event);
        retval = dss_dns_get_config_params(res_cb_ptr->session_handle,
                                            DSS_DNS_CONFIG_PARAMS_ADD_TO_CACHE,
                                            &add_to_cache,
                                            sizeof (boolean),
                                            &ps_errno);
        if (DSS_ERROR == retval)
        {
          LOG_MSG_ERROR_0 ("Get session info for add to cache failed");
          res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_FAIL;
          res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_ERROR_SESSION_MGR;
          break;
        }

        if (add_to_cache)
        {
          retval = ps_dns_cache_add_rr_q( res_cb_ptr->fwk_inst,
                                                res_cb_ptr->query_type,
                                                res_cb_ptr->query_class,
                                                res_cb_ptr->query_data_ptr,
                                                res_cb_ptr->cname_data_ptr,
                                                &(res_cb_ptr->result_q),
                                                (sint15*)&ps_errno );
          if (DSS_ERROR == retval)
          {
            LOG_MSG_ERROR_2 ("Cache add failed for res 0x%p err %d",
                             res_cb_ptr, ps_errno);
            /*
            Clear the errno since failure of adding to the cache should not
            be considered as query failure. 
            */
            ps_errno = DSS_SUCCESS;
          }
        }

        res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_SUCCESS;
        res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_NONE;
        iterate = TRUE;
        break;

      case PS_DNSI_RESOLVER_STATE_SUCCESS:
        /*-------------------------------------------------------------------
          We should come here when event mask is RESOLVER_EVENT_NONE.
          Signal success to the API.
          Set resolver state to free - can be used again can or be freed.
        -------------------------------------------------------------------*/
        DS_ASSERT (PS_DNSI_RESOLVER_EVENT_NONE == res_cb_ptr->event);
        (* res_cb_ptr->cback_f_ptr) (PS_DNSI_RES_NOTIFY_SUCCESS,
                                       res_cb_ptr->handle,
                                       &(res_cb_ptr->result_q),
                                       res_cb_ptr->user_data_ptr,
                                       ps_errno);
        iterate = FALSE;
        break;

      /* Do recursive query in case of Alias */
      case PS_DNSI_RESOLVER_STATE_ALIAS:
      case PS_DNSI_RESOLVER_STATE_FAIL:
        /*-------------------------------------------------------------------
          We should come here when event mask is an error condition.

          For PS_DNSI_RESOLVER_EVENT_ERROR_NET_MGR and
          PS_DNSI_RESOLVER_EVENT_ERROR_IO_MGR events, retry till max_retries
          are reached.

          For Parsing errors, we should try sending the query to another server.
          For EHOSTNOTFOUND errors there is no point in trying another server.

          In other cases, or if max retries have been reached, signal error
          to the application and free resolver control block.
        -------------------------------------------------------------------*/
        retry_flag = FALSE;
        switch (res_cb_ptr->event)
        {
          case  PS_DNSI_RESOLVER_EVENT_ERROR_IO_MGR:
          case  PS_DNSI_RESOLVER_EVENT_ERROR_MSG_MGR_PARSE_RESP:
            if (DS_EHOSTNOTFOUND == ps_errno)
            {
               /* No need in retry */
               break;
            }
            retval = 
              dss_dns_get_config_params (
                             res_cb_ptr->session_handle,
                             DSS_DNS_CONFIG_PARAMS_LINEAR_BACKOFF_RETRY_INFO,
                             &query_retry_params,
                             sizeof (dss_dns_linear_backoff_retry_info_type),
                             &ps_errno);

            if (DSS_ERROR == retval)
            {
              LOG_MSG_ERROR_0 ("Get session info RETRY_MECHANISM_INFO failed");
              res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_FAIL;
              res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_ERROR_SESSION_MGR;
              iterate = TRUE;
              break;
            }
            /*
            Check that number of retries does't exceed the session setting 
            query_retry_params.
            */
            if (query_retry_params.max_retries > res_cb_ptr->io_retries++)
            {
               int32 retval32;
              /*-------------------------------------------------------------
                Try sending query to another server
              -------------------------------------------------------------*/
              res_cb_ptr->server_index++;
              /* Release the memory of the dns_response_ptr.*/
              if( NULL != res_cb_ptr->dns_response_ptr )
              {
                dsm_free_packet( &(res_cb_ptr->dns_response_ptr) );
                res_cb_ptr->dns_response_ptr = NULL;
              }

              /* Regenerate the query */
              dsm_free_packet(&(res_cb_ptr->query_msg));
              res_cb_ptr->query_msg = NULL;

              /* Check if recursion is desired */
              retval = dss_dns_get_config_params(res_cb_ptr->session_handle,
                                                 DSS_DNS_CONFIG_PARAMS_RECURSION_DESIRED,
                                                 &is_recursion_desired,
                                                 sizeof (boolean),
                                                 &ps_errno);
              if (DSS_ERROR == retval)
              {
                 LOG_MSG_ERROR_0 ("Get session info for DSS_DNS_CONFIG_PARAMS_RECURSION_DESIRED failed");
                 res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_FAIL;
                 res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_ERROR_SESSION_MGR;
                 iterate = TRUE;
                 break;
              }

              retval32 = ps_dnsi_msg_mgr_form_query( res_cb_ptr->query_data_ptr,
                                                     res_cb_ptr->query_type,
                                                     res_cb_ptr->query_class,
                                                     is_recursion_desired,
                                                     &res_cb_ptr->query_msg,
                                                     &ps_errno );

              if( DSS_ERROR == retval32 )
              {
                 LOG_MSG_ERROR_1("Can't create query message, error=%d",
                                 ps_errno);
                 break;
              }
              res_cb_ptr->query_id[res_cb_ptr->max_query_index] = (uint32) retval32;
              res_cb_ptr->query_id_valid[res_cb_ptr->max_query_index] = TRUE;
              res_cb_ptr->max_query_index++;

              pdn_ev_cb.event_cb = ps_dnsi_resolveri_iface_ev_cback;
              pdn_ev_cb.user_data_ptr = res_cb_ptr;
              pdn_ev_cb.event = PDN_ADDR_CHANGED_EV;

              if (DSS_SUCCESS != ds_pdn_ioctl(DS_FWK_CLIENT_SOCKETS, 
                                              res_cb_ptr->net_mgr_handle,
                                              DS_PDN_IOCTL_DEREG_EVENT_CB,
                                              &pdn_ev_cb,
                                              &ps_errno))
              {
                LOG_MSG_ERROR_1 ("ps_dnsi_resolveri_run(): "
                                 "Failed to deregister to PDN_ADDR_CHANGED_EV"
                                 " event, errno = %d",
                                  ps_errno);
                res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_FAIL;
                res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_ERROR_UNKNOWN;
                iterate = TRUE;
                break;
              }
              res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_NETWORK_OPEN;
              res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_NONE;
              retry_flag = TRUE;
            }
            break;

          case  PS_DNSI_RESOLVER_EVENT_ERROR_NET_MGR:
            LOG_MSG_ERROR_1 ("Net Mgr error for resolver 0x%p", res_cb_ptr);

            if (DSS_ERROR == dss_dns_getconst (DSS_DNS_CONST_NET_RETRIES,
                                               &max_net_retries,
                                               sizeof (int32)))
            {
              LOG_MSG_ERROR_0 ("Can't get const for max net retries");
              res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_FAIL;
              res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_ERROR_UNKNOWN;
              iterate = TRUE;
              break;
            }

 
            io_mgr_cb_ptr = ps_dnsi_io_mgri_is_handle_valid( res_cb_ptr->io_mgr_handle );

            if( io_mgr_cb_ptr == NULL )
            {
              LOG_MSG_ERROR_0("io_mgr_cb_ptr is NULL");
              res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_FAIL;
              res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_ERROR_UNKNOWN;
              iterate = TRUE;
              break;
            }
 
            if( io_mgr_cb_ptr->sockfd == 0 )
            {
              LOG_MSG_ERROR_1("Invalid IO Mgr CB 0x%p", io_mgr_cb_ptr);
              res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_FAIL;
              res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_ERROR_UNKNOWN;
              iterate = TRUE;
              break;;
            }
            ps_dnsi_io_mgri_sock_close(io_mgr_cb_ptr);
			
            pdn_ev_cb.event_cb = ps_dnsi_resolveri_iface_ev_cback;
            pdn_ev_cb.user_data_ptr = res_cb_ptr;
            pdn_ev_cb.event = PDN_ADDR_CHANGED_EV;

            if (DSS_SUCCESS != ds_pdn_ioctl(DS_FWK_CLIENT_SOCKETS, 
                                            res_cb_ptr->net_mgr_handle,
                                            DS_PDN_IOCTL_DEREG_EVENT_CB,
                                            &pdn_ev_cb,
                                            &ps_errno))
            {
              LOG_MSG_ERROR_1 ("ps_dnsi_resolveri_run(): "
                               "Failed to deregister to PDN_ADDR_CHANGED_EV"
                               " event, errno = %d",
                                ps_errno);
              res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_FAIL;
              res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_ERROR_UNKNOWN;
              iterate = TRUE;
              break;
            }

            /*---------------------------------------------------------------
              Delete the current network manager.
            ---------------------------------------------------------------*/
            if (DSS_ERROR != res_cb_ptr->net_mgr_handle)
            {
              if (DSS_ERROR == dss_bringdown_call(res_cb_ptr->net_mgr_handle,
                                                  &ps_errno))
              {
                LOG_MSG_ERROR_0 ("DSS bringdown call failure");
                res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_FAIL;
                res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_ERROR_UNKNOWN;
                iterate = TRUE;
                break;
              }

              res_cb_ptr->net_mgr_handle = DSS_ERROR;
            }
            else
            {
              LOG_MSG_ERROR_0 ("Net mgr handle was already DSS_ERROR");
            }
            /*Clear the cache for the Fwk inst*/
            if (NULL != res_cb_ptr->fwk_inst) 
            {
              res_cb_ptr->fwk_inst= NULL;
            }

            if (do_not_retry_network)
            {
              break;
            }

            if (max_net_retries > res_cb_ptr->net_retries++)
            {
              /*-------------------------------------------------------------
                Start from the beginning again.
              -------------------------------------------------------------*/
              res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_START;
              res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_NONE;
              retry_flag = TRUE;
            }
            break;

          case  PS_DNSI_RESOLVER_EVENT_ERROR_MSG_MGR_FORM_QUERY:
            LOG_MSG_ERROR_1 ("Error forming the query for res 0x%p",
                             res_cb_ptr);
            break;

          case  PS_DNSI_RESOLVER_EVENT_ERROR_SESSION_MGR:
            LOG_MSG_ERROR_1 ("Session Mgr error for resolver 0x%p",
                             res_cb_ptr);
            break;

          case  PS_DNSI_RESOLVER_EVENT_ERROR_CACHE_MGR:
            LOG_MSG_ERROR_1 ("Cache Mgr error for resolver 0x%p", res_cb_ptr);
            break;

          case  PS_DNSI_RESOLVER_EVENT_ERROR_LOCAL_QUERY_FAILED:
             LOG_MSG_ERROR_1 ("Local query failed for resolver 0x%p",
                              res_cb_ptr);
             ps_errno = DS_EHOSTNOTFOUND;
             break;

          case  PS_DNSI_RESOLVER_EVENT_ERROR_UNKNOWN:
            LOG_MSG_ERROR_2 ("Resolver 0x%p error %d",
                             res_cb_ptr, res_cb_ptr->event);
            ps_errno = DS_EINVAL;
            break;

          default:
            LOG_MSG_ERROR_2 ("Unknown Res 0x%p err %d",
                             res_cb_ptr, res_cb_ptr->event);
            ps_errno = DS_EINVAL;
            break;
        }

        if (retry_flag)
        {
          iterate = TRUE;
          break;
        }

        /*-------------------------------------------------------------------
          Notify the caller of the error condition and free the resolver.
        -------------------------------------------------------------------*/
        if (DSS_SUCCESS == ps_errno)
        {
          ps_errno = DSS_ERROR;
        }
        (* res_cb_ptr->cback_f_ptr)(PS_DNSI_RES_NOTIFY_FAILURE,
                                    res_cb_ptr->handle,
                                    &(res_cb_ptr->result_q),
                                    res_cb_ptr->user_data_ptr,
                                    ps_errno);

        /*-------------------------------------------------------------------
          TODO Flush the cache here if it was a network error?
        -------------------------------------------------------------------*/

        iterate = FALSE;
        break;

      case PS_DNSI_RESOLVER_STATE_FREE:
        /*-------------------------------------------------------------------
          This state is reached when the resolver operations are complete and
          the resolver control block can be cleared or used again if desired.
          Theoretically we should never come here.
        -------------------------------------------------------------------*/
        DS_ASSERT (0);
        iterate = FALSE;
        break;

      default:
        DATA_ERR_FATAL ("Invalid resolver state");
        iterate = FALSE;
        break;
    } /* switch (resolver state) */
  } /* while (iterate == TRUE) */

} /* ps_dnsi_resolveri_run() */

/*===========================================================================
STATIC FUNCTION PS_DNSI_RESOLVERI_TIMEOUT_CMD_HANDLER

DESCRIPTION
  This function processes PS_DNS_RESOLVER_TIMEOUT_CMD.

PARAMETERS
  ps_cmd   - Must be PS_DNS_RESOLVER_TIMEOUT_CMD.
  res_data - Pointer to the resolver control block whose timer expired.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void ps_dnsi_resolveri_timeout_cmd_handler
(
  ps_cmd_enum_type              ps_cmd,
  void                        * res_data
)
{
  ps_dnsi_resolver_cb_type    * res_cb_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2_0 ("Processing PS_DNS_RESOLVER_TIMEOUT_CMD");

  if (PS_DNS_RESOLVER_TIMEOUT_CMD != ps_cmd)
  {
    LOG_MSG_ERROR_1("ps_dnsi_resolveri_timeout_cmd_handler(): wrong cmd: %d",
                    ps_cmd);
    return;
  }

  if (!ps_mem_is_valid(res_data, PS_MEM_DNSI_RESOLVER_CB_TYPE))
  {
    /* A valid scenario can cause this when the instance got 
    delete command before the timeout in such case this is not an error.
    Therefor, to determine if the below is a real error please check
    the log to see if PS_DNS_DELETE_SESSION_CMD was processed for this
    instance before the timeout.
    */
    LOG_MSG_ERROR_1("Invalid timeout callback user data 0x%p", res_data);
    return;
  }
  res_cb_ptr = (ps_dnsi_resolver_cb_type *) res_data;

  if (PS_DNSI_RESOLVER_STATE_FAIL != res_cb_ptr->state)
  {
    /* The resolver's state was changed between sending the command and
    processing it 
    */
    LOG_MSG_ERROR_1("Invalid state: %d", res_cb_ptr->state);
    return;
  }

  ps_dnsi_resolveri_run(res_cb_ptr);

} /* ps_dnsi_resolveri_timeout_cmd_handler() */

/*===========================================================================
STATIC FUNCTION PS_DNSI_RESOLVERI_TIMEOUT_CB

DESCRIPTION
  This function is invoked when the event wait timer in the resolver
  expires. This function posts PS_DNS_REOLVER_TIMEOUT_CMD to PS to
  handle the timeout event. Cant directly handle timeout here, as
  timeout callbacks are called under tasklocks.

PARAMETERS
  res_data - Pointer to the resolver control block whose timer expired.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void ps_dnsi_resolveri_timeout_cb
(
  void * res_data
)
{
  ps_dnsi_resolver_cb_type      * res_cb_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!ps_mem_is_valid(res_data, PS_MEM_DNSI_RESOLVER_CB_TYPE))
  {
    LOG_MSG_ERROR_1 ("Resolver timer expired, "
                     "Invalid timeout callback user data 0x%p", res_data);
    ASSERT (0);
    return;
  }
  res_cb_ptr = (ps_dnsi_resolver_cb_type *) res_data;
  /*-------------------------------------------------------------------------
    If the resolver is in FREE state (which means it is already freed due to
    IO or net mgr errors), then simply ignore the event.
  -------------------------------------------------------------------------*/
  if (PS_DNSI_RESOLVER_STATE_FAIL == res_cb_ptr->state)
  {
    LOG_MSG_INFO1_1 ("Resolver timer expired, "
                     "Timeout recvd for already free res 0x%p", res_cb_ptr);
    return;
  }

  /*-------------------------------------------------------------------------
    Stop the timer.
  -------------------------------------------------------------------------*/
  LOG_MSG_INFO2_1 ("Resolver timer expired, "
                   "Canceling timer for resolver timeout event 0x%p", res_cb_ptr);
  (void) ps_timer_cancel(res_cb_ptr->pstimer);

  switch (res_cb_ptr->state)
  {
    case PS_DNSI_RESOLVER_STATE_IO_WAIT:
      res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_FAIL;
      res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_ERROR_IO_MGR;
      break;

    case PS_DNSI_RESOLVER_STATE_NETWORK_WAIT:
      res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_FAIL;
      res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_ERROR_NET_MGR;
      break;

    default:
      LOG_MSG_ERROR_0 ("Invalid State during timeout event.");
      res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_FAIL;
      res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_ERROR_UNKNOWN;
      break;
  }

  ps_send_cmd(PS_DNS_RESOLVER_TIMEOUT_CMD, (void *)res_cb_ptr);
} /* ps_dnsi_resolveri_timeout_cb() */

/*===========================================================================
FUNCTION PS_DNSI_RESOLVER_NET_EVENT_NOTIFY

DESCRIPTION
  Notifies the resolver of events of interest from the network manager.
  This is a callback function that the resolver registers with the network
  manager.

PARAMETERS
  net_mgr_handle    - Handle to network manager on which the event occurred.
  iface_id          - If event is NET_UP, then the iface_id of the
                      interface brought up.
  net_mgr_event     - Network manager event.
  user_data_ptr     - Resolver user data (Pointer to resolver control block)

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void ps_dnsi_resolver_net_event_notify
(
  dss_net_callback_info_s *net_callback_info
)
{

  int32                            net_mgr_handle;
  ds_fwk_s_type                    *fwk_inst = NULL;
  dss_net_mgr_net_event_enum_type  net_mgr_event = DSS_NET_MGR_NET_EVENT_MIN;
  void                             * user_data_ptr = NULL;
  ps_dnsi_resolver_cb_type         * res_cb_ptr = NULL;
  int16                            local_errno;
  acb_type                         *acb_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (net_callback_info == NULL)
  {
    LOG_MSG_ERROR_0 ("net_callback_info is NULL");
    goto bail;
  }

  net_mgr_handle = net_callback_info->dss_nethandle;

  if (net_callback_info->dss_errno == DS_ENETISCONN)
  {
    net_mgr_event = DSS_NET_MGR_NET_EVENT_UP;
  }
  else if (net_callback_info->dss_errno == DS_ENETNONET)
  {
    net_mgr_event = DSS_NET_MGR_NET_EVENT_DOWN;
  }

  acb_ptr = dssocki_dss_nethandletoacb(net_mgr_handle);
  if ( acb_ptr == NULL)
  {
    DS_ERROR_LOG_0("ps_dnsi_resolver_net_event_notify: ACB pointer is NULL");
    goto bail;
  }

  fwk_inst = acb_ptr->fw_inst;

  LOG_MSG_INFO2_3 ("NET event %d from NET mgr %d for fwk inst0x%x",
                   net_mgr_event, net_mgr_handle, fwk_inst);

  if (net_mgr_event != DSS_NET_MGR_NET_EVENT_UP  &&
      net_mgr_event != DSS_NET_MGR_NET_EVENT_DOWN)
  {
    LOG_MSG_ERROR_1 ("Unhandled network event %d", net_mgr_event);
    goto bail;
  }

  user_data_ptr = net_callback_info->net_cb_user_data;
  if (NULL == user_data_ptr)
  {
    LOG_MSG_ERROR_1 ("NULL user data from network event %d", net_mgr_event);
    goto bail;
  }

  /*-------------------------------------------------------------------------
    If the resolver control block is invalid simply return. We want to
    ignore the events for already freed resolver, or events from a previous
    resolver instance.
  -------------------------------------------------------------------------*/
  res_cb_ptr = (ps_dnsi_resolver_cb_type *) user_data_ptr;
  if (res_cb_ptr != ps_dnsi_resolveri_is_handle_valid(res_cb_ptr->handle))
  {
    LOG_MSG_ERROR_1 ("Invalid Net callback user data 0x%p", user_data_ptr);
    return;
  }

  /*-------------------------------------------------------------------------
    If the resolver is not in NETWORK_WAIT state, ignore UP events.
  -------------------------------------------------------------------------*/
  if( net_mgr_event == DSS_NET_MGR_NET_EVENT_UP )
  {
    if( PS_DNSI_RESOLVER_STATE_NETWORK_WAIT != res_cb_ptr->state )
    {
      LOG_MSG_ERROR_3("Invalid NET event %d for resolver 0x%p in state %d",
                      net_mgr_event, res_cb_ptr, res_cb_ptr->state);
      return;
    }
  }

  /* After a network error, a resolver can change a net_mgr handle to retry 
     the process, but will still receive events to old handle. 
     Ignore such events. */
  if (res_cb_ptr->net_mgr_handle != net_mgr_handle)
  {
    LOG_MSG_INFO2_1 ("This callback's net mgr is %d, ignoring old net_mgr event",
                     res_cb_ptr->net_mgr_handle);
    return;
  }

  /* If the resolver is already in FAIL state, ignore the event */
  if ((net_mgr_event == DSS_NET_MGR_NET_EVENT_DOWN) &&
      (PS_DNSI_RESOLVER_STATE_FAIL == res_cb_ptr->state))
  {
     LOG_MSG_INFO2_0 ( "ps_dnsi_resolver_net_event_notify(): Ignoring the event since the resolver is already in FAIL state");
     return;
  }
  /*-------------------------------------------------------------------------
    Stop the timer.
  -------------------------------------------------------------------------*/
  if (ps_timer_is_running(res_cb_ptr->pstimer))
  {
    LOG_MSG_INFO2_0 ("Canceling timer for resolver timeout event");
    (void) ps_timer_cancel(res_cb_ptr->pstimer);
  }

  switch (net_mgr_event)
  {
    case DSS_NET_MGR_NET_EVENT_UP:
      /*---------------------------------------------------------------------
        Store the network manager handle and the iface id brought up
      -----------------------------------------()----------------------------*/
      res_cb_ptr->fwk_inst= fwk_inst;
      res_cb_ptr->net_mgr_handle = net_mgr_handle;
      res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_NET_UP;
      break;

    case DSS_NET_MGR_NET_EVENT_DOWN:
      res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_FAIL;
      res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_ERROR_NET_MGR;
      break;

    default:
      LOG_MSG_ERROR_1 ("Unhandled network manager event %d", net_mgr_event);
      res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_FAIL;
      res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_ERROR_UNKNOWN;
      break;
  }

  ps_dnsi_resolveri_run(res_cb_ptr);
  return;

bail:
  if(res_cb_ptr != NULL)
  {
    dss_bringdown_call(res_cb_ptr->net_mgr_handle ,&local_errno);
    /*lint -save -e774 Reason for suppressing error 774*/
    /* Currently, when we arrive to bail res_cb_ptr is always NULL. If it will
       change, remove the Lint error suppression */
    res_cb_ptr->net_mgr_handle = DSS_ERROR;
    /*Clear the cache for the iface_id*/
    if (NULL != res_cb_ptr->fwk_inst) 
    {
      res_cb_ptr->fwk_inst = NULL;
    }
  }
  /*lint -restore Restore lint error 774*/
  return;
} /* ps_dnsi_resolver_net_event_notify() */

/*===========================================================================
FUNCTION PS_DNSI_RESOLVER_IO_EVENT_NOTIFY

DESCRIPTION
  Notifies the resolver of events of interest from the IO manager.
  This is a callback function that the resolver registers with the IO
  manager.

PARAMETERS

  io_mgr_handle     - Handle to the IO manager control block on which
                      the event occurred.
  io_mgr_event      - Type of event.
  reply_msg_ptr     - Reply message dsm pointer if the event is
                      IO_MGR_READ.
  io_cb_user_data   - Resolver user data (points to resolver control block)
  errno             - Error code in case of error.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void ps_dnsi_resolver_io_event_notify
(
  ps_dnsi_io_mgr_handle_type      io_mgr_handle,
  ps_dnsi_io_mgr_event_enum_type  io_mgr_event,
  dsm_item_type                 * dns_response_ptr,
  void                          * user_data_ptr,
  int16                           ps_errno
)
{
  ps_dnsi_resolver_cb_type      * res_cb_ptr = NULL;
  int16                           local_errno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_3 ("IO event %d from IO mgr %d with DNS response 0x%p",
                   io_mgr_event, io_mgr_handle, dns_response_ptr);

  if (io_mgr_event != PS_DNSI_IO_MGR_READ_EVENT &&
      io_mgr_event != PS_DNSI_IO_MGR_ERROR_EVENT)
  {
    LOG_MSG_ERROR_1 ("Unhandled IO manager event %d", io_mgr_event);
    goto bail;
  }

  if (NULL == user_data_ptr)
  {
    LOG_MSG_ERROR_1 ("NULL user data from IO event %d", io_mgr_event);
    goto bail;
  }

  res_cb_ptr = (ps_dnsi_resolver_cb_type *) user_data_ptr;
  if (res_cb_ptr != ps_dnsi_resolveri_is_handle_valid(res_cb_ptr->handle))
  {
    LOG_MSG_ERROR_1 ("Invalid IO callback user data 0x%p", res_cb_ptr);
    goto bail;
  }

  if (io_mgr_handle != res_cb_ptr->io_mgr_handle)
  {
    /*-------------------------------------------------------------------------
      We should not get any event from another io_mgr, since the previous
      io_mgr should be deleted immadiately after creating the new one.
    -------------------------------------------------------------------------*/
    LOG_MSG_ERROR_2 ("Unexpected io_mgr handle: %d. Expected %d. Ignoring the event",
                     io_mgr_handle, res_cb_ptr->io_mgr_handle);
    return;
  }

  /*-------------------------------------------------------------------------
    If the resolver is not is IO_WAIT state, simply return. We want to
    ignore the events for already freed resolver, or events from a previous
    resolver instance.
  -------------------------------------------------------------------------*/
  if (PS_DNSI_RESOLVER_STATE_IO_WAIT != res_cb_ptr->state)
  {
    LOG_MSG_ERROR_3 ("Invalid IO event %d for resolver 0x%p in state %d",
                     io_mgr_event, res_cb_ptr, res_cb_ptr->state);
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Stop the timer.
  -------------------------------------------------------------------------*/
  LOG_MSG_INFO2_0 ("Canceling timer for resolver timeout event");
  (void) ps_timer_cancel(res_cb_ptr->pstimer);

  switch (io_mgr_event)
  {
    case PS_DNSI_IO_MGR_READ_EVENT:
      /*---------------------------------------------------------------------
        Dup the DSM pointer to the response message.
      ---------------------------------------------------------------------*/
      if (NULL == dns_response_ptr)
      {
        LOG_MSG_ERROR_0 ("DNS Response msg is NULL for read event");
        res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_FAIL;
        res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_ERROR_IO_MGR;
      }
      else
      {
        (void) PS_UTILS_DSM_DUP_PACKET(&(res_cb_ptr->dns_response_ptr),
                               dns_response_ptr,
                               0,
                               (uint16)dsm_length_packet(dns_response_ptr));
        res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_IO_READ;
      }
      break;

    case PS_DNSI_IO_MGR_ERROR_EVENT:
      res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_FAIL;
      res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_ERROR_IO_MGR;
      break;

    default:
      LOG_MSG_ERROR_1 ("Unhandled IO manager event %d", io_mgr_event);
      res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_FAIL;
      res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_ERROR_UNKNOWN;
      break;
  }

  ps_dnsi_resolveri_run(res_cb_ptr);
  return;

bail:
  if ( NULL != res_cb_ptr )
  {
    res_cb_ptr->io_mgr_handle = PS_DNSI_IO_MGR_INVALID_HANDLE;
  }
  (void) ps_dnsi_io_mgr_delete_instance (io_mgr_handle, &local_errno);
  return;
} /* ps_dnsi_resolver_io_event_notify() */

/*===========================================================================
STATIC FUNCTION PS_DNSI_RESOLVERI_IFACE_EVENT_CMD_HANDLER

DESCRIPTION
  This function processes PS_DNS_RESOLVER_IFACE_DOWN_CMD and
  PS_DNS_RESOLVER_IFACE_ADDR_CHANGED_CMD.

PARAMETERS
  ps_cmd   - Must be PS_DNS_RESOLVER_IFACE_DOWN_CMD or
             PS_DNS_RESOLVER_IFACE_ADDR_CHANGED_CMD.
  res_data - Pointer to the resolver control block which got the iface event.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void ps_dnsi_resolveri_iface_event_cmd_handler
(
  ps_cmd_enum_type              ps_cmd,
  void                        * res_data
)
{
  iface_event_cb_user_data         * cmd_data   = NULL;
  ps_dnsi_resolver_cb_type         * res_cb_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_3 ("ps_dnsi_resolveri_iface_event_cmd_handler()",
                 "Processing iface event command %d for resolver 0x%x",
                 ps_cmd, res_data);

  if (PS_DNS_RESOLVER_IFACE_DOWN_CMD != ps_cmd &&
      PS_DNS_RESOLVER_IFACE_ADDR_CHANGED_CMD != ps_cmd)
  {
    LOG_MSG_ERROR_2 ("ps_dnsi_resolveri_iface_event_cmd_handler()",
                   "Unexpected iface command %d", ps_cmd);
    goto bail;
  }

  if (NULL == res_data)
  {
    LOG_MSG_ERROR_1 ("ps_dnsi_resolveri_iface_event_cmd_handler()",
                   "NULL user data");
    goto bail;
  }

  cmd_data = (iface_event_cb_user_data *) res_data;

  /*-------------------------------------------------------------------------
    If the resolver control block is invalid simply return. We want to
    ignore the events for already freed resolver, or events from a previous
    resolver instance.
  -------------------------------------------------------------------------*/
  res_cb_ptr = cmd_data->res_cb_ptr;
  if (res_cb_ptr != ps_dnsi_resolveri_is_handle_valid(res_cb_ptr->handle))
  {
    LOG_MSG_ERROR_2 ("ps_dnsi_resolveri_iface_event_cmd_handler():",
                   "Invalid resolver 0x%p", res_data);
    goto bail;
  }

  if (res_cb_ptr->netlib_handle != cmd_data->netlib_handle)
  {
    LOG_MSG_ERROR_2("ps_dnsi_resolveri_iface_ev_cback(): "
                    "Invalid user data, dss_net_handle is %d, exp %d",
                    cmd_data->netlib_handle, res_cb_ptr->netlib_handle);
    goto bail;
  }

#if 0
  if (cmd_data->fwk_inst != res_cb_ptr->fwk_inst_for_registration)
  {
     LOG_MSG_ERROR_1 ("Wrong iface id: 0x%x", cmd_data->fwk_inst);
     goto bail;
  }
#endif

  switch (ps_cmd)
  {
    case PS_DNS_RESOLVER_IFACE_DOWN_CMD:
      /* If the resolver is already in FAIL state, ignore the event */
      if (PS_DNSI_RESOLVER_STATE_FAIL == res_cb_ptr->state)
      {
        LOG_MSG_INFO2_1 ( "ps_dnsi_resolveri_iface_event_cmd_handler(): Ignoring",
                        "the event since the resolver is already in FAIL state"
                        );
        goto bail;
      }
      res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_FAIL;
      res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_ERROR_NET_MGR;
      break;

    case PS_DNS_RESOLVER_IFACE_ADDR_CHANGED_CMD:
      /*-------------------------------------------------------------------------
         If the resolver is not is IO_WAIT state, simply return.
      -------------------------------------------------------------------------*/
      if (PS_DNSI_RESOLVER_STATE_IO_WAIT != res_cb_ptr->state)
      {
        LOG_MSG_ERROR_3 ("ps_dnsi_resolveri_iface_event_cmd_handler(): Invalid iface command %d for resolver 0x%p in state %d",
                         ps_cmd, res_cb_ptr, res_cb_ptr->state);
        goto bail;
      }

      /*-------------------------------------------------------------------------
         Stop the timer.
      -------------------------------------------------------------------------*/
      LOG_MSG_INFO2_1 ("ps_dnsi_resolveri_iface_event_cmd_handler():",
                     "Canceling timer for resolver timeout event");
      (void) ps_timer_cancel(res_cb_ptr->pstimer);

      res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_NETWORK_WAIT;
      res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_ERROR_NET_MGR;
      break;

    default:
      LOG_MSG_ERROR_2 ("ps_dnsi_resolveri_iface_event_cmd_handler():",
                     "Unhandled iface command %d", ps_cmd);
      res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_FAIL;
      res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_ERROR_UNKNOWN;
      break;
  }

  ps_dnsi_resolveri_run(res_cb_ptr);

bail:
  if (NULL != res_data)
  {
    PS_DNSI_MEM_FREE(res_data);
  }
  return;
} /* ps_dnsi_resolveri_iface_event_cmd_handler() */

/*===========================================================================
FUNCTION PS_DNSI_RESOLVERI_DELETE_INSTANCE_CMD_HANDLER

DESCRIPTION
  Command handler for deleting the resolver control block. Also closes the
  network manager and IO manager associated with the resolver control block.

PARAMETERS
  ps_cmd      - Must be PS_DNS_RESOLVER_DELETE_INSTANCE_CMD.
  res_data    - A pointer to the resolver control block.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Aborts all queries in progress.
===========================================================================*/
static void ps_dnsi_resolveri_delete_instance_cmd_handler
(
  ps_cmd_enum_type                   ps_cmd,
  void                             * res_data
)
{
  ps_dnsi_resolver_cb_type   * res_cb_ptr;
  int16                        ps_errno = DSS_ERROR;
  ps_dnsi_generic_rr_type    * rr_node_ptr;
  ds_pdn_ioctl_ev_cb_type     pdn_ev_cb;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_2 ("ps_dnsi_resolveri_delete_instance_cmd_handler(): "
                   "Processing delete instance command %d for resolver 0x%x",
                   ps_cmd, res_data);

  if (PS_DNS_RESOLVER_DELETE_INSTANCE_CMD != ps_cmd)
  {
    LOG_MSG_ERROR_1 ("ps_dnsi_resolveri_delete_instance_cmd_handler(): "
                     "Un-handled delete command %d", ps_cmd);
    return;
  }

  res_cb_ptr = (ps_dnsi_resolver_cb_type *) res_data;

  if (NULL == res_cb_ptr)
  {
    LOG_MSG_ERROR_0 ("ps_dnsi_resolveri_delete_instance_cmd_handler(): "
                     "NULL user data");
    return;
  }

  if (!ps_mem_is_valid(res_cb_ptr, PS_MEM_DNSI_RESOLVER_CB_TYPE))
  {
    LOG_MSG_ERROR_1 ("ps_dnsi_resolveri_delete_instance_cmd_handler(): "
                     "Invalid user data 0x%p", res_cb_ptr);
    return;
  }

  if (PS_DNSI_IO_MGR_INVALID_HANDLE != res_cb_ptr->io_mgr_handle)
  {
    if (DSS_ERROR ==
          ps_dnsi_io_mgr_delete_instance(res_cb_ptr->io_mgr_handle,
                                         &ps_errno))
    {
      LOG_MSG_ERROR_2 ("ps_dnsi_resolveri_delete_instance_cmd_handler(): "
                       "IO mgr close err %d in res 0x%p",
                       ps_errno, res_cb_ptr);
    }
    res_cb_ptr->io_mgr_handle = PS_DNSI_IO_MGR_INVALID_HANDLE;
  }


  /*-------------------------------------------------------------------------
      deregister PDN_ADDR_CHANGED_EV event
  -------------------------------------------------------------------------*/

  pdn_ev_cb.event_cb = ps_dnsi_resolveri_iface_ev_cback;
  pdn_ev_cb.user_data_ptr = res_cb_ptr;
  pdn_ev_cb.event = PDN_ADDR_CHANGED_EV;

  if (DSS_SUCCESS != ds_pdn_ioctl(DS_FWK_CLIENT_SOCKETS,
  	                              res_cb_ptr->net_mgr_handle, 
                                  DS_PDN_IOCTL_DEREG_EVENT_CB, 
                                  &pdn_ev_cb,
                                  &ps_errno))
    {
      LOG_MSG_ERROR_1 ("ps_dnsi_resolveri_delete_instance_cmd_handler(): "
                       "Failed to deregister iface_addr_changed event, errno = %d",
                       ps_errno);
    }


  LOG_MSG_INFO2_1 (" ps_dnsi_resolveri_delete_instance_cmd_handler(): "
                   "res_cb_ptr->net_mgr_handle %d",
                   res_cb_ptr->net_mgr_handle);

  if (DSS_ERROR != res_cb_ptr->net_mgr_handle)
  {
    /* net_mgr_handle may be DSS_ERROR here since open_netlib is called only
       after network comes UP*/
    if (DSS_ERROR ==
          dss_bringdown_call(res_cb_ptr->net_mgr_handle,
                                          &ps_errno))
    {
      LOG_MSG_ERROR_2 ("ps_dnsi_resolveri_delete_instance_cmd_handler(): "
                       "Net mgr close err %d in res 0x%p",
                       ps_errno, res_cb_ptr);
    }
    res_cb_ptr->net_mgr_handle = DSS_ERROR;
    /*Clear the cache for the iface_id*/
    if (NULL != res_cb_ptr->fwk_inst) 
    {
      res_cb_ptr->fwk_inst = NULL;
    }
  }

  

  PS_DNSI_MEM_FREE(res_cb_ptr->query_data_ptr);
  PS_DNSI_MEM_FREE(res_cb_ptr->cname_data_ptr);

  if (NULL != res_cb_ptr->query_msg)
  {
    dsm_free_packet(&(res_cb_ptr->query_msg));
    res_cb_ptr->query_msg = NULL;
  }

  if (NULL != res_cb_ptr->dns_response_ptr)
  {
    dsm_free_packet(&(res_cb_ptr->dns_response_ptr));
    res_cb_ptr->dns_response_ptr = NULL;
  }

  while (NULL != (rr_node_ptr = q_get(&(res_cb_ptr->result_q))))
  {
    PS_DNSI_MEM_FREE(rr_node_ptr->rdata);
    PS_MEM_FREE(rr_node_ptr);
  }
  q_destroy(&(res_cb_ptr->result_q));

  PS_TIMER_FREE_HANDLE(res_cb_ptr->pstimer);

  Q_DELETE(&res_instance_q, &(res_cb_ptr->link));

  LOG_MSG_INFO2_1 ("ps_dnsi_resolveri_delete_instance_cmd_handler(): "
                   "Freeing resolver %x", res_cb_ptr);

  res_cb_ptr->handle = PS_DNSI_RESOLVER_INVALID_HANDLE;
  memset(res_cb_ptr, 0, sizeof (ps_dnsi_resolver_cb_type));
  PS_MEM_FREE(res_cb_ptr);
} /* ps_dnsi_resolveri_delete_instance_cmd_handler() */

/*===========================================================================
FUNCTION  PS_DNSI_RESOLVERI_CREATE_INSTANCE

DESCRIPTION
  Starts an instance of the resolver. An instance can be used to send
  multiple queries.

PARAMETERS
  session_handle         - Session identifier
  errno                  - Error code in case of error.

DEPENDENCIES
  The created control block should be freed using
  ps_dnsi_resolver_delete_instance() function.
  The resolver control block would not be created if it cannot
  register with the session manager.

RETURN VALUE
  Returns a pointer to the resolver control block on success.
  On error returns NULL

  errno values
  ------------
  DS_EFAULT       - Invalid arguments
  DS_ENOMEM       - No memory to create the resolver control block.
  DS_EMAXQUERIES  - If max no of resolvers are already registered with
                    the session manager. //TODO change errno
  DS_ESYSTEM      - Other system errors.

SIDE EFFECTS
  None.
===========================================================================*/
static ps_dnsi_resolver_cb_type * ps_dnsi_resolveri_create_instance
(
  dss_dns_session_mgr_handle_type     session_handle,
  int16                             * ps_errno
)
{
  int16                               buf_index;
  ps_dnsi_resolver_cb_type          * res_cb_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_1 ("Creating resolver instance for session %d",
                   session_handle);

  /*-------------------------------------------------------------------------
    Validate function arguments
  -------------------------------------------------------------------------*/
  if (NULL == ps_errno)
  {
    LOG_MSG_ERROR_0 ("Resolver create instance: errno is NULL");
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Create a resolver control block.
  -------------------------------------------------------------------------*/
  res_cb_ptr =
    (ps_dnsi_resolver_cb_type *) ps_mem_get_buf(PS_MEM_DNSI_RESOLVER_CB_TYPE);

  if (NULL == res_cb_ptr)
  {
    *ps_errno = DS_ENOMEM;
    return NULL;
  }

  memset(res_cb_ptr, 0, sizeof (ps_dnsi_resolver_cb_type));
  buf_index = ps_mem_buf_to_index(res_cb_ptr);
  if (DSS_ERROR == buf_index)
  {
    LOG_MSG_ERROR_1 ("Can't get memory index for resolver 0x%p", res_cb_ptr);
    *ps_errno = DS_ESYSTEM;
    PS_MEM_FREE(res_cb_ptr);
    return NULL;
  }

  res_cb_ptr->handle = ps_handle_mgr_get_handle(RESOLVER_CLIENT, buf_index);
  if (PS_HANDLE_MGR_INVALID_HANDLE == res_cb_ptr->handle)
  {
    LOG_MSG_ERROR_1 ("Can't get handle for index %d", buf_index);
    *ps_errno = DS_ESYSTEM;
    PS_MEM_FREE(res_cb_ptr);
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Allocate timers to wait for network and IO events
  -------------------------------------------------------------------------*/
  res_cb_ptr->pstimer = ps_timer_alloc(ps_dnsi_resolveri_timeout_cb,
                                        (void *) res_cb_ptr);
  if ((uint32)PS_TIMER_INVALID_HANDLE == res_cb_ptr->pstimer)
  {
    LOG_MSG_ERROR_1 ("Failed to acquire timer 0x%p", res_cb_ptr->pstimer);
    PS_MEM_FREE(res_cb_ptr);
    *ps_errno = DS_ESYSTEM;
    return NULL;
  }

  res_cb_ptr->session_handle    = session_handle;
  res_cb_ptr->state             = PS_DNSI_RESOLVER_STATE_FREE;
  res_cb_ptr->event             = PS_DNSI_RESOLVER_EVENT_NONE;
  res_cb_ptr->server_index      = 0;
  res_cb_ptr->net_retries       = 0;
  res_cb_ptr->io_retries        = 0;
  res_cb_ptr->fwk_inst          = NULL;
  res_cb_ptr->fwk_inst_for_registration= NULL;
  res_cb_ptr->io_mgr_handle     = PS_DNSI_IO_MGR_INVALID_HANDLE;
  res_cb_ptr->net_mgr_handle    = DSS_ERROR;
  res_cb_ptr->netlib_handle     = DSS_ERROR;
  res_cb_ptr->query_msg         = NULL;
  res_cb_ptr->dns_response_ptr  = NULL;

  memset(res_cb_ptr->query_id, 0, sizeof(res_cb_ptr->query_id));
  memset(res_cb_ptr->query_id_valid, 0, sizeof(res_cb_ptr->query_id_valid));
  res_cb_ptr->max_query_index = 0;
  res_cb_ptr->max_pending_servers_index = 0;
  memset(res_cb_ptr->pending_servers, 0, sizeof(res_cb_ptr->pending_servers));

  memset(&res_cb_ptr->result_q, 0, sizeof(res_cb_ptr->result_q));
  (void) q_init(&res_cb_ptr->result_q);
  (void) q_link(res_cb_ptr, &res_cb_ptr->link);
  return res_cb_ptr;

} /* ps_dnsi_resolveri_create_instance() */

/*===========================================================================
FUNCTION PS_DNSI_RESOLVERI_CMD_HANDLER

DESCRIPTION
  Handles the PS_DNS_RESOLVE_CMD. Checks the cache to see if the results
  can be found in the cache. If not, it starts the resolver using
  ps_dnsi_resolveri_run().

PARAMETERS
  ps_cmd        - Command for which this function is invoked. Must be
                  PS_DNS_RESOLVE_CMD.
  res_data      - User data for the command. Points to the resolver
                  control block.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void ps_dnsi_resolveri_cmd_handler
(
  ps_cmd_enum_type              ps_cmd,
  void                        * res_data
)
{
  ps_dnsi_resolver_cb_type    * res_cb_ptr = NULL;
  ps_dnsi_api_cb_type         * api_cb_ptr = NULL;
  int16                         ps_errno = DSS_SUCCESS;
  int32                         retval = DSS_ERROR;
  boolean                       use_cache_results = TRUE;
  boolean                       is_recursion_desired = TRUE;
  ds_fwk_s_type                *fwk_inst =NULL;  
  ds_fwk_index_type             fwk_index = DS_FWK_INST_INVALID;
  ds_policy_info_type           policy_info = {0};
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (PS_DNS_RESOLVE_CMD != ps_cmd)
  {
    LOG_MSG_ERROR_1("ps_dnsi_resolveri_cmd_handler(): wrong cmd: %d", ps_cmd);
    return;
  }

  res_cb_ptr = (ps_dnsi_resolver_cb_type *) res_data;

  LOG_MSG_INFO2_0 ("Processing PS_DNS_RESOLVE_CMD");

  if (NULL == res_cb_ptr)
  {
    LOG_MSG_ERROR_0 ("PS command user data is NULL");
    return;
  }

  if (res_cb_ptr != ps_dnsi_resolveri_is_handle_valid(res_cb_ptr->handle))
  {
    LOG_MSG_ERROR_1 ("Resolver ptr 0x%p is invalid", res_cb_ptr);
    return;
  }
  /*-------------------------------------------------------------------------
    Check if the query is numerical. If so, no need to send the query to
    DNS server, simply notify client.
  -------------------------------------------------------------------------*/
  if (DSS_SUCCESS ==
        ps_dnsi_msg_mgr_resolve_numerical_query(res_cb_ptr->query_type,
                                                 res_cb_ptr->query_class,
                                                 res_cb_ptr->query_data_ptr,
                                                 &(res_cb_ptr->result_q),
                                                 &ps_errno))
  {
    LOG_MSG_INFO1_1 ("Query 0x%x is numerical; notifying client",
                     res_cb_ptr->query_data_ptr);

    res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_SUCCESS;
    res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_NONE;
    ps_dnsi_resolveri_run(res_cb_ptr);
    return;
  }

  /* ------------------------------------------------------------------------
     Check if the query is a "wildcard query": a NULL hostname with 
     AI_PASSIVE flag set. See function description for the details. 
  -------------------------------------------------------------------------*/
  if (DSS_SUCCESS ==
        ps_dnsi_msg_mgr_resolve_wildcard_query(res_cb_ptr->query_type,
                                                 res_cb_ptr->query_class,
                                                 res_cb_ptr->query_data_ptr,
                                                 &(res_cb_ptr->result_q),
                                                 &ps_errno))
  {
    LOG_MSG_INFO1_1 ("Query 0x%x is a wildcard query; notifying client",
                     res_cb_ptr->query_data_ptr);

    res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_SUCCESS;
    res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_NONE;
    ps_dnsi_resolveri_run(res_cb_ptr);
    return;
  }

  /*-------------------------------------------------------------------------
    Check whether Application wants to use cache results or not.
  -------------------------------------------------------------------------*/
  retval = dss_dns_get_config_params(res_cb_ptr->session_handle,
                                      DSS_DNS_CONFIG_PARAMS_USE_CACHE_RESULTS,
                                      &use_cache_results,
                                      sizeof (boolean),
                                      &ps_errno);
  if (DSS_ERROR == retval)
  {
    /* Error indicates session might be deleted. Just return from here */
    LOG_MSG_ERROR_0 ("Get session info for use cache results failed");
    return;
  }

  if (TRUE == use_cache_results)
  {
    if(res_cb_ptr->fwk_inst == NULL)
    {
      //Do a look up to find out the fwk inst
      api_cb_ptr = (ps_dnsi_api_cb_type*)res_cb_ptr->user_data_ptr;
      memset(&policy_info, 0, sizeof(ds_policy_info_type)); 
      policy_info.apn_type = DS_APN_TYPE_UNSPECIFIED;
      policy_info.family   = api_cb_ptr->addr_family;
      if(res_cb_ptr->net_policy.name != NULL)
      {
        policy_info.length  = strlen ((const char*)res_cb_ptr->net_policy.name);
        if (policy_info.length != 0)
        {
          strlcpy(policy_info.name, (const char*)res_cb_ptr->net_policy.name, policy_info.length+1);
        }
      }
      fwk_index = ds_fwk_get_fwk_inst_by_policy(DS_FWK_CLIENT_SOCKETS,policy_info);
      if(fwk_index != DS_FWK_INST_INVALID)
      {
        fwk_inst = ds_fwk_get_inst_ptr_from_index(fwk_index);
        res_cb_ptr->fwk_inst = fwk_inst;
      }
    }
    retval = ps_dns_cache_find_rr_q( res_cb_ptr->fwk_inst,
                                           res_cb_ptr->query_type,
                                           res_cb_ptr->query_class, 
                                           res_cb_ptr->query_data_ptr,
                                           &(res_cb_ptr->result_q),
                                           (sint15*)&ps_errno );

    if (DSS_SUCCESS == retval)
    {
      /*-------------------------------------------------------------------------
        Cache Hit/Negative cache hit. Call API callback signalling success
      -------------------------------------------------------------------------*/
      res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_SUCCESS;
      res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_NONE;
      ps_dnsi_resolveri_run(res_cb_ptr);
      return;
    }
    else if (ps_errno != DS_EHOSTNOTFOUND)
    {
      /*-------------------------------------------------------------------------
        Some error occurred during getting cache results.
        Fallback: bypass cache.
      -------------------------------------------------------------------------*/
      LOG_MSG_ERROR_1("An error occurred during cache results retrieval. Bypassing cache. (errno= %d)",
                      ps_errno);
    }

  } /* if (TRUE == use_cache_results) */

  api_cb_ptr = (ps_dnsi_api_cb_type*)(res_cb_ptr->user_data_ptr);
  if ((NULL == api_cb_ptr) ||
      (!ps_dnsi_api_is_handle_valid(api_cb_ptr->handle))) {
     res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_FAIL;
     res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_ERROR_UNKNOWN;
     ps_dnsi_resolveri_run( res_cb_ptr );
     return;
  }
  if ( api_cb_ptr->flags & (int)DSS_DNS_AI_FLAGS_LOCAL_QUERY ) {
     res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_FAIL;
     res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_ERROR_LOCAL_QUERY_FAILED;
     ps_dnsi_resolveri_run( res_cb_ptr );
     return;
  }

  /*
    Check if recursion is desired
  */
  /*-------------------------------------------------------------------------
    Check whether Application wants the DNS to do recursion.
  -------------------------------------------------------------------------*/
  retval = dss_dns_get_config_params(res_cb_ptr->session_handle,
                                      DSS_DNS_CONFIG_PARAMS_RECURSION_DESIRED,
                                      &is_recursion_desired,
                                      sizeof (boolean),
                                      &ps_errno);
  if (DSS_ERROR == retval)
  {
    /* Session is deleted, no need to run resolver SM. Just return  */
    LOG_MSG_ERROR_0 ("Get session info for DSS_DNS_CONFIG_PARAMS_RECURSION_DESIRED failed");
    return;
  }

  /*-------------------------------------------------------------------------
    Cache miss or Don't use cache results flag is set.
    Form the query message to send and run the resolver state machine.
  -------------------------------------------------------------------------*/

  retval = ps_dnsi_msg_mgr_form_query(res_cb_ptr->query_data_ptr,
                                       res_cb_ptr->query_type,
                                       res_cb_ptr->query_class,
                                       is_recursion_desired,
                                       &res_cb_ptr->query_msg,
                                       &ps_errno);

  if (DSS_ERROR == retval)
  {
    LOG_MSG_ERROR_1 ("Cannot create query message, error=%d", ps_errno);
    res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_FAIL;
    res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_ERROR_MSG_MGR_FORM_QUERY;
    ps_dnsi_resolveri_run(res_cb_ptr);
    return;
  }

  res_cb_ptr->query_id[res_cb_ptr->max_query_index] = (uint32) retval;
  res_cb_ptr->query_id_valid[res_cb_ptr->max_query_index] = TRUE;
  res_cb_ptr->max_query_index++;

  res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_START;
  res_cb_ptr->event = PS_DNSI_RESOLVER_EVENT_NONE;
  ps_dnsi_resolveri_run(res_cb_ptr);

} /* ps_dnsi_resolveri_cmd_handler() */

/*===========================================================================

                         EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION  PS_DNSI_RESOLVER_INIT

DESCRIPTION
  Initializes the Resolver module during powerup.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
void ps_dnsi_resolver_init
(
  void
)
{
  #define dns_cofig_file_path "/nv/item_files/data/interfaces/dns_reserved_bits_config"

  int fd = 0;
  int fs_read = 0;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Initialize Pool
  -------------------------------------------------------------------------*/
  if (PS_MEM_POOL_INIT_OPT(PS_MEM_DNSI_RESOLVER_CB_TYPE,
                           ps_dnsi_resolver_buf_mem,
                           PS_DNSI_RESOLVER_BUF_SIZE,
                           PS_DNSI_RESOLVER_BUF_NUM,
                           PS_DNSI_RESOLVER_BUF_HIGH_WM,
                           PS_DNSI_RESOLVER_BUF_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) ps_dnsi_resolver_buf_hdr,
                           (int *) ps_dnsi_resolver_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                         ) == -1)
  {
    LOG_MSG_ERROR_0("Can't init the module");
  }

  /*------------------------------------------------------------------------
    Set command handler for Resolver module.
  -------------------------------------------------------------------------*/
  (void) ps_set_cmd_handler(PS_DNS_RESOLVE_CMD,
                            ps_dnsi_resolveri_cmd_handler);

  (void) ps_set_cmd_handler(PS_DNS_RESOLVER_TIMEOUT_CMD,
                            ps_dnsi_resolveri_timeout_cmd_handler);

  (void) ps_set_cmd_handler(PS_DNS_RESOLVER_IFACE_DOWN_CMD,
                            ps_dnsi_resolveri_iface_event_cmd_handler);

  (void) ps_set_cmd_handler(PS_DNS_RESOLVER_IFACE_ADDR_CHANGED_CMD,
                            ps_dnsi_resolveri_iface_event_cmd_handler);

  (void) ps_set_cmd_handler(PS_DNS_RESOLVER_DELETE_INSTANCE_CMD,
                            ps_dnsi_resolveri_delete_instance_cmd_handler);

  /*------------------------------------------------------------------------
    Initialize the cache managers and IO manager.
    Network manager is initialized during startup.
  -------------------------------------------------------------------------*/
  ps_dns_cache_init();
  ps_dnsi_io_mgr_init();
  ps_dnsi_msg_mgr_init();
  /*------------------------------------------------------------------------
    Initialize the resolver Q. This queue contains pointers to all
    active resolver instances.
  -------------------------------------------------------------------------*/
  (void) q_init(&res_instance_q);

  fd = efs_open(dns_cofig_file_path, O_RDONLY | O_TRUNC, S_IRUSR | S_IWUSR ); /* Opening the efs file */;
  if (fd < 0)
  {
    LOG_MSG_ERROR_1("efs_open dns config file: errcode %d", efs_errno);
    
  }
  else
  {
    fs_read = efs_read(fd,&dns_reserved_bits_config, sizeof(dns_reserved_bits_config));
    if(fs_read <= 0)
    {
      LOG_MSG_ERROR_0("efs_read for dns config file failed");  
    }
  }
  efs_close(fd);

} /* ps_dnsi_resolver_init() */

/*===========================================================================
FUNCTION  PS_DNSI_RESOLVER_START_QUERY

DESCRIPTION
  Starts a DNS query corresponding through a resolver instance.

PARAMETERS
  res_handle             - Resolver instance identifier.
  query_type             - Type of query
  query_class            - Class of query
  query_data_ptr         - Query data pointer
  cback_f_ptr            - Pointer to the callback function.
  user_data_ptr          - Callback function user data.
  errno                  - Error code in case of error.

DEPENDENCIES
  The caller must have created a resolver instance.
  Resolver state must be PS_DNSI_RESOLVER_STATE_FREE.

RETURN VALUE
  Always returns DSS_ERROR
    If errno is DS_EWOULDBLOCK, it indicates that the results would be
      returned through the registered callback.
    Else there is an irrecoverable error and resolver control block
      should be freed using ps_dnsi_resolver_delete_instance().

  errno values
  ------------
  DS_EWOULDBLOCK  - The call would block.
  DS_EFAULT       - Invalid arguments.
  DS_ESYSTEM      - Other system error.

SIDE EFFECTS
  None.
===========================================================================*/
int16 ps_dnsi_resolver_start_query
(
  ps_dnsi_resolver_handle_type      res_handle,
  ps_dnsi_query_type_enum_type      query_type,
  ps_dnsi_query_class_enum_type     query_class,
  char                            * query_data_ptr,
  ps_dnsi_resolver_notify_f_type    cback_f_ptr,
  void                            * user_data_ptr,
  int16                           * ps_errno
)
{
  ps_dnsi_resolver_cb_type        * res_cb_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_3 ("Starting query 0x%p using resolver %d; cback 0x%p",
                   query_data_ptr, res_handle, cback_f_ptr);

  /*-------------------------------------------------------------------------
    Validate function arguments
  -------------------------------------------------------------------------*/
  if (NULL == ps_errno)
  {
    LOG_MSG_ERROR_0 ("ps_dnsi_resolve: errno is NULL");
    return DSS_ERROR;
  }

  if (NULL == query_data_ptr  ||
      PS_DNSI_QUERY_TYPE_MAX  <= query_type  ||
      PS_DNSI_QUERY_CLASS_MAX <= query_class)
  {
    LOG_MSG_ERROR_3 ("Invalid args query 0x%p, class %d, type %d",
                     query_data_ptr, (int)query_class, (int)query_type);
    *ps_errno = DS_EFAULT;
    return DSS_ERROR;
  }

  res_cb_ptr = ps_dnsi_resolveri_is_handle_valid(res_handle);
  if (NULL == res_cb_ptr)
  {
    LOG_MSG_ERROR_2 ("Inv resolver cb 0x%p from handle %d",
                     res_cb_ptr, res_handle);
    *ps_errno = DS_EFAULT;
    return DSS_ERROR;
  }

  if (PS_DNSI_RESOLVER_STATE_FREE != res_cb_ptr->state)
  {
    LOG_MSG_ERROR_2 ("Invalid resolver 0x%p state %d",
                     res_cb_ptr, res_cb_ptr->state);
    *ps_errno = DS_EFAULT;
    return DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    If everything looks fine, start the resolver.
  -------------------------------------------------------------------------*/
  res_cb_ptr->state             = PS_DNSI_RESOLVER_STATE_START;
  res_cb_ptr->query_type        = query_type;
  res_cb_ptr->query_class       = query_class;
  res_cb_ptr->cback_f_ptr       = cback_f_ptr;
  res_cb_ptr->user_data_ptr     = user_data_ptr;

  memset(res_cb_ptr->query_id, 0, sizeof(res_cb_ptr->query_id));
  memset(res_cb_ptr->query_id_valid, 0, sizeof(res_cb_ptr->query_id_valid));
  res_cb_ptr->max_query_index = 0;
  res_cb_ptr->max_pending_servers_index = 0;
  memset(res_cb_ptr->pending_servers, 0, sizeof(res_cb_ptr->pending_servers));
  res_cb_ptr->cname_data_ptr = NULL;

  res_cb_ptr->query_data_ptr = ps_dnsi_mem_alloc(DSS_DNS_MAX_DOMAIN_NAME_LEN);
  if (NULL == res_cb_ptr->query_data_ptr)
  {
    LOG_MSG_ERROR_0 ("Memory allocation for query_data failed");
    *ps_errno = DS_ENOMEM;
    return DSS_ERROR;
  }
  (void)strlcpy(res_cb_ptr->query_data_ptr,
                      query_data_ptr,
                      DSS_DNS_MAX_DOMAIN_NAME_LEN);

  /*-------------------------------------------------------------------------
    Post PS_DNS_RESOLVE_CMD to PS
  -------------------------------------------------------------------------*/
  ps_send_cmd(PS_DNS_RESOLVE_CMD, (void *) res_cb_ptr);

  /*-------------------------------------------------------------------------
    Return the resolver control block identifier.
  -------------------------------------------------------------------------*/
  *ps_errno = DS_EWOULDBLOCK;
  return DSS_ERROR;
} /* ps_dnsi_resolver_start_query() */

/*===========================================================================
FUNCTION  PS_DNSI_RESOLVER_GET_INSTANCE()

DESCRIPTION
  Gets a free resolver instance in the current session if any.
  Otherwise tries to create a new resolver instance and returns its handle.

PARAMETERS
  session_handle  - Session control block handle.
  ps_errno           - Error code in case of error.

RETURN VALUE
  Handle to the resolver control block on success.
  PS_DNSI_RESOLVER_INVALID_HANDLE on error.

  errno values
  ------------
  DS_ENOMEM if the resolver instance cannot be created.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
ps_dnsi_resolver_handle_type ps_dnsi_resolver_get_instance
(
  dss_dns_session_mgr_handle_type     session_handle,
  int16                             * ps_errno
)
{
  ps_dnsi_resolver_cb_type      * res_cb_ptr;
  int16                           count;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get a free resolver regiestered with the session, if any.
    Traverse through the resolver Q, return the resolver handle of the
    control block that matches the session handle passed in.
  -------------------------------------------------------------------------*/
  res_cb_ptr = q_check(&res_instance_q);
  for (count = q_cnt(&res_instance_q); 
       count > 0 && res_cb_ptr != NULL; 
       count--)
  {
    if (session_handle == res_cb_ptr->session_handle &&
        PS_DNSI_RESOLVER_STATE_FREE == res_cb_ptr->state)
    {
      if (NULL != res_cb_ptr->query_msg ||
          NULL != res_cb_ptr->dns_response_ptr)
      {
        *ps_errno = DS_EINVAL;
        return PS_DNSI_RESOLVER_INVALID_HANDLE;
      }
      break;
    }
    res_cb_ptr = q_next(&res_instance_q, &res_cb_ptr->link);
  }

  /*-------------------------------------------------------------------------
    If no resolver is found, then create a new one and insert it into the 
    Resolver Q.
  -------------------------------------------------------------------------*/
  if (NULL == res_cb_ptr)
  {
    res_cb_ptr = ps_dnsi_resolveri_create_instance(session_handle,
                                                    ps_errno);
    if (NULL == res_cb_ptr)
    {
      LOG_MSG_ERROR_0 ("Cannot create resolver control block");
      return PS_DNSI_RESOLVER_INVALID_HANDLE;
    }
    
    q_put(&res_instance_q, &(res_cb_ptr->link));
  }

  LOG_MSG_INFO2_2 ("Using resolver inst 0x%p with handle %d",
                   res_cb_ptr, res_cb_ptr->handle);

  res_cb_ptr->state = PS_DNSI_RESOLVER_STATE_FREE;

  return res_cb_ptr->handle;
} /* ps_dnsi_resolver_get_instance() */

/*===========================================================================
FUNCTION PS_DNSI_RESOLVER_DELETE_INSTANCE

DESCRIPTION
  Marks the resolver as deleted, and posts a delete command to PS
PARAMETERS
  res_handle  - Handle to the resolver control block.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ps_dnsi_resolver_delete_instance
(
  ps_dnsi_resolver_handle_type   res_handle
)
{
  ps_dnsi_resolver_cb_type   * res_cb_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_1 ("Freeing resolver instance %d", res_handle);

  res_cb_ptr = ps_dnsi_resolveri_is_handle_valid(res_handle);
  if (NULL == res_cb_ptr)
  {
    LOG_MSG_ERROR_1 ("Resolver handle %d invalid", res_handle);
    return;
  }

  if (!ps_mem_is_valid(res_cb_ptr, PS_MEM_DNSI_RESOLVER_CB_TYPE))
  {
    LOG_MSG_INFO1_1 ("Invalid Resolver cb 0x%p being deleted", res_cb_ptr);
    return;
  }

  LOG_MSG_INFO1_2 ("Resolver 0x%p is being deleted in state %d",
                   res_cb_ptr, res_cb_ptr->state);

  ps_send_cmd(PS_DNS_RESOLVER_DELETE_INSTANCE_CMD, res_cb_ptr);

} /* ps_dnsi_resolver_delete_instance() */

/*===========================================================================
FUNCTION PS_DNSI_RESOLVER_DELETE_ALL_INSTANCES

DESCRIPTION
  Deletes all the resolver instances associated with a certain session.

PARAMETERS
  session_handle    - Session identifier.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Aborts all queries in progress.
===========================================================================*/
void ps_dnsi_resolver_delete_all_instances
(
  dss_dns_session_mgr_handle_type     session_handle
)
{
  ps_dnsi_resolver_cb_type      * res_cb_ptr = NULL;
  ps_dnsi_resolver_cb_type      * next_ptr = NULL;
  int16                           count = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_1 ("Deleting all resolver instances for sess %d",
                   session_handle);

  res_cb_ptr = (ps_dnsi_resolver_cb_type *) q_check(&res_instance_q);

  for (count = q_cnt(&res_instance_q); 
       count > 0 && NULL != res_cb_ptr; 
       count--)
  {
    if (NULL == ps_dnsi_resolveri_is_handle_valid(res_cb_ptr->handle))
    {
      LOG_MSG_ERROR_1 ("Resolver %d is invalid", res_cb_ptr->handle);
      break;
    }
    if (session_handle == res_cb_ptr->session_handle)
    {
      next_ptr = q_next(&res_instance_q, &(res_cb_ptr->link));
      ps_dnsi_resolver_delete_instance(res_cb_ptr->handle);
      res_cb_ptr = next_ptr;
    }
    else
    {
      res_cb_ptr = q_next(&res_instance_q, &(res_cb_ptr->link));
    }
  }
} /* ps_dnsi_resolver_delete_all_instances() */
#endif