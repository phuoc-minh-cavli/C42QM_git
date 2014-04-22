;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                           R E X  A R M   C O R E 
;
; GENERAL DESCRIPTION
;   Capture a "core" dump
;
; EXTERNALIZED FUNCTIONS
;   rex_jettison_core
;
; Copyright (c) 2002-2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

;============================================================================
;
;                      EDIT HISTORY FOR MODULE
;
; $Header: //components/rel/core.tx/6.0/debugtools/err/src/arm/err_jettison_core.s#1 $
;
; when      who    what, where, why
; --------  -----  -----------------------------------------------------------
;============================================================================

;
; Any change to the arch_coredump_type structure will
; necessitate a change to REX_CORE.
;
	
    AREA    REX_CORE, DATA

arm_core_t RN     r7 
           MAP    0,arm_core_t
	
; Fields must match up with arch_coredump_type for ARM
; Registers should match enum arm_coredump_register_type 
svc_r0     FIELD  4
svc_r1     FIELD  4
svc_r2     FIELD  4
svc_r3     FIELD  4
svc_r4     FIELD  4
svc_r5     FIELD  4
svc_r6     FIELD  4
svc_r7     FIELD  4
svc_r8     FIELD  4
svc_r9     FIELD  4
svc_r10    FIELD  4
svc_r11    FIELD  4
svc_r12    FIELD  4
svc_sp     FIELD  4
svc_lr     FIELD  4
svc_spsr   FIELD  4
svc_pc     FIELD  4

sys_sp     FIELD  4
sys_lr     FIELD  4

irq_sp     FIELD  4
irq_lr     FIELD  4
irq_spsr   FIELD  4

abt_sp     FIELD  4
abt_lr     FIELD  4
abt_spsr   FIELD  4

udf_sp     FIELD  4
udf_lr     FIELD  4
udf_spsr   FIELD  4

fiq_r8     FIELD  4
fiq_r9     FIELD  4
fiq_r10    FIELD  4
fiq_r11    FIELD  4
fiq_r12    FIELD  4
fiq_sp     FIELD  4
fiq_lr     FIELD  4
fiq_spsr   FIELD  4

jva_sp     FIELD  4
jva_lr     FIELD  4
jva_spsr   FIELD  4

cpsr       FIELD  4

    IMPORT arch_coredump_ptr

    AREA RexRom, CODE, READONLY
    CODE32

;=============================================================================
; FUNCTION JETTISON_CORE
;
; DESCRIPTION
;    Captures the ARM register state
;    - Store CPSR
;    - For each mode:
;        save reqd regs to rex_core
;    - Return to entry mode
;=============================================================================
    EXPORT jettison_core		
jettison_core	
    stmfd   sp!, {r7}

    ; Capture User Mode r0-r14 (no SPSR)
    ; Registers are stored in svc structure for backwards compatibility
    ldr     arm_core_t,=arch_coredump_ptr
    ldr     arm_core_t, [r7]
    str     r0,  svc_r0
    str     r1,  svc_r1
    str     r2,  svc_r2
    str     r3,  svc_r3
    str     r4,  svc_r4
    str     r5,  svc_r5
    str     r6,  svc_r6
    ; Store r7 later after restoring it from the stack
    str     r8,  svc_r8
    str     r9,  svc_r9
    str     r10, svc_r10
    str     r11, svc_r11    
    str     r12, svc_r12

    ; LR is USR (SYS) mode values.
    str     lr,  svc_lr

    ; We do not have an entry yet for the PC in user mode.Do not save this
    ; as it will cause confusion

    ; LR in all other modes will be destroyed by a syscall to determine
    ; it is contents, so ignore this for the time being

    ; Store R7
    ldmfd   sp!, {r2}
    str     r2, svc_r7

    ; Store SP value (USR mode)
    str     sp, svc_sp

    ; Store CPSR
    mrs     r0, CPSR
    str     r0, cpsr

    ; Store the PC for restoration later
    ldr     r0, =jettison_core
    str     r0, svc_pc
    
    ; Restore original r7 value (currently held in r2)
    mov     r7, r2	
		
    ; Finished so return
    bx      lr
    bx      lr

;======================================================================
    END
;======================================================================
