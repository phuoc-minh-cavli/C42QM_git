/**************************************************************************************************************
 * Copyright © 2018 Qualcomm Technologies, Inc.
 * @file  software_mgnt_app_v2.c
 * @author  Qualcomm Technologies, Inc.
 * @brief File contains the implementation for the software management stub application
 *
 **************************************************************************************************************/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "log_intf_module.h"
#include "fs_public.h"
#include "qapi_lwm2m.h"
#include "qapi_fs.h"
#include "sw_mgnt_stub_app_v2.h"
#include "qapi_socket.h"
#include "utils_app_v2.h"

#define SOFTWARE_MGNT_APP_FILE_PATH "/datatx/sw_mgnt_stub.bin"
#define QAPI_Net_LWM2M_SOFTWARE_MGT_INSTANCE_ID_0                  0
#define PKG_NAME "stub_package"
#define PKG_VERSION "1.0"

extern TX_BYTE_POOL *byte_pool_qcli;

qapi_Net_LWM2M_App_Handler_t gsoftware_mgnt_Handler_v2 = NULL;
bool gsw_mgnt_downloaded_v2 = false ;
bool gsw_mgnt_install_v2    = false ;
bool gsw_mgnt_activate_v2   = false ;
static int32_t sw_mgnt_file_fd = -1;

static int32_t sw_mgnt_handle_create_instance_v2(void);
void software_mngt_app_deregister_v2(void);
/*
 * @fn     int32_t software_mgnt_app_callback_v2()
 * @brief  This function invoked by when server does any operation on particular resources observed by application
 * @param  handler (IN) Indicates the handler that is provided to application on 
 Successful registration  
 * @param  lwm2m_data (IN) lwm2m object instance and its resource information 
 * @return on success - QAPI_OK
 on error   - QAPI_ERROR  
 */
