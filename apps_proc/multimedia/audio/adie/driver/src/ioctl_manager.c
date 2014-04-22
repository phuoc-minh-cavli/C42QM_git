/**
@file ioctl_manager.c
@brief
Implementation and internal functions of WCD IOCTL Manager
*/
/*-------------------------------------------------------------------------------
     Copyright (c) 2011 Qualcomm Technologies Incorporated.
	 All rights reserved.
	 Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
                             Edit History
$Id: //components/rel/audio.tx/2.0/audio/adie/driver/src/ioctl_manager.c#3 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
10/09/13   AG      Removing recursive calls to wcd_ioctl
07/15/13   OA      removed banned usage for memcpy function
03/07/13   SSP     Driver Unification Update
03/04/13   KNM     Fixed SAL warnings.
03/01/13   KNM     Refactored bus manager i/f and replaced E_WCD_PGD with
                      E_WCD_CODEC_MAIN to suit both I2C and SB PGD i/f and also
                      modified get codec infor to provide supported sample rates
                      based on codec tier.
12/15/10   SK      Initial revision
===============================================================================*/

#include "usecase_manager.h"
#include "connection_manager.h"
#include "wcd_utility.h"
#include "bus_manager.h"
#include "init_manager.h"
#include "shared_channel_manager.h"
#include "target_defs.h"
#include "anc_manager.h"
#include "wcd_gen_id_defines.h"
#ifdef WCD_MBHC_ENABLE
  #include "wcd_interrupt_handler.h"
  #include "mbhc.h"
#endif

#ifdef SPL_LOG_SUPPORT
	#include "ioctl_manager.tmh"
#endif

uint32  spkr_prot_ref_count = 0;

//#ifdef WCD_MBHC_ENABLE
  extern void* wcdIoctlMutex;
//#endif

wcd_result ioctl_disconnect_connections(wcd_usecase* p_handle);

wcd_result
ioctl_connect_source_to_sink(wcd_usecase* p_handle,
                             wcd_ioctl_cmd_connect_source_to_sink* p_command)
{
  wcd_result return_code = E_WCD_SUCCESS;

  //create a new connection info
  wcd_connection_info *new_connection_info;

  WCD_DEBUG_INFO_0("+WCD_93XX: <IOCTL: Connect source to sink.>\n");

  do
  {
    if(p_handle->connect_blocked)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Connections cannot be created in this usecase.>\n");
      return_code = E_WCD_CONNECTIONS_BLOCKED;
      break;
    }

    if (p_handle->enable_level == E_WCD_USECASE_ENABLED)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Usecase enabled. More connections cannot be added now.>\n");
      return_code = E_WCD_USECASE_STILL_ENABLED;
      break;
    }

    //initialize the connection info
    new_connection_info =
      (wcd_connection_info *)
      WCD_MALLOC (sizeof (wcd_connection_info), WCD_MALLOC_START_TAG + 2);

    if (new_connection_info == NULL) {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Unable to create new conection.>\n");
      return_code = E_WCD_MEMORY_ALLOCATION_ERROR;
      break;
    }

    new_connection_info->id = 1;
    new_connection_info->route = NULL;
    new_connection_info->connection_direction = ADIE_CODEC_RX;

    // assign a unique id to the connection for the client state
    if (p_handle->connection_list->tail != NULL) {
      new_connection_info->id =
        ((wcd_connection_info *) (p_handle->connection_list->tail->
                                         data))->id + 1;
    }

    // populate the connection id in the ioctl structure
    p_command->connection_id = new_connection_info->id;

    new_connection_info->state = E_WCD_CONNECTION_START;

    //call connect
    return_code = wcd_connect(p_handle, new_connection_info,
      &(p_command->source), &(p_command->sink), p_command->connection_type);
    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Unable to connect.>\n");
      wcd_clear_connection(new_connection_info);
      break;
    }

    //if everything is good now,
    //add connection_info to the connection list in the client state
    return_code =
      wcd_add_to_list (p_handle->connection_list, (void*)new_connection_info);
    if (return_code != E_WCD_SUCCESS) {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Unable to add connection to usecase.>\n");
      wcd_clear_connection(new_connection_info);
      break;
    }

  } while(WCD_G_FALSE_BOOLEAN);

  WCD_DEBUG_INFO_1("-WCD_93XX: <IOCTL: Connect source to sink. RC:%d.>\n", return_code);
  return return_code;
}

wcd_result
ioctl_connect_sources_to_sinks(wcd_usecase* p_handle,
                             wcd_ioctl_cmd_connect_sources_to_sinks* p_command)
{
  wcd_result return_code = E_WCD_SUCCESS;
  uint32 iterator;
  wcd_ioctl_cmd_connect_source_to_sink connect_payload;

  WCD_DEBUG_INFO_0("+WCD_93XX: <IOCTL: Connect sources to sinks.>\n");

  do
  {
    if(p_handle->connect_blocked)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Connections cannot be created in this usecase.>\n");
      return_code = E_WCD_CONNECTIONS_BLOCKED;
      break;
    }

    if (p_handle->enable_level == E_WCD_USECASE_ENABLED)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Usecase enabled. More connections cannot be added now.>\n");
      return_code = E_WCD_USECASE_STILL_ENABLED;
      break;
    }

    //create
    for(iterator = 0; iterator < p_command->number_of_connections; iterator++)
    {
      connect_payload.source = p_command->sources[iterator];
      connect_payload.sink = p_command->sinks[iterator];
      connect_payload.connection_type = p_command->connection_types[iterator];

      return_code = ioctl_connect_source_to_sink((wcd_usecase*)p_handle, (wcd_ioctl_cmd_connect_source_to_sink*)&connect_payload);

      if (return_code != E_WCD_SUCCESS)
      {
        break;
      }
    }

    if (return_code != E_WCD_SUCCESS)
    {
      return_code = ioctl_disconnect_connections((wcd_usecase*)p_handle);
      break;
    }

    //prevent any more connections from being created. have to do DISCONNECT_CONNECTIONS
    p_handle->connect_blocked = WCD_TRUE;

  } while(WCD_G_FALSE_BOOLEAN);

  WCD_DEBUG_INFO_1("-WCD_93XX: <IOCTL: Connect sources to sinks. RC:%d.>\n", return_code);
  return return_code;
}

wcd_result
ioctl_disconnect_connection(wcd_usecase* p_handle,
                            wcd_ioctl_cmd_disconnect_connection* p_command)
{
  wcd_result return_code = E_WCD_SUCCESS;
  wcd_connection_info *connection_info = NULL;
  wcd_list_element *current;

  WCD_DEBUG_INFO_0("+WCD_93XX: <IOCTL: Disconnect connection.>\n");

  do
  {
    if(p_handle->connect_blocked)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Connections cannot be disconnected individually in this usecase.>\n");
      return_code = E_WCD_CONNECTIONS_BLOCKED;
      break;
    }

    if (p_handle->enable_level == E_WCD_USECASE_ENABLED)
    {
      WCD_DEBUG_INFO_0("=WCD_93XX: <Usecase enabled. Reset will be required.>\n");
      wcd_reset_required();
    }

    current = p_handle->connection_list->head;

    while (current != NULL)
    {
      if ( ((wcd_connection_info *) current->data)->id == p_command->connection_id )
      {
        connection_info = current->data;
        break;
      }
      current = current->next;
    }

    //connection id could not be found in the client state..error out
    if (connection_info == NULL) {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Could not find connection in usecase.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
    }

    return_code = wcd_remove_from_list(p_handle->connection_list, connection_info);
    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Could not remove connection from connection list.>\n");
      break;
    }

    return_code = wcd_disconnect(p_handle, connection_info);
  } while(WCD_G_FALSE_BOOLEAN);

  WCD_DEBUG_INFO_1("-WCD_93XX: <IOCTL: Disconnect connection. RC:%d.>\n", return_code);
  return return_code;
}

