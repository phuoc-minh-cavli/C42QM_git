/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                      ESTK  BEARER INDEPENDENT PROTOCOL


GENERAL DESCRIPTION : ENHANCED STK layer BIP support for SCWS

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/estk/src/estk_bip_scws.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------------------
09/15/16   gs      Changes in TP based on RAT and App Type
05/25/16   shr     F3 reduction: Malloc Failure, Memcpy, Null Ptr checks
05/16/16   gs      F3 improvements
03/22/16   gm      Initial version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_msg.h"
#ifdef FEATURE_ESTK
#include "estk_bip.h"
#include "nv.h"
#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */


/*===========================================================================
FUNCTION: estk_scws_send_data

DESCRIPTION:
   This function sends the SCWS Send Data req to the Agent

PARAMETERS:
  index     - Index into the bip ch table
  cmd_ref_id - Cmd ref id for terminal response
DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error
    ESTK_BAD_INPUT_PARAM - Bad Input parameters

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_scws_send_data(
  int32               index,
  uint32              cmd_ref_id
)
{
  gstk_io_find_client_with_registered_event_in_param_type find_cli_reg;
  gstk_io_call_client_evt_cb_param_type    call_cli_evt_cb;
  uint32                                   user_data   = 0;
  gstk_cmd_from_card_type                  gstk_scws_cmd;
  uint8                                    addi_res    = 0;
  gstk_additional_info_ptr_type            additional_info;
  gstk_terminal_rsp_extra_param_type       tr_extra_param;
  gstk_client_id_type                      client_id       = 0;
  gstk_status_enum_type                    gstk_status     = GSTK_SUCCESS;

  UIM_MSG_HIGH_2("estk_scws_send_data: index=0x%x, cmd_ref_id=0x%x",
                 index, cmd_ref_id);

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  if (index >= ESTK_MAX_BIP_CH)
  {
    UIM_MSG_ERR_1("invalid ch idx 0x%x!", index);
    return ESTK_ERROR;
  }
  if(estk_curr_inst_ptr->bip_channel_table[index].send_buffer == NULL)
  {
    UIM_MSG_ERR_0("Send buffer is null");
    return ESTK_ERROR;
  }

  memset(&tr_extra_param, 0x00, sizeof(tr_extra_param));
  memset(&gstk_scws_cmd, 0x00, sizeof(gstk_cmd_from_card_type));
  memset(&additional_info, 0x00, sizeof(gstk_additional_info_ptr_type));
  memset(&tr_extra_param, 0x00, sizeof(gstk_terminal_rsp_extra_param_type));
  memset(&find_cli_reg, 0x00, sizeof(gstk_io_find_client_with_registered_event_in_param_type));
  memset(&call_cli_evt_cb, 0x00, sizeof(call_cli_evt_cb));

  additional_info.length = 1;
  additional_info.additional_info_ptr = &addi_res;

  tr_extra_param.rsp_type = GSTK_SEND_DATA_CNF;
  tr_extra_param.present = TRUE;

  find_cli_reg.cli_func_type = GSTK_HANDLE_DISPLAY_FUNC_ONLY;
  find_cli_reg.cmd_type = GSTK_SCWS_SEND_DATA_REQ;
  find_cli_reg.format_type = GSTK_GSTK_FORMAT;
  gstk_status = gstk_io_ctrl(
                  GSTK_IO_FIND_CLIENT_WITH_REGISTERED_EVENT,
                  (void *)&find_cli_reg,
                  (void *)&client_id);
  if(gstk_status != GSTK_SUCCESS)
  {
    UIM_MSG_HIGH_0("Did not find any clients registered for GSTK_SCWS_SEND_DATA_REQ");
    return ESTK_ERROR;
  }
  else
  {
    call_cli_evt_cb.client_id = client_id;
    call_cli_evt_cb.gstk_req_ptr = (gstk_cmd_from_card_type*)&gstk_scws_cmd;
    gstk_scws_cmd.hdr_cmd.command_id = GSTK_SCWS_SEND_DATA_REQ;
    gstk_scws_cmd.hdr_cmd.cmd_detail_reference = cmd_ref_id;
    gstk_scws_cmd.hdr_cmd.sim_slot_id =  estk_curr_inst_ptr->curr_cmd.hdr_cmd.sim_slot_id;
    gstk_scws_cmd.hdr_cmd.command_number = estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_number;
    gstk_scws_cmd.hdr_cmd.user_data = estk_curr_inst_ptr->curr_cmd.hdr_cmd.user_data;

    gstk_scws_cmd.cmd.scws_send_data_cmd_req.ch_id = index+1;
    gstk_scws_cmd.cmd.scws_send_data_cmd_req.ch_data.data_len =
      estk_curr_inst_ptr->bip_channel_table[index].send_buf_len;
    gstk_scws_cmd.cmd.scws_send_data_cmd_req.ch_data.data =
      estk_curr_inst_ptr->bip_channel_table[index].send_buffer;

    if (gstk_io_ctrl(
            GSTK_IO_CALL_CLIENT_EVT_CB,
            (void *)&call_cli_evt_cb,
            (void *)&user_data /* dummy data*/) != GSTK_SUCCESS)
    {
      UIM_MSG_ERR_0("can't call cli evt cb!");
      addi_res = (uint8)GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
      if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
      {
        UIM_MSG_HIGH_0("Saving SEND DATA TR");
        estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
        estk_curr_inst_ptr->result = GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR;
        estk_cache_additional_info(&additional_info);
        estk_curr_inst_ptr->extra_param = tr_extra_param;
      }
      else
      {
      /* Send error terminal response */
      (void)estk_bip_send_terminal_response(
              cmd_ref_id,
              GSTK_SEND_DATA_CNF,
              GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR,
              &additional_info,
              &tr_extra_param,
              index);
      }
    }
  }
  return ESTK_SUCCESS;
} /* estk_scws_send_data  */

