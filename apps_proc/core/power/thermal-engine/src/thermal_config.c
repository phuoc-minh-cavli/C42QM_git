/*===========================================================================

  Copyright (c) 2013 QUALCOMM Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

===========================================================================*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
//#include <unistd.h>
#include "thermal.h"
#include "therm_log.h"
#include "thermal_config.h"
#include "DALSys.h"
#include "fs_lib.h"
#include "fs_public.h"

#define CONFIG_FILE_SIZE_MAX  2048

#define CONFIG_FILE_DEFAULT  "/nv/item_files/thermal-engine/thermal-engine.conf"

enum field_type
{
  DEBUG,
  SAMPLING,
  THRESHOLDS,
  THRESHOLDS_CLR,
  ACTIONS,
  ACTION_INFO,
  ALGO_TYPE,
  P_CONST,
  I_CONST,
  D_CONST,
  ERR_WEIGHT,
  I_SAMPLES,
  UNITS_PER_CALC,
  DISABLE,
  SET_POINT,
  SET_POINT_CLR,
  SENSOR_FIELD,
  DEVICE_FIELD,
  DESCENDING,
  TIME_CONSTANT,
  /* Add fields above this. */
  FIELD_IDX_MAX,
};

struct fields
{
  char *name;
  enum field_type type;
};

struct fields fields[] = {
  { .name = "debug", .type = DEBUG },
  { .name = "sampling", .type = SAMPLING },
};

struct fields pid_fields[] = {
  { .name = "sampling", .type = SAMPLING },
  { .name = "p_const", .type = P_CONST },
  { .name = "i_const", .type = I_CONST },
  { .name = "d_const", .type = D_CONST },
  { .name = "err_weight", .type = ERR_WEIGHT },
  { .name = "i_samples", .type = I_SAMPLES },
  { .name = "dev_units_per_calc", .type = UNITS_PER_CALC },
  { .name = "disable", .type = DISABLE },
  { .name = "sensor", .type = SENSOR_FIELD },
  { .name = "device", .type = DEVICE_FIELD },
  { .name = "set_point", .type = SET_POINT },
  { .name = "set_point_clr", .type = SET_POINT_CLR },
};

struct fields ss_fields[] = {
  { .name = "sampling", .type = SAMPLING },
  { .name = "disable", .type = DISABLE },
  { .name = "sensor", .type = SENSOR_FIELD },
  { .name = "device", .type = DEVICE_FIELD },
  { .name = "set_point", .type = SET_POINT },
  { .name = "set_point_clr", .type = SET_POINT_CLR },
  { .name = "time_constant", .type = TIME_CONSTANT },
};

struct fields tm_fields[] = {
  { .name = "sampling", .type = SAMPLING },
  { .name = "thresholds", .type = THRESHOLDS },
  { .name = "thresholds_clr", .type = THRESHOLDS_CLR },
  { .name = "actions", .type = ACTIONS },
  { .name = "action_info", .type = ACTION_INFO },
  { .name = "disable", .type = DISABLE },
  { .name = "sensor", .type = SENSOR_FIELD },
  { .name = "descending", .type = DESCENDING },
};

static char buf[CONFIG_FILE_SIZE_MAX];
static int buf_bytes_read;

static char *algo_names[] = {
  "monitor",
  "pid",
  "ss",
};

static struct device_info *device_info_arr;
static int                 device_info_arr_len;
static struct sensor_info_type *sensor_info_arr;
static int                      sensor_info_arr_len;

static char* algo_idx_to_str(int alg)
{
  if ((alg < 0) || (alg >= ALGO_IDX_MAX))
  {
    return NULL;
  }
  return algo_names[alg];
}

void add_setting(struct thermal_setting_t *settings, struct setting_info *info)
{
  if ((settings == NULL) || (info == NULL))
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: Invalid args.", __func__);
    return;
  }

  /* Insert into the config list head */
  info->next = settings->list;
  settings->list = info;
}

void init_settings(struct thermal_setting_t *settings)
{
  if (!settings)
  {
    return;
  }

  memset(settings, 0, sizeof(struct thermal_setting_t));
  settings->sample_period_ms = -1;
}

static void init_tm_settings(struct setting_info *settings)
{
  if (!settings)
    return;

  memset(settings, 0, sizeof(struct setting_info));
  settings->algo_type = UKNOWN_ALGO_TYPE;
}

void skip_space(char **ppos)
{
  char *pos = *ppos;

  while (*pos != '\0')
  {
    if ((*pos == ' ') || (*pos == '\t') || (*pos == '\n') ||
        (*pos == '\r'))
    {
      pos++;
    }
    else
      break;
  }
  *ppos = pos;
}

void skip_line(char **ppos)
{
  char *pos = *ppos;
  char *ptmp;

  ptmp = strchr(pos, '\n');
  if (!ptmp)
  {
    *pos = '\0';
    return;
  }

  *ppos = ptmp + 1;
}

static int parse_algo(char **ppos)
{
  char *pos = *ppos;
  char *ptmp;
  char *pvalue, *psave = NULL;
  int  ret_val = ALGO_IDX_MAX;


  ptmp = strchr(pos, '\n');
  if (ptmp)
    *ptmp = '\0';

  pvalue = strtok_r(pos, "\t \r\n", &psave);

  if (pvalue != NULL)
  {
    int i;
    for (i = 0; i < ALGO_IDX_MAX; i++)
    {
      if (strcasecmp(algo_idx_to_str(i), pvalue) == 0)
      {
        ret_val = i;
        therm_log_printf(THERM_LOG_LEVEL_DEBUG, 1, "Algo Type '%s'", pvalue);
        break;
      }
    }

    if (ret_val == ALGO_IDX_MAX)
    {
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "Unknown algo '%s'", pvalue);
    }
  }

  *ppos = ptmp + 1;
  return ret_val;
}

