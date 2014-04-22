/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                 M M G S D I  E X T E R N A L  S I M L O C K  F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains functions to perform MMGSDI specific external SIM
  Lock tasks.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/mmgsdi/src/mmgsdi_external_simlock.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/24/18   rps     Refragment alloc extra param code
05/16/16   ar      F3 reduction
04/14/16   vdc     Initial version
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "uim_msg.h"
#include "mmgsdiutil.h"
#include "mmgsdi_evt.h"
#include "mmgsdi_card_init.h"
#include "mmgsdi_cnf.h"

/*===========================================================================
                  F U N C T I O N   P R O T O T Y P E S
============================================================================*/

/* ==========================================================================
FUNCTION:      MMGSDI_EXTERNAL_SIMLOCK_SUB_OK

DESCRIPTION:
  This function can be called by a client to inform MMGSDI that the subscription
  is OK and it can proceed with publishing the subscription to all clients thus
  allowing the modem to move to full service

DEPENDENCIES:
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

RETURN VALUE:
  mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_external_simlock_sub_ok(
  mmgsdi_session_id_type         session_id,
  boolean                        ok_for_subscription,
  mmgsdi_callback_type           response_cb_ptr,
  mmgsdi_client_data_type        client_ref
)
{
  mmgsdi_return_enum_type            mmgsdi_status  = MMGSDI_ERROR;
  mmgsdi_task_cmd_type              *task_cmd_ptr   = NULL;
  mmgsdi_session_sub_ok_req_type    *msg_ptr        = NULL;
  
  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Session ID
    2) Response CB function Check
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_session_id_valid_in_cmd(
                    session_id,
                    MMGSDI_SESSION_SUB_OK_REQ);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  /*---------------------------------------------------------------------------
    Memory allocation for msg_ptr and check for allocation
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, sizeof(mmgsdi_task_cmd_type));
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  msg_ptr = &task_cmd_ptr->cmd.cmd.session_sub_ok_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_session_sub_ok_req_type header
      1) Session ID
      2) Request TYPE
      3) Client Data Pointer
      4) Response CallBack
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_SESSION_SUB_OK_REQ;
  msg_ptr->request_header.session_id        = session_id;
  msg_ptr->request_header.request_type      = MMGSDI_SESSION_SUB_OK_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_SESSION_SUB_OK_REQ;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  msg_ptr->ok_for_sub = ok_for_subscription;

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put
       onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_SESSION_SUB_OK_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
} /* mmgsdi_external_simlock_sub_ok */


/*===========================================================================
FUNCTION MMGSDI_EXTERNAL_SIMLOCK_SUB_OK_PERSO_UPDATE

DESCRIPTION
  Function to update the state or global info as needed when external perso
  sub_ok is received

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static void mmgsdi_external_simlock_sub_ok_perso_update(
  mmgsdi_session_id_type        session_id,
  mmgsdi_perso_status_enum_type perso_status)
{
  mmgsdi_int_app_info_type       *int_app_info_ptr       = NULL;
  mmgsdi_return_enum_type         mmgsdi_status          = MMGSDI_ERROR;
  mmgsdi_slot_id_enum_type        slot_id                = MMGSDI_SLOT_AUTOMATIC;
  mmgsdi_evt_session_notify_type  notify_type;

  UIM_MSG_HIGH_0("mmgsdi_external_simlock_sub_ok_perso_update");

  mmgsdi_status = mmgsdi_util_get_prov_session_info(session_id,
                                                    NULL,
                                                    &slot_id,
                                                    &int_app_info_ptr);

  if ((mmgsdi_status != MMGSDI_SUCCESS) || (int_app_info_ptr == NULL))
  {
    /* Invalid session_id*/
    return;
  }

  memset(&notify_type, 0x00, sizeof(mmgsdi_evt_session_notify_type));

  notify_type.notify_type = MMGSDI_EVT_NOTIFY_ALL_SESSIONS;
  notify_type.slot_id     = slot_id;

  /* Only if the app state is WAIT_FOR_SUB_OK, trigger the state transition */
  if(int_app_info_ptr->app_state != MMGSDI_APP_STATE_WAIT_FOR_SUB_OK)
  {
    return;
  }

  /* Update to reflect the new state if we were in the WAIT_FOR_SUB_OK state
     before */
  if(perso_status == MMGSDI_PERSO_STATUS_GENERIC_FAILURE)
  {
    mmgsdi_state_update_app_state_for_prov_app(session_id,
                                               MMGSDI_APP_STATE_WAIT_FOR_SUB_OK,
                                               MMGSDI_APP_STATE_EXT_PERSO_FAILED,
                                               NULL);
  }
  /* if external perso succeeded */
  else if(perso_status == MMGSDI_PERSO_STATUS_DONE)
  {
    /* Move the app state to PERSO_VERIFIED */
    mmgsdi_state_update_app_state_for_prov_app(session_id,
                                               MMGSDI_APP_STATE_WAIT_FOR_SUB_OK,
                                               MMGSDI_APP_STATE_PERSO_VERIFIED,
                                               NULL);

    (void)mmgsdi_card_init_build_internal_pup_req(session_id,
                                                  slot_id,
                                                  MMGSDI_PIN1_EVT,
                                                  MMGSDI_STATE_CARD_INSERTED,
                                                  MMGSDI_APP_STATE_PERSO_VERIFIED);
  }
  else
  {
    return;
  }

  /* Build and send the perso event with DONE status */
  mmgsdi_evt_build_and_send_perso(
    notify_type,
    (const mmgsdi_int_app_info_type*)int_app_info_ptr,
    perso_status,
    MMGSDI_MAX_PERSO_FEATURE_ENUM,
    0,
    0);
} /* mmgsdi_external_simlock_sub_ok_perso_update */


