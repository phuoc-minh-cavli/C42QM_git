
/*===========================================================================

        D A T A  S E R V I C E S  S O C K E T  I N T E R N A L   F I L E

DESCRIPTION

 The Data Services sockets internal file. This contains all the functions
 used by internal applications.

Copyright (c) 2018 by Qualcomm Technologies, Incorporated. All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
==============================================================================
08/08/18        sr              Initial Version
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "ds_headers.h"
#include "target.h"

#include "dssocki.h"
#include "dss.h"
#include "ps_utils.h"
#include "ps_stat_sock.h"
#include "ps_ip6_addr.h"
#include "ds_ipfltr.h"

/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================
FUNCTION DSS_WRITE_DSM_CHAIN()

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
sint15 dss_write_dsm_chain
(
  sint15 sockfd,                                      /* socket descriptor */
  dsm_item_type **item_ptr,          /* DSM item chain containing the data */
  sint15 *dss_errno                               /* error condition value */
)
{
  struct dss_msghdr     msg;  /* Header containing data and dest addr      */
  struct ps_iovec          iov;  /* encapsulated data array                   */
  int                   flags = 0 ; /* local flags field for dss_sendmsg   */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG1(MSG_LEGACY_HIGH, "dss_write_dsm_chain: fd=%d", sockfd);

  iov.ps_iov_base = NULL;
  iov.ps_iov_len  = 0;

  msg.msg_name       = NULL;
  msg.msg_namelen    = 0;
  msg.msg_iov        = &iov;
  msg.msg_iovlen     = 1;
  msg.msg_control    = NULL;
  msg.msg_controllen = 0;
  msg.msg_flags      = 0;

  return dssocki_sendmsg(sockfd, &msg, item_ptr, flags, dss_errno);

} /* dss_write_dsm_chain() */