static int parse_float(char **ppos, float *result)
{
  char *pos = *ppos;
  char *ptmp;
  char *pvalue, *psave = NULL;
  float  ret_val = -(EFAULT);


  ptmp = strchr(pos, '\n');
  if (ptmp)
    *ptmp = '\0';

  pvalue = strtok_r(pos, "\t \r\n", &psave);

  if (pvalue != NULL)
  {
    *result = strtof(pvalue, NULL);
    ret_val = 0;
  }

  *ppos = ptmp + 1;
  return ret_val;
}

void parse_action(char *pvalue, struct action_t *result)
{
  int i;

  for (i = 0; i < device_info_arr_len; i++)
  {
    if (strcasecmp(device_info_arr[i].name, pvalue) == 0)
    {
      strlcpy(result->device, device_info_arr[i].name,
              DEVICES_MAX_NAME_LEN);
      return;
    }
  }
  therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "Unknown action '%s'", pvalue);
}



int parse_action_values(char **ppos,
                        struct action_t result[THRESHOLDS_MAX][ACTIONS_MAX],
                        int *num_actions)
{
  char *pos = *ppos;
  char *ptmp;
  char *pvalue, *psave = NULL;
  int  nresult = 0;

  ptmp = strchr(pos, '\n');
  if (ptmp)
    *ptmp = '\0';

  pvalue = strtok_r(pos, "\t \r\n", &psave);

  while (pvalue != NULL)
  {
    char *pstr = NULL;
    char *psave2 = NULL;
    int count = 0;

    pstr = strtok_r(pvalue, "+", &psave2);
    while (pstr != NULL)
    {
      therm_log_printf(THERM_LOG_LEVEL_DEBUG, 1, "Found action '%s'", pstr);
      parse_action(pstr, &result[nresult][count]);
      count++;
      if (count >= ACTIONS_MAX)
        break;

      pstr = strtok_r(NULL, "+", &psave2);
    }

    num_actions[nresult++] = count;
    if (nresult >= THRESHOLDS_MAX)
      break;
    pvalue = strtok_r(NULL, "\t \r\n", &psave);
  }

  therm_log_printf(THERM_LOG_LEVEL_DEBUG, 1, "No. of items found : %d", nresult);

  *ppos = ptmp + 1;
  return nresult;
}

int parse_sensor_field(char **ppos, char **result)
{
  char *pos = *ppos;
  char *ptmp;
  char *pvalue, *psave = NULL;
  int  ret_val = -(EFAULT);
  int  i;

  ptmp = strchr(pos, '\n');
  if (ptmp)
    *ptmp = '\0';

  pvalue = strtok_r(pos, "\t \r\n", &psave);

  if (pvalue == NULL)
    return ret_val;

  for (i = 0; i < sensor_info_arr_len; i++)
  {
    if (strcasecmp(sensor_info_arr[i].name, pvalue) == 0)
    {
      *result = sensor_info_arr[i].name;
      ret_val = 0;
      break;
    }
  }

  if (ret_val)
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 2, "%s: Unknown sensor %s",
                     __func__, pvalue);

  if (ptmp)
    *ppos = ptmp + 1;
  else
    *ppos = pos + strlen(pvalue);
  return ret_val;
}

int parse_device_field(char **ppos, char **result)
{
  char *pos = *ppos;
  char *ptmp;
  char *pvalue, *psave = NULL;
  int  ret_val = -(EFAULT);
  int  i;

  ptmp = strchr(pos, '\n');
  if (ptmp)
    *ptmp = '\0';

  pvalue = strtok_r(pos, "\t \r\n", &psave);

  if (pvalue == NULL)
    return ret_val;

  for (i = 0; i < device_info_arr_len; i++)
  {
    if (strcasecmp(device_info_arr[i].name, pvalue) == 0)
    {
      *result = device_info_arr[i].name;
      ret_val = 0;
      break;
    }
  }

  if (ret_val)
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 2, "%s: Unknown device %s",
                     __func__, pvalue);

  if (ptmp)
    *ppos = ptmp + 1;
  else
    *ppos = pos + strlen(pvalue);
  return ret_val;
}

static int parse_action_info_values(char **ppos,
                                    struct action_t result[THRESHOLDS_MAX][ACTIONS_MAX],
                                    int *num_actions)
{
  char *pos = *ppos;
  char *ptmp;
  char *pvalue, *psave = NULL;
  int  nresult = 0;

  ptmp = strchr(pos, '\n');
  if (ptmp)
    *ptmp = '\0';

  pvalue = strtok_r(pos, "\t \r\n", &psave);

  while (pvalue != NULL)
  {
    char *pstr = NULL;
    char *psave2 = NULL;
    int count = 0;

    pstr = strtok_r(pvalue, "+", &psave2);
    while (pstr != NULL)
    {
      therm_log_printf(THERM_LOG_LEVEL_DEBUG, 1, "Found action '%s'", pstr);

      result[nresult][count].info = atoi(pstr);/*(int)strtod(pstr, NULL);*/
      count++;
      if (count >= ACTIONS_MAX)
        break;

      pstr = strtok_r(NULL, "+", &psave2);
    }

    num_actions[nresult++] = count;
    if (nresult >= THRESHOLDS_MAX)
      break;
    pvalue = strtok_r(NULL, "\t \r\n", &psave);
  }

  therm_log_printf(THERM_LOG_LEVEL_DEBUG, 1, "No. of items found : %d",
                   nresult);

  *ppos = ptmp + 1;
  return nresult;
}

