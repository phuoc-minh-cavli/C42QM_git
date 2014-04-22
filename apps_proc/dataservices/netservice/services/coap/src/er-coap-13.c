/******************************************************************************

  @file    er-coap-13.c

  ---------------------------------------------------------------------------

  Copyright (c) 2016-2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------

 *****************************************************************************/

/*
 * Copyright (c) 2013, Institute for Pervasive Computing, ETH Zurich
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 */

/**
 * \file
 *      An implementation of the Constrained Application Protocol (draft 12)
 * \author
 *      Matthias Kovatsch <kovatsch@inf.ethz.ch>
 * \contributors
 *    David Navarro, Intel Corporation - Adapt to usage in liblwm2m
 */


#include <stdlib.h>
#include <stdio.h>
#include <stringl/stringl.h>
#include "mem_utility.h"
#include "er-coap-13.h"
#include "qapi_coap.h"


#define DEBUG 0
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#define PRINT6ADDR(addr) PRINTF("[%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x]", ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3], ((uint8_t *)addr)[4], ((uint8_t *)addr)[5], ((uint8_t *)addr)[6], ((uint8_t *)addr)[7], ((uint8_t *)addr)[8], ((uint8_t *)addr)[9], ((uint8_t *)addr)[10], ((uint8_t *)addr)[11], ((uint8_t *)addr)[12], ((uint8_t *)addr)[13], ((uint8_t *)addr)[14], ((uint8_t *)addr)[15])
#define PRINTLLADDR(lladdr) PRINTF("[%02x:%02x:%02x:%02x:%02x:%02x]",(lladdr)->addr[0], (lladdr)->addr[1], (lladdr)->addr[2], (lladdr)->addr[3],(lladdr)->addr[4], (lladdr)->addr[5])
#else
#define PRINTF(...)
#define PRINT6ADDR(addr)
#define PRINTLLADDR(addr)
#endif

#define COAP_STRCPY(d,s,n) strlcpy(d,s,n)
#define COAP_MEMCPY(d,dn,s,cn) memscpy(d,dn,s,cn)
#define COAP_MEMMOVE(d,dn,s,cn) memsmove(d,dn,s,cn)

/*-----------------------------------------------------------------------------------*/
/*- Variables -----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------*/

coap_status_t coap_error_code = NO_ERROR;
const char *coap_error_message = "";
/*-----------------------------------------------------------------------------------*/
/*- LOCAL HELP FUNCTIONS ------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------*/
/* @fn     coap_log_2()
 * @brief  This function is used to find the highest bit set in value
 * @param  value - integer value
 * @return position of highest bit set   
 */

static
uint16_t
coap_log_2(uint16_t value)
{
  uint16_t result = 0;
  do {
    value = value >> 1;
    result++;
  } while (value);

  return result ? result - 1 : result;
}
/*-----------------------------------------------------------------------------------*/
/* @fn     coap_parse_int_option()
 * @brief  This function is used to parse the buffer data into int option
 * @param  bytes - pointer to bytes  to be parse 
 *         length - length of bytes buffer     
 * @param  int value - var 
 */

  static uint32_t
coap_parse_int_option(uint8_t *bytes, size_t length)
{
  uint32_t var = 0;
  size_t i = 0;
  while (i<length)
  {
    var <<= 8;
    var |= (uint32_t)bytes[i++];
  }
  return var;
}
/*-----------------------------------------------------------------------------------*/
/* @fn     static unit8_t coap_option_nibble()
 * @brief  This function is used to check the value range  
 * @param  value - integer value 
 * @return returns int value based on condition true 
 */
  static uint8_t 
coap_option_nibble(unsigned int value)
{
  if (value<13)
  {
    return value;
  }
  else if (value<=0xFF+13)
  {
    return 13;
  }
  else
  {
    return 14;
  }
}
/*-----------------------------------------------------------------------------------*/
/* @fn     static size_t coap_serialize_option_headrer ()
 * @brief  This function is used to  set  header options
 * @param  delta          - option feild number  
 length         - length of buffer 
 buffer         - pointer to buffer to be filled

 * @return return value - filled buffer length   
 */
  static size_t
coap_set_option_header(unsigned int delta, size_t length, uint8_t *buffer)
{
  size_t written = 0;

  buffer[0] = coap_option_nibble(delta)<<4 | coap_option_nibble(length);

  if(delta > 268) {
    buffer[++written] = ((delta - 269) >> 8) & 0xff;
    buffer[++written] = (delta - 269) & 0xff;
  } else if(delta > 12) {
    buffer[++written] = (delta - 13);
  }

  if(length > 268) {
    buffer[++written] = ((length - 269) >> 8) & 0xff;
    buffer[++written] = (length - 269) & 0xff;
  } else if(length > 12) {
    buffer[++written] = (length - 13);
  }

  PRINTF("WRITTEN %u B opt header", written);

  return ++written;
}
/*-----------------------------------------------------------------------------------*/
/* @fn     static size_t coap_serialize_int_option ()
 * @brief  This function is used to serialize  int options
 * @param  number         - coap option filed enum number
 *         current_number - integer value 
 *         buffer         - pointer to buffer to be filled
 *         value          - value to be  serialized  
 * @return return value filled buffer length   
 */

  static size_t
coap_serialize_int_option(unsigned int number, unsigned int current_number, uint8_t *buffer, uint32_t value)
{
  size_t i = 0;

  if (0xFF000000 & value) ++i;
  if (0xFFFF0000 & value) ++i;
  if (0xFFFFFF00 & value) ++i;
  if (0xFFFFFFFF & value) ++i;

  PRINTF("OPTION %u (delta %u, len %u)", number, number - current_number, i);

  i = coap_set_option_header(number - current_number, i, buffer);

  if (0xFF000000 & value) buffer[i++] = (uint8_t) (value>>24);
  if (0xFFFF0000 & value) buffer[i++] = (uint8_t) (value>>16);
  if (0xFFFFFF00 & value) buffer[i++] = (uint8_t) (value>>8);
  if (0xFFFFFFFF & value) buffer[i++] = (uint8_t) (value);

  return i;
}
/*-----------------------------------------------------------------------------------*/
/* @fn     static size_t coap_serialize_array_option ()
 * @brief  This function is used to serialize  array options
 * @param  number         - coap option filed enum number
 *         current_number - integer value 
 *         buffer         - pointer to buffer to be filled  
 *         array          - pointer to multi_option array
 *         length         - length of array 
 *         split_char     - split char value i.e '\0' or '&'     
 * @return return value   - serialized array option total  length   
 */
  static size_t
coap_serialize_array_option(unsigned int number, unsigned int current_number, uint8_t *buffer, uint8_t *array, size_t length, char split_char)
{
  size_t i = 0;

  if (split_char!='\0')
  {
    size_t j;
    uint8_t *part_start = array;
    uint8_t *part_end = NULL;
    size_t temp_length;

    for (j = 0; j<=length; ++j)
    {
      if (array[j]==split_char || j==length)
      {
        part_end = array + j;
        temp_length = part_end-part_start;

        i += coap_set_option_header(number - current_number, temp_length, &buffer[i]);
        COAP_MEMCPY(&buffer[i], temp_length, part_start, temp_length);
        i += temp_length;

        PRINTF("OPTION type %u, delta %u, len %u, part [%.*s]", number, number - current_number, i, temp_length, part_start);

        ++j; /* skip the splitter */
        current_number = number;
        part_start = array + j;
      }
    } /* for */
  }
  else
  {
    i += coap_set_option_header(number - current_number, length, &buffer[i]);
    COAP_MEMCPY(&buffer[i], length, array, length);
    i += length;

    PRINTF("OPTION type %u, delta %u, len %u", number, number - current_number, length);
  }

  return i;
}
/*-----------------------------------------------------------------------------------*/
/* @fn     static size_t coap_serialize_multi_option ()
 * @brief  This function is used to serialize  multi options
 * @param  number         - coap option filed enum number
 *         current_number - integer value 
 *         buffer         - pointer to buffer to be filled  
 *         array          - pointer to multi_option array
 * @return return value   - serialized option total  length   
 */
  static size_t
coap_serialize_multi_option(unsigned int number, unsigned int current_number, uint8_t *buffer, multi_option_t *array)
{
  size_t i = 0;
  multi_option_t * j;

  for (j = array; j != NULL; j= j->next)
  {
    i += coap_set_option_header(number - current_number, j->len, &buffer[i]);
    current_number = number;
    COAP_MEMCPY(&buffer[i], j->len, j->data, j->len);
    i += j->len;
  } /* for */

  return i;
}

  static size_t
coap_serialize_ext_option(unsigned int number, unsigned int current_number, uint8_t *buffer, coap_packet_ext_opt_t *array, uint8_t curr_num)
{
  size_t i = 0, k = 0;
  coap_ext_msg_option_t * j = array->ext_option;

  while(k < curr_num && j != NULL)
  {
    j = j->next;
    k++;
  }

  if(j != NULL)
  {
    i += coap_set_option_header(number - current_number, j->len, &buffer[i]);
    current_number = number;
    COAP_MEMCPY(&buffer[i], j->len, j->val, j->len);
    i += j->len;
  } /* for */

  return i;
}

/*-----------------------------------------------------------------------------------*/

/* @fn     static void coap_add_multi_option ()
 * @brief  This function is used to merge multi option
 * @param  dst        - double pointer to fill multi option pointer 
 *         option     - pointer to option field to merge
 *         option_len - length of option     
 *         separator  - separator i.e &
 * @return void        
 */
  static void
coap_merge_multi_option(uint8_t **dst, size_t *dst_len, uint8_t *option, size_t option_len, char separator)
{
  /* Merge multiple options. */
  if (*dst_len > 0)
  {
    /* dst already contains an option: concatenate */
    (*dst)[*dst_len] = separator;
    *dst_len += 1;

    /* memmove handles 2-byte option headers */
    COAP_MEMMOVE((*dst)+(*dst_len), option_len, option, option_len);

    *dst_len += option_len;
  }
  else
  {
    /* dst is empty: set to option */
    *dst = option;
    *dst_len = option_len;
  }
}
/* @fn     void coap_add_multi_option ()
 * @brief  This function is used to add multi option 
 * @param  dst        - double pointer to fill multi option pointer 
 *         option     - pointer to option field to add
 *         option_len - length of option     
 *         is_stati   - 1 or 0 
 *                      1 - option field is static
 *                      0 - option field is dynamic  
 * @return void                     
 */
  void
