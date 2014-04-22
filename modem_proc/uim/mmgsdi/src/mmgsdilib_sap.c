/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                        M M G S D I   S A P   L I B R A R Y


GENERAL DESCRIPTION

  This file contains Library function that MMGSDI client can call to access
  various SAP functions


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

$Header: //components/rel/uim.mpss/3.3.1/mmgsdi/src/mmgsdilib_sap.c#1 $$ $DateTime: 2020/07/22 10:53:27 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/15/16   sp      Remove command type parameter from validate_client_id function
04/25/16   bcho    Initial version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "customer.h"
#include "intconv.h"
#include "mmgsdilib.h"
#include "mmgsdi.h"
#include "uim_msg.h"
#include "mmgsdiutil.h"
#include "mmgsdi_caller_context.h"

/*=============================================================================

                                  FUNCTIONS

=============================================================================*/

/*===========================================================================
FUNCTION MMGSDI_SAP_CONNECT_OPERATION

DESCRIPTION
  This function attempts to perform a SAP connect.

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  sap_mode:        Client/Server.
  condition:       Condition to check for active call status.
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achieved by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_sap_connect_operation(
    mmgsdi_client_id_type                 client_id,
    mmgsdi_slot_id_enum_type              card_slot,
    mmgsdi_sap_mode_enum_type             sap_mode,
    mmgsdi_sap_conn_condition_enum_type   condition,
    mmgsdi_callback_type                  response_cb_ptr,
    mmgsdi_client_data_type               client_ref)
{
  mmgsdi_task_cmd_type        *task_cmd_ptr  = NULL;
  mmgsdi_sap_connect_req_type* msg_ptr       = NULL;
  mmgsdi_return_enum_type      mmgsdi_status = MMGSDI_ERROR;
  uint32                       task_cmd_len  = 0;

  /* Input Parameters Check. */
  mmgsdi_status = mmgsdi_util_is_client_id_valid_in_cmd(client_id);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if (sap_mode != MMGSDIBT_SERVER)
  {
    UIM_MSG_ERR_1("mmgsdi_sap_connect, unsupported mode 0x%x", (int)sap_mode);
    return MMGSDI_NOT_SUPPORTED;
  }

  if(card_slot != MMGSDI_SLOT_1)
  {
    UIM_MSG_ERR_1("mmgsdi_sap_connect, not supported on slot 0x%x", card_slot);
    return MMGSDI_NOT_SUPPORTED;
  }

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  task_cmd_len = sizeof(mmgsdi_task_cmd_type);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*---------------------------------------------------------------------------
    - No need to Align pointers of the structures members
  ---------------------------------------------------------------------------*/
  msg_ptr = &task_cmd_ptr->cmd.cmd.sap_connect_req;

  /* Populating mmgsdi_sap_connect_req_type header. */
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_SAP_CONNECT_REQ;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.request_type      = MMGSDI_SAP_CONNECT_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_SAP_CONNECT_REQ;
  msg_ptr->request_header.slot_id           = card_slot;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /* Populating condition value */
  msg_ptr->condition = condition;

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_SAP_CONNECT_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
} /* mmgsdi_sap_connect_operation */


/*===========================================================================
FUNCTION MMGSDI_SAP_CONNECT

DESCRIPTION
  This function attempts to perform a SAP connect.

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  sap_mode:        Client/Server.
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_connect(
    mmgsdi_client_id_type     client_id,
    mmgsdi_slot_id_enum_type  card_slot,
    mmgsdi_sap_mode_enum_type sap_mode,
    mmgsdi_callback_type      response_cb_ptr,
    mmgsdi_client_data_type   client_ref)
{
  return mmgsdi_sap_connect_operation(
           client_id,
           card_slot,
           sap_mode,
           MMGSDI_SAP_CONN_COND_BLOCK_VOICE_OR_DATA,
           response_cb_ptr,
           client_ref);
} /* mmgsdi_sap_connect */


