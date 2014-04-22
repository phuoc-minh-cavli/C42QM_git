/*==============================================================================

                         Q M I _ U I M _ S A P . C

DESCRIPTION
  The Data Services Qualcomm MSM Interface Device Management Services source
  file.

Copyright (c) 2016, 2018 - 2019 QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary
==============================================================================*/
/*==============================================================================

                      EDIT HISTORY FOR FILE

$Header: //components/rel/uim.mpss/3.3.1/uimqmi/src/qmi_uim_sap.c#1 $

when        who    what, where, why
--------    ---    -------------------------------------------------------------
01/22/19    tq     Masked unsupported event registration 
11/13/18    tq     Migration of QMI UIM service to QCCI-QCSI
10/29/18    tq     Deprecated MMGSDI_TERMINAL_PROFILE_DL_EVT and MMGSDI_CARD_REMOVED_EVT 
10/12/18    bcho   RCINIT Optimization
06/22/16    sp     Initial Revision
==============================================================================*/

/*==============================================================================

                          INCLUDE FILES FOR MODULE

==============================================================================*/
#include "uim_variation.h"
#include "uim_msg.h"
#include "amssassert.h"
#include "qmi_uim_cat_common.h"
#include "qmi_uim.h"
#include "mmgsdisessionlib_v.h"
#include "fs_public.h"

#include "qmi_uim_sap.h"
#include "qmi_uim_parsing.h"
#include "qmi_uim_util.h"

/*==============================================================================

                             EXTERNAL FUNCTIONS

==============================================================================*/

/*===========================================================================
  FUNCTION QMI_UIM_IS_SAP_REQUEST_REJECTED()

  DESCRIPTION
    If APDU security restrictions are enabled on the device, this function
    parses the incoming SAP APDU request and checks if it is a SELECT command
    using the DF name (P1 = 0x04) and specifying AID of a non-telecom
    application. Access to such non-3GPP/2 RIDs are rejected.

  PARAMETERS
    apdu_data  : Incoming SAP APDU request

  RETURN VALUE
    Boolean indicating if the request is to be rejected or not.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_uim_is_sap_request_rejected
(
  mmgsdi_send_apdu_data_type    apdu_data
)
{
  const uint8 rid_value_3gpp[]  =  { 0xA0, 0x00, 0x00, 0x00, 0x87 };
  const uint8 rid_value_3gpp2[] =  { 0xA0, 0x00, 0x00, 0x03, 0x43 };

  ASSERT(qmi_uim_global_ptr);

  /* If no APDU security restrictions, nothing to check */
  if (qmi_uim_global_ptr->apdu_sec_restr_value == QMI_UIM_APDU_SEC_RESTRICTIONS_NONE)
  {
    return FALSE;
  }

  /* Check these bytes:
     Byte 2: INS
     Byte 3: P1
     Bytes 6 through 10: RID (first 5 bytes of AID), if needed
  */
  if ((apdu_data.data_ptr != NULL) &&
      (apdu_data.data_len >= 3))
  {
    if ((apdu_data.data_ptr[1] == QMI_UIM_APDU_INS_BYTE_SELECT_FILE) &&
        (apdu_data.data_ptr[2] == QMI_UIM_APDU_P1_BYTE_SELECT_DF_NAME))
    {
      /* Reject if it is an attempt to provide a truncated AID < 5 bytes */
      if (apdu_data.data_len < 10)
      {
        UIM_MSG_ERR_1("ADPU rejected due to security restrictions: data_len: 0x%x",
                      apdu_data.data_len);
        return TRUE;
      }
      /* Also reject if an RID other than 3GPP or 3GPP2 is provided */
      if ((memcmp(&apdu_data.data_ptr[5], rid_value_3gpp, sizeof(rid_value_3gpp)) != 0) &&
          (memcmp(&apdu_data.data_ptr[5], rid_value_3gpp2, sizeof(rid_value_3gpp2)) != 0))
      {
        UIM_MSG_ERR_0("ADPU rejected due to security restrictions: restricted AID");
        return TRUE;
      }
    }
  }

  return FALSE;
} /* qmi_uim_is_sap_request_rejected */


