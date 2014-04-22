/******************************************************************************

  @file    object_access_control.c

  ---------------------------------------------------------------------------
  Copyright (c) 2016-2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------

 *****************************************************************************/

/*******************************************************************************
 *
 * Copyright (c) 2015 Bosch Software Innovations GmbH Germany.
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
 *    Bosch Software Innovations GmbH - Please refer to git log
 *    Pascal Rieux - please refer to git log
 *    
 ******************************************************************************/

/*
 * This "Access Control" object is optional and multiple instantiated
 * 
 *  Resources:
 *
 *          Name         | ID | Oper. | Inst. | Mand.|  Type   | Range | Units |
 *  ---------------------+----+-------+-------+------+---------+-------+-------+
 *  Object ID            |  0 |   R   | Single|  Yes | Integer |1-65534|       |
 *  Object instance ID   |  1 |   R   | Single|  Yes | Integer |0-65535|       |
 *  ACL                  |  2 |   RW  | Multi.|  No  | Integer | 16bit |       |
 *  Access Control Owner |  3 |   RW  | Single|  Yes | Integer |0-65535|       |
 */

#include "liblwm2m.h"
#include "object_access_control.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "qapi_coap.h"
#include "ds_log.h"


#ifdef AUTHORIZATION_SUPPORT

// Resource Id's:
#define RES_M_OBJECT_ID             0
#define RES_M_OBJECT_INSTANCE_ID    1
#define RES_O_ACL                   2
#define RES_M_ACCESS_CONTROL_OWNER  3

#define MAX_ACL_RES_INSTANCES   4
extern client_data_t data;
extern bool enable_acl_obj;
typedef struct _aclres_peristent_info_
{
  bool                    isValid;
  uint16_t                resInstId; 
  uint16_t                accCtrlValue;
} aclres_persistent_info_t;

typedef struct _ac_persistent_info_
{
  uint16_t                  instanceId;
  uint16_t                  objectId;
  uint16_t                  objectInstId;
  uint16_t                  accCtrlOwner;
  bool                      isValid;
  aclres_persistent_info_t  accCtrlVal[MAX_ACL_RES_INSTANCES];
} ac_persistent_info_t;

int load_ac_persistent_info(lwm2m_object_t *aclObjP)
{
  int fd = 0, ret = 0, index = 0;
  acc_ctrl_oi_t *targetP = NULL;
  acc_ctrl_ri_t *aclP = NULL;
  ac_persistent_info_t info;

  if (NULL == aclObjP)
  {
    LOG_DEBUG("%s",nullError);
    return EFAILURE;
  }
  ds_iot_log_strf_med("EFS_OPT: File open %s",ACL_PERSISTENCE_FILE);
  fd = efs_open(ACL_PERSISTENCE_FILE, O_RDONLY);
  if (EFAILURE == fd)
  {
    ds_iot_ulog_err("failed to open server persistence file");
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
    if (LWM2M_FALSE == info.isValid)
    {
      continue;
    }

    targetP = lwm2m_calloc(1, sizeof(acc_ctrl_oi_t));
    if (NULL == targetP)
    {
      ret = -1;
      break;
    }

    targetP->objInstId = info.instanceId;
    targetP->objectId = info.objectId;
    targetP->objectInstId = info.objectInstId;
    targetP->accCtrlOwner = info.accCtrlOwner;
    for (index = 0; index < MAX_ACL_RES_INSTANCES; index++)
    {
      aclP = NULL;
      if (LWM2M_FALSE == info.accCtrlVal[index].isValid)
        continue;

      aclP = lwm2m_calloc(1, sizeof(acc_ctrl_ri_t));
      if (NULL == aclP)
      {
        ret = -1;
        break;
      }
      aclP->resInstId = info.accCtrlVal[index].resInstId;
      aclP->accCtrlValue = info.accCtrlVal[index].accCtrlValue;
      targetP->accCtrlValList = (acc_ctrl_ri_t *)LWM2M_LIST_ADD(targetP->accCtrlValList, aclP);
    }
    if (-1 == ret)
      break;
    aclObjP->instanceList = LWM2M_LIST_ADD(aclObjP->instanceList, targetP);
  }
  if (ret < 0)
  {
    efs_close(fd);
    clean_acl_ctrl_object(aclObjP);
    lwm2m_free(aclObjP);
    return EFAILURE;
  }
  efs_close(fd);
  return ESUCCESS;
}

