/**
@file wcd_osal.c
@brief 
  os abstraction layer for WCD driver
*/
/*-------------------------------------------------------------------------------
     Copyright (c) 2011-2015 QUALCOMM Technologies Incorporated.
     All rights reserved.
     Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
                             Edit History
$Id: //components/rel/audio.tx/2.0/audio/adie/bus_manager/src/wcd_osal_bus_specific.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/28/11   SK      initial version.
===============================================================================*/
#include "wcd_osal.h"
#include "DALDeviceId.h"
#include "ddii2c.h"
#include "bus_manager_target.h"
#include "bus_manager.h"

DalDeviceHandle* dal_bus_handle = NULL;
static I2cClientConfig     I2C_client_config;
uint8 device_I2C_Address_pool[WCD_I2C_NUM_CORES];

/**
 * \brief Initializes the bus to be used. This function will only be called once (unless deinit is called).
 *
 * @return: E_WCD_SUCCESS or E_WCD_BUS_ERROR
 */
wcd_result wcd_osal_initialize_bus( wcd_bus_type bus_type )
{
  DALResult dal_rc = DAL_SUCCESS;
  wcd_result return_code = E_WCD_SUCCESS;
  uint8* enumeration_address;
  uint8 enumeration_address_length;
    
  if (bus_type == E_WCD_I2C)
  {
    WCD_DEBUG_INFO_0("=WCD_BUS_MGR: <Initializing I2C.>\n");
          
    I2C_client_config.uBusFreqKhz = WCD_I2C_BUS_SPEED;
    I2C_client_config.uByteTransferTimeoutUs = WCD_TRANSFER_TIMEOUT;
          
    dal_rc = DAL_I2CDeviceAttach( WCD_I2C_BUS_DAL_DEVICE, &dal_bus_handle);
    if( DAL_SUCCESS == dal_rc )
    {
      device_I2C_Address_pool[0] = WCD_I2C_ADDRESS;
      //if the connection is correct then read the address of the other devices
      return_code = wcd_activate_bus();
    } // end device attach
    else
    {
      WCD_DEBUG_ERROR_1("=WCD_BUS_MGR: <Failed DAL_I2CDeviceAttach, rc = %d.>\n",dal_rc);
    }


    if (dal_rc == DAL_SUCCESS && return_code == E_WCD_SUCCESS)
    {
      int32 i = 0;
      wcd_set_enumeration_address(bus_type, 0 , &enumeration_address, &enumeration_address_length);
      for( i = 0; i < enumeration_address_length; i++ )
      {
        return_code = wcd_register_read(E_WCD_CODEC_MAIN, enumeration_address[i], 0xFF, 0, &device_I2C_Address_pool[i+1]);
        if( return_code != E_WCD_SUCCESS )
        {
          break;
        }
      }//end for
      return_code = wcd_deactivate_bus();
    }
  }

  return return_code;
}

/**
 * \brief Deinitializes the bus that was used. This function will only be called once (unless init is called).
 *
 * @return: E_WCD_SUCCESS or E_WCD_BUS_ERROR
 */
wcd_result wcd_osal_deinitialize_bus( void )
{
  if (dal_bus_handle)
  {
    DAL_DeviceDetach(dal_bus_handle);
    dal_bus_handle = NULL;
  }
  
  return E_WCD_SUCCESS;
}

static uint32 wcd_bus_get_i2c_slave_address(uint16 reg)
{
  uint32 index = (reg & WCD_I2C_DEVICE_ID_MASK )>> WCD_I2C_DEVICE_ID_SHIFT;
  uint32 slave_add = WCD_I2C_INVALID;
  if(index < WCD_I2C_NUM_CORES)
  {
    slave_add = device_I2C_Address_pool[index];
  }
  return slave_add;
}

/**
* \brief A bus repeat write is performed.
* This will only be called after the bus is properly initialized and activated.
*
* @param reg : Register to write
* @param value : Value to write to the register
* @param bus_interface : The bus interface, either CODEC, INTERFACE or AHB
* @param force_write : Indicates if the write can be buffered or if it needs to occur immediately
*                      [FALSE] = Write can be buffered
*                      [TRUE] = Write needs to occur immediately
* @note : If force_write is set to true, it may flush out all previous buffered registers, which is acceptable
*
* @return: E_WCD_SUCCESS or E_WCD_BUS_ERROR
*/
wcd_result wcd_osal_bus_write_repeat(uint16 reg, uint8* values, uint16 size, wcd_ctrl_bus_interface bus_interface, wcd_bool force_write)
{
	wcd_result return_code = E_WCD_SUCCESS;

	HANDLE_UNUSED_PARAMETER(reg);
	HANDLE_UNUSED_PARAMETER(size);
	HANDLE_UNUSED_PARAMETER(force_write);
	HANDLE_UNUSED_PARAMETER(values);
	HANDLE_UNUSED_PARAMETER(bus_interface);

	return return_code;
}

