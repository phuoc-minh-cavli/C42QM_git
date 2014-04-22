#include <stdint.h>

#include "tx_api.h"
#include "qurt_txm_qapi_fwk.h"
#include "qapi_sensors.h"

#include "sensor_als_tmd2772.h"
#include "sensor_accel_bmi160.h"

static UINT _handle_bmi160_sensor(qapi_Sensor_API_ID_t api_id)
{
  UINT ret_val = 0xffffffff;
  
  switch(api_id) {
   case QAPI_SENSOR_INIT_E:
    if(bmi160_init() == DAL_SUCCESS)
      ret_val = QAPI_OK;
    break;
   case QAPI_SENSOR_SHUTDOWN_E:
    if(bmi160_shutdown() == DAL_SUCCESS)
      ret_val = QAPI_OK;
    break;

  case QAPI_SENSOR_ENABLE_EVENTS_E:
    if(bmi160_enable_motion_events() == DAL_SUCCESS)
      ret_val = QAPI_OK;
    break;
  case QAPI_SENSOR_DISABLE_EVENTS_E:
    if(bmi160_disable_motion_events() == DAL_SUCCESS)
      ret_val = QAPI_OK;
    break;
  }
  return ret_val;
}


static UINT _handle_tmd2772_sensor(qapi_Sensor_API_ID_t api_id)
{
  UINT ret_val = 0xffffffff;
  
  switch(api_id) {
   case QAPI_SENSOR_INIT_E:
    if(tmd2772_init() == DAL_SUCCESS)
      ret_val = QAPI_OK;
    break;
   case QAPI_SENSOR_SHUTDOWN_E:
    if(tmd2772_shutdown() == DAL_SUCCESS)
      ret_val = QAPI_OK;
    break;

  case QAPI_SENSOR_ENABLE_EVENTS_E:
    if(tmd2772_enable_light_events() == DAL_SUCCESS)
      ret_val = QAPI_OK;
    break;
  case QAPI_SENSOR_DISABLE_EVENTS_E:
    if(tmd2772_disable_light_events() == DAL_SUCCESS)
      ret_val = QAPI_OK;
    break;
  }
  return ret_val;
}

qapi_Status_t qapi_Sensor_Intf(qapi_Sensor_Type_t sensor, qapi_Sensor_API_ID_t api_id)
{
  UINT retval = (UINT)QAPI_ERROR;
  switch(sensor) {
  case QAPI_SENSOR_BMI160_E:
    retval = (UINT)_handle_bmi160_sensor((qapi_Sensor_API_ID_t)api_id);
    break;
  case QAPI_SENSOR_TMD2772_E:
    retval = (UINT)_handle_tmd2772_sensor((qapi_Sensor_API_ID_t)api_id);
    break;
  default:
    break;
  }
  return retval;
}
static qapi_Sensor_Callback_t app_motion_event_handler = NULL;
static qapi_Sensor_Callback_t app_light_event_handler  = NULL;

static void _intr_motion_event_handler(int motion_direction)
{
  static int accel_motion_direction = 0;
  
  accel_motion_direction = motion_direction;
  
  if(app_motion_event_handler != NULL)
    app_motion_event_handler(&accel_motion_direction);
}

static void _intr_light_event_handler(void)
{
  if(app_light_event_handler != NULL)
    app_light_event_handler(NULL);
}


qapi_Status_t qapi_Sensor_Set_Callback(qapi_Sensor_Type_t sensor, qapi_Sensor_Callback_t callback)
{
  UINT retval = (UINT)QAPI_ERROR;

  if(callback != NULL) {
    switch(sensor) {
    case QAPI_SENSOR_BMI160_E:
      app_motion_event_handler = callback;
      bmi160_attach_motion_event_handler(_intr_motion_event_handler);
      retval = QAPI_OK;
      break;
    case QAPI_SENSOR_TMD2772_E:
      app_light_event_handler = callback;
      tmd2772_attach_light_event_handler(_intr_light_event_handler);
      retval = QAPI_OK;
      break;
    default:
      break;
    }
  }
  return retval;
}

qapi_Status_t qapi_Sensor_Set_Threshold(qapi_Sensor_Type_t sensor, const qapi_Sensor_Threshold_t *data)
{
  UINT retval = (UINT)QAPI_ERROR;
  
  if(data != NULL) {
    switch(sensor) {
    case QAPI_SENSOR_BMI160_E:
      if(bmi160_set_motion_event_threshold(data->data.accel.threshold) == DAL_SUCCESS) {
        retval = QAPI_OK;
      }
      break;
    case QAPI_SENSOR_TMD2772_E:
      if(tmd2772_set_light_event_threshold(data->data.lux.threshold_low, data->data.lux.threshold_high) == DAL_SUCCESS) {
        retval = QAPI_OK;
      }
      break;

    default:
      break;
    }
  }
  return retval;
}

qapi_Status_t qapi_Sensor_Get_Data(qapi_Sensor_Type_t sensor, qapi_Sensor_Data_t *data)
{
  UINT retval = (UINT)QAPI_ERROR;
  
  if(data != NULL) {
    switch(sensor) {
    case QAPI_SENSOR_BMI160_E:
      if(bmi160_get_accel(&data->data.accel.x, &data->data.accel.y,
                          &data->data.accel.z) == DAL_SUCCESS) {
        retval = QAPI_OK;
      }
      break;
    case QAPI_SENSOR_TMD2772_E:
      data->data.lux = tmd2772_get_lux();
      retval = QAPI_OK;
      break;

    default:
      break;
    }
  }
  return retval;
}

/*QAPI Device Info Handler*/
UINT qapi_Sensor_API_Handler(UINT id,
    UINT sensor,
    UINT b,
    UINT c,
    UINT d,
    UINT e,
    UINT f,
    UINT g,
    UINT h,
    UINT i,
    UINT j,
    UINT k,
    UINT l)
{
  UINT retval = (UINT)QAPI_ERROR;

  switch(id) {
    case TXM_QAPI_SENSOR_API:
      retval = qapi_Sensor_Intf((qapi_Sensor_Type_t)sensor, (qapi_Sensor_API_ID_t)b);
      break;
    
    case TXM_QAPI_SENSOR_API_SET_THRESHOLD:
      retval = qapi_Sensor_Set_Threshold((qapi_Sensor_Type_t)sensor, (qapi_Sensor_Threshold_t *)b);
      break;

    case TXM_QAPI_SENSOR_API_SET_CALLBACK:
      retval = qapi_Sensor_Set_Callback((qapi_Sensor_Type_t)sensor, (qapi_Sensor_Callback_t)b);
      break;
    
    case TXM_QAPI_SENSOR_API_GET_DATA:
      retval = qapi_Sensor_Get_Data((qapi_Sensor_Type_t)sensor, (qapi_Sensor_Data_t *)b);
      break;
    default:
      break;
  }
  return (UINT)retval;
}

void register_qapi_sensor_txm()
{
  qurt_qapi_register(TXM_QAPI_SENSOR_BASE + 1, TXM_QAPI_SENSOR_NUM_IDS, qapi_Sensor_API_Handler);
}
