/******************************************************************************

  @file    object_devicecap.c
  @brief   File contains the implementation for the operations related to object 
           Device capability management
  ---------------------------------------------------------------------------

  Copyright (c) 2016-2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------

 *****************************************************************************/

/*
 *  Object                         |      | Multiple  |     | Description                   |
 *  Name                           |  ID  | Instances |Mand.|                               |
 *---------------------------------+------+-----------+-----+-------------------------------+
 *  Device Capability Management   |  15  |    Yes    |  No |                               |
 *
 *  Resources:
 *  Name        | ID  | Oper.|Instances|Mand.|  Type   | Range | Units | Description                                                                      |
 * -------------+-----+------+---------+-----+---------+-------+-------+----------------------------------------------------------------------------------+
 *  Property    |  0  |  R   | Single  | Yes | String  |       |       | List of Device Capabilites inside a given group. The format is a free                                                                          list ASCII - represented integers seperated by semi-colon e.g - 0;1;10
 *  Group       |  1  |  R   | Single  | Yes | Integer |       |       | Group name of device Capabilites 0-SENSOR, 1-CONTROL, 2-CONNECTIVITY,                                                                          3-NAVIGATION, 4-STORAGE, 5-VISION, 6-SOUND, 7-ANALOG_INPUT, 
 8-ANALOG_OUTPUT.
 *  Description |  2  |  R   | Single  | No  | String  |       |       | Device capability description.                                        
 *  Attached    |  3  |  R   | Single  | No  | Boolean |       |       | When the resource doesn’t exist, it means the associated Device 
 Capability is not removable. When this resource is “False”, it means 
 the associated Device Capability is removable and is currently not 
 attached to the device. When this resource is “True”, it means the 
 associated Device Capability – if removable – is currently attached 
 to the Device. When a Device Capability is not removable, and the 
 “Attached” Resource is present, the “Attached” value but be set 
 to “True”.                                                            
 *  Enabled     |  4  |  R   | Single  | Yes | Boolean |       |       | This resource indicates whether the Device Capability is enabled 
 regardless whether the Device Capability is attached or not. If the 
 value of this resource is “True” the Device Capability is in Enabled 
 State. If the value is “False” the Device Capability is in Disabled 
 State; The ‘Attached’ property is independent of ‘Enabled’ property. 
 A Device Capability MAY have ‘True’ as value for ‘Enabled’ node 
 while having ‘False’ as value for the ‘Attached’ node. That means the 
 Device Capability is still not available and can’t be used until it 
 is attached to the Device, but will be useable once the Device 
 Capability is attached.                                               
 *  opEnable   |  5  |  E   | Single  | Yes |          |       |       | This command is used to enable the Device Capability to transfer 
 the  Device Capability from Disabled State to Enabled state. 
 In Enabled State, the Device Capability is allowed to work when it 
 is attached to the Device.
 * opDisable   |  6  |  E   | Single  | Yes |          |       |       | This command is used to disable the Device Capability to transfer 
 the Device Capability from Enabled State to Disabled State. 
 In Disabled state the Device Capability is not allowed to work.  
 * NotifyEn    |  7  |  RW  | Single  | No  | Boolean  |       |       | This resource specifies whether a LWM2M Server may be notified when 
 the “Attached” Resource is under “Observation” and the value is 
 changing. If the resource is not present or the value is ‘False’, 
 the LWM2M Server will be not notified about this change. If this 
 Resource  is present and the value is ‘True’,the LWM2M Server will 
 */

#include "liblwm2m.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "ds_log.h"


#ifdef ENABLE_DEVCAP_OBJ
static bool create_operation = LWM2M_FALSE;

#define RES_M_PROPERTY    0 /**< Resource ID for resource "property". */
#define RES_M_GROUP     1 /**< Resource ID for resource "group". */
#define RES_O_DESCRIPTION 2 /**< Resource ID for resource "description". */
#define RES_O_ATTACHED    3 /**< Resource ID for resource "attached". */
#define RES_M_ENABLED   4 /**< Resource ID for resource "enabled". */
#define RES_M_OP_ENABLE   5 /**< Resource ID for resource "op enable". */
#define RES_M_OP_DISBALE  6 /**< Resource ID for resource "op disable". */
#define RES_O_NOTIFY_EN   7 /**< Resource ID for resource "notify en". */

#define PRV_PROPERTY_MAXLEN   128 /**< "property" resource length. */
#define PRV_DESCRIPTION_MAXLEN  255 /**< "description" resource length. */

void free_object_devicecap(lwm2m_object_t * objectP);

/**
 * @brief Describes the Device capability management object resources
 *
 */
typedef struct _device_cap_manangement_t
{
  struct _device_cap_manangement_t *next;  /**< corresponding to lwm2m_list_t::next */
  uint16_t    id;                 /**< corresponding to lwm2m_list_t::id  */
  char property[PRV_PROPERTY_MAXLEN];  /**< Device capability list (Mandatory) */
  int8_t group; /**< Group name of device capability (Mandatory) */
  char description[PRV_DESCRIPTION_MAXLEN]; /**< Device capability
                                              description (Optional) */
  bool attached; /**< Device capability is removeable or not (optional) */
  bool enabled; /**< Device capability is enabled or not (Mandatory) */
  bool notifyen; /**< notification is enabled or nor (optional) */
} device_cap_manangement_t;

