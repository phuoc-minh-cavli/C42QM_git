/*===========================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#include "cpr_closed_loop.h"
#include "CoreVerify.h"
#include <string.h>
#include "cpr_image.h"
#include "cpr_enablement.h"
#include "cpr_voltage_ranges.h"
#include "cpr_logging.h"
#include <stdlib.h>
#include "cpr_smem.h"
#ifndef WINSIM
#include "stringl/stringl.h"
#endif

cpr_closed_loop_rail_t cpr_closed_loop_rail_root = NULL;

void cpr_enable_init_fn_closed_loop(cpr_rail_id_t rail_id,
                                    const cpr_enablement_versioned_rail_config_t* rail_enablement_config,
                                    cpr_enablement_stage_t enablement_stage);

const cpr_enablement_init_params_t CPR_ENABLE_CLOSED_LOOP =
{
    .enablement_type        = CPR_ENABLEMENT_CLOSED_LOOP_ID,
    .enablement_fn          = cpr_enable_init_fn_closed_loop,
};


#define CPR_DEFAULT_GCNT 19     //We always use 19 since we always run the CPR Ref Clk at 19.2MHz

/*===========================================================================
FUNCTION: cpr_closed_loop_rail_config

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
static const cpr_closed_loop_rail_config_t* cpr_closed_loop_rail_config(cpr_rail_id_t rail_id)
{
  for(int i=0; i<cpr_bsp_closed_loop_config.closed_loop_config_count; i++)
  {
    const cpr_closed_loop_rail_config_t* rail_config = cpr_bsp_closed_loop_config.closed_loop_config[i];
    if(rail_id == rail_config->rail_id)
    {
      return rail_config;
    }
  }
  CORE_VERIFY(0); //Couldn't find the necessary config.
  return NULL;
}

/*===========================================================================
FUNCTION: cpr_find_corner_params

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
const cpr_corner_params_t* cpr_find_corner_params(cpr_closed_loop_rail_t rail, cpr_voltage_mode_t voltage_mode)
{
  for(int i=0; i<rail->target_params_count; i++)
  {
    const cpr_corner_params_t* corner_params = &rail->target_params[i];
    if(voltage_mode == corner_params->voltage_mode)
    {
      return corner_params;
    }
  }
  return NULL;
}

/*===========================================================================
FUNCTION: cpr_find_margin_adjustment

DESCRIPTION:  Walk the list of margin adjustments and find the specified type

RETURN VALUE:  A cpr_margin_adjustment_t pointer or NULL if not found
===========================================================================*/
cpr_margin_adjustment_t* cpr_find_margin_adjustment(cpr_corner_params_t* corner_params, cpr_adjustment_t adjustment_type)
{
  for(int i=0; i<corner_params->margin_adjustments_count; i++)
  {
    cpr_margin_adjustment_t* margin_adjustments = &corner_params->margin_adjustments[i];
    if(margin_adjustments->adjustment_type == adjustment_type)
    {
      return margin_adjustments;
    }
  }
  return NULL;  
}

