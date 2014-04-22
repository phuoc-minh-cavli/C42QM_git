/******************************************************************************

  @file    discover.c

  ---------------------------------------------------------------------------
  Copyright (c) 2016-2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------

******************************************************************************/

/*******************************************************************************
 *
 * Copyright (c) 2015 Intel Corporation and others.
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
 *
 *******************************************************************************/


#include "internals.h"
#include "ds_log.h"


#define PRV_CONCAT_STR(buf, len, index, str, str_len)    \
{                                                    \
  if ((len)-(index) < (str_len)) return -1;        \
  LWM2M_MEMCPY((buf)+(index), ((len) - (index)), (str), (str_len));         \
  (index) += (str_len);                            \
}

/**
 * @fn static int prv_findAttributes()
 * @brief This function is used to return attributes for a given server
 *
 * @param contextP   handle to lwm2m context
 * @param uriP       handle to resource uri
 *
 * @return  NULL on failure
 *           
 */
 
#ifdef LWM2M_CLIENT_MODE

static lwm2m_attributes_t * prv_findAttributes(lwm2m_context_t * contextP,
                                               lwm2m_uri_t * uriP)
{
    lwm2m_observed_t * observedP;
    lwm2m_watcher_t * watcherP;
    lwm2m_attributes_t * paramP;
	lwm2m_server_t * serverP = NULL;

    paramP = NULL;

    if (contextP == NULL) return NULL;
    if (uriP == NULL) return NULL;

    if(!lwm2m_is_standard_object(uriP->objectId))
    {
      paramP = lwm2m_findByUri_extobj_attr(uriP);
      if(paramP == NULL)
      {
        ds_iot_ulog_high("prv_findAttributes:NULL Error");
      }
    }
    else
    {
      serverP = contextP->activeServerP;
      if (serverP == NULL) return NULL;
      observedP = observe_findByUri(contextP, uriP);
      if (observedP == NULL || observedP->watcherList == NULL) return NULL;

      for (watcherP = observedP->watcherList; watcherP != NULL; watcherP = watcherP->next)
      {
        if (watcherP->server == serverP)
        {
            paramP = watcherP->parameters;
        }
      }
    }
  return paramP;
}

/**
 * @fn static int serialized_attributes_size()
 * @brief This function is used to get size of serialized attributes
 *
 * @param contextP             handle to lwm2m context
 * @param uriP                 handle to resource uri
 * @param lwm2m_attributes_t   lwm2m attributes
 *
 * @return   -1 on failure
 *           
 */
static int serialized_attributes_size(lwm2m_context_t * contextP,
                                   lwm2m_uri_t * uriP,
                                   lwm2m_attributes_t * objectParamP)
{
    int length = 0;
    lwm2m_attributes_t * paramP;
    if (contextP == NULL || uriP == NULL ) 
    {
      LOG_DEBUG(" Passed  Argument is NULL");
      return -1;
    }
    paramP = prv_findAttributes(contextP, uriP);
    if (paramP == NULL) paramP = objectParamP;

    if (paramP != NULL)
    {
        if (paramP->toSet & LWM2M_ATTR_FLAG_MIN_PERIOD)
        {
            length += (LINK_ATTR_SEPARATOR_SIZE + ATTR_MIN_PERIOD_LEN + utils_numDigit(paramP->minPeriod)); 
        }
        else if ((objectParamP) && (objectParamP->toSet & LWM2M_ATTR_FLAG_MIN_PERIOD))
        {
            length += (LINK_ATTR_SEPARATOR_SIZE + ATTR_MIN_PERIOD_LEN + utils_numDigit(objectParamP->minPeriod));
        }

        if (paramP->toSet & LWM2M_ATTR_FLAG_MAX_PERIOD)
        {
            length += (LINK_ATTR_SEPARATOR_SIZE + ATTR_MAX_PERIOD_LEN + utils_numDigit(paramP->maxPeriod));
        }
        else if ((objectParamP != NULL) && (objectParamP->toSet & LWM2M_ATTR_FLAG_MAX_PERIOD))
        {
            length += (LINK_ATTR_SEPARATOR_SIZE + ATTR_MAX_PERIOD_LEN + utils_numDigit(objectParamP->maxPeriod));
        }
        if (paramP->toSet & LWM2M_ATTR_FLAG_GREATER_THAN)
        {
            length += (LINK_ATTR_SEPARATOR_SIZE + ATTR_GREATER_THAN_LEN + utils_numDigit(paramP->greaterThan));
        }
        if (paramP->toSet & LWM2M_ATTR_FLAG_LESS_THAN)
        {
            length += (LINK_ATTR_SEPARATOR_SIZE + ATTR_LESS_THAN_LEN + utils_numDigit(paramP->lessThan));
        }
        if (paramP->toSet & LWM2M_ATTR_FLAG_STEP)
        {
           length += (LINK_ATTR_SEPARATOR_SIZE + ATTR_STEP_LEN + utils_numDigit(paramP->step));
        }
        length += LINK_ITEM_ATTR_END_SIZE;
       
    }

    if (length > 0) length -= 1;

    return length;
}