typedef struct _device_cap_manangement_t_persistent_instance_s
{
  uint16_t    id;                 /**< corresponding to lwm2m_list_t::id  */
  char property[PRV_PROPERTY_MAXLEN];  /**< Device capability list (Mandatory) */
  int8_t group; /**< Group name of device capability (Mandatory) */
  char description[PRV_DESCRIPTION_MAXLEN]; /**< Device capability
                                              description (Optional) */
  bool attached; /**< Device capability is removeable or not (optional) */
  bool enabled; /**< Device capability is enabled or not (Mandatory) */
  bool notifyen; /**< notification is enabled or nor (optional) */
  bool isValid;
  
} device_cap_manangement_persistent_instance_t;


/**
 * @fn uint8_t store_device_cap_manangement_persistent_info()
 * @brief This function is used to store the device_cap_manangement profile into persistent storage
 * 
 * @param targetP pointer to device_cap_manangement object instance
 * @param store   to check whether instance stored is valid or not.
 * 
 * @return return error code
 */
 
int store_device_cap_manangement_persistent_info(device_cap_manangement_t * targetP, bool store)
{
  int fd = 0, ret = 0, index = 0;
  device_cap_manangement_persistent_instance_t *info = NULL;
  device_cap_manangement_persistent_instance_t *conn_ctx = NULL;

  if(NULL == targetP)
  {
    LOG_DEBUG(" %s",nullError);
    return EFAILURE;
  }

  info = (device_cap_manangement_persistent_instance_t *)malloc(sizeof(device_cap_manangement_persistent_instance_t));
  if(info == NULL)
  {
    return EFAILURE;
  }
  conn_ctx = (device_cap_manangement_persistent_instance_t *)malloc(sizeof(device_cap_manangement_persistent_instance_t));
  if(conn_ctx == NULL)
  {
    free(info);
    return EFAILURE;
  }

  /* open apn connection info persistent file.If not already present, create it*/
  ds_iot_log_strf_med("EFS_OPT: File open %s",LWM2M_DEVICE_CAP_MGNT_PERSISTENCE_FILE);
  fd = efs_open(LWM2M_DEVICE_CAP_MGNT_PERSISTENCE_FILE, O_CREAT | O_RDWR, 00644);
  
  if (EFAILURE == fd)
  {
    ds_iot_ulog_err(" failed to open apn conn persistence file");
    ret = EFAILURE;
    goto end;
  }

  while (1)
  {
    memset(info, 0x00, sizeof(device_cap_manangement_persistent_instance_t));
    ret = efs_read(fd, info, sizeof(device_cap_manangement_persistent_instance_t));

    if (ret <= 0)
    {
      break;
    }
    if(info->id == targetP->id)
    {
	  break;
    }
    index++;
  }

  memset(conn_ctx, 0x00, sizeof(device_cap_manangement_persistent_instance_t));

  LWM2M_STRCPY(conn_ctx->property, targetP->property, strlen(targetP->property) +1);
  LWM2M_STRCPY(conn_ctx->description, targetP->description, strlen(targetP->description) +1);
  conn_ctx->group = targetP->group;
  conn_ctx->attached = targetP->attached;
  conn_ctx->enabled = targetP->enabled;
  conn_ctx->notifyen = targetP->notifyen;
  conn_ctx->id = targetP->id;

  if(store)
  {
    conn_ctx->isValid = LWM2M_TRUE; 
  }
  else
  {
    conn_ctx->isValid = LWM2M_FALSE; 
  }

  /* lseek to reqd index and write the apn_persist_info structure in efs */
  efs_lseek(fd, index * sizeof(device_cap_manangement_persistent_instance_t), SEEK_SET);
  ds_iot_log_strf_med("EFS_OPT: File write %s",LWM2M_DEVICE_CAP_MGNT_PERSISTENCE_FILE);
  if (EFAILURE == efs_write(fd, conn_ctx, sizeof(device_cap_manangement_persistent_instance_t)))
  {
    ds_iot_ulog_err(" failed to store persistent value of apn conn instance");
    efs_close(fd);
    ret = EFAILURE;
    goto end;
  }

  efs_close(fd);
  ret = ESUCCESS;

end:
  free(info);
  free(conn_ctx);
  return ret;
}

/**
 * @fn uint8_t load_device_cap_manangement_persistent_info()
 * @brief This function is used to fetch the device_cap_manangement profile from persistent storage
 * 
 * @param objectP pointer to device_cap_manangement object
 * 
 * @return return error code
 */
