/*
 * Australian Public Licence B (OZPLB)
 *
 * Version 1-0
 *
 * Copyright (c) 2006-2010, Open Kernel Labs, Inc.
 *
 * All rights reserved.
 *
 * Developed by: Embedded, Real-time and Operating Systems Program (ERTOS)
 *               National ICT Australia
 *               http://www.ertos.nicta.com.au
 *
 * Permission is granted by Open Kernel Labs, Inc., free of charge, to
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
 *     * Neither the name of Open Kernel Labs, Inc., nor the names of its
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
 * conditions, or imposes obligations or liability on Open Kernel Labs, Inc.
 * or one of its contributors in respect of the Software that
 * cannot be wholly or partly excluded, restricted or modified, the
 * liability of Open Kernel Labs, Inc. or the contributor is limited, to
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
/*======================================================================
                        pgtable.c

GENERAL DESCRIPTION
  Contains pagetable related functions

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2016, 2019-2022  by Qualcomm Technologies, Inc.  All Rights Reserved.
 ======================================================================*/

#include "pgtable.h"
#include "qurt_memory.h"
#include "qurt_error.h"
#include "assert.h"
#include "malloc.h"
#include "qurt_system.h"
#include "qurt_interrupt.h"
#include "qapi_diag.h"

#define ARM_NUM_L1_PGTABLE_ENTRIES 4096
#define ARM_L2_PGTABLE_SIZE        1024
#define ARM_NUM_L2_PGTABLE_ENTRIES (ARM_L2_PGTABLE_SIZE/4)
#define ARM_NUM_L2_PGTABLES        (L2_PGTABLE_POOL_SIZE/ARM_L2_PGTABLE_SIZE)

#ifdef QURT_MAX_ASIDS
#define ARM_MAX_ASIDS QURT_MAX_ASIDS
#else
#define ARM_MAX_ASIDS 1
#endif

#define QURT_LOG(xx_fmt, ...) QAPI_MSG(MSG_SSID_DIAG, MSG_LEGACY_HIGH, xx_fmt,##__VA_ARGS__)

/* Page Table should be 16KB aligned */
uint32 arm_mmu_pgtable[ARM_MAX_ASIDS][ARM_NUM_L1_PGTABLE_ENTRIES] __attribute__((aligned(16384))) = {0};
uint32 arm_mmu_l2_pgtable[ARM_NUM_L2_PGTABLE_ENTRIES*ARM_NUM_L2_PGTABLES] __attribute__((aligned(4096))) = {0};

/*
typedef struct arm_pool_type {
    word_t * pool_start;
    word_t pool_size;
    word_t block_size;
    word_t * pool_mask;
    word_t mask_size;
} arm_pool_t;
*/
// arm_pool_t * arm_l2_pgtable_pool;
arm_block_pool arm_l2_pgtable_pool;

word_t arm_l2_pgtable_pool_initted = 0;


extern uint32 asid_allocation_bits;
extern void cache_clean_drange_kernel(void *src, word_t size);
extern void cache_clean_dline_kernel(void *target);
extern int arm_pagetable_lookup_with_asid (word_t vaddr, word_t *paddr,  word_t *size, word_t *cache_attribs, word_t *perm, word_t asid);


extern int is_valid_cache_attr (qurt_mem_cache_mode_t cache_attr);

void zero_mem_range(word_t dest, word_t bytes) {
    word_t words = bytes / (word_t)sizeof(word_t);
    word_t *target = (word_t *)dest;

    ASSERT(target != NULL);
    ASSERT((dest % sizeof(word_t)) == 0UL);
    ASSERT((bytes % sizeof(word_t)) == 0UL);

    while (words > 4UL) {
        *target++ = 0UL;
        *target++ = 0UL;
        *target++ = 0UL;
        *target++ = 0UL;
        words -= 4UL;
    }

    while (words > 0UL) {
        *target = 0UL;
        target++;
        words--;
    }
}

void writeZeroes(unsigned int addr){
	unsigned char *ptr = (unsigned char *)addr;
	int size = L2_PGTABLE_POOL_SIZE;
	while(size--){
		*ptr = 0x0;
		ptr++;
	}
}
void arm_l2_pgtable_pool_init() {
    int ret;
    
    if (arm_l2_pgtable_pool_initted == 0) {
        ret = arm_block_pool_create(&arm_l2_pgtable_pool, (word_t)arm_mmu_l2_pgtable , L2_PGTABLE_POOL_SIZE, ARM_L2_PGTABLE_SIZE);
        if (ret != QURT_EOK) {
            ERR_FATAL("Unable to create L2 PGTABLE Pool!", (uint32)&arm_l2_pgtable_pool,arm_mmu_l2_pgtable,L2_PGTABLE_POOL_SIZE);
        }
        arm_l2_pgtable_pool_initted = 1;
    }
}


