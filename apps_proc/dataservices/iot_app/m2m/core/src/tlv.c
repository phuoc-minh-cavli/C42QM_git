/******************************************************************************

  @file    tlv.c

  ---------------------------------------------------------------------------
  Copyright (c) 2016-2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------

******************************************************************************/

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
 *    Fabien Fleutot - Please refer to git log
 *    Bosch Software Innovations GmbH - Please refer to git log
 *    
 *******************************************************************************/

#include "internals.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <float.h>
#include "ds_log.h"


#ifndef LWM2M_BIG_ENDIAN
#ifndef LWM2M_LITTLE_ENDIAN
#error Please define LWM2M_BIG_ENDIAN or LWM2M_LITTLE_ENDIAN
#endif
#endif

#define _PRV_TLV_TYPE_MASK 0xC0
#define _PRV_TLV_HEADER_MAX_LENGTH 6

#define _PRV_TLV_TYPE_UNKNOWN           (uint8_t)0xFF
#define _PRV_TLV_TYPE_OBJECT            (uint8_t)0x10
#define _PRV_TLV_TYPE_OBJECT_INSTANCE   (uint8_t)0x00
#define _PRV_TLV_TYPE_RESOURCE          (uint8_t)0xC0
#define _PRV_TLV_TYPE_MULTIPLE_RESOURCE (uint8_t)0x80
#define _PRV_TLV_TYPE_RESOURCE_INSTANCE (uint8_t)0x40

/**
 * @fn prv_getHeaderType()
 * @brief This function is used to get the TLV header type based on
 *     lwm2m header type
 * @param type lwm2m header type value 
 * @return  If not found _PRV_TLV_TYPE_UNKNOWN type is returned
 */
static uint8_t prv_getHeaderType(lwm2m_data_type_t type)
{
  switch (type)
  {
    case LWM2M_TYPE_OBJECT:
      return _PRV_TLV_TYPE_OBJECT;

    case LWM2M_TYPE_OBJECT_INSTANCE:
      return _PRV_TLV_TYPE_OBJECT_INSTANCE;

    case LWM2M_TYPE_MULTIPLE_RESOURCE:
      return _PRV_TLV_TYPE_MULTIPLE_RESOURCE;


    case LWM2M_TYPE_STRING:
    case LWM2M_TYPE_INTEGER:
    case LWM2M_TYPE_FLOAT:
    case LWM2M_TYPE_BOOLEAN:
    case LWM2M_TYPE_OPAQUE:
    case LWM2M_TYPE_OBJECT_LINK:
      return _PRV_TLV_TYPE_RESOURCE;

    case LWM2M_TYPE_UNDEFINED:
    default:
      return _PRV_TLV_TYPE_UNKNOWN;
  }
}

/**
 * @fn prv_getDataType()
 * @brief This function is used to get the lwm2m data type based on
 *     tlv data type
 * @param type tlv data type value 
 * @return  If not found LWM2M_TYPE_UNDEFINED type is returned
 */
static lwm2m_data_type_t prv_getDataType(uint8_t type)
{
  switch (type)
  {
    case _PRV_TLV_TYPE_OBJECT:
      return LWM2M_TYPE_OBJECT;

    case _PRV_TLV_TYPE_OBJECT_INSTANCE:
      return LWM2M_TYPE_OBJECT_INSTANCE;

    case _PRV_TLV_TYPE_MULTIPLE_RESOURCE:
      return LWM2M_TYPE_MULTIPLE_RESOURCE;

    case _PRV_TLV_TYPE_RESOURCE:
    case _PRV_TLV_TYPE_RESOURCE_INSTANCE:
      return LWM2M_TYPE_OPAQUE;

    default:
      return LWM2M_TYPE_UNDEFINED;
  }
}

/**
 * @fn prv_getHeaderLength()
 * @brief This function is used to get the actual header length to 
 *     increment the buffer to fetch the data
 * @param id data ID
 * @param datalen data length
 * @return  retuns 2 or more than 2 as per processing
 */