int 
load_device_cap_manangement_persistent_info(lwm2m_object_t * objectP)
{
  int fd = 0, ret = 0;

  if (NULL == objectP)
  {
    return EFAILURE;
  }
  ds_iot_log_strf_med("EFS_OPT: File open %s",LWM2M_DEVICE_CAP_MGNT_PERSISTENCE_FILE);
  fd = efs_open(LWM2M_DEVICE_CAP_MGNT_PERSISTENCE_FILE, O_RDONLY);

  if (EFAILURE == fd)
  {
    ds_iot_ulog_err(" failed to open apn conn persistent storage");
    return EFAILURE;
  }

  while(1)
  {
    device_cap_manangement_persistent_instance_t info;
    device_cap_manangement_t * instanceP = NULL;

    memset(&info, 0, sizeof(device_cap_manangement_persistent_instance_t));
    ret = efs_read(fd, &info, sizeof(device_cap_manangement_persistent_instance_t));
	
    if (EFAILURE == ret || 0 == ret)
      break;

    /*Check if the structure entry in file is valid*/
    if(LWM2M_FALSE == info.isValid)
    {
      continue;
    }

    instanceP = (device_cap_manangement_t *)lwm2m_malloc(sizeof(device_cap_manangement_t));
    if(instanceP == NULL)
    {
      ret = -1;
      break;
    }

    memset(instanceP, 0, sizeof(device_cap_manangement_t));
  
    LWM2M_STRCPY(instanceP->property, info.property, strlen(info.property) +1);
    LWM2M_STRCPY(instanceP->description, info.description, strlen(info.description) +1);
    instanceP->group = info.group;
    instanceP->attached = info.attached;
    instanceP->enabled =  info.enabled;
    instanceP->notifyen = info.notifyen;
	instanceP->id = info.id;

    objectP->instanceList = LWM2M_LIST_ADD(objectP->instanceList, instanceP);
  }
  if (ret < 0)
  {
    efs_close(fd);
    free_object_devicecap(objectP);
    objectP = NULL;
    return EFAILURE;
  }
  efs_close(fd);
  return ESUCCESS;
}

