/*=============================================================================
 
									memory.c 
 
 GENERAL DESCRIPTION
		 Kernel memory functions
		 
 EXTERNAL FUNCTIONS
		None.
 
 INITIALIZATION AND SEQUENCING REQUIREMENTS
		None.
 
			 Copyright (c) 2009, 2021 - 2022
										 by Qualcomm Technologies Incorporated.	All Rights Reserved.
 
 =============================================================================*/
 
 /*=============================================================================
 
												 EDIT HISTORY FOR MODULE
 
	This section contains comments describing changes made to the module.
	Notice that changes are listed in reverse chronological order.
 
 
 $Header: //components/rel/core.tx/6.0/kernel/qurt/common/src/memory.c#4 $ 
 $DateTime: 2022/09/27 06:17:17 $ $Author: pwbldsvc $
 
 when 			who 		what, where, why
 -------- 	--- 		------------------------------------------------------------
 03/06/11 	op			Add header and history
 =============================================================================*/

#include "mem_utils.h"
#include "pgtable.h"
#include "qurt_thread.h"
#include "qapi_diag.h"

#define INVALID_ADDR QURT_INVALID_ADDR

#define QURT_LOG(...) 
//#define QURT_LOG(xx_fmt, ...) QAPI_MSG(MSG_SSID_DIAG, MSG_LEGACY_HIGH, xx_fmt,##__VA_ARGS__)

#ifdef TX_ENABLE_AWARE_CUSTOMIZATION
#define MAX_POOLS_TOTAL     (MAX_POOLS + 1)
#else
#define MAX_POOLS_TOTAL     (MAX_POOLS)
#endif

uint32   default_virt_pool_start;
uint32   default_virt_pool_size;

/* unsigned long long query_static_mapping (uint32 paddr, uint32 p_size, 
                                     qurt_mem_cache_mode_t cache_attribs,
                                     qurt_perm_t perm);
 */
//mem_region_t * g_region_free_list = NULL;
mem_region_t * QURTK_region_allocate_list = NULL;
struct vma_node * vma_list = NULL;


uint32 QURTK_pgsize_encode_to_size[] = {
    SIZE_4K,
    SIZE_64K,
    SIZE_1M,
    SIZE_16M,
};

uint32 max_regions_alloced = 0;
uint32 asid_allocation_bits = 0x1; // bit 0 is reserved for kernel space
qurt_mutex_t mem_lock;

extern unsigned long long QURTK_mmap_table[];
extern int QURTK_mmap_table_size;
extern struct mem_range QURTK_vmem_ranges[];
extern struct mem_range QURTK_pmem_ranges[];
extern QURTK_obj_cache_t QURTK_region_cache;
extern QURTK_obj_cache_t QURTK_memory_cache;
extern QURTK_obj_cache_t QURTK_vma_cache;
extern struct mem_pool default_virt_pool;

extern int arm_map_page (word_t vaddr, word_t paddr, word_t size,
                         word_t cache_attribs, word_t perm, word_t asid);
extern int arm_unmap_page (word_t vaddr, word_t size, word_t asid);
extern int arm_pagetable_lookup (word_t vaddr, word_t *paddr,  word_t *size, 
                                 word_t *cache_attribs, word_t *perm, word_t *asid);
extern qurt_mem_region_type_t cache_attr_to_region_type (qurt_mem_cache_mode_t cache_attribs);
extern qurt_mem_cache_mode_t region_type_to_cache_attr (qurt_mem_region_type_t region_type);

int QURTK_mem_region_queryK (uint32 *region_handle, uint32 vaddr, uint32 paddr);
void sys_mem_region_get_attr(qurt_mem_region_t region_obj, qurt_mem_region_attr_t *attr);

void mem_pool_dump (struct mem_pool * pool);
void print_glob_list(struct mem_pool *pool);

struct vma_node * vma_search(uint32 vaddr);


static uint32 addr_to_best_page( uint32 addr ){
    uint32 i;
    i = qurt_rbit(addr);
    i = qurt_clz(i);
    QURT_LOG( "best page for %p index %d:%d\n", addr, i,((i-12)>>2));
    i = ((i-12)>>2);
    return i <= 3 ? i: 3;
}

