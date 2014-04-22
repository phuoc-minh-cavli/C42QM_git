#ifndef SECCRYPT_INTERNAL_H
#define SECCRYPT_INTERNAL_H
/** 
  @file seccrypt_internal.h
*/
/*===========================================================================
Copyright (c) {2018} Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE
  $Header: //components/rel/core.mpss/3.10/securemsm/inc/seccrypt_internal.h#1 $ 
  $DateTime: 2019/03/17 23:07:12 $ 
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
13-02-18  sahaj    Initial version (FR#44835)
===========================================================================*/
#include "secerrno.h"
#include "secapi.h"

/**
  Sets the parameters for the DES encryption/decryption algorithm.

  There is no callback function invocation upon the completion of this 
  operation. 

  @param[in] handle           Handle to the crypto instance.
  @param[in] algo             Cipher algorithm specifier.
  @param[in] param            Parameter to get.
  @param[out] param_data_ptr  Pointer to the parameter-specific data.

  @return
  E_SUCCESS - Request was successful. \n
  E_FAILURE - Failed to process the request.

  @dependencies
  None.
*/

secerrno_enum_type seccrypt_enc_get_param
(
seccrypt_handle_type         handle,
secapi_algo_enum_type        algo,
secenc_param_enum_type   param,
secenc_param_data_type*  param_data_ptr
);

/**
  Creates a message digest using the algorithm specified. The message is in the
  DSM chain buffer. 

  @param[in] handle           Handle to the crypto instance.
  @param[in] handle_type      Algorithm type specifier.
  @param[in] msg_ptr          DSM chain pointer to the message to be 
                              authenticated.
  @param[in] data_ptr         Data buffer pointer to the data to be 
                              authenticated.
  @param[in] msg_byte_len     Length of the message in bytes.
  @param[out] msg_digest_ptr  Pointer to the message digest (memory 
                              provided by the caller).

  @return
  E_SUCCESS    - Request was successful. \n
  E_IN_PROGRES - QDSP is in use. \n
  E_FAILURE    - Failed to process the request.

  @dependencies
  The hash mode must be properly set by calling seccrypt_hsh_set_param.
*/
secerrno_enum_type seccrypt_hsh_create_digest
(
seccrypt_handle_type   handle,
secapi_algo_enum_type  handle_type,
dsm_item_type*         msg_ptr,
uint8*                 data_ptr,
uint16                 msg_byte_len,
dsm_item_type**        msg_digest_ptr
);


/**
  Creates a hash MAC per RFC 2104 using the specified hash algorithm. The 
  message is in the DSM chain buffer.

  @param[in] handle            Handle to the crypto instance.
  @param[in] hmac_type         HMAC types, used per invocation. Includes 
                               HMAC_SSL, TLS, RFC2104, etc.
  @param[in] hmac_algo         HMAC algorithm specifier.
  @param[in] msg_dsm_ptr       Pointer to the message in the DSM chain to 
                               be authenticated. If set, dsm_data_ptr 
                               should be NULL.
  @param[in] msg_data_ptr      Pointer to the message in the data buffer. 
                               If used, dsm_msg_ptr should be NULL.
  @param[in] msg_byte_len      Length of the message in bytes.
  @param[in] *key_dsm_ptr      Pointer to the key in the data buffer. If 
                               used, key_dsm_ptr should be NULL.
  @param[in] key_data_ptr      Pointer to the key in the DSM chain. If set,
                               key_data_ptr should be NULL.
  @param[in] key_byte_len      Length of the key bytes either in the DSM 
                               chain or the data buffer.
  @param[out] hmac_ptr         Pointer to the HMAC DSM chain output (memory
                               provided by the SEC API. The caller should 
                               call DSM_free_packet() to release the 
                               memory).
   
  @return
  E_SUCCESS    - Request was successful. \n
  E_IN_PROGRES - QDSP is in use. \n
  E_FAILURE    - Failed to process the request. 

  @dependencies
  None.
*/
secerrno_enum_type seccrypt_hsh_create_hmac
(
seccrypt_handle_type          handle,         
seccrypt_hsh_hmac_enum_type   hmac_type,  
secapi_algo_enum_type         hmac_algo,      
dsm_item_type*                msg_dsm_ptr,    
uint8*                        msg_data_ptr,                                  
uint16                        msg_byte_len,   
dsm_item_type*                key_dsm_ptr,    
uint8*                        key_data_ptr,   
uint16                        key_byte_len,             
dsm_item_type**               hmac_ptr         
);

