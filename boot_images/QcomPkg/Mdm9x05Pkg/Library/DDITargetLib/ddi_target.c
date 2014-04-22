/*===========================================================================

                    BOOT DDI TARGET DEFINITIONS

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
02/25/2019  ds      DDI framwork re-architecture for Twizy
01/03/2017  zhz     Make an SCM call to unlock the TZ region in OCIMEM.
20/12/2016  zhz     Added the target related function
09/14/2016  jh       Initial file creation
===========================================================================*/

#include "ddi_target.h"
#include "ddi_firehose.h"
#include "boot_cache_mmu.h"
#include "boothw_target.h"
#include "busywait.h"
#include "pm_ldo.h"
#include "boot_fastcall_tz.h"
#include "stdio.h"
#include "string.h"
#include "DDR_PHY.h"
#include "ddi_firehose.h"
#include "ddr_test.h"
#include "ddr_phy_training.h"
#include "boot_extern_clk_interface.h"
#include "ddr_params.h"

#define MAX_CLOCK_FREQ 15
#define VALID_CDC_START   270
extern uint32 ddr_type;
static clock_info_t g_clkInfo = {0, NULL,0,0};
extern char pattern_name[][20];
char str_error_log[BUFFER_LENGTH]={0};
int32 clock_index = -1;
uint32 failures[4];
uint32 error_statistics = 0;
uint32 dq_rout, dq_pslew, dq_nslew, read_cdc_delay, write_cdc_delay;
uint32 start_address;
uint32 test_size;
uint8 whichByte = 0;
boolean rdcdc_scan = FALSE;
DDR_TEST_PATTERN PATTERN;

#ifdef CDC_DEFAULT_EACH
static uint32 cdc_default_read[4] = {CDC_DEFAULT_RD_DQ0,
                                     CDC_DEFAULT_RD_DQ1,
                                     CDC_DEFAULT_RD_DQ2,
                                     CDC_DEFAULT_RD_DQ3};
static uint32 cdc_default_write[4] = {CDC_DEFAULT_WR_DQ0,
                                      CDC_DEFAULT_WR_DQ1,
                                      CDC_DEFAULT_WR_DQ2,
                                      CDC_DEFAULT_WR_DQ3};
#else
static uint32 cdc_default_read[4] = {CDC_DEFAULT_RD_DQ,
                                     CDC_DEFAULT_RD_DQ,
                                     CDC_DEFAULT_RD_DQ,
                                     CDC_DEFAULT_RD_DQ};
static uint32 cdc_default_write[4] = {CDC_DEFAULT_WR_DQ,
                                      CDC_DEFAULT_WR_DQ,
                                      CDC_DEFAULT_WR_DQ,
                                      CDC_DEFAULT_WR_DQ};
#endif

void ddr_debug_output(const char *msg)
{
} /* ddr_debug_output */


void* ddr_debug_get_clock_info(void)
{
  uint32 nNumLevel = 0, ii = 0;
  ClockPlanType *pClockPlan;

  //if(0 != g_clkInfo.ClockLevels && NULL != g_clkInfo.ClockLevels) return &g_clkInfo;

  /* Call BIMCQuery to get the Num Perf Levels */
   Clock_BIMCQuery(CLOCK_RESOURCE_QUERY_NUM_PERF_LEVELS,&nNumLevel);

  DALSYS_Malloc(nNumLevel * sizeof(ClockPlanType), (void **)&pClockPlan);
  if(NULL == pClockPlan) return NULL;

  /* Call BIMCQuery to get the Clock Plan */
  Clock_BIMCQuery(CLOCK_RESOURCE_QUERY_ALL_FREQ_KHZ,pClockPlan);

  g_clkInfo.ClockLevels = nNumLevel;
  
  DALSYS_Malloc(nNumLevel * sizeof(uint32), (void **)&g_clkInfo.ClockFreqKHz);
  if(NULL == g_clkInfo.ClockFreqKHz) return NULL;

  for(ii = 0; ii < nNumLevel; ii++){
    g_clkInfo.ClockFreqKHz[ii] = pClockPlan[ii].nFreqKHz * DDR_CLOCK_RATIO;
  }

  g_clkInfo.mc_fmax = pClockPlan[nNumLevel-1].nFreqKHz;
  g_clkInfo.ddr_fmax = pClockPlan[nNumLevel-1].nFreqKHz * DDR_CLOCK_RATIO;

  // invalid clock_index input, reset to -1
  if(clock_index < 0 || clock_index > g_clkInfo.ClockLevels - 1){
    clock_index = -1;
  }

  DALSYS_Free(pClockPlan);
  
  return &g_clkInfo;
}

