#ifndef DS_PDN_APN_SWITCH_MGR_H
#define DS_PDN_APN_SWITCH_MGR_H

/*===========================================================================
                      DS_PDN_APN_SWITCH_MGR.H
DESCRIPTION
This file exposes APN switch manager through APIs and data structures.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/inc/ds_pdn_apn_switch_mgr.h#1 $
  $DateTime: 2020/03/17 08:03:16 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/
#include "ds_pdn_defs.h"
#include "ds_pdn_apm.h"
#include "ds_pdn_manager.h"
#include "queue.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
typedef struct
{
  boolean                          payload_valid;
  ds_pdn_apm_ind_type              apm_ind;
  ds_pdn_apm_ind_pay_load          apm_ind_payload;
}ds_pdn_apn_switch_mgr_payload_info_type;


/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION       DS_PDN_APN_SWITCH_MGR_RESET_SWITCHING_ACTION_PAYLOAD

DESCRIPTION    This function resets the switching action payload per subs
 
PARAMETERS     NONE
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_pdn_apn_switch_mgr_reset_switching_action_payload
(
  void
);

/*===========================================================================
FUNCTION       DS_PDN_APN_SWITCH_MGR_SEND_IND_TO_APM

DESCRIPTION    This function retrieves the switching action payload 
                         and indicates it to APM 
 
PARAMETERS     None.
 
DEPENDENCIES   NONE

RETURN VALUE   ds_pdn_apm_ext_error_code: Error code after posting ind to APM

SIDE EFFECTS   NONE
===========================================================================*/
ds_pdn_apm_ext_error_code ds_pdn_apn_switch_mgr_send_ind_to_apm
(
  void
);

/*===========================================================================
FUNCTION       DS_PDN_APN_SWITCH_MGR_POPULATE_SWITCHING_PAYLOAD_PER_RAT

DESCRIPTION    This function extracts APM rule block entry for a given 
               profile ID and compares the cause code with APN Switching
               cause codes to decide APN Switch is needed or not
 
PARAMETERS     bearer_call_mode: Call mode of the bearer 
               cause_type: DETACH/REJECT NETWORK cause type
               profile_id: Profile of the PDN detached or rejected
               PDN State: State of PDN Context
               call_info_ptr: Call info passed by lower layers
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_pdn_apn_switch_mgr_populate_switching_payload_per_rat
(
  sys_sys_mode_e_type                  bearer_call_mode,
  uint16                               cause_type,
  uint8                                profile_id,
  ds_pdn_mgr_state_e                   pdn_state,
  void                                *call_info_ptr
);

#endif /* DS_PDN_APN_SWITCH_MGR_H*/
