
/*============================================================================
 
GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS
    
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 1999-2000, 2007-2008,2016 Qualcomm Technologies Incorporated.  All Rights Reserved.
Qualcomm Confidential and Proprietary
============================================================================*/

/*============================================================================

EDIT HISTORY FOR MODULE

$Header: //components/rel/core.tx/6.0/buses/uart/mdd/src/uart_event_queue.c#1 $

when          who        what, where, why
-----------   --------   -----------------------------------------------------
04-oct-2016   PR         replace memcpy with memscpy 
06-Aug-2011   cmihalik   Initial revision.

============================================================================*/

#include "comdef.h"

#include "assert.h"

#include "uart_event_queue.h"
#include "uart_log.h"
#include "qurt_mutex.h"
#include <stdlib.h>
#include <stringl.h>


//----------------------------------------------------------------------------
//  UART_TRACE_BUFFER_FILE_ID must be unique amongst all files writing to the
//  trace buffer.
//----------------------------------------------------------------------------

#define  UART_TRACE_BUFFER_FILE_ID            6

//----------------------------------------------------------------------------
//  uart_event_queue_init
//----------------------------------------------------------------------------

boolean
uart_event_queue_init( uart_event_queue_type* q_ptr )
{
   if( NULL == q_ptr )
      return( FALSE );

   //  Create lock.

   q_ptr->lock = (qurt_mutex_t *) malloc(sizeof(qurt_mutex_t));
   if(q_ptr->lock == NULL){return( FALSE );}
   
   qurt_mutex_init(q_ptr->lock);

   q_ptr->num_items = 0;
   q_ptr->max_items = UART_EVENT_QUEUE_SIZE;
   q_ptr->end_ptr   = &q_ptr->items[q_ptr->max_items];
   q_ptr->read_ptr  = q_ptr->items;
   q_ptr->write_ptr = q_ptr->items;

   return( TRUE );
}

//----------------------------------------------------------------------------
//  uart_event_queue_deinit
//----------------------------------------------------------------------------

void
uart_event_queue_deinit( uart_event_queue_type* q_ptr )
{
   if( NULL == q_ptr )
      return;

   //  Free the lock.

   qurt_mutex_destroy( q_ptr->lock );
   q_ptr->lock = NULL;
}

//----------------------------------------------------------------------------
//  uart_event_queue_add_item
//----------------------------------------------------------------------------

boolean
uart_event_queue_add_item( uart_event_queue_type* q_ptr, uint32 item )
{
   boolean          isAdded = FALSE;


   if( NULL == q_ptr )
      return( FALSE );

   if( NULL == q_ptr->lock)
      return( FALSE );

   //  Lock the queue.

   qurt_mutex_lock(q_ptr->lock );

   //  Add the item.

   if( q_ptr->num_items < q_ptr->max_items )
   {
      q_ptr->num_items++;

      *q_ptr->write_ptr++ = item;
      if( q_ptr->write_ptr >= q_ptr->end_ptr )
      {
         q_ptr->write_ptr = q_ptr->items;
      }

      isAdded = TRUE;
   }

   //  Unlock the queue.

   qurt_mutex_unlock(q_ptr->lock );

   return( isAdded );
}

//----------------------------------------------------------------------------
//  uart_event_queue_remove_all_items
//----------------------------------------------------------------------------

void                           
uart_event_queue_remove_all_items( uart_event_queue_type* q_ptr, uint32* dest, int* p_num_items,size_t des_size )
{
   int              total_num_items;
   int              num_items;
   int              num_bytes;
   int              num_bytes_copy;   



   if( NULL == q_ptr )
      return;

   if( NULL == dest )
      return;
   
   if( des_size <= 0 )
    {
      return;
    }

   if( NULL == p_num_items )
      return;

   if( NULL == q_ptr->lock )
      return;

   //  Lock.

   qurt_mutex_lock(q_ptr->lock );

   //  Error checking.

   if( !(( 0 <= *p_num_items ) && ( *p_num_items <= q_ptr->max_items )))
   {
      qurt_mutex_unlock(q_ptr->lock );
      return;
   }
   
   //  Return items.  read_ptr == write_ptr can happen if the queue is 
   //  empty or the queue is full.  We know the queue is empty if num_items
   //  is 0.  Otherwise, we can read the 

   total_num_items = 0;

   if( 0 == q_ptr->num_items )
   {
      //  Queue is empty.  Do nothing.

      total_num_items = 0;
   }
   else if( q_ptr->read_ptr < q_ptr->write_ptr )
   {
      //  Can read to write ptr.

      num_items = q_ptr->write_ptr - q_ptr->read_ptr;
      num_bytes = num_items * sizeof( q_ptr->items[0] );
	  
	  num_bytes_copy = memscpy( dest,des_size,q_ptr->read_ptr,num_bytes );
      if( num_bytes != num_bytes_copy )
	  {
	     UART_LOG_1( TRACE, "[%d] Total Number of bytes copied", num_bytes_copy );  
	  }

      total_num_items = num_items;
   }
   else
   {
      //  read_ptr to end of buffer.

      num_items = q_ptr->end_ptr - q_ptr->read_ptr;
      num_bytes = num_items * sizeof( q_ptr->items[0] );

      num_bytes_copy = memscpy( dest,des_size,q_ptr->read_ptr,num_bytes );
      if( num_bytes != num_bytes_copy )
	  {
	     UART_LOG_1( TRACE, "[%d] Total Number of bytes copied", num_bytes_copy );  
	  }
      dest += num_items;
      des_size -= num_bytes;

      total_num_items = num_items;

      //  Beginning of buffer to write_ptr.

      num_items = q_ptr->write_ptr - q_ptr->items;
      num_bytes = num_items * sizeof( q_ptr->items[0] );

      num_bytes_copy = memscpy( dest,des_size,q_ptr->read_ptr,num_bytes );
      if( num_bytes != num_bytes_copy )
	  {
	     UART_LOG_1( TRACE, "[%d] Total Number of bytes copied", num_bytes_copy );  
	  }

      total_num_items += num_items;
   }

   *p_num_items = total_num_items;

   //  Queue is now empty.

   q_ptr->num_items = 0;
   q_ptr->read_ptr  = q_ptr->items;
   q_ptr->write_ptr = q_ptr->items;

   //  Unlock.

   qurt_mutex_unlock( q_ptr->lock );
}

