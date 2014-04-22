/**
  @file qurt_txm_uid.h 
  @brief  Prototypes of QuRT TX Module Unique ID related APIs

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2019, 2020 by Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================*/

/*=============================================================================
                                 CONSTANTS AND MACROS
=============================================================================*/ 

#define KERNEL_MAGIC_MODULE_INSTANCE 0xABABABAB  /*  magic module instance value that is returned when 
                                                     qurt_txm_module_instance_get is called from an 
													 asynchronous context.  */
#define KERNEL_MAGIC_UID 0xBABABABA				 /*  magic module UID that is returned when qurt_txm_module_uid_get 
                                                     is called from an aynchronous context(the module instance passed is
													 KERNEL_MAGIC_MODULE_INSTANCE). */
/*=====================================================================
 Functions
======================================================================*/

/**@ingroup func_qurt_txm_module_uid_check
	This function extracts the Module Unique ID from given Module handle, 
	scans Module ID registry to check the uniqueness of the Module ID and 
	returns the status.
   
  @datatypes
  #TXM_MODULE_INSTANCE
 
  @param[in] module_instance  Module instance pointer.

  @return
  Module Unique ID check result \n
	QURT_EOK:      Success - This Module has unique Module ID. \n
	QURT_EINVALID: Module handle invalid or invalid Module ID. \n
	QURT_EFAILED:  Failure. ID found to be non-unique (i.e. repeated/duplicate) \n
	               for the system or number of concurrent Modules exceeds the limit. \n

  @dependencies
    Module to be loaded into the system and MMU mapping done.
*/
int qurt_txm_module_uid_check (TXM_MODULE_INSTANCE * module_instance);

/**@ingroup func_qurt_txm_module_instance_get
	This function retrieves module instance handle for current thread,
	which is associated with current module, if being called from given
	module's thread context.
	
	Returns magic ID KERNEL_MAGIC_MODULE_INSTANCE, if being called from
	out-of-context i.e. from native Kernel thread outside of Module's 
	context.
   
  @datatypes
  #TXM_MODULE_INSTANCE
 
  @param[in/out] module_instance:  Pointer to a pointer variable to hold module instance handle/pointer.

  @return
	QURT_EOK:      Successfully retrieved Module instance handle. \n
	QURT_EINVALID: Input arguments found to be invalid. \n
	QURT_EFAILED:  Operation failed due to internal error. 

  @dependencies
    None.
*/
int qurt_txm_module_instance_get(TXM_MODULE_INSTANCE **module_instance);

/**@ingroup func_qurt_txm_module_uid_get
	Retrieves Module Unique ID for specified Module instance handle, if handle 
	corresponds to valid Module. Returns magic ID KERNEL_MAGIC_MODULE_UID, 
	if called with KERNEL_MAGIC_MODULE_INSTANCE as Module instance handle.
   
  @datatypes
  #TXM_MODULE_INSTANCE
 
  @param[in] module_instance : Module instance pointer.
  @param[in/out] module_id   : pointer to variable to hold module unique ID value.

  @return
	QURT_EOK:      Successfully retrieved Module unique ID. \n
	QURT_EINVALID: Input arguments found to be invalid. \n
	QURT_EFAILED:  Operation failed due to internal error. 

  @dependencies
    None.
*/
int qurt_txm_module_uid_get(TXM_MODULE_INSTANCE *module_instance, UINT *module_uid);



