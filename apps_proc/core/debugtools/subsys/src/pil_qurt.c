/*=============================================================================

        P E R I P H E R A L   I M A G E   L O A D E R    M O D U L E
                O S   W r a p p e r   F u n c t i o n s 

GENERAL DESCRIPTION
  Implements Wrappers for OS entities like signal, mutex objects etc 

EXTERNALIZED FUNCTIONS
  Signal functions
    pil_os_sig_init() - Initializes signal object
    pil_os_sig_wait() - Waits for signal
    pil_os_sig_set() - Set a signal
    pil_os_sig_clr() - Clear a signal
    
  Mutex functions
    pil_os_mutex_init() - Initialize a mutex
    pil_os_mutex_lock() - Take Mutex
    pil_os_mutex_unlock() - Release Mutex
    pil_os_mutex_deinit() - DeInitialize Mutex
    
  Wait Obj functions
    pil_os_wait_obj_init() - Initialize wait object
    pil_os_start_wait_obj() - Start wait object with required timeout
    pil_os_check_wait_obj() - check if timeout happened
    pil_os_wait_sig_obj_deinit() - Deinitialize wait object
    
  Cache functions
    pil_os_cache_flush() - Flush full cache
    pil_os_mem_barrier() - apply memory barrier
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2016 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/debugtools/subsys/src/pil_qurt.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------

09/15/16   ab      File created.

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "pil.h"
#include "pil_os.h"
#include "pil_utils.h"
#include "qurt.h"

/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/
#define PIL_OS_WAIT_SIG_VALUE 0x1 
#define PIL_OS_WAIT_OBJ_MAGIC 0x87654321 

//typedef struct 
//{
//  qurt_signal_t signal;
//}pil_sig_obj_t;

/*=============================================================================

                             FUNCTION DEFINITIONS

=============================================================================*/

/*=============================================================================
FUNCTION 
  pil_os_sig_init

DESCRIPTION
  This function initializes/allocates signal object
  
DEPENDENCIES
  None

RETURN VALUE
  None
=============================================================================*/
void pil_os_sig_init(pil_sig_obj_ptr sig_obj)
{
  PIL_VERIFY(sig_obj != NULL, "pil_os_sig_init: sig_obj is NULL", 0, 0, 0);
  
  qurt_signal_init(&(sig_obj->signal));
} /* pil_os_sig_init */

/*=============================================================================
FUNCTION 
  pil_os_sig_wait

DESCRIPTION
  Waits for required signals on signal object
  
DEPENDENCIES
  sig_obj should be initialized before waiting

RETURN VALUE
  None
=============================================================================*/
pil_sigs_type pil_os_sig_wait(pil_sig_obj_ptr sig_obj, pil_sigs_type wait_sigs)
{
  PIL_VERIFY(sig_obj != NULL, "pil_os_sig_wait: sig_obj is NULL", 0, 0, 0);
  
  return qurt_signal_wait(&(sig_obj->signal), wait_sigs, QURT_SIGNAL_ATTR_WAIT_ANY);
} /* pil_os_sig_wait */


/*=============================================================================
FUNCTION 
  pil_os_sig_set

DESCRIPTION
  Sets given signal value to signal object
  
DEPENDENCIES
  sig_obj should be initialized before setting

RETURN VALUE
  In Qurt, it will return 0 always
=============================================================================*/
pil_sigs_type pil_os_sig_set(pil_sig_obj_ptr sig_obj, pil_sigs_type set_sigs)
{
  PIL_VERIFY(sig_obj != NULL, "pil_os_sig_set: sig_obj is NULL", set_sigs, 0, 0);
  
  //qurt_signal_set() doesnt return anything
  qurt_signal_set(&(sig_obj->signal), set_sigs);
  
  return 0;
} /* pil_os_sig_set */


/*=============================================================================
FUNCTION 
  pil_os_sig_clr

DESCRIPTION
  Clears required signals on signal object
  
DEPENDENCIES
  sig_obj should be initialized before clearing

RETURN VALUE
  None
=============================================================================*/
void pil_os_sig_clr(pil_sig_obj_ptr sig_obj, pil_sigs_type clr_sigs)
{
  PIL_VERIFY(sig_obj != NULL, "pil_os_sig_clr: sig_obj is NULL", 0, 0, 0);
  
  qurt_signal_clear(&(sig_obj->signal), clr_sigs);
} /* pil_os_sig_clr */


