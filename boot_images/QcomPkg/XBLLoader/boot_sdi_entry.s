.text
.align 8

.extern boot_sdi_ddr_exit_sr_handler
.global boot_ddr_exit_self_refresh
.global boot_ddr_enter_self_refresh
.global boot_sdi_stack_start

boot_ddr_exit_self_refresh:
    mov     r1, SP
    ldr     r0, =boot_sdi_stack_start
    mov     SP, r0
    push    {r1,r14}
    push    {r5-r12}
    bl boot_sdi_ddr_exit_sr_handler
    mov     r0, 1
    pop     {r5-r12}
    pop     {r1,r3}
    mov     SP, r1
    mov     PC, r3
    
boot_ddr_enter_self_refresh:
    mov     r1, SP
    ldr     r0, =boot_sdi_stack_start
    mov     SP, r0
    push    {r1,r14}
    push    {r5-r12}
    bl boot_sdi_ddr_enter_self_refresh
    mov     r0, 1
    pop     {r5-r12}
    pop     {r1,r3}
    mov     SP, r1
    mov     PC, r3

.p2align 7
boot_sdi_stack_end: 			.space  0x200
boot_sdi_stack_start: