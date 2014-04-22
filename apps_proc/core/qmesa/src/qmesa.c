/****************************************************************************************
*                                                                                       *
*    QMESA                                                                              *
*                                                                                       *
*   GENERAL DESCRIPTION                                                                 *
*        This file contains QMESA Module loader code.                                   *
*                                                                                       *
*   Copyright (c) 2018 by Qualcomm Technologies, Incorporated.  All Rights Reserved.    *
*    Qualcomm Confidential and Proprietary                                              *
*                                                                                       *
****************************************************************************************/
  
#include <string.h>
#include <stdio.h>
#include <rcinit.h>
#include "comdef.h"

#include "tx_api.h"
#include "txm_module.h"
#include "qurt_memory.h"
#include "qurt_system.h"
#include "qurt_thread.h"
#include "qurt_error.h"

#include "fs_public.h"
#include "fs_sys_types.h"

#include "qurt_shared_memory.h"
#include "qurt_txm_mmu.h"

#define QMESA_MODULE_FILE                        "/core/qmesa.bin"

TXM_MODULE_INSTANCE   qmesa_module;

//----------------------------------------------------------------------------
// Function: qmesa_init
// Description: Entry function for qmesa module loader.
//----------------------------------------------------------------------------
void qmesa_init(void *arg)
{
  int ret               = 0;
  
  ret = txm_module_manager_file_load(&qmesa_module, "qmesa", NULL, QMESA_MODULE_FILE);
  if (ret != TX_SUCCESS)
    return;
  
#ifdef TX_DAM_QC_CUSTOMIZATIONS
ret = qurt_txm_setup_module_mmu(&qmesa_module);

if(ret != QURT_EOK)
	 return;
#endif

  /* Start the module.  */
  ret = txm_module_manager_start(&qmesa_module);
  
  if (ret != TX_SUCCESS)
    return;
  
}


