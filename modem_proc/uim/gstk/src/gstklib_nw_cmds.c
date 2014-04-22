/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                     G S T K   L I B R A R Y  N W  C M D S


GENERAL DESCRIPTION

  This source file contains the library functions for NW commands that are exposed to GSTK
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

$Header: //components/rel/uim.mpss/3.3.1/gstk/src/gstklib_nw_cmds.c#1 $ $DateTime: 2020/07/22 10:53:27 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/23/16   gs      Initial Version

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "gstk.h"
#include "uim_msg.h"

/*===========================================================================
FUNCTION GSTK_SEND_ENVELOPE_CELL_BROADCAST_DL_COMMAND

DESCRIPTION

  Clients can call this function to send a cell broadcast download command to the
  SIM/USIM.

PARAMETERS
  user_data: [Input] User data that client wants GSTK to keep track of
  cb_page: [Input] Pointer indicating the Cell Broadcast page for the download
                    command
  gstk_cb_dl_cb: [Input] Function pointer to which GSTK will send
                             the card response data corresponding to the
                             Cell Broadcast download cmd

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  This function should only be called after gstk_slot_client_init_ext has been called.
  Client is responsible to pad the page to length 88 (31.111)

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_send_envelope_cell_broadcast_dl_command (
  uint32                     user_data,
  const gstk_cb_page_type   *cb_page_ptr,
  gstk_evt_cb_funct_type     gstk_cb_dl_cb
)
{
  return gstk_slot_send_envelope_cell_broadcast_dl_command(
           GSTK_SLOT_1,
           user_data,
           cb_page_ptr,
           gstk_cb_dl_cb);
}

/*===========================================================================
FUNCTION GSTK_SLOT_SEND_ENVELOPE_CELL_BROADCAST_DL_COMMAND

DESCRIPTION

  Clients can call this function to send a cell broadcast download command to
  a specific SIM/USIM slot

PARAMETERS
  slot: [Input] The SIM slot ID
  user_data: [Input] User data that client wants GSTK to keep track of
  cb_page: [Input] Pointer indicating the Cell Broadcast page for the download
                    command
  gstk_cb_dl_cb: [Input] Function pointer to which GSTK will send
                             the card response data corresponding to the
                             Cell Broadcast download cmd

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  This function should only be called after gstk_slot_client_init_ext has been called.
  Client is responsible to pad the page to length 88 (31.111)

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_slot_send_envelope_cell_broadcast_dl_command (
  gstk_slot_id_enum_type      slot,
  uint32                      user_data,
  const gstk_cb_page_type   * cb_page_ptr,
  gstk_evt_cb_funct_type      gstk_cb_dl_cb
)
{
  gstk_task_cmd_type    * task_cmd = NULL;

  UIM_MSG_HIGH_0("GSTK client cell broadcast dl");

  if (!GSTK_IS_VALID_SLOT_ID(slot))
  {
    UIM_MSG_ERR_1("invalid slot id 0x%x!", slot);
    return GSTK_BAD_PARAM;
  }

  if(gstk_instances_ptr[(uint32)slot-1] == NULL)
  {
    return GSTK_BAD_PARAM;
  }

  /* validate call back function */
  if(gstk_cb_dl_cb == NULL || cb_page_ptr == NULL)
  {
    UIM_MSG_ERR_2("Null Input Param: gstk_cb_dl_cb = 0x%x cb_page_ptr = 0x%x",
                  gstk_cb_dl_cb, cb_page_ptr);
    return GSTK_NULL_INPUT_PARAM;
  }

  /* If padding is required, it is required to be done by the caller */
  if(cb_page_ptr->length != GSTK_CELL_BROADCAST_PAGE_LEN)
  {
    /* mandatory param */
    UIM_MSG_ERR_1("Cell Broadcast len = 0x%x", cb_page_ptr->length);
    return GSTK_BAD_PARAM;
  }

  if(cb_page_ptr->cb_page_data == NULL)
  {
    /* null page data */
    UIM_MSG_ERR_0("Cell Broadcast data NULL");
    return GSTK_BAD_PARAM;
  }

  /* get command buf */
  task_cmd = gstk_task_get_cmd_buf();
  if(task_cmd != NULL)
  {
    /* successfully obtained command buffer - build the envelope command */
    task_cmd->cmd.cell_broadcast_envelope_ind.message_header.sim_slot_id   = slot;
    task_cmd->cmd.cell_broadcast_envelope_ind.message_header.client_id     = gstk_instances_ptr[(uint32)slot-1]->client_id;
    task_cmd->cmd.cell_broadcast_envelope_ind.message_header.command_group = GSTK_ENVELOPE_CMD;
    task_cmd->cmd.cell_broadcast_envelope_ind.message_header.command_id    = (int)GSTK_CELL_BROADCAST_DOWNLOAD_IND;
    task_cmd->cmd.cell_broadcast_envelope_ind.message_header.user_data     = user_data;
    task_cmd->cmd.cell_broadcast_envelope_ind.client_cb                    = gstk_cb_dl_cb;

    /* Cell Broadcast Page */
    task_cmd->cmd.cell_broadcast_envelope_ind.cb_page.length       = cb_page_ptr->length;
    task_cmd->cmd.cell_broadcast_envelope_ind.cb_page.cb_page_data = GSTK_CALLOC(cb_page_ptr->length);
    if(task_cmd->cmd.cell_broadcast_envelope_ind.cb_page.cb_page_data == NULL)
    {
      gstk_task_free_cmd_buf(task_cmd);
      return GSTK_MEMORY_ERROR;
    }
    (void)memscpy(task_cmd->cmd.cell_broadcast_envelope_ind.cb_page.cb_page_data,
           cb_page_ptr->length,
           cb_page_ptr->cb_page_data,
           cb_page_ptr->length) ;

    /* set GSTK_CMD_Q_SIG and put on GSTK queue */
    gstk_task_put_cmd_buf(task_cmd);
  }
  else
  {
    return GSTK_MEMORY_ERROR;
  }

  return GSTK_SUCCESS;
} /*gstk_slot_send_envelope_cell_broadcast_dl_command*/

