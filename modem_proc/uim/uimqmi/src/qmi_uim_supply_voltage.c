/*===========================================================================

           Q M I _ U I M _ S U P P L Y _ V O L T A G E . C

DESCRIPTION

 This file contains the functions for Supply Voltage functionality

Copyright (c) 2016, 2018 - 2019 QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/uim.mpss/3.3.1/uimqmi/src/qmi_uim_supply_voltage.c#1 $ $DateTime: 2020/07/22 10:53:27 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/22/19    tq     Masked unsupported event registration 
15/10/18    tq     Migration of QMI UIM service to QCCI-QCSI
07/26/16    sp     Move qmi_uimi_ functions to corresponding files
05/20/16    ar     Initial Revision.
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "qmi_uim_util.h"
#include "qmi_uim_cat_common.h"
#include "qmi_uim_parsing.h"
#include "qmi_uim_supply_voltage.h"
#include "uim_msg.h"
#include "uim_v.h"
#include "err.h"
#include "amssassert.h"

/*===========================================================================

                             EXTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
  FUNCTION QMI_UIM_ALL_REG_CLIENTS_ACK_VCC_DEACTIVATION()

  DESCRIPTION
    Indicates whether all registered clients have acknowledged Vcc
    power down.

  PARAMETERS
    slot_id : MMGSDI slot id

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_uim_all_reg_clients_ack_vcc_deactivation
(
  mmgsdi_slot_id_enum_type  mmgsdi_slot
)
{
  uint8             i          = 0;
  uint8             slot_idx   = 0;
  uim_slot_type     uim_slot   = UIM_SLOT_NONE;
  qmi_error_e_type  errval     = QMI_ERR_NONE;

  errval = uimqmi_mmgsdi_slot_to_slot_index(mmgsdi_slot,
                                            &slot_idx);
  if (errval != QMI_ERR_NONE)
  {
    return errval;
  }

  for (i = 0; i < UIMQMI_SVC_MAX_CLIDS; i++)
  {
    if(qmi_uim_state_ptr->client[i] &&
       qmi_uim_state_ptr->client[i]->uim_info.supply_voltage_pending_ack[slot_idx] == TRUE)
    {
      return QMI_ERR_NONE;
    }
  }

  errval = qmi_uim_util_mmgsdi_slot_id_to_uim_slot_id(mmgsdi_slot,
                                                      &uim_slot);
  if (errval != QMI_ERR_NONE)
  {
    return errval;
  }

  /* Send acknowledgement back to UIM drivers notifying them that all
     clients have notified with ok to proceed with Vcc power down */
  if (qmi_uim_global_ptr->supply_voltage_cb[slot_idx])
  {
    qmi_uim_global_ptr->supply_voltage_cb[slot_idx](uim_slot, UIMDRV_LDO_AWAITING_DEACTIVATION_IND_ACK);
  }

  /* After notifying UIM drivers we need to clear the saved supply voltage acknowledgement
     callback indicating that the indication for Vcc deactivate has been acknowledged. */
  qmi_uim_global_ptr->supply_voltage_cb[slot_idx] = NULL;

  return QMI_ERR_NONE;
} /* qmi_uim_all_reg_clients_ack_vcc_deactivation */