/*===========================================================================

FUNCTION: estk_process_scws_ch_status_req

DESCRIPTION :
  This function processes Ch Status request from the Agent

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_scws_ch_status_req(
  const estk_cmd_type *cmd_ptr)
{

  gstk_evt_dl_ind_type        ch_status;
  uint32                      index = 0;

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  if(cmd_ptr == NULL)
  {
    UIM_MSG_ERR_0("Invalid input for estk_process_scws_ch_status_req");
    return ESTK_ERROR;
  }

  index = cmd_ptr->cmd_data.scws_ch_status_req.ch_id - 1;

  UIM_MSG_HIGH_3("estk_process_scws_ch_status_req: ch_id=0x%x, "
                 "estk_curr_inst_ptr->bip_channel_table[0x%x].channel_id=0x%x",
                 cmd_ptr->cmd_data.scws_ch_status_req.ch_id,
                 index,
                 (index < ESTK_MAX_BIP_CH)? estk_curr_inst_ptr->bip_channel_table[index].channel_id:-1);

  if ((index >= ESTK_MAX_BIP_CH) ||
      (cmd_ptr->cmd_data.scws_ch_status_req.ch_id !=
       (uint32)estk_curr_inst_ptr->bip_channel_table[index].channel_id))
  {
    return ESTK_ERROR;
  }

  memset(&ch_status, 0x00, sizeof(gstk_evt_dl_ind_type));

  estk_curr_inst_ptr->bip_channel_table[index].status =
     (estk_channel_status_type)((uint32)cmd_ptr->cmd_data.scws_ch_status_req.ch_status + (uint32)ESTK_CH_SCWS_OFFSET);
  /* Send Channel status Event Download envelope command */
  ch_status.evt_type = GSTK_CH_STATUS_EVT_IND;
  ch_status.evt_ind.ch_status_evt_ind.ch_id =
         estk_curr_inst_ptr->bip_channel_table[index].channel_id;
  ch_status.evt_ind.ch_status_evt_ind.is_scws_ch = TRUE;
  ch_status.evt_ind.ch_status_evt_ind.scws_ch_status = cmd_ptr->cmd_data.scws_ch_status_req.ch_status;
  ch_status.evt_ind.ch_status_evt_ind.ch_status_additional_info =
         GSTK_CH_STATUS_NO_FURTHER_INFO;
  if(gstk_send_envelope_evt_download_command(estk_curr_inst_ptr->gstk_client_id,
             0, &ch_status) != GSTK_SUCCESS)
  {
    UIM_MSG_ERR_0("Sending Channel Status envelope failed");
    return ESTK_ERROR;
  }
  if((cmd_ptr->cmd_data.scws_ch_status_req.ch_status == GSTK_SCWS_TCP_IN_LISTEN_STATE) ||
     (cmd_ptr->cmd_data.scws_ch_status_req.ch_status == GSTK_SCWS_TCP_IN_CLOSED_STATE))
  {
     estk_scws_clear_send_receive_buf((uint8)ch_status.evt_ind.ch_status_evt_ind.ch_id);
  }
  if(cmd_ptr->cmd_data.scws_ch_status_req.ch_status == GSTK_SCWS_TCP_IN_LISTEN_STATE)
  {
    UIM_MSG_HIGH_0("Setting receive_data_allowed to FALSE");
    estk_curr_inst_ptr->bip_channel_table[index].receive_data_allowed = FALSE;
  }
  return ESTK_SUCCESS;
} /* estk_process_scws_ch_status_req */

