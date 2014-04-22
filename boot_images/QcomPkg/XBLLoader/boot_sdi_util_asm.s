.text
.align 2

   .global  sbl1_sdi_get_current_el
   .global  boot_sdi_scm

sbl1_sdi_get_current_el:
   bx       lr

boot_sdi_scm:
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