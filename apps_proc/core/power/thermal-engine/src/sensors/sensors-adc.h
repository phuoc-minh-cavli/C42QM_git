/*===========================================================================

  Copyright (c) 2013 QUALCOMM Technologies Incorporated. All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

===========================================================================*/
#ifndef __ADC_SENSOR_H__
#define __ADC_SENSOR_H__

#include "sensors_manager_internal.h"

int adc_sensors_setup(struct sensor_info *sensor);
//void tsens_sensors_shutdown(struct sensor_info *sensor);
int adc_sensor_get_temperature(struct sensor_info *sensor);
//void tsens_sensor_interrupt_wait(struct sensor_info *sensor);
//void tsens_sensor_update_thresholds(struct sensor_info *sensor, struct thresholds_req_t *thresh);
//void tsens_sensor_enable_thresholds(struct sensor_info *sensor, int hi_enabled, int lo_enabled);
//void tsens_sensor_enable_sensor(struct sensor_info *sensor, int enabled);

#endif  /* __TSENS_SENSOR_H__ */