word_t mmu_get_arch_rwx(word_t rwx, word_t asid) {
    word_t access;

    if (asid != 0) {
        switch (rwx) {
        case QURT_PERM_FULL:
            access = ARM_ACCESS_KRWX_URWX;
            break;
        case QURT_PERM_READ_WRITE:
            access = ARM_ACCESS_KRW_URW;
            break;
        case QURT_PERM_READ:
            access = ARM_ACCESS_KR_UR;
            break;
        case QURT_PERM_EXECUTE:
            access = ARM_ACCESS_KRX_URX;
            break;
        case QURT_PERM_NONE:
            /* FIXME: chagne this to ARM_ACCESS_KNONE later if it doesn't break anything */
            access = ARM_ACCESS_KRW_UNONE;
            break;
        default:
            access = ~0UL;
            break;
        }
    } else {
        switch (rwx) {
        case QURT_PERM_FULL:
            access = ARM_ACCESS_KRWX_UNONE;
            break;
        case QURT_PERM_READ_WRITE:
            access = ARM_ACCESS_KRW_UNONE;
            break;
        case QURT_PERM_READ:
            access = ARM_ACCESS_KR_UNONE;
            break;
        case QURT_PERM_EXECUTE:
            access = ARM_ACCESS_KRX_UNONE;
            break;
        case QURT_PERM_NONE:
            access = ARM_ACCESS_KNONE;
            break;
        default:
            access = ~0UL;
            break;
        }
    }
    return access;
}



word_t mmu_get_generic_rwx(word_t access) {
    word_t rwx;

    switch (access) {
    case ARM_ACCESS_KRWX_URWX:
    case ARM_ACCESS_KRWX_UNONE:
        rwx = QURT_PERM_FULL;
        break;
    case ARM_ACCESS_KRW_URW:
    case ARM_ACCESS_KRW_UNONE:
        rwx = QURT_PERM_READ_WRITE;
        break;
    case ARM_ACCESS_KR_UR:
    case ARM_ACCESS_KR_UNONE:
        rwx = QURT_PERM_READ;
        break;
    case ARM_ACCESS_KRX_URX:
    case ARM_ACCESS_KRX_UNONE:
        rwx = QURT_PERM_EXECUTE;
        break;
    case ARM_ACCESS_KNONE:
        rwx = QURT_PERM_NONE;
        break;
    default:
        rwx = ~0UL;
        break;
    }
    return rwx;
}


INLINE int is_valid_page_size (word_t size) {
    /* Is 'size' a power of two? */
    if (!IS_POWER2_OR_ZERO(size)) {
        return 0;
    }

    /* Is the given 'size' supported by hardware? */
    if (page_size_check(size) == 0UL) {
        return 0;
    }
    return 1;
}

INLINE int is_valid_asid (word_t asid) {
    if (asid < ARM_MAX_ASIDS)
        return 1;
    else
        return 0;
}

INLINE int is_active_asid(int asid){	
		if(!is_valid_asid(asid)) return 0;
		if (asid_allocation_bits & (1 << asid)) {
			return 1;
		} 
		return 0;
}

//#pragma push
//#pragma arm

