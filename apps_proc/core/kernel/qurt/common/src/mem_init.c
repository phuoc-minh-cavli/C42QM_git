/*======================================================================
                        mem_init.c 

GENERAL DESCRIPTION
  Memory pool management definitions

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2017, 2021 - 2022  by Qualcomm Technologies, Inc.  All Rights Reserved.
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
 * 8/30/21      NT      moving the L2 PT to a global array
 * 1/18/19		AJ		l2 pgtable across multiple boundries
 ======================================================================*/
 #include "mem_utils.h"

#define FREE_LIST(pool, idx)    (&(pool)->free_list[idx])

#ifdef TX_ENABLE_AWARE_CUSTOMIZATION
#define MAX_POOLS_TOTAL     (MAX_POOLS + 1)
#else
#define MAX_POOLS_TOTAL     (MAX_POOLS)
#endif

extern mem_region_t *QURTK_region_allocate_list;
extern QURTK_obj_cache_t QURTK_region_cache;
extern QURTK_obj_cache_t QURTK_memory_cache;

extern unsigned int size_to_best_page(unsigned int size);
extern unsigned int QURTK_pgsize_encode_to_size[];
extern struct phys_mem_pool_config pool_configs[];
extern unsigned long long QURTK_mmap_table[];
extern int QURTK_mmap_table_size;
extern int QURTK_mmap(uint32 vaddr, uint32 paddr, uint32 size, uint32 cache_mode, uint32 perm, uint32 asid);
extern qurt_mutex_t mem_lock;
extern uint32 max_regions_alloced;
extern uint32 Image$$APPS_END$$ZI$$Limit;
extern uint32 app_heap_start;
extern uint32 app_heap_size;

phys_pool_t QURTK_phys_pools[MAX_POOLS_TOTAL];
int default_pool_index = 0;
struct mem_pool default_virt_pool;
struct mem_pool default_phys_pool;
uint32 apps_end_post_qsh __attribute__((used)) = QURT_MEMPOOL_START_ADDRESS;

QURTK_obj_cache_t QURTK_memory_cache, QURTK_pgtbl_cache, QURTK_region_cache, QURTK_vma_cache;

void * memsetK(void *b, int c, qurt_size_t n){
    char *t = b;
    int i;
    for( i = 0 ; i < n; i++ ){
        t[i] = c;
    }
    return b;
}

int memcmpK(void *dest, void *src, qurt_size_t n) {
    int i;
    char *d = dest;
    char *s = src;
    for (i=0;i<n;i++)
    {
        if (d[i] != s[i])
            return i+1;
    }
    return 0;
}

int memcpyK(void *dest, void *src, qurt_size_t n) {
    int i;
    char *d = dest;
    char *s = src;
    for( i = 0 ; i < n; i++ ){
        d[i] = s[i];
    }
    return n;
}

/*
 * Check whether a pool has any memory in it or not.  Returns a bool.
 */
int
mem_empty(struct mem_pool *pool)
{
    unsigned i;

    for (i = 0; i <= MEM_MAX_IDX; i++) {
        if (!TAILQ_EMPTY(FREE_LIST(pool, i))) {
            return 0;
        }
    }
    return 1;
}


extern qurt_mem_pool_t qurt_mem_default_pool;

void phys_pool_init(void) {
    int i = 0, j = 0;
    char default_pool_name[32]="DEFAULT_PHYSPOOL";

     //Set up physical pools
    memsetK(QURTK_phys_pools, 0, sizeof(phys_pool_t)*MAX_POOLS_TOTAL);
    
    // Set the default phys pool
    memcpyK(QURTK_phys_pools[0].name, default_pool_name, 32);
    qurt_mem_default_pool = (qurt_mem_pool_t)&(QURTK_phys_pools[0].pool);
    
	// Case for dynamic Heap size allocation when ((AMSS_HEAP_SIZE == 0 )||(AMSS_HEAP_SIZE > app_heap_size))
	//Search for first entry with size = 0 will be updated here
	while( 0 != pool_configs[0].ranges[i].size ) { 
		i++;
		if (i == MAX_PHYPOOL_RANGES ){ 
			QURTK_fatal("DEFAULT_PHYSPOOL ranges exhausted! AMSS Heap entry could not be added dynamically");
		}
	}
	if (0 == app_heap_start ){
		QURTK_fatal("app_heap_start not yet updated!");
	}
	pool_configs[0].ranges[i].start = app_heap_start;
	pool_configs[0].ranges[i].size = app_heap_size;
	i = 0;
	
    while(pool_configs[i].name[0]!='\0' && i < MAX_POOLS_TOTAL ) {
        kprintf("%d name %s pool %p\n", i, pool_configs[i].name, &QURTK_phys_pools[i].pool);
        //copy 32 bytes
        memcpyK(QURTK_phys_pools[i].name, pool_configs[i].name, 32);
        j = 0;
        while(pool_configs[i].ranges[j].size != 0) {
            kprintf("\t%d start %x size %x\n", j, pool_configs[i].ranges[j].start, pool_configs[i].ranges[j].size);
            mem_add(&(QURTK_phys_pools[i].pool), pool_configs[i].ranges[j].start, pool_configs[i].ranges[j].size );  
            j++;
        }
        i++;
    }
}


