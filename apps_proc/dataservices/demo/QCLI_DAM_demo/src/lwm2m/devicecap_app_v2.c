/**************************************************************************************************************
 * Copyright © 2018 Qualcomm Technologies, Inc.
 * @file  devicecap_app_v2.c
 * @author  Qualcomm Technologies, Inc.
 * @brief File contains the implementation for device capability object application
 *
 **************************************************************************************************************/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "fs_public.h"
#include "log_intf_module.h"
#include "qapi_lwm2m.h"
#include "devicecap_stub_app_v2.h"
#include "qapi_socket.h"
#include "utils_app_v2.h"

#define QAPI_NET_LWM2M_DEVICE_CAP_INSTANCE_ID_0  0
#define PROPERTY "0;1;10"
#define DESCRIPTION "Sensor group"
#define MAX_INSTANCE_ID 20
int inst_index_v2 = 0;
int instance_created_v2[MAX_INSTANCE_ID] = {0};
int observe_mode_v2[MAX_INSTANCE_ID] = {0};

extern TX_BYTE_POOL *byte_pool_qcli;

//Application handler
qapi_Net_LWM2M_App_Handler_t handler_dev_v2;


/**
 * @fn devicecap_app_callback_v2()
 *
 * @brief This callback fucntion is called when observe operation is performed 
 *        on device capability object resource.
 * @param handler LWM2M application handler
 * @param lwm2m_data  Device Capability object data
 * @return QAPI_OK on success
 *         QAPI_ERROR on failure 
 */
