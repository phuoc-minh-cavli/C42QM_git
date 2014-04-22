/**
@file route_manager.c
@brief
Implementation and internal functions of WCD route manager
*/
/*-------------------------------------------------------------------------------
     Copyright (c) 2011 Qualcomm Technologies Incorporated.
     All rights reserved.
     Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
                             Edit History
$Id: //components/rel/audio.tx/2.0/audio/adie/driver/src/route_manager.c#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
05/12/14   AG       Coverity warning fix
07/25/13   KNM      Added a generic function to determine connection direction.
03/04/13   KNM      Fixed SAL warnings and added checks to cross-check validity of
                    a pointer before freeing it.
05/22/12   KNM      Modified get_codec_info function to read codec chip id & version.
12/15/10   SK       Initial revision
===============================================================================*/

#include "wcd_utility.h"
#include "route_manager.h"
#include "target_defs.h"
#include "wcd_gen_id_defines.h"
#include "wcd_osal.h"

#ifdef SPL_LOG_SUPPORT
	#include "route_manager.tmh"
#endif


/**************                    CLEAR ROUTE                  ***************/

/**
 * \brief Clears a list of elements
 */
wcd_result clear_element_list(wcd_list* list_handle)
{
  wcd_result return_code = E_WCD_SUCCESS;
  wcd_list_element *current;
  wcd_element *element;

  current = list_handle->head;

  //go through all the elements and free the list element
  //do not free the data inside the list element since all route elements are globals
  //and will be needed for other routes!
  while(current != NULL) {
    element = (wcd_element *) current->data;
    current = current->next;

    return_code = wcd_remove_from_list (list_handle, element);
    if (return_code != E_WCD_SUCCESS) {
      return return_code;
    }

    //free tx path types
    if (element->type == WCD_TX_PATH_TYPE_ID)
    {
      return_code = wcd_free_txpath(element);
      if (return_code != E_WCD_SUCCESS) {
        return return_code;
      }
    }

    //free the bus port if this is the last connection using it
    if (element->type == WCD_BUS_PORT_TYPE_ID && element->reference_count == 0)
    {
      return_code = wcd_free_slimbusport(element);
      if (return_code != E_WCD_SUCCESS) {
        return return_code;
      }
    }
  }

  return return_code;
}

wcd_result clear_element_table(wcd_element_table* element_table)
{
  uint32 iterator = 0;

  wcd_list *list_handle;
  wcd_element* element;
  wcd_list_element* list_iterator = NULL;
  wcd_result return_code = E_WCD_SUCCESS;

  for(iterator = 0; iterator < WCD_MAX_ELEMENT_TYPES; iterator++)
  {
    if (element_table->elements_list[iterator] != NULL)
    {
      list_handle = element_table->elements_list[iterator];
      list_iterator = element_table->elements_list[iterator]->head;

      while (list_iterator != NULL)
      {
        element = (wcd_element*)list_iterator->data;
        list_iterator = list_iterator->next;

        return_code = wcd_remove_from_list (list_handle, element);
        if (return_code != E_WCD_SUCCESS) {
          return return_code;
        }
      }

      WCD_FREE(element_table->elements_list[iterator], WCD_MALLOC_START_TAG + 8);
      element_table->elements_list[iterator] = NULL;
    }
  }

  return return_code;
}

/**
 * \brief This function frees all the internal members inside the wcd_route
 * Also frees the route.
 */
