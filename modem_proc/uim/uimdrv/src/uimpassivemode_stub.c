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

Copyright (c) 2016 by QUALCOMM Technologies, Inc.
All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/uimdrv/src/uimpassivemode_stub.c#1 $
$DateTime: 2020/07/22 10:53:27 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/15/16   dd      Fix compilation issue
06/08/16   ssr     Stub out passive mode functionality for IoE
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uimi.h"
#include "uimglobals.h"
#include "uimdrv_msg.h"

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
  UIMDRV_MSG_ERR_0(uim_ptr->id,"passive mode is disabled");
  (void) imask_ptr;
  (void) sig_mask_ptr;
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
  UIMDRV_MSG_ERR_0(uim_ptr->id,"passive mode is disabled");
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
  UIMDRV_MSG_ERR_0(uim_ptr->id,"passive mode is disabled");
  (void) rsp_ptr;
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

  if (uim_ptr->command.cmd_ptr->hdr.command  == UIM_POWER_UP_UIM_PASSIVE_F ||     
      uim_ptr->command.cmd_ptr->hdr.command  == UIM_RESET_SWITCH_UIM_PASSIVE_F)
  {
    return TRUE;
  }
  return FALSE;
}/* uim_passive_mode_is_not_allowed*/

