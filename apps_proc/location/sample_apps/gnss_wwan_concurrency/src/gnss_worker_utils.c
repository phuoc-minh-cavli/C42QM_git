/*****************************************************************************
 Copyright (c) 2020 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 *****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "app_utils_misc.h"
#include "qapi_loc_wrapper.h"
#include "qapi_timer.h"

// hacky fix for redefinition error
#undef true
#undef false
#include "qapi_device_info.h"

// Internal QAPI handles
static qapi_Device_Info_Hndl_t gDeviceInfoHandle = NULL;
static uint8 gIsDefaultAppPriorityGnss = 0;
static uint8 gIsWwanSleepCbSet = 0;

static void _gnss_worker_utils_init_device_info()
{
    if (NULL == gDeviceInfoHandle)
    {
        qapi_Status_t res = qapi_Device_Info_Init_v2(&gDeviceInfoHandle);
        if (QAPI_OK != res)
        {
            APP_LOGE("qapi_Device_Info_Init_v2 failed");
            return;
        }
        else
        {
            APP_LOGH("qapi_Device_Info_Init_v2 success.");
        }

        res = qapi_Device_Info_Pass_Pool_Ptr(gDeviceInfoHandle, app_utils_get_byte_pool());
        if(QAPI_OK != res)
        {
            APP_LOGE("qapi_Device_Info_Pass_Pool_Ptr returned error %d", res);
            return;
        }
        else
        {
            APP_LOGH("qapi_Device_Info_Pass_Pool_Ptr success.");
        }
    }
}

void gnss_worker_utils_set_app_priority(uint8 setGnssHighPriority)
{
    _gnss_worker_utils_init_device_info();

    int priorityValue[1] = {};
    // 0 = GNSS , 1 = WWAN
    if (setGnssHighPriority)
    {
        APP_LOGEVENT("Setting GNSS High priority");
        priorityValue[0] = 0;  // GNSS
    }
    else
    {
        APP_LOGEVENT("Setting WWAN High priority");
        priorityValue[0] = 1;  // WWAN
    }
    qapi_Status_t res = qapi_Device_Info_Set(
            gDeviceInfoHandle, QAPI_DEVICE_INFO_SET_APP_PRIORITY_E, 1, priorityValue);
    if (QAPI_OK == res)
    {
        APP_LOGH("Priority change successful.");
    }
    else
    {
        APP_LOGE("Priority change failed.");
    }
}

void gnss_worker_utils_set_gnss_high_priority()
{
    gnss_worker_utils_set_app_priority(1);
}
void gnss_worker_utils_set_wwan_high_priority()
{
    gnss_worker_utils_set_app_priority(0);
}

uint8 gnss_worker_utils_is_gnss_high_priority()
{
    _gnss_worker_utils_init_device_info();

    // 0 = GNSS , 1 = WWAN
    qapi_Device_Info_t deviceInfo = {};
    qapi_Status_t res = qapi_Device_Info_Get_v2(
            gDeviceInfoHandle, QAPI_DEVICE_INFO_GET_APP_PRIORITY_E, &deviceInfo);
    if (QAPI_OK == res)
    {
        if (deviceInfo.u.valueint == QAPI_DEVICE_INFO_GNSS_PRIORITY)
        {
            return 1;
        }
        else if (deviceInfo.u.valueint == QAPI_DEVICE_INFO_WWAN_PRIORITY)
        {
            return 0;
        }
        else
        {
            APP_LOGE("Invalid App priority fetched from device info QAPI");
        }
    }
    else
    {
        APP_LOGE("Failed to get current app priority");
    }

    return 0;
}

void gnss_worker_utils_restore_default_app_priority()
{
    uint8 isCurrentPriorityGnss = gnss_worker_utils_is_gnss_high_priority();

    if (isCurrentPriorityGnss && !gIsDefaultAppPriorityGnss)
    {
        APP_LOGEVENT("Restoring app priority to default value [WWAN]");
        gnss_worker_utils_set_wwan_high_priority();
    }

    if (!isCurrentPriorityGnss && gIsDefaultAppPriorityGnss)
    {
        APP_LOGEVENT("Restoring app priority to default value [GNSS]");
        gnss_worker_utils_set_gnss_high_priority();
    }
}

void gnss_worker_utils_log_state_event(
        gnss_worker_state state, gnss_worker_event event,
        qapi_Location_Meta_Data_Engine_Status_t status)
{
    if (event == GNSS_WORKER_EVENT_STATE_ENTRY)
    {
        APP_LOG_GNSS_HLOS_H("Entering State [%s] (%d)",
                app_utils_gnss_worker_state_str(state), state);
    }

    if (event == GNSS_WORKER_EVENT_ENGINE_STATUS_UPDATE)
    {
        app_utils_log_engine_status_recvd(state, status);
    }

    if (event == GNSS_WORKER_EVENT_SINGLESHOT_TIMEOUT)
    {
        APP_LOGEVENT("Recvd [Singleshot Timeout] in state (%d)", state);
    }

    if (event == GNSS_WORKER_EVENT_ABSOLUTE_MOTION)
    {
        APP_LOGEVENT("Recvd [Motion] in state (%d)", state);
    }
}

void gnss_worker_utils_init()
{
    if (gnss_worker_utils_is_gnss_high_priority())
    {
        APP_LOGH("Default APP Priority is GNSS");
        gIsDefaultAppPriorityGnss = 1;
    }
    else
    {
        APP_LOGH("Default APP Priority is WWAN");
        gIsDefaultAppPriorityGnss = 0;
    }
}

static void _gnss_worker_utils_wwan_sleep_callback(
        qapi_Device_Info_Hndl_t deviceInfoHandle, const qapi_Device_Info_t* info)
{
    APP_LOGM("WWAN Sleep callback :: hndl %p info %p", deviceInfoHandle, info);

    // Validate callback parameters
    if (deviceInfoHandle != gDeviceInfoHandle)
    {
        APP_LOGE("Invalid device info handle in callback.");
        return;
    }
    if (info->id != QAPI_DEVICE_INFO_WWAN_SLEEP_INFO_E)
    {
        APP_LOGE("Unsolicited info id %d", info->id);
        return;
    }
    if (info->info_type != QAPI_DEVICE_INFO_TYPE_INTEGER_E)
    {
        APP_LOGE("Invalid device info type %d", info->info_type);
        return;
    }

    uint32 sleepDuration = ((uint32)info->u.valueint)/1000;
    APP_LOGM("WWAN Sleep Duration = %d seconds", sleepDuration);

    // send message to gnss worker thread
    gnss_worker_send_msg_cb_wwan_sleep(sleepDuration);
}

void gnss_worker_utils_set_wwan_sleep_callback()
{
    _gnss_worker_utils_init_device_info();

    if (gIsWwanSleepCbSet == 0)
    {
        qapi_Status_t res = qapi_Device_Info_Set_Callback_v2(
                gDeviceInfoHandle, QAPI_DEVICE_INFO_WWAN_SLEEP_INFO_E,
                _gnss_worker_utils_wwan_sleep_callback);
        if (QAPI_OK == res)
        {
            APP_LOGH("Set wwan sleep callback success.");
        }
        else
        {
            APP_LOGE("Failed to set wwan sleep callback.");
        }

        gIsWwanSleepCbSet = 1;
    }
}
