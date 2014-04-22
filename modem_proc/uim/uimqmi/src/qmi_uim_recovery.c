/*===========================================================================

                         Q M I _ U I M _ R E C O V E R Y. C

DESCRIPTION

 The Data Services Qualcomm MSM Interface Device Management Services source
 file.

Copyright (c) 2016, 2018 - 2019 QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //components/rel/uim.mpss/3.3.1/uimqmi/src/qmi_uim_recovery.c#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/22/19    tq     Masked unsupported event registration 
10/15/18    tq     Migration of QMI UIM service to QCCI-QCSI
07/14/16    ar     Move qmi_uimi_recovery and unify fdn/acl get_status func.
05/19/16    vdc    Initial Revision.
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "uim_p.h"
#include "qmi_uim.h"
#include "mmgsdilib_common.h"
#include "qmi_uim_recovery.h"
#include "uim_msg.h"
#include "qmi_uim_util.h"
#include "amssassert.h"
#include "qmi_cat.h"

/*===========================================================================

                             EXTERNAL FUNCTIONS

===========================================================================*/


/*===========================================================================
  FUNCTION QMI_UIMI_RECOVERY_CALLBACK()

  DESCRIPTION
    This function is executed as callback of UIM Recovery API. This
    is executed in the context of UIM task and needs to make a deep
    copy of the results and post a command to the QMI task to be
    processed.

  PARAMETERS

  RETURN VALUE
    None.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uimi_recovery_callback
(
  uim_slot_type                     slot,
  const void                       *user_data,
  uimdrv_recovery_stage_enum_type   recovery_stage
)
{
  qmi_uim_message_type * msg_ptr = NULL;

  switch(recovery_stage)
  {
    case UIMDRV_RECOVERY_STAGE_START:
      UIM_MSG_MED_0("Recovery stage start response");
      /* Nothing to do */
      break;

    case UIMDRV_RECOVERY_STAGE_END:
      UIM_MSG_MED_0("Recovery stage end response");
      msg_ptr = qmi_uim_util_create_message(QMI_UIM_MESSAGE_RECOVERY_CB);
      if (msg_ptr != NULL)
      {
        msg_ptr->data.recovery_data.slot = slot;
        msg_ptr->data.recovery_data.user_data = user_data;

        /* Send command */
        qmi_uim_util_post_message(msg_ptr);
        msg_ptr = NULL;
      }
      break;

    default:
      break;
  }
} /* qmi_uimi_recovery_callback */


/*===========================================================================
  FUNCTION  QMI_UIM_RECOVERY_PROCESS_CALLBACK()

  DESCRIPTION
    Generic QMI processing for external commands

  PARAMETERS
    uim_message_ptr : message to the internal qmi_uim_message
                      containing an external command

  RETURN VALUE
    None

  DEPENDENCIES
    QMI UIM must already have been initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_recovery_process_callback
(
  qmi_uim_message_type * uim_message_ptr
)
{
  qmi_uimi_userdata_type             *cb_userdata_ptr   = NULL;
  uint16                              client_index      = 0;
  qmi_uim_cmd_buf_s                  *cmd_buf_ptr       = NULL;
  uim_recovery_resp_msg_v01          *recovery_resp_ptr = NULL;
  void                               *resp_ptr          = NULL;
  uint32                              resp_len          = 0;
  
  UIM_MSG_HIGH_0("Sending response for recovery");

  ASSERT(uim_message_ptr);

  /* Retrieve userdata */
  cb_userdata_ptr = (qmi_uimi_userdata_type*)uim_message_ptr->data.recovery_data.user_data;
  if (cb_userdata_ptr == NULL)
  {
    UIM_MSG_HIGH_0("Null userdata ptr in response");
    return;
  }

  /* First check if the requested client is still valid */
  if (FALSE == qmi_uim_util_check_clid(cb_userdata_ptr->client_handle, cb_userdata_ptr->client_reg_count, &client_index))
  {
    goto free_cmd_data;
  }
  
  cmd_buf_ptr = qmi_uim_util_get_cmd_buf_ptr(cb_userdata_ptr->req_handle, cb_userdata_ptr->client_handle);
  if(cmd_buf_ptr == NULL)
  {
    goto free_cmd_data;
  }

  ASSERT(cb_userdata_ptr->request_id == QMI_UIM_RECOVERY_REQ_V01);
  
  if(cb_userdata_ptr->request_id == QMI_UIM_RECOVERY_REQ_V01)
  {
    recovery_resp_ptr = (uim_recovery_resp_msg_v01*) uimqmi_malloc(sizeof(uim_recovery_resp_msg_v01));
    if(recovery_resp_ptr == NULL)
    {
      goto free_cmd_data;
    }
    resp_ptr = recovery_resp_ptr;
    resp_len = sizeof(uim_recovery_resp_msg_v01);

    qmi_uim_util_populate_resp(QMI_ERR_NONE_V01, &recovery_resp_ptr->resp);
  }

  qmi_uim_send_response_ind(
    cmd_buf_ptr,
    resp_ptr,
    cb_userdata_ptr->request_id,
    resp_len,
    FALSE,
    cb_userdata_ptr->client_handle);

  uimqmi_free(resp_ptr);
  