int parse_values(char **ppos, double *result)
{
  char *pos = *ppos;
  char *ptmp;
  char *pvalue, *psave = NULL;
  int  nresult = 0;

  ptmp = strchr(pos, '\n');
  if (ptmp)
    *ptmp = '\0';

  pvalue = strtok_r(pos, "\t \r\n", &psave);

  while (pvalue != NULL)
  {
    result[nresult] = atoi(pvalue);/*strtod(pvalue, NULL);*/
    nresult++;
    if (nresult >= THRESHOLDS_MAX)
      break;
    pvalue = strtok_r(NULL, "\t \r\n", &psave);
  }

  therm_log_printf(THERM_LOG_LEVEL_DEBUG, 1, "No. of items found : %d",
                   nresult);

  *ppos = ptmp + 1;
  return nresult;
}

static int parse_tm_section(struct setting_info *section, char *pos,
                            char *end_of_section)
{
  int  i;
  char *maxpos = end_of_section;
  int error_found = 0;
  char *idx;
  int in_field = FIELD_IDX_MAX;

  therm_log_printf(THERM_LOG_LEVEL_INFO, 3, "%s: Parsing section %s %s",
                   __func__, section->desc, pos);
  while (pos && (*pos != '\0') && (pos < maxpos))
  {
    switch (*pos)
    {
    case '\t':
    case '\r':
    case '\n':
    case ' ':
      skip_space(&pos);;
      break;

    default:
      if (in_field != FIELD_IDX_MAX)
      {
        int    j;
        int    num;
        double values[THRESHOLDS_MAX];
        int    num_actions[THRESHOLDS_MAX];
        struct action_t  actions[THRESHOLDS_MAX][ACTIONS_MAX];
        char *result_str;

        memset(&actions, 0x0, sizeof(actions));

        switch (in_field)
        {
        case SAMPLING:
          num = parse_values(&pos, values);
          if (num != 1)
          {
            therm_log_printf(THERM_LOG_LEVEL_ERROR, 0, "Invalid sampling value");
            error_found = 1;
            break;
          }
          if (values[0] < SAMPLING_MS_MINIMUM)
          {
            values[0] = SAMPLING_MS_MINIMUM;
            therm_log_printf(THERM_LOG_LEVEL_DEBUG, 1,
                             "Sampling time specified too low, using %d ms",
                             SAMPLING_MS_MINIMUM);
          }
          section->data.tm.sampling_period_ms = (int)values[0];
          break;
        case THRESHOLDS:
          num = parse_values(&pos, values);
          if (num < 1)
          {
            therm_log_printf(THERM_LOG_LEVEL_ERROR, 0, "Invalid thresholds");
            error_found = 1;
            break;
          }
          section->data.tm._n_thresholds = num;
          for (i = 0; i < num; i++)
          {
            section->data.tm.t[i].lvl_trig = values[i];
          }
          break;
        case THRESHOLDS_CLR:
          num = parse_values(&pos, values);
          if (num < 1)
          {
            therm_log_printf(THERM_LOG_LEVEL_ERROR, 0,
                             "Invalid thresholds_clr");
            error_found = 1;
            break;
          }
          section->data.tm._n_to_clear = num;
          for (i = 0; i < num; i++)
          {
            section->data.tm.t[i].lvl_clr = values[i];
          }
          break;
        case ACTIONS:
          num = parse_action_values(&pos, actions, num_actions);
          if (num < 1)
          {
            therm_log_printf(THERM_LOG_LEVEL_ERROR, 0, "Invalid actions");
            error_found = 1;
            break;
          }
          section->data.tm._n_actions = num;
          for (i = 0; i < num; i++)
          {
            section->data.tm.t[i].num_actions = num_actions[i];
            for (j = 0; j < num_actions[i]; j++)
            {
              strlcpy(section->data.tm.t[i].actions[j].device,
                      actions[i][j].device, DEVICES_MAX_NAME_LEN);
              therm_log_printf(THERM_LOG_LEVEL_DEBUG, 3,
                               "Threshold %d Action[%d] : %s", i, j,
                               section->data.tm.t[i].actions[j].device);
            }
          }
          break;
        case ACTION_INFO:
          num = parse_action_info_values(&pos, actions, num_actions);
          if (num < 1)
          {
            therm_log_printf(THERM_LOG_LEVEL_ERROR, 0, "Invalid action info");
            error_found = 1;
            break;
          }
          section->data.tm._n_action_info  = num;
          for (i = 0; i < num; i++)
          {
            for (j = 0; j < num_actions[i]; j++)
            {
              section->data.tm.t[i].actions[j].info = actions[i][j].info;
              therm_log_printf(THERM_LOG_LEVEL_DEBUG, 3,
                               "Threshold %d Action Info[%d] : %d", i, j,
                               actions[i][j].info);
            }
          }
          break;
        case DISABLE:
          num = parse_values(&pos, values);
          if (num != 1)
          {
            therm_log_printf(THERM_LOG_LEVEL_ERROR, 0,
                             "Invalid disable value");
            error_found = 1;
            break;
          }
          (values[0]) ? (section->disable = 1) :
                        (section->disable = 1);
          break;
        case SENSOR_FIELD:
          result_str = NULL;
          if (parse_sensor_field(&pos, &result_str) != 0)
          {
            therm_log_printf(THERM_LOG_LEVEL_ERROR, 0, "Invalid sensor");
            error_found = 1;
            break;
          }
          strlcpy(section->data.pid.sensor, result_str,
                  MAX_SENSOR_NAME_LEN);
          break;
        case DESCENDING:
          therm_log_printf(THERM_LOG_LEVEL_DEBUG, 0,
                           "Descending threshold triggers");
          section->data.tm.descending_thresh = 1;
          break;
        }
        in_field = FIELD_IDX_MAX;
        break;
      }

      idx = strpbrk(pos, "\t\r\n ");
      if (!idx)
      {
        therm_log_printf(THERM_LOG_LEVEL_ERROR, 0, "Error in file");
        error_found = 1;
        break;
      }
      *idx = '\0';
      for (i = 0; i < ARRAY_SIZE(tm_fields); i++)
      {
        if (strcasecmp(pos, tm_fields[i].name) != 0)
          continue;
        pos = idx + 1;
        skip_space(&pos);
        therm_log_printf(THERM_LOG_LEVEL_DEBUG, 1, "Found field '%s'",
                         tm_fields[i].name);
        in_field = tm_fields[i].type;
        break;
      }
      if (i == ARRAY_SIZE(tm_fields))
      {
        therm_log_printf(THERM_LOG_LEVEL_ERROR, 1,
                         "Ignoring unknown field '%s'", pos);
        pos = idx + 1;
        skip_line(&pos);
      }
      break;
    }

    if (error_found)
    {
      return -(EFAULT);
    }
  }
  return 0;
}

