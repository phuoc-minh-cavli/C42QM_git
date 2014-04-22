/******************************************************************************

  @file    object_firmware.c

  ---------------------------------------------------------------------------

  Copyright (c) 2016-2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------

 *****************************************************************************/

/*******************************************************************************
 *
 * Copyright (c) 2013, 2014 Intel Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * The Eclipse Distribution License is available at
 *    http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Julien Vermillard - initial implementation
 *    Fabien Fleutot - Please refer to git log
 *    David Navarro, Intel Corporation - Please refer to git log
 *    Bosch Software Innovations GmbH - Please refer to git log
 *    Pascal Rieux - Please refer to git log
 *    
 *******************************************************************************/

/*
 * This object is single instance only, and provide firmware upgrade functionality.
 * Object ID is 5.
 */

/*
 * resources:
 * 0 package                   write
 * 1 package url               write
 * 2 update                    exec
 * 3 state                     read
 * 4 update supported objects  read/write
 * 5 update result             read
 */

#include "liblwm2m.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "qapi_coap.h"
#include "lwm2mclient.h"
#include "ds_log.h"


#ifdef ENABLE_FIRMWARE_OBJ

// ---- private object "Firmware" specific defines ----
// Resource Id's:
#define RES_M_PACKAGE                   0
#define RES_M_PACKAGE_URI               1
#define RES_M_UPDATE                    2
#define RES_M_STATE                     3
#define RES_M_UPDATE_RESULT             5
#define RES_O_PKG_NAME                  6
#define RES_O_PKG_VERSION               7
#define RES_O_UPDATE_PROTOCOL           8
#define RES_M_UPDATE_DELIVERY_METHOD    9

#define VALUE_UPDATE_PROTOCOL_COAP  0
#define VALUE_UPDATE_PROTOCOL_COAPS 1
#define VALUE_UPDATE_PROTOCOL_HTTP  2
#define VALUE_UPDATE_PROTOCOL_HTTPS 3

#define VALUE_UPDATE_PROTOCOL_MAX   4

#define BITMASK(x) (0x1 << x)

#define MAX_FIRMWARE_URI_LEN 128
#define FIRMWARE_STR_LEN     128

typedef struct firmware_data_
{
  struct firmware_data_ *next;
  uint16_t instanceId;
  char    uri[MAX_FIRMWARE_URI_LEN];
  bool    updateExecuted;
  bool    supported;
  uint8_t state;
  uint8_t result;
  char package_name[FIRMWARE_STR_LEN];
  char package_version[FIRMWARE_STR_LEN];
  resource_instance_int_list_t *supported_protocol;
  uint8_t delivery_method;
  uint16_t last_server;
} firmware_data_t;

extern bool gInboundFirwareTransferComplete;
bool updateExecuted;
static firmware_data_t *firmwareInstance;

bool is_firmware_create = FALSE;
void free_object_firmware(lwm2m_object_t * objectP);
static void prv_change_protocol_support(firmware_data_t * data, uint8_t protocol_value, bool supported);

typedef struct 
{
  char    uri[MAX_FIRMWARE_URI_LEN];
  uint8_t state;
  uint8_t result;
  char package_name[FIRMWARE_STR_LEN];
  char package_version[FIRMWARE_STR_LEN];
  uint8_t delivery_method;
  pers_resource_instance_int_list_t supported_protocol[VALUE_UPDATE_PROTOCOL_MAX];
} firmware_data_persistent_t;



/** 
 * @fn     store_device_persistent_info()
 * @brief  This function is used to dump resource value set from App into persistence file
 * @param  fmDataP - pointer to Dev object
 * @return on success  - LWM2M_TRUE
           on error    - LWM2M_FALSE
 */ 
bool store_firmware_persistent_info(firmware_data_t* fmDataP)
{
  int fd = 0, index = 0;
  bool res = LWM2M_FALSE;
  firmware_data_persistent_t info;
  resource_instance_int_list_t *supported_protocol = NULL;

  if (NULL == fmDataP)
  {
    LOG_DEBUG("%s",nullError);
    return res;
  }
  ds_iot_log_strf_med("EFS_OPT: File open %s",FIRMWARE_PERSISTENCE_FILE);
  fd = efs_open(FIRMWARE_PERSISTENCE_FILE, O_CREAT | O_WRONLY | O_TRUNC, 00644);
  if (EFAILURE == fd)
  {
    ds_iot_ulog_err("failed to open firmware-object persistence file");
    return res;
  }

  memset(&info, 0x00, sizeof(firmware_data_persistent_t));

  memcpy(info.package_name, fmDataP->package_name, strlen(fmDataP->package_name));
  memcpy(info.uri, fmDataP->uri, strlen(fmDataP->uri));
  memcpy(info.package_version, fmDataP->package_version, strlen(fmDataP->package_version));
  info.state = fmDataP->state;
  info.result = fmDataP->result;
  info.delivery_method = fmDataP->delivery_method;
    
  supported_protocol = fmDataP->supported_protocol;
  while(supported_protocol)
  {
    info.supported_protocol[index].isValid = LWM2M_TRUE;
    info.supported_protocol[index].resInstId = supported_protocol->resInstId;
    info.supported_protocol[index++].InstValue = supported_protocol->InstValue;
    supported_protocol = supported_protocol->next;
  }
  while (index < VALUE_UPDATE_PROTOCOL_MAX)
  {
    info.supported_protocol[index++].isValid = LWM2M_FALSE;
  }
  ds_iot_log_strf_med("EFS_OPT: File write %s",FIRMWARE_PERSISTENCE_FILE);
  if(EFAILURE != efs_write(fd, &info, sizeof(info)))
  {
    res = LWM2M_TRUE;
  }
  
  efs_close(fd);
  if(res == LWM2M_FALSE)
  {//if any error happen when delete the file
    efs_unlink(FIRMWARE_PERSISTENCE_FILE);
    ds_iot_ulog_high("save firmware-object persistence file failed");	
  }
  else
  {
    ds_iot_ulog_high("save firmware-object persistence file success");	
  }
  return res;	
}

/** 
 * @fn     load_firmware_persistent_info()
 * @brief  This function is used to load resource value set from App from persistence file
 * @param  fmDataP - pointer to Dev object
 * @return on success  - LWM2M_TRUE
           on error    - LWM2M_FALSE
 */ 
