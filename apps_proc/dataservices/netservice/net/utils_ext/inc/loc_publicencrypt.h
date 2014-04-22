/* Copyright (c) 2022 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

/*
!! IMPORTANT NOTE: "DATA SERVICES" VERSION CATEGORY. 
*/

/**
  @file loc_publicencrypt.h
*/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _loc_SharkSslCrypto_h
#define _loc_SharkSslCrypto_h

#define SHARKSSL_LIB 1

#include "qapi_status.h"
#include <stdint.h>

#include "qapi_data_txm_base.h"

/* SSL LOC REQUEST ID DEFINES */

#define  TXM_LOC_SHARKSSL_GENERATE_HANDLE                   TXM_LOC_SSL_BASE + 1
#define  TXM_LOC_SHARKSSL_PEM_EXTRACTPUBLICKEY              TXM_LOC_SSL_BASE + 2
#define  TXM_LOC_SHARKSSLCERTKEY_RSA_PUBLIC_ENCRYPT         TXM_LOC_SSL_BASE + 3
#define  TXM_LOC_SHARKSSLCERTKEY_RSA_PUBLIC_DECRYPT         TXM_LOC_SSL_BASE + 4
#define  TXM_LOC_SHARKSSLAESGCMCTX_CONSTRUCTOR              TXM_LOC_SSL_BASE + 5
#define  TXM_LOC_SHARKSSLAESGCMCTX_DESTRUCTOR               TXM_LOC_SSL_BASE + 6
#define  TXM_LOC_SHARKSSLAESGCMCTX_ENCRYPT                  TXM_LOC_SSL_BASE + 7
#define  TXM_LOC_SHARKSSLAESGCMCTX_DECRYPT                  TXM_LOC_SSL_BASE + 8


typedef void * loc_Ssl_Session_Hdl_t; /**< Location SSL Session Handle. */

typedef struct loc_SharkSslAesParams_s
{
    unsigned char vect[12]; 
   	unsigned char tag[16];
} loc_SharkSslAesParams;



/** RSA Encryption/Decryption type */
 
#define LOC_SHARKSSL_PRIVATE      1
#define LOC_SHARKSSL_PUBLIC       2


/** SHARKSSL_RSA_NO_PADDING for raw RSA encryption (insecure).
 */
#define LOC_SHARKSSL_RSA_NO_PADDING      0


/** SHARKSSL_RSA_PKCS1_PADDING for
    [PKCS #1 v1.5](http://tools.ietf.org/html/rfc2313).
*/
#define LOC_SHARKSSL_RSA_PKCS1_PADDING   1

typedef struct loc_SharkSslAesCtx
{
   unsigned long key[60];
   unsigned short nr;
} loc_SharkSslAesCtx;

typedef struct loc_SharkSslAesGcmCtx
{
   loc_SharkSslAesCtx super;
   unsigned char M0[16][16];
} loc_SharkSslAesGcmCtx;


#ifdef  QAPI_TXM_MODULE     // USER_MODE_DEFS

#define loc_sharkssl_get_Handle(a)                   			 ((int32_t) (_txm_module_system_call12)(TXM_LOC_SHARKSSL_GENERATE_HANDLE      , (ULONG) a, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define loc_sharkssl_PEM_extractPublicKey(a,b,c,d)                   ((int32_t) (_txm_module_system_call12)(TXM_LOC_SHARKSSL_PEM_EXTRACTPUBLICKEY      , (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) d, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define loc_SharkSslCertKey_RSA_public_encrypt(a,b,c,d,e,f,g,h)    ((int32_t) (_txm_module_system_call12)(TXM_LOC_SHARKSSLCERTKEY_RSA_PUBLIC_ENCRYPT    , (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) d, (ULONG) e, (ULONG) f, (ULONG) g, (ULONG) h, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define loc_SharkSslCertKey_RSA_public_decrypt(a,b,c,d,e,f,g,h)    ((int32_t) (_txm_module_system_call12)(TXM_LOC_SHARKSSLCERTKEY_RSA_PUBLIC_DECRYPT  , (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) d, (ULONG) e, (ULONG) f, (ULONG) g, (ULONG) h, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define loc_SharkSslAesGcmCtx_constructor(a,b,c)                 ((int32_t) (_txm_module_system_call12)(TXM_LOC_SHARKSSLAESGCMCTX_CONSTRUCTOR   , (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define loc_SharkSslAesGcmCtx_destructor(a)                      ((int32_t) (_txm_module_system_call12)(TXM_LOC_SHARKSSLAESGCMCTX_DESTRUCTOR    , (ULONG) a, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define loc_SharkSslAesGcmCtx_encrypt(a,b,c,d,e,f)           ((int32_t) (_txm_module_system_call12)(TXM_LOC_SHARKSSLAESGCMCTX_ENCRYPT    , (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) d, (ULONG) e, (ULONG) f, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define loc_SharkSslAesGcmCtx_decrypt(a,b,c,d,e,f)           ((int32_t) (_txm_module_system_call12)(TXM_LOC_SHARKSSLAESGCMCTX_DECRYPT    , (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) d, (ULONG) e, (ULONG) f, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))

