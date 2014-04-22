/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                         M M G S D I  L I B R A R Y

                         D E P R I C A T E D  A P I 


GENERAL DESCRIPTION

  This source file contains the depricated MMGSDI library functions.

INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2018 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/mmgsdi/src/mmgsdi_depricated_api.c#1 $$ $DateTime: 2020/07/22 10:53:27 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/10/18   tq      Replace deprecated APIs with its sync API in UIM code
10/31/18   tq      Removal of MMGSDI_SESSION_PROV_APP_INIT_COMPLETE_REQ
09/17/18   rps     Change steering of roaming refresh handling
07/09/18   tq      initial versoin


===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "intconv.h"
#include "mmgsdilib.h"
#include "mmgsdi.h"
#include "uim_msg.h"
#include "mmgsdiutil.h"
#include "mmgsdi_session.h"
#include "uim.h"
#include "mmgsdi_evt.h"
#include "mmgsdi_nv.h"
#include "mmgsdi_refresh.h"
#include "mmgsdilib_p.h"
#include "mmgsdi_eons.h"
#include "mmgsdi_se13.h"
#include "mmgsdi_file.h"
#include "mmgsdi_manage_channel.h"
#include "mmgsdi_builtin_plmn.h"

/*=============================================================================

                                  FUNCTIONS

=============================================================================*/
/* ============================================================================
FUNCTION MMGSDI_GET_ATR

DESCRIPTION:
  This is the API used to retrieve the ATR Information which pertains to the
  card(s) currently inserted in the ME.

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  option:          Additional optional parameters for this API. Currently none
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Archived by calling mmgsdi_client_id_reg().

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
  ========================================================================== */
mmgsdi_return_enum_type mmgsdi_get_atr(
  mmgsdi_client_id_type    client_id,
  mmgsdi_slot_id_enum_type card_slot,
  mmgsdi_option_type       option,
  mmgsdi_callback_type     response_cb_ptr,
  mmgsdi_client_data_type  client_ref
)
{
  mmgsdi_task_cmd_type      * task_cmd_ptr  = NULL;
  mmgsdi_get_atr_req_type   * msg_ptr       = NULL;
  mmgsdi_return_enum_type     mmgsdi_status = MMGSDI_ERROR;
  uint32                      task_cmd_len  = 0;

  /* -------------------------------------------------------------------------
     Validate the Input Parameters
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_client_id_valid_in_cmd(client_id);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(card_slot, MMGSDI_SLOT_1, MMGSDI_SLOT_3);
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  /* --------------------------------------------------------------------------
     Allocate memory for the MMGSDI Task Command
     ------------------------------------------------------------------------*/
  task_cmd_len = sizeof(mmgsdi_task_cmd_type);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /* --------------------------------------------------------------------------
    No need to Align pointers of the structures members
    ------------------------------------------------------------------------ */
  msg_ptr = &task_cmd_ptr->cmd.cmd.get_atr_req;

  /*
   * Populate header.
   */
  task_cmd_ptr->cmd.cmd_enum                 = MMGSDI_GET_ATR_REQ;
  msg_ptr->request_header.client_id          = client_id;
  msg_ptr->request_header.request_type       = MMGSDI_GET_ATR_REQ;
  msg_ptr->request_header.orig_request_type  = MMGSDI_GET_ATR_REQ;
  msg_ptr->request_header.slot_id            = card_slot;
  msg_ptr->request_header.client_data        = client_ref;
  msg_ptr->request_header.response_cb        = response_cb_ptr;

  /* --------------------------------------------------------------------------
     Put on mmgsdi command queue
     ----------------------------------------------------------------------- */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the
    ** command queue
    */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_GET_ATR_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
} /* mmgsdi_get_atr */


/*=========================================================================
FUNCTION:      MMGSDI_CLIENT_ID_AND_EVT_REG

DESCRIPTION:
  This function allows client registration for new event callback that
  will contain additional info based on specific events being sent.
  The response to this request will return client ID to the calling task.

PARAMETERS:
evt_cb_ptr:      Pointer to callback for event notifications.
                 If NULL is passed, no event callback will be made

response_cb_ptr: Callback for MMGSDI to call upon completion of the
                 registration request.

client_ref_ptr:  User Data returned to the user upon completion of the
                 command.

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type

  MMGSDI_SUCCESS:          The command structure was properly generated
                           and queued onto the MMGSDI Command Queue.
  MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                           within appropriate ranges.
  MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                           because the max number of commands are already queued.

SIDE EFFECTS:
  Will result in your callback getting called everytime MMGSDI Needs to
  notify any client of any events.
  ===================================================================== */
mmgsdi_return_enum_type  mmgsdi_client_id_and_evt_reg (
  mmgsdi_evt_callback_type   *evt_cb_ptr,
  mmgsdi_callback_type       response_cb_ptr,
  mmgsdi_client_data_type    client_ref
)
{
  return mmgsdi_client_id_and_evt_reg_ext(evt_cb_ptr,
                                          response_cb_ptr,
                                          client_ref,
                                          NULL,
                                          0);
} /* mmgsdi_client_id_and_evt_reg */


/*=========================================================================
FUNCTION:      MMGSDI_CLIENT_ID_AND_EVT_REG_EXT

DESCRIPTION:
  This function allows client registration for new event callback that
  will contain additional info based on specific events being sent.
  The response to this request will return client ID to the calling task.

PARAMETERS:
evt_cb_ptr:      Pointer to callback for event notifications.
                 If NULL is passed, no event callback will be made
response_cb_ptr: Callback for MMGSDI to call upon completion of the
                 registration request.
client_ref_ptr:  User Data returned to the user upon completion of the
                 command.
client_name_ptr: Name of the client.
client_name_len: Length of the client name above.

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type

  MMGSDI_SUCCESS:          The command structure was properly generated
                           and queued onto the MMGSDI Command Queue.
  MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                           within appropriate ranges.
  MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                           because the max number of commands are already queued.

SIDE EFFECTS:
  Will result in your callback getting called everytime MMGSDI Needs to
  notify any client of any events.
  ===================================================================== */
