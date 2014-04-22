/*===========================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#include "cpr_voltage_ranges.h"
#include "cpr_target_quotients.h"
#include "CoreVerify.h"
#include "cpr_logging.h"
#include <string.h>


/*===========================================================================
FUNCTION: cpr_config_find_device_target_quotients

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
static const cpr_target_quotient_versioned_config_t*
cpr_config_find_device_target_quotients(const cpr_target_quotient_rail_config_t* rail_target_quotient_configs)
{
  for(int i=0; i<rail_target_quotient_configs->versioned_target_quotient_config_count; i++)
  {
    const cpr_target_quotient_versioned_config_t* versioned_target_quotients =
      rail_target_quotient_configs->versioned_target_quotient_config[i];

    //To do - factor out this hw version matching to a seperate function
    const cpr_config_hw_version_range* hw_versions = &versioned_target_quotients->hw_versions;

    if(cpr_device_hw_version_matches_this_device(hw_versions))
    {
      return versioned_target_quotients;
    }
  }
  return NULL;
}

/*===========================================================================
FUNCTION: cpr_target_quotient_find_rail_config

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
static const cpr_target_quotient_versioned_config_t*
cpr_target_quotient_find_rail_config(cpr_rail_id_t rail_id)
{
  for(int i=0; i<cpr_bsp_target_quotient_config.rail_config_count; i++)
  {
    const cpr_target_quotient_rail_config_t* rail_target_quotient_versions = cpr_bsp_target_quotient_config.rail_config[i];
    if(rail_target_quotient_versions->rail_id == rail_id)
    {
      return cpr_config_find_device_target_quotients(rail_target_quotient_versions);
    }
  }
  return NULL;
}

/*===========================================================================
FUNCTION: cpr_target_quotient_find_level

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
static const cpr_target_quotient_level_t*
cpr_target_quotient_find_level(const cpr_target_quotient_versioned_config_t* target_quotient_config, cpr_voltage_mode_t voltage_mode)
{
  CORE_VERIFY_PTR(target_quotient_config);
  for(int i=0; i<target_quotient_config->target_quotient_level_count; i++)
  {
    const cpr_target_quotient_level_t* target_quotient_level = &target_quotient_config->target_quotient_level[i];
    if(target_quotient_level->voltage_mode==voltage_mode)
    {
      return target_quotient_level;
    }
  }
  return NULL;
}

/*===========================================================================
FUNCTION: cpr_target_quotients_handle_rail_info_not_found

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
void cpr_target_quotients_handle_rail_info_not_found(cpr_target_quotient_set_t* target_quotients)
{
  target_quotients->ro_target_quotient = 0;
}

/*===========================================================================
FUNCTION: cpr_target_quotient_calculate_quotients

DESCRIPTION: RO number and RO quot value will be programmed in fuses. SW needs to read the RO number
             and RO quot value from fuses and program the quot val in the RO number specified in fuses.  

RETURN VALUE:
===========================================================================*/
void cpr_target_quotient_calculate_quotients(cpr_rail_id_t rail_id, cpr_voltage_mode_t voltage_mode, int32 static_margin_mv, cpr_target_quotient_set_t* target_quotients)
{
  uint32 ro_sel_fuse_val = 0, ro_quot_fuse_val = 0;
  const cpr_config_versioned_voltage_ranges_t* versioned_voltage_ranges;
  const cpr_config_voltage_level_t* cpr_config_voltage_level;
  
  
  const cpr_target_quotient_versioned_config_t* rail_target_quotient_config = cpr_target_quotient_find_rail_config(rail_id);
  if(!rail_target_quotient_config)
  {
    cpr_target_quotients_handle_rail_info_not_found(target_quotients);
    return;
  }
    
  const cpr_target_quotient_level_t* target_quotent_level_config = cpr_target_quotient_find_level(rail_target_quotient_config, voltage_mode);
  if(!target_quotent_level_config)
  {
    cpr_target_quotients_handle_rail_info_not_found(target_quotients);
    return;
  }
    
  versioned_voltage_ranges = cpr_config_find_versioned_voltage_ranges( rail_id );
  cpr_config_voltage_level = cpr_config_find_voltage_level(versioned_voltage_ranges, voltage_mode);
  
  CORE_VERIFY_PTR(cpr_config_voltage_level);
  
  ro_sel_fuse_val  = cpr_fuses_get_fuse_value(rail_id, cpr_config_voltage_level->fuse_type_ro_sel);  
  ro_quot_fuse_val = cpr_fuses_get_fuse_value(rail_id, cpr_config_voltage_level->fuse_type_ro_quot);

  target_quotients->rosc_num = (uint8)ro_sel_fuse_val;
  target_quotients->ro_target_quotient = ro_quot_fuse_val + (static_margin_mv * rail_target_quotient_config->ro_kv_x_100[ro_sel_fuse_val])/100;
}

