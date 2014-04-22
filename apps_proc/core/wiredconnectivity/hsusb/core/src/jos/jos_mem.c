/************************************************************************** 
*************************   Restricted access   *************************** 
*************************************************************************** 












This file must only be used for the development of the HSUSB 
driver for the AMSS / BREW SW baselines using the Jungo USB Stack.
This file must not be used in any way for the development of any
functionally equivalent USB driver not using the Jungo USB stack.

For any questions please contact: Sergio Kolor, Liron Manor,
Yoram Rimoni, Dedy Lansky.


==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/core/src/jos/jos_mem.c#1 $
  $DateTime: 2020/01/30 22:49:35 $
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------


==============================================================================









************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */
#define INCLUDE_MEM
#define INCLUDE_PCI
#include "jos_internal.h"
#include "hsu_log.h"
#include "hsu_common_int.h"

/* HSU ADDITION +++
** Uncomment below to debug memory leak.
*/
#ifdef FEATURE_HS_USB_TRACE
#define CONFIG_MEMPOOL_USAGE_SHOW
#ifndef JDEBUG
  #define JDEBUG
#endif //JDEBUG
#endif /* FEATURE_HS_USB_TRACE */
/** HSU ADDITION --- 
*/

#ifndef CONFIG_MEMPOOL_ALIGN
# define CONFIG_MEMPOOL_ALIGN (sizeof(void *))
#endif

#ifdef CONFIG_MEMPOOL

#ifdef CONFIG_MEMPOOL_USAGE_SHOW
# ifndef CONFIG_MEMPOOL
#  error "Cannot show mempool statistics when mempool is not configured !"
# endif
#endif

void *jmemp_alloc(juint32_t size, juint16_t flags);
void jmemp_free(void *item);
#endif

#if defined(JDEBUG) || defined(CONFIG_MEMPOOL_USAGE_SHOW)
static jint32_t total_dma_peak_curr = 0;
static jint32_t blocks_used_curr = 0;
static jint32_t total_dma_free_items_curr = 0;
static jint32_t total_dma_free_items_max = 0;
static jint32_t total_dma_items = 0;
#endif

#if defined(JDEBUG)
#define MEM_LOG_MAGIC_START     0x2B1A11CC
#define MEM_LOG_MAGIC_END       0x3C2B22DD

typedef struct jmemory_info_t 
{
    jint32_t     magic_start;
    char        *file;
    jint32_t     line;
    unsigned long size;
    unsigned long size_w_overhead;
    void        *org;
    void        *user_ptr;
    jint32_t     page_allocated;
    jint32_t     freed;
    jint32_t     id;
    struct jmemory_info_t *next, *prev;
    jint32_t     magic_end;
} jmemory_info_t;

#ifdef FEATURE_HS_USB_MEM_TEST
// Special structure only used for running various memory related tests
typedef struct jmemory_test_t
{
  void *vaddr;
  void *paddr;
  void *cookie; // handle
  juint32_t size;
  struct jmemory_test_t *next;
} jmemory_test_t;
#endif // FEATURE_HS_USB_MEM_TEST

static jmemory_info_t mem_log_head[] = {{
    MEM_LOG_MAGIC_START, "head", 0, 0, 0, (void*)0xffff, NULL,
    0, 0, 0, NULL, NULL, ~MEM_LOG_MAGIC_START }};

static jint32_t mem_log_id = 0;
void mem_log_print(void)
{
    jmemory_info_t *curr;
    jint32_t total_allocs = 0;
    jint32_t total_mem = 0;
    char * filename = NULL;
    juint16_t str_len = 0;
    juint16_t i = 0;

    HSU_ULOG_NOFN(NO_MSG, DBG_E_LOG, "DJOS");
    HSU_ULOG_NOFN(NO_MSG, DBG_E_LOG, "DJOS, MEM: starting_memory_log_dump");
    
    for (curr = mem_log_head->next; curr; curr = curr->next)
    {
        total_mem += curr->size;
        total_allocs ++;

        /* calculate the string length */
        filename = curr->file;
        while( *filename++ ) ;
        str_len = (int)(filename - curr->file - 1);

        /* find the last "\" in the string, point to the first character after it */
        filename = curr->file;
        for(i = str_len-1 ; i > 0 ; --i)
        {
          if (filename[i] == '\\')
          {
            filename = &(filename[i+1]);
            break;
          }
        }

        HSU_ULOG_NOFN_4(NO_MSG, DBG_E_LOG, "DJOS, MEM: ID %u, line %u, size %u, user_ptr 0x%X", curr->id, curr->line, curr->size, (uint32)curr->user_ptr);
        HSU_ULOG_NOFN_5(NO_MSG, DBG_E_LOG, "%c%c%c%c%c", filename[0],filename[1],filename[2],filename[3],filename[4]);
//        DBG_E(DJOS, ("MEM: %ld: %c%c%c%c%c:%ld - Size %ld    -  %p\n", 
//            curr->id,
//            filename[0],filename[1],filename[2],filename[3],filename[4],
//       /    curr->line, curr->size, curr->user_ptr));
    }
    HSU_ULOG_NOFN_2(NO_MSG, DBG_E_LOG, "DJOS, MEM: total_bad_entries %u, using memory_size %u (byte)", total_allocs, total_mem);
    HSU_ULOG_NOFN_2(NO_MSG, DBG_E_LOG, "DJOS, MEM: un_freed_DMA_entries %u, using dma_size %u (byte)", blocks_used_curr, total_dma_peak_curr);

}
#else
void mem_log_print(void)
{
}

#endif

/* 
 * Statically allocated memory management 
 *
 * Automaticaly allocate fragments from a large buffer passed during
 * initialization of the stack. Used instead of having to call the OS's
 * os_alloc/jos_free/os_dma_alloc/etc. */

#ifdef CONFIG_MEMPOOL

typedef enum {
    POOL_VIRTUAL,
#ifdef CONFIG_POOL_DMA 
    POOL_DMA,
# ifdef CONFIG_POOL_DMA_CACHABLE
    POOL_DMA_CACHABLE,
# endif
#endif    
    POOL_LAST
} pool_types;
   
static juint32_t pool_offset[POOL_LAST];
static juint32_t pool_size[POOL_LAST];
static juint8_t *pool_vbuf[POOL_LAST];
static juint8_t *pool_pbuf[POOL_LAST];

static void pool_init(pool_types type, void *vstart, 
    void *pstart, juint32_t size)
{
    juint32_t offset;
  
    DBG_I(DJOS_MEM, ("MEM: New pool %d size %d from [%x:%x]\n",
        type, size, vstart, pstart));

    offset = (juint32_t)(vstart) % CONFIG_MEMPOOL_ALIGN;
    if (offset)
        offset = CONFIG_MEMPOOL_ALIGN - offset;

    pool_offset[type] = offset;
    pool_size[type] = size;
    pool_vbuf[type] = (juint8_t *)vstart;
    pool_pbuf[type] = (juint8_t *)pstart;
}