/**
 * @fn static int serialized_linkdata_size()
 * @brief This function is used to get size of link data
 *
 * @param contextP    handle to lwm2m context
 * @param tlvP        handle to lwm2m data
 * @param parentUriP  handle to parent uri
 * @param parentUriLen  length of the uri
 * @return   -1 on failure
 *           
 */
static int serialized_linkdata_size(lwm2m_context_t * contextP,
    lwm2m_data_t * tlvP,
	lwm2m_attributes_t * objectParamP,
    lwm2m_uri_t * parentUriP,
    size_t parentUriLen,
    boolean isObjectLevel)
{
  int length;
  int res;
  lwm2m_uri_t uri;
  if( contextP == NULL || tlvP == NULL || parentUriP == NULL) 
  { 
    LOG_DEBUG(" Passed  Argument is NULL");
    return -1 ;
  }
  length = 0;

  switch (tlvP->type)
  {
    case LWM2M_TYPE_UNDEFINED:
    case LWM2M_TYPE_STRING:
    case LWM2M_TYPE_OPAQUE:
    case LWM2M_TYPE_INTEGER:
    case LWM2M_TYPE_FLOAT:
    case LWM2M_TYPE_BOOLEAN:
    case LWM2M_TYPE_OBJECT_LINK:
    case LWM2M_TYPE_MULTIPLE_RESOURCE:
        

      length += LINK_ITEM_START_SIZE + parentUriLen + LINK_URI_SEPARATOR_SIZE + utils_numDigit(tlvP->id);

      if ((tlvP->type == LWM2M_TYPE_MULTIPLE_RESOURCE) && (isObjectLevel != TRUE))
      {
        length += (LINK_ITEM_DIM_START_SIZE + utils_numDigit(tlvP->value.asChildren.count) + LINK_ITEM_ATTR_END_SIZE );
      }
      else
      {
        length += LINK_ITEM_END_SIZE;
      }
      LWM2M_MEMCPY(&uri, sizeof(lwm2m_uri_t), parentUriP, sizeof(lwm2m_uri_t));
      uri.resourceId = tlvP->id;
      uri.flag |= LWM2M_URI_FLAG_RESOURCE_ID;
      if(isObjectLevel != TRUE)
      {
        res = serialized_attributes_size(contextP, &uri, objectParamP);	  
        if (res < 0) return -1;    // careful, 0 is valid
        if (res > 0) length += res;
      }

      break;

    case LWM2M_TYPE_OBJECT_INSTANCE:
     {
        size_t uriLen ;
        size_t index;

        uriLen = parentUriLen + LINK_URI_SEPARATOR_SIZE + utils_numDigit(tlvP->id);
        length = LINK_ITEM_START_SIZE + uriLen + LINK_ITEM_END_SIZE;

        LWM2M_MEMCPY(&uri, sizeof(lwm2m_uri_t), parentUriP, sizeof(lwm2m_uri_t));
        uri.instanceId = tlvP->id;
        uri.flag |= LWM2M_URI_FLAG_INSTANCE_ID;

        for (index = 0; index < tlvP->value.asChildren.count; index++)
        {
          res = serialized_linkdata_size(contextP, tlvP->value.asChildren.array + index, objectParamP, &uri, uriLen, TRUE);
          if (res < 0) return -1;
          length += res;
        }
     }
        break;

   case LWM2M_TYPE_OBJECT:
    break;
   default:
      return -1;
  }

  return length;
}


/**
 * @fn int discover_serialize_get_size()
 * @brief This function is used to copy lwm2m data into given buffer
 *
 * @param contextP    handle to lwm2m context
 * @param uriP        handle to uri
 * @param size        number of elements in dataP
 * @param paramP      parent attributes
 * @param parentUri   parent URI
 *
 * @return   -1 on failure size on suceess
 *           
 */
