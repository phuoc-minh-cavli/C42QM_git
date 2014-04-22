#pragma once
/*=============================================================================
  @file sns_power_sensor.h

  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include "sns_data_stream.h"
#include "sns_sensor_uid.h"
#include "sns_suid_util.h"

/** Sensor power state structure */
typedef struct sns_power_sensor_state
{
  /* Registry and Timer SUID Lookup */
  SNS_SUID_LOOKUP_DATA(2) suid_lookup_data;
  /* Registry data stream */
  sns_data_stream         *reg_stream;
  /* Timer sensor data stream */
  sns_data_stream         *timer_stream;
} sns_power_sensor_state;

