#ifndef SECBOOT_UTIL_H
#define SECBOOT_UTIL_H

/*****************************************************************************
*
* @file  secboot_util.h (Wrapper for Secboot Authentication API)
*
* @brief This header files contains the types and API to authenticate an
*        image based on certificates and signature. 
* 
* @details
* 
* @note  
*     Example usecase for secure EFS tar feature:
*     //Caller should allocate memory (malloc) for the secboot_handle_ptr, before calling into the function
*     //Ensure that secboot_handle_ptr is 8byte aligned, Its a mandatory requirement before calling to function 
*     //and after the function call can deallocate this memory. Here we declared as static, please do malloc and 8byte alignment 
*     static secboot_util_handle_ptr   secboot_handle_ptr;
*     secboot_util_fuse_info_type      fuse_info;
*     secboot_util_verified_info_type  verified_info_ptr;
*     
*     memset(&fuse_info, 0, sizeof(fuse_info));
*     fuse_info.auth_use_serial_num = 0;
*     fuse_info.msm_hw_id = 0x0000000000000000;
*     fuse_info.root_of_trust = root_of_trust_buf;//supply the root of trust here
*     fuse_info.root_of_trust_len = 32;
*     //if we are signing the image with debug enable option then serial number 
*     //has to be supplied.For Secure EFS tar feature this is not applicable.So they can 
*     //give serial number as zero.
*     fuse_info.serial_num = 0x0; 
*     fuse_info.use_root_of_trust_only = FALSE;
*     fuse_info.use_supplied_fuses = TRUE;
*     
*     
*     ret_value = secboot_util_auth_img_verify
*                 (
*                   &secboot_handle_ptr,
*                   signed_img_ptr,
*                   signed_img_len,  
*                   SECBOOT_UTIL_HW_MSS_CODE_SEGMENT,
*                   0x0000000000000000, //This needs to be the value with which image is signed.
*                   &fuse_info,
*                   &verified_info_ptr
*                 );
*     
*     if(ret_value == FALSE)
*     {
*       //authentication failure.Dont execute/use the supplied image.
*     }
*     
*     Example use case for binary modem feature:
*     uint32 auth_enabled;
*     
*     ret_value = secboot_util_hw_is_auth_enabled(SECBOOT_UTIL_HW_MSS_CODE_SEGMENT, &auth_enabled);
*     if(ret_value != E_SUCCESS)
*      return E_FAILURE;
*     
*     //If authentication is enbaled then call secboot_util_auth_img_verify() to authenticate the image.
*     if(auth_enabled)
*     {
*       memset(&fuse_info, 0, sizeof(fuse_info));
*       fuse_info.use_supplied_fuses = FALSE;  
*       
*       ret_value = secboot_util_auth_img_verify
*                   (
*                     &secboot_handle_ptr,
*                     signed_img_ptr,
*                     signed_img_len,  
*                     SECBOOT_UTIL_HW_MSS_CODE_SEGMENT,
*                     0x0000000000000002, //Upper 32 bits contains version of the image 
*                                         // which has to be read from corresponding fuses if allocated
*                                         // for the image.Lower 32 bit contains sw type.
*                                         // For modem subsytem 0x2 is allocated.
*                     NULL,
*                     &verified_info_ptr
*                  );
*       
*       if(ret_value == FALSE)
*       {
*          //authentication failure.Dont execute/use the supplied image
*       }
*     }
* 
*    Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
*    Qualcomm Technologies Proprietary and Confidential.
*
*****************************************************************************/


/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:$
$Header: //components/rel/core.mpss/3.10/api/securemsm/secboot/secboot_util.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/19/2012 vg       Initial API - based on old api with some changes
============================================================================*/

/** @ingroup SecureMSM
 *  @{
 */

/** @defgroup SecbootAuthentication Secboot Authentication Library */

/**
 * @addtogroup SecbootAuthentication
 *
 * @{
 */


/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/

#include "comdef.h"
#include "secboot.h"

/*===========================================================================
                      PUBLIC DATA DECLARATIONS
===========================================================================*/
#define SECBOOT_ROOT_OF_TRUST_BYTE_SIZE 48

/* Max number or SW_IDs list supported for deferred oem */
#define MAX_NUM_SW_IDS 15

/**
 * Error codes specific to secboot.
 */
