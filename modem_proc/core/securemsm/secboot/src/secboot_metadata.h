#ifndef SECBOOT_METADATA_H
#define SECBOOT_METADATA_H

/*===========================================================================

  Function declaration for checking secboot meta data

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All rights reserved.
  Qualcomm Technologies, Inc. Confidential and Proprietary.

===========================================================================*/

/*=========================================================================

                            EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.10/securemsm/secboot/src/secboot_metadata.h#1 $
$DateTime: 2018/11/08 03:29:06 $
$Author: pwbldsvc $

when       who                  what, where, why
--------   ----                 ------------------------------------------- 
02/13/17    hw                  initial version

===========================================================================*/
#include "secboot_i.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define SECBOOT_METADATA_MAJOR  (0UL)
#define SECBOOT_METADATA_MINOR  (0UL)

/* Macro to declare meta data get function */
#define SECBOOT_METADATA_GET_FUNC_DECLARE(func_field_name, ret_data_type) \
ret_data_type secboot_metadata_get_##func_field_name \
( \
  secboot_context_type*      context_ptr \
);

/* Valid OU field options of IN_USE_SOC_HW_VERSION in signed image */
#define SECBOOT_IN_USE_SOC_HW_VER_MSK         (0x00000002UL)
#define SECBOOT_USE_SERIAL_NUMBER_MSK         (0x00000004UL)
#define SECBOOT_OEM_ID_INDP_MSK               (0x00000008UL)
#define SECBOOT_META_DEBUG_REENABLE_MSK       (0x00000300UL) 
#define SECBOOT_META_DEBUG_REENABLE_SHIFT     (0x8) 
#define SECBOOT_META_UIE_KEY_SWITCH_MSK       (0x000000C0UL)
#define SECBOOT_META_UIE_KEY_SWITCH_SHIFT     (0x6) 

#define SECBOOT_IN_USE_SOC_HW_VER_DISABLE  0x0 /**< Bit value 0 - do not use soc hw version */
#define SECBOOT_IN_USE_SOC_HW_VER_ENABLE  0x1 /**< Bit value 1 - use soc hw version */

/* Valid OU field options of USE_SERIAL_NUMBER_IN_SIGNING in signed image */
#define SECBOOT_USE_SERIAL_NUMBER_DISABLE  0x0/**< Bit value 0 - do not use serial number */                                     
#define SECBOOT_USE_SERIAL_NUMBER_ENABLE  0x1 /**< Bit value 1 - use serial number */

/* Valid OU field options of OEM_ID_INDEPENDENT in signed image */
#define SECBOOT_OEM_ID_INDP_DISABLE 0x0/**< Bit value 0 - do not use oem independent id */                                     
#define SECBOOT_OEM_ID_INDP_ENABLE  0x1 /**< Bit value 1 - use oem independent id */

/** 
 * @brief UIE key switch enablement settings which are specified in the 
 *        UIE_KEY_SWITCH_ENABLE OU field of the attestation certificate to
 *        indicate whether it is allowed to switch UIE key. If enabled, 
 *        the write permission of the fuse to select UIE key is enabled. 
 *        If disabled, the fuse is not programmable.
 */
#define SECBOOT_UIE_KEY_SWITCH_DISABLE 0x0 /**< Bit value 0 - uie key switch fuse is not 
                                                programmable. */
#define SECBOOT_UIE_KEY_SWITCH_ENABLE  0x1 /**< Bit value 1 - uie key switch fuse is programmable  */
#define SECBOOT_UIE_KEY_SWITCH_ENABLE_SN 0x2 /**< Bit value 2 - uie key switch fuse is
                                                  programmable with matched serial num. */

/**
* @brief Revoke/Activate settings which are specified in the
*        Revoke/Activate OU field of the attestation certificate to
*        indicate whether revoke/activate is allowed. The Revoke/Activate
*        OU field is 64 bits, lower 32 bits denote what
*        Revoke/Activate settings the attestation certifiate has
*        been signed with and the higer 32 bits can be the
*        serial num. Bit 0 of the Revoke/Activate settings denote
*        the boolean flag. 1 or 2 means enabled and 0 means
*        disabled. When revoke/activate is enabled
*        (SECBOOT_REVOCATION_ENABLE_SN), the serial number
*        embedded in the OU field must match the fuse. When
*        revoke/activate is enabled (SECBOOT_REVOCATION_ENABLE), the
*        serial number is not embedded in the OU fields.
*        Absense of the field means disabled.
*/
#define SECBOOT_REVOKE_ACTIVATE_DISABLE 0x0 /**< Bit value 0 - revocation is disabled */                                     
#define SECBOOT_REVOKE_ACTIVATE_ENABLE  0x1 /**< Bit value 1 - revocation is enabled */
#define SECBOOT_REVOKE_ACTIVATE_ENABLE_SN 0x2 /**< Bit value 2 - revocation is enabled with serial num*/

