#ifndef _CSD_OS_DEPENDENCIES_H_
#define _CSD_OS_DEPENDENCIES_H_
/*===========================================================================
                          CSD   W r a p p e r 
               f o r   W32/ Simulator OS dependencies

  @file CadOsDependencies.h
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
11/15/16   rameshkr    Abstracting OSAL layer
10/23/12   jk          Added support for dynamic module enablement  
05/25/12   rp          Added  Mem region allocation support 
03/02/09   asheeshk    Created file.

============================================================================*/
#include "csd_os_dependencies_defs.h"


#define CSD_ACDB_FILENAME_MAX_LEN         256
#define CSD_ACDB_MAX_FILES                 20

#define CSD_OSAL_EOK          ( 0x00000000 ) /**< Success; completed; no errors. */
#define CSD_OSAL_EFAILED      ( 0x00000001 ) /**< General failure. */
#define CSD_OSAL_EBADPARAM    ( 0x00000002 ) /**< Bad operation parameter. */
#define CSD_OSAL_EUNSUPPORTED ( 0x00000003 ) /**< Unsupported routine/operation. */
#define CSD_OSAL_EVERSION     ( 0x00000004 ) /**< Unsupported version. */
#define CSD_OSAL_EUNEXPECTED  ( 0x00000005 ) /**< Unexpected problem encountered. */
#define CSD_OSAL_EPANIC       ( 0x00000006 ) /**< Unhandled problem occurred. */

#define CSD_OSAL_ENORESOURCE  ( 0x00000007 ) /**< Unable to allocate resources. */
#define CSD_OSAL_EHANDLE      ( 0x00000008 ) /**< Invalid handle. */
#define CSD_OSAL_EALREADY     ( 0x00000009 ) /**< Operation is already processed. */
#define CSD_OSAL_ENOTREADY    ( 0x0000000A ) /**< Operation is not ready to be
                                             processed. */

#define CSD_OSAL_EPENDING     ( 0x0000000B ) /**< Operation is pending completion. */
#define CSD_OSAL_EBUSY        ( 0x0000000C ) /**< Operation could not be accepted or
                                             processed. */

#define CSD_OSAL_EABORTED     ( 0x0000000D ) /**< Operation aborted due to an error. */
#define CSD_OSAL_EPREEMPTED   ( 0x0000000E ) /**< Operation is preempted by a higher
                                             priority. */
#define CSD_OSAL_ECONTINUE    ( 0x0000000F ) /**< Operation requests intervention to
                                             complete. */
#define CSD_OSAL_EIMMEDIATE   ( 0x00000010 ) /**< Operation requests immediate
                                             intervention to complete. */
#define CSD_OSAL_ENOTIMPL     ( 0x00000011 ) /**< Operation is not implemented. */
#define CSD_OSAL_ENEEDMORE    ( 0x00000012 ) /**< Operation needs more data or
                                             resources. */
#define CSD_OSAL_ELPC         ( 0x00000013 ) /**< Operation is a local procedure
                                             call. */

#define CSD_OSAL_ENOMEMORY    ( 0x00000014 ) /**< Unable to allocate sufficient
                                             memory. */
#define CSD_OSAL_ENOTEXIST    ( 0x00000015 ) /**< Item does not exist. */
#define CSD_OSAL_ENOSERVICE   ( 0x00000016 ) /**< Operation sent to an unregistered
                                             service. */
#define CSD_OSAL_EDENIED      ( 0x00000017 ) /**< Access is denied. */

struct csd_acdb_file_entry {
   uint32_t filenamelen;
   char filename[CSD_ACDB_FILENAME_MAX_LEN];
};

struct csd_acdb_files {
   uint32_t num_files;
   struct csd_acdb_file_entry file_list[CSD_ACDB_MAX_FILES];
};

