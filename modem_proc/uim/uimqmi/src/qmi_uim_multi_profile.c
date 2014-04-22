/*==============================================================================

                   Q M I _ U I M _ M U L T I _ P R O F I L E _ C

DESCRIPTION
  The Data Services Qualcomm MSM Interface Device Management Services source
  file.

Copyright (c) 2016, 2018 QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary
==============================================================================*/
/*==============================================================================

                      EDIT HISTORY FOR FILE

$Header: //components/rel/uim.mpss/3.3.1/uimqmi/src/qmi_uim_multi_profile.c#1 $

when        who    what, where, why
--------    ---    -------------------------------------------------------------
10/15/18    tq     Migration of QMI UIM service to QCCI-QCSI
06/22/16    sp     Initial Revision
==============================================================================*/

/*==============================================================================

                          INCLUDE FILES FOR MODULE

==============================================================================*/

#include "uim_variation.h"
#include "mmgsdilib_common.h"
#include "uim_msg.h"
#include "qmi_uim_util.h"
#include "amssassert.h"
#include "qmi_uim_cat_common.h"
#include "qmi_uim.h"
#include "mmgsdisessionlib_v.h"
#include "fs_public.h"
#include "modem_mem.h"
#include <stringl/stringl.h>

#include "qmi_uim_multi_profile.h"
#include "qmi_uim_parsing.h"

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
  qmi_error_e_type                 errval                         = QMI_ERR_NONE;
  mmgsdi_return_enum_type          mmgsdi_status                  = MMGSDI_SUCCESS;
  qmi_uimi_userdata_type          *cb_userdata_ptr                = NULL;
  uim_get_sim_profile_req_msg_v01  *get_sim_profile_req_ptr       = NULL;
  uim_get_sim_profile_resp_msg_v01 *get_sim_profile_resp_ptr      = NULL;
  qmi_uim_cmd_buf_s                *cmd_buf_p                     = NULL;
  uint16                            client_index                  = 0;
  mmgsdi_slot_id_enum_type          mmgsdi_slot                   = MMGSDI_MAX_SLOT_ID_ENUM;
  qmi_csi_error                     csi_errval                    = QMI_CSI_NO_ERR;

  ASSERT(cl_sp && req_c_struct && req_c_struct_len > 0 && qmi_uim_state_ptr);

  if(!qmi_uim_util_get_cli_index(cl_sp->client_handle, &client_index))
  {
    return QMI_CSI_INVALID_HANDLE;
  }

  cmd_buf_p = qmi_uim_util_get_cmd_buf_ptr(req_handle, cl_sp->client_handle);

  ASSERT(cmd_buf_p);

  get_sim_profile_req_ptr = (uim_get_sim_profile_req_msg_v01 *)req_c_struct;
  cmd_buf_p->req_handle = req_handle;  
  
  errval = qmi_uim_map_and_validate_slot(&mmgsdi_slot, get_sim_profile_req_ptr->slot);
  if(errval != QMI_ERR_NONE_V01)
  {
    goto send_result;
  }

  /* Create userdata */
  cb_userdata_ptr = qmi_uim_util_get_cb_userdata(QMI_UIM_GET_SIM_PROFILE_REQ_V01,
                                                 cmd_buf_p,
                                                 NULL,
                                                 cl_sp->client_handle);
  if (cb_userdata_ptr == NULL)
  {
    errval = QMI_ERR_NO_MEMORY_V01;
    goto send_result;
  }

  /* Execute MMGSDI function to get sim profile */
  mmgsdi_status = mmgsdi_get_sim_profile(
                    qmi_uim_global_ptr->mmgsdi_client_id,
                    mmgsdi_slot,
                    qmi_uimi_mmgsdi_callback,
                    (mmgsdi_client_data_type)cb_userdata_ptr );

  /* Convert MMGSDI status in QMI code */
  errval = qmi_uim_util_convert_mmgsdi_status_to_errval(mmgsdi_status);

  /* Free userdata in case of error */
  QMI_UIM_UTIL_FREE_USERDATA_IF_ERROR(mmgsdi_status, cb_userdata_ptr);