int discover_serialize_get_size(lwm2m_context_t * contextP,
    lwm2m_uri_t * uriP,
    int size,
    lwm2m_data_t * dataP, 
    lwm2m_attributes_t * paramP,
    lwm2m_uri_t parentUri,
    bool flag_resID)
{
   uint8_t baseUriStr[URI_MAX_STRING_LEN];
  int baseUriLen = 0;
   int index = 0;
   size_t length = 0;
   int res = 0;


  if (flag_resID == false)
  {
        paramP = NULL;

        if (LWM2M_URI_IS_SET_INSTANCE(uriP))
        {
           length += LINK_ITEM_START_SIZE + LINK_URI_SEPARATOR_SIZE + utils_numDigit(uriP->objectId) + LINK_URI_SEPARATOR_SIZE 
                  + utils_numDigit(uriP->instanceId) + LINK_ITEM_END_SIZE;

            parentUri.instanceId = uriP->instanceId;
            parentUri.flag = LWM2M_URI_FLAG_INSTANCE_ID;
            res = serialized_attributes_size(contextP, &parentUri, NULL);
            if (res < 0) return -1;    // careful, 0 is valid
            length += res;
        }
        else
        {
            length += LINK_ITEM_START_SIZE + LINK_URI_SEPARATOR_SIZE + utils_numDigit(uriP->objectId) + LINK_ITEM_END_SIZE;

            res = serialized_attributes_size(contextP, &parentUri, NULL);
            if (res < 0) return -1;    // careful, 0 is valid
                length += res;
        }
    }

    baseUriLen = uri_toString(uriP, baseUriStr, URI_MAX_STRING_LEN, NULL);
    if (baseUriLen < 0) return -1;
    baseUriLen -= 1;

    for (index = 0; index < size ; index++)
    {
        if(LWM2M_URI_IS_SET_INSTANCE(uriP))
          res = serialized_linkdata_size(contextP, dataP + index, paramP, uriP, baseUriLen, FALSE);
        else
          res = serialized_linkdata_size(contextP, dataP + index, paramP, uriP, baseUriLen, TRUE);
        if (res < 0) return -1;
        length += res;
    }
    return (int)length;
}



/**
 * @fn static int prv_serializeAttributes()
 * @brief This function is used to copy attributes into given buffer
 *
 * @param contextP   handle to lwm2m context
 * @param uriP       handle to resource uri
 * @param buffer   handle to result buffer
 * @param uriLen     length of the uri
 * @param bufferLen  length of the buffer
 *
 * @return   -1 on failure
 *           
 */
