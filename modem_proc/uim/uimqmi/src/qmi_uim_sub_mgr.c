/*===========================================================================

                         Q M I _ U I M _ S U B _ M G R. C

DESCRIPTION

 The Data Services Qualcomm MSM Interface Device Management Services source
 file.

Copyright (c) 2016 - 2019 QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //components/rel/uim.mpss/3.3.1/uimqmi/src/qmi_uim_sub_mgr.c#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/22/19    tq     Masked unsupported event registration 
10/15/18    tq     Migration of QMI UIM service to QCCI-QCSI
05/09/17    ks     Fixing compilation issue with sub-manager feature
05/08/17    ks     Fix compilation issue
07/26/16    sp     Move qmi_uimi_ functions to corresponding files
06/16/16    vdc    Split QMI UIM parsing files for supporting IOE
05/19/16    vdc    Initial Revision.
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "qmi_uim_sub_mgr.h"
#include "qmi_uim_sub_mgr_parsing.h"
#include "qmi_framework.h"
#include "qmi_svc_utils.h"
#include "uim_msg.h"
#include "amssassert.h"
#include <stringl/stringl.h>
#include "qmi_uim_util.h"
#include "uimsub_manager.h"

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

 #ifdef FEATURE_UIM_DS_SUBSCRIPTION_MANAGER
/*===========================================================================
FUNCTION QMI_UIM_SUB_MGR_IS_PHYSICAL_SLOT_ACTIVE_FOR_LOGICAL_SLOT()

DESCRIPTION
  Figures out if for the logical slot, the physical slot is active or not.

PARAMETERS
  uim_slot_type        : logical slot id
  uim_slot_type        : physical slot id

RETURN VALUE
  None

DEPENDENCIES
  QMI UIM service must be initialized and registered with Framework

SIDE EFFECTS
  None

===========================================================================*/
boolean qmi_uim_sub_mgr_is_physical_slot_active_for_logical_slot
(
  uim_slot_type                   logical_slot_id,
  uim_slot_type                   physical_slot_id
)
{
  uint8 slot_idx = 0;

  ASSERT(qmi_uim_global_ptr != NULL);

  /* Convert slot to index */
  switch (physical_slot_id)
  {
    case UIM_SLOT_1:
      slot_idx = 0;
      break;
    case UIM_SLOT_2:
      slot_idx = 1;
      break;
    default:
      return FALSE;
  }

  if(slot_idx < qmi_uim_global_ptr->physical_slots_info.num_slots &&
     qmi_uim_global_ptr->physical_slots_info.slot_status_ptr != NULL &&
     qmi_uim_global_ptr->physical_slots_info.slot_status_ptr[slot_idx].logical_slot == logical_slot_id &&
     qmi_uim_global_ptr->physical_slots_info.slot_status_ptr[slot_idx].activity_status == UIM_SLOT_STATE_ACTIVE)
  {
    return TRUE;
  }

  return FALSE;
} /* qmi_uim_sub_mgr_is_physical_slot_active_for_logical_slot() */


/*===========================================================================
FUNCTION QMI_UIM_SUB_MGR_NOTIFY_SLOTS_STATUS_CB()

DESCRIPTION
  A callback called by uimdrv when an association of a logical slot to
  a physical slot changes or whenever there is a change to the card state
  in any physical slot.

PARAMETERS
  uint8                   : number of physical slots
  uim_phy_slot_status_type: status of the physical slots

RETURN VALUE
  None

DEPENDENCIES
  QMI UIM service must be initialized and registered with Framework

SIDE EFFECTS
  None

===========================================================================*/
void qmi_uim_sub_mgr_notify_slots_status_cb
(
  uint8                           num_physical_slots,
  const uim_phy_slot_status_type *physical_slot_status_ptr
)
{
  qmi_uim_message_type * msg_ptr = NULL;

  if (num_physical_slots == 0 ||
      physical_slot_status_ptr == NULL)
  {
    UIM_MSG_ERR_0("Invalid physical slots info reported by drivers");
    return;
  }

  msg_ptr = qmi_uim_util_create_message(QMI_UIM_MESSAGE_PHYSICAL_SLOTS_STATUS);
  if (msg_ptr == NULL)
  {
    return;
  }

  /* Copy the physical slots info into the qmiuim message */
  msg_ptr->data.physical_slots_info.num_slots = num_physical_slots;
  uimqmi_deep_copy(
       (void**)&msg_ptr->data.physical_slots_info.slot_status_ptr,
       (void*)physical_slot_status_ptr,
       sizeof(uim_phy_slot_status_type) * num_physical_slots);

  /* Send command */
  qmi_uim_util_post_message(msg_ptr);
  msg_ptr = NULL;
} /* qmi_uim_sub_mgr_notify_slots_status_cb() */
#endif /* FEATURE_UIM_DS_SUBSCRIPTION_MANAGER */


