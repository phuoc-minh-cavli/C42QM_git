/******************************************************************************

  @file    utils.c

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
 *    Toby Jaffey - Please refer to git log
 *    
 *******************************************************************************/

/*
   Copyright (c) 2013, 2014 Intel Corporation

   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 * Neither the name of Intel Corporation nor the names of its contributors
 may be used to endorse or promote products derived from this software
 without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 THE POSSIBILITY OF SUCH DAMAGE.

 David Navarro <david.navarro@intel.com>

*/

#include "internals.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <float.h>
#include "ds_log.h"


/**
 * @fn utils_plainTextToInt64()
 * @brief This function is used to convert plain text to int64 format 
 *
 * @param buffer Input plain text
 * @param length length of the buffer
 * @param dataP  output param after conversion
 *
 * @return 0 on failure
 *         1 on success
 */

int utils_plainTextToInt64(uint8_t * buffer,
    int length,
    int64_t * dataP)
{
  uint64_t result = 0;
  int sign = 1;
  int i = 0;

  if (0 == length || dataP == NULL || buffer == NULL ) return 0;

  if (buffer[0] == '-')
  {
    sign = -1;
    i = 1;
  }

  while (i < length)
  {
    if ('0' <= buffer[i] && buffer[i] <= '9')
    {
      if (result > (UINT64_MAX / 10)) return 0;
      result *= 10;
      result += buffer[i] - '0';
    }
    else
    {
      return 0;
    }
    i++;
  }

  if (result > INT64_MAX) return 0;

  if (sign == -1)
  {
    *dataP = 0 - result;
  }
  else
  {
    *dataP = result;
  }

  return 1;
}

/**
 * @fn utils_plainTextToFloat64()
 * @brief This function is used to convert plain text to float 64 format
 *
 * @param buffer Input plain text
 * @param length length of the buffer
 * @param dataP  output param after conversion 
 * @return 0 on failure
 *         1 on success
 */

int utils_plainTextToFloat64(uint8_t * buffer,
    int length,
    double * dataP)
{
  double result;
  int sign;
  int i;

  if (0 == length || buffer == NULL || dataP == NULL) return 0;

  if (buffer[0] == '-')
  {
    sign = -1;
    i = 1;
  }
  else
  {
    sign = 1;
    i = 0;
  }

  result = 0;
  while (i < length && buffer[i] != '.')
  {
    if ('0' <= buffer[i] && buffer[i] <= '9')
    {
      if (result > (DBL_MAX / 10)) return 0;
      result *= 10;
      result += (buffer[i] - '0');
    }
    else
    {
      return 0;
    }
    i++;
  }
  if (buffer[i] == '.')
  {
    double dec;

    i++;
    if (i == length) return 0;

    dec = 0.1;
    while (i < length)
    {
      if ('0' <= buffer[i] && buffer[i] <= '9')
      {
        if (result > (DBL_MAX - 1)) return 0;
        result += (buffer[i] - '0') * dec;
        dec /= 10;
      }
      else
      {
        return 0;
      }
      i++;
    }
  }

  *dataP = result * sign;
  return 1;
}

/**
 * @fn utils_intToText()
 * @brief This function is used to convert integer into plain text
 *
 * @param data Input integer value
 * @param string output param after conversion
 * @param length length of the string
 * @return 0 on failure
 *         1 on success
 */
size_t utils_intToText(int64_t data,
    uint8_t * string,
    size_t length)
{
  int index = 0;
  bool minus = LWM2M_FALSE;
  size_t result = 0;   
  if( string == NULL )  
  {
    ds_iot_ulog_err("LWM2M_LOG: NULL Error");
    return 0;
  }

  if (data < 0)
  {
    minus = true;
    data = 0 - data;
  }
  else
  {
    minus = false;
  }

  index = length - 1;
  do
  {
    string[index] = '0' + data%10;
    data /= 10;
    index --;
  } while (index >= 0 && data > 0);

  if (data > 0) return 0;

  if (minus == true)
  {
    if (index == 0) return 0;
    string[index] = '-';
  }
  else
  {
    index++;
  }

  result = length - index;

  if (result < length)
  {
    LWM2M_MEMMOVE(string, length, string + index, result);
  }

  return result;
}

