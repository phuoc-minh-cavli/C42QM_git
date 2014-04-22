/*===========================================================================
  	
  	FILE:         rbcpr_bsp.c


	DESCRIPTION:  Per target rbcpr bsp configurations
  	
=============================================================================
	
Edit History

$Header: //components/rel/rpm.bf/2.1.3/core/power/rbcpr/src/target/9x05/rbcpr_bsp.c#5 $
$Date: 2019/06/25 $
	 
when       who     what, where, why
--------   ---     --------------------------------------------------------
  
===========================================================================
Copyright (c) 2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary and Confidential.
===========================================================================*/

 /* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include "rbcpr.h"
#include "rbcpr_comdef.h"
#include "rbcpr_qfprom.h"
#include "railway.h"
#include "Chipinfo.h"
#include "CoreVerify.h"
#include "rbcpr_stats.h"

#define SS_THRESHOLD_OPENLOOP_VOL  1187500
static bool rbcpr_ss_vol_threshold = false;
void rbcpr_bsp_check_ss_part(void);
extern int    cx_id;

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------
void rbcpr_pin_voltage_to_max_of_cx_apc(rbcpr_corner_params_type* corner_params, struct rbcpr_bsp_rail_info* rail, int);
void rbcpr_pin_mx_voltage_to_global_ceiling(rbcpr_corner_params_type* mx_corner_params, struct rbcpr_bsp_rail_info* mx_rail, int);
 /* -----------------------------------------------------------------------
**                     		DATA
** ----------------------------------------------------------------------- */
rbcpr_bsp_rail_params_type tsmc_cpr_settings[] =
{
     {   //VddMx
        .target_params = (rbcpr_corner_params_and_fuse_type[])
            {
                {
                .corner_params =
                    {
                    .corner         = RAILWAY_SVS_SOC,
                    .fuse           = RBCPR_FUSE_SVS,
                    .voltage_ceil   = 1050000,
                    .voltage_floor  = 1050000,
					.open_loop_adjust = 0,
                    },
                },
                {
                .corner_params =
                    {
                    .corner         = RAILWAY_NOMINAL,
                    .fuse           = RBCPR_FUSE_NOMINAL,
                    .voltage_ceil   = 1225000,
                    .voltage_floor  = 1050000,
					.open_loop_adjust = 0,
                    },
                },
                {
                .corner_params =
                    {
                    .corner         = RAILWAY_SUPER_TURBO,
                    .fuse           = RBCPR_FUSE_TURBO,
                    .voltage_ceil   = 1350000, //not actual ceiling, this is fuse reference, ceiling is 1325mv
                    .voltage_floor  = 1162500,
					.open_loop_adjust = 0,
                    },
                },
				{
                .corner_params =
                    {
                    .corner         = RAILWAY_SUPER_TURBO_NO_CPR,
                    .fuse           = RBCPR_FUSE_TURBO,
                    .voltage_ceil   = 1350000, //not actual ceiling, this is fuse reference, ceiling is 1325mv
                    .voltage_floor  = 1325000,
					.open_loop_adjust = 0,
                    },
                },
            },
        .rbcpr_enablement=RBCPR_ENABLED_OPEN_LOOP,
        .number_of_target_params=4,
     },
	
     {   // VDDCX
        .target_params = (rbcpr_corner_params_and_fuse_type[])
            {
                {
                .corner_params =
                     {
                        .corner         = RAILWAY_SVS_SOC,
                        .fuse           = RBCPR_FUSE_SVS,
                        .voltage_ceil   = 1050000,
                        .voltage_floor  = 900000,
						.open_loop_adjust = 0,
                    },
                },
                {
                .corner_params =
                    {
                        .corner         = RAILWAY_NOMINAL,
                        .fuse           = RBCPR_FUSE_NOMINAL,
                        .voltage_ceil   = 1225000,
                        .voltage_floor  = 1050000,
						.open_loop_adjust = 0,
                    },
                },
                {
                .corner_params =
                    {
                        .corner         = RAILWAY_SUPER_TURBO,
                        .fuse           = RBCPR_FUSE_TURBO,
                        .voltage_ceil   = 1350000, //not actual ceiling, this is fuse reference, ceiling is 1325mv
                        .voltage_floor  = 1162500,
						.open_loop_adjust = 0,
                    },
                },
				{
                .corner_params =
                    {
                        .corner         = RAILWAY_SUPER_TURBO_NO_CPR,
                        .fuse           = RBCPR_FUSE_TURBO,
                        .voltage_ceil   = 1350000, //not actual ceiling, this is fuse reference, ceiling is 1325mv
                        .voltage_floor  = 1325000,
						.open_loop_adjust = 0,
                    },
                },
            },
        .rbcpr_enablement=RBCPR_ENABLED_OPEN_LOOP,
        .number_of_target_params=4,
		
    },
};

