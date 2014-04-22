/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                 U I M   G S M   S T A T E   M A C H I N E

GENERAL DESCRIPTION
  This module contains the state machine for the passive mode functionality
  of the UIM interface.

EXTERNALIZED FUNCTIONS

  uim_handle_cmd_rsp_timeout_sig_in_passive_mode
    handles the UIM_TRANSACTION_SIG signal

  uim_process_passive_mode_response
    Processes the responses for given command in passive mode.

  uim_is_passive_mode_allow
    check for passive mode is supported by UIM Server or not.

  uim_process_passive_mode_command
    This function contains the state machine that processes APDUs
    received in passive mode .

Copyright (c) 2016, 2018 by QUALCOMM Technologies, Inc.
All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/uimdrv/src/uimpassivemode.c#1 $
$DateTime: 2020/07/22 10:53:27 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/23/18   ku      Enabling Passive mode and remote client parallely
06/21/16   ks      Resetting the command transacted flag if the 
                   response is not received from card
06/08/16   ssr     Stub out passive mode functionality for IoE
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uimi.h"
#include "uimglobals.h"


/*  UIM_RESET_SWITCH_UIM_PASSIVE_F,         Power up making UIM passive */
static const uim_generic_state_type UIM_RST_SWITCH_UIM_PASSIVE_STATES[] =
{ UIM_POWER_UP_ST, UIM_RESET_ST,
  UIM_DELAY_AFTER_ATR_ST, UIM_PPS_ST, UIM_UPDATE_OP_PARAMS_ST,
#if defined( FEATURE_UIM_T_1_SUPPORT )
  UIM_IFS_NEG_ST,
#endif /* FEATURE_UIM_T_1_SUPPORT */
  UIM_DONE_ST
};

#ifdef FEATURE_UIM_BTSAP_CLIENT
/* POWER UP UIM due to BTSAP client connection request in PASSIVE mode */
static const uim_generic_state_type UIM_BTSAP_PASSVE_CLIENT_PUP_STATES[] =
{ UIM_POWER_DOWN_ST, UIM_POWER_UP_ST, UIM_RESET_ST,
  UIM_DONE_ST };
#endif



/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*===========================================================================
              F U N C T I O N   P R O T O T Y P E S
===========================================================================*/


/**
*  This function handles the UIM_TRANSACTION_SIG signal in passive mode.
*
*  SIDE EFFECTS
*    This function may change the value of *imask_ptr or *sig_mask_ptr.
*
*
*  @param sig_mask_ptr  Pointer to the OR's set of signals that UIM task needs to
*                       handle.  The value may be updated by this function.
*  @param imask_ptr Pointer to the mask of signals that the uim task is interested
*                   in handling.  Unused in this function.
*  @param uim_ptr Pointer to the current instance of the UIM global structure
*  @return void
*/
void uim_handle_cmd_rsp_timeout_sig_in_passive_mode(rex_sigs_type *sig_mask_ptr,
                                              rex_sigs_type *imask_ptr,
                                              uim_instance_global_type *uim_ptr)
{
  /* Set the flag to false since the command is completed */
  uim_ptr->flag.command_in_progress = FALSE;
  /* Setting the status to ERROR prior to invoking power down.
     While shutting the LDO down we evaluate the uim status to
     cater to the user customized LDO settings if needed. */
  uim_ptr->state.status = UIM_ERR_S;
  uim_power_down(uim_ptr);
  /* Clear out any votes to keep UIM powered on */
  uim_ptr->state.ctl = UIM_NONE;
  uim_ptr->flag.powerdown = TRUE;

  /* internal command timed-out for max attempts */
  /* Send the Error Report*/
  if(!uim_ptr->flag.static_buffer_used && uim_ptr->command.cmd_ptr != NULL)
  {
    /* reset the command transacted flag as response 
       is not succesfully received from card*/
    uim_ptr->command.rpt_buf.cmd_transacted = FALSE;

    uim_send_err_rpt(uim_ptr->command.cmd_ptr, uim_ptr);
  }

  /* flush all the commands in the command queue */
  uim_flush_command(UIM_FLUSH_ALL, uim_ptr);

  /* Notify the higher layers that UIM is declared faulty */
  uim_notify_error(UIM_CMD_TIMED_OUT_IN_PASSIVE_MODE_S, uim_ptr);

  /* Whenver we receive a command, send an error report as the
     SIM is to be powered down */
  (*imask_ptr) |= UIM_CMD_Q_SIG;

  /* reset the directory struct and power down to UIM */
  uim_reset_dir(uim_ptr);

  /* Reset the static buffer used flag */
  uim_ptr->flag.static_buffer_used = FALSE;

  /* Reset the static command count */
  uim_ptr->command.static_cmd_buf.hdr.cmd_count = 0;
} /* uim_handle_cmd_rsp_timeout_sig_in_passive_mode */


