#ifndef PS_ADDR_V6_H
#define PS_ADDR_V6_H
/** 
  @file ps_addr_v6.h 
  @brief 
   Contains common data declarations and function prototypes for 
   accessing IPv6 address(s).
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        P S _ A D D R _ V 6 . H

GENERAL DESCRIPTION
 Internet Protocol Version 6 - Interface Layer Functionality

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*=============================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The dms_mainpage.dox file contains all file/group descriptions 
      that are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      dms_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the tcpUdpIpIp6 group 
      description in the dms_mainpage.dox file. 
=============================================================================*/
/*=============================================================================
Copyright (c) 2018 - 2019 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR MODULE

  $Header: //components/rel/dataiot.mpss/2.0/protocols/inet/inc/ps_addr_v6.h#1 $
  $DateTime: 2020/03/17 08:03:16 $

  09/28/18   ds      Code changes for chiltepin(9x05).
===========================================================================*/
/* group: tcpUdpIpIp6 */

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "pstimer.h"
#include "ds_fwk.h"
#include "ds_defs.h"

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                         PUBLIC MACRO DEFINITIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/** @addtogroup macros_tcpUdpIpIp6
  @{
*/

/*===========================================================================
PS_IPV6_INFINITE_LIFE_TIME
===========================================================================*/
/** 
  @brief
  Defines the infinite life time constant.
 
  PS_IPV6_INFINITE_LIFE_TIME - Infinite life time

*/
#define PS_IPV6_INFINITE_LIFE_TIME  0xFFFFFFFFUL

/*===========================================================================
MACRO IPV6_ADDR_MSG()
===========================================================================*/
/**
  Prints an IPv6 address.

  @param[in] ip_addr   IPv6 address in network byte order.

  @return
  None.

  @dependencies 
  None.
*/
#define IPV6_ADDR_MSG(ip_addr) MSG_8(MSG_SSID_DS, \
                        MSG_LEGACY_HIGH, \
                        "IPV6 Address %x:%x:%x:%x:%x:%x:%x:%x", \
                        (uint16)(ps_ntohs(ip_addr[0])), \
                        (uint16)(ps_ntohs(ip_addr[0] >> 16)), \
                        (uint16)(ps_ntohs(ip_addr[0] >> 32)) , \
                        (uint16)(ps_ntohs(ip_addr[0] >> 48)), \
                        (uint16)(ps_ntohs(ip_addr[1])), \
                        (uint16)(ps_ntohs(ip_addr[1] >> 16)), \
                        (uint16)(ps_ntohs(ip_addr[1] >> 32)) , \
                        (uint16)(ps_ntohs(ip_addr[1] >> 48)))

/*===========================================================================
MACRO IPV6_PREFIX_MSG()
===========================================================================*/
/**
  Prints an IPv6 Prefix.

  @param[in] prefix   The IPv6 Prefix in network byte order.

  @return
  None.

  @dependencies 
  None.
*/
#define IPV6_PREFIX_MSG(prefix) MSG_4( MSG_SSID_DS, \
                                       MSG_LEGACY_HIGH, \
                                       "Prefix is %x:%x:%x:%x", \
                                       (uint16)(ps_ntohs(prefix)), \
                                       (uint16)(ps_ntohs(prefix >> 16)), \
                                       (uint16)(ps_ntohs(prefix >> 32)) , \
                                       (uint16)(ps_ntohs(prefix >> 48)))


/*===========================================================================
MACRO IPV6_IID_MSG()
===========================================================================*/
/**
  Prints an IPv6 IID.

  @param[in] iid   The IPv6 IID in network byte order.

  @return
  None.

  @dependencies 
  None.
*/
#define IPV6_IID_MSG(iid) MSG_4( MSG_SSID_DS, \
                                 MSG_LEGACY_HIGH, \
                                 "IID is %x:%x:%x:%x", \
                                 (uint16)(ps_ntohs(iid)), \
                                 (uint16)(ps_ntohs(iid >> 16)), \
                                 (uint16)(ps_ntohs(iid >> 32)) , \
                                 (uint16)(ps_ntohs(iid >> 48)))


/*===========================================================================
MACRO PS_GET_IP_V6_IID()
===========================================================================*/
/**
  Returns the IPv6 interface identifier (the v6 address suffix)
  assigned to the interface.

  @note1hang This is not the same as ps_iface.

  @param[in] fwk_inst_ptr   Pointer to the ds_fwk instance.

  @return
  IPv6 interface identifier (last 64 bits of the address).\n
  0 -- Interface pointer is NULL.

  @dependencies 
  None.
*/
#define PS_GET_IP_V6_IID( fwk_inst_ptr )                     \
ps_get_v6_iid(fwk_inst_ptr)

