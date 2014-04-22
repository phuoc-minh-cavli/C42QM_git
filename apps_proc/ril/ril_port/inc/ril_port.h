/******************************************************************************

                        R I L _ P O R T . H

******************************************************************************/

/******************************************************************************

  @file    ril_port.h
  @brief   RIL Porting Header File

  DESCRIPTION
  Header file for Porting DS Util Functionality to other platforms.

  ---------------------------------------------------------------------------
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------

******************************************************************************/
#ifndef __RIL_PORT_H__
#define __RIL_PORT_H__

#include "qurt_thread.h"
#include "qurt_mutex.h"
#include "qurt_signal.h"
#include "qurt_pipe.h"
#include "qurt_timer.h"
#include "timer.h"
#include "pmapp_rtc.h"
#include "rcinit.h"

#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#include <stdint.h>

#include <err.h>
#include <comdef.h>

#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#include <msg.h>

#ifndef ASSERT
#define ASSERT(X) assert(X)
#endif

#undef htonl
#undef ntohl
#define htonl(l)    (((((l) >> 24) & 0x000000ff)) | \
                    ((((l) >>  8) & 0x0000ff00)) | \
                    (((l) & 0x0000ff00) <<  8) | \
                    (((l) & 0x000000ff) << 24))
#define ntohl(l)    htonl(l)

/* Qurt Signal Mask for command queue signalling */
#define CMDQ_SIGNAL_MASK         0x00000001

#define SLEEP_10MS  (10)
#define SLEEP_100MS (10*SLEEP_10MS)
#define SLEEP_1S    (100*SLEEP_10MS)

#define TIMER_10MS  (10)
#define TIMER_1S    (100*TIMER_10MS)

/*
  Constants relevant to FOTA cookie
*/
#define BOOT_IMEM_MAGIC_NUM        0xC1F8DB40
#define BOOT_IMEM_START_ADDR       0x08600000

#ifdef FEATURE_PSM_TARGET_9205
#define BOOT_IMEM_COOKIE_OFFSET    0x00028000
#else
#define BOOT_IMEM_COOKIE_OFFSET    0x00000000
#endif

/* Typedefing Qurt types with Platform independent types */
typedef qurt_mutex_t             ril_platform_thread_mutex_t;
typedef qurt_thread_t            ril_platform_thread_t;
typedef qurt_signal_t            ril_platform_thread_cond_t;
typedef qurt_thread_attr_t       ril_platform_thread_attr_t;
typedef qurt_pipe_t              ril_platform_pipe_t;
typedef qurt_pipe_attr_t         ril_platform_pipe_attr_t;
typedef timer_type               ril_platform_timer_t;
typedef qurt_timer_attr_t        ril_platform_timer_attr_t;


#ifndef timeval
struct timeval
{
   time_t tv_sec;     /* seconds */
   long   tv_usec;    /* and microseconds */
};

#endif

/*************************************************************************************************************
 *                                             THREADS & MUTEXES                                             *
 ************************************************************************************************************/

