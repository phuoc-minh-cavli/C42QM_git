#ifndef THERM_CONFIG_H
#define THERM_CONFIG_H

/*============================================================================
  FILE:         therm_config.h

  OVERVIEW:     This file declares the private interface to thermal SW.

                Copyright (c) 2019 QUALCOMM Technologies
                Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/*=======================================================================
$Header: //components/rel/core.mpss/3.10/power/thermal/inc/therm_config.h#1 $
$DateTime: 2019/06/20 01:03:47 $
$Author: pwbldsvc $
========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "../src/common/therm_monitor.h"

/*----------------------------------------------------------------------------
 * Type Definitions
 * -------------------------------------------------------------------------*/

typedef struct
{  
  /* Resource path name */
  char                             sensor_resource_path[MAX_RESOURCE_PATH];
  
  /* Resource path name */
  char                             sampling_resource_path[MAX_RESOURCE_PATH];
  
  /* Resource path name */
  char                             mitigate_state_resource_path[MAX_RESOURCE_PATH];
  
  /* Number of sampling states */
  int                              num_of_states;
  
  /* Individual states thresholds and actions to take */
  therm_monitor_thresh_state_type  *state_data;
} therm_monitor_default_type;	

/*===========================================================================

                     EXTERNAL FUNCTION DECLARATIONS
                     
===========================================================================*/
/**
  @brief therm_monitor_load_default_config

  Thermal monitor load default config data.

  @param  : None.

  @return : MONITOR_SUCCESS or MONITOR_FAILED.

*/
int therm_monitor_load_default_config(therm_monitor_type *config_data);
#endif /* THERM_CONFIG_H */