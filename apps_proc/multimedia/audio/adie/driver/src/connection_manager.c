/**
@file connection_manager.c
@brief
Implementation and internal functions of WCD connection manager
*/
/*-------------------------------------------------------------------------------
     Copyright (c) 2011 Qualcomm Technologies Incorporated.
     All rights reserved.
     Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
                             Edit History
$Id: //components/rel/audio.tx/2.0/audio/adie/driver/src/connection_manager.c#3 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/25/13   KNM     Added a generic function to determine connection direction.
03/07/13   SSP     Driver Unification Update
03/04/13   knm     Fixed SAL warnings and avoided memory allocation for differential
                   path if connection type is default.
05/22/12   knm     Modified get_codec_info function to read codec chip id & version.
12/15/10   SK      Initial revision
===============================================================================*/

#include "wcd_utility.h"
#include "connection_manager.h"
#include "target_defs.h"
#include "route_manager.h"
#include "wcd_gen_id_defines.h"
#include "wcd_osal.h"

#ifdef SPL_LOG_SUPPORT
	#include "connection_manager.tmh"
#endif

WCD_CORE_PAGED_CODE_DATA_START

#define WCD_TRIGGER_COMMAND_ELEMENT_TYPE_MASK   0x0FFF0000
#define WCD_TRIGGER_COMMAND_ELEMENT_TYPE_SHIFT  4*4
#define WCD_TRIGGER_COMMAND_EVENT_TYPE_MASK     0x0000FFFF
#define WCD_TRIGGER_COMMAND_PARAMETER_MASK      0xFFFFFFFF

WCD_CORE_PAGED_CODE_DATA_STOP

WCD_CORE_PAGED_CODE_SEGMENT_START

/**************                  CLEAR CONNECTION                ***************/

/**
 * \brief This function frees all the internal members inside the connection_info
 * Also frees the connection info.
 *
 * \Note Do not call this function on a connection that is connected...bad things
 * will happen.
 */
wcd_result
wcd_clear_connection (wcd_connection_info *p_handle)
{
  WCD_CORE_PAGED_CODE
	
  wcd_result return_code = E_WCD_SUCCESS;

  {
    WCD_DEBUG_INFO_0("+WCD_93XX: <Clear WCD connection.>\n");
    if (p_handle == NULL) {
      return_code = E_WCD_INVALID_HANDLE;
      goto end;
    }

    //now clear the route info
    if (p_handle->route != NULL)
    {
      return_code = wcd_clear_route (p_handle->route);
    }

    //everything internal is freed, free the connection and return
    WCD_FREE (p_handle, WCD_MALLOC_START_TAG + 2);
  } 

end:
  WCD_DEBUG_INFO_1("-WCD_93XX: <Clear WCD connection. RC: %d.>\n", return_code);

  return return_code;
}

/**************                CONNECTION RESERVING              ***************/

/**
 * \brief Reserves the elements in a connection.
 */
wcd_result reserve_connection(wcd_connection_info* p_handle)
{
  WCD_CORE_PAGED_CODE
	
  wcd_result return_code = E_WCD_SUCCESS;

  wcd_list_element* iterator = NULL;
  wcd_element* previous_element = NULL;
  wcd_element* current_element = NULL;

  do
  {
    WCD_DEBUG_INFO_0("+WCD_93XX: <Reserving connection.>\n");

    if (p_handle == NULL || p_handle->route == NULL)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Connection or route is NULL.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
    }

    if ( create_element_table( &p_handle->route->route_table ) != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Error resetting route table.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
    }

	if( p_handle->route->differential_elements != NULL)
	{
      if ( create_element_table( &p_handle->route->differential_table ) != E_WCD_SUCCESS)
      {
        WCD_DEBUG_INFO_0("=WCD_93XX: <Error resetting differential table.>\n");
        return_code = E_WCD_INVALID_PARAMETER_ERROR;
        break;
      }
	}

    if (p_handle->route->elements == NULL)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Elements in route is NULL.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
    }

    iterator = p_handle->route->elements->head;

    while(iterator != NULL)
    {
      current_element = (wcd_element*)iterator->data;

	  if(current_element != NULL)
	  {
        current_element->reference_count++;

        if (previous_element != NULL)
        {
          current_element->connected_element = previous_element;
        }
	  }

      previous_element = current_element;

      return_code = add_element_to_table( current_element, p_handle->route->route_table );
      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error adding element to element table.>\n");
        break;
      }

      iterator = iterator->next;
    }

    //repeat for differential
    if(p_handle->route->differential_elements != NULL && return_code == E_WCD_SUCCESS)
    {
      previous_element = NULL;
      iterator = p_handle->route->differential_elements->head;

      while(iterator != NULL)
      {
        current_element = (wcd_element*)iterator->data;

		if(current_element != NULL)
		{
          current_element->reference_count++;

          if (previous_element != NULL)
          {
            current_element->connected_element = previous_element;
          }
		}

        previous_element = current_element;

        return_code = add_element_to_table( current_element, p_handle->route->differential_table );
        if (return_code != E_WCD_SUCCESS)
        {
          WCD_DEBUG_ERROR_0("=WCD_93XX: <Error adding element to element table.>\n");
          break;
        }

        iterator = iterator->next;
      }
    }

  } while(WCD_G_FALSE_BOOLEAN);

  WCD_DEBUG_INFO_1("-WCD_93XX: <Reserving connection. RC:%d.>\n", return_code);

  return return_code;
}

