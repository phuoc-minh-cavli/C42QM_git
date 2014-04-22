/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                        M M G S D I  S E S S I O N  L I B R A R Y


GENERAL DESCRIPTION

  This file contains Library function that MMGSDI client can call to access
  various CSIM/RUIM functions. This file only contains functions related
  to 3GPP2 technology

INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header:

when       who     what, where, why
--------   ---     -----------------------------------------------------------
06/21/16   ar      Moving compute_ip_auth handler to 3GPP2 specific file
05/02/16   ar      Initial Version

=============================================================================*/

/*=============================================================================

                         INCLUDE FILES FOR MODULE

=============================================================================*/
#include "uim_variation.h"
#include "intconv.h"
#include "mmgsdisessionlib.h"
#include "mmgsdisessionlib_v.h"
#include "mmgsdi.h"
#include "uim_msg.h"
#include "mmgsdiutil.h"
#include "nvruimi.h"
#include "nvruim_p.h"
#include "mmgsdi_caller_context.h"
#include "mmgsdi_file.h"

/*=============================================================================

                                  FUNCTIONS

=============================================================================*/

/*===========================================================================
FUNCTION        MMGSDI_SESSION_RUN_CAVE_ALGO

DESCRIPTION
  This function will build a request to the MMGSDI Task to perform a run cave
  operation in the UIM

PARAMETERS
  session_id:        Session ID of the caller
  rand_type:         Type of Random Challenge
  rand_chal:         Rand Chal for CAVE Algo
  dig_len:           Length of digits provided
  digits:            Digits for CAVE calculation
  process_control:   Process Control for CAVE
  esn:               ESN for CAVE Calculation
  response_cb_ptr:   Response callback.
  client_ref:        User Data returned upon completion of this cmd.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

LIMITATIONS

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

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_session_run_cave_algo (
  mmgsdi_session_id_type             session_id,
  mmgsdi_cave_rand_type_type         rand_type,
  mmgsdi_cave_rand_chal_type         rand_chal,
  mmgsdi_cave_dig_len_type           dig_len,
  mmgsdi_cave_digits_type            digits,
  mmgsdi_cave_process_control_type   process_control,
  mmgsdi_cave_esn_type               esn,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
  mmgsdi_task_cmd_type              *task_cmd_ptr   = NULL;
  mmgsdi_run_cave_req_type          *msg_ptr        = NULL;
  mmgsdi_return_enum_type            mmgsdi_status  = MMGSDI_ERROR;
  uint32                             task_cmd_len   = 0;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Session ID
    2) Response CB function Check
    3) Memory allocation for msg_ptr and check for allocation
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  mmgsdi_status = mmgsdi_util_is_session_id_valid_in_cmd(
                    session_id,
                    MMGSDI_RUN_CAVE_REQ);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  task_cmd_len = sizeof(mmgsdi_task_cmd_type);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*---------------------------------------------------------------------------
    - Align pointers of the various structures members
  ---------------------------------------------------------------------------*/
  mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.run_cave_req,
                         MMGSDI_RUN_CAVE_REQ);

  msg_ptr = &task_cmd_ptr->cmd.cmd.run_cave_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_compute_ip_req_type header
      1) Session ID
      2) Request TYPE
      3) Client Data Pointer
      4) Response CallBack
      5) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_RUN_CAVE_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_RUN_CAVE_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_RUN_CAVE_REQ;
  msg_ptr->request_header.session_id        = session_id;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;
   /*---------------------------------------------------------------------------
    Populating Compute IP DATA
  ---------------------------------------------------------------------------*/
  msg_ptr->dig_len                          = dig_len;
  mmgsdi_memscpy(msg_ptr->digits,
                 sizeof(msg_ptr->digits),
                 digits,
                 sizeof(mmgsdi_cave_digits_type));
  qw_equ(msg_ptr->esn, esn);
  msg_ptr->process_control                  = process_control;
  msg_ptr->rand_chal                        = rand_chal;
  msg_ptr->rand_type                        = rand_type;

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);

  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_RUN_CAVE_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
}/* mmgsdi_session_run_cave_algo */ /*lint !e715*/ /*lint !e818*/


/*===========================================================================
FUNCTION        MMGSDI_SESSION_CDMA_AKA_AUTHENTICATE
DESCRIPTION
  This function will build a request to the MMGSDI Task to perform 3GPP2
  authentication and key agreement commands

PARAMETERS
  session_id:        Session ID of the caller
  aka_cmd:           Specifies the AKA command
  randa:             Random Anonymity Key
  autn:              Authentication Token
  response_cb_ptr:   Response callback.
  client_ref:        User Data returned upon completion of this cmd.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

LIMITATIONS

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

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_session_cdma_aka_authenticate(
  mmgsdi_session_id_type             session_id,
  mmgsdi_cdma_aka_cmd_enum_type      aka_cmd,
  mmgsdi_cdma_aka_randa_type         randa,
  mmgsdi_data_type                   autn,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
  mmgsdi_task_cmd_type              *task_cmd_ptr   = NULL;
  mmgsdi_cdma_aka_req_type          *msg_ptr        = NULL;
  mmgsdi_return_enum_type            mmgsdi_status  = MMGSDI_ERROR;
  int32                              total_mem_len  = 0;
  int32                              temp_mem       = 0;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Session ID
    2) Response CB function Check
    3) Memory allocation for msg_ptr and check for allocation
    4) Authentication token data check
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  if ((autn.data_len <= 0) || (autn.data_ptr == NULL))
  {
    UIM_MSG_ERR_0("Authenticate token data length or data invalid");
    return MMGSDI_INCORRECT_PARAMS;
  }

  mmgsdi_status = mmgsdi_util_is_session_id_valid_in_cmd(
                    session_id,
                    MMGSDI_SESSION_CDMA_AKA_REQ);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if (aka_cmd != MMGSDI_CDMA_AKA_AUTH_3G &&
      aka_cmd != MMGSDI_CDMA_AKA_AUTH_WLAN)
  {
    UIM_MSG_ERR_0("Invalid authentication command");
    return MMGSDI_INCORRECT_PARAMS;
  }

  total_mem_len = sizeof(mmgsdi_task_cmd_type);
  mmgsdi_util_cal_align_size(total_mem_len, &temp_mem);
  MMGSDIUTIL_RETURN_IF_ALIGNED_VAR_OUT_OF_BOUNDS(total_mem_len, temp_mem);

  total_mem_len = temp_mem + autn.data_len;

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, int32touint32(total_mem_len));
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*---------------------------------------------------------------------------
    - Align pointers of the various structures members
  ---------------------------------------------------------------------------*/
  mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.cdma_aka_req,
                         MMGSDI_SESSION_CDMA_AKA_REQ);

  msg_ptr = &task_cmd_ptr->cmd.cmd.cdma_aka_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_cdma_aka_req_type header
      1) Session ID
      2) Request TYPE
      3) Client Data Pointer
      4) Response CallBack
      5) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_SESSION_CDMA_AKA_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_SESSION_CDMA_AKA_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_SESSION_CDMA_AKA_REQ;
  msg_ptr->request_header.session_id        = session_id;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;
   /*---------------------------------------------------------------------------
    Populating AKA DATA
  ---------------------------------------------------------------------------*/
  msg_ptr->aka_cmd                          = aka_cmd;
  mmgsdi_memscpy(msg_ptr->randa,
                 sizeof(mmgsdi_cdma_aka_randa_type),
                 randa,
                 sizeof(mmgsdi_cdma_aka_randa_type));
  msg_ptr->autn.data_len                    = autn.data_len;
  mmgsdi_memscpy(msg_ptr->autn.data_ptr,
                 int32touint32(msg_ptr->autn.data_len),
                 autn.data_ptr,
                 int32touint32(autn.data_len));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_SESSION_CDMA_AKA_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
}/* mmgsdi_session_cdma_aka_authenticate */


