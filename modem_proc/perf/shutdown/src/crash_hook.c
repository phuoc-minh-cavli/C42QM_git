/**
   @file crash_hook.c
   @author anandj, raholr
   @brief Crash handler for shutdown module in perf.mpss component 
    
   Copyright (c) 2016 by Qualcomm Technologies, Inc.  All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
#include "crash_hook.h"
#include "err.h"
#include "qurt.h"
#include "qurt_tlb.h"
#include "ULogFront.h"
#include "perf_err_cb.h"

#define SIZE_ALIGN 4096

/** @brief TLB entry type in fields and 64-bit value form
    @todo anandj This should ideally be exported by QuRT. Until that happens,
    here is it... */
typedef union 
{
    /** @brief Bit breakdown of TLB entry */
    struct
    {
        unsigned int    S: 1;
        unsigned int  PPN:23;
        unsigned int    C: 4;
        unsigned int    U: 1;
        unsigned int    R: 1;
        unsigned int    W: 1;
        unsigned int    X: 1;
        unsigned int  VPN:20;
        unsigned int ASID: 7;
        unsigned int   A0: 1;
        unsigned int   A1: 1;
        unsigned int   EP: 1;
        unsigned int    G: 1;
        unsigned int    V: 1;
    } fields;

    /** 64-bit value of TLB entry */
    unsigned long long int value;

} tlb_entry_t;

/** @brief reference to start_va_enable_fw_dump_overlay It is the starting point
           for firmware dump overlay */
unsigned int __attribute__((section (".data"))) start_va_enable_fw_dump_overlay = 0;

/** @brief Reference to symbol that marks the end of CLADE/Q6Zip's RW build-time
           compressed section */
unsigned int __attribute__((section (".data"))) end_va_enable_fw_dump_overlay = 0;

/** @brief Reference to symbols that marks the start and end of overlay_mem_dump build-time
           section */
unsigned int __attribute__ ((section(".data"))) start_va_map_fw_dump_overlay = 0;

unsigned int __attribute__ ((section(".data"))) end_va_map_fw_dump_overlay = 0;

qurt_paddr_t start_pa_enable_fw_dump_overlay;
unsigned int size_pa_enable_fw_dump_overlay = 0;

extern int perf_has_crashed;
/** @brief ULog handle for this module */
static ULogHandle ulogh;

static void perf_shutdown_crash_hook_enable_firmware_dump_overlay (void)
{
    int qurt_retval;
    tlb_entry_t tlb_entry;
    unsigned int tlb_idx, begin_tlb_idx, end_tlb_idx;
    
    perf_has_crashed = 1;
    /** @brief no VA overlay, need to create a seperate maping for fw dump overlay*/
    if (start_va_map_fw_dump_overlay != 0)
    {
      ULOG_RT_PRINTF_2( ulogh, "Creating PT Mapping for VA start: 0x%08X  VA end: 0x%08X",
                          start_va_map_fw_dump_overlay, end_va_map_fw_dump_overlay);
      //lookup physical addresses
      //returns 0 if there is no VA to PA mapping or if translation fails
      start_pa_enable_fw_dump_overlay = qurt_lookup_physaddr(start_va_enable_fw_dump_overlay);
      if (!start_pa_enable_fw_dump_overlay)
      {
        ULOG_RT_PRINTF_0( ulogh, "Error: Could not get the start PA for the fw dump overlay section" );
        return;
      }
      
      ULOG_RT_PRINTF_1( ulogh, "Creating PT Mapping starting at PA 0x%08X",
                          start_pa_enable_fw_dump_overlay);
      //calculate the size over which we are add and remove mapping (Also ALIGN by 4K)
      size_pa_enable_fw_dump_overlay = (end_va_map_fw_dump_overlay - start_va_map_fw_dump_overlay + (SIZE_ALIGN - 1)) & (~(SIZE_ALIGN-1));
      qurt_retval = qurt_mapping_create(start_va_map_fw_dump_overlay, start_pa_enable_fw_dump_overlay, size_pa_enable_fw_dump_overlay, QURT_MEM_CACHE_WRITEBACK, QURT_PERM_READ | QURT_PERM_WRITE);
      if(qurt_retval != QURT_EOK)
      {
        ULOG_RT_PRINTF_0( ulogh, "Error: Cannot not add back RW mapping on FW dump overlay region" );
      }
      return;
    }

    /* Firmware dump overlay starts from start_va_enable_fw_dump_overlay
       @warning Ensure that this matches with CLADE/Q6Zip RWs section in the linker script */
    qurt_retval = qurt_tlb_entry_query( &begin_tlb_idx, (qurt_addr_t)start_va_enable_fw_dump_overlay, 0 );
    if ( qurt_retval == QURT_EOK )
    {
        qurt_retval = qurt_tlb_entry_query( &end_tlb_idx, end_va_enable_fw_dump_overlay-1, 0 );
        if ( qurt_retval != QURT_EOK )
        {
            ULOG_RT_PRINTF_1( ulogh, "Failed to query entry for end_va_enable_fw_dump_overlay, VA=0x%08x", end_va_enable_fw_dump_overlay );
            end_tlb_idx = begin_tlb_idx;
        }

        ULOG_RT_PRINTF_2( ulogh, "TLB indexes [ 0x%03x ... 0x%03x ] being made writable",
                          begin_tlb_idx, end_tlb_idx );

        for ( tlb_idx = begin_tlb_idx; tlb_idx <= end_tlb_idx; tlb_idx++ )
        {
            qurt_retval = qurt_tlb_entry_get( tlb_idx, &tlb_entry.value );
            if ( qurt_retval == QURT_EOK )
            {
                /* Make this TLB entry writable */
                tlb_entry.fields.W = 1;

                qurt_retval = qurt_tlb_entry_set( tlb_idx, tlb_entry.value );
                if ( qurt_retval == QURT_EOK )
                {
                    /* Success! */
                }
                else
                {
                    ULOG_RT_PRINTF_1( ulogh, "Failed to set TLB entry, index=0x%03x", tlb_idx );
                }
            }
            else
            {
                ULOG_RT_PRINTF_1( ulogh, "Failed to get TLB entry, index=0x%03x", tlb_idx );
            }
        }
    }
    else
    {
        ULOG_RT_PRINTF_1( ulogh, "Failed to query entry for start_va_enable_fw_dump_overlay, VA=0x%08x", start_va_enable_fw_dump_overlay );
    }
}


void perf_shutdown_crash_hook_rcinit (void)
{
    ULogFront_RealTimeInit( &ulogh, "Perf Crash Hook", 128, ULOG_MEMORY_LOCAL, ULOG_LOCK_OS );

    if (!err_crash_cb_register( perf_shutdown_crash_hook_enable_firmware_dump_overlay ) )
    {
        ULOG_RT_PRINTF_0( ulogh, "Failed to register error callback" );
    }
}
