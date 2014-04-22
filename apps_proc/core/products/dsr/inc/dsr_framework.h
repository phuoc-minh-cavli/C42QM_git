#ifndef __DSR_FRAMEWORK_H__
#define __DSR_FRAMEWORK_H__
/*============================================================================
  @file dsr_framework.h

  Function and data structure declarations for DSR (Deferred Service Routine)
  framework.


                Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/* $Header: //components/rel/core.tx/6.0/products/dsr/inc/dsr_framework.h#1 $ */

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/
#include "com_dtypes.h"
#include "dsr.h"

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/
#define DSR_THREAD_NAME_IST 			"DSR_ISR"
#define DSR_THREAD_ISR_COUNT			4
#define DSR_TASKNAME_SIZE				16

#define DSR_THREAD_NAME_WORKLOOP 	"DSR_WORKLOOP"
#define DSR_THREAD_WORKLOOP_COUNT	2

#define DSR_GET_THREAD_LIST(x)			(x == DSR_CONTEXT_ISR) ? dsr_thread_list_isr : dsr_thread_list_wklp
#define DSR_GET_OBJECT_LIST(x)			(x == DSR_CONTEXT_ISR) ? dsr_obj_list_isr : dsr_obj_list_wklp

#define DSR_GET_NUM_THREADS(x)			(x == DSR_CONTEXT_ISR) ? dsr_num_isr_threads : dsr_num_wklp_threads

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/

/**
 * @brief Initializes the DSR framework
 *
 * @details This function initializes the DSR framework. It should be
 *          called once in platform.c
 *
 * @attention Clients should not call this function.
 *
 * @return None
 */
void dsr_init_framework(void);

/**
 * @brief Whether or not a DSR is pending
 *
 * @details This function indicates whether or not any DSRs are currently
 *          enqueued in the list.
 *
 * @attention Clients should not call this function.
 *
 * @param[in]  context    Context depicting DSR for ISR or Workloop 
 *
 * @return TRUE is a DSR is pending; otherwise, FALSE
 */
boolean dsr_is_dsr_pending(dsr_context_t context);

#endif /* #ifndef __DSR_FRAMEWORK_H__ */

