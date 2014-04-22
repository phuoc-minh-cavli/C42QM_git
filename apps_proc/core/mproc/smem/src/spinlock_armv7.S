;
;        ARMv7  S P I N L O C K  A S S E M B L Y   R O U T I N E 
;
; GENERAL DESCRIPTION
;   This module contains the WinMob port of REX spinlock functions that needed   
;   to be coded in assembly.                                                     
;   This is a replacement for drivers\smem\spinlock due to suspicion that        
;   __asm{} code is modified by the ARM compiler in ways that are undesireable.  
;                                                                                
; EXTERNALIZED FUNCTIONS                                                         
;   spin_lock_internal                                                           
;                                                                                
;   spin_unlock_internal                                                         
;                                                                                
; INITIALIZATION AND SEQUENCING REQUIREMENTS                                     
;                                                                                
;  Copyright (c) 1998-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.      
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*    
                                                                                 
;============================================================================        
;                                                                                
;                      EDIT HISTORY FOR MODULE                              
;
; This section contains comments describing changes made to the module.
; Notice that changes are listed in reverse chronological order.
;
; $Header: //components/rel/core.tx/6.0/mproc/smem/src/spinlock_armv7.S#1 $
;
; when      who    what, where, why
; --------  -----  -----------------------------------------------------------
; 01/22/13    bt   Modify to work for TN Apps (Cortex-A) as a replacement for
;                  nonfunctional HW Mutexes on B-family.
;                  - Also add spin_lock_get_asm implementation.
; 11/05/10    rs   Hold both spinlock implementations, assembly and HW,
;                  to allow processors switch to HW mutexes at their own pace 
; 05/27/08    sa   Cleanup to make this file arch specific, no os dependency.
;                  Renamed file to spinlock_armv7.s           
; 05/13/08   hwu   Spinlock for ARM V7.
; 07/25/07   hwu   Added spinlock support for Scorpion.
;                  Removed MSMHW_NO_SWP_INSTRUCTION which is a workaround for
;                  7200 90nm rev. A. We only build for 7200 90nm rev. B.
; 04/08/07   ptm   Add smem_ prefix to spin lock names.
; 08/18/06   gfr   Support for MSMHW_NO_SWP_INSTRUCTION.
; 07/18/06   taw   Added error checking for input param in spin_lock() and
;                  spin_unlock().
; 03/24/06   ptm   Moved spinlocks to shared memory structure.
; 07/29/05   ptm   Added PRESERVE8 directive and changed spin_unlock to comply
; 05/27/05    jc   Moved g_intlock_array from CODE to DATA area.
; 02/15/05    cr   Spin lock as originally implemented, using two SWP 
;                  instructions, had a timing hole, or race condition, between
;                  two CPUs such that dead lock could occur. The new 
;                  spin lock implementation errors on the side of correctness
;                  and simplicity. Obviously execution speed optimizations are
;                  not attempted.
; 01/31/05    cr   Add spinlock/unlock around access to LED such that TIMETEST
;                  will work correctly in the presence of multiple CPU. The
;                  spinlock code is in services\rex\spinlockarm.[sh] and 
;                  depends on uncached, unbuffered shared memory between CPU, 
;                  and depends on ARM SWP instruction being implemented to 
;                  to provide atomicity of memory access between processors.
;                  The TIMETEST functionality now uses a bit in the LED register
;                  to indicate from which of two CPU the TIMETEST data 
;                  originated.                                                         
;=============================================================================            
   IMPORT spinlock_ptr

#ifdef IMAGE_APPS_PROC
spinlock_pid    EQU    1               ; PID or "owner"
#elif defined(RIVA_BRINGUP_VIRTIO)
spinlock_pid    EQU    5
#elif defined(IMAGE_RPM_PROC)
spinlock_pid    EQU    6               ; PID or "owner"
#endif

    AREA RexRom1, CODE, READONLY
    ARM

;=============================================================================          
;                                                                                       
;  FUNCTION spin_unlock_internal_asm                                                    
;                                                                                       
;  DESCRIPTION                                                                          
;    Free a spin lock as indicated by input integer.                                    
;    Re-enable IRQ if it was disabled by spin_lock.                                     
;                                                                                       
;  DEPENDENCIES                                                                         
;    Depends on shared memory data definitions in smem_data_decl.c.                     
;    Depends on MMU being configured as non-cached, non-buffered for the                 
;    shared memory.                                                                      
;                                                                                        
;  PROTOTYPE                                                                             
;    void spin_unlock_internal_asm(int)                                                           
;                                                                                        
;  RETURN VALUE                                                                          
;    None                                                                                
;                                                                                        
;  SIDE EFFECTS                                                                          
;                                                                                        
;=============================================================================           
  EXPORT  spin_unlock_internal_asm
        ALIGN
        ROUT
spin_unlock_internal_asm

    ; load r1 with pointer to spin lock,
    ldr     r1, =spinlock_ptr
    ldr     r1,[r1]
    add     r1, r1, r0, lsl #2

    ; release spin lock
    mov     r2, #0
    str     r2, [r1]

    bx      lr

;=============================================================================           
;                                                                                        
;  FUNCTION spin_lock_internal_asm                                                       
;                                                                                        
;  DESCRIPTION                                                                           
;    Acquire a spin lock as indicated by input integer.                                  
;    Disable IRQ if not already disabled.                                                
;                                                                                        
;  DEPENDENCIES                                                                          
;    Depends on shared memory data definitions in smem_data_decl.c.                      
;    Depends on MMU being configured as non-cached, non-buffered for the                 
;    shared memory.                                                                      
;                                                                                        
;  PROTOTYPE                                                                             
;    void spin_lock_internal_asm(int)                                                             
;                                                                                        
;  RETURN VALUE                                                                          
;    None                                                                                
;                                                                                        
;  SIDE EFFECTS                                                                          
;                                                                                        
;=============================================================================           
  EXPORT  spin_lock_internal_asm
        ALIGN
        ROUT
spin_lock_internal_asm

    ldr     r1, =spinlock_ptr
    ldr     r1,[r1]
    add     r1, r1, r0, lsl #2
    mov     r2, #spinlock_pid          ; PID to hold the lock

tryit
    ldrex   r3,[r1]
    cmp     r3,#0                      ;Lock is not free need to try again
    bne     tryit

    strex   r3, r2, [r1]
    cmp     r3,#0                      ;store failed need to try again
    bne     tryit

    bx      lr

;=============================================================================           
;                                                                                        
;  FUNCTION spin_lock_get_asm                                                       
;                                                                                        
;  DESCRIPTION                                                                           
;    Returns the current owner PID of the lock                                           
;                                                                                        
;  DEPENDENCIES                                                                          
;    Depends on shared memory data definitions in smem_data_decl.c.                      
;    Depends on MMU being configured as non-cached, non-buffered for the                 
;    shared memory.                                                                      
;                                                                                        
;  PROTOTYPE                                                                             
;    int spin_lock_get_asm(int)                                                             
;                                                                                        
;  RETURN VALUE                                                                          
;    Current value of the spin lock                                                                                
;                                                                                        
;  SIDE EFFECTS                                                                          
;                                                                                        
;=============================================================================
  EXPORT  spin_lock_get_asm
        ALIGN
        ROUT
spin_lock_get_asm

    ldr     r1, =spinlock_ptr
    ldr     r1,[r1]
    add     r1, r1, r0, lsl #2
    ldr     r0,[r1]

    bx      lr

    END
