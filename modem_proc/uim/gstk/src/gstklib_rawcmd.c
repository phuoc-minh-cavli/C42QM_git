/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                     G S T K   L I B R A R Y   R A W C M D


GENERAL DESCRIPTION

  This source file contains the library functions for raw TR and Envelope that are exposed to GSTK
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

$Header: //components/rel/uim.mpss/3.3.1/gstk/src/gstklib_rawcmd.c#1 $ $DateTime: 2020/07/22 10:53:27 $ $Author: pwbldsvc $

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
FUNCTION gstk_send_raw_terminal_response

DESCRIPTION
  This function is served as a utility function to the gstk client, so that
  the client can call this function in sending a raw terminal response to
  a proactive command.

PARAMETERS
  client_id: [Input] Client id of the calling client.  This should be the
                     value returned from the gstk_slot_client_init_ext()
  user_data: [Input] User data that client wants GSTK to keep track of
  cmd_detail_ref_id: [Input] Specifies the reference ID for command details of
                             the proactive command that the client is sending
                             a terminal response to.  The reference id value
                             is sent to the client earlier in the structure
                             of the proactive command request, inside the
                             gstk_exp_hdr_type
 term_rsp: [Input] The raw terminal response that the client wants to send

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type:
    GSTK_SUCCESS,               - Terminal response is sent successfully
    GSTK_CLIENT_NOT_REGISTERED, - The client ID passed in is invalid
    GSTK_BAD_PARAM,             - One of parameters passed in is invalid
    GSTK_MEMORY_ERROR           - Can't allocate memory

