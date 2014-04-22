#ifndef SECBOOT_RSA_PRIM_H
#define SECBOOT_RSA_PRIM_H

/*===========================================================================

  Function declaration for rsa operation

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All rights reserved.
  Qualcomm Technologies, Inc. Confidential and Proprietary.

===========================================================================*/

/*=========================================================================

                            EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.10/securemsm/secboot/src/secboot_rsa_primitive.h#1 $
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
 * @brief This function calculate modulo exponent on signature in LITTLE 
 *        ENDIAN.
 *
 * @param[in]         context_ptr     Pointer to secboot context
 * @param[in]         pkey            Pointer to public key
 * @param[in]         input           Pointer to input data
 * @param[in]         input_len       Size of the input data
 *         
 * @param[in/out]     output          Pointer to output data buffer
 * @param[in]         output_len      Size of the output data
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
secboot_error_type secboot_rsa_primitive
(
  secboot_context_type* 	      context_ptr,
  const pbl_secx509_pubkey_type*  pkey,
  const uint8* 					  input,
  uint32						  input_len,
  uint8*                          output,
  uint32                          output_len
);
#endif
