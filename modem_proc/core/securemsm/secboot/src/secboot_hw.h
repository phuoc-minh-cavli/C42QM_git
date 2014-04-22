#ifndef SECBOOT_HW_H
#define SECBOOT_HW_H
/*****************************************************************************
*
* @file  secboot_hw.h (Secboot Hardware API)
*
* @brief API to read Security Control Fuses containing authentication
*        information
*       
* Copyright (c) 2010,2014,2016-2017 Qualcomm Technologies, Inc.
* All rights reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*
*****************************************************************************/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.10/securemsm/secboot/src/secboot_hw.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/11/17   hw      Secboot 3.0 refactoring
02/02/16   hw      Update for Nazgul boot arch
08/05/14   hw      Add QPSI code review fix
05/28/14   hw      Add RoT support
05/27/10   sm      Created module

============================================================================*/
#include "secboot_env_i.h"
#include "secboot.h"

/**
 * @addtogroup SecbootHardwareMSM
 *
 * @{
 */

 /**
 * @brief Identifies the error type returned by the API.
 */
typedef enum
{
  E_SECBOOT_HW_SUCCESS                = 0x5A9D3AA0, /**< Operation was successful. */
  E_SECBOOT_HW_FAILURE                = 0x1, /**< General failure. */
  E_SECBOOT_HW_INVALID_PARAM          = 0x2, /**< Parameter passed in was invalid */
  E_SECBOOT_HW_OUT_OF_RANGE           = 0x3, /**< Index out of bounds */
  E_SECBOOT_HW_FEC_ERROR              = 0x4, /**< FEC error status detected */
  E_SECBOOT_HW_MAX                    = 0x7FFFFFFF /**< Max unused code */
} secboot_hw_etype;

/*----------------------------------------------------------------------------
 * Secboot Hardware Constants (define constants used by secboot hardware function)
  * -------------------------------------------------------------------------*/
 /** @name Secboot Hardware Constants
 @{ */

#define SECBOOT_CODE_SEG_MIN 1   /**< Corresponds to first SECBOOTn supported */
#define SECBOOT_CODE_SEG_MAX 28  /**< Corresponds to last SECBOOTn supported */

#define SECBOOT_SEC_CODE_SEG_MIN 0x40  /**< Min Code Segment for Secure Platform Code without fuses */
#define SECBOOT_SEC_CODE_SEG_MAX 0x7F  /**< Max Code Segment for Secure Platform Code without fuses */

/** @brief This macro checks if the given segment number is within the  
  *        valid range supported 
  *
  * @param[in] segment segment number 
  * @return TRUE if segment is within the supported range, FALSE otherwise 
  * */
#define SECBOOT_CHECK_CODE_SEGMENT_VALID(segment) (((segment) >= SECBOOT_CODE_SEG_MIN) && ((segment) <= SECBOOT_CODE_SEG_MAX))

/* Check if code segment falls within trusted code and return the Qualcomm Root if so*/
#define SECBOOT_CHECK_SEC_CODE_SEGMENT_VALID(segment) (((segment) >= SECBOOT_SEC_CODE_SEG_MIN) \
                                  && ((segment) <= SECBOOT_SEC_CODE_SEG_MAX))

/**
 * @def SECBOOT_HW_OEM_INDEPENDENT_ID
 *
 * @details OEM INDEPENDENT ID is used by default for QC signing, which
 *          however can be disabled by fuse on SP PBL. When this OU is used,
 *          Signing is not tied to OEM_ID. When it is disabled by
 *          fuse, signing shall be tied to OEM_ID.                  
 */
#define SECBOOT_HW_OEM_INDEPENDENT_ID   (0x00010000)

/* The mask for family number and device number within soc hw version */
/* NOT reading hw fuse, use only the mask definition from hwio header */
#define SECBOOT_SOC_HW_VER_FAMILY_DEVICE_NUMBER \
    (HWIO_FMSK(TCSR_SOC_HW_VERSION, FAMILY_NUMBER) | \
     HWIO_FMSK(TCSR_SOC_HW_VERSION, DEVICE_NUMBER))

