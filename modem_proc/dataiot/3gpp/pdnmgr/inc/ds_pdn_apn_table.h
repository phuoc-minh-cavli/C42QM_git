#ifndef DS_PDN_APN_TABLE_H
#define DS_PDN_APN_TABLE_H

/*===========================================================================
                      DS_PDN_APN_TABLE.H
DESCRIPTION
This file exposes APN table through APIs and data structures.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/inc/ds_pdn_apn_table.h#1 $
  $DateTime: 2020/03/17 08:03:16 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ds_pdn_defs.h"


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define DS_MAX_APN_NUM DS_3GPP_MAX_PDN_CONTEXT

typedef enum 
{
     DS_IP_TYPE_INVALID = 0,
     DS_IPV4_ONLY,
     DS_IPV6_ONLY,
     DS_NON_IP,
     DS_IPV4_SINGLE_BEARER,
     DS_IPV6_SINGLE_BEARER,
     DS_NON_IP_SINGLE_BEARER,
     DS_SINGLE_BEARER,
     DS_IPV4V6,
     DS_IPV4_NON_IP,
     DS_IPV6_NON_IP,
     DS_IPV4V6_NON_IP
} ds_apn_ip_support_type_e;

typedef struct 
{
  uint32                      cookie;
  char                        apn[DS_PROFILE_APN_STRING_LEN+1];  /* APN string */
  char                        resolved_apn[DS_PROFILE_APN_STRING_LEN+1];  
                                   /* APN string as resolved by NW. Filled up if apn 
                                                                  requested OTA is NULL */
  ds_apn_ip_support_type_e    ip_support;                     /* PDN IP support */
  ds_pdn_mgr_s*               v4_pdn_ptr;
  ds_pdn_mgr_s*               v6_pdn_ptr;
  ds_pdn_mgr_s*               non_ip_pdn_ptr;
} ds_pdn_mgr_apn_table_s;

#define APN_VALID_COOKIE  0xa6a6a6a6

/*============================================================
FUNCTION DS_APN_TABLE_GET_APN_TABLE_PTR

DESCRIPTION
  This function searchs the APN table and returns the APN table ptr of a 
  particular APN

PARAMETERS
  APN name
  APN table pointer - to be filled
   
DEPENDENCIES
  None.

RETURN VALUE
  loop index - Index in the APN table, if found
  -1            - If not entry found 

SIDE EFFECTS
  None
=============================================================*/
int ds_apn_table_get_apn_table_ptr 
(
  char                      *apn,
  ds_pdn_mgr_apn_table_s   **apn_ptr
);

/*============================================================
FUNCTION DS_APN_TABLE_GET_APN_IP_SUPPORT_INFO

DESCRIPTION
  This function searchs the APN table and returns the ip support type of a 
  particular APN

PARAMETERS
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If APN table has an entry for this apn name
  FALSE- Otherwise

SIDE EFFECTS
  None
=============================================================*/
boolean ds_apn_table_get_apn_ip_support_info 
(
  char                      *apn,
  ds_apn_ip_support_type_e  *ip_support
);

/*===========================================================================
FUNCTION DS_APN_TABLE_UPDATE_APN_TABLE

DESCRIPTION
  This function updates the APN table with the supplied info

PARAMETERS   :
    
DEPENDENCIES
  Remember to call update_pdn_ptr fn before you update the IP support of the 
  entry in the APN table. Should the order be reversed, APN table behaviour
  will be unpredictable.

RETURN VALUE
  TRUE - Successfully updated APN table
  FALSE- Otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_apn_table_update_apn_table
(
  char                     *apn,
  ds_apn_ip_support_type_e  ip_support,
  ds_pdn_mgr_s             *pdn_ptr
);

/*===========================================================================
FUNCTION DS_APN_TABLE_REMOVE_ENTRY_IN_APN_TABLE

DESCRIPTION
  This function removes the corresponding entry in the APN table for a
  specific APN

PARAMETERS   :
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Successfully removed this entry
  FALSE- Otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_apn_table_remove_entry_in_apn_table
(
  ds_pdn_mgr_s   *incoming_pdn_mgr_p
);

/*===========================================================================
FUNCTION DS_APN_TABLE_GET_PDN_PDP_TYPE

DESCRIPTION
  This function gets the ip support type of the given pdn context pointer

PARAMETERS   
    
DEPENDENCIES
  None.

RETURN VALUE
  IP support type of the pdn context

SIDE EFFECTS
  None

===========================================================================*/
ds_profile_pdp_type_e ds_apn_table_get_pdn_pdp_type
(
  ds_pdn_mgr_s*       pdn_mgr_p
);