/*===========================================================================
FUNCTION        MMGSDI_SESSION_GENERATE_KEY_VPM

DESCRIPTION
  This function will build a request to the MMGSDI Task to perform a generate
  key command in the UIM

PARAMETERS
  session_id:        Session ID of the caller
  first_octet:       First Octet of VPM to be output
  last_octet:        Last Octet of VPM to be output
  response_cb_ptr:   Response callback.
  client_ref:        User Data returned upon completion of this cmd.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

LIMITATIONS

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.
    MMGSDI_AUTH_ERROR_CAVE_NOT_RUN:  The command cannot be called unless
                             the RUN CAVE command has already been run with
                             SAVE Registers turned on

SIDE EFFECTS

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_session_generate_key_vpm (
  mmgsdi_session_id_type             session_id,
  mmgsdi_single_vpm_octet_type       first_octet,
  mmgsdi_single_vpm_octet_type       last_octet,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
  mmgsdi_task_cmd_type              *task_cmd_ptr   = NULL;
  mmgsdi_generate_key_req_type      *msg_ptr        = NULL;
  mmgsdi_return_enum_type            mmgsdi_status  = MMGSDI_ERROR;
  uint32                             task_cmd_len   = 0;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Session ID
    2) Response CB function Check
    3) Memory allocation for msg_ptr and check for allocation
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  mmgsdi_status = mmgsdi_util_is_session_id_valid_in_cmd(
                    session_id,
                    MMGSDI_GENERATE_KEY_VPM_REQ);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  task_cmd_len = sizeof(mmgsdi_task_cmd_type);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  msg_ptr = &task_cmd_ptr->cmd.cmd.generate_key_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_generate_key_req_type header
      1) Session ID
      2) Request TYPE
      3) Client Data Pointer
      4) Response CallBack
      5) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_GENERATE_KEY_VPM_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_GENERATE_KEY_VPM_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_GENERATE_KEY_VPM_REQ;
  msg_ptr->request_header.session_id        = session_id;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;
   /*---------------------------------------------------------------------------
    Populating Generate Key DATA
  ---------------------------------------------------------------------------*/
  msg_ptr->first_octet                      = first_octet;
  msg_ptr->last_octet                       = last_octet;

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put
       onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);

  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_GENERATE_KEY_VPM_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
}/* mmgsdi_session_generate_key_vpm */


/*================================================================
FUNCTION:      MMGSDI_SESSION_SSD_UPDATE

DESCRIPTION:
  This API is used to perform an SSD Update

PARAMETERS:
  session_id     : Session Id of the caller
  randssd        : 56 bits from BS for SSD Generation
  process_control: process control bits
  esn:           : esn
  response_cb_ptr: Call back function when the command execution is done.
  client_ref     : User Data returned upon completion of this cmd.

DEPENDENCIES:
  A valid 1X Session ID is required, which can be achieved by calling
  mmgsdi_session_open().
  The session ID should only be that of a provisioning application

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type:
    MMGSDI_SUCCESS:          The command structure was properly
                             Generated and queued onto the MMGSDI
                             Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API
                             are not within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the
                             MMGSDI Task because the max number of
                             commands are already queued.

SIDE EFFECTS:
  None
================================================================*/
mmgsdi_return_enum_type mmgsdi_session_ssd_update(
  mmgsdi_session_id_type               session_id,
  qword                                randssd,
  byte                                 process_control,
  dword                                esn,
  mmgsdi_callback_type                 response_cb_ptr,
  mmgsdi_client_data_type              client_ref
)
{
  mmgsdi_task_cmd_type                  * task_cmd_ptr   = NULL;
  mmgsdi_session_ssd_update_req_type    * msg_ptr        = NULL;
  mmgsdi_return_enum_type                 mmgsdi_status  = MMGSDI_ERROR;
  uint32                                  task_cmd_len   = 0;

  /*---------------------------------------------
    Input Parameters Checking
    1) session_id           Shall be a valid session id.
    2) randssd              Not required
    3) process_control      Not required
    2) esn                  Shall not be 0.
    5) response_cb_ptr      Shall not be NULL
    6) client_ref           Not required
  -----------------------------------------------*/
  if (0 == esn)
  {
    UIM_MSG_ERR_0("ESN value shall not be 0");
    return MMGSDI_INCORRECT_PARAMS;
  }

  /* response_cb_ptr checking */
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  mmgsdi_status = mmgsdi_util_is_session_id_valid_in_cmd(
                    session_id,
                    MMGSDI_SESSION_SSD_UPDATE_REQ);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /*------------------------------------------------------------------------
   - Calculating memory to Task Command Pointer
   -------------------------------------------------------------------------*/
  task_cmd_len = sizeof(mmgsdi_task_cmd_type);

  /*------------------------------------------------------------------------
   - Allocating memory to Task Command Pointer
   -------------------------------------------------------------------------*/
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*------------------------------------------------------------------------
    Align Memory
  -------------------------------------------------------------------------*/
  mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.session_ssd_update_req,
                         MMGSDI_SESSION_SSD_UPDATE_REQ);

  /* -----------------------------------------------------------------------
  Populate Task data , Take the pointer of search request
  -------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum = MMGSDI_SESSION_SSD_UPDATE_REQ;
  msg_ptr                    = &task_cmd_ptr->cmd.cmd.session_ssd_update_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_session_ssd_update_req_type header
    1) Session ID
    2) Request TYPE
    3) Client Data Pointer
    4) Response CallBack
    5) Payload len
    6) request len
  ---------------------------------------------------------------------------*/
  /* Updating Request Header */
  msg_ptr->request_header.session_id        = session_id;
  msg_ptr->request_header.orig_request_type = MMGSDI_SESSION_SSD_UPDATE_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_SESSION_SSD_UPDATE_REQ;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /* Updating SSD Update Request data */
  msg_ptr->randssd[0]                       = randssd[0];
  msg_ptr->randssd[1]                       = randssd[1];
  msg_ptr->process_control                  = process_control;
  msg_ptr->esn                              = esn;

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the
       command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);

  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_SESSION_SSD_UPDATE_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
}/* mmgsdi_session_ssd_update */


/*================================================================
FUNCTION:      MMGSDI_SESSION_CONFIRM_SSD

DESCRIPTION:
  This API is used to perform an CONFIRM SSD

PARAMETERS:
  session_id     : Session Id of the caller
  authbs         : Auth Signature from Base Station
  response_cb_ptr: Call back function when the command execution is done.
  client_ref     : User Data returned upon completion of this cmd.

DEPENDENCIES:
  A valid 1X Session ID is required, which can be achieved by calling
  mmgsdi_session_open().
  The session ID should only be that of a provisioning application

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type:
    MMGSDI_SUCCESS:          The command structure was properly
                             Generated and queued onto the MMGSDI
                             Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API
                             are not within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the
                             MMGSDI Task because the max number of
                             commands are already queued.

SIDE EFFECTS:
  None
================================================================*/
mmgsdi_return_enum_type mmgsdi_session_confirm_ssd(
  mmgsdi_session_id_type               session_id,
  dword                                authbs,
  mmgsdi_callback_type                 response_cb_ptr,
  mmgsdi_client_data_type              client_ref
)
{
  mmgsdi_task_cmd_type                  * task_cmd_ptr   = NULL;
  mmgsdi_session_ssd_confirm_req_type   * msg_ptr        = NULL;
  mmgsdi_return_enum_type                 mmgsdi_status  = MMGSDI_ERROR;
  uint32                                  task_cmd_len   = 0;

  /*---------------------------------------------
    Input Parameters Checking
    1) session_id           Shall be a valid session id.
    2) authbs               Not required
    3) response_cb_ptr      Shall not be NULL
    4) client_ref           Not required
  -----------------------------------------------*/
  /* response_cb_ptr checking */
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  /* session id checking */
  mmgsdi_status = mmgsdi_util_is_session_id_valid_in_cmd(
                    session_id,
                    MMGSDI_SESSION_SSD_CONFIRM_REQ);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /*------------------------------------------------------------------------
   - Calculating memory to Task Command Pointer
   -------------------------------------------------------------------------*/
  task_cmd_len = sizeof(mmgsdi_task_cmd_type);

  /*------------------------------------------------------------------------
   - Allocating memory to Task Command Pointer
   -------------------------------------------------------------------------*/
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*------------------------------------------------------------------------
    Align Memory
  -------------------------------------------------------------------------*/
  mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.session_ssd_confirm_req,
                         MMGSDI_SESSION_SSD_CONFIRM_REQ);

  /* -----------------------------------------------------------------------
  Populate Task data , Take the pointer of search request
  -------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum = MMGSDI_SESSION_SSD_CONFIRM_REQ;
  msg_ptr                    = &task_cmd_ptr->cmd.cmd.session_ssd_confirm_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_session_ssd_confirm_req_type header
    1) Session ID
    3) Request TYPE
    4) Client Data Pointer
    5) Response CallBack
    6) Payload len
    7) request len
  ---------------------------------------------------------------------------*/
  /* Updating Request Header */
  msg_ptr->request_header.session_id        = session_id;
  msg_ptr->request_header.orig_request_type = MMGSDI_SESSION_SSD_CONFIRM_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_SESSION_SSD_CONFIRM_REQ;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /* Updating SSD Confirm Request data */
  msg_ptr->authbs                           = authbs;

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);

  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_SESSION_SSD_CONFIRM_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
}/* mmgsdi_session_confirm_ssd */