/*===========================================================================
FUNCTION: cpr_closed_loop_alloc_node

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
static cpr_closed_loop_rail_t cpr_closed_loop_alloc_node(cpr_rail_id_t rail_id, uint32 voltage_level_count, uint32 margin_adjustments_count)
{
  // use calloc to zero memory before allocation.
  cpr_closed_loop_rail_t closed_loop_rail = (cpr_closed_loop_rail_t)calloc(1,sizeof(cpr_closed_loop_rail_s));
  CORE_VERIFY(closed_loop_rail);

  closed_loop_rail->next_link = cpr_closed_loop_rail_root;
  cpr_closed_loop_rail_root = closed_loop_rail;

  closed_loop_rail->rail_id = rail_id;
  closed_loop_rail->rail_config = cpr_closed_loop_rail_config(rail_id);
  closed_loop_rail->voltage_control_handle = cpr_image_alloc_voltage_control_handle(rail_id);
  closed_loop_rail->image_rail_state = cpr_image_alloc_image_rail_state();

  //alloc for target params
  closed_loop_rail->target_params_count = voltage_level_count;
  closed_loop_rail->target_params = (cpr_corner_params_t*)calloc(closed_loop_rail->target_params_count, sizeof(cpr_corner_params_t));
  CORE_VERIFY_PTR(closed_loop_rail->target_params);
  
  //allocate space for the margin adjustments that will be applied to this corner. 
  for(int i = 0; i<closed_loop_rail->target_params_count; i++)
  {
    cpr_corner_params_t* current_target_params = &closed_loop_rail->target_params[i];
    CORE_VERIFY_PTR(current_target_params);
    current_target_params->margin_adjustments_count = margin_adjustments_count;
    current_target_params->margin_adjustments = (cpr_margin_adjustment_t*)calloc(current_target_params->margin_adjustments_count, sizeof(cpr_margin_adjustment_t));
    CORE_VERIFY_PTR(current_target_params->margin_adjustments);
  }

  closed_loop_rail->pmic_step_size = cpr_config_pmic_step_size(rail_id);

  return closed_loop_rail;
}

/*===========================================================================
FUNCTION: cpr_closed_loop_find_node

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
static cpr_closed_loop_rail_t cpr_closed_loop_find_node(cpr_rail_id_t rail_id)
{
  cpr_closed_loop_rail_t closed_loop_rail = cpr_closed_loop_rail_root;
  while(closed_loop_rail)
  {
    if(closed_loop_rail->rail_id == rail_id)
    {
      break;
    }
    closed_loop_rail = closed_loop_rail->next_link;
  }
  return closed_loop_rail;
}

/*===========================================================================
FUNCTION: cpr_closed_loop_init_default_voltage_levels

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
void cpr_closed_loop_init_default_voltage_levels(cpr_closed_loop_rail_t closed_loop_rail,
                    const cpr_enablement_versioned_rail_config_t* rail_enablement_config)
{
  const cpr_rail_id_t rail_id = closed_loop_rail->rail_id;

  CPR_LOGGING(2, "cpr_closed_loop_init_default_voltage_levels: %s  Supported Levels: %d", cpr_logging_get_ascii_rail_id_name(rail_id), rail_enablement_config->supported_level_count);
  for(int i=0; i<rail_enablement_config->supported_level_count; i++)
  {
    const cpr_enablement_supported_level_t* level_info = &rail_enablement_config->supported_level[i];
    cpr_voltage_mode_t voltage_mode = level_info->voltage_mode;
    
    uint32 open_loop_voltage = cpr_config_open_loop_voltage(rail_id, voltage_mode);

    //Always start from the Open-Loop voltage.
    cpr_image_set_rail_mode_voltage(closed_loop_rail->voltage_control_handle, voltage_mode, open_loop_voltage);
  }
}

/*===========================================================================
FUNCTION: cpr_calc_effective_margin

DESCRIPTION: Loop through all the margin adjusments and recalculate the effective margin

RETURN VALUE:
===========================================================================*/
int32 cpr_calc_effective_margin(cpr_corner_params_t* corner_target_params)
{
  int32 effective_margin = 0;
  int32 aging_margin = 0;
  int32 thermal_margin = 0;
  int32 static_margin = 0;
  for(int i = 0; i < corner_target_params->margin_adjustments_count; i++)
  {
    cpr_margin_adjustment_t* current_margin_adjustment = &corner_target_params->margin_adjustments[i];

    //save values for logging
    if ((current_margin_adjustment->adjustment_type == CPR_MARGIN_ADJUSTMENT_STATIC) &&
        (current_margin_adjustment->enabled == TRUE)){
        static_margin = current_margin_adjustment->margin_adjustment_mv;
    }
    if ((current_margin_adjustment->adjustment_type == CPR_MARGIN_ADJUSTMENT_AGING) &&
        (current_margin_adjustment->enabled == TRUE)){
        aging_margin = current_margin_adjustment->margin_adjustment_mv;
    }
    if ((current_margin_adjustment->adjustment_type == CPR_MARGIN_ADJUSTMENT_THERMO) &&
        (current_margin_adjustment->enabled == TRUE)){
        thermal_margin = current_margin_adjustment->margin_adjustment_mv;
    }


    CORE_VERIFY_PTR(current_margin_adjustment);
    if(current_margin_adjustment->enabled == TRUE)
    {
      effective_margin += current_margin_adjustment->margin_adjustment_mv;
    }
  }
  CPR_LOGGING(5, "cpr_calc_effective_margin: (mode: %s)  (static: %d) + (age: %d) + (therm: %d) = (effective:%d)",
                 cpr_logging_get_ascii_corner_name(corner_target_params->voltage_mode),
                 static_margin,
                 aging_margin,
                 thermal_margin,
                 effective_margin);

  return effective_margin;
}


uint32 cpr_calc_aging(cpr_closed_loop_rail_t rail, uint32 aging_scaling_factor)
{
    // check if aging is disabled
    CORE_VERIFY_PTR(rail->rail_config);
    int32 aging_margin_limit = rail->rail_config->aging_margin_limit;
    
    // if aging is disabled return the aging margin limit
    if(rail->rail_config->aging_sensor_count == 0)
    {
        return aging_margin_limit;
    }
    
    // ( max_age_delta * 100 / aging_kv_ro_x100 )
    uint32 aging_margin_added = ( rail->max_aging_delta * 100) / rail->rail_config->aging_ro_kv_x100 ;
    aging_margin_added = aging_margin_added * aging_scaling_factor;
    
    // limit to aging_margin_limit
    aging_margin_added = MIN(rail->rail_config->aging_margin_limit, aging_margin_added);

    CPR_LOGGING(4, "(max aging delta * 100 = %d)   (aging_ro_kv_x100: %d)   (aging_margin_added: %d) (aging_margin_limit: %d)", (rail->max_aging_delta * 100), rail->rail_config->aging_ro_kv_x100, aging_margin_added, rail->rail_config->aging_margin_limit);

    return aging_margin_added;
}


