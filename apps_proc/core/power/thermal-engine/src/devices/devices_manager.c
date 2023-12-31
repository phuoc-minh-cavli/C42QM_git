/*===========================================================================

  Copyright (c) 2013 QUALCOMM Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

===========================================================================*/

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
//#include <unistd.h>
#include <stringl/stringl.h>

#include "DALSys.h"
#include "devices_manager_internal.h"
#include "thermal.h"
#include "therm_log.h"

static struct devices_manager_dev *dev_list;
static int dev_cnt;

/* Protect client request collisions  */
//static pthread_mutexattr_t clnt_mtx_attr;
static qurt_mutex_t clnt_mtx;

static struct devices_manager_dev* find_dev(const char *dev_name)
{
  struct devices_manager_dev *curr = dev_list;

  /* Find dev info */
  while (curr != NULL)
  {
    if (strncmp(dev_name, curr->dev_info.name,
                DEVICES_MAX_NAME_LEN) == 0)
      break;
    curr = curr->next_dev;
  }
  return curr;
}

static int update_dev_state(struct devices_manager_dev *dev_mgr)
{
  union device_request req;
  struct device_clnt *client = NULL;

  if ((dev_mgr->dev_info.dev_type != DEVICE_GENERIC_TYPE) &&
      (dev_mgr->dev_info.dev_type != DEVICE_OP_VALUE_TYPE))
    return -(EFAULT);

  qurt_mutex_lock(&clnt_mtx);

  client = dev_mgr->client_list;

  if (dev_mgr->dev_info.dev_type == DEVICE_GENERIC_TYPE)
  {
    /* Start from zero to find the highest existing client request */
    req.value = 0;

    /* Walk client list to find highest mitigation level */
    while (client != NULL)
    {
      if (client->request_active)
        req.value = MAX(req.value, client->request.value);
      client = client->next_clnt;
    }
  }
  else if (dev_mgr->dev_info.dev_type == DEVICE_OP_VALUE_TYPE)
  {
    /* Start from max allowable value find lowest request */
    req.value = dev_mgr->dev_info.max_dev_op_value;

    /* Walk client list to find highest mitigation level */
    while (client != NULL)
    {
      if (client->request_active)
        req.value = MIN(req.value, client->request.value);
      client = client->next_clnt;
    }
  }

  if (dev_mgr->active_req.value != req.value)
  {
    dev_mgr->active_req.value = req.value;

    if (dev_mgr->action)
      dev_mgr->action(dev_mgr);

    /* Notify clients */
    client = dev_mgr->client_list;
    while (client != NULL)
    {
      if (client->cb_func != NULL)
        client->cb_func(client, &req,
                        client->cb_usr_data);
      client = client->next_clnt;
    }
  }
  qurt_mutex_unlock(&clnt_mtx);
  return 0;
}

static int validate_clnt(struct device_clnt *clnt)
{
  struct devices_manager_dev *dev_mgr = dev_list;
  struct device_clnt   *dev_clnt = NULL;
  uint8_t dev_valid = 0;
  uint8_t clnt_valid = 0;

  if ((clnt == NULL) || (clnt->dev_mgr == NULL))
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: Invalid args.", __func__);
    return -(EINVAL);
  }

  /* Validate the device pointer */
  while (dev_mgr != NULL)
  {
    if (dev_mgr == clnt->dev_mgr)
    {
      dev_valid = 1;
      break;
    }
    dev_mgr = dev_mgr->next_dev;
  }

  if (dev_valid == 0)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: Invalid dev.", __func__);
    return -(EFAULT);
  }

  qurt_mutex_lock(&clnt_mtx);
  /* Validate the client pointer exists for device */
  dev_clnt = dev_mgr->client_list;
  while (dev_clnt != NULL)
  {
    if (dev_clnt == clnt)
    {
      clnt_valid = 1;
      break;
    }
    dev_clnt = dev_clnt->next_clnt;
  }
  qurt_mutex_unlock(&clnt_mtx);

  if (clnt_valid == 0)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: Invalid clnt.", __func__);
    return -(EFAULT);
  }
  return 0;
}