static jresult_t pool_alloc(juint32_t size, void **vaddr, void **paddr, 
    pool_types type)
{
    juint32_t alloc_size = size;

    DBG_I(DJOS_MEM, ("MEM: Pool alloc %d from pool %d\n", size, type));
    
    if (alloc_size % CONFIG_MEMPOOL_ALIGN)
    {
        alloc_size = (alloc_size + CONFIG_MEMPOOL_ALIGN) - 
            (alloc_size % CONFIG_MEMPOOL_ALIGN);
    }

    if (pool_offset[type] + alloc_size >= pool_size[type])
    {
        HSU_ULOG_3(ERROR_MSG, DBG_E_LOG, "DJOS_MEM,"
            " not_enough_memory_on_pool, need %ld, have %ld", 
            type, size, (pool_size[type] - pool_offset[type])); 
        return JENOMEM;
    }
    
    if (vaddr)
        *vaddr = (void *)&((pool_vbuf[type])[pool_offset[type]]);

    if (paddr)
        *paddr = (void *)&((pool_pbuf[type])[pool_offset[type]]);

    pool_offset[type] += alloc_size;

    return 0;
}
#endif

/* 
 * DMA Memory pool management 
 *
 * We need to allocate a lot of small (many 8 byte, some larger)
 * memory blocks that can be used for DMA.  Using the bus_dma
 * routines directly would incur large overheads in space and time. */

#define DMA_MEM_BLOCK   32

#ifdef CONFIG_MEMPOOL_USAGE_SHOW
static juint32_t blocks_used_max = 0;
static juint32_t total_dma_peak_max = 0;

# define DBG_INC(x)             ((x)++)
# define DBG_DEC(x)             ((x)--)
#else
# define DBG_INC(x)
# define DBG_DEC(x)
#endif

#ifdef CONFIG_MEMPOOL

/* 
 * Implementation of Memory Pool allocation
 * ----------------------------------------
 * The code below allocates a single large block of memory from the
 * porting layer during the stack initialization and uses block lists to 
 * support basic malloc/free 
 * 
 * */

/* Set the distribution of item sizes */
// To prevent under-utilization, the increments must be at most multiples of 2 of the previous entry
juint32_t alloc_list_size[] =    {16, 32, 64, 128, 256, 512, 1024, 1536, 2048, 3072, 4096, 5120, 6144, 8192, 9216};
#define ALLOC_LIST_GET(i)       (alloc_list_size[i])

#define ALLOCATION_LISTS        (sizeof(alloc_list_size) / sizeof(juint32_t))

/* Holds the head of all allocation lists - last one for un-even sized blocks */
void *alloc_list[ALLOCATION_LISTS + 1];

#define ITEM_NEXT(x)            (*((void **)(x)))
#define ITEM_SIZE(x)            (((juint32_t *)(x))[-1])

#ifdef CONFIG_MEMPOOL_USAGE_SHOW
# define ITEM_SIZE_USED(x)      (((juint32_t *)(x))[-2])
static juint32_t alloc_list_watch[ALLOCATION_LISTS + 1][2];
static juint32_t total_peak_curr = 0;
static juint32_t total_peak_max = 0;
static juint32_t total_allocated = 0;

static void dump_mem_status(void);
#endif

static void *memp_item_alloc(juint32_t size);
static void *memp_item_get(void **head, juint32_t size);
static void *memp_item_get_ex(void **head, juint32_t size);

#ifdef JDEBUG
  #define DEBUG_ALLOC_OVERHEAD \
    (sizeof(jmemory_info_t) + sizeof(juint32_t) * 2 + 4)
#endif

void mempool_init(void)
{
    jint_t i;

#ifdef CONFIG_MEMPOOL_USAGE_SHOW
    for (i = 0; i < ALLOCATION_LISTS + 1; i++)
    {
        alloc_list_watch[i][0] = 0;
        alloc_list_watch[i][1] = 0;
    }
#endif
    
    /* Zero the lists */
    for (i = 0; i < ALLOCATION_LISTS + 1; i++)
        alloc_list[i] = NULL;

#ifdef JDEBUG
    /* In debug mode we have an overhead per malloc for the memory debug module,
     * so increase the alloc lists accordingly */
    for (i = 0; i < ALLOCATION_LISTS; i++)
        alloc_list_size[i] += DEBUG_ALLOC_OVERHEAD;
#endif
}

void mempool_uninit(void)
{
#ifdef CONFIG_MEMPOOL_USAGE_SHOW
    dump_mem_status();
#endif
}

void *jmemp_alloc(juint32_t size, juint16_t flags)
{
    jint_t i;
    void *item;

    /* Find smallest pool to alloc from */
    for (i = 0; i < ALLOCATION_LISTS && size > ALLOC_LIST_GET(i); i++)
        ;

    /* Get an item off the list */
    if (i < ALLOCATION_LISTS)
    {
#ifdef CONFIG_DATA_PATH_DEBUG
        DBG_X(DJOS_MEM, ("JOS_MEM: Allocating item from const list %d\n", i));
#endif /*  CONFIG_DATA_PATH_DEBUG */

        item = memp_item_get(&(alloc_list[i]), ALLOC_LIST_GET(i));
    }
    /* Use the un-even sized pool */
    else
    {
#ifdef CONFIG_DATA_PATH_DEBUG
        DBG_X(DJOS_MEM, ("JOS_MEM: Allocating from variable sized list\n"));
#endif /*  CONFIG_DATA_PATH_DEBUG */
        item = memp_item_get_ex(&alloc_list[ALLOCATION_LISTS], size);
    }

#ifdef CONFIG_DATA_PATH_DEBUG
    DBG_I(DJOS_MEM, ("JOS_MEM: Allocated %p size %ld pool %d\n", 
        item, size, i));
#endif /*  CONFIG_DATA_PATH_DEBUG */

#ifdef CONFIG_MEMPOOL_USAGE_SHOW
    if (item)
    {
        alloc_list_watch[i][0]++;
        alloc_list_watch[i][1] = MAX(alloc_list_watch[i][0],
            alloc_list_watch[i][1]);

        ITEM_SIZE_USED(item) = size;
        total_peak_curr += size;
        total_peak_max = MAX(total_peak_max, total_peak_curr);
    }
#endif
    if (item && (flags & M_ZERO))
        j_memset(item, 0, size);
    
    return item;
}