void static_mapping_init() {
    int mmap_table_idx, map_err=~(QURT_EOK);
    uint32 vaddr, paddr, size, cache_mode, perm;
    struct memory *mem = NULL;
    mem_region_t * region = NULL;
    mem_pool_t * hwio_pool = &(QURTK_phys_pools[1].pool);
    
    mmap_table_idx = QURTK_mmap_table_size - 1;
	qurt_mutex_lock( &mem_lock );
    while (mmap_table_idx >= 0) {

        vaddr = QURTK_get_vpn(QURTK_mmap_table[mmap_table_idx]) << 12;
        paddr = QURTK_get_ppn(QURTK_mmap_table[mmap_table_idx]) << 12;
        size = QURTK_get_pgsize(QURTK_mmap_table[mmap_table_idx]) << 12;
        cache_mode = QURTK_get_cache_attr(QURTK_mmap_table[mmap_table_idx]);
        perm = QURTK_get_perm(QURTK_mmap_table[mmap_table_idx]);
        
        region = (mem_region_t *)QURTK_obj_cache_alloc(&QURTK_region_cache);
        if (region == NULL) {
          // Region objects exhausted.
          QURTK_fatal(ABORT_HEAP_EXHAUSTED);
        }
        
        region->phys_pool = hwio_pool;
        region->size = size;
        region->mapping_type = QURT_MEM_MAPPING_PHYS_CONTIGUOUS;
        region->cache_mode = (qurt_mem_cache_mode_t)cache_mode;
        region->perm = (qurt_perm_t)perm;
        region->owner_thread = 0UL;
        region->vma = NULL;

        // block the region in physical pool
        region->phys_mem = QURTK_mem_alloc_fixed( hwio_pool, size, vaddr );
        if(!region->phys_mem) {
            map_err = QURT_EMEM;
            break;
        }
        region->magic = QURT_MEM_MAPPING_PHYS_CONTIGUOUS;
        
        // block the region from HWIO_POOL (FIXME: hard-coded to use HWIO_POOL)
        region->virt_mem = QURTK_mem_alloc_fixed( &default_virt_pool, size, paddr );
        if(!region->virt_mem) {
            QURTK_mem_free((region->phys_pool), region->phys_mem);
            map_err = QURT_EMEM;
            break;
        }
        
        map_err = QURTK_mmap( vaddr, paddr, size, cache_mode, perm, KERNEL_ASID);
        if( map_err != QURT_EOK ) {
            QURTK_mem_free((region->phys_pool), region->phys_mem);
            QURTK_mem_free(&default_virt_pool, region->virt_mem);
            break;
        }
        QURTK_lifo_push(&QURTK_region_allocate_list, region );
        max_regions_alloced++;
        
        mmap_table_idx--;
    }

    qurt_mutex_unlock( &mem_lock );

    if( map_err != QURT_EOK ) {
        QURTK_obj_cache_free(&QURTK_region_cache, region);
        QURTK_fatal(QURT_EMEM);
    }
}

void static_pool_init(void){
    kprintf("Init memory pools...\n");
    
    //Add VA range from 4KB to 1GB (VA : 0x1000 - 0x40000000)
    mem_add(&default_virt_pool, FULL_VA_RANGE_START, (QURT_TXM_VA_RANGE_START - FULL_VA_RANGE_START) ); 
    
    /* Reserving QURT_TXM_VA_RANGE_SIZE(512 MB) VA from QURT_TXM_VA_RANGE_START(0x40000000) for ThreadX Module manager, by NOT doing a mem_add() for this range */

    // Add VA range from 1.5GB to (4GB - 16MB) (VA : 0x60000000 - 0xFF000000)
    mem_add(&default_virt_pool, (QURT_TXM_VA_RANGE_START + QURT_TXM_VA_RANGE_SIZE), (FULL_VA_RANGE_SIZE - (QURT_TXM_VA_RANGE_START + QURT_TXM_VA_RANGE_SIZE)) );

    phys_pool_init();
    
    static_mapping_init();
/* 
#ifdef CONFIG_TRACEBUFFER
    create_trace_buffers();
#endif */

}

/* struct memory object cache init function */
void mem_cache_init(void **free_obj_list, void *page) {
    int i, obj_size = sizeof(struct memory);
    int total_objs = SIZE_4K/obj_size;
    void *tmp;

    memsetK(page, 0, 4096);
    for (i = 0; i < total_objs; i++) {
        tmp = (void *)((char *)page + (i * obj_size));
        QURTK_lifo_push(free_obj_list, (void *)tmp);
    }
}


/* page table object cache init function*/
void pgtbl_cache_init(void **free_obj_list, void *page) {
    QURTK_lifo_push(free_obj_list, (void *)page);
}

