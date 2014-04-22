/*===========================================================================

                    BOOT_DDI_TOOL

DESCRIPTION
  This file contains the code for Boot DDR Debug Image tool.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright 2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/25/19   ds      DDI framwork re-architecture for Twizy
04/09/17   zhz     Add DQ Vref CDC shmoo test
03/06/17   zhz     Read the correct CA Vref value from ca_vref.range_vref
01/09/17   zhz     move the target related function to ddi_target.c
09/09/16   jh       Ported various bugfixes from 8996
07/27/16   qbz     Cleaned files included
05/06/15   sng     Initial Release
=============================================================================*/
#include "ddi_target.h"
#include "ddi_tool.h"
#include "ddi_firehose.h"
#include "ddi_initialize.h"
#include "boot_visual_indication.h"
#include "boot_extern_ddr_interface.h"
#include "boot_cache_mmu.h"
#include "boothw_target.h"
#include "ClockBoot.h"
#include "pm_rgb.h"
#include "BIMC.h"
#include "boot_target.h"
#include "ddr_common_params.h"
#include "HALhwio.h"
#include "boot_error_if.h"
/*==============================================================================
                                  MACROS
==============================================================================*/

extern void logMessage(const char* format, ...);

/*==============================================================================
                                  DATA
==============================================================================*/

typedef struct  {
    char *name;
    boolean (*fp)(const uint32, const uint32);
}funcDefectTest_s;

char pattern_name[][20] =
{
    {"DEFAULT"},
    {"Bit Flip"},
    {"Checkerboard"},
    {"Bit Spread"},
    {"Solid bits"},
    {"Walking ones"},
    {"Walking zeros"},
    {"Seq incr"},
    {"Customer"},
};

uint32 DDR_TEST_SIZE = 1024;
static volatile uint32 min_base = 0;
/*==============================================================================
                                  FUNCTIONS
==============================================================================*/
void resetDDI()
{
    boot_hw_reset(BOOT_WARM_RESET_TYPE);
}

void InitDDITransport()
{
    deviceprogrammer_zi_buffers ();
    initFirehoseProtocol(); // initializes default values for fh structure
    ddi_init_hw();
  //  initMRStruct();
}

void deInitTransport()
{
    shutDownUSB();
}

/* =============================================================================
**  Function : ddr_function_defect_test_solid_bits
** =============================================================================
*/
/**
 * @brief DDR function defect test API
 *        Write the data 0x00000000 and 0x11111111 alternately to the tested
 *  memory area, and read back to check if the same value is fetched.
 *
 *
 * @param[in] base  test memory base start pointer
 * @param[in] size  the type of space used to be test
 *
 * @return TRUE test pass, FALSE test fail
 *
 *   @sideeffects
 *   Memory is corrupted after this function is called. Error is logged if any.
 */
boolean ddr_function_defect_test_solid_bits(uint32 base, uint32 size)
{
  const uint32 UINT32_ZERO = 0x0;
  const uint8 LOOP_CYCLES = 64;
  uint8  j = 0;
  PATTERN = SOLID_BITS_PATTERN;

  for (j = 0; j < LOOP_CYCLES; ++j)
  {
    uint32 val = (j % 2) == 0 ? (~UINT32_ZERO) : UINT32_ZERO;

	 ddr_write_pattern(base,size,&val);

    /* verify */
     if(ddr_read_pattern(base,size,&val)==FALSE)
        return FALSE;

  }
  return TRUE;
}

/* =============================================================================
**  Function : ddr_function_defect_test_check_board
** =============================================================================
*/
/**
 * @brief DDR function defect test API
 *        CheckBoard test on the memory test size given
 *        with partten (0x55555555, 0xAAAAAAAA)
 *
 * @param[in] base  test memory base start pointer
 * @param[in] size  the type of space used to be test
 *
 * @return TRUE test pass, FALSE test fail
 *
 *   @sideeffects
 *   Memory is corrupted after this function is called. Error is logged if any.
 */
boolean ddr_function_defect_test_check_board(uint32 base, uint32 size)
{
  const uint32 CHECK_BOARD = 0x55555555;
  const uint8 LOOP_CYCLES = 64;
  uint8  j = 0;
  PATTERN = CHECK_BOARD_PATTERN;

  for (j = 0; j < LOOP_CYCLES; ++j)
  {
    uint32 val = (j % 2) == 0 ? (~CHECK_BOARD) : CHECK_BOARD;
	ddr_write_pattern(base,size,&val);


      /* verify */
	if(ddr_read_pattern(base,size,&val)==FALSE)
       return FALSE;

  }

  return TRUE;
}