void jmemp_free(void *item)
{
    juint32_t size;
    jint_t i;

    HSU_ASSERT(item != NULL);

    size = ITEM_SIZE(item);
    
    /* Find the pool we alloced from */
    for (i = 0; i < ALLOCATION_LISTS && size > ALLOC_LIST_GET(i); i++)
        ;

#ifdef CONFIG_DATA_PATH_DEBUG
    DBG_I(DJOS_MEM, ("JOS_MEM: Freeing item %p size %ld pool %d\n",
        item, size, i));
#endif /* CONFIG_DATA_PATH_DEBUG */

#ifdef CONFIG_MEMPOOL_USAGE_SHOW
    alloc_list_watch[i][0]--;
    total_peak_curr -= ITEM_SIZE_USED(item);
#endif

    ITEM_NEXT(item) = alloc_list[i];
    alloc_list[i] = item;
}

static void *memp_item_alloc(juint32_t size)
{
    juint32_t *item;
    juint32_t alloc_size = size + sizeof(juint32_t);
    void *ptr;

#ifdef CONFIG_MEMPOOL_USAGE_SHOW
    alloc_size += sizeof(juint32_t);
#endif 

    DBG_X(DJOS_MEM, ("JOS_MEM:  Allocating a new item size %ld\n" , size));
#ifdef CONFIG_MEMPOOL
    if (pool_alloc(alloc_size, &ptr, NULL, POOL_VIRTUAL))
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DJOS_MEM, not_enough_memory_on_pool"); 
        return NULL;
    }
#else
    ptr = jos_malloc(alloc_size, 0);
    if (!ptr)
        return NULL;
#endif
    
    item = (juint32_t *)ptr;

#ifdef CONFIG_MEMPOOL_USAGE_SHOW
    /* Save space to put the "used size" of the item */
    item = &(item[1]);
    total_allocated += alloc_size;
#endif

    /* Save size of the item at the start */
    item[0] = size;

    DBG_X(DJOS_MEM, ("JOS_MEM:  Allocated %p (orig %p) - %ld\n",
        &item[1], item, item[0]));

    /* Advance actual pointer to follow the size */
    return ((void *) &(item[1]));
}

static void *memp_item_get(void **head, juint32_t size)
{
    void *item;
   
    /* If the list is empty alloc new item */
    if (!*head)
    {
        item = memp_item_alloc(size);
        if (!item)
            return NULL;
    }
    /* Get one off the list */
    else
    {
#ifdef CONFIG_DATA_PATH_DEBUG
        DBG_X(DJOS_MEM, ("JOS_MEM: Getting item of list %p\n", *head));
#endif /* CONFIG_DATA_PATH_DEBUG */
        item = *head;
        *head = ITEM_NEXT(item);
    }

    return item;
}

static void *memp_item_get_ex(void **head, juint32_t size)
{
    void *item = NULL;
    void **curr = head;
    
#ifdef CONFIG_DATA_PATH_DEBUG
    DBG_I(DJOS_MEM, ("JOS_MEM: Item_get_ex %p size %ld\n", *head, size));
#endif /* CONFIG_DATA_PATH_DEBUG */

    while (*curr)
    {
#ifdef CONFIG_DATA_PATH_DEBUG
        DBG_V(DJOS_MEM, ("JOS_MEM: Checking %p size %ld\n",
            *curr, ITEM_SIZE(*curr)));
#endif /* CONFIG_DATA_PATH_DEBUG */

        if (ITEM_SIZE(*curr) >= size)
        {
            item = *curr;
            *curr = ITEM_NEXT(item);
            break;
        }

        /* Get the next item from the start of the previous mem block */
        curr = &(ITEM_NEXT(*curr));
    }

    if (!item)
    {
#ifdef CONFIG_DATA_PATH_DEBUG
        DBG_V(DJOS_MEM, ("JOS_MEM: Nothing fitting found, making new one\n"));
#endif /* CONFIG_DATA_PATH_DEBUG */
        item = memp_item_alloc(size);
        if (!item)
            return NULL;
    }
    else
    {
#ifdef CONFIG_DATA_PATH_DEBUG
        DBG_I(DJOS_MEM, ("JOS_MEM: Reusing big block %p sized %ld\n", item, 
            ITEM_SIZE(item)));
#endif /* CONFIG_DATA_PATH_DEBUG */
    }

    return item;
}

#ifdef CONFIG_POOL_DMA
static jresult_t pool_dma_alloc(juint32_t size, void **vaddr, void **paddr,
    juint16_t flags, jbool_t *cachable)
{
#ifdef CONFIG_POOL_DMA_CACHABLE
    jresult_t rc;
#endif

    DBG_V(DJOS_MEM, ("JOS_MEM: Allocating a new dma buffer size %ld\n" , size));

#ifdef CONFIG_POOL_DMA_CACHABLE
    /* For cachable buffers check if there is room in the cachable pool */
    if (flags & M_CACHABLE)
    {
        rc = pool_alloc(size, vaddr, paddr, POOL_DMA_CACHABLE);
        if (!rc)
        {
            (*cachable) = TRUE;
            return 0;
        }
    }
#endif

    *cachable = FALSE;
    // +HSU ADDITION
    // Pool has to be virtual and not dma_able since direct phy-virt mapping no longer applies
    // Turn on CONFIG_POOL_DMA feature flag but force allocation from virtual pool
    return pool_alloc(size, vaddr, paddr, POOL_VIRTUAL);
    // return pool_alloc(size, vaddr, paddr, POOL_DMA);
    // -HSU ADDITION
}
#endif

#ifdef JDEBUG
void *jmalloc_d(juint32_t size, juint16_t flags, char *file, jint32_t line)
{
    juint32_t *p = NULL;
    jmemory_info_t *m;
    juint32_t i;

#ifdef CONFIG_MEMPOOL
    m = jmemp_alloc(size + sizeof(jmemory_info_t) + sizeof(juint32_t) * 2 + 4, 
        flags | M_ZERO);
#else
    m = jos_malloc(size + sizeof(jmemory_info_t) + sizeof(juint32_t) * 2 + 4, 
        flags | M_ZERO);
#endif
    if (!m)
        return NULL;
    m->magic_start      = MEM_LOG_MAGIC_START;
    m->magic_end        = ~MEM_LOG_MAGIC_START;
    m->file             = file;
    m->line             = line;
    m->size             = size;

    // +++ HSU_ADDITION Account for sizes introduced by all debug variables
    // Add the overhead seen in the above call into jmemp_alloc/jos_malloc
    m->size_w_overhead = size + sizeof(jmemory_info_t) + sizeof(juint32_t) * 2 + 4;
    for (i = 0; i < ALLOCATION_LISTS && m->size_w_overhead > ALLOC_LIST_GET(i); i++)
        ;

    // memp_item_alloc adds overhead
    m->size_w_overhead  = ((i < ALLOCATION_LISTS)?(ALLOC_LIST_GET(i)):(m->size_w_overhead)) + sizeof(juint32_t);
    #ifdef CONFIG_MEMPOOL_USAGE_SHOW
    // if feature flag defined, memp_item_alloc adds more debug info
    m->size_w_overhead += sizeof(juint32_t);
    #endif

    // Further adjustments within pool_alloc function
    if (m->size_w_overhead % CONFIG_MEMPOOL_ALIGN)
    {
        m->size_w_overhead = (m->size_w_overhead + CONFIG_MEMPOOL_ALIGN) - 
            (m->size_w_overhead % CONFIG_MEMPOOL_ALIGN);
    }
    // --- HSU_ADDITION

    m->org              = m;
    m->id               = mem_log_id ++;
    m->freed            = 0;
    m->prev             = mem_log_head;
    m->next             = mem_log_head->next;
    mem_log_head->next  = m;
    if (m->next)
        m->next->prev = m;
    
    p = (juint32_t *)(m + 1);
    m->user_ptr = (void *)p;

    p[(size / 4) + 1] = MEM_LOG_MAGIC_END;
    p[(size / 4) + 2] = ~MEM_LOG_MAGIC_END;

    return (void *)p;
}

