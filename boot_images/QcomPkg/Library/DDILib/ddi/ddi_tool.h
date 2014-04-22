#ifndef _BOOT_DDI_TOOL_H
#define _BOOT_DDI_TOOL_H
/*==================================================================
 *
 * FILE:        ddi_tool.h
 *
 * DESCRIPTION:
 *
 *
 *        Copyright © 2016 Qualcomm Technologies Incorporated.
 *               All Rights Reserved.
 *               QUALCOMM Proprietary
 *==================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *
 * YYYY-MM-DD   who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 2019-02-25   ds     DDI framwork re-architecture for Twizy
 ===========================================================================*/
 
#include "ddr_common.h"
#include "ddr_test.h"
#include "ddi_target.h"
#include "ddr_common.h"
#include "boot_logger.h"
#include "boot_page_table_armv7.h"

/*DDR_DEBUG_PHY_H */
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
#if 0

#ifdef SEC_WDOG_ENABLE
void sysdbg_enable(void);
void ddr_debug_dog_init(boolean normal);
void ddr_debug_dog_enable(boolean enable);
void ddr_debug_dog_kick( void );
#endif
#endif

void resetDDI();
void InitDDITransport();
void deInitTransport();
boolean ddr_function_defect_test_bit_flip(const uint32 base, const uint32 size);
boolean ddr_defect_interval_test(uint32 test_select,SDRAM_INTERFACE interface,SDRAM_CHIPSELECT  chip_select);
boolean ddr_interval_stress_test(uint32 const base, const uint32 size);
boolean ddr_interval_stress_test_per_step(uint32  const base, const uint32 length,
        const uint32 stepPower, const uint32 sampleSize);
boolean ddr_function_defect_test(const uint32  ddr_base, const uint32 size);
boolean ddr_basically_test(ddr_test_suite test_suite, uint32 test_level, SDRAM_CHIPSELECT chip_select);
static boolean ddr_test_data_lines(uint32 base, uint32 limit);
static boolean ddr_test_addr_lines(uint32 base, uint32 limit);

#endif