int devices_manager_get_info(const char *dev_name, struct device_info *info)
{
  struct devices_manager_dev *dev_mgr = NULL;

  if ((dev_name == NULL) || (info == NULL))
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: Invalid args.", __func__);
    return -(EINVAL);
  }

  /* Find dev info */
  dev_mgr = find_dev(dev_name);

  if (dev_mgr == NULL)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: Invalid dev.", __func__);
    return -(EINVAL);
  }

  /* Actually copying device info */
  *info = dev_mgr->dev_info;

  return 0;
}

int devices_manager_get_list(struct device_info *info_arr, int *info_arr_len)
{
  int dev_idx;
  struct devices_manager_dev *curr = dev_list;

  if ((info_arr == NULL) && (info_arr_len == NULL))
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: Invalid args.", __func__);
    return -(EINVAL);
  }

  if (info_arr == NULL)
  {
    /* Interpret as request for number of dev's present. */
    *info_arr_len = dev_cnt;
    return 0;
  }

  /* Don't exceed end of info_array */
  *info_arr_len = MIN(*info_arr_len, dev_cnt);

  for (dev_idx = 0; (dev_idx < *info_arr_len) && (curr != NULL); dev_idx++)
  {
    info_arr[dev_idx] = curr->dev_info;
    curr = curr->next_dev;
  }

  return 0;
}

int devices_manager_get_lvl_list(const char *dev_name,
                                 struct device_lvl_info *info_arr,
                                 int *info_arr_len)
{
  struct devices_manager_dev *dev_mgr = NULL;

  if ((dev_name == NULL) || (info_arr_len == NULL))
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: Invalid args.", __func__);
    return -(EINVAL);
  }

  /* Find dev info */
  dev_mgr = find_dev(dev_name);

  if (dev_mgr == NULL)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: Invalid dev.", __func__);
    return -(EINVAL);
  }

  if (info_arr == NULL)
  {
    /* Interpret as request for number of dev levels. */
    *info_arr_len = dev_mgr->dev_info.num_of_levels;
    return 0;
  }

  if (dev_mgr->lvl_info == NULL)
  {
    /* No real dev lvl_info for this device.
       ie. DEVICE_GENERIC_TYPE */
    return -(EFAULT);
  }

  /* Safe memcpy - (dst, dst_size, src, src_size) */
  memscpy( info_arr, 
           sizeof(struct device_lvl_info) * (*info_arr_len),
           dev_mgr->lvl_info, 
           sizeof(struct device_lvl_info) * dev_mgr->dev_info.num_of_levels );

  return 0;
}

device_clnt_handle devices_manager_reg_clnt(const char *dev_name)
{
  struct device_clnt *client = NULL;
  struct devices_manager_dev *dev_mgr = NULL;

  if (dev_name == NULL)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: Invalid args.", __func__);
    return client;
  }

  dev_mgr = find_dev(dev_name);

  if (dev_mgr == NULL)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 2, "%s: Invalid dev %s.", __func__,
                     dev_name);
    return client;
  }

  client = Core_Malloc(sizeof(struct device_clnt));
  if (NULL == client)
  {
    return client;
  }
  memset(client, 0x0, sizeof(struct device_clnt));

  qurt_mutex_lock(&clnt_mtx);
  /* Insert the client */
  client->dev_mgr = dev_mgr;
  client->next_clnt = dev_mgr->client_list;
  dev_mgr->client_list = client;
  qurt_mutex_unlock(&clnt_mtx);
  return client;
}

int devices_manager_dereg_clnt(device_clnt_handle clnt)
{
  struct device_clnt   *prev = NULL;
  struct device_clnt   *curr = NULL;
  struct devices_manager_dev *dev_mgr = NULL;
  struct device_clnt   *client = clnt;
  int ret_val = 0;

  if (client == NULL)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: Invalid args.", __func__);
    return -(EINVAL);
  }

  ret_val = validate_clnt(client);
  if (ret_val != 0)
    return ret_val;

  dev_mgr = client->dev_mgr;

  /* Search for client in dev client list */
  qurt_mutex_lock(&clnt_mtx);
  curr = dev_mgr->client_list;
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
      dev_mgr->client_list = curr->next_clnt;
      break;
    }
    prev = curr;
    curr = curr->next_clnt;
  }
  qurt_mutex_unlock(&clnt_mtx);

  Core_Free(client);

  /* Update state device state without client request.  */
  update_dev_state(dev_mgr);

  return 0;
}