/**
  Creates a hash MAC per RFC 2104 using the specified hash algorithm.

  The HMAC return value is the pointer of uint8.

  @param[in] handle           Handle to the crypto instance.
  @param[in] hmac_type        HMAC type specifier.
  @param[in] hmac_algo        Hashing algorithm specifier.
  @param[in] inner_msg_ptr    DSM chain pointer to the inner message to be
                              authenticated.
  @param[in] inner_data_ptr   Data buffer pointer to the inner message to 
                              be authenticated.
  @param[in] inner_byte_len   Inner byte length.
  @param[in] outer_msg_ptr    DSM chain pointer to the outer message to be
                              authenticated.
  @param[in] outer_data_ptr   Data buffer pointer to the outer message to 
                              be authenticated.
  @param[in] outer_byte_len   Outer byte length.
  @param[out] hmac_ptr        Pointer to the message digest (memory provided 
                              by the caller).

  @return
  E_SUCCESS    - Request was successful. \n
  E_IN_PROGRES - QDSP is in use. \n
  E_FAILURE    - Failed to process the request.

  @dependencies
  None.
*/
secerrno_enum_type seccrypt_hsh_create_hmac_int
(
seccrypt_handle_type          handle,
seccrypt_hsh_hmac_enum_type   hmac_type,
secapi_algo_enum_type         hmac_algo,
dsm_item_type*                inner_msg_ptr,
uint8*                        inner_data_ptr,
uint16                        inner_byte_len,
dsm_item_type*                outer_msg_ptr,
uint8*                        outer_data_ptr,
uint16                        outer_byte_len,
uint8*                        hmac_ptr
);

/**
  Sets the hashing parameters for the passed instance handle.

  There is no callback function invocation upon the completion of this
  operation.

  @param[in] handle           Handle to the crypto instance.
  @param[in] algo             Hashing algorithm specifier.
  @param[in] param            Parameter to be set.
  @param[out] param_data_ptr  Pointer to the parameter-specific data.

  @return
  E_SUCCESS - Request was successful. \n
  E_FAILURE - Failed to process the request.

  @dependencies
  None.
*/
secerrno_enum_type seccrypt_hsh_set_param
(
seccrypt_handle_type     handle,
secapi_algo_enum_type    algo,
sechsh_param_enum_type   param,
sechsh_param_data_type*  param_data_ptr
);

/**
  Verifies an RSA signature by decrypting and decoding the signature. 
   
  @note  
  - The comparison of the hash data is not done here. This should be done by 
    the calling function.
  - The signature to be verified should conform to certain restrictions. 
    Otherwise, an E_FAILURE is returned.

  @param[in] handle               Handle to the crypto instance. 
  @param[in] algo                 PKX authentication algorithm. 
  @param[in] padding_type         PKX cipher padding type. 
  @param[in] signature_ptr        Pointer to the signature data.
  @param[in] signature_len        Length of the signature.
  @param[out] msg_hash_ptr        Pointer to the hash (memory provided by 
                                  the caller).
  @param[in,out] msg_hash_len_ptr Pointer to the hash data length.

  @return
  E_SUCCESS     - Function executes successfully. \n
  E_NOT_ALLOWED - One or more Diffie-Hellman parameters have not been set. \n 
  E_IN_PROGRESS - CRYPTO-DSP interface was already busy. \n
  E_FAILURE     - Any other failures. 

  @dependencies
  None.
*/
secerrno_enum_type seccrypt_pkx_authenticate
(
seccrypt_handle_type          handle,
secapi_algo_enum_type         algo,
secapi_pkx_padding_enum_type  padding_type,
uint8*                        signature_ptr,
uint16                        signature_len,
uint8*                        msg_hash_ptr,
uint16*                       msg_hash_len_ptr
);

/**
  Creates a key using the Diffie-Hellman algorithm. 

  The key created is dependent on the passed key type: Private, Public, or 
  Secret. The key is written to the passed address. 

  This function uses previously stored values for the base, modulus, and key 
  information needed by the Diffie-Hellman algorithm. 
   
  @note 
  If none of these values have been set (via seccrypt_pkx_dh_set_param), 
  default values will be used.

  @param[in] handle        Handle to the crypto instance.
  @param[in] algo          Algorithm.
  @param[in] key           Type of key to create.
  @param[in] key_ptr       Pointer to the memory for the key generation 
                           result.

  @return
  Status of request.

  @dependencies
  - The passed crypto instance must be initialized to perform the Diffie-Hellman
  algorithm.
  - The maximum calculation time stored for Diffie-Hellman must have a non-zero
  value.
        
  @sideeffects
  If a private key is requested, the generated key will be stored as the
  Diffie-Hellman key for the passed instance of Diffie-Hellman.
*/
secerrno_enum_type seccrypt_pkx_create_key
(
seccrypt_handle_type          handle,
secapi_algo_enum_type         algo,
secapi_pkx_key_enum_type      key,
uint8*                        key_ptr
);