/**
 * \brief Frees all reservations on the elements in a connection.
 */
wcd_result unreserve_connection(wcd_connection_info* p_handle)
{
  WCD_CORE_PAGED_CODE
	
  wcd_result return_code = E_WCD_SUCCESS;

  wcd_list_element* iterator = NULL;
  wcd_element* current_element = NULL;

  do
  {
    WCD_DEBUG_INFO_0("+WCD_93XX: <Unreserving connection.>\n");

    if (p_handle == NULL || p_handle->route == NULL)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Connection or route is NULL.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
    }

    if (p_handle->route->elements == NULL)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Elements in route is NULL.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
    }

    iterator = p_handle->route->elements->head;

    while(iterator != NULL)
    {
      current_element = (wcd_element*)iterator->data;

      current_element->reference_count--;

      if (current_element->reference_count == 0)
      {
        current_element->connected_element = NULL;
      }

      iterator = iterator->next;
    }

    //repeat for differential
    if(p_handle->route->differential_elements != NULL)
    {
      iterator = p_handle->route->differential_elements->head;

      while(iterator != NULL)
      {
        current_element = (wcd_element*)iterator->data;

        current_element->reference_count--;

        if (current_element->reference_count == 0)
        {
          current_element->connected_element = NULL;
        }

        iterator = iterator->next;
      }
    }

    //repeat for sidetone
    if(p_handle->route->sidetone_elements != NULL)
    {
      iterator = p_handle->route->sidetone_elements->head;

      while(iterator != NULL)
      {
        current_element = (wcd_element*)iterator->data;

        current_element->reference_count--;

        if (current_element->reference_count == 0)
        {
          current_element->connected_element = NULL;
        }

        iterator = iterator->next;
      }
    }

  } while(WCD_G_FALSE_BOOLEAN);

  WCD_DEBUG_INFO_1("-WCD_93XX: <Reserving connection. RC:%d.>\n", return_code);

  return return_code;
}

/**************                 CONNECTION ROUTING               ***************/

/**
 * \brief This function validates the source/sink/connection type and manages all the
 * routes that are needed for the particular connection.
 *
 */
