/*******************************************************************************
 *
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * QUALCOMM Proprietary and Confidential.
 *
 * THIS IS AUTO-GENERATED CPR CONFIG FILE FOR Mdm9x05.
 *
 * DateTime: 2023/05/16 23:22:35
 *
 *******************************************************************************/

#include "cpr_data.h"
#include "cpr_image_target_init.h"
#include "cpr_target_hwio.h"


//hash value of Voltage Plan file (extracted all cell values)
static const char voltage_plan_hash_value[] = "DDF2152B53B83977D3B26FC2DD120B78";


/*******************************************************************************
********************************************************************************
**
** Voltage Plan Data
**
********************************************************************************
********************************************************************************/

static cpr_margin_cfg margins_14 = {
    .count = 1,
    .data = (const cpr_margin_data[]) { { .cprRevMin = 0, .cprRevMax = 7, .openLoop =       0, .closedLoop =       0, .maxFloorToCeil =      0 } }
};


static cpr_fuse_data  fuses_1216533902 =
{
    .volt       = (cpr_fuse[]) { {.count = 1, .data = (struct raw_fuse_data[]) { CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW1_LSB, CPR0_TARG_VOLT_SVS) } } },
    .quot       = NULL,
    .rosel      = NULL,
    .quotOffset = NULL,
    .voltOffset = NULL,
};


static cpr_fuse_data  fuses_1891160438 =
{
    .volt       = (cpr_fuse[]) { {.count = 1, .data = (struct raw_fuse_data[]) { CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW1_LSB, CPR0_TARG_VOLT_NOM) } } },
    .quot       = NULL,
    .rosel      = NULL,
    .quotOffset = NULL,
    .voltOffset = NULL,
};


static cpr_fuse_data  fuses_2658944667 =
{
    .volt       = (cpr_fuse[]) { {.count = 1, .data = (struct raw_fuse_data[]) { CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW1_LSB, CPR0_TARG_VOLT_TUR) } } },
    .quot       = NULL,
    .rosel      = NULL,
    .quotOffset = NULL,
    .voltOffset = NULL,
};


/*
 * CX Voltage Plan
 */
static cpr_voltage_plan voltage_plan_8283475290 =
{
    .modesCount      = 3,
    .tempAdjCfg      = NULL,
    .kvCfg           = NULL,
    .supportedModes  = (const cpr_voltage_mode[]) { CPR_VOLTAGE_MODE_SVS,
                                                    CPR_VOLTAGE_MODE_NOMINAL,
                                                    CPR_VOLTAGE_MODE_TURBO },
    .modes           = (struct cpr_voltage_data[]) {
        // SVS
        {.fref = 1050000, .freqDelta =       0, .fuses = &fuses_1216533902, .quotients = NULL, .margins = &margins_14,
         .marginTempBands = {0},
         .subModesCount   = 1,
         .subModes        = (cpr_freq_data[]){ {.ceiling = 1050000, .floor =  900000, .freq =       0, .interplEnabled = false} } },

        // NOMINAL
        {.fref = 1225000, .freqDelta =       0, .fuses = &fuses_1891160438, .quotients = NULL, .margins = &margins_14,
         .marginTempBands = {0},
         .subModesCount   = 1,
         .subModes        = (cpr_freq_data[]){ {.ceiling = 1225000, .floor = 1050000, .freq =       0, .interplEnabled = false} } },

        // TURBO
        {.fref = 1350000, .freqDelta =       0, .fuses = &fuses_2658944667, .quotients = NULL, .margins = &margins_14,
         .marginTempBands = {0},
         .subModesCount   = 1,
         .subModes        = (cpr_freq_data[]){ {.ceiling = 1325000, .floor = 1155000, .freq =       0, .interplEnabled = false} } }},
};


static const cpr_version  ver_TSMC_1_0_1_255 = { .foundry = CPR_FOUNDRY_TSMC, .min = CPR_CHIPINFO_VERSION(1,0), .max = CPR_CHIPINFO_VERSION(1,255) };


static cpr_margin_cfg margins_45008 = {
    .count = 2,
    .data = (const cpr_margin_data[]) { { .cprRevMin = 0, .cprRevMax = 0, .openLoop =       0, .closedLoop =       0, .maxFloorToCeil =      0 },
                                  { .cprRevMin = 1, .cprRevMax = 2, .openLoop =   15000, .closedLoop =       0, .maxFloorToCeil =      0 } }
};


