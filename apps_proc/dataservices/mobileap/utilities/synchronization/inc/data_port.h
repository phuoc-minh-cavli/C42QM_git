/***************************************************************************************************
    @file
    data_port.h

    Exports standard utilities.

    Copyright (c) 2019 Qualcomm Technologies, Inc.
    All Rights Reserved.
    Confidential and Proprietary - Qualcomm Technologies, Inc.
***************************************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "qurt_thread.h"
#include "qurt_mutex.h"
#include "qurt_timer.h"
#include "errno.h"

#define CMDQ_SIGNAL_MASK         0x00000001


typedef qurt_mutex_t             qcmap_pthread_mutex_t;
typedef qurt_thread_t            qcmap_pthread_t;
typedef qurt_signal_t            qcmap_pthread_cond_t;


/*===========================================================================
  FUNCTION  ds_platform_pthread_set_attr
===========================================================================*/
/*!
@brief
  Wrapper over the Qurt API for setting thread attributes

@return
  int - Zero value

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/

int qcmap_pthread_set_attr
(
  qurt_thread_attr_t *attr,
  char *pname,
  unsigned priority,
  unsigned stack_size
 );


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
    qcmap_pthread_cond_t  *cond,
    qcmap_pthread_mutex_t *mutex,
    int                         secs
);

