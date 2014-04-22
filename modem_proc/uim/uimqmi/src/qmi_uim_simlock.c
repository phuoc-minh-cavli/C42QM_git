/*===========================================================================

                         Q M I _ U I M _ S I M L O C K. C

DESCRIPTION

 The Data Services Qualcomm MSM Interface Device Management Services source
 file.

Copyright (c) 2016, 2018, 2021 QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //components/rel/uim.mpss/3.3.1/uimqmi/src/qmi_uim_simlock.c#2 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
07/07/21    tq     Enabled support for SIMLOCK
10/15/18    tq     Migration of QMI UIM service to QCCI-QCSI
12/05/16    bcho   Remove compilation error
09/29/16    sn     Added support to retrieve retry attempts left for locking
09/28/16    sn     Send control key presence in get config presence
09/29/16    sn     Populate retries left correctly
07/26/16    sp     Move qmi_uimi_ functions to corresponding files
06/16/16    vdc    Split QMI UIM parsing files for supporting IOE
05/19/16    vdc    Initial Revision.
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "simlock_common.h"
#include "simlock_modem_lib.h"
#include "simlock_modem_p.h"
#include "user_identity_module_v01.h"
#include "user_identity_module_impl_v01.h"
#include "qmi_uim_simlock.h"
#include "mmgsdilib_common.h"
#include "qmi_uim.h"
#include "uim_msg.h"
#include "qmi_uim_util.h"
#include "qmi_uim_parsing.h"
#include "qmi_uim_simlock_parsing.h"
#include "qmi_uim_encryption.h"
#include "amssassert.h"
#include <stringl/stringl.h>

/*===========================================================================

                             EXTERNAL FUNCTIONS

===========================================================================*/

#ifdef FEATURE_SIMLOCK
/*===========================================================================
  FUNCTION QMI_UIM_SIMLOCK_MMGDSI_SLOT_ID_TO_SIMLOCK_SLOT_ID()

  DESCRIPTION
    Converts MMGSDI slot id to SIMLOCK slot id.

  PARAMETERS
    mmgsdi_slot_id       : MMGSDI slot id
    simlock_slot_id_ptr  : output parameter containing SIMLOCK slot id

  RETURN VALUE
    QMI client-specific error message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_uim_simlock_mmgsdi_slot_id_to_simlock_slot_id
(
  mmgsdi_slot_id_enum_type   mmgsdi_slot_id,
  simlock_slot_enum_type   * simlock_slot_id_ptr
)
{
  ASSERT(simlock_slot_id_ptr);

  switch(mmgsdi_slot_id)
  {
    case MMGSDI_SLOT_1:
      *simlock_slot_id_ptr = SIMLOCK_SLOT_1;
      break;

    case MMGSDI_SLOT_2:
      *simlock_slot_id_ptr = SIMLOCK_SLOT_2;
      break;

    case MMGSDI_SLOT_3:
      *simlock_slot_id_ptr = SIMLOCK_SLOT_3;
      break;

    default:
      *simlock_slot_id_ptr = SIMLOCK_SLOT_NUM;
      return QMI_ERR_INTERNAL_V01;
  }

  return QMI_ERR_NONE_V01;
} /* qmi_uim_simlock_mmgsdi_slot_id_to_simlock_slot_id */


/*===========================================================================
  FUNCTION QMI_UIM_SIMLOCK_SIMLOCK_SLOT_ID_TO_MMGSDI_SLOT_ID()

  DESCRIPTION
    Converts SIMLOCK slot id to MMGSDI slot id.

  PARAMETERS
    simock_slot_id     : SIMLOCK slot id
    mmgsdi_slot_id_ptr : output parameter containing MMGSDI slot id

  RETURN VALUE
    QMI client-specific error message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_type_v01 qmi_uim_simlock_simlock_slot_id_to_mmgsdi_slot_id
(
  simlock_slot_enum_type      simock_slot_id,
  mmgsdi_slot_id_enum_type  * mmgsdi_slot_id_ptr
)
{
  ASSERT(mmgsdi_slot_id_ptr);

  switch(simock_slot_id)
  {
    case SIMLOCK_SLOT_1:
      *mmgsdi_slot_id_ptr = MMGSDI_SLOT_1;
      break;

    case SIMLOCK_SLOT_2:
      *mmgsdi_slot_id_ptr = MMGSDI_SLOT_2;
      break;

    case SIMLOCK_SLOT_3:
      *mmgsdi_slot_id_ptr = MMGSDI_SLOT_3;
      break;

    default:
      *mmgsdi_slot_id_ptr = MMGSDI_MAX_SLOT_ID_ENUM;
      return QMI_ERR_INTERNAL_V01;
  }

  return QMI_ERR_NONE_V01;
} /* qmi_uim_simlock_simlock_slot_id_to_mmgsdi_slot_id */


/*===========================================================================
  FUNCTION QMI_UIM_SIMLOCK_REMOTE_SFS_CB()

  DESCRIPTION
    This function is executed in the context of SIM Lock task, when the
    simlock requires the data to be read from or written to a remote
    SFS

  PARAMETERS

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static simlock_result_enum_type qmi_uim_simlock_remote_sfs_cb (
  simlock_file_operation_enum_type            operation,
  simlock_file_type                           file,
  simlock_token_id_type                       token,
  const simlock_data_type                   * simlock_data_ptr,
  simlock_remote_sfs_response_cb_type         simlock_resp_cb
)
{
  qmi_uim_message_type * msg_ptr = NULL;

  ASSERT(simlock_data_ptr != NULL);

  UIM_MSG_HIGH_1("qmi_uim_simlock_remote_sfs_cb, operation: 0x%x", operation);

  msg_ptr = qmi_uim_util_create_message(QMI_UIM_MESSAGE_SIMLOCK_REMOTE_SFS_CB);

  if(msg_ptr != NULL)
  {
    msg_ptr->data.simlock_remote_sfs_req.operation           = operation;
    msg_ptr->data.simlock_remote_sfs_req.request_token_id    = token;
    msg_ptr->data.simlock_remote_sfs_req.file                = file;
    msg_ptr->data.simlock_remote_sfs_req.simlock_resp_cb     = simlock_resp_cb;

    if (simlock_data_ptr->data_len > 0)
    {
      msg_ptr->data.simlock_remote_sfs_req.simlock_data.data_len =
                                               simlock_data_ptr->data_len;

      uimqmi_deep_copy(
        (void**)&msg_ptr->data.simlock_remote_sfs_req.simlock_data.data_ptr,
        (void*)simlock_data_ptr->data_ptr,
        (uint32)simlock_data_ptr->data_len);
    }
    else
    {
      msg_ptr->data.simlock_remote_sfs_req.simlock_data.data_len = 0;
      msg_ptr->data.simlock_remote_sfs_req.simlock_data.data_ptr = NULL;
    }

    /* Send command */
    qmi_uim_util_post_message(msg_ptr);
    msg_ptr = NULL;

    return SIMLOCK_SUCCESS;
  }

  return SIMLOCK_GENERIC_ERROR;
} /* qmi_uim_simlock_remote_sfs_cb */


/*===========================================================================
  FUNCTION QMI_UIM_SIMLOCK_REGISTER_FOR_REMOTE_SFS_OPERATION()

  DESCRIPTION
    This function registers the QMI UIM callback with the simlock, so the
    remote SFS operations can be performed.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_simlock_register_for_remote_sfs_operation
(
  void
)
{
  /* Register for events from SIM Lock task */
  (void)simlock_register_remote_sfs_operation(SIMLOCK_REGISTER,
                                              qmi_uim_simlock_remote_sfs_cb);
} /* qmi_uim_simlock_register_for_remote_sfs_operation */


/*===========================================================================
  FUNCTION QMI_UIM_SIMLOCK_CONVERT_SIMLOCK_CATEGORY()

  DESCRIPTION
    Converts the SIM Lock category to MMGSDI feature type

  PARAMETERS
    category            : simlock category
    feature_ptr         : response mmgsdi category
    category_index_ptr  : response index value

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_uim_simlock_convert_simlock_category
(
  simlock_category_enum_type        simlock_category,
  mmgsdi_perso_feature_enum_type  * mmgsdi_category_ptr
)
{
  ASSERT(mmgsdi_category_ptr);

  switch (simlock_category)
  {
    case SIMLOCK_CATEGORY_3GPP_NW:
      *mmgsdi_category_ptr = MMGSDI_PERSO_NW;
      break;
    case SIMLOCK_CATEGORY_3GPP_NS:
      *mmgsdi_category_ptr = MMGSDI_PERSO_NS;
      break;
    case SIMLOCK_CATEGORY_3GPP_SP:
      *mmgsdi_category_ptr = MMGSDI_PERSO_SP;
      break;
    case SIMLOCK_CATEGORY_3GPP_CP:
      *mmgsdi_category_ptr = MMGSDI_PERSO_CP;
      break;
    case SIMLOCK_CATEGORY_3GPP_SIM:
      *mmgsdi_category_ptr = MMGSDI_PERSO_SIM;
      break;
    case SIMLOCK_CATEGORY_3GPP2_NW_TYPE1:
      *mmgsdi_category_ptr = MMGSDI_PERSO_RUIM_NW1;
      break;
    case SIMLOCK_CATEGORY_3GPP2_NW_TYPE2:
      *mmgsdi_category_ptr = MMGSDI_PERSO_RUIM_NW2;
      break;
    case SIMLOCK_CATEGORY_3GPP2_HRPD:
      *mmgsdi_category_ptr = MMGSDI_PERSO_RUIM_HRPD;
      break;
    case SIMLOCK_CATEGORY_3GPP2_SP:
      *mmgsdi_category_ptr = MMGSDI_PERSO_RUIM_SP;
      break;
    case SIMLOCK_CATEGORY_3GPP2_CP:
      *mmgsdi_category_ptr = MMGSDI_PERSO_RUIM_CP;
      break;
    case SIMLOCK_CATEGORY_3GPP2_RUIM:
      *mmgsdi_category_ptr = MMGSDI_PERSO_RUIM_RUIM;
      break;
    default:
      UIM_MSG_ERR_1("Invalid SIM Lock category: 0x%x", simlock_category);
      *mmgsdi_category_ptr = MMGSDI_MAX_PERSO_FEATURE_ENUM;
      return FALSE;
  }

  return TRUE;
} /* qmi_uim_simlock_convert_simlock_category */


/*===========================================================================
  FUNCTION QMI_UIM_SIMLOCK_UPDATE_SIMLOCK_RETRIES()

  DESCRIPTION
    Updates the number of SIM Lock retries in the global card status after
    a failed perso operation

  PARAMETERS
    category    : category to update retries
    slot        : slot id corresponding to unlock request
    slot_policy : the current slot policy of the simlock
    retries     : new number of retries

  RETURN VALUE
    Boolean indicating if there was an update

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_uim_simlock_update_simlock_retries
(
  simlock_category_enum_type      category,
  simlock_slot_enum_type          slot,
  simlock_slot_policy_enum_type   slot_policy,
  uint32                          retries
)
{
  uint16                           i                 = 0;
  boolean                          result            = FALSE;
  mmgsdi_slot_id_enum_type         mmgsdi_slot_id    = MMGSDI_SLOT_1;
  mmgsdi_perso_feature_enum_type   feature           = MMGSDI_MAX_PERSO_FEATURE_ENUM;

  ASSERT(qmi_uim_global_ptr != NULL);

  UIM_MSG_MED_2("qmi_uim_update_simlock_retries, slot: 0x%x, slot_policy: 0x%x",
                slot, slot_policy);

  result = qmi_uim_simlock_convert_simlock_category(category, &feature);
  if (result == FALSE)
  {
    return result;
  }

  if(qmi_uim_simlock_simlock_slot_id_to_mmgsdi_slot_id(slot, &mmgsdi_slot_id) != QMI_ERR_NONE_V01)
  {
    return FALSE;
  }

  for (i = 0; i < qmi_uim_global_ptr->card_state.num_apps &&
              i < QMI_UIM_MAX_APP_COUNT; i++)
  {
    if (qmi_uim_global_ptr->card_state.application[i] != NULL &&
        (qmi_uim_global_ptr->card_state.application[i]->app_state ==
          QMI_UIM_APP_STATE_PERSO ||
         qmi_uim_global_ptr->card_state.application[i]->app_state ==
          QMI_UIM_APP_STATE_READY) &&
        qmi_uim_global_ptr->card_state.application[i]->perso_feature == feature)
    {
      /* In this case, it is possible that each slot has a different
         lock configuration. So only apps belonging to a slot on which
         the unlock operation was performed are updated */
      if(slot_policy == SIMLOCK_SLOT_POLICY_UNIQUE_FOR_EACH_SLOT &&
         qmi_uim_global_ptr->card_state.application[i]->slot != mmgsdi_slot_id)
      {
        continue;
      }

      /* Update global status */
      if (qmi_uim_global_ptr->card_state.application[i]->perso_retries != retries)
      {
        qmi_uim_global_ptr->card_state.application[i]->perso_retries = retries;
        result = TRUE;
      }
    }
  }

  return result;
} /* qmi_uim_simlock_update_simlock_retries */


/*===========================================================================
  FUNCTION QMI_UIM_SIMLOCK_SEND_REMOTE_SFS_OPERATION_INDICATION()

  DESCRIPTION
    This function sends a simlock operation indication to a specific client.

  PARAMETERS
    client_handle  : Client Handle
    uim_message_ptr: QMI UIM message

  RETURN VALUE
    qmi_error_type_v01

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_type_v01 qmi_uim_simlock_send_remote_sfs_operation_indication
(
  qmi_client_handle               client_handle,
  const qmi_uim_message_type    * uim_message_ptr
)
{
  uim_simlock_configuration_ind_msg_v01  * indication_ptr   = NULL;
  qmi_csi_error                            csi_errval       = QMI_CSI_NO_ERR;

  ASSERT(uim_message_ptr);

  indication_ptr = (uim_simlock_configuration_ind_msg_v01*)uimqmi_malloc(sizeof(uim_simlock_configuration_ind_msg_v01));
  if(indication_ptr == NULL)
  {
    return QMI_ERR_NO_MEMORY_V01;
  }
  
  /* Insert the indication payload in TLV */
  if (FALSE == qmi_uim_simlock_remote_sfs_encrypted_ind(
                   QMI_UIM_SIMLOCK_CONFIGURATION_REQ_V01,
                   uim_message_ptr->data.simlock_remote_sfs_req.operation,
                   uim_message_ptr->data.simlock_remote_sfs_req.request_token_id,
                   uim_message_ptr->data.simlock_remote_sfs_req.file,
                   uim_message_ptr->data.simlock_remote_sfs_req.simlock_data,
                   indication_ptr))
  {
    /* Ignore error... there is nothing we can do about it.
       The event will not be delivered to the client */
    uimqmi_free(indication_ptr);
    return QMI_ERR_INTERNAL_V01;
  }

  /* Send indication to the client */
  csi_errval=  qmi_uim_send_response_ind(NULL,
                                         indication_ptr,
                                         (uint16)QMI_UIM_SIMLOCK_CONFIGURATION_REQ_V01,
                                         sizeof(uim_simlock_configuration_ind_msg_v01),
                                         TRUE,
                                         client_handle);
  uimqmi_free(indication_ptr);
  if(csi_errval != QMI_CSI_NO_ERR)
  {
    return QMI_ERR_INTERNAL_V01;
  }

  return QMI_ERR_NONE_V01;
} /* qmi_uim_simlock_send_remote_sfs_operation_indication */


/*===========================================================================
  FUNCTION QMI_UIM_SIMLOCK_SEND_REMOTE_SFS_OPERATION_INDICATION_TO_ALL()

  DESCRIPTION
    This function will send the simlock data operation indication to all
    registered clients.

  PARAMETERS
    uim_message_ptr: QMI UIM message

  RETURN VALUE
    TRUE  : If indication is sent to at least one client
    FALSE : If otherwise

  DEPENDENCIES
    QMI UIM must already have been initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_uim_simlock_send_remote_sfs_operation_indication_to_all
(
  const qmi_uim_message_type * uim_message_ptr
)
{
  uint8                              i               = 0;
  qmi_uimi_client_state_type       * cl_sp           = NULL;
  qmi_error_type_v01                 errval          = QMI_ERR_NONE_V01;
  boolean                            indication_sent = FALSE;

  if((qmi_uim_state_ptr == NULL) ||
     (uim_message_ptr == NULL))
  {
    return FALSE;
  }

  UIM_MSG_HIGH_0("qmi_uim_simlock_send_remote_sfs_operation_indication_to_all");

  for (i = 0; i < UIMQMI_SVC_MAX_CLIDS; i++)
  {
    cl_sp = (qmi_uimi_client_state_type*)qmi_uim_state_ptr->client[i];
    if(cl_sp == NULL)
    {
      continue;
    }

    /* Send simlock remote SFS indication if client requested for them during client event
       registration (saved in reg_event_type.simlock_remote_sfs_events flag) */
    if (cl_sp->uim_info.reg_event_type.simlock_remote_sfs_events)
    {
      errval = qmi_uim_simlock_send_remote_sfs_operation_indication(
                   cl_sp->client_handle,
                   uim_message_ptr);
      if ((errval == QMI_ERR_NONE_V01) &&
          (!indication_sent))
      {
        indication_sent = TRUE;
      }
    }
  }

  return indication_sent;
} /* qmi_uim_simlock_send_remote_sfs_operation_indication_to_all */


