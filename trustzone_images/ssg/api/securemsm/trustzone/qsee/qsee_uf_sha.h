#ifndef QSEE_UF_SHA_H
#define QSEE_UF_SHA_H

/**
@file qsee_uf_sha.h
@brief Provide Software crypto Hash and Hmac API wrappers
*/

/*===========================================================================
   Copyright (c) {2012, 2019} by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <stdint.h>

/**
@addtogroup qtee_uf_sha
@{
  Within Unified SHA, the following return values are defined by 
  the Unified Crypto Environment:
  - UC_E_SUCCESS            =  0 
  - UC_E_FAILURE            =  1
  - UC_E_NOT_ALLOWED        =  2
  - UC_E_NOT_AVAILABLE      =  3
  - UC_E_NOT_SUPPORTED      =  4
  - UC_E_NO_MEMORY          = 15
  - UC_E_INVALID_ARG        = 16
*/
/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

// SHA DIGEST and Block sizes
#define SW_SHA_BLOCK_SIZE               64
#define SW_SHA1_BLOCK_SIZE              SW_SHA_BLOCK_SIZE
#define SW_SHA1_DIGEST_SIZE             (160 / 8)

#define SW_SHA256_BLOCK_SIZE            SW_SHA1_BLOCK_SIZE
#define SW_SHA256_DIGEST_SIZE           (256 / 8)

#define SW_SHA224_BLOCK_SIZE            SW_SHA256_BLOCK_SIZE
#define SW_SHA224_DIGEST_SIZE           (224 / 8)

#define SW_SHA512_BLOCK_SIZE            128
#define SW_SHA512_DIGEST_SIZE           (512 / 8)

#define SW_SHA384_BLOCK_SIZE            SW_SHA512_BLOCK_SIZE
#define SW_SHA384_DIGEST_SIZE           (384 / 8)

#define SW_MD5_BLOCK_SIZE               (512 / 8)
#define SW_MD5_DIGEST_SIZE              16

#define SW_SM3_BLOCK_SIZE               64
#define SW_SM3_DIGEST_SIZE              32
/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/**
  Software hash algorithm types.
 */
typedef enum
{
  SW_AUTH_ALG_NULL               = 0x1,       /**< Default NULL algorithm. */
  SW_AUTH_ALG_SHA1               = 0x2,       /**< SHA1 hash algorithm. */
  SW_AUTH_ALG_SHA256,                         /**< SHA256 hash algorithm. */
  SW_AUTH_ALG_SHA224,                         /**< SHA224 hash algorithm. */
  SW_AUTH_ALG_SHA384,                         /**< SHA384 hash algorithm. */
  SW_AUTH_ALG_SHA512,                         /**< SHA512 hash algorithm. */
  SW_AUTH_ALG_MD5,                            /**< MD5 hash algorithm. */
  SW_AUTH_ALG_SM3,                            /**< SM3 hash algorithm. */
  SW_AUTH_ALG_MAX,
  SW_AUTH_ALG_INVALID            = 0x7FFFFFFF 
  /**< Unknown or invalid algorithm. */
} SW_Auth_Alg_Type;

/** 
  Software hash parameters.
 */
typedef enum 
{ 
  SW_HASH_PARAM_MODE              = 0x01,       /**< Hashing mode parameter. */
  SW_HASH_PARAM_IV                = 0x02,
  /**< Initialization Vector parameter. */
  SW_HASH_PARAM_HMAC_KEY          = 0x03,
  /**< HMAC Key value parameter. */
  SW_HASH_PARAM_AUTH_KEY          = 0x04,
  /**< Authentication key parameter. */
  SW_HASH_PARAM_COPY              = 0x05,
  /**< Make a copy of the input Hash context handle. */
  SW_HASH_PARAM_HMAC_COPY         = 0x06,
  /**< Make a copy of the input HMAC context handle. */
  SW_HASH_PARAM_MAX               = 0x7FFFFFFF
  /**< Unknown or invalid parameter. */
} SW_Auth_Param_Type;

/** @cond */
#ifndef SW_CRYPTO_TYPEDEF
#define SW_CRYPTO_TYPEDEF
typedef uint32_t sw_crypto_errno_enum_type;
#endif

#ifndef IOVECDEF
#define IOVECDEF
typedef struct
{
  void                              *pvBase;
  uint32_t                            dwLen;
} __attribute__((__packed__)) IovecType;

typedef struct
{
  IovecType                     *iov;
  uint32_t                        size;
} __attribute__((__packed__)) IovecListType;
#endif