static int prv_getHeaderLength(uint16_t id,
    size_t dataLen)
{
  int length;

  length = 2;

  if (id > 0xFF)
  {
    length += 1;
  }

  if (dataLen > 0xFFFF)
  {
    length += 3;
  }
  else if (dataLen > 0xFF)
  {
    length += 2;
  }
  else if (dataLen > 7)
  {
    length += 1;
  }

  return length;
}

/**
 * @fn prv_createHeader()
 * @brief This function is used to create header 
 * @param header 
 * @param isInstance specifies the resource instance
 * @param type lwm2m data type, eg int, float etc...
 * @param id specifies object instance or resource value
 * @param data_len length of the data to be populated
 * @return returns header length populated
 */
static int prv_createHeader(uint8_t * header,
    bool isInstance,
    lwm2m_data_type_t type,
    uint16_t id,
    size_t data_len)
{
  int header_len = 0;
  int offset = 0;
  uint8_t hdrType = 0;
  if ( header == NULL ) 
  {
    LOG_DEBUG("%s",nullError);
    return 0;
  }
  header_len = prv_getHeaderLength(id, data_len);
  if (isInstance == true)
  {
    hdrType = _PRV_TLV_TYPE_RESOURCE_INSTANCE;
  }
  else
  {
    hdrType = prv_getHeaderType(type);
  }

  header[0] = 0;
  header[0] |= (uint8_t)hdrType & _PRV_TLV_TYPE_MASK;

  if (id > 0xFF)
  {
    header[0] |= 0x20;
    header[1] = (id >> 8) & 0XFF;
    header[2] = id & 0XFF;
    offset = 3;
  }
  else
  {
    header[1] = id;
    offset = 2;
  }
  if (data_len <= 7)
  {
    header[0] += data_len;
  }
  else if (data_len <= 0xFF)
  {
    header[0] |= 0x08;
    header[offset] = data_len;
  }
  else if (data_len <= 0xFFFF)
  {
    header[0] |= 0x10;
    header[offset] = (data_len >> 8) & 0XFF;
    header[offset + 1] = data_len & 0XFF;
  }
  else if (data_len <= 0xFFFFFF)
  {
    header[0] |= 0x18;
    header[offset] = (data_len >> 16) & 0XFF;
    header[offset + 1] = (data_len >> 8) & 0XFF;
    header[offset + 2] = data_len & 0XFF;
  }

  return header_len;
}

/**
 * @fn lwm2m_decode_TLV()
 * @brief This function is used to decode the recieved TLV data
 * @param buffer input TLV buffer
 * @param buffer_len buffer length
 * @param oType output type value
 * @param oID output object ID
 * @param oDataIndex output number of instances
 * @param oDatalen output length of the data 
 * @return 0 on failure
 *     valid length decoded on success
 */

int lwm2m_decode_TLV(const uint8_t * buffer,
    size_t buffer_len,
    lwm2m_data_type_t * oType,
    uint16_t * oID,
    size_t * oDataIndex,
    size_t * oDataLen)
{

  if( buffer == NULL || oType == NULL || oID == NULL 
      || oDataIndex == NULL || oDataLen == NULL ) 
  {
    LOG_DEBUG("%s",nullError);
    return 0;
  }

  LOG_DEBUG("buffer_len: %d", buffer_len);
  ;
  if (buffer_len < 2) return 0;

  *oDataIndex = 2;

  *oType = prv_getDataType(buffer[0]&_PRV_TLV_TYPE_MASK);

  if ((buffer[0]&0x20) == 0x20)
  {
    // id is 16 bits long
    if (buffer_len < 3) return 0;
    *oDataIndex += 1;
    *oID = (buffer[1]<<8) + buffer[2];
  }
  else
  {
    // id is 8 bits long
    *oID = buffer[1];
  }

  switch (buffer[0]&0x18)
  {
    case 0x00:
      // no length field
      *oDataLen = buffer[0]&0x07;
      break;
    case 0x08:
      // length field is 8 bits long
      if (buffer_len < *oDataIndex + 1) return 0;
      *oDataLen = buffer[*oDataIndex];
      *oDataIndex += 1;
      break;
    case 0x10:
      // length field is 16 bits long
      if (buffer_len < *oDataIndex + 2) return 0;
      *oDataLen = (buffer[*oDataIndex]<<8) + buffer[*oDataIndex+1];
      *oDataIndex += 2;
      break;
    case 0x18:
      // length field is 24 bits long
      if (buffer_len < *oDataIndex + 3) return 0;
      *oDataLen = (buffer[*oDataIndex]<<16) + (buffer[*oDataIndex+1]<<8) + buffer[*oDataIndex+2];
      *oDataIndex += 3;
      break;
    default:
      // can't happen
      return 0;
  }

  if (*oDataIndex + *oDataLen > buffer_len) return 0;

  return *oDataIndex + *oDataLen;
}