/*===========================================================================
  FUNCTION QMI_UIM_SUB_MGR_SEND_SLOTS_STATUS_INDICATION()

  DESCRIPTION
    This function sends a slots status indication to a specific client

  PARAMETERS
    cli_handle : client handle

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_uim_sub_mgr_send_slots_status_indication
(
  qmi_client_handle cli_handle
)
{
  uim_slot_status_change_ind_msg_v01* indication_ptr = NULL;

  ASSERT(qmi_uim_util_get_cli_index(cli_handle, NULL) && qmi_uim_global_ptr);
  indication_ptr = (uim_slot_status_change_ind_msg_v01*)uimqmi_malloc(sizeof(uim_slot_status_change_ind_msg_v01));
  if(indication_ptr == NULL)
  {
    return;
  }
  /* Insert slots status item in TLV */
  if (FALSE == qmi_uim_sub_mgr_response_physical_slots_status(
                 &indication_ptr->physical_slot_status,
                 &indication_ptr->physical_slot_status_len,
                 &indication_ptr->physical_slot_status_valid,
                 qmi_uim_global_ptr->physical_slots_info.slot_status_ptr,
                 qmi_uim_global_ptr->physical_slots_info.num_slots))
  {
    /* Ignore error... there is nothing we can do about it.
       The event will not be delivered to the client */
    uimqmi_free(&indication_ptr);
    return;
  }

  /* Send indication to the client */ 
  (void)qmi_uim_send_response_ind(NULL,
                                  indication_ptr,
                                  (uint16)QMI_UIM_SLOT_STATUS_CHANGE_IND_V01,
                                  sizeof(uim_slot_status_change_ind_msg_v01),
                                  TRUE,
                                  cli_handle);
  uimqmi_free(indication_ptr);
} /* qmi_uim_sub_mgr_send_slots_status_indication */


/*===========================================================================
FUNCTION QMI_UIM_SUB_MGR_STORE_PHYSICAL_SLOTS_STATUS()

DESCRIPTION
  Copies the physical slots status from the passed-in parameter to the
  QMI UIM global for physical slots status.

PARAMETERS
  uint8                   : number of physical slots supported
  uim_phy_slot_status_type: status of the physical slots

RETURN VALUE
  TRUE                    : Global updated with latest physical slots info
  FALSE                   : Otherwise

DEPENDENCIES
  QMI UIM service must be initialized and registered with Framework

SIDE EFFECTS
  None

===========================================================================*/
static boolean qmi_uim_sub_mgr_store_physical_slots_status
(
  uint8                           num_physical_slots,
  const uim_phy_slot_status_type *physical_slot_status_ptr
)
{
  uint8         index           = 0;
  boolean       is_info_updated = FALSE;

  ASSERT(qmi_uim_global_ptr != NULL);

  /* If number of slots QMI UIM received by calling get_num_physical_slots
     during power up is not same as number of physical slots mentioned in
     the uimdrv callback here, there's a serious problem */
  if(qmi_uim_global_ptr->physical_slots_info.slot_status_ptr == NULL ||
     qmi_uim_global_ptr->physical_slots_info.num_slots == 0 ||
     physical_slot_status_ptr == NULL ||
     num_physical_slots != qmi_uim_global_ptr->physical_slots_info.num_slots)
  {
    UIM_MSG_ERR_0("qmi_uim_sub_mgr_store_physical_slots_status: slots validation error");
    return FALSE;
  }

  /* Overwrite the qmi uim global ptr for physical slots status info with
     latest info */
  for(index = 0; index < qmi_uim_global_ptr->physical_slots_info.num_slots && index < UIM_NUM_PHY_SLOTS; index ++)
  {
    /* Do not update the card hotswap status if it has not changed since last
       time we updated it (to CARD_INSERTED or CARD_REMOVED) */
    if(physical_slot_status_ptr[index].card_presence_status != UIM_CARD_UNCHANGED &&
       qmi_uim_global_ptr->physical_slots_info.slot_status_ptr[index].card_presence_status !=
         physical_slot_status_ptr[index].card_presence_status)
    {
      qmi_uim_global_ptr->physical_slots_info.slot_status_ptr[index].card_presence_status =
        physical_slot_status_ptr[index].card_presence_status;
      is_info_updated = TRUE;
    }
    if(qmi_uim_global_ptr->physical_slots_info.slot_status_ptr[index].activity_status !=
         physical_slot_status_ptr[index].activity_status)
    {
      qmi_uim_global_ptr->physical_slots_info.slot_status_ptr[index].activity_status =
        physical_slot_status_ptr[index].activity_status;
      is_info_updated = TRUE;
    }
    if(qmi_uim_global_ptr->physical_slots_info.slot_status_ptr[index].logical_slot !=
         physical_slot_status_ptr[index].logical_slot)
    {
      qmi_uim_global_ptr->physical_slots_info.slot_status_ptr[index].logical_slot =
        physical_slot_status_ptr[index].logical_slot;
      is_info_updated = TRUE;
    }
    if(qmi_uim_global_ptr->physical_slots_info.slot_status_ptr[index].iccid_len !=
         physical_slot_status_ptr[index].iccid_len ||
       memcmp(qmi_uim_global_ptr->physical_slots_info.slot_status_ptr[index].iccid,
              physical_slot_status_ptr[index].iccid,
              physical_slot_status_ptr[index].iccid_len) != 0)
    {
      qmi_uim_global_ptr->physical_slots_info.slot_status_ptr[index].iccid_len =
          physical_slot_status_ptr[index].iccid_len;
      (void)memscpy(qmi_uim_global_ptr->physical_slots_info.slot_status_ptr[index].iccid,
                    sizeof(qmi_uim_global_ptr->physical_slots_info.slot_status_ptr[index].iccid),
                    physical_slot_status_ptr[index].iccid,
                    physical_slot_status_ptr[index].iccid_len);
      is_info_updated = TRUE;
    }
  }

  return is_info_updated;
} /* qmi_uim_sub_mgr_store_physical_slots_status() */