/* =============================================================================
**  Function : ddr_function_defect_test_bit_spread
** =============================================================================
*/
/**
 * @brief DDR function defect test API
 *        BitSpread test on the memory test size given
 *        with address:   00000101b -> 00001010b -> 00010100b  -> ...
 *             address++: 11111010b -> 11110101b -> 11101011b -> ...
 *             ...      :
 *        shmoo
 * @param[in] base  test memory base start pointer
 * @param[in] size  the type of space used to be test
 *
 * @return TRUE test pass, FALSE test fail
 *
 * @sideeffects
 * Memory is corrupted after this function is called. Error is logged if any.
 */
static boolean ddr_function_defect_test_bit_spread(const uint32 base, const uint32 size)
{
  const uint8 UINT32_LEN = 32;
  const uint32 ONE = 0x1;
  uint8  j = 0;
  PATTERN = BITSPREAD_PATTERN;

  for (j = 0; j < UINT32_LEN * 2; ++j)
  {
    uint32 val = 0;
    val = (ONE << j) | (ONE << (j + 2));

    ddr_write_pattern(base,size,&val);


    /* verify */
     if(ddr_read_pattern(base,size,&val)==FALSE)
        return FALSE;
  }

   return TRUE;
}

/* =============================================================================
**  Function : ddr_function_defect_test_bit_flip
** =============================================================================
*/
/**
 * @brief DDR function defect test API
 *        BitFlip test on the memory test size given
 *
 * @param[in] base  test memory base start pointer
 * @param[in] size  the type of space used to be test
 *
 * @return TRUE test pass, FALSE test fail
 *
 *   @sideeffects
 *   Memory is corrupted after this function is called. Error is logged if any.
 */
boolean ddr_function_defect_test_bit_flip(const uint32 base, const uint32 size)
{
  const uint8 UINT32_LEN = 32;
  const uint32 ONE = 0x1;
  uint8 j = 0;
  PATTERN = BITFLIP_PATTERN;

  for (j = 0; j < UINT32_LEN * 2; ++j)
  {
    uint32 val = ONE << j;

    ddr_write_pattern(base,size,&val);


      /* verify */
     if(ddr_read_pattern(base,size,&val)==FALSE)
          return FALSE;
        }


  return TRUE;
}


boolean ddr_function_defect_test(const uint32  ddr_base, const uint32 size)
{
  static funcDefectTest_s function_defect_test[] =
  {
    {"Solid bits", ddr_function_defect_test_solid_bits},
    {"Checkerboard", ddr_function_defect_test_check_board},
    {"Bit Spread", ddr_function_defect_test_bit_spread},
    {"Bit Flip", ddr_function_defect_test_bit_flip},
   //{"Walking ones", ddr_function_defect_test_walking_one},
  //  {"Walking zeros", ddr_function_defect_test_walking_zero},
   // {"Random value", ddr_function_defect_test_rand_val},
   // {"Seq incr", ddr_function_defect_test_seq_incr},
  };

  uint8 ddr_function_defect_test_num = sizeof(function_defect_test) / sizeof(*function_defect_test);
  uint8 i= 0;

  for(i=0; i < ddr_function_defect_test_num; i++)
  {
    if(function_defect_test[i].name && function_defect_test[i].fp)
    {
      ddr_debug_output(function_defect_test[i].name);
      ddr_debug_output(":\r\n");
      if(!(function_defect_test[i].fp(ddr_base, size >> 2)))
      {
      	return 	FALSE;
      }
      else
        ddr_debug_output("ok\r\n");
    }
   }

  return TRUE;
}

static boolean ddr_test_data_lines(uint32 base, uint32 limit)
{
  volatile uint32 *base_addr;
  uint32 pattern;
  uint32 data0, data1;
  uint8 result = TRUE;
    
  /* convert base address and limit for accessing memory by word */
  base_addr = (uint32 *)base;
  limit >>= 2;

  /* save the data before writing to it for ram dump case */
  data0 = base_addr[0];
  data1 = base_addr[limit];

  /* test data lines by walking-ones */
  for (pattern = 0x1; pattern != 0x0; pattern <<= 1)
  {
    /* write (0x1 << n) to first word */
    base_addr[0] = pattern;

    /* write ~(0x1 << n) to last word to clear data lines */
    base_addr[limit] = ~pattern;

    /* check if same value is read back */
    if (base_addr[0] != pattern)
    {
      logMessage("Failure happened on data line:%d",pattern);
      result = FALSE;
    }
  }

	
	/* restore the data for ram dump*/
  base_addr[0] = data0;
  base_addr[limit]= data1;

  return result;
}

