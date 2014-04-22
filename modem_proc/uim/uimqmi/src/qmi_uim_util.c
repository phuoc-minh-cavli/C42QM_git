/*===========================================================================

                         Q M I _ U I M _ U T I L. C

DESCRIPTION

 The Data Services Qualcomm MSM Interface Device Management Services source
 file.

Copyright (c) 2016 - 2018 QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //components/rel/uim.mpss/3.3.1/uimqmi/src/qmi_uim_util.c#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/06/18    bcho   Copy label info for other than USIM/CSIM apps also
10/15/18    tq     Migration of QMI UIM service to QCCI-QCSI
10/13/17    rps    Remove old  perso support
08/10/16    ar     Support for setting terminal capability from AP
07/21/16    sp     Seperate ERA-GLONASS and SAP related QMI UIM functionality
07/14/16    ar     Move qmi_uimi_recovery and unify fdn/acl get_status func.
06/29/16    vdc    Remove callback user data from qmi uim global
06/22/16    sp     Increase max no of non prov session
05/26/16    sp     F3 logging improvements
05/19/16    vdc    Initial Revision.
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "intconv.h"
#include "mmgsdilib_p.h"
#include "qmi_uim_util.h"
#include "qmi_uim.h"
#include "qmi_uim_parsing.h"
#include "uim_msg.h"
#include "mmgsdisessionlib_v.h"
#include "amssassert.h"
#include <stringl/stringl.h>
#include "qmi_uim_multi_profile.h"
#include "qmi_uim_encryption.h"

/*===========================================================================

                            CONSTANT DEFINITIONS

===========================================================================*/


/*===========================================================================

                                DATA TYPES

===========================================================================*/



/*===========================================================================

                               INTERNAL DATA

===========================================================================*/



/*===========================================================================

                             EXTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
  FUNCTION QMI_UIM_UTIL_CHECK_CLID()

  DESCRIPTION
    To validate if the passed client handle is stil valid within the UIM
    service state ptr

  PARAMETERS
    client_handl[In]      : client handle provided by QCSI - QCCI, unique for each client
    client_reg_count[In]  : number o time time this clinet is deregistered
    cli_inx[Out]          : clinet index 

  RETURN VALUE
    TRUE if successful, else FALSE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_util_check_clid
(
  qmi_client_handle             client_handle,
  uint32                        client_reg_count,
  uint16                       *cli_inx
)
{
  uint16    client_index = 0;

  ASSERT(qmi_uim_state_ptr);

  if(qmi_uim_util_get_cli_index(client_handle, &client_index))
    {
      if(qmi_uim_state_ptr->client_reg_count[client_index] == client_reg_count)
      {
      if(cli_inx)
      {
        *cli_inx = client_index;
      }
        return TRUE;
      }
      else
      {
        UIM_MSG_HIGH_3("Client reg. count mismatch: 0x%x and 0x%x on clid",
                       client_reg_count,
                       qmi_uim_state_ptr->client_reg_count[client_index],
                      client_handle);

        return FALSE;
      }
    }
  UIM_MSG_HIGH_1("Invalid clid: 0x%x", client_handle);

  return FALSE;
} /* qmi_uim_util_check_clid */


/*===========================================================================
  FUNCTION QMI_UIM_UTIL_RESPONSE_IND_TOKEN()

  DESCRIPTION
    Composes the TLV content of indication token

  PARAMETERS
    tlv_id             : id to be used for the TLV
    response_pptr      : output sdu
    token              : token info to be sent

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_util_response_ind_token
(
  byte                            tlv_id,
  dsm_item_type **                response_pptr,
  uint32                          token
)
{
  PACKED struct PACKED_POST
  {
    uint32 token;
  } tlv_data;

  ASSERT(response_pptr);

  tlv_data.token  = token;

  return qmi_svc_put_param_tlv(response_pptr,
                               tlv_id,
                               sizeof(tlv_data),
                               (void *)&tlv_data);
} /* qmi_uim_util_response_ind_token */


/*===========================================================================
  FUNCTION QMI_UIM_UTIL_POPULATE_UINT32()

  DESCRIPTION
    It populate the uint32 data as well as mark valid 
    flag as true

  PARAMETERS
    field_valid_ptr[OUT] : output data valid or not
    field_data_ptr[OUT]  : output data 
    data[IN]             : input data

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_util_populate_uint32
(
  uint8                          * field_valid_ptr,
  uint32                         * field_data_ptr,
  uint32                           data
)
{
  ASSERT(field_valid_ptr && field_data_ptr);

  *field_valid_ptr = TRUE;
  *field_data_ptr  = data;
} /* qmi_uim_util_populate_uint32 */


/*===========================================================================
  FUNCTION QMI_UIM_UTIL_POPULATE_UINT16()

  DESCRIPTION
    It populate the uint32 data as well as mark valid 
    flag as true

  PARAMETERS
    field_valid_ptr[OUT] : output data valid or not
    field_data_ptr[OUT]  : output data 
    data[IN]             : input data

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_util_populate_uint16
(
  uint8                          * field_valid_ptr,
  uint16                         * field_data_ptr,
  uint32                           data
)
{
  ASSERT(field_valid_ptr && field_data_ptr);

  *field_valid_ptr = TRUE;
  *field_data_ptr  = data;
} /* qmi_uim_util_populate_uint16 */


/*===========================================================================
  FUNCTION QMI_UIM_UTIL_POPULATE_OPAQUE()

  DESCRIPTION
    Populate opeque field and field mark as valid.

  PARAMETERS 
    field_valid_ptr[OUT]         : output data is valid or not
    field_data_buf_ptr[OUT]      : output data buf
    field_data_len_ptr[OUT]      : output data len 
    field_data_buf_size[IN]      : output data buf size
    data_ptr[IN]                 : input data
    data_len[IN]                 : input data len
    mandatory                    : TLV mandatory or not

  RETURN VALUE
    boolean

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_util_populate_opaque
(
  uint8        * field_valid_ptr,
  uint8        * field_data_buf_ptr,
  uint32       * field_data_len_ptr,  
  uint32         field_data_buf_size,
  uint8        * data_ptr,
  uint32         data_len,
  boolean        mandatory
)
{
  if(mandatory &&
     (data_ptr == NULL ||data_len == 0 ||field_data_buf_ptr  == NULL || field_data_buf_size < data_len))
  {
    return FALSE;
  }
  
  if(field_valid_ptr)
  {
    *field_valid_ptr = TRUE;
  }
  if(field_data_buf_ptr)
  {
    memscpy(field_data_buf_ptr, field_data_buf_size, data_ptr, data_len);
  }
  if(field_data_len_ptr)
  {
    *field_data_len_ptr = data_len;
  }
  return TRUE;
} /* qmi_uim_util_populate_opaque */


/*===========================================================================
  FUNCTION QMI_UIM_UTIL_GET_CB_USERDATA

  DESCRIPTION
    To obtain the structure used for MMGSDI callbacks

  PARAMETERS
    request_id             : Type of the request
    cmd_buf_ptr            : QMI command buffer required for response
    token_ptr              : pointer to token info sent by client
    cli_handle             : Client handle to identify the clinet

  RETURN VALUE
    qmi_uimi_userdata_type : Poiner to the structure used for userdata

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_uimi_userdata_type* qmi_uim_util_get_cb_userdata
(
  uint16                     request_id,
  qmi_uim_cmd_buf_s        * cmd_buf_ptr,
  const uint32             * token_ptr,
  qmi_client_handle          cli_handle
)
{
  qmi_uimi_userdata_type  * userdata_ptr = NULL;
  uint16                    cli_index    = 0;

  ASSERT(qmi_uim_global_ptr && qmi_uim_state_ptr);

  if (!qmi_uim_util_get_cli_index(cli_handle, &cli_index))
  {
    UIM_MSG_ERR_1("Invalid cli_handle 0x%x", cli_handle);
    return NULL;
  }

  userdata_ptr = uimqmi_malloc(sizeof(qmi_uimi_userdata_type));
  if (userdata_ptr == NULL)
  {
    return NULL;
  }

  memset(userdata_ptr, 0, sizeof(qmi_uimi_userdata_type));

  /* Update parameters */
  userdata_ptr->request_id       = request_id;
  userdata_ptr->req_handle       = (cmd_buf_ptr) ? cmd_buf_ptr->req_handle : 0;
  userdata_ptr->client_handle    = cli_handle;
  userdata_ptr->client_reg_count = qmi_uim_state_ptr->client_reg_count[cli_index];

  if (token_ptr)
  {
    userdata_ptr->ind_token.token    = *token_ptr;
    userdata_ptr->ind_token.is_valid = TRUE;
  }

  return userdata_ptr;
} /* qmi_uim_util_get_cb_userdata */


/*===========================================================================
  FUNCTION QMI_UIM_UTIL_CREATE_MESSAGE

  DESCRIPTION
    Allocates the memory for a QMI UIM message and initializes it.

  PARAMETERS
    message_type: QMI UIM message type

  RETURN VALUE
    Pointer to the allocated message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_uim_message_type * qmi_uim_util_create_message
(
  qmi_uim_message_enum_type message_type
)
{
  qmi_uim_message_type * msg_ptr = NULL;

  msg_ptr = uimqmi_malloc(sizeof(qmi_uim_message_type));
  if (msg_ptr != NULL)
  {
    memset(msg_ptr, 0, sizeof(qmi_uim_message_type));
    msg_ptr->message_type = message_type;
  }

  return msg_ptr;
} /* qmi_uim_util_create_message */


/*===========================================================================
  FUNCTION QMI_UIM_UTIL_POST_MESSAGE

  DESCRIPTION
    Adds a message to the QMI UIM queue and posts an event to DCC task,
    so that it can be processed later in the right context.

  PARAMETERS
    uim_message_ptr: QMI UIM message

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_util_post_message
(
  qmi_uim_message_type * uim_message_ptr
)
{
  ASSERT(uim_message_ptr    != NULL);
  ASSERT(qmi_uim_global_ptr != NULL);

  /* Add the message to the MMGSDI queue */
  q_put(&qmi_uim_global_ptr->qmi_uim_q, &uim_message_ptr->link_next);

  /* Enable the MMGSDI task sig to process in MMGSDI context*/
  mmgsdi_set_qmi_uim_sig();
} /* qmi_uim_util_post_message */


/*===========================================================================
  FUNCTION QMI_UIM_UTIL_FREE_CB_USERDATA

  DESCRIPTION
      To free the structure used for MMGSDI callbacks

  PARAMETERS
    cb_userdata_ptr        : Callback user data pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_util_free_cb_userdata
(
  qmi_uimi_userdata_type*  cb_userdata_ptr
)
{
  ASSERT(qmi_uim_global_ptr);

  if (cb_userdata_ptr != NULL)
  {
    /* Free memory specific for each request id */
    switch(cb_userdata_ptr->request_id)
    {
      case QMI_UIM_READ_RECORD_REQ_V01:
        if (cb_userdata_ptr->data.read_record.records_cnf != NULL)
        {
          uint16   i = 0;
          for (i = 0; i < cb_userdata_ptr->data.read_record.read_records; i++)
          {
            /* Need to loop only thru the records that were read (successfully or not) */
            if (cb_userdata_ptr->data.read_record.records_cnf[i].read_data.data_ptr)
            {
              uimqmi_free(cb_userdata_ptr->data.read_record.records_cnf[i].read_data.data_ptr);
              cb_userdata_ptr->data.read_record.records_cnf[i].read_data.data_ptr = NULL;
            }
          }
              uimqmi_free(cb_userdata_ptr->data.read_record.records_cnf);
          cb_userdata_ptr->data.read_record.records_cnf = NULL;
        }
        break;
      case QMI_UIM_WRITE_RECORD_REQ_V01:
        if (cb_userdata_ptr->data.write_record.data.data_ptr != NULL)
        {
              uimqmi_free(cb_userdata_ptr->data.write_record.data.data_ptr);
          cb_userdata_ptr->data.write_record.data.data_ptr = NULL;
        }
        break;
      case QMI_UIM_PERSONALIZATION_REQ_V01:
      case QMI_UIM_PERSONALIZATION_SECURE_REQ_V01:
        if(cb_userdata_ptr->data.simlock_lock.code_ptr != NULL)
        {
          uimqmi_free(cb_userdata_ptr->data.simlock_lock.code_ptr);
          cb_userdata_ptr->data.simlock_lock.code_ptr = NULL;
        }
        break;
      default:
        /* Nothing to free */
        break;
    }

    memset(cb_userdata_ptr, 0, sizeof(qmi_uimi_userdata_type));
    uimqmi_free(cb_userdata_ptr);
    cb_userdata_ptr = NULL;
  }
} /* qmi_uim_util_free_cb_userdata */


