/*=============================================================================

FILE:         qdss_dump.h

DESCRIPTION:  Defines and functions used by various Debug drivers to handle
              saving of registers & associated memory to DDR for crash analysis

================================================================================
            Copyright (c) 2015, 2019 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/


#include "qdss_dump.h"
#include "qdss_osal.h"
#include "qdss_throw.h"
#include "qdss_utils.h"
#include "qdss_imem.h"
#include "DALStdDef.h"
#include "DALStdErr.h"
#include "err.h"

#include "qurt_memory.h"
#include "qurt_error.h"

typedef struct
{
  boolean initialized;
  uint32 buffer_phys_base;
  uint32 buffer_size;

  DALSYSMemHandle mem_handle;
  QDSSMemInfo mem_info;

  uint32 global_dump_tbl_addr;
  dump_table_type *global_dump_tbl;
  DALSYSMemHandle global_dump_tbl_mem_handle;
  QDSSMemInfo global_dump_tbl_mem_info;

  uint32 apps_dump_tbl_addr;
  dump_table_type *apps_dump_tbl;
  DALSYSMemHandle apps_dump_tbl_mem_handle;
  QDSSMemInfo apps_dump_tbl_mem_info;

  qdss_buf_hdr *hdr;
}  debug_dump_info_type;
static debug_dump_info_type debug_dump_info;

#define QDSS_BUFFER_ALIGNMENT  0x1000
#define QDSS_BUFFER_ALIG_MASK  0xFFF

uint32 qdss_ddr_curr_buffer_size; 

// trace sink size to request for ETR 
uint32 qdss_buffer_sizes[4]={QDSS_DDR_BUFFER_SIZE_512KB, QDSS_DDR_BUFFER_SIZE_256KB, QDSS_DDR_BUFFER_SIZE_128KB, QDSS_DDR_BUFFER_SIZE_64KB};

/* Memory map the are in the Diag buffer where Debug related data will also
   be dumped */
static DALResult
qdss_debug_dump_buffer_init(void)
{


  DALResult result;
  qurt_mem_region_t QdssdumpMem_region;
  qurt_mem_region_attr_t QdssdumpMem_region_attr;
  uint32 i =0;
  
  qurt_mem_region_attr_init(&QdssdumpMem_region_attr);
  
  QdssdumpMem_region_attr.mapping_type = QURT_MEM_MAPPING_VIRTUAL;
  QdssdumpMem_region_attr.region_type = QURT_MEM_REGION_DEVICE;
  QdssdumpMem_region_attr.cache_attribs = QURT_MEM_CACHE_DEVICE;
  QdssdumpMem_region_attr.permission = QURT_PERM_FULL;
  QdssdumpMem_region_attr.physaddr = (qurt_addr_t)-1;
  QdssdumpMem_region_attr.virtaddr = (qurt_addr_t)-1;
    
  do {
		QdssdumpMem_region_attr.size = qdss_buffer_sizes[i]; 
		result = qurt_mem_region_create(&QdssdumpMem_region, QdssdumpMem_region_attr.size, qurt_mem_default_pool, &QdssdumpMem_region_attr );
		if (result == QURT_EMEM)
		{
			i++; 		
		}
		else 
		{
			break;
		}
  } while (i < 4);
	
  if (result != QURT_EOK)
  {
    return DAL_ERROR;
  }
  
  result = qurt_mem_region_attr_get(QdssdumpMem_region, &QdssdumpMem_region_attr);
  if (result != QURT_EOK)
  {
    return DAL_ERROR;
  }
  debug_dump_info.mem_info.VirtualAddr = QdssdumpMem_region_attr.virtaddr;
  
  /* ETM_BUF_EXPLICIT feature is defined for supporting ETM trace collection in 64MB DDR flavor.
     The feature is defined at target level */
  #ifdef  ETM_BUF_EXPLICIT
    debug_dump_info.buffer_phys_base = ETM_BUF_EXPLICIT_BASE;
    debug_dump_info.buffer_size = ETM_BUF_EXPLICIT_SIZE;
  #else
    debug_dump_info.buffer_phys_base = (uint32)QdssdumpMem_region_attr.physaddr;
    debug_dump_info.buffer_size = QdssdumpMem_region_attr.size;
  #endif
  
  qdss_ddr_curr_buffer_size = debug_dump_info.buffer_size;
	   
  
  /* Create the Dump table header at the top of the dump to help the parser */
  debug_dump_info.hdr = (qdss_buf_hdr *) debug_dump_info.mem_info.VirtualAddr;
  debug_dump_info.hdr->magic = QDSS_DUMP_MAGIC_NUMBER;
  debug_dump_info.hdr->dump_table_offset = QDSS_BUF_DUMP_TABLE_OFFSET;
  debug_dump_info.hdr->etb_regs_offset = QDSS_BUF_ETB_REGS_OFFSET;
  debug_dump_info.hdr->etr_regs_offset = QDSS_BUF_ETR_REGS_OFFSET;
  debug_dump_info.hdr->etb_data_offset = QDSS_BUF_ETB_DATA_OFFSET;
  debug_dump_info.hdr->etr_data_offset = QDSS_BUF_ETR_DATA_OFFSET;
  debug_dump_info.hdr->dcc_regs_offset = QDSS_BUF_DCC_REGS_OFFSET;
  debug_dump_info.hdr->dcc_data_offset = QDSS_BUF_DCC_DATA_OFFSET;

  return DAL_SUCCESS;
}


