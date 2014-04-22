#ifndef QURTSTORAGECFG_H
#define QURTSTORAGECFG_H
/*
  ===========================================================================
*/
/**
  @file qurt_storage_cfg.h
  @brief  This public api has declaration of qurt_txm_module_set_storage_type 
		to set the storage_type for module read and has macro for EFS and NAND
		
  ===========================================================================

*/

/*
  ===========================================================================

  Copyright (c) 2019 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR

  ===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/16/19   ahemanth     Initial
  ===========================================================================
*/
#include "txm_module.h"
#include "flash_ftl.h"
#include "fs_public.h"
#include "fx_api.h"
#include "qurt_error.h"
#include "qurt_memory.h"

/*======================================================================
                          CONSTANTS AND MACROS
=======================================================================*/

/* macro for storage_type EFS and NAND*/

#define  EFS 									0
#define  NAND									0x11A11D


/*=====================================================================
                          Functions
======================================================================*/

/**@ingroup func_qurt_txm_module_set_storage_type
  @xreflabel{sec:qurt_txm_module_set_storage_type}
  This function sets the storage type for the Module associated with given Module instance handle. 
  Specified storage type would be used while accessing the Module image during Module load.
  
  EFS would be considered as default storage type and Module access would be expected as EFS based if 
  client (module Loader) do not explicitly call this API to set required storage type.
  
  @datatypes
  #TXM_MODULE_INSTANCE
 
  @param[in] module_ptr  Pointer to the module instance created.
  @param[in] storage_type (Currently supported values: EFS (for EFS) or NAND (for NAND partition)
 
  @dependencies
  TXM_MODULE_INSTANCE should be allocated and instance should be passed to the function.
 */
INT qurt_txm_module_set_storage_type(TXM_MODULE_INSTANCE *module_ptr,
									  UINT storage_type);

#endif
