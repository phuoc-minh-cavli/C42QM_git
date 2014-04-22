#ifndef QMI_UIM_SIMLOCK_H
#define QMI_UIM_SIMLOCK_H
/*===========================================================================

                         Q M I _ U I M _ S I M L O C K. H

DESCRIPTION

 The Data Services Qualcomm MSM Interface Device Management Services source
 file.

Copyright (c) 2016, 2018, 2021 QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //components/rel/uim.mpss/3.3.1/uimqmi/src/qmi_uim_simlock.h#2 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
07/07/21    tq     Enabled support for SIMLOCK
10/15/18    tq     Migration of QMI UIM service to QCCI-QCSI
07/26/16    sp     Move qmi_uimi_ functions to corresponding files
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
#include "ds_qmi_fw_common.h"
#include "qmi_uim.h"

/*===========================================================================

                            CONSTANT DEFINITIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Macro used to free userdata in case of error
---------------------------------------------------------------------------*/
#define QMI_UIM_SIMLOCK_FREE_USERDATA_IF_ERROR(simlock_status, cb_userdata_ptr)  \
            if (simlock_status != SIMLOCK_SUCCESS)                               \
            {                                                                    \
              qmi_uim_util_free_cb_userdata(cb_userdata_ptr);                         \
              cb_userdata_ptr = NULL;                                            \
            }

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

/*===========================================================================
  FUNCTION QMI_UIMI_REMOTE_UNLOCK()

  DESCRIPTION
    This function provides following functionality based on the type of
    request:
    Provides the key in the encrypted format OR
    Applies the simlock configuration data provided as the input OR
    Generates HMAC on the input data OR
    Retrieve the version of blob supported by Simlock engine

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
);

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
);

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
);

/*===========================================================================
  FUNCTION QMI_UIM_SIMLOCK_CHECK_AND_DEREGISTER_REMOTE_SFS_OPERATION()

  DESCRIPTION
    This function deregisters the QMI UIM callback with the simlock.

  PARAMETERS
    qmi_client_handle client_handle

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
);

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
);
#endif /* QMI_UIM_SIMLOCK_H */
