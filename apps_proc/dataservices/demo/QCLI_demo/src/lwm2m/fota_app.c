/**************************************************************************************************************
 * Copyright © 2017 Qualcomm Technologies, Inc.
 * @file  fota_app.c
 * @author  Qualcomm Technologies, Inc.
 * @brief File contains the implementation for Firmware object application
 *
 **************************************************************************************************************/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "fs_public.h"
#include "log_intf.h"
#include "qapi_lwm2m.h"
#include "fota_stub_app.h"
#include "utils_app.h"

#define FIRMWARE_NAME    "IOT.TX.2.0"
#define FIRMWARE_VERSION "67"
#define FOTA_APP_FILE_PATH "/datatx/fota_qapi_file"

#define FOTA_INSTANCE_ID_0                   0

// EFS File FD to store package
static int fota_file_fd  = -1;
bool push_allowed = false;

bool pull_allowed = false;
// variable to check the download state
bool gdownloaded;
// variable to check the update state
bool gupdate;
// FoTA handler
qapi_Net_LWM2M_App_Handler_t gHandler_fota = NULL;

void fota_app_deregister(void);
/**
 * @fn app_callback()
 *
 * @brief This callback fucntion is called when observe operation is performed 
 *        on firmware object resource.
 * @param handler LWM2M application handler
 * @param lwm2m_data  Firmware object data
 * @return on success - QAPI_OK
 on failure - QAPI_ERROR
 */