int arm_map_page (word_t vaddr, word_t paddr, word_t size, word_t cache_attribs, word_t perm, word_t asid) {

    /*
     * Basic operations: Note 1k pages not used.
     * Mappings are not replaced if existing.
     *
     * index pgtable (1M)
     *  - if 16M
     *    - 16M duplicate entries
     *  - if 1M - set entry
     *  - if smaller - goto sub page (alloc sub-page)
     *    - index subpage (4k)
     *    if 64k
     *      - 64k duplicate entries
     *    else
     *      - set entry
     */

    word_t *entry_ptr;
    word_t entry;
    word_t access;
    word_t *pgtable;
    unsigned int int_save;

	//==== save input args
		word_t arg_vaddr;
		word_t arg_paddr;
		word_t arg_size;
		word_t arg_cache_attribs;
		word_t l_asid;
	//====
	

    if (!is_valid_asid(asid)) {
        QURT_LOG("Invalid page asid! vaddr:0x%x paddr:0x%x size:0x%x asid:%d", vaddr, paddr, size, asid);
        return QURT_EINVALID;
    }

    if (!is_valid_page_size(size)) {
        QURT_LOG("Invalid page size! vaddr:0x%x paddr:0x%x size:0x%x", vaddr, paddr, size);
        return QURT_EINVALID;
    }

    /* Is virtual address aligned to the size? */
    if ((vaddr & (size - 1UL)) != 0UL) {
        QURT_LOG("Invalid virt addr! vaddr:0x%x paddr:0x%x size:0x%x",vaddr,paddr,size);
        return QURT_EINVALID;
    }

    /* check if page is inside kernel virtual address range */
    /* Fixme: skip this check for now as exact user memory boundary is not yet known.
    if (!is_user_virt(vaddr, size)) {
        return QURT_EINVALID;
    } */

    /* Is physical address aligned to the size? */
    if ((paddr & (size - 1UL)) != 0UL) {
        QURT_LOG("Invalid phys addr! vaddr:0x%x paddr:0x%x size:0x%x",vaddr,paddr,size);
        return QURT_EINVALID;
    }

    /* Are the RWX flags valid and supported by hardware? */
    access = mmu_get_arch_rwx(perm, asid);
    if (access == ~0UL) {
        QURT_LOG("Invalid perm! vaddr:0x%x paddr:0x%x perm:0x%x asid:%d",vaddr,paddr,perm,asid);
        return QURT_EINVALID;
    }

    /* Are the cache (and other) cache_attribs valid? */
    if (is_valid_cache_attr((qurt_mem_cache_mode_t)cache_attribs) == 0) {
        QURT_LOG("Invalid cache attr! vaddr:0x%x paddr:0x%x cache_attribs:0x%x", vaddr, paddr, cache_attribs);
        return QURT_EINVALID;
    }


    INTLOCK_SAVE(int_save);
	if(asid == KERNEL_ASID)
	{
		arg_vaddr = vaddr;
		arg_paddr = paddr;
		arg_size  = size;
		arg_cache_attribs = cache_attribs;
	}	
    
    pgtable = (word_t *)arm_mmu_pgtable[asid];
    /* Get the top-level pagetable entry for this virtual address. */
    entry_ptr = arm_pgindex_1m(pgtable, vaddr);
    entry = *entry_ptr;

    if (size == SIZE_16M) {
        word_t i, *tmp;

        tmp = entry_ptr;
        for (i = 0UL; i < (1UL << (ARM_16M_BITS - ARM_1M_BITS)); i++) {
            //WCET_CONSTANT(1UL << (ARM_16M_BITS - ARM_1M_BITS));

            if (is_arm_l1_valid(*tmp)) {
                INTFREE_RESTORE(int_save);
                QURT_LOG("Re-mapping an existing mapping for a 16M page! vaddr:0x%x paddr:0x%x size:0x%x", vaddr, paddr);
                return QURT_EREMAP;
            }
            tmp += 1;
        }
        entry = arm_l1_gen_16mpage(paddr, vaddr, access, cache_attribs);
        tmp = entry_ptr;
        for (i = 0UL; i < (1UL << (ARM_16M_BITS - ARM_1M_BITS)); i++) {
            //WCET_CONSTANT(1UL << (ARM_16M_BITS - ARM_1M_BITS));

            *tmp = entry;
            tmp += 1;
        }
        /* 16M mapping uses 16 entries (2 cache lines), clean second line now */
        cache_clean_dline_kernel(entry_ptr + 8);
    } else if (size == SIZE_1M) {
        if (is_arm_l1_valid(entry)) {
            INTFREE_RESTORE(int_save);
            QURT_LOG("Re-mapping an existing mapping for a 1M page! vaddr:0x%x paddr:0x%x size:0x%x", vaddr, paddr);
            return QURT_EREMAP;
        }
        entry = arm_l1_gen_1mpage(paddr, vaddr, access, cache_attribs);
        *entry_ptr = entry;
    } else {
        word_t *subtree, phys_tree;
        // word_t err = ERROR_OK;

        if (!is_arm_l1_valid(entry)) {
            if (arm_l2_pgtable_pool_initted == 0)
                arm_l2_pgtable_pool_init();
            /* Allocate memory for the subtree. */
            subtree = arm_block_allocate(&arm_l2_pgtable_pool);

            /* Now install the subtree into our top-level pagetable. */
            entry = arm_l1_gen_subtree(mmu_virt_to_phys(subtree), vaddr);
            *entry_ptr = entry;

            /* Clean cache line for hardware walker visibility. */
            cache_clean_dline_kernel(entry_ptr);
        } else if (!is_arm_l1_subtree(entry)) {
            INTFREE_RESTORE(int_save);
            QURT_LOG("Re-mapping an existing mapping! vaddr:0x%x paddr:0x%x size:0x%x", vaddr, paddr, size);
            return QURT_EREMAP;
        }
        phys_tree = arm_l1_subtree_addr(entry);
        if (phys_tree == 0UL) {
            INTFREE_RESTORE(int_save);
            QURT_LOG("No valid subtree addr for the VA! vaddr:0x%x paddr:0x%x size:0x%x", vaddr, paddr, size);
            return QURT_EMEM;
        }
        subtree = (word_t *)mmu_phys_to_virt(phys_tree);

        if (size == SIZE_64K) {
            word_t i, *tmp;
            entry_ptr = arm_pgindex_64k(subtree, vaddr);

            /* Handle duplicate entries for 64k */
            tmp = entry_ptr;
            for (i = 0UL; i < (1UL << (ARM_64K_BITS - ARM_4K_BITS)); i++) {
                //WCET_CONSTANT(1UL << (ARM_64K_BITS - ARM_4K_BITS));

                if (is_arm_l2_valid(*tmp)) {
                    INTFREE_RESTORE(int_save);
                    QURT_LOG("Re-mapping an existing mapping for a 64K page! vaddr:0x%x paddr:0x%x", vaddr, paddr);
                    return QURT_EREMAP;
                }
                tmp += 1;
            }
            entry = arm_l2_gen_64kpage(paddr, vaddr, access, cache_attribs);
            tmp = entry_ptr;
            for (i = 0UL; i < (1UL << (ARM_64K_BITS - ARM_4K_BITS)); i++) {
                //WCET_CONSTANT(1UL << (ARM_64K_BITS - ARM_4K_BITS));

                *tmp = entry;
                tmp += 1;
            }
            /* 64K mapping uses 16 entries (2 cache lines), clean second line now */
            cache_clean_dline_kernel(entry_ptr + 8);
        } else if (size == SIZE_4K) {
            entry_ptr = arm_pgindex_4k(subtree, vaddr);
            entry = *entry_ptr;

            if (is_arm_l2_valid(entry)) {
                INTFREE_RESTORE(int_save);
                QURT_LOG("Re-mapping an existing mapping for a 4K page! vaddr:0x%x paddr:0x%x", vaddr, paddr);
                return QURT_EREMAP;
            }
            entry = arm_l2_gen_4kpage(paddr, vaddr, access, cache_attribs);
            *entry_ptr = entry;
        } else {
            //panic("");
            INTFREE_RESTORE(int_save);
            QURT_LOG("Unhandled page size!! vaddr:0x%x paddr:0x%x size:0x%x",vaddr, paddr,size);
            return QURT_EINVALID;

        }
    }

    /* Clean cache line for hardware walker visibility. */
    cache_clean_dline_kernel(entry_ptr);

    // cache_drain_writebuffer();
    __asm ( "dsb" );

    INTFREE_RESTORE(int_save);

	if(asid == KERNEL_ASID){		// map to all active asid if this is a kernel mapping
		for (l_asid = 1; l_asid < ARM_MAX_ASIDS; l_asid++) {
			if(is_active_asid(l_asid)){
				/* Map to this ASID with only READ-WRITE permission for Kernel, and none to USER */
				arm_map_page (arg_vaddr, arg_paddr, arg_size, arg_cache_attribs, QURT_PERM_NONE, l_asid);
			}
		}
	}

    return QURT_EOK;
}



