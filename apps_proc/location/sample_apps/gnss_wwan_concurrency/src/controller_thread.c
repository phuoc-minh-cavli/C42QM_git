/*****************************************************************************
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*****************************************************************************/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>

#include "app_thread.h"
#include "app_utils_db_iface.h"
#include "app_utils_fs_iface.h"
#include "app_utils_misc.h"
#include "controller_thread.h"
#include "qapi_loc_wrapper.h"

#include "gnss_worker_thread.h"
#include "app_cfg.h"

/* Thread Constants */
static app_thread_id _LocalControllerTaskId = APP_THREAD_ID_CONTROLLER;
static char* _LocalControllerTaskName = "local_controller";

int local_controller_get_task_id()
{
    return _LocalControllerTaskId;
}

char* local_controller_get_task_name()
{
    return _LocalControllerTaskName;
}

// Internal struct to allow passing message data to this thread
typedef struct controller_msg
{
    // report location
    qapi_Location_t location;

} controller_msg;


// Convenience functions to send messages to this local controller thread
void local_controller_send_msg_init()
{
    app_thread_send_msg(
            _LocalControllerTaskId,
            LOCAL_CONTROLLER_MSG_ID_INIT, NULL, 0);
}

void local_controller_send_msg_report_location(qapi_Location_t location)
{
    controller_msg msg = {0};
    size_t msgSize = sizeof(controller_msg);

    msg.location = location;

    app_thread_send_msg(
            _LocalControllerTaskId,
            LOCAL_CONTROLLER_MSG_ID_REPORT_LOCATION,
            &msg, msgSize);
}

static void _local_controller_handle_msg_init(void* dataPtr, size_t dataSize)
{
    APP_LOGM("Local Controller INIT");

    // Now start executing the main logic
    if (APP_CONFIG.LOCATION_MODE == 0)
    {
        APP_LOGH("Starting Singleshot Mode execution");

        qapi_Location_t location = {};
        for (uint32 i = 0; i < APP_CONFIG.LOCATION_REPORT_COUNT; i++)
        {
            APP_LOGH("Requesting location report number: %d", i);

            location.size = 0;

            // First get best available position
            gnss_worker_sync_get_best_avail_location(&location);

            if (location.size == sizeof(location))
            {
                APP_LOGH("Got location.");
                app_utils_log_location(location);
            }
            else
            {
                APP_LOGE("Failed to get best available position");
            }

            gnss_worker_send_msg_request_delayed_location(
                    APP_CONFIG.LOCATION_REPORTING_INTERVAL_SEC,
                    APP_CONFIG.LOCATION_SESSION_MAX_POWER_LEVEL,
                    APP_CONFIG.LOCATION_SESSION_MIN_ACCURACY_LEVEL,
                    APP_CONFIG.ALLOW_SYSTEM_PRIORITY_INVERSION);

            // Sleep for delay amount
            APP_LOGH("Sleeping for location report number: %d", i);
            app_utils_sleep_ms(APP_CONFIG.LOCATION_REPORTING_INTERVAL_SEC * 1000);
        }
    }
    else if (APP_CONFIG.LOCATION_MODE == 1)
    {
        APP_LOGH("Starting Tracking Mode execution");

        gnss_worker_send_msg_request_tracking_start(
                APP_CONFIG.TRACKING_TBF_MILLIS,
                0, // distance argument always 0, no distance based tracking support in this app.
                APP_CONFIG.TRACKING_ACCURACY,
                APP_CONFIG.ALLOW_SYSTEM_PRIORITY_INVERSION);

        if (APP_CONFIG.TRACKING_DURATION_SEC > 0)
        {
            APP_LOGH("Controller sleeping for tracking duration: %d sec",
                    APP_CONFIG.TRACKING_DURATION_SEC);
            app_utils_sleep_ms(APP_CONFIG.TRACKING_DURATION_SEC * 1000);

            gnss_worker_send_msg_request_tracking_stop();
        }
    }
    else if (APP_CONFIG.LOCATION_MODE == 2)
    {
        APP_LOGH("Starting Geofence Mode execution");

        gnss_worker_send_msg_request_add_geofence(
                APP_CONFIG.GEOFENCE_LATITUDE, APP_CONFIG.GEOFENCE_LONGITUDE, APP_CONFIG.GEOFENCE_RADIUS_METERS,
                APP_CONFIG.GEOFENCE_MASK, APP_CONFIG.GEOFENCE_RESPONSIVENESS_MILLIS, APP_CONFIG.GEOFENCE_DWELL_MILLIS,
                APP_CONFIG.ALLOW_SYSTEM_PRIORITY_INVERSION);
    }
    else
    {
        APP_LOGE("Invalid location mode for execution.");
    }
}

static void _local_controller_handle_msg_report_location(void* data, size_t size)
{
    APP_LOGM("Local Controller REPORT LOCATION");

    // validate
    controller_msg* msg = (controller_msg*)data;
    if (msg == NULL || size != sizeof(controller_msg))
    {
        APP_LOGE("Invalid msg");
        return;
    }

    // Extract parameters
    qapi_Location_t location = msg->location;

    app_utils_log_location(location);
}

static void _local_controller_process_msg(int msgId, void *msgPtr, size_t msgSize)
{
    APP_LOGM("Local Controller received message %d", msgId);

    // Make sure to free any data pointer within the message
    // As part of handling the message..
    switch (msgId)
    {
        case LOCAL_CONTROLLER_MSG_ID_INIT:
            _local_controller_handle_msg_init(msgPtr, msgSize);
            break;

        case LOCAL_CONTROLLER_MSG_ID_REPORT_LOCATION:
            _local_controller_handle_msg_report_location(msgPtr, msgSize);
            break;

        default :
            APP_LOGE("Invalid msgId %d", msgId);
    }

    APP_LOGM("Local Controller message processing complete");
}

/* This is the entry point for Local Controller task */
void local_controller_task_entry(ULONG args)
{
    AppMsg *payload = NULL;

    APP_LOGH("Local Controller Thread started. TCB %x TID %d", tx_thread_identify(), args);

    while(0 == app_thread_rcv_msg(args, (void **)&payload))
    {
        _local_controller_process_msg(payload->msgId, payload->msg, payload->msgSize);
        if (payload->msg != NULL)
        {
            app_utils_byte_release(payload->msg);
        }
        app_utils_byte_release(payload);
    }
}