/*===========================================================================
FUNCTION: cpr_enable_init_fn_closed_loop

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
void cpr_enable_init_fn_closed_loop(cpr_rail_id_t rail_id,
                                    const cpr_enablement_versioned_rail_config_t* rail_enablement_config,
                                    cpr_enablement_stage_t enablement_stage)
{
  cpr_closed_loop_rail_t closed_loop_rail = cpr_closed_loop_find_node(rail_id);

  if(enablement_stage == CPR_ENABLEMENT_STAGE_INIT_INITAL_VOLTAGE_CONFIG)
  {
    //Sanity check that we've not already alloced a node.
    CORE_VERIFY(!closed_loop_rail);
    closed_loop_rail = cpr_closed_loop_alloc_node(rail_id, rail_enablement_config->supported_level_count, rail_enablement_config->margin_adjustment_enablement_count);

    cpr_closed_loop_init_default_voltage_levels(closed_loop_rail, rail_enablement_config);
    return;
  }

  //Sanity check that we could find the rail node.
  CORE_VERIFY_PTR(closed_loop_rail);


  CPR_LOGGING(2, "cpr_enable_init_fn_closed_loop: %s  Supported Levels: %d", cpr_logging_get_ascii_rail_id_name(rail_id), rail_enablement_config->supported_level_count);
    
  //Init target quotients, floor and ceiling voltages.
  for(int i=0; i<rail_enablement_config->supported_level_count; i++)
  {
    const cpr_enablement_supported_level_t* level_info = &rail_enablement_config->supported_level[i];
    cpr_voltage_mode_t voltage_mode = level_info->voltage_mode;
    closed_loop_rail->target_params[i].voltage_mode = voltage_mode;
    cpr_corner_params_t* corner_target_params = &closed_loop_rail->target_params[i];
    
    CPR_LOGGING(2, "Configure closed_loop mode: %s  margin adjustment factors: %d", cpr_logging_get_ascii_corner_name(voltage_mode), corner_target_params->margin_adjustments_count);

    //for each supported corner, fill in pointers to the margin adjusters. 
    for(int j=0; j<corner_target_params->margin_adjustments_count; j++)
    {
      cpr_margin_adjustment_t* current_margin_adjustment = &corner_target_params->margin_adjustments[j];
      current_margin_adjustment->adjustment_type = rail_enablement_config->margin_adjustment_enablement[j].adjustment_type;
      current_margin_adjustment->enabled = rail_enablement_config->margin_adjustment_enablement[j].enabled;
    }
    
    // aggregate all margin adjustments
    
    // fill in static margin adjustment details, every corner on every chip will get this setting. 
    cpr_margin_adjustment_t* margin_adjustment_static = cpr_find_margin_adjustment(corner_target_params, CPR_MARGIN_ADJUSTMENT_STATIC);
    if (margin_adjustment_static != NULL){
	    if (level_info->custom_static_margin_fn)
        {  //if there's a custom static margin function use it. 
		    int32 orig_static_margin_mv = level_info->static_margin_mv;
            margin_adjustment_static->margin_adjustment_mv = level_info->custom_static_margin_fn (rail_id, voltage_mode, orig_static_margin_mv);
			CPR_LOGGING(3, "Custom static margin mode: %s  margin changed from %d to %d", cpr_logging_get_ascii_corner_name(voltage_mode), orig_static_margin_mv, margin_adjustment_static->margin_adjustment_mv);
		} else {
            margin_adjustment_static->margin_adjustment_mv = level_info->static_margin_mv;
        }
    }
    
    // fill in aging margin adjustment details
    cpr_margin_adjustment_t* margin_adjustment_aging = cpr_find_margin_adjustment(corner_target_params, CPR_MARGIN_ADJUSTMENT_AGING);
    if (margin_adjustment_aging != NULL){
      margin_adjustment_aging->margin_adjustment_mv = cpr_calc_aging(closed_loop_rail, level_info->aging_scaling_factor);
      cpr_logging_update_aging_stats(margin_adjustment_aging->margin_adjustment_mv, closed_loop_rail->rail_config->aging_margin_limit);
    } else {
        //sending 0's puts a "Aging Not Enabled" in the log
      cpr_logging_update_aging_stats(0, 0);
    }

    // fill in aging margin adjustment details
    cpr_margin_adjustment_t* margin_adjustment_thermal = cpr_find_margin_adjustment(corner_target_params, CPR_MARGIN_ADJUSTMENT_THERMO);
    if (margin_adjustment_thermal != NULL){
        margin_adjustment_thermal->margin_adjustment_mv = 0;  //start with no adjustment due to thermal. 
        cpr_logging_update_therm_stats("init", 0, 0);
    } else {
        //sending 0's puts a "Thermal Not Enabled" in the log
        cpr_logging_update_therm_stats(NULL, 0, 0);
    }

    
    // calculate the effective static margin
    corner_target_params->effective_margin_mv = cpr_calc_effective_margin(corner_target_params);
    
    // apply the static margin to the ro target values
    cpr_target_quotient_set_t* ro_targets = &corner_target_params->rosc_targets;
    cpr_target_quotient_calculate_quotients(rail_id, voltage_mode, corner_target_params->effective_margin_mv, ro_targets);
    

    // apply custom voltage function if there are any
    if(rail_enablement_config->custom_voltage_fn)
    {
      corner_target_params->voltage_ceil  = rail_enablement_config->custom_voltage_fn(CPR_OPEN_LOOP_VOLTAGE, voltage_mode);
      corner_target_params->voltage_floor = rail_enablement_config->custom_voltage_fn(CPR_CLOSED_LOOP_FLOOR_VOLTAGE, voltage_mode);
    }
    else
    {
      corner_target_params->voltage_ceil = cpr_config_open_loop_voltage(rail_id, voltage_mode);
      corner_target_params->voltage_floor = cpr_config_closed_loop_floor_voltage(rail_id, voltage_mode);
    }
  }
    
  //Now do what's remaining with the HW init.
#ifdef CPR_CONFIG_NO_FEEDBACK_LOOP

  if(rail_enablement_config->mode_to_settle_count>0)
  {
    cpr_init_rail_hw(closed_loop_rail);

    cpr_settle_voltage(closed_loop_rail, rail_enablement_config);
  }

#else

  cpr_init_rail_hw(closed_loop_rail);
  if(rail_enablement_config->mode_to_settle_count>0)
  {
    cpr_settle_voltage(closed_loop_rail, rail_enablement_config);
  }
  
  /*
  Commenting out the enabling. On the modem (MSS CPR)  CPR should not be enabled at init.  Wait until CPR enable is called later.
  cpr_enable_closed_loop_feedback(closed_loop_rail);
  */

