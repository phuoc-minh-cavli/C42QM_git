/******************************************************************************

  @file    bootstrap_info.c
  @brief   Interface File Containing Factory Bootstrapping Interfaces and DS
           This file implements a part of factory bootstraping. The file is adapted 
           from wakaama bootstrap server.
  ---------------------------------------------------------------------------
  Copyright (c) 2016-2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------

 *****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h> // isspace

#include "liblwm2m.h"
#include "object_access_control.h"
#include "object_security.h"
#include "object_server.h"
#include "object_apn_connection.h"
#include "lwm2mclient.h"
#include "bootstrap_info.h"
#include "fs_public.h"
#include "ds_log.h"


#ifdef FACTORY_BOOTSTRAPPING
extern int json_parse(lwm2m_uri_t * uriP, uint8_t * buffer, size_t bufferLen,
    lwm2m_data_t ** dataP);

extern bool enable_acl_obj;
extern bool enable_apn_conn_obj;

/**
 * @fn bool get_bootstrap_info()
 * @brief This function is used to get bootstrap information. This function 
 *        reads the config file and gets all the server accounts and creates 
 *        corresponding server and security object instances, also create 
 *        AC object instance for the same. First 4 bytes of the file contains
 *        the integer which tells the number of server accounts configured, 
 *        followed by the server account as defined by structure BS_Config_t.
 *
 * @param secObjP  handle to security object
 * @param serObjP  handle to server object
 * @param acObjP   handle to acces control object
 * @param apnObjP  handle to apn connection object
 * @param nInstances the number of ac object instance created. This is updated 
 *                   with the number of instance created in this function
 *
 * @return On success, return LWM2M_TRUE
 *         On failure, return LWM2M_FALSE
 */
