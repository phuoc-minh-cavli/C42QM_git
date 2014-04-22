/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                     G S T K   L I B R A R Y   D I S P L A Y


GENERAL DESCRIPTION

  This source file contains the library functions for display related Envelopes that are exposed to GSTK
  client as well as internal GSTK clients.


                        COPYRIGHT INFORMATION

Copyright (c) 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/gstk/src/gstklib_display.c#1 $ $DateTime: 2020/07/22 10:53:27 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/15/16   gs      Changes in TP based on RAT and App Type
06/23/16   gs      Initial Version

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "gstk.h"
#include "gstkutil.h"
#include "uim_msg.h"

/*===========================================================================
FUNCTION GSTK_SEND_ENVELOPE_MENU_SEL_COMMAND

DESCRIPTION
  This function sends the menu selection envelope command down to SIM/USIM.
  Clients need to provide the item identifier for the user selected item.

PARAMETERS
  client_id: [Input] Client id of the calling client.  This should be the value
                     returned from the gstk_slot_client_init_ext()
  user_data: [Input] User data that client wants GSTK to keep track of
  item_chosen: [Input] User selected item.
                       Valid value [0x01, 0xFF],
                       0x00 is used to indicate a null item identifier
  help_requested: [Input] Optional data field in the envelope command:
                          TRUE -> help is requested
                          FALSE -> help is not requested and should not
                                   be sent down to the SIM/USIM

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type:
    GSTK_SUCCESS,
    GSTK_ERROR,
    GSTK_CLIENT_NOT_REGISTERED,
    GSTK_MEMORY_ERROR,
    GSTK_NULL_INPUT_PARAM,

COMMENTS
  This function should only be called after gstk_slot_client_init_ext has been called
  Any of the following values will be returned when gstk processes the command after
  dequeuing from the command queue.
  gstk_status_enum_type:
    GSTK_SUCCESS,
    GSTK_ERROR,
    GSTK_CLIENT_NOT_REGISTERED,
    GSTK_MEMORY_ERROR,
    GSTK_BAD_PARAM,
    GSTK_NULL_INPUT_PARAM,
    GSTK_PARAM_EXCEED_BOUNDARY,
    GSTK_INVALID_LENGTH,
    GSTK_UNSUPPORTED_COMMAND,
    GSTK_UNKNOWN_COMMAND,
    GSTK_INSUFFICIENT_INFO,
    GSTK_NOT_INIT

SIDE EFFECTS
  Upon calling this function, client should expect to receive
  gstk_general_envelope_rsp_enum_type in its callback function.

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_send_envelope_menu_sel_command (
        gstk_client_id_type            client_id,
        uint32                         user_data,
        uint8                          item_chosen,
        boolean                        help_requested )
{
  return gstk_send_envelope_menu_sel_command_ext(client_id,
                                                 user_data,
                                                 item_chosen,
                                                 help_requested,
                                                 NULL);
} /* gstk_send_envelope_menu_sel_command */