/*===========================================================================

FUNCTION: estk_process_scws_data_avail_req

DESCRIPTION :
  This function processes Data Avail req from the Agent

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_scws_data_avail_req(
  estk_cmd_type *cmd_ptr)
{

   gstk_evt_dl_ind_type        data_avail;
   uint32                      index = 0;
   boolean                     data_avail_required = FALSE;
   uint8                       *tmp_ptr = NULL;

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  if(cmd_ptr == NULL)
  {
    UIM_MSG_ERR_0("Invalid input for estk_process_scws_data_avail_req");
    estk_scws_send_data_avail_rsp((uint32)index, FALSE);
    return ESTK_ERROR;
  }

  if(cmd_ptr->cmd_data.scws_data_avail_req.data_payload.data_ptr == NULL)
  {
    UIM_MSG_ERR_0("No data was sent, return error");
    estk_scws_send_data_avail_rsp((uint32)index, FALSE);
    return ESTK_ERROR;
  }

  index = cmd_ptr->cmd_data.scws_data_avail_req.ch_id - 1;
  UIM_MSG_HIGH_3("estk_process_scws_data_avail_req: ch_id=0x%x, "
                 "estk_curr_inst_ptr->bip_channel_table[0x%x].channel_id=0x%x",
                 cmd_ptr->cmd_data.scws_data_avail_req.ch_id,
                 index,
                 (index < ESTK_MAX_BIP_CH)? estk_curr_inst_ptr->bip_channel_table[index].channel_id:-1);

  if ((index >= ESTK_MAX_BIP_CH)||
      (cmd_ptr->cmd_data.scws_data_avail_req.ch_id !=
       (uint32)estk_curr_inst_ptr->bip_channel_table[index].channel_id))
  {
    gstk_free(cmd_ptr->cmd_data.scws_data_avail_req.data_payload.data_ptr);
    cmd_ptr->cmd_data.scws_data_avail_req.data_payload.data_ptr = NULL;
    estk_scws_send_data_avail_rsp((uint32)index, FALSE);
    return ESTK_ERROR;
  }

  memset(&data_avail, 0x00, sizeof(gstk_evt_dl_ind_type));

  if(estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len == 0)
  {
    /* Since the receive data is zero, Data Availale envelope is required */
    data_avail_required = TRUE;
  }

  UIM_MSG_HIGH_3("Data received is 0x%x, buffer size is 0x%x receive buffer len is 0x%x",
                 cmd_ptr->cmd_data.scws_data_avail_req.data_payload.len,
                 estk_curr_inst_ptr->bip_channel_table[index].buffer_size,
                 estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len);

  if((estk_curr_inst_ptr->bip_channel_table[index].recv_buffer != NULL) &&
     ((uint32)cmd_ptr->cmd_data.scws_data_avail_req.data_payload.len <=
      (estk_curr_inst_ptr->bip_channel_table[index].buffer_size - (uint32)estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len))
      && (estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf == NULL))
  {
    if(gstk_memcpy((void *)(estk_curr_inst_ptr->bip_channel_table[index].recv_buffer +
                   estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len),
                   (void *)cmd_ptr->cmd_data.scws_data_avail_req.data_payload.data_ptr,
                   cmd_ptr->cmd_data.scws_data_avail_req.data_payload.len,
                   estk_curr_inst_ptr->bip_channel_table[index].buffer_size,
                   cmd_ptr->cmd_data.scws_data_avail_req.data_payload.len) <
       cmd_ptr->cmd_data.scws_data_avail_req.data_payload.len)
    {
      gstk_free(cmd_ptr->cmd_data.scws_data_avail_req.data_payload.data_ptr);
      cmd_ptr->cmd_data.scws_data_avail_req.data_payload.data_ptr = NULL;
      estk_scws_send_data_avail_rsp((uint32)index, FALSE);
      return ESTK_ERROR;
    }
    estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len += cmd_ptr->cmd_data.scws_data_avail_req.data_payload.len;
  }
  else
  {
    UIM_MSG_HIGH_0("Have to use the SCWS overflow buffer");
    /* Copy the data to the overflow buffer */
    if(estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf == NULL)
    {
      UIM_MSG_HIGH_0("scws_overflow_buf is Null");
      estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf =
            (uint8 *)GSTK_CALLOC(cmd_ptr->cmd_data.scws_data_avail_req.data_payload.len * sizeof(uint8));
      if(estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf == NULL)
      {
        gstk_free(cmd_ptr->cmd_data.scws_data_avail_req.data_payload.data_ptr);
        cmd_ptr->cmd_data.scws_data_avail_req.data_payload.data_ptr = NULL;
        estk_scws_send_data_avail_rsp((uint32)index, FALSE);
        return ESTK_ERROR;
      }

      (void)gstk_memcpy((void *)estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf,
                     (void *)cmd_ptr->cmd_data.scws_data_avail_req.data_payload.data_ptr,
                     cmd_ptr->cmd_data.scws_data_avail_req.data_payload.len,
                     cmd_ptr->cmd_data.scws_data_avail_req.data_payload.len,
                     cmd_ptr->cmd_data.scws_data_avail_req.data_payload.len);
      estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len = cmd_ptr->cmd_data.scws_data_avail_req.data_payload.len;
      UIM_MSG_HIGH_1("scws_overflow_buf_len is 0x%x",
                     estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len);
    }
    else
    {
      /* The buffer is not empty, extend it with new data */
      UIM_MSG_HIGH_1("SCWS buffer already exists, allocating a buffer of size 0x%x",
                     cmd_ptr->cmd_data.scws_data_avail_req.data_payload.len +
                     estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len);
      tmp_ptr =
         (uint8 *)GSTK_CALLOC(cmd_ptr->cmd_data.scws_data_avail_req.data_payload.len +
           estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len);
      if(tmp_ptr == NULL)
      {
        gstk_free(cmd_ptr->cmd_data.scws_data_avail_req.data_payload.data_ptr);
        cmd_ptr->cmd_data.scws_data_avail_req.data_payload.data_ptr = NULL;
        estk_scws_send_data_avail_rsp((uint32)index, FALSE);
        return ESTK_ERROR;
      }
      /* Copy the existing overflow buffer */
      if(gstk_memcpy((void *)tmp_ptr,
                     (void *)estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf,
                     estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len,
                     cmd_ptr->cmd_data.scws_data_avail_req.data_payload.len + estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len,
                     estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len) <
         estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len)
      {
        gstk_free(cmd_ptr->cmd_data.scws_data_avail_req.data_payload.data_ptr);
        cmd_ptr->cmd_data.scws_data_avail_req.data_payload.data_ptr = NULL;
        gstk_free(tmp_ptr);
        tmp_ptr = NULL;
        estk_scws_send_data_avail_rsp((uint32)index, FALSE);
        return ESTK_ERROR;
      }
      /* Copy new data */
      if(gstk_memcpy((void *)(tmp_ptr + estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len),
                     (void *)cmd_ptr->cmd_data.scws_data_avail_req.data_payload.data_ptr,
                     cmd_ptr->cmd_data.scws_data_avail_req.data_payload.len,
                     estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len + cmd_ptr->cmd_data.scws_data_avail_req.data_payload.len,
                     cmd_ptr->cmd_data.scws_data_avail_req.data_payload.len) <
         cmd_ptr->cmd_data.scws_data_avail_req.data_payload.len)
      {
        gstk_free(cmd_ptr->cmd_data.scws_data_avail_req.data_payload.data_ptr);
        cmd_ptr->cmd_data.scws_data_avail_req.data_payload.data_ptr = NULL;
        gstk_free(tmp_ptr);
        tmp_ptr = NULL;
        estk_scws_send_data_avail_rsp((uint32)index, FALSE);
        return ESTK_ERROR;
      }
      estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len +=
        cmd_ptr->cmd_data.scws_data_avail_req.data_payload.len;
      UIM_MSG_HIGH_1("New scws overflow buf is 0x%x",
                     estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len);
      /* Release old buffer and use the new one */
      gstk_free(estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf);
      estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf = NULL;
      estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf = tmp_ptr;
    }
  }

   /* Send event download envelope with data available event */
  if (data_avail_required)
  {
    data_avail.evt_ind.data_avail_evt_ind.ch_status.ch_id
      = estk_curr_inst_ptr->bip_channel_table[index].channel_id;
    data_avail.evt_ind.data_avail_evt_ind.ch_status.is_scws_ch = TRUE;
    data_avail.evt_ind.data_avail_evt_ind.ch_status.scws_ch_status =
        (gstk_scws_ch_state_enum_type)((uint32)estk_curr_inst_ptr->bip_channel_table[index].status - (uint32)ESTK_CH_SCWS_OFFSET);
    data_avail.evt_ind.data_avail_evt_ind.ch_status.ch_status_additional_info
      = GSTK_CH_STATUS_NO_FURTHER_INFO;
    data_avail.evt_ind.data_avail_evt_ind.ch_status.is_link_est = TRUE;
    data_avail.evt_type = GSTK_DATA_AVAIL_EVT_IND;
    data_avail.evt_ind.data_avail_evt_ind.ch_len
       = estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len;

    UIM_MSG_HIGH_1("ESTK sending data available event with data length 0x%x",
                   data_avail.evt_ind.data_avail_evt_ind.ch_len);
    if(gstk_send_envelope_evt_download_command(estk_curr_inst_ptr->gstk_client_id,
                                               0, &data_avail) != GSTK_SUCCESS)
    {
      UIM_MSG_ERR_0("Sending data available envelope failed");
      gstk_free(cmd_ptr->cmd_data.scws_data_avail_req.data_payload.data_ptr);
      cmd_ptr->cmd_data.scws_data_avail_req.data_payload.data_ptr = NULL;
      estk_scws_send_data_avail_rsp((uint32)index, FALSE);
      return ESTK_ERROR;
    }
  }

  gstk_free(cmd_ptr->cmd_data.scws_data_avail_req.data_payload.data_ptr);
  cmd_ptr->cmd_data.scws_data_avail_req.data_payload.data_ptr = NULL;

   /* Reply to the data available message */
  return estk_scws_send_data_avail_rsp((uint32)index, TRUE);
} /* estk_process_scws_data_avail_req */