static int parse_pid_section(struct setting_info *section, char *pos,
                             char *end_of_section)
{
  int i;
  char *maxpos = end_of_section;
  int error_found = 0;
  char *idx;
  int in_field = FIELD_IDX_MAX;

  therm_log_printf(THERM_LOG_LEVEL_DEBUG, 3, "%s: Parsing section %s %s",
                   __func__, section->desc, pos);

  while (pos && (*pos != '\0') && (pos < maxpos))
  {
    switch (*pos)
    {
    case '\t':
    case '\r':
    case '\n':
    case ' ':
      skip_space(&pos);;
      break;

    default:
      if (in_field != FIELD_IDX_MAX)
      {
        int    num;
        double values[THRESHOLDS_MAX];
        float  fvalue;
        char   *result_str;

        switch (in_field)
        {
        case P_CONST:
          if (parse_float(&pos, &fvalue) == 0)
          {
            section->data.pid.p_const = fvalue;
            therm_log_printf(THERM_LOG_LEVEL_DEBUG, 2, "%s: %f",
                             pid_fields[in_field].name, fvalue);
          }
          break;
        case I_CONST:
          if (parse_float(&pos, &fvalue) == 0)
          {
            section->data.pid.i_const = fvalue;
            therm_log_printf(THERM_LOG_LEVEL_DEBUG, 2, "%s: %f",
                             pid_fields[in_field].name, fvalue);
          }
          break;
        case D_CONST:
          if (parse_float(&pos, &fvalue) == 0)
          {
            section->data.pid.d_const = fvalue;
            therm_log_printf(THERM_LOG_LEVEL_DEBUG, 2, "%s: %f",
                             pid_fields[in_field].name, fvalue);
          }
          break;
        case ERR_WEIGHT:
          if (parse_float(&pos, &fvalue) == 0)
          {
            section->data.pid.err_weight = fvalue;
            therm_log_printf(THERM_LOG_LEVEL_DEBUG, 2,"%s: %f",
                             pid_fields[in_field].name, fvalue);
          }
          break;
        case I_SAMPLES:
          num = parse_values(&pos, values);
          if ((num != 1) || (values[0] <= 0))
          {
            therm_log_printf(THERM_LOG_LEVEL_ERROR, 1,
                             "Sampling window invalid, using %d", I_SAMPLES);
            error_found = 1;
            break;
          }
          section->data.pid.i_samples = values[0];
          break;
        case UNITS_PER_CALC:
          num = parse_values(&pos, values);
          if (num != 1)
          {
            therm_log_printf(THERM_LOG_LEVEL_ERROR, 0,
                             "Invalid units per C value");
            error_found = 1;
            break;
          }
          section->data.pid.units_per_C = values[0];
          break;
        case DISABLE:
          num = parse_values(&pos, values);
          if (num != 1)
          {
            therm_log_printf(THERM_LOG_LEVEL_ERROR, 0,
                             "Invalid disable value");
            error_found = 1;
            break;
          }
          (values[0]) ? (section->disable = 1) :
                        (section->disable = 1);
          break;
        case SET_POINT:
          num = parse_values(&pos, values);
          if (num != 1)
          {
            therm_log_printf(THERM_LOG_LEVEL_ERROR, 0,
                             "Invalid set_point value");
            error_found = 1;
            break;
          }
          section->data.pid.set_point = values[0];
          break;
        case SET_POINT_CLR:
          num = parse_values(&pos, values);
          if (num != 1)
          {
            therm_log_printf(THERM_LOG_LEVEL_ERROR, 0,
                             "Invalid set_point value");
            error_found = 1;
            break;
          }
          section->data.pid.set_point_clr = values[0];
          break;
        case SAMPLING:
          num = parse_values(&pos, values);
          if (num != 1)
          {
            therm_log_printf(THERM_LOG_LEVEL_ERROR, 0,
                             "Invalid sampling value");
            error_found = 1;
            break;
          }
          if (values[0] < SAMPLING_MS_MINIMUM)
          {
            values[0] = SAMPLING_MS_MINIMUM;
            therm_log_printf(THERM_LOG_LEVEL_DEBUG, 1,
                             "Sampling time specified too low, using %d ms",
                             SAMPLING_MS_MINIMUM);
          }
          section->data.pid.sampling_period_ms = (int)values[0];
          break;
        case SENSOR_FIELD:
          result_str = NULL;
          if (parse_sensor_field(&pos, &result_str) != 0)
          {
            therm_log_printf(THERM_LOG_LEVEL_ERROR, 0, "Invalid sensor");
            error_found = 1;
            break;
          }
          strlcpy(section->data.pid.sensor, result_str,
                  MAX_SENSOR_NAME_LEN);
          break;
        case DEVICE_FIELD:
          result_str = NULL;
          if (parse_device_field(&pos, &result_str) != 0)
          {
            therm_log_printf(THERM_LOG_LEVEL_ERROR, 0, "Invalid device");
            error_found = 1;
            break;
          }
          strlcpy(section->data.pid.device, result_str,
                  DEVICES_MAX_NAME_LEN);
          break;
        }
        in_field = FIELD_IDX_MAX;
        break;
      }

      idx = strpbrk(pos, "\t\r\n ");
      if (!idx)
      {
        therm_log_printf(THERM_LOG_LEVEL_ERROR, 0, "Error in file");
        error_found = 1;
        break;
      }
      *idx = '\0';
      for (i = 0; i < ARRAY_SIZE(pid_fields); i++)
      {
        if (strcasecmp(pos, pid_fields[i].name) != 0)
          continue;
        pos = idx + 1;
        skip_space(&pos);
        therm_log_printf(THERM_LOG_LEVEL_DEBUG, 1, "Found field '%s'",
                         pid_fields[i].name);
        in_field = pid_fields[i].type;
        break;
      }
      if (i == ARRAY_SIZE(pid_fields))
      {
        therm_log_printf(THERM_LOG_LEVEL_ERROR, 1,
                         "Ignoring unknown field '%s'", pos);
        pos = idx + 1;
        skip_line(&pos);
      }
      break;
    }

    if (error_found)
    {
      return -(EFAULT);
    }
  }
  return 0;
}

