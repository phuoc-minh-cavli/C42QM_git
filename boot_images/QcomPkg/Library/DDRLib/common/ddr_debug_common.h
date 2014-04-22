#ifndef __DDR_DEBUG_COMMON_H__
#define __DDR_DEBUG_COMMON_H__


/*==============================================================================
                      Warning: This file is auto-generated
================================================================================
                   Copyright 2012-2013 Qualcomm Technologies Incorporated
                              All Rights Reserved
                     Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
                                  INCLUDES
==============================================================================*/
#include "ddr_common.h"
#include DDR_DEBUG_PHY_H
#include "ddr_test.h"
/* Length of transportation buffer in characters */
#define BUFFER_LENGTH   512

#define DEFAULT_SAMPLE_SIZE   (32)

#define MAX_STEP_POWER (16) 

/* Maximum length of command in characters */
#define CMD_MAX_LENGTH  64

/* Watchdog bark and bite time */
#define WATCHDOG_BARK_TIME_SCLK  0x7FFF
#define WATCHDOG_BITE_TIME_SCLK  0xFFFF

/* DDR tuning parameters */
#define DDR_TUNING_LOOP_COUNT  128
#define DQ_ROUT_MAX            0x7
#define DQ_PSLEW_MAX           0x3
#define DQ_NSLEW_MAX           0x3

#define RESET_DEBUG_SW_ENTRY_ENABLE  0x1

typedef enum{
	DEFAULT_PATTERN,
	BITFLIP_PATTERN,
	CHECK_BOARD_PATTERN,
	BITSPREAD_PATTERN,
	SOLID_BITS_PATTERN,
	WORKING_ONE_PATTERN,
	WORKING_ZERO_PATTERN,
	SEQ_INR_PARTTERN,
	CUSTOMER_PARTTERN,
	INVALID_PARTTERN
}DDR_TEST_PATTERN;
extern char str[BUFFER_LENGTH];
extern char str_error_log[BUFFER_LENGTH];
extern DDR_TEST_PATTERN PATTERN;
extern uint32 start_address;
extern uint32 test_size;
extern uint32 vref;
/*==============================================================================
                                  Function
==============================================================================*/


void ddr_debug_test(uint32 *ddr_base, uint32 failures[4]);
//boolean ddr_defect_interval_test_mem_map( uint32 base, uint32 limit);
boolean ddr_function_defect_test_bit_flip_tuning(const uint32 base, const uint32 size, boolean rdcdc_scan,uint8 coarse_sweep,uint8 fine_sweep, uint32 failures[4]);
#if 0
void ddr_debug_tune_cdc(boolean in_progress);
//void ddr_debug_tune_all(boolean in_progress);

#ifdef SEC_WDOG_ENABLE
void sysdbg_enable(void);
void ddr_debug_dog_init(boolean normal);
void ddr_debug_dog_enable(boolean enable);
void ddr_debug_dog_kick( void );
#endif

void ddr_debug_dq_training(boolean in_progress);
#endif
boolean ddr_function_defect_test_bit_flip(const uint32 base, const uint32 size);
boolean ddr_defect_interval_test(uint32 test_select,SDRAM_INTERFACE interface,SDRAM_CHIPSELECT  chip_select);
boolean ddr_function_defect_test(const uint32  ddr_base, const uint32 size);
boolean ddr_basically_test(ddr_test_suite test_suite, uint32 test_level, SDRAM_CHIPSELECT chip_select);
#endif

