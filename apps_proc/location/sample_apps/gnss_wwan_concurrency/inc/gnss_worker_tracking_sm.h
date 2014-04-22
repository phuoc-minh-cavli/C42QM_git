/*****************************************************************************
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*****************************************************************************/
#ifndef __GNSS_WORKER_TRACKING_SM_H__
#define __GNSS_WORKER_TRACKING_SM_H__

#include <stdint.h>
#include <qapi_location.h>
#include "gnss_worker_thread.h"

// Init
void gnss_worker_tracking_sm_init();

// INCOMING REQUESTS
void gnss_worker_tracking_sm_request_tracking_start(
        uint32 tbf, uint32 dbf, qapi_Location_Accuracy_Level_t accuracy,
        uint32 allowPriorityInversion);
void gnss_worker_tracking_sm_request_tracking_stop();

// QAPI LOCATION CALLBACKS
void gnss_worker_tracking_sm_tracking_cb(qapi_Location_t location);
void gnss_worker_tracking_sm_meta_data_cb(qapi_Location_Meta_Data_t metaData);
void gnss_worker_tracking_sm_motion_info_cb(qapi_Location_Motion_Info_t motionInfo);
void gnss_worker_tracking_sm_nmea_cb(qapi_Gnss_Nmea_t nmea);

#endif /* __GNSS_WORKER_TRACKING_SM_H__ */