/*===========================================================================
  FUNCTION QMI_UIM_SEND_SUPPLY_VOLTAGE_INDICATION()

  DESCRIPTION
    This function sends a supply voltage indication to a specific client.

  PARAMETERS
    cli_handle    : Client Handle
    mmgsdi_slot   : Slot of cooresponding supply voltage change
    ldo_state     : Current LDO state to be notified

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_type_v01 qmi_uim_send_supply_voltage_indication
(
  qmi_client_handle               cli_handle,
  mmgsdi_slot_id_enum_type        mmgsdi_slot,
  uimdrv_qmi_indications_type     ldo_state
)
{
  uim_supply_voltage_ind_msg_v01               * indication_ptr   = NULL;
  qmi_csi_error                                  csi_err          = QMI_CSI_NO_ERR;
  
  ASSERT(qmi_uim_util_get_cli_index(cli_handle, NULL) && qmi_uim_global_ptr);

  if(mmgsdi_slot > MMGSDI_SLOT_3)
  {
    return QMI_ERR_INTERNAL_V01;
  }
  
  indication_ptr = uimqmi_malloc(sizeof(*indication_ptr));
  if(indication_ptr == NULL)
  { 
    return QMI_ERR_NO_MEMORY_V01;
  }

  indication_ptr->slot = (uim_slot_enum_v01)mmgsdi_slot;
  
  if(ldo_state == UIMDRV_LDO_ACTIVATED_IND)
  {
    indication_ptr->vcc_command = UIM_SUPPLY_VOLTAGE_ACTIVATE_V01;
  }
  else if (ldo_state == UIMDRV_LDO_AWAITING_DEACTIVATION_IND)
  {
    indication_ptr->vcc_command = UIM_SUPPLY_VOLTAGE_DEACTIVATE_V01;
  }
  else
  {
    uimqmi_free(indication_ptr);
    return QMI_ERR_INTERNAL_V01;
    
  }  
  /* Send indication to the client */
  csi_err = qmi_uim_send_response_ind(NULL,
                                      indication_ptr,
                                      (uint16)QMI_UIM_SUPPLY_VOLTAGE_REQ_V01,
                                      sizeof(uim_supply_voltage_ind_msg_v01),
                                      TRUE,
                                      cli_handle);
  uimqmi_free(indication_ptr);

  if(csi_err != QMI_CSI_NO_ERR)
  {
    return QMI_ERR_INTERNAL_V01;
  }
  
  return QMI_ERR_NONE_V01;
} /* qmi_uim_send_supply_voltage_indication */


/*===========================================================================
  FUNCTION QMI_UIM_SEND_SUPPLY_VOLTAGE_INDICATION_TO_ALL()

  DESCRIPTION
    This function will send the supply voltage indication to all registered
    clients.

  PARAMETERS
    uim_message_ptr: QMI UIM message

  RETURN VALUE
    None

  DEPENDENCIES
    QMI UIM must already have been initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_uim_send_supply_voltage_indication_to_all
(
  const qmi_uim_message_type * uim_message_ptr
)
{
  uint8                                         i                = 0;
  qmi_uimi_client_state_type                  * cl_sp            = NULL;
  uint8                                         slot_idx         = 0;
  mmgsdi_slot_id_enum_type                      mmgsdi_slot      = MMGSDI_MAX_SLOT_ID_ENUM;
  qmi_error_type_v01                            errval           = QMI_ERR_NONE_V01;

  if(qmi_uim_state_ptr == NULL ||
     uim_message_ptr == NULL)
  {
    return;
  }

  UIM_MSG_HIGH_1("qmi_uim_send_supply_voltage_indication_to_all: slot = 0x%x",
                 uim_message_ptr->data.supply_voltage.slot);

  errval = qmi_uim_util_uim_slot_id_to_mmgsdi_slot_id(uim_message_ptr->data.supply_voltage.slot,
                                                      &mmgsdi_slot);
  if (errval != QMI_ERR_NONE_V01)
  {
    return;
  }

  if (uimqmi_mmgsdi_slot_to_slot_index(mmgsdi_slot,
                                       &slot_idx) != QMI_ERR_NONE)
  {
    return;
  }

  switch (uim_message_ptr->data.supply_voltage.ldo_state)
  {
    case UIMDRV_LDO_ACTIVATED_IND:
      qmi_uim_global_ptr->supply_voltage_cb[slot_idx] = NULL;
      break;
    case UIMDRV_LDO_AWAITING_DEACTIVATION_IND:
      qmi_uim_global_ptr->supply_voltage_cb[slot_idx] =
        uim_message_ptr->data.supply_voltage.uim_callback;
      break;
    default:
      UIM_MSG_ERR_1("Invalid LDO state: 0x%x",
                    uim_message_ptr->data.supply_voltage.ldo_state);
      return;
  }

  /* After saving the supply voltage callback, we need to clear the supply voltage acknowledgement
     bit indicating that none of the clients have sent acknowledgments just in case accidentally
     any client did not vote in the previous Vcc deactivation. */
  for (i = 0; i < UIMQMI_SVC_MAX_CLIDS; i++)
  {
    cl_sp = (qmi_uimi_client_state_type*)qmi_uim_state_ptr->client[i];
    if(cl_sp == NULL)
    {
      continue;
    }

    cl_sp->uim_info.supply_voltage_pending_ack[slot_idx] = FALSE;

    /* Send supply voltage indication if client requested for them during client event
       registration (saved in reg_event_type.supply_voltage_events flag) */
    if ( qmi_uim_util_get_cli_index(cl_sp->client_handle, NULL) &&
         cl_sp->uim_info.reg_event_type.supply_voltage_events)
    {
      errval = qmi_uim_send_supply_voltage_indication(
                   cl_sp->client_handle,
                   mmgsdi_slot,
                   uim_message_ptr->data.supply_voltage.ldo_state);

      if(uim_message_ptr->data.supply_voltage.ldo_state == UIMDRV_LDO_AWAITING_DEACTIVATION_IND &&
         errval == QMI_ERR_NONE_V01)
      {
        cl_sp->uim_info.supply_voltage_pending_ack[slot_idx] = TRUE;
      }
    }
  }
} /* qmi_uim_send_supply_voltage_indication_to_all */


