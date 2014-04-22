#ifndef SECIPS_H
#define SECIPS_H

/*===========================================================================

               A S W   S E C U R I T Y   S E R V I C E S

                        I P S E C   L A Y E R

                 D M S S   I P S E C   I N T E R F A C E

          I N I T I A L I Z A T I O N   F U N C T I O N   A N D

      A C C E S S   C O N T R O L   L I S T   D E C L A R A T I O N S


    Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
    Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

/**
  @file  secips.h

  Provides an interface to the functions that are used <b>ONLY by internal 
  applications</b> to establish the IPsec tunnel using IKEv2.

  These functions are used for allocating the logical interface and for setting 
  the Secirity Policy, self/peer identity and authentication, and endpoint information, 
  among others.

  Externalized functions:
  None.
*/
  	
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: $
  $Header: //components/rel/dataiot.mpss/2.0/api/public/secips.h#1 $ $DateTime: 2020/03/17 08:03:16 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/21/15   ks      API to reset NATT keep alive timer
10/07/15   ks      Src_port update to SA_info callback
04/15/14   nm      NULL APN Support
1/30/14    nm      Increase iface support to 16
10/07/13   JK      Add support for IKEv2 Trigger DPD API within IPSEC to be used by IPSEC
10/07/13   JK      ADDED new API  to support IPSEC SA Rekey trigger
07/08/13   nm      Abort Tunnel API addition
07/28/10   dm      Re-architectured IPsec API files
07/07/10   sw      (Tech Pubs) edited/added Doxygen comments and markup.
11/20/09   sp      Using new PS naming conventions.
09/22/09   nk      IPsec CMI Decoupling.
08/14/08   sp      Fix for CR 154340 - Added a new API, which can be
                   called by the application to clear the ID list.
08/01/08   sp      Support for mapping EAP-AKA Notification code
                   with PS_NET_DOWN_REASON code.
07/09/08   sp      Added support for IKEv2 Multiple Authentication.
05/14/07   sl      Added cb for IKE SA establishment failure
05/14/07   jg      Added secipsiface_set_selector().
04/25/07   jg      New secipsiface interface functions.
04/23/07   ssm/sl  IKEv2 support
11/23/06   ssm/sv  Manual Keying/Transport Mode support
04/20/05   ssm     Added Phase2 renegotiation timers and code cleanup
01/25/05   sv/ssm  Re-architectured IPsec to support multiple SA's and added
                   support for AH.
01/31/04   usb     Removed externalization of SEC ACL.
05/25/03   om      Created Module.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ps_in.h"
#include "secapi.h"
#include "IPSec.h"
#include "secipssp.h"
#include "dsm.h"

/*===========================================================================

                     DATA TYPES AND DEFINITIONS

===========================================================================*/
/* ---------- Not needed in the PDF - Start ---------- */
/** @cond */

/** Holds the fields that are part of an Authentication Header.
 */ 
typedef struct
{
  uint8  next_header;  /**< Protocol of the next header. */
  uint8  payload_len;  /**< Size of the payload. */
  uint16 reserved;     /**< Reserved bytes (value is 0). */
  uint32 spi;          /**< Security parameter index. */
  uint32 seqno;        /**< Sequence number. */
  uint8  auth_data[SECIPSTYPES_AH_AUTH_DATA_SIZE];  /**< Authentication data. */
}secips_ah_hdr;

/**
   @}
*/ 
/* end_group ipsec_internal_ikev2_ds */

/* ---------- Not needed in the PDF - Start ---------- */
  /** @cond */

/* Security suite type.*/
typedef struct
{
  uint32                   ipsec_handle;  /* --Pointer to the SPD information. */
  secipssa_ipsec_sa_type*  esp_sa_ptr;
  secipssa_ipsec_sa_type*  ah_sa_ptr;
} secipsiface_sec_suite_type;

/* ---------- Not needed in the PDF - End ---------- */
/** @endcond */

/**
@addtogroup ipsec_internal_ikev2_api

  @{ */

 /**
   Generates IPsec information that includes the interface information from a 
   given IPsec handle.
 
   @param *ipsec_handle [in] -- IPsec handle that is the IPsec filtering result.
   @param *pkt_info_ptr [in] -- IP packet information.
   @param *ipsec_info  [out] -- IPsec interface information.
   @param user_id		[in] -- Information provided by the user of the IPsec.
 
   @return
   secerrno_enum_type -- E_SUCCESS or E_FAILURE.
 
	  @dependencies
   None.
 */
 secerrno_enum_type secips_generate_ipsec_info
 (
   void	                   *ipsec_handle,
   ip_pkt_info_type        *pkt_info_ptr,
   ps_ipsec_info_type      *ipsec_info,
   uint32                  user_id
 );

 /**
   Parses the Encapsulated Security Payload header of an incoming IP packet.
 
   @param **bp	   [in/out] -- Reference to the incoming IP packet.
   @param *offset	   [in] -- offset to the IP packet, from where ESP header is 
							   to be extracted.
   @param *ip_pkt_info [in] -- IP packet information.
 
   @return
   E_SUCCESS or a corresponding failure code.
 
   @dependencies
   None.
 */
 int secipsesp_hdr_parse
 (
   dsm_item_type     **bp,
   uint16            *offset,
   ip_pkt_info_type  *ip_pkt_info
 );
 
 /**
   Creates an Encapsulated Security Payload header for a given IP packet.
 
   @param *ipsec_sa_ptr    [in] -- Reference to the ESP SA.
   @param *next_header_ptr [in] -- Header next to the ESP header.
   @param **msg_ptr    [in/out] -- Reference to the IP packet on which the ESP 
								  header is to be created.
 
   @return
   E_SUCCESS or a corresponding failure code.
 
   @dependencies
   None.
 */
 int secipsesp_hdr_create
 (
  void            *ipsec_sa_ptr,
  uint8           *next_header_ptr,
  dsm_item_type   **msg_ptr
 );

/**
  Creates an Authentication Header for a given IP Packet.

  @param *ipsec_sa_ptr [in] -- Reference to the AH SA.
  @param next_header   [in] -- Header next to the AH.
  @param ip_pkt_info   [in] -- IP packet information.
  @param **msg_ptr [in/out] -- Reference to the IP packet on which the AH is to 
                               be created.

  @return
  E_SUCCESS or a corresponding failure code.

  @dependencies
  None.
*/
int secipsah_hdr_create
(
  void*                   ipsec_sa_ptr,
  uint8                   next_header,
  ip_pkt_info_type        ip_pkt_info,
  dsm_item_type**         msg_ptr
);

/**
  Parses the Authentication Header of an incoming IP packet.

  @param **msg_ptr [in/out] -- Reference to the incoming IP packet.
  @param *offset       [in] -- Offset to the IP packet, from where the AH is to 
                               be extracted.
  @param *pkt_info_ptr [in] -- IP packet information.

  @return
  E_SUCCESS or a corresponding failure code.

  @dependencies
  None.
*/
int secipsah_hdr_parse
(
 dsm_item_type**         msg_ptr,
 uint16                 * offset,
 ip_pkt_info_type       * pkt_info_ptr
);

#endif /* SECIPS_H */

