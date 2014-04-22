#ifndef CRYPTOUTILS_H_INCLUDED
#define CRYPTOUTILS_H_INCLUDED
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                               <CryptoUtils.h>
  DESCRIPTION
    Definitions and declarations for crypto utility functions using
    GP library.

  Copyright (c) 2018 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*==========================================================================
                                EDIT HISTORY

  $Header: //components/rel/ssg.tz/1.1.3/securemsm/trustzone/qsapps/libs/tee_se_utils/inc/CryptoUtils.h#1 $

    when     who     what, where, why
  --------   ---     -------------------------------------------------------
  26/08/17   ADG     Initial version
  ==========================================================================*/

/*==========================================================================*
 *                             INCLUDE FILES                                *
 *==========================================================================*/
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "tee_internal_api.h"
#include "tee_internal_se_api.h"

#ifdef __cplusplus
extern "C" {
#endif
/*==========================================================================*
 *                              DATA TYPES                                  *
 *==========================================================================*/

/* Convert between ECC public and private key sizes:               */
/* - public key in the uncompressed form of TR-03111 section 3.2.1 */
/* - private key as an array of bytes                              */
#define ECCSK2PK(skBytes)             ((2 * (skBytes)) + 1)
#define ECCPK2SK(pkBytes)             (((pkBytes) - 1) / 2)
#define ECCPK_PACKED_TAG              0x04

/* SHA block and digest sizes */
#define SHA256_BLOCK_SIZE_BITS        (64)
#define SHA256_DIGEST_SIZE_BYTES      (256 / 8)

#define SHA384_BLOCK_SIZE_BITS        (128)
#define SHA384_DIGEST_SIZE_BYTES      (384 / 8)

#define SHA512_BLOCK_SIZE_BITS        (128)
#define SHA512_DIGEST_SIZE_BYTES      (512 / 8)

/* SHA algorithms supported */
#define SHA_ALGO_SHA256               TEE_ALG_SHA256
#define SHA_ALGO_SHA384               TEE_ALG_SHA384
#define SHA_ALGO_SHA512               TEE_ALG_SHA512
#define SHA_ALGO_INVALID              0xFFFFFFFF

typedef uint32_t                      SHAALGORITHM;

/* Key Parameter Reference values */
#define KPREF_NIST_P256               0x00
#define KPREF_NIST_P384               0x01
#define KPREF_NIST_P521               0x02
#define KPREF_BP_P256r1               0x03
#define KPREF_BP_P256t1               0x04
#define KPREF_BP_P384r1               0x05
#define KPREF_BP_P384t1               0x06
#define KPREF_BP_P512r1               0x07
#define KPREF_BP_P512t1               0x08
#define KPREF_INVALID                 0xFF

/*==========================================================================*
 *                              DEFINITIONS                                 *
 *==========================================================================*/

/*==========================================================================*/
/**
 *  @brief  Increment an arbitrarily sized Big Endian integer used as an
 *          IV Counter for CBC Mode
 *
 *  @param  [in,out] pIVCounter     Pointer to the integer buffer
 *  @param  [in]     length         Length of the integer buffer
 *
 *  @return Nothing
 */
/*==========================================================================*/
extern void IncrementIVCounter(uint8_t *pIVCounter, size_t length);

/*==========================================================================*/
/**
 *  @brief  Calculate and return a digest on a block of data
 *
 *  @param  [in]    algorithm       The algorithm to use
 *  @param  [in]    pMsg            The input data for the digest
 *  @param  [in]    msgLength       The size of the digest input data
 *  @param  [out]   pDigest         Buffer to receive the digest
 *  @param  [in]    digestLength    The size of the digest buffer
 *
 *  @return The actual digest length if successful, otherwise zero.
 */
/*==========================================================================*/
extern size_t SHADigest(SHAALGORITHM   algorithm,
                        const uint8_t *pMsg,
                        size_t         msgLength,
                        uint8_t       *pDigest,
                        size_t         digestLength);

/*==========================================================================*/
/**
 *  @brief  Determine the GP TEE_ECC_CURVE_* value given the
 *          Key Parameter Reference value used in many APDUs
 *
 *  @param  [in]    kpRef           The Key Parameter Reference value
 *
 *  @return The corresponding TEE_ECC_CURVE_* value if successful,
 *          or zero otherwise.
 */
/*==========================================================================*/
extern uint32_t GetECCCurveFromKPRef(uint8_t kpRef);

/*==========================================================================*/
/**
 *  @brief  Determine the Key Parameter Reference value given
 *          the corresponding GP TEE_ECC_CURVE_* value.
 *
 *  @param  [in]    teeCurve        The TEE_ECC_CURVE_* value
 *
 *  @return The corresponding Key Parameter Reference value if successful,
 *          or KPREF_INVALID otherwise.
 */
/*==========================================================================*/
extern uint8_t GetKPRefFromECCCurve(uint32_t teeCurve);

/*==========================================================================*/
/**
 *  @brief  Return the key size in bits, and the correct ECDSA and digest
 *          algorithm and AES key size for given TEE_ECC_CURVE_* value.
 *
 *  @param  [in]    teeCurve        The TEE_ECC_CURVE_* value
 *  @param  [out]   pKeyBits        Pointer to a variable for the key size
 *                                  in bits - NULL if not required
 *  @param  [out]   pDigestAlgo     Pointer to a variable for the digest
 *                                  algorithm - NULL if not required
 *  @param  [out]   pDigestBits     Pointer to a variable for the digest
 *                                  size - NULL if not required
 *  @param  [out]   pEcdsaAlgo      Pointer to a variable for the ECDSA
 *                                  algorithm - NULL if not required
 *  @param  [out]   pAESKeyBits     Pointer to a variable for the AES
 *                                  key size - NULL if not required
 *
 *  @return TRUE if the ECC Curve was valid, otherwise FALSE.
 *          In the latter case all returned parameters will also be zero.
 */
/*==========================================================================*/
extern bool GetECCInfo(uint32_t  teeCurve,
                       size_t   *pKeyBits,
                       uint32_t *pDigestAlgo,
                       size_t   *pDigestBits,
                       uint32_t *pECDSAAlgo,
                       size_t   *pAESKeyBits);

/*==========================================================================*/
/**
 *  @brief  Return the key size in bits, and the correct ECDSA and digest
 *          algorithm and AES key size for given Key Parameter Reference
 *          value.
 *
 *  @param  [in]    kpRef           The Key Parameter Reference value
 *  @param  [out]   pTEECurve       Pointer to a variable for the TEE_ECC_CURVE_*
 *                                  value - NULL if not required
 *  @param  [out]   pKeyBits        Pointer to a variable for the key size
 *                                  in bits - NULL if not required
 *  @param  [out]   pDigestAlgo     Pointer to a variable for the digest
 *                                  algorithm - NULL if not required
 *  @param  [out]   pDigestBits     Pointer to a variable for the digest
 *                                  size - NULL if not required
 *  @param  [out]   pEcdsaAlgo      Pointer to a variable for the ECDSA
 *                                  algorithm - NULL if not required
 *  @param  [out]   pAESKeyBits     Pointer to a variable for the AES
 *                                  key size - NULL if not required
 *
 *  @return TRUE if the Key Parameter Reference was valid, otherwise FALSE.
 *          In the latter case all returned parameters will also be zero.
 */
/*==========================================================================*/
extern bool GetKPRefInfo(uint32_t  kpRef,
                         uint32_t *pTEECurve,
                         size_t   *pKeyBits,
                         uint32_t *pDigestAlgo,
                         size_t   *pDigestBits,
                         uint32_t *pECDSAAlgo,
                         size_t   *pAESKeyBits);

/*==========================================================================*/
/**
 *  @brief  Pack an ECC Public Key into the form described in
 *          TR-03111 section 3.2.1.
 *
 *  @param  [out]   pDest           The buffer to receive the packed key
 *  @param  [in]    destSize        The size of the destination buffer
 *  @param  [in]    keyObject       A Transient Object containing the
 *                                  Public Key to be packed.
 *
 *  @return The packed key size in bytes, or 0 on failure
 */
/*==========================================================================*/
extern size_t PackECCPublicKey(uint8_t          *pDest,
                               size_t            destSize,
                               TEE_ObjectHandle  keyObject);

/*==========================================================================*/
/**
 *  @brief  Pack an ECC Private Key into a buffer.
 *
 *  @param  [out]   pDest           The buffer to receive the packed key
 *  @param  [in]    destSize        The size of the destination buffer
 *  @param  [in]    keyObject       A Transient Object containing the
 *                                  Private Key to be packed.
 *
 *  @return The packed key size in bytes, or 0 on failure
 */
/*==========================================================================*/
extern size_t PackECCPrivateKey(uint8_t          *pDest,
                                size_t            destSize,
                                TEE_ObjectHandle  keyObject);

/*==========================================================================*/
/**
 *  @brief  Create a ECC Key Pair or Public Key Transient Object, from
 *          the public/private key byte arrays (if given).
 *          Optionally generate the keys if not supplied.
 *
 *  @param  [in]    eccCurve        One of the TEE_ECC_CURVE_* constants
 *  @param  [in]    objectType      The type of key object to create. One of:
 *                                    TEE_TYPE_ECDSA_PUBLIC_KEY,
 *                                    TEE_TYPE_ECDSA_KEYPAIR,
 *                                    TEE_TYPE_ECDH_PUBLIC_KEY,
 *                                    TEE_TYPE_ECDH_KEYPAIR
 *  @param  [in]    pPk             Pointer to the public key, formatted as
 *                                  described in TR-03111 section 3.2.1
 *  @param  [in]    pkSize          Public key buffer length, in bytes
 *  @param  [in]    pSk             Pointer to the private key
 *  @param  [in]    skSize          Private key buffer length, in bytes
 *  @param  [in]    canGenerate     Generate the key if not given
 *
 *  @return the object handle, or TEE_HANDLE_NULL on failure
 */
/*==========================================================================*/
extern TEE_ObjectHandle CreateECCKeyObject(uint32_t eccCurve,
                                           uint32_t objectType,
                                           const uint8_t *pPk, size_t pkSize,
                                           const uint8_t *pSk, size_t skSize,
                                           bool canGenerate);


/*==========================================================================*
 *                              END OF FILE                                 *
 *==========================================================================*/
#ifdef __cplusplus
}
#endif

#endif /* CRYPTOUTILS_H_INCLUDED */

