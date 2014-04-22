#ifndef DS3GPPNETWORKOVERRIDEHDLR_H
#define DS3GPPNETWORKOVERRIDEHDLR_H

/*===========================================================================
                      DS_PDN_NETWORK_OVERRIDE_HDLR.H
DESCRIPTION
This file declares header for network override handler APIs.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/inc/ds_3gpp_network_override_hdlr.h#1 $
  $DateTime: 2020/03/17 08:03:16 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ds_pdn_defs.h"
#include "ds_pdn_hdlr.h"
#include "ds_pdn_manager.h"
#include "ds_pdn_apn_table.h"
#include "ds_3gpp_pdn_throttle_sm.h"


#define DS_3GPP_PDP_UNDEFINED_BMASK        (0x1 << DS_3GPP_PDN_THROT_PDP_UNDEFINED)
#define DS_3GPP_PDP_IPV4_BMASK             (0x1 << DS_3GPP_PDN_THROT_PDP_IPV4)
#define DS_3GPP_PDP_IPV6_BMASK             (0x1 << DS_3GPP_PDN_THROT_PDP_IPV6)
#define DS_3GPP_PDP_IPV4V6_BMASK           (0x1 << DS_3GPP_PDN_THROT_PDP_IPV4V6_ONLY)
#define DS_3GPP_PDP_NON_IP_BMASK           (0x1 << DS_3GPP_PDN_THROT_PDP_NON_IP)

#define DS_3GPP_GET_NW_OVERRIDE_BMASK(val) (0x1 << val)

#define DS_3GPP_NW_OVERRIDE_IPV4_ALLOWED(val) \
            ((val & DS_3GPP_PDP_IPV4_BMASK) == DS_3GPP_PDP_IPV4_BMASK)

#define DS_3GPP_NW_OVERRIDE_IPV6_ALLOWED(val) \
            ((val & DS_3GPP_PDP_IPV6_BMASK) == DS_3GPP_PDP_IPV6_BMASK)

#define DS_3GPP_NW_OVERRIDE_IPV4V6_ALLOWED(val) \
            ((val & DS_3GPP_PDP_IPV4V6_BMASK) == DS_3GPP_PDP_IPV4V6_BMASK)

#define DS_3GPP_NW_OVERRIDE_NON_IP_ALLOWED(val) \
            ((val & DS_3GPP_PDP_NON_IP_BMASK) == DS_3GPP_PDP_NON_IP_BMASK)

/*===========================================================================
FUNCTION DS_3GPP_NETWORK_OVERRIDE_HDLR_RESET

DESCRIPTION
  This function initializes the data structure to NULL

PARAMETERS 
   rel12_plus_check  

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_3gpp_network_override_hdlr_reset
(
  boolean                rel12_plus_check  
);

/*===========================================================================
FUNCTION DS_3GPP_NETWORK_OVERRIDE_HDLR_RESET_IF_PLMN_CHANGED

DESCRIPTION
  This function initializes the data structure to NULL

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_3gpp_network_override_hdlr_reset_if_plmn_changed
(
  sys_plmn_id_s_type        plmn_id,
  ds_3gpp_throt_rat_e_type  rat_type,
  boolean                   rel12_plus_check
);

/*=========================================================================
FUNCTION DS_3GPP_NETWORK_OVERRIDE_HDLR_SET_OVERRIDDEN_IP

DESCRIPTION
  This function sets the passed IP type based on the apn entry passed.
  If the apn entry does not exist, it would create a new entry

PARAMETERS
   byte                              *apn
   ds_3gpp_network_overrridden_ip_type_e   ip_type
   override cause
   rat

DEPENDENCIES
  None.

RETURN VALUE
  boolean 

SIDE EFFECTS
  None

===========================================================================*/

boolean ds_3gpp_network_override_hdlr_set_overridden_ip
(
   byte                                  *apn,
   ds_3gpp_pdn_throt_ntw_o_ridden_ip_type_e  
                                          overridden_ip,
   ds_3gpp_pdn_throt_ntw_o_ride_cause      
                                          override_cause,
   uint32                                 rat_mask,
   reattempt_ind_T                        reattempt_indicator
);

/*===========================================================================
FUNCTION DS_3GPP_NETWORK_OVERRIDE_HDLR_GET_OVERRIDDEN_IP

DESCRIPTION
  This function returns the overridden IP on the specified rat

PARAMETERS
   byte                              *apn
   rat_type

DEPENDENCIES
  None.

RETURN VALUE
  Overriden IP Bmask - IPv4/IPv6/PDP_UNDEFINIED 

SIDE EFFECTS
  None
==============================================================================*/

uint8 ds_3gpp_network_override_hdlr_get_overridden_ip
(
   byte                              *apn,
   ds_3gpp_throt_rat_e_type           rat_type,
   sys_plmn_id_s_type                *plmn_id_p
);

/*===========================================================================
FUNCTION       DS_3GPP_NTW_OVERRIDE_GET_EPLMN_LIST_PER_PLMN

DESCRIPTION
  API used to get the list of throttled ePLMNs on a particular RAT for a
  PLMN ID that is passed to this function. 

PARAMETERS  
  eplmn_list_ptr - Pointer to the ePLMN list 
  rat_type       - Enum that indicates the RAT on which ePLMN list info
                   is needed.
                   
DEPENDENCIES 
  None.
  
RETURN VALUE 
  Returns a pointer to the ePLMN list in a particular RAT
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
sys_plmn_list_s_type* ds_3gpp_ntw_override_get_eplmn_list_per_plmn
(
  sys_plmn_id_s_type           plmn_id,
  ds_3gpp_throt_rat_e_type     rat_type
);

/*===========================================================================
FUNCTION DS_3GPP_NETWORK_OVERRIDE_HDLR_RESET_THROT_PER_RAT

DESCRIPTION
  This function is used to clear network override throttling information
  for a given rat.

PARAMETERS 
  rat_type  - RAT on which throttling needs to be cleared

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_3gpp_network_override_hdlr_reset_throt_per_rat
(
  ds_3gpp_throt_rat_e_type         rat_type
);

#endif