/**
 * @fn utils_floatToText()
 * @brief This function is used to convert float into plain text
 *
 * @param data Input float value
 * @param string output param after conversion
 * @param length length of the string
 * @return 0 on failure
 *         1 on success
 */
size_t utils_floatToText(double data,
    uint8_t * string,
    size_t length)
{
  size_t intLength = 0;
  size_t decLength = 0;
  int64_t intPart = 0;
  double decPart = 0;

  if (string == NULL)
  {
    ds_iot_ulog_err("LWM2M_LOG: NULL Error");
    return 0;
  }

  if (data <= (double)INT64_MIN || data >= (double)INT64_MAX) return 0;

  intPart = (int64_t)data;
  decPart = data - intPart;
  if (decPart < 0)
  {
    decPart = 1 - decPart;
  }
  else
  {
    decPart = 1 + decPart;
  }

  if (decPart <= 1 + FLT_EPSILON)
  {
    decPart = 0;
  }

  if (intPart == 0 && data < 0)
  {
    // deal with numbers between -1 and 0
    if (length < 4) return 0;   // "-0.n"
    string[0] = '-';
    string[1] = '0';
    intLength = 2;
  }
  else
  {
    intLength = utils_intToText(intPart, string, length);
    if (intLength == 0) return 0;
  }
  decLength = 0;
  if (decPart >= FLT_EPSILON)
  {
    int i;
    double noiseFloor;

    if (intLength >= length - 1) return 0;

    i = 0;
    noiseFloor = FLT_EPSILON;
    do
    {
      decPart *= 10;
      noiseFloor *= 10;
      i++;
    } while (decPart - (int64_t)decPart > noiseFloor);

    decLength = utils_intToText(decPart, string + intLength, length - intLength);
    if (decLength <= 1) return 0;

    // replace the leading 1 with a dot
    string[intLength] = '.';
  }

  return intLength + decLength;
}

/**
 * @fn utils_int64ToPlainText()
 * @brief This function is used to convert uint64 value into plain text
 *
 * @param data Input float value
 * @param buffer output param after conversion
 * @return 0 on failure
 *         length of buffer after conversion on success
 */
size_t utils_int64ToPlainText(int64_t data,
    uint8_t ** bufferP)
{
#define _PRV_STR_LENGTH 32
  uint8_t string[_PRV_STR_LENGTH];
  size_t length = 0;
  if( bufferP == NULL ) 
  { 
    ds_iot_ulog_err(" LWM2M_LOG: NULL Error");
    return 0;
  }
  length = utils_intToText(data, string, _PRV_STR_LENGTH);
  if (length == 0) return 0;

  *bufferP = (uint8_t *)lwm2m_malloc(length);
  if (NULL == *bufferP) return 0;

  LWM2M_MEMCPY(*bufferP, length, string, length);

  return length;
}

/**
 * @fn utils_float64ToPlainText()
 * @brief This function is used to convert Float 64 value into plain text
 *
 * @param data Input float value
 * @param buffer output param after conversion
 * @return 0 on failure
 *         length of buffer after conversion on success
 */
size_t utils_float64ToPlainText(double data,
    uint8_t ** bufferP)
{
  uint8_t string[_PRV_STR_LENGTH * 2];
  size_t length = 0;
  if( bufferP== NULL ) 
  {
    ds_iot_ulog_err("LWM2M_LOG: NULL Error");
    return 0;
  }
  length = utils_floatToText(data, string, _PRV_STR_LENGTH * 2);
  if (length == 0) return 0;

  *bufferP = (uint8_t *)lwm2m_malloc(length);
  if (NULL == *bufferP) return 0;

  LWM2M_MEMCPY(*bufferP, length, string, length);

  return length;
}

/**
 * @fn utils_boolToPlainText()
 * @brief This function is used to convert bool value into plain text
 *
 * @param data Input bool value
 * @param buffer output param after conversion
 * @return 0 on failure
 *         length of buffer after conversion on success
 */
size_t utils_boolToPlainText(bool data,
    uint8_t ** bufferP)
{
  if( bufferP == NULL ) 
  { 
    ds_iot_ulog_err("LWM2M_LOG: NULL Error");
    return 0;
  }
  return utils_int64ToPlainText((int64_t)(data?1:0), bufferP);
}

