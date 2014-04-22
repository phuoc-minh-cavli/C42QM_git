#ifndef SECBOOT_H
#define SECBOOT_H

/*****************************************************************************
*
* @file  secboot.h (Secboot Authentication API)
*
* @brief This header files contains the types and API to authenticate an
*        image based on certificates and signature. 
* 
* @details
* 
* @note  Example usage:
*          secboot_handle_type            secboot_handle; //try and put this as a static global, as it is
*                                                         //is 512 bytes
* 
*          secboot_image_info_type        image_info;
*          uint32 auth_enabled = 0;
* 
*          if (E_SECBOOT_HW_SUCCESS != secboot_hw_is_auth_enabled(SECBOOT_HW_APPS_CODE_SEGMENT, &auth_enabled))
*             return FAILURE 
* 
*          if (auth_enabled == 1) //Authentication is enabled for this image. Image MUST be authenticated successfully
*          {
*              image_info.header_ptr_1    = header_buf;
*              image_info.header_len_1    = header_len;
*              image_info.code_ptr_1      = code_buf;
*              image_info.code_len_1      = code_len;
*              image_info.x509_chain_ptr  = chain_buf;
*              image_info.x509_chain_len  = chain_len;
*              image_info.signature_ptr   = sig_buf;
*              image_info.signature_len   = sig_len;
*              image_info.sw_type         = TZ_SW_SIGN_TYPE; //Type identifier for TZ images during signing
*              image_info.sw_version      = NumOfOneBitsIn(TZ_ANTI_REPLAY_FUSES);
* 
*              if (E_SECBOOT_SUCCESS != secboot_init(&secboot_handle, &crypto_hash_ftbl, NULL or secboot ftbl))
*                  return FAILURE
* 
*              if (E_SECBOOT_SUCCESS != secboot_authenticate(&secboot_handle,
*                                                            SECBOOT_HW_APPS_CODE_SEGMENT,
*                                                            &image_info, verified_info))
*              {
*                  secboot_deinit(secboot_handle);
*                  return FAILURE;
*              }
*  
*              secboot_deinit(&secboot_handle);
*              return SUCCESS;
*          }
* 
* Copyright (c) 2011,2013-2015,2017 Qualcomm Technologies, Inc.
* All rights reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*
*****************************************************************************/


/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:$
$Header: //components/rel/core.mpss/3.10/securemsm/secboot/inc/secboot.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/28/2017 hw       refactor for secboot 3.0
06/09/2015 mm       Add support for SOC_HW_VERSION and more
08/05/2014 hw       Add QPSI code review fix
05/28/2014 hw       Add RoT support
08/28/2013 hw       Added retail unlock feature code
03/06/2013 vg       Added a function to take the fuse values.
08/25/2011 sm       Initial API - based on old api with some changes
============================================================================*/


/**
 * @addtogroup SecbootAuthenticationMSM
 *
 * @{
 */


/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/

#include "secboot_env.h"
#include "secboot_swid.h"

/*===========================================================================
                      PUBLIC DATA DECLARATIONS
===========================================================================*/
/*----------------------------------------------------------------------------
 * Secboot Parm Define (define Macros used by the caller)
 * -------------------------------------------------------------------------*/
/**
 * Error codes specific to secboot.
 */