#endif
}

/*===========================================================================
FUNCTION: cpr_closed_loop_init_default_voltages_from_smem

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
void cpr_closed_loop_init_default_voltages_from_smem(cpr_closed_loop_rail_t closed_loop_rail, const cpr_externalized_state_closed_loop_level_t* closed_loop_levels, uint32 closed_loop_level_count, uint32 margin_adjustment_enablement_count)
{
 // Not used on MSS CPR driver.
}

/*===========================================================================
FUNCTION: cpr_enable_init_fn_closed_loop_from_smem

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
void cpr_enable_init_fn_closed_loop_from_smem(cpr_rail_id_t rail_id, const cpr_externalized_state_closed_loop_level_t* closed_loop_levels, 
                                              uint32 closed_loop_level_count, uint32 margin_adjustment_enablement_count, cpr_enablement_stage_t enablement_stage)
{
 // Not used on MSS CPR driver.
}

/*===========================================================================
FUNCTION: rbcpr_prepare_for_sleep

DESCRIPTION: disables RBCPR clocks.

    FOR each rail that is enabled
        Disable CPR
        Turn off clocks

RETURN VALUE:
===========================================================================*/
void rbcpr_prepare_for_sleep(void)
{
  cpr_closed_loop_rail_t node = cpr_closed_loop_rail_root;

  while(node)
  {
    if(node->block_enabled)
    {
      cpr_pause_cpr(node);
    }
    node = node->next_link;
  }
}

/*===========================================================================
FUNCTION: rbcpr_exit_sleep

DESCRIPTION: enables RBCPR clocks.

    FOR each rail that was enabled
        Turn on clocks
        Enable CPR

RETURN VALUE:
===========================================================================*/
void rbcpr_exit_sleep(void)
{
  cpr_closed_loop_rail_t node = cpr_closed_loop_rail_root;

  while(node)
  {
    if(node->block_enabled)
    {
      uint32 current_voltage = cpr_image_get_rail_mode_voltage(node->voltage_control_handle, node->current_target_params->voltage_mode);
      cpr_prepare_cpr(node, node->current_target_params, current_voltage);
      cpr_restart_cpr(node);
    }
    node = node->next_link;
  }
}
