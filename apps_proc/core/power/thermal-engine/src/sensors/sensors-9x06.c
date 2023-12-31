/*===========================================================================

  Copyright (c) 2013-16 QUALCOMM Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

===========================================================================*/
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "thermal.h"
#include "therm_log.h"
#include "sensors-tsens.h"
#include "sensors-adc.h"
#include "sensors.h"
#include "sensors_manager_internal.h"

#if 0
/* PA sensor setup function */
static int  pa_sensor_setup(struct sensor_info *sensor);
#endif 

struct alias_map_t
{
  char *sensor_name;
  char *alias;
};

#if 0
static struct alias_map_t alias_map_9x25[] =
{};
#endif

static struct sensor_info tsens_sensors[] = {
  {
    .name = "tsens_tz_sensor0",
    .setup = tsens_sensors_setup,
    .shutdown = tsens_sensors_shutdown,
    .get_temperature = tsens_sensor_get_temperature,
    .interrupt_wait = tsens_sensor_interrupt_wait,
    .update_thresholds = tsens_sensor_update_thresholds,
    .tzn = 0,
    .data = NULL,
    .interrupt_enable = 1,
  },
  {
    .name = "tsens_tz_sensor1",
    .setup = tsens_sensors_setup,
    .shutdown = tsens_sensors_shutdown,
    .get_temperature = tsens_sensor_get_temperature,
    .interrupt_wait = tsens_sensor_interrupt_wait,
    .update_thresholds = tsens_sensor_update_thresholds,
    .tzn = 1,
    .data = NULL,
    .interrupt_enable = 1,
  },
  {
    .name = "tsens_tz_sensor2",
    .setup = tsens_sensors_setup,
    .shutdown = tsens_sensors_shutdown,
    .get_temperature = tsens_sensor_get_temperature,
    .interrupt_wait = tsens_sensor_interrupt_wait,
    .update_thresholds = tsens_sensor_update_thresholds,
    .tzn = 2,
    .data = NULL,
    .interrupt_enable = 1,
  },
  {
    .name = "tsens_tz_sensor3",
    .setup = tsens_sensors_setup,
    .shutdown = tsens_sensors_shutdown,
    .get_temperature = tsens_sensor_get_temperature,
    .interrupt_wait = tsens_sensor_interrupt_wait,
    .update_thresholds = tsens_sensor_update_thresholds,
    .tzn = 3,
    .data = NULL,
    .interrupt_enable = 1,
  },
  {
    .name = "tsens_tz_sensor4",
    .setup = tsens_sensors_setup,
    .shutdown = tsens_sensors_shutdown,
    .get_temperature = tsens_sensor_get_temperature,
    .interrupt_wait = tsens_sensor_interrupt_wait,
    .update_thresholds = tsens_sensor_update_thresholds,
    .tzn = 4,
    .data = NULL,
    .interrupt_enable = 1,
  }
};

#if 1
static struct sensor_info pa_sensors[] = {
  {
    .name = "PA_THERM",
    .setup = adc_sensors_setup,
    .shutdown = NULL,
    .get_temperature = adc_sensor_get_temperature,
    .interrupt_wait = NULL,
    .update_thresholds = NULL,
    .tzn = 0,
    .data = NULL,
    .interrupt_enable = 0,
    .temp_conv = 1,
  },
  {
    .name = "PA_THERM1",
    .setup = adc_sensors_setup,
    .shutdown = NULL,
    .get_temperature = adc_sensor_get_temperature,
    .interrupt_wait = NULL,
    .update_thresholds = NULL,
    .tzn = 0,
    .data = NULL,
    .interrupt_enable = 0,
    .temp_conv = 1,
  },
  {
    .name = "XO_THERM",
    .setup = adc_sensors_setup,
    .shutdown = NULL,
    .get_temperature = adc_sensor_get_temperature,
    .interrupt_wait = NULL,
    .update_thresholds = NULL,
    .tzn = 0,
    .data = NULL,
    .interrupt_enable = 0,
    .temp_conv = 1000,
  }
};
#endif

static char* find_alias(const char *sensor_name)
{
  char               *ret_val = NULL;
  static uint8_t      init;
  static struct alias_map_t *arr;
  static uint8_t      arr_len;

  if (init == 0)
  {
#if 0
    switch (therm_get_msm_id())
    {
    case THERM_MSM_9x25:
      arr = alias_map_9x25;
      arr_len = ARRAY_SIZE(alias_map_9x25);
      break;
    default:
      msg("%s: Uknown B-family device", __func__);
      break;
    }
#endif
    init = 1;
  }

  if (arr != NULL)
  {
    uint8_t idx;
    for (idx = 0; idx < arr_len; idx++)
    {
      if (strncmp(sensor_name, arr[idx].sensor_name,
                  MAX_SENSOR_NAME_LEN) == 0)
      {
        ret_val = arr[idx].alias;
        break;
      }
    }
  }
  return ret_val;
}

static void generic_wait(struct sensors_mgr_sensor_info *sensor_mgr)
{
  struct sensor_info *sensor = (struct sensor_info *)sensor_mgr->data;
  sensor->interrupt_wait(sensor);
}