qapi_Status_t app_callback( qapi_Net_LWM2M_App_Handler_t handler, qapi_Net_LWM2M_Data_t * lwm2m_data)
{
  int32_t rc = 0;
  qapi_Status_t result = QAPI_OK;
  qapi_Net_LWM2M_Data_t *lwm2m_dataP = NULL;
  qapi_Net_LWM2M_Object_Info_t lwm2m_info_req = {0};
  if (handler == NULL || handler != gHandler_fota || lwm2m_data == NULL || \
      lwm2m_data->instance_info == NULL || \
      lwm2m_data->instance_info->resource_info == NULL)
  {
    LOG_INFO ("LWM2M_LOG : passed arguments are NULL\n");
    return QAPI_ERROR;
  }

  LOG_DEBUG("App callback is invoked DATA is --> :\n");   
  LOG_DEBUG("LWM2M_LOG: object_ID ----> %d\n",lwm2m_data->object_ID );
  LOG_DEBUG("LWM2M_LOG: instance id ----> %d \n",
      lwm2m_data->instance_info->instance_ID);
  LOG_DEBUG("LWM2M_LOG: resouces id -----> %d \n",
      lwm2m_data->instance_info->resource_info->resource_ID);
  switch (lwm2m_data->instance_info->resource_info->type) {
    case QAPI_NET_LWM2M_TYPE_STRING_E :
    case QAPI_NET_LWM2M_TYPE_OPAQUE_E :
	  LOG_DEBUG ("LWM2M_LOG: resource value is length %d\t ",
	    lwm2m_data->instance_info->resource_info->value.asBuffer.length );
      if (lwm2m_data->instance_info->resource_info->value.asBuffer.length)
        LOG_DEBUG("LWM2M_LOG: resource value is %s\n", 
            lwm2m_data->instance_info->resource_info->value.asBuffer.buffer ); 
      break;
    case QAPI_NET_LWM2M_TYPE_INTEGER_E :
      LOG_DEBUG("LWM2M_LOG: resource value is %ld\n",
          lwm2m_data->instance_info->resource_info->value.asInteger );
      break;
    case QAPI_NET_LWM2M_TYPE_FLOAT_E :
      LOG_DEBUG("LWM2M_LOG: resource value is %f\n",
          lwm2m_data->instance_info->resource_info->value.asFloat );
      break;
    case QAPI_NET_LWM2M_TYPE_BOOLEAN_E :
      LOG_DEBUG("LWM2M_LOG: resource value is %d\n",
          lwm2m_data->instance_info->resource_info->value.asBoolean );
      break;
  }

  //check if the resource is Package that the application has done a observation on
  if (lwm2m_data->instance_info->resource_info->resource_ID == 
      QAPI_NET_LWM2M_FOTA_RES_M_PACKAGE_E)
  {
    if(push_allowed != true)
      return QAPI_ERROR;
    if(!gdownloaded)  { 
      gdownloaded = true;
    }

    // In case of Package write the same into EFS file
    if (fota_file_fd == -1) 
    {
      fota_file_fd = efs_open(FOTA_APP_FILE_PATH, (O_CREAT | O_RDWR));
    }

    if(fota_file_fd < 0) {
      LOG_INFO (" FILE OPENED FAILED \n");
      fota_file_fd = -1;
      return QAPI_ERROR;
    }   

    if (lwm2m_data->instance_info->resource_info->value.asBuffer.length) 
    {
      rc = efs_write(fota_file_fd, 
          lwm2m_data->instance_info->resource_info->value.asBuffer.buffer, 
          lwm2m_data->instance_info->resource_info->value.asBuffer.length);

      if(rc < lwm2m_data->instance_info->resource_info->value.asBuffer.length)
      {
        qapi_Net_LWM2M_Data_t lwm2m_data ; 
        LOG_INFO("FOTA : Download file write failed\n"); 
        memset (&lwm2m_data , 0 , sizeof (qapi_Net_LWM2M_Data_t));  
        lwm2m_data.object_ID = QAPI_NET_LWM2M_FIRMWARE_UPDATE_OBJECT_ID_E;
        lwm2m_data.no_instances++;
        lwm2m_data.instance_info = 
          (qapi_Net_LWM2M_Instance_Info_t *)malloc (sizeof (qapi_Net_LWM2M_Instance_Info_t));
        if( NULL == lwm2m_data.instance_info)
        { 
          return QAPI_ERROR;
        }  
        memset (lwm2m_data.instance_info , 0 , sizeof (qapi_Net_LWM2M_Instance_Info_t));
        lwm2m_data.instance_info->instance_ID = FOTA_INSTANCE_ID_0;
        lwm2m_data.instance_info->no_resources++;
        lwm2m_data.instance_info->resource_info = 
          (qapi_Net_LWM2M_Resource_Info_t *) malloc (sizeof (qapi_Net_LWM2M_Resource_Info_t));
        if( NULL == lwm2m_data.instance_info->resource_info)
        {
          app_free_instance_info(lwm2m_data.instance_info);
          return QAPI_ERROR;
        }
        memset (lwm2m_data.instance_info->resource_info , 0 , sizeof (qapi_Net_LWM2M_Resource_Info_t));
        lwm2m_data.instance_info->resource_info->resource_ID = QAPI_NET_LWM2M_FOTA_RES_M_STATE_E;
        lwm2m_data.instance_info->resource_info->type = QAPI_NET_LWM2M_TYPE_INTEGER_E;
        lwm2m_data.instance_info->resource_info->value.asInteger = QAPI_NET_LWM2M_FIRWARE_STATE_IDLE_E; 
        result = qapi_Net_LWM2M_Set ( handler,&lwm2m_data);
        if (result == QAPI_ERROR)
        {
          LOG_INFO ("LWM2M_LOG : qapi set got failed \n");
        }
        app_free_instance_info(lwm2m_data.instance_info);

        memset (&lwm2m_data , 0 , sizeof (qapi_Net_LWM2M_Data_t)); 

        lwm2m_data.object_ID = QAPI_NET_LWM2M_FIRMWARE_UPDATE_OBJECT_ID_E;
        lwm2m_data.no_instances++;
        lwm2m_data.instance_info = 
          (qapi_Net_LWM2M_Instance_Info_t *)malloc (sizeof (qapi_Net_LWM2M_Instance_Info_t));
        if( NULL == lwm2m_data.instance_info) return QAPI_ERROR;
        memset (lwm2m_data.instance_info , 0 , sizeof (qapi_Net_LWM2M_Instance_Info_t));
        lwm2m_data.instance_info->instance_ID = FOTA_INSTANCE_ID_0;
        lwm2m_data.instance_info->no_resources++;
        lwm2m_data.instance_info->resource_info = 
          (qapi_Net_LWM2M_Resource_Info_t *) malloc (sizeof (qapi_Net_LWM2M_Resource_Info_t));
        if( NULL == lwm2m_data.instance_info->resource_info)
        {
          app_free_instance_info(lwm2m_data.instance_info);
          return QAPI_ERROR;
        }
        memset (lwm2m_data.instance_info->resource_info , 0 , 
            sizeof (qapi_Net_LWM2M_Resource_Info_t));
        lwm2m_data.instance_info->resource_info->resource_ID = 
          QAPI_NET_LWM2M_FOTA_RES_M_UPDATE_RESULT_E;
        lwm2m_data.instance_info->resource_info->type = 
          QAPI_NET_LWM2M_TYPE_INTEGER_E;
        lwm2m_data.instance_info->resource_info->value.asInteger = 3; 
        result = qapi_Net_LWM2M_Set ( handler,&lwm2m_data);
        if (result == QAPI_ERROR)
        {
          LOG_INFO ("LWM2M_LOG : qapi set got failed \n");
        }
        app_free_instance_info(lwm2m_data.instance_info);
        return QAPI_ERROR;
      }
    }  
  }
  //check if the resource is Package URI that the application has done a observation on
  else if (lwm2m_data->instance_info->resource_info->resource_ID == QAPI_NET_LWM2M_FOTA_RES_M_PACKAGE_URI_E)
  {
    if(pull_allowed != true)
      return QAPI_ERROR;
    if (gdownloaded == false)
    {

      qapi_Net_LWM2M_Data_t lwm2m_data ;  
      gdownloaded = true; 
      LOG_INFO ("LWM2M_LOG : package uri is received \n");
      LOG_INFO ("LWM2M_LOG : ENTER the update result \n"); 
      memset (&lwm2m_data , 0 , sizeof (qapi_Net_LWM2M_Data_t));  
      lwm2m_data.object_ID = QAPI_NET_LWM2M_FIRMWARE_UPDATE_OBJECT_ID_E;
      lwm2m_data.no_instances++;
      lwm2m_data.instance_info = (qapi_Net_LWM2M_Instance_Info_t *)malloc (sizeof (qapi_Net_LWM2M_Instance_Info_t));
      if( NULL == lwm2m_data.instance_info) return QAPI_ERROR;
      memset (lwm2m_data.instance_info , 0 , sizeof (qapi_Net_LWM2M_Instance_Info_t));
      lwm2m_data.instance_info->instance_ID = FOTA_INSTANCE_ID_0;
      lwm2m_data.instance_info->no_resources++;
      lwm2m_data.instance_info->resource_info = (qapi_Net_LWM2M_Resource_Info_t *) malloc (sizeof (qapi_Net_LWM2M_Resource_Info_t));
      if( NULL == lwm2m_data.instance_info->resource_info)
      {
        app_free_instance_info(lwm2m_data.instance_info);
        return QAPI_ERROR;
      }
      memset (lwm2m_data.instance_info->resource_info , 0 , sizeof (qapi_Net_LWM2M_Resource_Info_t));
      lwm2m_data.instance_info->resource_info->resource_ID = QAPI_NET_LWM2M_FOTA_RES_M_STATE_E;
      lwm2m_data.instance_info->resource_info->type = QAPI_NET_LWM2M_TYPE_INTEGER_E;
      lwm2m_data.instance_info->resource_info->value.asInteger = QAPI_NET_LWM2M_FIRWARE_STATE_DOWNLOADING_E; 
      result = qapi_Net_LWM2M_Set ( handler,&lwm2m_data);
      if (result == QAPI_ERROR)
      {
        LOG_INFO ("LWM2M_LOG : qapi set got failed \n");
      }
      app_free_instance_info(lwm2m_data.instance_info);
    } 
    else
    {
      LOG_INFO("LWM2M_LOG : package uri is \n");

    }

  } 
  //check if the resource is Update execute that the application has done a observation on  
  else if ( lwm2m_data->instance_info->resource_info->resource_ID == QAPI_NET_LWM2M_FOTA_RES_M_UPDATE_E)
  {

    qapi_Net_LWM2M_Data_t lwm2m_data ;
	  if ((gupdate == false) && (gdownloaded == true ))
	  {
	    gupdate = true;
		
		LOG_INFO ("LWM2M_INFO : enter the updated result \n");	
	  } 
	  else
	  {
	    LOG_INFO ("LWM2M_LOG : exec on /5/0/2 \n");
	  }
      memset (&lwm2m_data , 0 , sizeof (qapi_Net_LWM2M_Data_t));  
      lwm2m_data.object_ID = QAPI_NET_LWM2M_FIRMWARE_UPDATE_OBJECT_ID_E;
      lwm2m_data.no_instances++;
      lwm2m_data.instance_info = (qapi_Net_LWM2M_Instance_Info_t *)malloc (sizeof (qapi_Net_LWM2M_Instance_Info_t));
      if( NULL == lwm2m_data.instance_info) return QAPI_ERROR;
      
      memset (lwm2m_data.instance_info, 0, sizeof (qapi_Net_LWM2M_Instance_Info_t));
      lwm2m_data.instance_info->instance_ID = FOTA_INSTANCE_ID_0;
      lwm2m_data.instance_info->no_resources++;
      lwm2m_data.instance_info->resource_info = (qapi_Net_LWM2M_Resource_Info_t *) malloc (sizeof (qapi_Net_LWM2M_Resource_Info_t));
      
      if( NULL == lwm2m_data.instance_info->resource_info)
      { 
        app_free_instance_info(lwm2m_data.instance_info);
        return QAPI_ERROR;
      }
      memset (lwm2m_data.instance_info->resource_info ,0,sizeof (qapi_Net_LWM2M_Resource_Info_t));
      lwm2m_data.instance_info->resource_info->resource_ID     = QAPI_NET_LWM2M_FOTA_RES_M_STATE_E;
      lwm2m_data.instance_info->resource_info->type            = QAPI_NET_LWM2M_TYPE_INTEGER_E;
      lwm2m_data.instance_info->resource_info->value.asInteger = QAPI_NET_LWM2M_FIRWARE_STATE_UPDATING_E; 
      result = qapi_Net_LWM2M_Set ( handler,&lwm2m_data);
	  if (result == QAPI_ERROR)
	  {
	    LOG_INFO ("LWM2M_LOG : qapi set got failed \n");
	  }
	  app_free_instance_info(lwm2m_data.instance_info);
	}
  memset (&lwm2m_info_req , 0 , sizeof (qapi_Net_LWM2M_Object_Info_t));
  lwm2m_info_req.no_object_info++;
  lwm2m_info_req.id_info = (qapi_Net_LWM2M_Id_Info_t *)malloc (sizeof (qapi_Net_LWM2M_Id_Info_t));
  
  if(lwm2m_info_req.id_info == NULL)
    return QAPI_ERROR;

  memset(lwm2m_info_req.id_info, 0, sizeof (qapi_Net_LWM2M_Id_Info_t));
  lwm2m_info_req.id_info->id_set = lwm2m_info_req.id_info->id_set  | QAPI_NET_LWM2M_OBJECT_ID_E;
  lwm2m_info_req.id_info->id_set = lwm2m_info_req.id_info->id_set  | QAPI_NET_LWM2M_INSTANCE_ID_E;
  lwm2m_info_req.id_info->object_ID = QAPI_NET_LWM2M_FIRMWARE_UPDATE_OBJECT_ID_E;
  lwm2m_info_req.id_info->instance_ID = lwm2m_data->instance_info->instance_ID;
  lwm2m_info_req.id_info->next =NULL;
  result = qapi_Net_LWM2M_Get (gHandler_fota, &lwm2m_info_req, &lwm2m_dataP);
  if(result == QAPI_OK)
  { 
   
    if ( lwm2m_dataP != NULL ) {
        if (lwm2m_dataP->instance_info != NULL ) {
          while( lwm2m_dataP->instance_info->resource_info != NULL ) {
            if(lwm2m_dataP->instance_info->resource_info->resource_ID == QAPI_NET_LWM2M_FOTA_RES_M_STATE_E )
            {
              LOG_INFO("LWM2M_LOG:FOTA_STATE->%d\n",lwm2m_dataP->instance_info->resource_info->value.asInteger);
            }
            else if(lwm2m_dataP->instance_info->resource_info->resource_ID ==  QAPI_NET_LWM2M_FOTA_RES_M_UPDATE_RESULT_E) 
            {
              LOG_INFO("LWM2M_LOG:FOTA_UPDATE_RESULT->%d\n",lwm2m_dataP->instance_info->resource_info->value.asInteger);
            }
            else if(lwm2m_dataP->instance_info->resource_info->resource_ID ==  QAPI_NET_LWM2M_FOTA_RES_O_PACKAGE_NAME_E ) {
              if (lwm2m_dataP->instance_info->resource_info->value.asBuffer.buffer !=NULL ) {
                LOG_INFO("LWM2M_LOG:FOTA_PKG_NAME->%s\n",lwm2m_dataP->instance_info->resource_info->value.asBuffer.buffer);
              }  
              else 
                LOG_INFO("LWM2M_LOG:failed to get FOTA Package Name\n");
            }  
            else if(lwm2m_dataP->instance_info->resource_info->resource_ID ==  QAPI_NET_LWM2M_FOTA_RES_O_PACKAGE_VERSION_E) {
              if (lwm2m_dataP->instance_info->resource_info->value.asBuffer.buffer !=NULL ) 
              {
                LOG_INFO("LWM2M_LOG:FOTA_PKG_VERSION->%s\n",lwm2m_dataP->instance_info->resource_info->value.asBuffer.buffer);
              } else 
                 LOG_INFO("LWM2M_LOG:failed to get FOTA Package Name\n");
            }   
            lwm2m_dataP->instance_info->resource_info=lwm2m_dataP->instance_info->resource_info->next;
          }  
        }
    }    
  }
  else {
    return QAPI_ERROR;
  }
  
  return QAPI_OK;
}