rbcpr_bsp_rail_params_type gf_cpr_settings_rev1[] =
{
     {   //VddMx
        .target_params = (rbcpr_corner_params_and_fuse_type[])
            {
                {
                .corner_params =
                    {
                    .corner         = RAILWAY_SVS_SOC,
                    .fuse           = RBCPR_FUSE_SVS,
                    .voltage_ceil   = 1050000,
                    .voltage_floor  = 1050000,
					.open_loop_adjust = 0,
                    },
                },
                {
                .corner_params =
                    {
                    .corner         = RAILWAY_NOMINAL,
                    .fuse           = RBCPR_FUSE_NOMINAL,
                    .voltage_ceil   = 1225000,
                    .voltage_floor  = 1050000,
					.open_loop_adjust = -20000,
                    },
                },
                {
                .corner_params =
                    {
                    .corner         = RAILWAY_SUPER_TURBO,
                    .fuse           = RBCPR_FUSE_TURBO,
                    .voltage_ceil   = 1350000, //not actual ceiling, this is fuse reference, ceiling is 1325mv
                    .voltage_floor  = 1162500,
					.open_loop_adjust = -40000,
                    },
                },
				{
                .corner_params =
                    {
                    .corner         = RAILWAY_SUPER_TURBO_NO_CPR,
                    .fuse           = RBCPR_FUSE_TURBO,
                    .voltage_ceil   = 1350000, //not actual ceiling, this is fuse reference, ceiling is 1325mv
                    .voltage_floor  = 1325000,
					.open_loop_adjust = 0,
                    },
                },
            },
        .rbcpr_enablement=RBCPR_ENABLED_OPEN_LOOP,
        .number_of_target_params=4,
     },
	
     {   // VDDCX
        .target_params = (rbcpr_corner_params_and_fuse_type[])
            {
                {
                .corner_params =
                     {
                        .corner         = RAILWAY_SVS_SOC,
                        .fuse           = RBCPR_FUSE_SVS,
                        .voltage_ceil   = 1050000,
                        .voltage_floor  = 900000,
						.open_loop_adjust = 0,
                    },
                },
                {
                .corner_params =
                    {
                        .corner         = RAILWAY_NOMINAL,
                        .fuse           = RBCPR_FUSE_NOMINAL,
                        .voltage_ceil   = 1225000,
                        .voltage_floor  = 1050000,
						.open_loop_adjust = -25000,
                    },
                },
                {
                .corner_params =
                    {
                        .corner         = RAILWAY_SUPER_TURBO,
                        .fuse           = RBCPR_FUSE_TURBO,
                        .voltage_ceil   = 1350000, //not actual ceiling, this is fuse reference, ceiling is 1325mv
                        .voltage_floor  = 1162500,
						.open_loop_adjust = -40000,
                    },
                },
				{
                .corner_params =
                    {
                        .corner         = RAILWAY_SUPER_TURBO_NO_CPR,
                        .fuse           = RBCPR_FUSE_TURBO,
                        .voltage_ceil   = 1350000, //not actual ceiling, this is fuse reference, ceiling is 1325mv
                        .voltage_floor  = 1325000,
						.open_loop_adjust = 0,
                    },
                },
            },
        .rbcpr_enablement=RBCPR_ENABLED_OPEN_LOOP,
        .number_of_target_params=4,
		
    },
};

