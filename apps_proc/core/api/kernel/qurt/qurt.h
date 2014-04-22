#ifndef QURT_H
#define QURT_H 

/**
  @file qurt.h 
  @brief  Contains kernel header files which provides kernel OS API functions, constants, and 
  definitions 

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2016  by Qualcomm Technologies, Inc.  All Rights Reserved.
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
 *	
 *
 *
 * when 				who 		what, where, why
 * ---------- 	--- 		------------------------------------------------
 * 2011-02-25 	op			Add Header file
   2012-12-16   cm          (Tech Pubs) Edited/added Doxygen comments and markup.
 ======================================================================*/
 

#ifdef __cplusplus
extern "C" {
#endif

#include "qurt_consts.h"
#include "qurt_error.h"
#include "qurt_init.h"
#include "qurt_interrupt.h"
#include "qurt_memory.h"
#include "qurt_mutex.h"
#include "qurt_pipe.h"
//#include "qurt_power.h"
#include "qurt_signal.h"
#include "qurt_system.h"
#include "qurt_thread.h"
#include "qurt_timer.h"
#include "qurt_types.h"

#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif

#endif /* QURT_H */