/*===========================================================================
  FUNCTION QMI_UIM_SUB_MGR_SEND_SLOTS_STATUS_INDICATION_TO_ALL()

  DESCRIPTION
    This function sends a slots status indication to all QMI_UIM clients

  PARAMETERS

  RETURN VALUE
    None.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_uim_sub_mgr_send_slots_status_indication_to_all
(
  void
)
{
  uint8                       j                       = 0;
  qmi_uimi_client_state_type* cl_sp                   = NULL;

  if(qmi_uim_state_ptr == NULL)
  {
    return;
  }

  for (j = 0; j < UIMQMI_SVC_MAX_CLIDS; j++)
  {
    cl_sp = (qmi_uimi_client_state_type*)qmi_uim_state_ptr->client[j];
    if (cl_sp != NULL &&
        qmi_uim_util_get_cli_index(cl_sp->client_handle, NULL) &&
        cl_sp->uim_info.reg_event_type.physical_slots_status)
    {
      /* Send indication to specific client */
      qmi_uim_sub_mgr_send_slots_status_indication(cl_sp->client_handle);
    }
  }
} /* qmi_uim_sub_mgr_send_slots_status_indication_to_all */


/*===========================================================================
  FUNCTION QMI_UIM_SUB_PROCESS_PHYSICAL_SLOTS_STATUS_MSG()

  DESCRIPTION
    This function handles message from the QMI UIM queue for physical slots
    status from drivers

  PARAMETERS
    uim_message_ptr: Pointer to QMI UIM message from message queue

  RETURN VALUE
    None.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_sub_mgr_process_physical_slots_status_msg
(
  qmi_uim_message_type * uim_message_ptr
)
{
  boolean is_info_updated = FALSE;

  ASSERT(uim_message_ptr);

  UIM_MSG_HIGH_1("Processing physical slots status CB from drivers; num physical slots: 0x%x",
                 uim_message_ptr->data.physical_slots_info.num_slots);

  is_info_updated = qmi_uim_sub_mgr_store_physical_slots_status(
                        uim_message_ptr->data.physical_slots_info.num_slots,
                        uim_message_ptr->data.physical_slots_info.slot_status_ptr);

  /* Free the physical slots info in the message */
  if(uim_message_ptr->data.physical_slots_info.slot_status_ptr != NULL)
  {
    uimqmi_free(uim_message_ptr->data.physical_slots_info.slot_status_ptr);
    uim_message_ptr->data.physical_slots_info.slot_status_ptr = NULL;
  }

  if(is_info_updated != FALSE)
  {
    /* Send slots status indication */
    qmi_uim_sub_mgr_send_slots_status_indication_to_all();
  }
} /* qmi_uim_sub_mgr_process_physical_slots_status_msg */


