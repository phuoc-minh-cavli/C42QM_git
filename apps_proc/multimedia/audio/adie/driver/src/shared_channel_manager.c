/**
@file shared_channel_manager.c
@brief 
Implementation and internal functions of WCD Shared Channel Manager
*/
/*-------------------------------------------------------------------------------
     Copyright (c) 2011 Qualcomm Technologies Incorporated.
	 All rights reserved.
	 Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
                             Edit History
$Header: //source/qcom/qct/multimedia2/Audio/drivers/hw/Adie/AdieCodec/
			Common3/main/latest/src/shared_channel_manager.c $

when       who    what, where, why
--------   ---    ------------------------------------------------------------
06/24/14   AG	  Klockwork errors.
10/09/13   AG     Defining default slave port id and direction 
12/15/10   SK	  Initial revision
===============================================================================*/

#include "shared_channel_manager.h"
#include "wcd_utility.h"
#include "bus_manager.h"
#include "bus_manager_target.h"
#include "target_defs.h"
#include "wcd_gen_id_defines.h"
#include "wcd_osal.h"

#ifdef SPL_LOG_SUPPORT
	#include "shared_channel_manager.tmh"
#endif

WCD_CORE_PAGED_CODE_DATA_START

void*   spkr_prot_shared_channel_handle[WCD_SPKR_PROT_CHANNELS] = { NULL, NULL };
void*   spkr_prot_shared_channel_group_handle = NULL;

void*   spkr_2_prot_shared_channel_handle[WCD_SPKR_PROT_CHANNELS] = { NULL, NULL };
void*   spkr_2_prot_shared_channel_group_handle = NULL;

// void*   mad_shared_channel_handle = NULL;
// void*   mad_shared_channel_group_handle = NULL;

/**
 * \Defines for default port ids and direction
 */
#define WCD_DEFAULT_SLAVE_PORT_ID 0x40000
#define WCD_DEFAULT_DIRECTON      0x0 /** corresponds to RX direction **/

WCD_CORE_PAGED_CODE_DATA_STOP

WCD_CORE_PAGED_CODE_SEGMENT_START

/**
 * \brief Counts the number of unique slimbus ports in the usecase.
 */
wcd_result determine_slimbus_port_count(wcd_usecase* usecase, uint32* port_count)
{
  WCD_CORE_PAGED_CODE
	
  wcd_element* slimbus_port_elements[WCD_MAX_ADIE_CHANNELS];
  uint32 counter = 0;

  wcd_list_element* connection_iterator;
  wcd_connection_info* connection;
  wcd_element* element;

  wcd_bool unique_port = WCD_FALSE;
  uint32 iterator = 0;

  wcd_result return_code = E_WCD_SUCCESS;

  WCD_DEBUG_INFO_0("+WCD_93XX: <Determine slimbus port count.>\n");


    connection_iterator = usecase->connection_list->head;

    while (connection_iterator != NULL)
    {
      connection = (wcd_connection_info*)connection_iterator->data;
      connection_iterator = connection_iterator->next;

      //try head element (RX)
      element = (wcd_element*)connection->route->elements->head->data;

      if (element->type != WCD_BUS_PORT_TYPE_ID)
      {
        //try tail element (TX)
        element = (wcd_element*)connection->route->elements->tail->data;

        if (element->type != WCD_BUS_PORT_TYPE_ID)
        {
          WCD_DEBUG_ERROR_0("=WCD_93XX: <Neither the tail nor the head of the route's elements was a BUSPORT type.>\n");
          return_code = E_WCD_INVALID_PARAMETER_ERROR;

          break;
        }
      }

      //verify element is not in slimbus_port_elements and add it
      unique_port = WCD_TRUE;

      for(iterator = 0; iterator < counter; iterator++)
      {
        if (slimbus_port_elements[iterator] == element)
        {
          WCD_DEBUG_INFO_0("=WCD_93XX: <Not a unique slimbus port.>\n");
          unique_port = WCD_FALSE;
          break;
        }
      }

      //no elements in slimbus_port_elements matched the element being added now
      if (unique_port == WCD_TRUE)
      {
        slimbus_port_elements[counter] = element;
        counter++;
      }
    }

    if (return_code == E_WCD_SUCCESS)
    {
      *port_count = counter;
    }

  WCD_DEBUG_INFO_1("-WCD_93XX: <Determine slimbus port count. RC:%d.>\n", return_code);

  return return_code;

}