coap_add_multi_option(multi_option_t **dst, uint8_t *option, size_t option_len, uint8_t is_static,void * umem)
{
  multi_option_t *opt = NULL;

  opt = (multi_option_t *)mem_utility_Get_Memory_From_Pool(umem, sizeof(multi_option_t));

  if (opt)
  {
    opt->next = NULL;
    opt->len = (uint8_t)option_len;
    if (is_static)
    {
      opt->data = option;
      opt->is_static = 1;
    }
    else
    {
      opt->is_static = 0;

	  opt->data = (uint8_t *)mem_utility_Get_Memory_From_Pool(umem, option_len);

      if (opt->data == NULL)
      {
        mem_utility_release_memory(umem!=NULL, opt);
        return;
      }
      memset(opt->data,0,option_len);
      COAP_MEMCPY(opt->data, option_len, option, option_len);
    }

    if (*dst)
    {
      multi_option_t * i = *dst;
      while (i->next)
      {
        i = i->next;
      }
      i->next = opt;
    }
    else
    {
      *dst = opt;
    }
  }
}
/* @fn     free_multi_option() 
 * @brief  This function is used to free the mutli options 
 * @param  dst - pointer to multi_option_t 
 * @return void
 */   
  void
free_multi_option(multi_option_t *dst,void * umem)
{
  if (dst)
  {
    multi_option_t *n = dst->next;
    dst->next = NULL;
    if ((dst->is_static == 0) && (dst->data))
    {
      mem_utility_release_memory(umem!=NULL, dst->data);
    }

    mem_utility_release_memory(umem!=NULL, dst);
    free_multi_option(n,umem);
  }
}

void free_ext_msg_option(coap_ext_msg_option_t *ext_option, void * umem)
{
  if(ext_option != NULL)
  {
    coap_ext_msg_option_t *nextP;

    nextP = ext_option->next;
    if(ext_option->val != NULL)
    {
      mem_utility_release_memory(umem!=NULL, ext_option->val);
    }

    mem_utility_release_memory(umem!=NULL, ext_option);
    free_ext_msg_option(nextP, umem);
  }
}

void 
free_ext_option(coap_packet_ext_opt_t *dst,void * umem)
{
  coap_ext_msg_option_t *ext_opt = NULL;
  if (dst)
  {
    COAP_LIST_FREE(dst->options, umem);

    ext_opt = dst->ext_option;

    free_ext_msg_option(ext_opt, umem);

    dst->options = NULL;
    dst->ext_option= NULL;
  }
}
 
/* @fn     char * coap_get_multi_option_as_string()
 * @brief  This function is used to convert multi options as string 
 * @param  option - pointer to multi_option_t
 * @return char pointer to output string  
 *
 */
char * coap_get_multi_option_as_string(multi_option_t * option , int * length) 
{
  size_t len = 0;
  multi_option_t * opt;
  char * output;

  for (opt = option; opt != NULL; opt = opt->next)
  {
    len += opt->len + 1;     // for separator
  }

  output = malloc(len + 1); // for String terminator

  if (output != NULL)
  {
    size_t i = 0;

    for (opt = option; opt != NULL; opt = opt->next)
    {
      
      output[i] = '/';
      i += 1;
      
      COAP_MEMMOVE(output + i, len + 1 - i, opt->data, opt->len);
      i += opt->len;
    }
    output[i] = 0;
	*length = i;
  }

  return output;
}


/* @fn     char * coap_get_multi_option_as_string()
 * @brief  This function is used to convert multi options as string 
 * @param  option - pointer to multi_option_t
 * @return char pointer to output string  
 *
 */
char * coap_get_multi_option_as_string_for_uri_query(multi_option_t * option , int * length)
{
  size_t len = 0;
  multi_option_t * opt;
  char * output;

  for (opt = option; opt != NULL; opt = opt->next)
  {
    len += opt->len + 1;     // for separator
  }

     output = malloc(len + 1); // for String terminator
 
  if (output != NULL)
  {
    size_t i = 0;

    for (opt = option; opt != NULL; opt = opt->next)
    {
      COAP_MEMMOVE(output + i, len + 1 - i, opt->data, opt->len);
      i += opt->len;
    }
    output[i] = 0;
	*length = i;
  }

 
  
  return output;
}

/*-----------------------------------------------------------------------------------*/
/*- MEASSAGE PROCESSING -------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------*/
/* @fn     void coap_init_message()
 * @brief  This function is used to initialize  the coap message 
 * @param  packet - pointer to coap_packet_t 
 *         type   - coap message type 
 *         mid    - message id
 * @return void
 */

  void
coap_init_message(void *packet, coap_message_type_t type, uint8_t code, uint16_t mid)
{
  coap_packet_ext_t *const coap_pkt = (coap_packet_ext_t *) packet;

   if(coap_pkt == NULL)
  {
     return ;
  }

  /* Important thing */
  memset(coap_pkt, 0, sizeof(coap_packet_ext_t));

  coap_pkt->coap_pkt.type = type;
  coap_pkt->coap_pkt.code = code;
  coap_pkt->coap_pkt.mid = mid;
}
/* @fn     void coap_free_header()
 * @brief  This function is used to free the coap header 
 * @param  packet - pointer to coap_packet
 * @return void
 */ 
  void
coap_free_header(void *packet, void * umem)
{
  coap_packet_ext_t *const coap_pkt = (coap_packet_ext_t *) packet;

  free_multi_option(coap_pkt->coap_pkt.uri_path,umem);
  free_multi_option(coap_pkt->coap_pkt.uri_query,umem);
  free_multi_option(coap_pkt->coap_pkt.location_path,umem);
  coap_pkt->coap_pkt.uri_path = NULL;
  coap_pkt->coap_pkt.uri_query = NULL;
  coap_pkt->coap_pkt.location_path = NULL;

  free_ext_option(&coap_pkt->coap_ext_opt, umem);
}
/* @fn     size_t coap_serialize_get_size()
 * @brief  This function is used to get the size of coap packet
 * @param  packet - pointer to coap packet
 * @return length - length of packet 
 */

/*-----------------------------------------------------------------------------------*/
size_t coap_serialize_get_size(void *packet)
{
  coap_packet_ext_t *const coap_pkt = (coap_packet_ext_t *) packet;
  size_t length = 0;
  coap_opt_num_t *options = NULL;
  uint8_t ext_num = 0;

  length = COAP_HEADER_LEN + coap_pkt->coap_pkt.payload_len + coap_pkt->coap_pkt.token_len;

  options = coap_pkt->coap_ext_opt.options;

  while(options)
  {
    switch(options->opt_num)
    {
      case COAP_OPTION_IF_MATCH:
      {
        if (IS_OPTION(&coap_pkt->coap_pkt, COAP_OPTION_IF_MATCH))
        {
          length += COAP_MAX_OPTION_HEADER_LEN + coap_pkt->coap_pkt.if_match_len;
        }
      }
      break;

      case COAP_OPTION_URI_HOST:
      {
        if (IS_OPTION(&coap_pkt->coap_pkt, COAP_OPTION_URI_HOST))
        {
          length += COAP_MAX_OPTION_HEADER_LEN + coap_pkt->coap_pkt.uri_host_len;
        }
      }
      break;

      case COAP_OPTION_ETAG:
      {
        if (IS_OPTION(&coap_pkt->coap_pkt, COAP_OPTION_ETAG))
        {
          length += COAP_MAX_OPTION_HEADER_LEN + coap_pkt->coap_pkt.etag_len;
        }
      }
      break;

      case COAP_OPTION_IF_NONE_MATCH:
      {
        if (IS_OPTION(&coap_pkt->coap_pkt, COAP_OPTION_IF_NONE_MATCH))
        {
          // can be stored in extended fields
          length += COAP_MAX_OPTION_HEADER_LEN;
        }
      }
      break;

      case COAP_OPTION_OBSERVE:
      {
        if (IS_OPTION(&coap_pkt->coap_pkt, COAP_OPTION_OBSERVE))
        {
          // can be stored in extended fields
          length += COAP_MAX_OPTION_HEADER_LEN;
        }
      }
      break;

      case COAP_OPTION_URI_PORT:
      {
        if (IS_OPTION(&coap_pkt->coap_pkt, COAP_OPTION_URI_PORT))
        {
          // can be stored in extended fields
          length += COAP_MAX_OPTION_HEADER_LEN;
        }
      }
      break;

      case COAP_OPTION_LOCATION_PATH:
      {
        if (IS_OPTION(&coap_pkt->coap_pkt, COAP_OPTION_LOCATION_PATH))
        {
          multi_option_t * optP;
      
          for (optP = coap_pkt->coap_pkt.location_path ; optP != NULL ; optP = optP->next)
          {
            length += COAP_MAX_OPTION_HEADER_LEN + optP->len;
          }
        }
      }
      break;

      case COAP_OPTION_URI_PATH:
      {
        if (IS_OPTION(&coap_pkt->coap_pkt, COAP_OPTION_URI_PATH))
        {
          multi_option_t * optP;
      
          for (optP = coap_pkt->coap_pkt.uri_path ; optP != NULL ; optP = optP->next)
          {
            length += COAP_MAX_OPTION_HEADER_LEN + optP->len;
          }
        }
      }
      break;
      
      case COAP_OPTION_CONTENT_TYPE:
      {
        if (IS_OPTION(&coap_pkt->coap_pkt, COAP_OPTION_CONTENT_TYPE))
        {
          // can be stored in extended fields
          length += COAP_MAX_OPTION_HEADER_LEN;
        }
      }
      break;

      case COAP_OPTION_MAX_AGE:
      {
        if (IS_OPTION(&coap_pkt->coap_pkt, COAP_OPTION_MAX_AGE))
        {
          // can be stored in extended fields
          length += COAP_MAX_OPTION_HEADER_LEN;
        }
      }
      break;

      case COAP_OPTION_URI_QUERY:
      {
        if (IS_OPTION(&coap_pkt->coap_pkt, COAP_OPTION_URI_QUERY))
        {
          multi_option_t * optP;
      
          for (optP = coap_pkt->coap_pkt.uri_query ; optP != NULL ; optP = optP->next)
          {
            length += COAP_MAX_OPTION_HEADER_LEN + optP->len;
          }
        }
      }
      break;

      case COAP_OPTION_ACCEPT:
      {
        if (IS_OPTION(&coap_pkt->coap_pkt, COAP_OPTION_ACCEPT))
        {
          length += coap_pkt->coap_pkt.accept_num * COAP_MAX_OPTION_HEADER_LEN;
        }
      }
      break;

      case COAP_OPTION_LOCATION_QUERY:
      {
        if (IS_OPTION(&coap_pkt->coap_pkt, COAP_OPTION_LOCATION_QUERY))
        {
          length += COAP_MAX_OPTION_HEADER_LEN + coap_pkt->coap_pkt.location_query_len;
        }
      }
      break;

      case COAP_OPTION_BLOCK2:
      {
        if (IS_OPTION(&coap_pkt->coap_pkt, COAP_OPTION_BLOCK2))
        {
          // can be stored in extended fields
          length += COAP_MAX_OPTION_HEADER_LEN;
        }
      }
      break;

      case COAP_OPTION_BLOCK1:
      {
        if (IS_OPTION(&coap_pkt->coap_pkt, COAP_OPTION_BLOCK1))
        {
          // can be stored in extended fields
          length += COAP_MAX_OPTION_HEADER_LEN;
        }
      }
      break;

      case COAP_OPTION_SIZE:
      {
        if (IS_OPTION(&coap_pkt->coap_pkt, COAP_OPTION_SIZE))
        {
          // can be stored in extended fields
          length += COAP_MAX_OPTION_HEADER_LEN;
        }
      }
      break;
      case COAP_OPTION_SIZE1:
      {
        if (IS_OPTION(&coap_pkt->coap_pkt, COAP_OPTION_SIZE1))
        {
          // can be stored in extended fields
          length += COAP_MAX_OPTION_HEADER_LEN;
        }
      }
      break;

      case COAP_OPTION_PROXY_URI:
      {
        if (IS_OPTION(&coap_pkt->coap_pkt, COAP_OPTION_PROXY_URI))
        {
          length += COAP_MAX_OPTION_HEADER_LEN + coap_pkt->coap_pkt.proxy_uri_len;
        }
      }
      break;

      default:
      {
        if(ext_num < coap_pkt->coap_ext_opt.ext_option_count)
        {
          int k = 0;
          coap_ext_msg_option_t *j = coap_pkt->coap_ext_opt.ext_option;
          while(k < ext_num && j != NULL)
          {
            j = j->next;
            k++;
          }
          if(j != NULL)
          {
            length += COAP_MAX_OPTION_HEADER_LEN + j->len;
            ext_num++;
          }
        }
      }
    }
    options = options->next;
  }
  if (coap_pkt->coap_pkt.payload_len)
  {
    // Account for the payload marker.
    length += 1;
  }

  return length;
}