void ddr_debug_tuning_init(void)
{
  ddr_debug_get_clock_info();
}

boolean sbl1_ddr_debug_set_rout(uint32 base, SDRAM_INTERFACE interface, uint32 rout, uint32 byte)
{
  uint32 phy_offset;
  uint32 offset;

  /* PHY offset */
  phy_offset = base + ((interface == SDRAM_INTERFACE_0) ? PHY_0_OFFSET : PHY_1_OFFSET);

  if(0 != byte && 1 != byte && 2 != byte && 3 != byte) return FALSE;

  /* Offset for each DQ PHY */
  offset = phy_offset + PHY_DQ1_OFFSET * byte;

  /* Update DQ read CDC offset */
  BOOT_HWIO_OUTM(PHY_DQ_ADDR(PAD_CFG0),
                 offset,
                 HWIO_FMSK(PHY_DQ_ADDR(PAD_CFG0), DQ_ROUT),
                 rout << HWIO_SHFT(PHY_DQ_ADDR(PAD_CFG0), DQ_ROUT) );

  return TRUE;
}

void ddr_tuning_log_out(uint32 failures[4],uint32 tempresult[4],uint16 CDC_DELAY_START,uint16 WRITE_CDC_DELAY_MAX)
{
  memset(str,0,BUFFER_LENGTH);
  if(0 == failures[0] && 0 == failures[1] && 0 == failures[2] && 0 == failures[3]) {
    snprintf(str, BUFFER_LENGTH,
             "%4u, %4u, %4u, "
             "%3u, %3u, %3u, %3u.[log]%s\r\n",
             dq_rout, read_cdc_delay, write_cdc_delay,
             failures[0], failures[1], failures[2], failures[3],"[stress test]PASS");
  }
  else {
    snprintf(str, BUFFER_LENGTH,
             "%4u, %4u, %4u, "
             "%3u, %3u, %3u, %3u.[log]%s\r\n",
             dq_rout, read_cdc_delay, write_cdc_delay,
             failures[0], failures[1], failures[2], failures[3],str_error_log);
  }
  ddi_firehose_print_log(str);
  memset(str_error_log,0,BUFFER_LENGTH);
  tempresult[0]= failures[0];
  tempresult[1]= failures[1];
  tempresult[2]= failures[2];
  tempresult[3]= failures[3];
}