rbcpr_bsp_rail_params_type smic_cpr_settings_rev1[] =
{
     {   //VddMx
        .target_params = (rbcpr_corner_params_and_fuse_type[])
            {
                {
                .corner_params =
                    {
                    .corner         = RAILWAY_SVS_SOC,
                    .fuse           = RBCPR_FUSE_SVS,
                    .voltage_ceil   = 1050000,
                    .voltage_floor  = 1050000,
					.open_loop_adjust = 0,
                    },
                },
                {
                .corner_params =
                    {
                    .corner         = RAILWAY_NOMINAL,
                    .fuse           = RBCPR_FUSE_NOMINAL,
                    .voltage_ceil   = 1225000,
                    .voltage_floor  = 1050000,
					.open_loop_adjust = 0,
                    },
                },
                {
                .corner_params =
                    {
                    .corner         = RAILWAY_SUPER_TURBO,
                    .fuse           = RBCPR_FUSE_TURBO,
                    .voltage_ceil   = 1350000, //not actual ceiling, this is fuse reference, ceiling is 1325mv
                    .voltage_floor  = 1162500,
					.open_loop_adjust = 0,
                    },
                },
				{
                .corner_params =
                    {
                    .corner         = RAILWAY_SUPER_TURBO_NO_CPR,
                    .fuse           = RBCPR_FUSE_TURBO,
                    .voltage_ceil   = 1350000, //not actual ceiling, this is fuse reference, ceiling is 1325mv
                    .voltage_floor  = 1325000,
					.open_loop_adjust = 0,
                    },
                },
            },
        .rbcpr_enablement=RBCPR_ENABLED_OPEN_LOOP,
        .number_of_target_params=4,
     },
	
     {   // VDDCX
        .target_params = (rbcpr_corner_params_and_fuse_type[])
            {
                {
                .corner_params =
                     {
                        .corner         = RAILWAY_SVS_SOC,
                        .fuse           = RBCPR_FUSE_SVS,
                        .voltage_ceil   = 1050000,
                        .voltage_floor  = 900000,
						.open_loop_adjust = 15000,
                    },
                },
                {
                .corner_params =
                    {
                        .corner         = RAILWAY_NOMINAL,
                        .fuse           = RBCPR_FUSE_NOMINAL,
                        .voltage_ceil   = 1225000,
                        .voltage_floor  = 1050000,
						.open_loop_adjust = 0,
                    },
                },
                {
                .corner_params =
                    {
                        .corner         = RAILWAY_SUPER_TURBO,
                        .fuse           = RBCPR_FUSE_TURBO,
                        .voltage_ceil   = 1350000, //not actual ceiling, this is fuse reference, ceiling is 1325mv
                        .voltage_floor  = 1162500,
						.open_loop_adjust = 0,
                    },
                },
				{
                .corner_params =
                    {
                        .corner         = RAILWAY_SUPER_TURBO_NO_CPR,
                        .fuse           = RBCPR_FUSE_TURBO,
                        .voltage_ceil   = 1350000, //not actual ceiling, this is fuse reference, ceiling is 1325mv
                        .voltage_floor  = 1325000,
						.open_loop_adjust = 0,
                    },
                },
            },
        .rbcpr_enablement=RBCPR_ENABLED_OPEN_LOOP,
        .number_of_target_params=4,
		
    },
};
rbcpr_bsp_rail_params_type gf_cpr_settings_rev2[] =
{
     {   //VddMx
        .target_params = (rbcpr_corner_params_and_fuse_type[])
            {
                {
                .corner_params =
                    {
                    .corner         = RAILWAY_SVS_SOC,
                    .fuse           = RBCPR_FUSE_SVS,
                    .voltage_ceil   = 1050000,
                    .voltage_floor  = 1050000,
					.open_loop_adjust = 0,
                    },
                },
                {
                .corner_params =
                    {
                    .corner         = RAILWAY_NOMINAL,
                    .fuse           = RBCPR_FUSE_NOMINAL,
                    .voltage_ceil   = 1225000,
                    .voltage_floor  = 1050000,
					.open_loop_adjust = -20000,
                    },
                },
                {
                .corner_params =
                    {
                    .corner         = RAILWAY_SUPER_TURBO,
                    .fuse           = RBCPR_FUSE_TURBO,
                    .voltage_ceil   = 1350000, //not actual ceiling, this is fuse reference, ceiling is 1325mv
                    .voltage_floor  = 1162500,
					.open_loop_adjust = -40000,
                    },
                },
				{
                .corner_params =
                    {
                    .corner         = RAILWAY_SUPER_TURBO_NO_CPR,
                    .fuse           = RBCPR_FUSE_TURBO,
                    .voltage_ceil   = 1350000, //not actual ceiling, this is fuse reference, ceiling is 1325mv
                    .voltage_floor  = 1325000,
					.open_loop_adjust = 0,
                    },
                },
            },
        .rbcpr_enablement=RBCPR_ENABLED_OPEN_LOOP,
        .number_of_target_params=4,
     },
	
     {   // VDDCX
        .target_params = (rbcpr_corner_params_and_fuse_type[])
            {
                {
                .corner_params =
                     {
                        .corner         = RAILWAY_SVS_SOC,
                        .fuse           = RBCPR_FUSE_SVS,
                        .voltage_ceil   = 1050000,
                        .voltage_floor  = 900000,
						.open_loop_adjust = 0,
                    },
                },
                {
                .corner_params =
                    {
                        .corner         = RAILWAY_NOMINAL,
                        .fuse           = RBCPR_FUSE_NOMINAL,
                        .voltage_ceil   = 1225000,
                        .voltage_floor  = 1050000,
						.open_loop_adjust = -35000,
                    },
                },
                {
                .corner_params =
                    {
                        .corner         = RAILWAY_SUPER_TURBO,
                        .fuse           = RBCPR_FUSE_TURBO,
                        .voltage_ceil   = 1350000, //not actual ceiling, this is fuse reference, ceiling is 1325mv
                        .voltage_floor  = 1162500,
						.open_loop_adjust = -80000,
                    },
                },
				{
                .corner_params =
                    {
                        .corner         = RAILWAY_SUPER_TURBO_NO_CPR,
                        .fuse           = RBCPR_FUSE_TURBO,
                        .voltage_ceil   = 1350000, //not actual ceiling, this is fuse reference, ceiling is 1325mv
                        .voltage_floor  = 1325000,
						.open_loop_adjust = 0,
                    },
                },
            },
        .rbcpr_enablement=RBCPR_ENABLED_OPEN_LOOP,
        .number_of_target_params=4,
		
    },
};

