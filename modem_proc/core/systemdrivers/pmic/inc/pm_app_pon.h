#ifndef __PM_APP_PON__H__
#define __PM_APP_PON__H__

/*! \file pm_app_pon.h
 *  \n
 *  \brief  PMIC PON APP API support functions
 *  \details  This file contains functions and variable declarations to support PMIC PON APP Level APIs
 *
 *
 *  \n &copy; Copyright (c) 2016-2017, 2019  by QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================
                                Edit History
This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.

 
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/25/19   abh     Added file for Tiny Modem image in Twizy
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_err_flags.h"
#include "pm_pon.h"

/*=========================================================================== 
 
                     GLOBAL DEFINES
 
===========================================================================*/
enum
{
  PON_WARM_RST                      = PM_PON_RESET_CFG_WARM_RESET,
  PON_SHDN                          = PM_PON_RESET_CFG_NORMAL_SHUTDOWN,
  PON_DVDD_SHDN                     = PM_PON_RESET_CFG_D_VDD_BATT_REMOVE_SHUTDOWN,
  PON_XVDD_SHDN                     = PM_PON_RESET_CFG_X_VDD_COIN_CELL_REMOVE_SHUTDOWN,
  PON_HARD_RST                      = PM_PON_RESET_CFG_HARD_RESET,
  PON_DVDD_HARD_RST                 = PM_PON_RESET_CFG_D_VDD_BATT_REMOVE_HARD_RESET,
  PON_XVDD_HARD_RST                 = PM_PON_RESET_CFG_X_VDD_COIN_CELL_REMOVE_HARD_RESET,
  PON_IMMEDIATE_XVDD_SHDN           = PM_PON_RESET_CFG_IMMEDIATE_X_VDD_COIN_CELL_REMOVE_SHUTDOWN,
  PON_WARM_RST_THEN_DVDD_SHDN       = PM_PON_RESET_CFG_WARM_RESET_THEN_D_VDD_BATT_REMOVE_SHUTDOWN,
  PON_WARM_RST_THEN_XVDD_SHDN       = PM_PON_RESET_CFG_WARM_RESET_THEN_X_VDD_COIN_CELL_REMOVE_SHUTDOWN,
  PON_WARM_RST_THEN_SHDN            = PM_PON_RESET_CFG_WARM_RESET_THEN_SHUTDOWN,
  PON_WARM_RST_THEN_HARD_RST        = PM_PON_RESET_CFG_WARM_RESET_THEN_HARD_RESET,
  PON_WARM_RST_THEN_DVDD_HARD_RST   = PM_PON_RESET_CFG_WARM_RESET_THEN_D_VDD_BATT_REMOVE_HARD_RESET,
  PON_WARM_RST_THEN_XVDD_HARD_RST   = PM_PON_RESET_CFG_WARM_RESET_THEN_X_VDD_COIN_CELL_REMOVE_HARD_RESET,
  PON_NO_CFG_NEEDED                 = PM_PON_RESET_CFG_INVALID  
};

#endif  //__PM_APP_PON__H__