/**
 * @fn utils_stringToBinding()
 * @brief This function is used to convert Binding support input to 
 *    Binding support string
 * @param data Input bool value
 * @param buffer output param after conversion
 * @return BINDING_UNKNOWN on failure
 *         LWM2M Binding mode on success
 */
lwm2m_binding_t utils_stringToBinding(uint8_t * binding_buf, size_t length)
{
  uint32_t cntr = 0x00;
  lwm2m_binding_t binding_mode = BINDING_UNKNOWN;

  if (length == 0 || binding_buf == NULL)
  {
    ds_iot_ulog_err1("Invalid arguments - binding_buf, len[0x%x]", length);
    return binding_mode;
  }

  /* Validate the bindings resource content from the LwM2Mv1.1 server */ 
  while (cntr < length)
  {
    switch(binding_buf[cntr])
    {
      case 'U':
        binding_mode |= BINDING_U;
        break;

      case 'Q':
        binding_mode |= BINDING_Q;
        break; 
             
      case 'S':
        binding_mode |= BINDING_S;
        break;

      case 'N': 
        binding_mode |= BINDING_N;
        break;

      case 'T':
        binding_mode |= BINDING_T;
        break;

      default:
        return BINDING_UNKNOWN;
    }

    cntr += 1;
  }          

  return binding_mode;
}

/**
 * @fn utils_convertMediaType()
 * @brief This function is used to convert content type to lwm2m content type
 * @param type coap content type
 * @return  LWM2M content type format
 */
lwm2m_media_type_t utils_convertMediaType(qapi_Coap_Content_Type_t type)
{
  // Here we just check the content type is a valid value for LWM2M
  ds_iot_ulog_high1("Content type %d", type);
  switch((uint16_t)type)
  {
    case QAPI_TEXT_PLAIN:
      return LWM2M_CONTENT_TEXT;
    case QAPI_APPLICATION_OCTET_STREAM:
      return LWM2M_CONTENT_OPAQUE;
    case QAPI_M2M_TLV:
      return LWM2M_CONTENT_TLV;
/*JSON should be treated as unknown format */
#if 0
    case QAPI_M2M_JSON:
      return LWM2M_CONTENT_JSON;
#endif
    case QAPI_APPLICATION_LINK_FORMAT:
      return LWM2M_CONTENT_LINK;

    default:
      return LWM2M_CONTENT_UNSUPPORTED;
  }
}

#ifdef LWM2M_CLIENT_MODE
/**
 * @fn utils_findServer()
 * @brief This function is used to find the server from lwm2m context
 * @param contextP lwm2m context
 * @param fromSessionH Server session 
 * @return  NULL on failure
 *          server context on success
 */
lwm2m_server_t * utils_findServer(lwm2m_context_t * contextP, void * fromSessionH)
{

  lwm2m_server_t * targetP = NULL;

  if ( contextP == NULL || fromSessionH == NULL ) 
  {
    ds_iot_ulog_err("LWM2M_LOG: NULL Error");
    return NULL;
  }

  targetP = contextP->serverList;
  while (targetP != NULL
      && (false == lwm2m_coap_sessionhandle_is_equal(targetP->coapHandle, fromSessionH, contextP->userData)) 
       )
  {
    targetP = targetP->next;
    
  }
    


  return targetP;
}
#endif

/**
 * @fn utils_findBootstrapServer()
 * @brief This function is used to find the bootstarp server from lwm2m context
 * @param contextP lwm2m context
 * @param fromSessionH bootstrap server session  
 * @return  NULL on failure
 *          server context on success
 */

lwm2m_server_t * utils_findBootstrapServer(lwm2m_context_t * contextP,
    void * fromSessionH)
{
#ifdef LWM2M_CLIENT_MODE

  lwm2m_server_t * targetP = NULL;

  if( contextP == NULL || fromSessionH == NULL ) 
  { 
    ds_iot_ulog_err("LWM2M_LOG: NULL Error");
    return NULL;
  }

  targetP = contextP->bootstrapServer;
  if(targetP != NULL
     && false == lwm2m_coap_sessionhandle_is_equal(targetP->coapHandle, fromSessionH, contextP->userData))
  {
    return NULL;
  }

  return targetP;

#else

  return NULL;

#endif
 
}

