#ifndef _KERNEL_ARM_V7_MMU_PGTABLE_H_
#define _KERNEL_ARM_V7_MMU_PGTABLE_H_

#include "mem_utils.h"
#include "assert.h"

/*
#define SIZE_16M (1UL << 24)
#define SIZE_1M (1UL << 20)
#define SIZE_64K (1UL << 16)
#define SIZE_4K (1UL << 12)
*/
#define PAGE_SIZES (SIZE_16M | SIZE_1M | SIZE_64K | SIZE_4K)

#define VIRT_PHYS_OFFSET 0 /* since 1-1 mapping is assumed */

/* ARM v7 MMU access values */
/* (APX << 3) | (AP << 1) | XN */
#define ARM_ACCESS_KNONE 0UL
#define ARM_ACCESS_KRW_UNONE 3UL
#define ARM_ACCESS_KRWX_UNONE 2UL
#define ARM_ACCESS_KRW_UR 5UL
#define ARM_ACCESS_KRWX_URX 4UL
#define ARM_ACCESS_KRW_URW 7UL
#define ARM_ACCESS_KRWX_URWX 6UL
#define ARM_ACCESS_KR_UNONE 0xbUL
#define ARM_ACCESS_KRX_UNONE 0xaUL
#define ARM_ACCESS_KR_UR 0xfUL
#define ARM_ACCESS_KRX_URX 0xeUL

#define ARM_ACCESS_MAX 0xfUL
#define HW_CACHE_ATTR_MAX 0x3fUL
/* ARM Domains */
#define ARM_DOMAIN_MAX 15UL

/* Virtual memory indexes */
#define ARM_VIRT_BITS 32UL
#define ARM_VIRT_MASK 0xffffffffUL
#define ARM_16M_BITS 24UL
#define ARM_1M_BITS 20UL
#define ARM_64K_BITS 16UL
#define ARM_4K_BITS 12UL
#define ARM_ENTRY_BITS 2UL

#define ARM_TOPTREE_ENTRIES (1UL << (ARM_VIRT_BITS - ARM_1M_BITS))
#define ARM_SUBTREE_ENTRIES (1UL << (ARM_1M_BITS - ARM_4K_BITS))

#define ARM_TOPTREE_SIZE (ARM_TOPTREE_ENTRIES << ARM_ENTRY_BITS)
#define ARM_SUBTREE_SIZE (ARM_SUBTREE_ENTRIES << ARM_ENTRY_BITS)

#define ARM_SUPPORTED_PAGE_BITS (SIZE_16M | SIZE_1M | SIZE_64K | SIZE_4K)

#define ARM_PGENT_8BIT_MASK 0xff000000UL
#define ARM_PGENT_12BIT_MASK 0xfff00000UL
#define ARM_PGENT_20BIT_MASK 0xfffff000UL

/* FIXME: Jira ticket OLD-1383 - carl. */

/* ARM 1M Level PageTable Entries */
#define ARM_L1_TYPE(x) ((x) & 3UL)
#define ARM_L1_TYPE_INVALID 0UL
#define ARM_L1_TYPE_CTABLE 1UL /* Coarse table (64+4K pages) */
#define ARM_L1_TYPE_SECTION 2UL /* Valid 1M or 16M page */
#define ARM_L1_TYPE_NONE 3UL /* Reserved */
#define ARM_L1_SUBTYPE(x) ((x) & 0x40000UL)
#define ARM_L1_SUBTYPE_16MPAGE 0x40000UL /* Valid 16M page */
#define ARM_L1_SUBTYPE_1MPAGE 0UL /* Valid 1M page */

#define ARM_L1_CTABLE_BASE(x) \
    (((x) >> 10) << 10)
#define ARM_L1_CTABLE_DOMAIN(x) \
    (((x) >> 5) & 0xfUL)

#define ARM_L1_CTABLE_GEN_BASE(x) (x)
#define ARM_L1_CTABLE_GEN_DOMAIN(x) ((x) << 5)

#define ARM_L1_16MPAGE_PHYS(x) \
    (((x) >> 20) << 20)
#define ARM_L1_16MPAGE_ATTRIBS(x) \
    ((((x) >> 1) & 6UL) | ((x >> 9) & 0x38UL) | ((x >> 16) & 1UL))