/*===========================================================================
FUNCTION MMGSDI_SAP_CONNECT_EXT

DESCRIPTION
  This function attempts to perform a SAP connect. It also provides an option
  for the client to specify a condition for the SAP connection request.
  SAP connection request with proceed based on this condition.

PARAMS:
  client_id:           Client Id of the caller
  card_slot:           Physical slot for multiple slot targets.
  sap_mode:            Client/Server.
  condition:           Condtion to check for active call status.
  response_cb_ptr:     Response callback.
  client_ref:          User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_connect_ext(
    mmgsdi_client_id_type                 client_id,
    mmgsdi_slot_id_enum_type              card_slot,
    mmgsdi_sap_mode_enum_type             sap_mode,
    mmgsdi_sap_conn_condition_enum_type   condition,
    mmgsdi_callback_type                  response_cb_ptr,
    mmgsdi_client_data_type               client_ref)
{
  return mmgsdi_sap_connect_operation(
           client_id,
           card_slot,
           sap_mode,
           condition,
           response_cb_ptr,
           client_ref);
} /* mmgsdi_sap_connect_ext */


/*===========================================================================
FUNCTION MMGSDI_SAP_DISCONNECT

DESCRIPTION
  This function attempts to perform a SAP disconnect.

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  sap_mode:        Client/Server.
  disconnect_mode: Graceful/Immediate.
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_disconnect(
    mmgsdi_client_id_type               client_id,
    mmgsdi_slot_id_enum_type            card_slot,
    mmgsdi_sap_mode_enum_type           sap_mode,
    mmgsdi_disconnect_mode_enum_type    disconnect_mode,
    mmgsdi_callback_type                response_cb_ptr,
    mmgsdi_client_data_type             client_ref)
{
  mmgsdi_task_cmd_type          * task_cmd_ptr  = NULL;
  mmgsdi_sap_disconnect_req_type* msg_ptr       = NULL;
  mmgsdi_return_enum_type         mmgsdi_status = MMGSDI_ERROR;
  uint32                          task_cmd_len  = 0;
  mmgsdi_event_data_type          event_data;

  /* Input Parameters Check. */
  mmgsdi_status = mmgsdi_util_is_client_id_valid_in_cmd(client_id);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if(card_slot != MMGSDI_SLOT_1)
  {
    UIM_MSG_ERR_1("mmgsdi_sap_disconnect, not supported on slot 0x%x",
                  card_slot);
    return MMGSDI_NOT_SUPPORTED;
  }

  memset(&event_data, 0, sizeof(mmgsdi_event_data_type));

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  task_cmd_len = sizeof(mmgsdi_task_cmd_type);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*---------------------------------------------------------------------------
    - No need to Align pointers of the structures members
  ---------------------------------------------------------------------------*/
  msg_ptr = &task_cmd_ptr->cmd.cmd.sap_disconnect_req;

  /* Populating mmgsdi_sap_disconnect_req_type header. */
  task_cmd_ptr->cmd.cmd_enum               = MMGSDI_SAP_DISCONNECT_REQ;
  msg_ptr->request_header.client_id        = client_id;
  msg_ptr->request_header.request_type     = MMGSDI_SAP_DISCONNECT_REQ;
  msg_ptr->request_header.orig_request_type= MMGSDI_SAP_DISCONNECT_REQ;
  msg_ptr->request_header.slot_id          = card_slot;
  msg_ptr->request_header.client_data      = client_ref;
  msg_ptr->request_header.response_cb      = response_cb_ptr;

  /* Populating mmgsdi_sap_disconnect_req_type content. */
  msg_ptr->disconnect_mode = disconnect_mode;
  msg_ptr->sap_mode        = sap_mode;

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_SAP_DISCONNECT_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
} /* mmgsdi_sap_disconnect */