typedef enum secboot_error_type
{
  E_SECBOOT_SUCCESS                = 0x2ECB770,   /**< Operation was successful. */
  E_SECBOOT_FAILURE                = 1,   /**< General failure. */
  E_SECBOOT_INVALID_PARAM          = 2,   /**< Invalid parameter passed into function. */
  E_SECBOOT_INVALID_DATA           = 3,   /**< Data is invalid. */
  E_SECBOOT_UNSUPPORTED            = 4,   /**< Secctrl command option not supported. */
  E_SECBOOT_RSA_FAIL               = 5,   /**< Failure occured for RSA. */
  E_SECBOOT_HASH_FAIL              = 6,   /**< Failure occured for hash. */
  E_SECBOOT_HW_FAIL                = 7,   /**< Failure occured for HW. */
  E_SECBOOT_X509_FAIL              = 8,   /**< Failure occured during cert chain parsing. */
  E_SECBOOT_INVALID_CERT           = 9,   /**< Cert chain validation checks failed. */
  E_SECBOOT_INVALID_CERT_SIG       = 10,  /**< Cert chain signature validation failed. */
  E_SECBOOT_UNTRUSTED_ROOT         = 11,   /**< Root certificate is not the root of trust. */
  E_SECBOOT_INVALID_IMAGE_SIG      = 12,  /**< Invalid image signature. */
  E_SECBOOT_INVALID_SW_TYPE        = 13,  /**< Unexpected software type. */
  E_SECBOOT_INVALID_SW_VERSION     = 14,  /**< Image has been signed with an older version */
  E_SECBOOT_INVALID_MSM_HW_ID      = 15,  /**< Image has been signed for a different hw id */
  E_SECBOOT_INVALID_DEBUG          = 16,  /**< Invalid debug cert */
  E_SECBOOT_INIT                   = 17,  /**< Initialization failed */
  E_SECBOOT_DEINIT                 = 18,  /**< De-initialization failed */
  E_SECBOOT_INVALID_ROOT_SEL       = 19,  /**< Root selection failed, selected root index isn't active/non-revoked */
  E_SECBOOT_INVALID_CRASH_DUMP     = 20,  /**< Invalid crash dump cert */
  E_SECBOOT_ECC_FAIL               = 21,  /**< Failure occured for ECC. */
  /* Error 22-23 are for ROT Transfer, not supported */
  E_SECBOOT_INVALID_IN_USE_SOC_HW_VER_OPTION = 23,  /**< Invalid soc hw version option value */
  E_SECBOOT_INVALID_OU_USE_SERIAL_NUMBER_OPTION = 24,  /**< Invalid OU field option from the cert */
  E_SECBOOT_INVALID_SERIAL_NUM     = 25,  /**< Invalid serial number */
  E_SECBOOT_INVALID_OU_OEM_ID_INDP_OPTION = 26,  /**< Invalid OU field option from the cert */
  E_SECBOOT_SERIAL_NUM_BINDING_FAIL = 27,  /**< The signing is expected bound to serial number, but
                                                valid serial number is not found in signing. */
  E_SECBOOT_INVALID_OU_UIE_KEY_SWITCH = 28,  /**< Invalid uie key switch enablement OU field */
  E_SECBOOT_INVALID_OU_ROOT_REVOKE_ACTIVATE_ENABLE = 29,  /**< Invalid ROOT_REVOKE_ACTIVATE_ENABLE OU field */
  E_SECBOOT_INIT_CM_INFO_FAIL      = 31,  /**< Fail to initialize counter measure info. */
  E_SECBOOT_INIT_CRYPTO_BUF_NOT_PROVIDED = 32,  /**< crypto run-time buffer is not provided */
  E_SECBOOT_KPI_OPTIM_FAILURE      = 33,  /**< kpi optimization failure */
  E_SECBOOT_INVALID_SIGNATURE      = 34,  /**< Generic signature validation failed. */
  E_SECBOOT_ROOT_CERT_NOT_INSTALLED = 35,  /**< root certificate is not provisioned */
  E_SECBOOT_SUBCA_CERT_NOT_INSTALLED = 36, /**< subca certificate is not provisioned */
  E_SECBOOT_ATTEST_CERT_NOT_INSTALLED = 37, /**< subca certificate is not provisioned */
  E_SECBOOT_UNSUPPORTED_SIGN_ALGO  = 38, /**< unsupport signing algorithm */
  E_SECBOOT_INVALID_CERT_DATA      = 39, /**< invalid certificate data */
  E_SECBOOT_NO_CERT_CACHE_BUF      = 40, /**< cert cache buffer not enough, single cert verification is disabled */
  E_SECBOOT_CTX_NOT_INITED         = 41, /**< secboot context is not initialized */ 
  E_SECBOOT_METADATA_NOT_PROVIDED  = 42, /**< no metadata for usage */ 
  E_SECBOOT_METADATA_VERSION_NOT_SUPPORT  = 43, /**< meta data version is not supported */ 
  E_SECBOOT_INVALID_OEM_ID         = 44,
  E_SECBOOT_INVALID_MODEL_ID       = 45,
  E_SECBOOT_INVALID_MBN_HEADER     = 46,  
  E_SECBOOT_MAX                    = 0x7FFFFFFF /**< Force to 32 bits */
} secboot_error_type;

 /** @name Secboot Constants
 @{ */
#define SECBOOT_SHARED_API_VERSION 0x2ECB0001 //0x2ECB00RR - RR:secboot callback release version

#define SECBOOT_CRYPTO_CB_VERSION 0xCA110001 //0xCA1100RR - RR:secboot callback release version

#define SECBOOT_CM_CB_VERSION 0xC0EA0001 //0xC0EA00RR - RR:secboot callback release version

/** Secboot handle context buffer size (4 byte alignment) = size of internal required context/4, 
    used to allocate context buffer */
#define SECBOOT_CONTEXT_HANDLE_SIZE (SECBOOT_HANDLE_WORD_SIZE_ENV)

/** Define cert caching buffer */
#define SECBOOT_CERT_CACHE_BUF_SIZE (SECBOOT_MAX_CERT_SIZE_IN_BYTE)

/** Maximum Allowed Cert Chain Size - the caller verifies cert chain size in elf hash segment header. */
#define SECBOOT_MAX_CERT_CHAIN_SIZE secboot_get_max_cert_chain_size()

/**
 * Secboot Code Segments (Identifies the secure boot fuses which represent the
 * authentication information for the code.)
 */
#define SECBOOT_HW_APPS_CODE_SEGMENT   1 /**< Code segment in SECURE_BOOTn register 
                                              representing authentication information 
                                              for the application processors images */     
#define SECBOOT_HW_MBA_CODE_SEGMENT    2 /**< Code segment in SECURE_BOOTn register 
                                              representing authentication information 
                                              for the Modem Boot Authentication (MBA) image */     
#define SECBOOT_HW_MSS_CODE_SEGMENT    3 /**< Code segment in SECURE_BOOTn register 
                                              representing authentication information 
                                              for the modem image */

/**
 * Secboot Sec Code Segments (for QTI signing)
 */
#define SECBOOT_HW_SP_BOOT_CODE_SEGMENT 0x80 /**< Code segment in SECURE_BOOTn register 
                                                  representing authentication information 
                                                  for the secure processor image */     
#define SECBOOT_APPS_SEC_CODE_SEGMENT 0x40 /**< Code Segment in Secure Boot for
                                                Secure World code */
#define SECBOOT_HW_QSEE_ROOT_CODE_SEGMENT 0x43 /**< Code Segment in Secure Boot for
                                                    Image using qsee production root for signing */
#define SECBOOT_HW_QSEE_TEST_ROOT_CODE_SEGMENT 0x44 /**< Code Segment in Secure Boot for
                                                         Image using qsee test root for signing */
#define SECBOOT_DEFAULT_CODE_SEGMENT 0x49 /**< Default Code Segment in Secure Boot */

/**
 * This control command type define what operation the secboot_ctrl() function can perform.
 * The caller provides secboot context, command type and parameters.
 */
