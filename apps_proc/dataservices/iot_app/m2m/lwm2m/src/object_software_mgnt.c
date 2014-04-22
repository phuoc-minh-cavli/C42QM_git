/******************************************************************************

  @file    object_software_mgnt.c
  @brief   File contains the implementation for the operations related to object
           Software management
  ---------------------------------------------------------------------------

  Copyright (c) 2016-2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------

 *****************************************************************************/

/*
 *  Object                         |      | Multiple  |     | Description
 *  |
 *  Name                           |  ID  | Instances |Mand.|
 *  |
 *---------------------------------+------+-----------+-----+-------------------------------+
 *  Software Management            |  9   |    Yes    |  No | urn:oma:lwm2m:oma:9
 *  |
 *
 *  Resources:
 *  Name        | ID  | Oper.|Instances|Mand.|  Type   | Range | Units |
 *  Description
 *  |
 * -------------+-----+------+---------+-----+---------+-------+-------+----------------------------------------------------------------------------------+
 *  PkgName     |  0  |  R   | Single  | Yes | String  | 0-255 |       | Name of the software package
 *  PkgVersion  |  1  |  R   | Single  | Yes | String  | 0-255 |       | Version of the software package
 *  Package     |  2  |  W   | Single  | No  | Opaque  |       |       | Software package The package can be in one single software component,
 or any delivery material used by the Device to determine the
 software component(s) and proceed to their installation. Can be 
 archive file, executable, manifest. This resource to be used when it 
 is single block of delivery.
 * Package URI  |  3  |  W   | Single  | No  | String  | 0-255 |       | URI from where the device can download the software package by an 
 alternative mechanism. As soon the device has received the Package 
 URI it performs the download at the next practical opportunity.
 Can be direct link to a single software component or link to archive 
 file, executable, or manifest, used by the Device to determine, 
 then access to the software component(s). This resource to be used 
 when it is single block of delivery.
 * Install      |  4  |  E   | Single  | Yes |         |       |       | Installs software from the package either stored in Package 
 resource, or,downloaded from the Package URI. This Resource is only
 executable when the value of the State Resource is DELIVERED.
 *


*/
#include "liblwm2m.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "ds_log.h"


#ifdef ENABLE_SOFTWARE_MGNT_OBJ
#define RES_M_PKG_NAME      0 /**< Resource ID for "PkgName" */
#define RES_M_PKG_VERSION       1 /**< Resource ID for "PkgVersion" */
#define RES_O_PACKAGE           2 /**< Resource ID for "Package" */
#define RES_O_PACKAGE_URI       3 /**< Resource ID for "Package URI" */
#define RES_M_INSTALL           4 /**< Resource ID for "Install" */
#define RES_O_CHECKPOINT        5 /**< Resource ID for "Checkpoint" */
#define RES_M_UNINSTALL     6 /**< Resource ID for "Uninstall" */
#define RES_M_UPDATE_STATE  7 /**< Resource ID for "Update State" */
#define RES_O_UPDATE_SUPPORTED_OBJ  8 /**< Resource ID for "Update
                                        Supported Objects" */
#define RES_M_UPDATE_RESULT 9 /**< Resource ID for "Update
                                Result" */   
#define RES_M_ACTIVATE      10 /**< Resource ID for "Activate" */
#define RES_M_DEACTIVATE        11 /**< Resource ID for "Deactivate" */
#define RES_M_ACTIVATION_STATE  12 /**< Resource ID for "Activation
                                     State" */
#define RES_O_PACKAGE_SETTINGS  13 /**< Resource ID for "Package
                                     Settings" */
#define RES_O_USERNAME          14 /**< Resource ID for "User Name" */
#define RES_O_PASSWORD          15 /**< Resource ID for "Password" */
#define RES_O_STATUS_REASON     16 /**< Resource ID for "Status
                                     Reason" */
#define RES_O_SOFTWARE_COMP_LINK    17 /**< Resource ID for "Software
                                         Component Link" */
#define RES_O_SOFTWARE_COMP_TREE_LEN    18 /**< Resource ID for "Software
                                             Component tree length" */

#define PRV_UPDATE_STATE_INITIAL         0/**< Update state "initial" */
#define PRV_UPDATE_STATE_DOWNLOAD_STARTED   1 /**< Update state "Download
                                                started" */
#define PRV_UPDATE_STATE_DOWNLOADED    2 /**< Update state "Downloaded" */
#define PRV_UPDATE_STATE_DELIVERED      3 /**< Update state "Delivered " */
#define PRV_UPDATE_STATE_INSTALLED      4 /**< Update state "State installed" */


#define PRV_UPDATE_RES_INITIAL      0 /**< Update result "Initial" */
#define PRV_UPDATE_RES_DOWNLOADING  1 /**< Update result "Downloading" */
#define PRV_UPDATE_RES_INSTALL_SUCCESS  2 /**< Update result "install success" */
#define PRV_UPDATE_RES_DOWNLOAD_SUCCESS 3 /**< Update result "Download success" */
#define PRV_UPDATE_RES_NO_ENOUGH_STORAGE    50 /**< Update result "Not enough
                                                 storage" */
#define PRV_UPDATE_RES_OUT_OF_MEMORY    51 /**< Update result "Device is out of
                                             memory" */
#define PRV_UPDATE_RES_CONNECTION_LOST  52 /**< Update result "Connection lost" */
#define PRV_UPDATE_RES_PKG_CHECK_FAILURE    53 /**< Update result "Package check
                                                 failure" */
#define PRV_UPDATE_RES_PKG_UNSUPPORTED      54 /**< Update result "package
                                                 unsupported" */
