.text
.align 2

   .global  sbl1_get_current_el
   .global  boot_scm
   .global  qmemset

    //AREA  BOOT_UTIL_ASM, CODE, READONLY, ALIGN=4
    .section  BOOT_UTIL_ASM, "ax", %progbits
    .p2align 4
	
sbl1_get_current_el:
   bx       lr

boot_scm:
   //with pushing 10 registers(r4-r12) 0x28(40) bytes on stack, 
   //  r13 will reduce (stack grows) by 0x28 bytes
   stmfd sp!, {r4-r12, lr}  
// r0-r3 are passed as parameters and r4-r7 are pushed on stack for armv7 
//        compiled code with 7 arguments SMC call
// retrieve r4-r7 from registers pushed on stack before boot_scm was called.
   ldr      r4,[r13, #(0x0+0x28)] //x28 offset on stack has r4 due to pushing (r4-r12,lr) on stack.
   ldr      r5,[r13, #(0x4+0x28)]
   ldr      r6,[r13, #(0x8+0x28)]
   ldr      r7,[r13, #(0xC+0x28)]
   
exec_again:
   smc     0x0
smc_ret:
// If return value == 1, SMC call interrupted 
// should not happen in SBL with interrupts disabled and running single core
       
   cmp    r0, #1         
   beq    exec_again     // re-execute smc call if == 1
// At this point r0 contains TZ smc call return status
      
// if resp_buff_ptr is NULL then skip storing return response 
   cmp     r7, #0
   beq     scm_exit
// else: store SMC return response registers in to resp_buff data structure
//             start from r1 since r0 has SMC_SUCCESS(0)/FAILIRE(1), handle in SMCCALL macro.
   str     r1, [r7, #0x0]	     // resp_buff_ptr->reg[0]
   str     r2, [r7, #0x4]        // resp_buff_ptr->reg[1]
   str     r3, [r7, #0x8]        // resp_buff_ptr->reg[2]
scm_exit: 
   ldmfd sp!, {r4-r12, pc}
    bx lr  

//======================================================================
//
// void __aeabi_memset(void *dst, uint32 size, uint8 c)
// void __aeabi_memclr(uint32 *dst, uint32 size)
// void __aeabi_memclr4(uint32 *dst, uint32 size)
// void __aeabi_memclr8(uint64 *dst, uint32 size)
// void __rt_memclr(uint32 *dst, uint32 size)
// void __rt_memclr_w(uint32 *dst, uint32 size)
//
// Fast replacement for C library memset(), optimized using the stm
// instruction to generate INCR8 AXI transactions, and replacing all
// 8-bit memory accesses with 32-bit accesses.
// 
// Parameters:
//   r0 contains the destination address
//   r1 is the size in bytes to be set or cleared
//   r2 is the byte pattern to fill the destination memory
//
//======================================================================
//
// void qmemset(uint32 *dst, uint32 val, uint32 len)
// 
// Parameters:
//   r0 contains the destination address
//   r1 is the byte pattern to fill the destination memory
//   r2 is the size in bytes to be set
//
//======================================================================
qmemset:
   // Re-arrange the parameters AND
   // Fill the 32-bit value with the byte pattern that we want to set
   and      r3, r1, #0xFF
   mov      r1, r2
   orr      r2, r3, r3, lsl #0x08
   orr      r2, r2, r2, lsl #0x10
   b        _memset_entry
__aeabi_memset:
   // Fill the 32-bit value with the byte pattern that we want to set
   and      r3, r2, #0xFF
   orr      r2, r3, r3, lsl #0x08
   orr      r2, r2, r2, lsl #0x10
   b        _memset_entry
__rt_memclr:
__rt_memclr_w:
__aeabi_memclr:
__aeabi_memclr4:
__aeabi_memclr8:
   // If the compiler tells us we are doing a memclr then initialize the
   // byte pattern to 0
   mov      r2, #0
_memset:
_memset_w:
_memset_entry:
   // First test if the destination is unaligned and do an initial unaligned
   // access if so
   ands     r3, r0, #3
   bne      _memset_first_unaligned_bytes
   // Else test if the total remaining bytes to set is less than a word,
   // and if so just set the last bytes
   cmp      r1, #4
   bcc      _memset_last_uneven_bytes
_memset_main:
   // Store the work registers on the stack, store 8 registers so we can
   // do this with a single burst transfer
   stmdb    r13!,{r4-r11}
   // Initialize all the work registers to the byte pattern
   mov      r3, r2
   mov      r4, r2
   mov      r5, r2
   mov      r6, r2
   mov      r7, r2
   mov      r8, r2
   mov      r9, r2
   // We will transfer 8 words at a time, so subtract 32 from the total
   // and set the flags
   subs     r1, r1, #0x20
_memset_loop:
   // If the total is still non-negative then write 8 words to memory
   stmia  r0!, {r2-r9}
   // Subtract 32 from the total again, do this twice in the main loop 
   // to avoid delays due to pipeline flushing in the tight loop
   subs   r1, r1, #0x20
   //stmia  r0!, {r2-r9}
   //subcs   r1, r1, #0x20
   // If the total is still non-negative then transfer more 
   cmp r1, #0
   bge  _memset_loop
   // Now shift out the total 28 places, even though the total went negative
   // the lower 5 bits represent the number of bytes still to transfer, and
   // by shifting by 28 bit 4 goes into the carry bit and bit 3 becomes the 
   // sign bit
   ands r1, r1, #0x1F
   //movs     r1, r1, lsl #0x1B
   cmp r1, #0x10
   blt _copy_8_bytes
   stmia  r0!, {r2-r5}
   
_copy_8_bytes:
   ands r1, r1, #0xF
   cmp r1, #0x8
   blt _copy_4_bytes
   stmia  r0!, {r2-r3}

_copy_4_bytes:   
   // Now shift the total out 2 more bits, bit 2 of the total is now the carry
   // bit and bit 1 is now the sign bit
   ands r1, r1, #0x7
   cmp r1, #0x4
   blt _end_copy   
   stmia  r0!, {r2}

_end_copy:   
   // We can restore the work registers now
   ands r1, r1, #0x3
   ldmia    r13!,{r4-r11}
   // If bits 1 and 0 are zero then we are done
   bxeq     lr
   // Otherwise shift the total back to the right
   mov      r1, r1, lsr #0x1E
_memset_last_uneven_bytes:
   // Take the number of bytes left to copy in r1, multiply by 8, and then
   // shift our mask by that amount
   mov      r1, r1, LSL #3  // Multiply by 8 bits per byte
   ldr      r12, =0xFFFFFFFF
   mov      r12, r12, LSL r1
   // Load the first partial word from memory, mask out the part that will be
   // set, mask out the part of the memset value that we will use set the
   // memory, OR the values together and then store back to memory
   ldr      r3, [r0]
   and      r3, r3, r12
   bic      r12, r2, r12
   orr      r3, r3, r12
   str      r3, [r0]
   bx       lr
_memset_first_unaligned_bytes:
   // r3 has the unaligned offset into the first word, subtract from
   // r0 to get an aligned pointer, subtract (4 minus this value) from 
   // the total size to transfer// set the flags in case the total size 
   // just went went negative
   sub      r0, r0, r3
   add      r1, r1, r3
   subs     r1, r1, #4
   // load the full 32-bit mask for memclr of partial word
   ldr      r12, =0xFFFFFFFF
   // r3 has unaligned offset, multiply this by 8 and then shift the mask
   // by this amount
   mov      r3, r3, LSL #3  // Multiply by 8 bits per byte
   mov      r12, r12, LSL r3
   // If the total size went negative, the negative of r1 now has the number
   // of extra bytes that would be copied if we copied all the bytes to the end
   // of the first word, so negate r1, multiply by 8 and then shift out the
   // mask by this amount and then shift it back, so that the upper bits of
   // the mask are now zeros
   rsbmi    r3, r1, #0  // r3 = -r1
   movmi    r3, r3, LSL #3  // Multiply by 8 bits per byte
   movmi    r12, r12, LSL r3
   movmi    r12, r12, LSR r3
   // Load the first partial word from memory, mask out the part that will be
   // set, mask out the part of the memset value that we will use set the
   // memory, OR the values together and then store back to memory
   ldr      r3, [r0]
   bic      r3, r3, r12
   and      r12, r2, r12
   orr      r3, r3, r12
   str      r3, [r0], #4
   // If the total size is still greater than 0 then do aligned memsets
   bgt      _memset_main
   bx       lr	