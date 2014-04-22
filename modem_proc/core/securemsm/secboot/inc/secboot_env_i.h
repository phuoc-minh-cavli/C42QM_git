#ifndef SECBOOT_ENV_I_H
#define SECBOOT_ENV_I_H

/**
@file secboot_env.h
@brief Define functions or parameters for particular environment
*/

/*===========================================================================
   Copyright (c) 2016 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/core.mpss/3.10/securemsm/secboot/inc/secboot_env_i.h#2 $
  $DateTime: 2019/04/19 04:30:18 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
12/16/16   hw       Init Version 

===========================================================================*/

/**
 * @addtogroup SecbootCounterMeasure
 *
 * @{
 */
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "secboot_env.h"
#include "secboot_chipset_i.h"

/*----------------------------------------------------------------------------
 * Paramater Macro Definition
 * -------------------------------------------------------------------------*/
/* Value to mask out the MSM HW Revision from JTAG_ID */
#define HW_REVISION_AUTHENTICATION_MASK 0x0FFFFFFFUL
/*----------------------------------------------------------------------------
 * Function Definitions
 * -------------------------------------------------------------------------*/
/*
 * @brief get time stamp for kpi data
 */
#define secboot_tmr_get_timestamp()   (0)

/* env specific function */
#define SECBOOT_ENV_SPECIFIC_FUNC(func) func ##_env

/* env specific function */
#define SECBOOT_STUB_FUNC(func) func ##_stub

/**
* @brief This function verifies signatures for public key authentication
*/
#define secboot_verify_signature_env  secboot_verify_signature_legacy

/**
* @brief This function verifies signatures for public key authentication
*/
#define ModExp_env  ModExp

/**
* @brief this macro define signing algorithm that this secboot supports
*/
#define SECBOOT_IS_SIG_ALGO_SUPPORTED(algo) (     \
  ( (algo == SECX509_RSAPSSEncryption) ||   \
       (algo == SECX509_ecdsaWithSHA384)            \
  ))

/**
* @brief this macro define situations that use Legacy sig (QC HMAC)
*        OEM signing could use legacy signature, but production signing
*        never uses.
*        
*/
#define SECBOOT_AUTH_USE_LEGACY_SIG(context_ptr) (     \
  (SECBOOT_CHECK_CODE_SEGMENT_VALID(context_ptr->code_segment)) && \
  (context_ptr->fuses.auth_use_legacy_sig == SECBOOT_LEGACY_SIG_ENABLED) \
  ) 

/**
* @brief pkcs is not allowed, and stub out the pkcs sig verificaiton
*/
#define secboot_verify_signature_RSA_PKCS1_v15 SECBOOT_STUB_FUNC(secboot_verify_signature_RSA_PKCS1_v15)

/**
* @brief This function reads OEM ID
*/
#define secboot_read_oem_id() HWIO_IN( OEM_ID )

/**
* @brief This function reads pk hash in fuse
*/
#define secboot_read_secure_bootn_pk_hash_in_fuse(code_segment) \
	HWIO_INFI( SECURE_BOOTn, code_segment, PK_HASH_IN_FUSE)

/**
* @brief This function reads pk hash index
*/
#define secboot_read_secure_bootn_rom_pk_hash_index(code_segment) \
	HWIO_INFI( SECURE_BOOTn, code_segment, ROM_PK_HASH_INDEX )

/**
* @brief This function reads pk hash index
*/
#define secboot_read_secure_bootn_use_serial_num(code_segment) \
	HWIO_INFI( SECURE_BOOTn, code_segment, USE_SERIAL_NUM );

/**
* @brief This function reads auth enable
*/
#define secboot_read_secure_bootn_auth_en(code_segment) \
	HWIO_INFI( SECURE_BOOTn, code_segment, AUTH_EN )

/**
* @brief This function reads xbl-sec auth disable
*/
#define secboot_read_feature_config_xbl_sec_auth_disable() \
	SECBOOT_CHIPSET_SPECIFIC(secboot_read_feature_config_xbl_sec_auth_disable)()

/**
* @brief This function reads xbl-sec auth disable
*/
#define secboot_read_boot_config_force_msa_auth_en() \
	(0)

/**
* @brief This function reads pk hash
*/
#define secboot_read_pk_hash_0_hash_data() \
	SECBOOT_CHIPSET_SPECIFIC(secboot_read_pk_hash_0_hash_data)()

#define secboot_read_pk_hash_1_hash_data() \
	SECBOOT_CHIPSET_SPECIFIC(secboot_read_pk_hash_1_hash_data)()