/*===========================================================================
FUNCTION GSTK_SEND_ENVELOPE_MENU_SEL_COMMAND_EXT

DESCRIPTION

  This function sends the menu selection envelope command down to SIM/USIM.
  Clients need to provide the item identifier for the user selected item.

PARAMETERS
  client_id     : [Input] Client id of the calling client. This should be the value
                          returned from the gstk_slot_client_init_ext()
  user_data     : [Input] User data that client wants GSTK to keep track of
  item_chosen   : [Input] User selected item.
                          Valid value [0x01, 0xFF],
                          0x00 is used to indicate a null item identifier
  help_requested: [Input] Optional data field in the envelope command:
                          TRUE -> help is requested
                          FALSE -> help is not requested and should not
                                   be sent down to the SIM/USIM
  gstk_raw_env_cb:[Input] Callback function

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type:
    GSTK_SUCCESS,
    GSTK_ERROR,
    GSTK_CLIENT_NOT_REGISTERED,
    GSTK_MEMORY_ERROR,
    GSTK_NULL_INPUT_PARAM,

COMMENTS
  This function should only be called after gstk_slot_client_init_ext has been called
  Any of the following values will be returned when gstk processes the command after
  dequeuing from the command queue.
  gstk_status_enum_type:
    GSTK_SUCCESS,
    GSTK_ERROR,
    GSTK_CLIENT_NOT_REGISTERED,
    GSTK_MEMORY_ERROR,
    GSTK_BAD_PARAM,
    GSTK_NULL_INPUT_PARAM,
    GSTK_PARAM_EXCEED_BOUNDARY,
    GSTK_INVALID_LENGTH,
    GSTK_UNSUPPORTED_COMMAND,
    GSTK_UNKNOWN_COMMAND,
    GSTK_INSUFFICIENT_INFO,
    GSTK_NOT_INIT

SIDE EFFECTS
  Upon calling this function, client should expect to receive
  GSTK_MENU_IND_RSP in its callback function.

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_send_envelope_menu_sel_command_ext (
        gstk_client_id_type            client_id,
        uint32                         user_data,
        uint8                          item_chosen,
        boolean                        help_requested,
        gstk_evt_cb_funct_type         gstk_raw_env_cb)
{
  gstk_status_enum_type   gstk_status = GSTK_SUCCESS;
  gstk_task_cmd_type    * task_cmd    = NULL;
  uint32                  i           = 0;
#ifdef FEATURE_ESTK_SETUP_MENU_INTERFACE
 estk_cmd_type            *estk_cmd_ptr = NULL;
#endif /*FEATURE_ESTK_SETUP_MENU_INTERFACE*/

  UIM_MSG_HIGH_2("GSTK client send menu selection extension Extension, 0x%x 0x%x",
                 item_chosen, help_requested);

  /* validate client id */
  if (!gstk_util_is_valid_client_id(client_id))
  {
    /* it is not gstk calling this function either */
    UIM_MSG_ERR_1("Client 0x%x not registered", client_id);
    return GSTK_CLIENT_NOT_REGISTERED;
  }

  /* check if another menu selection message is already in progress */
  for (i = 0; i < GSTK_MAX_ENVELOPE_COMMAND; i++)
  {
    if ((gstk_shared_data.envelope_cmd_ref_table[i].is_free == FALSE) &&
        (gstk_shared_data.envelope_cmd_ref_table[i].expected_cmd_rsp_type == GSTK_MENU_IND_RSP))
    {
      UIM_MSG_ERR_1("GSTK Menu Selection message already in progress. Client ID: 0x%x",
                    client_id);
      return GSTK_ERROR;
    }
  }

#ifdef FEATURE_ESTK_SETUP_MENU_INTERFACE
  if (gstk_shared_data.gstk_client_table[client_id - 1].client_type !=
      GSTK_ESTK_TYPE)
  {
    if(GSTK_IDLE_S_RECOVERY(GSTK_SLOT_1))
    {
      UIM_MSG_ERR_0("gstk_send_envelope_menu_sel_command_ext: GSTK_IDLE_S_RECOVERY");
      return GSTK_ERROR;
    }

    estk_cmd_ptr = estk_get_cmd_buf();
    if (estk_cmd_ptr != NULL)
    {
      estk_cmd_ptr->hdr.cli_user_data  = user_data;
      estk_cmd_ptr->hdr.cmd            = ESTK_MENU_SEL_REQ_CMD;
      estk_cmd_ptr->hdr.cmd_ref_id     = 0;     /*doesn't matter*/
      estk_cmd_ptr->hdr.command_number = 0;     /*doesn't matter*/
      estk_cmd_ptr->hdr.sim_slot_id = GSTK_SLOT_1; /* doesn't matter */
      estk_cmd_ptr->cmd_data.menu_sel_req.chosen_item_id = item_chosen;
      estk_cmd_ptr->cmd_data.menu_sel_req.request_help   = help_requested;
      estk_cmd_ptr->estk_raw_evt_cb                      = gstk_raw_env_cb;
      estk_put_cmd(estk_cmd_ptr);
      return GSTK_SUCCESS;
    }
    else
    {
      UIM_MSG_ERR_0("failed to get estk cmd buf!");
      return GSTK_ERROR;
    }
  }
