#ifndef DS_PDN_CALL_HDLR_H
#define DS_PDN_CALL_HDLR_H

/*===========================================================================
                      DS_PDN_CALL_HDLR.H
DESCRIPTION
This file exposes PDN call handler through APIs and data structures.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/inc/ds_pdn_call_hdlr.h#1 $
  $DateTime: 2020/03/17 08:03:16 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "ds_pdn_defs.h"
#include "ds_pdn_apn_table.h"
#include "ds_pdn_pco_epco_hdlr.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
/*===========================================================================
FUNCTION  DS_PDN_CALL_HDLR_GET_ATTACH_PDN_BASED_ON_PROFILE

DESCRIPTION
This function retuens the attach pdn basedon the profile pdp type

PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE
  pdn_cntx_p : OUT - Pointer to PDN context if there is one already
                     present in APN table.
                     
SIDE EFFECTS
  None.

===========================================================================*/

ds_pdn_mgr_s * ds_pdn_call_hdlr_get_attach_pdn_based_on_profile
(
 ds_pdn_mgr_s              *v4_pdn_p,
 ds_pdn_mgr_s              *v6_pdn_p,
 ds_pdn_mgr_s              *non_ip_pdn_p,
 ds_profile_context_type    attach_profile_context,
 ds_apn_ip_support_type_e   attach_ip_support
);

/*===========================================================================
FUNCTION DS_PDN_CALL_HDLR_FILL_NON_IP_LINK_MTU_CONTAINER

DESCRIPTION
  This function fills the Non-IP mtu PCO container
  
PARAMETERS 
    protocol_cfg_options_T    *protocol_config_options_p 
    *profile_p                 Ptr to ds profile
    subs_id                    Subscription Id
                               
DEPENDENCIES 
  None.

RETURN VALUE  
  None.
  
SIDE EFFECTS 
  Updates Orig_params contents : 
     orig_params_ptr->protocol_config_options.num_protocol_ids will be updated
     to reflect the no. of protocol config options included by this function
     
===========================================================================*/
void ds_pdn_call_hdlr_fill_non_ip_link_mtu_container
(
 ds_pdn_protocol_cfg_options_s  *protocol_config_options_p
);

/*===========================================================================
FUNCTION      DS_PDN_CALL_HDLR_DECODE_NON_IP_LINK_MTU

DESCRIPTION
  This function decodes the non-ip link mtu size from the PCO.

PARAMETERS
  protocol_pkt_ptr   -  pointer to the PCO contents field
  protocol_pkt_len   -  container length
  pdn_context_p      -  Pointer to current PDN context
  non_ip_iface           - non_ip_iface of the PDN
  decode_mask        -  Mask pointer containing status of decode operation

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void ds_pdn_call_hdlr_decode_non_ip_link_mtu
(
 const byte           *protocol_pkt_ptr, /* Pointer to the buffer forMTU info */
 byte                  protocol_pkt_len,  /* Length of the MTU container */
 ds_pdn_mgr_s         *pdn_mgr_p,
 uint32               *decode_mask_ptr
);

/*===========================================================================
FUNCTION DS_3GPP_CALL_HDLR_CHECK_IF_PDP_TYPE_THROTTLED

DESCRIPTION
  This function checks if the iface bringup needs to be done or not based on the
  network override throttle information.

PARAMETERS   :
      iface_p                                 Pointer to the iface that is being brought up
     pdn_cntx_p                        Pointer to the PDN context
     network_mode                 Current network_mode of operation
      subs_id                               Subscription ID
 
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If PDP type throttled
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/

boolean  ds_3gpp_call_hdlr_check_if_pdp_type_throttled
(
   ds_pdn_mgr_inst_type_s         *inst_p,
   ds_pdn_mgr_s                   *pdn_cntx_p,
   sys_sys_mode_e_type            network_mode
 );

/*===========================================================================
FUNCTION      DS_3GPP_CALL_HDLR_DISALLOW_CLASS3_PDN

DESCRIPTION
  this function will be called when iface bringup validation. it will check
  below scenerio:
  don't allow class3 pdn bringup when the class 3 APN in the APN table is 
  different from the APN chosen by the network for the attach.

PARAMETERS
  ds_pdn_mgr_s            *pdn_mgr_p
  char                    *resolve_apn_ptr
  
DEPENDENCIES
  Called in DS TASK context.

RETURN VALUE
  TRUE:validation pass
  FALSE:validation false

SIDE EFFECT
  None.
===========================================================================*/
boolean ds_pdn_call_hdlr_disallow_class3_pdn
(
  ds_pdn_mgr_s            *pdn_mgr_p,
  char                    *resolve_apn_ptr
);



#endif /* DS_PDN_CALL_HDLR_H */