typedef enum secboot_util_error_type
{
  E_SECBOOT_UTIL_SUCCESS                = 0x2ECB770,   /**< Operation was successful. */
  E_SECBOOT_UTIL_FAILURE                = 1,   /**< General failure. */
  E_SECBOOT_UTIL_INVALID_PARAM          = 2,   /**< Invalid parameter passed into function. */
  E_SECBOOT_UTIL_INVALID_DATA           = 3,   /**< Data is invalid. */
  E_SECBOOT_UTIL_UNSUPPORTED            = 4,   /**< Secctrl command option not supported. */
  E_SECBOOT_UTIL_RSA_FAIL               = 5,   /**< Failure occured for RSA. */
  E_SECBOOT_UTIL_HASH_FAIL              = 6,   /**< Failure occured for hash. */
  E_SECBOOT_UTIL_HW_FAIL                = 7,   /**< Failure occured for HW. */
  E_SECBOOT_UTIL_X509_FAIL              = 8,   /**< Failure occured during cert chain parsing. */
  E_SECBOOT_UTIL_INVALID_CERT           = 9,   /**< Cert chain validation checks failed. */
  E_SECBOOT_UTIL_INVALID_CERT_SIG       = 10,  /**< Cert chain signature validation failed. */
  E_SECBOOT_UTIL_UNTRUSTED_ROOT         = 11,   /**< Root certificate is not the root of trust. */
  E_SECBOOT_UTIL_INVALID_IMAGE_SIG      = 12,  /**< Invalid image signature. */
  E_SECBOOT_UTIL_INVALID_SW_TYPE        = 13,  /**< Unexpected software type. */
  E_SECBOOT_UTIL_INVALID_SW_VERSION     = 14,  /**< Image has been signed with an older version */
  E_SECBOOT_UTIL_INVALID_MSM_HW_ID      = 15,  /**< Image has been signed for a different hw id */
  E_SECBOOT_UTIL_INVALID_DEBUG          = 16,  /**< Invalid debug cert */
  E_SECBOOT_UTIL_INIT                   = 17,  /**< Initialization failed */
  E_SECBOOT_UTIL_DEINIT                 = 18,  /**< De-initialization failed */
  E_SECBOOT_UTIL_INVALID_ROOT_SEL       = 19,  /**< Root selection failed, selected root index isn't active/non-revoked */
  E_SECBOOT_UTIL_INVALID_CRASH_DUMP     = 20,  /**< Invalid crash dump cert */
  E_SECBOOT_UTIL_ECC_FAIL               = 21,  /**< Failure occured for ECC. */
  /* Error 22-23 are for ROT Transfer, not supported */
  E_SECBOOT_UTIL_INVALID_IN_USE_SOC_HW_VER_OPTION = 23,  /**< Invalid soc hw version option value */
  E_SECBOOT_UTIL_INVALID_OU_USE_SERIAL_NUMBER_OPTION = 24,  /**< Invalid OU field option from the cert */
  E_SECBOOT_UTIL_INVALID_SERIAL_NUM     = 25,  /**< Invalid serial number */
  E_SECBOOT_UTIL_INVALID_OU_OEM_ID_INDP_OPTION = 26,  /**< Invalid OU field option from the cert */
  E_SECBOOT_UTIL_SERIAL_NUM_BINDING_FAIL = 27,  /**< The signing is expected bound to serial number, but
                                                valid serial number is not found in signing. */
  E_SECBOOT_UTIL_INVALID_OU_UIE_KEY_SWITCH = 28,  /**< Invalid uie key switch enablement OU field */
  E_SECBOOT_UTIL_INVALID_OU_ROOT_REVOKE_ACTIVATE_ENABLE = 29,  /**< Invalid ROOT_REVOKE_ACTIVATE_ENABLE OU field */
  E_SECBOOT_UTIL_INIT_CM_INFO_FAIL      = 31,  /**< Fail to initialize counter measure info. */
  E_SECBOOT_UTIL_INIT_CRYPTO_BUF_NOT_PROVIDED = 32,  /**< crypto run-time buffer is not provided */
  E_SECBOOT_UTIL_KPI_OPTIM_FAILURE      = 33,  /**< kpi optimization failure */
  E_SECBOOT_UTIL_INVALID_SIGNATURE      = 34,  /**< Generic signature validation failed. */
  E_SECBOOT_UTIL_ROOT_CERT_NOT_INSTALLED = 35,  /**< root certificate is not provisioned */
  E_SECBOOT_UTIL_SUBCA_CERT_NOT_INSTALLED = 36, /**< subca certificate is not provisioned */
  E_SECBOOT_UTIL_ATTEST_CERT_NOT_INSTALLED = 37, /**< subca certificate is not provisioned */
  E_SECBOOT_UTIL_UNSUPPORTED_SIGN_ALGO  = 38, /**< unsupport signing algorithm */
  E_SECBOOT_UTIL_INVALID_CERT_DATA      = 39, /**< invalid certificate data */
  E_SECBOOT_UTIL_NO_CERT_CACHE_BUF      = 40, /**< cert cache buffer not enough, single cert verification is disabled */
  E_SECBOOT_UTIL_CTX_NOT_INITED         = 41, /**< secboot context is not initialized */ 
  E_SECBOOT_UTIL_METADATA_NOT_PROVIDED  = 42, /**< no metadata for usage */ 
  E_SECBOOT_UTIL_METADATA_VERSION_NOT_SUPPORT  = 43, /**< meta data version is not supported */ 
  E_SECBOOT_UTIL_INVALID_OEM_ID         = 44,
  E_SECBOOT_UTIL_INVALID_MODEL_ID       = 45,
  E_SECBOOT_UTIL_INVALID_MBN_HEADER     = 46,  
  E_SECBOOT_UTIL_MAX                    = 0x7FFFFFFF /**< Force to 32 bits */
} secboot_util_error_type;

