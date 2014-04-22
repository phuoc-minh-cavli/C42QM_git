/*===========================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#include "cpr_device_hw_version.h"
#include "cpr_logging.h"
#include "cpr_image.h"
#include "DDIChipInfo.h"

uint32 cpr_chip_version;
cpr_foundry_id cpr_foundry;
uint16 cpr_fusing_version;

void cpr_device_hw_version_init(void)
{
    static bool cpr_device_hw_version_init_done = false;
    if(cpr_device_hw_version_init_done)
    {
        return;
    }
    cpr_device_hw_version_init_done = true;

    cpr_fuses_individual_fuse_init(&cpr_bsp_fusing_rev_info);
    cpr_fusing_version = cpr_bsp_fusing_rev_info.cpr_fuse_value;

    if (DalChipInfo_ChipFamily() == DALCHIPINFO_FAMILY_MSM8996SG)
    {
        cpr_fusing_version = 4;
    }

    CPR_LOGGING(1, "CPR Fusing Version is %d", cpr_fusing_version);

    cpr_chip_version = cpr_image_get_chip_info_version();
    CPR_LOGGING(1, "CPR Chip Version is %x", cpr_chip_version);

    cpr_foundry = cpr_image_get_foundry_info();
}

uint16 cpr_device_hw_get_cpr_fusing_rev(void)
{
    cpr_device_hw_version_init();
    return cpr_fusing_version;
}

bool cpr_device_hw_version_matches_this_device(const cpr_config_hw_version_range* hw_range)
{
    cpr_device_hw_version_init();

    for(int j=0; j<hw_range->foundry_range_count; j++)
    {
        const cpr_config_foundry_range* foundry_range = &hw_range->foundry_range[j];
        if((cpr_foundry==foundry_range->foundry_id) &&
           (cpr_chip_version>=foundry_range->min_version) &&
           (cpr_chip_version<foundry_range->max_version))
        {
            if ((cpr_fusing_version >= foundry_range->min_cpr_fusing_rev) &&
                (cpr_fusing_version < foundry_range->max_cpr_fusing_rev))
            {
                return true;
            }
        }
    }
    return false;
}

