#ifndef QURT_ERROR_H
#define QURT_ERROR_H

/**
  @file qurt_error.h 
  @brief Error results- QURT defines a set of standard symbols for the error result values. This file lists the
  symbols and their corresponding values.

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc..
 ======================================================================*/

#include "qurt_types.h"

/** @addtogroup chapter_error
@{ */

/*=====================================================================
Constants and macros
======================================================================*/
#define QURT_EOK                             0  /**< Operation successfully performed. */
#define QURT_EFATAL                         -1  /**< FATAL error which should never happen. */
#define QURT_EVAL                           -2  /**< Wrong values for parameters. The specified page does not exist. */
#define QURT_EMEM                           -3  /**< Not enough memory to perform operation.*/

#define QURT_EINVALID                       -4  /**< Invalid argument value, invalid key. */ 
#define QURT_EFAILED_TIMEOUT                -5  /**< time out. */
#define QURT_EUNKNOWN                       -6  /**< Defined but never used in BLAST. */
#define QURT_EFAILED                        -7  /**< Operation failed. */
#define QURT_ENOMSGS                        -8  /**< Message queue empty. */

#define QURT_ENOTALLOWED                    -9  /**< Operation not allowed. */
#define QURT_ETLSAVAIL                      -10 /**< No free TLS key is available. */
#define QURT_ETLSENTRY                      -11 /**< TLS key is not already free. */
#define QURT_ENOTHREAD                      -12 /**< Thread no longer exists. */

#define QURT_EREMAP                         -13 /**< Remap an existing mapping. */
#define QURT_ENOMAP                         -14 /**< Mapping doesn't exist. */

extern uint32 crashed_module_thrd;			/*  crashed module thread pointer,if any */

typedef enum _QurtExcnCause{
	ERR_UNDEFINED_INSTRUCTION	= 2,
	ERR_ABORT_DOMAIN_FAULT		= 3,
	ERR_ABORT_PERMISSION_FAULT	= 4,
	ERR_ABORT_TRANSLATION_FAULT = 5,
	ERR_ABORT_ALIGNMENT_FAULT	= 6,
	ERR_PRECISE_EXTERNAL_ABORT	= 6,
	ERR_ABORT_TLBMISS_FAULT 	= 7,
	ERR_ABORT_DEBUG_EXCEPTION	= 8,
	ERR_ABORT_ICACHE_FAULT		= 9,
	ERR_ABORT_PAGETABLE_FAULT	= 10,
	ERR_ABORT_TLBLOCK_FAULT 	= 11,
	ERR_COPROCESSOR_DATA_ABORT	= 12,
	ERR_ABORT_PARITY_FAULT		= 13,
	ERR_ABORT_UNDEFINED 		= 14
} QurtExcnCause ;


typedef enum _QurtExcnType{
	QURT_NO_EXCEPTION = 0,
	QURT_FIRST_EXCEPTION = 1,
	QURT_SECOND_EXCEPTION = 2
} QurtExcnType ;


/*=============================================================================
                        TYPEDEFS
=============================================================================*/

/* struct QURT_global_regs {
  
} */

struct QURT_local_regs {
  uint32 r0;
  uint32 r1;
  uint32 r2;
  uint32 r3;
  uint32 r4;
  uint32 r5;
  uint32 r6;
  uint32 r7;
  uint32 r8;
  uint32 r9;
  uint32 r10;
  uint32 r11;
  uint32 r12;
  uint32 sp;
  uint32 lr;
  uint32 pc;
  uint32 cpsr;
};

struct QURT_error {
  QurtExcnType status;
  QurtExcnCause cause;
  uint32 thread_id;
  uint32 fault_address;
//  struct QURT_global_regs global_regs;
  struct QURT_local_regs local_regs;
};


/*=============================================================================
                                    FUNCTIONS
=============================================================================*/

/**@ingroup func_qurt_exception_wait  
  Registers the program exception handler.
  This function assigns the current thread as the QuRT program exception handler and suspends the
  thread until a program exception occurs.

  When a program exception occurs, the thread is awakened with error information
  assigned to this operations parameters.

  @param[out]  ip      Pointer to the instruction memory address where the exception occurred.
  @param[out]  sp      Stack pointer of the exception thread.
  @param[out]  badva   Pointer to the virtual data address where the exception occurred.
  @param[out]  cause   Pointer to the QuRT error result code.   

  @return
  Registry status: \n
  - Thread identifier -- Handler successfully registered. \n
  - QURT_EFATAL -- Registration failed.

  @dependencies
  None.
*/
unsigned int qurt_exception_wait (unsigned int *ip, unsigned int *sp,
                                  unsigned int *badva, unsigned int *cause);


#endif /* QURT_ERROR_H */