/* Locate the Apps dump table */
static DALResult
qdss_debug_dump_locate_apps_dump_table (void)
{
  DALResult result;
  uint32 i;
  dump_entry_type *dump_entry = NULL;
  uint32 tmp, virtAddr;

  productlock_cs_lock();

  debug_dump_info.apps_dump_tbl = NULL;

  debug_dump_info.global_dump_tbl_addr = (*(uint32*)(qdss_shared_imem_base()+0x10));
 
   
  /* SDI is not enabled or something went wrong in allocating apps dump table,
     So bail. */
  if (debug_dump_info.global_dump_tbl_addr == SYSDBG_MAGIC_NUMBER)
  {
    result = DAL_ERROR_NOT_INITIALIZED;
    goto bail;
  }

  if (debug_dump_info.global_dump_tbl_addr == 0)
  {
    result = DAL_ERROR_NOT_INITIALIZED;
    goto bail;
  }

  tmp = debug_dump_info.global_dump_tbl_addr;
  debug_dump_info.global_dump_tbl = (dump_table_type *) tmp;

  /* No entries in main dump table. Bail */
  if (debug_dump_info.global_dump_tbl->num_entries == 0)
  {
    result = DAL_ERROR_NOT_INITIALIZED;
    goto bail;
  }

  /* Search for apps dump table entry */
  for (i = 0; i < debug_dump_info.global_dump_tbl->num_entries; i++)
  {
    dump_entry = &(debug_dump_info.global_dump_tbl->entries[i]);
    if (dump_entry->type == MDM_DUMP_TYPE_TABLE &&
        dump_entry->id == MDM_DUMP_CLIENT_APPS  &&
        dump_entry->start_addr != 0)
    {
      break;
    }
  }

  /* Could not find apps dump table, bail */
  if (i == debug_dump_info.global_dump_tbl->num_entries)
  {
    result = DAL_ERROR_NOT_INITIALIZED;
    goto bail;
  }
  
  /* virtAddr will be updated with the virtual address of the AppsDumpTable after calling into get_AppsDumpTable_addr*/
  
  virtAddr = get_AppsDumpTable_addr();
  
  debug_dump_info.apps_dump_tbl_addr = (uint32)virtAddr;

  /* Found apps table. Memory map it to add debugtrace entries */
  tmp = (uint32) virtAddr;
  debug_dump_info.apps_dump_tbl = (dump_table_type *) tmp;

  result = DAL_SUCCESS;

bail:
  productlock_cs_unlock();
  return result;
}

DALResult
qdss_debug_dump_get_dump_buffer_base (uint32 *debug_trace_buffer_phy,
                                      uint32 *debug_trace_buffer_virt,
                                      uint32 *debug_trace_buffer_size)
{
  if (debug_trace_buffer_phy == NULL ||
      debug_trace_buffer_virt == NULL ||
      debug_trace_buffer_size == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  if (debug_dump_info.initialized == FALSE)
  {
    return DAL_ERROR_NOT_INITIALIZED;
  }

  *debug_trace_buffer_phy = debug_dump_info.buffer_phys_base;
  *debug_trace_buffer_virt = debug_dump_info.mem_info.VirtualAddr;
  *debug_trace_buffer_size = debug_dump_info.buffer_size;

  return DAL_SUCCESS;
}

DALResult
qdss_debug_dump_add_dump_entry (const dump_entry_type *entry)
{
  uint32 max_entries;
  DALResult result;
  dump_entry_type *dump_entry;
  dump_table_type *apps_dump_tbl;

  if (entry == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  if (debug_dump_info.initialized == FALSE ||
      debug_dump_info.apps_dump_tbl == NULL)
  {
    return DAL_ERROR_NOT_INITIALIZED;
  }

  apps_dump_tbl = debug_dump_info.apps_dump_tbl;

  max_entries = sizeof (apps_dump_tbl->entries) /
                  sizeof (apps_dump_tbl->entries[0]);

  productlock_cs_lock();

  if (apps_dump_tbl->num_entries == max_entries)
  {
    result = DAL_ERROR_INSUFFICIENT_MEMORY;
    goto bail;
  }

  dump_entry = &(apps_dump_tbl->entries[apps_dump_tbl->num_entries]);
  dump_entry->id = entry->id;
  dump_entry->type = entry->type;
  dump_entry->start_addr = entry->start_addr;

  apps_dump_tbl->num_entries += 1;

  result = DAL_SUCCESS;

 bail:
  productlock_cs_unlock();
  return result;
}

/* Initialize the buffers to store all Debugtrace related data. Called from
   RCINIT */
void
qdss_debug_dump_init (void)
{
  DALResult result;

  if (debug_dump_info.initialized == TRUE)
  {
    return;
  }

  result = qdss_debug_dump_buffer_init ();
  if (result != DAL_SUCCESS)
  {
    return;
  }

  (void) qdss_debug_dump_locate_apps_dump_table ();

  debug_dump_info.initialized = TRUE;
}