#endif /*FEATURE_ESTK_SETUP_MENU_INTERFACE*/

  /* get command buf */
  task_cmd = gstk_task_get_cmd_buf();
  if(task_cmd != NULL)
  {
    /* successfully obtained command buffer - build the envelope command */
    task_cmd->cmd.menu_selection_envelope_ind.message_header.client_id     = client_id;
    task_cmd->cmd.menu_selection_envelope_ind.message_header.command_group = GSTK_ENVELOPE_CMD;
    task_cmd->cmd.menu_selection_envelope_ind.message_header.command_id    = (int)GSTK_MENU_SELECTION_IND;
    task_cmd->cmd.menu_selection_envelope_ind.message_header.sim_slot_id   = gstk_util_get_sim_slot_id(client_id);
    task_cmd->cmd.menu_selection_envelope_ind.message_header.user_data     = user_data;
    task_cmd->cmd.menu_selection_envelope_ind.chosen_item_id               = item_chosen;
    task_cmd->cmd.menu_selection_envelope_ind.request_help                 = help_requested;
    task_cmd->cmd.menu_selection_envelope_ind.client_cb                    = gstk_raw_env_cb;

    /* set GSTK_CMD_Q_SIG and put on GSTK queue */
    gstk_task_put_cmd_buf(task_cmd);
  }
  else
  {
    gstk_status = GSTK_MEMORY_ERROR;
  }

  return gstk_status;
} /* gstk_send_envelope_menu_sel_command_ext */


/*===========================================================================
FUNCTION   gstk_send_icon_display_rsp

DESCRIPTION
  Library function called by client to say if the icon was displayed.

PARAMETERS
  client_id  : client id
  cmd_ref_id : This is the gstk command reference id in the
               gstk header information sent to the client in the callback
  icon_disp :  yes / no response for whether the icon was displayed

DEPENDENCIES
  estk command buffer is available

RETURN VALUE
  gstk_status_enum_type
    GSTK_CLIENT_NOT_REGISTERED - if client id is not valid
    GSTK_MEMORY_ERROR          - if no free space in ESTK queue
    GSTK_BAD_PARAM             - Bad parameters passed in
    GSTK_SUCCESS               - Command was queued successfully to ESTK

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_send_icon_display_rsp(
  gstk_client_id_type  client_id,
  uint32               gstk_user_data,
  boolean              icon_disp
)
{
#ifdef FEATURE_ESTK
  estk_cmd_type          *task_cmd_ptr  = NULL;

  /* Check for validity of client id */
  if (!gstk_util_is_valid_client_id(client_id))
  {
    UIM_MSG_ERR_1("GSTK_CLIENT_NOT_REGISTERED 0x%x", client_id);
    return GSTK_CLIENT_NOT_REGISTERED;
  }

  /* Check for validity of cmd_ref_id */
  if (GSTK_CMD_REF_TABLE_INDEX(gstk_user_data) >= GSTK_MAX_PROACTIVE_COMMAND)
  {
    UIM_MSG_ERR_1("client ref id not within range 0x%x", gstk_user_data);
    return GSTK_BAD_PARAM;
  }

  UIM_MSG_HIGH_1(" IN gstk_send_icon_display_rsp: Icon_disp is 0x%x",
                 icon_disp);
  /* get the command from buffer */
  task_cmd_ptr = estk_get_cmd_buf(); /* memset is done in this function */
  if (task_cmd_ptr == NULL)
  {
    return GSTK_MEMORY_ERROR;
  }
  task_cmd_ptr->hdr.cmd_ref_id               = gstk_user_data;
  task_cmd_ptr->hdr.cli_user_data            = 0;
  task_cmd_ptr->hdr.cmd                      = ESTK_ICON_DISPLAY_CONF_CMD;
  task_cmd_ptr->hdr.sim_slot_id              = gstk_util_get_sim_slot_id(client_id);
  task_cmd_ptr->result                       = ESTK_SUCCESS;
  task_cmd_ptr->cmd_data.user_rsp.icon_disp  = icon_disp;
  /* Ignore return value for estk_put_cmd() as the function returns error
  ** for a null task_cmd_ptr which has already been checked for in this function
  */
  estk_put_cmd(task_cmd_ptr);
  return GSTK_SUCCESS;
#else /* FEATURE_ESTK */
  (void)client_id;
  (void)gstk_user_data;
  (void)icon_disp;
  return GSTK_UNSUPPORTED_COMMAND;
#endif /* FEATURE_ESTK */
}/* gstk_send_icon_display_rsp */