/*===========================================================================
  FUNCTION QMI_UIM_UTIL_SAME_FILE

  DESCRIPTION
    Compares to files passed by path.

  PARAMETERS
    file1         : first file
    file2         : second file

  RETURN VALUE
    boolean       : Indicates if the two files are the same

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_util_same_file
(
  mmgsdi_path_type         file1,
  mmgsdi_path_type         file2
)
{
  if (file1.path_len > 0 &&
      file1.path_len <= MMGSDI_MAX_PATH_LEN &&
      file1.path_len == file2.path_len)
  {
    if (memcmp(file1.path_buf, file2.path_buf, file1.path_len * sizeof(uint16)) == 0)
    {
      return TRUE;
    }
  }
  return FALSE;
} /* qmi_uim_util_same_file */


/*===========================================================================
  FUNCTION QMI_UIM_UTIL_CONVERT_MMGSDI_STATUS_TO_ERRVAL

  DESCRIPTION
    To obtain QMI client-specific error message from MMGSDI error message

  PARAMETERS
    mmgsdi_status   : MMGSDI return value

  RETURN VALUE
    errval          : QMI client-specific error message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_uim_util_convert_mmgsdi_status_to_errval
(
  mmgsdi_return_enum_type mmgsdi_status
)
{
  qmi_error_type_v01 errval  = QMI_ERR_INTERNAL_V01;

  switch(mmgsdi_status)
  {
    case MMGSDI_SUCCESS:
      errval = QMI_ERR_NONE_V01;
      break;

    case MMGSDI_WARNING_NO_INFO_GIVEN:
    case MMGSDI_INCOMPAT_PIN_STATUS:
      errval = QMI_ERR_NO_EFFECT_V01;
      break;

    case MMGSDI_INCORRECT_CODE:
    case MMGSDI_PERSO_INVALID_CK:
      errval = QMI_ERR_INCORRECT_PIN_V01;
      break;

    case MMGSDI_CODE_BLOCKED:
    case MMGSDI_PERSO_CK_BLOCKED:
      errval = QMI_ERR_PIN_BLOCKED_V01;
      break;

    case MMGSDI_CODE_PERM_BLOCKED:
      errval = QMI_ERR_PIN_PERM_BLOCKED_V01;
      break;

    case MMGSDI_PIN_NOT_INITIALIZED:
      errval = QMI_ERR_SIM_NOT_INITIALIZED_V01;
      break;

    case MMGSDI_ACCESS_DENIED:
      errval = QMI_ERR_ACCESS_DENIED_V01;
      break;

    case MMGSDI_NOT_FOUND:
      errval = QMI_ERR_SIM_FILE_NOT_FOUND_V01;
      break;

    case MMGSDI_INCORRECT_PARAMS:
      errval = QMI_ERR_INTERNAL_V01;
      break;

    case MMGSDI_AUTH_ERROR_GSM_CNTXT_NOT_SUP:
    case MMGSDI_AUTH_ERROR_INCORRECT_MAC:
    case MMGSDI_AUTH_FAIL:
    case MMGSDI_AUTS_FAIL:
      errval = QMI_ERR_AUTHENTICATION_FAILED_V01;
      break;

    case MMGSDI_MULTI_PROFILE_NO_EFFECT:
      errval = QMI_ERR_NO_EFFECT_V01;
      break;

    case MMGSDI_MULTI_PROFILE_NOT_SUPPORTED:
      errval = QMI_ERR_NOT_SUPPORTED_V01;
      break;

    default:
      errval = QMI_ERR_INTERNAL_V01;
      break;
  }

  if(mmgsdi_status != MMGSDI_SUCCESS )
  {
    UIM_MSG_HIGH_1("Received mmgsdi_status 0x%x", mmgsdi_status);
  }
  return errval;
} /* qmi_uim_util_convert_mmgsdi_status_to_errval() */


/*===========================================================================
  FUNCTION QMI_UIM_UTIL_SESSION_TYPE_TO_SESSION_ID

  DESCRIPTION
    Convert a session type and AID in the corresponding session id.

  PARAMETERS
    session_id_ptr : Output parameter with session id
    session_info   : session type
    aid            : AID value

  RETURN VALUE
    errval         : QMI client-specific error message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_uim_util_session_type_to_session_id
(
  mmgsdi_session_id_type *        session_id_ptr,
  mmgsdi_session_type_enum_type   session_type,
  mmgsdi_static_data_type         aid
)
{
  uint16  i  = 0;

  ASSERT(session_id_ptr);
  ASSERT(qmi_uim_global_ptr);

  /* Initialize output value */
  *session_id_ptr = UIM_INVALID_SESSION_ID;

  switch(session_type)
  {
    case MMGSDI_GW_PROV_PRI_SESSION:
      *session_id_ptr = qmi_uim_global_ptr->mmgsdi_gw_session_id[0];
      break;

    case MMGSDI_1X_PROV_PRI_SESSION:
      *session_id_ptr = qmi_uim_global_ptr->mmgsdi_1x_session_id[0];
      break;

    case MMGSDI_GW_PROV_SEC_SESSION:
      *session_id_ptr = qmi_uim_global_ptr->mmgsdi_gw_session_id[1];
      break;

    case MMGSDI_1X_PROV_SEC_SESSION:
      *session_id_ptr = qmi_uim_global_ptr->mmgsdi_1x_session_id[1];
      break;

    case MMGSDI_GW_PROV_TER_SESSION:
      *session_id_ptr = qmi_uim_global_ptr->mmgsdi_gw_session_id[2];
      break;

    case MMGSDI_1X_PROV_TER_SESSION:
      *session_id_ptr = qmi_uim_global_ptr->mmgsdi_1x_session_id[2];
      break;

    case MMGSDI_CARD_SESSION_SLOT_1:
      *session_id_ptr = qmi_uim_global_ptr->mmgsdi_card_slot_session_id[0];
      break;

    case MMGSDI_CARD_SESSION_SLOT_2:
      *session_id_ptr = qmi_uim_global_ptr->mmgsdi_card_slot_session_id[1];
      break;

    case MMGSDI_CARD_SESSION_SLOT_3:
      *session_id_ptr = qmi_uim_global_ptr->mmgsdi_card_slot_session_id[2];
      break;

    case MMGSDI_NON_PROV_SESSION_SLOT_1:
    case MMGSDI_NON_PROV_SESSION_SLOT_2:
    case MMGSDI_NON_PROV_SESSION_SLOT_3:
      if (aid.data_len == QMI_UIM_TLV_CHANNEL_ID_LEN)
      {
        mmgsdi_slot_id_enum_type slot = MMGSDI_SLOT_1;
        if(session_type == MMGSDI_NON_PROV_SESSION_SLOT_1)
        {
          slot = MMGSDI_SLOT_1;
        }
        else if(session_type == MMGSDI_NON_PROV_SESSION_SLOT_2)
        {
          slot = MMGSDI_SLOT_2;
        }
        else
        {
          slot = MMGSDI_SLOT_3;
        }

        /* AID length is 1 for a session TLV that is overloaded with channel id, so check
           apdu_channel info for session_id */
        for (i = 0; i < UIM_MAX_APDU_CHANNEL_COUNT; i++)
        {
          if (qmi_uim_global_ptr->apdu_channel[i] &&
              qmi_uim_global_ptr->apdu_channel[i]->slot == slot &&
              qmi_uim_global_ptr->apdu_channel[i]->logical_channel == aid.data_ptr[0])
          {
            *session_id_ptr = qmi_uim_global_ptr->apdu_channel[i]->mmgsdi_session_id;
            break;
          }
        }
      }
       else if (aid.data_len <= MMGSDI_MAX_AID_LEN)
      {
        /* For all other AID length, check non_prov_session info */
        for (i = 0; i < UIM_MAX_NON_PROV_SESSIONS; i++)
        {
          if (qmi_uim_global_ptr->non_prov_session[i] != NULL &&
              qmi_uim_global_ptr->non_prov_session[i]->session_type == session_type)
          {
            if ((qmi_uim_global_ptr->non_prov_session[i]->aid.data_len == 0 &&
                 aid.data_len == 0)||
                (qmi_uim_global_ptr->non_prov_session[i]->aid.data_len > 0 &&
                 qmi_uim_global_ptr->non_prov_session[i]->aid.data_len == aid.data_len &&
                 memcmp((void*)qmi_uim_global_ptr->non_prov_session[i]->aid.data_ptr,
                        (void*)aid.data_ptr,
                        int32touint32(aid.data_len) ) == 0))
            {
              *session_id_ptr = qmi_uim_global_ptr->non_prov_session[i]->mmgsdi_session_id;
              break;
            }
          }
        }
      }
      else
      {
        UIM_MSG_ERR_1("Unexpected aid length: 0x%x", aid.data_len);
        return QMI_ERR_INTERNAL_V01;
      }
      break;

    default:
      UIM_MSG_ERR_1("Invalid session type: 0x%x", session_type);
      return QMI_ERR_INTERNAL_V01;
  }

  /* Even if the session was found, it might be not initialized yet.
     Check the value before continuing */
  if (*session_id_ptr == UIM_INVALID_SESSION_ID)
  {
    UIM_MSG_ERR_1("Session not ready: 0x%x", session_type);
    if(session_type != MMGSDI_NON_PROV_SESSION_SLOT_1 &&
       session_type != MMGSDI_NON_PROV_SESSION_SLOT_2 &&
       session_type != MMGSDI_NON_PROV_SESSION_SLOT_3)
    {
      return QMI_ERR_DEVICE_NOT_READY_V01;
    }
    return QMI_ERR_INTERNAL_V01;
  }

  return QMI_ERR_NONE_V01;
} /* qmi_uim_util_session_type_to_session_id */


