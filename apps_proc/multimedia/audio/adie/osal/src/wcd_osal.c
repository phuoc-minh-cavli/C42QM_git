/**
@file wcd_osal.c
@brief 
  os abstraction layer for WCD driver
*/
/*-------------------------------------------------------------------------------
     Copyright (c) 2011 Qualcomm Technologies Incorporated.
     All rights reserved.
     Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
                             Edit History
$Id: //components/rel/audio.tx/2.0/audio/adie/osal/src/wcd_osal.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/15/13   OA      removed banned usage for memcpy function
01/28/11   SK      initial version.
===============================================================================*/
#include "wcd_osal.h"
#include "csd_os_dependencies.h"
//Check where is it used.
#include "DDITimer.h"

/**************************************************************************
 ****************           DEFINES                     *******************
 ***************************************************************************/

/* OSAL Features. */
#define MULTI_EVENT_SUPPORT
//#define DEBUG_OSAL

#define MS_TO_US(time)      ( (time)*1000 )
#define MS_TO_NS(time)      ( (time)*( (uint64_t)1000000 ) )
#define WCD_OSAL_MAX_EVENTS ( 20 )
#define MAX_WCD_THREADS     ( 3 )

/**************************************************************************
 ****************           DEFINITIONS                  *******************
 ***************************************************************************/

typedef struct {

  int32 autoreset;
  csd_event_t eventObject;
#ifdef MULTI_EVENT_SUPPORT
  int32 counter;
  csd_lock_t eventMutex;
#endif
} wcd_osal_event;

typedef struct {

  csd_timer_t timerObject;
  wcd_osal_timer_callback_fcn callback;
  void* params;
  int32 isPeriodic;
} wcd_osal_timer;

typedef struct 
{
  int32 inuse;
  wcd_osal_thread entry_fn;
  void* parameter;
  csd_thread_t threadObject;
} wcd_osal_thread_context;

/**************************************************************************
 ****************           GLOBAL VARIABLES            *******************
 ***************************************************************************/

uint32 wcd_osal_thread_context_initialized = FALSE;
int32 g_wcd_malloc_counter = 0;
static wcd_osal_thread_context wcd_osal_thread_context_array[MAX_WCD_THREADS];
static csd_mem_handle_t phyMemHandle = NULL;


/**************************************************************************
 ****************            WCD MEMORY                 *******************
 ***************************************************************************/


void WCD_OSAL_FREE_imp ( 
  void* ptr,
  uint8 tag 
)
{
  HANDLE_UNUSED_PARAMETER( tag );

#ifdef DEBUG_OSAL
  if( ptr )
  {
    WCD_DEBUG_INFO_2("=WCD_OSAL: <free memory 0x%x %d pointers left>\n", ptr, --g_wcd_malloc_counter );
  }
  else
  {
    WCD_DEBUG_INFO_2("=WCD_OSAL: <free memory trying to free a null pointer %d pointers left>\n", ptr, g_wcd_malloc_counter );
  }
#endif

  if( ptr )
  {
    csd_free( ptr );
  }

  return;
}


void* WCD_OSAL_MALLOC_imp (
  size_t size,
  uint8 tag
)
{
  void* ptr = NULL;
  HANDLE_UNUSED_PARAMETER( tag );

  csd_malloc( size, (void**)&ptr );

#ifdef DEBUG_OSAL
  if( NULL == ptr )
  {
    WCD_DEBUG_INFO_3("=WCD_OSAL: <alloc memory 0x%x %d pointers size = %d>\n", ptr, ++g_wcd_malloc_counter, size );
  }
#endif

  return ptr;
}

void WCD_MEMSET_imp (
  void* ptr,
  uint8 pattern,
  size_t size
)
{
  uint32_t pattern_32 = pattern|(pattern<<8)|(pattern<<16)|(pattern<<24);

#ifdef DEBUG_OSAL
  WCD_DEBUG_INFO_2("=WCD_OSAL: <set memory 0x%x %d bytes>\n", ptr, size );
#endif

  csd_memset( ptr, pattern_32, size );
  return;
}


