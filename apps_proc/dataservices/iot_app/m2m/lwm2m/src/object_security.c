/******************************************************************************

  @file    object_security.c

  ---------------------------------------------------------------------------

  Copyright (c) 2016-2023 Qualcomm Technologies, Inc.
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
 *    David Navarro, Intel Corporation - initial API and implementation
 *    Bosch Software Innovations GmbH - Please refer to git log
 *    Pascal Rieux - Please refer to git log
 *    
 *******************************************************************************/

/*
 *  Resources:
 *
 *          Name            | ID | Operations | Instances | Mandatory |  Type   |  Range  | Units |
 *  Server URI              |  0 |            |  Single   |    Yes    | String  |         |       |
 *  Bootstrap Server        |  1 |            |  Single   |    Yes    | Boolean |         |       |
 *  Security Mode           |  2 |            |  Single   |    Yes    | Integer |   0-3   |       |
 *  Public Key or ID        |  3 |            |  Single   |    Yes    | Opaque  |         |       |
 *  Server Public Key or ID |  4 |            |  Single   |    Yes    | Opaque  |         |       |
 *  Secret Key              |  5 |            |  Single   |    Yes    | Opaque  |         |       |
 *  SMS Security Mode       |  6 |            |  Single   |    Yes    | Integer |  0-255  |       |
 *  SMS Binding Key Param.  |  7 |            |  Single   |    Yes    | Opaque  |   6 B   |       |
 *  SMS Binding Secret Keys |  8 |            |  Single   |    Yes    | Opaque  | 32-48 B |       |
 *  Server SMS Number       |  9 |            |  Single   |    Yes    | Integer |         |       |
 *  Short Server ID         | 10 |            |  Single   |    No     | Integer | 1-65535 |       |
 *  Client Hold Off Time    | 11 |            |  Single   |    Yes    | Integer |         |   s   |
 *
 */

/*
 * Here we implement a very basic LWM2M Security Object which only knows NoSec security mode.
 */

#include "liblwm2m.h"
#include "lwm2mclient.h"

#include "object_security.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "log_utils.h"
#include "qapi_coap.h"
#include "ds_log.h"



#define LWM2M_SECURITY_URI_ID                 0
#define LWM2M_SECURITY_BOOTSTRAP_ID           1
#define LWM2M_SECURITY_MODE_ID                2
#define LWM2M_SECURITY_PUBLIC_KEY_ID          3
#define LWM2M_SECURITY_SERVER_PUBLIC_KEY_ID   4
#define LWM2M_SECURITY_SECRET_KEY_ID          5
#define LWM2M_SECURITY_SMS_SECURITY_ID        6
#define LWM2M_SECURITY_SMS_KEY_PARAM_ID       7
#define LWM2M_SECURITY_SMS_SECRET_KEY_ID      8
#define LWM2M_SECURITY_SMS_SERVER_NUMBER_ID   9
#define LWM2M_SECURITY_SHORT_SERVER_ID        10
#define LWM2M_SECURITY_HOLD_OFF_ID            11
#define LWM2M_SECURITY_RESOURCE_30000         30000

#define MAX_KEY_LEN 128
#define MAX_URI_LEN 128
#define PERSISTENCE_MAGIC_WORD_SIZE 16

/*magic word is used to check whether the persistent file version match with code, 
  func load_security_persistent_info will detect whether magic word in persistence file is different with code, 
  if different then reset the persistence file */
/* magic workd = M2M_Pr_SObj_V12 */
char g_lwm2m_persstence_magic_word[PERSISTENCE_MAGIC_WORD_SIZE] =
{
    0x4d, 0x32, 0x4d, 0x5f, 0x50, 0x72, 0x5f, 0x53, 0x4f, 0x62, 0x6a, 0x5f, 0x56, 0x31, 0x32, 0x00
};

extern bool g_carrier_cfg;  // flag to check  carrier config is enabled or not 
extern bool persis_bootstrap_feat_enabled;

static bool create_operation = LWM2M_FALSE;

typedef struct _security_persistent_info_
{

  char            magicWord[PERSISTENCE_MAGIC_WORD_SIZE];
  char            clientCertFName[MAX_CERT_NAME_LEN];
  char            serverCertFName[MAX_CERT_NAME_LEN];
  char            uri[MAX_URI_LEN];
  uint16_t        instanceId; 
  uint16_t        shortID;
  uint16_t        secretKeyLen;
  uint16_t        serverPublicKeyLen;
  uint16_t        publicIdLen;
  uint16_t        uriLen;
  bool            isBootstrap;
  bool            isValid;
  uint8_t         securityMode;
  uint16_t        clientHoldOffTime;
  char            sms_number[MAX_SMS_NUMBER];
  char            sms_number_len;
  pers_resource_instance_int_list_t custom30000List[MAX_CUST_30000_RES_INSTANCES];
  cert_source_enum	cert_source;

} security_persistent_info_t;