/*===========================================================================
  MACRO  ril_platform_thread_create
===========================================================================*/
/*!
@brief
  Wrapper over the Qurt API for creating a thread

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/

#define ril_platform_thread_create(thread, attr, start_routine, arg) \
        qurt_thread_create(thread, attr, (void (*)(void *))start_routine, arg)

#define ril_platform_thread_join(thread, value_ptr) 0


void ril_format_log_msg
(
    char *buf_ptr,
    int buf_size,
    char *fmt,
    ...
);

#define RIL_MAX_LOG_MSG_SIZE 100

/* Log message to Diag */
#define RIL_LOG_MSG( lvl, fmt, ... )                                           \
    {                                                                            \
        char log_fmt[ RIL_MAX_LOG_MSG_SIZE ]; \
        char log_buf[ RIL_MAX_LOG_MSG_SIZE ]; \
        strlcpy(log_fmt, fmt, sizeof(log_fmt));\
        ril_format_log_msg( log_buf, RIL_MAX_LOG_MSG_SIZE, log_fmt, ##__VA_ARGS__ );\
        MSG_SPRINTF_1( MSG_SSID_ANDROID_QCRIL, lvl, "%s", log_buf );\
    }

/* Log message to Diag */
#define RIL_LOG_CRITICAL( lvl, fmt, ... )                                           \
    {                                                                            \
        char log_fmt[ RIL_MAX_LOG_MSG_SIZE ]; \
        char log_buf[ RIL_MAX_LOG_MSG_SIZE ]; \
        strlcpy(log_fmt, fmt, sizeof(log_fmt));\
        ril_format_log_msg( log_buf, RIL_MAX_LOG_MSG_SIZE, log_fmt, ##__VA_ARGS__ );\
        MSG_SPRINTF_1(MSG_SSID_AWARE_PSM_DAEMON, lvl, "%s", log_buf );\
    }


#define RIL_LOG_MSG_VERBOSE(fmt) \
            MSG(MSG_SSID_ANDROID_QCRIL, MSG_LEGACY_LOW, fmt)
#define RIL_LOG_MSG_VERBOSE1(fmt, arg) \
            MSG_1(MSG_SSID_ANDROID_QCRIL, MSG_LEGACY_LOW, fmt, arg)
#define RIL_LOG_MSG_VERBOSE2(fmt, arg1, arg2) \
            MSG_2(MSG_SSID_ANDROID_QCRIL, MSG_LEGACY_LOW, fmt, arg1, arg2)
#define RIL_LOG_MSG_VERBOSE3(fmt, arg1, arg2, arg3) \
            MSG_3(MSG_SSID_ANDROID_QCRIL, MSG_LEGACY_LOW, fmt, arg1, arg2, arg3)

#define RIL_LOG_MSG_VERBOSE_STR(...) \
            RIL_LOG_MSG(MSG_LEGACY_LOW, __VA_ARGS__ )

#define RIL_LOG_MSG_INFO(fmt) \
            MSG(MSG_SSID_ANDROID_QCRIL, MSG_LEGACY_MED, fmt)
#define RIL_LOG_MSG_INFO1(fmt, arg) \
            MSG_1(MSG_SSID_ANDROID_QCRIL, MSG_LEGACY_MED, fmt, arg)
#define RIL_LOG_MSG_INFO2(fmt, arg1, arg2) \
            MSG_2(MSG_SSID_ANDROID_QCRIL, MSG_LEGACY_MED, fmt, arg1, arg2)
#define RIL_LOG_MSG_INFO3(fmt, arg1, arg2, arg3) \
            MSG_3(MSG_SSID_ANDROID_QCRIL, MSG_LEGACY_MED, fmt, arg1, arg2, arg3)
#define RIL_LOG_MSG_INFO4(fmt, arg1, arg2, arg3, arg4) \
            MSG_4(MSG_SSID_ANDROID_QCRIL, MSG_LEGACY_MED, fmt, arg1, arg2, arg3, arg4)

#define RIL_LOG_MSG_INFO_STR(...) \
            RIL_LOG_MSG(MSG_LEGACY_MED, __VA_ARGS__ )

#define RIL_LOG_MSG_DEBUG(fmt) \
            MSG(MSG_SSID_ANDROID_QCRIL, MSG_LEGACY_HIGH, fmt)
#define RIL_LOG_MSG_DEBUG1(fmt, arg) \
            MSG_1(MSG_SSID_ANDROID_QCRIL, MSG_LEGACY_HIGH, fmt, arg)
#define RIL_LOG_MSG_DEBUG2(fmt, arg1 ,arg2) \
            MSG_2(MSG_SSID_ANDROID_QCRIL, MSG_LEGACY_HIGH, fmt, arg1, arg2)
#define RIL_LOG_MSG_DEBUG3(fmt, arg1, arg2, arg3) \
            MSG_3(MSG_SSID_ANDROID_QCRIL, MSG_LEGACY_HIGH, fmt, arg1, arg2, arg3)

#define RIL_LOG_MSG_DEBUG_STR(...) \
            RIL_LOG_MSG(MSG_LEGACY_HIGH, __VA_ARGS__ )

#define RIL_LOG_MSG_ERROR(fmt) \
            MSG(MSG_SSID_ANDROID_QCRIL, MSG_LEGACY_ERROR, fmt)
#define RIL_LOG_MSG_ERROR1(fmt, arg) \
            MSG_1(MSG_SSID_ANDROID_QCRIL, MSG_LEGACY_ERROR, fmt, arg)
#define RIL_LOG_MSG_ERROR2(fmt, arg1, arg2) \
            MSG_2(MSG_SSID_ANDROID_QCRIL, MSG_LEGACY_ERROR, fmt, arg1, arg2)
#define RIL_LOG_MSG_ERROR3(fmt, arg1, arg2, arg3) \
            MSG_3(MSG_SSID_ANDROID_QCRIL, MSG_LEGACY_ERROR, fmt, arg1, arg2, arg3)

#define RIL_LOG_MSG_ERROR_STR(...) \
            RIL_LOG_MSG(MSG_LEGACY_ERROR, __VA_ARGS__ )

#define RIL_LOG_FUNC_ENTRY(fmt) \
            RIL_LOG_MSG_VERBOSE(fmt)

#define RIL_LOG_FUNC_RETURN(fmt) \
            RIL_LOG_MSG_VERBOSE(fmt)

#ifdef PSM_ENABLE_AWARE_CUSTOMIZATION
#define RIL_LOG_MSG_CRITICAL(...) \
     RIL_LOG_CRITICAL(MSG_LEGACY_HIGH, __VA_ARGS__ )
#else
#define RIL_LOG_MSG_CRITICAL(...)
#endif


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

int ril_platform_thread_cond_init(qurt_signal_t *cond, int attribute);


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

int ril_platform_thread_cond_destroy(ril_platform_thread_cond_t *cond);


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

int ril_platform_thread_cond_signal(qurt_signal_t *cond);


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

int ril_platform_thread_cond_wait(qurt_signal_t *cond, qurt_mutex_t *mutex);

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
);

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
);


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
);


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