/*===========================================================================
  FUNCTION qmi_uimi_get_slots_status()

  DESCRIPTION
    Get current physical slots status

  PARAMETERS
    cl_sp     : client state pointer
    req_handle       : req handle
    req_c_struct     : IDL client structure
    req_c_struct_len : structure len

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    QMI UIM service must be initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_error  qmi_uimi_get_slots_status
(
  qmi_uimi_client_state_type  * cl_sp,
  qmi_req_handle                req_handle,
  void                        * req_c_struct,
  int                           req_c_struct_len
)
{
  qmi_error_type_v01                    errval                    = QMI_ERR_NONE_V01;
  uim_get_slots_status_resp_msg_v01    *get_slots_status_resp_ptr = NULL;
  uint16                                client_index              = 0;
  qmi_uim_cmd_buf_s                    *cmd_buf_p                 = NULL;
  qmi_csi_error                         csi_errval                = QMI_CSI_NO_ERR;

  ASSERT(cl_sp && req_c_struct && req_c_struct_len > 0 && qmi_uim_state_ptr);

  get_slots_status_resp_ptr = uimqmi_malloc(sizeof(uim_get_slots_status_resp_msg_v01));
  if(!get_slots_status_resp_ptr)
  {
    return QMI_CSI_NO_MEM;
  }

  if(!qmi_uim_util_get_cli_index(cl_sp->client_handle, &client_index))
  {
    return QMI_CSI_INVALID_HANDLE;
  }

  cmd_buf_p = qmi_uim_util_get_cmd_buf_ptr(req_handle, cl_sp->client_handle);

  ASSERT(cmd_buf_p);

  cmd_buf_p->req_handle = req_handle;  

#ifdef FEATURE_UIM_DS_SUBSCRIPTION_MANAGER
  if(qmi_uim_global_ptr->physical_slots_info.num_slots == 0||
     qmi_uim_global_ptr->physical_slots_info.slot_status_ptr == NULL)
  {
    /* If QMI UIM never registered with drivers for slots status or if
       or if the number of physical slots reported by drivers is zero,
       return error to user */
    errval = QMI_ERR_INTERNAL_V01;
    goto send_result;
  }

  /* Insert physical slots status data from the global into the response TLV */
  if (FALSE == qmi_uim_sub_mgr_response_physical_slots_status(&get_slots_status_resp_ptr->physical_slot_status,
                                                              &get_slots_status_resp_ptr->physical_slot_status_len,
                                                              &get_slots_status_resp_ptr->physical_slot_status_valid,
                                                      qmi_uim_global_ptr->physical_slots_info.slot_status_ptr,
                                                      qmi_uim_global_ptr->physical_slots_info.num_slots))
  {
    errval = QMI_ERR_INTERNAL_V01;
  }
#else
  errval = QMI_ERR_NOT_SUPPORTED_V01;
#endif /* FEATURE_UIM_DS_SUBSCRIPTION_MANAGER */

#ifdef FEATURE_UIM_DS_SUBSCRIPTION_MANAGER
send_result:
#endif /* FEATURE_UIM_DS_SUBSCRIPTION_MANAGER */
  qmi_uim_util_populate_resp(errval, &get_slots_status_resp_ptr->resp);

  csi_errval = qmi_uim_send_immidiate_resp(cmd_buf_p, 
                                           get_slots_status_resp_ptr,
                                           cmd_buf_p->cmd_type,
                                           sizeof(uim_get_slots_status_resp_msg_v01),
                                           cl_sp->client_handle);
  uimqmi_free(get_slots_status_resp_ptr);

  return csi_errval;
} /* qmi_uimi_get_slots_status() */