/*===========================================================================
  FUNCTION QMI_UIM_SIMLOCK_CHECK_AND_UPDATE_IF_SEND_TEMPORARY_UNLOCK_STATUS_CHANGED()

  DESCRIPTION
    This function will check if the temporary unlock status is changed.
        It compares the temporary unlock status received from SIMLOCK against the one
        maintained at QMI UIM.If atleast a single feature's temporary unlock status is
    changed then this function would return TRUE.Also, it would update the QMI UIM
    global maintaining the temp unlock status.

  PARAMETERS
    get_status_resp_ptr  : Pointer to the get status response received from SIMLOCK

  RETURN VALUE
    TRUE  : If status is changed
    FALSE : If otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_uim_simlock_check_and_update_if_simlock_temporary_unlock_status_changed(
  const simlock_get_status_msg_resp_type  *get_status_ptr
)
{
  boolean          send_ind     = FALSE;
  uint8            i            = 0;
  uint8            j            = 0;
  uint8            slot_cnt     = 0;

  ASSERT(get_status_ptr);

  slot_cnt = MIN(qmi_uim_global_ptr->card_state.num_slots, QMI_UIM_MAX_CARD_COUNT);

  for (i = 0;
       i < SIMLOCK_SLOT_COUNT_MAX && i < slot_cnt;
       i++)
  {
    /* Check the features that are active and insert the
       corresponding feature id and time left in the TLV data */
    for(j = 0;
        j < SIMLOCK_CATEGORY_COUNT_MAX && j < QMI_UIM_MAX_NUMBER_PERSO_FEATURES;
        j++)
    {
      if ((get_status_ptr->lock_info[i].category_info[j].category_status) &&
          (get_status_ptr->remaining_time.duration[i][j] > 0))
      {
        if(qmi_uim_global_ptr->temp_unlock_info[i][j] == FALSE)
        {
          /* A new feature is found to be temporarily unlocked.
             Need to send indication to client */
            send_ind = TRUE;
            qmi_uim_global_ptr->temp_unlock_info[i][j] = TRUE;
          }
        }
      else
      {
        if(qmi_uim_global_ptr->temp_unlock_info[i][j])
        {
          /* Temporary unlock for a feature seem to be expired.
             Need to send indication to client */
            send_ind = TRUE;
            qmi_uim_global_ptr->temp_unlock_info[i][j] = FALSE;
        }
      }
    }
  }

  UIM_MSG_MED_1("qmi_uim_check_and_update_if_simlock_temporary_unlock_status_changed "
                "send_ind is 0x%x", send_ind);

  return TRUE;
}/* qmi_uim_simlock_check_and_update_if_simlock_temporary_unlock_status_changed */


/*===========================================================================
  FUNCTION QMI_UIM_SIMLOCK_SEND_TEMPORARY_UNLOCK_INDICATION()

  DESCRIPTION
    This function sends a temporary unlock  indication to a specific client.

  PARAMETERS
    cli_handle           : Client Handle
    uim_message_ptr      : QMI UIM message

  RETURN VALUE
    qmi_error_type_v01

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_type_v01 qmi_uim_simlock_send_temporary_unlock_indication
(
  qmi_client_handle                        cli_handle,
  const simlock_get_status_msg_resp_type  *get_status_ptr
)
{
  uim_temporary_unlock_ind_msg_v01    * indication_ptr = NULL;
  qmi_csi_error                         csi_err_val    = QMI_CSI_NO_ERR;

  ASSERT(get_status_ptr);

  indication_ptr = (uim_temporary_unlock_ind_msg_v01*)uimqmi_malloc(sizeof(uim_temporary_unlock_ind_msg_v01));
  if (indication_ptr == NULL)
  {
    return QMI_ERR_NO_MEMORY_V01;
  }

  /* Insert the indication payload in TLV */
  if (FALSE == qmi_uim_response_temporary_unlock_status(indication_ptr,
                                                        get_status_ptr,
                                                        qmi_uim_global_ptr->card_state.num_slots))
  {
    /* Ignore error... there is nothing we can do about it.
       The event will not be delivered to the client */
    UIM_MSG_MED_1("qmi_uim_response_temporary_unlock_status failed. No ind sent to client 0x%x", cli_handle);
    uimqmi_free(indication_ptr);
    return QMI_ERR_INTERNAL_V01;
  }

  csi_err_val = qmi_uim_send_response_ind(NULL,
                                          indication_ptr,
                                          QMI_UIM_TEMPORARY_UNLOCK_STATUS_IND_V01,
                                          sizeof(uim_temporary_unlock_ind_msg_v01),
                                          TRUE,
                                          cli_handle);
  uimqmi_free(indication_ptr);

  if(csi_err_val != QMI_CSI_NO_ERR)
  {
    return QMI_ERR_INTERNAL_V01;
  }

  return QMI_ERR_NONE_V01;
} /* qmi_uim_simlock_send_temporary_unlock_indication */


/*===========================================================================
  FUNCTION QMI_UIM_SIMLOCK_SEND_TEMPORARY_UNLOCK_INDICATION_TO_ALL()

  DESCRIPTION
    This function will send the temporary unlock indication to all
    registered clients.

  PARAMETERS
    get_status_ptr: Pointer to get status response received from SIMLOCK

  RETURN VALUE
    None

  DEPENDENCIES
    QMI UIM must already have been initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_uim_simlock_send_temporary_unlock_indication_to_all
(
  const simlock_get_status_msg_resp_type *get_status_ptr
)
{
  uint8                              i               = 0;
  qmi_uimi_client_state_type       * cl_sp           = NULL;
  qmi_error_type_v01                 errval          = QMI_ERR_NONE_V01;

  if(qmi_uim_state_ptr == NULL ||
     get_status_ptr == NULL)
  {
    return;
  }

  UIM_MSG_HIGH_0("qmi_uim_send_temporary_unlock_indication_to_all");

  for (i = 0; i < UIMQMI_SVC_MAX_CLIDS; i++)
  {
    cl_sp = (qmi_uimi_client_state_type*)qmi_uim_state_ptr->client[i];
    if(cl_sp == NULL)
    {
      continue;
    }

    /* Send temporary unlock indication if client requested for them during client event
       registration (saved in reg_event_type.simlock_temp_unlock_status flag) */
    if (cl_sp->uim_info.reg_event_type.simlock_temp_unlock_status)
    {
      errval = qmi_uim_simlock_send_temporary_unlock_indication(
                   cl_sp->client_handle,
                   get_status_ptr);
      }
    }
} /* qmi_uim_simlock_send_temporary_unlock_indication_to_all */


/*===========================================================================
  FUNCTION QMI_UIMI_SIMLOCK_SET_REMOTE_SFS_CALLBACK()

  DESCRIPTION
    This function is executed as callback of simlock_set_remote_sfs API.
    This is executed in the context of SIM Lock task.

  PARAMETERS

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uimi_simlock_set_remote_sfs_callback
(
  simlock_result_enum_type                    status,
  const void                                * user_data_ptr
)
{
  qmi_uim_message_type * msg_ptr = NULL;

  ASSERT(user_data_ptr != NULL);

  UIM_MSG_HIGH_0("Got response from set remote SFS req");

  msg_ptr = qmi_uim_util_create_message(QMI_UIM_MESSAGE_SIMLOCK_SET_REMOTE_SFS_CB);

  if(msg_ptr != NULL)
  {
    msg_ptr->data.simlock_set_remote_sfs_resp.status        = status;
    msg_ptr->data.simlock_set_remote_sfs_resp.user_data_ptr = user_data_ptr;

    /* Send command */
    qmi_uim_util_post_message(msg_ptr);
    msg_ptr = NULL;
  }
} /* qmi_uimi_simlock_set_remote_sfs_callback */


/*===========================================================================
  FUNCTION QMI_UIMI_SIMLOCK_REMOTE_UNLOCK_CALLBACK()

  DESCRIPTION
    This function is executed as callback of SIM Lock remote unlock API.
    This is executed in the context of SIM Lock task and needs to make a deep
    copy of the results and post a command to the QMI task to be processed.

  PARAMETERS

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uimi_simlock_remote_unlock_callback
(
  simlock_result_enum_type             status,
  const simlock_data_type              resp_data,
  const void                         * user_data_ptr
)
{
  qmi_uim_message_type * msg_ptr = NULL;

  msg_ptr = qmi_uim_util_create_message(QMI_UIM_MESSAGE_REMOTE_UNLOCK_CB);
  if(msg_ptr != NULL)
  {
    msg_ptr->data.remote_unlock_resp.status        = status;
    msg_ptr->data.remote_unlock_resp.user_data_ptr = user_data_ptr;

    if (resp_data.data_len > 0)
    {
      msg_ptr->data.remote_unlock_resp.key_data.data_len =
                                               resp_data.data_len;

      uimqmi_deep_copy(
        (void**)&msg_ptr->data.remote_unlock_resp.key_data.data_ptr,
        (void*)resp_data.data_ptr,
        (uint32)resp_data.data_len);
    }
    else
    {
      msg_ptr->data.remote_unlock_resp.key_data.data_len = 0;
      msg_ptr->data.remote_unlock_resp.key_data.data_ptr = NULL;
    }

    /* Send command */
    qmi_uim_util_post_message(msg_ptr);
    msg_ptr = NULL;
  }
} /* qmi_uimi_simlock_remote_unlock_callback */


/*===========================================================================
  FUNCTION QMI_UIMI_SIMLOCK_TEMPORARY_UNLOCK_CALLBACK()

  DESCRIPTION
    This function is executed as callback of SIM Lock API. This is
    executed in the context of SIM Lock task and needs to make a deep
    copy of the results and post a command to the QMI task to be
    processed.

  PARAMETERS

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_uimi_simlock_temporary_unlock_callback
(
  simlock_result_enum_type                    status,
  const simlock_message_response_data_type  * resp_ptr,
  const void                                * user_data_ptr
)
{
  qmi_uim_message_type * msg_ptr = NULL;

  ASSERT(resp_ptr);

  UIM_MSG_HIGH_0("qmi_uimi_simlock_get_status_callback");

  msg_ptr = qmi_uim_util_create_message(QMI_UIM_MESSAGE_SIMLOCK_TEMPORARY_UNLOCK_CB);
  if (msg_ptr != NULL)
  {
    msg_ptr->data.simlock.status  = status;
    msg_ptr->data.simlock.user_data_ptr = user_data_ptr;

    memscpy(&msg_ptr->data.simlock.data,
            sizeof(msg_ptr->data.simlock.data),
            resp_ptr,
            sizeof(simlock_message_response_data_type));

    /* Send command */
    qmi_uim_util_post_message(msg_ptr);
    msg_ptr = NULL;
  }
} /* qmi_uimi_simlock_temporary_unlock_callback */


/*===========================================================================
  FUNCTION  QMI_UIM_SIMLOCK_PROCESS_UNLOCK_RESP

  DESCRIPTION
    Processes the response to the SIM Lock unlock command

  PARAMETERS
    status          : Status of the unlock request
    unlock_ptr      : Pointer to the unlock response data
    user_data_ptr   : Pointer to the user data

  RETURN VALUE
    None

  DEPENDENCIES
    QMI UIM must already have been initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_uim_simlock_process_unlock_resp
(
  simlock_result_enum_type                     status,
  const simlock_unlock_device_msg_resp_type  * unlock_ptr,
  qmi_uimi_userdata_type                     * user_data_ptr
)
{
  qmi_error_type_v01                         errval               = QMI_ERR_NONE_V01;
  uint32                                     num_retries          = 0;
  uim_depersonalization_secure_resp_msg_v01 *deperso_sec_resp_ptr = NULL;
  uim_depersonalization_resp_msg_v01        *deperso_resp_ptr     = NULL;
  uint16                                     client_index         = 0;
  void                                      *ind_resp_ptr         = NULL;
  uint32                                     ind_resp_len         = 0;
  qmi_uim_cmd_buf_s                         *cmd_buf_ptr          = NULL;

  UIM_MSG_HIGH_0("qmi_uim_simlock_process_unlock_resp");

  ASSERT(unlock_ptr && user_data_ptr);

  if (status != SIMLOCK_SUCCESS && user_data_ptr->data.simlock_unlock.ck_present)
  {
    simlock_result_enum_type    simlock_status  = SIMLOCK_SUCCESS;

    if (unlock_ptr->num_retries_max == 0)
    {
      num_retries = QMI_UIM_MAX_RETRIES;
    }
    else if (unlock_ptr->curr_retries >= unlock_ptr->num_retries_max)
    {
      num_retries = 0;
    }
    else
    {
      num_retries = unlock_ptr->num_retries_max - unlock_ptr->curr_retries;
    }

    user_data_ptr->data.simlock_unlock.num_retries = num_retries;
    user_data_ptr->data.simlock_unlock.result      = status;

    simlock_status = simlock_get_status(qmi_uimi_simlock_callback,
                                        user_data_ptr);
    if(simlock_status == SIMLOCK_SUCCESS)
    {
      /* In case of sucess return. The response to client
         is sent in the get_status callback */
      return;
    }

    /* Update SIM Lock retries in the global variable. Since
       get_status failed, use the default slot policy to update retries */
    (void)qmi_uim_simlock_update_simlock_retries(user_data_ptr->data.simlock_unlock.category,
                                                 user_data_ptr->data.simlock_unlock.slot,
                                                 SIMLOCK_SLOT_POLICY_SAME_ON_ALL_SLOTS,
                                                 num_retries);
  }

  /* Convert SIMLOCK status in QMI code */
  errval = qmi_uim_simlock_convert_simlock_status_to_errval(status);

  /* First check if the requested client is still valid */
  if (FALSE == qmi_uim_util_check_clid(user_data_ptr->client_handle, user_data_ptr->client_reg_count, &client_index))
  {
    goto free_cmd_data;
  }

  cmd_buf_ptr = qmi_uim_util_get_cmd_buf_ptr(user_data_ptr->req_handle, user_data_ptr->client_handle);
  if(cmd_buf_ptr == NULL)
  {
    goto free_cmd_data;
  }

  if(user_data_ptr->request_id == QMI_UIM_DEPERSONALIZATION_SECURE_REQ_V01)
  {
    ind_resp_len  = sizeof(uim_depersonalization_secure_resp_msg_v01);
    deperso_sec_resp_ptr = (uim_depersonalization_secure_resp_msg_v01*) uimqmi_malloc(ind_resp_len);
    ind_resp_ptr = deperso_sec_resp_ptr;
    if(ind_resp_ptr == NULL)
    {
      goto free_cmd_data;
    }
  
    /* build the result and encrypt the data */
    deperso_sec_resp_ptr->encrypted_resp_len = sizeof(deperso_sec_resp_ptr->encrypted_resp); 
    deperso_sec_resp_ptr->encrypted_resp_valid = 1;
    errval = qmi_uim_response_encrypted_status(QMI_UIM_DEPERSONALIZATION_SECURE_REQ_V01,
                                                   user_data_ptr->data.simlock_unlock.request_id,
                                                   errval,
                                                   deperso_sec_resp_ptr->encrypted_resp,
                                                   &deperso_sec_resp_ptr->encrypted_resp_len);
    if(errval != QMI_ERR_NONE_V01)
    {
      /* Even if encrypted status fails, continue with sending
         response back to client so they are at least notified. */
      deperso_sec_resp_ptr->encrypted_resp_valid = 0;
      UIM_MSG_HIGH_0("failed to build and encrypt response");
    }
    qmi_uim_util_populate_resp(errval, &deperso_sec_resp_ptr->resp);
  }
  else if(user_data_ptr->request_id == QMI_UIM_DEPERSONALIZATION_REQ_V01)
  {
    ind_resp_len  = sizeof(uim_depersonalization_resp_msg_v01);
    deperso_resp_ptr = (uim_depersonalization_resp_msg_v01*) uimqmi_malloc(ind_resp_len);
    ind_resp_ptr = deperso_resp_ptr;
    if(ind_resp_ptr == NULL)
    {
      goto free_cmd_data;
    }  
    if (status != SIMLOCK_SUCCESS && user_data_ptr->data.simlock_unlock.ck_present)
    {
      /* Send the num_retries in the response */
      qmi_uim_response_retries_left(
                    &deperso_resp_ptr->retries_left.unblock_left,
                    &deperso_resp_ptr->retries_left.verify_left,
                    num_retries,
                    0);
      deperso_resp_ptr->retries_left_valid = 1;
    }
    qmi_uim_util_populate_resp(errval, &deperso_resp_ptr->resp);
  }
  else
  {
    UIM_MSG_HIGH_1("unexpected commaned 0x%x", user_data_ptr->request_id);
    goto free_cmd_data;
  }
  
  qmi_uim_send_response_ind( 
    cmd_buf_ptr,
    ind_resp_ptr,
    user_data_ptr->request_id,
    ind_resp_len,
    FALSE,
    user_data_ptr->client_handle);

