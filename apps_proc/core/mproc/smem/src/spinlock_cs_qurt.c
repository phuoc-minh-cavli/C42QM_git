
/*===========================================================================

               S P I N L O C K   C R I T I C A L  S E C T I O N S

===========================================================================*/
/**
  @file spinlock_cs.c
*/
/* Abstracts the OS specific implementation of Critical Section used by 
   spinlocks.
*/


/* NOTE: For the output PDF generated using Doxygen and Latex, all file and group 
         descriptions are maintained in the SMEM_mainpage file. To change any of the 
         the file/group text for the PDF, edit the SMEM_mainpage file, or contact 
         Tech Pubs.

         The above description for this file is part of the "smem" group description 
         in the SMEM_mainpage file. 
*/

/*===========================================================================
Copyright (c) 2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/smem/src/spinlock_cs_qurt.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/11/11   hwu     Moved assert.h and err.h to os file.
01/25/11   rs      Initial revision of DALSYS critical sections used by 
                   spinlocks.
===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "qurt.h"
#include "smem_os.h"
#include "spinlock_cs.h"

/*===========================================================================

                        MACRO DEFINITIONS

===========================================================================*/

#define SMEM_SPIN_LOCK_CS_CEILING_PRIO    0 /* High priory */

/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/

typedef struct
{    
  uint32       orig_prio;    /* For preemption disable on QURT */
  qurt_mutex_t mutex;
} spinlocks_cs_type;

/*===========================================================================

                        LOCAL DATA DEFINITIONS

===========================================================================*/

/** The critical section structure for the spinlock */
static spinlocks_cs_type spinlocks_cs;

/*===========================================================================

                      PRIVATE FUNCTION DECLARATIONS
                      
===========================================================================*/

/*===========================================================================
  FUNCTION  smem_spin_lock_disable_preemption
===========================================================================*/
/**
  Disables preemption by setting high priority to current thread.
  
  ** THIS CODE IS COPIED FROM DAL SYS **
 
 @param[in]  cs     critical section pointer
 
 @return
  None

  @dependencies 
  None.

  @sideeffects
  None.
*/
/*==========================================================================*/
static void smem_spin_lock_disable_preemption(spinlocks_cs_type *cs)
{
  if(!cs->orig_prio) /* is this a nested call to disable preemption ? */
  {
    int qurt_id = qurt_thread_get_id();
    /* store original priority */
    cs->orig_prio = (0x0000FFFF & qurt_thread_get_priority(qurt_id));
    qurt_thread_set_priority(qurt_id, SMEM_SPIN_LOCK_CS_CEILING_PRIO);
  }

  /* maintain a count in the top 16 bits to support nested calls */
  cs->orig_prio += 0x10000;
}

/*===========================================================================
  FUNCTION  smem_spin_lock_disable_preemption
===========================================================================*/
/**
  Enables preemption by restoring the thread.
  
  ** THIS CODE IS COPIED FROM DAL SYS **
 
 @param[in]  cs     critical section pointer
 
 @return
  None

  @dependencies 
  None.

  @sideeffects
  None.
*/
/*==========================================================================*/
static void smem_spin_lock_sync_enable_preemption(spinlocks_cs_type *cs)
{
   cs->orig_prio -= 0x10000;
   if(!(cs->orig_prio&0xFFFF0000)) /* check count of nested calls*/
   {
      qurt_thread_set_priority(qurt_thread_get_id(), cs->orig_prio);
      cs->orig_prio = 0x0;
   }
}


/*===========================================================================

                      PUBLIC FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
  FUNCTION  smem_spin_lock_cs_init
===========================================================================*/
/**
  Creates the non-preemtable critical section used to acquire the spinlock
  from different tasks.
 
  @return
  None

  @dependencies 
  None.

  @sideeffects
  None.
*/
/*==========================================================================*/
void smem_spin_lock_cs_init( void )
{
  memset(&spinlocks_cs, 0, sizeof(spinlocks_cs));
  
  /* Initialize the critical section */
  qurt_mutex_init(&spinlocks_cs.mutex);
}

/*===========================================================================
  FUNCTION  smem_spin_lock_cs_lock
===========================================================================*/
/**
  Locks the critical section used by the spinlock.
 
  This function uses the OS-supplied mutual exclusion primitives so that
  multiple tasks that try to acquire the spinlock can be scheduled, elevating
  priority to prevent priority-inversion deadlocks.
 
  In order to avoid context switches while the spinlock is held, the task's
  priority is elevated on multi-core processors, or preemption is disabled
  entirely on single-core processors. That way the amount of time the remote
  processor spins is reduced to the smallest amount of time possible
  while the spinlock is held.
 
  @return
  None

  @dependencies 
  None.

  @sideeffects
  Prevents context switch.
*/
/*==========================================================================*/
void smem_spin_lock_cs_lock( void ) 
{
  /* Lock the non-preemtable critical section */
  qurt_mutex_lock(&spinlocks_cs.mutex);
  smem_spin_lock_disable_preemption(&spinlocks_cs);
}

/*===========================================================================
  FUNCTION  smem_spin_lock_cs_unlock
===========================================================================*/
/**
  Releases the critical section used by the spinlock.
 
  This function uses the OS-supplied mutual exclusion primitives so that
  multiple tasks that try to release the spinlock can be scheduled, allowing
  context switch once the spinlock is unlocked.
 
  @return
  None

  @dependencies 
  None.

  @sideeffects
  Allows context switch.
*/
/*==========================================================================*/
void smem_spin_lock_cs_unlock( void )
{
  /* Unlock the non-preemptable critical section */
  smem_spin_lock_sync_enable_preemption(&spinlocks_cs);
  qurt_mutex_unlock(&spinlocks_cs.mutex);
}