static int parse_ss_section(struct setting_info *section, char *pos,
                            char *end_of_section)
{
  int i;
  char *maxpos = end_of_section;
  int error_found = 0;
  char *idx;
  int in_field = FIELD_IDX_MAX;

  therm_log_printf(THERM_LOG_LEVEL_DEBUG, 3, "%s: Parsing section %s %s",
                   __func__, section->desc, pos);

  while (pos && (*pos != '\0') && (pos < maxpos))
  {
    switch (*pos)
    {
    case '\t':
    case '\r':
    case '\n':
    case ' ':
      skip_space(&pos);;
      break;

    default:
      if (in_field != FIELD_IDX_MAX)
      {
        int    num;
        double values[THRESHOLDS_MAX];
        char   *result_str;

        switch (in_field)
        {
        case DISABLE:
          num = parse_values(&pos, values);
          if (num != 1)
          {
            therm_log_printf(THERM_LOG_LEVEL_ERROR, 0,
                             "Invalid disable value");
            error_found = 1;
            break;
          }
          (values[0]) ? (section->disable = 1) :
                        (section->disable = 1);
          break;
        case SET_POINT:
          num = parse_values(&pos, values);
          if (num != 1)
          {
            therm_log_printf(THERM_LOG_LEVEL_ERROR, 0,
                             "Invalid set_point value");
            error_found = 1;
            break;
          }
          section->data.ss.set_point = values[0];
          break;
        case SET_POINT_CLR:
          num = parse_values(&pos, values);
          if (num != 1)
          {
            therm_log_printf(THERM_LOG_LEVEL_ERROR, 0,
                             "Invalid set_point_clr value");
            error_found = 1;
            break;
          }
          section->data.ss.set_point_clr = values[0];
          break;
        case SAMPLING:
          num = parse_values(&pos, values);
          if (num != 1)
          {
            therm_log_printf(THERM_LOG_LEVEL_ERROR, 0,
                             "Invalid sampling value");
            error_found = 1;
            break;
          }
          if (values[0] < SAMPLING_MS_MINIMUM)
          {
            values[0] = SAMPLING_MS_MINIMUM;
            therm_log_printf(THERM_LOG_LEVEL_ERROR, 1,
                             "Sampling time specified too low, using %d ms",
                             SAMPLING_MS_MINIMUM);
          }
          section->data.ss.sampling_period_ms = (int)values[0];
          break;
        case SENSOR_FIELD:
          result_str = NULL;
          if (parse_sensor_field(&pos, &result_str) != 0)
          {
            therm_log_printf(THERM_LOG_LEVEL_ERROR, 0, "Invalid sensor");
            error_found = 1;
            break;
          }
          strlcpy(section->data.ss.sensor, result_str,
                  MAX_SENSOR_NAME_LEN);
          break;
        case DEVICE_FIELD:
          result_str = NULL;
          if (parse_device_field(&pos, &result_str) != 0)
          {
            therm_log_printf(THERM_LOG_LEVEL_ERROR, 0, "Invalid device");
            error_found = 1;
            break;
          }
          strlcpy(section->data.ss.device, result_str,
                  DEVICES_MAX_NAME_LEN);
          break;
        case TIME_CONSTANT:
          num = parse_values(&pos, values);
          if (num != 1)
          {
            therm_log_printf(THERM_LOG_LEVEL_ERROR, 0,
                             "Invalid Time Constant value");
            error_found = 1;
            break;
          }
          if (values[0] < 0)
          {
            therm_log_printf(THERM_LOG_LEVEL_ERROR, 1,
                             "Invalid TC specified %d ms", (int)values[0]);
            values[0] = 0;
          }
          section->data.ss.time_constant = (int)values[0];
          break;
        }
        in_field = FIELD_IDX_MAX;
        break;
      }

      idx = strpbrk(pos, "\t\r\n ");
      if (!idx)
      {
        therm_log_printf(THERM_LOG_LEVEL_ERROR, 0, "Error in file");
        error_found = 1;
        break;
      }
      *idx = '\0';
      for (i = 0; i < ARRAY_SIZE(ss_fields); i++)
      {
        if (strcasecmp(pos, ss_fields[i].name) != 0)
          continue;
        pos = idx + 1;
        skip_space(&pos);
        therm_log_printf(THERM_LOG_LEVEL_DEBUG, 1, "Found field '%s'",
                         ss_fields[i].name);
        in_field = ss_fields[i].type;
        break;
      }
      if (i == ARRAY_SIZE(ss_fields))
      {
        therm_log_printf(THERM_LOG_LEVEL_ERROR, 1,
                         "Ignoring unknown field '%s'", pos);
        pos = idx + 1;
        skip_line(&pos);
      }
      break;
    }

    if (error_found)
    {
      return -(EFAULT);
    }
  }
  return 0;
}

