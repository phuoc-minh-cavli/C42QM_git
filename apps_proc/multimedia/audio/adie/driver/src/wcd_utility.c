/**
@file wcd_utility.c
@brief 
Implementation of WCD Utility functions
*/
/*-------------------------------------------------------------------------------
     Copyright (c) 2011 Qualcomm Technologies Incorporated.
	 All rights reserved.
	 Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
                             Edit History
$Id: //components/rel/audio.tx/2.0/audio/adie/driver/src/wcd_utility.c#2 $
when       who     what, where, why
--------   ---     ------------------------------------------------------------
12/15/10	  SK		Initial revision
===============================================================================*/

#include "wcd_utility.h"
#include "wcd_osal.h"

#ifdef SPL_LOG_SUPPORT
	#include "wcd_utility.tmh"
#endif

#define SINGLE_ELEMENT 1

wcd_bool WCD_G_FALSE_BOOLEAN = WCD_FALSE;


/**
 * \brief This function creates and initializes a list
 *
 */
wcd_result
wcd_create_and_initialize_list (void **p_list_handle)
{
  wcd_list *new_list;
  wcd_result return_code = E_WCD_SUCCESS;

  do
  {
    WCD_DEBUG_INFO_0("+WCD_93XX: <Create and initialize list.>\n");

    if (p_list_handle == NULL) {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <List handle pointer is NULL.>\n");
      return_code = E_WCD_INVALID_HANDLE;
      break;
    }

    new_list = (wcd_list*) WCD_MALLOC (sizeof (wcd_list), WCD_MALLOC_START_TAG + 8);

    if (new_list == NULL) {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Unable to allocate memory.>\n");
      return_code = E_WCD_MEMORY_ALLOCATION_ERROR;
      break;
    }

    new_list->head = NULL;
    new_list->tail = NULL;
    new_list->list_size = 0;

    *p_list_handle = new_list;
   
  } while(WCD_G_FALSE_BOOLEAN);

  WCD_DEBUG_INFO_1("-WCD_93XX: <Create and initialize list. RC: %d.>\n", return_code);

  return return_code;
}

/**
 * \brief This function adds a new element with the data to the top of the list
 *
 */
wcd_result
wcd_push_to_list (wcd_list *p_list_handle, void *data)
{
  //create a new element
  wcd_list_element *new_element;
  wcd_result return_code = E_WCD_SUCCESS;
  
  do
  {
    WCD_DEBUG_INFO_0("+WCD_93XX: <Push to list.>\n");

    if (p_list_handle == NULL) {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <List handle is NULL.>\n");
      return_code = E_WCD_INVALID_HANDLE;
      break;
    }

    if (data == NULL) {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Data is NULL.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
    }

    new_element =
      (wcd_list_element *) WCD_MALLOC (sizeof (wcd_list_element), WCD_MALLOC_START_TAG + 9);

    if (new_element == NULL) {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Unable to allocate memory.>\n");
      return_code = E_WCD_MEMORY_ALLOCATION_ERROR;
      break;
    }

    new_element->data = data;
    new_element->previous = NULL;
    new_element->next = p_list_handle->head;

    if (p_list_handle->head != NULL) {
      p_list_handle->head->previous = new_element;
    }

    if (p_list_handle->tail == NULL) {
      p_list_handle->tail = new_element;
    }

    p_list_handle->head = new_element;
    p_list_handle->list_size++;
  } while(WCD_G_FALSE_BOOLEAN);

  WCD_DEBUG_INFO_1("-WCD_93XX: <Push to list. RC: %d.>\n", return_code);
  return return_code;
}

/**
 * \brief This function adds a new element with the data to the bottom of the list
 *
 */
wcd_result
wcd_add_to_list (wcd_list *p_list_handle, void *data)
{
  //create a new element
  wcd_list_element *new_element;
  wcd_result return_code = E_WCD_SUCCESS;

  do
  {
    WCD_DEBUG_INFO_0("+WCD_93XX: <Add to list.>\n");

    if (p_list_handle == NULL) {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <List handle is NULL.>\n");
      return_code = E_WCD_INVALID_HANDLE;
      break;
    }

    if (data == NULL) {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Data is NULL.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
    }

    new_element =
      (wcd_list_element *) WCD_MALLOC (sizeof (wcd_list_element), WCD_MALLOC_START_TAG + 9);

    if (new_element == NULL) {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Unable to allocate memory.>\n");
      return_code = E_WCD_MEMORY_ALLOCATION_ERROR;
      break;
    }

    new_element->data = data;
    new_element->previous = p_list_handle->tail;
    new_element->next = NULL;

    if (p_list_handle->tail != NULL) {
      p_list_handle->tail->next = new_element;
    }

    if (p_list_handle->head == NULL) {
      p_list_handle->head = new_element;
    }

    p_list_handle->tail = new_element;
    p_list_handle->list_size++;
  } while(WCD_G_FALSE_BOOLEAN);

  WCD_DEBUG_INFO_1("-WCD_93XX: <Add to list. RC: %d.>\n", return_code);

  return return_code;
}

/**
 * \brief This function removes the element with the data from the passed in list
 * 
 * @note This does not free the data within the element, it is the callee's
 * responsibility to free the data after calling this function.
 *
 */
wcd_result
wcd_remove_from_list (wcd_list *p_list_handle, void *data)
{
  //find the element with the data
  wcd_list_element *current;
  wcd_result return_code = E_WCD_SUCCESS;

  do
  {
    WCD_DEBUG_INFO_0("+WCD_93XX: <Remove from list.>\n");

    if (p_list_handle == NULL) {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <List handle is NULL.>\n");
      return_code = E_WCD_INVALID_HANDLE;
      break;
    }

    if (data == NULL) {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Data is NULL.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
    }

    current = wcd_find_element_inside_list (p_list_handle, data);

    //the list does not contain the data
    if (current == NULL) {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <List does not have data.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
    }

    if (p_list_handle->list_size != SINGLE_ELEMENT) {
      if (p_list_handle->tail != current) {
        current->next->previous = current->previous;
      }
      if (p_list_handle->head != current) {
        current->previous->next = current->next;
      }
    }

    if (p_list_handle->head == current) {
      p_list_handle->head = current->next;
    }

    if (p_list_handle->tail == current) {
      p_list_handle->tail = current->previous;
    }

    p_list_handle->list_size--;

    //free the element
    WCD_FREE (current, WCD_MALLOC_START_TAG + 9);
  } while(WCD_G_FALSE_BOOLEAN);

  WCD_DEBUG_INFO_1("-WCD_93XX: <Remove from list. RC:%d.>\n",return_code);
  return return_code;
}


/**
 * \brief This function finds the element containing the passed in data.
 * If the data is not found, or if the data or list is invalid
 * NULL is returned.
 *
 */
wcd_list_element *
wcd_find_element_inside_list (wcd_list *p_list_handle, void *data)
{
  wcd_list_element *current;

  if (p_list_handle == NULL) {
    return NULL;
  }

  if (data == NULL) {
    return NULL;
  }

  current = p_list_handle->head;

  while (current != NULL) {
    if (current->data == data) {
      return current;
    }

    current = current->next;
  }

  return NULL;
}