/*================================================================
FUNCTION:      MMGSDI_SESSION_BS_CHAL

DESCRIPTION:
  This API is used to perform an BS Challenge

PARAMETERS:
  session_id     : Session Id of the caller
  randseed       : Random number generator seed
  response_cb_ptr: Call back function when the command execution is done.
  client_ref     : User Data returned upon completion of this cmd.

DEPENDENCIES:
  A valid 1X Session ID is required, which can be achieved by calling
  mmgsdi_session_open().
  The session ID should only be that of a provisioning application

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type:
    MMGSDI_SUCCESS:          The command structure was properly
                             Generated and queued onto the MMGSDI
                             Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API
                             are not within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the
                             MMGSDI Task because the max number of
                             commands are already queued.

SIDE EFFECTS:
  None
================================================================*/
mmgsdi_return_enum_type mmgsdi_session_bs_chal(
  mmgsdi_session_id_type               session_id,
  dword                                randseed,
  mmgsdi_callback_type                 response_cb_ptr,
  mmgsdi_client_data_type              client_ref
)
{
  mmgsdi_task_cmd_type                  * task_cmd_ptr   = NULL;
  mmgsdi_session_bs_chal_req_type       * msg_ptr        = NULL;
  mmgsdi_return_enum_type                 mmgsdi_status  = MMGSDI_ERROR;
  uint32                                  task_cmd_len   = 0;

  /*---------------------------------------------
    Input Parameters Checking
    1) session_id           Shall be a valid session id.
    2) response_cb_ptr      Shall not be NULL
    3) client_ref           Not required
  -----------------------------------------------*/

  /* response_cb_ptr checking */
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  /* session id checking */
  mmgsdi_status = mmgsdi_util_is_session_id_valid_in_cmd(
                    session_id,
                    MMGSDI_SESSION_BS_CHAL_REQ);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /*------------------------------------------------------------------------
   - Calculating memory to Task Command Pointer
   -------------------------------------------------------------------------*/
  task_cmd_len = sizeof(mmgsdi_task_cmd_type);

  /*------------------------------------------------------------------------
   - Allocating memory to Task Command Pointer
   -------------------------------------------------------------------------*/
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*------------------------------------------------------------------------
    Align Memory
  -------------------------------------------------------------------------*/
  mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.session_bs_chal_req,
                         MMGSDI_SESSION_BS_CHAL_REQ);

  /* -----------------------------------------------------------------------
  Populate Task data , Take the pointer of search request
  -------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum = MMGSDI_SESSION_BS_CHAL_REQ;
  msg_ptr                    = &task_cmd_ptr->cmd.cmd.session_bs_chal_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_session_bs_chal_req_type header
    1) Session ID
    2) Request TYPE
    3) Client Data Pointer
    4) Response CallBack
    5) Payload len
    6) request len
  ---------------------------------------------------------------------------*/
  /* Updating Request Header */
  msg_ptr->request_header.session_id        = session_id;
  msg_ptr->request_header.orig_request_type = MMGSDI_SESSION_BS_CHAL_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_SESSION_BS_CHAL_REQ;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /* Updating BS CHAL Request data */
  msg_ptr->randseed                         = randseed;

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto
       the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);

  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_SESSION_BS_CHAL_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
}/* mmgsdi_session_bs_chal */


/*================================================================
FUNCTION:      MMGSDI_SESSION_OTASP_MS_KEY_REQ

DESCRIPTION:
  This API is used to perform an OTASP MS Key Request

PARAMETERS:
  session_id     : Session Id of the caller
  randseed       : Seed used to generate TRUE random no
  a_key_p_rev    : A-KEY-P_REV supported by BS
  param_p        : Param P
  param_g        : Param G
  response_cb_ptr: Call back function when the command execution is done.
  client_ref     : User Data returned upon completion of this cmd.

DEPENDENCIES:
  A valid 1X Session ID is required, which can be achieved by calling
  mmgsdi_session_open().
  The session ID should only be that of a provisioning application

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type:
    MMGSDI_SUCCESS:          The command structure was properly
                             Generated and queued onto the MMGSDI
                             Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API
                             are not within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the
                             MMGSDI Task because the max number of
                             commands are already queued.

SIDE EFFECTS:
  None
================================================================*/
mmgsdi_return_enum_type mmgsdi_session_otasp_ms_key_req(
  mmgsdi_session_id_type                session_id,
  mmgsdi_data_type                      randseed,
  byte                                  a_key_p_rev,
  mmgsdi_data_type                      param_p,
  mmgsdi_data_type                      param_g,
  mmgsdi_callback_type                  response_cb_ptr,
  mmgsdi_client_data_type               client_ref
)
{
  mmgsdi_task_cmd_type                  * task_cmd_ptr   = NULL;
  mmgsdi_session_otasp_ms_key_req_type  * msg_ptr        = NULL;
  mmgsdi_return_enum_type                 mmgsdi_status  = MMGSDI_ERROR;
  int32                                   total_mem_len  = 0;
  int32                                   var_len        = 0;
  int32                                   temp_mem       = 0;

  /*---------------------------------------------
    Input Parameters Checking
    1) session_id           Shall be a valid session id.
    2) response_cb_ptr      Shall not be NULL
    3) client_ref           Not required
    4) randseed             Data length shall be in range of 1 to UIM_RAND_SEED
                            Data ptr shall not be NULL.
    5) param_p              Data length shall be in range of 1 to UIM_PARAM_P
                            Data ptr shall not be NULL.
    6) param_g              Data length shall be in range of 1 to UIM_PARAM_G
                            Data ptr shall not be NULL.
    7) a_key_p_rev          Not Required
  -----------------------------------------------*/
  /* randseed checking */
  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(randseed.data_len,1, (UIM_RAND_SEED +1));
  MMGSDIUTIL_RETURN_IF_NULL(randseed.data_ptr);

  if(a_key_p_rev <= MMGSDI_A_KEY_P_REV)
  {
    /* param_p checking */
    MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(param_p.data_len, 1, (UIM_PARAM_P + 1));
    MMGSDIUTIL_RETURN_IF_NULL(param_p.data_ptr);

    /* param_g checking */
    MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(param_g.data_len, 1, (UIM_PARAM_G + 1));
    MMGSDIUTIL_RETURN_IF_NULL(param_g.data_ptr);
  }
  else
  {
    param_p.data_len = 0;
    param_g.data_len = 0;
    param_p.data_ptr = NULL;
    param_g.data_ptr = NULL;
  }

  /* response_cb_ptr checking */
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  /* session id checking */
  mmgsdi_status = mmgsdi_util_is_session_id_valid_in_cmd(
                    session_id,
                    MMGSDI_SESSION_OTASP_MS_KEY_REQ);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /*------------------------------------------------------------------------
   - Calculating memory to Task Command Pointer
   -------------------------------------------------------------------------*/
  mmgsdi_util_cal_align_size(randseed.data_len, &temp_mem);
  MMGSDIUTIL_RETURN_IF_ALIGNED_VAR_OUT_OF_BOUNDS(randseed.data_len, temp_mem);
  var_len = temp_mem;
  if(a_key_p_rev <= MMGSDI_A_KEY_P_REV)
  {
    mmgsdi_util_cal_align_size(param_p.data_len, &temp_mem);
    MMGSDIUTIL_RETURN_IF_ALIGNED_VAR_OUT_OF_BOUNDS(param_p.data_len, temp_mem);
    var_len += temp_mem;
    mmgsdi_util_cal_align_size(param_g.data_len, &temp_mem);
    MMGSDIUTIL_RETURN_IF_ALIGNED_VAR_OUT_OF_BOUNDS(param_g.data_len, temp_mem);
    var_len += temp_mem;
  }

  total_mem_len = sizeof(mmgsdi_task_cmd_type);
  mmgsdi_util_cal_align_size(total_mem_len, &temp_mem);
  MMGSDIUTIL_RETURN_IF_ALIGNED_VAR_OUT_OF_BOUNDS(total_mem_len, temp_mem);

  total_mem_len = temp_mem + var_len;

  /*------------------------------------------------------------------------
   - Allocating memory to Task Command Pointer
   -------------------------------------------------------------------------*/
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, int32touint32(total_mem_len));
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /* -----------------------------------------------------------------------
  Populate Task data , Take the pointer of search request
  -------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum = MMGSDI_SESSION_OTASP_MS_KEY_REQ;
  msg_ptr                    = &task_cmd_ptr->cmd.cmd.session_otasp_ms_key_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_session_otasp_ms_key_req_type header
    1) Session ID
    2) Request TYPE
    3) Client Data Pointer
    4) Response CallBack
    5) Payload len
    6) request len
  ---------------------------------------------------------------------------*/
  /* Updating Request Header */
  msg_ptr->request_header.session_id        = session_id;
  msg_ptr->request_header.orig_request_type = MMGSDI_SESSION_OTASP_MS_KEY_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_SESSION_OTASP_MS_KEY_REQ;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /* Updating OTAPA Request data */
  msg_ptr->randseed.data_len                = randseed.data_len;
  msg_ptr->a_key_p_rev                      = a_key_p_rev;
  msg_ptr->param_p.data_len                 = param_p.data_len;
  msg_ptr->param_g.data_len                 = param_g.data_len;

  /*------------------------------------------------------------------------
    Align Memory for variable data length
  -------------------------------------------------------------------------*/
  mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.session_otasp_ms_key_req,
                         MMGSDI_SESSION_OTASP_MS_KEY_REQ);

  mmgsdi_memscpy(msg_ptr->randseed.data_ptr,
                 int32touint32(msg_ptr->randseed.data_len),
                 randseed.data_ptr,
                 int32touint32(randseed.data_len));

  if(a_key_p_rev <= MMGSDI_A_KEY_P_REV)
  {
    mmgsdi_memscpy(msg_ptr->param_p.data_ptr,
                   int32touint32(msg_ptr->param_p.data_len),
                   param_p.data_ptr,
                   int32touint32(param_p.data_len));

    mmgsdi_memscpy(msg_ptr->param_g.data_ptr,
                   int32touint32(msg_ptr->param_g.data_len),
                   param_g.data_ptr,
                   int32touint32(param_g.data_len));
  }

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto
       the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);

  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_SESSION_OTASP_MS_KEY_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
}/* mmgsdi_session_otasp_ms_key_req */