/**
 * \brief A bus write is performed. 
 * This will only be called after the bus is properly initialized and activated.
 *
 * @param reg : Register to write
 * @param value : Value to write to the register
 * @param bus_interface : The bus interface, either CODEC, INTERFACE or AHB
 * @param force_write : Indicates if the write can be buffered or if it needs to occur immediately
 *                      [FALSE] = Write can be buffered
 *                      [TRUE] = Write needs to occur immediately
 * @note : If force_write is set to true, it may flush out all previous buffered registers, which is acceptable
 *
 * @return: E_WCD_SUCCESS or E_WCD_BUS_ERROR
 */
wcd_result wcd_osal_bus_write( uint16 reg, uint8 value, wcd_ctrl_bus_interface bus_interface, wcd_bool force_write )
{
  uint32              uNumCompleted;
  I2cBuffDesc         iovecs[2];
  uint8               aOffsetBuff[2];// a[15-8],a[7-0],  data[0-63]
  I2cTransfer         transfer;
  
  wcd_result return_code = E_WCD_BUS_ERROR;
  DALResult dal_rc = DAL_ERROR;

  aOffsetBuff[0] = (uint8) reg;
  iovecs[0].pBuff = aOffsetBuff;
  iovecs[0].uBuffSize = 1;

  iovecs[1].pBuff = &value;
  iovecs[1].uBuffSize = 1;

  transfer.pI2cBuffDesc = iovecs;
  transfer.eTranDirection = I2cTranDirOut;
  transfer.uTrSize = 2;
  transfer.tranCfg.uSlaveAddr = wcd_bus_get_i2c_slave_address(reg);
  transfer.eTranCtxt = I2cTrCtxNotASequence;
  dal_rc = DalI2C_Write(dal_bus_handle, &transfer, &I2C_client_config ,&uNumCompleted);
  
  if (dal_rc == DAL_SUCCESS)
  {
    return_code = E_WCD_SUCCESS;
  }
  else
  {
    WCD_DEBUG_ERROR_1("=WCD_BUS_MGR: <Failed I2C write, rc = %d.>\n",dal_rc);
  }
  
  return return_code;
}


/**
 * \brief A bus read is performed. 
 * This will only be called after the bus is properly initialized and activated.
 *
 * @param reg : Register to read
 * @param value : Buffer to store the read value to
 * @param bus_interface : The bus interface, either CODEC, INTERFACE or AHB
 *
 * @note: All register reads need to occur immediately.
 *
 * @return: E_WCD_SUCCESS or E_WCD_BUS_ERROR
 */
wcd_result wcd_osal_bus_read( uint16 reg, uint8* value, wcd_ctrl_bus_interface bus_interface )
{
  I2cIoResult         sres;
  I2cTransfer         rdTrans[2];
  I2cBuffDesc         iovecs[2];
  I2cSequence         rdSeq;
  uint8               aData[1];// a[7-0]
  
  wcd_result return_code = E_WCD_BUS_ERROR;
  DALResult dal_rc = DAL_ERROR;

  aData[0] = (uint8) (reg);//a[7-0] //Make sure address is a byte long

  iovecs[0].pBuff = aData;
  iovecs[0].uBuffSize = 1;

  // first transfer sets the offset.
  rdTrans[0].pI2cBuffDesc = &iovecs[0];
  rdTrans[0].eTranDirection = I2cTranDirOut;
  rdTrans[0].uTrSize = 1;
  rdTrans[0].tranCfg.uSlaveAddr = wcd_bus_get_i2c_slave_address(reg);
  rdTrans[0].eTranCtxt = I2cTrCtxSeqStart;
  // second transfer reads the data.
  iovecs[1].pBuff           = value;
  iovecs[1].uBuffSize       = 1;
  rdTrans[1].pI2cBuffDesc   = &iovecs[1];
  rdTrans[1].eTranDirection = I2cTranDirIn;
  rdTrans[1].uTrSize        = 1;
  rdTrans[1].tranCfg.uSlaveAddr = wcd_bus_get_i2c_slave_address(reg);
  rdTrans[1].eTranCtxt = I2cTrCtxSeqEnd;

  rdSeq.pTransfer     = &rdTrans[0];
  rdSeq.uNumTransfers = 2;
  dal_rc = DalI2C_BatchTransfer(dal_bus_handle, &rdSeq, &I2C_client_config ,&sres);

  if (dal_rc == DAL_SUCCESS)
  {
    return_code = E_WCD_SUCCESS;
  }
  else
  {
    WCD_DEBUG_ERROR_1("=WCD_BUS_MGR: <Failed I2C read, rc = %d.>\n",dal_rc);
  }
  
  return return_code;
}

/**
 * \brief Sets the logical address of the codec (pgd) and slimbus interface.
 *
 * @return: E_WCD_SUCCESS or E_WCD_BUS_ERROR
 */
wcd_result wcd_osal_bus_get_logical_address( uint8* logical_address_pgd, uint8* logical_address_intf )
{
  HANDLE_UNUSED_PARAMETER(logical_address_pgd);
  HANDLE_UNUSED_PARAMETER(logical_address_intf);

  return E_WCD_BUS_ERROR;
}

