/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                         G S T K     M M G S D I                               
   

GENERAL DESCRIPTION

  This source file contains functions related to intraction between GSTK and MMGSDI.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2018 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/gstk/src/gstk_mmgsdi.c#1 $ $DateTime: 2020/07/22 10:53:27 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/12/18   ks      Split ME and slot specific NV
07/09/18   tq      Initial Version


===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/* Command/Msg Headers/Memory */
#include "err.h"
#include "queue.h"
#include "memheap.h"
#include "modem_mem.h"
#include "uim_msg.h"

/* GSTK Specific Headers */
#include "gstk_nv_p.h"
#include "gstk.h"
#include "gstki.h"
#include "gstkutil.h"
#include "gstk_nv.h"

/* GSTK -> GSDI Interface */
#include "mmgsdisessionlib_v.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/* Queue to hold commands for gstk */
extern q_type gstk_task_cmd_q;

void gstk_mmgsdi_events_cb(const mmgsdi_event_data_type *evt_ptr);

/*===========================================================================
FUNCTION GSTK_MMGSDI_CMD_CB

DESCRIPTION
  GSTK Callback for Client Id Reg and Refresh from MMGSDI

PARAMETER
  status  [Input]: status of command returned from MMGSDI
  cnf     [Input]: confirmation type
  cnf_ptr [Input]: confirmation pointer

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_mmgsdi_cmd_cb(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
)
{
  gstk_status_enum_type   gstk_status          = GSTK_SUCCESS;
  gstk_task_cmd_type     *task_cmd_ptr         = NULL;
  boolean                 open_ch_requeue_flag = FALSE;
  gstk_slot_id_enum_type  slot                 = GSTK_SLOT_ID_MAX;

  if(cnf_ptr == NULL)
  {
    return;
  }
  
  slot = (gstk_slot_id_enum_type)cnf_ptr->response_header.slot_id;
  
  /* Validate Slot ID */
  if(!GSTK_IS_VALID_SLOT_ID(slot))
  {
    return;
  }

  switch(cnf)
  {
    case MMGSDI_REFRESH_CNF:
      gstk_status = gstk_util_compare_ref_id(cnf_ptr->refresh_cnf.response_header.client_data);
      if(gstk_status != GSTK_SUCCESS)
      {
        break;
      }
      gstk_status = gstk_handle_mmgsdi_refresh_cnf(status, &(cnf_ptr->refresh_cnf));
      break;
    case MMGSDI_READ_CNF:
      UIM_MSG_HIGH_2("mmgsdi read cnf: 0x%x, 0x%x",
                     status, cnf_ptr->read_cnf.access.file.file_enum);

      if(cnf_ptr->response_header.client_data != (uint32)gstk_instances_ptr[(uint8)slot - 1]->open_ch_imsi_m_ptr)
      {
        gstk_status = GSTK_BAD_PARAM;
        break;
      }

      if (status == MMGSDI_SUCCESS &&
          cnf_ptr->read_cnf.access.access_method == MMGSDI_BY_PATH_ACCESS &&
          cnf_ptr->read_cnf.access.file.path_type.path_len == 0x3 &&
          cnf_ptr->read_cnf.access.file.path_type.path_buf[0] == GSTK_CSIM_IMSI_M_PATH_BUF_1 &&
          cnf_ptr->read_cnf.access.file.path_type.path_buf[1] == GSTK_CSIM_IMSI_M_PATH_BUF_2 &&
          cnf_ptr->read_cnf.access.file.path_type.path_buf[2] == GSTK_CSIM_IMSI_M_PATH_BUF_3)
      {
        if (cnf_ptr->read_cnf.read_data.data_len > 8 &&
            !(cnf_ptr->read_cnf.read_data.data_ptr[7] & 0x80))
        {  
          /* based on 3GPP2 C.S0065, bit 8 of byte 8 of IMIS_M is not set,
             IMSI_M is not programmed */
          task_cmd_ptr = gstk_task_get_cmd_buf();
          if (task_cmd_ptr != NULL)
          {
            task_cmd_ptr->cmd.general_cmd.message_header.command_group = GSTK_SETUP_OTASP_CALL_CMD;
            task_cmd_ptr->cmd.general_cmd.message_header.command_id = GSTK_OTASP_CALL_ORIG;
            task_cmd_ptr->cmd.general_cmd.message_header.sim_slot_id = slot;
            task_cmd_ptr->cmd.general_cmd.message_header.user_data =
              cnf_ptr->response_header.client_data;
            gstk_task_put_cmd_buf(task_cmd_ptr);
          }
          else
          {
            break;
          }
        }
        else
        {
          UIM_MSG_HIGH_0("IMSI_M provisioned, requeue Open Ch cmd from mgmsdi context");
          gstk_shared_data.gstk_is_imsi_m_provisioned = TRUE;
          open_ch_requeue_flag = TRUE;
        }
      }
      else if(status != MMGSDI_SUCCESS &&
        cnf_ptr->read_cnf.access.access_method == MMGSDI_BY_PATH_ACCESS &&
        cnf_ptr->read_cnf.access.file.path_type.path_len == 0x3 &&
        cnf_ptr->read_cnf.access.file.path_type.path_buf[0] == GSTK_CSIM_IMSI_M_PATH_BUF_1 &&
        cnf_ptr->read_cnf.access.file.path_type.path_buf[1] == GSTK_CSIM_IMSI_M_PATH_BUF_2 &&
        cnf_ptr->read_cnf.access.file.path_type.path_buf[2] == GSTK_CSIM_IMSI_M_PATH_BUF_3)
      {
        UIM_MSG_ERR_0("MMSGDI read for CSIM IMSI_M failed");
        open_ch_requeue_flag = TRUE;
      }

      if(open_ch_requeue_flag == TRUE)
      {
        task_cmd_ptr = gstk_task_get_cmd_buf();
        if (task_cmd_ptr != NULL)
        {
          /* Copy over the open channel command to requeue*/
          (void)gstk_memcpy((void *)&(task_cmd_ptr->cmd.general_cmd),
                            (void *)cnf_ptr->response_header.client_data,
                            sizeof(gstk_proactive_cmd_type),
                            sizeof(gstk_proactive_cmd_type),
                            sizeof(gstk_proactive_cmd_type));
		    
          if(task_cmd_ptr->cmd.general_cmd.message_header.command_group == GSTK_PROACTIVE_CMD
               && task_cmd_ptr->cmd.general_cmd.message_header.command_id == 0)
          {
            task_cmd_ptr->cmd.general_cmd.message_header.command_id = GSTK_PRO_CMD_ID_OPEN_CH_IMSI_M_READ_FAIL;
            task_cmd_ptr->cmd.general_cmd.message_header.sim_slot_id = slot;
            gstk_task_put_cmd_buf(task_cmd_ptr);
          }
          else
          {
            gstk_task_free_cmd_buf(task_cmd_ptr);
          }
        }
        gstk_free((void *)cnf_ptr->response_header.client_data);
      }
      break;

    case MMGSDI_GSTK_LOC_ENV_RSP_RECEIVED_CNF:
      /* If IMSI is switched: Send NOT allowed to to NAS */
      /* If IMSI in not switched - Allow NAS */
      if(gstk_nv_get_feature_status(
           GSTK_CFG_FEATURE_SLOT_PROP_LOC_STATUS,
           (gstk_slot_id_enum_type)slot) == TRUE)
      {
        task_cmd_ptr = gstk_task_get_cmd_buf();
        if (task_cmd_ptr != NULL)
        {
          task_cmd_ptr->cmd.loc_env_rsp_rcvd_cnf.message_header.command_group = GSTK_MMGSDI_CMD;
          task_cmd_ptr->cmd.loc_env_rsp_rcvd_cnf.message_header.command_id = GSTK_MMGSDI_LOC_ENV_RSP_RCVD_CNF_EVT;
          task_cmd_ptr->cmd.loc_env_rsp_rcvd_cnf.message_header.sim_slot_id = slot;
          task_cmd_ptr->cmd.loc_env_rsp_rcvd_cnf.imsi_switched =
            cnf_ptr->gstk_loc_env_rsp_received_cnf.imsi_switched;
          gstk_task_put_cmd_buf(task_cmd_ptr);
        }
      }
      break;

    default:
      break;
  }
}/* gstk_mmgsdi_cmd_cb */