#define SECBOOT_CTRL_ENABLE_SINGLE_CERT_AUTH_WITH_BUF (0xB0800) /**< enable single cert auth feature with buffer */
#define SECBOOT_CTRL_ADD_ROOT_CERT_PUBKEY       (0xB0801) /**< add and verify root certificate */
#define SECBOOT_CTRL_ADD_SUBCA_CERT_PUBKEY      (0xB0802) /**< add and verify subca certificate */
#define SECBOOT_CTRL_ADD_ATTEST_CERT_PUBKEY     (0xB0803) /**< add and verify attest certificate */
#define SECBOOT_CTRL_VALIDATE_METADATA          (0xB0A01) /**< add and verify metadata
                                                               - param1 is secboot_api_verify_metadata_type and param2 is the size */
/**
 * These control commands type define the APIs to set authentication information or feature flag
 * The caller provides secboot context, command type and parameters.
 */
#define SECBOOT_CTRL_AUTH_RESET_FUSE_INFO       (0xB0F00) /**< reset/clearup provided fuse info within context,
                                                               - param1 and param2 are set to 0 */
#define SECBOOT_CTRL_AUTH_SET_MSM_HW_ID         (0xB0F01) /**< set fuse info: msm hw id,
                                                               - param1 is msm_hw_id and param2 is set to 0 */
#define SECBOOT_CTRL_AUTH_SET_USE_SERIAL_NUM    (0xB0F02) /**< set fuse info: use_serial_num
                                                               - param1 is use_serial_num and param2 is set to 0 */
#define SECBOOT_CTRL_AUTH_SET_USE_OEM_ID        (0xB0F03) /**< set fuse info: use_oem_id
                                                               - param1 is use_oem_id and param2 is set to 0 */
#define SECBOOT_CTRL_AUTH_SET_SERIAL_NUM        (0xB0F04) /**< set fuse info: serial number
                                                               - param1 is serial number and param2 is set to 0 */
#define SECBOOT_CTRL_AUTH_SET_SOC_HW_VERS       (0xB0F05) /**< set fuse info: soc hw version
                                                               - param1 is soc hw version and param2 is set to 0 */
#define SECBOOT_CTRL_AUTH_SET_ROOT_OF_TRUST     (0xB0F06) /**< set fuse info: root of trust
                                                               - param1 is root of trust buffera and param2 is buffer size
                                                               or param1 = Secboot_Hardware_Code_Segment and param2 is 0
                                                               to read from hw fuse. */
#define SECBOOT_CTRL_AUTH_SET_MRC_FUSE_INFO     (0xB0F07) /**< set fuse info: MRC fuse info
                                                               - param1 is pointer to secboot_mrc_fuse_info_type, param2
                                                               is sizeof(secboot_mrc_fuse_info_type)*/
#define SECBOOT_CTRL_AUTH_SET_USE_SUPPLIED_FUSE_INFO (0xB0F08) /**< set fuse info: use caller provided fuse info
                                                               - param1 is use_supplied_fuse_info flag and param2 is set to 0 */
#define SECBOOT_CTRL_AUTH_SET_USE_ONLY_SUPPLIED_ROT  (0xB0F09) /**< set fuse info: only use root of trust 
                                                               - param1 is use_root_of_trust_only flag and param2 is set to 0 */
#define SECBOOT_CTRL_AUTH_DISABLE_OEM_ID_INDEPENDENT (0xB0F0A) /**< set fuse info: disable oem_id_independent 
                                                               - param1 is oem_id_indp_disabled flag and param2 is set to 0 */
#define SECBOOT_CTRL_AUTH_SET_METADATA               (0xB0F0B) /**< set auth info: metadata is required to verify the cert chain 
                                                               - param1 is the address to metadata buffer and param2 is buffer size */
#define SECBOOT_CTRL_AUTH_SET_RSA_DISABLE            (0xB0F0C) /**< set fuse info: rsa_disable (fuse applies only on OEM signing) 
                                                               - param1 is rsa_disable flag and param2 is set to 0 */
#define SECBOOT_CTRL_AUTH_SET_OEM_ID                 (0xB0F0D) /* set fuse info: oem_id
                                                               - param1 is oem_id flag and param2 is set to 0 */
#define SECBOOT_CTRL_AUTH_SET_MODEL_ID               (0xB0F0E) /* set fuse info: model_id
                                                               - param1 is model_id flag and param2 is set to 0 */

/**
 * These control commands type define the APIs to retrieve the internal status of secboot handle.
 * (Some commands may not need secboot context)
 */
#define SECBOOT_CTRL_AUTH_GET_CRYPTO_FTBL            (0xB0F51) /* get crypto ftbl used by the secboot handle (return default, 
                                                                  if input secboot handle is not provided, NULL).
                                                                  - param1 is crypto ftbl buffer and param2 is buffer size */

/**
 * These control commands type define the APIs to read hardware fuse information
 * Some commands may not need secboot context.
 */
#define SECBOOT_CTRL_HW_API_GET_IS_AUTH_ENABLED   (0xB0FA1) /**< get fuse info: auth_en fuse. 
                                                                 - secboot context is required
                                                                 - param1 is pointer to uint32 and param2 is code_segment */
#define SECBOOT_CTRL_HW_API_GET_ROOT_OF_TRUST     (0xB0FA2) /**< get fuse info: OTP root of trust.
                                                                 - secboot context is NOT required
																 - param1 is secboot_hw_api_rot_data buffer and param2 is buffer size */
#define SECBOOT_CTRL_HW_API_GET_MSM_HW_ID         (0xB0FA3) /**< get fuse info: msm hw id.
                                                                 - secboot context is NOT required
																 - param1 is secboot_hw_api_msm_hw_id_data buffer and param2 is buffer size */
#define SECBOOT_CTRL_HW_API_GET_USE_SERIAL_NUM    (0xB0FA4) /**< get fuse info: use_serial_num.
                                                                 - secboot context is NOT required
                                                                 - param1 is pointer to uint32 and param2 is code_segment */