/*-----------------------------------------------------------------------------------*/
/* @fn     size_t coap_serialize_message()
 * @brief  This function is used to serialize the coap packet into buffer  
 * @param  packet - pointer to coap packet 
 *         buffer - coap buffer pointer   

 * @return packet length
 */

  size_t
coap_serialize_message(void *packet, uint8_t *buffer)
{
  coap_packet_ext_t *const coap_pkt = (coap_packet_ext_t *) packet;
  uint8_t *option;
  unsigned int current_number = 0;
  uint8_t ext_current = 0;
  coap_opt_num_t *options = NULL;

  /* Initialize */
  coap_pkt->coap_pkt.buffer = buffer;
  coap_pkt->coap_pkt.version = 1;

  PRINTF("-Serializing MID %u to %p, ", coap_pkt->coap_pkt.mid, coap_pkt->coap_pkt.buffer);

  /* set header fields */
  coap_pkt->coap_pkt.buffer[0]  = 0x00;
  coap_pkt->coap_pkt.buffer[0] |= COAP_HEADER_VERSION_MASK & (coap_pkt->coap_pkt.version)<<COAP_HEADER_VERSION_POSITION;
  coap_pkt->coap_pkt.buffer[0] |= COAP_HEADER_TYPE_MASK & (coap_pkt->coap_pkt.type)<<COAP_HEADER_TYPE_POSITION;
  coap_pkt->coap_pkt.buffer[0] |= COAP_HEADER_TOKEN_LEN_MASK & (coap_pkt->coap_pkt.token_len)<<COAP_HEADER_TOKEN_LEN_POSITION;
  coap_pkt->coap_pkt.buffer[1] = coap_pkt->coap_pkt.code;
  coap_pkt->coap_pkt.buffer[2] = (uint8_t) ((coap_pkt->coap_pkt.mid)>>8);
  coap_pkt->coap_pkt.buffer[3] = (uint8_t) (coap_pkt->coap_pkt.mid);

  /* set Token */
  PRINTF("Token (len %u)", coap_pkt->coap_pkt.token_len);
  option = coap_pkt->coap_pkt.buffer + COAP_HEADER_LEN;
  for (current_number=0; current_number<coap_pkt->coap_pkt.token_len; ++current_number)
  {
    PRINTF(" %02X", coap_pkt->coap_pkt.token[current_number]);
    *option = coap_pkt->coap_pkt.token[current_number];
    ++option;
  }
  PRINTF("-");

  /* Serialize options */
  current_number = 0;

  PRINTF("-Serializing options at %p-", option);

  options = coap_pkt->coap_ext_opt.options;
  while(options != NULL)
  {
    switch(options->opt_num)
    {
      /* The options must be serialized in the order of their number */
      case COAP_OPTION_IF_MATCH:
        COAP_SERIALIZE_BYTE_OPTION(   COAP_OPTION_IF_MATCH,       if_match, "If-Match")
      break;
  
      case COAP_OPTION_URI_HOST:
        COAP_SERIALIZE_STRING_OPTION( COAP_OPTION_URI_HOST,       uri_host, '\0', "Uri-Host")
      break;
  
      case COAP_OPTION_ETAG:
        COAP_SERIALIZE_BYTE_OPTION(   COAP_OPTION_ETAG,           etag, "ETag")
      break;
  
      case COAP_OPTION_IF_NONE_MATCH:
        COAP_SERIALIZE_INT_OPTION(    COAP_OPTION_IF_NONE_MATCH,  content_type-coap_pkt->coap_pkt.content_type, "If-None-Match") /* hack to get a zero field */
      break;
  
      case COAP_OPTION_OBSERVE:
        COAP_SERIALIZE_INT_OPTION(    COAP_OPTION_OBSERVE,        observe, "Observe")
      break;
  
      case COAP_OPTION_URI_PORT:
        COAP_SERIALIZE_INT_OPTION(    COAP_OPTION_URI_PORT,       uri_port, "Uri-Port")
      break;
  
      case COAP_OPTION_LOCATION_PATH:
        COAP_SERIALIZE_MULTI_OPTION(  COAP_OPTION_LOCATION_PATH,  location_path, "Location-Path")
      break;
  
      case COAP_OPTION_URI_PATH:
        COAP_SERIALIZE_MULTI_OPTION(  COAP_OPTION_URI_PATH,       uri_path, "Uri-Path")
      break;
  
      case COAP_OPTION_CONTENT_TYPE:
        COAP_SERIALIZE_INT_OPTION(    COAP_OPTION_CONTENT_TYPE,   content_type, "Content-Format")
      break;
  
      case COAP_OPTION_MAX_AGE:
        COAP_SERIALIZE_INT_OPTION(    COAP_OPTION_MAX_AGE,        max_age, "Max-Age")
      break;
  
      case COAP_OPTION_URI_QUERY:
        COAP_SERIALIZE_MULTI_OPTION(  COAP_OPTION_URI_QUERY,      uri_query, "Uri-Query")
      break;
  
      case COAP_OPTION_ACCEPT:
        COAP_SERIALIZE_ACCEPT_OPTION( COAP_OPTION_ACCEPT,         accept, "Accept")
      break;
  
      case COAP_OPTION_LOCATION_QUERY:
        COAP_SERIALIZE_STRING_OPTION( COAP_OPTION_LOCATION_QUERY, location_query, '&', "Location-Query")
       break;
  
      case COAP_OPTION_BLOCK2:
        COAP_SERIALIZE_BLOCK_OPTION(  COAP_OPTION_BLOCK2,         block2, "Block2")
      break;
  
      case COAP_OPTION_BLOCK1:
        COAP_SERIALIZE_BLOCK_OPTION(  COAP_OPTION_BLOCK1,         block1, "Block1")
      break;
  
      case COAP_OPTION_SIZE:
        COAP_SERIALIZE_INT_OPTION(    COAP_OPTION_SIZE,           size, "Size")
      break;
        
      case COAP_OPTION_SIZE1:
        COAP_SERIALIZE_INT_OPTION(    COAP_OPTION_SIZE1,           size1, "Size1")
      break;
  
      case COAP_OPTION_PROXY_URI:
        COAP_SERIALIZE_STRING_OPTION( COAP_OPTION_PROXY_URI,      proxy_uri, '\0', "Proxy-Uri")
      break;

      default:
        COAP_SERIALIZE_EXT_OPTION(options->opt_num)
      break;
    }

    options = options->next;
  }
  PRINTF("-Done serializing at %p----", option);



  /* Pack payload */
  /* Payload marker */
  if (coap_pkt->coap_pkt.payload_len)
  {
    *option = 0xFF;
    ++option;
  }

  COAP_MEMMOVE(option, coap_pkt->coap_pkt.payload_len, coap_pkt->coap_pkt.payload, coap_pkt->coap_pkt.payload_len);

  PRINTF("-Done %u B (header len %u, payload len %u)-", coap_pkt->coap_pkt.payload_len + option - buffer, option - buffer, coap_pkt->coap_pkt.payload_len);

  PRINTF("Dump [0x%02X %02X %02X %02X  %02X %02X %02X %02X]",
      coap_pkt->coap_pkt.buffer[0],
      coap_pkt->coap_pkt.buffer[1],
      coap_pkt->coap_pkt.buffer[2],
      coap_pkt->coap_pkt.buffer[3],
      coap_pkt->coap_pkt.buffer[4],
      coap_pkt->coap_pkt.buffer[5],
      coap_pkt->coap_pkt.buffer[6],
      coap_pkt->coap_pkt.buffer[7]
      );

  return (option - buffer) + coap_pkt->coap_pkt.payload_len; /* packet length */
}
/*-----------------------------------------------------------------------------------*/
/* @fn     coap_status_t coap_parse_message()
 * @brief  This function is used to parse the coap header from packet 
 * @param  packet  - pointer to coap packet  
 *         data    - pointer to buffer
 *         datalen - length of buffer
 * @return on success - NO_ERROR
 *         on error   - coap error code  
 */

  coap_status_t