bool get_bootstrap_info(lwm2m_object_t *secObjP, lwm2m_object_t *serObjP, lwm2m_object_t *acObjP, lwm2m_object_t *apnObjP)
{
  int fd = 0, len = 0;
  char *buffer = NULL, *bufferHoldPtr = NULL;


  /** NULL check */
  if (NULL == secObjP || NULL == serObjP)
    return LWM2M_FALSE;

#ifdef AUTHORIZATION_SUPPORT
  if( enable_acl_obj && NULL == acObjP)
  	return LWM2M_FALSE;
#endif
#ifdef ENABLE_APN_CONN_OBJ
  if( enable_apn_conn_obj && NULL == apnObjP)
  	return LWM2M_FALSE;
#endif  
  /** Read the configuration file and store the values */
  if (EFAILURE == (fd = efs_open(BOOTSTRAP_CFG_FILE, O_RDONLY))) 
  {
    ds_iot_ulog_err("Failed to open cfg file");
    return LWM2M_FALSE;
  }

  len = efs_lseek(fd, 0, SEEK_END);
  if (EFAILURE == len)
  {
    ds_iot_ulog_err("Failed to fetch fb file size");
    return LWM2M_FALSE;
  }
  if (EFAILURE == efs_lseek(fd, 0, SEEK_SET))
  {
    ds_iot_ulog_err("Failed to reset fb file pointer");
    return LWM2M_FALSE;
  }

  buffer = (char *)lwm2m_calloc(1, len + 1);
  if (NULL == buffer)
  {
    ds_iot_ulog_err("LWM2M_LOG: Memory Allocation FAILED");
    return LWM2M_FALSE;
  }
  bufferHoldPtr = buffer;

  if (EFAILURE == efs_read(fd, buffer, len))
  {
    lwm2m_free(buffer);
    buffer = NULL;
    efs_close(fd);
    return LWM2M_FALSE;
  }
  efs_close(fd);

  while (1)
  {
    char *tmp = NULL;
    lwm2m_data_t *dataP = NULL;
    int count = 0;

    // search for the delimiter
    tmp = strstr(buffer, "}]}");

    // If delimiter is not found, consider no more data available and terminate parsing and bootstrapping
    if (NULL == tmp)
    {
      break;
    }

    //incrementing to point after '}]}' 
    tmp += 3; 
  
    // Parse data from begining to delimiter
    count = json_parse(NULL, (uint8_t *)buffer, tmp - buffer, &dataP);
    if (0 == count || NULL == dataP)
    {
      ds_iot_ulog_err("Failed to parse factorybootstrap config");
      goto error;
    }
    if(dataP[0].id >= LWM2M_MAX_ID ||(!(dataP[0].value.asChildren.array) || (dataP[0].value.asChildren.array->id >= LWM2M_MAX_ID)))
    {
      lwm2m_data_free(count, dataP);
      goto error; 
    }
         
    //If Security object call create callback of Security Object and create instance
    if ((LWM2M_TYPE_OBJECT == dataP[0].type ) && (LWM2M_SECURITY_OBJECT_ID == dataP[0].id))
    {
      lwm2m_data_t *instP = dataP[0].value.asChildren.array;
  
      if (LWM2M_201_CREATED != secObjP->createFunc(instP->id, 
            instP->value.asChildren.count, instP->value.asChildren.array, 
            secObjP, TRUE))
      {
        ds_iot_ulog_err("failed to create SECURITY obj instance");
        lwm2m_data_free(count, dataP);
        goto error;
      }
    }

    //If Server object call create callback of Server Object and create instance
    if ((LWM2M_TYPE_OBJECT == dataP[0].type) && (LWM2M_SERVER_OBJECT_ID == dataP[0].id))
    {
      lwm2m_data_t *instP = dataP[0].value.asChildren.array;
  
      if (LWM2M_201_CREATED != serObjP->createFunc(instP->id, 
            instP->value.asChildren.count, instP->value.asChildren.array, 
            serObjP, TRUE))
      {
        ds_iot_ulog_err("failed to create SERVER obj instance");
        lwm2m_data_free(count, dataP);
        goto error;
      }      
    }
	
#ifdef AUTHORIZATION_SUPPORT
    if(enable_acl_obj)
    {
    //If ACL object call create callback of ACL Object and create instance
    if ((LWM2M_TYPE_OBJECT == (dataP[0].type)) && (LWM2M_ACL_OBJECT_ID == (dataP[0].id)))
    {
      lwm2m_data_t *instP = dataP[0].value.asChildren.array;
  
      if (LWM2M_201_CREATED != acObjP->createFunc(instP->id, 
            instP->value.asChildren.count, instP->value.asChildren.array, 
            acObjP, TRUE))
      {
        ds_iot_ulog_err("failed to create ACL obj instance");
        lwm2m_data_free(count, dataP);
        goto error;
      }      
    }
    }
#endif
#ifdef ENABLE_APN_CONN_OBJ
	if(enable_apn_conn_obj)
	{
    //If APN CONN object call create callback of APN Object and create instance
    if ((LWM2M_TYPE_OBJECT == (dataP[0].type)) && (LWM2M_APN_CONN_PROFILE_OBJECT_ID == (dataP[0].id)))
    {
      uint8_t result = LWM2M_IGNORE;
      lwm2m_data_t *instP = dataP[0].value.asChildren.array;

      result = apnObjP->createFunc(instP->id, 
            instP->value.asChildren.count, instP->value.asChildren.array, 
            apnObjP, TRUE);
      if (LWM2M_201_CREATED != result && LWM2M_NO_ERROR != result)
      {
        ds_iot_ulog_err("failed to create APN CONN obj instance");
        lwm2m_data_free(count, dataP);
        goto error;
      }      
    }
	}
#endif  
    lwm2m_data_free(count, dataP);
  
    //move to begining of next object instance
    buffer = tmp;
  }

  lwm2m_free(bufferHoldPtr);
  return LWM2M_TRUE;

error:
  lwm2m_free(bufferHoldPtr);
  clean_security_object(secObjP);
  clean_server_object(serObjP);
#ifdef AUTHORIZATION_SUPPORT
  if(enable_acl_obj)
  clean_acl_ctrl_object(acObjP);
#endif
#ifdef ENABLE_APN_CONN_OBJ
  if(enable_apn_conn_obj)
  clean_apn_conn_object(apnObjP);
#endif
  return LWM2M_FALSE;
}
#endif //FACTORY_BOOTSTRAPPING