static boolean ddr_test_addr_lines(uint32 base, uint32 limit)
{
  volatile uint32 *base_addr;
  uint32 addr_line;
  uint32 data[32] = {0};
  uint32 i;
  uint8 result = TRUE;


  /* convert base address and limit for accessing memory by word */
  base_addr = (uint32 *)base;
  limit >>= 2;

  /* write negation of address to each word at (0x1 << n) */
  for (addr_line = 0x1, i = 1; addr_line <= limit; addr_line <<= 1, i++)
  {
    /* save the address prior to overwriting it */
    data[i] = base_addr[addr_line];
    base_addr[addr_line] = ~addr_line;
  }

  /*
  save content at base */
  data[0] = base_addr[0x0];

  /* write negation of address to word at 0x0 */
  base_addr[0x0] = ~0x0;

  /* check if same value is read back from each word */
  for (addr_line = 0x1, i = 1; addr_line <= limit; addr_line <<= 1, i++)
  {
    if (base_addr[addr_line] != ~addr_line)
    {
      logMessage("Failure happened on address line:%d",addr_line+1);
      result = FALSE;
    }
    else
    {
      /* restore the address line's original content */
      base_addr[addr_line] = data[i];
    }
  }

  /* restore base content */
  base_addr[0x0] = data[0];
  return result;
}

static boolean ddr_test_own_addr
(
  uint32 base,
  uint32 limit
)
{
  volatile uint32 *base_addr;
  uint32 offset;

  
  /* convert base address and limit for accessing memory by word */
  base_addr = (uint32 *)base;
  limit >>= 2;

  /* write each word with its own address */
  for (offset = 0; offset <= limit; offset++)
  {
    base_addr[offset] = offset;
  }
  /* check if same value is read back from each word */
  for (offset = 0; offset <= limit; offset++)
  {
    if (base_addr[offset] != offset)
    {
      logMessage("Failed to Write address 0x%0x, date 0x%0x\n",&base_addr[offset], offset);
      return FALSE;
    }
  }

  /* write each word with negation of address */
  for (offset = 0; offset <= limit; offset++)
  {
    base_addr[offset] = ~offset;
  }
  /* check if same value is read back from each word */
  for (offset = 0; offset <= limit; offset++)
  {
    if (base_addr[offset] != ~offset)
    {
      logMessage("Failed to Write address 0x%0x, date 0x%0x\n",&base_addr[offset], ~offset);
      return FALSE;
    }
  }

  return TRUE;
}


boolean ddr_basically_test(ddr_test_suite	 test_suite,uint32			 test_level ,SDRAM_CHIPSELECT  chip_select)
{
 
 /* These variables are modified by Trace32 */
   static volatile boolean			 is_dcache_on = FALSE;
   //static volatile boolean			 is_test_all = TRUE;
   //static volatile SDRAM_INTERFACE	 interface = SDRAM_INTERFACE_0;
   uint32 size = 0;
   boolean result = TRUE;
   uint32 base = 0;
 	
	 if (!is_dcache_on)
	 {
	   struct mem_block sbl_ddr_mem_block =
	   {
		 0, 0, 0,
		 MMU_L1_SECTION_MAPPING, MMU_L1_PAGETABLE_MEM_READ_WRITE,
		 MMU_L1_PAGETABLE_MEM_NON_CACHEABLE, MMU_L1_PAGETABLE_NON_EXECUTABLE_REGION,
		 MMU_PAGETABLE_DEF_DOMAIN
	   };
  
	   ddr_size_info ddr_size = ddr_get_size();

	   
	   if(ddr_size.sdram0_cs1 != 0)
		 base = (ddr_size.sdram0_cs0_addr < ddr_size.sdram0_cs1_addr) ?
					 ddr_size.sdram0_cs0_addr : ddr_size.sdram0_cs1_addr;
	   else
		 base = ddr_size.sdram0_cs0_addr;
  
	   size = (ddr_size.sdram0_cs0 + ddr_size.sdram0_cs1 +
					  ddr_size.sdram1_cs0 + ddr_size.sdram1_cs1) << 20;
  
	   //size -= (SCL_SBL1_DDR_DATA_BASE + SCL_SBL1_DDR_DATA_SIZE) - base;
  
	   //base = (SCL_SBL1_DDR_DATA_BASE + SCL_SBL1_DDR_DATA_SIZE);
  
	   sbl_ddr_mem_block.p_base = sbl_ddr_mem_block.v_base = base;
	   sbl_ddr_mem_block.size_in_kbytes = size >> 10;
	   
	   BL_VERIFY(boot_mmu_page_table_map_single_mem_block(mmu_get_page_table_base(), &sbl_ddr_mem_block),
                BL_ERR_MMU_PGTBL_MAPPING_FAIL|BL_ERROR_GROUP_BOOT);
	 }

if(test_level == 0 )
{
		result = ddr_test_addr_lines(base,size-1);
		if(result){
			ddi_firehose_print_log("\nDDR_Addr_Lines_Test: PASS");
		}else{
			ddi_firehose_print_log("\nTDDR_Addr_Lines_Test: FAIL");
		}
		
		result = ddr_test_data_lines(base,size-1);
		
	    if(result){
			ddi_firehose_print_log("\nDDR_Data_Lines_Test: PASS");
		}else {
			ddi_firehose_print_log("\nDDR_Data_Lines_Test: FAIL");
		}
}
if(test_level == 1)
{
	result = ddr_test_own_addr(base,size-1);
	
	if(result){
		ddi_firehose_print_log("\nDDR_Own_Addr_Test: PASS");
	}else {
		ddi_firehose_print_log("\nDDR_Own_Addr_Test: FAIL");
	}
}		
	 return result;
}

