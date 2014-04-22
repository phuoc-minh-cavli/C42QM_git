#ifndef QURT_TIMER_H
#define QURT_TIMER_H
/**
  @file qurt_timer.h
  @brief  Prototypes of qurt_timer API 
  Qurt Timer APIs allows two different mechanism for user notification on timer
  expiry. signal and callback. A user can choose one of them but not both.

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2013-2015  by Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.


=============================================================================*/
#include "qurt_types.h"
#include "qurt_signal.h"
#include "qurt_timetick.h"

/*=============================================================================
                        CONSTANTS AND MACROS
=============================================================================*/
/**
   qurt timer options                                                       
 */
#define QURT_TIMER_ONESHOT              0x01   /**< one short timer .*/
#define QURT_TIMER_PERIODIC             0x02   /**< periodic timer  .*/
#define QURT_TIMER_NO_AUTO_START        0x04   /**< No Auto Activate.*/
#define QURT_TIMER_AUTO_START           0x08   /**< Default, Auto Activate */
#define QURT_TIMER_OBJ_SIZE_BYTES       128    /* on qurt its 40 */
#define QURT_TIMER_GRANULARITY			10	   /* 10 millisec */

/*=============================================================================
                        TYPEDEFS
=============================================================================*/

/** qurt_timer_t types.*/
typedef unsigned long  qurt_timer_t;

/** qurt_timer_cb_func_t types. */
typedef void (*qurt_timer_callback_func_t)( void *);

typedef struct qurt_timer_attr  /* 8 byte aligned */
{
   unsigned long long _bSpace[QURT_TIMER_OBJ_SIZE_BYTES/sizeof(unsigned long long)];
}qurt_timer_attr_t;

/** @} */ /* end_addtogroup timer_types */

/*=============================================================================
                        FUNCTIONS
=============================================================================*/
/**@ingroup func_qurt_timer_attr_init
  Initializes the specified timer attribute structure with default attribute values: \n
  - Timer duration -- 0  \n
  - Timer type -- QURT_TIMER_ONESHOT \n

  @datatypes
  #qurt_timer_attr_t

  @param[in,out] attr Pointer to the destination structure for the timer attributes.

  @return
  None.

  @dependencies
  None.
  
 */
void qurt_timer_attr_init(qurt_timer_attr_t *attr);

/**@ingroup func_qurt_timer_attr_set_duration
  Sets the timer duration in the specified timer attribute structure.\n

  The timer duration specifies the interval (in timer ticks) between the activation of the
  timer object and the generation of the corresponding timer event.
  
  @note1hang if the timer is activated during creation, the duration specified is the interval
  (in timer ticks) is between the creation and the generation of the corresponding timer event.

  @datatypes
  #qurt_timer_attr_t \n
  #qurt_time_t

  @param[in,out] attr    Pointer to the timer attribute structure.
  @param[in] duration    Timer duration (in timer ticks).

  @return
  None.

  @dependencies
  None.
  
 */
void qurt_timer_attr_set_duration(qurt_timer_attr_t *attr, qurt_time_t duration);

/**@ingroup func_qurt_timer_attr_set_callback
  Sets the timer callback function and callback context in the specified timer attribute structure. \n

  Callback function is invoked on timer expiry with callback context a user passed while creating the timer. 

  @datatypes
  #qurt_timer_attr_t \n
  #qurt_timer_cb_func_t

  @param[in] attr     Pointer to the timer attribute object.
  @param[in] cbfunc   Pointer to the timer callback function.
  @param[in] cbctxt   Pointer to the timer callback context.

  @return
  None.

  @dependencies
  None.
  
 */
void qurt_timer_attr_set_callback(qurt_timer_attr_t *attr, qurt_timer_callback_func_t cbfunc, void *cbctxt);

/**@ingroup func_qurt_timer_attr_set_signal
  Sets the signal object and mask in the specified timer attribute structure. \n
  signal mask are set on timer expiry.
  
  @datatypes
  #qurt_timer_attr_t \n

  @param[in] attr    Pointer to the timer attribute object. \n
  @param[in] *signal  Pointer to the signal object. \n
  @param[in] mask    signal mask to be set when timer expires. \n

  @return
  None.

  @dependencies
  None.
  
 */
void qurt_timer_attr_set_signal(qurt_timer_attr_t *attr, qurt_signal_t *signal, uint32 mask);

/**@ingroup func_qurt_timer_attr_set_reload
  Sets the timer reload time in the specified timer attribute structure.\n

  The timer reload specifies the interval (in timer ticks) for all timer 
  expiration after first. A zero value indicate the timer be one-shot timer.

  @datatypes
  #qurt_timer_attr_t 
  #qurt_time_t\n

  @param[in]  attr         Pointer to the timer attribute object.
  @param[out] reload_time  Timer reload (in timer ticks).

  @return
  QURT_EOK -- timer set reload success. \n
  QURT_EINVALID - Invalid argument. \n
  QURT_EFAILED -- timer set option failed. \n

  @dependencies
  None.
 */
void qurt_timer_attr_set_reload(qurt_timer_attr_t *attr, qurt_time_t reload_time);

