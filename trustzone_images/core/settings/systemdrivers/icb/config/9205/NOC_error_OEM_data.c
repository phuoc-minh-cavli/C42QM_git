/*==============================================================================

FILE:      NOC_error_OEM_data.c

DESCRIPTION: This file contains target/platform specific configuration data.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
Edit History

//#CHANGE - Update when put in the depot
$Header: //components/rel/core.tz/2.1.3/settings/systemdrivers/icb/config/9205/NOC_error_OEM_data.c#1 $ 
$DateTime: 2019/05/23 05:43:45 $
$Author: pwbldsvc $
$Change: 19292165 $ 

When        Who    What, where, why
----------  ---    -----------------------------------------------------------
2015/09/15  ddk   Disabling error fatal for bringup.
2015/05/07  tb     Port to 8952
2014/12/09  tb     Created
 
        Copyright (c) 2014-2018 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
#include "NOC_error.h"
#include "NOC_error_HWIO.h"

/*============================================================================
                           TARGET SPECIFIC DATA
============================================================================*/

/*============================================================================
                        DEVICE CONFIG PROPERTY DATA
============================================================================*/

/* OEM NOC Configuration Data*/
NOCERR_info_type_oem NOCERR_cfgdata_oem[] = 
{ 

  [0] = {
    .name        = "PCNOC",
    .intr_enable = true,
    .error_fatal = true,
    .sbms        = (NOCERR_sbm_info_type_oem []){ 
        [0] = { .faultin_en0_low  = 0x1F},
      },
    .obs_mask    = (NOCERR_sbm_info_type_oem []){ 
        [0] = { .faultin_en0_low = 0x1}, 
      },
    .to_reg_vals = NULL,
  },
};
uint32_t clock_reg_vals[] =
{
  [0] = 0x30001, /* Enable timeout reference clock and set div value*/
};

NOCERR_config_info_type_oem NOCERR_propdata_oem =
{
  .num_configs = 1,
  .configs = (NOCERR_propdata_type_oem [])
    {
      /* Target info: SDM670 v1.0 */
      [0] = 
        {
          .family          = CHIPINFO_FAMILY_MDM9205,
          .match           = false,
          .version         = CHIPINFO_VERSION(1,0),

          .len            = sizeof(NOCERR_cfgdata_oem)/sizeof(NOCERR_info_type_oem),
          .NOCInfoOEM     = NOCERR_cfgdata_oem,
          .clock_reg_vals = clock_reg_vals,
        },
    },
};
