/*! \file pm_target_information.c
*  
*  \brief This file contains target specific PMIC settings common across processors.
*  \n   
*  &copy; Copyright 2020 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.

$Header: //components/rel/core.mpss/3.10/systemdrivers/pmic/target/src/mdm9205/pm_target.c#2 $ 

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_target.h"
#include "rcecb.h"
#include "sys_m_messages.h"
#include "pm_ulog.h"
#include "pm_pon.h"
 /*===========================================================================
 
                         TYPE DEFINITIONS
 
 ===========================================================================*/
 
 /*===========================================================================
 
                         LOCAL DATA DEFINITIONS
 
 ===========================================================================*/
 
 /** STM RCECB callback handle */
 #ifndef FEATURE_APPS_ONLY_BOOT
 static RCECB_HANDLE pm_set_psm_handle;
 #endif
 /*===========================================================================
 
                         LOCAL FUNCTION DEFINITIONS
 
 ===========================================================================*/
 
 /*===========================================================================
   FUNCTION
 ===========================================================================*/
 
static void pm_set_psm_bit_callback( void )
{
  pm_app_misc_set_cfg(PM_APP_MISC_CFG_PSM,0x1,0x1);
}

void
pm_target_post_init(void)
{

  /* Register callback to set PSM bit on shutdown case 
     we are doing this in s/w due PBS ram limitaion */
  #ifndef FEATURE_APPS_ONLY_BOOT
  pm_set_psm_handle =rcecb_register_name(SYS_M_SHUTDOWN_ONLY,(RCECB_CALLBACK_FNSIG)pm_set_psm_bit_callback);
  if (RCECB_NULL == pm_set_psm_handle)
  {
    PM_LOG_MSG_ERR_FATAL( "set PSM bit on shutdown callback registration failed ");
  }   
  #endif
  return;

 }