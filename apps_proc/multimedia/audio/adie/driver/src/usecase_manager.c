/**
@file uscase_manager.c
@brief 
Implementation and internal functions of WCD usecase manager
*/
/*-------------------------------------------------------------------------------
     Copyright (c) 2011 Qualcomm Technologies Incorporated.
	 All rights reserved.
	 Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
                             Edit History
$Id: //components/rel/audio.tx/2.0/audio/adie/driver/src/usecase_manager.c#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/30/13   AG       SAL warning fixes after logging is enabled in debug mode
03/07/13   SSP      Driver Unification Update
03/04/13   KNM      Fixed SAL warnings and added checks to cross-check validity of
                    a pointer before freeing it.
03/01/13   KNM		Refactored bus manager i/f and replaced E_WCD_PGD with
                    E_WCD_CODEC_MAIN to suit both I2C and SB PGD i/f.
12/15/10   SK	    Initial revision
===============================================================================*/

#include "usecase_manager.h"
#include "wcd_utility.h"
#include "connection_manager.h"
#include "target_defs.h"
#include "bus_manager.h"
#ifdef WCD_MBHC_ENABLE
  #include "audio_resource_manager.h"
#endif

#ifdef SPL_LOG_SUPPORT
	#include "usecase_manager.tmh"
#endif

/**
 * \brief Global list of all active usecases
 */
wcd_list *gp_usecase_list = NULL;


/**************               GLOBAL USECASE LIST              ***************/

/** 
 * \brief Initialize the global usecase list.
 *
 */
wcd_result wcd_initialize_global_usecase_list(void)
{
  return wcd_create_and_initialize_list ( (void**) &gp_usecase_list );
}

/** 
 * \brief Deinitialize the global usecase list.
 *
 */
wcd_result wcd_deinitialize_global_usecase_list(void)
{
  //in the case there are still usecases open do not deinitialize the codec driver.
  if(gp_usecase_list && gp_usecase_list->list_size != 0)
  {
    return E_WCD_USECASE_STILL_OPEN;
  }

  WCD_FREE(gp_usecase_list, WCD_MALLOC_START_TAG + 8);
  gp_usecase_list = NULL;

  return E_WCD_SUCCESS;
}

/** 
 * \brief Finds a usecase inside the global usecase list.
 *
 */
wcd_result wcd_find_usecase(wcd_usecase* p_handle)
{
  if( wcd_find_element_inside_list(gp_usecase_list, p_handle) == NULL ) {
    return E_WCD_INVALID_PARAMETER_ERROR;
  }

  return E_WCD_SUCCESS;
}




/**************                CLOSING USECASES                ***************/

/**
 * \brief This function frees the client state as well as all internal members.
 * 
 */
wcd_result
wcd_clear_usecase (wcd_usecase *p_handle)
{
  wcd_result return_code = E_WCD_SUCCESS;

  do 
  {
    WCD_DEBUG_INFO_0("+WCD_93XX: <Clear usecase.>\n");
    if (p_handle == NULL) {
      return_code = E_WCD_INVALID_HANDLE;
      break;
    }

    //first clear the connections -- incase any were not freed
    if (p_handle->connection_list != NULL && p_handle->connection_list->list_size != 0)
    {
      return_code = E_WCD_CONNECTIONS_NOT_DISCONNECTED;
      break;
    }

    if (p_handle->primary_anc_elements != NULL && p_handle->primary_anc_elements->list_size != 0)
    {
      return_code = E_WCD_CONNECTIONS_NOT_DISCONNECTED;
      break;
    }

    if (p_handle->secondary_anc_elements != NULL && p_handle->secondary_anc_elements->list_size != 0)
    {
      return_code = E_WCD_CONNECTIONS_NOT_DISCONNECTED;
      break;
    }

	  if (p_handle->connection_list != NULL)
	  {
      WCD_FREE( p_handle->connection_list , WCD_MALLOC_START_TAG + 8);
      p_handle->connection_list = NULL;
	  }

	  if (p_handle->primary_anc_elements != NULL)
	  {
      WCD_FREE( p_handle->primary_anc_elements, WCD_MALLOC_START_TAG + 8 );
      p_handle->primary_anc_elements = NULL;
	  }

	  if (p_handle->secondary_anc_elements != NULL)
	  {
      WCD_FREE( p_handle->secondary_anc_elements, WCD_MALLOC_START_TAG + 8 );
      p_handle->secondary_anc_elements = NULL;
	  }

	  if (p_handle->anc_command_list != NULL)
	  {
      WCD_FREE( p_handle->anc_command_list, WCD_MALLOC_START_TAG + 6 );
	    p_handle->anc_command_list = NULL;
	  }

	  if (p_handle->command_list != NULL)
	  {
      WCD_FREE( p_handle->command_list, WCD_MALLOC_START_TAG + 6 );
	    p_handle->command_list = NULL;
	  }

      //free context pointer
	  if (p_handle->context_pointer != NULL && p_handle->context_pointer->busport_elements != NULL)
	  {
      WCD_FREE ( p_handle->context_pointer->busport_elements, WCD_MALLOC_START_TAG + 8 );
      p_handle->context_pointer->busport_elements = NULL;
	  }

	  if (p_handle->context_pointer != NULL)
	  {
      WCD_FREE ( p_handle->context_pointer, WCD_MALLOC_START_TAG + 3 );
      p_handle->context_pointer = NULL;
	  }

    //now the state it self
    WCD_FREE (p_handle, WCD_MALLOC_START_TAG + 1);

  } while(WCD_G_FALSE_BOOLEAN);

  WCD_DEBUG_INFO_1("-WCD_93XX: <Clear usecase. RC:%d.>\n", return_code);
  return return_code;
}