free_cmd_data:
  uimqmi_free(ind_resp_ptr);

  if(cmd_buf_ptr)
  {
    qmi_uim_util_remove_qmi_cmd(
      user_data_ptr->req_handle,
      user_data_ptr->client_handle,
      &qmi_uim_state_ptr->client[client_index]->cmd_q);
  }

  /* Free userdata */
  qmi_uim_util_free_cb_userdata(user_data_ptr);
  user_data_ptr = NULL;
} /* qmi_uim_simlock_process_unlock_resp */


/*===========================================================================
  FUNCTION  QMI_UIM_SIMLOCK_PROCESS_GET_STATUS_RESP_AFTER_UNLOCK

  DESCRIPTION
    Processes the response to the SIM Lock get status command which was
    requested due to a failed unlock attempt

  PARAMETERS
    status          : Status of the get_status request
    get_status_ptr  : Pointer to the response data
    user_data_ptr   : Pointer to the user data

  RETURN VALUE
    None

  DEPENDENCIES
    QMI UIM must already have been initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_uim_simlock_process_get_status_resp_after_unlock
(
  simlock_result_enum_type                  status,
  const simlock_get_status_msg_resp_type  * get_status_ptr,
  qmi_uimi_userdata_type                  * user_data_ptr
)
{
  qmi_uim_cmd_buf_s                         *cmd_buf_ptr          = NULL;
  qmi_error_type_v01                         errval               = QMI_ERR_NONE_V01;
  simlock_slot_policy_enum_type              slot_policy          = SIMLOCK_SLOT_POLICY_SAME_ON_ALL_SLOTS;
  uim_depersonalization_resp_msg_v01        *deperso_resp_ptr     = NULL;
  uint16                                     client_index         = 0;
  
  UIM_MSG_MED_0("qmi_uim_simlock_process_get_status_resp_after_unlock");

  ASSERT(get_status_ptr && user_data_ptr);

  /* First check if the requested client is still valid */
  if (FALSE == qmi_uim_util_check_clid(user_data_ptr->client_handle, user_data_ptr->client_reg_count, &client_index))
  {
    goto free_cmd_data;
  }

  cmd_buf_ptr = qmi_uim_util_get_cmd_buf_ptr(user_data_ptr->req_handle, user_data_ptr->client_handle);
  if(cmd_buf_ptr == NULL)
  {
    goto free_cmd_data;
  }

  /* If the request is a success, retrieve the
     slot_policy from the response. In case the request
     is a failure, then set the policy to SAME_ON_ALL_SLOTS
     as the best possible option */
  if (status == SIMLOCK_SUCCESS)
  {
    slot_policy = get_status_ptr->slot_policy;
  }

  /* Update SIM Lock retries in the global variable */
  (void)qmi_uim_simlock_update_simlock_retries(user_data_ptr->data.simlock_unlock.category,
                                               user_data_ptr->data.simlock_unlock.slot,
                                               slot_policy,
                                               user_data_ptr->data.simlock_unlock.num_retries);

  errval = qmi_uim_simlock_convert_simlock_status_to_errval(user_data_ptr->data.simlock_unlock.result);

  deperso_resp_ptr = (uim_depersonalization_resp_msg_v01*) uimqmi_malloc(sizeof(uim_depersonalization_resp_msg_v01));
  if(deperso_resp_ptr == NULL)
  {
    goto free_cmd_data;
  }  

  /* Send the num_retries in the response */
  qmi_uim_response_retries_left(
                    &deperso_resp_ptr->retries_left.unblock_left,
                    &deperso_resp_ptr->retries_left.verify_left,
                    user_data_ptr->data.simlock_unlock.num_retries,
                    0);
  deperso_resp_ptr->retries_left_valid = 1;
  
  qmi_uim_util_populate_resp(errval, &deperso_resp_ptr->resp);

  qmi_uim_send_response_ind( 
    cmd_buf_ptr,
    deperso_resp_ptr,
    user_data_ptr->request_id,
    sizeof(uim_depersonalization_resp_msg_v01),
    FALSE,
    user_data_ptr->client_handle);

  uimqmi_free(deperso_resp_ptr);

free_cmd_data:
  if(cmd_buf_ptr)
  {
    qmi_uim_util_remove_qmi_cmd(
      user_data_ptr->req_handle,
      user_data_ptr->client_handle,
      &qmi_uim_state_ptr->client[client_index]->cmd_q);
  }

  /* Free userdata */
  qmi_uim_util_free_cb_userdata(user_data_ptr);
  user_data_ptr = NULL;
} /* qmi_uim_simlock_process_get_status_resp_after_unlock */


/*===========================================================================
  FUNCTION  QMI_UIM_SIMLOCK_PROCESS_GET_STATUS_RESP

  DESCRIPTION
    Processes the response to the SIM Lock get status command

  PARAMETERS
    status          : Status of the get_status request
    get_status_ptr  : Pointer to the response data
    user_data_ptr   : Pointer to the user data

  RETURN VALUE
    None

  DEPENDENCIES
    QMI UIM must already have been initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_uim_simlock_process_get_status_resp
(
  simlock_result_enum_type                  status,
  const simlock_get_status_msg_resp_type  * get_status_ptr,
  qmi_uimi_userdata_type                  * user_data_ptr
)
{
  qmi_uim_cmd_buf_s                  *cmd_buf_ptr         = NULL;
  qmi_error_type_v01                  errval              = QMI_ERR_NONE_V01;
  uim_get_configuration_resp_msg_v01 *get_config_resp_ptr = NULL;
  uint16                              client_index        = 0;
   
  UIM_MSG_HIGH_0("qmi_uim_process_simlock_get_status_resp");

  ASSERT(get_status_ptr && user_data_ptr && qmi_uim_global_ptr);

  if (status != SIMLOCK_SUCCESS)
  {
    UIM_MSG_ERR_1("Error in SIM Lock result: 0x%x", status);
    errval = QMI_ERR_INTERNAL_V01;
  }

  get_config_resp_ptr = (uim_get_configuration_resp_msg_v01*) uimqmi_malloc(sizeof(uim_get_configuration_resp_msg_v01));
  if(get_config_resp_ptr == NULL)
  {
    goto free_cmd_data;
  }
  /* If we are sending result here then personalisation data was definitly requested.*/
  /* Send the automatic selection TLV only if it was requested, even if we failed to get perso
     configuration */
  if(user_data_ptr->data.get_configuration.automatic_selection_status)
  {
    qmi_uim_response_boolean(&get_config_resp_ptr->automatic_selection,
                             &get_config_resp_ptr->automatic_selection_valid,
                             user_data_ptr->data.get_configuration.automatic_selection);
  }

  /* Insert TLV with perso configuration only in case of success */
  if ((user_data_ptr->data.get_configuration.personalization_status) &&
      (errval == QMI_ERR_NONE_V01))
  {
    if (FALSE == qmi_uim_response_get_configuration_simlock(
                   &get_config_resp_ptr->personalization_status_len,
                   &get_config_resp_ptr->personalization_status_valid,
                   get_config_resp_ptr->personalization_status,
                   get_status_ptr))
    {
      errval = QMI_ERR_INTERNAL_V01;
    }
  }

    /* Insert TLV with halt subscription configuration */
  if((user_data_ptr->data.get_configuration.halt_subscription_status) &&
     (errval == QMI_ERR_NONE_V01))
  {
    qmi_uim_response_boolean(&get_config_resp_ptr->halt_subscription,
                             &get_config_resp_ptr->halt_subscription_valid,
                             user_data_ptr->data.get_configuration.halt_subscription);
  }

  /* Insert TLV with temporary unlock time left */
  if (errval == QMI_ERR_NONE_V01)
  {
    if (FALSE == qmi_uim_response_temporary_unlock_time_left(
                        &get_config_resp_ptr->temporary_depersonalization_status_len,
                        &get_config_resp_ptr->temporary_depersonalization_status_valid,
                        get_config_resp_ptr->temporary_depersonalization_status,
                        qmi_uim_global_ptr->card_state.num_slots,
                        get_status_ptr))
    {
      errval = QMI_ERR_INTERNAL_V01;
    }
  }

  /* Insert TLV with perso reactivation only in case of success */
  if ((user_data_ptr->data.get_configuration.perso_reactivation_status) &&
      (errval == QMI_ERR_NONE_V01))
  {
    if (FALSE == qmi_uim_response_perso_reactivation_status(
                        &get_config_resp_ptr->perso_reactivation_status_len,
                        &get_config_resp_ptr->perso_reactivation_status_valid,
                        get_config_resp_ptr->perso_reactivation_status,
                        qmi_uim_global_ptr->card_state.num_slots,
                        get_status_ptr))
    {
      errval = QMI_ERR_NO_MEMORY_V01;
    }
  }

  /* Insert TLV with perso control key only in case of success */
  if (errval == QMI_ERR_NONE_V01)
  {
    if (FALSE == qmi_uim_response_perso_control_key_presence(
                        &get_config_resp_ptr->perso_control_key_present_len,
                        &get_config_resp_ptr->perso_control_key_present_valid,
                        get_config_resp_ptr->perso_control_key_present,
                        qmi_uim_global_ptr->card_state.num_slots,
                        get_status_ptr))
    {
      errval = QMI_ERR_NO_MEMORY_V01;
    }
  }

  /* First check if the requested client is still valid */
  if (FALSE == qmi_uim_util_check_clid(user_data_ptr->client_handle, user_data_ptr->client_reg_count, &client_index))
  {
    goto free_cmd_data;
  }

  cmd_buf_ptr = qmi_uim_util_get_cmd_buf_ptr(user_data_ptr->req_handle, user_data_ptr->client_handle);
  if( cmd_buf_ptr == NULL)
  {
    goto free_cmd_data;
  }

  qmi_uim_send_response_ind( 
    cmd_buf_ptr,
    get_config_resp_ptr,
    user_data_ptr->request_id,
    sizeof(*get_config_resp_ptr),
    FALSE,
    user_data_ptr->client_handle);

  uimqmi_free(get_config_resp_ptr);

free_cmd_data:
  if(cmd_buf_ptr)
  {
    qmi_uim_util_remove_qmi_cmd(
      user_data_ptr->req_handle,
      user_data_ptr->client_handle,
      &qmi_uim_state_ptr->client[client_index]->cmd_q);
  }

  /* Free userdata */
  qmi_uim_util_free_cb_userdata(user_data_ptr);
  user_data_ptr = NULL;
} /* qmi_uim_simlock_process_get_status_resp */


/*===========================================================================
  FUNCTION  QMI_UIM_SIMLOCK_PROCESS_TEMPORARY_UNLOCK_CB

  DESCRIPTION
    Processes the response to the SIM Lock get status command which is queued
    in handling of MMGSDI_TEMPORARY_UNLOCK_EVENT.

  PARAMETERS
    uim_message_ptr : Message pointer containing the information received from SIMLOCK
                      get status request

  RETURN VALUE
    None

  DEPENDENCIES
    QMI UIM must already have been initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_simlock_process_temporary_unlock_cb(
  const qmi_uim_message_type *uim_message_ptr
)
{
  const simlock_get_status_msg_resp_type *get_status_ptr = NULL;

  ASSERT(uim_message_ptr);

  UIM_MSG_MED_0("qmi_uim_simlock_process_temporary_unlock_cb");

  if(uim_message_ptr->message_type != QMI_UIM_MESSAGE_SIMLOCK_TEMPORARY_UNLOCK_CB)
  {
    return;
  }

  /* If the response from SIMLOCK is failure and is not get_status message,
     then no further processing can be done.*/
  if(uim_message_ptr->data.simlock.status != SIMLOCK_SUCCESS ||
     uim_message_ptr->data.simlock.data.msg_type != SIMLOCK_GET_STATUS_MSG)
  {
    return;
  }

  get_status_ptr = &uim_message_ptr->data.simlock.data.message.get_status_resp;

  if(qmi_uim_simlock_check_and_update_if_simlock_temporary_unlock_status_changed(
       get_status_ptr))
  {
    qmi_uim_simlock_send_temporary_unlock_indication_to_all(get_status_ptr);
  }
} /* qmi_uim_simlock_process_temporary_unlock_cb */


/*===========================================================================
  FUNCTION  QMI_UIM_PROCESS_LOCK_RESP

  DESCRIPTION
    Processes the response of the personalization command

  PARAMETERS
    status          : Status of the lock request
    user_data_ptr   : Pointer to the user data

  RETURN VALUE
    None

  DEPENDENCIES
    QMI UIM must already have been initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_uim_simlock_process_lock_resp
(
  simlock_result_enum_type       status,
  qmi_uimi_userdata_type       * user_data_ptr
)
{
  qmi_uim_cmd_buf_s                       *cmd_buf_p           = NULL;
  qmi_error_type_v01                       errval              = QMI_ERR_NONE_V01;
  uint32                                   client_reg_count    = 0;
  uint16                                   request_id          = 0;
  boolean                                  ck_present          = TRUE;
  uint16                                   cmd_id              = UIMI_CMD_VAL_WIDTH;
  qmi_client_handle                        cli_handle          = 0;
  qmi_req_handle                           req_handle;
  void*                                    ind_resp_ptr        = NULL;
  uint16                                   ind_resp_len        = 0;
  uint16                                   client_index        = 0;

  memset(&req_handle, 0x00, sizeof(req_handle));

  UIM_MSG_HIGH_0("qmi_uim_simlock_process_lock_resp");

  ASSERT(user_data_ptr);

  /* Retrieve parameters */
  cli_handle       = user_data_ptr->client_handle;
  req_handle       = user_data_ptr->req_handle;
  cmd_id           = user_data_ptr->request_id;
  client_reg_count = user_data_ptr->client_reg_count;
  request_id       = user_data_ptr->data.simlock_lock.request_id;
  ck_present       = user_data_ptr->data.simlock_lock.ck_present;

  if((status == SIMLOCK_SUCCESS) &&
     (user_data_ptr->data.simlock_lock.next_list_to_lock <
               user_data_ptr->data.simlock_lock.total_list_cnt) &&
     (user_data_ptr->data.simlock_lock.code_ptr != NULL))
  {
    simlock_result_enum_type                 simlock_status     = SIMLOCK_SUCCESS;
    uint8                                    list_to_lock       = 0;
    simlock_add_lock_codes_ck_msg_req_type   add_lock_codes;

    memset(&add_lock_codes, 0x00, sizeof(simlock_add_lock_codes_ck_msg_req_type));

    UIM_MSG_HIGH_0("Processing additional lock codes");

    list_to_lock = user_data_ptr->data.simlock_lock.next_list_to_lock;

    add_lock_codes.slot      = user_data_ptr->data.simlock_lock.slot;
    add_lock_codes.ck        = user_data_ptr->data.simlock_lock.ck;
    add_lock_codes.blacklist = FALSE;
    add_lock_codes.category  = user_data_ptr->data.simlock_lock.code_ptr[list_to_lock];

    user_data_ptr->data.simlock_lock.next_list_to_lock++;

    simlock_status = simlock_add_lock_codes_ck(add_lock_codes,
                                               qmi_uimi_simlock_callback,
                                               user_data_ptr);
    if(simlock_status == SIMLOCK_SUCCESS)
    {
      return;
    }
  }

  if(user_data_ptr->data.simlock_lock.next_list_to_lock == 1)
  {
    /* Indicates there was only one list to lock */
    errval = qmi_uim_simlock_convert_simlock_status_to_errval(status);
  }
  else if((user_data_ptr->data.simlock_lock.next_list_to_lock ==
             user_data_ptr->data.simlock_lock.total_list_cnt)  &&
          (status == SIMLOCK_SUCCESS))
  {
    /* Indicates all the lists were successfully locked */
    errval = QMI_ERR_NONE_V01;
  }
  else
  {
    /* Indicates some lists were added successfully,
       but then an error occurred */
    errval = QMI_ERR_OP_PARTIAL_FAILURE_V01;
  }

  /* First check if the requested client is still valid */
  if (FALSE == qmi_uim_util_check_clid(user_data_ptr->client_handle, user_data_ptr->client_reg_count, &client_index))
  {
    goto free_cmd_data;
  }

  cmd_buf_p = qmi_uim_util_get_cmd_buf_ptr(req_handle, cli_handle);
  if(cmd_buf_p == NULL)
  {
    goto free_cmd_data;
  }

  if(cmd_id == QMI_UIM_PERSONALIZATION_SECURE_REQ_V01)
  {
    uim_personalization_secure_resp_msg_v01 *secure_perso_resp_ptr = NULL;
    ind_resp_len = sizeof(uim_personalization_secure_resp_msg_v01);
    secure_perso_resp_ptr = (uim_personalization_secure_resp_msg_v01*) uimqmi_malloc(ind_resp_len);
    if(secure_perso_resp_ptr == NULL)
    {
      goto free_cmd_data;
    }
    ind_resp_ptr                           = secure_perso_resp_ptr;
    
    /* build the result and encrypt the data */
    secure_perso_resp_ptr->encrypted_resp_valid = 1;
    secure_perso_resp_ptr->encrypted_resp_len = sizeof(secure_perso_resp_ptr->encrypted_resp);
    errval = qmi_uim_response_encrypted_status(QMI_UIM_PERSONALIZATION_SECURE_REQ_V01,
                                               request_id,
                                               errval,
                                               secure_perso_resp_ptr->encrypted_resp,
                                               &secure_perso_resp_ptr->encrypted_resp_len);
    if(errval != QMI_ERR_NONE_V01)
    {
      /* Even if encrypted status fails, continue with sending
         response back to client so they are at least notified. */
      secure_perso_resp_ptr->encrypted_resp_valid = 0;
      UIM_MSG_HIGH_0("failed to build and encrypt response");
    }
    qmi_uim_util_populate_resp(errval, &secure_perso_resp_ptr->resp);
  }
  else if(cmd_id == QMI_UIM_PERSONALIZATION_REQ_V01)
  {
    uim_personalization_resp_msg_v01 *perso_resp_ptr = NULL;
    ind_resp_len = sizeof(uim_personalization_resp_msg_v01);
    perso_resp_ptr = (uim_personalization_resp_msg_v01*) uimqmi_malloc(ind_resp_len);
    if(perso_resp_ptr == NULL)
    {
      goto free_cmd_data;
    }
    ind_resp_ptr                           = perso_resp_ptr;
    qmi_uim_util_populate_resp(errval, &perso_resp_ptr->resp);
  }
  else
  {
    UIM_MSG_HIGH_1("unexpected response 0x%x", cmd_id);
    goto free_cmd_data;
  }

  /* Update result TLV and Send response */
  qmi_uim_send_response_ind( 
    cmd_buf_p,
    ind_resp_ptr,
    cmd_id,
    ind_resp_len,
    FALSE,
    cli_handle);

  uimqmi_free(ind_resp_ptr);
  