static int prv_serializeAttributes(lwm2m_context_t * contextP,
                                   lwm2m_uri_t * uriP,
                                   lwm2m_attributes_t * objectParamP,
                                   uint8_t * buffer,
                                   size_t uriLen,
                                   size_t bufferLen)
{
    int head;
    int res;
    lwm2m_attributes_t * paramP;

    head = 0;
    if (contextP == NULL || uriP == NULL || buffer  ==  NULL ) 
    {
      LOG_DEBUG(" Passed  Argument is NULL");
      return -1;
    }
    paramP = prv_findAttributes(contextP, uriP);
    if (paramP == NULL) paramP = objectParamP;

    if (paramP != NULL)
    {
        head = uriLen;

        if (paramP->toSet & LWM2M_ATTR_FLAG_MIN_PERIOD)
        {
            PRV_CONCAT_STR(buffer, bufferLen, head, LINK_ATTR_SEPARATOR, LINK_ATTR_SEPARATOR_SIZE);
            PRV_CONCAT_STR(buffer, bufferLen, head, ATTR_MIN_PERIOD_STR, ATTR_MIN_PERIOD_LEN);

            res = utils_intToText(paramP->minPeriod, buffer + head, bufferLen - head);
            if (res <= 0) return -1;
            head += res;
        }
        else if ((objectParamP) && (objectParamP->toSet & LWM2M_ATTR_FLAG_MIN_PERIOD))
        {
            PRV_CONCAT_STR(buffer, bufferLen, head, LINK_ATTR_SEPARATOR, LINK_ATTR_SEPARATOR_SIZE);
            PRV_CONCAT_STR(buffer, bufferLen, head, ATTR_MIN_PERIOD_STR, ATTR_MIN_PERIOD_LEN);

            res = utils_intToText(objectParamP->minPeriod, buffer + head, bufferLen - head);
            if (res <= 0) return -1;
            head += res;
        }

        if (paramP->toSet & LWM2M_ATTR_FLAG_MAX_PERIOD)
        {
            PRV_CONCAT_STR(buffer, bufferLen, head, LINK_ATTR_SEPARATOR, LINK_ATTR_SEPARATOR_SIZE);
            PRV_CONCAT_STR(buffer, bufferLen, head, ATTR_MAX_PERIOD_STR, ATTR_MAX_PERIOD_LEN);

            res = utils_intToText(paramP->maxPeriod, buffer + head, bufferLen - head);
            if (res <= 0) return -1;
            head += res;
        }
        else if ((objectParamP != NULL) && (objectParamP->toSet & LWM2M_ATTR_FLAG_MAX_PERIOD))
        {
            PRV_CONCAT_STR(buffer, bufferLen, head, LINK_ATTR_SEPARATOR, LINK_ATTR_SEPARATOR_SIZE);
            PRV_CONCAT_STR(buffer, bufferLen, head, ATTR_MAX_PERIOD_STR, ATTR_MAX_PERIOD_LEN);

            res = utils_intToText(objectParamP->maxPeriod, buffer + head, bufferLen - head);
            if (res <= 0) return -1;
            head += res;
        }

        if (paramP->toSet & LWM2M_ATTR_FLAG_GREATER_THAN)
        {
            PRV_CONCAT_STR(buffer, bufferLen, head, LINK_ATTR_SEPARATOR, LINK_ATTR_SEPARATOR_SIZE);
            PRV_CONCAT_STR(buffer, bufferLen, head, ATTR_GREATER_THAN_STR, ATTR_GREATER_THAN_LEN);

            res = utils_floatToText(paramP->greaterThan, buffer + head, bufferLen - head);
            if (res <= 0) return -1;
            head += res;
        }
        if (paramP->toSet & LWM2M_ATTR_FLAG_LESS_THAN)
        {
            PRV_CONCAT_STR(buffer, bufferLen, head, LINK_ATTR_SEPARATOR, LINK_ATTR_SEPARATOR_SIZE);
            PRV_CONCAT_STR(buffer, bufferLen, head, ATTR_LESS_THAN_STR, ATTR_LESS_THAN_LEN);

            res = utils_floatToText(paramP->lessThan, buffer + head, bufferLen - head);
            if (res <= 0) return -1;
            head += res;
        }
        if (paramP->toSet & LWM2M_ATTR_FLAG_STEP)
        {
            PRV_CONCAT_STR(buffer, bufferLen, head, LINK_ATTR_SEPARATOR, LINK_ATTR_SEPARATOR_SIZE);
            PRV_CONCAT_STR(buffer, bufferLen, head, ATTR_STEP_STR, ATTR_STEP_LEN);

            res = utils_floatToText(paramP->step, buffer + head, bufferLen - head);
            if (res <= 0) return -1;
            head += res;
        }
        PRV_CONCAT_STR(buffer, bufferLen, head, LINK_ITEM_ATTR_END, LINK_ITEM_ATTR_END_SIZE);
    }

    if (head > 0) head -= uriLen+1;

    return head;
}

/**
 * @fn static int prv_serializeLinkData()
 * @brief This function is used to copy link data into given buffer
 *
 * @param contextP    handle to lwm2m context
 * @param tlvP        handle to lwm2m data
 * @param parentUriP  handle to parent uri
 * @param parentUriStr  parent uri in string
 * @param parentUriLen  length of the uri
 * @param buffer   handle to result buffer
 * @param bufferLen  length of the buffer
 *
 * @return   -1 on failure
 *           
 */
