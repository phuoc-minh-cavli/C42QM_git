/*=============================================================================

                alloc.c -- H E A D E R  F I L E

GENERAL DESCRIPTION
		  Kernel allocation functions  
			
EXTERNAL FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

      Copyright (c) 2009
                    by Qualcomm Technologies Incorporated.  All Rights Reserved.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


$Header: //components/rel/core.tx/6.0/kernel/qurt/common/src/alloc.c#1 $ 
$DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/03/11   op      Add header file
=============================================================================*/
/*
 * Australian Public Licence B (OZPLB)
 *
 * Version 1-0
 *
 * Copyright (c) 2007, Open Kernel Labs, Inc.
 *
 * All rights reserved. 
 *
 * Developed by: Embedded, Real-time and Operating Systems Program (ERTOS)
 *               National ICT Australia
 *               http://www.ertos.nicta.com.au
 *
 * Permission is granted by National ICT Australia, free of charge, to
 * any person obtaining a copy of this software and any associated
 * documentation files (the "Software") to deal with the Software without
 * restriction, including (without limitation) the rights to use, copy,
 * modify, adapt, merge, publish, distribute, communicate to the public,
 * sublicense, and/or sell, lend or rent out copies of the Software, and
 * to permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimers.
 *
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimers in the documentation and/or other materials provided
 *       with the distribution.
 *
 *     * Neither the name of National ICT Australia, nor the names of its
 *       contributors, may be used to endorse or promote products derived
 *       from this Software without specific prior written permission.
 *
 * EXCEPT AS EXPRESSLY STATED IN THIS LICENCE AND TO THE FULL EXTENT
 * PERMITTED BY APPLICABLE LAW, THE SOFTWARE IS PROVIDED "AS-IS", AND
 * NATIONAL ICT AUSTRALIA AND ITS CONTRIBUTORS MAKE NO REPRESENTATIONS,
 * WARRANTIES OR CONDITIONS OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO ANY REPRESENTATIONS, WARRANTIES OR CONDITIONS
 * REGARDING THE CONTENTS OR ACCURACY OF THE SOFTWARE, OR OF TITLE,
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, NONINFRINGEMENT,
 * THE ABSENCE OF LATENT OR OTHER DEFECTS, OR THE PRESENCE OR ABSENCE OF
 * ERRORS, WHETHER OR NOT DISCOVERABLE.
 *
 * TO THE FULL EXTENT PERMITTED BY APPLICABLE LAW, IN NO EVENT SHALL
 * NATIONAL ICT AUSTRALIA OR ITS CONTRIBUTORS BE LIABLE ON ANY LEGAL
 * THEORY (INCLUDING, WITHOUT LIMITATION, IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHERWISE) FOR ANY CLAIM, LOSS, DAMAGES OR OTHER
 * LIABILITY, INCLUDING (WITHOUT LIMITATION) LOSS OF PRODUCTION OR
 * OPERATION TIME, LOSS, DAMAGE OR CORRUPTION OF DATA OR RECORDS; OR LOSS
 * OF ANTICIPATED SAVINGS, OPPORTUNITY, REVENUE, PROFIT OR GOODWILL, OR
 * OTHER ECONOMIC LOSS; OR ANY SPECIAL, INCIDENTAL, INDIRECT,
 * CONSEQUENTIAL, PUNITIVE OR EXEMPLARY DAMAGES, ARISING OUT OF OR IN
 * CONNECTION WITH THIS LICENCE, THE SOFTWARE OR THE USE OF OR OTHER
 * DEALINGS WITH THE SOFTWARE, EVEN IF NATIONAL ICT AUSTRALIA OR ITS
 * CONTRIBUTORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH CLAIM, LOSS,
 * DAMAGES OR OTHER LIABILITY.
 *
 * If applicable legislation implies representations, warranties, or
 * conditions, or imposes obligations or liability on National ICT
 * Australia or one of its contributors in respect of the Software that
 * cannot be wholly or partly excluded, restricted or modified, the
 * liability of National ICT Australia or the contributor is limited, to
 * the full extent permitted by the applicable legislation, at its
 * option, to:
 * a.  in the case of goods, any one or more of the following:
 * i.  the replacement of the goods or the supply of equivalent goods;
 * ii.  the repair of the goods;
 * iii. the payment of the cost of replacing the goods or of acquiring
 *  equivalent goods;
 * iv.  the payment of the cost of having the goods repaired; or
 * b.  in the case of services:
 * i.  the supplying of the services again; or
 * ii.  the payment of the cost of having the services supplied again.
 *
 * The construction, validity and performance of this licence is governed
 * by the laws in force in New South Wales, Australia.
 */