free_cmd_data:
  if(cmd_buf_ptr)
  {
    qmi_uim_util_remove_qmi_cmd(
      cb_userdata_ptr->req_handle,
      cb_userdata_ptr->client_handle,
      &qmi_uim_state_ptr->client[client_index]->cmd_q);
  }

  /* Free userdata */
  qmi_uim_util_free_cb_userdata(cb_userdata_ptr);
  cb_userdata_ptr = NULL;
} /* qmi_uim_recovery_process_callback */


/*===========================================================================
  FUNCTION QMI_UIM_RECOVERY_SEND_COMPLETE_INDICATION()

  DESCRIPTION
    This function sends a recovery complete indication to a specific client.

  PARAMETERS
    clid   : Client ID
    slot   : Slot of cooresponding of recovery

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_uim_recovery_send_complete_indication
(
  qmi_client_handle             client_handle,
  mmgsdi_slot_id_enum_type      slot
)
{
  uim_recovery_ind_msg_v01               * indication_ptr   = NULL;

  ASSERT(qmi_uim_util_get_cli_index(client_handle, NULL));

  if(slot > MMGSDI_SLOT_3)
  { 
    return;
  }
  indication_ptr = (uim_recovery_ind_msg_v01*)uimqmi_malloc(sizeof(uim_recovery_ind_msg_v01));
  
  if(indication_ptr == NULL)
  {
    return;
  }
  indication_ptr->slot = (uim_slot_enum_v01)slot;

  /* Send indication to the client */
  (void)qmi_uim_send_response_ind(NULL,
                                  indication_ptr,
                                  (uint16)QMI_UIM_RECOVERY_IND_V01,
                                  sizeof(uim_recovery_ind_msg_v01),
                                  TRUE,
                                  client_handle);
  uimqmi_free(indication_ptr);
} /* qmi_uim_recovery_send_complete_indication */


/*===========================================================================
  FUNCTION QMI_UIM_RECOVERY_SEND_COMPLETE_INDICATION_TO_ALL()

  DESCRIPTION
    This function processes the recovery complete indication from UIM drivers.
    It passes indicates that recovery has completed successfully to all
    registered QMI clients.

  PARAMETERS
    slot : slot in which the recovery was performed

  RETURN VALUE
    None

  DEPENDENCIES
    QMI UIM must already have been initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_uim_recovery_send_complete_indication_to_all
(
  mmgsdi_slot_id_enum_type  slot
)
{
  uint8                         i           = 0;
  qmi_uimi_client_state_type  * cl_sp       = NULL;

  UIM_MSG_HIGH_1("qmi_uim_recovery_send_complete_indication_to_all: slot = 0x%x",
                 slot);

  if(qmi_uim_state_ptr == NULL)
  {
    return;
  }

  for (i = 0; i < UIMQMI_SVC_MAX_CLIDS; i++)
  {
    cl_sp = (qmi_uimi_client_state_type*)qmi_uim_state_ptr->client[i];

    /* Send recovery complete indication if client requested for them
       during client registration (saved in reg_for_recovery_complete_event flag) */
    if ( cl_sp != NULL &&
         qmi_uim_util_get_cli_index(cl_sp->client_handle, NULL) &&
         cl_sp->uim_info.reg_event_type.recovery_complete_event )
    {
      qmi_uim_recovery_send_complete_indication(cl_sp->client_handle, slot);
    }
  }
} /* qmi_uim_recovery_send_complete_indication_to_all */