/*===========================================================================
  FUNCTION QMI_UIM_UTIL_SESSION_ID_TO_SESSION_TYPE

  DESCRIPTION
    Convert a session id in the corresponding session type and AID.

  PARAMETERS
    session_type_ptr : Output parameter with session type
    aid_ptr          : Output parameter with AID
    session_id       : session id

  RETURN VALUE
    result           : QMI client-specific error message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_uim_util_session_id_to_session_type
(
  mmgsdi_session_type_enum_type * session_type_ptr,
  mmgsdi_static_data_type *       aid_ptr,
  mmgsdi_session_id_type          session_id
)
{
  uint16              i       = 0;
  qmi_error_type_v01  result  = QMI_ERR_NONE_V01;

  ASSERT(session_type_ptr);
  ASSERT(aid_ptr);
  ASSERT(qmi_uim_global_ptr);

  /* Initialize AID */
  memset(aid_ptr, 0, sizeof(mmgsdi_static_data_type));

  if (session_id == qmi_uim_global_ptr->mmgsdi_gw_session_id[0])
  {
    *session_type_ptr = MMGSDI_GW_PROV_PRI_SESSION;
  }
  else if (session_id == qmi_uim_global_ptr->mmgsdi_1x_session_id[0])
  {
    *session_type_ptr = MMGSDI_1X_PROV_PRI_SESSION;
  }
  else if (session_id == qmi_uim_global_ptr->mmgsdi_gw_session_id[1])
  {
    *session_type_ptr = MMGSDI_GW_PROV_SEC_SESSION;
  }
  else if (session_id == qmi_uim_global_ptr->mmgsdi_1x_session_id[1])
  {
    *session_type_ptr = MMGSDI_1X_PROV_SEC_SESSION;
  }
  else if (session_id == qmi_uim_global_ptr->mmgsdi_gw_session_id[2])
  {
    *session_type_ptr = MMGSDI_GW_PROV_TER_SESSION;
  }
  else if (session_id == qmi_uim_global_ptr->mmgsdi_1x_session_id[2])
  {
    *session_type_ptr = MMGSDI_1X_PROV_TER_SESSION;
  }
  else if (session_id == qmi_uim_global_ptr->mmgsdi_card_slot_session_id[0])
  {
    *session_type_ptr = MMGSDI_CARD_SESSION_SLOT_1;
  }
  else if (session_id == qmi_uim_global_ptr->mmgsdi_card_slot_session_id[1])
  {
    *session_type_ptr = MMGSDI_CARD_SESSION_SLOT_2;
  }
  else if (session_id == qmi_uim_global_ptr->mmgsdi_card_slot_session_id[2])
  {
    *session_type_ptr = MMGSDI_CARD_SESSION_SLOT_3;
  }
  else
  {
    /* Default to error case */
    result = QMI_ERR_INTERNAL_V01;

    /* Check if it's a non provisioning session id */
    for (i = 0; i < UIM_MAX_NON_PROV_SESSIONS; i++)
    {
      if (qmi_uim_global_ptr->non_prov_session[i] != NULL  &&
          qmi_uim_global_ptr->non_prov_session[i]->mmgsdi_session_id == session_id)
      {
        *session_type_ptr = qmi_uim_global_ptr->non_prov_session[i]->session_type;
        (void)memscpy(aid_ptr,
                      sizeof(mmgsdi_static_data_type),
                      &qmi_uim_global_ptr->non_prov_session[i]->aid,
                      sizeof(mmgsdi_static_data_type));

        result = QMI_ERR_NONE_V01;
        break;
      }
    }
  }

  if (result != QMI_ERR_NONE_V01)
  {
    UIM_MSG_ERR_1("Session id to session type: invalid session id: 0x%x",
                  session_id);
  }

  return result;
} /* qmi_uim_util_session_id_to_session_type */


/*===========================================================================
  FUNCTION QMI_UIM_UTIL_SESSION_TYPE_TO_CARD_TYPE

  DESCRIPTION
    Convert a session type and AID in the corresponding card type.

  PARAMETERS
    card_type_ptr : Output parameter with card type
    session_info  : session type
    aid           : AID value

  RETURN VALUE
    errval         : QMI client-specific error message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_uim_util_session_type_to_card_type
(
  mmgsdi_app_enum_type *          card_type_ptr,
  mmgsdi_session_type_enum_type   session_type,
  mmgsdi_static_data_type         aid
)
{
  uint16                   i     = 0;
  uint16                   index = UIM_INVALID_APP_INDEX;
  mmgsdi_slot_id_enum_type slot  = MMGSDI_SLOT_1;

  ASSERT(card_type_ptr);
  ASSERT(qmi_uim_global_ptr);

  /* Initialize output value */
  *card_type_ptr = MMGSDI_APP_NONE;

  switch(session_type)
  {
    case MMGSDI_GW_PROV_PRI_SESSION:
      index = qmi_uim_global_ptr->card_state.index_gw[0];
      break;

    case MMGSDI_1X_PROV_PRI_SESSION:
      index = qmi_uim_global_ptr->card_state.index_1x[0];
      break;

    case MMGSDI_GW_PROV_SEC_SESSION:
      index = qmi_uim_global_ptr->card_state.index_gw[1];
      break;

    case MMGSDI_1X_PROV_SEC_SESSION:
      index = qmi_uim_global_ptr->card_state.index_1x[1];
      break;

    case MMGSDI_GW_PROV_TER_SESSION:
      index = qmi_uim_global_ptr->card_state.index_gw[2];
      break;

    case MMGSDI_1X_PROV_TER_SESSION:
      index = qmi_uim_global_ptr->card_state.index_1x[2];
      break;

    case MMGSDI_CARD_SESSION_SLOT_1:
    case MMGSDI_CARD_SESSION_SLOT_2:
    case MMGSDI_CARD_SESSION_SLOT_3:
      if(session_type == MMGSDI_CARD_SESSION_SLOT_1)
      {
        slot = MMGSDI_SLOT_1;
      }
      else if(session_type == MMGSDI_CARD_SESSION_SLOT_2)
      {
        slot = MMGSDI_SLOT_2;
      }
      else
      {
        slot = MMGSDI_SLOT_3;
      }

      for (i = 0; i < qmi_uim_global_ptr->card_state.num_apps &&
                  i < QMI_UIM_MAX_APP_COUNT; i++)
      {
        if (qmi_uim_global_ptr->card_state.application[i] != NULL &&
            qmi_uim_global_ptr->card_state.application[i]->slot == slot)
        {
          index = i;
          break;
        }
      }
      break;

    case MMGSDI_NON_PROV_SESSION_SLOT_1:
    case MMGSDI_NON_PROV_SESSION_SLOT_2:
    case MMGSDI_NON_PROV_SESSION_SLOT_3:
      if(session_type == MMGSDI_NON_PROV_SESSION_SLOT_1)
      {
        slot = MMGSDI_SLOT_1;
      }
      else if(session_type == MMGSDI_NON_PROV_SESSION_SLOT_2)
      {
        slot = MMGSDI_SLOT_2;
      }
      else
      {
        slot = MMGSDI_SLOT_3;
      }

      for (i = 0; i < qmi_uim_global_ptr->card_state.num_apps &&
                  i < QMI_UIM_MAX_APP_COUNT; i++)
      {
        if (qmi_uim_global_ptr->card_state.application[i] != NULL &&
            qmi_uim_global_ptr->card_state.application[i]->slot == slot &&
            qmi_uim_global_ptr->card_state.application[i]->aid.data_len == aid.data_len)
        {
          if (memcmp(qmi_uim_global_ptr->card_state.application[i]->aid.data_ptr,
                     aid.data_ptr,
                     (uint32)aid.data_len) == 0)
          {
            index = i;
            break;
          }
        }
      }
      break;

    default:
      UIM_MSG_ERR_1("Invalid session type: 0x%x", session_type);
      return QMI_ERR_INTERNAL_V01;
  }

  /* Check if the index is valid */
  if (index >= qmi_uim_global_ptr->card_state.num_apps ||
      index >= QMI_UIM_MAX_APP_COUNT ||
      qmi_uim_global_ptr->card_state.application[index] == NULL)
  {
    UIM_MSG_ERR_1("Invalid index: 0x%x", index);
    return QMI_ERR_INTERNAL_V01;
  }

  /* Check if type is valid */
  if (qmi_uim_global_ptr->card_state.application[index]->app_type != MMGSDI_APP_SIM &&
      qmi_uim_global_ptr->card_state.application[index]->app_type != MMGSDI_APP_RUIM &&
      qmi_uim_global_ptr->card_state.application[index]->app_type != MMGSDI_APP_USIM &&
      qmi_uim_global_ptr->card_state.application[index]->app_type != MMGSDI_APP_CSIM)
  {
    UIM_MSG_ERR_1("Invalid card type: 0x%x",
                  qmi_uim_global_ptr->card_state.application[index]->app_type);
    return QMI_ERR_INTERNAL_V01;
  }

  *card_type_ptr = qmi_uim_global_ptr->card_state.application[index]->app_type;
  return QMI_ERR_NONE_V01;
} /* qmi_uim_util_session_type_to_card_type */


/*===========================================================================
  FUNCTION QMI_UIM_UTIL_SESSION_ID_TO_SLOT

  DESCRIPTION
    Convert a session id in the corresponding slot.

  PARAMETERS
    slot_ptr      : Output parameter with slot value
    session_id    : session id

  RETURN VALUE
    errval        : QMI client-specific error message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_uim_util_session_id_to_slot
(
  mmgsdi_slot_id_enum_type * slot_ptr,
  mmgsdi_session_id_type     session_id
)
{
  uint16 i     = 0;
  uint16 index = UIM_INVALID_APP_INDEX;

  ASSERT(slot_ptr);
  ASSERT(qmi_uim_global_ptr);

  for (i = 0; i < QMI_UIM_MAX_PROV_SESSIONS; i++)
  {
    if (session_id == qmi_uim_global_ptr->mmgsdi_gw_session_id[i])
    {
      index = qmi_uim_global_ptr->card_state.index_gw[i];
      break;
    }
    else if (session_id == qmi_uim_global_ptr->mmgsdi_1x_session_id[i])
    {
      index = qmi_uim_global_ptr->card_state.index_1x[i];
      break;
    }
  }

  if (index == UIM_INVALID_APP_INDEX)
  {
    for (i = 0; i < QMI_UIM_MAX_CARD_SESSIONS; i++)
    {
      if (session_id == qmi_uim_global_ptr->mmgsdi_card_slot_session_id[i])
      {
        switch (i)
        {
          case 0:
            *slot_ptr = MMGSDI_SLOT_1;
            return QMI_ERR_NONE_V01;
          case 1:
            *slot_ptr = MMGSDI_SLOT_2;
            return QMI_ERR_NONE_V01;
          case 2:
            *slot_ptr = MMGSDI_SLOT_3;
            return QMI_ERR_NONE_V01;
          default:
            return QMI_ERR_INTERNAL_V01;
        }
      }
    }

    /* Check if it's a non provisioning session id */
    for (i = 0; i < UIM_MAX_NON_PROV_SESSIONS; i++)
    {
      if (qmi_uim_global_ptr->non_prov_session[i] != NULL  &&
          qmi_uim_global_ptr->non_prov_session[i]->mmgsdi_session_id == session_id)
      {
        switch (qmi_uim_global_ptr->non_prov_session[i]->session_type)
        {
          case MMGSDI_NON_PROV_SESSION_SLOT_1:
            *slot_ptr = MMGSDI_SLOT_1;
            return QMI_ERR_NONE_V01;

          case MMGSDI_NON_PROV_SESSION_SLOT_2:
            *slot_ptr = MMGSDI_SLOT_2;
            return QMI_ERR_NONE_V01;

          case MMGSDI_NON_PROV_SESSION_SLOT_3:
            *slot_ptr = MMGSDI_SLOT_3;
            return QMI_ERR_NONE_V01;

          default:
            /* Nothing to do */
            break;
        }
      }
    }

    /* Check if the session id belongs to an APDU session */
    for (i = 0; i < UIM_MAX_APDU_CHANNEL_COUNT; i++)
    {
      if (qmi_uim_global_ptr->apdu_channel[i] != NULL &&
          qmi_uim_global_ptr->apdu_channel[i]->mmgsdi_session_id == session_id)
      {
        *slot_ptr = qmi_uim_global_ptr->apdu_channel[i]->slot;
        return QMI_ERR_NONE_V01;
      }
    }

    UIM_MSG_HIGH_1("Session id to slot: invalid session id: 0x%x", session_id);
    return QMI_ERR_INTERNAL_V01;
  }

  /* We arrive here in case of provisioning application.
     Check index value. */
  if (index == UIM_INVALID_APP_INDEX ||
      index >= qmi_uim_global_ptr->card_state.num_apps ||
      index >= QMI_UIM_MAX_APP_COUNT ||
      qmi_uim_global_ptr->card_state.application[index] == NULL)
  {
    UIM_MSG_ERR_1("Invalid index for slot conversion: 0x%x", index);
    return QMI_ERR_INTERNAL_V01;
  }

  *slot_ptr = qmi_uim_global_ptr->card_state.application[index]->slot;
  return QMI_ERR_NONE_V01;
} /* qmi_uim_util_session_id_to_slot */