#define SECBOOT_CTRL_HW_API_GET_SERIAL_NUM        (0xB0FA5) /**< get fuse info: serial num
                                                                 - secboot context is NOT required
                                                                 - param1 is pointer to uint32 and param2 is 0 */
#define SECBOOT_CTRL_HW_API_GET_SOC_HW_VER        (0xB0FA6) /**< get fuse info: soc_hw_version
                                                                 - secboot context is NOT required
                                                                 - param1 is pointer to uint32 and param2 is 0 */
#define SECBOOT_CTRL_HW_API_POPULATE_HW_FUSE_INFO  (0xB0FA7) /**< populate hw fuse info to secboot handle */ 

/**
 * @def SECBOOT_DEBUG_ENABLE
 *
 * @details Debug settings which are specified in the Debug OU field of the 
 *        attestation certificate to indicate what to do with the one-time
 *        override registers. The Debug OU field is 64 bits, lower 32 bits
 *        denote what debug settings the attestation certifiate has been
 *        signed with and the higer 32 bits are serial num. Bits 0 and 1 of 
 *        the debug settings denote if/what needs to be written to the one-time
 *        override registers. Images which need to act on these settings is architecture
 *        specific. When debug is enabled (SECBOOT_DEBUG_ENABLE), the serial number 
 *        embedded in the OU field must match the fuse. 
 */
#define SECBOOT_DEBUG_NOP     0x0 /**< Bit value 00 - No operation needs to be performed */
#define SECBOOT_DEBUG_DISABLE 0x1 /**< Bit value 01-  Write 0 to one-time override registers */                                     
#define SECBOOT_DEBUG_ENABLE  0x2 /**< Bit value 10 - Write 1 to one-time override registers*/

/**
 * @def SECBOOT_CRASH_DUMP_ENABLE
 *
 * @details Crash Dump settings which are specified in the Crash Dump OU field of the 
 *        attestation certificate to indicate whether the crash report is dumped for
 *        the specific phone. The Crash Dump OU field is 64 bits, lower 32 bits
 *        denote what Crash Dump settings the attestation certifiate has been
 *        signed with and the higer 32 bits are serial num. Bits 0 of the Crash Dump 
 *        settings denote the boolean flag. 0 means disabled and 1 means enabled. 
 *        When crash dump is enabled (SECBOOT_CRASH_DUMP_ENABLE), the serial number embedded 
 *        in the OU field must match the fuse. Absense of the field means disabled.
  */
#define SECBOOT_CRASH_DUMP_DISABLE 0x0 /**< Bit value 0 - crash dump is turned off */                                     
#define SECBOOT_CRASH_DUMP_ENABLE  0x1 /**< Bit value 1 - crash dump is turned on */

/**
 * @brief write permission settings which are specified by both root activation/revocation   
 *        enable OU field and uie key switch enable OU field in the attestation certificate
 *        to indicate whether the fuse region for configuring RoT and UIE key is writeable
 *        or not. 0x0 means the fuse region is open for write and 0x1 means not writeable.
 */
#define SECBOOT_ROOT_KEY_CONTROL_ENABLE   0x0 /**< Bit value 0 - enable write */                                     
#define SECBOOT_ROOT_KEY_CONTROL_DISABLE  0x1 /**< Bit value 1 - disable write */

/**
 * @def SECBOOT_ROOT_CERT_SEL_ENABLE
 *
 * @details The Root Cert Selection feature is disabled for SP and can
 *          be enabled for APPS/MODEM when TOTAL_ROT_NUM is blown
 *
 */
#define SECBOOT_ROOT_CERT_SEL_ENABLED  1 /**< Root Cert Selection is enabled */
#define SECBOOT_ROOT_CERT_SEL_DISABLED 0 /**< Root Cert Selection  is disabled */

/**
 * @brief Parm defined for secboot meta data structure.   
 */
#define MAX_SOC_VERS 12 /**< MAX number for supported soc hardware version */  
#define MAX_SERIAL_NUM 8 /**< MAX number for supported serial number */  
/** @} */ /* end_namegroup Secboot Constants */

/*----------------------------------------------------------------------------
 * Input data structures for Public APIs
 * -------------------------------------------------------------------------*/
 /**
 * Secboot handle, caller assigns the memory buffer for secboot operation.
 */
typedef struct secboot_handle_type
{
    uint32 handle[SECBOOT_CONTEXT_HANDLE_SIZE]; /**< 4 byte aligned buffer needed by secboot for it's operations */
}secboot_handle_type;

/**
 * Signing information about the image to be authenticated. This is input data
 * for authentication.
 * (The header + code are hashed for image signature verification)
 */
typedef struct secboot_image_info_type
{
  const uint8* header_ptr_1;   /**< Pointer to the header */
  uint32       header_len_1;   /**< Length in bytes of the image header */
  const uint8* code_ptr_1;     /**< Pointer to the code */
  uint32       code_len_1;     /**< Length in bytes of the image */
  const uint8* metadata_info_ptr; /**< Metadata information pointer */
  uint32       metadata_len;   /**< Metadata len */
  const uint8* x509_chain_ptr; /**< Pointer to the certificate chain */
  uint32       x509_chain_len; /**< Length in bytes of the certificate chain */
  const uint8* signature_ptr;  /**< Pointer to the signature */
  uint32       signature_len;  /**< Length in bytes of the  signature */
  uint32       sw_type;        /**< Type of the image being authenticated */
  uint32       secondary_sw_id; /**< Secondary SW ID for an image */
  uint32       sw_version;     /**< Minimum version of the image that can be executed (for rollback prevention) */
} secboot_image_info_type;

/**
 * MRC fuse information. This is part of secboot_fuse_info_type. This should not be
 * changed because secboot_fuse_info_type is released through secboot_init_hw_info()
 * public API.
 */
