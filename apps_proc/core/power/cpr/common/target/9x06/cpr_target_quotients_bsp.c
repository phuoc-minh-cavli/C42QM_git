/*===========================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#include "cpr_target_quotients.h"
#include "cpr_device_hw_version.h"

static const cpr_target_quotient_versioned_config_t mss_9x45_vALL_target_quotients =
{
    .hw_versions =
    {
        .foundry_range = (const cpr_config_foundry_range[])
        {  //foundary        min version (inclusive)      max version (exclusive)       min CPR Fuse rev,  max CPR Fuse rev
            { CPR_FOUNDRY_TSMC, CPR_CHIPINFO_VERSION(0, 0), CPR_CHIPINFO_VERSION(0xFF, 0xFF), 0, 0xff },
        },
        .foundry_range_count = 1,
    },
    .target_quotient_level = (const cpr_target_quotient_level_t[])
    {
        //Mode,                      RO
        {CPR_VOLTAGE_MODE_SVS,       0},
        {CPR_VOLTAGE_MODE_NOMINAL,   0},      
        {CPR_VOLTAGE_MODE_TURBO,     0},
    },
    .target_quotient_level_count = 3,
    .ro_kv_x_100 = {175, 0, 197, 0, 0, 0, 0, 0},
};

static const cpr_target_quotient_rail_config_t mss_9x45_target_quotient_config =
{
    .rail_id = CPR_RAIL_APC0,
    .versioned_target_quotient_config = (const cpr_target_quotient_versioned_config_t*[])
    {
        &mss_9x45_vALL_target_quotients,
    },
    .versioned_target_quotient_config_count = 1,
};


const cpr_target_quotient_global_config_t cpr_bsp_target_quotient_config = 
{
    .rail_config = (const cpr_target_quotient_rail_config_t*[])
    {
        &mss_9x45_target_quotient_config,
    },
    .rail_config_count = 1,
};
