#ifndef ARMASM_H
#define ARMASM_H

/**
  @file armasm.h
  @brief This file contains assembly language macros for use with the ARM 
  assembler.

*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The Utility_Services_API_mainpage.dox file contains all file/group 
      descriptions that are in the output PDF generated using Doxygen and 
      Latex. To edit or update any of the file/group text in the PDF, edit 
      the Utility_Services_API_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the "utils_services" 
      group description in the Utility_Services_API_mainpage.dox file. 
===========================================================================*/
/*=============================================================================

                        ARM Assembly Language Definitions

GENERAL DESCRIPTION
  This file contains assembly language macros for use with the ARM assembler.

Copyright (c) 1998-2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary. 
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/api/services/armasm.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/14/11   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
11/09/10  EBR      Doxygenated file.
07/09/10   pc      Added support for PSR_AsyncAbort_Mask.
09/13/05   ck      Copied from 6100 branch
07/12/02   jct     Removed conditional inclusion of customer.h and target.h
05/17/01   kar     Bigger SVC stack available to support REX from MSM archive
01/25/01   day     Merged from MSM5105_COMMON.00.00.05.
                     Added support for Interworking
                     Added define for system stack size
04/02/99    sk     Increased Abort stack size.
04/02/99    ms     Reduced stack sizes.
02/03/99    ms     Renamed blx to blatox since blx is a keyword in Assembler
                   of ARM SDK 2.5
01/18/99    ms     Moved ARM_TRAP_FRAME related declarations to boot_trap.h.
                   Moved context frame related macros to rexarm.s.
                   Incorporated exception frame related macros directly into
                   boot_abort_handler in bootsys.s.
12/10/98   jkl     Included r12 in the context frame.
11/23/98   jkl     Clean up code. Changed save_context_from_task macro.
10/27/98   jkl     Revised for new context
09/13/98   hcg     Changed exception/trap frame names
07/10/98   jct     Revised for coding standard, removed unused code
01/01/98   bwj     Created
=============================================================================*/

#include "target.h"
#include "customer.h"

/** @addtogroup utils_services
@{ */
/** @name Macros Used With the ARM Assembler

The following general-purpose assembly language macros are described in 
Chapter @latexonly\ref{chp:assemblyMacros}@endlatexonly. 
- pusha
- popa
- ENTRY_NODE
- ENTRY_NODE_END
- LEAF_NODE
- LEAF_NODE_END
- LEAF_NODE_16
- LEAF_NODE_END_16
- ALTERNATE_ENTRY_16
- blatox

The following macros are also used the ARM assembler.
@{ */

#define PSR_Thumb_Mask       0x20  /**< CPSR control mask for Thumb. */
#define PSR_Fiq_Mask         0x40  /**< CPSR control mask for FIQ. */
#define PSR_Irq_Mask         0x80  /**< CPSR control mask for IRQ. */
#define PSR_Mode_Mask        0x1f  /**< CPSR control mask for Mode. */
#define PSR_AsyncAbort_Mask  0x100 /**< CPSR control mask for Async Abort. */


#define PSR_User             0x10 /**< Processor mode definition for 
                                       User mode. */
#define PSR_Fiq              0x11 /**< Processor mode definition for 
                                       FIQ mode. */
#define PSR_Irq              0x12 /**< Processor mode definition for 
                                       IRQ mode. */
#define PSR_Supervisor       0x13 /**< Processor mode definition for 
                                       Supervisor mode. */
#define PSR_Abort            0x17 /**< Processor mode definition for 
                                       Abort mode. */
#define PSR_Undef            0x1b /**< Processor mode definition for 
                                       undefined. mode. */
#define PSR_System           0x1f /**< Processor mode definition for 
                                       System mode. */
#define PSR_Thumb            0x20 /**< Processor mode definition for 
                                       Thumb mode. */
/** @} */ /* end_name_group Macros Used With the ARM Assembler */

/*===========================================================================
**  GENERAL PURPOSE ASSEMBLY MACROS.
**=========================================================================*/

/** @cond
*/
#if defined(_ARM_ASM_)

        GBLS    current_node_name
        GBLS    current_node_type
        GBLA    current_node_aregcount
        GBLA    current_node_vregcount