typedef struct secboot_mrc_fuse_info_type
{
  uint32   is_root_sel_enabled;  /**< Is Root Cert Selection enabled, set value as
                                      SECBOOT_ROOT_CERT_SEL_ENABLED/SECBOOT_ROOT_CERT_SEL_DISABLED */
  uint32   num_root_certs;       /**< valid only if root cert selection is enabled */
  uint32   revocation_list; /**< list of revoked root cert indicies */
  uint32   activation_list; /**< list of active root cert indicies */
} secboot_mrc_fuse_info_type; 

/**
 * secboot_ctrl data type for reading OTP root of trust fuse value. 
 * the root of trust value could be different per core (code_segment).
 */
typedef struct secboot_hw_api_rot_data
{
  uint32   code_segment;  /**< code segment, required by hw api*/
  uint8    root_of_trust[SECBOOT_OTP_ROOT_OF_TRUST_BYTE_SIZE];   /**< array for root of trust data */
} secboot_hw_api_rot_data;

/**
 * secboot_ctrl data type for reading msm hw id. 
 * the msm hw id value could be different per core.
 */
typedef struct secboot_hw_api_msm_hw_id_data
{
  uint32   code_segment;  /**< code segment, required by hw api*/
  uint64   msm_hw_id;     /**< fuse value integer */
} secboot_hw_api_msm_hw_id_data;

/**
 * the meta data which are tied to the signing and which will be verified
 * by secboot_authenticate call
 */
typedef PACKED_STRUCT secboot_metadata_type
{
  uint32 major_version;
  uint32 minor_version;
  uint32 sw_id;
  uint32 hw_id;
  uint32 oem_id;
  uint32 model_id;
  uint32 secondary_sw_id;
  uint32 flags;
  uint32 soc_vers[MAX_SOC_VERS];
  uint32 serial_num[MAX_SERIAL_NUM];
  uint32 root_cert_sel;
  uint32 anti_rollback;
}secboot_metadata_type;

/*----------------------------------------------------------------------------
 * OUTPUT shared data structures for Public APIs 
 * Data can be shared through PBL shared data and should be versionized, because
 * might be updated with new shared data as required by new features
 * -------------------------------------------------------------------------*/
 /**
 * Code Hashing information of the image. This is the output data for the caller to 
 * check image and its hash information.
 */
typedef struct secboot_code_hash_info_type
{
  uintnt   code_address;       /**< Address (pointer value) of the code that was hashed */
  uint32   code_length;        /**< The code length */
  uint32   image_hash_length;  /**< Hash length */
  uint8    image_hash[SECBOOT_HASH_DIGEST_BYTE_SIZE]; /**< hash of HEADER + CODE */
}secboot_image_hash_info_type;

/**
 * Secboot structure to hold the timestamps for PBL profile markers.
 */
typedef struct _secboot_pbl_profile_marker
{
	uint32 secboot_pbl_authenticate_init_entry;  /**< Start of secboot_authenticate entry */
	uint32 secboot_pbl_parse_cert_buffer;        /**< Start of parse_cert_buffer */
	uint32 secboot_pbl_hash_certificates;        /**< Start of hash certificates */
	uint32 secboot_pbl_verify_cert_chain;        /**< Start of verify cert chain */
	uint32 secboot_pbl_verify_image_signature;   /**< start of verify image signature */
  uint32 secboot_pbl_authenticate_close;       /**< secboot_authenticate is complete */
	
}secboot_pbl_profile_marker_type;

/**
 * Data returned from a successful authentication. In PBL case, this data is shared with
 * post-PBL image through PBL shared data.
 */
typedef struct secboot_verified_info_type
{
  uint32                       version_id;   /**< The version id (define the secboot lib version) */
  uint64                       sw_id;        /**< The software id (upper 32 bits:version, lower 32 bits:type)
											 the image was signed with */
  uint32                       enable_debug; /**< Value of the debug settings from the attestation cert, i.e., 
                                                  SECBOOT_DEBUG_NOP, SECBOOT_DEBUG_DISABLE, SECBOOT_DEBUG_ENABLE */
  secboot_image_hash_info_type image_hash_info; /**< Hash of the header + code */

  uint32                       enable_root_key_control; /**< Value to indicate if the configu fuse region is writeable
                                                              or not. i.e SECBOOT_ROOT_KEY_CONTROL_ENABLE,
                                                                          SECBOOT_ROOT_KEY_CONTROL_DISABLE */
  secboot_metadata_type metadata; /** Metadata from the image that is verified */
  
  secboot_pbl_profile_marker_type secboot_timestamps; /**< Performance timestamps */

} secboot_verified_info_type;

/*----------------------------------------------------------------------------
 * IN/OUT shared data structures for Public APIs 
 * -------------------------------------------------------------------------*/
/**
 * secboot_ctrl data type for verifying metadata. 
 */
typedef struct secboot_api_verify_meta_data_type
{
  const uint8* metadata_info_ptr; /**<Metadata information pointer */
  uint32       metadata_len;      /**<Metadata len */
  uint32       code_segment;      /**< code segment, required by hw api*/
  uint32       sw_id;             /**< Type of the image being authenticated */
  uint32       secondary_sw_id;   /**< Secondary SW ID for an image */
  uint32       sw_version;        /**< Minimum version of the image that can be executed (for rollback prevention) */
  secboot_verified_info_type*     verified_info_ptr; /**< output verified info data */
} secboot_api_verify_meta_data_type;

/*----------------------------------------------------------------------------
 * Function Tables
 * -------------------------------------------------------------------------*/
/** 
 * Error type definition for secboot crypto callback function
 */