/**
 * @fn tlv_parse()
 * @brief This function is used to parse the tlv values received
 * @param buffer input buffer to be parsed
 * @param bufferLen buffer length 
 * @param dataP output buffer after parsing is completed
 * @return 0 on failure
 *     number of elements parsed on success
 */
int tlv_parse(uint8_t * buffer,
    size_t bufferLen,
    lwm2m_data_t ** dataP)
{
  lwm2m_data_type_t type;
  uint16_t id = 0;
  size_t dataIndex = 0;
  size_t dataLen = 0;
  int index = 0;
  int result = 0;
  int size = 0;
  if ( dataP == NULL ) 
  {
    LOG_DEBUG("%s",nullError);
    return 0;
  }

  LOG_DEBUG("bufferLen: %d", bufferLen);

  *dataP = NULL;

  while (0 != (result = lwm2m_decode_TLV((uint8_t*)buffer + index, bufferLen - index, &type, &id, &dataIndex, &dataLen)))
  {
    lwm2m_data_t * newTlvP;

    newTlvP = lwm2m_data_new(size + 1);
    if (newTlvP == NULL)
    {
      lwm2m_data_free(size, *dataP);
      return 0;
    }
    if (size >= 1 && *dataP != NULL)
    {
      LWM2M_MEMCPY(newTlvP, (size + 1) * sizeof(lwm2m_data_t), *dataP, size * sizeof(lwm2m_data_t));
      lwm2m_free(*dataP);
    }
    *dataP = newTlvP;

    (*dataP)[size].type = type;
    (*dataP)[size].id = id;
    if (type == LWM2M_TYPE_OBJECT_INSTANCE || type == LWM2M_TYPE_MULTIPLE_RESOURCE)
    {
      (*dataP)[size].value.asChildren.count = tlv_parse(buffer + index + dataIndex,
          dataLen,
          &((*dataP)[size].value.asChildren.array));
      if ((*dataP)[size].value.asChildren.count == 0)
      {
        lwm2m_data_free(size + 1, *dataP);
        return 0;
      }
    }
    else
    {
      lwm2m_data_encode_opaque(buffer + index + dataIndex, dataLen, (*dataP) + size);
    }
    size++;
    index += result;
  }

  return size;
}

/**
 * @fn prv_getLength()
 * @brief This function is used to get the length of the each item in the
 *        tlv items
 * @param size number of items in the decoded tlv data
 * @param dataP decoded data
 * @return  -1 on failure
 *      length of the data on success
 */
