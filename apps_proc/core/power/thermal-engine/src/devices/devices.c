/*===========================================================================

  Copyright (c) 2013, 2017 QUALCOMM Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

===========================================================================*/

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
//#include <unistd.h>

#include "DALSys.h"
#include "devices_manager_internal.h"
#include "devices_actions.h"
#include "devices.h"
#include "devices-qmi.h"
#include "therm_log.h"
#include "thermal.h"

struct tmd_generic_dev_info_t
{
  char *name;
  uint32_t num_of_lvls;
  device_action action;
  void *data;
  enum device_type dev_type;
};

static int modem_action(struct devices_manager_dev *dev_mgr);
static int modem_cx_action(struct devices_manager_dev *dev_mgr);
static int vdd_restriction_action(struct devices_manager_dev *dev_mgr);
static int mx_min_request_action(struct devices_manager_dev *dev_mgr);
#ifdef CPR_COLD_REQUEST
static int cpr_cold_action(struct devices_manager_dev *dev_mgr);
#endif
static int cpu_action(struct devices_manager_dev *dev_mgr);
static int cx_min_request_action(struct devices_manager_dev *dev_mgr);
static int modem_proc_action(struct devices_manager_dev *dev_mgr);
static int modem_proc_current_action(struct devices_manager_dev *dev_mgr);
static void generic_dev_release(struct devices_manager_dev *dev_mgr);
static int generic_dev_add(struct tmd_generic_dev_info_t *gen_info);

static struct tmd_generic_dev_info_t gen_dev_list[] = {
  {
    .name = "none",
    .dev_type = DEVICE_NONE_TYPE,
  },
  {
    .name = "modem",
    .num_of_lvls = 4,
    .action = modem_action,
  },
  {
    .name = "modem_cx",
    .num_of_lvls = 4,
    .action = modem_cx_action,
  },
  {
    .name = "vdd_restriction",
    .num_of_lvls = 2,
    .action = vdd_restriction_action,
  },
  {
    .name = "mx_min_request",
    .num_of_lvls = 2,
    .action = mx_min_request_action,
  },
#ifdef CPR_COLD_REQUEST
  {
    .name = "cpr_cold",
    .num_of_lvls = 4,
    .action = cpr_cold_action,
  },
#endif
  {
    .name = "cx_min_request",
    .num_of_lvls = 2,
    .action = cx_min_request_action,
  },
  {
    .name = "modem_proc",
    .num_of_lvls = 4,
    .action = modem_proc_action,
  },
  {
    .name = "modem_proc_current",
    .num_of_lvls = 4,
    .action = modem_proc_current_action,
  },
  {
    .name = "cpu",
    .num_of_lvls = 0x7FFFFFFF,
    .action = cpu_action,
    .dev_type = DEVICE_OP_VALUE_TYPE,
  },
};

static int modem_action(struct devices_manager_dev *dev_mgr)
{
  return modem_request(dev_mgr->active_req.value);
}
static int modem_cx_action(struct devices_manager_dev *dev_mgr)
{
  return modem_cx_request(dev_mgr->active_req.value);
}

static int vdd_restriction_action(struct devices_manager_dev *dev_mgr)
{
  return vdd_restriction_request(dev_mgr->active_req.value);
}

static int mx_min_request_action(struct devices_manager_dev *dev_mgr)
{
  return mx_min_request(dev_mgr->active_req.value);
}

#ifdef CPR_COLD_REQUEST
static int cpr_cold_action(struct devices_manager_dev *dev_mgr)
{
  return cpr_cold_request(dev_mgr->active_req.value);
}
#endif

static int cx_min_request_action(struct devices_manager_dev *dev_mgr)
{
  return cx_min_request(dev_mgr->active_req.value);
}

static int modem_proc_action(struct devices_manager_dev *dev_mgr)
{
  return modem_proc_request(dev_mgr->active_req.value);
}

static int modem_proc_current_action(struct devices_manager_dev *dev_mgr)
{
  return modem_proc_current_request(dev_mgr->active_req.value);
}
static int cpu_action(struct devices_manager_dev *dev_mgr)
{
  return cpu_request(dev_mgr->active_req.value);
}
static void generic_dev_release(struct devices_manager_dev *dev_mgr)
{
  Core_Free(dev_mgr);
}

static int generic_dev_add(struct tmd_generic_dev_info_t *gen_info)
{
  int ret_val = 0;
  struct devices_manager_dev *dev = NULL;

  dev = Core_Malloc(sizeof(struct devices_manager_dev));
  
  if (dev == NULL)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: malloc failed.",
                     __func__);
    ret_val = DAL_ERROR_INSUFFICIENT_MEMORY;
    goto handle_error;
  }
  memset(dev, 0, sizeof(struct devices_manager_dev));

  dev->dev_info.num_of_levels = gen_info->num_of_lvls;

  strlcpy(dev->dev_info.name, gen_info->name, DEVICES_MAX_NAME_LEN);
  dev->dev_info.dev_type = gen_info->dev_type;

  if (dev->dev_info.dev_type == DEVICE_OP_VALUE_TYPE)
  {
    dev->dev_info.max_dev_op_value_valid = 1;
    dev->dev_info.max_dev_op_value = gen_info->num_of_lvls;
    dev->active_req.value = dev->dev_info.max_dev_op_value;
  }

  if (gen_info->action)
    dev->action = gen_info->action;

  if (gen_info->data)
    dev->data = gen_info->data;

  dev->release = generic_dev_release;

  if (devices_manager_add_dev(dev) != 0)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: Can not add device",
                     __func__);
    ret_val = -1;
  }

handle_error:
  if (ret_val < 0)
  {
    /* Error clean up */
    if (dev)
      Core_Free(dev);
  }
  return ret_val;
}

static int init_generic_devs(void)
{
  int ret_val = 0;
  int idx;

  for (idx = 0; idx < ARRAY_SIZE(gen_dev_list); idx++)
  {
    ret_val = generic_dev_add(&gen_dev_list[idx]);
    if (ret_val)
    {
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 2,
                       "%s: failed to add %s device.", __func__,
                       gen_dev_list[idx].name);
      break;
    }
  }
  return ret_val;
}

/* TMD init */
int devices_init(int minimum_mode)
{
  int ret_val = 0;

  qmi_communication_init();

  ret_val = init_generic_devs();
  if (ret_val)
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 2,
                     "%s: Init generic TMDs failed %d", __func__, ret_val);

  /* Thermal device specific initialization */
  vdd_rstr_init();
  mx_min_request_init();
  cx_min_request_init();
  //cpr_cold_request_init();
  cpu_request_init();

  return ret_val;
}

void devices_release(void)
{
  qmi_communication_release();
}
