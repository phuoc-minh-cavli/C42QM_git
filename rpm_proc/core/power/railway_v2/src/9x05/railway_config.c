/*
===========================================================================

FILE:         railway_config.c

DESCRIPTION:
  Per target railway configurations

===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.3/core/power/railway_v2/src/9x05/railway_config.c#4 $
$Date: 2019/02/08 $

when       who     what, where, why
--------   ---     --------------------------------------------------------

===========================================================================
             Copyright (c) 2012 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
              QUALCOMM Proprietary and Confidential
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include "CoreVerify.h"
#include "kvp.h"
#include "msmhwio.h"
#include "rpmserver.h"
#include "railway_config.h"
#include "pm_version.h"
#include "railway_adapter.h"
#include "PlatformInfo.h"
#include "Chipinfo.h"

/* -----------------------------------------------------------------------
**                           DATA
** ----------------------------------------------------------------------- */

#if 0 /* TODO: Delete this when no need to support 8997 RUMI */
#include "pm_npa_irep.h"

extern struct ResourceData *rpmserver_lookup_resource(rpm_resource_type resource, unsigned id);

void railway_dummy_pmic_xlate(rpm_translation_info *info) { }
void railway_dummy_pmic_apply(rpm_application_info *info) { }
#endif

//
// BEGIN config data; should migrate to the system enumeration data method
//
static const railway_config_data_t temp_config_data =
{
    .rails     = (railway_rail_config_t[])
    {
        // Must init VDDMX first, as voting on the other rails will cause Mx changes to occur.
        {
             .rail_type      = RAILWAY_RAIL_TYPE_MX,
            .vreg_name      = "vddmx",

            .vreg_type      = RPM_LDO_A_REQ,
            .vreg_num       = 4,

            .pmic_step_size = 12500,    // not used

            .initial_corner = RAILWAY_SUPER_TURBO,

            .supports_explicit_voltage_requests = true,
            .default_uvs = (const unsigned[])
            {
                0,                      // RAILWAY_NO_REQUEST
                800000,                 // RAILWAY_RETENTION
                1050000,                // RAILWAY_SVS_KRAIT
                1050000,                // RAILWAY_SVS_SOC
				1225000,                // RAILWAY_SVS_HIGH
                1225000,                // RAILWAY_NOMINAL
                1350000,                // RAILWAY_NOMINAL_HIGH
                1350000,                // RAILWAY_TURBO
                1350000,                // RAILWAY_SUPER_TURBO
				//1350000,                // RAILWAY_TURBO_HIGH
                1350000,                // RAILWAY_SUPER_TURBO_NO_CPR
            },

            .supported_corners = (railway_corner[])
            {
                RAILWAY_RETENTION,
                RAILWAY_SVS_SOC,
				RAILWAY_NOMINAL,
				RAILWAY_SUPER_TURBO,
				RAILWAY_SUPER_TURBO_NO_CPR,
            },
            .supported_corners_count = 5,
        },

        // VDDCX
         {
            .rail_type      = RAILWAY_RAIL_TYPE_LOGIC,
            .vreg_name      = "vddcx",

            .vreg_type      = RPM_LDO_A_REQ,
            .vreg_num       = 1,

            .pmic_step_size = 12500,     // not used

            .initial_corner = RAILWAY_SUPER_TURBO,

            .supports_explicit_voltage_requests = true,

            .default_uvs = (const unsigned[])
            {
                0,                      // RAILWAY_NO_REQUEST
                750000,                 // RAILWAY_RETENTION
                1050000,                // RAILWAY_SVS_KRAIT
                1050000,                // RAILWAY_SVS_SOC
				1225000,                // RAILWAY_SVS_HIGH
                1225000,                // RAILWAY_NOMINAL
                1350000,                // RAILWAY_NOMINAL_HIGH
                1350000,                // RAILWAY_TURBO
                1350000,                // RAILWAY_SUPER_TURBO
				//1350000,                // RAILWAY_TURBO_HIGH
                1350000,                // RAILWAY_SUPER_TURBO_NO_CPR
            },

            .supported_corners = (railway_corner[])
            {
                RAILWAY_RETENTION,
                RAILWAY_SVS_SOC,
				RAILWAY_NOMINAL,
				RAILWAY_SUPER_TURBO,
				RAILWAY_SUPER_TURBO_NO_CPR,
            },
            .supported_corners_count = 5,
        },
    },

    .num_rails = 2,
};
//
// END config data
//

const railway_config_data_t * const RAILWAY_CONFIG_DATA = &temp_config_data;
uint32 railway_target_type;

/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */
extern void railway_set_pmic_voltage(int rail_num, unsigned microvolts);
// TODO: This can be avoided if railway_init() is after pm_init()
// TODO: Make rail_state.corner_uvs a copy of rail_config.default_uvs,
//       instead of a pointer to rail_config.default_uvs
void railway_init_early_proxy_votes(void)
{
  int i;

    // Force vote to pmic for initial voltages
   
    for (i = 0; i < RAILWAY_CONFIG_DATA->num_rails; i++)
    {
        railway_rail_config_t *rail_cfg   = &(RAILWAY_CONFIG_DATA->rails[i]);
        unsigned               initial_uv = rail_cfg->default_uvs[rail_cfg->initial_corner];
       railway_rail_state_t  *rail_state = &(railway.rail_state[i]);

        railway_set_pmic_voltage(i, initial_uv);

        rail_state->current_active.mode       = rail_cfg->initial_corner;
        rail_state->current_active.microvolts = initial_uv;
    }
}

void railway_init_proxies_and_pins(void)
{
//Enable when you want to peg CX/MX to turbo
#if 0   
	  const int cx_rail_id = rail_id("vddcx");

	assert(RAIL_NOT_SUPPORTED_BY_RAILWAY != cx_rail_id);

	railway_voter_t cx_pin = railway_create_voter(cx_rail_id, true, RAILWAY_RPM_INIT_VOTER);

	railway_corner_vote(cx_pin, RAILWAY_SUPER_TURBO);
   #endif
}


void railway_target_init(void)
{

#if 0
		/* TODO: Delete this when pm_init is registering for Resources
         * PMIC driver does not create PMIC resources if it cannot detect PMIC HW on Virtio/RUMI.
         * Create dummy resources with dummy callback to make Railway driver happly.
         */
        if(!rpmserver_lookup_resource(RPM_LDO_A_REQ, 4))
        {
            rpm_register_resource(RPM_LDO_A_REQ, 4+1, sizeof(pm_npa_smps_int_rep), railway_dummy_pmic_xlate, railway_dummy_pmic_apply, NULL);
        }
        if(!rpmserver_lookup_resource(RPM_LDO_A_REQ, 1))
        {
            rpm_register_resource(RPM_LDO_A_REQ, 4+1, sizeof(pm_npa_smps_int_rep), railway_dummy_pmic_xlate, railway_dummy_pmic_apply, NULL);
        }
#endif
	
}

void railway_remap_target_specific_corners(rpm_translation_info *info, railway_voter_irep *req)
{
   
}