#define ARM_L1_16MPAGE_ACCESS(x) \
    ((((x) >> 9) & 6UL) | ((x >> 4) & 1UL) | ((x >> 12) & 8UL))
#define ARM_L1_16MPAGE_NOTGLOBAL(x) \
    ((x >> 17) & 1UL)

#define ARM_L1_16MPAGE_GEN_PHYS(x) \
    (x)
#define ARM_L1_16MPAGE_GEN_ATTRIBS(x) \
    (((x & 6UL) << 1) | ((x & 0x38UL) << 9) | ((x & 1UL) << 16))
#define ARM_L1_16MPAGE_GEN_ACCESS(x) \
    (((x & 6UL) << 9) | ((x & 1UL) << 4) | ((x & 8UL) << 12))
#define ARM_L1_16MPAGE_GEN_NOTGLOBAL(x) \
    ((x & 1UL) << 17)

#define ARM_L1_1MPAGE_PHYS(x) \
    (((x) >> 20) << 20)
#define ARM_L1_1MPAGE_ATTRIBS(x) \
    ((((x) >> 1) & 6UL) | ((x >> 9) & 0x38UL) | ((x >> 16) & 1UL))
#define ARM_L1_1MPAGE_DOMAIN(x) \
    (((x) >> 5) & 0xfUL)
#define ARM_L1_1MPAGE_ACCESS(x) \
    ((((x) >> 9) & 6UL) | ((x >> 4) & 1UL) | ((x >> 12) & 8UL))
#define ARM_L1_1MPAGE_NOTGLOBAL(x) \
    ((x >> 17) & 1UL)

#define ARM_L1_1MPAGE_GEN_PHYS(x) \
    (x)
#define ARM_L1_1MPAGE_GEN_ATTRIBS(x) \
    (((x & 6UL) << 1) | ((x & 0x38UL) << 9) | ((x & 1UL) << 16))
#define ARM_L1_1MPAGE_GEN_DOMAIN(x) \
    ((x) << 5)
#define ARM_L1_1MPAGE_GEN_ACCESS(x) \
    (((x & 6UL) << 9) | ((x & 1UL) << 4) | ((x & 8UL) << 12))
#define ARM_L1_1MPAGE_GEN_NOTGLOBAL(x) \
    ((x & 1UL) << 17)

/* ARM L2 Level PageTable Entries */
#define ARM_L2_TYPE(x) ((((x) & 3UL) > 1UL) ? 2UL : ((x) & 3UL))
#define ARM_L2_TYPE_INVALID 0UL
#define ARM_L2_TYPE_64KPAGE 1UL /* Valid 64K page */
#define ARM_L2_TYPE_4KPAGE 2UL /* Valid 4K page */

#define ARM_L2_4KPAGE_PHYS(x) \
    (((x) >> 12) << 12)
#define ARM_L2_4KPAGE_ATTRIBS(x) \
    ((((x) >> 1) & 6UL) | ((x >> 3) & 0x38UL) | ((x >> 10) & 1UL))
#define ARM_L2_4KPAGE_ACCESS(x) \
    (((x >> 3) & 6UL) | (x & 1UL) | ((x >> 6) & 8UL))
#define ARM_L2_4KPAGE_NOTGLOBAL(x) \
    ((x >> 11) & 1UL)

#define ARM_L2_64KPAGE_PHYS(x) \
    (((x) >> 16) << 16)
#define ARM_L2_64KPAGE_ATTRIBS(x) \
    ((((x) >> 1) & 6UL) | ((x >> 9) & 0x38UL) | ((x >> 10) & 1UL))
#define ARM_L2_64KPAGE_ACCESS(x) \
    (((x >> 3) & 6UL) | ((x >> 15) & 1UL) | ((x >> 6) & 8UL))
#define ARM_L2_64KPAGE_NOTGLOBAL(x) \
    ((x >> 11) & 1UL)

#define ARM_L2_4KPAGE_GEN_PHYS(x) \
    (x)
#define ARM_L2_4KPAGE_GEN_ATTRIBS(x) \
    (((x & 6UL) << 1) | ((x & 0x38UL) << 3) | ((x & 1UL) << 10))
#define ARM_L2_4KPAGE_GEN_ACCESS(x) \
    (((x & 6UL) << 3) | (x & 1UL) | ((x & 8UL) << 6))
#define ARM_L2_4KPAGE_GEN_NOTGLOBAL(x) \
    ((x & 1UL) << 11)