/*===========================================================================
FUNCTION DSS_READ_DSM_CHAIN()

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
sint15 dss_read_dsm_chain
(
  sint15 sockfd,                                      /* socket descriptor */
  dsm_item_type  **item_ptr,          /* ptr to item chain containing data */
  sint15 *dss_errno                               /* error condition value */
)
{
  struct scb_s *scb_ptr; /* ptr to socket control block allocated in       */
  sint15 result = 0;                        /* error checking return value */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    DS_MSG1(MSG_LEGACY_HIGH, "dss_read_dsm_chain: fd=%d", sockfd);
  /*-------------------------------------------------------------------------
    Assert that a valid ptr to a dss_errno variable was specified - Prevents
    dereferencing of NULL ptrs.
  -------------------------------------------------------------------------*/
  if( dss_errno == NULL )
  {
    //ASSERT(0);
    return (DSS_ERROR);
  }
  *dss_errno = DSS_SUCCESS;

  /*-------------------------------------------------------------------------
    Convert socket descriptor to SCB
   ------------------------------------------------------------------------*/
  if ( (scb_ptr=dssocki_sockfdtoscb(sockfd)) == NULL)
  {
    *dss_errno = DS_EBADF;
    return (DSS_ERROR);
  }

 /*-------------------------------------------------------------------------
    Only support TCP
  -------------------------------------------------------------------------*/
  if (scb_ptr->protocol != (byte)PS_IPPROTO_TCP)
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
    if ( *dss_errno == DS_EEOF)
    {
      /*---------------------------------------------------------------------
        Check if there is data available. Return 0, if there is no data
        available (EOF indication). Otherwise, call the protocol dependent
        read function.
      ---------------------------------------------------------------------*/
      if( scb_ptr->data_available == FALSE)
      {
        return (DSS_SUCCESS);
      }
    }
    else
    {
      return (result);
    }
  }

  /*-------------------------------------------------------------------------
    Make sure that the passed item_ptr is valid
  -------------------------------------------------------------------------*/
  if ( item_ptr == NULL )
  {
    *dss_errno = DS_EFAULT;
    return DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    Call protocol specific function to read bytes into a array.
  -------------------------------------------------------------------------*/
  result = scb_ptr->fcn_ptr_table->read_dsm_chain(scb_ptr, item_ptr, NULL,
                                                  NULL,dss_errno);

  /*-------------------------------------------------------------------------
    Read may return EWOULDBLOCK if the network is down.  Make sure ENETDOWN
    is returned in the case when the network is down.
  -------------------------------------------------------------------------*/
  if ((result == DSS_ERROR)      &&
      (*dss_errno == DS_EWOULDBLOCK) &&
      !(scb_ptr->cached_info.ip.routing_cache))
  {
    *dss_errno = DS_ENETDOWN;
    result = DSS_ERROR;
  }

  if (result != DSS_ERROR)
  {
    SOCK_INC_STATS(bytes_rx, result);
  }
  return result;

} /* dss_read_dsm_chain() */


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
  struct ps_sockaddr *fromaddr,                        /* destination address */
  uint16 *addrlen,                                       /* address length */
  sint15 *dss_errno                               /* error condition value */
)
{
  struct dss_msghdr     msg; /* Header containing data and dest address    */
  struct ps_iovec          iov; /* Data encapsulation array                   */
  int                   bytes_read; /* Bytes read                          */
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DS_MSG2(MSG_LEGACY_HIGH, "dss_recvfrom_dsm_chain: fd=%d, flags=0x%x",
          sockfd, flags);

  iov.ps_iov_base = NULL;
  iov.ps_iov_len = 0;

  msg.msg_name       = fromaddr;
  msg.msg_namelen    = (addrlen != NULL) ? *addrlen : 0;
  msg.msg_iov        = &iov;
  msg.msg_iovlen     = 1;
  msg.msg_control    = NULL;
  msg.msg_controllen = 0;
  msg.msg_flags      = 0;

  bytes_read = dssocki_recvmsg(sockfd, &msg, item_ptr,
                               (int)flags, dss_errno);

  if(addrlen != NULL)
  {
    *addrlen = msg.msg_namelen;
  }

  return (int16)bytes_read;

} /* dss_recvfrom_dsm_chain() */

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
  struct ps_sockaddr *toaddr,      /* destination address                     */
  uint16 addrlen,               /* address length                          */
  sint15 *dss_errno                               /* error condition value */
)
{
  struct dss_msghdr     msg;   /* Header containing data and dest addr     */
  struct ps_iovec          iov;   /* encapsulated data array                  */

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DS_MSG2(MSG_LEGACY_HIGH,"dss_sendto_dsm_chain: fd=%d, flags=0x%x",
          sockfd, flags);
  iov.ps_iov_base = NULL;
  iov.ps_iov_len  = 0;

  msg.msg_name       = toaddr;
  msg.msg_namelen    = addrlen;
  msg.msg_iov        = &iov;
  msg.msg_iovlen     = 1;
  msg.msg_control    = NULL;
  msg.msg_controllen = 0;
  msg.msg_flags      = 0;

  return dssocki_sendmsg(sockfd, &msg, item_ptr, (int)flags, dss_errno);

} /* dss_sendto_dsm_chain() */



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
)
{
  struct scb_s     * scb_ptr;     /* ptr to socket control block structure */
  ip_filter_type   * ipfltr_ptr;
  struct socket      localname;
  int                loop_var;
  uint8              next_hdr_prot = (uint8)PS_NO_NEXT_HDR;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == dss_errno)
  {
    DS_MSG0(MSG_LEGACY_ERROR, "dss_reg_ip_filter: dss_errno is NULL");
    return DSS_ERROR;
  }

  *dss_errno = DSS_SUCCESS;

  if ((NULL == filters)||(0 == num_filters)||
       (NULL == filter_handle))
  {
    *dss_errno = DS_EFAULT;
    DS_MSG2(MSG_LEGACY_ERROR,"dss_reg_ip_filter: Bad arg - filters 0x%p, "
            "num_filters %d",filters, num_filters);
    return DSS_ERROR;
  }

  if (NULL == (scb_ptr = dssocki_sockfdtoscb(sockfd)))
  {
    *dss_errno = DS_EBADF;
    DS_MSG1(MSG_LEGACY_ERROR,"dss_reg_ip_filter: Bad sockfd %d", sockfd);
    return DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    Validate each filter.
    Verify that if socket is bound the address/port binding matches the
    values contained in the filter
  -------------------------------------------------------------------------*/
  DSSOCKI_CACHE_GET_LOCAL(scb_ptr, localname);
  for (loop_var = 0; loop_var < num_filters; loop_var++)
  {
    ipfltr_ptr = (((ip_filter_type*)filters) + loop_var);
    /*-----------------------------------------------------------------------
      Verify network parameters
    -----------------------------------------------------------------------*/
    switch(ipfltr_ptr->ip_vsn)
    {
    case IP_V4:
      if ((uint8)IPFLTR_MASK_IP4_NEXT_HDR_PROT & ipfltr_ptr->ip_hdr.v4.field_mask)
      {
        next_hdr_prot = ipfltr_ptr->ip_hdr.v4.next_hdr_prot;
      }
      break;


    case IP_V6:
      if ((byte)DSS_AF_INET == scb_ptr->family)
      {
        *dss_errno = DS_EINVAL;
        DS_MSG2(MSG_LEGACY_ERROR,"dss_reg_ip_filter: v6 filter %d on v4 socket %d",
                       loop_var, sockfd);
        return DSS_ERROR;
      }

      if ((uint8)IPFLTR_MASK_IP6_NEXT_HDR_PROT & ipfltr_ptr->ip_hdr.v6.field_mask)
      {
        next_hdr_prot = ipfltr_ptr->ip_hdr.v6.next_hdr_prot;
      }
      break;


    default:
      *dss_errno = DS_EINVAL;
      DS_MSG2(MSG_LEGACY_ERROR,"dss_reg_ip_filter: filter %d invalid addr family %d",
                      loop_var, ipfltr_ptr->ip_vsn);
      return DSS_ERROR;
    }

    /*-----------------------------------------------------------------------
      Verify transport protocol parameters.

      Next header protocol must be specified. Otherwise all IP traffic will
      be snooped and sent up the stack starving laptop of all traffic.

      Make sure that local port is same as the destination port specified in
      the filter. If local port is 0, validation fails in ps_iface_ipfltr
      library validation if destination port is also specified as 0. Also
      make sure that range is 0 in order to ensure that filter is narrow.
      App shouldn't open socket x and snoop on data destined for sockets
      x to x + y.
    -----------------------------------------------------------------------*/
    switch (next_hdr_prot)
    {
      case PS_IPPROTO_TCP:
        if (((uint8)IPFLTR_MASK_TCP_DST_PORT &
              ipfltr_ptr->next_prot_hdr.tcp.field_mask) &&
            (localname.port != ipfltr_ptr->next_prot_hdr.tcp.dst.port ||
             0 != ipfltr_ptr->next_prot_hdr.tcp.dst.range))
        {
          *dss_errno = DS_EINVAL;
          DS_MSG3(MSG_LEGACY_HIGH,
                  "dss_reg_ip_filter: invalid destination port or range, "
                   "bound socket port %d, filter port %d, range %d",
                   localname.port,
                   ipfltr_ptr->next_prot_hdr.tcp.dst.port,
                   ipfltr_ptr->next_prot_hdr.tcp.dst.range);
          return DSS_ERROR;
        }
        break;

      case PS_IPPROTO_UDP:
        if (((uint8)IPFLTR_MASK_UDP_DST_PORT &
              ipfltr_ptr->next_prot_hdr.udp.field_mask) &&
            (localname.port != ipfltr_ptr->next_prot_hdr.udp.dst.port ||
             0 != ipfltr_ptr->next_prot_hdr.udp.dst.range))
        {
          *dss_errno = DS_EINVAL;
          DS_MSG3(MSG_LEGACY_HIGH,
                  "dss_reg_ip_filter: invalid destination port or range, "
                   "bound socket port %d, filter port %d, range %d",
                   localname.port,
                   ipfltr_ptr->next_prot_hdr.udp.dst.port,
                   ipfltr_ptr->next_prot_hdr.udp.dst.range);
          return DSS_ERROR;
        }
        break;

      case PS_IPPROTO_ICMP:
      case PS_IPPROTO_ICMP6:
        break;

      default:
        *dss_errno = DS_EINVAL;
        DS_MSG2(MSG_LEGACY_ERROR,"dss_reg_ip_filter: filter %d invalid next_hdr %d",
                       loop_var, next_hdr_prot);
        return DSS_ERROR;
    }
  }

  /*-------------------------------------------------------------------------
    Call filter registering function
  -------------------------------------------------------------------------*/
  DS_MSG1(MSG_LEGACY_HIGH,"dss_reg_ip_filter: filter %d",
                       loop_var);
  return dssocki_reg_ip_filter( sockfd,
                                num_filters,
                                filters,
                                filter_handle,
                                dss_errno );
} /* dss_reg_ip_filter() */



