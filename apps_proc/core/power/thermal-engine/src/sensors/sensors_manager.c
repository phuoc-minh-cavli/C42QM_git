/*===========================================================================

  Copyright (c) 2013 QUALCOMM Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

===========================================================================*/
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
//#include <unistd.h>

#include "DALSys.h"
#include "thermal.h"
#include "therm_log.h"
#include "sensors_manager_internal.h"
#include "timer.h"

#define SENSOR_REQ_WAIT_TRIGGER     0x0000001
#define SENSOR_POLL_WAIT_TRIGGER    0x0000002

#define THERM_MUTEX_LOCK(x) qurt_mutex_lock((x)); {
#define THERM_MUTEX_UNLOCK(x) } qurt_mutex_unlock(x);

static struct sensors_mgr_sensor_info *sensor_list;
static int sensor_cnt;
static int alias_cnt;

/* Protect client request collisions  */
//static pthread_mutexattr_t ts_clnt_mtx_attr;
static qurt_mutex_t ts_clnt_mtx;

static void poll_cb( int32 time_ms, timer_cb_data_type data)
{
  struct sensors_mgr_sensor_info *sensor_mgr =
    (struct sensors_mgr_sensor_info *)data;

  qurt_mutex_lock(&(sensor_mgr->poll_wait_mutex));
  qurt_cond_broadcast(&(sensor_mgr->poll_wait_cond));
  qurt_mutex_unlock(&(sensor_mgr->poll_wait_mutex));
}

static struct sensors_mgr_sensor_info* find_sensor(const char *sensor_name)
{
  struct sensors_mgr_sensor_info *curr = sensor_list;

  /* Find dev info */
  while (curr != NULL)
  {
    if (strncmp(sensor_name, curr->name,
                MAX_SENSOR_NAME_LEN) == 0)
    {
      break;
    }

    if ((curr->alias != NULL) &&
        (strncmp(sensor_name, curr->alias,
                 MAX_SENSOR_NAME_LEN) == 0))
      break;
    curr = curr->next_sensor;
  }

  return curr;
}

static int validate_thresh_info(struct sensor_thresh_req *thresh_info)
{
  struct thresholds_req_t *thresh = &(thresh_info->thresh);

  if (thresh_info->notify_cb_func == NULL)
    return -(EINVAL);

  if (thresh->high_valid && thresh->low_valid &&
      (thresh->low >= thresh->high))
    return -(EINVAL);

  if (thresh_info->polling_interval_valid &&
      (thresh_info->polling_interval <= 0))
    return -(EINVAL);

  return 0;
}

static int update_active_thresh(struct sensors_mgr_sensor_info *sensor_mgr)
{
  struct sensor_client_type  *client = NULL;
  struct sensor_thresh_req *active = NULL;
  uint8_t                active_req = 0;


  if (sensor_mgr == NULL)
    return DAL_ERROR_INVALID_PARAMETER;

  active = &sensor_mgr->active_thresh;

  memset(active, 0, sizeof(struct sensor_thresh_req));
  active->thresh.low = INT32_MIN;
  active->thresh.high  = INT32_MAX;
  active->polling_interval = INT32_MAX;

  THERM_MUTEX_LOCK(&ts_clnt_mtx);

  client = sensor_mgr->client_list;
  while (client != NULL)
  {
    if (!client->request_active)
    {
      client = client->next_clnt;
      continue;
    }

    struct sensor_thresh_req *thresh = &client->request;
    /* Find active high */
    if (thresh->thresh.high_valid)
    {
      active->thresh.high_valid = 1;
      active->thresh.high = MIN(active->thresh.high,
                                thresh->thresh.high);
    }

    /* Find active low */
    if (thresh->thresh.low_valid)
    {
      active->thresh.low_valid = 1;
      active->thresh.low = MAX(active->thresh.low,
                               thresh->thresh.low);
    }

    /* Find min polling interval */
    if (thresh->polling_interval_valid)
    {
      active->polling_interval_valid = 1;
      active->polling_interval = MIN(active->polling_interval,
                                     thresh->polling_interval);
    }

    active_req = 1;
    client = client->next_clnt;
  }

  if ((active->thresh.high > active->thresh.low) &&
      (active->thresh.high_valid || active->thresh.low_valid))
  {
    /* We can take advantage of interrupt */
    sensor_mgr->active_thresh_valid = 1;
  }
  else
  {
    sensor_mgr->active_thresh_valid = 0;
  }

  therm_log_printf(THERM_LOG_LEVEL_DEBUG, 9,
                   "%s: %s Active(%d), Hi(%d) %d, Lo(%d) %d, Interval(%d) %d",
                   __func__, sensor_mgr->name,
                   sensor_mgr->active_thresh_valid,
                   active->thresh.high_valid, active->thresh.high,
                   active->thresh.low_valid, active->thresh.low,
                   active->polling_interval_valid,
                   active->polling_interval);

  /* Room for optimization if thresholds didn't change. */
  if (sensor_mgr->active_thresh_valid &&
      sensor_mgr->update_thresholds)
  {
    sensor_mgr->update_thresholds(sensor_mgr);
  }

  if (!sensor_mgr->req_active && active_req)
  {
    /* Advertise there is now an active request available */
    qurt_mutex_lock(&(sensor_mgr->req_wait_mutex));
    sensor_mgr->req_active = 1;
    qurt_cond_broadcast(&(sensor_mgr->req_wait_cond));
    qurt_mutex_unlock(&(sensor_mgr->req_wait_mutex));
  }
  else
  {
    sensor_mgr->req_active = active_req;
  }

  THERM_MUTEX_UNLOCK(&ts_clnt_mtx);
  return 0;
}

