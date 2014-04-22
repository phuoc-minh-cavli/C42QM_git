/*
@file ipprotector_api.h
@brief This file contains all definitions used in ipprotector
*/
/*===========================================================================
  Copyright (c) 2018, 2021 by Qualcomm Technologies, Incorporated.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //components/rel/ssg.tz/1.1.3/securemsm/trustzone/qsapps/libs/ipprotectorlib/ipprotector_api.h#2 $
  $DateTime: 2021/10/01 11:04:45 $
  $Author: pwbldsvc $

# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
===========================================================================*/

#ifndef __IPPROTECTOR_API_H__
#define __IPPROTECTOR_API_H__

enum error_codes {
    /* These are IP Protector Lib error codes */
    IP_OUT_OF_MEMORY = 100,
    IP_MISSING_BUFFER,
    IP_INVALID_PARAMETER,
    IP_OBJECT_ERROR,
    /* IP Protector Kernel Service error codes are between 200 and 300 */
    IP_KERNEL_ERROR = 200,
    /* These are ELF Parser error codes */
    IP_ELF_CONSTRUCTOR_FAILURE = 300,
    IP_SEGMENT_CONSTRUCTOR_FAILURE,
    IP_INVALID_NUMBER_OF_SEGMENTS,
    IP_MISSING_HASH_SEGMENT,
    IP_INVALID_PHDR_SEGMENT,
    IP_METADATA_OVERLAP,
};

/*
 * Description: Decrypt encrypted LOAD segments of ELF.
 *              In order for decryption to be successful:
 *              1) ELF must have been encrypted via Sectools using a
 *                 UIE_feature_id value matching feat_id.
 *              2) Sectools must have encrypted the ELF using the
 *                 Public Key retrieved via ipprotector_get_public_key
 *                 on a Device with the same configured values for MSA,
 *                 APPS Secure, OEM ID, OEM RCH, OTP UIE L1 Key, and
 *                 Feature ID.
 *
 * In:          feat_id: Feature ID to use during key derivation
 *              image_addr: Address of buffer containing encrypted ELF
 *              image_size: Size of buffer containing encrypted ELF
 *              image_out_size: Size of buffer containing decrypted ELF
 * Out:         image_out_addr: Buffer containing decrypted ELF. ELF
                                will be decrypted in-place if provided
                                image_out_addr is NULL
 * Return:      SUCCESS on success.
 *              Any other error code on failure.
 */
int32_t ipprotector_decrypt(uint32_t feat_id, uint8_t *image_addr,
                            uint32_t image_size, uint8_t *image_out_addr,
                            uint32_t image_out_size);

/*
 * Description: Verify signature of ELF. If ELF is encrypted, ipprotector_decrypt
 *              must first be called to decrypt the ELF before calling
 *              ipprotector_verify_sig.
 *              In order for verification to be successful:
 *              1) ELF must have been signed via Sectools using
 *                 UIE_feature_id, anti_rollback_version, and sw_id values
 *                 matching those of feat_id, anti_rollback_version, and 0x2A
 *                 respectively.
 *
 * In:          feat_id: Feature ID to use during signature verification
 *              anti_rollback_version: Anti-Rollback Version to use during
 *                                     signature verification
 *              image_addr: Address of buffer containing signed ELF
 *              image_size: Size of buffer containing signed ELF
 * Out:         void
 * Return:      SUCCESS on success.
 *              Any other error code on failure.
 */
int32_t ipprotector_verify_sig(uint32_t feat_id, uint32_t anti_rollback_version,
                               uint8_t *image_addr, uint32_t image_size);

/*
 * Description: Get L1 Public Key derived from provided Feature ID. The
 *              returned Public Key must be provided to Sectools for
 *              encrypting the ELF.
 *
 * In:          feat_id: Feature ID to use during key derivation
 *              key_out_size: Size of buffer containing public key
 * Out:         key_out_addr: Buffer containing public key
 *
 * Return:      SUCCESS on success.
 *              Any other error code on failure.
 */
int32_t ipprotector_get_public_key(uint32_t feat_id, uint8_t *key_out_addr,
                                   uint32_t key_out_size);

#endif  //__IPPROTECTOR_API_H__
