#ifndef _BOOT_DDI_TARGET_H
#define _BOOT_DDI_TARGET_H
/*===========================================================================

                    BOOT DDI TARGET DECLARATIONS

DESCRIPTION
  Contains target specific code for Boot DDR Debug Image tool.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright 2016 by Qualcomm Technologies, Inc.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when        who      what, where, why
--------    ---      ----------------------------------------------------------
20/12/2016  zhz     Added the target related function
09/14/2016  jh       Initial file creation
===========================================================================*/
#include "ddr_common.h"
#include "HALhwio.h"
#include "ClockBoot.h"
#include <stdlib.h>

#ifdef CDC_DEFAULT_EACH
#define CDC_DEFAULT_RD_DQ0   0xFA
#define CDC_DEFAULT_RD_DQ1   0xFA
#define CDC_DEFAULT_RD_DQ2   0xFA
#define CDC_DEFAULT_RD_DQ3   0xFA

#define CDC_DEFAULT_WR_DQ0   0xFA
#define CDC_DEFAULT_WR_DQ1   0xFA
#define CDC_DEFAULT_WR_DQ2   0xFA
#define CDC_DEFAULT_WR_DQ3   0xFA
#else
#define CDC_DEFAULT_RD_DQ    0xFA
#define CDC_DEFAULT_WR_DQ    0xFA
#endif

#define CDC_DEFAULT_CA       0x174

#define CDC_STEP_DQ          15
#define CDC_STEP_CA          15

#define BIT_ERROR_STATISTICS

typedef struct {
  uint32 ClockLevels;
  uint32 *ClockFreqKHz;
  uint32 mc_fmax;
  uint32 ddr_fmax;
}clock_info_t, *clock_info_p;

typedef struct{
  uint32 cur_clocklevel;
  uint32 cur_vref;
  uint32 cur_rout;
  uint32 cur_byte;
  uint32 cur_rwtype;
  uint32 cur_startaddr;
  uint32 cur_testsize;
}arg_dqshmoo, *arg_dqshmoo_p;

typedef struct{
  uint32 cur_clocklevel;
  uint32 cur_vref;
}arg_cashmoo, *arg_cashmoo_p;
/*==============================================================================
                                  Function
==============================================================================*/
void ddr_debug_output(const char *msg);
void  ddr_write_pattern(const uint32 base,uint32 test_size,uint32 *val);
boolean ddr_read_pattern(const uint32 base,uint32 test_size,uint32 *val);

//void ddr_debug_do_tuning(int lineType, int rwType, boolean is_training); // will be deprecated

void* ddr_debug_get_clock_info(void); // return clock_info_p
boolean ddr_debug_dq_cdc_sweep(void* inArgs); // input arg_dqshmoo_p
boolean ddr_debug_ca_cdc_sweep(void* inArgs); //input arg_cashmoo_p

boolean sbl1_ddr_debug_set_rout(uint32 base, SDRAM_INTERFACE interface, uint32 rout, uint32 byte);
void ddr_tuning_log_out(uint32 failures[4],uint32 tempresult[4],uint16 CDC_DELAY_START,uint16 WRITE_CDC_DELAY_MAX);
void displayData(void * buffer, uint32 size, void* tArgs, void *training_params_ptr);
void changeDRAMSettings(uint32 clk_in_khz); 

#ifdef FEATURE_BOOT_EXTERN_CLK_COMPLETED
boolean boot_clock_set_bimcspeed(uint32 nFreqKHz);
#else
 static __inline boolean boot_clock_set_bimcspeed(uint32 nFreqKHz)
 {
  return TRUE;
 }
#endif

#ifdef FEATURE_BOOT_EXTERN_CLK_COMPLETED
void boot_query_bimc_clock(ClockQueryType nQuery,void* pResource);
#else
 static __inline void boot_query_bimc_clock(ClockQueryType nQuery,void* pResource)
 {
 }
#endif

#endif