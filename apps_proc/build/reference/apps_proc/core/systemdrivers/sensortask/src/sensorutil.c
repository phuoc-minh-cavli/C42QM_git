/*=============================================================================
  Copyright (c) 2016  by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/

#include <comdef.h>
#include <stdio.h>
#include <stdlib.h>

#include "sensorutil.h"
#include "qurt_error.h"
#include "qurt_thread.h"
#include "qurt_timer.h"
#include "ULogFront.h"

#include "DALDeviceId.h"
#include "DalDevice.h"
#include "DALSys.h"
#include "DALSysCmn.h"
#include "ddii2c.h"
#include "I2cTransfer.h"
#include "DDITlmm.h"
#include "DDIGPIOInt.h"
#include "dsr.h"

#define MAX_DATA_SIZE  							2

typedef struct {
    dsr_obj_t *sensorutil_gpio_dsr_obj;
    DalDeviceHandle    *h_tlmm;
    DalDeviceHandle    *h_gpio_int;
    uint32  gpio;
    DALGpioSignalType  pin_config;
    GPIOIntTriggerType trigger;
    sensorutil_gpio_event_handler_type event_handler;
} sensorutil_gpio_interrupt_handle_type;

typedef struct {
    DalDeviceHandle *i2cHandle;
    I2cTransfer     transferWrite;
    I2cTransfer     transferRead;
    I2cClientConfig clntCfg;
} sensorutil_i2c_handle_type;

void sensorutil_gpio_isr_cb(void *payload) {
    boolean dsr_status;
    DALResult result = DAL_SUCCESS;

    sensorutil_gpio_interrupt_handle_type *handle = (sensorutil_gpio_interrupt_handle_type *)payload;

    dsr_status = dsr_enqueue(handle->sensorutil_gpio_dsr_obj, (void *)handle);

    if (!dsr_status || (result != DAL_SUCCESS)) {
        // error
        return;
    }
}

void sensorutil_gpio_dsr_cb(dsr_obj_t *dsr_obj, void *ctxt, void *payload) {
    sensorutil_gpio_interrupt_handle_type *handle = (sensorutil_gpio_interrupt_handle_type *)payload;
    DALResult result = DAL_SUCCESS;

    result = GPIOInt_DeregisterIsr(handle->h_gpio_int, (uint32)handle->gpio, (GPIOINTISR)sensorutil_gpio_isr_cb);

    if (handle && handle->event_handler) {
        handle->event_handler();

        result = GPIOInt_RegisterIsr(handle->h_gpio_int, handle->gpio, handle->trigger, (GPIOINTISR)sensorutil_gpio_isr_cb, (uint32)handle);
    }

    if (result != DAL_SUCCESS) {
        // error
        return;
    }
}

int sensorutil_register_gpio_interrupt(int gpio, sensorutil_gpio_int_trigger_type trigger, sensorutil_gpio_event_handler_type event_handler) {

    DALResult result = DAL_SUCCESS;
    boolean dsr_status = TRUE;
    sensorutil_gpio_interrupt_handle_type* handle = 
        (sensorutil_gpio_interrupt_handle_type*)malloc(sizeof(sensorutil_gpio_interrupt_handle_type));

    handle->gpio = (uint32)gpio;

    switch (trigger) {
    case GPIO_INT_TRIGGER_HIGH:
        handle->trigger = GPIOINT_TRIGGER_HIGH; 
        break;
    case GPIO_INT_TRIGGER_LOW: 
        handle->trigger = GPIOINT_TRIGGER_LOW; 
        break;
    case GPIO_INT_TRIGGER_RISING: 
        handle->trigger = GPIOINT_TRIGGER_RISING; 
        break;
    case GPIO_INT_TRIGGER_FALLING: 
        handle->trigger = GPIOINT_TRIGGER_FALLING; 
        break;
    case GPIO_INT_TRIGGER_DUAL_EDGE:
        handle->trigger = GPIOINT_TRIGGER_DUAL_EDGE; 
        break;
    default:
        // unsupported trigger
        return DAL_ERROR;
    }

    handle->event_handler = event_handler;

    result = DAL_DeviceAttach(DALDEVICEID_TLMM, &handle->h_tlmm);

    result = DAL_DeviceAttach(DALDEVICEID_GPIOINT, &handle->h_gpio_int);

    handle->pin_config = DAL_GPIO_CFG(handle->gpio, 0, DAL_GPIO_INPUT, DAL_GPIO_PULL_UP, DAL_GPIO_2MA);

    result = DalTlmm_ConfigGpio(handle->h_tlmm, handle->pin_config, DAL_TLMM_GPIO_ENABLE);

    dsr_status = dsr_create(&handle->sensorutil_gpio_dsr_obj, (dsr_hdlr_t)sensorutil_gpio_dsr_cb, NULL, DSR_PRI_HIGH, DSR_CONTEXT_ISR);
    if (!dsr_status) {
        return DAL_ERROR;
    }

    // Ensure this is the last statement in the function as ISR could get triggered
    // right away, and handle object may not be fully populated
    result = GPIOInt_RegisterIsr(handle->h_gpio_int, handle->gpio, handle->trigger, (GPIOINTISR)sensorutil_gpio_isr_cb, (uint32)handle);

    return result;
}

int sensorutil_i2c_init_handle(void **ctx, uint8 slave_addr) {
    DALResult result = DAL_SUCCESS;
    sensorutil_i2c_handle_type* handle = (sensorutil_i2c_handle_type*)malloc(sizeof(sensorutil_i2c_handle_type));

    if (!handle) {
        return DAL_ERROR_INVALID_POINTER;
    }
    *(sensorutil_i2c_handle_type **)ctx = handle; 

    handle->clntCfg.uByteTransferTimeoutUs    = 2500;
    handle->clntCfg.uBusFreqKhz			      = 100;

    handle->transferWrite.tranCfg.uSlaveAddr  = slave_addr;
    handle->transferWrite.eTranDirection	  = I2cTranDirOut;
    handle->transferWrite.eTranCtxt		   	  = I2cTrCtxNotASequence;

    handle->transferRead.tranCfg.uSlaveAddr	  = slave_addr;
    handle->transferRead.eTranDirection		  = I2cTranDirIn;
    handle->transferRead.eTranCtxt			  = I2cTrCtxNotASequence;

    /*Attach*/
    result = DAL_I2CDeviceAttach(DALDEVICEID_I2C_DEVICE_4, &handle->i2cHandle);
    if (DAL_SUCCESS == result) {
        result = DalDevice_Open(handle->i2cHandle, DAL_OPEN_SHARED);
    }

    return result;
}

