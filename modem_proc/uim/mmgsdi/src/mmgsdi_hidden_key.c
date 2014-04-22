/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                    M M G S D I   H I D D E N   K E Y   F U N C T I O N S


GENERAL DESCRIPTION

This source file contains hidden key related support and processing for MMGSDI.

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

$Header: //components/rel/uim.mpss/3.3.1/mmgsdi/src/mmgsdi_hidden_key.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
10/24/18   rps     Refragment alloc extra param code
12/05/16   bcho    Remove compilation error
05/25/16   vdc     Remove F3 messages for memory allocation failure
05/12/16   vdc     Initial version
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "uim_msg.h"
#include "mmgsdi.h"
#include "mmgsdi_evt.h"
#include "mmgsdiutil.h"
#include "mmgsdicache.h"
#include "mmgsdi_cnf.h"
#include "mmgsdi_uicc_rsp.h"
#include "mmgsdi_card_init.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*===========================================================================
   FUNCTION MMGSDI_HIDDEN_KEY_CONVERT_HIDDENKEY

   DESCRIPTION:
     This function converts the ASCII Hiddenkey input to the card format

   PARAMS:
    hkey_ptr: Contains the Hidden Key length and data
    hkey_card_ptr: Contains the Hidden key in card format

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE
     mmgsdi_return_enum_type

   SIDE EFFECTS:
     None
 ===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_hidden_key_convert_hiddenkey(
  const mmgsdi_static_data_type * hkey_ptr,
  mmgsdi_static_data_type       * hkey_card_ptr)
{
  mmgsdi_len_type         len = 0;

  MMGSDIUTIL_RETURN_IF_NULL_2(hkey_ptr, hkey_card_ptr);

  /* Since we have checks in the PIN APIs for valid length
     and data range, we do not have any checks here */

  /* Hiddenkey file size is fixed */
  hkey_card_ptr->data_len = MMGSDI_HIDDENKEY_LEN;

  memset((void*)hkey_card_ptr->data_ptr, 0xFF,
         hkey_card_ptr->data_len);

  /* Convert the Hiddenkey data in ASCII to the card format */
  for(len = 0; len + 1 < hkey_ptr->data_len; (len = len + 2))
  {
    hkey_card_ptr->data_ptr[len/2] = ((hkey_ptr->data_ptr[len] - '0') << 0x04) |
                                      (hkey_ptr->data_ptr[len + 1] - '0');
  }

  if(hkey_ptr->data_len % 2)
  {
    hkey_card_ptr->data_ptr[len/2] = ((hkey_ptr->data_ptr[len] - '0') << 0x04) |
                                      0xF;
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_hidden_key_convert_hiddenkey */


/*===========================================================================
   FUNCTION MMGSDI_HIDDEN_KEY_VERIFY_HIDDENKEY

   DESCRIPTION:
     This function verifies the ASCII Hiddenkey input against
     the Hiddenkey data from the card

   PARAMS:
    session_id: Input session ID
    hkey_ptr: Hidden key length and data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE
     mmgsdi_return_enum_type

   SIDE EFFECTS:
     None
 ===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_hidden_key_verify_hiddenkey(
  mmgsdi_session_id_type          session_id,
  const mmgsdi_static_data_type * hkey_ptr)
{
  mmgsdi_data_type              hkey_cache     = {0, NULL};
  mmgsdi_len_type               hkey_data_len  = 0;
  mmgsdi_return_enum_type       mmgsdi_status  = MMGSDI_SUCCESS;
  mmgsdi_session_type_enum_type session_type   = MMGSDI_MAX_SESSION_TYPE_ENUM;
  mmgsdi_access_type            file_access;
  mmgsdi_static_data_type       hkey_card;

  MMGSDIUTIL_RETURN_IF_NULL(hkey_ptr);

  /* Convert input Hiddenkey data in ASCII to card format */
  mmgsdi_status = mmgsdi_hidden_key_convert_hiddenkey(hkey_ptr, &hkey_card);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  memset((void*)&file_access, 0x00, sizeof(mmgsdi_access_type));

  /* Read EFHiddenkey from cache */
  file_access.access_method  = MMGSDI_EF_ENUM_ACCESS;

  mmgsdi_status = mmgsdi_util_get_session_type(session_id, &session_type, NULL);

  if(mmgsdi_status != MMGSDI_SUCCESS ||
     mmgsdi_util_is_prov_session(session_type) == FALSE)
  {
    UIM_MSG_ERR_1("Invalid session id 0x%x", session_id);
    return MMGSDI_ERROR;
  }

  if((session_type == MMGSDI_GW_PROV_PRI_SESSION) ||
     (session_type == MMGSDI_GW_PROV_SEC_SESSION) ||
     (session_type == MMGSDI_GW_PROV_TER_SESSION))
  {
    file_access.file.file_enum = MMGSDI_USIM_HIDDENKEY;
  }
  else
  {
    file_access.file.file_enum = MMGSDI_CSIM_HIDDEN_KEY;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(hkey_cache.data_ptr,
                                     MMGSDI_HIDDENKEY_LEN);
  if(hkey_cache.data_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  mmgsdi_status = mmgsdi_cache_read_len(session_id,
                                        &file_access,
                                        &hkey_data_len,
                                        0,
                                        NULL);
  if ((mmgsdi_status != MMGSDI_SUCCESS) ||
      (hkey_data_len != MMGSDI_HIDDENKEY_LEN) ||
      (mmgsdi_cache_read(session_id,
                        &file_access,
                        &hkey_cache,
                        0,
                        NULL) != MMGSDI_SUCCESS))
  {
    UIM_MSG_ERR_0("Could not read Hidden Key from cache");
    MMGSDIUTIL_TMC_MEM_FREE(hkey_cache.data_ptr);
    return MMGSDI_ERROR;
  }

  /* Compare input Hiddenkey with the card data */
  if(memcmp((const void*)hkey_card.data_ptr,
            (const void*)hkey_cache.data_ptr,
            MMGSDI_HIDDENKEY_LEN))
  {
    UIM_MSG_ERR_0("The Hiddenkey value provided by client for verification does not match the card data");
    mmgsdi_status = MMGSDI_ERROR;
  }

  MMGSDIUTIL_TMC_MEM_FREE(hkey_cache.data_ptr);

  return mmgsdi_status;
} /* mmgsdi_hidden_key_verify_hiddenkey */


/*===========================================================================
  FUNCTION:      MMGSDI_HIDDEN_KEY_OPERATION

  DESCRIPTION:
    This function will handle the Hidden Key requests

  PARAMS:
    req_ptr: PIN operation request pointer

  DEPENDENCIES:
    NONE

  LIMITATIONS:
    NONE

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_hidden_key_operation (
  const mmgsdi_pin_operation_req_type * req_ptr
)
{
  mmgsdi_return_enum_type              mmgsdi_status    = MMGSDI_SUCCESS;
  uint8                                client_cmd_index = 0;
  mmgsdi_int_app_info_type            *app_info_ptr     = NULL;

 /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr                                  - Cannot be NULL
    2. req_ptr->request_header.response_cb      - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL_2(req_ptr, req_ptr->request_header.response_cb);

  /* Get the app related to the session */
  mmgsdi_status = mmgsdi_util_get_prov_session_info(
                    req_ptr->request_header.session_id,
                    NULL,
                    NULL,
                    &app_info_ptr);

  if((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
  {
    return MMGSDI_ERROR;
  }

  UIM_MSG_MED_2("Hidden key status 0x%x and Hidden key operation 0x%x",
                app_info_ptr->hiddenkey.status, req_ptr->pin_op);

  if(app_info_ptr->hiddenkey.status == MMGSDI_PIN_STATUS_NOT_INITIALIZED)
  {
    return MMGSDI_NOT_INIT;
  }
  else if(app_info_ptr->hiddenkey.status == MMGSDI_PIN_NOT_FOUND)
  {
    return MMGSDI_NOT_FOUND;
  }

  /* Check if the current Hiddenkey state allows the requested operation */
  switch(req_ptr->pin_op)
  {
    case MMGSDI_PIN_OP_VERIFY:
    case MMGSDI_PIN_OP_DISABLE:
    case MMGSDI_PIN_OP_CHANGE:
      if((app_info_ptr->hiddenkey.status != MMGSDI_PIN_ENABLED_NOT_VERIFIED) &&
         (app_info_ptr->hiddenkey.status != MMGSDI_PIN_ENABLED_VERIFIED))
      {
        return MMGSDI_ACCESS_DENIED;
      }
      break;
    case MMGSDI_PIN_OP_ENABLE:
      if(app_info_ptr->hiddenkey.status != MMGSDI_PIN_DISABLED)
      {
        return MMGSDI_ACCESS_DENIED;
      }
      break;
    default:
      return MMGSDI_INCORRECT_PARAMS;
  }

  /* Verify the Hiddenkey data provided by the client against the card */
  if(req_ptr->pin_op != MMGSDI_PIN_OP_ENABLE)
  {
    /* The current Hiddenkey data passed in by the client is verified against
       the value in the cache */
    mmgsdi_status = mmgsdi_hidden_key_verify_hiddenkey(
                      req_ptr->request_header.session_id,
                      &req_ptr->pin_puk);

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      return MMGSDI_INCORRECT_CODE;
    }
  }

  mmgsdi_status = mmgsdi_util_fill_and_alloc_request_table(MMGSDI_PIN_OPERATION_REQ,
                                                           (void*)req_ptr,
                                                           NULL,
                                                           &client_cmd_index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_queue_mmgsdi_uim_report_rsp(client_cmd_index,
                                                          MMGSDI_PIN_OPERATION_REQ,
                                                          MMGSDI_SUCCESS);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(client_cmd_index);
  }

  return mmgsdi_status;
} /* mmgsdi_hidden_key_operation */


/* ==========================================================================
  FUNCTION MMGSDI_HIDDEN_KEY_WRITE_EF_CB

  DESCRIPTION
    This function will handle Hiddenkey write CNF geneted by MMGSDI

  PARAMS
    status: Callback status after writing data in EF-HIDDENKEY
    cnf: Confirmation tye
    cnf_ptr: Confirmation pointer

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
=============================================================================*/
static void mmgsdi_hidden_key_write_ef_cb(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type   *cnf_ptr)
{
  uint32                             index           = 0;
  mmgsdi_request_header_type         req_header      = {0};
  mmgsdi_client_req_extra_info_type *extra_param_ptr = NULL;
  mmgsdi_sw_status_type              status_word     = {0};
  mmgsdi_int_app_info_type          *app_info_ptr    = NULL;
  mmgsdi_return_enum_type            mmgsdi_status   = MMGSDI_SUCCESS;

  if (cnf_ptr == NULL)
  {
    return;
  }

  UIM_MSG_MED_2("mmgsdi_hidden_key_write_ef_cb: cnf: 0x%x, status: 0x%x", cnf, status);

  /* Obtain the index */
  index = cnf_ptr->response_header.client_data;

  /* Retrieve the original Hiddenkey request */
  mmgsdi_status = mmgsdi_util_get_client_request_table_info(
                    index,
                    &req_header,
                    &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("Hiddenkey EF Write CB: Could not extract the client info - 0x%x",
                  mmgsdi_status);
    return;
  }
  /* If a confirmation has already been sent for this command as part of
     CARD_ERROR handling, free only the client_req_table index. */
  if((index < MMGSDI_MAX_CLIENT_REQ_INFO) &&
     (mmgsdi_client_req_table_info_ptr[index]->uim_rsp_status == MMGSDI_UIM_RSP_NOT_RECEIVED))
  {
    UIM_MSG_HIGH_2("Free client_req_table index 0x%x for cmd 0x%x",
                   index,
                   mmgsdi_client_req_table_info_ptr[index]->request_type);

    mmgsdi_util_free_client_request_table_index(index);
    return;
  }

  switch(cnf)
  {
    case MMGSDI_WRITE_REQ:
      if(status != MMGSDI_SUCCESS)
      {
        status = MMGSDI_ERROR;
      }
      else
      {
        /* Get the app related to the session */
        mmgsdi_status = mmgsdi_util_get_prov_session_info(req_header.session_id,
                                                          NULL,
                                                          NULL,
                                                          &app_info_ptr);

        if((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
        {
          UIM_MSG_ERR_0("Hiddenkey EF Write CB: Could not retrieve the prov app info");
          status = MMGSDI_ERROR;
        }
        else
        {
          if (extra_param_ptr != NULL)
          {
             /* If the request is for Hiddenkey disable */
            if(extra_param_ptr->pin_data.orig_pin_op == MMGSDI_PIN_OP_DISABLE)
            {
              app_info_ptr->hiddenkey.status = MMGSDI_PIN_DISABLED;
            }
            /* The request is for enable/change Hiddenkey */
            else if((extra_param_ptr->pin_data.orig_pin_op == MMGSDI_PIN_OP_ENABLE) ||
                    (extra_param_ptr->pin_data.orig_pin_op == MMGSDI_PIN_OP_CHANGE))
            {
              app_info_ptr->hiddenkey.status = MMGSDI_PIN_ENABLED_VERIFIED;
            }
            mmgsdi_memscpy((void*)&extra_param_ptr->pin_data.pin_info,
                           sizeof(mmgsdi_app_pin_info_type),
                           (const void*)&app_info_ptr->hiddenkey,
                           sizeof(mmgsdi_app_pin_info_type));
          }
        }
      }

      /* Queue the confirmation */
      mmgsdi_status = mmgsdi_cnf_build_and_queue(status,
                                                 &req_header,
                                                 extra_param_ptr,
                                                 NULL,
                                                 FALSE,
                                                 status_word);
      break;
    default:
      return;
  }

  mmgsdi_util_free_client_request_table_index(index);
} /* mmgsdi_hidden_key_write_ef_cb */


/* ============================================================================
   FUNCTION:      MMGSDI_HIDDEN_KEY_RSP_OPERATION

   DESCRIPTION:
     This function will build the response to the Hiddenkey Command

   PARAMS
     uim_rsp_ptr: Response pointer for hidden key operation

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

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_hidden_key_rsp_operation (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
)
{
  mmgsdi_request_header_type          req_header          ={0};
  uint32                              index               = 0;
  mmgsdi_return_enum_type             mmgsdi_status       = MMGSDI_SUCCESS;
  mmgsdi_client_req_extra_info_type * extra_param_ptr     = NULL;
  mmgsdi_return_enum_type             report_status       = MMGSDI_SUCCESS;
  mmgsdi_int_app_info_type          * app_info_ptr        = NULL;
  uint8                               client_cmd_index    = 0;
  mmgsdi_client_req_extra_info_type * new_extra_param_ptr = NULL;
  mmgsdi_session_type_enum_type       session_type        = MMGSDI_MAX_SESSION_TYPE_ENUM;
  mmgsdi_static_data_type             hkey_card;
  mmgsdi_write_data_type              write_data;
  mmgsdi_access_type                  file_access;

  /* --------------------------------------------------------------------------
     Validate the Input pointers.  Must not be NULL
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);

  /*---------------------------------------------------------------------------
    Obtain index
    Index should be [0, MMGSDI_MAX_CLIENT_REQ_INFO-1],
    Index is set when
    a) a success queuing of command to UIM
    b) a failure in queuing command to UIM but client_request pointer should
      be set in a valid index location in mmgsdi process command
  ---------------------------------------------------------------------------*/
  index = uim_rsp_ptr->rsp_data_index;

  mmgsdi_status = mmgsdi_util_get_client_request_table_info(
                                index,
                                &req_header,
                                &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS || extra_param_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  /* Get the app related to the session */
  mmgsdi_status = mmgsdi_util_get_prov_session_info(
                    req_header.session_id,
                    NULL,
                    NULL,
                    &app_info_ptr);

  if((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
  {
    return MMGSDI_ERROR;
  }

  /* Obtain the Stats of the operation */
  report_status = mmgsdi_uicc_rsp_parse_uim_response(uim_rsp_ptr);

  if (report_status == MMGSDI_SUCCESS)
  {
    /* If the requested is Hiddenkey operation is a verify */
    if(extra_param_ptr->pin_data.orig_pin_op == MMGSDI_PIN_OP_VERIFY)
    {
      app_info_ptr->hiddenkey.status = MMGSDI_PIN_ENABLED_VERIFIED;
    }
    else
    {
      /* If the requested Hiddenkey operation is disable, create
         the Hiddenkey data to be written to the card */
      if(extra_param_ptr->pin_data.orig_pin_op == MMGSDI_PIN_OP_DISABLE)
      {
        hkey_card.data_len = MMGSDI_HIDDENKEY_LEN;
        memset((void*)hkey_card.data_ptr, 0xFF, hkey_card.data_len);
      }
      /* If the requested Hiddenkey operation is an Enable or Change, convert
         the Hiddenekey data from the client to the Card format */
      else if((extra_param_ptr->pin_data.orig_pin_op == MMGSDI_PIN_OP_ENABLE) ||
              (extra_param_ptr->pin_data.orig_pin_op == MMGSDI_PIN_OP_CHANGE))
      {
        mmgsdi_status = mmgsdi_hidden_key_convert_hiddenkey(&extra_param_ptr->pin_data.pin_data,
                                                            &hkey_card);

        if(mmgsdi_status != MMGSDI_SUCCESS)
        {
          UIM_MSG_ERR_0("Could not convert input Hiddenkey data to the card format");
          return MMGSDI_ERROR;
        }
      }
      else
      {
        UIM_MSG_ERR_1("Invalid Hiddenkey operation: 0x%x",
                      extra_param_ptr->pin_data.orig_pin_op);
        return MMGSDI_ERROR;
      }

      mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&client_cmd_index);
      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        UIM_MSG_ERR_1("Unable to find valid index, 0x%x", mmgsdi_status);
        return mmgsdi_status;
      }

      /*-------------------------------------------------------------------------
      allocate and build extra param ptr
      -------------------------------------------------------------------------*/
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(new_extra_param_ptr,
        sizeof(mmgsdi_pin_extra_info_type));

      if (new_extra_param_ptr == NULL)
      {
        mmgsdi_util_free_client_request_table_index(client_cmd_index);
        return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
      }

      mmgsdi_memscpy((void*)&new_extra_param_ptr->pin_data,
                     sizeof(new_extra_param_ptr->pin_data),
                     (const void*)&extra_param_ptr->pin_data,
                     sizeof(mmgsdi_pin_extra_info_type));

      mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                        client_cmd_index,
                        &req_header,
                        new_extra_param_ptr);

      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        UIM_MSG_ERR_1("Unable to populate client request 0x%x", mmgsdi_status);
        mmgsdi_util_free_client_request_table_index(client_cmd_index);
        MMGSDIUTIL_TMC_MEM_FREE(new_extra_param_ptr);
        return mmgsdi_status;
      }

      /* File to be written to */
      memset((void*)&file_access, 0x00, sizeof(mmgsdi_access_type));
      file_access.access_method = MMGSDI_EF_ENUM_ACCESS;

      mmgsdi_status = mmgsdi_util_get_session_type(req_header.session_id,
                                                   &session_type,
                                                   NULL);

      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        UIM_MSG_ERR_0("Could not get Session type");
        mmgsdi_util_free_client_request_table_index(client_cmd_index);
        return MMGSDI_ERROR;
      }

      if((session_type == MMGSDI_GW_PROV_PRI_SESSION) ||
         (session_type == MMGSDI_GW_PROV_SEC_SESSION) ||
         (session_type == MMGSDI_GW_PROV_TER_SESSION))
      {
        file_access.file.file_enum = MMGSDI_USIM_HIDDENKEY;
      }
      else if((session_type == MMGSDI_1X_PROV_PRI_SESSION) ||
              (session_type == MMGSDI_1X_PROV_SEC_SESSION) ||
              (session_type == MMGSDI_1X_PROV_TER_SESSION))
      {
        file_access.file.file_enum = MMGSDI_CSIM_HIDDEN_KEY;
      }
      else
      {
        UIM_MSG_ERR_0("Invalid Session type");
        mmgsdi_util_free_client_request_table_index(client_cmd_index);
        return MMGSDI_ERROR;
      }

      /* Data to be written to the card */
      write_data.data_len = hkey_card.data_len;
      write_data.data_ptr = hkey_card.data_ptr;

      /* Issue a WRITE request to EFHiddenkey */
      mmgsdi_status = mmgsdi_session_write_transparent(
                        req_header.session_id,
                        file_access,
                        0,
                        write_data,
                        mmgsdi_hidden_key_write_ef_cb,
                        (mmgsdi_client_data_type)client_cmd_index);

      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        UIM_MSG_ERR_1("Could not post Hiddenkey Write request, status 0x%x",
                      mmgsdi_status);
        mmgsdi_util_free_client_request_table_index(client_cmd_index);
      }
      return mmgsdi_status;
    }
  }

  mmgsdi_memscpy((void*)&extra_param_ptr->pin_data.pin_info,
                 sizeof(extra_param_ptr->pin_data.pin_info),
                 (const void*)&app_info_ptr->hiddenkey,
                 sizeof(mmgsdi_app_pin_info_type));

  /* Queue the confirmation */
  mmgsdi_status = mmgsdi_cnf_build_and_queue(report_status,
                                             &req_header,
                                             extra_param_ptr,
                                             (void*)&uim_rsp_ptr->rsp_data,
                                             FALSE, /* sw not required for pin op */
                                             uim_rsp_ptr->status_word);

  return mmgsdi_status;
} /* mmgsdi_hidden_key_rsp_operation */


/*===========================================================================
   FUNCTION MMGSDI_HIDDEN_KEY_IS_VALID

   DESCRIPTION:
     This function checks if the Hidden Key data is valid
     (This is the ASCII format input Hiddenkey provided by clients)

   PARAMS:
    hkey_data_ptr: Contains the Hidden Key length and data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE
      boolean:  TRUE  Hidden Key data is valid
                FALSE Hidden Key data is not valid

   SIDE EFFECTS:
     None
 ===========================================================================*/
boolean mmgsdi_hidden_key_is_valid(
  const mmgsdi_data_type * hkey_data_ptr
)
{
  int32 hkey_index = 0;

  if ((NULL == hkey_data_ptr) || (NULL == hkey_data_ptr->data_ptr))
  {
     return FALSE;
  }

  /* Verify that the Hidden Key data length is valid */
  if(hkey_data_ptr->data_len < MMGSDI_PIN_MIN_LEN ||
     hkey_data_ptr->data_len > MMGSDI_PIN_MAX_LEN)
  {
    UIM_MSG_ERR_0("Hiddenkey data length is invalid");
    return FALSE;
  }

  /* Verify that the Hidden Key data is valid */
  for (hkey_index = 0; hkey_index < hkey_data_ptr->data_len; hkey_index++)
  {
    if ((hkey_data_ptr->data_ptr[hkey_index] < '0') ||
        (hkey_data_ptr->data_ptr[hkey_index] > '9'))
    {
       UIM_MSG_ERR_0("Hiddenkey data is invalid");
       return FALSE;
    }
  }

  return TRUE;
} /* mmgsdi_hidden_key_is_valid */


/*===========================================================================
   FUNCTION MMGSDI_HIDDEN_KEY_IS_VALID_FROM_CARD

   DESCRIPTION:
     This function checks if the Hidden Key data retrieved from
     the card is valid

   PARAMS:
    hkey_data_ptr: Contains the Hidden Key length and data
                        retrieved from the card

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE
      boolean:  TRUE  Hidden Key data is valid
                FALSE Hidden Key data is not valid

   SIDE EFFECTS:
     None
 ===========================================================================*/
static boolean mmgsdi_hidden_key_is_valid_from_card(
  const mmgsdi_data_type * hkey_data_ptr
)
{
  int32   hkey_index        = 0;
  uint8   hkey_val          = 0;
  boolean hkey_unused_digit = FALSE;
  boolean is_valid          = TRUE;

  if ((NULL == hkey_data_ptr) || (NULL == hkey_data_ptr->data_ptr))
  {
    return FALSE;
  }

  do
  {
    /* Verify that the Hidden Key data length is valid */
    if(hkey_data_ptr->data_len != MMGSDI_HIDDENKEY_LEN)
    {
      is_valid = FALSE;
      break;
    }

    /* Verify that the Hidden Key data is valid */
    for (hkey_index = 0; hkey_index < MMGSDI_HIDDENKEY_LEN; hkey_index++)
    {
      hkey_val = (hkey_data_ptr->data_ptr[hkey_index] & 0xF0) >> 4;
  
      /* The minimum number of digits for Hiddenkey is 4, and
         these shall all be valid BCD digits */
      if(hkey_index < MMGSDI_HIDDENKEY_MIN_VALID_LEN)
      {
        if(hkey_val > 0x9)
        {
          is_valid = FALSE;
          break;
        }
      }
      /* Beyond the minimum number (4 digits), once an 'F' is encountered (which
         indicates that it is an unused digit), the remaining digits
         shall also be 'F'
         Example: Hiddenkey value of 1234F8FF needs to be marked as invalid.*/
      else
      {
        if(hkey_val == 0xF)
        {
          hkey_unused_digit = TRUE;
        }
        else if((hkey_val > 0x9) || (hkey_unused_digit))
        {
          is_valid = FALSE;
          break;
        }
      }

      hkey_val = hkey_data_ptr->data_ptr[hkey_index] & 0x0F;

      if(hkey_index < MMGSDI_HIDDENKEY_MIN_VALID_LEN)
      {
        if(hkey_val > 0x9)
        {
          is_valid = FALSE;
          break;
        }
      }
      else
      {
        if(hkey_val == 0xF)
        {
          hkey_unused_digit = TRUE;
        }
        else if((hkey_val > 0x9) || (hkey_unused_digit))
        {
          is_valid = FALSE;
          break;
        }
      }
    }
  }while(0);

  UIM_MSG_HIGH_1("Hiddenkey data from card, valid: 0x%x", is_valid);

  return is_valid;
} /* mmgsdi_hidden_key_is_valid_from_card */


/*===========================================================================
  FUNCTION MMGSDI_HIDDEN_KEY_REFRESH_PROCESS_FCN

  DESCRIPTION:
    This function will handle the Refresh FCN of USIM/CSIM Hiddenkey

  PARAMS:
    event_ptr: Pointer to event data type
    file: Input file on which refresh FCN needs to be processed

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    None

  SIDE EFFECTS:
    Hiddenkey Event will be sent to clients
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_hidden_key_refresh_process_fcn(
  const mmgsdi_event_data_type *event_ptr,
  mmgsdi_file_enum_type         file
)
{
  mmgsdi_int_app_info_type       *app_info_ptr  = NULL;
  mmgsdi_evt_session_notify_type  notify_type;
  mmgsdi_data_type                hkey_data;
  mmgsdi_return_enum_type         mmgsdi_status = MMGSDI_ERROR;

  MMGSDIUTIL_RETURN_IF_NULL(event_ptr);

  memset(&notify_type, 0x00, sizeof(mmgsdi_evt_session_notify_type));
  memset(&hkey_data, 0x00, sizeof(mmgsdi_data_type));

  /* Get the app related to the session */
  mmgsdi_status = mmgsdi_util_get_prov_session_info(event_ptr->session_id,
                                                    NULL,
                                                    &notify_type.slot_id,
                                                    &app_info_ptr);

  if((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
  {
    return MMGSDI_SUCCESS;
  }

  /* Recache Hiddenkey data */
  mmgsdi_status = mmgsdi_card_init_cache_binary(
                    event_ptr->session_id,
                    event_ptr->data.refresh.aid.app_type,
                    event_ptr->data.refresh.slot,
                    file,
                    &hkey_data);

  /* Clear the Hiddenkey status */
  app_info_ptr->hiddenkey.status = MMGSDI_PIN_STATUS_NOT_INITIALIZED;

  if(mmgsdi_status == MMGSDI_SUCCESS)
  {
    /* Verify if the Hiddenkey data is valid */
    if(mmgsdi_hidden_key_is_valid_from_card(&hkey_data))
    {
      app_info_ptr->hiddenkey.status = MMGSDI_PIN_ENABLED_NOT_VERIFIED;
    }
    else
    {
      app_info_ptr->hiddenkey.status = MMGSDI_PIN_DISABLED;
    }
  }
  else if(mmgsdi_status == MMGSDI_NOT_FOUND)
  {
    app_info_ptr->hiddenkey.status = MMGSDI_PIN_NOT_FOUND;
  }

  notify_type.notify_type = MMGSDI_EVT_NOTIFY_ALL_SESSIONS;
  notify_type.session_id  = event_ptr->session_id;

  /* Send out the Hiddenkey event */
  mmgsdi_evt_build_and_send_pin(notify_type,
                                MMGSDI_HIDDENKEY,
                                app_info_ptr->hiddenkey.status,
                                (const mmgsdi_int_app_info_type*)app_info_ptr);

  MMGSDIUTIL_TMC_MEM_FREE(hkey_data.data_ptr);

  return mmgsdi_status;
} /* mmgsdi_hidden_key_refresh_process_fcn */


/*=============================================================================
FUNCTION:      MMGSDI_HIDDEN_KEY_VALIDATE_HKEY_DATA

DESCRIPTION:
  This is the validation function for checking the Hidden Key data received
  from the card

PARAMS:
  session_id: Input session id to validate read hidden key
  status: Read status
  data_ptr: Hidden key length and data

DEPENDENCIES:
  None

LIMITATIONS:
  NONE

RETURN VALUE:
  mmgsdi_return_enum_type

SIDE EFFECTS:
  NONE
============================================================================*/
mmgsdi_return_enum_type mmgsdi_hidden_key_validate_hkey_data (
  mmgsdi_session_id_type  session_id,
  mmgsdi_return_enum_type status,
  const mmgsdi_data_type *data_ptr
)
{
  mmgsdi_int_app_info_type       *app_info_ptr  = NULL;
  mmgsdi_evt_session_notify_type  notify_type;
  mmgsdi_return_enum_type         mmgsdi_status = MMGSDI_ERROR;

  memset(&notify_type, 0x00, sizeof(mmgsdi_evt_session_notify_type));

  /* Get the app related to the session */
  mmgsdi_status = mmgsdi_util_get_prov_session_info(session_id,
                                                    NULL,
                                                    &notify_type.slot_id,
                                                    &app_info_ptr);

  if((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
  {
    return MMGSDI_SUCCESS;
  }

  /* Identical to the PIN verification status update handling during REFRESH,
     Hiddenkey status is cleared and the Hiddenkey event sent out only if
     the app is completely reinitialized (i.e for  REFRESH APP_RESET,
     REFRESH RESET and REFRESH RESET_AUTO modes) or if EFHiddenkey
     is updated as part of a REFRESH FCN.
     For REFRESH modes that do post-pin1 app. init procedures
     (REFRESH INIT, INIT FCN, INIT FFCN, 3G Session RESET),
     Hiddenkey verification status is retained as is, and no
     event is sent out */

  /* Update Hiddenkey status and send out Hiddenkey event only if status
     is not initialized (i.e. during a regular power-up or a cache
     re-initialization due to a REFRESH APP_RESET/ REFRESH RESET/REFRESH
     RESET_AUTO) */
  if(app_info_ptr->hiddenkey.status == MMGSDI_PIN_STATUS_NOT_INITIALIZED)
  {
    if(status == MMGSDI_SUCCESS)
    {
      /* Verify if the Hiddenkey data is valid */
      if(mmgsdi_hidden_key_is_valid_from_card(data_ptr))
      {
        app_info_ptr->hiddenkey.status = MMGSDI_PIN_ENABLED_NOT_VERIFIED;
      }
      else
      {
        app_info_ptr->hiddenkey.status = MMGSDI_PIN_DISABLED;
      }
    }
    else if(status == MMGSDI_NOT_FOUND)
    {
      app_info_ptr->hiddenkey.status = MMGSDI_PIN_NOT_FOUND;
    }

    notify_type.notify_type = MMGSDI_EVT_NOTIFY_ALL_SESSIONS;
    notify_type.session_id  = session_id;

    /* Send out the Hiddenkey event */
    mmgsdi_evt_build_and_send_pin(notify_type,
                                  MMGSDI_HIDDENKEY,
                                  app_info_ptr->hiddenkey.status,
                                  (const mmgsdi_int_app_info_type*)app_info_ptr);
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_hidden_key_validate_hkey_data */
