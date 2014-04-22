#ifndef SECBOOT_RSA_PKCS_H
#define SECBOOT_RSA_PKCS_H

/*===========================================================================

  Function definition for rsa pkcs 1.5 operation

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All rights reserved.
  Qualcomm Technologies, Inc. Confidential and Proprietary.

===========================================================================*/

/*=========================================================================

                            EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.10/securemsm/secboot/src/secboot_rsa_pkcs1_v15.h#1 $
$DateTime: 2018/11/08 03:29:06 $
$Author: pwbldsvc $

when       who                  what, where, why
--------   ----                 ------------------------------------------- 
02/13/17    hw                  initial version

===========================================================================*/
#include "secboot_i.h"

/*----------------------------------------------------------------------------
 * Function Declaration
 * -------------------------------------------------------------------------*/
/**
 * @brief This function verify signature in RSA PKCS 1.5 scheme
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
secboot_error_type secboot_verify_signature_RSA_PKCS1_v15
(
  secboot_context_type* 	        context_ptr,
  const pbl_secx509_pubkey_type*    pkey,
  uint8*                            hash_digest,
  uint32                            hash_algo,
  const pbl_secx509_signature_type* sig_info
);

/**
 * @brief stub function if not supported
 */
secboot_error_type SECBOOT_STUB_FUNC(secboot_verify_signature_RSA_PKCS1_v15)
(
  secboot_context_type*             context_ptr,
  const pbl_secx509_pubkey_type*    pkey,
  uint8*                            hash_digest,
  uint32                            hash_algo,
  const pbl_secx509_signature_type* sig_info
);

#endif
