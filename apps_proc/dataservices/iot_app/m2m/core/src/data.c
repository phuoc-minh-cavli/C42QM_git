/******************************************************************************

  @file    data.c

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
#include <float.h>
#include "ds_log.h"


// dataP array length is assumed to be 1.
/**
 * @fn static size_t prv_textSerialize()
 * @brief This function is used to convert data into text format
 *
 * @param dataP     data lwm2m_data_t strucuture pointer
 * @param bufferP   double pointer to hold result
 *
 * @return    number of bytes copied on success
 *        Zero on failure
 */
static size_t prv_textSerialize(lwm2m_data_t * dataP,
    uint8_t ** bufferP)
{

  if( dataP == NULL || bufferP == NULL ) 
  {
    LOG_DEBUG("Passed  Argument is NULL");
    return 0;
  }

  switch (dataP->type)
  {
    case LWM2M_TYPE_STRING:
    {
      *bufferP = (uint8_t *)lwm2m_malloc(dataP->value.asBuffer.length);
      if (*bufferP == NULL) return 0;
      LWM2M_MEMCPY(*bufferP, dataP->value.asBuffer.length, dataP->value.asBuffer.buffer, dataP->value.asBuffer.length);
      return dataP->value.asBuffer.length;
    }

    case LWM2M_TYPE_INTEGER:
      return utils_int64ToPlainText(dataP->value.asInteger, bufferP);

    case LWM2M_TYPE_FLOAT:
      return utils_float64ToPlainText(dataP->value.asFloat, bufferP);

    case LWM2M_TYPE_BOOLEAN:
      return utils_boolToPlainText(dataP->value.asBoolean, bufferP);

    case LWM2M_TYPE_OBJECT_LINK:
    { 
      char link_buf[12]; 
      uint8_t link_len = 0x00;
      uint16_t obj_id = dataP->value.asObjLink >> 16;
      uint16_t inst_id = dataP->value.asObjLink & 0xFFFF;

      memset(link_buf, 0x00, sizeof(link_buf));
      snprintf(link_buf, sizeof(link_buf), "%u:%u", obj_id, inst_id);

      link_len = strlen(link_buf);
      *bufferP = (uint8_t *)lwm2m_malloc(link_len);
      if (*bufferP == NULL) return 0;

      LWM2M_MEMCPY(*bufferP, link_len, link_buf, link_len);
      return link_len;
    } 

    case LWM2M_TYPE_OPAQUE:
    {
      size_t length;

      length = prv_getBase64Size(dataP->value.asBuffer.length);
      *bufferP = (uint8_t *)lwm2m_malloc(length);
      if (*bufferP == NULL) return 0;
      length = utils_base64Encode(dataP->value.asBuffer.buffer, dataP->value.asBuffer.length, *bufferP, length);
      if (length == 0)
      {
        lwm2m_free(*bufferP);
        *bufferP = NULL;
        return 0;
      }
      return (int)length;
    }
    case LWM2M_TYPE_UNDEFINED:
    default:
      return 0;
  }
}

/**
 * @fn static int prv_setBuffer()
 * @brief This function is used to set buffer
 *
 * @param dataP     data lwm2m_data_t strucuture pointer
 * @param buffer    pointer to the value
 * @param bufferLen buffer length
 *
 * @return    1 on success
 *        Zero on failure
 */
static int prv_setBuffer(lwm2m_data_t * dataP,
    uint8_t * buffer,
    size_t bufferLen)
{
  if( dataP == NULL || buffer == NULL || bufferLen == 0 ) 
  { 

    LOG_DEBUG(" Passed  Argument is NULL");
    return 0;
  }

  dataP->value.asBuffer.buffer = (uint8_t *)lwm2m_malloc(bufferLen);
  if (dataP->value.asBuffer.buffer == NULL)
  {
    return 0;
  }
  dataP->value.asBuffer.length = bufferLen;
  LWM2M_MEMCPY(dataP->value.asBuffer.buffer, bufferLen, buffer, bufferLen);

  return 1;
}

/**
 * @fn lwm2m_data_t * lwm2m_data_new()
 * @brief This function is used to allocate memory to lwm2m_data_t type
 *
 * @param size   size, to be allocated
 *
 * @return    pointer of lwm2m_data_t type on success
 *        NULL on failure
 */
