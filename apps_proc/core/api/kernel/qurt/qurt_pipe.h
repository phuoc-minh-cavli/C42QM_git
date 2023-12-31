#ifndef QURT_PIPE_H
#define QURT_PIPE_H
/**
  @file qurt_pipe.h 

  @brief   Prototypes of the pipe interface API  
   This is a pipe or message queue
   
   The QuRT pipe service is used to perform synchronized exchange of data streams
   between threads.
   
   A size of the pipe is defined as a pipe attribute.
   
   A message to be send over pipe is called element. An element is of fixed size
   defined as a pipe attribute.
   
   Multiple threads can read from or write to a single pipe.

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

 Copyright (c) 2013-2015, 2019  by Qualcomm Technologies, Inc.  All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc. 

=============================================================================*/
#include "qurt_types.h"

/** @addtogroup pipe_types
@{ */
/*=============================================================================
                        CONSTANTS AND MACROS
=============================================================================*/

#define QURT_PIPE_ATTR_OBJ_SIZE_BYTES  16 

/*=============================================================================
                        TYPEDEFS
=============================================================================*/

/** Represents pipes.*/
typedef void * qurt_pipe_t;

/**  Represents pipe attributes. */
typedef struct qurt_pipe_attr  /* 8 byte aligned */
{
   unsigned long long _bSpace[QURT_PIPE_ATTR_OBJ_SIZE_BYTES/sizeof(unsigned long long)];
}qurt_pipe_attr_t;

/** @} */ /* end_addtogroup pipe_types */
/*=============================================================================
                        FUNCTIONS
=============================================================================*/
/**@ingroup func_qurt_pipe_attr_init
  Initializes the specified pipe attribute structure.

  @datatypes
  #qurt_pipe_attr_t
 
  @param[in,out] attr Pointer to the destination structure for the pipe 
                      attributes.

  @return
  None.

  @dependencies
  None.
*/
void qurt_pipe_attr_init(qurt_pipe_attr_t *attr);

/**@ingroup func_qurt_pipe_attr_set_elements

  Specifies the length of the memory area to be used for a pipe's data buffer. 
  
  The length is expressed in terms of the number of data elements that can be stored
  in the buffer.
  
  The buffer is allocated when the pipe is created.

  @datatypes
  #qurt_pipe_attr_t

  @param[in,out] attr Pointer to the pipe attribute structure.
  @param[in] elements Pipe length . 

  @return
  None.

  @dependencies
  None.
*/
void qurt_pipe_attr_set_elements(qurt_pipe_attr_t *attr, uint32 elements);

/**@ingroup func_qurt_pipe_attr_set_element_size

  Specifies the size of element in a pipe.
  
  @note1cont The user thread is responsible for the data elements to be sent through a pipe.

  @datatypes
  #qurt_pipe_attr_t

  @param[in,out] attr Pointer to the pipe attribute structure.
  @param[in] elements_size Size of element.

  @return
  None.

  @dependencies
  None.
*/
void qurt_pipe_attr_set_element_size(qurt_pipe_attr_t *attr, uint32 element_size);

/**@ingroup func_qurt_pipe_create
  Creates a pipe.\n
  
  Creates a pipe object and its associated data buffer, and initializes the pipe object.

  @datatypes
  #qurt_pipe_t \n
  #qurt_pipe_attr_t
  
  @param[out] pipe  Pointer to the created pipe object.
  @param[in]  attr  Pointer to the attribute structure used to create the pipe.

  @return 
  QURT_EOK -- Pipe created. \n
  QURT_EFAILED -- Pipe not created. \n
  QURT_ENOTALLOWED -- Pipe cannot be created in TCM/LPM.

  @dependencies
  None.
 */
int qurt_pipe_create(qurt_pipe_t *pipe, qurt_pipe_attr_t *attr);

/**@ingroup func_qurt_pipe_delete
  Deletes the pipe.\n
  
  Destroys the specified pipe and deallocates the pipe object and its
  associated data buffer.

  @note1cont Pipes must be deleted when they are no longer in use. Failure to do this 
             causes resource leaks in the QuRT library.\n
  @note1cont Pipes must not be deleted while they are still in use. If this happens the
             behaviour of QuRT is undefined. 

  @datatypes
  #qurt_pipe_t
  
  @param[in] pipe Pipe object to destroy.

  @return 
  None.

  @dependencies
  None.
 */