static int parse_section(struct setting_info *section, char *pos,
                         char *end_of_section)
{
  char *idx;
  char *maxpos = end_of_section;

  therm_log_printf(THERM_LOG_LEVEL_DEBUG, 1, "Parsing section %s",
                   section->desc);

  /* Back up end of section so as not to destroy the start of another
     section */
  if (*maxpos == '[')
    --maxpos;

  *maxpos = '\0';

  /* Skip to first actual character. */
  skip_space(&pos);

  if (section->dyanmic)
  {
    idx = strpbrk(pos, "\t\r\n ");
    if (!idx)
    {
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: Error in section",
                       __func__);
      return -(EFAULT);
    }
    *idx = '\0';

    /* First field should be algo_type */
    if (strcasecmp(pos, "algo_type") != 0)
    {
      therm_log_printf(THERM_LOG_LEVEL_ERROR, 1,
                       "%s: algo_type field not first field in section",
                       __func__);
      return -(EFAULT);
    }

    pos = idx + 1;
    skip_space(&pos);
    section->algo_type = parse_algo(&pos);
    skip_space(&pos);
  }

  switch (section->algo_type)
  {
  case MONITOR_ALGO_TYPE:
    if (parse_tm_section(section, pos, maxpos) != 0)
      section->disable = 1;
    break;
  case PID_ALGO_TYPE:
    if (parse_pid_section(section, pos, maxpos) != 0)
      section->disable = 1;
    break;
  case SS_ALGO_TYPE:
    if (parse_ss_section(section, pos, maxpos) != 0)
      section->disable = 1;
    break;
  default:
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 2,
                     "%s: Uknown algo type for section %s", __func__,
                     section->desc);
    section->disable = 1;
    break;
  }
  return 0;
}

struct setting_info* find_section_id(struct thermal_setting_t *settings,
                                     const char *desc)
{
  struct setting_info *curr = NULL;

  curr = settings->list;
  while (curr)
  {
    if (strncasecmp(curr->desc, desc, MAX_ALGO_DESC) == 0)
      return curr;
    curr = curr->next;
  }
  return NULL;
}