mmgsdi_return_enum_type  mmgsdi_client_id_and_evt_reg_ext (
  mmgsdi_evt_callback_type   *evt_cb_ptr,
  mmgsdi_callback_type       response_cb_ptr,
  mmgsdi_client_data_type    client_ref,
  const char*                client_name_ptr,
  uint8                      client_name_len
)
{
  mmgsdi_task_cmd_type                  *task_cmd_ptr   = NULL;
  mmgsdi_client_id_reg_req_type         *msg_ptr        = NULL;
  mmgsdi_return_enum_type               mmgsdi_status   = MMGSDI_ERROR;
  uint32                                task_cmd_len    = 0;
  char                                  rex_task_name[REX_TASK_NAME_LEN + 1];

 /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Response CB function Check
    2) Memory allocation for msg_ptr and check for allocation
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  task_cmd_len = sizeof(mmgsdi_task_cmd_type);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /* No Need to Align Pointer inside client ID and Evt Reg structure */
  msg_ptr = &task_cmd_ptr->cmd.cmd.client_id_reg_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_usim_auth_req_type header
      1) Client ID
      2) Request TYPE
      3) Slot ID
      4) Client Data Pointer
      5) Response CallBack
      6) Event Callback
      7) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_CLIENT_ID_AND_EVT_REG_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_CLIENT_ID_AND_EVT_REG_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_CLIENT_ID_AND_EVT_REG_REQ;
  msg_ptr->request_header.client_id         = 0;
  msg_ptr->request_header.slot_id           = MMGSDI_SLOT_AUTOMATIC;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  (void)rex_get_task_name(rex_self(), rex_task_name, sizeof(rex_task_name),NULL);
  strlcpy(msg_ptr->client_task_name, rex_task_name, sizeof(msg_ptr->client_task_name));

  if (client_name_ptr &&
      client_name_len <= MMGSDI_TASK_NAME_LEN)
  {
    strlcpy(msg_ptr->client_name, client_name_ptr, client_name_len);
  }
  else
  {
    /* If the client did not provide any name, copy task_name as both client_name
       and task_name */
    strlcpy(msg_ptr->client_name, rex_task_name, sizeof(msg_ptr->client_name));
  }

  /* Populate command specific information */
  msg_ptr->evt_cb_ptr                       = evt_cb_ptr;

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put
     onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);

  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_CLIENT_ID_AND_EVT_REG_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
} /* mmgsdi_client_id_and_evt_reg_ext */


/* ============================================================================
   FUNCTION       MMGSDI_GET_ALL_AVAILABLE_APPS

   DESCRIPTION
     This function will build a request to the MMGSDI Task to request the
     information of all available applications from the UICC/ICC Card(s).

   PARAMETERS:
   client_id:         Client ID of the requesting client.
   response_cb_ptr:   Response callback.
   client_ref:        User Data returned upon completion of this cmd.

   DEPENDENCIES:
     A valid Client ID is required, which can be achieved by calling
     mmgsdi_client_id_reg().

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                              because the max number of commands are already queued.

   SIDE EFFECTS:

========================================================================*/
mmgsdi_return_enum_type mmgsdi_get_all_available_apps (
  mmgsdi_client_id_type              client_id,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
  mmgsdi_task_cmd_type                 *task_cmd_ptr   = NULL;
  mmgsdi_available_apps_req_type       *msg_ptr        = NULL;
  mmgsdi_return_enum_type               mmgsdi_status  = MMGSDI_ERROR;
  uint32                                task_cmd_len   = 0;

 /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Client id check
    2) Response CB function Check
    3) Memory allocation for msg_ptr and check for allocation
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_client_id_valid_in_cmd(client_id);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  task_cmd_len = sizeof(mmgsdi_task_cmd_type);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /* No need to align pointer inside available apps request structure */
  msg_ptr = &task_cmd_ptr->cmd.cmd.available_apps_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_available_apps_req_type header
      1) Client ID
      2) Request TYPE
      3) Slot ID
      4) Client Data Pointer
      5) Response CallBack
      6) Event Callback
      7) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_GET_ALL_AVAILABLE_APPS_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_GET_ALL_AVAILABLE_APPS_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_GET_ALL_AVAILABLE_APPS_REQ;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.session_id        = MMGSDI_INVALID_SESSION_ID;
  msg_ptr->request_header.slot_id           = MMGSDI_SLOT_AUTOMATIC;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put
     onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);

  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_GET_ALL_AVAILABLE_APPS_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
} /* mmgsdi_get_all_available_apps */