void WCD_MEMCOPY_imp (
  void* source,
  size_t source_size,
  void* destination,
  size_t dest_size
)
{
#ifdef DEBUG_OSAL
  WCD_DEBUG_INFO_3("=WCD_OSAL: <memory copy 0x%x = 0x%x, %d bytes>\n", destination, source, dest_size );
#endif

  csd_memscpy( destination, dest_size, source, source_size );
}


/*This implementation assumes the code will only require one single physical
  memory pointer, if more than one pointer is required this implementation
  needs to be changed. if the api is called twice the second call will return 
  NULL*/
void* WCD_OSAL_PMEM_MALLOC_imp( 
  size_t size
)
{
  uint8_t* phyMemAddr = NULL;

  if( NULL == phyMemHandle )
  {
    csd_mem_alloc( size, &phyMemHandle, &phyMemAddr);
  }

#ifdef DEBUG_OSAL
  if( phyMemAddr )
  {
    /*on different targets the actual physical memory is shown here, keep the message
      this way so it can be used using the same parser*/
    WCD_DEBUG_INFO_1("=WCD_OSAL: <alloc memory 0x%x (physical)>", phyMemAddr );
  }
#endif

  return phyMemAddr;
}

void  WCD_OSAL_PMEM_FREE_imp( void* ptr )
{ 
  uint32_t rc = CSD_OSAL_EOK;

#ifdef DEBUG_OSAL
  if( ptr )
  {
    WCD_DEBUG_INFO_1("=WCD_OSAL: <free memory 0x%x (physical)>", ptr );
  }
  else
  {
    WCD_DEBUG_INFO_0("=WCD_OSAL: <free physical memory trying to free a null pointer>");
  }
#endif  

  if( NULL != phyMemHandle)
  {
    rc = csd_mem_free( phyMemHandle );
    phyMemHandle = NULL;

    if( rc != CSD_OSAL_EOK )
    {
#ifdef DEBUG_OSAL    
      WCD_DEBUG_INFO_0("=WCD_OSAL: DALSYS_DestroyObject returned an error while trying to free physical memory");
#endif
    }
  }
}

/***************************************************************************
 ****************            WCD THREAD                 ********************
 ***************************************************************************/

wcd_osal_thread_context* wcd_osal_get_unused_thread_context()
{
  wcd_osal_thread_context* thread = NULL;
  int32 i = 0;
  
  if( wcd_osal_thread_context_initialized == FALSE )
  {
    csd_memset( &wcd_osal_thread_context_array, 0, sizeof(wcd_osal_thread_context_array) );
    wcd_osal_thread_context_initialized = WCD_TRUE;
  }
  
  for ( ; i < MAX_WCD_THREADS; i++ )
  {
    if( wcd_osal_thread_context_array[i].inuse == FALSE )
    {
      wcd_osal_thread_context_array[i].inuse = WCD_TRUE;
      thread = &(wcd_osal_thread_context_array[i]);
      break;
    }
  }

#ifdef DEBUG_OSAL
  WCD_DEBUG_INFO_1("=WCD_OSAL: <get unused context = 0x%x>\n", rc );
#endif

  return thread;
}


void wcd_osal_free_thread ( 
  wcd_osal_thread_context* thread
)
{
  int32 i = 0;

#ifdef DEBUG_OSAL
  WCD_DEBUG_INFO_1("=WCD_OSAL: <free thread context (0x%x)>\n", thread );
#endif

  for ( ; i < MAX_WCD_THREADS; i++ )
  {
    if( thread == &(wcd_osal_thread_context_array[i]) )
    {
      csd_memset( thread, 0, sizeof(wcd_osal_thread_context) );
      break;
    }
  }

  return;
}

int32_t wcd_thread_function_wrapper( void* parameter )
{
  wcd_osal_thread_context* thread = (wcd_osal_thread_context*) parameter;
  
  if( thread != NULL )
  {
    thread->entry_fn( thread->parameter );

    /*release memory used by the thread*/
    csd_thread_destroy( (csd_thread_t) thread->threadObject);
    wcd_osal_free_thread ( thread );
  }

  return 0;
}