coap_parse_message(void *packet, uint8_t *data, uint16_t data_len, void *umem)
{
  uint8_t *current_option = NULL;
  uint8_t *data_end = data + data_len;
  uint32_t option_number = 0;
  uint32_t option_delta = 0;
  uint32_t option_length = 0;
  coap_packet_ext_t *const coap_pkt = (coap_packet_ext_t *) packet;
  uint32_t *x;
  int i;

  if(coap_pkt == NULL)
  {
     return MEMORY_ALLOCATION_ERROR;
  }

  /* 
   * Minimum length: 
   * CoAP packet length can not be less than minimal CoAP message size(4 byte)
   */
   
  if (data_len < COAP_HEADER_LEN) 
  {
	  return BAD_REQUEST_4_00;
  }
  
  /* Initialize packet */
  memset(coap_pkt, 0, sizeof(coap_packet_ext_t));

  /* pointer to packet bytes */
  coap_pkt->coap_pkt.buffer = data;

  /* parse header fields */
  coap_pkt->coap_pkt.version = (COAP_HEADER_VERSION_MASK & coap_pkt->coap_pkt.buffer[0])>>COAP_HEADER_VERSION_POSITION;
  coap_pkt->coap_pkt.type = (coap_message_type_t) ( (COAP_HEADER_TYPE_MASK & coap_pkt->coap_pkt.buffer[0])>>COAP_HEADER_TYPE_POSITION );
  coap_pkt->coap_pkt.token_len = (COAP_HEADER_TOKEN_LEN_MASK & coap_pkt->coap_pkt.buffer[0])>>COAP_HEADER_TOKEN_LEN_POSITION;
  coap_pkt->coap_pkt.code = coap_pkt->coap_pkt.buffer[1];
  coap_pkt->coap_pkt.mid = coap_pkt->coap_pkt.buffer[2]<<8 | coap_pkt->coap_pkt.buffer[3];
  coap_pkt->coap_pkt.buffer_len = data_len;

  if(IS_PARSED_COAP_PACKETS_INVALID(coap_pkt->coap_pkt))
  {
    return BAD_REQUEST_4_00;
  }
  
  current_option = data + COAP_HEADER_LEN;

  if (coap_pkt->coap_pkt.token_len != 0)
  {
    if (current_option + coap_pkt->coap_pkt.token_len > data_end) 
	{
       return BAD_REQUEST_4_00;
    }
    COAP_MEMCPY(coap_pkt->coap_pkt.token, COAP_TOKEN_LEN, current_option, coap_pkt->coap_pkt.token_len);
    SET_OPTION(&coap_pkt->coap_pkt, COAP_OPTION_TOKEN);
  }

  /* parse options */
  current_option += coap_pkt->coap_pkt.token_len;

  while (current_option < data_end)
  {
    int set_option = 1;

    /* Payload marker 0xFF */
    if (current_option[0] == 0xFF)
    {

	  /*
       * The presence of a marker followed by a zero-length payload MUST be
       * processed as a message format error.
       */
      if (current_option + 1 >= data_end) 
	  {
        return BAD_REQUEST_4_00;
      }
      coap_pkt->coap_pkt.payload = ++current_option;
      coap_pkt->coap_pkt.payload_len = data_end - coap_pkt->coap_pkt.payload;

	  coap_pkt->coap_pkt.payload = (uint8_t *)mem_utility_Get_Memory_From_Pool(umem, coap_pkt->coap_pkt.payload_len);

      if(coap_pkt->coap_pkt.payload == NULL)
      {
        return MEMORY_ALLOCATION_ERROR;
      }

      if(coap_pkt->coap_pkt.payload)
      {
        memset(coap_pkt->coap_pkt.payload,0,coap_pkt->coap_pkt.payload_len);
        memcpy(coap_pkt->coap_pkt.payload,current_option,coap_pkt->coap_pkt.payload_len);
      }

      break;
    }

    option_delta = current_option[0]>>4;
    option_length = current_option[0] & 0x0F;

    /* Error on reserved option values */
	if ((option_delta == COAP_HEADER_OPTION_DELTA_RESERVED) ||
       (option_length == COAP_HEADER_OPTION_LENGTH_RESERVED)) 
    {
      return BAD_REQUEST_4_00;
    }
		
	/* avoids code duplication without function overhead */
    x = &option_delta;
	for (i = 0; i < 2; i++)
    {
      if (*x==13)
      {
        if (++current_option >= data_end) {
          return BAD_REQUEST_4_00;
        }
        *x += current_option[0];
      }
      else if (*x==14)
      {
        *x += 255;
        if (++current_option >= data_end) {
          return BAD_REQUEST_4_00;
        }
        *x += current_option[0]<<8;
        if (++current_option >= data_end) {
          return BAD_REQUEST_4_00;
        }
        *x += current_option[0];
      }
	  x = &option_length;
    } 
	option_number += option_delta;

    if (++current_option + option_length > data_end)
    {
      return BAD_REQUEST_4_00;
    }
    
    switch (option_number)
    {
      case COAP_OPTION_CONTENT_TYPE:
        coap_pkt->coap_pkt.content_type = (coap_content_type_t) coap_parse_int_option(current_option, option_length);
    
        break;
      case COAP_OPTION_MAX_AGE:
        coap_pkt->coap_pkt.max_age = coap_parse_int_option(current_option, option_length);
     
        break;
      case COAP_OPTION_ETAG:
        coap_pkt->coap_pkt.etag_len = (uint8_t)(MIN(COAP_ETAG_LEN, option_length));
        COAP_MEMCPY(coap_pkt->coap_pkt.etag, COAP_ETAG_LEN, current_option, coap_pkt->coap_pkt.etag_len);
    
        break;
      case COAP_OPTION_ACCEPT:
        if (coap_pkt->coap_pkt.accept_num < COAP_MAX_ACCEPT_NUM)
        {
          coap_pkt->coap_pkt.accept[coap_pkt->coap_pkt.accept_num] = (coap_content_type_t)coap_parse_int_option(current_option, option_length);
          coap_pkt->coap_pkt.accept_num += 1;
        }
        break;
      case COAP_OPTION_IF_MATCH:
        /*FIXME support multiple ETags */
        coap_pkt->coap_pkt.if_match_len = (uint8_t)(MIN(COAP_ETAG_LEN, option_length));
        COAP_MEMCPY(coap_pkt->coap_pkt.if_match, COAP_ETAG_LEN, current_option, coap_pkt->coap_pkt.if_match_len);
        break;
      case COAP_OPTION_IF_NONE_MATCH:
        coap_pkt->coap_pkt.if_none_match = 1;
        break;

      case COAP_OPTION_URI_HOST:
        coap_pkt->coap_pkt.uri_host = current_option;
        coap_pkt->coap_pkt.uri_host_len = option_length;
        break;
      case COAP_OPTION_URI_PORT:
        coap_pkt->coap_pkt.uri_port = coap_parse_int_option(current_option, option_length);
        break;
      case COAP_OPTION_URI_PATH:
        /* coap_merge_multi_option() operates in-place on the IPBUF, but final packet field should be const string -> cast to string */
        // coap_merge_multi_option( (char **) &(coap_pkt->uri_path), &(coap_pkt->uri_path_len), current_option, option_length, 0);
		/*is_static set to 0, since it contains pointers to the original buffer, 
		which might get corrupted if another packet is received before this is processed */
        coap_add_multi_option( &(coap_pkt->coap_pkt.uri_path), current_option, option_length, 0, umem);
        break;
      case COAP_OPTION_URI_QUERY:
        /* coap_merge_multi_option() operates in-place on the IPBUF, but final packet field should be const string -> cast to string */
        // coap_merge_multi_option( (char **) &(coap_pkt->uri_query), &(coap_pkt->uri_query_len), current_option, option_length, '&');
		/*is_static set to 0, since it contains pointers to the original buffer, 
		which might get corrupted if another packet is received before this is processed */
        coap_add_multi_option( &(coap_pkt->coap_pkt.uri_query), current_option, option_length, 0, umem);
        break;

      case COAP_OPTION_LOCATION_PATH:
	  	/*is_static set to 0, since it contains pointers to the original buffer, 
		which might get corrupted if another packet is received before this is processed */
        coap_add_multi_option( &(coap_pkt->coap_pkt.location_path), current_option, option_length, 0, umem);
        break;
      case COAP_OPTION_LOCATION_QUERY:
        /* coap_merge_multi_option() operates in-place on the IPBUF, but final packet field should be const string -> cast to string */
        coap_merge_multi_option( &(coap_pkt->coap_pkt.location_query), &(coap_pkt->coap_pkt.location_query_len), current_option, option_length, '&');
        break;

      case COAP_OPTION_PROXY_URI:
        /*FIXME check for own end-point */
        coap_pkt->coap_pkt.proxy_uri = current_option;
        coap_pkt->coap_pkt.proxy_uri_len = option_length;
        /*TODO length > 270 not implemented (actually not required) */
        coap_error_message = "This is a constrained server (Contiki)";
        return PROXYING_NOT_SUPPORTED_5_05;
        //commented the break to address the dead code, as this case is currently not supported 
        //break;

      case COAP_OPTION_OBSERVE:
        coap_pkt->coap_pkt.observe = coap_parse_int_option(current_option, option_length);
        break;
      case COAP_OPTION_BLOCK2:
       coap_pkt->coap_pkt.block2_num = coap_parse_int_option(current_option, option_length);
       coap_pkt->coap_pkt.block2_more = (coap_pkt->coap_pkt.block2_num & 0x08)>>3;
       coap_pkt->coap_pkt.block2_size = 16 << (coap_pkt->coap_pkt.block2_num & 0x07);
       coap_pkt->coap_pkt.block2_offset = (coap_pkt->coap_pkt.block2_num & ~0x0000000F)<<(coap_pkt->coap_pkt.block2_num & 0x07);
       coap_pkt->coap_pkt.block2_num >>= 4;
       break;
      case COAP_OPTION_BLOCK1:
        coap_pkt->coap_pkt.block1_num = coap_parse_int_option(current_option, option_length);
        coap_pkt->coap_pkt.block1_more = (coap_pkt->coap_pkt.block1_num & 0x08)>>3;
        coap_pkt->coap_pkt.block1_size = 16 << (coap_pkt->coap_pkt.block1_num & 0x07);
        coap_pkt->coap_pkt.block1_offset = (coap_pkt->coap_pkt.block1_num & ~0x0000000F)<<(coap_pkt->coap_pkt.block1_num & 0x07);
        coap_pkt->coap_pkt.block1_num >>= 4;
        break;
      case COAP_OPTION_SIZE:
        coap_pkt->coap_pkt.size = coap_parse_int_option(current_option, option_length);
        break;
      case COAP_OPTION_SIZE1:
        coap_pkt->coap_pkt.size1 = coap_parse_int_option(current_option, option_length);
        break;
      default:
        /* Check if critical (odd) */
        /*if (option_number & 1)
        {
          coap_error_message = "Unsupported critical option";
          return BAD_OPTION_4_02;
        }*/

        coap_set_header_ext(coap_pkt, option_number, (const char *)current_option,
                            option_length, umem);
        set_option = 0;
    }

    if(set_option == 1)
      SET_OPTION(&coap_pkt->coap_pkt, option_number);

    coap_set_option(coap_pkt, option_number, umem);

    current_option += option_length;
  } /* for */
  return NO_ERROR;
}