/*===========================================================================

FUNCTION: estk_process_scws_open_ch_rsp

DESCRIPTION :
  This function processes the Open Ch response from the Agent

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_scws_open_ch_rsp(
  const estk_cmd_type *cmd_ptr)
{
  estk_result_enum_type               estk_status  = ESTK_ERROR;
  uint32                              index        = 0;
  gstk_terminal_rsp_extra_param_type  tr_extra_param;
  uint32                              cmd_ref_id = 0;
  gstk_general_result_enum_type       gen_result       = GSTK_GENERAL_RESULT_MAX;

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  if(cmd_ptr == NULL)
  {
    UIM_MSG_ERR_0("Invalid input for estk_process_scws_open_ch_rsp");
    return ESTK_ERROR;
  }

  index = cmd_ptr->cmd_data.scws_open_ch_rsp.ch_id - 1;
  UIM_MSG_HIGH_3("estk_process_scws_open_ch_rsp: ch_id=0x%x, "
                 "estk_curr_inst_ptr->bip_channel_table[0x%x].channel_id=0x%x",
                 cmd_ptr->cmd_data.scws_open_ch_rsp.ch_id,
                 index,
                 (index < ESTK_MAX_BIP_CH)? estk_curr_inst_ptr->bip_channel_table[index].channel_id:-1);

  if ((index >= ESTK_MAX_BIP_CH) ||
      (cmd_ptr->cmd_data.scws_open_ch_rsp.ch_id !=
       (uint32)estk_curr_inst_ptr->bip_channel_table[index].channel_id))
  {
    return ESTK_ERROR;
  }

  memset(&tr_extra_param, 0x00, sizeof(tr_extra_param));
  cmd_ref_id = estk_curr_inst_ptr->curr_cmd.hdr_cmd.cmd_detail_reference;
  tr_extra_param.present = TRUE;
  tr_extra_param.rsp_type = GSTK_OPEN_CH_CNF;

  estk_curr_inst_ptr->bip_channel_table[index].status =
      (estk_channel_status_type)((uint32)cmd_ptr->cmd_data.scws_open_ch_rsp.ch_status + (uint32)ESTK_CH_SCWS_OFFSET);
  if(cmd_ptr->cmd_data.scws_open_ch_rsp.ch_status != GSTK_SCWS_TCP_IN_LISTEN_STATE)
  {
    tr_extra_param.extra_param.open_ch_extra_param.ch_status.is_scws_ch = TRUE;
    UIM_MSG_ERR_1("The channel is not in 0x%x state",
                  estk_curr_inst_ptr->bip_channel_table[index].status);
    return estk_bip_send_terminal_response(
             cmd_ref_id,
             GSTK_OPEN_CH_CNF,
             GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR,
             NULL,
             &tr_extra_param,
             index);
  }
  else
  {
    gen_result = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
    if(estk_curr_inst_ptr->bip_channel_table[index].open_ch_param_modified)
    {
      gen_result = GSTK_COMMAND_PERFORMED_WITH_MODIFICATION;
    }
    tr_extra_param.present = TRUE;
    tr_extra_param.rsp_type = GSTK_OPEN_CH_CNF;
    tr_extra_param.extra_param.open_ch_extra_param.ch_status.ch_id =
          estk_curr_inst_ptr->bip_channel_table[index].channel_id;
    tr_extra_param.extra_param.open_ch_extra_param.ch_status.is_scws_ch = TRUE;
  tr_extra_param.extra_param.open_ch_extra_param.ch_status.scws_ch_status = cmd_ptr->cmd_data.scws_open_ch_rsp.ch_status;
    tr_extra_param.extra_param.open_ch_extra_param.buffer_size =
          estk_curr_inst_ptr->bip_channel_table[index].buffer_size;
    estk_status = estk_bip_send_terminal_response(
                    cmd_ref_id,
                    GSTK_OPEN_CH_CNF,
                    gen_result,
                    NULL,
                    &tr_extra_param,
                    index);
  }
  return estk_status;
} /* estk_process_scws_open_ch_rsp */