/**
 * @fn utils_isAltPathValid()
 * @brief This function is used to validate alternate path received
 *     in CoAP packet
 * @param altPath alt path string
 * @return  0 on failure
 *          1 on success
 */

int utils_isAltPathValid(const char * altPath)
{
  int i;

  if (altPath == NULL) return 0;

  if (altPath[0] != '/') return 0;

  for (i = 1 ; altPath[i] != 0 ; i++)
  {
    // TODO: Support multi-segment alternative path
    if (altPath[i] == '/') return 0;
    // TODO: Check needs for sub-delims, ':' and '@'
    if ((altPath[i] < 'A' || altPath[i] > 'Z')      // ALPHA
        && (altPath[i] < 'a' || altPath[i] > 'z')
        && (altPath[i] < '0' || altPath[i] > '9')      // DIGIT
        && (altPath[i] != '-')                         // Other unreserved
        && (altPath[i] != '.')
        && (altPath[i] != '_')
        && (altPath[i] != '~')
        && (altPath[i] != '%'))                        // pct_encoded
    {
      return 0;
    }

  }
  return 1;
}

/**
 * @fn utils_stringCopy()
 * @brief This function is used to copy a string in a buffer
 * @param buffer buffer into which string needs to be copied
 * @param length length of the buffer
 * @param str string which needs to be copied
 * @return  return the number of copied bytes or -1 if the buffer is not 
 *          large enough
 */
int utils_stringCopy(char * buffer,
    size_t length,
    const char * str)
{
  size_t i = 0;
  if ( length == 0 || buffer == NULL || str == NULL) 
  {
    ds_iot_ulog_err("LWM2M_LOG: NULL Error");
    return -1;
  }
  for (i = 0 ; i < length && str[i] != 0 ; i++)
  {
    buffer[i] = str[i];
  }

  if (i == length) return -1;

  buffer[i] = 0;

  return (int)i;
}

/**
 * @fn utils_intCopy()
 * @brief This function is used to integer into a buffer
 * @param buffer buffer into which string needs to be copied
 * @param length length of the buffer
 * @param value integer value which needs to be copied
 * @return  return the number of copied bytes or -1 if the buffer is not 
 *          large enough
 */
int utils_intCopy(char * buffer,
    size_t length,
    int32_t value)
{
#define _PRV_INT32_MAX_STR_LEN 11
  uint8_t str[_PRV_INT32_MAX_STR_LEN];
  size_t len = 0;
  if ( buffer == NULL ) 
  { 
    ds_iot_ulog_err("LWM2M_LOG: NULL Error");
    return -1;
  } 
  len = utils_intToText(value, str, _PRV_INT32_MAX_STR_LEN);
  if (len == 0) return -1;
  if (len > length + 1) return -1;

  LWM2M_MEMCPY(buffer, length, str, len);
  buffer[len] = 0;

  return len;
}

/**
 * @fn utils_copyValue() 
 * @brief This function is used to copy value input string into output string
 * @param dst destination string
 * @param src source string
 * @param len length to be copied
 * @return  void
 */
void utils_copyValue(void * dst,
    const void * src,
    size_t len)
{ 
#ifdef LWM2M_LITTLE_ENDIAN
  size_t i = 0;
#endif  
  if (dst == NULL || src == NULL)
  {
    ds_iot_ulog_err("LWM2M_LOG: NULL Error");
    return;
  }
#ifdef LWM2M_BIG_ENDIAN
  LWM2M_MEMCPY(dst, src, len);
#else
#ifdef LWM2M_LITTLE_ENDIAN
  for (i = 0; i < len; i++)
  {
    ((uint8_t *)dst)[i] = ((uint8_t *)src)[len - 1 - i];
  }
#endif
#endif
}

/**
 * @fn utils_opaqueToInt()
 * @brief This function is used to convert opaque into integer
 * @param buffer input opaque buffer
 * @param buffer_len length of the buffer
 * @param dataP output integer param
 * @return 0 on failure
 *         buffer_len on success
 */
