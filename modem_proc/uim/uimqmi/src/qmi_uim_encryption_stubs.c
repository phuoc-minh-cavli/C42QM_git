/*================================================================================

DESCRIPTION
  This file contains stub functions for encryption-decryption of card PIN and
  HMAC generation.

Copyright (c) 2016 - 2018 QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary

$Header: //components/rel/uim.mpss/3.3.1/uimqmi/src/qmi_uim_encryption_stubs.c#1 $$ $DateTime: 2020/07/22 10:53:27 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
11/19/18   tq      Fix compilation issue
10/15/18   tq      Migration of QMI UIM service to QCCI-QCSI
11/01/17   ku      Adding missinng functions in stubs file
05/11/16   bcho    Initial version
=============================================================================*/

#include "comdef.h"
#include "qmi_uim_encryption.h"


/*===========================================================================
  FUNCTION  QMI_UIM_ENCRYPT_PAYLOAD()

  DESCRIPTION
    Creates an encrypted message from the input plain text message. When
    this function returns successfully, the caller is responsbile for freeing
    the returned encrypted pointer.

  PARAMETERS
    input_data            : Pointer to input plain text message
    encrypted_data_ptr    : Pointer to output encrypted message

  RETURN VALUE
    boolean          : TRUE if successful in creating an encrypted message,
                       FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_encrypt_payload
(
  mmgsdi_data_type            input_data,
  mmgsdi_data_type          * encrypted_data_ptr
)
{
  (void)input_data;
  (void)encrypted_data_ptr;
  return FALSE;
} /* qmi_uim_encrypt_payload */


/*===========================================================================
  FUNCTION  QMI_UIM_DECRYPT_PAYLOAD()

  DESCRIPTION
    Creates an decrypted message from the input message buffer. Caller is
    responsbile for passing output buffer large enough to hold the decrypted
    buffer. Recommended output buffer size is atleast in_data_len.

  PARAMETERS
    in_data_ptr            : Pointer to input encrypted message
    in_data_len            : Length of input encrypted message
    out_data_ptr           : Pointer to output plaintext message
    out_data_len_ptr       : Pointer to length of output plaintext message

  RETURN VALUE
    boolean          : TRUE if successful in decrypting the message,
                       FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_decrypt_payload
(
  const uint8               * in_data_ptr,
  uint32                      in_data_len,
  uint8                     * out_data_ptr,
  uint32                    * out_data_len_ptr
)
{
  (void)in_data_ptr;
  (void)in_data_len;
  (void)out_data_ptr;
  (void)out_data_len_ptr;
  return FALSE;
}/* qmi_uim_decrypt_payload */


/*===========================================================================
  FUNCTION QMI_UIM_DECRYPT_PIN()

  DESCRIPTION
    Decrypts the input PIN buffer. Result of the security API is also unpacked
    appropriately. First byte of the raw decrypted data is length of the
    original un-encrypted PIN, next is PIN data, rest of them are padded bytes.

  PARAMETERS
    in_pin_length       : input pin length of the encrypted PIN
    in_pin_value_ptr    : pointer to input encrypted pin buffer
    out_pin_length_ptr  : pointer to length of decrypted pin buffer
    out_pin_value_ptr   : pointer to store decrypted pin data

  RETURN VALUE
    boolean to indicate success or failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_decrypt_pin
(
  uint8                   in_pin_length,
  const uint8           * in_pin_value_ptr,
  int32                 * out_pin_length_ptr,
  uint8                 * out_pin_value_ptr
)
{
  (void)in_pin_length;
  (void)in_pin_value_ptr;
  (void)out_pin_length_ptr;
  (void)out_pin_value_ptr;
  return FALSE;
}/* qmi_uim_decrypt_pin */