WCD_HANDLE WCD_OSAL_CREATE_THREAD_imp( 
   wcd_osal_thread wcd_thread_function,
   void* thread_parameter,
   wcd_osal_priority_t priority
)
{
  wcd_osal_thread_context * thread = NULL;
  uint32 priority_value;
  uint32_t rc = CSD_OSAL_EOK;

  if ( priority > E_priority_veryhigh )
  {
#ifdef DEBUG_OSAL
    WCD_DEBUG_INFO_0("=WCD_OSAL: <create thread. invalid priority.>\n");
#endif
    return (WCD_HANDLE)thread;
  }

  thread = wcd_osal_get_unused_thread_context();
  if ( NULL == thread ) return (WCD_HANDLE)thread;

  thread->entry_fn = wcd_thread_function;
  thread->parameter = thread_parameter;
  switch(priority)
  {
    case E_priority_low:
      priority_value = 50;
      break;
    case E_priority_normal:
      priority_value = 100;
      break;
    case E_priority_high:
      priority_value = 96;
      break;
    case E_priority_veryhigh:
      priority_value = 200;
      break;
  }

  rc = csd_thread_create( &thread->threadObject, NULL, priority_value, NULL, 0,
                          wcd_thread_function_wrapper, thread );
  if ( CSD_OSAL_EOK != rc ) 
  {
    wcd_osal_free_thread( thread );
    thread = NULL;
  }

#ifdef DEBUG_OSAL
    WCD_DEBUG_INFO_1("=WCD_OSAL: <create thread  context = 0x%x>\n", thread );
#endif

  return (WCD_HANDLE) thread;
}

int32 WCD_OSAL_JOIN_THREAD ( 
  WCD_HANDLE threadhandle,
  wcd_osal_kill_thread_fcn kill_function,
  void* param
)
{
  int32 rc = WCD_JOIN_FAILED;
  wcd_osal_thread_context* thread = (wcd_osal_thread_context*)threadhandle;

  if( thread && thread->inuse )
  {
    //if killfunction is null just wait the threads dead.
    if( kill_function )
    {
      kill_function(param);
    }

    //free memory used to control the thread pool.
    csd_thread_destroy( (csd_thread_t) thread->threadObject);
    wcd_osal_free_thread( thread );
    rc = WCD_JOIN_DONE;
  }

  return rc;
}

/**************************************************************************
 ****************            WCD MUTEX                  *******************
***************************************************************************/

void* WCD_OSAL_CREATE_MUTEX_imp( void )
{
  csd_lock_t mutex = NULL;

  csd_lock_create( &mutex );

#ifdef DEBUG_OSAL
  WCD_DEBUG_INFO_1("=WCD_OSAL: <create mutex 0x%x>\n", mutex );
#endif

  return (void*)mutex;
}

void WCD_OSAL_DESTROY_MUTEX_imp( void* mutex )
{
#ifdef DEBUG_OSAL
  WCD_DEBUG_INFO_1("=WCD_OSAL: <destroy mutex 0x%x>\n", mutex );
#endif

  csd_lock_destroy( (csd_lock_t) mutex );
  return;
}


int32 WCD_OSAL_WAIT_MUTEX_imp( void* mutex, int32 timeout_in_ms )
{
#ifdef DEBUG_OSAL
  WCD_DEBUG_INFO_1("=WCD_OSAL: <hold mutex 0x%x>\n", mutex );
#endif

  ( void ) csd_lock_enter( (csd_lock_t) mutex );
  return WCD_WAIT_DONE;
}

int32 WCD_OSAL_RELEASE_MUTEX_imp( void* mutex )
{
#ifdef DEBUG_OSAL
  WCD_DEBUG_INFO_1("=WCD_OSAL: <release mutex 0x%x>\n", mutex );
#endif

  ( void ) csd_lock_leave( (csd_lock_t) mutex );
  return WCD_WAIT_DONE;
}


/**************************************************************************
 ****************            WCD EVENT                  *******************
 ***************************************************************************/



