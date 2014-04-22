#ifndef SECBOOT_CHIPSET_I_H
#define SECBOOT_CHIPSET_I_H

/**
@file secboot_chipset.h
@brief Define functions or parameters for particular chipset
*/

/*===========================================================================
   Copyright (c) 2017 Qualcomm Technologies, Inc.
   All rights reserved.
   Qualcomm Technologies, Inc. Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/core.mpss/3.10/securemsm/secboot/inc/secboot_chipset_i.h#1 $
  $DateTime: 2018/11/08 03:29:06 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
02/16/17   hw       Init Version 

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "secboot_env_i.h" // inter included
/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants (Used by callers to
 * implement CounterMeasure)
 * -------------------------------------------------------------------------*/
/**
* @brief This macro converts chipset specific function name
*/
#define SECBOOT_CHIPSET_SPECIFIC(func) func ##_chipset

/**
* @brief This macro converts RoT related function name
*/
#define SECBOOT_CHIPSET_ROT(func) func ##_sha384_rot
/*----------------------------------------------------------------------------
 * Definitions for secboot functions
 * -------------------------------------------------------------------------*/
/**
* @brief define hw function to get RoT pk hash
*/
#define secboot_hw_get_root_of_trust_chipset \
	SECBOOT_CHIPSET_ROT(secboot_hw_get_root_of_trust)

/**
* @brief define sub cert fingerprint
*/
#define secboot_subca_cert_fingerprint_chipset \
	SECBOOT_CHIPSET_ROT(secboot_subca_cert_fingerprint)

/**
* @brief define the size of subca cert fingerprints  
*/
#define secboot_subca_cert_fingerprint_size_chipset \
	SECBOOT_CHIPSET_ROT(secboot_subca_cert_fingerprint_size)

/**
* @brief define root cert fingerprint
*/
#define secboot_root_cert_fingerprint_chipset \
	SECBOOT_CHIPSET_ROT(secboot_root_cert_fingerprint)

/**
* @brief define the size of root cert fingerprints  
*/
#define secboot_root_cert_fingerprint_size_chipset \
	SECBOOT_CHIPSET_ROT(secboot_root_cert_fingerprint_size)

/**
* @brief define the number of fingerprints of the cert chains
*/
#define secboot_cert_fingerprints_num_chipset \
	SECBOOT_CHIPSET_ROT(secboot_cert_fingerprints_num)

/**
* @brief This function read OTP xbl_sec auth disable
*/
#define secboot_read_feature_config_xbl_sec_auth_disable_chipset() \
 	HWIO_INF( FEATURE_CONFIG9, XBL_SEC_AUTH_DISABLE )

/**
* @brief This function read OTP root of trust fuses
*/
#define secboot_read_pk_hash_0_hash_data_chipset() \
    HWIO_INF(PK_HASH0_0, HASH_DATA0) 

#define secboot_read_pk_hash_1_hash_data_chipset() \
    HWIO_INF(PK_HASH0_1, HASH_DATA0)

#define secboot_read_pk_hash_2_hash_data_chipset() \
    HWIO_INF(PK_HASH0_2, HASH_DATA0) 

#define secboot_read_pk_hash_3_hash_data_chipset() \
    HWIO_INF(PK_HASH0_3, HASH_DATA0)

#define secboot_read_pk_hash_4_hash_data_chipset() \
    HWIO_INF(PK_HASH0_4, HASH_DATA0) 

#define secboot_read_pk_hash_5_hash_data_chipset() \
    HWIO_INF(PK_HASH0_5, HASH_DATA0)

#define secboot_read_pk_hash_6_hash_data_chipset() \
    HWIO_INF(PK_HASH0_6, HASH_DATA0) 

#define secboot_read_pk_hash_7_hash_data_chipset() \
    HWIO_INF(PK_HASH0_7, HASH_DATA0)

#define secboot_read_pk_hash_8_hash_data_chipset() \
    HWIO_INF(PK_HASH0_8, HASH_DATA0) 

#define secboot_read_pk_hash_9_hash_data_chipset() \
    HWIO_INF(PK_HASH0_9, HASH_DATA0) 

#define secboot_read_pk_hash_10_hash_data_chipset() \
    HWIO_INF(PK_HASH0_10, HASH_DATA0) 

#define secboot_read_pk_hash_11_hash_data_chipset() \
    HWIO_INF(PK_HASH0_11, HASH_DATA0) 

/**
* @brief This function read OTP debug disable in rom
*/
#define secboot_read_oem_config_debug_disable_in_rom_chipset() \
    HWIO_INF(OEM_CONFIG0, DEBUG_DISABLE_IN_ROM)

/**
* @brief This function read OTP debug disable in rom
*/
#define secboot_read_oem_config_disable_rsa_chipset() \
    HWIO_INF(OEM_CONFIG2, DISABLE_RSA);
	
/**
* @brief These functions read oem id and model id
*/
#define secboot_read_oem_id_oem_id_chipset() \
    HWIO_INF( OEM_ID, OEM_ID )

#define secboot_read_oem_id_model_id_chipset() \
    HWIO_INF( OEM_ID, OEM_PRODUCT_ID )

/**
* @brief This function reads use legacy sig fuse
*/
#define secboot_read_use_legacy_sig_chipset() \
   (0)

#endif /* SECBOOT_CHIPSET_I_H */
