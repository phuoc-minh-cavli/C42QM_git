/*****************************************************************************
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*****************************************************************************/
#ifndef __GNSS_WORKER_GEOFENCE_SM_H__
#define __GNSS_WORKER_GEOFENCE_SM_H__

#include <stdint.h>
#include <qapi_location.h>
#include "gnss_worker_thread.h"

// Init
void gnss_worker_geofence_sm_init();

// INCOMING REQUESTS
void gnss_worker_geofence_sm_request_add_geofence(
        double latitude, double longitude, double radius,
        qapi_Geofence_Breach_Mask_t mask, uint32 responsiveness, uint32 dwellTime,
        uint32 allowPriorityInversion);
void gnss_worker_geofence_sm_request_remove_geofence();

// QAPI LOCATION CALLBACKS
void gnss_worker_geofence_sm_breach_cb(
        qapi_Location_t location, qapi_Geofence_Breach_t breachType, uint64_t timestamp);
void gnss_worker_geofence_sm_meta_data_cb(qapi_Location_Meta_Data_t metaData);

#endif /* __GNSS_WORKER_GEOFENCE_SM_H__ */