wcd_result
ioctl_disconnect_connections(wcd_usecase* p_handle)
{
  wcd_result return_code = E_WCD_SUCCESS;
  wcd_ioctl_cmd_disconnect_connection disconnect_payload;

  WCD_DEBUG_INFO_0("+WCD_93XX: <IOCTL: Disconnect connections.>\n");

  do
  {
    if (p_handle->enable_level == E_WCD_USECASE_ENABLED)
    {
      WCD_DEBUG_INFO_0("=WCD_93XX: <Usecase enabled. Reset will be required.>\n");
      wcd_reset_required();
    }

    //unblock connecting/disconnecting connections
    p_handle->connect_blocked = WCD_FALSE;

    while(p_handle->connection_list->head != NULL)
    {
      disconnect_payload.connection_id = ((wcd_connection_info*)p_handle->connection_list->head->data)->id;
      return_code = ioctl_disconnect_connection((wcd_usecase*)p_handle, (wcd_ioctl_cmd_disconnect_connection*)&disconnect_payload);

      if (return_code != E_WCD_SUCCESS)
      {
        break;
      }
    }

  } while(WCD_G_FALSE_BOOLEAN);

  WCD_DEBUG_INFO_1("-WCD_93XX: <IOCTL: Disconnect connections. RC:%d.>\n", return_code);
  return return_code;
}

wcd_result
ioctl_enable_usecase(wcd_usecase* p_handle, wcd_ioctl_cmd_enable_usecase* p_command)
{
  wcd_result return_code = E_WCD_SUCCESS;

  WCD_DEBUG_ERROR_0("+WCD_93XX: <IOCTL: Enable usecase.>\n");

  do
  {
    if (p_handle->enable_level == E_WCD_USECASE_ENABLED)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Usecase already enabled.>\n");
      return_code = E_WCD_USECASE_STILL_ENABLED;
      break;
    }

    if (p_command == NULL)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <No sample rate or bitwidth passed.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
    }

    if (p_command->bit_width != 12 && p_command->bit_width != 16 &&
        p_command->bit_width != 20 && p_command->bit_width != 24)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Invalid bit width provided.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
    }

    if (p_command->sample_rate != 8000 && p_command->sample_rate != 16000 &&
        p_command->sample_rate != 32000 && p_command->sample_rate != 48000 &&
        p_command->sample_rate != 96000 && p_command->sample_rate != 192000)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Invalid sample rate provided.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
    }

    if (p_handle->context_pointer == NULL)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Usecase context poitner is NULL.>\n");
      return_code = E_WCD_INVALID_HANDLE;
      break;
    }

    return_code = wcd_activate_bus();
    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Could not activate bus before enabling usecase.>\n");
      break;
    }

    p_handle->context_pointer->bit_width = p_command->bit_width;
    p_handle->context_pointer->sample_rate = p_command->sample_rate;

    return_code = wcd_enable_usecase(p_handle);

    if (wcd_init_options.bustype == E_WCD_I2C)
    {
      return_code = wcd_deactivate_bus();
    }

  } while(WCD_G_FALSE_BOOLEAN);

  WCD_DEBUG_ERROR_1("-WCD_93XX: <IOCTL: Enable usecase. RC:%d.>\n", return_code);
  return return_code;
}

wcd_result
ioctl_disable_usecase(wcd_usecase* p_handle)
{
  wcd_result return_code = E_WCD_SUCCESS;
  wcd_usecase *sidetone_rx_usecase = NULL;  
  wcd_codec_info codec_info;

  WCD_DEBUG_ERROR_0("+WCD_93XX: <IOCTL: Disable usecase.>\n");

  do
  {
    if (p_handle->enable_level == E_WCD_USECASE_DISABLED)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Usecase already disabled.>\n");
      return_code = E_WCD_USECASE_STILL_DISABLED;
      break;
    }

    if (wcd_init_options.bustype == E_WCD_I2C)
    {
      return_code = wcd_activate_bus();
    }

    //check if this usecase is part of sidetone
    sidetone_rx_usecase = wcd_check_for_sidetone_usecase(p_handle);

    if (sidetone_rx_usecase != NULL)
    {
      return_code = wcd_ioctl(sidetone_rx_usecase, WCD_IOCTL_DISABLE_SIDETONE, NULL);

      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error disabling sidetone.>\n");
        break;
      }
    }

    //check if speaker protection is enabled on this usecase, if so, disable it.
    if (p_handle->spkr_prot_enabled)
    {
      return_code = wcd_ioctl(p_handle, WCD_IOCTL_DISABLE_FB_SPKR_PROT, NULL);

      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error disabling speaker protection.>\n");
        break;
      }
    }

    //check if anc is enabled on this usecase, if so, disable it
    if (p_handle->anc_command_list != NULL)
    {
      return_code = wcd_ioctl(p_handle, WCD_IOCTL_DISABLE_ANC, NULL);

      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error disabling speaker protection.>\n");
        break;
      }
    }

	/* Helicon does not support slimbus interface. So, skipping
	   shared channel configure/open/close for helicon codec */
    return_code = wcd_get_codec_info(&codec_info);
	if (codec_info.codec != E_WCD_HELICON)
    {
      //disable slimbus interface
      WCD_DEBUG_INFO_0("=WCD_93XX: <Disabling SLIMbus interface.>\n");
      return_code = wcd_close_shared_channel(p_handle);
	}

    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Error disabling SLIMbus interface.>\n");
      break;
    }

    WCD_DEBUG_INFO_0("=WCD_93XX: <Disabling usecase.>\n");
    return_code = wcd_disable_usecase(p_handle);

    WCD_DEBUG_INFO_0("=WCD_93XX: <Deactivating bus.>\n");
    return_code = wcd_deactivate_bus();

  } while(WCD_G_FALSE_BOOLEAN);

  WCD_DEBUG_ERROR_1("-WCD_93XX: <IOCTL: Disable usecase. RC:%d.>\n", return_code);
  return return_code;
}

wcd_result
ioctl_set_profile(wcd_usecase* p_handle, wcd_ioctl_cmd_set_profile* p_command)
{
  wcd_result return_code = E_WCD_SUCCESS;

  WCD_DEBUG_INFO_0("+WCD_93XX: <IOCTL: Set Profile.>\n");

  do
  {
    if (p_handle->command_list_size != 0 || p_handle->current_command != 0 ||
      p_handle->command_list != NULL)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Usecase already has a profile.>\n");
      return_code = E_WCD_PROFILE_ALREADY_SET;
      break;
    }

    //verify there is a end of enable and disable in the commands
    return_code = wcd_verify_acdb_profile(p_command->command_count, p_command->acdb_commands, p_command->version);
    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Invalid profile. Missing END_OF_ENABLE or END_OF_DISABLE.>\n");
      break;
    }

    //memcopy the profile and we are done
    p_handle->command_list_size = p_command->command_count;
    p_handle->command_list = (wcd_acdb_command*)WCD_MALLOC(p_command->command_count * sizeof(wcd_acdb_command), WCD_MALLOC_START_TAG + 6);

    if (p_handle->command_list == NULL)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Error mallocing space for the profile in usecase.>\n");
      return_code = E_WCD_OS_CALL_ERROR;
      break;
    }

    WCD_MEMCOPY(p_command->acdb_commands, p_command->command_count * sizeof(wcd_acdb_command), p_handle->command_list,
      p_command->command_count * sizeof(wcd_acdb_command));

  } while(WCD_G_FALSE_BOOLEAN);

  WCD_DEBUG_INFO_1("-WCD_93XX: <IOCTL: Set Profile. RC:%d.>\n", return_code);
  return return_code;
}

