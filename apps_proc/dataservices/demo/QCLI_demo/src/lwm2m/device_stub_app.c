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
#include "log_intf.h"
#include "qapi_lwm2m.h"
#include "device_stub_app.h"
#include "utils_app.h"

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
  if (gHandler_device_stub != NULL)
  {
    return;
  }

  if(QAPI_OK ==  qapi_Net_LWM2M_Register_App(&gHandler_device_stub))
  {
    LOG_INFO ("LWM2M_LOG: Device stub application started \n"); 
  }
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

  device_resource_data = (qapi_Net_LWM2M_Data_t *)malloc(sizeof(qapi_Net_LWM2M_Data_t));

  if(device_resource_data == NULL) 
  {
    return QAPI_ERR_NO_MEMORY;
  }

  memset(device_resource_data, 0, sizeof(qapi_Net_LWM2M_Data_t));

  device_resource_data->instance_info = (qapi_Net_LWM2M_Instance_Info_t *)malloc(sizeof (qapi_Net_LWM2M_Instance_Info_t));

  if (device_resource_data->instance_info == NULL) 
  {
    app_free_data(device_resource_data);
    return QAPI_ERR_NO_MEMORY;
  }

  memset(device_resource_data->instance_info, 0, sizeof (qapi_Net_LWM2M_Instance_Info_t));

  device_resource_data->instance_info->resource_info = (qapi_Net_LWM2M_Resource_Info_t *)malloc(sizeof (qapi_Net_LWM2M_Resource_Info_t));

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
  device_resource_data->instance_info->resource_info->value.asBuffer.buffer = (uint8_t *)malloc(len);

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