int load_security_persistent_info(lwm2m_object_t *secObjInstP)
{
  int fd = 0 , ret = 0; 

  security_instance_t *targetP = NULL;
  security_persistent_info_t info;
#if defined (LWM2M_BOOTSTRAP)
  resource_instance_int_list_t *resList = NULL;
  int index = 0;
#endif

  if (NULL == secObjInstP)
  {
    LOG_DEBUG("%s",nullError);
    return EFAILURE;
  }
  ds_iot_log_strf_med("EFS_OPT: File open %s",SECURITY_PERSISTENCE_FILE);
  fd = efs_open(SECURITY_PERSISTENCE_FILE, 00);
  if (EFAILURE == fd)
  {
    ds_iot_ulog_err("failed to open security persistence file ");
    return EFAILURE;
  }

  while (1)
  {
    memset(&info, 0x00, sizeof(info));
    targetP = NULL;

    ret = efs_read(fd, &info, sizeof(info));
    if (ret <= 0)
    {
      break;
    }
	
	/*Check if the structure entry in file is valid*/
	if (LWM2M_FALSE == info.isValid)
    {
      continue;
    }

    /*Added new magic word with file version. If the version mismatch, then it would do the factory reset. 
      This option would take care of previous files compatibitly issue */
    if (strncmp(info.magicWord, g_lwm2m_persstence_magic_word,  PERSISTENCE_MAGIC_WORD_SIZE) != 0) {
      ds_iot_ulog_err("Security persistence doesn't match. So resetting persistence files");
      clean_security_object(secObjInstP);
      efs_close(fd);
      purge_persistent_values();
      return EFAILURE;
    }

    targetP = lwm2m_calloc(1, sizeof(security_instance_t));
    if (NULL == targetP)
    {
      ret = -1;
      break;
    }
    if (0 != info.uriLen)
    {
      targetP->uri = lwm2m_calloc(1, info.uriLen + 1);
      if (NULL == targetP->uri)
      {
        ret = -1;
        break;
      }
      strlcpy(targetP->uri, info.uri, info.uriLen + 1);
    }

    if (0 != info.sms_number_len)
    {
      targetP->sms_number = lwm2m_calloc(1, info.sms_number_len + 1);
      if (NULL == targetP->sms_number)
      {
        ret = -1;
        break;
      }
      strlcpy(targetP->sms_number, info.sms_number, info.sms_number_len + 1);
    }

    /* copieing the secure certificate file name. So that It will be loaded during SSL connect 
       Other secure certificate/PSK parameter are removed from persistence file due to security risk*/
    if ((NULL != targetP->clientCertFName) && (strlen(info.clientCertFName) > 0))
    {
      strlcpy(targetP->clientCertFName, info.clientCertFName, MAX_CERT_NAME_LEN);
    }
    if ((NULL != targetP->serverCertFName) && (strlen(info.serverCertFName) > 0))
    {
      strlcpy(targetP->serverCertFName, info.serverCertFName, MAX_CERT_NAME_LEN);
    }

    targetP->instanceId = info.instanceId;
    targetP->shortID = info.shortID;
    targetP->isBootstrap = info.isBootstrap;
    targetP->securityMode = info.securityMode;
	targetP->cert_source = info.cert_source;

#if defined (LWM2M_BOOTSTRAP)
    if(persis_bootstrap_feat_enabled) {
      for (index = 0; index < MAX_CUST_30000_RES_INSTANCES; index++)
      {
        resList = NULL;
        if (LWM2M_FALSE == info.custom30000List[index].isValid)
          continue;
        resList = lwm2m_calloc(1, sizeof(resource_instance_int_list_t));
        if (NULL == resList)
        {
          ret = -1;
          break;
        }
        resList->resInstId = info.custom30000List[index].resInstId;
        resList->InstValue= info.custom30000List[index].InstValue;
        targetP->custom30000List = (resource_instance_int_list_t *)LWM2M_LIST_ADD(targetP->custom30000List, resList);
      }
    }
#endif
    targetP->clientHoldOffTime = info.clientHoldOffTime;

    secObjInstP->instanceList = LWM2M_LIST_ADD(secObjInstP->instanceList, targetP);
  }
  if (ret < 0)
  {
    clean_security_object(secObjInstP);
    efs_close(fd);
    return EFAILURE;
  }
  efs_close(fd);
  return ESUCCESS;
}