#ifdef __cplusplus 
extern "C"
{
#endif

#ifdef __cplusplus
	#define CSD_EXTERNAL extern "C"
    /**< Export macro that indicates an external function, which is intended
         for public use. */
#else
    #define CSD_EXTERNAL extern
    /**< Export macro that indicates an external function, which is intended
         for public use. */
#endif /* __cplusplus */

typedef void* csd_mem_handle_t;
typedef void* csd_lock_t;
typedef void* csd_event_t;
typedef void* csd_thread_t;
typedef void* csd_timer_t;

typedef int32_t ( *csd_thread_fn_t ) ( void* param );
typedef void ( *csd_timer_fn_t ) ( void* client_token );

/*
 * Allocates the Physical memory of given size
 *
 * @param[in ] size     - required memory size
 * @param[out] handle   - mem_region
 * @param[out] vir_addr - pointer to the allocated memory
 * @return CSD_EOK value on success
 *         CSD_EFAILED on failure */
CSD_EXTERNAL uint32_t csd_mem_alloc( uint32_t size, csd_mem_handle_t *handle, uint8_t** vir_addr );

/*
 * Free the Memory allocated through csd_mem_alloc
 *
 * @param[in] handle   - mem_region
 * @return CSD_EOK value on success
 *         CSD_EFAILED value on failure
 */
CSD_EXTERNAL uint32_t csd_mem_free( csd_mem_handle_t handle );

/*
 * Memory copy
 *
 * @param[in] dst - destination memory pointer
 * @param[in] dst_size - destination memory size
 * @param[in] src - source memory pointer
 * @param[in] src_size - Memory handle
 * @return CSD_EOK value on success
 *         CSD_EBADPARAM value on bad input paramters
 */
CSD_EXTERNAL uint32_t csd_memscpy(void *dst, uint32_t dst_size,
                     const void *src, uint32_t src_size);

/*
 * Memory allocation from heap
 *
 * @param[in] size - size of the required memory
 * @param[out] ptr - pointer to the allocated memory
 */					 
CSD_EXTERNAL void csd_malloc( uint64_t size, void** ptr );

/*
 * Free the memory allocated through csd_malloc
 *
 * @param[in] ptr - pointer of the allocated memory
 */					 
CSD_EXTERNAL void csd_free( void *ptr );

CSD_EXTERNAL void csd_memset( void* dst, int32_t c, uint32_t size );

/*
 * Get Memory information
 * @param[in] handle - memory region 
 * @param[out] phy_addr - physical address
 * @param[out] cur_size - physical size
 * @return CSD_EOK value on success
 *             CSD_EBADPARAM value on bad input paramters
 */
CSD_EXTERNAL uint32_t csd_get_mem_info( csd_mem_handle_t handle, uint64_t* phy_addr, uint32_t* cur_size );

/*
 * Create mutex lock
 * @param[out] lock - pointer to mutex lock
 * @return CSD_EOK value on success
 *         CSD_EBADPARAM value on bad input paramters
 *	    CSD_ENORESOURCE value on memory allocation failure
 */
CSD_EXTERNAL uint32_t csd_lock_create ( csd_lock_t* lock );

/*
 * Destroy mutex lock
 * @param[in] lock - mutex lock pointer
 * @return CSD_EOK value on success
 *             CSD_EBADPARAM value on bad input paramters
 */
CSD_EXTERNAL uint32_t csd_lock_destroy ( csd_lock_t lock );

/*
 * Acquire mutex lock
 * @param[in] lock - mutex lock pointer
 * @return CSD_EOK value on success
 *             CSD_EBADPARAM value on bad input paramters
 */
CSD_EXTERNAL uint32_t csd_lock_enter ( csd_lock_t lock );

/*
 * Release mutex lock
 * @param[in] lock - mutex lock pointer
 * @return CSD_EOK value on success
 *             CSD_EBADPARAM value on bad input paramters
 */
CSD_EXTERNAL uint32_t csd_lock_leave ( csd_lock_t lock );

CSD_EXTERNAL int csd_criticalsection_create( void );

CSD_EXTERNAL int csd_criticalsection_enter( void );

CSD_EXTERNAL int csd_criticalsection_leave( void );

CSD_EXTERNAL int csd_criticalsection_create( void );

/*
 * Create event
 * @param[out] event - pointer to event object
 * @return CSD_EOK value on success
 *         CSD_EBADPARAM value on bad input paramters
 *	     CSD_ENORESOURCE value on memory allocation failure
 */
CSD_EXTERNAL uint32_t csd_event_create ( csd_event_t* event );

/*
 * Destroy event
 * @param[in] event - event object pointer
 * @return CSD_EOK value on success
 *             CSD_EBADPARAM value on bad input paramters
 */
CSD_EXTERNAL uint32_t csd_event_destroy ( csd_event_t event );

/*
 * Wait for events
 * @param[in] event - event object pointer
 * @return set signal on success
 *         CSD_EBADPARAM value on bad input paramters
 */
CSD_EXTERNAL uint32_t csd_event_wait ( csd_event_t event );

/*
 * Wait for events and return after time out.
 * @param[in] event - event object pointer
 * @param[in] time_ms - timeout value in ms
 * @return CSD_EOK on success
 *             CSD_ETIMEOUT when no signal set for event object before timeout
 *             CSD_EBADPARAM value on bad input paramters
 */
CSD_EXTERNAL uint32_t csd_event_wait_timed( csd_event_t event, uint32_t time_ms );

/*
 * set signal
 * @param[in] event - event object pointer
 * @return CSD_EOK on success
 *             CSD_EBADPARAM value on bad input paramters
 */
CSD_EXTERNAL uint32_t csd_event_signal ( csd_event_t event );

/*
 * set signal
 * @param[in] event - event object pointer
 * @return CSD_EOK on success
 *             CSD_EBADPARAM value on bad input paramters
 */
CSD_EXTERNAL uint32_t csd_event_signal_abortall ( csd_event_t event );

CSD_EXTERNAL uint32_t csd_thread_create ( csd_thread_t* ret_thread, char_t* name, uint32_t priority,
										  uint8_t* stack_base, uint32_t stack_size, csd_thread_fn_t thread_fn,
										  void* thread_param );
CSD_EXTERNAL uint32_t csd_thread_destroy ( csd_thread_t thread );

CSD_EXTERNAL uint32_t csd_timer_create ( csd_timer_t* ret_timer, csd_timer_fn_t timer_cb, void* client_token );

CSD_EXTERNAL uint32_t csd_timer_destroy ( csd_timer_t timer );

CSD_EXTERNAL uint32_t csd_timer_start ( csd_timer_t timer, uint64_t time_ns );

CSD_EXTERNAL uint32_t csd_timer_stop ( csd_timer_t timer );

int32_t csd_query_acdb_files(struct csd_acdb_files *pacdb_files);

/*
 * Queries the platform configuration for ACDB Initialization 
 *
 * @param[out] acdb_cfg - ACDB flavor configuration.
 *
 * @return ACDB_SUCCESS value on success else
 *         ACDB_DATA_NOT_FOUND value on bad input parameters
 *         CSD_EFAILED  value on failures
 */
int32_t acdb_get_platform_config( uint32_t *acdb_cfg );


#ifdef __cplusplus 
}
#endif

#endif /* _CSD_OS_DEPENDENCIES_H_ */