static cpr_margin_cfg margins_29986 = {
    .count = 2,
    .data = (const cpr_margin_data[]) { { .cprRevMin = 0, .cprRevMax = 1, .openLoop =       0, .closedLoop =       0, .maxFloorToCeil =      0 },
                                  { .cprRevMin = 2, .cprRevMax = 2, .openLoop =  -10000, .closedLoop =       0, .maxFloorToCeil =      0 } }
};


static cpr_margin_cfg margins_89986 = {
    .count = 2,
    .data = (const cpr_margin_data[]) { { .cprRevMin = 0, .cprRevMax = 1, .openLoop =       0, .closedLoop =       0, .maxFloorToCeil =      0 },
                                  { .cprRevMin = 2, .cprRevMax = 2, .openLoop =  -30000, .closedLoop =       0, .maxFloorToCeil =      0 } }
};


/*
 * CX Voltage Plan
 */
static cpr_voltage_plan voltage_plan_6924585391 =
{
    .modesCount      = 3,
    .tempAdjCfg      = NULL,
    .kvCfg           = NULL,
    .supportedModes  = (const cpr_voltage_mode[]) { CPR_VOLTAGE_MODE_SVS,
                                                    CPR_VOLTAGE_MODE_NOMINAL,
                                                    CPR_VOLTAGE_MODE_TURBO },
    .modes           = (struct cpr_voltage_data[]) {
        // SVS
        {.fref = 1050000, .freqDelta =       0, .fuses = &fuses_1216533902, .quotients = NULL, .margins = &margins_45008,
         .marginTempBands = {0},
         .subModesCount   = 1,
         .subModes        = (cpr_freq_data[]){ {.ceiling = 1050000, .floor =  900000, .freq =       0, .interplEnabled = false} } },

        // NOMINAL
        {.fref = 1225000, .freqDelta =       0, .fuses = &fuses_1891160438, .quotients = NULL, .margins = &margins_29986,
         .marginTempBands = {0},
         .subModesCount   = 1,
         .subModes        = (cpr_freq_data[]){ {.ceiling = 1225000, .floor = 1050000, .freq =       0, .interplEnabled = false} } },

        // TURBO
        {.fref = 1350000, .freqDelta =       0, .fuses = &fuses_2658944667, .quotients = NULL, .margins = &margins_89986,
         .marginTempBands = {0},
         .subModesCount   = 1,
         .subModes        = (cpr_freq_data[]){ {.ceiling = 1325000, .floor = 1155000, .freq =       0, .interplEnabled = false} } }},
};


static const cpr_version  ver_SMIC_1_0_1_255 = { .foundry = CPR_FOUNDRY_SMIC, .min = CPR_CHIPINFO_VERSION(1,0), .max = CPR_CHIPINFO_VERSION(1,255) };


static cpr_margin_cfg margins_4 = {
    .count = 1,
    .data = (const cpr_margin_data[]) { { .cprRevMin = 0, .cprRevMax = 2, .openLoop =       0, .closedLoop =       0, .maxFloorToCeil =      0 } }
};


static cpr_fuse_data  fuses_185387428 =
{
    .volt       = (cpr_fuse[]) { {.count = 1, .data = (struct raw_fuse_data[]) { CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW1_MSB, CPR1_TARG_VOLT_SVS) } } },
    .quot       = NULL,
    .rosel      = NULL,
    .quotOffset = NULL,
    .voltOffset = NULL,
};


static cpr_margin_cfg margins_59992 = {
    .count = 2,
    .data = (const cpr_margin_data[]) { { .cprRevMin = 0, .cprRevMax = 0, .openLoop =       0, .closedLoop =       0, .maxFloorToCeil =      0 },
                                  { .cprRevMin = 1, .cprRevMax = 2, .openLoop =  -20000, .closedLoop =       0, .maxFloorToCeil =      0 } }
};


static cpr_fuse_data  fuses_2328662968 =
{
    .volt       = (cpr_fuse[]) { {.count = 1, .data = (struct raw_fuse_data[]) { CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW1_MSB, CPR1_TARG_VOLT_NOM) } } },
    .quot       = NULL,
    .rosel      = NULL,
    .quotOffset = NULL,
    .voltOffset = NULL,
};


static cpr_margin_cfg margins_119992 = {
    .count = 2,
    .data = (const cpr_margin_data[]) { { .cprRevMin = 0, .cprRevMax = 0, .openLoop =       0, .closedLoop =       0, .maxFloorToCeil =      0 },
                                  { .cprRevMin = 1, .cprRevMax = 2, .openLoop =  -40000, .closedLoop =       0, .maxFloorToCeil =      0 } }
};