/*
 * Memory Pools
 * ============
 *
 * This file implements a low level memory allocator.  The functionality here
 * shouldn't be exposed directly to the user.  These functions should only
 * really be accessed by the PhysMem and VirtMem objects.
 *
 * This allocator implements a segregated fit algorithm.  In contrast to
 * traditional segregated fit allocators, memory is stored in the power of two
 * sized list corresponding the largest aligned block.
 *
 * These functions are not especially optimised, but should perform
 * reasonably.  We defer coalescing till an allocation fails.  Some papers say
 * that this improves overall performance, but we'd need benchmarks to verify
 * that.  In any case deferring coalescing makes things relatively simpler --
 * we would need more complicated data structures to coalesce on the fly.
 *
 * The most obvious optimisation I can see would be to speed up free list
 * searches.  At present free lists are ordered linked lists, and they are
 * scanned linearly to find a suitable block.  In general this will hopefully
 * not be an issue, since there is a free list per super page size.  These
 * free lists are optimised to deal with aligned allocations, so allocating a
 * power of two sized region can just pop the head of the appropriate list.
 *
 * The allocator implemented here is the descendent of the old binary buddy
 * Fpage allocator.  Using segregated fit has resulted in slightly more
 * complicated computation to find the best block, but has drastically reduced
 * the intermodule complexity.
 */

#include "mem_utils.h"

#define ALLOC_DEBUG(...) 
//#define ALLOC_DEBUG kprintf

//#define ALLOC_MERGE_DEBUG kprintf
#define ALLOC_MERGE_DEBUG(...)

//#define ALLOC_POOL_DUMP_DEBUG kprintf
#define ALLOC_POOL_DUMP_DEBUG(...)

//struct mem_pool shared_virt_pool;
//struct mem_pool default_direct_pool;
//struct mem_pool * tcm_phys_pool = NULL;
//
void mem_pool_dump(struct mem_pool *pool );
extern QURTK_obj_cache_t QURTK_memory_cache;

int clz_to_page_size[20] = {
    0x1000000, 0x1000000, 0x1000000, 0x1000000,
    0x1000000, 0x1000000, 0x1000000, 0x1000000,
    0x100000, 0x100000, 0x100000, 0x100000,
    0x10000, 0x10000, 0x10000, 0x10000,
    0x1000, 0x1000, 0x1000, 0x1000}; 
#define min(a, b) ( (a)<(b)? (a): (b) )
#define INVALID_ADDR 	0xffffffff
/*
 * FIXME: This should probably be moved to <compat/bitops.h> or similar.
 */


//#define ALIGN_UP(addr, size)    (((addr) + (size) - 1) & ~((size) - 1))
#define BEGIN(mem)              ((mem)->addr)
#define END(mem)                ((mem)->addr + (mem)->size - 1)
#define FREE_LIST(pool, idx)    (&(pool)->free_list[idx])
#define SIZE(mem)               ((mem)->size)
#define GLOBAL_LIST(pool)	(&(pool)->global_list)

struct mem_pool *internal_physpool;
struct mem_pool *internal_virtpool;

INLINE qurt_size_t idx_to_size(unsigned idx) {
    return 1UL << (idx*4 + MEM_MIN_BITS);
}

INLINE unsigned size_to_idx(qurt_size_t size) {
    return MEM_MAX_IDX - qurt_clz(size)/4;
}

INLINE int addr_aligned(struct memory *mem, qurt_size_t size) {
    qurt_addr_t addr;
    qurt_size_t rsize;

    addr = ALIGN_UP(mem->addr, clz_to_page_size[qurt_clz(size)]);
    rsize = END(mem) - addr + 1;

    if (rsize < size) {
        return 0;
    }
    return addr;

}

INLINE qurt_addr_t best_addr(qurt_size_t size, struct memory *mem) {
    qurt_addr_t addr;
    qurt_size_t rsize;
    unsigned idx;

    idx = size_to_idx(size);
    addr = ALIGN_UP(mem->addr, idx_to_size(idx));
    rsize = END(mem) - addr + 1;

    if (rsize < size) {
        addr -= size - rsize;
    }
    return addr;
}