void  ddr_write_pattern(const uint32 base, uint32 test_size, uint32 *val)
{
  volatile uint32 * base_addr = (uint32 *)base;
  uint32 ii = 0;
  uint32 value;
  uint32 ddi_ddr_freq;
  ddr_interface_state ddr_status;

  value = *val;
  ddr_status = ddr_get_status();

  ddi_ddr_freq = -1 == clock_index ? 
                 g_clkInfo.ddr_fmax :
                 g_clkInfo.ClockFreqKHz[clock_index];

  if(rdcdc_scan == TRUE)
  {
    /* Clock API to scale to low speed */
    HAL_SDRAM_BIMC_Update_Clock_Period(0, ddr_status.clk_speed, ddr_divide_func, ddr_type);
    HAL_SDRAM_DPE_Pre_Clock_Switch(0, SDRAM_INTERFACE_0, 0, ddr_status.clk_speed,ddr_type);    
    boot_Clock_SetBIMCSpeed(ddr_status.clk_speed);  
    HAL_SDRAM_DPE_Post_Clock_Switch(0, SDRAM_INTERFACE_0, 0, ddr_status.clk_speed,ddr_type);
  }
  else
  {
    /* Clock API to scale to high speed */
    HAL_SDRAM_BIMC_Update_Clock_Period(0, ddi_ddr_freq, ddr_divide_func, ddr_type);
    HAL_SDRAM_DPE_Pre_Clock_Switch(0, SDRAM_INTERFACE_0, 0, ddi_ddr_freq, ddr_type);     
    boot_Clock_SetBIMCSpeed(ddi_ddr_freq);
    HAL_SDRAM_DPE_Post_Clock_Switch(0, SDRAM_INTERFACE_0, 0, ddi_ddr_freq, ddr_type);

    HAL_SDRAM_DDR_PHY_Update_WR_CDC_Config(0, SDRAM_INTERFACE_0, write_cdc_delay, whichByte);
  }

  switch(PATTERN)
  {
    case BITFLIP_PATTERN:
    case CHECK_BOARD_PATTERN:
    case BITSPREAD_PATTERN:
    case SOLID_BITS_PATTERN:
      for (ii = 0; ii <= test_size; ii++){
        base_addr[ii] = (ii % 2) == 0 ? value : ~value;
      }
      break;
    case WORKING_ONE_PATTERN:
    case WORKING_ZERO_PATTERN:
      for (ii = 0; ii <= test_size; ii++){
        base_addr[ii] = value;
      }
      break;
    case SEQ_INR_PARTTERN:
      for (ii = 0; ii <= test_size; ++ii){
        base_addr[ii] = value;
        ++value;
      }
      break;
    case CUSTOMER_PARTTERN:
      break;
	case DEFAULT_PATTERN:
	  break;
	case INVALID_PARTTERN:
	  break;
  }
  
}

