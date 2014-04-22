/*
 * mqtt_port.c
 */
 
/*
 * Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "mqtt_port.h"

#ifndef MOB_TX
#include "qurt_txm_qapi_fwk.h"
#endif

#include "timer.h"

#define     IN_SEM_EVENT            (0x1) 
#define     CLEAR_EVENT_ON_EXIT     TRUE 
#define     EXIT_ON_ANY_EVENT       FALSE 
extern qurt_mutex_t * mqtt_mutex;

void
_dtrap(char *file, int line)
{
   volatile int loop = 1;
   TRAP;
   while (loop)
   {
      /* null */ ;
   }
}

void *
npalloc(uint32_t bytes)
{
   void *ptr;

   /* round number of bytes to a multiple of 4 */
   bytes = ((bytes + 3) & ~0x3);

#ifdef TRACE_MEMORY
   if (bytes == 0)
      DTRAP();
#endif

#ifdef FREE_RTOS
   ptr = pvPortMalloc(bytes);
#else
   ptr = malloc(bytes);
#endif
   if (ptr)
   {
#ifdef TRACE_MEMORY
      int  i;
      struct in_mem_struct *mp = &in_memory[0];

      if (!in_mfull)
      {
         /* find an empty slot in the memory trace array */
         for (i = 0; i < in_memx; i++)
         {
            if (mp->in_maddr == NULL)
            {
               break;
            }
            mp++;
         }

         /* if we didn't find an entry, append a new entry to the
          * end of the table. If the memory trace table is full,
          * notify the user and call DTRAP().
          */
         if (i >= in_memx)
         {
            if (++in_memx >= IN_MSIZE)
            {
               in_mfull = TRUE;
               panic("memory trace overflow");
            }
         }

         /* add entry and update statistics */
         mp->in_maddr = ptr;
         mp->in_msize = bytes;
         in_mallocs++;
         if ((in_mused += bytes) > in_mallocated)
            in_mallocated = in_mused;
      }
#endif

      MEMSET(ptr, 0, bytes);
   }
#ifdef NPDEBUG
   else
   {
      LOG_ERROR("npalloc error. size = %d\n", bytes);
   }
#endif

   return (ptr);
}

int mqtt_mutex_lock(qurt_mutex_t *  mutex)
{
  qurt_mutex_lock(mutex);
  return 0;
}

int mqtt_mutex_unlock(qurt_mutex_t *  mutex)
{
  qurt_mutex_unlock(mutex);
  return 0;
}


void
npfree(void *ptr)
{
#ifdef TRACE_MEMORY
   int  i;
   struct in_mem_struct *mp = &in_memory[in_memx-1];

   if (ptr == NULL)
      DTRAP();

   /* find the allocation entry in memory trace array */
   for (i = in_memx-1; i >= 0; i--)
   {
      if (ptr == mp->in_maddr)
      {
         in_mfreed += mp->in_msize;    /* update statistics */
         in_mfrees++;
         in_mused  -= mp->in_msize;
         in_mallocs--;
         mp->in_maddr = NULL;          /* clear entry */
         mp->in_msize = 0;
         break;
      }
      mp--;
   }
   if (i < 0)
      DTRAP();    /* Trying to free an unallocated block */
#endif
#ifdef FREE_RTOS
   vPortFree(ptr);
#else
   free(ptr);
#endif
}

uint32_t mqtt_event_set(tk_event_handle event_ptr, uint32_t bitsToSet)
{
   uint32_t os_ret;

   qurt_signal_set(event_ptr, bitsToSet);
   os_ret = QURT_EOK;

   return os_ret;
}


IN_MUTEX
mqtt_mutex_create(void)
{
   qurt_mutex_t *mutex = malloc(sizeof(qurt_mutex_t));

   if(mutex)
   {
      memset(mutex, 0, sizeof(qurt_mutex_t));
      qurt_mutex_init(mutex);
   }

   return (mutex);
}

int mqtt_sem_post(IN_SEM sem)
{
   return mqtt_event_set(sem, IN_SEM_EVENT);
}