wcd_result wcd_clear_route(wcd_route* p_handle)
{
  wcd_result return_code = E_WCD_SUCCESS;

  WCD_DEBUG_INFO_0("+WCD_93XX: <Clear route.>\n");

  do
  {
    if (p_handle == NULL) {
      WCD_DEBUG_INFO_0("=WCD_93XX: <Invalid route handle.>\n");
      return_code = E_WCD_INVALID_HANDLE;
      break;
    }

    if (p_handle->elements != NULL)
    {
      WCD_DEBUG_INFO_0("=WCD_93XX: <Clearing elements.>\n");
      return_code = clear_element_list(p_handle->elements);
    }

    if (p_handle->differential_elements != NULL)
    {
      WCD_DEBUG_INFO_0("=WCD_93XX: <Clearing differential elements.>\n");
      return_code = clear_element_list(p_handle->differential_elements);
    }

    if (p_handle->sidetone_elements != NULL)
    {
      WCD_DEBUG_INFO_0("=WCD_93XX: <Clearing sidetone elements.>\n");
      return_code = clear_element_list(p_handle->sidetone_elements);
    }

    if (p_handle->route_table != NULL)
    {
      WCD_DEBUG_INFO_0("=WCD_93XX: <Clearing route table.>\n");
      return_code = clear_element_table(p_handle->route_table);
    }

    if (p_handle->differential_table != NULL)
    {
      WCD_DEBUG_INFO_0("=WCD_93XX: <Clearing differential table.>\n");
      return_code = clear_element_table(p_handle->differential_table);
    }

    if (p_handle->sidetone_table != NULL)
    {
      WCD_DEBUG_INFO_0("=WCD_93XX: <Clearing sidetone table.>\n");
      return_code = clear_element_table(p_handle->sidetone_table);
    }

    //free the list and the route and return success
	if (p_handle->elements != NULL)
	{
    WCD_FREE (p_handle->elements, WCD_MALLOC_START_TAG + 8);
	  p_handle->elements = NULL;
	}

	if (p_handle->differential_elements != NULL)
	{
    WCD_FREE (p_handle->differential_elements, WCD_MALLOC_START_TAG + 8);
	  p_handle->differential_elements = NULL;
	}

	if (p_handle->sidetone_elements != NULL)
	{
    WCD_FREE (p_handle->sidetone_elements, WCD_MALLOC_START_TAG + 8);
	  p_handle->sidetone_elements = NULL;
	}

	if (p_handle->route_table != NULL)
	{
    WCD_FREE (p_handle->route_table, WCD_MALLOC_START_TAG + 5);
	  p_handle->route_table = NULL;
	}

	if (p_handle->differential_table != NULL)
	{
    WCD_FREE (p_handle->differential_table, WCD_MALLOC_START_TAG + 5);
	  p_handle->differential_table = NULL;
	}

	if (p_handle->sidetone_table != NULL)
	{
    WCD_FREE (p_handle->sidetone_table, WCD_MALLOC_START_TAG + 5);
	  p_handle->sidetone_table = NULL;
	}

  WCD_FREE (p_handle, WCD_MALLOC_START_TAG + 4);

  } while(WCD_G_FALSE_BOOLEAN);

  WCD_DEBUG_INFO_1("-WCD_93XX: <Clear route. RC:%d>\n", return_code);
  return return_code;
}


/**************                 CONNECTION ROUTING               ***************/

/**
 * \brief This function routes from the source to the sink, using the provided route stack.
 *
 */