send_result:
  if(errval == QMI_ERR_NONE_V01)
  {
    cmd_buf_p->resp_pending = TRUE;
    return QMI_CSI_NO_ERR;
  }
  
  if(get_sim_profile_resp_ptr == NULL)
  {
    get_sim_profile_resp_ptr =   uimqmi_malloc(sizeof(uim_get_sim_profile_resp_msg_v01));
    if(!get_sim_profile_resp_ptr)
    {
      return QMI_CSI_NO_MEM;
    }
  }
  
  qmi_uim_util_populate_resp(errval, &get_sim_profile_resp_ptr->resp);

  csi_errval = qmi_uim_send_immidiate_resp(cmd_buf_p, 
                                           get_sim_profile_resp_ptr,
                                           cmd_buf_p->cmd_type,
                                           sizeof(uim_get_sim_profile_resp_msg_v01),
                                           cl_sp->client_handle);
  uimqmi_free(get_sim_profile_resp_ptr);
 
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
    ptr to response or NULL if none.

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
  mmgsdi_return_enum_type           mmgsdi_status                = MMGSDI_SUCCESS;
  qmi_uimi_userdata_type*           cb_userdata_ptr              = NULL;
  qmi_error_type_v01                errval                       = QMI_ERR_NONE_V01;
  uim_set_sim_profile_req_msg_v01  *set_sim_profile_req_ptr      = NULL;
  uim_set_sim_profile_resp_msg_v01 *set_sim_profile_resp_ptr     = NULL;
  uint16                            client_index                 = 0;
  qmi_uim_cmd_buf_s                *cmd_buf_p                    = NULL;
  qmi_csi_error                     csi_errval                   = QMI_CSI_NO_ERR;
  mmgsdi_slot_id_enum_type          mmgsdi_slot                  = MMGSDI_MAX_SLOT_ID_ENUM;

  ASSERT(cl_sp && req_c_struct && req_c_struct_len > 0 && qmi_uim_state_ptr);

  if(!qmi_uim_util_get_cli_index(cl_sp->client_handle, &client_index))
  {
    return QMI_CSI_INVALID_HANDLE;
  }

  cmd_buf_p = qmi_uim_util_get_cmd_buf_ptr(req_handle, cl_sp->client_handle);

  ASSERT(cmd_buf_p);

  set_sim_profile_req_ptr = (uim_set_sim_profile_req_msg_v01 *)req_c_struct;
  cmd_buf_p->req_handle = req_handle;  
  
  errval = qmi_uim_map_and_validate_slot(&mmgsdi_slot, set_sim_profile_req_ptr->slot);
  if(errval != QMI_ERR_NONE_V01)
  {
    goto send_result;
  }

  if(set_sim_profile_req_ptr->profile_id < UIM_PROFILE_ID_1_V01 || 
     set_sim_profile_req_ptr->profile_id > UIM_PROFILE_ID_8_V01 )
  {
    errval = QMI_ERR_INVALID_ARG_V01;
    goto send_result;
  }

  /* Create userdata */
  cb_userdata_ptr = qmi_uim_util_get_cb_userdata(QMI_UIM_SET_SIM_PROFILE_REQ_V01,
                                                 cmd_buf_p,
                                                 NULL,
                                                 cl_sp->client_handle);
  if (cb_userdata_ptr == NULL)
  {
    errval = QMI_ERR_NO_MEMORY_V01;
    goto send_result;
  }


  /* Execute MMGSDI function to set SIM profile */
  mmgsdi_status = mmgsdi_set_sim_profile(
                    qmi_uim_global_ptr->mmgsdi_client_id,
                    mmgsdi_slot,
                    (mmgsdi_profile_id_enum_type)set_sim_profile_req_ptr->profile_id,
                    qmi_uimi_mmgsdi_callback,
                    (mmgsdi_client_data_type)cb_userdata_ptr);

  /* Convert MMGSDI status in QMI code */
  errval = qmi_uim_util_convert_mmgsdi_status_to_errval(mmgsdi_status);

  /* Free userdata in case of error */
  QMI_UIM_UTIL_FREE_USERDATA_IF_ERROR(mmgsdi_status, cb_userdata_ptr);