/**
 * \brief Activates the bus to perform register writes/reads.
 * This will only be called after the bus is properly initialized. 
 * This will only be called once until bus_deactivate is called.
 *
 * @return: E_WCD_SUCCESS or E_WCD_BUS_ERROR
 */
wcd_result wcd_osal_bus_activate(void)
{
  DALResult dal_rc      = DAL_SUCCESS;
  
  if ( dal_bus_handle != NULL )
  {
    dal_rc = DalDevice_Open( dal_bus_handle, DAL_OPEN_SHARED );
  }
  
  if ( dal_rc != DAL_SUCCESS )
  {
    WCD_DEBUG_ERROR_1("=WCD_BUS_MGR: <Failed DalDeviceOpen, rc = %d.>\n",dal_rc);
    return E_WCD_BUS_ERROR;
  }
  
  return E_WCD_SUCCESS;
}

/**
 * \brief Deactivates the bus to prevent register writes/reads (for power saving)
 * This will only be called once after the bus is activated. 
 *
 * @return: E_WCD_SUCCESS or E_WCD_BUS_ERROR
 */
wcd_result wcd_osal_bus_deactivate(void)
{
  DALResult dal_rc      = DAL_SUCCESS;
  
  if ( dal_bus_handle != NULL )
  {
    dal_rc = DalDevice_Close( dal_bus_handle );
  }
  
  if ( dal_rc != DAL_SUCCESS )
  {
    WCD_DEBUG_ERROR_1("=WCD_BUS_MGR: <Failed DalDevice_Close, rc = %d.>\n",dal_rc);
    return E_WCD_BUS_ERROR;
  }
  
  return E_WCD_SUCCESS;
}

/**
 * \brief Allocates a resource group to be use for any shared channel processing
 * The group will be assigned in the parameter group_handle.
 *
 * @return: E_WCD_SUCCESS or E_WCD_BUS_ERROR
 */
wcd_result wcd_osal_bus_sc_alloc_group(void** group_handle)
{
  HANDLE_UNUSED_PARAMETER(group_handle);
  
  return E_WCD_SUCCESS;
}

/**
 * \brief This should allocate the shared data channel, add the channel to the group
 * allocated in wcd_osal_bus_sc_alloc_group, get the port handle and finally connect the
 * port to the channel. The shared data channel should also be returned in the 
 * parameter data_channel
 *
 * @param port: This will include the RX/TX offset.
 *
 * @return: E_WCD_SUCCESS or E_WCD_BUS_ERROR
 */
wcd_result wcd_osal_bus_sc_connect_port_and_channel(uint32 port, uint32 channel, 
    void* group_handle, void** data_channel, wcd_slimbus_direction direction )
{
  HANDLE_UNUSED_PARAMETER(port);
  HANDLE_UNUSED_PARAMETER(channel);
  HANDLE_UNUSED_PARAMETER(group_handle);
  HANDLE_UNUSED_PARAMETER(data_channel);
  HANDLE_UNUSED_PARAMETER(direction);
  
  return E_WCD_SUCCESS;
}

/**
 * \brief Enables the shared channel using the group allocated in wcd_osal_bus_sc_alloc_group
 * with the specified sample rate and bitwidth
 *
 * @return: E_WCD_SUCCESS or E_WCD_BUS_ERROR
 */
wcd_result wcd_osal_bus_sc_enable(uint32 sample_rate, uint32 bit_width, void* group_handle)
{
  HANDLE_UNUSED_PARAMETER(sample_rate);
  HANDLE_UNUSED_PARAMETER(bit_width);
  HANDLE_UNUSED_PARAMETER(group_handle);
  
  return E_WCD_SUCCESS;
}

/**
 * \brief Disables the shared channel using the group allocated in wcd_osal_bus_sc_alloc_group
 * with the specified sample rate and bitwidth
 *
 * @return: E_WCD_SUCCESS or E_WCD_BUS_ERROR
 */
wcd_result wcd_osal_bus_sc_disable(void* group_handle)
{
  HANDLE_UNUSED_PARAMETER(group_handle);
  
  return E_WCD_SUCCESS;
}

/**
 * \brief Deallocates the shared data channel.
 *
 * @return: E_WCD_SUCCESS or E_WCD_BUS_ERROR
 */
wcd_result wcd_osal_bus_sc_dealloc_channel(void* channel_handle)
{
  HANDLE_UNUSED_PARAMETER(channel_handle);
  
  return E_WCD_SUCCESS;
}

/**
 * \brief Deallocates the shared channel group
 *
 * @return: E_WCD_SUCCESS or E_WCD_BUS_ERROR
 */
wcd_result wcd_osal_bus_sc_dealloc_group(void* group_handle)
{
  HANDLE_UNUSED_PARAMETER(group_handle);
  
  return E_WCD_SUCCESS;
}

void split_packet_to_valid_byte_writes(uint16 arrSize, uint8* outputArray, uint8* count)
{
  HANDLE_UNUSED_PARAMETER(arrSize);
  HANDLE_UNUSED_PARAMETER(outputArray);
  HANDLE_UNUSED_PARAMETER(count);
}



