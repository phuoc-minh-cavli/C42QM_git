/*===========================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#include "cpr_enablement.h"
#include "cpr_logging.h"
#include "CoreVerify.h"

#define CPR_CONFIG_EFUSE_MULTIPLIER_FLOOR_TO_CEILING 10000   //10mV

static const cpr_enablement_versioned_rail_config_t* cpr_enablement_find_device_config(const cpr_enablement_rail_config_t* enablement_config)
{
    for(int i=0; i<enablement_config->versioned_rail_config_count; i++)
    {
        const cpr_enablement_versioned_rail_config_t* versioned_enablement_config = enablement_config->versioned_rail_config[i];
        const cpr_config_hw_version_range* hw_versions = &versioned_enablement_config->hw_versions;
        //CPR_LOGGING(1, "cpr_enablement_find_device_config for rail: %s  ", cpr_logging_get_ascii_rail_id_name(enablement_config->rail_id));
        if(cpr_device_hw_version_matches_this_device(hw_versions))
        {
                return versioned_enablement_config;
        }
    }   
    return NULL;
}

uint32 cpr_enablement_number_of_rails(void)
{
    return cpr_bsp_enablement_config.rail_enablement_config_count;
}

void cpr_enablement_rail_info(uint32 index, cpr_rail_id_t* rail_id, const cpr_enablement_versioned_rail_config_t** rail_enablement_versioned_config)
{
    CORE_VERIFY(index<cpr_bsp_enablement_config.rail_enablement_config_count);
   
    const cpr_enablement_rail_config_t* rail_enablement_config = cpr_bsp_enablement_config.rail_enablement_config[index];
    *rail_id = rail_enablement_config->rail_id;
    *rail_enablement_versioned_config = cpr_enablement_find_device_config(rail_enablement_config);
}

const cpr_enablement_versioned_rail_config_t* cpr_enablement_find_versioned_rail_config(cpr_rail_id_t cpr_rail_id)
{
    for(int i = 0; i < cpr_bsp_enablement_config.rail_enablement_config_count; i++)
    {
        if(cpr_bsp_enablement_config.rail_enablement_config[i]->rail_id == cpr_rail_id)
        {
            return cpr_enablement_find_device_config(cpr_bsp_enablement_config.rail_enablement_config[i]);
        }
    }
    return NULL;
}

/**
 * 
 * <!-- cpr_config_max_floor_to_ceiling -->
 *
 * @breif For a given rail and voltage mode return the max floor to ceiling value.  
 *
 * @param rail_id : usually CPR_RAIL_APC0 for the initial implementation.
 * @param voltage_mode : CPR_VOLTAGE_MODE_SVS, CPR_VOLTAGE_MODE_NOMINAL, etc...
 *
 * @return returns the range in uV or 0 if there was no information avaliable. 
 */
uint32 cpr_config_max_floor_to_ceiling(cpr_rail_id_t rail_id, cpr_voltage_mode_t voltage_mode){
    int i;
    uint32 calculated_max_floor_to_ceiling;
    const cpr_enablement_versioned_rail_config_t* rail_enablement_for_this_device = cpr_enablement_find_versioned_rail_config(rail_id);

    if (rail_enablement_for_this_device == NULL){
        return 0;
    }

    for (i = 0; i < rail_enablement_for_this_device->max_floor_to_ceiling_level_count; i++){
        //find the voltage mode in the list
        if (rail_enablement_for_this_device->max_floor_to_ceilings[i].voltage_mode == voltage_mode){
            //sometimes the max floor to ceiling gets modified, for now we're just using the stored config value. 
            calculated_max_floor_to_ceiling = rail_enablement_for_this_device->max_floor_to_ceilings[i].floor_to_ceiling_mv;
            CPR_LOGGING(2, "cpr_config_max_floor_to_ceiling mode %s (value: %d)", cpr_logging_get_ascii_corner_name(voltage_mode), calculated_max_floor_to_ceiling);
            return calculated_max_floor_to_ceiling;
        }
    }
    //info not found, return 0
    return 0;
}

