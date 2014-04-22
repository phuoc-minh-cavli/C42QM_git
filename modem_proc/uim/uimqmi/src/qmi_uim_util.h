#ifndef QMI_UIM_UTIL_H
#define QMI_UIM_UTIL_H
/*===========================================================================

                         Q M I _ U I M _ U T I L. C

DESCRIPTION

 The Data Services Qualcomm MSM Interface Device Management Services source
 file.

Copyright (c) 2016, 2018 QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //components/rel/uim.mpss/3.3.1/uimqmi/src/qmi_uim_util.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/15/18    tq     Migration of QMI UIM service to QCCI-QCSI
07/21/16    sp     Seperate ERA-GLONASS and SAP related QMI UIM functionality
07/14/16    ar     Move qmi_uimi_recovery and unify fdn/acl get_status func.
05/19/16    vdc    Initial Revision.
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "qmi_uim.h"
#include "qmi_uim_parsing.h"
#include "mmgsdilib_common.h"

/*===========================================================================

                            CONSTANT DEFINITIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Macro used to free userdata in case of error
---------------------------------------------------------------------------*/
#define QMI_UIM_UTIL_FREE_USERDATA_IF_ERROR(mmgsdi_status, cb_userdata_ptr)  \
            if (mmgsdi_status != MMGSDI_SUCCESS)                        \
            {                                                           \
              qmi_uim_util_free_cb_userdata(cb_userdata_ptr);                \
              cb_userdata_ptr = NULL;                                   \
            }

/*---------------------------------------------------------------------------
  Macro used to free userdata in case of error
---------------------------------------------------------------------------*/
#define QMI_UIM_UTIL_LPA_FREE_USERDATA_IF_ERROR(lpa_status, cb_userdata_ptr)  \
            if (lpa_status != LPA_SUCCESS)                               \
            {                                                            \
              qmi_uim_util_free_cb_userdata(cb_userdata_ptr);                 \
              cb_userdata_ptr = NULL;                                    \
            }