static int generic_read(struct sensors_mgr_sensor_info *sensor_mgr)
{
  struct sensor_info *sensor = (struct sensor_info *)sensor_mgr->data;
  return sensor->get_temperature(sensor);
}

static void generic_update_thresholds(struct sensors_mgr_sensor_info *sensor_mgr)
{
  struct sensor_info *sensor = (struct sensor_info *)sensor_mgr->data;
  sensor->update_thresholds(sensor,
                            &sensor_mgr->active_thresh.thresh);
}

static void generic_shutdown(struct sensors_mgr_sensor_info *sensor_mgr)
{
  struct sensor_info *sensor = (struct sensor_info *)sensor_mgr->data;
  sensor->shutdown(sensor);

  Core_Free(sensor_mgr);
}

#if 0 /*DISABLED ON PUROSE */
static int  pa_sensor_setup(struct sensor_info *sensor)
{
  int sensor_count = 0;

  sensor_count = adc_sensors_setup(sensor);
  if (sensor_count)
  {
    msg("ADC sensor found for %s\n", sensor->name);
    sensor->shutdown        = adc_sensors_shutdown;
    sensor->get_temperature = adc_sensor_get_temperature;
  }
  else
  {
    sensor_count = qmi_ts_setup(sensor);
    if (sensor_count)
    {
      msg("QMI TS sensor found for %s\n", sensor->name);
      sensor->update_thresholds = qmi_ts_update_thresholds;
      sensor->get_temperature   = qmi_ts_get_temperature;
      sensor->interrupt_wait    = qmi_ts_interrupt_wait;
      sensor->shutdown          = qmi_ts_shutdown;
      sensor->interrupt_enable  = 1;
    }
  }
  return sensor_count;
}
#endif

static int add_tgt_sensor(struct sensor_info *sensor)
{
  int ret_val = 0;
  struct sensors_mgr_sensor_info *sensor_mgr = NULL;

  sensor_mgr = (struct sensors_mgr_sensor_info *)Core_Malloc(sizeof(struct sensors_mgr_sensor_info));

  if (NULL == sensor_mgr)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: malloc failed.",
                     __func__);
    ret_val = -(ENOMEM);
    goto error_handler;
  }
  memset(sensor_mgr, 0, sizeof(struct sensors_mgr_sensor_info));

  sensor_mgr->name = sensor->name;
  sensor_mgr->alias = find_alias(sensor->name);

  if (sensor->setup(sensor) == 0)
  {
    ret_val = -(EFAULT);
    goto error_handler;
  }

  sensor_mgr->data = (void *)sensor;
  sensor->sensor_mgr = sensor_mgr;

  sensor_mgr->get_temperature = generic_read;
  sensor_mgr->shutdown = generic_shutdown;

  if (sensor->interrupt_wait)
    sensor_mgr->wait = generic_wait;
  if (sensor->update_thresholds)
    sensor_mgr->update_thresholds = generic_update_thresholds;

  sensors_manager_add_sensor(sensor_mgr);

error_handler:
  if (ret_val)
  {
    if (sensor_mgr)
      Core_Free(sensor_mgr);
  }
  return ret_val;
}

int add_tgt_sensors_set(struct sensor_info *sensor_arr, int arr_size)
{
  int idx = 0;
  int ret_val = 0;

  for (idx = 0; idx < arr_size; idx++)
  {
    if (add_tgt_sensor(&sensor_arr[idx]) != 0)
    {
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 2, "%s: Error adding %s",
                       __func__, sensor_arr[idx].name);
      ret_val = -(EFAULT);
      break;
    }
  }

  return ret_val;
}

int sensors_init(int minimum_mode)
{
  int ret_val = 0;

#if 0 /* DISABLED ON PURPOSE */
  if (!min_mode)
    modem_ts_qmi_init();
#endif

  if (add_tgt_sensors_set(tsens_sensors,
                          ARRAY_SIZE(tsens_sensors)) != 0)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: Error adding TSENS TS",
                     __func__);
    ret_val = -(EFAULT);
  }

  /* PA */
  if (add_tgt_sensors_set(pa_sensors,
                          ARRAY_SIZE(pa_sensors)) != 0)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: Error adding ADC TS",
                     __func__);
    ret_val = -(EFAULT);
  }
#if 0 /* DISABLED ON PURPOSE */
  /* GEN */
  if (add_tgt_sensors_set(gen_sensors,
                          ARRAY_SIZE(gen_sensors)) != 0)
  {
    msg("%s: Error adding GEN TS\n", __func__);
    ret_val = -(EFAULT);
  }

  /* BCL */
  if (add_tgt_sensors_set(bcl_sensors,
                          ARRAY_SIZE(bcl_sensors)) != 0)
  {
    msg("%s: Error adding BCL TS\n", __func__);
    ret_val = -(EFAULT);
  }
#endif

  return ret_val;
}

void sensors_release(void)
{
#if 0 /* DISABLED ON PURPOSE */
  if (!min_mode)
    modem_qmi_ts_comm_release();
#endif
}
