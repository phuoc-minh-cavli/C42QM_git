#ifndef SECMATH_H
#define SECMATH_H

/**
  @file secmath.h

  @brief This header file defines the interface to all crypto math
         operations.

   The approach to crypto math operations can be broken into two types: provide
   big integer operations and let the caller construct the operations they need
   from such primitives, or make a crypto math library that is tuned for the type
   of operations that the crypto library is performing. This library is performing
   the latter
*/

/*===========================================================================
   Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/core.mpss/3.10/securemsm/secmath/api/secmath.h#2 $
  $DateTime: 2018/11/08 03:29:06 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
05/11/11   vg             Initial Revision

===========================================================================*/

#include "comdef.h"
#include "secmath_mod_exp.h"
#include "secboot.h"

/**
 * @addtogroup SecMath
 * @{
 */
#ifdef __cplusplus
extern "C"
{
#endif

/* version */
#define E_SECMATH   0x0001
#define S_SECMATH   "0.01"

/* error codes */
typedef enum
{
  E_SECMATH_SUCCESS = 0,         ///< Result OK
  E_SECMATH_FAILURE,             ///< Generic Error */
  E_SECMATH_NOP,                 ///< Not a failure but no operation was performed */
  E_SECMATH_FAIL_TESTVECTOR,     ///< Algorithm failed test vectors */
  E_SECMATH_BUFFER_OVERFLOW,     ///< Not enough space for output */
  E_SECMATH_MEM,                 ///< Out of memory */
  E_SECMATH_INVALID_ARG,         ///< Generic invalid argument */
  E_SECMATH_INVALID_PRIME_SIZE,  ///< Invalid size of prime requested */
  E_SECMATH_NOT_SUPPORTED        ///< Operation not supported
} SECMATH_ERRNO_ET;

/** Common input/output type */
typedef struct common_in_out_s
{
  uint8 *ptr;
  uint32 len;
} common_in_out_t;

/* define the endianness that secmath supports and data should be converted to */
typedef enum secmath_endianness
{
  SECMATH_BIG_ENDIAN,
  SECMATH_LITTLE_ENDIAN
} secmath_endianness_t;

typedef uint32 BLONG;

#ifdef SP_BOOT_ROM /* SP_BOOT_ROM flag defined by SP PBL */
  #define MAX_KEY_SIZE_IN_BITS 2048                  ///< Maximum key size in bits for SP
#else
  #define MAX_KEY_SIZE_IN_BITS 4096                  ///< Maximum key size in bits for Non-SP
#endif
#define BLONG_SIZE sizeof(BLONG)                   ///< Bytes per digit
#define BN_BITS_PER_WORD (BLONG_SIZE * 8)          ///< Bit per digit
#define BLONGS_PER_KEY  (MAX_KEY_SIZE_IN_BITS + 8*BLONG_SIZE - 1)/(8*BLONG_SIZE)  ///< Digits per key
#define BLONGS_PER_INT  (1)                        /**< word size for Int data struct */
#define BYTES_TO_BLONG_SHIFT 2                     ///< bytes/sizeof(BLONG)

#define BLONG_MAX_ELEMENTS BLONGS_PER_KEY

/** 
 * Definition of Int data structure. 
 * The Int data struct should have at least one BLONG size buffer.
 */
typedef struct
{
  BLONG a[BLONGS_PER_INT];             /**< Array to assign data buffer */
  int byte_size;                       /**< actually data length within a (in byte size) */
} Int;

/** 
 * Definition of BigInt data structure
 */
typedef struct
{
  BLONG a[BLONGS_PER_KEY];             ///< Make room for multiplication
  int n;                               ///< Current length of a (in word size)
  int byte_size;                       ///< Current length of a (in byte size)
} BigInt;

/**
 * @brief
 *  Read an unsigned buffer of bytes into a big integer
 */
SECMATH_ERRNO_ET secmath_BIGINT_read_unsigned_bin(BigInt * a, const uint8 * buf,
                                                  uint32 len, secmath_endianness_t endian_sel);

 /**
 * The function works like a factory function to initialize Int struct with 
 * pre-assigned buffer. 
 *
 * @ param[in,out] a       Pointer to Int data struct
 * @ param[in] buf         Pointer to the input unsigned binary buffer
 * @ param[in] len         The size of the input unsigned binary buffer
 * @ param[in] endian_sel  The endianness used for the data of Int struct
 *
 * @return SECMATH_ERRNO_ET
 *
 */
SECMATH_ERRNO_ET secmath_INT_read_unsigned_bin(Int * a, const uint8 * buf,
                                               uint32 len, secmath_endianness_t endian_sel);

 /**
 * @brief
 *  Read a zero terminated string into a big integer
 */
SECMATH_ERRNO_ET secmath_BIGINT_read_radix(BigInt * a, const char *str,
                                           uint32 radix);

/**
 * @brief
 *  Write a big integer into NULL terminate string
 */
SECMATH_ERRNO_ET secmath_BIGINT_to_radix(const BigInt * a, char *str, uint32 *len,
                                         uint32 radix);

/**
 * @brief
 *  Write a big integer into a buffer
 */
SECMATH_ERRNO_ET secmath_BIGINT_to_unsigned_bin(const BigInt * a, uint8 * buf,
                                                uint32 size);

/**
 * @brief
 *  r = b^e mod m
 */
SECMATH_ERRNO_ET secmath_BIGINT_modexp(secboot_crypto_ftbl_type *  crypto_ftbl, BigInt * r, BigInt * b, Int * e, BigInt * m);

#ifdef __cplusplus
}
#endif

/// @}

#endif
