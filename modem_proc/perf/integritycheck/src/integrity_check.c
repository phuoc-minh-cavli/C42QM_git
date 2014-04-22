/*==========================================================================
 * FILE:         integrity_xor.c
 *
 * SERVICES:     INTEGRITY XOR
 *
 * DESCRIPTION:  This file provides the implementation of checking corruption in
 *               in the RO_compressed region using XOR Checksum
 *
 * Copyright (c) 2015 Qualcomm Technologies Incorporated.
 * All Rights Reserved. QUALCOMM Proprietary and Confidential.
=============================================================================*/


/*===========================================================================

            EDIT HISTORY FOR MODULE

$Header: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/16/16   rr      provide multi-region support for integrity check
01/20/16   rr      register integrity_fletcher with err_crash_register_cb
10/26/15   rr      register integrity_fletcher with perf error callback
06/30/15   rr      Created file
===========================================================================*/
#include <stdlib.h>
#include <qurt.h>
#include <hexagon_protos.h>
#include "ULogFront.h"
#include "err.h"
#include "perf_err_cb.h"
#include "perf_mem_services.h"

#define MAX_SECTIONS 7
#define SZ (14)
#define BLOCK_SIZE	 (1 << SZ)
#define CHECKSUM_SIZE    (4)
#define MAX_CHECKSUM_BLOCKS (1 << (SZ - 2))
#define MAX_SIZE (MAX_CHECKSUM_BLOCKS << (SZ))
#define CACHE_LINE_SHIFT (5)
#define CACHE_LINE_SZ  (1 << CACHE_LINE_SHIFT)
#define PREFETCH_DIST ((BLOCK_SIZE + CACHE_LINE_SZ - 1)/CACHE_LINE_SZ)


static ULogHandle ulog_handle;

unsigned int __attribute__((section (".data"))) perf_integritycheck_compress_section_begin[MAX_SECTIONS];
unsigned int __attribute__((section (".data"))) perf_integritycheck_compress_section_size[MAX_SECTIONS];
unsigned int __attribute__((section (".data"))) perf_integritycheck_checksum_buf[MAX_CHECKSUM_BLOCKS];
unsigned int __attribute__((section (".data"))) perf_integritycheck_num_sections = 0;
unsigned int perf_integritycheck_first_corrupt_page_addr = 0;
unsigned int perf_integritycheck_num_corrupted = 0;
unsigned int perf_integritycheck_copy_size = 0;
/************************************************************************************//**
*\brief checks for corruption in the compressed region when there is a crash using Fletcher's checksum		
*\param input_arg: start address of region, end address of region, count variable which points to next 16 KB
*\ checksum entry in the perf_integritycheck_checksum_buf
*\return none
****************************************************************************************/

static inline void integrity_fletcher(unsigned int address_start, unsigned int address_end, unsigned int* count)
{
	
	unsigned int sa, sb,  step_end, start , ii, result, end;
	end = address_end;
  
	for(start = address_start; start < end; start = start + BLOCK_SIZE)
	{
    /*check if there is an entire 16 KB block before prefetching*/
    if (end - start >= (BLOCK_SIZE << 1))
    {
      asm volatile ("l2fetch(%[addr],%[dim])" : : 
        [addr] "r" (start + BLOCK_SIZE), 
        [dim] "r" ( Q6_P_combine_IR(CACHE_LINE_SZ, Q6_R_combine_RlRl(CACHE_LINE_SZ, PREFETCH_DIST)))
        : "memory");
    }
		 sa = 0;
		 sb = 0;
		 step_end = start + BLOCK_SIZE;
		 if(step_end > end)
			 step_end = end;
		 for( ii = start;ii < step_end; ii = ii + CHECKSUM_SIZE)
		 {	
			sa = sa + (*((uint32_t*)ii));
			sb = sb + sa;
		 }
		 result = sb ^ sa;
		 if(perf_integritycheck_checksum_buf[(*count)] != result)
		{
			if(perf_integritycheck_num_corrupted == 0)
      {
			  perf_integritycheck_first_corrupt_page_addr  = start; /*get the address of the first corrupted page*/
        perf_integritycheck_copy_size = step_end - start;
			}
      perf_integritycheck_num_corrupted++;
			
		}
		(*count)++;
	}
	
}

/************************************************************************************//**
*\brief Checks for corruption in each region	
*\param input_arg: none
*\return none
****************************************************************************************/
void perf_run_integrity_check(void)
{
  unsigned int cycles1, cycles2,count,ii;

  ULOG_RT_PRINTF_0(ulog_handle, "Running Integrity Check on compressed sections");
  cycles1 = qurt_sysclock_get_hw_ticks();
  count = 0;
  for (ii = 0; ii < perf_integritycheck_num_sections; ii++)
  {
    integrity_fletcher(perf_integritycheck_compress_section_begin[ii], 
      perf_integritycheck_compress_section_begin[ii] + perf_integritycheck_compress_section_size[ii], &count);
  }
  
  cycles2 = qurt_sysclock_get_hw_ticks();
	ULOG_RT_PRINTF_3(ulog_handle, "Duration(ticks): %u, Duration (us): %u, blocks:%u", cycles2 - cycles1, ((cycles2 - cycles1)*13) >> 8,count);
	if(perf_integritycheck_num_corrupted != 0){
		/*save a copy of the first corrupted page before FW overlay*/
		ULOG_RT_PRINTF_0(ulog_handle, "Integrity Check Failed");
		memcpy((void*)perf_integritycheck_checksum_buf,(void*)perf_integritycheck_first_corrupt_page_addr , perf_integritycheck_copy_size); 
		ULOG_RT_PRINTF_2(ulog_handle, "Address of first corrupt block: 0x%08x, number of corrupted blocks: %u", perf_integritycheck_first_corrupt_page_addr, perf_integritycheck_num_corrupted);
	}
	else
		ULOG_RT_PRINTF_0(ulog_handle, "Integrity Check Pass");
}

/************************************************************************************//**
*\brief registers integrity_xor with the crash handler
*\param input_arg: none
*\return void
****************************************************************************************/


void init_integrity(void)
{

	if( perf_integritycheck_num_sections != 0) /*check if integrity_check.py was executed*/
	{
		ULogFront_RealTimeInit(&ulog_handle, "Integrity Check", 128, ULOG_MEMORY_LOCAL, ULOG_LOCK_OS);
		if(!(err_crash_cb_register(perf_run_integrity_check)))
			ULOG_RT_PRINTF_0(ulog_handle, "unable to register with crash handler");
	}

}