/**
 * Macro that passes a byte pool pointer for the Application.
 *
 * Parameter a -- Handle. \n
 * Parameter b -- Pointer to byte pool.
 *
 * On success, QAPI_OK is returned. On error, QAPI_ERROR is returned.
 *
 * @note1hang This macro is only used in the DAM space.
 */


#define loc_SSL_Pass_Pool_Ptr(a,b)                      init_update_byte_pool(a, b)

static __inline int init_update_byte_pool(loc_Ssl_Session_Hdl_t hndl, void *loc_byte_pool_ptr)
{
  int ret = QAPI_ERROR;
  qapi_cb_params_uspace_ptr_t *uspace_memory = NULL;
  
  /* Retrieve the user space information stored internally */
  qapi_data_map_handle_to_u_addr(hndl, LOC_APP_SSL, (void**)&uspace_memory);
  /* Handle to user space information already exists */
  if (uspace_memory)
    return QAPI_OK;
  
  tx_byte_allocate(loc_byte_pool_ptr, (VOID **) &uspace_memory, sizeof(qapi_cb_params_uspace_ptr_t), TX_NO_WAIT);
  if (NULL == uspace_memory)
    return QAPI_ERROR;
  
  memset(uspace_memory, 0x00, sizeof(qapi_cb_params_uspace_ptr_t));

  /* Initialize user space information */
  uspace_memory->usr_pool_ptr = loc_byte_pool_ptr;  

  /* Map the user space information to the handle internally and store */ 
  ret = qapi_data_map_u_addr_to_handle((void *)(hndl), LOC_APP_SSL, uspace_memory, 0x00);
  if (ret != QAPI_OK)
  {
    /* Release the allocated resources */
    if (uspace_memory)
      tx_byte_release(uspace_memory);
  }

  return ret;
}

#else

int32_t loc_SSL_Handler(UINT id, UINT a1, UINT a2, UINT a3, UINT a4, UINT a5, UINT a6, UINT a7, UINT a8, UINT a9, UINT a10, UINT a11, UINT a12);

/** @ingroup loc_sharkssl_get_Handle
 *
 * Initiate a SharkSsl handle
 * @param[in] hndl      Pointer to the handle
 *
 * @return
 * On success, 0. On error, -1. \n
 */

qapi_Status_t loc_sharkssl_get_Handle(loc_Ssl_Session_Hdl_t* hndl);

/** @ingroup loc_sharkssl_PEM_extractPublicKey
 *
 * Extract the public key form a certificate in PEM format
 *
 * @param[in] certPEM   Certificate in PEM format
 * @param[out] key      Pointer to the extracted public key
 * @param[in] type      LOC_SHARKSSL_PRIVATE/LOC_SHARKSSL_PUBLIC if certificate contains private/public key 
 * @param[out] keylen   Returns the length of key generated
 *
 * @return
 * On success, 0. On error, -1. \n
 */

qapi_Status_t loc_sharkssl_PEM_extractPublicKey(const char *certPEM, unsigned char **key, int type, int *keylen);

/* encrypts the 'len' bytes at 'in' using the 'RSAKey' key and stores the result in 'out' */

/** @ingroup loc_SharkSslCertKey_RSA_public_encrypt
 *
 * Encrypt data using the public or private key.
 *
 * @param[in] RSAkey    RSA key
 * @param[in] len       Length/size of parameter 'in'
 * @param[in] in        The plaintext
 * @param[out] out      The encrypted ciphertext is copied to this buffer.
 * @param[out] out_len  The encrypted ciphertext length
 * @param[in] padding   One of SHARKSSL_RSA_PKCS1_PADDING or SHARKSSL_RSA_NO_PADDING
 * @param[in] type      LOC_SHARKSSL_PRIVATE/LOC_SHARKSSL_PUBLIC for private/public RSA encryption
 * @param[in] keylen    The length of key passed 
 *
 * @return
 * On success, 0. On error, -1. \n
 */