void* WCD_OSAL_CREATE_EVENT_imp( 
  int32 manual_reset,
  int32 initial_state
)
{
  wcd_osal_event* event = NULL;
  uint32_t rc = CSD_OSAL_EOK;

  for ( ;; )
  {
    csd_malloc( sizeof(wcd_osal_event), (void**)&event );
	if( NULL == event ) break;

    csd_memset( event, 0, sizeof(wcd_osal_event) );

    rc = csd_event_create( &event->eventObject );
    if( CSD_OSAL_EOK != rc )
    {
      csd_free( (void*)event );
      break;
    }

#ifdef MULTI_EVENT_SUPPORT
    csd_lock_create( &event->eventMutex );
    if( CSD_OSAL_EOK != rc )
    {
      csd_event_destroy( event->eventObject );
      csd_free( (void*)event );
      break;
    }
#endif
    event->autoreset = !manual_reset;

    if( WCD_TRUE == initial_state )
    {
      csd_event_signal( event->eventObject );
    }

    break;
  }

#ifdef DEBUG_OSAL
  WCD_DEBUG_INFO_1("=WCD_OSAL: <create event 0x%x >\n",event);
#endif

  return (void*) event;
}

void WCD_OSAL_DESTROY_EVENT_imp( 
  void* handle
)
{
  wcd_osal_event* event = (wcd_osal_event*) handle;

  if( NULL != event )
  {

#ifdef DEBUG_OSAL
    WCD_DEBUG_INFO_2("=WCD_OSAL: <destroy event 0x%x counter = %d>\n",event, (((wcd_osal_event*)event)->counter));
#endif

#ifdef MULTI_EVENT_SUPPORT
    csd_lock_destroy( event->eventMutex );
#endif

    csd_event_destroy( event->eventObject );
    csd_free( event );
  }

  return;
}


int32 WCD_OSAL_WAIT_EVENT_imp( 
  void* handle,
  int32 timeout_in_ms
)
{
  int32 rc = WCD_WAIT_FAILED;
  wcd_osal_event* event = (wcd_osal_event*)handle;

#ifdef DEBUG_OSAL
  WCD_DEBUG_INFO_1("=WCD_OSAL: <wait for event 0x%x>\n", event );
#endif

  if( NULL != event )
  {
    uint32_t result = CSD_OSAL_EOK;

#ifdef MULTI_EVENT_SUPPORT
    int32 temp_counter;

    WCD_OSAL_WAIT_MUTEX_imp(event->eventMutex, WCD_INFINITE);
    temp_counter = event->counter;
    WCD_OSAL_RELEASE_MUTEX_imp(event->eventMutex);

#ifdef DEBUG_OSAL
      WCD_DEBUG_INFO_1("=WCD_OSAL: <event counter = %d>\n", (event->counter));
#endif

    if( temp_counter == 0 )
#endif
    { 
      if( WCD_INFINITE != timeout_in_ms )
      {
        result = csd_event_wait_timed( (csd_event_t)event, timeout_in_ms );
      }
      else
      {
        result = csd_event_wait( (csd_event_t)event );
      }
    }

    rc = (CSD_OSAL_EOK != result)? WCD_WAIT_FAILED : WCD_WAIT_DONE;

    if( WCD_TRUE == event->autoreset )
    {
      /*Note: Inorder to reset/clear the event using timeout valude of zero. */
      csd_event_wait_timed( (csd_event_t) event, 0 );
    }
  }

  return rc;
}

void WCD_OSAL_SET_EVENT_imp( 
  void* handle
)
{
 wcd_osal_event* event = (wcd_osal_event*)handle;

#ifdef DEBUG_OSAL
  WCD_DEBUG_INFO_1("=WCD_OSAL: <set event 0x%x>\n", event);
#endif

  if( event )
  {
#ifdef MULTI_EVENT_SUPPORT
    csd_lock_enter( event->eventMutex );
    ++(event->counter);
    csd_lock_leave(event->eventMutex);

#ifdef DEBUG_OSAL
    WCD_DEBUG_INFO_1("=WCD_OSAL: <event counter = %d>\n", event->counter);
#endif
#endif

    csd_event_signal( (csd_event_t) event );
  }

  return;
}

