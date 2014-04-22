/*======================================================================
                        product_dump.c 

GENERAL DESCRIPTION
  Setup APPS memory dump table for SDI purpose.

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2016, 2019  by Qualcomm Technologies, Inc.  All Rights Reserved.
 ======================================================================*/
/*====================================================================
		Includes
======================================================================*/
#include "stdint.h"
#include "malloc.h"
#include "qurt_memory.h"
#include "product_dump.h"
#include "err.h"
#include "comdef.h"

/*==========================================================================
				Global Variables and Macro definitions
===========================================================================*/

#define PRODUCT_DUMP_TABLE_VERSION     PRODUCT_DUMP_MAKE_VERSION(2, 0)

/*Global dump table*/
static dump_table_type DumpTableEntry __attribute__((aligned(0x100)));

/*Apps dump table*/
static dump_table_type *AppsDumpTable;

/*APPs Core0 context will be dumped to this variable in SDI pass#1.*/
static sysdbgCPUCtxtType sysdbgCPUCtxt;

volatile uint32 crash_load_from_elf_start = XBL_CRASHDUMP_IMG_BASE;  
volatile uint32 crash_load_from_elf_size = XBL_CRASHDUMP_IMG_SIZE; 


/*===========================================================================

                              Function definitions

===========================================================================*/

/**@ sdi_write_imem_cookie
    Updates the global dump table address in the shared IMEM location.
	SDI uses this address and populates the dump table.

  @param[in] None.

  @returns
  TRUE -- if successfully updates the IMEM.

  @dependencies
  None.
*/
boolean sdi_write_imem_cookie(void)
{
   qurt_addr_t paddr = 0;
   
   uint32* base_addr = (uint32*)(SCL_SHARED_IMEM_BASE+0x10);
   
   paddr = qurt_lookup_physaddr((qurt_addr_t)&DumpTableEntry);
   if(paddr == 0)
   {
		ERR_FATAL("Products Dump:%d: qurt_lookup_physaddr failed",__LINE__, 0, 0);
		return FALSE;
   }
   
	*base_addr = paddr;
	return TRUE;
}

/**@ sdi_clear_imem_cookie
    Clears the SDI dump table shared IMEM location.

  @param[in] None.

  @returns
   None

  @dependencies
  None.
*/
void sdi_clear_imem_cookie(void)
{
   uint32 *sdi_addr = (uint32*)(SCL_SHARED_IMEM_BASE+0x10);

   /* Erase ocimem cookie*/
   *sdi_addr = 0;
}


boolean sdi_enabled(void)
{
     return TRUE;
}

/**@ apps_dump_data_register
    Registers a dump entry into the APPs dump table.
  
   @datatypes
   #dump_entry_type
   @param[in], dump entry to be registered.

   @returns
    None

   @dependencies, Call this only after APPs dump table set up is done.
   None.
*/

boolean apps_dump_data_register(dump_entry_type *entry) 
{
  uint32 idx;
  
  if((AppsDumpTable == NULL)||(entry == NULL))
  {
	return FALSE;
  }
  
  idx = AppsDumpTable->num_entries;	
  
  if(idx >= (MDM_MAX_DUMP>>4))
  {
	return FALSE;
  }
  
  AppsDumpTable->entries[idx].id 		 = entry->id;
  AppsDumpTable->entries[idx].type 		 = entry->type;
  AppsDumpTable->entries[idx].start_addr = entry->start_addr;
  AppsDumpTable->num_entries++;
  
  return TRUE;
}

/**@ register_apps_cpu_dump_entry
    Adds CPU_REGS_DUMP dump entry into APPs dump table.
  
   @param[in],None.

   @returns
    TRUE, If the CPU dump entry registered successfully.
	FALSE, If fail.

   @dependencies, Call this only after APPs dump table set up is done
   None.
*/

static boolean register_apps_cpu_dump_entry(void)
{
   dump_data_type* dump_data = NULL;
   qurt_addr_t paddr;
   dump_entry_type entry;
   
  /* populate the CPU dump table */
  dump_data = (dump_data_type*)malloc(sizeof(dump_data_type));
  if(dump_data == NULL)
  {
	ERR_FATAL("Products Dump:%d: malloc failed",__LINE__, 0, 0);
	return FALSE;
  }
  
  memset(dump_data, 0, sizeof(dump_data_type));
  
  /*Get physical address of CPU_REGS dump buffer.*/
  paddr = qurt_lookup_physaddr((qurt_addr_t)&sysdbgCPUCtxt);
  if(paddr == 0)
  {
	ERR_FATAL("Products Dump:%d: qurt_lookup_physaddr failed",__LINE__, 0, 0);
	return FALSE;
  }
  
  dump_data->start_addr = paddr;
  dump_data->len = sizeof(sysdbgCPUCtxtType);
  
  qurt_mem_cache_clean((qurt_addr_t)&dump_data, sizeof(dump_data_type), QURT_MEM_CACHE_FLUSH, QURT_MEM_DCACHE);
  
  paddr = qurt_lookup_physaddr((qurt_addr_t)dump_data);
  if(paddr == 0)
  {
	ERR_FATAL("Products Dump:%d: qurt_lookup_physaddr failed",__LINE__, 0, 0);
	return FALSE;
  }
 
  /*Register CPU_REGS dump entry*/ 
  entry.id   		= MDM_CPU_REGS_DUMP;
  entry.type 		= MDM_DUMP_TYPE_DATA;
  entry.start_addr  = (uint64)paddr;
 
  if (apps_dump_data_register(&entry) == FALSE)
  {
	ERR_FATAL("Products Dump:%d: apps_dump_data_register failed",__LINE__, 0, 0);
	return FALSE;
  }
  
  return TRUE;
}