wcd_result wcd_configure_shared_channel(wcd_usecase *usecase, wcd_ioctl_cmd_slimbus_intf_config* slimbus_cmd)
{
  WCD_CORE_PAGED_CODE
	
  wcd_result return_code = E_WCD_SUCCESS;

  int16     max_num_of_slave_ports = 0;
  uint16    slave_port_offset = 0;

  uint8     used_slimbus_ports[WCD_MAX_ADIE_CHANNELS];
  uint8     i;
  wcd_bool  port_configured = WCD_FALSE;

  uint32    slimbus_unique_port_count = 0;
  
  wcd_connection_info*  connection;
  wcd_list_element*     iterator;
  uint16                ch_index;
  
  uint32                slave_port_id = WCD_DEFAULT_SLAVE_PORT_ID;
  wcd_slimbus_direction direction;// = WCD_DEFAULT_DIRECTON;

  uint16  rx_multichannel_payload = 0;
  uint16  tx_multichannel_payload = 0;
  
  uint16 reg = 0;
  uint8  mask = 0xFF;
  uint8  shift = 0;
  uint8  value = 0;  

  WCD_DEBUG_INFO_0("+WCD_93XX: <Configure SLIMBUS shared channels.>\n");

  for(i = 0; i < WCD_MAX_ADIE_CHANNELS; i++)
  {
    used_slimbus_ports[i] = 0;
  }

  do
  {
    //this is opposite from what is stored in ACDB....
	  if (slimbus_cmd->direction == E_WCD_SLIMBUS_RX)
    {
      max_num_of_slave_ports = WCD_PGD_MAX_NUMBER_OF_RX_SLAVE_DEV_PORTS;
      slave_port_offset = WCD_RX_PORT_ID_OFFSET;
    }
    else if (slimbus_cmd->direction == E_WCD_SLIMBUS_TX)
    {
      max_num_of_slave_ports = WCD_PGD_MAX_NUMBER_OF_TX_SLAVE_DEV_PORTS;
      slave_port_offset = WCD_TX_PORT_ID_OFFSET;
    }

    if (max_num_of_slave_ports == 0)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Error determining direction.>\n");
      return_code = E_WCD_INVALID_SLIMBUS_INTERFACE_CONFIGURATION;
      break;
    }

    //make sure there is a context pointer
    if (usecase->context_pointer == NULL)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Usecase's context pointer is NULL. Unable to get sample rate/bitwidth.>\n");
      return_code = E_WCD_INVALID_HANDLE;
      break;
    }
     
    if (usecase->connection_list == NULL)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <No connections defined in usecase.>\n");
      return_code = E_WCD_INVALID_HANDLE;
      break;
    }

    //verify the number of channels to enable on the sb slave matches the 
    //number of unique slimbus ports in the usecase
    if (determine_slimbus_port_count(usecase, &slimbus_unique_port_count) != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Error determining the number of unique slimbus ports in the usecase.>\n");
      return_code = E_WCD_INVALID_SLIMBUS_INTERFACE_CONFIGURATION;
      break;
    }

    if (slimbus_unique_port_count != slimbus_cmd->num_of_channels)
    {
      WCD_DEBUG_ERROR_2("=WCD_93XX: <Mismatch between usecase and slimbus cmd IOCTL. Usecase has %u unique ports. IOCTL has %u channels.>\n", (unsigned int)slimbus_unique_port_count, (unsigned int)slimbus_cmd->num_of_channels);
      return_code = E_WCD_INVALID_SLIMBUS_INTERFACE_CONFIGURATION;
      break;
    }

    //loop through the channels once to determine multi channel grouping
    iterator = usecase->connection_list->head;

    while(iterator != NULL)
    {
      connection = (wcd_connection_info*)iterator->data;

      if (connection->route == NULL || connection->route->elements == NULL)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <No route or elements in connection.>\n");
        return_code = E_WCD_INVALID_PARAMETER_ERROR;
        break;
      }

      //determine the slimbus port number
      if (slimbus_cmd->direction == E_WCD_SLIMBUS_RX)
      {
        return_code = wcd_determine_slimbusport_number( (wcd_element*)connection->route->elements->head->data,
                                                        &slave_port_id,
                                                        &direction );
      }
      else
      {
        return_code = wcd_determine_slimbusport_number( (wcd_element*)connection->route->elements->tail->data,
                                                        &slave_port_id,
                                                        &direction );
      }

      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error determing slimbus port number.>\n");
        break;
      }

      slave_port_id--; //subtract one to get slave port id (we only have slave port number)

      //add the slimbus port to the multichannel paylod
      if (direction == E_WCD_SLIMBUS_RX)
      {
        rx_multichannel_payload = rx_multichannel_payload | ( 1 << slave_port_id);
      }
      else 
      {
        tx_multichannel_payload = tx_multichannel_payload | ( 1 << slave_port_id);
      }

      iterator = iterator->next;
    }

    //break if error
    if (return_code != E_WCD_SUCCESS)
    {
      break;
    }

    //group up all the channels into 1 resource group
    if ( wcd_osal_bus_sc_alloc_group(&(usecase->slimbus_shared_channel_group_handle)) != E_WCD_SUCCESS )
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Could not allocate resource group.>\n");
      return_code = E_WCD_SLIMBUS_SHARED_CHANNEL_ERROR;
      break;
    }

    //go through channels again to get the handles and apply the payload determined above
    iterator = usecase->connection_list->head;
    ch_index = 0;

    while(iterator != NULL)
    {
      port_configured = WCD_FALSE;
      connection = (wcd_connection_info*)iterator->data;

      if (connection->route == NULL || connection->route->elements == NULL)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <No route or elements in connection.>\n");
        return_code = E_WCD_INVALID_PARAMETER_ERROR;
        break;
      }

      if (slimbus_cmd->direction == E_WCD_SLIMBUS_RX)
      {
        return_code = wcd_determine_slimbusport_number( (wcd_element*)connection->route->elements->head->data,
                                                        &slave_port_id,
                                                        &direction );
      }
      else
      {
        return_code = wcd_determine_slimbusport_number( (wcd_element*)connection->route->elements->tail->data,
                                                        &slave_port_id,
                                                        &direction );
      }

      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error determing slimbus port number.>\n");
        break;
      }

      //before we do anything, make sure the slave port was not used already -- for reuse last bus port
      for(i = 0; i < ch_index; i++)
      {
        if (used_slimbus_ports[i] == slave_port_id)
        {
          port_configured = WCD_TRUE;
          break;
        }
      }

      //if the port is already configured, move to the next connection
      //ch_index should not move because shared channel is less than number of connections
      if (port_configured)
      {
        iterator = iterator->next;

        continue;
      }

      //mark the slimbus port for the particular channel index
      used_slimbus_ports[ch_index] = (uint8)slave_port_id;

      slave_port_id--; //subtract one to get slave port id (we only have slave port number)

      //write channel mapping + watermark enable to registers
      if (slimbus_cmd->direction == E_WCD_SLIMBUS_RX) // RX
      {
        //multi channel mapping reg 0
        reg = (uint16)(WCD_PGD_RX_PORT_MULTI_CHANNEL_0_REG_BASE + 
          (WCD_PGD_RX_PORT_MULTI_CHANNEL_0_REG_OFFSET_MULT * slave_port_id));
        value = (uint8)(rx_multichannel_payload & 0xFF);
        
        return_code = wcd_register_write(E_WCD_INTF, reg, mask, shift, value);
        if (return_code != E_WCD_SUCCESS)
        {
          break;
        }

        //multi channel mapping reg 1
        reg = (uint16)(WCD_PGD_RX_PORT_MULTI_CHANNEL_1_REG_BASE + 
          (WCD_PGD_RX_PORT_MULTI_CHANNEL_1_REG_OFFSET_MULT * slave_port_id));
        value = (uint8)((rx_multichannel_payload & 0xFF00) >> 8);
        
        return_code = wcd_register_write(E_WCD_INTF, reg, mask, shift, value);
        if (return_code != E_WCD_SUCCESS)
        {
          break;
        }

        //watermark + enable
        reg = (uint16)(WCD_PGD_RX_PORT_CONFIGURATION_REG_BASE + 
          (WCD_PGD_RX_PORT_CONFIGURATION_REG_OFFSET_MULT * slave_port_id));
        value = (uint8)(WCD_SLAVE_PORT_WATER_MARK_VALUE<<WCD_SLAVE_PORT_WATER_MARK_SHIFT);
        value |= WCD_SLAVE_PORT_ENABLE;

        return_code = wcd_register_write(E_WCD_INTF, reg, mask, shift, value);
        if (return_code != E_WCD_SUCCESS)
        {
          break;
        }
      }
      else // TX .... SLIMBUS_PORT_SOURCE_FLOW
      {
        //multi channel mapping reg 0
        reg = (uint16)(WCD_PGD_TX_PORT_MULTI_CHANNEL_0_REG_BASE + 
          (WCD_PGD_TX_PORT_MULTI_CHANNEL_0_REG_OFFSET_MULT * slave_port_id));
        value = (uint8)(tx_multichannel_payload & 0xFF);
        
        return_code = wcd_register_write(E_WCD_INTF, reg, mask, shift, value);
        if (return_code != E_WCD_SUCCESS)
        {
          break;
        }

        //multi channel mapping reg 1
        reg = (uint16)(WCD_PGD_TX_PORT_MULTI_CHANNEL_1_REG_BASE + 
          (WCD_PGD_TX_PORT_MULTI_CHANNEL_1_REG_OFFSET_MULT * slave_port_id));
        value = (uint8)((tx_multichannel_payload & 0xFF00) >> 8);
        
        return_code = wcd_register_write(E_WCD_INTF, reg, mask, shift, value);
        if (return_code != E_WCD_SUCCESS)
        {
          break;
        }

        //watermark + enable
        reg = (uint16)(WCD_PGD_TX_PORT_CONFIGURATION_REG_BASE + 
          (WCD_PGD_TX_PORT_CONFIGURATION_REG_OFFSET_MULT * slave_port_id));
        value = (uint8)(WCD_SLAVE_PORT_WATER_MARK_VALUE<<WCD_SLAVE_PORT_WATER_MARK_SHIFT);
        value |= WCD_SLAVE_PORT_ENABLE;

        return_code = wcd_register_write(E_WCD_INTF, reg, mask, shift, value);
        if (return_code != E_WCD_SUCCESS)
        {
          break;
        }
      }

      wcd_bus_flush(E_WCD_INTF);
      WCD_DEBUG_INFO_1("=WCD_93XX: <Allocating shared channel: %u.>\n", slimbus_cmd->shared_channel_num[ch_index]);
	  
      return_code = wcd_osal_bus_sc_connect_port_and_channel(slave_port_id + slave_port_offset,
                                              slimbus_cmd->shared_channel_num[ch_index],
                                              usecase->slimbus_shared_channel_group_handle,
                                              &(usecase->slimbus_shared_channel_handle[ch_index]),
                                              slimbus_cmd->direction);

      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error connecting the port to the channel.>\n");
			  return_code = E_WCD_INVALID_BUS_HANDLE;
        break;
      }

      iterator = iterator->next;
      ch_index++;
    }

    //error out if there is an error
    if (return_code != E_WCD_SUCCESS)
    {
      break;
    }

    //enable shared channels now
    return_code = wcd_osal_bus_sc_enable(usecase->context_pointer->sample_rate,
                                          usecase->context_pointer->bit_width,
                                          usecase->slimbus_shared_channel_group_handle);

  } while(WCD_G_FALSE_BOOLEAN);

  if (return_code != E_WCD_SUCCESS)
  {
    //just call deinitialize which will clean it all up
    wcd_close_shared_channel(usecase);
  }

  WCD_DEBUG_INFO_1("-WCD_93XX: <Configure SLIMBUS shared channels. RC:%d.>\n", return_code);

  return return_code;
}