free_cmd_data:
  if(cmd_buf_p)
  {
    qmi_uim_util_remove_qmi_cmd(
      req_handle,
      cli_handle,
      &qmi_uim_state_ptr->client[client_index]->cmd_q);
  }
  
  /* Free userdata */
   qmi_uim_util_free_cb_userdata(user_data_ptr);
   user_data_ptr = NULL;
} /* qmi_uim_simlock_process_lock_resp */


/*===========================================================================
  FUNCTION  QMI_UIM_SIMLOCK_PROCESS_RELOCK_RESP

  DESCRIPTION
    Processes the response of the relock personalization command

  PARAMETERS
    status          : Status of the relock request
    user_data_ptr   : Pointer to the user data

  RETURN VALUE
    None

  DEPENDENCIES
    QMI UIM must already have been initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_uim_simlock_process_relock_resp
(
  simlock_result_enum_type                      status,
  const simlock_relock_device_msg_resp_type   * relock_rsp_ptr,
  qmi_uimi_userdata_type                      * user_data_ptr
)
{
  qmi_uim_cmd_buf_s               * cmd_buf_p         = NULL;
  qmi_error_type_v01                errval            = QMI_ERR_NONE_V01;
  uint16                            client_index      = 0;
  uim_personalization_resp_msg_v01 *perso_resp_ptr    = NULL;

  UIM_MSG_HIGH_0("qmi_uim_simlock_process_relock_resp");

  ASSERT(relock_rsp_ptr && user_data_ptr);

  /* First check if the requested client is still valid */
  if (FALSE == qmi_uim_util_check_clid(user_data_ptr->client_handle, user_data_ptr->client_reg_count, &client_index))
  {
    goto free_cmd_data;
  }

  cmd_buf_p = qmi_uim_util_get_cmd_buf_ptr(user_data_ptr->req_handle, user_data_ptr->client_handle);
  if(cmd_buf_p == NULL)
  {
    goto free_cmd_data;
  }

  perso_resp_ptr = (uim_personalization_resp_msg_v01*) uimqmi_malloc(sizeof(uim_personalization_resp_msg_v01));
  if(perso_resp_ptr == NULL)
  {
    goto free_cmd_data;
  }

  /* For relock, retries are sent in these cases */
  if ((status == SIMLOCK_SUCCESS)       ||
      (status == SIMLOCK_INCORRECT_KEY) ||
      (status == SIMLOCK_MAX_RETRIES_REACHED))
  {
    uint32   num_retries = 0;

    if (relock_rsp_ptr->num_retries_max == 0)
    {
      num_retries = QMI_UIM_MAX_RETRIES;
    }
    else if (relock_rsp_ptr->curr_retries >= relock_rsp_ptr->num_retries_max)
    {
      num_retries = 0;
    }
    else
    {
      num_retries = relock_rsp_ptr->num_retries_max - relock_rsp_ptr->curr_retries;
    }

    /* Send the num_retries in the response */
    qmi_uim_response_retries_left(
                  &perso_resp_ptr->retries_left.unblock_left,
                  &perso_resp_ptr->retries_left.verify_left,
                  num_retries,
                  0);
    perso_resp_ptr->retries_left_valid = 1;
    
  }

  errval = qmi_uim_simlock_convert_simlock_status_to_errval(status);

  qmi_uim_util_populate_resp(errval, &perso_resp_ptr->resp);
  
  qmi_uim_send_response_ind( 
    cmd_buf_p,
    perso_resp_ptr,
    user_data_ptr->request_id,
    sizeof(*perso_resp_ptr),
    FALSE,
    user_data_ptr->client_handle);

  uimqmi_free(perso_resp_ptr);

free_cmd_data:
  if(cmd_buf_p)
  {
    qmi_uim_util_remove_qmi_cmd(
      user_data_ptr->req_handle,
      user_data_ptr->client_handle,
      &qmi_uim_state_ptr->client[client_index]->cmd_q);
  }

  /* Free userdata */
  qmi_uim_util_free_cb_userdata(user_data_ptr);
  user_data_ptr = NULL;
} /* qmi_uim_simlock_process_relock_resp */


/*===========================================================================
  FUNCTION  QMI_UIM_SIMLOCK_PROCESS_SET_DEVICE_MODE_RESP

  DESCRIPTION
    Processes the response of the set device mode command

  PARAMETERS
    status          : Status of the set device mode request
    user_data_ptr   : Pointer to the user data

  RETURN VALUE
    None

  DEPENDENCIES
    QMI UIM must already have been initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_uim_simlock_process_set_device_mode_resp
(
  simlock_result_enum_type       status,
  qmi_uimi_userdata_type       * user_data_ptr
)
{
  qmi_uim_cmd_buf_s                *cmd_buf_p       = NULL;
  qmi_error_type_v01                errval          = QMI_ERR_NONE_V01;
  uint16                            client_index    = 0;
  uint16                            request_id      = 0;
  uim_emergency_only_resp_msg_v01  *resp_ptr        = NULL;

  ASSERT( user_data_ptr );

  /* Retrieve parameters */
  request_id       = user_data_ptr->data.simlock_set_device_mode.request_id;

  UIM_MSG_HIGH_0("qmi_uim_process_set_device_mode_resp");

  /* First check if the requested client is still valid */
  if (FALSE == qmi_uim_util_check_clid(user_data_ptr->client_handle, user_data_ptr->client_reg_count, &client_index))
  {
    goto free_cmd_data;
  }

  cmd_buf_p = qmi_uim_util_get_cmd_buf_ptr(user_data_ptr->req_handle, user_data_ptr->client_handle);
  if(cmd_buf_p == NULL)
  {
    goto free_cmd_data;
  }

  resp_ptr = (uim_emergency_only_resp_msg_v01*) uimqmi_malloc(sizeof(uim_emergency_only_resp_msg_v01));
  if(resp_ptr == NULL)
  {
    goto free_cmd_data;
  }

  errval = qmi_uim_simlock_convert_simlock_status_to_errval(status);

  resp_ptr->encrypted_resp_len = sizeof(resp_ptr->encrypted_resp);
  resp_ptr->encrypted_resp_valid = 1;
  /* build the result and encrypt the data */
  errval = qmi_uim_response_encrypted_status(QMI_UIM_EMERGENCY_ONLY_REQ_V01,
                                             request_id,
                                             errval,
                                             resp_ptr->encrypted_resp,
                                            &resp_ptr->encrypted_resp_len);
  if(errval != QMI_ERR_NONE_V01)
  {
    resp_ptr->encrypted_resp_valid = 0;
  }
  
  qmi_uim_util_populate_resp(errval, &resp_ptr->resp);

  (void)qmi_uim_send_response_ind( 
          cmd_buf_p,
          resp_ptr,
          QMI_UIM_EMERGENCY_ONLY_REQ_V01,
          sizeof(uim_emergency_only_resp_msg_v01),
          FALSE,
          user_data_ptr->client_handle);

  uimqmi_free(resp_ptr);

free_cmd_data:
  if(cmd_buf_p)
  {
    qmi_uim_util_remove_qmi_cmd(
      user_data_ptr->req_handle,
      user_data_ptr->client_handle,
      &qmi_uim_state_ptr->client[client_index]->cmd_q);
  }

  /* Free userdata */
  qmi_uim_util_free_cb_userdata(user_data_ptr);
  user_data_ptr = NULL;
} /* qmi_uim_simlock_process_set_device_mode_resp */


/*===========================================================================
  FUNCTION  QMI_UIM_SIMLOCK_PROCESS_SET_REMOTE_SFS_RESP

  DESCRIPTION
    Processes the response of the set remote SFS command

  PARAMETERS
    status          : Status of the set remote SFS request
    user_data_ptr   : Pointer to the user data

  RETURN VALUE
    None

  DEPENDENCIES
    QMI UIM must already have been initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_simlock_process_set_remote_sfs_resp
(
  simlock_result_enum_type       status,
  qmi_uimi_userdata_type       * user_data_ptr
)
{
  qmi_uim_cmd_buf_s                     * cmd_buf_p           = NULL;
  qmi_error_type_v01                      errval              = QMI_ERR_NONE_V01;
  uint16                                  request_id          = 0;
  uint16                                  cmd_id              = UIMI_CMD_VAL_WIDTH;
  uim_simlock_configuration_resp_msg_v01 *config_resp_ptr     = NULL;
  uint16                                  client_index        = 0;
 
  ASSERT( user_data_ptr );

  /* Retrieve parameters */
  cmd_id           = user_data_ptr->request_id;
  request_id       = user_data_ptr->data.simlock_set_remote_sfs.request_id;

  /* First check if the requested client is still valid */
  if (FALSE == qmi_uim_util_check_clid(user_data_ptr->client_handle, user_data_ptr->client_reg_count, &client_index))
  {
    goto free_cmd_data;
  }

  cmd_buf_p = qmi_uim_util_get_cmd_buf_ptr(user_data_ptr->req_handle, user_data_ptr->client_handle);
  if(cmd_buf_p == NULL)
  {
    goto free_cmd_data;
  }

  errval = qmi_uim_simlock_convert_simlock_status_to_errval(status);
  
  config_resp_ptr = uimqmi_malloc(sizeof(uim_simlock_configuration_resp_msg_v01));
  if(config_resp_ptr == NULL)
  {
    goto free_cmd_data;
  }
  
  config_resp_ptr->encrypted_resp_len = sizeof(config_resp_ptr->encrypted_resp);
  /* build the result and encrypt the data */
  config_resp_ptr->encrypted_resp_valid = 1;
  errval = qmi_uim_response_encrypted_status(QMI_UIM_SIMLOCK_CONFIGURATION_REQ_V01,
                                             request_id,
                                             errval,
                                             config_resp_ptr->encrypted_resp,
                                             &config_resp_ptr->encrypted_resp_len);

  if(errval != QMI_ERR_NONE_V01)
  {
    config_resp_ptr->encrypted_resp_valid = 0;
  }
  
  qmi_uim_util_populate_resp(errval, &config_resp_ptr->resp);

  qmi_uim_send_response_ind( 
    cmd_buf_p,
    config_resp_ptr,
    QMI_UIM_SIMLOCK_CONFIGURATION_REQ_V01,
    sizeof(uim_simlock_configuration_resp_msg_v01),
    FALSE,
    user_data_ptr->client_handle);

  uimqmi_free(config_resp_ptr);

free_cmd_data:
  if(cmd_buf_p)
  {
    qmi_uim_util_remove_qmi_cmd(
      user_data_ptr->req_handle,
      user_data_ptr->client_handle,
      &qmi_uim_state_ptr->client[client_index]->cmd_q);
  }

  /* Free userdata */
  qmi_uim_util_free_cb_userdata(user_data_ptr);
  user_data_ptr = NULL;
} /* qmi_uim_simlock_process_set_remote_sfs_resp */


/*===========================================================================
  FUNCTION  QMI_UIM_SIMLOCK_PROCESS_REMOTE_UNLOCK_RESP

  DESCRIPTION
    Processes the response of the remote unlock command

  PARAMETERS
    status             : Status of the remote unlock request
    encrypted_key_ptr  : Pointer to encrypted key data
    user_data_ptr      : Pointer to the user data

  RETURN VALUE
    None

  DEPENDENCIES
    QMI UIM must already have been initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_simlock_process_remote_unlock_resp
(
  simlock_result_enum_type         status,
  simlock_data_type             *  encrypted_key_ptr,
  qmi_uimi_userdata_type        *  user_data_ptr
)
{
  qmi_uim_cmd_buf_s              * cmd_buf_p                 = NULL;
  qmi_error_type_v01               errval                    = QMI_ERR_NONE_V01;
  uim_remote_unlock_resp_msg_v01 * remote_unlock_resp_ptr    = NULL;
  uint16                           client_index              = 0;
  
  ASSERT(user_data_ptr);
  ASSERT(encrypted_key_ptr);

  /* First check if the requested client is still valid */
  if (FALSE == qmi_uim_util_check_clid(user_data_ptr->client_handle, user_data_ptr->client_reg_count, &client_index))
  {
    goto free_cmd_data;
  }

  cmd_buf_p = qmi_uim_util_get_cmd_buf_ptr(user_data_ptr ->req_handle, user_data_ptr ->client_handle);
  if(cmd_buf_p == NULL)
  {
    goto free_cmd_data;
  }

  remote_unlock_resp_ptr = (uim_remote_unlock_resp_msg_v01*) uimqmi_malloc(sizeof(uim_remote_unlock_resp_msg_v01));
  if(remote_unlock_resp_ptr == NULL)
  {
    goto free_cmd_data;
  }

  errval = qmi_uim_simlock_convert_simlock_status_to_errval(status);

  if((status == SIMLOCK_SUCCESS) &&
     (encrypted_key_ptr->data_len > 0) &&
     (encrypted_key_ptr->data_ptr != NULL))
  {
    mmgsdi_data_type    key_data = { 0, NULL };

    key_data.data_ptr = encrypted_key_ptr->data_ptr;
    key_data.data_len = (int32)encrypted_key_ptr->data_len;
    if (FALSE == qmi_uim_util_populate_opaque(
                                  &remote_unlock_resp_ptr->encrypted_key_valid,
                                  remote_unlock_resp_ptr->encrypted_key,
                                  &remote_unlock_resp_ptr->encrypted_key_len,
                                  sizeof(remote_unlock_resp_ptr->encrypted_key),
                                  key_data.data_ptr,
                                  key_data.data_len,
                                  TRUE))
    {
      errval = QMI_ERR_NO_MEMORY_V01;
    }
  }

  qmi_uim_util_populate_resp(errval, &remote_unlock_resp_ptr->resp);

  qmi_uim_send_response_ind( 
    cmd_buf_p,
    remote_unlock_resp_ptr,
    user_data_ptr->request_id,
    sizeof(uim_remote_unlock_resp_msg_v01),
    FALSE,
    user_data_ptr->client_handle);

  uimqmi_free(remote_unlock_resp_ptr);

free_cmd_data:
  if(encrypted_key_ptr->data_ptr != NULL)
  {
    uimqmi_free(encrypted_key_ptr->data_ptr);
  }
  encrypted_key_ptr->data_ptr = NULL;

  if(cmd_buf_p)
  {
    qmi_uim_util_remove_qmi_cmd(
      user_data_ptr->req_handle,
      user_data_ptr->client_handle,
      &qmi_uim_state_ptr->client[client_index]->cmd_q);
  }

  /* Free userdata */
  qmi_uim_util_free_cb_userdata(user_data_ptr);
  user_data_ptr = NULL;
} /* qmi_uim_simlock_process_remote_unlock_resp */