/*===========================================================================
   FUNCTION:      MMGSDI_CLIENT_ID_AND_EVT_DEREG

   DESCRIPTION:
     This function allows MMGSDI clients to deregister from MMGSDI. If
     successful, the client will no longer receive MMGSDI events or be able
     to send commands to MMGSDI.

     If the client still has card or provisioning sessions opened then these
     will be closed. Non-provisioning sessions must be closed before calling
     this API. If non-provisioning sessions are still open then the command
     will not be executed and the command callback will return MMGSDI_ERROR.

   PARAMETERS:
   mmgsdi_client_id_type      client_id:    Input.  Client ID

   mmgsdi_callback_type response_cb_ptr:    Input.  Callback for MMGSDI to call
                                                    upon completion of the
                                                    deregistration request.

   mmgsdi_client_data_type client_ref_ptr:  Input.  User Data returned to the user
                                                    upon completion of the command.

   DEPENDENCIES:
     A valid Client ID is required, which can be achieved by calling
     mmgsdi_client_id_reg()

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_ERROR:            The command could not be generated and queued.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                              because the max number of commands are already
                              queued.

   SIDE EFFECTS:
     None.
===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_client_id_and_evt_dereg (
  mmgsdi_client_id_type      client_id,
  mmgsdi_callback_type       response_cb_ptr,
  mmgsdi_client_data_type    client_ref
)
{
  mmgsdi_task_cmd_type            * task_cmd_ptr  = NULL;
  mmgsdi_client_id_dereg_req_type * msg_ptr       = NULL;
  mmgsdi_return_enum_type           mmgsdi_status = MMGSDI_ERROR;
  uint32                            task_cmd_len  = 0;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Client ID
    2) Response CB function Check
    3) Memory allocation for msg_ptr and check for allocation
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_client_id_valid_in_cmd(client_id);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if (NULL == response_cb_ptr)
  {
    /* if client does not provide the call back then use mmgsdi internal call back */
    response_cb_ptr = mmgsdi_util_discard_cb;
  }
  task_cmd_len = sizeof(mmgsdi_task_cmd_type);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /* No Need to Align Pointer inside client ID Dereg structure */
  msg_ptr = &task_cmd_ptr->cmd.cmd.client_id_dereg_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_client_id_and_evt_dereg_type header
      1) Client ID
      2) Request TYPE
      3) Slot ID
      4) Client Data Pointer
      5) Response CallBack
      6) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_CLIENT_ID_AND_EVT_DEREG_REQ;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.request_type      = MMGSDI_CLIENT_ID_AND_EVT_DEREG_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_CLIENT_ID_AND_EVT_DEREG_REQ;
  msg_ptr->request_header.slot_id           = MMGSDI_SLOT_AUTOMATIC;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);

  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_CLIENT_ID_AND_EVT_DEREG_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
} /* mmgsdi_client_id_and_evt_dereg */


/*===========================================================================
  FUNCTION:  MMGSDI_GET_SE13_PLMN_NAMES

  This API is used to get plmn names by directly accessing SE13 table for
  given set of PLMN Ids.
 Upon successful completion of this function, MMGSDI sends a confirmation to
 client with PLMN name list.

  mmgsdi_client_id_type    client_id:           Client ID
  mmgsdi_plmnId_list_type  plmn_id_list:        list of PLMN id
  mmgsdi_callback_type     response_cb_ptr:     Response callback
  mmgsdi_client_data_type  client_ref:          User Data returned upon
                                                completion of this cmd.

  @return
  MMGSDI_SUCCESS:          The command structure was properly generated
                           and queued onto the MMGSDI Command Queue.
  MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                           within appropriate ranges.
  MMGSDI_CMD_QUEUE_FULL:   The command was not queued to the MMGSDI Task
                           because the max number of commands are already queued.

  @dependencies
  The Client ID specified must be valid.

  @limitations
  None 

  @side effects
  None 
=========================================================================== */
mmgsdi_return_enum_type mmgsdi_get_se13_plmn_names(
  mmgsdi_client_id_type              client_id,
  mmgsdi_plmn_id_list_type           plmn_id_list,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
  mmgsdi_task_cmd_type                *task_cmd_ptr   = NULL;
  mmgsdi_get_se13_plmn_names_req_type *msg_ptr        = NULL;
  mmgsdi_return_enum_type              mmgsdi_status  = MMGSDI_SUCCESS;
  int32                                var_len        = 0;
  int32                                total_mem_len  = 0;
  int32                                temp_mem       = 0;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Client ID
    2) Response CB function Check
    3) plmn list ptr != NULL checl
    4) num of plmn ids > 0 Check
    5) word aligned variable check
    6) Memory allocation for task_cmd_ptr and check for allocation
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_client_id_valid_in_cmd(client_id);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  /*return INCORRECT PARAM error when API is called with
    Number of plmn ids 0 or plmn list ptr is NULL*/
  if(plmn_id_list.plmn_list_ptr == NULL || plmn_id_list.num_of_plmn_ids == 0)
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  var_len = plmn_id_list.num_of_plmn_ids * sizeof(mmgsdi_plmn_id_type);

  total_mem_len = sizeof(mmgsdi_task_cmd_type);
  mmgsdi_util_cal_align_size(var_len,&temp_mem);

  MMGSDIUTIL_RETURN_IF_ALIGNED_VAR_OUT_OF_BOUNDS(var_len,temp_mem);

  total_mem_len += temp_mem;

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, int32touint32(total_mem_len));
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*---------------------------------------------------------------------------
    - Align pointers of the various structures members
  ---------------------------------------------------------------------------*/
  mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.get_se13_plmn_names_req,
                         MMGSDI_GET_SE13_PLMN_NAMES_REQ);

  msg_ptr = &task_cmd_ptr->cmd.cmd.get_se13_plmn_names_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_get_se13_plmn_names_req_type header
      1) Client ID
      2) Request TYPE
      3) Client Data Pointer
      4) Response CallBack
      5) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_GET_SE13_PLMN_NAMES_REQ;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.request_type      = MMGSDI_GET_SE13_PLMN_NAMES_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_GET_SE13_PLMN_NAMES_REQ;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  msg_ptr->plmn_id_list.num_of_plmn_ids = plmn_id_list.num_of_plmn_ids;
  mmgsdi_memscpy(msg_ptr->plmn_id_list.plmn_list_ptr,
                 plmn_id_list.num_of_plmn_ids * sizeof(mmgsdi_plmn_id_type),
                 plmn_id_list.plmn_list_ptr,
                 plmn_id_list.num_of_plmn_ids * sizeof(mmgsdi_plmn_id_type));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_GET_SE13_PLMN_NAMES_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
}/* mmgsdi_get_se13_plmn_names */