static int prv_serializeLinkData(lwm2m_context_t * contextP,
    lwm2m_data_t * tlvP,
	lwm2m_attributes_t * objectParamP,
    lwm2m_uri_t * parentUriP,
    uint8_t * parentUriStr,
    size_t parentUriLen,
    uint8_t * buffer,
    size_t bufferLen,
    boolean isObjectLevel)
{
  int head;
  int res;
  lwm2m_uri_t uri;
  if( contextP == NULL || tlvP == NULL || parentUriP == NULL 
      || parentUriStr == NULL || buffer == NULL ) 
  { 
    LOG_DEBUG(" Passed  Argument is NULL");
    return -1 ;
  }
  head = 0;

  switch (tlvP->type)
  {
    case LWM2M_TYPE_UNDEFINED:
    case LWM2M_TYPE_STRING:
    case LWM2M_TYPE_OPAQUE:
    case LWM2M_TYPE_INTEGER:
    case LWM2M_TYPE_FLOAT:
    case LWM2M_TYPE_BOOLEAN:
    case LWM2M_TYPE_OBJECT_LINK:
    case LWM2M_TYPE_MULTIPLE_RESOURCE:
      if (bufferLen < LINK_ITEM_START_SIZE) return -1;
      LWM2M_MEMCPY(buffer + head, bufferLen, LINK_ITEM_START, LINK_ITEM_START_SIZE);
      head = LINK_ITEM_START_SIZE;

      if (parentUriLen > 0)
      {
        if (bufferLen - head < parentUriLen) return -1;
        LWM2M_MEMCPY(buffer + head, bufferLen - head, parentUriStr, parentUriLen);
        head += parentUriLen;
      }

      if (bufferLen - head < LINK_URI_SEPARATOR_SIZE) return -1;
      LWM2M_MEMCPY(buffer + head, bufferLen - head, LINK_URI_SEPARATOR, LINK_URI_SEPARATOR_SIZE);
      head += LINK_URI_SEPARATOR_SIZE;

      res = utils_intToText(tlvP->id, buffer + head, bufferLen - head);
      if (res <= 0) return -1;
      head += res;

      if ((tlvP->type == LWM2M_TYPE_MULTIPLE_RESOURCE) && (isObjectLevel != TRUE))
      {
        if (bufferLen - head < LINK_ITEM_DIM_START_SIZE) return -1;
        LWM2M_MEMCPY(buffer + head, bufferLen - head, LINK_ITEM_DIM_START, LINK_ITEM_DIM_START_SIZE);
        head += LINK_ITEM_DIM_START_SIZE;

        res = utils_intToText(tlvP->value.asChildren.count, buffer + head, bufferLen - head);
        if (res <= 0) return -1;
        head += res;

        if (bufferLen - head < LINK_ITEM_ATTR_END_SIZE) return -1;
        LWM2M_MEMCPY(buffer + head, bufferLen - head, LINK_ITEM_ATTR_END, LINK_ITEM_ATTR_END_SIZE);
        head += LINK_ITEM_ATTR_END_SIZE;
      }
      else
      {
        if (bufferLen - head < LINK_ITEM_END_SIZE) return -1;
        LWM2M_MEMCPY(buffer + head, bufferLen - head, LINK_ITEM_END, LINK_ITEM_END_SIZE);
        head += LINK_ITEM_END_SIZE;
      }

      LWM2M_MEMCPY(&uri, sizeof(lwm2m_uri_t), parentUriP, sizeof(lwm2m_uri_t));
      uri.resourceId = tlvP->id;
      uri.flag |= LWM2M_URI_FLAG_RESOURCE_ID;
      if(isObjectLevel != TRUE)
      {
        res = prv_serializeAttributes(contextP, &uri, objectParamP, buffer, head - 1, bufferLen);	  
        if (res < 0) return -1;    // careful, 0 is valid
        if (res > 0) head += res;
      }

      break;

    case LWM2M_TYPE_OBJECT_INSTANCE:
      {
        uint8_t uriStr[URI_MAX_STRING_LEN];
        size_t uriLen;
        size_t index;

        if (parentUriLen > 0)
        {
          if (URI_MAX_STRING_LEN < parentUriLen) return -1;
          LWM2M_MEMCPY(uriStr, URI_MAX_STRING_LEN, parentUriStr, parentUriLen);
          uriLen = parentUriLen;
        }
        else
        {
          uriLen = 0;
        }

        if (URI_MAX_STRING_LEN - uriLen < LINK_URI_SEPARATOR_SIZE) return -1;
        if (uriLen >= URI_MAX_STRING_LEN) return -1;
        LWM2M_MEMCPY(uriStr + uriLen, URI_MAX_STRING_LEN - uriLen, LINK_URI_SEPARATOR, LINK_URI_SEPARATOR_SIZE);
        uriLen += LINK_URI_SEPARATOR_SIZE;

        res = utils_intToText(tlvP->id, uriStr + uriLen, URI_MAX_STRING_LEN - uriLen);
        if (res <= 0) return -1;
        uriLen += res;

        LWM2M_MEMCPY(&uri, sizeof(lwm2m_uri_t), parentUriP, sizeof(lwm2m_uri_t));
        uri.instanceId = tlvP->id;
        uri.flag |= LWM2M_URI_FLAG_INSTANCE_ID;

        head = 0;

        if (bufferLen < LINK_ITEM_START_SIZE) return -1;
        LWM2M_MEMCPY(buffer + head, bufferLen, LINK_ITEM_START, LINK_ITEM_START_SIZE);
        head = LINK_ITEM_START_SIZE;
		
		if (bufferLen - head < uriLen) return -1;
        LWM2M_MEMCPY(buffer + head, bufferLen - head, uriStr, uriLen);
        head += uriLen;

		if (bufferLen - head < LINK_ITEM_END_SIZE) return -1;
        LWM2M_MEMCPY(buffer + head, bufferLen - head, LINK_ITEM_END, LINK_ITEM_END_SIZE);
        head += LINK_ITEM_END_SIZE;

        for (index = 0; index < tlvP->value.asChildren.count; index++)
        {
          res = prv_serializeLinkData(contextP, tlvP->value.asChildren.array + index, objectParamP, &uri, uriStr, uriLen, buffer + head, bufferLen - head, TRUE);
          if (res < 0) return -1;
          head += res;
        }
      }
      break;

    case LWM2M_TYPE_OBJECT:
    default:
      return -1;
  }

  return head;
}