uint32 size_to_best_page( uint32 size ) {
    uint32 i;
    i = qurt_clz(size);
    i = 31 - i;
    QURT_LOG( "best page for size %p index %d:%d\n", size, i,((i-12)>>2));
    i = ((i-12)>>2);
    return i <= 3 ? i: 3;
}

#define min(x,y) ((x)<=(y)?(x):(y))

int QURTK_mmap(uint32 vaddr, uint32 paddr, uint32 size, uint32 cache_mode, uint32 perm, uint32 asid) {

    uint32 best_virt_page, l_vaddr = ~0UL;
    uint32 best_phys_page, l_paddr = ~0UL;
    uint32 best_size_page, l_size = 0UL;
    uint32 page;
    uint32 real_size;
    int    ret=~(QURT_EOK), add_page_count = 0;

    //Assure that it is 4K aligned
    QURTK_assert( (size & 0xfff) == 0 );

    while( size ) {
        best_size_page = size_to_best_page( size );
        best_virt_page = addr_to_best_page( vaddr );
        best_phys_page = addr_to_best_page( paddr );
        page = min(best_phys_page , best_virt_page );
        page = min( page, best_size_page );
        real_size = QURTK_pgsize_encode_to_size[page];
        QURT_LOG("mmap best size %d real size %p\n",page,real_size);

        ret = arm_map_page(vaddr, paddr, real_size, cache_mode, perm, asid);
        if (ret != QURT_EOK)
            break;
        
        size -= real_size;
        vaddr += real_size;
        paddr += real_size;
        add_page_count++;
        QURT_LOG( "mmap vaddr %p paddr %p size %p\n", vaddr, paddr, size);
    }
    
    if (ret != QURT_EOK) {
        while(add_page_count--) {
            best_size_page = size_to_best_page( l_size );
            best_virt_page = addr_to_best_page( l_vaddr );
            best_phys_page = addr_to_best_page( l_paddr );
            page = min(best_phys_page , best_virt_page );
            page = min( page, best_size_page );
            real_size = QURTK_pgsize_encode_to_size[page];
            QURT_LOG( "mmap fail best size %d real size %p\n", page,real_size );

            arm_unmap_page (l_vaddr, real_size, asid);
            
            l_size -= real_size;
            l_vaddr += real_size;
            l_paddr += real_size;
            QURT_LOG( "mmap fail vaddr %p paddr %p size %p\n", l_vaddr, l_paddr, l_size);
        }
    }
    return ret;
}


int QURTK_munmap(uint32 vaddr, uint32 paddr, uint32 size, uint32 asid) {

    uint32 best_virt_page;
    uint32 best_phys_page;
    uint32 best_size_page;
    uint32 page;
    uint32 real_size;

    //Assure that it is 4K aligned
    QURTK_assert( (size & 0xfff) == 0 );

    while( size ){
        best_size_page = size_to_best_page( size );
        best_virt_page = addr_to_best_page( vaddr );
        best_phys_page = addr_to_best_page( paddr );
        page = min(best_phys_page , best_virt_page );
        page = min( page, best_size_page );
        real_size = QURTK_pgsize_encode_to_size[page];
        QURT_LOG( "munmap best size %d real size %p\n", page,real_size );

        arm_unmap_page (vaddr, real_size, asid);
        
        size -= real_size;
        vaddr += real_size;
        paddr += real_size;
        QURT_LOG( "munmap vaddr %p paddr %p size %p\n", vaddr, paddr, size);
    }
    return QURT_EOK;
}