/*===========================================================================

Name: pusha, popa
=============================================================================*/
/**
Readable name for push\n

MODE: Any\n
STATE: ARM\n

@return
None

@dependencies
None

@sideeffects
Registers Modified: sp
*/
        MACRO
        pusha   $stack, $reg
        str     $reg, [$stack, #-4]!
        MEND

/**
Readable name for pop\n

MODE: Any\n
STATE: ARM\n

@return
None

@dependencies
None

@sideeffects
Registers Modified: sp
*/
        MACRO
        popa    $stack, $reg
        ldr     $reg, [$stack], #4
        MEND

/*===========================================================================

Name: Entry_Node, Entry_Node_End
=============================================================================*/
/**
Entry Node\n
Defines an empty function prolog and epilog of a piece of assembly code.\n
This macro takes care of the following:\n
  - Exporting the entry point label\n
  - Creating a stack frame\n
  - Software stack checking\n

Defined Labels:\n
  "node_name" - The main thumb mode entry point of the function.\n
  "node_name"_end - A label marking the begining of the function epilog.\n
  
MODE: Any\n
STATE: ARM\n

@param[in] node_name The function name of the function to be defined.

@return
None

@dependencies
None


*/
        MACRO
        ENTRY_NODE $node_name
current_node_name       SETS    "$node_name"
        EXPORT  $node_name
        ALIGN
        ROUT
$node_name
        MEND
                                        ;  The end of an entry node.
/**
Entry Node End\n
Defines an empty function prolog and epilog of a piece of assembly code.\n
This macro takes care of the following:\n
  - Exporting the entry point label\n
  - Creating a stack frame\n
  - Software stack checking\n

Defined Labels:\n
  "node_name" - The main thumb mode entry point of the function.\n
  "node_name"_end - A label marking the begining of the function epilog.\n
  
MODE: Any\n
STATE: ARM\n

@return
None

@dependencies
None


*/
        MACRO
        ENTRY_NODE_END
$current_node_name._exit
current_node_name       SETS    ""
        MEND

/*===========================================================================

  Name: Leaf_Node, Leaf_Node_End
=============================================================================*/
/**
Leaf Node
Defines the function prolog and epilog of a function that makes no function 
calls.\n
This macro takes care of the following:\n
  - Exporting the entry point label.\n
  - Creating 16 and 32 bit entry points.\n
  - Creating a stack frame.\n
  - Software stack checking.\n
  
Defined Labels:\n
  "node_name" - The main thumb mode entry point of the function.\n
  "node_name"_32 - The ARM state entry point to the function.\n
  "node_name"_end - A label marking the begining of the function epilog.\n

MODE: Any\n
STATE: ARM and Thumb state entry points. Code runs in ARM state.\n

@param[in] node_name The function name of the function to be defined.

@return
None

@dependencies
None

*/
        MACRO
        LEAF_NODE $node_name
current_node_type       SETS    "Leaf"
        CODE16
        ENTRY_NODE $node_name
        bx      pc
        ALIGN
        CODE32
        EXPORT  $node_name._32
$node_name._32
        MEND
		
/**
Leaf Node End
Defines the function prolog and epilog of a function that makes no function 
calls.\n
This macro takes care of the following:\n
  - Exporting the entry point label.\n
  - Creating 16 and 32 bit entry points.\n
  - Creating a stack frame.\n
  - Software stack checking.\n
  
Defined Labels:\n
  "node_name" - The main thumb mode entry point of the function.\n
  "node_name"_32 - The ARM state entry point to the function.\n
  "node_name"_end - A label marking the begining of the function epilog.\n

MODE: Any\n
STATE: ARM and Thumb state entry points. Code runs in ARM state.\n

@return
None

@dependencies
None

*/
        MACRO
        LEAF_NODE_END $node_name
        ASSERT  "$current_node_type" = "Leaf"
        ENTRY_NODE_END $node_name
        bx     lr
current_node_type       SETS    ""
        MEND


/*===========================================================================

  Name: Leaf_Node_16, Leaf_Node_End_16
=============================================================================*/
/**
Leaf Node 16

Defines the function prolog and epilog of a Thumb mode only function that makes
no function calls. 

This macro takes care of the following:\n
  - Exporting the entry point label.\n
  - Creating a 16 bit entry point.\n
  - Creating a stack frame.\n
  - Software stack checking.\n

Defined Labels:\n
  "node_name" - The main thumb mode entry point of the function.\n
  "node_name"_end - A label marking the begining of the function epilog.\n
  
@param[in] node_name The function name of the function to be defined.

MODE: Any\n
STATE: Thumb\n

@return
None

@dependencies
None

*/
        MACRO
        LEAF_NODE_16 $node_name
current_node_type       SETS    "Leaf16"
        CODE16
        ENTRY_NODE $node_name
        MEND
/**
Leaf Node End 16

Defines the function prolog and epilog of a Thumb mode only function that makes
no function calls. 

This macro takes care of the following:\n
  - Exporting the entry point label.\n
  - Creating a 16 bit entry point.\n
  - Creating a stack frame.\n
  - Software stack checking.\n

Defined Labels:\n
  "node_name" - The main thumb mode entry point of the function.\n
  "node_name"_end - A label marking the begining of the function epilog.\n

MODE: Any\n
STATE: Thumb\n
  
@return
None

@dependencies
None

*/
        MACRO
        LEAF_NODE_END_16 $node_name
        ASSERT  "$current_node_type" = "Leaf16"
        ENTRY_NODE_END $node_name
        bx     lr
current_node_type       SETS    ""
        MEND

/**
Alternate Entry 16

Defines the function prolog and epilog of a Thumb mode only function that makes
no function calls. 

This macro takes care of the following:\n
  - Exporting the entry point label.\n
  - Creating a 16 bit entry point.\n
  - Creating a stack frame.\n
  - Software stack checking.\n

Defined Labels:\n
  "node_name" - The main thumb mode entry point of the function.\n
  "node_name"_end - A label marking the begining of the function epilog.\n
  
@param[in] node_name The function name of the function to be defined.

@return
None

@dependencies
None

*/
        MACRO
        ALTERNATE_ENTRY_16 $node_name
        ASSERT  "$current_node_type" = "Leaf16"
        EXPORT $node_name
$node_name
        MEND


/*===========================================================================

  Name: blatox
=============================================================================*/
/**
Calls a function from ARM state without having to know whether 
that function is ARM or Thumb state code.\n
 
MODE: Any\n
STATE: ARM. Can call either ARM or Thumb state functions.\n
 
@param[in] destreg The register that contains the address of the function 
to be called.

@return
None

@dependencies
None

@sideeffects
Registers Modified: lr
*/
        MACRO
        blatox     $destreg
        ROUT

        tst     $destreg, #0x01         /* Test for thumb mode call.  */

        ldrne   lr, =%1
        ldreq   lr, =%2
        bx      $destreg
1
        CODE16
        bx      pc
        ALIGN
        CODE32
2
        MEND

#endif   /* ARM_ASM  */

/** @endcond */
/** @} */ /* end_addtogroup utils_services */
#endif   /* ARMASM_H */