/*====================================================================
FUNCTION MMGSDI_GET_SE13_PLMN_INFO_BY_NAME

DESCRIPTION
  This API is used to search PLMN long name/short name in SE13 table.

  Upon successful completion of this function, MMGSDI sends a confirmation to
  client with MCC, MNC, RAT type, PLMN long name and PLMN short name.

PARAMETERS
  mmgsdi_client_id_type    client_id:           Client ID
  mmgsdi_data_type         plmn_long_name:      plmn long name ptr and length
  mmgsdi_data_type         plmn_short_name:     plmn short name ptr and length
  mmgsdi_rat_enum_type     rat_type             RAT type, if valid RAT type is
                                                passed MMGSDI returns entries
                                                matching the RAT.
  mmgsdi_callback_type     response_cb_ptr:     Response callback
  mmgsdi_client_data_type  client_ref:          User Data returned upon
                                                completion of this cmd.

DEPENDENCIES
  The Client ID specified must be valid.
  PLMN name string must be in Default 7 bit coded alphabet encoding.

LIMITATIONS
  None

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not queued to the MMGSDI Task
                             because the max number of commands are already queued.

SIDE EFFECTS
  None
====================================================================== */
mmgsdi_return_enum_type mmgsdi_get_se13_plmn_info_by_name(
  mmgsdi_client_id_type              client_id,
  mmgsdi_data_type                   plmn_long_name,
  mmgsdi_data_type                   plmn_short_name,
  mmgsdi_rat_enum_type               rat_type,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
  mmgsdi_task_cmd_type                       *task_cmd_ptr   = NULL;
  mmgsdi_get_se13_plmn_info_by_name_req_type *msg_ptr        = NULL;
  mmgsdi_return_enum_type                     mmgsdi_status  = MMGSDI_SUCCESS;
  int32                                       var_len        = 0;
  int32                                       total_mem_len  = 0;
  int32                                       temp_mem       = 0;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Client ID
    2) Response CB function Check
    3) plmn long name validity check
    4) plmn short name validity Check
    5) word aligned variable check
    6) Memory allocation for task_cmd_ptr and check for allocation
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_client_id_valid_in_cmd(client_id);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(rat_type,
                                    MMGSDI_RAT_NONE,
                                    MMGSDI_RAT_TDS);

  /*return INCORRECT PARAM error when API is called with
    Number of plmn ids 0 or plmn list ptr is NULL*/
  if((plmn_long_name.data_ptr != NULL &&
       (plmn_long_name.data_len <= 0 ||
        plmn_long_name.data_len > MMGSDI_EONS_MAX_PLMN_LONG_NAME_LEN)) ||
     (plmn_long_name.data_len > 0 && plmn_long_name.data_ptr == NULL) ||
     (plmn_short_name.data_ptr != NULL &&
       (plmn_short_name.data_len <= 0 ||
        plmn_short_name.data_len > MMGSDI_EONS_MAX_PLMN_SHORT_NAME_LEN)) ||
     (plmn_short_name.data_len > 0 && plmn_short_name.data_ptr == NULL) ||
     (plmn_long_name.data_ptr == NULL && plmn_short_name.data_ptr == NULL))
  {
    UIM_MSG_ERR_3("Invalid params: long name len = 0x%x long name ptr = 0x%x short name ptr = 0x%x",
                  plmn_long_name.data_len,
                  plmn_long_name.data_ptr,
                  plmn_short_name.data_ptr);
    return MMGSDI_INCORRECT_PARAMS;
  }

  var_len = plmn_long_name.data_len + plmn_short_name.data_len;

  total_mem_len = sizeof(mmgsdi_task_cmd_type);
  mmgsdi_util_cal_align_size(var_len,&temp_mem);

  MMGSDIUTIL_RETURN_IF_ALIGNED_VAR_OUT_OF_BOUNDS(var_len,temp_mem);

  total_mem_len += temp_mem;

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, int32touint32(total_mem_len));
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  msg_ptr = &task_cmd_ptr->cmd.cmd.get_se13_plmn_info_by_name_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_get_se13_plmn_info_by_name_req_type header
      1) Client ID
      2) Request TYPE
      3) Client Data Pointer
      4) Response CallBack
      5) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_GET_SE13_PLMN_INFO_BY_NAME_REQ;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.request_type      = MMGSDI_GET_SE13_PLMN_INFO_BY_NAME_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_GET_SE13_PLMN_INFO_BY_NAME_REQ;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  msg_ptr->rat_type = rat_type;
  msg_ptr->plmn_long_name.data_len = plmn_long_name.data_len;
  msg_ptr->plmn_short_name.data_len = plmn_short_name.data_len;

  /*---------------------------------------------------------------------------
    - Align pointers of the various structures members
  ---------------------------------------------------------------------------*/
  mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.get_se13_plmn_info_by_name_req,
                         MMGSDI_GET_SE13_PLMN_INFO_BY_NAME_REQ);

  if(plmn_long_name.data_len > 0)
  {
    mmgsdi_memscpy(msg_ptr->plmn_long_name.data_ptr,
                   plmn_long_name.data_len,
                   plmn_long_name.data_ptr,
                   plmn_long_name.data_len);
  }

  if(plmn_short_name.data_len > 0)
  {
    mmgsdi_memscpy(msg_ptr->plmn_short_name.data_ptr,
                   plmn_short_name.data_len,
                   plmn_short_name.data_ptr,
                   plmn_short_name.data_len);
  }

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_GET_SE13_PLMN_INFO_BY_NAME_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
}/* mmgsdi_get_se13_plmn_info_by_name */


