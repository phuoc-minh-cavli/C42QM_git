#ifndef __PM_PON_H__
#define __PM_PON_H__
/*! \file pm_pon.h
 *  \n
 *  \brief This file contains functions and variable declarations to support 
 *   the pon driver module.
 *
 *  Copyright (c) 2012-2017, 2019, 2023 Qualcomm Technologies, Inc. All rights reserved.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/25/19   abh     Added file for Tiny Modem image in Twizy
===========================================================================*/
/*===========================================================================

                        HEADER FILES

===========================================================================*/
#include "pm_err_flags.h"
#include "com_dtypes.h"
#include "pm_resources_and_types.h"
#include "pm_irq.h"
/*===========================================================================
                        TYPE DEFINITIONS 
===========================================================================*/

#define PSM_USB_PON_DISABLE    0
#define PSM_USB_PON_ENABLE     1

/*! \enum pm_pon_reset_cfg_type
   \brief Reset configure type.
 */
typedef enum
{
  PM_PON_RESET_CFG_WARM_RESET,
  /* Shutdown + Auto pwr up */
  PM_PON_RESET_CFG_HARD_RESET,
  /*Normal shutdown*/
  PM_PON_RESET_CFG_NORMAL_SHUTDOWN,  
  /* Main Batt and coin cell remove Shutdown + Auto pwr up 8: DVDD_HARD_RESET */
  PM_PON_RESET_CFG_D_VDD_BATT_REMOVE_HARD_RESET,
  /* Shutdown to a state of main battery removal */
  PM_PON_RESET_CFG_D_VDD_BATT_REMOVE_SHUTDOWN,
   /* Shutdown to a state of main battery and coin cell removal*/
  PM_PON_RESET_CFG_X_VDD_COIN_CELL_REMOVE_SHUTDOWN,  
  PM_PON_RESET_CFG_X_VDD_COIN_CELL_REMOVE_HARD_RESET,
  PM_PON_RESET_CFG_IMMEDIATE_X_VDD_COIN_CELL_REMOVE_SHUTDOWN,
  /* Warm Reset and Main Batt/coin cell remove Shutdown */
  PM_PON_RESET_CFG_WARM_RESET_THEN_D_VDD_BATT_REMOVE_SHUTDOWN,
  PM_PON_RESET_CFG_WARM_RESET_THEN_X_VDD_COIN_CELL_REMOVE_SHUTDOWN,
  PM_PON_RESET_CFG_WARM_RESET_THEN_SHUTDOWN,
  /* Warm Reset then Main Batt/coin cell remove Shutdown + Auto pwr up */
  PM_PON_RESET_CFG_WARM_RESET_THEN_HARD_RESET,
  PM_PON_RESET_CFG_WARM_RESET_THEN_D_VDD_BATT_REMOVE_HARD_RESET,
  PM_PON_RESET_CFG_WARM_RESET_THEN_X_VDD_COIN_CELL_REMOVE_HARD_RESET,
  PM_PON_RESET_CFG_INVALID  
}pm_pon_reset_cfg_type;

/*! \enum pmapp_ps_hold_cfg_type
   \brief PS_HOLD reset configure type.
 */
typedef enum
{
  PM_APP_PON_CFG_WARM_RESET,
  PM_APP_PON_CFG_HARD_RESET,
  PM_APP_PON_CFG_NORMAL_SHUTDOWN,
  PM_APP_PON_CFG_DVDD_HARD_RESET,
  PM_APP_PON_CFG_DVDD_SHUTDOWN,
  PM_APP_PON_CFG_MAX
}pm_app_pon_reset_cfg_type;

/*! \enum pm_app_misc_cfg_type
   \brief :to configure the PMIC misc register which controls the 
   	Tiny/full Modem loading and PSM mode.
 */

typedef enum
{
  PM_APP_MISC_CFG_BOOT_OPTION,
  PM_APP_MISC_CFG_PSM,
  PM_APP_MISC_CFG_MAX
}pm_app_misc_cfg_type;

/*! \enum pm_pon_trigger_type
   \brief Power On Triggers
 */
