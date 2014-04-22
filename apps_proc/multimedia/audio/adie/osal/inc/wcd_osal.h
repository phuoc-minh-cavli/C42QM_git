#ifndef __WCD_OS_ABSTRACTION__
#define __WCD_OS_ABSTRACTION__

/**
@file wcd_osal.h
@brief OS abstraction layer for WCD driver

*/
/*-----------------------------------------------------------------------------
    Copyright (c) 2011 Qualcomm Technologies Incorporated.
    All rights reserved.
    Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
Edit History
$Id: //components/rel/audio.tx/2.0/audio/adie/osal/inc/wcd_osal.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/15/13   OA      removed banned usage for memcpy function
03/07/13   SSP     Driver Unification Update
01/28/11   SK      Initial revision
=============================================================================*/

#include "time.h"
#include "wcd_result.h"
#include "wcd_types.h"

////////////////////////////////////////////////////////////////////////////////
//                                 BUS ACCESS                                 //
////////////////////////////////////////////////////////////////////////////////

/**
 * \brief Initializes the bus to be used. This function will only be called once (unless deinit is called).
 *
 * @return: E_WCD_SUCCESS or E_WCD_BUS_ERROR
 */
wcd_result wcd_osal_initialize_bus( wcd_bus_type bus_type );

/**
 * \brief Deinitializes the bus that was used. This function will only be called once (unless init is called).
 *
 * @return: E_WCD_SUCCESS or E_WCD_BUS_ERROR
 */
wcd_result wcd_osal_deinitialize_bus( void );

/**
 * \brief A bus write is performed. 
 * This will only be called after the bus is properly initialized and activated.
 *
 * @param reg : Register to write
 * @param value : Value to write to the register
 * @param bus_interface : The bus interface, either CODEC, INTERFACE or AHB
 * @param force_write : Indicates if the write can be buffered or if it needs to occur immediately
 *                      [FALSE] = Write can be buffered
 *                      [TRUE] = Write needs to occur immediately
 * @note : If force_write is set to true, it may flush out all previous buffered registers, which is acceptable
 *
 * @return: E_WCD_SUCCESS or E_WCD_BUS_ERROR
 */
wcd_result wcd_osal_bus_write( uint16 reg, uint8 value, wcd_ctrl_bus_interface bus_interface, wcd_bool force_write );

/**
 * \brief A bus read is performed. 
 * This will only be called after the bus is properly initialized and activated.
 *
 * @param reg : Register to read
 * @param value : Buffer to store the read value to
 * @param bus_interface : The bus interface, either CODEC, INTERFACE or AHB
 *
 * @note: All register reads need to occur immediately.
 *
 * @return: E_WCD_SUCCESS or E_WCD_BUS_ERROR
 */
wcd_result wcd_osal_bus_read( uint16 reg, uint8* value, wcd_ctrl_bus_interface bus_interface );


wcd_result wcd_osal_bus_write_repeat(uint16 reg, uint8* values, uint16 size, wcd_ctrl_bus_interface bus_interface, wcd_bool force_write);

/**
 * \brief Flushes all the buffered bus writes to the actual device.
 * This will only be called after the bus is properly initialized and activated.
 *
 * @param bus_interface : The bus interface to flush, either CODEC, INTERFACE or AHB
 *
 * @return: E_WCD_SUCCESS or E_WCD_BUS_ERROR
 */
wcd_result wcd_osal_bus_flush( wcd_ctrl_bus_interface bus_interface );

/**
 * \brief Sets the logical address of the codec (pgd) and slimbus interface.
 *
 * @return: E_WCD_SUCCESS or E_WCD_BUS_ERROR
 */
wcd_result wcd_osal_bus_get_logical_address( uint8* la_pgd, uint8* la_intf );

/**
 * \brief Activates the bus to perform register writes/reads.
 * This will only be called after the bus is properly initialized. 
 * This will only be called once until bus_deactivate is called.
 *
 * @return: E_WCD_SUCCESS or E_WCD_BUS_ERROR
 */
wcd_result wcd_osal_bus_activate(void);

/**
 * \brief Deactivates the bus to prevent register writes/reads (for power saving)
 * This will only be called once after the bus is activated. 
 *
 * @return: E_WCD_SUCCESS or E_WCD_BUS_ERROR
 */
wcd_result wcd_osal_bus_deactivate(void);

/**
 * \brief Allocates a resource group to be use for any shared channel processing
 * The group will be assigned in the parameter group_handle.
 *
 * @return: E_WCD_SUCCESS or E_WCD_BUS_ERROR
 */
wcd_result wcd_osal_bus_sc_alloc_group(void** group_handle);