wcd_result wcd_close_shared_channel(wcd_usecase *usecase)
{
  WCD_CORE_PAGED_CODE
  wcd_result return_code = E_WCD_SUCCESS;
  uint8 i = 0;

  WCD_DEBUG_INFO_0("+WCD_93XX: <Close SLIMBUS shared channels.>\n");
  
  do
  {
    if (usecase->slimbus_shared_channel_group_handle == NULL)
    {
      break;
    }

    return_code = wcd_osal_bus_sc_disable(usecase->slimbus_shared_channel_group_handle);

    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Could not deactivate shared channels.>\n");
    }

    for(i = 0; i < WCD_MAX_ADIE_CHANNELS; i++)
    {
      if((usecase->slimbus_shared_channel_handle[i]) != NULL)
      {
        return_code = wcd_osal_bus_sc_dealloc_channel(usecase->slimbus_shared_channel_handle[i]);

        if (return_code != E_WCD_SUCCESS)
        {
          WCD_DEBUG_ERROR_1("=WCD_93XX: <Could not deallocated shared channel. Channel Index:%u.>\n", i);
        }

        usecase->slimbus_shared_channel_handle[i] = NULL;
      }
    }

    return_code = wcd_osal_bus_sc_dealloc_group(usecase->slimbus_shared_channel_group_handle);

    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Could not deallocated resource group.>\n");
    }

    usecase->slimbus_shared_channel_group_handle = NULL;

  } while(WCD_G_FALSE_BOOLEAN);

  WCD_DEBUG_INFO_1("-WCD_93XX: <Close SLIMBUS shared channels. RC:%d.>\n", return_code);

  return return_code;
}