#ifndef _DEFINED_CRYPTOTOCNTXHANDLE
#define _DEFINED_CRYPTOTOCNTXHANDLE
typedef void CryptoCntxHandle;
#endif

/** @endcond */

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
  Initializes a hash context.
 
  @param[out] handle      Double pointer to hash context handle.
  @param[in] auth_alg     Hash algorithm, see SW_Auth_Alg_Type
 
  @return
  UC_E_SUCCESS -- Function executes successfully. \n
  UC_E_FAILURE -- Operation failed due to unknown error. \n
  UC_E_NOT_ALLOWED -- Operation currently not allowed.  \n
  UC_E_NOT_SUPPORTED -- Operation not yet implemented. \n
  UC_E_NO_MEMORY -- Allocation from a memory pool failed. \n
  UC_E_INVALID_ARG -- Unrecognized argument. \n
 */
sw_crypto_errno_enum_type qsee_SW_Hash_Init(CryptoCntxHandle** handle, SW_Auth_Alg_Type auth_alg);


/**
  Resets a hash context.
 
  @param[in,out] handle     Pointer to hash context handle.
 
  @return
  UC_E_SUCCESS -- Function executes successfully. \n
  UC_E_INVALID_ARG -- Unrecognized argument. \n
 */
sw_crypto_errno_enum_type qsee_SW_Hash_Reset(CryptoCntxHandle* handle);

/**
  Updates the function for intermediate data blocks hash.
 
  @param[in,out] handle   Pointer to hash context handle.
  @param[in] ioVecIn      Input to hash.
 
  @return
  UC_E_SUCCESS -- Function executes successfully. \n
  UC_E_INVALID_ARG -- Unrecognized argument. \n
 */
sw_crypto_errno_enum_type qsee_SW_Hash_Update(CryptoCntxHandle* handle, IovecListType ioVecIn);

/**
  Last block hash update.
 
  @param[in] handle       Pointer to hash context handle.
  @param[out] ioVecOut    Pointer to hash output vector.
 
  @return
  UC_E_SUCCESS -- Function executes successfully. \n
  UC_E_INVALID_ARG -- Unrecognized argument. \n
 */
sw_crypto_errno_enum_type qsee_SW_Hash_Final (CryptoCntxHandle* handle, IovecListType *ioVecOut);

/**
  Deinitializes hash context.

  @param[in] cntx       Double pointer to hash context handle.

  @return
  UC_E_SUCCESS -- Function executes successfully. \n
  UC_E_INVALID_ARG -- Unrecognized argument. \n
 */
sw_crypto_errno_enum_type qsee_SW_Hash_Deinit(CryptoCntxHandle** cntx);

/**
  Hashes data into the hash context structure (must be
  initialized by qsee_SW_Hash_Init(...)).
 
  @param[in,out] handle  Pointer to hash context handle.
  @param[in] msg         Pointer to the data to hash.
  @param[in] msg_len     Length of the data to hash.
 
  @return
  UC_E_SUCCESS -- Function executes successfully. \n
  UC_E_INVALID_ARG -- Unrecognized argument. \n

  @dependencies 
  qsee_SW_Hash_Init
 */
sw_crypto_errno_enum_type qsee_SW_Hash_Update_Ez(CryptoCntxHandle* handle, const uint8_t *msg, uint32_t msg_len);

/**
  Computes the digest hash value.
 
  @param[in] handle       Pointer to hash context handle.
  @param[out] digest      Pointer to output message digest hash.
  @param[in] digest_len   Length of the output message digest hash
                          buffer in bytes.
 
  @return
  UC_E_SUCCESS -- Function executes successfully. \n
  UC_E_NO_MEMORY -- Allocation from a memory pool failed. \n
  UC_E_INVALID_ARG -- Unrecognized argument. \n
 
  @dependencies
  qsee_SW_Hash_Init
 */
sw_crypto_errno_enum_type qsee_SW_Hash_Final_Ez(CryptoCntxHandle* handle, uint8_t *digest, uint32_t digest_len);

/**
  Sets hash parameters (Mode and Key for HMAC).
 
  @param[in,out] cntx   Pointer to hash context handle.
  @param[in] nParamID   HMAC parameter ID to set.
  @param[in] pParam     Pointer to parameter data.
  @param[in] cParam     Size of parameter data in bytes.
  @param[in] pAlgo      Algorithm type.
 
  @return 
  UC_E_SUCCESS -- Function executes successfully. \n
  UC_E_INVALID_ARG -- Unrecognized argument. \n
 */ 
