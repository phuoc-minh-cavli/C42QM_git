

.text
.align 2

    GCC_ASM_EXPORT (boot_enable_aarch64)
    GCC_ASM_EXPORT (boot_generate_warm_reset)
    GCC_ASM_EXPORT (boot_configure_vbar)
   .global  dcache_inval_region
   .global  dcache_flush_region
   .global dcache_clean_region
   .global  mmu_flush_cache
   .global  mmu_invalidate_tlb
   .global  boot_disable_mmu
   .global  boot_enable_mmu
   .global  mmu_relocate_page_table_base
   .global  mmu_get_page_table_base
   .global  mmu_set_dacr
   .global  memory_barrier
   .global  boot_is_icache_enabled
   .global  boot_is_dcache_enabled
   .global  cache_mmu_init_page_table
   .global  cache_mmu_disable
   .global  mmu_enable_i_and_d_cache
   .global  mmu_invalidate_instruction_cache
   .global  mmu_enable_instruction_cache

//============================================================================
//                             MODULE IMPORTS
//============================================================================
#ifdef BOOT_USES_L2_DCACHE     
   .extern  l2_cache_flush
   .extern  l2_ranged_cache_invalidate
   .extern  l2_ranged_cache_flush
   .extern  l2_cache_flush_and_disable
#endif      
   
boot_enable_aarch64:
    mrc p15, 0, r0, c12, c0, 2 
    orr r0, r0, #0x1  
    dsb
    mcr p15, 0, r0, c12, c0, 2
    isb
    dsb
    bx lr

	
boot_generate_warm_reset:
    mrc p15, 0, r1, c12, c0, 2
    orr r1, r1, #2    
    dsb 
    mcr p15, 0, r1, c12, c0, 2
    isb
    dsb
    isb
    wfi
generate_warm_reset_loop:
    b generate_warm_reset_loop

boot_configure_vbar:
    mcr p15, 0, r0, c12, c0, 0  
    bx  lr

dcache_clean_all:
    dsb                      // ensure ordering with previous memory accesses
    mrc  p15, 1, r0, c0, c0, 1    // read clidr
    ands  r3, r0, #0x7000000    // extract loc from clidr
    mov  r3, r3, lsr #23          // left align loc bit field
    beq  finished              // if loc is 0, then no need to clean
    mov  r10, #0                // start clean at cache level 0
    loop1:
    add  r2, r10, r10, lsr #1    // work out 3x current cache level
    mov  r1, r0, lsr r2          // extract cache type bits from clidr
    and  r1, r1, #7              // mask of the bits for current cache only
    cmp  r1, #2                // see what cache we have at this level
    blt  skip                // skip if no cache, or just i-cache
    mcr  p15, 2, r10, c0, c0, 0    // select current cache level in cssr
    isb                      // isb to sych the new cssr&csidr
    mrc  p15, 1, r1, c0, c0, 0    // read the new csidr
    and  r2, r1, #7              // extract the length of the cache lines
    add  r2, r2, #4              // add 4 (line length offset)
    ldr  r4, =0x3ff
    ands  r4, r4, r1, lsr #3    // find maximum number on the way size
    clz  r5, r4                // find bit position of way size increment
    ldr  r7, =0x7fff
    ands  r7, r7, r1, lsr #13    // extract max number of the index size
    loop2:
    mov  r9, r4                // create working copy of max way size
    loop3:
    orr  r11, r10, r9, lsl r5      // factor way and cache number into r11
    orr  r11, r11, r7, lsl r2      // factor index number into r11
    mcr  p15, 0, r11, c7, c14, 2    // clean & invalidate by set/way
    subs  r9, r9, #1          // decrement the way
    bge  loop3
    subs  r7, r7, #1          // decrement the index
    bge  loop2
    skip:
    add  r10, r10, #2          // increment cache number
    cmp  r3, r10
    bgt  loop1
    finished:
    mov  r10, #0                // swith back to cache level 0
    mcr  p15, 2, r10, c0, c0, 0    // select current cache level in cssr
    dsb
    isb
    bx lr
	