static cpr_fuse_data  fuses_691043773 =
{
    .volt       = (cpr_fuse[]) { {.count = 1, .data = (struct raw_fuse_data[]) { CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW1_MSB, CPR1_TARG_VOLT_TUR) } } },
    .quot       = NULL,
    .rosel      = NULL,
    .quotOffset = NULL,
    .voltOffset = NULL,
};


/*
 * MX Voltage Plan
 */
static cpr_voltage_plan voltage_plan_5524825865 =
{
    .modesCount      = 3,
    .tempAdjCfg      = NULL,
    .kvCfg           = NULL,
    .supportedModes  = (const cpr_voltage_mode[]) { CPR_VOLTAGE_MODE_SVS,
                                                    CPR_VOLTAGE_MODE_NOMINAL,
                                                    CPR_VOLTAGE_MODE_TURBO },
    .modes           = (struct cpr_voltage_data[]) {
        // SVS
        {.fref = 1050000, .freqDelta =       0, .fuses = &fuses_185387428, .quotients = NULL, .margins = &margins_4,
         .marginTempBands = {0},
         .subModesCount   = 1,
         .subModes        = (cpr_freq_data[]){ {.ceiling = 1050000, .floor = 1050000, .freq =       0, .interplEnabled = false} } },

        // NOMINAL
        {.fref = 1225000, .freqDelta =       0, .fuses = &fuses_2328662968, .quotients = NULL, .margins = &margins_59992,
         .marginTempBands = {0},
         .subModesCount   = 1,
         .subModes        = (cpr_freq_data[]){ {.ceiling = 1225000, .floor = 1050000, .freq =       0, .interplEnabled = false} } },

        // TURBO
        {.fref = 1350000, .freqDelta =       0, .fuses = &fuses_691043773, .quotients = NULL, .margins = &margins_119992,
         .marginTempBands = {0},
         .subModesCount   = 1,
         .subModes        = (cpr_freq_data[]){ {.ceiling = 1325000, .floor = 1155000, .freq =       0, .interplEnabled = false} } }},
};


static const cpr_version  ver_GF_1_0_1_255 = { .foundry = CPR_FOUNDRY_GF, .min = CPR_CHIPINFO_VERSION(1,0), .max = CPR_CHIPINFO_VERSION(1,255) };


static cpr_margin_cfg margins_29994 = {
    .count = 3,
    .data = (const cpr_margin_data[]) { { .cprRevMin = 0, .cprRevMax = 0, .openLoop =       0, .closedLoop =       0, .maxFloorToCeil =      0 },
                                  { .cprRevMin = 1, .cprRevMax = 1, .openLoop =  -25000, .closedLoop =       0, .maxFloorToCeil =      0 },
                                  { .cprRevMin = 2, .cprRevMax = 2, .openLoop =  -35000, .closedLoop =       0, .maxFloorToCeil =      0 } }
};


static cpr_margin_cfg margins_119994 = {
    .count = 3,
    .data = (const cpr_margin_data[]) { { .cprRevMin = 0, .cprRevMax = 0, .openLoop =       0, .closedLoop =       0, .maxFloorToCeil =      0 },
                                  { .cprRevMin = 1, .cprRevMax = 1, .openLoop =  -40000, .closedLoop =       0, .maxFloorToCeil =      0 },
                                  { .cprRevMin = 2, .cprRevMax = 2, .openLoop =  -80000, .closedLoop =       0, .maxFloorToCeil =      0 } }
};


/*
 * CX Voltage Plan
 */
static cpr_voltage_plan voltage_plan_6802494335 =
{
    .modesCount      = 3,
    .tempAdjCfg      = NULL,
    .kvCfg           = NULL,
    .supportedModes  = (const cpr_voltage_mode[]) { CPR_VOLTAGE_MODE_SVS,
                                                    CPR_VOLTAGE_MODE_NOMINAL,
                                                    CPR_VOLTAGE_MODE_TURBO },
    .modes           = (struct cpr_voltage_data[]) {
        // SVS
        {.fref = 1050000, .freqDelta =       0, .fuses = &fuses_1216533902, .quotients = NULL, .margins = &margins_4,
         .marginTempBands = {0},
         .subModesCount   = 1,
         .subModes        = (cpr_freq_data[]){ {.ceiling = 1050000, .floor =  900000, .freq =       0, .interplEnabled = false} } },

        // NOMINAL
        {.fref = 1225000, .freqDelta =       0, .fuses = &fuses_1891160438, .quotients = NULL, .margins = &margins_29994,
         .marginTempBands = {0},
         .subModesCount   = 1,
         .subModes        = (cpr_freq_data[]){ {.ceiling = 1225000, .floor = 1050000, .freq =       0, .interplEnabled = false} } },

        // TURBO
        {.fref = 1350000, .freqDelta =       0, .fuses = &fuses_2658944667, .quotients = NULL, .margins = &margins_119994,
         .marginTempBands = {0},
         .subModesCount   = 1,
         .subModes        = (cpr_freq_data[]){ {.ceiling = 1325000, .floor = 1155000, .freq =       0, .interplEnabled = false} } }},
};


