/*===========================================================================

Copyright (c) 2013-18 QUALCOMM Technologies Incorporated.  All Rights Reserved.
QUALCOMM Proprietary and Confidential.

===========================================================================*/

#include "thermal.h"
#include "therm_log.h"
#include "thermal_config.h"
#include "CoreVerify.h"

#define VDD_MIN_DISABLE_THRESH 5000
#define VDD_MIN_DISABLE_THRESH_CLR 10000

#define VDD_RESTRICT_DEVICE "vdd_restriction"
#define MX_MIN_REQUEST_DEVICE "mx_min_request"
#define CX_MIN_REQUEST_DEVICE "cx_min_request"
#define CPU_DEVICE "cpu"

/* Thermal monitoring algorithm for 9205.  */
static struct setting_info tm_cfgs_9205[] =
{
  {
    .desc = "VDD_RSTR_MONITOR-TSENS0",
    .algo_type = MONITOR_ALGO_TYPE,
    .data.tm = {
      .sensor = "tsens_tz_sensor0",
      .sampling_period_ms = 1000,
      .num_thresholds = 1,
      .descending_thresh = 1,
      ._n_thresholds = 1,
      ._n_to_clear = 1,
      ._n_actions = 1,
      ._n_action_info = 1,
      .t[0] = {
        .lvl_trig = VDD_MIN_DISABLE_THRESH,
        .lvl_clr = VDD_MIN_DISABLE_THRESH_CLR,
        .num_actions = 1,
        .actions[0] = {
          .device = VDD_RESTRICT_DEVICE,
          .info = 1,
        },
      },
    },
  },
  {
    .desc = "VDD_RSTR_MONITOR-TSENS2",
    .algo_type = MONITOR_ALGO_TYPE,
    .data.tm = {
      .sensor = "tsens_tz_sensor2",
      .sampling_period_ms = 1000,
      .num_thresholds = 1,
      .descending_thresh = 1,
      ._n_thresholds = 1,
      ._n_to_clear = 1,
      ._n_actions = 1,
      ._n_action_info = 1,
      .t[0] = {
        .lvl_trig = VDD_MIN_DISABLE_THRESH,
        .lvl_clr = VDD_MIN_DISABLE_THRESH_CLR,
        .num_actions = 1,
        .actions[0] = {
          .device = VDD_RESTRICT_DEVICE,
          .info = 1,
        },
      },
    },
  },
  {
    .desc = "CPU-MONITOR0",
    .algo_type = MONITOR_ALGO_TYPE,
    .data.tm = {
      .sensor = "tsens_tz_sensor4",
      .sampling_period_ms = 100,
      .num_thresholds = 1,
      ._n_thresholds = 1,
      ._n_to_clear = 1,
      ._n_actions = 1,
      ._n_action_info = 1,
      .t[0] = {
        .lvl_trig = 95000,
        .lvl_clr = 93000,
        .num_actions = 1,
        .actions[0] = {
          .device = CPU_DEVICE,
          .info = 1190400,
        },
      },
    },
  },
  {
    .desc = "CPU-MONITOR1",
    .algo_type = MONITOR_ALGO_TYPE,
    .data.tm = {
      .sensor = "tsens_tz_sensor4",
      .sampling_period_ms = 100,
      .num_thresholds = 1,
      ._n_thresholds = 1,
      ._n_to_clear = 1,
      ._n_actions = 1,
      ._n_action_info = 1,
      .t[0] = {
        .lvl_trig = 97000,
        .lvl_clr = 94000,
        .num_actions = 1,
        .actions[0] = {
          .device = CPU_DEVICE,
          .info = 1094400,
        },
      },
    },
  },
  {
    .desc = "CPU-MONITOR2",
    .algo_type = MONITOR_ALGO_TYPE,
    .data.tm = {
      .sensor = "tsens_tz_sensor4",
      .sampling_period_ms = 100,
      .num_thresholds = 1,
      ._n_thresholds = 1,
      ._n_to_clear = 1,
      ._n_actions = 1,
      ._n_action_info = 1,
      .t[0] = {
        .lvl_trig = 99000,
        .lvl_clr = 96000,
        .num_actions = 1,
        .actions[0] = {
          .device = CPU_DEVICE,
          .info = 800000,
        },
      },
    },
  },
};
void thermal_monitor_init_data(struct thermal_setting_t *setting)
{
  int i, arr_size = 0;

  struct setting_info *cfg = NULL;

  switch (therm_get_msm_id()) {

    case THERM_MDM_9205:
      cfg = tm_cfgs_9205;
      arr_size = ARRAY_SIZE(tm_cfgs_9205);
      break;

    default:
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: ERROR Uknown device",
                       __func__);

      /* Asserting as ther is no correct configuration for this target */
      CORE_VERIFY(0);
      break;
  }

  for (i = 0; i < arr_size; i++) {
    add_setting(setting, &cfg[i]);
  }
}