int arm_unmap_page (word_t vaddr, word_t size, word_t asid) {

    /*
     * Basic operations: Note 1k pages not used.
     * Only single pages are removed, this does not do batching.
     * Subtree's are not removed.
     *
     * index pgtable (1M)
     *  - if 1M - clear entry
     *  - if smaller - goto sub page (alloc sub-page)
     *    - index subpage (4k)
     *    if 64k
     *      - clear 64k duplicate entries
     *    else
     *      - clear entry
     */

    word_t *entry_ptr;
    word_t entry;
    word_t *pgtable;
    unsigned int int_save;
	word_t is_subtree_free_required = 1;
	word_t physaddr;
	word_t cache_attribs;
	word_t perm;

	//==== save input args
		word_t arg_vaddr;
		word_t arg_size;

		word_t l_asid;
	//====


    if (!is_valid_page_size(size)) {
        QURT_LOG("Invalid page size! vaddr:0x%x size:0x%x asid :%d",vaddr,size,asid);
		return QURT_EINVALID;
    }

    /* Is virtual address aligned to the size? */
    if ((vaddr & (size - 1UL)) != 0UL) {
        QURT_LOG("Invalid virt addr! vaddr:0x%x size:0x%x asid:%d",vaddr,size,asid);
		return QURT_EINVALID;
    }


    /* check if page is inside kernel virtual address range */
    /* Fixme: skip this check for now as exact user memory boundary is not yet known.
    if (!is_user_virt(vaddr, size)) {
        return QURT_EINVALID;
    } */

    if (!is_valid_asid(asid)) {
        QURT_LOG("Invalid page asid! vaddr:0x%x size:0x%x asid:0x%x ",vaddr,size,asid);
        return QURT_EINVALID;
    }

    INTLOCK_SAVE(int_save);

	if(asid == KERNEL_ASID){
		arg_vaddr = vaddr;
		arg_size  = size;
	}
	
    pgtable = (word_t *)arm_mmu_pgtable[asid];
    entry_ptr = arm_pgindex_1m(pgtable, vaddr);
    entry = *entry_ptr;

    if (size == SIZE_16M) {
        word_t i, *tmp;

        tmp = entry_ptr;
        for (i = 0UL; i < (1UL << (ARM_16M_BITS - ARM_1M_BITS)); i++) {
            // WCET_CONSTANT(1UL << (ARM_16M_BITS - ARM_1M_BITS));

            if (!is_arm_l1_16mpage(*tmp)) {
                INTFREE_RESTORE(int_save);
                QURT_LOG("Mem Unmap failed: Not a valid 16M page entry! entry_ptr:0x%x vaddr:0x%x size;0x%x", entry_ptr, vaddr, size);
				return QURT_ENOMAP;
            }
            tmp += 1;
        }

        tlb_flush_addr_asid(vaddr, asid);

        entry = arm_l1_gen_invalid();
        tmp = entry_ptr;
        for (i = 0UL; i < (1UL << (ARM_16M_BITS - ARM_1M_BITS)); i++) {
            // WCET_CONSTANT(1UL << (ARM_16M_BITS - ARM_1M_BITS));

            *tmp = entry;
            tmp += 1;
        }
        /* 16M mapping uses 16 entries (2 cache lines), clean second line now */
        cache_clean_dline_kernel(entry_ptr + 8);
    } else if (size == SIZE_1M) {
        if (!is_arm_l1_1mpage(entry)) {
            INTFREE_RESTORE(int_save);
            QURT_LOG("Mem Unmap failed: Not a valid 1M page entry! entry_ptr:0x%x vaddr:0x%x size;0x%x", entry_ptr, vaddr, size);
			return QURT_ENOMAP;
        }

        tlb_flush_addr_asid(vaddr, asid);

        entry = arm_l1_gen_invalid();
        *entry_ptr = entry;
    } else {
        word_t *subtree, phys_tree;
        if (!is_arm_l1_valid(entry)) {
            INTFREE_RESTORE(int_save);
            QURT_LOG("Mem Unmap failed: Not a valid L1 PT entry! entry_ptr:0x%x vaddr:0x%x size;0x%x", entry_ptr, vaddr, size);
			return QURT_ENOMAP;
        }
        if (!is_arm_l1_subtree(entry)) {
            INTFREE_RESTORE(int_save);
            QURT_LOG("Mem Unmap failed: Other mappings exist! entry_ptr:0x%x vaddr:0x%x size;0x%x", entry_ptr, vaddr, size);
			return QURT_EFAILED;
        }
        phys_tree = arm_l1_subtree_addr(entry);
        if (phys_tree == 0UL) {
            INTFREE_RESTORE(int_save);
            QURT_LOG("Mem Unmap failed: No L2 pagetable! entry_ptr:0x%x vaddr:0x%x size;0x%x", entry_ptr, vaddr, size);
			return QURT_EFAILED;
        }
        subtree = (word_t *)mmu_phys_to_virt(phys_tree);

        if (size == SIZE_64K) {
            word_t i, *tmp;
            entry_ptr = arm_pgindex_64k(subtree, vaddr);

            /* Handle duplicate entries for 64k */
            tmp = entry_ptr;
            for (i = 0UL; i < (1UL << (ARM_64K_BITS-ARM_4K_BITS)); i++) {
                // WCET_CONSTANT(1UL << (ARM_64K_BITS-ARM_4K_BITS));

                if (!is_arm_l2_64kpage(*tmp)) {
                    QURT_LOG("Mem Unmap failed: Mapping doesn't exist for 64K page! entry_ptr:0x%x vaddr:0x%x", entry_ptr, vaddr);
					return QURT_ENOMAP;
                }
                tmp += 1;
            }

            tlb_flush_addr_asid(vaddr, asid);

            entry = arm_l2_gen_invalid();
            tmp = entry_ptr;

            for (i = 0UL; i < (1UL << (ARM_64K_BITS-ARM_4K_BITS)); i++) {
                // WCET_CONSTANT(1UL << (ARM_64K_BITS-ARM_4K_BITS));

                *tmp = entry;
                tmp += 1;
            }
            /* 64K mapping uses 16 entries (2 cache lines), clean second line now */
            cache_clean_dline_kernel(entry_ptr + 8);
        } else if (size == SIZE_4K) {
            entry_ptr = arm_pgindex_4k(subtree, vaddr);
            entry = *entry_ptr;

            if (!is_arm_l2_4kpage(entry)) {
                INTFREE_RESTORE(int_save);
                QURT_LOG("Mem Unmap failed: Mapping doesn't exist for 4K page! entry_ptr:0x%x vaddr:0x%x", entry_ptr, vaddr);
				return QURT_ENOMAP;
            }

            tlb_flush_addr_asid(vaddr, asid);

            entry = arm_l2_gen_invalid();
            *entry_ptr = entry;
        } else {
            INTFREE_RESTORE(int_save);
            QURT_LOG("Mem Unmap failed: Invalid size! entry_ptr:0x%x vaddr:0x%x size;0x%x", entry_ptr, vaddr, size);
			return QURT_EFAILED;
        }

        /* If no valid L2 entries exist in subtree after unmapping, 
           free subtree and mark L1 PTE as invalid */
        {
            word_t i, *tmp, *l1_entry_ptr;
            tmp = subtree;
            
            for(i = 0; i < ARM_SUBTREE_ENTRIES; i++, tmp++)
            {
                if(is_arm_l2_4kpage(*tmp) || is_arm_l2_64kpage(*tmp))
                {
                    is_subtree_free_required = 0;
                    break;
                }   
            }

            if(is_subtree_free_required)
            {
                /* Free subtree memory */
                arm_block_free(&arm_l2_pgtable_pool, (word_t)subtree);

                /* Mark L1 entry as invalid */
                l1_entry_ptr = arm_pgindex_1m(pgtable, vaddr);
                entry = arm_l1_gen_invalid();
                *l1_entry_ptr = entry;

                /* Flush cache line of L1 entry */
                cache_clean_dline_kernel(l1_entry_ptr);
            }
        }
 
    }

    /* Clean cache line for hardware walker visibility. */
    cache_clean_dline_kernel(entry_ptr);

    // cache_drain_writebuffer();
    __asm ( "dsb" );

   INTFREE_RESTORE(int_save);

	if(asid == KERNEL_ASID){
		for (l_asid = 1; l_asid < ARM_MAX_ASIDS; l_asid++) {
			if(is_active_asid(l_asid)){
				/* Check if there is a mapping for this virtual address in the page table corresponding to this ASID. */
				if(!arm_pagetable_lookup_with_asid(vaddr, &physaddr, &size, &cache_attribs, &perm, l_asid))
				{
				    arm_unmap_page (arg_vaddr, arg_size,l_asid);
			    }
				else
				{
					QURT_LOG("Mem unmap failed: no valid mapping in asid: %d vaddr:0x%x, size:0x%x",l_asid,vaddr,size);
				}
		    }
	    }
	}

    return QURT_EOK;
}
/*
int arm_map_page (word_t vaddr, word_t paddr, word_t size, word_t cache_attribs, word_t perm, word_t asid) {

    int ret;
    
    // For a user space mapping, always create a corresponding mapping in the kernel space as well
    if (asid != 0) {
        arm_map_page_imp(vaddr, paddr, size, cache_attribs, perm, asid);
        if (ret != QURT_EOK)
            return ret;
    }
    
    ret = arm_map_page_imp(vaddr, paddr, size, cache_attribs, perm, 0UL);
    if (ret != QURT_EOK) {
        arm_unmap_page_imp(vaddr, size, asid);
    }
    return ret;
}


int arm_unmap_page (word_t vaddr, word_t size, word_t asid) {

    int ret;
    // For a user space mapping, there should be a corresponding mapping in the kernel space as well
    if (asid != 0) {
        ret = arm_unmap_page_imp(vaddr, size, asid);
        if (ret != QURT_EOK)
            return ret;
    }
    
    return arm_unmap_page_imp(vaddr, size, 0UL);
}
*/