static size_t prv_getLength(int size,
    lwm2m_data_t * dataP)
{
  int length = 0;
  int i = 0;

  length = 0;
  if ( dataP == NULL ) 
  {
    LOG_DEBUG("%s",nullError);
    return 0;
  }
  for (i = 0 ; i < size && length != -1 ; i++)
  {
    switch (dataP[i].type)
    {
      case LWM2M_TYPE_OBJECT_INSTANCE:
        {
          int subLength;

          subLength = prv_getLength(dataP[i].value.asChildren.count, dataP[i].value.asChildren.array);
          if (subLength == -1)
          {
            length = -1;
          }
          else
          {
            length += prv_getHeaderLength(dataP[i].id, subLength) + subLength;
          }
        }
        break;
        case LWM2M_TYPE_MULTIPLE_RESOURCE:
        {
          int subLength;

          if(dataP[i].value.asChildren.count == 0 || dataP[i].value.asChildren.array == NULL)
          {
            subLength = 0;
            length += prv_getHeaderLength(dataP[i].id, subLength) + subLength;
            break;
          }

          subLength = prv_getLength(dataP[i].value.asChildren.count, dataP[i].value.asChildren.array);
          if (subLength == -1)
          {
            length = -1;
          }
          else
          {
            length += prv_getHeaderLength(dataP[i].id, subLength) + subLength;
          }
        }
        break;

      case LWM2M_TYPE_STRING:
      case LWM2M_TYPE_OPAQUE:
        length += prv_getHeaderLength(dataP[i].id, dataP[i].value.asBuffer.length) + dataP[i].value.asBuffer.length;
        break;

      case LWM2M_TYPE_INTEGER:
        {
          size_t data_len;
          uint8_t unused_buffer[_PRV_64BIT_BUFFER_SIZE];

          data_len = utils_encodeInt(dataP[i].value.asInteger, unused_buffer);
          length += prv_getHeaderLength(dataP[i].id, data_len) + data_len;
        }
        break;

      case LWM2M_TYPE_FLOAT:
        {
          size_t data_len;

          if ((dataP[i].value.asFloat < 0.0 - (double)FLT_MAX)
              || (dataP[i].value.asFloat >(double)FLT_MAX))
          {
            data_len = 8;
          }
          else
          {
            data_len = 4;
          }

          length += prv_getHeaderLength(dataP[i].id, data_len) + data_len;
        }
        break;

      case LWM2M_TYPE_BOOLEAN:
        // Booleans are always encoded on one byte
        length += prv_getHeaderLength(dataP[i].id, 1) + 1;
        break;

      case LWM2M_TYPE_OBJECT_LINK:
        // Object Link are always encoded on four bytes
        length += prv_getHeaderLength(dataP[i].id, 4) + 4;
        break;

      default:
        length = -1;
        break;
    }
  }

  if (length < 0)
  {
    return 0;
  }
  else
  {
    return (size_t)length;
  }
}

/**
 * @fn tlv_serialize()
 * @brief This function is used to form the buffer to be sent into 
 *     n/w in tlv format
 * @param isResourceInstance indicates if its a resource instance that
 *     is being serialized
 * @param size number of entitites to be serialized
 * @param dataP lwm2m data to be serialized
 * @param bufferP output buffer
 */