/**@ AppsDumpTable_alloc
    Set up APPs memory dump table for SDI purpose..
  
   @param[in],None.

   @returns
    TRUE,If successfully set up the APPS memory dump table.
	FALSE, If fail.

   @dependencies
   None.
*/
boolean AppsDumpTable_alloc(void)
{
   uintptr_t paddr;
   
   dump_table_type ** DumpTable = (dump_table_type **)((uint32 *)(SCL_SHARED_IMEM_BASE+0x10));
   
  /* Dump table should be initialized to magic number by TZ if SDI is enabled */
  /* Check if SDI is enabled or not */
  if (*DumpTable != (dump_table_type *)SYSDBG_MAGIC_NUMBER)
  {
		/* SDI is not enabled . Return back */
		*DumpTable = (dump_table_type *)SYSDBG_MAGIC_NUMBER;
		return FALSE;	
  }  
 
  /* Initialize the top level APPS memory dump table and register in the Global dump table */
  AppsDumpTable = (dump_table_type *)malloc(sizeof(dump_table_type));
  if (AppsDumpTable == NULL)
  {
	ERR_FATAL("Products Dump:%d: malloc failed",__LINE__, 0, 0);
	return FALSE;
  }
  
  memset(AppsDumpTable, 0, sizeof(AppsDumpTable));
  AppsDumpTable->version = PRODUCT_DUMP_TABLE_VERSION;

 /*SDI dumps debug data to physical addresses since no MMU in SDI path, so get the physical address.*/  
  paddr = qurt_lookup_physaddr((qurt_addr_t)AppsDumpTable);
  if(paddr == 0)
   {
	ERR_FATAL("Products Dump:%d: qurt_lookup_physaddr failed",__LINE__, 0, 0);
	return FALSE;
   }
   
  DumpTableEntry.version = PRODUCT_DUMP_TABLE_VERSION;
  DumpTableEntry.num_entries 			= 0x1;
  DumpTableEntry.entries[0].id 			= (dump_type)MDM_DUMP_CLIENT_APPS;
  DumpTableEntry.entries[0].type 		= MDM_DUMP_TYPE_TABLE;
  DumpTableEntry.entries[0].start_addr	= paddr;
  
  /*Register apps cpu context dump entry in the apps dump table*/
  if ( register_apps_cpu_dump_entry() == NULL)
  {
	ERR_FATAL("Products Dump:%d: register_apps_cpu_dump_entry failed",__LINE__, 0, 0);
	return FALSE;
  }
  
  /*Flush the dump table data*/
  qurt_mem_cache_clean((qurt_addr_t)&DumpTableEntry, sizeof(dump_table_type), QURT_MEM_CACHE_FLUSH, QURT_MEM_DCACHE);
  qurt_mem_cache_clean((qurt_addr_t)AppsDumpTable, sizeof(dump_table_type), QURT_MEM_CACHE_FLUSH, QURT_MEM_DCACHE);
  
  return TRUE;
}

void sdi_alloc(void)
{
  /*Initialize product lock mutex variable*/
   productlock_cs_init();
   
   productlock_cs_lock();

   /*Set up APPS memory dump table*/
   AppsDumpTable_alloc();
   
   /*Write Memory dump table addess in SDI Imem location*/
   sdi_write_imem_cookie();

   productlock_cs_unlock();      
}

/*Main APPs memory dump driver init function. 
It will be called from the rcinit framework during the bootup.*/ 

void product_buffer_init(void)
{
   sdi_alloc();
}

/**@ get_AppsDumpTable_addr
    For getting the address of the AppsDumpTable
  
   @param
   None

   @returns
   The address of AppsDumpTable

   @dependencies
   None.
*/

uint32 get_AppsDumpTable_addr()
{
   /* This is to get the virtual address of ApssDumpTable */
   return (uint32)AppsDumpTable;
}	