/*=============================================================================
FUNCTION 
  pil_os_mutex_init

DESCRIPTION
  Initialize Mutex object
  
DEPENDENCIES
  None

RETURN VALUE
  None
=============================================================================*/
pil_error_type pil_os_mutex_init(pil_mutex_obj_ptr mutex_obj)
{
  PIL_VERIFY(mutex_obj != NULL, "pil_os_mutex_init: mutex_obj is NULL", 0, 0, 0);
  
  qurt_mutex_init(&(mutex_obj->mutex));
  
  return PIL_SUCCESS;
} /* pil_os_mutex_init */


/*=============================================================================
FUNCTION 
  pil_os_mutex_deinit

DESCRIPTION
  De-Initialize Mutex object
  
DEPENDENCIES
  Mutex obj needs to be initialized

RETURN VALUE
  None
=============================================================================*/
pil_error_type pil_os_mutex_deinit(pil_mutex_obj_ptr mutex_obj)
{
  PIL_VERIFY(mutex_obj != NULL, "pil_os_mutex_deinit: mutex_obj is NULL", 0, 0, 0);
  
  qurt_mutex_destroy(&(mutex_obj->mutex));
  
  return PIL_SUCCESS;
} /* pil_os_mutex_deinit */


/*=============================================================================
FUNCTION 
  pil_os_mutex_lock

DESCRIPTION
  Take hold of Mutex
  
DEPENDENCIES
  Mutex obj needs to be initialized

RETURN VALUE
  None
=============================================================================*/
pil_error_type pil_os_mutex_lock(pil_mutex_obj_ptr mutex_obj)
{
  PIL_VERIFY(mutex_obj != NULL, "pil_os_mutex_lock: mutex_obj is NULL", 0, 0, 0);  
  
  qurt_mutex_lock(&(mutex_obj->mutex));
  
  return PIL_SUCCESS;
} /* pil_os_mutex_lock */


/*=============================================================================
FUNCTION 
  pil_os_mutex_unlock

DESCRIPTION
  Release a mutex
  
DEPENDENCIES
  Mutex obj needs to be initialized

RETURN VALUE
  None
=============================================================================*/
pil_error_type pil_os_mutex_unlock(pil_mutex_obj_ptr mutex_obj)
{
  PIL_VERIFY(mutex_obj != NULL, "pil_os_mutex_unlock: mutex_obj is NULL", 0, 0, 0);  
  
  qurt_mutex_unlock(&(mutex_obj->mutex));
  
  return PIL_SUCCESS;
} /* pil_os_mutex_unlock */


/*=============================================================================
FUNCTION 
  pil_os_cache_flush

DESCRIPTION
  Flushes full cache
  
DEPENDENCIES
  None

RETURN VALUE
  None
=============================================================================*/
void pil_os_cache_flush(void)
{
  qurt_mem_cache_clean(0, 0, QURT_MEM_CACHE_FLUSH_ALL, QURT_MEM_DCACHE);
  
  //Todo: Once above api works, below thing can be removed i guess.. 
  //Also we are using non-cache memories.. we shouldnt be really needing this
  pil_busywait(50);
} /* pil_os_cache_flush */


/*=============================================================================
FUNCTION 
  pil_os_mem_barrier

DESCRIPTION
  Applies barrier operation for completing previous instructions.
  Once previous instructions are executed, then below function will return.
  
DEPENDENCIES
  None

RETURN VALUE
  None
=============================================================================*/
void pil_os_mem_barrier(void)
{
  qurt_mem_barrier();
} /* pil_os_mem_barrier */


/*=============================================================================
FUNCTION 
  pil_os_wait_obj_init

DESCRIPTION
  Initializes PIL wait object
  
DEPENDENCIES
  None

RETURN VALUE
  None
=============================================================================*/
//These functions can also be implemented using obtaining timetick and checking 
//how much time elapsed whenever check function is called instead of using timers
pil_error_type pil_os_wait_obj_init(pil_wait_obj_type *wait_obj)
{
  timer_error_type Terr;
  
  PIL_VERIFY(wait_obj != NULL, "pil_os_wait_obj_init: wait_obj is NULL", 0, 0, 0);
  
  qurt_signal_init(&(wait_obj->signal));
  
  wait_obj->sig_val = PIL_OS_WAIT_SIG_VALUE;
  
  Terr = timer_def_osal(&(wait_obj->timer), &timer_non_defer_group, TIMER_NATIVE_OS_SIGNAL_TYPE, &(wait_obj->signal), PIL_OS_WAIT_SIG_VALUE);
  
  if(Terr != TE_SUCCESS)
  {
    qurt_signal_destroy(&(wait_obj->signal));
    return PIL_FAILURE;
  }
  
  wait_obj->magic = PIL_OS_WAIT_OBJ_MAGIC;
  
  return PIL_SUCCESS;
} /* pil_os_wait_obj_init */


