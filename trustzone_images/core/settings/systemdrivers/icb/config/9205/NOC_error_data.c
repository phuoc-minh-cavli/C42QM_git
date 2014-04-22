/*==============================================================================

FILE:      NOC_error_data.c

DESCRIPTION: This file contains target/platform specific configuration data.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
Edit History

//#CHANGE - Update when put in the depot
$Header: //components/rel/core.tz/2.1.3/settings/systemdrivers/icb/config/9205/NOC_error_data.c#1 $ 
$DateTime: 2019/05/23 05:43:45 $
$Author: pwbldsvc $
$Change: 19292165 $ 

When        Who    What, where, why
----------  ---    -----------------------------------------------------------
2015/05/07  tb     Port to 8952
2013/10/30  pm     Port to 8916
2012/10/03  av     Created
 
        Copyright (c) 2012-2018 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
#include "NOC_error.h"
#include "NOC_error_HWIO.h"
#include "HALhwio.h"

/*============================================================================
                        DEVICE CONFIG PROPERTY DATA
============================================================================*/

/* Internal NOC Configuration Data*/
static NOCERR_info_type NOCERR_cfgdata[] = 
{ 

  [0] = {
    .name            = "PC_NOC",
    .hw              = &QNOC_2_1,
    .base_addr       = (void *)HWIO_ADDR(PCNOC_OBS_ID_COREID),
    .intr_vector     = 226,
    .num_sbms        = 1,
    .sb_hw           = (NOC_sideband_hw_type *[]){ &QNOC_SB_2_12 },
    .sb_base_addrs   = (void *[]){ (void *)HWIO_ADDR(PCNOC_SBM_ID_COREID) },
    .num_tos         = 0,
    .to_addrs        = NULL,
    .syndrome        = { .sbms = (NOCERR_sbm_syndrome_type []){ {0, 0} }, },
  },
};

const char *const BIMC_reg_names[] = {
  "BIMC_S_DDR0_SCMO_INTERRUPT_STATUS",
  "BIMC_S_DDR0_SCMO_ESYN_ADDR",
  "BIMC_S_DDR0_SCMO_ESYN_APACKET_0",
  "BIMC_S_DDR0_SCMO_ESYN_APACKET_1",
  "BIMC_S_DDR0_SCMO_ESYN_APACKET_2",
  "BIMC_S_DEFAULT_SWAY_INTERRUPT_STATUS",
  "BIMC_S_DEFAULT_SWAY_DEFAULT_SLAVE_STATUS_0",
  "BIMC_S_DEFAULT_SWAY_DEFAULT_SLAVE_STATUS_1",
  "BIMC_S_DEFAULT_SWAY_DEFAULT_SLAVE_STATUS_2",
  "BIMC_BRIC_INTERRUPT_0_STATUS_0",
  "BIMC_BRIC_INTERRUPT_0_STATUS_1",
  "BIMC_BRIC_INTERRUPT_0_STATUS_2",
  "BIMC_BRIC_INTERRUPT_0_STATUS_3",
  "BIMC_BRIC_INTERRUPT_0_STATUS_4",
};

const uint64 BIMC_reg_addr[] = {
  HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_STATUS_ADDR,
  HWIO_BIMC_S_DDR0_SCMO_ESYN_ADDR_ADDR,
  HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_0_ADDR,
  HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_ADDR,
  HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_2_ADDR,
  HWIO_BIMC_S_DEFAULT_SWAY_INTERRUPT_STATUS_ADDR,
  HWIO_BIMC_S_DEFAULT_SWAY_DEFAULT_SLAVE_STATUS_0_ADDR,
  HWIO_BIMC_S_DEFAULT_SWAY_DEFAULT_SLAVE_STATUS_1_ADDR,
  HWIO_BIMC_S_DEFAULT_SWAY_DEFAULT_SLAVE_STATUS_2_ADDR,
  HWIO_BIMC_BRIC_INTERRUPT_0_STATUS_0_ADDR,
  HWIO_BIMC_BRIC_INTERRUPT_0_STATUS_1_ADDR,
  HWIO_BIMC_BRIC_INTERRUPT_0_STATUS_2_ADDR,
  HWIO_BIMC_BRIC_INTERRUPT_0_STATUS_3_ADDR,
  HWIO_BIMC_BRIC_INTERRUPT_0_STATUS_4_ADDR,
};

static void *clock_reg_addrs[] =
{
  (void *)HWIO_GCC_PCNOC_EXTREF_CTL_ADDR,
};

NOCERR_config_info_type NOCERR_propdata =
{
  .num_configs = 1,
  .configs = (NOCERR_propdata_type [])
  {
      [0] = 

{
          .family          = CHIPINFO_FAMILY_MDM9205,
          .match           = false,
          .version         = CHIPINFO_VERSION(1,0),
          /* NoC info. */
          .len             = sizeof(NOCERR_cfgdata)/sizeof(NOCERR_info_type),
          .NOCInfo         = NOCERR_cfgdata,
          .BIMC_num_reg    = sizeof(BIMC_reg_addr)/sizeof(BIMC_reg_addr[0]),
          .BIMC_reg_names  = BIMC_reg_names,
          .BIMC_reg_addr   = BIMC_reg_addr,
          .num_clock_regs  = sizeof(clock_reg_addrs)/sizeof(clock_reg_addrs[0]),
          .clock_reg_addrs = clock_reg_addrs,
        },
    },
};
