/*===========================================================================

  sensor-adc.c

  DESCRIPTION
  thermisor sensor access functions.

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  setup() function should be called before get_temperature().
  shutdown() function should be called to clean up resources.

  Copyright (c) 2013 QUALCOMM Technologies Incorporated. All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

===========================================================================*/

#include <string.h>
#include <stdlib.h>

#include "DALSys.h"
#include "DDIAdc.h"
#include "sensors-adc.h"
#include "sensors_manager_internal.h"
#include "thermal.h"
#include "therm_log.h"
#include "therm_diag.h"

#define PA_CONV(x,n) ((x)/(n))

struct pa_sens_data
{
  int sensor_shutdown;
  struct sensor_info *sensor;
  const char *pszChannelName;
  AdcInputPropertiesType channel;
  AdcResultType reading;
  qurt_mutex_t pa_sens_mutex;
  qurt_cond_t pa_sens_condition;
  int threshold_reached;
};

/* TSENS DAL devince handle */
static DalDeviceHandle *phPSENSDALDevHandle = NULL;

static uint32 numSensors;

static qurt_mutex_t handle_mutex;

/* NOTE: adc_sensors_setup() function does not enable the sensor
 * or set thresholds. This should be done in the target-specific setup */
int adc_sensors_setup(struct sensor_info *sensor)
{
  int sensor_count = 0;
  struct pa_sens_data *pa_sens = NULL;
  static uint8_t init = 0;
  DALResult result;

  if (!init)
  {
    if (DAL_SUCCESS !=DAL_AdcDeviceAttach(DALDEVICEID_ADC, &phPSENSDALDevHandle))
    {
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 0,
                       "FAILED to attach to ADC DAL");
      return 0;
    }
    qurt_mutex_init(&handle_mutex);
    init = 1;
  }

  /* Allocate ADC data */
  pa_sens = Core_Malloc(sizeof(struct pa_sens_data));
  if (NULL == pa_sens)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: malloc failed", __func__);
    return 0;
  }
  
  memset(pa_sens, 0, sizeof(struct pa_sens_data));
  result = DalAdc_GetAdcInputProperties(phPSENSDALDevHandle, sensor->name, strlen(sensor->name)+1, &pa_sens->channel);
  
  if (result != DAL_SUCCESS) {
	  therm_log_printf(THERM_LOG_LEVEL_ERROR, 0,
                       "%s:FAILED to get channel propertiess",__func__);
      return 0;
  }
  
  sensor->data = (void *)pa_sens;
  
  qurt_mutex_init(&(pa_sens->pa_sens_mutex));
  qurt_cond_init(&(pa_sens->pa_sens_condition));
  pa_sens->threshold_reached = 0;
  pa_sens->sensor_shutdown = 0;
  pa_sens->sensor = sensor;
  sensor_count++;

  return sensor_count;
}

int adc_sensor_get_temperature(struct sensor_info *sensor)
{
  diag_therm_packet therm_packet;
  struct pa_sens_data *pa_sens = NULL;
  DALResult result;
  int temp = 0;
  
  if (NULL == sensor ||
      NULL == sensor->data)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: unexpected NULL",
                     __func__);
    return 0;
  }
  pa_sens = (struct pa_sens_data *)sensor->data;
  
  qurt_mutex_lock(&handle_mutex);
  result = DalAdc_Read(phPSENSDALDevHandle, &pa_sens->channel, &pa_sens->reading);
  qurt_mutex_unlock(&handle_mutex);
  
  if (result != DAL_SUCCESS) {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: Get Temp failed",
                     __func__);
    return 0;
  }
  temp = PA_CONV(pa_sens->reading.nPhysical,sensor->temp_conv);
  
  

  therm_packet.version = 0x1;
  therm_packet.num_samples = 1;

  therm_packet.samples[0].subsystem_id = strncmp(sensor->name,"XO_THERM",strlen(sensor->name)) ? PA_THERM_SS : XO_THERM_SS;
  therm_packet.samples[0].sensor_id    = sensor->tzn;
  therm_packet.samples[0].temp         = temp;
  therm_sensor_diag_log_reading(&therm_packet, sensor->name);

  return temp;
}
