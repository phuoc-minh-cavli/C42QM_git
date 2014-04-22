/*===========================================================================

  Copyright (c) 2016-2017 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/
#include "cpr_enablement.h"
#include "cpr_device_hw_version.h"
#include "cpr_voltage_ranges.h"

////////////////////////////////////////////////
// APC rail CPR config
////////////////////////////////////////////////

static const cpr_enablement_versioned_rail_config_t apc_9x45_V0_versioned_cpr_enablement =
{
    .hw_versions =
    {
        .foundry_range = (const cpr_config_foundry_range[])
        {   //foundary        min version (inclusive)      max version (exclusive)       min CPR Fuse rev (inc),  max CPR Fuse rev(exl)
            { CPR_FOUNDRY_TSMC, CPR_CHIPINFO_VERSION(0, 0), CPR_CHIPINFO_VERSION(0xFF, 0xFF), 0, 0x1 },
        },
        .foundry_range_count = 1,
    },
    //.enablement_init_params = &CPR_ENABLE_GLOBAL_CEILING_VOLTAGE,
    .enablement_init_params = &CPR_ENABLE_OPEN_LOOP,
	//.enablement_init_params = &CPR_ENABLE_CLOSED_LOOP,
    .supported_level = (const cpr_enablement_supported_level_t[])
    {
        //Mode                Static-Margin (mV) Custom static margin function    aging_scaling_factor
        { CPR_VOLTAGE_MODE_SVS,           25,                  NULL,                  1},
        { CPR_VOLTAGE_MODE_NOMINAL,       25,                  NULL,                  1},
        { CPR_VOLTAGE_MODE_TURBO,         25,                  NULL,                  1},
    },
    .supported_level_count = 3,
    .margin_adjustment_enablement = (const cpr_margin_adjustment_enablement_t[])
    {
        //adjustment type            //enablement
        {CPR_MARGIN_ADJUSTMENT_STATIC, TRUE},
        {CPR_MARGIN_ADJUSTMENT_AGING,  FALSE},
        {CPR_MARGIN_ADJUSTMENT_THERMO, FALSE},
    },
    .margin_adjustment_enablement_count = 3,	  //three contributors to MSS CPR margins.
    .max_floor_to_ceiling_level_count = 0,        //max floor to ceilings aren't used in 9x45
    .custom_voltage_fn = NULL,         // NULL ==  Use ordinary floor and ceiling calculation functions.
    .mode_to_settle_count = 0,         // 0 == No init time CPR measurements
};

//V1
static const cpr_enablement_versioned_rail_config_t apc_9x45_V1_versioned_cpr_enablement =
{
    .hw_versions =
    {
        .foundry_range = (const cpr_config_foundry_range[])
        {   //foundary        min version (inclusive)      max version (exclusive)       min CPR Fuse rev (inc),  max CPR Fuse rev(exl)
            { CPR_FOUNDRY_TSMC, CPR_CHIPINFO_VERSION(0, 0), CPR_CHIPINFO_VERSION(0xFF, 0xFF), 0x1, 0x2 },
        },
        .foundry_range_count = 1,
    },
    //.enablement_init_params = &CPR_ENABLE_GLOBAL_CEILING_VOLTAGE,
    //.enablement_init_params = &CPR_ENABLE_OPEN_LOOP,
    .enablement_init_params = &CPR_ENABLE_CLOSED_LOOP,
    .supported_level = (const cpr_enablement_supported_level_t[])
    {
        //Mode                Static-Margin (mV) Custom static margin function    aging_scaling_factor
        { CPR_VOLTAGE_MODE_SVS,           25,                  NULL,                  1},
        { CPR_VOLTAGE_MODE_NOMINAL,       13,                 NULL,                  1},
        { CPR_VOLTAGE_MODE_TURBO,         25,                  NULL,                  1},
    },
    .supported_level_count = 3,
    .margin_adjustment_enablement = (const cpr_margin_adjustment_enablement_t[])
    {
        //adjustment type            //enablement
        {CPR_MARGIN_ADJUSTMENT_STATIC, TRUE},
        {CPR_MARGIN_ADJUSTMENT_AGING,  FALSE},
        {CPR_MARGIN_ADJUSTMENT_THERMO, FALSE},
    },
    .margin_adjustment_enablement_count = 3,	  //three contributors to MSS CPR margins.
    .max_floor_to_ceiling_level_count = 0,        //max floor to ceilings aren't used in 9x45
    .custom_voltage_fn = NULL,         // NULL ==  Use ordinary floor and ceiling calculation functions.
    .mode_to_settle_count = 0,         // 0 == No init time CPR measurements
};

static const cpr_enablement_versioned_rail_config_t apc_9x45_V2_versioned_cpr_enablement =
{
    .hw_versions =
    {
        .foundry_range = (const cpr_config_foundry_range[])
        {   //foundary        min version (inclusive)      max version (exclusive)       min CPR Fuse rev (inc),  max CPR Fuse rev(exl)
            { CPR_FOUNDRY_TSMC, CPR_CHIPINFO_VERSION(0, 0), CPR_CHIPINFO_VERSION(0xFF, 0xFF), 0x2, 0xFF },
        },
        .foundry_range_count = 1,
    },
    //.enablement_init_params = &CPR_ENABLE_GLOBAL_CEILING_VOLTAGE,
    //.enablement_init_params = &CPR_ENABLE_OPEN_LOOP,
    .enablement_init_params = &CPR_ENABLE_CLOSED_LOOP,
    .supported_level = (const cpr_enablement_supported_level_t[])
    {
        //Mode                Static-Margin (mV) Custom static margin function    aging_scaling_factor
        { CPR_VOLTAGE_MODE_SVS,           25,                  NULL,                  1},
        { CPR_VOLTAGE_MODE_NOMINAL,       25,                  NULL,                  1},
        { CPR_VOLTAGE_MODE_TURBO,         38,                  NULL,                  1},
    },
    .supported_level_count = 3,
    .margin_adjustment_enablement = (const cpr_margin_adjustment_enablement_t[])
    {
        //adjustment type            //enablement
        {CPR_MARGIN_ADJUSTMENT_STATIC, TRUE},
        {CPR_MARGIN_ADJUSTMENT_AGING,  FALSE},
        {CPR_MARGIN_ADJUSTMENT_THERMO, FALSE},
    },
    .margin_adjustment_enablement_count = 3,	  //three contributors to MSS CPR margins.
    .max_floor_to_ceiling_level_count = 0,        //max floor to ceilings aren't used in 9x45
    .custom_voltage_fn = NULL,         // NULL ==  Use ordinary floor and ceiling calculation functions.
    .mode_to_settle_count = 0,         // 0 == No init time CPR measurements
};

static const cpr_enablement_rail_config_t apc_9x45_cpr_enablement =
{
    .rail_id = CPR_RAIL_APC0,
    .versioned_rail_config = (const cpr_enablement_versioned_rail_config_t*[])
    {
        &apc_9x45_V0_versioned_cpr_enablement,
	&apc_9x45_V1_versioned_cpr_enablement,
	&apc_9x45_V2_versioned_cpr_enablement
    },
    .versioned_rail_config_count = 3,
};



/////////////////////////////////////////////////////////////////////////////////////
// CX config  used onlt for CX ELDO calculations.  CX CPR is operated on the RPM
///////////////////////////////////////////////////////////////////////////////////

static const cpr_enablement_versioned_rail_config_t cx_9x45_versioned_cpr_enablement =
{
    .hw_versions =
    {
        .foundry_range = (const cpr_config_foundry_range[])
        {    //foundary        min version (inclusive)      max version (exclusive)       min CPR Fuse rev (inc),  max CPR Fuse rev(exl)
            { CPR_FOUNDRY_TSMC, CPR_CHIPINFO_VERSION(0, 0), CPR_CHIPINFO_VERSION(0xFF, 0xFF), 0, 0xff },
        },
        .foundry_range_count = 1,
    },
    .enablement_init_params = &CPR_ENABLE_GLOBAL_CEILING_VOLTAGE,  //read the fuses, but we won't actually set any voltages.
    //.enablement_init_params = &CPR_ENABLE_GLOBAL_CEILING_VOLTAGE,
    .supported_level = (const cpr_enablement_supported_level_t[])
    {
        //Mode                Static-Margin (mV) Custom static margin function    aging_scaling_factor
        { CPR_VOLTAGE_MODE_SVS,     0,                NULL,                          1 },       //these are placeholders, the static margin is not actualy used
        { CPR_VOLTAGE_MODE_NOMINAL, 0,                NULL,                          1 },   //these are placeholders, the static margin is not actualy used
    },
    .supported_level_count = 2,
    .margin_adjustment_enablement_count = 0,
    .max_floor_to_ceiling_level_count = 0,        //max floor to ceilings aren't used in 9x45
    .custom_voltage_fn = NULL,  // NULL ==  Use ordinary floor and ceiling calculation functions.
    .mode_to_settle_count = 0,  // 0 == No init time CPR measurements

};


// CX config  used onlt for CX ELDO calculations.  CX CPR is operated on the RPM
static const cpr_enablement_rail_config_t cx_9x45_cpr_enablement =
{
    .rail_id = CPR_RAIL_CX,
    .versioned_rail_config = (const cpr_enablement_versioned_rail_config_t*[])
    {
        &cx_9x45_versioned_cpr_enablement,
    },
    .versioned_rail_config_count = 1,
};


///////////////////////////////////////////////////////////////////
// cpr_bsp_enablement_config - All the rails are listed here.
//////////////////////////////////////////////////////////////////
const cpr_enablement_config_t cpr_bsp_enablement_config =
{
    .rail_enablement_config = (const cpr_enablement_rail_config_t*[])
    {
        &apc_9x45_cpr_enablement,
        &cx_9x45_cpr_enablement,
    },
    .rail_enablement_config_count = 2,
};