void WCD_OSAL_RESET_EVENT_imp ( 
  void* handle
)
{
  wcd_osal_event* event = (wcd_osal_event*) handle;

#ifdef DEBUG_OSAL
  WCD_DEBUG_INFO_1("=WCD_OSAL: <reset event 0x%x>\n", event);
#endif


  if ( NULL != event )
  {

#ifdef MULTI_EVENT_SUPPORT
    csd_lock_enter( event->eventMutex );
    if( event->counter > 0 )
    {
      (event->counter)--;
    }
    csd_lock_leave( event->eventMutex );
#ifdef DEBUG_OSAL
    WCD_DEBUG_INFO_1("=WCD_OSAL: <event counter = %d>\n", event->counter);
#endif
#endif

    /*Note: Inorder to reset/clear the event using timeout valude of zero. */
    csd_event_wait_timed( (csd_event_t) event, 0 );
  }

  return;
}


/**************************************************************************
 ****************            WCD TIMER                  *******************
 ***************************************************************************/



static void wcd_timer_wrapper_cb(
  void* handle
)
{
  wcd_osal_timer* timer = (wcd_osal_timer*) handle;

  if( timer )
  {
    if( timer->isPeriodic == FALSE )
    {
      csd_timer_stop( timer->timerObject );
    }

    timer->callback( timer->params );
  }

  return;
}


void* WCD_CREATE_TIMER_imp (
  wcd_osal_timer_callback_fcn callback,
  void* param,
  int32 isPeriodic
)
{
  wcd_osal_timer* timer = NULL;
  uint32_t rc = CSD_OSAL_EOK;

  for ( ;; )
  {
    csd_malloc( sizeof(wcd_osal_timer), (void**)&timer );
    if( NULL == timer ) break;

    timer->callback = callback;
    timer->params = param;
    timer->isPeriodic = isPeriodic;

    rc = csd_timer_create( &timer->timerObject, wcd_timer_wrapper_cb, timer );
    if ( CSD_OSAL_EOK != rc )
    {
      csd_free( timer );
      timer = NULL;
    }

    break;
  }

#ifdef DEBUG_OSAL
  WCD_DEBUG_INFO_1("=WCD_OSAL: <create timer rc = 0x%x>\n", timer );
#endif

  return timer;
}


void  WCD_ENABLE_TIMER_imp (
  void* handle,
  int32 milliseconds 
)
{
  wcd_osal_timer* timer = (wcd_osal_timer*) handle;

#ifdef DEBUG_OSAL
      WCD_DEBUG_INFO_2( "=WCD_OSAL: <Timer enable 0x%x, %d milliseconds>\n", handle, milliseconds );
#endif

  if( handle )
  {
    /*Note: Granuality of CSD timer is in nanoseconds. */
    csd_timer_start( timer->timerObject, MS_TO_NS(milliseconds) );
  }

  return;
}

void  WCD_DISABLE_TIMER_imp ( 
  void* handle
)
{
  wcd_osal_timer* timer = (wcd_osal_timer*) handle;

#ifdef DEBUG_OSAL
      WCD_DEBUG_INFO_1("=WCD_OSAL: <Timer disable 0x%x>\n", handle );
#endif

  if( handle )
  {
    csd_timer_stop( timer->timerObject );
  }

  return;
}

void  WCD_DESTROY_TIMER_imp ( 
  void* handle
)
{
  wcd_osal_timer* timer = (wcd_osal_timer*) handle;

#ifdef DEBUG_OSAL
      WCD_DEBUG_INFO_1("=WCD_OSAL: <Timer Destroy 0x%x>\n", handle);
#endif

  if( handle )
  {
    csd_timer_destroy( timer->timerObject );
    csd_free( timer );
  }

  return;
}

/**************************************************************************
 ****************            WCD MISC                   *******************
 ***************************************************************************/


void  WCD_OSAL_SLEEP_imp(
  int32 timeout_milliseconds
)
{
  CSD_SLEEP( timeout_milliseconds );
}