void qurt_pipe_delete(qurt_pipe_t pipe);

/**@ingroup func_qurt_pipe_send
  Writes a data item to the specified pipe. \n
  If a thread writes to a full pipe, it is suspended on the pipe. When another thread reads
  from the pipe, the suspended thread is awakened and can then write data to the pipe.\n
  Pipe writes are limited to transferring a single data element per operation.

  @datatypes
  #qurt_pipe_t \n
  
  @param[in] pipe Pipe object to write to.
  @param[in] data Pointer to the buffer containing the data to be sent.

  @return
  None.

  @dependencies
  None.
*/
void qurt_pipe_send(qurt_pipe_t pipe, void *data);

/**@ingroup func_qurt_pipe_receive
  Reads a data item from the specified pipe.

  If a thread reads from an empty pipe, it is suspended on the pipe. When another thread
  writes to the pipe, the suspended thread is awakened and can then read data from the pipe.
  Pipe data items are defined as elements. \n
  
  Pipe reads are limited to transferring a single element per operation.

  @datatypes
  #qurt_pipe_t
  
  @param[in] pipe Pipe object to read from.
  @param[out]data Pointer to the buffer where the data is received.

  @return
  None.

  @dependencies
  None.
*/
void qurt_pipe_receive(qurt_pipe_t pipe, void *data);

/**@ingroup func_qurt_pipe_try_send
  Writes a data item to the specified pipe (without suspending the thread if the pipe is full).\n

  If a thread writes to a full pipe, the operation returns immediately with value -1.
  Otherwise, success is always set to 0 to indicate a successful write operation.\n
  
  Pipe writes are limited to transferring a single data element per operation.

  @datatypes
  #qurt_pipe_t \n
  
  @param[in] pipe Pointer to the pipe object to write to.
  @param[in] data Pointer to the buffer containing the data to be sent.

  @return
  0 -- Success. \n
  -1 -- Failure (pipe full).

  @dependencies
  None.
*/ 
int qurt_pipe_try_send(qurt_pipe_t pipe, void *data);

/**@ingroup func_qurt_pipe_try_receive
  Reads a data item from the specified pipe (without suspending the thread if the pipe is
  empty).\n
  If a thread reads from an empty pipe, the operation returns immediately with value
  to -1. Otherwise, success is always set to 0 to indicate a successful read operation.\n

  Pipe reads are limited to transferring a single data element per operation.

  @datatypes
  #qurt_pipe_t
  
  @param[in]  pipe    Pipe object to read from.
  @param[out] data    Pointer to the buffer where the data is received.

  @return
  0 -- Success. \n
  -1 -- Failure (pipe empty).

  @dependencies
  None.
*/
int qurt_pipe_try_receive(qurt_pipe_t pipe, void *data);

/**@ingroup func_qurt_pipe_send_timed
  Writes a data item to the specified pipe.

  If a thread writes to a full pipe, it is suspended on the pipe for the specified
  time period. \n
  
  A timeout occurs when no thread reads data from the pipe during the specified
  time period.\n

  Pipe writes are limited to transferring a single data element per operation.

  @datatypes
  #qurt_pipe_t \n
  #qurt_time_t
  
  @param[in] pipe    Pipe object to write to.
  @param[in] data    Pointer to the buffer containing the data to be sent.
  @param[in] timeout How long a thread will wait on the pipe before the send is cancelled.
                      - QURT_PIPE_NO_WAIT � Return immdeiately without any waiting
                      - QURT_PIPE_WAIT_FOREVER � Equivalent to regular qurt_pipe_send()
                      - Timeout value (in system ticks) � Return after waiting for 
                        specified time value.

  @return
  QURT_EOK � Data item sent. 
  QURT_EINVALID - Invalid argument. 
  QURT_EFAILED_TIMEOUT � Wait ended because timeout interval was exceeded.

  @dependencies
  None.
*/
int qurt_pipe_send_timed(qurt_pipe_t pipe, void *data, qurt_time_t timeout);

