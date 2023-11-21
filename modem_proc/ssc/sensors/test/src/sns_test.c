/**
 * @file sns_test.c
 *
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * $Id: //components/rel/ssc.slpi/5.1/sensors/test/src/sns_test.c#1 $
 * $DateTime: 2019/04/24 05:02:39 $
 * $Change: 18986584 $
 *
 **/
#include "sns_rc.h"
#include "sns_register.h"
#include "sns_test_sensor.h"
#include "sns_test_sensor_instance.h"

/** Public Function Definitions. */

sns_rc sns_test_register(sns_register_cb const *register_api)
{
  register_api->init_sensor(sizeof(sns_test_state), &sns_test_sensor_api,
      &sns_test_sensor_instance_api);

  return SNS_RC_SUCCESS;
}
