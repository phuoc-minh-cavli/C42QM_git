/*=============================================================================
                        Products Memory Dump Header File 

GENERAL DESCRIPTION     
  This module defines structures for the memory dump driver.
  
	Copyright 2012, 2019 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: 

when       who          what, where, why
--------   ---          -------------------------------------------------- 
07/31/2016 martham      Updated Bear SDI debug magic
11/13/2013	rssridha	Dump memory allocation for 9x25 SDI and TZ

===========================================================================*/

#ifndef _PRODUCT_DUMP_H
#define _PRODUCT_DUMP_H

#include "comdef.h"
#include "err.h"

/*===========================================================================

                     INTERNAL FUNCTION PROTOTYPES

===========================================================================*/
// Common Macros
#define SYSDBG_ENUM_32BITS(x) x##_INVALID=0x7FFFFFFF

#define PRODUCT_DUMP_MAKE_VERSION(ma, mi)   ((ma << 20) | mi)
#define PRODUCT_DUMP_MAJOR(val)             (val >> 20)
#define PRODUCT_DUMP_MINOR(val)             (val & 0xFFFFF)

/*INFO::SYSDBG_MAGIC_NUMBER definition is moved to  target.builds file*/
/*===========================================================================

                     INTERNAL DATA DECLARATIONS

===========================================================================*/
typedef enum
{
	CPU_SEC_CTXT,
	CPU_NSEC_CTXT,
	CPU_CTXT_COUNT,
	SYSDBG_ENUM_32BITS(CPU_CTXT),
}cpu_ctxt_type;

typedef enum
{
	/* ETR - Embedded Trace Router */
	TMC_ETR_TYPE=0,
	/* Embedded Trace Buffer / Funnel */	
	TMC_ETB_TYPE,
	MAX_TMC_TYPES,
	SYSDBG_ENUM_32BITS(TMC_TYPES),
}tmc_type;

/* Dump data type */
typedef struct
{
    uint32 version;
    uint32 magic;
    char name[32];
    uint64 start_addr;
    uint64 len;
}dump_data_type;

/* External CPU Dump Structure - 64 bit EL */
typedef struct
{
	uint64 x0;
	uint64 x1;
	uint64 x2;
	uint64 x3;
	uint64 x4;
	uint64 x5;
	uint64 x6;
	uint64 x7;
	uint64 x8;
	uint64 x9;
	uint64 x10;
	uint64 x11;
	uint64 x12;
	uint64 x13;
	uint64 x14;
	uint64 x15;
	uint64 x16;
	uint64 x17;
	uint64 x18;
	uint64 x19;
	uint64 x20;
	uint64 x21;
	uint64 x22;
	uint64 x23;
	uint64 x24;
	uint64 x25;
	uint64 x26;
	uint64 x27;
	uint64 x28;
	uint64 x29;
	uint64 x30;
	uint64 pc;
	uint64 currentEL;
    uint64 sp_el3;
	uint64 elr_el3;
    uint64 spsr_el3;
	uint64 sp_el2;
    uint64 elr_el2;
    uint64 spsr_el2;
	uint64 sp_el1;
    uint64 elr_el1;
    uint64 spsr_el1;
    uint64 sp_el0;
    uint64 __reserved1;
    uint64 __reserved2;
    uint64 __reserved3;
    uint64 __reserved4;
}sysdbg_cpu64_ctxt_regs_type;

/* External CPU Dump format - 32 bit EL */
typedef struct
{
	uint64 r0;
	uint64 r1;
	uint64 r2;
	uint64 r3;
	uint64 r4;
	uint64 r5;
	uint64 r6;
	uint64 r7;
	uint64 r8;
	uint64 r9;
	uint64 r10;
	uint64 r11;
	uint64 r12;
	uint64 r13_usr;
	uint64 r14_usr;
	uint64 r13_hyp;
	uint64 r14_irq;
	uint64 r13_irq;
	uint64 r14_svc;
	uint64 r13_svc;
	uint64 r14_abt;
	uint64 r13_abt;
	uint64 r14_und;
	uint64 r13_und;
	uint64 r8_fiq;
	uint64 r9_fiq;
	uint64 r10_fiq;
	uint64 r11_fiq;
	uint64 r12_fiq;
	uint64 r13_fiq;
	uint64 r14_fiq;
	uint64 pc;
	uint64 r13_mon;
	uint64 r14_mon;
	uint64 r14_hyp;
	uint64 _reserved;
    uint64 __reserved1;
    uint64 __reserved2;
    uint64 __reserved3;
    uint64 __reserved4;
}sysdbg_cpu32_ctxt_regs_type;

typedef union
{
	sysdbg_cpu32_ctxt_regs_type cpu32_ctxt;
	sysdbg_cpu64_ctxt_regs_type cpu64_ctxt;
}sysdbg_cpu_ctxt_regs_type;

// External format for CPU context
typedef struct
{
    /* Status fields */
	uint32 status[4];
	/* Context for all CPUs */
	sysdbg_cpu_ctxt_regs_type cpu_regs;
	/* Secure Context - Not used  */
	sysdbg_cpu_ctxt_regs_type __reserved3;
}sysdbgCPUCtxtType;

typedef struct
{
    dump_data_type *CPUDump;
}sysdbgCPUDumpType;