static int notify_clnts(struct sensors_mgr_sensor_info *sensor_mgr)
{
  struct sensor_client_type *client = NULL;
  enum sensor_notify_event_type thresh_event;

  if (sensor_mgr == NULL)
    return DAL_ERROR_INVALID_PARAMETER; 

  THERM_MUTEX_LOCK(&ts_clnt_mtx);

  client = sensor_mgr->client_list;
  while (client != NULL)
  {
    if (client->request_active)
    {
      struct thresholds_req_t *thresh = &client->request.thresh;

      /* Notify clients of thresh crossings */
      thresh_event = SENSOR_NOTIFY_NORMAL_THRESH_EVENT;
      if (thresh->high_valid &&
          (sensor_mgr->last_reading >= thresh->high))
      {
        thresh_event = SENSOR_NOTIFY_HIGH_THRESH_EVENT;
      }
      else if (thresh->low_valid &&
               (sensor_mgr->last_reading <= thresh->low))
      {
        thresh_event = SENSOR_NOTIFY_LOW_THRESH_EVENT;
      }

      if (thresh_event != SENSOR_NOTIFY_NORMAL_THRESH_EVENT)
      {
        client->request_active = 0;
        client->request.notify_cb_func(client,
                                       thresh_event,
                                       sensor_mgr->last_reading,
                                       client->request.notify_cb_data);
      }
    }
    client = client->next_clnt;
  }

  THERM_MUTEX_UNLOCK(&ts_clnt_mtx);
  update_active_thresh(sensor_mgr);
  return 0;
}

static int validate_clnt(struct sensor_client_type *clnt)
{
  struct sensors_mgr_sensor_info *sensor_mgr = sensor_list;
  struct sensor_client_type   *sensor_clnt = NULL;
  uint8_t sensor_valid = 0;
  uint8_t clnt_valid = 0;

  if ((clnt == NULL) || (clnt->sensor_mgr == NULL))
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: Invalid args.",
                     __func__);
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /* Validate the device pointer */
  while (sensor_mgr != NULL)
  {
    if (sensor_mgr == clnt->sensor_mgr)
    {
      sensor_valid = 1;
      break;
    }
    sensor_mgr = sensor_mgr->next_sensor;
  }

  if (sensor_valid == 0)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: Invalid sensor.",
                     __func__);
    return -(EFAULT);
  }

  THERM_MUTEX_LOCK(&ts_clnt_mtx);
  /* Validate the client pointer exists for device */
  sensor_clnt = sensor_mgr->client_list;
  while (sensor_clnt != NULL)
  {
    if (sensor_clnt == clnt)
    {
      clnt_valid = 1;
      break;
    }
    sensor_clnt = sensor_clnt->next_clnt;
  }
  THERM_MUTEX_UNLOCK(&ts_clnt_mtx);

  if (clnt_valid == 0)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: Invalid clnt.",
                     __func__);
    return -(EFAULT);
  }
  return 0;
}