#define E_SECBOOT_CB_SUCCESS                (0)   /**< Operation was successful. */
#define E_SECBOOT_CB_FAILURE                (1)   /**< General failure. */
#define E_SECBOOT_CB_INVALID_PARAM          (2)   /**< Invalid parameter passed into function. */
#define E_SECBOOT_CB_HASH_INIT_FAIL         (3)   /**< crypto hash init fail. */
#define E_SECBOOT_CB_HASH_UPDATE_FAIL       (4)   /**< crypto hash update fail. */
#define E_SECBOOT_CB_HASH_FINAL_FAIL        (5) /**< crypto hash final fail */
#define E_SECBOOT_CB_HASH_DEINIT_FAIL       (6)   /**< crypto hash deinit fail. */
#define E_SECBOOT_CB_INVALID_CTX_SIZE_FAIL  (7)   /**< ctx size is not sufficient. */
#define E_SECBOOT_CB_RSA_MODEXP_FAIL        (16)   /**< rsa modexp operation error. */

/**
 * Data Type for crypto run-time buffer. Caller MUST provide run-time buffer.
 */
typedef struct crypto_ctx_type {
  uint8    *ctx_imem;  /**< pointer to crypto internal memory, assigned by caller for crypto operation */
  uint32   ctx_imem_size;   /**< Crypto driver to provide the size of crypto run-time buffer */
} crypto_ctx_type;

/**
 * Type definition of the crypto callback function pointer table. 
 *
 *    Caller implements required crypto function, and Secboot library will use them for
 *    image authentication. The proper run-time buffer should be provided as well by caller.
 *    The caller can use default Crypto functions defined in the image by passing NULL to
 *    function pointers. Default functions uses Crypto driver in local image. The caller still
 *    needs to provide run time buffer.
 *    
 */
typedef struct secboot_crypto_ftbl_type {
	uint32 version;         /**< version id - set to SECBOOT_CRYPTO_CB_VERSION */
    crypto_ctx_type handle; /**< caller passes in handle with run-time buffer */
    uint32 (*HashInit) (crypto_ctx_type* ctx, uint32 hash_algo);
    uint32 (*HashUpdate) (crypto_ctx_type* ctx, const void *data, uint64 data_len);
    uint32 (*HashFinal) (crypto_ctx_type* ctx, void* digest_ptr, uint32 digest_len);
    uint32 (*HashDeInit) (crypto_ctx_type* ctx);
    uint32 (*ModExp) (crypto_ctx_type* ctx,      // the handle context
                      uint32 mod_size,                     // the mod size
                      uint32 exp_size,                     // the exp size
                      uint8* mod,                          // the ptr to mod
                      uint8* message,                      // the ptr to message
                      uint8* exp,                          // the ptr to exp
                      uint8* r);                           // the ptr to return value
}secboot_crypto_ftbl_type;

/**
 * Type definition of the CounterMeasure function pointer table. 
 *
 *    Caller passes in caller-defined CounterMeasure function, and Secimgauth and Secboot library will  
 *    use the caller-defined functions for CounterMeasure Protection. The caller should define  
 *    CounterMeasure functions based on API signature in this table. 
 * 
 * NOTE: The caller SHOULD provide COMPLETE set of counter measure functions. Otherwise, image
 *       authenticaton will fail.
 */
typedef struct secboot_cm_ftbl_type {
  uint32 version;  /**< the version id - set to SECBOOT_CM_CB_VERSION*/

  /**
   * Pointer to the function to Random Wait. (MUST-HAVE from the caller)
   *
   * @return none, as no need to check the return
   *
   */
  void (*random_delay)();

  /**
   * Pointer to the function to handle the CM fault. (MUST-HAVE from the caller)
   *
   * @return none, as no need to check the return
   *
   */
  void (*fault_detected)();
}secboot_cm_ftbl_type;

/* Counter-measure functions */
void mba_cm_random_wait(void);
void mba_cm_fault_detected(void);

/* Define Macros to use countermeasure function table */
/* call random_delay if cm_ftbl pointer is not NULL. */
#define CM_RANDOM_DELAY(cm_ftbl) { \
    if ((cm_ftbl != NULL) &&  \
        (cm_ftbl->random_delay != NULL)) \
    { \
      cm_ftbl->random_delay(); \
    }}

/* break out and execute operation if (cond) is TRUE. otherwise contine. */
#define CM_BREAKIF(cond, operation) {\
    if (cond) \
    { \
      {operation;} \
      break; \
    }}

/* execute operations whenever Single Instruction Attack is detected. The error should
 * be handled right at the spot once the fault is detected.
 *
 * NOTE:
 *   The fault operations are called twice in a row when fault is detected.
 */
#define CM_FAULT_DETECTED_OPS(cm_ftbl) {\
    if ((cm_ftbl != NULL) &&  \
        (cm_ftbl->fault_detected != NULL)) \
    { \
      cm_ftbl->fault_detected(); \
      cm_ftbl->fault_detected(); \
    }}

/* break out if (cond) cannot be verified or SINGLE INSTRUCTION ATTACK is 
 * FOUND, otherwise contine. wait for random amount of time between two
 * checks. if cm_ftbl is NULL, no wait between two checks.
 *
 * NOTE:
 *   the varible in {cond} must be volatile to defend the Single Instructio
 * Attack
 */
#define CM_VERIFY_BREAK(cond, cm_ftbl, SIA_detected_instruction) {\
  if(cond) \
  { \
    CM_RANDOM_DELAY(cm_ftbl); \
    CM_BREAKIF(!(cond), {SIA_detected_instruction}); \
  } \
  else \
  { \
  	break; \
  }}

/* return TRUE if cm_ftbl or any function pointer within cm_ftbl fails the 
 * NULL pointer check. return FALSE if there is no NULL pointer on required
 * function pointer within cm_ftbl
 */