boolean ddr_read_pattern(const uint32 base,uint32 test_size,uint32 *val)
{
  volatile uint32 * base_addr = (uint32 *)base;
  uint32 ii = 0;
  uint32 value;
  uint32 temp_value = 0;
  uint32 expect_value = 0;
  uint32 result = 0;
  uint32 idx = 0;
  uint32 ddi_ddr_freq;
  ddr_interface_state ddr_status;

  value = *val;
  ddr_status = ddr_get_status();

  ddi_ddr_freq = -1 == clock_index ? 
                 g_clkInfo.ddr_fmax :
                 g_clkInfo.ClockFreqKHz[clock_index];
  
  if(rdcdc_scan == TRUE)
  {
    /* clock api to restore to high speed */
    HAL_SDRAM_BIMC_Update_Clock_Period(0, ddi_ddr_freq, ddr_divide_func, ddr_type);
    HAL_SDRAM_DPE_Pre_Clock_Switch(0, SDRAM_INTERFACE_0, 0, ddi_ddr_freq, ddr_type);     
    boot_Clock_SetBIMCSpeed(ddi_ddr_freq);
    HAL_SDRAM_DPE_Post_Clock_Switch(0, SDRAM_INTERFACE_0, 0, ddi_ddr_freq, ddr_type); 

    HAL_SDRAM_DDR_PHY_Update_RD_CDC_Config(0, SDRAM_INTERFACE_0, read_cdc_delay, whichByte);
  }
  else
  {
    /* Clock API to scale to low speed */
    HAL_SDRAM_BIMC_Update_Clock_Period(0, ddr_status.clk_speed, ddr_divide_func,ddr_type);
    HAL_SDRAM_DPE_Pre_Clock_Switch(0, SDRAM_INTERFACE_0, 0, ddr_status.clk_speed, ddr_type);    
    boot_Clock_SetBIMCSpeed(ddr_status.clk_speed);  
    HAL_SDRAM_DPE_Post_Clock_Switch(0, SDRAM_INTERFACE_0, 0, ddr_status.clk_speed, ddr_type);
  }

  switch(PATTERN)
  {
    case BITFLIP_PATTERN:
    case CHECK_BOARD_PATTERN:
    case BITSPREAD_PATTERN:
    case SOLID_BITS_PATTERN:
      for (ii = 0; ii <= test_size; ii++)
      {
        temp_value = base_addr[ii];
        expect_value = (ii % 2) == 0 ? value : ~value;
        if (temp_value != expect_value)
        {
          result = temp_value ^ expect_value;
#ifdef BIT_ERROR_STATISTICS
          error_statistics |= result;
          if(rdcdc_scan == TRUE)
            snprintf(str_error_log,BUFFER_LENGTH, "[%s]read failed at address:0x%x, error result:0x%08x, expect result:0x%08x", 
                     pattern_name[PATTERN],(base_addr+ii), error_statistics, 0x0);
          else
            snprintf(str_error_log,BUFFER_LENGTH, "[%s]write failed at address:0x%x, error result:0x%08x, expect result:0x%08x", 
                     pattern_name[PATTERN],(base_addr+ii), error_statistics, 0x0);
#else
          if(rdcdc_scan == TRUE)
            snprintf(str_error_log,BUFFER_LENGTH, "[%s]read failed at address:0x%x, error data:0x%08x, expect data:0x%08x", 
                     pattern_name[PATTERN],(base_addr+ii), temp_value, expect_value);
          else
            snprintf(str_error_log,BUFFER_LENGTH, "[%s]write failed at address:0x%x, error data:0x%08x, expect data:0x%08x", 
                     pattern_name[PATTERN],(base_addr+ii), temp_value, expect_value);
#endif
          for (idx = 0; idx < 4; idx++) {
            if( result & (0xFF << (idx*8)) ) failures[idx]++;
          }
        }
      }
      break;
    case WORKING_ONE_PATTERN:
    case WORKING_ZERO_PATTERN:
      /* verify */
      for (ii = 0; ii <= test_size; ii++)
      {
        temp_value = base_addr[ii];
        if (temp_value != value)
        {
          result = temp_value ^ value;
#ifdef BIT_ERROR_STATISTICS
          error_statistics |= result;
          if(rdcdc_scan == TRUE)
            snprintf(str_error_log,BUFFER_LENGTH, "[%s]read failed at address:0x%x, error result:0x%08x, expect result:0x%08x", 
                     pattern_name[PATTERN],(base_addr+ii), error_statistics, 0x0);
          else
            snprintf(str_error_log,BUFFER_LENGTH, "[%s]write failed at address:0x%x, error result:0x%08x, expect result:0x%08x", 
                     pattern_name[PATTERN],(base_addr+ii), error_statistics, 0x0);
#else
          if(rdcdc_scan == TRUE)
            snprintf(str_error_log,BUFFER_LENGTH, "[%s]read failed at address:0x%08x, error data:0x%08x, expect data:0x%08x\r\n", 
                     pattern_name[PATTERN],base_addr + ii, base_addr[ii], ((ii % 2) == 0 ? value : ~value));
          else
            snprintf(str_error_log,BUFFER_LENGTH, "[%s]write failed at address:0x%08x, error data:0x%08x, expect data:0x%08x\r\n", 
                     pattern_name[PATTERN],base_addr + ii, base_addr[ii], ((ii % 2) == 0 ? value : ~value));
#endif
          for (idx = 0; idx < 4; idx++) {
            if ( result & (0xFF << (idx*8)) ) failures[idx]++;
          }
        }
      }
      break;
    case SEQ_INR_PARTTERN:
      for (ii = 0; ii <= test_size; ++ii)
      {
        temp_value = base_addr[ii];
        if (temp_value != value)
        {
          result = temp_value ^ value;
#ifdef BIT_ERROR_STATISTICS
          error_statistics |= result;
          if(rdcdc_scan == TRUE)
            snprintf(str_error_log,BUFFER_LENGTH, "[%s]read failed at address:0x%x, error result:0x%08x, expect result:0x%08x", 
                     pattern_name[PATTERN],(base_addr+ii), error_statistics, 0x0);
          else
            snprintf(str_error_log,BUFFER_LENGTH, "[%s]write failed at address:0x%x, error result:0x%08x, expect result:0x%08x", 
                     pattern_name[PATTERN],(base_addr+ii), error_statistics, 0x0);
#else
          if(rdcdc_scan == TRUE)
            snprintf(str_error_log,BUFFER_LENGTH, "[%s]read failed at address:address:0x%08x, error data:0x%08x, expect data:0x%08x\r\n", 
                     pattern_name[PATTERN], base_addr + ii, base_addr[ii], value);
          else
            snprintf(str_error_log,BUFFER_LENGTH, "[%s]write failed at address:address:0x%08x, error data:0x%08x, expect data:0x%08x\r\n", 
                     pattern_name[PATTERN],  base_addr + ii, base_addr[ii], value);
#endif
          for (idx = 0; idx < 4; idx++) {
            if ( result & (0xFF << (idx*8)) ) failures[idx]++;
          }
        }
        ++value;
      }
      break;
	case CUSTOMER_PARTTERN:
      break;
	case DEFAULT_PATTERN:
	  break;
	case INVALID_PARTTERN:
	  break;
  }

  /* verify */
#ifdef ONLY_TARGET_FAIL
  if(failures[whichByte]) return FALSE;
#else
  if(failures[0]||failures[1]||failures[2]||failures[3]) return FALSE;
#endif
  
  return TRUE;
  
}