lwm2m_data_t * lwm2m_data_new(int size)
{
  lwm2m_data_t * dataP = NULL;

  LOG_DEBUG("size: %d", size);
  if (size <= 0) return NULL;

  dataP = (lwm2m_data_t *)lwm2m_malloc(size * sizeof(lwm2m_data_t));

  if (dataP != NULL)
  {
    memset(dataP, 0, size * sizeof(lwm2m_data_t));
  }

  return dataP;
}

/**
 * @fn void lwm2m_data_free()
 * @brief This function is used to free, allocated memory to lwm2m_data_t type
 *
 * @param size      size, to be allocated
 * @param dataP     data lwm2m_data_t strucuture pointer 
 *
 * @return    void
 */
void lwm2m_data_free(int size,
    lwm2m_data_t * dataP)
{
  int i = 0;

  LOG_DEBUG("size: %d", size);
  if (size == 0 || dataP == NULL) return;

  for (i = 0; i < size; i++)
  {
    switch (dataP[i].type)
    {
      case LWM2M_TYPE_MULTIPLE_RESOURCE:
      case LWM2M_TYPE_OBJECT_INSTANCE:
      case LWM2M_TYPE_OBJECT:
        lwm2m_data_free(dataP[i].value.asChildren.count, dataP[i].value.asChildren.array);
        break;

      case LWM2M_TYPE_STRING:
      case LWM2M_TYPE_OPAQUE:
        if (dataP[i].value.asBuffer.buffer != NULL)
        {
          lwm2m_free(dataP[i].value.asBuffer.buffer);
        }

      default:
        // do nothing
        break;
    }
  }
  lwm2m_free(dataP);
}

void lwm2m_data_free_ext(int size,
                                 lwm2m_data_t * dataP)
{
  int i = 0;

  LOG_DEBUG("size: %d", size);
  if (size == 0 || dataP == NULL) return;

  for (i = 0; i < size; i++)
  {
    switch (dataP[i].type)
    {
      case LWM2M_TYPE_MULTIPLE_RESOURCE:
      case LWM2M_TYPE_OBJECT_INSTANCE:
      case LWM2M_TYPE_OBJECT:
        lwm2m_flat_data_release(dataP[i].value.asChildren.count, &dataP[i].value.asChildren.array);
        break;

      case LWM2M_TYPE_STRING:
      case LWM2M_TYPE_OPAQUE:
        if (dataP[i].value.asBuffer.buffer != NULL)
        {
          lwm2m_free(dataP[i].value.asBuffer.buffer);
        }

      default:
        // do nothing
        break;
    }
  }
  lwm2m_free(dataP);
}

/**
 * @fn void lwm2m_data_encode_string()
 * @brief This function is used to write string type of data into
 *        lwm2m_data_t structure
 *
 * @param string    pointer to the data to be written
 * @param dataP     data lwm2m_data_t strucuture pointer 
 *
 * @return    void
 */
void lwm2m_data_encode_string(const char * string,
    lwm2m_data_t * dataP)
{
  size_t len = 0;
  int res = 0;

  if(dataP == NULL)
  {
    LOG_DEBUG("Passed NULL Arguments");
    return;
  }

  LOG_DEBUG("\"%s\"", string);
  if (string == NULL)
  {
    len = 0;
  }
  else
  {
    for (len = 0; string[len] != 0; len++);
  }

  if (len == 0)
  {
    dataP->value.asBuffer.length = 0;
    dataP->value.asBuffer.buffer = NULL;
    res = 1;
  }
  else
  {
    res = prv_setBuffer(dataP, (uint8_t *)string, len);
  }

  if (res == 1)
  {
    dataP->type = LWM2M_TYPE_STRING;
  }
  else
  {
    dataP->type = LWM2M_TYPE_UNDEFINED;
  }
}

/**
 * @fn void lwm2m_data_encode_opaque()
 * @brief This function is used to write opaque type of data into
 *        lwm2m_data_t structure
 *
 * @param buffer       buffer data to be written
 * @param length        length of the buffer
 * @param dataP     data lwm2m_data_t strucuture pointer
 *
 * @return    void
 */
