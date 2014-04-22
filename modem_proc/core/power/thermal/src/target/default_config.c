/*============================================================================
  @file default_config.c

  Loads Default Config_data if config.ini is not pushed 

  Copyright (c) 2019 QUALCOMM Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/

/*=======================================================================
$Header: //components/rel/core.mpss/3.10/power/thermal/src/target/default_config.c#3 $
$DateTime: 2019/09/16 02:09:44 $
$Author: pwbldsvc $
========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "stdlib.h"
#include "therm_config.h"
#include "stdio.h"

/*=======================================================================

                         LOCAL DEFINITIONS

========================================================================*/
#define ARRAY_SIZE(x) sizeof(x)/sizeof(x[0])

therm_monitor_thresh_state_type qfe_pa0_statedata[] =  
{
       {
        .sampling =5000,
        .thresh = 114,
        .thresh_clr = -273,
        .action = THERM_MONITOR_SET_MITIGATE_STATE,
        .action_info = 0,
       },
       {
        .sampling =5000,
        .thresh = 120,
        .thresh_clr = 105,
        .action = THERM_MONITOR_SET_MITIGATE_STATE,
        .action_info = 2,
       },
       {
        .sampling =5000,
        .thresh = 125,
        .thresh_clr = 115,
        .action = THERM_MONITOR_SET_MITIGATE_STATE,
        .action_info = 3,
       }
};

therm_monitor_thresh_state_type modem_tsens_statedata[] =  
{
       {
        .sampling =1000,
        .thresh = 102,
        .thresh_clr = -273,
        .action = THERM_MONITOR_SET_MITIGATE_STATE,
        .action_info = 0,
       },
       {
        .sampling =1000,
        .thresh = 125,
        .thresh_clr = 97,
        .action = THERM_MONITOR_SET_MITIGATE_STATE,
        .action_info = 3,
        }
};

therm_monitor_default_type default_config_data[] =
{
      {
        .sensor_resource_path = "/therm/sensor/qfe_pa0",
        .sampling_resource_path = "/therm/sensor/qfe_pa0/sampling",
        .mitigate_state_resource_path = "/therm/mitigate/pa",
        .num_of_states = ARRAY_SIZE(qfe_pa0_statedata),
        .state_data = qfe_pa0_statedata,
      },
      {
        .sensor_resource_path = "/therm/sensor/modem_tsens",
        .sampling_resource_path = "/therm/sensor/modem_tsens/sampling",
        .mitigate_state_resource_path = "/therm/mitigate/pa",
        .num_of_states =  ARRAY_SIZE(modem_tsens_statedata),
        .state_data = modem_tsens_statedata,
       },   
};

/*=======================================================================

                  LOCAL FUNCTION DEFINITIONS

========================================================================*/
/**
  @brief therm_monitor_load_default_config

  Thermal monitor load default config data if config file not present.

  @param  : None.

  @return : MONITOR_SUCCESS or MONITOR_FAILED.

*/

int therm_monitor_load_default_config(therm_monitor_type *config_data)
{
  
  int i= 0, j = 0, size = 0; 
 
  size = ARRAY_SIZE(default_config_data);
  config_data->num_valid_sensors = size;
  for(i = 0 ; i < size ; i++)
  {
    snprintf(config_data->sensor[i].sensor_resource_path, MAX_RESOURCE_PATH, "%s", default_config_data[i].sensor_resource_path);
    snprintf(config_data->sensor[i].sampling_resource_path, MAX_RESOURCE_PATH, "%s", default_config_data[i].sampling_resource_path);
    snprintf(config_data->sensor[i].mitigate_state_resource_path, MAX_RESOURCE_PATH, "%s", default_config_data[i].mitigate_state_resource_path);
    config_data->sensor[i].num_of_states = default_config_data[i].num_of_states;
    for(j = 0 ; j < default_config_data[i].num_of_states; j++)
    { 
      config_data->sensor[i].state_data[j].sampling = default_config_data[i].state_data[j].sampling;
      config_data->sensor[i].state_data[j].thresh = default_config_data[i].state_data[j].thresh;
      config_data->sensor[i].state_data[j].thresh_clr = default_config_data[i].state_data[j].thresh_clr;
      config_data->sensor[i].state_data[j].action = default_config_data[i].state_data[j].action;
      config_data->sensor[i].state_data[j].action_info = default_config_data[i].state_data[j].action_info;
    }
  }
  return MONITOR_SUCCESS;
}