/**
 * @brief Secboot handle
 */
typedef struct secboot_util_handle_type
{
    uint32 handle[208]; /* Size of handle required to run secboot */
}secboot_util_handle_type;


/*----------------------------------------------------------------------------
 * Data Structure Definition
 * -------------------------------------------------------------------------*/
/**
 * MRC fuse information. This is part of secboot_fuse_info_type.
 */
typedef struct secboot_util_mrc_fuse_data_type
{
  uint32   is_root_sel_enabled;  /**< Is Root Cert Selection enabled  */
  uint32   num_root_certs;       /**< valid only if root cert selection is enabled */
  uint32   revocation_list; /**< list of revoked root cert indicies */
  uint32   activation_list; /**< list of active root cert indicies */
} secboot_util_mrc_fuse_data_type; 

/**
 * @brief Structure to store fuses related information.
 */

typedef struct secboot_util_fuse_info_type
{
  uint8        use_supplied_info; /**< User supplied fuse information */
  uint8        root_of_trust[SECBOOT_ROOT_OF_TRUST_BYTE_SIZE]; /**< sha384 hash of the root certificate */
  uint64       msm_hw_id;              /**< Fuse to the device's hw id */
  uint32       auth_use_serial_num;    /**< Use serial number in device's hw id */
  uint32       serial_num;             /**< Fuse to device's serial number */
  uint32       use_root_of_trust_only; /**< Caller sets this variable to TRUE if 
                                            secboot needs to use only root of trust from the 
                                            supplied fuses */
  secboot_util_mrc_fuse_data_type    mrc_fuse_info; /**< contains mrc specific values */
  uint32       use_soc_hw_version;     /**< Use Soc Hw Version in device's hw id */
  uint32       soc_hw_version;         /**< The fuse value of family number and 
                                                       device number of the soc hw version */
  uint32       oem_id_indp_disabled;   /**< Fuse to disable oem independent id */
  uint32       auth_use_oem_id;        /**< Enforce the usage of oem_id and model_id provided already
                                      as part of msm_hw_id. No change could be made to msm_hw_id
                                      to replace the oem_id and model_id */
  uint32       rsa_disable; /**< Variable for DISABLE_RSA fuse, 
                                 if 0x0 RSA is enabled,
                                 if 0x1 RSA is disabled */
  uint32       debug_disable_in_rom; /**< Variable for DEBUG_DISABLE_IN_ROM fuse, 
                                          if 0x0 debug can be re-enabled,
                                          if 0x1 debug cannot be re-enabled */
  uint32       oem_id;                 /**< OEM ID */
  uint32       model_id;               /**< Model ID */
  uint32       hw_id;                  /**< JTAG ID */
  uint32       rot_algo;               /**< Hash Algo for root of trust */
  uint32       auth_use_legacy_sig;    /**< Calculate signature with legacy sig format */
} secboot_util_fuse_info_type;

/**
 * @brief Data returned from a successful authentication.
 */
typedef struct secboot_util_verified_info_type
{
  uint64       sw_id;        /**< The software id (upper 32 bits:version, lower 32 bits:type)
                                  the image was signed with */
  uint32       img_offset;
  uint32       img_len;
} secboot_util_verified_info_type;

/* MBNv6 structure format */
typedef struct
{
    uint32 res1;                /* Reserved for compatibility: was image_id */
    uint32 version;             /* Reserved for compatibility: was header_vsn_num */
    uint32 qc_signature_size;   /* Reserved for compatibility: was image_src */
    uint32 qc_cert_chain_size;  /* Reserved for compatibility: was image_dest_ptr */
    uint32 image_size;          /* Size of complete hash segment in bytes */
    uint32 code_size;           /* Size of hash table in bytes */
    uint32 res5;                /* Reserved for compatibility: was signature_ptr */
    uint32 oem_signature_size;  /* Size of the attestation signature in bytes */
    uint32 res6;                /* Reserved for compatibility: was cert_chain_ptr */
    uint32 oem_cert_chain_size; /* Size of the attestation chain in bytes */
    uint32 qc_metadata_size;
    uint32 oem_metadata_size;
} sec_img_mbn_header_t;


