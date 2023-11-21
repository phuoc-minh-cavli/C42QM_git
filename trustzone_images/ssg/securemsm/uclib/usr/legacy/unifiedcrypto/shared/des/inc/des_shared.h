#ifndef DES_SHARED_H
#define DES_SHARED_H
/*===========================================================================

                             Unified Crypto 

GENERAL DESCRIPTION


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/ssg.tz/1.1.3/securemsm/uclib/usr/legacy/unifiedcrypto/shared/des/inc/des_shared.h#1 $
  $DateTime: 2019/05/23 05:32:51 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/16/13   nk     initial version
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uc_comdef.h"


/**
 * DES Key and IV sizes
 */
#define SW_DES128_KEY_SIZE        16
#define SW_DES_IV_SIZE            8
#define SW_DES_BLOCK_BYTE_LEN     8    /**< DES cipher block length. */
#define SW_3DES_BLOCK_BYTE_LEN    8    /**< 3DES cipher block length. */
#define SW_DES192_KEY_SIZE        24
#define SW_DES256_KEY_SIZE        32
#define SW_DES_MAX_KEY_SIZE       60

#define SW_3DES_KEY_BYTE_LEN      24   /**< 3DES cipher key length. */
#define SW_DES_KEY_BYTE_LEN       8    /**< DES cipher key length. */

#ifndef IOVECDEF  
typedef    struct 
{
  void    *pvBase;
  uint32  dwLen;
} IovecType;

typedef    struct 
{
  IovecType    *iov;
  uint32       size;
} IovecListType;

#define IOVECDEF

#endif  

/**
 * Enum for Cipher DES mode
 */
typedef enum 
{
  SW_CIPHER_DES_MODE_ECB          = 0x0,    ///< ECB Mode
  SW_CIPHER_DES_MODE_CBC          = 0x1    ///< CBC Mode
} SW_CipherDESModeType;

/**
 * Enum for Cipher DES direction
 */
typedef enum 
{
  SW_CIPHER_DES_ENCRYPT           = 0x00,
  SW_CIPHER_DES_DECRYPT           = 0x01,
  SW_CIPHER_DES_MAX               = 0x7FFFFFFF
} SW_CipherDESEncryptDir;

/**
 * Enum for Cipher DES parameters
 */
typedef enum 
{ 
  SW_CIPHER_DES_PARAM_DIRECTION   = 0x01,
  SW_CIPHER_DES_PARAM_KEY         = 0x02,
  SW_CIPHER_DES_PARAM_IV          = 0x03,
  SW_CIPHER_DES_PARAM_MODE        = 0x04,
  SW_CIPHER_DES_PARAM_BLOCKSIZE   = 0x05,
  SW_CIPHER_DES_PARAM_KEY_SIZE    = 0x06,
  SW_CIPHER_DES_PARAM_IV_SIZE     = 0x07,
  SW_CIPHER_DES_PARAM_COPY        = 0x08,
  SW_CIPHER_DES_PARAM_MAX         = 0x7FFFFFFF
} SW_CipherDESParam;

/**
 * Enum for Cipher DES algorithm type
 */
typedef enum {
  SW_CIPHER_ALG_DES               = 0x0,
  SW_CIPHER_ALG_DES3              = 0x1,
  SW_CIPHER_ALG_DES_MAX           = 0x7FFFFFFF
} SW_Cipher_DES_Alg_Type;
/**
 * Enum for Cipher key size
 */
typedef enum 
{
  SW_CIPHER_KEY_SIZE_DES          = 0x0,
  SW_CIPHER_KEY_SIZE_3DES         = 0x1,
  SW_CIPHER_KEY_SIZE_DES_MAX      = 0x7FFFFFFF
} SW_Cipher_DES_Key_Size;

