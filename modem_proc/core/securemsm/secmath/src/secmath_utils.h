/**
  @file secmath_utils.h

  @brief Utility functions for crypto_math.h
*/

/*===========================================================================
   Copyright (c) 2011,2016 Qualcomm Technologies, Inc.
   All rights reserved.
   Qualcomm Technologies, Inc. Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/core.mpss/3.10/securemsm/secmath/src/secmath_utils.h#2 $
  $DateTime: 2018/11/08 03:29:06 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
07/13/16   hw       SP RAM Reduction
05/11/11   vg       Initial Revision

===========================================================================*/


#ifndef SECMATH_UTILS_H
#define SECMATH_UTILS_H

#include "secmath.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define SECMATH_PRESERVE_ENDIANNESS (0x0) /**< preserve data endianness */
#define SECMATH_FLIP_ENDIANNESS (0x1) /**< flip data endianness */
 
SECMATH_ERRNO_ET secmath_hex_str_to_bin(uint8 * buffer, uint32 buflen,
                                const char *str, uint32 * total_len);

SECMATH_ERRNO_ET secmath_bin_to_hex_str(char *str, uint32 len,
                                const uint8 * buf, uint32 buflen);

/** 
 * The function memcpy unsigned binary (in bytes) to buffer. It clears up
 * the buffer before copy. And the caller can define if data endianness should
 * be flipped or not. The functions does not support local switch.
 *
 * @param[in,out] buf           The pointer to buffer that holds the data
 * @param[in] buf_len           The buffer size
 * @param[in] data_ptr          The pointer to data
 * @param[in] data_len          The data size
 * @param[in] flip_endianness   Indicate if data endianness is flipped or not 
 *                              (0: preserve the endianness; non-0: flip the endianness)
 *
 */
SECMATH_ERRNO_ET secmath_memcpy_unsigned_bin(uint8 *  buf, uint32 buf_len,
                                             const uint8 * data_ptr, uint32 data_len,
                                             uint8 flip_endianness);
#ifdef __cplusplus
}
#endif

#endif
