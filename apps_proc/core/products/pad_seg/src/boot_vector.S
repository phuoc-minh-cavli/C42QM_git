;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;             A R M 1 1 3 6 E J S   E X C E P T I O N    V E C T O R S
;
; GENERAL DESCRIPTION
;   This file includes the exception vectors for the ARM1136EJS processor.
;   The base address of the exception vector segment is located at addresses
;   0x00000000 or 0xFFFF0000 (set by setting the V bit in the Control Register)
;
;   This file includes the exception vectors for the ARM926EJS processor.
;   The base address of the exception vector segment must be 0x00000000.
;
; EXTERNALIZED FUNCTIONS
;   None.
;
;   None.
;
; INITIALIZATION AND SEQUENCING REQUIREMENTS
;   None.
;
; Copyright (c) 2003-2006 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                           EDIT HISTORY FOR FILE
;
; This section contains comments describing changes made to the module.
; Notice that changes are listed in reverse chronological order.
;
; $Header: //components/rel/core.tx/6.0/products/pad_seg/src/boot_vector.S#1 $
; 
; when       who     what, where, why
; --------   ---     --------------------------------------------------------
; 05/18/06   ptm     Removed unused exception handlers added boot_clear_sl.
; 05/10/05   sd      Added new boot block information area.
;
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

;============================================================================
;                           MODULE INCLUDE FILES
;============================================================================
#include "customer.h"
#include "armasm.h"

;============================================================================
;                             MODULE IMPORTS
;============================================================================
        IMPORT  Reset_Handler

;============================================================================
;                             MODULE EXPORTS
;============================================================================
        ; Export the symbols __main and _main to prevent the linker from
        ; including the standard runtime library and startup routine.
        EXPORT __main
        EXPORT _main

        EXPORT boot_clear_sl

        EXPORT boot_info_symbol
        EXPORT boot_mob_model
        EXPORT boot_cust_mob_model
        EXPORT boot_block_ver
        EXPORT stamp_mob_model
        EXPORT stamp_md5_hash
        EXPORT chk_security_code_ptr

;============================================================================
;                       PROCESSOR EXCEPTION VECTORS
;============================================================================
; Reference:            ARM1136 Rev:r0p1 Technical Reference Manual
; Document Number:      ARM DDI 00211C
; Issued:               February 2003
; Section:              2.10.16
;
; The location of the exception vector addresses can be configured by
; setting the V bit in CP15cl Control Register as shown:
;
; Value of V bit        Exception vector base location
;----------------------------------------------------------------------------
;     0                 Normal exception vectors = 0x00000000
;     1                 High exception vectors = 0xFFFF0000
;
; *** TBD - where do we select which base location we want??? TBD ***
;
; Offect from base     EXCEPTION               MODE ON ENTRY
;----------------------------------------------------------------------------
; 0x00                  Reset                   Supervisor
;----------------------------------------------------------------------------
; 0x04                  Undefined instruction   Undefined
;----------------------------------------------------------------------------
; 0x08                  Software Interrupt      Supervisor
;----------------------------------------------------------------------------
; 0x0C                  Abort (prefetch)        Abort
;----------------------------------------------------------------------------
; 0x10                  Abort (data)            Abort
;----------------------------------------------------------------------------
; 0x14                  Reserved                Reserved
;----------------------------------------------------------------------------
; 0x18                  IRQ                     IRQ
;----------------------------------------------------------------------------
; 0x1C                  FIQ                     FIQ
;============================================================================

        AREA    Int_Vect, CODE, READONLY
        CODE32                         ; 32 bit ARM instruction set.
        ENTRY
        
__main
_main
        
#ifdef FEATURE_SMI
        ldr     pc, =Reset_Handler
#else
        B       Reset_Handler
#endif

        AREA  ERR_HANDLER, CODE, READONLY

;============================================================================
; BOOT_CLEAR_SL
;
;  DESCRIPTION -
;     C prototype - void boot_clear_sl(void);
;     This function clears the SL register (R10).
;============================================================================

 LEAF_NODE boot_clear_sl
 
        eor     r10, r10, r10
        
 LEAF_NODE_END

;============================================================================
;                      BOOT BLOCK INFORMATIONAL AREA
;============================================================================

        AREA    Boot_Block_Info, CODE, READONLY
        
;----------------------------------------------------------------------------
; The following string is used by the QPST application to locate this
; informational block in the ROM image.
;----------------------------------------------------------------------------
boot_info_symbol
        DCB     "QCOM"

;----------------------------------------------------------------------------
; The following number is the family number for this phone and MUST be
; defined.
;----------------------------------------------------------------------------
boot_mob_model
        DCW     CUST_MOB_MODEL

;----------------------------------------------------------------------------
; Customer Mobile Model Number.
;
; This number is only a placeholder for a number which the customer
; can use to differentiate various models in the same family of phones.
;
; Important :
;    Exactly one DCW must appear in the final code at this location.
;    Otherwise, the next symbol, boot_block_ver, will move up in the
;    memory map and lead to problems.
;----------------------------------------------------------------------------
boot_cust_mob_model
        DCW     CUST_MOB_MODEL

;----------------------------------------------------------------------------
; The following numbers are the version number data for the informational
; block.
;----------------------------------------------------------------------------
boot_block_ver
        DCW     1                      ; Current version number.

        DCW     1                      ; Minimum version number for
                                       ; backwards compatability.

;============================================================================
;                      APPLICATION BOOT CHECK AREA
;
;  This segment reserves 32 bytes of space for the check values written
;  after the build has completed.  They are initialized to 0xFF so value
;  can be written directly into the flash part, like the security code.
;
;  This area must be located at the top of application ROM space.
;============================================================================
stamp_mob_model
        DCW     CUST_MOB_MODEL         ; Space for the mobile model number.

stamp_md5_hash
        DCW     &FFFF                  ; Space for the MD5 hash value which
        DCD     &FFFFFFFF              ; can be used as a ROM image checksum.
        DCD     &FFFFFFFF
        DCD     &FFFFFFFF
        DCW     &FFFF

reserved
        DCW     &FFFF
        DCD     &FFFFFFFF

chk_security_code_ptr
        DCD     &FFFFFFFF              ; Space for the 64 bit security code
        DCD     &FFFFFFFF              ; which can be used for access control
                                       ; of the serial link.

;============================================================================
;============================================================================
        END