#define ARM_L2_64KPAGE_GEN_PHYS(x) \
    (x)
#define ARM_L2_64KPAGE_GEN_ATTRIBS(x) \
    (((x & 6UL) << 1) | ((x & 0x38UL) << 9) | ((x & 1UL) << 10))
#define ARM_L2_64KPAGE_GEN_ACCESS(x) \
    (((x & 6UL) << 3) | ((x & 1UL) << 15) | ((x & 8UL) << 6))
#define ARM_L2_64KPAGE_GEN_NOTGLOBAL(x) \
    ((x & 1UL) << 11)


/* Page table indexing functions */

INLINE void * arm_pgindex_16m(void *tree, word_t addr) {
    word_t base = (word_t)tree;
    /* get index */
    addr = (addr >> ARM_16M_BITS);
    addr = addr << ARM_ENTRY_BITS;

    return (void *)(base + addr);
}

INLINE void * arm_pgindex_1m(void *tree, word_t addr) {
    word_t base = (word_t)tree;
    /* get index */
    addr = (addr >> ARM_1M_BITS);
    addr = addr << ARM_ENTRY_BITS;

    return (void *)(base + addr);
}

INLINE void * arm_pgindex_64k(void *tree, word_t addr) {
    word_t base = (word_t)tree;

    /* mask off top bits */
    addr = (addr << (ARM_VIRT_BITS - ARM_1M_BITS));
    addr = (addr >> (ARM_VIRT_BITS - ARM_1M_BITS));
    /* get index */
    addr = (addr >> ARM_64K_BITS);
    addr = addr << (ARM_ENTRY_BITS + (ARM_64K_BITS - ARM_4K_BITS));

    return (void *)(base + addr);
}

INLINE void * arm_pgindex_4k(void *tree, word_t addr) {
    word_t base = (word_t)tree;

    /* mask off top bits */
    addr = (addr << (ARM_VIRT_BITS - ARM_1M_BITS));
    addr = (addr >> (ARM_VIRT_BITS - ARM_1M_BITS));
    /* get index */
    addr = (addr >> ARM_4K_BITS);
    addr = addr << ARM_ENTRY_BITS;

    return (void *)(base + addr);
}

/* ARM PageTable 1M level accessor functions */

INLINE int is_arm_l1_valid(word_t pgentry) {
    return (ARM_L1_TYPE(pgentry) != ARM_L1_TYPE_INVALID) &&
            (ARM_L1_TYPE(pgentry) != ARM_L1_TYPE_NONE);
}

INLINE int is_arm_l1_subtree(word_t pgentry) {
    return ARM_L1_TYPE(pgentry) == ARM_L1_TYPE_CTABLE;
}

INLINE int is_arm_l1_1mpage(word_t pgentry) {
    return (ARM_L1_TYPE(pgentry) == ARM_L1_TYPE_SECTION) &&
            (ARM_L1_SUBTYPE(pgentry) == ARM_L1_SUBTYPE_1MPAGE);
}

INLINE int is_arm_l1_16mpage(word_t pgentry) {
    return (ARM_L1_TYPE(pgentry) == ARM_L1_TYPE_SECTION) &&
            (ARM_L1_SUBTYPE(pgentry) == ARM_L1_SUBTYPE_16MPAGE);
}

INLINE word_t arm_l1_subtree_addr(word_t pgentry) {
    return ARM_L1_CTABLE_BASE(pgentry);
}

INLINE word_t arm_l1_subtree_domain(word_t pgentry) {
    return ARM_L1_CTABLE_DOMAIN(pgentry);
}

INLINE word_t arm_l1_16mpage_attribs(word_t pgentry) {
    return (word_t)ARM_L1_16MPAGE_ATTRIBS(pgentry);
}

