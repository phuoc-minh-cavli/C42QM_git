/*===========================================================================

Copyright (c) 2013,2015 QUALCOMM Technologies Incorporated.  All Rights Reserved.
QUALCOMM Proprietary and Confidential.

===========================================================================*/


//#include <sys/types.h>
#include <limits.h>
#include <string.h>
#include "therm_log.h"
#include "thermal.h"
#include "thermal_config.h"
#include "sensors_manager.h"
#include "devices_manager.h"
#include "DALSys.h"

#define MAX_TM_INSTANCES_SUPPORTED 32
#define MAX_ACTIONS_PER_TM_INSTANCE 16
#define TM_WAIT_TRIGGER     0x0000001



static qurt_thread_t       tm_thread;
static qurt_cond_t         wait_cond;
static uint32_t            thresh_reached_mask;
static qurt_mutex_t        wait_mutex;

struct tm_action
{
  struct device_info *dev_info;
  int    dev_clnt_idx;
};

struct tm_threshold
{
  struct tm_action action[ACTIONS_MAX];
};

struct tm_instance_info
{
  struct setting_info *setting;
  int                  last_lvl;
  unsigned int         action_mask;
  int                  lvl_alarm[THRESHOLDS_MAX];
  device_clnt_handle   dev_clnt_list[MAX_ACTIONS_PER_TM_INSTANCE];
  struct device_info  *dev_info_list[MAX_ACTIONS_PER_TM_INSTANCE];
  sensor_clnt_handle   ts_clnt;
  uint8_t              disable;
};

static struct tm_instance_info tm_states[MAX_TM_INSTANCES_SUPPORTED];
static int tm_cnt;

static struct device_info *device_info_arr;
static int                 device_info_arr_len;

/* vdd_restriction client for thermal_monitor init. */
static device_clnt_handle vrestrict_clnt;

/* mx_min_request client for thermal_monitor init. */
static device_clnt_handle mx_min_req_clnt;

/* cx_min_request client for thermal_monitor init. */
static device_clnt_handle cx_min_req_clnt;

/* cpr_cold_request client for thermal_monitor init. */
static device_clnt_handle cpr_cold_req_clnt;

static int report_msg(device_clnt_handle clnt, const char *sensor,
                      int temperature, int level, int is_trigger)
{
  char tempBuf[REPORT_MSG_MAX];
  union device_request req;

  therm_log_printf(THERM_LOG_LEVEL_INFO, 4, "ACTION: REPORT - Sensor '%s' - "
                   "temperature %d, threshold level %d, is_trigger %s",
                   sensor, temperature, level + 1,
                   is_trigger ? "true" : "false");

  snprintf(tempBuf, REPORT_MSG_MAX, "%s\n%d\n%d\n%s",
           sensor, temperature, level + 1,
           is_trigger ? "true" : "false");

  req.data = (void *)tempBuf;
  device_clnt_request(clnt, &req);
  return 0;
}

struct tm_devices_list
{
  char *device[MAX_ACTIONS_PER_TM_INSTANCE];
  int   cnt;
};

static struct device_info* get_device_info(const char *device)
{
  int i;

  struct device_info *info = NULL;
  for (i = 0; i < device_info_arr_len; i++)
  {
    if (strncasecmp(device_info_arr[i].name, device,
                    DEVICES_MAX_NAME_LEN) == 0)
      info = &device_info_arr[i];
  }
  return info;
}

/* Add device to list if needed, and return position it was added or already
   exists. List will be filled in order. Return -1 if can not be added. */
static int add_device_to_list(struct tm_instance_info *tm_instance_info,
                              struct tm_devices_list *list, const char *device)
{
  int i;

  /* Search for match or first available slot. */
  for (i = 0; i < MAX_ACTIONS_PER_TM_INSTANCE; i++)
  {
    /* Add to first empty entry, if no previous match. */
    if (list->device[i] == NULL)
      break;
    if (strncasecmp(list->device[i], device,
                    DEVICES_MAX_NAME_LEN) == 0)
      break;
  }