dcache_clean_region:
  stmfd sp!, {r0-r12, lr} 
  mov r4, r0                      // copy addr
  mov r5, r1                      // copy size

  // If the size is invalid, then there is no clean required
  cmp r1, #0
  ble clean_done

  add r1, r0, r1                  // get the end address  
  sub r1, r1, #1                  // Adjust the end address to correct value.
  
  //dcache_line_size r2, r3         // r2 holds cache line size
  
  mrc p15, 0, r3, c0, c0, 1 // read CTR register
  // read CTR bits 16 through 19 for DMINLINE
  mov   r3, r3, lsr #16     // 
  ands  r3, r3, #0xF      // cache line size encoding, 4 bits.
  moveq r2, #0             // for the unlikely event that DMINLINE is 0
  movne r2, #4             // size offset. (DMINLINE is log2 words)
  mov   r2, r2, lsl r3  // actual cache line size in $reg. 64 bytes 
                              // for Krait and 32 bytes for Scorpion.
  sub r3, r2, #1                  // r3 holds alignment mask
  
    
  // Get the cache line aligned start address
  bic r0, r0, r3                  // r0 holds cache aligned start address
  
  // Get the cache line aligned end address and clean the line	
  bic r1, r1, r3                 // r1 holds cache aligned end address
  mcr p15, 0, r1, c7, c14, 1    // clean D / U line	
  
  // Execute the loop until we have cleaned all the lines between start and end
clean_loop:
  mcr p15, 0, r0, c7, c14, 1 // clean D / U line
  add r0, r0, r2
  cmp r0, r1
  bcs clean_done 
  b clean_loop
  
clean_done: 
  dsb                             // ensure all memory operations are complete

#ifdef BOOT_USES_L2_DCACHE   	
  // L2 cache clean
  mov r0, r4                      // copy addr to r0
  mov r1, r5                      // copy size to r1
  bl l2_ranged_cache_clean	
#endif

  dsb
  isb
  ldmfd sp!, {r0-r12, pc} 	
	
dcache_inval_region:
    stmfd  sp!, {r0-r12, lr}  
    mov r4, r0                      // copy start addr
    mov r5, r1                      // copy size	

    // If the size is invalid, then there is no invalidation required
    cmp r1, #0
    ble inval_done

    add r1, r0, r1                  // get the end address  
    sub r1, r1, #1                  // Adjust the end address to correct value.
    mov r6, r1                      // copy end address
  
    //dcache_line_size r2, r3         // r2 holds cache line size
    mrc p15, 0, r3, c0, c0, 1 // read CTR register
    // read CTR bits 16 through 19 for DMINLINE
    mov   r3, r3, lsr #16      
    ands  r3, r3, #0xF      // cache line size encoding, 4 bits.
    moveq r2, #0             // for the unlikely event that DMINLINE is 0
    movne r2, #4             // size offset. (DMINLINE is log2 words)
    mov   r2, r2, lsl r3  // actual cache line size in $reg. 64 bytes 
                              // for Krait and 32 bytes for Scorpion.
	sub r3, r2, #1                  // r3 holds alignment mask

    // Get the cache line aligned start address and flush the line	
    bic r0, r0, r3                  // r0 holds cache aligned start address
    cmp r0, r4                      // compare if start address is already cache line aligned
    mcrne p15, 0, r0, c7, c14, 1 // clean & invalidate D / U line only if not cache aligned
  
    // Get the cache line aligned end address and flush the line		
    bic r1, r1, r3                  // r1 holds cache aligned end address
    cmp r1, r6                      // compare if end address is already cache line aligned
    mcrne p15, 0, r1, c7, c14, 1 // clean & invalidate D / U line only if not cache aligned
    mcreq p15, 0, r1, c7, c6, 1  // invalidate D / U line if cache aligned
  
    // Execute the loop until we have invalidated all the lines between start and end	
    inval_loop:
    mcr p15, 0, r0, c7, c6, 1    // invalidate D / U line
    add r0, r0, r2
    cmp r0, r1
    bcs inval_done
    b inval_loop
  
    inval_done:	
    dsb                             // ensure all memory operations are complete

#ifdef BOOT_USES_L2_DCACHE   	
    // L2 cache inval
    mov r0, r4                       // copy addr to r0
    mov r1, r5                      // copy size to r1
    bl l2_ranged_cache_invalidate	
#endif

    dsb
    isb
    ldmfd sp!, {r0-r12, pc}

	
