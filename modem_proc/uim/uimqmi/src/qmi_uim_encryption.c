/*================================================================================

DESCRIPTION
  This file contains functions for encryption-decryption of card PIN and HMAC
  generation.

Copyright (c) 2015 - 2018 QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary

$Header: //components/rel/uim.mpss/3.3.1/uimqmi/src/qmi_uim_encryption.c#1 $$ $DateTime: 2020/07/22 10:53:27 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------- 
10/15/18   tq      Migration of QMI UIM service to QCCI-QCSI
11/21/17   nc      QMI UIM TC Fixes
09/27/17   nc      UIM RMT Secure Channel 
05/25/16   vdc     Remove F3 messages for memory allocation failure
05/16/16   bcho    Removal of encrypt-decrypt pin functionality for IOE devices
05/25/15   ar      Remove feature FEATURE_SEC_HW_CRYPTO
08/20/15   bcho    Initial version
=============================================================================*/

#include "comdef.h"
#include "uim_msg.h"
#include "qmi_uim_encryption.h"
#include "qmi_svc_utils.h"
#include "qmi_uim_cat_common.h"
#include "qmi_uim_parsing.h"
#include "secapi_util.h"
#include "amssassert.h"
#include <stringl/stringl.h>

/* These includes need to always be at bottom - offtarget use only */
#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

#define QMI_UIM_PLAINTEXT_PIN_SIZE                (15)
#define QMI_UIM_INIT_VECTOR_SIZE                  (16)
#define QMI_UIM_KDF_AES256_KEY_SIZE               (32)
#define QMI_UIM_HMAC_SHA256_SIZE                  (32)
#define QMI_UIM_EXTRA_ENCRYPTION_BYTES            (100)

/*---------------------------------------------------------------------------
  Size of remaining encryped read request header RFUs
---------------------------------------------------------------------------*/
#define QMI_UIM_ENCRYPTED_READ_RSP_HEADER_RFU     (6)


/*===========================================================================
  FUNCTION  QMI_UIM_ENCRYPT_PAYLOAD_GENERIC()

  DESCRIPTION
    Creates an encrypted message from the input plain text message. When
    this function returns successfully, the caller is responsbile for freeing
    the returned encrypted pointer.

  PARAMETERS
    input_data            : Pointer to input plain text message
    encrypted_data_ptr    : Pointer to output encrypted message
    secapi_sc_cid_e_type  : Client ID

  RETURN VALUE
    boolean          : TRUE if successful in creating an encrypted message,
                       FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_uim_encrypt_payload_generic
(
  mmgsdi_data_type            input_data,
  mmgsdi_data_type          * encrypted_data_ptr,
  secapi_sc_cid_e_type        secure_channel_cid
)
{
  secerrno_enum_type  sec_err = E_FAILURE;

  if ((encrypted_data_ptr  == NULL) ||
      (input_data.data_len == 0)    ||
      (input_data.data_ptr == NULL) ||
      ((secure_channel_cid != SC_CID_UIM) &&
      (secure_channel_cid != SC_CID_SIM)))
  {
    return FALSE;
  }

  UIM_MSG_MED_0("qmi_uim_encrypt_payload_generic");

  /* Allocate buffer for encrypted message. The security API requires 100
     additional bytes to be allocated for encrypting messages.
     Note on the allocated memory for encrypted data: when this function
     returns successfully, the caller is responsbile for freeing it */
  encrypted_data_ptr->data_len = input_data.data_len + QMI_UIM_EXTRA_ENCRYPTION_BYTES;
  encrypted_data_ptr->data_ptr = (uint8*)uimqmi_malloc(encrypted_data_ptr->data_len);
  if (encrypted_data_ptr->data_ptr == NULL)
  {
    encrypted_data_ptr->data_len = 0;
    return FALSE;
  }

#ifndef FEATURE_QMI_UIM_SECURE_CHANNEL_TEST
  /* Proceed to encrypted the input data */
  sec_err = secapi_secure_message(
              SC_SSID_TZ,          /* TrustZone as destination end */
              secure_channel_cid,  
              input_data.data_ptr,
              input_data.data_len,
              encrypted_data_ptr->data_ptr,
              (uint32*)&encrypted_data_ptr->data_len);
  if (sec_err != E_SUCCESS)
  {
    UIM_MSG_ERR_1("Error encryption the data, sec_err: 0x%x", sec_err);
    uimqmi_free(encrypted_data_ptr->data_ptr);
    memset(encrypted_data_ptr, 0x00, sizeof(mmgsdi_data_type));
    return FALSE;
  }
#else
  #error code not present
