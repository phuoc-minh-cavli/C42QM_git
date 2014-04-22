
#include "pgtable.h"
#include "assert.h"
#include "malloc.h"
#include "qurt_interrupt.h"


int arm_block_pool_create (arm_block_pool * pool, word_t pool_start, word_t pool_size, word_t block_size) {
    
    word_t * next, *prev;
    word_t num_blocks;
    // arm_block_pool *pool;
    
    /* block size should be a power of 2 */
    ASSERT(IS_POWER2(block_size));
    /* pool size should be a multiple of block size */
    ASSERT(pool_size % block_size == 0);
    /* pool_start should be aligned to the block_size */
    ASSERT(IS_ALIGNED(pool_start, block_size));
    
    // pool = (arm_block_pool *)malloc(sizeof(struct arm_block_pool_type));
    ASSERT(pool != NULL);
    
    num_blocks = pool_size / block_size;
    next = (word_t *)pool_start;
    prev = NULL;
    
    while (num_blocks--) {
        *next = (word_t)prev;
        prev = next;
        next += block_size/sizeof(word_t);
    }
    
    pool->pool_start = pool_start;
    pool->next_free_block = prev;
    pool->pool_size = pool_size;
    pool->block_size = block_size;
    
    return QURT_EOK;
}


word_t * arm_block_allocate (arm_block_pool *pool) {
    
    word_t *next = pool->next_free_block;
    
    if (next) {
        pool->next_free_block = (word_t *)*next;
    }
    
    return next;
}


void arm_block_free (arm_block_pool *pool, word_t block_start) {
    
    word_t *prev, *block_addr;
    
    /* block address should be aligned to the block_size */
    ASSERT(IS_ALIGNED(block_start, pool->block_size));
    
    if ((block_start < pool->pool_start) || ((block_start + pool->block_size) > (pool->pool_start + pool->pool_size))) {
        /* block address is not within the pool range */
        ASSERT(0);
    }
    block_addr = (word_t *)block_start;
    prev = pool->next_free_block;
    *block_addr = (word_t)prev;
    pool->next_free_block = block_addr;
} 