dcache_flush_region:
  stmfd  sp!, {r0-r12, lr}  
  mov r4, r0                      // copy addr
  mov r5, r1                      // copy size

  // If the size is invalid, then there is no flushing required
  cmp r1, #0
  ble flush_done
  
  add r1, r0, r1                  // get the end address
  sub r1, r1, #1                  // Adjust the end address to correct value.

  //dcache_line_size r2, r3         // r2 holds cache line size
  mrc p15, 0, r3, c0, c0, 1 // read CTR register
  // read CTR bits 16 through 19 for DMINLINE
  mov   r3, r3, lsr #16      
  ands  r3, r3, #0xF      // cache line size encoding, 4 bits.
  moveq r2, #0             // for the unlikely event that DMINLINE is 0
  movne r2, #4             // size offset. (DMINLINE is log2 words)
  mov   r2, r2, lsl r3  // actual cache line size in $reg. 64 bytes 
                            // for Krait and 32 bytes for Scorpion.
  sub r3, r2, #1                  // r3 holds alignment mask
  
  // Get the cache line aligned start address
  bic r0, r0, r3                  // r0 holds cache aligned start address
  
  // Get the cache line aligned end address and flush the line 
  bic r1, r1, r3                // r1 holds cache aligned end address
  mcr p15, 0, r1, c7, c14, 1   // clean & invalidate D / U line 
  
  // Execute the loop until we have flushed all the lines between start and end	
flush_1oop:
  mcr  p15, 0, r0, c7, c14, 1    // clean & invalidate D / U line
  add  r0, r0, r2
  cmp  r0, r1
  bcs flush_done
  b flush_1oop
  
flush_done:
  dsb                             // ensure all memory operations are complete

#ifdef BOOT_USES_L2_DCACHE   	
  // L2 cache flush
  mov r0, r4                      // copy addr to r0
  mov r1, r5                      // copy size to r1
  bl l2_ranged_cache_flush
#endif

  dsb
  isb
  ldmfd sp!, {r0-r12, pc}	
	


mmu_flush_cache:
  stmfd sp!, {r0-r12, lr} 
  // Call memory barrier operations to ensure completion of all cache 
  // maintenance & branch predictor operations appearing in program 
  // order, before these instructions 
  dsb
  isb
  bl  dcache_clean_all
  mov r0, #0
  mcr p15, 0, r0, c7, c5, 0  // I+BTB cache invalidate
  dsb
  isb

#ifdef BOOT_USES_L2_DCACHE      
  // Flush L2 cache
  bl  l2_cache_flush
  dsb
  isb
#endif

  ldmfd sp!, {r0-r12, pc}

mmu_invalidate_tlb:
   // Call memory barrier operations to ensure completion of all cache 
   // maintenance & branch predictor operations appearing in program 
   // order, before these instructions 
   dsb
   isb
   mov     r0 , #0
   mcr     p15, 0, r0, c8, c7, 0
   dsb
   isb
   bx lr 

 
boot_enable_mmu:
   // Call memory barrier operations to ensure completion of all cache 
   // maintenance & branch predictor operations appearing in program 
   // order, before these instructions 
   dsb
   isb
   mrc     p15, 0, r0, c1, c0, 0      // Read SCTLR, Control Register
   orr     r0, r0, #0x1               // Sets MMU bit
   mcr     p15, 0, r0, c1, c0, 0      // Write SCTLR, Control Register
   dsb
   isb 
   bx lr   
   

boot_disable_mmu:
   // Call memory barrier operations to ensure completion of all cache 
   // maintenance & branch predictor operations appearing in program 
   // order, before these instructions 
   dsb
   isb
   mrc     p15, 0, r0, c1, c0, 0      // Read SCTLR, Control Register
   bic     r0, r0, #0x1               // Clear MMU bit
   mcr     p15, 0, r0, c1, c0, 0      // Write SCTLR, Control Register
   dsb
   isb
   bx lr

mmu_relocate_page_table_base:

   stmfd    sp!,{r0-r12,lr}
   dsb
   isb
   mov      r1,r0                   // move destination page table base to r1
   mrc      p15, 0, r0, c2, c0, 0   // R0 = TTBR0 
   and      r0, r0, #0x7F           // KEEP THE ATTRIBUTE BITS
   orr      r0, r0, r1             // get the new PT BASE with the old ATTR bits
   mcr      p15, 0, r0, c2, c0, 0   // Update the TTB register to point to Page table base 
   dsb
   isb
   mcr      p15, 0, r0, c8, c7, 0   // UTLBIALL
   mcr      p15, 0, r0, c7, c5, 6   // BPIALL
   
   dsb
   isb
   ldmfd    sp!,{r0-r12,lr}
   bx lr