/**
 * @fn fota_init()
 *
 * @brief This fucntion is used to initialise FOTA application.
 *
 * @param void
 * @return void
 */
void fota_init()
{

  qapi_Net_LWM2M_Object_Info_t observe_info;
  qapi_Net_LWM2M_Id_Info_t * id_info1 = NULL;
  qapi_Net_LWM2M_Id_Info_t * id_info2 = NULL; 
  qapi_Net_LWM2M_Id_Info_t * id_info3 = NULL;
    qapi_Net_LWM2M_Data_t *lwm2m_dataP = NULL;
  qapi_Net_LWM2M_Object_Info_t lwm2m_info_req = {0};
  qapi_Status_t result = QAPI_OK;
  qapi_Net_LWM2M_Data_t lwm2m_data = {0};

  if (gHandler_fota != NULL)
  {
    LOG_INFO ("FOTA : FOTA APP ALREADY STARTED \n");
    return ;
  }
  if(QAPI_OK ==  qapi_Net_LWM2M_Register_App(&gHandler_fota))
  {
    LOG_INFO ("FOTA APP STARTED \n");

    memset (&lwm2m_data , 0 ,sizeof (qapi_Net_LWM2M_Data_t));
    lwm2m_data.object_ID = QAPI_NET_LWM2M_FIRMWARE_UPDATE_OBJECT_ID_E;
    lwm2m_data.no_instances = 1;
    lwm2m_data.instance_info = calloc(sizeof(qapi_Net_LWM2M_Instance_Info_t), 1);

    qapi_Net_LWM2M_Create_Object_Instance(gHandler_fota,&lwm2m_data);

    if(lwm2m_data.instance_info != NULL) {

      //read the state resource value and update local gdownloaded/gupdate value
	  memset (&lwm2m_info_req , 0 , sizeof (qapi_Net_LWM2M_Object_Info_t));
	  lwm2m_info_req.no_object_info++;
	  lwm2m_info_req.id_info = (qapi_Net_LWM2M_Id_Info_t *)malloc (sizeof (qapi_Net_LWM2M_Id_Info_t));
	  
	  if(lwm2m_info_req.id_info != NULL) 
	  {
		//return QAPI_ERROR;
		memset(lwm2m_info_req.id_info, 0, sizeof (qapi_Net_LWM2M_Id_Info_t));
		lwm2m_info_req.id_info->id_set = lwm2m_info_req.id_info->id_set  | QAPI_NET_LWM2M_OBJECT_ID_E;
		lwm2m_info_req.id_info->id_set = lwm2m_info_req.id_info->id_set  | QAPI_NET_LWM2M_INSTANCE_ID_E;
		lwm2m_info_req.id_info->id_set = lwm2m_info_req.id_info->id_set  | QAPI_NET_LWM2M_RESOURCE_ID_E;
		lwm2m_info_req.id_info->object_ID = QAPI_NET_LWM2M_FIRMWARE_UPDATE_OBJECT_ID_E;
		lwm2m_info_req.id_info->instance_ID = FOTA_INSTANCE_ID_0;
		lwm2m_info_req.id_info->resource_ID = QAPI_NET_LWM2M_FOTA_RES_M_STATE_E;
		lwm2m_info_req.id_info->next =NULL;
		result = qapi_Net_LWM2M_Get (gHandler_fota, &lwm2m_info_req, &lwm2m_dataP);
	  }
	  else
	  {
		 LOG_INFO("lwm2m_info_req.id_info Malloc failed while invoking qapi lwm2m get\n");
	  }
	  
	  if(result == QAPI_OK)
	  { 
		  if(lwm2m_dataP != NULL && 
		  	 lwm2m_dataP->instance_info != NULL && 
		  	 lwm2m_dataP->instance_info->resource_info != NULL && 
		  	 lwm2m_dataP->instance_info->resource_info->resource_ID == QAPI_NET_LWM2M_FOTA_RES_M_STATE_E )
		  {
		    int resource_state_value = lwm2m_dataP->instance_info->resource_info->value.asInteger;
			LOG_INFO("LWM2M_LOG:FOTA_STATE->%d\n", resource_state_value);
			switch(resource_state_value)
			{
			  case QAPI_NET_LWM2M_FIRWARE_STATE_IDLE_E:			  	
			  	gdownloaded = false;
				gupdate     = false;
			  	break;
			  case QAPI_NET_LWM2M_FIRWARE_STATE_DOWNLOADING_E:
			  case QAPI_NET_LWM2M_FIRWARE_STATE_DOWNLOADED_E:
			  	gdownloaded = true;
				gupdate     = false;
			  	break;
			  case QAPI_NET_LWM2M_FIRWARE_STATE_UPDATING_E:
				gdownloaded = true;
				gupdate	    = true;
			  	break;
			  default:
			  	LOG_INFO("LWM2M_LOG:unexpected FOTA_STATE\n");
			    break;
			}
				
		  }
		  LOG_INFO("LWM2M_LOG:FOTA_STATE->gdownloaded[%d],gupdate[%d]\n", gdownloaded, gupdate);
		  free(lwm2m_dataP);
		  lwm2m_dataP = NULL;

	  }
	  else 
	  {
		LOG_INFO("Lwm2m_dataP NULL\n");
	  }
	  
      lwm2m_data.instance_info->no_resources  = 1;
      lwm2m_data.instance_info->instance_ID   = 0;
      lwm2m_data.instance_info->resource_info = calloc(sizeof(qapi_Net_LWM2M_Resource_Info_t), 1);

      if(lwm2m_data.instance_info->resource_info != NULL) {
        lwm2m_data.instance_info->resource_info->resource_ID = QAPI_NET_LWM2M_FOTA_RES_O_PACKAGE_NAME_E;
        lwm2m_data.instance_info->resource_info->type        = QAPI_NET_LWM2M_TYPE_STRING_E;
        lwm2m_data.instance_info->resource_info->value.asBuffer.buffer = FIRMWARE_NAME;
        lwm2m_data.instance_info->resource_info->value.asBuffer.length = strlen(FIRMWARE_NAME)+1;

        result = qapi_Net_LWM2M_Set (gHandler_fota, &lwm2m_data);
        if (result == QAPI_ERROR)
        {
          LOG_INFO ("FOTA : qapi_Net_LWM2M_Set failed \n");

        }
        lwm2m_data.instance_info->resource_info->resource_ID = QAPI_NET_LWM2M_FOTA_RES_O_PACKAGE_VERSION_E;
        lwm2m_data.instance_info->resource_info->type        = QAPI_NET_LWM2M_TYPE_STRING_E;
        lwm2m_data.instance_info->resource_info->value.asBuffer.buffer = FIRMWARE_VERSION;
        lwm2m_data.instance_info->resource_info->value.asBuffer.length = strlen(FIRMWARE_VERSION)+1;
        result = qapi_Net_LWM2M_Set (gHandler_fota, &lwm2m_data);
        if (result == QAPI_ERROR)
        {
          LOG_INFO ("FOTA : qapi_Net_LWM2M_Set failed \n");
        }
        free(lwm2m_data.instance_info->resource_info);
      }
      free(lwm2m_data.instance_info);
    }

    memset (&observe_info , 0, sizeof (qapi_Net_LWM2M_Object_Info_t));
    observe_info.no_object_info++;
    id_info1 = (qapi_Net_LWM2M_Id_Info_t *) malloc(sizeof(qapi_Net_LWM2M_Id_Info_t));
    if(NULL == id_info1)
    {
      return;        
    }
    memset(id_info1 , 0 , sizeof (qapi_Net_LWM2M_Id_Info_t));
    id_info1->id_set = id_info1->id_set | QAPI_NET_LWM2M_OBJECT_ID_E;
    id_info1->id_set = id_info1->id_set | QAPI_NET_LWM2M_INSTANCE_ID_E;
    id_info1->id_set = id_info1->id_set | QAPI_NET_LWM2M_RESOURCE_ID_E;
    id_info1->object_ID = QAPI_NET_LWM2M_FIRMWARE_UPDATE_OBJECT_ID_E;
    id_info1->instance_ID = FOTA_INSTANCE_ID_0;
    id_info1->resource_ID = QAPI_NET_LWM2M_FOTA_RES_M_PACKAGE_E;

    observe_info.id_info = id_info1;

    observe_info.no_object_info++;
    id_info2 = (qapi_Net_LWM2M_Id_Info_t *) malloc(sizeof(qapi_Net_LWM2M_Id_Info_t));
    if(NULL == id_info2)
    {
      app_free_id_info(id_info1);
      return;        
    }
    memset(id_info2 , 0 , sizeof (qapi_Net_LWM2M_Id_Info_t));
    id_info2->id_set = id_info2->id_set | QAPI_NET_LWM2M_OBJECT_ID_E;
    id_info2->id_set = id_info2->id_set | QAPI_NET_LWM2M_INSTANCE_ID_E;
    id_info2->id_set = id_info2->id_set | QAPI_NET_LWM2M_RESOURCE_ID_E;
    id_info2->object_ID = QAPI_NET_LWM2M_FIRMWARE_UPDATE_OBJECT_ID_E;
    id_info2->instance_ID = FOTA_INSTANCE_ID_0;
    id_info2->resource_ID = QAPI_NET_LWM2M_FOTA_RES_M_PACKAGE_URI_E;

    observe_info.no_object_info++;
    observe_info.id_info->next = id_info2;

    id_info3 = (qapi_Net_LWM2M_Id_Info_t *) malloc(sizeof(qapi_Net_LWM2M_Id_Info_t));
    if(NULL == id_info3)
    {
      app_free_id_info(id_info1);
      return;        
    }

    memset(id_info3 , 0 , sizeof (qapi_Net_LWM2M_Id_Info_t));
    id_info3->id_set = id_info3->id_set | QAPI_NET_LWM2M_OBJECT_ID_E;
    id_info3->id_set = id_info3->id_set | QAPI_NET_LWM2M_INSTANCE_ID_E;
    id_info3->id_set = id_info3->id_set | QAPI_NET_LWM2M_RESOURCE_ID_E;
    id_info3->object_ID = QAPI_NET_LWM2M_FIRMWARE_UPDATE_OBJECT_ID_E;
    id_info3->instance_ID = FOTA_INSTANCE_ID_0;
    id_info3->resource_ID = QAPI_NET_LWM2M_FOTA_RES_M_UPDATE_E;
    id_info3->next = NULL;
    observe_info.id_info->next->next = id_info3;

    result = qapi_Net_LWM2M_Observe(gHandler_fota, app_callback, &observe_info);
    if (result == QAPI_ERROR) 
    {
      LOG_INFO ("LWM2M_LOG : set observe failed \n");
    }
    app_free_id_info(id_info1);

    //qapi_Net_LWM2M_DeRegister_App(handler);
    //handler = NULL;    
  }      
  memset (&lwm2m_info_req , 0 , sizeof (qapi_Net_LWM2M_Object_Info_t));
  lwm2m_info_req.no_object_info++;
  lwm2m_info_req.id_info = (qapi_Net_LWM2M_Id_Info_t *)malloc (sizeof (qapi_Net_LWM2M_Id_Info_t));
  
  if(lwm2m_info_req.id_info != NULL) 
  {
    //return QAPI_ERROR;
    memset(lwm2m_info_req.id_info, 0, sizeof (qapi_Net_LWM2M_Id_Info_t));
    lwm2m_info_req.id_info->id_set = lwm2m_info_req.id_info->id_set  | QAPI_NET_LWM2M_OBJECT_ID_E;
    lwm2m_info_req.id_info->id_set = lwm2m_info_req.id_info->id_set  | QAPI_NET_LWM2M_INSTANCE_ID_E;
    lwm2m_info_req.id_info->object_ID = QAPI_NET_LWM2M_FIRMWARE_UPDATE_OBJECT_ID_E;
    lwm2m_info_req.id_info->instance_ID = FOTA_INSTANCE_ID_0;
    lwm2m_info_req.id_info->next =NULL;
    result = qapi_Net_LWM2M_Get (gHandler_fota, &lwm2m_info_req, &lwm2m_dataP);
  }
  else
  {
     LOG_INFO("lwm2m_info_req.id_info Malloc failed while invoking qapi lwm2m get\n");
  }
  if(result == QAPI_OK)
  { 
    dump_firmware_get_info(lwm2m_dataP);
  }
  else 
  {
    LOG_INFO("Lwm2m_dataP NULL\n");
    //return QAPI_ERROR;
  }
}