/**<  Shift to get at the serial number from upper 32 bits of the debug enable OU field*/
#define SECBOOT_DEBUG_SERIALNUM_FIELD_SHFT    (0x20)

/**<  Mask to get the crash dump enable/disable bits in the crash dump OU field */
#define SECBOOT_CRASH_DUMP_SETTINGS_FIELD_MASK     (0xFFFFFFFFUL)
/**<  Shift to get at the serial number from upper 32 bits of the crash dump enable OU field*/
#define SECBOOT_CRASH_DUMP_SERIALNUM_FIELD_SHFT    (0x20)

/**<  Zero out the upper 32bit of msm_hw_id for soc hw version, and keep the lower 32bit unchanged */
#define SECBOOT_SOC_HW_VER_FIELD_MASK     (0xFFFFFFFFULL)
/**<  Shift the soc hw version to upper 32 bits of msm_hw_id */
#define SECBOOT_SOC_HW_VER_FIELD_SHFT    (0x20)

/**<  Zero out the lower 32bit of msm_hw_id for serial number, and keep the upper 32bit unchanged */
#define SECBOOT_OU_USE_SERIAL_NUMBER_FIELD_MASK     (0xFFFFFFFF00000000ULL)

/**<  Zero out the lower 32bit of msm_hw_id for oem independent id, and keep the upper 32bit unchanged */
#define SECBOOT_OU_OEM_ID_INDEPENDENT_FIELD_MASK     (SECBOOT_UINT64_HIGH_32BIT_FIELD_MASK)

/**<  take out the lower 32bit of the OU field SW_ID for image sw id, and the higher 32bit is rollback ver. */
#define SECBOOT_OU_SW_ID_FIELD_MASK     (SECBOOT_UINT64_LOW_32BIT_FIELD_MASK)

/**<  take out the lower 32bit of the UIE_KEY_SWITCH_ENABLE OU field . */
#define SECBOOT_OU_UIE_KEY_SWITCH_ENABLE_FIELD_MASK     (SECBOOT_UINT64_LOW_32BIT_FIELD_MASK)

/**<  Shift to get at the serial number from upper 32 bits of the UIE_KEY_SWITCH_ENABLE OU field*/
#define SECBOOT_OU_UIE_KEY_SWITCH_ENABLE_SERIALNUM_FIELD_SHFT    (0x20)
/*----------------------------------------------------------------------------
 * Function Declaration
 * -------------------------------------------------------------------------*/
/**
* @brief This function return root cert selection from metadata
*        secboot_metadata_get_root_cert_sel()
*/
SECBOOT_METADATA_GET_FUNC_DECLARE(root_cert_sel, uint32);

/**
* @brief This function return sw id from metadata
*        secboot_metadata_get_sw_id()
*/
SECBOOT_METADATA_GET_FUNC_DECLARE(legacy_sw_id, uint64);

/**
* @brief This function validate metadata for certificate
*
* @param[in] context_ptr           Pointer to the context 
*
* @param[in] metadata_info_ptr     Pointer to image info data
*
* @param[in] metadata_len         meta data length
*
* @return E_SECBOOT_SUCCESS on success. Appropriate error code on failure.
*
* @dependencies Previous function verifies input parameters
*
*/
secboot_error_type secboot_set_metadata
(
  secboot_context_type*      context_ptr,
  const uint8*               metadata_info_ptr,
  uint32                     metadata_len
);

/**
* @brief This function validate metadata for certificate
*
* @param[in] context_ptr           Pointer to the context 
*
* @param[in] image_info_ptr        Pointer to image info data
*
* @param[out] verified_info_ptr    Pointer to verified output data
*
* @return E_SECBOOT_SUCCESS on success. Appropriate error code on failure.
*
* @dependencies Previous function verifies input parameters
*
*/
secboot_error_type secboot_validate_metadata
(
  secboot_context_type*               context_ptr,
  const secboot_image_info_type*      image_info_ptr,
  secboot_verified_info_type*         verified_info_ptr
);

#endif //