wcd_result wcd_configure_spkr_prot_shared_channel(uint8 v_shared_channel_num, uint8 i_shared_channel_num)
{
  WCD_CORE_PAGED_CODE
	
  wcd_result return_code = E_WCD_SUCCESS;

  uint16  slave_port_offset = 0;

  uint16  ch_index;
  uint8   shared_channel_num;
  uint32  slave_port_id = 0; /*To avoid klocwork errors*/
  uint16  tx_multichannel_payload = 0;

  uint16 reg = 0;
  uint8  mask = 0xFF;
  uint8  shift = 0;
  uint8  value = 0;

  WCD_DEBUG_INFO_0("+WCD_93XX: <Configure Speaker Protection SLIMBUS shared channels.>\n");

  do
  {
    slave_port_offset = WCD_TX_PORT_ID_OFFSET;

    //get tx multichannel payload
    //first voltage
    return_code = wcd_get_spkr_prot_voltage_port(&slave_port_id);
    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Error getting voltage port.>\n");
      break;
    }

    slave_port_id--; //subtract one to get slave port id (we only have slave port number)
    tx_multichannel_payload = tx_multichannel_payload | ( 1 << slave_port_id);

    //second current
    return_code = wcd_get_spkr_prot_current_port(&slave_port_id);
    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Error getting voltage port.>\n");
      break;
    }

    slave_port_id--; //subtract one to get slave port id (we only have slave port number)
    tx_multichannel_payload = tx_multichannel_payload | ( 1 << slave_port_id);


    //group up all the channels into 1 resource group
    if ( wcd_osal_bus_sc_alloc_group(&spkr_prot_shared_channel_group_handle) != E_WCD_SUCCESS )
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Could not allocate resource group.>\n");
      return_code = E_WCD_SLIMBUS_SHARED_CHANNEL_ERROR;
      break;
    }

    //go through channels again to get the handles and apply the payload determined above
    
    for(ch_index = 0; ch_index < WCD_SPKR_PROT_CHANNELS; ch_index++)
    {
      //first voltage
      if (ch_index == 0)
      {
        return_code = wcd_get_spkr_prot_voltage_port(&slave_port_id);
        shared_channel_num = v_shared_channel_num;
      }
      //second current
      else
      {
        return_code = wcd_get_spkr_prot_current_port(&slave_port_id);
        shared_channel_num = i_shared_channel_num;
      }

      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error getting voltage/current port.>\n");
        break;
      }

      slave_port_id--; //subtract one to get slave port id (we only have slave port number)

      //multi channel mapping reg 0
      reg = (uint16)(WCD_PGD_TX_PORT_MULTI_CHANNEL_0_REG_BASE + 
        (WCD_PGD_TX_PORT_MULTI_CHANNEL_0_REG_OFFSET_MULT * slave_port_id));
      value = (uint8)(tx_multichannel_payload & 0xFF);
      
      return_code = wcd_register_write(E_WCD_INTF, reg, mask, shift, value);
      if (return_code != E_WCD_SUCCESS)
      {
        break;
      }

      //multi channel mapping reg 1
      reg = (uint16)(WCD_PGD_TX_PORT_MULTI_CHANNEL_1_REG_BASE + 
        (WCD_PGD_TX_PORT_MULTI_CHANNEL_1_REG_OFFSET_MULT * slave_port_id));
      value = (uint8)((tx_multichannel_payload & 0xFF00) >> 8);
      
      return_code = wcd_register_write(E_WCD_INTF, reg, mask, shift, value);
      if (return_code != E_WCD_SUCCESS)
      {
        break;
      }

      //watermark + enable
      reg = (uint16)(WCD_PGD_TX_PORT_CONFIGURATION_REG_BASE + 
        (WCD_PGD_TX_PORT_CONFIGURATION_REG_OFFSET_MULT * slave_port_id));
      value = (uint8)(WCD_SLAVE_PORT_WATER_MARK_VALUE<<WCD_SLAVE_PORT_WATER_MARK_SHIFT);
      value |= WCD_SLAVE_PORT_ENABLE;

      return_code = wcd_register_write(E_WCD_INTF, reg, mask, shift, value);
      if (return_code != E_WCD_SUCCESS)
      {
        break;
      }

      wcd_bus_flush(E_WCD_INTF);
      WCD_DEBUG_INFO_1("=WCD_93XX: <Allocating shared channel: %u.>\n", shared_channel_num);
	  
      return_code = wcd_osal_bus_sc_connect_port_and_channel(slave_port_id + slave_port_offset,
                                              shared_channel_num,
                                              spkr_prot_shared_channel_group_handle,
                                              &(spkr_prot_shared_channel_handle[ch_index]),
                                              E_WCD_SLIMBUS_TX);

      if (return_code != E_WCD_SUCCESS)
      {
        WCD_DEBUG_ERROR_0("=WCD_93XX: <Error connecting the port to the channel.>\n");
			  return_code = E_WCD_INVALID_BUS_HANDLE;
        break;
      }
    }

    //error out if there is an error
    if (return_code != E_WCD_SUCCESS)
    {
      break;
    }

    //enable shared channels now
    return_code = wcd_osal_bus_sc_enable(48000,16, spkr_prot_shared_channel_group_handle);

  } while(WCD_G_FALSE_BOOLEAN);

  if (return_code != E_WCD_SUCCESS)
  {
    //just call deinitialize which will clean it all up
    wcd_close_spkr_prot_shared_channel();
  }

  WCD_DEBUG_INFO_1("-WCD_93XX: <Configure Speaker Protection SLIMBUS shared channels. RC:%d.>\n", return_code);

  return return_code;
}

