/*===========================================================================
                          CAD   W r a p p e r 
               f o r   QuRT   OS dependencies

  @file csd_os_dependencies.c
  This file contains OS dependencies for CAD. Each platform (OS) will 
  have its own copy of this file with their platform (OS) specific 
  dependencies/ code

Copyright (c) 2008 - 2012, 2016 QUALCOMM Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
========================================================================*/

/*===========================================================================
                             Edit History

$Header:

when       who         what, where, why
--------   ---         ------------------------------------------------------- 
03/13/17   vk          Fix QACT crash issue
11/15/16   rk          Abstracting QuRT OSAL layer 
10/21/13   vk          changes related to TLMM GPIO 
09/27/13   rp          Modified DAL memory allocation attributes
07/03/13   vk          Changes to remove compiler warnings 
06/04/13   unni        NV based CSD configuration
05/25/12   rp          Added  Mem region allocation support
03/02/09   asheeshk    Created file.

============================================================================*/

/*===========================================================================
 Include Files
============================================================================*/
#include "csd_status.h"
#include "mmdefs.h"
#include "customer.h"
#include "csd_os_dependencies.h"
#include "csd_debug_msg.h"    
#include "acdb.h"
#include "fs_lib.h"
#include "qurt_signal.h"
#include "timer.h"
#include "qurt_thread.h"
#include "qurt_mutex.h"
#include "qurt_memory.h"
#include "qurt_error.h"
#include "mmstd.h"

#define MDM_ACDB_FILENAME_MAX_LEN           128
#define ACDB_IMG_TAG                        0xacdbacdb
#define CSD_NV_CONFIG_FILE_NAME             "/nv/item_files/csd/csd.conf"
#define CSD_NV_CONFIG_SIZE                  64

#define PLATFORM_SUBTYPE_DEFAULT            0x00000000
#define PLATFORM_SUBTYPE_I2S                0x00000001
#define PLATFORM_SUBTYPE_SLIMBUS            0x00000002

#define BIT_SET(val, bit) (((val) & (1 << (bit))) ? 1 : 0)

#define EC_NZERO(func_call, cleanup)\
if(1){\
  int qurt_status = QURT_EOK;\
  if( 0 != (qurt_status = func_call)) {\
     CSD_MSG(CSD_PRIO_ERROR, "func_call failed, error code ", qurt_status);\
     goto cleanup;\
    }\
}

#define CSD_EVENT_SET_CLEARED_V ( 0 )
#define CSD_EVENT_SET_SIGNALLED_V ( 1 )
#define CSD_EVENT_ABORT_CLEARED_V ( 0 )
#define CSD_EVENT_ABORT_SIGNALLED_V ( 1 )
#define CSD_EVENT_ABORT_PROCESSED_V ( 2 )
#define CSD_DONE_SIG 0x00000001

#define CSD_TIMER_MASK_RESERVED ( 0x1 )
#define CSD_TIMER_MASK_EXPIRED  ( 0x2 )
#define CSD_TIMER_MASK_DESTROY  ( 0x4 )
#define CSD_TIMER_MASK_EXIT     ( 0x8 )
#define CSD_TIMER_THREAD_PRI_ORDER ( 50 )
#define CSD_TIMER_MASK_THREAD_WAIT (CSD_TIMER_MASK_EXPIRED|\
                                      CSD_TIMER_MASK_DESTROY)
#define CSD_TIMER_THREAD_STACK_SIZE ( 16384 )
#define CSD_TIMER_THREAD_NAME_SIZE ( 15 + 1 ) /* Last byte reserved for NULL. */
#define CSD_THREAD_STACK_SIZE ( 8192 )
#define CSD_THREAD_NAME_SIZE ( 12 + 1 ) /* Last byte reserved for NULL. */

#define SYSTEM_TICK_COUNT           19200

extern qurt_mem_region_attr_t  acdb_mem_region_attr;

struct file_entry {
  uint32_t filenamelen;
  char filename[MDM_ACDB_FILENAME_MAX_LEN];
  uint32_t offset;
  uint32_t size;
  uint32_t group_id;
  uint32_t flavor_id;
};

struct img_file_format {
  uint32_t acdb_tag;
  uint16_t major_version;
  uint16_t minor_version;
  uint32_t num_files;
};

typedef struct {
  bool_t disable_wcd_codec;
  bool_t use_barebone_acdb;
}nv_based_config_t ;

static uint32_t acdb_flavor_config = ACDB_FLAVOR_I2S;

typedef struct csd_mem_handle_internal_t
{
  qurt_mem_region_t handle;
}csd_mem_handle_internal_t;

typedef struct csd_lock_internal_t
{
  qurt_mutex_t lock;
}csd_lock_internal_t;

typedef struct csd_event_internal_t
{
  qurt_signal_t signal;
} csd_event_internal_t;

typedef struct csd_thread_internal_t
{
  char_t name[ CSD_THREAD_NAME_SIZE ];
  uint8_t * stack;
  void* param;
  csd_thread_fn_t fn;
  unsigned int handle;
  int32_t result;
  bool_t user_provided_stack;
} csd_thread_internal_t;