/*===========================================================================
  FUNCTION QMI_UIM_SAP_INFO

  DESCRIPTION
    map mgsdi sap state and slot to uim sap state and slot

  PARAMETERS
    is_enabled        : flag to indicate if SAP is enabled
    sap_state         : SAP state
    slot_idx          : slot subscript,
    uim_sap_state_ptr : output uim sap state
    uim_slot_ptr      : output uim slot
  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_uim_sap_info
(
  boolean                         is_enabled,
  mmgsdi_sap_state_enum_type      sap_state,
  mmgsdi_slot_id_enum_type        slot,  
  uim_sap_status_enum_v01        *uim_sap_state_ptr,
  uim_slot_enum_v01              *uim_slot_ptr
)
{
  ASSERT(uim_sap_state_ptr || uim_slot_ptr);

  if(uim_sap_state_ptr)
  {
    /* Update sap state */
    if (!is_enabled)
    {
      *uim_sap_state_ptr = UIM_SAP_STATE_NOT_ENABLED_V01;
    }
    else
    {
      switch(sap_state)
      {
        case MMGSDI_SAP_CONNECTING:
          *uim_sap_state_ptr = UIM_SAP_STATE_CONNECTING_V01;
          break;
        case MMGSDI_SAP_CONNECTED:
          *uim_sap_state_ptr = UIM_SAP_STATE_CONNECTED_SUCCESSFULLY_V01;
          break;
        case MMGSDI_SAP_CONNECT_FAIL:
        case MMGSDI_SAP_DISCONNECT_FAIL:
          *uim_sap_state_ptr = UIM_SAP_STATE_CONNECTION_ERROR_V01;
          break;
        case MMGSDI_SAP_DISCONNECTING:
          *uim_sap_state_ptr = UIM_SAP_STATE_DISCONNECTING_V01;
          break;
        case MMGSDI_SAP_DISCONNECTED:
          *uim_sap_state_ptr = UIM_SAP_STATE_DISCONNECTED_SUCCESSFULLY_V01;
          break;
        default:
          return FALSE;
       }
    }
  }

  if(uim_slot_ptr)
  {
    /* Update slot state */
    switch(slot)
    {
      case MMGSDI_SLOT_1:
        *uim_slot_ptr = UIM_SLOT_1_V01;
        break;
      case MMGSDI_SLOT_2:
        *uim_slot_ptr = UIM_SLOT_2_V01;
        break;
      case MMGSDI_SLOT_3:
        *uim_slot_ptr = UIM_SLOT_3_V01;
        break;
      default:
        return FALSE;
    }
  }
  return TRUE;
} /* qmi_uim_sap_info */