int utils_opaqueToInt(const uint8_t * buffer,
    size_t buffer_len,
    int64_t * dataP)
{
  if( buffer == NULL || dataP == NULL ) 
  {
    ds_iot_ulog_err("LWM2M_LOG: NULL Error");
    return 0;
  } 

  *dataP = 0;

  switch (buffer_len)
  {
    case 1:
      {
        *dataP = (int8_t)buffer[0];

        break;
      }

    case 2:
      {
        int16_t value;

        utils_copyValue(&value, buffer, buffer_len);

        *dataP = value;
        break;
      }

    case 4:
      {
        int32_t value;

        utils_copyValue(&value, buffer, buffer_len);

        *dataP = value;
        break;
      }

    case 8:
#if 0 // KW issue
      utils_copyValue(dataP, buffer, buffer_len);
      return buffer_len;
#else
      if(buffer_len <= sizeof(int64_t) )
      {
        utils_copyValue(dataP, buffer, buffer_len);
        return buffer_len;
      }
      else
      {
        return 0;
      }
#endif
    default:
      return 0;
  }

  return buffer_len;
}

/**
 * @fn utils_opaqueToFloat()
 * @brief This function is used to convert opaque to Float
 * @param buffer input opaque buffer
 * @param buffer_len buffer length
 * @param dataP output float param
 * @return 0 on failure
 *         buffer_len on success
 */
int utils_opaqueToFloat(const uint8_t * buffer,
    size_t buffer_len,
    double * dataP)
{ 
  if( buffer == NULL || dataP == NULL ) 
  { 
    ds_iot_ulog_err("LWM2M_LOG: NULL Error");
    return 0;
  }
  switch (buffer_len)
  {
    case 4:
      {
        float temp;

        utils_copyValue(&temp, buffer, buffer_len);

        *dataP = temp;
      }
      return 4;

    case 8:
      utils_copyValue(dataP, buffer, buffer_len);
      return 8;

    default:
      return 0;
  }
}

/**
 * Encode an integer value to a byte representation.
 * Returns the length of the result. For values < 0xff length is 1,
 * for values < 0xffff length is 2 and so on.
 * @param data        Input value
 * @param data_buffer Result in data_buffer is in big endian encoding
 *                    Negative values are represented in two's complement as of
 *                    OMA-TS-LightweightM2M-V1_0-20160308-D, Appendix C
 */
size_t utils_encodeInt(int64_t data,
    uint8_t data_buffer[_PRV_64BIT_BUFFER_SIZE])
{
  size_t length = 0;

  memset(data_buffer, 0, _PRV_64BIT_BUFFER_SIZE);

  if (data >= INT8_MIN && data <= INT8_MAX)
  {
    length = 1;
    data_buffer[0] = data;
  }
  else if (data >= INT16_MIN && data <= INT16_MAX)
  {
    int16_t value;

    value = data;
    length = 2;
    data_buffer[0] = (value >> 8) & 0xFF;
    data_buffer[1] = value & 0xFF;
  }
  else if (data >= INT32_MIN && data <= INT32_MAX)
  {
    int32_t value;

    value = data;
    length = 4;
    utils_copyValue(data_buffer, &value, length);
  }
  else if (data >= INT64_MIN && data <= INT64_MAX)
  {
    length = 8;
    utils_copyValue(data_buffer, &data, length);
  }

  return length;
}


/**
 * Encode an objectlink value to a byte representation.
 * Returns the length of the result. For values < 0xff length is 1,
 * for values < 0xffff length is 2 and so on.
 * @param data        Input value
 * @param data_buffer Result in data_buffer is in big endian encoding
 *                    Negative values are represented in two's complement as of
 *                    OMA-TS-LightweightM2M-V1_0-20160308-D, Appendix C
 */
size_t utils_encodeObjectLink(int64_t data,
    uint8_t data_buffer[_PRV_64BIT_BUFFER_SIZE])
{
  size_t length = 0;
  uint32_t value = data;

  memset(data_buffer, 0, _PRV_64BIT_BUFFER_SIZE);
 
  length = 4;
  utils_copyValue(data_buffer, &value, length);
 
  return length;
}