typedef struct csd_timer_internal_t
{
  timer_type handle;
  timer_group_type csd_timer_group;
  qurt_signal_t  signal;
  unsigned int      mask;
  qurt_mutex_t      csd_timer_lock;
  csd_timer_fn_t timer_cb;
  void* client_token;
  bool_t            is_thread_active;
  struct
  {
    unsigned int handle;
    char_t name[ CSD_TIMER_THREAD_NAME_SIZE ];
    uint8_t stack[ CSD_TIMER_THREAD_STACK_SIZE ];
    qurt_thread_attr_t attr;
    qurt_thread_t tid;
    void* param;
    int32_t result;
  }timer_thread;
}csd_timer_internal_t;

csd_lock_t csd_critical_section_mutex = NULL;

CSD_EXTERNAL uint32_t csd_mem_alloc( uint32_t size, csd_mem_handle_t *handle, uint8_t** vir_addr )
{
  int16_t qurt_mem_rc;
  qurt_mem_region_attr_t mem_table_region_attr;
  csd_mem_handle_internal_t *mem_handle;

  if ( handle == NULL )
  {
    return CSD_OSAL_EBADPARAM;
  }
  
  csd_malloc( sizeof( csd_mem_handle_internal_t ), (void**)&mem_handle );
  if ( mem_handle == NULL )
  {
    return CSD_OSAL_ENORESOURCE;
  }

  qurt_mem_region_attr_init( &mem_table_region_attr );
  mem_table_region_attr.mapping_type = QURT_MEM_MAPPING_PHYS_CONTIGUOUS;
  mem_table_region_attr.cache_attribs = QURT_MEM_CACHE_NONE_SHARED;

  qurt_mem_rc = qurt_mem_region_create( &mem_handle->handle, size,
                                        qurt_mem_default_pool, &mem_table_region_attr );

  if( qurt_mem_rc == QURT_EOK )
  {
    if( qurt_mem_region_attr_get( mem_handle->handle, &mem_table_region_attr ) == QURT_EOK )
    {
      *vir_addr = (uint8_t *)mem_table_region_attr.virtaddr;
	  *handle = mem_handle;
	  return CSD_OSAL_EOK;
    }
    else
    {
      qurt_mem_region_delete( mem_handle->handle );
      qurt_mem_rc = CSD_OSAL_EFAILED;
	  goto error;
    }
  }
 error:
  csd_free( mem_handle );	
  return qurt_mem_rc;
}

CSD_EXTERNAL uint32_t csd_mem_free( csd_mem_handle_t handle )
{
  csd_mem_handle_internal_t *mem_handle = handle;

  if( mem_handle == NULL )
  {
	return CSD_OSAL_EBADPARAM;
  }

  if( qurt_mem_region_delete( mem_handle->handle ) != QURT_EOK )
  {
    return CSD_OSAL_EFAILED;
  }
  csd_free( mem_handle );
  return CSD_OSAL_EOK;
}

CSD_EXTERNAL uint32_t csd_memscpy(void *dst, uint32_t dst_size,
                    const void *src, uint32_t src_size)
{
  if(dst == NULL || src == NULL)
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_memscpy: Null Param : dst[0x%x], src[0x%x]", dst, src);
    return CSD_OSAL_EBADPARAM;
  }
  else
  {
    memscpy(dst, dst_size, src, src_size);
  }
  return CSD_OSAL_EOK;
}

CSD_EXTERNAL void csd_malloc( uint64_t size, void **ptr )
{
	*ptr = malloc(size);
}

CSD_EXTERNAL void csd_memset( void* dst, int32_t c, uint32_t size)
{
	memset( dst, c, size );
}

CSD_EXTERNAL void csd_free(void *ptr)
{
	free(ptr);
	ptr=NULL;
}

CSD_EXTERNAL uint32_t csd_get_mem_info( csd_mem_handle_t handle, uint64_t* phy_addr, uint32_t* cur_size )
{
  qurt_mem_region_attr_t mem_table_region_attr;
  csd_mem_handle_internal_t *mem_handle = handle;

  if( mem_handle == NULL )
  {
	return CSD_OSAL_EBADPARAM;
  }
  if( qurt_mem_region_attr_get( mem_handle->handle, &mem_table_region_attr ) == QURT_EOK )
  {
    *phy_addr = (uint64_t)mem_table_region_attr.physaddr;
    *cur_size = (uint32_t)mem_table_region_attr.size;
  }
  else
  {
    return CSD_OSAL_EFAILED;
  }
  return CSD_OSAL_EOK;
}

CSD_EXTERNAL uint32_t csd_lock_create ( csd_lock_t* lock )
{
  csd_lock_internal_t* mutex_lock;

  if ( lock == NULL )
  {
    return CSD_OSAL_EBADPARAM;
  }
 
  csd_malloc( sizeof( csd_lock_internal_t ), (void**)&mutex_lock );
  if ( mutex_lock == NULL )
  {
    return CSD_OSAL_ENORESOURCE;
  }

  qurt_mutex_init( &mutex_lock->lock );

  *lock = mutex_lock;

  return CSD_OSAL_EOK;
}

CSD_EXTERNAL uint32_t csd_lock_destroy ( csd_lock_t lock )
{
  csd_lock_internal_t* the_lock = lock;

  if ( the_lock == NULL )
  {
    return CSD_OSAL_EBADPARAM;
  }

  qurt_mutex_destroy( &the_lock->lock );
  csd_free( the_lock );

  return CSD_OSAL_EOK;
}