wcd_result wcd_connect(wcd_usecase* p_usecase, wcd_connection_info* p_connection,
                       wcd_source* source, wcd_sink* sink,
                       wcd_connection_type connection_type)
{
  WCD_CORE_PAGED_CODE
	
  wcd_result return_code = E_WCD_SUCCESS;

  wcd_element* source_element = NULL;
  wcd_element* sink_element = NULL;
  uint32* route_stack = NULL;

  wcd_element* differential_sink_element = NULL;
  uint32* differential_route_stack = NULL;

  wcd_codec_info codec_info;

  do
  {
    WCD_DEBUG_INFO_0("+WCD_93XX: <WCD Connect.>\n");

    if (p_usecase == NULL)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Usecase handle is NULL.>\n");
      return_code = E_WCD_INVALID_HANDLE;
      break;
    }

    if (p_connection == NULL)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Connection handle is NULL.>\n");
      return_code = E_WCD_INVALID_HANDLE;
      break;
    }

    //validate source
    return_code = wcd_map_source_to_element(p_usecase, source, &source_element);
    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Error mapping source element.>\n");
      break;
    }
    
    //validate sink
    return_code = wcd_map_sink_to_element(*source, sink, connection_type, &sink_element);
    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Error mapping sink element.>\n");
      break;
    }
    
    // connection direction
    return_code = wcd_get_connection_direction(source_element, sink_element, &p_connection->connection_direction);
    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Error getting connection direction.>\n");
      break;
    }

    //route_stack
    return_code = wcd_determine_route_stack(*source, *sink, &route_stack);
    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Error getting route stack.>\n");
      break;
    }

    //route
    return_code = wcd_determine_route(&(p_connection->route), source_element, sink_element, route_stack);
    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Error routing from source to sink.>\n");
      break;
    }

	/* Helicon does not support differential connections. So, skipping
	   differential connections for helicon codec */
    return_code = wcd_get_codec_info(&codec_info);
    //route differential
    if ((connection_type == E_WCD_CONNECTION_DIFFERENTIAL) && (codec_info.codec != E_WCD_HELICON))
    {
      WCD_DEBUG_INFO_0("=WCD_93XX: <Connecting differential portion.>\n");

      //validate sink
      return_code = wcd_map_sink_to_differential_element(*sink, connection_type, &differential_sink_element);
      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error mapping differential sink element.>\n");
        wcd_clear_route(p_connection->route);
        p_connection->route = NULL;
        break;
      }

      //route stack
      return_code = wcd_determine_differential_route_stack(*sink,
                                                          &differential_route_stack);
      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error getting differential route stack.>\n");
        wcd_clear_route(p_connection->route);
        p_connection->route = NULL;
        break;
      }

      //route
      return_code = wcd_determine_differential_route(p_connection->route,
        differential_sink_element, differential_route_stack);
      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error generating differential route.>\n");
        wcd_clear_route(p_connection->route);
        p_connection->route = NULL;
        break;
      }
    }
    //reserve elements and set connected sources
    return_code = reserve_connection(p_connection);
    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Error reserving connection.>\n");
      wcd_clear_route(p_connection->route);
      p_connection->route = NULL;
      break;
    }

    if (p_usecase->context_pointer == NULL || p_usecase->context_pointer->busport_elements == NULL)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Usecase does not have context pointer.>\n");
      wcd_clear_route(p_connection->route);
      p_connection->route = NULL;
      break;
    }

    //add the bus port elements to the usecase's context pointer
    if (p_connection->route->route_table->elements_list[WCD_BUS_PORT_TYPE_ID - 1]->head != NULL)
    {
      return_code = wcd_add_to_list(p_usecase->context_pointer->busport_elements,
        (void*)p_connection->route->route_table->elements_list[WCD_BUS_PORT_TYPE_ID - 1]->head->data);
    }

    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Error adding busport element to context pointer.>\n");
      wcd_clear_route(p_connection->route);
      p_connection->route = NULL;
      break;
    }
	/* Helicon does not support differential connections. So, skipping
	   differential connections for helicon codec */
    if (codec_info.codec != E_WCD_HELICON)
    {
      //add the differential bus port elements to the usecase's context pointer
      if (p_connection->route->differential_table != NULL && p_connection->route->differential_table->elements_list[WCD_BUS_PORT_TYPE_ID - 1]->head != NULL)
      {
        return_code = wcd_add_to_list(p_usecase->context_pointer->busport_elements,
          (void*)p_connection->route->differential_table->elements_list[WCD_BUS_PORT_TYPE_ID - 1]->head->data);
      }
      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error adding differential busport element to context pointer.>\n");
        wcd_clear_route(p_connection->route);
        p_connection->route = NULL;
        break;
      }
    }

    p_connection->state = E_WCD_CONNECTION_CONNECTED;

  } while(WCD_G_FALSE_BOOLEAN);

  WCD_DEBUG_INFO_1("-WCD_93XX: <WCD Connect. RC:%d.>\n", return_code);

  return return_code;

}

/**
 * \brief This function disconnects all elements in an existing connection.
 *
 */
wcd_result wcd_disconnect(wcd_usecase* p_usecase, wcd_connection_info* p_connection)
{
  WCD_CORE_PAGED_CODE
	
  wcd_result return_code = E_WCD_SUCCESS;

  {
    WCD_DEBUG_INFO_0("+WCD_93XX: <WCD Disconnect.>\n");

    if (p_connection == NULL || p_usecase == NULL)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Usecase handle or connection handle is NULL.>\n");
      return_code = E_WCD_INVALID_HANDLE;
      goto end;
    }

    if (p_usecase->context_pointer == NULL || p_usecase->context_pointer->busport_elements == NULL)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Usecase's context pointer is NULL.>\n");
      return_code = E_WCD_INVALID_HANDLE;
    }

    //remove busport element from usecase's context pointer
    if (p_connection->route->route_table->elements_list[WCD_BUS_PORT_TYPE_ID - 1]->head != NULL)
    {
      if (p_usecase->context_pointer != NULL && p_usecase->context_pointer->busport_elements != NULL)
      {
        return_code = wcd_remove_from_list(p_usecase->context_pointer->busport_elements,
          (void*)p_connection->route->route_table->elements_list[WCD_BUS_PORT_TYPE_ID - 1]->head->data);
      }
    }

    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Error removing busport element from context pointer.>\n");
    }

    //remove differential busport element from usecase's context pointer
    if (p_connection->route->differential_table != NULL && p_connection->route->differential_table->elements_list[WCD_BUS_PORT_TYPE_ID - 1]->head != NULL)
    {
      if (p_usecase->context_pointer != NULL && p_usecase->context_pointer->busport_elements != NULL)
      {
        return_code = wcd_remove_from_list(p_usecase->context_pointer->busport_elements,
          (void*)p_connection->route->differential_table->elements_list[WCD_BUS_PORT_TYPE_ID - 1]->head->data);
      }
    }

    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Error removing differential busport element from context pointer.>\n");
    }


    //unreserve elemnts in route
    return_code = unreserve_connection(p_connection);
    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Error unreserving elements.>\n");
      //goto end;
    }

    //kill route/connection
    return_code = wcd_clear_connection(p_connection);
  } 