/* ==========================================================================
FUNCTION:       MMGSDI_SET_BUILTIN_PLMN_LIST

  This function is used to set builtin PLMN list which is received from network.
  MMGSI stores this list in EFS and gives it precedence over EF-OPLMNwACT.

  @param[in] client_id:               Client ID
  @param[in] builtin_plmn_list        list of builtin PLMNs
  @param[in] response_cb_ptr          Response callback
  @param[in] client_ref               User Data returned upon
                                      completion of this cmd.

  @return
  MMGSDI_SUCCESS          -- The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
  MMGSDI_INCORRECT_PARAMS.-- The parameters supplied to the API are not
                             within appropriate ranges.
  MMGSDI_CMD_QUEUE_FULL.  -- The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

  @dependencies
  The client must have a valid client ID.
=========================================================================== */
mmgsdi_return_enum_type  mmgsdi_set_builtin_plmn_list (
  mmgsdi_client_id_type                  client_id,
  mmgsdi_builtin_plmn_list_type          builtin_plmn_list,
  mmgsdi_callback_type                   response_cb_ptr,
  mmgsdi_client_data_type                client_ref
)
{
  mmgsdi_task_cmd_type                  *task_cmd_ptr   = NULL;
  mmgsdi_set_builtin_plmn_list_req_type *msg_ptr        = NULL;
  mmgsdi_return_enum_type                mmgsdi_status  = MMGSDI_SUCCESS;
  int32                                  var_len        = 0;
  int32                                  total_mem_len  = 0;
  int32                                  temp_mem       = 0;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Client ID
    2) Response CB function Check
    3) plmn list ptr != NULL checl
    4) num of plmn ids > 0 Check
    5) word aligned variable check
    6) Memory allocation for task_cmd_ptr and check for allocation
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_client_id_valid_in_cmd(client_id);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  /* Return INCORRECT PARAM error when API is called with
     Number of plmn ids 0 or plmn list ptr is NULL. Also note that
     Number of plmn ids 0 and plmn list ptr as NULL is a valid input */
  if((builtin_plmn_list.plmn_list_ptr == NULL && builtin_plmn_list.num_of_plmn_ids != 0) ||
     (builtin_plmn_list.plmn_list_ptr != NULL && builtin_plmn_list.num_of_plmn_ids == 0))
  {
    UIM_MSG_ERR_2("Invalid params: Number of plmn ids = 0x%x plmn list ptr = 0x%x",
                  builtin_plmn_list.num_of_plmn_ids, builtin_plmn_list.plmn_list_ptr);
    return MMGSDI_INCORRECT_PARAMS;
  }

  var_len = builtin_plmn_list.num_of_plmn_ids * sizeof(mmgsdi_builtin_plmn_id_type);

  total_mem_len = sizeof(mmgsdi_task_cmd_type);
  mmgsdi_util_cal_align_size(var_len,&temp_mem);

  MMGSDIUTIL_RETURN_IF_ALIGNED_VAR_OUT_OF_BOUNDS(var_len,temp_mem);

  total_mem_len += temp_mem;

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, int32touint32(total_mem_len));
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*---------------------------------------------------------------------------
    - Align pointers of the various structures members
  ---------------------------------------------------------------------------*/
  mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.set_builtin_plmn_list_req ,
                         MMGSDI_SET_BUILTIN_PLMN_LIST_REQ);

  msg_ptr = &task_cmd_ptr->cmd.cmd.set_builtin_plmn_list_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_get_se13_plmn_names_req_type header
      1) Client ID
      2) Request TYPE
      3) Client Data Pointer
      4) Response CallBack
      5) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_SET_BUILTIN_PLMN_LIST_REQ;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.request_type      = MMGSDI_SET_BUILTIN_PLMN_LIST_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_SET_BUILTIN_PLMN_LIST_REQ;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  msg_ptr->builtin_plmn_list.num_of_plmn_ids = builtin_plmn_list.num_of_plmn_ids;
  if (builtin_plmn_list.plmn_list_ptr)
  {
    mmgsdi_memscpy(msg_ptr->builtin_plmn_list.plmn_list_ptr,
                   builtin_plmn_list.num_of_plmn_ids * sizeof(mmgsdi_builtin_plmn_id_type),
                   builtin_plmn_list.plmn_list_ptr,
                   builtin_plmn_list.num_of_plmn_ids * sizeof(mmgsdi_builtin_plmn_id_type));
  }

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_SET_BUILTIN_PLMN_LIST_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
}/* mmgsdi_set_builtin_plmn_list */


