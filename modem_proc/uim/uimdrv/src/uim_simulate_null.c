/*============================================================================
  FILE:         uim_simulate_null.c

  OVERVIEW:
   The file contains the simulate NULL related APIs.

  EXTERNALIZED FUNCTIONS:
    uim_switch_slot_mapping
    Api to switch logical to physical slot mapping.

    uim_get_logical_to_physical_mapping
    Api to query the current slot mapping

  DEPENDENCIES: N/A

                Copyright (c) 2016 QUALCOMM Technologies, Inc(QTI).
                All Rights Reserved.
                QUALCOMM Technologies Confidential and Proprietary
============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

$Header: //components/rel/uim.mpss/3.3.1/uimdrv/src/uim_simulate_null.c#1 $
$DateTime: 2020/07/22 10:53:27 $
$Author: pwbldsvc $

when          who        what, where, why
--------      ----       ---------------------------------------------------------
06/07/16      gm         Initial Revision
============================================================================*/
#include "uimglobals.h"
#include "uim_simulate_null.h"
#include "uim_common_efs.h"
#include "uimdrv_msg.h"


/*===========================================================================

FUNCTION UIM_SIMULATE_NULL_ADD_STATE

DESCRIPTION
  This function add extra state to uim command,

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_simulate_null_add_state
(
  uim_instance_global_type *uim_ptr
)
{
  UIMDRV_MSG_HIGH_2(uim_ptr->id,  "Received Stream 7816 APDU 0x%x command with timer = 0x%x s",
                    uim_ptr->debug.uim_feature_busy_response_simulate_null.uim_simulate_null_ins,
                    uim_ptr->command.cmd_ptr->stream_iso7816_apdu.cmd_data[UIM_CASE2_7816_APDU_SIZE] );
  uim_ptr->mode.generic_states[0] = UIM_STREAM_APDU_ST;
  uim_ptr->mode.generic_states[1] = UIM_SIMULATE_NULL_ST;
  uim_ptr->mode.generic_states[2] = UIM_DONE_ST;
  
  /* Set the global value for the uim_simulate_null state machine */
  uim_ptr->debug.uim_simulate_null_timer_val_s = uim_ptr->command.cmd_ptr->stream_iso7816_apdu.cmd_data[UIM_CASE2_7816_APDU_SIZE];
  uim_ptr->command.generic_state_ptr           = uim_ptr->mode.generic_states;
}


/*===========================================================================

FUNCTION uim_simulate_read_from_efs

DESCRIPTION
  This function read data from efs for simulate NULL

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_simulate_null_read_from_efs
(
  uim_instance_global_type *uim_ptr
)
{
  /* Read the UIM_FEATURE_BUSY_RESPONSE_SIMULATE_NULL_FILE from EFS */
  if (UIM_COMMON_EFS_SUCCESS == uim_common_efs_read(UIM_COMMON_EFS_UIMDRV_FEATURE_BUSY_RESPONSE_SIMULATE_NULL,
                                                    UIM_COMMON_EFS_ITEM_FILE_TYPE,
                                                    UIM_COMMON_EFS_DEVICE,
                                                   (uint8 *)&(uim_ptr->debug.uim_feature_busy_response_simulate_null),
                                                    sizeof(uim_ptr->debug.uim_feature_busy_response_simulate_null)))
  {
    UIMDRV_MSG_HIGH_0(uim_ptr->id, "Successfully read the UIM_FEATURE_BUSY_RESPONSE_SIMULATE_NULL_FILE from EFS");
  }
  else
  {
    /* simulate null timer is disabled by default */
    uim_ptr->debug.uim_feature_busy_response_simulate_null.uim_disable_simulate_null = TRUE;
    uim_ptr->debug.uim_feature_busy_response_simulate_null.uim_simulate_null_ins = UIM_SIMULATE_NULL_INS_DEFAULT;
    uim_common_efs_write(UIM_COMMON_EFS_UIMDRV_FEATURE_BUSY_RESPONSE_SIMULATE_NULL,
                         UIM_COMMON_EFS_ITEM_FILE_TYPE,
                         UIM_COMMON_EFS_DEVICE,
                        (char *)&(uim_ptr->debug.uim_feature_busy_response_simulate_null),
                         sizeof(uim_ptr->debug.uim_feature_busy_response_simulate_null));
    UIMDRV_MSG_HIGH_0(uim_ptr->id, "Failed to read UIM_FEATURE_BUSY_RESPONSE_SIMULATE_NULL_FILE, force set to defaults");
  }
}