/*===========================================================================

FUNCTION: estk_process_scws_close_ch_rsp

DESCRIPTION :
  This function processes the Close Ch response from the Agent

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_scws_close_ch_rsp(
  const estk_cmd_type *cmd_ptr)
{
  uint32                              index = 0;
  gstk_additional_info_ptr_type       additional_info;
  uint8                               addi_res   = 0;
  uint32                              cmd_ref_id = 0;

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  if(cmd_ptr == NULL)
  {
    UIM_MSG_ERR_0("Invalid input for estk_process_scws_close_ch_rsp");
    return ESTK_ERROR;
  }

  index = cmd_ptr->cmd_data.scws_close_ch_rsp.ch_id - 1;
  UIM_MSG_HIGH_3("estk_process_scws_close_ch_rsp: ch_id=0x%x, "
                 "estk_curr_inst_ptr->bip_channel_table[0x%x].channel_id=0x%x",
                 cmd_ptr->cmd_data.scws_close_ch_rsp.ch_id,
                 index,
                 (index < ESTK_MAX_BIP_CH)? estk_curr_inst_ptr->bip_channel_table[index].channel_id:-1);

  if ((index >= ESTK_MAX_BIP_CH) ||
      (cmd_ptr->cmd_data.scws_close_ch_rsp.ch_id !=
       (uint32)estk_curr_inst_ptr->bip_channel_table[index].channel_id))
  {
    return ESTK_ERROR;
  }

  memset(&additional_info, 0x00, sizeof(additional_info));
  cmd_ref_id = estk_curr_inst_ptr->curr_cmd.hdr_cmd.cmd_detail_reference;
  addi_res = (uint8)GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
  additional_info.additional_info_ptr = &addi_res;
  additional_info.length = 1;

  estk_curr_inst_ptr->bip_channel_table[index].status =
     (estk_channel_status_type)((uint32)cmd_ptr->cmd_data.scws_close_ch_rsp.ch_status + (uint32)ESTK_CH_SCWS_OFFSET);
  if(cmd_ptr->cmd_data.scws_close_ch_rsp.ch_status !=
    estk_curr_inst_ptr->curr_cmd.cmd.close_ch_pro_cmd_req.ch_state)
  {
    UIM_MSG_ERR_1("The channel is not in 0x%x state",
                  estk_curr_inst_ptr->bip_channel_table[index].status);
    if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
    {
      UIM_MSG_HIGH_0("Saving CLOSE CHANNEL TR");
      estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
      estk_curr_inst_ptr->result = GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR;
      estk_cache_additional_info(&additional_info);
      return ESTK_SUCCESS;
    }
    else
    {
      estk_bip_clean_up_ch_table(index);
      return estk_bip_send_terminal_response(cmd_ref_id,
               GSTK_CLOSE_CH_CNF,
               GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR,
               &additional_info,
               NULL,
               index);
    }
  }
  else
  {
    if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
    {
      UIM_MSG_HIGH_0("Saving CLOSE CHANNEL TR");
      estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
      estk_curr_inst_ptr->result = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
      return ESTK_SUCCESS;
    }
    else
    {
      estk_bip_clean_up_ch_table(index);
      return estk_bip_send_terminal_response(cmd_ref_id,
               GSTK_CLOSE_CH_CNF,
               GSTK_COMMAND_PERFORMED_SUCCESSFULLY,
               NULL,
               NULL,
               index);
    }
  }
} /* estk_process_scws_close_ch_rsp */