/*===========================================================================
MACRO PS_GET_IP_V6_PREFIX()
===========================================================================*/
/**
  Returns the IPv6 prefix (the v6 address prefix)
  assigned to the interface.

  @note1hang This is not the same as ps_iface.

  @param[in] fwk_inst_ptr   Pointer to the ds_fwk instance.

  @return
  IPv6 address prefix (first 64 bits of the address).\n
  0 -- Interface pointer is NULL.

  @dependencies 
  None.
*/
#define PS_GET_IP_V6_PREFIX( fwk_inst_ptr )                     \
ps_get_v6_prefix(fwk_inst_ptr)

/*===========================================================================
MACRO PS_V6_ADDR_MATCH
===========================================================================*/
/**
  Matches the passed IPv6 address with the possible IPv6 addresses of the 
  passed interface.

  @param[in] v6_addr_ptr   Pointer to IPv6 address to match.
  @param[in] fwk_ptr       DS fwk pointer.

  @return
  TRUE  -- Passed address matches any of the IPv6 addresses of the interface.\n
  FALSE -- Otherwise.

  @dependencies 
  None.
*/
#define PS_V6_ADDR_MATCH( v6_addr_ptr, fwk_ptr )              \
ps_v6_addr_match( v6_addr_ptr, fwk_ptr)

/*===========================================================================
MACRO PS_SET_IP_V6_IID()
===========================================================================*/
/**
  Sets the IPv6 interface identifier (the v6 address suffix) in
  the interface.

  @param[in] fwk_ptr     Pointer to the DS FWK instacne.
  @param[in] iid 6       4-bit IPv6 interface identifier. The IPv6 address 
                         suffix.

  @return
  FALSE -- Interface pointer is NULL.\n
  TRUE  -- Otherwise.
  
  @dependencies 
  None.
*/
#define PS_SET_IP_V6_IID( fwk_ptr, iid )                        \
 ps_set_v6_iid(fwk_ptr,iid)

/** @} */ /* end_addtogroup macros_tcpUdpIpIp6 */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                         PUBLIC FUNCTION DEFINITIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*===========================================================================
FUNCTION PS_GET_ALL_V6_PREFIXES()
===========================================================================*/
/** @ingroup ps_get_all_v6_prefixes
  Retrieves all of the prefixes on an interface along with the state and length 
  of each prefix.

  @datatypes
  #ds_fwk_s_type\n
  #ds_prefix_info_type

  @vertspace
  @param[in] fwk_inst_ptr   Pointer to the fwk instance on which to cleanup the
                            neighbor discovery caches.
  @param[in] prefix_info    Pointer to the prefix name, state, and length.
  @param[in] num_prefixes   Pointer to the space allocated for prefixes and the
                            number passed back.

  @return
  None.
  
  @dependencies 
  None.
  @newpage
*/
void ps_get_all_v6_prefixes
(
  ds_fwk_s_type             *fwk_inst_ptr,
  ds_prefix_info_type       *prefix_info,
  uint8                     *num_prefixes
);

/*===========================================================================
FUNCTION PS_APPLY_V6_PREFIX()
===========================================================================*/
/** @ingroup ps_apply_v6_prefix
  Applies a prefix to a particular interface. The initial instance only stores 
  a single prefix. 

  @datatypes
  #ds_fwk_s_type

  @vertspace
  @param[in] fwk_inst_ptr     Pointer to the fwk instance control block on
                              which the prefix is to be applied.
  @param[in] gateway_iid      Interface ID of the router.
  @param[in] prefix           Prefix being added.
  @param[in] valid_lifetime   Lifetime of prefix (seconds). Refer to RFC 2461 
                              @xhyperref{S12,[S12]}, Section 4.6.2.
  @param[in] pref_lifetime    Preferred lifetime for prefix. Refer to RFC 2462 
                              @xhyperref{S13,[S13]}, Section 2.
  @param[in] prefix_length    Length of the prefix.
 
  @return
   0 -- Prefix was applied successfully.\n
  -1 -- Failure, or the prefix was not applied.
  
  @dependencies 
  None.
  @newpage
*/
int ps_apply_v6_prefix
(
  ds_fwk_s_type *fwk_inst_ptr,
  uint64         gateway_iid,
  uint64         prefix,
  uint32         valid_lifetime,
  uint32         pref_lifetime,
  uint8          prefix_length
);