int parse_config(struct thermal_setting_t *settings)
{
  int sz, i;
  char *pos = buf;
  char *maxpos;
  char *end_of_section = NULL;
  int error_found = 0;
  char *idx;
  struct setting_info *in_section = NULL;
  int in_field = FIELD_IDX_MAX;

  therm_log_printf(THERM_LOG_LEVEL_INFO, 0,
                   "Loading configuration file...");

  sz = buf_bytes_read;
  if (sz < 0)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 0, "Failed to read config file");
    return -(EIO);
  }
  maxpos = pos + sz;
  therm_log_printf(THERM_LOG_LEVEL_INFO, 1, "Config file read (%d bytes)", sz);

  buf[CONFIG_FILE_SIZE_MAX - 1] = '\0';

  while (pos && (*pos != '\0') && (pos < maxpos))
  {
    switch (*pos)
    {
    case '[':
      idx = strchr(++pos, ']');
      if (!idx)
      {
        error_found = 1;
        break;
      }
      in_field = FIELD_IDX_MAX;
      *idx = '\0';
      /* Look for existing section */
      in_section = find_section_id(settings, pos);

      /*If it doesn't exist allocate one */
      if (in_section == NULL)
      {
        in_section = (struct setting_info *)Core_Malloc(sizeof(struct setting_info));
        if (NULL == in_section)
        {
          therm_log_printf(THERM_LOG_LEVEL_ERROR, 0,
                           "Failed to alloc tm struct setting_info");
          error_found = 1;
          break;
        }
        init_tm_settings(in_section);

        /* Insert into the config list head */
        add_setting(settings, in_section);

        strlcpy(in_section->desc, pos, MAX_ALGO_DESC);
        in_section->dyanmic = 1;
        therm_log_printf(THERM_LOG_LEVEL_INFO, 1,"Create section '%s'",
                         in_section->desc);
      }
      else
        therm_log_printf(THERM_LOG_LEVEL_INFO, 1,"Found section '%s'",
                         in_section->desc);
      pos = idx + 1;

      /* Find end of section */
      end_of_section = strpbrk(pos, "[");
      if (!end_of_section)
        end_of_section = maxpos;

      parse_section(in_section, pos, end_of_section);
      pos = end_of_section;
      break;

    case '\t':
    case '\r':
    case '\n':
    case ' ':
      skip_space(&pos);;
      break;

    default:
      if (in_field != FIELD_IDX_MAX)
      {
        int    num;
        double values[THRESHOLDS_MAX];

        switch (in_field)
        {
        case DEBUG:
          therm_log_printf(THERM_LOG_LEVEL_INFO, 0,
                           "Debug output enabled from config");
          debug_output = 1;
          break;
        case SAMPLING:
          num = parse_values(&pos, values);
          if (num < 1)
            break;
          if (values[0] < SAMPLING_MS_MINIMUM)
          {
            values[0] = SAMPLING_MS_MINIMUM;
            therm_log_printf(THERM_LOG_LEVEL_DEBUG, 1,
                             "Sampling time specified too low, using %d ms",
                             SAMPLING_MS_MINIMUM);
          }
          settings->sample_period_ms = (int)values[0];
          break;
        }
        in_field = FIELD_IDX_MAX;
        break;
      }

      idx = strpbrk(pos, "\t\r\n ");
      if (!idx)
      {
        therm_log_printf(THERM_LOG_LEVEL_ERROR, 0, "Error in file");
        error_found = 1;
        break;
      }
      *idx = '\0';
      for (i = 0; i < ARRAY_SIZE(fields); i++)
      {
        if (strcasecmp(pos, fields[i].name) != 0)
          continue;
        pos = idx + 1;
        skip_space(&pos);
        therm_log_printf(THERM_LOG_LEVEL_DEBUG, 1, "Found field '%s'",
                         fields[i].name);
        in_field = i;
        break;
      }
      if (i == ARRAY_SIZE(fields))
      {
        therm_log_printf(THERM_LOG_LEVEL_ERROR, 1,
                         "Ignoring unknown field '%s'", pos);
        pos = idx + 1;
        skip_line(&pos);
      }
      break;
    }

    if (error_found)
    {
      return -(EFAULT);
    }
  }
  return 0;
}

int threshold_array_compare(const void *x, const void *y)
{
  struct threshold_t *i = (struct threshold_t *)x;
  struct threshold_t *j = (struct threshold_t *)y;

  if (i->lvl_trig < j->lvl_trig)
    return -1;
  else if (i->lvl_trig > j->lvl_trig)
    return 1;
  return 0;
}

int descending_threshold_array_compare(const void *x, const void *y)
{
  struct threshold_t *i = (struct threshold_t *)x;
  struct threshold_t *j = (struct threshold_t *)y;

  if (i->lvl_trig < j->lvl_trig)
    return 1;
  else if (i->lvl_trig > j->lvl_trig)
    return -1;
  return 0;
}

static void fallback_sampling_values(struct thermal_setting_t *settings,
                                     struct setting_info *section)
{
  /* Fallback to default sampling times if sampling period
     was not configured */
  if (settings == NULL ||
      section == NULL)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1, "%s: unexpected NULL",
                     __func__);
    return;
  }

  if (settings->sample_period_ms > 0 &&
      section->data.tm.sampling_period_ms == -1)
  {
    section->data.tm.sampling_period_ms =
      settings->sample_period_ms;
    therm_log_printf(THERM_LOG_LEVEL_DEBUG, 3, "Using configured default "
                     "sampling period %dms for section[%s] sensor[%s]",
                     settings->sample_period_ms, section->desc,
                     section->data.tm.sensor);
  }
  else if (settings->sample_period_ms == -1 &&
           section->data.tm.sampling_period_ms == -1)
  {
    section->data.tm.sampling_period_ms =
      SAMPLING_MS_DEFAULT;
    therm_log_printf(THERM_LOG_LEVEL_DEBUG, 3, "Using default sampling period "
                     " %dms for section[%s] sensor[%s]\n", SAMPLING_MS_DEFAULT,
                     section->desc, section->data.tm.sensor);
  }
}