/*===========================================================================
  FUNCTION QMI_UIMI_SAP_CONNECTION()

  DESCRIPTION
    Send connection related SAP commands to the card

  PARAMETERS
    cl_sp            : client state pointer
    req_handle       : req handle
    req_c_struct     : IDL client structure
    req_c_struct_len : structure len

  RETURN VALUE
    ptr to response or NULL if none.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_error qmi_uimi_sap_connection
(
  qmi_uimi_client_state_type  * cl_sp,
  qmi_req_handle                req_handle,
  void                        * req_c_struct,
  int                           req_c_struct_len
)
{
  uim_sap_connection_req_msg_v01  *sap_connection_req_ptr  = NULL;
  uim_sap_connection_resp_msg_v01 *sap_connection_resp_ptr = NULL;
  uint16                           client_index            = 0;
  qmi_uim_cmd_buf_s               *cmd_buf_p               = NULL;
  qmi_csi_error                    csi_errval              = QMI_CSI_NO_ERR;
  qmi_error_type_v01               errval                  = QMI_ERR_NONE_V01;
  mmgsdi_return_enum_type          mmgsdi_status           = MMGSDI_SUCCESS;
  uint8                            slot_idx                = 0;
  uim_sap_connect_op_enum_v01      op_type                 = UIM_SAP_OP_CHECK_STATUS_V01;
  qmi_uimi_userdata_type*          cb_userdata_ptr         = NULL;
  mmgsdi_disconnect_mode_enum_type mode                    = MMGSDIBT_DISCONNECT_IMMED;
  mmgsdi_slot_id_enum_type         slot                    = MMGSDI_MAX_SLOT_ID_ENUM;
  uim_sap_status_enum_v01          sap_state               = UIM_SAP_STATUS_ENUM_MAX_ENUM_VAL_V01;
  uint8                            sap_state_valid         = 0;
  qmi_uim_nv_status_enum_type     sap_sec_restr_supported  = QMI_UIM_NV_STATUS_NOT_SET;

  ASSERT(cl_sp && req_c_struct && req_c_struct_len > 0 && qmi_uim_state_ptr);

  if(!qmi_uim_util_get_cli_index(cl_sp->client_handle, &client_index))
  {
    return QMI_CSI_INVALID_HANDLE;
  }

  cmd_buf_p = qmi_uim_util_get_cmd_buf_ptr(req_handle, cl_sp->client_handle);

  ASSERT(cmd_buf_p);

  sap_connection_req_ptr = (uim_sap_connection_req_msg_v01 *)req_c_struct;
  cmd_buf_p->req_handle = req_handle;

  if(sap_connection_req_ptr->sap_connect.connect  > UIM_SAP_OP_CHECK_STATUS_V01)
  {
    errval = QMI_ERR_INVALID_ARG_V01;
    goto send_result;
  }

  /* Derive Slot id and return */
  errval = qmi_uim_map_and_validate_slot(&slot, sap_connection_req_ptr->sap_connect.slot);

  if (errval != QMI_ERR_NONE_V01 || 
      uimqmi_mmgsdi_slot_to_slot_index(slot,
                                       &slot_idx) != QMI_ERR_NONE_V01)
  {
    errval = QMI_ERR_INVALID_ARG_V01;
    goto send_result;
  }

  if(slot_idx >= QMI_UIM_MAX_CARD_COUNT)
  {
    errval = QMI_ERR_INVALID_ARG_V01;
    goto send_result;
  }

  op_type = sap_connection_req_ptr->sap_connect.connect;

  qmi_uim_read_efs_configuration(UIM_COMMON_EFS_UIMQMI_QMI_UIM_CONFIG_SAP_SEC_RESTR,
                                 (void *)&sap_sec_restr_supported);

  /* Check to see if SAP connect is blocked/disabled based on the NV */
  if (sap_sec_restr_supported == QMI_UIM_NV_STATUS_TRUE &&
      op_type == UIM_SAP_OP_CONNECT_V01)
  {
    errval = QMI_ERR_ACCESS_DENIED_V01;
    UIM_MSG_ERR_0("SAP request rejected due to security restrictions");
    goto send_result;
  }

  /* Create userdata */
  cb_userdata_ptr = qmi_uim_util_get_cb_userdata(QMI_UIM_SAP_CONNECTION_REQ_V01,
                                                 cmd_buf_p,
                                                 NULL,
                                                 cl_sp->client_handle);
  if (cb_userdata_ptr == NULL)
  {
    errval = QMI_ERR_NO_MEMORY;
    goto send_result;
  }

  /* Perform respective connection operation */
  switch (op_type)
  {
    case UIM_SAP_OP_DISCONNECT_V01:
      /* Call MMGSDI function to disconnect */
      if (qmi_uim_global_ptr->sap_info[slot_idx].is_enabled)
      {
        /* Connection operation */
        if (sap_connection_req_ptr->disconnect_mode_valid)
        {
          if(sap_connection_req_ptr->disconnect_mode == UIM_SAP_DISCONNECT_IMMEDIATE_V01)
          {
            mode = MMGSDIBT_DISCONNECT_IMMED;
          }
          else if(sap_connection_req_ptr->disconnect_mode == UIM_SAP_DISCONNECT_GRACEFULL_V01)
          {
            mode = MMGSDIBT_DISCONNECT_GRACE;
          }
          else
          {
            errval = QMI_ERR_INVALID_ARG_V01;
            mmgsdi_status = MMGSDI_ERROR;
            break;
          }
        }
        mmgsdi_status = mmgsdi_sap_disconnect(
                              qmi_uim_global_ptr->mmgsdi_client_id,
                              slot,
                              MMGSDIBT_SERVER,
                              mode,
                              qmi_uimi_mmgsdi_callback,
                              (mmgsdi_client_data_type)cb_userdata_ptr);
        /* Convert MMGSDI status in QMI code */
        errval = qmi_uim_util_convert_mmgsdi_status_to_errval(mmgsdi_status);

        /* Send SAP indication with disconnected successfully to the client if
           the queuing of the SAP disconnect is successful. We do this as it
           is not expected that the SAP disconnection would fail. This allows
           to better handle the card error event and disconnection CNF from MMGSDI */
        if(errval == QMI_ERR_NONE_V01)
        {
          /* Simulate SAP state as DISCONNECTED and send the SAP indication
             to all the SAP registered clients */
          qmi_uim_global_ptr->sap_info[slot_idx].sap_state = MMGSDI_SAP_DISCONNECTED;
          qmi_uim_send_sap_status_indication_to_all(slot);

          /* Set is_enabled flag to FALSE after sending SAP disconnected
             indication to clients */
          qmi_uim_global_ptr->sap_info[slot_idx].is_enabled = FALSE;
        }
      }
      else
      {
        errval = QMI_ERR_NO_EFFECT;
        mmgsdi_status = MMGSDI_ERROR;
      }
      break;

    case UIM_SAP_OP_CONNECT_V01:
      /* Call MMGSDI function to connect */
      if (!qmi_uim_global_ptr->sap_info[slot_idx].is_enabled)
      {
        mmgsdi_sap_conn_condition_enum_type conn_condition = MMGSDI_SAP_CONN_COND_BLOCK_VOICE_OR_DATA;
        if(sap_connection_req_ptr->connection_condition_valid && 
           (sap_connection_req_ptr->connection_condition > UIM_SAP_CONNECTION_COND_BLOCK_NONE_V01 ||
            sap_connection_req_ptr->connection_condition < UIM_SAP_CONNECTION_COND_BLOCK_VOICE_OR_DATA_V01))
        {
          errval = QMI_ERR_INVALID_ARG_V01;
          mmgsdi_status = MMGSDI_ERROR;
          break;
        }
        /* We use the condition from connection condtion TLV, if present.
           If TLV is not present, we default to either voice or data */
        conn_condition =
          sap_connection_req_ptr->connection_condition_valid ? sap_connection_req_ptr->connection_condition :
                                MMGSDI_SAP_CONN_COND_BLOCK_VOICE_OR_DATA;

        mmgsdi_status = mmgsdi_sap_connect_ext(
                            qmi_uim_global_ptr->mmgsdi_client_id,
                            slot,
                            MMGSDIBT_SERVER,
                            conn_condition,
                            qmi_uimi_mmgsdi_callback,
                            (mmgsdi_client_data_type)cb_userdata_ptr);
        /* Convert MMGSDI status in QMI code */
        errval = qmi_uim_util_convert_mmgsdi_status_to_errval(mmgsdi_status);

        /* Update userdata with the intermediate get response info */
        if (sap_connection_req_ptr->intermediate_get_response_valid && (errval == QMI_ERR_NONE_V01))
        {
          cl_sp->uim_info.sap_intermediate_get_resp = 
            (boolean)sap_connection_req_ptr->intermediate_get_response;
        }
        else
        {
          cl_sp->uim_info.sap_intermediate_get_resp = FALSE;
        }
      }
      else
      {
        errval = QMI_ERR_NO_EFFECT_V01;
        mmgsdi_status = MMGSDI_ERROR;
      }
      break;

    case UIM_SAP_OP_CHECK_STATUS_V01:
      /* Send a synchronous reponse for check status */
      sap_connection_resp_ptr->sap_state_valid = 1;
      if(!qmi_uim_sap_info(
                      qmi_uim_global_ptr->sap_info[slot_idx].is_enabled,
                      qmi_uim_global_ptr->sap_info[slot_idx].sap_state,
                      slot,
                      &sap_state,
                      NULL))
      {
        sap_state_valid = 1;
        errval = QMI_ERR_INTERNAL_V01;
      }
      /* Fake setting just to make sure cb_userdata_ptr is cleaned */
      mmgsdi_status = MMGSDI_ERROR;
      break;

    default:
      /* At this point, this shouldnt happen */
      mmgsdi_status = MMGSDI_ERROR;
      errval = QMI_ERR_INVALID_ARG;
      break;
  }

  /* Free userdata in case of error */
  QMI_UIM_UTIL_FREE_USERDATA_IF_ERROR(mmgsdi_status, cb_userdata_ptr);