void WCD_OSAL_GET_CURRENT_TIMESTAMP_imp(wcd_owner_module id)
{
  //not implemented
  /*
  if( id == E_WCD_MBHC )
  {
    clock_gettime(CLOCK_REALTIME, &mbhc_current_system_time);
    //clock_gettime(CLOCK_MONOTONIC, &mbhc_current_system_time);
#ifdef DEBUG_OSAL
    WCD_DEBUG_INFO_2("WCD_OSAL_GET_CURRENT_TIMESTAMP_imp: current_time_sec=(%ld), current_time_nsec=(%ld) \n", mbhc_current_system_time.tv_sec, mbhc_current_system_time.tv_nsec);    
#endif
  }
  else if( id == E_WCD_CORE )
  {
    clock_gettime(CLOCK_REALTIME, &wcd_current_system_time);
    //clock_gettime(CLOCK_MONOTONIC, &wcd_current_system_time);
#ifdef DEBUG_OSAL
    WCD_DEBUG_INFO_2("WCD_OSAL_GET_CURRENT_TIMESTAMP_imp: current_time_sec=(%ld), current_time_nsec=(%ld) \n", wcd_current_system_time.tv_sec, wcd_current_system_time.tv_nsec);    
#endif
  }
  */
}

void WCD_OSAL_GET_NEW_TIMESTAMP_imp(wcd_owner_module id)
{
  //not implemented
  /*
  if( id == E_WCD_MBHC )
  {
    clock_gettime(CLOCK_REALTIME, &mbhc_new_system_time);
    //clock_gettime(CLOCK_MONOTONIC, &mbhc_new_system_time);
#ifdef DEBUG_OSAL
    WCD_DEBUG_INFO_2("WCD_OSAL_GET_NEW_TIMESTAMP_imp: new_time_sec=(%ld), new_time_nsec=(%ld) \n", mbhc_new_system_time.tv_sec, mbhc_new_system_time.tv_nsec);    
#endif
  }
  else if( id == E_WCD_CORE )
  {
    clock_gettime(CLOCK_REALTIME, &wcd_new_system_time);
    //clock_gettime(CLOCK_MONOTONIC, &wcd_new_system_time);
#ifdef DEBUG_OSAL
    WCD_DEBUG_INFO_2("WCD_OSAL_GET_NEW_TIMESTAMP_imp: new_time_sec=(%ld), new_time_nsec=(%ld) \n", wcd_new_system_time.tv_sec, wcd_new_system_time.tv_nsec);    
#endif
  }
  */
}

void WCD_OSAL_GET_TIME_DURATION_imp(wcd_owner_module id, long *time)
{
  //not implemented
  /*
  if( id == E_WCD_MBHC )
  {
    *time = (long)( (long long)( (long long)((long long)(mbhc_new_system_time.tv_sec - mbhc_current_system_time.tv_sec) * 1000000000) + (long long)(mbhc_new_system_time.tv_nsec - mbhc_current_system_time.tv_nsec) ) / 1000000 );
#ifdef DEBUG_OSAL
    WCD_DEBUG_INFO_2("WCD_OSAL_GET_TIME_DURATION_imp: New_time_sec= (%ld), Current_time_sec= (%ld) \n", mbhc_new_system_time.tv_sec, mbhc_current_system_time.tv_sec);
    WCD_DEBUG_INFO_2("WCD_OSAL_GET_TIME_DURATION_imp: New_time_nsec=(%ld), Current_time_nsec=(%ld) \n", mbhc_new_system_time.tv_nsec, mbhc_current_system_time.tv_nsec);    
    WCD_DEBUG_INFO_1("WCD_OSAL_GET_TIME_DURATION_imp: Time_duration = (%ld) \n", *time);        
#endif
  }
  else if( id == E_WCD_CORE )
  {
    *time = (long)( (long long)( (long long)((long long)(wcd_new_system_time.tv_sec - wcd_current_system_time.tv_sec) * 1000000000) + (long long)(wcd_new_system_time.tv_nsec - wcd_current_system_time.tv_nsec) ) / 1000000 );
#ifdef DEBUG_OSAL
    WCD_DEBUG_INFO_2("WCD_OSAL_GET_TIME_DURATION_imp: New_time_sec= (%ld), Current_time_sec= (%ld) \n", wcd_new_system_time.tv_sec, wcd_current_system_time.tv_sec);
    WCD_DEBUG_INFO_2("WCD_OSAL_GET_TIME_DURATION_imp: New_time_nsec=(%ld), Current_time_nsec=(%ld) \n", wcd_new_system_time.tv_nsec, wcd_current_system_time.tv_nsec);    
    WCD_DEBUG_INFO_1("WCD_OSAL_GET_TIME_DURATION_imp: Time_duration = (%ld) \n", *time);        
#endif
  }
  */
}