/**
 * Encode a float value to a byte representation.
 * @param data        Input value
 * @param data_buffer Result in data_buffer is in big endian encoding
 *                    Negative values are represented in two's complement as of
 *                    OMA-TS-LightweightM2M-V1_0-20160308-D, Appendix C
 */
size_t utils_encodeFloat(double data,
    uint8_t data_buffer[_PRV_64BIT_BUFFER_SIZE])
{
  size_t length = 0;

  memset(data_buffer, 0, _PRV_64BIT_BUFFER_SIZE);

  if ((data < 0.0 - (double)FLT_MAX) || (data >(double)FLT_MAX))
  {
    length = 8;
    utils_copyValue(data_buffer, &data, 8);
  }
  else
  {
    float value;

    length = 4;
    value = (float)data;
    utils_copyValue(data_buffer, &value, 4);
  }

  return length;
}

#define PRV_B64_PADDING '='

static char b64Alphabet[64] =
{
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
  'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
  'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
  'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
};

/**
 * @fn static void prv_encodeBlock()
 * @brief This function is used to encode block value 
 * @param input input block
 * @param output  output block
 * @return void
 */
static void prv_encodeBlock(uint8_t input[3],
    uint8_t output[4])
{
  output[0] = b64Alphabet[input[0] >> 2];
  output[1] = b64Alphabet[((input[0] & 0x03) << 4) | (input[1] >> 4)];
  output[2] = b64Alphabet[((input[1] & 0x0F) << 2) | (input[2] >> 6)];
  output[3] = b64Alphabet[input[2] & 0x3F];
}


#ifdef TX_DATA_UNUSED_CODE 
/**
 * @fn static uint8_t prv_b64Revert()
 * @brief This function is used to revert given value to base64
 * @param value input value
 * @return base64 value
 */
static uint8_t prv_b64Revert(uint8_t value)
{
  if (value >= 'A' && value <= 'Z')
  {
    return (value - 'A');
  }
  if (value >= 'a' && value <= 'z')
  {
    return (26 + value - 'a');
  }
  if (value >= '0' && value <= '9')
  {
    return (52 + value - '0');
  }
  switch (value)
  {
    case '+':
      return 62;
    case '/':
      return 63;
    default:
      return 0;
  }
}

/**
 * @fn static void prv_decodeBlock()
 * @brief This function is used to decode block value 
 * @param input input block
 * @param output  output block
 * @return void
 */
static void prv_decodeBlock(uint8_t input[4],
    uint8_t output[3])
{
  uint8_t tmp[4];
  int i;

  memset(output, 0, 3);

  for (i = 0; i < 4; i++)
  {
    tmp[i] = prv_b64Revert(input[i]);
  }

  output[0] = (tmp[0] << 2) | (tmp[1] >> 4);
  output[1] = (tmp[1] << 4) | (tmp[2] >> 2);
  output[2] = (tmp[2] << 6) | tmp[3];
}

#endif

/**
 * @fn static size_t prv_getBase64Size()
 * @brief This function is used to get base 64 size for given data length
 * @param dataLen  length of the data
 * @return base64 size
 */
size_t prv_getBase64Size(size_t dataLen)
{
  size_t result_len;

  result_len = 4 * (dataLen / 3);
  if (dataLen % 3) result_len += 4;

  return result_len;
}

/**
 * @fn size_t utils_base64Encode()
 * @brief This function is used to decode given value
 * @param dataP  value to be encoded
 * @param dataLen  length of the dataP
 * @param bufferP  buffer to hold result value
 * @param bufferLen  length of the buffer
 * @return 0 on failure,
 *         result_len on success
 */
