#ifndef DS_PDN_AUTH_MGR_H
#define DS_PDN_AUTH_MGR_H

/*===========================================================================
                      DS_PDN_AUTH_MGR.H
DESCRIPTION
This file exposes PDN auth manager through APIs and data structures.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/inc/ds_pdn_auth_mgr.h#1 $
  $DateTime: 2020/03/17 08:03:16 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "ds_pdn_defs.h"


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/



/*===========================================================================

                           FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION DS_PDN_AUTH_MGR_INIT

DESCRIPTION
  This function initializes the auth manager. It sets up the auth
  table (ds_3gpp_auth_fb_tbl).
 
PARAMETERS
  is_init          : Is it part of power up init

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  
===========================================================================*/
void 
ds_pdn_auth_mgr_init(boolean is_init);

/*===========================================================================
FUNCTION DS_PDN_AUTH_MGR_NEED_REDIAL

DESCRIPTION
  Queris the auth manager if a redial is required.
 
PARAMETERS   :
  apn                  - Apn for which the redial query is made
  requdested_auth_type - The auth type that the app originally
                         requested
  cause_code           - PS IFACE cause code specifying the reason for
                         prior termination
  subs_id              - Subscription Id
DEPENDENCIES 
  None

RETURN VALUE
  TRUE - if redialing is required
  FALSE - Otherwise

SIDE EFFECTS
  
===========================================================================*/
boolean
 ds_pdn_auth_mgr_need_redial

(
 char                          *apn,
 ds_profile_auth_enum_type      requested_auth_type,
 ds_net_down_reason_type        cause_code,
 boolean                        call_was_connected
);

/*===========================================================================
FUNCTION DS_PDN_AUTH_MGR_GET_AUTH_TYPE

DESCRIPTION
  Queries the auth manager for the appropriate auth_type to use for an
  outgoing call.
 
PARAMETERS   : 
  IN
  apn                  - Apn for which the redial query is made
  requdested_auth_type - The auth type that the app originally
                         requested
  subs_id              - Subscription Id
  OUT
  auth_type_to_use_p  - Pointer to the auth_type to be used

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/ 
void 
ds_pdn_auth_mgr_get_auth_type 
(
 const char                  *apn, 
 ds_profile_auth_enum_type    requested_auth_type, 
 ds_profile_auth_enum_type   *auth_type_to_use_p
);

/*===========================================================================
FUNCTION DS_PDN_AUTH_MGR_CALL_FAILURE

DESCRIPTION
  This function notifies the auth manager of a call failure event..
 
PARAMETERS   : 
  apn                  - Apn for which the redial query is made
  requdested_auth_type - The auth type that the app originally
                         requested
  subs_id              - Subscription Id

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/ 
void 
ds_pdn_auth_mgr_call_failure 
(
 char                          *apn, 
 ds_profile_auth_enum_type     requested_auth_type, 
 ds_net_down_reason_type       cause_code,
 boolean                       call_was_connected
);

/*===========================================================================
FUNCTION DS_PDN_AUTH_MGR_CALL_SUCCESS

DESCRIPTION
  This function notifies the auth manager of a call success event.
 
PARAMETERS   : 
  apn                  - Apn for which the redial query is made
  requdested_auth_type - The auth type that the app originally
                         requested
  subs_id              - Subscription Id 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/ 
void 
ds_pdn_auth_mgr_call_success 
(
 char                       *apn, 
 ds_profile_auth_enum_type   requested_auth_type
);

/*===========================================================================
  FUNCTION DS_PDN_AUTH_MGR_CLEAR_INFO

  DESCRIPTION
  This function performs resetting of the PDN auth module info.
  
  PARAMETERS  
  None. 
    
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
===========================================================================*/
void ds_pdn_auth_mgr_clear_info
(
  void
);

#endif /* DS_PDN_AUTH_MGR_H */