  if (i >= MAX_ACTIONS_PER_TM_INSTANCE)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 2, "%s: No room for device %s",
                     __func__, device);
    return -1;
  }

  /* Check if we need to create the device client */
  if (list->device[i] == NULL)
  {
    tm_instance_info->dev_info_list[i] = get_device_info(device);
    if (tm_instance_info->dev_info_list[i] == NULL)
      return -1;
    tm_instance_info->dev_clnt_list[i] =
      devices_manager_reg_clnt(device);
    if (tm_instance_info->dev_clnt_list[i] == NULL)
      return -1;
    list->device[i] = (char *)device;
    list->cnt++;
  }

  return i;
}
static int create_device_clnts(struct tm_instance_info *tm_instance_info)
{
  int ret_val = 0;
  int t_idx, a_idx;
  struct tm_devices_list list;
  struct tm_setting *tm_setting_info = &tm_instance_info->setting->data.tm;

  memset(&list, 0x0, sizeof(struct tm_devices_list));
  /* Create list of unique actions */
  for (t_idx = 0; t_idx < tm_setting_info->num_thresholds; t_idx++)
  {
    for (a_idx = 0; a_idx < tm_setting_info->t[t_idx].num_actions;
         a_idx++)
    {
      /* Index used by tm to make requests on correct device
         client */
      tm_setting_info->t[t_idx].actions[a_idx].device_idx  =
        add_device_to_list(tm_instance_info, &list,
                           tm_setting_info->t[t_idx].actions[a_idx].device);
      if (tm_setting_info->t[t_idx].actions[a_idx].device_idx < 0)
      {
        therm_log_printf(THERM_LOG_LEVEL_ERROR, 2, "%s: Error adding device %s",
                         __func__,
                         tm_setting_info->t[t_idx].actions[a_idx].device);
        ret_val = -(EFAULT);
        goto error_handler;
      }
    }
  }

error_handler:
  return ret_val;
}

static int sensors_setup(void)
{
  int i = 0;
  int sensor_count = 0;

  if (!tm_cnt)
    return 0;

  /* Set up tm instances */
  therm_log_printf(THERM_LOG_LEVEL_INFO, 2, "%s: tm_cnt %d", __func__, tm_cnt);
  for (i = 0; i < tm_cnt; i++)
  {
    struct tm_instance_info *tm_instance_info;
    struct setting_info *setting;
    struct tm_setting *tm_setting_info;

    tm_instance_info = &tm_states[i];
    setting = tm_instance_info->setting;
    tm_setting_info = &(setting->data.tm);

    therm_log_printf(THERM_LOG_LEVEL_INFO, 4,
                     "%s: TM Id %s Sensor %s num_thresholds %d", __func__,
                     setting->desc, tm_setting_info->sensor,
                     tm_setting_info->num_thresholds);
    if ((tm_setting_info->num_thresholds > 0) ||
        (tm_instance_info->disable == 0))
    {
      /* Create sensor client */
      tm_instance_info->ts_clnt =
        sensors_manager_reg_clnt(tm_setting_info->sensor);
      if (tm_instance_info->ts_clnt == NULL)
      {
        therm_log_printf(THERM_LOG_LEVEL_ERROR, 2,
                         "%s: Can't create client for %s.\n", __func__,
                         tm_setting_info->sensor);
        tm_instance_info->disable = 1;
        continue;
      }

      /* Create necessary device clients */
      if (create_device_clnts(tm_instance_info) == 0)
        sensor_count++;
      else
        tm_instance_info->disable = 1;
    }
  }
  return sensor_count;
}

static void sensors_shutdown(void)
{
  int i = 0;
  int a_idx = 0;
  /* Set up sensors */
  for (i = 0; i < tm_cnt; i++)
  {
    /* Destroy ts client */
    if (tm_states[i].ts_clnt)
      sensors_manager_dereg_clnt(tm_states[i].ts_clnt);

    /* Destroy tmd clients */
    for (a_idx = 0; a_idx < MAX_ACTIONS_PER_TM_INSTANCE; a_idx++)
    {
      if (tm_states[i].dev_clnt_list[a_idx])
        devices_manager_dereg_clnt(tm_states[i].dev_clnt_list[a_idx]);
    }
  }
}