/*===========================================================================
FUNCTION MMGSDI_SAP_GET_ATR

DESCRIPTION:
  This is the API used to retrieve the ATR Information which pertains to the
  card(s) currently inserted in the ME.

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_get_atr(
  mmgsdi_client_id_type    client_id,
  mmgsdi_slot_id_enum_type card_slot,
  mmgsdi_callback_type     response_cb_ptr,
  mmgsdi_client_data_type  client_ref)
{
  mmgsdi_task_cmd_type          * task_cmd_ptr  = NULL;
  mmgsdi_sap_get_atr_req_type   * msg_ptr       = NULL;
  mmgsdi_return_enum_type         mmgsdi_status = MMGSDI_ERROR;
  uint32                          task_cmd_len  = 0;

  /* Input Parameters Check. */
  mmgsdi_status = mmgsdi_util_is_client_id_valid_in_cmd(client_id);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if(card_slot != MMGSDI_SLOT_1)
  {
    UIM_MSG_ERR_1("mmgsdi_sap_get_atr, not supported on slot 0x%x", card_slot);
    return MMGSDI_NOT_SUPPORTED;
  }

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  task_cmd_len = sizeof(mmgsdi_task_cmd_type);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*---------------------------------------------------------------------------
    - No need to Align pointers of the structures members
  ---------------------------------------------------------------------------*/
  msg_ptr = &task_cmd_ptr->cmd.cmd.sap_get_atr_req;

  /* Populate header. */
  task_cmd_ptr->cmd.cmd_enum                 = MMGSDI_SAP_GET_ATR_REQ;
  msg_ptr->request_header.client_id          = client_id;
  msg_ptr->request_header.request_type       = MMGSDI_SAP_GET_ATR_REQ;
  msg_ptr->request_header.orig_request_type  = MMGSDI_SAP_GET_ATR_REQ;
  msg_ptr->request_header.slot_id            = card_slot;
  msg_ptr->request_header.client_data        = client_ref;
  msg_ptr->request_header.response_cb        = response_cb_ptr;

  /* Put on mmgsdi command queue. */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_SAP_GET_ATR_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
}/* mmgsdi_sap_get_atr */


