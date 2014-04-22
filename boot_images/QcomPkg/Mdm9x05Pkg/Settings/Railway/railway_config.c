/*
===========================================================================

FILE:         railway_config.c

DESCRIPTION:
  Per target railway configurations

===========================================================================

                             Edit History

$Date: 2019/04/23 $

when       who     what, where, why
--------   ---     --------------------------------------------------------

===========================================================================
Copyright (c) 2013, 2019 by Qualcomm Technologies, Inc. All Rights Reserved.
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include "railway_config.h"
#include "pm_ldo.h"
#include "pm_smps.h"
#include "pm_version.h" 

/* -----------------------------------------------------------------------
**                           DATA
** ----------------------------------------------------------------------- */

//
// BEGIN config data; should migrate to the system enumeration data method
//
static const railway_config_data_t temp_config_data =
{
    .rails     = (railway_rail_config_t[])
    {
        //Must init Mx first, as voting on the other rails will cause Mx changes to occur.
        {
            .vreg_name = "vddmx",

            // SMPS1
            .pmic_rail_type = RAILWAY_LDO_TYPE,
            .pmic_chip_id   = 0,
            .pmic_peripheral_index = PM_LDO_4,


            .initial_corner = RAILWAY_NO_REQUEST,
            .default_retention_uv = 752000,
        },

		// VDDCX
        {
            .vreg_name = "vddcx",

            // LDO1
            .pmic_rail_type = RAILWAY_LDO_TYPE,
            .pmic_chip_id   = 0,
            .pmic_peripheral_index = PM_LDO_1,

            .initial_corner = RAILWAY_NO_REQUEST,
            .default_retention_uv = 752000,
        },
    },

    .num_rails = 2,
};
//
// END config data
//

const railway_config_data_t * const RAILWAY_CONFIG_DATA = &temp_config_data;