/*===========================================================================
FUNCTION MMGSDI_EXTERNAL_SIMLOCK_PROCESS_SUB_OK_REQ

DESCRIPTION
  This function, called in mmgsdi_process_command.
  It will determine if a subscription that is initialized successfully will
  get published to the rest of the modem.

DEPENDENCIES
  None

LIMITATIONS
  This command is only supported on provisioning subscriptions

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_NOT_SUPPORTED    - Subscription state doesn't support this command
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_external_simlock_process_sub_ok_req (
  const mmgsdi_session_sub_ok_req_type *sub_ok_req_ptr)
{
  mmgsdi_return_enum_type    mmgsdi_status     = MMGSDI_ERROR;
  mmgsdi_return_enum_type    req_status        = MMGSDI_ERROR;
  mmgsdi_int_app_info_type  *int_app_info_ptr  = NULL;
  mmgsdi_slot_id_enum_type   slot_id           = MMGSDI_MAX_SLOT_ID_ENUM;
  uint8                      index             = 0;

  req_status = mmgsdi_util_get_prov_session_info(
                 sub_ok_req_ptr->request_header.session_id,
                 NULL, &slot_id, &int_app_info_ptr);

  if ((req_status == MMGSDI_SUCCESS) && (int_app_info_ptr != NULL))
  {
    UIM_MSG_HIGH_1("Identified prov app for SUB OK request.App type=0x%x",
                   int_app_info_ptr->app_data.app_type);
    if (int_app_info_ptr->app_state == MMGSDI_APP_STATE_WAIT_FOR_SUB_OK)
    {
      mmgsdi_perso_status_enum_type  perso_status      = MMGSDI_PERSO_STATUS_NONE;
      mmgsdi_session_id_type         mmgsdi_session_id = 0;

      /* Map input Session ID to MMGSDI Session ID. MMGSDI Session ID needs to
         be used in MMGSDI power-up sequence to ensure any ongoing REFRESH which
         triggered the power-up sequence is correctly completed */
      mmgsdi_status = mmgsdi_util_map_to_mmgsdi_session(
                        sub_ok_req_ptr->request_header.session_id,
                        &mmgsdi_session_id);

      if(mmgsdi_status == MMGSDI_SUCCESS)
      {
        if (sub_ok_req_ptr->ok_for_sub == TRUE)
        {
          UIM_MSG_HIGH_0("In mmgsdi_external_simlock_process_sub_ok_req, perso done");
          perso_status = MMGSDI_PERSO_STATUS_DONE;
        }
        else
        {
          UIM_MSG_HIGH_0("In mmgsdi_external_simlock_process_sub_ok_req, perso failed");
          perso_status = MMGSDI_PERSO_STATUS_GENERIC_FAILURE;
        }

        mmgsdi_external_simlock_sub_ok_perso_update(mmgsdi_session_id,
                                                    perso_status);
      }
      else
      {
        UIM_MSG_ERR_1("Received invalid Session ID: 0x%x in SUB OK req",
                      sub_ok_req_ptr->request_header.session_id);
        req_status = MMGSDI_INCORRECT_PARAMS;
      }
    }
    else
    {
      UIM_MSG_ERR_1("Received SUB OK req in unsupported state=0x%x",
                    int_app_info_ptr->app_state);
      req_status = MMGSDI_NOT_SUPPORTED;
    }
  }
  else
  {
    req_status = MMGSDI_ERROR;
    if (int_app_info_ptr != NULL)
    {
      UIM_MSG_ERR_2("SUB OK req with session id=0x%x,app type=0x%x failed",
                  sub_ok_req_ptr->request_header.session_id,
                  int_app_info_ptr->app_data.app_type);
    }
    else
    {
      UIM_MSG_ERR_1("SUB OK req with session id=0x%x failed",
                    sub_ok_req_ptr->request_header.session_id);
    }
  }

  /* Get an index from the table used to store client information */
  mmgsdi_status = mmgsdi_util_fill_and_alloc_request_table(MMGSDI_SESSION_SUB_OK_REQ,
                                                           (void*)sub_ok_req_ptr,
                                                           NULL,
                                                           &index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Send out the response */
  mmgsdi_status = mmgsdi_util_queue_mmgsdi_uim_report_rsp(
                    index, sub_ok_req_ptr->request_header.request_type,
                    req_status);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
  }

  return mmgsdi_status;
} /* mmgsdi_external_simlock_process_sub_ok_req */


/*===========================================================================
FUNCTION MMGSDI_EXTERNAL_SIMLOCK_PROCESS_SUB_OK_RSP

DESCRIPTION
  This function is called in mmgsdi_process_response.
  It serves mainly to pack the response and queue the confirmation

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_external_simlock_process_sub_ok_rsp (
  const mmgsdi_uim_report_rsp_type *uim_rsp_ptr
)
{
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_ERROR;
  mmgsdi_request_header_type         req_header       = {0};
  int32                              index            = 0;
  mmgsdi_client_req_extra_info_type *extra_param_ptr  = NULL;

  UIM_MSG_HIGH_0("MMGSDI PROCESS SUB OK RSP");

  /* Check input pointer */
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);

  index = uim_rsp_ptr->rsp_data_index;

  MMGSDIUTIL_RETURN_IF_EXCEEDS(index, MMGSDI_MAX_CLIENT_INFO - 1);

  mmgsdi_status = mmgsdi_util_get_client_request_table_info(
                                index,
                                &req_header,
                                &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  return mmgsdi_cnf_build_and_queue(uim_rsp_ptr->mmgsdi_error_status,&req_header,
           extra_param_ptr,NULL,TRUE,uim_rsp_ptr->status_word);
} /* mmgsdi_external_simlock_process_sub_ok_rsp */