/******************************************************************************
 **************            [INTF] USECASE OPEN/CLOSE            ***************
 *****************************************************************************/

/** 
 * \brief This function opens the driver for excersizing a use case. This function must be
 * called prior to calling any IOCTLs using a handle.
 *
 * @note wcd_init must be called prior to calling wcd_open
 *
 */
wcd_result
wcd_open (void **p_handle)
{
  wcd_usecase *new_usecase;
  wcd_result return_code = E_WCD_SUCCESS;
  uint8 i;

  WCD_DEBUG_INFO_0("+WCD_93XX: <Open usecase.>\n");

  if (p_handle == NULL)
  {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <Usecase handle pointer is NULL.>\n");
    WCD_DEBUG_INFO_1("-WCD_93XX: <Open usecase. RC:%d.>\n", E_WCD_INVALID_HANDLE);
    return E_WCD_INVALID_HANDLE;
  }

  if (*p_handle != NULL) {
    WCD_DEBUG_ERROR_0("=WCD_93XX: <Usecase handle is not NULL.>\n");
    WCD_DEBUG_INFO_1("-WCD_93XX: <Open usecase. RC:%d.>\n", E_WCD_INVALID_HANDLE);
    return E_WCD_INVALID_HANDLE;
  }

  do
  {
    if (gp_usecase_list == NULL)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Global usecase list is NULL.>\n");
      return_code = E_WCD_UNINITIALIZED_DRIVER;
      break;
    }

    //allocate the client state
    new_usecase =
      (wcd_usecase *) WCD_MALLOC (sizeof (wcd_usecase), WCD_MALLOC_START_TAG + 1);

    if (new_usecase == NULL) {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Unable to allocate memory for usecase.>\n");
      return_code =  E_WCD_MEMORY_ALLOCATION_ERROR;
      break;
    }

    //Initialize the new client
    new_usecase->id = 1;
    new_usecase->enable_level = E_WCD_USECASE_DISABLED;
    
    new_usecase->current_command = 0;
    new_usecase->command_list_size = 0;
    new_usecase->command_list = NULL;

    new_usecase->anc_command_list_size = 0;
    new_usecase->anc_current_command = 0;
    new_usecase->anc_command_list = NULL;
    new_usecase->primary_anc_elements = NULL;
    new_usecase->secondary_anc_elements = NULL;
    new_usecase->adaptive_anc = WCD_FALSE;

    return_code = wcd_create_and_initialize_list ( (void**) &new_usecase->connection_list);
    if (return_code != E_WCD_SUCCESS) {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Unable to allocate connection list.>\n");
      WCD_FREE (new_usecase, WCD_MALLOC_START_TAG + 1);
      break;
    }
    new_usecase->connect_blocked = WCD_FALSE;

    //initialize context pointer
    new_usecase->context_pointer = NULL;
    new_usecase->context_pointer = (wcd_event_context_pointer*)WCD_MALLOC(sizeof(wcd_event_context_pointer), WCD_MALLOC_START_TAG + 3);
    if (new_usecase->context_pointer == NULL)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Unable to allocate memory for usecase context pointer.>\n");
      return_code =  E_WCD_MEMORY_ALLOCATION_ERROR;
      WCD_FREE (new_usecase, WCD_MALLOC_START_TAG + 1);
      break;
    }

    //initialize context pointer internal
    new_usecase->context_pointer->bit_width = 0;
    new_usecase->context_pointer->sample_rate = 0;
    return_code = wcd_create_and_initialize_list ( (void**)&new_usecase->context_pointer->busport_elements );
    if (return_code != E_WCD_SUCCESS) {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Unable to allocate context pointer list.>\n");
      WCD_FREE (new_usecase->context_pointer, WCD_MALLOC_START_TAG + 3);
      WCD_FREE (new_usecase, WCD_MALLOC_START_TAG + 1);
      break;
    }

    //initialize the shared channel variables
    new_usecase->slimbus_shared_channel_group_handle = NULL;
    for(i = 0; i < WCD_MAX_ADIE_CHANNELS; i++)
    {
      new_usecase->slimbus_shared_channel_handle[i] = NULL;
    }

    //initialize speaker protection shared channel variables
    new_usecase->spkr_prot_enabled = WCD_FALSE;

    //set the client id
    if (gp_usecase_list->tail != NULL) {
      new_usecase->id = ( (wcd_usecase*)gp_usecase_list->tail->data )->id + 1;
    }

    return_code = wcd_add_to_list (gp_usecase_list, new_usecase);

    *p_handle = new_usecase;

  } while(WCD_G_FALSE_BOOLEAN);

  WCD_DEBUG_INFO_1("-WCD_93XX: <Open usecase. RC:%d.>\n", return_code);
  return return_code;

}