/*===========================================================================

                             EXTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
  FUNCTION QMI_UIM_UTIL_CHECK_CLID()

  DESCRIPTION
    To validate if the passed client handle is still valid within the UIM
    service state ptr

  PARAMETERS
    client_handl[In]      : client handle provided by QCSI - QCCI, unique for each client
    client_reg_count[In]  : number o time time this clinet is deregistered
    cli_inx[Out]          : clinet index 

  RETURN VALUE
    TRUE if successful, else FALSE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_util_check_clid
(
  qmi_client_handle             client_handle,
  uint32                        client_reg_count,
  uint16                       *cli_inx
);

/*===========================================================================
  FUNCTION QMI_UIM_UTIL_RESPONSE_IND_TOKEN()

  DESCRIPTION
    Composes the TLV content of indication token

  PARAMETERS
    tlv_id             : id to be used for the TLV
    response_pptr      : output sdu
    token              : token info to be sent

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_util_response_ind_token
(
  byte                            tlv_id,
  dsm_item_type **                response_pptr,
  uint32                          token
);

/*===========================================================================
  FUNCTION QMI_UIM_UTIL_GET_CB_USERDATA

  DESCRIPTION
    To obtain the structure used for MMGSDI callbacks

  PARAMETERS
    request_id             : Type of the request
    cmd_buf_ptr            : QMI command buffer required for response
    token_ptr              : pointer to token info sent by client
    cli_handle             : Client handle to identify the clinet

  RETURN VALUE
    qmi_uimi_userdata_type : Poiner to the structure used for userdata

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_uimi_userdata_type* qmi_uim_util_get_cb_userdata
(
  uint16                     request_id,
  qmi_uim_cmd_buf_s        * cmd_buf_ptr,
  const uint32             * token_ptr,
  qmi_client_handle          cli_handle
);

/*===========================================================================
  FUNCTION QMI_UIM_UTIL_CREATE_MESSAGE

  DESCRIPTION
    Allocates the memory for a QMI UIM message and initializes it.

  PARAMETERS
    message_type: QMI UIM message type

  RETURN VALUE
    Pointer to the allocated message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_uim_message_type * qmi_uim_util_create_message
(
  qmi_uim_message_enum_type message_type
);

/*===========================================================================
  FUNCTION QMI_UIM_UTIL_POST_MESSAGE

  DESCRIPTION
    Adds a message to the QMI UIM queue and posts an event to DCC task,
    so that it can be processed later in the right context.

  PARAMETERS
    uim_message_ptr: QMI UIM message

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_util_post_message
(
  qmi_uim_message_type * uim_message_ptr
);

/*===========================================================================
  FUNCTION QMI_UIM_UTIL_FREE_CB_USERDATA

  DESCRIPTION
    To free the structure passed as user data while invoking APIs.
    It is safe to call it from different task context when API callback is hit.

  PARAMETERS
    cb_userdata_ptr        : Callback user data pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_util_free_cb_userdata
(
  qmi_uimi_userdata_type*  cb_userdata_ptr
);

/*===========================================================================
  FUNCTION QMI_UIM_UTIL_SAME_FILE

  DESCRIPTION
    Compares to files passed by path.

  PARAMETERS
    file1         : first file
    file2         : second file

  RETURN VALUE
    boolean       : Indicates if the two files are the same

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_util_same_file
(
  mmgsdi_path_type         file1,
  mmgsdi_path_type         file2
);

/*===========================================================================
  FUNCTION QMI_UIM_UTIL_CONVERT_MMGSDI_STATUS_TO_ERRVAL

  DESCRIPTION
    To obtain QMI client-specific error message from MMGSDI error message

  PARAMETERS
    mmgsdi_status   : MMGSDI return value

  RETURN VALUE
    errval          : QMI client-specific error message of 
                      type qmi_error_type_v01

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_uim_util_convert_mmgsdi_status_to_errval
(
  mmgsdi_return_enum_type mmgsdi_status
);

/*===========================================================================
  FUNCTION QMI_UIM_UTIL_SESSION_TYPE_TO_SESSION_ID

  DESCRIPTION
    Convert a session type and AID in the corresponding session id.

  PARAMETERS
    session_id_ptr : Output parameter with session id
    session_info   : session type
    aid            : AID value

  RETURN VALUE
    errval         : QMI client-specific error message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_uim_util_session_type_to_session_id
(
  mmgsdi_session_id_type *        session_id_ptr,
  mmgsdi_session_type_enum_type   session_type,
  mmgsdi_static_data_type         aid
);

/*===========================================================================
  FUNCTION QMI_UIM_UTIL_SESSION_ID_TO_SESSION_TYPE

  DESCRIPTION
    Convert a session id in the corresponding session type and AID.

  PARAMETERS
    session_type_ptr : Output parameter with session type
    aid_ptr          : Output parameter with AID
    session_id       : session id

  RETURN VALUE
    result           : QMI client-specific error message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_uim_util_session_id_to_session_type
(
  mmgsdi_session_type_enum_type * session_type_ptr,
  mmgsdi_static_data_type *       aid_ptr,
  mmgsdi_session_id_type          session_id
);

/*===========================================================================
  FUNCTION QMI_UIM_UTIL_SESSION_TYPE_TO_CARD_TYPE

  DESCRIPTION
    Convert a session type and AID in the corresponding card type.

  PARAMETERS
    card_type_ptr : Output parameter with card type
    session_info  : session type
    aid           : AID value

  RETURN VALUE
    errval         : QMI client-specific error message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_uim_util_session_type_to_card_type
(
  mmgsdi_app_enum_type *          card_type_ptr,
  mmgsdi_session_type_enum_type   session_type,
  mmgsdi_static_data_type         aid
);

/*===========================================================================
  FUNCTION QMI_UIM_UTIL_SESSION_ID_TO_SLOT

  DESCRIPTION
    Convert a session id in the corresponding slot.

  PARAMETERS
    slot_ptr      : Output parameter with slot value
    session_id    : session id

  RETURN VALUE
    errval        : QMI client-specific error message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_uim_util_session_id_to_slot
(
  mmgsdi_slot_id_enum_type * slot_ptr,
  mmgsdi_session_id_type     session_id
);

/*===========================================================================
  FUNCTION QMI_UIM_UTIL_CARD_PROTOCOL()

  DESCRIPTION
    Utility function to find out if the card is ICC or UICC.

  PARAMETERS
    slot                       : slot

  RETURN VALUE
    mmgsdi_protocol_enum_type  : Whether the card is ICC or UICC

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
mmgsdi_protocol_enum_type qmi_uim_util_card_protocol
(
  mmgsdi_slot_id_enum_type  slot
);

/*===========================================================================
  FUNCTION QMI_UIM_UTIL_SESSION_ID_TO_REFRESH_INDEX

  DESCRIPTION
    Convert a session id into an index.

  PARAMETERS
    refresh_index     : output parameter with index used for refresh.
    session_id    : session id

  RETURN VALUE
    errval        : QMI client-specific error message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_uim_util_session_id_to_refresh_index
(
  uint16                 * refresh_index_ptr,
  mmgsdi_session_id_type   session_id
);

/*===========================================================================
  FUNCTION QMI_UIM_UTIL_MMGDSI_SLOT_ID_TO_UIM_SLOT_ID()

  DESCRIPTION
    Converts MMGSDI slot id to UIM slot id.

  PARAMETERS
    mmgsdi_slot_id : MMGSDI slot id
    uim_slot_id    : output parameter containing UIM slot id

  RETURN VALUE
    QMI client-specific error message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_uim_util_mmgsdi_slot_id_to_uim_slot_id
(
  mmgsdi_slot_id_enum_type  mmgsdi_slot_id,
  uim_slot_type            *uim_slot_id
);

/*===========================================================================
  FUNCTION QMI_UIM_UTIL_UIM_SLOT_ID_TO_MMGSDI_SLOT_ID()

  DESCRIPTION
    Converts UIM slot id to MMGSDI slot id.

  PARAMETERS
    uim_slot_id         : UIM slot id
    mmgsdi_slot_id_ptr  : output parameter containing MMGSDI slot id

  RETURN VALUE
    QMI client-specific error message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_uim_util_uim_slot_id_to_mmgsdi_slot_id
(
  uim_slot_type             uim_slot_id,
  mmgsdi_slot_id_enum_type *mmgsdi_slot_id_ptr
);

/*===========================================================================
  FUNCTION QMI_UIM_UTIL_SLOT_INDEX_TO_MMGSDI_SLOT()

  DESCRIPTION
    Determines the MMGSDI slot id from the slot index

  PARAMETERS
    slot_index          : slot index
    mmgsdi_slot_ptr     : output parameter containing MMGSDI slot id

  RETURN VALUE
    QMI client-specific error message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_uim_util_slot_index_to_mmgsdi_slot
(
  uint8                              slot_index,
  mmgsdi_slot_id_enum_type          *mmgsdi_slot_ptr
);

/*===========================================================================
  FUNCTION QMI_UIM_UTIL_GSTK_SLOT_ID_TO_MMGSDI_SLOT_ID()

  DESCRIPTION
    Converts GSTK slot id to MMGSDI slot id.

  PARAMETERS
    gstk_slot_id        : GSTK slot id
    mmgsdi_slot_id_ptr  : output parameter containing MMGSDI slot id

  RETURN VALUE
    QMI client-specific error message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_uim_util_gstk_slot_id_to_mmgsdi_slot_id
(
  gstk_slot_id_enum_type     gstk_slot_id,
  mmgsdi_slot_id_enum_type  *mmgsdi_slot_id_ptr
);

/*===========================================================================
  FUNCTION QMI_UIM_CONVERT_SLOT_TO_INDEX()

  DESCRIPTION
    Converts MMGSDI slot information into index, starting from 0.

  PARAMETERS
    slot_id : MMGSDI slot information
    index   : output parameter with index value

  RETURN VALUE
    boolean to indicate success or failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_convert_slot_to_index
(
  mmgsdi_slot_id_enum_type  slot_id,
  uint8 *                   index_ptr
);

/*===========================================================================
  FUNCTION QMI_UIM_UTIL_IS_APDU_SESSION

  DESCRIPTION
    Check if the given session id belongs to an APDU session

  PARAMETERS
    session_id    : session id

  RETURN VALUE
    boolean       : indicates if the session id belongs to an APDU session

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_util_is_apdu_session
(
  mmgsdi_session_id_type   session_id
);

/*===========================================================================
  FUNCTION QMI_UIM_UTIL_READ_IMSI_FOR_SESSION_ID()

  DESCRIPTION
    Reads the IMSI from the card for the specified Session_ID

  PARAMETERS
    session_id       : Session ID
    imsi_ptr         : Pointer for IMSI data

  RETURN VALUE
    errval           : QMI client-specific error message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_uim_util_read_imsi_for_session_id
(
  mmgsdi_session_id_type session_id,
  qmi_uim_imsi_type*     imsi_ptr
);

/*===========================================================================
  FUNCTION QMI_UIM_UTIL_UPDATE_NON_PROV_SESSION_CL_SP()

  DESCRIPTION
    Extracts the Session Info from the session ptr, and if it is a
    non-prov. session already present in the global array, then the
    passed in cl_sp is associated with the session.

  PARAMETERS
  qmi_uim_session_information_type     : session_ptr,
  qmi_uimi_client_state_type           : client_sp

  RETURN VALUE
    errval         : QMI client-specific error message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_uim_util_update_non_prov_session_cl_sp
(
  qmi_uim_session_information_type     * session_ptr,
  qmi_uimi_client_state_type           * client_sp
);

/*===========================================================================
  FUNCTION QMI_UIM_UTIL_ADD_AID()

  DESCRIPTION
    Utility function to add an application

  PARAMETERS
    aid_ptr  : pointer to AID + application type
    slot     : slot

  RETURN VALUE
    uint16: indicates the index of the application. The function
            return UIM_INVALID_APP_INDEX if the application could not
            be added

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
uint16 qmi_uim_util_add_aid
(
  const mmgsdi_aid_type*   aid_ptr,
  mmgsdi_slot_id_enum_type slot
);

/*===========================================================================
  FUNCTION QMI_UIM_UTIL_FIND_AID()

  DESCRIPTION
    Utility function to locate the application with the specified AID

  PARAMETERS
    aid_ptr  : pointer to AID + application type
    slot     : slot

  RETURN VALUE
    uint16: indicates the index of the application. The function
            return UIM_INVALID_APP_INDEX if the application is not
            found

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
uint16 qmi_uim_util_find_aid
(
  const mmgsdi_aid_type *  aid_ptr,
  mmgsdi_slot_id_enum_type slot
);

/*===========================================================================
  FUNCTION QMI_UIM_UTIL_REMOVE_ALL_AID_IN_SLOT()

  DESCRIPTION
    Utility function to remove all applications for a specific slot

  PARAMETERS
    slot     : slot

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_util_remove_all_aid_in_slot
(
  mmgsdi_slot_id_enum_type slot
);

/*===========================================================================
  FUNCTION QMI_UIM_UTIL_IS_PROVISIONING_APP_INDEX()

  DESCRIPTION
    Checks if the given index is associated with a provisioning application

  PARAMETERS
    index : index of application

  RETURN VALUE
    TRUE  : If the index is associated with prov app
    FALSE : If the index is not associated with prov app

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_util_is_provisioning_app_index
(
  uint16 index
);

/*===========================================================================
  FUNCTION QMI_UIM_UTIL_IS_REQUEST_REJECTED()

  DESCRIPTION
    If APDU security restrictions are enabled on the device, this function
    checks the incoming request to determine if it should be rejected. It
    will be rejected if the logical channel in request doesnt belongs to the
    requesting client.

  PARAMETERS
    session_type   : session type
    aid            : AID value
    cl_sp          : Client state pointer

  RETURN VALUE
    Boolean indicating if the request is to be rejected or not.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_util_is_request_rejected
(
  mmgsdi_session_type_enum_type   session_type,
  mmgsdi_static_data_type         aid,
  qmi_uimi_client_state_type    * cl_sp
);

/*===========================================================================
  FUNCTION QMI_UIM_util_GENERIC_RESP_ALLOC_POPULATE()

  DESCRIPTION
    This function allocate the memory for response structure corresponding 
    to request type. it also populate response and return void pointer.

  PARAMETERS
    errval    : error value
    cmd_type  : request type
    resp_len  : Output response len

  RETURN VALUE
    response pointer of void type. it will be null in fail case

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void* qmi_uim_util_generic_resp_alloc_populate
(
  qmi_error_type_v01                                errval,
  uint16                                            cmd_type,
  uint16                                           *resp_len
);

/*===========================================================================
  FUNCTION QMI_UIM_UTIL_GET_CLI_INDEX()

  DESCRIPTION
    This function checks if client already registered with 
    QMI UIM service, if yes then it return client index

  PARAMETERS
    cli_handle[in]: Client Handle
    cli_index[out]: Client Index

  RETURN VALUE
    TRUE:  if already registered
    FALSE: otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_util_get_cli_index
(
  qmi_client_handle             cli_handle,
  uint16                       *cli_index
);

/*===========================================================================
  FUNCTION QMI_UIM_UTIL_MAP_TO_MMGSDI_SESSION_TYPE()

  DESCRIPTION
    This function will map uim_session_type_enum_v01 to 
    mmgsdi_session_type_enum_type.

  PARAMETERS
    uim_session_type_enum_v01    :  uim_session_type,
    mmgsdi_session_type_enum_type: *mmgsdi_session_ptr 

  RETURN VALUE
    qmi_error_type_v01

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_uim_util_map_to_mmgsdi_session_type
(
  uim_session_type_enum_v01             uim_session_type,
  mmgsdi_session_type_enum_type        *mmgsdi_session_ptr                           
);

/*===========================================================================
  FUNCTION QMI_UIM_REMOTE_MAP_CSI_TO_CSI_CB_ERR

  DESCRIPTION
    Function converts a CSI error to a CSI callback error type

  PARAMETERS
   qmi_csi_error

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
 qmi_csi_cb_error qmi_uim_util_map_csi_to_csi_cb_err (
  qmi_csi_error  csi_error
);

/*===========================================================================
FUNCTION QMI_UIM_COMPARE_QMI_CMD

DESCRIPTION
This function will be called by q_linear_search() to check if perticular cmd
is present in the queue or not.

PARAMETER
  item_ptr  [Input]:  ptr to queue item
  param_ptr [Input]:  qmi_uim_cmd_buf

DEPENDENCIES
  None

RETURN VALUE
  1 - if there is a match
  0 - if there is no match

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
int qmi_uim_util_compare_qmi_cmd
(
  void *item_ptr,
  void *param_ptr
);

/*===========================================================================
FUNCTION QMI_UIM_UTIL_REMOVE_QMI_CMD

DESCRIPTION
  This function will remove the perticular cmd from the 
  client cmd q and free it, which match with provided
  req_handle and client_handle.
  
PARAMETER
  q_ptr  [IN]        :     ptr to queue
  qmi_req_handle[IN] :     req_handle
  uint32[IN]         :     client_handle

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None

SEE ALSO
  None

===========================================================================*/
void qmi_uim_util_remove_qmi_cmd
(
  qmi_req_handle      req_handle,
  qmi_client_handle   client_handle,
  q_type             *q_ptr
);