/*=============================================================================
FUNCTION:       MMGSDI_SESSION_OPEN_EXT

  Opens multiple sessions to an application on the card.
  The session can point to the GSM/WCDMA (GW) or 1X provisioning application
  NON_PROV sessions are not supported.
  Event callback is common to all the sessions opened.A client must provide an
  event callback if it is to receive session-related event notifications. If the
  pointer is NULL, the client receives no event.

  Upon successful completion of this command,the sessions have an associated
  logical channels and associated UICC application IDs. The sessions can be
  MUXED on the same channel as other sessions pointing to the same application.

  @param[in] client_id              Client ID of the caller.
  @param[in] session_type_mask      Holds a mask of all sessions requested.
                                    (GW prov. and/or 1X prov. and/or Card session)
                                    Session_type will be determined by the bits set.
  @param[in] evt_cb_ptr       Application event callback. A client must provide
                              an event callback if it is to receive
                              session-related event notifications. If the
                              pointer is NULL, the client receives no event
                              notifications.
  @param[in] set_notify_mask  Mask which can indicate set_notify for all 4 prov sessions
                              Indicates whether the client is to call
                              mmgsdi_session_prov_app_init_complete() when
                              the client finishes initialization procedures
                              for the applications. This parameter has
                              no meaning if evt_cb_ptr is a NULL pointer.
                              Bits will be set in accordance with the session.
  @param[in] response_cb_ptr  Pointer to the response callback.
  @param[in] client_ref       User data returned upon completion of this
                              command.
  @limitations
  If any one of the requested session fails, all other opened sessions
  will be cleaned up and will return generic error.
===============================================================================*/
mmgsdi_return_enum_type  mmgsdi_session_open_ext (
  mmgsdi_client_id_type                 client_id,
  uint32                                session_type_mask,
  mmgsdi_evt_callback_type              evt_cb_ptr,
  uint32                                set_notify_mask,
  mmgsdi_callback_type                  response_cb_ptr,
  mmgsdi_client_data_type               client_ref
)
{
  mmgsdi_task_cmd_type             *task_cmd_ptr   = NULL;
  mmgsdi_session_open_ext_req_type *msg_ptr        = NULL;
  mmgsdi_return_enum_type           mmgsdi_status  = MMGSDI_ERROR;
  int32                             total_mem_len  = 0;
  int32                             temp_mem       = 0;

 if (set_notify_mask != 0)
  {
    return MMGSDI_INCORRECT_PARAMS;	
  }
  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Client ID
    2) Response CB function Check
    3) Session Type Mask
    4) Memory allocation for msg_ptr and check for allocation
    5) APP ID Length and pointer
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_client_id_valid_in_cmd(client_id);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  if(evt_cb_ptr == NULL)
  {
    UIM_MSG_HIGH_0("mmgsdi_session_open_ext null evt_cb_ptr");
  }
  if( !session_type_mask || (session_type_mask & (~MMGSDI_SESSION_VALID_MASK)))
  {
    UIM_MSG_ERR_1("mmgsdi_session_open_ext - Invalid Session Mask 0x%x",
                  session_type_mask);
    return MMGSDI_INCORRECT_PARAMS;
  }

  total_mem_len = sizeof(mmgsdi_task_cmd_type);
  mmgsdi_util_cal_align_size(total_mem_len, &temp_mem);
  MMGSDIUTIL_RETURN_IF_ALIGNED_VAR_OUT_OF_BOUNDS(total_mem_len, temp_mem);

  total_mem_len = temp_mem;

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, int32touint32(total_mem_len));
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  msg_ptr = &task_cmd_ptr->cmd.cmd.session_open_ext_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_session_open_ext_req_type header
      1) Client ID
      2) Request TYPE
      3) Client Data Pointer
      4) Response CallBack
      5) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_SESSION_OPEN_EXT_REQ;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.request_type      = MMGSDI_SESSION_OPEN_EXT_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_SESSION_OPEN_EXT_REQ;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_session_open_ext_req_type content
    - session_type_mask
    - evt_cb_ptr
  ---------------------------------------------------------------------------*/
  msg_ptr->session_type_mask       = session_type_mask;
  msg_ptr->evt_cb_ptr              = evt_cb_ptr;

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put
       onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_SESSION_OPEN_EXT_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
} /* mmgsdi_session_open_ext */


/*=============================================================================
FUNCTION:   MMGSDI_SESSION_REGISTER_FOR_REFRESH_OK_TO_PROCEED_VOTING

  Registers a client for FCNs for specific files, or for voting to refresh an
  application, or sending approval for proceeding with refresh.

  When an FCN request is received, the MMGSDI sends an MMGSDI_REFRESH_EVT event
  with mode = MMGSDI_REFRESH_FCN and with the file list that has changed.
  The MMGSDI filters out the file list based on what the client registered for.

  Some FCNs may result in bringing the protocol stack down. If the client has
  registered for these files, it should also register for voting explicitly
  on the default application.

  Some refresh scenario may result in a card powerdown. If a client is interested
  in providing approval before MMGSDI can proceed with refresh and do a card
  power down later, they do it using proceed_with_pdown flag.

  A Refresh vote means to either take the call stack down or terminate an
  application.

PARAMETERS
  session_id:      Session ID of the caller
  refresh_files:   Files that the Client is interested in getting
                   notification for
  vote_for_init:   Vote to ok bringing the call stack down/terminating
                   the application
  proceed_with_pdown: Approval before power down.
  response_cb_ptr: Callback to this command
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

LIMITATIONS
  This API will be limited to the session on which the client is.
  If the client is interested in default app, then the appropriate client
  id should be used.

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_session_register_for_refresh_ok_to_proceed_voting (
  mmgsdi_session_id_type             session_id,
  mmgsdi_refresh_file_list_type      refresh_files,
  boolean                            vote_for_init,
  boolean                            proceed_with_refresh,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_refresh_req_type req           = {{0}};

  req.refresh_req_type = MMGSDI_REFRESH_ORIG_REG_REQ;

  MMGSDIUTIL_CHK_REFRESH_FILE_PTRS(refresh_files.file_list_ptr,
    refresh_files.file_path_ptr, mmgsdi_status);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_parse_refresh_file_list(refresh_files, &req);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_0("Failed Parsing Refresh File List");
    return mmgsdi_status;
  }

  req.refresh_req_type = MMGSDI_REFRESH_ORIG_REG_REQ;
  req.refresh.reg_req.vote_for_init = vote_for_init;
  req.refresh.reg_req.ok_to_proceed = proceed_with_refresh;

  mmgsdi_status = mmgsdi_session_build_refresh_req(session_id,
                                                   &req,
                                                   response_cb_ptr,
                                                   client_ref);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("Command Queuing Failed 0x%x ,mmgsdi_session_register_for_refresh_ok_to_proceed_voting",
                  mmgsdi_status);
  }

  /* free the file_path if required */
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
    req.refresh.reg_req.refresh_files.file_path_ptr);

   return mmgsdi_status;
}/*mmgsdi_session_register_for_refresh_ok_to_proceed_voting*/ /*lint !e715*/