/**
 * @fn int discover_serialize()
 * @brief This function is used to copy lwm2m data into given buffer
 *
 * @param contextP  handle to lwm2m context
 * @param uriP      handle to uri
 * @param size      number of elements in dataP
 * @param tlvP      handle to lwm2m data
 * @param buffer  handle to result buffer
 *
 * @return   -1 on failure
 *           
 */
int discover_serialize(lwm2m_context_t * contextP,
    lwm2m_uri_t * uriP,
    int size,
    lwm2m_data_t * dataP,
    uint8_t ** bufferP)
{
  uint8_t *bufferLink = NULL;
  uint8_t baseUriStr[URI_MAX_STRING_LEN];
  int baseUriLen = 0;
  int index = 0;
  size_t head = 0;
  int res = 0;
  lwm2m_uri_t parentUri;
  lwm2m_attributes_t * paramP = NULL;
  lwm2m_attributes_t mergedParam;  
  bool flag_resID = false;
  size_t disc_length = 0;

  if ( contextP == NULL || uriP == NULL || dataP == NULL || bufferP == NULL ) 
  { 
    LOG_DEBUG(" Passed  Argument is NULL");
    return -1; 
  }
  LOG_DEBUG("size: %d", size);
  LOG_URI(uriP);

  head = 0;
  memset(&parentUri, 0, sizeof(lwm2m_uri_t));
   parentUri.objectId = uriP->objectId;
    parentUri.flag = LWM2M_URI_FLAG_OBJECT_ID;

    if (LWM2M_URI_IS_SET_RESOURCE(uriP))
    {
        lwm2m_uri_t tempUri;
        lwm2m_attributes_t * objParamP;
        lwm2m_attributes_t * instParamP;

        memset(&parentUri, 0, sizeof(lwm2m_uri_t));
        tempUri.objectId = uriP->objectId;
        tempUri.flag = LWM2M_URI_FLAG_OBJECT_ID;

        // get object level attributes
        objParamP = prv_findAttributes(contextP, &tempUri);
        
        // get object instance level attributes
        tempUri.instanceId = uriP->instanceId;
        tempUri.flag = LWM2M_URI_FLAG_INSTANCE_ID;
        instParamP = prv_findAttributes(contextP, &tempUri);

        if (objParamP != NULL)
        {
            if (instParamP != NULL)
            {
                memset(&mergedParam, 0, sizeof(lwm2m_attributes_t));
                mergedParam.toSet = objParamP->toSet | instParamP->toSet;
                if (mergedParam.toSet & LWM2M_ATTR_FLAG_MIN_PERIOD)
                {
                    if (instParamP->toSet & LWM2M_ATTR_FLAG_MIN_PERIOD)
                    {
                        mergedParam.minPeriod = instParamP->minPeriod;
                    }
                    else
                    {
                        mergedParam.minPeriod = objParamP->minPeriod;
                    }
                }
                if (mergedParam.toSet & LWM2M_ATTR_FLAG_MAX_PERIOD)
                {
                    if (instParamP->toSet & LWM2M_ATTR_FLAG_MAX_PERIOD)
                    {
                        mergedParam.maxPeriod = instParamP->maxPeriod;
                    }
                    else
                    {
                        mergedParam.maxPeriod = objParamP->maxPeriod;
                    }
                }
                paramP = &mergedParam;
            }
            else
            {
                paramP = objParamP;
            }
        }
        else
        {
            paramP = instParamP;
        }
        uriP->flag &= ~LWM2M_URI_FLAG_RESOURCE_ID;
        flag_resID = true;
    }

    disc_length = discover_serialize_get_size(contextP, uriP, size, dataP, paramP, parentUri,flag_resID);

    /* Adding padding of size two because 
      we are using 2 extra unit to prepare payload  */
    disc_length += BUFFER_PADDING_LENGTH;

    bufferLink = (uint8_t *) malloc(disc_length);

    if(bufferLink == NULL)
    {
       ds_iot_ulog_err("no memory for discover payload");
       return -1;
    }

    memset(bufferLink, 0x00, disc_length);

    if(flag_resID == false)
    {
        paramP = NULL;

        if (LWM2M_URI_IS_SET_INSTANCE(uriP))
        {
  
            PRV_CONCAT_STR(bufferLink, disc_length, head, LINK_ITEM_START, LINK_ITEM_START_SIZE);
            PRV_CONCAT_STR(bufferLink, disc_length, head, LINK_URI_SEPARATOR, LINK_URI_SEPARATOR_SIZE);
            res = utils_intToText(uriP->objectId, bufferLink + head, disc_length - head);
            if (res <= 0) 
            {
               goto error;
            }
            head += res;
            PRV_CONCAT_STR(bufferLink, disc_length, head, LINK_URI_SEPARATOR, LINK_URI_SEPARATOR_SIZE);
            res = utils_intToText(uriP->instanceId, bufferLink + head, disc_length - head);
            if (res <= 0) 
            {
               goto error;
            }
            head += res;
            PRV_CONCAT_STR(bufferLink, disc_length, head, LINK_ITEM_END, LINK_ITEM_END_SIZE);
            parentUri.instanceId = uriP->instanceId;
            parentUri.flag = LWM2M_URI_FLAG_INSTANCE_ID;
            res = prv_serializeAttributes(contextP, &parentUri, NULL, bufferLink, head - 1, disc_length);
            if (res < 0)     // careful, 0 is valid
            {
               goto error;
            }
            head += res;
        }
        else
        {
            PRV_CONCAT_STR(bufferLink, disc_length, head, LINK_ITEM_START, LINK_ITEM_START_SIZE);
            PRV_CONCAT_STR(bufferLink, disc_length, head, LINK_URI_SEPARATOR, LINK_URI_SEPARATOR_SIZE);
            res = utils_intToText(uriP->objectId, bufferLink + head, disc_length - head);
            if (res <= 0) 
            {
               goto error;
            }
            head += res;
            PRV_CONCAT_STR(bufferLink, disc_length, head, LINK_ITEM_END, LINK_ITEM_END_SIZE);
            res = prv_serializeAttributes(contextP, &parentUri, NULL, bufferLink, head - 1, disc_length);
            if (res < 0)    // careful, 0 is valid
            {
               goto error;
            }
                head += res;
        }
    }

    baseUriLen = uri_toString(uriP, baseUriStr, URI_MAX_STRING_LEN, NULL);
    if (baseUriLen < 0)
    {
       goto error;
    }
    baseUriLen -= 1;

    for (index = 0; index < size && head < disc_length; index++)
    {
        if(LWM2M_URI_IS_SET_INSTANCE(uriP))
          res = prv_serializeLinkData(contextP, dataP + index, paramP, uriP, baseUriStr, baseUriLen, bufferLink + head, disc_length - head, FALSE);
        else
          res = prv_serializeLinkData(contextP, dataP + index, paramP, uriP, baseUriStr, baseUriLen, bufferLink + head, disc_length - head, TRUE);
        if (res < 0) 
        {
           goto error;
        }
        head += res;
    }


    if (head > 0)
    {
        head -= 1;
        *bufferP = (uint8_t *)lwm2m_malloc(head);
        if (*bufferP == NULL) goto error;
        memset(*bufferP, 0x00, head);
        LWM2M_MEMCPY(*bufferP, head, bufferLink, head);
    }

  free(bufferLink);
  return (int)head;
error:
   if(bufferLink)
     free(bufferLink);
   return -1;
    
}