/*===========================================================================
  FUNCTION  QMI_UIM_SIMLOCK_PROCESS_SIMLOCK_CB

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
void qmi_uim_simlock_process_simlock_cb
(
  qmi_uim_message_type * uim_message_ptr
)
{
  qmi_uimi_userdata_type * cb_userdata_ptr = NULL;

  ASSERT(uim_message_ptr);

  UIM_MSG_HIGH_1("qmi_uim_simlock_process_simlock_cb, msg_type: 0x%x",
                 uim_message_ptr->data.simlock.data.msg_type);

  switch (uim_message_ptr->data.simlock.data.msg_type)
  {
    case SIMLOCK_UNLOCK_DEVICE_MSG:
    case SIMLOCK_TEMPORARY_UNLOCK_MSG:
      qmi_uim_simlock_process_unlock_resp(uim_message_ptr->data.simlock.status,
                                          &uim_message_ptr->data.simlock.data.message.unlock_resp,
                                          (qmi_uimi_userdata_type*)uim_message_ptr->data.simlock.user_data_ptr);
      break;

    case SIMLOCK_GET_STATUS_MSG:
      cb_userdata_ptr = (qmi_uimi_userdata_type*)uim_message_ptr->data.simlock.user_data_ptr;
      if (cb_userdata_ptr == NULL)
      {
        return;
      }
      if(cb_userdata_ptr->request_id == QMI_UIM_GET_CONFIGURATION_REQ_V01)
      {
        qmi_uim_simlock_process_get_status_resp(uim_message_ptr->data.simlock.status,
                                                &uim_message_ptr->data.simlock.data.message.get_status_resp,
                                                (qmi_uimi_userdata_type*)uim_message_ptr->data.simlock.user_data_ptr);
      }
      else if (cb_userdata_ptr->request_id == QMI_UIM_DEPERSONALIZATION_REQ_V01)
      {
        qmi_uim_simlock_process_get_status_resp_after_unlock(uim_message_ptr->data.simlock.status,
                                                             &uim_message_ptr->data.simlock.data.message.get_status_resp,
                                                             (qmi_uimi_userdata_type*)uim_message_ptr->data.simlock.user_data_ptr);
      }
      break;

    case SIMLOCK_SET_LOCK_CK_MSG:
    case SIMLOCK_ADD_LOCK_CODES_CK_MSG:
      qmi_uim_simlock_process_lock_resp(uim_message_ptr->data.simlock.status,
                                        (qmi_uimi_userdata_type*)uim_message_ptr->data.simlock.user_data_ptr);
      break;

    case SIMLOCK_SET_DEVICE_MODE_MSG:
      qmi_uim_simlock_process_set_device_mode_resp(uim_message_ptr->data.simlock.status,
                                                   (qmi_uimi_userdata_type*)uim_message_ptr->data.simlock.user_data_ptr);
      break;

    case SIMLOCK_RELOCK_DEVICE_MSG:
      qmi_uim_simlock_process_relock_resp(uim_message_ptr->data.simlock.status,
                                          &uim_message_ptr->data.simlock.data.message.relock_resp,
                                         (qmi_uimi_userdata_type*)uim_message_ptr->data.simlock.user_data_ptr);
      break;

    default:
      break;
  }
} /* qmi_uim_simlock_process_simlock_cb */


/*===========================================================================
  FUNCTION QMI_UIM_SIMLOCK_PROCESS_REMOTE_SFS_CB()

  DESCRIPTION
    This function processes the callback from simlock task. It indicates
    either the simlock data needs to be read from or write to a remote
    storage location to all registered QMI clients.

  PARAMETERS
    uim_message_ptr: QMI UIM message

  RETURN VALUE
    None

  DEPENDENCIES
    QMI UIM must already have been initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_simlock_process_remote_sfs_cb
(
  const qmi_uim_message_type * uim_message_ptr
)
{
  boolean is_indication_sent = FALSE;

  ASSERT(uim_message_ptr != NULL);
  ASSERT(qmi_uim_global_ptr);

  is_indication_sent = qmi_uim_simlock_send_remote_sfs_operation_indication_to_all(uim_message_ptr);

  if (is_indication_sent)
  {
    /* Save the simlock cb. In case the simlock cb is not NULL,
       indicating a response from TZ is pending, we ignore and
       overwrite it. The expectation is the TZ should respond
       to each operation request in requested order.*/
    qmi_uim_global_ptr->simlock_remote_sfs_operation_resp_cb =
      uim_message_ptr->data.simlock_remote_sfs_req.simlock_resp_cb;
  }
  else
  {
    UIM_MSG_ERR_0("simlock_remote_sfs indication was not sent");

    /* No indication was sent, invoke the callback with error status */
    if (uim_message_ptr->data.simlock_remote_sfs_req.simlock_resp_cb)
    {
      uim_message_ptr->data.simlock_remote_sfs_req.simlock_resp_cb(SIMLOCK_GENERIC_ERROR,
                                                                   uim_message_ptr->data.simlock_remote_sfs_req.request_token_id,
                                                                   NULL);
    }
  }

  if (uim_message_ptr->data.simlock_remote_sfs_req.simlock_data.data_ptr != NULL)
  {
    uimqmi_free(uim_message_ptr->data.simlock_remote_sfs_req.simlock_data.data_ptr);
  }
} /* qmi_uim_simlock_process_remote_sfs_cb */


/*===========================================================================
  FUNCTION QMI_UIM_SIMLOCK_PROCESS_TEMPORARY_UNLOCK_EVT()

  DESCRIPTION
    This function processes the Temporary unlock event from MMGSDI

  PARAMETERS
    evt_ptr  : Contains the event information sent by MMGSDI

  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_simlock_process_temporary_unlock_event(
  const mmgsdi_event_data_type *evt_ptr
)
{
  simlock_result_enum_type   simlock_status = SIMLOCK_GENERIC_ERROR;
  if(evt_ptr == NULL ||
     evt_ptr->evt != MMGSDI_PERSO_TEMPORARY_UNLOCK_EVT)
  {
    return;
  }

  UIM_MSG_MED_2("qmi_uim_simlock_process_temporary_unlock_event slot 0x%x feature 0x%x",
                 evt_ptr->data.perso_temp_unlock.slot_id,
                 evt_ptr->data.perso_temp_unlock.feature);

  /* Get the current lock status from SIMLOCK */
  simlock_status = simlock_get_status(qmi_uimi_simlock_temporary_unlock_callback,
                                      NULL);
  if(SIMLOCK_SUCCESS != simlock_status)
  {
    UIM_MSG_MED_1("Failed to queue simlock get status command 0x%x", simlock_status);
  }
} /* qmi_uim_simlock_process_temporary_unlock_event */


/*===========================================================================
  FUNCTION QMI_UIMI_DEPERSONALIZATION()

  DESCRIPTION
    Deactivates or Unblocks depersonalization

  PARAMETERS
    cl_sp     : client state pointer
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
qmi_csi_error qmi_uimi_depersonalization
(
  qmi_uimi_client_state_type  * cl_sp,
  qmi_req_handle                req_handle,
  void                        * req_c_struct,
  int                           req_c_struct_len
)
{
  qmi_error_type_v01                    errval           = QMI_ERR_NONE_V01;
  qmi_uimi_userdata_type*             cb_userdata_ptr  = NULL;
  simlock_result_enum_type            simlock_status   = SIMLOCK_SUCCESS;
  simlock_unlock_device_msg_req_type  unlock_msg;
  mmgsdi_slot_id_enum_type            slot             = MMGSDI_MAX_SLOT_ID_ENUM;
  qmi_uim_tlv_depersonalization_type  depersonalization;
  uim_depersonalization_req_msg_v01  *deperso_req_ptr  = NULL;
  uim_depersonalization_resp_msg_v01 *deperso_resp_ptr = NULL;
  uint16                              client_index     = 0;
  qmi_uim_cmd_buf_s                  *cmd_buf_p        = NULL;
  qmi_csi_error                       csi_errval       = QMI_CSI_NO_ERR;

  memset(&depersonalization, 0, sizeof(depersonalization));
  memset(&unlock_msg, 0, sizeof(unlock_msg));

  ASSERT(cl_sp && req_c_struct && req_c_struct_len > 0 && qmi_uim_state_ptr);

  if(!qmi_uim_util_get_cli_index(cl_sp->client_handle, &client_index))
  {
    return QMI_CSI_INVALID_HANDLE;
  }

  cmd_buf_p = qmi_uim_util_get_cmd_buf_ptr(req_handle, cl_sp->client_handle);

  ASSERT(cmd_buf_p);

  deperso_req_ptr = (uim_depersonalization_req_msg_v01 *)req_c_struct;
  cmd_buf_p->req_handle = req_handle;  

  errval = qmi_uim_tlv_depersonalization(&deperso_req_ptr->depersonalization,
                                         sizeof(deperso_req_ptr),
                                         &depersonalization);
  if (errval != QMI_ERR_NONE_V01)
  {
    goto send_result;
  }

  if (deperso_req_ptr->slot_valid)
  {
    errval = qmi_uim_map_and_validate_slot(&slot, deperso_req_ptr->slot);
    if(errval != QMI_ERR_NONE_V01)
    {  
      goto send_result;
    }
  }
  unlock_msg.category  = depersonalization.category;
  unlock_msg.ck        = depersonalization.ck;

  if (deperso_req_ptr->slot_valid)
  {
    if(qmi_uim_simlock_mmgsdi_slot_id_to_simlock_slot_id(slot,
                                                         &unlock_msg.slot) != QMI_ERR_NONE_V01)
    {
      errval = QMI_ERR_INTERNAL_V01;
      goto send_result;
    }
  }
  else
  {
    /* default to slot-1 */
    unlock_msg.slot = SIMLOCK_SLOT_1;
  }

  /* Create userdata */
  cb_userdata_ptr = qmi_uim_util_get_cb_userdata(QMI_UIM_DEPERSONALIZATION_REQ_V01,
                                                 cmd_buf_p,
                                                 NULL,
                                                 cl_sp->client_handle);
  if (cb_userdata_ptr == NULL)
  {
    errval = QMI_ERR_NO_MEMORY_V01;
    goto send_result;
  }

  cb_userdata_ptr->data.simlock_unlock.category      = unlock_msg.category;
  cb_userdata_ptr->data.simlock_unlock.slot          = unlock_msg.slot;
  cb_userdata_ptr->data.simlock_unlock.ck_present    = TRUE;
  /* Set request id to dummy value as it will not be used for regular depersonalization */
  cb_userdata_ptr->data.simlock_unlock.request_id    = 0;

  /* Unlock the SIM */
  simlock_status = simlock_unlock(unlock_msg,
                                  qmi_uimi_simlock_callback,
                                  cb_userdata_ptr);

  /* Free userdata in case of error */
  QMI_UIM_SIMLOCK_FREE_USERDATA_IF_ERROR(simlock_status, cb_userdata_ptr);

  /* Convert MMGSDI status into QMI err code */
  errval = qmi_uim_simlock_convert_simlock_status_to_errval(simlock_status);

send_result:
  if(errval == QMI_ERR_NONE_V01)
  {
    cmd_buf_p->resp_pending = TRUE;
    return QMI_CSI_NO_ERR;
  }

  /* Free user data in case of error */
  qmi_uim_util_free_cb_userdata(cb_userdata_ptr);

  deperso_resp_ptr =   uimqmi_malloc(sizeof(uim_depersonalization_resp_msg_v01));
  if(!deperso_resp_ptr)
  {
    return QMI_CSI_NO_MEM;
  }

  qmi_uim_util_populate_resp(errval, &deperso_resp_ptr->resp);

  csi_errval = qmi_uim_send_immidiate_resp(cmd_buf_p, 
                                           deperso_resp_ptr,
                                           cmd_buf_p->cmd_type,
                                           sizeof(uim_depersonalization_resp_msg_v01),
                                           cl_sp->client_handle);
  uimqmi_free(deperso_resp_ptr);

return csi_errval;
} /* qmi_uimi_depersonalization() */


/*===========================================================================
  FUNCTION QMI_UIMI_PERSONALIZATION()

  DESCRIPTION
    Activates the personalization of the device to a particular lock feature.
    Only one feature can be activated at a time

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
qmi_csi_error qmi_uimi_personalization
(
  qmi_uimi_client_state_type  * cl_sp,
  qmi_req_handle                req_handle,
  void                        * req_c_struct,
  int                           req_c_struct_len
)
{
  qmi_error_type_v01                        errval               = QMI_ERR_NONE_V01;
  qmi_uimi_userdata_type                   *cb_userdata_ptr      = NULL;
  simlock_result_enum_type                  simlock_status       = SIMLOCK_GENERIC_ERROR;
  simlock_slot_enum_type                    simlock_slot         = SIMLOCK_SLOT_1;
  uint8                                     index                = 0;
  uint8                                     code_index           = 0;
  uint8                                     loop_cnt             = 0;
  uint8                                     table_indexes[]      = {1, 2, 3, 4, 5, 6, 7, 8};
  mmgsdi_slot_id_enum_type                  mmgsdi_slot          = MMGSDI_MAX_SLOT_ID_ENUM;
  uim_personalization_req_msg_v01          *perso_req_ptr        = NULL;
  uim_personalization_resp_msg_v01         *perso_resp_ptr       = NULL;
  uint16                                    client_index         = 0;
  qmi_uim_cmd_buf_s                        *cmd_buf_p            = NULL;
  qmi_csi_error                             csi_errval           = QMI_CSI_NO_ERR;
  simlock_ck_data_type                      ck_data              = {0,};
  void                                     *code_data_ptr        = NULL;
  uint32                                    num_code             = 0;
  simlock_category_enum_type                category             = SIMLOCK_CATEGORY_3GPP_NW;    
   qmi_uim_tlv_personalization_code_type     personalization_code = {0, NULL};
  
  ASSERT(cl_sp && req_c_struct && req_c_struct_len > 0 && qmi_uim_state_ptr);

  if(!qmi_uim_util_get_cli_index(cl_sp->client_handle, &client_index))
  {
    return QMI_CSI_INVALID_HANDLE;
  }

  cmd_buf_p = qmi_uim_util_get_cmd_buf_ptr(req_handle, cl_sp->client_handle);

  ASSERT(cmd_buf_p);

  perso_req_ptr = (uim_personalization_req_msg_v01 *)req_c_struct;
  cmd_buf_p->req_handle = req_handle;  

  /* Validate ck length */
  if (perso_req_ptr->ck_value_len == 0 ||
      perso_req_ptr->ck_value_len > QMI_UIM_CK_MAX_V01)
  {
    errval = QMI_ERR_ARG_TOO_LONG_V01;
    goto send_result;
  }

  /* populate Control key value */
  ck_data.simlock_ck_length = perso_req_ptr->ck_value_len;
  (void)memscpy((void*)ck_data.simlock_ck_data,
                sizeof(ck_data.simlock_ck_data),
                (void*)perso_req_ptr->ck_value,
                (uint32)perso_req_ptr->ck_value_len);

  /* Create userdata */
  cb_userdata_ptr = qmi_uim_util_get_cb_userdata(QMI_UIM_PERSONALIZATION_REQ_V01,
                                                 cmd_buf_p,
                                                 NULL,
                                                 cl_sp->client_handle);
  if (cb_userdata_ptr == NULL)
  {
    errval = QMI_ERR_NO_MEMORY_V01;
    goto send_result;
  }

  /* Extract slot if present */
  if(perso_req_ptr->slot_valid)
  {
    errval = qmi_uim_map_and_validate_slot(&mmgsdi_slot, perso_req_ptr->slot);
    if(errval != QMI_ERR_NONE_V01)
    {
      goto send_result;
    }
    if(qmi_uim_simlock_mmgsdi_slot_id_to_simlock_slot_id(mmgsdi_slot,
                                                         &simlock_slot) != QMI_ERR_NONE_V01)
    {
      errval = QMI_ERR_INTERNAL_V01;
      goto send_result;
    }
  }

  if(perso_req_ptr->feature_gw_network_perso_valid)
  {
    code_index++;
    category      = SIMLOCK_CATEGORY_3GPP_NW; 
    num_code      = perso_req_ptr->feature_gw_network_perso_len;
    code_data_ptr = perso_req_ptr->feature_gw_network_perso;
  }

  if(perso_req_ptr->feature_gw_network_subset_perso_valid)
  {
    code_index++;
    category      = SIMLOCK_CATEGORY_3GPP_NS; 
    num_code      = perso_req_ptr->feature_gw_network_subset_perso_len;
    code_data_ptr = perso_req_ptr->feature_gw_network_subset_perso;
  }

  if(perso_req_ptr->feature_gw_sp_perso_valid)
  {
    code_index++;
    category      = SIMLOCK_CATEGORY_3GPP_SP; 
    num_code      = perso_req_ptr->feature_gw_sp_perso_len;
    code_data_ptr = perso_req_ptr->feature_gw_sp_perso;
  }

  if(perso_req_ptr->feature_gw_corporate_perso_valid)
  {
    code_index++;
    category      = SIMLOCK_CATEGORY_3GPP_CP; 
    num_code      = perso_req_ptr->feature_gw_corporate_perso_len;
    code_data_ptr = perso_req_ptr->feature_gw_corporate_perso;
  }

  if(perso_req_ptr->feature_gw_sim_perso_valid)
  {
    code_index++;
    category      = SIMLOCK_CATEGORY_3GPP_SIM; 
    num_code      = perso_req_ptr->feature_gw_sim_perso_len;
    code_data_ptr = perso_req_ptr->feature_gw_sim_perso;
  }

  if(perso_req_ptr->feature_1x_network1_perso_valid )
  {
    code_index++;
    category      = SIMLOCK_CATEGORY_3GPP2_NW_TYPE1; 
    num_code      = perso_req_ptr->feature_1x_network1_perso_len;
    code_data_ptr = perso_req_ptr->feature_1x_network1_perso;
  }

  if(perso_req_ptr->feature_1x_network2_perso_valid )
  {
    code_index++;
    category      = SIMLOCK_CATEGORY_3GPP2_NW_TYPE2; 
    num_code      = perso_req_ptr->feature_1x_network2_perso_len;
    code_data_ptr = perso_req_ptr->feature_1x_network2_perso;
  }

  if(perso_req_ptr->feature_1x_ruim_perso_valid)
  {
    code_index++;
    category      = SIMLOCK_CATEGORY_3GPP2_RUIM; 
    num_code      = perso_req_ptr->feature_1x_ruim_perso_len;
    code_data_ptr = perso_req_ptr->feature_1x_ruim_perso;
  }

  if(code_index >1)
  {
    errval = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
    goto send_result;
  }

