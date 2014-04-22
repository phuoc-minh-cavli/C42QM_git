/*======================================================================
                        cust_config.c 

GENERAL DESCRIPTION
  Memory pool definitions

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2017-2020, 2022  by Qualcomm Technologies, Inc.  All Rights Reserved.
 ======================================================================*/
/*======================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *  
 *
 *
 * when         who     what, where, why
 * ----------   ---     ------------------------------------------------
 * 07/02/2017   priygupt  Updated static_pool with macro values defined in target.builds
 * 20/01/2017   c_sbegum  Add OEM_POOL
 * 07/01/2019	aj	 	  IMEM size increased from 16KB to 164KB
 * 24/08/2020   nt        KW error fix
 ======================================================================*/
#include "mem_utils.h"

#define OEM_POOL_NAME    "OEM_POOL"
  /* Added entry 0x80000000, 0x01E22000 for modem image loading by PIL
     Added entry 0x08600000, 0x29000 for IMEM region */
#define MODEM_BASE_ADDR   0x80000000
#define MODEM_RANGE       0x01E22000
#define IMEM_BASE_ADDR    0x08600000
#define IMEM_RANGE        0x29000

#ifdef TX_ENABLE_AWARE_CUSTOMIZATION
#define MAX_POOLS_TOTAL     (MAX_POOLS + 1)
#else
#define MAX_POOLS_TOTAL     (MAX_POOLS)
#endif

struct phys_mem_pool_config pool_configs[MAX_POOLS_TOTAL] = {
    {"DEFAULT_PHYSPOOL", { {QURT_MEMPOOL_START_ADDRESS, QURT_MEMPOOL_SIZE}, {0/*AMSS Heap Base Address Updated in mem_init.c */, 0 /*AMSS Heap Size Updated in mem_init.c */}, {0, 0}, {0, 0}, {0, 0}, 
                           {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} }
    },
    {"HWIO_POOL", { {0x00000000, 0x100000}, {0x00200000, 0x100000}, {0x00400000, 0x200000}, {0x00700000, 0x100000}, 
                    {0x01000000, 0x400000}, {0x01800000, 0x400000}, {0x01e00000, 0x100000}, {0x02000000, 0x1C00000}, {0x04000000, 0x800000},
                    {0x06000000, 0x100000}, {0x07700000, 0x300000}, {0x07c00000, 0x100000}, {0x08000000, 0x100000}, 
                    {0x08800000, 0x100000}, {0x0b000000, 0x200000}, {0x8600000, 0x29000}, {0x09002000, 0x2000},{0x000C0000,0x14000},
                    {0, 0}, {0, 0} }
    },
    {"STATIC_POOL", { {MODEM_BASE_ADDR, MODEM_RANGE}, {IMEM_BASE_ADDR, IMEM_RANGE}, {SCL_SHARED_RAM_BASE, SCL_SHARED_RAM_SIZE}, {0x09280000, 0x1000}, {ACDB_RAM_BASE_PHYS, ACDB_RAM_SIZE}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} }
    },
    {OEM_POOL_NAME, { {OEM_POOL_START, OEM_POOL_SIZE}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, 
                   {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} }
    },
	#ifdef DEFINE_QSEECOM_POOL
    {"QSEECOM_POOL", { {QSEECOM_POOL_START, QSEECOM_POOL_SIZE}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
                       {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} }
    },
	#endif
#ifdef TX_ENABLE_AWARE_CUSTOMIZATION
    {"FOTA_POOL", { {FOTA_POOL_START, FOTA_POOL_SIZE}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
                    {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} },
    },
#endif
};

/*
vpn:        63:44
size:       43:30 // restricting the max size to 3M for now
cache_attr: 29:24
perm:       23:20
ppn:        19:0
*/
#define MEMORY_MAP(vpn, ppn, size, cache_attr, perm) \
    ( ( (unsigned long long)((vpn << 12) | ((size >> 2) & 0xFFF) ) << 32 ) \
    | ( ((size & 0x3) << 30) | (cache_attr << 24) | (perm << 20) | ppn ) )


