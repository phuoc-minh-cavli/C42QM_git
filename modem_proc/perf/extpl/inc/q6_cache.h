#pragma once
#include <hexagon_protos.h>
#include <stdint.h>
/** @file q6_cache
    @details Q6 cache operations */

#if !defined( L2_FETCH )
#define L2_FETCH
#endif

#if !defined( DC_ZERO )
#define DC_ZERO
#endif
#define CACHE_LINE_SHIFT (5)
#define CACHE_LINE_SZ  (1 << CACHE_LINE_SHIFT)

static inline void l2fetch_buffer
(
  void *addr,  /*!< (Virtual) address to start fetch */
  uint32_t len   /*!< Length (in bytes) to fetch. */
)
{
#if defined( L2_FETCH )
  /* Cache-align starting address and length. */
  uint32_t ofs = ((uint32_t) addr) & (CACHE_LINE_SZ-1);
  addr = (void *) ((uint32_t) addr - ofs);
  len  = (len+ofs+CACHE_LINE_SZ-1) / CACHE_LINE_SZ;

  /* Width=cache line, height=# cache lines, stride=cache line */
  asm volatile ("l2fetch(%[addr],%[dim])" : : 
     [addr] "r" (addr), 
     [dim] "r" ( Q6_P_combine_IR(CACHE_LINE_SZ, Q6_R_combine_RlRl(CACHE_LINE_SZ, len)) )
     : "memory");
#endif
} /* l2fetch_buffer() */

/* DCZERO the output buffer, 32 bytes at a time.
   Assume page size is a multiple of 32
   Assume dest addr is multiple of 32
*/
void dczero(uint32_t addr, uint32_t size);