void dump_firmware_get_info(qapi_Net_LWM2M_Data_t *lwm2m_dataP)
{
  
    if ( lwm2m_dataP != NULL ) 
    {
      if (lwm2m_dataP->instance_info != NULL ) 
      {
        while( lwm2m_dataP->instance_info->resource_info != NULL ) 
        {
          if(lwm2m_dataP->instance_info->resource_info->resource_ID == QAPI_NET_LWM2M_FOTA_RES_M_STATE_E )
          {
            LOG_INFO("LWM2M_LOG:FOTA_STATE->%d\n",lwm2m_dataP->instance_info->resource_info->value.asInteger);
          }
          else if(lwm2m_dataP->instance_info->resource_info->resource_ID ==  QAPI_NET_LWM2M_FOTA_RES_M_UPDATE_RESULT_E) 
          {
            LOG_INFO("LWM2M_LOG:FOTA_UPDATE_RESULT->%d\n",lwm2m_dataP->instance_info->resource_info->value.asInteger);
          }
          else if(lwm2m_dataP->instance_info->resource_info->resource_ID ==  QAPI_NET_LWM2M_FOTA_RES_O_PACKAGE_NAME_E ) 
          {
            if (lwm2m_dataP->instance_info->resource_info->value.asBuffer.buffer !=NULL ) 
            {
              LOG_INFO("LWM2M_LOG:FOTA_PKG_NAME->%s\n",lwm2m_dataP->instance_info->resource_info->value.asBuffer.buffer);
            }  
            else 
              LOG_INFO("LWM2M_LOG:failed to get FOTA Package Name\n");
          }  
          else if(lwm2m_dataP->instance_info->resource_info->resource_ID ==  QAPI_NET_LWM2M_FOTA_RES_O_PACKAGE_VERSION_E) 
          {
            if (lwm2m_dataP->instance_info->resource_info->value.asBuffer.buffer !=NULL ) 
            {
              LOG_INFO("LWM2M_LOG:FOTA_PKG_VERSION->%s\n",lwm2m_dataP->instance_info->resource_info->value.asBuffer.buffer);
            } 
            else 
              LOG_INFO("LWM2M_LOG:failed to get FOTA Package Name\n");
          }  
          else if(lwm2m_dataP->instance_info->resource_info->resource_ID ==  QAPI_NET_LWM2M_FOTA_RES_M_UPDATE_DELIVERY_METHOD_E) 
          {
            LOG_INFO("LWM2M_LOG:FOTA_DELIVERY_METHOD->%d\n",lwm2m_dataP->instance_info->resource_info->value.asInteger);
          } 
          lwm2m_dataP->instance_info->resource_info = lwm2m_dataP->instance_info->resource_info->next;
        }  
      }
    } 
}

    
void handle_fw_set_delivery_method(int32_t option)
{
  qapi_Net_LWM2M_Data_t lwm2m_data;
  qapi_Status_t result;

  if((option < 0) || (option > 2) || (gHandler_fota == NULL))
    return;

  memset(&lwm2m_data, 0, sizeof(qapi_Net_LWM2M_Data_t));
  lwm2m_data.object_ID = QAPI_NET_LWM2M_FIRMWARE_UPDATE_OBJECT_ID_E;
  lwm2m_data.no_instances = 1;
  lwm2m_data.instance_info = calloc(sizeof(qapi_Net_LWM2M_Instance_Info_t), 1);
  if(option == 0) {pull_allowed = true; push_allowed = false;}
  if(option == 1) {pull_allowed = false; push_allowed = true;}
  if(option == 2) {pull_allowed = true; push_allowed = true;}
  if(lwm2m_data.instance_info != NULL) {
    lwm2m_data.instance_info->no_resources = 1;
    lwm2m_data.instance_info->instance_ID = 0;
    lwm2m_data.instance_info->resource_info =
      calloc(sizeof(qapi_Net_LWM2M_Resource_Info_t), 1);
    if(lwm2m_data.instance_info->resource_info != NULL) {
      lwm2m_data.instance_info->resource_info->resource_ID =
        QAPI_NET_LWM2M_FOTA_RES_M_UPDATE_DELIVERY_METHOD_E;
      lwm2m_data.instance_info->resource_info->type =
        QAPI_NET_LWM2M_TYPE_INTEGER_E;
      lwm2m_data.instance_info->resource_info->value.asInteger = option;
      result = qapi_Net_LWM2M_Set(gHandler_fota, &lwm2m_data);
      if(result == QAPI_ERROR) {
        LOG_INFO("FOTA : qapi_Net_LWM2M_Set failed \n");
      }
      free(lwm2m_data.instance_info->resource_info);
    }
    free(lwm2m_data.instance_info);
  }
}