unsigned long long QURTK_mmap_table[] = {
// static mappings picked up from xml
    MEMORY_MAP(0x00022UL, 0x00022UL, 0x1UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
    MEMORY_MAP(0x00023UL, 0x00023UL, 0x1UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
    MEMORY_MAP(0x00024UL, 0x00024UL, 0x1UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
    MEMORY_MAP(0x00025UL, 0x00025UL, 0x1UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
    MEMORY_MAP(0x00026UL, 0x00026UL, 0x1UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
    MEMORY_MAP(0x00028UL, 0x00028UL, 0x1UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
    MEMORY_MAP(0x00029UL, 0x00029UL, 0x1UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
    MEMORY_MAP(0x0002AUL, 0x0002AUL, 0x1UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
    MEMORY_MAP(0x0002EUL, 0x0002EUL, 0x1UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
    MEMORY_MAP(0x0002FUL, 0x0002FUL, 0x1UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
    MEMORY_MAP(0x00032UL, 0x00032UL, 0x1UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
    MEMORY_MAP(0x00033UL, 0x00033UL, 0x1UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
    MEMORY_MAP(0x00034UL, 0x00034UL, 0x6UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
    MEMORY_MAP(0x00040UL, 0x00040UL, 0x8UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
    MEMORY_MAP(0x00048UL, 0x00048UL, 0x1UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
    MEMORY_MAP(0x00050UL, 0x00050UL, 0x4UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
    MEMORY_MAP(0x00060UL, 0x00060UL, 0x8UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
    MEMORY_MAP(0x00068UL, 0x00068UL, 0x4UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
    MEMORY_MAP(0x0006CUL, 0x0006CUL, 0x1UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
    MEMORY_MAP(0x000A0UL, 0x000A0UL, 0x10UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
    MEMORY_MAP(0x00200UL, 0x00200UL, 0x100UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
    MEMORY_MAP(0x00400UL, 0x00400UL, 0x80UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
    MEMORY_MAP(0x004A0UL, 0x004A0UL, 0x10UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
    MEMORY_MAP(0x00500UL, 0x00500UL, 0x16UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
    MEMORY_MAP(0x00700UL, 0x00700UL, 0x40UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
    MEMORY_MAP(0x01000UL, 0x01000UL, 0x400UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
    MEMORY_MAP(0x01800UL, 0x01800UL, 0x100UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
    MEMORY_MAP(0x01900UL, 0x01900UL, 0x80UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
	MEMORY_MAP(0x01A00UL, 0x01A00UL, 0x200UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
    MEMORY_MAP(0x01E00UL, 0x01E00UL, 0x100UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
    MEMORY_MAP(0x02000UL, 0x02000UL, 0x1C00UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
    MEMORY_MAP(0x04000UL, 0x04000UL, 0x800UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
    MEMORY_MAP(0x06000UL, 0x06000UL, 0x30UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
    MEMORY_MAP(0x06041UL, 0x06041UL, 0x2UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
    MEMORY_MAP(0x06045UL, 0x06045UL, 0x4UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
    MEMORY_MAP(0x06070UL, 0x06070UL, 0x30UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
    MEMORY_MAP(0x07700UL, 0x07700UL, 0x20UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
    MEMORY_MAP(0x07800UL, 0x07800UL, 0x25UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
    MEMORY_MAP(0x07840UL, 0x07840UL, 0x25UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
    MEMORY_MAP(0x07880UL, 0x07880UL, 0x3BUL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
    MEMORY_MAP(0x078C0UL, 0x078C0UL, 0x1AUL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
    MEMORY_MAP(0x07980UL, 0x07980UL, 0x40UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
    MEMORY_MAP(0x07C00UL, 0x07C00UL, 0x4UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
    MEMORY_MAP(0x07C40UL, 0x07C40UL, 0x20UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
    MEMORY_MAP(0x08000UL, 0x08000UL, 0x40UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
    MEMORY_MAP(0x08801UL, 0x08801UL, 0x1UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
    MEMORY_MAP(0x0B000UL, 0x0B000UL, 0x200UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
    MEMORY_MAP(0x000C0UL, 0x000C0UL, 0x14UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
// static mappings added newly
    MEMORY_MAP(0x8600UL, 0x8600UL, 0x29UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
	/*STM*/
	MEMORY_MAP(0x09002UL, 0x09002UL, 0x2UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
	/*Tfunnel*/
	MEMORY_MAP(0x06068UL, 0x06068UL, 0x1UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
	/*Modem cpu 0 CTI*/
	MEMORY_MAP(0x06038UL, 0x06038UL, 0x1UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
	/*RPM cpu 0 CTI*/
	MEMORY_MAP(0x0603CUL, 0x0603CUL, 0x1UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
	/*cpu 0 CTI*/
	MEMORY_MAP(0x06043UL, 0x06043UL, 0x1UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
	/*HWEvent registers*/
	MEMORY_MAP(0x0606CUL, 0x0606CUL, 0x1UL, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE),
};

int QURTK_mmap_table_size = sizeof(QURTK_mmap_table)/sizeof(unsigned long long);

unsigned int getOemPoolSize(void)
{
	int i=0;
	while( (i < MAX_POOLS_TOTAL) && (pool_configs[i].name[0]!='\0') ){
		if(strcasecmp(OEM_POOL_NAME,pool_configs[i].name) == 0) 
			return pool_configs[i].ranges[0].size;
		i++;
	}
	return 0x0;	/* OEM_POOL not found */
}

/*** Changes related to configurable memory region query framework ***/

typedef struct memRegion_info {
    uint32 reg_id;
    qurt_addr_t reg_start; 
    qurt_size_t reg_size; 
} memRegion_info_t;

/* Static registry for configurable memory regions.
   Region IDs are defined in qurt_memory.h file
   Entries for Region base address and region size should match with what is captured in .builds or .h file.
   Any mismatch in entry (like for example, size is derived as A+B in .builds or .h 
   but entered as only A or B here) would result into wrong result from the query API.
*/
static memRegion_info_t gMemRegion_list[MEM_REGION_ID_MAX] = 
{
	// Region ID 0: Reserved
	{MEM_REGION_ID_RESERVED, NULL, 0},
	
	// Region ID 1: OEM_POOL
	{MEM_REGION_ID_OEM_POOL, OEM_POOL_START, OEM_POOL_SIZE},
	
	#ifdef DEFINE_TZ_APPS_REGION
	// Region ID 2: TZ_APPS
	{MEM_REGION_ID_TZ_APPS, TZ_APPS_START, (TZ_APPS_END - TZ_APPS_START + 1)},
	#endif
};


/***
   Function to query Memory region info based on client supplied region ID.
   If input region ID is valid and present in region list, it would return 
   region start address and region size to client.

   Parameters:
	[in] region_id     : ID of the Memory region for which info is requested.
	[out] pRegion_start : Client supplied pointer, to which, region start address would be updated.
	[out] pRegion_size  : Client supplied pointer, to which, region size would be updated.

  Return:
	QURT_EOK      -- Success. Supplied pointers are updated with respective info.
	QURT_EINVALID -- Invalid input param.
	QURT_EFAILED  -- Failure due to internal error.

  Dependencies
	None.
***/
int qurt_get_mem_region_info(uint32 region_id, qurt_addr_t *pRegion_start, qurt_size_t *pRegion_size) 
{ 
	int i = 0; 
	int ret = QURT_EFAILED;
	int num_elements;

	/* Validate the addresses passed */
	if((pRegion_start == NULL) || (pRegion_size == NULL))
		return QURT_EINVALID;
	
	// Derive num elements in the array and use it for boundary check for extra safeguard.
	num_elements = sizeof(gMemRegion_list)/sizeof(memRegion_info_t);
	
	// Validate input params
	if((region_id == MEM_REGION_ID_RESERVED) || (region_id >= MEM_REGION_ID_MAX) || (region_id >= num_elements))
	{
		*pRegion_start = NULL;
		*pRegion_size  = 0;

		return QURT_EINVALID;
	}

	for(i = 0; i < num_elements; i++)
	{
		if(gMemRegion_list[i].reg_id == region_id)
		{
			*pRegion_start = gMemRegion_list[i].reg_start;
			*pRegion_size  = gMemRegion_list[i].reg_size;
			ret = QURT_EOK;
			
			break;
		}
	}
	
	return ret;
}

/*** ***/
