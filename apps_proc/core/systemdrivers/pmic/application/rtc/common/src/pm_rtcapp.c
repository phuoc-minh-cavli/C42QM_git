/*! \file
*  
*  \brief  pm_rtc_app.c ----File contains the implementation of the public APIs for RTCAPP resource type.
*  \details Implementation file for RTCAPP resourece type.
*  Each of the APIs checks for access and then if necessary directs
*  the call to Driver implementation or to RPC function for the master processor.
*  
*    PMIC code generation Version: 2.0.0.22
*    This file contains code for Target specific settings and modes.
*  
*  &copy; Copyright 2010, 2020 Qualcomm Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.

$Header: //components/rel/core.tx/6.0/systemdrivers/pmic/application/rtc/common/src/pm_rtcapp.c#3 $ 

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pmapp_rtc.h"
#include "pm_ulog.h" 
#include "pm_version.h"

/*===========================================================================

                     FUNCTION DECLARATION 

===========================================================================*/


/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/



pm_err_flag_type pmapp_rtc_get_time(pm_rtc_time_type  *time_ptr)
{
    uint8             pmic_index = 0;
    
    if (NULL == time_ptr)
    {
        return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
    }        
    return pm_rtc_rw_get_ms_time (pmic_index, time_ptr);
    
}


pm_err_flag_type pmapp_rtc_alarm_set_time(pm_rtc_time_type  *time_ptr)
{
	uint8             pmic_index = 0;    
	pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;
    pm_rtc_time_type  current_rtc_time;
    
    if (NULL == time_ptr)
    {
        return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
    }
    
    errFlag = pm_rtc_rw_get_ms_time (pmic_index, &current_rtc_time);
    if (errFlag == PM_ERR_FLAG__SUCCESS)
    {
        // Need to check for over flow
        if ((current_rtc_time.msec + time_ptr->msec) > 1000) 
        {
            current_rtc_time.sec++;
        }
         
        current_rtc_time.sec += time_ptr->sec;
        current_rtc_time.msec = (current_rtc_time.msec + time_ptr->msec)%1000; 
        
        errFlag = pm_rtc_alarm_set_ms_time (pmic_index, &current_rtc_time);
        
        if (errFlag == PM_ERR_FLAG__SUCCESS)
        {
            errFlag = pm_rtc_alarm_enable (pmic_index, TRUE);
        }
    }
        
    return errFlag;
}
