/*==============================================================================

FILE:      ABT_OEM_data.c

DESCRIPTION: This file contains target/platform specific configuration data.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
Edit History

//#CHANGE - Update when put in the depot
$Header: //components/rel/core.tz/2.1.3/settings/systemdrivers/icb/config/9205/ABT_OEM_data.c#1 $ 
$DateTime: 2019/05/23 05:43:45 $
$Author: pwbldsvc $
$Change: 19292165 $ 

When        Who    What, where, why
----------  ---    -----------------------------------------------------------  
2015/09/14  ddk    Port for 8937
2015/05/07  tb     Port for 8952
2014/12/09  tb     Created
 
        Copyright (c) 2014-2018 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
#include "ABTimeout.h"
#include "ABTimeout_HWIO.h"

/*============================================================================
                      TARGET AND PLATFORM SPECIFIC DATA
============================================================================*/

/*============================================================================
                        DEVICE CONFIG PROPERTY DATA
============================================================================*/

/* OEM ABT Configuration Data*/
const static ABT_slave_info_type_oem ABT_cfgdata_oem[] = 
{ 
//ABT_SLAVE_INFO_OEM(name,     sl_en,  int_en, to_val)
  ABT_SLAVE_INFO_OEM(PCNOC_0,   TRUE,   TRUE,   0xFF),
  ABT_SLAVE_INFO_OEM(PCNOC_1,   TRUE,   TRUE,   0xFF),
  ABT_SLAVE_INFO_OEM(PCNOC_2,   TRUE,   TRUE,   0xFF),
  ABT_SLAVE_INFO_OEM(PCNOC_3,   TRUE,   TRUE,   0xFF),
  ABT_SLAVE_INFO_OEM(PCNOC_5,   TRUE,   TRUE,   0xFF),
  ABT_SLAVE_INFO_OEM(PCNOC_4,   TRUE,   TRUE,   0xFF),
  ABT_SLAVE_INFO_OEM(PCNOC_6,   TRUE,   TRUE,   0xFF),
  ABT_SLAVE_INFO_OEM(PCNOC_7,   TRUE,   TRUE,   0xFF),
};

/* OEM ABT Configuration Property Data*/
const ABT_propdata_type_oem ABT_propdata_oem =  
{
    /* Length of the OEM config data array */
    sizeof(ABT_cfgdata_oem)/sizeof(ABT_slave_info_type_oem), 
    /* Pointer to OEM config data array */ 
    ABT_cfgdata_oem                                 
};