/*===========================================================================
  FUNCTION QMI_UIM_UTIL_CARD_PROTOCOL()

  DESCRIPTION
    Utility function to find out if the card is ICC or UICC.

  PARAMETERS
    slot                       : slot

  RETURN VALUE
    mmgsdi_protocol_enum_type  : Whether the card is ICC or UICC

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
mmgsdi_protocol_enum_type qmi_uim_util_card_protocol
(
  mmgsdi_slot_id_enum_type  slot
)
{
  uint16   index            = 0;
  boolean  unknown_uicc_app = FALSE;

  ASSERT(qmi_uim_global_ptr);

  if (slot != MMGSDI_SLOT_1 &&
      slot != MMGSDI_SLOT_2 &&
      slot != MMGSDI_SLOT_3)
  {
    return MMGSDI_NO_PROTOCOL;
  }

  for (index = 0; index < qmi_uim_global_ptr->card_state.num_apps &&
                  index < QMI_UIM_MAX_APP_COUNT; index++)
  {
    if (qmi_uim_global_ptr->card_state.application[index] != NULL &&
        qmi_uim_global_ptr->card_state.application[index]->slot == slot)
    {
      if (qmi_uim_global_ptr->card_state.application[index]->app_type == MMGSDI_APP_SIM ||
          qmi_uim_global_ptr->card_state.application[index]->app_type == MMGSDI_APP_RUIM)
      {
        return MMGSDI_ICC;
      }
      else if (qmi_uim_global_ptr->card_state.application[index]->app_type == MMGSDI_APP_USIM ||
               qmi_uim_global_ptr->card_state.application[index]->app_type == MMGSDI_APP_CSIM)
      {
        return MMGSDI_UICC;
      }
      else if (qmi_uim_global_ptr->card_state.application[index]->app_type == MMGSDI_APP_UNKNOWN)
      {
        unknown_uicc_app = TRUE;
      }
    }
  }

  /* no known GW or 1X apps but there is unknown uicc app, must be UICC card */
  if (unknown_uicc_app == TRUE)
  {
    return MMGSDI_UICC;
  }

  /* no known GW or 1X application or unknown app, return error for now */
  UIM_MSG_HIGH_0("Found neither any known GW/1X application nor unknown UICC app");
  return MMGSDI_NO_PROTOCOL;
} /* qmi_uim_util_card_protocol */


/*===========================================================================
  FUNCTION QMI_UIM_UTIL_SESSION_ID_TO_REFRESH_INDEX

  DESCRIPTION
    Convert a session id into an index.

  PARAMETERS
    refresh_index     : output parameter with index used for refresh.
    session_id    : session id

  RETURN VALUE
    errval        : QMI client-specific error message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_uim_util_session_id_to_refresh_index
(
  uint16                 * refresh_index_ptr,
  mmgsdi_session_id_type   session_id
)
{
  uint16                         i            = 0;
  qmi_error_type_v01             result       = QMI_ERR_NONE_V01;
  mmgsdi_session_type_enum_type  session_type = MMGSDI_GW_PROV_PRI_SESSION;
  mmgsdi_static_data_type        aid;

  memset(&aid, 0, sizeof(mmgsdi_static_data_type));

  ASSERT(refresh_index_ptr != NULL);
  ASSERT(qmi_uim_global_ptr != NULL);

  /* Convert session id in session type */
  result = qmi_uim_util_session_id_to_session_type(&session_type,
                                                   &aid,
                                                   session_id);
  if (result != QMI_ERR_NONE_V01)
  {
    return result;
  }

  /* Convert session type into refresh index */
  switch(session_type)
  {
    case MMGSDI_GW_PROV_PRI_SESSION:
      *refresh_index_ptr = 0;
      return QMI_ERR_NONE_V01;
    case MMGSDI_1X_PROV_PRI_SESSION:
      *refresh_index_ptr = 1;
      return QMI_ERR_NONE_V01;
    case MMGSDI_GW_PROV_SEC_SESSION:
      *refresh_index_ptr = 2;
      return QMI_ERR_NONE_V01;
    case MMGSDI_1X_PROV_SEC_SESSION:
      *refresh_index_ptr = 3;
      return QMI_ERR_NONE_V01;
    case MMGSDI_GW_PROV_TER_SESSION:
      *refresh_index_ptr = 4;
      return QMI_ERR_NONE_V01;
    case MMGSDI_1X_PROV_TER_SESSION:
      *refresh_index_ptr = 5;
      return QMI_ERR_NONE_V01;
    case MMGSDI_CARD_SESSION_SLOT_1:
      *refresh_index_ptr = 6;
      return QMI_ERR_NONE_V01;
    case MMGSDI_CARD_SESSION_SLOT_2:
      *refresh_index_ptr = 7;
      return QMI_ERR_NONE_V01;
    case MMGSDI_CARD_SESSION_SLOT_3:
      *refresh_index_ptr = 8;
      return QMI_ERR_NONE_V01;
    case MMGSDI_NON_PROV_SESSION_SLOT_1:
    case MMGSDI_NON_PROV_SESSION_SLOT_2:
    case MMGSDI_NON_PROV_SESSION_SLOT_3:
      /* Search if this session id was already used */
      for (i = 0; i < UIM_REFRESH_NON_PROV_COUNT; i++)
      {
        if (qmi_uim_global_ptr->non_prov_refresh_session_id[i] == session_id)
        {
          *refresh_index_ptr = UIM_REFRESH_PROV_COUNT + i;
          return QMI_ERR_NONE_V01;
        }
      }
      /* Find a slot for this new session id */
      for (i = 0; i < UIM_REFRESH_NON_PROV_COUNT; i++)
      {
        if (qmi_uim_global_ptr->non_prov_refresh_session_id[i] == UIM_INVALID_SESSION_ID)
        {
          qmi_uim_global_ptr->non_prov_refresh_session_id[i] = session_id;
          *refresh_index_ptr = UIM_REFRESH_PROV_COUNT + i;
          return QMI_ERR_NONE_V01;
        }
      }
      break;
    default:
      break;
  }

  return QMI_ERR_INTERNAL_V01;
} /* qmi_uim_util_session_id_to_refresh_index */


/*===========================================================================
  FUNCTION QMI_UIM_UTIL_MMGDSI_SLOT_ID_TO_UIM_SLOT_ID()

  DESCRIPTION
    Converts MMGSDI slot id to UIM slot id.

  PARAMETERS
    mmgsdi_slot_id : MMGSDI slot id
    uim_slot_id    : output parameter containing UIM slot id

  RETURN VALUE
    QMI client-specific error message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_uim_util_mmgsdi_slot_id_to_uim_slot_id
(
  mmgsdi_slot_id_enum_type  mmgsdi_slot_id,
  uim_slot_type            *uim_slot_id
)
{
  ASSERT(uim_slot_id);

  switch(mmgsdi_slot_id)
  {
    case MMGSDI_SLOT_1:
      *uim_slot_id = UIM_SLOT_1;
      break;

    case MMGSDI_SLOT_2:
      *uim_slot_id = UIM_SLOT_2;
      break;

    case MMGSDI_SLOT_3:
      *uim_slot_id = UIM_SLOT_3;
      break;

    default:
      *uim_slot_id = UIM_SLOT_NONE;
      return QMI_ERR_INTERNAL_V01;
  }

  return QMI_ERR_NONE_V01;
} /* qmi_uim_util_mmgsdi_slot_id_to_uim_slot_id */


/*===========================================================================
  FUNCTION QMI_UIM_UTIL_UIM_SLOT_ID_TO_MMGSDI_SLOT_ID()

  DESCRIPTION
    Converts UIM slot id to MMGSDI slot id.

  PARAMETERS
    uim_slot_id         : UIM slot id
    mmgsdi_slot_id_ptr  : output parameter containing MMGSDI slot id

  RETURN VALUE
    QMI client-specific error message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_uim_util_uim_slot_id_to_mmgsdi_slot_id
(
  uim_slot_type             uim_slot_id,
  mmgsdi_slot_id_enum_type *mmgsdi_slot_id_ptr
)
{
  ASSERT(mmgsdi_slot_id_ptr);

  switch(uim_slot_id)
  {
    case UIM_SLOT_1:
      *mmgsdi_slot_id_ptr = MMGSDI_SLOT_1;
      break;

    case UIM_SLOT_2:
      *mmgsdi_slot_id_ptr = MMGSDI_SLOT_2;
      break;

    case UIM_SLOT_3:
      *mmgsdi_slot_id_ptr = MMGSDI_SLOT_3;
      break;

    default:
      *mmgsdi_slot_id_ptr = MMGSDI_MAX_SLOT_ID_ENUM;
      return QMI_ERR_INTERNAL_V01;
  }

  return QMI_ERR_NONE_V01;
} /* qmi_uim_util_uim_slot_id_to_mmgsdi_slot_id */


/*===========================================================================
  FUNCTION QMI_UIM_UTIL_SLOT_INDEX_TO_MMGSDI_SLOT()

  DESCRIPTION
    Determines the MMGSDI slot id from the slot index

  PARAMETERS
    slot_index          : slot index
    mmgsdi_slot_ptr     : output parameter containing MMGSDI slot id

  RETURN VALUE
    QMI client-specific error message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_uim_util_slot_index_to_mmgsdi_slot
(
  uint8                              slot_index,
  mmgsdi_slot_id_enum_type          *mmgsdi_slot_ptr
)
{
  ASSERT(mmgsdi_slot_ptr);

  switch(slot_index)
  {
    case 0:
      *mmgsdi_slot_ptr = MMGSDI_SLOT_1;
      break;

    case 1:
      *mmgsdi_slot_ptr = MMGSDI_SLOT_2;
      break;

    case 2:
      *mmgsdi_slot_ptr = MMGSDI_SLOT_3;
      break;

    default:
      return QMI_ERR_INTERNAL_V01;
  }

  return QMI_ERR_NONE_V01;
}/* qmi_uim_util_slot_index_to_mmgsdi_slot */


/*===========================================================================
  FUNCTION QMI_UIM_UTIL_GSTK_SLOT_ID_TO_MMGSDI_SLOT_ID()

  DESCRIPTION
    Converts GSTK slot id to MMGSDI slot id.

  PARAMETERS
    gstk_slot_id        : GSTK slot id
    mmgsdi_slot_id_ptr  : output parameter containing MMGSDI slot id

  RETURN VALUE
    QMI client-specific error message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_uim_util_gstk_slot_id_to_mmgsdi_slot_id
(
  gstk_slot_id_enum_type     gstk_slot_id,
  mmgsdi_slot_id_enum_type  *mmgsdi_slot_id_ptr
)
{
  switch(gstk_slot_id)
  {
    case GSTK_SLOT_1:
      *mmgsdi_slot_id_ptr = MMGSDI_SLOT_1;
      break;

    case GSTK_SLOT_2:
      *mmgsdi_slot_id_ptr = MMGSDI_SLOT_2;
      break;

    case GSTK_SLOT_3:
      *mmgsdi_slot_id_ptr = MMGSDI_SLOT_3;
      break;

    default:
      *mmgsdi_slot_id_ptr = MMGSDI_MAX_SLOT_ID_ENUM;
      return QMI_ERR_INTERNAL_V01;
  }

  return QMI_ERR_NONE_V01;
} /* qmi_uim_util_gstk_slot_id_to_mmgsdi_slot_id */