/*===========================================================================
FUNCTION DS_APN_TABLE_IS_PDN_ASSOCIATED_FOR_APN

DESCRIPTION
  This function searchs the APN table and returns whether pdn context exists
  for a given apn

PARAMETERS
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If there is a pdn context associated with this apn
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_apn_table_is_pdn_associated_for_apn
(
  char                      *apn
);

/*===========================================================================
FUNCTION DS_APN_TABLE_GET_IP_SUPPORT_INFO_FOR_PDN

DESCRIPTION
  This function searchs the APN table and returns the ip support type of a 
  particular PDN. Note that DS_IPV4V6_SINGLE_BEARER is not a valid support
  type for a particular PDN since there would be 2 PDN connectivity to the 
  same APN and this particular PDN can only be one of them.

PARAMETERS
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If APN table has an entry for this pdn
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_apn_table_get_ip_support_info_for_pdn
(
  ds_pdn_mgr_s              *pdn_mgr_ptr,
  ds_apn_ip_support_type_e  *ip_support
);

/*===========================================================================
FUNCTION DS_APN_TABLE_GET_V4_PDN

DESCRIPTION
  This function returns the pointer to the V4 PDN

PARAMETERS   :
  byte*                  - APN name
  ds_pdn_mgr_s**  - Pointer to the PDN context
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If APN table has an entry for this apn name
  FALSE- Otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_apn_table_get_v4_pdn
(
  char                      *apn,
  ds_pdn_mgr_s             **pdn_mgr_p
);

/*===========================================================================
FUNCTION DS_APN_TABLE_GET_V6_PDN

DESCRIPTION
  This function returns the pointer to the V6 PDN

PARAMETERS   :
  byte*                  - APN name
  ds_pdn_mgr_s** - Pointer to the PDN context
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If APN table has an entry for this apn name
  FALSE- Otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_apn_table_get_v6_pdn
(
  char                    *apn,
  ds_pdn_mgr_s           **pdn_mgr_p
);

/*===========================================================================
FUNCTION DS_APN_TABLE_GET_NON_IP_PDN

DESCRIPTION
  This function returns the pointer to the nonIP PDN

PARAMETERS   :
  byte*                  - APN name
  ds_pdn_mgr_s** - Pointer to the PDN context
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If APN table has an entry for this apn name
  FALSE- Otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_apn_table_get_non_ip_pdn
(
  char                    *apn,
  ds_pdn_mgr_s           **pdn_mgr_p
);

/*===========================================================================
FUNCTION DS_APN_TABLE_UPDATE_ALLOWED_IP_TYPE

DESCRIPTION
  This function blocks a specific IP type.
  1. If the IP_type becomes unavailable due to address configuration failure.
     In this case IP support will change from V4V6 to V4_ONLY or V6_ONLY.
  2. If the profile is configured single IP but OTA request is V4V6
     In this case IP support will change from SINGLE_BEARER to V4_ONLY or
     V6_ONLY.
  This function should not be called to change the the ip_support from one
  IP type to another V6 ex of unsupported transition (V4_Single_bearer to
  V6_Single_bearer or V6_ONLY)
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_apn_table_update_allowed_ip_type
(
  ds_pdn_mgr_s             *pdn_mgr_ptr,
  ds_apn_ip_support_type_e  allowed_ip_type
);

/*===========================================================================
FUNCTION DS_APN_TABLE_UPDATE_RESOLVED_APN_NAME

DESCRIPTION
  This function updates an entry in the APN table with the resolved APN name.
  This is used only if the APN name in the profile was NULL

PARAMETERS   :
  pdn_mgr_ptr* - Pointer to PDN context
  byte*              - APN name as returned by the network

DEPENDENCIES
  None

RETURN VALUE
  TRUE - if the entry corresponding to the profile APN name was found
  FALSE - if otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_apn_table_update_resolved_apn_name 
(
  ds_pdn_mgr_s *pdn_mgr_p,
  char         *resolved_apn_name
);

/*==============================================================================
FUNCTION DS_APN_TABLE_GET_RESOLVED_APN_NAME

DESCRIPTION
  This function is used to return the resolved name given 
  the pdn context

PARAMETERS   :

  IN
  pdn_mgr_ptr* - Pointer to PDN context

  OUT
  byte** - Resolved APN name as returned by the network

DEPENDENCIES
  None

RETURN VALUE
  TRUE - if the entry corresponding to the profile APN name was found
  FALSE - if otherwise

SIDE EFFECTS
  None
==============================================================================*/
boolean ds_apn_table_get_resolved_apn_name
(
  ds_pdn_mgr_s  *pdn_mgr_p, 
  char         **res_apn_name
);

/*===========================================================================
FUNCTION ds_apn_table_add_pdn

DESCRIPTION
  This function adds an entry to the APN table with the supplied info

PARAMETERS
  apn        - name of the APN
  ip_support - IP support for the APN 
  pdn_ptr    - Pointer to the PDN context


    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Successfully updated APN table
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_apn_table_add_pdn
(
  char                      *apn,
  ds_apn_ip_support_type_e   incoming_ip_support,
  ds_pdn_mgr_s              *pdn_ptr
);

/*===========================================================================
FUNCTION DS_APN_TABLE_RECOMPUTE_IP_SUPPORT_FOR_APN

DESCRIPTION
  This function recomputes the IP support for a given APN. Recomputation is
  done based on two aspects:
  1. a prior knowledge about the IP support of an APN. This is usually 
     assumed based on the PDP type
  2. New data from the network - The new data that this function uses is
     from the PDN reject indication. If the call is accepted, there is no
     need to recompute as the assumed IP support is correct.

  This function gets the recomputed IP support and removes the PDN context
  pointer (that was rejected) from the APN table. It also updates the APN
  table so that the correct info is present in the table.

PARAMETERS
  IN
    rejected_pdn_cntx_p - PDN context that has to be removed from the table
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If APN table has an entry for this apn name
  FALSE- Otherwise

SIDE EFFECTS
  When bearer_down_indication is processed, we will call remove_entry_from_apn
  with the same PDN context once again. However, that function call will result
  in no operation since the PDN context pointer was already removed.

===========================================================================*/
void ds_apn_table_recompute_ip_support_for_apn
(
  ds_pdn_mgr_s  *rejected_pdn_cntx_p
);

#endif    /* DS_PDN_APN_TABLE_H */