/*===========================================================================
FUNCTION DSS_DEREG_IP_FILTER()

DESCRIPTION
  This function deregisters all the IP filters previosuly added to an filter 
  handle associated with the specified socket.

DEPENDENCIES
  None.

PARAMETERS
  sockfd     - Descriptor of the Socket requesting filtering
  filter_handle - The handle for the filter to be deregistered
  dss_errno  - Ptr to contain error code on the return

RETURN VALUE
  0 for success, DSS_ERROR for error (sets dss_errno appropriately)

SIDE EFFECTS
  Removes socket client filters from the passed filter handle
===========================================================================*/
int dss_dereg_ip_filter
(
  sint15             sockfd,
  uint32             filter_handle,
  sint15            *dss_errno
)
{
  struct scb_s *scb_ptr;          /* ptr to socket control block structure */
  ds_ipfltr_delete_param_type     fltr_delete_param;
  sint15 dss_handle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == dss_errno)
  {
    DS_ERROR_LOG_0("dss_dereg_ip_filter: dss_errno is NULL");
    return DSS_ERROR;
  }

  *dss_errno = DSS_SUCCESS;

  //Retrieve the DSS Handle from the SCB
  scb_ptr = dssocki_sockfdtoscb(sockfd);
  if (NULL == scb_ptr)
  {
    *dss_errno = DS_EBADF;
    DS_ERROR_LOG_1( "dss_dereg_ip_filter: Bad sockfd %d", sockfd);
    return DSS_ERROR;
  }

  if (scb_ptr->acb_ptr == NULL)
  {
    *dss_errno = DS_EBADF;
    DS_ERROR_LOG_1( "dss_dereg_ip_filter: ACB ptr is NULL for sockfd %u", sockfd);
    return DSS_ERROR;
  }

  dss_handle = scb_ptr->acb_ptr->dss_handle;

  DS_MSG2( MSG_LEGACY_HIGH,"dss_dereg_ip_filter: DSS Net Handle %u, Dss Handle %u",
           scb_ptr->acb_ptr->dss_nethandle,dss_handle);

  fltr_delete_param.client_type = DS_FWK_CLIENT_SOCKETS;
  fltr_delete_param.client_handle = dss_handle;
  fltr_delete_param.client_id = FLTR_CLIENT_SOCKETS;
  fltr_delete_param.fi_handle = filter_handle;

  return( ds_ipfltr_delete(&fltr_delete_param, dss_errno));

} /* dss_dereg_ip_filter() */