/*===========================================================================

FUNCTION: estk_process_scws_send_data_rsp

DESCRIPTION :
  This function processes the Send Data Response from the Agent

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_scws_send_data_rsp(
  const estk_cmd_type *cmd_ptr)
{

  gstk_terminal_rsp_extra_param_type  tr_extra_param;
  gstk_additional_info_ptr_type       additional_info;
  uint8                               addi_res   = 0;
  uint32                              cmd_ref_id = 0;
  uint32                              index      = 0;

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  if(cmd_ptr == NULL)
  {
    UIM_MSG_ERR_0("Invalid input for estk_process_scws_send_data_rsp");
    return ESTK_ERROR;
  }

  index = cmd_ptr->cmd_data.scws_send_data_rsp.ch_id - 1;
  UIM_MSG_HIGH_3("estk_process_scws_send_data_rsp: estk_curr_inst_ptr->bip_channel_table idx=0x%x, "
                 "channel_id=0x%x, "
                 "status=0x%x",
                 index,
                 estk_curr_inst_ptr->bip_channel_table[index].channel_id,
                 estk_curr_inst_ptr->bip_channel_table[index].status);
  if ((ESTK_CH_CLOSED == estk_curr_inst_ptr->bip_channel_table[index].status) ||
      (cmd_ptr->cmd_data.scws_send_data_rsp.ch_id !=
       (uint32)estk_curr_inst_ptr->bip_channel_table[index].channel_id))
  {
    return ESTK_ERROR;
  }

  memset(&tr_extra_param, 0x00, sizeof(tr_extra_param));
  memset(&additional_info, 0x00, sizeof(additional_info));

  cmd_ref_id = estk_curr_inst_ptr->curr_cmd.hdr_cmd.cmd_detail_reference;
  addi_res = (uint8)GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
  additional_info.additional_info_ptr = &addi_res;
  additional_info.length = 1;
  tr_extra_param.rsp_type = GSTK_SEND_DATA_CNF;
  tr_extra_param.present = TRUE;

  if(!cmd_ptr->cmd_data.scws_send_data_rsp.is_success)
  {
    if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
    {
      UIM_MSG_HIGH_0("Saving SEND DATA TR");
      estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
      estk_curr_inst_ptr->result = GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR;
      estk_cache_additional_info(&additional_info);
      estk_curr_inst_ptr->extra_param = tr_extra_param;
    }
    else
    {
    /* Send error terminal response */
    (void)estk_bip_send_terminal_response(cmd_ref_id,
            GSTK_SEND_DATA_CNF,
            GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR,
            &additional_info,
            &tr_extra_param,
            index);
    }
    return ESTK_ERROR;
  }
  else
  {
    estk_curr_inst_ptr->bip_channel_table[index].send_buf_len = 0;
    tr_extra_param.extra_param.send_data_extra_param.ch_data_remain_buf_len =
       estk_curr_inst_ptr->bip_channel_table[index].buffer_size;
    if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
    {
      UIM_MSG_HIGH_0("Saving SEND DATA TR");
      estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
      estk_curr_inst_ptr->result = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
      estk_curr_inst_ptr->extra_param = tr_extra_param;
    }
    else
    {
    return estk_bip_send_terminal_response(
             cmd_ref_id,
             GSTK_SEND_DATA_CNF,
             GSTK_COMMAND_PERFORMED_SUCCESSFULLY,
             NULL,
             &tr_extra_param,
             index);
    }
    return ESTK_SUCCESS;
  }
} /* estk_process_scws_send_data_rsp */

/*===========================================================================

FUNCTION: estk_scws_send_data_avail_rsp

DESCRIPTION :
  This function sends the data available response

PARAMETERS:
  index : [Input] Index to the estk channel table

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_scws_send_data_avail_rsp (
  uint32 index, boolean result)
{

  gstk_status_enum_type                    gstk_status = GSTK_SUCCESS;
  gstk_io_find_client_with_registered_event_in_param_type find_cli_reg;
  gstk_io_call_client_evt_cb_param_type call_cli_evt_cb;
  uint32                                   user_data   = 0;
  gstk_cmd_from_card_type                  gstk_scws_cmd;
  gstk_client_id_type                      client_id   = 0;

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  if (index >= ESTK_MAX_BIP_CH)
  {
    UIM_MSG_ERR_1("Index is out or range 0x%x", index);
    return ESTK_ERROR;
  }

  memset(&find_cli_reg, 0x00, sizeof(gstk_io_find_client_with_registered_event_in_param_type));
  memset(&call_cli_evt_cb, 0x00, sizeof(call_cli_evt_cb));
  memset(&gstk_scws_cmd, 0x00, sizeof(gstk_cmd_from_card_type));

  find_cli_reg.cli_func_type = GSTK_HANDLE_DISPLAY_FUNC_ONLY;
  find_cli_reg.cmd_type = GSTK_SCWS_DATA_AVAIL_RSP_REQ;
  find_cli_reg.format_type = GSTK_GSTK_FORMAT;
  gstk_status = gstk_io_ctrl(
                  GSTK_IO_FIND_CLIENT_WITH_REGISTERED_EVENT,
                  (void *)&find_cli_reg,
                  (void *)&client_id);
  if(gstk_status != GSTK_SUCCESS)
  {
    return ESTK_ERROR;
  }
  else
  {
    call_cli_evt_cb.client_id = client_id;
    call_cli_evt_cb.gstk_req_ptr = (gstk_cmd_from_card_type*)&gstk_scws_cmd;
    gstk_scws_cmd.hdr_cmd.command_id = GSTK_SCWS_DATA_AVAIL_RSP_REQ;
    gstk_scws_cmd.hdr_cmd.cmd_detail_reference = estk_curr_inst_ptr->curr_cmd.hdr_cmd.cmd_detail_reference;
    gstk_scws_cmd.hdr_cmd.sim_slot_id =  estk_curr_inst_ptr->slot_id;
    gstk_scws_cmd.hdr_cmd.command_number = estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_number;
    gstk_scws_cmd.hdr_cmd.user_data = estk_curr_inst_ptr->curr_cmd.hdr_cmd.user_data;

    gstk_scws_cmd.cmd.scws_data_avail_rsp_cmd_req.ch_id    = index+1;
    gstk_scws_cmd.cmd.scws_data_avail_rsp_cmd_req.success  = result;

    if (gstk_io_ctrl(
          GSTK_IO_CALL_CLIENT_EVT_CB,
          (void *)&call_cli_evt_cb,
          (void *)&user_data /* dummy data*/) != GSTK_SUCCESS)
    {
      UIM_MSG_ERR_0("can't call cli evt cb!");
      return ESTK_ERROR;
    }
  }
  return ESTK_SUCCESS;
} /* estk_scws_send_data_avail_rsp */