int sensorutil_i2c_write_nodata(void *ctx, uint8 offset) {
    DALResult     result           			   = DAL_SUCCESS;
    uint32        u32NumCompleted  			   = 0;
    unsigned char i2c_write_buf[MAX_DATA_SIZE] = { 0, 0 };
    I2cBuffDesc   clientWriteBuff;
    sensorutil_i2c_handle_type* handle = ctx;

    if (!handle) {
        return DAL_ERROR_INVALID_POINTER;
    }

    i2c_write_buf[0] = offset;
    clientWriteBuff.pBuff	  	  = i2c_write_buf;    // Write offset
    clientWriteBuff.uBuffSize 	  = 1;
    handle->transferWrite.pI2cBuffDesc    = &clientWriteBuff;
    handle->transferWrite.uTrSize   	  = 1;

    result = DalI2C_Write(handle->i2cHandle, &handle->transferWrite, &handle->clntCfg, &u32NumCompleted);

    return result;
}

int sensorutil_i2c_write(void *ctx, uint8 offset, uint8 data) {
    DALResult     result           			   = DAL_SUCCESS;
    uint32        u32NumCompleted  			   = 0;
    unsigned char i2c_write_buf[MAX_DATA_SIZE] = { 0, 0 };
    I2cBuffDesc   clientWriteBuff;
    sensorutil_i2c_handle_type* handle = ctx;

    if (!handle) {
        return DAL_ERROR_INVALID_POINTER;
    }

    i2c_write_buf[0] = offset;
    i2c_write_buf[1] = data;
    clientWriteBuff.pBuff	  	  = i2c_write_buf;    // Write offset and Data
    clientWriteBuff.uBuffSize 	  = 2;
    handle->transferWrite.pI2cBuffDesc    = &clientWriteBuff;
    handle->transferWrite.uTrSize   	  = 2;

    result = DalI2C_Write(handle->i2cHandle, &handle->transferWrite, &handle->clntCfg, &u32NumCompleted);

    return result;
}

