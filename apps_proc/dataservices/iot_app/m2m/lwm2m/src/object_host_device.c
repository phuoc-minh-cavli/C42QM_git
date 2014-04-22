/******************************************************************************

  @file    object_host_device.c
  @brief   File contains the implementation for Host device object
  ---------------------------------------------------------------------------

  Copyright (c) 2016-2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------

 *****************************************************************************/

#include "liblwm2m.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "object_host_device.h"
#include "log_utils.h"
#include "platform.h"
#include "iotapp_qmi_internal.h"
#include "ds_log.h"


#ifdef ENABLE_HOST_DEV_OBJ
void free_object_host_device(lwm2m_object_t * object);
/**
 * @fn static uint8_t prv_res2tlv()
 *
 * @brief This fucntion is used to populate the values of the resource
 *        to be provided to the server during the read operation on the object.
 * @param dataP data array for the resource 
 * @param host_dev_dataP Host Device object data
 * @return LWM2M response type
 */
static uint8_t prv_res2tlv(lwm2m_data_t* dataP,
    object_host_dev_t* host_dev_dataP)
{
  uint8_t ret = LWM2M_205_CONTENT; 
  if( dataP == NULL || host_dev_dataP == NULL ) 
  {
    ds_iot_ulog_err("fun [prv_res2tlv] LWM2M_LOG: NULL Error");
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  switch (dataP->id)     
  {
    case RES_M_HOST_DEV_MANUFACTURER:
      lwm2m_data_encode_string((const char *)host_dev_dataP->host_dev_manufacturer, dataP);
      break;
    case RES_M_HOST_DEV_MODEL:
      lwm2m_data_encode_string((const char *)host_dev_dataP->host_dev_model, dataP);
      break;
    case RES_M_HOST_DEV_ID:
      lwm2m_data_encode_string((const char *)host_dev_dataP->host_dev_id, dataP);
      break;
    default:
      ret = LWM2M_404_NOT_FOUND;
      break;
  }

  return ret;
}

/**
 * @fn static uint8_t prv_host_dev_read()
 *
 * @brief This function implements the read operation performed on Host
 * Device object
 *
 * @param objInstId Object ID of the object to be read
 * @param numDataP number of resources in the array
 * @param tlvArrayP resource value array
 * @param objectP Object to be read
 *
 * @return LWM2M response type
 */
static uint8_t prv_host_dev_read(uint16_t objInstId,
    int*  numDataP,
    lwm2m_data_t** tlvArrayP,
    lwm2m_object_t*  objectP,
    uint16_t resInstId, lwm2m_server_t *serverP)
{
  int     i = 0;
  uint8_t result = LWM2M_500_INTERNAL_SERVER_ERROR;
  object_host_dev_t* host_dev_dataP = NULL;
  if( tlvArrayP == NULL || objectP == NULL || numDataP == NULL) 
  {
    ds_iot_ulog_err("fun [prv_host_dev_read] : LWM2M_LOG: NULL Error");
    return LWM2M_400_BAD_REQUEST;
  }

  host_dev_dataP = (object_host_dev_t*) lwm2m_list_find(objectP->instanceList, objInstId);

  if (host_dev_dataP == NULL) 
  {
    ds_iot_ulog_err ("Not found in list ");
    return LWM2M_404_NOT_FOUND;
  }
  if (*numDataP == 0)     // full object, readable resources!
  {
    uint16_t readResIds[] = {
      RES_M_HOST_DEV_MANUFACTURER,
      RES_M_HOST_DEV_MODEL,
      RES_M_HOST_DEV_ID
    }; // readable resources!

    *numDataP  = sizeof(readResIds)/sizeof(uint16_t);
    *tlvArrayP = lwm2m_data_new(*numDataP);
    if (*tlvArrayP == NULL) return LWM2M_500_INTERNAL_SERVER_ERROR;

    // init readable resource id's
    for (i = 0 ; i < *numDataP ; i++)
    {
      (*tlvArrayP)[i].id = readResIds[i];
    }
  }

  for (i = 0 ; i < *numDataP ; i++)
  {
    result = prv_res2tlv ((*tlvArrayP)+i, host_dev_dataP);
    if (result!=LWM2M_205_CONTENT) break;
  }
  return result;
}

/**
 * @fn static uint8_t prv_host_dev_discover()
 *
 * @brief This fucntion implements the discover operation performed on the
 *     Host Device object
 *
 * @param instanceId Instance ID of the object to be read
 * @param numDataP number of resources in the array
 * @param dataArrayP resource value array
 * @param objectP Object to be read
 *
 * @return LWM2M response type
 *
 */
static uint8_t prv_host_dev_discover(uint16_t instanceId,
    int * numDataP,
    lwm2m_data_t ** dataArrayP,
    lwm2m_object_t * objectP, lwm2m_server_t *serverP)
{
  uint8_t result = LWM2M_205_CONTENT;
  int i = 0;
  if( numDataP == NULL || dataArrayP == NULL || objectP == NULL ) 
  {
    ds_iot_ulog_err("fun [prv_host_dev_discover] : LWM2M_LOG: NULL Error");
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  if(*numDataP == 0)
  {
    uint16_t resList[] = {
      RES_M_HOST_DEV_MANUFACTURER,
      RES_M_HOST_DEV_MODEL,
      RES_M_HOST_DEV_ID       
    };
    int nbRes = sizeof(resList) / sizeof(uint16_t);

    *dataArrayP = lwm2m_data_new(nbRes);
    if (*dataArrayP == NULL) return LWM2M_500_INTERNAL_SERVER_ERROR;
    *numDataP = nbRes;
    for (i = 0; i < nbRes; i++)
    {
      (*dataArrayP)[i].id = resList[i];
    }
  }
  else
  {
    for (i = 0; i < *numDataP && result == LWM2M_205_CONTENT; i++)
    {
      switch ((*dataArrayP)[i].id)
      {
        case RES_M_HOST_DEV_MANUFACTURER:
        case RES_M_HOST_DEV_MODEL:
        case RES_M_HOST_DEV_ID:
          break;
        default:
          result = LWM2M_404_NOT_FOUND;
      }
    }
  }

  return result;                  
}

/**
 * @fn static uint8_t prv_host_dev_create()
 *
 * @brief This fucntion is used to create Host Device object
 *
 * @param instanceId Instance ID of the object to be created
 * @param numData number of resources in the array
 * @param dataArray resource value array
 * @param objectP Object to be read
 *
 * @return LWM2M response type
 */
static uint8_t prv_host_dev_create(uint16_t instanceId,
    int numData,
    lwm2m_data_t * dataArray,
    lwm2m_object_t * objectP, bool apiCall)
{
  object_host_dev_t *host_dev = NULL;

  if( dataArray == NULL || objectP == NULL ) 
  {
    ds_iot_ulog_err("LWM2M_LOG: NULL Error");
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  host_dev = (object_host_dev_t*) lwm2m_calloc(1, sizeof(object_host_dev_t));
  if( NULL == host_dev)
  {
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  host_dev->instanceId = instanceId;

  /** fetch the device manufacturer */
  host_dev->host_dev_manufacturer = get_device_manufacturer();
  if(NULL == host_dev->host_dev_manufacturer)
    host_dev->host_dev_manufacturer = PRV_EMPTY;
  LOG_DEBUG("manufacture code :%s ",host_dev->host_dev_manufacturer);

  /** Fetch the model number of the device */
  host_dev->host_dev_model =  get_device_model_number();
  if(NULL == host_dev->host_dev_model)
    host_dev->host_dev_model = PRV_EMPTY;
  LOG_DEBUG(" Model Number :%s ",host_dev->host_dev_model);

  /** Fetch the device id of the device */
  host_dev->host_dev_id = get_device_serial_number();
  if(NULL == host_dev->host_dev_id)
    host_dev->host_dev_id = PRV_EMPTY;
  LOG_DEBUG("serial number :%s ",host_dev->host_dev_id);

  objectP->instanceList = LWM2M_LIST_ADD(objectP->instanceList, host_dev);

  return LWM2M_201_CREATED;
}

/**
 * @fn static uint8_t prv_host_dev_delete()
 *
 * @brief This fucntion is used to delete Host Device object
 *
 * @param id Instance ID of the object to be deleted
 * @param objectP Object to be deleted
 *
 * @return LWM2M response type
 */
static uint8_t prv_host_dev_delete(uint16_t id,
    lwm2m_object_t * objectP,
    bool persist_file)
{
  object_host_dev_t * host_dev = NULL;

  if( objectP == NULL ) 
  {
    ds_iot_ulog_err("LWM2M_LOG: NULL Error");
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  objectP->instanceList = lwm2m_list_remove(objectP->instanceList, id, (lwm2m_list_t **)&host_dev);
  if (NULL == host_dev) 
  {
    ds_iot_ulog_err1("host device instance id %d not found in list to delete  ",id);    
    return LWM2M_404_NOT_FOUND;
  }

  if(NULL != host_dev->host_dev_manufacturer 
      && strcmp((const char *)host_dev->host_dev_manufacturer, PRV_EMPTY))
  {
    lwm2m_free(host_dev->host_dev_manufacturer);
  }
  if(NULL != host_dev->host_dev_model 
      && strcmp((const char *)host_dev->host_dev_model, PRV_EMPTY))
  {
    lwm2m_free(host_dev->host_dev_model);
  }
  if(NULL != host_dev->host_dev_id 
      && strcmp((const char *)host_dev->host_dev_id, PRV_EMPTY))
  {
    lwm2m_free(host_dev->host_dev_id);
  }

  lwm2m_free (host_dev);
  return LWM2M_202_DELETED;

}
/**
 * @fn lwm2m_object_t * get_object_host_dev()
 *
 * @brief This function is used to get the information regarding the Host
 *        Device object during client registartion
 *
 * @param void
 *
 * @return Host Device object  
 *
 */
lwm2m_object_t * get_object_host_dev(void)
{
  lwm2m_object_t * host_dev_obj = NULL;
  object_host_dev_t *host_dev_data = NULL;

  host_dev_obj = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));
  if(NULL != host_dev_obj)
  {
    memset(host_dev_obj, 0, sizeof(lwm2m_object_t));

    // It assigns its unique ID
    // The 10255 is the standard ID for the optional object "Host Device".

    host_dev_obj->objID = LWM2M_HOST_DEVICE_OBJECT_ID;

    // And the private function that will access the object.
    // Those function will be called when a read query is made by the server.
    // In fact the library don't need to know the resources of the object, only the server does.

    host_dev_obj->readFunc    = prv_host_dev_read;
    host_dev_obj->discoverFunc= prv_host_dev_discover;
    host_dev_obj->createFunc  = prv_host_dev_create;
    host_dev_obj->deleteFunc  = prv_host_dev_delete; 

    host_dev_data = (object_host_dev_t*)lwm2m_malloc(sizeof(object_host_dev_t));

    if (NULL != host_dev_data)
    {
      memset (host_dev_data, 0 , sizeof (object_host_dev_t));  
      /** fetch the device manufacturer */
      host_dev_data->host_dev_manufacturer = get_device_manufacturer();
      if(NULL == host_dev_data->host_dev_manufacturer)
        host_dev_data->host_dev_manufacturer = PRV_EMPTY;
      LOG_DEBUG("manufacture code :%s ",host_dev_data->host_dev_manufacturer);

      /** Fetch the model number of the device */
      host_dev_data->host_dev_model =  get_device_model_number();
      if(NULL == host_dev_data->host_dev_model)
        host_dev_data->host_dev_model = PRV_EMPTY;
      LOG_DEBUG(" Model Number :%s ",host_dev_data->host_dev_model);

      /** Fetch the device id of the device */
      host_dev_data->host_dev_id = get_device_serial_number();
      if(NULL == host_dev_data->host_dev_id)
        host_dev_data->host_dev_id = PRV_EMPTY;
      LOG_DEBUG("serial number :%s ",host_dev_data->host_dev_id);
      host_dev_data->instanceId = 0;

      host_dev_obj->instanceList = LWM2M_LIST_ADD(host_dev_obj->instanceList, host_dev_data); 

    }

    host_dev_obj->userData = NULL;
  }
  return host_dev_obj;
}

/**
 * @fn void free_object_host_device()
 *
 * @brief This fucntion frees the Host Device object allocated
 * @param ObjectP Object to free
 *
 * @return void
 */
void free_object_host_device(lwm2m_object_t * object)
{
  object_host_dev_t *instanceP =  NULL;
  if( object == NULL)  
  {
    ds_iot_ulog_err("fun [free_object_host_device] :LWM2M_LOG: NULL Error");
    return ;
  }

  if (NULL != object->userData)
  {
    instanceP = object->userData;
    if(NULL != instanceP->host_dev_manufacturer 
        && strcmp((const char *)instanceP->host_dev_manufacturer, PRV_EMPTY))
    {
      lwm2m_free(instanceP->host_dev_manufacturer);
    }
    if(NULL != instanceP->host_dev_model 
        && strcmp((const char *)instanceP->host_dev_model, PRV_EMPTY))
    {
      lwm2m_free(instanceP->host_dev_model);
    }
    if(NULL != instanceP->host_dev_id 
        && strcmp((const char *)instanceP->host_dev_id, PRV_EMPTY))
    {
      lwm2m_free(instanceP->host_dev_id);
    }
    object->userData = NULL;
  }
  if (NULL != object->instanceList)
  {
    lwm2m_free(object->instanceList);
    object->instanceList = NULL;
  }

  lwm2m_free(object);
}

#endif