/*===========================================================================
  FUNCTION QMI_UIMI_SWITCH_SLOT()

  DESCRIPTION
    Switches the physical slot tied to current logical slot such that the
    current logical slot now maps to the physical slot mentioned in the request

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
qmi_csi_error qmi_uimi_switch_slot
(
  qmi_uimi_client_state_type  * cl_sp,
  qmi_req_handle                req_handle,
  void                        * req_c_struct,
  int                           req_c_struct_len
)
{
  qmi_error_type_v01                    errval               = QMI_ERR_NONE_V01;
  uim_slot_type                         uim_log_slot         = UIM_SLOT_NONE;
  uim_slot_type                         uim_phy_slot         = UIM_SLOT_NONE;
  uim_switch_slot_req_msg_v01          *switch_slot_req_ptr  = NULL;
  uim_switch_slot_resp_msg_v01         *switch_slot_resp_ptr = NULL;
  uint16                                client_index         = 0;
  qmi_uim_cmd_buf_s                    *cmd_buf_p            = NULL;
  mmgsdi_slot_id_enum_type              logical_slot         = MMGSDI_MAX_SLOT_ID_ENUM;
  qmi_csi_error                         csi_errval           = QMI_CSI_NO_ERR;

  ASSERT(cl_sp && req_c_struct && req_c_struct_len > 0 && qmi_uim_state_ptr);

  if(!qmi_uim_util_get_cli_index(cl_sp->client_handle, &client_index))
  {
    return QMI_CSI_INVALID_HANDLE;
  }

  cmd_buf_p = qmi_uim_util_get_cmd_buf_ptr(req_handle, cl_sp->client_handle);

  ASSERT(cmd_buf_p);

  switch_slot_req_ptr = (uim_switch_slot_req_msg_v01 *)req_c_struct;
  cmd_buf_p->req_handle = req_handle;  

  errval = qmi_uim_map_and_validate_slot(&logical_slot, switch_slot_req_ptr->logical_slot);
  if(errval != QMI_ERR_NONE_V01)
  {
    goto send_result;
  }  
  /* Physical Slot */
  switch(switch_slot_req_ptr->physical_slot)
  {
    case UIM_PHY_SLOT_1_V01:
      uim_phy_slot = UIM_SLOT_1;
      break;
    case UIM_PHY_SLOT_2_V01:
      uim_phy_slot = UIM_SLOT_2;
      break;
    case UIM_PHY_SLOT_3_V01:
      uim_phy_slot = UIM_SLOT_3;
      break;
    case UIM_PHY_SLOT_4_V01:
    case UIM_PHY_SLOT_5_V01:
      errval = QMI_ERR_NOT_SUPPORTED_V01;
    default:
      errval = QMI_ERR_INVALID_ARG_V01;
  }
  if(errval != QMI_ERR_NONE_V01)
  {
    goto send_result;
  }
  if(qmi_uim_util_mmgsdi_slot_id_to_uim_slot_id(logical_slot,
                                                &uim_log_slot) != QMI_ERR_NONE_V01)
  {
    errval = QMI_ERR_NOT_SUPPORTED_V01;
    goto send_result;
  }

#ifdef FEATURE_UIM_DS_SUBSCRIPTION_MANAGER
  /* If the physical slot that is requested to be switched to or to be
     activated is already active, return */
  if(qmi_uim_sub_mgr_is_physical_slot_active_for_logical_slot(uim_log_slot,
                                                              uim_phy_slot))
  {
    errval = QMI_ERR_NO_EFFECT_V01;
    goto send_result;
  }

  /* Execute UIMDRV function to switch the slot */
  if(UIM_PASS != uim_sm_switch_slot(uim_log_slot,
                                    uim_phy_slot))
  {
    errval = QMI_ERR_INTERNAL_V01;
  }
#else
  errval = QMI_ERR_NOT_SUPPORTED_V01;
  goto send_result;
#endif /* FEATURE_UIM_DS_SUBSCRIPTION_MANAGER */

send_result:
  switch_slot_resp_ptr =   uimqmi_malloc(sizeof(uim_switch_slot_resp_msg_v01));
  if(!switch_slot_req_ptr)
  {
    return QMI_CSI_NO_MEM;
  }
  
  qmi_uim_util_populate_resp(errval, &switch_slot_resp_ptr->resp);

  csi_errval = qmi_uim_send_immidiate_resp(cmd_buf_p, 
                                           switch_slot_resp_ptr,
                                           cmd_buf_p->cmd_type,
                                           sizeof(uim_switch_slot_resp_msg_v01),
                                           cl_sp->client_handle);
  uimqmi_free(switch_slot_resp_ptr);
 
  return csi_errval;
} /* qmi_uimi_switch_slot() */


/*===========================================================================
  FUNCTION QMI_UIM_IS_SUB_MANAGER_SUPPORTED()

  DESCRIPTION
    this fucntion indicates qmi uim physical slot indication
    is supported or not based upon the file compilation.
    qmi_uim_sub_mgr.c or qmi_uim_sub_mgr_stubs.c

  PARAMETERS
    None
    
  RETURN VALUE
    TRUE: if qmi_uim_sub_mgr.c got compiled
    FALSE: if qmi_uim_sub_mgr_stubs.c got compiled
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_is_sub_manager_supported(void)
{
  return TRUE;
} /* qmi_uim_is_sub_manager_supported */
