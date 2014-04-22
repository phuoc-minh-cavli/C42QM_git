
;=========================================================================
;                             Edit History
;
; when       who       what, where, why
; --------   ---       ---------------------------------------------------
; 06/14/16   ck        Changed vector table to be 128 byte aligned
; 02/11/16   ck        Added boot_memory_barrier 
; 02/03/16   ck        Replaced BOOT_HWIO_INCLUDE_H with msmhwioreg.h 
; 01/05/16   ck        Updated for new SBL error logic
; 11/24/15   ck        Removed qc_image_jump_func
; 10/09/15   ck        Corrected stack address
; 09/16/15   ck        Updated to use shared stack between SBL and HLOS
; 07/31/15   ck        Initial version
;
;=========================================================================
;-------------------------------------------------------------------------
;                           INCLUDES
;-------------------------------------------------------------------------
;#include CUST_H
;#include "msmhwioreg.h"
	
;-------------------------------------------------------------------------
;                           IMPORTS
;-------------------------------------------------------------------------
;    IMPORT ||Image$$RAM_FOM_BSP_STACK_ZI_REGION$$ZI$$Limit||
;    IMPORT sbl1_nmi_c_handler
;    IMPORT sbl1_hard_fault_c_handler           
;    IMPORT sbl1_memory_management_fault_c_handler 
;    IMPORT sbl1_bus_fault_c_handler               
;    IMPORT sbl1_usage_fault_c_handler
     IMPORT entry_main_ctl
;    IMPORT qc_image_jump_func
    IMPORT _start
	
;-------------------------------------------------------------------------
;                           EXPORTS
;-------------------------------------------------------------------------
    // Export the symbols __main and _main to prevent the linker from
    // including the standard runtime library and startup routine.
    EXPORT   __main
    EXPORT   _main
    EXPORT   SBL_Entry
    EXPORT   boot_memory_barrier
	
;-------------------------------------------------------------------------
;                           DATA
;-------------------------------------------------------------------------

;    AREA    SBL_VECTOR_TABLE, DATA, READONLY, ALIGN=7
;__Vectors      ; Minimal SBL1 Vector Table entries
;    DCD     ||Image$$RAM_FOM_BSP_STACK_ZI_REGION$$ZI$$Limit||	   ; Initial Main SP
;    DCD     0x00000000                             ; Reset
;    DCD     sbl1_nmi_c_handler 	                   ; NMI
;    DCD     sbl1_hard_fault_c_handler              ; Hard Fault
;    DCD     sbl1_memory_management_fault_c_handler ; Memory Management Fault
;    DCD     sbl1_bus_fault_c_handler               ; Bus Fault
;    DCD     sbl1_usage_fault_c_handler             ; Usage Fault

	
;-------------------------------------------------------------------------
;                           FUNCTION DEFINITIONS
;-------------------------------------------------------------------------

    AREA    QUARTZ_SBL_ENTRY, CODE, READONLY, ALIGN=4
__main
_main 
SBL_Entry
	
	ldr    r5, =_start
    blx    r5
    bl loophere
	
loophere
    b loophere


;----------------------------------------------------------------------
; This function is to be used for memory barrier
;----------------------------------------------------------------------
boot_memory_barrier
    dsb
    isb
    bx	lr

    END