/*================================================================
FUNCTION:      MMGSDI_SESSION_OTASP_KEY_GEN_REQ

DESCRIPTION:
  This API is used to perform an OTASP Key Gen Request

PARAMETERS:
  session_id     : Session Id of the caller
  bs_result      : BS Result
  response_cb_ptr: Call back function when the command execution is done.
  client_ref     : User Data returned upon completion of this cmd.

DEPENDENCIES:
  A valid 1X Session ID is required, which can be achieved by calling
  mmgsdi_session_open().
  The session ID should only be that of a provisioning application

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type:
    MMGSDI_SUCCESS:          The command structure was properly
                             Generated and queued onto the MMGSDI
                             Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API
                             are not within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the
                             MMGSDI Task because the max number of
                             commands are already queued.

SIDE EFFECTS:
  None
================================================================*/
mmgsdi_return_enum_type mmgsdi_session_otasp_key_gen_req(
  mmgsdi_session_id_type                session_id,
  mmgsdi_data_type                      bs_result,
  mmgsdi_callback_type                  response_cb_ptr,
  mmgsdi_client_data_type               client_ref
)
{
  mmgsdi_task_cmd_type                  * task_cmd_ptr   = NULL;
  mmgsdi_session_otasp_key_gen_req_type * msg_ptr        = NULL;
  mmgsdi_return_enum_type                 mmgsdi_status  = MMGSDI_ERROR;
  uint32                                  task_cmd_len   = 0;
  int32                                   var_len        = 0;
  int32                                   temp_mem       = 0;

  /*---------------------------------------------
    Input Parameters Checking
    1) session_id           Shall be a valid session id.
    2) response_cb_ptr      Shall not be NULL
    3) client_ref           Not required
    4) bs_result            Data length shall be in range of 1 to UIM_PARAM_P
                            Data ptr shall not be NULL.
  -----------------------------------------------*/
  /* bs_result checking */
  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(bs_result.data_len,1, (UIM_PARAM_P + 1));
  MMGSDIUTIL_RETURN_IF_NULL(bs_result.data_ptr);

  /* response_cb_ptr checking */
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  /* session id checking */
  mmgsdi_status = mmgsdi_util_is_session_id_valid_in_cmd(
                    session_id,
                    MMGSDI_SESSION_OTASP_KEY_GEN_REQ);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /*------------------------------------------------------------------------
   - Calculating memory to Task Command Pointer
   -------------------------------------------------------------------------*/
  var_len                                   = bs_result.data_len;
  task_cmd_len                              = sizeof(mmgsdi_task_cmd_type);
  mmgsdi_util_cal_align_size(var_len,&temp_mem);
  MMGSDIUTIL_RETURN_IF_ALIGNED_VAR_OUT_OF_BOUNDS(var_len,temp_mem);
  task_cmd_len                             += temp_mem;

  /*------------------------------------------------------------------------
   - Allocating memory to Task Command Pointer
   -------------------------------------------------------------------------*/
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*------------------------------------------------------------------------
    Align Memory
  -------------------------------------------------------------------------*/
  mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.session_otasp_key_gen_req,
                         MMGSDI_SESSION_OTASP_KEY_GEN_REQ);

  /* -----------------------------------------------------------------------
  Populate Task data , Take the pointer of search request
  -------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum = MMGSDI_SESSION_OTASP_KEY_GEN_REQ;
  msg_ptr                    = &task_cmd_ptr->cmd.cmd.session_otasp_key_gen_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_session_otasp_key_gen_req_type header
    1) Session ID
    2) Request TYPE
    3) Client Data Pointer
    4) Response CallBack
    5) Payload len
    6) request len
  ---------------------------------------------------------------------------*/
  /* Updating Request Header */
  msg_ptr->request_header.session_id        = session_id;
  msg_ptr->request_header.orig_request_type = MMGSDI_SESSION_OTASP_KEY_GEN_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_SESSION_OTASP_KEY_GEN_REQ;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /* Updating OTAPA Request data */
  msg_ptr->bs_result.data_len               = bs_result.data_len;
  mmgsdi_memscpy(msg_ptr->bs_result.data_ptr,
                 int32touint32(msg_ptr->bs_result.data_len),
                 bs_result.data_ptr,
                 int32touint32(bs_result.data_len));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto
       the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);

  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_SESSION_OTASP_KEY_GEN_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
}/* mmgsdi_session_otasp_key_gen_req */


/*================================================================
FUNCTION:      MMGSDI_SESSION_OTASP_SSPR_CONFIG_REQ

DESCRIPTION:
  This API is used to perform an OTASP SSPR Config Request

PARAMETERS:
  session_id     : Session Id of the caller
  block_id       : Block ID of the block
  req_offset     : Requested offset in the PRL
  req_max_size   : Requested Max size in the response
  response_cb_ptr: Call back function when the command execution is done.
  client_ref     : User Data returned upon completion of this cmd.

DEPENDENCIES:
  A valid 1X Session ID is required, which can be achieved by calling
  mmgsdi_session_open().
  The session ID should only be that of a provisioning application

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type:
    MMGSDI_SUCCESS:          The command structure was properly
                             Generated and queued onto the MMGSDI
                             Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API
                             are not within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the
                             MMGSDI Task because the max number of
                             commands are already queued.

SIDE EFFECTS:
  None
================================================================*/
mmgsdi_return_enum_type mmgsdi_session_otasp_sspr_config_req(
  mmgsdi_session_id_type                session_id,
  byte                                  block_id,
  word                                  req_offset,
  byte                                  req_max_size,
  mmgsdi_callback_type                  response_cb_ptr,
  mmgsdi_client_data_type               client_ref
)
{
  mmgsdi_task_cmd_type                        * task_cmd_ptr   = NULL;
  mmgsdi_session_otasp_sspr_config_req_type   * msg_ptr        = NULL;
  mmgsdi_return_enum_type                       mmgsdi_status  = MMGSDI_ERROR;
  uint32                                        task_cmd_len   = 0;

  /*---------------------------------------------
    Input Parameters Checking
    1) session_id           Shall be a valid session id.
    2) response_cb_ptr      Shall not be NULL
    3) client_ref           Not required
    4) block id             Not required
    5) req_offset           Not required
    6) req_max_size         Not required
  -----------------------------------------------*/
  /* response_cb_ptr checking */
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  /* session id checking */
  mmgsdi_status = mmgsdi_util_is_session_id_valid_in_cmd(
                    session_id,
                    MMGSDI_SESSION_OTASP_SSPR_CONFIG_REQ);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /*------------------------------------------------------------------------
   - Calculating memory to Task Command Pointer
   -------------------------------------------------------------------------*/
  task_cmd_len = sizeof(mmgsdi_task_cmd_type);

  /*------------------------------------------------------------------------
   - Allocating memory to Task Command Pointer
   -------------------------------------------------------------------------*/
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*------------------------------------------------------------------------
    Align Memory
  -------------------------------------------------------------------------*/
  mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.session_otasp_sspr_config_req,
                         MMGSDI_SESSION_OTASP_SSPR_CONFIG_REQ);

  /* -----------------------------------------------------------------------
  Populate Task data , Take the pointer of search request
  -------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum = MMGSDI_SESSION_OTASP_SSPR_CONFIG_REQ;
  msg_ptr                    = &task_cmd_ptr->cmd.cmd.session_otasp_sspr_config_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_session_otasp_sspr_config_req_type header
    1) Session ID
    2) Request TYPE
    3) Client Data Pointer
    4) Response CallBack
    5) Payload len
    6) request len
  ---------------------------------------------------------------------------*/
  /* Updating Request Header */
  msg_ptr->request_header.session_id        = session_id;
  msg_ptr->request_header.orig_request_type = MMGSDI_SESSION_OTASP_SSPR_CONFIG_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_SESSION_OTASP_SSPR_CONFIG_REQ;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /* Updating OTAPA SSPR Config Request data */
  msg_ptr->block_id                         = block_id;
  msg_ptr->req_offset                       = req_offset;
  msg_ptr->req_max_size                     = req_max_size;
  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto
       the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);

  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_SESSION_OTASP_SSPR_CONFIG_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
}/* mmgsdi_session_otasp_sspr_config_req */