// Takes vaddr as input and returns all other attributes
int arm_pagetable_lookup_with_asid (word_t vaddr, word_t *paddr,  word_t *size, word_t *cache_attribs, word_t *perm, word_t asid) {

    word_t *entry_ptr;
    word_t entry;
    word_t access;
    word_t *pgtable;
    unsigned int int_save;
	word_t *subtree, phys_tree;

    /* check if page is inside kernel virtual address range */
    /* Fixme: skip this check for now as exact user memory boundary is not yet known.
    if (vaddr >= KERNEL_VADDR_BASE) {
        return ERROR_INVALID_ARGUMENT;
    } */

    if (!is_active_asid(asid)) {
        QURT_LOG("Invalid page asid! vaddr:0x%x paddr:0x%x size:0x%x asid:0x%x", vaddr, paddr, size, asid);
        return QURT_EINVALID;
    }

    INTLOCK_SAVE(int_save);
    
        pgtable = (word_t *)arm_mmu_pgtable[asid];
        entry_ptr = arm_pgindex_1m(pgtable, vaddr);
        entry = *entry_ptr;

        if (is_arm_l1_subtree(entry)) {
            phys_tree = arm_l1_subtree_addr(entry);
            ASSERT(phys_tree != 0UL);
            subtree = mmu_phys_to_virt(phys_tree);

            entry_ptr = arm_pgindex_4k(subtree, vaddr);
            entry = *entry_ptr;

            if (is_arm_l2_4kpage(entry)) {
                access = arm_l2_4kpage_access(entry);
                *paddr = arm_l2_4kpage_phys(entry);
                *cache_attribs = arm_l2_4kpage_attribs(entry);
                *size = SIZE_4K;
            } else if (is_arm_l2_64kpage(entry)) {
                /* if (!IS_ALIGNED(vaddr, SIZE_64K)) {
                    INTFREE_RESTORE(int_save);
                    return QURT_EMEM;
                } */
                access = arm_l2_64kpage_access(entry);
                *paddr = arm_l2_64kpage_phys(entry);
                *cache_attribs = arm_l2_64kpage_attribs(entry);
                *size = SIZE_64K;
            } else {
                INTFREE_RESTORE(int_save);
                return QURT_EMEM;
            }
            
        } else if (is_arm_l1_1mpage(entry)) {
            /* if (!IS_ALIGNED(vaddr, SIZE_1M)) {
                INTFREE_RESTORE(int_save);
                return QURT_EMEM;
            } */

            access = arm_l1_1mpage_access(entry);
            *paddr = arm_l1_1mpage_phys(entry);
            *cache_attribs = arm_l1_1mpage_attribs(entry);
            *size = SIZE_1M;
        
        } else if (is_arm_l1_16mpage(entry)) {
            /* if (!IS_ALIGNED(vaddr, SIZE_16M)) {
                INTFREE_RESTORE(int_save);
                return QURT_EMEM;
            } */

            access = arm_l1_16mpage_access(entry);
            *paddr = arm_l1_16mpage_phys(entry);
            *cache_attribs = arm_l1_16mpage_attribs(entry);
            *size = SIZE_16M;
        
        } else {
            INTFREE_RESTORE(int_save);
            return QURT_ENOMAP;
        }

    *perm = access;
    if (*perm == ~0UL) {
        INTFREE_RESTORE(int_save);
        QURT_LOG("Page Table entry has invalid permission bits! access:0x%x paddr:0x%x vaddr:0x%x", access, *paddr, vaddr);
        return QURT_EMEM;
    }

    if (!is_valid_page_size(*size)) {
        INTFREE_RESTORE(int_save);
        //return QURT_EINVALID;
        QURT_LOG("Page Table entry has invalid page size! vaddr:0x%x paddr:0x%x size:0x%x", vaddr, *paddr, size);
        return QURT_EMEM;
    }

    /* Is physical address aligned to the size? */
    if ((*paddr & (*size - 1UL)) != 0UL) {
        INTFREE_RESTORE(int_save);
        QURT_LOG("Page Table entry has invalid phys addr! vaddr:0x%x paddr:0x%x size:0x%x", vaddr, *paddr, size);
        return QURT_EMEM;
    }
    
    if (is_valid_cache_attr((qurt_mem_cache_mode_t)*cache_attribs) == 0) {
        INTFREE_RESTORE(int_save);
        QURT_LOG("Page Table entry has invalid cache attr! vaddr:0x%x paddr:0x%x cache_attr:0x%x", vaddr, *paddr, cache_attribs);
        return QURT_EMEM;
    }

    INTFREE_RESTORE(int_save);
    return QURT_EOK;
}