/*===========================================================================
FUNCTION GSTK_FIND_MMGSDI_CMD

DESCRIPTION
  This function is called by q_linear_delete_new. This function will return TRUE only when the
  command id is that of a GSTK_MMGSDI_CMD.

PARAMETER
  item_ptr  [Input]:  ptr to queue item
  param_ptr [Input]:  ptr to seq num

DEPENDENCIES
  None

RETURN VALUE
  1 - if there is a match
  0 - if there is no match

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static int gstk_find_mmgsdi_cmd(
   void *item_ptr,
   void *param_ptr
)
{
  gstk_task_cmd_type     *task_cmd_ptr   = NULL;
  gstk_slot_id_enum_type  slot           = GSTK_SLOT_ID_MAX;

  if(item_ptr == NULL || param_ptr == NULL)
  {
    return 0;
  }

  task_cmd_ptr = (gstk_task_cmd_type*)item_ptr;
  slot = *((gstk_slot_id_enum_type *)param_ptr);

  if(task_cmd_ptr->cmd.general_cmd.message_header.command_group == GSTK_MMGSDI_CMD &&
     task_cmd_ptr->cmd.general_cmd.message_header.sim_slot_id == slot )
  {
    UIM_MSG_HIGH_1("GSTK_MMGSDI_CMD  of cmd id 0x%x found for deletion",
                   task_cmd_ptr->cmd.general_cmd.message_header.command_id);
    return 1;
  }
  else
  {
    return 0;
  }
} /* gstk_find_mmgsdi_cmd */