/** @} */ /* end_namegroup Secboot Hardware Constants */

/*----------------------------------------------------------------------------
 * Input Data Structure for setting up the fuse info.
 * -------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
 * Output Shared Data Structure
 * -------------------------------------------------------------------------*/

/**
 * Secboot Hardware Function Table. This is shared by PBL for post-PBL user calls.
 * This table should NOT be modified unless post-PBL images's asking for new hwio
 * API.
 */
typedef struct secboot_hw_ftbl_type
{
  secboot_hw_etype (*secboot_hw_is_auth_enabled) /** Pointer to secboot_hw_is_auth_enabled */
                    (uint32    code_segment,
                     uint32*   auth_enabled_ptr,
                     secboot_cm_ftbl_type* cm_ftbl);
  secboot_hw_etype (*secboot_hw_get_root_of_trust) /** Pointer to secboot_hw_get_root_of_trust */
                    (uint32 code_segment,
                     uint8*  root_of_trust,
					 uint32 rot_size);
  secboot_hw_etype (*secboot_hw_get_msm_hw_id)  /** Pointer to secboot_hw_get_msm_hw_id */
                    (uint32        code_segment,
                     uint64*       msm_hw_id_ptr);
  secboot_hw_etype (*secboot_hw_get_use_serial_num)  /** Pointer to secboot_hw_get_use_serial_num */
                    (uint32         code_segment,
                     uint32*        auth_use_serial_num_ptr);
  secboot_hw_etype (*secboot_hw_get_serial_num)  /** Pointer to secboot_hw_get_serial_num */
                    (uint32*  serial_num_ptr);
  secboot_hw_etype (*secboot_hw_get_is_oem_id_independent_disabled)  /** Pointer to secboot_hw_get_serial_num */
                    (uint32*  is_oem_id_indp_disabled);
  secboot_hw_etype (*secboot_hw_get_mrc_fuse_info)   /** Pointer to secboot_hw_get_mrc_fuse_info */
                    (uint32  code_segment, 
                     uint32* is_root_cert_enabled,
                     uint32* root_cert_total_num,
                     uint32* revocation_list,
                     uint32* activation_list);
  secboot_hw_etype (*secboot_hw_get_rsa_disable) 
                     (uint32*  rsa_disable_ptr);
  secboot_hw_etype (*secboot_hw_get_debug_disable_in_rom)
                     (uint32*  debug_disable_in_rom_ptr);
  secboot_hw_etype (*secboot_hw_get_soc_hw_version)
                     (uint32*  soc_hw_ver_family_device_num);
  secboot_hw_etype (*secboot_hw_get_oem_id)
                     (uint32*  oem_id);
  secboot_hw_etype (*secboot_hw_get_model_id)
                     (uint32*  model_id);
  secboot_hw_etype (*secboot_hw_get_jtag_id)
                     (uint32*  jtag_id);
}secboot_hw_ftbl_type;

/*----------------------------------------------------------------------------
 * Secboot Hardware Public APIs
 * -------------------------------------------------------------------------*/
/**
 * This function checks if the image associated with the code segment
 *        needs to be authenticated. If authentication is required, callers
 *        MUST authenticate the image successfully before allowing it to execute.
 *
 * @param[in]     code_segment       Code segment in SECURE_BOOTn register
 *                                   containing authentication information
 *                                   of the image. (Secboot Code Segments)
 * @param[in,out] auth_enabled_ptr   Pointer to a uint32 indicating whether
 *                                   authentication is required. Will be
 *                                   populated to 0 if authentication
 *                                   is not required, 1 if authentication
 *                                   is required.
 * @param[in]     cm_ftbl            Pointer to the function table of counter
 *                                   measure supporting functions (MUST be not
 *                                   NULL for counter measure protection, if
 *                                   it NULL, the call will fail out)
 *         
 * @return E_SECBOOT_HW_SUCCESS on success. Appropriate error code on failure. 
 *         Callers must NOT allow execution to continue if a failure is returned.
 *
 * @dependencies None
 *
 * @sideeffects  None
 *
 * @sa Security Control HDD and SWI for SECURE_BOOT fuses
 *
 */