/*===========================================================================
  FUNCTION QMI_UIM_UTIL_GET_BUF_PTR()

  DESCRIPTION
   This function uses client handle and request handle to check
   if any cmd is available in the client cmd q corresponding
   to provided clinet handle.

  PARAMETERS
    qmi_req_handle        req_handle
    qmi_client_handle     cli_handle

  RETURN VALUE
    qmi_uim_cmd_buf_s

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_uim_cmd_buf_s* qmi_uim_util_get_cmd_buf_ptr
(
  qmi_req_handle        req_handle,
  qmi_client_handle     cli_handle
)
{
  qmi_uim_compare_s       compare_s   = {0, };
  uint16                  cli_index   = 0;
  qmi_uim_cmd_buf_s      *cmd_buf_ptr = NULL;

  ASSERT(qmi_uim_state_ptr);
  
  if(!qmi_uim_util_get_cli_index( cli_handle, &cli_index))
  { 
    return NULL;
  }
  
  compare_s.client_handle = cli_handle;
  compare_s.req_handle    = req_handle;
  
  cmd_buf_ptr =   q_linear_search(&qmi_uim_state_ptr->client[cli_index]->cmd_q , 
                                  qmi_uim_util_compare_qmi_cmd, 
                                  (void*)&compare_s);

  return cmd_buf_ptr;
} /* qmi_uim_util_get_cmd_buf_ptr */


/*===========================================================================
  FUNCTION QMI_UIM_CONVERT_SLOT_TO_INDEX()

  DESCRIPTION
    Converts MMGSDI slot information into index, starting from 0.

  PARAMETERS
    slot_id : MMGSDI slot information
    index   : output parameter with index value

  RETURN VALUE
    boolean to indicate success or failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_convert_slot_to_index
(
  mmgsdi_slot_id_enum_type  slot_id,
  uint8 *                   index_ptr
)
{
  ASSERT(index_ptr);

  switch(slot_id)
  {
    case MMGSDI_SLOT_1:
      *index_ptr = 0;
      break;

    case MMGSDI_SLOT_2:
      *index_ptr = 1;
      break;

    case MMGSDI_SLOT_3:
      *index_ptr = 2;
      break;

    default:
      return FALSE;
  }

  if (*index_ptr >= QMI_UIM_MAX_CARD_COUNT)
  {
    return FALSE;
  }

  return TRUE;
} /* qmi_uim_convert_slot_to_index */


/*===========================================================================
  FUNCTION QMI_UIM_UTIL_IS_APDU_SESSION

  DESCRIPTION
    Check if the given session id belongs to an APDU session

  PARAMETERS
    session_id    : session id

  RETURN VALUE
    boolean       : indicates if the session id belongs to an APDU session

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_util_is_apdu_session
(
  mmgsdi_session_id_type   session_id
)
{
  uint8 i = 0;

  for (i = 0; i < UIM_MAX_APDU_CHANNEL_COUNT; i++)
  {
    if (qmi_uim_global_ptr->apdu_channel[i] != NULL &&
        qmi_uim_global_ptr->apdu_channel[i]->mmgsdi_session_id == session_id)
    {
      return TRUE;
    }
  }

  return FALSE;
} /* qmi_uim_util_is_apdu_session */


/*===========================================================================
  FUNCTION QMI_UIM_UTIL_READ_IMSI_FOR_SESSION_ID()

  DESCRIPTION
    Reads the IMSI from the card for the specified Session_ID

  PARAMETERS
    session_id       : Session ID
    imsi_ptr         : Pointer for IMSI data

  RETURN VALUE
    errval           : QMI client-specific error message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_uim_util_read_imsi_for_session_id
(
  mmgsdi_session_id_type session_id,
  qmi_uim_imsi_type*     imsi_ptr
)
{
  mmgsdi_file_enum_type         file_name        = MMGSDI_MAX_FILE_ENUM;
  mmgsdi_app_enum_type          app_type         = MMGSDI_APP_NONE;
  mmgsdi_session_type_enum_type session_type     = MMGSDI_MAX_SESSION_TYPE_ENUM;
  mmgsdi_return_enum_type       mmgsdi_status    = MMGSDI_ERROR;
  qmi_error_type_v01            result           = QMI_ERR_NONE_V01;
  mmgsdi_static_data_type       aid              = { 0, };
  mmgsdi_data_type              mmgsdi_imsi_data = { 0, NULL};

  ASSERT(imsi_ptr);

  /* Get the session type in order to get the app_type for the IMSI file read */
  result = qmi_uim_util_session_id_to_session_type(&session_type, &aid, session_id);
  if(result != QMI_ERR_NONE_V01)
  {
    return result;
  }

  /* Use the session type to get the app_type for the IMSI file read */
  result = qmi_uim_util_session_type_to_card_type(&app_type, session_type, aid);
  if(result != QMI_ERR_NONE_V01)
  {
    return result;
  }

  switch(app_type)
  {
    case MMGSDI_APP_SIM:
      file_name = MMGSDI_GSM_IMSI;
      break;
    case MMGSDI_APP_RUIM:
      file_name = MMGSDI_CDMA_IMSI_M;
      break;
    case MMGSDI_APP_USIM:
      file_name = MMGSDI_USIM_IMSI;
      break;
    case MMGSDI_APP_CSIM:
      file_name = MMGSDI_CSIM_IMSI_M;
      break;
    default:
      return QMI_ERR_INTERNAL_V01;
  }

  mmgsdi_status = mmgsdi_session_read_cache_file_size(session_id, file_name,
                                                      &mmgsdi_imsi_data.data_len);

  imsi_ptr->len = (uint8)mmgsdi_imsi_data.data_len;

  if(mmgsdi_status != MMGSDI_SUCCESS || imsi_ptr->len > sizeof(imsi_ptr->imsi))
  {
    UIM_MSG_ERR_2("Error in IMSI length. mmgsdi_status:0x%x len:0x%x",
                  mmgsdi_status, imsi_ptr->len);
    return QMI_ERR_INTERNAL_V01;
  }

  mmgsdi_imsi_data.data_ptr = imsi_ptr->imsi;

  mmgsdi_status = mmgsdi_session_read_cache(session_id, file_name, mmgsdi_imsi_data);

  if(mmgsdi_status == MMGSDI_SUCCESS)
  {
    return QMI_ERR_NONE_V01;
  }

  return QMI_ERR_INTERNAL_V01;
} /* qmi_uim_util_read_imsi_for_session_id */


/*===========================================================================
  FUNCTION QMI_UIM_UTIL_ADD_CL_SP_TO_NON_PROV_SESSION_INDEX()

  DESCRIPTION
    Adds the passed in client state pointer to the non-prov. session
    index requested (in the global array).

    The cl_sp is not added if already present or if there are no
    slots available in the non-prov. session index.

  PARAMETERS
    index      : non-prov. session index in the global array
    client_sp  : client state pointer

  RETURN VALUE
    errval         : QMI client-specific error message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_type_v01 qmi_uim_util_add_cl_sp_to_non_prov_session_index
(
  uint16                       index,
  qmi_uimi_client_state_type * client_sp
)
{
  uint16                       cl_sp_index = 0;
  uint16                       empty_index = UIM_MAX_NON_PROV_CLIENTS;

  ASSERT(index < UIM_MAX_NON_PROV_SESSIONS);
  ASSERT(qmi_uim_global_ptr);

  if((client_sp == NULL) || 
     (qmi_uim_global_ptr->non_prov_session[index] == NULL))
  {
    return QMI_ERR_INVALID_ARG_V01;
  }

  /* Go through the cl_sp array, check if passed in cl_sp is already present,
     and also find any empty slot */
  for(cl_sp_index = 0;
      cl_sp_index < (uint16)UIM_MAX_NON_PROV_CLIENTS;
      cl_sp_index++)
  {
    if ((qmi_uim_global_ptr->non_prov_session[index]->client_info.opening_cl_sp[cl_sp_index] == NULL) &&
        (empty_index == UIM_MAX_NON_PROV_CLIENTS))
    {
      /* Empty slot found */
      empty_index = cl_sp_index;
    }

    /* If cl_sp already present, return */
    if (qmi_uim_global_ptr->non_prov_session[index]->client_info.opening_cl_sp[cl_sp_index] ==
         client_sp)
    {
      return QMI_ERR_NONE_V01;
    }
  }

  /* Index should be valid... otherwise it means that we didn't find
     the corresponding cl_sp and the array is also full */
  if (empty_index == UIM_MAX_NON_PROV_CLIENTS)
  {
    UIM_MSG_ERR_3("Cannot add cl_sp: 0x%x to non-prov. session index: 0x%x, session ID: 0x%x",
                  client_sp,
                  index,
                  qmi_uim_global_ptr->non_prov_session[index]->mmgsdi_session_id);
    return QMI_ERR_NO_MEMORY_V01;
  }

  /* Store the cl_sp */
  qmi_uim_global_ptr->non_prov_session[index]->client_info.opening_cl_sp[empty_index] =
    client_sp;
  /* Increment the Client count */
  qmi_uim_global_ptr->non_prov_session[index]->client_info.client_count++;

  return QMI_ERR_NONE_V01;
} /* qmi_uim_util_add_cl_sp_to_non_prov_session_index */


/*===========================================================================
  FUNCTION QMI_UIM_UTIL_UPDATE_NON_PROV_SESSION_CL_SP()

  DESCRIPTION
    Extracts the Session Info from the sesion_ptr, and if it is a
    non-prov. session already present in the global array, then the
    passed in cl_sp is associated with the session.

  PARAMETERS
  qmi_uim_session_information_type     : session_ptr
  qmi_uimi_client_state_type           : client_sp

  RETURN VALUE
    errval         : QMI client-specific error message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_uim_util_update_non_prov_session_cl_sp
(
  qmi_uim_session_information_type     * session_ptr,
  qmi_uimi_client_state_type           * client_sp
)
{
  uint16                                 i               = 0;

  ASSERT(session_ptr  && qmi_uim_global_ptr);

  if(client_sp == NULL)
  {
    return QMI_ERR_INVALID_ARG_V01;
  }

  /* Verify session type is non provisioning */
  if (session_ptr->session_type != MMGSDI_NON_PROV_SESSION_SLOT_1 &&
      session_ptr->session_type != MMGSDI_NON_PROV_SESSION_SLOT_2 &&
      session_ptr->session_type != MMGSDI_NON_PROV_SESSION_SLOT_3)
  {
    return QMI_ERR_NONE_V01;
  }

  /* Check if the requested non provisioning session is already available
     in the global array */
  for (i = 0; i < UIM_MAX_NON_PROV_SESSIONS; i++)
  {
    if (qmi_uim_global_ptr->non_prov_session[i] != NULL  &&
        qmi_uim_global_ptr->non_prov_session[i]->session_type == session_ptr->session_type &&
        qmi_uim_global_ptr->non_prov_session[i]->aid.data_len > 0 &&
        qmi_uim_global_ptr->non_prov_session[i]->aid.data_len == session_ptr->aid.data_len)
    {
      /* Add cl_sp to the non-prov. session */
      if (memcmp((void*)qmi_uim_global_ptr->non_prov_session[i]->aid.data_ptr,
                 (void*)session_ptr->aid.data_ptr,
                 session_ptr->aid.data_len) == 0)
      {
        return qmi_uim_util_add_cl_sp_to_non_prov_session_index(i, client_sp);
      }
    }
  }
  return QMI_ERR_NONE_V01;
} /* qmi_uim_util_update_non_prov_session_cl_sp */