/*===========================================================================
  FUNCTION QMI_UIM_ENCRYPT_PIN()

  DESCRIPTION
    Encrypts the input PIN buffer after packing it appropriately. Construction
    of the final encrypted TLV consists of the following parts & steps:
    Final TLV structure:[16-byte IV][16-byte encrypted PIN][32-byte HMAC]
    1. The plaintext PIN needs to be packed & padded into a 16-byte
    block of buffer (limitation in security API to pass a min input buffer
    of 16 bytes). First byte is length of the original un-encrypted PIN,
    next is PIN data, rest of the bytes padded with trailing zeroes.
    2. Fetch the IV (initialiation vector) & copy to TLV (16 bytes)
    3. Encrypt the plaintext PIN data & append to the TLV (16 bytes)
    4. Create the HMAC & append to the TLV (32 bytes)
    Note that the encrypted TLV can only have a max length of 255 bytes.

  PARAMETERS
    in_pin_length        : input pin length
    in_pin_value_ptr     : pointer to input pin buffer
    out_pin_length_ptr   : pointer to length of encrypted pin buffer
    out_pin_value_ptr    : pointer to encrypted pin buffer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_encrypt_pin
(
  uint8                   in_pin_length,
  const uint8           * in_pin_value_ptr,
  uint8                 * out_pin_length_ptr,
  uint8                 * out_pin_value_ptr
)
{
  (void)in_pin_length;
  (void)in_pin_value_ptr;
  (void)out_pin_length_ptr;
  (void)out_pin_value_ptr;
}/* qmi_uim_encrypt_pin */


/*===========================================================================
  FUNCTION  QMI_UIM_COMPLETE_ENCRYPT_READ_RSP()

  DESCRIPTION
    Encrypts complete read resp using Trust Zone application APIs

  PARAMETERS
    cmd_id        : Command to be saved in request header
    session_type  : Session type
    file_access   : File access
    read_data           : The data to be encrypted
    encryption_data_ptr : Pointer to encrypted data
    encryption_used     : Is encryption used

  RETURN VALUE
    errval        : QMI client-specific error message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_uim_complete_encrypt_read_rsp
(
  uint16                            cmd_id,
  mmgsdi_session_type_enum_type     session_type,
  mmgsdi_access_type                file_access,
  mmgsdi_data_type                  read_data,
  mmgsdi_data_type                * encryption_data_ptr,
  boolean                         * encryption_used_ptr                       
)
{
  (void)cmd_id;
  (void)session_type;
  (void)file_access;
  (void)read_data;
  (void)encryption_data_ptr;
  (void)encryption_used_ptr;
  return QMI_ERR_NOT_SUPPORTED_V01;
}/* qmi_uim_complete_encrypt_read_rsp */


/*===========================================================================
  FUNCTION  QMI_UIM_REMOTE_ENCRYPT_PAYLOAD()

  DESCRIPTION
    Creates an encrypted message from the input plain text message. When
    this function returns successfully, the caller is responsbile for freeing
    the returned encrypted pointer.

  PARAMETERS
    input_data            : Pointer to input plain text message
    encrypted_data_ptr    : Pointer to output encrypted message

  RETURN VALUE
    boolean          : TRUE if successful in creating an encrypted message,
                       FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_remote_encrypt_payload
(
  mmgsdi_data_type            input_data,
  mmgsdi_data_type          * encrypted_data_ptr
)
{
  (void)input_data;
  (void)encrypted_data_ptr;
  return FALSE;
} /* qmi_uim_remote_encrypt_payload */


/*===========================================================================
  FUNCTION  QMI_UIM_REMOTE_DECRYPT_PAYLOAD()

  DESCRIPTION
    Creates a decrypted message from the input message buffer. Caller is
    responsbile for passing output buffer large enough to hold the decrypted
    buffer. Recommended output buffer size is atleast in_data_len.

  PARAMETERS
    in_data_ptr            : Pointer to input encrypted message
    in_data_len            : Length of input encrypted message
    out_data_ptr           : Pointer to output plaintext message
    out_data_len_ptr       : Pointer to length of output plaintext message

  RETURN VALUE
    boolean          : TRUE if successful in decrypting the message,
                       FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_remote_decrypt_payload
(
  const uint8               * in_data_ptr,
  uint32                      in_data_len,
  uint8                     * out_data_ptr,
  uint32                    * out_data_len_ptr
)
{
  (void)in_data_ptr;
  (void)in_data_len;
  (void)out_data_ptr;
  (void)out_data_len_ptr;
  return FALSE;
} /* qmi_uim_remote_decrypt_payload */
