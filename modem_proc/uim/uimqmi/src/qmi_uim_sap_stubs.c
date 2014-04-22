/*==============================================================================

                     Q M I _ U I M _ S A P _ S T U B S _ C

DESCRIPTION
  The Data Services Qualcomm MSM Interface Device Management Services source
  file.

Copyright (c) 2016, 2018 - 2019 QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary
==============================================================================*/
/*==============================================================================

                      EDIT HISTORY FOR FILE

$Header: //components/rel/uim.mpss/3.3.1/uimqmi/src/qmi_uim_sap_stubs.c#1 $

when        who    what, where, why
--------    ---    -------------------------------------------------------------
01/22/19    tq     Masked unsupported event registration 
10/15/18    tq     Migration of QMI UIM service to QCCI-QCSI
06/22/16    sp     Initial Revision
==============================================================================*/

/*==============================================================================

                          INCLUDE FILES FOR MODULE

==============================================================================*/
#include "qmi_uim_sap.h"
#include "qmi_uim_util.h"
/*==============================================================================

                             EXTERNAL FUNCTIONS

==============================================================================*/

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
  uim_sap_connection_resp_msg_v01*sap_conn_resp_ptr    = NULL;
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

  sap_conn_resp_ptr =   uimqmi_malloc(sizeof(uim_sap_connection_resp_msg_v01));
  if(!sap_conn_resp_ptr)
  {
    return QMI_CSI_NO_MEM;
  }
  
  qmi_uim_util_populate_resp(QMI_ERR_NOT_SUPPORTED_V01, &sap_conn_resp_ptr->resp);

  csi_errval = qmi_uim_send_immidiate_resp(cmd_buf_p, 
                                           sap_conn_resp_ptr,
                                           cmd_buf_p->cmd_type,
                                           sizeof(uim_sap_connection_resp_msg_v01),
                                           cl_sp->client_handle);
  uimqmi_free(sap_conn_resp_ptr);
 
  return csi_errval;
}/* qmi_uimi_sap_connection */


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
  uim_sap_request_resp_msg_v01   *sap_request_resp_ptr = NULL;
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

  sap_request_resp_ptr =   uimqmi_malloc(sizeof(uim_sap_request_resp_msg_v01));
  if(!sap_request_resp_ptr)
  {
    return QMI_CSI_NO_MEM;
  }
  
  qmi_uim_util_populate_resp(QMI_ERR_NOT_SUPPORTED_V01, &sap_request_resp_ptr->resp);

  csi_errval = qmi_uim_send_immidiate_resp(cmd_buf_p, 
                                           sap_request_resp_ptr,
                                           cmd_buf_p->cmd_type,
                                           sizeof(uim_sap_request_resp_msg_v01),
                                           cl_sp->client_handle);
  uimqmi_free(sap_request_resp_ptr);
 
  return csi_errval;
} /* qmi_uimi_sap_request */


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
  (void)slot_id;
} /* qmi_uim_send_sap_status_indication_to_all */


/*===========================================================================
  FUNCTION QMI_UIM_PROCESS_SAP_EVT()

  DESCRIPTION
    This function processes the SAP events from MMGSDI

  PARAMETERS
    event_ptr: pointer to event

  RETURN VALUE
    TRUE on success. FALSE on failure

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
  (void)event_ptr;
  return FALSE;
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
  return FALSE;
} /* qmi_uim_is_sap_supported */