/**
 * \brief This should allocate the shared data channel, add the channel to the group
 * allocated in wcd_osal_bus_sc_alloc_group, get the port handle and finally connect the
 * port to the channel. The shared data channel should also be returned in the 
 * parameter data_channel
 *
 * @param port: This will include the RX/TX offset.
 *
 * @return: E_WCD_SUCCESS or E_WCD_BUS_ERROR
 */
wcd_result wcd_osal_bus_sc_connect_port_and_channel(uint32 port, uint32 channel, void* group_handle, void** data_channel, wcd_slimbus_direction direction );

/**
 * \brief Enables the shared channel using the group allocated in wcd_osal_bus_sc_alloc_group
 * with the specified sample rate and bitwidth
 *
 * @return: E_WCD_SUCCESS or E_WCD_BUS_ERROR
 */
wcd_result wcd_osal_bus_sc_enable(uint32 sample_rate, uint32 bit_width, void* group_handle);

/**
 * \brief Disables the shared channel using the group allocated in wcd_osal_bus_sc_alloc_group
 * with the specified sample rate and bitwidth
 *
 * @return: E_WCD_SUCCESS or E_WCD_BUS_ERROR
 */
wcd_result wcd_osal_bus_sc_disable(void* group_handle);

/**
 * \brief Deallocates the shared data channel.
 *
 * @return: E_WCD_SUCCESS or E_WCD_BUS_ERROR
 */
wcd_result wcd_osal_bus_sc_dealloc_channel(void* channel_handle);

/**
 * \brief Deallocates the shared channel group
 *
 * @return: E_WCD_SUCCESS or E_WCD_BUS_ERROR
 */
wcd_result wcd_osal_bus_sc_dealloc_group(void* group_handle);

////////////////////////////////////////////////////////////////////////////////
//                            INTERNAL FUNCTIONS                              //
////////////////////////////////////////////////////////////////////////////////

void  WCD_OSAL_FREE_imp( void* ptr, uint8 tag ); 
void* WCD_OSAL_MALLOC_imp( size_t size, uint8 tag );
void  WCD_MEMSET_imp( void* ptr, uint8 pattern, size_t size );
void  WCD_MEMCOPY_imp( void* source, size_t source_size, void* destination, size_t dest_size );
void* WCD_OSAL_PMEM_MALLOC_imp( size_t size );
void  WCD_OSAL_PMEM_FREE_imp( void* ptr );


WCD_HANDLE WCD_OSAL_CREATE_THREAD_imp( wcd_osal_thread thread_function, void* thread_parameter, wcd_osal_priority_t priority);
int32   WCD_OSAL_JOIN_THREAD( WCD_HANDLE threadhandle, wcd_osal_kill_thread_fcn kill_function, void* param );

void* WCD_OSAL_CREATE_MUTEX_imp( void );
void  WCD_OSAL_DESTROY_MUTEX_imp( void* mutex );
int32 WCD_OSAL_WAIT_MUTEX_imp( void* mutex, int32 timeout_in_ms );
int32 WCD_OSAL_RELEASE_MUTEX_imp( void* mutex );


void* WCD_OSAL_CREATE_EVENT_imp( int32 manual_reset, int32 initial_state );
void  WCD_OSAL_DESTROY_EVENT_imp( void* event_handle );
int32 WCD_OSAL_WAIT_EVENT_imp( void* event_handle, int32 timeout_in_ms );
void  WCD_OSAL_SET_EVENT_imp( void* event_handle );
void  WCD_OSAL_RESET_EVENT_imp( void* event_handle );


void* WCD_CREATE_TIMER_imp(  wcd_osal_timer_callback_fcn callback, void* param, int32 isPeriodic );
void  WCD_ENABLE_TIMER_imp( void* handle, int32 milliseconds );
void  WCD_DISABLE_TIMER_imp( void* handle );
void  WCD_DESTROY_TIMER_imp( void* handle );

void  WCD_OSAL_SLEEP_imp(int32 timeout_milliseconds);

void  WCD_OSAL_GET_CURRENT_TIMESTAMP_imp(wcd_owner_module id);
void  WCD_OSAL_GET_NEW_TIMESTAMP_imp(wcd_owner_module id);
void  WCD_OSAL_GET_TIME_DURATION_imp(wcd_owner_module id, long *time);

#define WCD_MIN(x, y) (((x) < (y)) ? (x) : (y))

////////////////////////////////////////////////////////////////////////////////
//                              MEMORY MACROS                                 //
////////////////////////////////////////////////////////////////////////////////

