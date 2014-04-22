/*==============================================================================

FILE:      icbcfg_data.c

DESCRIPTION: This file implements the ICB Configuration driver.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
Edit History

$Header: //components/rel/core.tz/2.1.3/settings/systemdrivers/icb/config/9205/icbcfg_data.c#1 $ 
$DateTime: 2019/05/23 05:43:45 $
$Author: pwbldsvc $
$Change: 19292165 $ 

When        Who    What, where, why
----------  ---    -----------------------------------------------------------
2015/10/09  rc	   Updated the base address to support possible all DDR 
                   configurations.
2014/05/12  sds    Created
 
        Copyright (c) 2018 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
#include "icbcfg.h"
#include "icbcfgi.h"
#include "icbcfg_hwio.h"
#include "HALbimc.h"
#include "HALbimcHwioGeneric.h"

/*---------------------------------------------------------------------------*/
/*          Macro and constant definitions                                   */
/*---------------------------------------------------------------------------*/
/* DDR slave indexes */
#define SLAVE_DDR_CH0 0


/*============================================================================
                        DEVICE CONFIG PROPERTY DATA
============================================================================*/

/*---------------------------------------------------------------------------*/
/*          Properties data for device ID  = "icbcfg/boot"                   */
/*---------------------------------------------------------------------------*/

/* ICBcfg Boot Configuration Data*/

static icbcfg_data_type icbcfg_boot_data[] = 
{
    /* Add configuration data using
      ICBCFG_HWIO_*() or
      ICBCFG_RAW_*() macros below
      .
      .                          
      .                          */
    ICBCFG_DEFAULT(),
};

icbcfg_prop_type icbcfg_boot_prop = 
{
    /* Length of the config  data array */
    sizeof(icbcfg_boot_data)/sizeof(icbcfg_data_type), 
    /* Pointer to config data array */ 
    icbcfg_boot_data                                    
};

/* DDR map information. */
icbcfg_mem_region_type map_ddr_regions[1] =
{
  { 0x40000000ULL, SCL_DDR_MAX_ALLOWABLE_ADDR },
};

uint32 channel_map[1] = { SLAVE_DDR_CH0};

HAL_bimc_InfoType bimc_hal_info =
{
  (uint8_t *)BIMC_BASE_PHYS, /* Base address */
  19200,     /* QoS frequency */
  {
    0,
    0,
    0,
    0,
    0,
    0,
    3, /**< Number of segments for address decode. */
  }
};

/* Make sure the config region is always prohibited when "resetting" */
HAL_bimc_SlaveSegmentType safe_reset_seg =
{
  true,
  0x00000000ULL,                 /* start of config region */
  0x40000000ULL,                 /* 1 GB */
  BIMC_SEGMENT_TYPE_SUBTRACTIVE,
  BIMC_INTERLEAVE_NONE,
};

/*---------------------------------------------------------------------------*/
/*          Properties data for new Device IDs: Add below                    */
/*---------------------------------------------------------------------------*/