rbcpr_bsp_rail_params_type smic_cpr_settings_rev2[] =
{
     {   //VddMx
        .target_params = (rbcpr_corner_params_and_fuse_type[])
            {
                {
                .corner_params =
                    {
                    .corner         = RAILWAY_SVS_SOC,
                    .fuse           = RBCPR_FUSE_SVS,
                    .voltage_ceil   = 1050000,
                    .voltage_floor  = 1050000,
					.open_loop_adjust = 0,
                    },
                },
                {
                .corner_params =
                    {
                    .corner         = RAILWAY_NOMINAL,
                    .fuse           = RBCPR_FUSE_NOMINAL,
                    .voltage_ceil   = 1225000,
                    .voltage_floor  = 1050000,
					.open_loop_adjust = 0,
                    },
                },
                {
                .corner_params =
                    {
                    .corner         = RAILWAY_SUPER_TURBO,
                    .fuse           = RBCPR_FUSE_TURBO,
                    .voltage_ceil   = 1350000, //not actual ceiling, this is fuse reference, ceiling is 1325mv
                    .voltage_floor  = 1162500,
					.open_loop_adjust = 0,
                    },
                },
				{
                .corner_params =
                    {
                    .corner         = RAILWAY_SUPER_TURBO_NO_CPR,
                    .fuse           = RBCPR_FUSE_TURBO,
                    .voltage_ceil   = 1350000, //not actual ceiling, this is fuse reference, ceiling is 1325mv
                    .voltage_floor  = 1325000,
					.open_loop_adjust = 0,
                    },
                },
            },
        .rbcpr_enablement=RBCPR_ENABLED_OPEN_LOOP,
        .number_of_target_params=4,
     },
	
     {   // VDDCX
        .target_params = (rbcpr_corner_params_and_fuse_type[])
            {
                {
                .corner_params =
                     {
                        .corner         = RAILWAY_SVS_SOC,
                        .fuse           = RBCPR_FUSE_SVS,
                        .voltage_ceil   = 1050000,
                        .voltage_floor  = 900000,
						.open_loop_adjust = 15000,
                    },
                },
                {
                .corner_params =
                    {
                        .corner         = RAILWAY_NOMINAL,
                        .fuse           = RBCPR_FUSE_NOMINAL,
                        .voltage_ceil   = 1225000,
                        .voltage_floor  = 1050000,
						.open_loop_adjust = -10000,
                    },
                },
                {
                .corner_params =
                    {
                        .corner         = RAILWAY_SUPER_TURBO,
                        .fuse           = RBCPR_FUSE_TURBO,
                        .voltage_ceil   = 1350000, //not actual ceiling, this is fuse reference, ceiling is 1325mv
                        .voltage_floor  = 1162500,
						.open_loop_adjust = -30000,
                    },
                },
				{
                .corner_params =
                    {
                        .corner         = RAILWAY_SUPER_TURBO_NO_CPR,
                        .fuse           = RBCPR_FUSE_TURBO,
                        .voltage_ceil   = 1350000, //not actual ceiling, this is fuse reference, ceiling is 1325mv
                        .voltage_floor  = 1325000,
						.open_loop_adjust = 0,
                    },
                },
            },
        .rbcpr_enablement=RBCPR_ENABLED_OPEN_LOOP,
        .number_of_target_params=4,
		
    },
};

