#ifndef QURT_TXM_QAPI_FWK_H
#define QURT_TXM_QAPI_FWK_H 

/**
  @file qurt_txm_qapi_fwk.h 

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2017 - 2020  by Qualcomm Technologies, Inc.  All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 ======================================================================*/
/*======================================================================
 *
 *											 EDIT HISTORY FOR FILE
 *
 *	 This section contains comments describing changes made to the
 *	 module. Notice that changes are listed in reverse chronological
 *	 order.
 *
 *	 This file contains QAPI framework declarations
 *
 *
 * when 				who 		what, where, why
 * ---------- 	--- 		------------------------------------------------
 * 2017-01-20   c_sbegum	Add Header file, initial QAPI Framework 

 ======================================================================*/
 

#ifdef __cplusplus
extern "C" {
#endif

#include "tx_api.h"
#include "qapi_txm_base.h"

// Function pointer typedef for QAPI handler at the Kernel side
typedef UINT (* pFn_t)(UINT id, UINT a1, UINT a2, UINT a3, UINT a4, UINT a5, UINT a6, UINT a7, UINT a8, UINT a9, UINT a10, UINT a11, UINT a12);

// Function to register QAPI handler with QuRT QAPI framework
UINT qurt_qapi_register(UINT id_start, UINT num_ids, void *cb);

/* Helper function to configure and register user space callback function 
   with TX Module manager.
   This function is to be called every time when Kernel space driver receives 
   the API from user space that need callback notificaiton at user space level

   params: 
	 [in] type:       Pre-allocated/assigned callback ID/type
	 [in] object_ptr: Object associated with callback. Can be driver handle, object handle etc.
     [in] id:         ID for this callback being configured. Currently unused.
	 [in] app_cb:     Application callback (User space callback function in the Module)
	 [in] app_cb_dispatcher: Dispatcher that will get invoked when Request Q receives cb request 
	                         from Kernel space
	                         Dispatcher then actually invokes/dispatches actual
							 *app_cb* function.
	 [in/out] manager_cb: Pointer to Module manager callback. Pointer would be updaetd with
						  function pointer of Module manager handler associated with this 
						  callback type/instance.
	 [in] reserved:  NOTE: Currently unused.
	 
   return:
      0xffffffff:	Error
	  0:			Success
*/
UINT qurt_qapi_callback_configure(UINT type, VOID *object_ptr, ULONG id,
                                  VOID *app_cb, VOID *app_cb_dispatcher,
								  VOID **manager_cb, VOID *reserved);

#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif

#endif /* QURT_TXM_QAPI_FWK_H */