void jfree_d(void *addr, char *file, jint32_t line)
{
    jmemory_info_t *m = addr;
    juint32_t *p = (juint32_t *)addr;
    m--;

    if (NULL == addr)
    {
      HSU_ERR_FATAL("NULL pointer passed to jos_free", addr, file, line);
    }

    if (!((m->magic_start == MEM_LOG_MAGIC_START) &&
        (m->magic_end == ~MEM_LOG_MAGIC_START)))
    {
      HSU_ERR_FATAL("Freeing someone else's memory", addr, file, line);
    }

    if (!(
        (p[(m->size / 4) + 1] == MEM_LOG_MAGIC_END) &&
        (p[(m->size / 4) + 2] == ~MEM_LOG_MAGIC_END)))
    {
      HSU_ERR_FATAL("Memory corrupted/overflown", addr, file, line);
    }
        
    if (m->freed)
    {
      HSU_ERR_FATAL("Memory was already freed", addr, file, line);
    }

    m->freed ++;
    m->prev->next = m->next;
    if (m->next)
        m->next->prev = m->prev;

#ifdef CONFIG_MEMPOOL
    jmemp_free(m->org);
#else
    jos_free(m->org);
#endif
}
#endif // JDEBUG

#ifdef CONFIG_MEMPOOL_USAGE_SHOW
static void dump_mem_status(void)
{
    jint_t i;

    jprintf("\n");

    jprintf("Memory utilization:\n");

    jprintf("\n");

    jprintf("RAM buffer usage: %ld\n", total_allocated);
    
#ifdef JDEBUG
    HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "DJOS_MEM,"
        " debug_overhead_%ld_byte_per_malloc", DEBUG_ALLOC_OVERHEAD); 

#endif

    jprintf("\n");
    
    jprintf("Actual Max use:\n");

    jprintf("RAM: %ld DMA: %ld Total: %ld\n",
        total_peak_max, total_dma_peak_max,
        total_peak_max + total_dma_peak_max);

    jprintf("\n");

    jprintf("Dumping RAM allocation statistics:\n");

    for (i = 0; i < ALLOCATION_LISTS + 1; i++)
    {
        jprintf("  %d: Size %4d in use %3d max used %3d\n",
            i, (i < ALLOCATION_LISTS ? ALLOC_LIST_GET(i) : 0),
            alloc_list_watch[i][0], alloc_list_watch[i][1]);
    }

    jprintf("\n");

    jprintf("Total dma entries %d, max on free list: %d \n",
        total_dma_items, total_dma_free_items_max);
    
    jprintf("\n");
}
#endif

#endif

#ifdef JDEBUG
# define CORRUPT_CHECK_MAGIC    0xAABB00CC
#endif

static LIST_HEAD(dma_frag_s_s, dma_block_s) dma_freelist =
    LIST_HEAD_INITIALIZER(dma_freelist);

//=============================================================================
// HSU ADDITION BEGIN: Trace JDMA heap usage
//=============================================================================
#ifdef FEATURE_HS_USB_TRACE
/* Link List to hold DMA items held by the client of the jdma heap */
static LIST_HEAD(dma_used_s_s, dma_trace_s) dma_usedlist =
    LIST_HEAD_INITIALIZER(dma_usedlist);

/* Adds the to the linked list that keep track of dma buffers held by client */
static void jdma_trace_add(jdma_t *dma)
{
  jdma_trace_t *dma_trc;   

  dma_trc = jmalloc(sizeof(struct dma_trace_s), M_ZERO);
  HSU_ASSERT(dma_trc != NULL);

  dma_trc->dma = dma;

  hsu_trace_copy(&(dma_trc->trace_info));
    
  LIST_INSERT_HEAD(&dma_usedlist, dma_trc, next);
}

/* Free dma from linked list that keep track of dma buffers held by client */
static void jdma_trace_free(jdma_t *dma)
{
  jdma_trace_t *dma_trc;
  
  for (dma_trc = LIST_FIRST(&dma_usedlist); dma_trc; dma_trc = LIST_NEXT(dma_trc, next)) 
  {
    if (dma_trc->dma == dma)
    {
      LIST_REMOVE(dma_trc, next);
  
      jfree(dma_trc);
      break;
    }
  }
  
  if (dma_trc == NULL)
  {
    HSU_ERR_FATAL("failed to find dma_trc",0, 0, 0);
  }
}

/* Free all dma from linked list used for tracing */
static void jdma_trace_free_all(void)
{
  jdma_trace_t *dma_trc;
  
  for (dma_trc = LIST_FIRST(&dma_usedlist); dma_trc; dma_trc = LIST_NEXT(dma_trc, next)) 
  {
    LIST_REMOVE(dma_trc, next);
    jfree(dma_trc);
    break;
  }      
}
#endif /* FEATURE_HS_USB_TRACE */
//=============================================================================
// HSU ADDITION END: Trace JDMA heap usage
//=============================================================================

void jdma_dummy_set(juint32_t size, void *vaddr, void *paddr, jdma_handle p)
{
    jdma_t *dma = (jdma_t *)p;

    if (!dma)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DJOS_MEM, setting_dummy_for_invalid_buffer");         
        return;
    }

    if (!(dma->flags & BLOCK_DUMMY))
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DJOS_MEM, trying_to_set_non_dummy_block_with_value");
        return;
    }
    
    dma->paddr = paddr;
    dma->vaddr = vaddr;
    dma->size = size;
}