/* Copy options from Src to dest CoAP Packet */
int coap_set_header_pkt(coap_packet_t * src_pkt , coap_packet_t * dest_pkt)
{
   if(dest_pkt == NULL || src_pkt == NULL)
     return -1;

   SET_COAP_OPTIONS(src_pkt , dest_pkt , COAP_OPTION_ETAG);
   SET_COAP_OPTIONS(src_pkt , dest_pkt , COAP_OPTION_LOCATION_PATH);
   SET_COAP_OPTIONS(src_pkt , dest_pkt , COAP_OPTION_URI_PATH);
   SET_COAP_OPTIONS(src_pkt , dest_pkt , COAP_OPTION_URI_QUERY);
   SET_COAP_OPTIONS(src_pkt , dest_pkt , COAP_OPTION_TOKEN);
   SET_COAP_OPTIONS(src_pkt , dest_pkt , COAP_OPTION_SIZE);
   SET_COAP_OPTIONS(src_pkt , dest_pkt , COAP_OPTION_SIZE1);
   SET_COAP_OPTIONS(src_pkt , dest_pkt , COAP_OPTION_CONTENT_TYPE);
   SET_COAP_OPTIONS(src_pkt , dest_pkt , COAP_OPTION_BLOCK1);
   SET_COAP_OPTIONS(src_pkt , dest_pkt , COAP_OPTION_BLOCK2);
   SET_COAP_OPTIONS(src_pkt , dest_pkt , COAP_OPTION_URI_HOST);
   SET_COAP_OPTIONS(src_pkt , dest_pkt , COAP_OPTION_ACCEPT);
   SET_COAP_OPTIONS(src_pkt , dest_pkt , COAP_OPTION_MAX_AGE);
   SET_COAP_OPTIONS(src_pkt , dest_pkt , COAP_OPTION_LOCATION_QUERY);
   SET_COAP_OPTIONS(src_pkt , dest_pkt , COAP_OPTION_OBSERVE);
 
   return 0;
}

/* @fn     int coap_copy_header()
 * @brief  This function is used to copy coap headers from src to dst coap packet
 * @param  dst_pkt - pointer to coap packet where headers are to be copied to
 *         src_pkt - pointer to coap packet where headers are to be copied from
 *         umem    - pointer to user memory byte pool, NULL if kernel mode
 * @return integer - -1 on failure, number of headers copied on success            
 */

int coap_copy_header(coap_packet_ext_t *dst_pkt , coap_packet_ext_t *src_pkt, void *umem)
{
    multi_option_t *opt = NULL;
    int res = 0;
    coap_opt_num_t *optNum = NULL;
    
	if (NULL == dst_pkt || NULL == src_pkt)
	{
        return -1;
	}
    optNum = src_pkt->coap_ext_opt.options;

    while (optNum && res < src_pkt->coap_ext_opt.option_count)
    {
        coap_set_option(dst_pkt, optNum->opt_num, umem);
        switch (optNum->opt_num)
        {
            case COAP_OPTION_IF_MATCH:
            {
                dst_pkt->coap_pkt.if_match_len = src_pkt->coap_pkt.if_match_len;
                COAP_MEMCPY(dst_pkt->coap_pkt.if_match, COAP_ETAG_LEN, src_pkt->coap_pkt.if_match, src_pkt->coap_pkt.if_match_len);
                res++;
            }
            break;            
            case COAP_OPTION_URI_HOST:
            {
                SET_OPTION(&dst_pkt->coap_pkt, COAP_OPTION_URI_HOST);
                dst_pkt->coap_pkt.uri_host = (uint8_t *)mem_utility_Get_Memory_From_Pool(umem, (src_pkt->coap_pkt.uri_host_len + 1));

                if (NULL == dst_pkt->coap_pkt.uri_host)
                    goto clean_copy;
                dst_pkt->coap_pkt.uri_host_len = src_pkt->coap_pkt.uri_host_len;
                COAP_STRCPY((char *)dst_pkt->coap_pkt.uri_host, (char *)src_pkt->coap_pkt.uri_host, src_pkt->coap_pkt.uri_host_len);
                res++;
            }
            break;
            case COAP_OPTION_ETAG:
            {
                SET_OPTION(&dst_pkt->coap_pkt, COAP_OPTION_ETAG);
                dst_pkt->coap_pkt.etag_len = src_pkt->coap_pkt.etag_len;
                COAP_MEMCPY(dst_pkt->coap_pkt.etag, COAP_ETAG_LEN, src_pkt->coap_pkt.etag, src_pkt->coap_pkt.etag_len);
                res++;
            }
            break;
            case COAP_OPTION_TOKEN:
            {
                SET_OPTION(&dst_pkt->coap_pkt, COAP_OPTION_TOKEN);
                dst_pkt->coap_pkt.token_len = src_pkt->coap_pkt.token_len;
                COAP_MEMCPY(dst_pkt->coap_pkt.token, COAP_TOKEN_LEN, src_pkt->coap_pkt.token, src_pkt->coap_pkt.token_len);
                res++;
            }
            break;
            case COAP_OPTION_IF_NONE_MATCH:
            {
                SET_OPTION(&dst_pkt->coap_pkt, COAP_OPTION_IF_NONE_MATCH);
                dst_pkt->coap_pkt.if_none_match = src_pkt->coap_pkt.if_none_match;
                res++;
            }
            break;
            case COAP_OPTION_OBSERVE:
            {
                SET_OPTION(&dst_pkt->coap_pkt, COAP_OPTION_OBSERVE);
                dst_pkt->coap_pkt.observe = src_pkt->coap_pkt.observe;
                res++;
            }
            break;
            case COAP_OPTION_URI_PORT:
            {
                SET_OPTION(&dst_pkt->coap_pkt, COAP_OPTION_URI_PORT);
                dst_pkt->coap_pkt.uri_port = src_pkt->coap_pkt.uri_port;
                res++;
            }
            break;
            case COAP_OPTION_LOCATION_PATH:
            {
                opt = src_pkt->coap_pkt.location_path;
                SET_OPTION(&dst_pkt->coap_pkt, COAP_OPTION_LOCATION_PATH);
                while (opt != NULL)
                {
                    coap_add_multi_option(&dst_pkt->coap_pkt.location_path, opt->data, opt->len, opt->is_static, umem);
                    opt=opt->next;
                }
                res++;
            }
            break;
            case COAP_OPTION_URI_PATH:
            {
                opt = src_pkt->coap_pkt.uri_path;
                SET_OPTION(&dst_pkt->coap_pkt, COAP_OPTION_URI_PATH);
                while (opt != NULL)
                {
                    coap_add_multi_option(&dst_pkt->coap_pkt.uri_path, opt->data, opt->len, opt->is_static, umem);
                    opt=opt->next;
                }
                res++;
            }
            break;
            case COAP_OPTION_CONTENT_TYPE:
            {
                SET_OPTION(&dst_pkt->coap_pkt, COAP_OPTION_CONTENT_TYPE);
                dst_pkt->coap_pkt.content_type = src_pkt->coap_pkt.content_type;
                res++;
            }
            break;
            case COAP_OPTION_MAX_AGE:
            {
                SET_OPTION(&dst_pkt->coap_pkt, COAP_OPTION_MAX_AGE);
                dst_pkt->coap_pkt.max_age = src_pkt->coap_pkt.max_age;
                res++;
            }
            break;
            case COAP_OPTION_URI_QUERY:
            {
                opt = src_pkt->coap_pkt.uri_query;
                SET_OPTION(&dst_pkt->coap_pkt, COAP_OPTION_URI_QUERY);
                while (opt != NULL)
                {
                    coap_add_multi_option(&dst_pkt->coap_pkt.uri_query, opt->data, opt->len, opt->is_static, umem);
                    opt=opt->next;
                }
                res++;
            }
            break;
            case COAP_OPTION_ACCEPT:
            {
                uint8_t i = 0;
                SET_OPTION(&dst_pkt->coap_pkt, COAP_OPTION_ACCEPT);
                dst_pkt->coap_pkt.accept_num = src_pkt->coap_pkt.accept_num;
                for (i = 0; i < src_pkt->coap_pkt.accept_num;i++)
                    dst_pkt->coap_pkt.accept[i] = src_pkt->coap_pkt.accept[i];
                res++;
            }
            break;
            case COAP_OPTION_LOCATION_QUERY:
            {
                SET_OPTION(&dst_pkt->coap_pkt, COAP_OPTION_LOCATION_QUERY);
                dst_pkt->coap_pkt.location_query = (uint8_t *)mem_utility_Get_Memory_From_Pool(umem, (src_pkt->coap_pkt.location_query_len + 1));

                if (NULL == dst_pkt->coap_pkt.location_query)
                    goto clean_copy;
                dst_pkt->coap_pkt.location_query_len = src_pkt->coap_pkt.location_query_len;
                COAP_STRCPY((char *)dst_pkt->coap_pkt.location_query, (char *)src_pkt->coap_pkt.location_query, src_pkt->coap_pkt.location_query_len);
                res++;
            }
            break;
            case COAP_OPTION_BLOCK2:
            {
                SET_OPTION(&dst_pkt->coap_pkt, COAP_OPTION_BLOCK2);
                dst_pkt->coap_pkt.block2_more = src_pkt->coap_pkt.block2_more;
                dst_pkt->coap_pkt.block2_num = src_pkt->coap_pkt.block2_num;
                dst_pkt->coap_pkt.block2_offset = src_pkt->coap_pkt.block2_offset;
                dst_pkt->coap_pkt.block2_size = src_pkt->coap_pkt.block2_size;
                res++;
            }
            break;
            case COAP_OPTION_BLOCK1:
            {
                SET_OPTION(&dst_pkt->coap_pkt, COAP_OPTION_BLOCK1);
                dst_pkt->coap_pkt.block1_more = src_pkt->coap_pkt.block1_more;
                dst_pkt->coap_pkt.block1_num = src_pkt->coap_pkt.block1_num;
                dst_pkt->coap_pkt.block1_offset = src_pkt->coap_pkt.block1_offset;
                dst_pkt->coap_pkt.block1_size = src_pkt->coap_pkt.block1_size;
                res++;
            }
            break;
            case COAP_OPTION_SIZE:
            {
                SET_OPTION(&dst_pkt->coap_pkt, COAP_OPTION_SIZE);
                dst_pkt->coap_pkt.size = src_pkt->coap_pkt.size;
                res++;
            }
            break;
            case COAP_OPTION_SIZE1:
            {
                SET_OPTION(&dst_pkt->coap_pkt, COAP_OPTION_SIZE1);
                dst_pkt->coap_pkt.size1 = src_pkt->coap_pkt.size1;
                res++;
            }
            break;
            case COAP_OPTION_PROXY_URI:
            {
                SET_OPTION(&dst_pkt->coap_pkt, COAP_OPTION_PROXY_URI);

                dst_pkt->coap_pkt.proxy_uri = (uint8_t *)mem_utility_Get_Memory_From_Pool(umem, (src_pkt->coap_pkt.proxy_uri_len + 1));
	
                if (NULL == dst_pkt->coap_pkt.proxy_uri)
                    goto clean_copy;
                dst_pkt->coap_pkt.proxy_uri_len = src_pkt->coap_pkt.proxy_uri_len;
                COAP_STRCPY((char *)dst_pkt->coap_pkt.proxy_uri, (char *)src_pkt->coap_pkt.proxy_uri, src_pkt->coap_pkt.proxy_uri_len);
                res++;
            }
            break;
            default:
                //Ignore
            break;
        }    
        optNum=optNum->next;
    }
    
    /* Add handling for src_pkt->coap_ext_opt->ext_option below */
    return res;
clean_copy:

    mem_utility_release_memory(umem!=NULL, dst_pkt->coap_pkt.uri_host);
	mem_utility_release_memory(umem!=NULL, dst_pkt->coap_pkt.location_query);
	mem_utility_release_memory(umem!=NULL, dst_pkt->coap_pkt.proxy_uri);
	mem_utility_release_memory(umem!=NULL, dst_pkt->coap_ext_opt.options);

    free_multi_option(dst_pkt->coap_pkt.location_path,umem);
    free_multi_option(dst_pkt->coap_pkt.uri_path,umem);
    free_multi_option(dst_pkt->coap_pkt.uri_query,umem);

    return -1;
}