/*===========================================================================
FUNCTION PS_REMOVE_V6_PREFIX()
===========================================================================*/
/** @ingroup ps_remove_v6_prefix
  Removes a prefix from the interface. The function fails only when the prefix 
  does not exist on this interface.

  @datatypes
  #ds_fwk_s_type

  @vertspace
  @param[in] fwk_inst_ptr     Pointer to the fwk instance control block on
                              which the prefix is to be removed.
  @param[in] prefix           Prefix being removed.
 
  @return
   0 -- Prefix is removed successfully.\n
  -1 -- Failure.
  
  @dependencies 
  None.
  @newpage
*/
int ps_remove_v6_prefix
(
  ds_fwk_s_type *fwk_inst_ptr,
  uint64         prefix
);

/*===========================================================================
FUNCTION PS_DELETE_ALL_V6_PREFIXES()
===========================================================================*/
/** @ingroup ps_delete_all_v6_prefixes
  Removes all prefixes associated with the interface.

  @datatypes
  #ds_fwk_s_type

  @vertspace
  @param[in] fwk_inst_ptr     Pointer to the FWK instance control block on
                              which all prefixes are to be removed.
 
  @return
   0 -- All prefixes are removed successfully.\n
  -1 -- Failure.
  
  @dependencies 
  None.
  @newpage
*/
int ps_delete_all_v6_prefixes
(
  ds_fwk_s_type *fwk_inst_ptr
);

/*===========================================================================
FUNCTION PS_GENERATE_IPV6_IID()
===========================================================================*/
/** @ingroup ps_generate_ipv6_iid
  Generates a random IPv6 IID. This function ensures that the IID generated
  is unique on the interface and begins the DAD, if necessary.

  @datatypes
  #ds_fwk_s_type

  @vertspace
  @param[in]  fwk_inst_ptr     Pointer to the FWK instance control block for
                               which a random IPv6 IID is to be generated.
  @param[out] iid              Pointer to the IID to be returned by
                               this function.
  @param[out] ps_errno         Pointer to the error code when an error occurs.
 
  @return
  None.
  
  @dependencies 
  None.
  @newpage
*/
int ps_generate_ipv6_iid
(
  ds_fwk_s_type *fwk_inst_ptr, 
  uint64        *iid,            
  int16         *ps_errno
);

/*===========================================================================
FUNCTION PS_GET_V6_IID
===========================================================================*/
/** @ingroup ps_get_v6_iid
  Returns the IPv6 IID of an interface. If the interface is NULL or IPv4, this
  function returns NULL.

  @datatypes
  #ds_fwk_s_type

  @vertspace
  @param[in]  fwk_inst_ptr   Pointer to the target FWK instance.
 
  @return
  IPv6 interface identifier. The last 64 bits of the address.\n
  0 -- Interface pointer is NULL, or interface is IPv4 family.
  
  @dependencies 
  None.
  @newpage
*/
uint64 ps_get_v6_iid
(
  ds_fwk_s_type       *fwk_inst_ptr
);

/*===========================================================================
FUNCTION PS_GET_V6_PREFIX
===========================================================================*/
/** @ingroup ps_get_v6_prefix
  Returns the IPv6 prefix of an interface. If the interface is NULL or IPv4, this
  function returns NULL.

  @datatypes
  #ds_fwk_s_type

  @vertspace
  @param[in]  fwk_inst_ptr   Pointer to the target FWK instance.
 
  @return
  IPv6 prefix. The first 64 bits of the address.\n
  0 -- Interface pointer is NULL, or interface is IPv4 family.
  
  @dependencies 
  None.
  @newpage
*/
uint64 ps_get_v6_prefix
(
  ds_fwk_s_type       *fwk_inst_ptr
);

/*===========================================================================
FUNCTION PS_V6_ADDR_MATCH
===========================================================================*/
/** @ingroup ps_v6_addr_match
  Matches the passed IPv6 address with the possible IPv6 addresses of the 
  passed interface.

  @datatypes
  #ps_in6_addr\n
  #ds_fwk_s_type

  @vertspace
  @param[in] v6_addr_ptr      Pointer to the IPv6 address to match.
  @param[in] fwk_inst_ptr     Pointer to the FWK instance control block.
 
  @return
  TRUE  -- Passed address matches any of the IPv6 addresses of the interface.\n
  FALSE -- Otherwise.
  
  @dependencies 
  None.
  @newpage
*/
boolean ps_v6_addr_match
(
  struct ps_in6_addr *v6_addr_ptr,
  ds_fwk_s_type   *fwk_inst_ptr
);

