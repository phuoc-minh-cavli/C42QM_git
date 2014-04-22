/**
 * Copyright 2015 QUALCOMM Technologies, Inc.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 */

#ifndef TZBSP_OEM_H
#define TZBSP_OEM_H

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <stdbool.h>
#include "comdef.h"

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
typedef struct {
  uint32 wQseeFatalErrFlag;
  uint32 wOemFatalErrFlag;
} tzbsp_reset_reason_pair_t;

typedef struct {
  uint32 dwCount;
  tzbsp_reset_reason_pair_t *rst_reason;
} tzbsp_cfg_oem_reset_reason_t;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
 * @brief Check if default application sandboxing is enabled
 *
 * @return true if sandbox by default is enabled, false otherwise
*/
bool oem_app_sandbox_default(void);


/**
 * Checks device config for property that can be used to bypass
 * meltdown security vulnerability mitigation on affected targets.
 *
 * @return true if meltdown mitigation should be bypassed
 */
bool oem_bypass_meltdown_mitigation(void);

/** Checks whether crash dump is enable before anti-rollback
 *  fuse is blown in DevCfg binary blob.
 *
 *  @retval true if allowed. false otherwise
 */
bool oem_is_dump_enable_before_arb(void);

/* Get activation and revocation list info from devcfg */
int tzbsp_oem_get_mrc_fuses(uint32* mrc_activation_list, uint32* mrc_revocation_list);

/** Read the value for enable counter measure property
 *  from devcfg.
 *
 *  @retval false if disabled true otherwise.
 */
bool oem_enable_counter_measure(void);

/* get public mod data (little endian) from devcfg */
int tzbsp_oem_get_pub_key(uint8** pub_mod_ptr, uint32* pub_mod_len, uint8** pub_exp_ptr, uint32* pub_exp_len);

int tzbsp_oem_get_decryption_key(uint8 *l1_key);

int tzbsp_oem_uie_encr_key1_selector(uint8 *oem_decryption_key_selector);

/* Get the value of PCIE PERST assertion detection timeout */
uint32_t tzbsp_oem_get_pcie_perst_timeout(void);
bool tzbsp_counter_enable_rpmb(void);

/** return enablement status for exception hooks */
bool tzbsp_oem_disable_exception_hooks(void);

/* For OEM to enable/disable the ARM Gold core Errata *** Specific to Hana Gold cores ***/
bool tzbsp_oem_image_ARM_Errata_enable(uint32_t errata_num);

/**Get value for logging level */
uint32_t oem_get_logging_level(void);

/** Checks whether the feature to support boot up from 
 *  a/b partition is enabled
 *
 *  @retval true if enabled. false otherwise
 */
bool oem_is_bootup_from_a_b_partition_supported(void);

#endif // TZBSP_OEM_H