typedef enum 
{
   /*! 1 - Trigger from SMPL.                  */
   PM_PON_TRIGGER_SMPL,
   /*! 2 - Trigger from RTC.                   */
   PM_PON_TRIGGER_RTC,
   /*! 3 - Trigger from DC Charger.            */
   PM_PON_TRIGGER_DC_CHG,
   /*! 4 - Trigger from USB Charger.           */
   PM_PON_TRIGGER_USB_CHG,
   /*! 5 - Trigger from PON1.                  */
   PM_PON_TRIGGER_PON1,
   /*! 6 - Trigger by CBL_PWR   .              */
   PM_PON_TRIGGER_CBLPWR,
   /*! 7 - Trigger by Keypad.                  */
   PM_PON_TRIGGER_KPDPWR,
   PM_PON_TRIGGER_INVALID
}pm_pon_trigger_type;

/*! \enum pm_pon_irq_type
   \brief different types of irq bit fields of pon irq module
 */
typedef enum {
  PM_PON_IRQ_KPDPWR_ON          = 0,  
  PM_PON_IRQ_RESIN_ON           = 1,  
  PM_PON_IRQ_CBLPWR_ON          = 2,  
  PM_PON_IRQ_KPDPWR_BARK        = 3,  
  PM_PON_IRQ_RESIN_BARK         = 4,  
  PM_PON_IRQ_KPDPWR_RESIN_BARK  = 5,  
  PM_PON_IRQ_PMIC_WD_BARK       = 6,  
  PM_PON_IRQ_SOFT_RESET 	      = 7,
  /*INVALID*/
  PM_PON_IRQ_INVALID
}pm_pon_irq_type;
  
  
typedef struct
{
  uint8 pon_data[9];
  uint8 AON_data[4];
  uint8 pmic_version[3];
}pm_pon_debug_type;
/*===========================================================================
                        FUNCTION PROTOTYPE
===========================================================================*/
/**
 * @brief Configure MSM PS_HOLD behavior.
 *  
 * @details Configure PMIC to act on MSM PS_HOLD state
 * 
 * @param pmic_device_index When the target has more than
 *          one pmic, this is the index of the PMIC in which
 *          the power on module is physically located. The device
 *          index starts at zero.
 * @param ps_hold_cfg 
 *          PM_PON_RESET_CFG_WARM_RESET,
 *          PM_PON_RESET_CFG_NORMAL_SHUTDOWN,
 *          PM_PON_RESET_CFG_D_VDD_BATT_REMOVE_SHUTDOWN, Shutdown + dVdd_rb remove main battery
 *          PM_PON_RESET_CFG_X_VDD_COIN_CELL_REMOVE_SHUTDOWN, Shutdown + xVdd_rb remove main and coin cell battery
 *          PM_PON_RESET_CFG_HARD_RESET,     Shutdown + Auto pwr up
 *   
 *                   
 * @return pm_err_flag_type
 */
pm_err_flag_type
pm_pon_ps_hold_cfg(uint32 pmic_device_index, pm_pon_reset_cfg_type ps_hold_cfg);

/**
 * @brief Configures PMIC to act on MSM PS_HOLD toggle. This
 *        is an app level API which handles all the required
 *        PS_HOLD config for all the applicable PMICs
 *        internally.
 *  
 * @details Configure PMIC to act on MSM PS_HOLD state. 
 *  
 * @param app_pshold_cfg : Supported configurations: 
 *                               PM_APP_PON_CFG_WARM_RESET,
 *                               PM_APP_PON_CFG_HARD_RESET,
 *                               PM_APP_PON_CFG_NORMAL_SHUTDOWN
 *   
 *                   
 * @return pm_err_flag_type
 *          PM_ERR_FLAG_INVALID_PARAMETER
 *          PM_ERR_FLAG_SUCCESS
 */
pm_err_flag_type
pm_app_pon_pshold_cfg(pm_app_pon_reset_cfg_type app_pshold_cfg);

/**
 * @brief gets the Configurations of PMIC MISC_MISC_PSM , MISC_MISC_MDM_BOOT_OPTION registers  
 *        
 * @details  to get the pmic misc_psm, misc_boot_option register data
 *           MISC_MISC_PSM is one bit register [0]
 *           MISC_MISC_MDM_BOOT_OPTION is 8 bit register
 * @param pm_app_misc_cfg_type 
 *
 * @return pm_err_flag_type
 *          PM_ERR_FLAG__INVALID
 *          PM_ERR_FLAG_SUCCESS
 */

