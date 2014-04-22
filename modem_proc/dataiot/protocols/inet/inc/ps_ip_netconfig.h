#ifndef PS_IP_NETCONFIG_H
#define PS_IP_NETCONFIG_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            P S _ I P _ N E T C O N F I G . H

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/**
   @file ps_ip_netconfig.h
   @brief This is the header file that defines all of the IP addressing types and
  macros. 
 */

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
Copyright (c) 2008-2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary.
=============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ip_addr.h_v   1.5   11 Oct 2002 09:15:46   ubabbar  $
  $Header: //components/rel/dataiot.mpss/2.0/protocols/inet/inc/ps_ip_netconfig.h#1 $ $DateTime: 2020/03/17 08:03:16 $ $Author: pwbldsvc $

when        who    what, where, why
-------     ---    ----------------------------------------------------------
08/28/11    sd     (Tech Pubs) Applied new Doxygen grouping markup.
02/03/11    cp     Removed the use of first hop mtu variable.
07/30/10    cp     Propagating the first hop mtu value to TE for v6.
12/01/10    SCD    (Tech Pubs) Edited Doxygen markup and comments.
12/01/10    sa     Modified the comments according to doxygen specific 
                   semantics.
03/26/09    pp     CMI De-featurization and Removed unused defs.
12/14/08    pp     Created module as part of Common Modem Interface: 
                   Public/Private API split.
===========================================================================*/
/* group: tcpUdpIpIp6 */



/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "ps_in.h"
#include "nv_items.h"
#include "ps_ppp_ext.h"

/** @addtogroup datatypes 
  @{
*/

/*---------------------------------------------------------------------------
  struct definitions to pass UM config info(ipcp, auth etc) in IFACE bring up command function.
  This replaces the previous struct used by umts (dsumtsps_rm_ppp_info)
  Now defined in a common place as will be used by cdma in future
---------------------------------------------------------------------------*/  

/** 
  @brief Structure definitions used to pass UM configuration 
  information (e.g., ipcp, auth) in IFACE bring up command function.
*/
typedef struct
{
  uint32  ip_address;          /**< IP address requested. */
  uint32  primary_dns;         /**< Primary DNS address. */
  uint32  secondary_dns;       /**< Secondary DNS address. */
  uint32  primary_nbns;        /**< Primary NBNS address. */
  uint32  secondary_nbns;      /**< Secondary NBNS address. */
} ipcp_info_type;

/**
  @brief Network parameters information for the interface. 
*/
typedef struct
{
  uint32 valid_fields;          /**< Mask to validate the authentication
                                     parameters. */
  /** Structure to hold authentication information. */
  struct
  {
    uint8 auth_type;            /**< Authorization negotiated. Valid values: CHAP, PAP, or 
                                     None. */
    ppp_auth_info_type params;  /**< Pointer to authorization information 
                                     parameters. */
    uint8 auth_id;              /**< ID used for authorization packets. */
  }auth_info;
  byte dial_string[NV_PKT_DIAL_STRING_SIZE]; /**< Dial string passed by ATCOP. */
  ipcp_info_type ipcp_info;     /**< IPCP information. */
} network_params_info_type;

/** @} */ /* end_addtogroup datatypes  */

#endif /* PS_IP_NETCONFIG_H */
