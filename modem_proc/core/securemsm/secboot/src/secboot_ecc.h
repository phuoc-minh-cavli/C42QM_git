#pragma once
#include "secboot_x509.h"
/*===========================================================================

  Function definition for data structure to allows ECC authentication
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All rights reserved.
  Qualcomm Technologies, Inc. Confidential and Proprietary.

===========================================================================*/

/**
* @brief This function verifies an ECDSA signatures ONLY on APPS
*        MODEM will return FAILURE
*
* @param[in]         pkey                      Pointer to the public key.
*
* @param[in]         hash_digest               Pointer to the message digest
*
* @param[in]         hash_algo                 Hash algorithm
*
* @param[in]         sig_info                  Pointer to the signature info
*
* @return E_SECBOOT_SUCCESS on success (ONLY on APPS, MODEM will return FAILURE). 
*         Appropriate error code on failure.
*
* @dependencies Caller should ensure all pointers and lengths passed in are valid
*
* @sideeffects  None
*
* @see None
*
*/
secboot_error_type secboot_verify_ecdsa_signature
(
  const pbl_secx509_pubkey_type           *pkey,
  const uint8                             *hash_digest,
  uint32                                   hash_algo,
  const pbl_secx509_signature_type        *sig_info
);

/**
* @brief This function initializes crypto memories before ECDSA 
*        operations are performed, Only applicable to APPS
*        side, MODEM will fail
* @return E_SECBOOT_SUCCESS on success (ONLY on APPS, MODEM will return FAILURE). 
*         Appropriate error code on failure.
*
* @sideeffects  None
*
* @see None
*
*/
secboot_error_type secboot_ecdsa_init(void);

/**
* @brief This function deinitializes crypto memories when ECDSA 
*        operations were performed, Only applicable to APPS
*        side, MODEM will fail
* @return E_SECBOOT_SUCCESS on success (ONLY on APPS, MODEM will return FAILURE). 
*         Appropriate error code on failure.
*
* @sideeffects  None
*
* @see None
*
*/
secboot_error_type secboot_ecdsa_deinit(void);
