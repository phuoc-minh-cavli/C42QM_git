#include "boot_msm.h"
#include "boot_target.h"
#include "boot_cache_mmu.h"

.set  Mode_SVC_OR_F,       (0x13 | 0x40)
.set  Mode_ABT_OR_I_OR_F,  (0x17 | 0x80 | 0x40)
.set  Mode_UND_OR_I_OR_F,  (0x1b | 0x80 | 0x40)
.set  Mode_IRQ_OR_I_OR_F,  (0x12 | 0x80 | 0x40)

.set  I_Bit,  0x80
.set  F_Bit,  0x40

    .extern boot_undefined_instruction_c_handler
    .extern boot_swi_c_handler
    .extern boot_prefetch_abort_c_handler
    .extern boot_data_abort_c_handler
    .extern boot_reserved_c_handler
    .extern boot_irq_c_handler
    .extern boot_fiq_c_handler
    .extern boot_nested_exception_c_handler
    .extern sbl1_main_ctl
    .extern dcache_flush_region
    .extern mmu_invalidate_instruction_cache
    .extern mmu_invalidate_tlb
    .extern mmu_enable_instruction_cache
    .extern cache_mmu_disable



    .global sbl_loop_here
    .global boot_read_l2esr
   
    .global   __main
    .global   _main
    .global   sbl1_entry
	
    //AREA  SBL1_ENTRY, CODE, READONLY, ALIGN=4
    .section  SBL1_ENTRY, "ax", %progbits
    .p2align 4
    
    //CODE
    //ENTRY
    
	
unused_reset_vector:
    B     0x00000000
undefined_instruction_vector:
    B     sbl1_undefined_instruction_nested_handler
swi_vector:
    B     boot_swi_c_handler
prefetch_abort_vector:
    B     sbl1_prefetch_abort_nested_handler
data_abort_vector:
    B     sbl1_data_abort_nested_handler
reserved_vector:
    B     boot_reserved_c_handler
irq_vector:
    B     sbl1_irq_handler
fiq_vector:
    B     boot_fiq_c_handler

    
__main:
_main:
sbl1_entry:

    msr     CPSR_c, #Mode_SVC_OR_F

    mov     r7, r0

    ldr     r0, =SCL_SBL1_VECTOR_BASE
    MCR     p15, 0, r0, c12, c0, 0
    ldr     r0, =(SCL_SBL1_CODE_STACK_BASE+SCL_SBL1_CODE_STACK_SIZE)
    mov     r13, r0
    msr     CPSR_c, #Mode_UND_OR_I_OR_F
    mov     r13, r0
    msr     CPSR_c, #Mode_ABT_OR_I_OR_F
    mov     r13, r0

    msr     CPSR_c, #Mode_IRQ_OR_I_OR_F
    ldr     r0, =(SCL_SBL1_IRQ_STACK_BASE+SCL_SBL1_IRQ_STACK_SIZE)
    mov     r13, r0	
	
    msr     CPSR_c, #Mode_SVC_OR_F

    // back-up r1 register before using it as parameter
    mov    r5, r1
    
    // invalidate i caches, TLBs and enable instruction cache.
    bl mmu_invalidate_instruction_cache
    bl mmu_invalidate_tlb
    bl mmu_enable_instruction_cache
    bl cache_mmu_disable

    mov     r0, r7
    ldr    r5, =sbl1_main_ctl
    blx    r5
    bl loophere  

sbl_loop_here:
    mov r0,#0
    mov r1,#0
    mov r2,#0
    mov r3,#0
    mov r4,#0
    mov r5,#0
    mov r6,#0
    mov r7,#0
    mov r8,#0
    mov r9,#0
    mov r10,#0
    mov r11,#0
    mov r12,#0
loophere:
    b loophere

sbl1_irq_handler:
	sub 	lr, lr, #4
	stmfd	sp!, {r0-r12, lr}
	blx     boot_irq_c_handler
	ldmfd	sp!, {r0-r12, pc}^
	
sbl1_undefined_instruction_nested_handler:
    ldr r5,=boot_undefined_instruction_c_handler
    b   check_for_nesting

sbl1_prefetch_abort_nested_handler:
    ldr r5,=boot_prefetch_abort_c_handler
    b   check_for_nesting

sbl1_data_abort_nested_handler:
    ldr r5,=boot_data_abort_c_handler
    b   check_for_nesting

check_for_nesting:

    ldr r6,=(SCL_SBL1_CODE_STACK_BASE+SCL_SBL1_CODE_STACK_SIZE)
    mov r7, r13                            
    cmp r6, r7                             
    blxeq r5                               
    ldr r5,=boot_nested_exception_c_handler
    blx r5

boot_read_l2esr:
    mov r1, #0x204   
    isb
    mcr p15,3,r1,c15,c0,6 
    isb
    mrc p15,3,r0,c15,c0,7 
    isb
    bx lr
    