wcd_result
wcd_determine_route(wcd_route** p_handle, wcd_element* source, wcd_element* sink, uint32* route_stack)
{
  wcd_result return_code = E_WCD_SUCCESS;
  wcd_route *route = NULL;
  uint32 route_stack_iterator = 0;
  wcd_element* txpath_element = NULL;

  uint32 connectable_elements_iterator = 0;
  wcd_element* current_element = NULL;
  wcd_codec_info codec_info;
  adie_codec_direction connection_direction;

  do
  {
    WCD_DEBUG_INFO_0("+WCD_93XX: <WCD Route.>\n");

    if (p_handle == NULL)
    {
      WCD_DEBUG_INFO_0("=WCD_93XX: <Route handle is NULL.>\n");
      return_code = E_WCD_INVALID_HANDLE;
      break;
    }

    if (*p_handle != NULL)
    {
      WCD_DEBUG_INFO_0("=WCD_93XX: <Route is not NULL.>\n");
      return_code = E_WCD_INVALID_HANDLE;
      break;
    }

    if (source == NULL || sink == NULL || route_stack == NULL)
    {
      WCD_DEBUG_INFO_0("=WCD_93XX: <Sink/source/route stack is NULL.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
    }

    route = (wcd_route*)WCD_MALLOC(sizeof(wcd_route), WCD_MALLOC_START_TAG + 4);
    if (route == NULL)
    {
      WCD_DEBUG_INFO_0("=WCD_93XX: <Unable to create new route.>\n");
      return_code = E_WCD_MEMORY_ALLOCATION_ERROR;
      break;
    }

    route->id = 1;
    route->elements = NULL;
    route->differential_elements = NULL;
    route->sidetone_elements = NULL;

    route->route_table = NULL;
    route->differential_table = NULL;
    route->sidetone_table = NULL;

    return_code = wcd_create_and_initialize_list((void**)&(route->elements));
    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_INFO_0("=WCD_93XX: <Unable to create element list.>\n");
      wcd_clear_route(route);
      break;
    }

    //first verify sink matches
    if (sink->type == route_stack[route_stack_iterator])
    {
      return_code = wcd_push_to_list(route->elements, sink);
    }
    else
    {
      WCD_DEBUG_INFO_0("=WCD_93XX: <Sink element doesn't match top of route stack.>\n");
      wcd_clear_route(route);
      return_code = E_WCD_UNABLE_TO_ROUTE;
      break;
    }

    if (return_code != E_WCD_SUCCESS)
    {
      break;
    }
    
    // connection direction
    return_code = wcd_get_connection_direction(source, sink, &connection_direction);
    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Error getting connection direction.>\n");
      wcd_clear_route(route);
      break;
    }
    
    //if TX connection figure out Decimator now
    if(connection_direction == ADIE_CODEC_TX)
    {
      return_code = wcd_determine_txpath(source, sink, &txpath_element);

      if (return_code != E_WCD_SUCCESS)
      {
        wcd_clear_route(route);
        break;
      }
    }

    //increment the iterator to the next element and start routing
    //the sink was already pushed in
    route_stack_iterator++;

    (void)wcd_get_codec_info(&codec_info);

    while(route_stack[route_stack_iterator] != WCD_ROUTE_STACK_END && return_code == E_WCD_SUCCESS)
    {
      return_code = E_WCD_UNABLE_TO_ROUTE;

      if (route_stack[route_stack_iterator] & WCD_ROUTE_STACK_SPECIAL_TYPE)
      {
        /*Special routings are not present in Helicon codec. So, skipping the below
        check condition for helicon codec*/
	    if (codec_info.codec != E_WCD_HELICON)
	    {
          //special element type
          return_code = wcd_handle_special_routing(route, route_stack[route_stack_iterator]);
          route_stack_iterator++;
          continue; //route stack loop
        }
	  }

      connectable_elements_iterator = 0;
      current_element = (wcd_element*)(route->elements->head->data);

      while (connectable_elements_iterator < current_element->connectable_elements_count)
      {
        //type matches
        if (current_element->connectable_elements[connectable_elements_iterator] != NULL &&
			current_element->connectable_elements[connectable_elements_iterator]->type
          == route_stack[route_stack_iterator])
        {
          //for mux we need to verify it is not used
          if (route_stack[route_stack_iterator] == WCD_MUX_TYPE_ID &&
             current_element->connectable_elements[connectable_elements_iterator]->
             connected_element != NULL)
          {
            WCD_DEBUG_INFO_0("=WCD_93XX: <Mux in use. Trying another.>\n");
            connectable_elements_iterator++;
            continue; //connectable elements loop
          }

          //for decimator we need to verify its the same as the decimator we selected earlier
          if (route_stack[route_stack_iterator] == WCD_TX_PATH_TYPE_ID &&
            current_element->connectable_elements[connectable_elements_iterator] != txpath_element)
          {
            WCD_DEBUG_INFO_0("=WCD_93XX: <TX Path is not what we are looking for. Trying another.>\n");
            connectable_elements_iterator++;
            continue; //connectable elements loop
          }
		  if (codec_info.codec == E_WCD_HELICON)
		  {
            // ADC2 is muxed with IN2 and IN3 in Helicon routing. So, finding the connectable element
			// for ADC2
            wcd_find_ADC_Path (&(current_element->connectable_elements[connectable_elements_iterator]),
                            source, route_stack[route_stack_iterator]);
          }
          //push the new element
          return_code = wcd_push_to_list(route->elements,
            current_element->connectable_elements[connectable_elements_iterator]);

          break;  //connectable elements loop
        }

        //if the type does not match increment the iterator
        connectable_elements_iterator++;
        continue; //connectable elements loop
      }

      //increment the route stack iterator
      route_stack_iterator++;
      continue; //route stack loop
    }

    if (return_code != E_WCD_SUCCESS)
    {
      //got an error
      WCD_DEBUG_INFO_0("=WCD_93XX: <Error creating a route.>\n");
      wcd_clear_route(route);
      //wcd_free_txpath(txpath_element);
      break;
    }

    //have to push source now
    return_code = wcd_push_to_list(route->elements, source);

    if (return_code != E_WCD_SUCCESS)
    {
      //got an error
      WCD_DEBUG_INFO_0("=WCD_93XX: <Error pushing source to route.>\n");
      wcd_clear_route(route);
      //wcd_free_txpath(txpath_element);
      break;
    }

    //if we pushed a ADC/DMIC have to push the MICBIAS MANAGER connected to it as well
    if (source->type == WCD_ADC_TYPE_ID || source->type == WCD_DMIC_TYPE_ID)
    {
      connectable_elements_iterator = 0;
      return_code = E_WCD_UNABLE_TO_ROUTE;

      WCD_DEBUG_INFO_0("=WCD_93XX: <ADC/DMIC. Pushing MICBIAS Manager.>\n");

      while (connectable_elements_iterator < source->connectable_elements_count)
      {
        if (source->connectable_elements[connectable_elements_iterator]->type == WCD_MICBIAS_MANAGER_TYPE_ID)
        {
          return_code = wcd_push_to_list(route->elements,
            source->connectable_elements[connectable_elements_iterator]);
          break;  //micbias loop
        }

        connectable_elements_iterator++;
      }

      if (return_code != E_WCD_SUCCESS)
      {
        //got an error
        WCD_DEBUG_INFO_0("=WCD_93XX: <Error pushing MICBIAS Manager to route.>\n");
        wcd_clear_route(route);
        //wcd_free_txpath(txpath_element);
        break;
      }
    }

    //done here
    *p_handle = route;

  } while(WCD_G_FALSE_BOOLEAN);

  WCD_DEBUG_INFO_1("-WCD_93XX: <WCD Route. RC:%d.>\n", return_code);

  return return_code;

}


/**
 * \brief This function routes the differential portion of the connection using the supplied
 * route stack.
 *
 */
wcd_result
wcd_determine_differential_route(wcd_route* p_route, wcd_element* sink, uint32* route_stack)
{
  wcd_result return_code = E_WCD_SUCCESS;

  uint32 route_stack_iterator = 0;
  uint32 connectable_elements_iterator = 0;
  wcd_element* current_element = NULL;

  do
  {
    WCD_DEBUG_INFO_0("+WCD_93XX: <Determine differential route.>\n");

    if (p_route == NULL)
    {
      //got an error
      WCD_DEBUG_INFO_0("=WCD_93XX: <Route is NULL.>\n");
      return_code = E_WCD_INVALID_HANDLE;
      break;
    }

    if (p_route->differential_elements != NULL)
    {
      //got an error
      WCD_DEBUG_INFO_0("=WCD_93XX: <Differential elements in route is not NULL.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
    }

    return_code = wcd_create_and_initialize_list((void**)&(p_route->differential_elements));
    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_INFO_0("=WCD_93XX: <Unable to create differential element list.>\n");
      break;
    }

    //first verify sink matches
    if (sink->type == route_stack[route_stack_iterator])
    {
      return_code = wcd_push_to_list(p_route->differential_elements, sink);
    }
    else
    {
      WCD_DEBUG_INFO_0("=WCD_93XX: <Sink element doesn't match top of route stack.>\n");
      return_code = E_WCD_UNABLE_TO_ROUTE;
      break;
    }

    if (return_code != E_WCD_SUCCESS)
    {
      break;
    }

    //increment the iterator to the next element and start routing
    //the sink was already pushed in
    route_stack_iterator++;

    while(route_stack[route_stack_iterator] != WCD_ROUTE_STACK_END && return_code == E_WCD_SUCCESS)
    {
      return_code = E_WCD_UNABLE_TO_ROUTE;

      if (route_stack[route_stack_iterator] & WCD_ROUTE_STACK_SPECIAL_TYPE)
      {
          //special element type
          return_code = wcd_handle_special_routing(p_route, route_stack[route_stack_iterator]);

          route_stack_iterator++;
          continue; //route stack loop
	  }

      connectable_elements_iterator = 0;
      current_element = (wcd_element*)(p_route->differential_elements->head->data);

      while (connectable_elements_iterator < current_element->connectable_elements_count)
      {
        //type matches
        if (current_element->connectable_elements[connectable_elements_iterator]->type
          == route_stack[route_stack_iterator])
        {
          //for mux we need to verify it is not used
          if (route_stack[route_stack_iterator] == WCD_MUX_TYPE_ID &&
             current_element->connectable_elements[connectable_elements_iterator]->
             connected_element != NULL)
          {
            WCD_DEBUG_INFO_0("=WCD_93XX: <Mux in use. Trying another.>\n");
            connectable_elements_iterator++;
            continue; //connectable elements loop
          }

          //push the new element
          return_code = wcd_push_to_list(p_route->differential_elements,
            current_element->connectable_elements[connectable_elements_iterator]);

          break;  //connectable elements loop
        }

        //if the type does not match increment the iterator
        connectable_elements_iterator++;
        continue; //connectable elements loop
      }

      //increment the route stack iterator
      route_stack_iterator++;
      continue; //route stack loop
    }

    if (return_code != E_WCD_SUCCESS)
    {
      //got an error
      WCD_DEBUG_INFO_0("=WCD_93XX: <Error creating a differential route.>\n");
      break;
    }

  } while(WCD_G_FALSE_BOOLEAN);

  WCD_DEBUG_INFO_1("-WCD_93XX: <Determine differential route. RC:%d.>\n", return_code);
  return return_code;

}

