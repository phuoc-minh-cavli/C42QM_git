
/*==================================================================================
                 Copyright (c) 2017 Qualcomm Technologies, Inc.
                          All Rights Reserved.
            Qualcomm Technologies, Inc. Confidential and Proprietary.
==================================================================================*/

/*==================================================================================

                             EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file. Notice that
changes are listed in reverse chronological order.

$Header: //components/dev/core.tx/1.0/c_vsarid.core.tx.1.0.tx_011117/buses/i2c/src/drv/qapi_i2c_test.c

when       who     what, where, why
--------   ---     -----------------------------------------------------------------
04/11/17   vg     Added Poweron & Poweroff calls in testcode
03/08/17   vg     Initial version

==================================================================================*/
#include "I2cDevice.h"
#include "I2cSys.h"
#include "ddii2c.h"
#include "DALDeviceId.h"
#include "DALStdErr.h"
#include "qapi_i2c_master.h"
#include "qapi_status.h"

/*Time stamp*/
 
#include "time_svc.h"
#include "time_types.h"
//#include "Busywait.h"
#include "ULog.h"
#include "ULogFront.h"

#define I2C_TEST_NAME                  "I2C_TEST"
#define I2C_TEST_LOG_SIZE              4096
#define I2C_TEST_LOG_LEVEL_INFO        1
#define I2C_TEST_LOG_LEVEL_ALL         0xFF