bool load_firmware_persistent_info(firmware_data_t* fmDataP)
{
  int fd = 0, i = 0;
  firmware_data_persistent_t info;
  int   res = 0;

  if (NULL == fmDataP)
  {
    LOG_DEBUG("%s",nullError);
    return LWM2M_FALSE;
  }
  ds_iot_log_strf_med("EFS_OPT: File open %s",FIRMWARE_PERSISTENCE_FILE);
  fd = efs_open(FIRMWARE_PERSISTENCE_FILE, 00);
  if (EFAILURE == fd)
  {
    ds_iot_ulog_err("failed to open device-object persistence file");
    return LWM2M_FALSE;
  }

  while (1)
  {
    memset(&info, 0x00, sizeof(info));

    res = efs_read(fd, &info, sizeof(info));
    if (res <= 0)
    {
      res = EFAILURE;
		break;
    }

    strlcpy(fmDataP->package_name, (const char*)info.package_name, FIRMWARE_STR_LEN);
    ds_iot_log_strf_high("LWM2M_LOG: load_firmware_persistent_info package_name=%s\n", fmDataP->package_name);
    strlcpy(fmDataP->package_version, (const char*)info.package_version, FIRMWARE_STR_LEN);
    ds_iot_log_strf_high("LWM2M_LOG: load_firmware_persistent_info package_version=%s\n", fmDataP->package_version);
    strlcpy(fmDataP->uri, (const char*)info.uri, MAX_FIRMWARE_URI_LEN);
    ds_iot_log_strf_high("LWM2M_LOG: load_firmware_persistent_info uri=%s\n", fmDataP->uri);
    fmDataP->state = info.state;
    fmDataP->result = info.result;
    fmDataP->delivery_method = info.delivery_method;
    for(i=0; i < VALUE_UPDATE_PROTOCOL_MAX && (info.supported_protocol[i].isValid == true); i++)
    {		    
      prv_change_protocol_support(fmDataP, info.supported_protocol[i].InstValue, true);
      ds_iot_log_high1("LWM2M_LOG: load_firmware_persistent_info update_protocol=%d\n", info.supported_protocol[i].InstValue);
    }
  }
  efs_close(fd);

  if(res == EFAILURE)
  {
    //clean device persistent data if meet failure during load
    memset(fmDataP->package_name, 0, FIRMWARE_STR_LEN);
    memset(fmDataP->package_version, 0, FIRMWARE_STR_LEN);
    memset(fmDataP->uri, 0, MAX_FIRMWARE_URI_LEN);
	fmDataP->state = LWM2M_FIRWARE_STATE_IDLE;
	fmDataP->result = LWM2M_FIRWARE_UPDATE_DEFAULT;
	fmDataP->delivery_method = 0;
	prv_change_protocol_support(fmDataP, VALUE_UPDATE_PROTOCOL_COAP, false);
	prv_change_protocol_support(fmDataP, VALUE_UPDATE_PROTOCOL_COAPS, false);
	prv_change_protocol_support(fmDataP, VALUE_UPDATE_PROTOCOL_HTTP, false);
	prv_change_protocol_support(fmDataP, VALUE_UPDATE_PROTOCOL_HTTPS, false);

	//delete the file since read it failure
    efs_unlink(FIRMWARE_PERSISTENCE_FILE);
  }

  return LWM2M_TRUE;
}

/** 
* @fn static uint8_t prv_firmware_create_write_check() 
*
* @brief This fucntion verifies and write the resource's into  
*        Firmware object 
*
* @param instanceId Instance of the firmware object to be create/write 
* @param numDataP number of resources in the array 
* @param dataArrayP resource value array 
* @param create_flag - if true creating instance else write on instance 
*
* @return LWM2M response type 
*
*/
/***************************************************************************************************

Under section 8.2.2 Operation on Object Instance of OMA 1.1

- For the "Write" operation on an Object Instance, the LwM2M Client MUST perform the operation, 
if all the Resources conveyed in the operation are allowed to perform the "Write" operation. 
If any Resource does not support the "Write" operation, the LwM2M Client MUST inform the LwM2M 
Server that the Object Instance cannot perform the requested "Write" operation in sending a 
"Operation is not supported" error code.

*************************************************************************************************************/  
static uint8_t prv_firmware_create_write_check(int numData,
                                               lwm2m_data_t * dataArray,
                                               bool create_flag)
{ 
  int16_t i = 0; 
  uint8_t result = LWM2M_NO_ERROR;
  if(NULL == dataArray)
  { 
    LOG_DEBUG("%s", nullError); 
    return LWM2M_500_INTERNAL_SERVER_ERROR; 
  } 
  i = 0;
   
  do
  {
    switch(dataArray[i].id)
    {
      case RES_M_UPDATE:
	  case RES_M_STATE:
      case RES_M_UPDATE_RESULT:
      case RES_O_PKG_NAME:
      case RES_O_PKG_VERSION:
      case RES_O_UPDATE_PROTOCOL:
      case RES_M_UPDATE_DELIVERY_METHOD:
      if (!create_flag)
      {
         result = LWM2M_405_METHOD_NOT_ALLOWED;
         break;
      }
      case RES_M_PACKAGE:
	  case RES_M_PACKAGE_URI:
         result = LWM2M_204_CHANGED;
         break;
      default:
        if(!create_flag)
		 {        
		    result = LWM2M_404_NOT_FOUND;        
			break;	   	 
		  }      
		  else      
		  {        
		     result = LWM2M_204_CHANGED;	  
		  }
    }
  }while(++i < numData && result == LWM2M_204_CHANGED);

  if(result != LWM2M_204_CHANGED)
  {
     ds_iot_ulog_high1("failed for resource id %d",dataArray[i-1].id);
     return result;
  }
   return result;
}




/**
 * @fn static uint8_t prv_set_value()
 * @brief This fucntion is used to populate the values of the resource
 * to be provided to the server during the read operation on the object.
 * @param dataP (OUT) data array for the resource 
 * @param firmwareDataP (IN) firmware object data
 * @return LWM2M response type
 */
