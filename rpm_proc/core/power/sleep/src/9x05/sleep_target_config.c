/*============================================================================
  FILE:         sleep_target_config.c

  OVERVIEW:     This file provides target-specific functionality for the RPM.

  DEPENDENCIES: None

                Copyright (c) 2011 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary

  $Header: //components/rel/rpm.bf/2.1.3/core/power/sleep/src/9x05/sleep_target_config.c#4 $
  $DateTime: 2019/06/14 03:43:13 $
  $Author: pwbldsvc $
============================================================================*/


#include "msmhwio.h"
#include "msmhwiobase.h"
#include "qfprom_pte_hwio.h"

// retention programmed in uV ( 600000uV = 0.6V )
static const uint32 vddcx_pvs_retention_data[8] =
{
	//TODO
  /* 000 */ 600000,
  /* 001 */ 450000, 
  /* 010 */ 600000, 
  /* 011 */ 600000, 
  /* 100 */ 600000, 
  /* 101 */ 600000, 
  /* 110 */ 600000,
  /* 111 */ 600000
};

// retention programmed in uV ( 600000uV = 0.6V )
static const uint32 vddmx_pvs_retention_data[4] =
{
  /* 00 */ 700000,
  /* 01 */ 650000, 
  /* 10 */ 600000, 
  /* 11 */ 550000, 
};

void set_vdd_mem_sleep_voltage (uint32 *vdd_mem)
{

  /* 3 MSB bits contain the values we need */
  uint32 pte_masked_val = HWIO_INF(QFPROM_CORR_PTE_ROW1_MSB, MX_RET_BIN);
//  uint32 lookup_val = pte_masked_val;
  *vdd_mem = vddmx_pvs_retention_data[pte_masked_val];
}
void set_vdd_dig_sleep_voltage (uint32 *vdd_dig)
{
  uint32 pte_masked_val; 	
  *vdd_dig = 650000;

  /* 3 MSB bits contain the values we need */
  pte_masked_val = HWIO_INF(QFPROM_CORR_PTE_ROW1_MSB, LOGIC_RETENTION);
 // uint32 lookup_val = pte_masked_val >> 5;
  *vdd_dig = vddcx_pvs_retention_data[pte_masked_val];
}