#define PRV_UPDATE_RES_INVALID_URI      56 /**< Update result "URI is invalid" */
#define PRV_UPDATE_RES_UPDATE_ERROR     57 /**< Update result "Update error" */
#define PRV_UPDATE_RES_INSTALL_ERROR    58 /**< Update result "Install error" */
#define PRV_UPDATE_RES_UNINSTALL_ERROR  59 /**< Update result "uninstall error" */
#define PRV_UPDATE_RES_IMPROPER_STATE 60 /**< Out of state events, Eg: Package written 
                                           when state is neither initial nor downloading */

#define PRV_MAX_STRSIZE 255

#define PRV_EMPTY   " "

extern bool gInboundFirwareTransferComplete;
bool gpackage_running = false;

void free_object_softwaremgnt(lwm2m_object_t * objectP);

/**
 * @brief Describes the Software management object resources
 *
 */
typedef struct _software_mgnt_t
{
  struct _software_mgnt_t *next;  /**< corresponding to lwm2m_list_t::next */
  uint16_t    id;                 /**< corresponding to lwm2m_list_t::id  */
  char pkg_name[PRV_MAX_STRSIZE];     /**< Package name (Mandatory) */
  char pkg_version[PRV_MAX_STRSIZE];  /**< Package version (Mandatory) */
  char package[PRV_MAX_STRSIZE];      /**< Package (optional) */
  char package_uri[PRV_MAX_STRSIZE];  /**< Pacakge URI (optional) */
  int8_t update_state;    /**< Update state (Mandatory) */
  bool update_supp_obj;   /**< Update supported objects (Optional) */
  int16_t update_result;  /**< Update result (Mandatory) */
  bool activation_state;  /**< Activation state (Mandatory) */
  char username[PRV_MAX_STRSIZE];     /**< User name (optional) */
  char password[PRV_MAX_STRSIZE];     /**< Password (optional) */
  char status_reason[PRV_MAX_STRSIZE];    /**< Status reason (optional) */
} software_mgnt_t;

/**
 * @brief Describes the Software management persistance instances .
 *
 */

typedef struct _software_mgnt_persistent_instance_s
{
  uint16_t    id;                 /**< corresponding to lwm2m_list_t::id  */
  char pkg_name[PRV_MAX_STRSIZE];     /**< Package name (Mandatory) */
  char pkg_version[PRV_MAX_STRSIZE];  /**< Package version (Mandatory) */
  int8_t update_state;    /**< Update state (Mandatory) */
  int16_t update_result;  /**< Update result (Mandatory) */
  bool activation_state;  /**< Activation state (Mandatory) */
  char username[PRV_MAX_STRSIZE];     /**< User name (optional) */
  char password[PRV_MAX_STRSIZE];     /**< Password (optional) */
  char status_reason[PRV_MAX_STRSIZE];    /**< Status reason (optional) */
  bool isValid;
  
} software_mgnt_persistent_instance_t;

/**
 * @fn uint8_t store_software_mgnt_persistent_info()
 * @brief This function is used to store the software management profile into persistent storage
 * 
 * @param targetP pointer to software management object instance
 * @param store   to check whether instance stored is valid or not.
 * 
 * @return return error code
 */
