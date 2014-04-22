/* Copyright (c) 2011, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "linked_list.h"
#include <stdio.h>
#include <string.h>

#define LOG_TAG "LocSvc_utils_ll"
#include "log_util.h"
//#include "platform_lib_includes.h"
#include <stdlib.h>
#include <stdint.h>

typedef struct list_element {
   struct list_element* next;
   struct list_element* prev;
   void* data_ptr;
   void (*dealloc_func)(void*);
}list_element;

typedef struct list_state {
   list_element* p_head;
   list_element* p_tail;
} list_state;

/* ----------------------- END INTERNAL FUNCTIONS ---------------------------------------- */

/*===========================================================================

  FUNCTION:   linked_list_init

  ===========================================================================*/
linked_list_err_type linked_list_init(void** list_data)
{
    list_state* tmp_list;
    if( list_data == NULL )
   {
      LOC_LOGH0("Invalid list parameter!");
      return eLINKED_LIST_INVALID_PARAMETER;
   }

   tmp_list = (list_state*)calloc(1, sizeof(list_state));
   if( tmp_list == NULL )
   {
      LOC_LOGH0("Unable to allocate space for list!");
      return eLINKED_LIST_FAILURE_GENERAL;
   }

   tmp_list->p_head = NULL;
   tmp_list->p_tail = NULL;

   *list_data = tmp_list;

   return eLINKED_LIST_SUCCESS;
}

/*===========================================================================

  FUNCTION:   linked_list_destroy

  ===========================================================================*/
linked_list_err_type linked_list_destroy(void** list_data)
{
   list_state* p_list;
    
   if( list_data == NULL )
   {
      LOC_LOGH0("Invalid list parameter!");
      return eLINKED_LIST_INVALID_HANDLE;
   }

   p_list = (list_state*)*list_data;

   linked_list_flush(p_list);

   free(*list_data);
   *list_data = NULL;

   return eLINKED_LIST_SUCCESS;
}

/*===========================================================================

  FUNCTION:   linked_list_add

  ===========================================================================*/
linked_list_err_type linked_list_add(void* list_data, void *data_obj, void (*dealloc)(void*))
{
   list_state* p_list;
   list_element* elem;
   list_element* tmp;

   LOC_LOGL1("Adding to list data_obj = 0x%08X", data_obj);
   if( list_data == NULL )
   {
      LOC_LOGH0("Invalid list parameter!");
      return eLINKED_LIST_INVALID_HANDLE;
   }

   if( data_obj == NULL )
   {
      LOC_LOGH0("Invalid input parameter!");
      return eLINKED_LIST_INVALID_PARAMETER;
   }

   p_list = (list_state*)list_data;
   elem = (list_element*)malloc(sizeof(list_element));
   if( elem == NULL )
   {
      LOC_LOGH0("Memory allocation failed");
      return eLINKED_LIST_FAILURE_GENERAL;
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

   return eLINKED_LIST_SUCCESS;
}

/*===========================================================================

  FUNCTION:   linked_list_remove

  ===========================================================================*/
linked_list_err_type linked_list_remove(void* list_data, void **data_obj)
{
   list_state* p_list;
   list_element* tmp;

   LOC_LOGL0("Removing from list");
   if( list_data == NULL )
   {
      LOC_LOGH0("Invalid list parameter!");
      return eLINKED_LIST_INVALID_HANDLE;
   }

   if( data_obj == NULL )
   {
      LOC_LOGH0("Invalid input parameter!");
      return eLINKED_LIST_INVALID_PARAMETER;
   }

   p_list = (list_state*)list_data;
   if( p_list->p_tail == NULL )
   {
      return eLINKED_LIST_UNAVAILABLE_RESOURCE;
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
   free(tmp);

   return eLINKED_LIST_SUCCESS;
}

/*===========================================================================

  FUNCTION:   linked_list_empty

  ===========================================================================*/
int linked_list_empty(void* list_data)
{
   if( list_data == NULL )
   {
      LOC_LOGL0("Invalid list parameter!");
      return (int)eLINKED_LIST_INVALID_HANDLE;
   }
   else
   {
      list_state* p_list = (list_state*)list_data;
      return p_list->p_head == NULL ? 1 : 0;
   }
}

/*===========================================================================

  FUNCTION:   linked_list_flush

  ===========================================================================*/
linked_list_err_type linked_list_flush(void* list_data)
{
    list_state* p_list;
    if( list_data == NULL )
   {
      LOC_LOGL0("Invalid list parameter!");
      return eLINKED_LIST_INVALID_HANDLE;
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
      free(p_list->p_head);

      p_list->p_head = tmp;
   }

   p_list->p_tail = NULL;

   return eLINKED_LIST_SUCCESS;
}

/*===========================================================================

  FUNCTION:   linked_list_search

  ===========================================================================*/
linked_list_err_type linked_list_search(void* list_data, void **data_p,
                                        bool (*equal)(void* data_0, void* data),
                                        void* data_0, bool rm_if_found)
{
   list_state* p_list;
   list_element* tmp;

   LOC_LOGL0("Search the list");
   if( list_data == NULL || NULL == equal )
   {
      LOC_LOGH2("Invalid list parameter! list_data %p equal %p",
            list_data, equal);
      return eLINKED_LIST_INVALID_HANDLE;
   }

   p_list = (list_state*)list_data;
   if( p_list->p_tail == NULL )
   {
      return eLINKED_LIST_UNAVAILABLE_RESOURCE;
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
         free(tmp);
       }

       tmp = NULL;
     } else {
       tmp = tmp->next;
     }
   }

   return eLINKED_LIST_SUCCESS;
}