/*===========================================================================
  FUNCTION QMI_UIM_PROCESS_SUPPLY_VOLTAGE_IND()

  DESCRIPTION
    This function processes the supply voltage indication from UIM drivers.
    It indicates either Vcc needs to be deactivated or that Vcc activation has
    occurred to all registered QMI clients.

  PARAMETERS
    uim_message_ptr: QMI UIM message

  RETURN VALUE
    None

  DEPENDENCIES
    QMI UIM must already have been initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_process_supply_voltage_ind
(
  const qmi_uim_message_type * uim_message_ptr
)
{
  uint8                     slot_idx     = 0;
  mmgsdi_slot_id_enum_type  mmgsdi_slot  = MMGSDI_MAX_SLOT_ID_ENUM;
  qmi_error_e_type          errval       = QMI_ERR_NONE;

  if(uim_message_ptr == NULL)
  {
    return;
  }

  errval = qmi_uim_util_uim_slot_id_to_mmgsdi_slot_id(uim_message_ptr->data.supply_voltage.slot,
                                                      &mmgsdi_slot);
  if (errval != QMI_ERR_NONE)
  {
    return;
  }

  if (uimqmi_mmgsdi_slot_to_slot_index(mmgsdi_slot,
                                       &slot_idx) != QMI_ERR_NONE)
  {
    return;
  }

  qmi_uim_send_supply_voltage_indication_to_all(uim_message_ptr);

  (void)qmi_uim_all_reg_clients_ack_vcc_deactivation(mmgsdi_slot);
} /* qmi_uim_process_supply_voltage_ind */


/*===========================================================================
  FUNCTION QMI_UIM_SUPPLY_VOLTAGE_IND()

  DESCRIPTION
    This function is invoked by the UIM drivers when the Vcc needs to
    be deactivated or when Vcc has been activated.

  PARAMETERS
    slot                                      : UIM slot id
    uimdrv_qmi_indications_type               : LDO state
    uimdrv_qmi_power_management_callback_type : callback to ack drivers

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_supply_voltage_ind
(
  uim_slot_type                                     slot,
  uimdrv_qmi_indications_type                       ldo_state,
  uimdrv_qmi_power_management_callback_type         uim_callback_ptr
)
{
  qmi_uim_message_type * msg_ptr = NULL;

  UIM_MSG_HIGH_2("Received supply voltage indication, slot: 0x%x, ldo_state: 0x%x",
                 slot, ldo_state);

  msg_ptr = qmi_uim_util_create_message(QMI_UIM_MESSAGE_SUPPLY_VOLTAGE_IND);
  if (msg_ptr == NULL)
  {
    return;
  }

  msg_ptr->data.supply_voltage.slot             = slot;
  msg_ptr->data.supply_voltage.ldo_state        = ldo_state;
  msg_ptr->data.supply_voltage.uim_callback     = uim_callback_ptr;

  /* Send command */
  qmi_uim_util_post_message(msg_ptr);
  msg_ptr = NULL;
} /* qmi_uim_supply_voltage_ind */