/*===========================================================================

FUNCTION: estk_scws_clear_send_receive_buf

DESCRIPTION :
  This function clears the Tx/Rx buffers

PARAMETERS:
   ch_id : Channel id

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_scws_clear_send_receive_buf(
   uint8 ch_id)
{

  uint8 index;

  ESTK_RETURN_IF_NULL_PTR(estk_curr_inst_ptr);

  index = ch_id - 1;

  if (index >= ESTK_MAX_BIP_CH)
  {
    UIM_MSG_ERR_1("Index is out or range 0x%x", index);
    return;
  }

  if(estk_curr_inst_ptr->bip_channel_table[index].recv_buffer != NULL)
  {
    /*Setting the receive buffer to 0*/
    memset(estk_curr_inst_ptr->bip_channel_table[index].recv_buffer,
           0x00,
           (uint32)estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len);
    estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len = 0;
  }
  if(estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf != NULL)
  {
    /*Freeing the SCWS overflow buffer*/
    gstk_free(estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf);
    estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf = NULL;
    estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len = 0;
  }
  if(estk_curr_inst_ptr->bip_channel_table[index].send_buffer != NULL)
  {
    /*Setting the send buffer to 0*/
    memset(estk_curr_inst_ptr->bip_channel_table[index].send_buffer,
           0x00,
           estk_curr_inst_ptr->bip_channel_table[index].send_buf_len);
    estk_curr_inst_ptr->bip_channel_table[index].send_buf_len = 0;
  }
} /* estk_scws_clear_send_receive_buf */

/*===========================================================================

FUNCTION: estk_scws_clear_receive_data_allowed

DESCRIPTION :
  This function sets receive data allowed to true

PARAMETERS:
  ch_id   [Output]: Channel ID
  slot_id [Output]: Slot ID

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_scws_clear_receive_data_allowed(
   uint8                  ch_id,
   gstk_slot_id_enum_type slot_id)
{

  uint8 index;

  UIM_MSG_HIGH_2("estk_scws_clear_receive_data_allowed, ch_id=0x%x, slot_id=0x%x",
                 ch_id, slot_id);

  if (!ESTK_IS_VALID_SLOT_ID(slot_id))
  {
    UIM_MSG_ERR_1("Invalid Slot ID: 0x%x",slot_id);
    return;
  }

  ESTK_RETURN_IF_NULL_PTR(estk_instances_ptr[(uint32)slot_id - 1]);

  index = ch_id - 1;

  if (index >= ESTK_MAX_BIP_CH)
  {
    UIM_MSG_ERR_1("Index is out or range 0x%x", index);
    return;
  }

  estk_instances_ptr[(uint32)slot_id - 1]->bip_channel_table[index].receive_data_allowed = TRUE;
} /* estk_scws_clear_receive_data_allowed */

