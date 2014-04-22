/*==========================================================================

                         S E C U R I T Y    S E R V I C E S

                 S E C U R E   B O O T   A U T H E N T I C A T I O N

                       I N T E R N A L   H E A D E R  F I L E

FILE: secboot_i.h

DESCRIPTION: This is the secure boot internal header file.

 Hierarchy of dependencies in files for the secure boot process

                      secboot.h (this file, the entry point for this API)
                          |
                      secboot.c--------- secboot_i.h (internal, helper
                                                        functions)
                          |
               +------------------------------------+
               |                                    |
  secboot_x509.h/c               secboot_pad_app_modem.c/secboot_pad_sp.c
   ( x509 parser)                (signature padding support)
         |
         |
  secboot_asn1.h/c
   ( asn1 library)

EXTERNALIZED FUNCTIONS
  None

  Copyright (c) 2004,2006,2012-2014,2016-2017 Qualcomm Technologies, Inc.
  All rights reserved.
  Qualcomm Technologies, Inc. Confidential and Proprietary.
==========================================================================*/

/*=========================================================================

                            EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.10/securemsm/secboot/src/secboot_i.h#2 $
$DateTime: 2018/11/08 03:29:06 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ------------------------------------------------
04/13/17   hw      Secboot 3.0 refactoring
07/13/16   hw      Remove SHA1, PKCS 1.5 support, disable exp3
08/05/14   hw      Add QPSI code review fix
05/28/14   hw      Added RoT support
08/28/13   hw      Added retail unlock feature code
03/06/13   vg      Fuse info structure moved to secboot.h header file.
02/10/12   sm      Reverted back to dynamic size check for context type 
05/02/06   AMW     Initial SC2X port.
08/25/04   SJD     Initial revision.  Original SW from the security team.

============================================================================*/
#ifndef SECBOOT_I_H_
#define SECBOOT_I_H_

/*===========================================================================

                              INCLUDE FILES

============================================================================*/
#define BREAKIF(cond)  { if (cond) break; }

#include "secboot.h"
#include "secboot_hw.h"
#include "secboot_env_i.h"
#include "secboot_x509.h"
#include "HALhwio.h"
#include "msmhwioreg.h"
#include "secboot_fuse_info.h" // Used in secboot.c
#include "PrngML.h" // Used in secboot.c
#include "stringl.h" // Used in secboot.c
/*============================================================================
                               Definitions
=============================================================================*/

#define SECBOOT_I_H_SIG_VERS1   // SIL signature to identify the fix only on 64bit Arch, this is used together with
                                // code change to define the 64bit arch only fix
/**
 * @brief Masks for attestation certificates debug enable OU field
 *        This is a 64 bit field, with lower (0:31) bits to indicate
 *        if debugging is to be enabled, and upper (32:63) to indicate
 *        the chip (based on serial number) that the debug cert was
 *        mean for i.e the chip that debugging is to be enabled on
 */
#define SECBOOT_UINT64_HIGH_32BIT_FIELD_MASK     (0xFFFFFFFF00000000ULL)
#define SECBOOT_UINT64_LOW_32BIT_FIELD_MASK     (0x00000000FFFFFFFFULL)
#define SECBOOT_UINT64_LOW_16BIT_FIELD_MASK     (0x000000000000FFFFULL)

/**< Size of Hash Digest. */
#define SECBOOT_HASH_DIGEST_SIZE_SHA256 (32)
#define SECBOOT_HASH_DIGEST_SIZE_SHA384 (48)

/**< get the hash size from hash algorithm, at least sha256 if not recognized */
#define SECBOOT_HASH_LEN(hashtype) \
  ((hashtype) == E_X509_CODE_HASH_SHA256 ? SECBOOT_HASH_DIGEST_SIZE_SHA256 : \
  ((hashtype) == E_X509_CODE_HASH_SHA384 ? SECBOOT_HASH_DIGEST_SIZE_SHA384 : \
                                           SECBOOT_HASH_DIGEST_SIZE_SHA256))