/*
 * MX Voltage Plan
 */
static cpr_voltage_plan voltage_plan_6639457319 =
{
    .modesCount      = 3,
    .tempAdjCfg      = NULL,
    .kvCfg           = NULL,
    .supportedModes  = (const cpr_voltage_mode[]) { CPR_VOLTAGE_MODE_SVS,
                                                    CPR_VOLTAGE_MODE_NOMINAL,
                                                    CPR_VOLTAGE_MODE_TURBO },
    .modes           = (struct cpr_voltage_data[]) {
        // SVS
        {.fref = 1050000, .freqDelta =       0, .fuses = &fuses_185387428, .quotients = NULL, .margins = &margins_14,
         .marginTempBands = {0},
         .subModesCount   = 1,
         .subModes        = (cpr_freq_data[]){ {.ceiling = 1050000, .floor = 1050000, .freq =       0, .interplEnabled = false} } },

        // NOMINAL
        {.fref = 1225000, .freqDelta =       0, .fuses = &fuses_2328662968, .quotients = NULL, .margins = &margins_14,
         .marginTempBands = {0},
         .subModesCount   = 1,
         .subModes        = (cpr_freq_data[]){ {.ceiling = 1225000, .floor = 1050000, .freq =       0, .interplEnabled = false} } },

        // TURBO
        {.fref = 1350000, .freqDelta =       0, .fuses = &fuses_691043773, .quotients = NULL, .margins = &margins_14,
         .marginTempBands = {0},
         .subModesCount   = 1,
         .subModes        = (cpr_freq_data[]){ {.ceiling = 1325000, .floor = 1155000, .freq =       0, .interplEnabled = false} } }},
};


/*
 * MX Voltage Plan
 */
static cpr_voltage_plan voltage_plan_4124189351 =
{
    .modesCount      = 3,
    .tempAdjCfg      = NULL,
    .kvCfg           = NULL,
    .supportedModes  = (const cpr_voltage_mode[]) { CPR_VOLTAGE_MODE_SVS,
                                                    CPR_VOLTAGE_MODE_NOMINAL,
                                                    CPR_VOLTAGE_MODE_TURBO },
    .modes           = (struct cpr_voltage_data[]) {
        // SVS
        {.fref = 1050000, .freqDelta =       0, .fuses = &fuses_185387428, .quotients = NULL, .margins = &margins_4,
         .marginTempBands = {0},
         .subModesCount   = 1,
         .subModes        = (cpr_freq_data[]){ {.ceiling = 1050000, .floor = 1050000, .freq =       0, .interplEnabled = false} } },

        // NOMINAL
        {.fref = 1225000, .freqDelta =       0, .fuses = &fuses_2328662968, .quotients = NULL, .margins = &margins_4,
         .marginTempBands = {0},
         .subModesCount   = 1,
         .subModes        = (cpr_freq_data[]){ {.ceiling = 1225000, .floor = 1050000, .freq =       0, .interplEnabled = false} } },

        // TURBO
        {.fref = 1350000, .freqDelta =       0, .fuses = &fuses_691043773, .quotients = NULL, .margins = &margins_4,
         .marginTempBands = {0},
         .subModesCount   = 1,
         .subModes        = (cpr_freq_data[]){ {.ceiling = 1325000, .floor = 1155000, .freq =       0, .interplEnabled = false} } }},
};


/*
 * Aging configs
 */

/*
 * Versioned Voltage Plans
 */

static const cpr_versioned_voltage_plan cx_vvp = {
    .rail     = CPR_RAIL_CX,
    .list     = (struct voltage_plan_list[]) {
        { .version = &ver_TSMC_1_0_1_255, .cfg = &voltage_plan_8283475290 },

        { .version = &ver_SMIC_1_0_1_255, .cfg = &voltage_plan_6924585391 },

        { .version = &ver_GF_1_0_1_255, .cfg = &voltage_plan_6802494335 },
     },
    .count    = 3,
    .agingCfg = NULL
};