void handle_fw_set_protocol_support(int32_t option)
{
  qapi_Net_LWM2M_Data_t lwm2m_data;
  qapi_Status_t result;

  if((option < 0) || (option > 15) || (gHandler_fota == NULL))
    return;

  memset(&lwm2m_data, 0, sizeof(qapi_Net_LWM2M_Data_t));
  lwm2m_data.object_ID = QAPI_NET_LWM2M_FIRMWARE_UPDATE_OBJECT_ID_E;
  lwm2m_data.no_instances = 1;
  lwm2m_data.instance_info = calloc(sizeof(qapi_Net_LWM2M_Instance_Info_t), 1);

  if(lwm2m_data.instance_info != NULL) {
    lwm2m_data.instance_info->no_resources = 1;
    lwm2m_data.instance_info->instance_ID = 0;
    lwm2m_data.instance_info->resource_info =
      calloc(sizeof(qapi_Net_LWM2M_Resource_Info_t), 1);
    if(lwm2m_data.instance_info->resource_info != NULL) {
      lwm2m_data.instance_info->resource_info->resource_ID =
        QAPI_NET_LWM2M_FOTA_RES_O_UPDATE_PROTOCOL_SUPPORT_E;
      lwm2m_data.instance_info->resource_info->type =
        QAPI_NET_LWM2M_TYPE_INTEGER_E;
      lwm2m_data.instance_info->resource_info->value.asInteger = option;
      result = qapi_Net_LWM2M_Set(gHandler_fota, &lwm2m_data);
      if(result == QAPI_ERROR) {
        LOG_INFO("FOTA : qapi_Net_LWM2M_Set failed \n");
      }
      free(lwm2m_data.instance_info->resource_info);
    }
    free(lwm2m_data.instance_info);
  }
}