/* default hash algo that secboot library supports, convert from hash size to hash type
   default to SHA256, if hash size is not recognized */
#define SECBOOT_HASH_DEFAULT_ALGO(hashsize) \
  ((hashsize) == SECBOOT_HASH_DIGEST_SIZE_SHA256 ? E_X509_CODE_HASH_SHA256 : \
  ((hashsize) == SECBOOT_HASH_DIGEST_SIZE_SHA384 ? E_X509_CODE_HASH_SHA384 : \
  E_X509_CODE_HASH_SHA256))

/* set magic number to secboot context if it is initialized */
#define CONTEXT_MAGIC_NUMBER 0x50425342

#define SECBOOT_IS_CONTEXT_INITED(context_ptr) \
	(((context_ptr) != NULL) && \
    ((context_ptr)->magic_number == CONTEXT_MAGIC_NUMBER))

#define SECBOOT_LOG_ERR(ctxptr, data1, data2, data3) secboot_log((ctxptr), (uintnt)(data1), (uintnt)(data2), (uintnt)(data3), __LINE__)

/* invoke function in delegation mode and return if provided function is not local.
   if the provided one is already local, just call the local */
#define SECBOOT_DELEGATE_INVOKE(ftbl, invoked_func, local_func, ...) \
  if (((ftbl) != NULL) && ((ftbl)->invoked_func != NULL) && ((ftbl)->invoked_func != local_func)) {\
    return (ftbl)->invoked_func(__VA_ARGS__); \
  }

#ifndef MIN
   #define  MIN( x, y ) ( ((x) < (y)) ? (x) : (y) )
#endif

/*----------------------------------------------------------------------------
 * Internal Data Structure
 * -------------------------------------------------------------------------*/
/* Error log structure to store constant data describing error location */
typedef struct secboot_error_log_type
{
  uint32                   linenum;
  uintnt                   data1;
  uintnt                   data2;
  uintnt                   data3;
} secboot_error_log_type;

/* data type to cache certificate data and cert type */
typedef struct secboot_cert_data_type
{
    uint8*                         cached_buf; /**< cached certificate to verify signature */
	uint32                         cached_buf_size;
    uint32                         cert_size;
	uint32                         cert_type; /**< type of the certificate: ATTEST_CERT_INDEX/CA_CERT_INDEX/ROOT_CERT_INDEX */
	pbl_secx509_cert_type          current_cert_info;
	secx509_metadata_info_type     cert_metadata;  /**< OU fields from cert for authenticating the cert chain and images */
}secboot_cert_data_type;

//Make sure this structure is uint32 aligned
typedef struct secboot_context_type
{
    uint32                         magic_number;
    uint32                         version; //version of secboot release
    secboot_error_log_type         error_log;
    uint32                         code_segment;
    secboot_fuse_info_type         fuses;
	const secboot_metadata_type    *metadata_ptr;  /**< metadata from caller to authenticate
												       the cert chain and images */
	secboot_cert_data_type         cert_data;       /**< certificate data */
    secboot_cm_ftbl_type          *cm_ftbl;        /**< Counter Meature Function Table */
    secboot_crypto_ftbl_type       crypto_ftbl;  /**< Callback Function Table */
	secboot_hw_ftbl_type           secboot_hw_ftbl; /**< Hardware Function Table */
	secboot_ftbl_type              secboot_ftbl_ptr;
}secboot_context_type;

/*----------------------------------------------------------------------------
 * Internal Function Declaration
 * -------------------------------------------------------------------------*/
/**
* @brief This function calculate hmac and update the input hash buffer
*
* @param[in] context_ptr           Pointer to the context 
*
* @param[in] hash_algo             Hash algorithm
*
* @param[in, out] image_hash       Buffer for the image hash
*
* @param[in] image_hash_sz         Size of the image hash
*
* @return E_SECBOOT_SUCCESS on success. Appropriate error code on failure.
*
* @dependencies Previous function verifies input parameters
*
*/
secboot_error_type secboot_cal_qc_hmac
(
  secboot_context_type             *context_ptr,
  uint32                            hash_algo,
  unsigned char                    *image_hash,
  uint32                           image_hash_sz          
);