wcd_result
ioctl_get_slimbus_logical_address(wcd_ioctl_cmd_slimbus_logical_address* p_command)
{
  WCD_DEBUG_INFO_0("+WCD_93XX: <IOCTL: Get SLIMbus logical address.>\n");
  return wcd_bus_get_logical_address(&(p_command->pgd_logical_addr), &(p_command->intf_logical_addr));
}

wcd_result
ioctl_register_write(wcd_ioctl_cmd_register_access* p_command)
{
  wcd_result return_code = E_WCD_SUCCESS;

  return_code = wcd_activate_bus();
  if (return_code != E_WCD_SUCCESS)
  {
    return return_code;
  }

  return_code = wcd_register_write(p_command->ctrl_bus_intf, p_command->reg, p_command->mask, p_command->shift, p_command->value);
  wcd_bus_flush(p_command->ctrl_bus_intf);

  wcd_deactivate_bus();

  return return_code;
}

wcd_result
ioctl_register_read(wcd_ioctl_cmd_register_access* p_command)
{
  wcd_result return_code = E_WCD_SUCCESS;

  return_code = wcd_activate_bus();
  if (return_code != E_WCD_SUCCESS)
  {
    return return_code;
  }

  return_code = wcd_register_read(p_command->ctrl_bus_intf, p_command->reg, p_command->mask, p_command->shift, &(p_command->value));

  wcd_deactivate_bus();

  return return_code;
}

wcd_result
ioctl_enable_slimbus_intf(wcd_usecase* p_handle, wcd_ioctl_cmd_slimbus_intf_config* p_command)
{
  wcd_codec_info codec_info;
  /* Helicon does not support slimbus interface. So, skipping
   shared channel configure/open/close for helicon codec */
  (void)wcd_get_codec_info(&codec_info);
  if (codec_info.codec == E_WCD_HELICON)
  {
    return E_WCD_SUCCESS;
  }
  WCD_DEBUG_INFO_0("=WCD_93XX: <IOCTL: Enable Slimbus INTF.>\n");

  return wcd_configure_shared_channel(p_handle, p_command);
}

wcd_result
ioctl_register_dump(void)
{
  return wcd_dump_registers();
}

wcd_result
ioctl_enable_anc(wcd_usecase* p_handle, wcd_ioctl_cmd_enable_anc* p_command)
{
  wcd_result return_code = E_WCD_SUCCESS;
  
  do
  {
    WCD_DEBUG_INFO_0("+WCD_93XX: <IOCTL: Enable ANC.>\n");

    return_code = wcd_activate_bus();
    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Could not activate bus before enabling ANC.>\n");
      break;
    }

    //verify profile first
    if (p_handle->anc_command_list_size != 0 || p_handle->anc_current_command != 0 ||
      p_handle->anc_command_list != NULL)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Usecase already has an ANC profile.>\n");
      return_code = E_WCD_ANC_ALREADY_ENABLED;
      break;
    }

    //verify there is a end of enable and disable in the commands
    return_code = wcd_verify_acdb_profile(p_command->command_count, p_command->acdb_commands, p_command->version);
    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Invalid ANC profile. Missing END_OF_ENABLE or END_OF_DISABLE.>\n");
      break;
    }

    //memcopy the profile and we are done
    p_handle->anc_command_list_size = p_command->command_count;
    p_handle->anc_command_list = (wcd_acdb_command*)WCD_MALLOC(p_command->command_count * sizeof(wcd_acdb_command), WCD_MALLOC_START_TAG + 6);

    if (p_handle->anc_command_list == NULL)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Error mallocing space for the ANC profile in usecase.>\n");
      return_code = E_WCD_OS_CALL_ERROR;
      break;
    }

    WCD_MEMCOPY(p_command->acdb_commands, p_command->command_count * sizeof(wcd_acdb_command), p_handle->anc_command_list,
      p_command->command_count * sizeof(wcd_acdb_command));


    //call ANC manager's enable ANC which will reserve elements for the ANC connection (micbias + mic)
    //and also enable the ANC profile and apply the ANC configuration
    return_code = wcd_enable_anc(p_handle, p_command->anc_configuration);

    if (return_code != E_WCD_SUCCESS)
    {
      //fre ANC profile
      WCD_FREE( p_handle->anc_command_list, WCD_MALLOC_START_TAG + 6 );

      p_handle->anc_command_list = NULL;
    }

    wcd_deactivate_bus();
  } while(WCD_G_FALSE_BOOLEAN);

  WCD_DEBUG_INFO_1("-WCD_93XX: <IOCTL: Enable ANC. RC:%d.>\n", return_code);

  return return_code;
}

wcd_result
ioctl_disable_anc(wcd_usecase* p_handle)
{
  wcd_result return_code = E_WCD_SUCCESS;

  do
  {
    WCD_DEBUG_INFO_0("+WCD_93XX: <IOCTL: Disable ANC.>\n");

    return_code = wcd_activate_bus();
    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Could not activate bus before disabling ANC.>\n");
      break;
    }

    //verify profile first
    if (p_handle->anc_command_list_size == 0 || p_handle->anc_command_list == NULL)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Usecase does not have ANC profile.>\n");
      return_code = E_WCD_INVALID_ANC_CONFIGURATION;
      break;
    }

    //call ANC manager's disable ANC which will disable the microphones/micbias, reset the ANC block and unreserve any elements
    return_code = wcd_disable_anc(p_handle);

    wcd_deactivate_bus();

  } while(WCD_G_FALSE_BOOLEAN);

  WCD_DEBUG_INFO_1("-WCD_93XX: <IOCTL: Enable ANC. RC:%d.>\n", return_code);

  return return_code;
}