const rbcpr_bsp_revision_array_type hw_version_specific_settings_9x05 =
{
    .hw_version_count = 1,
    .hw_version_array = (const rbcpr_versioned_bsp_rail_params_type[])
    {
		{
		.fab_process_id_count = 5,
		.rbcpr_bsp_params_tsmc_fab = (const rbcpr_tsmc_fab_id_settings[])
			{
				{
					.bsp_params = ( rbcpr_bsp_rail_params_type*)&tsmc_cpr_settings,
					.process_id = 0,
					.foundry_id = 0,
				},
				{
					.bsp_params = ( rbcpr_bsp_rail_params_type*)&gf_cpr_settings_rev1,
					.process_id = 1,
					.foundry_id = 1,
				},
				{
					.bsp_params = ( rbcpr_bsp_rail_params_type*)&smic_cpr_settings_rev1,
					.process_id = 1,
					.foundry_id = 5,
				},
				{
					.bsp_params = ( rbcpr_bsp_rail_params_type*)&gf_cpr_settings_rev2,
					.process_id = 2,
					.foundry_id = 1,
				},
				{
					.bsp_params = ( rbcpr_bsp_rail_params_type*)&smic_cpr_settings_rev2,
					.process_id = 2,
					.foundry_id = 5,
				},
			},
		 
        .supported_hw_after_version = CHIPINFO_VERSION(1, 0),
		},
		
		
    },
    .supported_chipset = CHIPINFO_FAMILY_MDM9205,
};

