/*===========================================================================

  Copyright (c) 2013 QUALCOMM Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

===========================================================================*/

#ifndef __THERMAL_CONFIG_H__
#define __THERMAL_CONFIG_H__

#include <math.h>
#include <stdio.h>
#include "devices_manager.h"
#include "sensors_manager.h"

#define SAMPLING_MS_DEFAULT  (5000)
#define SAMPLING_MS_MINIMUM  (250)
#define THRESHOLDS_MAX       (8)
#define ACTIONS_MAX          (8)
#define MAX_ALGO_DESC        (32)

enum algo_type {
	UKNOWN_ALGO_TYPE = -1,
	MONITOR_ALGO_TYPE = 0,
	PID_ALGO_TYPE,
	SS_ALGO_TYPE,
	ALGO_IDX_MAX,
};

struct action_t {
	char device[DEVICES_MAX_NAME_LEN];
	int  info;
	int  device_idx;
};

struct threshold_t {
	int lvl_trig;
	int lvl_clr;
	struct action_t actions[ACTIONS_MAX];
	int num_actions;
};

struct pid_setting {
	char sensor[MAX_SENSOR_NAME_LEN];
	char device[DEVICES_MAX_NAME_LEN];
	int  sampling_period_ms;
	int  i_samples;
	int  set_point;
	int  set_point_clr;
	int  units_per_C;
	float p_const;
	float i_const;
	float d_const;
	float err_weight;
};

struct tm_setting {
	char    sensor[MAX_SENSOR_NAME_LEN];
	int     algo_type;
	int     sampling_period_ms;
	int     num_thresholds;
	uint8_t descending_thresh;
	struct  threshold_t t[THRESHOLDS_MAX];

	/* internal counters used during config parsing */
	int _n_thresholds;
	int _n_to_clear;
	int _n_actions;
	int _n_action_info;
};

struct ss_setting {
	char sensor[MAX_SENSOR_NAME_LEN];
	char device[DEVICES_MAX_NAME_LEN];
	int  sampling_period_ms;
	int  set_point;
	int  set_point_clr;
	int  time_constant;
};

union algo_data {
	struct pid_setting pid;
	struct tm_setting tm;
	struct ss_setting ss;
};

struct setting_info {
	char desc[MAX_ALGO_DESC];
	int algo_type;
	uint8_t disable;
	uint8_t dyanmic;  /* dynamic != 0 if allocated using malloc. */
	struct setting_info *next;
	union algo_data data;
};

struct thermal_setting_t {
	int sample_period_ms;
	struct setting_info *list;
};

void init_settings(struct thermal_setting_t *settings);
int  load_config(struct thermal_setting_t *settings, const char *pFName);
void add_setting(struct thermal_setting_t *settings, struct setting_info *info);

#endif  /* __THERMAL_CONFIG_H__ */