CSD_EXTERNAL uint32_t csd_lock_enter ( csd_lock_t lock )
{
  csd_lock_internal_t* the_lock = lock;

  if ( the_lock == NULL )
  {
    return CSD_OSAL_EBADPARAM;
  }

  qurt_mutex_lock( &the_lock->lock );

  return CSD_OSAL_EOK;
}

CSD_EXTERNAL uint32_t csd_lock_leave ( csd_lock_t lock )
{
   csd_lock_internal_t* the_lock = lock;
 
   if ( the_lock == NULL )
   {
     return CSD_OSAL_EBADPARAM;
   }
 
   qurt_mutex_unlock( &the_lock->lock );
 
   return CSD_OSAL_EOK;
}

CSD_EXTERNAL int csd_criticalsection_create()
{
  if( !csd_critical_section_mutex )
  {
	return csd_lock_create(&csd_critical_section_mutex);
  }
  return CSD_OSAL_EOK;
}

CSD_EXTERNAL int csd_criticalsection_enter()
{
  if(csd_critical_section_mutex)
  {
    return csd_lock_enter( csd_critical_section_mutex );
  }
  return CSD_OSAL_EOK;
}

CSD_EXTERNAL int csd_criticalsection_leave()
{
  if(csd_critical_section_mutex)
  {
    return csd_lock_leave( csd_critical_section_mutex );
  }
  return CSD_OSAL_EOK;
}

CSD_EXTERNAL int csd_criticalsection_release()
{
  if( csd_critical_section_mutex )
  {
    return csd_lock_destroy( csd_critical_section_mutex );
  }
  return CSD_OSAL_EOK;
}

CSD_EXTERNAL uint32_t csd_event_create ( csd_event_t* event )
{
  csd_event_internal_t* the_event;

  if ( event == NULL )
  {
    return CSD_OSAL_EBADPARAM;
  }

  csd_malloc( sizeof( csd_event_internal_t ), (void **) &the_event );
  if ( the_event == NULL ) 
  {
    return CSD_OSAL_ENORESOURCE;
  }

  qurt_signal_init( (qurt_signal_t *)the_event );

  *event = ( ( csd_event_t* ) the_event );

  return CSD_OSAL_EOK;
}

CSD_EXTERNAL uint32_t csd_event_destroy ( csd_event_t event )
{
  csd_event_internal_t* the_event = ( ( csd_event_internal_t* ) event );

  if ( the_event == NULL )
  {
    return CSD_OSAL_EBADPARAM;
  }

  ( void ) qurt_signal_destroy((qurt_signal_t *) the_event );
  ( void ) free( the_event );

  return CSD_OSAL_EOK;
}

CSD_EXTERNAL uint32_t csd_event_wait ( csd_event_t event )
{
  csd_event_internal_t* the_event = ( ( csd_event_internal_t* ) event );

  if ( the_event == NULL )
  {
    return CSD_OSAL_EBADPARAM;
  }
  qurt_signal_wait((qurt_signal_t *)the_event, CSD_DONE_SIG, QURT_SIGNAL_ATTR_WAIT_ANY);
  qurt_signal_clear((qurt_signal_t *)the_event, CSD_DONE_SIG);
  return CSD_OSAL_EOK;
}

CSD_EXTERNAL uint32_t csd_event_wait_timed ( csd_event_t event, uint32_t time_ms )
{
  uint32 timer_event = 0;
  int result;
  csd_event_internal_t* the_event = ( ( csd_event_internal_t* ) event );
 
  if ( the_event == NULL )
  {
    return CSD_OSAL_EBADPARAM;
  }
  result= qurt_signal_wait_timed_ext((qurt_signal_t *) the_event, CSD_DONE_SIG, QURT_SIGNAL_ATTR_WAIT_ANY,
						         &timer_event, time_ms*SYSTEM_TICK_COUNT);							
 
  qurt_signal_clear((qurt_signal_t *)the_event, CSD_DONE_SIG);

  return (result == QURT_EOK) ? (CSD_OSAL_EOK) :((result == QURT_EFAILED_TIMEOUT) ? (CSD_ETIMEOUT) : (CSD_OSAL_EUNEXPECTED));
}

CSD_EXTERNAL uint32_t csd_event_signal ( csd_event_t event )
{
  csd_event_internal_t* the_event = ( ( csd_event_internal_t* ) event );

  if ( the_event == NULL )
  {
    return CSD_OSAL_EBADPARAM;
  }
  qurt_signal_set((qurt_signal_t *)the_event, CSD_DONE_SIG);
  return CSD_OSAL_EOK;
}

CSD_EXTERNAL uint32_t csd_event_signal_abortall ( csd_event_t event )
{
  csd_event_internal_t* the_event = ( ( csd_event_internal_t* ) event );
 
  if ( the_event == NULL )
  {
    return CSD_OSAL_EBADPARAM;
  }
  qurt_signal_set((qurt_signal_t *)the_event, CSD_DONE_SIG);	
  return CSD_OSAL_EOK;
}

static void csd_thread_wrapper ( void* param )
{
  csd_thread_internal_t* the_thread = ( ( csd_thread_internal_t* ) param );

  if ( the_thread != NULL )
  {
    the_thread->result = the_thread->fn( the_thread->param );
  }
  qurt_thread_stop();
}