/*-----------------------------------------------------------------------------------*/
/* @fn    int coap_set_status_code ()
 * @brief This function is used to set status code into coap packet
 * @param packet - pointer to coap packet 
 *        code   - coap status code value
 * @return on success - 1
 *         on error   - 0
 */         

  int
coap_set_status_code(void *packet, uint8_t code)
{
  if (code <= 0xFF)
  {
    ((coap_packet_t *)packet)->code = (uint8_t) code;
    return 1;
  }
  else
  {
    return 0;
  }
}
/*-----------------------------------------------------------------------------------*/
/*- HEADER OPTION GETTERS AND SETTERS -----------------------------------------------*/
/*-----------------------------------------------------------------------------------*/
/* @fn    coap_get_header_content_type
 * @brief This function is used to get header content type 
 * @param packet - pointer to coap packet   
 * \retur on success - content type
 *        on error   - -1 
 */
int
coap_get_header_content_type(void *packet , int * content_type)
{
  coap_packet_t *const coap_pkt = (coap_packet_t *) packet;

  if (!IS_OPTION(coap_pkt, COAP_OPTION_CONTENT_TYPE)) return -1;

  *content_type = coap_pkt->content_type;

  return coap_pkt->content_type;
}

/* @fn     int coap_set_header_content_type() 
 * @brief  This function is used to set the coap header content type
 * @param  packet - pointer to coap packet 
 *         content_type - coap content type
 * @return return value - 1
 *        
 */
  int
coap_set_header_content_type(void *packet, unsigned int content_type)
{
  coap_packet_t *const coap_pkt = (coap_packet_t *) packet;

  coap_pkt->content_type = (coap_content_type_t) content_type;
  SET_OPTION(coap_pkt, COAP_OPTION_CONTENT_TYPE);
  return 1;
}
/*-----------------------------------------------------------------------------------*/

/* @fn     int coap_get_header_accept()
 * @brief  This function is used to get coap accept option from header
 * @param  packet  - pointer to coap packet 
 *         accept  - double pointer to output 
 * @return on success - accept num
 *         on error   - 0         
 */


  int
coap_get_header_accept(void *packet, const uint16_t **accept)
{
  coap_packet_t *const coap_pkt = (coap_packet_t *) packet;

  if (!IS_OPTION(coap_pkt, COAP_OPTION_ACCEPT)) return -1;

  *accept = coap_pkt->accept;
  return coap_pkt->accept_num;
}
/* @fn      coap_set_header_accept()
 * @brief   This function is used to set the coap header option accept  
 * @param   packet - pointer to coap packet 
 *          accept - accept value
 * @return  int accept_num           
 */
  int
coap_set_header_accept(void *packet, uint16_t accept)
{
  coap_packet_t *const coap_pkt = (coap_packet_t *) packet;

  if (coap_pkt->accept_num < COAP_MAX_ACCEPT_NUM)
  {
    coap_pkt->accept[coap_pkt->accept_num] = (coap_content_type_t)accept;
    coap_pkt->accept_num += 1;

    SET_OPTION(coap_pkt, COAP_OPTION_ACCEPT);
  }
  return coap_pkt->accept_num;
}
/*-----------------------------------------------------------------------------------*/
/* @fn      coap_get_header_max_age()
 * @brief   This function is used to get max age from header
 * @param   packet - pointer to coap packet 
 *          age    - pointer to the output
 * @return  return value - 1
 */
  int
coap_get_header_max_age(void *packet, uint32_t *age)
{
  coap_packet_t *const coap_pkt = (coap_packet_t *) packet;

  if (!IS_OPTION(coap_pkt, COAP_OPTION_MAX_AGE)) {
    *age = COAP_DEFAULT_MAX_AGE;
  } else {
    *age = coap_pkt->max_age;
  }
  return 1;
}
/* @fn      coap_set_header_max_age()
 * @brief   This function is used to set max age into coap header
 * @param   packet - pointer to coap packet 
 *          age    - max age value
 * @return  return value - 1
 */
  int
coap_set_header_max_age(void *packet, uint32_t age)
{
  coap_packet_t *const coap_pkt = (coap_packet_t *) packet;

  coap_pkt->max_age = age;
  SET_OPTION(coap_pkt, COAP_OPTION_MAX_AGE);
  return 1;
}
/*-----------------------------------------------------------------------------------*/
/* @fn     int coap_get_header_eta()
 * @brief  This function is used to get the etag from coap header
 * @param  packet - pointer to coap packet
 *         etag   - double pointer to output   
 * @return on error   - 0
 *         on success - etag length 
 */
  int
coap_get_header_etag(void *packet, const uint8_t **etag)
{
  coap_packet_t *const coap_pkt = (coap_packet_t *) packet;

  if (!IS_OPTION(coap_pkt, COAP_OPTION_ETAG)) return -1;

  *etag = coap_pkt->etag;
  return coap_pkt->etag_len;
}
/* @fn     int coap_set_header_eta()
 * @brief  This function is used to set the etag into coap header
 * @param  packet   - pointer to coap packet
 *         etag     - pointer to etag
 *         etag_len - length of etag    
 * @return return value - etag length 
 */

  int
coap_set_header_etag(void *packet, const uint8_t *etag, size_t etag_len)
{
  coap_packet_t *const coap_pkt = (coap_packet_t *) packet;

  coap_pkt->etag_len = (uint8_t)(MIN(COAP_ETAG_LEN, etag_len));
  COAP_MEMCPY(coap_pkt->etag, COAP_ETAG_LEN, etag, coap_pkt->etag_len);

  SET_OPTION(coap_pkt, COAP_OPTION_ETAG);
  return coap_pkt->etag_len;
}

/*-----------------------------------------------------------------------------------*/
/* @fn     int coap_get_header_token()
 * @brief  This function is used to get header token from coap header
 * @param  packet - pointer to coap packet
 *         token  - double pointer to token output 
 * @return on success  - token length
 *         on error    - 0         
 */
  int
coap_get_header_token(void *packet, const uint8_t **token)
{
  coap_packet_t *const coap_pkt = (coap_packet_t *) packet;

  if (!IS_OPTION(coap_pkt, COAP_OPTION_TOKEN)) return -1;

  *token = coap_pkt->token;
  return coap_pkt->token_len;
}
/* @fn     int coap_set_header_token()
 * @brief  This function is used to set token into coap header
 * @param  packet       - pointer to coap packet
 *         token        - pointer to token string
 *         token_len    - length of token  
 * @return return value - token length
 */

  int
coap_set_header_token(void *packet, const uint8_t *token, size_t token_len)
{
  coap_packet_t *const coap_pkt = (coap_packet_t *) packet;

  coap_pkt->token_len = (uint8_t)(MIN(COAP_TOKEN_LEN, token_len));
  COAP_MEMCPY(coap_pkt->token, COAP_TOKEN_LEN, token, coap_pkt->token_len);

  SET_OPTION(coap_pkt, COAP_OPTION_TOKEN);
  return coap_pkt->token_len;
}

/*-----------------------------------------------------------------------------------*/

/* @fn     int coap_get_header_uri_host()
 * @brief  This function is used to get  host uri from coap header
 * @param  packet     - pointer to coap packet
 *         host       - double pointer to host uri output   
 * @return on success - host uri length
 *         on error   - 0         
 */
  int