int store_ac_persistent_info(acc_ctrl_oi_t *targetP, boolean store)
{
  int fd = 0, index = 0;
  ac_persistent_info_t info;
  acc_ctrl_ri_t *aclP;

  if (NULL == targetP)
  {
    LOG_DEBUG("%s",nullError);
    return EFAILURE;
  }
  ds_iot_log_strf_med("EFS_OPT: File open %s",ACL_PERSISTENCE_FILE);
  fd = efs_open(ACL_PERSISTENCE_FILE, O_CREAT | O_WRONLY, 00644);
  if (EFAILURE == fd)
  {
    ds_iot_ulog_err("failed to open server persistence file");
    return EFAILURE;
  }

  memset(&info, 0x00, sizeof(info));
  
  if(store)
  {
    info.isValid = LWM2M_TRUE; 
  }
  else
  {
    info.isValid = LWM2M_FALSE; 
  }
  info.instanceId = targetP->objInstId;
  info.objectId = targetP->objectId;
  info.objectInstId = targetP->objectInstId;
  info.accCtrlOwner = targetP->accCtrlOwner;
  aclP = targetP->accCtrlValList;
  while (aclP)
  {
    info.accCtrlVal[index].isValid = LWM2M_TRUE;
    info.accCtrlVal[index].resInstId = aclP->resInstId;
    info.accCtrlVal[index++].accCtrlValue = aclP->accCtrlValue;
    aclP = aclP->next;
  }
  while (index < MAX_ACL_RES_INSTANCES)
  {
    info.accCtrlVal[index++].isValid = LWM2M_FALSE;
  }

  efs_lseek(fd, info.instanceId * sizeof(info), SEEK_SET);
  ds_iot_log_strf_med("EFS_OPT: File write %s",ACL_PERSISTENCE_FILE);
  if (EFAILURE == efs_write(fd, &info, sizeof(info)))
  {
    ds_iot_ulog_err1("failed to store persistent value of server instance %u", info.instanceId);
    efs_close(fd);
    return EFAILURE;
  }
  efs_close(fd);
  return ESUCCESS;
}

/** 
* @fn static uint8_t prv_ac_create_write_check() 
*
* @brief This fucntion verifies and write the resource's into  
*        Access control object 
*
* @param instanceId Instance of the Access control to be create/write 
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

  
static uint8_t prv_ac_create_write_check(int numData,
                                               lwm2m_data_t * dataArray,
                                               bool create_flag)
{ 
  int16_t i = 0; 
  uint8_t result = LWM2M_NO_ERROR; 
  if((NULL == dataArray) )
  { 
    LOG_DEBUG("%s", nullError); 
    return LWM2M_500_INTERNAL_SERVER_ERROR; 
  } 
  i = 0; 
  do
  {
    switch(dataArray[i].id)
    {
      case RES_M_OBJECT_ID:
      case RES_M_OBJECT_INSTANCE_ID:
      if (!create_flag)
      {
        result = LWM2M_405_METHOD_NOT_ALLOWED;
        break;
      }
      case RES_O_ACL:
      case RES_M_ACCESS_CONTROL_OWNER:
        result = LWM2M_204_CHANGED;
        break;
      default:
        if(!create_flag)
        {
          result = LWM2M_404_NOT_FOUND;
        }
        else
        {
          result = LWM2M_204_CHANGED;
        }
        break;
     }
  }while(++i < numData && result == LWM2M_204_CHANGED);

  if(result != LWM2M_204_CHANGED)
  {
     ds_iot_ulog_high1("failed for resource id %d",dataArray[i-1].id);
  }
   return result;
}


/**
 * @fn static uint8_t prv_set_ac_tlv()
 *
 * @brief This fucntion is used to populate the values of the resource
 *        to be provided to the server during the read operation on the object.
 * @param dataP data array for the resource 
 * @param accCtrlOiP access control object data
 * @return LWM2M response type
 */