void  devicecap_app_callback_v2( qapi_Net_LWM2M_App_Handler_t handler, qapi_Net_LWM2M_Data_v2_t * lwm2m_data, qapi_Status_t *result)
{
  qapi_Net_LWM2M_Data_v2_t *lwm2m_dataP = NULL;
  qapi_Status_t res = QAPI_ERROR;

  if((!lwm2m_data) || (!handler))
  {
    LOG_INFO("Input param is NULL\n");
    *result = QAPI_ERROR;
    return;    
  }

  if(handler != handler_dev_v2)
  {
    LOG_INFO("Not a device cap handler\n");  
    *result = QAPI_ERROR;
    return;
  }      

  if((NULL == lwm2m_data->instance_info) || (NULL == lwm2m_data->instance_info->resource_info))
  {
    LOG_INFO("Instance info or resource info data is not recieved\n");
    *result = QAPI_ERROR;
    return;
  }

  LOG_DEBUG("App callback is invoked DATA is --> :\n");   
  LOG_DEBUG ("LWM2M_LOG: object_ID ----> %d\n",lwm2m_data->object_ID );
  LOG_DEBUG ("LWM2M_LOG: no of instances %d\n ",lwm2m_data->no_instances);
  LOG_DEBUG ("LWM2M_LOG:  instance id ----> %d \n",lwm2m_data->instance_info->instance_ID);
  LOG_DEBUG ("LWM2M_LOG: no of resources %d\n",lwm2m_data->instance_info->no_resources);
  LOG_DEBUG ("LWM2M_LOG: resouces id -----> %d \n",lwm2m_data->instance_info->resource_info->resource_ID);

  switch (lwm2m_data->instance_info->resource_info->type) 
  {
    case QAPI_NET_LWM2M_TYPE_STRING_E :
      if (lwm2m_data->instance_info->resource_info->value.asBuffer.length ) {
        LOG_DEBUG ("LWM2M_LOG: resource value is length %d\t and %s\n",lwm2m_data->instance_info->resource_info->value.asBuffer.length , lwm2m_data->instance_info->resource_info->value.asBuffer.buffer ); 
      }
      break;
    case QAPI_NET_LWM2M_TYPE_INTEGER_E :
      LOG_DEBUG ("LWM2M_LOG: resource value is %ld\n",lwm2m_data->instance_info->resource_info->value.asInteger );
      break;
    case QAPI_NET_LWM2M_TYPE_FLOAT_E :
      LOG_DEBUG ("LWM2M_LOG: resource value is %f\n",lwm2m_data->instance_info->resource_info->value.asFloat );
      break;
    case QAPI_NET_LWM2M_TYPE_BOOLEAN_E :
      LOG_DEBUG ("LWM2M_LOG: resource value is %d\n",lwm2m_data->instance_info->resource_info->value.asBoolean );
      break;
  }

  if (lwm2m_data->instance_info->resource_info->resource_ID == QAPI_NET_LWM2M_DEVICE_RES_O_NOTIFY_EN_E)
  {
    LOG_INFO ("LWM2M_LOG : Notify En got \n ");    
    LOG_INFO ("LWM2M_LOG :ENTER THE UPDATED result \n");

	tx_byte_allocate(byte_pool_qcli, (VOID **)&lwm2m_dataP, sizeof(qapi_Net_LWM2M_Data_v2_t), TX_NO_WAIT);
    if (lwm2m_dataP == NULL)
    {
      LOG_INFO ("memory allocation failed \n");  
      *result = QAPI_ERROR;
      return;
    }
    memset (lwm2m_dataP, 0 , sizeof (qapi_Net_LWM2M_Data_v2_t));

    lwm2m_dataP->object_ID = QAPI_NET_LWM2M_DEVICE_CAP_OBJECT_ID_E;
    lwm2m_dataP->no_instances++;

	tx_byte_allocate(byte_pool_qcli, (VOID **)&(lwm2m_dataP->instance_info), sizeof(qapi_Net_LWM2M_Instance_Info_v2_t), TX_NO_WAIT);
    if( NULL == lwm2m_dataP->instance_info) 
    {
      LOG_INFO ("LWM2M_LOG : memory allocation failed \n ");
      app_free_data_v2(lwm2m_dataP);
      *result = QAPI_ERROR;
       return;
    }

    memset (lwm2m_dataP->instance_info,0,sizeof (qapi_Net_LWM2M_Instance_Info_v2_t));
    lwm2m_dataP->instance_info->instance_ID = lwm2m_data->instance_info->instance_ID;
    lwm2m_dataP->instance_info->no_resources++;

	tx_byte_allocate(byte_pool_qcli, (VOID **)&(lwm2m_dataP->instance_info->resource_info), sizeof(qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);
    if( NULL == lwm2m_dataP->instance_info->resource_info)
    {
      app_free_data_v2(lwm2m_dataP);
      *result = QAPI_ERROR;
      return;
    }

    memset (lwm2m_dataP->instance_info->resource_info,0,sizeof (qapi_Net_LWM2M_Resource_Info_t));
    lwm2m_dataP->instance_info->resource_info->resource_ID = QAPI_NET_LWM2M_DEVICE_RES_O_NOTIFY_EN_E;
    lwm2m_dataP->instance_info->resource_info->type = QAPI_NET_LWM2M_TYPE_BOOLEAN_E;
    lwm2m_dataP->instance_info->resource_info->value.asBoolean = lwm2m_data->instance_info->resource_info->value.asBoolean;
    res = qapi_Net_LWM2M_Set_v2 (handler_dev_v2,lwm2m_dataP);
    if (res == QAPI_ERROR)
    {
      LOG_INFO ("LWM2M_LOG : qapi set got failed \n");
    }
    app_free_data_v2(lwm2m_dataP);

  }

  //check if the resource is Enable that the application has done a observation on
  else if (lwm2m_data->instance_info->resource_info->resource_ID == QAPI_NET_LWM2M_DEVICE_RES_M_OP_ENABLE_E)
  {
    LOG_INFO("Enable the device\n");
  }
  //check if the resource is Disable that the application has done a observation on
  else if (lwm2m_data->instance_info->resource_info->resource_ID == QAPI_NET_LWM2M_DEVICE_RES_M_OP_DISBALE_E)
  {
    LOG_INFO("Disable the device\n");
  }
  else 
  {
    LOG_INFO("Application callback invoked for invalid resource\n");
  }  
  *result = QAPI_OK;
   return;
}


qapi_Status_t devicecap_app_callback(qapi_Net_LWM2M_App_Handler_t handler, 
                                       qapi_Net_LWM2M_Server_Data_t *lwm2m_srv_data,
                                       void                         *user_data)
{
  return QAPI_OK;
}


/**
 * @fn devicecap_init_v2()
 * @brief This fucntion is used to initialise device cap application.
 * @param void
 * @return void
 */
void devicecap_init_v2()
{
  int32_t ret_value = 0;
  int i =0;
  void *user_data = NULL;

  if (handler_dev_v2!= NULL) 
  {
    LOG_INFO ("Device capability app is started already \n");
    return;
  }

  for(i =0; i< MAX_INSTANCE_ID;i++)
  {
    instance_created_v2[i] = 0;
  }
  
  //Register device capability application with qapi lwm2m
  if(QAPI_OK == qapi_Net_LWM2M_Register_App_Extended(&handler_dev_v2, user_data, devicecap_app_callback))
  {

    qapi_Net_LWM2M_Pass_Pool_Ptr(handler_dev_v2,byte_pool_qcli);

    LOG_INFO ("DEVCAP APP STARTED\n ");

    LOG_INFO ("LWM2M_LOG : INVOKING CREATE INSTANCE API \n"); 
    ret_value = devicecap_handle_create_instance_v2();

    if (ret_value != 0)
    {
      LOG_INFO ("LWM2M_LOG : [DEVICE CAPABILITY]CREATING OBJECT INSTANCE GOT FAILED \n");
      return ;
    }
  }
  return;
}

/*
 * @fn     int32_t devicecap_handle_observe_v2() 
 * @brief  This function is used to observe the device capability object resource 
 * @param  void
 * @return on success - 0
 on failure - -1
 */
int32_t devicecap_handle_observe_v2(uint16_t instance_id)
{
  qapi_Status_t result = QAPI_OK;
  qapi_Net_LWM2M_Object_Info_v2_t observe_info;
  qapi_Net_LWM2M_Id_Info_v2_t * id_info1 = NULL;
  qapi_Net_LWM2M_Id_Info_v2_t * id_info2 = NULL;
  qapi_Net_LWM2M_Id_Info_v2_t * id_info3 = NULL;
  qapi_Net_LWM2M_Id_Info_v2_t * id_info4 = NULL;
  qapi_Net_LWM2M_Id_Info_v2_t * id_info5 = NULL;
  qapi_Net_LWM2M_Id_Info_v2_t * id_info6 = NULL;
  qapi_Net_LWM2M_Id_Info_v2_t * id_info7 = NULL;
  qapi_Net_LWM2M_Id_Info_v2_t * id_info8 = NULL;
  qapi_Net_LWM2M_Id_Info_v2_t * id_info_prev = NULL;

  if (handler_dev_v2 == NULL ) 
  {
    LOG_INFO( "LWM2M_LOG : Application is not started  \n");  
    return -1;
  }

  if( instance_id >= MAX_INSTANCE_ID )
  {
    LOG_INFO( "LWM2M_LOG : Instance id is not valid.\n");
    return QAPI_ERROR;
  }
  else
  {
    if(instance_created_v2[instance_id] == 0)
    {
      LOG_INFO( "LWM2M_LOG : Instance is not created.\n");
      return QAPI_ERROR;
    }
  }

  memset (&observe_info , 0 ,sizeof (qapi_Net_LWM2M_Object_Info_v2_t));

  observe_info.no_object_info++;

  //Populate observe information the application is interested in [Property Resource] 
  tx_byte_allocate(byte_pool_qcli, (VOID **)&id_info1, sizeof(qapi_Net_LWM2M_Id_Info_v2_t), TX_NO_WAIT);
  
  if(NULL == id_info1)
  {
    return QAPI_ERROR;
  }
  memset (id_info1 , 0 , sizeof (qapi_Net_LWM2M_Id_Info_v2_t));
  id_info1->id_set = id_info1->id_set | QAPI_NET_LWM2M_OBJECT_ID_E;
  id_info1->id_set = id_info1->id_set | QAPI_NET_LWM2M_INSTANCE_ID_E;
  id_info1->id_set = id_info1->id_set | QAPI_NET_LWM2M_RESOURCE_ID_E;
  id_info1->object_ID   = QAPI_NET_LWM2M_DEVICE_CAP_OBJECT_ID_E;
  id_info1->instance_ID = instance_id;
  id_info1->resource_ID = QAPI_NET_LWM2M_DEVICE_RES_M_PROPERTY_E;
  id_info1->next = NULL;

  if (!observe_info.id_info )
  {
    observe_info.id_info = id_info1;
  }
  else
  {
    id_info_prev =  observe_info.id_info;
    while (id_info_prev->next != NULL) 
    {
      id_info_prev = id_info_prev->next;
    }
    id_info_prev->next = id_info1;       
  }

  observe_info.no_object_info++;

  //Populate observe information the application is interested in [Group Resource]
  tx_byte_allocate(byte_pool_qcli, (VOID **)&id_info2, sizeof(qapi_Net_LWM2M_Id_Info_v2_t), TX_NO_WAIT);
  if(NULL == id_info2)
  {
    free(id_info1);
    return QAPI_ERROR;
  }
  memset (id_info2 , 0 , sizeof (qapi_Net_LWM2M_Id_Info_v2_t));
  id_info2->id_set = id_info2->id_set | QAPI_NET_LWM2M_OBJECT_ID_E;
  id_info2->id_set = id_info2->id_set | QAPI_NET_LWM2M_INSTANCE_ID_E;
  id_info2->id_set = id_info2->id_set | QAPI_NET_LWM2M_RESOURCE_ID_E;
  id_info2->object_ID = QAPI_NET_LWM2M_DEVICE_CAP_OBJECT_ID_E;
  id_info2->instance_ID = instance_id;
  id_info2->resource_ID = QAPI_NET_LWM2M_DEVICE_RES_M_GROUP_E;
  id_info1->next = id_info2;
  id_info2->next = NULL;

  observe_info.no_object_info++;

  //Populate observe information the application is interested in [Description Resource]
  tx_byte_allocate(byte_pool_qcli, (VOID **)&id_info3, sizeof(qapi_Net_LWM2M_Id_Info_v2_t), TX_NO_WAIT);  
  if(NULL == id_info3)
  {
    app_free_id_info_v2(id_info1);
    return QAPI_ERROR;
  }
  memset (id_info3 , 0 , sizeof (qapi_Net_LWM2M_Id_Info_v2_t));
  id_info3->id_set = id_info3->id_set | QAPI_NET_LWM2M_OBJECT_ID_E;
  id_info3->id_set = id_info3->id_set | QAPI_NET_LWM2M_INSTANCE_ID_E;
  id_info3->id_set = id_info3->id_set | QAPI_NET_LWM2M_RESOURCE_ID_E;
  id_info3->object_ID = QAPI_NET_LWM2M_DEVICE_CAP_OBJECT_ID_E;
  id_info3->instance_ID = instance_id;
  id_info3->resource_ID = QAPI_NET_LWM2M_DEVICE_RES_O_DESCRIPTION_E;
  id_info2->next = id_info3;
  id_info3->next = NULL;

  observe_info.no_object_info++;

  //Populate observe information the application is interested in [Attached Resource]
  tx_byte_allocate(byte_pool_qcli, (VOID **)&id_info4, sizeof(qapi_Net_LWM2M_Id_Info_v2_t), TX_NO_WAIT);    
  if(NULL == id_info4)
  {
    app_free_id_info_v2(id_info1);
    return QAPI_ERROR;
  }
  memset (id_info4 , 0 , sizeof (qapi_Net_LWM2M_Id_Info_v2_t));
  id_info4->id_set = id_info4->id_set | QAPI_NET_LWM2M_OBJECT_ID_E;
  id_info4->id_set = id_info4->id_set | QAPI_NET_LWM2M_INSTANCE_ID_E;
  id_info4->id_set = id_info4->id_set | QAPI_NET_LWM2M_RESOURCE_ID_E;
  id_info4->object_ID = QAPI_NET_LWM2M_DEVICE_CAP_OBJECT_ID_E;
  id_info4->instance_ID = instance_id;
  id_info4->resource_ID = QAPI_NET_LWM2M_DEVICE_RES_O_ATTACHED_E;
  id_info3->next = id_info4;
  id_info4->next = NULL;

  observe_info.no_object_info++;

  //Populate observe information the application is interested in [Enabled Resource]
  tx_byte_allocate(byte_pool_qcli, (VOID **)&id_info5, sizeof(qapi_Net_LWM2M_Id_Info_v2_t), TX_NO_WAIT);      
  if(NULL == id_info5)
  {
    app_free_id_info_v2(id_info1);
    return QAPI_ERROR;
  }
  memset (id_info5 , 0 , sizeof (qapi_Net_LWM2M_Id_Info_v2_t));
  id_info5->id_set = id_info5->id_set | QAPI_NET_LWM2M_OBJECT_ID_E;
  id_info5->id_set = id_info5->id_set | QAPI_NET_LWM2M_INSTANCE_ID_E;
  id_info5->id_set = id_info5->id_set | QAPI_NET_LWM2M_RESOURCE_ID_E;
  id_info5->object_ID = QAPI_NET_LWM2M_DEVICE_CAP_OBJECT_ID_E;
  id_info5->instance_ID = instance_id;
  id_info5->resource_ID = QAPI_NET_LWM2M_DEVICE_RES_M_ENABLED_E;
  id_info4->next = id_info5;
  id_info5->next = NULL;

  observe_info.no_object_info++;

  //Populate observe information the application is interested in [OpEnable Resource]
  tx_byte_allocate(byte_pool_qcli, (VOID **)&id_info6, sizeof(qapi_Net_LWM2M_Id_Info_v2_t), TX_NO_WAIT);        
  if(NULL == id_info6)
  {
    app_free_id_info_v2(id_info1);
    return QAPI_ERROR;
  }
  memset (id_info6 , 0 , sizeof (qapi_Net_LWM2M_Id_Info_v2_t));
  id_info6->id_set = id_info6->id_set | QAPI_NET_LWM2M_OBJECT_ID_E;
  id_info6->id_set = id_info6->id_set | QAPI_NET_LWM2M_INSTANCE_ID_E;
  id_info6->id_set = id_info6->id_set | QAPI_NET_LWM2M_RESOURCE_ID_E;
  id_info6->object_ID = QAPI_NET_LWM2M_DEVICE_CAP_OBJECT_ID_E;
  id_info6->instance_ID = instance_id;
  id_info6->resource_ID = QAPI_NET_LWM2M_DEVICE_RES_M_OP_ENABLE_E;
  id_info5->next = id_info6;
  id_info6->next = NULL;

  observe_info.no_object_info++;

  //Populate observe information the application is interested in [OpDisable Resource]
  tx_byte_allocate(byte_pool_qcli, (VOID **)&id_info7, sizeof(qapi_Net_LWM2M_Id_Info_v2_t), TX_NO_WAIT);        
  if(NULL == id_info7)
  {
    app_free_id_info_v2(id_info1);
    return QAPI_ERROR;
  }
  memset (id_info7 , 0 , sizeof (qapi_Net_LWM2M_Id_Info_v2_t));
  id_info7->id_set = id_info7->id_set | QAPI_NET_LWM2M_OBJECT_ID_E;
  id_info7->id_set = id_info7->id_set | QAPI_NET_LWM2M_INSTANCE_ID_E;
  id_info7->id_set = id_info7->id_set | QAPI_NET_LWM2M_RESOURCE_ID_E;
  id_info7->object_ID = QAPI_NET_LWM2M_DEVICE_CAP_OBJECT_ID_E;
  id_info7->instance_ID = instance_id;
  id_info7->resource_ID = QAPI_NET_LWM2M_DEVICE_RES_M_OP_DISBALE_E;
  id_info6->next = id_info7;
  id_info7->next = NULL;

  observe_info.no_object_info++;

  //Populate observe information the application is interested in [NotifyEn Resource]
  tx_byte_allocate(byte_pool_qcli, (VOID **)&id_info8, sizeof(qapi_Net_LWM2M_Id_Info_v2_t), TX_NO_WAIT);        
  if(NULL == id_info8)
  {
    app_free_id_info_v2(id_info1);
    return QAPI_ERROR;
  }
  memset (id_info8 , 0 , sizeof (qapi_Net_LWM2M_Id_Info_v2_t));
  id_info8->id_set = id_info8->id_set | QAPI_NET_LWM2M_OBJECT_ID_E;
  id_info8->id_set = id_info8->id_set | QAPI_NET_LWM2M_INSTANCE_ID_E;
  id_info8->id_set = id_info8->id_set | QAPI_NET_LWM2M_RESOURCE_ID_E;
  id_info8->object_ID = QAPI_NET_LWM2M_DEVICE_CAP_OBJECT_ID_E;
  id_info8->instance_ID = instance_id;
  id_info8->resource_ID = QAPI_NET_LWM2M_DEVICE_RES_O_NOTIFY_EN_E;
  id_info7->next = id_info8;
  id_info8->next = NULL;

  //Perform observe operation on the observe information populated
  result = qapi_Net_LWM2M_Observe_v2(handler_dev_v2, devicecap_app_callback_v2, &observe_info);

  if (result == QAPI_ERROR)
  {
    LOG_INFO ("[DEV_CAP] : qapi observe failed \n ");
  }
  else
  {
    observe_mode_v2[instance_id] = 1;
  }
  app_free_id_info_v2 (id_info1);

  return result;
}

/*
 * @fn     int32_t devicecap_handle_cancel_observe_v2() 
 * @brief  This function is used to cancel observe the device capability object resource 
 * @param  void
 * @return on success - 0
 on failure - -1
 */
int32_t devicecap_handle_cancel_observe_v2(uint16_t instance_id)
{
  qapi_Status_t result = QAPI_OK;
  qapi_Net_LWM2M_Object_Info_v2_t observe_info;
  qapi_Net_LWM2M_Id_Info_v2_t * id_info1 = NULL;
  qapi_Net_LWM2M_Id_Info_v2_t * id_info2 = NULL; 
  qapi_Net_LWM2M_Id_Info_v2_t * id_info3 = NULL;
  qapi_Net_LWM2M_Id_Info_v2_t * id_info4 = NULL;
  qapi_Net_LWM2M_Id_Info_v2_t * id_info5 = NULL; 
  qapi_Net_LWM2M_Id_Info_v2_t * id_info6 = NULL;
  qapi_Net_LWM2M_Id_Info_v2_t * id_info7 = NULL;
  qapi_Net_LWM2M_Id_Info_v2_t * id_info8 = NULL;

  if (handler_dev_v2 == NULL ) 
  {
    LOG_INFO( "LWM2M_LOG : Application is not started  \n");  
    return -1;
  }

  if( (instance_id < MAX_INSTANCE_ID) && (instance_created_v2[instance_id] == 1) && (observe_mode_v2[instance_id] == 1))
  {
    memset (&observe_info , 0 ,sizeof (qapi_Net_LWM2M_Object_Info_v2_t));
    observe_info.no_object_info++;
	tx_byte_allocate(byte_pool_qcli, (VOID **)&id_info1, sizeof(qapi_Net_LWM2M_Id_Info_v2_t), TX_NO_WAIT);		
    if(NULL == id_info1)
    {
      return -1;
    }
    memset(id_info1 , 0 , sizeof (qapi_Net_LWM2M_Id_Info_v2_t));
    id_info1->id_set = id_info1->id_set | QAPI_NET_LWM2M_OBJECT_ID_E;
    id_info1->id_set = id_info1->id_set | QAPI_NET_LWM2M_INSTANCE_ID_E;
    id_info1->id_set = id_info1->id_set | QAPI_NET_LWM2M_RESOURCE_ID_E;
    id_info1->object_ID = QAPI_NET_LWM2M_DEVICE_CAP_OBJECT_ID_E;
    id_info1->instance_ID = instance_id;
    id_info1->resource_ID = QAPI_NET_LWM2M_DEVICE_RES_M_PROPERTY_E;


    observe_info.id_info = id_info1;

    observe_info.no_object_info++;
	tx_byte_allocate(byte_pool_qcli, (VOID **)&id_info2, sizeof(qapi_Net_LWM2M_Id_Info_v2_t), TX_NO_WAIT);		
    if(NULL == id_info2)
    {

      app_free_id_info_v2 (id_info1);
      return -1;        
    }
    memset(id_info2 , 0 , sizeof (qapi_Net_LWM2M_Id_Info_v2_t));
    id_info2->id_set = id_info2->id_set | QAPI_NET_LWM2M_OBJECT_ID_E;
    id_info2->id_set = id_info2->id_set | QAPI_NET_LWM2M_INSTANCE_ID_E;
    id_info2->id_set = id_info2->id_set | QAPI_NET_LWM2M_RESOURCE_ID_E;
    id_info2->object_ID = QAPI_NET_LWM2M_DEVICE_CAP_OBJECT_ID_E;
    id_info2->instance_ID = instance_id;
    id_info2->resource_ID = QAPI_NET_LWM2M_DEVICE_RES_M_GROUP_E;

    id_info1->next = id_info2;

    observe_info.no_object_info++;
	tx_byte_allocate(byte_pool_qcli, (VOID **)&id_info3, sizeof(qapi_Net_LWM2M_Id_Info_v2_t), TX_NO_WAIT);		
    if(NULL == id_info3)
    {
      app_free_id_info_v2 (id_info1);
      return -1;        
    }
    memset(id_info3 , 0 , sizeof (qapi_Net_LWM2M_Id_Info_v2_t));
    id_info3->id_set = id_info3->id_set | QAPI_NET_LWM2M_OBJECT_ID_E;
    id_info3->id_set = id_info3->id_set | QAPI_NET_LWM2M_INSTANCE_ID_E;
    id_info3->id_set = id_info3->id_set | QAPI_NET_LWM2M_RESOURCE_ID_E;
    id_info3->object_ID = QAPI_NET_LWM2M_DEVICE_CAP_OBJECT_ID_E;
    id_info3->instance_ID = instance_id;
    id_info3->resource_ID = QAPI_NET_LWM2M_DEVICE_RES_O_DESCRIPTION_E;
    id_info3->next = NULL;
    id_info2->next = id_info3;

    observe_info.no_object_info++;
	tx_byte_allocate(byte_pool_qcli, (VOID **)&id_info4, sizeof(qapi_Net_LWM2M_Id_Info_v2_t), TX_NO_WAIT);		
    if(NULL == id_info4)
    {
      app_free_id_info_v2 (id_info1);
      return -1;        
    }

    memset(id_info4, 0 , sizeof (qapi_Net_LWM2M_Id_Info_v2_t));
    id_info4->id_set = id_info3->id_set | QAPI_NET_LWM2M_OBJECT_ID_E;
    id_info4->id_set = id_info3->id_set | QAPI_NET_LWM2M_INSTANCE_ID_E;
    id_info4->id_set = id_info3->id_set | QAPI_NET_LWM2M_RESOURCE_ID_E;
    id_info4->object_ID = QAPI_NET_LWM2M_DEVICE_CAP_OBJECT_ID_E;
    id_info4->instance_ID = instance_id;
    id_info4->resource_ID = QAPI_NET_LWM2M_DEVICE_RES_O_ATTACHED_E;
    id_info4->next = NULL;
    id_info3->next = id_info4;

    observe_info.no_object_info++;
	tx_byte_allocate(byte_pool_qcli, (VOID **)&id_info5, sizeof(qapi_Net_LWM2M_Id_Info_v2_t), TX_NO_WAIT);		
    if(NULL == id_info5)
    {
      app_free_id_info_v2 (id_info1);  
      return -1;        
    }

    memset(id_info5 , 0 , sizeof (qapi_Net_LWM2M_Id_Info_v2_t));
    id_info5->id_set = id_info3->id_set | QAPI_NET_LWM2M_OBJECT_ID_E;
    id_info5->id_set = id_info3->id_set | QAPI_NET_LWM2M_INSTANCE_ID_E;
    id_info5->id_set = id_info3->id_set | QAPI_NET_LWM2M_RESOURCE_ID_E;
    id_info5->object_ID = QAPI_NET_LWM2M_DEVICE_CAP_OBJECT_ID_E;
    id_info5->instance_ID = instance_id;
    id_info5->resource_ID = QAPI_NET_LWM2M_DEVICE_RES_M_ENABLED_E;
    id_info5->next = NULL;
    id_info4->next = id_info5;

    observe_info.no_object_info++;
	tx_byte_allocate(byte_pool_qcli, (VOID **)&id_info6, sizeof(qapi_Net_LWM2M_Id_Info_v2_t), TX_NO_WAIT);		
    if(NULL == id_info6)
    {
      app_free_id_info_v2 (id_info1);
      return -1;        
    }

    memset(id_info6 , 0 , sizeof (qapi_Net_LWM2M_Id_Info_v2_t));
    id_info6->id_set = id_info3->id_set | QAPI_NET_LWM2M_OBJECT_ID_E;
    id_info6->id_set = id_info3->id_set | QAPI_NET_LWM2M_INSTANCE_ID_E;
    id_info6->id_set = id_info3->id_set | QAPI_NET_LWM2M_RESOURCE_ID_E;
    id_info6->object_ID = QAPI_NET_LWM2M_DEVICE_CAP_OBJECT_ID_E;
    id_info6->instance_ID = instance_id;
    id_info6->resource_ID = QAPI_NET_LWM2M_DEVICE_RES_M_OP_ENABLE_E;
    id_info6->next = NULL;
    id_info5->next = id_info6;

    observe_info.no_object_info++;
	tx_byte_allocate(byte_pool_qcli, (VOID **)&id_info7, sizeof(qapi_Net_LWM2M_Id_Info_v2_t), TX_NO_WAIT);		
    if(NULL == id_info7)
    {
      app_free_id_info_v2 (id_info1);
      return -1;        
    }

    memset(id_info7 , 0 , sizeof (qapi_Net_LWM2M_Id_Info_v2_t));
    id_info7->id_set = id_info7->id_set | QAPI_NET_LWM2M_OBJECT_ID_E;
    id_info7->id_set = id_info7->id_set | QAPI_NET_LWM2M_INSTANCE_ID_E;
    id_info7->id_set = id_info7->id_set | QAPI_NET_LWM2M_RESOURCE_ID_E;
    id_info7->object_ID = QAPI_NET_LWM2M_DEVICE_CAP_OBJECT_ID_E;
    id_info7->instance_ID = instance_id;
    id_info7->resource_ID = QAPI_NET_LWM2M_DEVICE_RES_M_OP_DISBALE_E;
    id_info7->next = NULL;
    id_info6->next = id_info7;

    observe_info.no_object_info++;
	tx_byte_allocate(byte_pool_qcli, (VOID **)&id_info8, sizeof(qapi_Net_LWM2M_Id_Info_v2_t), TX_NO_WAIT);		
    if(NULL == id_info8)
    {
      app_free_id_info_v2 (id_info1);
      return -1;        
    }

    memset(id_info8 , 0 , sizeof (qapi_Net_LWM2M_Id_Info_v2_t));
    id_info8->id_set = id_info8->id_set | QAPI_NET_LWM2M_OBJECT_ID_E;
    id_info8->id_set = id_info8->id_set | QAPI_NET_LWM2M_INSTANCE_ID_E;
    id_info8->id_set = id_info8->id_set | QAPI_NET_LWM2M_RESOURCE_ID_E;
    id_info8->object_ID = QAPI_NET_LWM2M_DEVICE_CAP_OBJECT_ID_E;
    id_info8->instance_ID = instance_id;
    id_info8->resource_ID = QAPI_NET_LWM2M_DEVICE_RES_O_NOTIFY_EN_E;
    id_info8->next = NULL;
    id_info7->next = id_info8;

    result = qapi_Net_LWM2M_Cancel_Observe_v2 (handler_dev_v2, &observe_info);
    if (result == QAPI_ERROR)
    {
      LOG_INFO ("LWM2M_LOG : cancel observe failed \n");  
    }
    else
    {
      observe_mode_v2[instance_id] = 0;
    }
    app_free_id_info_v2 (id_info1);

    return result;
  }

  else
  {
    LOG_INFO( "LWM2M_LOG : Observe is not present.on instance id\n");
    return QAPI_ERROR;
  }
}


/*
 * @fn     int32_t devicecap_handle_create_instance_v2() 
 * @brief  This function is used to create the device capability object instance 
 * @param  void
 * @return on success - 0
 on failure - -1
 */

int32_t devicecap_handle_create_instance_v2() 
{
  qapi_Net_LWM2M_Data_v2_t lwm2m_data;
  qapi_Net_LWM2M_Resource_Info_t *resource_info = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource_info1 = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource_info2 = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource_info3 = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource_info4 = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource_info5 = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource_info6 = NULL;
  qapi_Status_t result = QAPI_OK;
  int i =0;
  if (handler_dev_v2 == NULL ) 
  {
    LOG_INFO( "LWM2M_LOG : Create is not allowed as application is not started  \n"); 
    return -2;
  }

  memset (&lwm2m_data , 0 , sizeof (qapi_Net_LWM2M_Data_v2_t));

  lwm2m_data.object_ID    = QAPI_NET_LWM2M_DEVICE_CAP_OBJECT_ID_E;

  for(i=0; i< MAX_INSTANCE_ID; i++)
  {
    if(instance_created_v2[i] == 0)
    {
      inst_index_v2 =i;
      break;
    }
  }

  if(i == MAX_INSTANCE_ID)
  {
    LOG_INFO ("LWM2M_LOG : Max intsances limit reached. \n");  
    return -1;
  }

  lwm2m_data.no_instances++;

  tx_byte_allocate(byte_pool_qcli, (VOID **)&(lwm2m_data.instance_info), sizeof(qapi_Net_LWM2M_Instance_Info_v2_t), TX_NO_WAIT);		    
  if (lwm2m_data.instance_info == NULL) 
  {
    LOG_INFO ("LWM2M_LOG : MALLOC FAILURE \n");  
    return -1;
  }
  memset (lwm2m_data.instance_info  , 0 ,sizeof (qapi_Net_LWM2M_Instance_Info_v2_t));

  lwm2m_data.instance_info->instance_ID = inst_index_v2;
  lwm2m_data.instance_info->no_resources++;
  tx_byte_allocate(byte_pool_qcli, (VOID **)&(lwm2m_data.instance_info->resource_info), sizeof(qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);		    
  if (lwm2m_data.instance_info->resource_info == NULL) 
  {
    LOG_INFO ("LWM2M_LOG : MALLOC FAILURE \n");  
    return -1;
  }
  memset (lwm2m_data.instance_info->resource_info, 0 , sizeof (qapi_Net_LWM2M_Resource_Info_t));

  lwm2m_data.instance_info->resource_info->type  = QAPI_NET_LWM2M_TYPE_STRING_E;
  lwm2m_data.instance_info->resource_info->resource_ID = QAPI_NET_LWM2M_DEVICE_RES_M_PROPERTY_E;
  lwm2m_data.instance_info->resource_info->value.asBuffer.length = strlen(PROPERTY);
  tx_byte_allocate(byte_pool_qcli, (VOID **)&(lwm2m_data.instance_info->resource_info->value.asBuffer.buffer), strlen(PROPERTY)+1, TX_NO_WAIT);		    
  strlcpy((char *)lwm2m_data.instance_info->resource_info->value.asBuffer.buffer,
      PROPERTY,lwm2m_data.instance_info->resource_info->value.asBuffer.length+1);

  tx_byte_allocate(byte_pool_qcli, (VOID **)&resource_info , sizeof(qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);		    
  if (resource_info == NULL)
  {
    LOG_INFO ("LWM2M_LOG : MALLOC FAILURE \n");
    return -1;
  }
  memset (resource_info, 0 , sizeof (qapi_Net_LWM2M_Resource_Info_t));

  lwm2m_data.instance_info->no_resources++;
  lwm2m_data.instance_info->resource_info->next = resource_info;
  resource_info->type  = QAPI_NET_LWM2M_TYPE_INTEGER_E;
  resource_info->resource_ID = QAPI_NET_LWM2M_DEVICE_RES_M_GROUP_E;
  resource_info->value.asInteger = 0;

  tx_byte_allocate(byte_pool_qcli, (VOID **)&resource_info1 , sizeof(qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);		    
  if (resource_info1 == NULL)
  {
    LOG_INFO ("LWM2M_LOG : MALLOC FAILURE \n");
    return -1;
  }
  memset (resource_info1, 0 , sizeof (qapi_Net_LWM2M_Resource_Info_t));

  lwm2m_data.instance_info->no_resources++;
  resource_info->next = resource_info1;   
  resource_info1->type  = QAPI_NET_LWM2M_TYPE_STRING_E;
  resource_info1->resource_ID = QAPI_NET_LWM2M_DEVICE_RES_O_DESCRIPTION_E;
  resource_info1->value.asBuffer.length = strlen(PROPERTY);
  tx_byte_allocate(byte_pool_qcli, (VOID **)&(resource_info1->value.asBuffer.buffer), strlen(PROPERTY)+1, TX_NO_WAIT);		      
  strlcpy((char *)resource_info1->value.asBuffer.buffer,DESCRIPTION,
      resource_info1->value.asBuffer.length+1);

  tx_byte_allocate(byte_pool_qcli, (VOID **)&resource_info2, sizeof (qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);		      
  if (resource_info2 == NULL)
  {
    LOG_INFO ("LWM2M_LOG : MALLOC FAILURE \n");
    return -1;
  }
  memset (resource_info2, 0 , sizeof (qapi_Net_LWM2M_Resource_Info_t));

  lwm2m_data.instance_info->no_resources++;
  resource_info1->next = resource_info2;   
  resource_info2->type  = QAPI_NET_LWM2M_TYPE_BOOLEAN_E;
  resource_info2->resource_ID = QAPI_NET_LWM2M_DEVICE_RES_O_ATTACHED_E;
  resource_info2->value.asBoolean = true;

  tx_byte_allocate(byte_pool_qcli, (VOID **)&resource_info3, sizeof (qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);		      
  if (resource_info3 == NULL)
  {
    LOG_INFO ("LWM2M_LOG : MALLOC FAILURE \n");
    return -1;
  }
  memset (resource_info3, 0 , sizeof (qapi_Net_LWM2M_Resource_Info_t));

  lwm2m_data.instance_info->no_resources++;
  resource_info2->next = resource_info3;   
  resource_info3->type  = QAPI_NET_LWM2M_TYPE_BOOLEAN_E;
  resource_info3->resource_ID = QAPI_NET_LWM2M_DEVICE_RES_M_ENABLED_E;
  resource_info3->value.asBoolean = true;

  tx_byte_allocate(byte_pool_qcli, (VOID **)&resource_info4, sizeof (qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);		      
  if (resource_info4 == NULL)
  {
    LOG_INFO ("LWM2M_LOG : MALLOC FAILURE \n");
    return -1;
  }
  memset (resource_info4, 0 , sizeof (qapi_Net_LWM2M_Resource_Info_t));

  lwm2m_data.instance_info->no_resources++;
  resource_info3->next = resource_info4;   
  resource_info4->type  = QAPI_NET_LWM2M_TYPE_BOOLEAN_E;
  resource_info4->resource_ID = QAPI_NET_LWM2M_DEVICE_RES_M_OP_ENABLE_E;
  resource_info4->value.asBoolean = true;

  tx_byte_allocate(byte_pool_qcli, (VOID **)&resource_info5, sizeof (qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);		      
  if (resource_info5 == NULL)
  {
    LOG_INFO ("LWM2M_LOG : MALLOC FAILURE \n");
    return -1;
  }
  memset (resource_info5, 0 , sizeof (qapi_Net_LWM2M_Resource_Info_t));

  lwm2m_data.instance_info->no_resources++;
  resource_info4->next = resource_info5;   
  resource_info5->type  = QAPI_NET_LWM2M_TYPE_BOOLEAN_E;
  resource_info5->resource_ID = QAPI_NET_LWM2M_DEVICE_RES_M_OP_DISBALE_E;
  resource_info5->value.asBoolean = false;

  tx_byte_allocate(byte_pool_qcli, (VOID **)&resource_info6, sizeof (qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);		      
  if (resource_info6 == NULL)
  {
    LOG_INFO ("LWM2M_LOG : MALLOC FAILURE \n");
    return -1;
  }
  memset (resource_info6, 0 , sizeof (qapi_Net_LWM2M_Resource_Info_t));

  lwm2m_data.instance_info->no_resources++;
  resource_info5->next = resource_info6;
  resource_info6->type  = QAPI_NET_LWM2M_TYPE_BOOLEAN_E;
  resource_info6->resource_ID = QAPI_NET_LWM2M_DEVICE_RES_O_NOTIFY_EN_E;
  resource_info6->value.asBoolean = true;

  LOG_INFO ("LWM2M_LOG : INVOKING THE OBJECT CREATE INSTANCE  \n");

  result = qapi_Net_LWM2M_Create_Object_Instance_v2 (handler_dev_v2, &lwm2m_data); 
  if (result == QAPI_ERROR)
  {
    LOG_INFO ("LWM2M_LOG : create object instance got failed \n");
  }
  else
  {
    instance_created_v2[inst_index_v2] = 1;
  }
  app_free_instance_info_v2(lwm2m_data.instance_info);

  return 0;
}

/*
 * @fn     int32_t devicecap_handle_delete_instance_v2() 
 * @brief  This function is used to delete the device capability object instance 
 * @param  instance_id  - instance id to be deleted
 * @return on success - 0
 on failure - -1
 */

int32_t devicecap_handle_delete_instance_v2(uint16_t instance_id ) 
{
  qapi_Status_t  result = QAPI_OK;
  qapi_Net_LWM2M_Object_Info_v2_t object_info ;

  qapi_Net_LWM2M_Id_Info_v2_t *id_info = NULL;  

  if (handler_dev_v2 == NULL) 
  {
    LOG_INFO( "LWM2M_LOG : Delete is not allowed as application is not started  \n");  
    return -1;
  }

  if( instance_id >= MAX_INSTANCE_ID )
  {
    LOG_INFO ("LWM2M_LOG: Instance id is not valid.\n");
    return -1;
  }
  else
  {
    if(instance_created_v2[instance_id] == 0)
    {
      LOG_INFO ("LWM2M_LOG: Instance is not created.\n");
      return -1;
    }
  }
  

  tx_byte_allocate(byte_pool_qcli, (VOID **)&id_info, sizeof (qapi_Net_LWM2M_Id_Info_v2_t), TX_NO_WAIT);		        
  if (id_info == NULL)
  {
    LOG_INFO ("LWM2M : [DEVICE CAPABILITY APP] MALLOC FALIURE \n");
    return -1;
  }
  else
  {
    memset (&object_info , 0 , sizeof (qapi_Net_LWM2M_Object_Info_v2_t));
    object_info.no_object_info++;
    memset(id_info , 0 , sizeof (qapi_Net_LWM2M_Id_Info_v2_t));
    id_info->id_set = id_info->id_set | QAPI_NET_LWM2M_OBJECT_ID_E;
    id_info->id_set = id_info->id_set | QAPI_NET_LWM2M_INSTANCE_ID_E;
    id_info->object_ID = QAPI_NET_LWM2M_DEVICE_CAP_OBJECT_ID_E;
    id_info->instance_ID = instance_id;

    id_info->next = NULL;
    object_info.id_info = id_info;
    object_info.id_info->next = NULL;

    result = qapi_Net_LWM2M_Delete_Object_Instance_v2 (handler_dev_v2, &object_info);

    if (result == QAPI_ERROR)
    {
      LOG_INFO ("LWM2M_LOG : DELETE INSTANCE GOT FAILED \n");
      app_free_id_info_v2 (id_info);
      return -1;

    }
    else
    {
      instance_created_v2[instance_id] = 0;

      if(observe_mode_v2[instance_id] == 1)
      {
        qapi_Net_LWM2M_Object_Info_v2_t observe_info;
        qapi_Net_LWM2M_Id_Info_v2_t * id_info1 = NULL;
        qapi_Net_LWM2M_Id_Info_v2_t * id_info2 = NULL; 
        qapi_Net_LWM2M_Id_Info_v2_t * id_info3 = NULL;
        qapi_Net_LWM2M_Id_Info_v2_t * id_info4 = NULL;
        qapi_Net_LWM2M_Id_Info_v2_t * id_info5 = NULL; 
        qapi_Net_LWM2M_Id_Info_v2_t * id_info6 = NULL;
        qapi_Net_LWM2M_Id_Info_v2_t * id_info7 = NULL;
        qapi_Net_LWM2M_Id_Info_v2_t * id_info8 = NULL;

        memset (&observe_info , 0 ,sizeof (qapi_Net_LWM2M_Object_Info_v2_t));
        observe_info.no_object_info++;
		tx_byte_allocate(byte_pool_qcli, (VOID **)&id_info1, sizeof (qapi_Net_LWM2M_Id_Info_v2_t), TX_NO_WAIT);			  
        if(NULL == id_info1)
        {
          return -1;        
        }
        memset(id_info1 , 0 , sizeof (qapi_Net_LWM2M_Id_Info_v2_t));
        id_info1->id_set = id_info1->id_set | QAPI_NET_LWM2M_OBJECT_ID_E;
        id_info1->id_set = id_info1->id_set | QAPI_NET_LWM2M_INSTANCE_ID_E;
        id_info1->id_set = id_info1->id_set | QAPI_NET_LWM2M_RESOURCE_ID_E;
        id_info1->object_ID = QAPI_NET_LWM2M_DEVICE_CAP_OBJECT_ID_E;
        id_info1->instance_ID = instance_id;
        id_info1->resource_ID = QAPI_NET_LWM2M_DEVICE_RES_M_PROPERTY_E;


        observe_info.id_info = id_info1;

        observe_info.no_object_info++;
		tx_byte_allocate(byte_pool_qcli, (VOID **)&id_info2, sizeof (qapi_Net_LWM2M_Id_Info_v2_t), TX_NO_WAIT);			  
        if(NULL == id_info2)
        {

          app_free_id_info_v2 (id_info1);
          return -1;        
        }
        memset(id_info2 , 0 , sizeof (qapi_Net_LWM2M_Id_Info_v2_t));
        id_info2->id_set = id_info2->id_set | QAPI_NET_LWM2M_OBJECT_ID_E;
        id_info2->id_set = id_info2->id_set | QAPI_NET_LWM2M_INSTANCE_ID_E;
        id_info2->id_set = id_info2->id_set | QAPI_NET_LWM2M_RESOURCE_ID_E;
        id_info2->object_ID = QAPI_NET_LWM2M_DEVICE_CAP_OBJECT_ID_E;
        id_info2->instance_ID = instance_id;
        id_info2->resource_ID = QAPI_NET_LWM2M_DEVICE_RES_M_GROUP_E;

        id_info1->next = id_info2;

        observe_info.no_object_info++;
		tx_byte_allocate(byte_pool_qcli, (VOID **)&id_info3, sizeof (qapi_Net_LWM2M_Id_Info_v2_t), TX_NO_WAIT);			  
        if(NULL == id_info3)
        {
          app_free_id_info_v2 (id_info1);
          return -1;        
        }
        memset(id_info3 , 0 , sizeof (qapi_Net_LWM2M_Id_Info_v2_t));
        id_info3->id_set = id_info3->id_set | QAPI_NET_LWM2M_OBJECT_ID_E;
        id_info3->id_set = id_info3->id_set | QAPI_NET_LWM2M_INSTANCE_ID_E;
        id_info3->id_set = id_info3->id_set | QAPI_NET_LWM2M_RESOURCE_ID_E;
        id_info3->object_ID = QAPI_NET_LWM2M_DEVICE_CAP_OBJECT_ID_E;
        id_info3->instance_ID = instance_id;
        id_info3->resource_ID = QAPI_NET_LWM2M_DEVICE_RES_O_DESCRIPTION_E;
        id_info3->next = NULL;
        id_info2->next = id_info3;

        observe_info.no_object_info++;
		tx_byte_allocate(byte_pool_qcli, (VOID **)&id_info4, sizeof (qapi_Net_LWM2M_Id_Info_v2_t), TX_NO_WAIT);			  
        if(NULL == id_info4)
        {
          app_free_id_info_v2 (id_info1);
          return -1;        
        }

        memset(id_info4, 0 , sizeof (qapi_Net_LWM2M_Id_Info_v2_t));
        id_info4->id_set = id_info3->id_set | QAPI_NET_LWM2M_OBJECT_ID_E;
        id_info4->id_set = id_info3->id_set | QAPI_NET_LWM2M_INSTANCE_ID_E;
        id_info4->id_set = id_info3->id_set | QAPI_NET_LWM2M_RESOURCE_ID_E;
        id_info4->object_ID = QAPI_NET_LWM2M_DEVICE_CAP_OBJECT_ID_E;
        id_info4->instance_ID = instance_id;
        id_info4->resource_ID = QAPI_NET_LWM2M_DEVICE_RES_O_ATTACHED_E;
        id_info4->next = NULL;
        id_info3->next = id_info4;

        observe_info.no_object_info++;
		tx_byte_allocate(byte_pool_qcli, (VOID **)&id_info5, sizeof (qapi_Net_LWM2M_Id_Info_v2_t), TX_NO_WAIT);			  		
        if(NULL == id_info5)
        {
          app_free_id_info_v2 (id_info1);  
          return -1;        
        }

        memset(id_info5 , 0 , sizeof (qapi_Net_LWM2M_Id_Info_v2_t));
        id_info5->id_set = id_info3->id_set | QAPI_NET_LWM2M_OBJECT_ID_E;
        id_info5->id_set = id_info3->id_set | QAPI_NET_LWM2M_INSTANCE_ID_E;
        id_info5->id_set = id_info3->id_set | QAPI_NET_LWM2M_RESOURCE_ID_E;
        id_info5->object_ID = QAPI_NET_LWM2M_DEVICE_CAP_OBJECT_ID_E;
        id_info5->instance_ID = instance_id;
        id_info5->resource_ID = QAPI_NET_LWM2M_DEVICE_RES_M_ENABLED_E;
        id_info5->next = NULL;
        id_info4->next = id_info5;

        observe_info.no_object_info++;
		tx_byte_allocate(byte_pool_qcli, (VOID **)&id_info6, sizeof (qapi_Net_LWM2M_Id_Info_v2_t), TX_NO_WAIT);			  		
        if(NULL == id_info6)
        {
          app_free_id_info_v2 (id_info1);
          return -1;        
        }

        memset(id_info6 , 0 , sizeof (qapi_Net_LWM2M_Id_Info_v2_t));
        id_info6->id_set = id_info3->id_set | QAPI_NET_LWM2M_OBJECT_ID_E;
        id_info6->id_set = id_info3->id_set | QAPI_NET_LWM2M_INSTANCE_ID_E;
        id_info6->id_set = id_info3->id_set | QAPI_NET_LWM2M_RESOURCE_ID_E;
        id_info6->object_ID = QAPI_NET_LWM2M_DEVICE_CAP_OBJECT_ID_E;
        id_info6->instance_ID = instance_id;
        id_info6->resource_ID = QAPI_NET_LWM2M_DEVICE_RES_M_OP_ENABLE_E;
        id_info6->next = NULL;
        id_info5->next = id_info6;

        observe_info.no_object_info++;
		tx_byte_allocate(byte_pool_qcli, (VOID **)&id_info7, sizeof (qapi_Net_LWM2M_Id_Info_v2_t), TX_NO_WAIT);			  		
        if(NULL == id_info7)
        {
          app_free_id_info_v2 (id_info1);
          return -1;        
        }

        memset(id_info7 , 0 , sizeof (qapi_Net_LWM2M_Id_Info_v2_t));
        id_info7->id_set = id_info7->id_set | QAPI_NET_LWM2M_OBJECT_ID_E;
        id_info7->id_set = id_info7->id_set | QAPI_NET_LWM2M_INSTANCE_ID_E;
        id_info7->id_set = id_info7->id_set | QAPI_NET_LWM2M_RESOURCE_ID_E;
        id_info7->object_ID = QAPI_NET_LWM2M_DEVICE_CAP_OBJECT_ID_E;
        id_info7->instance_ID = instance_id;
        id_info7->resource_ID = QAPI_NET_LWM2M_DEVICE_RES_M_OP_DISBALE_E;
        id_info7->next = NULL;
        id_info6->next = id_info7;

        observe_info.no_object_info++;
		tx_byte_allocate(byte_pool_qcli, (VOID **)&id_info8, sizeof (qapi_Net_LWM2M_Id_Info_v2_t), TX_NO_WAIT);			  		
        if(NULL == id_info8)
        {
          app_free_id_info_v2 (id_info1);
          return -1;        
        }

        memset(id_info8 , 0 , sizeof (qapi_Net_LWM2M_Id_Info_v2_t));
        id_info8->id_set = id_info8->id_set | QAPI_NET_LWM2M_OBJECT_ID_E;
        id_info8->id_set = id_info8->id_set | QAPI_NET_LWM2M_INSTANCE_ID_E;
        id_info8->id_set = id_info8->id_set | QAPI_NET_LWM2M_RESOURCE_ID_E;
        id_info8->object_ID = QAPI_NET_LWM2M_DEVICE_CAP_OBJECT_ID_E;
        id_info8->instance_ID = instance_id;
        id_info8->resource_ID = QAPI_NET_LWM2M_DEVICE_RES_O_NOTIFY_EN_E;
        id_info8->next = NULL;
        id_info7->next = id_info8;

        result = qapi_Net_LWM2M_Cancel_Observe_v2 (handler_dev_v2, &observe_info);
        if (result == QAPI_ERROR)
        {
          LOG_INFO ("LWM2M_LOG : cancel observe failed \n");  
        }
        else
        {
          observe_mode_v2[instance_id] = 0;
        }
        app_free_id_info_v2 (id_info1);
      }
      else
      {
        LOG_INFO( "LWM2M_LOG : Observe is not present.on instance id\n");
      }
    }
  }
  return 0;
}

int device_cap_get_v2(qapi_Net_LWM2M_Id_Info_v2_t *id_info)
{
  qapi_Net_LWM2M_Data_v2_t *lwm2m_dataP = NULL;
  qapi_Status_t result = QAPI_ERROR;
  qapi_Net_LWM2M_Object_Info_v2_t lwm2m_info_req;

  if(id_info == NULL)
  {
    LOG_INFO("Null parameter passed.");
    return -1;
  }

  memset (&lwm2m_info_req, 0x00, sizeof (qapi_Net_LWM2M_Object_Info_v2_t));
  lwm2m_info_req.no_object_info++;
  lwm2m_info_req.id_info = id_info; 

  result = qapi_Net_LWM2M_Get_v2(handler_dev_v2, &lwm2m_info_req, &lwm2m_dataP);
  if (result == QAPI_ERROR)
  {
    LOG_INFO ("LWM2M_LOG : Get failed for device capabilities!\n");
  }
  else 
  {
    app_display_obj_info_v2(lwm2m_dataP);
  }

  app_free_data_v2(lwm2m_dataP);
  return result;  
}

/**
 * @fn devicecap_app_deregister_v2()
 * @brief This fucntion is used to deregister device cap application.
 * @param void
 * @return void
 */
void devicecap_app_deregister_v2()
{
  qapi_Status_t result = QAPI_OK;
  qapi_Status_t res = QAPI_OK;
  int i = 0;
  if(handler_dev_v2)
  {
    for(i =0; i< MAX_INSTANCE_ID;i++)
    {
      if(instance_created_v2[i] == 1)
      {
        res = devicecap_handle_delete_instance_v2(i);
        if(res == QAPI_ERROR)
        {
          LOG_INFO ("LWM2M_LOG : device capability instance is not deleted.\n");
          return;
        }
      }
    }

    result = qapi_Net_LWM2M_DeRegister_App(handler_dev_v2);
    if (result == QAPI_ERROR)
    {
      LOG_INFO ("LWM2M_LOG : device capability handler is not valid \n");
    }
    handler_dev_v2 = NULL;
  }
  else
  {
    LOG_INFO("Handler is not valid\n");  
  }
  return;
}

/**
 * @fn      set_notify_en_value_v2()
 *
 * @brief   This fucntion is used to set notify en resource value
 *
 * @param   notifyen Boolean Notify_en value to set
 * @return  1 on Success, 
 *          0 on Failure
 */
int set_notify_en_value_v2(uint16_t instance_id,bool notifyen)
{
  qapi_Net_LWM2M_Data_v2_t *devicecap_resource_data = NULL;
  qapi_Status_t result = QAPI_OK;

  if (handler_dev_v2 == NULL ) 
  {
    LOG_INFO( "LWM2M_LOG : Application is not started  \n");  
    return 0;
  }

  if((notifyen != 0) && (notifyen != 1))
  {
    LOG_INFO("LWM2M_LOG: Enter valid notify_en value\n");
    return 0;
  }
  
  if( instance_id >= MAX_INSTANCE_ID )
  {
    LOG_INFO ("LWM2M_LOG: Instance id is not valid.\n");
    return 0;
  }
  else
  {
    if(instance_created_v2[instance_id] == 0)
    {
      LOG_INFO ("LWM2M_LOG: Instance is not created.\n");
      return 0;
    }
  }
  
  tx_byte_allocate(byte_pool_qcli, (VOID **)&devicecap_resource_data, sizeof (qapi_Net_LWM2M_Data_v2_t), TX_NO_WAIT); 				  

  if(devicecap_resource_data == NULL)
  {
    return 0;
  }

  memset (devicecap_resource_data, 0 , sizeof (qapi_Net_LWM2M_Data_v2_t));

  devicecap_resource_data->object_ID = QAPI_NET_LWM2M_DEVICE_CAP_OBJECT_ID_E;
  devicecap_resource_data->no_instances++;

  tx_byte_allocate(byte_pool_qcli, (VOID **)&(devicecap_resource_data->instance_info), sizeof (qapi_Net_LWM2M_Instance_Info_v2_t), TX_NO_WAIT); 				  
  if( NULL == devicecap_resource_data->instance_info) 
  {
    LOG_INFO ("LWM2M_LOG : memory allocation failed \n ");
    app_free_data_v2(devicecap_resource_data);
    return 0;
  }

  memset (devicecap_resource_data->instance_info,0,sizeof (qapi_Net_LWM2M_Instance_Info_v2_t));
  devicecap_resource_data->instance_info->instance_ID = instance_id;
  devicecap_resource_data->instance_info->no_resources++;

  tx_byte_allocate(byte_pool_qcli, (VOID **)&(devicecap_resource_data->instance_info->resource_info), sizeof (qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT); 				  
  if( NULL == devicecap_resource_data->instance_info->resource_info)
  {
    app_free_data_v2(devicecap_resource_data);
    return 0;
  }

  memset (devicecap_resource_data->instance_info->resource_info,0,sizeof (qapi_Net_LWM2M_Resource_Info_t));
  devicecap_resource_data->instance_info->resource_info->resource_ID = QAPI_NET_LWM2M_DEVICE_RES_O_NOTIFY_EN_E;
  devicecap_resource_data->instance_info->resource_info->type = QAPI_NET_LWM2M_TYPE_BOOLEAN_E;
  devicecap_resource_data->instance_info->resource_info->value.asBoolean = notifyen;
  result = qapi_Net_LWM2M_Set_v2 (handler_dev_v2,devicecap_resource_data);
  if (result == QAPI_ERROR)
  {
    LOG_INFO ("LWM2M_LOG : qapi set got failed \n");
  }
  app_free_data_v2(devicecap_resource_data);

  return 1;
}