qapi_Status_t loc_SharkSslCertKey_RSA_public_encrypt(unsigned char *RSAkey, unsigned short len, unsigned char *in, unsigned char **out, unsigned short* out_len, unsigned char padding, int type, int keyLen);

/** @ingroup loc_SharkSslCertKey_RSA_public_decrypt
 *
 * Decrypt data using the public or private key.
 *
 * @param[in] RSAkey    RSA key
 * @param[in] len       Length/size of parameter 'in'
 * @param[in] in        The encrypted ciphertext (RSA signature)
 * @param[out] out      The message digest is copied to this buffer.
 * @param[out] out_len  The decrypted data length
 * @param[in] padding   One of SHARKSSL_RSA_PKCS1_PADDING or SHARKSSL_RSA_NO_PADDING
 * @param[in] type      LOC_SHARKSSL_PRIVATE/LOC_SHARKSSL_PUBLIC for private/public RSA decryption
 * @param[in] keylen    The length of key passed 
 *
 * @return
 * On success, 0. On error, -1. \n
 */

qapi_Status_t loc_SharkSslCertKey_RSA_public_decrypt(unsigned char *RSAkey, unsigned short len, unsigned char *in, unsigned char **out, unsigned short* out_len, unsigned char padding, int type, int keyLen);

/** @ingroup loc_SharkSslAesGcmCtx_constructor
 *
 * Initialize loc_SharkSslAesGcmCtx
 *
 * @param[out] ctx     Uninitialized data of size sizeof(loc_SharkSslAesGcmCtx)
 * @param[in]  key     The encryption/decryption key
 * @param[in]  keyLen  'key' length
 *
 * @return
 * On success, 0. On error, -1. \n
 */

qapi_Status_t loc_SharkSslAesGcmCtx_constructor(loc_SharkSslAesGcmCtx *ctx, unsigned char *key,  unsigned char keyLen);

/** @ingroup loc_SharkSslAesGcmCtx_destructor
 *
 *  Clear loc_SharkSslAesGcmCtx
 *
 * @param[in] ctx     Initialized data of size sizeof(loc_SharkSslAesGcmCtx)
 *
 * @return
 * On success, 0. On error, -1. \n
 */

qapi_Status_t loc_SharkSslAesGcmCtx_destructor(loc_SharkSslAesGcmCtx *ctx);


/** @ingroup loc_SharkSslAesGcmCtx_encrypt
 *
 * Encrypt data or a chunk of a large data set.
 *
 * @param[in] ctx         context initialized by SharkSslAesGcmCtx_constructor
 * @param[in/out] params  loc_SharkSslAesParams structure containing the initialization vector (IV)  which is a (public) fixed-size of 12 bytes input typically created from random data.
 *                        A 16 byte xored tag created as the data is encrypted. Set this to a random number for the first call to this function, or just leave the 16 byte data-buffer uninitialized. 
 * @param[in] auth		  The cipher's optional additional authenticated data
 * @param[in] authlen	  The length of the 'auth' parameter
 * @param[in/out] input   The data to be encrypted and encrypted data is copied back to this buffer
 * @param[in] len         The length of the input block
 * @return
 * On success, 0. On error, -1. \n
 */

qapi_Status_t loc_SharkSslAesGcmCtx_encrypt(loc_SharkSslAesGcmCtx *ctx, loc_SharkSslAesParams* params, unsigned char *auth, unsigned short authlen, unsigned char *input, unsigned short len);


/** @ingroup loc_SharkSslAesGcmCtx_decrypt
 *
 * Decrypt data or a chunk of a large data set.
 *
 * @param[in] ctx         context initialized by SharkSslAesGcmCtx_constructor
 * @param[in] params      loc_SharkSslAesParams structure containing the initialization vector (IV)  which is a (public) fixed-size of 12 bytes input typically created from random data.
 *                        the tagout from loc_SharkSslAesGcmCtx_encrypt. This data will change for each call to SharkSslAesGcmCtx_decrypt.
 * @param[in] auth		  The cipher's optional additional authenticated data
 * @param[in] authlen	  The length of the 'auth' parameter
 * @param[in/out] input   The data to be decrypted and decrypted data is copied back to this buffer
 * @param[in] len         The length of the input block
 * @return
 * On success, 0. On error, -1. \n
 */

qapi_Status_t loc_SharkSslAesGcmCtx_decrypt(loc_SharkSslAesGcmCtx *ctx, loc_SharkSslAesParams* params, unsigned char *auth, unsigned short authlen, unsigned char *input, unsigned short len);

#endif

#endif

#ifdef __cplusplus
}
#endif
