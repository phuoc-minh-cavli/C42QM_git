#ifndef __PMAPP_VBATT_H__
#define __PMAPP_VBATT_H__

/*===========================================================================


               P M A P P_ VBATT  H E A D E R    F I L E

\details
    This file contains functions prototypes and variable/type/constant 
  declarations for the PMIC VATT application
  
Copyright (c) 2017 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


                                Edit History
 
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/11/17   kcs     Updated the file for more Battery management API's (CR -2110414).
05/29/17   kcs     created.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#include "DDIAdc.h"
#include "pm_err_flags.h"
#include "pm_version.h"

/*=========================================================================== 
 
                     GLOBAL DEFINES
 
===========================================================================*/

typedef enum
{
  PMIC_BAT_TECH_LI_ION,
  PMIC_BAT_TECH_LI_POLYMER,
} pm_battery_technology;
	 
typedef enum
{
  SMB_ABSENT=0,
  SMB_PRESENT,
} smb_presence;

 
typedef enum
{
  BAT_COLD,
  BAT_HOT,
  BAT_GOOD
} pm_battery_temperature;


typedef enum
{
  BAT_OV,
  BAT_UV,
  BAT_MISSING,
  BAT_GOOD_HEALTH
} pm_battery_health;
 
typedef enum
{
  BAT_DISCHARGING,
  BAT_CHARGING
} pm_battery_chg_status;
 
typedef enum
{
  USB_CDP,
  USB_DCP,
  USB_SDP,
  CHG_ABSENT
} pm_battery_chg_src;
	 
typedef pm_battery_temperature battery_temp;
typedef pm_battery_technology battery_tech;
typedef pm_battery_health battery_hlth;
typedef pm_battery_chg_src battery_chg_source;
typedef pm_battery_chg_status battery_chg_status;

 
typedef struct pmapp_battery_data
{
  int bat_voltage;
  uint32 bat_percentage;
  boolean vbus_present;
  battery_temp bat_temp; /* Cold, Hot, Good - Depends on BAT_TERM*/
  battery_tech bat_technology; /* LI_ION or LI_POLYMER */
  battery_chg_status bat_chg_status; /* Charging/Discharging/Full*/
  battery_hlth bat_health; /* Over_Voltage, No Battery profile*/
  battery_chg_source bat_chg_src; /*AC, USB, Wireless */
}pmapp_battery_data;

extern pmapp_battery_data battery_stats;

 /*=========================================================================== 
 
                  Static Variable Definitions
 
===========================================================================*/

/*===========================================================================

                  PM APP VBATT FUNCTION PROTOTYPES

===========================================================================*/

/**
 * @name 
 *     pmapp_vbatt_get_battery_status
 *
 * @description
 *    This function gets the current battery percentage
 * 
 * @param 
 *     uint32* battery_percentage
 * 
 * @return 
 *     pm_err_flag_type
 * 
 */
/*===========================================================================*/
 pm_err_flag_type pmapp_vbatt_get_battery_status(uint8* battery_percentage);

/*===========================================================================*/
 /**
 * @name 
 *     pmapp_get_battery_stats_init
 *
 * @description
 *    This function initiises required API's
 * 
 * @param 
 *     None
 * 
 * @return 
 *     pm_err_flag_type
 * 
 */
/*===========================================================================*/
pm_err_flag_type pmapp_get_battery_stats_init(void);

/*===========================================================================*/
/**
 * @name 
 *     pmapp_get_battery_health
 *
 * @description
 *    This function gets battery healthiness
 * 
 * @param 
 *     Batt_health structure to get the Battery health information.
 * 
 * @return 
 *     pm_err_flag_type
 * 
 */
/*===========================================================================*/
pm_err_flag_type pmapp_get_battery_health(pm_battery_health *batt_health);

/*===========================================================================*/

/**
 * @name 
 *     pmapp_get_battery_temp
 *
 * @description
 *    This function gets the battery temparature
 * 
 * @param 
 *    Batt_temp structure to get the battery temparature.
 * 
 * @return 
 *     pm_err_flag_type
 * 
 */
/*===========================================================================*/
 
pm_err_flag_type pmapp_get_battery_temp(pm_battery_temperature *batt_temp);

/*===========================================================================*/

/**
 * @name 
 *     pmapp_get_battery_tech
 *
 * @description
 *    This function gets the battery technology
 * 
 * @param 
 *    Batt_tech structure to get the Battery technolgy.
 * 
 * @return 
 *     pm_err_flag_type
 * 
 */
 
 /*===========================================================================*/

pm_err_flag_type pmapp_get_battery_tech(pm_battery_technology *batt_tech);

/*===========================================================================*/

/**
 * @name 
 *     pmapp_get_charging_status
 *
 * @description
 *    This function gets battery charging status
 * 
 * @param 
 *   charge_status structure to get the charging status - charging or discharging
 * 
 * @return 
 *     pm_err_flag_type
 * 
 */

/*===========================================================================*/
pm_err_flag_type pmapp_get_charging_status(pm_battery_chg_status *charge_status);

/*===========================================================================*/

/**
 * @name 
 *     pmapp_get_charger_source
 *
 * @description
 *    This function gets the current charger type
 * 
 * @param 
 *    charge_source structure to get the charger type information.
 * 
 * @return 
 *     pm_err_flag_type
 * 
 */
/*===========================================================================*/

pm_err_flag_type pmapp_get_charger_source(pm_battery_chg_src *charger_source);

/*===========================================================================*/
#ifdef FEATURE_PMIC_CHARGER
/**
 * @name 
 *     pmapp_get_charge_reg
 *
 * @description
 *    This function gets the register value
 * 
 * @param 
 *    offset :register offset
 *    data   :pointer to read data buffer
 * 
 * @return 
 *     pm_err_flag_type
 * 
 */

/*===========================================================================*/

pm_err_flag_type pmapp_get_charge_reg(uint8 offset, uint8 *data);

/*===========================================================================*/

/**
 * @name 
 *     pmapp_set_charge_reg
 *
 * @description
 *    This function sets the register
 * 
 * @param 
 *    offset :register offset
 *    data   :write data buffer
 * 
 * @return 
 *     pm_err_flag_type
 * 
 */
/*===========================================================================*/

pm_err_flag_type pmapp_set_charge_reg(uint8 offset, uint8 data);

/*===========================================================================*/

/**
 * @name 
 *     pmapp_vbatt_get_battery_volt_status
 *
 * @description
 *    This function gets the current battery voltage
 * 
 * @param 
 *     uint32* battery_volt
 * 
 * @return 
 *     pm_err_flag_type
 * 
 */
/*===========================================================================*/
 pm_err_flag_type pmapp_vbatt_get_battery_volt_status(uint32* battery_volt);
/*===========================================================================*/

/**
 * @name 
 *     pmapp_config_sysok_option
 *
 * @description
 *    This function sets sysok option
 * 
 * @param 
 *    option : sysok options
 * 
 * @return 
 *     pm_err_flag_type
 * 
 */
/*===========================================================================*/

pm_err_flag_type pmapp_config_sysok_option(uint8 option);

/*===========================================================================*/

#endif
#endif  /* __PMAPP_VBATT_H__ */