/**************                     ELEMENT TABLE                   ***************/

/**
 * \brief Creates and resets the element table.
 */
wcd_result create_element_table(wcd_element_table** p_element_table)
{
  uint32 iterator;
  wcd_element_table* new_element_table;
  wcd_result return_code = E_WCD_SUCCESS;

  if (p_element_table == NULL)
  {
    WCD_DEBUG_INFO_0("=WCD_93XX: <Unable to reset element table.>\n");
    return E_WCD_INVALID_PARAMETER_ERROR;
  }
  else
  {
    new_element_table = (wcd_element_table*)WCD_MALLOC(sizeof(wcd_element_table), WCD_MALLOC_START_TAG + 5);
    if (new_element_table == NULL )
    {
      WCD_DEBUG_INFO_0("=WCD_93XX: <Error mallocing space for element table.>\n");
      return E_WCD_OS_CALL_ERROR;
    }

    for(iterator = 0; iterator < WCD_MAX_ELEMENT_TYPES; iterator++)
    {
      return_code = wcd_create_and_initialize_list( (void**)&(new_element_table->elements_list[iterator]) );

      if (return_code != E_WCD_SUCCESS)
      {
	  clear_element_table(new_element_table); 
      WCD_FREE(new_element_table, WCD_MALLOC_START_TAG + 5); 
	  return return_code;
      }
    }

    *p_element_table = new_element_table;
  }
  return E_WCD_SUCCESS;
}

/**
 * \brief Adds the element pointer to the element table array
 */
wcd_result add_element_to_table(wcd_element* element, wcd_element_table* element_table)
{
  if (element == NULL || element_table == NULL)
  {
    WCD_DEBUG_INFO_0("=WCD_93XX: <Could not add element to table. One of the required parameters are null.>\n");

    return E_WCD_INVALID_PARAMETER_ERROR;
  }

  if ( element->type > WCD_MAX_ELEMENT_TYPES || element->type <= 0 )
  {
    WCD_DEBUG_INFO_0("=WCD_93XX: <Could not add element to table. Element type is invalid.>\n");

    return E_WCD_INVALID_PARAMETER_ERROR;
  }

  if (element_table->elements_list[element->type - 1] == NULL)
  {
    WCD_DEBUG_INFO_0("=WCD_93XX: <Could not add element to table. Elment list inside of table is NULL.>\n");

    return E_WCD_INVALID_PARAMETER_ERROR;
  }

  return wcd_add_to_list( element_table->elements_list[element->type - 1], element );
}