void validate_config(struct thermal_setting_t *settings)
{
  struct setting_info *s, *next_s = NULL;
  int j, k;

  s = settings->list;
  while (s != NULL)
  {
    next_s = s->next;

    /* Only validate monitor type configs */
    if (s->algo_type != MONITOR_ALGO_TYPE)
    {
      s = next_s;
      continue;
    }

    /* Disable no threshold entries */
    if (s->data.tm._n_thresholds == 0)
    {
      s->disable = 1;
      s = next_s;
      continue;
    }

    /* Fill out the number of thresholds based on min entry */
    s->data.tm.num_thresholds = s->data.tm._n_thresholds;
    if (s->data.tm.num_thresholds > s->data.tm._n_to_clear)
      s->data.tm.num_thresholds = s->data.tm._n_to_clear;
    if (s->data.tm.num_thresholds > s->data.tm._n_actions)
      s->data.tm.num_thresholds = s->data.tm._n_actions;
    if (s->data.tm.num_thresholds > s->data.tm._n_action_info)
      s->data.tm.num_thresholds = s->data.tm._n_action_info;

    /* Ensure thresolds are in ascending or descending order
       depending on algorithm instance configuration */
    if (s->data.tm.num_thresholds > 1)
    {
      if (s->data.tm.descending_thresh)
        qsort(s->data.tm.t, s->data.tm.num_thresholds,
              sizeof(struct threshold_t),
              descending_threshold_array_compare);
      else
        qsort(s->data.tm.t, s->data.tm.num_thresholds,
              sizeof(struct threshold_t),
              threshold_array_compare);
    }

    /* Sort so as to move shutdown to last action */
    for (j = 0; j < s->data.tm.num_thresholds; j++)
    {
      for (k = 0; k < s->data.tm.t[j].num_actions; k++)
      {
        if ((k < s->data.tm.t[j].num_actions - 1) &&
            (s->data.tm.t[j].actions[k].device != NULL) &&
            (strncasecmp(s->data.tm.t[j].actions[k].device,
                         "shutdown", DEVICES_MAX_NAME_LEN) == 0))
        {
          struct action_t temp_action;

          /* Swap entries */
          temp_action = s->data.tm.t[j].actions[k];
          s->data.tm.t[j].actions[k] = s->data.tm.t[j].actions[k+1];
          s->data.tm.t[j].actions[k+1] = temp_action;
        }
      }
    }

    /* Fallback on default sampling times, if not configured */
    fallback_sampling_values(settings, s);

    therm_log_printf(THERM_LOG_LEVEL_INFO, 3,
                     "Desc '%s' Sensor '%s' config:  sampling %d", s->desc,
                     s->data.tm.sensor, s->data.tm.sampling_period_ms);
    for (j = 0; j < s->data.tm.num_thresholds; j++)
    {
      for (k = 0; k < s->data.tm.t[j].num_actions; k++)
      {
        therm_log_printf(THERM_LOG_LEVEL_INFO, 4," %d %d %s %d",
                         s->data.tm.t[j].lvl_trig,
                         s->data.tm.t[j].lvl_clr,
                         (s->data.tm.t[j].actions[k].device != NULL) ?
                         (s->data.tm.t[j].actions[k].device) : ("unknown"),
                         s->data.tm.t[j].actions[k].info);
      }
    }
    s = next_s;
  }
}

int load_config(struct thermal_setting_t *settings, const char *pFName)
{
  int ret_val = 0;
  const char *cf = (pFName) ? pFName : CONFIG_FILE_DEFAULT;
  struct fs_stat fbuf;
  
  if( efs_stat(cf,&fbuf) < 0){
	  ret_val = -(EIO);
	  goto error_handler;
  }

  buf_bytes_read = efs_get(cf, buf, fbuf.st_size);
  
  if (buf_bytes_read < 1)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1,
                     "Unable to open config file '%s'", cf);
    ret_val = -(EIO);
    goto error_handler;
  }

  /* Get sensors list count*/
  if (sensors_manager_get_list(NULL, &sensor_info_arr_len))
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 0,
                     "Failed to get sensor list length");
    ret_val = -(EFAULT);
    goto error_handler;
  }

  sensor_info_arr = (struct sensor_info_type *)Core_Malloc(sizeof(struct sensor_info_type) * sensor_info_arr_len);
  if (NULL == sensor_info_arr)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 0, "Failed to alloc ts_info_arr");
    ret_val = -(EFAULT);
    goto error_handler;
  }

  if (sensors_manager_get_list(sensor_info_arr,
                               &sensor_info_arr_len))
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 0, "Failed to get sensor list");
    ret_val = -(EFAULT);
    goto error_handler;
  }

  if (devices_manager_get_list(NULL, &device_info_arr_len))
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 0,
                     "Failed to get tmd list length");
    ret_val = -(EFAULT);
    goto error_handler;
  }

  device_info_arr = (struct device_info *)Core_Malloc(sizeof(struct device_info) * device_info_arr_len);
  if (NULL == device_info_arr)
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 0,
                     "Failed to alloc tmd_info_arr");
    ret_val = -(EFAULT);
    goto error_handler;
  }

  if (devices_manager_get_list(device_info_arr,
                               &device_info_arr_len))
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 0, "Failed to get tmd list");
    ret_val = -(EFAULT);
    goto error_handler;
  }

  if (parse_config(settings))
  {
    therm_log_printf(THERM_LOG_LEVEL_ERROR, 1,
                     "Failed to parse config file '%s'", cf);
    ret_val = -(EFAULT);
    goto error_handler;
  }

  validate_config(settings);

error_handler:
  if (ret_val)
  {
    if (sensor_info_arr)
      Core_Free(sensor_info_arr);

    if (device_info_arr)
      Core_Free(device_info_arr);
  }
  return ret_val;
}