mmu_get_page_table_base:
   nop
   mrc      p15, 0, r0, c2, c0, 0 // get TTBR0
   lsr      r0, r0, #14           // [31:14] is base address
   lsl      r0, r0, #14
   bx lr
   
   

mmu_set_dacr:
  // Configure Domain access control register register DACR to have appropriate 
  // permissions for different domains.
  mcr     p15, 0, r0,  c3, c0, 0         // Write DACR  
  dsb
  isb
  bx lr
  

memory_barrier:
   dsb
   bx     r14

boot_is_icache_enabled:
   stmfd    sp!,{r0-r12,lr}
   mrc     p15, 0, r1, c1, c0, 0      // Read SCTLR, Control Register 
   mov     r0, #1
   and     r0, r1, r0, lsl #12        // Check I bit 
   ldmfd    sp!,{r0-r12,lr}
   bx lr   

boot_is_dcache_enabled:
   stmfd    sp!,{r0-r12,lr}
   mrc     p15, 0, r1, c1, c0, 0      // Read SCTLR, Control Register 
   mov     r0, #1
   and     r0, r1, r0, lsl #2         // Check C bit 
   ldmfd    sp!,{r0-r12,lr}
   bx lr   

cache_mmu_init_page_table:
   stmfd    sp!, {r0-r12, lr}
   mov      r5, r0                  // backing up the base register
   bl       mmu_flush_cache
   mov      r0, #0    
   mcr      p15, 0, r0, c8, c7, 0   // Invalidate instruction and data TLB
   mcr      p15, 0, r0, c2, c0, 2   // Use Translation Base Reg 0, 16KB TTB boundary
   mov      r0, r5
//	[31:14-N]	Translation table base 0	Pointer to the level one translation table.
//	[13-N:7]	Reserved	UNP or SBZ.
//	[6]	IRGN[0]	Used with bit 0, IRGN[1] to describe inner cacheability.
//	[5]	Reserved	RAZ/WI.
//	[4:3]	RGN	
//	Outer Cacheable attributes for translation table walking:
//	b00 = Outer Non-cacheable
//	b01 = Outer Cacheable Write-Back cached, Write-Allocate
//	b10 = Outer Cacheable Write-Through, no allocate on write
//	b11 = Outer Cacheable Write-Back, no allocate on write.
//	[2]	Reserved	SBZ. This bit is not implemented on this processor.
//	[1]	S	
//	Translation table walk:
//	0 = to non-shared memory.
//	1 = to shared memory.
//	[0]	IRGN[1]	
//	Indicates inner cacheability for the translation table walk:
//	IRGN[1], IRGN[0]
//	00 = Non-cacheable
//	01 = Write-Back Write-Allocate
//	10 = Write-Through, no allocate on write
//	11 = Write-Back no allocate on write.
//	Page table walks do look-ups in the data cache only in write-back. Write-through is treated as non-cacheable.
   ORR      r0, r0, #0x6A           // 0x6A = 0b1101010. TTBR0[13:0]
   mcr      p15, 0, r0, c2, c0, 0   // Update the TTB register to point to Page table base 
   mov      r0, #0
   mcr      p15, 0, r0, c13, c0, 0  // Set FCSE PID Register to flat mapping
   //mmu_set_dom_access 0x55555555    // set all domains for client access   
   ldr     r0,  =0x55555555
   mcr     p15, 0, r0, c3, c0, 0
   bl mmu_disable_i_and_d_cache        // based on arm architecture doc, disable i-cache before enabling mmu
   bl boot_enable_mmu                       // Enable MMU
   bl mmu_enable_i_and_d_cache
   ldmfd    sp!, {r0-r12, lr}
   bx lr
 
cache_mmu_disable:
   stmfd  sp!, {r0-r12, lr}
   //Flushing cache and disabling should be atomic without any memory load
   //store operations in between.   
   bl     mmu_flush_cache_and_disable
   mov    r0, #0    
   bl boot_disable_mmu
   bl mmu_invalidate_tlb   
   ldmfd  sp!, {r0-r12, pc} 

