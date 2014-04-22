#ifndef DS_PDN_REDIAL_HDLR_H
#define DS_PDN_REDIAL_HDLR_H

/*===========================================================================
                      DS_PDN_REDIAL_HDLR.H
DESCRIPTION
This file exposes PDN redial handler through APIs and data structures.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/inc/ds_pdn_redial_hdlr.h#1 $
  $DateTime: 2020/03/17 08:03:16 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ds_pdn_defs.h"
#include "ds_pdn_manager.h"
#include "ds_bearer_manager.h"
#include "ds_eps_pdn_manager.h"



/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================
FUNCTION DS_PDN_MGR_DUAL_IP_REDIAL_REQUIRED

DESCRIPTION 
  This function is used to determine whether to redialing based on dual IP cause
  codes. Cause codes are set in NV.

PARAMETERS
  down_reason   - Cause code(reason) returned by NW.
                  This is just the reason part of net_down_reason_type

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: Redial required
  FALSE: Not a dual IP redial cause code
   
SIDE EFFECTS 
  None.
===========================================================================*/
boolean ds_pdn_mgr_dual_ip_redial_required
(
  ds_pdn_mgr_s                *pdn_mgr_p,
  uint16                       down_reason
);

/*===========================================================================
FUNCTION DS_PDN_MGR_SETUP_REDIAL

DESCRIPTION
  This function checks with the 3gpp bearers if redialing is required.
  If so, sets up the pdn context for redial.
  Otherwise, it cleans up the bearer and returns

PARAMETERS   
  pdn_mgr_p         - pdn context that we are workin on
  bearer_mgr_p     - bearer context
  down_reason        - reason for bearer deactivation
    
DEPENDENCIES
  Should always be called from within a critical section 
  ds3gpp_global_crit_sect

RETURN VALUE
  None  
 
SIDE EFFECTS
  None
===========================================================================*/
void ds_pdn_mgr_setup_redial
( 
  ds_pdn_mgr_s            *pdn_mgr_p, 
  ds_bearer_mgr_s         *bearer_mgr_p,
  ds_net_down_reason_type  down_reason
);

#endif /* DS_PDN_REDIAL_HDLR_H */
