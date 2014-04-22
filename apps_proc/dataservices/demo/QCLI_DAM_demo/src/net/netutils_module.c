/*!
  @file
  netutils_module.c

  @brief
  Networking stack related utilities implementation.
*/
/*===========================================================================
  Copyright (c) 2017  by Qualcomm Technologies, Inc.  All Rights Reserved.
 
  Qualcomm Technologies Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.
 ======================================================================*/


#include <stdarg.h>
#include "netutils.h"
#include "qapi_timer.h"
#include "qurt_timetick.h"
#include "log_intf_module.h"

#include "qapi_diag.h"

/* TEMP */ 
void qosal_intr_disable(void);
void qosal_intr_enable(void);

/*****************************************************************************
 * add item to the q's tail
 *****************************************************************************/
void enqueue(QUEUE_T *q, void *item)
{
    qp elt = (qp)item;

//    qosal_intr_disable(); 

    elt->qe_next = NULL;
    if (q->q_head == NULL)
    {
        q->q_head = elt;
    }
    else
    {
        q->q_tail->qe_next = elt;
    }

    q->q_tail = elt;
    ++(q->q_len);

//    qosal_intr_enable();

    return;
}

/*****************************************************************************
 * remove item from the q's head
 *****************************************************************************/
void * dequeue(QUEUE_T *q)
{
    qp elt;        /* elt for result */

//    qosal_intr_disable();

    if ((elt = q->q_head) != NULL)  /* queue is not empty */
    {
        q->q_head = elt->qe_next;   /* unlink */
        elt->qe_next = NULL;        /* avoid dangling pointer */
        if (q->q_head == NULL)      /* queue is empty */
        {
            q->q_tail = NULL;       /* update tail pointer too */
        }
        q->q_len--;                 /* update queue length */
    }

//    qosal_intr_enable();

    return ((void*)elt);
}

/*****************************************************************************
 *****************************************************************************/
uint32_t app_get_time(time_struct_t *time)
{

    uint64_t ticks;
    uint32_t ms;

    ticks = qurt_timer_get_ticks();
    ms = (uint32_t)qurt_timer_convert_ticks_to_time(ticks, QURT_TIME_MSEC);

    if (time)
    {
        time->seconds = ms / 1000;
        time->milliseconds = ms % 1000;
    }

    return ms;
}

/*****************************************************************************
 *****************************************************************************/
void app_msec_delay(uint32_t ms)
{
    qapi_Timer_Sleep(ms,QAPI_TIMER_UNIT_MSEC,true);
    return;
}

void app_usec_delay(uint32_t us)
{
  qapi_Timer_Sleep(us,QAPI_TIMER_UNIT_USEC,true);
    return;
}

void app_tick_delay(uint32_t us)
{
  qapi_Timer_Sleep(us,QAPI_TIMER_UNIT_TICK,true);
    return;
}