/*=============================================================================
FUNCTION        MMGSDI_SESSION_DEREGISTER_FOR_REFRESH

DESCRIPTION
  This function allows the client deregister for File change notifications
  for specific files or voting or both

PARAMETERS
  session_id:       Session ID of the caller
  Refresh_files:    Files that the Client is no longer interested in
                    getting notification for
  vote_for_init:    Deregister from Voting,
                    FALSE   NO changes to Current Registration,
                    TRUE    DeRegister from Voting
  Response_cb_ptr:  Callback to this command
  client_ref:       User Data returned upon completion of this cmd.


DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

LIMITATIONS
  None.

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_session_deregister_for_refresh (
   mmgsdi_session_id_type             session_id,
   mmgsdi_refresh_file_list_type      refresh_files,
   boolean                            vote_for_init,
   mmgsdi_callback_type               response_cb_ptr,
   mmgsdi_client_data_type            client_ref
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_refresh_req_type req           = {{0}};
  uint32                  i             = 0;
  uint8                  *temp_path_ptr = NULL;
  int32                   temp_len      = 0;

  req.refresh_req_type = MMGSDI_REFRESH_ORIG_DEREG_REQ;

  MMGSDIUTIL_CHK_REFRESH_FILE_PTRS(refresh_files.file_list_ptr,
    refresh_files.file_path_ptr, mmgsdi_status);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* copy the file list */
  if(refresh_files.file_list_ptr != NULL)
  {
    req.refresh.reg_req.refresh_files.file_list_ptr =
      refresh_files.file_list_ptr;
    req.refresh.reg_req.refresh_files.num_files =
      refresh_files.num_files;

    /* set file path ptr to NULL and num_file_paths to 0 */
    req.refresh.reg_req.refresh_files.file_path_ptr =
      NULL;
    req.refresh.reg_req.refresh_files.num_file_paths = 0;
  }

  /* copy the file path */
  if(refresh_files.file_path_ptr != NULL)
  {
    if(refresh_files.file_path_len < MMGSDI_MIN_EF_PATH_LEN)
    {
      UIM_MSG_ERR_1("Invalid file path len value 0x%x for refresh op.",
                    refresh_files.file_path_len);
      return MMGSDI_INCORRECT_PARAMS;
    }

    /* Assign file path pointer to request structure, allocate temporary
       buffer to account for number of path as first byte */
    /* require this temp to satisfy lint and compilation warning fixes */
    temp_len = uint32toint32((refresh_files.file_path_len + 1) * sizeof(uint8));

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(temp_path_ptr, temp_len);
    if (temp_path_ptr == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    /* Determine the number of file path */
    for (i = 0; i < (refresh_files.file_path_len - 1); i+=2)
    {
      if ((refresh_files.file_path_ptr[i] == MMGSDIUTIL_MF_HIGH_BYTE) &&
          (refresh_files.file_path_ptr[i+1] == 0x00))
      {
        temp_path_ptr[0] += 1;
      }
    }
    mmgsdi_memscpy(&temp_path_ptr[1],
                   temp_len - 1,
                   refresh_files.file_path_ptr,
                   refresh_files.file_path_len);

    /* Convert file data into file path array */
    mmgsdi_status = mmgsdi_util_convert_to_refresh_file_paths(
                      refresh_files.file_path_len+1,
                      temp_path_ptr,
                      &req.refresh.reg_req.refresh_files.file_path_ptr,
                      (uint8*)&req.refresh.reg_req.refresh_files.num_file_paths);

    MMGSDIUTIL_TMC_MEM_FREE(temp_path_ptr);

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_0("Refresh: Could not decode and copy file path");
      return MMGSDI_ERROR;
    }

    /* Set file enum list ptr to NULL and num files to  0*/
    req.refresh.reg_req.refresh_files.file_list_ptr = NULL;
    req.refresh.reg_req.refresh_files.num_files = 0;
  }

  req.refresh.reg_req.vote_for_init = vote_for_init;

  mmgsdi_status = mmgsdi_session_build_refresh_req(session_id,
                                                   &req,
                                                   response_cb_ptr,
                                                   client_ref);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("Command Queuing Failed 0x%x, mmgsdi_session_deregister_for_refresh",
                  mmgsdi_status);
  }

  /* free the file_path if required */
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
    req.refresh.reg_req.refresh_files.file_path_ptr);

  return mmgsdi_status;
}/*mmgsdi_session_deregister_for_refresh*/ /*lint !e715*/


/*=============================================================================
FUNCTION        MMGSDI_SESSION_REGISTER_FOR_REFRESH

DESCRIPTION
  This function allows the client to register for file change notifications
  for specific files and/or vote for refresh (i.e. vote for either taking
  the call stack down or terminating application that the client is
  interested in). When the FCN request is received, then MMGSDI will send
  an event MMGSDI_REFRESH_EVT with mode = MMGSDI_REFRESH_FCN with the file
  list that has changed. MMGSDI will filter out the file list based on what
  the client registered for.
  Some FCNs on default could potentially result in bringing the protocol
  stack down - if the client is interested in these files, then they should
  also register for voting explicitly on default app.

PARAMETERS
  session_id:      Session ID of the caller
  refresh_files:   Files that the Client is interested in getting
                   notification for
  vote_for_init:   Vote to ok bringing the call stack down/terminating
                   the application
  response_cb_ptr: Callback to this command
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

LIMITATIONS
  This API will be limited to the session on which the client is.
  If the client is interested in default app, then the appropriate client
  id should be used.

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_session_register_for_refresh (
  mmgsdi_session_id_type             session_id,
  mmgsdi_refresh_file_list_type      refresh_files,
  boolean                            vote_for_init,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
  return mmgsdi_session_register_for_refresh_ok_to_proceed_voting(
           session_id,
           refresh_files,
           vote_for_init,
           FALSE,
           response_cb_ptr,
           client_ref);
}/*mmgsdi_session_register_for_refresh*/ /*lint !e715*/