/*================================================================
FUNCTION:      MMGSDI_SESSION_OTASP_BLOCK_REQ

DESCRIPTION:
  This API is used to perform an OTASP BLOCK Request.  This will
  be used to perform a CONFIG REQUEST, DOWNLOAD REQUEST, SSPR
  DOWNLOAD REQUEST, or VALIDATE

PARAMETERS:
  session_id           : Session Id of the caller
  otasp_block_op_type  : Config, Download, SSPR download, or Validate
  block_id             : Block ID for request
  block_data           : Block DATA (only for download, sspr download, or validate)
  response_cb_ptr      : Call back function when execution is done.
  client_ref           : User Data returned upon completion of this cmd.

DEPENDENCIES:
  A valid 1X Session ID is required, which can be achieved by calling
  mmgsdi_session_open().
  The session ID should only be that of a provisioning application

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type:
    MMGSDI_SUCCESS:          The command structure was properly
                             Generated and queued onto the MMGSDI
                             Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API
                             are not within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the
                             MMGSDI Task because the max number of
                             commands are already queued.

SIDE EFFECTS:
  None
================================================================*/
mmgsdi_return_enum_type mmgsdi_session_otasp_block_req(
  mmgsdi_session_id_type                session_id,
  mmgsdi_otasp_block_op_enum_type       otasp_block_op_type,
  byte                                  block_id,
  mmgsdi_data_type                      block_data,
  mmgsdi_callback_type                  response_cb_ptr,
  mmgsdi_client_data_type               client_ref
)
{
  mmgsdi_task_cmd_type                  * task_cmd_ptr   = NULL;
  mmgsdi_session_otasp_block_req_type   * msg_ptr        = NULL;
  mmgsdi_return_enum_type                 mmgsdi_status  = MMGSDI_ERROR;
  int32                                   total_mem_len  = 0;
  int32                                   temp_mem       = 0;

  /*---------------------------------------------
    Input Parameters Checking
    1) session_id           Shall be a valid session id.
    2) otasp_block_op_type  Shall be in valid block op range
    3) block_id             Not required
    4) block_data           Shall not be NULL for data len
    5) response_cb_ptr      Shall not be NULL
    6) client_ref           Not required
  -----------------------------------------------*/
  /* block op type and block data checking */
  switch(otasp_block_op_type)
  {
    case MMGSDI_OTASP_CONFIG:        /* Perform config */
      /* It does not require block data, ignore if we have any block length */
      block_data.data_len =0;
      break;

    case MMGSDI_OTASP_DOWNLOAD:      /* Perform generic Download */
    case MMGSDI_OTASP_SSPR_DOWNLOAD: /* Perform SSPR Download */
    case MMGSDI_OTASP_VALIDATE:      /* Perform Validate */
      MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(block_data.data_len, 0, UIM_MAX_CHARS);
      if (block_data.data_len)
      {
        /* data ptr should not be NULL */
        MMGSDIUTIL_RETURN_IF_NULL(block_data.data_ptr);
      }
      break;

    default:
      UIM_MSG_ERR_1("Invalid value of otasp block op type 0x%x",
                    otasp_block_op_type);
      return MMGSDI_INCORRECT_PARAMS;
  }

  /* response_cb_ptr checking */
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  /* session id checking */
  mmgsdi_status = mmgsdi_util_is_session_id_valid_in_cmd(
                    session_id,
                    MMGSDI_SESSION_OTASP_BLOCK_REQ);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /*------------------------------------------------------------------------
   - Calculating memory to Task Command Pointer
   -------------------------------------------------------------------------*/
  total_mem_len = sizeof(mmgsdi_task_cmd_type);
  mmgsdi_util_cal_align_size(total_mem_len, &temp_mem);
  MMGSDIUTIL_RETURN_IF_ALIGNED_VAR_OUT_OF_BOUNDS(total_mem_len, temp_mem);

  total_mem_len = temp_mem + block_data.data_len;

  /*------------------------------------------------------------------------
   - Allocating memory to Task Command Pointer
   -------------------------------------------------------------------------*/
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, int32touint32(total_mem_len));
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*------------------------------------------------------------------------
    Align Memory
  -------------------------------------------------------------------------*/
  mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.session_otasp_block_req,
                         MMGSDI_SESSION_OTASP_BLOCK_REQ);

  /* -----------------------------------------------------------------------
  Populate Task data , Take the pointer of search request
  -------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum = MMGSDI_SESSION_OTASP_BLOCK_REQ;
  msg_ptr                    = &task_cmd_ptr->cmd.cmd.session_otasp_block_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_session_ota_op_otapa_req_type header
    1) Session ID
    2) Request TYPE
    3) Client Data Pointer
    4) Response CallBack
    5) Payload len
    6) request len
  ---------------------------------------------------------------------------*/
  /* Updating Request Header */
  msg_ptr->request_header.session_id        = session_id;
  msg_ptr->request_header.orig_request_type = MMGSDI_SESSION_OTASP_BLOCK_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_SESSION_OTASP_BLOCK_REQ;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /* Updating OTAPA Request data */
  msg_ptr->block_id                         = block_id;
  msg_ptr->block_op_type                    = otasp_block_op_type;
  msg_ptr->block_data.data_len              = block_data.data_len;
  if (block_data.data_ptr)
  {
    mmgsdi_memscpy(msg_ptr->block_data.data_ptr,
                   int32touint32(msg_ptr->block_data.data_len),
                   block_data.data_ptr,
                   int32touint32(block_data.data_len));
  }
  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);

  }

  UIM_MSG_HIGH_2("Queue of MMGSDI command: MMGSDI_SESSION_OTASP_BLOCK_REQ 0x%x status 0x%x",
                 otasp_block_op_type, mmgsdi_status);

  return mmgsdi_status;
}/* mmgsdi_session_otasp_block_req */


/*================================================================
FUNCTION:      MMGSDI_SESSION_OTASP_COMMIT_REQ

DESCRIPTION:
  This API is used to perform an OTASP Commit Request

PARAMETERS:
  session_id     : Session Id of the caller
  response_cb_ptr: Call back function when the command execution is done.
  client_ref     : User Data returned upon completion of this cmd.

DEPENDENCIES:
  A valid 1X Session ID is required, which can be achieved by calling
  mmgsdi_session_open().
  The session ID should only be that of a provisioning application

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type:
    MMGSDI_SUCCESS:          The command structure was properly
                             Generated and queued onto the MMGSDI
                             Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API
                             are not within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the
                             MMGSDI Task because the max number of
                             commands are already queued.

SIDE EFFECTS:
  None
================================================================*/
mmgsdi_return_enum_type mmgsdi_session_otasp_commit_req(
  mmgsdi_session_id_type                session_id,
  mmgsdi_callback_type                  response_cb_ptr,
  mmgsdi_client_data_type               client_ref
)
{
  mmgsdi_task_cmd_type                  * task_cmd_ptr   = NULL;
  mmgsdi_session_otasp_commit_req_type  * msg_ptr        = NULL;
  mmgsdi_return_enum_type                 mmgsdi_status  = MMGSDI_ERROR;
  uint32                                  task_cmd_len   = 0;

  /*---------------------------------------------
    Input Parameters Checking
    1) session_id           Shall be a valid session id.
    2) response_cb_ptr      Shall not be NULL
    3) client_ref           Not required
  -----------------------------------------------*/
  /* response_cb_ptr checking */
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  /* session id checking */
  mmgsdi_status = mmgsdi_util_is_session_id_valid_in_cmd(
                    session_id,
                    MMGSDI_SESSION_OTASP_COMMIT_REQ);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /*------------------------------------------------------------------------
   - Calculating memory to Task Command Pointer
   -------------------------------------------------------------------------*/
  task_cmd_len = sizeof(mmgsdi_task_cmd_type);

  /*------------------------------------------------------------------------
   - Allocating memory to Task Command Pointer
   -------------------------------------------------------------------------*/
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*------------------------------------------------------------------------
    Align Memory
  -------------------------------------------------------------------------*/
  mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.session_otasp_commit_req,
                         MMGSDI_SESSION_OTASP_COMMIT_REQ);

  /* -----------------------------------------------------------------------
  Populate Task data , Take the pointer of search request
  -------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum = MMGSDI_SESSION_OTASP_COMMIT_REQ;
  msg_ptr                    = &task_cmd_ptr->cmd.cmd.session_otasp_commit_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_session_otasp_commit_req_type header
    1) Session ID
    2) Request TYPE
    3) Client Data Pointer
    4) Response CallBack
    5) Payload len
    6) request len
  ---------------------------------------------------------------------------*/
  /* Updating Request Header */
  msg_ptr->request_header.session_id        = session_id;
  msg_ptr->request_header.orig_request_type = MMGSDI_SESSION_OTASP_COMMIT_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_SESSION_OTASP_COMMIT_REQ;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto
       the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);

  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_SESSION_OTASP_COMMIT_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
}/* mmgsdi_session_otasp_commit_req */