void lwm2m_data_encode_opaque(uint8_t * buffer,
    size_t length,
    lwm2m_data_t * dataP)
{
  int res;
  if (dataP == NULL) 
  {
    LOG_DEBUG("Passed  Argument is NULL");
    return ;
  }
  LOG_DEBUG("length: %d, buffer: \"%s\"", length, buffer);
  if (length == 0 || buffer == NULL)
  {
    dataP->value.asBuffer.length = 0;
    dataP->value.asBuffer.buffer = NULL;
    res = 1;
  }
  else
  {
    res = prv_setBuffer(dataP, buffer, length);
  }

  if (res == 1)
  {
    dataP->type = LWM2M_TYPE_OPAQUE;
  }
  else
  {
    dataP->type = LWM2M_TYPE_UNDEFINED;
  }
}

/**
 * @fn void lwm2m_data_encode_nstring()
 * @brief This function is used to write 'n' length of string type data 
 *        into lwm2m_data_t structure
 *
 * @param string    pointer to hold data to be written
 * @param length    length of the string
 * @param dataP     data lwm2m_data_t strucuture pointer
 *
 * @return    void
 */
void lwm2m_data_encode_nstring(const char * string,
    size_t length,
    lwm2m_data_t * dataP)
{ 
  if(dataP == NULL) 
  {   
    LOG_DEBUG(" Passed  Argument is NULL");
    return ;
  }

  LOG_DEBUG("length: %d, string: \"%s\"", length, string);
  lwm2m_data_encode_opaque((uint8_t *)string, length, dataP);

  if (dataP->type == LWM2M_TYPE_OPAQUE)
  {
    dataP->type = LWM2M_TYPE_STRING;
  }
}

/**
 * @fn void lwm2m_data_encode_int()
 * @brief This function is used to write int type data into 
 *        lwm2m_data_t structure
 *
 * @param value     int data to be written
 * @param dataP     data lwm2m_data_t strucuture pointer 
 *
 * @return    void
 */
void lwm2m_data_encode_int(int64_t value,
    lwm2m_data_t * dataP)
{
  LOG_DEBUG("value: %" PRId64 "", value);
  if( dataP == NULL ) 
  { 
    LOG_DEBUG(" Passed  Argument is NULL");
    return ;
  }

  dataP->type = LWM2M_TYPE_INTEGER;
  dataP->value.asInteger = value;
}

/**
 * @fn int lwm2m_data_decode_int()
 * @brief This function is used to read int type of data from
 *        lwm2m_data_t structure
 *
 * @param dataP     data lwm2m_data_t strucuture pointer 
 * @param valueP    pointer to hold the value
 *
 * @return    1 on success
 *            0 on failure
 */
int lwm2m_data_decode_int(const lwm2m_data_t * dataP,
    int64_t * valueP)
{
  int result;
  if ( dataP == NULL || valueP == NULL ) 
  {
    LOG_DEBUG("Passed  Argument is NULL");
    return 0;
  }
  LOG_DEBUG("Entering");
  switch (dataP->type)
  {
    case LWM2M_TYPE_INTEGER:
      *valueP = dataP->value.asInteger;
      result = 1;
      break;

    case LWM2M_TYPE_STRING:
      result = utils_plainTextToInt64(dataP->value.asBuffer.buffer, dataP->value.asBuffer.length, valueP);
      break;

    case LWM2M_TYPE_OPAQUE:
      result = utils_opaqueToInt(dataP->value.asBuffer.buffer, dataP->value.asBuffer.length, valueP);
      if (result == (int)dataP->value.asBuffer.length)
      {
        result = 1;
      }
      else
      {
        result = 0;
      }
      break;

    default:
      return 0;
  }
  LOG_DEBUG("result: %d, value: %" PRId64, result, *valueP);

  return result;
}

/**
 * @fn int lwm2m_data_decode_objLink()
 * @brief This function is used to read object link type of data from
 *        lwm2m_data_t structure
 *
 * @param dataP     data lwm2m_data_t strucuture pointer 
 * @param valueP    pointer to hold the value
 *
 * @return    1 on success
 *            0 on failure
 */