send_result:
  if(errval == QMI_ERR_NONE_V01 && op_type != UIM_SAP_OP_CHECK_STATUS_V01)
  {
    cmd_buf_p->resp_pending = TRUE;
    return QMI_CSI_NO_ERR;
  }

  sap_connection_resp_ptr =   uimqmi_malloc(sizeof(uim_sap_connection_resp_msg_v01));
  if(!sap_connection_resp_ptr)
  {
    return QMI_CSI_NO_MEM;
  }
  
  qmi_uim_util_populate_resp(errval, &sap_connection_resp_ptr->resp);
  sap_connection_resp_ptr->sap_state_valid = sap_state_valid;
  sap_connection_resp_ptr->sap_state = sap_state;

  csi_errval = qmi_uim_send_immidiate_resp(cmd_buf_p, 
                                           sap_connection_resp_ptr,
                                           cmd_buf_p->cmd_type,
                                           sizeof(uim_sap_connection_resp_msg_v01),
                                           cl_sp->client_handle);
  uimqmi_free(sap_connection_resp_ptr);
 
  return csi_errval;
} /* qmi_uimi_sap_connection */


/*===========================================================================
  FUNCTION QMI_UIMI_SAP_REQUEST()

  DESCRIPTION
    Send various SAP requests to the card

  PARAMETERS
    cl_sp            : client state pointer
    req_handle       : req handle
    req_c_struct     : IDL client structure
    req_c_struct_len : structure len

  RETURN VALUE
    ptr to response or NULL if none.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_error qmi_uimi_sap_request
(
  qmi_uimi_client_state_type  * cl_sp,
  qmi_req_handle                req_handle,
  void                        * req_c_struct,
  int                           req_c_struct_len
)
{
  qmi_error_type_v01              errval               = QMI_ERR_NONE_V01;
  mmgsdi_return_enum_type         mmgsdi_status        = MMGSDI_SUCCESS;
  uint8                           slot_idx             = 0;
  int32                           channel_id           = 0;
  qmi_uimi_userdata_type*         cb_userdata_ptr      = NULL;
  uim_sap_request_req_msg_v01    *sap_request_req_ptr  = NULL;
  uim_sap_request_resp_msg_v01   *sap_request_resp_ptr = NULL;
  uint16                          client_index         = 0;
  qmi_uim_cmd_buf_s              *cmd_buf_p            = NULL;
  qmi_csi_error                   csi_errval           = QMI_CSI_NO_ERR;
  mmgsdi_send_apdu_data_type      apdu_data            = {0, NULL};
  mmgsdi_slot_id_enum_type        slot                 = MMGSDI_MAX_SLOT_ID_ENUM;

  ASSERT(cl_sp && req_c_struct && req_c_struct_len > 0 && qmi_uim_state_ptr);

  if(!qmi_uim_util_get_cli_index(cl_sp->client_handle, &client_index))
  {
    return QMI_CSI_INVALID_HANDLE;
  }

  cmd_buf_p = qmi_uim_util_get_cmd_buf_ptr(req_handle, cl_sp->client_handle);

  ASSERT(cmd_buf_p);

  sap_request_req_ptr = (uim_sap_request_req_msg_v01 *)req_c_struct;
  cmd_buf_p->req_handle = req_handle;  

  /* Derive Slot id and return */
  errval = qmi_uim_map_and_validate_slot(&slot, sap_request_req_ptr->sap_request.slot);

  if (errval != QMI_ERR_NONE_V01 || 
      uimqmi_mmgsdi_slot_to_slot_index(slot,
                                       &slot_idx) != QMI_ERR_NONE_V01)
  {
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }

  if(slot_idx >= QMI_UIM_MAX_CARD_COUNT                   ||
     slot_idx >= qmi_uim_global_ptr->card_state.num_slots || 
     sap_request_req_ptr->sap_request.sap_request > UIM_SAP_REQUEST_CARD_READER_STATUS_V01)
  {
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }

  /* Continue only if SAP connection is up */
  if (!qmi_uim_global_ptr->sap_info[slot_idx].is_enabled)
  {
    errval = QMI_ERR_INTERNAL_V01;
    goto send_result;
  }

  /* Error if no send_apdu TLV present for send APDU request */
  if((sap_request_req_ptr->sap_request.sap_request == UIM_SAP_REQUEST_SEND_APDU_V01) &&
     (!sap_request_req_ptr->apdu_valid))
  {
    UIM_MSG_ERR_0("send APDU request requires send_apdu TLV");
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  /* Create userdata */
  cb_userdata_ptr = qmi_uim_util_get_cb_userdata(QMI_UIM_SAP_REQUEST_REQ_V01,
                                                 cmd_buf_p,
                                                 NULL,
                                                 cl_sp->client_handle);
  if (cb_userdata_ptr == NULL)
  {
    errval = QMI_ERR_NO_MEMORY;
    goto send_result;
  }

  /* Perform respective request operation */
  switch (sap_request_req_ptr->sap_request.sap_request)
  {
    case UIM_SAP_REQUEST_GET_ATR_V01:
      mmgsdi_status = mmgsdi_sap_get_atr(
                        qmi_uim_global_ptr->mmgsdi_client_id,
                        slot,
                        qmi_uimi_mmgsdi_callback,
                        (mmgsdi_client_data_type)cb_userdata_ptr);
      break;
    case UIM_SAP_REQUEST_SEND_APDU_V01:
      if(sap_request_req_ptr->apdu_len > 0)
      {
        apdu_data.data_len = sap_request_req_ptr->apdu_len;
        apdu_data.data_ptr = sap_request_req_ptr->apdu;
      }
      /* Check if SAP request is restricted */
      if (qmi_uim_is_sap_request_rejected(apdu_data))
      {
        mmgsdi_status = MMGSDI_ACCESS_DENIED;
      }
      else
      {
        mmgsdi_status = mmgsdi_sap_send_apdu(
                          qmi_uim_global_ptr->mmgsdi_client_id,
                          slot,
                          channel_id,
                          apdu_data,
                          qmi_uimi_mmgsdi_callback,
                          (mmgsdi_client_data_type)cb_userdata_ptr);

        /* Also save the INS byte if needed in userdata */
        if ((apdu_data.data_ptr) &&
            (apdu_data.data_len >= 2))
        {
          cb_userdata_ptr->data.sap_request.ins_value =
              apdu_data.data_ptr[1];
        }
      }
      break;
    case UIM_SAP_REQUEST_POWER_SIM_OFF_V01:
      mmgsdi_status = mmgsdi_sap_power_off(
                        qmi_uim_global_ptr->mmgsdi_client_id,
                        slot,
                        MMGSDIBT_SERVER,
                        qmi_uimi_mmgsdi_callback,
                        (mmgsdi_client_data_type)cb_userdata_ptr);
      break;
    case UIM_SAP_REQUEST_POWER_SIM_ON_V01:
      mmgsdi_status = mmgsdi_sap_power_on(
                        qmi_uim_global_ptr->mmgsdi_client_id,
                        slot,
                        MMGSDIBT_SERVER,
                        qmi_uimi_mmgsdi_callback,
                        (mmgsdi_client_data_type)cb_userdata_ptr);
      break;
    case UIM_SAP_REQUEST_RESET_SIM_V01:
      mmgsdi_status = mmgsdi_sap_reset(
                        qmi_uim_global_ptr->mmgsdi_client_id,
                        slot,
                        MMGSDIBT_SERVER,
                        qmi_uimi_mmgsdi_callback,
                        (mmgsdi_client_data_type)cb_userdata_ptr);
      break;
    case UIM_SAP_REQUEST_CARD_READER_STATUS_V01:
      mmgsdi_status = mmgsdi_sap_card_reader_status(
                        qmi_uim_global_ptr->mmgsdi_client_id,
                        slot,
                        MMGSDIBT_SERVER,
                        qmi_uimi_mmgsdi_callback,
                        (mmgsdi_client_data_type)cb_userdata_ptr);
      break;
    default:
      /* At this point, this shouldnt happen */
      mmgsdi_status = MMGSDI_ERROR;
      break;
  }

  /* Convert MMGSDI status in QMI code */
  errval = qmi_uim_util_convert_mmgsdi_status_to_errval(mmgsdi_status);

  /* Free userdata in case of error */
  QMI_UIM_UTIL_FREE_USERDATA_IF_ERROR(mmgsdi_status, cb_userdata_ptr);

  if(errval == QMI_ERR_NONE_V01)
  {
    cmd_buf_p->resp_pending = TRUE;
    return QMI_CSI_NO_ERR;
  }

send_result:
  sap_request_resp_ptr =   uimqmi_malloc(sizeof(uim_sap_request_resp_msg_v01));
  if(!sap_request_resp_ptr)
  {
    return QMI_CSI_NO_MEM;
  }
  
  qmi_uim_util_populate_resp(errval, &sap_request_resp_ptr->resp);

  csi_errval = qmi_uim_send_immidiate_resp(cmd_buf_p, 
                                           sap_request_resp_ptr,
                                           cmd_buf_p->cmd_type,
                                           sizeof(uim_sap_request_resp_msg_v01),
                                           cl_sp->client_handle);
  uimqmi_free(sap_request_resp_ptr);
 
  return csi_errval;
} /* qmi_uimi_sap_request */