CSD_EXTERNAL uint32_t csd_thread_create ( csd_thread_t* ret_thread, char_t* name, uint32_t priority,
										  uint8_t* stack_base, uint32_t stack_size, 
										  csd_thread_fn_t thread_fn, void* param )
{
  typedef void (*thread_handler_func_t) (void *);
  int cleanup_errors = 0;
  csd_thread_internal_t* the_thread;
  qurt_thread_attr_t attr;
  qurt_thread_t tid;
  int rc;

  if ( ret_thread == NULL )
  {
    return CSD_OSAL_EBADPARAM;
  }

  csd_malloc( sizeof( csd_thread_internal_t ), ( ( void** ) &the_thread ) );
  if ( !the_thread )
  {
    return CSD_OSAL_ENORESOURCE;
  }

  for ( ;; )
  {
    ( void ) mmstd_memset( the_thread, 0, sizeof( csd_thread_internal_t ) );

    if ( name )
    {
      ( void ) mmstd_strlcpy( the_thread->name, name, CSD_THREAD_NAME_SIZE );
    }
    else
    {
      ( void ) mmstd_strlcpy( the_thread->name, "AUDIO", CSD_THREAD_NAME_SIZE );
    }

    the_thread->fn = thread_fn;
    the_thread->param = param;
    the_thread->result = CSD_OSAL_EOK;

    qurt_thread_attr_init (&attr);
	qurt_thread_attr_set_name( &attr, the_thread->name );
	qurt_thread_attr_set_priority ( &attr, ( unsigned short ) priority );
    qurt_thread_attr_set_stack_size( &attr, stack_size );

    rc = qurt_thread_create(
            &tid,                                           /* thread ID */
            &attr,                                          /* thread attributes */
            ( thread_handler_func_t ) csd_thread_wrapper,   /* thread entry function */
            (void *) the_thread                             /* thread argument */
          );

    if ( rc == QURT_EFAILED )
    {
      break;
    }

    the_thread->handle = tid;

    *ret_thread = ( ( csd_thread_t* ) the_thread );

    return CSD_OSAL_EOK;
  }
  CSD_MSG(CSD_PRIO_HIGH, "csd_create_thread error");
  /* Perform error clean-up. */
  if ( the_thread->user_provided_stack == FALSE )
  {
    csd_free( the_thread->stack );
    if ( the_thread->stack ) cleanup_errors |= 2;

    the_thread->stack = NULL;
  }

  csd_free( the_thread );
  if ( the_thread ) cleanup_errors |= 1;

  return ( cleanup_errors ? CSD_OSAL_EPANIC : CSD_OSAL_EFAILED );
}

CSD_EXTERNAL uint32_t csd_thread_destroy ( csd_thread_t thread )
{
  int rc , thrd_join_status = 0 , cleanup_errors = 0;
  csd_thread_internal_t* the_thread = ( ( csd_thread_internal_t* ) thread );

  if ( the_thread == NULL )
  {
    return CSD_OSAL_EBADPARAM;
  }

  rc = qurt_thread_join( the_thread->handle, &thrd_join_status );

  if ( rc != QURT_ENOTHREAD && rc != QURT_EOK ) cleanup_errors |= 1;
  if ( the_thread->result ) cleanup_errors |= 2;

  if ( the_thread->user_provided_stack == FALSE )
  {
    csd_free( the_thread->stack );
    if ( the_thread->stack ) cleanup_errors |= 4;

    the_thread->stack = NULL;
  }

  csd_free( the_thread );
  if ( the_thread ) cleanup_errors |= 8;

  return ( cleanup_errors ? CSD_OSAL_EPANIC : rc );
}

static void csd_timer_wrapper ( void* param )
{
  csd_timer_internal_t* the_timer = ( ( csd_timer_internal_t* ) param );
  unsigned int ret_mask;

  if ( the_timer != NULL )
  {
      for(;;)
      {
        ret_mask = qurt_signal_wait( &the_timer->signal, CSD_TIMER_MASK_THREAD_WAIT,
									QURT_SIGNAL_ATTR_WAIT_ANY );
        if( ret_mask&(CSD_TIMER_MASK_DESTROY) )
        {
          the_timer->is_thread_active=FALSE;
          qurt_signal_clear(&the_timer->signal, CSD_TIMER_MASK_THREAD_WAIT);
          qurt_thread_stop();
        }
        qurt_signal_clear(&the_timer->signal, CSD_TIMER_MASK_EXPIRED); /* be careful not to clear destroy flag*/
        the_timer->timer_cb( the_timer->client_token );
      }
  }
  qurt_thread_stop();
}

static void csd_timer_interrupt_handler ( timer_cb_data_type data )
{
  csd_timer_internal_t* the_timer = ( ( csd_timer_internal_t* ) data );
  if ( the_timer != NULL )
  {
	 qurt_signal_set(&the_timer->signal, CSD_TIMER_MASK_EXPIRED);
  }
  return ;
}