/*===========================================================================

FUNCTION UIM_SIMULATE_NULL_TIMER_STATE_HANDLER

DESCRIPTION
  This function handles actions and change of state of uim_simulate_null_timer,
  when some event happens. This function is only for SIM BUSY Feature testing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_simulate_null_timer_state_handler
(
  uim_instance_global_type *uim_ptr,
  uim_busy_response_timer_event_type simulate_null_timer_event
)
{
  if ( NULL == uim_ptr ) {
    return;
  }

  switch (uim_ptr->card_cmd.uim_simulate_null_timer_state)
  {
    case UIM_BUSY_RESPONSE_TIMER_NOT_USED:
      if ( simulate_null_timer_event == UIM_BUSY_RESPONSE_EVT_NV_INIT &&
           uim_ptr->debug.uim_feature_busy_response_simulate_null.uim_disable_simulate_null == FALSE )
      {
        /* Called in uim_nv_init() if any of Busy respone timer in use */
        /* Define Timer */
        timer_def(&uim_ptr->card_cmd.uim_simulate_null_timer,
                  &uim_ptr->task.always_on_timer_group,
                  uim_ptr->tcb_ptr,
                  UIM_SIMULATE_NULL_TIMER_EXP_SIG, 0, 0);
        /* Set the timer state to not started to indicate the timer is defined but not start yet */
        uim_ptr->card_cmd.uim_simulate_null_timer_state = UIM_BUSY_RESPONSE_TIMER_NOT_STARTED;
      }
      break;
    case UIM_BUSY_RESPONSE_TIMER_NOT_STARTED:
      if (simulate_null_timer_event == UIM_BUSY_RESPONSE_EVT_CMD_RSP_SIG &&
           uim_ptr->debug.uim_simulate_null_timer_val_s > 0)
      {
        /* Called in uim_generic_command() UIM_SIMULATION_NULL_ST processing */
        /* Start timer */
        timer_set( &uim_ptr->card_cmd.uim_simulate_null_timer,
                   uim_ptr->debug.uim_simulate_null_timer_val_s,
                   0, T_SEC );

        /* Set the timer state to active to indicate the timer is running */
        uim_ptr->card_cmd.uim_simulate_null_timer_state = UIM_BUSY_RESPONSE_TIMER_ACTIVE;
      }
      break;
    case UIM_BUSY_RESPONSE_TIMER_ACTIVE:
      if ( simulate_null_timer_event == UIM_BUSY_RESPONSE_EVT_SIMULATE_NULL_TIMER_EXP_SIG )
      {
        /* Called in UIM_SIMULATE_NULL_TIMER_EXP_SIG handling in uim_task_common() */
        /* clear UIM_SIMULATE_NULL_TIMER_EXP_SIG */
        (void) rex_clr_sigs( uim_ptr->tcb_ptr, UIM_SIMULATE_NULL_TIMER_EXP_SIG );
        /* Handle busy response timers when get simulated response after "YY" sec of NULL byte */
        uim_busy_ind_timer_state_handler(uim_ptr, UIM_BUSY_RESPONSE_EVT_CMD_RSP_SIG);
        uim_trans_timer_state_handler(uim_ptr, UIM_BUSY_RESPONSE_EVT_CMD_RSP_SIG);
        /* Set CMD_RSP_SIG to trigger generic command state machine to move state to UIM_DONE_ST */
        (void) rex_set_sigs( uim_ptr->tcb_ptr, UIM_CMD_RSP_SIG );
        /* Clear uim_simulate_null_timer */
        rex_clr_timer(&uim_ptr->card_cmd.uim_simulate_null_timer);
        /* Set the timer state to active to indicate the timer is defined but not start yet */
        uim_ptr->card_cmd.uim_simulate_null_timer_state = UIM_BUSY_RESPONSE_TIMER_NOT_STARTED;
      }
      else if ( simulate_null_timer_event ==  UIM_BUSY_RESPONSE_EVT_POWER_DOWN )
      {
        /* Called in notify_error() */
        /* clear UIM_SIMULATE_NULL_TIMER_EXP_SIG, in case power down and timeout at the same time */
        (void) rex_clr_sigs( uim_ptr->tcb_ptr, UIM_SIMULATE_NULL_TIMER_EXP_SIG );
        /* Clear simulate_null_timer */
        rex_clr_timer(&uim_ptr->card_cmd.uim_simulate_null_timer);
        /* Set the timer state to active to indicate the timer is defined but not start yet */
        uim_ptr->card_cmd.uim_simulate_null_timer_state = UIM_BUSY_RESPONSE_TIMER_NOT_STARTED;
      }
      break;
    default:
      break;
  }
}/* uim_simulate_null_timer_state_handler */