typedef  union
{
    
  /* DES info - needs to be same size as 3DES */
    struct __des_cipher_s
  {
     struct __des_cipher_enc_s
    {
      uint8    iv[SW_DES_BLOCK_BYTE_LEN];
      uint8    key[SW_3DES_KEY_BYTE_LEN];
      uint8    subkey[16][SW_DES_BLOCK_BYTE_LEN];
  
      uint8    __reserved_key_space[2][16][SW_DES_BLOCK_BYTE_LEN];
      boolean_t    subkey_generated;
    } enc;

     struct __des_cipher_dec_s
   {
     uint8     iv[SW_DES_BLOCK_BYTE_LEN];
     uint8     key[SW_3DES_KEY_BYTE_LEN];
     uint8     subkey[16][SW_DES_BLOCK_BYTE_LEN];

     boolean_t    subkey_generated;
   } dec;
  } des __attribute__((packed));

  /* 3DES info */
   struct __des3_cipher_s  
  {
      struct __des3_cipher_enc_s
    {
      uint8  iv[SW_DES_BLOCK_BYTE_LEN];
      uint8  key[SW_3DES_KEY_BYTE_LEN];
      uint8  subkey[3][16][SW_DES_BLOCK_BYTE_LEN];
  
      boolean_t    subkey_generated;
    }enc __attribute__((packed)) ;
  
     struct __des3_cipher_dec_s
   {
      uint8  iv[SW_DES_BLOCK_BYTE_LEN];
      uint8  key[SW_3DES_KEY_BYTE_LEN];
      uint8  subkey[3][16][SW_DES_BLOCK_BYTE_LEN];
  
      boolean_t    subkey_generated;
   } dec __attribute__((packed)) ;
  }des3;
}des_cipher_info;

/**
 * Cipher Context definition 
 */
typedef  struct 
{
  SW_Cipher_DES_Alg_Type      algo;
  SW_Cipher_DES_Key_Size      keysize;
  SW_CipherDESEncryptDir      dir;
  SW_CipherDESModeType        mode;
  boolean_t                   bInit;
  des_cipher_info             cipher_info;
   
} SW_DES_Ctx;

#ifndef _DEFINED_CRYPTOCNTXHANDLE
#define _DEFINED_CRYPTOCNTXHANDLE
typedef void CryptoCntxHandle;
#endif

/**
 * @brief Intialize a cipher context 
 *
 * @param handle       [in] Pointer of pointer to cipher context
 * @param pAlgo        [in] Cipher algorithm type 
 *
 * @return sw_crypto_errno_enum_type
 *
 * @see 
 *
 */

sw_crypto_errno_enum_type SW_Cipher_DES_Init(CryptoCntxHandle **handle, SW_Cipher_DES_Alg_Type pAlgo);

/**
 * @brief Deintialize a cipher context 
 *
 * @param handle       [in] Pointer to pointer to cipher context
 *
 * @return errno_enum_type
 *
 * @see 
 *
 */

sw_crypto_errno_enum_type SW_Cipher_DES_DeInit(CryptoCntxHandle  **handle);

/**
 * @brief This function encrypts/decrypts the passed message
 *        using the specified algorithm.
 *
 * @param handle       [in] Pointer of pointer to cipher context
 * @param ioVecIn      [in] Input to cipher
 * @param ioVecOut     [in] Output from cipher
 *
 * @return sw_crypto_errno_enum_type
 *
 * @see 
 *
 */

sw_crypto_errno_enum_type SW_CipherDESData (CryptoCntxHandle   *handle,
                                                  IovecListType         ioVecIn,
                                                  IovecListType         *ioVecOut);



/**
 * @brief This functions sets the Cipher paramaters used by 
 *        SW_CRYPTO_CipherDESData
 *
 * @param handle    [in] Pointer of pointer to cipher context
 * @param nParamID  [in] Cipher parameter id to set
 * @param pParam    [in] Pointer to parameter data 
 * @param cParam    [in] Size of parameter data in bytes
 *
 * @return sw_crypto_errno_enum_type
 *
 * @see SW_CRYPTO_Cipher_ProcessBlocks 
 *
 */
sw_crypto_errno_enum_type SW_Cipher_DES_SetParam (CryptoCntxHandle   *handle,
                                                          SW_CipherDESParam      nParamID,
                                                          const void            *pParam,
                                                          uint32                 cParam );


#endif /* DES_SHARED */