#if 0
static uint8_t prv_set_value(lwm2m_data_t * dataP,
                              firmware_data_t * firmwareDataP)
{
   if( dataP == NULL || firmwareDataP == NULL ) 
  {
    LOG_ERROR("fun [%s] file [%s]  %s",__func__,__FILE__,nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  } 
  
  switch (dataP->id)
  {
    case RES_M_PACKAGE_URI:
      lwm2m_data_encode_string(firmwareDataP->uri, dataP);
      return LWM2M_205_CONTENT;
    case RES_M_STATE:
      lwm2m_data_encode_int(firmwareDataP->state, dataP);
      return LWM2M_205_CONTENT;
    case RES_M_UPDATE_RESULT :
      lwm2m_data_encode_int(firmwareDataP->result, dataP);
      return LWM2M_205_CONTENT;
    case RES_O_PKG_NAME:
      lwm2m_data_encode_string(firmwareDataP->package_name, dataP);
      return LWM2M_205_CONTENT;
    case RES_O_PKG_VERSION:
      lwm2m_data_encode_string(firmwareDataP->package_version, dataP);
      return LWM2M_205_CONTENT; 
    case RES_M_UPDATE_DELIVERY_METHOD:
      lwm2m_data_encode_int(firmwareDataP->delivery_method, dataP);
      return LWM2M_205_CONTENT;    
    case RES_M_PACKAGE:
    case RES_M_UPDATE :
    case RES_O_UPDATE_PROTOCOL:
      return LWM2M_405_METHOD_NOT_ALLOWED;
    default:
	  LOG_INFO("In default case of prv set for read on resource %d",dataP->id);
      return LWM2M_404_NOT_FOUND;
  }
}
#endif
    
/**
 * @fn static uint8_t prv_firmware_read()
 *
 * @brief This function implements the read operation performed on Firmware
 *         object
 *
 * @param instanceId Instance ID of the object to be read
 * @param numDataP number of resources in the array
 * @param dataArrayP resource value array
 * @param objectP Object to be read
 *
 * @return LWM2M response type
 */
static uint8_t prv_firmware_read(uint16_t instanceId,
    int * numDataP,
    lwm2m_data_t ** dataArrayP,
    lwm2m_object_t * objectP,
    uint16_t resInstId, lwm2m_server_t *serverP)
{
  int i = 0;
  uint8_t result = 0;
  firmware_data_t * data = NULL;
  if( numDataP == NULL || dataArrayP == NULL || objectP == NULL ) 
  {
    ds_iot_ulog_err("fun [prv_firmware_read]:LWM2M_LOG: NULL Error");
    return LWM2M_500_INTERNAL_SERVER_ERROR;   
  }

  data = (firmware_data_t*)(objectP->userData);

  // this is a single instance object
  if (instanceId != 0)
  {
    return LWM2M_404_NOT_FOUND;
  }

  // is the server asking for the full object ?
  if (*numDataP == 0)
  {
    uint16_t resList[] = {
      RES_M_PACKAGE_URI,
      RES_M_STATE,
      RES_M_UPDATE_RESULT ,
      RES_O_PKG_NAME ,
      RES_O_PKG_VERSION ,
      RES_O_UPDATE_PROTOCOL,
      RES_M_UPDATE_DELIVERY_METHOD
    };
    int nbRes = sizeof(resList) / sizeof(uint16_t);
    int num = nbRes;

    if(data->supported_protocol == NULL)
      nbRes--;

    *dataArrayP = lwm2m_data_new(nbRes);
    if (*dataArrayP == NULL) return LWM2M_500_INTERNAL_SERVER_ERROR;
    *numDataP = nbRes;
    for (i = 0; i < nbRes; i++)
    { 
      if(resList[i] == RES_O_UPDATE_PROTOCOL) {
        if(data->supported_protocol != NULL)
          (*dataArrayP)[i].id = resList[i];
        else if(i < (num-1))
          (*dataArrayP)[i].id = resList[i+1];
      } else
        (*dataArrayP)[i].id = resList[i];  
    }
  }

  i = 0;
  do
  {
    switch ((*dataArrayP)[i].id)
    {
      case RES_M_PACKAGE:
      case RES_M_UPDATE:
        result = LWM2M_405_METHOD_NOT_ALLOWED;
        break;

      case RES_M_PACKAGE_URI:
        lwm2m_data_encode_string(data->uri, *dataArrayP + i);
        result = LWM2M_205_CONTENT;
        break;
      case RES_M_STATE:
        // firmware update state (int)
        lwm2m_data_encode_int(data->state, *dataArrayP + i);
        result = LWM2M_205_CONTENT;
        break;
      case RES_O_PKG_NAME:
        lwm2m_data_encode_string(data->package_name, *dataArrayP + i);
        result = LWM2M_205_CONTENT;
        break;
      case RES_O_PKG_VERSION:
        lwm2m_data_encode_string(data->package_version, *dataArrayP + i);
        result = LWM2M_205_CONTENT;
        break;           
      case RES_M_UPDATE_RESULT:
        lwm2m_data_encode_int(data->result, *dataArrayP + i);
        result = LWM2M_205_CONTENT;
        break;
      case RES_M_UPDATE_DELIVERY_METHOD:
        lwm2m_data_encode_int(data->delivery_method, *dataArrayP + i);
        result = LWM2M_205_CONTENT;
        break;
      case RES_O_UPDATE_PROTOCOL:
        {
          resource_instance_int_list_t* protocolList = NULL;
          if(resInstId != INVALID_RES_INST_ID)
          {
            resource_instance_int_list_t *resInst = (resource_instance_int_list_t *)LWM2M_LIST_FIND(data->supported_protocol, resInstId);
            if(resInst != NULL)
            {
              lwm2m_data_encode_int(resInst->InstValue, *dataArrayP + i);
              result = LWM2M_205_CONTENT;
            }
            else
			{
			  ds_iot_ulog_high("fota read failed for support protocol valid resource instance sendin not found");
              result = LWM2M_404_NOT_FOUND;
			}
          }
          else
          {
            uint8_t ri = 0;
            if (data->supported_protocol)
			{
			   ri++;
               for(protocolList = data->supported_protocol;protocolList->next != NULL; protocolList = protocolList->next, ri++);
			}

            if(ri == 0) // no values!
            {
              lwm2m_data_t* subTlvP = NULL;
              lwm2m_data_encode_instances(subTlvP, 0, *dataArrayP);
              return LWM2M_205_CONTENT;
            }
            else
            {
              lwm2m_data_t *subTlvP = lwm2m_data_new(ri);
              if(NULL == subTlvP)
                return LWM2M_500_INTERNAL_SERVER_ERROR;

              for(protocolList = data->supported_protocol, ri = 0; protocolList != NULL;
                  protocolList = protocolList->next, ri++)
              {
                subTlvP[ri].id = (uint16_t)protocolList->resInstId;
                lwm2m_data_encode_int(protocolList->InstValue, &subTlvP[ri]);
              }
              lwm2m_data_encode_instances(subTlvP, ri, *dataArrayP + i);
              result = LWM2M_205_CONTENT;
            }
          }
        }
        break;
      default:
        result = LWM2M_404_NOT_FOUND;
    }

    i++;
  } while (i < *numDataP && result == LWM2M_205_CONTENT);

  return result;
}

//} 

static void prv_change_protocol_support(firmware_data_t * data, uint8_t protocol_value, bool supported)
{

  resource_instance_int_list_t *prev = NULL;
  resource_instance_int_list_t *protocolList = NULL;
  uint8_t ri = 0;
  bool found = false;

  for(protocolList = data->supported_protocol, prev = data->supported_protocol, ri = 0;
      protocolList != NULL; protocolList = protocolList->next, ri++)
  {
    if(protocol_value == protocolList->InstValue)
    {
      if(supported == true)
        return;
      else {
        found = true;
        break;
      }
    }
    prev = protocolList;
  }

  if(supported)
  {
    resource_instance_int_list_t *new = NULL;

    new = lwm2m_calloc(sizeof(resource_instance_int_list_t), 1);
    if(new != NULL) {
      if(prev == NULL) {
        data->supported_protocol = new;
      } else {
        prev->next = new;
      }
      new->resInstId = ri;
      new->InstValue = protocol_value;
      ds_iot_ulog_high2("FOTA adding resource instance %d protocol %d", ri, protocol_value);
      return;
    }else
      return;
  } 
  else 
  {
    if(found == true) {
      if(data->supported_protocol == protocolList)
        data->supported_protocol = protocolList->next;
      else {
        prev->next = protocolList->next;
      }     
      ds_iot_ulog_high2("FOTA removing instance %d value %d", protocolList->resInstId, protocolList->InstValue);
      lwm2m_free(protocolList); 
    }
  }

}

/*
 * @fn      static int32_t prv_firmware_setvalue ()
 * @brief   This function is used to set the particular resource value from pllication
 * @param   lwm2m_data - pointer to lwm2m object information
 targetP    - pointer to lwm2m object 
 * @return  on success - 1
 on error   - 0    
 */
static int32_t prv_firmware_setvalue (lwm2m_object_data_t * lwm2m_data, lwm2m_object_t * targetP)
{
  int result = 0;
  bool val_change = false;
  if(lwm2m_data == NULL || targetP == NULL ||
      lwm2m_data->instance_info == NULL ||
      lwm2m_data->instance_info->resource_info == NULL) {
      ds_iot_ulog_err("LWM2M_LOG: NULL Error");
    return 0;
  }

  if(((firmware_data_t *)targetP->userData) == NULL) return 0;

  switch(lwm2m_data->instance_info->resource_info->resource_ID) 
  {
    case RES_M_UPDATE_RESULT: 
      {
        /* Update result range is 0 -9 */
        if(lwm2m_data->instance_info->resource_info->value.asInteger < 0 
           || lwm2m_data->instance_info->resource_info->value.asInteger > 9)
        {
          result = 0;
          break;
        }

        if(lwm2m_data->instance_info->resource_info->type == LWM2M_TYPE_INTEGER)
        {  

          if(((firmware_data_t *)targetP->userData)->result != 
              lwm2m_data->instance_info->resource_info->value.asInteger)
          {
            val_change = true;
          }
          ((firmware_data_t *)targetP->userData)->result =
            lwm2m_data->instance_info->resource_info->value.asInteger;

          result = 1;
        } 
        else
        {
          result = 0;
        }
      }
      break;
    case RES_M_STATE: 
      {
        /* State range is 0 -3 */
        if(lwm2m_data->instance_info->resource_info->value.asInteger < 0 
           || lwm2m_data->instance_info->resource_info->value.asInteger > 3)
        {
          result = 0;
          break;
        }
        if(lwm2m_data->instance_info->resource_info->type == LWM2M_TYPE_INTEGER)
        {
          if(((firmware_data_t *)targetP->userData)->state != 
              lwm2m_data->instance_info->resource_info->value.asInteger)
          {
            val_change = true;
          }
          ((firmware_data_t *)targetP->userData)->state = lwm2m_data->instance_info->resource_info->value.asInteger;
          result = 1;
        } 
        else 
        {
          result = 0;
        }
      }
      break;
    case RES_O_PKG_NAME:
      {
        if(lwm2m_data->instance_info->resource_info->type == LWM2M_TYPE_STRING)
        {
          if(lwm2m_strcmp(((firmware_data_t *)targetP->userData)->package_name, 
              (char *)lwm2m_data->instance_info->resource_info->value.asBuffer.buffer) != 0)
          {
            val_change = true;
          }
          strlcpy(((firmware_data_t *)targetP->userData)->package_name, 
              (const char *)lwm2m_data->instance_info->resource_info->value.asBuffer.buffer, 
              FIRMWARE_STR_LEN);
          result = 1;
        }
        else
        {
          result = 0;
        }
      }
      break;
    case RES_O_PKG_VERSION:
      {
        if(lwm2m_data->instance_info->resource_info->type == LWM2M_TYPE_STRING)
        {  
          if(lwm2m_strcmp(((firmware_data_t *)targetP->userData)->package_version, 
              (char *)lwm2m_data->instance_info->resource_info->value.asBuffer.buffer) != 0)
          {
            val_change = true;
          }
          strlcpy(((firmware_data_t *)targetP->userData)->package_version, 
              (const char *)lwm2m_data->instance_info->resource_info->value.asBuffer.buffer, 
              FIRMWARE_STR_LEN);
          result = 1;
        }
        else
        {
          result = 0;
        }
      }
      break;
    case RES_M_UPDATE_DELIVERY_METHOD:
      {
        /* Firmware update delivery method range is 0 -2 */
        if(lwm2m_data->instance_info->resource_info->value.asInteger < 0 
           || lwm2m_data->instance_info->resource_info->value.asInteger > 2)
        {
          result = 0;
          break;
        }

        if(lwm2m_data->instance_info->resource_info->type == LWM2M_TYPE_INTEGER)
        {
          if(((firmware_data_t *)targetP->userData)->delivery_method != 
              lwm2m_data->instance_info->resource_info->value.asInteger)
          {
            val_change = true;
          }
          ((firmware_data_t *)targetP->userData)->delivery_method = lwm2m_data->instance_info->resource_info->value.asInteger;
          result = 1;
        }
        else
        {
          result = 0;
        }
      }
      break;
    case RES_O_UPDATE_PROTOCOL:
      {
        int protocol_bitmask = 0;
        bool coap_support, coaps_support, http_support, https_support;
        /* In case of multiple resource, each bit is treated as a protocol */

        if(lwm2m_data->instance_info->resource_info->type == LWM2M_TYPE_MULTIPLE_RESOURCE)
        {
          int ri = 0;
          lwm2m_data_t* subTlvArray = NULL;
          resource_instance_int_list_t *save_list = ((firmware_data_t *)targetP->userData)->supported_protocol;
          ((firmware_data_t *)targetP->userData)->supported_protocol = NULL;

          subTlvArray = lwm2m_data->instance_info->resource_info->value.asChildren.array;
          if (lwm2m_data->instance_info->resource_info->value.asChildren.count == 0)
          {
            result = 0;
          }
          else if (subTlvArray==NULL)
          {
            result = 0;
          }
          else
          {
            for (ri=0; ri < lwm2m_data->instance_info->resource_info->value.asChildren.count; ri++)
            {
              if(subTlvArray[ri].value.asInteger < 0 
                 || subTlvArray[ri].value.asInteger > 3)
              {
                result = 0;
                break;
              }
              prv_change_protocol_support((firmware_data_t *)targetP->userData, subTlvArray[ri].value.asInteger, true);
              val_change = true;
              result = 1;
            }
          }
          if(result == 1)
          {
            LWM2M_LIST_FREE(save_list);
          }
          else
          {
            LWM2M_LIST_FREE(((firmware_data_t *)targetP->userData)->supported_protocol);
            ((firmware_data_t *)targetP->userData)->supported_protocol = save_list;
          }
        }
        else
        {
        /* Update protocol range is 0 -5 */
        if(lwm2m_data->instance_info->resource_info->value.asInteger < 0 
             || lwm2m_data->instance_info->resource_info->value.asInteger > 15)
        {
          result = 0;
          break;
        }
  
        if(lwm2m_data->instance_info->resource_info->type == LWM2M_TYPE_INTEGER)
        {
          protocol_bitmask = lwm2m_data->instance_info->resource_info->value.asInteger;
  
          coap_support  = protocol_bitmask & BITMASK(VALUE_UPDATE_PROTOCOL_COAP)  ?true:false;
          coaps_support = protocol_bitmask & BITMASK(VALUE_UPDATE_PROTOCOL_COAPS) ?true:false;
          http_support  = protocol_bitmask & BITMASK(VALUE_UPDATE_PROTOCOL_HTTP)  ?true:false;
          https_support = protocol_bitmask & BITMASK(VALUE_UPDATE_PROTOCOL_HTTPS) ?true:false;
  
          prv_change_protocol_support((firmware_data_t *)targetP->userData, VALUE_UPDATE_PROTOCOL_COAP, coap_support);
          prv_change_protocol_support((firmware_data_t *)targetP->userData, VALUE_UPDATE_PROTOCOL_COAPS, coaps_support);
          prv_change_protocol_support((firmware_data_t *)targetP->userData, VALUE_UPDATE_PROTOCOL_HTTP, http_support);
          prv_change_protocol_support((firmware_data_t *)targetP->userData, VALUE_UPDATE_PROTOCOL_HTTPS, https_support);
          val_change = true;
          result = 1;
        } 
        else 
        {
          result = 0;
        }
      }
      }
      break;
    case RES_M_PACKAGE_URI:
      {
        if(lwm2m_data->instance_info->resource_info->type == LWM2M_TYPE_STRING
           || lwm2m_data->instance_info->resource_info->type == LWM2M_TYPE_OPAQUE)
        {  
          if(lwm2m_strcmp(((firmware_data_t *)targetP->userData)->uri, 
              (char *)lwm2m_data->instance_info->resource_info->value.asBuffer.buffer) != 0)
          {
            val_change = true;
          }
          strlcpy(((firmware_data_t *)targetP->userData)->uri, 
              (const char *)lwm2m_data->instance_info->resource_info->value.asBuffer.buffer, 
              MAX_FIRMWARE_URI_LEN);
          result = 1;
        }
        else
        {
          result = 0;
        }
      }
      break;
    default :
      result = 0;
      break;
  }

  if(result == 1 && val_change == true)
  {
    resource_value_change(lwm2m_data);
    store_firmware_persistent_info(firmwareInstance);  
  }
  return result;
}

/**
 * @brief This function is used to write the resources into object instance
 *        while creating the object instance.or got write request from server.

 * @param   objectP (IN)   handle to object
 * @param   instanceP (IN/OUT)   handle to object instance
 * @param   numData (IN)        number of resources to be written
 * @param   dataArray (IN)      array of resource values
 * @param resInstId Resource instance id or INVALID_RES_INST_ID 
 *                  if resource instance id is not passed.
 * @param write_method To distinguish between partial write or replace.
 * @return LWM2M response type
 */
static uint8_t prv_firmware_write_resources(lwm2m_object_t * objectP,
    int numData,
                                            lwm2m_data_t *dataArray,
    uint16_t resInstId, uint8_t write_method)
{
  int i = 0;
  uint8_t result = 0;
  lwm2m_object_data_t lwm2m_data;
  uint8_t id_set = 0;
  firmware_data_t *data = NULL;
  int8_t ret_val = 0;
  if(dataArray == NULL || objectP == NULL)
  {
    LOG_DEBUG("fun [%s] %s",__func__, nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }
  data = (firmware_data_t *)(objectP->userData);
  memset(&lwm2m_data, 0, sizeof(lwm2m_object_data_t));

  i = 0;

  ds_iot_ulog_high1("Fota object state %d",data->state);
  do
  {
    switch(dataArray[i].id)
    {
      /* 
            State             Package          Result
         1) Idle/Downloading  Non-NULL         2.04
         2) Idle/Downloading   NULL            5.00 
         3) Downloaded        Non-NULL         4.05
         4) Downloaded         NULL            2.04
      */
      case RES_M_PACKAGE:
        if(LWM2M_FIRWARE_STATE_DOWNLOADING != data->state &&
            LWM2M_FIRWARE_STATE_IDLE != data->state && 
            LWM2M_FIRWARE_STATE_DOWNLOADED != data->state && 
            LWM2M_FIRWARE_STATE_UPDATING != data->state)
        {
          result = LWM2M_405_METHOD_NOT_ALLOWED;
        }
        else
        {
          lwm2m_context_t *context = get_lwm2m_context();
          if(data->state == LWM2M_FIRWARE_STATE_IDLE)
          {
            if(context && context->activeServerP)
              data->last_server = context->activeServerP->shortID;
          }
          else if(data->state == LWM2M_FIRWARE_STATE_DOWNLOADING)
          {
            if(data->last_server != context->activeServerP->shortID)
            {
              ds_iot_ulog_err("FOTA write request from different server.");
              return LWM2M_405_METHOD_NOT_ALLOWED;
            }
          }
          if (0 != (ret_val = prv_init_resources(&lwm2m_data, &id_set, LWM2M_FIRMWARE_UPDATE_OBJECT_ID, 0, RES_M_PACKAGE)))
          return ret_val;

          lwm2m_data.instance_info->resource_info->type = (lwm2m_value_type_t)dataArray[i].type;
          lwm2m_data.instance_info->resource_info->value.asBuffer.length =
                dataArray[i].value.asBuffer.length;

          if(lwm2m_data.instance_info->resource_info->value.asBuffer.length
              && LWM2M_FIRWARE_STATE_DOWNLOADED != data->state)
          {
            lwm2m_data.instance_info->resource_info->value.asBuffer.buffer =
                  lwm2m_malloc(lwm2m_data.instance_info->resource_info->value
                      .asBuffer.length);
            if(lwm2m_data.instance_info->resource_info->value.asBuffer.buffer == NULL)
            {
              return LWM2M_500_INTERNAL_SERVER_ERROR;
            }
            memset(lwm2m_data.instance_info->resource_info->value.asBuffer.buffer,
                  0, lwm2m_data.instance_info->resource_info->value.asBuffer.length);
            memscpy(lwm2m_data.instance_info->resource_info->value.asBuffer.buffer,
                    lwm2m_data.instance_info->resource_info->value.asBuffer.length,
                    dataArray[i].value.asBuffer.buffer,
                    lwm2m_data.instance_info->resource_info->value.asBuffer.length);

            lwm2m_data.instance_info->resource_info->value.asBuffer.block1_more=
             dataArray[i].value.asBuffer.block1_more;
            lwm2m_data.instance_info->resource_info->value.asBuffer.block1_size=
             dataArray[i].value.asBuffer.block1_size;
            lwm2m_data.instance_info->resource_info->value.asBuffer.block1_num=
             dataArray[i].value.asBuffer.block1_num;
            lwm2m_data.instance_info->resource_info->value.asBuffer.block1_offset=
             dataArray[i].value.asBuffer.block1_offset;
            lwm2m_data.instance_info->resource_info->value.asBuffer.size1=
             dataArray[i].value.asBuffer.size1;
          }
          else if(lwm2m_data.instance_info->resource_info->value.asBuffer.length
                  && LWM2M_FIRWARE_STATE_DOWNLOADED == data->state)
          {
            lwm2m_free_instance_info(lwm2m_data.instance_info);
            return LWM2M_405_METHOD_NOT_ALLOWED;
          }
          else if(lwm2m_data.instance_info->resource_info->value.asBuffer.length == 0 ||
                  lwm2m_data.instance_info->resource_info->value.asBuffer.buffer == NULL)
          {
            lwm2m_data.instance_info->resource_info->value.asBuffer.length = 0;
            lwm2m_data.instance_info->resource_info->value.asBuffer.buffer = NULL;
          }

          lwm2m_check_if_observe_and_update_app (id_set, LWM2M_FIRMWARE_UPDATE_OBJECT_ID, 0, RES_M_PACKAGE, &lwm2m_data);
          lwm2m_free_instance_info(lwm2m_data.instance_info);
          result = LWM2M_NO_ERROR;
          data->state = LWM2M_FIRWARE_STATE_DOWNLOADING;
          if(gInboundFirwareTransferComplete == true)
          {
            result = LWM2M_204_CHANGED;
            data->state = LWM2M_FIRWARE_STATE_DOWNLOADED;
            data->last_server = 0; // Reset the last server
          }
        }
        break;

      /* 
            State             Package-URI     Result
         1) Idle              Non-NULL         2.04
         2) Idle               NULL            5.00 
         3) Downloaded        Non-NULL         4.05
         4) Downloaded         NULL            2.04
      */
      case RES_M_PACKAGE_URI:
        if(LWM2M_FIRWARE_STATE_IDLE != data->state && 
           LWM2M_FIRWARE_STATE_DOWNLOADED != data->state &&
           LWM2M_FIRWARE_STATE_DOWNLOADING != data->state &&
           LWM2M_FIRWARE_STATE_UPDATING != data->state)
        {
          result = LWM2M_405_METHOD_NOT_ALLOWED;
        }
        else
        {
          if (0 != (ret_val = prv_init_resources(&lwm2m_data, &id_set, LWM2M_FIRMWARE_UPDATE_OBJECT_ID, 0, RES_M_PACKAGE_URI)))
          return ret_val;

          lwm2m_data.instance_info->resource_info->type = (lwm2m_value_type_t)dataArray[i].type;
              lwm2m_data.instance_info->resource_info->value.asBuffer.length =
                dataArray[i].value.asBuffer.length;

              if(lwm2m_data.instance_info->resource_info->value.asBuffer.length && 
                 LWM2M_FIRWARE_STATE_DOWNLOADED != data->state)
              {
                lwm2m_data.instance_info->resource_info->value.asBuffer.buffer =
              lwm2m_malloc(lwm2m_data.instance_info->resource_info->value.asBuffer.length);
            if(lwm2m_data.instance_info->resource_info->value.asBuffer.buffer == NULL)
                {
                  return LWM2M_500_INTERNAL_SERVER_ERROR;
                }
            memset(lwm2m_data.instance_info->resource_info->value.asBuffer.buffer,
                0, lwm2m_data.instance_info->resource_info->value.asBuffer.length);
            memscpy(lwm2m_data.instance_info->resource_info->value.asBuffer.buffer,
                    lwm2m_data.instance_info->resource_info->value.asBuffer.length,
                    dataArray[i].value.asBuffer.buffer,
                    lwm2m_data.instance_info->resource_info->value.asBuffer.length);
            memset(data->uri,0, MAX_FIRMWARE_URI_LEN);
            memscpy(data->uri,MAX_FIRMWARE_URI_LEN,dataArray[i].value.asBuffer.buffer,
                    lwm2m_data.instance_info->resource_info->value.asBuffer.length);
              }
              else if(!lwm2m_data.instance_info->resource_info->value.asBuffer.length ||
                      lwm2m_data.instance_info->resource_info->value.asBuffer.buffer == NULL)
              {
                memset(data->uri,0, MAX_FIRMWARE_URI_LEN);
                lwm2m_data.instance_info->resource_info->value.asBuffer.length = 0;
                lwm2m_data.instance_info->resource_info->value.asBuffer.buffer = NULL;
              }
              else if(lwm2m_data.instance_info->resource_info->value.asBuffer.length && 
                      LWM2M_FIRWARE_STATE_DOWNLOADED == data->state)
              {
                lwm2m_free_instance_info(lwm2m_data.instance_info);
                return LWM2M_405_METHOD_NOT_ALLOWED;
              }

          lwm2m_check_if_observe_and_update_app(id_set, 5, 0, RES_M_PACKAGE_URI, &lwm2m_data);
          lwm2m_free_instance_info(lwm2m_data.instance_info);
          result = LWM2M_204_CHANGED;
        }
        break;
      case RES_M_UPDATE:
      case RES_M_STATE:
      case RES_M_UPDATE_RESULT:
      case RES_O_PKG_NAME:
      case RES_O_PKG_VERSION:
      case RES_O_UPDATE_PROTOCOL:
      case RES_M_UPDATE_DELIVERY_METHOD:
        /*If create/write have one more one resource to write we write on reablable resource*/
        if(numData > 1)
          result = LWM2M_204_CHANGED;
        else
          result = LWM2M_405_METHOD_NOT_ALLOWED;
      default:
        /*If create/write have one more one resource to write we write on unknown/notsupported resource*/
        if(numData > 1)
          result = LWM2M_204_CHANGED;
        else
          result = LWM2M_404_NOT_FOUND;
    }

    i++;
  } while(i < numData && result == LWM2M_204_CHANGED);
  
  return result;
  
}


/**
 * @fn static uint8_t prv_firmware_write()
 *
 * @brief This fucntion implements the write operation performed on 
 *        Firmware object
 *
 * @param instanceId Instance ID of the object to be read
 * @param numData number of resources in the array
 * @param dataArray resource value array
 * @param objectP Object to be read
 *
 * @return LWM2M response type
 */
static uint8_t prv_firmware_write(uint16_t instanceId,
    int numData,
    lwm2m_data_t * dataArray,
    lwm2m_object_t * objectP,
    uint16_t resInstId, uint8_t write_method, bool bs_true)
{
 
  firmware_data_t *firmwareObj = NULL;
  uint8_t result = LWM2M_205_CONTENT;
  if(dataArray == NULL || objectP == NULL)
  {
    ds_iot_ulog_err("fun [prv_firmware_write]: LWM2M_LOG: NULL Error");
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }
  firmwareObj = (firmware_data_t *)LWM2M_LIST_FIND(objectP->instanceList, 
                                                   instanceId);

  if(firmwareObj == NULL)
  {
    return LWM2M_404_NOT_FOUND;
  }

  // this is a single instance object
  if(instanceId != 0)
  {
    return LWM2M_404_NOT_FOUND;
  }
  result = prv_firmware_create_write_check(numData, dataArray, bs_true);
  if (result != LWM2M_204_CHANGED )
  {
    ds_iot_ulog_err ("Write operation failed due to write on non-writable resource.");
    return result;
          }

  result = prv_firmware_write_resources(objectP, numData, dataArray,
                                        resInstId, write_method);
  return result;
}

/**
 * @fn static uint8_t prv_firmware_execute()
 *
 * @brief This fucntion implements the execute operation performed on 
 *        Firmware object
 *
 * @param instanceId Instance ID of the object to be read
 * @param resourceId Resource ID of the resource to be executed
 * @param buffer message payload from server fro execute operation
 * @param length message payload length
 * @param objectP Object on which the operation needs to be executed
 *
 * @return LWM2M response type
 *
 */
static uint8_t prv_firmware_execute(uint16_t instanceId,
    uint16_t resourceId,
    uint8_t * buffer,
    int length,
    lwm2m_object_t * objectP)
{
  firmware_data_t *data = NULL;
  lwm2m_object_data_t lwm2m_data;
  uint8_t id_set = 0;
  uint8_t ret_val = 0;

  if(objectP == NULL)
  {
    LOG_DEBUG("fun [%s] %s",__func__, nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  data = (firmware_data_t *)(objectP->userData);

  // this is a single instance object
  if((instanceId != 0) || (data == NULL))
  {
    return LWM2M_404_NOT_FOUND;
  }

  // for execute callback, resId is always set.
  switch(resourceId)
  {
    case RES_M_UPDATE:
      if(data->state == LWM2M_FIRWARE_STATE_DOWNLOADED)
      {
        LOG_DEBUG(" FIRMWARE UPDATE");
        // trigger your firmware update logic

        firmwareInstance->updateExecuted = LWM2M_TRUE;
        updateExecuted = LWM2M_TRUE;
		if (0 != (ret_val = prv_init_resources(&lwm2m_data, &id_set, LWM2M_FIRMWARE_UPDATE_OBJECT_ID, instanceId, RES_M_UPDATE)))
		  return ret_val;

        lwm2m_check_if_observe_and_update_app(id_set, 5, 0, RES_M_UPDATE, &lwm2m_data);
        lwm2m_free_instance_info(lwm2m_data.instance_info);
        return LWM2M_204_CHANGED;
      }
      else
      {
        // firmware update already running
        return LWM2M_405_METHOD_NOT_ALLOWED;
      }
    case RES_M_PACKAGE:
    case RES_M_PACKAGE_URI:
    case RES_M_STATE:
    case RES_M_UPDATE_RESULT:
    case RES_O_PKG_NAME:
    case RES_O_PKG_VERSION:
    case RES_O_UPDATE_PROTOCOL:
    case RES_M_UPDATE_DELIVERY_METHOD:
      return LWM2M_405_METHOD_NOT_ALLOWED;
    default:
      return LWM2M_404_NOT_FOUND;
  }
}

/**
 * @fn static uint8_t prv_firmware_discover()
 *
 * @brief This fucntion implements the discover operation performed on the
 *     firmware object
 *
 * @param instanceId Instance ID of the object to be read
 * @param numDataP number of resources in the array
 * @param dataArrayP resource value array
 * @param objectP Object to be read
 *
 * @return LWM2M response type
 *
 */
static uint8_t prv_firmware_discover(uint16_t instanceId,
    int * numDataP,
    lwm2m_data_t ** dataArrayP,
    lwm2m_object_t * objectP, lwm2m_server_t *serverP)
{
  uint8_t result = 0;
  int i = 0;
  int ri = 0;
  resource_instance_int_list_t* protocolList = NULL;
  firmware_data_t * data = NULL;

  if (numDataP == NULL || dataArrayP == NULL ) 
  {
    LOG_DEBUG("fun [%s] %s",__func__, nullError);  
    return  LWM2M_400_BAD_REQUEST;
  }
  // this is a single instance object
  if (instanceId != 0)
  {
    return LWM2M_404_NOT_FOUND;
  }

  result = LWM2M_205_CONTENT;
  data = (firmware_data_t*)(objectP->userData);
  for(protocolList = data->supported_protocol, ri = 0; protocolList != NULL;
      protocolList = protocolList->next, ri++);

  // is the server asking for the full object ?
  if (*numDataP == 0)
  {
    uint16_t resList[] = {

      RES_M_PACKAGE  ,
      RES_M_PACKAGE_URI ,               
      RES_M_UPDATE ,
      RES_M_STATE  ,
      RES_M_UPDATE_RESULT ,
      RES_O_PKG_NAME ,
      RES_O_PKG_VERSION ,
      RES_O_UPDATE_PROTOCOL,
      RES_M_UPDATE_DELIVERY_METHOD
    };
    int nbRes = sizeof(resList) / sizeof(uint16_t);

    *dataArrayP = lwm2m_data_new(nbRes);
    if (*dataArrayP == NULL) return LWM2M_500_INTERNAL_SERVER_ERROR;
    *numDataP = nbRes;
    for (i = 0; i < nbRes; i++)
    {
      (*dataArrayP)[i].id = resList[i];
      if((*dataArrayP)[i].id == RES_O_UPDATE_PROTOCOL )
      {
        (*dataArrayP)[i].type = LWM2M_TYPE_MULTIPLE_RESOURCE;
        (*dataArrayP)[i].value.asChildren.count = ri;
      }
    }
  }
  else
  {
    for(i = 0; i < *numDataP && result == LWM2M_205_CONTENT; i++)
    {
      switch((*dataArrayP)[i].id)
      {
        case RES_O_UPDATE_PROTOCOL:
        {
              (*dataArrayP)[i].type = LWM2M_TYPE_MULTIPLE_RESOURCE;
              (*dataArrayP)[i].value.asChildren.count = ri;
              break;
        }
        case RES_M_PACKAGE:
        case RES_M_PACKAGE_URI:
        case RES_M_UPDATE:
        case RES_M_STATE:
        case RES_M_UPDATE_RESULT:
        case RES_O_PKG_NAME:
        case RES_O_PKG_VERSION:
        case RES_M_UPDATE_DELIVERY_METHOD:
          break;
        default:
          result = LWM2M_404_NOT_FOUND;
      }
    }
  }

  return result;
}

/**
 * @fn static uint8_t prv_firmware_create()
 *
 * @brief This fucntion is used to create Firmware Object
 *
 * @param instanceId Instance ID of the object to be created
 * @param numData number of resources in the array
 * @param dataArray resource value array
 * @param objectP Object to be read
 *
 * @return LWM2M response type
 */

/***************************************************************************************************

 Under section 8.2.3 Operation on Object of OMA 1.1

- If the "Create" operation is permitted, the LwM2M Client MUST perform the instantiation on the 
Object only if all the mandatory Resources are present in the "New Value" parameter (see Section 5). 
If all the mandatory Resources are not present, the LwM2M Client MUST send a "Bad Request" error code 
to the LwM2M Server


*************************************************************************************************************/

static uint8_t prv_firmware_create(uint16_t instanceId,
    int numData,
    lwm2m_data_t * dataArray,
    lwm2m_object_t * objectP, bool apiCall)
{
  firmware_data_t *firmwareObj = NULL;
  uint8_t result = 0;
  uint16_t mandatory_resource[] = {RES_M_STATE, RES_M_UPDATE_RESULT, RES_M_UPDATE_DELIVERY_METHOD, RES_M_PACKAGE, RES_M_PACKAGE_URI};
  uint8_t size = sizeof(mandatory_resource)/sizeof(mandatory_resource[0]);
  if( objectP == NULL ) 
  {
    LOG_DEBUG("%s",nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  /*check if already one instance is created */
  if(objectP->instanceList != NULL)
  {
    ds_iot_ulog_err("Firmware object instance exist");
    return LWM2M_400_BAD_REQUEST;
  }
  if(apiCall == TRUE)
  {
     goto skip_check;
  }

  result = prv_mandatory_check(numData, dataArray, mandatory_resource, size);
  if(result != LWM2M_NO_ERROR)
    return result;
  result = prv_firmware_create_write_check(numData, dataArray, TRUE);
  if (result != LWM2M_204_CHANGED )
  {
    ds_iot_ulog_err("Creating instance failed ");
    return result;
  }
skip_check:  
  firmwareObj = (firmware_data_t*) lwm2m_calloc(1, sizeof(firmware_data_t));
  if( NULL == firmwareObj)
  {
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  //TODO: Do  I need to call load_firmware_persistent_info() function?
  firmwareObj->state      = LWM2M_FIRWARE_STATE_IDLE;
  firmwareObj->supported  = false;
  firmwareObj->result     = LWM2M_FIRWARE_UPDATE_DEFAULT;

  firmwareObj->instanceId = instanceId;
  objectP->instanceList = (lwm2m_list_t *)lwm2m_malloc(sizeof(lwm2m_list_t));
  if (NULL != objectP->instanceList)
  {
    memset(objectP->instanceList, 0x00, sizeof(lwm2m_list_t));
  }
  objectP->userData = firmwareObj;
  firmwareInstance = objectP->userData;

  /* We instansiated the FW object */
  if(dataArray == NULL)
  {
    goto end;
  }
  result = prv_firmware_write_resources(objectP, numData, dataArray, 
                                          INVALID_RES_INST_ID, QAPI_COAP_POST);
  if (result != LWM2M_204_CHANGED)
  {
    // Issue in creating object instanace so retuning Bad request error.
    result = LWM2M_400_BAD_REQUEST;
    lwm2m_free(objectP->instanceList);
    objectP->instanceList = NULL;
    lwm2m_free(firmwareObj);
    objectP->userData = NULL;
    return result;
  }

end:
  efs_unlink(LWM2M_FIRMWARE_OBJ_PERSISTENCE_PATH);
  return LWM2M_201_CREATED;
}

/**
 * @fn static int32_t prv_firmware_getvalue
 * @brief   This function is used to set the particular resource value from application
 * @param   lwm2m_data - pointer to lwm2m object information
           targetP    - pointer to lwm2m object 
 * @return  on success - 1
           on error   -	0	   
 */

static int32_t prv_firmware_getvalue(lwm2m_id_info_t id_info, 
                                      lwm2m_object_data_t **lwm2m_dataP , 
                                      lwm2m_object_t * targetP, 
                                      lwm2m_resource_info_t **resource_info)
{
  int8_t result = -1;
  lwm2m_data_t * dataArrayP = NULL;
  //lwm2m_resource_info_t *resource_info = NULL;
  int numData = 0;
  uint16_t instance_id = 0;

  if(lwm2m_dataP == NULL)
    return -1;

  if((result = prv_init_getvalue(id_info,*lwm2m_dataP, resource_info, &dataArrayP, &numData))==-1)
  	return result;

  instance_id = id_info.instance_ID;

  result = prv_firmware_read(instance_id, &numData, &dataArrayP, targetP,INVALID_RES_INST_ID, NULL);
  /*result = prv_firmware_read(instance_id, &numData, &dataArrayP, targetP,INVALID_RES_INST_ID, NULL);
	Cant use prv_firmware_read since no getvalue on executable resource RES_O_UPDATE_PROTOCOL
  */
#if 0
  instanceP = (firmware_data_t *)targetP->userData;
  if (NULL == instanceP)
    return result;
  
  if (numData == 0)
  {
    uint16_t resList[] = {
      RES_M_PACKAGE_URI,
      RES_M_STATE  ,
      RES_M_UPDATE_RESULT ,
      RES_O_PKG_NAME ,
      RES_O_PKG_VERSION ,
      RES_M_UPDATE_DELIVERY_METHOD
    };    

    int nbRes = sizeof(resList)/sizeof(uint16_t);

    dataArrayP = lwm2m_data_new(nbRes);
    if (dataArrayP == NULL)
      return result;
    
    numData = nbRes;

    for (i = 0; i < nbRes; i++)
      (dataArrayP)[i].id = resList[i];
  }

  i = 0;
  do
  {
    result = prv_set_value((dataArrayP) + i, instanceP);
    i++;
  } while (i < numData && result == LWM2M_205_CONTENT);
#endif

  if (result != LWM2M_205_CONTENT)
  {
    result = -1;
    goto GET_ERROR;
  }
  if((result = prv_getvalue_resource_populate(lwm2m_dataP, dataArrayP, resource_info, numData))==-1)
	  goto GET_ERROR;

  result = 0;

GET_ERROR:
  /* Resources allocated for lwm2m_dataP are cleaned-up by the caller */
  if (dataArrayP)
    lwm2m_free(dataArrayP);
  
  return result;
}


/**
 * @fn static uint8_t prv_firmware_delete()
 *
 * @brief This fucntion is used to delete Firmware object
 *
 * @param id Instance ID of the object to be deleted
 * @param objectP Object to be deleted
 *
 * @return LWM2M response type
 */
static uint8_t prv_firmware_delete(uint16_t id,
    lwm2m_object_t * objectP,
    bool persist_file)
{
  int fd = 0;
  firmware_data_t * firmwareObj = NULL;

  if( objectP == NULL ) 
  {
    LOG_DEBUG("%s",nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  objectP->instanceList = lwm2m_list_remove(objectP->instanceList, id, (lwm2m_list_t **)&firmwareObj);
  if (NULL == firmwareObj) return LWM2M_404_NOT_FOUND;

  /*  Update single instance object persistence */

  if (NULL != objectP->userData)
  {
    lwm2m_free(objectP->userData);
    objectP->userData = NULL;
  }

  lwm2m_free(firmwareObj);
  firmwareObj = NULL;

  if(persist_file)
  {
    ds_iot_log_strf_med("EFS_OPT: File open %s",LWM2M_FIRMWARE_OBJ_PERSISTENCE_PATH);
  	fd = efs_open( LWM2M_FIRMWARE_OBJ_PERSISTENCE_PATH, O_CREAT, 00744);
  
	if( EFAILURE == fd)
	{
	  LOG_DEBUG("Failed to open single_instance_obj_pr");
	  return LWM2M_500_INTERNAL_SERVER_ERROR;
	}
	efs_close(fd);
  }
  
  return LWM2M_202_DELETED;

}

/**
 * @fn lwm2m_object_t * get_object_firmware()
 *
 * @brief This function is used to get the information regarding the Firmware
 *        object during client registartion
 *
 * @param void
 *
 * @return Firmware object 
 *
 */
lwm2m_object_t * get_object_firmware(void)
{
  /*
   * The get_object_firmware function create the object itself and return a pointer to the structure that represent it.
   */
  lwm2m_object_t * firmwareObj = NULL;
  int fd = 0;
  firmwareObj = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));

  if (NULL != firmwareObj)
  {
    memset(firmwareObj, 0, sizeof(lwm2m_object_t));

    /*
     * It assigns its unique ID
     * The 5 is the standard ID for the optional object "Object firmware".
     */
    firmwareObj->objID = LWM2M_FIRMWARE_UPDATE_OBJECT_ID;

    /*
     * And the private function that will access the object.
     * Those function will be called when a read/write/execute query is made by the server. In fact the library don't need to
     * know the resources of the object, only the server does.
     */
    firmwareObj->readFunc    = prv_firmware_read;
    firmwareObj->writeFunc   = prv_firmware_write;
    firmwareObj->executeFunc = prv_firmware_execute;
    firmwareObj->discoverFunc= prv_firmware_discover;
    firmwareObj->createFunc  = prv_firmware_create;
    firmwareObj->deleteFunc  = prv_firmware_delete;
    firmwareObj->setValueFunc = prv_firmware_setvalue;
    firmwareObj->get_Value_Func = prv_firmware_getvalue;
    
    /*  Check if firmware_obj_single_instance_pr file exist */
    ds_iot_log_strf_med("EFS_OPT: File open %s",LWM2M_FIRMWARE_OBJ_PERSISTENCE_PATH);
    fd = efs_open(LWM2M_FIRMWARE_OBJ_PERSISTENCE_PATH, O_RDONLY);
    if(fd != EFAILURE)
    {
      efs_close(fd);
      return firmwareObj;
    }
    /*
     * and its unique instance
     *
     */
    firmwareObj->instanceList = (lwm2m_list_t *)lwm2m_malloc(sizeof(lwm2m_list_t));
    if (NULL != firmwareObj->instanceList)
    {
      memset(firmwareObj->instanceList, 0, sizeof(lwm2m_list_t));
    }
    else
    {
      lwm2m_free(firmwareObj);
      return NULL;
    }


    firmwareObj->userData    = lwm2m_calloc(1, sizeof(firmware_data_t));

    /*
     * Also some user data can be stored in the object with a private structure containing the needed variables
     */
    if (NULL != firmwareObj->userData)
    {
      firmwareInstance = firmwareObj->userData;

      ((firmware_data_t*)firmwareObj->userData)->state      = LWM2M_FIRWARE_STATE_IDLE;
      ((firmware_data_t*)firmwareObj->userData)->supported  = false;
      ((firmware_data_t*)firmwareObj->userData)->result     = LWM2M_FIRWARE_UPDATE_DEFAULT;

	  load_firmware_persistent_info(firmwareInstance);

    }
    else
    {
      lwm2m_free(firmwareObj);
      firmwareObj = NULL;
    }
  }

  return firmwareObj;
}

/**
 * @fn void free_object_firmware()
 *
 * @brief This fucntion frees the Firmware object allocated
 * @param ObjectP Object to free
 *
 * @return void
 */
void free_object_firmware(lwm2m_object_t * objectP)
{
{
  lwm2m_list_t* objlist;

  if (objectP == NULL) return;

  objlist = objectP->instanceList;
  while(objlist)
  {
    prv_firmware_delete(objlist->id,objectP,false);
    objlist = objectP->instanceList;
  }

    lwm2m_free(objectP);
}}

#endif