/* mem_region_t object cache init function */
void mem_region_cache_init(void **free_obj_list, void *page) {
    int i, obj_size = sizeof(mem_region_t);
    int total_objs = SIZE_4K/obj_size;
    void *tmp;

    memsetK(page, 0, 4096);
    for (i = 0; i < total_objs; i++) {
        tmp = (void *)((char *)page + (i * obj_size));
        QURTK_lifo_push(free_obj_list, (void *)tmp);
    }
}

/* vma object cache init */
void vma_cache_init (void **free_obj_list, void *page) {
     int i, obj_size = sizeof(struct vma_node);
     int total_objs = SIZE_4K/obj_size;
     void *tmp;
     
    memsetK(page, 0, 4096);
      for (i = 0; i < total_objs; i++) {
           tmp = (void *)((char *)page + (i * obj_size));
           QURTK_lifo_push(free_obj_list, (void *)tmp);
      }
}

extern qurt_mutex_t mem_lock;

void QURTK_mem_init(void) {
    QURTK_heap_init();
    QURTK_obj_cache_init(&QURTK_memory_cache, mem_cache_init);
    QURTK_obj_cache_init(&QURTK_pgtbl_cache, pgtbl_cache_init);
    QURTK_obj_cache_init(&QURTK_region_cache, mem_region_cache_init);
    QURTK_obj_cache_init(&QURTK_vma_cache, vma_cache_init);
    static_pool_init();
    
    qurt_mutex_init(&mem_lock);
}


extern void zero_mem_range(word_t dest, word_t bytes);
extern uint32 Image$$BOOT$$Base;
extern uint32 Image$$ZI_REGION$$Base;
extern uint32 Image$$APPS_DATA_PAD$$Limit;
extern uint32 Image$$APPS_UNINIT_PAD$$ZI$$Limit;
extern uint32 Image$$APPS_END$$ZI$$Limit;

/* "Memory layout of APSS"
---------------------------------------------
|											|
|			 QURT_Memory_Pool				|	1 MB
|											| 
---------------------------------------------
|											| app_heap_end
|											|
|			 APPS Heap Region				| ~ 3.5 MB
|											|
|											| app_heap_start (next 4KB Aligned address) 
---------------------------------------------
|											| app_image_end (Image$$APPS_END$$ZI$$Limit)
|			    APPS Image					|
|											| ~ 6.5 MB
|											| 0x80000000
---------------------------------------------*/

int create_image_section() 
{
	int ret, max_available_amss_heap_size;
	uint32 specified_amss_heap_size = AMSS_HEAP_SIZE;
	app_heap_start = ((uint32)( apps_end_post_qsh + 0xFFF) & ~0xFFF);
    
    if(QURT_MEMPOOL_START_ADDRESS>=app_heap_start)	
        max_available_amss_heap_size = QURT_MEMPOOL_START_ADDRESS - app_heap_start;
    else
        QURTK_fatal("AMSS heap start is exceeding the QURT mempool.");
	#if (AMSS_HEAP_SIZE == 0)	
		app_heap_size = max_available_amss_heap_size;
	#else   
    if (specified_amss_heap_size > max_available_amss_heap_size){
	 	//Specified heap size is larger than max available, so restrict it to max available
		app_heap_size = max_available_amss_heap_size;
	}
	else {
		app_heap_size = specified_amss_heap_size;
	}
	#endif
	QURTK_mem_init();
	
	ret = qurt_mapping_create((uint32)&Image$$BOOT$$Base, (uint32)&Image$$BOOT$$Base, 
							 (uint32)&Image$$APPS_DATA_PAD$$Limit - (uint32)&Image$$BOOT$$Base  , 
							 QURT_MEM_CACHE_WRITEBACK, QURT_PERM_EXECUTE);
	if (ret != QURT_EOK)
		QURTK_fatal("Mapping creation failed for code region , ret = ", ret);

    ret = qurt_mapping_create((uint32)&Image$$APPS_DATA_PAD$$Limit, (uint32)&Image$$APPS_DATA_PAD$$Limit, 
    						 (uint32)&Image$$APPS_UNINIT_PAD$$ZI$$Limit - (uint32)&Image$$APPS_DATA_PAD$$Limit , 
    						 QURT_MEM_CACHE_WRITEBACK, QURT_PERM_READ_WRITE);
	if (ret != QURT_EOK)
		QURTK_fatal("Mapping creation failed for data region , ret = ", ret);
	
	ret = qurt_mapping_create((uint32)&Image$$APPS_UNINIT_PAD$$ZI$$Limit,(uint32)&Image$$APPS_UNINIT_PAD$$ZI$$Limit, 
                              (((uint32) apps_end_post_qsh  + 0xFFF) & ~(0xFFF)) - (uint32)&Image$$APPS_UNINIT_PAD$$ZI$$Limit , 
                              QURT_MEM_CACHE_WRITEBACK, QURT_PERM_READ);
	if (ret != QURT_EOK)
		QURTK_fatal("Mapping creation failed for ZI data region , ret = ", ret);
	
	return QURT_EOK;
}


