/*==============================================================================
  FILE:         rsc_hal_cmn.c
  
  OVERVIEW:     This file implements HAL APIs exposed to rsc driver and performs
                actual HW register read and write in island section if supported
 
  DEPENDENCIES: None

                Copyright (c) 2017-2018 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.mpss/3.10/power/rsc/hal/src/rsc_hal_cmn.c#1 $
$DateTime: 2018/12/28 00:36:13 $
==============================================================================*/
#include "comdef.h"
#include "rsc_hwio.h"

/*===========================================================================
 *                            EXTERNAL FUNCTIONS
 *==========================================================================*/
/*
 * rsc_hal_set_trigger_start_address
 */
void rsc_hal_set_trigger_start_address(uint32 rsc_base, uint16 addr)
{
  RSC_HWIO_OUTX(rsc_base, RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0, addr);
}

/* 
 * rsc_hal_seq_busy
 */
uint32 rsc_hal_seq_busy(uint32 rsc_base)
{
  return RSC_HWIO_INX(rsc_base, RSC_SEQ_BUSY_DRV0);
}

/*
 * rsc_hal_set_trigger_seq
 */
void rsc_hal_set_trigger_seq(uint32 rsc_base, uint8 val)
{
  RSC_HWIO_OUTX(rsc_base, RSC_SEQ_OVERRIDE_TRIGGER_DRV0, val);
}

/* 
 * rsc_hal_get_ts_unit_l
 */
uint32 rsc_hal_get_ts_unit_l(uint32 base, uint8 unit_num)
{
  return RSC_HWIO_INXFI2(base, RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd, 0, unit_num, TS_DATA_L);
}  

/* 
 * rsc_hal_get_ts_unit_h
 */
uint32 rsc_hal_get_ts_unit_h(uint32 base, uint8 unit_num)
{
  return RSC_HWIO_INXFI2(base, RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd, 0, unit_num, TS_DATA_H);
}

/* 
 * rsc_hal_get_ts_overflow
 */
uint32 rsc_hal_get_ts_overflow(uint32 base, uint8 unit_num)
{
  return RSC_HWIO_INXFI2(base, RSC_TIMESTAMP_UNITm_OUTPUT_DRVd, 0, unit_num, TS_OVERFLOW);
}

/*
 * rsc_hal_get_ts_valid
 */
uint32 rsc_hal_get_ts_valid(uint32 base, uint8 unit_num)
{
  return RSC_HWIO_INXFI2(base, RSC_TIMESTAMP_UNITm_OUTPUT_DRVd, 0, unit_num, TS_VALID);
}
