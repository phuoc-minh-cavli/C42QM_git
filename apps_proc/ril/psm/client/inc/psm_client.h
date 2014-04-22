/******************************************************************************
#@file    psm_client.h
#@brief   Provides interface for clients to access PSM functionality
#
#  ---------------------------------------------------------------------------
#
#  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#  ---------------------------------------------------------------------------
#******************************************************************************/

#ifndef PSM_CLIENT_H
#define PSM_CLIENT_H

#include <time.h>
#include "pmapp_rtc.h"
#include "qapi_psm_types.h"
#include "time_svc.h"
#include <stdlib.h>

/*=========================================================================
  FUNCTION:  psm_client_register

===========================================================================*/
/*!
    @brief
    Makes the app known to PSM server as a PSM aware application.
    This is the first API every PSM aware application need to call.
    Every application which need network related functionality should call this API.

    @return
    None.
*/
/*=========================================================================*/
psm_error_type_e psm_client_register(int *client_id, psm_client_cb_type cb_func);

/*=========================================================================
  FUNCTION:  psm_client_register_with_msg

===========================================================================*/
/*!
    @brief
    Register with the PSM Daemon along with a supplied memory to use with
    Callbacks.

    This is the first API every Userspace PSM aware application need to call.
    Every application which need network related functionality should call this API.

    @return
    None.
*/
/*=========================================================================*/
psm_error_type_e psm_client_register_with_msg
(
    int                 *client_id,
    psm_client_cb_type   cb_func,
    psm_status_msg_type *msg
);

/*=========================================================================
  FUNCTION:  psm_client_unregister

===========================================================================*/
/*!
    @brief
    Unregister a psm aware app.
    Application should not call any other PSM client API except psm_client_register after this API is called.

    @return
    None.
*/
/*=========================================================================*/
psm_error_type_e psm_client_unregister(int client_id);

/*=========================================================================
  FUNCTION:  psm_client_enter_power_save

===========================================================================*/
/*!
    @brief
    Used by app to notify its desire to enter PSM mode.
    Application need to pass active_time in secs, time in PSM mode, whether next wake up is for measurement
    purpose or access network. PSM time can will be accepted in either broken down format or
    in secs.

    @return
    None.
*/
/*=========================================================================*/
psm_error_type_e psm_client_enter_power_save
(
    int                 client_id,
    psm_info_type      *psm_info
);

/*=========================================================================
  FUNCTION:  psm_client_enter_backoff

===========================================================================*/
/*!
    @brief
    Used by app to notify its desire to enter PSM mode due to network OOS or MTC server not reachable.
    Duration for which app want to enter into PSM mode is decided by the PSM server based on
    the NV item configuration.

    @return
    None.
*/
/*=========================================================================*/
psm_error_type_e psm_client_enter_backoff( int client_id );


/*=========================================================================
  FUNCTION:  psm_cancel_power_save

===========================================================================*/
/*!
    @brief
    Used by app to notify its desire to withdraw a previous request to enter PSM mode.

    @return
    None.
*/
/*=========================================================================*/
psm_error_type_e psm_cancel_power_save( int client_id );

/*=========================================================================
  FUNCTION:  psm_client_load_modem

===========================================================================*/
/*!
    @brief
    Application should inform its intention to use modem services through this API.

    @return
    None.
*/
/*=========================================================================*/
psm_error_type_e psm_client_load_modem(int client_id);

/*=========================================================================
  FUNCTION:  psm_client_health_check_ack

===========================================================================*/
/*!
    @brief
    Application health check acknowledge API. Application need to call this API
    when it receives PSM_STATUS_HEALTH_CHECK event.
    This API will inform PSM server that application is healthy and functioning.
    Application should call this API from worker thread of the application.
    This will ensure that application malfunctioning is detected by PSM server.

    @return
    None.
*/
/*=========================================================================*/
psm_error_type_e psm_client_health_check_ack(int client_id);

/*=========================================================================
  FUNCTION:  psm_client_optimal_interval

===========================================================================*/
/*!
    @brief
    Used by app to find optimal PSM interval from a group of time intervals.

    @return
    None.
*/
/*=========================================================================*/
psm_error_type_e psm_client_optimal_interval
(
    int client_id,
    psm_time_interval_group_type *interval_group,
    psm_time_interval_type *optimal_interval,
    boolean request_from_app
);

/*=========================================================================
  FUNCTION:  psm_client_enter_psm_with_range

===========================================================================*/
/*!
    @brief
    Used by app to notify its desire to enter PSM mode with interval.
    Application need to pass active_time in secs, time interval for PSM, whether next wake up is for measurement
    purpose or access network. PSM time can be accepted in either broken down format or
    in secs..

    @return
    None.
*/
/*=========================================================================*/
psm_error_type_e psm_client_enter_psm_with_range
(
    int client_id,
    psm_info_interval_type *psm_info
);

/*=========================================================================
  FUNCTION:  psm_util_create_timer

===========================================================================*/
/*!
    @brief
    Utility function provide for applications to create time with specified timeout.

    @return
    None.
*/
/*=========================================================================*/
#if 0
int psm_util_create_timer(struct timeval *timeout,
                          psm_util_timer_expiry_cb_type timer_expiry_cb,
                          void *timer_expiry_cb_data,
                          size_t timer_expiry_cb_data_len);
#endif //0

/*=========================================================================
  FUNCTION:  psm_util_cancel_timer

===========================================================================*/
/*!
    @brief
    Application should inform its intention to use modem services through this API.

    @return
    None.
*/
/*=========================================================================*/
//int psm_util_cancel_timer(int timer_id);

/*=========================================================================
  FUNCTION:  psm_client_validate_module

===========================================================================*/
/*!
    @brief
    Utility function to validate the application credentials

    @return
    None.
*/
/*=========================================================================*/
psm_error_type_e psm_client_validate_module(int client_id);

#endif //PSM_CLIENT_H