/*===========================================================================
FUNCTION MMGSDI_SAP_SEND_APDU

DESCRIPTION
  This function will send an APDU to the SAP Client.

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  channel_id:      The channel id.
  apdu_data:       The APDU to send to the card.
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_send_apdu(
  mmgsdi_client_id_type          client_id,
  mmgsdi_slot_id_enum_type       card_slot,
  int32                          channel_id,
  mmgsdi_sap_send_apdu_data_type apdu_data,
  mmgsdi_callback_type           response_cb_ptr,
  mmgsdi_client_data_type        client_ref)
{
  mmgsdi_task_cmd_type          * task_cmd_ptr = NULL;
  mmgsdi_sap_send_apdu_req_type * msg_ptr      = NULL;
  mmgsdi_return_enum_type        mmgsdi_status = MMGSDI_ERROR;
  int32                          total_mem_len = 0;
  int32                          temp_mem      = 0;

  MMGSDIUTIL_RETURN_IF_NULL(apdu_data.data_ptr);

  mmgsdi_util_print_data_to_diag(
    "BT_EV_PF_SAP_TX_APDU_REQ:",
     apdu_data.data_ptr,
     (uint16)apdu_data.data_len);

  /* Input Parameters Check. */
  if ( channel_id < 0 )
  {
    return MMGSDI_INCORRECT_PARAMS;
  }
  else if (apdu_data.data_ptr == NULL)
  {
    return MMGSDI_INCORRECT_PARAMS;
  }
  else if ((apdu_data.data_len < MMGSDI_MIN_APDU_LEN) ||
           (apdu_data.data_len > MMGSDI_MAX_CHANNEL_DATA_CMD_LEN))
  {
    MMGSDI_DEBUG_MSG_ERROR_1("Bad data size (%u)", apdu_data.data_len);
    return MMGSDI_INCORRECT_PARAMS;
  }
  else if ((apdu_data.data_ptr[UIM_7816_APDU_INSTRN_OFFSET] == SELECT) &&
             (apdu_data.data_ptr[UIM_7816_APDU_P1_OFFSET] == UIM_7816_APDU_MIN_SIZE) &&
             ((apdu_data.data_len < MMGSDI_SELECT_APDU_MIN_LEN) ||
              (apdu_data.data_ptr[MMGSDI_SELECT_APDU_LEN_OFFSET] < MMGSDI_SELECT_APDU_MIN_LEN)))
  {
    return MMGSDI_INCORRECT_PARAMS;
  }
  else if (apdu_data.data_ptr[UIM_7816_APDU_INSTRN_OFFSET] == GET_RESPONSE)
  {
    if (apdu_data.data_len != 5)
    {
      MMGSDI_DEBUG_MSG_ERROR_2("Bad data size (req: %u, exp: %u)",
                               apdu_data.data_len, 5);
      return MMGSDI_INCORRECT_PARAMS;
    }
    else if ((apdu_data.data_ptr[UIM_7816_APDU_P1_OFFSET] != 0X00) ||
               (apdu_data.data_ptr[UIM_7816_APDU_P2_OFFSET] != 0X00))
    {
      MMGSDI_DEBUG_MSG_ERROR_2("GSDIBT bad p1(%u) or p2(%u)",
                               apdu_data.data_ptr[UIM_7816_APDU_P1_OFFSET],
                               apdu_data.data_ptr[UIM_7816_APDU_P2_OFFSET]);
      return MMGSDI_INCORRECT_PARAMS;
    }
  }

  mmgsdi_status = mmgsdi_util_is_client_id_valid_in_cmd(client_id);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if(card_slot != MMGSDI_SLOT_1)
  {
    UIM_MSG_ERR_1("mmgsdi_sap_send_apdu, not supported on slot 0x%x", card_slot);
    return MMGSDI_NOT_SUPPORTED;
  }

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  total_mem_len = sizeof(mmgsdi_task_cmd_type);
  mmgsdi_util_cal_align_size(total_mem_len, &temp_mem);
  MMGSDIUTIL_RETURN_IF_ALIGNED_VAR_OUT_OF_BOUNDS(total_mem_len, temp_mem);

  total_mem_len = temp_mem + apdu_data.data_len;

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, int32touint32(total_mem_len));
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*---------------------------------------------------------------------------
    - Align pointers of the various structures members
  ---------------------------------------------------------------------------*/
  mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.sap_send_apdu_req, MMGSDI_SAP_SEND_APDU_REQ);

  msg_ptr = &task_cmd_ptr->cmd.cmd.sap_send_apdu_req;

  /* Populate header. */
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_SAP_SEND_APDU_REQ;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.request_type      = MMGSDI_SAP_SEND_APDU_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_SAP_SEND_APDU_REQ;
  msg_ptr->request_header.slot_id           = card_slot;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /* Populate the payload. */
  msg_ptr->channel_id    = int32touint8(channel_id);
  msg_ptr->data.data_len = apdu_data.data_len;
  mmgsdi_memscpy(msg_ptr->data.data_ptr,
                 int32touint32(msg_ptr->data.data_len),
                 apdu_data.data_ptr,
                 int32touint32(msg_ptr->data.data_len));

  /* Put on mmgsdi command queue. */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_SEND_APDU_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
}/* mmgsdi_sap_send_apdu */


/*===========================================================================
FUNCTION MMGSDI_SAP_POWER_ON

DESCRIPTION
  This function attempts to perform a SAP power on

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  sap_mode:        Client/Server.
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_power_on(
    mmgsdi_client_id_type     client_id,
    mmgsdi_slot_id_enum_type  card_slot,
    mmgsdi_sap_mode_enum_type sap_mode,
    mmgsdi_callback_type      response_cb_ptr,
    mmgsdi_client_data_type   client_ref)
{
  mmgsdi_task_cmd_type         * task_cmd_ptr = NULL;
  mmgsdi_sap_power_on_req_type* msg_ptr       = NULL;
  mmgsdi_return_enum_type       mmgsdi_status = MMGSDI_ERROR;
  uint32                        task_cmd_len  = 0;

  /* Input Parameters Check. */
  mmgsdi_status = mmgsdi_util_is_client_id_valid_in_cmd(client_id);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if (sap_mode != MMGSDIBT_SERVER)
  {
    UIM_MSG_ERR_1("mmgsdi_sap_power_on, unsupported mode 0x%x", (int)sap_mode);
    return MMGSDI_NOT_SUPPORTED;
  }

  if(card_slot != MMGSDI_SLOT_1)
  {
    UIM_MSG_ERR_1("mmgsdi_sap_power_on, not supported on slot 0x%x", card_slot);
    return MMGSDI_NOT_SUPPORTED;
  }

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  task_cmd_len = sizeof(mmgsdi_task_cmd_type);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*---------------------------------------------------------------------------
    - No need to align pointers of the structures members
  ---------------------------------------------------------------------------*/
  msg_ptr = &task_cmd_ptr->cmd.cmd.sap_power_on_req;

  /*
   * Populating mmgsdi_sap_power_on_req_type header.
   */
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_SAP_POWER_ON_REQ;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.request_type      = MMGSDI_SAP_POWER_ON_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_SAP_POWER_ON_REQ;
  msg_ptr->request_header.slot_id           = card_slot;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_SAP_POWER_ON_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
} /* mmgsdi_sap_power_on */