/*===========================================================================
  FUNCTION QMI_UIMI_UTIL_POPULATE_RESP()

  DESCRIPTION
    Populates the response structure

  PARAMETERS  
    qmi_error_type_v01        errval[IN]
    qmi_response_type_v01   * resp[OUT]

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_util_populate_resp
(
  qmi_error_type_v01        errval,
  qmi_response_type_v01   * resp
)
{
  ASSERT(resp);
  resp->error = errval;
  resp->result = (errval != QMI_ERR_NONE_V01) ? QMI_RESULT_FAILURE_V01 : QMI_RESULT_SUCCESS_V01;
} /* qmi_uimi_util_populate_resp */


/*===========================================================================
  FUNCTION QMI_UIM_UTIL_POPULATE_READ_RESULT()

  DESCRIPTION
    Populate read data reust depend upon encryption status.

  PARAMETERS
  read_result_data[OUT]             : output read data buffer
  read_result_data_len[OUT]         : output read data len
  read_result_data_size[IN]         : outpt read data buffer size
  read_result_valid[OUT]            : read data is valid or not
  encryption_status[IN]             : encryption status type
  read_data[IN]                     : input read data
  cmd_id[IN]                        : cmd type
  session_type[IN]                  : session type 
  file_access[IN]                   : file access details
  encryption_used[OUT]              : encryption used in output read data or not

  RETURN VALUE
    qmi_error_type_v01

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_uim_util_populate_read_result
(
  uint8                                 *read_result_data_ptr,
  uint32                                *read_result_data_len_ptr,
  uint32                                 read_result_data_size,
  uint8                                 *read_result_valid_ptr,
  qmi_uim_requested_encrytion_type       encryption_status,
  mmgsdi_data_type                       read_data,
  uint16                                 cmd_id,
  mmgsdi_session_type_enum_type          session_type,
  mmgsdi_access_type                     file_access,
  boolean                               *encryption_used_ptr
)
{        
  mmgsdi_data_type             encrypted_read_data    = { 0, NULL };
  qmi_error_type_v01           errval                 = QMI_ERR_NONE_V01;
  boolean                      encryption             = FALSE;

  if(!read_result_data_ptr                                                || 
     !read_result_data_len_ptr                                            ||
     !read_result_valid_ptr                                               ||
     (!encryption_used_ptr && cmd_id == QMI_UIM_READ_TRANSPARENT_REQ_V01) ||
     (read_data.data_len > 0 && !read_data.data_ptr))
  {
    return QMI_ERR_INVALID_ARG_V01;
  }
  
  if(cmd_id == QMI_UIM_READ_TRANSPARENT_REQ_V01 && encryption_status == QMI_UIM_DATA_ONLY_ENCRYPTION)
  {
    *encryption_used_ptr = qmi_uim_encrypt_payload(read_data,
                                                   &encrypted_read_data);
  }
  else if(cmd_id == QMI_UIM_READ_TRANSPARENT_REQ_V01 && encryption_status == QMI_UIM_COMPLETE_ENCRYPTION)
  {
    errval = qmi_uim_complete_encrypt_read_rsp(cmd_id,
                                               session_type,
                                               file_access,
                                               read_data,
                                               &encrypted_read_data,
                                               &encryption);
    if(errval != QMI_ERR_NONE_V01 || *encryption_used_ptr)
    {
      return errval;
    }
  }
  qmi_uim_util_populate_opaque(read_result_valid_ptr,
                               read_result_data_ptr,
                               read_result_data_len_ptr,
                               read_result_data_size,
                               (encryption) ? encrypted_read_data.data_ptr : read_data.data_ptr,
                               (encryption) ? encrypted_read_data.data_len : read_data.data_len,
                               FALSE);
  if(encrypted_read_data.data_ptr)
  {
    uimqmi_free(encrypted_read_data.data_ptr);
  }
  if(encryption_used_ptr)
  {
    *encryption_used_ptr = encryption;
  }
  return  errval;
}/* qmi_uim_util_populate_read_result */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_CARD_RESULT()

  DESCRIPTION
    Populate card result.

  PARAMETERS
    uim_card_result_type_v01 *card_result[OUT]
    uint8*                   *card_result_valid[OUT]
    mmgsdi_sw_type            status_word[IN]

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_poplate_card_result
(
  uint8                    *card_result_valid,
  uim_card_result_type_v01 *card_result_ptr,
  mmgsdi_sw_type            status_word
)
{

  /* SW code is not present */
  if (status_word.present == FALSE)
  {
    return;
  }

  if(card_result_ptr)
  {
    card_result_ptr->sw1 = status_word.sw1;
    card_result_ptr->sw2 = status_word.sw2;
    if(card_result_valid)
    {
      *card_result_valid  = status_word.present;
    }
  }
} /* qmi_uim_poplate_card_result */


/*===========================================================================
  FUNCTION QMI_UIM_UTIL_ADD_AID()

  DESCRIPTION
    Utility function to add an application

  PARAMETERS
    aid_ptr  : pointer to AID + application type
    slot     : slot

  RETURN VALUE
    uint16: indicates the index of the application. The function
            return UIM_INVALID_APP_INDEX if the application could not
            be added

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
uint16 qmi_uim_util_add_aid
(
  const mmgsdi_aid_type*   aid_ptr,
  mmgsdi_slot_id_enum_type slot
)
{
  uint16 index = 0;

  ASSERT(aid_ptr);
  ASSERT(qmi_uim_global_ptr);

  /* Check slot */
  if (slot != MMGSDI_SLOT_1 && slot != MMGSDI_SLOT_2 && slot != MMGSDI_SLOT_3)
  {
    return UIM_INVALID_APP_INDEX;
  }

  /* Check if there is space */
  if (qmi_uim_global_ptr->card_state.num_apps >= QMI_UIM_MAX_APP_COUNT)
  {
    UIM_MSG_ERR_0("No space to add application");
    return UIM_INVALID_APP_INDEX;
  }

  index = qmi_uim_global_ptr->card_state.num_apps;

  /* Make sure the index is not already used */
  if (qmi_uim_global_ptr->card_state.application[index] != NULL)
  {
    UIM_MSG_ERR_1("Index 0x%x already in use", index);
    return UIM_INVALID_APP_INDEX;
  }

  qmi_uim_global_ptr->card_state.application[index] = (qmi_uim_app_info_type *)uimqmi_malloc(
                                                        sizeof(qmi_uim_app_info_type));
  if (qmi_uim_global_ptr->card_state.application[index] == NULL)
  {
    return UIM_INVALID_APP_INDEX;
  }

  memset(qmi_uim_global_ptr->card_state.application[index],
         0x00,
         sizeof(qmi_uim_app_info_type));

  /* Copy AID value */
  (void)memscpy(&qmi_uim_global_ptr->card_state.application[index]->aid,
                sizeof(mmgsdi_static_data_type),
                &aid_ptr->aid,
                sizeof(mmgsdi_static_data_type));

  /* Copy Label value */
    (void)memscpy(&qmi_uim_global_ptr->card_state.application[index]->label,
                  sizeof(mmgsdi_static_data_type),
                  &aid_ptr->label,
                  sizeof(mmgsdi_static_data_type));

  /* Copy app type and slot */
  qmi_uim_global_ptr->card_state.application[index]->app_type      = aid_ptr->app_type;
  qmi_uim_global_ptr->card_state.application[index]->slot          = slot;

  /* Initialize other states with generic values */
  qmi_uim_global_ptr->card_state.application[index]->app_state     = QMI_UIM_APP_STATE_DETECTED;
  qmi_uim_global_ptr->card_state.application[index]->perso_state   = QMI_UIM_PERSO_STATE_UNKNOWN;
  qmi_uim_global_ptr->card_state.application[index]->perso_feature = MMGSDI_MAX_PERSO_FEATURE_ENUM;

  qmi_uim_global_ptr->card_state.num_apps++;

  return index;
} /* qmi_uim_util_add_aid */


/*===========================================================================
  FUNCTION QMI_UIM_UTIL_FIND_AID()

  DESCRIPTION
    Utility function to locate the application with the specified AID

  PARAMETERS
    aid_ptr  : pointer to AID + application type
    slot     : slot

  RETURN VALUE
    uint16: indicates the index of the application. The function
            return UIM_INVALID_APP_INDEX if the application is not
            found

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
uint16 qmi_uim_util_find_aid
(
  const mmgsdi_aid_type *  aid_ptr,
  mmgsdi_slot_id_enum_type slot
)
{
  uint16 index;

  ASSERT(aid_ptr);
  ASSERT(qmi_uim_global_ptr);

  if (aid_ptr->aid.data_len > MMGSDI_MAX_AID_LEN)
  {
    return UIM_INVALID_APP_INDEX;
  }

  for (index = 0; index < qmi_uim_global_ptr->card_state.num_apps &&
                  index < QMI_UIM_MAX_APP_COUNT; index++)
  {
    if (qmi_uim_global_ptr->card_state.application[index] != NULL &&
        qmi_uim_global_ptr->card_state.application[index]->app_type == aid_ptr->app_type &&
        qmi_uim_global_ptr->card_state.application[index]->slot == slot &&
        qmi_uim_global_ptr->card_state.application[index]->aid.data_len == aid_ptr->aid.data_len)
    {
      if (aid_ptr->aid.data_len == 0)
      {
        return index;
      }
      if (memcmp( (void*)qmi_uim_global_ptr->card_state.application[index]->aid.data_ptr,
                  (void*)aid_ptr->aid.data_ptr,
                  int32touint32(aid_ptr->aid.data_len) ) == 0)
      {
        return index;
      }
    }
  }

  return UIM_INVALID_APP_INDEX;
} /* qmi_uim_util_find_aid */


/*===========================================================================
  FUNCTION QMI_UIM_UTIL_REMOVE_AID()

  DESCRIPTION
    Utility function to remove an application

  PARAMETERS
    aid_ptr  : pointer to AID + application type
    slot     : slot

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_uim_util_remove_aid
(
  const mmgsdi_aid_type*   aid_ptr,
  mmgsdi_slot_id_enum_type slot
)
{
  uint16 j     = 0;
  uint16 index = 0;

  ASSERT(aid_ptr);
  ASSERT(qmi_uim_global_ptr);

  index = qmi_uim_util_find_aid(aid_ptr, slot);

  if (index == UIM_INVALID_APP_INDEX ||
      index >= qmi_uim_global_ptr->card_state.num_apps ||
      qmi_uim_global_ptr->card_state.application[index] == NULL)
  {
    /* Application does not exist: immediately return */
    return;
  }

  /* Free info of the app which needs to be removed */
  uimqmi_free(qmi_uim_global_ptr->card_state.application[index]);
  qmi_uim_global_ptr->card_state.application[index] = NULL;

  /* Shift all applications in the table */
  for (j = index; j < qmi_uim_global_ptr->card_state.num_apps - 1 &&
                  j < QMI_UIM_MAX_APP_COUNT - 1; j++)
  {
    qmi_uim_global_ptr->card_state.application[j] =
      qmi_uim_global_ptr->card_state.application[j + 1];
    qmi_uim_global_ptr->card_state.application[j + 1] = NULL;
  }

  /* Decrease number of apps */
  qmi_uim_global_ptr->card_state.num_apps--;

  for (j = 0; j < QMI_UIM_MAX_PROV_SESSIONS; j++)
  {
    /* Update index of GW provisioning app */
    if (qmi_uim_global_ptr->card_state.index_gw[j] == index)
    {
      qmi_uim_global_ptr->card_state.index_gw[j] = UIM_INVALID_APP_INDEX;
    }
    else if (qmi_uim_global_ptr->card_state.index_gw[j] > index &&
             qmi_uim_global_ptr->card_state.index_gw[j] < QMI_UIM_MAX_APP_COUNT)
    {
      qmi_uim_global_ptr->card_state.index_gw[j]--;
    }
    else
    {
      /* Nothing to do */
    }

    /* Update index of 1X provisioning app */
    if (qmi_uim_global_ptr->card_state.index_1x[j] == index)
    {
      qmi_uim_global_ptr->card_state.index_1x[j] = UIM_INVALID_APP_INDEX;
    }
    else if (qmi_uim_global_ptr->card_state.index_1x[j] > index &&
             qmi_uim_global_ptr->card_state.index_1x[j] < QMI_UIM_MAX_APP_COUNT)
    {
      qmi_uim_global_ptr->card_state.index_1x[j]--;
    }
    else
    {
      /* Nothing to do */
    }
  }
} /* qmi_uim_util_remove_aid */


