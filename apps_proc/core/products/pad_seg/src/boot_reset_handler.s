;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                   B O O T    R E S E T    H A N D L E R
;
; GENERAL DESCRIPTION
;   This module contains the reset handler function that is executed after
;   reset of the ARM processor.
;
; EXTERNALIZED FUNCTIONS
;   Reset_Handler
;
; INITIALIZATION AND SEQUENCING REQUIREMENTS
;   None.
;
; Copyright (c) 2003-2008 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

;============================================================================
;
;                       EDIT HISTORY FOR MODULE
;
; This section contains comments describing changes made to the module.
; Notice that changes are listed in reverse chronological order.
;
;$Header: //components/rel/core.tx/6.0/products/pad_seg/src/boot_reset_handler.s#1 $ $DateTime: 2020/01/30 22:49:35 $
;
; when      who     what, where, why
; --------  -----   ---------------------------------------------------------
; 01/28/08  mjs     Export soft_breakpoints_enabled symbol for RVCT 3.0.
; 04/19/06  sam     Changes in process init due to L4/Iguana 1.1.1 release.
; 04/03/06  ptm     Move the initialization to process.c to support AMSS restart
; 03/27/06  jhs     Changed AMSS HEAP managment to MemHeap implementation.
; 09/01/05  amw     Added label for breaking to set soft breakpoints
; 08/19/05  sd      Made change to mARM/aARM synchronization when booting
;                   from flash until state machine in place. 
; 04/26/05  ptm     Remove unnecessary T_MSM7500 feature.
; 04/26/05  ptm     First release with edit history.
;============================================================================

;============================================================================
;                             MODULE INCLUDES
;============================================================================

;============================================================================
;                             MODULE IMPORTS
;============================================================================
        IMPORT  main
        IMPORT  process_init

;============================================================================
;                             MODULE EXPORTS
;============================================================================
        EXPORT  Reset_Handler
        EXPORT  soft_breakpoints_enabled

;============================================================================
;                   PROCESSOR RESET EXCEPTION HANDLER
;============================================================================
; Reference:            ARM1136 Rev:r0p1 Technical Reference Manual
; Document Number:      ARM DDI 00211C
; Issued:               February 2003
; Section:              2.10.6
;
;  When the nRESETIN signal is driven LOW a reset occurs, and the ARM1136J 
;  processor abandons the executing instruction. 
;  When nRESETIN is driven HIGH again the ARM1136J processor:
;
; 1. Forces CPSR M[4:0] to b10011 (Supervisor mode), sets the A, I, and F bits
;    in the CPSR, and clears the CPSR T bit and J bit. The E bit is set based on
;    the state of the BITENDIENIT and UBITINIT pins. Other bits in the CPSR are
;    indetermintate.
;
; 2  Forces the PC to fetch the next instruction from the reset vector address.
;
; 3  Reverts to ARM state and resumes execution.
; 
;  After reset, all register values except the PC and CPSR are indeterminate.
;============================================================================
        PRESERVE8
        AREA    RST_Handler, CODE, READONLY
        CODE32

Reset_Handler

    ;------------------------------------------------------------------
    ; process init - initializes the AMSS process
    ; returns the number of bytes to pop off the stack
    ;------------------------------------------------------------------
    ldr     r0, [sp]
    blx     process_init
    add     sp, sp, r0

soft_breakpoints_enabled ; All pages of apps code have been paged in.
    ;------------------------------------------------------------------
    ; Pop argc and argv and launch into main.  If main is compiled with
    ; thumb mode, blx will ensure that we properly change to thumb mode
    ;------------------------------------------------------------------
    ldmia   sp!, {r0}
    mov     r1, sp
    blx     main

;============================================================================

        END