static int sensor_threshold_trigger(int value, struct tm_instance_info *sensor, int level)
{
  struct tm_setting *tm_info = &(sensor->setting->data.tm);

  if (tm_info->descending_thresh)
  {
    if (value <= tm_info->t[level].lvl_trig)
      return 1;
    else
      return 0;
  }
  else
  {
    if (value >= tm_info->t[level].lvl_trig)
      return 1;
    else
      return 0;
  }
}

static int sensor_threshold_clear(int value, struct tm_instance_info *sensor, int level)
{
  struct tm_setting *tm_info = &(sensor->setting->data.tm);

  if (tm_info->descending_thresh)
  {
    if (value >= tm_info->t[level].lvl_clr)
      return 1;
    else
      return 0;
  }
  else
  {
    if (value <= tm_info->t[level].lvl_clr)
      return 1;
    else
      return 0;
  }
}


static int sensor_get_temperature(struct tm_instance_info *setting)
{
  int temp = 0;

  if (setting == NULL ||
      setting->ts_clnt == NULL)
  {
    return -EFAULT;
  }

  temp = sensors_manager_read(setting->ts_clnt);
  therm_log_printf(THERM_LOG_LEVEL_DEBUG, 4,"TM Id %s Sensor %s Reading %d.%d",
                   setting->setting->desc, setting->setting->data.tm.sensor,
                   RCONV(temp), ((temp % 1000) / 100));
  return temp;
}

static void sensor_thresh_notify(sensor_clnt_handle  clnt,
                                 enum sensor_notify_event_type   event,
                                 int                    reading,
                                 void                  *data)
{
  if (NULL == clnt)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: unexpected NULL",
                     __func__);
    return;
  }

  if (((int)data) >= tm_cnt)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 2, "%s: unexpected idx %d",
                     __func__, (int)data);
    return;
  }

  therm_log_printf(THERM_LOG_LEVEL_DEBUG, 3,"%s: Update recieved %s %d",
                   __func__, tm_states[(int)data].setting->desc, reading);

  /* Notify the waiting thread */
  qurt_mutex_lock(&wait_mutex);
  thresh_reached_mask |= (0x1 << (int)data);
  qurt_cond_broadcast(&wait_cond);
  qurt_mutex_unlock(&wait_mutex);
}

static void sensor_update_thresholds(struct tm_instance_info *setting,
                                     int threshold_type, int level,
                                     int idx)
{
  struct sensor_thresh_req ts_thresh_req;
  struct thresholds_req_t *thresh = &ts_thresh_req.thresh;
  struct tm_setting *tm_info = &(setting->setting->data.tm);

  if (setting == NULL  ||
      setting->ts_clnt == NULL)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: Unexpected NULL",
                     __func__);
    return;
  }

  memset(&ts_thresh_req, 0, sizeof(struct sensor_thresh_req));

  ts_thresh_req.notify_cb_func = sensor_thresh_notify;
  ts_thresh_req.notify_cb_data = (void *)idx;
  ts_thresh_req.polling_interval_valid = 1;
  ts_thresh_req.polling_interval = tm_info->sampling_period_ms;
  thresh->high_valid = 1;
  thresh->low_valid = 1;

  therm_log_printf(THERM_LOG_LEVEL_DEBUG, 5,
                   "%s: TM Id %s Sensor %s threshold_type %d, level %d",
                   __func__, setting->setting->desc, tm_info->sensor,
                   threshold_type, level);
  if (tm_info->descending_thresh)
  {
    /* Falling Trigger */
    if (level >= tm_info->num_thresholds)
    {
      /* handle corner low case */
      thresh->low = tm_info->t[tm_info->num_thresholds - 1].lvl_trig;
      thresh->low_valid = 0;
    }
    else
      thresh->low = tm_info->t[level].lvl_trig;

    if (level <= 0)
    {
      /* handle corner high case */
      thresh->high = tm_info->t[0].lvl_clr;
      thresh->high_valid = 0;
    }
    else
      thresh->high = tm_info->t[level - 1].lvl_clr;
  }
  else
  {
    /* Rising trigger */
    if (level >= tm_info->num_thresholds)
    {
      /* handle corner high case */
      thresh->high = tm_info->t[tm_info->num_thresholds - 1].lvl_trig;
      thresh->high_valid = 0;
    }
    else
      thresh->high = tm_info->t[level].lvl_trig;

    if (level <= 0)
    {
      /* handle corner low case */
      thresh->low = tm_info->t[0].lvl_clr;
      thresh->low_valid = 0;
    }
    else
      thresh->low = tm_info->t[level - 1].lvl_clr;
  }
  sensors_manager_set_thresh_lvl(setting->ts_clnt, &ts_thresh_req);
}