/**
 * \brief This function closes a particular usecase by removing it from the global list
 * The handle is deallocated as well. The usecase must be disabled prior to attempting a close.
 *
 */
wcd_result wcd_close  ( void *p_handle )
{
  wcd_result return_code = E_WCD_SUCCESS;

  do
  {
    WCD_DEBUG_INFO_0("+WCD_93XX: <Close usecase.>\n");

    if (p_handle == NULL) {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Usecase handle is NULL.>\n");
      return_code = E_WCD_INVALID_HANDLE;
      break;
    }

    if (gp_usecase_list == NULL) {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Global usecase list is NULL.>\n");
      return_code = E_WCD_INITIALIZATION_ERROR;
      break;
    }

    //check if the handle is located on the list
    if( wcd_find_usecase(p_handle) != E_WCD_SUCCESS ) {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Cannot close an unknown handle.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
    }

    if ( ((wcd_usecase*)p_handle)->enable_level == E_WCD_USECASE_ENABLED )
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Cannot close an enabled usecase.>\n");
      return_code = E_WCD_USECASE_STILL_ENABLED;
      break;
    }

    //first remove the client state from the global client list
    return_code = wcd_remove_from_list (gp_usecase_list, p_handle);
    if (return_code != E_WCD_SUCCESS) {
      break;
    }

    //free the client state along with every structure inside
    return_code =  wcd_clear_usecase ((wcd_usecase *) p_handle);

  } while(WCD_G_FALSE_BOOLEAN);

  WCD_DEBUG_INFO_1("-WCD_93XX: <Close usecase. RC:%d.>\n", return_code);
  return return_code;
}

/******************************************************************************
 **************            [INTF] USECASE OPEN/CLOSE            ***************
 *****************************************************************************/

/**
 * \brief The mask in parameter_one that has the command type
 */
#define WCD_COMMAND_TYPE_MASK             0xF0000000

/**
 * \brief The command types.
 */
#define WCD_COMMAND_TYPE_TRIGGER          0x10000000
#define WCD_COMMAND_TYPE_END_OF_ENABLE    0x20000000
#define WCD_COMMAND_TYPE_END_OF_DISABLE   0x30000000
#define WCD_COMMAND_TYPE_DELAY            0x40000000

/*
 *\brief The mask of the parameter (time in MS) for delay.
 */
#define WCD_DELAY_COMMAND_PARAMETER_MASK  0xFFFFFFFF

/******************************************************************************
 **************            USECASE PROFILE MANAGEMENT           ***************
 *****************************************************************************/

#define WCD_LATEST_ACDB_PROFILE_VERSION   1

/**
 * \brief Verifies the ACDB profile. Checks for END OF ENABLE and END OF DISABLE.
 * Also verifies the version of the profile is compatible with the driver.
 */
wcd_result wcd_verify_acdb_profile(uint32 command_count, wcd_acdb_command* commands, uint32 version)
{
  uint32 iterator = 0;
  uint32 found_end_of_enable = WCD_FALSE;
  uint32 found_end_of_disable = WCD_FALSE;

  if (commands == NULL)
  {
    return E_WCD_INVALID_PARAMETER_ERROR;
  }

  for(iterator = 0; iterator < command_count; iterator++)
  {
    if ( (commands[iterator].parameter_one & WCD_COMMAND_TYPE_MASK) == WCD_COMMAND_TYPE_END_OF_ENABLE)
    {
      found_end_of_enable = WCD_TRUE;
    }

    if ( (commands[iterator].parameter_one & WCD_COMMAND_TYPE_MASK) == WCD_COMMAND_TYPE_END_OF_DISABLE)
    {
      found_end_of_disable = WCD_TRUE;
    }

    if (found_end_of_disable && found_end_of_enable)
    {
      break;
    }
  }

  if (found_end_of_disable && found_end_of_enable)
  {
    //verify the version of hte profile coming in is at the latest version or older.
    if (version <= WCD_LATEST_ACDB_PROFILE_VERSION)
    {
      return E_WCD_SUCCESS;
    }
    else
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Profile version is not supported on current driver.>\n");
      WCD_DEBUG_ERROR_1("=WCD_93XX: <Current driver version: %d.>\n", WCD_LATEST_ACDB_PROFILE_VERSION);
      return E_WCD_ACDB_PROFILE_VERSION_INCOMPATIBLE;
    }
  }

  WCD_DEBUG_ERROR_0("=WCD_93XX: <Could not find END OF ENABLE or END OF DISABLE in profile.>\n");
  return E_WCD_INVALID_PROFILE;
}