#ifdef FEATURE_QMI_UIM_PERSONALIZATION
  /* If personalization is allowed, we can either:
     Activate SIMLock - by receiving only 1 Lock code TLV per request, or
     Re-activate SIMLock - by receiving a reactivate TLV & no Lock code TLVs */
  if ((code_index != 0 && perso_req_ptr->reactivate_feature_valid) ||
      (code_index == 0 && !perso_req_ptr->reactivate_feature_valid))
  {
    errval = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
    goto send_result;
  }
  if(code_index == 1 && (code_data_ptr == NULL || num_code == 0))
  {
    errval = QMI_ERR_INVALID_ARG_V01;  
    goto send_result;
  }
#else
  /* If personalization is not allowed, we can only do a Relock functionality
     by receiving a reactivate TLV & no Lock code TLVs */
  if (code_index != 0 || !perso_req_ptr->reactivate_feature_valid)
  {
    errval = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
    goto send_result;
  }
#endif /* FEATURE_QMI_UIM_PERSONALIZATION */

  /* Unless the perso feature is defined, this request is unsupported */
  errval = QMI_ERR_NOT_SUPPORTED_V01;

  if (perso_req_ptr->reactivate_feature_valid)
  {
    simlock_relock_device_msg_req_type   relock_msg;

    memset(&relock_msg, 0x00, sizeof(simlock_relock_device_msg_req_type));

    /* Update category */
    errval = qmi_uim_convert_category_to_simlock_category(
               perso_req_ptr->reactivate_feature,
               &relock_msg.category_type);
    if(errval != QMI_ERR_NONE_V01)
    {
      goto send_result;
    }

    relock_msg.ck            = ck_data;
    relock_msg.slot          = simlock_slot;
 
    /* Send the relock API call */
    simlock_status = simlock_relock(relock_msg,
                                    qmi_uimi_simlock_callback,
                                    cb_userdata_ptr);

    /* Convert MMGSDI status into QMI err code */
    errval = qmi_uim_simlock_convert_simlock_status_to_errval(simlock_status);
  }
#ifdef FEATURE_QMI_UIM_PERSONALIZATION
  else
  {
    simlock_set_lock_ck_msg_req_type     lock_ck_msg;

    memset(&lock_ck_msg, 0x00, sizeof(simlock_set_lock_ck_msg_req_type));

    lock_ck_msg.ck          = ck_data;
    errval = qmi_uim_tlv_personalization_code(code_data_ptr,
                                              num_code,
                                              category,
                                              &personalization_code);
      if(errval != QMI_ERR_NONE_V01 || personalization_code.code_ptr == NULL)
      {
        goto send_result;
      }
      (void)memscpy(&lock_ck_msg.category,
                    sizeof(simlock_category_data_type),
                    personalization_code.code_ptr,
                    sizeof(simlock_category_data_type));

    lock_ck_msg.auto_lock   = FALSE;
    lock_ck_msg.blacklist   = FALSE;
    lock_ck_msg.slot        = simlock_slot;

    /* If the TLV is missing, the SIMLock engine sets a default
       value to the iteration count */
    if(perso_req_ptr->num_iterations_valid)
    {
      lock_ck_msg_ptr->iteration_cnt = perso_req_ptr->num_iterations;
    }

    /* If the TLV is missing, unlimited number of unlock retry
       attempts are allowed */
    if(perso_req_ptr->num_retries_valid)
    {
      lock_ck_msg_ptr->num_retries_max  = perso_req_ptr->num_retries;
    }

    /* Set request id to dummy value as it will not be used for regular personalization */
    cb_userdata_ptr->data.simlock_lock.ck_present = TRUE;

    /* Initialize the list counts */
    cb_userdata_ptr->data.simlock_lock.total_list_cnt = 0;
    cb_userdata_ptr->data.simlock_lock.next_list_to_lock = 0;

    if (!perso_req_ptr->reuse_code_for_feature_valid)
    {
      /* Set request id to dummy value as it will not be used for regular personalization */
      cb_userdata_ptr->data.simlock_lock.request_id           = 0;
      cb_userdata_ptr->data.simlock_lock.total_list_cnt       = personalization_code.num_of_lists;
      cb_userdata_ptr->data.simlock_lock.slot                 = lock_ck_msg_ptr->slot;
      cb_userdata_ptr->data.simlock_lock.ck                   = ck_data;

      /* The lock codes contained in zeroth list are locked here. So
       mark the next list to lock as 1 */
      cb_userdata_ptr->data.simlock_lock.next_list_to_lock    = 1;

      /* Save the lock code data pointer */
      cb_userdata_ptr->data.simlock_lock.code_ptr             = personalization_code.code_ptr;

    /* set the original pointer to NULL */
    personalization_code.code_ptr = NULL;

    /* lock the SIM */
    simlock_status = simlock_ck_lock(lock_ck_msg,
                                     qmi_uimi_simlock_callback,
                                     cb_userdata_ptr);

    /* Convert MMGSDI status into QMI err code */
    errval = qmi_uim_simlock_convert_simlock_status_to_errval(simlock_status);
  }
#endif /* FEATURE_QMI_UIM_PERSONALIZATION */

send_result:
  if(errval == QMI_ERR_NONE_V01)
  {
    cmd_buf_p->resp_pending = TRUE;
    return QMI_CSI_NO_ERR;
  }

  /* Free userdata in case of error */
  qmi_uim_util_free_cb_userdata(cb_userdata_ptr);

  perso_resp_ptr =   uimqmi_malloc(sizeof(uim_personalization_resp_msg_v01));
  if(!perso_resp_ptr)
  {
    return QMI_CSI_NO_MEM;
  }
  
  qmi_uim_util_populate_resp(errval, &perso_resp_ptr->resp);

  csi_errval = qmi_uim_send_immidiate_resp(cmd_buf_p, 
                                           perso_resp_ptr,
                                           cmd_buf_p->cmd_type,
                                           sizeof(uim_personalization_resp_msg_v01),
                                           cl_sp->client_handle);
  uimqmi_free(perso_resp_ptr);
 
  return csi_errval;
} /* qmi_uimi_personalization() */


/*===========================================================================
  FUNCTION QMI_UIMI_DEPERSONALIZATION_SECURE()

  DESCRIPTION
    Deactivates personalization of the device to a particular lock feature
    without passing in a CK. The input TLV is encrypted and must be decrypted
    before processing to insure a safe connection with client.
    Only deactivation of an activated feature is allowed.

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
qmi_csi_error qmi_uimi_depersonalization_secure
(
  qmi_uimi_client_state_type  * cl_sp,
  qmi_req_handle                req_handle,
  void                        * req_c_struct,
  int                           req_c_struct_len
)
{
  qmi_error_type_v01                           errval                   = QMI_ERR_NONE_V01;
  qmi_uimi_userdata_type  *                    cb_userdata_ptr          = NULL;
  simlock_result_enum_type                     simlock_status           = SIMLOCK_SUCCESS;
  uint16                                       request_id               = 0;
  simlock_unlock_device_msg_req_type           unlock_msg;
  simlock_temporary_unlock_msg_req_type        temp_unlock_msg;
  uim_depersonalization_secure_req_msg_v01    *deperso_secure_req_ptr  = NULL;
  uim_depersonalization_secure_resp_msg_v01   *deperso_secure_resp_ptr = NULL;
  uint16                                       client_index             = 0;
  qmi_uim_cmd_buf_s                           *cmd_buf_p                = NULL;
  qmi_csi_error                                csi_errval               = QMI_CSI_NO_ERR;
  qmi_uim_tlv_decrypted_deperso_secure_type    decrypted_deperso_secure;

  memset(&decrypted_deperso_secure, 0x00, sizeof(decrypted_deperso_secure));
  
  ASSERT(cl_sp && req_c_struct && req_c_struct_len > 0 && qmi_uim_state_ptr);

  if(!qmi_uim_util_get_cli_index(cl_sp->client_handle, &client_index))
  {
    return QMI_CSI_INVALID_HANDLE;
  }

  cmd_buf_p = qmi_uim_util_get_cmd_buf_ptr(req_handle, cl_sp->client_handle);

  ASSERT(cmd_buf_p);

  deperso_secure_req_ptr = (uim_depersonalization_secure_req_msg_v01 *)req_c_struct;
  cmd_buf_p->req_handle = req_handle;  

  /* Parse TLV */
  errval = qmi_uim_tlv_decrypted_deperso_secure(deperso_secure_req_ptr->encrypted_depersonalization_data,
                                                    deperso_secure_req_ptr->encrypted_depersonalization_data_len,
                                                    &decrypted_deperso_secure);
  if (errval != QMI_ERR_NONE_V01)
  {
    goto send_result;
  }

  /* This is required to make sure that TLV was originated for this specific request. */
  if(decrypted_deperso_secure.header.cmd_id != QMI_UIM_DEPERSONALIZATION_SECURE_REQ_V01)
  {
    UIM_MSG_ERR_0("QMI-UIM: decrypt request failed");
    errval = QMI_ERR_MALFORMED_MSG_V01;
    goto send_result;
  }

  /* Retrieve the request_id from the request */
  request_id = decrypted_deperso_secure.header.request_id;

  /* Create userdata */
  cb_userdata_ptr = qmi_uim_util_get_cb_userdata(QMI_UIM_DEPERSONALIZATION_SECURE_REQ_V01,
                                                 cmd_buf_p,
                                                 NULL,
                                                 cl_sp->client_handle);
  if (cb_userdata_ptr == NULL)
  {
    errval = QMI_ERR_NO_MEMORY_V01;
    goto send_result;
  }

  cb_userdata_ptr->data.simlock_unlock.ck_present    = FALSE;
  cb_userdata_ptr->data.simlock_unlock.request_id    = request_id;
  cb_userdata_ptr->data.simlock_unlock.category      = decrypted_deperso_secure.category_type;
  cb_userdata_ptr->data.simlock_unlock.slot          = decrypted_deperso_secure.slot;

  switch (decrypted_deperso_secure.operation)
  {
    case QMI_UIM_DEPERSO_UNLOCK_WITHOUT_CK:
      memset(&unlock_msg, 0x00, sizeof(unlock_msg));
      memset(unlock_msg.ck.simlock_ck_data, 0x00, sizeof(unlock_msg.ck.simlock_ck_data));

      unlock_msg.ck.simlock_ck_length = 0;
      unlock_msg.category             = decrypted_deperso_secure.category_type;
      unlock_msg.slot                 = decrypted_deperso_secure.slot;

      simlock_status = simlock_unlock_trusted(unlock_msg,
                                              qmi_uimi_simlock_callback,
                                              cb_userdata_ptr);
      break;

    case QMI_UIM_DEPERSO_TEMP_UNLOCK_ENABLE:
    case QMI_UIM_DEPERSO_TEMP_UNLOCK_DISABLE:
      memset(&temp_unlock_msg, 0x00, sizeof(temp_unlock_msg));

      if (decrypted_deperso_secure.operation == QMI_UIM_DEPERSO_TEMP_UNLOCK_ENABLE)
      {
        temp_unlock_msg.temporary_unlock_type = SIMLOCK_TEMPORARY_UNLOCK_SET;
      }
      else
      {
        temp_unlock_msg.temporary_unlock_type = SIMLOCK_TEMPORARY_UNLOCK_UNSET;
      }

      temp_unlock_msg.category = decrypted_deperso_secure.category_type;
      temp_unlock_msg.slot = decrypted_deperso_secure.slot;
      temp_unlock_msg.duration = decrypted_deperso_secure.duration;

      simlock_status = simlock_temporary_unlock(&temp_unlock_msg,
                                                qmi_uimi_simlock_callback,
                                                cb_userdata_ptr);
      break;

    default:
      UIM_MSG_ERR_1("Invalid deperso secure operation: 0x%x",
                    decrypted_deperso_secure.operation);
      simlock_status = SIMLOCK_INCORRECT_PARAMS;
      break;
  }

  /* Convert MMGSDI status into QMI err code */
  errval = qmi_uim_simlock_convert_simlock_status_to_errval(simlock_status);

send_result:
  if(errval == QMI_ERR_NONE_V01)
  {
    cmd_buf_p->resp_pending = TRUE;
    return QMI_CSI_NO_ERR;
  }

  /* Free userdata in case of error */
  qmi_uim_util_free_cb_userdata(cb_userdata_ptr);
  
  deperso_secure_resp_ptr =   uimqmi_malloc(sizeof(uim_depersonalization_secure_resp_msg_v01));
  if(!deperso_secure_resp_ptr)
  {
    return QMI_CSI_NO_MEM;
  }
  
  /* Build the result and encrypt the data and populatate the output */
  deperso_secure_resp_ptr->encrypted_resp_len   = sizeof(deperso_secure_resp_ptr->encrypted_resp);
  deperso_secure_resp_ptr->encrypted_resp_valid = 1;
  errval = qmi_uim_response_encrypted_status(QMI_UIM_DEPERSONALIZATION_SECURE_REQ_V01,
                                                 request_id,
                                                 errval,
                                                 deperso_secure_resp_ptr->encrypted_resp,
                                                 &deperso_secure_resp_ptr->encrypted_resp_len);
  if(errval != QMI_ERR_NONE_V01)
  {
    /* Even if encrypted status fails, continue with sending
       response back to client so they are at least notified. */
    deperso_secure_resp_ptr->encrypted_resp_valid = 0;
    UIM_MSG_HIGH_0("failed to build and encrypt response");
  }

  qmi_uim_util_populate_resp(errval, &deperso_secure_resp_ptr->resp);

  csi_errval = qmi_uim_send_immidiate_resp(cmd_buf_p, 
                                           deperso_secure_resp_ptr,
                                           cmd_buf_p->cmd_type,
                                           sizeof(uim_depersonalization_secure_resp_msg_v01),
                                           cl_sp->client_handle);
  uimqmi_free(deperso_secure_resp_ptr);

  return csi_errval;
} /* qmi_uimi_depersonalization_secure() */


