/*==============================================================================

FILE:      BIMC_data.c

DESCRIPTION: This file contains target/platform specific configuration data.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
Edit History

//#CHANGE - Update when put in the depot
$Header: //components/rel/core.tz/2.1.3/settings/systemdrivers/icb/config/9205/BIMC_error_data.c#1 $ 
$DateTime: 2019/05/23 05:43:45 $
$Author: pwbldsvc $
$Change: 19292165 $ 

When        Who    What, where, why
----------  ---    -----------------------------------------------------------
2015/05/07  tb     Split Internal and OEM data
2014/06/06  tb     Created
 
        Copyright (c) 2014-2018 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
#include "BIMC_error.h"
#include "BIMC_error_HWIO.h"

/*============================================================================
                           TARGET SPECIFIC DATA
============================================================================*/

#define BIMC_INTERRUPT_VECTOR 211

#define BIMC_GLOBAL2_BASE_ADDR BIMC_GLOBAL2_REG_BASE

#define BIMC_DEFAULT_SWAY_BASE_ADDR BIMC_S_DEFAULT_SWAY_REG_BASE

#define BIMC_DDR0_SCMO_BASE_ADDR BIMC_S_DDR0_SCMO_REG_BASE

/*============================================================================
                        DEVICE CONFIG PROPERTY DATA
============================================================================*/

/* NOC Configuration Data*/
const static BIMC_BRIC_info_type BRIC_info[] = 
{
//  {name, base_addr, intr_status, intr_clear, intr_enable}
    {"BRIC INTERRUPT 1", BIMC_GLOBAL2_REG_BASE, 0x110, 0x118, 0x11c }
};

const static BIMC_DEFAULT_info_type DEFAULT_info[] =
{
//  {name, base_addr, intr_status, intr_clear, intr_enable, intr_status0, intr_status1, intr_status2}
    {"DEFAULT SWAY INTERRUPT", BIMC_S_DEFAULT_SWAY_REG_BASE, 0x100, 0x108, 0x10c, 0x400, 0x410, 0x420 }
};

const static BIMC_DDR_SCMO_info_type DDR_SCMO_info[] =
{
//  {name, base_addr, intr_status, intr_clear, intr_enable, intr_addr, intr_addr1, intr_packet0, intr_packet1, intr_packet2}
    {"DDR0 SCMO INTERRUPT", BIMC_S_DDR0_SCMO_REG_BASE, 0x100, 0x108, 0x10c, 0x120, REG_NOT_AVAIL, 0x128, 0x12c, 0x130 }
};

const BIMC_propdata_type BIMC_propdata =
{
    BIMC_INTERRUPT_VECTOR,
    sizeof(BRIC_info)/sizeof(BIMC_BRIC_info_type), 
    BRIC_info,
    sizeof(DEFAULT_info)/sizeof(BIMC_DEFAULT_info_type), 
    DEFAULT_info,
    sizeof(DDR_SCMO_info)/sizeof(BIMC_DDR_SCMO_info_type), 
    DDR_SCMO_info,
	0,
};