int ril_platform_thread_mutex_destroy(ril_platform_thread_mutex_t *mutex);

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

int ril_platform_thread_mutex_lock(ril_platform_thread_mutex_t *mutex);

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

int ril_platform_thread_mutex_unlock(ril_platform_thread_mutex_t *mutex);

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

//int ril_platform_thread_attr_init(ril_platform_thread_attr_t *attr);

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

int ril_platform_thread_sleep(uint32 ticks);


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
int ril_platform_pipe_attr_init(ril_platform_pipe_attr_t *attr);


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
int ril_platform_pipe_attr_set_elements(ril_platform_pipe_attr_t *attr, int num);


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
int ril_platform_pipe_attr_set_element_size(ril_platform_pipe_attr_t *attr, int sz);


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
int ril_platform_pipe_create(ril_platform_pipe_t *p, ril_platform_pipe_attr_t *attr);


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
//int ril_platform_pipe_delete(ril_platform_pipe_t p);

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
int ril_platform_pipe_receive(ril_platform_pipe_t p, void *buffer);


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
int ril_platform_pipe_send(ril_platform_pipe_t p, void *buffer);


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
//int ril_platform_timer_start_periodic(ril_platform_timer_t *timer_id, void(*callback_func) (void *), void *, uint32 secs);


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
int ril_platform_timer_start_oneshot(ril_platform_timer_t *timer_id, void(*callback_func) (void *), void *, uint32 secs);

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
int ril_platform_timer_restart(ril_platform_timer_t *timer_id, uint32 secs);

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
int ril_platform_timer_stop(ril_platform_timer_t *timer_id);


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
int ril_platform_timer_delete(ril_platform_timer_t *timer_id);

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
qurt_time_t ril_platform_timer_get_ticks(uint32 msecs);

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
uint32 ril_platform_get_imem_cookie_addr(void);

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
uint32 ril_platform_get_imem_magic_num(void);

#endif /* __RIL_PORT_H__ */