int lwm2m_data_decode_objLink(const lwm2m_data_t * dataP,
    objlink_t * valueP)
{
  if ( dataP == NULL || valueP == NULL ) 
  {
    LOG_DEBUG("Passed  Argument is NULL");
    return 0;
  }

  LOG_DEBUG("Entering");

  switch (dataP->type)
  {
    case LWM2M_TYPE_STRING:
    {  
      int cntr = 0; 
      uint8_t *objlnk_buf = dataP->value.asBuffer.buffer;
      uint8_t  objlnk_len = dataP->value.asBuffer.length;   
 
      /* Object link is set to STRING in cases where the configuration is sent from the server using TEXT format */  
      /* Object link (FFFF:FFFF) is within the allowed size of 9 bytes (+1 for the ':') */
      /* Bytes before the colon corresponds to object information and after corresponds to instance information */
      if (objlnk_len > 0x0B)
        return 0;

      for (cntr = 0; cntr < objlnk_len; cntr++)
      {
        /* Iterate through the buffer and search for ":" */
        if (':' == objlnk_buf[cntr])
        {
          int64_t object = 0x00;
          int64_t object_inst = 0x00; 

          /* Perform the data conversion for the object information */
          if (0 == utils_plainTextToInt64(objlnk_buf, cntr, &object))
            return 0;
 
          /* Perform the data conversion for the object instance information */
          if (0 == utils_plainTextToInt64(&objlnk_buf[cntr + 1], (objlnk_len - (cntr + 1)), &object_inst))
            return 0;
 
          /* Object and instance IDs should be a maximum size of 16-bit */
          if ((object > 65535) || (object_inst > 65535))
            return 0;          
 
          /* Object link information */
         *valueP = (objlink_t)object << 16 | object_inst;
          break;
        }
      }

      /* Invalid object link information received from the server */
      if (cntr >= objlnk_len) 
        return 0;
 
      break;
    }
    
    case LWM2M_TYPE_OPAQUE:
    {
      int64_t object_lnk = 0x00;

      /* Object link is set to OPAQUE in cases where the configuration is sent from the server using TLV format */  
      /* Object link size for TLV should be exactly four bytes (0xFFFFFFFF) */
      /* Upper 16 bits corresponds to object information and lower 16 bits corresponds to instance information */      
      //if (dataP->value.asBuffer.length != 0x04)
        //return 0;
 
      /* Perform the data conversion to object link */ 
      if (dataP->value.asBuffer.length != utils_opaqueToInt(dataP->value.asBuffer.buffer, dataP->value.asBuffer.length, &object_lnk))
        return 0;

      /* Update the retreived object link information */
      *valueP = (objlink_t)object_lnk; 
      break;
    }

    case LWM2M_TYPE_OBJECT_LINK:
    {
      /* Object link is set to OBJECT_LINK in cases where the configuration is read from bootstrap.ini and parsed using JSON decoder */ 
      /* Object link size for TLV should be exactly four bytes (0xFFFFFFFF) */
      /* Upper 16 bits corresponds to object information and lower 16 bits corresponds to instance information */      
      *valueP = dataP->value.asObjLink;
      break;
    }    
    
    default:
      return 0;
  }

  LOG_DEBUG("result: %d, value: %" PRId64, result, *valueP);
  return 1;
}



/**
 * @fn void lwm2m_data_encode_float()
 * @brief This function is used to write float type data into
 *        lwm2m_data_t structure
 *
 * @param value     float data to be written
 * @param dataP     data lwm2m_data_t strucuture pointer 
 *
 * @return    void
 */
void lwm2m_data_encode_float(double value,
    lwm2m_data_t * dataP)
{
  LOG_DEBUG("value: %f", value);
  if( dataP == NULL ) 
  { 
    LOG_DEBUG("Passed  Argument is NULL");
    return ;
  }

  dataP->type = LWM2M_TYPE_FLOAT;
  dataP->value.asFloat = value;
}

/**
 * @fn int lwm2m_data_decode_float()
 * @brief This function is used to read float type of data from 
 *        lwm2m_data_t structure
 *
 * @param dataP     data lwm2m_data_t strucuture pointer 
 * @param valueP    pointer to hold the value
 *
 * @return    1 on success
 *            0 on failure
 */