end:
  WCD_DEBUG_INFO_1("-WCD_93XX: <WCD Disconnect. RC:%d.>\n", return_code);

  return return_code;
}

/**************                 CONNECTION EXECUTION               ***************/

/**
 * \brief Executes a ACDB command on a list of elements. Looking for that element type
 * (from the command) and executing that element's event.
 */
wcd_result
wcd_execute_command_on_elements(wcd_list* element_list, wcd_acdb_command* command,
                                wcd_element_table* element_table, wcd_event_context_pointer* context_ptr)
{
  WCD_CORE_PAGED_CODE
	
  wcd_result return_code = E_WCD_SUCCESS;
  wcd_element* element;
  wcd_list_element* current;

  wcd_list* old_element_list;

  uint32 command_element_type = 0;
  uint32 command_event_id = 0;
  uint32 command_parameter = 0;

  do
  {
    WCD_DEBUG_INFO_0("+WCD_93XX: <Execute command on elements.>\n");

    if (element_list == NULL || command == NULL)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <One of the required parameters is NULL.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
    }

    old_element_list = element_list;

    command_element_type = ((command->parameter_one) & WCD_TRIGGER_COMMAND_ELEMENT_TYPE_MASK) >>
                        WCD_TRIGGER_COMMAND_ELEMENT_TYPE_SHIFT;
    command_event_id = ((command->parameter_one) & WCD_TRIGGER_COMMAND_EVENT_TYPE_MASK);
    command_parameter = ((command->parameter_two) & WCD_TRIGGER_COMMAND_PARAMETER_MASK);

    if (command_element_type == 0 || command_event_id == 0)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Element type or event inside of ACDB command is 0.>\n");
      return_code = E_WCD_INVALID_ACDB_COMMAND;
      break;
    }

    if (command_element_type > WCD_MAX_ELEMENT_TYPES || command_element_type <= 0)
    {
      WCD_DEBUG_ERROR_1("=WCD_93XX: <Invalid element type (%u) inside of ACDB command.>\n", (unsigned int)command_element_type);
      return_code = E_WCD_INVALID_ACDB_COMMAND;
      break;
    }

    //get the smaller, better element list if element_table is passed in
    if (element_table != NULL)
    {
      element_list = element_table->elements_list[command_element_type - 1];
    }

    if (element_list == NULL)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Element list is NULL.>\n");
      return_code = E_WCD_INVALID_ACDB_COMMAND;
      break;
    }

    current = element_list->head;

    while(current != NULL && return_code == E_WCD_SUCCESS)
    {
      element = (wcd_element*)current->data;

      //element matches command's element type
      if (element->type == command_element_type)
      {
        if (command_event_id >= element->events_count || command_event_id <= 0)
        {
          WCD_DEBUG_ERROR_1("=WCD_93XX: <Invalid event ID (%u) inside of ACDB command.>\n", (unsigned int)command_event_id);
          return_code = E_WCD_INVALID_ACDB_COMMAND;
          break;
        }

        //luckily event id is arranged by the event IDs inside each element
        return_code = element->events_map[command_event_id - 1].event_function(element, command_parameter, old_element_list, context_ptr);
      }

      //loop to the next element in the list
      current = current->next;
    } //element list loop

  } while(WCD_G_FALSE_BOOLEAN);

  WCD_DEBUG_INFO_1("-WCD_93XX: <Execute command on elements. RC:%d.>\n", return_code);

  return return_code;
}

WCD_CORE_PAGED_CODE_SEGMENT_STOP


