#ifndef PM_SMB_H
#define PM_SMB_H

/**
 * Copyright (c) 2018 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 * Export of this technology or software is regulated by the U.S. Government.
 * Diversion contrary to U.S. law prohibited.
*
 * All ideas, data and information contained in or disclosed by
 * this document are confidential and proprietary information of
 * Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
 * By accepting this material the recipient agrees that this material
 * and the information contained therein are held in confidence and in
 * trust and will not be used, copied, reproduced in whole or in part,
 * nor its contents revealed in any manner to others without the express
 * written permission of Qualcomm Technologies Incorporated.
*
*/

/**
 * @file pm_smb.h  External charger SMB Module related declaration.
 *
 * @brief This file contains functions and variable declarations to support
 *   the SMB module.
 */

/*===========================================================================

                        EDIT HISTORY FOR FILE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/05/18   sv     Implement SMB driver
===========================================================================*/

#include "I2cError.h"
#include "pm_err_flags.h"


enum{
    SMB_NO_CHARGING,
    SMB_PRE_CHARGING,
    SMB_FAST_CHARGING,
    SMB_TAPER_CHARGING ,
    SMB_INVALID_CHARGING
};

typedef enum {
    SMB_LOW_BATT_THRESHOLD_NONE,
    SMB_LOW_BATT_THRESHOLD_2P5V,
    SMB_LOW_BATT_THRESHOLD_2P6V,
    SMB_LOW_BATT_THRESHOLD_2P7V,
    SMB_LOW_BATT_THRESHOLD_2P8V,
    SMB_LOW_BATT_THRESHOLD_2P9V,
    SMB_LOW_BATT_THRESHOLD_3V,
    SMB_LOW_BATT_THRESHOLD_3P1V,
    SMB_LOW_BATT_THRESHOLD_3P7V,
    SMB_LOW_BATT_THRESHOLD_2P88V,
    SMB_LOW_BATT_THRESHOLD_3P0V,
    SMB_LOW_BATT_THRESHOLD_3P10V,
    SMB_LOW_BATT_THRESHOLD_3P25V,
    SMB_LOW_BATT_THRESHOLD_3P35V,
    SMB_LOW_BATT_THRESHOLD_3P46V,
    SMB_LOW_BATT_THRESHOLD_3P58V,
    SMB_LOW_BATT_THRESHOLD_INVALID
}pm_smb_batif_low_battery_threshold;

typedef enum
{
  PM_SMB_APSD_RESULT_STATUS_ACA_DOCK,
  PM_SMB_APSD_RESULT_STATUS_ACA_C_CHARGER,
  PM_SMB_APSD_RESULT_STATUS_ACA_B_CHARGER,
  PM_SMB_APSD_RESULT_STATUS_ACA_A_CHARGER,
  PM_SMB_APSD_RESULT_STATUS_SDP_CHARGER,
  PM_SMB_APSD_RESULT_STATUS_OCP_CHARGER,
  PM_SMB_APSD_RESULT_STATUS_DCP_CHARGER,
  PM_SMB_APSD_RESULT_STATUS_CDP_CHARGER,
  PM_SMB_APSD_RESULT_STATUS_INVALID,
}pm_smb_apsd_result_status_type;

typedef enum {
    SMB_AC_ICL_MAX_500MA,
    SMB_AC_ICL_MAX_685MA,
    SMB_AC_ICL_MAX_1000MA,
    SMB_AC_ICL_MAX_1100MA,
    SMB_AC_ICL_MAX_1200MA,
    SMB_AC_ICL_MAX_1300MA,
    SMB_AC_ICL_MAX_1500MA,
    SMB_AC_ICL_MAX_1600MA,
    SMB_AC_ICL_MAX_1700MA,
    SMB_AC_ICL_MAX_1800MA,
    SMB_AC_ICL_MAX_2000MA,
    SMB_AC_ICL_MAX_2200MA,
    SMB_AC_ICL_MAX_2500MA,
    SMB_AC_ICL_MAX_3000MA,
    SMB_AC_ICL_MAX_3500MA,
    SMB_AC_ICL_MAX_3940MA,
    SMB_AC_ICL_MAX_MAX = SMB_AC_ICL_MAX_3940MA,
}pm_smb_ac_icl_max;