/* =============================================================================
**  Function : ddr_defect_interval_test_mem_map
** =============================================================================
*/
/**
*   @brief
*   Test if memory size in words is power of 2, and base address is aligned with
*   size.
*
*   Memory size in words is power of 2 if and only if size in bytes is power of
*   2 (i.e., (size & (size-1)) == 0x0) and is no smaller than one word. Base
*   address is aligned with size if and only if (base & (size-1)) == 0x0.
*
*   Notice this function takes memory limit as input, where limit = (size-1).
*
*   @param[in]  base     Base address of memory
*   @param[in]  limit    Maximum offset (in bytes) of memory
*
*   @return TRUE test pass, FALSE test fail
*/

boolean ddr_defect_interval_test_mem_map
(
 uint32 base,
 uint32 limit
)
{
  return ( ((limit + 1) & limit) == 0x0 &&
           limit >= sizeof(uint32) - 1 &&
           (base & limit) == 0x0 );
}

/**
 * @brief DDR memory test by step
 *        test method:
 *        Step 0: write/read every double word until whole space looped
 *        Step 1: write/read every second double word until whole space looped
 *                totally 2 cycles.
 *        Step 2: write/read every four double word until whole space looped
 *                totally 4 cycles.
 *        ......
 *        Step n: write/read every 2^n double word until whole space looped
 *                totally 2^n cycles.
 *        NOTE: request at least 40MiB for test, and the memory space size
 *              should be n * 64KiByte
 *
 * @param base test memory space start pointer
 * @param length test memory space length in byte
 * @param stepPower the power of the step number
 * @param sampleSize the repeat times of this step test*
 * @return True test pass, FALSE test failure at read verification
 */
boolean ddr_interval_stress_test_per_step(uint32  const base, const uint32 length,
        const uint32 stepPower, const uint32 sampleSize)
{
  uint32 sampleCycle   = 0;
  uint32 stepStart     = 0;
  uint32 stepCycle     = 0;
  uint32 stepLength    = 0;
  uint32 dwordLength   = 0;
  volatile uint32 *base_addr;
  
  /* init function var*/
  stepLength = 1 << stepPower;
  dwordLength = length >> 2;
  base_addr = (uint32 *) base;
  	 
  /* write speed test by step defined*/
  for (sampleCycle = 0; sampleCycle != sampleSize; ++sampleCycle)
  {
    for (stepStart = 0; stepStart != stepLength; ++stepStart)
    {
      uint32 stepEnd = dwordLength - stepStart;
      for (stepCycle = 0; stepCycle < stepEnd;
          stepCycle += stepLength)
      {
        base_addr[stepStart + stepCycle] = stepStart + stepCycle;
      }
    }
  }	
  
  /* read verification for ram data retention */
  for (stepStart = 0; stepStart != stepLength; ++stepStart)
  {
    uint32 stepEnd = dwordLength - stepStart;
    for (stepCycle = 0; stepCycle < stepEnd;
        stepCycle += stepLength)
    {
      if (base_addr[stepStart + stepCycle] != stepStart + stepCycle)
      {
        ddi_firehose_print_log("address:0x%08x, error data:0x%08x, expect data:0x%08x\r\n", 
  	   	                        base_addr + stepStart + stepCycle, base_addr[stepStart + stepCycle],stepStart + stepCycle);
        return FALSE;
      }
    }
  }
  return TRUE;
}