int mem_region_create_imp(
    mem_region_t             **ret_region,
    mem_pool_t               * pool,
    uint32                     size,
    qurt_mem_mapping_t         mapping_type,
    uint32                     phys_addr,
    uint32                     virt_addr,
    qurt_mem_cache_mode_t      cache_mode,
    qurt_perm_t                perm,
    uint32                     asid
) {
    struct vma_node * vma_node = NULL;
    mem_region_t * region = NULL;
    int map_err;
    
    qurt_mutex_lock( &mem_lock );
    //kprintf( "mem_region_create space %p pool %p size %p mapping_type %p phys_addr %p virt_addr %p cache_mode %p type %p perm %p\n", space, pool, size, mapping_type, phys_addr, virt_addr, cache_mode, type, perm );
    
    //kprintf( "space->default_virt_pool %p share_virt_pool %p\n", &(space->default_virt_pool), &(space->shared_virt_pool));
    //mem_region_t * region = (mem_region_t *)QURTK_obj_cache_alloc(&QURTK_region_cache);
    
    /* Add buffer into appropriate region allocate list for debugging purpose */
    if (mapping_type != QURT_MEM_MAPPING_NONE){
      region = (mem_region_t *)QURTK_obj_cache_alloc(&QURTK_region_cache);
      if (region == NULL) {
          // Region objects exhausted.
          QURTK_fatal(ABORT_HEAP_EXHAUSTED);
      }
    }
 
    //if analigned addresses, align address, adjust size:
    if (((unsigned long)phys_addr != INVALID_ADDR)&&((unsigned long) phys_addr << 20)) //unaligned address
    {
       size += (phys_addr & 0x00000fff);
       phys_addr = phys_addr & 0xfffff000;
    }
    if (((unsigned long)virt_addr != INVALID_ADDR) && ((unsigned long) virt_addr << 20))
    {
      if (mapping_type == QURT_MEM_MAPPING_VIRTUAL_FIXED)
         size += (virt_addr & 0x00000fff);

      virt_addr = virt_addr & 0xfffff000;
    }  

    size = size_round_up(size, SIZE_4K);
    
    if (mapping_type != QURT_MEM_MAPPING_NONE) {
      region->phys_pool = pool;
      region->size = size;
      region->mapping_type = mapping_type;
      region->cache_mode = cache_mode;
      region->perm = perm;
      region->owner_thread = (unsigned int)qurt_thread_get_id();
      region->vma = NULL;
      region->asid = asid;
   }

    switch (mapping_type) {
       case QURT_MEM_MAPPING_VIRTUAL:
          if( ((unsigned long)phys_addr != INVALID_ADDR) ||
              ((unsigned long)virt_addr != INVALID_ADDR)  ){
              QURTK_fatal(ABORT_ARG_ERROR);
          }
          region->magic = QURT_MEM_MAPPING_VIRTUAL;
          //With current allocator, we don't have the case that return a memory
          //with smaller than than required.  Meaning, all memory allocated are
          //contiguous
          region->phys_mem = QURTK_mem_alloc( (region->phys_pool), size );
          if(!region->phys_mem){
              goto QURT_EMEM_bail_out;
          }

          //We intentionally allocation virtual same as physical to have
          //idempotent mapping.  It is not necessary, but just for easy debugging <- removed now
          region->virt_mem = QURTK_mem_alloc_fixed (&default_virt_pool, size, region->phys_mem->addr);
          if (!region->virt_mem) {
            //best effort to do idempotent mapping fails, use non-1-to-1 mapping instead
            region->virt_mem = QURTK_mem_alloc (&default_virt_pool, size);
          }
        
          if(!region->virt_mem){
              QURTK_mem_free( (region->phys_pool), region->phys_mem);
              goto QURT_EMEM_bail_out;
          }

          map_err = QURTK_mmap(region->virt_mem->addr, region->phys_mem->addr, size, cache_mode, perm, asid);
          if( map_err != QURT_EOK ) {
            QURTK_mem_free((region->phys_pool), region->phys_mem);
            QURTK_mem_free(&default_virt_pool, region->virt_mem);
            goto QURT_EMEM_bail_out;
          }
          QURTK_lifo_push(&QURTK_region_allocate_list, region );
          max_regions_alloced++; //count only non-vma?
                      
          qurt_mutex_unlock( &mem_lock );
          *ret_region = region;
          return QURT_EOK;
    
      case QURT_MEM_MAPPING_PHYS_CONTIGUOUS:
          if( (unsigned long)phys_addr != INVALID_ADDR ) {
              region->phys_mem = QURTK_mem_alloc_fixed( (region->phys_pool), size, phys_addr );
              if(!region->phys_mem){
                 goto QURT_EMEM_bail_out;
              }
          } /* else {
            QURTK_fatal("Error: Need provide physical address using QURT_MEM_MAPPING_PHYS_CONTIGUOUS!\n");
          } */
          //disallow PHYS_CONTIGUOUS to allocate with non-specified physaddr
          else{
              region->phys_mem = QURTK_mem_alloc( (region->phys_pool), size);
              if(!region->phys_mem){
                goto QURT_EMEM_bail_out;
              }
            }
        
          region->magic = QURT_MEM_MAPPING_PHYS_CONTIGUOUS;
          if (virt_addr == INVALID_ADDR)
          {
            //We intentionally allocation virtual same as physical to have
            //idempotent mapping.  It is not necessary, but just for easy debugging
            virt_addr = region->phys_mem->addr;
          }
          //allocate any free virtual range, not idempotent to phys addr
          region->virt_mem = QURTK_mem_alloc_fixed ( &default_virt_pool, size, virt_addr );
          if (!region->virt_mem)
            //best effort to do idempotent mapping fails, use non-1-to-1 mapping instead
            region->virt_mem = QURTK_mem_alloc ( &default_virt_pool, size );

          if(!region->virt_mem){
              QURTK_mem_free( (region->phys_pool), region->phys_mem);
              goto QURT_EMEM_bail_out;
          }

          map_err = QURTK_mmap(region->virt_mem->addr, region->phys_mem->addr, size, cache_mode, perm, asid);
          if( map_err != QURT_EOK ) {
            QURTK_mem_free((region->phys_pool), region->phys_mem);
            QURTK_mem_free(&default_virt_pool, region->virt_mem);
            goto QURT_EMEM_bail_out;
          }
          
          QURTK_lifo_push(&QURTK_region_allocate_list, region );
          max_regions_alloced++; //count only non-vma?
                      
          qurt_mutex_unlock( &mem_lock );
          *ret_region = region;
          return QURT_EOK;
   
       case QURT_MEM_MAPPING_NONE:
          vma_node = (struct vma_node *)QURTK_obj_cache_alloc(&QURTK_vma_cache);
          if (vma_node == NULL)
             goto QURT_EMEM_bail_out;

		  if ((unsigned long)virt_addr != INVALID_ADDR) {
			  vma_node->virt_mem = QURTK_mem_alloc_fixed ( &default_virt_pool, size, virt_addr);
              if(vma_node->virt_mem == NULL)
	             goto QURT_EMEM_bail_out;
		  } else {
              vma_node->virt_mem = QURTK_mem_alloc( &default_virt_pool, size);
              if(vma_node->virt_mem == NULL)
	             goto QURT_EMEM_bail_out;
          }
          QURTK_lifo_push(&vma_list, vma_node);
          QURT_LOG ("\t\t\t\t\t vma node :  self: %x addr %x, range %x, next %x\n", 
	  	   vma_node, vma_node->virt_mem->addr, vma_node->virt_mem->size, vma_node->next);
          vma_node->cnt = 0; 
          vma_node->magic = QURT_MEM_MAPPING_NONE;

          qurt_mutex_unlock (&mem_lock);
          *ret_region = (mem_region_t *)vma_node;//vma_handle
          return QURT_EOK;

      case QURT_MEM_MAPPING_IDEMPOTENT:
          if ( (unsigned long)phys_addr == INVALID_ADDR) {
               QURTK_fatal(ABORT_ARG_ERROR); 
           }
           region->magic = QURT_MEM_MAPPING_IDEMPOTENT; 
           //both addresses need to be provided
           region->phys_mem = QURTK_mem_alloc_fixed ((region->phys_pool), size, phys_addr);
           if (!region->phys_mem)
              goto QURT_EMEM_bail_out;

          region->virt_mem = QURTK_mem_alloc_fixed ( &default_virt_pool, size, phys_addr);
          if (!region->virt_mem){
             QURTK_mem_free( (region->phys_pool), region->phys_mem);
             goto QURT_EMEM_bail_out;
          }

          if (region->virt_mem->addr != phys_addr) 
             goto QURT_EMEM_bail_out; //or fatal error?

          map_err = QURTK_mmap(region->virt_mem->addr, region->phys_mem->addr, size, cache_mode, perm, asid);
          if( map_err != QURT_EOK ) {
            QURTK_mem_free((region->phys_pool), region->phys_mem);
            QURTK_mem_free(&default_virt_pool, region->virt_mem);
            goto QURT_EMEM_bail_out;
          }
          QURTK_lifo_push(&QURTK_region_allocate_list, region );
          max_regions_alloced++; //count only non-vma?
                      
          qurt_mutex_unlock ( &mem_lock );
          *ret_region = region;
          return QURT_EOK;
       
        default:
          qurt_mutex_unlock( &mem_lock );
          QURTK_fatal(ABORT_INVALID_MEM_MAPPING_TYPE);
    }

QURT_EMEM_bail_out:
    if (mapping_type == QURT_MEM_MAPPING_NONE) {
        if (vma_node)
           QURTK_obj_cache_free(&QURTK_vma_cache, vma_node);
    } else {
       //QURTK_lifo_remove(&QURTK_region_allocate_list, region );
       //max_regions_alloced--;
       QURTK_obj_cache_free(&QURTK_region_cache, region);
    }
    qurt_mutex_unlock( &mem_lock );
    *ret_region = NULL;
    return QURT_EMEM;

}