void *jdma_phys_addr(jdma_handle p, juint32_t offset)
{
    return (void *)(((char *)((jdma_t *)p)->paddr) + offset);
}

void *jdma_virt_addr(jdma_handle p, juint32_t offset)
{
    return (void *)(((char *)((jdma_t *)p)->vaddr) + offset);
}

static void jdma_block_free(jdma_t *dma)
{
#if defined(JDEBUG) || defined(CONFIG_MEMPOOL_USAGE_SHOW)
    total_dma_free_items_curr++;
    total_dma_free_items_max = 
        MAX(total_dma_free_items_max, total_dma_free_items_curr);
#endif

#ifdef JDEBUG
    ((juint32_t *)jdma_virt_addr(dma, 0))[0] = CORRUPT_CHECK_MAGIC;
#endif
    hsu_mem_log(MEM_JDMA_BLOCK_FREE_LOG, dma, dma->size); /* log the next TD that is about to be added */
    
    LIST_INSERT_HEAD(&dma_freelist, dma, next);
}

static jresult_t dma_block_add(juint32_t size, jbool_t cachable, 
    juint8_t *vaddr, juint8_t *paddr, void *handle)
{
    juint32_t addr = (juint32_t)paddr;
    jdma_t *dma;
    jint_t i;

    KASSERT(!((juint32_t)vaddr % CONFIG_MEMPOOL_ALIGN),
        ("MEM: New buffer is incorrectly aligned\n"));

#if defined(JDEBUG) || defined(CONFIG_MEMPOOL_USAGE_SHOW)
    total_dma_items ++;
#endif

    dma = jmalloc(sizeof(struct dma_block_s), M_ZERO);
    if (!dma)
        return JENOMEM;

    DBG_V(DJOS_MEM, ("MEM: dma_block_add(size %d [%p:%p] as %p)\n",
        size, vaddr, paddr, dma));

    dma->vaddr = (void *)vaddr;
    dma->paddr = (void *)paddr;
    dma->size = size;

    if (cachable)
        dma->flags |= BLOCK_CACHABLE;
    
    /* Calculate buffer alignment */
    for (i = 1; !(addr & i) && (i <= 4096); i *= 2)
            ;
    dma->align = i;
    
    /* Check if does not cross page boundry */
    if (((addr / PAGE_SIZE) == (addr + size) / PAGE_SIZE))
        dma->flags |= BLOCK_PAGE_ALIGN;

    /* Save handle if this struct owns the real buffer */
    if (handle)
    {
        dma->os_handle = handle;
        dma->flags |= BLOCK_OWNER;
    }

    /* Put at head of list */
    jdma_block_free(dma);

    hsu_mem_log(MEM_DMA__SIZE_LOG, dma, size);
    hsu_mem_log(MEM_DMA__POOL_OFFSET_LOG, dma, (uint32)pool_offset[0]);

#if defined(JDEBUG) || defined(CONFIG_MEMPOOL_USAGE_SHOW)
    hsu_mem_log(MEM_DMA__TOTAL_DMA_FREE_LOG, dma, (uint32)total_dma_free_items_curr);
    hsu_mem_log(MEM_DMA__TOTAL_DMA_ITEMS_LOG, dma, (uint32)total_dma_items);
#endif

    return 0;
}

static jresult_t dma_block_alloc_os(juint32_t size, void **vaddr, void **paddr,
    juint16_t flags, jbool_t *cachable, void **handle)
{
    jresult_t rc = 0;

#ifdef CONFIG_MEMPOOL_DMABLE
    *vaddr = jmalloc(size, 0);
    if (!*vaddr)
        return JENOMEM;
    
    *handle = *paddr = *vaddr;
    *cachable = FALSE;

#elif defined(CONFIG_POOL_DMA)
     /* Alloc memory block */
    rc = pool_dma_alloc(size, vaddr, paddr, flags, cachable);
    if (rc)
        return rc;

    *handle = NULL;
#else
    /* TODO: Pass the M_CACHABLE to the OS */
    rc = os_dma_alloc(size, vaddr, paddr, flags, handle);
    if (rc)
        return rc;

    *cachable = FALSE;
#endif

    return rc;
}

/**
 * Function name:  dma_block_recover
 * Description:    Try to recover memory region around a new block
 * Parameters:
 *     @size:      Size of available space for recovery
 *     @cachable:  Cachability of memory region
 *     @vaddr:     Virtual address of start of free region
 *     @paddr:     Physical address of start of free region
 *
 * Return value:   None
 * Scope:          Local
 **/
static void dma_block_recover(juint32_t size, jbool_t cachable, 
    juint8_t *vaddr, juint8_t *paddr)
{
    juint32_t phys_addr = (juint32_t)paddr;
    juint32_t offset = 0;

    /* Is the size enough to fit in the minimal block ? */
    if (size < DMA_MEM_BLOCK)
        return;

    /* Do we need to re-align for platform requirements ? */
    if (phys_addr % CONFIG_MEMPOOL_ALIGN)
        offset = CONFIG_MEMPOOL_ALIGN - (phys_addr % CONFIG_MEMPOOL_ALIGN);

    /* Did the re-alignment larger then the size we have ? */
    if (size <= offset)
        return;

    /* Do we still fit the minimal block (after re-align) */
    if ((size - offset) < DMA_MEM_BLOCK)
        return;

    /* Add new block */
    dma_block_add(size - offset, cachable, 
        (void *)((juint32_t)vaddr + offset), 
        (void *)((juint32_t)paddr + offset), NULL);
}