int devices_manager_get_state_info(device_clnt_handle clnt,
                                   struct device_state_info *info)
{
  struct devices_manager_dev *dev_mgr = NULL;
  struct device_clnt   *client = clnt;
  int ret_val = 0;

  if ((client == NULL) || (info == NULL))
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: Invalid args.", __func__);
    return -(EINVAL);
  }

  ret_val = validate_clnt(client);
  if (ret_val != 0)
    return ret_val;

  dev_mgr = client->dev_mgr;

  qurt_mutex_lock(&clnt_mtx);
  info->request_active = client->request_active;
  info->requested.lvl = client->request;
  info->current.lvl = dev_mgr->active_req;
  qurt_mutex_unlock(&clnt_mtx);

  return 0;
}

static int devices_manager_set_lvl(struct devices_manager_dev *dev_mgr,
                                   struct device_clnt *client, int lvl)
{
  if (lvl < 0)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: Invalid args.", __func__);
    return -(EINVAL);
  }

  lvl = MIN(lvl, (dev_mgr->dev_info.num_of_levels - 1));

  qurt_mutex_lock(&clnt_mtx);
  client->request.value = lvl;
  client->request_active = 1;
  qurt_mutex_unlock(&clnt_mtx);
  therm_log_printf(THERM_LOG_LEVEL_DEBUG, 3, "%s: DEV %s, lvl %d", __func__,
                   dev_mgr->dev_info.name, lvl);
  update_dev_state(dev_mgr);

  return dev_mgr->active_req.value;
}

static int devices_manager_set_op_value(struct devices_manager_dev *dev_mgr,
                                        struct device_clnt *client,
                                        int dev_op_value)
{
  int lvl_idx = 0;

  if (dev_op_value < 0)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: Invalid args.",
                     __func__);
    return DAL_ERROR_INVALID_PARAMETER;
  }

  dev_mgr = client->dev_mgr;

  if (dev_mgr->dev_info.max_dev_op_value_valid == 0)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: dev_op invalid.",
                     __func__);
    return -(EFAULT);
  }

  dev_op_value = MIN(dev_op_value, dev_mgr->dev_info.max_dev_op_value);

  if (dev_mgr->lvl_info && (dev_mgr->dev_info.num_of_levels > 0))
  {
    /* Translate to dev_op_value to supported mitigation value */
    for (lvl_idx = 0; lvl_idx < dev_mgr->dev_info.num_of_levels; lvl_idx++)
    {
      if (dev_mgr->lvl_info[lvl_idx].lvl.value <= dev_op_value)
        break;
    }
    if (lvl_idx >=  dev_mgr->dev_info.num_of_levels)
    {
      /* Apply highest lvl of mitigation possible */
      lvl_idx = dev_mgr->dev_info.num_of_levels - 1;
    }
    dev_op_value = dev_mgr->lvl_info[lvl_idx].lvl.value;
  }

  qurt_mutex_lock(&clnt_mtx);
  client->request_active = 1;
  client->request.value = dev_op_value;
  qurt_mutex_unlock(&clnt_mtx);
  therm_log_printf(THERM_LOG_LEVEL_DEBUG, 3, "%s: DEV %s, op_value %d",
                   __func__, dev_mgr->dev_info.name, dev_op_value);

  update_dev_state(dev_mgr);

  return dev_mgr->active_req.value;;
}

int devices_manager_set_direct_action(struct devices_manager_dev *dev_mgr,
                                      struct device_clnt *client,
                                      union device_request *req)
{
  dev_mgr = client->dev_mgr;

  if (dev_mgr->dev_info.dev_type != DEVICE_DIRECT_ACTION_TYPE)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 2, "%s: Unsupported for %s.",
                     __func__, dev_mgr->dev_info.name);
    return -(EFAULT);
  }

  if (dev_mgr->action != NULL)
  {
    qurt_mutex_lock(&clnt_mtx);
    dev_mgr->active_req = *req;
    dev_mgr->action(dev_mgr);
    qurt_mutex_unlock(&clnt_mtx);
  }
  therm_log_printf(THERM_LOG_LEVEL_DEBUG, 2, "%s: DEV %s", __func__,
                   dev_mgr->dev_info.name);

  return 0;
}