int mem_region_deleteK (qurt_mem_region_t region_obj) { 

    mem_region_t *region = (mem_region_t *)region_obj;
    struct vma_node * vma_node = NULL;
    mem_pool_t *phys_pool = NULL;
    
    qurt_mutex_lock ( &mem_lock );
	
    QURTK_assert (region != NULL);
    if (region->magic != QURT_MEM_MAPPING_NONE)
        phys_pool = region->phys_pool;    

    QURT_LOG ("deleting region %x, type %d\n", region, region->magic);    

   if (region->magic == QURT_MEM_MAPPING_NONE){
      //kprintf ("NONE\n");
      vma_node = (struct vma_node *)region;
      //check cnt == 0, remove from vma_list, release virt mem
      if (vma_node->cnt != 0)
      {
          kprintf ("!!! WARNING: virtual memory area still in use, delete vma node 0x%x FAILED ....\n", vma_node);
          //QURTK_fatal(ARG_ERROR); //should be a different error
          qurt_mutex_unlock ( &mem_lock );
          return QURT_EMEM;
      }
      else
      {
          QURT_LOG("removing node vma: %x\n", vma_node);
          QURT_LOG("vma_node->virt_mem: %x\n", vma_node->virt_mem->addr);
          QURTK_lifo_remove(&vma_list, vma_node);
          QURTK_mem_free ( &default_virt_pool, vma_node->virt_mem);
          QURTK_obj_cache_free(&QURTK_vma_cache, region);
      }
   }
   else if (region->magic == QURT_MEM_MAPPING_VIRTUAL_FIXED)
   {   
       QURT_LOG("VF: vaddr: %x\n", region->virt_mem->addr);
       max_regions_alloced--;
       QURTK_lifo_remove(&QURTK_region_allocate_list, region );
       QURTK_munmap( region->virt_mem->addr, region->phys_mem->addr, region->size , region->asid);
       region->vma->cnt--;
       QURTK_mem_free( (region->phys_pool), region->phys_mem);
       QURTK_obj_cache_free(&QURTK_memory_cache, region->virt_mem);
       QURTK_obj_cache_free(&QURTK_region_cache, region);
   }
   else
   {
       QURT_LOG("deleting qurt region: %x virt addr: %x, magic: %d\n", region, region->virt_mem->addr, region->magic);
       max_regions_alloced--;
       QURTK_lifo_remove(&QURTK_region_allocate_list, region );
       QURTK_munmap( region->virt_mem->addr, region->phys_mem->addr, region->size, region->asid );
       QURTK_mem_free(&default_virt_pool, region->virt_mem);
       QURTK_mem_free( (region->phys_pool), region->phys_mem);
       QURTK_obj_cache_free(&QURTK_region_cache, region);
   }

    //print pools: enable ALLOC_POOL_DUMP_DEBUG in alloc.c
    if (phys_pool!= NULL) {
      mem_pool_dump (phys_pool);
      print_glob_list (phys_pool);
    }
    print_glob_list (&default_virt_pool);
    
    //QURTK_mem_free( (region->phys_pool), region->phys_mem);
    //QURTK_obj_cache_free(&QURTK_region_cache, region);
    qurt_mutex_unlock ( &mem_lock );
    return QURT_EOK;
}