/**
  Decrypts a message using the configured private RSA key. 

  @param[in] handle                Handle to the crypto instance.
  @param[in] algo                  Algorithm.
  @param[in] padding_type          PKX cipher padding type.
  @param[in] ciphertext_ptr        Pointer to the ciphertext data.
  @param[in] ciphertext_len        Length of the ciphertext data buffer.
  @param[in] plaintext_ptr         Pointer to the plaintext data (memory 
                                   provided by the caller).
  @param[in,out] plaintext_len_ptr Pointer to the plaintext data length.

  @return
  E_SUCCESS     - Function executes successfully. \n
  E_NOT_ALLOWED - One or more Diffie-Hellman parameters have not been set. \n 
  E_IN_PROGRESS - CRYPTO-DSP interface was already busy. \n 
  E_FAILURE     - Any other failures.

  @dependencies
  None.
*/
secerrno_enum_type seccrypt_pkx_decrypt
(
seccrypt_handle_type          handle,
secapi_algo_enum_type         algo,
secapi_pkx_padding_enum_type  padding_type,
uint8*                        ciphertext_ptr,
uint16                        ciphertext_len,
uint8*                        plaintext_ptr,
uint16*                       plaintext_len_ptr
);

/**
  Encrypts a message using the configured public RSA key. 

  @param[in] handle                 Handle to the crypto instance.
  @param[in] algo                   Algorithm.
  @param[in] padding_type           PKX cipher padding type.
  @param[in] plaintext_ptr          Pointer to the plaintext data (memory 
                                    provided by the caller).
  @param[in] plaintext_len          Pointer to the plaintext data length.
  @param[in] ciphertext_ptr         Pointer to the plaintext text data.
  @param[in,out] ciphertext_len_ptr Length of the encrypted data 
                                    (ciphertext).

  @return
  E_SUCCESS     - Function executes successfully. \n
  E_NOT_ALLOWED - One or more Diffie-Hellman parameters have not been set. \n
  E_IN_PROGRESS - CRYPTO-DSP interface was already busy. \n
  E_FAILURE     - Any other failures.

  @dependencies
  None.
*/
secerrno_enum_type seccrypt_pkx_encrypt
(
seccrypt_handle_type          handle,
secapi_algo_enum_type         algo,
secapi_pkx_padding_enum_type  padding_type,
uint8*                        plaintext_ptr,
uint16                        plaintext_len,
uint8*                        ciphertext_ptr,
uint16*                       ciphertext_len_ptr
);

/**
  Sets specific Diffie-Hellman parameters for the DH instance specified by the 
  passed instance handle.

  @param[in] handle          Handle to the crypto instance.
  @param[in] algo            Algorithm.
  @param[in] param           Parameter to be set.
  @param[in] param_data_ptr  Pointer to the parameter-specific data.

  @return
  E_SUCCESS - Function executes successfully.

  @dependencies
  None.
*/
secerrno_enum_type seccrypt_pkx_set_param
(
seccrypt_handle_type          handle,
secapi_algo_enum_type         algo,
secpkx_param_enum_type        param,
secpkx_param_data_type*       param_data_ptr
);

/**
  Allows an application to register wait functions for all TCBs from which it 
  might be calling the CRYPTO API.

  A registration of a NULL wait function pointer will clear the entry for this 
  task, causing rex_wait() to be used subsequently.

  @return
  E_SUCCESS       - Registration successful. \n
  E_ALREADY_DONE  - Wait function is already registered for this TCB. \n 
  E_INVALID_ARG   - Called with a NULL fptr; this TCB is not registered. \n 
  E_NOT_ALLOWED   - Registry is full. 

  @dependencies
  Only one wait function can be registered per TCB.
*/
secerrno_enum_type seccrypt_register_wait_func
(
rex_tcb_type*  tcb_ptr,
rex_sigs_type  (*wait_fptr)( rex_sigs_type )
);