/******************************************************************************
 **************            USECASE PROFILE MANAGEMENT           ***************
 *****************************************************************************/

/******************************************************************************
 **************              USECASE ENABLE/DISABLE             ***************
 *****************************************************************************/

/**
 * \brief Enables the usecase by executing all the commands in the profile on all
 * connections (differential included).
 */
wcd_result wcd_enable_usecase(wcd_usecase* p_handle)
{
  wcd_result return_code = E_WCD_SUCCESS;  
#ifdef WCD_MBHC_ENABLE
  struct rm_status_notification rm_status_send;
  wcd_bool tx_connection_detected = WCD_FALSE;
  wcd_list_element* iterator = NULL;

  iterator = p_handle->connection_list->head;
  while (iterator != NULL) 
  {
    if (((wcd_connection_info*)iterator->data)->connection_direction == ADIE_CODEC_TX) 
    {
      tx_connection_detected = WCD_TRUE;
      break;
    }

    iterator = iterator->next;
  }

  // AUDIO_RESOURCE_MANAGER
  if (tx_connection_detected)
    rm_status_send.status = AUDIO_STATUS_RECORDING_ENABLE_START;
  else
    rm_status_send.status = AUDIO_STATUS_PLAYBACK_ENABLE_START;

  WCD_DEBUG_INFO_1("=WCD_93XX: <Calling AUDIO_RESOURCE_MANAGER with AUDIO_STATUS_ENABLE_START>, status=(%d) \n", rm_status_send.status);
  rm_audio_sr_ioctl(RM_WCD_MODULE, RM_CMD_STATUS_NOTIFICATION, &rm_status_send);

#endif
    
  WCD_DEBUG_INFO_0("+WCD_93XX: <Enable usecase.>\n");

  do
  {
    wcd_list_element* connection_node = NULL;
    wcd_connection_info* connection_info = NULL;
    wcd_acdb_command* command = NULL;
    wcd_bool end_of_enable_found = WCD_FALSE;
    uint32 current_channel_config = 0;
    uint32 channel_iterator = 0;

    return_code = wcd_pre_enable(p_handle);

    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Error in pre-enable.>\n");
      break;
    }

    return_code = wcd_initialize_codec();

    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Error initializing the codec.>\n");
      break;
    }

    connection_node = p_handle->connection_list->head;

    if( !connection_node )
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Cannot enable, no connections were defined.>\n");
      return_code = E_WCD_NO_CONNECTIONS_DEFINED;
      break;
    }

    if( p_handle->command_list_size == 0 )
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Cannot enable, profile not found.>\n");
      return_code = E_WCD_PROFILE_NOT_SET;
      break;
    }
    
    WCD_DEBUG_INFO_1("=WCD_93XX: <Reading command list, commands list size is %u.>\n", (unsigned int)p_handle->command_list_size);
    p_handle->current_command = 0;
    
    while(  p_handle->current_command < p_handle->command_list_size && 
            return_code == E_WCD_SUCCESS &&
            !end_of_enable_found )
    { //loop through commands
      
      command = &( p_handle->command_list[p_handle->current_command] );
           
      switch( command->parameter_one & WCD_COMMAND_TYPE_MASK )
      {
        case WCD_COMMAND_TYPE_END_OF_ENABLE:
            //end profile execution
          WCD_DEBUG_INFO_0("=WCD_93XX: <Found end of enable.>\n");
          end_of_enable_found = WCD_TRUE;
          break;
        case WCD_COMMAND_TYPE_END_OF_DISABLE:
          WCD_DEBUG_ERROR_0("=WCD_93XX: <Found an unexpected command on the profile.>\n");
          return_code = E_WCD_UNEXPECTED_COMMAND;
          break;
        case WCD_COMMAND_TYPE_DELAY:
          WCD_DEBUG_INFO_1("=WCD_93XX: <WCD_COMMAND_TYPE_DELAY, Sleeping for %u ms.>\n", (unsigned int)(command->parameter_two & WCD_DELAY_COMMAND_PARAMETER_MASK) );
          WCD_SLEEP(command->parameter_two & WCD_DELAY_COMMAND_PARAMETER_MASK);
          break;
        case WCD_COMMAND_TYPE_TRIGGER:
          connection_node = p_handle->connection_list->head;
          channel_iterator= 0;
          while( connection_node && return_code == E_WCD_SUCCESS )
          { 
            connection_info = (wcd_connection_info*)connection_node->data; 
            connection_node = connection_node->next;   

            if (connection_info->route == NULL)
            {
              WCD_DEBUG_ERROR_0("=WCD_93XX: <Route inside connection is NULL.>\n");
              return_code = E_WCD_INVALID_PARAMETER_ERROR;
              break;
            }

            channel_iterator++;

            if (current_channel_config != 0)
            {
              if (channel_iterator > current_channel_config)
              {
                break;
              }

              if (channel_iterator != current_channel_config)
              {
                continue;
              }
            }

            return_code = wcd_execute_command_on_elements(connection_info->route->elements, 
                                                          command, connection_info->route->route_table, p_handle->context_pointer);

            if (return_code != E_WCD_SUCCESS)
            {
              break;
            }

            if ( connection_info->route->differential_elements != NULL )
            {
              return_code = wcd_execute_command_on_elements(connection_info->route->differential_elements, 
                                                            command, connection_info->route->differential_table, p_handle->context_pointer);
            }

            if (return_code != E_WCD_SUCCESS)
            {
              break;
            }
          } // end connection loop
          break;
        case WCD_COMMAND_TYPE_BEGIN_CHAN_CONFIG:
          current_channel_config = (unsigned int)(command->parameter_one & WCD_BEGIN_CHAN_CONFIG_PARAMETER_MASK);
          WCD_DEBUG_INFO_1("=WCD_93XX: <Below commands only apply to Channel: %u.>\n", current_channel_config );
          break;
        case WCD_COMMAND_TYPE_END_CHAN_CONFIG:
          current_channel_config = 0;
          WCD_DEBUG_INFO_0("=WCD_93XX: <Ending channel configuration.>\n");
          break;
        default:
          return_code = E_WCD_INVALID_COMMAND_TYPE;
          break;
      } //end switch command
      
      if( return_code == E_WCD_SUCCESS )
      {
        p_handle->current_command++;
      }
    }//end command loop 

    //if we broke out and everything is good so far
    if( return_code == E_WCD_SUCCESS )
    {
      //if we found the end of enable we can mark the usecase+connections as enabled
      if (end_of_enable_found == WCD_TRUE)
      {
        p_handle->enable_level = E_WCD_USECASE_ENABLED;

        connection_node = p_handle->connection_list->head;
        while( connection_node )
        { 
          ((wcd_connection_info*)connection_node->data)->state = E_WCD_CONNECTION_ENABLED;
          connection_node = connection_node->next;
        }
      }
      else
      {
        return_code = E_WCD_MISSING_END_OF_STAGE_COMMAND;
      }
    }

  } while(WCD_G_FALSE_BOOLEAN);

