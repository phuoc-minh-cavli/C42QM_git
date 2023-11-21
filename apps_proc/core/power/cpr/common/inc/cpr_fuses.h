/*===========================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#ifndef CPR_FUSES_H
#define CPR_FUSES_H

#include "comdef.h"
#include <stdint.h>
#include "cpr_defs.h"
#include "cpr_device_hw_version.h"

#define CPR_FUSE_MAPPING(base_register, field) \
    {   \
    .fuse_address   = HWIO_##base_register##_ADDR, \
    .offset         = HWIO_##base_register##_##field##_SHFT, \
    .mask           = HWIO_##base_register##_##field##_BMSK, \
    }

typedef enum    //Used only for the eFuses
{
    CPR_FUSE_NO_FUSE,
    CPR_FUSE_SVS2,
    CPR_FUSE_SVS,
    CPR_FUSE_NOMINAL,                   /* names like TARG_VOLT_NOM_ in the fuse address header*/
    CPR_FUSE_TURBO,
    CPR_FUSE_SUTUR,
    CPR_FUSE_AGING,
    CPR_FUSE_SVS2_OFFSET,
    CPR_FUSE_SVS_OFFSET,      /* New July 2015 8996 v3 margin adjustments can be fused per part*/
    CPR_FUSE_NOMINAL_OFFSET,  
    CPR_FUSE_TURBO_OFFSET,
    CPR_FUSE_SUTURBO_OFFSET,
    CPR_FUSE_FUSING_REV,                /* New Aug 2015, the fusing algorithm can now be revised and decoded differntly */
    CPR_FUSE_RO_SEL_SVS,
    CPR_FUSE_RO_SEL_NOMINAL,
    CPR_FUSE_RO_SEL_TURBO,
    CPR_FUSE_LOCAL_QUOT_SVS,
    CPR_FUSE_LOCAL_QUOT_NOMINAL,
    CPR_FUSE_LOCAL_QUOT_TURBO,
    CPR_FUSE_MAX
} cpr_fuse_type;

typedef struct
{
    uintptr_t                                           fuse_address;
    uint32                                              offset;
    uint32                                              mask;
} cpr_bsp_part_individual_fuse_config_t;

// If a fuse is segmented across multiple registers, the partial_individual_fuse_configs
// must be listed in order from most significant to least significant bits.
//

typedef struct
{
    cpr_fuse_type                                       fuse_type;
    uint8                                               fuse_total_bits;
    uint8                                               number_of_partial_fuse_configs;
    int16                                               cpr_fuse_value;
    const cpr_bsp_part_individual_fuse_config_t*        partial_individual_fuse_configs;
} cpr_bsp_individual_fuse_config_t;

typedef struct
{
    cpr_rail_id_t                                       rail_id;
    uint32                                              number_of_fuses;
    cpr_bsp_individual_fuse_config_t*                   individual_fuse_configs;
} cpr_bsp_fuse_rail_config_t;

typedef struct
{
    cpr_config_hw_version_range                         hw_versions;
    const cpr_bsp_fuse_rail_config_t*                   rail_fuse_config;
    uint32                                              number_of_fused_rails;
} cpr_bsp_fuse_versioned_config_t;


typedef struct
{
    const cpr_bsp_fuse_versioned_config_t**   versioned_fuses;
    uint32                                    versioned_fuses_count;
} cpr_config_bsp_fuse_config_t;


//Every target must define a cpr_bsp_fuse_config instance
extern const cpr_config_bsp_fuse_config_t cpr_bsp_fuse_config;

//Every target must define a cpr_bsp_fusing_rev_info instance even if the result is forced to 0.
extern cpr_bsp_individual_fuse_config_t cpr_bsp_fusing_rev_info;

int32 cpr_fuses_get_fuse_value(cpr_rail_id_t rail_id, cpr_fuse_type);

void cpr_fuses_individual_fuse_init(cpr_bsp_individual_fuse_config_t* rail_fuse_config);

const cpr_bsp_fuse_versioned_config_t* cpr_config_find_device_fuses(const cpr_config_bsp_fuse_config_t* target_fuse_configs);

int32 cpr_config_which_device_fuses_used(const cpr_config_bsp_fuse_config_t* target_fuse_configs);

void cpr_fuses_init(void);

#endif //CPR_FUSES_H