int arm_pagetable_lookup (word_t vaddr, word_t *paddr,  word_t *size, word_t *cache_attribs, word_t *perm, word_t *asid) {

    word_t l_asid;
	word_t tmp;
    int ret = QURT_EINVALID;
    
    for (l_asid = 0; l_asid < ARM_MAX_ASIDS; l_asid++) {
		if(!is_active_asid(l_asid)) continue;
        ret = arm_pagetable_lookup_with_asid (vaddr, paddr, size, cache_attribs, perm, l_asid);
        if (ret == QURT_EOK) {
            *asid = l_asid;
			tmp = mmu_get_generic_rwx(*perm);
			*perm = tmp;
            break;
        }
    }
    return ret;
}

int arm_pagetable_lookup_reverse (word_t vaddr, word_t *paddr,  word_t *size, word_t *cache_attribs, word_t *perm, word_t *asid) {

    word_t l_asid;
    int ret = QURT_EINVALID;
  	/* pagetable lookup with user ASID first and last with Kernel ASID */
    for (l_asid = ARM_MAX_ASIDS-1; l_asid > 0 ; l_asid--) {
		if(!is_active_asid(l_asid)) continue;
        ret = arm_pagetable_lookup_with_asid (vaddr, paddr, size, cache_attribs, perm, l_asid);
        if ((ret == QURT_EOK) && (*perm ==ARM_ACCESS_KRWX_URWX) ) {
            *asid = l_asid;
            return ret;
        }
    }
	ret = arm_pagetable_lookup_with_asid (vaddr, paddr, size, cache_attribs, perm, KERNEL_ASID);
    if (ret == QURT_EOK ) {
          *asid = KERNEL_ASID;
    }
	return ret;
}