INLINE unsigned best_idx(struct memory *mem) {
//    qurt_addr_t addr;
//    qurt_size_t size;
    unsigned idx;

    idx = size_to_idx(mem->size);
//    size = idx_to_size(idx);
//    addr = ALIGN_UP(mem->addr, size);

//    if (addr + size - 1 > END(mem)) {
//        idx--;
//    }
    return idx;
}

static void mem_unfree(struct mem_pool *pool, struct memory *ent) {
    unsigned idx;

    idx = best_idx(ent);
    TAILQ_REMOVE(FREE_LIST(pool, idx), ent, mem_list);
    TAILQ_REMOVE(GLOBAL_LIST(pool), ent, phys_list);
}


void print_glob_list(struct mem_pool *pool)
{
   struct memory *ent, *tmp;
   
   ALLOC_POOL_DUMP_DEBUG("GLOBAL_LIST_PRINT: pool %p\n", pool);
   TAILQ_FOREACH_SAFE(ent, GLOBAL_LIST(pool), phys_list, tmp){
     ALLOC_POOL_DUMP_DEBUG( "ent %p %p -- %p   tf %d \n", ent, ent->addr, ent->addr+ent->size, ent->tf_bit );
   }
   ALLOC_POOL_DUMP_DEBUG("END_GLOB_LIST_PRINT\n");
}


static struct memory *
trim_mem(struct mem_pool *pool, struct memory *mem, qurt_addr_t addr, qurt_size_t size) {
    qurt_size_t rest;
    struct memory *tmp;
//    struct memory *begin_block; 
//    struct memory *end_block ;
    struct memory *prev, *next;
    struct memory *init_block;

    init_block = mem;     
    if (mem->addr < addr) {
        rest = mem->size - (addr - mem->addr);
        mem->size = addr - mem->addr;
        //anchor pointer before freeing
//        begin_block = mem;
        QURTK_mem_free(pool, mem);
        tmp = mem;
        mem = (struct memory *)QURTK_obj_cache_alloc(&QURTK_memory_cache);
        if (mem == NULL) {
            mem_unfree(pool, tmp);
            tmp->size += rest;
            QURTK_mem_free(pool, tmp);
            return NULL;
        }
        mem->addr = addr;
        mem->size = rest;
           
//        ALLOC_MERGE_DEBUG("2, mem %x(%x, %x), begin_block %x(%x, %x)\n",
//                       mem, mem->addr, mem->size, begin_block, begin_block->addr, begin_block->size);  

         
        TAILQ_INSERT_AFTER(GLOBAL_LIST(pool), init_block, mem, phys_list);

        //ALLOC_MERGE_DEBUG("2, init_block->phys_list.next %x\n", TAILQ_NEXT(init_block, phys_list));
 
        mem->tf_bit = 1;
    }
    if (SIZE(mem) > size) {
        init_block = mem;
        mem->addr = addr + size;
        mem->size = mem->size - size;
        //anchor pointer before freeing 
//        end_block = mem;
        QURTK_mem_free(pool, mem);
        tmp = mem;
        mem = (struct memory *)QURTK_obj_cache_alloc(&QURTK_memory_cache);
        if (mem == NULL) {
            mem_unfree(pool, tmp);
            tmp->size += size;
            QURTK_mem_free(pool, tmp);
            return NULL;
        }
        mem->addr = addr;
        mem->size = size;
        //adjust pointers and tf bit, here all blocks are adjacent, place on global list:
//        ALLOC_MERGE_DEBUG("4, mem %x(%x, %x), end_block %x(%x, %x)\n", 
//            		  mem, mem->addr, mem->size, end_block, end_block->addr, end_block->size);
        TAILQ_INSERT_BEFORE(GLOBAL_LIST(pool), init_block, mem, phys_list);
//        if (TAILQ_NEXT(TAILQ_LAST(GLOBAL_LIST(pool)), phys_list) != NULL) {
//        } 
            
        mem->tf_bit = 1;
    }
    //make sure tf_bit is set:
    mem->tf_bit = 1;

    //check phys_list here   
    prev = TAILQ_PREV(mem, phys_list);
    next = TAILQ_NEXT(mem, phys_list);

    ALLOC_MERGE_DEBUG("cur mem info....tf_bit: %d, addr: %x, size: %x, prev: %x, next: %x, self: %x\n",
    		mem->tf_bit, mem->addr, mem->size, TAILQ_PREV(mem, phys_list),
                TAILQ_NEXT(mem, phys_list), mem);
    if (((int)prev != INVALID_ADDR) && (prev != NULL))
        ALLOC_MERGE_DEBUG("prev_info: tf_bit: %d, addr: %x, size: %x, prev: %x, next: %x\n",
                prev->tf_bit, prev->addr, prev->size, TAILQ_PREV(prev, phys_list),
                TAILQ_NEXT(prev, phys_list));
    if (((int)next != INVALID_ADDR) && (next != NULL))
        ALLOC_MERGE_DEBUG("next_info: tf_bit: %d, addr: %x, size: %x, prev: %x\n",
                next->tf_bit, next->addr, next->size, TAILQ_PREV(next, phys_list));

    return mem;
}