COMMENTS
  This function should only be called after gstk_slot_client_init_ext has been called

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_send_raw_terminal_response (
  gstk_client_id_type                     client_id,
  uint32                                  user_data,
  uint32                                  cmd_detail_ref_id,
  gstk_generic_data_type                  term_resp)
{
  gstk_status_enum_type   gstk_status             = GSTK_SUCCESS;
  gstk_task_cmd_type      *task_cmd_ptr           = NULL;
  uint8                   *gstk_term_resp_buff_ptr= NULL;
  uint32                  index                   = GSTK_CMD_REF_TABLE_INDEX(cmd_detail_ref_id);

  UIM_MSG_HIGH_2("Client send raw terminal rsp, client_id:0x%x, cmd_detail_ref_id #:0x%x",
                 client_id, cmd_detail_ref_id);

  gstk_status = gstk_util_compare_ref_id(cmd_detail_ref_id);
  if(gstk_status != GSTK_SUCCESS)
  {
    return gstk_status;
  }

  /* validate client id */
  if (!gstk_util_is_valid_client_id(client_id))
  {
    /* it is not gstk calling this function either */
      UIM_MSG_ERR_1("Invalid Client ID 0x%x", client_id);
      return GSTK_CLIENT_NOT_REGISTERED;
    }

  /* raw terminal response data shoule be less than 255 bytes */
  if (term_resp.data_len > GSTK_MAX_DATA_SIZE)
  {
    UIM_MSG_ERR_1("Raw Terminal Response Data is too big (>%d bytes)!",
                  GSTK_MAX_DATA_SIZE);
    return GSTK_BAD_PARAM;
  }

  if (index >= GSTK_MAX_PROACTIVE_COMMAND)
  {
    UIM_MSG_ERR_1("cmd_details_ref_id (0x%x) is invalid!", cmd_detail_ref_id);
    return GSTK_BAD_PARAM;
  }

  if (command_details_ref_table_ptr[index].free_slot)
  {
    UIM_MSG_ERR_1("Invalid cmd_detail_ref_id 0x%x in gstk_send_raw_terminal_response()!",
                  cmd_detail_ref_id);
    return GSTK_BAD_PARAM;
  }

  if (term_resp.data_len <= 0 || term_resp.data_buffer_ptr == NULL)
  {
    UIM_MSG_ERR_2("Empty term resp passed in! data_len: %d, data_buffer_ptr: 0x%x",
                  term_resp.data_len, term_resp.data_buffer_ptr);
    return GSTK_BAD_PARAM;
  }

  /* check if data len is at least bigger than GSTK_CMD_TYPE_OFFSET before accessing it */
  if (term_resp.data_len <= GSTK_CMD_TYPE_OFFSET)
  {
    UIM_MSG_ERR_2("data_len (0x%x) is smaller than cmd type offset (0x%x)!",
                  term_resp.data_len, GSTK_CMD_TYPE_OFFSET);
    return GSTK_BAD_PARAM;
  }

  /* check data len is bigger than cmd details length */
  if (term_resp.data_len <=
      term_resp.data_buffer_ptr[GSTK_CMD_DETAILS_LEN_OFFSET])
  {
    UIM_MSG_ERR_2("data_len (0x%x) is smaller than length of cmd details (0x%x)!",
                  term_resp.data_len,
                  term_resp.data_buffer_ptr[GSTK_CMD_DETAILS_LEN_OFFSET]);
    return GSTK_BAD_PARAM;
  }

  /* check command number */
  if (command_details_ref_table_ptr[index].command_details.command_number
      != term_resp.data_buffer_ptr[GSTK_CMD_NUMBER_OFFSET])
  {
    UIM_MSG_ERR_2("Cmd num in raw term rsp is unexpected! expecting: 0x%x, passed in 0x%x",
                  command_details_ref_table_ptr[index].command_details.command_number,
                  term_resp.data_buffer_ptr[GSTK_CMD_NUMBER_OFFSET]);
    return GSTK_BAD_PARAM;
  }

  /* check expected type of command */
  if (command_details_ref_table_ptr[index].command_details.type_of_command
      != term_resp.data_buffer_ptr[GSTK_CMD_TYPE_OFFSET])
  {
    UIM_MSG_ERR_2("Command type in raw terminal response is unexpected! expecting 0x%x, passed in 0x%x",
                  command_details_ref_table_ptr[index].command_details.type_of_command,
                  term_resp.data_buffer_ptr[GSTK_CMD_TYPE_OFFSET]);
    return GSTK_BAD_PARAM;
  }

  /* allocate terminal response buffer used in GSTK */
  gstk_term_resp_buff_ptr = (uint8 *)GSTK_CALLOC(int32touint32(term_resp.data_len));
  if (gstk_term_resp_buff_ptr == NULL) {
    return GSTK_MEMORY_ERROR;
  }
  else {  /* copy client terminal response data to GSTK buffer */
    (void)gstk_memcpy(gstk_term_resp_buff_ptr,
                      term_resp.data_buffer_ptr,
                      int32touint32(term_resp.data_len),
                      int32touint32(term_resp.data_len),
                      int32touint32(term_resp.data_len));
  }

  task_cmd_ptr = gstk_task_get_cmd_buf();
  if ( task_cmd_ptr != NULL ) { /* successfully obtained command buffer */
    /* populate command buffer */
    task_cmd_ptr->cmd.client_raw_term_resp.message_header.command_group =
      GSTK_TERMINAL_RSP;
    task_cmd_ptr->cmd.client_raw_term_resp.message_header.command_id    =
      int32touint32((int32)GSTK_RAW_TERMIMAL_RESPONSE);
    task_cmd_ptr->cmd.client_raw_term_resp.cmd_details_ref_id           =
      cmd_detail_ref_id;
    task_cmd_ptr->cmd.client_raw_term_resp.message_header.user_data     =
      user_data;
    task_cmd_ptr->cmd.client_raw_term_resp.client_id                    =
      client_id;
    task_cmd_ptr->cmd.client_raw_term_resp.raw_term_resp.data_len       =
      term_resp.data_len;
    task_cmd_ptr->cmd.client_raw_term_resp.raw_term_resp.data_buffer_ptr=
      gstk_term_resp_buff_ptr;
    task_cmd_ptr->cmd.client_raw_term_resp.message_header.sim_slot_id   =
      gstk_util_get_sim_slot_id(client_id);

    /* put command into queue */
    gstk_task_put_cmd_buf(task_cmd_ptr);
  }
  else { /* command details info not okay */
    gstk_free(gstk_term_resp_buff_ptr);
    UIM_MSG_ERR_0("No GSTK Cmd Buf for Raw Terminal Resp Info! client_id: 0x%x, cmd_detail_ref_id: 0x%x");
    gstk_status = GSTK_MEMORY_ERROR;
  }
  return gstk_status;
} /*gstk_send_raw_terminal_response*/