wcd_result
ioctl_get_codec_info(wcd_ioctl_cmd_codec_info* p_command)
{
  wcd_result return_code = E_WCD_SUCCESS;
  wcd_codec_info codec_info;

  do
  {
    WCD_DEBUG_INFO_0("+WCD_93XX: <IOCTL: Get codec info.>\n");

    if (p_command == NULL)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <IOCTL command is NULL.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
    }

    return_code = wcd_get_codec_info(&codec_info);
    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Error getting codec info.>\n");
      break;
    }

    p_command->codec = codec_info.codec;
    p_command->version = codec_info.version;

    return_code = wcd_get_max_rx_ports(&(p_command->max_rx_ports));
    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Error getting RX port count.>\n");
      break;
    }

    return_code = wcd_get_max_tx_ports(&(p_command->max_tx_ports));
    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Error getting TX port count.>\n");
      break;
    }

    p_command->supported_bit_widths = WCD_BIT_WIDTH_12_BIT |
                                      WCD_BIT_WIDTH_16_BIT |
                                      WCD_BIT_WIDTH_20_BIT |
                                      WCD_BIT_WIDTH_24_BIT;

    p_command->supported_sample_rates = WCD_SAMPLE_RATE_8_KHZ |
                                        WCD_SAMPLE_RATE_16_KHZ |
                                        WCD_SAMPLE_RATE_32_KHZ |
                                        WCD_SAMPLE_RATE_48_KHZ;

    switch (codec_info.codec)
    {
	  case E_WCD_WCD9302:  /*fall through*/
	  case E_WCD_HELICON:
	  break;
	  default:
	    p_command->supported_sample_rates |= WCD_SAMPLE_RATE_96_KHZ |
                                             WCD_SAMPLE_RATE_192_KHZ;
	  break;

	}
  } while(WCD_G_FALSE_BOOLEAN);

  WCD_DEBUG_INFO_1("-WCD_93XX: <IOCTL: Get codec info. RC:%d>\n", return_code);

  return return_code;
}

wcd_result
ioctl_enable_sidetone(wcd_usecase* p_handle, wcd_ioctl_cmd_enable_sidetone* p_command)
{
  wcd_result return_code = E_WCD_SUCCESS;
  wcd_usecase* tx_usecase = NULL;
  wcd_connection_info* primary_tx_connection = NULL;
  wcd_list_element* iterator = NULL;
  wcd_list_element* iterator_two = NULL;
  wcd_element* tx_path_element = NULL;

  do
  {
    WCD_DEBUG_INFO_0("+WCD_93XX: <IOCTL: Enable Sidetone.>\n");

    if (p_command == NULL || p_command->tx_usecase == NULL || p_command->iir_configuration == NULL)
    {
      WCD_DEBUG_INFO_0("=WCD_93XX: <Invalid parameter.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
    }

    //find tx decimator in first connection in tx usecase
    tx_usecase = (wcd_usecase*)p_command->tx_usecase;

    if (tx_usecase->connection_list == NULL || tx_usecase->connection_list->head == NULL)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <TX usecase does not have a connection inside.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
    }

    primary_tx_connection = (wcd_connection_info*)tx_usecase->connection_list->head->data;
    if (primary_tx_connection == NULL)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Primary TX connection not set.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
    }

    iterator = primary_tx_connection->route->elements->head;

    while(iterator != NULL)
    {
      if (  ((wcd_element*)iterator->data)->type == WCD_TX_PATH_TYPE_ID )
      {
        tx_path_element = (wcd_element*)iterator->data;
      }

      iterator = iterator->next;
    }

    if (tx_path_element == NULL)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Could not find TX Path in primary TX connection.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
    }

    //first set the sidetone IIR
    return_code = wcd_set_sidetone_iir(p_handle, (wcd_usecase*)p_command->tx_usecase);
    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Error setting sidetone IIR.>\n");
      break;
    }

    //enable sidetone now
    return_code = wcd_enable_sidetone(p_handle, tx_path_element);
    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Error enabling IIR filter for sidetone.>\n");
      break;
    }

    //set the IIR filter coefficients + gain
    return_code = wcd_configure_sidetone_iir(p_handle, p_command->iir_configuration);
    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Error setting sidetone IIR configuration.>\n");
      break;
    }

    //connect the IIR filter to the RX paths
    iterator = p_handle->connection_list->head;

    while(iterator != NULL)
    {
      iterator_two = ((wcd_connection_info*)(iterator->data))->route->elements->head;

      while(iterator_two != NULL)
      {
        if ( ((wcd_element*)iterator_two->data)->type == WCD_RX_PATH_TYPE_ID )
        {
          return_code = wcd_connect_sidetone((wcd_element*)iterator_two->data);
          break;
        }

        iterator_two = iterator_two->next;
      }

      iterator = iterator->next;
    }    

  } while (WCD_G_FALSE_BOOLEAN);

  WCD_DEBUG_INFO_1("-WCD_93XX: <IOCTL: Enable Sidetone. RC:%d.>\n", return_code);

  return return_code;
}

wcd_result
ioctl_disable_sidetone(wcd_usecase* p_handle)
{
  wcd_result return_code = E_WCD_SUCCESS;

  wcd_list_element* iterator = NULL;
  wcd_list_element* iterator_two = NULL;

  do
  {
    WCD_DEBUG_INFO_0("+WCD_93XX: <IOCTL: Disable Sidetone.>\n");

    //disable sidetone now
    return_code = wcd_disable_sidetone(p_handle);
    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Error disabling IIR filter for sidetone.>\n");
      break;
    }

    //disconnect the IIR filter to the RX paths
    iterator = p_handle->connection_list->head;

    while(iterator != NULL)
    {
      iterator_two = ((wcd_connection_info*)(iterator->data))->route->elements->head;

      while(iterator_two != NULL)
      {
        if ( ((wcd_element*)iterator->data)->type == WCD_RX_PATH_TYPE_ID )
        {
          return_code = wcd_disconnect_sidetone((wcd_element*)iterator_two->data);
          break;
        }

        iterator_two = iterator_two->next;
      }

      iterator = iterator->next;
    }    

  } while (WCD_G_FALSE_BOOLEAN);

  WCD_DEBUG_INFO_1("-WCD_93XX: <IOCTL: Disable Sidetone. RC:%d.>\n", return_code);

  return return_code;
}

wcd_result
ioctl_enable_fb_spkr_prot(wcd_usecase* p_handle, wcd_ioctl_cmd_enable_fb_spkr_prot* p_command)
{
  wcd_result return_code = E_WCD_SUCCESS;

  do
  {
    WCD_DEBUG_INFO_0("+WCD_93XX: <IOCTL: Enable Feedback Speaker Protection.>\n");

    if (p_handle->spkr_prot_enabled)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Speaker protection already enabled for this usecase.>\n");
      break;
    }

    if (spkr_prot_ref_count == 0)
    {
      return_code = wcd_enable_speaker_protection();

      if (return_code != E_WCD_SUCCESS)
      {
        break;
      }

      return_code = wcd_configure_spkr_prot_shared_channel(p_command->shared_channel_num_v,
                                                           p_command->shared_channel_num_i);
    }

    if (return_code == E_WCD_SUCCESS)
    {
      spkr_prot_ref_count++;
      p_handle->spkr_prot_enabled = WCD_TRUE;
    }


  } while (WCD_G_FALSE_BOOLEAN);

  WCD_DEBUG_INFO_1("-WCD_93XX: <IOCTL: Enable Feedback Speaker Protection. RC:%d.>\n", return_code);

  return return_code;
}

wcd_result
ioctl_disable_fb_spkr_prot(wcd_usecase* p_handle)
{
  wcd_result return_code = E_WCD_SUCCESS;

  do
  {
    WCD_DEBUG_INFO_0("+WCD_93XX: <IOCTL: Disable Feedback Speaker Protection.>\n");

    if (!p_handle->spkr_prot_enabled)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Speaker protection not enabled for this usecase.>\n");
      break;
    }

    spkr_prot_ref_count--;

    if (spkr_prot_ref_count == 0)
    {
      return_code = wcd_close_spkr_prot_shared_channel();

      if (return_code != E_WCD_SUCCESS)
      {
        break;
      }

      return_code = wcd_disable_speaker_protection();
    }

    if (return_code == E_WCD_SUCCESS)
    {
      p_handle->spkr_prot_enabled = WCD_FALSE;
    }

  } while (WCD_G_FALSE_BOOLEAN);

  WCD_DEBUG_INFO_1("-WCD_93XX: <IOCTL: Disable Feedback Speaker Protection. RC:%d.>\n", return_code);

  return return_code;
}