/**
 * @fn      handle_update_state_input()
 *
 * @brief   This fucntion is used to update state based on the input option
 *
 * @param   option Option to set
 * @return  void
 */
void handle_update_state_input(int32_t option )
{
  qapi_Net_LWM2M_Data_t * lwm2m_data = NULL;
  qapi_Net_LWM2M_Data_t * lwm2m_dataP = NULL;
  qapi_Status_t result = QAPI_OK;

  if (gdownloaded != true ) 
  {
    LOG_INFO( "LWM2M_LOG : setting value is not allowed \n"); 
    return ;
  }

  if ( (0  >= option && 10 <= option))  
  {
    LOG_INFO( "select proper option \n");
    return ;   
  }
  lwm2m_data = (qapi_Net_LWM2M_Data_t *)malloc(sizeof(qapi_Net_LWM2M_Data_t)); 
  if (lwm2m_data == NULL) 
  {
    LOG_INFO ("LWM2M_LOG :  MALLOC  FAILURE \n"); 
    return ;
  }
  memset( lwm2m_data , 0 , sizeof( qapi_Net_LWM2M_Data_t )  );
  lwm2m_data->instance_info = (qapi_Net_LWM2M_Instance_Info_t *) malloc (sizeof (qapi_Net_LWM2M_Instance_Info_t));
  if (lwm2m_data->instance_info == NULL) 
  {
    LOG_INFO ("LWM2M_LOG :  MALLOC  FAILURE \n");
    app_free_data(lwm2m_data);
    return ;
  }
  memset( lwm2m_data->instance_info , 0 , sizeof ( qapi_Net_LWM2M_Instance_Info_t  ) );
  lwm2m_data->instance_info->resource_info = (qapi_Net_LWM2M_Resource_Info_t *) malloc (sizeof (qapi_Net_LWM2M_Resource_Info_t));
  if (lwm2m_data->instance_info->resource_info == NULL) 
  {
    LOG_INFO ("LWM2M_LOG :  MALLOC  FAILURE \n");
    app_free_data(lwm2m_data);    
    return ;
  }
  memset (lwm2m_data->instance_info->resource_info,0,sizeof (qapi_Net_LWM2M_Resource_Info_t));
  lwm2m_data->object_ID = QAPI_NET_LWM2M_FIRMWARE_UPDATE_OBJECT_ID_E;
  lwm2m_data->no_instances++;
  lwm2m_data->instance_info->instance_ID = FOTA_INSTANCE_ID_0;
  lwm2m_data->instance_info->no_resources++;
  lwm2m_data->instance_info->resource_info->resource_ID = QAPI_NET_LWM2M_FOTA_RES_M_UPDATE_RESULT_E;
  lwm2m_data->instance_info->resource_info->type = QAPI_NET_LWM2M_TYPE_INTEGER_E;
  lwm2m_data->instance_info->resource_info->value.asInteger = option; 

  lwm2m_dataP = (qapi_Net_LWM2M_Data_t *)malloc(sizeof(qapi_Net_LWM2M_Data_t)); 
  if (lwm2m_dataP == NULL) 
  {
    LOG_INFO ("LWM2M_LOG :  MALLOC  FAILURE \n"); 
    app_free_data(lwm2m_data);
    return ;
  }
  memset( lwm2m_dataP , 0 , sizeof( qapi_Net_LWM2M_Data_t ) );
  lwm2m_dataP->instance_info = (qapi_Net_LWM2M_Instance_Info_t *) malloc (sizeof (qapi_Net_LWM2M_Instance_Info_t));
  if (lwm2m_dataP->instance_info == NULL) 
  {
    LOG_INFO ("LWM2M_LOG :  MALLOC  FAILURE \n"); 
    app_free_data(lwm2m_data);
    app_free_data(lwm2m_dataP);
    return ;
  }
  memset( lwm2m_dataP->instance_info , 0 , sizeof( qapi_Net_LWM2M_Instance_Info_t ) );
  lwm2m_dataP->instance_info->resource_info = (qapi_Net_LWM2M_Resource_Info_t *) malloc (sizeof (qapi_Net_LWM2M_Resource_Info_t));
  if (lwm2m_dataP->instance_info->resource_info == NULL) 
  {
    LOG_INFO ("LWM2M_LOG :  MALLOC  FAILURE \n");
    app_free_data(lwm2m_data);
    app_free_data(lwm2m_dataP);  
    return ;
  }
  memset(lwm2m_dataP->instance_info->resource_info , 0 , sizeof( qapi_Net_LWM2M_Resource_Info_t ) );
  lwm2m_dataP->object_ID = QAPI_NET_LWM2M_FIRMWARE_UPDATE_OBJECT_ID_E;
  lwm2m_dataP->no_instances++;
  lwm2m_dataP->instance_info->instance_ID = FOTA_INSTANCE_ID_0;
  lwm2m_dataP->instance_info->no_resources++;
  lwm2m_dataP->instance_info->resource_info->resource_ID = QAPI_NET_LWM2M_FOTA_RES_M_STATE_E;
  lwm2m_dataP->instance_info->resource_info->type = QAPI_NET_LWM2M_TYPE_INTEGER_E;

  
  LOG_INFO ("LWM2M_LOG :  gdownloaded=[%d], gupdate[%d]\n", gdownloaded, gupdate);
  
  if ((option == 9) || ((option >= 3 && (option <= 7) && (gdownloaded == true && gupdate == false)))) 
  {
    lwm2m_dataP->instance_info->resource_info->value.asInteger = QAPI_NET_LWM2M_FIRWARE_STATE_IDLE_E;
    gdownloaded = false;
  }
  else if ((option == 2 || option == 8 ) && (gdownloaded == true && gupdate == true))
  {
    lwm2m_dataP->instance_info->resource_info->value.asInteger = QAPI_NET_LWM2M_FIRWARE_STATE_DOWNLOADED_E;
    gupdate = false; 

  } else if ((option == 1) && (gdownloaded == true && gupdate == true))
  {
    lwm2m_dataP->instance_info->resource_info->value.asInteger = QAPI_NET_LWM2M_FIRWARE_STATE_IDLE_E;
    gdownloaded = false;
    gupdate     = false;
  } else if ( (option == 10) && ((gdownloaded == true) && (gupdate == false)) )   
  {
    lwm2m_dataP->instance_info->resource_info->value.asInteger = QAPI_NET_LWM2M_FIRWARE_STATE_DOWNLOADED_E;
    lwm2m_data->instance_info->resource_info->value.asInteger = 0;

  }   
  else {
    LOG_INFO ("LWM2M_LOG : please enter valid updated result based on valid opertaion on firmware  \n");
    app_free_data(lwm2m_data);
    app_free_data(lwm2m_dataP); 
    return  ;
  }

  result = qapi_Net_LWM2M_Set ( gHandler_fota,lwm2m_data);
  if (result == QAPI_ERROR ) {
    LOG_INFO ("qapi set failed \n");
  }
  result = qapi_Net_LWM2M_Set ( gHandler_fota,lwm2m_dataP);
  if (result == QAPI_ERROR ) {
    LOG_INFO ("qapi set failed \n");
  }
  app_free_data(lwm2m_data);
  app_free_data(lwm2m_dataP);

}