/**
 * @fn int discover_serialize_wo_inst_get_size()
 * @brief This function is used to get the size of 
 *        without instance
 * @param contextP  handle to lwm2m context
 * @param uriP      handle to uri
 *
 * @return   -1 on failure, size on suceess
 *           
 */
int discover_serialize_wo_inst_get_size(lwm2m_context_t * contextP,
                               lwm2m_uri_t * uriP)
{
  size_t length = 0;
  int res = 0;
  lwm2m_uri_t parentUri;
  if ( contextP == NULL || uriP == NULL) 
  { 
    LOG_DEBUG(" Passed  Argument is NULL");
    return -1; 
  }

  length = 0;
  memset(&parentUri, 0, sizeof(lwm2m_uri_t));
  parentUri.objectId = uriP->objectId;
  parentUri.flag = LWM2M_URI_FLAG_OBJECT_ID;

  length += LINK_ITEM_START_SIZE + LINK_URI_SEPARATOR_SIZE + utils_numDigit(uriP->objectId) + LINK_ITEM_END_SIZE;
  res = serialized_attributes_size(contextP, &parentUri, NULL);
  if (res < 0) return -1;    // careful, 0 is valid
  length += res;
  
  if (length > 0)
  {
     length -= 1;
  }

  return (int)length;
}


