#ifndef __DDR_DEBUG_PHY_V1_H__
#define __DDR_DEBUG_PHY_V1_H__

/*==============================================================================
                   Copyright 2012-2013 Qualcomm Technologies Incorporated
                              All Rights Reserved
                     Qualcomm Confidential and Proprietary
==============================================================================*/

/*==============================================================================
                                  INCLUDES
==============================================================================*/
#include "ddr_common.h"

/*==============================================================================
                                  Macro
==============================================================================*/
//#define CDC_DEFAULT_EACH

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

void ddr_debug_do_tuning(int lineType, int rwType, boolean is_training); // will be deprecated

void* ddr_debug_get_clock_info(void); // return clock_info_p
boolean ddr_debug_dq_cdc_sweep(void* inArgs); // input arg_dqshmoo_p
boolean ddr_debug_ca_cdc_sweep(void* inArgs); //input arg_cashmoo_p


#endif