static uint8_t prv_set_ac_tlv(lwm2m_data_t* dataP, acc_ctrl_oi_t* accCtrlOiP, uint16_t resInstId)
{
  if (accCtrlOiP == NULL || dataP == NULL)
  {
    LOG_DEBUG("fun [%s] file [%s]  %s",__func__,nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  switch (dataP->id) 
  {
    case RES_M_OBJECT_ID:
      lwm2m_data_encode_int(accCtrlOiP->objectId, dataP);
      return LWM2M_205_CONTENT;
      // commented the break to address the dead code, as this case is currently not supported 
      //break;
    case RES_M_OBJECT_INSTANCE_ID:
      lwm2m_data_encode_int(accCtrlOiP->objectInstId, dataP);
      return LWM2M_205_CONTENT;
      // commented the break to address the dead code, as this case is currently not supported 
      //break;
    case RES_O_ACL:
      {
        if (resInstId != INVALID_RES_INST_ID)
        {
          resource_instance_int_list_t *resInst = (resource_instance_int_list_t *)LWM2M_LIST_FIND(accCtrlOiP->accCtrlValList,resInstId);
          if(resInst != NULL)
          {
            lwm2m_data_encode_int(resInst->InstValue, dataP);
            return LWM2M_205_CONTENT;
          }
          else
            return LWM2M_404_NOT_FOUND;
        }
        else
        {
          int ri = 0;
          acc_ctrl_ri_t* accCtrlRiP = NULL;
          for (accCtrlRiP =accCtrlOiP->accCtrlValList, ri=0;
              accCtrlRiP!=NULL;
              accCtrlRiP = accCtrlRiP->next, ri++);

          if (ri==0)  // no values!
          {
            lwm2m_data_t* subTlvP = NULL;
            lwm2m_data_encode_instances(subTlvP, 0, dataP);
            return LWM2M_205_CONTENT;
          }
          else
          {
            lwm2m_data_t* subTlvP = lwm2m_data_new(ri);
            if (NULL == subTlvP) return LWM2M_500_INTERNAL_SERVER_ERROR;
            for (accCtrlRiP = accCtrlOiP->accCtrlValList, ri = 0;
                accCtrlRiP!= NULL;
                accCtrlRiP = accCtrlRiP->next, ri++)
            {
#ifdef ACL_OBJ_FIX
              subTlvP[ri].id = (uint16_t)accCtrlRiP->resInstId;
#else
              subTlvP[ri].id = (uint16_t)ri;
#endif
              lwm2m_data_encode_int(accCtrlRiP->accCtrlValue, &subTlvP[ri]);
            }
#ifdef ACL_OBJ_FIX
            lwm2m_data_encode_instances(subTlvP, ri, dataP);
#else
            lwm2m_data_encode_instances(subTlvP, 2, dataP);
#endif
            return LWM2M_205_CONTENT;
          }
        }
      }
      // commented the break to address the dead code, as this case is currently not supported 
      // break;
    case RES_M_ACCESS_CONTROL_OWNER:
      lwm2m_data_encode_int(accCtrlOiP->accCtrlOwner, dataP);
      return LWM2M_205_CONTENT;
      // commented the break to address the dead code, as this case is currently not supported 
      //break;
    default:
      return LWM2M_404_NOT_FOUND ;
  }
}

/**
 * @fn static uint8_t prv_ac_read()
 * @brief Implementation (callback-) function of reading object resources.
 *        For whole object, single resources or a sequence of resources
 * 
 * @param objInstId    instances ID of the location object to read
 * @param numDataP     pointer to the number of resource to read
 * @param tlvArrayP    TLV data sequence with initialized resource ID to read
 * @param objectP      private location data structure
 *
 * @return LWM2M response type
 *
 */
static uint8_t prv_ac_read(uint16_t instanceId, int * numDataP,
    lwm2m_data_t** dataArrayP, lwm2m_object_t * objectP, uint16_t resInstId, lwm2m_server_t *serverP)
{
  uint8_t result = 0;
  int     ri = 0, ni = 0;
  acc_ctrl_oi_t* accCtrlOiP = NULL;
  if( numDataP == NULL || objectP == NULL || dataArrayP == NULL) 
  {
    ds_iot_ulog_err("fun [prv_ac_read] :LWM2M_LOG: NULL Error");
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  // multi-instance object: search instance
  accCtrlOiP = (acc_ctrl_oi_t *)lwm2m_list_find(objectP->instanceList, instanceId);
  if (accCtrlOiP == NULL)
  {
    return LWM2M_404_NOT_FOUND ;
  }

  // is the server asking for the full object ?
  if (*numDataP == 0)
  {
    uint16_t resList[] = {
      RES_M_OBJECT_ID,
      RES_M_OBJECT_INSTANCE_ID,
      RES_O_ACL,  // prv_set_tlv will return LWM2M_404_NOT_FOUND w/o values!
      RES_M_ACCESS_CONTROL_OWNER
    };
    int nbRes = sizeof(resList) / sizeof(uint16_t);

    *dataArrayP = lwm2m_data_new(nbRes);
    if (*dataArrayP == NULL)
      return LWM2M_500_INTERNAL_SERVER_ERROR ;
    *numDataP = nbRes;
    for (ri = 0; ri < nbRes; ri++)
    {
      (*dataArrayP)[ri].id = resList[ri];
    }
  }

  ni = 0;
  do
  {
    result = prv_set_ac_tlv((*dataArrayP) + ni, accCtrlOiP, resInstId);
    ni++;
  } while (ni < *numDataP && result == LWM2M_205_CONTENT);

  return result;
}

/**
 * @fn static bool prv_add_ac_val()
 * @brief Implementation function of setting access value for given access
 *        control object
 * 
 * @param accCtrlOiP   handle to access control object
 * @param acResId      resource instance id
 * @param acValue      access control value
 *
 * @return true on success, 
 *         false on failure
 *
 */
static bool prv_add_ac_val(acc_ctrl_oi_t* accCtrlOiP,
    uint16_t acResId, uint16_t acValue)
{
  bool ret = false;
  acc_ctrl_ri_t *accCtrlRiP = NULL, *temp;
  if( accCtrlOiP == NULL ) 
  {
    LOG_DEBUG("fun [%s] %s",__func__,nullError);
    return false;
  }
  temp = accCtrlOiP->accCtrlValList;
  while(temp)
  {
    if(temp->resInstId == acResId)
    {
      temp->accCtrlValue = acValue;
      return true;
    }
    temp = temp->next;
  }

  accCtrlRiP = (acc_ctrl_ri_t *)lwm2m_malloc(sizeof(acc_ctrl_ri_t));
  if (accCtrlRiP==NULL)
  {
    return ret;
  }
  else
  {
    memset(accCtrlRiP, 0, sizeof(acc_ctrl_ri_t));
    accCtrlRiP->resInstId      = acResId;
    accCtrlRiP->accCtrlValue   = acValue;

    accCtrlOiP->accCtrlValList = (acc_ctrl_ri_t*)
    LWM2M_LIST_ADD(accCtrlOiP->accCtrlValList, accCtrlRiP);
    ret = true;
  }
  return ret;

}

/**
 * @fn bool prv_remove_ac_val()
 * @brief Implementation function of removing access value for given access
 *        control object
 * 
 * @param accCtrlOiP   handle to access control object
 * @param acResId      resource instance id
 *
 * @return true on success, 
 *         false on failure
 *
 */
bool prv_remove_ac_val(acc_ctrl_oi_t* accCtrlOiP,
                       uint16_t acResId)
{
  bool ret = false;
  acc_ctrl_ri_t *accCtrlRiP = NULL;

  if( accCtrlOiP == NULL ) 
  {
    LOG_DEBUG("fun [%s] %s",__func__,nullError);
    return ret;
  }

  accCtrlOiP->accCtrlValList = (acc_ctrl_ri_t*)LWM2M_LIST_RM(accCtrlOiP->accCtrlValList, acResId, &accCtrlRiP);

  if(accCtrlRiP == NULL)
  {
    ret = false;
  }

  else
  {
    lwm2m_free(accCtrlRiP);
    accCtrlRiP = NULL;
    ret = true;
  }

  return ret;
}


/**
 * @fn static uint8_t prv_write_resources()
 *
 * @brief This fucntion implements the write operation performed on
 *        Access control object
 *
 * @param instanceId Instance ID of the object to be read
 * @param numData number of resources in the array
 * @param tlvArray resource value array
 * @param objectP Object to be read
 * @param doCreate flag to check
 *
 * @return LWM2M response type
 */
static uint8_t prv_write_resources(uint16_t instanceId, int numData,
    lwm2m_data_t* tlvArray, lwm2m_object_t* objectP, bool doCreate, uint16_t resInstId, uint8_t write_method){
  int i = 0;
  uint8_t result = 0;
  int64_t value = 0;
  acc_ctrl_oi_t* accCtrlOiP = NULL;
  lwm2m_client_state_t lwm2m_state;
  bool val_change = false;

  if( tlvArray == NULL || objectP == NULL ) 
  {
    LOG_DEBUG("fun [%s] %s",__func__,nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  accCtrlOiP = (acc_ctrl_oi_t *) lwm2m_list_find(objectP->instanceList, instanceId);
  if (NULL == accCtrlOiP)
    return LWM2M_404_NOT_FOUND ;

  i = 0;
  do
  {
    switch (tlvArray[i].id)
    {
      case RES_M_OBJECT_ID:
        if (doCreate==false)
        {
          result = LWM2M_405_METHOD_NOT_ALLOWED;
        }
        else
        {
          if (1 != lwm2m_data_decode_int(&tlvArray[i], &value))
          {
            result = LWM2M_400_BAD_REQUEST;
          }
          else if (value < 1 || value > 65534)
          {
            result = LWM2M_400_BAD_REQUEST;
          }
          else
          {
            accCtrlOiP->objectId = value;
            result = LWM2M_204_CHANGED;
          }
        }
        break;
      case RES_M_OBJECT_INSTANCE_ID:
        if (doCreate==false)
        {
          result = LWM2M_405_METHOD_NOT_ALLOWED;
        }
        else
        {
          if (1 != lwm2m_data_decode_int(&tlvArray[i], &value))
          {
            result = LWM2M_400_BAD_REQUEST;
          }
          else if (value < 0 || value > 65535)
          {
            result = LWM2M_400_BAD_REQUEST;
          }
          else
          {
            accCtrlOiP->objectInstId = value;
            result = LWM2M_204_CHANGED;
          }
        }
        break;
      case RES_O_ACL:
        {
          if(resInstId != INVALID_RES_INST_ID)
          {
            int64_t value = 0 ;
            if (1 != lwm2m_data_decode_int(&tlvArray[i], &value))
            {
              result = LWM2M_400_BAD_REQUEST;
            }
            else if (value < 0 || value > 0xFF )
            {
              result = LWM2M_400_BAD_REQUEST;
            }
            else
            {
              if (!prv_write_resource_inst_val((resource_instance_int_list_t **)&(accCtrlOiP->accCtrlValList),resInstId, value, &val_change))
              {
                result = LWM2M_500_INTERNAL_SERVER_ERROR;
              }
              else
              {
                result = LWM2M_204_CHANGED;
              }
            }
          }
          else
          {
            if (tlvArray[i].type!= LWM2M_TYPE_MULTIPLE_RESOURCE)
            {
              result = LWM2M_400_BAD_REQUEST;
            }
            else
            {
              int ri = 0;
              lwm2m_data_t* subTlvArray = NULL;

              acc_ctrl_ri_t* acValListSave = accCtrlOiP->accCtrlValList;
              if(write_method == QAPI_COAP_PUT)
                accCtrlOiP->accCtrlValList = NULL;

              subTlvArray = tlvArray[i].value.asChildren.array;

              if (tlvArray[i].value.asChildren.count == 0)
              {
                val_change = true;
                result = LWM2M_204_CHANGED;
              }
              else if (subTlvArray==NULL)
              {
                result = LWM2M_400_BAD_REQUEST;
              }
              else
              {
#ifdef ACL_OBJ_FIX
                for (ri=0; ri < tlvArray[i].value.asChildren.count; ri++)
#else
                  for (ri=0; tlvArray[i].value.asChildren.count; ri++)
#endif
                  {
                    if (1 != lwm2m_data_decode_int(&subTlvArray[ri], &value))
                    {
                      result = LWM2M_400_BAD_REQUEST;
                      break;
                    }
                    else if (value < 0 || value > 0xFF)
                    {
                      result = LWM2M_400_BAD_REQUEST;
                      break;
                    }
                    else if (!prv_add_ac_val(accCtrlOiP, subTlvArray[ri].id,
                          (uint16_t)value))
                    {
                      result = LWM2M_500_INTERNAL_SERVER_ERROR;
                      break;
                    }
                    else
                    {
                      val_change = true;
                      result = LWM2M_204_CHANGED;
                    }
                  }
              }

              if (result != LWM2M_204_CHANGED)
              {
                // free pot. partial created new ones
                LWM2M_LIST_FREE(accCtrlOiP->accCtrlValList);
                // restore old values:
                accCtrlOiP->accCtrlValList = acValListSave;
              }
              else
              {
                /* Free acValListSave during PUT, as memory for new nodes are allocated in prv_add_ac_val*/
                if(write_method == QAPI_COAP_PUT)
                  LWM2M_LIST_FREE(acValListSave);
              }
            }
          }
        }   break;
      case RES_M_ACCESS_CONTROL_OWNER: 
        {
          if (1 == lwm2m_data_decode_int(tlvArray + i, &value))
          {
            lwm2m_state = lwm2m_get_client_state();
            /*prevent to create Access Control Object Instance on Object if it's
              not in Bootstrap phase or in Initial state.
            */
            if ((value >= 0 && value < LWM2M_MAX_ID) ||
                (value == LWM2M_MAX_ID &&
                 ((lwm2m_state == STATE_INITIAL) ||
                  (lwm2m_state == STATE_BOOTSTRAP_REQUIRED) ||
                  ( lwm2m_state == STATE_BOOTSTRAPPING ))))
            {
              if(accCtrlOiP->accCtrlOwner != value)
              {
                val_change = true;
              }
              accCtrlOiP->accCtrlOwner = value;
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
      default:
        {
          /*If create/write is on an object/object instance level we ignore unknown resources*/
          if(numData > 1)
            result = LWM2M_204_CHANGED;
          else
            return LWM2M_404_NOT_FOUND;
        }
    }
    i++;
  } while (i < numData && result == LWM2M_204_CHANGED );

  if (LWM2M_204_CHANGED == result && (doCreate == true || val_change == true))
    store_ac_persistent_info(accCtrlOiP, LWM2M_TRUE);

  return result;
}

/**
 * @fn static uint8_t prv_ac_write()
 *
 * @brief This fucntion implements the write operation performed on
 *        Access control object
 *
 * @param instanceId Instance ID of the object to be read
 * @param numData number of resources in the array
 * @param tlvArray resource value array
 * @param objectP Object to be read
 *
 * @return LWM2M response type
 */
static uint8_t prv_ac_write(uint16_t instanceId, int numData,
    lwm2m_data_t* tlvArray, lwm2m_object_t* objectP, uint16_t resInstId, uint8_t write_method, bool bs_true)
{

  lwm2m_client_state_t lwm2m_state = (lwm2m_client_state_t)0;
  uint8_t result = LWM2M_205_CONTENT;

  if( tlvArray == NULL || objectP == NULL ) 
  {
    ds_iot_ulog_err("fun [prv_ac_write]:LWM2M_LOG: NULL Error");
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }
  /* Check all resources to be written are writable or not */
     	
  result = prv_ac_create_write_check(numData, tlvArray, bs_true);
  if (result != LWM2M_204_CHANGED )
  {
    ds_iot_ulog_err("Write operation failed due to write on non-writable resource. ");
    return result;
  }

  lwm2m_state = lwm2m_get_client_state();

  if( (lwm2m_state == STATE_BOOTSTRAP_REQUIRED) || ( lwm2m_state == STATE_BOOTSTRAPPING ) )
    return prv_write_resources(instanceId, numData, tlvArray, objectP, true, resInstId, write_method);
  else
    return prv_write_resources(instanceId, numData, tlvArray, objectP, false, resInstId, write_method);
}


/**
 * @fn static uint8_t prv_ac_delete()
 *
 * @brief This fucntion is used to delete access control object
 *
 * @param id Instance ID of the object to be deleted
 * @param objectP Object to be deleted
 *
 * @return LWM2M response type
 */
static uint8_t prv_ac_delete(uint16_t id, lwm2m_object_t * objectP, bool persist_file)
{
  acc_ctrl_oi_t* targetP = NULL;

  if ( objectP == NULL ) 
  {
    LOG_DEBUG("fun [%s] %s",__func__,nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  objectP->instanceList = lwm2m_list_remove(objectP->instanceList, id,
      (lwm2m_list_t**)&targetP);
  
  store_ac_persistent_info(targetP,LWM2M_FALSE);
  if (NULL == targetP) return LWM2M_404_NOT_FOUND;

  update_acl_specific_list(data.lwm2mH, objectP);

  LWM2M_LIST_FREE(targetP->accCtrlValList);
  lwm2m_free(targetP);

  return LWM2M_202_DELETED;
}

/**
 * @fn static uint8_t prv_ac_create()
 *
 * @brief This fucntion is used to create access control object
 *
 * @param instanceId Instance ID of the object to be created
 * @param numData number of resources in the array
 * @param tlvArray resource value array
 * @param objectP Object to be read
 *
 * @return LWM2M response type
 */

/***************************************************************************************************

 Under section 8.2.3 Operation on Object of OMA 1.1

- If the �Create� operation is permitted, the LwM2M Client MUST perform the instantiation on the 
Object only if all the mandatory Resources are present in the �New Value� parameter (see Section 5). 
If all the mandatory Resources are not present, the LwM2M Client MUST send a �Bad Request� error code 
to the LwM2M Server


*************************************************************************************************************/

static uint8_t prv_ac_create(uint16_t objInstId, int numData,
    lwm2m_data_t * tlvArray, lwm2m_object_t * objectP, bool apiCall)
{
  acc_ctrl_oi_t * targetP = NULL;
  uint8_t result = LWM2M_NO_ERROR;
  uint16_t mandatory_resource[] = {RES_M_OBJECT_ID, RES_M_OBJECT_INSTANCE_ID, RES_M_ACCESS_CONTROL_OWNER};
  uint8_t size = sizeof(mandatory_resource)/sizeof(mandatory_resource[0]);
  if( tlvArray == NULL || objectP ==  NULL ) 
  { 
    LOG_DEBUG("fun [%s] %s",__func__,nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }
  if(apiCall == TRUE)
  {
     goto skip_check;
  }
  
  result = prv_mandatory_check(numData, tlvArray, mandatory_resource, size);
  if(result != LWM2M_NO_ERROR)
    return result;

  /* Check all resources to be written are writable or not */
  result = prv_ac_create_write_check(numData, tlvArray, TRUE);
  if (result != LWM2M_204_CHANGED )
  {
     ds_iot_ulog_err("Creating instance failed");
     return result;
  }
skip_check:
  targetP = (acc_ctrl_oi_t *)lwm2m_malloc(sizeof(acc_ctrl_oi_t));
  if (NULL == targetP) return LWM2M_500_INTERNAL_SERVER_ERROR;
  memset(targetP, 0, sizeof(acc_ctrl_oi_t));

  targetP->objInstId    = objInstId;
  objectP->instanceList = LWM2M_LIST_ADD(objectP->instanceList, targetP);

  result = prv_write_resources(objInstId, numData, tlvArray, objectP, true, INVALID_RES_INST_ID, QAPI_COAP_PUT);

  if (result != LWM2M_204_CHANGED)
  {
    // Issue in creating object instanace so retuning Bad request error.
    result = LWM2M_400_BAD_REQUEST;
    prv_ac_delete(objInstId, objectP, false);
  }
  else
  {
    result = LWM2M_201_CREATED;
  }

  update_acl_specific_list(data.lwm2mH, objectP);

  return result;
}



/* @fn      static uint8_t prv_ac_discover()
 * @brief   This function is used to discover resources of a access control object
 * @param   instanceId - instance id of object 
 *          numdataP   - pointer to store number of data array elements 
 dataArrayP - pointer to store data 
 objectP    - pointer to lwm2m_object 
 * @return  on success - LWM2M_404_NOT_FOUND
 on error   - LWM2M_404_NOT_FOUND
 */   
static uint8_t prv_ac_discover(uint16_t instanceId,
    int * numDataP,
    lwm2m_data_t ** dataArrayP,
    lwm2m_object_t * objectP, lwm2m_server_t *serverP)
{
  uint8_t result = 0;
  int i = 0;
  acc_ctrl_oi_t* accCtrlOiP = NULL;
  int ri = 0;
  acc_ctrl_ri_t* accCtrlRiP = NULL;

  if( numDataP == NULL || dataArrayP == NULL || objectP == NULL ) 
  {
    LOG_DEBUG("fun [%s] %s",__func__,nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  result = LWM2M_205_CONTENT;
  
  accCtrlOiP = (acc_ctrl_oi_t *)lwm2m_list_find(objectP->instanceList, instanceId);
  if (accCtrlOiP != NULL)
  {
    for (accCtrlRiP =accCtrlOiP->accCtrlValList, ri=0;
              accCtrlRiP!=NULL;
              accCtrlRiP = accCtrlRiP->next, ri++);
  }
  if (*numDataP == 0)
  {
     uint16_t resList[] = {
       RES_M_OBJECT_ID,
       RES_M_OBJECT_INSTANCE_ID,
       RES_O_ACL,
       RES_M_ACCESS_CONTROL_OWNER
 };

    int nbRes = sizeof(resList) / sizeof(uint16_t);

    *dataArrayP = lwm2m_data_new(nbRes);
    if (*dataArrayP == NULL) return LWM2M_500_INTERNAL_SERVER_ERROR;
      *numDataP = nbRes;
    for (i = 0; i < nbRes; i++)
    {
      (*dataArrayP)[i].id = resList[i];
      if((*dataArrayP)[i].id == RES_O_ACL )
      {
        (*dataArrayP)[i].type = LWM2M_TYPE_MULTIPLE_RESOURCE;
        (*dataArrayP)[i].value.asChildren.count = ri;
      }
    }
  }
  else
  {
    for (i = 0; i < *numDataP && result == LWM2M_205_CONTENT; i++)
    {
      switch ((*dataArrayP)[i].id)
      {                        
	    case RES_O_ACL:
        {
          (*dataArrayP)[i].type = LWM2M_TYPE_MULTIPLE_RESOURCE;
          (*dataArrayP)[i].value.asChildren.count = ri;
          break;
        }
        case RES_M_OBJECT_ID:
        case RES_M_OBJECT_INSTANCE_ID:
        case RES_M_ACCESS_CONTROL_OWNER:
          break;
        default:
          result = LWM2M_404_NOT_FOUND;
      }
    }
  }

  return result;
}

#if 0
/**
 * @fn static uint8_t prv_ac_execute()
 *
 * @brief This fucntion implements the execute operation performed on
 *        Access control object
 *
 * @param instanceId (IN) Instance ID of the object to be read
 * @param resourceId (IN) Resource ID of the resource to be executed
 * @param buffer (IN) message payload from server fro execute operation
 * @param length (IN) message payload length
 * @param objectP Object on which the operation needs to be executed
 * @return LWM2M response type
 */
static uint8_t prv_ac_execute(uint16_t instanceId,
                                    uint16_t resourceId,
                                    uint8_t * buffer,
                                    int length,
                                    lwm2m_object_t * objectP)
{
  acc_ctrl_oi_t *instanceP = NULL;

  if ( objectP == NULL )
  {
    LOG_ERROR("%s",nullError);
    return LWM2M_400_BAD_REQUEST;
  }

  instanceP = (acc_ctrl_oi_t *)LWM2M_LIST_FIND(objectP->instanceList, instanceId);

  if (NULL == instanceP)
  {
    return LWM2M_404_NOT_FOUND;
  }

  switch(resourceId)
  {
    case RES_M_OBJECT_ID:
    case RES_M_OBJECT_INSTANCE_ID:
    case RES_O_ACL:
    case RES_M_ACCESS_CONTROL_OWNER:
      return LWM2M_405_METHOD_NOT_ALLOWED;
    default:
      return LWM2M_404_NOT_FOUND;
  }
}
#endif

/*
 * Create an empty multiple instance LWM2M Object: Access Control
 */
/**
 * @fn static uint8_t acc_ctrl_create_object()
 *
 * @brief This fucntion is used to Create an empty multiple instance 
 *        Access Control object
 *
 * @param instanceId Instance ID of the object to be created
 * @param numData number of resources in the array
 * @param tlvArray resource value array
 * @param objectP Object to be read
 *
 * @return handle to access control object type
 */
lwm2m_object_t * acc_ctrl_create_object(void)
{
  /*
   * The acc_ctrl_create_object() function creates an empty object
   * and returns a pointer to the structure that represent it.
   */
  lwm2m_object_t* accCtrlObj = NULL;

  accCtrlObj = (lwm2m_object_t *) lwm2m_malloc(sizeof(lwm2m_object_t));

  if (NULL != accCtrlObj)
  {
    memset(accCtrlObj, 0, sizeof(lwm2m_object_t));
    /*
     * It assign his unique object ID
     * The 2 is the standard ID for the optional object "Access Control".
     */
    accCtrlObj->objID = 2;
    // Init callbacks, empty instanceList!
    accCtrlObj->readFunc     = prv_ac_read;
    accCtrlObj->writeFunc    = prv_ac_write;
    accCtrlObj->createFunc   = prv_ac_create;
    accCtrlObj->deleteFunc   = prv_ac_delete;
    accCtrlObj->discoverFunc = prv_ac_discover;
    accCtrlObj->executeFunc  = NULL;
  }
  return accCtrlObj;
}

/**
 * @fn void acl_ctrl_free_object()
 *
 * @brief This fucntion is used to free access control object
 *
 * @param objectP handle to server object
 *
 * @return void
 *
 */
void clean_acl_ctrl_object(lwm2m_object_t * objectP)
{
  acc_ctrl_oi_t *accCtrlOiT = NULL;
  acc_ctrl_oi_t *accCtrlOiP = NULL;
  if (objectP == NULL)
  {
    LOG_DEBUG("%s",nullError);
    return;
  }
  accCtrlOiP = (acc_ctrl_oi_t*)objectP->instanceList;
  while (accCtrlOiP != NULL)
  {
    // first free acl (multiple resource!):
    LWM2M_LIST_FREE(accCtrlOiP->accCtrlValList);
    accCtrlOiT = accCtrlOiP;
    accCtrlOiP = accCtrlOiP->next;
    lwm2m_free(accCtrlOiT);
  }
  objectP->instanceList = NULL;
}

/**
 * @fn acc_ctrl_oi_t *acl_create_obj_inst_with_access_right()
 *
 * @brief This fucntion is used to create an access control object
 *        with specific access rights
 *
 * @param accCtrlObjP access control object data
 * @param instId Instance ID of the object
 * @param acObjectId Access control object id
 * @param acObjInstId instance id of object
 * @param acOwner access control owner value
 * @param isSetAccCtrlValue need to set accCtrlValue or not
 * @param accCtrlValue ACL resource instance value or access right
 *
 * @return handle to access control object
 */
acc_ctrl_oi_t *acl_create_obj_inst_with_access_right(lwm2m_object_t* accCtrlObjP,
    uint16_t instId, uint16_t acObjectId, uint16_t acObjInstId, 
    uint16_t acOwner, bool isSetAccCtrlValue, uint16_t accCtrlValue)
{
  acc_ctrl_oi_t *accCtrlOiP = NULL;

  if(!enable_acl_obj)
  	return NULL;

  if (accCtrlObjP == NULL)
  {
    LOG_DEBUG("%s",nullError);
    return NULL;
  }


  accCtrlOiP = (acc_ctrl_oi_t *)lwm2m_malloc(sizeof(acc_ctrl_oi_t));
  if (NULL != accCtrlOiP)
  {
    memset(accCtrlOiP, 0, sizeof(acc_ctrl_oi_t));
    accCtrlOiP->objInstId    = instId;
    accCtrlOiP->objectId     = acObjectId;
    accCtrlOiP->objectInstId = acObjInstId;
    //acOwner == 0 means it's default access right for other DM servers
    accCtrlOiP->accCtrlOwner = acOwner;

    if(isSetAccCtrlValue == true && !prv_add_ac_val(accCtrlOiP, acOwner, accCtrlValue))
    {
      lwm2m_free(accCtrlOiP);
      accCtrlOiP = NULL;
    }
    else
    {
      accCtrlObjP->instanceList = LWM2M_LIST_ADD(accCtrlObjP->instanceList, accCtrlOiP);
      store_ac_persistent_info(accCtrlOiP, LWM2M_TRUE);
    }
  }

  return accCtrlOiP;
}

/**
 * @fn uint16_t get_acl_access_right_sum()
 *
 * @brief This fucntion is used to calculate the access right sum for ACL instance value.
 *
 * @param accCtrlValue ACL resource instance value or access right
 *
 * @return the access right sum for ACL value
 */

uint16_t get_acl_access_right_sum(uint16_t accCtrlValue)
{
  uint16_t acl_acc_right_sum = 0;
  //refer to the LSB 1st~4th of access control value
  uint16_t acl_acc_right = accCtrlValue & ACC_CTRL_PERM_FULL_RIGHT;
  while(acl_acc_right != 0)
  {
    acl_acc_right_sum += acl_acc_right & 0x01;
    acl_acc_right >>= 1;
  }
  return acl_acc_right_sum;
}

/**
 * @fn void update_acl_specific_list()
 *
 * @brief This fucntion is used to update AC Object or Object Instance specific list.
 *
  *@param contextP lwm2m context
 * @param objectP here is the Access Control Object
 *
 * @return void
 */

void update_acl_specific_list(lwm2m_context_t *contextP, lwm2m_object_t *objectP)
{

  if(enable_acl_obj && contextP != NULL && objectP != NULL)
  {
    contextP->acObjInstList = (objectP->instanceList);
  }
  return;
}
#endif //AUTHORIZATION_SUPPORT