#ifdef WCD_MBHC_ENABLE
  // AUDIO_RESOURCE_MANAGER
  if (tx_connection_detected)
    rm_status_send.status = AUDIO_STATUS_RECORDING_ENABLE_COMPLETE;
  else
    rm_status_send.status = AUDIO_STATUS_PLAYBACK_ENABLE_COMPLETE;

  WCD_DEBUG_INFO_1("=WCD_93XX: <Calling AUDIO_RESOURCE_MANAGER with AUDIO_STATUS_ENABLE_COMPLETE>, status=(%d) \n", rm_status_send.status);
  rm_audio_sr_ioctl(RM_WCD_MODULE, RM_CMD_STATUS_NOTIFICATION, &rm_status_send);
#endif

  //flush at the end of enable 
  //this is a preventive flush to ensure everything is executed.
  wcd_bus_flush(E_WCD_CODEC_MAIN);

  WCD_DEBUG_INFO_0("-WCD_93XX: <Enable usecase.>\n");

  return return_code;
}

/**
 * \brief Disables the usecase by executing all the remaining commands in the profile on all
 * connections (differential included).
 */
wcd_result wcd_disable_usecase(wcd_usecase* p_handle)
{
  wcd_result return_code = E_WCD_SUCCESS;  
#ifdef WCD_MBHC_ENABLE
  struct rm_status_notification rm_status_send;
  wcd_bool tx_connection_detected = WCD_FALSE;
  wcd_list_element* iterator = NULL;

  iterator = p_handle->connection_list->head;
  while (iterator != NULL) 
  {
    if (((wcd_connection_info*)iterator->data)->connection_direction == ADIE_CODEC_TX) 
    {
      tx_connection_detected = WCD_TRUE;
      break;
    }
    
    iterator = iterator->next;
  }
    
  // AUDIO_RESOURCE_MANAGER
  if (tx_connection_detected)
    rm_status_send.status = AUDIO_STATUS_RECORDING_DISABLE_START;
  else
    rm_status_send.status = AUDIO_STATUS_PLAYBACK_DISABLE_START;

  WCD_DEBUG_INFO_1("=WCD_93XX: <Calling AUDIO_RESOURCE_MANAGER with AUDIO_STATUS_DISABLE_START>, status=(%d) \n", rm_status_send.status);
  rm_audio_sr_ioctl(RM_WCD_MODULE, RM_CMD_STATUS_NOTIFICATION, &rm_status_send);

#endif

  WCD_DEBUG_INFO_0("+WCD_93XX: <Disable usecase> \n");

  do
  {
    wcd_list_element* connection_node = NULL;
    wcd_connection_info* connection_info = NULL;
    wcd_acdb_command* command = NULL;
    wcd_bool end_of_disable_found = WCD_FALSE;
    uint32 current_channel_config = 0;
    uint32 channel_iterator = 0;

    connection_node = p_handle->connection_list->head;

    if( !connection_node )
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Cannot disable, no connections were defined.>\n");
      return_code = E_WCD_NO_CONNECTIONS_DEFINED;
      break;
    }

    if( p_handle->command_list_size == 0 )
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Cannot disable, profile not found.>\n");
      return_code = E_WCD_PROFILE_NOT_SET;
      break;
    }
    
    WCD_DEBUG_INFO_1("=WCD_93XX: <Reading command list, commands list size is %u.>\n", (unsigned int)p_handle->command_list_size);
        
    //current command is already at the first command after END_OF_ENABLE

    while(  p_handle->current_command < p_handle->command_list_size && 
            return_code == E_WCD_SUCCESS &&
            !end_of_disable_found )
    { //loop through commands
      
      command = &( p_handle->command_list[p_handle->current_command] );
           
      switch( command->parameter_one & WCD_COMMAND_TYPE_MASK )
      {
        case WCD_COMMAND_TYPE_END_OF_ENABLE:
          WCD_DEBUG_ERROR_0("=WCD_93XX: <Found an unexpected command on the profile.>\n");
          return_code = E_WCD_UNEXPECTED_COMMAND;
          break;
        case WCD_COMMAND_TYPE_END_OF_DISABLE:
          WCD_DEBUG_INFO_0("=WCD_93XX: <Found end of disable.>\n");
          end_of_disable_found = WCD_TRUE;
          break;
        case WCD_COMMAND_TYPE_DELAY:
          WCD_DEBUG_INFO_1("=WCD_93XX: <Sleeping for %u ms.>\n", (unsigned int)(command->parameter_two & WCD_DELAY_COMMAND_PARAMETER_MASK) );
          WCD_SLEEP(command->parameter_two & WCD_DELAY_COMMAND_PARAMETER_MASK);
          break;
        case WCD_COMMAND_TYPE_TRIGGER:
          connection_node = p_handle->connection_list->head;
          channel_iterator= 0;
          while( connection_node && return_code == E_WCD_SUCCESS )
          { 
            connection_info = (wcd_connection_info*)connection_node->data;             
            connection_node = connection_node->next;        

            if (connection_info->route == NULL)
            {
              WCD_DEBUG_ERROR_0("=WCD_93XX: <Route inside connection is NULL.>\n");
              return_code = E_WCD_INVALID_PARAMETER_ERROR;
              break;
            }

            channel_iterator++;

            if (current_channel_config != 0)
            {
              if (channel_iterator > current_channel_config)
              {
                break;
              }

              if (channel_iterator != current_channel_config)
              {
                continue;
              }
            }

            return_code = wcd_execute_command_on_elements(connection_info->route->elements, 
                                                          command, connection_info->route->route_table, p_handle->context_pointer);

            if (return_code != E_WCD_SUCCESS)
            {
              break;
            }

            if ( connection_info->route->differential_elements != NULL )
            {
              return_code = wcd_execute_command_on_elements(connection_info->route->differential_elements, 
                                                            command, connection_info->route->differential_table, p_handle->context_pointer);
            }

            if (return_code != E_WCD_SUCCESS)
            {
              break;
            }      
          }
          break;
        case WCD_COMMAND_TYPE_BEGIN_CHAN_CONFIG:
          current_channel_config = (unsigned int)(command->parameter_one & WCD_BEGIN_CHAN_CONFIG_PARAMETER_MASK);
          WCD_DEBUG_INFO_1("=WCD_93XX: <Below commands only apply to Channel: %u.>\n", current_channel_config );
          break;
        case WCD_COMMAND_TYPE_END_CHAN_CONFIG:
          current_channel_config = 0;
          WCD_DEBUG_INFO_0("=WCD_93XX: <Ending channel configuration.>\n");
          break;
        default:
          return_code = E_WCD_INVALID_COMMAND_TYPE;
          break;
      } //end switch command
      
      if( return_code == E_WCD_SUCCESS )
      {
        p_handle->current_command++;
      }
    }//end command loop 

    //if we broke out and everything is good so far
    if( return_code == E_WCD_SUCCESS )
    {
      //if we found the end of disable we can mark the usecase+connections as disabled
      if ( end_of_disable_found == WCD_TRUE )
      {
        p_handle->enable_level = E_WCD_USECASE_DISABLED;

        connection_node = p_handle->connection_list->head;
        while( connection_node )
        { 
          ((wcd_connection_info*)connection_node->data)->state = E_WCD_CONNECTION_DISABLED;
          connection_node = connection_node->next;
        }
      }
      else
      {
        return_code = E_WCD_MISSING_END_OF_STAGE_COMMAND;
      }
    }

    //flush here, wcd_deinitialize_codec also deinitializes the bus manager
    wcd_bus_flush(E_WCD_CODEC_MAIN);

    return_code = wcd_deinitialize_codec();

    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Error deinitializing the codec.>\n");
      break;
    }

    return_code = wcd_post_disable(p_handle);

    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Error in post-disable.>\n");
      break;
    }

  } while(WCD_G_FALSE_BOOLEAN);