int store_security_persistent_info(security_instance_t *secInstanceP, boolean store)
{
  int fd = 0; 
  security_persistent_info_t info;
#if defined (LWM2M_BOOTSTRAP)
  int index = 0;
  resource_instance_int_list_t *resList;
#endif 
  if (NULL == secInstanceP)
  {
    LOG_DEBUG("%s",nullError);
    return EFAILURE;
  }
  ds_iot_log_strf_med("EFS_OPT: File open %s",SECURITY_PERSISTENCE_FILE);
  fd = efs_open(SECURITY_PERSISTENCE_FILE, O_CREAT | O_WRONLY, 00644);
  if (EFAILURE == fd)
  {
    ds_iot_ulog_err("failed to open security persistence file");
    return EFAILURE;
  }

  memset(&info, 0x00, sizeof(info));

  /*Coping the secure word with version# for handling pervious/future version of the same file */
  strlcpy(info.magicWord, g_lwm2m_persstence_magic_word,  PERSISTENCE_MAGIC_WORD_SIZE);

  if (NULL != secInstanceP->uri)
  {
    strlcpy(info.uri, secInstanceP->uri, MAX_URI_LEN);
    info.uriLen = strlen(secInstanceP->uri);
  }

  if (NULL != secInstanceP->sms_number)
  {
    strlcpy(info.sms_number, secInstanceP->sms_number, MAX_SMS_NUMBER);
    info.sms_number_len = strlen(secInstanceP->sms_number);
  }

  /* Storing the PSK or certificates into secure location and remove the data
     from the structue. */
  if((secInstanceP->securityMode == LWM2M_SECURITY_MODE_PRE_SHARED_KEY) || 
     (secInstanceP->securityMode == LWM2M_SECURITY_MODE_CERTIFICATE))
  {
    bool bRootCAMode = FALSE;
    if(((secInstanceP->isBootstrap == true) && 
		 ((data.cfg_info.lwm2m_rootca_support_mode == LWM2M_ROOTCA_SUPPORT_BS_SERVERS) || 
		 (data.cfg_info.lwm2m_rootca_support_mode == LWM2M_ROOTCA_SUPPORT_BS_LWM2M_SERVERS))) ||
		((secInstanceP->isBootstrap == false) && 
          ((data.cfg_info.lwm2m_rootca_support_mode == LWM2M_ROOTCA_SUPPORT_LWM2M_SERVERS) || 
		  (data.cfg_info.lwm2m_rootca_support_mode == LWM2M_ROOTCA_SUPPORT_BS_LWM2M_SERVERS))))
    {
      bRootCAMode = TRUE;
    }

    /* lwm2m_ssl_security_store store the cert info in to secure location. lwm2m_ssl_security_store
       would be called in four scenarios.
       1. clientCertFName empty, for factory bootstrapping or factory reset
       2. serverCertFName empty and Not RootCA-mode, for factory bootstrapping or factory reset
       3. Client initiated bootstrapping either client certficate or 4. server certficate change.
       */
    if ( (strlen(secInstanceP->clientCertFName) == 0) || 
		 ((strlen(secInstanceP->serverCertFName) == 0) && (bRootCAMode == false)) ||
         ((NULL != secInstanceP->secretKey)       && (secInstanceP->secretKeyLen > 0) && 
          (NULL != secInstanceP->publicIdentity)  && (secInstanceP->publicIdLen > 0)) ||
         ((NULL != secInstanceP->serverPublicKey) && (secInstanceP->serverPublicKeyLen > 0)))
    {
       if(get_lwm2m_context() != NULL)
       {
         if(lwm2m_ssl_security_store(secInstanceP, secInstanceP->shortID) != QAPI_OK)
         {
           ds_iot_ulog_err("lwm2m_ssl_security_store failed ");
         }
		 /*store_security_persistent_info maybe be called multitimes, 
		   for example, called from update_isbootstrap_feild_for_security_instance which to update only isbootstrap field
		   so we can update cert_source only once*/
		 else if(secInstanceP->cert_source != CERT_SOURCE_NETWORK)
		 {		 		   
           if((secInstanceP->publicIdentity != NULL) && (secInstanceP->publicIdLen > 0) && 
		   	  (secInstanceP->secretKey != NULL)      && (secInstanceP->secretKeyLen > 0))
           {
			 ds_iot_ulog_high("cert stored from network(client initiated bootstrap)");
			 secInstanceP->cert_source = CERT_SOURCE_NETWORK;
           }
		   else
		   {
			 ds_iot_ulog_high("cert stored from local preloaded");
			 secInstanceP->cert_source = CERT_SOURCE_PRELOAD;	 
		   }
		 }
       }

    }
    
    /* cleaning up client or server certficate fields in the sec object due to security concern*/  
    if ((NULL != secInstanceP->secretKey) && (secInstanceP->publicIdLen > 0) 
        && (NULL != secInstanceP->publicIdentity) && (secInstanceP->secretKeyLen > 0) ) 
    {
      ds_iot_ulog_high("Freeing client security credentials after succesfully stored in secure location");
      secInstanceP->publicIdLen = 0;
      secInstanceP->secretKeyLen = 0;
      lwm2m_free(secInstanceP->publicIdentity);
      secInstanceP->publicIdentity = NULL;
      lwm2m_free(secInstanceP->secretKey);
      secInstanceP->secretKey = NULL;
    }
    if ((NULL != secInstanceP->serverPublicKey ) && (secInstanceP->serverPublicKeyLen > 0))  
    {
      ds_iot_ulog_high("Freeing server security credentials after succesfully stored in secure location");
      secInstanceP->serverPublicKeyLen = 0;
      lwm2m_free(secInstanceP->serverPublicKey);
      secInstanceP->serverPublicKey = NULL;
    }
  }

  /* certificate need to write always in sec object*/
  if (strlen(secInstanceP->clientCertFName) != 0)
  {
    strlcpy(info.clientCertFName, secInstanceP->clientCertFName, MAX_CERT_NAME_LEN);
  }

  if (strlen(secInstanceP->serverCertFName) != 0)
  {
    strlcpy(info.serverCertFName, secInstanceP->serverCertFName, MAX_CERT_NAME_LEN);
  }
  ds_iot_log_strf_high("Certificate/psk file name:%s Server_cert Name:%s", info.clientCertFName, info.serverCertFName);


  info.instanceId = secInstanceP->instanceId;
  info.shortID = secInstanceP->shortID;
  info.isBootstrap = secInstanceP->isBootstrap;
  info.securityMode = secInstanceP->securityMode;
  info.cert_source  = secInstanceP->cert_source;
#if defined (LWM2M_BOOTSTRAP)
  if(persis_bootstrap_feat_enabled) {
    resList = secInstanceP->custom30000List;
    while (resList)
    {
      info.custom30000List[index].isValid = LWM2M_TRUE;
      info.custom30000List[index].resInstId = resList->resInstId;
      info.custom30000List[index++].InstValue = resList->InstValue;
      resList = resList->next;
    }
    while (index < MAX_CUST_30000_RES_INSTANCES)
    {
      info.custom30000List[index++].isValid = LWM2M_FALSE;
    }
  }
#endif
  info.clientHoldOffTime = secInstanceP->clientHoldOffTime; 

  if(store)
  {
    info.isValid = LWM2M_TRUE; 
  }
  else
  {
    info.isValid = LWM2M_FALSE; 
  }

  efs_lseek(fd, info.instanceId * sizeof(info), SEEK_SET);
  ds_iot_log_strf_med("EFS_OPT: File write %s",SECURITY_PERSISTENCE_FILE);
  if (EFAILURE == efs_write(fd, &info, sizeof(info)))
  {
    ds_iot_ulog_err1("failed to store persistent value of security instance %u", info.instanceId);
    efs_close(fd);
    return EFAILURE;
  }
  efs_close(fd);
  return ESUCCESS;
}

/**
 * @fn static uint8_t prv_get_value()
 *
 * @brief This fucntion is used to populate the values of the resource
 *        to be provided to the server during the read operation on the object.
 * @param dataP data array for the resource 
 * @param targetP Security object data
 * @return LWM2M response type
 */