/*===========================================================================
  FUNCTION QMI_UIM_UTIL_REMOVE_ALL_AID_IN_SLOT()

  DESCRIPTION
    Utility function to remove all applications for a specific slot

  PARAMETERS
    slot     : slot

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_util_remove_all_aid_in_slot
(
  mmgsdi_slot_id_enum_type slot
)
{
  uint16 i  =  0;

  while(i < qmi_uim_global_ptr->card_state.num_apps && i < QMI_UIM_MAX_APP_COUNT)
  {
    if (qmi_uim_global_ptr->card_state.application[i] != NULL &&
        qmi_uim_global_ptr->card_state.application[i]->slot == slot)
    {
      mmgsdi_aid_type   aid_value = { MMGSDI_APP_NONE };

      aid_value.app_type = qmi_uim_global_ptr->card_state.application[i]->app_type;
      (void)memscpy(&aid_value.aid,
                    sizeof(mmgsdi_static_data_type),
                    &qmi_uim_global_ptr->card_state.application[i]->aid,
                    sizeof(mmgsdi_static_data_type));

      qmi_uim_util_remove_aid(&aid_value, slot);
    }
    else
    {
      i++;
    }
  }
} /* qmi_uim_util_remove_all_aid_in_slot */


/*===========================================================================
  FUNCTION QMI_UIM_UTIL_IS_PROVISIONING_APP_INDEX()

  DESCRIPTION
    Checks if the given index is associated with a provisioning application

  PARAMETERS
    index : index of application

  RETURN VALUE
    TRUE  : If the index is associated with prov app
    FALSE : If the index is not associated with prov app

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_util_is_provisioning_app_index
(
  uint16 index
)
{
  uint8   i      = 0;
  boolean result = FALSE;

  ASSERT(qmi_uim_global_ptr);

  for (i = 0; i < QMI_UIM_MAX_PROV_SESSIONS; i++)
  {
    if (index == qmi_uim_global_ptr->card_state.index_gw[i] ||
        index == qmi_uim_global_ptr->card_state.index_1x[i])
    {
      result = TRUE;
      break;
    }
  }

  return result;
} /* qmi_uim_util_is_provisioning_app_index */


/*===========================================================================
  FUNCTION QMI_UIM_UTIL_IS_REQUEST_REJECTED()

  DESCRIPTION
    If APDU security restrictions are enabled on the device, this function
    checks the incoming request to determine if it should be rejected. It
    will be rejected if the logical channel in request doesnt belongs to the
    requesting client.

  PARAMETERS
    session_type   : session type
    aid            : AID value
    cl_sp          : Client state pointer

  RETURN VALUE
    Boolean indicating if the request is to be rejected or not.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_util_is_request_rejected
(
  mmgsdi_session_type_enum_type   session_type,
  mmgsdi_static_data_type         aid,
  qmi_uimi_client_state_type    * cl_sp
)
{
  uint16                    i               = 0;
  uint16                    channel_index   = UIM_INVALID_APP_INDEX;
  mmgsdi_slot_id_enum_type  slot            = MMGSDI_SLOT_1;

  ASSERT(qmi_uim_global_ptr);

  if (qmi_uim_global_ptr->apdu_sec_restr_value == QMI_UIM_APDU_SEC_RESTRICTIONS_NONE)
  {
    return FALSE;
  }

  /* Validate session type & channel id length */
  if ((session_type != MMGSDI_NON_PROV_SESSION_SLOT_1) &&
      (session_type != MMGSDI_NON_PROV_SESSION_SLOT_2) &&
      (session_type != MMGSDI_NON_PROV_SESSION_SLOT_3))
  {
    return FALSE;
  }

  if (aid.data_len != QMI_UIM_TLV_CHANNEL_ID_LEN)
  {
    return FALSE;
  }

  UIM_MSG_HIGH_1("logical_channel: 0x%x", aid.data_ptr[0]);

  /* Check if the logical channel belongs to requesting client */
  switch(session_type)
  {
    case MMGSDI_NON_PROV_SESSION_SLOT_1:
      slot = MMGSDI_SLOT_1;
      break;
    case MMGSDI_NON_PROV_SESSION_SLOT_2:
      slot = MMGSDI_SLOT_2;
      break;
    case MMGSDI_NON_PROV_SESSION_SLOT_3:
      slot = MMGSDI_SLOT_3;
      break;
    default:
      return FALSE;
  }

  /* If any match is found, the request is OK to proceed */
  for (i = 0; i < UIM_MAX_APDU_CHANNEL_COUNT; i++)
  {
    if (qmi_uim_global_ptr->apdu_channel[i] &&
        qmi_uim_global_ptr->apdu_channel[i]->slot == slot &&
        qmi_uim_global_ptr->apdu_channel[i]->logical_channel == aid.data_ptr[0] &&
        qmi_uim_global_ptr->apdu_channel[i]->opening_cl_sp == cl_sp)
    {
      channel_index = i;
      break;
    }
  }
  if (channel_index == UIM_INVALID_APP_INDEX)
  {
    UIM_MSG_ERR_0("Request rejected due to security restrictions: logical channel doesnt belong to client");
    return TRUE;
  }

  return FALSE;
} /* qmi_uim_util_is_request_rejected */


/*===========================================================================
  FUNCTION QMI_UIM_util_GENERIC_RESP_ALLOC_POPULATE()

  DESCRIPTION
    This function allocate the memory for response structure corresponding 
    to request type. it also populate response and return void pointer.

  PARAMETERS
    errval    : error value
    cmd_type  : request type
    resp_len  : Output response len

  RETURN VALUE
    response pointer of void type. it will be null in fail case

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void* qmi_uim_util_generic_resp_alloc_populate
(
  qmi_error_type_v01                                errval,
  uint16                                            cmd_type,
  uint16                                           *resp_len
)
{
  void*   ind_resp_ptr = NULL;

  ASSERT(resp_len);

  switch(cmd_type)
  {
    case QMI_UIM_SET_SIM_PROFILE_REQ_V01:
    { 
      uim_set_sim_profile_resp_msg_v01 *set_profile_ptr  = (uim_set_sim_profile_resp_msg_v01*) uimqmi_malloc(sizeof(uim_set_sim_profile_resp_msg_v01));
      *resp_len = sizeof(uim_set_sim_profile_resp_msg_v01);
      ind_resp_ptr = set_profile_ptr;
      if(ind_resp_ptr)
      {
        qmi_uim_util_populate_resp(errval, &set_profile_ptr->resp);
      }
      break;
    }
    case QMI_UIM_EUICC_MEMORY_RESET_REQ_V01:
    { 
      uim_euicc_memory_reset_resp_msg_v01 *euicc_memory_reset_ptr  =(uim_euicc_memory_reset_resp_msg_v01*) uimqmi_malloc(sizeof(uim_euicc_memory_reset_resp_msg_v01));
      *resp_len = sizeof(uim_euicc_memory_reset_resp_msg_v01);
      ind_resp_ptr = euicc_memory_reset_ptr;
      if(ind_resp_ptr)
      {
        qmi_uim_util_populate_resp(errval, &euicc_memory_reset_ptr->resp);
      }
      break;
    }
    case QMI_UIM_DELETE_PROFILE_REQ_V01:
    { 
      uim_delete_profile_resp_msg_v01 *delete_profile_ptr  = (uim_delete_profile_resp_msg_v01*) uimqmi_malloc(sizeof(uim_delete_profile_resp_msg_v01));
      *resp_len = sizeof(uim_delete_profile_resp_msg_v01);
      ind_resp_ptr = delete_profile_ptr;
      if(ind_resp_ptr)
      {
        qmi_uim_util_populate_resp(errval, &delete_profile_ptr->resp);
      }
      break;
    }
    case QMI_UIM_ADD_PROFILE_REQ_V01:
    { 
      uim_add_profile_resp_msg_v01 *add_profile_ptr  =(uim_add_profile_resp_msg_v01*) uimqmi_malloc(sizeof(uim_add_profile_resp_msg_v01));
      *resp_len = sizeof(uim_add_profile_resp_msg_v01);
      ind_resp_ptr = add_profile_ptr;
      if(ind_resp_ptr)
      {
        qmi_uim_util_populate_resp(errval, &add_profile_ptr->resp);
      }
      break;
    }
    case QMI_UIM_UPDATE_PROFILE_NICKNAME_REQ_V01:
    { 
      uim_update_profile_nickname_resp_msg_v01 *update_nickname_ptr = (uim_update_profile_nickname_resp_msg_v01*) uimqmi_malloc(sizeof(uim_update_profile_nickname_req_msg_v01));
      *resp_len = sizeof(uim_update_profile_nickname_resp_msg_v01);
      ind_resp_ptr = update_nickname_ptr;
      if(ind_resp_ptr)
      {
        qmi_uim_util_populate_resp(errval, &update_nickname_ptr->resp);
      }
      break;
    }
    case QMI_UIM_SUBSCRIPTION_OK_REQ_V01:
    { 
      uim_subscription_ok_resp_msg_v01 *subscription_ok_ptr  =
        (uim_subscription_ok_resp_msg_v01*) uimqmi_malloc(sizeof(uim_subscription_ok_resp_msg_v01));
      *resp_len = sizeof(uim_subscription_ok_resp_msg_v01);
      ind_resp_ptr = subscription_ok_ptr;
      if(ind_resp_ptr)
      {
        qmi_uim_util_populate_resp(errval, &subscription_ok_ptr->resp);
      }
      break;
    }
    case QMI_UIM_POWER_DOWN_REQ_V01:
    { 
      uim_power_down_resp_msg_v01 *pdown_ptr  = (uim_power_down_resp_msg_v01*) uimqmi_malloc(sizeof(uim_power_down_resp_msg_v01 ));
      *resp_len = sizeof(uim_power_down_resp_msg_v01);
      ind_resp_ptr = pdown_ptr;
      if(ind_resp_ptr)
      {
        qmi_uim_util_populate_resp(errval, &pdown_ptr->resp);
      }
      break;
    }
    case QMI_UIM_SET_SERVICE_STATUS_REQ_V01:
    { 
      uim_set_service_status_resp_msg_v01 *set_service_status_ptr  = (uim_set_service_status_resp_msg_v01*) uimqmi_malloc(sizeof(uim_set_service_status_resp_msg_v01 ));
      *resp_len = sizeof(uim_set_service_status_resp_msg_v01);
      ind_resp_ptr = set_service_status_ptr;
      if(ind_resp_ptr)
      {
        qmi_uim_util_populate_resp(errval, &set_service_status_ptr->resp);
      }
      break;
    }
    case QMI_UIM_CHANGE_PROVISIONING_SESSION_REQ_V01:
    { 
      uim_change_provisioning_session_resp_msg_v01 *change_prov_session_ptr  = (uim_change_provisioning_session_resp_msg_v01*) uimqmi_malloc(sizeof(uim_change_provisioning_session_resp_msg_v01 ));
      *resp_len = sizeof(uim_change_provisioning_session_resp_msg_v01);
      ind_resp_ptr = change_prov_session_ptr;
      if(ind_resp_ptr)
      {
        qmi_uim_util_populate_resp(errval, &change_prov_session_ptr->resp);
      }
      break;
    }
    case QMI_UIM_SAP_CONNECTION_REQ_V01:
    { 
      uim_sap_connection_resp_msg_v01 *sap_connection_ptr  =
        (uim_sap_connection_resp_msg_v01*) uimqmi_malloc(sizeof(uim_sap_connection_resp_msg_v01));
      *resp_len = sizeof(uim_sap_connection_resp_msg_v01);
      ind_resp_ptr = sap_connection_ptr;
      if(ind_resp_ptr)
      {
        qmi_uim_util_populate_resp(errval, &sap_connection_ptr->resp);
      }
      break;
    }
    case QMI_UIM_SET_FILE_STATUS_REQ_V01:
    { 
      uim_set_file_status_resp_msg_v01 *set_file_status_ptr  =
        (uim_set_file_status_resp_msg_v01*) uimqmi_malloc(sizeof(uim_set_file_status_resp_msg_v01));
      *resp_len = sizeof(uim_set_file_status_resp_msg_v01);
      ind_resp_ptr = set_file_status_ptr;
      if(ind_resp_ptr)
      {
        qmi_uim_util_populate_resp(errval, &set_file_status_ptr->resp);
      }
      break;
    }
    default:
      UIM_MSG_HIGH_1("Unexpected response type 0x%x", cmd_type);
      break;
  }
  
  return ind_resp_ptr;
}/* qmi_uim_util_generic_resp_alloc_populate */


