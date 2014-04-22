/*===========================================================================

        Q M I _ U I M _ S U P P L Y _ V O L T A G E _ S T U B S . C

DESCRIPTION

 This file contains the stub functions for Supply Voltage functionality

Copyright (c) 2016, 2018 - 2019 QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/uim.mpss/3.3.1/uimqmi/src/qmi_uim_supply_voltage_stubs.c#1 $ $DateTime: 2020/07/22 10:53:27 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/22/19    tq     Masked unsupported event registration 
10/15/18    tq     Migration of QMI UIM service to QCCI-QCSI
07/26/16    sp     Move qmi_uimi_ functions to corresponding files
06/16/16    vdc    Split QMI UIM parsing files for supporting IOE
05/20/16    ar     Initial Revision.
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "qmi_uim_supply_voltage.h"
#include "qmi_uim_util.h"
#include "uim_v.h"

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
  (void)mmgsdi_slot;
  return QMI_ERR_NOT_SUPPORTED;
} /* qmi_uim_all_reg_clients_ack_vcc_deactivation */


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
  (void)uim_message_ptr;
} /* qmi_uim_process_supply_voltage_ind */


/*===========================================================================
  FUNCTION QMI_UIM_SUPPLY_VOLTAGE_IND()

  DESCRIPTION
    This function is invoked by the UIM drivers when the Vcc needs to
    be deactivated or when Vcc has been activated.

  PARAMETERS
    slot                   : UIM slot id
    ldo_state              : LDO state
    uim_callback_ptr       : callback to ack drivers

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
  (void)slot;
  (void)ldo_state;
  (void)uim_callback_ptr;
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
  uim_supply_voltage_resp_msg_v01 *resp_ptr             = NULL;
  uint16                           client_index         = 0;
  qmi_uim_cmd_buf_s               *cmd_buf_p            = NULL;
  qmi_csi_error                    csi_errval           = QMI_CSI_NO_ERR;

  ASSERT(cl_sp && req_c_struct && req_c_struct_len > 0 && qmi_uim_state_ptr);
  
  if(!qmi_uim_util_get_cli_index(cl_sp->client_handle, &client_index))
  {
    return QMI_CSI_INVALID_HANDLE;
  }

  cmd_buf_p = qmi_uim_util_get_cmd_buf_ptr(req_handle, cl_sp->client_handle);

  ASSERT(cmd_buf_p);

  cmd_buf_p->req_handle = req_handle;  

  resp_ptr =   uimqmi_malloc(sizeof(uim_supply_voltage_resp_msg_v01));
  if(!resp_ptr)
  {
    return QMI_CSI_NO_MEM;
  }
  
  qmi_uim_util_populate_resp(QMI_ERR_NOT_SUPPORTED_V01, &resp_ptr->resp);

  csi_errval = qmi_uim_send_immidiate_resp(cmd_buf_p, 
                                           resp_ptr,
                                           cmd_buf_p->cmd_type,
                                           sizeof(uim_supply_voltage_resp_msg_v01),
                                           cl_sp->client_handle);
  uimqmi_free(resp_ptr);
 
  return csi_errval;
}/* qmi_uimi_supply_voltage */


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
  return FALSE;
} /* qmi_uim_is_supply_voltage_supported */
