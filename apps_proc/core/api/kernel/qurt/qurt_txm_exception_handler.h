#ifndef QURT_TXM_EXCEPTION_HANDLER_H
#define QURT_TXM_EXCEPTION_HANDLER_H 

/**
  @file qurt_txm_exception_handler.h 

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2022  by Qualcomm Technologies, Inc.  
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 ======================================================================*/
/*======================================================================
 *
 *						 EDIT HISTORY FOR FILE
 *
 *	 This section contains comments describing changes made to the
 *	 module. Notice that changes are listed in reverse chronological
 *	 order.
 *
 *	 This file contains DAM exception handling declarations
 *
 *
 * when 				who 		what, where, why
 * ---------- 	        --- 		-------------------------------------
 * 2022-10-30           VU      	Initial version

 ======================================================================*/
 

#ifdef __cplusplus
extern "C" {
#endif

#include "tx_api.h"
#include "txm_module.h"
#include "qurt_error.h"


typedef UINT (* pFn_module_err_cb_t)(ULONG arg1,ULONG arg2,ULONG arg3,ULONG arg4 );

typedef struct qurt_txm_module_exception_info_t{
UINT r0;   /* Registers R0-R12 */
UINT r1;
UINT r2;
UINT r3;
UINT r4;
UINT r5;
UINT r6;
UINT r7;
UINT r8;
UINT r9;
UINT r10;
UINT r11;
UINT r12;
UINT SP;  /* Stack Pointer */
UINT LR;  /* Link Register */
UINT PC;  /* Program Counter */
UINT spsr;   /* Program status register */
UINT fault_address;  /* The address that caused the crash */
CHAR crashed_module_thread_name[QURT_THREAD_ATTR_NAME_MAXLEN]; /* Name of the thread that crashed */ 
QurtExcnCause cause; /* Type of excpetion */
}qurt_txm_module_exception_info;


/**@ingroup func_ qurt_txm_module_exception_notify_cb_register
   Registers a callback with the exception handler. Registered callback function 
   would get invoked in case any thread associated with given module meets an exception.

  @datatypes
  #TXM_MODULE_INSTANCE \n
  #void \n
  #ULONG

  @param[in] module_instance: pointer to the module structure
  @param[in] cb_function: pointer to the callback function that needs to be invoked
  @param[in] arg1-arg4: arguments to be passed while invoking the cb_function 
                                                
  @return
  QURT_EOK -- Callback successfully registered.
  QURT_EINVALID -- One of the input arguments found to be invalid.

  @dependencies
  Module should be loaded prior to this.
  
  @note : within cb function, client should not invoke any blocking calls(like mutex lock, signal wait, thread sleep, 
        queue send/receive). Recommend to have minimal functionality since this is not a handler function and only a notify function. Any blocking operation or any heavy operation like restarting the module can be performed after spawning a new thread and in the newly created thread's context. Some of the possible operations within callback function are:
            - spawn a thread with priority > 1
            - signal a server thread (Kernel space) having priority > 1
            - take dump or add logs.
*/

INT qurt_txm_module_exception_notify_cb_register (TXM_MODULE_INSTANCE* module_instance, pFn_module_err_cb_t cb_function, ULONG arg1, ULONG arg2, ULONG arg3, ULONG arg4);


/**@ingroup func_ qurt_txm_module_get_exception_info
   Extracts the relevant details regarding the module crash. This API gives details 
   on last exception as captured in given module’s error info structure.

  @datatypes
  #TXM_MODULE_INSTANCE \n
  #qurt_txm_module_exception_info

  @param[in] module_instance: pointer to the module structure
  @param[in] error_info: pointer to error info
                                                
  @return
  QURT_EOK -- successfully extracted the error info.
  QURT_EINVALID -- One of the input arguments found to be invalid.
  QURT_EFAILURE -- no module crash has happened or failure in address validation

  @dependencies 
  Module loader should register an exception callback with the kernel and the callback should be invoked. 
  Recommended to invoke this function from the callback.

  @note
  Callback function should return the control as soon as possible to avoid impacting 
  exception handling at system level and impact to other Modules and Kernel threads 
  running in the system.

*/
INT qurt_txm_module_get_exception_info (TXM_MODULE_INSTANCE* module_instance, qurt_txm_module_exception_info * error_info) ;

#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif

#endif /* QURT_TXM_EXCEPTION_HANDLER_H */