/*===========================================================================
  FUNCTION QMI_UIMI_PERSONALIZATION_SECURE()

  DESCRIPTION
    Activates the personalization of the device to a particular lock feature
    without passing in a CK. The input TLV is encrypted and must be decrypted
    before processing to insure a safe connection with client.

    Only one feature can be activated at a time.

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
qmi_csi_error qmi_uimi_personalization_secure
(
  qmi_uimi_client_state_type  * cl_sp,
  qmi_req_handle                req_handle,
  void                        * req_c_struct,
  int                           req_c_struct_len
)
{
  qmi_error_type_v01                          errval                = QMI_ERR_NONE_V01;
  qmi_uimi_userdata_type  *                   cb_userdata_ptr       = NULL;
  simlock_result_enum_type                    simlock_status        = SIMLOCK_SUCCESS;
  uint16                                      request_id            = 0;
  uim_personalization_secure_req_msg_v01     *perso_secure_req_ptr  = NULL;
  uim_personalization_secure_resp_msg_v01    *perso_secure_resp_ptr = NULL;
  uint16                                      client_index          = 0;
  qmi_uim_cmd_buf_s                          *cmd_buf_p             = NULL;
  qmi_csi_error                               csi_errval            = QMI_CSI_NO_ERR;
  qmi_uim_tlv_decrypted_perso_secure_type     decoded_perso;
  simlock_set_lock_ck_msg_req_type            lock_msg;

  memset(&decoded_perso, 0x00, sizeof(decoded_perso));
  memset(&lock_msg, 0x00, sizeof(lock_msg));

  ASSERT(cl_sp && req_c_struct && req_c_struct_len > 0 && qmi_uim_state_ptr);

  if(!qmi_uim_util_get_cli_index(cl_sp->client_handle, &client_index))
  {
    return QMI_CSI_INVALID_HANDLE;
  }

  cmd_buf_p = qmi_uim_util_get_cmd_buf_ptr(req_handle, cl_sp->client_handle);

  ASSERT(cmd_buf_p);

  perso_secure_req_ptr = (uim_personalization_secure_req_msg_v01 *)req_c_struct;
  cmd_buf_p->req_handle = req_handle;  

  errval = qmi_uim_tlv_decrypted_perso_secure(perso_secure_req_ptr->encrypted_personalization_data,
                                                  perso_secure_req_ptr->encrypted_personalization_data_len,
                                                  &decoded_perso);
  if (errval != QMI_ERR_NONE_V01)
  {
    goto send_result;
  }

  /* This is required to make sure that TLV was originated for this specific request */
  if(decoded_perso.header.cmd_id != QMI_UIM_PERSONALIZATION_SECURE_REQ_V01)
  {
    UIM_MSG_ERR_1("Invalid command id: 0x%x", decoded_perso.header.cmd_id);
    errval = QMI_ERR_MALFORMED_MSG_V01;
    goto send_result;
  }

  /* Retrieve the request_id from the request */
  request_id = decoded_perso.header.request_id;

  if (decoded_perso.code_ptr == NULL)
  {
    UIM_MSG_ERR_0("Invalid category data");
    errval = QMI_ERR_INTERNAL_V01;
    goto send_result;
  }

  memset(&lock_msg, 0x00, sizeof(simlock_set_lock_ck_msg_req_type));

  /* Set default lock values */
  lock_msg.auto_lock        = FALSE;
  lock_msg.blacklist        = FALSE;

  /* Set iteration count and number of max retries to 0 since there is
     no chance of unlock failure as there is no CK */
  lock_msg.iteration_cnt    = 0;
  lock_msg.num_retries_max  = 0;
  lock_msg.slot             = decoded_perso.slot;
  lock_msg.category         = *decoded_perso.code_ptr;

  memset(lock_msg.ck.simlock_ck_data, 0x00, sizeof(lock_msg.ck.simlock_ck_data));
  lock_msg.ck.simlock_ck_length = 0;

  /* Create userdata */
  cb_userdata_ptr = qmi_uim_util_get_cb_userdata(QMI_UIM_PERSONALIZATION_SECURE_REQ_V01,
                                                 cmd_buf_p,
                                                 NULL,
                                                 cl_sp->client_handle);
  if (cb_userdata_ptr == NULL)
  {
    errval = QMI_ERR_NO_MEMORY_V01;
    goto send_result;
  }

  /* Save information in the user data */
  cb_userdata_ptr->data.simlock_lock.request_id           = request_id;
  cb_userdata_ptr->data.simlock_lock.ck_present           = FALSE;
  cb_userdata_ptr->data.simlock_lock.total_list_cnt       = decoded_perso.num_of_lists;
  cb_userdata_ptr->data.simlock_lock.slot                 = decoded_perso.slot;

  memset(cb_userdata_ptr->data.simlock_lock.ck.simlock_ck_data, 0x00, SIMLOCK_CK_MAX);
  cb_userdata_ptr->data.simlock_lock.ck.simlock_ck_length = 0;

  /* The lock codes contained in zeroth list are locked here. So
     mark the next list to lock as 1 */
  cb_userdata_ptr->data.simlock_lock.next_list_to_lock    = 1;

  /* Save the lock code data pointer */
  cb_userdata_ptr->data.simlock_lock.code_ptr             = decoded_perso.code_ptr;

  /* Set the original pointer to NULL */
  decoded_perso.code_ptr           = NULL;

  simlock_status = simlock_ck_lock(lock_msg,
                                   qmi_uimi_simlock_callback,
                                   cb_userdata_ptr);

  /* Free userdata in case of error */
  QMI_UIM_SIMLOCK_FREE_USERDATA_IF_ERROR(simlock_status, cb_userdata_ptr);

  /* Convert simlock status into QMI err code */
  errval = qmi_uim_simlock_convert_simlock_status_to_errval(simlock_status);

send_result:
  /* Free the buffer used for data */ 
  if (decoded_perso.code_ptr)
  {
    uimqmi_free(decoded_perso.code_ptr);
    decoded_perso.code_ptr = NULL;
  }

  if(errval == QMI_ERR_NONE_V01)
  {
    cmd_buf_p->resp_pending = TRUE;
    return QMI_CSI_NO_ERR;
  }

  perso_secure_resp_ptr =   uimqmi_malloc(sizeof(uim_personalization_secure_resp_msg_v01));
  if(!perso_secure_resp_ptr)
  {
    return QMI_CSI_NO_MEM;
  }
  
  /* Build the result and encrypt the data and populatate the output */
  perso_secure_resp_ptr->encrypted_resp_len   = sizeof(perso_secure_resp_ptr->encrypted_resp);
  perso_secure_resp_ptr->encrypted_resp_valid = 1;
  errval = qmi_uim_response_encrypted_status(QMI_UIM_PERSONALIZATION_SECURE_REQ_V01,
                                                 request_id,
                                                 errval,
                                                 perso_secure_resp_ptr->encrypted_resp,
                                                 &perso_secure_resp_ptr->encrypted_resp_len);
  if(errval != QMI_ERR_NONE_V01)
  {
    /* Even if encrypted status fails, continue with sending
       response back to client so they are at least notified. */
    perso_secure_resp_ptr->encrypted_resp_valid = 0;
    UIM_MSG_HIGH_0("failed to build and encrypt response");
  }

  qmi_uim_util_populate_resp(errval, &perso_secure_resp_ptr->resp);

  csi_errval = qmi_uim_send_immidiate_resp(cmd_buf_p, 
                                           perso_secure_resp_ptr,
                                           cmd_buf_p->cmd_type,
                                           sizeof(*perso_secure_resp_ptr),
                                           cl_sp->client_handle);
  uimqmi_free(perso_secure_resp_ptr);
  
  return csi_errval;
} /* qmi_uimi_personalization_secure */


/*===========================================================================
  FUNCTION QMI_UIMI_EMERGENCY_ONLY()

  DESCRIPTION
    Sets the device in a emergency only or normal operation mode. The input
    TLV is encrypted and must be decrypted before processing to insure a safe
    connection with client.

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
qmi_csi_error qmi_uimi_emergency_only
(
  qmi_uimi_client_state_type  * cl_sp,
  qmi_req_handle                req_handle,
  void                        * req_c_struct,
  int                           req_c_struct_len
)
{
  qmi_error_type_v01                        errval                  = QMI_ERR_NONE_V01;
  qmi_uimi_userdata_type  *                 cb_userdata_ptr         = NULL;
  simlock_result_enum_type                  simlock_status          = SIMLOCK_SUCCESS;
  uint16                                    request_id              = 0;
  simlock_set_device_mode_msg_req_type      device_mode_msg;
  qmi_uim_tlv_decrypted_emergency_only_type decoded_emergency_only;
  qmi_csi_error                             csi_errval              = QMI_CSI_NO_ERR;
  uim_emergency_only_req_msg_v01           *emergency_only_req_ptr  = NULL;
  uim_emergency_only_resp_msg_v01          *emergency_only_resp_ptr = NULL;
  uint16                                    client_index            = 0;
  qmi_uim_cmd_buf_s                        *cmd_buf_p               = NULL;

  memset(&decoded_emergency_only, 0x00, sizeof(decoded_emergency_only));
  memset(&device_mode_msg, 0x00, sizeof(device_mode_msg));

  ASSERT(cl_sp && req_c_struct && req_c_struct_len > 0 && qmi_uim_state_ptr);

  if(!qmi_uim_util_get_cli_index(cl_sp->client_handle, &client_index))
  {
    return QMI_CSI_INVALID_HANDLE;
  }

  cmd_buf_p = qmi_uim_util_get_cmd_buf_ptr(req_handle, cl_sp->client_handle);

  ASSERT(cmd_buf_p);

  emergency_only_req_ptr = (uim_emergency_only_req_msg_v01 *)req_c_struct;
  cmd_buf_p->req_handle  = req_handle;  

  errval = qmi_uim_tlv_decrypted_emergency_only(emergency_only_req_ptr->encrypted_emergency_data,
                                                emergency_only_req_ptr->encrypted_emergency_data_len,
                                                &decoded_emergency_only);
  if (errval != QMI_ERR_NONE_V01)
  {
    goto send_result;
  }

  /* This is required to make sure that TLV was originated for this specific request. */
  if(decoded_emergency_only.header.cmd_id != QMI_UIM_EMERGENCY_ONLY_REQ_V01)
  {
    UIM_MSG_ERR_0("QMI-UIM: decrypt request failed");
    errval = QMI_ERR_MALFORMED_MSG_V01;
    goto send_result;
  }

  memset(&device_mode_msg, 0x00, sizeof(simlock_set_device_mode_msg_req_type));

  /* Retrieve the mode and request_id from the request */
  device_mode_msg.mode = decoded_emergency_only.device_mode;
  request_id           = decoded_emergency_only.header.request_id;

  /* Create userdata */
  cb_userdata_ptr = qmi_uim_util_get_cb_userdata(QMI_UIM_EMERGENCY_ONLY_REQ_V01,
                                                 cmd_buf_p,
                                                 NULL,
                                                 cl_sp->client_handle);
  if (cb_userdata_ptr == NULL)
  {
    errval = QMI_ERR_NO_MEMORY_V01;
    goto send_result;
  }

  /* save information in the user data */
  cb_userdata_ptr->data.simlock_set_device_mode.request_id = request_id;

  simlock_status = simlock_set_device_operation_mode(device_mode_msg,
                                                     qmi_uimi_simlock_callback,
                                                     cb_userdata_ptr);

  /* Convert MMGSDI status into QMI err code */
  errval = qmi_uim_simlock_convert_simlock_status_to_errval(simlock_status);

send_result:
  if(errval == QMI_ERR_NONE_V01)
  {
    cmd_buf_p->resp_pending = TRUE;
    return QMI_CSI_NO_ERR;
  }

  /* Free userdata in case of error */
  qmi_uim_util_free_cb_userdata(cb_userdata_ptr);
  
  emergency_only_resp_ptr = uimqmi_malloc(sizeof(uim_emergency_only_resp_msg_v01));
  if(!emergency_only_resp_ptr)
  {
    return QMI_CSI_NO_MEM;
  }
  
  /* Build the result and encrypt the data and populatate the output */
  emergency_only_resp_ptr->encrypted_resp_len   = sizeof(emergency_only_resp_ptr->encrypted_resp);
  emergency_only_resp_ptr->encrypted_resp_valid = 1;
  errval = qmi_uim_response_encrypted_status(QMI_UIM_EMERGENCY_ONLY_REQ_V01,
                                             request_id,
                                             errval,
                                             emergency_only_resp_ptr->encrypted_resp,
                                             &emergency_only_resp_ptr->encrypted_resp_len);
  if(errval != QMI_ERR_NONE_V01)
  {
    /* Even if encrypted status fails, continue with sending
       response back to client so they are at least notified. */
    emergency_only_resp_ptr->encrypted_resp_valid = 0;
    UIM_MSG_HIGH_0("failed to build and encrypt response");
  }

  qmi_uim_util_populate_resp(errval, &emergency_only_resp_ptr->resp);

  csi_errval = qmi_uim_send_immidiate_resp(cmd_buf_p, 
                                           emergency_only_resp_ptr,
                                           cmd_buf_p->cmd_type,
                                           sizeof(uim_emergency_only_resp_msg_v01),
                                           cl_sp->client_handle);
  uimqmi_free(emergency_only_resp_ptr);

  return csi_errval;
} /* qmi_uimi_emergency_only */


/*===========================================================================
  FUNCTION QMI_UIMI_SIMLOCK_CONFIGURATION()

  DESCRIPTION
    Response to a simlock operation request sent via the indication to the
    client. The input TLV is encrypted and must be decrypted before
    processing to insure a safe connection with client.

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
qmi_csi_error  qmi_uimi_simlock_configuration
(
  qmi_uimi_client_state_type  * cl_sp,
  qmi_req_handle                req_handle,
  void                        * req_c_struct,
  int                           req_c_struct_len
)
{
  qmi_error_type_v01                          errval             = QMI_ERR_NONE_V01;
  qmi_uimi_userdata_type  *                 cb_userdata_ptr    = NULL;
  simlock_result_enum_type                  simlock_status     = SIMLOCK_SUCCESS;
  qmi_uim_simlock_config_operation_type     operation_type     = QMI_UIM_SIMLOCK_CONFIG_OPERATION_READ;
  uint16                                    request_id         = 0;
  qmi_csi_error                             csi_errval         = QMI_CSI_NO_ERR;
  uim_simlock_configuration_req_msg_v01    *config_req_ptr     = NULL;
  uim_simlock_configuration_resp_msg_v01   *config_resp_ptr    = NULL;
  uint16                                    client_index       = 0;
  qmi_uim_cmd_buf_s                        *cmd_buf_p          = NULL;
  qmi_uim_tlv_decrypted_simlock_config_type decoded_simlock_config;

  memset(&decoded_simlock_config, 0, sizeof(decoded_simlock_config));

  ASSERT(cl_sp && req_c_struct && req_c_struct_len > 0 && qmi_uim_state_ptr);

  if(!qmi_uim_util_get_cli_index(cl_sp->client_handle, &client_index))
  {
    return QMI_CSI_INVALID_HANDLE;
  }

  cmd_buf_p = qmi_uim_util_get_cmd_buf_ptr(req_handle, cl_sp->client_handle);

  ASSERT(cmd_buf_p);

  config_req_ptr = (uim_simlock_configuration_req_msg_v01 *)req_c_struct;
  cmd_buf_p->req_handle = req_handle;  

  errval = qmi_uim_tlv_decrypted_simlock_config(config_req_ptr->encrypted_simlock_config_data,
                                                config_req_ptr->encrypted_simlock_config_data_len,
                                                &decoded_simlock_config);
  if (errval != QMI_ERR_NONE_V01)
  {
    goto send_result;
  }

  /* This is required to make sure that TLV was originated for this specific request */
  if(decoded_simlock_config.header.cmd_id != QMI_UIM_SIMLOCK_CONFIGURATION_REQ_V01)
  {
    UIM_MSG_ERR_1("Invalid command id: 0x%x", decoded_simlock_config.header.cmd_id);
    errval = QMI_ERR_MALFORMED_MSG_V01;
    goto send_result;
  }

  /* Retrieve the operation and request_id from the request */
  operation_type = decoded_simlock_config.operation;
  request_id     = decoded_simlock_config.header.request_id;

  /* If the operation is enable or disable remote SFS, call the
     simlock API */
  if((operation_type == QMI_UIM_SIMLOCK_CONFIG_OPERATION_ENABLE_REMOTE_SFS) ||
     (operation_type == QMI_UIM_SIMLOCK_CONFIG_OPERATION_DISABLE_REMOTE_SFS))
  {
    /* Create userdata */
    cb_userdata_ptr = qmi_uim_util_get_cb_userdata(QMI_UIM_SIMLOCK_CONFIGURATION_REQ_V01,
                                                   cmd_buf_p,
                                                   NULL,
                                                   cl_sp->client_handle);
    if (cb_userdata_ptr == NULL)
    {
      errval = QMI_ERR_NO_MEMORY_V01;
      goto send_result;
    }

    /* save information in the user data */
    cb_userdata_ptr->data.simlock_set_remote_sfs.request_id = request_id;

    simlock_status = simlock_set_remote_sfs(decoded_simlock_config.operation_type.remote_sfs_operation,
                                            qmi_uimi_simlock_set_remote_sfs_callback,
                                            cb_userdata_ptr);

    /* Free userdata in case of error */
    QMI_UIM_SIMLOCK_FREE_USERDATA_IF_ERROR(simlock_status, cb_userdata_ptr);

    /* Convert MMGSDI status into QMI err code */
    errval = qmi_uim_simlock_convert_simlock_status_to_errval(simlock_status);
  }

  /* If the operation is read/write simlock data in remote SFS, call the
     simlock response cb */
  if(operation_type == QMI_UIM_SIMLOCK_CONFIG_OPERATION_READ)
  {
    if(qmi_uim_global_ptr->simlock_remote_sfs_operation_resp_cb != NULL)
    {
      qmi_uim_global_ptr->simlock_remote_sfs_operation_resp_cb(decoded_simlock_config.operation_type.sfs_read_operation.result,
                                                               request_id,
                                                               &decoded_simlock_config.operation_type.sfs_read_operation.data);
      qmi_uim_global_ptr->simlock_remote_sfs_operation_resp_cb = NULL;
    }
    else
    {
      /* A NULL callback indicates that the modem is not
         expecting this message. So return an error */
      errval = QMI_ERR_INVALID_OPERATION_V01;
    }
  }
  else if(operation_type == QMI_UIM_SIMLOCK_CONFIG_OPERATION_WRITE)
  {
    if(qmi_uim_global_ptr->simlock_remote_sfs_operation_resp_cb != NULL)
    {
      qmi_uim_global_ptr->simlock_remote_sfs_operation_resp_cb(decoded_simlock_config.operation_type.sfs_write_operation.result,
                                                               request_id,
                                                               NULL);
      qmi_uim_global_ptr->simlock_remote_sfs_operation_resp_cb = NULL;
    }
    else
    {
      /* A NULL callback indicates that the modem is not
         expecting this message. So return an error */
      errval = QMI_ERR_INVALID_OPERATION_V01;
    }
  }