int lwm2m_data_decode_float(const lwm2m_data_t * dataP,
    double * valueP)
{
  int result;
  if( dataP == NULL || valueP == NULL ) 
  {
    LOG_DEBUG(" Passed  Argument is NULL");
    return 0; 
  }


  LOG_DEBUG("Entering");
  switch (dataP->type)
  {
    case LWM2M_TYPE_FLOAT:
      *valueP = dataP->value.asFloat;
      result = 1;
      break;

    case LWM2M_TYPE_INTEGER:
      *valueP = (double)dataP->value.asInteger;
      result = 1;
      break;

    case LWM2M_TYPE_STRING:
      result = utils_plainTextToFloat64(dataP->value.asBuffer.buffer, dataP->value.asBuffer.length, valueP);
      break;

    case LWM2M_TYPE_OPAQUE:
      result = utils_opaqueToFloat(dataP->value.asBuffer.buffer, dataP->value.asBuffer.length, valueP);
      if (result == (int)dataP->value.asBuffer.length)
      {
        result = 1;
      }
      else
      {
        result = 0;
      }
      break;

    default:
      return 0;
  }

  LOG_DEBUG("result: %d, value: %f", result, *valueP);

  return result;
}

/**
 * @fn void lwm2m_data_encode_bool()
 * @brief This function is used to write bool type data into
 *        lwm2m_data_t structure
 *
 * @param value     bool data to be written
 * @param dataP     data lwm2m_data_t strucuture pointer 
 *
 * @return    void
 */
void lwm2m_data_encode_bool(bool value,
    lwm2m_data_t * dataP)
{
  LOG_DEBUG("value: %s", value?"true":"false");
  if( dataP == NULL ) 
  { 
    LOG_DEBUG(" Passed  Argument is NULL");
    return ;
  }

  dataP->type = LWM2M_TYPE_BOOLEAN;
  dataP->value.asBoolean = value;
}

/**
 * @fn int lwm2m_data_decode_bool()
 * @brief This function is used to read bool type of data from 
 *        lwm2m_data_t structure
 *
 * @param dataP     data lwm2m_data_t strucuture pointer 
 * @param valueP    pointer to hold the value
 *
 * @return    1 on success
 *            0 on failure
 */
int lwm2m_data_decode_bool(const lwm2m_data_t * dataP,
    bool * valueP)
{
  int result; 

  if (dataP == NULL || valueP == NULL) 
  {
    LOG_DEBUG(" Passed  Argument is NULL");
    return 0;
  }

  LOG_DEBUG("Entering");

  switch (dataP->type)
  {
    case LWM2M_TYPE_BOOLEAN:
    {
     *valueP = dataP->value.asBoolean;
      result = 1;
      break;
    }

    case LWM2M_TYPE_STRING:
    case LWM2M_TYPE_OPAQUE:
    {
      if (dataP->value.asBuffer.length == 1)
      {
        switch (dataP->value.asBuffer.buffer[0])
        {
          case '0':
          case 0 :
           *valueP = false;
            result = 1;
            break;
          case '1':
          case 1 :
           *valueP = true;
            result = 1;
            break;
          default:
            result = 0;
            break;
        }
      } 
      else if (!strncasecmp("true",(const char*)dataP->value.asBuffer.buffer, dataP->value.asBuffer.length))
      {  
        *valueP = true;
         result = 1;
      }
      else if (!strncasecmp("false",(const char*)dataP->value.asBuffer.buffer, dataP->value.asBuffer.length))
      { 
        *valueP = false;
         result = 1;
      }
      else
      {
         return 0;
      }
     break;
    }
    default:
      result = 0;
      break;
  }

  LOG_DEBUG("result: %d, value: %s", result, *valueP ? "true" : "false");

  return result;
}


/**
 * @fn void lwm2m_data_encode_objLink()
 * @brief This function is used to write object link type data into 
 *        lwm2m_data_t structure
 *
 * @param value     object link data to be written
 * @param dataP     data lwm2m_data_t strucuture pointer 
 *
 * @return    void
 */
void lwm2m_data_encode_objLink(objlink_t value,
    lwm2m_data_t * dataP)
{
  LOG_DEBUG("value: %" OBJLINK "", value);
  if( dataP == NULL ) 
  { 
    LOG_DEBUG(" Passed  Argument is NULL");
    return ;
  }

  dataP->type = LWM2M_TYPE_OBJECT_LINK;
  dataP->value.asObjLink = value;
}

/**
 * @fn void lwm2m_data_include()
 * @brief This function is used to add sub data to the data of 
 *        type lwm2m_data_t structure
 *
 * @param subDataP    lwm2m_data_t strucuture pointer
 * @param count       count of sub data
 * @param dataP       data lwm2m_data_t strucuture pointer
 *
 * @return    void
 */