static uint8_t prv_get_value(lwm2m_data_t * dataP,
    security_instance_t * targetP, uint16_t resInstId)
{
  if( dataP == NULL || targetP == NULL ) 
  {
    LOG_DEBUG("fun [%s] file [%s]  %s",__func__,__FILE__, nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }
  switch (dataP->id)
  {
    case LWM2M_SECURITY_URI_ID:
      lwm2m_data_encode_string(targetP->uri, dataP);
      return LWM2M_205_CONTENT;

    case LWM2M_SECURITY_BOOTSTRAP_ID:
      lwm2m_data_encode_bool(targetP->isBootstrap, dataP);
      return LWM2M_205_CONTENT;

    case LWM2M_SECURITY_SECURITY_ID:
      lwm2m_data_encode_int(targetP->securityMode, dataP);
      return LWM2M_205_CONTENT;

    case LWM2M_SECURITY_PUBLIC_KEY_ID:
      lwm2m_data_encode_opaque((uint8_t *)targetP->publicIdentity, targetP->publicIdLen, dataP);
      return LWM2M_205_CONTENT;

    case LWM2M_SECURITY_SERVER_PUBLIC_KEY_ID:
      lwm2m_data_encode_opaque((uint8_t *)targetP->serverPublicKey, targetP->serverPublicKeyLen, dataP);
      return LWM2M_205_CONTENT;

    case LWM2M_SECURITY_SECRET_KEY_ID:
      lwm2m_data_encode_opaque((uint8_t *)targetP->secretKey, targetP->secretKeyLen, dataP);
      return LWM2M_205_CONTENT;

    case LWM2M_SECURITY_SMS_SECURITY_ID:
      lwm2m_data_encode_int(targetP->smsSecurityMode, dataP);
      return LWM2M_205_CONTENT;

    case LWM2M_SECURITY_SMS_KEY_PARAM_ID:
      lwm2m_data_encode_opaque((uint8_t *)targetP->smsParams, targetP->smsParamsLen, dataP);
      return LWM2M_205_CONTENT;

    case LWM2M_SECURITY_SMS_SECRET_KEY_ID:
      lwm2m_data_encode_opaque((uint8_t *)targetP->smsSecret, targetP->smsSecretLen, dataP);
      return LWM2M_205_CONTENT;

    case LWM2M_SECURITY_SMS_SERVER_NUMBER_ID:
      lwm2m_data_encode_int(0, dataP);
      return LWM2M_205_CONTENT;

    case LWM2M_SECURITY_SHORT_SERVER_ID:
      lwm2m_data_encode_int(targetP->shortID, dataP);
      return LWM2M_205_CONTENT;
    case LWM2M_SECURITY_HOLD_OFF_ID:
      lwm2m_data_encode_int(targetP->clientHoldOffTime, dataP);
      return LWM2M_205_CONTENT;

#if defined (LWM2M_BOOTSTRAP)  
    case LWM2M_SECURITY_RESOURCE_30000 :
      {
        if (g_carrier_cfg) 
        {
          if (resInstId != INVALID_RES_INST_ID)
          {
            resource_instance_int_list_t *resInst = (resource_instance_int_list_t *)LWM2M_LIST_FIND(targetP->custom30000List,resInstId);
            if(resInst != NULL)
            {
              lwm2m_data_encode_int(resInst->InstValue, dataP);
              return LWM2M_205_CONTENT;
            }
            else
              return LWM2M_404_NOT_FOUND;
          }
          else {
            int ri = 0;
            resource_instance_int_list_t* custom30000RI = NULL;
            for (custom30000RI = targetP->custom30000List, ri=0;
                custom30000RI!=NULL;
                custom30000RI = custom30000RI->next, ri++);

            if (ri==0)  // no values!
            {
              return LWM2M_205_CONTENT;
            }
            else
            {
              lwm2m_data_t* subTlvP = lwm2m_data_new(ri);
              if (NULL == subTlvP) return LWM2M_500_INTERNAL_SERVER_ERROR;
              for (custom30000RI = targetP->custom30000List, ri = 0;
                  custom30000RI!= NULL;
                  custom30000RI = custom30000RI->next, ri++)
              {
                subTlvP[ri].id = (uint16_t)custom30000RI->resInstId;

                lwm2m_data_encode_int(custom30000RI->InstValue, &subTlvP[ri]);
              }
              lwm2m_data_encode_instances(subTlvP, ri, dataP);
              return LWM2M_205_CONTENT;
            }
          }
        }
      }
#endif
    default:
      return LWM2M_404_NOT_FOUND;
  }
}

/**
 * @fn static uint8_t prv_security_read()
 *
 * @brief This function implements the read operation performed on 
 *        Security Object
 *
 * @param instanceId Instance ID of the object to be read
 * @param numDataP number of resources in the array
 * @param dataArrayP resource value array
 * @param objectP Object to be read
 *
 * @return LWM2M response type
 */
static uint8_t prv_security_read(uint16_t instanceId,
    int * numDataP,
    lwm2m_data_t ** dataArrayP,
    lwm2m_object_t * objectP,
    uint16_t resInstId, lwm2m_server_t *serverP)
{

  uint8_t result = 0;
  int i = 0, ri = 0;
#if defined (LWM2M_BOOTSTRAP)
  resource_instance_int_list_t *custom30000RI;
#endif
  security_instance_t * targetP = NULL;
  int nbRes = 0;
  int temp;
  if( numDataP == NULL || dataArrayP == NULL || objectP == NULL ) 
  {
    ds_iot_ulog_err("fun [prv_security_read]: LWM2M_LOG: NULL Error");
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }     

  targetP = (security_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);
  if (NULL == targetP) return LWM2M_404_NOT_FOUND;

  // is the server asking for the full instance ?
  if (*numDataP == 0)
  {
    uint16_t resList[] = {LWM2M_SECURITY_URI_ID,
      LWM2M_SECURITY_BOOTSTRAP_ID,
      LWM2M_SECURITY_SECURITY_ID,
      LWM2M_SECURITY_PUBLIC_KEY_ID,
      LWM2M_SECURITY_SERVER_PUBLIC_KEY_ID,
      LWM2M_SECURITY_SECRET_KEY_ID,
      LWM2M_SECURITY_SMS_SECURITY_ID,
      LWM2M_SECURITY_SMS_KEY_PARAM_ID,
      LWM2M_SECURITY_SMS_SECRET_KEY_ID,
      LWM2M_SECURITY_SMS_SERVER_NUMBER_ID,
      LWM2M_SECURITY_SHORT_SERVER_ID,
      LWM2M_SECURITY_HOLD_OFF_ID 
    };
    nbRes = sizeof(resList)/sizeof(uint16_t);
    temp = nbRes;
    if(g_carrier_cfg) {
#if defined (LWM2M_BOOTSTRAP)
      for (custom30000RI = targetP->custom30000List, ri=0;
          custom30000RI!=NULL;
          custom30000RI = custom30000RI->next, ri++);

      if (ri!=0)  // no values!
      {
        nbRes++;
      }
#endif
    }

    *dataArrayP = lwm2m_data_new(nbRes);
    if (NULL == *dataArrayP) return LWM2M_500_INTERNAL_SERVER_ERROR;

    *numDataP = nbRes;

    if (*dataArrayP == NULL) return LWM2M_500_INTERNAL_SERVER_ERROR;

    for (i = 0 ; i < temp ; i++)
    {
      (*dataArrayP)[i].id = resList[i];
    }

    if ((g_carrier_cfg) && (ri!=0))  // no values!
    {
      (*dataArrayP)[temp].id = LWM2M_SECURITY_RESOURCE_30000 ;
    }
  }

  i = 0;
  do
  {
    result = prv_get_value((*dataArrayP) + i, targetP, resInstId);
    i++;
  } while (i < *numDataP && result == LWM2M_205_CONTENT);

  return result;
}

/**
 * @fn static uint8_t prv_security_write()
 *
 * @brief This fucntion implements the write operation performed on 
 *        Security object
 *
 * @param instanceId Instance ID of the object to be read
 * @param numData number of resources in the array
 * @param dataArray resource value array
 * @param objectP Object to be read
 *
 * @return LWM2M response type
 */
static uint8_t prv_security_write(uint16_t instanceId,
    int numData,
    lwm2m_data_t * dataArray,
    lwm2m_object_t * objectP,
    uint16_t resInstId, uint8_t write_method, bool bs_true)
{
  security_instance_t * targetP = NULL;
  int i;
  uint8_t result = LWM2M_204_CHANGED;
  bool val_change = false;

  if( dataArray == NULL || objectP == NULL ) 
  {
    ds_iot_ulog_err("fun [prv_security_write]:LWM2M_LOG: NULL Error");
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  targetP = (security_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);
  if (NULL == targetP)
  {
    return LWM2M_404_NOT_FOUND;
  }

  i = 0;
  do {
    switch (dataArray[i].id)
    {
      case LWM2M_SECURITY_URI_ID:
        {
          if(lwm2m_strcmp(targetP->uri, (char *)dataArray[i].value.asBuffer.buffer) != 0)
          {
            val_change = true;
          }
          if (targetP->uri != NULL) lwm2m_free(targetP->uri);
          if(dataArray[i].value.asBuffer.buffer != NULL && dataArray[i].value.asBuffer.length > 0)
          {
            targetP->uri = (char *)lwm2m_malloc(dataArray[i].value.asBuffer.length + 1);
            if (targetP->uri != NULL)
            {
              memset(targetP->uri, 0, dataArray[i].value.asBuffer.length + 1);
              strlcpy(targetP->uri, (char*)dataArray[i].value.asBuffer.buffer, dataArray[i].value.asBuffer.length + 1);
              result = LWM2M_204_CHANGED;
            }
            else
            {
              result = LWM2M_500_INTERNAL_SERVER_ERROR;
            }
          }
          else
          {
            targetP->uri = NULL;
            result = LWM2M_204_CHANGED;
          }
        }
        break;

      case LWM2M_SECURITY_BOOTSTRAP_ID:
        {
          bool value = targetP->isBootstrap;
          if (1 == lwm2m_data_decode_bool(dataArray + i, &(targetP->isBootstrap)))
          {
            if(targetP->isBootstrap != value)
            {
              val_change = true;
            }
            result = LWM2M_204_CHANGED;
          }
          else
          {
            result = LWM2M_400_BAD_REQUEST;
          }
#if defined (LWM2M_BOOTSTRAP)
          if(targetP->isBootstrap == true)
          {
            prv_write_resource_inst_val(&(targetP->custom30000List),LWM2M_SECURITY_RESOURCE_INST_IS_HOLD_OFF_TIMER, HOLD_OFF_TIMER_DEFAULT, &val_change);
          }
#endif
        }
        break;

      case LWM2M_SECURITY_SECURITY_ID:
        {
          int64_t value;

          if (1 == lwm2m_data_decode_int(dataArray + i, &value))
          {
            if (value >= 0 && value <= 3)
            {
              if(targetP->securityMode != value)
              {
                val_change = true;
              }
              targetP->securityMode = value;
              result = LWM2M_204_CHANGED;
            }
            else
            {
              result = LWM2M_400_BAD_REQUEST;
            }
          }
          else
          {
            result = LWM2M_400_BAD_REQUEST;
          }
        }
        break;
      case LWM2M_SECURITY_PUBLIC_KEY_ID:
        if(lwm2m_strcmp(targetP->publicIdentity, (char *)dataArray[i].value.asBuffer.buffer) != 0)
        {
          val_change = true;
        }
        if (targetP->publicIdentity != NULL) lwm2m_free(targetP->publicIdentity);
        targetP->publicIdentity = (char *)lwm2m_malloc(dataArray[i].value.asBuffer.length +1);
        if (targetP->publicIdentity  == NULL) 
        {
          result = LWM2M_500_INTERNAL_SERVER_ERROR;
        } 
        else
        {        
          memset(targetP->publicIdentity, 0, dataArray[i].value.asBuffer.length + 1);
          LWM2M_MEMCPY(targetP->publicIdentity, dataArray[i].value.asBuffer.length +1, 
              (char*)dataArray[i].value.asBuffer.buffer, dataArray[i].value.asBuffer.length);
          targetP->publicIdLen = dataArray[i].value.asBuffer.length;
          result = LWM2M_204_CHANGED;

        }
        break;

      case LWM2M_SECURITY_SERVER_PUBLIC_KEY_ID:
        if(lwm2m_strcmp(targetP->serverPublicKey, (char *)dataArray[i].value.asBuffer.buffer) != 0)
        {
          val_change = true;
        }
        if (targetP->serverPublicKey != NULL) lwm2m_free(targetP->serverPublicKey);
        targetP->serverPublicKey = (char *)lwm2m_malloc(dataArray[i].value.asBuffer.length +1);
        if (targetP->serverPublicKey == NULL)
        {
          result = LWM2M_500_INTERNAL_SERVER_ERROR;
        }
        else 
        {
          memset(targetP->serverPublicKey, 0, dataArray[i].value.asBuffer.length + 1);
          LWM2M_MEMCPY(targetP->serverPublicKey, dataArray[i].value.asBuffer.length +1, 
              (char*)dataArray[i].value.asBuffer.buffer, dataArray[i].value.asBuffer.length);
          targetP->serverPublicKeyLen = dataArray[i].value.asBuffer.length;
          result = LWM2M_204_CHANGED;
        }
        break;

      case LWM2M_SECURITY_SECRET_KEY_ID:
        if(lwm2m_strcmp(targetP->secretKey, (char *)dataArray[i].value.asBuffer.buffer) != 0)
        {
          val_change = true;
        }
        if (targetP->secretKey != NULL) lwm2m_free(targetP->secretKey);
        targetP->secretKey = (char *)lwm2m_malloc(dataArray[i].value.asBuffer.length +1);
        if (targetP->secretKey  == NULL )
        {
          result = LWM2M_500_INTERNAL_SERVER_ERROR;
        } 
        else
        {  
          memset(targetP->secretKey, 0, dataArray[i].value.asBuffer.length + 1);
          LWM2M_MEMCPY(targetP->secretKey, dataArray[i].value.asBuffer.length +1, 
              (char*)dataArray[i].value.asBuffer.buffer, dataArray[i].value.asBuffer.length);
          targetP->secretKeyLen = dataArray[i].value.asBuffer.length;
          result = LWM2M_204_CHANGED;  
        }
        break;

      case LWM2M_SECURITY_SMS_SECURITY_ID:
        // Let just ignore this
        result = LWM2M_204_CHANGED;
        break;

      case LWM2M_SECURITY_SMS_KEY_PARAM_ID:
        // Let just ignore this
        result = LWM2M_204_CHANGED;
        break;

      case LWM2M_SECURITY_SMS_SECRET_KEY_ID:
        // Let just ignore this
        result = LWM2M_204_CHANGED;
        break;

      case LWM2M_SECURITY_SMS_SERVER_NUMBER_ID:
        {
          if(lwm2m_strcmp(targetP->sms_number, (char *)dataArray[i].value.asBuffer.buffer) != 0)
          {
            val_change = true;
          }
          if (targetP->sms_number!= NULL) lwm2m_free(targetP->sms_number);
          if(dataArray[i].value.asBuffer.buffer != NULL && dataArray[i].value.asBuffer.length > 0)
          {
            targetP->sms_number = (char *)lwm2m_malloc(dataArray[i].value.asBuffer.length + 1);
            if (targetP->sms_number != NULL)
            {
              memset(targetP->sms_number, 0, dataArray[i].value.asBuffer.length + 1);
              strlcpy(targetP->sms_number, (char*)dataArray[i].value.asBuffer.buffer, dataArray[i].value.asBuffer.length + 1);
              result = LWM2M_204_CHANGED;
            }
            else
            {
              result = LWM2M_500_INTERNAL_SERVER_ERROR;
            }
          }
          else
          {
            targetP->sms_number = NULL;
            result = LWM2M_204_CHANGED;
          }
        }
        break;

      case LWM2M_SECURITY_SHORT_SERVER_ID:
        {
          int64_t value;

          if (1 == lwm2m_data_decode_int(dataArray + i, &value))
          {
            if (value >= 0 && value <= 0xFFFF)
            {
              if(targetP->shortID != value)
              {
                val_change = true;
              }
              targetP->shortID = value;
              result = LWM2M_204_CHANGED;
              ds_iot_ulog_high1("fun [prv_security_write]  short server id is %d", value);
            }
            else
            {
              result = LWM2M_400_BAD_REQUEST;
            }
          }
          else
          {
            result = LWM2M_400_BAD_REQUEST;
          }
        }
        break;

      case LWM2M_SECURITY_HOLD_OFF_ID:
        {
          int64_t value;

          if (1 == lwm2m_data_decode_int(dataArray + i, &value))
          {
            if (value >= 0 && value <= 0xFFFF)
            {
              if(targetP->clientHoldOffTime != value)
              {
                val_change = true;
              }
              targetP->clientHoldOffTime = value;
              result = LWM2M_204_CHANGED;
            }
            else
            {
              result = LWM2M_400_BAD_REQUEST;
            }
          }
          else
          {
            result = LWM2M_400_BAD_REQUEST;
          }
          break;
        }
#if defined (LWM2M_BOOTSTRAP)
      case  LWM2M_SECURITY_RESOURCE_30000:
        {
          if (g_carrier_cfg){
            ds_iot_ulog_high1("fun [prv_security_write]  Passed  Argument is %d", LWM2M_SECURITY_RESOURCE_30000);
            if(resInstId != INVALID_RES_INST_ID)
            {
              int64_t value = 0 ; 
              lwm2m_data_decode_int(dataArray + i, &value);
              if (!prv_write_resource_inst_val(&(targetP->custom30000List),resInstId, value, &val_change))
              {
                result = LWM2M_500_INTERNAL_SERVER_ERROR;
              }
              else
              {
                result = LWM2M_204_CHANGED;
              }
            }
            else
            {
              if (dataArray[i].type!= LWM2M_TYPE_MULTIPLE_RESOURCE)
              {
                result = LWM2M_400_BAD_REQUEST;
              }
              else
              {
                int ri = 0;
                lwm2m_data_t* subTlvArray = NULL;
                int64_t value = 0;

                resource_instance_int_list_t* custom30000ListSave = targetP->custom30000List;
                targetP->custom30000List = NULL;

                subTlvArray = dataArray[i].value.asChildren.array;

                if (dataArray[i].value.asChildren.count == 0)
                {
                  result = LWM2M_204_CHANGED;
                  val_change = true;
                }
                else if (subTlvArray==NULL)
                {
                  result = LWM2M_400_BAD_REQUEST;
                }
                else
                {
                  for (ri=0; ri < dataArray[i].value.asChildren.count; ri++)
                  {
                    if (1 != lwm2m_data_decode_int(&subTlvArray[ri], &value))
                    {
                      result = LWM2M_400_BAD_REQUEST;
                      break;
                    }
                    else if (value < 0 || value > 0xFFFF)
                    {
                      result = LWM2M_400_BAD_REQUEST;
                      break;
                    }
                    else if (!prv_write_resource_inst_val(&(targetP->custom30000List),subTlvArray[ri].id, (uint16_t)value,&val_change))
                    {
                      result = LWM2M_500_INTERNAL_SERVER_ERROR;
                      break;
                    }
                    else
                    {
                      val_change = true;
                      result = LWM2M_204_CHANGED;
                    }
                    ds_iot_ulog_high1("fun [prv_security_write]  Passed  Argument value is %d", value);
                  }
                }

                if (result != LWM2M_204_CHANGED)
                {
                  // free pot. partial created new ones
                  LWM2M_LIST_FREE(targetP->custom30000List);
                  // restore old values:
                  targetP->custom30000List = custom30000ListSave;
                }
                else
                {
                  // final free saved value list
                  LWM2M_LIST_FREE(custom30000ListSave);
                }
              }
            }
            break;
          }
        }

#endif 

      default:
        /*If create/write is on an object/object instance level we ignore unknown resources*/
        if(numData > 1)
          result = LWM2M_204_CHANGED;
        else
          result = LWM2M_404_NOT_FOUND;
    }
    i++;
  } while (i < numData && result == LWM2M_204_CHANGED);

  if (LWM2M_204_CHANGED == result  && (create_operation == true || val_change == true))
    store_security_persistent_info(targetP, LWM2M_TRUE);
  if (result != LWM2M_204_CHANGED)
    ds_iot_ulog_err1("failed to write on resource id %d",dataArray[i-1].id);

  return result;
}

/**
 * @fn static uint8_t prv_security_delete()
 *
 * @brief This fucntion is used to delete Security object
 *
 * @param id Instance ID of the object to be deleted
 * @param objectP Object to be read
 *
 * @return LWM2M response type
 */
static uint8_t prv_security_delete(uint16_t id,
    lwm2m_object_t * objectP,
    bool persist_file)
{
  security_instance_t * targetP = NULL;
  if( objectP == NULL ) 
  {
    LOG_DEBUG("fun [%s] %s",__func__, nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }


  objectP->instanceList = lwm2m_list_remove(objectP->instanceList, id, (lwm2m_list_t **)&targetP);
  if (NULL == targetP) return LWM2M_404_NOT_FOUND;

  store_security_persistent_info(targetP,LWM2M_FALSE);
  
#if defined (LWM2M_BOOTSTRAP) 

  LWM2M_LIST_FREE(targetP->custom30000List);

#endif

  if (NULL != targetP->uri)
  {
    lwm2m_free(targetP->uri);
  }
  if (NULL != targetP->sms_number)
  {
     lwm2m_free(targetP->sms_number);
  }

  lwm2m_free(targetP);

  return LWM2M_202_DELETED;
}

/**
 * @fn static uint8_t prv_security_create()
 *
 * @brief This fucntion is used to delete Security object
 *
 * @param instanceId Instance ID of the object to be created
 * @param numData number of resources in the array
 * @param dataArray resource value array
 * @param objectP Object to be read
 *
 * @return LWM2M response type
 */
static uint8_t prv_security_create(uint16_t instanceId,
    int numData,
    lwm2m_data_t * dataArray,
    lwm2m_object_t * objectP, bool apiCall)
{
  security_instance_t * targetP = NULL;
  uint8_t result;
  if( dataArray == NULL || objectP == NULL ) 
  {
    LOG_DEBUG("fun [%s] %s",__func__,nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR; 
  }


  targetP = (security_instance_t *)lwm2m_malloc(sizeof(security_instance_t));
  if (NULL == targetP) return LWM2M_500_INTERNAL_SERVER_ERROR;
  memset(targetP, 0, sizeof(security_instance_t));

  targetP->instanceId = instanceId;
  objectP->instanceList = LWM2M_LIST_ADD(objectP->instanceList, targetP);
  create_operation = TRUE;
  result = prv_security_write(instanceId, numData, dataArray, objectP, INVALID_RES_INST_ID, QAPI_COAP_POST, TRUE);
  create_operation = FALSE;
  ds_iot_ulog_high1("fun [prv_security_create] result %d ", result);
  if (result != LWM2M_204_CHANGED)
  {
    // Issue in creating object instanace so retuning Bad request error.
    result = LWM2M_400_BAD_REQUEST;
    ds_iot_ulog_err1 ("failed to create security  instance for id %d  so deleting instance ",instanceId);  
	/* 3rd param Doesn't matter for Multi instance or Mandotary Objects,
	only applicable for single instance optional objects.	*/
    (void)prv_security_delete(instanceId, objectP,false);
  }
  else
  {
    result = LWM2M_201_CREATED;
  }

  return result;
}

/**
 * @fn void clean_security_object()
 *
 * @brief This fucntion is used to free security instance
 *
 * @param object handle to security object
 *
 * @return void
 *
 */
void clean_security_object(lwm2m_object_t * objectP)
{
  if( objectP == NULL ) 
  {
    LOG_DEBUG("fun [%s] %s",__func__, nullError);
    return ;
  }

  while (objectP->instanceList != NULL)
  {
    security_instance_t * securityInstance = (security_instance_t *)objectP->instanceList;
    objectP->instanceList = objectP->instanceList->next;
    if (NULL != securityInstance->uri)
    {
      lwm2m_free(securityInstance->uri);
    }
	if (NULL != securityInstance->sms_number)
    {
      lwm2m_free(securityInstance->sms_number);
    }
    if (securityInstance->securityMode == LWM2M_SECURITY_MODE_PRE_SHARED_KEY)
    {
      lwm2m_free(securityInstance->publicIdentity);
      lwm2m_free(securityInstance->secretKey);
    }
#if defined (LWM2M_BOOTSTRAP) 

    LWM2M_LIST_FREE(securityInstance->custom30000List);

#endif
    lwm2m_free(securityInstance);
  }
}

#ifdef FACTORY_BOOTSTRAPPING
/**
 * @fn lwm2m_object_t * get_security_object()
 *
 * @brief This function is used to get the information regarding the Security
 *        object during client registartion
 *
 * @param void
 *
 * @return Security object  
 *
 */
lwm2m_object_t * get_security_object()
#else
  /**
   * @fn lwm2m_object_t * get_security_object()
   *
   * @brief This function is used to get the information regarding the Security
   *        object during client registartion
   *
   * @param serverId server id 
   * @param serverUri handle to server uri 
   * @param bsPskId handle to bsPskId
   * @param psk     handle t psk
   * @param pskLen  length of psk
   * @param isBootstrap variable to check for bootstrap
   *
   * @return Security object  
   *
   */
lwm2m_object_t * get_security_object(int serverId,
    const char* serverUri,
    char * bsPskId,
    char * psk,
    uint16_t pskLen,
    bool isBootstrap)
#endif
{
  lwm2m_object_t * securityObj = NULL;
#ifndef FACTORY_BOOTSTRAPPING
  if( serverUri == NULL || bsPskId == NULL || psk == NULL ) 
  {
    LOG_DEBUG("fun [%s] %s",__func__, nullError);
    return NULL ; 
  }
#endif 


  securityObj = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));

  if (NULL != securityObj)
  {
#ifndef FACTORY_BOOTSTRAPPING
    security_instance_t * targetP = NULL;
#endif
    memset(securityObj, 0, sizeof(lwm2m_object_t));

    securityObj->objID = 0;

#ifndef FACTORY_BOOTSTRAPPING
    // Manually create an hardcoded instance
    targetP = (security_instance_t *)lwm2m_malloc(sizeof(security_instance_t));
    if (NULL == targetP)
    {
      lwm2m_free(securityObj);
      return NULL;
    }

    memset(targetP, 0, sizeof(security_instance_t));
    targetP->instanceId = 0;

    targetP->uri = (char*)lwm2m_malloc(strlen(serverUri)+1); 
    if(targetP->uri == NULL)
    {
      lwm2m_free(securityObj);
      lwm2m_free(targetP);
      return NULL;
    }

    LWM2M_STRCPY(targetP->uri, serverUri, strlen(targetP->uri) + 1);
    targetP->securityMode = LWM2M_SECURITY_MODE_NONE;
    targetP->publicIdentity = NULL;
    targetP->publicIdLen = 0;
    targetP->secretKey = NULL;
    targetP->secretKeyLen = 0;
    if (bsPskId != NULL || psk != NULL)
    {
      targetP->securityMode = LWM2M_SECURITY_MODE_PRE_SHARED_KEY;
      if (bsPskId)
      {
        targetP->publicIdentity = lwm2m_strdup(bsPskId);
        targetP->publicIdLen = strlen(bsPskId);
      }
      if (pskLen > 0)
      {
        targetP->secretKey = (char*)lwm2m_malloc(pskLen);
        if (targetP->secretKey == NULL)
        {
          clean_security_object(securityObj);
          return NULL;
        }
        LWM2M_MEMCPY(targetP->secretKey, pskLen, psk, pskLen);
        targetP->secretKeyLen = pskLen;
      }
    }
    targetP->isBootstrap = isBootstrap;
    targetP->shortID = serverId;
    targetP->clientHoldOffTime = 10;

    securityObj->instanceList = LWM2M_LIST_ADD(securityObj->instanceList, targetP);
#endif

    securityObj->readFunc = prv_security_read;

    securityObj->writeFunc = prv_security_write;
    securityObj->createFunc = prv_security_create;
    securityObj->deleteFunc = prv_security_delete;

  }

  return securityObj;
}

/**
 * @fn char * get_server_uri()
 *
 * @brief This function is used to get the server uri
 *
 * @param object handle to security object
 * @param secObjInstID security object id
 *
 * @return pointer to server uri if found,
 *         NULL if not found 
 *
 */
char * get_server_uri(lwm2m_object_t * objectP,
    uint16_t secObjInstID)
{
  security_instance_t * targetP = NULL;
  if( objectP == NULL ) 
  {
    LOG_DEBUG("fun [%s] %s",__func__, nullError);
    return NULL;
  }

  targetP = (security_instance_t *)LWM2M_LIST_FIND(objectP->instanceList, secObjInstID);

  if (NULL != targetP)
  {
    return lwm2m_strdup(targetP->uri);
  }

  return NULL;
}

#if defined (LWM2M_BOOTSTRAP) 
/* 
 * @fn     bool get_serverobj_isbootstrapped()
 * @brief  This function is used to get isBootstrapped resource from security object 
 * @param  objectP - pointer to security object 
 *         secObjInstID - security instance id 
 * @return bool value     
 */ 
bool get_serverobj_isbootstrapped(lwm2m_object_t * objectP,
    uint16_t secObjInstID)
{
  security_instance_t * targetP = NULL;
  if( objectP == NULL ) 
  {
    LOG_DEBUG("fun [%s] %s",__func__,nullError);
    return LWM2M_FALSE;
  }

  targetP = (security_instance_t *)LWM2M_LIST_FIND(objectP->instanceList, secObjInstID);

  if (targetP == NULL) 
  {
    LOG_DEBUG ("NO INSTANCE FOUND FOR secObjInstID -> %d " ,secObjInstID );
  }

  if (NULL != targetP)
  {
    resource_instance_int_list_t *resInst = (resource_instance_int_list_t *)LWM2M_LIST_FIND(targetP->custom30000List, LWM2M_SECURITY_RESOURCE_INST_IS_BOOTSTRAPPED);
    if(resInst != NULL)
      return resInst->InstValue;
    else
      return LWM2M_FALSE;
  }

  return LWM2M_FALSE ;
}

/* 
 * @fn     void lwm2m_update_security_credentials
 * @brief  This function is used to provisioning  the certificates/PSK during factor
 *         bootup process from AFS 
 * @lwm2m_context_t  contextP - Conext object 
 * @return void     
 */ 

void lwm2m_update_security_credentials(lwm2m_context_t * contextP)
{
  lwm2m_object_t * sec_objectP = NULL;
  security_instance_t * targetP = NULL;  

  if( contextP == NULL ) 
  {
    LOG_DEBUG ("%s LWM2M ctxt", nullError) ;  
    return ;
  } 

  sec_objectP = (lwm2m_object_t *)LWM2M_LIST_FIND(contextP->objectList, LWM2M_SECURITY_OBJECT_ID);
  if (sec_objectP == NULL ) 
  {
    ds_iot_ulog_err ("NO Sec object found from the context") ;  
    return ;
  } 

  targetP = (security_instance_t *)sec_objectP->instanceList;
  while (targetP)
  {
    store_security_persistent_info(targetP, LWM2M_TRUE); 
    targetP = targetP->next;
  }
  return;
}


/* 
 * @fn     bool update_security_isbootstrap_flag()
 * @brief  This function is used to update isbootstrapped  resource to security object 
 * @param  sec_objectP - pointer to security object 
 * @return void     
 */ 


void update_security_isbootstrap_flag(lwm2m_object_t * sec_objectP)
{
  security_instance_t * targetP = NULL;  
  bool val_changed = false;
  if (sec_objectP== NULL ) 
  {
    LOG_DEBUG("fun [%s] %s",__func__, nullError);
    return ;
  } 
  targetP = (security_instance_t *)sec_objectP->instanceList;
  while (targetP)
  {
    if (targetP->uri) {
      // if it is bootstrap server security instance , dont update 
      if (targetP->isBootstrap != LWM2M_TRUE) 
      {     
        prv_write_resource_inst_val(&(targetP->custom30000List),LWM2M_SECURITY_RESOURCE_INST_IS_BOOTSTRAPPED, 1, &val_changed);
        if(val_changed == true)
        {
        store_security_persistent_info(targetP, LWM2M_TRUE); 
      }
    }
    }
    targetP = targetP->next;
  }
  return ;
}
/* 
 * @fn     bool update_isbootstrap_field_for_security_instance()
 * @brief  This function is used to update isbootstrapped  resource to security object instance 
 * @param  sec_objectP - pointer to security object 
           secObjInstID - Id of security object instance 
 * @return void     
 */ 
void update_isbootstrap_field_for_security_instance(lwm2m_object_t *sec_objectP,uint16_t secObjInstID)
{
  security_instance_t * targetP = NULL; 
  bool val_changed = false;
  if (sec_objectP== NULL ) 
  {
    LOG_DEBUG("fun [%s] %s",__func__, nullError);
    return ;
  }
  targetP = (security_instance_t *)LWM2M_LIST_FIND(sec_objectP->instanceList, secObjInstID);
  if (targetP == NULL)
  {
    LOG_DEBUG ("In fun [%s] instance list is NULL",__func__);
    return;
  }
  prv_write_resource_inst_val(&(targetP->custom30000List),LWM2M_SECURITY_RESOURCE_INST_IS_BOOTSTRAPPED, 1, &val_changed);
  if(val_changed == true)
  {
    store_security_persistent_info(targetP, LWM2M_TRUE);
  }

  return;
}
#endif