/*
INLINE string_t arm_generic_attribs_str(word_t attribs) {
    switch (attribs) {
    case HW_CACHE_ATTR_STRONG:
        return "ST";
    case HW_CACHE_ATTR_WRITETHROUGH:
        return "WT";
    case HW_CACHE_ATTR_WRITETHROUGH_SHARED:
        return "WT-S";
    case HW_CACHE_ATTR_WRITEBACK:
        return "WB";
    case HW_CACHE_ATTR_WRITEBACK_SHARED:
        return "WB-S";
    case HW_CACHE_ATTR_WRITEBACK_ALLOC:
        return "WBa";
    case HW_CACHE_ATTR_WRITEBACK_ALLOC_SHARED:
        return "WBa-S";
    case HW_CACHE_ATTR_UNCACHED:
        return "NC";
    case HW_CACHE_ATTR_UNCACHED_SHARED:
        return "NC-S";
    case HW_CACHE_ATTR_DEVICE:
        return "IO";
    case HW_CACHE_ATTR_DEVICE_SHARED:
        return "IO-S";
    default:
        return "??";
    }
}

INLINE string_t arm_l1_16mpage_attribs_str(word_t pgentry) {
    word_t attribs = arm_l1_16mpage_attribs(pgentry);
    return arm_generic_attribs_str(attribs);
}
*/

INLINE word_t arm_l1_16mpage_access(word_t pgentry) {
    return ARM_L1_16MPAGE_ACCESS(pgentry);
}
/*
INLINE string_t arm_generic_access_str(word_t access) {
    switch (access) {
    case ARM_ACCESS_KNONE:
        return "NONE";
    case ARM_ACCESS_KRW_UNONE:
        return "KRW_UNONE";
    case ARM_ACCESS_KRWX_UNONE:
        return "KRWX_UNONE";
    case ARM_ACCESS_KRW_UR:
        return "KRW_UR";
    case ARM_ACCESS_KRWX_URX:
        return "KRWX_URX";
    case ARM_ACCESS_KRW_URW:
        return "KRW_URW";
    case ARM_ACCESS_KRWX_URWX:
        return "KRWX_URWX";
    case ARM_ACCESS_KR_UNONE:
        return "KR_UNONE";
    case ARM_ACCESS_KRX_UNONE:
        return "KRX_UNONE";
    case ARM_ACCESS_KR_UR:
        return "KR_UR";
    case ARM_ACCESS_KRX_URX:
        return "KRX_URX";
    default:
        return "????";
    }
}

INLINE string_t arm_l1_16mpage_access_str(word_t pgentry) {
    word_t access = arm_l1_16mpage_access(pgentry);
    return arm_generic_access_str(access);
}

*/

INLINE word_t  arm_l1_16mpage_phys(word_t pgentry) {
    return ARM_L1_16MPAGE_PHYS(pgentry);
}

INLINE int arm_l1_16mpage_not_global(word_t pgentry) {
    return ARM_L1_16MPAGE_NOTGLOBAL(pgentry) != 0UL;
}

/* end of 16MB pages */

INLINE word_t arm_l1_1mpage_attribs(word_t pgentry) {
    return (word_t)ARM_L1_1MPAGE_ATTRIBS(pgentry);
}
/* 
INLINE string_t arm_l1_1mpage_attribs_str(word_t pgentry) {
    word_t attribs = arm_l1_1mpage_attribs(pgentry);
    return arm_generic_attribs_str(attribs);
}
 */
INLINE word_t arm_l1_1mpage_access(word_t pgentry) {
    return ARM_L1_1MPAGE_ACCESS(pgentry);
}
/* 
INLINE string_t arm_l1_1mpage_access_str(word_t pgentry) {
    word_t access = arm_l1_1mpage_access(pgentry);
    return arm_generic_access_str(access);
}
 */
INLINE word_t arm_l1_1mpage_domain(word_t pgentry) {
    return ARM_L1_1MPAGE_DOMAIN(pgentry);
}

INLINE word_t arm_l1_1mpage_phys(word_t pgentry) {
    return ARM_L1_1MPAGE_PHYS(pgentry);
}

INLINE int arm_l1_1mpage_not_global(word_t pgentry) {
    return ARM_L1_1MPAGE_NOTGLOBAL(pgentry) != 0UL;
}

/* ARM PageTable 4K/64k level accessor functions */

INLINE int is_arm_l2_valid(word_t pgentry) {
    return ARM_L2_TYPE(pgentry) != ARM_L2_TYPE_INVALID;
}

INLINE int is_arm_l2_4kpage(word_t pgentry) {
    return ARM_L2_TYPE(pgentry) == ARM_L2_TYPE_4KPAGE;
}

INLINE int is_arm_l2_64kpage(word_t pgentry) {
    return ARM_L2_TYPE(pgentry) == ARM_L2_TYPE_64KPAGE;
}