CSD_EXTERNAL uint32_t csd_timer_create ( csd_timer_t* ret_timer, csd_timer_fn_t timer_cb,
										 void* client_token )
{
  csd_timer_internal_t* the_timer = NULL;

  if ( ret_timer == NULL )
  {
    return CSD_OSAL_EBADPARAM;
  }

  csd_malloc( sizeof( csd_timer_internal_t ), ( ( void** ) &the_timer ) );
  if ( the_timer == NULL )
  {
    return CSD_OSAL_ENORESOURCE;
  }
  mmstd_memset( the_timer, 0, sizeof( csd_timer_internal_t ) );
  qurt_signal_init(&the_timer->signal);
  EC_NZERO(timer_def_osal(&the_timer->handle,
					&the_timer->csd_timer_group,
					TIMER_FUNC1_CB_TYPE,
					&csd_timer_interrupt_handler,
					0), cleanup);
  
  the_timer->mask= CSD_TIMER_MASK_EXIT;
  qurt_mutex_init(&the_timer->csd_timer_lock);

  the_timer->timer_cb = timer_cb;
  the_timer->client_token = client_token;

  *ret_timer = ( ( csd_timer_t* ) the_timer );

  return CSD_OSAL_EOK;
cleanup :
   csd_free( the_timer );
   return CSD_OSAL_EFAILED;
}

CSD_EXTERNAL uint32_t csd_timer_destroy ( csd_timer_t timer )
{
  int thrd_join_status = 0;
  csd_timer_internal_t* the_timer = ( ( csd_timer_internal_t* ) timer );
  int qurt_rc = QURT_EOK;

  if ( the_timer == NULL )
  {
    return CSD_OSAL_EBADPARAM;
  }

  qurt_mutex_lock (&the_timer->csd_timer_lock);
  timer_clr_64(&the_timer->handle, T_NONE);

  if(the_timer->is_thread_active)
  {
    the_timer->mask = CSD_TIMER_MASK_DESTROY;
    qurt_signal_set(&the_timer->signal, CSD_TIMER_MASK_DESTROY);
  }
  qurt_mutex_unlock (&the_timer->csd_timer_lock);

  /* wait for dispatcher thread to exit */
  qurt_rc = qurt_thread_join(the_timer->timer_thread.tid, &thrd_join_status);
  if( qurt_rc != QURT_ENOTHREAD && qurt_rc != QURT_EOK )
  {
	CSD_MSG(CSD_PRIO_ERROR, "qurt_thread_join failed, error code ", qurt_rc);
    goto cleanup2;
  }

  /* no more threads using the_timer, safe to destroy now*/
  timer_undef(&the_timer->handle);
  qurt_signal_destroy(&the_timer->signal);
  qurt_mutex_destroy(&the_timer->csd_timer_lock);
  csd_free( the_timer );

  return CSD_OSAL_EOK;

 cleanup2:
  return CSD_OSAL_EFAILED;
}

/*this function generates 9999 different thread names
 */
char* csd_timer_get_thread_name(uint32 timer_count)
{
  static char name[]=
               {'C','S','D','_','T','I','M','E','R','_','0','0','0','0','\0'};
  unsigned long temp;

  temp = 48+(timer_count%10);
  mmstd_memcpy(name+13, sizeof(name)-13,(char*)&temp, sizeof(char));

  temp = 48+((timer_count%100)/10);
  mmstd_memcpy(name+12, sizeof(name)-12, (char*)&temp, sizeof(char));

  temp = 48+((timer_count%1000)/100);
  mmstd_memcpy(name+11, sizeof(name)-11,(char*)&temp, sizeof(char));

  temp = 48+(timer_count/1000);
  mmstd_memcpy(name+10, sizeof(name)-10, (char*)&temp, sizeof(char));

  return name;
}

CSD_EXTERNAL uint32_t csd_timer_start ( csd_timer_t timer, uint64_t time_ns )
{
  static uint32 timer_count=0;
  csd_timer_internal_t* the_timer = ( ( csd_timer_internal_t* ) timer );
  typedef void (*timer_handler_func_t) (void *);
  void * pStack;
  void * pStackAlligned;
  unsigned int alligned_stack_size;
  uint64_t time_ms;
  time_ms = time_ns/1000000;

  if ( the_timer == NULL )
  {
    return CSD_OSAL_EBADPARAM;
  }

  qurt_mutex_lock (&the_timer->csd_timer_lock);
  if(!the_timer->is_thread_active)
  {
    ( void ) mmstd_memset(
       &the_timer->timer_thread, 0, sizeof( the_timer->timer_thread ) );

    ( void ) mmstd_strlcpy(
       the_timer->timer_thread.name,
       csd_timer_get_thread_name(timer_count++),
       CSD_TIMER_THREAD_NAME_SIZE-1 );

    if(timer_count>=10000)
      timer_count=0;

    the_timer->timer_thread.param = the_timer;

    /* Align the stack pointer. */
    pStack = ( void* )( &the_timer->timer_thread.stack );
    pStackAlligned = ( void* )( (uint32_t)((uint32_t)pStack + 127) & (~127) );

    /* Calculate alligned stack size. */
    alligned_stack_size =
      (unsigned int)(CSD_TIMER_THREAD_STACK_SIZE-((uint32_t)pStackAlligned - (uint32_t)pStack));
    alligned_stack_size -= alligned_stack_size % 128;

    qurt_thread_attr_init (&the_timer->timer_thread.attr);
    qurt_thread_attr_set_stack_size(
      &the_timer->timer_thread.attr, alligned_stack_size );
/* ToDO */
#if 0
   qurt_thread_attr_set_stack_addr(
      &the_timer->timer_thread.attr, pStackAlligned );
#endif
    qurt_thread_attr_set_priority (
      &the_timer->timer_thread.attr, CSD_TIMER_THREAD_PRI_ORDER );
    qurt_thread_attr_set_name(
      &the_timer->timer_thread.attr, the_timer->timer_thread.name );

    EC_NZERO( qurt_thread_create(
          &the_timer->timer_thread.tid,                 /* thread ID */
          &the_timer->timer_thread.attr,                /* thread attributes */
          (timer_handler_func_t)csd_timer_wrapper,/* thread entry function */
          (void *) the_timer                            /* thread argument */
       ), cleanup);

    the_timer->is_thread_active=TRUE;
  }
			 
  timer_set(&the_timer->handle, time_ms, 0, T_MSEC );		 
  qurt_mutex_unlock (&the_timer->csd_timer_lock);
  return CSD_OSAL_EOK;

 cleanup:
  qurt_mutex_unlock (&the_timer->csd_timer_lock);
  return CSD_OSAL_EFAILED;
}

