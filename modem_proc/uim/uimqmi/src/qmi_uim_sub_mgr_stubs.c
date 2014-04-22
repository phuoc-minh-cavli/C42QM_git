/*===========================================================================

                         Q M I _ U I M _ S U B _ M G R. C

DESCRIPTION

 The Data Services Qualcomm MSM Interface Device Management Services source
 file.

Copyright (c) 2016, 2018 - 2019 QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //components/rel/uim.mpss/3.3.1/uimqmi/src/qmi_uim_sub_mgr_stubs.c#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/22/19    tq     Masked unsupported event registration 
10/15/18    tq     Migration of QMI UIM service to QCCI-QCSI
07/26/16    sp     Move qmi_uimi_ functions to corresponding files
06/16/16    vdc    Split QMI UIM parsing files for supporting IOE
05/19/16    vdc    Initial Revision.
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "uim_p.h"
#include "qmi_uim_sub_mgr.h"
#include "qmi_uim_sub_mgr_parsing.h"
#include "qmi_uim_util.h"
#include "qmi_framework.h"
#include "qmi_svc_utils.h"

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
  (void)logical_slot_id;
  (void)physical_slot_id;

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
  (void)num_physical_slots;
  (void)physical_slot_status_ptr;
} /* qmi_uim_sub_mgr_notify_slots_status_cb() */
#endif /* FEATURE_UIM_DS_SUBSCRIPTION_MANAGER */


/*===========================================================================
  FUNCTION QMI_UIM_SUB_MGR_RESPONSE_PHYSICAL_SLOTS_STATUS()

  DESCRIPTION
    Composes the TLV content for slots status indication

  PARAMETERS
    phy_slot_status_ptr             : Output status of physical slots 
    phy_slot_status_len_ptr         : Output number of slots
    is_valid                        : Output status of physical slots is valid
    slot_status_ptr                 : Status of slots
    num_slots                       : Number of physical slots

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_sub_mgr_response_physical_slots_status
(
  uim_physical_slot_status_type_v01 *phy_slot_status_ptr,
  uint32                            *phy_slot_status_len_ptr,
  uint8                             *is_valid,
  const uim_phy_slot_status_type   * slot_status_ptr,
  uint8                              num_slots
)
{
  (void)phy_slot_status_ptr;
  (void)phy_slot_status_len_ptr;
  (void)is_valid;
  (void)slot_status_ptr;
  (void)num_slots;

  return FALSE;
} /* qmi_uim_sub_mgr_response_physical_slots_status */

/*===========================================================================
  FUNCTION QMI_UIM_PROCESS_PHYSICAL_SLOTS_STATUS_MSG()

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
  (void)uim_message_ptr;
} /* qmi_uim_sub_mgr_process_physical_slots_status_msg */


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
  uim_switch_slot_resp_msg_v01   *switch_slot_resp_ptr = NULL;
  uint16                          client_index         = 0;
  qmi_uim_cmd_buf_s              *cmd_buf_p            = NULL;
  qmi_csi_error                   csi_errval           = QMI_CSI_NO_ERR;

  ASSERT(cl_sp && req_c_struct && req_c_struct_len > 0 && qmi_uim_state_ptr);

  if(!qmi_uim_util_get_cli_index(cl_sp->client_handle, &client_index))
  {
    return QMI_CSI_INVALID_HANDLE;
  }

  cmd_buf_p = qmi_uim_util_get_cmd_buf_ptr(req_handle, cl_sp->client_handle);

  ASSERT(cmd_buf_p);

  cmd_buf_p->req_handle = req_handle;  

  switch_slot_resp_ptr =   uimqmi_malloc(sizeof(uim_switch_slot_resp_msg_v01));
  if(!switch_slot_resp_ptr)
  {
    return QMI_CSI_NO_MEM;
  }

  qmi_uim_util_populate_resp(QMI_ERR_NOT_SUPPORTED_V01, &switch_slot_resp_ptr->resp);

  csi_errval = qmi_uim_send_immidiate_resp(cmd_buf_p, 
                                           switch_slot_resp_ptr,
                                           cmd_buf_p->cmd_type,
                                           sizeof(uim_switch_slot_resp_msg_v01),
                                           cl_sp->client_handle);
  uimqmi_free(switch_slot_resp_ptr);

  return csi_errval;
}/* qmi_uimi_switch_slot */


/*===========================================================================
  FUNCTION qmi_uimi_get_slots_status()

  DESCRIPTION
    Get current physical slots status

  PARAMETERS
    cl_sp            : client state pointer
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
  uim_get_slots_status_resp_msg_v01 *slot_status_resp_ptr = NULL;
  uint16                             client_index         = 0;
  qmi_uim_cmd_buf_s                 *cmd_buf_p            = NULL;
  qmi_csi_error                      csi_errval           = QMI_CSI_NO_ERR;

  ASSERT(cl_sp && req_c_struct && req_c_struct_len > 0 && qmi_uim_state_ptr);
  
  if(!qmi_uim_util_get_cli_index(cl_sp->client_handle, &client_index))
  {
    return QMI_CSI_INVALID_HANDLE;
  }

  cmd_buf_p = qmi_uim_util_get_cmd_buf_ptr(req_handle, cl_sp->client_handle);

  ASSERT(cmd_buf_p);

  cmd_buf_p->req_handle = req_handle;  

  slot_status_resp_ptr =   uimqmi_malloc(sizeof(uim_get_slots_status_resp_msg_v01));
  if(!slot_status_resp_ptr)
  {
    return QMI_CSI_NO_MEM;
  }
  
  qmi_uim_util_populate_resp(QMI_ERR_NOT_SUPPORTED_V01, &slot_status_resp_ptr->resp);
  
  csi_errval = qmi_uim_send_immidiate_resp(cmd_buf_p, 
                                           slot_status_resp_ptr,
                                           cmd_buf_p->cmd_type,
                                           sizeof(uim_get_slots_status_resp_msg_v01),
                                           cl_sp->client_handle);
  uimqmi_free(slot_status_resp_ptr);
 
  return csi_errval;
} /* qmi_uimi_get_slots_status() */


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
  return FALSE;
} /* qmi_uim_is_sub_manager_supported */
