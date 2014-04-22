/*===========================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.


  Fuses spread across multiple locations are always defined from MSB to LSBs. As the fuse reader
  code reads partial values, it shifts previous results left to make room for the new bits
  Example config showing the required order from MSBs to LSBs:
             CPR_FUSE_MAPPING(QFPROM__ROW1_LSB, CPRn_TARG_VOLT_OFFSET_SVS2),       //bit 7
             CPR_FUSE_MAPPING(QFPROM__ROW3_MSB, CPRn_TARG_VOLT_OFFSET_SVS2_BIT_1), //bit 6
             CPR_FUSE_MAPPING(QFPROM__ROW2_MSB, CPRn_TARG_VOLT_OFFSET_SVS2),       //bits 4-5
             CPR_FUSE_MAPPING(QFPROM__ROW2_MSB, CPRn_TARG_VOLT_OFFSET_SVS),        //bits 0-3

  ===========================================================================*/

#include "cpr_fuses.h"
#include "cpr_device_hw_version.h"
#include "cpr_qfprom.h"
#include "SecurityControlV2HWIO.h"
#ifdef WINSIM
#define CPR_FUSE_MAPPING(x,y) (0)
#else
#include "HALhwio.h"
#endif

// -----------------------------------------------------------------
// 9x45 V1 Fuses 
// -----------------------------------------------------------------
const cpr_bsp_fuse_versioned_config_t cpr_bsp_fuse_config_vALL =
{
    .hw_versions =
    {
        .foundry_range = (const cpr_config_foundry_range[])
        {    //foundary        min version (inclusive)      max version (exclusive)       min CPR Fuse rev,  max CPR Fuse rev
            {CPR_FOUNDRY_TSMC, CPR_CHIPINFO_VERSION(0, 0), CPR_CHIPINFO_VERSION(0xFF, 0xFF), 0, 0xFF},
        },
        .foundry_range_count = 1,
    },
    .rail_fuse_config = (cpr_bsp_fuse_rail_config_t[])
    {
        {   //Vdd_Mss
            .rail_id = CPR_RAIL_APC0,
            .number_of_fuses = 10,
            .individual_fuse_configs = (cpr_bsp_individual_fuse_config_t[])
            {
                {
                    .fuse_type = CPR_FUSE_SVS,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW1_LSB, CPR2_TARG_VOLT_SVS)
                        },
                },
                {
                    .fuse_type = CPR_FUSE_NOMINAL,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW1_LSB, CPR2_TARG_VOLT_NOM)
                        },
                },
                {
                    .fuse_type = CPR_FUSE_TURBO,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW0_MSB, CPR2_TARG_VOLT_TUR)
                        },
                },
                {
                    .fuse_type = CPR_FUSE_AGING,
                    .number_of_partial_fuse_configs = 4,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                         {
                             CPR_FUSE_MAPPING(QFPROM_RAW_CALIB_ROW0_MSB, CPR2_TARG_VOLT_TUR),       /*bit 7*/
                             CPR_FUSE_MAPPING(QFPROM_RAW_CALIB_ROW0_MSB, CPR2_TARG_VOLT_TUR), /*bit 6*/
                             CPR_FUSE_MAPPING(QFPROM_RAW_CALIB_ROW0_MSB, CPR2_TARG_VOLT_TUR),       /*bits 4-5*/
                             CPR_FUSE_MAPPING(QFPROM_RAW_CALIB_ROW0_MSB, CPR2_TARG_VOLT_TUR),        /*bits 0-3*/
                         },
                },
                {
                    .fuse_type = CPR_FUSE_RO_SEL_SVS,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW0_MSB, CPR2_RO_SEL_SVS)
                        },
                },
                {
                    .fuse_type = CPR_FUSE_RO_SEL_NOMINAL,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW0_MSB, CPR2_RO_SEL_NOM)
                        },
                },
                {
                    .fuse_type = CPR_FUSE_RO_SEL_TURBO,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW0_MSB, CPR2_RO_SEL_TUR)
                        },
                },	
                {
                    .fuse_type = CPR_FUSE_LOCAL_QUOT_SVS,
                    .number_of_partial_fuse_configs = 2,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW0_MSB, CPR2_QUOT_VMIN_SVS_1),
			                CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW0_LSB, CPR2_QUOT_VMIN_SVS),
                        },
                },
                {
                    .fuse_type = CPR_FUSE_LOCAL_QUOT_NOMINAL,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW0_LSB, CPR2_QUOT_VMIN_NOM)
                            },
                        },
                {
                    .fuse_type = CPR_FUSE_LOCAL_QUOT_TURBO,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW0_LSB, CPR2_QUOT_VMIN_TUR)
                        },
                },					
            },
        },
        
		{   //Vdd_CX  (only putting in a few corners for the MPSS eLDO voltage calculations
            .rail_id = CPR_RAIL_CX,
            .number_of_fuses = 3,
            .individual_fuse_configs = (cpr_bsp_individual_fuse_config_t[])
            {
                {
                    .fuse_type = CPR_FUSE_SVS2,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_RAW_CALIB_ROW1_LSB, CPR2_TARG_VOLT_NOM)
                        },
                },
                {
                    .fuse_type = CPR_FUSE_SVS,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_RAW_CALIB_ROW1_LSB, CPR2_TARG_VOLT_NOM)
                        },
                },
                {
                    .fuse_type = CPR_FUSE_NOMINAL,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_RAW_CALIB_ROW1_LSB, CPR2_TARG_VOLT_NOM)
                        },
                },
            },
  	    },
    },
    .number_of_fused_rails = 2,
};

//what version of CPR fuse revs are being used.
cpr_bsp_individual_fuse_config_t cpr_bsp_fusing_rev_info =
{
    .fuse_type = CPR_FUSE_FUSING_REV,
    .number_of_partial_fuse_configs = 1,
    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
    {
        { HWIO_QFPROM_CORR_CALIB_ROW1_MSB_ADDR, 21, (1 << 23) | (1 << 22) | (1 << 21) },
    },
};


const cpr_config_bsp_fuse_config_t cpr_bsp_fuse_config =
{
    .versioned_fuses = (const cpr_bsp_fuse_versioned_config_t*[])
    {
        &cpr_bsp_fuse_config_vALL,
    },
    .versioned_fuses_count = 1,
};