#endif /* FEATURE_QMI_UIM_SECURE_CHANNEL_TEST */

  return TRUE;
} /* qmi_uim_encrypt_payload_generic */


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
  return qmi_uim_encrypt_payload_generic(input_data,
                                         encrypted_data_ptr,
                                         SC_CID_UIM);
} /* qmi_uim_encrypt_payload */


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
  return qmi_uim_encrypt_payload_generic(input_data,
                                         encrypted_data_ptr,
                                         SC_CID_SIM);
} /* qmi_uim_remote_encrypt_payload */


/*===========================================================================
  FUNCTION  QMI_UIM_DECRYPT_PAYLOAD_GENERIC()

  DESCRIPTION
    Creates a decrypted message from the input message buffer. Caller is
    responsbile for passing output buffer large enough to hold the decrypted
    buffer. Recommended output buffer size is atleast in_data_len.

  PARAMETERS
    in_data_ptr            : Pointer to input encrypted message
    in_data_len            : Length of input encrypted message
    out_data_ptr           : Pointer to output plaintext message
    out_data_len_ptr       : Pointer to length of output plaintext message
    secapi_sc_cid_e_type   : Client ID

  RETURN VALUE
    boolean          : TRUE if successful in decrypting the message,
                       FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_uim_decrypt_payload_generic
(
  const uint8               * in_data_ptr,
  uint32                      in_data_len,
  uint8                     * out_data_ptr,
  uint32                    * out_data_len_ptr,
  secapi_sc_cid_e_type        secure_channel_cid
)
{
  boolean              ret_val               = TRUE;
  secerrno_enum_type   sec_err               = E_FAILURE;
  uint8               *decrypt_buf_data_ptr  = NULL;
  uint32               decrypt_buf_data_len  = 0;

  if ((secure_channel_cid != SC_CID_UIM       &&
      secure_channel_cid != SC_CID_SIM)       ||
      in_data_ptr == NULL || in_data_len == 0 ||
      out_data_ptr == NULL || out_data_len_ptr == NULL) 
  {
    return FALSE;
  }

  do
  {
    decrypt_buf_data_len = in_data_len;

    decrypt_buf_data_ptr = (uint8*)uimqmi_malloc(decrypt_buf_data_len);

    if (decrypt_buf_data_ptr == NULL)
    {
      ret_val = FALSE;
      break;
    }

    sec_err = secapi_authenticate_decrypt_message(
                SC_SSID_TZ,                       /* TrustZone as source end */
                secure_channel_cid,               
                in_data_ptr,
                in_data_len,
                decrypt_buf_data_ptr,
                &decrypt_buf_data_len);
    if (sec_err != E_SUCCESS)
    {
      UIM_MSG_ERR_1("secapi_authenticate_decrypt_message failed, sec_err: 0x%x", sec_err);
      ret_val = FALSE;
      break;  
    }
  
    if (decrypt_buf_data_len > *out_data_len_ptr)
    {
      UIM_MSG_ERR_1("Invalid IMSI data length: 0x%x", decrypt_buf_data_len);
      ret_val = FALSE;
      break;  
    }
  
    *out_data_len_ptr = decrypt_buf_data_len;
  
    (void)memscpy((void*)out_data_ptr,
                  (*out_data_len_ptr),
                  (void*)decrypt_buf_data_ptr,
                  decrypt_buf_data_len);
  }while(0);
  if (decrypt_buf_data_ptr != NULL)
  {
    uimqmi_free(decrypt_buf_data_ptr);
  }
  return ret_val;
} /* qmi_uim_decrypt_payload_generic */


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
  return qmi_uim_decrypt_payload_generic(in_data_ptr,
                                         in_data_len,
                                         out_data_ptr,
                                         out_data_len_ptr,
                                         SC_CID_UIM);
} /* qmi_uim_decrypt_payload */


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
  return qmi_uim_decrypt_payload_generic(in_data_ptr,
                                         in_data_len,
                                         out_data_ptr,
                                         out_data_len_ptr,
                                         SC_CID_SIM);
} /* qmi_uim_remote_decrypt_payload */


