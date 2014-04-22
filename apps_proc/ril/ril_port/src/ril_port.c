/******************************************************************************

                        RIL _ P O R T . C

******************************************************************************/

/******************************************************************************

  @file    ril_port.c
  @brief   RIL Porting File

  DESCRIPTION
  Source file for Porting RIL Functionality to other platforms.

  ---------------------------------------------------------------------------
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------

******************************************************************************/
#include <stdlib.h>
#include <stdarg.h>
#include "string.h"
#include "qurt_thread.h"
#include "qurt_mutex.h"
#include "ril_port.h"
#include "timer.h"
#include <stdint.h>


/*************************************************************************************************************
 *                                             THREADS & MUTEXES                                             *
 ************************************************************************************************************/

/*===========================================================================*/
/*!
    @brief
    Format debug message for logging.

    @return
    None
*/
/*=========================================================================*/
void ril_format_log_msg
(
    char *buf_ptr,
    int buf_size,
    char *fmt,
    ...
)
{
    va_list ap;
    va_start( ap, fmt );

    if ( NULL != buf_ptr && buf_size > 0 )
    {
        vsnprintf( buf_ptr, buf_size, fmt, ap );
    }

    va_end( ap );
} /* ril_format_log_msg */


/*===========================================================================
  FUNCTION  ril_platform_thread_mutex_init
===========================================================================*/
/*!
@brief
  Wrapper over the Qurt API for initializing a mutex

@return
  int - Zero value

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/

int ril_platform_thread_mutex_init
(
  ril_platform_thread_mutex_t *mutex,
  void *attr
 )
{
    qurt_mutex_init(mutex);
    return 0;
}
/*===========================================================================
  FUNCTION  ril_platform_thread_mutex_destroy
===========================================================================*/
/*!
@brief
  Wrapper over the Qurt API for destroying a mutex 

@return
  int - Zero value

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/

int ril_platform_thread_mutex_destroy(ril_platform_thread_mutex_t *mutex)
{
    qurt_mutex_destroy(mutex);
    return 0;
}

/*===========================================================================
  FUNCTION  ril_platform_thread_mutex_destroy
===========================================================================*/
/*!
@brief
  Wrapper over the Qurt API for setting a mutex to the locked state

@return
  int - Zero value

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/

int ril_platform_thread_mutex_lock(ril_platform_thread_mutex_t *mutex)
{
    qurt_mutex_lock(mutex);
    return 0;
}

/*===========================================================================
  FUNCTION  ril_malloc
===========================================================================*/
/*!
@brief
  Wrapper over the Qurt API for setting a mutex to the unlocked state

@return
  int - Zero value

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/

int ril_platform_thread_mutex_unlock(ril_platform_thread_mutex_t *mutex)
{
    qurt_mutex_unlock(mutex);
    return 0;
}

/*===========================================================================
  FUNCTION  ril_platform_thread_cond_wait
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

int ril_platform_thread_cond_wait
(
    ril_platform_thread_cond_t *cond,
    ril_platform_thread_mutex_t *mutex
)
{
    int ret = 0;
    qurt_mutex_unlock(mutex);
    ret = qurt_signal_wait(cond, CMDQ_SIGNAL_MASK, QURT_SIGNAL_ATTR_WAIT_ANY);
    qurt_mutex_lock(mutex);
    return ret;
}

/*===========================================================================
  FUNCTION  ril_platform_thread_cond_wait_and_clear
===========================================================================*/
/*!
@brief
  Wrapper over the Qurt API for waiting over a signal object and clear after
  recived

@return
  int - A 32-bit word with current signal masks being waited, set.

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/

int ril_platform_thread_cond_wait_and_clear
(
    ril_platform_thread_cond_t *cond,
    ril_platform_thread_mutex_t *mutex
)
{
    int ret = 0;
    qurt_mutex_unlock(mutex);
    ret = qurt_signal_wait(cond, CMDQ_SIGNAL_MASK, QURT_SIGNAL_ATTR_CLEAR_MASK);
    qurt_mutex_lock(mutex);
    return ret;
}

/*===========================================================================
  FUNCTION  ril_platform_thread_cond_wait_timed
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

int ril_platform_thread_cond_wait_timed
(
    ril_platform_thread_cond_t *cond,
    ril_platform_thread_mutex_t *mutex,
    int secs
)
{
    int ret = 0;
    uint32 signal = 0;
    qurt_time_t ticks;

    ticks = ril_platform_timer_get_ticks(secs);
    qurt_mutex_unlock(mutex);
    ret = qurt_signal_wait_timed_ext(cond, CMDQ_SIGNAL_MASK, QURT_SIGNAL_ATTR_WAIT_ANY, &signal, ticks);
    qurt_mutex_lock(mutex);
    return ret;
}



/*===========================================================================
  FUNCTION  ril_platform_thread_cond_signal
===========================================================================*/
/*!
@brief
  Wrapper over the Qurt API for setting a signal object

@return
  int - Zero value

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/

int ril_platform_thread_cond_signal(ril_platform_thread_cond_t *cond)
{
    qurt_signal_set(cond, CMDQ_SIGNAL_MASK);
    return 0;
}

/*===========================================================================
  FUNCTION  ril_platform_thread_cond_init
===========================================================================*/
/*!
@brief
  Wrapper over the Qurt API for initializing a signal object

@return
  int - Zero value

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/

int ril_platform_thread_cond_init(ril_platform_thread_cond_t *cond, int attribute)
{
    qurt_signal_init(cond);
    return 0;
}

/*===========================================================================
  FUNCTION  ril_platform_thread_cond_destroy
===========================================================================*/
/*!
@brief
  Wrapper over the Qurt API for destroying a signal object

@return
  int - Zero value

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/

int ril_platform_thread_cond_destroy(ril_platform_thread_cond_t *cond)
{
    qurt_signal_destroy(cond);
    return 0;
}

/*===========================================================================
  FUNCTION  ril_malloc
===========================================================================*/
/*!
@brief
  Wrapper over the Qurt API for setting a mutex to the unlocked state

@return
  int - Zero value

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
/*int ril_platform_thread_attr_init(ril_platform_thread_attr_t *attr)
{
    qurt_thread_attr_init(attr);
    return 0;
}*/