coap_get_header_uri_host(void *packet, const char **host)
{
  coap_packet_t *const coap_pkt = (coap_packet_t *) packet;

  if (!IS_OPTION(coap_pkt, COAP_OPTION_URI_HOST)) return -1;

  *host = (char *)coap_pkt->uri_host;
  return coap_pkt->uri_host_len;
}

/* @fn     int coap_set_header_uri_host()
 * @brief  This function is used to set  host uri into coap header
 * @param  packet       - pointer to coap packet
 *         host         - pointer to host ur   
 * @return return value - host uri  length        
 */
  int
coap_set_header_uri_host(void *packet, const char *host)
{
  coap_packet_t *const coap_pkt = (coap_packet_t *) packet;

  coap_pkt->uri_host = (uint8_t *)host;
  coap_pkt->uri_host_len = strlen(host);

  SET_OPTION(coap_pkt, COAP_OPTION_URI_HOST);
  return coap_pkt->uri_host_len;
}
/*-----------------------------------------------------------------------------------*/
/* @fn     int coap_get_header_uri_path()
 * @brief  This function is used to get  uri path from coap header
 * @param  packet     - pointer to coap packet
 *         host       - double pointer to  uri path  output   
 * @return on success - 0
 *         on error   - 0         
 */
  int
coap_get_header_uri_path(void *packet, char **path)
{
  coap_packet_t *const coap_pkt = (coap_packet_t *) packet;
  multi_option_t * option;
  int length = 0;

  if (!IS_OPTION(coap_pkt, COAP_OPTION_URI_PATH)) return -1;

  option = coap_pkt->uri_path;

  if(option == NULL) return -1;
 
  *path = coap_get_multi_option_as_string((void *)option , &length);

   if(*path == NULL)
   return -1;
   
   return length;

}
/* @fn     int coap_set_header_uri_path()
 * @brief  This function is used to set  uri path into coap header
 * @param  packet       - pointer to coap packet
 *         host         - pointer to  uri path string   
 * @return return value - length of uri path           
 */

  int
coap_set_header_uri_path(void *packet, const char *path)
{
  coap_packet_t *coap_pkt = (coap_packet_t *) packet;
  int length = 0;

  free_multi_option(coap_pkt->uri_path,NULL);
  coap_pkt->uri_path = NULL;

  if (path[0]=='/') ++path;

  do
  {
    int i = 0;

    while (path[i] != 0 && path[i] != '/') i++;
    coap_add_multi_option(&(coap_pkt->uri_path), (uint8_t *)path, i, 0,NULL);

    if (path[i] == '/') i++;
    path += i;
    length += i;
  } while (path[0] != 0);

  SET_OPTION(coap_pkt, COAP_OPTION_URI_PATH);
  return length;
}

/*-----------------------------------------------------------------------------------*/
/* @fn     int coap_get_header_uri_query()
 * @brief  This function is used to get  uri query from coap header
 * @param  packet       - pointer to coap packet
 *         query        - double pointer to query output   
 * @return return value - 0           
 */
  int
coap_get_header_uri_query(void *packet, char **query)
{
  coap_packet_t *const coap_pkt = (coap_packet_t *) packet;
  multi_option_t * option;
  int length = 0;

  if (!IS_OPTION(coap_pkt, COAP_OPTION_URI_QUERY)) return -1;
  option = coap_pkt->uri_query;

  if(option == NULL) return -1;
  
  *query = coap_get_multi_option_as_string_for_uri_query((void *)option , &length);

   if(*query == NULL)
    return -1;

   return length;

}
/* @fn     int coap_set_header_uri_query()
 * @brief  This function is used to set  uri query from coap header
 * @param  packet       - pointer to coap packet
 *         query        - pointer to uri query string   
 * @return return value - length of uri query           
 */

  int
coap_set_header_uri_query(void *packet, const char *query)
{
  int length = 0;
  coap_packet_t *const coap_pkt = (coap_packet_t *) packet;

  free_multi_option(coap_pkt->uri_query,NULL);
  coap_pkt->uri_query = NULL;

  if (query[0]=='?') ++query;

  do
  {
    int i = 0;

    while (query[i] != 0 && query[i] != '&') i++;
    coap_add_multi_option(&(coap_pkt->uri_query), (uint8_t *)query, i, 0,NULL);

    if (query[i] == '&') i++;
    query += i;
    length += i;
  } while (query[0] != 0);

  SET_OPTION(coap_pkt, COAP_OPTION_URI_QUERY);
  return length;
}
/*-----------------------------------------------------------------------------------*/
/* @fn     int coap_get_header_location_path()
 * @brief  This function is used to get location path from coap header
 * @param  packet       - pointer to coap packet
 *         query        - double pointer to loaction path  output   
 * @return return value - 0           
 */
int
coap_get_header_location_path(void *packet, char **path)
{
  coap_packet_t *const coap_pkt = (coap_packet_t *) packet;
  multi_option_t * option;
  int length = 0;

  if (!IS_OPTION(coap_pkt, COAP_OPTION_LOCATION_PATH)) return -1;

   option = coap_pkt->location_path;

   if(option == NULL) return -1;

   *path = coap_get_multi_option_as_string((void *)option , &length);

    if(*path == NULL)
		return -1;
	
    return length;
}
/* @fn     int coap_set_header_location_path()
 * @brief  This function is used to get location path from coap header
 * @param  packet       - pointer to coap packet
 *         query        - pointer to loaction path string   
 * @return return value - length of location path string           
 */

  int
coap_set_header_location_path(void *packet, const char *path)
{
  coap_packet_t *coap_pkt = (coap_packet_t *) packet;
  int length = 0;

  free_multi_option(coap_pkt->location_path,NULL);
  coap_pkt->location_path = NULL;

  if (path[0]=='/') ++path;

  do
  {
    int i = 0;

    while (path[i] != 0 && path[i] != '/') i++;
    coap_add_multi_option(&(coap_pkt->location_path), (uint8_t *)path, i, 0,NULL);

    if (path[i] == '/') i++;
    path += i;
    length += i;
  } while (path[0] != 0);

  SET_OPTION(coap_pkt, COAP_OPTION_LOCATION_PATH);
  return length;
}
/*-----------------------------------------------------------------------------------*/
/* @fn     int coap_get_header_location_query()
 * @brief  This function is used to get location query from coap header
 * @param  packet    - pointer to coap packet
 *         query     - double pointer to loaction query  output   
 * @return if  set   - location query length
 *          else     - 0             
 */
  int
coap_get_header_location_query(void *packet, const char **query)
{
  coap_packet_t *const coap_pkt = (coap_packet_t *) packet;

  if (!IS_OPTION(coap_pkt, COAP_OPTION_LOCATION_QUERY)) return -1;

  *query = (char *) coap_pkt->location_query;
  return coap_pkt->location_query_len;
}
/* @fn     int coap_set_header_location_query()
 * @brief  This function is used to set location into from coap header
 * @param  packet         - pointer to coap packet
 *         query          - pointer to loaction query     
 * @return return value   - location query length             
 */

  int
coap_set_header_location_query(void *packet, char *query)
{
  coap_packet_t *const coap_pkt = (coap_packet_t *) packet;

  while (query[0]=='?') ++query;

  coap_pkt->location_query = (uint8_t *)query;
  coap_pkt->location_query_len = strlen(query);

  SET_OPTION(coap_pkt, COAP_OPTION_LOCATION_QUERY);
  return coap_pkt->location_query_len;
}
/*-----------------------------------------------------------------------------------*/
/* @fn     int coap_get_header_observe()
 * @brief  This function is used to get observe  from coap header
 * @param  packet    - pointer to coap packet
 *         query     - pointer to observe output   
 * @return if  set   - 1
 *          else     - 0             
 */
  int 
coap_get_header_observe(void *packet, uint32_t *observe)
{
  coap_packet_t *const coap_pkt = (coap_packet_t *) packet;

  if (!IS_OPTION(coap_pkt, COAP_OPTION_OBSERVE)) return -1;

  *observe = coap_pkt->observe;
  return 1;
}
/* @fn     int coap_set_header_observe()
 * @brief  This function is used to set observe  into coap header
 * @param  packet    - pointer to coap packet
 *         query     - observe value   
 * @return return value   - 1            
 */

  int 
coap_set_header_observe(void *packet, uint32_t observe)
{
  coap_packet_t *const coap_pkt = (coap_packet_t *) packet;

  coap_pkt->observe = 0x00FFFFFF & observe;
  SET_OPTION(coap_pkt, COAP_OPTION_OBSERVE);
  return 1;
}
/*-----------------------------------------------------------------------------------*/

/* @fn     int coap_get_header_block2()
 * @brief  This function is used to get block 2 info from coap header
 * @param  packet    - pointer to coap packet
 *         num       -  pointer to block number output  
 *         more      -  pointer to block more info output 
 *         size      - pointer to block2 size output
 *         offset    - pointer to block2 offset output   
 * @return if  set   - 1
 *          else     - 0             
 */
  int
coap_get_header_block2(void *packet, uint32_t *num, uint8_t *more, uint16_t *size, uint32_t *offset)
{
  coap_packet_t *const coap_pkt = (coap_packet_t *) packet;

  if (!IS_OPTION(coap_pkt, COAP_OPTION_BLOCK2)) return -1;

  /* pointers may be NULL to get only specific block parameters */
  if (num!=NULL) *num = coap_pkt->block2_num;
  if (more!=NULL) *more = coap_pkt->block2_more;
  if (size!=NULL) *size = coap_pkt->block2_size;
  if (offset!=NULL) *offset = coap_pkt->block2_offset;

  return 1;
}
/* @fn     int coap_set_header_block2()
 * @brief  This function is used to set block 2 info into coap header
 * @param  packet    - pointer to coap packet
 *         num       -  block number  
 *         more      -  block more info  
 *         size      -  block2 size    
 * @return return value - 1             
 */
  int
coap_set_header_block2(void *packet, uint32_t num, uint8_t more, uint16_t size)
{
  coap_packet_t *const coap_pkt = (coap_packet_t *) packet;

  if (num>0x0FFFFF) return -1;
  if (!(IsPowerOfTwo(size)) || size < BLOCK_MIN_SIZE || size > BLOCK_MAX_SIZE)
  {
  	 return -1;
  }
  coap_pkt->block2_num = num;
  coap_pkt->block2_more = more ? 1 : 0;
  coap_pkt->block2_size = size;

  SET_OPTION(coap_pkt, COAP_OPTION_BLOCK2);
  return 1;
}
/*-----------------------------------------------------------------------------------*/
/* @fn     int coap_get_header_block1()
 * @brief  This function is used to get block 1 info from coap header
 * @param  packet    - pointer to coap packet
 *         num       -  pointer to block number output  
 *         more      -  pointer to block more info output 
 *         size      - pointer to block1 size output
 *         offset    - pointer to block1 offset output   
 * @return if  set   - 1
 *          else     - 0             
 */ 
  int
