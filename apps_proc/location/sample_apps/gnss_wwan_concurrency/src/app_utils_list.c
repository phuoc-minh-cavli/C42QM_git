/*****************************************************************************
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "app_utils_list.h"
#include "app_utils_misc.h"
#include "txm_module.h"

typedef struct list_element {
   struct list_element* next;
   struct list_element* prev;
   void* data_ptr;
   void (*dealloc_func)(void*);
} list_element;

typedef struct list_state {
   list_element* p_head;
   list_element* p_tail;
} list_state;

app_utils_list_err app_utils_list_init(void** list_data)
{
    int ret = -1;
    list_state* tmp_list;
    if( list_data == NULL )
    {
      APP_LOGE("Invalid list parameter!");
      return LINKED_LIST_INVALID_PARAMETER;
    }

    if(0 !=(ret = app_utils_byte_allocate((VOID **)&tmp_list, sizeof(list_state))))
    {
      APP_LOGE("Unable to allocate space for list! %d", ret);
      return LINKED_LIST_FAILURE_GENERAL;
    }

    tmp_list->p_head = NULL;
    tmp_list->p_tail = NULL;

    *list_data = tmp_list;

    return LINKED_LIST_SUCCESS;
}

app_utils_list_err app_utils_list_destroy(void** list_data)
{
   list_state* p_list;
    
   if( list_data == NULL )
   {
      APP_LOGE("Invalid list parameter!");
      return LINKED_LIST_INVALID_HANDLE;
   }

   p_list = (list_state*)*list_data;

   app_utils_list_flush(p_list);

   app_utils_byte_release(*list_data);
   *list_data = NULL;

   return LINKED_LIST_SUCCESS;
}

app_utils_list_err app_utils_list_add(void* list_data, void *data_obj, void (*dealloc)(void*))
{
   int ret = -1;
   list_state* p_list;
   list_element* elem;
   list_element* tmp;

   if( list_data == NULL )
   {
      APP_LOGE("Invalid list parameter!");
      return LINKED_LIST_INVALID_HANDLE;
   }

   if( data_obj == NULL )
   {
      APP_LOGE("Invalid input parameter!");
      return LINKED_LIST_INVALID_PARAMETER;
   }

   p_list = (list_state*)list_data;
   if(0 !=(ret = app_utils_byte_allocate((VOID **)&elem, sizeof(list_element))))
   {
      APP_LOGE("Memory allocation failed err %d", ret);
      return LINKED_LIST_FAILURE_GENERAL;
   }

   /* Copy data to newly created element */
   elem->data_ptr = data_obj;
   elem->next = NULL;
   elem->prev = NULL;
   elem->dealloc_func = dealloc;

   /* Replace head element */
   tmp = p_list->p_head;
   p_list->p_head = elem;
   /* Point next to the previous head element */
   p_list->p_head->next = tmp;

   if( tmp != NULL )
   {
      tmp->prev = p_list->p_head;
   }
   else
   {
      p_list->p_tail = p_list->p_head;
   }

   return LINKED_LIST_SUCCESS;
}

/*It is callers responsibility to deallocat the data_obj */
app_utils_list_err app_utils_list_remove(void* list_data, void **data_obj)
{
   list_state* p_list;
   list_element* tmp;

   if( list_data == NULL )
   {
      APP_LOGE("Invalid list parameter!");
      return LINKED_LIST_INVALID_HANDLE;
   }

   if( data_obj == NULL )
   {
      APP_LOGE("Invalid input parameter!");
      return LINKED_LIST_INVALID_PARAMETER;
   }

   p_list = (list_state*)list_data;
   if( p_list->p_tail == NULL )
   {
      return LINKED_LIST_UNAVAILABLE_RESOURCE;
   }

   tmp = p_list->p_tail;

   /* Replace tail element */
   p_list->p_tail = tmp->prev;

   if( p_list->p_tail != NULL )
   {
      p_list->p_tail->next = NULL;
   }
   else
   {
      p_list->p_head = p_list->p_tail;
   }

   /* Copy data to output param */
   *data_obj = tmp->data_ptr;

   /* Free allocated list element */
   app_utils_byte_release(tmp);

   return LINKED_LIST_SUCCESS;
}

int app_utils_list_empty(void* list_data)
{
   if( list_data == NULL )
   {
      APP_LOGE("Invalid list parameter!");
      return (int)LINKED_LIST_INVALID_HANDLE;
   }
   else
   {
      list_state* p_list = (list_state*)list_data;
      return p_list->p_head == NULL ? 1 : 0;
   }
}

app_utils_list_err app_utils_list_flush(void* list_data)
{
    list_state* p_list;
    if( list_data == NULL )
   {
      APP_LOGE("Invalid list parameter!");
      return LINKED_LIST_INVALID_HANDLE;
   }

   p_list = (list_state*)list_data;

   /* Remove all dynamically allocated elements */
   while( p_list->p_head != NULL )
   {
      list_element* tmp = p_list->p_head->next;

      /* Free data pointer if told to do so. */
      if( p_list->p_head->dealloc_func != NULL )
      {
         p_list->p_head->dealloc_func(p_list->p_head->data_ptr);
      }

      /* Free list element */
      app_utils_byte_release(p_list->p_head);

      p_list->p_head = tmp;
   }

   p_list->p_tail = NULL;

   return LINKED_LIST_SUCCESS;
}

app_utils_list_err app_utils_list_search(void* list_data, void **data_p,
                                        boolean (*equal)(void* data_0, void* data),
                                        void* data_0, boolean rm_if_found)
{
   list_state* p_list;
   list_element* tmp;

   if( list_data == NULL || NULL == equal )
   {
      APP_LOGE("Invalid list parameter! list_data %p equal %p",
            list_data, equal);
      return LINKED_LIST_INVALID_HANDLE;
   }

   p_list = (list_state*)list_data;
   if( p_list->p_tail == NULL )
   {
      return LINKED_LIST_UNAVAILABLE_RESOURCE;
   }

   tmp = p_list->p_head;

   if (NULL != data_p) {
     *data_p = NULL;
   }

   while (NULL != tmp) {
     if ((*equal)(data_0, tmp->data_ptr)) {
       if (NULL != data_p) {
         *data_p = tmp->data_ptr;
       }

       if (rm_if_found) {
         if (NULL == tmp->prev) {
           p_list->p_head = tmp->next;
         } else {
           tmp->prev->next = tmp->next;
         }

         if (NULL == tmp->next) {
           p_list->p_tail = tmp->prev;
         } else {
           tmp->next->prev = tmp->prev;
         }

         tmp->prev = tmp->next = NULL;

         // dealloc data if it is not copied out && caller
         // has given us a dealloc function pointer.
         if (NULL == data_p && NULL != tmp->dealloc_func) {
             tmp->dealloc_func(tmp->data_ptr);
         }
         app_utils_byte_release(tmp);
       }

       tmp = NULL;
     } else {
       tmp = tmp->next;
     }
   }

   return LINKED_LIST_SUCCESS;
}

