#ifndef SECBOOT_ENV_H
#define SECBOOT_ENV_H

/**
@file secboot_env.h
@brief Define functions or parameters for particular environment
*/

/*===========================================================================
   Copyright (c) 2017 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/core.mpss/3.10/securemsm/secboot/inc/secboot_env.h#1 $
  $DateTime: 2018/11/08 03:29:06 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
02/16/17   hw       Init Version 

===========================================================================*/

#include "comdef.h" // type definition with std library
#include "string.h" // memcpy declared explicitly in PBL (in comdef). But not here
/*----------------------------------------------------------------------------
 * Environment Supporting Preprocessor Definition
 * -------------------------------------------------------------------------*/
#define MEMSET  memset
#define MEMCPY  memcpy
#define MEMCMP  memcmp
#define BLOAD8(x) ((uint32)(*((uint8*)(x)))) 
// note, there is an assumption that the value
// in the uint32 is a byte (<=255). Not sure why the
// boot_util function bStor8 does not check for this and
// actually just OR's the uint32 value with byte*,
// so other bytes would also get affected if the
// uint32 value is greater than 255.
#define BSTOR8(x,y) (*((uint8*)(x))) = (uint8)(y)

#define SECBOOT_PRINT printf

#ifndef PACKED_STRUCT
	#define PACKED_STRUCT struct __attribute__((__packed__)) 
#endif

/*----------------------------------------------------------------------------
 * Preprocessor Definitions for Environment Constants
 * -------------------------------------------------------------------------*/
#define SECBOOT_HASH_DIGEST_BYTE_SIZE SECBOOT_OTP_ROOT_OF_TRUST_BYTE_SIZE
                                             /**< The MAX Digest Size that secboot supports (in bytes). 
                                                     This impacts the size of image hash and certificate hash,
                                                     and the size of root of trust hash provided by caller to
                                                     verify the root certificate. the OTP root of trust is not
                                                     impacted because that is fixed for chipset. */
#define SECBOOT_MAX_CERT_SIZE_IN_BYTE (2048) /**< buffer for certificate */

#define SECBOOT_MAX_KEY_SIZE_IN_BITS (4096) /**< Max public key size supported by secboot (in bits) */
#define SECBOOT_MIN_KEY_SIZE_IN_BITS (2048) /**< minimum cert size to support */

#define SECBOOT_MIN_NUM_CERTS        (2) /**< Minimum number of cert levels in a cert chain*/
#define SECBOOT_MAX_CERT_CHAIN_LEVEL (3) /**< Maximum number of cert levels in a cert chain */
#define SECBOOT_MAX_NUM_ROOT_CERTS   (4) /**< Maximum number of Root Certs for APPS/MODEM */

#define SECBOOT_OTP_ROOT_OF_TRUST_BYTE_SIZE (48) // support sha384 48 bytes

#define BITS_IN_BYTE (8) // the number of bits in one byte
#define WORD_LEN (sizeof(uint32)) // byte size of one word

/** Total number of certs including max root certs */
#define SECBOOT_TOTAL_MAX_CERTS      (SECBOOT_MAX_NUM_ROOT_CERTS + SECBOOT_MAX_CERT_CHAIN_LEVEL - 1)

#define SECBOOT_HANDLE_WORD_SIZE_ENV (0xD0) /**< The word size of context handle required to run
                                                 secboot. The size is various on diff env. */
/*----------------------------------------------------------------------------
 * Preprocessor Definitions for Feature Enablement
 * -------------------------------------------------------------------------*/
#define SECBOOT_SUPPORT_METADATA_FROM_MBN_HEADER (TRUE)

#endif /* SECBOOT_ENV_H */