CSD_EXTERNAL uint32_t csd_timer_stop (
  csd_timer_t timer
)
{
  csd_timer_internal_t* the_timer = ( ( csd_timer_internal_t* ) timer );

  if ( the_timer == NULL )
  {
    return CSD_OSAL_EBADPARAM;
  }
  timer_clr_64(&the_timer->handle, T_NONE);
  return CSD_OSAL_EOK;
}


/*=============================================================================

  FUNCTION      csd_get_cdt_platform_info

  DESCRIPTION   This function gets CDT platform information

  DEPENDENCIES  CDT

  PARAMETERS    @param PlatformInfo : CDT data structure to fill

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
int32_t csd_get_cdt_platform_info( DalPlatformInfoPlatformInfoType *PlatformInfo )
{
  DALResult eResult = DAL_SUCCESS;
  DalDeviceHandle *pPlatformInfoHandle = NULL;
  
  int32_t rc = CSD_EOK;
  
  if( PlatformInfo )
  {
    if( !pPlatformInfoHandle )
    {
      eResult = DAL_DeviceAttach( DALDEVICEID_PLATFORMINFO,
                                  &pPlatformInfoHandle );
      if( eResult != DAL_SUCCESS )
      {
        rc = CSD_EFAILED;
        CSD_MSG( CSD_PRIO_ERROR, "csd_get_cdt_platform_info: "
                 "Unable to execute DAL_DeviceAttach for "
                 "DALDEVICEID_PLATFORMINFO, dal_rc[0x%x]", eResult );
      }
    }
    
    if( eResult == DAL_SUCCESS )
    {
      eResult = DalPlatformInfo_GetPlatformInfo( pPlatformInfoHandle,
                                                 PlatformInfo );
      if( eResult == DAL_SUCCESS )
      {
        CSD_MSG( CSD_PRIO_ERROR, "sucess_csd_get_cdt_platform_info: "
                 "PlatformInfo.subtype[%d], version[%d], "
                 "platform[%d]", PlatformInfo->subtype, PlatformInfo->version,
                 PlatformInfo->platform );
      }
      else
      {
        rc = CSD_EFAILED;
        CSD_MSG( CSD_PRIO_ERROR, "csd_get_cdt_platform_info: "
                 "Unable to get platform info, dal_rc[0x%x]", eResult );
      }
    }
  }
  else
  {
    rc = CSD_EBADPARAM;
    CSD_MSG( CSD_PRIO_ERROR, "csd_get_cdt_platform_info: "
             "Argument [*PlatformInfo] is NULL" );
  }
  
  if( pPlatformInfoHandle )
  {
    if( DAL_SUCCESS != DAL_DeviceDetach(pPlatformInfoHandle) )
    {
      CSD_MSG(CSD_PRIO_ERROR, "csd_get_cdt_platform_info: "
              "Cannot detach DAL driver for platform info");
    }
  }

  return rc;
}


/*==============================================================================

  FUNCTION      csd_get_nv_config

  DESCRIPTION   Fetch nv item from /nv/item_files/csd/csd.conf
                |Bit|             Feature                                      |
                |  0| Disable WCD Codec                                        |
                |  1| Use Barebone ACDB image                                  |
                | XX| <Description>                                            |
                | 63| <Last Bit>                                               |

  DEPENDENCIES  remote efs

  PARAMETERS

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

==============================================================================*/
static int32_t csd_get_nv_config(nv_based_config_t* config)
{
  int32_t rc = CSD_EOK;
  static bool_t initialized = FALSE;
  /* All Flags in cached_nv_config is initalized to 0 - FALSE */
  static nv_based_config_t cached_nv_config = {0};

  if(!initialized)
  {
    int32_t efs_rc = 0;
    DALResult dal_rc = DAL_SUCCESS;
    uint8_t* nv = NULL;

    /* Singleton procedure to grab CSD config from NV */
    csd_malloc(CSD_NV_CONFIG_SIZE, (void**)&nv);
    if(nv == NULL)
    {
      CSD_MSG(CSD_PRIO_ERROR, "csd_get_nv_config: "
              "Unable to allocate %d size, dal rc=0x%x",
              CSD_NV_CONFIG_SIZE, dal_rc);
      return CSD_ENORESOURCE;
    }

    if(nv)
    {
      efs_rc = efs_get(CSD_NV_CONFIG_FILE_NAME, nv, CSD_NV_CONFIG_SIZE);
      if(efs_rc != CSD_NV_CONFIG_SIZE)
      {
        CSD_MSG(CSD_PRIO_ERROR, "csd_get_nv_config: "
                "error reading bytes, efs_rc 0x%x", efs_rc);
        rc = CSD_EFAILED;
      }
    }

    if(CSD_EOK == rc && nv)
    {
      /* populate config structure based on nv */
      if(BIT_SET(nv[0],0))
      {
        cached_nv_config.disable_wcd_codec = TRUE;
      }
      if(BIT_SET(nv[0],1))
      {
        cached_nv_config.use_barebone_acdb = TRUE;
      }

      initialized = TRUE;
    }

    if(nv)
    {
      csd_free(nv);
    }
  }

  if( initialized )
  {
    csd_memscpy(config, sizeof(*config),
                &cached_nv_config, sizeof(cached_nv_config));
  }

  return rc;
}

