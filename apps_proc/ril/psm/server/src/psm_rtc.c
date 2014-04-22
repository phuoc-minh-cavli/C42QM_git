/******************************************************************************
#@file    psm_rtc.c
#@brief   Provides API to configure RTC
#
#  ---------------------------------------------------------------------------
#
#  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#  ---------------------------------------------------------------------------
#******************************************************************************/

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "pm_rtc.h"
#include "pmapp_rtc.h"

#include "psm_rtc.h"
#include "psm_core.h"
#include "psm_common.h"

/*==============================================================================
FUNCTION psm_rtc_enable_rtc

Enable RTC Alarm functionality.

ARGUMENTS
    none

RETURN VALUE
    int - 0 for success, -1 for failure
==============================================================================*/

int psm_rtc_enable_rtc(void)
{
    pm_err_flag_type   rc = PM_ERR_FLAG__SUCCESS;

#ifndef FEATURE_PSM_USE_NEW_PMIC_RTC_API
    rc = pm_rtc_init();
    if ( rc != PM_ERR_FLAG__SUCCESS )
    {
        PSM_LOG_INFO("psm_rtc_enable_rtc: Failed to enable RTC alarm functionality");
    }

    rc = pm_set_rtc_display_mode(PM_RTC_24HR_MODE);
    if ( rc != PM_ERR_FLAG__SUCCESS )
    {
        PSM_LOG_INFO("psm_rtc_enable_rtc: Failed to enable RTC alarm functionality");
    }
#else
    /**
    * _rtc_init() is not required as per latest pmic API
    **/
    PSM_LOG_INFO("psm_rtc_enable_rtc: enabled RTC alarm functionality");
#endif
    return rc;
}


/*==============================================================================
FUNCTION psm_rtc_set_alarm

Set RTC alarm

ARGUMENTS
    time_in_sec      - Duration after which RTC should trigger interupt to bootup device.

RETURN VALUE
    int - 0 for success, -1 for failure
==============================================================================*/
int psm_rtc_set_alarm(uint32 time_in_sec)
{
    pm_err_flag_type      ret = PM_ERR_FLAG__SUCCESS;
#ifdef FEATURE_PSM_USE_NEW_PMIC_RTC_API
    pm_rtc_time_type    time;
    PSM_LOG_FUNC_ENTRY("psm_rtc_set_alarm: function entry");

    do {
        time.sec = time_in_sec;
        time.msec = 0;

        PSM_LOG_INFO2("psm_rtc_set_alarm: set rtc_time %d-%d ",time.sec,time.msec);
        ret = pmapp_rtc_alarm_set_time(&time);

        if ( ret != PM_ERR_FLAG__SUCCESS )
        {
            break;
        }

    } while(FALSE);
#else
    pm_rtc_julian_type    time;
    uint32                rtc_secs = 0;
    PSM_LOG_FUNC_ENTRY("psm_rtc_set_alarm: function entry");

    do
    {
        psm_rtc_get_time(&rtc_secs);
        rtc_secs += time_in_sec;

        ret = pm_rtc_app_rtc_secs_to_julian(rtc_secs, &time);
        if ( ret != PM_ERR_FLAG__SUCCESS )
        {
            break;
        }

        ret = pm_rtc_alarm_rw_cmd(PM_RTC_SET_CMD, PM_RTC_ALARM_1, &time);
        if ( ret != PM_ERR_FLAG__SUCCESS )
        {
            break;
        }
    }while(FALSE);
#endif

    PSM_LOG_FUNC_RETURN_WITH_RET(ret);
    return ret;

}

/*==============================================================================
FUNCTION psm_rtc_get_time

Get current time.

ARGUMENTS
    time_in_sec      - UTC time in secs.

RETURN VALUE
    int - 0 for success, -1 for failure
==============================================================================*/
int psm_rtc_get_time(uint32 *time_in_secs)
{
    pm_err_flag_type   ret = PM_ERR_FLAG__SUCCESS;

#ifdef FEATURE_PSM_USE_NEW_PMIC_RTC_API
    pm_rtc_time_type time;
    PSM_LOG_FUNC_ENTRY("psm_rtc_get_time: function entry");

    do
    {
        ret = pmapp_rtc_get_time(&time);
        if ( ret != PM_ERR_FLAG__SUCCESS )
        {
            break;
        }
        *time_in_secs = time.sec;
        PSM_LOG_INFO2("psm_rtc_get_time: get rtc_time %d-%d ",time.sec,time.msec);
    } while(FALSE);
#else

    pm_rtc_julian_type time;
    PSM_LOG_FUNC_ENTRY("psm_rtc_get_time: function entry");

    do
    {
        ret = pm_rtc_rw_cmd(PM_RTC_GET_CMD, &time);
        if ( ret != PM_ERR_FLAG__SUCCESS )
        {
            break;
        }

        *time_in_secs = pm_rtc_app_rtc_julian_to_secs(&time);
    }while(FALSE);
#endif

    PSM_LOG_FUNC_RETURN_WITH_RET(ret);
    return ret;
}