static jresult_t dma_block_alloc(juint32_t size, juint16_t align, juint16_t flags)
{
    jresult_t rc;
    jbool_t cachable = 0;
    juint32_t needed_offset = 0;
    juint32_t alloc_size = size;
    juint8_t *vaddr, *paddr;
    void *vtemp, *ptemp = NULL;
    void *handle = NULL;
    juint32_t adj_size_to_alloc_list = 0;
    juint32_t i = 0;

    KASSERT(alloc_size, ("MEM: Cannot use dma_block_alloc for zero "
        "sized blocks\n"));

    /* Make sure the user doesn't want something crazy */
    if ((flags & M_PAGE_ALIGN) && (size > PAGE_SIZE))
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DJOS_MEM,"
          " requested_buffer_not_to_fall_on_page_boundry, buffer_larger_than_a_page");        
        return JEINVAL;
    }
    
    /* Make sure we allocate aleast DMA_MEM_BLOCK size */
    alloc_size = MAX(alloc_size, DMA_MEM_BLOCK);

    /* Extra space for alignment */
    alloc_size += align - 1;

    /* Extra space for page boundry restriction */
    if (flags & M_PAGE_ALIGN)
        alloc_size += alloc_size;

    DBG_V(DJOS_MEM, ("MEM: dma_block_alloc(size %d real %d)\n",
        alloc_size, size));

    adj_size_to_alloc_list = alloc_size;
    /* Find smallest pool to alloc from */
    for (i = 0; i < ALLOCATION_LISTS && adj_size_to_alloc_list > ALLOC_LIST_GET(i); i++)
        ;
    if (i < ALLOCATION_LISTS)
    {
      // allocate larger to avoid fragmentation
      adj_size_to_alloc_list = ALLOC_LIST_GET(i);
    }
    // else use the size as is

    /* Alloc memory block */
    rc = dma_block_alloc_os(adj_size_to_alloc_list, &vtemp, &ptemp, 
        flags, &cachable, &handle);
    if (rc)
        return rc;

    vaddr = (juint8_t *)vtemp;
    paddr = (juint8_t *)ptemp;

    /* See if page bounds are crossed */
    if (flags & M_PAGE_ALIGN)
    {
        // use original client requested size to determine alignment
        if ((((juint32_t)paddr % PAGE_SIZE) + (size + align - 1)) > PAGE_SIZE)
            needed_offset = PAGE_SIZE - ((juint32_t)paddr % PAGE_SIZE);
    }

    /* See if we need to realign the resulting buffer */
    if (((juint32_t)paddr + needed_offset) % align)
    {
        needed_offset = needed_offset + align - 
            (((juint32_t)paddr + needed_offset) % align);
    }

    if (flags & M_PAGE_ALIGN)
    {
      /* Recover the wasted space around the allocated buffer */
      dma_block_recover(needed_offset, cachable, vaddr, paddr);
      dma_block_recover(adj_size_to_alloc_list - size - needed_offset, cachable, 
          vaddr + needed_offset + size, 
          paddr + needed_offset + size);

      /* Add the required block */
      return dma_block_add(size, cachable, vaddr + needed_offset, 
          paddr + needed_offset, handle);
    }
    else
    {
      // Only recover the wasted space preceding the allocated buffer
      // Removed the code that reclaims the extra space after the client buffer as it causes fragementation
      dma_block_recover(needed_offset, cachable, vaddr, paddr);

      /* Add the required block */
      return dma_block_add(adj_size_to_alloc_list - needed_offset, cachable, vaddr + needed_offset, 
          paddr + needed_offset, handle);
    }
}

jdma_t *find_existing(juint32_t size, juint16_t align, juint16_t flags)
{
    jdma_t *dma, *found = NULL;
    juint32_t adj_size_for_alignment = 0;

    /* First check the free list. */
    for (dma = LIST_FIRST(&dma_freelist); 
        dma && size; 
        dma = LIST_NEXT(dma, next)) 
    {
#ifdef JDEBUG
        if (((juint32_t *)jdma_virt_addr(dma, 0))[0] != CORRUPT_CHECK_MAGIC)
        {
            HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "DJOS_MEM,"
                " DMA_block 0x%08X or freelist_corrupted", (uint32) dma);                
            return NULL;
        }
#endif
        /* Make sure its has the same cachablitity */
        if (((flags & M_CACHABLE) && !(dma->flags & BLOCK_CACHABLE)) || 
            (!(flags & M_CACHABLE) && (dma->flags & BLOCK_CACHABLE))) 
        {
            continue;
        }

        /* Make sure buffer is page aligned, if required */
        if ((flags & M_PAGE_ALIGN) && !(dma->flags & BLOCK_PAGE_ALIGN))
            continue;

        /* Make sure the alignment requirment is met */
        if (dma->align < align)
            continue;

        /* We need a block about the same size */
        if (dma->size < size)
            continue;

        /* HSU fix: Ignore maximum size limitation if requested */
        // To prevent under-utilization, the increments must be at most multiples of 2 of the previous entry.
        // If found DMA item is already less than or equal to the smallest entry in the alloc_list_size, 
        //   then under-utilization does not need to be checked.
        adj_size_for_alignment = (align * 2) + size;

        if ( (dma->size > alloc_list_size[0]) 
          && (!(flags & M_IGNORE_MAX_SIZE_LIMITATION) && dma->size >= (adj_size_for_alignment * 2)) )
            continue;
        /* End HSU fix */

        LIST_REMOVE(dma, next);

#if defined(JDEBUG) || defined(CONFIG_MEMPOOL_USAGE_SHOW)
        total_dma_free_items_curr --;
#endif

        found = dma;
#ifdef CONFIG_DATA_PATH_DEBUG
        DBG_X(DJOS_MEM,("MEM: Found block %p size %ld\n", dma, dma->size));
#endif /* CONFIG_DATA_PATH_DEBUG */

        break;
    }

    return found;
}

jresult_t jdma_alloc(juint32_t size, juint16_t align, void **vaddr, void **paddr, 
    juint16_t flags, jdma_handle *handle)
{
    jresult_t err;
    jdma_t *dma = NULL; 
#ifdef JDEBUG
    juint32_t calc;
#endif

#ifdef CONFIG_DATA_PATH_DEBUG
    DBG_I(DJOS_MEM, ("MEM: j_dma_alloc(%d align %d flags %x)\n",
        size, align, flags));
#endif /* CONFIG_DATA_PATH_DEBUG */
    /* Handle empty dummy blocks */
    if (size == 0)
    {
        dma = jmalloc(sizeof(struct dma_block_s), M_ZERO);
        if (!dma)
            return JENOMEM;

#ifdef CONFIG_DATA_PATH_DEBUG
        DBG_X(DJOS_MEM, ("MEM: Alloc dummy %p\n", dma));
#endif /* CONFIG_DATA_PATH_DEBUG */

        dma->flags |= BLOCK_DUMMY;

        *handle = (jdma_handle)dma;

#if defined(JDEBUG) || defined(CONFIG_MEMPOOL_USAGE_SHOW)
        blocks_used_curr ++;
#endif

#ifdef FEATURE_HS_USB_TRACE
        jdma_trace_add(dma);
#endif /* FEATURE_HS_USB_TRACE */

        return 0;
    }

    /* Make sure we allocate aleast DMA_MEM_BLOCK size */
    size = MAX(size, DMA_MEM_BLOCK);  

    /* Make alignment reasonable */
    align = MAX(align, CONFIG_MEMPOOL_ALIGN);

    /* Try to find an existing buffer on the free list */
    dma = find_existing(size, align, flags);
      
    /* If we didn't find a cachable buffer, get a normal one */
    if ((!dma) && (flags & M_CACHABLE))
    {
        dma = find_existing(size, align, (flags & ~M_CACHABLE));

        hsu_mem_log(MEM_JDMA__FIND_EXISTING_UNCACHABLE_LOG, dma, size);
    }

    /* If we didn't find a suitable buffer, allocate a new one */
    if (!dma)
    {
        /* Alloc a new block if nothing matching was found */
        err = dma_block_alloc(size, align, flags);
        /* HSU Fix: Search DMA freelist without maximum size limitation */
        if (err)
        {
            DBG_W(DJOS_MEM, ("JOS_MEM: Doing extended DMA freelist search\n"));
            dma = find_existing(size, align, (flags | M_IGNORE_MAX_SIZE_LIMITATION));
            
            hsu_mem_log(MEM_JDMA__FIND_EXISTING_IGNORE_MAX_SIZE_LOG, dma, size);
            
            /* If we didn't find a cachable buffer, get a normal one */
            if ((!dma) && (flags & M_CACHABLE))
                dma = find_existing(size, align, ((flags & ~M_CACHABLE) | M_IGNORE_MAX_SIZE_LIMITATION));
            
            if (!dma)
            {
                HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DJOS_MEM,"
                  " cannot_find_suitable_buffer_in_freelist");
                return (err);
            }
        }
        else /* If a new buffer was successfully allocated on the pool */
        /* Get the first block 
         * (dma_alloc_block will put the needed block at head) */
        {
        dma = LIST_FIRST(&dma_freelist);
        LIST_REMOVE(dma, next);

#if defined(JDEBUG) || defined(CONFIG_MEMPOOL_USAGE_SHOW)
            total_dma_free_items_curr --;
#endif
        }
        /* End HSU fix */

#ifdef JDEBUG
        if (((juint32_t *)jdma_virt_addr(dma, 0))[0] != CORRUPT_CHECK_MAGIC)
        {
            HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DJOS_MEM, new_item_on_freelist_is_corrupted");            
            return EUNKNOWN;
        }
#endif
    }
    
    if (handle)
        *handle = (jdma_handle)dma;

    if (vaddr)
        *vaddr = jdma_virt_addr(dma, 0);

    if (paddr)
        *paddr = jdma_phys_addr(dma, 0);

    if (flags & M_ZERO)
        j_memset(jdma_virt_addr(dma, 0), 0, size);