/*===========================================================================
  FUNCTION QMI_UIM_PROCESS_RECOVERY_COMPLETE()

  DESCRIPTION
    This function processes the recovery complete indication from UIM drivers.
    It passes indicates that recovery has completed successfully to all registered
    QMI clients and QMI CAT.

  PARAMETERS
    slot : slot in which the recovery was performed

  RETURN VALUE
    None

  DEPENDENCIES
    QMI UIM must already have been initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_process_recovery_complete
(
  uim_slot_type        slot
)
{
  mmgsdi_slot_id_enum_type      mmgsdi_slot = MMGSDI_MAX_SLOT_ID_ENUM;

  UIM_MSG_HIGH_1("qmi_uim_process_recovery_complete: uim slot = 0x%x", slot);

  if(qmi_uim_state_ptr == NULL)
  {
    return;
  }

  /* Convert uim_slot_type to mmgsdi_slot_id_enum_type */
  if(qmi_uim_util_uim_slot_id_to_mmgsdi_slot_id(slot, &mmgsdi_slot) != QMI_ERR_NONE_V01)
  {
    return;
  }

  /* Notify QMI CAT */
  qmi_cat_recovery_complete(mmgsdi_slot);

  /* Notify QMI UIM clients */
  qmi_uim_recovery_send_complete_indication_to_all(mmgsdi_slot);
} /* qmi_uim_process_recovery_complete */


/*===========================================================================
  FUNCTION QMI_UIM_RECOVERY_PROCESS_COMPLETE_EVT()

  DESCRIPTION
    This function processes the recovery complete indication from UIM drivers.
    It indicates to all registered QMI clients and to QMI CAT that recovery
    has completed successfully.

  PARAMETERS
    mmgsdi_slot:  Slot in which the recovery was performed

  RETURN VALUE
    None

  DEPENDENCIES
    QMI UIM must already have been initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_recovery_process_complete_evt
(
  mmgsdi_slot_id_enum_type        mmgsdi_slot
)
{

  UIM_MSG_HIGH_1("qmi_uim_recovery_process_complete_evt: mmgsdi slot = 0x%x",
                 mmgsdi_slot);

  /* Notify QMI CAT */
  qmi_cat_recovery_complete(mmgsdi_slot);

  /* Notify QMI UIM clients */
  qmi_uim_recovery_send_complete_indication_to_all(mmgsdi_slot);
} /* qmi_uim_recovery_process_complete_evt */