/*================================================================
FUNCTION:      MMGSDI_SESSION_OTASP_OTAPA_REQ

DESCRIPTION:
  This API is used to perform an OTASP OTAPA Request

PARAMETERS:
  session_id     : Session Id of the caller
  start_stop     : OTAPA Start Stop
  randseed       : Random seed
  esn            : ESN or Pseudo ESN
  response_cb_ptr: Call back function when the command execution is done.
  client_ref     : User Data returned upon completion of this cmd.

DEPENDENCIES:
  A 1X valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().
  The session ID should only be that of a provisioning application

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type:
    MMGSDI_SUCCESS:          The command structure was properly
                             Generated and queued onto the MMGSDI
                             Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API
                             are not within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the
                             MMGSDI Task because the max number of
                             commands are already queued.

SIDE EFFECTS:
  None
================================================================*/
mmgsdi_return_enum_type mmgsdi_session_otasp_otapa_req(
  mmgsdi_session_id_type                session_id,
  byte                                  start_stop,
  dword                                 randseed,
  dword                                 esn,
  mmgsdi_callback_type                  response_cb_ptr,
  mmgsdi_client_data_type               client_ref
)
{
  mmgsdi_task_cmd_type                  * task_cmd_ptr = NULL;
  mmgsdi_session_otasp_otapa_req_type   * msg_ptr      = NULL;
  mmgsdi_return_enum_type               mmgsdi_status  = MMGSDI_ERROR;
  uint32                                task_cmd_len   = 0;

  /*---------------------------------------------
    Input Parameters Checking
   -----------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_session_id_valid_in_cmd(
                    session_id,
                    MMGSDI_SESSION_OTASP_OTAPA_REQ);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* start_stop checking not required,
     its possible values are 0x00 or 0x80 value */
  start_stop = start_stop & 0x80;

  if (0x00 == randseed)
  {
    UIM_MSG_ERR_0("Random number should not be 0x00");
    return MMGSDI_INCORRECT_PARAMS;
  }
  /* ESN may be 0 for ICC Card, where card does not have the MEID support */

  /* response_cb_ptr checking */
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  /*------------------------------------------------------------------------
  - Allocating memory to Task Command Pointer
  -------------------------------------------------------------------------*/
  task_cmd_len = sizeof(mmgsdi_task_cmd_type);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*------------------------------------------------------------------------
    Align Memory
  -------------------------------------------------------------------------*/
  mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.session_otasp_otapa_req,
                         MMGSDI_SESSION_OTASP_OTAPA_REQ);

  /* -----------------------------------------------------------------------
  Populate Task data , Take the pointer of search request
  -------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum = MMGSDI_SESSION_OTASP_OTAPA_REQ;
  msg_ptr = &task_cmd_ptr->cmd.cmd.session_otasp_otapa_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_session_otasp_otapa_req_type header
  1) Session ID
  2) Request TYPE
  3) Client Data Pointer
  4) Response CallBack
  5) Payload len
  6) request len
  ---------------------------------------------------------------------------*/
  /* Updating Request Header */
  msg_ptr->request_header.session_id        = session_id;
  msg_ptr->request_header.orig_request_type = MMGSDI_SESSION_OTASP_OTAPA_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_SESSION_OTASP_OTAPA_REQ;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /* Updating OTAPA Request data */
  msg_ptr->esn                              = esn;
  msg_ptr->randseed                         = randseed;
  msg_ptr->start_stop                       = start_stop;

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_SESSION_OTASP_OTAPA_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
}/* mmgsdi_session_otasp_otapa_req */


/*================================================================
FUNCTION:      MMGSDI_SESSION_READ_PRL

DESCRIPTION:
  This API is used to get the IS-683-C or IS-683-A PRL from either
  EF-EPRL or EF-PRL when EF-EPRL does not contain a valid PRL

PARAMETERS:
  session_id     : Session Id of the caller

DEPENDENCIES:
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().
  The session ID should only be that of a GW provisioning application

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type:
    MMGSDI_SUCCESS:          The command structure was properly
                             Generated and queued onto the MMGSDI
                             Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API
                             are not within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the
                             MMGSDI Task because the max number of
                             commands are already queued.

SIDE EFFECTS:
  None
================================================================*/
mmgsdi_return_enum_type mmgsdi_session_read_prl (
  mmgsdi_session_id_type               session_id,
  mmgsdi_callback_type                 response_cb_ptr,
  mmgsdi_client_data_type              client_ref
)
{
  mmgsdi_task_cmd_type                  *task_cmd_ptr     = NULL;
  mmgsdi_session_read_prl_req_type      *msg_ptr          = NULL;
  mmgsdi_return_enum_type                mmgsdi_status    = MMGSDI_ERROR;
  uint32                                 task_cmd_len     = 0;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Session ID
    2) Response CB function Check
    3) Memory allocation for msg_ptr and check for allocation
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_session_id_valid_in_cmd(
                    session_id,
                    MMGSDI_SESSION_READ_PRL_REQ);

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
  /*---------------------------------------------------------------------------
    - Align pointers of the various structures members
  ---------------------------------------------------------------------------*/
  mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.session_read_prl_req,
                         MMGSDI_SESSION_READ_PRL_REQ);

  msg_ptr = &task_cmd_ptr->cmd.cmd.session_read_prl_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_session_get_cphs_info_req_type header
      1) Session ID
      2) Request TYPE
      3) Client Data Pointer
      4) Response CallBack
      5) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_SESSION_READ_PRL_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_SESSION_READ_PRL_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_SESSION_READ_PRL_REQ;
  msg_ptr->request_header.session_id        = session_id;
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

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_SESSION_READ_PRL_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
}/* mmgsdi_session_read_prl*/