/*===========================================================================
FUNCTION MMGSDI_SAP_POWER_OFF

DESCRIPTION
  This function attempts to perform a SAP power off

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  sap_mode:        Client/Server.
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_power_off(
    mmgsdi_client_id_type     client_id,
    mmgsdi_slot_id_enum_type  card_slot,
    mmgsdi_sap_mode_enum_type sap_mode,
    mmgsdi_callback_type      response_cb_ptr,
    mmgsdi_client_data_type   client_ref)
{
  mmgsdi_task_cmd_type          * task_cmd_ptr  = NULL;
  mmgsdi_sap_power_off_req_type * msg_ptr       = NULL;
  mmgsdi_return_enum_type         mmgsdi_status = MMGSDI_ERROR;
  uint32                          task_cmd_len  = 0;

  /* Input Parameters Check. */
  if (sap_mode != MMGSDIBT_SERVER)
  {
    UIM_MSG_ERR_1("mmgsdi_sap_power_off, unsupported mode 0x%x", (int)sap_mode);
    return MMGSDI_NOT_SUPPORTED;
  }

  mmgsdi_status = mmgsdi_util_is_client_id_valid_in_cmd(client_id);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if(card_slot != MMGSDI_SLOT_1)
  {
    UIM_MSG_ERR_1("mmgsdi_sap_power_off, not supported on slot 0x%x", card_slot);
    return MMGSDI_NOT_SUPPORTED;
  }

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  task_cmd_len = sizeof(mmgsdi_task_cmd_type);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*---------------------------------------------------------------------------
    - No need to align pointers of the structures members
  ---------------------------------------------------------------------------*/
  msg_ptr = &task_cmd_ptr->cmd.cmd.sap_power_off_req;

  /* Populating mmgsdi_sap_power_off_req_type header. */
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_SAP_POWER_OFF_REQ;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.request_type      = MMGSDI_SAP_POWER_OFF_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_SAP_POWER_OFF_REQ;
  msg_ptr->request_header.slot_id           = card_slot;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_SAP_POWER_OFF_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
} /* mmgsdi_sap_power_off */


