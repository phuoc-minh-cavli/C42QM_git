/*===========================================================================

  sensor-tsens.c

  DESCRIPTION
  TSENS sensor access functions.

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  setup() function should be called before get_temperature().
  shutdown() function should be called to clean up resources.

  Copyright (c) 2013 QUALCOMM Technologies Incorporated. All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

===========================================================================*/

#include <string.h>
#include <stdlib.h>
//#include <pthread.h>
//#include <unistd.h>

#include "DALSys.h"
#include "DDITsens.h"
#include "sensors-tsens.h"
#include "sensors_manager_internal.h"
#include "thermal.h"
#include "therm_log.h"
#include "therm_diag.h"

#define TSENS_WAIT_TRIGGER     0x0000001

struct tsens_data
{
  int sensor_shutdown;
  //DALSYSEventHandle hEvent;
  //DALSYS_EVENT_OBJECT(hEventObject);
  TsensTempRangeType range;
  struct sensor_info *sensor;
  qurt_mutex_t tsens_mutex;
  qurt_cond_t tsens_condition;
  int threshold_reached;
};

/* TSENS DAL devince handle */
static DalDeviceHandle *phTSENSDALDevHandle = NULL;

static uint32 numSensors;

static qurt_mutex_t handle_mutex;

static void tsens_thresh_cb(void *pCtxt, const TsensCallbackPayloadType *pPayload)
//static void *tsens_thresh_cb(void *user_data, uint32 cb_event_type,
//                             void *cb_data, uint32 size )
{
  struct sensor_info *sensor = (struct sensor_info *)pCtxt;
  struct tsens_data *tsens;

  if (NULL == sensor ||
      NULL == sensor->data)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: unexpected NULL",
                     __func__);
    return;
  }

  tsens = (struct tsens_data *)sensor->data;
  therm_log_printf(THERM_LOG_LEVEL_DEBUG, 2, "%s: %s", __func__, sensor->name);

  /* notify the waiting threads */
  qurt_mutex_lock(&(tsens->tsens_mutex));
  tsens->threshold_reached = 1;
  qurt_cond_broadcast(&(tsens->tsens_condition));
  qurt_mutex_unlock(&(tsens->tsens_mutex));
  return;
}

/* NOTE: tsens_sensors_setup() function does not enable the sensor
 * or set thresholds. This should be done in the target-specific setup */
int tsens_sensors_setup(struct sensor_info *sensor)
{
  int sensor_count = 0;
  struct tsens_data *tsens = NULL;
  static uint8_t init = 0;

  if (!init)
  {
    //DALSYS_InitMod(NULL);

    if (DAL_SUCCESS !=
          DAL_TsensDeviceAttach(DALDEVICEID_TSENS, &phTSENSDALDevHandle))
    {
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 0,
                       "FAILED to attach to TSENS DAL");
      return 0;
    }

    if (DAL_SUCCESS != DalTsens_GetNumSensors(phTSENSDALDevHandle,
                                              &numSensors))
    {
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 0,
                       "FAILED to get number of TSENS sensors");
      return 0;
    }

    qurt_mutex_init(&handle_mutex);
    init = 1;
  }

  if ( sensor->tzn > numSensors)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "Invalid sensor number %d",
                     sensor->tzn);
  }

  /* Allocate TSENS data */
  tsens = Core_Malloc(sizeof(struct tsens_data));
  if (NULL == tsens)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: malloc failed", __func__);
    return 0;
  }
  memset(tsens, 0, sizeof(struct tsens_data));
  sensor->data = (void *)tsens;

  if (DalTsens_GetTempRange(phTSENSDALDevHandle, sensor->tzn, &tsens->range) !=
      DAL_SUCCESS)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: can't get temp range",
                     __func__);
    return 0;
  }

#if 0
  if (DALSYS_EventCreate(DALSYS_EVENT_ATTR_CALLBACK_EVENT, &tsens->hEvent,
                         &tsens->hEventObject) != DAL_SUCCESS)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: can't create event handle",
                     __func__);
    return 0;
  }

  if (DALSYS_SetupCallbackEvent(tsens->hEvent, tsens_thresh_cb, sensor) !=
      DAL_SUCCESS)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: setup event cb",
                     __func__);
    return 0;
  }

  DALSYS_EventCtrl(tsens->hEvent, DALSYS_EVENT_CTRL_RESET);
#endif

  qurt_mutex_init(&(tsens->tsens_mutex));
  qurt_cond_init(&(tsens->tsens_condition));
  tsens->threshold_reached = 0;
  tsens->sensor_shutdown = 0;
  tsens->sensor = sensor;
  sensor_count++;

  return sensor_count;
}