/*===========================================================================
  FUNCTION QMI_UIM_UTIL_GET_CLI_INDEX()

  DESCRIPTION
    This function checks if client already registered with 
    QMI UIM service, if yes then it return client index

  PARAMETERS
    cli_handle[in]: Client Handle
    cli_index[out]: Client Index

  RETURN VALUE
    TRUE:  if already registered
    FALSE: otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_util_get_cli_index
(
  qmi_client_handle             cli_handle,
  uint16                       *cli_index
)
{
  uint16    client_index = 0;

  if(qmi_uim_state_ptr == NULL)
  {
    return FALSE;
  }
  for(client_index = 0; client_index < (int)UIMQMI_SVC_MAX_CLIDS; client_index++)
  {
    if ((qmi_uim_state_ptr->client[client_index] != NULL) &&
        (qmi_uim_state_ptr->client[client_index]->client_handle == cli_handle))
    {
      if(cli_index)
      {
        *cli_index = client_index;
      }
      return TRUE;
    }
  }
  UIM_MSG_ERR_1("not able to find client index for client handle 0x%x", cli_handle);

  return FALSE;
} /* qmi_uim_util_get_cli_index */


/*===========================================================================
  FUNCTION QMI_UIM_UTIL_MAP_TO_MMGSDI_SESSION_TYPE()

  DESCRIPTION
    This function will map uim_session_type_enum_v01 to 
    mmgsdi_session_type_enum_type.

  PARAMETERS
    uim_session_type_enum_v01    :   uim_session_type,
    mmgsdi_session_type_enum_type:  *mmgsdi_session_ptr 

  RETURN VALUE
    qmi_error_type_v01

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_uim_util_map_to_mmgsdi_session_type
(
  uim_session_type_enum_v01             uim_session_type,
  mmgsdi_session_type_enum_type        *mmgsdi_session_ptr
)
{
/* Session type */
  switch (uim_session_type)
  {
  case UIM_SESSION_TYPE_PRIMARY_GW_V01:
    *mmgsdi_session_ptr = MMGSDI_GW_PROV_PRI_SESSION;
    break;
  case UIM_SESSION_TYPE_PRIMARY_1X_V01:
    *mmgsdi_session_ptr = MMGSDI_1X_PROV_PRI_SESSION;
    break;
  case UIM_SESSION_TYPE_SECONDARY_GW_V01:
    *mmgsdi_session_ptr = MMGSDI_GW_PROV_SEC_SESSION;  
    break;
  case UIM_SESSION_TYPE_SECONDARY_1X_V01:
    *mmgsdi_session_ptr = MMGSDI_1X_PROV_SEC_SESSION;
    break;
  case UIM_SESSION_TYPE_NONPROVISIONING_SLOT_1_V01:
    *mmgsdi_session_ptr = MMGSDI_NON_PROV_SESSION_SLOT_1;
    break;
  case UIM_SESSION_TYPE_NONPROVISIONING_SLOT_2_V01:
    *mmgsdi_session_ptr = MMGSDI_NON_PROV_SESSION_SLOT_2;
    break;
  case UIM_SESSION_TYPE_CARD_ON_SLOT_1_V01:
    *mmgsdi_session_ptr = MMGSDI_CARD_SESSION_SLOT_1;
    break;
  case UIM_SESSION_TYPE_CARD_ON_SLOT_2_V01:
    *mmgsdi_session_ptr = MMGSDI_CARD_SESSION_SLOT_2;
    break;
  case UIM_SESSION_TYPE_CHANNEL_ID_SLOT_1_V01:
    *mmgsdi_session_ptr = MMGSDI_NON_PROV_SESSION_SLOT_1;
     break;
  case UIM_SESSION_TYPE_CHANNEL_ID_SLOT_2_V01:
    *mmgsdi_session_ptr = MMGSDI_NON_PROV_SESSION_SLOT_2;
     break;
  case UIM_SESSION_TYPE_TERTIARY_GW_V01:
    *mmgsdi_session_ptr = MMGSDI_GW_PROV_TER_SESSION;
    break;
  case UIM_SESSION_TYPE_TERTIARY_1X_V01:
    *mmgsdi_session_ptr = MMGSDI_1X_PROV_TER_SESSION;
    break;
  case UIM_SESSION_TYPE_QUATERNARY_GW_V01:
  case UIM_SESSION_TYPE_QUATERNARY_1X_V01:
  case UIM_SESSION_TYPE_QUINARY_GW_V01:
  case UIM_SESSION_TYPE_QUINARY_1X_V01:
    return QMI_ERR_NOT_SUPPORTED_V01;
  case UIM_SESSION_TYPE_NONPROVISIONING_SLOT_3_V01:
    *mmgsdi_session_ptr = MMGSDI_NON_PROV_SESSION_SLOT_3;
    break;
  case UIM_SESSION_TYPE_NONPROVISIONING_SLOT_4_V01:
  case UIM_SESSION_TYPE_NONPROVISIONING_SLOT_5_V01:
    return QMI_ERR_NOT_SUPPORTED_V01;
  case UIM_SESSION_TYPE_CARD_ON_SLOT_3_V01:
    *mmgsdi_session_ptr = MMGSDI_CARD_SESSION_SLOT_3;
    break;
  case UIM_SESSION_TYPE_CARD_ON_SLOT_4_V01:
  case UIM_SESSION_TYPE_CARD_ON_SLOT_5_V01:
    return QMI_ERR_NOT_SUPPORTED_V01;
  case UIM_SESSION_TYPE_CHANNEL_ID_SLOT_3_V01:
    *mmgsdi_session_ptr = MMGSDI_NON_PROV_SESSION_SLOT_3;
    break;
  case UIM_SESSION_TYPE_CHANNEL_ID_SLOT_4_V01:
  case UIM_SESSION_TYPE_CHANNEL_ID_SLOT_5_V01:
    return QMI_ERR_NOT_SUPPORTED_V01;
  default:
    return QMI_ERR_INVALID_ARG_V01;
  }

  return QMI_ERR_NONE_V01;
}/* qmi_uim_util_map_to_mmgsdi_session_type */
  

/*===========================================================================
  FUNCTION QMI_UIM_REMOTE_MAP_CSI_TO_CSI_CB_ERR

  DESCRIPTION
    Function converts a CSI error to a CSI callback error type

  PARAMETERS
   qmi_csi_error

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
 qmi_csi_cb_error qmi_uim_util_map_csi_to_csi_cb_err (
  qmi_csi_error  csi_error
)
{
  switch (csi_error)
  {
    case QMI_CSI_NO_ERR:
      return QMI_CSI_CB_NO_ERR;

    case QMI_CSI_CONN_REFUSED:
      return QMI_CSI_CB_CONN_REFUSED;

    case QMI_CSI_INTERNAL_ERR:
      return QMI_CSI_CB_INTERNAL_ERR;

    case QMI_CSI_NO_MEM:
      return QMI_CSI_CB_NO_MEM;

    case QMI_CSI_CONN_BUSY:
    case QMI_CSI_INVALID_HANDLE:
    case QMI_CSI_INVALID_ARGS:
    case QMI_CSI_ENCODE_ERR:
    case QMI_CSI_DECODE_ERR:
    default:
      return QMI_CSI_CB_UNSUPPORTED_ERR;
  }
} /* qmi_uim_util_map_csi_to_csi_cb_err */


/*===========================================================================
FUNCTION QMI_UIM_COMPARE_QMI_CMD

DESCRIPTION
This function will be called by q_linear_delete() to check if particular cmd
is present in the queue or not.

PARAMETER
  item_ptr  [Input]:  ptr to queue item
  param_ptr [Input]:  qmi_uim_cmd_buf

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
int qmi_uim_util_compare_qmi_cmd
(
  void *item_ptr,
  void *param_ptr
)
{
  qmi_uim_compare_s       *param_cmd_buf_ptr    = NULL;
  qmi_uim_cmd_buf_s       *item_cmd_buf_ptr     = NULL;
  
  item_cmd_buf_ptr  = (qmi_uim_cmd_buf_s*)item_ptr;
  param_cmd_buf_ptr = (qmi_uim_compare_s*)param_ptr;

  if (!item_cmd_buf_ptr || !param_cmd_buf_ptr)
  {
    return 0;
  }

  if(param_cmd_buf_ptr->client_handle == item_cmd_buf_ptr->client_handle &&
     param_cmd_buf_ptr->req_handle == item_cmd_buf_ptr->req_handle)
  {
    return 1;
  }
  return 0;
} /* qmi_uim_util_compare_qmi_cmd */


/*===========================================================================
FUNCTION QMI_UIM_UTIL_REMOVE_QMI_CMD

DESCRIPTION
  This function will remove the perticular cmd from the 
  client cmd q and free it, which match with provided
  req_handle and client_handle.
  
PARAMETER
  q_ptr  [IN]        :     ptr to queue
  qmi_req_handle[IN] :     req_handle
  uint32[IN]         :     client_handle

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None

SEE ALSO
  None

===========================================================================*/
void qmi_uim_util_remove_qmi_cmd
(
  qmi_req_handle      req_handle,
  qmi_client_handle   client_handle,
  q_type             *q_ptr
)
{
  qmi_uim_cmd_buf_s *item_ptr  = NULL;
  qmi_uim_compare_s  compare_s = {0,};
  
  if(!q_ptr)
  {
    return;
  }

  compare_s.client_handle = client_handle;
  compare_s.req_handle    = req_handle;

  item_ptr = (qmi_uim_cmd_buf_s*)q_linear_search(q_ptr, qmi_uim_util_compare_qmi_cmd, &compare_s);
  if(item_ptr)
  {
    if(!q_delete_ext(q_ptr, &item_ptr->link_next))
    {
      UIM_MSG_ERR_0("unable to delete from client q");
    }
    uimqmi_free(item_ptr);
  }
} /* qmi_uim_util_remove_qmi_cmd */
