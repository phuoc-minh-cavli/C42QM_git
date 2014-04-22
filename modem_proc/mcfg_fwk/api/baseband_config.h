#pragma once
/*==============================================================================

        MODEM_CFG  -     Baseband_config.h

GENERAL DESCRIPTION
  APIs to access Baseband Configuration Items based on the profile configured in efs.

Copyright (c) 2014-2015 by QUALCOMM Technologies Incorporated.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/mcfg_fwk.mpss/7.10.1.1/api/baseband_config.h#2 $
$DateTime: 2021/03/09 06:47:03 $ 
$Author: pwbldsvc $
$Change: 29378162 $

when     who  what, where, why
-------- ---  ----------------------------------------------------------------
02/24/15 sbt   Initial creation

==============================================================================*/

/*==============================================================================

                PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================
  Constants and Macros
==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/

typedef enum
{
  BB_CONFIG_PARAM_MIN = 0,
  BB_CONFIG_PARAM_SLEEP_CLOCK_SOURCE = BB_CONFIG_PARAM_MIN,
  BB_CONFIG_PARAM_EARLY_TERMINATION,
  BB_CONFIG_PARAM_CONNECTED_MODE_SLEEP,
  BB_CONFIG_PARAM_POI,
  BB_CONFIG_PARAM_RAFTT,
  BB_CONFIG_PARAM_ML1_SLEEP_THRESHOLD,
  BB_CONFIG_PARAM_TTL_WARM_UP,
  BB_CONFIG_HIGH_THRESHOLD_SEARCH,
  BB_CONFIG_PARAM_MAX  
}bb_config_param_id_e_type;


typedef enum
{

  BB_CONFIG_PARAM_STATUS_OK = 0,
  BB_CONFIG_PARAM_STATUS_NOT_CONFIGURED,
  BB_CONFIG_PARAM_STATUS_UNSUPPORTED,
  BB_CONFIG_PARAM_STATUS_ERROR,
  BB_CONFIG_PARAM_STATUS_MAX 
} bb_config_param_status_e_type;


/*==============================================================================
                    PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/
/*===========================================================================

FUNCTION baseband_config_param_read()

DESCRIPTION
  It fetches the value of baseband parameter and populates the buffer passed into the API.
  
DEPENDENCIES
  None.

PARAMETERS
  bb_config_param_id_e_type                 [in]  Baseband parameter ID
  void *data_ptr,                           [out] Buffer to store the parameter value
  uint32 length, 							[in]  Size of parameter
   
   
RETURN VALUE

  bb_config_param_status_e_type 
  
  BB_CONFIG_PARAM_STATUS_OK  => Sucess case, baseband param value is enforced from framework
  BB_CONFIG_PARAM_STATUS_NOT_CONFIGURED => Profile not configured or profile read failed or profile configured as zero
  BB_CONFIG_PARAM_STATUS_UNSUPPORTED, => Parameter passed is not suppoted.
  BB_CONFIG_PARAM_STATUS_ERROR  => other Internal errors ex:like NULL PTR etc.
  
SIDE EFFECTS
  None

===========================================================================*/
bb_config_param_status_e_type baseband_config_param_read(
    bb_config_param_id_e_type baseband_parameter_id, 
	void *data_ptr,
	uint32 length);