/*=============================================================================

  FUNCTION      csd_query_acdb_files

  DESCRIPTION   This function get the appropriate ACDB files to load

  DEPENDENCIES  Based on cdt(1st) and nv(2nd) configuration determine which
                ACDB image to load

  PARAMETERS    @param pacdb_files : acdb data structure to fill

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
int32_t csd_query_acdb_files(struct csd_acdb_files *pacdb_files)
{
  int32_t rc = CSD_EOK;
  struct img_file_format *pheader = NULL;
  int16_t i = 0;
  struct file_entry *pfile_list = NULL;
  uint16_t num_of_files_loaded = 0;
  DalPlatformInfoPlatformInfoType PlatformInfo;  
  
  acdb_mem_region_attr.virtaddr = acdb_mem_region_attr.virtaddr + 40; // added offset to align to acdb tag

  /* Read image header */
  pheader = (struct img_file_format *)acdb_mem_region_attr.virtaddr;
  
  if( ( NULL == pacdb_files ) || ( NULL == pheader ) )
  {
    CSD_MSG( CSD_PRIO_ERROR, "csd_query_acdb_files: wrong parameter, pacdb_file[0x%x], pheader[0x%x]", pacdb_files, pheader );
    return CSD_EBADPARAM;
  }
  
  /* Verify the image loaded is valid */
  if( ACDB_IMG_TAG != pheader->acdb_tag )
  {
    CSD_MSG( CSD_PRIO_ERROR, "csd_query_acdb_files: wrong ACDB header[0x%x]", pheader->acdb_tag );
    return CSD_EFAILED;  
  }
  
  // Query CDT to decide which flavor of ACDB to use
  //   1. Default is I2S
  //   2. If CDT query fails, use I2S
  //   3. If platform is MTP and subtype indicates SLIMBUS, use SLIMBUS
  //   4. I2S is used for all other scenarios. 
  rc = csd_get_cdt_platform_info( &PlatformInfo );
  if( rc == CSD_EOK )
  {
    if( PlatformInfo.platform == DALPLATFORMINFO_TYPE_MTP )
    {
      if( PlatformInfo.subtype == PLATFORM_SUBTYPE_SLIMBUS )
        acdb_flavor_config = ACDB_FLAVOR_SLIMBUS;
    }
  }  
    
  CSD_MSG( CSD_PRIO_LOW, "csd_query_acdb_files: acdb_flavor_config[%d]", acdb_flavor_config );
    
  pfile_list = (struct file_entry*)( acdb_mem_region_attr.virtaddr + sizeof(struct img_file_format) );
  pacdb_files->num_files = pheader->num_files;

  for( i = 0; i < pheader->num_files; i++ )
  {
    if( pfile_list[i].flavor_id == ACDB_FLAVOR_UNKNOWN ||
        pfile_list[i].flavor_id == acdb_flavor_config )
    {
      pacdb_files->file_list[num_of_files_loaded].filenamelen = pfile_list[i].filenamelen;
      csd_memscpy(pacdb_files->file_list[num_of_files_loaded].filename,
                  sizeof(pacdb_files->file_list[num_of_files_loaded].filename),
                  pfile_list[i].filename, pfile_list[i].filenamelen);
      num_of_files_loaded++;
    }
  }
  
  pacdb_files->num_files = num_of_files_loaded;
  CSD_MSG( CSD_PRIO_LOW, "csd_query_acdb_files: [%d] ACDB files to load", num_of_files_loaded );
  
  if( num_of_files_loaded > 0 )
  {
    rc = CSD_EOK;
  }
  else
  {
    rc = CSD_EFAILED;
  }
  
  return rc;
}

/*=============================================================================

  FUNCTION      acdb_get_platform_config

  DESCRIPTION   This function allows ACDB init utility to get the ACDB flavor

  DEPENDENCIES  mpme

  PARAMETERS    @param acdb_cfg : ACDB flavor id to fill

  RETURN VALUE  acdb return status @see acdb.h

  SIDE EFFECTS  none

=============================================================================*/
int32_t acdb_get_platform_config( uint32_t *acdb_cfg )
{
  if( acdb_flavor_config == ACDB_FLAVOR_UNKNOWN )
  {
    return ACDB_DATA_NOT_FOUND;
  }
  else
  {
    *acdb_cfg = acdb_flavor_config;
  }
  return ACDB_SUCCESS;
}