/*===========================================================================
FUNCTION GSTK_MMGSDI_EVENTS_CB

DESCRIPTION
  GSTK Callback for events from MMGSDI

PARAMETER
  evt_ptr [Input]: the MMGSDI event received

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_mmgsdi_events_cb (
  const mmgsdi_event_data_type *evt_ptr
)
{
  gstk_task_cmd_type        *task_cmd_ptr    = NULL;
  gstk_mmgsdi_evt_enum_type  gstk_mmgsdi_evt = GSTK_MMGSDI_EVT_MAX;
  gstk_slot_id_enum_type     slot            = GSTK_SLOT_AUTO;

  if(evt_ptr == NULL) 
  {
    return;
  }

  switch (evt_ptr->evt)
  {
    case MMGSDI_CARD_INSERTED_EVT:
      gstk_mmgsdi_evt = GSTK_MMGSDI_CARD_INSERTED_EVT;
      slot = (gstk_slot_id_enum_type)evt_ptr->data.card_inserted.slot;
      break;

    case MMGSDI_SESSION_CHANGED_EVT:
      gstk_mmgsdi_evt = GSTK_MMGSDI_SESSION_CHANGED_EVT;
      slot = (gstk_slot_id_enum_type)evt_ptr->data.session_changed.app_info.slot;
      break;

    case MMGSDI_SUBSCRIPTION_READY_EVT:
      gstk_mmgsdi_evt = GSTK_MMGSDI_SUBSCRIPTION_READY_EVT;
      slot = (gstk_slot_id_enum_type)evt_ptr->data.subscription_ready.app_info.slot;
      break;

    case MMGSDI_REFRESH_EVT:
      gstk_mmgsdi_evt = GSTK_MMGSDI_REFRESH_EVT;
      slot = (gstk_slot_id_enum_type)evt_ptr->data.refresh.slot;
      break;

    case MMGSDI_CARD_ERROR_EVT:
      gstk_mmgsdi_evt = GSTK_MMGSDI_CARD_ERROR_EVT;
      slot = (gstk_slot_id_enum_type)evt_ptr->data.card_error.slot;
      /* Entering gstk critical section */
      rex_enter_crit_sect(&gstk_shared_data.gstk_crit_sect);
      /* Find and delete pending MMGSDI commands */
      while( (task_cmd_ptr = (gstk_task_cmd_type*) q_linear_delete_new (&gstk_task_cmd_q,
                                                     gstk_find_mmgsdi_cmd,
                                                     &slot,
                                                     NULL,
                                                     NULL)) != NULL)
      {
        gstk_task_free_cmd_buf(task_cmd_ptr);
      }
      rex_leave_crit_sect(&gstk_shared_data.gstk_crit_sect);
      task_cmd_ptr = NULL;
      break;

    default:
      break;
  }

  if(!GSTK_IS_VALID_SLOT_ID(slot))
  {
    return;
  }

  task_cmd_ptr = gstk_task_get_cmd_buf();
  if (task_cmd_ptr != NULL)
  {
    task_cmd_ptr->cmd.mmgsdi_evt.message_header.command_group = GSTK_MMGSDI_CMD;
    task_cmd_ptr->cmd.mmgsdi_evt.message_header.command_id    = gstk_mmgsdi_evt;
    task_cmd_ptr->cmd.mmgsdi_evt.message_header.sim_slot_id   = slot;
    task_cmd_ptr->cmd.mmgsdi_evt.message_header.user_data     = 0;
    task_cmd_ptr->cmd.mmgsdi_evt.message_header.client_id     = 0;
    task_cmd_ptr->cmd.mmgsdi_evt.session_id                   = evt_ptr->session_id;
    (void)gstk_memcpy(&task_cmd_ptr->cmd.mmgsdi_evt.evt,
                      &evt_ptr->data,
                      sizeof(task_cmd_ptr->cmd.mmgsdi_evt.evt),
                      sizeof(task_cmd_ptr->cmd.mmgsdi_evt.evt),
                      sizeof(evt_ptr->data));

    if(evt_ptr->evt == MMGSDI_CARD_ERROR_EVT)
    {
      gstk_task_put_cmd_priority_buf(task_cmd_ptr);
    }
    else
    {
      gstk_task_put_cmd_buf(task_cmd_ptr);
    }
  }
} /*gstk_mmgsdi_events_cb*/