#define I2C_TEST_LOG_INFO(hdl,args...) \
             ULogFront_RealTimePrintf((ULogHandle)hdl,##args); 

/*-------------------------------------------------------------------------
* Static Function Declarations and Definitions
* ----------------------------------------------------------------------*/

volatile static uint64 gcurrentTime = 0;
volatile static uint64 gprevTime = 0;
volatile uint32 wrt_result = 0x00;
volatile uint32 read_result = 0x00;

typedef struct {
    void    *log_hdl;
} i2c_test_log_type;
static i2c_test_log_type       i2c_test_log;

static uint32 i2c_test_log_level = I2C_TEST_LOG_LEVEL_ALL;

qapi_I2CM_Config_t config;
qapi_I2CM_Descriptor_t desc[2];
qapi_Status_t res = QAPI_OK;
void *client_handle = NULL;

void *i2c_test_log_create(const char *name, uint32 size)
{
   ULogHandle ulog = NULL;

   if (name != NULL) {
      ULogFront_RealTimeInit(&ulog, name, size, ULOG_MEMORY_LOCAL, ULOG_LOCK_OS);
   }
   return (void *)ulog;
}

void client_callback (uint32 i_status, void *ctxt)
 {
    //uint32 index = 0;
    
  //  if (I2C_QAPI_SUCCESS(i_status))
 //   {
    //      if(desc->transferred == desc->length)
   //       	{
   //       	 res = qapi_I2CM_Close (client_handle);
   //      	}
   // }

 }
 
qapi_Status_t i2c_test (void)
 {  
   uint32_t transferred1, transferred2;
   uint8_t buffer[4] = { 1, 2, 3, 4 };
   uint8_t reg[2] = {0x01,0x02};
   
   i2c_test_log.log_hdl = i2c_test_log_create(I2C_TEST_NAME, I2C_TEST_LOG_SIZE);
   
   // Obtain a client specific connection handle to the i2c bus instance 1
   res = qapi_I2CM_Open (QAPI_I2CM_INSTANCE_001_E, &client_handle);
    
   res = qapi_I2CM_Power_On(client_handle);

   // Configure the bus speed and slave address
   config.bus_Frequency_KHz = 400; 
   config.slave_Address     = 0x36;
   config.SMBUS_Mode        = FALSE;
   
   config.core_Configuration1 = 0;
   config.core_Configuration2 = 0;

   // <S>  - START bit
   // <P>  - STOP  bit
   // <Sr> - Repeat Start bit
   // <A>  - Acknowledge bit
   // <N>  - Not-Acknowledge bit
   // <R>  - Read Transfer
   // <W>  - Write Transfer

   // Single write transfer of N bytes
   // <S><slave_address><W><A><data1><A><data2><A>...<dataN><A><P>
   desc[0].buffer      = buffer;
   desc[0].length      = 4;
   desc[0].transferred = (uint32)&transferred1;
   desc[0].flags       = QAPI_I2C_FLAG_START | QAPI_I2C_FLAG_WRITE | QAPI_I2C_FLAG_STOP;
   
   gprevTime = time_get_ms_native();
   
   res = qapi_I2CM_Transfer (client_handle, &config, &desc[0], 1, (qapi_I2CM_Transfer_CB_t)client_callback, (void *)0x11,0);
   
   gcurrentTime = time_get_ms_native();
   wrt_result = (gcurrentTime - gprevTime);

   I2C_TEST_LOG_INFO(i2c_test_log.log_hdl, 2, " EEProm_Write taken time = 0X%X",wrt_result );
   wrt_result = 0; 
	
   // Single read transfer of N bytes
   // <S><slave_address><R><A><data1><A><data2><A>...<dataN><N><P>
   desc[0].buffer      = buffer;
   desc[0].length      = 4;
   desc[0].transferred = (uint32)&transferred1;
   desc[0].flags       = QAPI_I2C_FLAG_START | QAPI_I2C_FLAG_READ  | QAPI_I2C_FLAG_STOP;
   
   gprevTime = time_get_ms_native();
   
   res = qapi_I2CM_Transfer (client_handle, &config, &desc[0], 1, (qapi_I2CM_Transfer_CB_t)client_callback, (void *)0x12,0);
   
   gcurrentTime = time_get_ms_native();
   read_result = (gcurrentTime - gprevTime);
   I2C_TEST_LOG_INFO(i2c_test_log.log_hdl, 2, " EEProm_read taken time = 0X%X ", read_result); 
   read_result = 0;  
	
   // Read N bytes from a register 0x01 on a sensor device
   // <S><slave_address><W><A><0x01><A><S><slave_address><R><A><data1><A><data2><A>...<dataN><N><P>
   
   desc[0].buffer      = (uint8 *)&reg;
   desc[0].length      = 1;
   desc[0].transferred = (uint32)&transferred1;
   desc[0].flags       = QAPI_I2C_FLAG_START | QAPI_I2C_FLAG_WRITE;

   desc[1].buffer      = buffer;
   desc[1].length      = 4;
   desc[1].transferred = (uint32)&transferred2;
   desc[1].flags       = QAPI_I2C_FLAG_START | QAPI_I2C_FLAG_READ  | QAPI_I2C_FLAG_STOP;
   res = qapi_I2CM_Transfer ((DalDeviceHandle *)client_handle, &config, &desc[0], 2, (qapi_I2CM_Transfer_CB_t)client_callback, (void *)0x13,0);

   // Read N bytes from eeprom address 0x0102
   // <S><slave_address><W><A><0x01><A><0x02><A><S><slave_address><R><A><data1><A><data2><A>...<dataN><N><P>
   //reg[2] = { 0x01, 0x02 };
   desc[0].buffer      = (uint8 *)reg;
   desc[0].length      = 2;
   desc[0].transferred = (uint32)&transferred1;
   desc[0].flags       = QAPI_I2C_FLAG_START | QAPI_I2C_FLAG_WRITE;

   desc[1].buffer      = buffer;
   desc[1].length      = 4;
   desc[1].transferred = (uint32) &transferred2;
   desc[1].flags       = QAPI_I2C_FLAG_START | QAPI_I2C_FLAG_READ  | QAPI_I2C_FLAG_STOP;
   res = qapi_I2CM_Transfer (client_handle, &config, &desc[0], 2, (qapi_I2CM_Transfer_CB_t)client_callback, (void *)0x14,0);
  
  //res = qapi_I2CM_Power_On(client_handle);
   // Close the connection handle to the i2c bus instance
//   res = qapi_I2CM_Close (client_handle);

   return res;
 }

 uint32 test_variable = 0x21;
 
 void i2c_test_task(void)
 {
 
		if (test_variable)
		{
			i2c_test();
		}
}
	 