/**@ingroup func_qurt_pipe_send_timed_ext
  Writes a data item to the specified pipe. 
  This is a deferrable version of qurt_pipe_send_timed

  If a thread writes to a full pipe, it is suspended on the pipe for the specified
  time period. \n 
  
  A timeout occurs when no thread reads data from the pipe during the specified
  time period.\n
  
  In case system goes into low power mode, timer would be halted and will resume
  from halted time/tick when system wakes up at later point of time. \n

  Pipe writes are limited to transferring a single data element per operation.

  @datatypes
  #qurt_pipe_t \n
  #qurt_time_t
  
  @param[in] pipe    Pipe object to write to.
  @param[in] data    Pointer to the buffer containing the data to be sent.
  @param[in] timeout How long a thread will wait on the pipe before the send is cancelled.
                      - QURT_PIPE_NO_WAIT � Return immdeiately without any waiting
                      - QURT_PIPE_WAIT_FOREVER � Equivalent to regular qurt_pipe_send()
                      - Timeout value (in system ticks) � Return after waiting for 
                        specified time value.

  @return
  QURT_EOK � Data item sent. 
  QURT_EINVALID - Invalid argument. 
  QURT_EFAILED_TIMEOUT � Wait ended because timeout interval was exceeded.

  @dependencies
  None.
*/
int qurt_pipe_send_timed_ext(qurt_pipe_t pipe, void *data, qurt_time_t timeout);


/**@ingroup func_qurt_pipe_receive_timed
  Reads a data item from the specified pipe.

  If a thread reads from an empty pipe, it is suspended on the pipe for the specified
  time period. When another thread writes to the pipe, the suspended thread is awakened
  and can then read data from the pipe. \n
 
  A timeout occurs when no thread writes data to the pipe during the specified time period.\n
  
  Pipe reads are limited to transferring a single element per operation.

  @datatypes
  #qurt_pipe_t
  
  @param[in]  pipe    Pipe object to read from.
  @param[out] data    Pointer to the buffer where the data is received.
  @param[in] timeout  How long a thread will wait on the pipe before the receive is cancelled.
                      - QURT_TIME_NO_WAIT � Return immediately without any waiting
                      - QURT_TIME_WAIT_FOREVER � Equivalent to regular qurt_pipe_receive()
                      - Timeout value (in system ticks) � Return after waiting for 
                        specified time value.
  @return
  None.

  @dependencies
  None.
*/
int qurt_pipe_receive_timed(qurt_pipe_t pipe, void *data, qurt_time_t timeout);

/**@ingroup func_qurt_pipe_receive_timed_ext
  Reads a data item from the specified pipe.
  This is a deferrable version of qurt_pipe_receive_timed

  If a thread reads from an empty pipe, it is suspended on the pipe for the specified
  time period. When another thread writes to the pipe, the suspended thread is awakened
  and can then read data from the pipe. \n
 
  A timeout occurs when no thread writes data to the pipe during the specified time period.\n

  In case system goes into low power mode, timer would be halted and will resume
  from halted time/tick when system wakes up at later point of time. \n

  Pipe reads are limited to transferring a single element per operation.

  @datatypes
  #qurt_pipe_t
  
  @param[in]  pipe    Pipe object to read from.
  @param[out] data    Pointer to the buffer where the data is received.
  @param[in] timeout  How long a thread will wait on the pipe before the receive is cancelled.
                      - QURT_TIME_NO_WAIT � Return immediately without any waiting
                      - QURT_TIME_WAIT_FOREVER � Equivalent to regular qurt_pipe_receive()
                      - Timeout value (in system ticks) � Return after waiting for 
                        specified time value.
  @return
  None.

  @dependencies
  None.
*/
int qurt_pipe_receive_timed_ext(qurt_pipe_t pipe, void *data, qurt_time_t timeout);

/**@ingroup func_qurt_pipe_flush
  Reset the pipe to its original empty state.

  Any pending data contained in the pipe at the time it is flused is discarded. \n
 
  @datatypes
  #qurt_pipe_t
  
  @param[in]  pipe    Pipe object for the pipe being flushed.
  
  @return
  QURT_EOK � Pipe successfully flushed. 
  QURT_EFAILED � Pipe flushed failed.  

  @dependencies
  None.
*/
int qurt_pipe_flush(qurt_pipe_t pipe);

#endif  /* QURT_PIPE_H */

