
/**
  @file qurt_shared_memory.h 
  @brief  Prototypes of qurt shared memory API

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2017  by Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================*/


/**@ingroup func_qurt_shared_memory_get
  Function to provide a shared memory to modules.
  
  @param[in]  module_ptr     Module pointer
  @param[in/out]  shMemVa   	 Shared memory start address
  @param[in/out]  shMemSz   	 Shared memory size                   
                        
  @return
  Status (FAIL/PASS)

  @dependencies
  None.

 */ 


int qurt_shared_memory_get(void *module_ptr,unsigned long *shMemVa,unsigned long *shMemSz);

   