size_t tlv_serialize(bool isResourceInstance, 
    int size,
    lwm2m_data_t * dataP,
    uint8_t ** bufferP)
{
  size_t length = 0;
  int index = 0;
  int i = 0;
  if ( dataP == NULL || bufferP == NULL ) 
  {
    ds_iot_ulog_err("LWM2M_LOG: NULL Error");
    return 0;
  }
  LOG_DEBUG("isResourceInstance: %s, size: %d", isResourceInstance?"true":"false", size);

  *bufferP = NULL;
  length = prv_getLength(size, dataP);
  if (length <= 0) return length;

  *bufferP = (uint8_t *)lwm2m_malloc(length);
  if (*bufferP == NULL) return 0;

  index = 0;
  for (i = 0 ; i < size && length != 0 ; i++)
  {
    int headerLen;
    bool isInstance;

    isInstance = isResourceInstance;
    switch (dataP[i].type)
    {
      case LWM2M_TYPE_MULTIPLE_RESOURCE:
        
        {
          uint8_t * tmpBuffer;
          size_t tmpLength;

          isInstance = true;

          if(dataP[i].value.asChildren.count == 0 || dataP[i].value.asChildren.array == NULL)
          {
            uint8_t buf = 0;
            tmpLength = 0;
            headerLen = prv_createHeader(*bufferP + index, false, dataP[i].type, dataP[i].id, tmpLength);
            index += headerLen;
            LWM2M_MEMCPY(*bufferP + index, length - index, &buf, tmpLength);
            index += tmpLength;
            break;
          }

          tmpLength = tlv_serialize(isInstance, dataP[i].value.asChildren.count, dataP[i].value.asChildren.array, &tmpBuffer);
          if (tmpLength == 0)
          {
            length = 0;
          }
          else
          {
            headerLen = prv_createHeader(*bufferP + index, false, dataP[i].type, dataP[i].id, tmpLength);
            index += headerLen;
            LWM2M_MEMCPY(*bufferP + index, length - index, tmpBuffer, tmpLength);
            index += tmpLength;
            lwm2m_free(tmpBuffer);
          }
        }
        break;
        // fall throught
      case LWM2M_TYPE_OBJECT_INSTANCE:
        {
          uint8_t * tmpBuffer;
          size_t tmpLength;

          tmpLength = tlv_serialize(isInstance, dataP[i].value.asChildren.count, dataP[i].value.asChildren.array, &tmpBuffer);
          if (tmpLength == 0)
          {
            length = 0;
          }
          else
          {
            headerLen = prv_createHeader(*bufferP + index, false, dataP[i].type, dataP[i].id, tmpLength);
            index += headerLen;
            LWM2M_MEMCPY(*bufferP + index, length - index, tmpBuffer, tmpLength);
            index += tmpLength;
            lwm2m_free(tmpBuffer);
          }
        }
        break;

      case LWM2M_TYPE_STRING:
      case LWM2M_TYPE_OPAQUE:
        headerLen = prv_createHeader(*bufferP + index, isInstance, dataP[i].type, dataP[i].id, dataP[i].value.asBuffer.length);
        // following piece of code is put into #if 0 to address klockwork issues as prv_createHeader() will never return 0
#if 0     
        if (headerLen == 0)
        {
          length = 0;
        }
        else
#endif        
        {
          index += headerLen;
          LWM2M_MEMCPY(*bufferP + index, length - index, dataP[i].value.asBuffer.buffer, dataP[i].value.asBuffer.length);
          index += dataP[i].value.asBuffer.length;
        }
        break;

      case LWM2M_TYPE_OBJECT_LINK:
      {
        size_t data_len;
        uint8_t data_buffer[_PRV_64BIT_BUFFER_SIZE];

        data_len = utils_encodeObjectLink(dataP[i].value.asObjLink, data_buffer);
        headerLen = prv_createHeader(*bufferP + index, isInstance, dataP[i].type, dataP[i].id, data_len);
        index += headerLen;
        LWM2M_MEMCPY(*bufferP + index, length - index, data_buffer, data_len);
        index += data_len;
       
        break;  
      }
 
      case LWM2M_TYPE_INTEGER:
        {
          size_t data_len;
          uint8_t data_buffer[_PRV_64BIT_BUFFER_SIZE];

          data_len = utils_encodeInt(dataP[i].value.asInteger, data_buffer);
          headerLen = prv_createHeader(*bufferP + index, isInstance, dataP[i].type, dataP[i].id, data_len);
          // following piece of code is put into #if 0 to address klockwork issues as prv_createHeader() will never return 0
#if 0
          if (headerLen == 0)
          {
            length = 0;
          }
          else
#endif
          {
            index += headerLen;
            LWM2M_MEMCPY(*bufferP + index, length - index, data_buffer, data_len);
            index += data_len;
          }
        }
        break;

      case LWM2M_TYPE_FLOAT:
        {
          size_t data_len;
          uint8_t data_buffer[_PRV_64BIT_BUFFER_SIZE];

          data_len = utils_encodeFloat(dataP[i].value.asFloat, data_buffer);
          headerLen = prv_createHeader(*bufferP + index, isInstance, dataP[i].type, dataP[i].id, data_len);
          // following piece of code is put into #if 0 to address klockwork issues as prv_createHeader() will never return 0
#if 0
          if (headerLen == 0)
          {
            length = 0;
          }
          else
#endif
          {
            index += headerLen;
            LWM2M_MEMCPY(*bufferP + index, length - index, data_buffer, data_len);
            index += data_len;
          }
        }
        break;

      case LWM2M_TYPE_BOOLEAN:
        headerLen = prv_createHeader(*bufferP + index, isInstance, dataP[i].type, dataP[i].id, 1);
        // following piece of code is put into #if 0 to address klockwork issues as prv_createHeader() will never return 0
#if 0
        if (headerLen == 0)
        {
          length = 0;
        }
        else
#endif
        {
          index += headerLen;
          (*bufferP)[index] = dataP[i].value.asBoolean ? 1 : 0;
          index += 1;
        }
        break;

      default:
        length = 0;
        break;
    }
  }

  if (length == 0)
  {
    lwm2m_free(*bufferP);
    *bufferP = NULL;
  }

  LOG_DEBUG("returning %u", length);

  return length;
}