boolean ddr_debug_dq_cdc_sweep(void* inArgs)
{
  arg_dqshmoo_p tmpDQArgs;
  uint32 ddi_ddr_freq;
  uint8 sweep;
  uint32 max_sweep=0, loop_count=0;
  uint32 training_period_1ps=0;
  static uint32 tempresult[4]={0xFF};
  ddr_size_info ddr_info;
  uint32 dq = 0;
#ifdef DEBUG_PHY_V1
  uint32 curr_delay;
#endif

  if(NULL == inArgs) return FALSE;
  tmpDQArgs = (arg_dqshmoo_p)inArgs;

  // not good for following codes, may change in future
  clock_index = tmpDQArgs->cur_clocklevel;
  dq_rout = tmpDQArgs->cur_rout;
  whichByte = tmpDQArgs->cur_byte;
  start_address = tmpDQArgs->cur_startaddr;
  test_size = tmpDQArgs->cur_testsize;

  ddr_info = ddr_get_size();

  ddr_debug_tuning_init();

  ddi_ddr_freq = -1 == clock_index ?
                 g_clkInfo.ddr_fmax :
                 g_clkInfo.ClockFreqKHz[clock_index];

  training_period_1ps = 1000000000 / ddi_ddr_freq;
  max_sweep = training_period_1ps / 2;

  if (max_sweep < 1000)
  	 max_sweep = 1000;

  for(sweep = 0; sweep < MAX_COARSE_STEP; sweep++) {
    if(cdc_lut[sweep]> max_sweep) break;
  }
  max_sweep = sweep;
//  max_sweep = MAX_COARSE_STEP;

  snprintf(str, BUFFER_LENGTH, "$Clock(%4u, 0, %4u)\r\n", ddi_ddr_freq, ddi_ddr_freq);
  ddi_firehose_print_log(str);

  snprintf(str, BUFFER_LENGTH, "$Vref(%4u, 0, %4u)\r\n", tmpDQArgs->cur_vref, tmpDQArgs->cur_vref);
  ddi_firehose_print_log(str);

  snprintf(str, BUFFER_LENGTH, "$Rout(%4u, 0, %4u)\r\n", dq_rout, dq_rout);
  ddi_firehose_print_log(str);

  snprintf(str, BUFFER_LENGTH, "$Byte(%4u, 0, %4u)\r\n", whichByte, whichByte);
  ddi_firehose_print_log(str);

  //update rout
  if(FALSE == sbl1_ddr_debug_set_rout(0,SDRAM_INTERFACE_0,dq_rout, whichByte)) return FALSE;

  if(ddr_info.sdram0_cs0!=0){
    //read scan
    if(0 == tmpDQArgs->cur_rwtype || 1 == tmpDQArgs->cur_rwtype){// for all & read only
      rdcdc_scan = TRUE;
      write_cdc_delay = 0xffff;

      // init cdc
      for(dq = 0; dq < PHY_DQ_MAX_NUM; dq++){
        HAL_SDRAM_DDR_PHY_Update_WR_CDC_Config(0, SDRAM_INTERFACE_0, cdc_default_write[dq], dq);
        HAL_SDRAM_DDR_PHY_Update_RD_CDC_Config(0, SDRAM_INTERFACE_0, cdc_default_read[dq], dq);
      }

#ifdef DEBUG_PHY_V1
      for(sweep=0; sweep < 70; sweep++){
#else
      for(sweep = 0; sweep < max_sweep; sweep++){
#endif
        for(loop_count = 0; loop_count < 1; loop_count++){
          failures[0] = failures[1] = failures[2] = failures[3] = 0;
          error_statistics = 0;

#ifdef DEBUG_PHY_V1
          read_cdc_delay = CDC_STEP_DQ * sweep;
#else
          read_cdc_delay = cdc_lut[sweep];
#endif
          if(read_cdc_delay < VALID_CDC_START) {
            ddi_firehose_print_log("Skip read=%d", read_cdc_delay);
            continue;
          }

          ddr_function_defect_test((uint32)ddr_info.sdram0_cs0_addr, test_size);

          ddr_tuning_log_out(failures,tempresult,cdc_lut[0],cdc_lut[FAIL_HISTOGRAM_SIZE-1]);

#ifdef DEBUG_PHY_V1
          //to see if cdc really take effect
          switch(whichByte){
            case 0:
              curr_delay = BOOT_HWIO_INM(PHY_DQ_ADDR(RD_CDC_STATUS1),
                                         PHY_DQ0_OFFSET,
                                         HWIO_FMSK(PHY_DQ_ADDR(RD_CDC_STATUS1), CURR_DELAY_VALUE)
                                         ) / 0x10000;
              break;
            case 1:
              curr_delay = BOOT_HWIO_INM(PHY_DQ_ADDR(RD_CDC_STATUS1),
                                         PHY_DQ1_OFFSET,
                                         HWIO_FMSK(PHY_DQ_ADDR(RD_CDC_STATUS1), CURR_DELAY_VALUE)
                                         ) / 0x10000;
              break;
            case 2:
              curr_delay = BOOT_HWIO_INM(PHY_DQ_ADDR(RD_CDC_STATUS1),
                                         PHY_DQ2_OFFSET,
                                         HWIO_FMSK(PHY_DQ_ADDR(RD_CDC_STATUS1), CURR_DELAY_VALUE)
                                         ) / 0x10000;
              break;
            case 3:
              curr_delay = BOOT_HWIO_INM(PHY_DQ_ADDR(RD_CDC_STATUS1),
                                         PHY_DQ3_OFFSET,
                                         HWIO_FMSK(PHY_DQ_ADDR(RD_CDC_STATUS1), CURR_DELAY_VALUE)
                                         ) / 0x10000;
              break;
            default:
              curr_delay = 0xffff;
              break;
          }
          snprintf(str, BUFFER_LENGTH, "setting=%4u, curr_delay(read)=%4u\r\n", read_cdc_delay, curr_delay);
          ddi_firehose_print_log(str);
#endif
        }
      }
    }

    //write scan
    if(0 == tmpDQArgs->cur_rwtype || 2 == tmpDQArgs->cur_rwtype){// for all & write only
      rdcdc_scan = FALSE;
      read_cdc_delay = 0xffff;

      // init cdc
      for(dq = 0; dq < PHY_DQ_MAX_NUM; dq++){
        HAL_SDRAM_DDR_PHY_Update_WR_CDC_Config(0, SDRAM_INTERFACE_0, cdc_default_write[dq], dq);
        HAL_SDRAM_DDR_PHY_Update_RD_CDC_Config(0, SDRAM_INTERFACE_0, cdc_default_read[dq], dq);
      }

#ifdef DEBUG_PHY_V1
      for(sweep=0; sweep < 70; sweep++){
#else
      for(sweep = 0; sweep < max_sweep; sweep++){
#endif
        for(loop_count = 0; loop_count < 1; loop_count++){
          failures[0] = failures[1] = failures[2] = failures[3] = 0;
          error_statistics = 0;

#ifdef DEBUG_PHY_V1
          write_cdc_delay = CDC_STEP_DQ * sweep;
#else
          write_cdc_delay = cdc_lut[sweep];
#endif
          if(write_cdc_delay < VALID_CDC_START) {
            ddi_firehose_print_log("Skip write=%d", write_cdc_delay);
            continue;
          }

          ddr_function_defect_test((uint32)ddr_info.sdram0_cs0_addr, test_size);

          ddr_tuning_log_out(failures,tempresult,cdc_lut[0],cdc_lut[FAIL_HISTOGRAM_SIZE-1]);

#ifdef DEBUG_PHY_V1
          //to see if cdc really take effect
          switch(whichByte){
            case 0:
              curr_delay = BOOT_HWIO_INM(PHY_DQ_ADDR(WR_CDC_STATUS1),
                                         PHY_DQ0_OFFSET,
                                         HWIO_FMSK(PHY_DQ_ADDR(WR_CDC_STATUS1), CURR_DELAY_VALUE)
                                         ) / 0x10000;
              break;
            case 1:
              curr_delay = BOOT_HWIO_INM(PHY_DQ_ADDR(WR_CDC_STATUS1),
                                         PHY_DQ1_OFFSET,
                                         HWIO_FMSK(PHY_DQ_ADDR(WR_CDC_STATUS1), CURR_DELAY_VALUE)
                                         ) / 0x10000;
              break;
            case 2:
              curr_delay = BOOT_HWIO_INM(PHY_DQ_ADDR(WR_CDC_STATUS1),
                                         PHY_DQ2_OFFSET,
                                         HWIO_FMSK(PHY_DQ_ADDR(WR_CDC_STATUS1), CURR_DELAY_VALUE)
                                         ) / 0x10000;
              break;
            case 3:
              curr_delay = BOOT_HWIO_INM(PHY_DQ_ADDR(WR_CDC_STATUS1),
                                         PHY_DQ3_OFFSET,
                                         HWIO_FMSK(PHY_DQ_ADDR(WR_CDC_STATUS1), CURR_DELAY_VALUE)
                                        ) / 0x10000;
              break;
            default:
              curr_delay = 0xffff;
              break;
          }
          snprintf(str, BUFFER_LENGTH, "setting=%4u, curr_delay(write)=%4u\r\n", write_cdc_delay, curr_delay);
          ddi_firehose_print_log(str);
#endif
        }
      }
    }
  }

  return TRUE;
}

extern boolean ddr_do_phy_training(void);
extern uint8 fail_count_histogram_perbyte[PHY_DQ_MAX_NUM][FAIL_HISTOGRAM_SIZE];

boolean ddr_debug_ca_cdc_sweep(void* inArgs)
{
  SDRAM_CHIPSELECT chip_select;
  struct ddr_device_params_common *ddr_params_ch0;
  arg_cashmoo_p tmpCArgs;
  uint32 coarse_sweep = 0;
  uint32 max_coarse = 0;
  uint32 ddi_ddr_freq;
  uint32 coarse_init_val = 10;
  uint32 training_period_1ps=0;
  char logStr[BUFFER_LENGTH];
  union ddr_trained_params *ddr_params;
#ifdef DEBUG_PHY_V1
  uint32 curr_delay;
#endif

  struct ddr_params_partition * ddr_training_partition;
  uint32 size = 0;
  uint32 ddr_training_partition_size;

  if(NULL == ddr_params_get_training_data(&ddr_training_partition_size)) {
    ddr_params_set_training_data((void *)SHARED_IMEM_DDR_BASE, ddr_training_partition_size);
  }

  ddr_do_phy_training();

  if(NULL == inArgs) return FALSE;
  tmpCArgs = (arg_cashmoo_p)inArgs;

  clock_index = tmpCArgs->cur_clocklevel;

  ddr_debug_tuning_init();

  ddi_ddr_freq = -1 == clock_index ? g_clkInfo.ddr_fmax : g_clkInfo.ClockFreqKHz[clock_index];

  snprintf(logStr, BUFFER_LENGTH, "$Clock(%4u, 0, %4u)\r\n", ddi_ddr_freq, ddi_ddr_freq);
  ddi_firehose_print_log(logStr);

  snprintf(logStr, BUFFER_LENGTH, "$Vref(%4u, 0, %4u)\r\n", tmpCArgs->cur_vref, tmpCArgs->cur_vref);
  ddi_firehose_print_log(logStr);

  ddr_params_ch0 = &(ddr_get_params(SDRAM_INTERFACE_0)->common);
  if(ddr_params_ch0 == NULL) return FALSE;

  chip_select = SDRAM_CS_NONE;
  if (ddr_params_ch0->num_banks_cs0 != 0) chip_select |= SDRAM_CS0;
  if (ddr_params_ch0->num_banks_cs1 != 0) chip_select |= SDRAM_CS1;

  ddr_training_partition = ddr_params_get_training_data(&size);
  memset(ddr_training_partition, 0, size);

  ddr_params = ddr_get_trained_params(SDRAM_INTERFACE_0);

  if(NULL == ddr_params)
  {
    /* training parameters not found. */
    return FALSE;
  }

  ddr_params->lpddr3_training.nlevels = DDR_FREQ_MAX_NUM_LEVELS;

  HAL_SDRAM_BIMC_Update_Clock_Period(0, ddi_ddr_freq, ddr_divide_func, ddr_type);
  HAL_SDRAM_DPE_Pre_Clock_Switch(0, SDRAM_INTERFACE_0, 0, ddi_ddr_freq, ddr_type);
  boot_Clock_SetBIMCSpeed(ddi_ddr_freq);
  HAL_SDRAM_DPE_Post_Clock_Switch(0, SDRAM_INTERFACE_0, 0, ddi_ddr_freq, ddr_type);
  BIMC_Disable_All_Periodic(0, SDRAM_INTERFACE_0, chip_select);

  HAL_SDRAM_ddr_phy_ca_training( 0,
			   SDRAM_INTERFACE_0,
			   chip_select,
			   8,
			   0xA,
			   &(ddr_params->lpddr3_training.phy_cdc_params.ca_training),
			   ddi_ddr_freq);

  BIMC_Enable_All_Periodic(0, SDRAM_INTERFACE_0, chip_select);

  training_period_1ps = 1000000000 / ddi_ddr_freq;
  max_coarse = training_period_1ps / 2;
  
  for(coarse_sweep = coarse_init_val; coarse_sweep < MAX_COARSE_STEP; coarse_sweep++)
  {
		if(cdc_lut[coarse_sweep]> max_coarse) break;
  }

  max_coarse = coarse_sweep ;
  
  if(max_coarse > MAX_COARSE_STEP)
  {
	  max_coarse = MAX_COARSE_STEP;
  }
  
  for(coarse_sweep = coarse_init_val; coarse_sweep < max_coarse; coarse_sweep++) {
  	 if(fail_count_histogram_perbyte[0][coarse_sweep] > 0)
	    ddi_firehose_print_log("%4u, %4u, %4u, %4u [log]FAIL \r\n", 0xffff, cdc_lut[coarse_sweep], 0xffff, fail_count_histogram_perbyte[0][coarse_sweep]);
	 else
	 	ddi_firehose_print_log("%4u, %4u, %4u, %4u [log]PASS \r\n", 0xffff, cdc_lut[coarse_sweep], 0xffff, fail_count_histogram_perbyte[0][coarse_sweep]);
  }

  return TRUE;
}