#define CM_CHECK_FTBL_NULL_PTR(cm_ftbl) \
    ((cm_ftbl == NULL) || \
     (cm_ftbl->random_delay == NULL) || \
     (cm_ftbl->fault_detected == NULL))

/**
 * Secboot function table, shared with post-PBL caller through PBL shared data.
 * The post-PBL caller can use Secboot APIs within PBL.
 * (Data )
 */
typedef struct secboot_ftbl_type
{
  uint32              version;        /** Version id - set to SECBOOT_SHARED_API_VERSION */
  secboot_error_type (*secboot_init)  /** Pointer to secboot_init */
                      (secboot_handle_type*                secboot_handle_ptr,
                       secboot_crypto_ftbl_type*           secboot_crypto_ftbl_ptr,
					   secboot_cm_ftbl_type*               cm_ftbl,
                       struct secboot_ftbl_type*           secboot_ftbl_ptr); // set crypto cb function
  secboot_error_type (*secboot_deinit)  /** Pointer to secboot_deinit */
                      (secboot_handle_type*                secboot_handle_ptr);
  secboot_error_type (*secboot_authenticate)  /** Pointer to secboot_authenticate */
                      (secboot_handle_type*                secboot_handle_ptr,
                       uint32                              code_segment,
                       const secboot_image_info_type*      image_info_ptr,
                       secboot_verified_info_type*         verified_info_ptr);
  secboot_error_type (*secboot_ctrl)    /** Pointer to secboot_ctrl */
                     ( secboot_handle_type*   secboot_handle_ptr,
                       uint32                 cmd_id, 
                       uint64                 param,
                       uint64                 param_size);
}secboot_ftbl_type;

/*----------------------------------------------------------------------------
 * Secboot Library Public APIs
 * -------------------------------------------------------------------------*/
/**
 * This function initializes the secboot handle and sets up the crypto hw engine
 *  
 * @param[in,out] secboot_handle_ptr    Pointer to the secboot handle. The handle
 *                                      must be allocated in protected memory
 *                                      (Pointer must not be NULL)
 * @param[in]     crypto_ftbl_ptr       Pointer to the crypto hash driver functions 
 *                                      that secboot should use for hashing and RSA. 
 *                                      Caller always provide proper run-time buffer.
 *                                      (Pointer must not be NULL)
 *                                      Caller can provide crypto functions, or uses
 *                                      default ones defined locally if setting 
 *                                      function pointers within the table to NULL.
 * @param[in]     cm_ftbl               Pointer to CM function table.
 *                                      (MUST be not NULL)
 * @param[in]     secboot_ftbl_ptr      Pointer to the secboot functions. Secboot  
 *                                      call caller provided functions in delegation
 *                                      mode if this is not NULL. If caller provides
 *                                      NULL, secboot uses local implementation.
 *  
 * @return E_SECBOOT_HW_SUCCESS on success. Appropriate error code on failure. If 
 *         this function returns an error, callers need not call secboot_deinit()
 *         as this will be called from within the error handling routine of this
 *         function 
 *
 * @dependencies This function must be called before any other function in this
 *               API is called. Caller must not deallocate the secboot handle
 *               until after secboot_deinit().
 *
 * @sideeffects  Calls the CeMLInit() to initialize the Crypto HW engine. 
 *
 * @sa secboot_deinit()
 *
 */
secboot_error_type secboot_init
(
  secboot_handle_type*       secboot_handle_ptr,
  secboot_crypto_ftbl_type*  secboot_crypto_ftbl_ptr,
  secboot_cm_ftbl_type*      cm_ftbl,
  secboot_ftbl_type*         secboot_ftbl_ptr
);

/**
 * This function authenticates an image. 
 *
 * @param[in,out]     secboot_handle_ptr   Pointer to the secboot handle returned
 *                                         from secboot_init()
 *
 * @param[in]         code_segment         Code segment in SECURE_BOOTn fuse register
 *                                         containing authentication settings for
 *                                         the image.
 *
 * @param[in]         image_info_ptr       Information on the image to be authenticated
 *                                         
 * @param[in,out]     verified_info_ptr    Pointer to data that was processed during
 *                                         authentication. Caller can use this data
 *                                         if the function returned success
 *
 * @return E_SECBOOT_HW_SUCCESS on success. Appropriate error code on failure. If a failure 
 * was returned, the caller SHOULD NOT allow the image to execute
 *
 * @dependencies secboot_init() needs to have been called. 
 *
 * @sideeffects  none
 *               
 * @sa secboot_init(), secboot_deinit()
 *               
 */
secboot_error_type secboot_authenticate
(
  secboot_handle_type*                secboot_handle_ptr,
  uint32                              code_segment,
  const secboot_image_info_type*      image_info_ptr,
  secboot_verified_info_type*         verified_info_ptr
);

/**
 * This function clears the secboot handle.
 *
 * @param[in,out] secboot_handle_ptr   Pointer to the secboot handle returned
 *                                     from secboot_init()
 *
 * @return E_SECBOOT_HW_SUCCESS on success. Appropriate error code on failure.
 *
 * @dependencies None
 *
 * @sideeffects  Calls the CeMLDeInit() to de-initialize the Crypto HW engine
 *
 * @sa secboot_init()
 *
 */
secboot_error_type secboot_deinit
(
  secboot_handle_type*      secboot_handle_ptr
);

/**
 * This function returns pointers to the secboot functions linked into the image.
 *
 * @param[in,out] ftbl_ptr              Pointer to the function table structure
 *                                      to populate. The pointer must be allocated
 *                                      by the caller.
 *
 * @return E_SECBOOT_HW_SUCCESS on success. Appropriate error code on failure.
 *
 * @sideeffects  None
 *
 *
 */
secboot_error_type secboot_get_ftbl
(
  secboot_ftbl_type* ftbl_ptr
);