mmu_disable_i_and_d_cache:
   stmfd  sp!, {r0-r4, lr}
   // Call memory barrier operations to ensure completion of all cache 
   // maintenance & branch predictor operations appearing in program 
   // order, before these instructions
   dsb
   isb
   mrc     p15, 0, r0, c1, c0, 0   //  Read SCTLR, Control register
   mov     r1, #1
   bic     r0, r0, r1, lsl #2             //  C bit - Disable Data Cache
   bic     r0, r0, r1, lsl #12            //  I bit - Disable Instruction Cache
   mcr     p15, 0, r0, c1, c0, 0   //  Write SCTLR, Control register
   dsb
   isb   
   ldmfd  sp!, {r0-r4, pc}    
   
 
mmu_enable_i_and_d_cache:
   stmfd  sp!, {r0-r4, lr}
   // Call memory barrier operations to ensure completion of all cache 
   // maintenance & branch predictor operations appearing in program 
   // order, before these instructions
   dsb
   isb
   mrc     p15, 0, r0, c1, c0, 0   //  Read SCTLR, Control register
   mov     r1, #1
   orr     r0, r0, r1, lsl #2             //  C bit - Enable Data Cache
   orr     r0, r0, r1, lsl #12            //  I bit - Enable Instruction Cache
   mcr     p15, 0, r0, c1, c0, 0   //  Write SCTLR, Control register
   dsb
   isb   
   ldmfd  sp!, {r0-r4, pc}    
     
   
mmu_flush_cache_and_disable:
  stmfd  sp!, {r0-r12, lr}	
  // Call memory barrier operations to ensure completion of all cache 
  // maintenance & branch predictor operations appearing in program 
  // order, before these instructions 
  dsb
  isb
  bl dcache_clean_all
  // need to disable caches here. else the restoring of regs [ldmfd]
  // caches those addresses and cache does not remain flushed.
  bl mmu_disable_i_and_d_cache
  
  // Invalidate cache after disabling it. This ensures no valid cache lines
  // remain in disabled cache.  
  mov  r0, #0
  mcr  p15, 0, r0, c7, c5, 0		// I+BTB cache invalidate
  dsb
  isb
  
#ifdef BOOT_USES_L2_DCACHE
  // Flush L2 cache and disable it
  bl  l2_cache_flush_and_disable
  dsb
  isb
#endif
  
  ldmfd  sp!, {r0-r12, pc}   
  
mmu_invalidate_data_cache:
   stmfd  sp!, {r0-r12, lr}
   // Call memory barrier operations to ensure completion of all cache 
   // maintenance & branch predictor operations appearing in program 
   // order, before these instructions 
   dsb
   isb
   ldr r0, =0x0
   mcr p15, 0, r0, c7, c6, 1
   dsb
   isb
   ldmfd  sp!, {r0-r12, lr}
   bx      lr

mmu_invalidate_instruction_cache:
   stmfd  sp!, {r0, lr}
   // Call memory barrier operations to ensure completion of all cache 
   // maintenance & branch predictor operations appearing in program 
   // order, before these instructions 
   dsb
   isb
   ldr     r0, =0x0
   mcr     p15, 0, r0, c7, c5, 0
   dsb
   isb
   ldmfd  sp!, {r0, lr}
   bx      lr   
   
//============================================================================
// mmu_enable_instruction_cache
//
// ARGS
//   NONE
//
// DESCRIPTION
//   Enables the level one and level two intruction cache by setting the I1 and I2 bit 
//   in the ACTLR register. The mmu remains disabled
//============================================================================
mmu_enable_instruction_cache:
   stmfd  sp!, {r0-r12, lr}
   // Call memory barrier operations to ensure completion of all cache 
   // maintenance & branch predictor operations appearing in program 
   // order, before these instructions 
   dsb
   isb
   mrc    p15, 0, r0, c1, c0, 0     // Read from CP15 Control Register
   orr    r0, r0, # (1 << 12 )      // Enable Instruction Cache
   mcr    p15, 0, r0, c1, c0, 0     // Write back to CP15 Control Register
   dsb
   isb
   ldmfd  sp!, {r0-r12, pc}   