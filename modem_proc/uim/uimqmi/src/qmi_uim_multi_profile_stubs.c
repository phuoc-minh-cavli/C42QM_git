/*==============================================================================

            Q M I _ U I M _ M U L T I _ P R O F I L E _ S T U B S _ C

DESCRIPTION
  The Data Services Qualcomm MSM Interface Device Management Services source
  file.

Copyright (c) 2016, 2018 QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary
==============================================================================*/
/*==============================================================================

                      EDIT HISTORY FOR FILE

$Header: //components/rel/uim.mpss/3.3.1/uimqmi/src/qmi_uim_multi_profile_stubs.c#1 $

when        who    what, where, why
--------    ---    -------------------------------------------------------------
10/15/18    tq     Migration of QMI UIM service to QCCI-QCSI
06/22/16    sp     Initial Revision
==============================================================================*/

/*==============================================================================

                          INCLUDE FILES FOR MODULE

==============================================================================*/

#include"qmi_uim_multi_profile.h"
#include"qmi_uim_util.h"

/*==============================================================================

                             EXTERNAL FUNCTIONS

==============================================================================*/

/*===========================================================================
  FUNCTION QMI_UIMI_GET_SIM_PROFILE()

  DESCRIPTION
    Retrieves the profile information supported by the card

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
qmi_csi_error qmi_uimi_get_sim_profile
(
  qmi_uimi_client_state_type  * cl_sp,
  qmi_req_handle                req_handle,
  void                        * req_c_struct,
  int                           req_c_struct_len
)
{
  uim_get_sim_profile_resp_msg_v01 *resp_ptr             = NULL;
  uint16                            client_index         = 0;
  qmi_uim_cmd_buf_s                *cmd_buf_p            = NULL;
  qmi_csi_error                     csi_errval           = QMI_CSI_NO_ERR;

  ASSERT(cl_sp && req_c_struct && req_c_struct_len > 0 && qmi_uim_state_ptr);

  if(!qmi_uim_util_get_cli_index(cl_sp->client_handle, &client_index))
  {
    return QMI_CSI_INVALID_HANDLE;
  }

  cmd_buf_p = qmi_uim_util_get_cmd_buf_ptr(req_handle, cl_sp->client_handle);

  ASSERT(cmd_buf_p);

  cmd_buf_p->req_handle = req_handle;  

  resp_ptr =   uimqmi_malloc(sizeof(uim_get_sim_profile_resp_msg_v01));
  if(!resp_ptr)
  {
    return QMI_CSI_NO_MEM;
  }
  
  qmi_uim_util_populate_resp(QMI_ERR_NOT_SUPPORTED_V01, &resp_ptr->resp);

  csi_errval = qmi_uim_send_immidiate_resp(cmd_buf_p, 
                                           resp_ptr,
                                           cmd_buf_p->cmd_type,
                                           sizeof(uim_get_sim_profile_resp_msg_v01),
                                           cl_sp->client_handle);
  uimqmi_free(resp_ptr);
 
  return csi_errval;
} /* qmi_uimi_get_sim_profile */


/*===========================================================================
  FUNCTION QMI_UIMI_SET_SIM_PROFILE()

  DESCRIPTION
    Switches the profile in the card

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
qmi_csi_error qmi_uimi_set_sim_profile
(
  qmi_uimi_client_state_type  * cl_sp,
  qmi_req_handle                req_handle,
  void                        * req_c_struct,
  int                           req_c_struct_len
)
{
  uim_set_sim_profile_resp_msg_v01 *resp_ptr             = NULL;
  uint16                            client_index         = 0;
  qmi_uim_cmd_buf_s                *cmd_buf_p            = NULL;
  qmi_csi_error                     csi_errval           = QMI_CSI_NO_ERR;

  ASSERT(cl_sp && req_c_struct && req_c_struct_len > 0 && qmi_uim_state_ptr);
  
  if(!qmi_uim_util_get_cli_index(cl_sp->client_handle, &client_index))
  {
    return QMI_CSI_INVALID_HANDLE;
  }

  cmd_buf_p = qmi_uim_util_get_cmd_buf_ptr(req_handle, cl_sp->client_handle);

  ASSERT(cmd_buf_p);

  cmd_buf_p->req_handle = req_handle;  

  resp_ptr =   uimqmi_malloc(sizeof(uim_set_sim_profile_resp_msg_v01));
  if(!resp_ptr)
  {
    return QMI_CSI_NO_MEM;
  }
  
  qmi_uim_util_populate_resp(QMI_ERR_NOT_SUPPORTED_V01, &resp_ptr->resp);

  csi_errval = qmi_uim_send_immidiate_resp(cmd_buf_p, 
                                           resp_ptr,
                                           cmd_buf_p->cmd_type,
                                           sizeof(uim_set_sim_profile_resp_msg_v01),
                                           cl_sp->client_handle);
  uimqmi_free(resp_ptr);

  return csi_errval;
} /* qmi_uimi_set_sim_profile */


/*===========================================================================
  FUNCTION QMI_UIMI_GET_SIM_PROFILE_RESP()

  DESCRIPTION
    Composes the response for get SIM profile request

  PARAMETERS
    status                  : status of MMGSDI operation
    get_sim_profile_cnf_ptr : result of MMGSDI operation

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uimi_get_sim_profile_resp
(
  mmgsdi_return_enum_type                 status,
  const mmgsdi_get_sim_profile_cnf_type  *get_sim_profile_cnf_ptr
)
{
  (void)status;
  (void)get_sim_profile_cnf_ptr;
} /* qmi_uimi_get_sim_profile_resp */

