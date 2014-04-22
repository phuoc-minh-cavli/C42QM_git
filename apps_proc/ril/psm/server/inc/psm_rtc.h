/******************************************************************************
#@file    psm_rtc.h
#@brief   Provides API to configure RTC
#
#  ---------------------------------------------------------------------------
#
#  Copyright (c) 2015 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#  ---------------------------------------------------------------------------
#******************************************************************************/

#ifndef PSM_RTC_H
#define PSM_RTC_H

/*==============================================================================
FUNCTION psm_rtc_set_alarm

Set RTC alarm

ARGUMENTS
    time_in_sec      - Duration after which RTC should trigger interupt to bootup device.

RETURN VALUE
    int - 0 for success, -1 for failure
==============================================================================*/
int psm_rtc_set_alarm(uint32 time_in_sec);

/*==============================================================================
FUNCTION psm_rtc_get_time

Get current time.

ARGUMENTS
    time_in_sec      - UTC time in secs.

RETURN VALUE
    int - 0 for success, -1 for failure
==============================================================================*/
int psm_rtc_get_time(uint32 *time_in_secs);

/*==============================================================================
FUNCTION psm_rtc_enable_rtc

Enable RTC Alarm functionality.

ARGUMENTS
    none

RETURN VALUE
    int - 0 for success, -1 for failure
==============================================================================*/
int psm_rtc_enable_rtc(void);


#endif // PSM_RTC_H