#ifdef CONFIG_DATA_PATH_DEBUG
    DBG_I(DJOS_MEM, ("MEM: jdma_alloc_done(%p:%p)\n",
        jdma_virt_addr(dma, 0),
        jdma_phys_addr(dma, 0)));
#endif /* CONFIG_DATA_PATH_DEBUG */

#if defined(JDEBUG) || defined(CONFIG_MEMPOOL_USAGE_SHOW)
    total_dma_peak_curr += dma->size;
    blocks_used_curr ++;
#endif

#ifdef CONFIG_MEMPOOL_USAGE_SHOW
    total_dma_peak_max = MAX(total_dma_peak_max, total_dma_peak_curr);
    blocks_used_max = MAX(blocks_used_max, blocks_used_curr);
#endif

#ifdef JDEBUG
    /* Check the alignment is as requested */
    calc = (juint32_t)jdma_phys_addr(dma, 0);
    if (calc % align)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DJOS_MEM, allocated_buffer_not_fit_requirement");        
        return JEINVAL;
    }

    if (flags & M_PAGE_ALIGN)
    {
        if ((calc / PAGE_SIZE) != ((calc + size) / PAGE_SIZE))
        {
            HSU_ULOG_2(ERROR_MSG, DBG_E_LOG, "DJOS_MEM,"
                " allocated_buffer_not_fit_requirement, buff 0x%08X, size &u",
                calc, size);              
            return JEINVAL;
        }
    }
#endif

#ifdef FEATURE_HS_USB_TRACE
    jdma_trace_add(dma);
#endif /* FEATURE_HS_USB_TRACE */

    return 0;
}

void jdma_free(jdma_handle p)
{
    jdma_t *dma = (jdma_t *)p;
    
    HSU_ASSERT(dma != NULL);

#ifdef FEATURE_HS_USB_TRACE
    jdma_trace_free(dma);
#endif /* FEATURE_HS_USB_TRACE */

#ifdef CONFIG_DATA_PATH_DEBUG
    DBG_V(DJOS_MEM, ("MEM: jdma_free(%p - [%p:%p] size %d)\n",
        dma, dma->vaddr, dma->paddr, dma->size));
#endif /* CONFIG_DATA_PATH_DEBUG */

    if (dma->flags & BLOCK_DUMMY)
    {
        if (dma->size)
        {
            HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DJOS_MEM, dummy_not_reset_before_freed"); 
        }       

#ifdef CONFIG_DATA_PATH_DEBUG
        DBG_X(DJOS_MEM, ("MEM: Freeing dummy !\n"));
#endif /* CONFIG_DATA_PATH_DEBUG */
        jfree(dma);

#if defined(JDEBUG) || defined(CONFIG_MEMPOOL_USAGE_SHOW)
        blocks_used_curr --;
#endif
        return;
    }

    jdma_block_free(dma);

#if defined(JDEBUG) || defined(CONFIG_MEMPOOL_USAGE_SHOW)
    total_dma_peak_curr -= dma->size;
    blocks_used_curr --;
#endif
}

void jdma_free_all(void)
{
    jdma_t *dma;

    /* HSU Addition: KW fix to catch infinite loop */
    #define HSU_DMA_FREE_LIST_TRAVERSE_MAX_LOOP_COUNT (1000000)
    juint32_t max_loop_cnt;
    max_loop_cnt = HSU_DMA_FREE_LIST_TRAVERSE_MAX_LOOP_COUNT;

    /* Free the blocks */
    for (dma = LIST_FIRST(&dma_freelist);
        dma;
        dma = LIST_FIRST(&dma_freelist))
    {
        LIST_REMOVE(dma, next);

        if (!(dma->flags & BLOCK_OWNER))
        {
            jfree(dma);
            continue;
        }

        DBG_I(DJOS_MEM, ("MEM: Freeing block %p handle %p alignment %d\n", 
            dma, dma->os_handle, dma->align));

#ifdef CONFIG_MEMPOOL_DMABLE
        jfree(dma->os_handle);
#elif defined(CONFIG_POOL_DMA)
        /* We give the whole pool back on un-init */
#else
        os_dma_free(dma->os_handle);
#endif

        jfree(dma);

        HSU_ASSERT(max_loop_cnt--);
    }

#ifdef FEATURE_HS_USB_TRACE
    jdma_trace_free_all();
#endif /* FEATURE_HS_USB_TRACE */
}