/*================================================================
FUNCTION:      MMGSDI_SESSION_GET_NVRUIM_DATA_SYNC

DESCRIPTION:
  This API is used to get nvruim state information and part of
  nvruim's global cache. This is a synchronous call.

  This API uses critical sections to protect accesses to
  MMGSDI global data. Hence, it is advisable to pet the dog
  before invoking the API.

PARAMETERS:
  session_id      : Session Id of the caller
  nvruim_data_ptr:  returned data

DEPENDENCIES:
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().
  The session ID should only be that of a 1x provisioning application

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type:
    MMGSDI_SUCCESS:          The command structure was properly
                             Generated and queued onto the MMGSDI
                             Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API
                             are not within appropriate ranges.
    MMGSDI_ERROR:            Request handling failed

SIDE EFFECTS:
  None
================================================================*/
mmgsdi_return_enum_type mmgsdi_session_get_nvruim_data_sync(
  mmgsdi_session_id_type               session_id,
  mmgsdi_nvruim_data_type             *nvruim_data_ptr
)
{
  mmgsdi_return_enum_type             mmgsdi_status    = MMGSDI_ERROR;
  mmgsdi_session_id_status_enum_type  session_status   = MMGSDI_SESSION_ID_UNASSIGNED;
  mmgsdi_session_info_type           *session_info_ptr = NULL;
  nvruim_esn_usage_type               esn_usage        = NV_RUIM_USE_ESN;
  nvruim_an_hrpd_support_status       an_hrpd_status   = NVRUIM_AN_HRPD_NO_SUPPORT;
  mmgsdi_nvruim_esn_usage_enum_type   mmgsdi_esn_usg   = MMGSDI_NVRUIM_USE_ESN;
  nvruim_3gpd_support_status          nvruim_3gpd_stat = NVRUIM_3GPD_MIP_RUIM_SIP_RUIM;

  UIM_MSG_HIGH_0("mmgsdi_session_get_nvruim_data_sync");

  /*---------------------------------------------------------------------------
     Input Parameters Check
     1) nvruim_data_rsp check
     2) Session ID
   ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(nvruim_data_ptr);

  /* Protect access to Client ID Reg.Table/App. table */
  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  do
  {
    session_status = mmgsdi_util_is_session_id_valid(session_id);
    if((session_status !=  MMGSDI_SESSION_ID_ASSIGNED_AND_ACTIVE) &&
       (session_status !=  MMGSDI_SESSION_ID_ASSIGNED_NOT_ACTIVE))
    {
      UIM_MSG_ERR_2("Input session ID is not in use session_id=0x%x status=0x%x ",
                    session_id, session_status);
      mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
      break;
    }
    mmgsdi_status = mmgsdi_util_get_session_app_info(
                      session_id,NULL,NULL,NULL,
                      NULL,&session_info_ptr,NULL);
    if(mmgsdi_status != MMGSDI_SUCCESS || session_info_ptr == NULL)
    {
      UIM_MSG_ERR_1("Invalid session ID = 0x%x", session_id);
      mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
      break;
    }
    switch (session_info_ptr->session_type)
    {
      case MMGSDI_1X_PROV_PRI_SESSION:
      case MMGSDI_1X_PROV_SEC_SESSION:
      case MMGSDI_1X_PROV_TER_SESSION:
        break;
      default:
        UIM_MSG_ERR_1("Session type is invalid. session_type=0x%x",
                      session_info_ptr->session_type);
        mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
        break;
    }
  }while(0);

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  switch(nvruim_data_ptr->req_type)
  {
    case MMGSDI_NVRUIM_GET_ESN_USAGE_REQ:
    case MMGSDI_NVRUIM_GET_3GPD_CTRL_AND_ESN_USG_DATA_REQ:
      esn_usage = nvruim_get_esn_usage();
      switch(esn_usage)
      {
        case NV_RUIM_USE_ESN:
          mmgsdi_esn_usg = MMGSDI_NVRUIM_USE_ESN;
          break;
        case NV_RUIM_USE_RUIM_ID:
          mmgsdi_esn_usg = MMGSDI_NVRUIM_USE_RUIM_ID;
          break;
        case NV_RUIM_ZERO_ESN:
          mmgsdi_esn_usg = MMGSDI_NVRUIM_ZERO_ESN;
          break;
        case NV_RUIM_USE_MEID:
          nvruim_data_ptr->data.esn_usage = MMGSDI_NVRUIM_USE_MEID;
          break;
        case NV_RUIM_USE_SF_EUIMID:
          mmgsdi_esn_usg = MMGSDI_NVRUIM_USE_SF_EUIMID;
          break;
        case NV_RUIM_USE_SF_EUIMID_ESN:
          mmgsdi_esn_usg = MMGSDI_NVRUIM_USE_SF_EUIMID_ESN;
          break;
        case NV_RUIM_USE_MEID_UIMID:
          mmgsdi_esn_usg = MMGSDI_NVRUIM_USE_MEID_UIMID;
          break;
        default:
          UIM_MSG_ERR_1("Invalid ESN usage type reported 0x%x", esn_usage);
          /* setting default */
          mmgsdi_esn_usg = MMGSDI_NVRUIM_USE_ESN;
          break;
      }
      if(nvruim_data_ptr->req_type ==
         MMGSDI_NVRUIM_GET_3GPD_CTRL_AND_ESN_USG_DATA_REQ)
      {
        mmgsdi_nvruim_3gpd_support_status mmgsdi_3gpd_stat = MMGSDI_NVRUIM_3GPD_MIP_RUIM_SIP_RUIM;

        nvruim_data_ptr->data.nvruim_3gpd_ctrl_and_esn_usg.esn_usage_type
          = mmgsdi_esn_usg;
        nvruim_data_ptr->data.nvruim_3gpd_ctrl_and_esn_usg.mmgsdi_nvruim_3gpd_sip_svc
          = nvruim_3gpd_sip_svc;
        nvruim_data_ptr->data.nvruim_3gpd_ctrl_and_esn_usg.mmgsdi_nvruim_3gpd_mip_svc
          = nvruim_3gpd_mip_svc;
        nvruim_3gpd_stat = uim_3gpd_support();

        switch(nvruim_3gpd_stat)
        {
          case NVRUIM_3GPD_MIP_RUIM_SIP_RUIM:
            mmgsdi_3gpd_stat = MMGSDI_NVRUIM_3GPD_MIP_RUIM_SIP_RUIM;
            break;
          case NVRUIM_3GPD_MIP_NV_SIP_NV:
            mmgsdi_3gpd_stat = MMGSDI_NVRUIM_3GPD_MIP_NV_SIP_NV;
            break;
          case NVRUIM_3GPD_MIP_RUIM_SIP_NV:
            mmgsdi_3gpd_stat = MMGSDI_NVRUIM_3GPD_MIP_RUIM_SIP_NV;
            break;
          case NVRUIM_3GPD_MIP_NV_SIP_RUIM:
            mmgsdi_3gpd_stat = MMGSDI_NVRUIM_3GPD_MIP_NV_SIP_RUIM;
            break;
        }
        nvruim_data_ptr->data.nvruim_3gpd_ctrl_and_esn_usg.mmgsdi_nvruim_3gpd_control_data =
          mmgsdi_3gpd_stat;
      }
      else if(nvruim_data_ptr->req_type == MMGSDI_NVRUIM_GET_ESN_USAGE_REQ)
      {
        nvruim_data_ptr->data.esn_usage = mmgsdi_esn_usg;
      }
      break;
    case MMGSDI_NVRUIM_GET_HRPD_CONTROL_REQ:
      an_hrpd_status = uim_an_hrpd_support();
      switch(an_hrpd_status)
      {
        case NVRUIM_AN_HRPD_SUPPORT:
          nvruim_data_ptr->data.an_hrpd_status =
            MMGSDI_NVRUIM_AN_HRPD_SUPPORT;
          break;
        case NVRUIM_AN_HRPD_NO_SUPPORT:
          nvruim_data_ptr->data.an_hrpd_status =
            MMGSDI_NVRUIM_AN_HRPD_NO_SUPPORT;
          break;
        case NVRUIM_AN_HRPD_USE_CAVE:
          nvruim_data_ptr->data.an_hrpd_status =
            MMGSDI_NVRUIM_AN_HRPD_USE_CAVE;
          break;
      default:
        UIM_MSG_ERR_1("Invalid an_hrpd_status value 0x%x",
                      nvruim_data_ptr->req_type);
        /* set to default value */
        nvruim_data_ptr->data.an_hrpd_status =
          MMGSDI_NVRUIM_AN_HRPD_NO_SUPPORT;
      }
      break;
    case MMGSDI_NVRUIM_GET_3GPD_EXT_SUPPORT_REQ:
      nvruim_data_ptr->data.nvruim_3gpd_ext_support =
        nvruim_data_3gpd_ext_support();
      break;
    default:
      UIM_MSG_ERR_1("Incorrect data_req type 0x%x", an_hrpd_status);
      return MMGSDI_INCORRECT_PARAMS;
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_session_get_nvruim_data_sync */


/*===========================================================================
FUNCTION        MMGSDI_SESSION_COMPUTE_IP_AUTH

DESCRIPTION
  This function will build a request to the MMGSDI Task to compute IP
  authentication for 3GPD SIMPLE IP CHAP, Mobile IP, or HRPD CHAP
  Authentication.

PARAMETERS
  session_id:        Session ID of the caller
  compute_ip_data:   Data required for Compute IP Command
  response_cb_ptr:   Response callback.
  client_ref:        User data returned upon completion of this cmd.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

LIMITATIONS

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

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_session_compute_ip_auth (
  mmgsdi_session_id_type             session_id,
  mmgsdi_compute_ip_data_type        compute_ip_data,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
  mmgsdi_task_cmd_type              *task_cmd_ptr      = NULL;
  mmgsdi_compute_ip_req_type        *msg_ptr           = NULL;
  mmgsdi_return_enum_type            mmgsdi_status     = MMGSDI_ERROR;
  int32                              total_mem_len     = 0;
  int32                              var_len           = 0;
  int32                              temp_mem          = 0;
  boolean                            operation_type_ok = FALSE;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Session ID
    2) Response CB function Check
    3) Memory allocation for msg_ptr and check for allocation
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  mmgsdi_status = mmgsdi_util_is_session_id_valid_in_cmd(
                    session_id,
                    MMGSDI_COMPUTE_IP_AUTH_REQ);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if((compute_ip_data.operation_type == MMGSDI_CMPT_IP_SIP_CHAP) ||
     (compute_ip_data.operation_type == MMGSDI_CMPT_IP_MIP_MN_HA)||
     (compute_ip_data.operation_type == MMGSDI_CMPT_IP_MIP_RRQ)  ||
     (compute_ip_data.operation_type == MMGSDI_CMPT_IP_MIP_MN_AAA))
  {
    operation_type_ok = TRUE;
  }

  if(compute_ip_data.operation_type == MMGSDI_CMPT_IP_HRPD_CHAP)
  {
    operation_type_ok = TRUE;
  }

  if(!operation_type_ok)
  {
    UIM_MSG_ERR_0("Invalid operation type");
    return MMGSDI_ERROR;
  }
  switch(compute_ip_data.operation_type)
  {
    case MMGSDI_CMPT_IP_SIP_CHAP:
      if((compute_ip_data.data.sip_chap_data.chap_challenge_length >
          MMGSDI_MAX_3GPD_CHAP_CHALLENGE_LENGTH) ||
         (compute_ip_data.data.sip_chap_data.chap_challenge_length <= 0))
      {
        UIM_MSG_ERR_0("Invalid Length for SIP CHAP");
        return MMGSDI_ERROR;
      }
      var_len = 0;
      break;
    case MMGSDI_CMPT_IP_MIP_MN_HA:
      if((compute_ip_data.data.mn_ha_data.registration_data_length >
          MMGSDI_MAX_3GPD_MN_HA_REG_DATA_LENGTH)||
         (compute_ip_data.data.mn_ha_data.registration_data_length <= 0) ||
         (compute_ip_data.data.mn_ha_data.registration_data_ptr == NULL))
      {
        UIM_MSG_ERR_0("Invalid data length or pointer for MIP MN HA");
        return MMGSDI_ERROR;
      }
      var_len = compute_ip_data.data.mn_ha_data.registration_data_length;
      break;
    case MMGSDI_CMPT_IP_MIP_RRQ:
      if((compute_ip_data.data.rrq_data.rrq_data_length >
          MMGSDI_MAX_3GPD_HASH_RRQ_DATA_LENGTH) ||
         (compute_ip_data.data.rrq_data.rrq_data_length <= 0) ||
         (compute_ip_data.data.rrq_data.rrq_data_ptr == NULL))
      {
        UIM_MSG_ERR_0("Invalid data length or pointer for MIP RRQ HASH");
        return MMGSDI_ERROR;
      }
      var_len = compute_ip_data.data.rrq_data.rrq_data_length;
      break;
    case MMGSDI_CMPT_IP_MIP_MN_AAA:
      if((compute_ip_data.data.mn_aaa_data.challenge_length >
          MMGSDI_3GPD_MAX_MIP_CHALLENGE_LENGTH) ||
         (compute_ip_data.data.mn_aaa_data.challenge_length <= 0) ||
         (compute_ip_data.data.mn_aaa_data.challenge_ptr == NULL))
      {
        UIM_MSG_ERR_0("Invalid data length or pointer for MIP MN AAA");
        return MMGSDI_ERROR;
      }
      var_len = compute_ip_data.data.mn_aaa_data.challenge_length;
      break;
    case MMGSDI_CMPT_IP_HRPD_CHAP:
      if((compute_ip_data.data.hrpd_chap_data.chap_challenge_length >
          MMGSDI_MAX_HRPD_CHAP_CHALLENGE_LENGTH) ||
         (compute_ip_data.data.hrpd_chap_data.chap_challenge_length <= 0) ||
         (compute_ip_data.data.hrpd_chap_data.chap_challenge_ptr == NULL))
      {
        UIM_MSG_ERR_0("Invalid data length or pointer for HRPD AN CHAP");
        return MMGSDI_ERROR;
      }
      var_len = compute_ip_data.data.hrpd_chap_data.chap_challenge_length;
      break;
    default:
      break;
  }

  total_mem_len = sizeof(mmgsdi_task_cmd_type);
  mmgsdi_util_cal_align_size(total_mem_len, &temp_mem);
  MMGSDIUTIL_RETURN_IF_ALIGNED_VAR_OUT_OF_BOUNDS(total_mem_len, temp_mem);

  total_mem_len = temp_mem + var_len;

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, int32touint32(total_mem_len));
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /* Copy the compute IP authentication type to the command buffer */
  task_cmd_ptr->cmd.cmd.compute_ip_req.compute_ip_data.operation_type =
    compute_ip_data.operation_type;

  /*---------------------------------------------------------------------------
    - Align pointers of the various structures members
  ---------------------------------------------------------------------------*/
  mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.compute_ip_req,
                         MMGSDI_COMPUTE_IP_AUTH_REQ);

  msg_ptr = &task_cmd_ptr->cmd.cmd.compute_ip_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_compute_ip_req_type header
      1) Session ID
      2) Request TYPE
      3) Client Data Pointer
      4) Response CallBack
      5) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_COMPUTE_IP_AUTH_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_COMPUTE_IP_AUTH_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_COMPUTE_IP_AUTH_REQ;
  msg_ptr->request_header.session_id        = session_id;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

   /*---------------------------------------------------------------------------
    Populating Compute IP DATA
  ---------------------------------------------------------------------------*/
  switch(compute_ip_data.operation_type)
  {
    case MMGSDI_CMPT_IP_SIP_CHAP:
      msg_ptr->compute_ip_data.data.sip_chap_data.nai_entry_index =
        compute_ip_data.data.sip_chap_data.nai_entry_index;
      msg_ptr->compute_ip_data.data.sip_chap_data.chap_id =
        compute_ip_data.data.sip_chap_data.chap_id;
      msg_ptr->compute_ip_data.data.sip_chap_data.chap_challenge_length =
        compute_ip_data.data.sip_chap_data.chap_challenge_length;
      mmgsdi_memscpy(
        msg_ptr->compute_ip_data.data.sip_chap_data.chap_challenge,
        int32touint32(msg_ptr->compute_ip_data.data.sip_chap_data.chap_challenge_length),
        compute_ip_data.data.sip_chap_data.chap_challenge,
        int32touint32(compute_ip_data.data.sip_chap_data.chap_challenge_length));
      break;
    case MMGSDI_CMPT_IP_MIP_MN_HA:
      msg_ptr->compute_ip_data.data.mn_ha_data.nai_entry_index =
        compute_ip_data.data.mn_ha_data.nai_entry_index;
      msg_ptr->compute_ip_data.data.mn_ha_data.registration_data_length =
        compute_ip_data.data.mn_ha_data.registration_data_length;
      mmgsdi_memscpy(
        msg_ptr->compute_ip_data.data.mn_ha_data.registration_data_ptr,
        int32touint32(msg_ptr->compute_ip_data.data.mn_ha_data.registration_data_length),
        compute_ip_data.data.mn_ha_data.registration_data_ptr,
        int32touint32(compute_ip_data.data.mn_ha_data.registration_data_length));
      break;
    case MMGSDI_CMPT_IP_MIP_RRQ:
      msg_ptr->compute_ip_data.data.rrq_data.rrq_data_length =
        compute_ip_data.data.rrq_data.rrq_data_length;
      mmgsdi_memscpy(
        msg_ptr->compute_ip_data.data.rrq_data.rrq_data_ptr,
        int32touint32(msg_ptr->compute_ip_data.data.rrq_data.rrq_data_length),
        compute_ip_data.data.rrq_data.rrq_data_ptr,
        int32touint32(compute_ip_data.data.rrq_data.rrq_data_length));
      break;
    case MMGSDI_CMPT_IP_MIP_MN_AAA:
      msg_ptr->compute_ip_data.data.mn_aaa_data.nai_entry_index =
        compute_ip_data.data.mn_aaa_data.nai_entry_index;
      msg_ptr->compute_ip_data.data.mn_aaa_data.challenge_length =
        compute_ip_data.data.mn_aaa_data.challenge_length;
      mmgsdi_memscpy(
        msg_ptr->compute_ip_data.data.mn_aaa_data.challenge_ptr,
        int32touint32(msg_ptr->compute_ip_data.data.mn_aaa_data.challenge_length),
        compute_ip_data.data.mn_aaa_data.challenge_ptr,
        int32touint32(compute_ip_data.data.mn_aaa_data.challenge_length));
      break;
    case MMGSDI_CMPT_IP_HRPD_CHAP:
      msg_ptr->compute_ip_data.data.hrpd_chap_data.chap_id =
        compute_ip_data.data.hrpd_chap_data.chap_id;
      msg_ptr->compute_ip_data.data.hrpd_chap_data.chap_challenge_length =
        compute_ip_data.data.hrpd_chap_data.chap_challenge_length;
      mmgsdi_memscpy(
        msg_ptr->compute_ip_data.data.hrpd_chap_data.chap_challenge_ptr,
        int32touint32(msg_ptr->compute_ip_data.data.hrpd_chap_data.chap_challenge_length),
        compute_ip_data.data.hrpd_chap_data.chap_challenge_ptr,
        int32touint32(compute_ip_data.data.hrpd_chap_data.chap_challenge_length));
      break;
    default:
      UIM_MSG_ERR_1("Invalid computer IP auth operation type 0x%x",
                    compute_ip_data.operation_type);
      break;

  }

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_COMPUTE_IP_AUTH_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
}/* mmgsdi_session_compute_ip_auth */