/* Structure for holding the PK_Hash for oem deferred authentication */

typedef struct {
  uint8 root_of_trust[SECBOOT_ROOT_OF_TRUST_BYTE_SIZE];
} SecbootRootOfTrustType;

/**
 * @brief Length of a SHA256 hash.
 */
#define SHA256_HASH_LENGTH 32

/**
 * @brief Length of a SHA384 hash.
 */
#define SHA384_HASH_LENGTH 48

/**
 Identifies the secure boot fuses which represent the
 authentication information for the code.
 */
#define SECBOOT_UTIL_HW_APPS_CODE_SEGMENT   1 /**< Code segment in SECURE_BOOTn register */
                                              /**< representing authentication information. */
                                              /**< for the application processors images */     
#define SECBOOT_UTIL_HW_MBA_CODE_SEGMENT    2 /**< Code segment in SECURE_BOOTn register */
                                              /**< representing authentication information. */
                                              /**< for the Modem Boot Authentication (MBA) image */     
#define SECBOOT_UTIL_HW_MSS_CODE_SEGMENT    3 /**< Code segment in SECURE_BOOTn register */
                                              /**< representing authentication information. */
                                              /**< for the modem image */     


/**
 * @brief This function authenticates an image.
 *
 * @param[in,out] secboot_handle_ptr Pointer to the secboot handle. The handle
 *                                   must be allocated in protected memory
 *                                   Ensure that secboot_handle_ptr is 8byte aligned, 
 *                                   Its a mandatory requirement before calling to function
 * @param[in]     signed_img_ptr     Pointer to signed image.
 * @param[in]     signed_img_len     Length of the signed image.
 * @param[in]     code_segment       Code segment in SECURE_BOOTn fuse register
 *                                   containing authentication settings for
 *                                   the image.Length of the signed image
 * @param[in]     sw_id              The software id (upper 32 bits:version, lower 32 bits:type)
 *                                   the image was signed with. 
 * @param[in]     fuse_info_ptr      Contains fuse information.Caller can pass as NULL
 *                                   if they dont need to pass fuse info.
 * @param[in,out] verified_info_ptr  Pointer to data that was processed during
 *                                   authentication. Caller can use this data
 *                                   if the function returned success.Caller can pass
 *                                   NULL if they dont need this info.
 * 
 * @return E_SECBOOT_SUCCESS on success and Error Code on Failure.
 *
 * @dependencies Call this function if the secboot fuse is blown.
 *
 * @sideeffects  none  
 *
 * @see none
 *
 */
secboot_util_error_type secboot_util_auth_img_verify
(
  secboot_util_handle_type            *secboot_handle_ptr,
  const uint8                         *signed_img_ptr,
  uint32                               signed_img_len,  
  uint32                               code_segment,
  uint64                               sw_id,  
  const secboot_util_fuse_info_type   *fuse_info_ptr,
  secboot_util_verified_info_type     *verified_info_ptr
);


/**
 * @brief This function checks if the image associated with the code segment
 *        needs to be authenticated. If authentication is required, callers
 *        MUST authenticate the image successfully before allowing it to execute.
 *
 * @param[in]     code_segment       Code segment in SECURE_BOOTn register
 *                                   containing authentication information
 *                                   of the image.
 * @param[in,out] auth_enabled_ptr   Pointer to a uint32 indicating whether
 *                                   authentication is required. Will be
 *                                   populated to 0 if authentication
 *                                   is not required, 1 if authentication
 *                                   is required.
 *         
 * @return E_SECBOOT_HW_SUCCESS on success. Appropriate error code on failure. 
 *         Caller's must NOT allow execution to continue if a failure is returned.
 *
 * @dependencies None
 *
 * @sideeffects  None
 *
 * @see Security Control HDD and SWI for SECURE_BOOT fuses
 *
 */
uint32 secboot_util_hw_is_auth_enabled
(
  uint32    code_segment,
  uint32   *auth_enabled_ptr
);


/**
* @brief This function updates the fuse information for authenticating on a
* non-secure device. 
*
* @param[in,out]      fuse_info           Fuse info pointer where the information has to
*                                     be updated
*    
* @return E_SECBOOT_SUCCESS on success. On failure, send the appropriate error code
*/
secboot_util_error_type secboot_util_update_fuse_info
(
   secboot_util_fuse_info_type * fuse_info
);

/// @}
//
/// @}
#endif /* SECBOOT_UTIL_H */