jresult_t jos_mem_init(uw_args_t *args)
{
    jresult_t rc = 0;
   
    KASSERT(sizeof(juint32_t) <= DMA_MEM_BLOCK,
        ("MEM: Minimal dma alloc buffer is too small (%d) should"
        "be atleast %d\n", DMA_MEM_BLOCK, sizeof(juint32_t)));

#ifdef CONFIG_MEMPOOL
    mempool_init();
#endif

#ifdef CONFIG_MEMPOOL
    pool_init(POOL_VIRTUAL, args->mem_vbuf, args->mem_pbuf, args->mem_buf_size);
#endif

#ifdef CONFIG_POOL_DMA
    pool_init(POOL_DMA, args->dma_vbuf, args->dma_pbuf, args->dma_buf_size);
    
# ifdef CONFIG_POOL_DMA_CACHABLE
    pool_init(POOL_DMA_CACHABLE, args->dma_cachable_vbuf, 
        args->dma_cachable_pbuf, args->dma_cachable_size);
# endif
#endif

    return rc;
}

void jos_mem_uninit()
{
#ifdef CONFIG_MEMPOOL
    mempool_uninit();
#endif
}


#ifdef FEATURE_HS_USB_MEM_TEST
#define MEM_TEST_NUM_OF_ITER 5
#define TDI_4X_TRANSFER_DESCRIPTOR_SIZE 28
#define TDI_4X_TRANSFER_DESCRIPTOR_ALIGN 0x40
// Allocate fixed size memory MEM_TEST_NUM_OF_ITER times from jdma_alloc
// Free memory that was allocated
// Allocate memory MEM_TEST_NUM_OF_ITER times and then user examines whether there is memory leak and/or fragmentation
void jos_mem__jdma_alloc_free_fixed_size_test(jmemory_test_t* mem_test_list, juint32_t size, juint16_t align, juint16_t flag)
{
  // JDMA_ALLOC/JDMA_FREE test
  jmemory_test_t *iter_ptr;
  jmemory_test_t *next_ptr;
  uint32 i;

  iter_ptr = mem_test_list;
  for (i = 0; i < MEM_TEST_NUM_OF_ITER; i++)
  {
    jdma_alloc(size, align,
      &(iter_ptr->vaddr), &(iter_ptr->paddr), flag, 
      &(iter_ptr->cookie));
    iter_ptr->size = size;
    iter_ptr->next = iter_ptr + 1;
    ++iter_ptr;
  }

  // Set breakpoint here to see memory allocations

  iter_ptr = mem_test_list;
  for (i = 0; i < MEM_TEST_NUM_OF_ITER; i++)
  {
    if (iter_ptr->cookie)
    {
      next_ptr = iter_ptr->next;
      jdma_free(iter_ptr->cookie);
      j_memset((void*)iter_ptr, 0, sizeof(jmemory_test_t));
      iter_ptr = next_ptr;
    }
  }

  // Allocate one more time to see how memory is utilized

  iter_ptr = mem_test_list;
  for (i = 0; i < MEM_TEST_NUM_OF_ITER; i++)
  {
    jdma_alloc(size, align,
      &(iter_ptr->vaddr), &(iter_ptr->paddr), flag, 
      &(iter_ptr->cookie));
    iter_ptr->size = size;
    iter_ptr->next = iter_ptr + 1;
    ++iter_ptr;
  }
}

// Allocate fixed size memory MEM_TEST_NUM_OF_ITER times from jmalloc
// Free memory that was allocated
// Allocate memory MEM_TEST_NUM_OF_ITER times and then user examines whether there is memory leak and/or fragmentation
void jos_mem__jmalloc_test(jmemory_test_t* mem_test_list, juint32_t size)
{
  jmemory_test_t *iter_ptr;
  jmemory_test_t *next_ptr;
  uint32 i;

  iter_ptr = mem_test_list;
  for (i = 0; i < MEM_TEST_NUM_OF_ITER; i++)
  {
    iter_ptr->cookie = jmalloc(size, M_ZERO);
    iter_ptr->size = size;
    iter_ptr->next = iter_ptr + 1;
    ++iter_ptr;
  }

  iter_ptr = mem_test_list;
  for (i = 0; i < MEM_TEST_NUM_OF_ITER; i++)
  {
    if (iter_ptr->cookie)
    {
      next_ptr = iter_ptr->next;
      jfree(iter_ptr->cookie);
      j_memset((void*)iter_ptr, 0, sizeof(jmemory_test_t));
      iter_ptr = next_ptr;
    }
  }

  iter_ptr = mem_test_list;
  for (i = 0; i < MEM_TEST_NUM_OF_ITER; i++)
  {
    iter_ptr->cookie = jmalloc(size, M_ZERO);
    iter_ptr->size = size;
    iter_ptr->next = iter_ptr + 1;
    ++iter_ptr;
  }
}


#define MEM_TEST_INCREASING_SIZE_NUM_OF_ITER 1000
// Allocate fixed size memory from jdma_alloc
// Free memory that was allocated
// Increment the fixed size by 1 and repeat the test MEM_TEST_INCREASING_SIZE_NUM_OF_ITER times
void jos_mem__jdma_alloc_increasing_size_test(jmemory_test_t* mem_test_list)
{
  jmemory_test_t *iter_ptr;
  jmemory_test_t *next_ptr;
  uint32 i;
  uint32 size = 31;

  iter_ptr = mem_test_list;
  for (i = 0; i < MEM_TEST_INCREASING_SIZE_NUM_OF_ITER; i++)
  {
    jdma_alloc(size, 0,
      &(iter_ptr->vaddr), &(iter_ptr->paddr), 0, 
      &(iter_ptr->cookie));
    iter_ptr->size = size;
    iter_ptr->next = iter_ptr; // point to itself so that we can independently use MEM_TEST_INCREASING_SIZE_NUM_OF_ITER
    ++size; // next test case

    iter_ptr = mem_test_list;
    if (iter_ptr->cookie)
    {
      next_ptr = iter_ptr->next;
      jdma_free(iter_ptr->cookie);
      j_memset((void*)iter_ptr, 0, sizeof(jmemory_test_t));
      iter_ptr = next_ptr;
    }
  }
}

// Main API for client to select test case
void jos_mem_test(void* param)
{
  static jmemory_test_t memory_test_list[MEM_TEST_NUM_OF_ITER];
  static uint32 hsu_mem_test_case = 3;
  HSU_USE_PARAM(param);
    
  if (1 == hsu_mem_test_case)
  {
    jos_mem__jdma_alloc_free_fixed_size_test(memory_test_list, 
        TDI_4X_TRANSFER_DESCRIPTOR_SIZE, 
        TDI_4X_TRANSFER_DESCRIPTOR_ALIGN,
        0);
  }
  else if (2 == hsu_mem_test_case)
  {
    jos_mem__jmalloc_test(memory_test_list, 253);
  }
  else if (3 == hsu_mem_test_case)
  {
    jos_mem__jdma_alloc_increasing_size_test(memory_test_list);
  }
}
#endif // FEATURE_HS_USB_MEM_TEST