/*===========================================================================
  FUNCTION QMI_UIM_SEND_SAP_STATUS_INDICATION()

  DESCRIPTION
    This function sends a SAP status indication to a specific client

  PARAMETERS
    cl_handle   : Client handle
    slot        : Card slot

  RETURN VALUE
    None.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_uim_send_sap_status_indication
(
  qmi_client_handle             cl_handle,
  mmgsdi_slot_id_enum_type      slot
)
{
  uim_sap_connection_ind_msg_v01 *sap_indid_ptr = NULL;
  uint8                           slot_idx      = 0;
  uint16                          cli_index     = 0; 
  
  ASSERT(qmi_uim_util_get_cli_index(cl_handle, &cli_index) && qmi_uim_global_ptr);

  if (uimqmi_mmgsdi_slot_to_slot_index(slot,
                                       &slot_idx) != QMI_ERR_NONE_V01)
  {
    return;
  }

  if(slot_idx >= QMI_UIM_MAX_CARD_COUNT ||
     slot_idx >= qmi_uim_global_ptr->card_state.num_slots)
  {
    UIM_MSG_ERR_1("Invalid slot : 0x%x", slot);
    return;
  }

  sap_indid_ptr = uimqmi_malloc(sizeof(uim_sap_connection_ind_msg_v01));
  if(sap_indid_ptr == NULL)
  {
    return;
  }
  
  /* Insert sap state & slot item */
  if (FALSE == qmi_uim_sap_info(
                   qmi_uim_global_ptr->sap_info[slot_idx].is_enabled,
                   qmi_uim_global_ptr->sap_info[slot_idx].sap_state,
                   slot,
                   &sap_indid_ptr->sap_connection_event.sap_state,
                   &sap_indid_ptr->sap_connection_event.slot))
  {
    /* Ignore error... there is nothing we can do about it.
       The event will not be delivered to the client */
    uimqmi_free(sap_indid_ptr);
    return;
  }  
  sap_indid_ptr->sap_connection_event_valid = 1;

   /* Send indication to the client */
  (void)qmi_uim_send_response_ind(NULL,
                                  sap_indid_ptr,
                                  (uint16)QMI_UIM_SAP_CONNECTION_IND_V01,
                                  sizeof(uim_sap_connection_ind_msg_v01),
                                  TRUE,
                                  cl_handle);
  uimqmi_free(sap_indid_ptr);
} /* qmi_uim_send_sap_status_indication */