/*===========================================================================
FUNCTION gstk_process_mmgsdi_evt

DESCRIPTION
  Process MMGSDI events

PARAMETERS
  cmd_ptr: [Input] MMGSDI event ptr

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_process_mmgsdi_evt(
  gstk_cmd_type *cmd_ptr
)
{
  uint32                  i             = 0;
  gstk_slot_id_enum_type  slot          = GSTK_SLOT_AUTO;
  gstk_status_enum_type   gstk_status   = GSTK_SUCCESS;
  gstk_cmd_from_card_type response;

  GSTK_RETURN_IF_NULL_PTR(gstk_curr_inst_ptr);

  if(cmd_ptr == NULL)
  {
    return;
  }

  UIM_MSG_HIGH_1("gstk_process_mmgsdi_evt(): evt 0x%x",
                 cmd_ptr->mmgsdi_evt.message_header.command_id);

  switch (cmd_ptr->mmgsdi_evt.message_header.command_id)
  {
    case GSTK_MMGSDI_CARD_ERROR_EVT:
      slot = (gstk_slot_id_enum_type)cmd_ptr->mmgsdi_evt.evt.card_error.slot;
      /* Card error may come during recovery is in process */
      if(gstk_curr_inst_ptr->gstk_sub_state == GSTK_RECOVERY_SUB_S)
      {
        /* Move to non recovery state */
        gstk_curr_inst_ptr->gstk_sub_state = GSTK_INVALID_SUB_S;
      }

      /* Find and process any pending envelope responses and terminal response report in gstk_cmd_q */
      gstk_util_find_and_process_pending_responses();

      /* Cleanup pending envelope commands by sending error envelope response */
      for(i = 0; i < GSTK_MAX_ENVELOPE_COMMAND; ++i)
      {
        if(gstk_util_get_sim_slot_id(gstk_shared_data.envelope_cmd_ref_table[i].client_id) == slot &&
           !gstk_shared_data.envelope_cmd_ref_table[i].is_free )
        {
          if(gstk_util_build_error_envelope_response(i) != GSTK_SUCCESS)
          {
            gstk_util_env_cmd_ref_table_free_slot(i);
          }
        }
      }

      /* Clear retry data since we already cleared envelope_cmd_ref_table */
      gstk_util_clear_retry_envelope_data(slot);

      /* Clean up Pending proactive command */
      for(i = 0; i < GSTK_MAX_PROACTIVE_COMMAND; ++i)
      {
        if ((!command_details_ref_table_ptr[i].free_slot) &&
            (command_details_ref_table_ptr[i].sim_slot_id ==  slot) )
        {
          gstk_util_cmd_details_ref_table_free_slot(i);
          /* Only one proactive command can be processed at a time */
          break;
        }
      }

      if(gstk_curr_inst_ptr->gstk_proactive_cmd_in_progress == TRUE)
      {
        if(gstk_nv_get_me_feature_status(GSTK_CFG_FEATURE_ME_REPORT_SW1_SW2) == TRUE)
        {
          if(gstk_curr_inst_ptr->tr_unique_ref_id != 0)
          {
            /* TR already sent to uimdrv, check whether it is sent by GSTK or client */
            gstk_client_id_type client_id  = GSTK_CMD_REF_TABLE_INDEX(gstk_curr_inst_ptr->tr_unique_ref_id);
            if(client_id != gstk_curr_inst_ptr->client_id)
            {
               gstk_util_send_sw_to_client(NULL);
            }
          }
          else
          {
            /* check the queue for pending RAW TR */
            gstk_task_cmd_type *cmd_p = gstk_util_find_terminal_rsp_in_queue();
            if(cmd_p != NULL)
            {
              if(cmd_p->cmd.general_cmd.message_header.command_id == GSTK_RAW_TERMIMAL_RESPONSE)
              {
                gstk_client_raw_term_resp_type   *raw_term_resp_ptr = &cmd_p->cmd.client_raw_term_resp;
                gstk_shared_data.gstk_client_table[raw_term_resp_ptr->client_id - 1].user_data =
                                                          raw_term_resp_ptr->message_header.user_data;
                /* TR is not generated by GSTK */
                gstk_util_send_sw_to_client(NULL);
              }
              gstk_task_free_cmd_buf(cmd_p);
            }
          }
        }
        /* find and send end proactive seesion for pending proactive command */
        gstk_util_find_and_build_proactive_end_session(GSTK_TAG_CARD_ERROR);
      }

      /* Move to GSTK_NO_SIM state */
      gstk_curr_inst_ptr->gstk_state = GSTK_NO_SIM_S;

      /* Clean up the GSTK Global data */
      gstk_util_clean_up_gstk_global_data(slot, GSTK_TAG_CARD_ERROR);
      break;

    case GSTK_MMGSDI_TERMINAL_PROFILE_DL_EVT:
      memset(&response, 0x00, sizeof(gstk_cmd_from_card_type));

      if(cmd_ptr->mmgsdi_evt.evt.terminal_profile.ok_dl)
      {
        /* use the gstk_terminal_profile_cache info and send it to uim */
        gstk_status = gstk_pack_tp_cmd_to_uim(cmd_ptr->mmgsdi_evt.evt.terminal_profile.exit_from_psm_status);
        if (gstk_status == GSTK_SUCCESS)
        {
          gstk_curr_inst_ptr->tp_info.user_data = cmd_ptr->mmgsdi_evt.message_header.user_data;
          gstk_curr_inst_ptr->tp_info.gstk_tp_dl_cb = cmd_ptr->mmgsdi_evt.client_cb;
          /* go to wait for TP response state */
          gstk_curr_inst_ptr->gstk_state = GSTK_TERM_PROFILE_RSP_WAIT_S;
        }
        else
        {
          UIM_MSG_ERR_1("Unbale to send TP to card in state 0x%x",
                        gstk_curr_inst_ptr->gstk_state);
          gstk_curr_inst_ptr->gstk_state        = GSTK_NO_SIM_S;
          /* Unblock MMGSDI */
          response.hdr_cmd.command_id           = GSTK_PROFILE_DL_IND_RSP;
          response.hdr_cmd.sim_slot_id          = cmd_ptr->mmgsdi_evt.message_header.sim_slot_id;
          response.hdr_cmd.cmd_detail_reference = 0;
          response.hdr_cmd.command_number       = 0;
          response.hdr_cmd.user_data            = cmd_ptr->mmgsdi_evt.message_header.user_data;
          if(cmd_ptr->mmgsdi_evt.client_cb != NULL)
          {
            (cmd_ptr->mmgsdi_evt.client_cb)(&response);
          }
        }
      }
      else
      {
        gstk_curr_inst_ptr->gstk_state        = GSTK_NO_SIM_S;
        /* Unblock MMGSDI */
        response.hdr_cmd.command_id           = GSTK_PROFILE_DL_IND_RSP;
        response.hdr_cmd.sim_slot_id          = cmd_ptr->mmgsdi_evt.message_header.sim_slot_id;
        response.hdr_cmd.cmd_detail_reference = 0;
        response.hdr_cmd.command_number       = 0;
        response.hdr_cmd.user_data            = cmd_ptr->mmgsdi_evt.message_header.user_data;
        if(cmd_ptr->mmgsdi_evt.client_cb != NULL)
        {
          (cmd_ptr->mmgsdi_evt.client_cb)(&response);
        }
      }
      break;

    case GSTK_MMGSDI_CARD_INSERTED_EVT:
      gstk_get_and_store_curr_card_protocol();

      /* If NV items had been updated due to non-standard NV refresh in
         the previous card power-cycle, re-read the same.
         Card Inserted event handling is the earliest that this re-read
         can be attempted in a card power-cycle for ICC cards */
      if(gstk_curr_inst_ptr->card_protocol == MMGSDI_ICC)
      {
        gstk_nv_init_slot_cache(
          GSTK_NV_INIT_CATEG_POWER_UP_INIT,
          gstk_curr_inst_ptr->slot_id);
      }
      break;

    case GSTK_MMGSDI_SESSION_CHANGED_EVT:
      gstk_process_session_changed_evt(cmd_ptr);
      break;

    case GSTK_MMGSDI_SUBSCRIPTION_READY_EVT:
      for (i = 0; i < GSTK_MMGSDI_SESS_INFO_SIZE; ++i)
      {
        if (gstk_shared_data.mmgsdi_sess_info[i].sess_id ==
            cmd_ptr->mmgsdi_evt.session_id)
        {
          if((gstk_shared_data.mmgsdi_sess_info[i].sess_type == MMGSDI_GW_PROV_PRI_SESSION) ||
             (gstk_shared_data.mmgsdi_sess_info[i].sess_type == MMGSDI_GW_PROV_SEC_SESSION) ||
             (gstk_shared_data.mmgsdi_sess_info[i].sess_type == MMGSDI_GW_PROV_TER_SESSION))
          {
            gstk_curr_inst_ptr->is_subs_ready.subs_ready_gw = TRUE;
          }
          else if((gstk_shared_data.mmgsdi_sess_info[i].sess_type == MMGSDI_1X_PROV_PRI_SESSION) ||
                  (gstk_shared_data.mmgsdi_sess_info[i].sess_type == MMGSDI_1X_PROV_SEC_SESSION) ||
                  (gstk_shared_data.mmgsdi_sess_info[i].sess_type == MMGSDI_1X_PROV_TER_SESSION))
          {
            gstk_curr_inst_ptr->is_subs_ready.subs_ready_1x = TRUE;
          }

          if (gstk_nv_get_feature_status(
                GSTK_CFG_FEATURE_SLOT_PROP_LOC_STATUS,
                gstk_curr_inst_ptr->slot_id) == TRUE)
          {
            if (gstk_curr_inst_ptr->imsi_switched)
            {
              gstk_curr_inst_ptr->imsi_switched = FALSE;
              UIM_MSG_HIGH_0("loc_info_env_res = GSTK_ATTACH_CTRL_ALLOWED");
              gstk_curr_inst_ptr->loc_info_env_res = GSTK_ATTACH_CTRL_ALLOWED;
            }
          }
          gstk_shared_data.mmgsdi_sess_info[i].slot =
            (gstk_slot_id_enum_type)cmd_ptr->mmgsdi_evt.evt.subscription_ready.app_info.slot;
        } /* End of - if */
      } /* End of - for (i = 0; i < GSTK_MMGSDI_SESS_INFO_SIZE; ++i)*/
      break;

    case GSTK_MMGSDI_REFRESH_EVT:
      if(cmd_ptr->mmgsdi_evt.evt.refresh.stage == MMGSDI_REFRESH_STAGE_START)
      {
        slot = cmd_ptr->mmgsdi_evt.evt.refresh.slot;
        if((cmd_ptr->mmgsdi_evt.evt.refresh.mode == MMGSDI_REFRESH_RESET) ||
           (cmd_ptr->mmgsdi_evt.evt.refresh.mode == MMGSDI_REFRESH_RESET_AUTO))
        {
          /* Find and process any pending envelope responses and terminal response report in gstk_cmd_q */
          gstk_util_find_and_process_pending_responses();

          /* Cleanup pending envelope commands by sending error envelope response */
          for(i = 0; i < GSTK_MAX_ENVELOPE_COMMAND; ++i)
          {
            if(gstk_util_get_sim_slot_id(gstk_shared_data.envelope_cmd_ref_table[i].client_id) == slot &&
               !gstk_shared_data.envelope_cmd_ref_table[i].is_free)
            {
              if(gstk_util_build_error_envelope_response(i) != GSTK_SUCCESS)
              {
                gstk_util_env_cmd_ref_table_free_slot(i);
              }
            }
          }

          /* Clean up Pending proactive command */
          for(i = 0; i < GSTK_MAX_PROACTIVE_COMMAND; ++i)
          {
            if ((!command_details_ref_table_ptr[i].free_slot) &&
                (command_details_ref_table_ptr[i].sim_slot_id ==  slot) )
            {
              gstk_util_cmd_details_ref_table_free_slot(i);
              /* Only one proactive command can be processed at a time */
              break;
            }
          }

          if(gstk_curr_inst_ptr->gstk_proactive_cmd_in_progress == TRUE)
          {
            /* find and send end proactive seesion for pending proactive command */
            gstk_util_find_and_build_proactive_end_session(GSTK_TAG_REFRESH_RESET);
          }

          /* Move to GSTK_NO_SIM state */
          gstk_curr_inst_ptr->gstk_state = GSTK_NO_SIM_S;
          gstk_util_clean_up_gstk_global_data(slot, GSTK_TAG_REFRESH_RESET);
        }
        else if((cmd_ptr->mmgsdi_evt.evt.refresh.mode == MMGSDI_REFRESH_NAA_INIT_FULL_FCN) ||
                (cmd_ptr->mmgsdi_evt.evt.refresh.mode == MMGSDI_REFRESH_NAA_INIT_FCN) ||
                (cmd_ptr->mmgsdi_evt.evt.refresh.mode == MMGSDI_REFRESH_NAA_INIT) ||
                (cmd_ptr->mmgsdi_evt.evt.refresh.mode == MMGSDI_REFRESH_NAA_APP_RESET) ||
                (cmd_ptr->mmgsdi_evt.evt.refresh.mode == MMGSDI_REFRESH_3G_SESSION_RESET))
        {
          if((cmd_ptr->mmgsdi_evt.evt.refresh.app_info.app_data.app_type == MMGSDI_APP_SIM)||
             (cmd_ptr->mmgsdi_evt.evt.refresh.app_info.app_data.app_type == MMGSDI_APP_RUIM))
          {
            gstk_util_clean_up_gstk_global_data(slot, GSTK_TAG_REFRESH_2G_INIT);
          }

          for (i = 0; i < GSTK_MMGSDI_SESS_INFO_SIZE; ++i)
          {
            if (gstk_shared_data.mmgsdi_sess_info[i].sess_id ==
                cmd_ptr->mmgsdi_evt.session_id)
            {
              if((gstk_shared_data.mmgsdi_sess_info[i].sess_type == MMGSDI_GW_PROV_PRI_SESSION) ||
                 (gstk_shared_data.mmgsdi_sess_info[i].sess_type == MMGSDI_GW_PROV_SEC_SESSION) ||
                 (gstk_shared_data.mmgsdi_sess_info[i].sess_type == MMGSDI_GW_PROV_TER_SESSION))
              {
                gstk_curr_inst_ptr->is_subs_ready.subs_ready_gw = FALSE;
              }
              else if((gstk_shared_data.mmgsdi_sess_info[i].sess_type == MMGSDI_1X_PROV_PRI_SESSION) ||
                 (gstk_shared_data.mmgsdi_sess_info[i].sess_type == MMGSDI_1X_PROV_SEC_SESSION) ||
                 (gstk_shared_data.mmgsdi_sess_info[i].sess_type == MMGSDI_1X_PROV_TER_SESSION))
              {
                gstk_curr_inst_ptr->is_subs_ready.subs_ready_1x = FALSE;
              }
            }
          }
        }
      }
      UIM_MSG_HIGH_2("SUBS_READY GW: 0x%x, 1x : 0x%x",
                   gstk_curr_inst_ptr->is_subs_ready.subs_ready_gw,
                   gstk_curr_inst_ptr->is_subs_ready.subs_ready_1x);
      break;

    case GSTK_MMGSDI_LOC_ENV_RSP_RCVD_CNF_EVT:
      UIM_MSG_HIGH_1("Received Location Env Rsp Rcvd conf cmd; imsi_switched: 0x%x",
                     cmd_ptr->loc_env_rsp_rcvd_cnf.imsi_switched);
      if (gstk_nv_get_feature_status(
            GSTK_CFG_FEATURE_SLOT_PROP_LOC_STATUS,
            gstk_curr_inst_ptr->slot_id) == TRUE)
      {
        gstk_curr_inst_ptr->imsi_switched =
        cmd_ptr->loc_env_rsp_rcvd_cnf.imsi_switched;
        if (cmd_ptr->loc_env_rsp_rcvd_cnf.imsi_switched)
        {
          gstk_curr_inst_ptr->loc_info_env_res = GSTK_ATTACH_CTRL_NOT_ALLOWED;
          gstk_curr_inst_ptr->attach_ctrl_res = GSTK_ATTACH_CTRL_NOT_ALLOWED;
          gstk_util_send_attach_ctrl_res();
        }
        else
        {
          gstk_curr_inst_ptr->loc_info_env_res = GSTK_ATTACH_CTRL_ALLOWED;
          gstk_curr_inst_ptr->attach_ctrl_res = GSTK_ATTACH_CTRL_ALLOWED;
          gstk_util_send_attach_ctrl_res();
        }
      }
      break;

    default:
      break;
  }
} /*gstk_process_mmgsdi_evt*/