size_t utils_base64Encode(uint8_t * dataP,
    size_t dataLen, 
    uint8_t * bufferP,
    size_t bufferLen)
{
  unsigned int data_index = 0;
  unsigned int result_index = 0;
  size_t result_len = 0;
  if( dataP == NULL || bufferP == NULL ) 
  { 
    ds_iot_ulog_err("LWM2M_LOG: NULL Error");
    return 0;
  }
  result_len = prv_getBase64Size(dataLen);

  if (result_len > bufferLen) return 0;

  data_index = 0;
  result_index = 0;
  while (data_index < dataLen)
  {
    switch (dataLen - data_index)
    {
      case 0:
        // should never happen
        break;
      case 1:
        bufferP[result_index] = b64Alphabet[dataP[data_index] >> 2];
        bufferP[result_index + 1] = b64Alphabet[(dataP[data_index] & 0x03) << 4];
        bufferP[result_index + 2] = PRV_B64_PADDING;
        bufferP[result_index + 3] = PRV_B64_PADDING;
        break;
      case 2:
        bufferP[result_index] = b64Alphabet[dataP[data_index] >> 2];
        bufferP[result_index + 1] = b64Alphabet[(dataP[data_index] & 0x03) << 4 | (dataP[data_index + 1] >> 4)];
        bufferP[result_index + 2] = b64Alphabet[(dataP[data_index + 1] & 0x0F) << 2];
        bufferP[result_index + 3] = PRV_B64_PADDING;
        break;
      default:
        prv_encodeBlock(dataP + data_index, bufferP + result_index);
        break;
    }
    data_index += 3;
    result_index += 4;
  }

  return result_len;
}

#ifdef TX_DATA_UNUSED_CODE


/**
 * @fn size_t utils_opaqueToBase64()
 * @brief This function is used to convert opaque to base64
 * @param dataP  value to be converted
 * @param dataLen  length of the dataP
 * @param bufferP  buffer to hold result value
 * @return 0 on failure,
 *         result_len on success
 */
size_t utils_opaqueToBase64(uint8_t * dataP,
    size_t dataLen,
    uint8_t ** bufferP)
{
  size_t buffer_len = 0;
  size_t result_len = 0;
  if( dataP == NULL || bufferP == NULL ) 
  { 
    ds_iot_ulog_err("LWM2M_LOG: NULL Error");
    return 0;
  }
  buffer_len = prv_getBase64Size(dataLen);

  *bufferP = (uint8_t *)lwm2m_malloc(buffer_len);
  if (!*bufferP) return 0;
  memset(*bufferP, 0, buffer_len);

  result_len = utils_base64Encode(dataP, dataLen, *bufferP, buffer_len);

  if (result_len == 0)
  {
    lwm2m_free(*bufferP);
    *bufferP = NULL;
  }

  return result_len;
}

/**
 * @fn size_t utils_base64ToOpaque()
 * @brief This function is used to convert base64 to opaque
 * @param dataP  value to be converted
 * @param dataLen  length of the dataP
 * @param bufferP  buffer to hold result value
 * @return 0 on failure,
 *         result_len on success
 */
size_t utils_base64ToOpaque(uint8_t * dataP,
    size_t dataLen,
    uint8_t ** bufferP)
{
  size_t data_index = 0;
  size_t result_index = 0;
  size_t result_len = 0;
  if( dataP == NULL || bufferP == NULL ) 
  {
    ds_iot_ulog_err("LWM2M_LOG: NULL Error");
    return 0;  
  }
  if (dataLen % 4) return 0;

  result_len = (dataLen >> 2) * 3;
  *bufferP = (uint8_t *)lwm2m_malloc(result_len);
  if (NULL == *bufferP) return 0;
  memset(*bufferP, 0, result_len);

  // remove padding
  while (dataP[dataLen - 1] == PRV_B64_PADDING)
  {
    dataLen--;
  }

  data_index = 0;
  result_index = 0;
  while (data_index < dataLen)
  {
    prv_decodeBlock(dataP + data_index, *bufferP + result_index);
    data_index += 4;
    result_index += 3;
  }
  switch (data_index - dataLen)
  {
    case 0:
      break;
    case 2:
      {
        uint8_t tmp[2];

        tmp[0] = prv_b64Revert(dataP[dataLen - 2]);
        tmp[1] = prv_b64Revert(dataP[dataLen - 1]);

        *bufferP[result_index - 3] = (tmp[0] << 2) | (tmp[1] >> 4);
        *bufferP[result_index - 2] = (tmp[1] << 4);
        result_len -= 2;
      }
      break;
    case 3:
      {
        uint8_t tmp[3];

        tmp[0] = prv_b64Revert(dataP[dataLen - 3]);
        tmp[1] = prv_b64Revert(dataP[dataLen - 2]);
        tmp[2] = prv_b64Revert(dataP[dataLen - 1]);

        *bufferP[result_index - 3] = (tmp[0] << 2) | (tmp[1] >> 4);
        *bufferP[result_index - 2] = (tmp[1] << 4) | (tmp[2] >> 2);
        *bufferP[result_index - 1] = (tmp[2] << 6);
        result_len -= 1;
      }
      break;
    default:
      // error
      lwm2m_free(*bufferP);
      *bufferP = NULL;
      result_len = 0;
      break;
  }

  return result_len;
}