send_result:
  /* Free the buffer used for data: this also clears the read data
     information for security reason */        
  if (decoded_simlock_config.operation_type.sfs_read_operation.data.data_ptr)
  {
    uimqmi_free(decoded_simlock_config.operation_type.sfs_read_operation.data.data_ptr);
    decoded_simlock_config.operation_type.sfs_read_operation.data.data_ptr = NULL;
  }
  
  if ((errval == QMI_ERR_NONE_V01) &&
      (operation_type == QMI_UIM_SIMLOCK_CONFIG_OPERATION_ENABLE_REMOTE_SFS ||
       operation_type == QMI_UIM_SIMLOCK_CONFIG_OPERATION_DISABLE_REMOTE_SFS))
  {
    cmd_buf_p->resp_pending = TRUE;
    return QMI_CSI_NO_ERR;
  }

  config_resp_ptr =   uimqmi_malloc(sizeof(uim_simlock_configuration_resp_msg_v01));
  if(!config_resp_ptr)
  {
    return QMI_CSI_NO_MEM;
  }

  config_resp_ptr->encrypted_resp_len = sizeof(config_resp_ptr->encrypted_resp);
  config_resp_ptr->encrypted_resp_valid = 1;
  /* Build the result and encrypt the data and populatate the output sdu */
  errval = qmi_uim_response_encrypted_status(QMI_UIM_SIMLOCK_CONFIGURATION_REQ_V01,
                                                 request_id,
                                                 errval,
                                                 config_resp_ptr->encrypted_resp,
                                                 &config_resp_ptr->encrypted_resp_len);
  if(errval != QMI_ERR_NONE_V01)
  {
    /* Even if encrypted status fails, continue with sending
       response back to client so they are at least notified. */
    config_resp_ptr->encrypted_resp_valid = 0;
    UIM_MSG_HIGH_0("failed to build and encrypt response");
  }

  qmi_uim_util_populate_resp(errval, &config_resp_ptr->resp);

  csi_errval = qmi_uim_send_immidiate_resp(cmd_buf_p, 
                                           config_resp_ptr,
                                           cmd_buf_p->cmd_type,
                                           sizeof(uim_simlock_configuration_resp_msg_v01),
                                           cl_sp->client_handle);
  uimqmi_free(config_resp_ptr);

  return csi_errval;
} /* qmi_uimi_simlock_configuration */


/*===========================================================================
  FUNCTION QMI_UIMI_REMOTE_UNLOCK()

  DESCRIPTION
    Provides the key in the encrypted format or applies the simlock 
    configuration data provided as the input.

  PARAMETERS
    cl_sp     : client state pointer
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
qmi_csi_error qmi_uimi_remote_unlock
(
  qmi_uimi_client_state_type  * cl_sp,
  qmi_req_handle                req_handle,
  void                        * req_c_struct,
  int                           req_c_struct_len
)
{
  qmi_error_type_v01                           errval              = QMI_ERR_NONE_V01;
  simlock_result_enum_type                   simlock_status      = SIMLOCK_SUCCESS;
  qmi_uimi_userdata_type                    *cb_userdata_ptr     = NULL;
  uim_remote_unlock_req_msg_v01             *rmt_unlock_req_ptr  = NULL;
  uim_remote_unlock_resp_msg_v01            *rmt_unlock_resp_ptr      = NULL;
  uint16                                     client_index        = 0;
  qmi_uim_cmd_buf_s                         *cmd_buf_p           = NULL;
  qmi_csi_error                              csi_errval          = QMI_CSI_NO_ERR;
  simlock_data_type                          unlock_config_data  = {0, NULL};

  ASSERT(cl_sp && req_c_struct && req_c_struct_len > 0 && qmi_uim_state_ptr);

  if(!qmi_uim_util_get_cli_index(cl_sp->client_handle, &client_index))
  {
    return QMI_CSI_INVALID_HANDLE;
  }

  cmd_buf_p = qmi_uim_util_get_cmd_buf_ptr(req_handle, cl_sp->client_handle);

  ASSERT(cmd_buf_p);

  rmt_unlock_req_ptr = (uim_remote_unlock_req_msg_v01 *)req_c_struct;
  cmd_buf_p->req_handle = req_handle;  

  if(rmt_unlock_req_ptr->simlock_data_valid)
  {
    if(rmt_unlock_req_ptr->simlock_data_len == 0 ||
       rmt_unlock_req_ptr->simlock_data_len > QMI_UIM_SIMLOCK_DATA_MAX_V01)
    {
      errval = QMI_ERR_ARG_TOO_LONG_V01;
      goto send_result;
    }
    unlock_config_data.data_ptr = rmt_unlock_req_ptr->simlock_data;
    unlock_config_data.data_len = rmt_unlock_req_ptr->simlock_data_len;
  }

  /* Create userdata */
  cb_userdata_ptr = qmi_uim_util_get_cb_userdata(QMI_UIM_REMOTE_UNLOCK_REQ_V01,
                                                cmd_buf_p,
                                                NULL,
                                                cl_sp->client_handle);
  if (cb_userdata_ptr == NULL)
  {
    errval = QMI_ERR_NO_MEMORY_V01;
    goto send_result;
  }

  if(rmt_unlock_req_ptr->simlock_data_valid)
  {
    if((unlock_config_data.data_len > 0) &&
       (unlock_config_data.data_ptr != NULL))
    {
      simlock_status = simlock_rsu_set_configuration(unlock_config_data,
                                                     qmi_uimi_simlock_remote_unlock_callback,
                                                     cb_userdata_ptr);
    }
    else
    {
      simlock_status = SIMLOCK_INCORRECT_PARAMS;
    }
  }
  else
  {
    simlock_status = simlock_rsu_get_encrypted_key(qmi_uimi_simlock_remote_unlock_callback,
                                                   cb_userdata_ptr);
  }

  /* Convert simlock status into QMI err code */
  errval = qmi_uim_simlock_convert_simlock_status_to_errval(simlock_status);

send_result:
  if(errval == QMI_ERR_NONE_V01)
  {
    cmd_buf_p->resp_pending = TRUE;
    return QMI_CSI_NO_ERR;
  }
  
  /* Free userdata in case of error */
  qmi_uim_util_free_cb_userdata(cb_userdata_ptr);

  rmt_unlock_resp_ptr =   uimqmi_malloc(sizeof(uim_remote_unlock_resp_msg_v01));
  if(!rmt_unlock_resp_ptr)
  {
    return QMI_CSI_NO_MEM;
  }
  
  qmi_uim_util_populate_resp(errval, &rmt_unlock_resp_ptr->resp);

  csi_errval = qmi_uim_send_immidiate_resp(cmd_buf_p, 
                                           rmt_unlock_resp_ptr,
                                           cmd_buf_p->cmd_type,
                                           sizeof(uim_remote_unlock_resp_msg_v01),
                                           cl_sp->client_handle);
  uimqmi_free(rmt_unlock_resp_ptr);

  return csi_errval;
} /* qmi_uimi_remote_unlock */


/*===========================================================================
  FUNCTION QMI_UIMI_VERIFY_IMSI()

  DESCRIPTION
    Verifies that subscription related to IMSI is validated by SIMLOCK or not.
    If optional IMSI TLV is not present, it verifies the IMSI of all active
    provisioning sessions and returns an error if any of them is not verified.

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
qmi_csi_error  qmi_uimi_verify_imsi
(
  qmi_uimi_client_state_type  * cl_sp,
  qmi_req_handle                req_handle,
  void                        * req_c_struct,
  int                           req_c_struct_len
)
{
  qmi_error_type_v01              errval                 = QMI_ERR_NONE_V01;
  simlock_result_enum_type        simlock_status         = SIMLOCK_SUCCESS;
  uint8                           loop_index             = 0;
  uint8                           index                  = 0;
  qmi_uim_imsi_type               imsi_data              = {0, };
  simlock_imsi_type               simlock_imsi           = {0, };
  uint16                          *session_index_array[] = {qmi_uim_global_ptr->card_state.index_gw,
                                                            qmi_uim_global_ptr->card_state.index_1x};
  mmgsdi_session_id_type          *session_id_array[]    = {qmi_uim_global_ptr->mmgsdi_gw_session_id,
                                                            qmi_uim_global_ptr->mmgsdi_1x_session_id};
  uim_verify_imsi_req_msg_v01     *verify_imsi_req_ptr   = NULL;
  uim_verify_imsi_resp_msg_v01    *verify_imsi_resp_ptr  = NULL;
  uint16                           client_index          = 0;
  qmi_uim_cmd_buf_s               *cmd_buf_p             = NULL;
  qmi_csi_error                    csi_errval            = QMI_CSI_NO_ERR;
 
  ASSERT(cl_sp && req_c_struct && req_c_struct_len > 0 && qmi_uim_state_ptr);

  if(!qmi_uim_util_get_cli_index(cl_sp->client_handle, &client_index))
  {
    return QMI_CSI_INVALID_HANDLE;
  }

  cmd_buf_p = qmi_uim_util_get_cmd_buf_ptr(req_handle, cl_sp->client_handle);

  ASSERT(cmd_buf_p);

  verify_imsi_req_ptr = (uim_verify_imsi_req_msg_v01 *)req_c_struct;
  cmd_buf_p->req_handle = req_handle;  

  if (verify_imsi_req_ptr->imsi_valid && 
      (verify_imsi_req_ptr->imsi_len > sizeof(simlock_imsi.imsi) ||
       verify_imsi_req_ptr->imsi_len == 0))
  {
    errval = QMI_ERR_ARG_TOO_LONG_V01;
    goto send_result;
  }
 
  if(verify_imsi_req_ptr->imsi_valid)
  {
    memscpy(simlock_imsi.imsi, sizeof(simlock_imsi.imsi), verify_imsi_req_ptr->imsi, verify_imsi_req_ptr->imsi_len);
    simlock_imsi.imsi_len = verify_imsi_req_ptr->imsi_len;
    if(verify_imsi_req_ptr->imsi_len > 0)
    {
      simlock_status = simlock_verify_imsi(simlock_imsi);
    }
    else
    {
      simlock_status = SIMLOCK_INCORRECT_PARAMS;
    }
  }
  else
  {
    /* Read IMSI for all active subscriptions synchronously and verify IMSI
       is validated by SIMLOCK module or not */
    for(loop_index = 0;
        loop_index < sizeof(session_index_array) / sizeof(session_index_array[0]);
        loop_index++)
    {
      for (index = 0; index < QMI_UIM_MAX_PROV_SESSIONS; index++)
      {
        if (session_index_array[loop_index][index] != 0xFFFF)
        {
          errval = qmi_uim_util_read_imsi_for_session_id(session_id_array[loop_index][index],
                                                         &imsi_data);
          /* It is possible that IMSI data is not cached yet for subscription
             i.e. it gets stuck because of enabled PIN1. This also means that
             SIMLOCK algo is not run for that subscription. For such scenario we
             should not assume that IMSI is not valid; just continue to verify
             IMSI of next active subscription */
          if(errval != QMI_ERR_NONE_V01)
          {
            continue;
          }

          simlock_imsi.imsi_len = imsi_data.len;
          (void)memscpy(simlock_imsi.imsi, sizeof(simlock_imsi.imsi), imsi_data.imsi, imsi_data.len);

          simlock_status = simlock_verify_imsi(simlock_imsi);
          if(simlock_status != SIMLOCK_SUCCESS)
          {
            UIM_MSG_ERR_2("Invalid IMSI for session at index 0x%x, 0x%x",
                          loop_index, index);
            break;
          }
        }
      }

      /*If any of the IMSI is not valid, break the loop */
      if(simlock_status != SIMLOCK_SUCCESS)
      {
        break;
      }
    }
  }

  /* If SIMLOCK module returns generic error, map it to INVALID ID */
  if(simlock_status == SIMLOCK_GENERIC_ERROR)
  {
    errval = QMI_ERR_INVALID_ID_V01;
  }
  else
  {
    errval = qmi_uim_simlock_convert_simlock_status_to_errval(simlock_status);
  }

send_result:
  verify_imsi_resp_ptr =   uimqmi_malloc(sizeof(uim_verify_imsi_resp_msg_v01));
  if(!verify_imsi_resp_ptr)
  {
    return QMI_CSI_NO_MEM;
  }
  
  qmi_uim_util_populate_resp(errval, &verify_imsi_resp_ptr->resp);

  csi_errval = qmi_uim_send_immidiate_resp(cmd_buf_p, 
                                           verify_imsi_resp_ptr,
                                           cmd_buf_p->cmd_type,
                                           sizeof(uim_verify_imsi_resp_msg_v01),
                                           cl_sp->client_handle);
  uimqmi_free(verify_imsi_resp_ptr);

  return csi_errval;
} /* qmi_uimi_verify_imsi */
#endif /* FEATURE_SIMLOCK */


/*===========================================================================
  FUNCTION QMI_UIMI_SIMLOCK_CALLBACK()

  DESCRIPTION
    This function is executed as callback of SIM Lock API. This is
    executed in the context of SIM Lock task and needs to make a deep
    copy of the results and post a command to the QMI task to be
    processed.

  PARAMETERS

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uimi_simlock_callback
(
  simlock_result_enum_type                    status,
  const simlock_message_response_data_type  * resp_ptr,
  const void                                * user_data_ptr
)
{
  qmi_uim_message_type * msg_ptr = NULL;

  ASSERT(resp_ptr != NULL);
  ASSERT(user_data_ptr != NULL);

  UIM_MSG_HIGH_1("Got response from SIM Lock for message: 0x%x", resp_ptr->msg_type);

  switch(resp_ptr->msg_type)
  {
    case SIMLOCK_UNLOCK_DEVICE_MSG:
    case SIMLOCK_TEMPORARY_UNLOCK_MSG:
    case SIMLOCK_GET_STATUS_MSG:
    case SIMLOCK_SET_LOCK_CK_MSG:
    case SIMLOCK_SET_DEVICE_MODE_MSG:
    case SIMLOCK_ADD_LOCK_CODES_CK_MSG:
    case SIMLOCK_RELOCK_DEVICE_MSG:
      msg_ptr = qmi_uim_util_create_message(QMI_UIM_MESSAGE_SIMLOCK_CB);
      if (msg_ptr != NULL)
      {
        msg_ptr->data.simlock.status  = status;
        msg_ptr->data.simlock.user_data_ptr = user_data_ptr;

        memscpy(&msg_ptr->data.simlock.data,
                sizeof(msg_ptr->data.simlock.data),
                resp_ptr,
                sizeof(simlock_message_response_data_type));

        /* Send command */
        qmi_uim_util_post_message(msg_ptr);
        msg_ptr = NULL;
      }
      break;

    default:
      break;
  }
} /* qmi_uimi_simlock_callback */


/*===========================================================================
  FUNCTION QMI_UIM_SIMLOCK_CONVERT_SIMLOCK_STATUS_TO_ERRVAL

  DESCRIPTION
    To obtain QMI client-specific error message from SIM Lock error message

  PARAMETERS
    simlock_status  : SIM Lock return value

  RETURN VALUE
    errval          : QMI client-specific error message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_uim_simlock_convert_simlock_status_to_errval
(
  simlock_result_enum_type simlock_status
)
{
  qmi_error_type_v01 errval  = QMI_ERR_INTERNAL_V01;

  UIM_MSG_HIGH_1("Received simlock_status 0x%x", simlock_status);

  switch(simlock_status)
  {
    case SIMLOCK_SUCCESS:
      errval = QMI_ERR_NONE_V01;
      break;

    case SIMLOCK_INVALID_STATE:
      errval = QMI_ERR_NO_EFFECT_V01;
      break;

    case SIMLOCK_INCORRECT_KEY:
      errval = QMI_ERR_INCORRECT_PIN_V01;
      break;

    case SIMLOCK_MAX_RETRIES_REACHED:
      errval = QMI_ERR_PIN_PERM_BLOCKED_V01;
      break;

    case SIMLOCK_INCORRECT_PARAMS:
      errval = QMI_ERR_MALFORMED_MSG_V01;
      break;

    case SIMLOCK_UNSUPPORTED:
      errval = QMI_ERR_NOT_SUPPORTED_V01;
      break;

    case SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED:
      errval = QMI_ERR_NO_MEMORY_V01;
      break;

    default:
      errval = QMI_ERR_INTERNAL_V01;
      break;
  }

  return errval;
} /* qmi_uim_simlock_convert_simlock_status_to_errval() */


/*===========================================================================
  FUNCTION QMI_UIM_SIMLOCK_CHECK_AND_DEREGISTER_REMOTE_SFS_OPERATION()

  DESCRIPTION
    This function deregisters the QMI UIM callback with the simlock.

  PARAMETERS
     qmi_client_handle;  client_handle

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_simlock_check_and_deregister_remote_sfs_operation
(
  qmi_client_handle client_handle
)
{
  uint8                              i               = 0;
  qmi_uimi_client_state_type       * cl_sp           = NULL;

  if((qmi_uim_state_ptr == NULL) ||
     !qmi_uim_util_get_cli_index(client_handle, NULL))
  {
    return;
  }

  for(i = 0; i < UIMQMI_SVC_MAX_CLIDS; i++)
  {
    cl_sp = (qmi_uimi_client_state_type*)qmi_uim_state_ptr->client[i];
    if(cl_sp == NULL) 
    {
      continue;
    }

    if(cl_sp->client_handle== client_handle)
    {
      continue;
    }

    /* If there is at least one client who registered for remote SFS
       events, do not de-register with simlock */
    if(cl_sp->uim_info.reg_event_type.simlock_remote_sfs_events)
    {
      return;
    }
  }

  /* Deregister for events from SIM Lock task */
  (void)simlock_register_remote_sfs_operation(SIMLOCK_DEREGISTER,
                                              NULL);
} /* qmi_uim_simlock_check_and_deregister_remote_sfs_operation */