/**
* @brief This function log debug data
*
* @param[in] *context_ptr	  Pointer to the secboot context
*
* @param[in] data1		  the debug data1
*
* @param[in] data2		  the debug data2
*
* @param[in] data3		  Hash algorithm for certificate
*
* @param[in] linenum		  the code line for debug
*
* @return E_SECBOOT_SUCCESS on success. Appropriate error code on failure.
*
* @dependencies None
*
*/
void secboot_log
(
  secboot_context_type*        context_ptr,
  uintnt data1,
  uintnt data2,
  uintnt data3,
  uint32 linenum
);

/**
 * @brief This function checks to see if the oem_id_independent_disabled
 *        is blowin, which means disabling the oem independent id.
 *
 * @param is_oem_id_indp_disabled [in,out] Pointer to uint32 that will be
 *                                populated with the fuse value.
 *                         
 * @return E_SECBOOT_HW_SUCCESS on success. Appropriate error code on failure.
 *
 * @see Security Control HDD for SECURE_BOOT fuses
 * 
 */
secboot_hw_etype secboot_hw_get_is_oem_id_independent_disabled(
  uint32* is_oem_id_indp_disabled
);

/**
 * @brief This function reads the timestamp and saves it in parameter
 *
 *                                    
 * @param[in,out]  ts   32 byte value which holds the time stamp value
 *
 * @return None  
 *
 * @dependencies None
 * 
 * @sideeffects  None
 *
 */
void secboot_pbl_get_timestamp(uint32 *ts);

/**
* This function returns fingerprint (hash value) of the sub-ca cert 
* for improved secboot performance
*
* @param[in] idx the index of the subca fingerprint table
*
* @return the pointer to const uint8 array of hash value
*
*/
const uint8* SECBOOT_CHIPSET_ROT(secboot_subca_cert_fingerprint)(uint32 idx);

/**
* @brief This function returns the size of the subca fingerprints (hash value)  
*        in the subca fingerprint table
*
* @return the size of subca fingerprint
*
*/
uint32 SECBOOT_CHIPSET_ROT(secboot_subca_cert_fingerprint_size)(void);

/**
* This function returns fingerprint (hash value) of the root cert  
* for improved secboot performance
*
* @param[in] code_segment the code segment
* @param[out] hash_ptr pointer to the root cert hash buffer within the ROM 
*                      (init'ed value = NULL)
* @param[in,out] idx the index to the pk hash table, tracking the position 
*                    with in/out buffer
*
* @return E_SECBOOT_HW_SUCCESS on success. Appropriate error code on failure.
*
*/
secboot_hw_etype SECBOOT_CHIPSET_ROT(secboot_root_cert_fingerprint)
(
  uint32 code_segment,
  const uint8** hash_ptr,
  uint32* idx
);

/**
* @brief This function returns the size of the root fingerprints (hash value)  
*        in the root fingerprint table
*
* @return the size of root fingerprint
*
*/
uint32 SECBOOT_CHIPSET_ROT(secboot_root_cert_fingerprint_size)(void);

/**
* @brief This function returns the number of fingerprints of cert chains 
*
* @return the number of fingerprints
*
*/
uint32 SECBOOT_CHIPSET_ROT(secboot_cert_fingerprints_num)(void);

/* ---------------------------------------------------------------------- */
/*                            MRC Functions                               */
/* ---------------------------------------------------------------------- */