/* Remap works only if we are trying to change the page attributes of a particular page.
 * So page size should match that of an existing mapping and VA should point to start of the page
 */
int arm_remap_page (word_t vaddr, word_t paddr, word_t size, word_t cache_attribs, word_t perm, word_t asid) {

    /*
     * Basic operations: Note 1k pages not used.
     * Mappings are not replaced if existing.
     *
     * index pgtable (1M)
     *  - if 16M
     *    - 16M duplicate entries
     *  - if 1M - set entry
     *  - if smaller - goto sub page (alloc sub-page)
     *    - index subpage (4k)
     *    if 64k
     *      - 64k duplicate entries
     *    else
     *      - set entry
     */

    word_t *entry_ptr;
    word_t entry;
    word_t access;
    word_t *pgtable;
    unsigned int int_save;

    if (!is_valid_page_size(size)) {
        QURT_LOG("Invalid page size! vaddr : 0x%x, paddr : 0x%x, size : 0x%x ", vaddr, paddr, size);
        return QURT_EINVALID;
    }

    /* Is virtual address aligned to the size? */
    if ((vaddr & (size - 1UL)) != 0UL) {
        QURT_LOG("Invalid virt addr! vaddr : 0x%x, paddr : 0x%x, size : 0x%x ", vaddr, paddr, size);
        return QURT_EINVALID;
    }

    /* check if page is inside kernel virtual address range */
    /* Fixme: skip this check for now as exact user memory boundary is not yet known.
    if (!is_user_virt(vaddr, size)) {
        return QURT_EINVALID;
    } */

    /* Is physical address aligned to the size? */
    if ((paddr & (size - 1UL)) != 0UL) {
        QURT_LOG("Invalid phys addr! vaddr : 0x%x, paddr : 0x%x, size : 0x%x ", vaddr, paddr, size);
        return QURT_EINVALID;
    }

    /* Are the RWX flags valid and supported by hardware? */
    access = mmu_get_arch_rwx(perm, asid);
    if (access == ~0UL) {
        QURT_LOG("Invalid permission! vaddr : 0x%x, paddr : 0x%x, perm : 0x%x ", vaddr, paddr, perm);
        return QURT_EINVALID;
    }

    /* Are the cache (and other) cache_attribs valid? */
    if (is_valid_cache_attr((qurt_mem_cache_mode_t)cache_attribs) == 0) {
        QURT_LOG("Invalid cache attr! vaddr : 0x%x, paddr : 0x%x, cache_attr : 0x%x ", vaddr, paddr, cache_attribs);
        return QURT_EINVALID;
    }

    if (!is_valid_asid(asid)) {
        QURT_LOG("Invalid page asid! vaddr : 0x%x, paddr : 0x%x, size : 0x%x, asid : %d", vaddr, paddr, size,asid);
        return QURT_EINVALID;
    }

    INTLOCK_SAVE(int_save);
    
    pgtable = (word_t *)arm_mmu_pgtable[asid];
    /* Get the top-level pagetable entry for this virtual address. */
    entry_ptr = arm_pgindex_1m(pgtable, vaddr);
    entry = *entry_ptr;

    if (size == SIZE_16M) {
        word_t i, *tmp;

        tmp = entry_ptr;
        entry = arm_l1_gen_16mpage(paddr, vaddr, access, cache_attribs);
        for (i = 0UL; i < (1UL << (ARM_16M_BITS - ARM_1M_BITS)); i++) {
            //WCET_CONSTANT(1UL << (ARM_16M_BITS - ARM_1M_BITS));

            if (!is_arm_l1_valid(*tmp)) {
                INTFREE_RESTORE(int_save);
                QURT_LOG("Trying to Remap a non-existing mapping SIZE_16M ! vaddr : 0x%x, paddr : 0x%x, size : 0x%x ", vaddr, paddr, size);
                return QURT_EREMAP;
            }
            tmp += 1;
        }
        
        tmp = entry_ptr;
        for (i = 0UL; i < (1UL << (ARM_16M_BITS - ARM_1M_BITS)); i++) {
            //WCET_CONSTANT(1UL << (ARM_16M_BITS - ARM_1M_BITS));

            *tmp = entry;
            tmp += 1;
        }
        /* 16M mapping uses 16 entries (2 cache lines), clean second line now */
        cache_clean_dline_kernel(entry_ptr + 8);
    } else if (size == SIZE_1M) {
        if (!is_arm_l1_valid(entry)) {
            INTFREE_RESTORE(int_save);
            QURT_LOG("Trying to Remap a non-existing mapping SIZE_1M ! vaddr : 0x%x, paddr : 0x%x, size : 0x%x ", vaddr, paddr, size);
            return QURT_EREMAP;
        }
        entry = arm_l1_gen_1mpage(paddr, vaddr, access, cache_attribs);
        *entry_ptr = entry;
    } else {
        word_t *subtree, phys_tree;
        // word_t err = ERROR_OK;

        if (!is_arm_l1_valid(entry)) {
            if (arm_l2_pgtable_pool_initted == 0)
                arm_l2_pgtable_pool_init();
            /* Allocate memory for the subtree. */
            subtree = arm_block_allocate(&arm_l2_pgtable_pool);

            /* Now install the subtree into our top-level pagetable. */
            entry = arm_l1_gen_subtree(mmu_virt_to_phys(subtree), vaddr);
            *entry_ptr = entry;

            /* Clean cache line for hardware walker visibility. */
            cache_clean_dline_kernel(entry_ptr);
        } else if (!is_arm_l1_subtree(entry)) {
            INTFREE_RESTORE(int_save);
            QURT_LOG("Trying to Remap a non-existing mapping! vaddr : 0x%x, paddr : 0x%x, size : 0x%x ", vaddr, paddr, size);
            return QURT_EREMAP;
        }
        phys_tree = arm_l1_subtree_addr(entry);
        if (phys_tree == 0UL) {
            INTFREE_RESTORE(int_save);
            QURT_LOG("L1 Page Table entry creation failed! vaddr : 0x%x, paddr : 0x%x, size : 0x%x ", vaddr, paddr, size);
            return QURT_EMEM;
        }
        subtree = (word_t *)mmu_phys_to_virt(phys_tree);

        if (size == SIZE_64K) {
            word_t i, *tmp;
            entry_ptr = arm_pgindex_64k(subtree, vaddr);

            /* Handle duplicate entries for 64k */
            tmp = entry_ptr;
            for (i = 0UL; i < (1UL << (ARM_64K_BITS - ARM_4K_BITS)); i++) {
                //WCET_CONSTANT(1UL << (ARM_64K_BITS - ARM_4K_BITS));

                if (!is_arm_l2_valid(*tmp)) {
                    INTFREE_RESTORE(int_save);
                    QURT_LOG("Trying to Remap a non-existing mapping SIZE_64K ! vaddr : 0x%x, paddr : 0x%x, size : 0x%x ", vaddr, paddr, size);
                    return QURT_EREMAP;
                }
                tmp += 1;
            }
            entry = arm_l2_gen_64kpage(paddr, vaddr, access, cache_attribs);
            tmp = entry_ptr;
            for (i = 0UL; i < (1UL << (ARM_64K_BITS - ARM_4K_BITS)); i++) {
                //WCET_CONSTANT(1UL << (ARM_64K_BITS - ARM_4K_BITS));

                *tmp = entry;
                tmp += 1;
            }
            /* 64K mapping uses 16 entries (2 cache lines), clean second line now */
            cache_clean_dline_kernel(entry_ptr + 8);
        } else if (size == SIZE_4K) {
            entry_ptr = arm_pgindex_4k(subtree, vaddr);
            entry = *entry_ptr;

            if (!is_arm_l2_valid(entry)) {
                INTFREE_RESTORE(int_save);
                QURT_LOG("Trying to Remap a non-existing mapping SIZE_4K ! vaddr : 0x%x, paddr : 0x%x, size : 0x%x ", vaddr, paddr, size);
                return QURT_EREMAP;
            }
            entry = arm_l2_gen_4kpage(paddr, vaddr, access, cache_attribs);
            *entry_ptr = entry;
        } else {
            //panic("");
            INTFREE_RESTORE(int_save);
            QURT_LOG("Unhandled page size! vaddr : 0x%x, paddr : 0x%x, size : 0x%x ", vaddr, paddr, size);
            return QURT_EINVALID;

        }
    }

    /* Clean cache line for hardware walker visibility. */
    cache_clean_dline_kernel(entry_ptr);

    // cache_drain_writebuffer();
    __asm ( "dsb" );

    INTFREE_RESTORE(int_save);
    return QURT_EOK;
}