int strncmpK( char * dest, const char *src, uint32 size ){
    int i;
    for ( i = 0 ; i < size; i++ ){
        if( dest[i] != src[i] ){
            return 1;
        }
        if( dest[i] == '\0' ){
            return 0;
        }
    }
    return 0;
}

extern phys_pool_t QURTK_phys_pools[MAX_POOLS_TOTAL];

int mem_pool_attachK(const char *name, uint32 *pool) {
    int i;
    for( i = 0 ; i < MAX_POOLS_TOTAL; i++ ){
        if(!strncmpK(QURTK_phys_pools[i].name,name,32)){
            (*pool) = (uint32)&QURTK_phys_pools[i].pool;
            return QURT_EOK;
        }
    }
	QURTK_fatal(ABORT_NOPOOL);
    return QURT_EVAL;
}


struct vma_node * vma_search(uint32 vaddr)
{
   struct vma_node * temp;
   temp = vma_list;
   do{
       if((temp->virt_mem->addr <= vaddr) &&
          ((temp->virt_mem->addr + temp->virt_mem->size) > vaddr))
       {
           //found the vma
           //vma_ptr = temp;
           break;
       } 
       temp = temp->next;
   }while (temp != NULL);
   return temp;
}


int QURTK_mem_region_queryK (uint32 *region_handle, uint32 vaddr, uint32 paddr)
{
  int err = QURT_EINVALID; //error code
  //search entire list for a given paddr
  //return handle of the corresponding region  
  mem_region_t *temp; 
  temp = QURTK_region_allocate_list; //point at the head, there is at least one region: image
  QURTK_assert (temp != NULL); //there is at least one region: image itself
  *region_handle = INVALID_ADDR;

  if ((vaddr == INVALID_ADDR) && (paddr == INVALID_ADDR))
  {
     QURT_LOG ("invalid arguments......., region_handle: %x\n", *region_handle);
     err = QURT_EINVALID; //QURT_EFATAL; //invalid parameters
  }
  else if (vaddr == INVALID_ADDR) //search based on phys addr
  {
     do{
       if ((temp->phys_mem->addr <= paddr) && 
           ((temp->phys_mem->addr + temp->size) > paddr))
       {
           //return region handle
           *region_handle = (uint32 )temp; //should be void 
           err = QURT_EOK;
           break;
       }
       temp = temp->next;
     } while (temp != NULL);
  }
  else if (paddr == INVALID_ADDR) //search based on virt addr
  {
     do{
         QURT_LOG ("cur_node %x cur_addr %x curr_size %x\n", temp, temp->virt_mem->addr, temp->size);
         if ((temp->virt_mem->addr <= vaddr) &&
             ((temp->virt_mem->addr + temp->size) > vaddr))
         {
             //return region handle
             *region_handle = (uint32) temp;
             err = QURT_EOK;
             break;
         }
         temp = temp->next;
      } while (temp != NULL);
   }

  if (temp == NULL) 
     err = QURT_EMEM; //1 not found, else 0
  //in case both parameters are set, temp_ptr != NULL, so err=-1 remains
  
  //explicitly shove *vaddr into r1, similarly to get_attr, this is temporary work-around
/*    __asm__ __volatile__ (
   "r1 = %0\n"
       :  
       :"r"(*region_handle)
       :"r1"
       ); 
 */
  return err;
}

 
int mem_region_queryK(uint32 *vaddr, uint32 paddr, uint32 size, qurt_mem_cache_mode_t cache_attribs, qurt_perm_t perm) {

    int ret, offset;
    uint32 region_handle;
    qurt_mem_region_attr_t attr;
   
    ret = QURTK_mem_region_queryK(&region_handle, INVALID_ADDR, paddr);
    if (ret != QURT_EOK) {
        *vaddr = INVALID_ADDR;
        return QURT_EMEM;
    }
    
    sys_mem_region_get_attr((qurt_mem_region_t)region_handle, &attr);
    if ((cache_attribs == attr.cache_attribs) &&
        (perm == attr.permission)            &&
        (paddr >= attr.physaddr)            &&
        ((paddr + size) <= (attr.physaddr + attr.size))) {
        
        if (paddr > attr.physaddr)
            offset = paddr - attr.physaddr;
        else
            offset = 0;
        
        *vaddr = attr.virtaddr + offset;
        ret = QURT_EOK;
    } else {
        *vaddr = INVALID_ADDR;
        ret = QURT_EVAL;
    }
    
    return ret;
}