/**
 * @fn      handle_fw_set_pkg_version()
 *
 * @brief   This fucntion is used to set pkg version of firmware object
 *
 * @param   buf         Value need to be set 
 * @param   len         Length of resource value 
 *
 * @return  QAPI_OK on Success,
 *          QAPI_ERROR on Failure 
 *          QAPI_ERR_INVALID_PARAM on Invalid parameters provided.
 *          QAPI_ERR_NO_MEMORY on Memory allocation failure.
 */
int handle_fw_set_pkg_version(char* buf, int len)
{
  qapi_Net_LWM2M_Data_t *fw_resource_data = NULL;
  qapi_Status_t result = QAPI_ERROR;

  if(gHandler_fota == NULL)
  {
    LOG_INFO("LWM2M_LOG: Firmware stub application not started.\n");
    return QAPI_ERROR;
  }

  if(buf == NULL || len <= 0)
  {
    LOG_INFO("LWM2M_LOG: Not valid resource value passed.\n");
    return QAPI_ERR_INVALID_PARAM;
  }

  fw_resource_data = (qapi_Net_LWM2M_Data_t *)malloc(sizeof(qapi_Net_LWM2M_Data_t));

  if(fw_resource_data == NULL) 
  {
    return QAPI_ERR_NO_MEMORY;
  }

  memset(fw_resource_data, 0, sizeof(qapi_Net_LWM2M_Data_t));

  fw_resource_data->instance_info = (qapi_Net_LWM2M_Instance_Info_t *)malloc(sizeof (qapi_Net_LWM2M_Instance_Info_t));

  if (fw_resource_data->instance_info == NULL) 
  {
    app_free_data(fw_resource_data);
    return QAPI_ERR_NO_MEMORY;
  }

  memset(fw_resource_data->instance_info, 0, sizeof (qapi_Net_LWM2M_Instance_Info_t));

  fw_resource_data->instance_info->resource_info = (qapi_Net_LWM2M_Resource_Info_t *)malloc(sizeof (qapi_Net_LWM2M_Resource_Info_t));

  if (fw_resource_data->instance_info->resource_info == NULL) 
  {
    app_free_data(fw_resource_data);
    return QAPI_ERR_NO_MEMORY;
  }

  memset(fw_resource_data->instance_info->resource_info, 0, sizeof (qapi_Net_LWM2M_Resource_Info_t));

  fw_resource_data->object_ID = QAPI_NET_LWM2M_FIRMWARE_UPDATE_OBJECT_ID_E;
  fw_resource_data->no_instances = 1;
  fw_resource_data->instance_info->instance_ID = FOTA_INSTANCE_ID_0;
  fw_resource_data->instance_info->no_resources = 1;
  fw_resource_data->instance_info->resource_info->resource_ID = QAPI_NET_LWM2M_FOTA_RES_O_PACKAGE_VERSION_E;
  fw_resource_data->instance_info->resource_info->type = QAPI_NET_LWM2M_TYPE_STRING_E;
  fw_resource_data->instance_info->resource_info->value.asBuffer.length = len;
  fw_resource_data->instance_info->resource_info->value.asBuffer.buffer = (uint8_t *)malloc(len);

  if(fw_resource_data->instance_info->resource_info->value.asBuffer.buffer == NULL)
  {
    app_free_data(fw_resource_data);
    return QAPI_ERR_NO_MEMORY;
  }

  memset(fw_resource_data->instance_info->resource_info->value.asBuffer.buffer, 0, len);

  memcpy((void *)fw_resource_data->instance_info->resource_info->value.asBuffer.buffer,
      (const void*)buf,len);

  result = qapi_Net_LWM2M_Set(gHandler_fota,fw_resource_data);

  if (result == QAPI_ERROR )
  {
    LOG_INFO ("Failed to set resource\n");
  }

  app_free_data(fw_resource_data);

  return result;
}

/**
 * @fn    fota_app_deregister()
 * @brief This fucntion is used to deregister software management application.
 * @param void
 * @return void
 */
void fota_app_deregister()
{
  qapi_Status_t result = QAPI_OK; 
  if(gHandler_fota)
  {      
    result = qapi_Net_LWM2M_DeRegister_App(gHandler_fota);
    if (result == QAPI_ERROR)
    {
      LOG_INFO ("LWM2M_LOG : fota handler is not valid \n");
    }
    gHandler_fota = NULL;    
  }
  else
  {
    LOG_INFO("Handler is not valid\n");  
  }      
  return;
}
