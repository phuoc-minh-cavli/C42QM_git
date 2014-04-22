/*! \file pm_target.c
*  
*  \brief This file contains target specific PMIC settings common across processors.
*  \n   
*  &copy; Copyright 2020 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.

$Header: //components/rel/core.tx/6.0/systemdrivers/pmic/target/src/mdm9205/pm_target.c#1 $ 

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_target.h"
#include "pmapp_pon.h"


/* =========================================================================
**  Function : pm_shutdown_target
** =========================================================================*/
/**
  See pm_pwroff.h
*/


pm_err_flag_type pm_shutdown_target (void)
{
  pm_err_flag_type err = PM_ERR_FLAG__SUCCESS;
  #ifdef USES_APPS_ONLY_BOOT
  err = pm_app_misc_set_cfg(PM_APP_MISC_CFG_PSM,0x1,0x1);
  #endif
  return err;
}