/*===========================================================================
  FUNCTION QMI_UIMI_SUPPLY_VOLTAGE()

  DESCRIPTION
    Marks that the client has acknowledged the supply voltage indication
    and once all clients have responded, notifies uimdrv to proceed with
    Vcc deactivation.

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
qmi_csi_error  qmi_uimi_supply_voltage
(
  qmi_uimi_client_state_type  * cl_sp,
  qmi_req_handle                req_handle,
  void                        * req_c_struct,
  int                           req_c_struct_len
)
{
  uim_slot_type                      uim_slot                = UIM_SLOT_NONE;
  mmgsdi_slot_id_enum_type           mmgsdi_slot             = MMGSDI_MAX_SLOT_ID_ENUM;
  uint8                              slot_idx                = 0;
  qmi_error_type_v01                 errval                  = QMI_ERR_NONE_V01;
  uim_supply_voltage_req_msg_v01    *supply_voltage_req_ptr  = NULL;
  uim_supply_voltage_resp_msg_v01   *supply_voltage_resp_ptr = NULL;
  uint16                             client_index            = 0;
  qmi_uim_cmd_buf_s                 *cmd_buf_p               = NULL;
  qmi_csi_error                      csi_errval              = QMI_CSI_NO_ERR;

  ASSERT(cl_sp && req_c_struct && req_c_struct_len > 0 && qmi_uim_state_ptr);

  if(!qmi_uim_util_get_cli_index(cl_sp->client_handle, &client_index))
  {
    return QMI_CSI_INVALID_HANDLE;
  }

  cmd_buf_p = qmi_uim_util_get_cmd_buf_ptr(req_handle, cl_sp->client_handle);

  ASSERT(cmd_buf_p);

  supply_voltage_req_ptr = (uim_supply_voltage_req_msg_v01 *)req_c_struct;
  cmd_buf_p->req_handle = req_handle;  

  errval = qmi_uim_map_and_validate_slot(&mmgsdi_slot, supply_voltage_req_ptr->slot);
  if(errval != QMI_ERR_NONE_V01)
  {
    goto send_result;
  }

  errval = qmi_uim_util_mmgsdi_slot_id_to_uim_slot_id(mmgsdi_slot,
                                                      &uim_slot);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  errval = uimqmi_mmgsdi_slot_to_slot_index(mmgsdi_slot,
                                            &slot_idx);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  if (!cl_sp->uim_info.supply_voltage_pending_ack[slot_idx])
  {
    UIM_MSG_ERR_0("Client is not pending any supply voltage acknowledgement");
    errval = QMI_ERR_DEVICE_NOT_READY_V01;
    goto send_result;
  }

  cl_sp->uim_info.supply_voltage_pending_ack[slot_idx] = FALSE;

  /* Only send acknowledgement to UIM drivers once all registered clients have responded
     with their acknowledgements */
  errval = qmi_uim_all_reg_clients_ack_vcc_deactivation(mmgsdi_slot);

send_result:
  supply_voltage_resp_ptr =   (uim_supply_voltage_resp_msg_v01*)uimqmi_malloc(sizeof(uim_supply_voltage_resp_msg_v01));
  if(!supply_voltage_resp_ptr)
  {
    return QMI_CSI_NO_MEM;
  }
  
  qmi_uim_util_populate_resp(errval, &supply_voltage_resp_ptr->resp);

  csi_errval = qmi_uim_send_immidiate_resp(cmd_buf_p, 
                                           supply_voltage_resp_ptr,
                                           cmd_buf_p->cmd_type,
                                           sizeof(uim_supply_voltage_resp_msg_v01),
                                           cl_sp->client_handle);
  uimqmi_free(supply_voltage_resp_ptr);
 
  return csi_errval;
} /* qmi_uimi_supply_voltage */


/*===========================================================================
  FUNCTION QMI_UIM_IS_SUPPLY_VOLTAGE_SUPPORTED()

  DESCRIPTION
    this fucntion indicates qmi uim supply_voltage indication
    is supported or not based upon the file compilation.
    qmi_uim_supply_voltage.c or qmi_uim_supply_voltage_stubs.c

  PARAMETERS
    None
    
  RETURN VALUE
    TRUE: if qmi_uim_supply_voltage.c got compiled
    FALSE: if qmi_uim_supply_voltage_stubs.c got compiled
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_is_supply_voltage_supported(void)
{
  return TRUE;
} /* qmi_uim_is_supply_voltage_supported */