/* Copies kernel page table to user space.
 * Potential DANGER of overwriting any existing user space page table entries.
 */
int copy_pgtable_to_user (word_t asid) {
    
    word_t i;
    
    if (!is_active_asid(asid)) {
        QURT_LOG("Invalid page asid! size : 0x%x ", asid);
        return QURT_EINVALID;
    }

    for (i = 0; i < ARM_NUM_L1_PGTABLE_ENTRIES; i++) {
        arm_mmu_pgtable[asid][i] = arm_mmu_pgtable[0][i];
    }
    /* Clean the page table for hardware walker visibility. */
    cache_clean_drange_kernel(arm_mmu_pgtable[asid], ARM_NUM_L1_PGTABLE_ENTRIES*4);
    // cache_drain_writebuffer();
    __asm ( "dsb" );
    
    return QURT_EOK;
}

int clear_user_pgtable(word_t asid)
{
	word_t i;
	
	if (!is_active_asid(asid)) {
        return QURT_EFAILED;
    }
	
	/* zeroing out all addresses in the asid */
    for (i = 0; i < ARM_NUM_L1_PGTABLE_ENTRIES; i++) {
        arm_mmu_pgtable[asid][i] = 0;
    }
    /* Clean the page table for hardware walker visibility. */
    cache_clean_drange_kernel(arm_mmu_pgtable[asid], ARM_NUM_L1_PGTABLE_ENTRIES*4);
	
    __asm ( "dsb" );
    
    return QURT_EOK;
	
}
//#pragma pop
