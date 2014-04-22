// file: utils_asm.S


    ARM
    AREA ||.text||, CODE, READONLY

    EXPORT QURTK_lifo_pop
QURTK_lifo_pop
    ldr     r2, [r0]
    cmp     r2, #0
    beq     QURTK_lifo_pop_ret
    ldr     r1, [r2]
    str     r1, [r0]
QURTK_lifo_pop_ret
    mov     r0, r2
    bx      lr

    EXPORT QURTK_lifo_push
QURTK_lifo_push
    ldr     r2, [r0]
    str     r2, [r1]
    str     r1, [r0]
    bx      lr
    
    END