static void clear_all_alarms(struct tm_instance_info *sensor)
{
  int i;

  for (i = 0; i < MAX_ACTIONS_PER_TM_INSTANCE; i++)
  {
    if (sensor->dev_clnt_list[i] == NULL)
      continue;

    /* check if action may have been set */
    if ((sensor->action_mask & (1 << i)) == 0)
      continue;

    switch (sensor->dev_info_list[i]->dev_type)
    {
    case DEVICE_GENERIC_TYPE:
    case DEVICE_OP_VALUE_TYPE:
      device_clnt_cancel_request(sensor->dev_clnt_list[i]);
      break;
    default:
      therm_log_printf(THERM_LOG_LEVEL_DEBUG, 2,"%s: No clearing of action %s",
                       __func__, sensor->dev_info_list[i]->name);
    }
  }

  sensor->action_mask = 0;
}

static void handle_thresh_sig(void)
{
  int i, j, idx;
  int max_thr;
  int alarm_raised = 0, alarm_cleared = 0;
  int sensor_temp = 0;
  int lvl_max, lvl_min;
  int threshold_type = THRESHOLD_NOCHANGE;
  int threshold_level = 0;
  int action_info, action_idx;
  struct device_info *dev_info;
  struct tm_instance_info *sensor;
  struct setting_info *info;
  struct tm_setting *tm_info;
  union device_request req;

  /* Get temp and handle */
  for (idx = 0; idx < tm_cnt; idx++)
  {
    if ((thresh_reached_mask & (0x1 << idx)) == 0)
    {
      continue;
    }

    sensor = &tm_states[idx];
    info = sensor->setting;
    tm_info = &(info->data.tm);

    if ((tm_info->num_thresholds < 1) ||
        (sensor->disable))
    {
      /* Unmask TM instance as handled */
      qurt_mutex_lock(&wait_mutex);
      thresh_reached_mask &= (~(0x1 << idx));
      qurt_mutex_unlock(&wait_mutex);  
      continue;
    }

    max_thr = tm_info->num_thresholds;
    sensor_temp = sensor_get_temperature(sensor);
    therm_log_printf(THERM_LOG_LEVEL_DEBUG, 4,
                     "%s: TM Id %s Sensor %s Temp %d", __func__, info->desc,
                     tm_info->sensor, sensor_temp);

    lvl_max = -1;
    lvl_min = INT_MAX;
    for (i = max_thr - 1; i >= 0; i--)
    {
      /* Scan for new alarm conditions */
      if (sensor_threshold_trigger(sensor_temp, sensor, i))
      {
        if (sensor->lvl_alarm[i] == 0)
        {
          therm_log_printf(THERM_LOG_LEVEL_INFO, 5,
                           "TM Id '%s' Sensor '%s' - alarm raised %d at %d.%d degC",
                           info->desc, tm_info->sensor, i + 1,
                           RCONV(sensor_temp),
                           (sensor_temp % 1000) / 100);
          sensor->lvl_alarm[i] = 1;
          alarm_raised = 1;
        }
        if (i > lvl_max)
          lvl_max = i;
      }
      /* Scan for alarm clearing conditions */
      if (sensor_threshold_clear(sensor_temp, sensor, i))
      {
        if (sensor->lvl_alarm[i] == 1)
        {
          therm_log_printf(THERM_LOG_LEVEL_INFO, 5,
                           "TM Id '%s' Sensor '%s' - alarm cleared %d at %d.%d degC",
                           info->desc, tm_info->sensor, i + 1,
                           RCONV(sensor_temp),
                           (sensor_temp % 1000) / 100);
          sensor->lvl_alarm[i] = 0;
          alarm_cleared = 1;
        }
        if (i < lvl_min)
          lvl_min = i;
      }
    }

    /* Update temperature thresholds */
    if (alarm_raised)
    {
      threshold_type = THRESHOLD_CROSS;
      threshold_level = lvl_max + 1;
    }
    else if (alarm_cleared)
    {
      threshold_type = THRESHOLD_CLEAR;
      threshold_level = lvl_min;
    }
    else
    {
      threshold_type = THRESHOLD_NOCHANGE;
      threshold_level = sensor->last_lvl;
    }
    sensor->last_lvl = threshold_level;

    qurt_mutex_lock(&wait_mutex);
    /* Unmask TM instance as handled */
    thresh_reached_mask &= (~(0x1 << idx));
    qurt_mutex_unlock(&wait_mutex);

    sensor_update_thresholds(sensor, threshold_type,
                             threshold_level, idx);

    if (!alarm_raised && !alarm_cleared)
    {
      continue;
    }

    /* Perform actions on highest level alarm */
    for (i = max_thr - 1; i >= 0; i--)
    {
      if (sensor->lvl_alarm[i] == 0)
        continue;

      for (j = 0; j < tm_info->t[i].num_actions; j++)
      {
        action_idx = tm_info->t[i].actions[j].device_idx;
        dev_info = sensor->dev_info_list[action_idx];
        if (dev_info == NULL)
          continue;

        action_info = tm_info->t[i].actions[j].info;
        therm_log_printf(THERM_LOG_LEVEL_DEBUG, 6,
                         "j=%d i=%d TM Id %s Sensor %s: Action %s value %d",
                         j, i, info->desc, tm_info->sensor, dev_info->name,
                         action_info);
        sensor->action_mask |= (1 << action_idx);
        req.value = action_info;
        switch (dev_info->dev_type)
        {
        case DEVICE_DIRECT_ACTION_TYPE:
          if (alarm_raised &&
              (strncasecmp(dev_info->name,
                           "report",
                           DEVICES_MAX_NAME_LEN) == 0))
            report_msg(sensor->dev_clnt_list[action_idx],
                       tm_info->sensor, sensor_temp, i, 1);
          else
            device_clnt_request(sensor->dev_clnt_list[action_idx],
                                &req);
          break;
        case DEVICE_OP_VALUE_TYPE:
        case DEVICE_GENERIC_TYPE:
          device_clnt_request(sensor->dev_clnt_list[action_idx], &req);
          break;
        case DEVICE_NONE_TYPE:
          break;
        default:
          therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "Unknown action %s",
                           dev_info->name);
        }
      }

      break;
    }

    if (alarm_cleared)
    {
      /* Handle alarm clearing cases */
      if (lvl_min == 0)
      {
        therm_log_printf(THERM_LOG_LEVEL_DEBUG, 0,"Clearing all alarms");
        clear_all_alarms(sensor);
      }
      /* report threshold clearing */
      for (j = 0; j < tm_info->t[lvl_min].num_actions; j++)
      {
        action_idx = tm_info->t[lvl_min].actions[j].device_idx;
        dev_info = sensor->dev_info_list[action_idx];
        if (strncasecmp(dev_info->name, "report",
                        DEVICES_MAX_NAME_LEN) == 0)
        {
          report_msg(sensor->dev_clnt_list[action_idx],
                     tm_info->sensor, sensor_temp,
                     lvl_min, 0);
          break;
        }
      }
    }
    alarm_raised = 0;
    alarm_cleared = 0;
  }
}

