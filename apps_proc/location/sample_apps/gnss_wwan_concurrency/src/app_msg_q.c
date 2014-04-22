/*****************************************************************************
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include "app_msg_q.h"
#include "app_utils_list.h"
#include "app_utils_misc.h"

typedef struct app_msg_q
{
   void* msg_list;                       /* Linked list to store information */
   TX_MUTEX* list_mutex;                 /* Mutex for exclusive access to message queue */
   TX_EVENT_FLAGS_GROUP* list_signal;    /* Signal, To wait for message */
} app_msg_q;

static app_msq_q_err_type _convert_linked_list_err_type(app_utils_list_err linked_list_val)
{
   switch( linked_list_val )
   {
   case LINKED_LIST_SUCCESS:
      return APP_MSG_Q_SUCCESS;
   case LINKED_LIST_INVALID_PARAMETER:
      return APP_MSG_Q_INVALID_PARAMETER;
   case LINKED_LIST_INVALID_HANDLE:
      return APP_MSG_Q_INVALID_HANDLE;
   case LINKED_LIST_UNAVAILABLE_RESOURCE:
      return APP_MSG_Q_UNAVAILABLE_RESOURCE;
   case LINKED_LIST_INSUFFICIENT_BUFFER:
      return APP_MSG_Q_INSUFFICIENT_BUFFER;
   case LINKED_LIST_FAILURE_GENERAL:
   default:
      return APP_MSG_Q_FAILURE_GENERAL;
   }
}

app_msq_q_err_type app_msg_q_init(void** msg_q_data)
{
    app_msg_q* tmp_msg_q = NULL;
    if( msg_q_data == NULL )
    {
        APP_LOGE("Invalid msg_q_data parameter!");
        return APP_MSG_Q_INVALID_PARAMETER;
    }

    app_utils_byte_allocate((void **)&tmp_msg_q, sizeof(app_msg_q));

    if( tmp_msg_q == NULL )
    {
        APP_LOGE("Unable to allocate space for message queue!");
        return APP_MSG_Q_FAILURE_GENERAL;
    }

    if( app_utils_list_init(&tmp_msg_q->msg_list) != 0 )
    {
        APP_LOGE("Unable to initialize storage list!");
        app_utils_byte_release(tmp_msg_q);
        return APP_MSG_Q_FAILURE_GENERAL;
    }

    if( app_utils_mutex_init(&tmp_msg_q->list_mutex, "list_mutex") != 0 )
    {
        APP_LOGE("Unable to initialize list mutex!");
        app_utils_list_destroy(&tmp_msg_q->msg_list);
        app_utils_byte_release(tmp_msg_q);
        return APP_MSG_Q_FAILURE_GENERAL;
    }

    if( app_utils_init_signal(&tmp_msg_q->list_signal, "list_signal") != 0)
    {
        APP_LOGE("Unable to initialize list Signal!");
        app_utils_list_destroy(&tmp_msg_q->msg_list);
        app_utils_mutex_delete(tmp_msg_q->list_mutex);
        app_utils_byte_release(tmp_msg_q);
        return APP_MSG_Q_FAILURE_GENERAL;
    }

    *msg_q_data = tmp_msg_q;

    return APP_MSG_Q_SUCCESS;
}

const void* app_msg_q_init2()
{
  void* q = NULL;
  if (APP_MSG_Q_SUCCESS != app_msg_q_init(&q)) {
    q = NULL;
  }
  return q;
}

app_msq_q_err_type app_msg_q_destroy(void** msg_q_data)
{
    app_msg_q* p_msg_q;

   if( msg_q_data == NULL )
   {
      APP_LOGE("Invalid msg_q_data parameter!");
      return APP_MSG_Q_INVALID_HANDLE;
   }

   p_msg_q = (app_msg_q*)*msg_q_data;

   app_utils_list_destroy(&p_msg_q->msg_list);
   app_utils_mutex_delete(p_msg_q->list_mutex);
   app_utils_deinit_signal(p_msg_q->list_signal);
   app_utils_byte_release(*msg_q_data);
   *msg_q_data = NULL;

   return APP_MSG_Q_SUCCESS;
}

app_msq_q_err_type app_msg_q_snd(const void* msg_q_data, void* msg_obj, void (*dealloc)(void*))
{
   app_msg_q* p_msg_q;
   app_msq_q_err_type rv;
   if( msg_q_data == NULL )
   {
      APP_LOGE("Invalid msg_q_data parameter!");
      return APP_MSG_Q_INVALID_HANDLE;
   }
   if( msg_obj == NULL )
   {
      APP_LOGE("Invalid msg_obj parameter!");
      return APP_MSG_Q_INVALID_PARAMETER;
   }

   p_msg_q = (app_msg_q*)msg_q_data;
   
   app_utils_mutex_get(p_msg_q->list_mutex);
   rv = _convert_linked_list_err_type(app_utils_list_add(p_msg_q->msg_list, msg_obj, dealloc));
   app_utils_set_signal(p_msg_q->list_signal, p_msg_q->list_mutex);
   app_utils_mutex_put(p_msg_q->list_mutex);

   return rv;
}

app_msq_q_err_type app_msg_q_rcv(const void* msg_q_data, void** msg_obj)
{
    app_msg_q* p_msg_q;
    app_msq_q_err_type rv;
   
    if( msg_q_data == NULL )
    {
       APP_LOGE("Invalid msg_q_data parameter!");
       return APP_MSG_Q_INVALID_HANDLE;
    }

    if( msg_obj == NULL )
    {
       APP_LOGE("Invalid msg_obj parameter!");
       return APP_MSG_Q_INVALID_PARAMETER;
    }

    p_msg_q = (app_msg_q*)msg_q_data;

    app_utils_mutex_get(p_msg_q->list_mutex);

    /* Wait for data in the message queue */
    while( app_utils_list_empty(p_msg_q->msg_list))
    {
        app_utils_wait_on_signal(p_msg_q->list_signal, p_msg_q->list_mutex);
    }

    rv = _convert_linked_list_err_type(app_utils_list_remove(p_msg_q->msg_list, msg_obj));
    app_utils_mutex_put(p_msg_q->list_mutex);

    return rv;
}

app_msq_q_err_type app_msg_q_flush(void* msg_q_data)
{
   app_msg_q* p_msg_q;
   app_msq_q_err_type rv;
   if ( msg_q_data == NULL )
   {
      APP_LOGE("Invalid msg_q_data parameter!");
      return APP_MSG_Q_INVALID_HANDLE;
   }

   p_msg_q = (app_msg_q*)msg_q_data;

   app_utils_mutex_get(p_msg_q->list_mutex);
   rv = _convert_linked_list_err_type(app_utils_list_flush(p_msg_q->msg_list));
   app_utils_mutex_put(p_msg_q->list_mutex);

   return rv;
}