int sensors_manager_get_list(struct sensor_info_type *info_arr, int *info_arr_len)
{
  int dev_idx;
  struct sensors_mgr_sensor_info *curr = sensor_list;

  if ((info_arr == NULL) && (info_arr_len == NULL))
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: Invalid args.",
                     __func__);
    return -(EINVAL);
  }

  if (info_arr == NULL)
  {
    /* Interpret as request for number of sensors present. */
    *info_arr_len = sensor_cnt + alias_cnt;
    return 0;
  }

  *info_arr_len = MIN(*info_arr_len, sensor_cnt + alias_cnt);

  curr = sensor_list;
  for (dev_idx = 0; (dev_idx < *info_arr_len) && (curr != NULL); dev_idx++)
  {
    strlcpy(info_arr[dev_idx].name, curr->name, MAX_SENSOR_NAME_LEN);
    if ((curr->alias != NULL) &&
        (++dev_idx < *info_arr_len))
    {
      /* Copy alias name over if possible. */
      strlcpy(info_arr[dev_idx].name, curr->alias, MAX_SENSOR_NAME_LEN);
    }
    curr = curr->next_sensor;
  }

  return 0;
}

sensor_clnt_handle sensors_manager_reg_clnt(const char *sensor_name)
{
  struct sensor_client_type *client = NULL;
  struct sensors_mgr_sensor_info *sensor_mgr = NULL;

  if (sensor_name == NULL)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: Invalid args.",
                     __func__);
    return client;
  }

  sensor_mgr = find_sensor(sensor_name);

  if (sensor_mgr == NULL)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 2, "%s: Invalid sensor %s.",
                     __func__, sensor_name);
    return client;
  }

  client = Core_Malloc(sizeof(struct sensor_client_type));
  if (NULL == client)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: malloc failed.", __func__);
    return client;
  }

  memset(client, 0x0, sizeof(struct sensor_client_type));

  THERM_MUTEX_LOCK(&ts_clnt_mtx);
  /* Insert the client */
  client->sensor_mgr = sensor_mgr;
  client->next_clnt = sensor_mgr->client_list;
  sensor_mgr->client_list = client;
  THERM_MUTEX_UNLOCK(&ts_clnt_mtx);

  return client;
}

int sensors_manager_dereg_clnt(sensor_clnt_handle clnt)
{
  struct sensor_client_type   *prev = NULL;
  struct sensor_client_type   *curr = NULL;
  struct sensors_mgr_sensor_info *sensor_mgr = NULL;
  struct sensor_client_type   *client = clnt;
  int ret_val = 0;

  if (client == NULL)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: Invalid args.",
                     __func__);
    return -(EINVAL);
  }

  ret_val = validate_clnt(client);
  if (ret_val != 0)
    return ret_val;

  sensor_mgr = client->sensor_mgr;

  THERM_MUTEX_LOCK(&ts_clnt_mtx);

  /* Search for client in dev client list */
  curr = sensor_mgr->client_list;
  while (curr != NULL)
  {
    if ((curr == client) && (prev != NULL))
    {
      /* In middle of list */
      prev->next_clnt = curr->next_clnt;
      break;
    }
    else if ((curr == client))
    {
      /* At begining of list */
      sensor_mgr->client_list = curr->next_clnt;
      break;
    }
    prev = curr;
    curr = curr->next_clnt;
  }

  THERM_MUTEX_UNLOCK(&ts_clnt_mtx);

  if (client->request_active)
  {
    /* Update thresholds without client request. */
    update_active_thresh(sensor_mgr);
  }
  Core_Free(client);

  return 0;
}

int sensors_manager_read(sensor_clnt_handle clnt)
{
  struct sensors_mgr_sensor_info *sensor_mgr = NULL;
  struct sensor_client_type   *client = clnt;
  int ret_val = INT32_MIN;

  if (client == NULL)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: Invalid args.",
                     __func__);
    return ret_val;
  }

  if (validate_clnt(client) != 0)
    return ret_val;

  sensor_mgr = client->sensor_mgr;

  ret_val = sensor_mgr->get_temperature(sensor_mgr);
  return ret_val;
}