struct memory * QURTK_mem_alloc(struct mem_pool *pool, qurt_size_t size) {
    qurt_addr_t addr;
    struct memory *ent = NULL;
    unsigned i, idx;

    ALLOC_DEBUG( "QURTK_mem_alloc pool %p size %p\n", pool, size );
    
    size = ALIGN_UP(size, MEM_MIN_SIZE);

    idx = size_to_idx(size);
    for (i = idx; i <= MEM_MAX_IDX; i++) {
        TAILQ_FOREACH(ent, FREE_LIST(pool, i), mem_list) {
            ALLOC_DEBUG( "QURTK_mem_alloc ent %p size %p\n", ent, size );
            if (ent->size >= size) {
                break;
            }
        }
        if (ent != NULL) {
            break;
        }
    }
    if (ent == NULL) {
        mem_coalesce(pool);
        for (i = idx; i <= MEM_MAX_IDX; i++) {
            TAILQ_FOREACH(ent, FREE_LIST(pool, i), mem_list) {
                if (ent->size >= size) {
                    break;
                }
            }
            if (ent != NULL) {
                break;
            }
        }
    }
    if (ent == NULL) {
        return NULL;
    }
    TAILQ_REMOVE(FREE_LIST(pool, i), ent, mem_list);

    addr = best_addr(size, ent);
    ent = trim_mem(pool, ent, addr, size);

    //print_glob_list(pool);
    return ent;
}

struct memory * QURTK_mem_alloc_fixed(struct mem_pool *pool, qurt_size_t size, qurt_addr_t addr) {
    struct memory *ent;
    unsigned idx;

    size = ALIGN_UP(size, MEM_MIN_SIZE);

    ALLOC_DEBUG( "QURTK_mem_alloc_fixed pool %p addr %p size %p\n", pool, addr, size );
    for (idx = 0; idx <= MEM_MAX_IDX; idx++) {
        TAILQ_FOREACH(ent, FREE_LIST(pool, idx), mem_list) {
            if (ent->addr <= addr && (addr+size-1) <= END(ent)) {
                break;
            }
        }
        if (ent != NULL) {
            break;
        }
    }
    if (ent == NULL) {
        if (pool->coalesced == 0)
            mem_coalesce (pool);
        //repeat the whole search
        for (idx = 0; idx <= MEM_MAX_IDX; idx++)
        {
            TAILQ_FOREACH(ent, FREE_LIST(pool, idx), mem_list)
            {
               if (ent->addr <= addr && (addr+size-1) <= END(ent))
                          break;
            }
            // Fix memory optimization bug
            if (ent != NULL)
                break;
        }
        if (ent == NULL) 
            return NULL;
    }
    TAILQ_REMOVE(FREE_LIST(pool, idx), ent, mem_list);
    ent = trim_mem(pool, ent, addr, size);
    //print_glob_list(pool);
    return ent;
}