wcd_result wcd_close_spkr_prot_shared_channel(void)
{
  WCD_CORE_PAGED_CODE
	
  wcd_result return_code = E_WCD_SUCCESS;
  uint8 i;

  WCD_DEBUG_INFO_0("+WCD_93XX: <Close Speaker Protection SLIMBUS shared channels.>\n");

  do
  {
    if (spkr_prot_shared_channel_group_handle == NULL)
    {
      break;
    }

    return_code = wcd_osal_bus_sc_disable(spkr_prot_shared_channel_group_handle);

    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Could not deactivate shared channels.>\n");
    }
 

    for(i = 0; i < WCD_SPKR_PROT_CHANNELS; i++)
    {
      if(spkr_prot_shared_channel_handle[i] != NULL)
      {
        return_code = wcd_osal_bus_sc_dealloc_channel(spkr_prot_shared_channel_handle[i]);

        if (return_code != E_WCD_SUCCESS)
        {
          WCD_DEBUG_ERROR_1("=WCD_93XX: <Could not deallocated shared channel. Channel Index:%u.>\n", i);
        }

        spkr_prot_shared_channel_handle[i] = NULL;
      }
    }

    return_code = wcd_osal_bus_sc_dealloc_group(spkr_prot_shared_channel_group_handle);

    if (return_code != E_WCD_SUCCESS)
    {
      WCD_DEBUG_ERROR_0("=WCD_93XX: <Could not deallocated resource group.>\n");
    }

    spkr_prot_shared_channel_group_handle = NULL;

  } while(WCD_G_FALSE_BOOLEAN);

  WCD_DEBUG_INFO_1("-WCD_93XX: <Close Speaker Protection SLIMBUS shared channels. RC:%d.>\n", return_code);

  return return_code;
}

WCD_CORE_PAGED_CODE_SEGMENT_STOP