const rbcpr_supported_hw_array_type rbcpr_supported_hw_array =
{
    .hw_version_specific_settings = (const rbcpr_bsp_revision_array_type*[])
    {
        &hw_version_specific_settings_9x05,
    },
    .hw_version_specific_settings_count = 1,
};

const rbcpr_bsp_rail_const_info rbcpr_rail_const_info[] =
{
    {   //VddMx
        .rail_name                      = "vddmx",
        .pmic_step_size_uv              = 12500, 
        .efuse_target_voltage_multipler = 12500,
        .disable_fused_floor            = true,
    },
    {   // VDDCX
        .rail_name              = "vddcx",
        .pmic_step_size_uv      = 12500,
        .efuse_target_voltage_multipler = 12500,
        .disable_fused_floor            = true,
    },
};

rbcpr_bsp_type rbcpr_bsp_data =
{
    .rails = (rbcpr_bsp_rail_info[])
    {
        {   //VddMx
             .rail_const_info = &rbcpr_rail_const_info[0],
        },
        {   //VddCx
             .rail_const_info = &rbcpr_rail_const_info[1],
        },
    },
    .num_rails = 2,
};

const rbcpr_sensor_set mx_sensors = 
{
    .rail_id = 1,       //Cx
    .sensors_count = 2,
    .sensors = (const int[])
    {
        10,11,
    },
};

void rbcpr_bsp_target_init()
{
 const int cx_rail_id = mx_sensors.rail_id;
    
    rbcpr_bsp_rail_info* rail = &rbcpr_bsp_data.rails[cx_rail_id];
    
    if(rail->bsp_data->rbcpr_enablement != RBCPR_ENABLED_CLOSED_LOOP)
    {
        return; //We're not using the CPR HW block on the relevant rail, just return.
    }

    for(int i=0; i< mx_sensors.sensors_count; i++)
    {
        HAL_rbcpr_mask_sensor(rail->hal_handle, mx_sensors.sensors[i], TRUE);
    }
}


void rbcpr_bsp_check_ss_part(void)
{

   if( railway_get_corner_voltage(cx_id, RAILWAY_NOMINAL) >= SS_THRESHOLD_OPENLOOP_VOL)
	 {
	    rbcpr_ss_vol_threshold = true; 
	 }
}   
	   

	   
/*===========================================================================
FUNCTION: rbcpr_init_rosc_target

DESCRIPTION: initializes the rosc_target values based on static margin if closed loop

RETURN VALUE: None
===========================================================================*/
void rbcpr_bsp_init_rosc_target(rbcpr_bsp_rail_info* rail)
{
    if (rail->bsp_data->rbcpr_enablement != RBCPR_ENABLED_CLOSED_LOOP)
	    return;
	
	rbcpr_bsp_check_ss_part();
	
	for (int i=0; i < rail->bsp_data->number_of_target_params; i++)	
    {
	if(rbcpr_ss_vol_threshold ==true)
    {
	    rail->bsp_data->target_params[i].corner_params.static_margin += rail->bsp_data->target_params[i].corner_params.ss_static_margin_adjust;
	}
	
    	for (int j=0; j < RBCPR_ROSC_COUNT; j++)
		{
			// divide it by 1000000, because voltage delta is in uv and voltage scaling factor (kv) is also multiplied by 1000 to avoid floating point arthmatic
			int32 calc_value = ((rail->bsp_data->target_params[i].corner_params.static_margin ) * (rail->bsp_data->voltage_scaling_vector[j]));
            calc_value = (int32)calc_value/1000000;
			  
			// adjust precision from above calculation
			rail->bsp_data->target_params[i].corner_params.rosc_target[j] +=  calc_value + rail->bsp_data->target_params[i].corner_params.static_margin_adjust;
			
		}
    }
}