/*===========================================================================
  FUNCTION QMI_UIMI_RECOVERY()

  DESCRIPTION
    Recovers the card information

  PARAMETERS
    cl_sp            : client state pointer
    req_handle       : req handle
    req_c_struct     : IDL client structure
    req_c_struct_len : structure len

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_error qmi_uimi_recovery
(
  qmi_uimi_client_state_type  * cl_sp,
  qmi_req_handle                req_handle,
  void                        * req_c_struct,
  int                           req_c_struct_len
)
{
  qmi_error_type_v01           errval            = QMI_ERR_NONE_V01;
  uim_return_type              uim_status        = UIM_GENERIC_ERROR;
  uim_slot_type                slot              = UIM_SLOT_NONE;
  qmi_uimi_userdata_type      *cb_userdata_ptr   = NULL;
  uint8                        recovery_flag     = 0;
  mmgsdi_slot_id_enum_type     mmgsdi_slot       = MMGSDI_MAX_SLOT_ID_ENUM;
  uim_recovery_req_msg_v01    *recovery_req_ptr  = NULL;
  uim_recovery_resp_msg_v01   *recovery_resp_ptr = NULL;
  uint16                       client_index      = 0;
  qmi_uim_cmd_buf_s           *cmd_buf_p         = NULL;
  qmi_csi_error                csi_errval        = QMI_CSI_NO_ERR;

  ASSERT(cl_sp && req_c_struct && req_c_struct_len > 0 && qmi_uim_state_ptr);

  if(!qmi_uim_util_get_cli_index(cl_sp->client_handle, &client_index))
  {
    return QMI_CSI_INVALID_HANDLE;
  }

  cmd_buf_p = qmi_uim_util_get_cmd_buf_ptr(req_handle, cl_sp->client_handle);

  ASSERT(cmd_buf_p);

  recovery_req_ptr = (uim_recovery_req_msg_v01 *)req_c_struct;
  cmd_buf_p->req_handle = req_handle;  

  errval = qmi_uim_map_and_validate_slot(&mmgsdi_slot, recovery_req_ptr->slot);
  if(errval != QMI_ERR_NONE_V01)
  {
    goto send_result;
  }

  if(qmi_uim_util_mmgsdi_slot_id_to_uim_slot_id(mmgsdi_slot,
                                                &slot) != QMI_ERR_NONE_V01)
  {
    errval = QMI_ERR_INVALID_ARG_V01;
    goto send_result;
  }

  if (uim_common_efs_read(
        UIM_COMMON_EFS_UIMQMI_QMI_UIM_CONFIG_SILENT_RECOVERY,
        UIM_COMMON_EFS_ITEM_FILE_TYPE,
        UIM_COMMON_EFS_DEVICE,
        &recovery_flag,
        sizeof(recovery_flag)) != UIM_COMMON_EFS_SUCCESS)
  {
    UIM_MSG_HIGH_0("Failed to read recovery configuration from NV");
    recovery_flag = FALSE;
  }

  if (recovery_flag == FALSE)
  {
    UIM_MSG_HIGH_0("External recovery not allowed");
    errval = QMI_ERR_NOT_SUPPORTED_V01;
    goto send_result;
  }

  /* Create userdata */
  cb_userdata_ptr = qmi_uim_util_get_cb_userdata(QMI_UIM_RECOVERY_REQ_V01,
                                                 cmd_buf_p,
                                                 NULL,
                                                 cl_sp->client_handle);
  if (cb_userdata_ptr == NULL)
  {
    errval = QMI_ERR_NO_MEMORY;
    goto send_result;
  }

  uim_status = uim_trigger_silent_recovery(slot,
                                           cb_userdata_ptr,
                                           qmi_uimi_recovery_callback);

  if (uim_status != UIM_SUCCESS)
  {
    errval = QMI_ERR_INTERNAL_V01;
  }

send_result:
  if(errval == QMI_ERR_NONE_V01)
  {
    cmd_buf_p->resp_pending = TRUE;
    return QMI_CSI_NO_ERR;
  }
  
  recovery_resp_ptr =   uimqmi_malloc(sizeof(uim_recovery_resp_msg_v01));
  if(!recovery_resp_ptr)
  {
    return QMI_CSI_NO_MEM;
  }
  /* Free user data in case of error*/
  qmi_uim_util_free_cb_userdata(cb_userdata_ptr);
  
  qmi_uim_util_populate_resp(errval, &recovery_resp_ptr->resp);

  csi_errval = qmi_uim_send_immidiate_resp(cmd_buf_p, 
                                           recovery_resp_ptr,
                                           cmd_buf_p->cmd_type,
                                           sizeof(uim_recovery_resp_msg_v01),
                                           cl_sp->client_handle);
  uimqmi_free(recovery_resp_ptr);
 
  return csi_errval;
} /* qmi_uimi_recovery */


/*===========================================================================
  FUNCTION QMI_UIM_IS_RECOVERY_SUPPORTED()

  DESCRIPTION
    this fucntion indicatesqmi uim recovery indication
    is supported or not based upon the file compilation.
    qmi_uim_recovery.c or qmi_uim_recovery_stubs.c

  PARAMETERS
    None
    
  RETURN VALUE
    TRUE: if qmi_uim_recovery.c got compiled
    FALSE: if qmi_uim_recovery_stubs.c got compiled
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_is_recovery_supported(void)
{
  return TRUE;
} /* qmi_uim_is_recovery_supported */
