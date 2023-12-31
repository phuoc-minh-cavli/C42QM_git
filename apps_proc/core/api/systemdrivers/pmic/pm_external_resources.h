#ifndef _PM_EXTERNAL_RESOURCES_H
#define _PM_EXTERNAL_RESOURCES_H
/*! \file 
 *  \n
 *  \brief  pm_external_resources.h ---- PMIC external resources header file 
 *  \n
 *  \n This file contains enumerations with resource names for different
 *  \n module types that should be used by external clients when accessing
 *  \n the resources that are required
 *  \n
 *  \n &copy; Copyright 2003-2011 Qualcomm Technologies Incorporated, All Rights Reserved
 */
/* ======================================================================= */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/api/systemdrivers/pmic/pm_external_resources.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/29/11   hs      Added CLK resource IDs.
09/14/11   dy      Move UICC resource IDs to this file
04/25/11   wra     Adding Battery Temperature Management BTM, LPG, more LVS, ADC_ARB,
                    OVP & INTerrupt channel enumeration needed for PM8921
02/08/11   hw      Merging changes from the PMIC Distributed Driver Arch branch
07/05/10   wra     Modified Header includes to accomodate MSM8660
=============================================================================*/

/* 
 * Indexes for MPPs are in CMI header file "pm_mpps.h"
 * Indexes for GPIOs are in CMI header file "pm_gpio.h" 
 */

/*! \enum chargers
 *  \brief This enum contains definitions of all chargers in the target
 */
enum
{
  PM_CHG_1,
  PM_CHG_INVALID
};

/*! \enum High_current_LEDS
 *  \brief This enum contains definitions of all High current LED resources in the target
 */
enum
{
  PM_LEDH_1,
  PM_LEDH_2,
  PM_LEDH_3,
  PM_LEDH_INVALID
};

/*! \enum Low_current_LEDS
 *  \brief This enum contains definitions of all Low current LED resources in the target
 */
enum
{
  PM_LEDL_1,
  PM_LEDL_2,
  PM_LEDL_3,
  PM_LEDL_INVALID
};

/*! \enum PWMs
 *  \brief This enum contains definitions of all PWM resources in the target
 */
enum
{
  PM_PWM_1,
  PM_PWM_2,
  PM_PWM_3,
  PM_PWM_INVALID
};

/*! \enum HSED
 *  \brief This enum contains definitions of all HSED resources in the target
 */
enum
{    
  PM_HSED_1,
  PM_HSED_2,
  PM_HSED_3,
  PM_HSED_INVALID
};

/*! \enum external_chargers
 *  \brief This enum contains definitions of all External Charging Hardware Resources in the target
 */
enum
{
  PM_CHG_EXT_1,
  PM_CHG_EXT_INVALID
}; 

/*! \enum switched mode battery charger enum
 *  \brief This enum contains definitions of all Switched Mode Battery Charger Resources in the target
 */
enum
{
  PM_SMBC_1,
  PM_SMBC_INVALID
};

/*! \enum battery management system (BMS) enum
 *  \brief This enum contains definitions of all Battery Management System Resources in the target
 */
enum
{
  PM_BMS_1,
  PM_BMS_INVALID
};

/*! \enum battery temperature management (BTM) enum
 *  \brief This enum contains definitions of all Battery Temperature Management Resources in the target
 */
enum
{
  PM_BTM_1,
  PM_BTM_INVALID
};

/*! \enum ADC arbiter channels available on PMIC component
 *  \brief This enum contains definitions of all ADC Arbiter Channel Resources in the target
 *          Typically a processor will own a channel and the arbiter will provide equal access
 *          to the ADC for each processor.
 */
enum
{
    PM_ARB_ADC_1, 
    PM_ARB_ADC_2,
    PM_ARB_ADC_3,
    PM_ARB_ADC_INVALID
};

/*! \enum Light Pulse Generator LPG resources available on PMIC component
 *  \brief This enum contains definitions of all Light Pulse Generator LPG Resources in the target
 */
enum
{
    PM_LPG_1,
    PM_LPG_2,
    PM_LPG_3,
    PM_LPG_4,
    PM_LPG_5,
    PM_LPG_6,
    PM_LPG_7,
    PM_LPG_8,
    PM_LPG_INVALID
};

/*! \enum Interrupt resources available on PMIC component
 *  \brief This enum contains definitions of all Interrupt Resources on the target
 */
enum
{
    PM_INTERRUPT_1, //RPM/SEC
    PM_INTERRUPT_2, //MDM
    PM_INTERRUPT_3, //APP/USER
    PM_INTERRUPT_INVALID
};

/*! \enum Over Voltage Protection resources available on PMIC component
 *  \brief This enum contains definitions of all Over Voltage Protection OVP Resources on the target
 */
enum
{
    PM_OVP_1,
    PM_OVP_2,
    PM_OVP_INVALID
};

/*! \enum Mega XO resources available on PMIC component
 *  \brief This enum contains definitions the Mega XO resources in the target
 */
enum
{
    PM_MEGA_XO_1,
    PM_MEGA_XO_2,
    PM_MEGA_XO_3,
    PM_MEGA_XO_4,
    PM_MEGA_XO_5,
    PM_MEGA_XO_6,
    PM_MEGA_XO_7,
    PM_MEGA_XO_8,
    PM_MEGA_XO_9,
    PM_MEGA_XO_INVALID,
};

/*! \enum UICC resources available on PMIC component
 *  \brief This enum contains definitions the UICC resources in the target
 */
enum
{
  PM_BATTERY_ALARM,
  PM_UICC_1,
  PM_UICC_2,
  PM_UICC_3,
  PM_UICC_INVALID
};

#endif /* _PM_EXTERNAL_RESOURCES_H */