/*===========================================================================
  FUNCTION QMI_UIM_GENERATE_HMAC()

  DESCRIPTION
    Generated the HMAC out of the passed input buffer. In the PIN encryption
    case, input buffer consists of IV (16 bytes) + encrypted PIN (16 bytes)
    
  PARAMETERS
    in_data_len            : input data length whose HMAC has to be found
    in_data_ptr            : input data pointer whose HMAC has to be found
    out_hmac_data_len_ptr  : pointer to length of HMAC data
    out_hmac_data_ptr      : pointer to HMAC data

  RETURN VALUE
    boolean to indicate success or failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_uim_generate_hmac
(
  uint8             in_data_len,
  const uint8     * in_data_ptr,
  uint16          * out_hmac_data_len_ptr,
  uint8           * out_hmac_data_ptr
)
{
#ifndef FEATURE_UIM_TEST_FRAMEWORK
  IxErrnoType                 retval        = E_FAILURE;
  secerrno_enum_type          sec_err       = E_SUCCESS;
  secapi_handle_type          hash_handle   = NULL;
  secapi_hsh_param_data_type  hash_param;
  uint8                       derived_key[QMI_UIM_KDF_AES256_KEY_SIZE] = {0};  
  static char                 label[]   = "QMI_UIM";
  static char                 context[] = "Derived HW key encryption for HMAC";

  ASSERT(in_data_len > 0);
  ASSERT(in_data_ptr);
  ASSERT(out_hmac_data_ptr);
  ASSERT(out_hmac_data_len_ptr);

  if (*out_hmac_data_len_ptr < QMI_UIM_HMAC_SHA256_SIZE)
  {
    UIM_MSG_ERR_1("Insufficient out_hmac_data_length: 0x%x", *out_hmac_data_len_ptr);
    return FALSE;
  }

  /* Get the derived key K (NULL input key indicates use HW key) */
  retval = secapi_util_kdf(NULL,
                           QMI_UIM_KDF_AES256_KEY_SIZE,
                           label,
                           strlen(label),
                           context,
                           strlen(context),
                           derived_key,
                           sizeof(derived_key));
  if (retval != E_SUCCESS)
  {
    UIM_MSG_ERR_1("secapi_util_kdf failed, retval:0x%x", retval);
    return FALSE;	
  }

  /* Get the Secapi handle */
  sec_err = secapi_new(&hash_handle, SECAPI_SHA);
  if (sec_err != E_SUCCESS)
  {
    UIM_MSG_ERR_1("secapi_new, sec_err: 0x%x", sec_err);
    return FALSE;
  }

  /* Set mode */
  hash_param.mode = SECAPI_HSH_INPUT_MODE_ATOMIC;
  sec_err = secapi_hsh_set_param(hash_handle,
                                 SECAPI_SHA,
                                 SECAPI_HSH_INPUT_MODE,
                                 &hash_param);
  if (sec_err != E_SUCCESS)
  {
    UIM_MSG_LOW_1("Set Param error, sec_err: 0x%x", sec_err);
    secapi_delete(&hash_handle);
    return FALSE;
  }

  /* Set preferred execution platform type */
  hash_param.common.exec_platform = SECAPI_EXEC_PLATFORM_ARM;
  sec_err = secapi_hsh_set_param(hash_handle,
                                 SECAPI_SHA,
                                 SECAPI_EXEC_PLATFORM,
                                 &hash_param );
  if (sec_err != E_SUCCESS)
  {
    UIM_MSG_LOW_1( "Set param platform error: 0x%x", sec_err);
    secapi_delete(&hash_handle);
    return FALSE;	
  }

  sec_err = secapi_hsh_create_hmac(hash_handle,
                                   SECAPI_SHA,
                                   (uint8 *)in_data_ptr,
                                   in_data_len,
                                   derived_key,
                                   sizeof(derived_key),
                                   out_hmac_data_ptr);
  if (sec_err != E_SUCCESS)
  {
    UIM_MSG_ERR_1( "Create HMAC error error: 0x%x", sec_err);
    secapi_delete(&hash_handle);
    return FALSE;	
  }

  *out_hmac_data_len_ptr = QMI_UIM_HMAC_SHA256_SIZE;

  secapi_delete(&hash_handle);
  return TRUE;
#else
  #error code not present
#endif /* !FEATURE_UIM_TEST_FRAMEWORK */
} /* qmi_uim_generate_hmac */