wcd_result
ioctl_configure_mad_common( wcd_ioctl_cmd_configure_mad* p_command )
{
  wcd_result return_code = E_WCD_SUCCESS;
  WCD_DEBUG_ERROR_0("+WCD_93XX: <ioctl_configure_mad_common, IOCTL called.>\n");

  do
  {
    if( mad_global_info )
    {
      return_code = E_WCD_INITIALIZATION_ERROR;
      WCD_DEBUG_ERROR_1("=WCD_93XX: <ioctl_configure_mad_common, Cannot reconfigure MAD while it is enabled, RC:%d.>\n", return_code);
      break;
    }

    if( p_command == NULL
      || (int32)p_command->micbias.mbias < E_WCD_MAD_MBIAS_FIRST
      || (int32)p_command->micbias.mbias > E_WCD_MAD_MBIAS_LAST
      || (int32)p_command->microphone_info.microphone < E_WCD_MAD_ADC_FIRST
      || (int32)p_command->microphone_info.microphone > E_WCD_MAD_DMIC_LAST
      || ((int32)p_command->microphone_info.microphone > E_WCD_MAD_ADC_LAST
          && (int32)p_command->microphone_info.microphone < E_WCD_MAD_DMIC_FIRST) )
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <ioctl_configure_mad_common, Invalid parameter.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
    }

    // Configure MAD if no usecase is already enabled
    return_code = wcd_configure_mad_common( p_command );

    if( return_code == E_WCD_SUCCESS)
    {
      // Update common information, make sure all previous information is removed
      if( !mad_global_info )
      {
        mad_global_info = (wcd_configure_mad*) WCD_MALLOC(sizeof(wcd_configure_mad), WCD_MALLOC_START_TAG + 7);
      }
      if( mad_global_info )
      {
        WCD_MEMCOPY( (void*)p_command, sizeof(wcd_configure_mad), (void*)mad_global_info, sizeof(wcd_configure_mad) );
      }
    }

  } while(WCD_G_FALSE_BOOLEAN);

  WCD_DEBUG_ERROR_1("-WCD_93XX: <ioctl_configure_mad_common, IOCTL returned RC:%d.>\n", return_code);
  return return_code;
}

wcd_result
ioctl_disable_mad_common( void )
{
  wcd_result return_code = E_WCD_SUCCESS;
  WCD_DEBUG_ERROR_0("+WCD_93XX: <ioctl_disable_mad_common, IOCTL called.>\n");

  if( mad_global_info )
  {
    return_code = wcd_disable_mad_common();
    
    WCD_FREE(mad_global_info, WCD_MALLOC_START_TAG + 7);
    mad_global_info = NULL;
  }

  WCD_DEBUG_ERROR_1("-WCD_93XX: <ioctl_disable_mad_common: IOCTL returned RC:%d.>\n", return_code);
  return return_code;
}