send_result:
if(errval == QMI_ERR_NONE_V01)
  {
    cmd_buf_p->resp_pending = TRUE;
    return QMI_CSI_NO_ERR;
  }

  set_sim_profile_resp_ptr =   uimqmi_malloc(sizeof(uim_set_sim_profile_resp_msg_v01));
  if(!set_sim_profile_resp_ptr)
  {
    return QMI_CSI_NO_MEM;
  }
  
  qmi_uim_util_populate_resp(errval, &set_sim_profile_resp_ptr->resp);

  csi_errval = qmi_uim_send_immidiate_resp(cmd_buf_p, 
                                           set_sim_profile_resp_ptr,
                                           cmd_buf_p->cmd_type,
                                           sizeof(uim_set_sim_profile_resp_msg_v01),
                                           cl_sp->client_handle);
  
  uimqmi_free(set_sim_profile_resp_ptr);
 
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
  qmi_uimi_userdata_type            * cb_userdata_ptr          = NULL;
  qmi_error_type_v01                  errval                   = QMI_ERR_NONE_V01;
  uim_get_sim_profile_resp_msg_v01   *get_sim_profile_resp_ptr = NULL;
  uint16                              client_index             = 0;
  qmi_uim_cmd_buf_s                  *cmd_buf_ptr              = NULL;

  UIM_MSG_HIGH_0("Sending response for get sim profile");

  ASSERT(get_sim_profile_cnf_ptr);

  /* Retrieve userdata */
  cb_userdata_ptr =
    (qmi_uimi_userdata_type*)get_sim_profile_cnf_ptr->response_header.client_data;
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
  if(!cb_userdata_ptr->ind_token.is_valid && cmd_buf_ptr == NULL)
  {
    goto free_cmd_data;
  }

  get_sim_profile_resp_ptr = (uim_get_atr_resp_msg_v01*) uimqmi_malloc(sizeof(uim_get_atr_resp_msg_v01));
  if(get_sim_profile_resp_ptr == NULL)
  {
    goto free_cmd_data;
  }
  
  /* Insert APDU response in the TLV, in case of success */
  if (status == MMGSDI_SUCCESS)
  {
    uint8 num_profiles  = MIN(get_sim_profile_cnf_ptr->num_profile, MMGSDI_MAX_PROFILE);
    uint8 profile_index = 0;

    for(profile_index = 0; profile_index < num_profiles; profile_index++)
    {
      get_sim_profile_resp_ptr->profile_info[profile_index].profile_id =
        get_sim_profile_cnf_ptr->profile_list[profile_index].profile_id;
      get_sim_profile_resp_ptr->profile_info[profile_index].profile_type =
        get_sim_profile_cnf_ptr->profile_list[profile_index].profile_type;
      get_sim_profile_resp_ptr->profile_info_valid = 1;
      get_sim_profile_resp_ptr->profile_info_len++;
    }

    get_sim_profile_resp_ptr->active_profile_info.profile_id =
      get_sim_profile_cnf_ptr->active_profile.profile_id;
    get_sim_profile_resp_ptr->active_profile_info.profile_type =
      get_sim_profile_cnf_ptr->active_profile.profile_type;
    get_sim_profile_resp_ptr->active_profile_info_valid = 1;
  }
  else
  {
    /* Convert MMGSDI status to QMI err code */
    errval = qmi_uim_util_convert_mmgsdi_status_to_errval(status);
  }

  qmi_uim_util_populate_resp(errval, &get_sim_profile_resp_ptr->resp);

  qmi_uim_send_response_ind( 
    cmd_buf_ptr,
    get_sim_profile_resp_ptr,
    cb_userdata_ptr->request_id,
    sizeof(uim_get_sim_profile_resp_msg_v01),
    FALSE,
    cb_userdata_ptr->client_handle);

  uimqmi_free(get_sim_profile_resp_ptr);

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
} /* qmi_uimi_get_sim_profile_resp */