INLINE word_t arm_l2_4kpage_attribs(word_t pgentry) {
    return (word_t)ARM_L2_4KPAGE_ATTRIBS(pgentry);
}
/* 
INLINE string_t arm_l2_4kpage_attribs_str(word_t pgentry) {
    word_t attribs = arm_l2_4kpage_attribs(pgentry);
    return arm_generic_attribs_str(attribs);
} */

INLINE word_t arm_l2_4kpage_access(word_t pgentry) {
    return ARM_L2_4KPAGE_ACCESS(pgentry);
}
/* 
INLINE string_t arm_l2_4kpage_access_str(word_t pgentry) {
    word_t access = arm_l2_4kpage_access(pgentry);
    return arm_generic_access_str(access);
} */

INLINE word_t arm_l2_4kpage_phys(word_t pgentry) {
    return ARM_L2_4KPAGE_PHYS(pgentry);
}

INLINE int arm_l2_4kpage_not_global(word_t pgentry) {
    return ARM_L2_4KPAGE_NOTGLOBAL(pgentry) != 0UL;
}

INLINE word_t arm_l2_64kpage_attribs(word_t pgentry) {
    return (word_t)ARM_L2_64KPAGE_ATTRIBS(pgentry);
}

/* INLINE string_t arm_l2_64kpage_attribs_str(word_t pgentry) {
    word_t attribs = arm_l2_64kpage_attribs(pgentry);
    return arm_generic_attribs_str(attribs);
} */

INLINE word_t arm_l2_64kpage_access(word_t pgentry) {
    return ARM_L2_64KPAGE_ACCESS(pgentry);
}
/* 
INLINE string_t arm_l2_64kpage_access_str(word_t pgentry) {
    word_t access = arm_l2_64kpage_access(pgentry);
    return arm_generic_access_str(access);
} */

INLINE word_t arm_l2_64kpage_phys(word_t pgentry) {
    return ARM_L2_64KPAGE_PHYS(pgentry);
}

INLINE int arm_l2_64kpage_not_global(word_t pgentry) {
    return ARM_L2_64KPAGE_NOTGLOBAL(pgentry) != 0UL;
}

word_t mmu_get_generic_rwx(word_t access);

/* 
INLINE void arm_l1_free_subtree(struct domain *domain, word_t virt,  word_t *subtree) {
    objmanager_free(domain, subtree, ARM_SUBTREE_SIZE);
}
 */
INLINE word_t arm_l1_gen_subtree(word_t phys_tree, word_t virt) {
    word_t entry = ARM_L1_TYPE_CTABLE;
    ASSERT((phys_tree & (ARM_SUBTREE_SIZE - 1UL)) == 0UL);

    entry |= (ARM_L1_CTABLE_GEN_DOMAIN(0UL) |
            ARM_L1_CTABLE_GEN_BASE(phys_tree));

    return entry;
}

INLINE word_t arm_l1_gen_1mpage(word_t phys, word_t virt, word_t rights, word_t attribs) {
    word_t entry = (ARM_L1_TYPE_SECTION | ARM_L1_SUBTYPE_1MPAGE);
    ASSERT(rights <= ARM_ACCESS_MAX);
    ASSERT(attribs <= HW_CACHE_ATTR_MAX);
    ASSERT((phys & (SIZE_1M - 1UL)) == 0UL);

    entry |= (ARM_L1_1MPAGE_GEN_ACCESS(rights) | ARM_L1_1MPAGE_GEN_DOMAIN(0UL) |
        ARM_L1_1MPAGE_GEN_ATTRIBS((word_t)attribs) |
        ARM_L1_1MPAGE_GEN_PHYS(phys) | ARM_L1_1MPAGE_GEN_NOTGLOBAL(1UL));

    return entry;
}

INLINE word_t arm_l1_gen_16mpage(word_t phys, word_t virt, word_t rights,
                                        word_t attribs) {
    word_t entry = (ARM_L1_TYPE_SECTION | ARM_L1_SUBTYPE_16MPAGE);
    ASSERT(rights <= ARM_ACCESS_MAX);
    ASSERT(attribs <= HW_CACHE_ATTR_MAX);
    ASSERT((phys & (SIZE_16M - 1UL)) == 0UL);

    entry |= (ARM_L1_16MPAGE_GEN_ACCESS(rights) |
        ARM_L1_16MPAGE_GEN_ATTRIBS((word_t)attribs) |
        ARM_L1_16MPAGE_GEN_PHYS(phys) | ARM_L1_16MPAGE_GEN_NOTGLOBAL(1UL));

    return entry;
}