#ifdef WCD_MBHC_ENABLE
  // AUDIO_RESOURCE_MANAGER
  if (tx_connection_detected)
    rm_status_send.status = AUDIO_STATUS_RECORDING_DISABLE_COMPLETE;
  else
    rm_status_send.status = AUDIO_STATUS_PLAYBACK_DISABLE_COMPLETE;

  WCD_DEBUG_INFO_1("=WCD_93XX: <Calling AUDIO_RESOURCE_MANAGER with AUDIO_STATUS_DISABLE_COMPLETE>, status=(%d) \n", rm_status_send.status);
  rm_audio_sr_ioctl(RM_WCD_MODULE, RM_CMD_STATUS_NOTIFICATION, &rm_status_send);
#endif

  WCD_DEBUG_INFO_0("-WCD_93XX: <Disable usecase.>\n");

  return return_code;
}


/******************************************************************************
 **************              USECASE ENABLE/DISABLE             ***************
 *****************************************************************************/


/******************************************************************************
 **************                ANC ENABLE/DISABLE               ***************
 *****************************************************************************/

/**
 * \brief Enables the ANC input devices using the reserved elements + ANC command list
 * inside of the usecase
 */
wcd_result wcd_enable_anc_profile(wcd_usecase* usecase)
{
  wcd_result return_code = E_WCD_SUCCESS;
  wcd_bool end_of_enable_found = WCD_FALSE;
  wcd_acdb_command* command = NULL;
  uint32 current_channel_config = 0;

  do
  {
    WCD_DEBUG_INFO_0("+WCD_93XX: <Enable ANC on usecase.>\n");

    if (usecase == NULL || usecase->primary_anc_elements == NULL || usecase->secondary_anc_elements == NULL)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Usecase or ANC element list is NULL.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
    }

    if( usecase->anc_command_list_size == 0 )
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Cannot enable, ANC profile not found.>\n");
      return_code = E_WCD_PROFILE_NOT_SET;
      break;
    }

    WCD_DEBUG_INFO_1("=WCD_93XX: <Reading ANC command list, commands list size is %u.>\n", (unsigned int)usecase->anc_command_list_size);
    usecase->anc_current_command = 0;

    while(  usecase->anc_current_command < usecase->anc_command_list_size &&
            return_code == E_WCD_SUCCESS &&
            !end_of_enable_found )
    {//loop through commands
      command = &(usecase->anc_command_list[usecase->anc_current_command]);

      switch( command->parameter_one & WCD_COMMAND_TYPE_MASK )
      {
        case WCD_COMMAND_TYPE_END_OF_ENABLE:
          //end profile execution
          WCD_DEBUG_INFO_0("=WCD_93XX: <Found end of enable.>\n");
          end_of_enable_found = WCD_TRUE;
          break;
        case WCD_COMMAND_TYPE_END_OF_DISABLE:
          WCD_DEBUG_ERROR_0("=WCD_93XX: <Found an unexpected command on the ANC profile.>\n");
          return_code = E_WCD_UNEXPECTED_COMMAND;
          break;
        case WCD_COMMAND_TYPE_DELAY:
          WCD_DEBUG_INFO_1("=WCD_93XX: <Sleeping for %u ms.>\n", (unsigned int)(command->parameter_two & WCD_DELAY_COMMAND_PARAMETER_MASK) );
          WCD_SLEEP(command->parameter_two & WCD_DELAY_COMMAND_PARAMETER_MASK);
          break;
        case WCD_COMMAND_TYPE_TRIGGER:
          if (current_channel_config == 0 || current_channel_config == 1)
          {
            return_code = wcd_execute_command_on_elements(usecase->primary_anc_elements,
                                                          command, NULL, usecase->context_pointer);
            if (return_code != E_WCD_SUCCESS)
            {
              break;
            }
          }

          if (current_channel_config == 0 || current_channel_config == 2)
          {
            if ( usecase->secondary_anc_elements != NULL )
            {
              return_code = wcd_execute_command_on_elements(usecase->secondary_anc_elements,
                                                            command, NULL, usecase->context_pointer);
            }
          }

          break;
        case WCD_COMMAND_TYPE_BEGIN_CHAN_CONFIG:
          current_channel_config = (unsigned int)(command->parameter_one & WCD_BEGIN_CHAN_CONFIG_PARAMETER_MASK);
          WCD_DEBUG_INFO_1("=WCD_93XX: <Below commands only apply to Channel: %u.>\n", current_channel_config );
          break;
        case WCD_COMMAND_TYPE_END_CHAN_CONFIG:
          current_channel_config = 0;
          WCD_DEBUG_INFO_0("=WCD_93XX: <Ending channel configuration.>\n");
          break;
        default:
          return_code = E_WCD_INVALID_COMMAND_TYPE;
          break;
      } //end switch command

      if( return_code == E_WCD_SUCCESS )
      {
        usecase->anc_current_command++;
      }

    } //end loop through commands

  } while(WCD_G_FALSE_BOOLEAN);

  WCD_DEBUG_INFO_1("-WCD_93XX: <Enable ANC on usecase. RC:%d.>\n", return_code);

  return return_code;
}