secboot_hw_etype secboot_hw_is_auth_enabled
(
  uint32    code_segment,
  uint32*   auth_enabled_ptr,
  secboot_cm_ftbl_type* cm_ftbl
);

/**
 * This function returns the hash of the trusted root certificate for
 *        the image. The image's certificate chain must chain back to this
 *        trusted root certificate. When RoT transfer is turned off, it returns
 *        the default root of trust 0, but when RoT tranfer is turned on, it
 *        returns the root of trust 1.
 *
 *
 * @param[in]      code_segment       Code segment in SECURE_BOOTn register
 *                                    containing authentication information
 *                                    of the image. (Secboot Code Segments)
 * @param[in,out]  root_of_trust      32 byte buffer which will be
 *                                    populated with the SHA256 hash of
 *                                    the trusted root certificate.
  * @param[in]     rot_size           Size of the rot buffer
 *                         
 * @return E_SECBOOT_HW_SUCCESS on success. Appropriate error code on failure. 
 *         Callers must not allow execution to continue if a failure is returned. 
 *
 * @dependencies None
 * 
 * @sideeffects  None
 *
 * @sa Security Control HDD and SWI for SECURE_BOOT fuses
 *
 */
secboot_hw_etype SECBOOT_CHIPSET_ROT(secboot_hw_get_root_of_trust)
(
  uint32 code_segment,
  uint8* root_of_trust,
  uint32 rot_size
);

/**
 * This function returns the msm_hw_id used to authenticate the image's
 *        signature. The 64 bit msm_hw_id is comprised of the 32 bit JTAG ID
 *        (with the tapeout version in the upper 4 bits masked out) + the 32 bit
 *        OEM_ID or SERIAL_NUM value
 *
 * @param[in]      code_segment      Code segment in SECURE_BOOTn register
 *                                   containing authentication information
 *                                   of the image. (Secboot Code Segments)
 *
 * @param[in]      root_of_trust     32 bytes buffer containing the root of
 *                                   trust hash which was populate by
*                                    calling secboot_hw_get_root_of_trust()
 *
 * @param[in,out] msm_hw_id_ptr      Pointer to a uint64 which will
 *                                   be populated with the msm hardware id.
 *                                   The uint64 is allocated by the caller.
 *                         
 * @return E_SECBOOT_HW_SUCCESS on success. Appropriate error code on failure. 
 *         Callers must not allow execution to continue if a failure is returned  
 * 
 * @dependencies secboot_hw_get_root_of_trust() must have been called
 * 
 * @sideeffects  None
 *
 * @sa Security Control HDD and SWI for SECURE_BOOT fuses
 *
 */
secboot_hw_etype secboot_hw_get_msm_hw_id
(
  uint32        code_segment,
  uint64*       msm_hw_id_ptr
);

/**
 * This function returns if the serial number is to be used for authentication.
 *
 * @param[in]      code_segment             Code segment in SECURE_BOOTn register
 *                                          containing authentication information
 *                                          of the image. (Secboot Code Segments)
 * @param[in,out]  auth_use_serial_num_ptr  Pointer to a uint32 which will be
 *                                          populated with the USE_SERIAL_NUM
 *                                          fuse value. The uint32 is allocated by
 *                                          the caller.
 *                         
 * @return E_SECBOOT_HW_SUCCESS on success. Appropriate error code on failure. 
 *         Callers must not allow execution to continue if a failure is returned 
 * 
 * @dependencies secboot_hw_get_root_of_trust() must have been called
 * 
 * @sideeffects  None
 *
 * @sa Security Control HDD and SWI for SECURE_BOOT fuses
 *
 */