/*===========================================================================
  FUNCTION QMI_UIM_DECRYPT_DATA()

  DESCRIPTION
    Decrypts the passed input buffer with the help of the derived HW key.
    
  PARAMETERS
    in_data_len          : length of the encrypted data
    in_data_ptr          : pointer to the encrypted data
    out_pin_length_ptr   : pointer to the length of plaintext data
    out_pin_value_ptr    : pointer to the plaintext data
    iv_ptr               : pointer to the initialization vector buffer

  RETURN VALUE
    boolean to indicate success or failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_uim_decrypt_data
(
  uint8               in_data_len,
  const uint8       * in_data_ptr,
  uint16            * out_data_len_ptr,
  uint8             * out_data_ptr,
  const uint8       * iv_ptr
)
{
#ifndef FEATURE_UIM_TEST_FRAMEWORK
  IxErrnoType                 retval          = E_FAILURE;
  secapi_handle_type          cipher_handle   = NULL;
  uint16                      temp_len_uint16 = 0;
  secerrno_enum_type          sec_err         = E_SUCCESS;
  secapi_enc_param_data_type  cipher_param;
  uint8                       derived_key[QMI_UIM_KDF_AES256_KEY_SIZE] = {0};
  static char                 label[]   = "QMI_UIM";
  static char                 context[] = "Derived HW key encryption for QMI_UIM";

  ASSERT(in_data_len > 0);
  ASSERT(in_data_ptr);
  ASSERT(out_data_ptr);
  ASSERT(out_data_len_ptr);
  ASSERT(iv_ptr);

  temp_len_uint16 = *out_data_len_ptr;

  /* Get the derived key K (NULL input key indicates use HW key) */
  retval = secapi_util_kdf(NULL,
                           QMI_UIM_KDF_AES256_KEY_SIZE,
                           label,
                           strlen(label),
                           context,
                           strlen(context),
                           derived_key,
                           sizeof(derived_key));
  if (retval != E_SUCCESS)
  {
    UIM_MSG_ERR_1("secapi_util_kdf failed, retval:0x%x", retval);
    return FALSE;
  }

  /* Get the Secapi handle */
  sec_err = secapi_new( &cipher_handle, SECAPI_AES256);
  if (sec_err != E_SUCCESS)
  {
    UIM_MSG_ERR_1("secapi_new, sec_err: 0x%x", sec_err);
    return FALSE;
  }

  /* Set platform type */
  memset(&cipher_param, 0, sizeof(secapi_enc_param_data_type));
  cipher_param.common.exec_platform = SECAPI_EXEC_PLATFORM_ARM;
  sec_err = secapi_enc_set_param(cipher_handle,
                                 SECAPI_AES256,
                                 SECAPI_EXEC_PLATFORM,
                                 &cipher_param );
  if (sec_err != E_SUCCESS)
  {
    UIM_MSG_LOW_1( "Set param platform error: 0x%x", sec_err);
    secapi_delete(&cipher_handle);
    return FALSE;	
  }

  /* Initialize data with previously obtained K & IV.
     Note that IV needs to come from the incoming encrypted TLV
     (first 16 bytes of the encrypted TLV is reserved for IV) */
  memset(&cipher_param, 0, sizeof(secapi_enc_param_data_type));
  cipher_param.init_data.enc.key_ptr = derived_key;
  cipher_param.init_data.dec.key_ptr = derived_key;
  cipher_param.init_data.enc.iv_ptr  = (uint8 *)iv_ptr;
  cipher_param.init_data.dec.iv_ptr  = (uint8 *)iv_ptr;
  sec_err = secapi_enc_set_param(cipher_handle,
                                 SECAPI_AES256,
                                 SECAPI_ENC_INIT_DATA,
                                 &cipher_param );
  if (sec_err != E_SUCCESS)
  {
    UIM_MSG_LOW_1("Set param error for init_data: 0x%x", sec_err);
    secapi_delete(&cipher_handle);
    return FALSE;	
  }

  /* Set operation mode */
  memset(&cipher_param, 0, sizeof(secapi_enc_param_data_type));
  cipher_param.op_mode =  SECAPI_ENC_OP_MODE_CBC;
  sec_err = secapi_enc_set_param(cipher_handle,
                                 SECAPI_AES256,
                                 SECAPI_ENC_OP_MODE,
                                 &cipher_param );
  if ( sec_err != E_SUCCESS )
  {
    UIM_MSG_LOW_1("Set param error for op_mode: 0x%x", sec_err);
    secapi_delete(&cipher_handle);
    return FALSE;	
  }

  /* Set padding type */
  memset(&cipher_param, 0, sizeof(secapi_enc_param_data_type));
  cipher_param.padding.type = SECAPI_ENC_NO_PADDING;
  sec_err = secapi_enc_set_param(cipher_handle,
                                 SECAPI_AES256,
                                 SECAPI_ENC_PADDING_TYPE,
                                 &cipher_param );
  if (sec_err != E_SUCCESS)
  {
    UIM_MSG_LOW_1("Set param error for padding: 0x%x", sec_err);
    secapi_delete(&cipher_handle);
    return FALSE;
  }

  sec_err = secapi_enc_decrypt(cipher_handle,
                               SECAPI_AES256,
                               (uint8 *)in_data_ptr,
                               in_data_len,
                               out_data_ptr,
                               &temp_len_uint16);

  if (sec_err != E_SUCCESS)
  {
    UIM_MSG_ERR_1("Error in decrypting PIN: 0x%x", sec_err);
    secapi_delete(&cipher_handle);
    return FALSE;
  }

  secapi_delete(&cipher_handle);
  return TRUE;