/**@ingroup func_qurt_timer_attr_set_option
  Sets the timer option in the specified timer attribute object. 
  
  The timer type specifies the functional behavior of the timer: \n
  - A one-shot timer (QURT_TIMER_ONESHOT) waits for the specified timer duration
      and then generates a single timer event. After this the timer is nonfunctional. \n
  - A periodic timer (QURT_TIMER_PERIODIC) repeatedly waits for the specified
     timer duration and then generates a timer event. The result is a series of timer
     events with interval equal to the timer duration. \n
  - A auto activate timer option (QURT_TIMER_AUTO_ACTIVATE) activate the timer when it is created. \n
  - A no auto activate timer option (QURT_TIMER_NO_AUTO_ACTIVATE) create the timer in a non-active state. \n
  
   @note 
   QURT_TIMER_ONESHOT and QURT_TIMER_PERIODIC are mutally exclusive.\n
   QURT_TIMER_AUTO_ACTIVATE and QURT_TIMER_NO_AUTO_ACTIVATE are mutally exclusive.\n
   options can be ORed. \n
   
   @datatypes 
   #qurt_timer_attr_t \n
   
   @param[in,out]  attr  Pointer to the timer attribute object.
   @param[in]      option  Timer option. Values are: \n
                   - QURT_TIMER_ONESHOT - One time timer \n
                   - QURT_TIMER_PERIODIC - Periodic timer
                   - QURT_TIMER_AUTO_ACTIVATE -- Auto enable \n
                   - QURT_TIMER_NO_AUTO_ACTIVATE -- Auto disable. @tablebulletend

   @return
   None.

   @dependencies
   None.
   
 */
void qurt_timer_attr_set_option(qurt_timer_attr_t *attr, uint32 option);

/**@ingroup func_qurt_timer_create
  Creates a timer.
 
  QuRT timer can be created as signal or callback as notification mechanism on timer expiry .\n
  The option is mutually exclusive and defined via attribute apis. Please refer apis - 
  qurt_timer_attr_set_callback, qurt_timer_attr_set_signal.
  
  Timer can be started at the time of creation or started explicitly latter based on options
  set by users.
  
  @note1hang A timer signal handler must be defined to wait on the specified signal 
             in order to handle the timer signal.

  @datatypes
  #qurt_timer_t \n
  #qurt_timer_attr_t \n

  @param[out] timer   Pointer to the created timer object.
  @param[in]  attr    Pointer to the timer attribute structure.

  @return
  QURT_EOK -- Success. \n
  QURT_EFAILED -- Failed to create timer. \n
  
  @dependencies
  None.
  
 */
int qurt_timer_create (qurt_timer_t *timer, const qurt_timer_attr_t *attr); 
                  
/**@ingroup func_qurt_timer_start
  Activate the timer.

  @datatypes
  #qurt_timer_t 

  @param[in] timer   created timer object.

  @return
  QURT_EOK -- timer activated successfully. \n
  QURT_EFAILED -- timer activation failed . \n
  QURT_EINVALID - Invalid argument. \n
  
  @dependencies
  None.
  
 */
int qurt_timer_start(qurt_timer_t timer);

/**@ingroup func_qurt_timer_restart
  @xreflabel{sec:qurt_timer_restart}
  Restarts a stopped timer with the specified duration.
  The timer must be a one-shot timer.

  @note1hang Timers become stopped after they have expired or after they are explicitly
             stopped with the timer stop operation, see Section @xref{sec:qurt_timer_stop}.
  
  @note1hang Timer expires after to duration passed as user param from the time this api is
  invoked.
  
  @datatypes
  #qurt_timer_t \n
  #qurt_time_t

  @param[in] timer        Timer object. 
  @param[in] duration     Timer duration (in timer ticks) before the restarted timer
                          expires again.
  @return             
  QURT_EOK -- Success. \n
  QURT_EINVALID -- Invalid timer ID or duration value. \n
  QURT_ENOTALLOWED -- Timer is not a one-shot timer. \n
  QURT_EMEM --  Out-of-memory error. \n

  @dependencies
  None.
  
 */
int qurt_timer_restart (qurt_timer_t timer, qurt_time_t duration);

/**@ingroup func_qurt_timer_get_attr
  Gets the timer attributes of the specified timer.

  @note1hang After a timer is created, its attributes cannot be changed.

  @datatypes
  #qurt_timer_t \n
  #qurt_timer_attr_t

  @param[in] timer  Timer object.
  @param[out] attr  Pointer to the destination structure for timer attributes.

  @return
  QURT_EOK -- Success. \n
  QURT_VAL -- Argument passed is not a valid timer.

  @dependencies
  None.
 */
int qurt_timer_get_attr(qurt_timer_t timer, qurt_timer_attr_t *attr);