static const cpr_versioned_voltage_plan mx_vvp = {
    .rail     = CPR_RAIL_MX,
    .list     = (struct voltage_plan_list[]) {
        { .version = &ver_GF_1_0_1_255, .cfg = &voltage_plan_5524825865 },

        { .version = &ver_TSMC_1_0_1_255, .cfg = &voltage_plan_6639457319 },

        { .version = &ver_SMIC_1_0_1_255, .cfg = &voltage_plan_4124189351 },
     },
    .count    = 3,
    .agingCfg = NULL
};

/*******************************************************************************
********************************************************************************
**
** Enablement Config Data
**
********************************************************************************
********************************************************************************/

/*
 * CPR Controller Thread Config
 */

static const cpr_hal_thread_cfg mx_thread_cfg = {
    .upThresh     = 2,
    .dnThresh     = 2,
    .consecUp     = 0,
    .consecDn     = 2,
    .sensors      = NULL,
    .sensorsCount = 0,
};


static cpr_hal_thread  mx_thread = {
    .id   = 0,
    .cfg  = &mx_thread_cfg,
};


/*
 * CPR Controller Config
 */

static const cpr_hal_controller_cfg rbcpr_controller_cfg = {
    .stepQuotMin         = 12,
    .stepQuotMax         = 14,
    .sensorsTotal        = 0,
    .bypassSensors       = NULL,
    .bypassSensorsCount  = 0,
    .disableSensors      = NULL,
    .disableSensorsCount = 0,
    .refClk              = NULL,
    .ahbClk              = NULL,
};


static cpr_hal_controller  rbcpr_controller = {
    .base                = 0x48000,
    .type                = CPR_CONTROLLER_TYPE_SW_CL_ONLY,
    .cfg                 = &rbcpr_controller_cfg,
    .threads             = (cpr_hal_thread*[]){ &mx_thread },
    .threadsCount        = 1,
};


/*
 * HAL Rail Config
 */

static cpr_rail cpr_rail_cx_cfg = {
    .id               = CPR_RAIL_CX,
    .name             = "CX",
    .hal              = { .controller = NULL, .thread = NULL },
    .interruptId      = 0,
    .settleModes      = NULL,
    .settleModesCount = 0,
};


static cpr_rail cpr_rail_mx_cfg = {
    .id               = CPR_RAIL_MX,
    .name             = "MX",
    .hal              = { .controller = &rbcpr_controller, .thread = &mx_thread },
    .interruptId      = 0,
    .settleModes      = NULL,
    .settleModesCount = 0,
};


/*
 * Rail Enablement Config
 */

static const cpr_version  ver_ANY_0_0_255_255 = { .foundry = CPR_FOUNDRY_ANY, .min = CPR_CHIPINFO_VERSION(0,0), .max = CPR_CHIPINFO_VERSION(255,255) };


static const cpr_enablement cpr_rail_cx_enablement_0_0_255_255 =
{
    .id                = CPR_RAIL_CX,
    .version           = &ver_ANY_0_0_255_255,
    .funcId            = CPR_INIT_OPEN_LOOP,
    .fuseMultiplier    = 12500,
    .stepSize          = 12000,
};


static const cpr_enablement cpr_rail_mx_enablement_0_0_255_255 =
{
    .id                = CPR_RAIL_MX,
    .version           = &ver_ANY_0_0_255_255,
    .funcId            = CPR_INIT_OPEN_LOOP,
    .fuseMultiplier    = 12500,
    .stepSize          = 12000,
};


static const cpr_misc_cfg misc_cfg = {
    .cprRev = {.count = 1, .data = (struct raw_fuse_data[]) { {0xa423c, 19, 0x380000} } }
};


const cpr_settings cpr_settings_Mdm9x05 =
{
    .hashValue        = voltage_plan_hash_value,

    .railVVPs         = (const cpr_versioned_voltage_plan*[]) { &cx_vvp, &mx_vvp },
    .rails            = (cpr_rail*[]) { &cpr_rail_cx_cfg, &cpr_rail_mx_cfg },
    .railsCount       = 2,

    .controllers      = (cpr_hal_controller*[]) { &rbcpr_controller },
    .controllersCount = 1,

    .enablements      = (const cpr_enablement*[]) { &cpr_rail_cx_enablement_0_0_255_255, &cpr_rail_mx_enablement_0_0_255_255 },
    .enablementsCount = 2,

    .miscCfg          = &misc_cfg,
};

