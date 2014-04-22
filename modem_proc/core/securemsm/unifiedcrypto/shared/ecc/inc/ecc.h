/*
  ===========================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

  ===========================================================================

  $Header: //components/rel/core.mpss/3.10/securemsm/unifiedcrypto/shared/ecc/inc/ecc.h#1 $
  $DateTime: 2018/11/08 03:29:06 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#ifndef _ECC_H
#define _ECC_H

/* 
 * Define EXPLICIT_INT_TYPES if the environment does not
 * have sys/types.h and, if necessary, stdint.h
 */
#define EXPLICIT_INT_TYPES

#ifdef EXPLICIT_INT_TYPES
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef int int32_t;
typedef long long int64_t;
typedef unsigned long long uint64_t;

/*
typedef int boolean_t;
#define B_FALSE 0
#define B_TRUE 1
#else 
*/
/* EXPLICIT_INT_TYPES not defined */ 
/* On Ubuntu Linux this is necessary */
//#include <stdint.h>
/* On some systems the following is needed. */
typedef enum {B_FALSE, B_TRUE} boolean_t;
#endif /* EXPLICIT_INT_TYPES not defined */

/* 
 * Define either ECC_P256 or ECC_P192 to control which polynomial is used.
 * Of course, the binary must have been compiled with the same .h file
 * contents as are used by applications.
 *
 * Arguably, to avoid branching in version control systems, this
 * should be supplied by a -DP256 or -DP192 at compile time. It would be
 * good to include a run-time check to make sure the
 * library was compiled with the same modulus as the include file
 * specifies, but it seems require that the main "functions" be
 * macros, and I'm not sure I want to do that.
 */

#define ECDSA
#define ECC_P384
//#define ECC_P256

#ifdef ECC_P384
#define BIGLEN 13

#define ECC_KEY_SIZE_IN_BITS 384
#define ECC_MALLOC_KEY_SIZE  (512/8) /*big number next to 384 which is power of 2 */
#endif

#ifdef ECC_P256
#define BIGLEN 9
#endif

#ifdef ECC_P192
#define BIGLEN 7
#endif

#if !defined ECC_P384 && !defined ECC_P256 && !defined ECC_P192 
#error
#endif

#define ECC_KEY_SIZE_IN_BYTES (ECC_KEY_SIZE_IN_BITS/8)
#define ECC_MAX_KEY_SIZE ECC_KEY_SIZE_IN_BYTES

/* 
 * For P256 bigval_t types hold 288-bit 2's complement numbers (9
 * 32-bit words).  For P192 they hold 224-bit 2's complement numbers
 * (7 32-bit words).
 * 
 * The representation is little endian by word and native endian
 * within each word.
 */

typedef struct {
      uint32_t data[BIGLEN];
} bigval_t;


typedef struct {
      bigval_t x;
      bigval_t y;
      boolean_t infinity;
} affine_point_t;


typedef struct {
      bigval_t r;
      bigval_t s;
} ECDSA_sig_t;


/* Define crypto shared data structure */
typedef struct crypto_ecc_shared_info_type
{
   uint32_t shared_data_version;
   int (* ecc_init)(void);
   int (* ecc_deinit)(void);
   boolean_t (* ecc_verify)(bigval_t const *msgdgst,  
                            affine_point_t const *pubkey, 
                            ECDSA_sig_t const *sig);
   void (* ecc_hash_to_bigval)(bigval_t *tgt, 
                               void const *hashp, 
                               unsigned int hashlen);
}crypto_ecc_shared_info_type;



void ECC_hash_to_bigval(bigval_t *tgt, 
                        void const *hashp, 
                        unsigned int hashlen);

boolean_t ECDSA_verify(
		       bigval_t const *msgdgst,  
           affine_point_t const *pubkey, 
           ECDSA_sig_t const *sig);

/* Initializes all the dependency for ECDSA processing such as laoding firmware for PKA.
 * return 0 in case of success, negative number otherwise 
 * Note: This API is preferred to be called only once during the execution and before 
 * using any ECDSA services, to avoid multiple times loading the firmware on to the RAM.*/
int ECDSA_init(void);

/* Deinitializes all the dependency for ECDSA, such as clearing PKA operand memories.
 * return 0 in case of success, negative number otherwise.
 * Note: This API is preferred to be called only once during the execution and after the  
 * usage of ECDSA services, to avoid multiple times writing zeros in the firmware RAM area.*/
int ECDSA_deinit(void);

/**
  Function to share the driver function pointers into the structure.
*/
uint32_t pbl_ecc_share_data(void* ecc_shared_data, uint32_t size);

#endif /* _ECC_H */
