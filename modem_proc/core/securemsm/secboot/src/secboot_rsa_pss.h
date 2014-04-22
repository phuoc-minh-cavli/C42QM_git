#ifndef SECBOOT_RSA_PSS_H
#define SECBOOT_RSA_PSS_H

/*===========================================================================

  Function declaration for signature verification with rsa pss operation

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All rights reserved.
  Qualcomm Technologies, Inc. Confidential and Proprietary.

===========================================================================*/

/*=========================================================================

                            EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.10/securemsm/secboot/src/secboot_rsa_pss.h#1 $
$DateTime: 2018/11/08 03:29:06 $
$Author: pwbldsvc $

when       who                  what, where, why
--------   ----                 ------------------------------------------- 
02/13/17    hw                  initial version

===========================================================================*/
#include "secboot_i.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
/* max message size for PSS type padding before message is hashed
* size is determined from this expression
* (8 + hash_len + salt_len)
* where hash_len and salt_len equal size of SHA */
#define MAX_PSS_MESSAGE_SIZE 72

/* max mask size used in mgf to determine salt value
* size is determined from this expression
* (em_len - hash_len - 1)
* where em_len equals size of sig and hash_len equals size of SHA */
#define MAX_PSS_MASK_SIZE 491

/* counter size for temp_counter in mgf*/
#define PSS_MGF_COUNTER_SIZE 4

/*----------------------------------------------------------------------------
 * Function Declaration
 * -------------------------------------------------------------------------*/
/**
 * @brief This function verify signature in RSA PSS scheme
 *
 * @param[in]         context_ptr     Pointer to secboot context
 * @param[in]         pkey            Pointer to public key
 * @param[in]         hash_digest     Pointer to hash digest
 *         
 * @param[in]         hash_algo       Hash algorithm
 * @param[in]         sig_info        Signature Information
 *     
 * @return E_SECBOOT_SUCCESS on success. Appropriate error code on failure.
 *
 * @dependencies None
 *
 * @sideeffects  None
 *           
 * @see None
 *
 */
secboot_error_type secboot_verify_signature_RSA_PSS
(
  secboot_context_type*              context_ptr,
  const pbl_secx509_pubkey_type*     pkey,
  const uint8*                       hash_digest,
  uint32                             hash_algo,
  const pbl_secx509_signature_type*  sig_info
);

#endif