#define WCD_MALLOC_START_TAG        91
#define WCD_MALLOC_END_TAG          109
#define MBHC_MALLOC_START_TAG       110
#define MBHC_MALLOC_END_TAG         126


/**
  @brief Allocation memory routine
  
  @return void on success pointer pointing to the allocated memory otherwise NULL
  
  @param size Amount of memory to be allocated
*/
#define WCD_MALLOC(size, tag) WCD_OSAL_MALLOC_imp(size, tag)

/**
  @brief Deallocation memory routine
  
  @return None
  
  @param ptr Pointer to the memory to be dealocated
*/
#define WCD_FREE(ptr, tag) WCD_OSAL_FREE_imp( (void*) ptr, tag )

/**
  @brief Fill a memory location with a byte.
  
  @return None
  
  @param ptr memory location to be filled  
  @param pattern byte to be used to fill the memory space
  @param size amount of data to be written to the pointed memory
*/
#define WCD_MEMSET( ptr, pattern, size) WCD_MEMSET_imp( ptr, pattern, size )

/**
  @brief Copy a memory location to a selected destination
  
  @return None
  
  @param source Pointer to the source memory location
  @param destination Pointer to the memory to be written
  @param size amount of memory to be copied
*/
#define WCD_MEMCOPY( source, source_size, destination, dest_size ) WCD_MEMCOPY_imp( source, source_size, destination, dest_size )



/**
  @brief Allocation memory routine
  
  @return void on success pointer pointing to the allocated memory otherwise NULL
  
  @param size Amount of memory to be allocated
*/
#define WCD_PMEM_MALLOC(size) WCD_OSAL_PMEM_MALLOC_imp(size)

/**
  @brief Deallocation memory routine
  
  @return None
  
  @param ptr Pointer to the memory to be dealocated
*/
#define WCD_PMEM_FREE(ptr) WCD_OSAL_PMEM_FREE_imp( (void*) ptr)


////////////////////////////////////////////////////////////////////////////////
//                               THREAD MACROS                                //
////////////////////////////////////////////////////////////////////////////////
#define WCD_JOIN_DONE          0
#define WCD_JOIN_FAILED        1
/**
  @brief Create Thread macro. Creates a thread with no attributes, default stack size which is immediately running, 
  note: the thread handle has to be deallocated once the thread is not on use.
 
  @return pointer to the thread handle.
 
  @param tread_function
  @param thread_parameter void* ptr to the data to be sent to the new thread
  @param priority Thread priority
 */
#define WCD_CREATE_THREAD(thread_function, thread_parameter, priority) WCD_OSAL_CREATE_THREAD_imp( thread_function, thread_parameter, priority )

/**
  @brief Join Thread macro. waits for the specified thread to die after executing the specified kill_function
 
  @return WCD_JOIN_DONE if no error found.
 
  @param threadhandle
  @param kill_function: this parameter is a function with the rquired routine to be executed to clean memory before killing the thread
                        it may also contain specific sequences to exit the thread.
  @param param: parameter to be passed to the thread kill function.
 */
#define WCD_JOIN_THREAD( threadhandle, kill_function, param ) WCD_OSAL_JOIN_THREAD( (WCD_HANDLE) threadhandle, kill_function, param )


////////////////////////////////////////////////////////////////////////////////
//                               MUTEX MACROS                                 //
////////////////////////////////////////////////////////////////////////////////


/**
  @brief Create Mutex. Creates a default mutex without a default owner
 
  @return on success Pointer to the new mutex otherwise NULL
 */
#define WCD_CREATE_MUTEX() WCD_OSAL_CREATE_MUTEX_imp()

/**
  @brief Destroy Mutex macro. Creates a default mutex without a default owner
 
  @return None
  
  @param mutex mutex to be destroyed
 */
#define WCD_DESTROY_MUTEX( mutex ) WCD_OSAL_DESTROY_MUTEX_imp( mutex )

/**
 @brief Wait for and acquire a mutex. Wait until a particular mutex has been released. Can set timeout to WCD_OSAL_INFINITE to not have a timeout if it is supported
  
 @return 
    --WCD_WAIT_DONE on success
    --WCD_WAIT_FAILED error ocurred while OS call
    --WCD_WAIT_ABANDONED the request has timed out
 
  @param mutex Mutex to be waited
 */
#define WCD_WAIT_FOR_MUTEX(mutex, timeout_milliseconds) WCD_OSAL_WAIT_MUTEX_imp( mutex, timeout_milliseconds )

/**
  @brief Macro to release a mutex handle.
 
 */
#define WCD_RELEASE_MUTEX(mutex_handle) WCD_OSAL_RELEASE_MUTEX_imp( mutex_handle )


