/**************************************************************************************************************
 * Copyright © 2017 Qualcomm Technologies, Inc.
 * @file  location_stub_app.c
 * @author  Qualcomm Technologies, Inc.
 * @brief File contains the implementation for Firmware object application
 *
 **************************************************************************************************************/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "log_intf.h"
#include "qapi_lwm2m.h"
#include "location_stub_app.h"
#include "utils_app.h"

qapi_Net_LWM2M_App_Handler_t gHandler_location_stub = NULL;

/**
 * @fn location_stub_app()
 *
 * @brief This fucntion is used to initialise Location stub application.
 *
 * @param void
 * @return void
 */
void location_stub_app_init()
{  
  if (gHandler_location_stub != NULL) return ;

  if(QAPI_OK ==  qapi_Net_LWM2M_Register_App(&gHandler_location_stub))
  {
    LOG_INFO ("LWM2M_LOG: Location stub application started \n"); 
  }
}

/**
 * @fn      set_location_radius_value()
 *
 * @brief   This fucntion is used to set give radius value to the location radius resource
 *
 * @param   radius Radius value to set
 * @return  1 on Success, 
 *          0 on Failure
 */
int set_location_radius_value(double radius)
{
  qapi_Net_LWM2M_Data_t * loc_resource_data = NULL;
  qapi_Status_t result = QAPI_OK;

  if(radius < 0)
  {
    LOG_INFO("LWM2M_LOG: Enter valid Radius value\n");
    return 0;
  }

  loc_resource_data = (qapi_Net_LWM2M_Data_t *)calloc(1,sizeof(qapi_Net_LWM2M_Data_t)); 
  if (loc_resource_data == NULL) return 0;

  loc_resource_data->instance_info = (qapi_Net_LWM2M_Instance_Info_t *) calloc (1, sizeof (qapi_Net_LWM2M_Instance_Info_t));
  if (loc_resource_data->instance_info == NULL) 
  {
    app_free_data(loc_resource_data);
    return 0;
  }

  loc_resource_data->instance_info->resource_info = (qapi_Net_LWM2M_Resource_Info_t *) calloc (1, sizeof (qapi_Net_LWM2M_Resource_Info_t));
  if (loc_resource_data->instance_info->resource_info == NULL) 
  {
    app_free_data(loc_resource_data);   
    return 0;
  }

  loc_resource_data->object_ID = QAPI_NET_LWM2M_LOCATION_OBJECT_ID_E;
  loc_resource_data->no_instances = 1;
  loc_resource_data->instance_info->instance_ID = LOCATION_INSTANCE_ID_0;
  loc_resource_data->instance_info->no_resources = 1;
  loc_resource_data->instance_info->resource_info->resource_ID = QAPI_NET_LWM2M_LOCATION_RES_O_RADIUS_E;
  loc_resource_data->instance_info->resource_info->type = QAPI_NET_LWM2M_TYPE_FLOAT_E;
  loc_resource_data->instance_info->resource_info->value.asFloat = radius;

  result = qapi_Net_LWM2M_Set ( gHandler_location_stub,loc_resource_data);
  if (result == QAPI_ERROR ) {
    LOG_INFO ("qapi set failed \n");
  }
  return result;
}