/*====================================================================
FUNCTION        MMGSDI_SESSION_GET_OPERATOR_NAME_EXT

DESCRIPTION
  This API is used to get operator name according to PLMN Id.

  Upon successful completion of this function, MMGSDI sends a confimation to
  client with PLMN name list and service provider name. Client can set param
  ignore_display_condition to bypass display condition provided in EF-SPN for
  RPLMN name and SPN.
  If any PLMN id of PLMN Id list matches with registered PLMN Id,
  MMGSDI sends registered PLMN name in confirmation.

PARAMETERS
  session_id:                Session ID
  plmn_id_list:              list of PLMN id
  ignore_display_condition:  ignore display condition enum type
  response_cb_ptr:           Response callback
  client_ref:                User Data returned upon completion of this cmd.

DEPENDENCIES
  The client must have a valid client ID, and the Session ID specified must
  be valid.

LIMITATIONS
  None.

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS

====================================================================== */
mmgsdi_return_enum_type  mmgsdi_session_get_operator_name_ext (
  mmgsdi_session_id_type                 session_id,
  mmgsdi_plmn_id_list_type               plmn_id_list,
  mmgsdi_eons_ignore_disp_cond_enum_type ignore_display_condition,
  mmgsdi_callback_type                   response_cb_ptr,
  mmgsdi_client_data_type                client_ref
)
{
  mmgsdi_task_cmd_type              *task_cmd_ptr   = NULL;
  mmgsdi_session_get_eons_req_type  *msg_ptr        = NULL;
  mmgsdi_return_enum_type            mmgsdi_status  = MMGSDI_SUCCESS;
  int32                              var_len        = 0;
  int32                              temp_mem       = 0;
  uint32                             task_cmd_len   = 0;
  mmgsdi_client_id_type              client_id      = 0;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) valid ignore display condition check
    2) Session ID
    3) Response CB function Check
    4) plmn list ptr and num of plmn ids check
    5) word aligned variable check
    6) Memory allocation for tsk_cmd_ptr and check for allocation
  ---------------------------------------------------------------------------*/
  if(ignore_display_condition > MMGSDI_EONS_IGNORE_SPN_RPLMN_DISPLAY_COND)
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  mmgsdi_status = mmgsdi_util_is_session_id_valid_in_cmd(
                    session_id,
                    MMGSDI_SESSION_GET_OPERATOR_NAME_REQ);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  /*return INCORRECT PARAM error when API is called with
    1.Number of plmn ids 0 and a valid plmn list ptr
    2.Or number of plmn ids > 0 and plmn list ptr is NULL*/
  if((plmn_id_list.plmn_list_ptr == NULL && plmn_id_list.num_of_plmn_ids > 0)||
     (plmn_id_list.plmn_list_ptr != NULL && plmn_id_list.num_of_plmn_ids == 0))
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  var_len = plmn_id_list.num_of_plmn_ids * sizeof(mmgsdi_plmn_id_type);

  task_cmd_len = sizeof(mmgsdi_task_cmd_type);
  mmgsdi_util_cal_align_size(var_len,&temp_mem);

  MMGSDIUTIL_RETURN_IF_ALIGNED_VAR_OUT_OF_BOUNDS(var_len,temp_mem);

  task_cmd_len += (temp_mem + sizeof(q_link_type));

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*---------------------------------------------------------------------------
    - Align pointers of the various structures members
  ---------------------------------------------------------------------------*/
  mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.session_get_eons_req,
                         MMGSDI_SESSION_GET_OPERATOR_NAME_REQ);

  msg_ptr = &task_cmd_ptr->cmd.cmd.session_get_eons_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_session_get_eons_req_type header
      1) Session ID
      2) Client ID
      3) Request TYPE
      4) Client Data Pointer
      5) Response CallBack
      6) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_SESSION_GET_OPERATOR_NAME_REQ;
  msg_ptr->request_header.session_id        = session_id;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.request_type      = MMGSDI_SESSION_GET_OPERATOR_NAME_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_SESSION_GET_OPERATOR_NAME_REQ;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  msg_ptr->ignore_display_condition     = ignore_display_condition;
  msg_ptr->plmn_id_list.num_of_plmn_ids = plmn_id_list.num_of_plmn_ids;
  if(msg_ptr->plmn_id_list.plmn_list_ptr && plmn_id_list.plmn_list_ptr)
  {
    if(plmn_id_list.num_of_plmn_ids > 0)
    {
      mmgsdi_memscpy(msg_ptr->plmn_id_list.plmn_list_ptr,
        msg_ptr->plmn_id_list.num_of_plmn_ids * sizeof(mmgsdi_plmn_id_type),
        plmn_id_list.plmn_list_ptr,
        plmn_id_list.num_of_plmn_ids * sizeof(mmgsdi_plmn_id_type));
    }
  }

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);

  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_SESSION_GET_OPERATOR_NAME_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
}/* mmgsdi_session_get_operator_name_ext */


/*====================================================================
FUNCTION        MMGSDI_SESSION_GET_OPERATOR_NAME

DESCRIPTION
  This API is used to get operator name according to PLMN Id.

  Upon successful completion of this function, MMGSDI sends a confimation to
  client with PLMN name list and service provider name
  If any PLMN id of PLMN Id list matches with registered PLMN Id,
  MMGSDI sends registered PLMN name in confirmation.

PARAMETERS
  mmgsdi_session_id_type   session_id:      Session ID
  mmgsdi_plmnId_list_type  plmn_id_list:    list of PLMN id
  mmgsdi_callback_type     response_cb_ptr: Response callback
  mmgsdi_client_data_type  client_ref:      User Data returned upon
                                            completion of this cmd.

DEPENDENCIES
  The client must have a valid client ID, and the Session ID specified must
  be valid.

LIMITATIONS
  None.

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS

====================================================================== */
mmgsdi_return_enum_type  mmgsdi_session_get_operator_name (
  mmgsdi_session_id_type    session_id,
  mmgsdi_plmn_id_list_type  plmn_id_list,
  mmgsdi_callback_type      response_cb_ptr,
  mmgsdi_client_data_type   client_ref
)
{
  return mmgsdi_session_get_operator_name_ext(
           session_id,
           plmn_id_list,
           MMGSDI_EONS_IGNORE_DISPLAY_COND_NONE,
           response_cb_ptr,
           client_ref);
}/* mmgsdi_session_get_operator_name */
