#ifndef DS_PDN_THROT_SPEC_H
#define DS_PDN_THROT_SPEC_H

/*===========================================================================
                      DS_PDN_THROT_SPEC.H
DESCRIPTION
This file exposes PDN spec throttling through APIs and data structures.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/inc/ds_3gpp_pdn_throt_spec.h#2 $
  $DateTime: 2022/08/08 06:31:48 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "ds_pdn_defs.h"
#include "ds_eps_pdn_manager.h"
#include "ds_3gpp_pdn_throttle_sm.h"


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*-------------------------------------------------------------------------- 
  This structure maintains the parsed back-off timer information
-------------------------------------------------------------------------*/
typedef struct
{
  boolean                    retry_on_rat_change;
  boolean                    retry_on_proc_change;
  boolean                    retry_on_eplmns;
  uint32                     timer_count;
  boolean                    timer_is_active;  
} ds_pdn_throt_spec_parsed_backoff_info_s;

/*===========================================================================
FUNCTION DS_PDN_THROT_SPEC_UNBLOCK_ALL_APN_GLOBALLY_BLKED_APN

DESCRIPTION
  This function processes cmd to unblock all the globally blocked APN
  
PARAMETERS   : 
  void
 
DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_throt_spec_unblock_esm_backoff_throttling
(
  void
);

/*===========================================================================
FUNCTION DS_PDN_THROT_SPEC_TIMESTAMP_APN_THROT_TMR_CMD_HDLR

DESCRIPTION
  This function timestamps the APN throttle timer event in EFS
  
PARAMETERS   : 
  throt_sm_p      - pointer to core Throt state machine
 
DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_throt_spec_timestamp_apn_throt_tmr_cmd_hdlr
(
  void*                  throt_sm_p
);

/*===========================================================================
FUNCTION DS_PDN_THROT_SPEC_IMSI_INFO_AVAIL_CMD_HDLR

DESCRIPTION
  This function checks to see if the IMSI info matches the one stored in EFS.
  If yes, powerup global APN throttling is performed. Else, it has already
  been done, so nothing is done in this function.
 
PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_throt_spec_imsi_info_avail_cmd_hdlr
(
  void
);

/*===========================================================================
FUNCTION DS_PDN_THROT_SPEC_INIT

DESCRIPTION
  This function does the powerup initialization for the APN throttle
  functionality.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_throt_spec_init
(
  void
);

/*===========================================================================
FUNCTION      DS_PDN_THROT_SPEC_CLEAR_T3396_TMR_ENTRY

DESCRIPTION   This function resets the t3396 timer entry from local cache and 
              also the EFS file.
 
PARAMETERS 
apn - apn name for which it was received 
plmn_id   - plmn passed by the client

RETURN VALUE 
void 

SIDE EFFECTS  None
===========================================================================*/
void ds_pdn_throt_spec_clear_t3396_tmr_entry 
(
  byte                      *apn,
  sys_plmn_id_s_type         plmn_id
);

/*===========================================================================
FUNCTION DS_PDN_THROT_SPEC_SAMPLING_TIMER_EXP_CMD_HDLR 
 
DESCRIPTION
  This function handles the sampling timer expiration command.
  
PARAMETERS   : 
  void

DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_throt_spec_sampling_timer_exp_cmd_hdlr
(
  void
);

/*===========================================================================
FUNCTION DS_PDN_THROT_SPEC_LTE_HANDLE_NAS_REJECT_WITH_T3396_IE 
 
DESCRIPTION
  This function handles the reject message received from NAS with a t3396 IE
  based on Release 11 requirements

PARAMETERS   : 
  cm_lte_call_info_s_type              *lte_call_info,
  ds_pdn_context_s                     *pdn_context_p,
  ps_iface_net_down_reason_type        down_reason

DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_throt_spec_lte_handle_nas_reject_with_t3396_ie
(
  ds_bearer_deact_params_s   *lte_call_info,
  ds_pdn_mgr_s                   *pdn_context_p,
  ds_net_down_reason_type         down_reason
);

/*===========================================================================
FUNCTION DS_PDN_THROT_SPEC_UNBLOCK_APN_TIMER

DESCRIPTION
  This function unblocks apn timer 
 
PARAMETERS   : 
  throt_sm_p
 
DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_throt_spec_unblock_apn_timer
(
  void                       *throt_sm_p
);

/*===========================================================================
FUNCTION      DS_3GPP_SPEC_T3396_BACKOFF_CAUSE_CODE

DESCRIPTION 
  This function checks whether T3396 related backoff handling may be
  required for the given PS Net Down Reason

PARAMETERS 
  down_reason - DS Net Down Reason

RETURN VALUE 
  TRUE,  if T3396 backoff is required
  FALSE, otherwise

SIDE EFFECTS 
  None
===========================================================================*/
boolean ds_pdn_throt_spec_t3396_backoff_cause_code
(
  ds_net_down_reason_type   down_reason
);

/*===========================================================================
FUNCTION      DS_PDN_THROT_SPEC_HANDLING_IS_REQUIRED

DESCRIPTION 
  This function checks whether Spec handling is required for the given
  DS Net Down Reason

PARAMETERS 
  down_reason - DS Net Down Reason

RETURN VALUE 
  TRUE,  if T3396 backoff is required
  FALSE, otherwise

SIDE EFFECTS 
  None
===========================================================================*/
boolean ds_pdn_throt_spec_handling_is_required
(
  ds_bearer_deact_params_s *mode_info_p,
  ds_net_down_reason_type   down_reason
);

/*===========================================================================
FUNCTION      DS_PDN_THROT_SPEC_POWER_UP_INIT

DESCRIPTION 
  This function performs power up initialization of the PDN Throt Spec module

PARAMETERS 
 None

RETURN VALUE 
  None
 
SIDE EFFECTS 
  None
===========================================================================*/
void ds_pdn_throt_spec_power_up_init
(
  void 
);

/*===========================================================================
FUNCTION DS_PDN_THROT_SPEC_CLEAR_T3396_ENTRY

DESCRIPTION
  This function clears all T3396 entries

PARAMETERS
  Clear thriottling boolean

DEPENDENCIES
  void

RETURN VALUE  
  void

SIDE EFFECTS

===========================================================================*/
void ds_pdn_throt_spec_clear_t3396_entry
(
  boolean                clear_esm26_throttle,
  boolean                clear_throttling
);

/*===========================================================================
FUNCTION DS_PDN_THROT_WRITE_THROT_TIMER_TO_EFS
 
DESCRIPTION
  This function wites the throttling timer value to efs location.
  
PARAMETERS   : 
  void

DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_throt_write_throt_timer_to_efs
(
  void
);

/*===========================================================================
FUNCTION DS_PDN_THROT_SPEC_POWERUP_THROT_HDLR

DESCRIPTION
  This function does the powerup throttling of APNs which were globally
  blocked before last UE power down.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
 void ds_pdn_throt_spec_powerup_throt_hdlr
(
  void
);
/*===========================================================================
FUNCTION DS_PDN_PSM_CALCULATE_REMAINING_THROTTLING_TIME
 
DESCRIPTION
  This function calculate the remaing throttling timer post PSM
  
PARAMETERS   : 
  void

DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_psm_calculate_remaining_throttling_time();
#endif /* DS_PDN_THROT_SPEC_H */