/*===========================================================================
  FUNCTION QMI_UIM_SEND_SAP_STATUS_INDICATION_TO_ALL()

  DESCRIPTION
    This function sends a SAP status indication to all QMI_UIM clients

  PARAMETERS
    slot_id : Card slot

  RETURN VALUE
    None.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_send_sap_status_indication_to_all
(
  mmgsdi_slot_id_enum_type    slot_id
)
{
  uint8                         j           = 0;
  qmi_uimi_client_state_type  * cl_sp       = NULL;

  if(qmi_uim_state_ptr == NULL)
  {
    return;
  }

  for (j = 0; j < UIMQMI_SVC_MAX_CLIDS; j++)
  {
    cl_sp = (qmi_uimi_client_state_type*)qmi_uim_state_ptr->client[j];

    if ( cl_sp != NULL &&
         cl_sp->uim_info.reg_event_type.sap_events )
    {
      /* Send indication to specific client */
      qmi_uim_send_sap_status_indication(cl_sp->client_handle,
                                         slot_id);
    }
  }
} /* qmi_uim_send_sap_status_indication_to_all */


/*===========================================================================
  FUNCTION QMI_UIM_PROCESS_SAP_EVT()

  DESCRIPTION
    This function processes the SAP events from MMGSDI

  PARAMETERS
    event_ptr: pointer to event

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_process_sap_evt
(
  const mmgsdi_event_data_type * event_ptr
)
{
  boolean                       result      = FALSE;
  uint8                         slot_idx    = 0;
  mmgsdi_slot_id_enum_type      slot        = MMGSDI_SLOT_1;
  mmgsdi_sap_state_enum_type    sap_state   = MMGSDI_MAX_SAP_STATE_ENUM;

  ASSERT(event_ptr);
  ASSERT(qmi_uim_global_ptr);

  switch (event_ptr->evt)
  {
    case MMGSDI_SAP_CONNECT_EVT:
      slot = event_ptr->data.sap_connect.slot;
      sap_state = event_ptr->data.sap_connect.connect_state;
      break;
    case MMGSDI_SAP_DISCONNECT_EVT:
      slot = event_ptr->data.sap_disconnect.slot;
      sap_state = event_ptr->data.sap_disconnect.disconnect_state;
      break;
    case MMGSDI_CARD_ERROR_EVT:
      slot = event_ptr->data.card_error.slot;
      break;
    default:
      return result;
  }

  UIM_MSG_HIGH_3("Event: 0x%x, slot = 0x%x, sap state = 0x%x",
                 event_ptr->evt, slot, sap_state);

  if (uimqmi_mmgsdi_slot_to_slot_index(slot,
                                       &slot_idx) != QMI_ERR_NONE_V01)
  {
    return result;
  }

  if(slot_idx >= QMI_UIM_MAX_CARD_COUNT ||
     slot_idx >= qmi_uim_global_ptr->card_state.num_slots)
  {
    return result;
  }

  switch (event_ptr->evt)
  {
    case MMGSDI_SAP_CONNECT_EVT:
    case MMGSDI_SAP_DISCONNECT_EVT:
      /* Update global SAP state */
      if (qmi_uim_global_ptr->sap_info[slot_idx].sap_state != sap_state)
      {
        result = TRUE;
        qmi_uim_global_ptr->sap_info[slot_idx].sap_state = sap_state;

        /* After connection errors have been posted to clients, we reset our flag */
        if (sap_state == MMGSDI_SAP_CONNECTING ||
            sap_state == MMGSDI_SAP_CONNECTED  ||
            sap_state == MMGSDI_SAP_DISCONNECTING)
        {
          qmi_uim_global_ptr->sap_info[slot_idx].is_enabled = TRUE;
        }
        else
        {
          qmi_uim_global_ptr->sap_info[slot_idx].is_enabled = FALSE;
        }
      }
      break;
    case MMGSDI_CARD_ERROR_EVT:
      /* Send SAP disconnect indication only if we are connected and the card
         error info is not due to SAP connected */
      if ((qmi_uim_global_ptr->sap_info[slot_idx].sap_state == MMGSDI_SAP_CONNECTING ||
           qmi_uim_global_ptr->sap_info[slot_idx].sap_state == MMGSDI_SAP_CONNECTED ||
           qmi_uim_global_ptr->sap_info[slot_idx].sap_state == MMGSDI_SAP_DISCONNECTING) &&
          qmi_uim_global_ptr->sap_info[slot_idx].is_enabled                              &&
          event_ptr->data.card_error.info != MMGSDI_CARD_ERR_SAP_CONNECTED)
      {
        /* Update global SAP state and send the indication*/
        qmi_uim_global_ptr->sap_info[slot_idx].sap_state = MMGSDI_SAP_DISCONNECTED;
        qmi_uim_send_sap_status_indication_to_all(slot);

        /* Update is_enabled global after sending SAP indication */
        qmi_uim_global_ptr->sap_info[slot_idx].is_enabled = FALSE;
      }
      break;
    default:
      break;
  }

  return result;
} /* qmi_uim_process_sap_evt */


/*===========================================================================
  FUNCTION QMI_UIM_IS_SAP_SUPPORTED()

  DESCRIPTION
    this fucntion indicates qmi uim sap indication
    is supported or not based upon the file compilation.
    qmi_uim_sap.c or qmi_uim_sap_stubs.c

  PARAMETERS
    None
    
  RETURN VALUE
    TRUE: if qmi_uim_sap.c got compiled
    FALSE: if qmi_uim_sap_stubs.c got compiled
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_is_sap_supported(void)
{
  return TRUE;
} /* qmi_uim_is_sap_supported */
