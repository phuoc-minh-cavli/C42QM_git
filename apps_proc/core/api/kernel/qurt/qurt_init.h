#ifndef QURT_INIT_H
#define QURT_INIT_H

/**
  @file qurt_init.h 
  @brief  Prototypes of QuRT Initialization API 

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2013-2015  by Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================*/

#include "qurt_types.h"

/*=============================================================================
                                 CONSTANTS AND MACROS
=============================================================================*/ 

#define QURT_INFO_OBJ_SIZE_BYTES    64
extern unsigned int QtmrAddress;			/* QTMR HW clock address */
extern unsigned int isTraceEnabled;			

/*=============================================================================
                                    TYPEDEFS
=============================================================================*/
/** @addtogroup init_data_types
@{ */

typedef struct qurt_data_s
{
  /* Pointer to the heap used by RTOS */
  void *hPtr;
  /* Reserved pointer for future use */
  void *rPtr;
} qurt_data_t;

/** QuRT Info type */
typedef struct qurt_info  /* 8 byte aligned */
{
   unsigned long long _bSpace[QURT_INFO_OBJ_SIZE_BYTES/sizeof(unsigned long long)];
}qurt_info_t;

/** @} */ /* end_init data_types */

/*=============================================================================
                                    FUNCTIONS
=============================================================================*/
/**@ingroup func_qurt_init
  Initializes QuRT Library  
  
  @param[in,out] arg Pointer to initialization parameter .

  @return
  None.

  @dependencies
  None.
  
*/
int qurt_init (void *arg);

/**@ingroup func_qurt_timetick_init
	
	Initializes the qurt's system timer handle.
	  
	@param
	None.
	  
	@return			   
	None
	   
	@dependencies
	None.
	   
*/
void qurt_timetick_init(void);


/**@ingroup func_qurt_system_register_bsp_hook
  BSP hook that need to be set before qurt_init gets invoked
  
  @param[in] entrypoint Pointer to BSP initialization function .

  @return
  None.

  @dependencies
  None.
  
*/
void qurt_system_register_bsp_hook(void (*entrypoint) (void));

#endif /* QURT_INIT_H */

