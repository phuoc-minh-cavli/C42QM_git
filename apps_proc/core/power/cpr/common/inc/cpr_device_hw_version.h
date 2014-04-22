/*===========================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#ifndef CPR_DEVICE_HW_VERSION_H
#define CPR_DEVICE_HW_VERSION_H

#include <stdbool.h>
#include "comdef.h"

//To do, replace with System drivers API once available.
typedef enum
{
    CPR_FOUNDRY_TSMC,
    CPR_FOUNDRY_GF,
    CPR_FOUNDRY_SS,
    CPR_FOUNDRY_IBM,
    CPR_FOUNDRY_UMC,
} cpr_foundry_id;

typedef struct
{
    uint32      foundry_id;
    uint32      min_version;
    uint32      max_version;        //Non-inclusive
    uint16      min_cpr_fusing_rev;         
    uint16      max_cpr_fusing_rev;       //Non-inclusive
} cpr_config_foundry_range;

typedef struct
{
    const cpr_config_foundry_range*     foundry_range;
    uint32                              foundry_range_count;
} cpr_config_hw_version_range;

bool cpr_device_hw_version_matches_this_device(const cpr_config_hw_version_range* hw_range);

uint16 cpr_device_hw_get_cpr_fusing_rev(void);

#ifdef  RPM_CPR_BUILD

#include "Chipinfo.h"
#define CPR_CHIPINFO_VERSION CHIPINFO_VERSION

#elif defined(WINSIM)
/* TODO: Revise hardware version testing for PC build */
#define CPR_CHIPINFO_VERSION(x, y) x 

#else   //RPM_CPR_BUILD

#include "DDIChipInfo.h"
#define CPR_CHIPINFO_VERSION DALCHIPINFO_VERSION

#endif  //RPM_CPR_BUILD

#endif //CPR_DEVICE_HW_VERSION_H