pm_err_flag_type
pm_app_misc_get_cfg(pm_app_misc_cfg_type app_misc_cfg, uint8 *data);

/**
 * @brief Configures PMIC MISC_MISC_PSM, MISC_MISC_MDM_BOOT_OPTION registers  
 *        
 * @details  to set the pmic misc_psm, misc_boot_option register data
 *           MISC_MISC_PSM is one bit register [0]
 *           MISC_MISC_MDM_BOOT_OPTION is 8 bit register [7:0]
 * @param pm_app_misc_cfg_type
 *
 * @return pm_err_flag_type
 *          PM_ERR_FLAG__INVALID
 *          PM_ERR_FLAG_SUCCESS
 */

pm_err_flag_type
pm_app_misc_set_cfg(pm_app_misc_cfg_type app_misc_cfg, uint8 mask, uint8 data);

/**
* @brief This function allows the user to enable/disable power on trigger
*       
*
* @param[in]  pmic_device_index: Device index of PMIC chip. 0 for primary, 1 for secondary and so on.
* @param[in]  trigger: PON trigger. Refer enum#pm_pon_trigger_type for more info
* @param[in]  enable:  Enable/disable trigger for PON
*
* @return PM_ERR_FLAG__SUCCESS                = The operation was successful.
*          PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Parameter 1 is out of range.
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not supported
*          PM_ERR_FLAG__SPMI_OPT_ERR          = The SPMI driver failed to
*                                               communicate with the PMIC.
*
*/
pm_err_flag_type 
pm_pon_trigger_enable(uint32 pmic_device_index, pm_pon_trigger_type trigger, boolean enable);

/**
 * @brief This function enables or disables pon irq
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0 Secondary PMIC: 1
 * @param[in] irq:  
 *                PON irq type. Refer enum pm_pon_irq_type.
 *  
 * @param[in] enable:  
 *                TRUE: Enable corresponding PON interrupt 
 *                FALSE:Disable corresponding PON interrupt
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__PAR1_OUT_OF_RANGE = Device Index out of
 *          range.
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *
 */
pm_err_flag_type 
pm_pon_irq_enable(uint8 pmic_device_index, pm_pon_irq_type irq, boolean enable);


/**
 * @brief This function clears the PON irq 
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0 Secondary PMIC: 1
 * @param[in] irq:  
 *                PON irq type. Refer enum pm_pon_irq_type.  
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__PAR1_OUT_OF_RANGE = Device Index out of
 *          range.
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *
 */
pm_err_flag_type 
pm_pon_irq_clear(uint8  pmic_device_index, pm_pon_irq_type irq);

/**
 * @brief This function configures the PON irq trigger 
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0 Secondary PMIC: 1
 * @param[in] irq:  
 *                PON irq type. Refer enum pm_pon_irq_type.  
 * @param[in] trigger:  
 *                One of different irq triggers. Refer enum pm_irq_trigger_type 
 *                from pm_resources_and_types.h for more details
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__PAR1_OUT_OF_RANGE = Device Index out of
 *          range.
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *
 */
pm_err_flag_type 
pm_pon_irq_set_trigger(uint8 pmic_device_index, pm_pon_irq_type irq, pm_irq_trigger_type trigger);

/**
 * @brief This function configures the PON for irq 
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0 Secondary PMIC: 1
 * @param[in] irq:  
 *                PON irq type. Refer enum pm_pon_irq_type. 
 * @param[in] type:  
 *                Type of IRQ status to read. Refer enum pm_irq_status_type 
 *                from pm_resources_and_types.h for more details
 * @param[out] status:  
 *                IRQ status.
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__PAR1_OUT_OF_RANGE = Device Index out of
 *          range.
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *
 */
pm_err_flag_type 
pm_pon_irq_status(uint8 pmic_device_index, pm_pon_irq_type irq, pm_irq_status_type type, boolean *status);

/**
 * @brief This function for debug PON reason 
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0 Secondary PMIC: 1
 * @param[out] ptr_debug_data:  
 *                PMIC version details type structpointer. holds pon reason, AON register and PMIC version details.
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__PAR1_OUT_OF_RANGE = Device Index out of
 *          range.
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *
 */
pm_err_flag_type 
pm_pon_reason_debug(uint8 pmic_device_index, pm_pon_debug_type *ptr_debug_data);
#endif // __PM_PON_H__