/* Device request */
int device_clnt_request(device_clnt_handle clnt, union device_request *req)
{
  struct devices_manager_dev *dev_mgr = NULL;
  struct device_clnt   *client = clnt;
  int ret_val = 0;

  if ((client == NULL) || (req == NULL))
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: Invalid args.",
                     __func__);
    return DAL_ERROR_INVALID_PARAMETER;
  }

  ret_val = validate_clnt(client);
  if (ret_val != 0)
    return ret_val;

  dev_mgr = client->dev_mgr;

  switch (dev_mgr->dev_info.dev_type)
  {
  case DEVICE_GENERIC_TYPE:
    ret_val = devices_manager_set_lvl(dev_mgr, client, req->value);
    break;
  case DEVICE_OP_VALUE_TYPE:
    ret_val = devices_manager_set_op_value(dev_mgr, client,
                                           req->value);
    break;
  case DEVICE_DIRECT_ACTION_TYPE:
    ret_val = devices_manager_set_direct_action(dev_mgr, client,
                                                req);
    break;
  default:
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 2, "%s: Unhandled dev_type %d",
                     __func__, dev_mgr->dev_info.dev_type);
    break;
  }
  return ret_val;
}

/* Device cancel request */
void device_clnt_cancel_request(device_clnt_handle clnt)
{
  struct devices_manager_dev *dev_mgr = NULL;
  struct device_clnt   *client = clnt;
  int ret_val = 0;

  if (client == NULL)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: Invalid args.",
                     __func__);
    return;
  }

  ret_val = validate_clnt(client);
  if (ret_val != 0)
    return;

  dev_mgr = client->dev_mgr;

  client->request_active = 0;
  therm_log_printf(THERM_LOG_LEVEL_DEBUG, 2, "%s: DEV %s", __func__,
                   dev_mgr->dev_info.name);
  if ((dev_mgr->dev_info.dev_type == DEVICE_GENERIC_TYPE) ||
      (dev_mgr->dev_info.dev_type == DEVICE_OP_VALUE_TYPE))
    update_dev_state(dev_mgr);
}

int devices_manager_reg_notify(device_clnt_handle clnt,
                               device_notify_cb_func cb_func,
                               void *notify_cb_data)
{
  int ret_val = 0;
  struct device_clnt *client = clnt;

  if (client == NULL)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: Invalid args.",
                     __func__);
    return -(EINVAL);
  }

  ret_val = validate_clnt(client);
  if (ret_val != 0)
    return ret_val;

  qurt_mutex_lock(&clnt_mtx);
  client->cb_func = cb_func;
  client->cb_usr_data = notify_cb_data;
  qurt_mutex_unlock(&clnt_mtx);
  return 0;
}

int devices_manager_add_dev(struct devices_manager_dev *dev_mgr)
{
  /* Make multi-thread safe for dynamic adding of devices. */
  /* Just add new device to begining of list */
  dev_mgr->next_dev = dev_list;
  dev_list = dev_mgr;
  dev_cnt++;
  return 0;
}

int devices_manager_rm_dev(struct devices_manager_dev *dev_mgr)
{
  struct devices_manager_dev *prev = NULL;
  struct devices_manager_dev *curr = dev_list;
  int ret_val = 0;

  if (dev_mgr == NULL)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: Invalid args.",
                     __func__);
    return -(EINVAL);
  }
  /* Find the previous dev in list */
  while (curr != NULL)
  {
    if ((curr == dev_mgr) && (prev != NULL))
    {
      /* In middle of list */
      prev->next_dev = curr->next_dev;
      break;
    }
    else if ((curr == dev_mgr))
    {
      /* At begining of list */
      dev_list = curr->next_dev;
      break;
    }
    prev = curr;
    curr = curr->next_dev;
  }

  /* Note it's not safe to destroy clients, but clients become invalid
     after the dev_info is removed from dev_list. */

  if (dev_mgr->release)
    dev_mgr->release(dev_mgr);
  dev_cnt--;

  return ret_val;
}

int devices_manager_init(void)
{
  //pthread_mutexattr_init(&clnt_mtx_attr);
  //pthread_mutexattr_settype(&clnt_mtx_attr, PTHREAD_MUTEX_RECURSIVE);
  qurt_mutex_init(&clnt_mtx);
  therm_log_printf(THERM_LOG_LEVEL_INFO, 1, "%s: Init", __func__);
  return 0;
}

void devices_manager_release(void)
{
  therm_log_printf(THERM_LOG_LEVEL_INFO, 1, "%s: Release", __func__);
}