int mmap_attr_lookup (uint32 vaddr, uint32 *paddr, uint32 *size, uint32 *cache_attribs, uint32 * perm, uint32 *asid) {
   
    return arm_pagetable_lookup (vaddr, paddr, size, cache_attribs, perm, asid);
}

uint32 mem_lookup_physaddrK (uint32 vaddr) {
   
    uint32 offset, paddr, size, cache_attribs, perm, asid;
    int ret;

    ret = mmap_attr_lookup (vaddr, &paddr, &size, &cache_attribs, &perm, &asid);

    if (ret != QURT_EOK)
        return 0;

    offset = vaddr - (vaddr & ~(size - 1));
    return paddr + offset;
}


int mem_region_createK(
    uint32                  *obj,
    uint32                   size,
    void                    *pool,
    qurt_mem_region_attr_t  *attr
) {

    int r;
//    qurt_perm_t perm;
    mem_region_t * region;

    //kprintf ("printing mapping: %d, virtual addr: %x\n", attr->mapping_type, attr->virtaddr);
    r = mem_region_create_imp( &region, (mem_pool_t *)pool, size, attr->mapping_type, attr->physaddr, attr->virtaddr, attr->cache_attribs, attr->permission, attr->asid);
    if( r == QURT_EOK )
    {   
       if (attr->mapping_type == QURT_MEM_MAPPING_NONE)
           QURT_LOG ("kernel created region %p mapping NONE\n", region);
       else if (attr->mapping_type != QURT_MEM_MAPPING_VIRTUAL_FIXED)
       {    
           QURT_LOG( "kernel created region %p vaddr %p physaddr %p next %p \n", region, region->virt_mem->addr, 
									region->phys_mem->addr, region->next  );
       }
       else
       {
           QURT_LOG( "kernel created region %p vaddr %p physaddr %p, vma_head: %x\n", region, region->virt_mem->addr, 
							region->phys_mem->addr, region->vma);   
       }  
       (*obj)= (uint32)region;
        //(*obj) = region->obj.raw;
    }
    return r;
}