#define secboot_read_pk_hash_2_hash_data() \
	SECBOOT_CHIPSET_SPECIFIC(secboot_read_pk_hash_2_hash_data)()

#define secboot_read_pk_hash_3_hash_data() \
	SECBOOT_CHIPSET_SPECIFIC(secboot_read_pk_hash_3_hash_data)()

#define secboot_read_pk_hash_4_hash_data() \
	SECBOOT_CHIPSET_SPECIFIC(secboot_read_pk_hash_4_hash_data)()

#define secboot_read_pk_hash_5_hash_data() \
	SECBOOT_CHIPSET_SPECIFIC(secboot_read_pk_hash_5_hash_data)()

#define secboot_read_pk_hash_6_hash_data() \
	SECBOOT_CHIPSET_SPECIFIC(secboot_read_pk_hash_6_hash_data)()

#define secboot_read_pk_hash_7_hash_data() \
	SECBOOT_CHIPSET_SPECIFIC(secboot_read_pk_hash_7_hash_data)()

#define secboot_read_pk_hash_8_hash_data() \
	SECBOOT_CHIPSET_SPECIFIC(secboot_read_pk_hash_8_hash_data)()

#define secboot_read_pk_hash_9_hash_data() \
	SECBOOT_CHIPSET_SPECIFIC(secboot_read_pk_hash_9_hash_data)()

#define secboot_read_pk_hash_10_hash_data() \
	SECBOOT_CHIPSET_SPECIFIC(secboot_read_pk_hash_10_hash_data)()

#define secboot_read_pk_hash_11_hash_data() \
	SECBOOT_CHIPSET_SPECIFIC(secboot_read_pk_hash_11_hash_data)()
	
/**
* @brief This function reads jtag id
*/
#define secboot_read_jtag_id() \
	HWIO_INM(JTAG_ID, HW_REVISION_AUTHENTICATION_MASK)

/**
* @brief This function reads serial num
*/
#define secboot_read_serial_num() \
	HWIO_INF(SERIAL_NUM , SERIAL_NUM )

/**
* @brief This function reads mrc total root num
*/
#define secboot_read_oem_config_mrc_total_root_num() \
	HWIO_INF(OEM_CONFIG1, ROOT_CERT_TOTAL_NUM)

/**
* @brief This function reads mrc root activation list
*/
#define secboot_read_mrc_activation_list() \
	HWIO_INF(MRC_2_0_0, ROOT_CERT_ACTIVATION_LIST)

/**
* @brief This function reads mrc root revocation list
*/
#define secboot_read_mrc_revocation_list() \
	HWIO_INF(MRC_2_0_2, ROOT_CERT_REVOCATION_LIST)

/**
* @brief This function reads debug disable in rom
*/
#define secboot_read_oem_config_debug_disable_in_rom() \
	SECBOOT_CHIPSET_SPECIFIC(secboot_read_oem_config_debug_disable_in_rom)()

/**
* @brief This function reads disable rsa
*/
#define secboot_read_oem_config_disable_rsa() \
	SECBOOT_CHIPSET_SPECIFIC(secboot_read_oem_config_disable_rsa)()

/**
* @brief This function reads debug disable in rom
*/
#define secboot_read_oem_id_independent_disable() (0)

/**
* @brief This function reads soc hw version, defined in secfuses
*/
#define secboot_read_soc_hw_family_num() (HWIO_INM(TCSR_SOC_HW_VERSION, HWIO_FMSK(TCSR_SOC_HW_VERSION, FAMILY_NUMBER)))

/**
* @brief This function reads soc hw version, defined in secfuses
*/
#define secboot_read_soc_hw_device_num() (HWIO_INM(TCSR_SOC_HW_VERSION, HWIO_FMSK(TCSR_SOC_HW_VERSION, DEVICE_NUMBER)))

/**
* @brief This function reads OEM ID
*/
#define secboot_read_oem_id_oem_id() \
	SECBOOT_CHIPSET_SPECIFIC(secboot_read_oem_id_oem_id)()

/**
* @brief This function reads Model ID
*/
#define secboot_read_oem_id_model_id() \
	SECBOOT_CHIPSET_SPECIFIC(secboot_read_oem_id_model_id)()

/**
* @brief This function reads use legacy sig fuse
*/
#define secboot_read_use_legacy_sig() \
	SECBOOT_CHIPSET_SPECIFIC(secboot_read_use_legacy_sig)()

#endif /* SECBOOT_ENV_I_H */
