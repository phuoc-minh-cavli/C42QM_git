/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "LocationAPI.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "qcli.h"
#include "qcli_api.h"

#ifdef __cplusplus
}
#endif

extern QCLI_Group_Handle_t qcli_loc_handle;
extern TX_EVENT_FLAGS_GROUP gSignalGtpWwan;

// LOGGING MACROS
#define LOC_LOG_QCLI(fmt, ...) QCLI_Printf(qcli_loc_handle, fmt "\n", ##__VA_ARGS__)

extern "C" {

    static void location_control_response_callback(
        LocationError err)
    {
        LOC_LOG_QCLI("LOC API CALLBACK : CONTROL RESPONSE");
        tx_event_flags_set(&gSignalGtpWwan, 1, TX_OR);
    }

    static LocationControlCallbacks gLocationControlCallbacks = {
        sizeof(LocationControlCallbacks),
        location_control_response_callback
    };


    int enableGtpWwan(void)
    {
        ULONG set_signal = 0;
        LocationControlAPI* pLocationControlAPI = NULL;
        LocationControlCallbacks locationControlCallbacks =
            gLocationControlCallbacks;
        static bool bInitialized = false;

        LOC_LOG_QCLI("enableGtpWwan!");

        if (!bInitialized) {
            pLocationControlAPI = NULL;
            pLocationControlAPI = LocationControlAPI::createInstance(locationControlCallbacks);
            if (NULL == pLocationControlAPI) {
                LOC_LOG_QCLI("pLocationControlAPI is NULL in enableGtpWwan!");
                return -1;
            }
            bInitialized = true;
        }
        if (NULL == pLocationControlAPI) {
            LOC_LOG_QCLI("pLocationControlAPI NULL in enableGtpWwan");
            return -1;
        }
        pLocationControlAPI->enableGtpWwan();

        LOC_LOG_QCLI("Waiting on Callback..");
        tx_event_flags_get(&gSignalGtpWwan,
            1, TX_OR_CLEAR,
            &set_signal, TX_WAIT_FOREVER); 	// Wait for ever

        LOC_LOG_QCLI("enableGtpWwan SUCCESS");
        return 0;
    }
}
