.text
.align 2

    GCC_ASM_EXPORT (boot_disable_intr)
    GCC_ASM_EXPORT (boot_enable_intr)
    
boot_disable_intr:
	mrs r0, cpsr
    cpsid i
    bx lr

boot_enable_intr:
    cpsie i
    bx lr	