#else
  #error code not present
#endif /* !FEATURE_UIM_TEST_FRAMEWORK */
} /* qmi_uim_decrypt_data */


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
  int32                * out_pin_length_ptr,
  uint8                 * out_pin_value_ptr
)
{
  uint8                    *  temp_ptr       = NULL;
  uint16                      temp_len       = 0;
  uint32                      remaining_len  = 0;
  uint8                       hmac_offset    = 0;
  uint8                       init_vector[QMI_UIM_INIT_VECTOR_SIZE];
  uint8                       hmac[QMI_UIM_HMAC_SHA256_SIZE];

  PACKED struct PACKED_POST
  {
    uint8  pin_len;
    uint8  pin_data[QMI_UIM_PLAINTEXT_PIN_SIZE];
  } output_pin_info;

  ASSERT(in_pin_length > 0);
  ASSERT(in_pin_value_ptr);
  ASSERT(out_pin_value_ptr);
  ASSERT(out_pin_length_ptr);

  /* Check incoming pin buffer length to contain the min length */
  if (in_pin_length <= ((sizeof(init_vector)) + QMI_UIM_HMAC_SHA256_SIZE))
  {
    UIM_MSG_ERR_1("QMI_UIM PIN len invalid, in_pin_length: 0x%x", in_pin_length);
    return FALSE;
  }

  temp_ptr      = (uint8 *)in_pin_value_ptr;
  remaining_len = in_pin_length;
  hmac_offset   = remaining_len - QMI_UIM_HMAC_SHA256_SIZE;

  memset(&output_pin_info, 0, sizeof(output_pin_info));
  memset(&init_vector, 0, sizeof(init_vector));
  memset(&hmac, 0, sizeof(hmac));

  /* First extract the HMAC from the incoming IV + encrypted data */
  temp_len = QMI_UIM_HMAC_SHA256_SIZE;
  if (!qmi_uim_generate_hmac(remaining_len - QMI_UIM_HMAC_SHA256_SIZE,
                             temp_ptr,
                             &temp_len,
                             hmac))
  {
    return FALSE;
  }

  /* Then compare the incoming HMAC against the calculated one.
     Proceed only if they match */
  if (memcmp(hmac, temp_ptr + hmac_offset, QMI_UIM_HMAC_SHA256_SIZE) != 0)
  {
    UIM_MSG_ERR_0("HMAC is not the same");
    return FALSE;
  }

  /* Extract the IV (first 16 bytes) & pass it for decryption */
  (void)memscpy((void*)init_vector,
                sizeof(init_vector),
                (void*)temp_ptr,
                (uint32)in_pin_length);

  /* Update pointer & length. Note that the remaining len has been updated
     to remove the HMAC length that is present at the end of the data */
  temp_ptr      += sizeof(init_vector);
  remaining_len -= (sizeof(init_vector) + QMI_UIM_HMAC_SHA256_SIZE);
  temp_len       = sizeof(output_pin_info);

  /* Decrypt PIN data */
  if (!qmi_uim_decrypt_data(remaining_len,
                            temp_ptr,
                            &temp_len,
                            (uint8 *)&output_pin_info,
                            init_vector))
  {
    return FALSE;
  }

  /* Sanity checks */
  if (output_pin_info.pin_len == 0 ||
      output_pin_info.pin_len > QMI_UIM_TLV_MAX_PIN_LEN ||
      output_pin_info.pin_len > *out_pin_length_ptr)
  {
    UIM_MSG_ERR_1("QMI_UIM invalid PIN len: 0x%x", output_pin_info.pin_len);
    return FALSE;
  }

  /* Unpack the decrypted PIN1 value */
  (void)memscpy((void*)out_pin_value_ptr,
                *out_pin_length_ptr,
                (void*)output_pin_info.pin_data,
                output_pin_info.pin_len);

  /* Update the decrypted PIN1 length */
  *out_pin_length_ptr = output_pin_info.pin_len;

  return TRUE;
} /* qmi_uim_decrypt_pin */


/*===========================================================================
  FUNCTION QMI_UIM_ENCRYPT_DATA()

  DESCRIPTION
    Encrypts the passed input buffer with the help of the derived HW key.
    
  PARAMETERS
    in_data_len          : length of the data that needs to be encrypted
    in_data_ptr          : pointer to the data that needs to be encrypted
    out_data_len_ptr     : pointer to the length of encrypted data
    out_data_ptr         : pointer to the encrypted data
    iv_ptr               : pointer to the initialization vector buffer

  RETURN VALUE
    boolean to indicate success or failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_uim_encrypt_data
(
  uint8                   in_data_len,
  const uint8           * in_data_ptr,
  uint8                 * out_data_len_ptr,
  uint8                 * out_data_ptr,
  const uint8           * iv_ptr
)
{
#ifndef FEATURE_UIM_TEST_FRAMEWORK
  secerrno_enum_type          sec_err        = E_SUCCESS;
  IxErrnoType                 retval         = E_FAILURE;
  secapi_handle_type          cipher_handle  = NULL;
  secapi_enc_param_data_type  cipher_param;
  uint8                       derived_key[QMI_UIM_KDF_AES256_KEY_SIZE] = {0};
  static char                 label[]   = "QMI_UIM";
  static char                 context[] = "Derived HW key encryption for QMI_UIM";

  ASSERT(in_data_len > 0);
  ASSERT(in_data_ptr);
  ASSERT(out_data_ptr);
  ASSERT(out_data_len_ptr);
  ASSERT(iv_ptr);

  /* Get the derived key K (NULL input key indicates use HW key) */
  retval = secapi_util_kdf(NULL,
                           QMI_UIM_KDF_AES256_KEY_SIZE,
                           label,
                           strlen(label),
                           context,
                           strlen(context),
                           derived_key,
                           QMI_UIM_KDF_AES256_KEY_SIZE);
  if (retval != E_SUCCESS)
  {
    UIM_MSG_ERR_1("secapi_util_kdf failed, retval:0x%x", retval);
    return FALSE;	
  }

  /* Get the Secapi handle */
  sec_err = secapi_new( &cipher_handle, SECAPI_AES256);
  if (sec_err != E_SUCCESS)
  {
    UIM_MSG_ERR_1("secapi_new, sec_err: 0x%x", sec_err);
    return FALSE;
  }

  /* Set platform type */
  memset(&cipher_param, 0, sizeof(secapi_enc_param_data_type));
  cipher_param.common.exec_platform = SECAPI_EXEC_PLATFORM_ARM;
  sec_err = secapi_enc_set_param(cipher_handle,
                                 SECAPI_AES256,
                                 SECAPI_EXEC_PLATFORM,
                                 &cipher_param );
  if (sec_err != E_SUCCESS)
  {
    UIM_MSG_LOW_1( "Set param platform error: 0x%x", sec_err);
    secapi_delete(&cipher_handle);
    return FALSE;	
  }

  /* Initialize data with previously obtained K & IV */
  memset(&cipher_param, 0, sizeof(secapi_enc_param_data_type));
  cipher_param.init_data.enc.key_ptr = derived_key;
  cipher_param.init_data.dec.key_ptr = derived_key;
  cipher_param.init_data.enc.iv_ptr  = (uint8 *)iv_ptr;
  cipher_param.init_data.dec.iv_ptr  = (uint8 *)iv_ptr;
  sec_err = secapi_enc_set_param(cipher_handle,
                                 SECAPI_AES256,
                                 SECAPI_ENC_INIT_DATA,
                                 &cipher_param );
  if (sec_err != E_SUCCESS)
  {
    UIM_MSG_ERR_1("Set param error for init_data: 0x%x", sec_err);
    secapi_delete(&cipher_handle);
    return FALSE;	
  }

  /* Set operation mode */
  memset(&cipher_param, 0, sizeof(secapi_enc_param_data_type));
  cipher_param.op_mode =  SECAPI_ENC_OP_MODE_CBC;
  sec_err = secapi_enc_set_param(cipher_handle,
                                 SECAPI_AES256,
                                 SECAPI_ENC_OP_MODE,
                                 &cipher_param );
  if ( sec_err != E_SUCCESS )
  {
    UIM_MSG_LOW_1("Set param error for op_mode: 0x%x", sec_err);
    secapi_delete(&cipher_handle);
    return FALSE;	
  }

  /* Set padding type */
  memset(&cipher_param, 0, sizeof(secapi_enc_param_data_type));
  cipher_param.padding.type = SECAPI_ENC_NO_PADDING;
  sec_err = secapi_enc_set_param(cipher_handle,
                                 SECAPI_AES256,
                                 SECAPI_ENC_PADDING_TYPE,
                                 &cipher_param );

  if (sec_err != E_SUCCESS)
  {
    UIM_MSG_LOW_1("Set param error for padding: 0x%x", sec_err);
    secapi_delete(&cipher_handle);
    return FALSE;
  }

  /* Finally encrypt data */
  sec_err = secapi_enc_encrypt(cipher_handle,
                               SECAPI_AES256,
                               (uint8 *)in_data_ptr,
                               (uint16)in_data_len,
                               out_data_ptr,
                               (uint16 *)out_data_len_ptr);
  if (sec_err != E_SUCCESS)
  {
    UIM_MSG_ERR_1("Error in encrypting data, sec_err: 0x%x", sec_err);
    secapi_delete(&cipher_handle);
    return FALSE;
  }

  secapi_delete(&cipher_handle);
  return TRUE;
