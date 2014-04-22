/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            P S _ I C M P 6 _ N D . H

GENERAL DESCRIPTION
 Internet Protocol Version 6 - Neighbor Discovery(IPv6 ND)

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c)2007-2020 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                           EDIT HISTORY FOR MODULE

  $Header: //components/rel/dataiot.mpss/2.0/protocols/inet/inc/ps_icmp6_nd.h#2 $
  $Author: pwbldsvc $ $DateTime: 2020/04/12 07:58:21 $

  04/12/20    ds     Code changes to add support for NS/NA procedure.
  09/28/18    ds     Code changes for IOT.
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#include "datamodem_variation.h"
#if defined (FEATURE_DATA_PS) && defined (FEATURE_DATA_PS_IPV6)

#include "ps_in.h"
#include "ps_icmp6_msg.h"
#include "ps_addri_mgmt.h"
#include "ds_rmsmi.h"

/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/
#define LAN_IEEE_MAC_ADDR_LEN        6

typedef enum
{
  ICMP6_ND_INVALID_EV = 0,
  ICMP6_ND_RA_SENT_EV = 1,
  ICMP6_ND_LL_ADDR_RESOLVED_EV = 2
} ps_icmp6_nd_event_enum_type;


typedef void (*ps_icmp6_ra_cback_f_ptr_type)
(
  uint8                       event,
  void*                       user_data_ptr
);

/**
  @brief ICMP6 neighbor discovery to start the configuration items.
*/
typedef struct
{
  ps_icmp6_ra_cback_f_ptr_type  cback_f_ptr;  
    /**< Callback to be called in case of ND events. */
  void                         *usr_data_ptr;  
    /**< User data to be passed back in the callback. */	
  ds_rmsm_rm_dev_instance_type  dev_instance;	
    /**< RMSM dev instance */
} ps_icmp6_ra_cb_config_type;

/*===========================================================================

FUNCTION PS_ICMP6_RA_CB_REGISTER()

DESCRIPTION
  Registers callback with protocol to get notified when the RA message 
  gets sent

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

void ps_icmp6_ra_cb_register
(
  ps_icmp6_ra_cb_config_type         *cb_config
);

/*===========================================================================

FUNCTION PS_ICMP6_RA_CB_DEREGISTER()

DESCRIPTION
  Deregisters callback from protocol for RA sent callback

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

void ps_icmp6_ra_cb_deregister
(
  ds_rmsm_rm_dev_instance_type dev_instance
);

/*===========================================================================

FUNCTION PS_ICMP6_ND_PROCESS_RA()

DESCRIPTION

 The router advertisement message and parameters are all verified. Then
 action is taken to apply prefixes, if any were validated properly.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - RA processed properly
  FALSE - There was an error in processing the RA
SIDE EFFECTS
  None
===========================================================================*/
boolean ps_icmp6_nd_process_ra
(
  ds_fwk_s_type     *ds_fwk_inst_ptr, /* Input interface                  */
  ps_icmp6_msg_type  *icmp6,           /* ICMP6 Message                    */
  struct ps_in6_addr *gateway_ptr      /* Gateway which sent RA            */
);

/*===========================================================================
FUNCTION PS_ICMP6_ND_CREATE_RA()

DESCRIPTION
  This function will compose and send a router advertisement in response
  to a router solicitation. The message type is formed according to passed
  parameters. Currently only responses on Rm interfaces are supported.

PARAMETERS
  this_iface_ptr: The pointer to the interface on which to send the RA.
  dest_addr: The pointer to the destination address.
  dep:  Whether or not the prefix should be depricated (set the preferred value
        to 1).

RETURN VALUE
  0 for success
 -1 for failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_icmp6_nd_create_ra
(
  ds_fwk_s_type                  *this_fwk_ptr,
  struct ps_in6_addr             *dest_addr,
  boolean                         deprecate_prefix
);

/*===========================================================================
FUNCTION PS_ICMP6_ND_SEND_RA()

DESCRIPTION
  This function will compose and send a router advertisement in response
  to a router solicitation. The message type is formed according to passed
  parameters. Currently only responses on Rm interfaces are supported.

PARAMETERS
  this_iface_ptr: The pointer to the interface on which to send the RA.
  dest_addr:      The pointer to the destination address

RETURN VALUE
  0 for success
 -1 for failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_icmp6_nd_send_ra
(
  ds_fwk_s_type          *this_fwk_ptr,
  struct ps_in6_addr     *dest_addr
);

/*===========================================================================

FUNCTION PS_ICMP6_ND_PROCESS_RS()

DESCRIPTION

 Validates the router solicitation and sends a router advertisement in
 response to the sender.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - RS processed properly
  FALSE - There was an error in processing the RS

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_icmp6_nd_process_rs
(
  ds_fwk_s_type *ds_fwk_inst_ptr,     /* FWK instance ptr                  */
  ps_icmp6_msg_type *icmp6,           /* ICMP6 Message                     */
  ip_pkt_info_type  *ip_pkt_info_ptr  /* IP packet information             */
);

/*===========================================================================
FUNCTION PS_ICMP6_ND_SEND_NA()

DESCRIPTION
  This function will compose and send a neighbor advertisement in response
  to a neighbor solicitation. The message type is formed according to passed
  parameters.

PARAMETERS
  fwk_inst_ptr:   The pointer to the interface on which to send the NA.
  dest:           The destination address to send to
  target:         The target address to use in the NA
  na_type:        The type of neighbor advertisement we are sending
  override:       Whether the included link layer addr should override
                  what's in the peers cache

RETURN VALUE
  0 for success
 -1 for failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_icmp6_nd_send_na
(
  ds_fwk_s_type          *fwk_inst_ptr,
  struct ps_in6_addr     *dest,
  struct ps_in6_addr     *target,
  ps_icmp6_nd_msg_type    na_type,
  boolean                 override
);

/*===========================================================================

FUNCTION PS_ICMP6_ND_PROCESS_NS()

DESCRIPTION

 The  neighbor solicitation message and parameters are all verified. Then
 action is taken to send neighbor advertisement message accrodingly.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - NS processed properly
  FALSE - There was an error in processing the NS

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_icmp6_nd_process_ns
(
  ds_fwk_s_type     *ds_fwk_inst_ptr, /* Input interface                   */
  ps_icmp6_msg_type *icmp6,           /* ICMP6 Message                     */
  ip_pkt_info_type  *ip_pkt_info_ptr  /* IP packet information             */
);

#endif /* FEATURE_DATA_PS && FEATURE_DATA_PS_IPV6 */