boolean ddr_interval_stress_test(uint32 const base, const uint32 size)
{
  uint32 i = 0;
  uint32 stepShmooBeg = 0;
  uint32 stepShmooEnd = MAX_STEP_POWER;
  
  /* print out related test information */
  ddi_firehose_print_log("\r\nMemory Test Space: 0x%x++(0x%x) %%BYTE\r\n"
        "\rInterval step ran ge: 0x%x~0x%x \r\n", base, size, stepShmooBeg, stepShmooEnd);
		
  ddi_firehose_print_log("\rstep\t\tResult\r\n"); 
  
  for (i = stepShmooBeg; i != stepShmooEnd; i++)
  {
    if (!(ddr_interval_stress_test_per_step(base, size, i, DEFAULT_SAMPLE_SIZE)))
    {
      return FALSE;
    }
    else
    {
      snprintf(str,BUFFER_LENGTH, "\r\n%d\t\tok\r\n", 1 << i);
      ddi_firehose_print_log(str); 	   	
    }
  }		
  return TRUE;
}

boolean ddr_defect_interval_test
(  
  uint32            test_select,
  SDRAM_INTERFACE   interface,
  SDRAM_CHIPSELECT  chip_select
)
{   
  uint32 ddr_base = 0, ddr_size = 0;
  ddr_size_info ddr_info = ddr_get_size();
  
  /* get base address and size */
  if (interface == SDRAM_INTERFACE_0)
  {
    if (chip_select == SDRAM_CS0)
    {
      ddr_base = ddr_info.sdram0_cs0_addr;
      ddr_size = ddr_info.sdram0_cs0;
    }
    else if (chip_select == SDRAM_CS1)
    {
      ddr_base = ddr_info.sdram0_cs1_addr;
      ddr_size = ddr_info.sdram0_cs1;
    }
    else if (chip_select == SDRAM_BOTH)
    {
      ddr_base = (ddr_info.sdram0_cs0_addr < ddr_info.sdram0_cs1_addr) ?
                  ddr_info.sdram0_cs0_addr : ddr_info.sdram0_cs1_addr;
      ddr_size = ddr_info.sdram0_cs0 + ddr_info.sdram0_cs1;
    }
  }
  else if (interface == SDRAM_INTERFACE_1)
  {
    if (chip_select == SDRAM_CS0)
    {
      ddr_base = ddr_info.sdram1_cs0_addr;
      ddr_size = ddr_info.sdram1_cs0;
    }
    else if (chip_select == SDRAM_CS1)
    {
      ddr_base = ddr_info.sdram1_cs1_addr;
      ddr_size = ddr_info.sdram1_cs1;
    }
    else if (chip_select == SDRAM_BOTH)
    {
      ddr_base = (ddr_info.sdram1_cs0_addr < ddr_info.sdram1_cs1_addr) ?
                  ddr_info.sdram1_cs0_addr : ddr_info.sdram1_cs1_addr;
      ddr_size = ddr_info.sdram1_cs0 + ddr_info.sdram1_cs1;
    }
  }
  ddr_size = DDR_TEST_SIZE/1024;    //test 64M 
  /* convert size from megabyte to byte */
  ddr_size <<= 20;
  
  if (ddr_size == 0)
  {
    return FALSE;
  }
  else if (!ddr_defect_interval_test_mem_map(ddr_base, ddr_size-1))
  {
    ddr_debug_output("\r\nThe memory size in words is not power of 2\r\n");
    return FALSE;
  }
  else if (ddr_base + ddr_size > min_base)
  {
    /* adjust base address and size based on minimum base address for test */
    if (ddr_base < min_base)
    {
      ddr_size -= (min_base - ddr_base);
      ddr_base = min_base;
    }	 
  
    ddr_size >>= 2;
  		
  	if(test_select == 0)
  	{
  	  if(!(ddr_function_defect_test(ddr_base, ddr_size - 1)))
	  {
		ddi_firehose_print_log("DDR_FunctionDefect_Test: FAIL");
  	    return FALSE;
	  }else {
		  ddi_firehose_print_log("DDR_FunctionDefect_Test: PASS");
	  }
  	}
  	if (test_select == 1)
  	{
  	  if(!(ddr_interval_stress_test(ddr_base, ddr_size - 1)))
	  {
		ddi_firehose_print_log("DDR_IntervalStress_Test: FAIL");
  	    return FALSE;
	  }else {
		   ddi_firehose_print_log("DDR_IntervalStress_Test: PASS");
	  }
  	}		
  }
  
  return TRUE;
}