////////////////////////////////////////////////////////////////////////////////
//                               EVENT MACROS                                 //
////////////////////////////////////////////////////////////////////////////////

#define WCD_WAIT_DONE          0
#define WCD_WAIT_FAILED        -2
#define WCD_WAIT_ABANDONED     -3

/**
  @brief Create Event macro. Creates an event with a particular start state and whether it is a manual reset event or not
 
  @return Event handler
  
  @param manual_reset if TRUE, client must reset the event before using it again. if false the event is reset automatically
  @param initial_state if TRUE, the event will be created and set, note this option is not supported by all the implementations
  @param event_name Name of the event, note this option is not supported by all the implementations
 */
#define WCD_CREATE_EVENT( manual_reset, initial_state, event_name ) WCD_OSAL_CREATE_EVENT_imp( manual_reset, initial_state )

/**
  @brief desroy event macro. destroys a previously created event.
 
  @return None
  
  @param handle to the event to be destroyed
 */
#define WCD_DESTROY_EVENT(handle) WCD_OSAL_DESTROY_EVENT_imp( handle )

/**
  \brief Wait for object macro. Wait until a particular event handle has been set.
 
  @return None
  
  @param handle handle of the event to be waited
  @param timeout_milliseconds Timeout in milliseconds, if WCD_INFINITE is set the call will return ONLY when the event is set.
 
 */
#define WCD_WAIT_FOR_EVENT(handle, timeout_milliseconds) WCD_OSAL_WAIT_EVENT_imp( handle, timeout_milliseconds )

/**
  @brief Macro to trigger a particular event.
 */
#define WCD_SET_EVENT(handle) WCD_OSAL_SET_EVENT_imp( handle )

/**
  @brief Macro to reset a particular event.
 */
#define WCD_RESET_EVENT(handle) WCD_OSAL_RESET_EVENT_imp( handle )

////////////////////////////////////////////////////////////////////////////////
//                               TIMER MACROS                                 //
////////////////////////////////////////////////////////////////////////////////

/**
  @brief Create Timer macro. Creates a timer handle to be used with WCD_SET_TIMER.
  
  @return Timer handle
  
  @param callback Function to be called on each timer tick.
  @param params  void pointer passed to the callback function, used for storing context.
  @param isPeriodic If FALSE the callback will be called once, if TRUE the callback will be called 
                    periodically.
*/
#define WCD_CREATE_TIMER(  callback, params, isPeriodic ) WCD_CREATE_TIMER_imp(  callback, params, isPeriodic )

/**
  @brief Enable Timer macro, Sets the timer to the specified time, once the timer ticks, the callback
    will be called, the function is used for both periodic and non periodic timers.

  @return none.
  
  @param Callback  callback function to be called once the timer ticks.
  @param params, void* pointer pointing to the data to be passed to the callback.
  @param milliseconds to wait before calling the callback.
*/
#define WCD_ENABLE_TIMER( handle, milliseconds ) WCD_ENABLE_TIMER_imp( handle, milliseconds )


/**
  @brief Disable Timer macro, stops the periodic
  
  @return none
  
  @param handle of the timer to be disabled.
  
*/  
#define WCD_DISABLE_TIMER( handle ) WCD_DISABLE_TIMER_imp( handle )


/**
  @brief Destroys the handle created by WCD
  
  @return none.
  
  @param handle, handle to be destroyed
  
*/
#define WCD_DESTROY_TIMER( handle ) WCD_DESTROY_TIMER_imp( handle )


/**
  @brief Make the thread sleep for a certain amount of time
 */
#define WCD_SLEEP(timeout_milliseconds) WCD_OSAL_SLEEP_imp(timeout_milliseconds) 

/**
  @brief  Gets the current timestamp for the specific module
 */
#define WCD_GET_CURRENT_TIMESTAMP(module)                  WCD_OSAL_GET_CURRENT_TIMESTAMP_imp(module)

/**
  @brief  Gets the new timestamp for the specific module
 */
#define WCD_GET_NEW_TIMESTAMP(module)                      WCD_OSAL_GET_NEW_TIMESTAMP_imp(module)

/**
  @brief  Gets the time duration for the specific module
 */
#define WCD_GET_TIME_DURATION(module, time_milliseconds)   WCD_OSAL_GET_TIME_DURATION_imp(module, time_milliseconds)

////////////////////////////////////////////////////////////////////////////////
//                               GLOBAL CONSTANTS                              //
////////////////////////////////////////////////////////////////////////////////

void split_packet_to_valid_byte_writes(uint16 arrSize, uint8* outputArray, uint8* count);

#endif