#endif

/**
 * @fn lwm2m_data_type_t utils_depthToDatatype()
 * @brief This function is used to convert given uri depth to data type
 * @param depth  depth of the uri
 * @return LWM2M response type
 */
lwm2m_data_type_t utils_depthToDatatype(uri_depth_t depth)
{
  switch (depth)
  {
    case URI_DEPTH_OBJECT:
      return LWM2M_TYPE_OBJECT;
    case URI_DEPTH_OBJECT_INSTANCE:
      return LWM2M_TYPE_OBJECT_INSTANCE;
    default:
      break;
  }

  return LWM2M_TYPE_UNDEFINED;
}

/**
 * @fn bool lwm2m_session_is_equal()
 * @brief This function is used to check whether two sessions are equal or not
 * @param session1  handle to first session
 * @param session2  handle to second session
 * @param userData  handle to user data
 * @return true if both sessions are equal,
 *         false if both sessions are not equal
 */
bool lwm2m_coap_sessionhandle_is_equal(void * handle1,
    void * handle2,
    void * userData)
{

  if((handle1 == NULL) || (handle2 == NULL))
    return false;

  return ((uint32_t)handle1 == (uint32_t)handle2);
}

/**
 * @fn void intToString()
 * @brief This function is used to convert the integer to string
 * @param str holds the string convert from integer
 * @param num number to convert to string
 * @return void 
 */
void m2m_intToString(char *str, int num)
{
  int i = 0, rem = 0, len = 0, n = 0;
  n = num;
  while(n != 0)
  {
    len++;
    n /= 10;    
  }
  for(i = 0; i < len; i++)
  {
    rem = num % 10;
    num = num / 10;
    str[len - (i + 1)] = rem + '0';   
  }
  str[len] = '\0';
}

#ifdef TX_DATA_UNUSED_CODE


/**
 * @fn retrieve_lwm2m_ctx_info_from_id()
 * @brief This function is used to get handle fron object ID
 * @param objectId object ID
 * @return handle if found otherwise Null 
 */

lwm2m_handle_info_t * retrieve_lwm2m_ctx_info_from_id(uint16_t objectId)
{
  lwm2m_handle_info_t *lwm2m_hndl = data.glwm2m_app_ctx.handles;
  lwm2m_app_obj_info_t *object_info = NULL;
  
  /* Retrieve the LWM2M context information associated with the handle */
  while (lwm2m_hndl)
  {
    object_info = lwm2m_hndl->obj_info;
    while(object_info)
    {
      if (objectId == object_info->object_ID)
        return lwm2m_hndl;

      object_info = object_info->next;
    }
    
    lwm2m_hndl = lwm2m_hndl->next;
  }
  
  return NULL; 
}

#endif

/**
 * @fn utils_intToText()
 * @brief This function is used to count the number of digit in integer 
 *
 * @param data Input integer value
 *
 * @return number of digit
 */
size_t utils_numDigit(int64_t data)
{
  int count = 0;

  if (data < 0)
  {
    data = 0 - data;
  }
  
  do
  {
    data /= 10;
    count++;
  } while ( data > 0);

  return count;
}

#ifdef TX_DATA_UNUSED_CODE

/**
 * @fn utils_highestPower2()
 * @brief This function is used to find out highest power of a given number 
 *
 * @param data Input integer value
 *
 * @return highest power of 2
 */
int utils_highestPower2(uint32_t number)
{

   int power2 = 0; 
   for (power2= number; power2 >= 1; power2--) 
   { 
      if ((power2 & (power2-1)) == 0) //check for power of 2
      { 
            return power2;
      } 
   } 
   return 0; 
}

#endif