secboot_hw_etype secboot_hw_get_use_serial_num
(
  uint32         code_segment,
  uint32*        auth_use_serial_num_ptr
);

/**
 * This function returns the serial number of the chip.
 *                         
 * @param[in,out]  serial_num_ptr       Pointer to a uint32 which will
 *                                      be populated with the SERIAL_NUM
 *                                      fuse value. The uint32 is allocated by
 *                                      the caller.
 *
 * @return E_SECBOOT_HW_SUCCESS on success. Appropriate error code on failure. 
 *         Callers must not allow execution to continue if a failure is returned 
 * 
 * @dependencies None
 * 
 * @sideeffects  None
 *
 * @sa Security Control HDD and SWI for SECURE_BOOT fuses
 *
 */
secboot_hw_etype secboot_hw_get_serial_num
(
  uint32*  serial_num_ptr
);

/**
* This function returns the value read from the DISABLE_RSA efuse.
*
* @param[in, out]  rsa_disable_ptr  Pointer to a uint32 which will
*                                   be populated with the DISABLE_RSA
*                                   fuse value. The uint32 is allocated by
*                                   the caller. 
*
* @return E_SECBOOT_HW_SUCCESS on success. Appropriate error code on failure.
*         Caller's must not allow execution to continue if a failure is returned.
*
* @dependencies None
*
* @sideeffects  None
*
* @sa Security Control HDD and SWI for SECURE_BOOT fuses
*
*/
secboot_hw_etype secboot_hw_get_rsa_disable
(
uint32*  rsa_disable_ptr
);

/**
* This function returns the value read from the DEBUG_DISABLE_IN_ROM efuse.
*
* @param[in,out]  debug_disable_in_rom_ptr  Pointer to a uint32 which will
*                                      be populated with the DEBUG_DISABLE_IN_ROM
*                                      fuse value. The uint32 is allocated by
*                                      the caller.
*
* @return E_SECBOOT_HW_SUCCESS on success. Appropriate error code on failure.
*         Caller's must not allow execution to continue if a failure is returned.
*
* @dependencies None
*
* @sideeffects  None
*
* @sa Security Control HDD and SWI for SECURE_BOOT fuses
*
*/
secboot_hw_etype secboot_hw_get_debug_disable_in_rom
(
uint32*  debug_disable_in_rom_ptr
);

/**
 * This function returns rot fuse info from the chip
 *                         
 * @param[in]      code_segment         Code segment in SECURE_BOOTn register
 *                                      containing authentication information
 *                                      of the image. (Secboot Code Segments)
 * @param[in,out]  disable_rot_transfer       Pointer to a uint32 which will 
 *                                      be populated by fuse and indicates if
 *                                      ROT is disabled, The uint32 is 
 *                                      allocated by the caller.
 * @param[in,out]  current_sw_rot       Pointer to a uint32 which will
 *                                      be populated by fuse and indicates
 *                                      if current sw is using rot root of trust
 *                                      The uint32 is allocated by the caller.
 * @param[in,out]  sw_rot_use_serial_num       Pointer to a uint32 which will
 *                                      be populated by fuse and indicates
 *                                      if ROT_EN OU field is bound by device's
 *                                      serial num. The uint32 is allocated by
 *                                      the caller.
 *
 * @return E_SECBOOT_HW_SUCCESS on success. Appropriate error code on failure. 
 *         Callers must not allow execution to continue if a failure is returned 
 * 
 * @dependencies None
 * 
 * @sideeffects  None
 *
 * @sa Security Control HDD and SWI for SECURE_BOOT fuses
 *
 */
secboot_hw_etype secboot_hw_get_mrc_fuse_info
(
  uint32  code_segment,
  uint32* is_root_cert_enabled_ptr,
  uint32* root_cert_total_num,
  uint32* revocation_list,
  uint32* activation_list
);