void tsens_sensors_shutdown(struct sensor_info *sensor)
{
  struct tsens_data *tsens;

  if (NULL == sensor ||
      NULL == sensor->data)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: unexpected NULL", __func__);
    return;
  }
  tsens = (struct tsens_data *)sensor->data;
  tsens->sensor_shutdown = 1;
  Core_Free(tsens);
}

int tsens_sensor_get_temperature(struct sensor_info *sensor)
{
  TsensTempType tsensTemp = {0};
  diag_therm_packet therm_packet;

  if (NULL == sensor ||
      NULL == sensor->data)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: unexpected NULL",
                     __func__);
    return 0;
  }

  qurt_mutex_lock(&handle_mutex);
  if (DalTsens_GetTemp(phTSENSDALDevHandle, sensor->tzn, &tsensTemp) !=
      DAL_SUCCESS)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: Get Temp failed",
                     __func__);
    return 0;
  }
  qurt_mutex_unlock(&handle_mutex);

  therm_packet.version = 0x1;
  therm_packet.num_samples = 1;

  therm_packet.samples[0].subsystem_id = MSM_THERM_SS;
  therm_packet.samples[0].sensor_id    = sensor->tzn;
  therm_packet.samples[0].temp         = tsensTemp.nDegC;
  therm_sensor_diag_log_reading(&therm_packet, sensor->name);

  return CONV(tsensTemp.nDegC);
}

void tsens_sensor_interrupt_wait(struct sensor_info *sensor)
{
  struct tsens_data *tsens;

  if (NULL == sensor ||
      NULL == sensor->data)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: unexpected NULL",
                     __func__);
    return;
  }
  if (sensor->interrupt_enable)
  {
    tsens = (struct tsens_data *)sensor->data;

    /* Wait for sensor threshold condition */
    qurt_mutex_lock(&(tsens->tsens_mutex));
    while (!tsens->threshold_reached) {
      therm_log_printf(THERM_LOG_LEVEL_DEBUG, 2, "%s: WAIT %s", __func__, sensor->name);
      qurt_cond_wait(&(tsens->tsens_condition),
                        &(tsens->tsens_mutex));
      therm_log_printf(THERM_LOG_LEVEL_DEBUG, 2, "%s: RCVD %s", __func__, sensor->name);
    }
    tsens->threshold_reached = 0;
    qurt_mutex_unlock(&(tsens->tsens_mutex));
  }
}

void tsens_sensor_update_thresholds(struct sensor_info *sensor,
                                    struct thresholds_req_t *thresh)
{
  struct tsens_data *tsens;
  DALResult result = DAL_SUCCESS;

  if (NULL == thresh ||
      NULL == sensor ||
      NULL == sensor->data)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: unexpected NULL",
                     __func__);
    return;
  }
  tsens = (struct tsens_data *)sensor->data;

  /* Convert thresholds to Celsius for TSENS*/
  thresh->high = RCONV(thresh->high);
  thresh->low = RCONV(thresh->low);

  qurt_mutex_lock(&handle_mutex);
  DalTsens_SetEnableThresholds(phTSENSDALDevHandle, 0);
  if (thresh->high_valid)
  {
    if (thresh->high > tsens->range.nMaxDegC)
    {
      thresh->high = tsens->range.nMaxDegC;
    }
    
    result = DalTsens_SetThreshold(phTSENSDALDevHandle, sensor->tzn,
                                   TSENS_THRESHOLD_UPPER, thresh->high,
                                   (TsensThresholdCbType)tsens_thresh_cb, (void *)sensor);
  }
  else
  {
    result = DalTsens_SetThreshold(phTSENSDALDevHandle, sensor->tzn,
                                   TSENS_THRESHOLD_UPPER,
                                   TSENS_INVALID_THRESHOLD,
                                   (TsensThresholdCbType)tsens_thresh_cb, (void *)sensor);
  }

  if (thresh->low_valid)
  {
    if (thresh->low < tsens->range.nMinDegC)
    {
      thresh->low = tsens->range.nMinDegC;
    }
    result = DalTsens_SetThreshold(phTSENSDALDevHandle, sensor->tzn,
                                   TSENS_THRESHOLD_LOWER, thresh->low,
                                   (TsensThresholdCbType)tsens_thresh_cb, (void *)sensor);
  }
  else
  {
    result = DalTsens_SetThreshold(phTSENSDALDevHandle, sensor->tzn,
                                   TSENS_THRESHOLD_LOWER,
                                   TSENS_INVALID_THRESHOLD,
                                   (TsensThresholdCbType)tsens_thresh_cb, (void *)sensor);
  }

  DalTsens_SetEnableThresholds(phTSENSDALDevHandle, 1);
  qurt_mutex_unlock(&handle_mutex);
  if (result != DAL_SUCCESS)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 3, "%s: %s error %d", __func__,
                     sensor->name, result);
  }
}

