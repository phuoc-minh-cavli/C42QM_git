/*===========================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/
#include "cpr_voltage_ranges.h"
#include "cpr_device_hw_version.h"

// --------------------------------------------------------------------------------------------------
// APC Voltage Ranges
// --------------------------------------------------------------------------------------------------
//   V1 
static const cpr_config_versioned_voltage_ranges_t v1_TSMC_9x45_apc_voltage_ranges =
{
    .hw_versions =
    {
        .foundry_range = (const cpr_config_foundry_range[])
        {  //foundary        min version (inclusive)      max version (exclusive)       min CPR Fuse rev,  max CPR Fuse rev
            { CPR_FOUNDRY_TSMC, CPR_CHIPINFO_VERSION(0, 0), CPR_CHIPINFO_VERSION(0xFF, 0xFF), 0, 0xFF },
        },
        .foundry_range_count = 1,
    },
    .voltage_level = (const cpr_config_voltage_level_t[])
    {
      //Mode,                          Ceiling,  Floor, Fuse-Ref, Corr-factor,         Fuse-Type
        {CPR_VOLTAGE_MODE_SVS,         1050000, 1050000, 1050000,    0,            CPR_FUSE_SVS,          CPR_FUSE_RO_SEL_SVS,      CPR_FUSE_LOCAL_QUOT_SVS     },
        {CPR_VOLTAGE_MODE_NOMINAL,     1225000, 1050000, 1225000,    0,            CPR_FUSE_NOMINAL,      CPR_FUSE_RO_SEL_NOMINAL,  CPR_FUSE_LOCAL_QUOT_NOMINAL },
        {CPR_VOLTAGE_MODE_TURBO,       1350000, 1150000, 1350000,    0,            CPR_FUSE_TURBO,        CPR_FUSE_RO_SEL_TURBO,    CPR_FUSE_LOCAL_QUOT_TURBO   },
    },
    .voltage_level_count = 3,
};


static const cpr_config_rail_voltage_ranges_t apc_9x45_voltage_ranges =
{
    .rail_id = CPR_RAIL_APC0,
    .versioned_voltage_range = (const cpr_config_versioned_voltage_ranges_t*[])
    {
        &v1_TSMC_9x45_apc_voltage_ranges,
    },
    .disable_fused_floor = false,
    .versioned_voltage_range_count = 1,
};


// --------------------------------------------------------------------------------------------------
// CX Voltage Ranges
//
// This is only for the SVS and NOM fuse reads for the eLDO calculations
// CX CPR is done in the RPM
//-----------------------------------------------------------------------------
static const cpr_config_versioned_voltage_ranges_t vALL_TSMC_9x45_cx_voltage_ranges =
{
    .hw_versions =
    {
        .foundry_range = (const cpr_config_foundry_range[])
        {   //foundary        min version (inclusive)      max version (exclusive)       min CPR Fuse rev,  max CPR Fuse rev
            { CPR_FOUNDRY_TSMC, CPR_CHIPINFO_VERSION(0, 0), CPR_CHIPINFO_VERSION(0xff, 0xff), 0, 0xff },
        },
        .foundry_range_count = 1,
    },
    .voltage_level = (const cpr_config_voltage_level_t[])
    {
      //Mode,                              Ceiling,  Floor, Fuse-Ref, Corr-factor,         Fuse-Type
        {CPR_VOLTAGE_MODE_SVS,             800000,   700000,  800000,       0,             CPR_FUSE_SVS,         CPR_FUSE_NO_FUSE,      CPR_FUSE_NO_FUSE  },
        {CPR_VOLTAGE_MODE_NOMINAL,         900000,   725000,  900000,       0,             CPR_FUSE_NOMINAL,     CPR_FUSE_NO_FUSE,      CPR_FUSE_NO_FUSE  },
        {CPR_VOLTAGE_MODE_TURBO,           1000000,  875000,  1000000,      0,             CPR_FUSE_TURBO,       CPR_FUSE_NO_FUSE,      CPR_FUSE_NO_FUSE  },
    },
    .voltage_level_count = 3,
};


static const cpr_config_rail_voltage_ranges_t cx_9x45_voltage_ranges =
{
    .rail_id = CPR_RAIL_CX,
    .versioned_voltage_range = (const cpr_config_versioned_voltage_ranges_t*[])
    {
        &vALL_TSMC_9x45_cx_voltage_ranges,
    },
    .disable_fused_floor = false,
    .versioned_voltage_range_count = 1,
};




const cpr_config_global_voltage_ranges_t cpr_bsp_voltage_ranges_config =
{
    .rail_voltage_ranges = (const cpr_config_rail_voltage_ranges_t*[])
    {
        &apc_9x45_voltage_ranges,
        &cx_9x45_voltage_ranges,
    },
    .rail_voltage_ranges_count = 2,
};


const cpr_config_misc_info_t cpr_bsp_misc_info_config =
{
    .rail_info = (const cpr_config_misc_rail_info_t[])
    {
        //Rail,             PMIC Step-Size
        {CPR_RAIL_APC0,               12500},      
        {CPR_RAIL_CX,                 12500},      //we only use the fuse values currently, this is a placeholder. 
    },
    .rail_info_count = 2,
};
