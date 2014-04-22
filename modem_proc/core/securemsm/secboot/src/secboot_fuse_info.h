#ifndef SECBOOT_FUSE_INFO_H
#define SECBOOT_FUSE_INFO_H

/*===========================================================================

  Function definition for data structure to store secboot fuse information
  Define data type separately without dependency on secboot, hw or x509 component.

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All rights reserved.
  Qualcomm Technologies, Inc. Confidential and Proprietary.

===========================================================================*/

/*=========================================================================

                            EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.10/securemsm/secboot/src/secboot_fuse_info.h#1 $
$DateTime: 2018/11/08 03:29:06 $
$Author: pwbldsvc $

when       who                  what, where, why
--------   ----                 ------------------------------------------- 
04/13/17    hw                  initial version

===========================================================================*/
#include "secboot_env.h"

/*----------------------------------------------------------------------------
 * Paramater and Macro Definition
 * -------------------------------------------------------------------------*/
/* MAX buffer size to cache root of trust */
#define SECBOOT_MAX_CACHE_ROOT_OF_TRUST_BYTE_SIZE (SECBOOT_HASH_DIGEST_BYTE_SIZE)

 /** @name Secboot Hardware Fuse Constants
 @{ */
/**
* @brief If the DEBUG OU field is absent and DEBUG_DISABLE_IN_ROM fuse is 0x1,
* then debug cannot be re-enabled.
* If DEBUG OU field is absent and DEBUG_DISABLE_IN_ROM fuse is 0x0, then debug
* can be re-enabled later in SBL/TZ/MBA
*
*/
#define SECBOOT_HW_DEBUG_REENABLE_DISABLED  0x1 /**< Debug cannot be re-enabled */
#define SECBOOT_HW_DEBUG_REENABLE_ENABLED  0x0 /**< Debug can be re-enabled */

/**
 * @def SECBOOT_RSA_ENABLED
 *
 * RSA can be enabled
 * by blowing 0x0 in the DISABLE_RSA fuse. If this is blown then
 * RSA certs and verification are allowed. By default the feature is
 * enabled.
 */
#define SECBOOT_RSA_DISABLED  0x1 /**< RSA is disbled */
#define SECBOOT_RSA_ENABLED 0x0 /**< RSA is enabled */

/**
 * @def SECBOOT_USE_LEGACY_SIG
 *
 * Calculate the image signature with legacy sig format: QC_HMAC + no metadata
 */
#define SECBOOT_LEGACY_SIG_ENABLED  0x1 /**< Legacy sig is used */
#define SECBOOT_LEGACY_SIG_DISABLED 0x0 /**< Legacy sig is not used */
/**<  Shift msm_hw_id for hw_id and oem_id */
#define SECBOOT_MSM_HW_ID_HW_ID_FIELD_SHFT       (0x20)
#define SECBOOT_MSM_HW_ID_OEM_ID_FIELD_SHFT      (0x10)

/** @} */ /* end_namegroup Secboot Hardware Fuse Constants */

/*----------------------------------------------------------------------------
 * Data Structure Definition
 * -------------------------------------------------------------------------*/
/**
 * MRC fuse information. This is part of secboot_fuse_info_type.
 */
typedef struct secboot_mrc_fuse_data_type
{
  uint32   is_root_sel_enabled;  /**< Is Root Cert Selection enabled  */
  uint32   num_root_certs;       /**< valid only if root cert selection is enabled */
  uint32   revocation_list; /**< list of revoked root cert indicies */
  uint32   activation_list; /**< list of active root cert indicies */
} secboot_mrc_fuse_data_type; 

/**
 * This structure contains the fuse information used by the secboot authentication process.
 * Caller can optionally supply these fuse value by setting use_supplied_info to TRUE. (this 
 * could be done by manually setting the variable or through secboot_init_fuses() api)
 *
 * Caller has option to supply either all the fuse values or only root_of_trust. Which will
 * be determined by structure parameter "use_root_of_trust_only". If it is equal to TRUE, Caller
 * supplies only root_of_trust (256bit). Otherwise, for value FALSE, Caller supplies all the fuses.
 * 
 * If caller didn't pass any fuse value (don't set use_supplied_info to TRUE), secboot authentication 
 * process read all these fuse parameters from h/w.
 *
 * If the caller passes only root_of_trust (caller sets use_root_of_trust_only = TRUE), secboot  
 * authentication process reads remaining fuse parameters from hardware.
 *
 * This should not be changed because it is public released through secboot_init_hw_info() API,
 * and this data structure has been used by PBL and xBL-sec.
 *
 * For any newly added fuse data in the future, we should secboot_ctrl() API to set the value.
 */
typedef struct secboot_fuse_info_type
{
  uint8        use_supplied_info; /**< User supplied fuse information */
  uint8        root_of_trust[SECBOOT_MAX_CACHE_ROOT_OF_TRUST_BYTE_SIZE]; /**< sha256 hash of the root certificate */
  uint64       msm_hw_id;              /**< Fuse to the device's hw id */
  uint32       auth_use_serial_num;    /**< Use serial number in device's hw id */
  uint32       serial_num;             /**< Fuse to device's serial number */
  uint32       use_root_of_trust_only; /**< Caller sets this variable to TRUE if 
                                            secboot needs to use only root of trust from the 
                                            supplied fuses */
  secboot_mrc_fuse_data_type    mrc_fuse_info; /**< contains mrc specific values */
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
} secboot_fuse_info_type;

#endif