IN_SEM
mqtt_sem_create(void)
{
    IN_SEM sem;

    sem = (IN_SEM)npalloc(sizeof(qurt_signal_t));
    if (sem)
    {
        memset(sem, 0, sizeof(qurt_signal_t));
        qurt_signal_init((qurt_signal_t *)sem);
    }

    return sem;
}


void
mqtt_wake(TASK tk)
{
   if (!tk)
   {
      DTRAP();                /* cannot find the task! */
   }

   qurt_thread_resume(tk);
}


static TASK
mqtt_current_task(void)
{
   return (TASK) qurt_thread_get_id();
}


void
mqtt_suspend(TASK tk)
{
   if (tk == (TASK)NULL)
      tk = mqtt_current_task();

   if (tk == (TASK)NULL)
   {
      DTRAP();                /* cannot find the task! */
   }

   qurt_thread_suspend(tk);
}


void
mqtt_sem_free(IN_SEM sem)
{
    if (sem)
    {
        qurt_signal_destroy(sem);
        npfree(sem);
    }
    return;
}

uint32_t mqtt_event_wait(tk_event_handle event_ptr,
                       uint32_t bitsToWaitFor,
                       uint32_t all,
                       uint32_t var1,
                       uint32_t ticksToWait)
{
   uint32_t    os_ret;
   uint32      attribute    = 0;
   uint32      curr_signals;
   //qurt_time_t qtime;

   if (all == TRUE)
   {
      attribute |= QURT_SIGNAL_ATTR_CLEAR_MASK;
   }
   if (var1 == TRUE)
   {
      attribute |= QURT_SIGNAL_ATTR_WAIT_ALL;
   }

   os_ret = qurt_signal_wait_timed_ext(event_ptr, bitsToWaitFor, attribute, &curr_signals, ticksToWait);

   return os_ret;
}


int
mqtt_mutex_pend(IN_MUTEX mutex, int32_t timeout)
{
   int  err;

   if (!mutex)
   {
      DTRAP();
      return (-1);
   }

   switch (timeout)
   {
      case INFINITE_DELAY:
         qurt_mutex_lock((qurt_mutex_t *)mutex);
         err = QURT_EOK;
         break;
      case 0:
         err = qurt_mutex_try_lock((qurt_mutex_t *)mutex);
         break;
      default:
      {
         err = qurt_mutex_lock_timed_ext((qurt_mutex_t *)mutex, timeout);
         break;
      }
   }

   return ((err == QURT_EOK) ? ESUCCESS : TK_TIMEOUT);
}

int
mqtt_sem_pend(IN_SEM sem, int32_t timeout)
{
   return mqtt_event_wait(sem, IN_SEM_EVENT, CLEAR_EVENT_ON_EXIT, EXIT_ON_ANY_EVENT, timeout);
}

int
mqtt_mutex_post(IN_MUTEX mutex)
{
   if (!mutex)
   {
      DTRAP();
      return (-1);
   }

   qurt_mutex_unlock((qurt_mutex_t *)mutex);

   return (ESUCCESS);
}


void mqtt_mutex_free(IN_MUTEX mutex)
{
   /* TODO: Do we need to protect against destroying in-use mutexes? */
   if(mutex)
   {
      qurt_mutex_destroy((qurt_mutex_t *)mutex);

      free((qurt_mutex_t *)mutex);
   }
}


void* mqtt_memcpy(void *d, const void *s, size_t n)
{
    size_t i;
    uintptr_t align = sizeof(uintptr_t) - 1;

    if (((uintptr_t)d & align) || ((uintptr_t)s & align) || (n & align)) {
        char *bs = (char *)s;
        char *bd = (char *)d;

        // Note: This could be optimised. 
        for (i = 0; i < n; i++) {
            *bd++ = *bs++;
        }
    } else {                     //memcpy - word aligned 
        uintptr_t *ws = (uintptr_t *)s;
        uintptr_t *wd = (uintptr_t *)d;

        n /= sizeof(uintptr_t);

        for (i = 0; i < n; i++)
            *wd++ = *ws++;
    }
    return d;
}