/**
 * This function calls the crypto software driver's hash API to hash the data.
 *
 * @param[in]         crypto_ftbl       Pointer to the crypto hash function table,
 *                                      secboot_crypto_ftbl_type. Caller to define
 *                                      crypto hash function and provides run-time
 *                                      buffer that hash functions require. (Only
 *                                      HashInit, HashUpdate, HashFinal, HashDeInit
 *                                      are required. ModExp is set to NULL)
 *
 * @param[in]         hash_algo         Hash algorithm to use. Caller defines and  
 *                                      interpret in HashUpdate for crypto engine.
 *
 * @param[in]         data1_in_ptr      Pointer to the data to hash. Cannot be NULL. 
 *                                      as we don't expect boot images/certificates
 *                                      to be loaded at address 0
 *
 * @param[in]         data1_len         Length in bytes of the data pointed to by
 *                                      data1_in_ptr. Cannot be 0 as we don't allow
 *                                      image or certificate size to be 0
 *
 * @param[in]         data2_in_ptr      Pointer to the data to hash. This is an
 *                                      optional field and can be NULL.
 *
 * @param[in]         data2_len         Length in bytes of the data pointed to by
 *                                      data2_in_ptr. 0 if data2_in_ptr is NULL.
 *                                  
 * @param[in,out]     digest_ptr        Pointer to a buffer to store the hash result.
 *
 * @param[in]         digest_len        Length in bytes of buffer pointed to by
 *                                      digest_ptr.
 *
 * @return E_SECBOOT_SUCCESS on success. Appropriate error code on failure.
 *
 * @dependencies Caller should ensure the pointers and lengths passed in are valid
 *
 */
secboot_error_type secboot_hash
(
  secboot_crypto_ftbl_type*                 crypto_ftbl,
  uint32                                    hash_algo,
  const uint8*                              data1_in_ptr,
  uint64                                    data1_len,
  const uint8*                              data2_in_ptr,
  uint64                                    data2_len,
  uint8*                                    digest_ptr,
  uint32                                    digest_len
);

/**
  * @brief This function return the maximum allowed size of the
  *        certificate chain
  *
  * @return cert chain size
  *
  * @dependencies None
  *
  * @sideeffects  None
  *
  * @see None
  *
  */
uint32 secboot_get_max_cert_chain_size(void);

/**
  * @brief This function performs the paramaters used by secboot
  *
  * @param secboot_handle_ptr   [in] Pointer to context handle
  *                                  Part of commands accept context handle as NULL.
  *                                  NULL handle means all local operation (no delegation 
  *                                  disabled)
  * @param cmd_id               [in] command id
  * @param param1               [in/out] parameter data.
  *                                      For set function, provide data to set. If struct
  *                                      is provided, struct size is provided in param2.
  *                                      For get function, provide data buffer pointer. If
  *                                      buffer size is larger than sizeof(uint32), buffer
  *                                      size should be provided in param2.
  * @param param2               [in] used as the size in bytes of data, or additional
  *                                  parameter, or if not used, set as 0.
  *
  * @return E_SECBOOT_SUCCESS on success. Appropriate error code on failure.
  *
  * @sa secboot_init()
  *
  */

secboot_error_type secboot_ctrl
(
  secboot_handle_type*   secboot_handle_ptr,
  uint32                 cmd_id, 
  uint64                 param1,
  uint64                 param2
);


/** this function addes public key from the cert chain to authenticate
  * image signature
  * @param[in]     handle       Pointer to handle
  * @param[in]     cert_chain   Pointer to the buffer start address for the cert chain
  * @param[in]     size         The buffer size
  *
  * @return E_SECBOOT_SUCCESS on success. Appropriate error code on failure.
  *
  * @sa secboot_init()
  *
  */
#define secboot_add_cert_chain(handle, cert_chain, size) \
          secboot_ctrl(handle, SECBOOT_CTRL_ADD_CERT_CHAIN, \
		               cert_chain, size) \

/** this function verifies single root cert and addes public key for authentication
  * subca cert
  * @param[in]     handle       Pointer to handle
  * @param[in]     root_cert    Pointer to the buffer start address for the root cert
  * @param[in]     size         The buffer size
  *
  * @return E_SECBOOT_SUCCESS on success. Appropriate error code on failure.
  *
  * @sa secboot_init()
  *
  */
#define secboot_add_root_pubkey(handle, root_cert, size) \
          secboot_ctrl(handle, SECBOOT_CTRL_ADD_ROOT_CERT_PUBKEY, \
		               root_cert, size) \

/** this function verifies single subca cert and addes public key for authentication
  * attest cert
  * @param[in]     handle       Pointer to handle
  * @param[in]     subca_cert   Pointer to the buffer start address for subca cert
  * @param[in]     size         The buffer size
  *
  * @return E_SECBOOT_SUCCESS on success. Appropriate error code on failure.
  *
  * @sa secboot_init()
  *
  */
#define secboot_add_subca_pubkey(handle, subca_cert, size) \
          secboot_ctrl(handle, SECBOOT_CTRL_ADD_SUBCA_CERT_PUBKEY, \
		               subca_cert, size) \

/** this function verifies single attest cert and addes public key for authentication
  * image signature
  * @param[in]     handle       Pointer to handle
  * @param[in]     attest_cert  Pointer to the buffer start address for the attest cert
  * @param[in]     size         The buffer size
  *
  * @return E_SECBOOT_SUCCESS on success. Appropriate error code on failure.
  *
  * @sa secboot_init()
  *
  */
#define secboot_add_attest_pubkey(handle, attest_cert, size) \
          secboot_ctrl(handle, SECBOOT_CTRL_ADD_ATTEST_CERT_PUBKEY, \
		               attest_cert, size) \
 
/** @}
*/
#endif /* SECBOOT_H */