int sensors_manager_set_thresh_lvl(sensor_clnt_handle clnt,
                                   struct sensor_thresh_req *thresh_info)
{
  struct sensors_mgr_sensor_info *sensor_mgr = NULL;
  struct sensor_client_type   *client = clnt;
  int ret_val = 0;

  if (client == NULL)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: Invalid args.",
                     __func__);
    return DAL_ERROR_INVALID_PARAMETER; 
  }

  ret_val = validate_clnt(client);
  if (ret_val != 0)
    return ret_val;

  sensor_mgr = client->sensor_mgr;

  THERM_MUTEX_LOCK(&ts_clnt_mtx);
  if (thresh_info == NULL)
  {
    /* Clear client request */
    client->request_active = 0;
    therm_log_printf(THERM_LOG_LEVEL_DEBUG, 2,
                     "%s: %s clear threshold request",
                     __func__, sensor_mgr->name);
  }
  else if (validate_thresh_info(thresh_info) == 0)
  {
    client->request = *thresh_info;

    client->request_active = 1;
    therm_log_printf(THERM_LOG_LEVEL_DEBUG, 8,
                     "%s: %s Hi(%d) %d, Lo(%d) %d, Interval(%d) %d",
                     __func__, sensor_mgr->name,
                     thresh_info->thresh.high_valid, thresh_info->thresh.high,
                     thresh_info->thresh.low_valid, thresh_info->thresh.low,
                     thresh_info->polling_interval_valid,
                     thresh_info->polling_interval);
  }
  THERM_MUTEX_UNLOCK(&ts_clnt_mtx);

  /* Update thresholds. */
  update_active_thresh(sensor_mgr);
  return 0;
}

/* Sensor internal manager functions */
static void sensor_wait(struct sensors_mgr_sensor_info *sensor_mgr)
{
  if (sensor_mgr->wait)
  {
    sensor_mgr->wait(sensor_mgr);
  }
  else
  {
    int polling_interval =
      (sensor_mgr->active_thresh.polling_interval_valid) ?
      (sensor_mgr->active_thresh.polling_interval) :
      (SENSOR_DEFAULT_POLLING_INTERVAL);
    therm_log_printf(THERM_LOG_LEVEL_DEBUG, 3, "%s: %s Wait start. %dms",
                     __func__, sensor_mgr->name, polling_interval);
    qurt_mutex_lock(&sensor_mgr->poll_wait_mutex);
/*    timer_reg(&(sensor_mgr->poll_timer), poll_cb,
              (timer_cb_data_type)sensor_mgr, polling_interval, 0);   */
    timer_set(&(sensor_mgr->poll_timer), polling_interval, 0, T_MSEC);
    qurt_cond_wait(&(sensor_mgr->poll_wait_cond),
                      &(sensor_mgr->poll_wait_mutex));
    qurt_mutex_unlock(&sensor_mgr->poll_wait_mutex);
    therm_log_printf(THERM_LOG_LEVEL_DEBUG, 2, "%s: %s Wait done.", __func__,
                     sensor_mgr->name);
  }
}

static void sensor_monitor(void *vsensor_mgr)
{
  struct sensors_mgr_sensor_info *sensor_mgr = vsensor_mgr;

  while (sensor_mgr->thread_shutdown != 1)
  {
    /* Wait here until there is actually a request to process */
    if (!sensor_mgr->req_active)
    {
      therm_log_printf(THERM_LOG_LEVEL_DEBUG, 2,
                       "%s: %s Wait for client request.", __func__,
                       sensor_mgr->name);
      qurt_mutex_lock(&(sensor_mgr->req_wait_mutex));
      while (!sensor_mgr->req_active)
      {
        qurt_cond_wait(&(sensor_mgr->req_wait_cond),
                          &(sensor_mgr->req_wait_mutex));
      }
      qurt_mutex_unlock(&(sensor_mgr->req_wait_mutex));
    }
    therm_log_printf(THERM_LOG_LEVEL_DEBUG, 2, "%s: %s Sensor wait.",
                     __func__, sensor_mgr->name);
    sensor_wait(sensor_mgr);

    sensor_mgr->last_reading = sensor_mgr->get_temperature(sensor_mgr);
    therm_log_printf(THERM_LOG_LEVEL_DEBUG, 3, "%s: %s Reading %d.",
                     __func__, sensor_mgr->name, sensor_mgr->last_reading);

    notify_clnts(sensor_mgr);
  }

  return;
}

