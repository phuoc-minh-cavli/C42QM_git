/**************************************************************************************************************
 * Copyright © 2017 Qualcomm Technologies, Inc.
 * @file    device_stub_app.c
 * @author  Qualcomm Technologies, Inc.
 * @brief   File contains the implementation for Device object application
 *
 **************************************************************************************************************/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "log_intf_module.h"
#include "qapi_lwm2m.h"
#include "device_stub_app.h"
#include "qapi_socket.h"
#include "utils_app.h"

extern TX_BYTE_POOL *byte_pool_qcli;

qapi_Net_LWM2M_App_Handler_t gHandler_device_stub = NULL;

/**
 * @fn device_stub_app()
 *
 * @brief This fucntion is used to initialise Device stub application.
 *
 * @param void
 * @return void
 */
void device_stub_app_init()
{
  void *uspace_memory_lwm2m = NULL;

  if (gHandler_device_stub != NULL)
  {
    return;
  }

  if(QAPI_OK ==  qapi_Net_LWM2M_Register_App(&gHandler_device_stub))
  {
    LOG_INFO ("LWM2M_LOG: Device stub application started \n"); 
  }

  tx_byte_allocate(byte_pool_qcli, (VOID **) &uspace_memory_lwm2m,2048, TX_NO_WAIT);
  if(uspace_memory_lwm2m == NULL)
  {
    LOG_INFO("Cannot assign memory for uspace_memory_lwm2m_ext");
    return;
  }

    qapi_Net_LWM2M_Pass_Pool_Ptr(gHandler_device_stub, byte_pool_qcli);

}

/**
 * @fn      set_device_resource_value()
 *
 * @brief   This function is used to set given resource value to the appropriate resource
 *
 * @param   resource_id Resource whose value need to be updated 
 * @param   buf         Value need to be set 
 * @param   len         Length of resource value 
 *
 * @return  QAPI_OK on Success,
 *          QAPI_ERROR on Failure
 *          QAPI_ERR_INVALID_PARAM on Invalid parameters provided.
 *          QAPI_ERR_NO_MEMORY on Memory allocation failure.
 */
int set_device_resource_value(int resource_id, char* buf, int len)
{
  qapi_Net_LWM2M_Data_t *device_resource_data = NULL;
  qapi_Status_t result = QAPI_ERROR;

  if(gHandler_device_stub == NULL)
  {
    LOG_INFO("LWM2M_LOG: Device stub application not started.\n");
    return QAPI_ERROR;
  }

  if(buf == NULL || len <= 0)
  {
    LOG_INFO("LWM2M_LOG: Not valid resource value passed.\n");
    return QAPI_ERR_INVALID_PARAM;
  }

  tx_byte_allocate(byte_pool_qcli, (VOID **)&device_resource_data, sizeof(qapi_Net_LWM2M_Data_t), TX_NO_WAIT);		    

  if(device_resource_data == NULL) 
  {
    return QAPI_ERR_NO_MEMORY;
  }

  memset(device_resource_data, 0, sizeof(qapi_Net_LWM2M_Data_t));

  tx_byte_allocate(byte_pool_qcli, (VOID **)&(device_resource_data->instance_info), sizeof(qapi_Net_LWM2M_Instance_Info_t), TX_NO_WAIT);		    

  if (device_resource_data->instance_info == NULL) 
  {
    app_free_data(device_resource_data);
    return QAPI_ERR_NO_MEMORY;
  }

  memset(device_resource_data->instance_info, 0, sizeof (qapi_Net_LWM2M_Instance_Info_t));

  tx_byte_allocate(byte_pool_qcli, (VOID **)&(device_resource_data->instance_info->resource_info), sizeof(qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);		    

  if (device_resource_data->instance_info->resource_info == NULL) 
  {
    app_free_data(device_resource_data);
    return QAPI_ERR_NO_MEMORY;
  }

  memset(device_resource_data->instance_info->resource_info, 0, sizeof (qapi_Net_LWM2M_Resource_Info_t));

  device_resource_data->object_ID = QAPI_NET_LWM2M_DEVICE_OBJECT_ID_E;
  device_resource_data->no_instances = 1;
  device_resource_data->instance_info->instance_ID = DEVICE_INSTANCE_ID_0;
  device_resource_data->instance_info->no_resources = 1;
  device_resource_data->instance_info->resource_info->resource_ID = resource_id;
  device_resource_data->instance_info->resource_info->type = QAPI_NET_LWM2M_TYPE_STRING_E;
  device_resource_data->instance_info->resource_info->value.asBuffer.length = len;
  tx_byte_allocate(byte_pool_qcli, (VOID **)&(device_resource_data->instance_info->resource_info->value.asBuffer.buffer), len, TX_NO_WAIT);		    

  if(device_resource_data->instance_info->resource_info->value.asBuffer.buffer == NULL)
  {
    app_free_data(device_resource_data);
    return QAPI_ERR_NO_MEMORY;
  }

  memset(device_resource_data->instance_info->resource_info->value.asBuffer.buffer, 0, len);

  memcpy((void *)device_resource_data->instance_info->resource_info->value.asBuffer.buffer,
      (const void*)buf,len);

  result = qapi_Net_LWM2M_Set(gHandler_device_stub,device_resource_data);

  if (result == QAPI_ERROR )
  {
    LOG_INFO ("Failed to set resource\n");
  }

  app_free_data(device_resource_data);

  return result;
}

/**
 * @fn    device_app_deregister()
 * @brief This fucntion is used to deregister device object application.
 * @param void
 * @return void
 */
void device_app_deregister()
{
  qapi_Status_t result = QAPI_OK;

  if(gHandler_device_stub)
  {
    result = qapi_Net_LWM2M_DeRegister_App(gHandler_device_stub);

    if (result == QAPI_ERROR)
    {
      LOG_INFO ("LWM2M_LOG : device stub handler is not valid \n");
    }
    gHandler_device_stub = NULL;
  }
  else
  {
    LOG_INFO("Handler is not valid\n");
  }
  return;
}