/*===========================================================================
  FUNCTION  ril_malloc
===========================================================================*/
/*!
@brief
  Wrapper over the Qurt API for setting a mutex to the unlocked state

@return
  int - Zero value

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int ril_platform_thread_sleep(uint32 secs)
{
    timer_sleep(secs,T_MSEC, TRUE);
    return 0;
}


/*===========================================================================
  FUNCTION  ril_platform_pipe_attr_init
===========================================================================*/
/*!
@brief
  Wrapper over the Qurt API for init pipe attributes

@return
  int - Zero value

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int ril_platform_pipe_attr_init(ril_platform_pipe_attr_t *attr)
{
    qurt_pipe_attr_init(attr);
    return 0;
}


/*===========================================================================
  FUNCTION  ril_platform_pipe_attr_set_elements
===========================================================================*/
/*!
@brief
  Wrapper over the Qurt API for setting size of pipe

@return
  int - Zero value

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int ril_platform_pipe_attr_set_elements(ril_platform_pipe_attr_t *attr, int num)
{
    qurt_pipe_attr_set_elements(attr, num);
    return 0;
}


/*===========================================================================
  FUNCTION  ril_platform_pipe_attr_set_element_size
===========================================================================*/
/*!
@brief
  Wrapper over the Qurt API for setting a mutex to the unlocked state

@return
  int - Zero value

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int ril_platform_pipe_attr_set_element_size(ril_platform_pipe_attr_t *attr, int sz)
{
    qurt_pipe_attr_set_element_size(attr, sz);
    return 0;
}

/*===========================================================================
  FUNCTION  ril_platform_pipe_create
===========================================================================*/
/*!
@brief
  Wrapper over the Qurt API for creating pipe

@return
  int - Zero value

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int ril_platform_pipe_create(ril_platform_pipe_t *p, ril_platform_pipe_attr_t *attr)
{
    return qurt_pipe_create(p, attr);
}


/*===========================================================================
  FUNCTION  ril_platform_pipe_delete
===========================================================================*/
/*!
@brief
  Wrapper over the Qurt API for deleting pipe

@return
  int - Zero value

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
/*int ril_platform_pipe_delete(ril_platform_pipe_t p)
{
    qurt_pipe_delete(p);
    return 0;
}*/