/**
 * @fn int discover_serialize_without_instance()
 * @brief This function is used to copy lwm2m data into given buffer for object 
 *        without instance
 * @param contextP  handle to lwm2m context
 * @param uriP      handle to uri
 * @param buffer  handle to result buffer
 *
 * @return   -1 on failure
 *           
 */
int discover_serialize_wo_inst(lwm2m_context_t * contextP,
                               lwm2m_uri_t * uriP,
                               uint8_t ** bufferP)
{
  uint8_t *bufferLink = NULL;
  size_t head = 0;
  int res = 0;
  lwm2m_uri_t parentUri;
  size_t disc_length = 0;
  if ( contextP == NULL || uriP == NULL || bufferP == NULL ) 
  { 
    LOG_DEBUG(" Passed  Argument is NULL");
    return -1; 
  }

  
  disc_length = discover_serialize_wo_inst_get_size(contextP, uriP);

  /* Adding padding of size two because 
      we are using 2 extra unit to prepare payload  */
  disc_length += BUFFER_PADDING_LENGTH;
          
  bufferLink = (uint8_t *) malloc(disc_length);
  
  if(bufferLink == NULL)
  {
    ds_iot_ulog_err("no memory for discover wo inst payload");
    return -1;
  }

  memset(bufferLink, 0x00, disc_length);

  head = 0;
  memset(&parentUri, 0, sizeof(lwm2m_uri_t));
  parentUri.objectId = uriP->objectId;
  parentUri.flag = LWM2M_URI_FLAG_OBJECT_ID;


  PRV_CONCAT_STR(bufferLink, disc_length, head, LINK_ITEM_START, LINK_ITEM_START_SIZE);
  PRV_CONCAT_STR(bufferLink, disc_length, head, LINK_URI_SEPARATOR, LINK_URI_SEPARATOR_SIZE);
  res = utils_intToText(uriP->objectId, bufferLink + head, disc_length - head);
  if (res <= 0) 
  {
     free(bufferLink);
     return -1;
  }
  head += res;
  PRV_CONCAT_STR(bufferLink, disc_length, head, LINK_ITEM_END, LINK_ITEM_END_SIZE);
  res = prv_serializeAttributes(contextP, &parentUri, NULL, bufferLink, head - 1, disc_length);
  if (res < 0)  // careful, 0 is valid
  {
     free(bufferLink);
     return -1;
  }
    head += res;

  if (head > 0)
  {
     head -= 1;
    *bufferP = (uint8_t *)lwm2m_malloc(head);
     if (*bufferP == NULL)
     {
        free(bufferLink);
        return -1;
     }
     memset(*bufferP, 0x00, head);
      LWM2M_MEMCPY(*bufferP, head, bufferLink, head);
  }

  free(bufferLink);
  return (int)head;
}

#endif