/*===========================================================================
FUNCTION PS_SET_V6_IID
===========================================================================*/
/** @ingroup ps_set_v6_iid
  Sets the IPv6 IID of an interface. If the interface is NULL or IPv4, this 
  function returns FALSE.

  @datatypes
  #ds_fwk_s_type

  @vertspace
  @param[in] fwk_inst_ptr     Pointer to the FWK instance.
  @param[in] iid              64-bit IPv6 interface identifier.
                              The IPv6 address suffix.
 
  @return
  FALSE -- Interface pointer is NULL, IPv4, or Logical.\n
  TRUE -- Otherwise.
  
  @dependencies 
  None.
  @newpage
*/
boolean ps_set_v6_iid
(
  ds_fwk_s_type       *fwk_inst_ptr,
  uint64               iid
);

/*===========================================================================
FUNCTION PS_GET_V6_DNS_ADDRS
===========================================================================*/
/** @ingroup ps_get_v6_dns_addrs
  Returns the primary and secondary DNS addresses on the IPv6 interface.

  @datatypes
  #ds_fwk_s_type\n
  #ip_addr_type

  @vertspace
  @param[in] fwk_inst_ptr     Pointer to the FWK instance control block
                              for which the primary and secondary DNS addresses 
                              are to be obtained.
  @param[out] prim_dns_ptr    Primary IPv6 DNS address.
  @param[out] sec_dns_ptr     Secondary IPv6 DNS address.
 
  @return
  None.  However, if the address family is not IPv6, the output parameters
  are stored with zero.
 
  @dependencies 
  None.
  @newpage
*/
void ps_get_v6_dns_addrs
(
  ds_fwk_s_type         *fwk_inst_ptr,
  ip_addr_type          *prim_dns_ptr,
  ip_addr_type          *sec_dns_ptr
);

/*===========================================================================
FUNCTION PS_SET_V6_DNS_ADDRS
===========================================================================*/
/** @ingroup ps_set_v6_dns_addrs
  Sets the primary and secondary DNS addresses on the IPv6 interface.

  @datatypes
  #ds_fwk_s_type\n
  #ps_ip_addr_type

  @vertspace
  @param[in] fwk_inst_ptr     Pointer to the FWK instance control block
                              on which the primary and secondary DNS addresses 
                              are to be set.
  @param[in] prim_dns_ptr     Primary IPv6 DNS address.
  @param[in] sec_dns_ptr      Secondary IPv6 DNS address.
 
  @return
  0 -- Primary and secondary DNS addresses are set.\n
 -1 -- Failure.

  @dependencies 
  None.
  @newpage
*/
int ps_set_v6_dns_addrs
(
  ds_fwk_s_type         *fwk_inst_ptr,
  ps_ip_addr_type       *prim_dns_ptr,
  ps_ip_addr_type       *sec_dns_ptr
);

/*===========================================================================
FUNCTION PS_GET_V6_SIP_SERV_ADDR
===========================================================================*/
/** @ingroup ps_get_v6_sip_serv_addr
  Returns all the SIP server addresses stored on the IPv6 interface.

  @datatypes
  #ds_fwk_s_type\n
  #ip_addr_type

  @vertspace
  @param[in] fwk_inst_ptr              Pointer to the FWK instance control block
                                       for which the primary and secondary DNS
                                       addresses are to be obtained.
  @param[out] sip_serv_addr_ptr_array  Pointer to the SIP server addresses 
                                       array.
  @param[out] count                    As input value it contains the count
                                       of the SIP server addresses asked for
                                       and as a return value it contains the
                                       actual number of addresses returned.
 
  @return
  None.  However, if the address family is not IPv6, the output parameters
  are stored with zero.
 
  @dependencies 
  None.
  @newpage
*/
void ps_get_v6_sip_serv_addr
(
  ds_fwk_s_type      *fwk_inst_ptr,
  ip_addr_type       *sip_serv_addr_ptr_array,
  uint32             *count
);

/*===========================================================================
FUNCTION PS_IS_V6_ADDR_EXTERNAL
===========================================================================*/
/** @ingroup ps_is_v6_address_external
  Returns whether given ip6 address is internal or not

  @datatypes
  #ds_fwk_s_type\n
  #ps_in6_addr

  @vertspace
  @param[in] fwk_inst_ptr              Pointer to the FWK instance control block
                                       for which the primary and secondary DNS
                                       addresses are to be obtained.
  @param[in] ipv6_addr_ptr             Pointer to the IPV6 address 
  
 
  @return
  TRUE : IPv6 address is external.
  FALSE : IPv6 address is not external.
  
  @dependencies 
  None.
  @newpage
*/
boolean ps_is_v6_address_external
(
  ds_fwk_s_type             * fwk_inst_ptr,
  struct ps_in6_addr        * ipv6_addr_ptr
);

#endif /* PS_ADDR_V6_H */