/*===========================================================================
FUNCTION GSTK_SEND_RAW_ENVELOPE_COMMAND

DESCRIPTION

  This function is used to send raw envelope command down to SIM/USIM/RUIM.

PARAMETERS
  client_id: [Input] Client id of the calling client.  This should be the value
                     returned from the gstk_slot_client_init_ext()
  user_data: [Input] User data that client wants GSTK to keep track of
  env_cmd:   [Input] The envelope command data to be sent to SIM/USIM
  env_cmd_cb:[Input] Callback function

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type:
    GSTK_SUCCESS,               - The envelopse command is sent successfully
    GSTK_MEMORY_ERROR,          - Can't allocate memory or envelope command information buffer
    GSTK_BAD_PARAM,             - The parameter passed in is invalid
    GSTK_CLIENT_NOT_REGISTERED  - The client is not registered

COMMENTS
  This function should only be called after gstk_slot_client_init_ext has been called

SIDE EFFECTS
  Upon calling this function, client should expect to receive
  gstk_raw_envelope_rsp_type in its callback function.

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_send_raw_envelope_command (
  gstk_client_id_type          client_id,
  uint32                       user_data,
  gstk_generic_data_type       env_cmd,
  gstk_evt_cb_funct_type       gstk_raw_env_cb)
{
  gstk_status_enum_type   gstk_status  = GSTK_SUCCESS;
  gstk_task_cmd_type    * task_cmd_ptr = NULL;
#ifdef FEATURE_ESTK_SETUP_MENU_INTERFACE
  boolean                 help_request = FALSE;
  uint8                   item_chosen  = 0;
#endif /*FEATURE_ESTK_SETUP_MENU_INTERFACE*/

  UIM_MSG_HIGH_1("GSTK client send raw envelope of len 0x%x",
                 env_cmd.data_len);

  /* validate client id */
  if (!gstk_util_is_valid_client_id(client_id))
  {
    /* it is not gstk calling this function either */
    UIM_MSG_ERR_1("Client 0x%x not registered", client_id);
    return GSTK_CLIENT_NOT_REGISTERED;
  }

  if (env_cmd.data_len <= 0)
  {
    UIM_MSG_ERR_1("invalid env cmd data len! 0x%x", env_cmd.data_len);
    return GSTK_BAD_PARAM;
  }

  if (env_cmd.data_buffer_ptr == NULL)
  {
    UIM_MSG_ERR_0("Null Ptr!");
    return GSTK_BAD_PARAM;
  }

#ifdef FEATURE_ESTK_SETUP_MENU_INTERFACE
  if (env_cmd.data_buffer_ptr[0] == 0xD3)
  {
    if (env_cmd.data_len < 9)
    {
      UIM_MSG_ERR_1("invalid len of env cmd: 0x%x", env_cmd.data_len);
      return GSTK_BAD_PARAM;
    }
    else
    {
      item_chosen = env_cmd.data_buffer_ptr[8];
      if (env_cmd.data_len > 9)
      {
        help_request = TRUE;
      }
    }

    /* menu selection envelope cmd */
    gstk_status = gstk_send_envelope_menu_sel_command_ext(
                    client_id,
                    user_data,
                    item_chosen,
                    help_request,
                    gstk_raw_env_cb);
    return gstk_status;
  }
#endif /*FEATURE_ESTK_SETUP_MENU_INTERFACE*/

  /* get command buf */
  task_cmd_ptr = gstk_task_get_cmd_buf();
  if ( task_cmd_ptr != NULL )
  {
    /* successfully obtained command buffer - build the envelope command */
    task_cmd_ptr->cmd.raw_envelope_ind.message_header.client_id     = client_id;
    task_cmd_ptr->cmd.raw_envelope_ind.message_header.sim_slot_id   = gstk_util_get_sim_slot_id(client_id);
    task_cmd_ptr->cmd.raw_envelope_ind.message_header.command_group = GSTK_ENVELOPE_CMD;
    task_cmd_ptr->cmd.raw_envelope_ind.message_header.command_id    = (int)GSTK_RAW_ENVELOPE_IND;
    task_cmd_ptr->cmd.raw_envelope_ind.message_header.user_data     = user_data;
    task_cmd_ptr->cmd.raw_envelope_ind.client_cb                    = gstk_raw_env_cb;

    task_cmd_ptr->cmd.raw_envelope_ind.env_len      = int32touint32(env_cmd.data_len);
    task_cmd_ptr->cmd.raw_envelope_ind.env_data_ptr = GSTK_CALLOC(int32touint32(env_cmd.data_len));
    if (task_cmd_ptr->cmd.raw_envelope_ind.env_data_ptr == NULL)
    {
      gstk_task_free_cmd_buf(task_cmd_ptr);
      return GSTK_MEMORY_ERROR;
    }

    (void)gstk_memcpy(task_cmd_ptr->cmd.raw_envelope_ind.env_data_ptr,
                      env_cmd.data_buffer_ptr,
                      int32touint32(env_cmd.data_len),
                      task_cmd_ptr->cmd.raw_envelope_ind.env_len,
                      int32touint32(env_cmd.data_len));

    /* set GSTK_CMD_Q_SIG and put on GSTK queue */
    gstk_task_put_cmd_buf(task_cmd_ptr);
  }
  else
  {
    gstk_status = GSTK_MEMORY_ERROR;
  }

  return gstk_status;
} /* gstk_send_raw_envelope_command */