/**
* @brief This function ensures the the selected root cert is
*        active and non-revoked
*
* @param[in]         root_cert_sel    Pointer to the selected
*                                     root cert from the
*                                     attestation cert OU field
*
* @param[in]         revocation_list  Pointer from the fuses to
*                                     the indicies that are
*                                     revoked indicated by a 1
*
* @param[in]         activation_list  Pointer from the fuses
*                                     to the indicies that are
*                                     active indicated by a 1
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
secboot_error_type secboot_verify_root_cert_index
(
uint16 root_cert_sel,
uint32* revocation_list,
uint32* activation_list
);

/**
* @brief This function verifies signatures for public key authentication
*
* @param[in]         context_ptr               Pointer table to secboot context
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
secboot_error_type secboot_verify_signature_legacy
(
  secboot_context_type    *context_ptr,
  const pbl_secx509_pubkey_type *pkey,
  uint8 *hash_digest,
  uint32 hash_algo,
  const pbl_secx509_signature_type *sig_info
);

/**
* @brief This function return the maximum number of root
*        certificates
*
* @return MAX_ROOT_CERTS
*
* @dependencies None
*
* @sideeffects  None
*
* @see None
*
*/
uint32 secboot_get_max_root_certs(void);

/**
 * The function initializes the CE. (the default function defined by the library)
 *        Context memory must be assigned and passed in outside the call.
 *
 * @param[in] ctx       The handle context
 * @param[in] hash_algo The hash algorithm type
 *
 * @return 0 on success, error code E_SECBOOT_CB error type on failure.
 *
 */
uint32 HashInit(crypto_ctx_type* ctx, uint32 hash_algo);

/**
 * The function hashes data into the hash context
 *        structure, which must have been initialized by
 *        HashInit. (the default function defined by the library)
 *
 * @param[in] ctx      The handle context
 * @param[in] data     Pointer to input message to be hashed
 * @param[in] data_len The message byte size
 *
 * @return 0 on success, error code E_SECBOOT_CB error type on failure.
 *
 */
uint32 HashUpdate(crypto_ctx_type* ctx, const void *data, uint64 data_len);

/**
 * The function computes the final digest hash value.
 *         (the default function defined by the library)
 *
 * @param[in]  ctx         The handle context
 * @param[out] digest_ptr  Pointer to digest data
 * @param[in] digest_len   The digest byte size
 *
 * @return 0 on success, error code E_SECBOOT_CB error type on failure.
 *
 */
uint32 HashFinal(crypto_ctx_type* ctx, void* digest_ptr, uint32 digest_len);

/**
 * The function deintializes a cipher context.
 *          (the default function defined by the library)
 *
 * @param[in] ctx        The handle context
 *
 * @return 0 on success, error code E_SECBOOT_CB error type on failure.
 *
 */
uint32 HashDeInit(crypto_ctx_type* ctx);

/**
 * The function decrypts the message in RSA operation with given mod and exp (used within Secboot library).
 *        The mod and exp are sent in big endian for SP PBL, as SP PBL use hardware based RSA.
 *        The hardware based RSA operates in big endian. The APPS and MPSS PBL uses software based RSA,
 *        which works in little endian. (the default function defined by the library)
 *
 * @param[in]  ctx      the handle context
 * @param[in]  mod_size the modsize (byte size), the same as the return value size
 * @param[in]  exp_size the exp size (byte size)
 * @param[in]  mod the ptr to mod
 * @param[in]  message the ptr to message
 * @param[in]  exp the ptr to exp
 * @param[out]  r the ptr to return value
 *
 * @return 0 on success, error code E_SECBOOT_CB error type on failure.
 *
 */
uint32 ModExp 
(
  crypto_ctx_type*     ctx,
  uint32               mod_size,
  uint32               exp_size,
  uint8*               mod,     
  uint8*               message, 
  uint8*               exp,     
  uint8*               r
  );

/**
 * @brief This function reverses the order of the data in buffer.
 *
 * @param[in]         buf_ptr    pointer to the data
 * @param[in/out]     buf_len    len of the data
 *     
 * @return int on 0. non-0 value means failure.
 *
 * @dependencies None
 *
 * @sideeffects  None
 *           
 * @see None
 *
 */
int reverse(uint8* buf_ptr, uint32 buf_len);

#endif