/*===========================================================================

FUNCTION: estk_scws_populate_open_ch_table

DESCRIPTION :
  This function populates information into the Open channel Table.

PARAMETERS:
  index : [Input] Index of the channel to be populated

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_scws_populate_open_ch_table(int32 index)
{
  gstk_open_ch_req_type                    *open_ch_cmd = NULL;
  uint32                                   user_data   = 0;
  gstk_cmd_from_card_type                  gstk_scws_cmd;
  gstk_client_id_type                      client_id   = 0;
  gstk_status_enum_type                    gstk_status = GSTK_SUCCESS;
  int                                      i           = 0;
  gstk_io_find_client_with_registered_event_in_param_type find_cli_reg;
  gstk_io_call_client_evt_cb_param_type call_cli_evt_cb;

  UIM_MSG_HIGH_1("estk_scws_populate_open_ch_table: index=0x%x", index);

  if (index >= ESTK_MAX_BIP_CH)
  {
    UIM_MSG_ERR_1("Invalid ch idx: 0x%x", index);
    return ESTK_ERROR;
  }

  memset(&gstk_scws_cmd, 0x00, sizeof(gstk_cmd_from_card_type));
  memset(&find_cli_reg, 0x00, sizeof(gstk_io_find_client_with_registered_event_in_param_type));
  memset(&call_cli_evt_cb, 0x00, sizeof(call_cli_evt_cb));

  open_ch_cmd = &estk_curr_inst_ptr->curr_cmd.cmd.open_ch_pro_cmd_req;
  estk_curr_inst_ptr->bip_channel_table[index].current_command_id = GSTK_OPEN_CH_REQ;
  estk_curr_inst_ptr->bip_channel_table[index].status = ESTK_CH_PENDING;
  estk_curr_inst_ptr->bip_channel_table[index].channel_id = (index + 1);
  estk_curr_inst_ptr->bip_channel_table[index].transport_protocol =
    open_ch_cmd->transport_level.transport_protocol;
  estk_curr_inst_ptr->bip_channel_table[index].port_number =
    uint32touint16(open_ch_cmd->transport_level.port_number);
  if(open_ch_cmd->transport_level.transport_protocol
     == GSTK_PKT_DATA_TCP_UICC_SERVER_MODE)
  {
     estk_curr_inst_ptr->bip_channel_table[index].is_scws_ch = TRUE;
  }
  else
  {
    estk_curr_inst_ptr->bip_channel_table[index].is_scws_ch = FALSE;
  }
  /* Handle buffer size and if the command parameters are modified */
  if(open_ch_cmd->buffer_size <= ESTK_BIP_MAX_BUFFER_SIZE)
  {
    estk_curr_inst_ptr->bip_channel_table[index].buffer_size = open_ch_cmd->buffer_size;
    estk_curr_inst_ptr->bip_channel_table[index].open_ch_param_modified = FALSE;
  }
  else
  {
    estk_curr_inst_ptr->bip_channel_table[index].buffer_size = ESTK_BIP_MAX_BUFFER_SIZE;
    estk_curr_inst_ptr->bip_channel_table[index].open_ch_param_modified = TRUE;
  }

  /* Handle memory allocation for the receive buffer */
  if(estk_curr_inst_ptr->bip_channel_table[index].recv_buffer != NULL)
  {
    /* Free dangling pointer if any */
    gstk_free(estk_curr_inst_ptr->bip_channel_table[index].recv_buffer);
  }

  estk_curr_inst_ptr->bip_channel_table[index].recv_buffer = (uint8 *)GSTK_CALLOC(
    (estk_curr_inst_ptr->bip_channel_table[index].buffer_size));
  if(estk_curr_inst_ptr->bip_channel_table[index].recv_buffer == NULL)
  {
    return ESTK_ERROR;
  }

  if(estk_curr_inst_ptr->bip_channel_table[index].send_buffer != NULL)
  {
    /* Free dangling pointer if any */
    gstk_free(estk_curr_inst_ptr->bip_channel_table[index].send_buffer);
  }

  estk_curr_inst_ptr->bip_channel_table[index].send_buffer = (uint8 *)GSTK_CALLOC(
    (estk_curr_inst_ptr->bip_channel_table[index].buffer_size));
  if(estk_curr_inst_ptr->bip_channel_table[index].send_buffer == NULL)
  {
    return ESTK_ERROR;
  }

  estk_curr_inst_ptr->bip_channel_table[index].send_buf_len = 0;
  estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len = 0;
  estk_curr_inst_ptr->bip_channel_table[index].send_data_timer_expired = FALSE;

  estk_curr_inst_ptr->bip_channel_table[index].receive_data_allowed = TRUE;

  find_cli_reg.cli_func_type = GSTK_HANDLE_DISPLAY_FUNC_ONLY;
  find_cli_reg.cmd_type = GSTK_SCWS_OPEN_CH_REQ;
  find_cli_reg.format_type = GSTK_GSTK_FORMAT;
  gstk_status = gstk_io_ctrl(
                  GSTK_IO_FIND_CLIENT_WITH_REGISTERED_EVENT,
                  (void *)&find_cli_reg,
                  (void *)&client_id);
  if(gstk_status != GSTK_SUCCESS)
  {
    return ESTK_ERROR;
  }
  else
  {
    call_cli_evt_cb.client_id = client_id;
    call_cli_evt_cb.gstk_req_ptr = (gstk_cmd_from_card_type*)&gstk_scws_cmd;
    gstk_scws_cmd.hdr_cmd.command_id = GSTK_SCWS_OPEN_CH_REQ;
    gstk_scws_cmd.hdr_cmd.cmd_detail_reference = estk_curr_inst_ptr->curr_cmd.hdr_cmd.cmd_detail_reference;
    gstk_scws_cmd.hdr_cmd.sim_slot_id =  estk_curr_inst_ptr->curr_cmd.hdr_cmd.sim_slot_id;
    gstk_scws_cmd.hdr_cmd.command_number = estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_number;
    gstk_scws_cmd.hdr_cmd.user_data = estk_curr_inst_ptr->curr_cmd.hdr_cmd.user_data;

    gstk_scws_cmd.cmd.scws_open_ch_cmd_req.ch_id       = (uint32)(index+1);
    gstk_scws_cmd.cmd.scws_open_ch_cmd_req.buffer_size = estk_curr_inst_ptr->bip_channel_table[index].buffer_size;
    gstk_scws_cmd.cmd.scws_open_ch_cmd_req.port_number = estk_curr_inst_ptr->bip_channel_table[index].port_number;
    gstk_scws_cmd.cmd.scws_open_ch_cmd_req.alpha.alphabet_given = open_ch_cmd->alpha.alphabet_given;
    estk_curr_inst_ptr->curr_bip_cmd_index = (int8)index;

    if(open_ch_cmd->alpha.length > 0)
    {
      gstk_scws_cmd.cmd.scws_open_ch_cmd_req.alpha.length = int32touint8(open_ch_cmd->alpha.length);
      gstk_scws_cmd.cmd.scws_open_ch_cmd_req.alpha.alphabet =
        (uint16 *)GSTK_CALLOC(gstk_scws_cmd.cmd.scws_open_ch_cmd_req.alpha.length * 2 );
      if(gstk_scws_cmd.cmd.scws_open_ch_cmd_req.alpha.alphabet == NULL)
      {
        gstk_scws_cmd.cmd.scws_open_ch_cmd_req.alpha.length = 0;
      }
      else
      {
        for(i=0; i < gstk_scws_cmd.cmd.scws_open_ch_cmd_req.alpha.length; i++)
        {
          gstk_scws_cmd.cmd.scws_open_ch_cmd_req.alpha.alphabet[i] = uint16touint8(open_ch_cmd->alpha.alphabet[i]);
          UIM_MSG_HIGH_2("BIP alpha 8 bit = 0x%x , 16 bit = 0x%x",
                         gstk_scws_cmd.cmd.scws_open_ch_cmd_req.alpha.alphabet[i],
                         open_ch_cmd->alpha.alphabet[i]);
        }
      }
    }

    if (gstk_io_ctrl(
            GSTK_IO_CALL_CLIENT_EVT_CB,
            (void *)&call_cli_evt_cb,
            (void *)&user_data /* dummy data*/) != GSTK_SUCCESS)
    {
      UIM_MSG_ERR_0("can't call cli evt cb!");
      return ESTK_ERROR;
    }
    if(gstk_scws_cmd.cmd.scws_open_ch_cmd_req.alpha.alphabet != NULL)
    {
      gstk_free(gstk_scws_cmd.cmd.scws_open_ch_cmd_req.alpha.alphabet);
      gstk_scws_cmd.cmd.scws_open_ch_cmd_req.alpha.alphabet = NULL;
      gstk_scws_cmd.cmd.scws_open_ch_cmd_req.alpha.length = 0;
    }
  }
  return ESTK_SUCCESS;
} /* estk_scws_populate_open_ch_table */

/*===========================================================================
FUNCTION: estk_check_scws_capability

DESCRIPTION :
  Checks the SCWS capability

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  TRUE

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_check_scws_capability(void)
{
  return TRUE;
} /* estk_check_scws_capability */

#endif /* FEATURE_ESTK */