#else
  #error code not present
#endif /* !FEATURE_UIM_TEST_FRAMEWORK */
} /* qmi_uim_encrypt_data */


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
  secerrno_enum_type         sec_err;
  uint16                     temp_len       = 0;
  uint8                   *  temp_ptr       = NULL;
  uint8                      bytes_written  = 0;
  uint8                      init_vector[QMI_UIM_INIT_VECTOR_SIZE];

  PACKED struct PACKED_POST
  {
    uint8  pin_len;
    uint8  pin_data[QMI_UIM_PLAINTEXT_PIN_SIZE];
  } input_pin_info;

  ASSERT(in_pin_length > 0 && in_pin_value_ptr && out_pin_length_ptr && out_pin_value_ptr);

  temp_len = *out_pin_length_ptr;
  temp_ptr = out_pin_value_ptr;
  memset(&input_pin_info, 0, sizeof(input_pin_info));

  /* Check input length */
  if (in_pin_length >= (QMI_UIM_PLAINTEXT_PIN_SIZE+1))
  {
    UIM_MSG_ERR_1("QMI_UIM invalid len: 0x%x", in_pin_length);
    return;
  }


  /* Check output length */
  if (*out_pin_length_ptr < sizeof(init_vector))
  {
    UIM_MSG_ERR_1("Invalid out_pin_length: 0x%x", *out_pin_length_ptr);
    return;
  }

  /* Pack the data to be encrypted */
  input_pin_info.pin_len = in_pin_length;
  (void)memscpy((void*)input_pin_info.pin_data,
                sizeof(input_pin_info.pin_data),
               (void*)in_pin_value_ptr,
               (uint32)input_pin_info.pin_len);

  /* Fetch random data for IV */
  sec_err = secapi_get_random(SECAPI_SECURE_RANDOM,
                              init_vector,
                              sizeof(init_vector));
  if (sec_err != E_SUCCESS)
  {
    UIM_MSG_ERR_1( "secapi_get_random error, sec_err: 0x%x", sec_err);
    return;
  }

  /* Now copy IV in the first 16 bytes */
  (void)memscpy((void*)out_pin_value_ptr,
                *out_pin_length_ptr,
                (void*)init_vector,
                (uint32)sizeof(init_vector));

  bytes_written += sizeof(init_vector);
  temp_ptr      += bytes_written;
  temp_len      -= bytes_written;

  /* Encrypt PIN data */
  if (qmi_uim_encrypt_data((uint16)sizeof(input_pin_info),
                           (uint8 *)&input_pin_info,
                           (uint8 *)&temp_len,
                           temp_ptr,
                           init_vector) == FALSE)
  {
    return;
  }

  bytes_written += temp_len;
  temp_ptr      += temp_len;
  temp_len      -= bytes_written;

  /* Create HMAC from the IV + Encrypted data */
  if (!qmi_uim_generate_hmac(bytes_written,
                             out_pin_value_ptr,
                             &temp_len,
                             temp_ptr))
  {
    return;
  }

  /* Finally update the number of bytes written */
  bytes_written += temp_len;
  *out_pin_length_ptr = bytes_written;
} /* qmi_uim_encrypt_pin */