void sys_mem_region_get_attr(qurt_mem_region_t region_obj, qurt_mem_region_attr_t *attr) {
    struct vma_node *vregion;
    mem_region_t * region = (mem_region_t *)region_obj;

    if (region->magic == QURT_MEM_MAPPING_NONE) {
       vregion = (struct vma_node *)region_obj;
       attr->virtaddr = vregion->virt_mem->addr;
       attr->size = vregion->virt_mem->size;
       
       attr->mapping_type = QURT_MEM_MAPPING_NONE;
       attr->region_type = QURT_MEM_REGION_INVALID;
       attr->cache_attribs = QURT_MEM_CACHE_INVALID;
       attr->permission = QURT_PERM_NONE;
       attr->physaddr = INVALID_ADDR;
       attr->asid = 0;
    } else {
       attr->virtaddr = region->virt_mem->addr;
       attr->physaddr = region->phys_mem->addr;
       
       attr->mapping_type = region->mapping_type;
       attr->region_type = cache_attr_to_region_type(region->cache_mode);
       attr->cache_attribs = region->cache_mode;
       attr->permission = region->perm;
       attr->size = region->size;
       attr->asid = region->asid;
    }
}


int QURTK_asid_allocate() {

    int asid;
    
    asid = 32 - qurt_clz(asid_allocation_bits);
    if ((asid == 0) || (asid >= QURT_MAX_ASIDS)) {
        return QURT_EFAILED;
    } else {
        asid_allocation_bits |= 1 << asid;
        return asid;
    }
}

int QURTK_asid_remove(int asid) {
    
    if ((asid == 0) || (asid >= QURT_MAX_ASIDS)) {
        return QURT_EFAILED;
    }
    if (asid_allocation_bits & (1 << asid)) {
        asid_allocation_bits &= ~(1 << asid);
        return QURT_EOK;
    } 
    return QURT_EFAILED;
}