sw_crypto_errno_enum_type qsee_SW_Hash_SetParam (CryptoCntxHandle*   cntx,
                                                SW_Auth_Param_Type  nParamID, 
                                                const void         *pParam, 
                                                uint32_t             cParam,  
                                                SW_Auth_Alg_Type   pAlgo);

/**
  Main function for HMAC.
 
  @param[in] key_ptr      Pointer to Key for HMAC.
  @param[in] keylen       Key length.
  @param[in] ioVecIn      Hash input vector.
  @param[out] ioVecOut    Pointer to hash output vector.
  @param[in] pAlgo        HMAC algorithm to use (see SW_Auth_Alg_Type).
 
  @return
  UC_E_SUCCESS -- Function executes successfully. \n
  UC_E_FAILURE -- Operation failed due to unknown error. \n
  UC_E_NOT_ALLOWED -- Operation currently not allowed.  \n
  UC_E_NOT_SUPPORTED -- Operation not yet implemented. \n
  UC_E_NO_MEMORY -- Allocation from a memory pool failed. \n
  UC_E_INVALID_ARG -- Unrecognized argument. \n
 */
sw_crypto_errno_enum_type qsee_SW_Hmac (uint8_t * key_ptr, uint32_t keylen, IovecListType ioVecIn,
                              IovecListType* ioVecOut, SW_Auth_Alg_Type  pAlgo);

/**
  Initializes hash HMAC using the specified hash algorithm.
 
  @param[out] cntx        Double pointer to HMAC context handle.
  @param[in] pAlgo        Algorithm to use (see SW_Auth_Alg_Type).
 
  @return 
  UC_E_SUCCESS -- Function executes successfully. \n
  UC_E_FAILURE -- Operation failed due to unknown error. \n
  UC_E_NOT_ALLOWED -- Operation currently not allowed.  \n
  UC_E_NOT_SUPPORTED -- Operation not yet implemented. \n
  UC_E_NO_MEMORY -- Allocation from a memory pool failed. \n
  UC_E_INVALID_ARG -- Unrecognized argument. \n
 */
sw_crypto_errno_enum_type qsee_SW_Hmac_Init(CryptoCntxHandle** cntx, SW_Auth_Alg_Type  pAlgo);

/**
  Updates function for intermediate data blocks hash.
 
  @param[in,out] cntx     Pointer to HMAC context handle.
  @param[in] ioVecIn      Hash input vector.
 
  @return 
  UC_E_SUCCESS -- Function executes successfully. \n
  UC_E_FAILURE -- Operation failed due to unknown error. \n
  UC_E_NOT_ALLOWED -- Operation currently not allowed.  \n
  UC_E_NOT_SUPPORTED -- Operation not yet implemented. \n
  UC_E_NO_MEMORY -- Allocation from a memory pool failed. \n
  UC_E_INVALID_ARG -- Unrecognized argument. \n
 */
sw_crypto_errno_enum_type qsee_SW_Hmac_Update(CryptoCntxHandle*  cntx, IovecListType ioVecIn);

/**
  Resets HMAC context.

  @param[in] cntx        Pointer to HMAC context handle.

  @return
  UC_E_SUCCESS -- Function executes successfully. \n
  UC_E_INVALID_ARG -- Unrecognized argument. \n
  UC_E_NO_MEMORY -- Allocation from a memory pool failed. \n
*/
sw_crypto_errno_enum_type qsee_SW_Hmac_Reset(CryptoCntxHandle* cntx);

/**
  HMAC last block hash update.
 
  @param[in] cntx         Pointer to HMAC context handle.
  @param[out] ioVecOut    Pointer to HMAC output vector.
 
  @return
  UC_E_SUCCESS -- Function executes successfully. \n
  UC_E_FAILURE -- Operation failed due to unknown error. \n
  UC_E_INVALID_ARG -- Unrecognized argument. \n
 */
 sw_crypto_errno_enum_type qsee_SW_Hmac_Final(CryptoCntxHandle* cntx, IovecListType * ioVecOut);
 
/**
  Deinitializes HMAC context.
 
  @param[in] cntx          Double pointer to HMAC context handle.
 
  @return
  UC_E_SUCCESS -- Function executes successfully. \n
  UC_E_FAILURE -- Operation failed due to unknown error. \n
 */
sw_crypto_errno_enum_type qsee_SW_Hmac_Deinit(CryptoCntxHandle** cntx);

/** @} */

#endif /* QSEE_UF_SHA_H */
