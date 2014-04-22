/*=======================================================================
 Copyright (c) 2020  Qualcomm Technologies, Inc.  All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 ======================================================================*/

#ifndef __QAPI_TIMER_INTERNAL_H__
#define __QAPI_TIMER_INTERNAL_H__

#include "timer.h"
#include "txm_module.h"

/* This is the structure for the qapi timer table which holds the 
   timer pointer and module instance id for all the clients */

typedef struct
{
  timer_type* timer_ptr; 
  TXM_MODULE_INSTANCE* module_instance;
}qapi_timer_table_type;


#endif /*__QAPI_TIMER_INTERNAL_H__ */