void QURTK_mem_free(struct mem_pool *pool, struct memory *mem) {
    struct memory *tmp;
    unsigned idx;
    struct memory *prev, *next;

    mem->tf_bit = 0;
    if (TAILQ_EMPTY(GLOBAL_LIST(pool))){
           TAILQ_INIT(GLOBAL_LIST(pool));
           TAILQ_INSERT_HEAD(GLOBAL_LIST(pool), mem, phys_list);
           ALLOC_MERGE_DEBUG("empty global list, mem: %x, pool %x \n", mem, pool);
    }
    
    //print_glob_list(pool);

    prev = TAILQ_PREV(mem, phys_list);
    next = TAILQ_NEXT(mem, phys_list);
    //check neighbors and merge if free

    pool->coalesced = 0;

    ALLOC_MERGE_DEBUG("cur mem info....tf_bit: %d, addr: %x, size: %x, prev: %x, next: %x, self: %x\n",
                 mem->tf_bit, mem->addr, mem->size, TAILQ_PREV(mem, phys_list), 
    		 TAILQ_NEXT(mem, phys_list), mem); 
    //check neighbors here, if not free, continue as before,
    //if free: merge, and continue as before

    //check prev
   
    if (((unsigned int)prev != INVALID_ADDR) && (prev != NULL))
    {
      ALLOC_MERGE_DEBUG("prev_info: tf_bit: %d, addr: %x, size: %x, prev: %x, next: %x\n", 
		   prev->tf_bit, prev->addr, prev->size, TAILQ_PREV(prev, phys_list), 
		   TAILQ_NEXT(prev, phys_list));
      if (!prev->tf_bit) {
        if ( (END(prev)+1) == mem->addr ) {
            ALLOC_MERGE_DEBUG("coalescing block %x(%x, %x) with prev block %x(%x, %x)\n",
			 mem, mem->addr, mem->size, prev, prev->addr, prev->size);
            
            mem->size += prev->size;
            mem->addr = prev->addr;
            mem_unfree(pool, prev); //remove neighbor from free list
            QURTK_obj_cache_free(&QURTK_memory_cache, prev);
        }
       }
    }  
    //check next
    if (((unsigned int)next != INVALID_ADDR) && (next != NULL)) 
    {
      ALLOC_MERGE_DEBUG("next_info: tf_bit: %d, addr: %x, size: %x, prev: %x\n",
                  next->tf_bit, next->addr, next->size, TAILQ_PREV(next, phys_list)); 
      if(!next->tf_bit){
        if ( (END(mem)+1) == next->addr ) {
	    ALLOC_MERGE_DEBUG("coalescing block %x(%x, %x) with next block %x(%x, %x)\n",
                 mem, mem->addr, mem->size, next, next->addr, next->size);
            mem_unfree(pool, next);
            mem->size += next->size;
            QURTK_obj_cache_free(&QURTK_memory_cache, next);
        }    
      }
    }
  
    idx = best_idx(mem);

    if (TAILQ_EMPTY(FREE_LIST(pool, idx))) {
          TAILQ_INIT(FREE_LIST(pool, idx)); //add init
          TAILQ_INSERT_HEAD(FREE_LIST(pool, idx), mem, mem_list);   
          return; 
    }
 
    TAILQ_FOREACH(tmp, FREE_LIST(pool, idx), mem_list) {
        if (mem->size < tmp->size) {
            TAILQ_INSERT_BEFORE(FREE_LIST(pool, idx), tmp, mem, mem_list);
            return;
        }
    }
    TAILQ_INSERT_TAIL(FREE_LIST(pool, idx), mem, mem_list);
}


int mem_add(struct mem_pool *pool, qurt_addr_t addr, qurt_size_t size) {
    struct memory *ent;

    ent = mem_add_and_alloc(pool, addr, size);
    if (ent == NULL) {
        return QURT_EFATAL;
    }
    QURTK_mem_free(pool, ent);
    return QURT_EOK;
}

struct memory * mem_add_and_alloc(struct mem_pool *pool, qurt_addr_t addr, qurt_size_t size) {
    struct memory *ent;

    /* FIXME: We should not be adding more memory than is passed in. Bootinfo
     * needs to be fixed so that it rounds up small sizes to added. */
    size = ALIGN_UP(size, MEM_MIN_SIZE);

    ent = (struct memory *)QURTK_obj_cache_alloc(&QURTK_memory_cache);
    if (ent == NULL) {
        return NULL;
    }
    ent->addr = addr;
    ent->size = size;
    ALLOC_DEBUG( "alloc mem(%p) addr %p size %x\n", ent, addr, size ); 
    return ent;
}