/*===========================================================================
  FUNCTION  ril_platform_pipe_receive
===========================================================================*/
/*!
@brief
  Wrapper over the Qurt API for receiving data over pipe

@return
  int - Zero value

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int ril_platform_pipe_receive(ril_platform_pipe_t p, void *buffer)
{
    qurt_pipe_receive(p, buffer);
    return 0;
}


/*===========================================================================
  FUNCTION  ril_platform_pipe_send
===========================================================================*/
/*!
@brief
  Wrapper over the Qurt API for sending data over pipe

@return
  int - Zero value

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int ril_platform_pipe_send(ril_platform_pipe_t p, void *buffer)
{
    return qurt_pipe_try_send(p, buffer);
}


/*===========================================================================
  FUNCTION  ril_platform_start_periodic_timer
===========================================================================*/
/*!
@brief
  Wrapper over the Qurt API for starting timer

@return
  int - Zero value

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
/*int ril_platform_timer_start_periodic(ril_platform_timer_t *timer_id,
                                                     void(*callback_func) (void *),
                                                     void *cb_data,
                                                     uint32 secs)
{
    timer_def_osal(timer_id, &timer_non_defer_group, TIMER_FUNC1_CB_TYPE, callback_func, (time_osal_notify_data) cb_data);
    timer_set(timer_id, secs, TRUE, T_MSEC);

    return 0;
}*/

/*===========================================================================
  FUNCTION  ril_platform_start_oneshot_timer
===========================================================================*/
/*!
@brief
  Wrapper over the Qurt API for starting timer

@return
  int - Zero value

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int ril_platform_timer_start_oneshot(ril_platform_timer_t *timer_id,
                                                   void(*callback_func) (void *),
                                                   void *cb_data,
                                                   uint32 secs)
{
    timer_def_osal(timer_id, &timer_non_defer_group, TIMER_FUNC1_CB_TYPE, callback_func, (time_osal_notify_data) cb_data);
    timer_set(timer_id, secs, FALSE, T_MSEC);

    return 0;
}

/*===========================================================================
  FUNCTION  ril_platform_timer_restart
===========================================================================*/
/*!
@brief
  Wrapper over the Qurt API for restarting timer

@return
  int - Zero value

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int ril_platform_timer_restart(ril_platform_timer_t *timer_id, uint32 secs)
{
    timer_clr(timer_id, T_MSEC);
    timer_set(timer_id, secs, FALSE, T_MSEC);

    return 0;
}

/*===========================================================================
  FUNCTION  ril_platform_start_oneshot_timer
===========================================================================*/
/*!
@brief
  Wrapper over the Qurt API for starting timer

@return
  int - Zero value

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int ril_platform_timer_stop(ril_platform_timer_t *timer_id)
{
    timer_clr(timer_id, T_MSEC);
    return 0;
}


/*===========================================================================
  FUNCTION  ril_platform_start_oneshot_timer
===========================================================================*/
/*!
@brief
  Wrapper over the Qurt API for starting timer

@return
  int - Zero value

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int ril_platform_timer_delete(ril_platform_timer_t *timer_id)
{
    timer_undef(timer_id);
    return 0;
}

/*===========================================================================
  FUNCTION  ril_platform_timer_get_ticks
===========================================================================*/
/*!
@brief
  Wrapper over the Qurt Timer API for converting PSM seconds to timer ticks

@return
  qurt_time_t - ticks

@note

  - Dependencies
    - None

  - Side Effects
    - None

    Usage Note
    - Input secs is in milliseconds.
*/
/*=========================================================================*/
qurt_time_t ril_platform_timer_get_ticks(uint32 msecs)
{
    return qurt_timer_convert_time_to_ticks(msecs, QURT_TIME_MSEC);
}

/*===========================================================================
  FUNCTION  ril_platform_get_imem_cookie_addr
===========================================================================*/
/*!
@brief
  Wrapper to calculate IMEM cookie address based upon platform/target

@return
  uint32 - The start address of IMEM structure

@note

  - Dependencies
    - None

  - Side Effects
    - None

    Usage Note
    - Gets the physical address of cookie structure.
*/
/*=========================================================================*/
uint32 ril_platform_get_imem_cookie_addr(void)
{
    return BOOT_IMEM_START_ADDR+BOOT_IMEM_COOKIE_OFFSET;
}

/*===========================================================================
  FUNCTION  ril_platform_get_imem_magic_num
===========================================================================*/
/*!
@brief
  Wrapper to get IMEM cookie magic number

@return
  uint32 - The IMEM cookie magic number

@note

  - Dependencies
    - None

  - Side Effects
    - None

    Usage Note
    - Gets the cookie magic number.
*/
/*=========================================================================*/
uint32 ril_platform_get_imem_magic_num(void)
{
    return BOOT_IMEM_MAGIC_NUM;
}