int sensors_manager_add_sensor(struct sensors_mgr_sensor_info *sensor_mgr)
{
  char thread_buf[12];
  static uint8_t thread_cnt;
  qurt_thread_attr_t  attr;

  if (sensor_mgr == NULL)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: Invalid args.",
                     __func__);
    return DAL_ERROR_INVALID_PARAMETER; 
  }
  /* Just add new device to begining of list */
  sensor_mgr->next_sensor = sensor_list;
  sensor_list = sensor_mgr;
  sensor_cnt++;
  if (sensor_mgr->alias)
    alias_cnt++;

  qurt_mutex_init(&(sensor_mgr->req_wait_mutex));
  qurt_cond_init(&(sensor_mgr->req_wait_cond));
  qurt_mutex_init(&(sensor_mgr->poll_wait_mutex));
  qurt_cond_init(&(sensor_mgr->poll_wait_cond));
  //timer_def2(&(sensor_mgr->poll_timer), NULL);
  timer_def_osal(&(sensor_mgr->poll_timer), NULL, TIMER_FUNC2_CB_TYPE, poll_cb, (time_osal_notify_data)sensor_mgr);
  snprintf(thread_buf, sizeof(thread_buf), "sensorMon%d", thread_cnt++);
  therm_pthread_attr_init(&attr, thread_buf);
  qurt_thread_attr_set_stack_size(&attr, (size_t)THREAD_4K_STACK_SIZE);
  if( QURT_EOK != qurt_thread_create(&(sensor_mgr->monitor_thread), &attr,
                 sensor_monitor, sensor_mgr))
  {
    CORE_VERIFY(NULL);
  }
  //pthread_attr_destroy(&attr);
  return 0;
}

int sensors_manager_rm_sensor(struct sensors_mgr_sensor_info *sensor_mgr)
{
  struct sensors_mgr_sensor_info *prev = NULL;
  struct sensors_mgr_sensor_info *curr = sensor_list;
  int ret_val = 0;

  if (sensor_mgr == NULL)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: Invalid args.",
                     __func__);
    return DAL_ERROR_INVALID_PARAMETER; 
  }
  /* Find the previous sensor in list */
  while (curr != NULL)
  {
    if ((curr == sensor_mgr) && (prev != NULL))
    {
      /* In middle of list */
      prev->next_sensor = curr->next_sensor;
      break;
    }
    else if ((curr == sensor_mgr))
    {
      /* At begining of list */
      sensor_list = curr->next_sensor;
      break;
    }
    prev = curr;
    curr = curr->next_sensor;
  }
  /* TODO clean up clients */
  sensor_mgr->thread_shutdown = 1;
  qurt_thread_join(sensor_mgr->monitor_thread, NULL);
  qurt_cond_destroy(&(sensor_mgr->req_wait_cond));
  qurt_mutex_destroy(&(sensor_mgr->req_wait_mutex));
  /* destory polling mutex and conditional variables too? */

  if (sensor_mgr->alias)
    alias_cnt--;

  sensor_mgr->shutdown(sensor_mgr->data);
  sensor_cnt--;

  return ret_val;
}

int sensors_manager_init(void)
{
  //pthread_mutexattr_init(&ts_clnt_mtx_attr);
 // pthread_mutexattr_settype(&ts_clnt_mtx_attr, PTHREAD_MUTEX_RECURSIVE);
  qurt_mutex_init(&ts_clnt_mtx);
  therm_log_printf(THERM_LOG_LEVEL_INFO, 1, "%s: Init", __func__);
  return 0;
}

void sensors_manager_release(void)
{
   qurt_mutex_destroy(&ts_clnt_mtx);
  therm_log_printf(THERM_LOG_LEVEL_INFO, 1, "%s: Release", __func__);
}
