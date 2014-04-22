#ifndef ARM_H
#define ARM_H
/**
  @file arm.h
  @brief Macros and constants specific to the ARM processor.

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
/*==========================================================================

                         ARM Definitions Header File

DESCRIPTION
  Macros and constants specific to the ARM processor

Copyright (c) 1998-2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary. 
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$PVCSPath: L:/src/asw/COMMON/vcs/arm.h_v   1.0   Aug 21 2000 11:51:02   lpetrucc  $
$Header: //components/rel/core.tx/6.0/api/services/arm.h#1 $ $DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/15/11   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
11/09/10   EBR     Doxygenated File.
12/02/98    ms     Added P_IRQ_VECT and P_FIQ_VECT for T_MSM3
11/02/98    ms     Added P_INT0_VECT and P_INT1_VECT declarations.
07/10/98   jct     Revised for coding standard, removed unused code
01/01/98   bwj     Created

===========================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/

/** @addtogroup utils_services
@{ */
/** @name ARM Processor-Specific Macros
@{ */

/* Interrupts must be level 0 or higher to be serviced.
** Since we don't have any interrupts at level 0, no interrupts
** will be serviced.
**
** N.B.! Make sure we do not have any interrupts at level 0 including
** timer ( defaults to 0 ) and DMA interrupts.
**
** Account for pipeline delay of priority mask register by disabling
** interrupts around the disable instruction to avoid interrupts
** ( and subsequent tasks ) from running for an unknown length of time
** with all interrupts disabled.
*/
/**
  Uses the Priority Mask feature of the Intel<sup>&reg;</sup> 80186 processor 
  to turn off interrupts (PRIMASK_DISABLE()) and restore them to the previous 
  state (PRIMASK_ENABLE()). This scheme is not used anywhere else in the 
  AMSS code; there is no need to save and restore the interrupt priority 
  mask state.
 
  @return
  None.

  @dependencies
  None.
*/
#define PRIMASK_DISABLE()

/* Interrupts must be level 7 or higher to be serviced.
** Since level 7 is the lowest priority, all interrupts that
** are otherwise enabled will be serviced normally.
*/
/**
  Restores interrupts to the previous state. This macro is used in 
  conjunction with PRIMASK_DISABLE().
   
  @return
  None.

  @dependencies
  None.
*/
#define PRIMASK_ENABLE()


/*===========================================================================
MACRO SET_VECT

===========================================================================*/
/**
  Stores a specified ISR address at a specific interrupt vector.

  @param[in] vect_num Interrupt vector number.
  @param[in] ptr      Pointer to the ISR for vect_num. 

  @return
  None.

  @dependencies
  None.
*/
#define  SET_VECT( vect_num, ptr )   rex_set_interrupt_vector(vect_num, ptr)
/** Enables interrupts for the vect_num passed in. 
    
	@param[in] vect_num Interrupt vector number.
	
	@return
	None.
	
	@dependencies
	None.
*/
#define  ENABLE_INTERRUPT(vect_num)  rex_enable_interrupt(vect_num)
/** Disables interrupts for the vect_num passed in. 
  
    @param[in] vect_num Interrupt vector number.
	
	@return
	None.
	
	@dependencies
	None.
*/
#define  DISABLE_INTERRUPT(vect_num) rex_disable_interrupt(vect_num)

/* -----------------------------------------------------------------------
**                  Peripheral Chip Select Registers 
** ----------------------------------------------------------------------- */

#define P_IRQ_VECT   0  /**< IRQ peripheral chip select registers. */
#define P_FIQ_VECT   1  /**< FIQ peripheral chip select registers. */

/** @cond 
*/ 
#define P_PCS_BASE   0
#define IO_BASE      0
/** @endcond */

/** @} */ /* end_name_group ARM Processor-Specific Macros */
/** @} */ /* end_addtogroup utils_services */
#endif /* END ARM_H */