void lwm2m_data_include(lwm2m_data_t * subDataP,
    size_t count,
    lwm2m_data_t * dataP)
{

  if ( dataP  == NULL ) return;

  if(subDataP == NULL || count == 0)
  {
    dataP->type = LWM2M_TYPE_OBJECT_INSTANCE;
    dataP->value.asChildren.count = 0;
    dataP->value.asChildren.array = NULL;
    return;
  }

  LOG_DEBUG("count: %d", count);

  switch (subDataP[0].type)
  {
    case LWM2M_TYPE_STRING:
    case LWM2M_TYPE_OPAQUE:
    case LWM2M_TYPE_INTEGER:
    case LWM2M_TYPE_FLOAT:
    case LWM2M_TYPE_BOOLEAN:
    case LWM2M_TYPE_OBJECT_LINK:
    case LWM2M_TYPE_MULTIPLE_RESOURCE:
      dataP->type = LWM2M_TYPE_OBJECT_INSTANCE;
      break;
    case LWM2M_TYPE_OBJECT_INSTANCE:
      dataP->type = LWM2M_TYPE_OBJECT;
      break;
    default:
      return;
  }
  dataP->value.asChildren.count = count;
  dataP->value.asChildren.array = subDataP;
}

/**
 * @fn void lwm2m_data_encode_instances()
 * @brief This function is used to write lwm2m_data_t structure type of data
 *         into lwm2m_data_t structure
 *
 * @param subDataP    lwm2m_data_t strucuture pointer
 * @param count       count of sub data
 * @param dataP       data lwm2m_data_t strucuture pointer
 *
 * @return    void
 */
void lwm2m_data_encode_instances(lwm2m_data_t * subDataP,
    size_t count,
    lwm2m_data_t * dataP)
{

  if(dataP == NULL) 
  { 
    LOG_DEBUG(" Passed  Argument is NULL");
    return ;
  }
  LOG_DEBUG("count: %d", count);    
  lwm2m_data_include(subDataP, count, dataP);
  dataP->type = LWM2M_TYPE_MULTIPLE_RESOURCE;
}

/**
 * @fn void int lwm2m_data_parse()
 * @brief This function is used to write lwm2m_data_t structure type of
 *        data into lwm2m_data_t structure
 *
 * @param uriP handle to the resource uri
 * @param buffer handle to chanracter buffer
 * @param format media type of the data
 * @param dataP handle to lwm2m data type
 *
 * @return    0 on error
 *            parsed count on success
 */
int lwm2m_data_parse(lwm2m_uri_t * uriP,
    uint8_t * buffer,
    size_t bufferLen,
    lwm2m_media_type_t format,
    lwm2m_data_t ** dataP)
{
  int result = 0;
  if ( uriP == NULL || dataP == NULL ) 
  { 
    LOG_DEBUG(" Passed  Argument is NULL");
    return 0 ; 
  }

  LOG_DEBUG("format: %s, bufferLen: %d", STR_MEDIA_TYPE(format), bufferLen);
  LOG_URI(uriP);
  do
  {
    switch (format)
    {
      case LWM2M_CONTENT_TEXT:
      case LWM2M_CONTENT_OPAQUE:
        if(!LWM2M_URI_IS_SET_RESOURCE(uriP)) 
        {
          result = 0;
          break;
        }
        *dataP = lwm2m_data_new(1);
        if (*dataP == NULL)
        {
          result = 0;
          break;
        }
        (*dataP)->id = uriP->resourceId;
        if(format == LWM2M_CONTENT_TEXT)
        {
          (*dataP)->type = LWM2M_TYPE_STRING;
          lwm2m_data_encode_nstring((const char *)buffer, bufferLen, *dataP);
        }
        else
        {
          (*dataP)->type = LWM2M_TYPE_OPAQUE;
          lwm2m_data_encode_opaque(buffer, bufferLen, *dataP);
        }
        if(*dataP)
        {
          if((*dataP)->type == LWM2M_TYPE_UNDEFINED)
          {
            result = 0;
            break;
          }
          else
          {
            return 1;
          }
        }
        else
        {
          result = 0;
          break;
        }
      case LWM2M_CONTENT_TLV:
        if(buffer == NULL)
        {
          result = 0;
          break;
        }
        else{
          result = tlv_parse(buffer, bufferLen, dataP);
          break;
        }
  
  #ifdef LWM2M_SUPPORT_JSON
      case LWM2M_CONTENT_JSON:
        if(buffer == NULL)
       {
         result = 0;
         break;
       }
       else{
         result = json_parse(uriP, buffer, bufferLen, dataP);
         break;
       }
  #endif
      default:
        result = 0;
    }
  }while(0);
  
  if(result == 0)
    ds_iot_ulog_high1("lwm2m_parse_data failed, format[%d]",format);
  return result;
}
/**
 * @fn size_t lwm2m_data_serialize()
 * @brief This function is used to copy the value from lwm2m_data_t 
 *        strucuture into buffer
 *
 * @param uriP       lwm2m_uri_t pointer
 * @param size       size
 * @param dataP      data lwm2m_data_t strucuture pointer
 * @param formatP    format of the data
 * @param bufferP    buffer, double pointer to the data copied
 *
 * @return    number of bytes copied on success
 *            0 on failure
 */
