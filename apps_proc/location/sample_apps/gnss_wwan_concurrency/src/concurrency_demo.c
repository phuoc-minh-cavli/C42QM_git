/*****************************************************************************
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*****************************************************************************/
#include <stdio.h>

#include "app_cfg.h"
#include "app_thread.h"
#include "app_utils_misc.h"
#include "controller_thread.h"
#include "gnss_worker_thread.h"
#include "qapi_loc_wrapper.h"

/* This function is the main entry point for the application.*/
int concurrency_demo_app_start(void)
{
    int ret = 0;

    char startMsg[32] = {0};
    char stopMsg[16] = {0};

    // Start with fetching the config
    int configRead = app_read_config();

    // Some wait to allow for logging to initialize on bootup
    app_utils_sleep_ms_with_log(APP_CONFIG.BOOTUP_DELAY_SEC * 1000, "Bootup Delay");

    if (0 != configRead)
    {
        APP_LOGH("Failed to read from app.conf, using default config values");
    }

    app_print_config();

    // Initialize platform utilities
    app_utils_byte_create_pool();
    app_thread_module_init();

    // Create GNSS Worker Thread
    APP_LOGH("Creating Location QAPI thread");
    if(0 != (ret = app_thread_create(
                        gnss_worker_get_task_name(),
                        gnss_worker_task_entry,
                        gnss_worker_get_task_id())))
    {
        APP_LOGE("GNSS Worker thread creation failed (%d)", ret);
        return ret;
    }

    // Create CONTROLLER Thread
    APP_LOGH("Creating Local Controller Thread");
    if(0 != (ret = app_thread_create(
                        local_controller_get_task_name(),
                        local_controller_task_entry,
                        local_controller_get_task_id())))
    {
        APP_LOGE("Local Controller Thread creation failed (%d)", ret);
        return ret;
    }

    app_utils_sleep_ms(500);

    local_controller_send_msg_init();

    APP_LOGH("Main thread done.");
    return ret;
}