void mem_pool_dump(struct mem_pool *pool ) {
    struct memory *ent;
    unsigned idx;

    ALLOC_POOL_DUMP_DEBUG( "pool %p\n", pool ); 
    for (idx = 0; idx <= MEM_MAX_IDX; idx++) {
        TAILQ_FOREACH(ent, FREE_LIST(pool, idx), mem_list) {
            ALLOC_POOL_DUMP_DEBUG( "ent %p %p -- %p   __[%d]\n", ent, ent->addr, ent->addr+ent->size, idx ); 
        }
    }
    ALLOC_POOL_DUMP_DEBUG("finished pool dump\n");
}

/*
 * This function traverses all the memory in the pool and joins adjacent
 * blocks.  This is rather unoptimised at the moment, but not ridiculously so.
 * The implementation is intended to be simple.  This function would probably
 * benefit most from tweaking the data structures.
 */
void mem_coalesce(struct mem_pool *pool) {
    //print_glob_list(pool);
    struct mem_list tmplist;
    struct memory *ent, *tmp, *prev;
    unsigned idx;

    if (pool->coalesced) {
        return;
    }
    /*
     * First we take all of the memory out of the lists...
     */
    TAILQ_INIT(&tmplist);
    for (idx = 0; idx <= MEM_MAX_IDX; idx++) {
        TAILQ_FOREACH_SAFE(ent, FREE_LIST(pool, idx), mem_list, tmp) {
            struct memory * sort_ent;
            TAILQ_REMOVE(FREE_LIST(pool, idx), ent, mem_list);

            /* Insert into tmplist in soted order */
            TAILQ_FOREACH(sort_ent, &tmplist, mem_list){
                if(ent->addr < sort_ent->addr){
                    TAILQ_INSERT_BEFORE(&tmplist, sort_ent, ent, mem_list);
                    break;
                }
            }

            if (sort_ent == NULL){
                TAILQ_INSERT_TAIL(&tmplist, ent, mem_list);
            }
        }
    }

    /*
     * Then we scan the list merging adjacent blocks, and adding the coalesced
     * blocks back into the free lists.
     */

    prev = NULL;
    TAILQ_FOREACH_SAFE(ent, &tmplist, mem_list, tmp) {
        if (prev) {
            TAILQ_REMOVE(&tmplist, prev, mem_list);
            if ( (END(prev)+1) == ent->addr ) {
                ent->size += prev->size;
                ent->addr  = prev->addr;
                QURTK_obj_cache_free(&QURTK_memory_cache, prev);
            }
            else {
                QURTK_mem_free(pool, prev);
            }
        }
        /*
         * Here, ent is the "current" block, which has not yet been added to
         * the free list.
         */
        prev = ent;
    }
    if (prev) QURTK_mem_free(pool, prev);

    pool->coalesced = 1;
}


struct memory * QURTK_mem_alloc_aligned(struct mem_pool *pool, qurt_size_t size) {
    qurt_addr_t addr;
    struct memory *ent = NULL;
    unsigned i, idx;

    ALLOC_DEBUG( "QURTK_mem_alloc_aligned pool %p size %p\n", pool, size );
    size = ALIGN_UP(size, MEM_MIN_SIZE);

    idx = size_to_idx(size);
    for (i = idx; i <= MEM_MAX_IDX; i++) {
        TAILQ_FOREACH(ent, FREE_LIST(pool, i), mem_list) {
            ALLOC_DEBUG( "QURTK_mem_alloc ent %p size %p\n", ent, size );
            if (ent->size >= size && addr_aligned(ent, size)) {
                break;
            }
        }
        if (ent != NULL) {
            break;
        }
    }
    if (ent == NULL) {
        mem_coalesce(pool);
        for (i = idx; i <= MEM_MAX_IDX; i++) {
            TAILQ_FOREACH(ent, FREE_LIST(pool, i), mem_list) {
                if (ent->size >= size && addr_aligned(ent, size)) {
                    break;
                }
            }
            if (ent != NULL) {
                break;
            }
        }
    }
    if (ent == NULL) {
        return NULL;
    }
    TAILQ_REMOVE(FREE_LIST(pool, i), ent, mem_list);

    addr = best_addr(size, ent);
    ent = trim_mem(pool, ent, addr, size);

    //print_glob_list(pool);
    return ent;
}

