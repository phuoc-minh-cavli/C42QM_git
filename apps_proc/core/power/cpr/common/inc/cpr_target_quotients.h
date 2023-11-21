/*===========================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#ifndef CPR_TARGET_QUOTIENTS_H
#define CPR_TARGET_QUOTIENTS_H

#include "comdef.h"
#include "cpr_defs.h"
#include "cpr_device_hw_version.h"
#include "cpr_target_defs.h"
#include "cpr_fuses.h"

typedef struct
{
    cpr_voltage_mode_t      voltage_mode;
    uint16                  ro_base_quotient;
} cpr_target_quotient_level_t;

typedef struct
{
    cpr_config_hw_version_range             hw_versions;
    const cpr_target_quotient_level_t*      target_quotient_level;
    uint32                                  target_quotient_level_count;
    uint16                                  ro_kv_x_100[CPR_ROSC_COUNT];
} cpr_target_quotient_versioned_config_t;

typedef struct
{
    cpr_rail_id_t                                   rail_id;
    const cpr_target_quotient_versioned_config_t**  versioned_target_quotient_config;
    uint32                                          versioned_target_quotient_config_count;
} cpr_target_quotient_rail_config_t;

typedef struct
{
    const cpr_target_quotient_rail_config_t**   rail_config;
    uint32                                      rail_config_count;
} cpr_target_quotient_global_config_t;

typedef struct
{
    uint16  ro_target_quotient;
    uint8   rosc_num;
} cpr_target_quotient_set_t;

// All BSPs must provide a cpr_bsp_target_quotient_config
extern const cpr_target_quotient_global_config_t cpr_bsp_target_quotient_config;

void cpr_target_quotient_calculate_quotients(cpr_rail_id_t rail_id, cpr_voltage_mode_t voltage_mode, int32 static_margin_mv, cpr_target_quotient_set_t* target_quotients);
//void cpr_target_quotient_apply_therm(cpr_rail_id_t rail_id, cpr_voltage_mode_t voltage_mode, int32 therm_margin_mv, cpr_target_quotient_set_t* target_quotients);
#endif //CPR_TARGET_QUOTIENTS_H