/* ETB / ETF Dump Structure */
typedef struct
{
	uint32 entries[1024];
}sysdbgETBDumpType;

/* ETM Dump Structure per CPU */
typedef struct
{
	uint32 entries[1024];
}sysdbgETMCPUDumpType;

/* Global struct for ETM Regs */
typedef struct
{
	dump_data_type *ETMCPU;
}sysdbgETMDumpType;

/* TMC Dump Structure per Tracing Cell */
typedef struct
{
	uint32 entries[1024];
}sysdbgTMCPerType;

typedef struct
{
	dump_data_type *TMCs[MAX_TMC_TYPES];
//	sysdbgTMCPerType *TMCs[MAX_TMC_TYPES];
}sysdbgTMCDumpType;

/* HLOS Dump Structure */
/* Dump Type */
typedef enum
{
    MDM_CPU_REGS_DUMP = 0x00,
    CPU_L0_ICACHE_DUMP = 0x20,
    CPU_L0_DCACHE_DUMP = 0x40,
    CPU_L1_ICACHE_DUMP = 0x60,
    CPU_L1_DCACHE_DUMP = 0x80,
    CPU_L2_DCACHE_DUMP = 0xC0,
    CPU_L2_ICACHE_DUMP = CPU_L2_DCACHE_DUMP,
    CPU_L3_CACHE_DUMP = 0xD0,
    CPU_L3_ICACHE_DUMP = CPU_L3_CACHE_DUMP,
    CPU_L3_DCACHE_DUMP = CPU_L3_ICACHE_DUMP,
    _RESERVED1 = 0xE0, // Reserved for OCMEM
    MSM_PMIC_DUMP = 0xE4,
    MDM_ETB_DUMP = 0xF0, 
    MDM_ETM_DUMP = 0xA0,
    MDM_TMC_DUMP = 0x100,
    MDM_TMC_2_DUMP = 0x101,
    MDM_MAX_DUMP = 0x200,
    MAX_DUMP_TYPES = 0x7FFFFFFF,
}dump_type;

/* Dump entry in the table */
typedef struct
{
    dump_type id;
    char name[32];
    uint32 type;
    uint64 start_addr;
}dump_entry_type;


/* Dump Table */
typedef struct 
{
	uint32 version;
	uint32 num_entries;
	dump_entry_type entries[MDM_MAX_DUMP>>4];
}dump_table_type;

/* Enums for the various clients in the dump table */
typedef enum
{
    MDM_DUMP_CLIENT_APPS = 0,
    MDM_DUMP_CLIENT_RPM  = 1,
    MDM_DUMP_CLIENT_MPSS = 2, 
    MDM_DUMP_CLIENT_MAX  = 3,
}mdm_dump_clients;

/* Enum for the type of dump entry provided by HLOS */
typedef enum
{
    MDM_DUMP_TYPE_DATA = 0x0,
    MDM_DUMP_TYPE_TABLE = 0x1,
}mdm_dump_entry_type;

/* =========================================================================
**  Function : product_buffer_init
** =========================================================================*/
/**
  
  RCinit wrapper function that calls into TZ and SDI mem alloc functions
*/

void product_buffer_init(void);


/* =========================================================================
**  Function : sdi_alloc
** =========================================================================*/
/**
  Initializes Dump_table data struct with memory region addresses to be passed into SDI

*/

boolean AppsDumpTable_alloc(void);

boolean apps_dump_data_register(dump_entry_type *entry);
/* ========================================================================
**  Function : sdi_write_ocimem_cookie
** =========================================================================*/
/**
 Writes cookie to IMEM location pointing to Dump Table structure

*/

void sdi_write_ocimem_cookie(void);

void sdi_clear_ocimem_cookie(void);

typedef struct sec_wdt_bite_buffer_s
 {
   uint32 dload_mode_req;
   uint32 spare;
 }sec_wdt_bite_buffer_t;


int wdog_bite_test(void);

boolean sdi_enabled(void);


/*===========================================================================
  FUNCTION  productlock_cs_init
===========================================================================*/
/**
  Creates the non-preemtable critical section.
 
  @return
  None

  @dependencies 
  None.

  @sideeffects
  None.
*/
/*==========================================================================*/
void productlock_cs_init( void );

/*===========================================================================
  FUNCTION  productlock_cs_lock
===========================================================================*/
/**
  Locks the critical section.
 
 
  @return
  None

  @dependencies 
  None.

  @sideeffects
  None.
*/
/*==========================================================================*/
void productlock_cs_lock( void );

/*===========================================================================
  FUNCTION  productlock_cs_unlock
===========================================================================*/
/**
  Releases the critical section.
 
  @return
  None

  @dependencies 
  None.

  @sideeffects
  None.
*/
/*==========================================================================*/
void productlock_cs_unlock( void );

/*===========================================================================
  FUNCTION  get_AppsDumpTable_addr
===========================================================================*/
/**
  For getting the address of the AppsDumpTable
  
   @param
   None
 
  @return
  The virtual address of AppsDumpTable

  @dependencies 
  None.

  @sideeffects
  None.
*/
/*==========================================================================*/
uint32 get_AppsDumpTable_addr( void );

#endif /* _PRODUCT_DUMP_H */