/*===========================================================================

FUNCTION UIM_PROCESS_PASSIVE_MODE_COMMAND

DESCRIPTION
  This procedure processes UIM request in passive mode. It sets the UIM
  state variable and calls the UIM state machine to process the command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  It sets the RUIM state variable amongst others.

===========================================================================*/
void uim_process_passive_mode_command
(
  uim_instance_global_type *uim_ptr
)
{
#ifdef FEATURE_UIM_USB_UICC
  if(uim_is_usb_enabled(uim_ptr) && uim_usb_is_usb_interface_selected(uim_ptr))
  {
    UIMDRV_MSG_HIGH_1(uim_ptr->id,  "Received reset to make UIM USB UICC passive 0x%x ",
                      uim_ptr->command.cmd_ptr->hdr.command );
    /* Traverse through the power off,on and get ATR states only */
    uim_ptr->command.generic_state_ptr = UIM_USB_RST_SWITCH_UIM_PASSIVE_STATES;
  }
  else
#endif /* FEATURE_UIM_USB_UICC */

#ifdef FEATURE_UIM_BTSAP_CLIENT
  if(IS_BTSAP_HANDLE_ACTIVE(uim_ptr))
  {
    UIMDRV_MSG_HIGH_1(uim_ptr->id,"Received reset to make BTSAP client passive 0x%x ",
                      uim_ptr->command.cmd_ptr->hdr.command );

    /* Set BTSAP handle to enable mask */
    uim_ptr->remote.btsap.handle = (UIM_BTSAP_ACTIVE_ENABLE_MASK);

    /* Traverse through the power off,on and get ATR states only */
    uim_ptr->command.generic_state_ptr = UIM_BTSAP_PASSVE_CLIENT_PUP_STATES;
  }
  else
#endif
  {
    UIMDRV_MSG_HIGH_1(uim_ptr->id,  "Received Reset to make UIM passive 0x%x ",
                      uim_ptr->command.cmd_ptr->hdr.command );
    /* Traverse through the atr and pps states only */
    uim_ptr->command.generic_state_ptr = UIM_RST_SWITCH_UIM_PASSIVE_STATES;
  }
  /* The status of UIM is set to Uninitialized since reset the UIM */
  uim_ptr->state.status = UIM_UNINITIALIZED_S;
  /* Clean out the last selected directory as we will reset the card. */
  uim_reset_dir(uim_ptr);
  /* !!!!! Do not clear the ATCSIM directory structure !!!!! */
} /* uim_process_passive_mode_command */


/*===========================================================================

FUNCTION UIM_PROCESS_PASSIVE_MODE_RESPONSE

DESCRIPTION
  This procedure processes the response that has been recd from the UIM
  at the completion of the command processing and releases the command
  buffer for a generic command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Releases the command buffer.

===========================================================================*/
void uim_process_passive_mode_response
(
  uim_rsp_buf_type const   *rsp_ptr,
  uim_instance_global_type *uim_ptr
)
{
  /* Report back the status as success */
  if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
  {
    if (uim_ptr->command.cmd_ptr->hdr.command == UIM_POWER_UP_UIM_PASSIVE_F)
    {
      uim_ptr->command.rpt_buf.rpt_type = UIM_POWER_UP_UIM_PASSIVE_R;
    }
    else
    {
      uim_ptr->command.rpt_buf.rpt_type = UIM_RESET_SWITCH_UIM_PASSIVE_R;
    }
    /* Fill in necessary fields of command block */
    uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

    uim_ptr->command.rpt_buf.rpt.rst_uim_passive.num_bytes =
    uim_ptr->atr.atr_buf.num_bytes;
    uim_memscpy(uim_ptr->command.rpt_buf.rpt.rst_uim_passive.data,
               sizeof(uim_ptr->command.rpt_buf.rpt.rst_uim_passive.data),
               uim_ptr->atr.atr_buf.data,
               uim_ptr->atr.atr_buf.num_bytes);

    /* Make sure that we have switched the UIM mode to passive */
    uim_ptr->command.mode = UIM_PASSIVE_MODE;

    /* Set clock stop allowed in passive mode flag to FALSE */
    uim_ptr->flag.passive_clk_stop_allowed = FALSE;

    /* Flush all the existing commands to empty the queue
    */
    uim_flush_command(UIM_FLUSH_ALL, uim_ptr);

    /* Report to requesting task */
    (*(uim_ptr->command.cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
  } /* end if - is the report function defined. */
}/* uim_process_passive_mode_response */


/*===========================================================================

FUNCTION UIM_PASSIVE_MODE_IS_NOT_ALLOWED

DESCRIPTION
  passive mode is supported by the UIM Server or not 
DEPENDENCIES
  None

RETURN VALUE
  Boolean:
    TRUE: Passive mode is not supported by UIM Server
    FALSE: Passive mode is supported by UIM Server

SIDE EFFECTS
  Releases the command buffer.

===========================================================================*/
boolean uim_passive_mode_is_not_allowed 
(
  uim_instance_global_type *uim_ptr
)
{
  return FALSE;
} /* uim_passive_mode_is_not_allowed */

