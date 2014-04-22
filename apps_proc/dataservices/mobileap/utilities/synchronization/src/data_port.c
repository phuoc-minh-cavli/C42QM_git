/***************************************************************************************************
    @file
    data_port.c

    Copyright (c) 2019 Qualcomm Technologies, Inc.
    All Rights Reserved.
    Confidential and Proprietary - Qualcomm Technologies, Inc.
***************************************************************************************************/


#include <stdlib.h>
#include "string.h"
#include "qurt_thread.h"
#include "qurt_mutex.h"
#include "qurt_timer.h"
#include <stdint.h>
#include "data_port.h"
#include "data_util_synchronization.h"

#ifdef MOB_TX
#include "offtarget_stubs.h"
#endif

int qcmap_pthread_set_attr
(
  qurt_thread_attr_t *attr,
  char *pname,
  unsigned priority,
  unsigned stack_size
 )
{
  qurt_thread_attr_init(attr); 
  qurt_thread_attr_set_name(attr, pname);
  qurt_thread_attr_set_priority(attr, priority);
  qurt_thread_attr_set_stack_size(attr, stack_size);
  return 0;
}


/*===========================================================================
  FUNCTION  qcmap_pthread_cond_wait_timed
===========================================================================*/
/*!
@brief
  Wrapper over the Qurt API for waiting over a signal object

@return
  int - A 32-bit word with current signal masks being waited, set.

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/

int qcmap_pthread_cond_wait_timed
(
    qcmap_pthread_cond_t *cond,
    qcmap_pthread_mutex_t *mutex,
    int secs
)
{
    int ret = 0;
    uint32 signal = 0;
    qurt_time_t ticks;

    ticks = qurt_timer_convert_time_to_ticks(secs, QURT_TIME_MSEC);
    qurt_mutex_unlock(mutex);
    ret = qurt_signal_wait_timed_ext(cond, CMDQ_SIGNAL_MASK, QURT_SIGNAL_ATTR_WAIT_ANY, &signal, ticks);
    qurt_mutex_lock(mutex);
    return ret;
}