/**
 * This function checks to see if the root of trust hash
 *        is blowin in the OEM_PK_HASH fuses or is in the
 *        root_of_trust_pk_hash_table.
 *
 * @param[in] code_segment        Segment of SECURE_BOOTn register holding
 *                                authentication information for the code.
 *                                (Secboot Code Segments)
 *
 * @param[in,out] is_hash_in_fuse_ptr  Pointer to uint32 that will be
 *                                populated with the fuse value. 0 means
 *                                hash is in the QTI root of trust
 *                                table.
 *                         
 * @return E_SECBOOT_HW_SUCCESS on success. Appropriate error code on failure.
 *
 * @dependencies None
 * 
 * @sideeffects  None
 *
 * @see Security Control HDD for SECURE_BOOT fuses
 * 
 */
secboot_hw_etype secboot_hw_get_is_hash_in_fuse(
  uint32 code_segment,
  uint32* is_hash_in_fuse_ptr
);

/**
 * @brief This function retrieves the index in the Qualcomm Root of Trust 
 *        table, that needs to be used for authentication
 *
 * @param code_segment       [in] Segment of SECURE_BOOTn register holding
 *                                authentication information for the code
 *
 * @param hash_index_ptr     [in,out] Pointer to uint32 that will be
 *                                populated with the index. uint32 is
 *                                allocated by the caller
 *                         
 * @return E_SECBOOT_HW_SUCCESS on success. Appropriate error code on failure.
 *
 * @see Security Control HDD for SECURE_BOOT fuses
 * 
 */
secboot_hw_etype secboot_hw_get_pk_hash_index
(
  uint32 code_segment,
  uint32* hash_index_ptr
);

/**
 * @brief This function reads device soc hw version
 *
 * @param soc_hw_ver_family_device_num     [in,out] Pointer soc hw version
 *                         
 * @return E_SECBOOT_HW_SUCCESS on success. Appropriate error code on failure.
 *
 * @see Security Control HDD for SECURE_BOOT fuses
 * 
 */
secboot_hw_etype secboot_hw_get_soc_hw_version
(
  uint32* soc_hw_ver_family_device_num
);

/**
 * @brief This function reads device's oem_id
 *
 * @param oem_id     [in,out] Pointer to oem_id
 *                         
 * @return E_SECBOOT_HW_SUCCESS on success. Appropriate error code on failure.
 *
 * @see Security Control HDD for SECURE_BOOT fuses
 * 
 */
secboot_hw_etype secboot_hw_get_oem_id
(
  uint32* oem_id
);

/**
 * @brief This function reads device's model_id
 *
 * @param model_id     [in,out] Pointer to model_id
 *                         
 * @return E_SECBOOT_HW_SUCCESS on success. Appropriate error code on failure.
 *
 * @see Security Control HDD for SECURE_BOOT fuses
 * 
 */
secboot_hw_etype secboot_hw_get_model_id
(
  uint32* model_id
);

/**
 * @brief This function reads device's jtag_id
 *
 * @param jtag_id     [in,out] Pointer to jtag_id
 *                         
 * @return E_SECBOOT_HW_SUCCESS on success. Appropriate error code on failure.
 *
 * @see Security Control HDD for SECURE_BOOT fuses
 * 
 */
secboot_hw_etype secboot_hw_get_jtag_id
(
  uint32* jtag_id
);

/**
 * @brief This function reads use_legacy_sig fuse
 *
 * @param use_legacy_sig     [in,out] Pointer to use_legacy_sig variable
 *                         
 * @return E_SECBOOT_HW_SUCCESS on success. Appropriate error code on failure.
 *
 * @see Security Control HDD for SECURE_BOOT fuses
 * 
 */
secboot_hw_etype secboot_hw_get_auth_use_legacy_sig
(
uint32*  use_legacy_sig
);

/** @}
*/
#endif //SECBOOT_HW_H