/*===========================================================================
  FUNCTION QMI_UIM_CONVERT_MMGSDI_SESSION_TYPE_TO_TLV

  DESCRIPTION
    Converts the MMGSDI session type to a TLV byte.

  PARAMETERS
    mmgsdi_session_type     : mmgsdi session type
    tlv_session_type_ptr    : return tlv session type

  RETURN VALUE
    errval          : QMI client-specific error message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_type_v01 qmi_uim_convert_mmgsdi_session_type_to_tlv
(
  mmgsdi_session_type_enum_type     mmgsdi_session_type,
  uint8                           * tlv_session_type_ptr
)
{
  ASSERT(tlv_session_type_ptr);

  switch (mmgsdi_session_type)
  {
    case MMGSDI_GW_PROV_PRI_SESSION:
      *tlv_session_type_ptr = 0;
      break;
    case MMGSDI_1X_PROV_PRI_SESSION:
      *tlv_session_type_ptr = 1;
      break;
    case MMGSDI_GW_PROV_SEC_SESSION:
      *tlv_session_type_ptr = 2;
      break;
    case MMGSDI_1X_PROV_SEC_SESSION:
      *tlv_session_type_ptr = 3;
      break;
    case MMGSDI_NON_PROV_SESSION_SLOT_1:
      *tlv_session_type_ptr = 4;
      break;
    case MMGSDI_NON_PROV_SESSION_SLOT_2:
      *tlv_session_type_ptr = 5;
      break;
    case MMGSDI_CARD_SESSION_SLOT_1:
      *tlv_session_type_ptr = 6;
      break;
    case MMGSDI_CARD_SESSION_SLOT_2:
      *tlv_session_type_ptr = 7;
      break;
    case MMGSDI_GW_PROV_TER_SESSION:
      *tlv_session_type_ptr = 10;
      break;
    case MMGSDI_1X_PROV_TER_SESSION:
      *tlv_session_type_ptr = 11;
      break;
    case MMGSDI_NON_PROV_SESSION_SLOT_3:
      *tlv_session_type_ptr = 16;
      break;
    case MMGSDI_CARD_SESSION_SLOT_3:
      *tlv_session_type_ptr = 19;
      break;
    default:
      UIM_MSG_ERR_1("QMI_UIM invalid session type: 0x%x",
                    mmgsdi_session_type);
      return QMI_ERR_INVALID_ARG_V01;
  }

  return QMI_ERR_NONE_V01;
} /* qmi_uim_convert_mmgsdi_session_type_to_tlv */


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
  mmgsdi_data_type      decrypted_data      = { 0, NULL };
  qmi_error_type_v01    errval              = QMI_ERR_NONE_V01;

  PACKED struct PACKED_POST
  {
    uint16    cmd_id;
    uint8     rfu[QMI_UIM_ENCRYPTED_READ_RSP_HEADER_RFU];
    uint8     session_type;
    uint16    file_id;
    uint8     path_len;
    uint8     path_buf[MMGSDI_MAX_PATH_LEN * 2];
    uint16    data_len;
  } fixed_data;

  if (read_data.data_len > QMI_UIM_CONTENT_TRANSPARENT_MAX_V01 ||
      read_data.data_len <= 0 ||
      read_data.data_ptr == NULL)
  {
    UIM_MSG_ERR_1("Invalid read data, data length: 0x%x", read_data.data_len);
    return QMI_ERR_INTERNAL_V01;
  }

  if (encryption_data_ptr == NULL ||
      encryption_used_ptr == NULL)
  {
    return QMI_ERR_INTERNAL_V01;
  }

  if (file_access.access_method != MMGSDI_BY_PATH_ACCESS ||
      file_access.file.path_type.path_len > MMGSDI_MAX_PATH_LEN)
  {
    UIM_MSG_ERR_2("Invalid file path length: 0x%x or access method: 0x%x",
                  file_access.file.path_type.path_len,
                  file_access.access_method);
    return QMI_ERR_INTERNAL_V01;
  }

  memset(&fixed_data, 0x00, sizeof(fixed_data));

  fixed_data.cmd_id = cmd_id;

  errval = qmi_uim_convert_mmgsdi_session_type_to_tlv(
                session_type,
                &fixed_data.session_type);
  if (errval != QMI_ERR_NONE_V01)
  {
    return errval;
  }

  fixed_data.file_id = file_access.file.path_type.path_buf[file_access.file.path_type.path_len - 1];
  fixed_data.path_len = file_access.file.path_type.path_len * 2;

  (void)memscpy((void*)fixed_data.path_buf,
                sizeof(fixed_data.path_buf),
                (void*)file_access.file.path_type.path_buf,
                fixed_data.path_len);

  fixed_data.data_len = (uint16)read_data.data_len;

  decrypted_data.data_len = sizeof(fixed_data) + read_data.data_len;
  decrypted_data.data_ptr = uimqmi_malloc(decrypted_data.data_len);
  if (decrypted_data.data_ptr == NULL)
  {
    return QMI_ERR_NO_MEMORY_V01;
  }

  /* copy the fixed data including the request header, session type,
     file access data and data length. */
  (void)memscpy((void*)decrypted_data.data_ptr,
                sizeof(fixed_data),
                (void*)&fixed_data,
                sizeof(fixed_data));

  /* copy the variable file data */
  (void)memscpy((void*)(decrypted_data.data_ptr + sizeof(fixed_data)),
                read_data.data_len,
                (void*)read_data.data_ptr,
                read_data.data_len);

  *encryption_used_ptr = qmi_uim_encrypt_payload(decrypted_data,
                                                 encryption_data_ptr);

  uimqmi_free(decrypted_data.data_ptr);
  decrypted_data.data_ptr = NULL;

  return errval;
} /* qmi_uim_complete_encrypt_read_rsp */

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