size_t lwm2m_data_serialize(lwm2m_uri_t * uriP,
    int size,
    lwm2m_data_t * dataP,
    lwm2m_media_type_t * formatP,
    uint8_t ** bufferP)
{

  if ( uriP == NULL || dataP == NULL || formatP == NULL || bufferP == NULL ) 
  {
    LOG_DEBUG(" Passed  Argument is NULL");
    return 0;
  }

  LOG_URI(uriP);
  LOG_DEBUG("size: %d, formatP: %s", size, STR_MEDIA_TYPE(*formatP));

  // Check format
  if (*formatP == LWM2M_CONTENT_TEXT
      || *formatP == LWM2M_CONTENT_OPAQUE)
  {
    if (size != 1
        || !LWM2M_URI_IS_SET_RESOURCE(uriP)
        || dataP->type == LWM2M_TYPE_OBJECT
        || dataP->type == LWM2M_TYPE_OBJECT_INSTANCE
        || dataP->type == LWM2M_TYPE_MULTIPLE_RESOURCE)
    {
#ifdef LWM2M_SUPPORT_JSON
      *formatP = LWM2M_CONTENT_JSON;
#else
      *formatP = LWM2M_CONTENT_TLV;
#endif
    }
  }

  if ( (*formatP == LWM2M_CONTENT_OPAQUE) && (dataP->type != LWM2M_TYPE_OPAQUE))
  {
    ds_iot_ulog_high("Opaque format reserved for opaque resources.");
    return 0;
  }
  LOG_DEBUG("Final format: %s", STR_MEDIA_TYPE(*formatP));

  switch (*formatP)
  {
    case LWM2M_CONTENT_TEXT:
      return prv_textSerialize(dataP, bufferP);

    case LWM2M_CONTENT_OPAQUE:
    {
      if((dataP->type == LWM2M_TYPE_INTEGER) ||(dataP->type == LWM2M_TYPE_FLOAT) || (dataP->type == LWM2M_TYPE_BOOLEAN) )
      {
        return 0;
      }
      else
      {
        *bufferP = (uint8_t *)lwm2m_malloc(dataP->value.asBuffer.length);
        if (*bufferP == NULL) return 0;
          LWM2M_MEMCPY(*bufferP, dataP->value.asBuffer.length, dataP->value.asBuffer.buffer, dataP->value.asBuffer.length);
      }
    }
      return dataP->value.asBuffer.length;

    case LWM2M_CONTENT_TLV:
      {
        uint8_t baseUriStr[URI_MAX_STRING_LEN];
        int uriLen = 0;
        bool isResourceInstance;

        uriLen = uri_toString(uriP, baseUriStr, URI_MAX_STRING_LEN, NULL);
        if (uriLen <= 0) return 0;

        if (LWM2M_URI_IS_SET_RESOURCE_INST(uriP))
        {
          isResourceInstance = true;
        }
        else
        {
          isResourceInstance = false;
        }
        return tlv_serialize(isResourceInstance, size, dataP, bufferP);
      }

#ifdef LWM2M_CLIENT_MODE
    case LWM2M_CONTENT_LINK:
      return discover_serialize(data.lwm2mH, uriP, size, dataP, bufferP);
#endif
#ifdef LWM2M_SUPPORT_JSON
    case LWM2M_CONTENT_JSON:
      return json_serialize(uriP, size, dataP, bufferP);
#endif

    default:
      return 0;
  }
}

