/*! \file   pm_proc_app.c
*  
*  \brief   This file contains PMIC Modem initialization
*  \details PMIC Modem Proc initialization to initialize driver resources
*  
*  &copy; Copyright 2010-2013 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.

$Header: //components/rel/core.tx/6.0/systemdrivers/pmic/framework/src/pm_init.c#3 $ 

when        who     what, where, why
--------    ---     ----------------------------------------------------------
07/12/12    umr     Add support for BUA.  
07/12/12    vk      Satisfy COREBSP Dependency to pm_init() and deploy in RCINIT 
                    Framework

===========================================================================*/

#include "DDIPlatformInfo.h"
#include "pm_qc_pmic.h"
#include "pm_apps_proc_npa.h"
#include "device_info.h"
#include "pm_ulog.h"

#include "pm_malloc.h" // pm_malloc
#include "pm_comm.h"
#include "pm_target_information.h"

#include "pm_resource_manager.h"

#ifdef PM_FEATURE_ON_TARGET_ENGINEERING_BUILD
#include "pm_on_target_test.h" 
#endif /* PM_FEATURE_ON_TARGET_ENGINEERING_BUILD */

#include "npa.h"

#ifdef FEATURE_TX_SP_AWARE_TRACKER
#include "pmapp_fg.h"
#endif
boolean bPMICStub = FALSE;

void pm_init ( )
{
	//PMIC init not required on RUMI
	if (DalPlatformInfo_Platform() == DALPLATFORMINFO_TYPE_RUMI)
	{
		bPMICStub = TRUE;
	}
	
    //protects double calls from tmc
    #if defined(FEATURE_RCINIT)
    { 
          static boolean call_once = FALSE;
          if(FALSE != call_once)
               return;
          call_once = TRUE;
    }
    #endif

     pm_ulog_init();

    pm_comm_channel_init_internal();
    pm_version_detect();

    pm_target_information_init();
	
    pm_comm_info_init();
	
	/*
	* Stub out driver initialization in applicable environments.
	*/
	if (bPMICStub == FALSE)
	{
		pm_resource_manager_init();

		#if defined(PM_TEST_MODE)
		   pm_app_set_mode_init();
		#endif 
	}
	
    pm_apps_proc_npa_init ();

    pm_comm_post_init();
#ifdef FEATURE_TX_SP_AWARE_TRACKER
    pmapp_fuel_init_start();
#endif
}
