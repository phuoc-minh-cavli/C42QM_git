/*!
  @file
  netutils.c

  @brief
  Networking stack related suporting API's implementation.
*/
/*===========================================================================
  Copyright (c) 2017 - 2019  by Qualcomm Technologies, Inc.  All Rights Reserved.
 
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


/* Copyright (c) 2011-2019.  Qualcomm Atheros, Inc.
All rights reserved.

Qualcomm Atheros Confidential and Proprietary.

*/

#include <stdarg.h>
#include "qurt_types.h"
#include "qurt_thread.h"
#include "qurt_timer.h"
#include "netutils.h"
#include "timer.h"
#include "log_intf.h"
#if defined(MINI_QCLI) || !defined(ECM_ENABLE)
#include "fs_public.h"
#endif

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
   timer_sleep(ms,T_MSEC,TRUE);
    return;
}
void app_usec_delay(uint32_t us)
{
   timer_sleep(us,T_USEC,TRUE);
    return;
}

void app_tick_delay(uint32_t us)
{
   timer_sleep(us,T_TICK,TRUE);
    return;
}

#define MAX_DIAG_LOG_MSG_SIZE 512
void m2m_stub_log
(
  const char* file_name, uint32_t line_number, uint32_t mask,
  const char *fmt, 
  ...
)
{
  va_list ap;
  char buf[MAX_DIAG_LOG_MSG_SIZE]; 
  msg_const_type xx_msg_const;
  
  xx_msg_const.desc.line    = line_number;
  xx_msg_const.desc.ss_id   = MSG_SSID_LINUX_DATA;
  xx_msg_const.desc.ss_mask = mask;
  xx_msg_const.fmt          = "%s";
  xx_msg_const.fname        = file_name;

  va_start( ap, fmt );
  vsnprintf( buf, (size_t)MAX_DIAG_LOG_MSG_SIZE, fmt, ap );
  va_end( ap );

  qapi_msg_sprintf(&xx_msg_const, 1, buf);
}
#if defined(MINI_QCLI) || !defined(ECM_ENABLE)
/***************************************************************************************************
    @function
    data_getline

    @implementation detail
    None.
***************************************************************************************************/

int data_getline(char **lineptr, size_t *n, int stream) 
{
    char *bufptr = NULL;
    char *p = bufptr;
    size_t size;
    char c;
	int len=0;

    if (lineptr == NULL || stream < 0 || n == NULL ) 
	{
    	return -1;
    }

    bufptr = *lineptr;
    size = *n;

    len = efs_read(stream, &c, 1);
    if ( len == 0) {
    	return -1;
    }
    if (bufptr == NULL) 
	{
    	bufptr = malloc(128);
    	if (bufptr == NULL) 
		{
    		return -1;
    	}
    	size = 128;
    }
    p = bufptr;
    while(len != 0) 
	{
    	if ((p - bufptr) > (size - 1)) 
		{
    		size = size + 128;
    		bufptr = realloc(bufptr, size);
    		if (bufptr == NULL) {
    			return -1;
    		}
            p = bufptr;
    	}
		if ( c != '\r' )
		{
	    	*p++ = c;
		}
    	if (c == '\n') 
		{
    		break;
    	}
    	len = efs_read(stream, &c, 1);
    }

    *p++ = '\0';
    *lineptr = bufptr;
    *n = size;

    return p - bufptr - 1;
}
#endif