/*===========================================================================
FUNCTION MMGSDI_SAP_CARD_READER_STATUS

DESCRIPTION
  This function attempts to perform a SAP card reader status

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  sap_mode:        Client/Server.
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_card_reader_status(
    mmgsdi_client_id_type     client_id,
    mmgsdi_slot_id_enum_type  card_slot,
    mmgsdi_sap_mode_enum_type sap_mode,
    mmgsdi_callback_type      response_cb_ptr,
    mmgsdi_client_data_type   client_ref)
{
  mmgsdi_task_cmd_type                  * task_cmd_ptr  = NULL;
  mmgsdi_sap_card_reader_status_req_type* msg_ptr = NULL;
  mmgsdi_return_enum_type                 mmgsdi_status    = MMGSDI_ERROR;
  uint32                                  task_cmd_len  = 0;

  /* Input Parameters Check. */
  if (sap_mode != MMGSDIBT_SERVER)
  {
    UIM_MSG_ERR_1("mmgsdi_sap_card_reader_status, unsupported mode 0x%x",
                  (int)sap_mode);
    return MMGSDI_NOT_SUPPORTED;
  }

  mmgsdi_status = mmgsdi_util_is_client_id_valid_in_cmd(client_id);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if(card_slot != MMGSDI_SLOT_1)
  {
    UIM_MSG_ERR_1("mmgsdi_sap_card_reader_status, not supported on slot 0x%x",
                  card_slot);
    return MMGSDI_NOT_SUPPORTED;
  }

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  task_cmd_len = sizeof(mmgsdi_task_cmd_type);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*---------------------------------------------------------------------------
    - No need to align pointers of the structures members
  ---------------------------------------------------------------------------*/
  msg_ptr = &task_cmd_ptr->cmd.cmd.sap_card_reader_status_req;

  /* Populating mmgsdi_sap_card_reader_status_req_type header. */
  task_cmd_ptr->cmd.cmd_enum                  = MMGSDI_SAP_CARD_READER_STATUS_REQ;
  msg_ptr->request_header.client_id           = client_id;
  msg_ptr->request_header.request_type        = MMGSDI_SAP_CARD_READER_STATUS_REQ;
  msg_ptr->request_header.orig_request_type   = MMGSDI_SAP_CARD_READER_STATUS_REQ;
  msg_ptr->request_header.slot_id             = card_slot;
  msg_ptr->request_header.client_data         = client_ref;
  msg_ptr->request_header.response_cb         = response_cb_ptr;

  /* Populating mmgsdi_sap_card_reader_status_req_type content. */

  mmgsdi_status = mmgsdi_caller_context_sap_card_reader_status_and_notify_client(task_cmd_ptr);

  MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  return mmgsdi_status;
} /* mmgsdi_sap_card_reader_status */


/*===========================================================================
FUNCTION MMGSDI_SAP_RESET

DESCRIPTION
  This function attempts to perform a SAP RESET

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  sap_mode:        Client/Server.
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_reset(
    mmgsdi_client_id_type     client_id,
    mmgsdi_slot_id_enum_type  card_slot,
    mmgsdi_sap_mode_enum_type sap_mode,
    mmgsdi_callback_type      response_cb_ptr,
    mmgsdi_client_data_type   client_ref)
{
  mmgsdi_task_cmd_type          * task_cmd_ptr  = NULL;
  mmgsdi_sap_reset_req_type     * msg_ptr     = NULL;
  mmgsdi_return_enum_type         mmgsdi_status = MMGSDI_ERROR;
  uint32                          task_cmd_len  = 0;

  /* Input Parameters Check. */
  if (sap_mode != MMGSDIBT_SERVER)
  {
    UIM_MSG_ERR_1("mmgsdi_sap_reset, unsupported mode 0x%x", (int)sap_mode);
    return MMGSDI_NOT_SUPPORTED;
  }

  mmgsdi_status = mmgsdi_util_is_client_id_valid_in_cmd(client_id);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if(card_slot != MMGSDI_SLOT_1)
  {
    UIM_MSG_ERR_1("mmgsdi_sap_reset, not supported on slot 0x%x", card_slot);
    return MMGSDI_NOT_SUPPORTED;
  }

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  task_cmd_len = sizeof(mmgsdi_task_cmd_type);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*---------------------------------------------------------------------------
    - No need to align pointers
    ---------------------------------------------------------------------------*/
  msg_ptr = &task_cmd_ptr->cmd.cmd.sap_reset_req;

  /* Populating mmgsdi_sap_reset_req_type header. */
  task_cmd_ptr->cmd.cmd_enum                 = MMGSDI_SAP_RESET_REQ;
  msg_ptr->request_header.client_id          = client_id;
  msg_ptr->request_header.request_type       = MMGSDI_SAP_RESET_REQ;
  msg_ptr->request_header.orig_request_type  = MMGSDI_SAP_RESET_REQ;
  msg_ptr->request_header.slot_id            = card_slot;
  msg_ptr->request_header.client_data        = client_ref;
  msg_ptr->request_header.response_cb        = response_cb_ptr;

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_SAP_RESET_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
} /* mmgsdi_sap_reset */

