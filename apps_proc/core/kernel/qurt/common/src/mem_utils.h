#ifndef MEM_UTILS_H
#define MEM_UTILS_H


#include "qurt_types.h"
#include "qurt_error.h"
#include "obj_cache.h"
#include "alloc.h"
#include "pgtable.h"
#include "qurt_memory.h"
#include "qurt_error.h"
#include "assert.h"
//#include "malloc.h"
#include "qurt_system.h"
#include "qurt_interrupt.h"
#include "memory.h"



#define QURTK_fatal(...) ASSERT(0)
#define QURTK_assert ASSERT
#define kprintf(...) 



/* Common definitions */
#define IS_ALIGNED(x, a) (((word_t)(x) & ((a) - 1UL)) == 0UL)
#define IS_POWER2_OR_ZERO(n) (((n) & ((n) - 1UL)) == 0UL)
#define IS_POWER2(n) (((n) != 0UL) && IS_POWER2_OR_ZERO(n))
#define ADD_WILL_OVERFLOW(a, b) ((a) > (~(b)))
#define MULTIPLY_WILL_OVERFLOW(a, b) \
    ((((uint64_t)(a)) * ((uint64_t)(b))) > (uint64_t)(~0UL))

#define ALIGN_UP(x, a) (((word_t)(x) + ((word_t)(a) - 1UL)) & \
                        ~((word_t)(a) - 1UL))
#define ALIGN_DOWN(x, a) ((word_t)(x) & ~((word_t)(a) - 1UL))
#define LEAST_POWER2(x) ((word_t)(x) & (word_t)(-(sword_t)(x)))
#define ALIGN_UP_TO_NEXT_POWER2(x) align_up_to_next_power2(x)
#define ARRAY_SIZE(exp) ((word_t)(sizeof(exp)/sizeof((exp)[0])))

FORCE_INLINE int qurt_clz (uint32 x) {
    int r;
    __asm { clz r, x }
    return r;
}

FORCE_INLINE int qurt_rbit (uint32 x) {
    int r;
    __asm { rbit r, x }
    return r;
}


FORCE_INLINE word_t align_up_to_next_power2 (word_t size) {
    if (!IS_POWER2(size)) {
        /* Avoid overflow */
        return 1UL << (32 - (word_t)qurt_clz(size));
    } else {
        return size;
    }
}

void QURTK_heap_init(void);
void * QURTK_heap_alloc(void);
void QURTK_heap_free( void * buf );


void * QURTK_lifo_pop( void *buf_head );
void QURTK_lifo_push( void *buf_head, void * buf );

//Remove an element from lifo.
//
//BE NOTED:!!! such operation has to be within some lock with lifo_push and
//lifo_pop
//
//void QURTK_lifo_remove( void *buf_head, void * buf );

INLINE void QURTK_lifo_remove( void *buf_head, void * buf ) {
    struct m {
        struct m * next;
    };
    typedef struct m lifo_node_t;

    lifo_node_t ** prev = (lifo_node_t **)buf_head;
    lifo_node_t *node = (lifo_node_t *)buf; 
    while( (NULL != *prev) && ((*prev) != node) ){
        prev = &((*prev)->next);
    }
    if( NULL == *prev ){
        QURTK_fatal(ABORT_LIFO_REMOVE_NON_EXIST_ITEM);
    }
    *prev = node->next;
}


INLINE qurt_perm_t QURTK_get_perm(unsigned long long mapping) {return (qurt_perm_t)((((uint32)mapping) >> 20) & 0xF);}
INLINE qurt_mem_cache_mode_t QURTK_get_cache_attr(unsigned long long mapping) {return (qurt_mem_cache_mode_t)((((uint32)mapping) >> 24) & 0x3F);}
INLINE uint32 QURTK_get_pgsize(unsigned long long mapping) {return (uint32)((mapping >> 30) & 0x3FFF); }
INLINE uint32 QURTK_get_vpn(unsigned long long mapping) {return (uint32)(mapping >> 44); }
INLINE uint32 QURTK_get_ppn(unsigned long long mapping) {return (uint32) (((uint32)mapping) & 0xFFFFF); }

#endif /* BUF_H */