/**
  Decrypts the passed message in iovector using the specified algorithm. 

  @param[in] handle         Handle to the crypto instance.
  @param[in] algo           Cipher algorithm specifier.
  @param[in] in_msg_ptr     Pointer to the ciphertext data.
  @param[in] out_msg_ptr    Pointer to the plaintext data (memory provided 
                            by the caller).
  @param[in] consume_input  Cipher in place or not.
  @param[in] mac_pkt_info   MAC information passed in the packet.

  @return 
  E_SUCCESS        - Successful completion. \n
  E_BAD_DATA       - Data verification failed. \n
  E_NOT_SUPPORTED  - Option is currently not supported. \n
  E_FAILURE        - Any other failure to process the request.
   
  @dependencies
  - This function must be called with a complete block ready for decryption if 
  the algorithm is a block cipher. 
  - The memory allocated for the ciphertext must be large enough to
  hold the plaintext equivalent (padding is removed by the cipher).
  - The cipher instance pointer must be properly initialized by seccrypt_new().
  - Cipher algorithm parameters must be properly set by 
  seccrypt_enc_set_param().
*/
secerrno_enum_type seccryptv_enc_decrypt
(
seccrypt_handle_type         handle,
secapi_algo_enum_type        algo,
IOVEC_T*                     in_msg_ptr,
IOVEC_T*                     out_msg_ptr,
boolean                      consume_input,
seccrypt_mac_pkt_info_type*  mac_pkt_info
);

/**
  Encrypts the passed message in the iovector memory using the specified
  algorithm. 

  @param[in] handle                    Handle to the crypto instance.
  @param[in] algo                      Cipher algorithm specifier.
  @param[in] in_msg_ptr                Pointer to the plaintext data.
  @param[in] out_msg_ptr               Pointer to the ciphertext data.
  @param[in] consume_input             Cipher in place or not.
  @param[in] mac_pkt_info              MAC information passed in the packet.

  @return
  E_SUCCESS        - Sucessful completion. \n
  E_BAD_DATA       - Data verification failed. \n 
  E_NOT_SUPPORTED  - Option is not currently supported. \n 
  E_FAILURE        - Any other failure to process the request.
   
  @dependencies
  - The memory allocated for the ciphertext must be large enough to hold the 
  plaintext equivalent and any cipherspecific padding.
  - The cipher instance pointer must be properly initialized by seccrypt_new().
  - Cipher algorithm parameters must be properly set by 
  seccrypt_enc_set_param().
*/

secerrno_enum_type seccryptv_enc_encrypt
(
seccrypt_handle_type         handle,
secapi_algo_enum_type        algo,
IOVEC_T*                     in_msg_ptr,
IOVEC_T*                     out_msg_ptr,
boolean                      consume_input,
seccrypt_mac_pkt_info_type*  mac_pkt_info
);

/**
  Creates a message digest using the algorithm specified.  

  The message is in the iovector type.

  @param[in] handle              Handle to the crypto instance.
  @param[in] handle_type         Hashing algorithm specifier.
  @param[in] msg_iovec_table_ptr iovect type pointer to the message to be 
                                 authenticated.
  @param[out] msg_digest_ptr     Pointer to the message digest (memory 
                                 provided by the caller).

  @return
  E_SUCCESS    - Request was successful. \n 
  E_IN_PROGRES - QDSP is in use. \n 
  E_FAILURE    - Failed to process the request.

  @dependencies
  The hash mode must be properly set by calling seccrypt_hsh_set_param.
*/
secerrno_enum_type seccryptv_hsh_create_digest
(
seccrypt_handle_type   handle,
secapi_algo_enum_type  handle_type,
IOVEC_T*               msg_iovec_table_ptr,
uint8*                 msg_digest_ptr
);

/**
  Creates a hash MAC per RFC 2104 using the specified hash algorithm. 

  The inner and outer message is the IOVEC type.

  @param[in] handle           Handle to the crypto instance.
  @param[in] hmac_type        HMAC types, used per invocation. Includes 
                              HMAC_SSL, TLS, RFC2104, etc.
  @param[in] hmac_algo        HMAC algorithm specifier.
  @param[in] inner_msg_ptr    Inner message pointer for the HMAC calculation.
  @param[in] outer_msg_ptr    Outer message pointer for the HMAC calculation.
  @param[out] hmac_ptr        Pointer to the calculated HMAC value.

  @return
  E_SUCCESS    - Request was successful. \n
  E_IN_PROGRES - QDSP is in use. \n
  E_FAILURE    - Failed to process the request.

  @dependencies
  None.
*/
secerrno_enum_type seccryptv_hsh_create_hmac
(
seccrypt_handle_type          handle,
seccrypt_hsh_hmac_enum_type   hmac_type,
secapi_algo_enum_type         hmac_algo,
IOVEC_T*                      inner_msg_ptr,
IOVEC_T*                      outer_msg_ptr,
uint8*                        hmac_ptr
);

#endif /* SECCRYPT_INTERNAL_H */
