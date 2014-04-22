#ifndef __PIL_OS_H__
#define __PIL_OS_H__

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

$Header: //components/rel/core.tx/6.0/debugtools/subsys/src/pil_os.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------

09/15/16   ab      File created.

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "comdef.h"
#include "qurt.h"
#include "timer.h"

/*=============================================================================

                            DATA DECLARATIONS
                            
=============================================================================*/

typedef struct pil_sig_obj_t
{
  qurt_signal_t signal;
}pil_sig_obj_type;

typedef struct pil_mutex_obj_t
{
  qurt_mutex_t mutex;
}pil_mutex_obj_type;

typedef struct pil_wait_obj_t
{
  uint32 magic;
  qurt_signal_t signal;
  uint32 sig_val;
  timer_type timer;
  uint32 time_ms;
}pil_wait_obj_type;

typedef pil_mutex_obj_type* pil_mutex_obj_ptr;
typedef pil_sig_obj_type* pil_sig_obj_ptr;
typedef uint32 pil_sigs_type;


/*=============================================================================

                            FUNCTION DECLARATIONS
                            
=============================================================================*/
//Note: For details please look at function header in related .c OS file (ex: pil_qurt.c)

//Signal Functions
void pil_os_sig_init(pil_sig_obj_ptr sig_obj);
pil_sigs_type pil_os_sig_wait(pil_sig_obj_ptr sig_obj, pil_sigs_type wait_sigs);
pil_sigs_type pil_os_sig_set(pil_sig_obj_ptr sig_obj, pil_sigs_type set_sigs);
void pil_os_sig_clr(pil_sig_obj_ptr sig_obj, pil_sigs_type clr_sigs);

//Mutex Functions
pil_error_type pil_os_mutex_init(pil_mutex_obj_ptr mutex_obj);
pil_error_type pil_os_mutex_lock(pil_mutex_obj_ptr mutex_obj);
pil_error_type pil_os_mutex_unlock(pil_mutex_obj_ptr mutex_obj);
pil_error_type pil_os_mutex_deinit(pil_mutex_obj_ptr mutex_obj);

//cache functions
void pil_os_cache_flush(void);
void pil_os_mem_barrier(void);

//Wait Obj functions
pil_error_type pil_os_wait_obj_init(pil_wait_obj_type *wait_obj);
pil_error_type pil_os_start_wait_obj(pil_wait_obj_type *wait_obj, uint32 time_ms);
//pil_error_type pil_os_stop_wait_obj(pil_wait_obj_type *wait_obj);
boolean pil_os_check_wait_obj(pil_wait_obj_type *wait_obj);
pil_error_type pil_os_wait_sig_obj_deinit(pil_wait_obj_type *wait_obj);

#endif /* __PIL_OS_H__ */
