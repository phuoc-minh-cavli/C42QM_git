/*===========================================================================

  Copyright (c) 2014, 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#ifndef CPR_ENABLEMENT_H
#define CPR_ENABLEMENT_H

#include "comdef.h"
#include "cpr_defs.h"
#include "cpr_device_hw_version.h"
#include "cpr_fuses.h"

typedef enum
{
    CPR_OPEN_LOOP_VOLTAGE,          //This doubles as the Closed-Loop Ceiling also.
    CPR_CLOSED_LOOP_FLOOR_VOLTAGE,
} cpr_enablement_custom_voltage_calculation_type_t;

typedef uint32 (*cpr_custom_voltage_calculation_fn)(cpr_enablement_custom_voltage_calculation_type_t voltage_calculation_type, cpr_voltage_mode_t voltage_mode);

typedef enum
{
    CPR_ENABLEMENT_OPEN_LOOP_ID,
    CPR_ENABLEMENT_CLOSED_LOOP_ID,
    CPR_ENABLEMENT_GLOBAL_CEILING_VOLTAGE_ID,
    CPR_ENABLEMENT_READ_CONFIG_FROM_SMEM_ID,
    CPR_ENABLEMENT_HV_PARTS_ID,                //8996 only
} cpr_enablement_type_t;

typedef enum
{
    CPR_ENABLEMENT_STAGE_INIT_INITAL_VOLTAGE_CONFIG,
    CPR_ENABLEMENT_STAGE_INIT_CLOSED_LOOP,
} cpr_enablement_stage_t;

struct cpr_enablement_versioned_rail_config_t;
typedef void (*cpr_enablement_fn_t)(cpr_rail_id_t rail_id, const struct cpr_enablement_versioned_rail_config_t* rail_enablement_config, cpr_enablement_stage_t enablement_stage);
typedef int32 (*cpr_enablement_custom_static_margin_fn_t)(cpr_rail_id_t rail_id, cpr_voltage_mode_t voltage_mode,
                                                          int32 static_margin_mv);

typedef struct
{
    cpr_enablement_type_t   enablement_type;
    cpr_enablement_fn_t     enablement_fn;
} cpr_enablement_init_params_t;

//Standard enablement configs available for all rails, all targets.
//Not all rails can support Closed-Loop, but all rails should support any of the Open-Loop configuration options.
extern const cpr_enablement_init_params_t CPR_ENABLE_OPEN_LOOP;
extern const cpr_enablement_init_params_t CPR_ENABLE_CLOSED_LOOP;
extern const cpr_enablement_init_params_t CPR_ENABLE_GLOBAL_CEILING_VOLTAGE; //This will enable the voltage at the SS voltage.
extern const cpr_enablement_init_params_t CPR_ENABLE_READ_CONFIG_FROM_SMEM;



typedef struct
{
  cpr_voltage_mode_t  voltage_mode;
  int32               static_margin_mv;
  cpr_enablement_custom_static_margin_fn_t custom_static_margin_fn;
  int32               aging_scaling_factor;
} cpr_enablement_supported_level_t;


/**
 * 7/21/15 CPR floor calculations are evolving to be defined as a range value for each corner.  
 * Examples and explination: 
 * A) The value will be defined as a constant initially. For example, SVS might have a max floor to ceiling of 200mV. 
 *    So a part with an openloop ceiling value of 800mV would have a floor voltage of 600mV.
 *
 * B) Later the floor to ceiling voltage will be defined by a value and an added fuse value. So our SVS example might have 
 *    a max floor to ceiling voltage of 160mV + (an SVS floor to ceiling fuse value) 
 *
 * This struct holds a mV value and a fuse value. These values will be combined to calculate the max floor to ceiling amount.  
 * CPR_FUSE_NO_FUSE returns 0 so the same equasion can be used for the A and B examples listed above. 
 */
typedef struct 
{
    cpr_voltage_mode_t  voltage_mode;
    int32               floor_to_ceiling_mv;  
} cpr_config_max_floor_to_ceiling_level_t;


/**
 * There are numerous variable contributions that sum together to determine a mode's static margin.
 */
typedef struct
{
    cpr_adjustment_t adjustment_type;
    boolean          enabled;
} cpr_margin_adjustment_enablement_t;

typedef struct cpr_enablement_versioned_rail_config_t
{
    cpr_config_hw_version_range                     hw_versions;
    const cpr_enablement_init_params_t*             enablement_init_params;
    const cpr_enablement_supported_level_t*         supported_level;
    uint32                                          supported_level_count;
    const cpr_config_max_floor_to_ceiling_level_t*  max_floor_to_ceilings;
    uint32                                          max_floor_to_ceiling_level_count;
    const cpr_margin_adjustment_enablement_t*       margin_adjustment_enablement;  //array of margin adjustments that get applied to each corner
    uint32                                          margin_adjustment_enablement_count;
    cpr_custom_voltage_calculation_fn               custom_voltage_fn;        //Typically will be NULL.
    const cpr_voltage_mode_t*                       mode_to_settle;
    uint32                                          mode_to_settle_count;
} cpr_enablement_versioned_rail_config_t;

typedef struct
{
    cpr_rail_id_t                                   rail_id;
    const cpr_enablement_versioned_rail_config_t**  versioned_rail_config;
    uint32                                          versioned_rail_config_count;
} cpr_enablement_rail_config_t;

typedef struct
{
    const cpr_enablement_rail_config_t**    rail_enablement_config;
    uint32                                  rail_enablement_config_count;
} cpr_enablement_config_t;

//All BSPs must supply an instance of a cpr_bsp_enablement_config
extern const cpr_enablement_config_t cpr_bsp_enablement_config;

uint32 cpr_enablement_number_of_rails(void);

//Index must be between 0 and cpr_enablement_number_of_rails-1
//rail_enablement_config can be NULL if the index doesn't have a valid CPR enablement setting for this device.
void cpr_enablement_rail_info(uint32 index, cpr_rail_id_t* rail_id, const cpr_enablement_versioned_rail_config_t** rail_enablement_config);
const cpr_enablement_versioned_rail_config_t* cpr_enablement_find_versioned_rail_config(cpr_rail_id_t cpr_rail_id);
uint32 cpr_config_max_floor_to_ceiling(cpr_rail_id_t rail_id, cpr_voltage_mode_t voltage_mode);

#endif //CPR_ENABLEMENT_H