/**@ingroup func_qurt_timer_attr_get_duration
  Gets the timer duration from the specified timer attribute structure.
  
  @note1hang A call to the api will return the duration of timer that was
  originally set for timer to expire. This api doesnot return remaining time
  of active timer. see Section @xref{sec:qurt_timer_attr_get_remaining}.
  
  @datatypes
  #qurt_timer_attr_t \n
  #qurt_time_t

  @param[in]  attr       Pointer to the timer attributes object
  @param[out] duration   Pointer to the destination variable for timer duration.

  @return
  QURT_EOK -- timer get duration success. \n
  QURT_EINVALID - Invalid argument. \n
  QURT_EFAILED -- timer get duration failed. \n

  @dependencies
  None.
  
 */
int qurt_timer_attr_get_duration(qurt_timer_attr_t *attr, qurt_time_t *duration);

/**@ingroup func_qurt_timer_attr_get_remaining
  Gets the timer remaining duration from the specified timer attribute structure. \n

  The timer remaining duration indicates (in timer ticks) how much time remains before
  the generation of the next timer event on the corresponding timer.

  @note1hang This attribute is read-only and thus cannot be set.

  @datatypes
  #qurt_timer_attr_t \n
  #qurt_time_t

  @param[in] attr          Pointer to the timer attribute object.
  @param[out] remaining    Pointer to the destination variable for remaining time.

  @return
  QURT_EOK -- timer get remaining time success. \n
  QURT_EINVALID - Invalid argument. \n
  QURT_EFAILED -- timer get duration failed. \n

  @dependencies
  None.
  
 */
int qurt_timer_attr_get_remaining(qurt_timer_attr_t *attr, qurt_time_t *remaining);

/**@ingroup func_qurt_timer_attr_get_option
  Gets the timer option from the specified timer attribute object.

  @datatypes
  #qurt_timer_attr_t \n

  @param[in]  attr  Pointer to the timer attribute object.
  @param[out] type  Pointer to the destination variable for the timer option.

  @return
  QURT_EOK -- timer get option success. \n
  QURT_EINVALID - Invalid argument. \n
  QURT_EFAILED -- timer get duration failed. \n

  @dependencies
  None.
 */
int qurt_timer_attr_get_option(qurt_timer_attr_t *attr, uint32  *option);

/**@ingroup func_qurt_timer_attr_get_reload
  Gets the timer reload time from specified timer attribute structure.\n
  
  The timer reload specifies the interval (in timer ticks) for all timer 
  expiration after first. A zero value indicate the timer be one-shot timer.
  
  @datatypes
  #qurt_timer_attr_t
  #qurt_time_t \n
  
  @param[in]  attr  Pointer to the timer attribute object.
  @param[out] type  Timer reload (in timer ticks).

  @return
  QURT_EOK -- timer get reload success. \n
  QURT_EINVALID - Invalid argument. \n
  QURT_EFAILED -- timer get reload failed. \n

  @dependencies
  None.
 */
int qurt_timer_attr_get_reload(qurt_timer_attr_t *attr, qurt_time_t * reload_time);

/**@ingroup func_qurt_timer_delete
  Deletes the timer.\n
  Destroys the specified timer and deallocates the timer object.

  @datatypes
  #qurt_timer_t
  
  @param[in] timer  Timer object.

  @return       
  QURT_EOK -- timer delete success. \n
  QURT_EFAILED -- timer delete failed. \n
  QURT_EVAL -- Argument passed is not a valid timer. 

  @dependencies
  None.
  
 */
int qurt_timer_delete(qurt_timer_t timer);

/**@ingroup func_qurt_timer_stop
  @xreflabel{sec:qurt_timer_stop}  
  Stops a running timer.
  The timer must be a one-shot timer.

  @note1hang Stopped timers can be restarted with the timer restart/activate operation,
             see Section @xref{sec:qurt_timer_restart}. 

  @datatypes
  #qurt_timer_t
  
  @param[in] timer    Timer object. 

  @return
  QURT_EOK -- Success. \n
  QURT_EINVALID -- Invalid timer ID or duration value. \n
  QURT_ENOTALLOWED -- Timer is not a one shot timer. \n
  QURT_EMEM -- Out of memory error.

  @dependencies
  None.
  
 */
int qurt_timer_stop (qurt_timer_t timer);

/**@ingroup func_qurt_timer_get_remaining
   
  Gets the timer remaining duration from the QuRT library timer service.\n
 
  The timer remaining duration indicates (in system ticks) how much time remains before 
  the generation of the next timer event on any active timer in the QuRT application system.
  
  @note1hang This API must be used only when the QuRT application system is running 
  in STM mode with all interrupts disabled. Otherwise, it will result in undefined 
  behaviour, and may have side effects.
  
  @note1handle Primary user of this API is sleep. 
  
  @param
  None.
  
  @return
  Number of system ticks until next timer event.
  QURT_TIME_WAIT_FOREVER – no pending timer events.
   
  @dependencies
  None.
  
 */
qurt_time_t qurt_timer_get_remaining ( void );
 
/*
	Return timer_tick for specified system ticks
	1 timer_tick == 1 timer interrupt interver == QURT_TIMER_NEXT_EXPIRY period.
*/
uint32  qurt_timer_get_timer_tick(qurt_time_t  sys_ticks) ;

#endif /* QURT_TIMER_H */