/*===========================================================================
FUNCTION gstk_mmgsdi_init

DESCRIPTION
  Function for having GSTK register with MMGSDI and open prov and card sessions.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_mmgsdi_init(void)
{
  mmgsdi_return_enum_type         mmgsdi_status             = MMGSDI_SUCCESS;
  mmgsdi_client_id_type           mmgsdi_cl_id              = MMGSDI_CLIENT_ID_ZERO;
  uint32                          session_type_mask         = 0;
  mmgsdi_session_open_info_type  *mmgsdi_session_info_ptr   = NULL;
  uint8                           num_of_session_open       = 0;
  uint8                           i                         = 0;

  /* Get client id from mmgsdi */
  mmgsdi_cl_id = mmgsdi_client_id_and_evt_reg_sync(
                   gstk_mmgsdi_events_cb,
                   NULL,
                   0);
  if(mmgsdi_cl_id == MMGSDI_CLIENT_ID_ZERO)
  {
    UIM_MSG_ERR_0("mmgsdi_client_id_reg() returned error");
    return;
  }

  gstk_shared_data.gstk_mmgsdi_client_id = mmgsdi_cl_id;
  
  /* This is single SIM - 3GPP only target, so we will include mask accordingly */
  session_type_mask = MMGSDI_GW_PROV_PRI_SESSION_MASK |
                      MMGSDI_CARD_SESSION_SLOT_1_MASK;
                     
  /* Number of sessions need to be opened, we will allocate memory accordingly */
  num_of_session_open = 2;

  /* Allocate memory for session ptr */
  mmgsdi_session_info_ptr = GSTK_CALLOC(sizeof(mmgsdi_session_open_info_type) * num_of_session_open);
  if(mmgsdi_session_info_ptr == NULL)
  {
    return;
  }

  for (i = 0; i < GSTK_MMGSDI_SESS_INFO_SIZE; ++i)
  {
    gstk_shared_data.mmgsdi_sess_info[i].slot = GSTK_SLOT_ID_MAX;
    gstk_shared_data.mmgsdi_sess_info[i].sess_type =
      MMGSDI_MAX_SESSION_TYPE_ENUM;
  }

  mmgsdi_status = mmgsdi_session_open_ext_sync(
                    gstk_shared_data.gstk_mmgsdi_client_id,
                    session_type_mask,
                    gstk_mmgsdi_events_cb,
                    mmgsdi_session_info_ptr);
  
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("failed to open sessions, mmgsdi_status: 0x%x",
                  mmgsdi_status);
    gstk_free(mmgsdi_session_info_ptr);
    return;
  }

  /* Store the session id that were opened */
  for (i = 0; i < num_of_session_open; i++)
  {
    mmgsdi_session_type_enum_type session_type =
      mmgsdi_session_info_ptr[i].session_type;
    mmgsdi_session_id_type session_id =
      mmgsdi_session_info_ptr[i].session_id;

    switch (session_type)
    {
      case MMGSDI_GW_PROV_PRI_SESSION:
        gstk_shared_data.mmgsdi_sess_info[i].sess_id   = session_id;
        gstk_shared_data.mmgsdi_sess_info[i].sess_type = session_type;
        break;
      case MMGSDI_CARD_SESSION_SLOT_1:
        gstk_shared_data.mmgsdi_sess_info[i].slot = GSTK_SLOT_1;
        gstk_shared_data.mmgsdi_sess_info[i].sess_id   = session_id;
        gstk_shared_data.mmgsdi_sess_info[i].sess_type = session_type;
      default:
        break;
    }
  }
  
  gstk_free(mmgsdi_session_info_ptr);
} /* gstk_mmgsdi_init */