/*==============================================================================

  FUNCTION      csd_dev_is_wcd_codec_present

  DESCRIPTION   Based on cdt(1st) and nv(2nd) configuration determine whether
                to use codec or not

  DEPENDENCIES  remote efs and cdt

  PARAMETERS

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

==============================================================================*/
int32_t csd_is_wcd_codec_present(bool_t* flag)
{
  static bool_t init = FALSE;           /* One time initialization */
  static bool_t enable_codec = TRUE;    /* cached flag initialized to
                                           default setting */
  int32_t rc = CSD_EOK;

  if(NULL == flag)
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_is_wcd_codec_present: "
            "Bad param flag[0x%x]", flag);
    return CSD_EBADPARAM;
  }

  // Query CDT and NV to decide if codec needs to be enabled
  //   1. Default is CODEC_ENABLE  
  //   2. If CDT indicates Fusion device, use CODEC_DISABLE
  //   3. Query NV if it's not a Fusion device and/or CDT query fails
  //   4. If NV query fails, use CODEC_ENABLE
  //   5. If NV codec flag is false, use CODEC_DISABLE
  if(!init)
  {
    DalPlatformInfoPlatformInfoType cdt_info;

    rc = csd_get_cdt_platform_info(&cdt_info);
    if(CSD_EOK == rc && cdt_info.fusion)
    {      
      enable_codec = FALSE;      
      init = TRUE;
    }    
  }

  if(!init)
  {
    nv_based_config_t nv_info;

    rc = csd_get_nv_config(&nv_info);
    if(CSD_EOK == rc)
    {
      /* NV config based decision */
      if(nv_info.disable_wcd_codec)
      {
        enable_codec = FALSE;
      }
    }
    else
    {
      CSD_MSG(CSD_PRIO_HIGH,"csd_is_wcd_codec_present: "
              "Unable to read NV item rc=0x%x", rc);
      /* NV is not compulsory */
	  enable_codec=TRUE;
      rc = CSD_EOK;

      /* We are not confident to disable the codec hence defaulting to codec
         present */
    }

    /* Init completed */
    init = TRUE;
  }

  *flag = enable_codec;

  return rc;
}

/*=============================================================================

  FUNCTION      csd_get_cdt_chip_family

  DESCRIPTION   This function gets CDT chip family info

  DEPENDENCIES  CDT

  PARAMETERS    @param chip_family : CDT chip family info to fill

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/

int32_t csd_get_cdt_chip_family( DalChipInfoFamilyType *chip_family )
{
  DALResult eResult = DAL_SUCCESS;
  DalDeviceHandle *chip_info = NULL;
  
  int32_t rc = CSD_EOK;

  if( chip_family )
  {
    eResult = DAL_DeviceAttach(DALDEVICEID_CHIPINFO, &chip_info);
    if(( DAL_SUCCESS != eResult ) || ( NULL == chip_info ))
    {
      rc = CSD_EFAILED;
      CSD_MSG( CSD_PRIO_ERROR, "csd_get_cdt_chip_family: "
               "Unable to execute DAL_DeviceAttach for "
                 "DALDEVICEID_CHIPINFO, dal_rc[0x%x]", eResult );
    }
		 
    if( CSD_EOK == rc )
    {
      eResult = DalDevice_Open(chip_info, DAL_OPEN_SHARED);
      if( DAL_SUCCESS != eResult )
      {
        rc = CSD_EFAILED;
        CSD_MSG(CSD_PRIO_ERROR, "csd_get_cdt_chip_family: "
                "Unable to open dal device for chip info, "
                    "dal_rc[0x%x]", eResult);
      }
    }

    if( CSD_EOK == rc )
    {
      eResult = DalChipInfo_GetChipFamily(chip_info,chip_family);
      if( eResult == DAL_SUCCESS )
      {
        CSD_MSG( CSD_PRIO_MEDIUM, "csd_get_cdt_chip_family: "
                 "Chip_family[%d]", *chip_family );
      }
      else
      {
        rc = CSD_EFAILED;
        CSD_MSG( CSD_PRIO_ERROR, "csd_get_cdt_chip_family: "
                 "Unable to get chip family, dal_rc[0x%x]", eResult );
      }
    }
  }
  else
  {
    rc = CSD_EBADPARAM;
    CSD_MSG( CSD_PRIO_ERROR, "csd_get_cdt_chip_family: "
              "Argument [*chip_family] is NULL" );
  }

  if( chip_info )
  {
    /* closing dal tlmm chip info handle */
    if( DAL_SUCCESS != DalDevice_Close(chip_info) )
    {
      CSD_MSG(CSD_PRIO_ERROR, "csd_get_cdt_chip_family: "
              "Cannot close tlmm dal device for chip info");
    }
    /* detaching dal tlmm chip info handle */
    if( DAL_SUCCESS != DAL_DeviceDetach(chip_info) )
    {
      CSD_MSG(CSD_PRIO_ERROR, "csd_get_cdt_chip_family: "
              "Cannot detach DAL driver for chip info");
    }
  }

  return rc;
}