void software_mgnt_app_callback_v2( qapi_Net_LWM2M_App_Handler_t handler, qapi_Net_LWM2M_Data_v2_t * lwm2m_data, qapi_Status_t *result)
{
  int rc = 0; 
  qapi_Net_LWM2M_Data_v2_t *lwm2m_dataP = NULL;
  qapi_Net_LWM2M_Data_v2_t *dataP = NULL;
  qapi_Status_t res = QAPI_ERROR;
  qapi_Net_LWM2M_Object_Info_v2_t lwm2m_info_req ;
  uint32 written_bytes = 0;
  
  LOG_INFO ("LWM2M_LOG : App call back invoked \n");
  if (handler == NULL || handler != gsoftware_mgnt_Handler_v2 || lwm2m_data == NULL || lwm2m_data->instance_info == NULL || \
      lwm2m_data->instance_info->resource_info == NULL)
  {
    LOG_INFO ("LWM2M_LOG : passed arguments are NULL\n");
    *result = QAPI_ERROR;
    return;
  }
  LOG_INFO ("LWM2M_LOG: object_ID ----> %d\n",lwm2m_data->object_ID );
  LOG_INFO ("LWM2M_LOG: instance id ----> %d \n",lwm2m_data->instance_info->instance_ID);
  LOG_INFO ("LWM2M_LOG: resouces id -----> %d \n",lwm2m_data->instance_info->resource_info->resource_ID);
  switch (lwm2m_data->instance_info->resource_info->type)
  {
    case QAPI_NET_LWM2M_TYPE_STRING_E :
      LOG_INFO ("LWM2M_LOG: resource value is length %d\t ",lwm2m_data->instance_info->resource_info->value.asBuffer.length );
      if (lwm2m_data->instance_info->resource_info->value.asBuffer.length)
        LOG_INFO ("LWM2M_LOG: resource value is %s\n", lwm2m_data->instance_info->resource_info->value.asBuffer.buffer ); 
      break;
    case QAPI_NET_LWM2M_TYPE_INTEGER_E :
      LOG_INFO ("LWM2M_LOG: resource value is %ld\n",lwm2m_data->instance_info->resource_info->value.asInteger );
      break;
    case QAPI_NET_LWM2M_TYPE_FLOAT_E :
      LOG_INFO ("LWM2M_LOG: resource value is %f\n",lwm2m_data->instance_info->resource_info->value.asFloat );
      break;
    case QAPI_NET_LWM2M_TYPE_BOOLEAN_E :
      LOG_INFO ("LWM2M_LOG: resource value is %d\n",lwm2m_data->instance_info->resource_info->value.asBoolean );
      break;
  }
  if (lwm2m_data->instance_info->resource_info->resource_ID == QAPI_NET_LWM2M_SW_MGNT_RES_O_PACKAGE_E)
  {
    if(!gsw_mgnt_downloaded_v2)  
      gsw_mgnt_downloaded_v2 = true;
    if (sw_mgnt_file_fd == -1) 
    {
       res = qapi_FS_Open(SOFTWARE_MGNT_APP_FILE_PATH, (O_CREAT | O_RDWR), &sw_mgnt_file_fd);
    }

    if(sw_mgnt_file_fd < 0) {
      LOG_INFO (" FILE OPENED FAILED \n");
      sw_mgnt_file_fd = -1;
      *result = QAPI_ERROR;
      return;
    }   

    if (lwm2m_data->instance_info->resource_info->value.asBuffer.length) 
    {
      rc = qapi_FS_Write(sw_mgnt_file_fd, lwm2m_data->instance_info->resource_info->value.asBuffer.buffer, lwm2m_data->instance_info->resource_info->value.asBuffer.length, &written_bytes);

      if(rc < 0 || written_bytes < lwm2m_data->instance_info->resource_info->value.asBuffer.length)
      {
        LOG_INFO("LWM2M_LOG : Software management Download file write failed\n");
		tx_byte_allocate(byte_pool_qcli, (VOID **)&lwm2m_dataP, sizeof (qapi_Net_LWM2M_Data_v2_t), TX_NO_WAIT);		  		
        if (lwm2m_dataP == NULL)
        {
          LOG_INFO ("memory allocation falied \n");  
          *result = QAPI_ERROR;
           return;
        }
        memset (lwm2m_dataP, 0 , sizeof (qapi_Net_LWM2M_Data_v2_t)); 
        LOG_INFO("LWM2M_LOG: SOFTWARE MANAGEMENT Download complete\n");
        lwm2m_dataP->object_ID = QAPI_NET_LWM2M_SOFTWARE_MGNT_OBJECT_ID_E;
        lwm2m_dataP->no_instances++;
		tx_byte_allocate(byte_pool_qcli, (VOID **)&(lwm2m_dataP->instance_info), sizeof (qapi_Net_LWM2M_Instance_Info_v2_t), TX_NO_WAIT);		  		
        if( NULL == lwm2m_dataP->instance_info) 
        {
          LOG_INFO ("LWM2M_LOG : memory allocation failed \n ");
          app_free_data_v2(lwm2m_dataP);

          *result = QAPI_ERROR;
           return;
        }
        memset (lwm2m_dataP->instance_info ,0,sizeof (qapi_Net_LWM2M_Instance_Info_v2_t));
        lwm2m_dataP->instance_info->instance_ID = QAPI_Net_LWM2M_SOFTWARE_MGT_INSTANCE_ID_0;
        lwm2m_dataP->instance_info->no_resources++;
		tx_byte_allocate(byte_pool_qcli, (VOID **)&(lwm2m_dataP->instance_info->resource_info), sizeof (qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);		  		
        if( NULL == lwm2m_dataP->instance_info->resource_info)
        {
          app_free_data_v2(lwm2m_dataP);
          *result = QAPI_ERROR;
           return;
        }
        memset (lwm2m_dataP->instance_info->resource_info , 0, sizeof (qapi_Net_LWM2M_Resource_Info_t));
        lwm2m_dataP->instance_info->resource_info->resource_ID = QAPI_NET_LWM2M_SW_MGNT_RES_M_UPDATE_STATE_E;
        lwm2m_dataP->instance_info->resource_info->type = QAPI_NET_LWM2M_TYPE_INTEGER_E;
        lwm2m_dataP->instance_info->resource_info->value.asInteger = QAPI_NET_LWM2M_SW_MGNT_UPDATE_STATE_INITIAL_E; 
        res = qapi_Net_LWM2M_Set_v2 ( gsoftware_mgnt_Handler_v2,lwm2m_dataP);
        if (res == QAPI_ERROR)
        {
          LOG_INFO ("LWM2M_LOG : qapi set got failed \n");
        }
        app_free_data_v2(lwm2m_dataP);

		tx_byte_allocate(byte_pool_qcli, (VOID **)&dataP, sizeof (qapi_Net_LWM2M_Data_v2_t), TX_NO_WAIT);		  		
        if (dataP == NULL)
        {
          LOG_INFO ("memory allocation falied \n");  
          *result = QAPI_ERROR;
           return;
        }
        memset (dataP, 0 , sizeof (qapi_Net_LWM2M_Data_v2_t)); 
        LOG_INFO("LWM2M_LOG: SOFTWARE MANAGEMENT Download complete\n");
        dataP->object_ID = QAPI_NET_LWM2M_SOFTWARE_MGNT_OBJECT_ID_E;
        dataP->no_instances++;
		tx_byte_allocate(byte_pool_qcli, (VOID **)&(dataP->instance_info), sizeof (qapi_Net_LWM2M_Instance_Info_v2_t), TX_NO_WAIT);		  		
        if( NULL == dataP->instance_info) 
        {
          LOG_INFO ("LWM2M_LOG : memory allocation failed \n ");
          app_free_data_v2(dataP);
          *result = QAPI_ERROR;
           return;
        }
        memset (dataP->instance_info,0,sizeof (qapi_Net_LWM2M_Instance_Info_v2_t));
        dataP->instance_info->instance_ID = QAPI_Net_LWM2M_SOFTWARE_MGT_INSTANCE_ID_0;
        dataP->instance_info->no_resources++;
		tx_byte_allocate(byte_pool_qcli, (VOID **)&(dataP->instance_info->resource_info), sizeof (qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);		  		
        if( NULL == dataP->instance_info->resource_info)
        {
          app_free_data_v2(dataP);
          *result = QAPI_ERROR;
           return;
        }
        memset (dataP->instance_info->resource_info ,0,sizeof (qapi_Net_LWM2M_Resource_Info_t));
        dataP->instance_info->resource_info->resource_ID = QAPI_NET_LWM2M_SW_MGNT_RES_M_UPDATE_RESULT_E;
        dataP->instance_info->resource_info->type = QAPI_NET_LWM2M_TYPE_INTEGER_E;
        dataP->instance_info->resource_info->value.asInteger = QAPI_NET_LWM2M_SW_MGNT_UPDATE_RES_NO_ENOUGH_STORAGE_E; 
        res = qapi_Net_LWM2M_Set_v2 ( gsoftware_mgnt_Handler_v2,dataP);
        if (res == QAPI_ERROR)
        {
          LOG_INFO ("LWM2M_LOG : qapi set got failed \n");
        }
        app_free_data_v2(dataP); 

        *result = QAPI_ERROR;
        return;
      }

    } 
  }
  else if (lwm2m_data->instance_info->resource_info->resource_ID == QAPI_NET_LWM2M_SW_MGNT_RES_M_UPDATE_STATE_E)
  {
    if (lwm2m_data->instance_info->resource_info->value.asInteger == QAPI_NET_LWM2M_SW_MGNT_UPDATE_STATE_DOWNLOADED_E) 
    { 
	  tx_byte_allocate(byte_pool_qcli, (VOID **)&lwm2m_dataP, sizeof (qapi_Net_LWM2M_Data_v2_t), TX_NO_WAIT);			  
      if (lwm2m_dataP == NULL)
      {
        LOG_INFO ("memory allocation falied \n");  
        *result = QAPI_ERROR;
         return;
      }
      memset (lwm2m_dataP, 0 , sizeof (qapi_Net_LWM2M_Data_v2_t)); 
      LOG_INFO("LWM2M_LOG: SOFTWARE MANAGEMENT Download complete\n");
      lwm2m_dataP->object_ID = QAPI_NET_LWM2M_SOFTWARE_MGNT_OBJECT_ID_E;
      lwm2m_dataP->no_instances++;
	  tx_byte_allocate(byte_pool_qcli, (VOID **)&(lwm2m_dataP->instance_info), sizeof (qapi_Net_LWM2M_Instance_Info_v2_t), TX_NO_WAIT);			  
      if( NULL == lwm2m_dataP->instance_info) 
      {
        LOG_INFO ("LWM2M_LOG : memory allocation failed \n ");
        app_free_data_v2(lwm2m_dataP);

        *result = QAPI_ERROR;
        return;
      }
      memset (lwm2m_dataP->instance_info,0,sizeof (qapi_Net_LWM2M_Instance_Info_v2_t));
      lwm2m_dataP->instance_info->instance_ID = QAPI_Net_LWM2M_SOFTWARE_MGT_INSTANCE_ID_0;
      lwm2m_dataP->instance_info->no_resources++;
	  tx_byte_allocate(byte_pool_qcli, (VOID **)&(lwm2m_dataP->instance_info->resource_info), sizeof (qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);			  
      if( NULL == lwm2m_dataP->instance_info->resource_info)
      {
        app_free_data_v2(lwm2m_dataP);
        *result = QAPI_ERROR;
        return;
      }
      memset (lwm2m_dataP->instance_info->resource_info,0,sizeof (qapi_Net_LWM2M_Resource_Info_t));
      lwm2m_dataP->instance_info->resource_info->resource_ID = QAPI_NET_LWM2M_SW_MGNT_RES_M_UPDATE_STATE_E;
      lwm2m_dataP->instance_info->resource_info->type = QAPI_NET_LWM2M_TYPE_INTEGER_E;
      lwm2m_dataP->instance_info->resource_info->value.asInteger = QAPI_NET_LWM2M_SW_MGNT_UPDATE_STATE_DELIVERED_E; 
      res = qapi_Net_LWM2M_Set_v2 ( gsoftware_mgnt_Handler_v2,lwm2m_dataP);
      if (res == QAPI_ERROR)
      {
        LOG_INFO ("LWM2M_LOG : qapi set got failed \n");
      }
      app_free_data_v2(lwm2m_dataP);

	  tx_byte_allocate(byte_pool_qcli, (VOID **)&dataP, sizeof (qapi_Net_LWM2M_Data_v2_t), TX_NO_WAIT);			  
      if (dataP == NULL)
      {
        LOG_INFO ("memory allocation falied \n");  
        *result = QAPI_ERROR;
        return;
      }
      memset (dataP, 0 , sizeof (qapi_Net_LWM2M_Data_v2_t)); 
      LOG_INFO("LWM2M_LOG: SOFTWARE MANAGEMENT Download complete\n");
      dataP->object_ID = QAPI_NET_LWM2M_SOFTWARE_MGNT_OBJECT_ID_E;
      dataP->no_instances++;
	  tx_byte_allocate(byte_pool_qcli, (VOID **)&(dataP->instance_info), sizeof (qapi_Net_LWM2M_Instance_Info_v2_t), TX_NO_WAIT);			  
      if( NULL == dataP->instance_info) 
      {
        LOG_INFO ("LWM2M_LOG : memory allocation failed \n ");
        app_free_data_v2(dataP);

        *result = QAPI_ERROR;
        return;
      }
      memset ( dataP->instance_info,0,sizeof (qapi_Net_LWM2M_Instance_Info_v2_t));
      dataP->instance_info->instance_ID = QAPI_Net_LWM2M_SOFTWARE_MGT_INSTANCE_ID_0;
      dataP->instance_info->no_resources++;
	  tx_byte_allocate(byte_pool_qcli, (VOID **)&(dataP->instance_info->resource_info), sizeof (qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);			  
      if( NULL == dataP->instance_info->resource_info)
      {
        app_free_data_v2(dataP);
        *result = QAPI_ERROR;
           return;
      }
      memset ( dataP->instance_info->resource_info,0,sizeof (qapi_Net_LWM2M_Resource_Info_t));
      dataP->instance_info->resource_info->resource_ID = QAPI_NET_LWM2M_SW_MGNT_RES_M_UPDATE_RESULT_E;
      dataP->instance_info->resource_info->type = QAPI_NET_LWM2M_TYPE_INTEGER_E;
      dataP->instance_info->resource_info->value.asInteger = QAPI_NET_LWM2M_SW_MGNT_UPDATE_RES_INITIAL_E; 
      res = qapi_Net_LWM2M_Set_v2 ( gsoftware_mgnt_Handler_v2,dataP);
      if (res == QAPI_ERROR)
      {
        LOG_INFO ("LWM2M_LOG : qapi set got failed \n");
      }
      app_free_data_v2(dataP); 
    }
  }
  else if (lwm2m_data->instance_info->resource_info->resource_ID == QAPI_NET_LWM2M_SW_MGNT_RES_O_PACKAGE_URI_E)
  {
    if(!gsw_mgnt_downloaded_v2)  
      gsw_mgnt_downloaded_v2 = true; 
    LOG_INFO ("LWM2M_LOG : Package URI got \n ");    
    LOG_INFO ("LWM2M_LOG :ENTER THE UPDATED result \n");
	tx_byte_allocate(byte_pool_qcli, (VOID **)&lwm2m_dataP, sizeof (qapi_Net_LWM2M_Data_v2_t), TX_NO_WAIT);				
    if (lwm2m_dataP == NULL)
    {
      LOG_INFO ("memory allocation falied \n");  
      *result = QAPI_ERROR;
       return;
    }
    memset (lwm2m_dataP, 0 , sizeof (qapi_Net_LWM2M_Data_v2_t)); 
    LOG_INFO("LWM2M_LOG: SOFTWARE MANAGEMENT Download complete\n");
    lwm2m_dataP->object_ID = QAPI_NET_LWM2M_SOFTWARE_MGNT_OBJECT_ID_E;
    lwm2m_dataP->no_instances++;
	tx_byte_allocate(byte_pool_qcli, (VOID **)&(lwm2m_dataP->instance_info), sizeof (qapi_Net_LWM2M_Instance_Info_v2_t), TX_NO_WAIT);				
    if( NULL == lwm2m_dataP->instance_info) 
    {
      LOG_INFO ("LWM2M_LOG : memory allocation failed \n ");
      app_free_data_v2(lwm2m_dataP);

     *result = QAPI_ERROR;
      return;
    }
    memset (lwm2m_dataP->instance_info,0,sizeof (qapi_Net_LWM2M_Instance_Info_v2_t));
    lwm2m_dataP->instance_info->instance_ID = QAPI_Net_LWM2M_SOFTWARE_MGT_INSTANCE_ID_0;
    lwm2m_dataP->instance_info->no_resources++;
	tx_byte_allocate(byte_pool_qcli, (VOID **)&(lwm2m_dataP->instance_info->resource_info), sizeof (qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);				
    if( NULL == lwm2m_dataP->instance_info->resource_info)
    {
      app_free_data_v2(lwm2m_dataP);
      *result = QAPI_ERROR;
      return;
    }
    memset (lwm2m_dataP->instance_info->resource_info,0,sizeof (qapi_Net_LWM2M_Resource_Info_t));
    lwm2m_dataP->instance_info->resource_info->resource_ID = QAPI_NET_LWM2M_SW_MGNT_RES_M_UPDATE_STATE_E;
    lwm2m_dataP->instance_info->resource_info->type = QAPI_NET_LWM2M_TYPE_INTEGER_E;
    lwm2m_dataP->instance_info->resource_info->value.asInteger = QAPI_NET_LWM2M_SW_MGNT_UPDATE_STATE_DOWNLOAD_STARTED_E; 
    res = qapi_Net_LWM2M_Set_v2 ( gsoftware_mgnt_Handler_v2,lwm2m_dataP);
    if (res == QAPI_ERROR)
    {
      LOG_INFO ("LWM2M_LOG : qapi set got failed \n");
    }
    app_free_data_v2(lwm2m_dataP);

	tx_byte_allocate(byte_pool_qcli, (VOID **)&(dataP), sizeof (qapi_Net_LWM2M_Data_v2_t), TX_NO_WAIT);				
    if (dataP == NULL)
    {
      LOG_INFO ("memory allocation falied \n");  
      *result = QAPI_ERROR;
      return;
    }
    memset (dataP, 0 , sizeof (qapi_Net_LWM2M_Data_v2_t)); 
    LOG_INFO("LWM2M_LOG: SOFTWARE MANAGEMENT Download complete\n");
    dataP->object_ID = QAPI_NET_LWM2M_SOFTWARE_MGNT_OBJECT_ID_E;
    dataP->no_instances++;
	tx_byte_allocate(byte_pool_qcli, (VOID **)&(dataP->instance_info), sizeof (qapi_Net_LWM2M_Instance_Info_v2_t), TX_NO_WAIT);				
    if( NULL == dataP->instance_info) 
    {
      LOG_INFO ("LWM2M_LOG : memory allocation failed \n ");
      app_free_data_v2(dataP);

     *result = QAPI_ERROR;
      return;
    }
    memset (dataP->instance_info,0,sizeof (qapi_Net_LWM2M_Instance_Info_v2_t));
    dataP->instance_info->instance_ID = QAPI_Net_LWM2M_SOFTWARE_MGT_INSTANCE_ID_0;
    dataP->instance_info->no_resources++;
	tx_byte_allocate(byte_pool_qcli, (VOID **)&(dataP->instance_info->resource_info), sizeof (qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);				
    if( NULL == dataP->instance_info->resource_info)
    {
      app_free_data_v2(dataP);
      *result = QAPI_ERROR;
       return;
    }
    memset (dataP->instance_info->resource_info,0,sizeof (qapi_Net_LWM2M_Resource_Info_t));
    dataP->instance_info->resource_info->resource_ID = QAPI_NET_LWM2M_SW_MGNT_RES_M_UPDATE_RESULT_E;
    dataP->instance_info->resource_info->type = QAPI_NET_LWM2M_TYPE_INTEGER_E;
    dataP->instance_info->resource_info->value.asInteger = QAPI_NET_LWM2M_SW_MGNT_UPDATE_RES_DOWNLOADING_E; 
    res = qapi_Net_LWM2M_Set_v2 ( gsoftware_mgnt_Handler_v2,dataP);
    if (res == QAPI_ERROR)
    {
      LOG_INFO ("LWM2M_LOG : qapi set got failed \n");
    }
    app_free_data_v2(dataP);

  }
  else if (lwm2m_data->instance_info->resource_info->resource_ID == QAPI_NET_LWM2M_SW_MGNT_RES_M_INSTALL_E)
  {
    if((gsw_mgnt_downloaded_v2 == true) && (gsw_mgnt_install_v2 == false)) 
    {
      gsw_mgnt_install_v2 = true;  
      LOG_INFO ("LWM2M_LOG :  GOT INSTALL REQUEST FOR SOFTWARE MGNT is \n");
      LOG_INFO ("ENTER the updated result \n");
    }
  }
  else if (lwm2m_data->instance_info->resource_info->resource_ID == QAPI_NET_LWM2M_SW_MGNT_RES_M_UNINSTALL_E)
  {
    LOG_INFO ("LWM2M_LOG :  GOT UNINSTALL REQUEST FOR SOFTWARE MGNT is \n");
    gsw_mgnt_downloaded_v2 = false ;
    gsw_mgnt_install_v2    = false ;
    gsw_mgnt_activate_v2   = false ; 
	tx_byte_allocate(byte_pool_qcli, (VOID **)&(lwm2m_dataP), sizeof (qapi_Net_LWM2M_Data_v2_t), TX_NO_WAIT);				
    if (lwm2m_dataP == NULL)
    {
      LOG_INFO ("memory allocation falied \n");  
      *result = QAPI_ERROR;
       return;
    }
    memset (lwm2m_dataP, 0 , sizeof (qapi_Net_LWM2M_Data_v2_t)); 
    LOG_INFO("LWM2M_LOG: SOFTWARE MANAGEMENT Download complete\n");
    lwm2m_dataP->object_ID = QAPI_NET_LWM2M_SOFTWARE_MGNT_OBJECT_ID_E;
    lwm2m_dataP->no_instances++;
	tx_byte_allocate(byte_pool_qcli, (VOID **)&(lwm2m_dataP->instance_info ), sizeof (qapi_Net_LWM2M_Instance_Info_v2_t), TX_NO_WAIT);				
    if( NULL == lwm2m_dataP->instance_info) 
    {
      LOG_INFO ("LWM2M_LOG : memory allocation failed \n ");
      app_free_data_v2(lwm2m_dataP);

      *result = QAPI_ERROR;
       return;
    }
    memset (lwm2m_dataP->instance_info,0,sizeof (qapi_Net_LWM2M_Instance_Info_v2_t));
    lwm2m_dataP->instance_info->instance_ID = QAPI_Net_LWM2M_SOFTWARE_MGT_INSTANCE_ID_0;
    lwm2m_dataP->instance_info->no_resources++;
	tx_byte_allocate(byte_pool_qcli, (VOID **)&(lwm2m_dataP->instance_info->resource_info), sizeof (qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);				
    if( NULL == lwm2m_dataP->instance_info->resource_info)
    {
      app_free_data_v2(lwm2m_dataP);
      *result = QAPI_ERROR;
       return;
    }
    memset (lwm2m_dataP->instance_info->resource_info,0,sizeof (qapi_Net_LWM2M_Resource_Info_t));
    lwm2m_dataP->instance_info->resource_info->resource_ID = QAPI_NET_LWM2M_SW_MGNT_RES_M_UPDATE_STATE_E;
    lwm2m_dataP->instance_info->resource_info->type = QAPI_NET_LWM2M_TYPE_INTEGER_E;
    lwm2m_dataP->instance_info->resource_info->value.asInteger = QAPI_NET_LWM2M_SW_MGNT_UPDATE_STATE_INITIAL_E; 
    res = qapi_Net_LWM2M_Set_v2 ( gsoftware_mgnt_Handler_v2,lwm2m_dataP);
    if (res == QAPI_ERROR)
    {
      LOG_INFO ("LWM2M_LOG : qapi set got failed \n");
    }
    app_free_data_v2(lwm2m_dataP);

	tx_byte_allocate(byte_pool_qcli, (VOID **)&(dataP), sizeof (qapi_Net_LWM2M_Data_v2_t), TX_NO_WAIT);				

    if (dataP == NULL)
    {
      LOG_INFO ("memory allocation falied \n");  
      *result = QAPI_ERROR;
       return;
    }
    memset (dataP, 0 , sizeof (qapi_Net_LWM2M_Data_v2_t)); 
    LOG_INFO("LWM2M_LOG: SOFTWARE MANAGEMENT Download complete\n");
    dataP->object_ID = QAPI_NET_LWM2M_SOFTWARE_MGNT_OBJECT_ID_E;
    dataP->no_instances++;
	tx_byte_allocate(byte_pool_qcli, (VOID **)&dataP->instance_info, sizeof (qapi_Net_LWM2M_Instance_Info_v2_t), TX_NO_WAIT);				
    if( NULL == dataP->instance_info) 
    {
      LOG_INFO ("LWM2M_LOG : memory allocation failed \n ");
      app_free_data_v2(dataP);

      *result = QAPI_ERROR;
       return;
    }
    memset (dataP->instance_info,0,sizeof (qapi_Net_LWM2M_Instance_Info_v2_t));
    dataP->instance_info->instance_ID = QAPI_Net_LWM2M_SOFTWARE_MGT_INSTANCE_ID_0;
    dataP->instance_info->no_resources++;
	tx_byte_allocate(byte_pool_qcli, (VOID **)&(dataP->instance_info->resource_info), sizeof (qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);				
    if( NULL == dataP->instance_info->resource_info)
    {
      app_free_data_v2(dataP);
      *result = QAPI_ERROR;
       return;
    }
    memset (dataP->instance_info->resource_info,0,sizeof (qapi_Net_LWM2M_Resource_Info_t));
    dataP->instance_info->resource_info->resource_ID = QAPI_NET_LWM2M_SW_MGNT_RES_M_UPDATE_RESULT_E;
    dataP->instance_info->resource_info->type = QAPI_NET_LWM2M_TYPE_INTEGER_E;
    dataP->instance_info->resource_info->value.asInteger = QAPI_NET_LWM2M_SW_MGNT_UPDATE_STATE_INITIAL_E; 
    res = qapi_Net_LWM2M_Set_v2 ( gsoftware_mgnt_Handler_v2,dataP);
    if (res == QAPI_ERROR)
    {
      LOG_INFO ("LWM2M_LOG : qapi set got failed \n");
    }
    app_free_data_v2(dataP);
    dataP = NULL;  
	tx_byte_allocate(byte_pool_qcli, (VOID **)&(dataP), sizeof (qapi_Net_LWM2M_Data_v2_t), TX_NO_WAIT);				
    if (dataP == NULL)
    {
      LOG_INFO ("memory allocation falied \n");  
      *result = QAPI_ERROR;
       return;
    }
    memset (dataP, 0 , sizeof (qapi_Net_LWM2M_Data_v2_t)); 
    LOG_INFO("LWM2M_LOG: SOFTWARE MANAGEMENT Download complete\n");
    dataP->object_ID = QAPI_NET_LWM2M_SOFTWARE_MGNT_OBJECT_ID_E;
    dataP->no_instances++;
	tx_byte_allocate(byte_pool_qcli, (VOID **)&(dataP->instance_info), sizeof (qapi_Net_LWM2M_Instance_Info_v2_t), TX_NO_WAIT);				
    if( NULL == dataP->instance_info) 
    {
      LOG_INFO ("LWM2M_LOG : memory allocation failed \n ");
      app_free_data_v2(dataP);

      *result = QAPI_ERROR;
       return;
    }
    memset (dataP->instance_info,0,sizeof (qapi_Net_LWM2M_Instance_Info_v2_t));
    dataP->instance_info->instance_ID = QAPI_Net_LWM2M_SOFTWARE_MGT_INSTANCE_ID_0;
    dataP->instance_info->no_resources++;
	tx_byte_allocate(byte_pool_qcli, (VOID **)&(dataP->instance_info->resource_info), sizeof (qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);				
    if( NULL == dataP->instance_info->resource_info)
    {
      app_free_data_v2(dataP);
      *result = QAPI_ERROR;
       return;
    }
    memset (dataP->instance_info->resource_info,0,sizeof (qapi_Net_LWM2M_Resource_Info_t));
    dataP->instance_info->resource_info->resource_ID = QAPI_NET_LWM2M_SW_MGNT_RES_M_ACTIVATION_STATE_E;
    dataP->instance_info->resource_info->type = QAPI_NET_LWM2M_TYPE_INTEGER_E;
    dataP->instance_info->resource_info->value.asInteger = false; 
    res = qapi_Net_LWM2M_Set_v2 ( gsoftware_mgnt_Handler_v2,dataP);
    if (res == QAPI_ERROR)
    {
      LOG_INFO ("LWM2M_LOG : qapi set got failed \n");
    }
    app_free_data_v2(dataP);

  }
  else if (lwm2m_data->instance_info->resource_info->resource_ID == QAPI_NET_LWM2M_SW_MGNT_RES_M_ACTIVATE_E)
  {
    if ((gsw_mgnt_downloaded_v2 == true) && (gsw_mgnt_install_v2 == true) && (gsw_mgnt_activate_v2 == false))
    {
      LOG_INFO ("LWM2M_LOG : GOT ACTIVATE REQUEST FOR SOFTWARE MGNT \n");
      LOG_INFO ("LWM2M_LOG : Enter the upated the result \n");
      gsw_mgnt_activate_v2 =  true;        
    }
  }
  else if (lwm2m_data->instance_info->resource_info->resource_ID == QAPI_NET_LWM2M_SW_MGNT_RES_M_DEACTIVATE_E)
  {
    LOG_INFO ("LWM2M_LOG : GOT DEACTIVATE REQUEST FOR SOFTWARE MGNT\n"); 
	tx_byte_allocate(byte_pool_qcli, (VOID **)&(lwm2m_dataP), sizeof (qapi_Net_LWM2M_Data_v2_t), TX_NO_WAIT);				
    if (lwm2m_dataP == NULL)
    {
      LOG_INFO ("memory allocation falied \n");  
      *result = QAPI_ERROR;
       return;
    }
    memset (lwm2m_dataP, 0 , sizeof (qapi_Net_LWM2M_Data_v2_t)); 
    LOG_INFO("LWM2M_LOG: SOFTWARE MANAGEMENT Download complete\n");
    lwm2m_dataP->object_ID = QAPI_NET_LWM2M_SOFTWARE_MGNT_OBJECT_ID_E;
    lwm2m_dataP->no_instances++;
	tx_byte_allocate(byte_pool_qcli, (VOID **)&(lwm2m_dataP->instance_info), sizeof (qapi_Net_LWM2M_Instance_Info_v2_t), TX_NO_WAIT);				
    if( NULL == lwm2m_dataP->instance_info) 
    {
      LOG_INFO ("LWM2M_LOG : memory allocation failed \n ");
      app_free_data_v2(lwm2m_dataP);

      *result = QAPI_ERROR;
       return;
    }
    memset (lwm2m_dataP->instance_info,0,sizeof (qapi_Net_LWM2M_Instance_Info_v2_t));
    lwm2m_dataP->instance_info->instance_ID = QAPI_Net_LWM2M_SOFTWARE_MGT_INSTANCE_ID_0;
    lwm2m_dataP->instance_info->no_resources++;
	tx_byte_allocate(byte_pool_qcli, (VOID **)&(lwm2m_dataP->instance_info->resource_info), sizeof (qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);				
    if( NULL == lwm2m_dataP->instance_info->resource_info)
    {
      app_free_data_v2(lwm2m_dataP);
      *result = QAPI_ERROR;
       return;
    }
    memset (lwm2m_dataP->instance_info->resource_info,0,sizeof (qapi_Net_LWM2M_Resource_Info_t));
    lwm2m_dataP->instance_info->resource_info->resource_ID = QAPI_NET_LWM2M_SW_MGNT_RES_M_ACTIVATION_STATE_E;
    lwm2m_dataP->instance_info->resource_info->type = QAPI_NET_LWM2M_TYPE_INTEGER_E;;
    lwm2m_dataP->instance_info->resource_info->value.asInteger = false; 
    res = qapi_Net_LWM2M_Set_v2( gsoftware_mgnt_Handler_v2,lwm2m_dataP);
    if (res == QAPI_ERROR)
    {
      LOG_INFO ("LWM2M_LOG : set value got failed \n");
    }
    app_free_data_v2(lwm2m_dataP);

  }
  memset (&lwm2m_info_req , 0 , sizeof (qapi_Net_LWM2M_Object_Info_v2_t));
  lwm2m_info_req.no_object_info++;
  tx_byte_allocate(byte_pool_qcli, (VOID **)&(lwm2m_info_req.id_info), sizeof (qapi_Net_LWM2M_Id_Info_v2_t), TX_NO_WAIT);			  
  if (lwm2m_info_req.id_info != NULL)
  { 

    qapi_Net_LWM2M_Data_v2_t *lwm2m_dataP = NULL;
    memset (lwm2m_info_req.id_info , 0 , sizeof (qapi_Net_LWM2M_Id_Info_v2_t));
    LOG_INFO ("LWM2M_LOG : INVOKING THE GET QAPI \n");
    lwm2m_info_req.id_info->id_set = lwm2m_info_req.id_info->id_set  | QAPI_NET_LWM2M_OBJECT_ID_E;
    lwm2m_info_req.id_info->id_set = lwm2m_info_req.id_info->id_set  | QAPI_NET_LWM2M_INSTANCE_ID_E ;
    lwm2m_info_req.id_info->id_set = lwm2m_info_req.id_info->id_set  | QAPI_NET_LWM2M_RESOURCE_ID_E;
    lwm2m_info_req.id_info->object_ID = QAPI_NET_LWM2M_SOFTWARE_MGNT_OBJECT_ID_E;
    lwm2m_info_req.id_info->instance_ID = lwm2m_data->instance_info->instance_ID;
    lwm2m_info_req.id_info->resource_ID = QAPI_NET_LWM2M_SW_MGNT_RES_M_UPDATE_STATE_E; 
    lwm2m_info_req.id_info->next =NULL;
    //result = qapi_Net_LWM2M_Get (gsoftware_mgnt_Handler, &lwm2m_info_req , &lwm2m_dataP);
    if (res == QAPI_ERROR)
    {
      LOG_INFO ("LWM2M_LOG : qapi get got failed \n");
    }
    if (lwm2m_dataP != NULL)
    {
      LOG_INFO ("LWM2M_LOG : VALUE GOT IS %d\n",lwm2m_dataP->instance_info->resource_info->value.asInteger);
    }
    app_free_id_info_v2 (lwm2m_info_req.id_info);
  }

  *result = QAPI_OK;
   return;
}

qapi_Status_t software_mngt_reg_callback(qapi_Net_LWM2M_App_Handler_t handler, 
                                       qapi_Net_LWM2M_Server_Data_t *lwm2m_srv_data,
                                       void                         *user_data)
{
  return QAPI_OK;
}

/**
 * @fn     software_mngt_init_v2()
 *
 * @brief  This fucntion is used to initialise SOFTWARE MANAGEMENT application.
 *
 * @param  void
 * @return void
 */
void software_mngt_init_v2()
{

  int32_t ret_value = 0;
  qapi_Status_t result = QAPI_OK;
  qapi_Net_LWM2M_Object_Info_v2_t observe_info;
  qapi_Net_LWM2M_Id_Info_v2_t * id_info1 = NULL;
  qapi_Net_LWM2M_Id_Info_v2_t * id_info2 = NULL; 
  qapi_Net_LWM2M_Id_Info_v2_t * id_info3 = NULL;
  qapi_Net_LWM2M_Id_Info_v2_t * id_info4 = NULL;
  qapi_Net_LWM2M_Id_Info_v2_t * id_info5 = NULL; 
  qapi_Net_LWM2M_Id_Info_v2_t * id_info6 = NULL;
  qapi_Net_LWM2M_Id_Info_v2_t * id_info7 = NULL; 
  
  void *user_data = NULL;

  qapi_Net_LWM2M_Id_Info_v2_t * id_info_prev = NULL;

  if (gsoftware_mgnt_Handler_v2 != NULL)
  {
    LOG_INFO ("LWM2M_LOG : SOFTWARE MANAGEMENT  APP ALREADY STARTED \n");
    return ;
  }
  if(QAPI_OK ==  qapi_Net_LWM2M_Register_App_Extended(&gsoftware_mgnt_Handler_v2, user_data, software_mngt_reg_callback))
  {
    qapi_Net_LWM2M_Pass_Pool_Ptr(gsoftware_mgnt_Handler_v2,byte_pool_qcli);

    LOG_INFO ("SOFTWARE MANAGEMENT APP STARTED \n");  
    memset (&observe_info , 0 ,sizeof (qapi_Net_LWM2M_Object_Info_v2_t));  
    LOG_INFO ("LWM2M_LOG : INVOKING CREATE INSTANCE API \n"); 
    ret_value = sw_mgnt_handle_create_instance_v2();
    if (ret_value != 0)
    {
      LOG_INFO ("LWM2M_LOG : [SOFTWARE MNGT]CRATING  OBJECT INSTANCE GOT FAILED \n");
      return ;
    }

    observe_info.no_object_info++;
	tx_byte_allocate(byte_pool_qcli, (VOID **)&(id_info1), sizeof (qapi_Net_LWM2M_Id_Info_v2_t), TX_NO_WAIT);			
	
    if(NULL == id_info1)
    {
      return;        
    }
    memset(id_info1 , 0 , sizeof (qapi_Net_LWM2M_Id_Info_v2_t));
    id_info1->id_set = id_info1->id_set | QAPI_NET_LWM2M_OBJECT_ID_E;
    id_info1->id_set = id_info1->id_set | QAPI_NET_LWM2M_INSTANCE_ID_E;
    id_info1->id_set = id_info1->id_set | QAPI_NET_LWM2M_RESOURCE_ID_E;
    id_info1->object_ID = QAPI_NET_LWM2M_SOFTWARE_MGNT_OBJECT_ID_E;
    id_info1->instance_ID = QAPI_Net_LWM2M_SOFTWARE_MGT_INSTANCE_ID_0;
    id_info1->resource_ID = QAPI_NET_LWM2M_SW_MGNT_RES_O_PACKAGE_E; // 1

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
	tx_byte_allocate(byte_pool_qcli, (VOID **)&(id_info2), sizeof (qapi_Net_LWM2M_Id_Info_v2_t), TX_NO_WAIT);			
    if(NULL == id_info2)
    {
      app_free_id_info_v2 (id_info1);
      return;        
    }
    memset(id_info2 , 0 , sizeof (qapi_Net_LWM2M_Id_Info_v2_t));
    id_info2->id_set = id_info2->id_set | QAPI_NET_LWM2M_OBJECT_ID_E;
    id_info2->id_set = id_info2->id_set | QAPI_NET_LWM2M_INSTANCE_ID_E;
    id_info2->id_set = id_info2->id_set | QAPI_NET_LWM2M_RESOURCE_ID_E;
    id_info2->object_ID = QAPI_NET_LWM2M_SOFTWARE_MGNT_OBJECT_ID_E;
    id_info2->instance_ID = QAPI_Net_LWM2M_SOFTWARE_MGT_INSTANCE_ID_0;
    id_info2->resource_ID = QAPI_NET_LWM2M_SW_MGNT_RES_O_PACKAGE_URI_E; // 3

    id_info1->next = id_info2;

    observe_info.no_object_info++;
	tx_byte_allocate(byte_pool_qcli, (VOID **)&(id_info3), sizeof (qapi_Net_LWM2M_Id_Info_v2_t), TX_NO_WAIT);			
    if(NULL == id_info3)
    {
      app_free_id_info_v2 (id_info1);
      return;        
    }
    memset(id_info3 , 0 , sizeof (qapi_Net_LWM2M_Id_Info_v2_t));
    id_info3->id_set = id_info3->id_set | QAPI_NET_LWM2M_OBJECT_ID_E;
    id_info3->id_set = id_info3->id_set | QAPI_NET_LWM2M_INSTANCE_ID_E;
    id_info3->id_set = id_info3->id_set | QAPI_NET_LWM2M_RESOURCE_ID_E;
    id_info3->object_ID = QAPI_NET_LWM2M_SOFTWARE_MGNT_OBJECT_ID_E;
    id_info3->instance_ID = QAPI_Net_LWM2M_SOFTWARE_MGT_INSTANCE_ID_0;
    id_info3->resource_ID = QAPI_NET_LWM2M_SW_MGNT_RES_M_INSTALL_E;   // 4
    id_info3->next = NULL;
    id_info2->next = id_info3;

    observe_info.no_object_info++;
	tx_byte_allocate(byte_pool_qcli, (VOID **)&(id_info4), sizeof (qapi_Net_LWM2M_Id_Info_v2_t), TX_NO_WAIT);			
    if(NULL == id_info4)
    {
      app_free_id_info_v2 (id_info1);
      return;        
    }

    memset(id_info4, 0 , sizeof (qapi_Net_LWM2M_Id_Info_v2_t));
    id_info4->id_set = id_info4->id_set | QAPI_NET_LWM2M_OBJECT_ID_E;
    id_info4->id_set = id_info4->id_set | QAPI_NET_LWM2M_INSTANCE_ID_E;
    id_info4->id_set = id_info4->id_set | QAPI_NET_LWM2M_RESOURCE_ID_E;
    id_info4->object_ID = QAPI_NET_LWM2M_SOFTWARE_MGNT_OBJECT_ID_E;
    id_info4->instance_ID = QAPI_Net_LWM2M_SOFTWARE_MGT_INSTANCE_ID_0;
    id_info4->resource_ID = QAPI_NET_LWM2M_SW_MGNT_RES_M_UNINSTALL_E;  // 6
    id_info4->next = NULL;
    id_info3 -> next = id_info4;

    observe_info.no_object_info++;
	tx_byte_allocate(byte_pool_qcli, (VOID **)&(id_info5), sizeof (qapi_Net_LWM2M_Id_Info_v2_t), TX_NO_WAIT);			
    if(NULL == id_info5)
    {
      app_free_id_info_v2 (id_info1);
      return;        
    }

    memset(id_info5 , 0 , sizeof (qapi_Net_LWM2M_Id_Info_v2_t));
    id_info5->id_set = id_info5->id_set | QAPI_NET_LWM2M_OBJECT_ID_E;
    id_info5->id_set = id_info5->id_set | QAPI_NET_LWM2M_INSTANCE_ID_E;
    id_info5->id_set = id_info5->id_set | QAPI_NET_LWM2M_RESOURCE_ID_E;
    id_info5->object_ID = QAPI_NET_LWM2M_SOFTWARE_MGNT_OBJECT_ID_E;
    id_info5->instance_ID = QAPI_Net_LWM2M_SOFTWARE_MGT_INSTANCE_ID_0;
    id_info5->resource_ID = QAPI_NET_LWM2M_SW_MGNT_RES_M_ACTIVATE_E;   // 10
    id_info5->next = NULL;
    id_info4->next = id_info5;

    observe_info.no_object_info++;
	tx_byte_allocate(byte_pool_qcli, (VOID **)&(id_info6), sizeof (qapi_Net_LWM2M_Id_Info_v2_t), TX_NO_WAIT);			
    if(NULL == id_info6)
    {
      app_free_id_info_v2 (id_info1);
      return;        
    }

    memset(id_info6 , 0 , sizeof (qapi_Net_LWM2M_Id_Info_v2_t));
    id_info6->id_set = id_info6->id_set | QAPI_NET_LWM2M_OBJECT_ID_E;
    id_info6->id_set = id_info6->id_set | QAPI_NET_LWM2M_INSTANCE_ID_E;
    id_info6->id_set = id_info6->id_set | QAPI_NET_LWM2M_RESOURCE_ID_E;
    id_info6->object_ID = QAPI_NET_LWM2M_SOFTWARE_MGNT_OBJECT_ID_E;
    id_info6->instance_ID = QAPI_Net_LWM2M_SOFTWARE_MGT_INSTANCE_ID_0;
    id_info6->resource_ID = QAPI_NET_LWM2M_SW_MGNT_RES_M_DEACTIVATE_E;
    id_info6->next = NULL;
    id_info5->next = id_info6;

    observe_info.no_object_info++;
	tx_byte_allocate(byte_pool_qcli, (VOID **)&(id_info7), sizeof (qapi_Net_LWM2M_Id_Info_v2_t), TX_NO_WAIT);			
    if(NULL == id_info7)
    {
      app_free_id_info_v2 (id_info1);
      return;        
    }

    memset(id_info7 , 0 , sizeof (qapi_Net_LWM2M_Id_Info_v2_t));
    id_info7->id_set = id_info7->id_set | QAPI_NET_LWM2M_OBJECT_ID_E;
    id_info7->id_set = id_info7->id_set | QAPI_NET_LWM2M_INSTANCE_ID_E;
    id_info7->id_set = id_info7->id_set | QAPI_NET_LWM2M_RESOURCE_ID_E;
    id_info7->object_ID = QAPI_NET_LWM2M_SOFTWARE_MGNT_OBJECT_ID_E;
    id_info7->instance_ID = QAPI_Net_LWM2M_SOFTWARE_MGT_INSTANCE_ID_0;
    id_info7->resource_ID = QAPI_NET_LWM2M_SW_MGNT_RES_M_UPDATE_STATE_E;  
    id_info7->next = NULL;
    id_info6->next = id_info7;



    result = qapi_Net_LWM2M_Observe_v2(gsoftware_mgnt_Handler_v2, software_mgnt_app_callback_v2, &observe_info);
    if (result == QAPI_ERROR)
    {
      LOG_INFO ("[SWMGNT] : qapi observe failed \n ");
    }
    app_free_id_info_v2 (id_info1);

  }

  //qapi_Net_LWM2M_DeRegister_App(handler);
  //handler = NULL;          

}
/*
 * @fn     void sw_mgnt_handle_update_state_input_v2() 
 * @brief  This function is used to update the  state  of software management resource 
 * @param  option - value given from cli 
 * @return void
 */
void sw_mgnt_handle_update_state_input_v2(int32_t option) 
{
  qapi_Net_LWM2M_Data_v2_t * lwm2m_data = NULL;
  qapi_Net_LWM2M_Data_v2_t * lwm2m_dataP = NULL;
  qapi_Status_t result = QAPI_OK;

  if (gsw_mgnt_downloaded_v2 != true ) 
  {
    LOG_INFO( "LWM2M_LOG : setting value is not allowed \n"); 
    return ;
  }
  tx_byte_allocate(byte_pool_qcli, (VOID **)&(lwm2m_data), sizeof (qapi_Net_LWM2M_Data_v2_t), TX_NO_WAIT);		  
  if (lwm2m_data == NULL) 
  {
    LOG_INFO ("LWM2M_LOG :  MALLOC  FAILURE \n"); 
    return ;
  }
  memset (lwm2m_data , 0 , sizeof (qapi_Net_LWM2M_Data_v2_t));
  tx_byte_allocate(byte_pool_qcli, (VOID **)&(lwm2m_data->instance_info), sizeof (qapi_Net_LWM2M_Instance_Info_v2_t), TX_NO_WAIT);		  
  if (lwm2m_data->instance_info == NULL) 
  {
    LOG_INFO ("LWM2M_LOG :  MALLOC  FAILURE \n");
    app_free_data_v2(lwm2m_data);
    return ;
  }
  memset (lwm2m_data->instance_info , 0 , sizeof (qapi_Net_LWM2M_Instance_Info_v2_t));
  tx_byte_allocate(byte_pool_qcli, (VOID **)&(lwm2m_data->instance_info->resource_info), sizeof (qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);		  
  if (lwm2m_data->instance_info->resource_info == NULL) 
  {
    LOG_INFO ("LWM2M_LOG :  MALLOC  FAILURE \n");
    app_free_data_v2(lwm2m_data);
    return ;
  }
  memset (lwm2m_data->instance_info->resource_info , 0 , sizeof (qapi_Net_LWM2M_Resource_Info_t));
  lwm2m_data->object_ID = QAPI_NET_LWM2M_SOFTWARE_MGNT_OBJECT_ID_E;
  lwm2m_data->no_instances++;
  lwm2m_data->instance_info->instance_ID = QAPI_Net_LWM2M_SOFTWARE_MGT_INSTANCE_ID_0;
  lwm2m_data->instance_info->no_resources++;
  lwm2m_data->instance_info->resource_info->resource_ID = QAPI_NET_LWM2M_SW_MGNT_RES_M_UPDATE_RESULT_E;
  lwm2m_data->instance_info->resource_info->type = QAPI_NET_LWM2M_TYPE_INTEGER_E;

  tx_byte_allocate(byte_pool_qcli, (VOID **)&(lwm2m_dataP), sizeof (qapi_Net_LWM2M_Data_v2_t), TX_NO_WAIT);		  
  if (lwm2m_dataP == NULL) 
  {
    LOG_INFO ("LWM2M_LOG :  MALLOC  FAILURE \n"); 
    app_free_data_v2(lwm2m_data);
    return ;
  }
  memset (lwm2m_dataP , 0 , sizeof (qapi_Net_LWM2M_Data_v2_t));
  tx_byte_allocate(byte_pool_qcli, (VOID **)&(lwm2m_dataP->instance_info), sizeof (qapi_Net_LWM2M_Instance_Info_v2_t), TX_NO_WAIT);		  
  if (lwm2m_dataP->instance_info == NULL) 
  {
    LOG_INFO ("LWM2M_LOG :  MALLOC  FAILURE \n"); 
    app_free_data_v2(lwm2m_data);
    app_free_data_v2(lwm2m_dataP);
    return ;
  }
  memset (lwm2m_dataP->instance_info , 0 , sizeof (qapi_Net_LWM2M_Instance_Info_v2_t));
  tx_byte_allocate(byte_pool_qcli, (VOID **)&(lwm2m_dataP->instance_info->resource_info), sizeof (qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);		  
  if (lwm2m_dataP->instance_info->resource_info == NULL) 
  {
    LOG_INFO ("LWM2M_LOG :  MALLOC  FAILURE \n"); 
    app_free_data_v2(lwm2m_data);
    app_free_data_v2(lwm2m_dataP);  
    return ;
  }
  memset (lwm2m_dataP->instance_info->resource_info , 0 , sizeof (qapi_Net_LWM2M_Resource_Info_t));
  lwm2m_dataP->object_ID = QAPI_NET_LWM2M_SOFTWARE_MGNT_OBJECT_ID_E;
  lwm2m_dataP->no_instances++;
  lwm2m_dataP->instance_info->instance_ID = QAPI_Net_LWM2M_SOFTWARE_MGT_INSTANCE_ID_0;
  lwm2m_dataP->instance_info->no_resources++;

  //lwm2m_dataP->instance_info->resource_info->resource_ID = QAPI_NET_LWM2M_SW_MGNT_RES_M_UPDATE_STATE_E;
  lwm2m_dataP->instance_info->resource_info->type = QAPI_NET_LWM2M_TYPE_INTEGER_E;
  if (((option == 1) && (gsw_mgnt_downloaded_v2 == true) ) && ((gsw_mgnt_install_v2 == true) && (gsw_mgnt_activate_v2 == true))) 
  {
    LOG_INFO ("LWM2M_LOG : OPTION SELECTED is : %d\n",option);
    LOG_INFO ("LWM2M_LOG : STATE IS SET  TO ACTIVATED  \n");
	lwm2m_dataP->instance_info->resource_info->type = QAPI_NET_LWM2M_TYPE_BOOLEAN_E;
    lwm2m_dataP->instance_info->resource_info->resource_ID = QAPI_NET_LWM2M_SW_MGNT_RES_M_ACTIVATION_STATE_E;
    lwm2m_dataP->instance_info->resource_info->value.asBoolean = true;
    //gsw_mgnt_activate == false; 
  }
  else if  (((option == 0) && (gsw_mgnt_downloaded_v2 == true) ) && ((gsw_mgnt_install_v2 == true) && (gsw_mgnt_activate_v2 == true)))   
  {
    LOG_INFO ("LWM2M_LOG : OPTION SELECTED is : %d\n",option);
    LOG_INFO ("LWM2M_LOG : STATE IS REVERTED  TO DELIVERED  \n"); 
	lwm2m_dataP->instance_info->resource_info->type = QAPI_NET_LWM2M_TYPE_BOOLEAN_E;
    lwm2m_dataP->instance_info->resource_info->resource_ID = QAPI_NET_LWM2M_SW_MGNT_RES_M_ACTIVATION_STATE_E;   
    lwm2m_dataP->instance_info->resource_info->value.asBoolean = false;   
    gsw_mgnt_activate_v2 = false;
  } 
  else
  {
    lwm2m_dataP->instance_info->resource_info->resource_ID = QAPI_NET_LWM2M_SW_MGNT_RES_M_UPDATE_STATE_E;
    if ( (((option == 11) || (option == 10)) && (gsw_mgnt_downloaded_v2 == true) ) && ((gsw_mgnt_install_v2 == false) && (gsw_mgnt_activate_v2 == false))) 
    {
      if(option  == 10)
      {
        lwm2m_dataP->instance_info->resource_info->value.asInteger = QAPI_NET_LWM2M_SW_MGNT_UPDATE_STATE_DOWNLOADED_E;
      }
      else if (option == 11)
      {
        lwm2m_dataP->instance_info->resource_info->value.asInteger = QAPI_NET_LWM2M_SW_MGNT_UPDATE_STATE_DELIVERED_E;
      }
      lwm2m_data->instance_info->resource_info->value.asInteger = QAPI_NET_LWM2M_SW_MGNT_UPDATE_RES_INITIAL_E;  // initial state

    }
    else if (((option == QAPI_NET_LWM2M_SW_MGNT_UPDATE_RES_OUT_OF_MEMORY_E || option == QAPI_NET_LWM2M_SW_MGNT_UPDATE_RES_CONNECTION_LOST_E || \
            option == QAPI_NET_LWM2M_SW_MGNT_UPDATE_RES_PKG_UNSUPPORTED_E || option == QAPI_NET_LWM2M_SW_MGNT_UPDATE_RES_INVALID_URI_E || \
            option == QAPI_NET_LWM2M_SW_MGNT_UPDATE_RES_NO_ENOUGH_STORAGE_E || QAPI_NET_LWM2M_SW_MGNT_UPDATE_RES_PKG_CHECK_FAILURE_E ) && \
          (gsw_mgnt_downloaded_v2 == true) ) && ((gsw_mgnt_install_v2 == false) && (gsw_mgnt_activate_v2 == false)))
    {
      lwm2m_dataP->instance_info->resource_info->value.asInteger = QAPI_NET_LWM2M_SW_MGNT_UPDATE_STATE_INITIAL_E;

      lwm2m_data->instance_info->resource_info->value.asInteger = option; 
      gsw_mgnt_downloaded_v2 = false;        
    } else if (((option == 12) && (gsw_mgnt_downloaded_v2== true) ) && ((gsw_mgnt_install_v2 == true) && (gsw_mgnt_activate_v2 == false))) 
    {
      lwm2m_dataP->instance_info->resource_info->value.asInteger = QAPI_NET_LWM2M_SW_MGNT_UPDATE_STATE_INSTALLED_E;
      lwm2m_data->instance_info->resource_info->value.asInteger = QAPI_NET_LWM2M_SW_MGNT_UPDATE_RES_INSTALL_SUCCESS_E;   // successfully installed 
    } 
    else if (((/*option == QAPI_NET_LWM2M_SW_MGNT_UPDATE_RES_NO_ENOUGH_STORAGE_E || option == QAPI_NET_LWM2M_SW_MGNT_UPDATE_RES_PKG_CHECK_FAILURE_E ||*/ \
            option == QAPI_NET_LWM2M_SW_MGNT_UPDATE_RES_UPDATE_ERROR_E || option == QAPI_NET_LWM2M_SW_MGNT_UPDATE_RES_INSTALL_ERROR_E ) ) && \
        ((gsw_mgnt_downloaded_v2 == true) && (gsw_mgnt_install_v2 == true) && (gsw_mgnt_activate_v2 == false)))
    {
      lwm2m_dataP->instance_info->resource_info->value.asInteger =  QAPI_NET_LWM2M_SW_MGNT_UPDATE_STATE_DELIVERED_E;
      lwm2m_data->instance_info->resource_info->value.asInteger =   option; 
      gsw_mgnt_install_v2 = false;
      LOG_INFO ("LWM2M_LOG : OPTION SELECTED is : %d\n",option);
      LOG_INFO ("LWM2M_LOG : STATE IS REVERED BACK TO DELIVERED \n");
    } 
    else {
      LOG_INFO ("LWM2M_LOG : please enter valid updated result based on valid opertaion on firmware  \n");  
      app_free_data_v2(lwm2m_data);
      app_free_data_v2(lwm2m_dataP);
      return ;
    }
  }
  if (option != 1 && option != 0) 
  {    
    result = qapi_Net_LWM2M_Set_v2 ( gsoftware_mgnt_Handler_v2,lwm2m_data);
    if (result == QAPI_ERROR)
    {
      LOG_INFO ("LWM2M_LOG : set value got failed \n");
    }
  }
  result = qapi_Net_LWM2M_Set_v2 ( gsoftware_mgnt_Handler_v2,lwm2m_dataP);
  if (result == QAPI_ERROR)
  {
    LOG_INFO ("LWM2M_LOG : set value got failed \n");
  }


  app_free_data_v2(lwm2m_data);
  app_free_data_v2(lwm2m_dataP);

  return ;
}

/*
 * @fn     static int32_t sw_mgnt_handle_create_instance_v2() 
 * @brief  This function is used to create the software management object instance 
 * @param  void
 * @return on success - 0
 on failure - -1
 */

static int32_t sw_mgnt_handle_create_instance_v2() 
{
  qapi_Net_LWM2M_Data_v2_t lwm2m_data;
  qapi_Net_LWM2M_Resource_Info_t *resource_info = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource_info1 = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource_info2 = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource_info3 = NULL;
  qapi_Status_t result = QAPI_OK;
  if (gsoftware_mgnt_Handler_v2 == NULL ) 
  {
    LOG_INFO( "LWM2M_LOG : create is not allowed as application is not started  \n"); 
    return -1;
  }
  memset (&lwm2m_data , 0 , sizeof (qapi_Net_LWM2M_Data_v2_t));
  lwm2m_data.object_ID    = QAPI_NET_LWM2M_SOFTWARE_MGNT_OBJECT_ID_E;
  lwm2m_data.no_instances++;
  tx_byte_allocate(byte_pool_qcli, (VOID **)&(lwm2m_data.instance_info), sizeof (qapi_Net_LWM2M_Instance_Info_v2_t), TX_NO_WAIT);		  
  if (lwm2m_data.instance_info == NULL) 
  {
    LOG_INFO ("LWM2M_LOG : MALLOC FAILURE \n");  
    return -1;
  }
  memset (lwm2m_data.instance_info  , 0 ,sizeof (qapi_Net_LWM2M_Instance_Info_v2_t));
  lwm2m_data.instance_info->instance_ID = QAPI_Net_LWM2M_SOFTWARE_MGT_INSTANCE_ID_0;
  lwm2m_data.instance_info->no_resources++;
  tx_byte_allocate(byte_pool_qcli, (VOID **)&(lwm2m_data.instance_info->resource_info), sizeof (qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);		  
  if (lwm2m_data.instance_info->resource_info == NULL) 
  {
    LOG_INFO ("LWM2M_LOG : MALLOC FAILURE \n");  
    return -1;
  } 
  memset (lwm2m_data.instance_info->resource_info, 0 , sizeof (qapi_Net_LWM2M_Resource_Info_t));   
  lwm2m_data.instance_info->resource_info->type  = QAPI_NET_LWM2M_TYPE_INTEGER_E;
  lwm2m_data.instance_info->resource_info->resource_ID = QAPI_NET_LWM2M_SW_MGNT_RES_M_UPDATE_RESULT_E;
  lwm2m_data.instance_info->resource_info->value.asInteger = 0;


  tx_byte_allocate(byte_pool_qcli, (VOID **)&(resource_info), sizeof (qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);		  
  if (resource_info == NULL)
  {
    LOG_INFO ("LWM2M_LOG : MALLOC FAILURE \n");
    return -1;
  }
  memset (resource_info, 0 , sizeof (qapi_Net_LWM2M_Resource_Info_t));
  lwm2m_data.instance_info->no_resources++;
  lwm2m_data.instance_info->resource_info->next = resource_info;
  resource_info->type  = QAPI_NET_LWM2M_TYPE_INTEGER_E;
  resource_info->resource_ID = QAPI_NET_LWM2M_SW_MGNT_RES_M_UPDATE_STATE_E ;
  resource_info->value.asInteger = 0;

  tx_byte_allocate(byte_pool_qcli, (VOID **)&(resource_info1), sizeof (qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);		  
  if (resource_info1 == NULL)
  {
    LOG_INFO ("LWM2M_LOG : MALLOC FAILURE \n");
    return -1;
  }
  memset (resource_info1, 0 , sizeof (qapi_Net_LWM2M_Resource_Info_t));

  lwm2m_data.instance_info->no_resources++;
  resource_info->next = resource_info1;   
  resource_info1->type  = QAPI_NET_LWM2M_TYPE_BOOLEAN_E;
  resource_info1->resource_ID = QAPI_NET_LWM2M_SW_MGNT_RES_M_ACTIVATION_STATE_E;
  resource_info1->value.asBoolean = 0;

  tx_byte_allocate(byte_pool_qcli, (VOID **)&(resource_info2), sizeof (qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);		  
  if (resource_info2 == NULL)
  {
    LOG_INFO ("LWM2M_LOG : MALLOC FAILURE \n");
    return -1;
  }
  memset (resource_info2, 0 , sizeof (qapi_Net_LWM2M_Resource_Info_t));

  lwm2m_data.instance_info->no_resources++;
  resource_info1->next = resource_info2;   
  resource_info2->type  = QAPI_NET_LWM2M_TYPE_STRING_E;
  resource_info2->resource_ID = QAPI_NET_LWM2M_SW_MGNT_RES_O_PACKAGE_NAME_E;
  resource_info2->value.asBuffer.length = strlen(PKG_NAME);
  tx_byte_allocate(byte_pool_qcli, (VOID **)&(resource_info2->value.asBuffer.buffer), strlen(PKG_NAME)+1, TX_NO_WAIT);		  
  strlcpy((char *)resource_info2->value.asBuffer.buffer,PKG_NAME,resource_info2->value.asBuffer.length+1);

  tx_byte_allocate(byte_pool_qcli, (VOID **)&(resource_info3), sizeof (qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);		  
  if (resource_info3 == NULL)
  {
    LOG_INFO ("LWM2M_LOG : MALLOC FAILURE \n");
    return -1;
  }
  memset (resource_info3, 0 , sizeof (qapi_Net_LWM2M_Resource_Info_t));

  lwm2m_data.instance_info->no_resources++;
  resource_info2->next = resource_info3;   
  resource_info3->type  = QAPI_NET_LWM2M_TYPE_STRING_E;
  resource_info3->resource_ID = QAPI_NET_LWM2M_SW_MGNT_RES_O_PACKAGE_VERSION_E;
  resource_info3->value.asBuffer.length = strlen(PKG_VERSION);
  tx_byte_allocate(byte_pool_qcli, (VOID **)&(resource_info3->value.asBuffer.buffer), strlen(PKG_VERSION)+1, TX_NO_WAIT);		    
  strlcpy((char *)resource_info3->value.asBuffer.buffer,PKG_VERSION,resource_info3->value.asBuffer.length+1);

  LOG_INFO ("LWM2M_LOG : INVOKING THE OBJECT CREATE INSTANCE  \n");          
  result = qapi_Net_LWM2M_Create_Object_Instance_v2 (gsoftware_mgnt_Handler_v2, &lwm2m_data); 
  if (result == QAPI_ERROR)
  {
    LOG_INFO ("LWM2M_LOG : create object instance got failed \n");
  }
  app_free_instance_info_v2(lwm2m_data.instance_info);

  return 0;
}

/*
 * @fn     int32_t sw_mgnt_handle_delete_instance_v2() 
 * @brief  This function is used to delete the software management object instance 
 * @param  instance_id  - instance id to be deleted
 * @return on success - 0
 on failure - -1
 */

int32_t sw_mgnt_handle_delete_instance_v2(uint16_t instance_id ) 
{
  qapi_Status_t  result = QAPI_OK;
  qapi_Net_LWM2M_Object_Info_v2_t object_info ;

  qapi_Net_LWM2M_Id_Info_v2_t * id_info = NULL;  

  if (gsoftware_mgnt_Handler_v2 == NULL  ) 
  {
    LOG_INFO( "LWM2M_LOG : create is not allowed as application is not started  \n");  
    return -1;
  }

  tx_byte_allocate(byte_pool_qcli, (VOID **)&(id_info), sizeof (qapi_Net_LWM2M_Id_Info_v2_t ), TX_NO_WAIT);		    
  if (id_info == NULL)
  {
    LOG_INFO ("LWM2M : [SOFTWARE MGNT APP] MALLOC FALIURE \n");
    return -1;
  }
  else
  {

    memset (&object_info , 0 , sizeof (qapi_Net_LWM2M_Object_Info_v2_t));
    object_info.no_object_info++;
    memset(id_info , 0 , sizeof (qapi_Net_LWM2M_Id_Info_v2_t));
    id_info->id_set = id_info->id_set | QAPI_NET_LWM2M_OBJECT_ID_E;
    id_info->id_set = id_info->id_set | QAPI_NET_LWM2M_INSTANCE_ID_E;
    id_info->object_ID = QAPI_NET_LWM2M_SOFTWARE_MGNT_OBJECT_ID_E;
    id_info->instance_ID = instance_id;


    id_info->next = NULL;
    object_info.id_info = id_info;
    object_info.id_info->next = NULL;

    result = qapi_Net_LWM2M_Delete_Object_Instance_v2 (gsoftware_mgnt_Handler_v2, &object_info);

    if (result == QAPI_ERROR)
    {
      LOG_INFO ("LWM2M_LOG : DELETE INSTANCE GOT FAILED \n");
      app_free_id_info_v2 (id_info);
      return -1;

    }
    else
    { 
      qapi_Net_LWM2M_Object_Info_v2_t observe_info;
      qapi_Net_LWM2M_Id_Info_v2_t * id_info1 = NULL;
      qapi_Net_LWM2M_Id_Info_v2_t * id_info2 = NULL; 
      qapi_Net_LWM2M_Id_Info_v2_t * id_info3 = NULL;
      qapi_Net_LWM2M_Id_Info_v2_t * id_info4 = NULL;
      qapi_Net_LWM2M_Id_Info_v2_t * id_info5 = NULL; 
      qapi_Net_LWM2M_Id_Info_v2_t * id_info6 = NULL;
      qapi_Net_LWM2M_Id_Info_v2_t * id_info7 = NULL;

      memset (&observe_info , 0 ,sizeof (qapi_Net_LWM2M_Object_Info_v2_t));
      observe_info.no_object_info++;
	  tx_byte_allocate(byte_pool_qcli, (VOID **)&(id_info1), sizeof (qapi_Net_LWM2M_Id_Info_v2_t ), TX_NO_WAIT);			
      if(NULL == id_info1)
      {
        return -1;        
      }
      memset(id_info1 , 0 , sizeof (qapi_Net_LWM2M_Id_Info_v2_t));
      id_info1->id_set = id_info1->id_set | QAPI_NET_LWM2M_OBJECT_ID_E;
      id_info1->id_set = id_info1->id_set | QAPI_NET_LWM2M_INSTANCE_ID_E;
      id_info1->id_set = id_info1->id_set | QAPI_NET_LWM2M_RESOURCE_ID_E;
      id_info1->object_ID = QAPI_NET_LWM2M_SOFTWARE_MGNT_OBJECT_ID_E;
      id_info1->instance_ID = instance_id;
      id_info1->resource_ID = QAPI_NET_LWM2M_SW_MGNT_RES_O_PACKAGE_E; // 2


      observe_info.id_info = id_info1;

      observe_info.no_object_info++;
	  tx_byte_allocate(byte_pool_qcli, (VOID **)&(id_info2), sizeof (qapi_Net_LWM2M_Id_Info_v2_t ), TX_NO_WAIT);			
      if(NULL == id_info2)
      {

        app_free_id_info_v2 (id_info1);
        return -1;        
      }
      memset(id_info2 , 0 , sizeof (qapi_Net_LWM2M_Id_Info_v2_t));
      id_info2->id_set = id_info2->id_set | QAPI_NET_LWM2M_OBJECT_ID_E;
      id_info2->id_set = id_info2->id_set | QAPI_NET_LWM2M_INSTANCE_ID_E;
      id_info2->id_set = id_info2->id_set | QAPI_NET_LWM2M_RESOURCE_ID_E;
      id_info2->object_ID = QAPI_NET_LWM2M_SOFTWARE_MGNT_OBJECT_ID_E;
      id_info2->instance_ID = instance_id;
      id_info2->resource_ID = QAPI_NET_LWM2M_SW_MGNT_RES_O_PACKAGE_URI_E; // 3

      id_info1->next = id_info2;

      observe_info.no_object_info++;
	  tx_byte_allocate(byte_pool_qcli, (VOID **)&(id_info3), sizeof (qapi_Net_LWM2M_Id_Info_v2_t ), TX_NO_WAIT);			
      if(NULL == id_info3)
      {
        app_free_id_info_v2 (id_info1);
        return -1;        
      }
      memset(id_info3 , 0 , sizeof (qapi_Net_LWM2M_Id_Info_v2_t));
      id_info3->id_set = id_info3->id_set | QAPI_NET_LWM2M_OBJECT_ID_E;
      id_info3->id_set = id_info3->id_set | QAPI_NET_LWM2M_INSTANCE_ID_E;
      id_info3->id_set = id_info3->id_set | QAPI_NET_LWM2M_RESOURCE_ID_E;
      id_info3->object_ID = QAPI_NET_LWM2M_SOFTWARE_MGNT_OBJECT_ID_E;
      id_info3->instance_ID = instance_id;
      id_info3->resource_ID = QAPI_NET_LWM2M_SW_MGNT_RES_M_INSTALL_E;   // 4
      id_info3->next = NULL;
      id_info2->next = id_info3;

      observe_info.no_object_info++;
	  tx_byte_allocate(byte_pool_qcli, (VOID **)&(id_info4), sizeof (qapi_Net_LWM2M_Id_Info_v2_t ), TX_NO_WAIT);			
      if(NULL == id_info4)
      {
        app_free_id_info_v2 (id_info1);
        return -1;        
      }

      memset(id_info4, 0 , sizeof (qapi_Net_LWM2M_Id_Info_v2_t));
      id_info4->id_set = id_info3->id_set | QAPI_NET_LWM2M_OBJECT_ID_E;
      id_info4->id_set = id_info3->id_set | QAPI_NET_LWM2M_INSTANCE_ID_E;
      id_info4->id_set = id_info3->id_set | QAPI_NET_LWM2M_RESOURCE_ID_E;
      id_info4->object_ID = QAPI_NET_LWM2M_SOFTWARE_MGNT_OBJECT_ID_E;
      id_info4->instance_ID = instance_id;
      id_info4->resource_ID = QAPI_NET_LWM2M_SW_MGNT_RES_M_UNINSTALL_E;  // 6
      id_info4->next = NULL;
      id_info3 -> next = id_info4;

      observe_info.no_object_info++;
	  tx_byte_allocate(byte_pool_qcli, (VOID **)&(id_info5), sizeof (qapi_Net_LWM2M_Id_Info_v2_t ), TX_NO_WAIT);			
      if(NULL == id_info5)
      {
        app_free_id_info_v2 (id_info1);  
        return -1;        
      }

      memset(id_info5 , 0 , sizeof (qapi_Net_LWM2M_Id_Info_v2_t));
      id_info5->id_set = id_info3->id_set | QAPI_NET_LWM2M_OBJECT_ID_E;
      id_info5->id_set = id_info3->id_set | QAPI_NET_LWM2M_INSTANCE_ID_E;
      id_info5->id_set = id_info3->id_set | QAPI_NET_LWM2M_RESOURCE_ID_E;
      id_info5->object_ID = QAPI_NET_LWM2M_SOFTWARE_MGNT_OBJECT_ID_E;
      id_info5->instance_ID = instance_id;
      id_info5->resource_ID = QAPI_NET_LWM2M_SW_MGNT_RES_M_ACTIVATE_E;   // 10
      id_info5->next = NULL;
      id_info4->next = id_info5;

      observe_info.no_object_info++;
      tx_byte_allocate(byte_pool_qcli, (VOID **)&(id_info6), sizeof (qapi_Net_LWM2M_Id_Info_v2_t ), TX_NO_WAIT);			
      if(NULL == id_info6)
      {
        app_free_id_info_v2 (id_info1);
        return -1;        
      }

      memset(id_info6 , 0 , sizeof (qapi_Net_LWM2M_Id_Info_v2_t));
      id_info6->id_set = id_info3->id_set | QAPI_NET_LWM2M_OBJECT_ID_E;
      id_info6->id_set = id_info3->id_set | QAPI_NET_LWM2M_INSTANCE_ID_E;
      id_info6->id_set = id_info3->id_set | QAPI_NET_LWM2M_RESOURCE_ID_E;
      id_info6->object_ID = QAPI_NET_LWM2M_SOFTWARE_MGNT_OBJECT_ID_E;
      id_info6->instance_ID = instance_id;
      id_info6->resource_ID = QAPI_NET_LWM2M_SW_MGNT_RES_M_DEACTIVATE_E;   // 11
      id_info6->next = NULL;
      id_info5->next = id_info6;

      observe_info.no_object_info++;
      tx_byte_allocate(byte_pool_qcli, (VOID **)&(id_info7), sizeof (qapi_Net_LWM2M_Id_Info_v2_t ), TX_NO_WAIT);
      if(NULL == id_info7)
      {
        app_free_id_info_v2 (id_info1);
        return -1;        
      }

      memset(id_info7 , 0 , sizeof (qapi_Net_LWM2M_Id_Info_v2_t));
      id_info7->id_set = id_info7->id_set | QAPI_NET_LWM2M_OBJECT_ID_E;
      id_info7->id_set = id_info7->id_set | QAPI_NET_LWM2M_INSTANCE_ID_E;
      id_info7->id_set = id_info7->id_set | QAPI_NET_LWM2M_RESOURCE_ID_E;
      id_info7->object_ID = QAPI_NET_LWM2M_SOFTWARE_MGNT_OBJECT_ID_E;
      id_info7->instance_ID = QAPI_Net_LWM2M_SOFTWARE_MGT_INSTANCE_ID_0;
      id_info7->resource_ID = QAPI_NET_LWM2M_SW_MGNT_RES_M_UPDATE_STATE_E;     //7
      id_info7->next = NULL;
      id_info6->next = id_info7;

      result = qapi_Net_LWM2M_Cancel_Observe_v2 (gsoftware_mgnt_Handler_v2, &observe_info);
      if (result == QAPI_ERROR)
      {
        LOG_INFO ("LWM2M_LOG : cancel observe failed \n");  
      }
      app_free_id_info_v2 (id_info1);
    }
  }
  return 0;
}

int software_mngt_get_v2(qapi_Net_LWM2M_Id_Info_v2_t *id_info)
{
  qapi_Net_LWM2M_Data_v2_t *lwm2m_dataP = NULL;
  qapi_Status_t result = QAPI_ERROR;
  qapi_Net_LWM2M_Object_Info_v2_t lwm2m_info_req;

  if(id_info == NULL)
  {
    LOG_INFO("Null parameter passed. ");
    return -1;
  }

  memset (&lwm2m_info_req, 0x00, sizeof (qapi_Net_LWM2M_Object_Info_v2_t));
  lwm2m_info_req.no_object_info++;
  lwm2m_info_req.id_info = id_info; 

  result = qapi_Net_LWM2M_Get_v2(gsoftware_mgnt_Handler_v2, &lwm2m_info_req, &lwm2m_dataP);
  if (result == QAPI_ERROR)
  {
    LOG_INFO ("LWM2M_LOG : Get failed for software management!\n");
  }
  else 
  {
    app_display_obj_info_v2(lwm2m_dataP); 
  }

  app_free_data_v2(lwm2m_dataP);
  return result;
}

/**
 * @fn software_mngt_app_deregister_v2()
 * @brief This fucntion is used to deregister software management application.
 * @param void
 * @return void
 */
void software_mngt_app_deregister_v2()
{
  qapi_Status_t result = QAPI_OK; 
  if(gsoftware_mgnt_Handler_v2)
  {      
    result = qapi_Net_LWM2M_DeRegister_App(gsoftware_mgnt_Handler_v2);
    if (result == QAPI_ERROR)
    {
      LOG_INFO ("LWM2M_LOG : sw mgnt handler is not valid \n");
    }
    gsoftware_mgnt_Handler_v2 = NULL;    
  }
  else
  {
    LOG_INFO("Handler is not valid\n");  
  }      
  return;
}
