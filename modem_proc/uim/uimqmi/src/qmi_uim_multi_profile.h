#ifndef QMI_UIM_MULTI_PROFILE_H
#define QMI_UIM_MULTI_PROFILE_H
/*==============================================================================

             Q M I _ U I M _ M U L T I _ P R O F I L E _ P A R S I N G _ H

DESCRIPTION
 
 The Data Services Qualcomm MSM Interface Device Management Services source
 file.

Copyright (c) 2016, 2018 QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary
==============================================================================*/
/*==============================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/uim.mpss/3.3.1/uimqmi/src/qmi_uim_multi_profile.h#1 $

when        who    what, where, why
--------    ---    ------------------------------------------------------------
10/15/18    tq     Migration of QMI UIM service to QCCI-QCSI
06/22/16    sp     Initial Revision.
==============================================================================*/

/*==============================================================================

                          INCLUDE FILES FOR MODULE

==============================================================================*/
#include "uim_p.h"
#include "mmgsdilib_common.h"
#include "qmi_uim.h"

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
    ptr to response or NULL if none.

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
);

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
);

/*===========================================================================
  FUNCTION QMI_UIMI_DELETE_PROFILE()

  DESCRIPTION
    Delete the requested profile from card. The command will go to LPA in all
    cases even if card is not marked as eUICC and LPA needs to reject it.

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
qmi_csi_error qmi_uimi_delete_profile
(
  qmi_uimi_client_state_type  * cl_sp,
  qmi_req_handle                req_handle,
  void                        * req_c_struct,
  int                           req_c_struct_len
);

/*===========================================================================
  FUNCTION QMI_UIMI_EUICC_MEMORY_RESET()

  DESCRIPTION
    Reset the eUICC memory for that Slot. The command will go to LPA in all
    scases even if card is not marked as eUICC and LPA needs to reject it.

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
qmi_csi_error qmi_uimi_euicc_memory_reset
(
  qmi_uimi_client_state_type  * cl_sp,
  qmi_req_handle                req_handle,
  void                        * req_c_struct,
  int                           req_c_struct_len
);

/*===========================================================================
  FUNCTION QMI_UIMI_UPDATE_PROFILE_NICKNAME()

  DESCRIPTION
    Updates the nickname of requested profile. The command will go to LPA in
    all cases even if card is not marked as eUICC and LPA needs to reject it.

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
qmi_csi_error qmi_uimi_update_profile_nickname
(
  qmi_uimi_client_state_type  * cl_sp,
  qmi_req_handle                req_handle,
  void                        * req_c_struct,
  int                           req_c_struct_len
);

/*===========================================================================
  FUNCTION QMI_UIMI_GET_PROFILE_INFO()

  DESCRIPTION
    Provide the complete info of the requested profile. The command
    will go to LPA in all cases even if card is not marked as eUICC
    and LPA needs to reject it.

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
qmi_csi_error qmi_uimi_get_profile_info
(
  qmi_uimi_client_state_type  * cl_sp,
  qmi_req_handle                req_handle,
  void                        * req_c_struct,
  int                           req_c_struct_len
);

/*===========================================================================
  FUNCTION QMI_UIMI_ADD_PROFILE()

  DESCRIPTION
    Add a new profile in card. The command will go to LPA in all cases
    even if card is not marked as eUICC and LPA needs to reject it.

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
qmi_csi_error qmi_uimi_add_profile
(
  qmi_uimi_client_state_type  * cl_sp,
  qmi_req_handle                req_handle,
  void                        * req_c_struct,
  int                           req_c_struct_len
);

/*===========================================================================
  FUNCTION QMI_UIMI_PROFILE_USER_CONSENT()

  DESCRIPTION
    Provides the user consent to LPA for the add_profile operation

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
qmi_csi_error qmi_uimi_profile_user_consent
(
  qmi_uimi_client_state_type  * cl_sp,
  qmi_req_handle                req_handle,
  void                        * req_c_struct,
  int                           req_c_struct_len
);

/*===========================================================================
  FUNCTION QMI_UIMI_EUICC_DEFAULT_SERVER_ADDRESS()

  DESCRIPTION
    Get the default SMDS and SMDP address (if configured) from card.
    Also, configure the value of defualt SMDP address in card.

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
qmi_csi_error qmi_uimi_euicc_default_server_address
(
  qmi_uimi_client_state_type  * cl_sp,
  qmi_req_handle                req_handle,
  void                        * req_c_struct,
  int                           req_c_struct_len
);

/*===========================================================================
  FUNCTION QMI_UIMI_GET_EUICC_PROFILE_RESP()

  DESCRIPTION
    Composes the response for get_sim_profile request

  PARAMETERS
    status                     : status of LPA operation
    get_profiles_info_resp_ptr : result of LPA operation
    cb_userdata_ptr            : user_data to retrieve the cmd

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uimi_get_euicc_profile_resp
(
  void *            sp,
  void *            cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
);

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
);

#endif /* QMI_UIM_MULTI_PROFILE_H*/