static void sensor_monitor(void *data)
{
  /* Set mask to all CPU's Trigger setting all thresholds */
  thresh_reached_mask = (1 << tm_cnt) - 1;

  /* Set initial thresholds */
  handle_thresh_sig();

  /* Cancel vdd_restriction request after init. */
  if (vrestrict_clnt != NULL)
  {
    devices_manager_dereg_clnt(vrestrict_clnt);
  }
  
  /* Cancel mx_min request after init. */
  if (mx_min_req_clnt != NULL)
  {
    devices_manager_dereg_clnt(mx_min_req_clnt);
  }

  /* Cancel cx_min request after init. */
  if (cx_min_req_clnt != NULL)
  {
    devices_manager_dereg_clnt(cx_min_req_clnt);
  }

  /* Cancel cpr_cold request after init. */
  if (cpr_cold_req_clnt != NULL)
  {
    devices_manager_dereg_clnt(cpr_cold_req_clnt);
  }

  while (exit_daemon != 1)
  {
    therm_log_printf(THERM_LOG_LEVEL_DEBUG, 1,"%s: Wait for EV", __func__);
    qurt_mutex_lock(&wait_mutex);
    if (!thresh_reached_mask)
    {
      qurt_cond_wait(&wait_cond, &wait_mutex);
    }
    qurt_mutex_unlock(&wait_mutex);

    therm_log_printf(THERM_LOG_LEVEL_DEBUG, 1,"%s: Thresh EVT", __func__);
    handle_thresh_sig();
  }

  sensors_shutdown();
  Core_Free(device_info_arr);
  return;
}

