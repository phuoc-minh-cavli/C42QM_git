#include "uim_variation.h"
#ifndef GSTK_STATE_C
#define GSTK_STATE_C


/*===========================================================================


                           G S T K       S T A T E


GENERAL DESCRIPTION
  This source file contains function to handle GSTK main state machine.


FUNCTIONS
  gstk_process_state
    This function dispatches the command to the current state of the GSTK state
    machine.

  gstk_start_toolkit_session
    This function starts the toolkit session in the GSDI sign wait state or
    terminal profile wait state

  gstk_state_determine_action_after_gsdi_evt
    This function determine the state and the preferred toolkit slot based
    on the current toolkit_info mask

  gstk_state_non_idle_cleanup
    This function performs necessary clean up when state is not idle

INITIALIZATION AND SEQUENCING REQUIREMENTS

===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2003-2004,2009 - 2014, 2016 QUALCOMM Technologies, Inc (QTI) 
and its licensors. All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software is regulated by the U.S. Government. 
Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


$Header: //components/rel/uim.mpss/3.3.1/gstk/src/gstk_state.c#1 $ $DateTime: 2020/07/22 10:53:27 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/23/16   shr     Deprecate GSTK TAL
05/25/16   shr     F3 reduction: Malloc Failure, Memcpy, Null Ptr checks
05/11/16   vr      F3 reduction
02/29/16   gs      Removal of Dynamic TP Mechanism
10/07/14   shr     Memory optmization to the GSTK NV logic
08/27/14   vr      Move gstk_shared_data and gstk_instance_data to global
07/11/14   shr     Add Support for NV Refresh
07/11/14   bd      Enable retry for call control envelopes by default
06/03/14   sw      Implemented retry for call-control envelopes
01/16/14   hn      Replace MSG_* with UIM_MSG_* macros to save memory
01/13/12   nb      Fix compilation warning
12/20/11   nb      GSDI Cleanup
05/12/11   xz      Fix an issue of infinite loop while process envelope cmd
05/04/11   xz      Fix compile issue
05/03/11   xz      Add cleanup when state is not idle
02/22/11   js      Fix for async processing of TAL commands
02/07/11   dd      Fix dual slot define
08/30/10   xz      Fix compile warning
02/15/10   nb      Moved Dual Slot Changes
04/20/09   yb      Implemented a mechanism to notify client when TP DL fails
01/07/05   jk      Fixed Compiler warnings
03/05/04   tml     Allowed single slot to operate individually in Dual Slot
                   solution
09/04/03   tml     Dual slot support
05/16/03   tml     removed power down state function call, linted
02/07/03   tml     Initial Version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "gstk_state.h"
#include "gstk_term_profile.h"
#include "err.h"
#include "gstkutil.h"
#include "gstk_nv.h"
#include "uim_msg.h"

/*===========================================================================
FUNCTION GSTK_PROCESS_STATE

DESCRIPTION

  This function dispatches the command to the current state of the GSTK state
  machine.

PARAMETERS
  cmd_p: [Input] Pointer to the command to be processed by various the
                 GSTK state machine

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_process_state (
        gstk_cmd_type            * cmd_p )
{
  gstk_status_enum_type  gstk_status = GSTK_SUCCESS;

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  UIM_MSG_HIGH_2("gstk_process_state, State: 0x%x, Slot: 0x%x",
                 gstk_curr_inst_ptr->gstk_state,
                 gstk_curr_inst_ptr->slot_id);

  if(cmd_p == NULL)
  {
    return GSTK_BAD_PARAM;
  }

#ifdef FEATURE_MODEM_CONFIG_REFRESH
  if(cmd_p->general_cmd.message_header.command_group == GSTK_NV_REFRESH_CMD)
  {
    return gstk_nv_mcfg_process_refresh(cmd_p);
  }
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

  if (gstk_curr_inst_ptr->gstk_state != GSTK_IDLE_S)
  {
    gstk_state_non_idle_cleanup();
  }

  /* Send the command to the current state for processing */
  switch(gstk_curr_inst_ptr->gstk_state) {
  case GSTK_START_S: /* Start state */
      gstk_status = gstk_state_start(cmd_p);
      break;

  case GSTK_MMGSDI_SIG_WAIT_S: /* waiting for gsdi to signal term profile process state */
      gstk_status = gstk_state_gsdi_sig_wait(cmd_p);
      break;

  case GSTK_TERM_PROFILE_RSP_WAIT_S: /* waiting for response from uim in
                                        regards to term profile state */
      gstk_status = gstk_state_tp_rsp_wait(cmd_p);
      break;

  case GSTK_IDLE_S: /* idle state */
      gstk_status = gstk_state_idle(cmd_p);
      break;

  case GSTK_NO_SIM_S: /* no sim state */
      gstk_status = gstk_state_no_sim(cmd_p);
      break;

  case GSTK_POWER_DOWN_S: /* power down state */
      UIM_MSG_MED_0("Ignore cmd for pwr dn");
      break;
  default:
      UIM_MSG_MED_1("Unhandled state 0x%x", gstk_curr_inst_ptr->gstk_state);
      break;
  }
  return gstk_status;
}

/*===========================================================================
FUNCTION gstk_state_non_idle_cleanup

DESCRIPTION
  This function performs necessary clean up when state is not idle

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_state_non_idle_cleanup(void)
{
  GSTK_RETURN_IF_NULL_PTR(gstk_curr_inst_ptr);

  if (gstk_curr_inst_ptr->gstk_state != GSTK_TERM_PROFILE_RSP_WAIT_S
      &&
      gstk_curr_inst_ptr->is_tp_dl_done == FALSE)
  {
    /* set to TRUE because there won't be TP DL in progress */
    UIM_MSG_HIGH_0("reset is_tp_dl_done to TRUE");
    gstk_curr_inst_ptr->is_tp_dl_done = TRUE;
  }

  if (gstk_nv_get_feature_status(
        GSTK_CFG_FEATURE_SLOT_PROP_LOC_STATUS,
        gstk_curr_inst_ptr->slot_id) == TRUE)
  {
    UIM_MSG_HIGH_0("Clean attach ctrl");
    gstk_curr_inst_ptr->attach_ctrl_res = GSTK_ATTACH_CTRL_NOT_ALLOWED;
    gstk_util_send_attach_ctrl_res();
  }

  if (gstk_curr_inst_ptr->gstk_curr_envelope_info.gstk_env_state == GSTK_SUB_ST_PENDING_ENV_RSP_FROM_CARD)
  {
    gstk_util_clear_retry_envelope_data(
      gstk_util_get_sim_slot_id(gstk_curr_inst_ptr->client_id));
  }
} /* gstk_state_non_idle_cleanup */
#endif  /* GSTK_STATE_C */
/*lint -e750 local macro gstk_c not being used */