/** 
* @fn static uint8_t prv_devicecap_create_write_check() 
*
* @brief This fucntion verifies and write the resource's into  
*        Divicecap object 
*
* @param instanceId Instance of the Divicecap object to be create/write 
* @param numDataP number of resources in the array 
* @param dataArrayP resource value array 
* @param create_flag - if true creating instance else write on instance 
*
* @return LWM2M response type 
*
*/
/***************************************************************************************************

Under section 8.2.2 Operation on Object Instance of OMA 1.1

- For the �Write� operation on an Object Instance, the LwM2M Client MUST perform the operation, 
if all the Resources conveyed in the operation are allowed to perform the �Write� operation. 
If any Resource does not support the �Write� operation, the LwM2M Client MUST inform the LwM2M 
Server that the Object Instance cannot perform the requested �Write� operation in sending a 
�Operation is not supported� error code.

*************************************************************************************************************/  
static uint8_t prv_devicecap_create_write_check(int numData,
                                               lwm2m_data_t * dataArray,
                                               bool create_flag)
{ 
  int16_t i = 0;
  uint8_t result = LWM2M_NO_ERROR;
  if((NULL == dataArray))
  { 
    LOG_DEBUG("%s",nullError); 
    return LWM2M_500_INTERNAL_SERVER_ERROR; 
  }
  i = 0;
   
  do
  {
    switch(dataArray[i].id)
    {

	   
      case RES_M_OP_ENABLE:
      case RES_M_OP_DISBALE:	
      case RES_M_PROPERTY:
      case RES_M_GROUP:
      case RES_O_DESCRIPTION:
      case RES_O_ATTACHED:
      case RES_M_ENABLED:
         if (!create_flag)
         {
           result = LWM2M_405_METHOD_NOT_ALLOWED;
           break;
         }
      case RES_O_NOTIFY_EN:
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
 * @brief This function is used to write the resources into object instance
 * while creating the object instance. Note: not to be called to perform write
 * operation
 * @param   instanceP (INOUT)   handle to object instance
 * @param   numData (IN)        number of resources to be written
 * @param   dataArray (IN)      array of resource values
 * @return  LWM2M_NO_ERROR on success
 */
static uint8_t prv_devicecap_write_resources(device_cap_manangement_t *instanceP,
    int numData,
    lwm2m_data_t *dataArray, bool *val_change)
{
  int i = 0;
  
  if( instanceP == NULL  || dataArray == NULL || val_change == NULL)
  {
    LOG_DEBUG("fun [%s] %s",__func__,nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  for (i = 0; i < numData; i++)
  {
    ds_iot_ulog_high1 ("IN devicecap dataArray -----> %p ",dataArray);
    switch((dataArray + i)->id)
    {
      case RES_M_PROPERTY:
        if(lwm2m_strcmp(instanceP->property, (char *)(dataArray + i)->value.asBuffer.buffer) != 0)
        {
          *val_change = true;
        }
        LWM2M_STRCPY(instanceP->property, (const char *)((dataArray + i)->value.asBuffer.buffer),
            ((dataArray + i)->value.asBuffer.length) + 1);
        break;

      case RES_M_GROUP:
        {
          int64_t value = 0;
          if ((1 != lwm2m_data_decode_int(dataArray+i, (int64_t*)&value)) && (value>=0 && value<=15) )
          {
            return LWM2M_400_BAD_REQUEST;
          }
          if(instanceP->group != value)
          {
            *val_change = true;
          }
          instanceP->group= (int8_t)value;
        }
        break;

      case RES_M_ENABLED:
        {
          bool value = instanceP->enabled;
        if(1 != lwm2m_data_decode_bool(dataArray+i, &instanceP->enabled))
        {
          return LWM2M_400_BAD_REQUEST;
        }
          if(instanceP->enabled != value)
          {
            *val_change = true;
          }
        }
        break;

      case RES_O_DESCRIPTION:
        {
          if(lwm2m_strcmp(instanceP->description, (char *)(dataArray + i)->value.asBuffer.buffer) != 0)
          {
            *val_change = true;
          }
          LWM2M_STRCPY(instanceP->description, (const char *)((dataArray + i)->value.asBuffer.buffer),
              ((dataArray + i)->value.asBuffer.length) + 1);
        }
        break;

      case RES_O_ATTACHED:
      {
        bool value = instanceP->attached;
        if(1 != lwm2m_data_decode_bool(dataArray+i, &instanceP->attached))
        {
          return LWM2M_400_BAD_REQUEST;
        }
        if(instanceP->attached != value)
        {
          *val_change = true;
        }
        break;
      }

      case RES_O_NOTIFY_EN:
      {
        bool value = instanceP->notifyen;
        if(1 != lwm2m_data_decode_bool(dataArray+i, &instanceP->notifyen))
        {
          return LWM2M_400_BAD_REQUEST;
        }
        if(instanceP->notifyen != value)
        {
          *val_change = true;
        }
        break;
      }

      default:
	  	//Ignoring any optional resource
       break;
    }
  }

  if(create_operation == TRUE)
  {
    store_device_cap_manangement_persistent_info(instanceP, LWM2M_TRUE);
  }
  return LWM2M_204_CHANGED;
}

/**
 * @fn static uint8_t prv_devicecap_create()
 * @brief   This function implements the create operation performed
 * on the Device Capability object
 * @param   instanceId (IN) Instance ID of the object instance to be created
 * @param   numData (IN)    number of resources in the array i.e. dataArray
 * @param   dataArray (IN)  resource value array
 * @param   objectP (IN)    Object whose instance is to be created
 * return   COAP response type
 */

/***************************************************************************************************

 Under section 8.2.3 Operation on Object of OMA 1.1

- If the �Create� operation is permitted, the LwM2M Client MUST perform the instantiation on the 
Object only if all the mandatory Resources are present in the �New Value� parameter (see Section 5). 
If all the mandatory Resources are not present, the LwM2M Client MUST send a �Bad Request� error code 
to the LwM2M Server


*************************************************************************************************************/

static uint8_t prv_devicecap_create(uint16_t instanceId,
    int numData,
    lwm2m_data_t* dataArray,
    lwm2m_object_t* objectP, bool apiCall)
{
  device_cap_manangement_t *instanceP = NULL;
  uint8_t result = LWM2M_500_INTERNAL_SERVER_ERROR;
  bool val_change = false;
  uint16_t mandatory_resource[] = {RES_M_PROPERTY, RES_M_GROUP, RES_M_ENABLED};
  uint8_t size = sizeof(mandatory_resource)/sizeof(mandatory_resource[0]);
  if( dataArray == NULL || objectP == NULL ) 
  {
    LOG_DEBUG("fun [%s] %s",__func__,nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }
  if(apiCall == TRUE)
  {
     goto skip_check;
  }
  result = prv_mandatory_check(numData, dataArray, mandatory_resource, size);
  if(result != LWM2M_NO_ERROR)
		return result;
  /* Check all resources to be written are writable or not */
  result = prv_devicecap_create_write_check(numData, dataArray, TRUE);
  if (result != LWM2M_204_CHANGED )
  {
     ds_iot_ulog_err("Creating instance failed ");
     return result;
  }

skip_check:
  instanceP = (device_cap_manangement_t *)lwm2m_malloc(sizeof(device_cap_manangement_t));

  if (instanceP != NULL)
  {
    memset(instanceP, 0, sizeof(device_cap_manangement_t));
    instanceP->id = instanceId;
    create_operation = TRUE;
    result = prv_devicecap_write_resources(instanceP, numData, dataArray, &val_change);
    create_operation = FALSE;
    if (result == LWM2M_204_CHANGED)
    {
      objectP->instanceList = LWM2M_LIST_ADD(objectP->instanceList, instanceP);
      result = LWM2M_201_CREATED;
    }
    else
    {
      lwm2m_free(instanceP);
      // Issue in creating object instanace so retuning Bad request error.
      result = LWM2M_400_BAD_REQUEST;
    }
  }
  return result;

}

/**
 * @fn static uint8_t prv_set_value()
 * @brief This fucntion is used to populate the values of the resource
 * to be provided to the server during the read operation on the object.
 * @param dataP (OUT) data array for the resource 
 * @param devDataP (IN) device capability object data
 * @return LWM2M response type
 */
static uint8_t prv_set_value(lwm2m_data_t * dataP,
    device_cap_manangement_t * devDataP)
{

  if( dataP == NULL || devDataP == NULL ) 
  {
    LOG_DEBUG("fun [%s] file [%s]  %s",__func__,__FILE__,nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  } 

  switch (dataP->id)
  {
    case RES_M_PROPERTY:
      lwm2m_data_encode_string(devDataP->property, dataP);
      return LWM2M_205_CONTENT;

    case RES_M_GROUP:
      lwm2m_data_encode_int(devDataP->group, dataP);
      return LWM2M_205_CONTENT;

    case RES_O_DESCRIPTION:
      lwm2m_data_encode_string(devDataP->description, dataP);
      return LWM2M_205_CONTENT;

    case RES_O_ATTACHED:
      lwm2m_data_encode_bool(devDataP->attached, dataP);
      return LWM2M_205_CONTENT;

    case RES_M_ENABLED:
      lwm2m_data_encode_bool(devDataP->enabled, dataP);
      return LWM2M_205_CONTENT;

    case RES_O_NOTIFY_EN:
	  ds_iot_ulog_high1("Device_cap Notify_EN value : %d",devDataP->notifyen);
      lwm2m_data_encode_bool(devDataP->notifyen, dataP);
      return LWM2M_205_CONTENT;

    case RES_M_OP_DISBALE:
    case RES_M_OP_ENABLE:
      return LWM2M_405_METHOD_NOT_ALLOWED;
    default:
      return LWM2M_404_NOT_FOUND;
  }
}

/**
 * @brief This function implements the read operation performed on device
 * capability management object
 * @param instanceId (IN) Instance ID of the object to be read
 * @param numDataP (OUT) number of resources in the array
 * @param dataArrayP (OUT) resource value array
 * @param objectP (IN) Object to be read
 * @return LWM2M response type
 */ 
static uint8_t prv_devicecap_read(uint16_t instanceId,
    int * numDataP,
    lwm2m_data_t ** dataArrayP,
    lwm2m_object_t * objectP,
    uint16_t resInstId, lwm2m_server_t *serverP)
{
  uint8_t result = LWM2M_404_NOT_FOUND;
  int i = 0;
  device_cap_manangement_t *instanceP = NULL;

  if( numDataP == NULL || dataArrayP == NULL || objectP == NULL ) 
  {
    ds_iot_ulog_err("fun [prv_devicecap_read] LWM2M_LOG: NULL Error");
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  instanceP = (device_cap_manangement_t *)LWM2M_LIST_FIND(objectP->instanceList, 
      instanceId);
  if (NULL == instanceP)
  {
    return result;
  }

  if(*numDataP == 0)
  {
    uint16_t resList[] = {
      RES_M_PROPERTY,
      RES_M_GROUP,
      RES_O_DESCRIPTION,
      RES_O_ATTACHED,
      RES_M_ENABLED,
      RES_O_NOTIFY_EN
    };

    int nbRes = sizeof(resList)/sizeof(uint16_t);

    *dataArrayP = lwm2m_data_new(nbRes);
    if (*dataArrayP == NULL)
    {
      return LWM2M_500_INTERNAL_SERVER_ERROR;
    }

    *numDataP = nbRes;
    for (i = 0 ; i < nbRes ; i++)
    {
      (*dataArrayP)[i].id = resList[i];
    }
  }

  i = 0;
  do
  {
	ds_iot_ulog_high1("Device Cap READ ON  Resource ID %d ",((*dataArrayP)+i)->id);  
    result = prv_set_value((*dataArrayP)+i, instanceP);
    i++;
  }while (i < *numDataP && result == LWM2M_205_CONTENT);

  return result;
}

/**
 * @brief This fucntion implements the discover operation performed on the 
 *        Device capability management object
 * @param instanceId (IN) Instance ID of the object to be read
 * @param numDataP (OUT) number of resources in the array
 * @param dataArrayP (OUT) resource value array
 * @param objectP (IN) Object to be read
 * @return LWM2M response type
 */ 
static uint8_t prv_devicecap_discover(uint16_t instanceId,
    int * numDataP,
    lwm2m_data_t ** dataArrayP,
    lwm2m_object_t * objectP, lwm2m_server_t *serverP)
{

  uint8_t result = LWM2M_205_CONTENT;
  int i = 0;
  if( numDataP == NULL || dataArrayP == NULL || objectP == NULL ) 
  {
    LOG_DEBUG("fun [%s] %s",__func__, nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  if(*numDataP == 0)
  {
    uint16_t resList[] = {
      RES_M_PROPERTY,
      RES_M_GROUP,
      RES_O_DESCRIPTION,
      RES_O_ATTACHED,
      RES_M_ENABLED,
      RES_M_OP_ENABLE,
      RES_M_OP_DISBALE,
      RES_O_NOTIFY_EN
    };
    int nbRes = sizeof(resList) / sizeof(uint16_t);

    *dataArrayP = lwm2m_data_new(nbRes);
    if (*dataArrayP == NULL)
    {
      return LWM2M_500_INTERNAL_SERVER_ERROR;
    }

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
        case RES_M_PROPERTY:
        case RES_M_GROUP:
        case RES_O_DESCRIPTION:
        case RES_O_ATTACHED:
        case RES_M_ENABLED:
        case RES_M_OP_ENABLE:
        case RES_M_OP_DISBALE:
        case RES_O_NOTIFY_EN:
          break;
        default:
          result = LWM2M_404_NOT_FOUND;
      }
    }
  }

  return result;
}

/**
 * @brief This fucntion implements the write operation performed on Device
 * capability management object.
 * @param instanceId (IN) Instance ID of the object to be read
 * @param numDataP (OUT) number of resources in the array
 * @param dataArrayP (OUT) resource value array
 * @param objectP (IN) Object to be read
 * @return LWM2M response type
 */ 
static uint8_t prv_devicecap_write(uint16_t instanceId,
    int numData,
    lwm2m_data_t * dataArray,
    lwm2m_object_t * objectP,
    uint16_t resInstId, uint8_t write_method, bool bs_true)
{

  int i = 0;
  uint8_t result = LWM2M_404_NOT_FOUND;
  device_cap_manangement_t *instanceP = NULL;
  lwm2m_object_data_t lwm2m_data;
  uint8_t id_set = 0;
  bool val_change = false;

  /**< No instance id check as its a multiple instance object */
  if( dataArray == NULL || objectP == NULL ) 
  {
    ds_iot_ulog_err("fun [prv_devicecap_write] LWM2M_LOG: NULL Error");
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }
  result = prv_devicecap_create_write_check(numData, dataArray, bs_true);
  if (result != LWM2M_204_CHANGED )
  {
    ds_iot_ulog_err ("Write operation failed due to write on non-writable resource. ");
    return result;
  }

  instanceP = (device_cap_manangement_t *)LWM2M_LIST_FIND(objectP->instanceList, 
      instanceId);
  if (NULL == instanceP)
  {
    return result;
  }

  do
  {
   switch(dataArray[i].id) {
    case RES_O_NOTIFY_EN:
    {
	  if(0 != (result = prv_write_init_resources(&lwm2m_data, &id_set, dataArray+i, LWM2M_DEVICE_CAP_OBJECT_ID, instanceId, QAPI_NET_LWM2M_TYPE_BOOLEAN_E)))
		return result;

      if(instanceP->notifyen != lwm2m_data.instance_info->resource_info->value.asBoolean)
      {
        val_change = TRUE;
      }
      instanceP->notifyen = lwm2m_data.instance_info->resource_info->value.asBoolean;
      lwm2m_check_if_observe_and_update_app(id_set, LWM2M_DEVICE_CAP_OBJECT_ID, instanceId, RES_O_NOTIFY_EN, &lwm2m_data);
      lwm2m_free_instance_info(lwm2m_data.instance_info);
      
      result = LWM2M_204_CHANGED;
    }
    break;
    case RES_M_PROPERTY:
    case RES_M_GROUP:
    case RES_O_DESCRIPTION:
    case RES_O_ATTACHED:
    case RES_M_ENABLED:
    case RES_M_OP_ENABLE:
    case RES_M_OP_DISBALE:
      return LWM2M_405_METHOD_NOT_ALLOWED;
   }
   i++;
  } while (i < numData); //&& result == LWM2M_204_CHANGED); commented to fix KW

  if (result == LWM2M_204_CHANGED && val_change == TRUE)
	   store_device_cap_manangement_persistent_info(instanceP, LWM2M_TRUE);

  return result;

}

/**
 * @brief This fucntion is used to change the value of resource "enabled" based
 * on the execute operation performed on resource "op_enable" or "op_disable"
 * @param objectP (IN/OUT) Object value to be changed
 * @param status (IN) status of the object 
 * @return LWM2M response type
 */ 
static uint8_t prv_change_device_status(lwm2m_object_t * objectP, bool status, 
    uint16_t instanceId)
{

  device_cap_manangement_t* device_cap_obj = NULL;

  if( objectP == NULL ) 
  {
    LOG_DEBUG("fun [%s] %s",__func__, nullError);
    return  LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  device_cap_obj = (device_cap_manangement_t *)LWM2M_LIST_FIND(objectP->instanceList, 
      instanceId);
  if (NULL == device_cap_obj)
  {
    return LWM2M_404_NOT_FOUND;
  }

  if(status == false)
  {
    if(device_cap_obj->enabled == true)
    {
      LOG_DEBUG("Disabling device ");
      device_cap_obj->enabled = false;
      return LWM2M_204_CHANGED;
    }
    else
    {
      LOG_DEBUG("Device already disabled");
      return LWM2M_406_NOT_ACCEPTABLE;
    }
  }

  else
  {
    if(device_cap_obj->enabled == false)
    {
      LOG_DEBUG("Enabling device ");
      device_cap_obj->enabled = true;
      return LWM2M_204_CHANGED;
    }
    else
    {
      LOG_DEBUG("Device already enabled");
      return LWM2M_406_NOT_ACCEPTABLE;
    }
  }
}

/**
 * @brief This fucntion implements the execute operation performed on Device
 * capability management object.
 * @param instanceId (IN) Instance ID of the object to be read
 * @param resourceId (IN) Resource ID of the resource to be executed
 * @param buffer (IN) message payload from server fro execute operation
 * @param length (IN) message payload length
 * @param objectP Object on which the operation needs to be executed
 * @return LWM2M response type
 */ 
static uint8_t prv_devicecap_execute(uint16_t instanceId,
    uint16_t resourceId,
    uint8_t * buffer,
    int length,
    lwm2m_object_t * objectP)
{
  device_cap_manangement_t *instanceP = NULL;
  uint8_t ret_val = LWM2M_405_METHOD_NOT_ALLOWED;
  lwm2m_object_data_t lwm2m_data;
  uint8_t id_set = 0;

  if ( objectP == NULL )
  {
    LOG_DEBUG("fun [%s] %s",__func__, nullError);
    return LWM2M_400_BAD_REQUEST;
  }

  instanceP = (device_cap_manangement_t *)LWM2M_LIST_FIND(objectP->instanceList, instanceId);

  if (NULL == instanceP)
  {
    return LWM2M_404_NOT_FOUND;
  }

  switch(resourceId)
  {
    case RES_M_OP_ENABLE:
      ret_val = prv_change_device_status(objectP, true, instanceId);

      LOG_DEBUG(" OP ENABLE");

      //Intimate the device capability app on value change
      if (0 != (ret_val = prv_init_resources(&lwm2m_data, &id_set, LWM2M_DEVICE_CAP_OBJECT_ID, instanceId, RES_M_OP_ENABLE)))
	  	return ret_val;
      lwm2m_data.instance_info->resource_info->type = QAPI_NET_LWM2M_TYPE_BOOLEAN_E;
      lwm2m_data.instance_info->resource_info->value.asBoolean  = 1;
      instanceP->enabled = TRUE;
      lwm2m_check_if_observe_and_update_app(id_set, LWM2M_DEVICE_CAP_OBJECT_ID, instanceId, RES_M_OP_ENABLE, &lwm2m_data);
      lwm2m_free_instance_info(lwm2m_data.instance_info);
      ret_val = LWM2M_204_CHANGED;
      break;

    case RES_M_OP_DISBALE:
      ret_val = prv_change_device_status(objectP, false, instanceId);
      LOG_DEBUG(" OP DISABLE");
      //Intimate the device capability app on value change
      if (0 != (ret_val = prv_init_resources(&lwm2m_data, &id_set, LWM2M_DEVICE_CAP_OBJECT_ID, instanceId, RES_M_OP_DISBALE)))
	  	return ret_val;

      lwm2m_data.instance_info->resource_info->type = QAPI_NET_LWM2M_TYPE_BOOLEAN_E;
      lwm2m_data.instance_info->resource_info->value.asBoolean  = 0;
      instanceP->enabled = FALSE;
      lwm2m_check_if_observe_and_update_app(id_set, LWM2M_DEVICE_CAP_OBJECT_ID, instanceId, RES_M_OP_DISBALE, &lwm2m_data);
      lwm2m_free_instance_info(lwm2m_data.instance_info);
      ret_val = LWM2M_204_CHANGED;

     break;
    case RES_M_PROPERTY:
    case RES_M_GROUP:
    case RES_O_DESCRIPTION:
    case RES_O_ATTACHED:
    case RES_M_ENABLED:
    case RES_O_NOTIFY_EN:
      return LWM2M_405_METHOD_NOT_ALLOWED;
    default:
      return LWM2M_404_NOT_FOUND;
  }
  return ret_val;
}


/**
 * @fn      static uint8_t prv_devicecap_delete()
 * @brief   This function implements the delete operation performed
 *           on the Device cap object
 * @param   instanceId (IN) Instance ID of the object instance to be deleted
 * @param   objectP (IN)    Object whose instance is to be deleted
 * @return  LWM2M response type
 */
static uint8_t prv_devicecap_delete(uint16_t instanceId,
    lwm2m_object_t *objectP,
    bool persist_file)
{
  device_cap_manangement_t *instanceP = NULL;
  if( objectP == NULL )
  {
    LOG_DEBUG("fun [%s] %s",__func__,nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  objectP->instanceList = LWM2M_LIST_RM(objectP->instanceList,instanceId, &instanceP);
  if (instanceP == NULL)
  {
    return LWM2M_404_NOT_FOUND;
  }

   /* Update the persistent file */
  store_device_cap_manangement_persistent_info(instanceP, LWM2M_FALSE);
  
  lwm2m_free(instanceP);

  return LWM2M_202_DELETED;
}

/*
 * @fn      static int32_t prv_devicecap_setvalue ()
 * @brief   This function is used to  set the particular resource values for given instance 
 * @param   lwm2m_data - pointer to lwm2m object information
 *          targetP    - pointer to lwm2m object 
 * @return  on success - 1
 on failure - 0
 */

static int32_t prv_devicecap_setvalue (lwm2m_object_data_t * lwm2m_data, lwm2m_object_t * targetP)
{

  uint8_t result = 0;
  device_cap_manangement_t* instanceP = NULL;  
  lwm2m_data_t* dataP = NULL;
  bool val_change = false;

  if (lwm2m_data == NULL || targetP == NULL)
  {
    ds_iot_ulog_err ("LWM2M_LOG: NULL Error");
    return 0;
  }

  if((lwm2m_data->no_instances != 0x01)|| 
         (!lwm2m_data->instance_info) ||
         ((lwm2m_data->instance_info->no_resources) &&
         (!lwm2m_data->instance_info->resource_info)))
    return 0;
  
  instanceP = (device_cap_manangement_t *)LWM2M_LIST_FIND(targetP->instanceList, lwm2m_data->instance_info->instance_ID);

  if (NULL == instanceP)
  {
    return 0;
  }

  if(QAPI_OK != lwm2m_parse_application_object_info(lwm2m_data->instance_info,
                                                    lwm2m_data->instance_info->resource_info,
                                                    &dataP))
  {
    return 0;
  }

  result = prv_devicecap_write_resources(instanceP, 1, dataP, &val_change);

  lwm2m_data_free_ext(lwm2m_data->instance_info->no_resources, dataP);
  if(result != LWM2M_204_CHANGED)
  {
    return 0;
  }
  else
  {
    if(val_change == true)
    {
      resource_value_change(lwm2m_data);
      store_device_cap_manangement_persistent_info(instanceP, LWM2M_TRUE);
    }
    result = 1;
  }

  return result;

}


static uint8_t prv_devicecap_getValue(uint16_t instanceId, lwm2m_object_t *objectP, void * getval)
{
  device_cap_manangement_t *device_cap_obj = NULL;
  bool *retval;

  if((objectP == NULL) || (NULL == getval)) 
  {
    ds_iot_ulog_err("fun [prv_devicecap_getValue] LWM2M_LOG: NULL Error");
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  device_cap_obj = (device_cap_manangement_t *)LWM2M_LIST_FIND(objectP->instanceList, 
      instanceId);
  if (NULL == device_cap_obj)
  {
    return LWM2M_404_NOT_FOUND;
  }

  retval = getval;
  *retval = device_cap_obj->notifyen;

  return LWM2M_204_CHANGED;
}

/*
 * @fn      static int32_t prv_devicecap_getvalue ()
 * @brief   This function is used to  get the particular resource values for given instance 
 * @param   id_info      - indicates what values need to be filled whether resource or instance or object level 
 lwm2m_dataP  - pointer to lwm2m object information to be filled 
 *          targetP      - pointer to lwm2m object 
 * @return  on successs  -  0
 on failure   - -1
 */


int32_t prv_devicecap_getvalue(lwm2m_id_info_t id_info, lwm2m_object_data_t **lwm2m_dataP , lwm2m_object_t * targetP
                                         , lwm2m_resource_info_t **resource_info)
{
  int8_t result = -1;
  lwm2m_data_t *dataArrayP = NULL;
  //lwm2m_resource_info_t *resource_info = NULL;

  int numData = 0;
  uint16_t instance_id = 0;

  if(lwm2m_dataP == NULL)
    return -1;

  if((result = prv_init_getvalue(id_info,*lwm2m_dataP, resource_info, &dataArrayP, &numData))==-1)
    return result;

  instance_id = id_info.instance_ID;
  result = prv_devicecap_read(instance_id, &numData, &dataArrayP, targetP,INVALID_RES_INST_ID, NULL );

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
 * @brief This function is used to get the information regarding the Device
 * capability management object during client registration.
 * @param void
 * @return Device capability management object  
 */ 
lwm2m_object_t * get_object_deviceCap()
{

  /* The get_object_deviceCap function create the object itself and return a 
     pointer to the structure that represent it.*/
  lwm2m_object_t * deviceCapObj = NULL;

  deviceCapObj = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));

  if(NULL != deviceCapObj)
  {
    memset(deviceCapObj, 0, sizeof(lwm2m_object_t));

    /* It assigns his unique ID. The 15 is the standard ID for the mandatory 
       object "Object device".*/
    deviceCapObj->objID = LWM2M_DEVICE_CAP_OBJECT_ID;

    /*
     * And the private function that will access the object.
     * Those function will be called when a read/write/execute query is made by the server. In fact the library don't need to
     * know the resources of the object, only the server does.
     */
    deviceCapObj->readFunc     = prv_devicecap_read;
    deviceCapObj->discoverFunc = prv_devicecap_discover;
    deviceCapObj->writeFunc    = prv_devicecap_write;
    deviceCapObj->executeFunc  = prv_devicecap_execute;
    deviceCapObj->createFunc   = prv_devicecap_create;
    deviceCapObj->deleteFunc   = prv_devicecap_delete;
    deviceCapObj->getValueFunc = prv_devicecap_getValue;
    deviceCapObj->get_Value_Func = prv_devicecap_getvalue;
    deviceCapObj->setValueFunc   = prv_devicecap_setvalue;
  }

  return deviceCapObj;
}

/**
 * @brief This fucntion frees the Device capability management object allocated
 * @param ObjectP (IN) Object to free
 * @return void
 */ 
void free_object_devicecap(lwm2m_object_t * objectP)
{
  device_cap_manangement_t *instances = NULL;

  if( objectP == NULL )
  { 
    LOG_DEBUG("fun [%s] %s",__func__,nullError);
    return ;
  }

  if(NULL != objectP->userData)
  {
    lwm2m_free(objectP->userData);
    objectP->userData = NULL;
  }

  instances = (device_cap_manangement_t *)objectP->instanceList;

  while(NULL != instances)
  {
    objectP->instanceList = (lwm2m_list_t *)instances->next;
	store_device_cap_manangement_persistent_info(instances , LWM2M_FALSE);
    lwm2m_free(instances);
    instances = (device_cap_manangement_t *)objectP->instanceList;
  }

  lwm2m_free(objectP);
}

#endif //ENABLE_DEVCAP_OBJ