coap_get_header_block1(void *packet, uint32_t *num, uint8_t *more, uint16_t *size, uint32_t *offset)
{
  coap_packet_t *const coap_pkt = (coap_packet_t *) packet;

  if (!IS_OPTION(coap_pkt, COAP_OPTION_BLOCK1)) return -1;

  /* pointers may be NULL to get only specific block parameters */
  if (num!=NULL) *num = coap_pkt->block1_num;
  if (more!=NULL) *more = coap_pkt->block1_more;
  if (size!=NULL) *size = coap_pkt->block1_size;
  if (offset!=NULL) *offset = coap_pkt->block1_offset;

  return 1;
}
/* @fn     int coap_set_header_block1()
 * @brief  This function is used to set block 1 info into coap header
 * @param  packet    - pointer to coap packet
 *         num       - block number   
 *         more      - block more info  
 *         size      - block1 size     
 * @return on success  - 1
 *         on error    - 0             
 */ 

  int
coap_set_header_block1(void *packet, uint32_t num, uint8_t more, uint16_t size)
{
  coap_packet_t *const coap_pkt = (coap_packet_t *) packet;

  if (num>0x0FFFFF) return -1;
  if (!(IsPowerOfTwo(size)) || size < BLOCK_MIN_SIZE || size > BLOCK_MAX_SIZE)
  {
  	 return -1;
  }

  coap_pkt->block1_num = num;
  coap_pkt->block1_more = more;
  coap_pkt->block1_size = size;

  SET_OPTION(coap_pkt, COAP_OPTION_BLOCK1);
  return 1;
}
/*-----------------------------------------------------------------------------------*/
/* @fn      int coap_get_header_size()
 * @brief  This function is used to get header size from coap packet 
 * @param  packet - pointer to coap packet
 *         size   - pointer to size output 
 * \retun if set  - 1
 *        else    - 0 
 */
  int
coap_get_header_size(void *packet, uint32_t *size)
{
  coap_packet_t *const coap_pkt = (coap_packet_t *) packet;

  if (!IS_OPTION(coap_pkt, COAP_OPTION_SIZE)) return -1;

  *size = coap_pkt->size;
  return 1;
}
/* @fn      int coap_set_header_size()
 * @brief  This function is used to set header size info into  coap packet 
 * @param  packet       - pointer to coap packet
 *         size         - size of coap header 
 * \retun  return value - 1
 */

  int
coap_set_header_size(void *packet, uint32_t size)
{
  coap_packet_t *const coap_pkt = (coap_packet_t *) packet;

  coap_pkt->size = size;
  SET_OPTION(coap_pkt, COAP_OPTION_SIZE);
  return 1;
}

/*-----------------------------------------------------------------------------------*/
/* @fn      int coap_get_header_size1()
 * @brief  This function is used to get size1 from coap packet 
 * @param  packet - pointer to coap packet
 *         size   - pointer to size output 
 * \retun if set  - 1
 *        else    - 0 
 */
  int
coap_get_header_size1(void *packet, uint32_t *size)
{
  coap_packet_t *const coap_pkt = (coap_packet_t *) packet;

  if (!IS_OPTION(coap_pkt, COAP_OPTION_SIZE1)) return -1;

  *size = coap_pkt->size1;
  return 1;
}

/* @fn      int coap_set_header_size1()
 * @brief  This function is used to set size1 info into  coap packet 
 * @param  packet       - pointer to coap packet
 *         size         - size of coap header 
 * \retun  return value - 1
 */

  int
coap_set_header_size1(void *packet, uint32_t size)
{
  coap_packet_t *const coap_pkt = (coap_packet_t *) packet;

  coap_pkt->size1 = size;
  SET_OPTION(coap_pkt, COAP_OPTION_SIZE1);
  return 1;
}

/* @fn     int coap_set_payload()
 * @brief  This function is used to set the payload into coap packet
 * @param  packet  - pointer to coap packet
 *         payload - pointer topayload to be filled 
 *         length  - length of payload
 * @return - payload length      
 */
  int
coap_set_payload(void *packet, const void *payload, size_t length)
{
  coap_packet_t *const coap_pkt = (coap_packet_t *) packet;

  coap_pkt->payload = (uint8_t *) payload;
  coap_pkt->payload_len = (uint16_t)(length);

  return coap_pkt->payload_len;
}

void coap_set_option(coap_packet_ext_t *packet, uint16_t option_number,
                            void *umem)
{
  coap_opt_num_t *opt = NULL;

  if(coap_is_standard_option(option_number))
  {
     if(COAP_LIST_FIND(packet->coap_ext_opt.options, option_number) != NULL)
     {
        return;
     }
  }

  opt = (coap_opt_num_t *)mem_utility_Get_Memory_From_Pool(umem, sizeof(coap_opt_num_t));

  if(opt == NULL)
  {
      return;
  }

  memset(opt, 0x00, sizeof(coap_opt_num_t));
  opt->opt_num = option_number;

  packet->coap_ext_opt.options = (coap_opt_num_t *)COAP_LIST_ADD(packet->coap_ext_opt.options, opt);
  packet->coap_ext_opt.option_count++;

  return;
}

int
coap_set_header_ext(void *packet, uint16_t option_num, const char *val,
                            uint16_t len, void * umem)
{
  coap_packet_ext_t *coap_pkt = (coap_packet_ext_t *) packet;
  coap_ext_msg_option_t *ext_opt = NULL;

  ext_opt = (coap_ext_msg_option_t *)mem_utility_Get_Memory_From_Pool(umem, sizeof(coap_ext_msg_option_t));

  if(ext_opt)
  {
    memset(ext_opt, 0x00, sizeof(coap_ext_msg_option_t));
    ext_opt->next = NULL;
    ext_opt->len =  len;
    ext_opt->option_num = option_num;

	ext_opt->val = (uint8_t *)mem_utility_Get_Memory_From_Pool(umem, len);

    if(ext_opt->val == NULL)
    {
      mem_utility_release_memory(umem!=NULL, ext_opt);
      return -1;
    }
    memset(ext_opt->val,0,len);
    COAP_MEMCPY(ext_opt->val, len, val, len);

    coap_pkt->coap_ext_opt.ext_option = (coap_ext_msg_option_t *)COAP_LIST_ADD(coap_pkt->coap_ext_opt.ext_option,
                                                          ext_opt);

    coap_pkt->coap_ext_opt.ext_option_count++;
    return 0;
  }
  return -1;
}

int coap_fill_ext_header_info(coap_ext_msg_option_t **option,
                                         coap_ext_msg_option_t *ext_option)
{
  coap_ext_msg_option_t *add_option = NULL;

    add_option = (coap_ext_msg_option_t *)malloc(sizeof(coap_ext_msg_option_t));

  if(add_option != NULL)
  {
    memset(add_option, 0x00, sizeof(coap_ext_msg_option_t));
    add_option->next = NULL;
    add_option->len = ext_option->len;
    add_option->option_num = ext_option->option_num;

      add_option->val = (uint8_t *)malloc(ext_option->len);

    if(add_option->val == NULL)
    {
      free(add_option);
      return -1;
    }
    memset(add_option->val,0x00,ext_option->len);
    COAP_MEMCPY(add_option->val, ext_option->len, ext_option->val, ext_option->len);

    *option = (coap_ext_msg_option_t *)COAP_LIST_ADD(*option,
                            add_option);

    return 0;
  }
  return -1;
}

int
coap_get_header_ext(void *packet, coap_ext_msg_option_t **option , uint16_t opt_num)
{
  coap_packet_ext_t *const coap_pkt = (coap_packet_ext_t *) packet;
  coap_ext_msg_option_t *ext_option = NULL;
  int ret = -1;

  *option = NULL;
  ext_option = coap_pkt->coap_ext_opt.ext_option;

  while(ext_option)
  {
    if(ext_option->option_num == opt_num)
    {
      ret = coap_fill_ext_header_info(option, ext_option);
      if(ret == -1)
        goto end;
    }
    ext_option = ext_option->next;
  }

end:

  if(ret != 0)
  {
    free_ext_msg_option(*option, NULL);
  }
  return ret;
}

int coap_get_all_header_ext(void *packet, coap_ext_msg_option_t **option)
{
  coap_packet_ext_t *const coap_pkt = (coap_packet_ext_t *) packet;
  coap_ext_msg_option_t *ext_option = NULL;
  int ret = -1;

  *option = NULL;
  ext_option = coap_pkt->coap_ext_opt.ext_option;

  while(ext_option)
  {
    ret = coap_fill_ext_header_info(option, ext_option);
    if(ret == -1)
      goto end;

    ext_option = ext_option->next;
  }

end:

  if(ret != 0)
  {
    free_ext_msg_option(*option, NULL);
  }
  return ret;
}

boolean coap_is_standard_option(uint16_t opt)
{
    
 if(opt ==  COAP_OPTION_URI_HOST ||  opt ==  COAP_OPTION_ETAG  ||  opt ==  COAP_OPTION_OBSERVE  || 
     opt ==  COAP_OPTION_CONTENT_TYPE ||  opt ==  COAP_OPTION_MAX_AGE  ||  opt ==  COAP_OPTION_ACCEPT  || 
     opt ==  COAP_OPTION_LOCATION_QUERY ||  opt ==  COAP_OPTION_BLOCK2  ||  opt ==  COAP_OPTION_BLOCK1 || 
     opt ==  COAP_OPTION_SIZE  || opt == COAP_OPTION_IF_MATCH || opt == COAP_OPTION_IF_NONE_MATCH || 
     opt == COAP_OPTION_URI_PORT || opt == COAP_OPTION_LOCATION_PATH || opt == COAP_OPTION_URI_PATH  ||
     opt == COAP_OPTION_URI_QUERY || opt == COAP_OPTION_TOKEN || opt == COAP_OPTION_PROXY_URI ||
     opt == COAP_OPTION_SIZE1)
  {
     return true;
  }
  return false;
}

/*-----------------------------------------------------------------------------------*/
