
#ifndef QURT_TXM_MMU
#define QURT_TXM_MMU

/*======================================================================
                        qurt_txm_mmu.h 

GENERAL DESCRIPTION
  MMU setup routine for Threadx Module

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2018-2019  by Qualcomm Technologies, Inc.  All Rights Reserved.
 ======================================================================*/
/*======================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *  
 *
 *
 * when         who     what, where, why
 * ----------   ---     ------------------------------------------------
 * 12/27/2017   AJ      MMU setup routine
 ======================================================================*/

#include "txm_module.h"

/**@ingroup func_qurt_txm_setup_module_mmu

   API to setup MMU for given user module
  
   @param[in] module_pointer	Module instance pointer

  
   @return             
	 0 : Success
	-1 : FAILED
   
   @dependencies
   None.
   
 */
int qurt_txm_setup_module_mmu (TXM_MODULE_INSTANCE *module_ptr);

/**@ingroup func_qurt_txm_clear_module_mmu

   API to clear MMU for given user module
  
   @param[in] module_pointer	Module instance pointer

  
   @return             
	 QURT_EOK : Success
	 QURT_EINVALID : Invalid argument 
	 QURT_EFAILED : Operation failed
   
   @dependencies
   None.
   
 */
int qurt_txm_clear_module_mmu(TXM_MODULE_INSTANCE *module_ptr);

#endif /* QURT_TXM_MMU */





