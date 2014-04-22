#ifndef DS_PDN_CLAT_HDLR_H
#define DS_PDN_CLAT_HDLR_H

/*===========================================================================
                      DS_PDN_CLAT_HDLR.H
DESCRIPTION
This file exposes PDN clat handler through APIs and data structures.

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: 
  $DateTime:

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/24/17    aks      First checkin
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "ds_pdn_defs.h"
#ifdef FEATURE_DATA_PS_464XLAT
#include "ps_clat_sm.h"
#endif /* #ifdef FEATURE_DATA_PS_464XLAT */




typedef enum
{ 
  DS_PDN_CLAT_MIN,
  DS_PDN_CLAT_DOWN = DS_PDN_CLAT_MIN,
  DS_PDN_CLAT_COMING_UP,
  DS_PDN_CLAT_UP,
  DS_PDN_CLAT_GOING_DOWN,
  DS_PDN_CLAT_MAX_STATE
} ds_pdn_clat_sm_state;


/*===========================================================================
FUNCTION ds_pdn_mgr_is_prof_clat_enabled

DESCRIPTION
  

PARAMETERS
  prof_num : Profile id

DEPENDENCIES
  None.

RETURN VALUE
  None
 
SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdnmgr_is_prof_clat_enabled(uint8           prof_num);

/*===========================================================================
FUNCTION ds_pdn_create_clat_sm

DESCRIPTION
  

PARAMETERS
  client_data_ptr: Current active PS subs

DEPENDENCIES
  None.

RETURN VALUE
  None
 
SIDE EFFECTS
  None.
===========================================================================*/
boolean  ds_pdn_create_clat_sm(ds_pdn_mgr_s *pdn_ptr);

/*===========================================================================
FUNCTION ds_pdn_clat_sm_transition_state

DESCRIPTION

PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE
  None
 
SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_clat_sm_transition_state
(
  ds_pdn_mgr_s              *pdn_mgr_p,          
  ds_pdn_clat_sm_state      new_state
);

/*===========================================================================
FUNCTION ds_pdn_get_clat_state

DESCRIPTION

PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE
  None
 
SIDE EFFECTS
  None.
===========================================================================*/
ds_pdn_clat_sm_state ds_pdn_get_clat_state
(
  ds_pdn_mgr_s        *pdn_mgr_p
);

/*===========================================================================
FUNCTION ds_pdn_post_clat_sm_creation

DESCRIPTION

PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE
  None
 
SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_post_clat_sm_creation
(
  ds_pdn_mgr_s            *pdn_mgr_p
);

/*===========================================================================
FUNCTION ds_pdn_post_clat_sm_create_cmd_hdlr

DESCRIPTION

PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE
  None
 
SIDE EFFECTS
  None.
===========================================================================*/

void ds_pdn_post_clat_sm_create_cmd_hdlr
(
  ds_pdn_mgr_s            *pdn_mgr_p
);

/*===========================================================================
FUNCTION ds_pdn_teardown_clat_sm

DESCRIPTION

PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE
  None
 
SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_teardown_clat_sm
(
  ds_pdn_mgr_s *pdn_mgr_p
);

#ifdef FEATURE_DATA_PS_464XLAT
/*===========================================================================
FUNCTION ds_pdn_handle_clat_ev_cback

DESCRIPTION

PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE
  None
 
SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_handle_clat_ev_cback
(
  ps_clat_sm_type          *clat_sm_inst,
  clat_sm_indication_type  indication,
  void                    *user_data
);
#endif /*#ifdef FEATURE_DATA_PS_464XLAT*/

/*===========================================================================
FUNCTION ds_pdn_free_clat_sm

DESCRIPTION

PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE
  None
 
SIDE EFFECTS
  None.
===========================================================================*/
void  ds_pdn_free_clat_sm
(
  ds_pdn_mgr_s *pdn_ptr
);

#endif /* DS_PDN_CLAT_HDLR_H */