int sensorutil_i2c_read(void *ctx, uint8 offset, uint8 *pData) {
    DALResult result               = DAL_SUCCESS;
    uint32 u32NumCompleted     	   = 0;
    I2cBuffDesc clientReadBuff;
    I2cBuffDesc clientWriteBuff;
    unsigned char i2c_read_buf[MAX_DATA_SIZE] = { 0, 0 };
    unsigned char i2c_write_buf[MAX_DATA_SIZE] = { 0, 0 };
    sensorutil_i2c_handle_type* handle = ctx;

    if (!handle) {
        return DAL_ERROR_INVALID_POINTER;
    }

    i2c_write_buf[0] = offset;
    clientWriteBuff.pBuff	  	  			   = i2c_write_buf;    // Write offset
    clientWriteBuff.uBuffSize 	 			   = 1;
    handle->transferWrite.pI2cBuffDesc    			   = &clientWriteBuff;
    handle->transferWrite.uTrSize   	  			   = 1;

    result = DalI2C_Write(handle->i2cHandle, &handle->transferWrite, &handle->clntCfg, &u32NumCompleted);
    if (result != DAL_SUCCESS) {
        return result;
    }

    u32NumCompleted           = 0; // reset num read
    i2c_read_buf[0] = 0; // reset buffer - only 1 byte read
    clientReadBuff.pBuff	  = i2c_read_buf;
    clientReadBuff.uBuffSize  = 1;
    handle->transferRead.pI2cBuffDesc = &clientReadBuff;
    handle->transferRead.uTrSize	  = 1;

    result = DalI2C_Read(handle->i2cHandle, &handle->transferRead, &handle->clntCfg, &u32NumCompleted);
    if (result != DAL_SUCCESS) {
        return result;
    }

    *pData = i2c_read_buf[0];

    return result;
}

int sensorutil_gpio_set_value(int gpio_num, unsigned char value){
    int nErr = DAL_SUCCESS;
    DALDEVICEHANDLE hTlmm;

    nErr = DAL_DeviceAttach(DALDEVICEID_TLMM,&hTlmm);
    if (nErr != DAL_SUCCESS)
        return nErr;

    nErr = DalDevice_Open(hTlmm, DAL_OPEN_SHARED);
    if (nErr != DAL_SUCCESS)
        return nErr;

    nErr = DalTlmm_ConfigGpio(hTlmm,DAL_GPIO_CFG( gpio_num, 0, DAL_GPIO_OUTPUT,
                                                  DAL_GPIO_NO_PULL, DAL_GPIO_2MA ),
                                    DAL_TLMM_GPIO_ENABLE );
    if (nErr != DAL_SUCCESS)
        return nErr;

    nErr = DalTlmm_GpioOut(hTlmm,   DAL_GPIO_CFG( gpio_num, 0, DAL_GPIO_OUTPUT,
                                                  DAL_GPIO_NO_PULL, DAL_GPIO_2MA ),
                                    value == 0 ? DAL_GPIO_LOW_VALUE:DAL_GPIO_HIGH_VALUE);
    if (nErr != DAL_SUCCESS)
        return nErr; 

    return nErr;
}

