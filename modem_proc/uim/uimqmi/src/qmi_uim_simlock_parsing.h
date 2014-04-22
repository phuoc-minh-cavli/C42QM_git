/*================================================================================

                         Q M I _ U I M _ S I M L O C K _ P A R S I N G. H


DESCRIPTION
  This file contains functions for QMI UIM service's SIMLOCK TLV parsing functionality.

Copyright (c) 2016, 2018, 2021 QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary

$Header: //components/rel/uim.mpss/3.3.1/uimqmi/src/qmi_uim_simlock_parsing.h#2 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
07/07/21    tq     Enabled support for SIMLOCK
10/15/18    tq     Migration of QMI UIM service to QCCI-QCSI
06/16/16    vdc    Initial Revision
=============================================================================*/

#include "uim_variation.h"
#include "customer.h"
#include "uim_msg.h"
#include "qmi_uim_parsing.h"
#include "simlock_common.h"

#ifdef FEATURE_SIMLOCK
/*===========================================================================
  FUNCTION QMI_UIM_TLV_DEPERSONALIZATION()

  DESCRIPTION
    Parses the TLV item of deactivate perso

  PARAMETERS
    deperso_data_ptr      : input deprso data
    len                   : length of the TLV item
    depersonalization_ptr : output with deactivate perso value

  RETURN VALUE
    QMI_ERR_NONE_V01 on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_uim_tlv_depersonalization
(
  uim_depersonalization_type_v01     * deperso_data_ptr,
  uint16                               len,
  qmi_uim_tlv_depersonalization_type * depersonalization_ptr
);

/*===========================================================================
  FUNCTION QMI_UIM_TLV_PERSONALIZATION_CODE()

  DESCRIPTION
    Parses the lock code TLV item of activate perso

  PARAMETERS
    in_code_data_ptr      : input code data
    num_codes             : number of input code
    category              : simlock category
    personalization_ptr   : output with activate perso value

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_uim_tlv_personalization_code
(
  void                                          *in_code_data_ptr,
  uint16                                         num_codes,
  simlock_category_enum_type                     category,
  qmi_uim_tlv_personalization_code_type         *personalization_code_ptr
);

/*===========================================================================
  FUNCTION QMI_UIM_TLV_DECRYPTED_PERSO_SECURE()

  DESCRIPTION
    Parses the TLV item for encrypted perso request

  PARAMETERS
    data_ptr_in               : input data
    len                       : length of input data
    decoded_perso_ptr         : output decoded perso secure data

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_uim_tlv_decrypted_perso_secure
(
  uint8                                   *  data_ptr_in,
  uint16                                     len,
  qmi_uim_tlv_decrypted_perso_secure_type *  decoded_perso_ptr
);

/*===========================================================================
  FUNCTION QMI_UIM_TLV_DECRYPTED_DEPERSO_SECURE()

  DESCRIPTION
    Parses the TLV item for encrypted deperso request

  PARAMETERS
    data_ptr_in               : input data
    len                       : length of the TLV item
    decoded_perso_ptr         : output decoded perso secure data

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_uim_tlv_decrypted_deperso_secure
(
  uint8                                     *  data_ptr_in,
  uint16                                       len,
  qmi_uim_tlv_decrypted_deperso_secure_type *  decoded_deperso_ptr
);

/*===========================================================================
  FUNCTION QMI_UIM_TLV_DECRYPTED_EMERGENCY_ONLY()

  DESCRIPTION
    Parses the TLV item for encrypted emergency only request

  PARAMETERS
    data_ptr_in               : input data
    len                       : length of the TLV item
    decoded_perso_ptr         : output decoded perso secure data

  RETURN VALUE
    QMI_ERR_NONE_V01 on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_uim_tlv_decrypted_emergency_only
(
  uint8                                     *  data_ptr_in,
  uint32                                       len,
  qmi_uim_tlv_decrypted_emergency_only_type *  decoded_emergency_only_ptr
);

/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_GET_CONFIGURATION_SIMLOCK()

  DESCRIPTION
    Composes the TLV content of personalization in the
    configuration

  PARAMETERS
    perso_len_ptr         : output perso status ptr len
    perso_valid_ptr       : output perso status ptr valid or not
    perso_status_ptr      : output perso status ptr
    get_status_ptr        : feature status information

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_get_configuration_simlock
(
  uint32                                   *perso_len_ptr,
  uint8                                    *perso_valid_ptr,
  uim_personalization_status_type_v01      *perso_status_ptr,
  const simlock_get_status_msg_resp_type   *get_status_ptr
);

/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_TEMPORARY_UNLOCK_TIME_LEFT()

  DESCRIPTION
    Composes the TLV content of temporary depersonalization time left

  PARAMETERS
    tlv_id              : id to be used for the TLV
    response_pptr       : output sdu
    num_slots           : number of slots supported
    get_status_ptr      : feature status information

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_temporary_unlock_time_left
(
  uint32                                          *tmp_unlock_len_ptr,
  uint8                                           *tmp_unlock_valid_ptr,
  uim_temporary_depersonalization_status_type_v01 *tmp_unlock_ptr,
  uint8                                            num_slots,
  const simlock_get_status_msg_resp_type          *get_status_ptr
);

/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_PERSO_REACTIVATION_STATUS()

  DESCRIPTION
    Composes the TLV content of perso reactivation status

  PARAMETERS
    react_status_len_ptr         : output reactivate status len
    react_status_valid_ptr       : output reactivate status valid
    react_status_ptr             : output reactivate status 
    num_slots                    : number of slots supported
    get_status_ptr               : feature status information

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_perso_reactivation_status
(
  uint32                                   *react_status_len_ptr,
  uint8                                    *react_status_valid_ptr,
  uim_perso_reactivation_status_type_v01   *react_status_ptr,
  uint8                                     num_slots,
  const simlock_get_status_msg_resp_type  * get_status_ptr
);

/*===========================================================================
  FUNCTION  QMI_UIM_RESPONSE_TEMPORARY_UNLOCK_STATUS()

  DESCRIPTION
    Prepares the temporary unlock indication based on the
    response of get status response

  PARAMETERS
    ind_data_ptr                 : Pointer to Indication pointer
    temporary_unlock_status_ptr  : Pointer to the SIMLOCK get status information
    num_slots                    : Number of slots supported

  RETURN VALUE
    TRUE        : If the indication can be send to client
    FALSE       : If otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_temporary_unlock_status
(
  uim_temporary_unlock_ind_msg_v01        *ind_data_ptr,
  const simlock_get_status_msg_resp_type  *temporary_unlock_status_ptr,
  uint8                                    num_slots
);

/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_PERSO_CONTROL_KEY_PRESENCE()

  DESCRIPTION
    Composes the TLV content of perso control key presence

  PARAMETERS
    key_status_len_ptr  : output key status len
    key_status_valid_ptr: output key status valid
    key_status_ptr      : output key status 
    num_slots           : number of slots supported
    get_status_ptr      : feature status information

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_perso_control_key_presence
(
  uint32                                  * key_status_len_ptr,
  uint8                                   * key_status_valid_ptr,
  uim_perso_control_key_present_type_v01  * key_status_ptr,
  uint8                                     num_slots,
  const simlock_get_status_msg_resp_type  * get_status_ptr
);

/*===========================================================================
  FUNCTION  QMI_UIM_SIMLOCK_REMOTE_SFS_ENCRYPTED_IND()

  DESCRIPTION
    Encrypts remote SFS operation request using Trust Zone application APIs

  PARAMETERS
    cmd_id        : request type
    operation     : Operation requested (Read/Write)
    request_id    : Request ID for this request
    file          : The file to perform operation on
    write_data    : The data to be written (valid for write operation)
    resposne_pptr : Pointer to command response pointer

  RETURN VALUE
    TRUE        : If the encrypted response is build successfully
    FALSE       : If otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_simlock_remote_sfs_encrypted_ind
(
  uint16                                  cmd_id,
  simlock_file_operation_enum_type        operation,
  simlock_token_id_type                   request_id,
  simlock_file_type                       file,
  simlock_data_type                       write_data,
  uim_simlock_configuration_ind_msg_v01 * ind_ptr
);

/*===========================================================================
  FUNCTION QMI_UIM_CONVERT_CATEGORY_TO_SIMLOCK_CATEGORY()

  DESCRIPTION
    Converts the incoming category to SimLock category.

  PARAMETERS
    convert_category       : category value to be converted
    simlock_category_ptr   : output parameter containing SimLock category

  RETURN VALUE
    QMI client-specific error message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
 qmi_error_type_v01 qmi_uim_convert_category_to_simlock_category
(
  uint8                         convert_category,
  simlock_category_enum_type  * simlock_category_ptr
);

/*===========================================================================
  FUNCTION QMI_UIM_TLV_DECRYPTED_SIMLOCK_CONFIG()

  DESCRIPTION
    Parses the TLV item for encrypted simlock config request

  PARAMETERS
    in_encry_data_ptr          : input encrypted datat
    len                        : length of input encrypted datat
    decoded_simlock_config_ptr : output decoded simlock config data

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_uim_tlv_decrypted_simlock_config
(
  uint8                                     *  in_encry_data_ptr,
  uint16                                       len,
  qmi_uim_tlv_decrypted_simlock_config_type *  decoded_simlock_config_ptr
);

#endif /* FEATURE_SIMLOCK */