/**
 * \brief Disables the ANC input devices using the reserved elements + ANC command list
 * inside of the usecase
 */
wcd_result wcd_disable_anc_profile(wcd_usecase* usecase)
{
  wcd_result return_code = E_WCD_SUCCESS;
  wcd_bool end_of_disable_found = WCD_FALSE;
  wcd_acdb_command* command = NULL;
  uint32 current_channel_config = 0;

  do
  {
    WCD_DEBUG_INFO_0("+WCD_93XX: <Disable ANC on usecase.>\n");

    if (usecase == NULL || usecase->primary_anc_elements == NULL || usecase->secondary_anc_elements == NULL)
    {
      WCD_DEBUG_INFO_0("=WCD_93XX: <Usecase or ANC element list is NULL.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
    }

    if( usecase->anc_command_list_size == 0 )
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Cannot disable, ANC profile not found.>\n");
      return_code = E_WCD_PROFILE_NOT_SET;
      break;
    }

    WCD_DEBUG_INFO_1("=WCD_93XX: <Reading ANC command list, commands list size is %u.>\n", (unsigned int)usecase->anc_command_list_size);
    
    //current command is already at the first command after END_OF_ENABLE

    while(  usecase->anc_current_command < usecase->anc_command_list_size &&
            return_code == E_WCD_SUCCESS &&
            !end_of_disable_found )
    {//loop through commands
      command = &(usecase->anc_command_list[usecase->anc_current_command]);

      switch( command->parameter_one & WCD_COMMAND_TYPE_MASK )
      {
        case WCD_COMMAND_TYPE_END_OF_ENABLE:
          WCD_DEBUG_ERROR_0("=WCD_93XX: <Found an unexpected command on the profile.>\n");
          return_code = E_WCD_UNEXPECTED_COMMAND;
          break;
        case WCD_COMMAND_TYPE_END_OF_DISABLE:
          //end profile execution
          WCD_DEBUG_INFO_0("=WCD_93XX: <Found end of disable.>\n");
          end_of_disable_found = WCD_TRUE;
          break;
        case WCD_COMMAND_TYPE_DELAY:
          WCD_DEBUG_INFO_1("=WCD_93XX: <Sleeping for %u ms.>\n", (unsigned int)(command->parameter_two & WCD_DELAY_COMMAND_PARAMETER_MASK) );
          WCD_SLEEP(command->parameter_two & WCD_DELAY_COMMAND_PARAMETER_MASK);
          break;
        case WCD_COMMAND_TYPE_TRIGGER:
          if (current_channel_config == 0 || current_channel_config == 1)
          {
            return_code = wcd_execute_command_on_elements(usecase->primary_anc_elements,
                                                          command, NULL, usecase->context_pointer);
            if (return_code != E_WCD_SUCCESS)
            {
              break;
            }
          }

          if (current_channel_config == 0 || current_channel_config == 2)
          {
            if ( usecase->secondary_anc_elements != NULL )
            {
              return_code = wcd_execute_command_on_elements(usecase->secondary_anc_elements,
                                                            command, NULL, usecase->context_pointer);
            }
          }

          break;
        case WCD_COMMAND_TYPE_BEGIN_CHAN_CONFIG:
          current_channel_config = (unsigned int)(command->parameter_one & WCD_BEGIN_CHAN_CONFIG_PARAMETER_MASK);
          WCD_DEBUG_INFO_1("=WCD_93XX: <Below commands only apply to Channel: %u.>\n", current_channel_config );
          break;
        case WCD_COMMAND_TYPE_END_CHAN_CONFIG:
          current_channel_config = 0;
          WCD_DEBUG_INFO_0("=WCD_93XX: <Ending channel configuration.>\n");
          break;
        default:
          return_code = E_WCD_INVALID_COMMAND_TYPE;
          break;
      } //end switch command

      if( return_code == E_WCD_SUCCESS )
      {
        usecase->anc_current_command++;
      }

    } //end loop through commands

  } while(WCD_G_FALSE_BOOLEAN);

  WCD_DEBUG_INFO_1("-WCD_93XX: <Disable ANC on usecase. RC:%d.>\n", return_code);

  return return_code;
}


/******************************************************************************
 **************                ANC ENABLE/DISABLE               ***************
 *****************************************************************************/