INLINE word_t arm_l1_gen_invalid() {
    return (word_t)ARM_L1_TYPE_INVALID;
}

INLINE word_t arm_l2_gen_64kpage(word_t phys, word_t virt,
        word_t rights, word_t attribs) {
    word_t entry = ARM_L2_TYPE_64KPAGE;
    ASSERT(rights <= ARM_ACCESS_MAX);
    ASSERT(attribs <= HW_CACHE_ATTR_MAX);
    ASSERT((phys & (SIZE_4K - 1UL)) == 0UL);

    entry |= (ARM_L2_64KPAGE_GEN_ACCESS(rights) |
        ARM_L2_64KPAGE_GEN_ATTRIBS((word_t)attribs) |
        ARM_L2_64KPAGE_GEN_PHYS(phys) | ARM_L2_64KPAGE_GEN_NOTGLOBAL(1UL));

    return entry;
}

INLINE word_t arm_l2_gen_4kpage(word_t phys, word_t virt,
        word_t rights, word_t attribs) {
    word_t entry = ARM_L2_TYPE_4KPAGE;
    ASSERT(rights <= ARM_ACCESS_MAX);
    ASSERT(attribs <= HW_CACHE_ATTR_MAX);
    ASSERT((phys & (SIZE_4K - 1UL)) == 0UL);

    entry |= (ARM_L2_4KPAGE_GEN_ACCESS(rights) |
        ARM_L2_4KPAGE_GEN_ATTRIBS((word_t)attribs) |
        ARM_L2_4KPAGE_GEN_PHYS(phys) | ARM_L2_4KPAGE_GEN_NOTGLOBAL(1UL));

    return entry;
}

INLINE word_t arm_l2_gen_invalid() {
    return (word_t)ARM_L2_TYPE_INVALID;
}

INLINE word_t page_size_check(word_t size) {
    return size & ARM_SUPPORTED_PAGE_BITS;
}



INLINE word_t mmu_virt_to_phys(void *addr) {
    return (word_t)addr - VIRT_PHYS_OFFSET;
}

INLINE void * mmu_phys_to_virt(word_t addr) {
    return (void *)(addr + VIRT_PHYS_OFFSET);
}




INLINE void tlb_flush(void)
{
    /* Set the control register */
   //  write_cp15_register(C15_tlb, c7, 0, 0UL);
    __asm
    {
        mcr  p15, 0, 0UL, c8, c7, 0
    }
}


INLINE void tlb_flush_asid(word_t asid)
{
    word_t val;
    ASSERT(asid <= ARM_DOMAIN_MAX);

    val = asid;

    // write_cp15_register(C15_tlb, c7, 2, val);
    __asm
    {
        mcr p15, 0, val, c8, c7, 2
    }
    
}

INLINE void tlb_flush_addr_asid(word_t addr, word_t asid)
{
    word_t val;
    ASSERT(asid <= ARM_DOMAIN_MAX);

    val = asid;
    val |= (addr & 0xfffffc00UL);

    //write_cp15_register(C15_tlb, c7, 1, val);
    __asm
    {
        mcr p15, 0, val, c8, c7, 1
    }
}

/*

#define ASM_TLB_FLUSH_ALL(reg) \
    mov reg, #0; \
    mcr p15, 0, reg, c8, c7, 0;

INLINE void write_arm_ttbase0(word_t val)
{
    write_cp15_register(C15_ttbase, C15_CRm_default, 0,
                        val | TTBASE_FLAGS);
}

INLINE void write_arm_ttbase1(word_t val)
{
    write_cp15_register(C15_ttbase, C15_CRm_default, 1,
                        val | TTBASE_FLAGS);
}
*/

typedef struct arm_block_pool_type {
    word_t pool_start;
    word_t * next_free_block;
    word_t pool_size;
    word_t block_size;
    
} arm_block_pool;

int arm_block_pool_create (arm_block_pool * pool, word_t pool_start, word_t pool_size, word_t block_size);

word_t * arm_block_allocate (arm_block_pool *pool);

void arm_block_free (arm_block_pool *pool, word_t block_start);

#endif /* _KERNEL_ARM_V7_MMU_PGTABLE_H_ */