void thermal_monitor(struct thermal_setting_t *settings)
{
  struct setting_info *cfg_setting;
  qurt_thread_attr_t  attr;

  /* Build Device Info List */
  if (devices_manager_get_list(NULL, &device_info_arr_len))
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 0,
                     "Failed to get device list length");
    return;
  }

  device_info_arr = (struct device_info *)Core_Malloc(sizeof(struct device_info) * device_info_arr_len);
  if (NULL == device_info_arr )
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 0,
                     "Failed to alloc device_info_arr");
    return;
  }


  if (devices_manager_get_list(device_info_arr,
                               &device_info_arr_len))
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 0, "Failed to get device list");
    Core_Free(device_info_arr);
    return;
  }

  cfg_setting = settings->list;

  while (cfg_setting && (tm_cnt < MAX_TM_INSTANCES_SUPPORTED))
  {
    if ((cfg_setting->algo_type != MONITOR_ALGO_TYPE) ||
        (cfg_setting->disable != 0))
    {
      cfg_setting = cfg_setting->next;
      continue;
    }

    therm_log_printf(THERM_LOG_LEVEL_DEBUG, 2, "%s: Import %s", __func__,
                     cfg_setting->desc);
    tm_states[tm_cnt].setting = cfg_setting;
    tm_cnt++;
    cfg_setting = cfg_setting->next;
  }

  if (!sensors_setup())
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 0,
                     "Failed to setup at least one sensor for monitoring");
    return;
  }
  qurt_mutex_init(&wait_mutex);
  qurt_cond_init(&wait_cond);
  therm_pthread_attr_init(&attr, "thermMon");

  /* Vote for vdd_restrict until init is complete. */
  vrestrict_clnt = devices_manager_reg_clnt("vdd_restriction");
  if (vrestrict_clnt != NULL)
  {
    union device_request req;
    req.value = 1;
    device_clnt_request(vrestrict_clnt, &req);
  }
  else
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 2, "%s: Error creating client to %s",
                     __func__, "vdd_restriction");
  }

  /* Vote for mx_min until init is complete. */
  mx_min_req_clnt = devices_manager_reg_clnt("mx_min_request");
  if (mx_min_req_clnt != NULL)
  {
    union device_request req;
    req.value = 1;
    device_clnt_request(mx_min_req_clnt, &req);
  }
  else
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 2, "%s: Error creating client to %s",
                     __func__, "mx_min_request");
  }

  /* Vote for cx_min until init is complete. */
  cx_min_req_clnt = devices_manager_reg_clnt("cx_min_request");
  if (cx_min_req_clnt != NULL)
  {
    union device_request req;
    req.value = 1;
    device_clnt_request(cx_min_req_clnt, &req);
  }
  else
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 2, "%s: Error creating client to %s",
                     __func__, "cx_min_request");
  }

  /* Vote for cpr_cold until init is complete. */
  cpr_cold_req_clnt = devices_manager_reg_clnt("cpr_cold");
  if (cpr_cold_req_clnt != NULL)
  {
    union device_request req;
    req.value = 1;
    device_clnt_request(cpr_cold_req_clnt, &req);
  }
  else
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 2, "%s: Error creating client to %s",
                     __func__, "cpr_cold_request");
  }

  qurt_thread_attr_set_stack_size(&attr, (size_t)THREAD_4K_STACK_SIZE);
  if (qurt_thread_create(&tm_thread, &attr, &sensor_monitor,
                     (void *)NULL) != QURT_EOK)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 0,
                     "Error initializing thermal monitor");

    /* Error fatal */
    CORE_VERIFY(NULL);   
  }

  //pthread_attr_destroy(&attr);
}