/*
 * Initialization SMB charger chip. 
 *
 * @enable: Whether to enable charging function in SMB chip
 * @return PM_ERR_FLAG__I2C_OPT_ERR if I2C error happened
 *         PM_ERR_FLAG__SUCCESS if success
 */

pm_err_flag_type pm_smb_init(void);

pm_err_flag_type pm_smb_deinit(void);

/**
* @brief This function Gets the LowBatt threshold status
*
* @details
*
* @param[out] LowBatt           LowBattStatus
*
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smb_get_low_bat_threshold_status(boolean *LowBatt);

/**
* @brief This function Gets the battMissing status
*
* @details
*
* @param[out] battMissing           battMissing Status
*
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smb_get_batt_missing_status(boolean *battMissing);

/**
* @brief This function Gets the ChargeExist status
*
* @details
*
* @param[out] ChargeExist           ChargeExist Status
*
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smb_get_charger_exist_status(boolean *ChargeExist);

/**
* @brief This function Gets the chargingProgressStatus status
*
* @details
*
* @param[out] chargingProgressStatus           chargingProgressStatus Status
*
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smb_get_charging_status(boolean *chargingProgressStatus);

/**
* @brief This function Sets the Suspend status
*
* @details
*
* @param[out] Suspend           Suspend Status
*
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smb_cfg_suspend(boolean Suspend);

/**
* @brief This function enables charging
*
* @details
*
* @param[in] ChargeEnable           Enable/disable charging
*
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smb_enable_charging(boolean ChargeEnable);


/**
* @brief This function reads the APSD done status
*
* @details
*
* @param[out] result           APSD done status
*
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smb_get_apsd_status(boolean *result);


/**
* @brief This function reads the APSD result
*
* @details
*
* @param[out] apsd_result           APSD result
*
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smb_get_apsd_result_status(pm_smb_apsd_result_status_type *apsd_result);


/**
* @brief This function programs the ICL 
*
* @details
*
* @param[in] icl_max           ICL max to be programmed
*
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smb_set_usbin_current_limit_cfg(pm_smb_ac_icl_max icl_max);


/**
* @brief This function reads the USBIN status ( VBUS detect) 
*
* @details
*
* @param[out] Valid           If valid input is present or not
*
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smb_get_usbin_status(boolean *Valid);

/**
* @brief This function Sets the vBatLowThreshold
*
* @details
*
* @param[out] vBatLowThreshold           vBatLowThreshold Value
*
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smb_set_low_bat_threshold (pm_smb_batif_low_battery_threshold vBatLowThreshold);


/**
* @brief This function Initializes the SMB Driver
*
* @details
*
* @param[in] void           None
*
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_sbl_i2c_smb_exit(void);

/**
* @brief This function exits the SMB Driver
*
* @details
*
* @param[in] void           None
*
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_sbl_i2c_smb_init(void);

/**
* @brief This function Reads the SMB registers through I2C driver
*
* @details
*
* @param[in] slaveAddress   Slave Address of SMB device
* @param[in] offset         Register Offset
* @param[in] data           Data to write to SMB
*
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type i2c_smb_read(uint8 slaveAddress, uint8 offset, uint8 *data);

/**
* @brief This function Writes the SMB registers through I2C driver
*
* @details
*
* @param[in] slaveAddress   Slave Address of SMB device
* @param[in] offset         Register Offset
* @param[in] mask           Register mask
* @param[out] data           Data to Read from SMB
*
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type i2c_smb_write_mask(uint8 slaveAddress, uint8 offset, uint8 mask,  uint8 data);

#endif /*PM_SMB_H*/