/*===========================================================================
FUNCTION gstk_get_and_store_curr_card_protocol

DESCRIPTION
  This function stores the current card protocol as obtained from MMGSDI

PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
void gstk_get_and_store_curr_card_protocol(void)
{
  mmgsdi_protocol_enum_type   protocol_data = MMGSDI_MAX_PROTOCOL_ENUM;
  mmgsdi_return_enum_type     mmgsdi_status = MMGSDI_ERROR;

  GSTK_RETURN_IF_NULL_PTR(gstk_curr_inst_ptr);

  mmgsdi_status = mmgsdi_get_protocol(
                    gstk_shared_data.gstk_mmgsdi_client_id,
                    (mmgsdi_slot_id_enum_type)gstk_util_get_sim_slot_id(gstk_curr_inst_ptr->client_id),
                    &protocol_data);
  
  if((MMGSDI_SUCCESS != mmgsdi_status) ||
  	 (protocol_data != MMGSDI_ICC && protocol_data != MMGSDI_UICC))
  {
    UIM_MSG_ERR_2("Could not get Card Protocol 0x%x, proto data 0x%x", 
                   mmgsdi_status, protocol_data);
    return;
  }

  gstk_curr_inst_ptr->card_protocol = protocol_data;
}/* gstk_get_and_store_curr_card_protocol */