wcd_result
ioctl_get_sidetone_iir_config ( wcd_usecase* p_handle, wcd_ioctl_cmd_sidetone_iir_config* p_command )
{
  wcd_result return_code = E_WCD_SUCCESS;
  WCD_DEBUG_INFO_0("+WCD_93XX: <IOCTL: ioctl_get_sidetone_iir_config.>\n");

  do
  {
    if (p_command->acdb_configuration.module_id != WCD_IIR_CONFIG_MID ||
        p_command->acdb_configuration.parameter_id != WCD_IIR_CONFIG_PID )
    {
      WCD_DEBUG_INFO_0("=WCD_93XX: <Invalid module or parameter ID.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
    }

    return_code = wcd_get_sidetone_iir_configuration(p_handle, &(p_command->acdb_configuration.iir_config));
  } while (WCD_G_FALSE_BOOLEAN);

  WCD_DEBUG_INFO_0("-WCD_93XX: <IOCTL: ioctl_get_sidetone_iir_config.>\n");
  return return_code;
}

wcd_result
ioctl_set_sidetone_iir_config ( wcd_usecase* p_handle, wcd_ioctl_cmd_sidetone_iir_config* p_command )
{
  wcd_result return_code = E_WCD_SUCCESS;
  wcd_ioctl_cmd_enable_sidetone enable_sidetone_command;

  WCD_DEBUG_INFO_0("+WCD_93XX: <IOCTL: ioctl_set_sidetone_iir_config.>\n");

  do
  {
    if (p_command->acdb_configuration.module_id != WCD_IIR_CONFIG_MID ||
        p_command->acdb_configuration.parameter_id != WCD_IIR_CONFIG_PID )
    {
      WCD_DEBUG_INFO_0("=WCD_93XX: <Invalid module or parameter ID.>\n");
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
    }

    if ( p_command->acdb_configuration.iir_config.enable == WCD_FALSE )
    {
      return_code = wcd_ioctl(p_handle, WCD_IOCTL_DISABLE_SIDETONE, NULL);
    }

    if ( p_command->acdb_configuration.iir_config.enable == WCD_TRUE )
    {
      //check if this usecase is part of sidetone already
      if ( wcd_check_for_sidetone_usecase(p_handle) == NULL )
      {
        //not part of sidetone, we need to enable it with the IIR Configuration

        if ( ((void*)p_command->acdb_configuration.tx_usecase_handle) == NULL )
        {
          WCD_DEBUG_INFO_0("=WCD_93XX: <Cannot set sidetone IIR configuration with NULL TX usecase handle.>\n");
          return_code = E_WCD_INVALID_HANDLE;
          break;
        }

        enable_sidetone_command.tx_usecase = (void*)(p_command->acdb_configuration.tx_usecase_handle);
        enable_sidetone_command.iir_configuration = &p_command->acdb_configuration.iir_config;

        //enable sidetone with the iir configuration
        return_code = wcd_ioctl(p_handle, WCD_IOCTL_ENABLE_SIDETONE, &enable_sidetone_command);
      }
      else
      {
        //this usecase is part of sidetone, just configure it since it is already enabled

        WCD_DEBUG_INFO_0("=WCD_93XX: <Configuring sidetone IIR.>\n");
        return_code = wcd_configure_sidetone_iir(p_handle, &(p_command->acdb_configuration.iir_config));
      }
    }
    
  } while (WCD_G_FALSE_BOOLEAN);

  WCD_DEBUG_INFO_0("-WCD_93XX: <IOCTL: ioctl_set_sidetone_iir_config.>\n");

  return return_code;
}





#ifdef WCD_MBHC_ENABLE

/**
 * \brief Client of MBHC calls this
 * IOCTL to start MBHC.
 */
wcd_result
ioctl_cmd_mbhc_set_start(wcd_usecase *p_handle, void* p_ioctl_params)
{
  wcd_result return_code = E_WCD_SUCCESS;

  HANDLE_UNUSED_PARAMETER(p_handle);

  WCD_DEBUG_INFO_0("+WCD_93XX: <ioctl_cmd_mbhc_set_start called> \n");

  {
    if (p_ioctl_params == NULL)
    {
        WCD_DEBUG_INFO_0("=WCD_93XX: <NULL payload passed> \n");
        return_code = E_WCD_INVALID_PARAMETER_ERROR;
        goto endnext;
    }

    return_code = wcd_activate_bus();
    if (return_code == E_WCD_SUCCESS)
    {
        return_code = mbhc_ioctl(E_WCD_CMD_MBHC_SET_START, p_ioctl_params, NULL);
        wcd_deactivate_bus();
        WCD_DEBUG_INFO_1("=WCD_93XX: <E_WCD_CMD_MBHC_SET_START completed, return_code=(0x%x)> \n", return_code);
    }
    else
    {
        WCD_DEBUG_INFO_0("=WCD_93XX: <E_WCD_CMD_MBHC_SET_START, Bus activate error> \n");
    }

  }

endnext:

  WCD_DEBUG_INFO_1("-WCD_93XX: <ioctl_cmd_mbhc_set_start, return_code=(0x%x)> \n", return_code);
  return return_code;
}

/**
 * \brief Client of MBHC calls this
 * IOCTL to stop MBHC.
 */
wcd_result
ioctl_cmd_mbhc_set_stop(wcd_usecase *p_handle)
{
  wcd_result return_code = E_WCD_SUCCESS;

  HANDLE_UNUSED_PARAMETER(p_handle);

  WCD_DEBUG_INFO_0("+WCD_93XX: <ioctl_cmd_mbhc_set_stop called> \n");

  {
    return_code = wcd_activate_bus();
    if (return_code == E_WCD_SUCCESS)
    {
        return_code = mbhc_ioctl(E_WCD_CMD_MBHC_SET_STOP, NULL, NULL);
        wcd_deactivate_bus();
        WCD_DEBUG_INFO_1("=WCD_93XX: <E_WCD_CMD_MBHC_SET_STOP completed, return_code=(0x%x)> \n", return_code);
    }
    else
    {
        WCD_DEBUG_INFO_0("=WCD_93XX: <E_WCD_CMD_MBHC_SET_STOP, Bus activate error> \n");
    }

  }

  WCD_DEBUG_INFO_1("-WCD_93XX: <ioctl_cmd_mbhc_set_stop, return_code=(0x%x)> \n", return_code);
  return return_code;
}

/**
 * \brief This function is called by WCD when a WCD CB
 * function is invoked by the WCD client to notify WCD with
 * button press - release interrupts from MSM GPIO
 */
wcd_result
wcd_ih_irq_handler_callback(wcd_usecase *p_handle, void* p_ioctl_params)
{
  wcd_result return_code = E_WCD_SUCCESS;

  HANDLE_UNUSED_PARAMETER(p_handle);
  HANDLE_UNUSED_PARAMETER(p_ioctl_params);

  WCD_DEBUG_INFO_0("+WCD_93XX: <wcd_ih_irq_handler_callback called> \n");

  {
    return_code = wcd_activate_bus();
    if (return_code == E_WCD_SUCCESS)
    {
        return_code = wcd_ih_irq_handler(NULL);
        wcd_deactivate_bus();
        WCD_DEBUG_INFO_1("=WCD_93XX: <E_WCD_CMD_IH_INVOKE_INTR_CB completed, return_code=(0x%x)> \n", return_code);
    }
    else
    {
        WCD_DEBUG_INFO_0("=WCD_93XX: <E_WCD_CMD_IH_INVOKE_INTR_CB, Bus activate error> \n");
    }

  }

  WCD_DEBUG_INFO_1("-WCD_93XX: <wcd_ih_irq_handler_callback, return_code=(0x%x)> \n", return_code);
  return return_code;
}

/**
 * \brief Client of MBHC calls this
 * IOCTL to start MBHC in FTM mode.
 */
wcd_result
ioctl_cmd_mbhc_set_ftm_start(wcd_usecase *p_handle)
{
  wcd_result return_code = E_WCD_SUCCESS;

  HANDLE_UNUSED_PARAMETER(p_handle);

  WCD_DEBUG_INFO_0("+WCD_93XX: <ioctl_cmd_mbhc_set_ftm_start called> \n");

  {
    return_code = mbhc_ioctl(E_WCD_CMD_MBHC_SET_FTM_START, NULL, NULL);
  }

  WCD_DEBUG_INFO_1("-WCD_93XX: <ioctl_cmd_mbhc_set_ftm_start, return_code=(0x%x)> \n", return_code);
  return return_code;
}

/**
 * \brief Client of MBHC calls this
 * IOCTL to send FTM commands to MBHC.
 */
wcd_result
ioctl_cmd_mbhc_set_ftm_cmd(wcd_usecase *p_handle, void* p_ioctl_params)
{
  wcd_result return_code = E_WCD_SUCCESS;

  HANDLE_UNUSED_PARAMETER(p_handle);

  WCD_DEBUG_INFO_0("+WCD_93XX: <ioctl_cmd_mbhc_set_ftm_cmd called> \n");

  {
    if (p_ioctl_params == NULL)
    {
        WCD_DEBUG_INFO_0("=WCD_93XX: <NULL payload passed> \n");
        return_code = E_WCD_INVALID_PARAMETER_ERROR;
        goto endnext;
    }

    return_code = wcd_activate_bus();
    if (return_code == E_WCD_SUCCESS)
    {
        return_code = mbhc_ioctl(E_WCD_CMD_MBHC_SET_FTM_CMD, p_ioctl_params, NULL);
        wcd_deactivate_bus();
        WCD_DEBUG_INFO_1("=WCD_93XX: <E_WCD_CMD_MBHC_SET_FTM_CMD completed, return_code=(0x%x)> \n", return_code);
    }
    else
    {
        WCD_DEBUG_INFO_0("=WCD_93XX: <E_WCD_CMD_MBHC_SET_FTM_CMD, Bus activate error> \n");
    }

  }

endnext:

  WCD_DEBUG_INFO_1("-WCD_93XX: <ioctl_cmd_mbhc_set_ftm_cmd, return_code=(0x%x)> \n", return_code);
  return return_code;
}

/**
 * \brief Client of MBHC calls this
 * IOCTL to send preset trigger commands to MBHC
 */
wcd_result
ioctl_cmd_mbhc_set_trigger_cmd(wcd_usecase *p_handle, void* p_ioctl_params)
{
  wcd_result return_code = E_WCD_SUCCESS;

  HANDLE_UNUSED_PARAMETER(p_handle);

  WCD_DEBUG_INFO_0("+WCD_93XX: <ioctl_cmd_mbhc_set_trigger_cmd called> \n");

  {
    if (p_ioctl_params == NULL)
    {
        WCD_DEBUG_INFO_0("=WCD_93XX: <NULL payload passed> \n");
        return_code = E_WCD_INVALID_PARAMETER_ERROR;
        goto endnext;
    }

    return_code = wcd_activate_bus();
    if (return_code == E_WCD_SUCCESS)
    {
        return_code = mbhc_ioctl(E_WCD_CMD_MBHC_SET_TRIGGER_CMD, p_ioctl_params, NULL);
        wcd_deactivate_bus();
        WCD_DEBUG_INFO_1("=WCD_93XX: <E_WCD_CMD_MBHC_SET_TRIGGER_CMD completed, return_code=(0x%x)> \n", return_code);
    }
    else
    {
        WCD_DEBUG_INFO_0("=WCD_93XX: <E_WCD_CMD_MBHC_SET_TRIGGER_CMD, Bus activate error> \n");
    }

  }

endnext:

  WCD_DEBUG_INFO_1("-WCD_93XX: <ioctl_cmd_mbhc_set_trigger_cmd, return_code=(0x%x)> \n", return_code);
  return return_code;
}

#endif

/**
 * \brief This WCD IOCTL is used by its caller to provide
 * its callback function to WCD. WCD modules store this
 *  CB function pointer and use it to call the client to
 *  read/write PMIC/MSM GPIOs
 */
wcd_result
ioctl_cmd_set_gpio_intf_cb(wcd_usecase *p_handle, void* p_ioctl_params)
{
  wcd_result return_code = E_WCD_SUCCESS;

  HANDLE_UNUSED_PARAMETER(p_handle);

  WCD_DEBUG_INFO_0("+WCD_93XX: <ioctl_cmd_set_gpio_intf_cb called> \n");

  {
    if (p_ioctl_params == NULL)
    {
        WCD_DEBUG_INFO_0("=WCD_93XX: <NULL payload passed> \n");
        return_code = E_WCD_INVALID_PARAMETER_ERROR;
        goto endnext;
    }

    return_code = wcd_execute_special_command(E_WCD_CMD_SET_GPIO_INTF_CB, p_ioctl_params, NULL);

  }

endnext:

  WCD_DEBUG_INFO_1("-WCD_93XX: <ioctl_cmd_set_gpio_intf_cb, return_code=(0x%x)> \n", return_code);
  return return_code;
}

/**
 * \brief This WCD IOCTL is used by its caller to provide
 * its callback function to WCD. WCD modules such as MBHC, MAD, VBAT
 * store this CB function pointer and use it to send events back to
 * its caller
 */
wcd_result
ioctl_cmd_set_event_cb(wcd_usecase *p_handle, void* p_ioctl_params)
{
  wcd_result return_code = E_WCD_SUCCESS;

  HANDLE_UNUSED_PARAMETER(p_handle);

  WCD_DEBUG_INFO_0("+WCD_93XX: <ioctl_cmd_set_event_cb called> \n");

  {
    if (p_ioctl_params == NULL)
    {
        WCD_DEBUG_INFO_0("=WCD_93XX: <NULL payload passed> \n");
        return_code = E_WCD_INVALID_PARAMETER_ERROR;
        goto endnext;
    }

    return_code = wcd_execute_special_command(E_WCD_CMD_SET_EVENT_CB, p_ioctl_params, NULL);

  }

endnext:

  WCD_DEBUG_INFO_1("-WCD_93XX: <ioctl_cmd_set_event_cb, return_code=(0x%x)> \n", return_code);
  return return_code;
}

/******************************************************************************
 **************              [INTF] IOCTL HANDLER               ***************
 *****************************************************************************/

wcd_result
wcd_ioctl (void *p_handle, uint32 ioctl_cmd, void *p_ioctl_command)
{
  wcd_result return_code = E_WCD_INVALID_PARAMETER_ERROR;
  wcd_usecase* usecase = NULL;

#ifdef WCD_MBHC_ENABLE
  wcd_invoke_cb_event_struct    temp_wcd_cb_event;
#endif
  int wcdIoctlMutexStatus     = E_WCD_GENERIC_ERROR;
  uint32 mutex_timeout        = WCD_INFINITE;


  // WCD_IOCTL_CMD_SET_GPIO_INTF_CB & WCD_IOCTL_CMD_SET_EVENT_CB can be called by the HLOS client before WCD driver is initialized
  if ( (wcd_driver_initialized == WCD_FALSE) && (ioctl_cmd != WCD_IOCTL_CMD_SET_GPIO_INTF_CB) && (ioctl_cmd != WCD_IOCTL_CMD_SET_EVENT_CB) )
  {
    WCD_DEBUG_INFO_0("+WCD_93XX: <IOCTL handler.>\n"); //this line is used to balance the logs
    WCD_DEBUG_INFO_0("-WCD_93XX: <Driver has not been initialized.>\n");
    return E_WCD_UNINITIALIZED_DRIVER;
  }

  WCD_DEBUG_INFO_1("=WCD_93XX: <WCD IOCTL called, cmd=(%u)> \n", (unsigned int)ioctl_cmd);

//#ifdef WCD_MBHC_ENABLE
  if (wcdIoctlMutex != NULL) 
  {
    wcdIoctlMutexStatus = WCD_WAIT_FOR_MUTEX((void*)wcdIoctlMutex, mutex_timeout);
  }
  WCD_DEBUG_INFO_2("=WCD_93XX: <WCD IOCTL cmd=(%u), Acquired mutex %u> \n", (unsigned int)ioctl_cmd, (unsigned int)wcdIoctlMutexStatus);
//#endif

  if (p_handle == NULL)
  {
    switch( ioctl_cmd )
    {
      case WCD_IOCTL_GET_SLIMBUS_LOGICAL_ADDRESS:
      case WCD_IOCTL_REGISTER_WRITE:
      case WCD_IOCTL_REGISTER_READ:
      case WCD_IOCTL_REGISTER_DUMP_TO_LOG:
      case WCD_IOCTL_GET_CODEC_INFO:
      case WCD_IOCLT_CONFIGURE_MAD:
      case WCD_IOCLT_DISABLE_MAD:
      case WCD_IOCTL_CMD_MBHC_SET_START:
      case WCD_IOCTL_CMD_MBHC_SET_STOP:
      case WCD_IOCTL_CMD_INVOKE_CB:
      case WCD_IOCTL_CMD_SET_GPIO_INTF_CB:
      case WCD_IOCTL_CMD_SET_EVENT_CB:
      case WCD_IOCTL_CMD_MBHC_SET_FTM_START:
      case WCD_IOCTL_CMD_MBHC_SET_FTM_CMD:
      case WCD_IOCTL_CMD_MBHC_SET_TRIGGER_CMD:
        break;
      default:
        //this line is used to balance the logs
        WCD_DEBUG_INFO_0("+WCD_93XX: <IOCTL handler.>\n");
        WCD_DEBUG_INFO_1("-WCD_93XX: <Client state is NULL. RC:%d.>\n",E_WCD_INVALID_HANDLE);
        return_code = E_WCD_INVALID_HANDLE;
        goto ioctl_end;
    }
  }
  else
  {
    //check if the handle is located on the list
    if( wcd_find_usecase((wcd_usecase*)p_handle) != E_WCD_SUCCESS )
    {
      WCD_DEBUG_INFO_0("+WCD_93XX: <IOCTL handler.>\n"); //this line is used to balance the logs
      WCD_DEBUG_INFO_1("-WCD_93XX: <Unknown handle, cannot execute ioctl. RC:%d.>\n",E_WCD_INVALID_PARAMETER_ERROR);
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      goto ioctl_end;
    }
  }

  usecase = (wcd_usecase*)p_handle;

  WCD_DEBUG_INFO_1("+WCD_93XX: <Processing IOCTL handler, cmd=(%u)> \n", (unsigned int)ioctl_cmd);

  //call the appropriate function based on the ioctl_cmd
  switch (ioctl_cmd)
  {
    case WCD_IOCTL_GET_SLIMBUS_LOGICAL_ADDRESS:
      return_code = ioctl_get_slimbus_logical_address((wcd_ioctl_cmd_slimbus_logical_address*)p_ioctl_command);
      break;
    case WCD_IOCTL_REGISTER_READ:
      return_code = ioctl_register_read((wcd_ioctl_cmd_register_access*)p_ioctl_command);
      break;
    case WCD_IOCTL_REGISTER_WRITE:
      return_code = ioctl_register_write((wcd_ioctl_cmd_register_access*)p_ioctl_command);
      break;
    case WCD_IOCTL_CONNECT_SOURCE_TO_SINK:
      return_code = ioctl_connect_source_to_sink(usecase, (wcd_ioctl_cmd_connect_source_to_sink*)p_ioctl_command);
      break;
    case WCD_IOCTL_CONNECT_SOURCES_TO_SINKS:
      return_code = ioctl_connect_sources_to_sinks(usecase, (wcd_ioctl_cmd_connect_sources_to_sinks*)p_ioctl_command);
      break;
    case WCD_IOCTL_DISCONNECT_CONNECTION:
      return_code = ioctl_disconnect_connection(usecase, (wcd_ioctl_cmd_disconnect_connection*)p_ioctl_command);
      break;
    case WCD_IOCTL_DISCONNECT_CONNECTIONS:
      return_code = ioctl_disconnect_connections(usecase);
      break;
    case WCD_IOCTL_SET_PROFILE:
      return_code = ioctl_set_profile(usecase, (wcd_ioctl_cmd_set_profile*)p_ioctl_command);
      break;
    case WCD_IOCTL_ENABLE_USECASE:
      return_code = ioctl_enable_usecase(usecase, (wcd_ioctl_cmd_enable_usecase*)p_ioctl_command);
      break;
    case WCD_IOCTL_DISABLE_USECASE:
      return_code = ioctl_disable_usecase(usecase);
      break;
    case WCD_IOCTL_REGISTER_DUMP_TO_LOG:
      return_code = ioctl_register_dump();
      break;
    case WCD_IOCTL_ENABLE_ANC:
      return_code = ioctl_enable_anc(usecase, (wcd_ioctl_cmd_enable_anc*)p_ioctl_command);
      break;
    case WCD_IOCTL_ENABLE_SLIMBUS_INTF:
      return_code = ioctl_enable_slimbus_intf( usecase, (wcd_ioctl_cmd_slimbus_intf_config*)p_ioctl_command);
      break;
    case WCD_IOCTL_DISABLE_ANC:
      return_code = ioctl_disable_anc(usecase);
      break;
    case WCD_IOCTL_GET_CODEC_INFO:
      return_code = ioctl_get_codec_info( (wcd_ioctl_cmd_codec_info*)p_ioctl_command );
      break;
    case WCD_IOCTL_ENABLE_SIDETONE:
      return_code = ioctl_enable_sidetone( usecase, (wcd_ioctl_cmd_enable_sidetone*)p_ioctl_command );
      break;
    case WCD_IOCTL_DISABLE_SIDETONE:
      return_code = ioctl_disable_sidetone( usecase );
      break;
    case WCD_IOCTL_ENABLE_FB_SPKR_PROT:
      return_code = ioctl_enable_fb_spkr_prot( usecase, (wcd_ioctl_cmd_enable_fb_spkr_prot*)p_ioctl_command );
      break;
    case WCD_IOCTL_DISABLE_FB_SPKR_PROT:
      return_code = ioctl_disable_fb_spkr_prot( usecase );
      break;
    case WCD_IOCLT_CONFIGURE_MAD:
      return_code = ioctl_configure_mad_common( (wcd_ioctl_cmd_configure_mad*) p_ioctl_command );
      break;
    case WCD_IOCLT_DISABLE_MAD:
      return_code = ioctl_disable_mad_common();
      break;
    case WCD_IOCTL_GET_SIDETONE_IIR_CONFIG:
      return_code = ioctl_get_sidetone_iir_config( usecase, (wcd_ioctl_cmd_sidetone_iir_config*) p_ioctl_command );
      break;
    case WCD_IOCTL_SET_SIDETONE_IIR_CONFIG:
      return_code = ioctl_set_sidetone_iir_config( usecase, (wcd_ioctl_cmd_sidetone_iir_config*) p_ioctl_command );
      break;
#ifdef WCD_MBHC_ENABLE
    case WCD_IOCTL_CMD_MBHC_SET_START:
      return_code = ioctl_cmd_mbhc_set_start(usecase, (void*)p_ioctl_command);
      break;
    case WCD_IOCTL_CMD_MBHC_SET_STOP:
      return_code = ioctl_cmd_mbhc_set_stop(usecase);
      break;
    case WCD_IOCTL_CMD_INVOKE_CB:
      temp_wcd_cb_event.wcd_cb_event = (wcd_invoke_cb_event)(((p_wcd_invoke_cb_event_struct)p_ioctl_command)->wcd_cb_event);
      if (temp_wcd_cb_event.wcd_cb_event == E_WCD_INVOKE_CB_IH_EVENT)
      {
          return_code = wcd_ih_irq_handler_callback(usecase, (void*)p_ioctl_command);
      }
      else
      {
          WCD_DEBUG_INFO_0("=WCD_93XX: <WCD_IOCTL_CMD_INVOKE_CB: Invalid CB Event> \n");
          return_code = E_WCD_INVALID_PARAMETER_ERROR;
      }
      break;
    case WCD_IOCTL_CMD_MBHC_SET_FTM_START:
      return_code = ioctl_cmd_mbhc_set_ftm_start(usecase);
      break;
    case WCD_IOCTL_CMD_MBHC_SET_FTM_CMD:
      return_code = ioctl_cmd_mbhc_set_ftm_cmd(usecase, (void*)p_ioctl_command);
      break;
    case WCD_IOCTL_CMD_MBHC_SET_TRIGGER_CMD:
      return_code = ioctl_cmd_mbhc_set_trigger_cmd(usecase, (void*)p_ioctl_command);
      break;
#endif

    case WCD_IOCTL_CMD_SET_GPIO_INTF_CB:
      return_code = ioctl_cmd_set_gpio_intf_cb(usecase, (void*)p_ioctl_command);
      break;
    case WCD_IOCTL_CMD_SET_EVENT_CB:
      return_code = ioctl_cmd_set_event_cb(usecase, (void*)p_ioctl_command);
      break;
    default:
      return_code = E_WCD_INVALID_PARAMETER_ERROR;
      break;
  }

ioctl_end:
//#ifdef WCD_MBHC_ENABLE
  if ( (wcdIoctlMutexStatus == WCD_WAIT_DONE) && (wcdIoctlMutex != NULL) )
  {	
      wcdIoctlMutexStatus = WCD_RELEASE_MUTEX(wcdIoctlMutex);
      WCD_DEBUG_INFO_1("-WCD_93XX: <Releasing wcdIoctlMutex (%u)> \n", (unsigned int)wcdIoctlMutexStatus);
  }
//#endif

  WCD_DEBUG_INFO_2("-WCD_93XX: <Exiting WCD IOCTL handler, cmd=(%u), return_code=(%d)> \n", (unsigned int)ioctl_cmd, return_code);
  return return_code;
}

/******************************************************************************
 **************              [INTF] IOCTL HANDLER               ***************
 *****************************************************************************/

