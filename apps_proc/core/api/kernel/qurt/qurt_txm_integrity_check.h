#ifndef QURT_TXM_INTEGRITY_CHECK_H
#define QURT_TXM_INTEGRITY_CHECK_H

/**
  @file qurt_txm_integrity_check.h
  @brief  Prototype of kernel APIs for module integrity check 

  - Performs the integrity check on the module loaded

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2019 by Qualcomm Technologies, Inc.  All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 ======================================================================*/

#include "txm_module.h"

/*======================================================================
                          CONSTANTS AND MACROS
=======================================================================*/


/*=====================================================================
                          Functions
======================================================================*/

/**@ingroup func_qurt_txm_integrity_check
  @xreflabel{sec:qurt_txm_integrity_check}
  Performs the integrity check on the module loaded and returns the integrity check result.
 
  @datatypes
  #TXM_MODULE_INSTANCE
 
  @param[in] module_ptr  Pointer to the module instance created.
 
  @return
  QURT_EOK -- Integrity check successful.\n
  QURT_EFAILED -- Integrity check failed.\n
  QURT_EINVALID -- Invalid format.
 
  @dependencies
  Should be called after loading the module.
 */

int qurt_txm_integrity_check(TXM_MODULE_INSTANCE *module_ptr);

/**@ingroup func_qurt_txm_check_if_MBN_type
  @xreflabel{sec:qurt_txm_check_if_MBN_type}
  This API checks if the image loaded is of type MBN
  
  @datatypes
  #TXM_MODULE_INSTANCE
  
  @param[in]  module_ptr  Pointer to the module instance created.
  
  @param[in]  image   		points to name of the file loaded in case of txm_module_manager_file_load.
							points to the buffer containing the image in case of txm_module_manager_memory_load.
  @param[in]  imageIsFile   TRUE if txm_module_manager_file_load is used to load image.	
							FALSE if txm_module_manager_memory_load is used to load image.

  @return
  QURT_EOK -- Image is of MBN type.\n
  QURT_EINVALID -- Image is not of MBN/ELF type or image type check failed.

  @dependencies
  None.
*/
int qurt_txm_check_if_MBN_type(TXM_MODULE_INSTANCE *module_ptr,void *image, USHORT imageIsFile);

#endif /* QURT_TXM_INTEGRITY_CHECK_H */