static int store_software_mgnt_persistent_info(software_mgnt_t * targetP, bool store)
{

	  int fd = 0, ret = 0, index = 0;
	  software_mgnt_persistent_instance_t *info = NULL;
	  software_mgnt_persistent_instance_t *conn_ctx = NULL;
	  
	  if(NULL == targetP)
	  {
		LOG_DEBUG("LWM2M_LOG: NULL Error Object Instance");
		return EFAILURE;
	  }
	
	  info = (software_mgnt_persistent_instance_t *)malloc(sizeof(software_mgnt_persistent_instance_t));
	  
	  if(info == NULL)
	  { 
	    ds_iot_ulog_err("LWM2M_LOG: NULL Error: info");
		return EFAILURE;
	  }
	  
	  conn_ctx = (software_mgnt_persistent_instance_t *)malloc(sizeof(software_mgnt_persistent_instance_t));
	  
	  if(conn_ctx == NULL)
	  {
		free(info);
		info = NULL;
	    ds_iot_ulog_err("LWM2M_LOG: NULL Error: conn_ctx ");
		return EFAILURE;
	  }
	
	  /* open software management info persistent file.If not already present, create it*/
	  ds_iot_log_strf_med("EFS_OPT: File open %s",LWM2M_SOFTWARE_MGNT_PERSISTENCE_FILE);
	  fd = efs_open(LWM2M_SOFTWARE_MGNT_PERSISTENCE_FILE, O_CREAT | O_RDWR, 00644);
	  
	  if (EFAILURE == fd)
	  {
		ds_iot_ulog_err("failed to open software management persistence file");
		ret = EFAILURE;
		goto end;
	  }
	
	  while (1)
	  {
		memset(info, 0x00, sizeof(software_mgnt_persistent_instance_t));
		ret = efs_read(fd, info, sizeof(software_mgnt_persistent_instance_t));

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
	
	  memset(conn_ctx, 0x00, sizeof(software_mgnt_persistent_instance_t));
	  
      LWM2M_STRCPY(conn_ctx->pkg_name, targetP->pkg_name , strlen(targetP->pkg_name) + 1);
      LWM2M_STRCPY(conn_ctx->pkg_version,targetP->pkg_version, strlen(targetP->pkg_version) + 1); 
	  LWM2M_STRCPY(conn_ctx->username, targetP->username, strlen(targetP->username) + 1);
      LWM2M_STRCPY(conn_ctx->password, targetP->password , strlen(targetP->password) + 1);
      LWM2M_STRCPY(conn_ctx->status_reason, targetP->status_reason , strlen(targetP->status_reason) + 1);

      conn_ctx->update_state = targetP->update_state;
      conn_ctx->update_result = targetP->update_result;
      conn_ctx->activation_state = targetP->activation_state;
	  conn_ctx->id = targetP->id;
	  
	  if(store)
	  {
		conn_ctx->isValid = LWM2M_TRUE; 
	  }
	  else
	  {
		conn_ctx->isValid = LWM2M_FALSE; 
	  }
	
	  /* lseek to reqd index and write the software management_info structure in efs */
	  efs_lseek(fd, index * sizeof(software_mgnt_persistent_instance_t), SEEK_SET);
	  ds_iot_log_strf_med("EFS_OPT: File write %s",LWM2M_SOFTWARE_MGNT_PERSISTENCE_FILE);
	  if (EFAILURE == efs_write(fd, conn_ctx, sizeof(software_mgnt_persistent_instance_t)))
	  {
		ds_iot_ulog_err("failed to store persistent value of software management instance");
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
 * @fn uint8_t load_software_mgnt_persistent_info()
 * @brief This function is used to fetch the software management profile from persistent storage
 * 
 * @param objectP pointer to software management object instance
 * 
 * @return return error code
 */
int load_software_mgnt_persistent_info(lwm2m_object_t * objectP)
{
   int fd = 0, ret = 0;

   if (NULL == objectP)
   {
	  return EFAILURE;
   }
   ds_iot_log_strf_med("EFS_OPT: File open %s",LWM2M_SOFTWARE_MGNT_PERSISTENCE_FILE);
   fd = efs_open(LWM2M_SOFTWARE_MGNT_PERSISTENCE_FILE, O_RDONLY);
   
   if (EFAILURE == fd)
   {
      ds_iot_ulog_err(" EFAILURE == fd");
      return EFAILURE;
   }

  while(1)
  {
    software_mgnt_persistent_instance_t info;
    software_mgnt_t *instanceP = NULL;

    memset(&info, 0, sizeof(info));
    ret = efs_read(fd, &info, sizeof(info));
	
    if (EFAILURE == ret || 0 == ret)
    { 
      ds_iot_ulog_err(" EFAILURE == ret || 0 == ret");
      break;
	}
      

    /*Check if the structure entry in file is valid*/
    if(LWM2M_FALSE == info.isValid)
    {
      continue;
    }

    instanceP = (software_mgnt_t *)lwm2m_malloc(sizeof(software_mgnt_t));
	
    if(instanceP == NULL)
    {
      ret = -1;
      break;
    }

    memset(instanceP, 0, sizeof(software_mgnt_t));
   
	LWM2M_STRCPY(instanceP->pkg_name, info.pkg_name , strlen(info.pkg_name) + 1);
    LWM2M_STRCPY(instanceP->pkg_version,info.pkg_version, strlen(info.pkg_version) + 1);

    instanceP->update_state = info.update_state;
    instanceP->update_result = info.update_result;
    instanceP->activation_state = info.activation_state;
	instanceP->id = info.id ;

	LWM2M_STRCPY(instanceP->username, info.username, strlen(info.username) + 1);
    LWM2M_STRCPY(instanceP->password, info.password , strlen( info.password) + 1);
    LWM2M_STRCPY(instanceP->status_reason, info.status_reason , strlen(info.status_reason) + 1);

    objectP->instanceList = LWM2M_LIST_ADD(objectP->instanceList, instanceP);
  }
  
  if (ret < 0)
  {
    efs_close(fd);
    free_object_softwaremgnt(objectP);
    objectP = NULL;
    return EFAILURE;
  }
  
  efs_close(fd);
  return ESUCCESS;

}

/** 
* @fn static uint8_t prv_softwaremgnt_create_write_check() 
*
* @brief This fucntion verifies and write the resource's into  
*        software management object 
*
* @param instanceId Instance of the software management object to be create/write 
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
static uint8_t prv_softwaremgnt_create_write_check(int numData,
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
      case RES_M_INSTALL:
      case RES_M_UNINSTALL:
      case RES_M_ACTIVATE:
      case RES_M_DEACTIVATE:	
      case RES_M_PKG_NAME:
      case RES_M_PKG_VERSION:
	  case RES_M_UPDATE_STATE:	
	  case RES_M_UPDATE_RESULT:
	  case RES_M_ACTIVATION_STATE:
	  case RES_O_STATUS_REASON:
	     if (!create_flag)
         {
           result = LWM2M_405_METHOD_NOT_ALLOWED;
           break;
         }
      case RES_O_PACKAGE:
      case RES_O_PACKAGE_URI:
	  case RES_O_UPDATE_SUPPORTED_OBJ:
	  case RES_O_PACKAGE_SETTINGS:
      case RES_O_USERNAME:
      case RES_O_PASSWORD:
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
static uint8_t prv_write_smresources(software_mgnt_t *instanceP, int numData,
    lwm2m_data_t *dataArray)
{
  int i = 0;
  if( instanceP == NULL  || dataArray == NULL )
  {
    LOG_DEBUG("fun [%s] %s",__func__, nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  for (i = 0; i < numData; i++)
  {
    switch((dataArray + i)->id)
    {
      case RES_M_PKG_NAME:
        LWM2M_STRCPY(instanceP->pkg_name, (const char *) ((dataArray + i)->value.asBuffer.buffer), 
            PRV_MAX_STRSIZE);
        break;
      case RES_M_PKG_VERSION:
        LWM2M_STRCPY(instanceP->pkg_version, (const char *)((dataArray + i)->value.asBuffer.buffer),
            PRV_MAX_STRSIZE);
        break;
      case RES_M_UPDATE_STATE:
        {
          int64_t value = 0;
          if (1 != lwm2m_data_decode_int(dataArray + i, (int64_t*)&value))
            return LWM2M_400_BAD_REQUEST;
          instanceP->update_state = (int8_t)value;
        }
        break;
      case RES_M_UPDATE_RESULT:
        {
          int64_t value = 0;
          if (1 != lwm2m_data_decode_int(dataArray + i, (int64_t*)&value))
            return LWM2M_400_BAD_REQUEST;
          instanceP->update_result = value;
        }
        break;
      case RES_M_ACTIVATION_STATE:
        if (1 != lwm2m_data_decode_bool(dataArray + i, &instanceP->activation_state))
          return LWM2M_400_BAD_REQUEST;
        break;
      default:
        //Ignoring any optional resource
        break;
    }
  }

  store_software_mgnt_persistent_info(instanceP, LWM2M_TRUE);
  
  return LWM2M_NO_ERROR;
}
/**
 * @fn static uint8_t prv_softwaremgnt_create()
 * @brief   This function implements the create operation performed
 * on the Software Management object
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

static uint8_t prv_softwaremgnt_create(uint16_t instanceId, int numData, 
    lwm2m_data_t * dataArray, 
    lwm2m_object_t * objectP, bool apiCall)
{
  software_mgnt_t *instanceP = NULL;
  uint8_t result = LWM2M_500_INTERNAL_SERVER_ERROR;
  uint16_t mandatory_resource[] = {RES_M_PKG_NAME, RES_M_PKG_VERSION, RES_M_UPDATE_STATE, RES_M_UPDATE_RESULT,RES_M_ACTIVATION_STATE};
  uint8_t size = sizeof(mandatory_resource)/sizeof(mandatory_resource[0]);
  if( dataArray == NULL || objectP == NULL ) 
  {
    LOG_DEBUG("fun [%s] %s",__func__, nullError);
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
  result = prv_softwaremgnt_create_write_check(numData, dataArray, TRUE);
  if (result != LWM2M_204_CHANGED )
  {
     ds_iot_ulog_err("Creating instance failed ");
     return result;
  }

skip_check:
  instanceP = (software_mgnt_t *)lwm2m_malloc(sizeof(software_mgnt_t));
  if (instanceP != NULL)
  {
    memset(instanceP, 0, sizeof(software_mgnt_t));
    instanceP->id = instanceId;
    result = prv_write_smresources(instanceP, numData, dataArray);

    if (result == LWM2M_NO_ERROR)
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
 * @fn static uint8_t prv_softwaremgnt_delete(uint16_t instanceId, 
 *                          lwm2m_object_t * objectP)
 * @brief   This function implements the delete operation performed
 * on the Software Management object
 * @param   instanceId (IN) Instance ID of the object instance to be deleted
 * @param   objectP (IN)    Object whose instance is to be deleted
 * @return  LWM2M response type
 */
static uint8_t prv_softwaremgnt_delete(uint16_t instanceId,
    lwm2m_object_t *objectP,
    bool persist_file)
{
  software_mgnt_t *instanceP = NULL;
  if( objectP == NULL ) 
  {
    LOG_DEBUG("fun [%s] %s",__func__, nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  objectP->instanceList = LWM2M_LIST_RM(objectP->instanceList,instanceId, &instanceP);
  if (instanceP == NULL) return LWM2M_404_NOT_FOUND;

   /* Update the persistent file */
  store_software_mgnt_persistent_info(instanceP, LWM2M_FALSE);
   
  lwm2m_free(instanceP);

  return LWM2M_202_DELETED;
}

/**
 * @brief This fucntion is used to populate the values of the resource
 * to be provided to the server during the read operation on the object.
 * @param dataP (OUT) data array for the resource 
 * @param swMgntDataP (IN) Software Management object data
 * @return LWM2M response type
 */
static uint8_t prv_set_value(lwm2m_data_t * dataP,
    software_mgnt_t * swMgntDataP)
{
  if( dataP == NULL || swMgntDataP == NULL ) 
  {
    LOG_DEBUG("fun [%s] file [%s]  %s",__func__,__FILE__, nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  switch (dataP->id)
  {
    case RES_M_PKG_NAME:
      lwm2m_data_encode_string(swMgntDataP->pkg_name, dataP); 
      return LWM2M_205_CONTENT;
    case RES_M_PKG_VERSION:
      lwm2m_data_encode_string(swMgntDataP->pkg_version, dataP);
      return LWM2M_205_CONTENT;
    case RES_M_UPDATE_STATE:
      lwm2m_data_encode_int(swMgntDataP->update_state, dataP);
      return LWM2M_205_CONTENT;
    case RES_M_UPDATE_RESULT:
      lwm2m_data_encode_int(swMgntDataP->update_result, dataP);
      return LWM2M_205_CONTENT;
    case RES_M_ACTIVATION_STATE:
      lwm2m_data_encode_bool(swMgntDataP->activation_state, dataP);
      return LWM2M_205_CONTENT;
    case RES_O_STATUS_REASON:
      lwm2m_data_encode_string(swMgntDataP->status_reason, dataP);
      return LWM2M_205_CONTENT;
    case RES_O_PACKAGE:
    case RES_O_PACKAGE_URI:
    case RES_M_INSTALL:
    case RES_M_UNINSTALL:
    case RES_M_ACTIVATE:
    case RES_M_DEACTIVATE:
    case RES_O_USERNAME:
    case RES_O_PASSWORD:
      return LWM2M_405_METHOD_NOT_ALLOWED;
    case RES_O_CHECKPOINT:
    case RES_O_PACKAGE_SETTINGS:
    case RES_O_SOFTWARE_COMP_LINK:
    case RES_O_SOFTWARE_COMP_TREE_LEN:
    case RES_O_UPDATE_SUPPORTED_OBJ:
    default:
      return LWM2M_404_NOT_FOUND;
  }
}

/**
 * @brief This function implements the read operation performed on Software
 * management object
 * @param instanceId (IN) Instance ID of the object to be read
 * @param numDataP (OUT) number of resources in the array
 * @param dataArrayP (OUT) resource value array
 * @param objectP (IN) Object to be read
 * @return LWM2M response type
 */
static uint8_t prv_softwaremgnt_read(uint16_t instanceId,
    int * numDataP,
    lwm2m_data_t ** dataArrayP,
    lwm2m_object_t * objectP,
    uint16_t resInstId, lwm2m_server_t *serverP)
{
  uint8_t result = LWM2M_404_NOT_FOUND;
  int i = 0;
  software_mgnt_t *instanceP = NULL;
  if( dataArrayP == NULL || objectP == NULL ) 
  {
    ds_iot_ulog_err("fun [prv_softwaremgnt_read]:LWM2M_LOG: NULL Error");
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  instanceP = (software_mgnt_t *)LWM2M_LIST_FIND(objectP->instanceList, 
      instanceId);
  if (NULL == instanceP) return result;

  if(*numDataP == 0)
  {
    uint16_t resList[] = {
      RES_M_PKG_NAME,
      RES_M_PKG_VERSION,
      RES_M_UPDATE_STATE,
      RES_M_UPDATE_RESULT,
      RES_M_ACTIVATION_STATE,
      RES_O_STATUS_REASON
    };

    int nbRes = sizeof(resList)/sizeof(uint16_t);

    *dataArrayP = lwm2m_data_new(nbRes);
    if(*dataArrayP == NULL) return LWM2M_500_INTERNAL_SERVER_ERROR;
    *numDataP = nbRes;
    for(i = 0; i < nbRes; i++)
    {
      (*dataArrayP)[i].id = resList[i];
    }       
  }

  i = 0;
  do
  {
    result = prv_set_value((*dataArrayP) + i, instanceP);
    i++;
  } while(i < *numDataP && result == LWM2M_205_CONTENT);

  return result;
}

/**
 * @brief This fucntion implements the discover operation performed on the
 * Software management object
 * @param instanceId (IN) Instance ID of the object to be read
 * @param numDataP (OUT) number of resources in the array
 * @param dataArrayP (OUT) resource value array
 * @param objectP (IN) Object to be read
 * @return LWM2M response type
 */
static uint8_t prv_softwaremgnt_discover(uint16_t instanceId,
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
      RES_M_PKG_NAME,
      RES_M_PKG_VERSION,
      RES_O_PACKAGE,
      RES_O_PACKAGE_URI,
      RES_M_INSTALL,
      //Uncomment below line when checkpoint resource is supported
      //RES_O_CHECKPOINT, 
      RES_M_UNINSTALL,
      RES_M_UPDATE_STATE,
      RES_M_UPDATE_RESULT,
      RES_M_ACTIVATE,
      RES_M_DEACTIVATE,
      RES_M_ACTIVATION_STATE,  
      //Uncomment below line when package setting resource is supported
      //RES_O_PACKAGE_SETTINGS,
      RES_O_USERNAME,
      RES_O_PASSWORD,
      RES_O_STATUS_REASON,
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
        case RES_M_PKG_NAME:
        case RES_M_PKG_VERSION:
        case RES_O_PACKAGE:
        case RES_O_PACKAGE_URI: 
        case RES_M_INSTALL:
          //Uncomment below line when checkpoint resource is supported
          //case RES_O_CHECKPOINT:
        case RES_M_UNINSTALL:
        case RES_M_UPDATE_STATE:
        case RES_M_UPDATE_RESULT:
        case RES_M_ACTIVATE:
        case RES_M_DEACTIVATE:
        case RES_M_ACTIVATION_STATE:
          //Uncomment below line when package setting resource is supported
          //case RES_O_PACKAGE_SETTINGS:
        case RES_O_USERNAME:
        case RES_O_PASSWORD:
        case RES_O_STATUS_REASON:
          break;
        default:
          result = LWM2M_404_NOT_FOUND;
      }
    }
  }

  return result;
}

/**
 * @brief This fucntion implements the write operation performed on software
 * management object
 * @param instanceId (IN) Instance ID of the object to be read
 * @param numDataP (OUT) number of resources in the array
 * @param dataArrayP (OUT) resource value array
 * @param objectP (IN) Object to be read
 * @return LWM2M response type
 */
static uint8_t prv_softwaremgnt_write(uint16_t instanceId,
    int numData,
    lwm2m_data_t * dataArray,
    lwm2m_object_t * objectP,
    uint16_t resInstId, uint8_t write_method, bool bs_true)
{
  int i = 0;
  uint8_t result = LWM2M_404_NOT_FOUND;
  bool val_change = FALSE;

  lwm2m_object_data_t lwm2m_data;
  uint8_t id_set = 0;
  software_mgnt_t *instanceP = NULL;
  if( dataArray == NULL || objectP == NULL ) 
  {
    ds_iot_ulog_err("fun [prv_softwaremgnt_write] LWM2M_LOG: NULL Error");
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  } 

  instanceP = (software_mgnt_t *)LWM2M_LIST_FIND(objectP->instanceList, 
      instanceId);
  if (NULL == instanceP) return result;
  /* Check all resources to be written are writable or not */
  result = prv_softwaremgnt_create_write_check(numData, dataArray, bs_true);
  if (result != LWM2M_204_CHANGED )
  {
    ds_iot_ulog_err("Write operation failed due to write on non-writable resource. ");
    return result;
  }

  do
  {
    switch(dataArray[i].id)
    {
      case RES_O_PACKAGE:
        if ((PRV_UPDATE_STATE_DOWNLOAD_STARTED != instanceP->update_state
              && PRV_UPDATE_STATE_INITIAL != instanceP->update_state ) || (gpackage_running == true))
        {
          ds_iot_ulog_err1("Wrong state [%d]", instanceP->update_state);
          return LWM2M_400_BAD_REQUEST;
        }
        if(lwm2m_strcmp(instanceP->pkg_name, (char *)dataArray[i].value.asBuffer.buffer) != 0)
        {
          val_change = true;
        }
		if(0 != (result = prv_write_init_resources(&lwm2m_data, &id_set, dataArray+i, LWM2M_SOFTWARE_MGNT_OBJECT_ID, instanceId, QAPI_NET_LWM2M_TYPE_OPAQUE_E)))
			return result;

        lwm2m_check_if_observe_and_update_app(id_set, LWM2M_SOFTWARE_MGNT_OBJECT_ID, instanceId, RES_O_PACKAGE, &lwm2m_data);
        lwm2m_free_instance_info(lwm2m_data.instance_info);
        instanceP->update_state  = PRV_UPDATE_STATE_DOWNLOAD_STARTED; 
        instanceP->update_result = PRV_UPDATE_RES_DOWNLOADING;
        val_change = TRUE;
        result = LWM2M_204_CHANGED;
        if (gInboundFirwareTransferComplete == true) {
          ds_iot_ulog_high ("last chunk received for package"); 
          instanceP->update_state  = PRV_UPDATE_STATE_DOWNLOADED;
          instanceP->update_result = PRV_UPDATE_RES_INITIAL;
          val_change = TRUE;
		  if(0 != (result = prv_init_resources(&lwm2m_data, &id_set, LWM2M_SOFTWARE_MGNT_OBJECT_ID, instanceId, RES_M_UPDATE_STATE )))
			return result;

		   lwm2m_data.instance_info->resource_info->type = (lwm2m_value_type_t)LWM2M_TYPE_INTEGER;
           lwm2m_data.instance_info->resource_info->value.asInteger = PRV_UPDATE_STATE_DOWNLOADED;
           lwm2m_check_if_observe_and_update_app(id_set, LWM2M_SOFTWARE_MGNT_OBJECT_ID, instanceId, RES_M_UPDATE_STATE, &lwm2m_data);
           lwm2m_free_instance_info(lwm2m_data.instance_info);
           result = LWM2M_204_CHANGED;        
        }

        break;
      case RES_O_PACKAGE_URI:
        if (PRV_UPDATE_STATE_INITIAL != instanceP->update_state)
        {
          ds_iot_ulog_err1("Wrong state [%d]", instanceP->update_state);
          return LWM2M_400_BAD_REQUEST;
        }
        if(lwm2m_strcmp(instanceP->package_uri, (char *)dataArray[i].value.asBuffer.buffer) != 0)
        {
          val_change = true;
        }
		if(0 != (result = prv_write_init_resources(&lwm2m_data, &id_set, dataArray+i, LWM2M_SOFTWARE_MGNT_OBJECT_ID, instanceId, QAPI_NET_LWM2M_TYPE_STRING_E)))
			return result;

        memscpy(instanceP->package_uri, PRV_MAX_STRSIZE,
              (const char *)lwm2m_data.instance_info->resource_info->value.asBuffer.buffer, 
              lwm2m_data.instance_info->resource_info->value.asBuffer.length);
        lwm2m_check_if_observe_and_update_app(id_set, LWM2M_SOFTWARE_MGNT_OBJECT_ID, instanceId, RES_O_PACKAGE_URI, &lwm2m_data);
        lwm2m_free_instance_info(lwm2m_data.instance_info);
        gpackage_running = true;                 
        result = LWM2M_204_CHANGED;
    
        break;
        /* commented the break to address the dead code, as this case is currently not supported 
           break; */
      case RES_O_USERNAME:
        if (PRV_MAX_STRSIZE < (dataArray + i)->value.asBuffer.length)
          return LWM2M_400_BAD_REQUEST;
        if(lwm2m_strcmp(instanceP->username, (char *)dataArray[i].value.asBuffer.buffer) != 0)
        {
          val_change = true;
        }
        LWM2M_STRCPY(instanceP->username, (const char *) ((dataArray + i)->value.asBuffer.buffer), 
            PRV_MAX_STRSIZE); 
        result = LWM2M_204_CHANGED;
        break;
      case RES_O_PASSWORD:
        if (PRV_MAX_STRSIZE < (dataArray + i)->value.asBuffer.length)
          return LWM2M_400_BAD_REQUEST;
        if(lwm2m_strcmp(instanceP->password, (char *)dataArray[i].value.asBuffer.buffer) != 0)
        {
          val_change = true;
        }
        LWM2M_STRCPY(instanceP->password, (const char *) ((dataArray + i)->value.asBuffer.buffer), 
            PRV_MAX_STRSIZE); 
        result = LWM2M_204_CHANGED;
        break;
        case RES_M_PKG_NAME:
        case RES_M_PKG_VERSION:
        case RES_M_INSTALL:
          //Uncomment below line when checkpoint resource is supported
          //case RES_O_CHECKPOINT:
        case RES_M_UNINSTALL:
        case RES_M_UPDATE_STATE:
        case RES_M_UPDATE_RESULT:
        case RES_M_ACTIVATE:
        case RES_M_DEACTIVATE:
        case RES_M_ACTIVATION_STATE:
          //Uncomment below line when package setting resource is supported
          //case RES_O_PACKAGE_SETTINGS:
        case RES_O_STATUS_REASON:
        /*If create/write have one more one resource to write we write on reablable resource*/
        if(numData > 1)
          result = LWM2M_204_CHANGED;
        else
          result = LWM2M_405_METHOD_NOT_ALLOWED;
          break;

      default:
        /*If create/write have one more one resource to write we write on unknown/notsupported resource*/
        if(numData > 1)
          result = LWM2M_204_CHANGED;
        else
          result = LWM2M_404_NOT_FOUND;
    }
    i++;
  } while (i < numData); // && result == LWM2M_204_CHANGED); commented to fix KW

  if (LWM2M_204_CHANGED == result && val_change == TRUE)
   store_software_mgnt_persistent_info(instanceP, LWM2M_TRUE);
  
  return result;

}

/**
 * @brief This fucntion implements the execute operation performed on software
 * management object
 * @param instanceId (IN) Instance ID of the object to be read
 * @param resourceId (IN) Resource ID of the resource to be executed
 * @param buffer (IN) message payload from server fro execute operation
 * @param length (IN) message payload length
 * @param objectP Object on which the operation needs to be executed
 * @return LWM2M response type
 */
static uint8_t prv_softwaremgnt_execute(uint16_t instanceId,
    uint16_t resourceId,
    uint8_t * buffer,
    int length,
    lwm2m_object_t * objectP)
{
  software_mgnt_t *instanceP = NULL;
  lwm2m_object_data_t lwm2m_data;
  uint8_t id_set = 0 ;
  uint8_t result = 0;
  if( objectP == NULL ) 
  {
    LOG_DEBUG("fun [%s] %s",__func__, nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  instanceP = (software_mgnt_t *)LWM2M_LIST_FIND(objectP->instanceList, instanceId);
  if (NULL == instanceP) return LWM2M_404_NOT_FOUND;

  switch(resourceId)
  {
    case RES_M_INSTALL:
      if (PRV_UPDATE_STATE_DELIVERED != instanceP->update_state)
      {
        return LWM2M_405_METHOD_NOT_ALLOWED; 
      }
  	  if(0 != (result = prv_init_resources(&lwm2m_data, &id_set, LWM2M_SOFTWARE_MGNT_OBJECT_ID, instanceId, RES_M_INSTALL )))
		return result;

      lwm2m_check_if_observe_and_update_app(id_set, LWM2M_SOFTWARE_MGNT_OBJECT_ID, instanceId, RES_M_INSTALL, &lwm2m_data);
      lwm2m_free_instance_info(lwm2m_data.instance_info);
      return LWM2M_204_CHANGED;      

    case RES_M_UNINSTALL:
      if (PRV_UPDATE_STATE_INSTALLED != instanceP->update_state  && PRV_UPDATE_STATE_DELIVERED != instanceP->update_state)
      {
        return LWM2M_405_METHOD_NOT_ALLOWED; 
      }
      else 
      {
        if (PRV_UPDATE_STATE_INSTALLED == instanceP->update_state)
        {
          if (length != 0 && buffer != NULL && buffer[0] == '1')
          {
            LOG_DEBUG(" PREPARING FOR UPDATE");
          }
          else //if (length != 0 && buffer != NULL && buffer[0] != '1')
          {
            LOG_DEBUG(" SOFTWARE UNINSTALL");
            /* TODO: Call package manager api to uninstall */
          }
        }
        else if (PRV_UPDATE_STATE_DELIVERED == instanceP->update_state)
        {
          LOG_DEBUG(" SOFTWARE PACKAGE REMOVED");
        }

        memset(instanceP->package, 0, PRV_MAX_STRSIZE);
		if(0 != (result = prv_init_resources(&lwm2m_data, &id_set, LWM2M_SOFTWARE_MGNT_OBJECT_ID, instanceId, RES_M_UNINSTALL )))
			return result;

        lwm2m_check_if_observe_and_update_app(id_set, LWM2M_SOFTWARE_MGNT_OBJECT_ID, instanceId, RES_M_UNINSTALL, &lwm2m_data);
        lwm2m_free_instance_info(lwm2m_data.instance_info);
        return LWM2M_204_CHANGED;        

      }

    case RES_M_ACTIVATE: 
      if ( (PRV_UPDATE_STATE_INSTALLED != instanceP->update_state )
          || (false != instanceP->activation_state) )
      {
        return LWM2M_405_METHOD_NOT_ALLOWED; 
      } 
      else 
      {
		if(0 != (result = prv_init_resources(&lwm2m_data, &id_set, LWM2M_SOFTWARE_MGNT_OBJECT_ID, instanceId, RES_M_ACTIVATE )))
			return result;      

        lwm2m_check_if_observe_and_update_app(id_set, LWM2M_SOFTWARE_MGNT_OBJECT_ID, instanceId, RES_M_ACTIVATE, &lwm2m_data);
        lwm2m_free_instance_info(lwm2m_data.instance_info);  
        return LWM2M_204_CHANGED;
            
      }
    case RES_M_DEACTIVATE:
      if (true != instanceP->activation_state)
      {
        return LWM2M_405_METHOD_NOT_ALLOWED; 
      } 
      else
      {
	  if(0 != (result = prv_init_resources(&lwm2m_data, &id_set, LWM2M_SOFTWARE_MGNT_OBJECT_ID, instanceId, RES_M_DEACTIVATE )))
        return result;

        lwm2m_check_if_observe_and_update_app(id_set, LWM2M_SOFTWARE_MGNT_OBJECT_ID, instanceId, RES_M_DEACTIVATE, &lwm2m_data);
        lwm2m_free_instance_info(lwm2m_data.instance_info);
        return LWM2M_204_CHANGED;        
      }
    case RES_M_PKG_NAME:
    case RES_M_PKG_VERSION:
    case RES_O_PACKAGE:
    case RES_O_PACKAGE_URI: 
      //Uncomment below line when checkpoint resource is supported
      //case RES_O_CHECKPOINT:
    case RES_M_UPDATE_STATE:
    case RES_M_UPDATE_RESULT:
    case RES_M_ACTIVATION_STATE:
          //Uncomment below line when package setting resource is supported
          //case RES_O_PACKAGE_SETTINGS:
    case RES_O_USERNAME:
    case RES_O_PASSWORD:
    case RES_O_STATUS_REASON:
      return LWM2M_405_METHOD_NOT_ALLOWED;

    default:
      return LWM2M_404_NOT_FOUND;         
  }
}

/*
 * @fn      static int32_t prv_softwaremgnt_getvalue ()
 * @brief   This function is used to  get the particular resource values for given instance 
 * @param   id_info      - indicates what values need to be filled whether resource or instance or object level 
 lwm2m_dataP  - pointer to lwm2m object information to be filled 
 *          targetP      - pointer to lwm2m object 
 * @return  on successs  -  0
 on failure   - -1
 */


int32_t prv_softwaremgnt_getvalue(lwm2m_id_info_t id_info, lwm2m_object_data_t **lwm2m_dataP , lwm2m_object_t * targetP,
                                              lwm2m_resource_info_t **resource_info)
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
  
  result = prv_softwaremgnt_read(instance_id, &numData, &dataArrayP, targetP,INVALID_RES_INST_ID, NULL);

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

/*
 * @fn      static int32_t prv_softwaremgnt_setvalue ()
 * @brief   This function is used to  set the particular resource values for given instance 
 * @param   lwm2m_data - pointer to lwm2m object information
 *          targetP    - pointer to lwm2m object 
 * @return  on success - 1
 on failure - 0
 */

static int32_t prv_softwaremgnt_setvalue (lwm2m_object_data_t * lwm2m_data, lwm2m_object_t * targetP)
{
  int result = 0;
  software_mgnt_t *instanceP = NULL;
  lwm2m_data_t *dataP = NULL;
  bool val_change = FALSE;

  if (lwm2m_data == NULL || targetP == NULL)
  {
    ds_iot_ulog_err("LWM2M_LOG: NULL Error");
    return 0;
  }
  LOG_DEBUG ("SOFTWARE MGNT SET CALL BACK GOT HIT  ");

  if((lwm2m_data->no_instances != 0x01)|| 
         (!lwm2m_data->instance_info) ||
         ((lwm2m_data->instance_info->no_resources) &&
         (!lwm2m_data->instance_info->resource_info)))
    return 0;
  
  instanceP = (software_mgnt_t *)LWM2M_LIST_FIND(targetP->instanceList, lwm2m_data->instance_info->instance_ID);

  if (NULL == instanceP) return 0;

  if(QAPI_OK != lwm2m_parse_application_object_info(lwm2m_data->instance_info,
                                                    lwm2m_data->instance_info->resource_info,
                                                    &dataP))
  {
    return 0;
  }
  switch (lwm2m_data->instance_info->resource_info->resource_ID) 
  {
    case RES_M_UPDATE_RESULT:
      {
        int64_t value = 0;
        if ((1 == lwm2m_data_decode_int(dataP , &value)) && (value>=0 && value<=200))
          {
          if(instanceP->update_result != value)
          {
            val_change = true;
          }
          instanceP->update_result = value;
          result = 1;
        }
        else
        {
          result = 0;
        }
      }
      break;
    case RES_M_UPDATE_STATE: 
      {
        int64_t value = 0;
        if ((1 == lwm2m_data_decode_int(dataP , &value)) &&  (value>=0 && value<=4))
        {
          if(instanceP->update_state != value)
          {
            val_change = true;
          }
          instanceP->update_state = value;
          if (instanceP->update_state == PRV_UPDATE_STATE_INITIAL || instanceP->update_state == PRV_UPDATE_STATE_DELIVERED ) {
            gpackage_running = false;
          }
          result = 1;
        }
        else
        {
          result = 0;
        }
      }
      break;
    case RES_M_ACTIVATION_STATE: 
      {
        bool value = 0;
        if (1 == lwm2m_data_decode_bool(dataP , &value))
        {
          if(instanceP->activation_state != value)
          {
            val_change = true;
          }
          instanceP->activation_state = value;
          result = 1;
        }
        else
        {
          result = 0;
        }
      } 
      break;

    default :
      LOG_DEBUG ("SOFTWARE MNGT IN default case set call back  ");  
      result = 0;
      break;
  }
  lwm2m_data_free_ext(lwm2m_data->instance_info->no_resources,dataP);

  if(result == 1 && val_change == TRUE)
  {
    resource_value_change(lwm2m_data);
    store_software_mgnt_persistent_info(instanceP, LWM2M_TRUE);
  }
  
  return result;

}

/**
 * @brief This function is used to get the information regarding the Software
 * management object during client registartion
 * @param void
 * @return Device capability management object  
 */
lwm2m_object_t * get_object_software_mgnt()
{

  lwm2m_object_t * softwaremgntObj = NULL;
  softwaremgntObj = (lwm2m_object_t *) lwm2m_malloc(sizeof(lwm2m_object_t));
  if(NULL != softwaremgntObj)
  {
    memset(softwaremgntObj, 0, sizeof(lwm2m_object_t));

    softwaremgntObj->objID  = LWM2M_SOFTWARE_MGNT_OBJECT_ID;
    softwaremgntObj->readFunc = prv_softwaremgnt_read;
    softwaremgntObj->discoverFunc = prv_softwaremgnt_discover;
    softwaremgntObj->writeFunc = prv_softwaremgnt_write;
    softwaremgntObj->executeFunc = prv_softwaremgnt_execute;
    softwaremgntObj->createFunc = prv_softwaremgnt_create;
    softwaremgntObj->deleteFunc = prv_softwaremgnt_delete;
    softwaremgntObj->setValueFunc = prv_softwaremgnt_setvalue;
    softwaremgntObj->get_Value_Func = prv_softwaremgnt_getvalue;  

  }

  return softwaremgntObj;
}

/**
 * @brief This fucntion frees the Software management object allocated
 * @param ObjectP (IN) Object to free
 * @return void
 */
void free_object_softwaremgnt(lwm2m_object_t * objectP)
{
  software_mgnt_t *instances = NULL;
  if( objectP == NULL ) 
  {
    LOG_DEBUG("fun [%s] %s",__func__, nullError);
    return ;
  }

  if(NULL != objectP->userData)
  {
    lwm2m_free(objectP->userData);
    objectP->userData = NULL;
  }
  instances = (software_mgnt_t *)objectP->instanceList;
  while(NULL != instances)
  {
    objectP->instanceList = (lwm2m_list_t *)instances->next;
	store_software_mgnt_persistent_info(instances, LWM2M_FALSE);
    lwm2m_free(instances);
    instances = (software_mgnt_t *)objectP->instanceList;
  }

  lwm2m_free(objectP);
}

#endif //ENABLE_SOFTWARE_MGNT_OBJ