/*=============================================================================
FUNCTION 
  pil_os_start_wait_obj

DESCRIPTION
  Trigger a wait object with given timeout
  
DEPENDENCIES
  wait obj should be initialized before

RETURN VALUE
  None
=============================================================================*/
pil_error_type pil_os_start_wait_obj(pil_wait_obj_type *wait_obj, uint32 time_ms)
{
  PIL_VERIFY(wait_obj != NULL, "pil_os_start_wait_obj: wait_obj is NULL", 0, 0, 0);
  PIL_VERIFY(wait_obj->magic == PIL_OS_WAIT_OBJ_MAGIC, "pil_os_start_wait_obj: magic is not PIL_OS_WAIT_OBJ_MAGIC", 0, 0, 0);
  
  wait_obj->time_ms = time_ms;
  
  if(time_ms == 0)
    return PIL_SUCCESS;
  
  qurt_signal_clear(&(wait_obj->signal), PIL_OS_WAIT_SIG_VALUE);
  timer_set(&(wait_obj->timer), time_ms, 0, T_MSEC);

  return PIL_SUCCESS;
} /* pil_os_start_wait_obj */


/*=============================================================================
FUNCTION 
  pil_os_stop_wait_obj

DESCRIPTION
  Stops a wait object
  
DEPENDENCIES
  wait obj should be initialized before

RETURN VALUE
  None
=============================================================================*/
#if 0 //not needed
pil_error_type pil_os_stop_wait_obj(pil_wait_obj_type *wait_obj)
{
  PIL_VERIFY(wait_obj != NULL, "pil_os_stop_wait_obj: wait_obj is NULL", 0, 0, 0);
  PIL_VERIFY(wait_obj->magic == PIL_OS_WAIT_OBJ_MAGIC, "pil_os_stop_wait_obj: magic is not PIL_OS_WAIT_OBJ_MAGIC", 0, 0, 0);
  
  (void)timer_stop(&(wait_obj->timer), T_MSEC, NULL);

  return PIL_SUCCESS;
} /* pil_os_stop_wait_obj */
#endif


/*=============================================================================
FUNCTION 
  pil_os_check_wait_obj

DESCRIPTION
  Returns if the wait has expired. If so, returns TRUE. 
  If not expired returns FALSE
  
DEPENDENCIES
  wait obj should be initialized before

RETURN VALUE
  None
=============================================================================*/
boolean pil_os_check_wait_obj(pil_wait_obj_type *wait_obj)
{
  uint32 sig_val;
  
  PIL_VERIFY(wait_obj != NULL, "pil_os_check_wait_obj: wait_obj is NULL", 0, 0, 0);
  PIL_VERIFY(wait_obj->magic == PIL_OS_WAIT_OBJ_MAGIC, "pil_os_check_wait_obj: magic is not PIL_OS_WAIT_OBJ_MAGIC", 0, 0, 0);
  
  //If time_ms is 0, it means there is no timeout
  if(wait_obj->time_ms == 0)  
    return FALSE;
   
  sig_val = qurt_signal_get(&(wait_obj->signal));
  
  return (sig_val & PIL_OS_WAIT_SIG_VALUE);
} /* pil_os_check_wait_obj */


/*=============================================================================
FUNCTION 
  pil_os_wait_sig_obj_deinit

DESCRIPTION
  De-initialize wait object
  
DEPENDENCIES
  wait obj should be initialized before

RETURN VALUE
  None
=============================================================================*/
pil_error_type pil_os_wait_sig_obj_deinit(pil_wait_obj_type *wait_obj)
{
  PIL_VERIFY(wait_obj != NULL, "pil_os_wait_sig_obj_deinit: wait_obj is NULL", 0, 0, 0);
  PIL_VERIFY(wait_obj->magic == PIL_OS_WAIT_OBJ_MAGIC, "pil_os_wait_sig_obj_deinit: magic is not PIL_OS_WAIT_OBJ_MAGIC", 0, 0, 0);
 
  (void)timer_undef(&(wait_obj->timer));
  
  qurt_signal_destroy(&(wait_obj->signal));
  
  wait_obj->magic = 0;
  
  return PIL_SUCCESS;
} /* pil_os_wait_sig_obj_deinit */