/*===========================================================================
  FUNCTION QMI_UIM_SEND_RESPONSE_IND

  DESCRIPTION
    This function validated the client and then send response or indication to
    the client depends upon its type.

  PARAMETERS
    qmi_uim_cmd_buf_s *      cmd_buf_ptr
    void *                   response_ptr
    uint16                   cmd_type
    uint16                   response_size
    boolean                  ind_required
    qmi_client_handle        cli_handle    

  DEPENDENCIES
    None

  RETURN VALUE
    qmi_csi_error

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_error qmi_uim_send_response_ind
(
  qmi_uim_cmd_buf_s *      cmd_buf_ptr,
  void *                   response_ptr,
  uint16                   cmd_type,
  uint16                   response_size,
  boolean                  ind_required,
  qmi_client_handle        cli_handle
);

/*===========================================================================
  FUNCTION QMI_UIM_SEND_IMMIDIATE_RESP

  DESCRIPTION
    This function validated the client and then send immidiate response
    to the client and free the cmd data from clinet queue.

  PARAMETERS
    qmi_uim_cmd_buf_s *      cmd_buf_ptr
    void *                   response_ptr
    uint16                   cmd_type
    uint16                   response_size
    qmi_client_handle        cli_handle    

  DEPENDENCIES
    None

  RETURN VALUE
    qmi_csi_error

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_error qmi_uim_send_immidiate_resp
(
  qmi_uim_cmd_buf_s       *cmd_buf_ptr,
  void                    *response_ptr,
  uint16                   cmd_type,
  uint16                   response_size,
  qmi_client_handle        cli_handle
);

/*===========================================================================
  FUNCTION QMI_UIM_UTIL_GET_BUF_PTR()

  DESCRIPTION
   This function uses client handle and request handle to check
   if any cmd is available in the client cmd q corresponding
   to provided clinet handle.

  PARAMETERS
    qmi_req_handle        req_handle
    qmi_client_handle     cli_handle

  RETURN VALUE
    qmi_uim_cmd_buf_s

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_uim_cmd_buf_s* qmi_uim_util_get_cmd_buf_ptr
(
  qmi_req_handle        req_handle,
  qmi_client_handle     cli_handle
);

/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_CARD_RESULT()

  DESCRIPTION
    Populate card result

  PARAMETERS
    uim_card_result_type_v01 *card_result_ptr[OUT]
    uint8*                   *card_result_valid[OUT]
    mmgsdi_sw_type            status_word[IN]

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_poplate_card_result
(
  uint8                    *card_result_valid,
  uim_card_result_type_v01 *card_result_ptr,
  mmgsdi_sw_type            status_word
);

/*===========================================================================
  FUNCTION QMI_UIMI_UTIL_POPULATE_RESP()

  DESCRIPTION
    Populates the response structure

  PARAMETERS  
    qmi_error_type_v01        errval[IN]
    qmi_response_type_v01   * resp[OUT]

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_util_populate_resp
(
  qmi_error_type_v01        errval,
  qmi_response_type_v01   * resp
);

/*===========================================================================
  FUNCTION QMI_UIM_UTIL_POPULATE_READ_RESULT()

  DESCRIPTION
    Populate read data reust depend upon encryption status.

  PARAMETERS
  read_result_data[OUT]             : output read data buffer
  read_result_data_len[OUT]         : output read data len
  read_result_data_size[IN]         : outpt read data buffer size
  read_result_valid[OUT]            : read data is valid or not
  encryption_status[IN]             : encryption status type
  read_data[IN]                     : input read data
  cmd_id[IN]                        : cmd type
  session_type[IN]                  : session type 
  file_access[IN]                   : file access details
  encryption_used[OUT]              : encryption used in output read data or not

  RETURN VALUE
    qmi_error_type_v01

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_uim_util_populate_read_result
(
  uint8                                 *read_result_data_ptr,
  uint32                                *read_result_data_len_ptr,
  uint32                                 read_result_data_size,
  uint8                                 *read_result_valid_ptr,
  qmi_uim_requested_encrytion_type       encryption_status,
  mmgsdi_data_type                       read_data,
  uint16                                 cmd_id,
  mmgsdi_session_type_enum_type          session_type,
  mmgsdi_access_type                     file_access,
  boolean                               *encryption_used_ptr
);

/*===========================================================================
  FUNCTION QMI_UIM_UTIL_POPULATE_UINT32()

  DESCRIPTION
    It populate the uint32 data as well as mark valid 
    flag as true

  PARAMETERS
    field_valid_ptr[OUT] : output data valid or not
    field_data_ptr[OUT]  : output data 
    data[IN]             : input data

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_util_populate_uint32
(
  uint8                          * field_valid_ptr,
  uint32                         * field_data_ptr,
  uint32                           data
);

/*===========================================================================
  FUNCTION QMI_UIM_UTIL_POPULATE_UINT16()

  DESCRIPTION
    It populate the uint32 data as well as mark valid 
    flag as true

  PARAMETERS
    field_valid_ptr[OUT] : output data valid or not
    field_data_ptr[OUT]  : output data 
    data[IN]             : input data

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_util_populate_uint16
(
  uint8                          * field_valid_ptr,
  uint16                         * field_data_ptr,
  uint32                           data
);

/*===========================================================================
  FUNCTION QMI_UIM_UTIL_POPULATE_OPAQUE()

  DESCRIPTION
    Populate opeque field and field mark as valid.

  PARAMETERS 
    field_valid_ptr[OUT]         : output data is valid or not
    field_data_buf_ptr[OUT]      : output data buf
    field_data_len_ptr[OUT]      : output data len 
    field_data_buf_size[IN]      : output data buf size
    data_ptr[IN]                 : input data
    data_len[IN]                 : input data len
    mandatory                    : TLV mandatory or not

  RETURN VALUE
    boolean

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_util_populate_opaque
(
  uint8        * field_valid_ptr,
  uint8        * field_data_buf_ptr,
  uint32       * field_data_len_ptr,  
  uint32         field_data_buf_size,
  uint8        * data_ptr,
  uint32         data_len,
  boolean        mandatory
);

#endif /* QMI_UIM_UTIL_H */
