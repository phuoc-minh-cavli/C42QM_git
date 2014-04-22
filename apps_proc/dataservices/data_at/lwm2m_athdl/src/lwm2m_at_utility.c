/******************************************************************************

                        Lwm2m_at_utility . C 

******************************************************************************/

/******************************************************************************

  @file    lwm2m_at_utility.c
  @brief   lwm2m at utility

  DESCRIPTION
  Utility functions support the handler of QCT native lwM2M AT commands

  ---------------------------------------------------------------------------
  Copyright (c) 2017, 2018, 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------

******************************************************************************/
#ifdef LWM2M_AT_ENABLE
/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "lwm2m_at_utility.h"
#include "lwm2m_id_mapping.h"

/*===========================================================================

                           Global Variables

===========================================================================*/
extern util_at_list_t * lwm2m_at_req_store_hdr;
extern int utils_plainTextToFloat64(uint8_t * buffer, int length, double * dataP);
extern size_t utils_float64ToPlainText(double data, uint8_t ** bufferP);


/*===========================================================================

                           Utility Functions

===========================================================================*/
/**
 * @fn boolean data_atfwd_athdl_buffer_extend()
 * @brief extend string buffer
 * @param
 *     @param[in/out] src_buffer
 *     @param[in/out] src_len
 *     @param[in] add_buffer
 *     @param[in] add_len
 *     @param[in] buf_type
 * @return TRUE/FALSE
 */
boolean lwm2m_util_buffer_extend(uint8 ** src_buffer,size_t * src_len, uint8 * add_buffer, size_t add_len, uint8 buf_type)
{
  uint8 * tmp = NULL;
  boolean result = FALSE;
  ATFWD_LOG_MSG_MED("buffer_extend() in", 0, 0, 0);

  if(src_buffer == NULL || *src_buffer == NULL || src_len == NULL || *src_len == 0 || add_buffer == NULL || add_len == 0 ||
    (buf_type != QAPI_NET_LWM2M_TYPE_STRING_E && buf_type != QAPI_NET_LWM2M_TYPE_OPAQUE_E))
  {
    ATFWD_LOG_MSG_LOW("input parameter error", 0, 0, 0);
    goto end;
  }
  if(buf_type == QAPI_NET_LWM2M_TYPE_STRING_E)
  {
    tmp = athdl_malloc(*src_len+add_len+1);
    if(tmp == NULL)
    {
      goto end;
    }
    *src_len += add_len;
    snprintf((char *)tmp, *src_len+1, "%s%s", (char *)(*src_buffer), (char *)add_buffer);
  }
  else
  {
    size_t tmp_len = 0;
    tmp = athdl_malloc(*src_len+add_len);
    if(tmp == NULL)
    {
      goto end;
    }
    tmp_len += memscpy(tmp, *src_len+add_len, *src_buffer, *src_len); //copy src buffer
    tmp_len += memscpy(tmp+tmp_len,*src_len+add_len-tmp_len, add_buffer, add_len); //copy add buffer
    *src_len = tmp_len;
  }
  athdl_free(*src_buffer);
  *src_buffer = tmp;
  result = true;
end:
  ATFWD_LOG_MSG_MED("buffer_extend() out, bool res %d", result, 0, 0);
  return result;
}
/**
 * @fn boolean lwm2m_util_pack_buffer()
 * @brief pack input string into buffer
 * @param
 *     @param[in/out] target_buf
 *     @param[in/out] target_len
 *     @param[in] input_buf
 *     @param[in] input_len
 *     @param[in] buffer_type
 * @return TRUE/FALSE
 */
boolean lwm2m_util_pack_buffer(uint8 ** target_buf,size_t * target_len, uint8 * input_buf, size_t input_len, uint8 buf_type)
{
  size_t tmp_index = 0;
  uint8_t input_str[5];
  int32 input_value = 0;
  boolean result = FALSE;
  ATFWD_LOG_MSG_MED("pack_buffer() in, with input_len %d", input_len, 0, 0);

  if(target_buf == NULL || input_buf == NULL || target_len == NULL || input_len == 0 ||
     (buf_type != QAPI_NET_LWM2M_TYPE_STRING_E && buf_type != QAPI_NET_LWM2M_TYPE_OPAQUE_E))
  {
    ATFWD_LOG_MSG_LOW("input parameter error", 0, 0, 0);
    goto end;
  }
  if(buf_type == QAPI_NET_LWM2M_TYPE_STRING_E)
  {
    *target_buf = athdl_malloc(input_len);
    if(*target_buf == NULL)
    {
      ATFWD_LOG_MSG_LOW("pack buffer memory error", 0, 0, 0);
      goto end;
    }
    strlcpy((char *)(*target_buf), (char *)(input_buf),input_len);
    *target_len = input_len;
  }
  else
  {
    if(input_len <= 3 || ((input_len-1) % 2) != 0 || !(input_buf[0] == '0' && input_buf[1] == 'x'))
    {
      ATFWD_LOG_MSG_LOW("input parameter error", 0, 0, 0);
      goto end;
    }

    memset(input_str, 0x00, 5);
    input_str[0] = '0'; input_str[1] = 'x';

    *target_buf = athdl_malloc((input_len-3) / 2);
    if(*target_buf == NULL)
    {
      ATFWD_LOG_MSG_LOW("pack buffer memory error", 0, 0, 0);
      goto end;
    }

    for(tmp_index = 2; tmp_index < (input_len - 1); tmp_index += 2)
    {
      input_str[2] = input_buf[tmp_index];
      input_str[3] = input_buf[tmp_index+1];
      if(FALSE == util_at_string_to_Integer(input_str, &input_value) || input_value > 255 || input_value < 0)
      {
        ATFWD_LOG_MSG_LOW("parse integer fail", 0, 0, 0);
        goto end;
      }
      (*target_buf)[tmp_index / 2 - 1] = (uint8)input_value;
    }
    *target_len = (input_len - 3) / 2;
    ATFWD_LOG_MSG_LOW("target buffer len is %d",*target_len, 0, 0);
  }

  result = true;
end:
  if(result != true && target_buf != NULL)
  {
    athdl_free(*target_buf);
  }
  ATFWD_LOG_MSG_MED("pack_buffer() out, bool res %d, target_len %d, buf_type %d", result, *target_len, buf_type);
  return result;
}
/**
 * @fn boolean lwm2m_util_unpack_buffer()
 * @brief pack input string into buffer
 * @param
 *     @param[in/out] target_buf
 *     @param[in/out] target_len
 *     @param[in] input_buf
 *     @param[in] input_len
 *     @param[in] buffer_type
 * @return TRUE/FALSE
 */
boolean lwm2m_util_unpack_buffer(uint8 ** target_buf,size_t * target_len, uint8 * input_buf, size_t input_len, uint8 buf_type)
{
  size_t tmp_index = 0;

  boolean result = FALSE;
  ATFWD_LOG_MSG_MED("unpack_buffer() in, with input_len %d", input_len, 0, 0);

  if(target_buf == NULL || target_len == NULL || (input_buf != NULL && input_len == 0) || (input_buf == NULL && input_len != 0) ||
     (buf_type != QAPI_NET_LWM2M_TYPE_STRING_E && buf_type != QAPI_NET_LWM2M_TYPE_OPAQUE_E))
  {
    ATFWD_LOG_MSG_LOW("input parameter error", 0, 0, 0);
    goto end;
  }
  if(input_buf == NULL && input_len == 0)
  {
    *target_buf = athdl_malloc(1);
    if(*target_buf == NULL)
    {
      ATFWD_LOG_MSG_LOW("pack buffer memory error", 0, 0, 0);
      goto end;
    }
  }
  else
  {
    if(buf_type == QAPI_NET_LWM2M_TYPE_STRING_E)
    {
      *target_buf = athdl_malloc(input_len+1);
      if(*target_buf == NULL)
      {
        ATFWD_LOG_MSG_LOW("pack buffer memory error", 0, 0, 0);
        goto end;
      }
      strlcpy((char *)(*target_buf), (char *)(input_buf),input_len+1);
      *target_len = input_len+1;
      ATFWD_LOG_MSG_LOW_SPRINT("unpack str %s, len %d", *target_buf, *target_len, 0);
    }
    else
    {
      *target_buf = athdl_malloc(input_len*2 + 3);
      if(*target_buf == NULL)
      {
        ATFWD_LOG_MSG_LOW("pack buffer memory error", 0, 0, 0);
        goto end;
      }
      *target_len += snprintf((char *)((*target_buf)+(*target_len)), input_len*2 + 3 - *target_len, "0x");
      for(tmp_index = 0; tmp_index < input_len; tmp_index++)
      {
         ATFWD_LOG_MSG_LOW("unpack buffer index %d is %x, target_len is %d", tmp_index, input_buf[tmp_index], *target_len);
        *target_len += snprintf((char *)((*target_buf)+(*target_len)), input_len*2 + 3 - *target_len, "%02x", input_buf[tmp_index]);
      }
      (*target_buf)[*target_len] = '\0';
      (*target_len) ++;
    }
  }
  result = true;
end:
  if(result != true && target_buf != NULL)
  {
    athdl_free(*target_buf);
  }
  ATFWD_LOG_MSG_MED("unpack_buffer() out, bool res %d, target_len %d, buf_type %d", result, *target_len, buf_type);
  return result;
}

/**
 * @fn boolean lwm2m_util_string_to_objlink()
 * @brief this function convert string into objlink
 * @param 
 *     @param[in] String_Buffer pointer to beginning of string
 *     @param[out] objlink_t *objlink_Value
 * @return TRUE on success FALSE on failure
 */
boolean lwm2m_util_string_to_objlink(const uint8 *String_Buffer, objlink_t *objlink_Value)
{
  int string_len = 0;
  int separate_offset = 0;
  int i = 0;
  uint8 * tmp = NULL;
  int32 int_tmp = 0;
  uint16 obj_id = 0;
  uint16 ins_id = 0;
  boolean result = FALSE;

  ATFWD_LOG_MSG_MED("string_to_objlink() in", 0, 0, 0);

  string_len = strlen((char *)String_Buffer);

  if(string_len > UTIL_AT_STR_LEN_MAX|| string_len <= 2 || String_Buffer[0]!= '['|| String_Buffer[string_len -1] != ']')
  {/*Object link should be like [num1:num2]*/
    ATFWD_LOG_MSG_LOW("String_Buffer error", 0, 0, 0);
    goto end;
  }
  for(i = 1; i < string_len-1; i++)
  {
    if(String_Buffer[i] == ':')
    {
      separate_offset = i;
    }
  }
  if(separate_offset == 0)
  {
    ATFWD_LOG_MSG_LOW("error, no :", 0, 0, 0);
    goto end;
  }

  tmp = athdl_malloc(LWM2M_AT_STR_MAX_LEN);
  if(tmp == NULL)
  {
    ATFWD_LOG_MSG_LOW("malloc failed", 0, 0, 0);
    goto end;
  }

  strlcpy((char *)tmp, (char *)(&String_Buffer[1]),separate_offset);
  if(FALSE == util_at_string_to_Integer(tmp, &int_tmp) )
  {
    ATFWD_LOG_MSG_LOW("obj_id not integer", 0, 0, 0);
    goto end;
  }
  obj_id = (uint16)int_tmp;

  strlcpy((char *)tmp, (char *)(&String_Buffer[separate_offset+1]), string_len - separate_offset - 1);
  if(FALSE == util_at_string_to_Integer(tmp, &int_tmp) )
  {
    ATFWD_LOG_MSG_LOW("inst_id not integer", 0, 0, 0);
    goto end;
  }
  ins_id = (uint16)int_tmp;

  *objlink_Value = (objlink_t)obj_id;
  *objlink_Value = *objlink_Value << 16;
  *objlink_Value |= (objlink_t)ins_id;
  result = TRUE;

end:
  ATFWD_LOG_MSG_MED("string_to_objlink() out, bool res %d", (int)result, 0, 0);
  athdl_free(tmp);
  return result;
}
/**
 * @fn void lwm2m_util_parse_value()
 * @brief this function parse input <value> into qapi_Net_LWM2M_Flat_Data_t
 * @param 
 *     @param[in] a_ptr pointer to beginning of input buffer
 *     @param[in] value type
 *     @param[out] Info_res pointer to qapi_Net_LWM2M_Flat_Data_t
 * @return TRUE on success FALSE on failure
 */
boolean lwm2m_util_parse_value(uint8 * a_ptr, uint8 value_type, qapi_Net_LWM2M_Flat_Data_t * data)
{
  int32 input_value = 0;
  int string_len = 0;
  boolean result = FALSE;

  ATFWD_LOG_MSG_MED("parse_value() in", 0, 0, 0);

  if(a_ptr == NULL || data == NULL)
  {
    ATFWD_LOG_MSG_LOW("input error", 0, 0, 0);
    goto end;
  }
  data->id = 0;
  data->type = (qapi_Net_LWM2M_Value_Type_t)value_type;

  switch(value_type)
  {
    case QAPI_NET_LWM2M_TYPE_INTEGER_E:
    {
      if(FALSE == util_at_string_to_Integer(a_ptr, &input_value))
      {
        ATFWD_LOG_MSG_LOW("parse integer fail", 0, 0, 0);
        goto end;
      }
      data->value.asInteger = (int64_t)input_value;
      ATFWD_LOG_MSG_LOW("parse integer as %d", (int)data->value.asInteger, 0, 0);
      result = TRUE;
      goto end;
    }
    case QAPI_NET_LWM2M_TYPE_FLOAT_E:
    {
      if(FALSE == util_at_string_to_Double(a_ptr, &(data->value.asFloat)))
      {
        ATFWD_LOG_MSG_LOW("parse float fail", 0, 0, 0);
        goto end;
      }
      ATFWD_LOG_MSG_LOW("parse float as %d.%d", (int)data->value.asFloat, (int)((data->value.asFloat - (int)data->value.asFloat)*1000), 0);
      result = TRUE;
      goto end;
    }
    case QAPI_NET_LWM2M_TYPE_BOOLEAN_E:
    {
      if(FALSE == util_at_string_to_Integer(a_ptr, &input_value) || (input_value != 0 && input_value != 1))
      {
        ATFWD_LOG_MSG_LOW("parse boolean fail", 0, 0, 0);
        goto end;
      }
      data->value.asBoolean = (input_value == 0? FALSE: TRUE);
      ATFWD_LOG_MSG_LOW("parse boolean as %d", (int)data->value.asBoolean, 0, 0);
      result = TRUE;
      goto end;
    }
    case QAPI_NET_LWM2M_TYPE_STRING_E:
    case QAPI_NET_LWM2M_TYPE_OPAQUE_E:
    {
      if(data->value.asBuffer.buffer != NULL)
      {
        ATFWD_LOG_MSG_LOW("parse string err", 0, 0, 0);
        goto end;
      }
      string_len = strlen((char *)(a_ptr));
      if(string_len >UTIL_AT_STR_LEN_MAX|| string_len <= 2 || a_ptr[0]!= '"'|| a_ptr[string_len -1] != '"')
      {
        ATFWD_LOG_MSG_LOW("parse string length error", 0, 0, 0);
        goto end;
      }
      a_ptr[string_len - 1] = '\0';
      if(FALSE == lwm2m_util_pack_buffer(&(data->value.asBuffer.buffer),&(data->value.asBuffer.length),&a_ptr[1],string_len - 1,value_type))
      {
        ATFWD_LOG_MSG_LOW("pack buffer error", 0, 0, 0);
        goto end;
      }
      result = TRUE;
      goto end;
    }
    case QAPI_NET_LWM2M_TYPE_OBJECT_LINK:
    {
      if(FALSE == lwm2m_util_string_to_objlink(a_ptr, &(data->value.asObjLink)))
      {
        ATFWD_LOG_MSG_LOW("parse ObjLink fail", 0, 0, 0);
        goto end;
      }
      ATFWD_LOG_MSG_LOW("parse Objlink as %d", (int)data->value.asObjLink, 0, 0);
      result = TRUE;
      goto end;
    }
    default:
      goto end;
  }
end:
  ATFWD_LOG_MSG_MED("parse_value() out, bool res %d, value_type %d", (int)result, (int)value_type, 0);
  return result;
}

/**
 * @fn void lwm2m_util_parse_uri_str()
 * @brief this function parse uri string into qapi_Net_LWM2M_Obj_Info_t
 * @param 
 *     @param[in] a_ptr pointer to beginning of string
 *     @param[out] Info_res pointer to qapi_Net_LWM2M_Obj_Info_t
 * @return TRUE on success FALSE on failure
 */
boolean lwm2m_util_parse_uri_str(char * a_ptr, qapi_Net_LWM2M_Obj_Info_t * id_info)
{
  char * token = NULL;
  char *tmp_ptr = NULL;
  int order = 0;
  boolean result = FALSE;

  int32 tmp_num = 0;
  ATFWD_LOG_MSG_MED("parse_uri_str() in", 0, 0, 0);
  if(a_ptr == NULL || id_info == NULL )
  {
    ATFWD_LOG_MSG_LOW("invalid input",0,0,0);
    goto end; //result as False
  }
  memset(id_info, 0x00, sizeof(qapi_Net_LWM2M_Obj_Info_t));
  tmp_ptr = a_ptr;
  token = strtok_r(tmp_ptr, "/", &tmp_ptr);

  while (token) /* walk through tokens */
  {
    order++;
    if(FALSE == util_at_string_to_Integer((uint8_t *)token, &tmp_num) || tmp_num < 0)
    {
      ATFWD_LOG_MSG_LOW("input invalid",0,0,0);
      goto end; //result as False
    }
    else
    {
      //ToDo check tmp_num
      if(order == 1)
      {
        id_info->obj_id = (uint16_t)tmp_num;
        id_info->obj_mask |= QAPI_NET_LWM2M_OBJECT_ID_E;
        ATFWD_LOG_MSG_LOW("id_info->obj_id %d",id_info->obj_id,0,0);
      }
      else if(order == 2)
      {
        id_info->obj_inst_id = (uint16_t)tmp_num;
        id_info->obj_mask |= QAPI_NET_LWM2M_INSTANCE_ID_E;
        ATFWD_LOG_MSG_LOW("id_info->obj_inst_id %d",id_info->obj_inst_id,0,0);
      }
      else if(order == 3)
      {
        id_info->res_id = (uint16_t)tmp_num;
        id_info->obj_mask |= QAPI_NET_LWM2M_RESOURCE_ID_E;
        ATFWD_LOG_MSG_LOW("id_info->res_id %d",id_info->res_id,0,0);
      }
      else if(order == 4)
      {
        id_info->res_inst_id = (uint16_t)tmp_num;
        id_info->obj_mask |= QAPI_NET_LWM2M_RESOURCE_INSTANCE_ID_E;
        ATFWD_LOG_MSG_LOW("id_info->res_inst_id %d",id_info->res_inst_id,0,0);
      }
      else
      {
        ATFWD_LOG_MSG_LOW("too much order",0,0,0);
        goto end;
      }
    }
    token = strtok_r(tmp_ptr, "/", &tmp_ptr);
  }

  result = TRUE;

end:
  ATFWD_LOG_MSG_MED("parse_uri_str() out, bool res %d", (int)result, 0, 0);
  return result;
}
/**
 * @fn void lwm2m_util_parse_attr()
 * @brief this function parse attr string into qapi_Net_LWM2M_Attributes_t
 * @param
 *     @param[in] a_ptr pointer to beginning of string
 *     @param[out] Info_res pointer to qapi_Net_LWM2M_Attributes_t
 * @return TRUE on success FALSE on failure
 */
boolean lwm2m_util_parse_attr(uint8 * a_ptr, qapi_Net_LWM2M_Attributes_t * attr_info)
{
  char * token = NULL;
  char * input_ptr = NULL;
  char *tmp_ptr = NULL;
  int32 tmp_num = 0;

  char * local_tmp_ptr = NULL;
  char * local_token = NULL;
  boolean result = FALSE;

  ATFWD_LOG_MSG_MED("parse_attr() in", 0, 0, 0);

  if(a_ptr == NULL || attr_info == NULL )
  {
    ATFWD_LOG_MSG_LOW("invalid input",0,0,0);
    goto end;
  }

  //Save attr string
  input_ptr = util_at_save_str(a_ptr);
  if(input_ptr == NULL)
  {
    ATFWD_LOG_MSG_LOW("save input_ptr error", 0, 0, 0);
    goto end;
  }

  //Decode attr string
  tmp_ptr = input_ptr;
  token = strtok_r(tmp_ptr, ";", &tmp_ptr);
  while(token) /* walk through tokens */
  {
    local_tmp_ptr = token;
    local_token = NULL;

    local_token = strtok_r(local_tmp_ptr, "=", &local_tmp_ptr);
    if(local_token == NULL || local_tmp_ptr == NULL)
    {
      ATFWD_LOG_MSG_LOW("parse token error", 0, 0, 0);
      goto end;
    }
    if(!strncasecmp(local_token, "dim", strlen(local_token)))
    {
      if((FALSE == util_at_string_to_Integer((uint8 *)local_tmp_ptr, &tmp_num))||(tmp_num < 0))
      {
        ATFWD_LOG_MSG_LOW("parse dim fail", 0, 0, 0);
        goto end;
      }
      attr_info->dim = (uint8)tmp_num;
      attr_info->set_attr_mask |= QAPI_NET_LWM2M_DIM_E;
    }
    else if(!strncasecmp(local_token, "pmin", strlen(local_token)))
    {
      if((FALSE == util_at_string_to_Integer((uint8 *)local_tmp_ptr, &tmp_num))||(tmp_num < 0))
      {
        ATFWD_LOG_MSG_LOW("parse pmin fail", 0, 0, 0);
        goto end;
      }
      attr_info->minPeriod = (uint32)tmp_num;
      attr_info->set_attr_mask |= QAPI_NET_LWM2M_MIN_PERIOD_E;
    }
    else if(!strncasecmp(local_token, "pmax", strlen(local_token)))
    {
      if((FALSE == util_at_string_to_Integer((uint8 *)local_tmp_ptr, &tmp_num))||(tmp_num < 0))
      {
        ATFWD_LOG_MSG_LOW("parse pmax fail", 0, 0, 0);
        goto end;
      }
      attr_info->maxPeriod = (uint32)tmp_num;
      attr_info->set_attr_mask |= QAPI_NET_LWM2M_MAX_PERIOD_E;
    }
    else if(!strncasecmp(local_token, "gt", strlen(local_token)))
    {
      if(FALSE == util_at_string_to_Double((uint8 *)local_tmp_ptr, &(attr_info->greaterThan)))
      {
        ATFWD_LOG_MSG_LOW("parse gt fail", 0, 0, 0);
        goto end;
      }
      attr_info->set_attr_mask |= QAPI_NET_LWM2M_GREATER_THAN_E;
    }
    else if(!strncasecmp(local_token, "lt", strlen(local_token)))
    {
      if(FALSE == util_at_string_to_Double((uint8 *)local_tmp_ptr, &(attr_info->lessThan)))
      {
        ATFWD_LOG_MSG_LOW("parse lt fail", 0, 0, 0);
        goto end;
      }
      attr_info->set_attr_mask |= QAPI_NET_LWM2M_LESS_THAN_E;
    }
    else if(!strncasecmp(local_token, "st", strlen(local_token)))
    {
      if(FALSE == util_at_string_to_Double((uint8 *)local_tmp_ptr, &(attr_info->step)))
      {
        ATFWD_LOG_MSG_LOW("parse st fail", 0, 0, 0);
        goto end;
      }
      attr_info->set_attr_mask |= QAPI_NET_LWM2M_STEP_E;
    }
    else
    {
      ATFWD_LOG_MSG_LOW("attr not support", 0, 0, 0);
      goto end;
    }
    token = strtok_r(tmp_ptr, ";", &tmp_ptr);
  }
  result = TRUE;
end:
  ATFWD_LOG_MSG_MED("parse_attr() out, bool res %d, attr mask %d", result, ((attr_info == NULL)? -1 : attr_info->set_attr_mask), 0);
  athdl_free(input_ptr);
  return result;
}

/**
 * @fn void lwm2m_util_resp_code_check()
 * @brief generate check response code validity
 * @param qapi_Net_LWM2M_Response_Code_t input_code
 * @return TRUE/FALSE
 */
boolean lwm2m_util_resp_code_check(int32 input_code)
{
  switch(input_code)
  {
    case QAPI_NET_LWM2M_IGNORE_E:
    case QAPI_NET_LWM2M_201_CREATED_E:
    case QAPI_NET_LWM2M_202_DELETED_E:
    case QAPI_NET_LWM2M_204_CHANGED_E:
    case QAPI_NET_LWM2M_205_CONTENT_E:
    case QAPI_NET_LWM2M_400_BAD_REQUEST_E:
    case QAPI_NET_LWM2M_401_UNAUTHORIZED_E:
    case QAPI_NET_LWM2M_402_BAD_OPTION_E:
    case QAPI_NET_LWM2M_403_FORBIDDEN_E:
    case QAPI_NET_LWM2M_404_NOT_FOUND_E:
    case QAPI_NET_LWM2M_405_METHOD_NOT_ALLOWED_E:
    case QAPI_NET_LWM2M_406_NOT_ACCEPTABLE_E:
    case QAPI_NET_LWM2M_408_ENTITY_INCOMPLETE_E:
    case QAPI_NET_LWM2M_413_ENTITY_TOO_LARGE_E:
    case QAPI_NET_LWM2M_415_UNSUPPORTED_DATA_FORMAT_E:
    case QAPI_NET_LWM2M_500_INTERNAL_SERVER_E:
    {
      return TRUE;
    }
    default:
      return FALSE;
  }
}
/**
 * @fn void lwm2m_util_resource_type_check()
 * @brief generate check resource type validity
 * @param
 *     @param[in] qapi_Net_LWM2M_Obj_Info_t * id_info
 *     @param[in] uint8 type
 * @return TRUE/FALSE
 */
boolean lwm2m_util_resource_type_check(qapi_Net_LWM2M_Obj_Info_t * id_info, uint8 type)
{
  boolean result = FALSE;
  ATFWD_LOG_MSG_MED("resource_type_check() in", 0, 0, 0);

  if(id_info == NULL ||
    (!(id_info->obj_mask & QAPI_NET_LWM2M_OBJECT_ID_E) || !(id_info->obj_mask & QAPI_NET_LWM2M_RESOURCE_ID_E))
    )
  {
    ATFWD_LOG_MSG_LOW("invalid input", 0, 0, 0);
    goto end;
  }

  switch(id_info->obj_id)
  {
    case QAPI_NET_LWM2M_SECURITY_OBJECT_ID_E:
    {
      switch(id_info->res_id)
      {
        case QAPI_NET_LWM2M_SECURITY_RES_M_SERVER_URI_E:
        case QAPI_NET_LWM2M_SECURITY_RES_O_SMS_SERVER_NUMBER_E:
          result = (type == QAPI_NET_LWM2M_TYPE_STRING_E);
          goto end;
        case QAPI_NET_LWM2M_SECURITY_RES_M_SECURITY_MODE_E:
        case QAPI_NET_LWM2M_SECURITY_RES_O_SHORT_SERVER_ID_E:
        case QAPI_NET_LWM2M_SECURITY_RES_O_HOLDOFF_TIME_E:
          result = (type == QAPI_NET_LWM2M_TYPE_INTEGER_E);
          goto end;
        case QAPI_NET_LWM2M_SECURITY_RES_M_BOOTSTRAP_SERVER_E: 
          result = (type == QAPI_NET_LWM2M_TYPE_BOOLEAN_E);
          goto end;
        case QAPI_NET_LWM2M_SECURITY_RES_O_PUBLIC_KEY_E: 
        case QAPI_NET_LWM2M_SECURITY_RES_O_SERVER_PUBLIC_KEY_E: 
        case QAPI_NET_LWM2M_SECURITY_RES_O_SECRET_KEY_E: 
          result = (type == QAPI_NET_LWM2M_TYPE_OPAQUE_E);
          goto end;
        default:
        {
          goto end;
        }
      }
    }
    case QAPI_NET_LWM2M_SERVER_OBJECT_ID_E:
    {
      switch(id_info->res_id)
      {
        case QAPI_NET_LWM2M_SERVER_RES_M_SHORT_SERVER_ID_E:
        case QAPI_NET_LWM2M_SERVER_RES_M_LIFE_TIME_E:
        case QAPI_NET_LWM2M_SERVER_RES_O_DEFAULT_PMIN_E:
        case QAPI_NET_LWM2M_SERVER_RES_O_DEFAULT_PMAX_E:
        case QAPI_NET_LWM2M_SERVER_RES_O_DISABLE_TIMEOUT_ID_E:
        case QAPI_NET_LWM2M_SERVER_RES_O_REG_PRI_ORD_E:
        case QAPI_NET_LWM2M_SERVER_RES_O_INITIAL_DELAY_TIMER_E:
        case QAPI_NET_LWM2M_SERVER_RES_O_COMM_RETRY_COUNT_E:
        case QAPI_NET_LWM2M_SERVER_RES_O_COMM_RETRY_TIMER_E:
        case QAPI_NET_LWM2M_SERVER_RES_O_COMM_SEQ_DELAY_TIMER_E:
        case QAPI_NET_LWM2M_SERVER_RES_O_COMM_SEQ_RETRY_COUNT_E:
          result = (type == QAPI_NET_LWM2M_TYPE_INTEGER_E);
          goto end;
        case QAPI_NET_LWM2M_SERVER_RES_M_BINDING_ID_E:
        case QAPI_NET_LWM2M_SERVER_RES_O_PREFERRED_TRANSPORT_E:
          result = (type == QAPI_NET_LWM2M_TYPE_STRING_E);
          goto end;	
        case QAPI_NET_LWM2M_SERVER_RES_O_TRIGGER_E:
        case QAPI_NET_LWM2M_SERVER_RES_M_STORING_ID_E:
        case QAPI_NET_LWM2M_SERVER_RES_O_REG_FAILURE_BLOCK_E:
        case QAPI_NET_LWM2M_SERVER_RES_O_BS_ON_REG_FAILURE_E:
          result = (type == QAPI_NET_LWM2M_TYPE_BOOLEAN_E);
          goto end;
        case QAPI_NET_LWM2M_SERVER_RES_O_APN_LINK_E:
          result = (type == QAPI_NET_LWM2M_TYPE_OBJECT_LINK);
          goto end;
        default:
        {
          goto end;
        }
      }
    }
    case QAPI_NET_LWM2M_ACCESS_CONTROL_OBJECT_ID_E:
    {
      switch(id_info->res_id)
      {
        case QAPI_NET_LWM2M_ACL_RES_M_OBJECT_ID_E:
        case QAPI_NET_LWM2M_ACL_RES_M_OBJECT_INSTANCE_ID_E:
        case QAPI_NET_LWM2M_ACL_RES_M_ACL_ID:
        case QAPI_NET_LWM2M_ACL_RES_M_ACL_OWNER:
          result = (type == QAPI_NET_LWM2M_TYPE_INTEGER_E);
          goto end;
        default:
        {
          goto end;
        }
      }
    }
    case QAPI_NET_LWM2M_DEVICE_OBJECT_ID_E:
    {
      switch(id_info->res_id)
      {
        case LWM2M_AT_DEVICE_RES_MANUFACTURER:
        case LWM2M_AT_DEVICE_RES_MODEL_NUMBER:
        case LWM2M_AT_DEVICE_RES_DEVICE_INFO:
        {
          result = (type == QAPI_NET_LWM2M_TYPE_STRING_E);
          goto end;
        }
        default:
        {
          goto end;
        }
      }
    }
    case QAPI_NET_LWM2M_SOFTWARE_MGNT_OBJECT_ID_E:
    {
      switch(id_info->res_id)
      {
        case QAPI_NET_LWM2M_SW_MGNT_RES_O_PACKAGE_NAME_E:
        case QAPI_NET_LWM2M_SW_MGNT_RES_O_PACKAGE_VERSION_E:
        case QAPI_NET_LWM2M_SW_MGNT_RES_O_PACKAGE_URI_E: //Optional resource
        {
          result = (type == QAPI_NET_LWM2M_TYPE_STRING_E);
          goto end;
        }
        case QAPI_NET_LWM2M_SW_MGNT_RES_M_UPDATE_STATE_E:
        case QAPI_NET_LWM2M_SW_MGNT_RES_M_UPDATE_RESULT_E:
        {
          result = (type == QAPI_NET_LWM2M_TYPE_INTEGER_E);
          goto end;
        }
        case QAPI_NET_LWM2M_SW_MGNT_RES_M_ACTIVATION_STATE_E:
        {
          result = (type == QAPI_NET_LWM2M_TYPE_BOOLEAN_E);
          goto end;
        }
        case QAPI_NET_LWM2M_SW_MGNT_RES_O_PACKAGE_E://Optional resource
        {
          result = (type == QAPI_NET_LWM2M_TYPE_OPAQUE_E);
          goto end;
        }
        default:
        {
          goto end;
        }
      }
    }
    case QAPI_NET_LWM2M_DEVICE_CAP_OBJECT_ID_E:
    {
      switch(id_info->res_id)
      {
        case QAPI_NET_LWM2M_DEVICE_RES_M_PROPERTY_E:
        case QAPI_NET_LWM2M_DEVICE_RES_O_DESCRIPTION_E:
        {
          result = (type == QAPI_NET_LWM2M_TYPE_STRING_E);
          goto end;
        }
        case QAPI_NET_LWM2M_DEVICE_RES_M_GROUP_E:
        {
          result = (type == QAPI_NET_LWM2M_TYPE_INTEGER_E);
          goto end;
        }
        case QAPI_NET_LWM2M_DEVICE_RES_O_ATTACHED_E:
        case QAPI_NET_LWM2M_DEVICE_RES_M_ENABLED_E:
        case QAPI_NET_LWM2M_DEVICE_RES_O_NOTIFY_EN_E:
        {
          result = (type == QAPI_NET_LWM2M_TYPE_BOOLEAN_E);
          goto end;
        }
        default:
        {
          goto end;
        }
      }
    }
    case QAPI_NET_LWM2M_APN_CONN_OBJECT_ID_E:
    {
      switch(id_info->res_id)
      {
        case QAPI_NET_LWM2M_APN_M_PROFILE_NAME_E:
        case QAPI_NET_LWM2M_APN_O_APN_E:
        case QAPI_NET_LWM2M_APN_O_USER_NAME_E:
        case QAPI_NET_LWM2M_APN_O_SECRET_E:
        case QAPI_NET_LWM2M_APN_O_RECONN_SCHEDULE_E:
        case QAPI_NET_LWM2M_APN_O_VALIDITY_E:
        case QAPI_NET_LWM2M_APN_O_IP_ADDR_E:              
        case QAPI_NET_LWM2M_APN_O_PREFIX_LEN_E:
        case QAPI_NET_LWM2M_APN_O_SUBNET_MASK_E:
        case QAPI_NET_LWM2M_APN_O_GATEWAY_E:
        case QAPI_NET_LWM2M_APN_O_PRI_DNS_E:
        case QAPI_NET_LWM2M_APN_O_SEC_DNS_E:
        {
          result = (type == QAPI_NET_LWM2M_TYPE_STRING_E);
          goto end;
        }
        case QAPI_NET_LWM2M_APN_O_AUTO_SELECT_APN_E:
        case QAPI_NET_LWM2M_APN_O_ENABLE_STATUS_E:
        {
          result = (type == QAPI_NET_LWM2M_TYPE_BOOLEAN_E);
          goto end;
        }
        case QAPI_NET_LWM2M_APN_M_AUTH_TYPE_E:
        case QAPI_NET_LWM2M_APN_O_QCI_E:
        case QAPI_NET_LWM2M_APN_O_PDN_TYPE_E:
        case QAPI_NET_LWM2M_APN_O_APN_CONTROL_E:
        case QAPI_NET_LWM2M_APN_O_CONN_ESTB_RESULT_E:
        case QAPI_NET_LWM2M_APN_O_CONN_ESTB_REJECT_CAUSE_E:
        case QAPI_NET_LWM2M_APN_O_CONN_END_TIME_E:
        case QAPI_NET_LWM2M_APN_O_TOT_BYTES_SENT_E:
        case QAPI_NET_LWM2M_APN_O_TOT_PKTS_SENT_E:
        case QAPI_NET_LWM2M_APN_O_TOT_BYTES_RECV_E:
        {
          result = (type == QAPI_NET_LWM2M_TYPE_INTEGER_E);
          goto end;
        }			
        case QAPI_NET_LWM2M_APN_O_VENDOR_EXT_E:
        {
          result = (type == QAPI_NET_LWM2M_TYPE_OBJECT_LINK);
          goto end;
        }
        default:
        {
          goto end;
        }
      }
    }
    case QAPI_NET_LWM2M_CELL_NW_CONN_OBJECT_ID_E:
    {
      switch(id_info->res_id)
      {
        case QAPI_NET_LWM2M_NW_CONN_O_SMSC_ADDR_E:
        case QAPI_NET_LWM2M_NW_CONN_O_MOD_ACTIVATION_CODE_E:
        {
          result = (type == QAPI_NET_LWM2M_TYPE_STRING_E);
          goto end;
        }
        case QAPI_NET_LWM2M_NW_CONN_O_DISABLE_RADIO_PERIOD_E:
        case QAPI_NET_LWM2M_NW_CONN_O_PSM_TIMER_E:
        case QAPI_NET_LWM2M_NW_CONN_O_ACTIVE_TIMER_E:
        case QAPI_NET_LWM2M_NW_CONN_O_SERVING_PLMN_RATE_CONTROL_E:
        {
          result = (type == QAPI_NET_LWM2M_TYPE_INTEGER_E);
          goto end;
        }
        case QAPI_NET_LWM2M_NW_CONN_O_VENDOR_EXT_E:
        case QAPI_NET_LWM2M_NW_CONN_M_ACTIVATED_PROF_NAME_E:
        {
          result = (type == QAPI_NET_LWM2M_TYPE_OBJECT_LINK);
          goto end;
        }
        case QAPI_NET_LWM2M_NW_CONN_O_EDRX_IU_E:
        case QAPI_NET_LWM2M_NW_CONN_O_EDRX_WB_S1_E:
        case QAPI_NET_LWM2M_NW_CONN_O_EDRX_NB_S1_E:
        case QAPI_NET_LWM2M_NW_CONN_O_EDRX_AGB_E:
        {
          result = (type == QAPI_NET_LWM2M_TYPE_OPAQUE_E);
          goto end;
        }
        default:
        {
          goto end;
        }
      }
    }

    default:
    {
      goto end;
    }
  }
end:
  ATFWD_LOG_MSG_MED("resource_type_check() out, bool res %d", result, 0, 0);
  return result;

}
/**
 * @fn void lwm2m_util_obj_info_check()
 * @brief check validity for target with standard as the reference
 * @param
 *     @param[in] int level, level 1 means target must to be totally match with standard, level 0 allow target to be sub-level of standard.
 *     @param[in] qapi_Net_LWM2M_Obj_Info_t * standard_ptr
 *     @param[in] qapi_Net_LWM2M_Obj_Info_t * target_ptr
 * @return boolean
 */
boolean lwm2m_util_obj_info_check(int level, qapi_Net_LWM2M_Obj_Info_t * standard_ptr, qapi_Net_LWM2M_Obj_Info_t * target_ptr)
{
  boolean result = FALSE;
  ATFWD_LOG_MSG_MED("obj_info_check() in", 0,0,0);

  if(standard_ptr == NULL || target_ptr == NULL || (level != 0 && level != 1))
  {
    ATFWD_LOG_MSG_LOW("invalid input", 0, 0, 0);
    goto end;
  }
  if(level == 1 && target_ptr->obj_mask != standard_ptr->obj_mask)
  {
    goto end;
  }
  if(standard_ptr->obj_mask & QAPI_NET_LWM2M_OBJECT_ID_E)
  {
    if(!(target_ptr->obj_mask & QAPI_NET_LWM2M_OBJECT_ID_E) || target_ptr->obj_id != standard_ptr->obj_id)
    {
      goto end;
    }
  }
  if(standard_ptr->obj_mask & QAPI_NET_LWM2M_INSTANCE_ID_E)
  {
    if(!(target_ptr->obj_mask & QAPI_NET_LWM2M_INSTANCE_ID_E) || target_ptr->obj_inst_id != standard_ptr->obj_inst_id)
    {
      goto end;
    }
  }
  if(standard_ptr->obj_mask & QAPI_NET_LWM2M_RESOURCE_ID_E)
  {
    if(!(target_ptr->obj_mask & QAPI_NET_LWM2M_RESOURCE_ID_E) || target_ptr->res_id != standard_ptr->res_id)
    {
      goto end;
    }
  }
  if(standard_ptr->obj_mask & QAPI_NET_LWM2M_RESOURCE_INSTANCE_ID_E)
  {
    if(!(target_ptr->obj_mask & QAPI_NET_LWM2M_RESOURCE_INSTANCE_ID_E) || target_ptr->res_inst_id != standard_ptr->res_inst_id)
    {
      goto end;
    }
  }
  result = true;
end:
  ATFWD_LOG_MSG_MED("obj_info_check() out, bool res %d, level %d, std mask %d", result, level, (int)((standard_ptr == NULL)? -1 : standard_ptr->obj_mask));
  return result;
}
/**
 * @fn void lwm2m_util_msgid_copy()
 * @brief generate copy msg id array, user need to guarantee dest size > src size, this function will also print msgid out.
 * @param int8_t * src_msgid_array, uint8_t dest_msgid_array, uint8_t msg_id_count
 * @return void
 */
void lwm2m_util_msgid_copy(uint8_t * src_msgid_array, uint8_t * dest_msgid_array, uint8_t msg_id_count)
{
  char attr_buffer[LWM2M_AT_PAYLOAD_MAX_LEN+1] = {0};
  int32 attr_len = 0;
  int i = 0;
  ATFWD_LOG_MSG_MED("msgid_copy() in", 0, 0, 0);

  if(src_msgid_array == NULL || dest_msgid_array == NULL || msg_id_count == 0)
  {
    ATFWD_LOG_MSG_LOW("invalid input", 0, 0, 0);
    goto end;
  }
  attr_len += snprintf(attr_buffer+attr_len, LWM2M_AT_PAYLOAD_MAX_LEN - attr_len,"TKN:[");
  for(i = 0; i < msg_id_count; i++)
  {
    dest_msgid_array[i] = src_msgid_array[i];
    attr_len += snprintf(attr_buffer+attr_len, LWM2M_AT_PAYLOAD_MAX_LEN - attr_len,"%x ",src_msgid_array[i]);
  }
  attr_len += snprintf(attr_buffer+attr_len, LWM2M_AT_PAYLOAD_MAX_LEN - attr_len,"]\n");
end:
  ATFWD_LOG_MSG_HIGH_SPRINT("msgid_copy() out, msg %s", attr_buffer, 0, 0);
}
/**
 * @fn qapi_Net_LWM2M_Flat_Data_t * lwm2m_util_FlatData_array_find()
 * @brief find id from qapi_Net_LWM2M_Flat_Data_t single array, count must to be the length of array.
 * @param
 *     @param[in] qapi_Net_LWM2M_Flat_Data_t * array
 *     @param[in] count
 *     @param[in] id
 * @return qapi_Net_LWM2M_Flat_Data_t pointer
 */
qapi_Net_LWM2M_Flat_Data_t * lwm2m_util_FlatData_array_find(qapi_Net_LWM2M_Flat_Data_t * array, unsigned int count, uint16_t id)
{
  int i = 0;
  qapi_Net_LWM2M_Flat_Data_t * match_ep = NULL;
  ATFWD_LOG_MSG_MED("FlatData_array_find() in", 0, 0, 0);

  if(array == NULL | count == 0)
  {
    ATFWD_LOG_MSG_LOW("invalid input", 0, 0, 0);
    goto end;
  }

  for(i = 0; i < count; i++)
  {
    if(array[i].id == id)
    {
      match_ep = &array[i];
      goto end;
    }
  }
end:
  ATFWD_LOG_MSG_MED("FlatData_array_find() out, match_ep %d", match_ep, 0, 0);
  return match_ep;
}
/**
 * @fn boolean lwm2m_util_FlatData_array_copy()
 * @brief copy qapi_Net_LWM2M_Flat_Data_t array structure, caller need to guarantee sizeof(dest) > sizeof(src) >= count
 * @param
 *     @param[in] qapi_Net_LWM2M_Flat_Data_t * src
 *     @param[in] qapi_Net_LWM2M_Flat_Data_t * dest
 *     @param[in] count
 * @return result
 */
boolean lwm2m_util_FlatData_array_copy(qapi_Net_LWM2M_Flat_Data_t * src, qapi_Net_LWM2M_Flat_Data_t * dest, unsigned int count)
{
  int i = 0;
  boolean result = FALSE;

  ATFWD_LOG_MSG_MED("FlatData_array_copy() in, src %d, dest %d, count %d", src, dest, 0);

  if(src == NULL || dest == NULL || count == 0)
  {
    goto end;
  }
  for(i = 0; i < count; i++)
  {
    dest[i].id = src[i].id;
    dest[i].type = src[i].type;
    switch(src[i].type)
    {
      case QAPI_NET_LWM2M_TYPE_INTEGER_E:
      {
        dest[i].value.asInteger = src[i].value.asInteger;
        break;
      }
      case QAPI_NET_LWM2M_TYPE_BOOLEAN_E:
      {
        dest[i].value.asBoolean= src[i].value.asBoolean;
        break;
      }
      case QAPI_NET_LWM2M_TYPE_FLOAT_E:
      {
        dest[i].value.asFloat= src[i].value.asFloat;
        break;
      }
      case QAPI_NET_LWM2M_TYPE_STRING_E:
      case QAPI_NET_LWM2M_TYPE_OPAQUE_E:
      {
        if(src[i].value.asBuffer.buffer == NULL || dest[i].value.asBuffer.buffer != NULL)
        {
          ATFWD_LOG_MSG_LOW("buffer error, src buffer %d, dest buffer %d", src[i].value.asBuffer.buffer, dest[i].value.asBuffer.buffer, 0);
          goto end;
        }
        dest[i].value.asBuffer.buffer = athdl_malloc(src[i].value.asBuffer.length);
        if(dest[i].value.asBuffer.buffer == NULL)
        {
          ATFWD_LOG_MSG_LOW("malloc failure", 0, 0, 0);
          goto end;
        }
        memscpy(dest[i].value.asBuffer.buffer,src[i].value.asBuffer.length,src[i].value.asBuffer.buffer,src[i].value.asBuffer.length);
        dest[i].value.asBuffer.length = src[i].value.asBuffer.length;
        break;
      }
      case QAPI_NET_LWM2M_TYPE_OBJECT_LINK:
      {
        dest[i].value.asObjLink= src[i].value.asObjLink;
        break;
      }
      case QAPI_NET_LWM2M_TYPE_OBJECT:
      case QAPI_NET_LWM2M_TYPE_OBJECT_INSTANCE:
      case QAPI_NET_LWM2M_TYPE_MULTIPLE_RESOURCE:
      {
        if(src[i].value.asChildren.array != NULL && dest[i].value.asChildren.array == NULL)
        {
          dest[i].value.asChildren.array = athdl_malloc(sizeof(qapi_Net_LWM2M_Flat_Data_t) * (src[i].value.asChildren.count));
          if(dest[i].value.asChildren.array == NULL)
          {
            ATFWD_LOG_MSG_LOW("array malloc failure", 0, 0, 0);
            goto end;
          }

          if(TRUE != lwm2m_util_FlatData_array_copy(src[i].value.asChildren.array, dest[i].value.asChildren.array,src[i].value.asChildren.count))
          {
            ATFWD_LOG_MSG_ERROR("sub arr cp err", 0, 0, 0);
            goto end;
          }
          dest[i].value.asChildren.count = src[i].value.asChildren.count;
          break;
        }
        else
        {
          break;
        }
      }
      case QAPI_NET_LWM2M_TYPE_UNDEFINED:
      {
        break;
      }
      default:
        ATFWD_LOG_MSG_ERROR("unknown type", 0, 0, 0);
        goto end;
    }
  }
  result = true;
end:
  ATFWD_LOG_MSG_MED("FlatData_array_copy() out, bool res %d, count %d", (int)result, count, 0);
  return result;
}
/**
 * @fn void lwm2m_util_FlatData_array_free()
 * @brief free qapi_Net_LWM2M_Flat_Data_t array.
 * @param
 *     @param[in] qapi_Net_LWM2M_Flat_Data_t * array
 *     @param[in] count 
 * @return N.A
 */
void lwm2m_util_FlatData_array_free(qapi_Net_LWM2M_Flat_Data_t ** array, unsigned int count)
{
  int i = 0;

  ATFWD_LOG_MSG_MED("FlatData_array_free() in, count %d", count, 0, 0);

  if(array == NULL || *array == NULL)
  {
    ATFWD_LOG_MSG_LOW("ATHDL: input error", 0, 0, 0);
    goto end;
  }

  ATFWD_LOG_MSG_LOW("freeing flat_data_ptr %d, id %d, type %d", *array, (*array)->id, (*array)->type);

  for(i = 0; i < count; i++)
  {
    if(((*array)[i].type == QAPI_NET_LWM2M_TYPE_STRING_E || (*array)[i].type == QAPI_NET_LWM2M_TYPE_OPAQUE_E) &&
    (*array)[i].value.asBuffer.buffer != NULL)
    {
      athdl_free((*array)[i].value.asBuffer.buffer);
    }
    else if(((*array)[i].type == QAPI_NET_LWM2M_TYPE_OBJECT || (*array)[i].type == QAPI_NET_LWM2M_TYPE_OBJECT_INSTANCE || (*array)[i].type == QAPI_NET_LWM2M_TYPE_MULTIPLE_RESOURCE) &&
             (*array)[i].value.asChildren.array != NULL)
    {
      lwm2m_util_FlatData_array_free(&((*array)[i].value.asChildren.array), (*array)[i].value.asChildren.count);
    }
    else
    {
      continue;
    }
  }
  athdl_free(*array);
end:
  ATFWD_LOG_MSG_MED("FlatData_array_free() out", 0, 0, 0);
  return;
}
/**
 * @fn boolean lwm2m_util_FlatData_array_extend()
 * @brief extend flat data array
 * @param
 *     @param[in/out] array
 *     @param[in/out] array_len
 *     @param[in] new_ep
 * @return TRUE/FALSE
 */
boolean lwm2m_util_FlatData_array_extend(qapi_Net_LWM2M_Flat_Data_t ** array, size_t * array_len, qapi_Net_LWM2M_Flat_Data_t ** new_ep)
{
  boolean result = FALSE;
  qapi_Net_LWM2M_Flat_Data_t * tmp_ptr = NULL;

  ATFWD_LOG_MSG_MED("FlatData_array_extend() in array %d, len %d", ((array == NULL)? -1 : (int)*array), ((array_len == NULL)? -1 : (int)*array_len), 0);

  if(new_ep == NULL || *new_ep == NULL || array == NULL || array_len == NULL || (*array == NULL && *array_len != 0))
  {
    ATFWD_LOG_MSG_LOW("input invalid", 0, 0, 0);
    goto end;
  }

  //extent array + 1
  tmp_ptr = *array;
  *array = athdl_malloc(sizeof(qapi_Net_LWM2M_Flat_Data_t) * ((*array_len)+1));
  if(*array == NULL)
  {
    ATFWD_LOG_MSG_LOW("memory fail", 0, 0, 0);
    goto end;
  }

  //copy original array back
  if(tmp_ptr != NULL && FALSE == lwm2m_util_FlatData_array_copy(tmp_ptr,*array,*array_len))
  {
    ATFWD_LOG_MSG_ERROR("cp org err", 0, 0, 0);
    lwm2m_util_FlatData_array_free(array, (*array_len)+1);
    *array = tmp_ptr;
    goto end;
  }
  if(FALSE == lwm2m_util_FlatData_array_copy(*new_ep, &((*array)[(*array_len)]), 1))
  {
    ATFWD_LOG_MSG_ERROR("cp new err", 0, 0, 0);
    lwm2m_util_FlatData_array_free(array, (*array_len)+1);
    *array = tmp_ptr;
    goto end;
  }
  //free original array memory
  lwm2m_util_FlatData_array_free(&tmp_ptr, *array_len);
  lwm2m_util_FlatData_array_free(new_ep,1);
  //array count++
  (*array_len)++;
  result = TRUE;
end:
  ATFWD_LOG_MSG_MED("FlatData_array_extend() out, bool res %d", (int)result, 0, 0);
  return result;
}
/**
 * @fn void lwm2m_util_FlatData_display()
 * @brief send flat data with URC
 * @param
 * @return boolean
 */
boolean lwm2m_util_FlatData_display(char * atcmd_name, char * prefix, int max_resp_len, qapi_Net_LWM2M_Flat_Data_t * lwm2m_data, boolean isEnd)
{
  int prefix_len = 0;
  char * tmp_ptr = NULL;
  uint8_t * data_buf = NULL;
  char * inter_resp = NULL; //whole body of each line

  boolean result = FALSE;
  ATFWD_LOG_MSG_MED("FlatData_FlatData_display() in", 0, 0, 0);

  if(atcmd_name == NULL || prefix == NULL || max_resp_len < ATHDL_RESP_LEN_MIN || lwm2m_data == NULL)
  {
    ATFWD_LOG_MSG_LOW("malloc inter_resp failed", 0, 0, 0);
    goto end;
  }
  ATFWD_LOG_MSG_LOW("input parameter lwm2m flat data with id: %d, type: %d", lwm2m_data->id, lwm2m_data->type, 0);

  prefix_len = strlen(prefix);
  
  max_resp_len++;//increase 1, to give \0 place.
  inter_resp = athdl_malloc(max_resp_len);
  if(inter_resp == NULL)
  {
    ATFWD_LOG_MSG_LOW("malloc inter_resp failed", 0, 0, 0);
    goto end;
  }

  if (QAPI_NET_LWM2M_TYPE_UNDEFINED == lwm2m_data->type ||
      QAPI_NET_LWM2M_TYPE_OBJECT_LINK == lwm2m_data->type ||
      QAPI_NET_LWM2M_TYPE_INTEGER_E == lwm2m_data->type ||
      QAPI_NET_LWM2M_TYPE_FLOAT_E == lwm2m_data->type ||
      QAPI_NET_LWM2M_TYPE_BOOLEAN_E == lwm2m_data->type)
  {
    switch(lwm2m_data->type)
    {
      case QAPI_NET_LWM2M_TYPE_UNDEFINED:
      {
        if(isEnd)
        {
          snprintf(inter_resp, max_resp_len, "%s%d\"\n", prefix, (int)(lwm2m_data->id));
        }
        else
        {
          snprintf(inter_resp, max_resp_len, "%s%d\",, 1\n", prefix, (int)(lwm2m_data->id));
        }
        break;
      }
      case QAPI_NET_LWM2M_TYPE_OBJECT_LINK:
      {
        uint16_t obj_id = lwm2m_data->value.asObjLink >> 16;
        uint16_t inst_id = lwm2m_data->value.asObjLink & 0xFFFF;
        if(isEnd)
        {
          snprintf(inter_resp, max_resp_len, "%s%d\", %d, [%d:%d]\n", prefix, (int)(lwm2m_data->id), (int)(lwm2m_data->type), obj_id, inst_id);
        }
        else
        {
          snprintf(inter_resp, max_resp_len, "%s%d\", %d, [%d:%d],, 1\n", prefix, (int)(lwm2m_data->id), (int)(lwm2m_data->type), obj_id, inst_id);
        }
        break;
      }
      case QAPI_NET_LWM2M_TYPE_INTEGER_E:
      {
        if(isEnd)
        {
          snprintf(inter_resp, max_resp_len, "%s%d\", %d, %lld\n", prefix, (int)(lwm2m_data->id), (int)(lwm2m_data->type), lwm2m_data->value.asInteger);
        }
        else
        {
          snprintf(inter_resp, max_resp_len, "%s%d\", %d, %lld,, 1\n", prefix, (int)(lwm2m_data->id), (int)(lwm2m_data->type), lwm2m_data->value.asInteger);
        }
        break;
      }
      case QAPI_NET_LWM2M_TYPE_FLOAT_E:
      {
        if((tmp_ptr = util_at_float_to_string(lwm2m_data->value.asFloat)) == NULL)
        {
          ATFWD_LOG_MSG_LOW("float_to_string err", 0, 0, 0);
          goto end;
        }
      
        if(isEnd)
        {
          snprintf(inter_resp, max_resp_len, "%s%d\", %d, %s\n", prefix, (int)(lwm2m_data->id), (int)(lwm2m_data->type), tmp_ptr);
        }
        else
        {
          snprintf(inter_resp, max_resp_len, "%s%d\", %d, %s,, 1\n", prefix, (int)(lwm2m_data->id), (int)(lwm2m_data->type), tmp_ptr);
        }
        break;
      }
      case QAPI_NET_LWM2M_TYPE_BOOLEAN_E:
      {
        if(isEnd)
        {
          snprintf(inter_resp, max_resp_len, "%s%d\", %d, %d\n", prefix, (int)(lwm2m_data->id), (int)(lwm2m_data->type), (int)(lwm2m_data->value.asBoolean));
        }
        else
        {
          snprintf(inter_resp, max_resp_len, "%s%d\", %d, %d,, 1\n", prefix, (int)(lwm2m_data->id), (int)(lwm2m_data->type), (int)(lwm2m_data->value.asBoolean));
        }
        break;
      }
    }
    qapi_atfwd_send_urc_resp(atcmd_name, inter_resp);
  }
  else if(QAPI_NET_LWM2M_TYPE_STRING_E == lwm2m_data->type || QAPI_NET_LWM2M_TYPE_OPAQUE_E == lwm2m_data->type)
  {
    size_t buf_len = 0;
    int other_len = 0;
  
    if(FALSE == lwm2m_util_unpack_buffer(&data_buf,&buf_len,
                                         lwm2m_data->value.asBuffer.buffer,lwm2m_data->value.asBuffer.length,
                                         lwm2m_data->type))
    {
      ATFWD_LOG_MSG_LOW("unpack buffer failed", 0, 0, 0);
      goto end;
    }
    buf_len = strlen((char*)data_buf);
    if(isEnd)
    {
      other_len = 14; //11 for {[res_id]", [value type], "", [Count Down]/n/0}  
    }
    else
    {
      other_len = 17; //17 for {[res_id]", [value type], "", [Count Down], 1/n/0}
    }
  
    if((prefix_len+buf_len+other_len) > max_resp_len)
    {
      int offset = 0;
      int split_buf_len = 0;
      int count_down = 0;
      tmp_ptr = athdl_malloc(max_resp_len);
      if(tmp_ptr == NULL)
      {
        ATFWD_LOG_MSG_LOW("malloc tmp_ptr failed", 0, 0, 0);
        goto end;
      }
      while(buf_len > 0)
      {
        memset(tmp_ptr, 0x00, max_resp_len);
        count_down = buf_len / (max_resp_len - (prefix_len+other_len));
        if(buf_len > (max_resp_len - (prefix_len+other_len)))
        {
          split_buf_len = (max_resp_len - (prefix_len+other_len));
        }
        else
        {
          split_buf_len = buf_len;
        }
        strlcpy(tmp_ptr, (char *)(data_buf+offset), split_buf_len + 1);
        offset = offset + strlen(tmp_ptr);
        buf_len = buf_len - split_buf_len;
  
        if(isEnd)
        {
          if(buf_len <= 0)
          {
            snprintf(inter_resp, max_resp_len, "%s%d\", %d, \"%s\"\n", prefix, (int)(lwm2m_data->id), (int)(lwm2m_data->type), tmp_ptr);
          }
          else
          {
            snprintf(inter_resp, max_resp_len, "%s%d\", %d, \"%s\", %d\n", prefix, (int)(lwm2m_data->id), (int)(lwm2m_data->type), tmp_ptr, count_down);
          }
        }
        else
        {
          if(buf_len <= 0)
          {
            snprintf(inter_resp, max_resp_len, "%s%d\", %d, \"%s\",, 1\n", prefix, (int)(lwm2m_data->id), (int)(lwm2m_data->type), tmp_ptr);
          }
          else
          {
            snprintf(inter_resp, max_resp_len, "%s%d\", %d, \"%s\", %d, 1\n", prefix, (int)(lwm2m_data->id), (int)(lwm2m_data->type), tmp_ptr, count_down);
          }
        }
        qapi_atfwd_send_urc_resp(atcmd_name, inter_resp);
        memset(inter_resp, 0x00, max_resp_len);
      }
    }
    else
    {
      if(isEnd)
      {
        snprintf(inter_resp, max_resp_len, "%s%d\", %d, \"%s\"\n", prefix, (int)(lwm2m_data->id), (int)(lwm2m_data->type), data_buf);
      }
      else
      {
        snprintf(inter_resp, max_resp_len, "%s%d\", %d, \"%s\",, 1\n", prefix, (int)(lwm2m_data->id), (int)(lwm2m_data->type), data_buf);
      }
      qapi_atfwd_send_urc_resp(atcmd_name, inter_resp);
    }
  }
  else
  {
    ATFWD_LOG_MSG_ERROR("Res / ResInsID : %d, type %d unsupport", (int)(lwm2m_data->id),(int)(lwm2m_data->type), 0);
  }
  result = TRUE;
end:
  athdl_free(data_buf);
  athdl_free(inter_resp);
  athdl_free(tmp_ptr);
  ATFWD_LOG_MSG_MED("FlatData_FlatData_display() out, bool res %d", (int)result,0, 0);
  return result;
}
/**
 * @fn void lwm2m_util_FlatData_array_display()
 * @brief send flat data with URC, only available to resource array
 * @param
 *     @param[in] atcmd_name
 *     @param[in] prefix //[urc_header, "ObjId/InstId/]
 *     @param[in] max_resp_len
 *     @param[in] lwm2m_data
 *     @param[in] data_len
 * @return boolean
 */
boolean lwm2m_util_FlatData_array_display(char * atcmd_name, char * prefix, int max_resp_len, qapi_Net_LWM2M_Flat_Data_t * lwm2m_data, size_t data_len)
{
  char * tmp_prefix = NULL;
  uint32_t cntr = 0;

  boolean result = FALSE;
  ATFWD_LOG_MSG_MED("FlatData_array_display() in", 0, 0, 0);

  if(atcmd_name == NULL || prefix == NULL || lwm2m_data == NULL || max_resp_len < ATHDL_RESP_LEN_MIN || data_len == 0)
  {
    ATFWD_LOG_MSG_LOW("ATHDL: invalid input", 0, 0, 0);
    goto end;
  }

  ATFWD_LOG_MSG_LOW_SPRINT("input cmd_name: %s, pre: %s", atcmd_name, prefix, 0);

  /* Object information */
  ATFWD_LOG_MSG_LOW("input parameter lwm2m flat data with id: %d, type: %d", lwm2m_data->id, lwm2m_data->type, 0);
  ATFWD_LOG_MSG_LOW_SPRINT("atcmd_name: %s, prefix: %s, max_resp_len: %d", atcmd_name, prefix, max_resp_len);

  tmp_prefix = athdl_malloc(max_resp_len);
  if(tmp_prefix == NULL)
  {
    ATFWD_LOG_MSG_LOW("malloc inter_resp failed", 0, 0, 0);
    goto end;
  }

  for (cntr = 0; cntr < data_len; cntr++)
  {
    if(QAPI_NET_LWM2M_TYPE_MULTIPLE_RESOURCE != lwm2m_data[cntr].type)
    {
      if(FALSE == lwm2m_util_FlatData_display(atcmd_name, prefix, max_resp_len, &(lwm2m_data[cntr]),(cntr == data_len -1)))
      {
        ATFWD_LOG_MSG_LOW("malloc inter_resp failed", 0, 0, 0);
        goto end;
      }
    }
    else
    {
      int internal_cntr = 0;
      memset(tmp_prefix, 0x00, max_resp_len);
      snprintf(tmp_prefix, max_resp_len, "%s%d/", prefix, (int)(lwm2m_data[cntr].id));
      for (internal_cntr = 0; internal_cntr < lwm2m_data[cntr].value.asChildren.count; internal_cntr++)
      {
        if(FALSE == lwm2m_util_FlatData_display(atcmd_name, tmp_prefix, max_resp_len, &(lwm2m_data[cntr].value.asChildren.array[internal_cntr]),(cntr == data_len -1)))
        {
          ATFWD_LOG_MSG_LOW("malloc inter_resp failed", 0, 0, 0);
          goto end;
        }
      }
    }
  }
  result = TRUE;
end:
  ATFWD_LOG_MSG_MED("FlatData_array_display() out, bool res %d, max_len: %d", (int)result, max_resp_len, 0);
  athdl_free(tmp_prefix);
  return result;
}

/**
 * @fn atfwd_result_code_t lwm2m_util_FlatData_struct_extend()
 * @brief extend obj_root with one new node.
 * @param
 *     @param[in] qapi_Net_LWM2M_Flat_Data_t * obj_root
 *     @param[in] qapi_Net_LWM2M_Obj_Info_t * id_info 
 *     @param[in|out] iqapi_Net_LWM2M_Flat_Data_t ** new_node
 * @return int32 AT Handler error
 */
atfwd_result_code_t lwm2m_util_FlatData_struct_extend(qapi_Net_LWM2M_Flat_Data_t * obj_root, qapi_Net_LWM2M_Obj_Info_t * id_info, qapi_Net_LWM2M_Flat_Data_t ** new_node)
{
  atfwd_result_code_t result = ATFWD_RET_MEMORY_ERR;

  ATFWD_LOG_MSG_MED("FlatData_struct_extend() in", 0, 0, 0);

  if(obj_root == NULL || id_info == NULL || !(id_info->obj_mask & QAPI_NET_LWM2M_OBJECT_ID_E) || obj_root->type != QAPI_NET_LWM2M_TYPE_OBJECT || id_info->obj_id != obj_root->id || new_node == NULL)
  {
    ATFWD_LOG_MSG_LOW("invalid input", 0, 0, 0);
    result = ATFWD_RET_PARAM_INPUT_ERR;
    goto end;
  }

  if(id_info->obj_mask & QAPI_NET_LWM2M_INSTANCE_ID_E)
  {/*set inst_id*/
    qapi_Net_LWM2M_Flat_Data_t * inst_id_match_ep = lwm2m_util_FlatData_array_find(obj_root->value.asChildren.array,
                                                                                   obj_root->value.asChildren.count,
                                                                                   id_info->obj_inst_id);
    if(inst_id_match_ep != NULL)
    {/*set inst_id -> find inst_id*/
      if(id_info->obj_mask & QAPI_NET_LWM2M_RESOURCE_ID_E)
      {/*set inst_id -> find inst_id -> set res_id*/
        qapi_Net_LWM2M_Flat_Data_t * res_id_match_ep = lwm2m_util_FlatData_array_find(inst_id_match_ep->value.asChildren.array,
                                                                                      inst_id_match_ep->value.asChildren.count,
                                                                                      id_info->res_id);
        if(res_id_match_ep != NULL)
        {/*set inst_id -> find inst_id -> set res_id -> find res_id*/
          if(id_info->obj_mask & QAPI_NET_LWM2M_RESOURCE_INSTANCE_ID_E)
          {/*set inst_id -> find inst_id -> set res_id -> find res_id -> set res_inst_id*/
            qapi_Net_LWM2M_Flat_Data_t * res_inst_id_match_ep = lwm2m_util_FlatData_array_find(res_id_match_ep->value.asChildren.array,
                                                                                               res_id_match_ep->value.asChildren.count,
                                                                                               id_info->res_inst_id);
            if(res_inst_id_match_ep != NULL)
            {/*set inst_id -> find inst_id -> set res_id -> find res_id -> set res_inst_id -> find res_inst_id*/
              ATFWD_LOG_MSG_LOW("Uir match to res_inst_id", 0, 0, 0);
              result = ATFWD_RET_EP_RES_INST_EXIST;
              goto end;
            }
            else
            {/*set inst_id -> find inst_id -> set res_id -> find res_id -> set res_inst_id -> !find res_inst_id*/
              if(*new_node != NULL)
              {
                (*new_node)->id = id_info->res_inst_id;
              }
              else
              {
                *new_node = athdl_malloc(sizeof(qapi_Net_LWM2M_Flat_Data_t));
                if(*new_node == NULL)
                {
                  ATFWD_LOG_MSG_LOW("new res_inst_node error, no memory", 0, 0, 0);
                  goto end;
                }
                (*new_node)->id = id_info->res_inst_id;
                (*new_node)->type = QAPI_NET_LWM2M_TYPE_UNDEFINED;
              }
              if(FALSE == lwm2m_util_FlatData_array_extend(&(res_id_match_ep->value.asChildren.array),
                                                            &(res_id_match_ep->value.asChildren.count),
                                                            new_node)) /*extend res_inst_array*/
              {
                ATFWD_LOG_MSG_LOW("extend res_inst_arr err", 0, 0, 0);
                result = ATFWD_RET_EP_RES_INST_MEMORY_ERR;
                if(*new_node != NULL && (*new_node)->type == QAPI_NET_LWM2M_TYPE_UNDEFINED)
                {
                  lwm2m_util_FlatData_array_free(new_node, 1);
                }
                goto end;
              }
              //*new_node = NULL;
            }
          }
          else
          {/*set inst_id -> find inst_id -> set res_id -> find res_id -> !set res_inst_id*/
            ATFWD_LOG_MSG_LOW("Uir match to res_id", 0, 0, 0);
            result = ATFWD_RET_EP_RES_EXIST;
            goto end;
          }
        }
        else
        {/*set inst_id -> find inst_id -> set res_id -> !find res_id*/
          if(id_info->obj_mask & QAPI_NET_LWM2M_RESOURCE_INSTANCE_ID_E)
          {/*set inst_id -> find inst_id -> set res_id -> !find res_id -> set res_inst_id*/
            res_id_match_ep = athdl_malloc(sizeof(qapi_Net_LWM2M_Flat_Data_t));
            if(res_id_match_ep == NULL)
            {
              ATFWD_LOG_MSG_LOW("new res_node error, no memory", 0, 0, 0);
              goto end;
            }
            res_id_match_ep->type = QAPI_NET_LWM2M_TYPE_MULTIPLE_RESOURCE;
            res_id_match_ep->id = id_info->res_id;
            if(*new_node != NULL)
            {
              (*new_node)->id = id_info->res_inst_id;
            }
            else
            {
              *new_node = athdl_malloc(sizeof(qapi_Net_LWM2M_Flat_Data_t));
              if(*new_node == NULL)
              {
                ATFWD_LOG_MSG_LOW("new res_inst_node error, no memory", 0, 0, 0);
                lwm2m_util_FlatData_array_free(&res_id_match_ep, 1);
                goto end;
              }
              (*new_node)->id = id_info->res_inst_id;
              (*new_node)->type = QAPI_NET_LWM2M_TYPE_UNDEFINED;
            }
            res_id_match_ep->value.asChildren.array = *new_node;
            res_id_match_ep->value.asChildren.count = 1;
            *new_node = NULL;
            if(FALSE == lwm2m_util_FlatData_array_extend(&(inst_id_match_ep->value.asChildren.array),
                                                          &(inst_id_match_ep->value.asChildren.count),
                                                          &res_id_match_ep)) /*extend res_array*/
            {
              ATFWD_LOG_MSG_LOW("extend res_arr err", 0, 0, 0);
              result = ATFWD_RET_EP_RES_MEMORY_ERR;
              if(res_id_match_ep)
              {
                lwm2m_util_FlatData_array_free(&res_id_match_ep, 1);
              }
              goto end;
            }
          }
          else
          {/*set inst_id -> find inst_id -> set res_id -> !find res_id -> !set res_inst_id*/
            if(*new_node != NULL)
            {
              (*new_node)->id = id_info->res_id;
            }
            else
            {
              *new_node = athdl_malloc(sizeof(qapi_Net_LWM2M_Flat_Data_t));
              if(*new_node == NULL)
              {
                ATFWD_LOG_MSG_LOW("new res_array node error, no memory", 0, 0, 0);
                goto end;
              }
              (*new_node)->id = id_info->res_id;
              (*new_node)->type = QAPI_NET_LWM2M_TYPE_UNDEFINED;
            }           
            if(FALSE == lwm2m_util_FlatData_array_extend(&(inst_id_match_ep->value.asChildren.array),
                                                          &(inst_id_match_ep->value.asChildren.count),
                                                          new_node)) /*extend res_array*/
            {
              ATFWD_LOG_MSG_LOW("extend res_arr err", 0, 0, 0);
              result = ATFWD_RET_EP_RES_MEMORY_ERR;
              if(*new_node != NULL && (*new_node)->type == QAPI_NET_LWM2M_TYPE_UNDEFINED)
              {
                lwm2m_util_FlatData_array_free(new_node, 1);
              }
              goto end;
            }
            //*new_node = NULL;
          }
        }
      }
      else
      {/*set inst_id -> find inst_id -> !set res_id*/
        ATFWD_LOG_MSG_LOW("Uir match to inst_id", 0, 0, 0);
        result = ATFWD_RET_EP_INST_EXIST;
        goto end;
      }
    }
    else
    {/*set inst_id -> !find inst_id*/
      inst_id_match_ep = athdl_malloc(sizeof(qapi_Net_LWM2M_Flat_Data_t));
      if(inst_id_match_ep == NULL)
      {
        ATFWD_LOG_MSG_LOW("extend inst_array error, no memory", 0, 0, 0);
        goto end;
      }
      inst_id_match_ep->type = QAPI_NET_LWM2M_TYPE_OBJECT_INSTANCE;
      inst_id_match_ep->id = id_info->obj_inst_id;
      if(id_info->obj_mask & QAPI_NET_LWM2M_RESOURCE_ID_E)
      {/*set inst_id -> !find inst_id -> set res_id*/
        if(id_info->obj_mask & QAPI_NET_LWM2M_RESOURCE_INSTANCE_ID_E)
        {/*set inst_id -> !find inst_id -> set res_id -> set res_inst_id*/
          inst_id_match_ep->value.asChildren.array = athdl_malloc(sizeof(qapi_Net_LWM2M_Flat_Data_t));
          if(inst_id_match_ep->value.asChildren.array == NULL)
          {
            ATFWD_LOG_MSG_LOW("extend inst_array error, no memory", 0, 0, 0);

            lwm2m_util_FlatData_array_free(&inst_id_match_ep, 1);

            goto end;
          }
          inst_id_match_ep->value.asChildren.array->type = QAPI_NET_LWM2M_TYPE_MULTIPLE_RESOURCE;
          inst_id_match_ep->value.asChildren.array->id = id_info->res_id;
          inst_id_match_ep->value.asChildren.count = 1;
          if(*new_node != NULL)
          {
            (*new_node)->id = id_info->res_inst_id;
          }
          else
          {
            *new_node = athdl_malloc(sizeof(qapi_Net_LWM2M_Flat_Data_t));
            if(*new_node == NULL)
            {
              ATFWD_LOG_MSG_LOW("new res_inst_array node error, no memory", 0, 0, 0);

              lwm2m_util_FlatData_array_free(&inst_id_match_ep, 1);

              goto end;
            }
            (*new_node)->id = id_info->res_inst_id;
            (*new_node)->type = QAPI_NET_LWM2M_TYPE_UNDEFINED;
          }
          inst_id_match_ep->value.asChildren.array->value.asChildren.array = *new_node;
          inst_id_match_ep->value.asChildren.array->value.asChildren.count = 1;
          *new_node = NULL;
        }
        else
        {/*set inst_id -> !find inst_id -> set res_id -> !set res_inst_id*/
          if(*new_node != NULL)
          {
            (*new_node)->id = id_info->res_id;
          }
          else
          {
            *new_node = athdl_malloc(sizeof(qapi_Net_LWM2M_Flat_Data_t));
            if(*new_node == NULL)
            {
              ATFWD_LOG_MSG_LOW("new res_array node error, no memory", 0, 0, 0);

              lwm2m_util_FlatData_array_free(&inst_id_match_ep, 1);

              goto end;
            }
            (*new_node)->id = id_info->res_id;
            (*new_node)->type = QAPI_NET_LWM2M_TYPE_UNDEFINED;
          }
          inst_id_match_ep->value.asChildren.array = *new_node;
          inst_id_match_ep->value.asChildren.count = 1;
          *new_node = NULL;
        }
      }
      if(FALSE == lwm2m_util_FlatData_array_extend(&(obj_root->value.asChildren.array),
                                                    &(obj_root->value.asChildren.count),
                                                    &(inst_id_match_ep))) /*extend inst_array*/
      {
        ATFWD_LOG_MSG_LOW("extend inst_arr err", 0, 0, 0);
        result = ATFWD_RET_EP_INST_MEMORY_ERR;
        if(inst_id_match_ep)
        {
          lwm2m_util_FlatData_array_free(&inst_id_match_ep, 1);
        }
      }
    }
  }
  else
  {/*!set inst_id*/
    ATFWD_LOG_MSG_LOW("Uir match to obj_id", 0, 0, 0);
    result = ATFWD_RET_EP_OBJ_EXIST;
  }
  result = ATFWD_RET_OK;
end:
  ATFWD_LOG_MSG_MED("FlatData_struct_extend() out, res %d", (int)result, 0, 0);
  return result;
}

/**
 * @fn atfwd_result_code_t lwm2m_util_ResInfo_list_extend()
 * @brief extend root with one new node.
 * @param
 *     @param[in] qapi_Net_LWM2M_Resource_Info_t ** root
 *     @param[in] qapi_Net_LWM2M_Obj_Info_t * id_info 
 *     @param[in|out] iqapi_Net_LWM2M_Flat_Data_t ** new_node
 * @return int32 AT Handler error
 */
atfwd_result_code_t lwm2m_util_ResInfo_list_extend(qapi_Net_LWM2M_Resource_Info_t ** root, qapi_Net_LWM2M_Obj_Info_t * id_info, qapi_Net_LWM2M_Flat_Data_t ** new_node)
{
  atfwd_result_code_t result = ATFWD_RET_MEMORY_ERR;
  qapi_Net_LWM2M_Resource_Info_t * tmp_res = NULL;

  ATFWD_LOG_MSG_MED("ResInfo_list_extend() in", 0, 0, 0);

  if((root == NULL || id_info == NULL || new_node == NULL || *new_node == NULL) ||
     (!(id_info->obj_mask & QAPI_NET_LWM2M_OBJECT_ID_E) || !(id_info->obj_mask & QAPI_NET_LWM2M_INSTANCE_ID_E) || !(id_info->obj_mask & QAPI_NET_LWM2M_RESOURCE_ID_E))
    )
  {
    ATFWD_LOG_MSG_LOW("invalid input", 0, 0, 0);
    result = ATFWD_RET_PARAM_INPUT_ERR;
    goto end;
  }

  tmp_res = athdl_malloc(sizeof(qapi_Net_LWM2M_Resource_Info_t));
  if(tmp_res == NULL)
  {
    goto end;
  }
  if(id_info->obj_mask & QAPI_NET_LWM2M_RESOURCE_INSTANCE_ID_E)
  {
    tmp_res->type = QAPI_NET_LWM2M_TYPE_MULTIPLE_RESOURCE;
    tmp_res->resource_ID = id_info->res_id;
    tmp_res->next = NULL;
    (*new_node)->id = id_info->res_inst_id;
    if(FALSE == lwm2m_util_FlatData_array_extend(&(tmp_res->value.asChildren.array),
                                                  &(tmp_res->value.asChildren.count),
                                                  new_node)) /*extend res_inst_array*/
    {
      ATFWD_LOG_MSG_LOW("extend res_inst_array error", 0, 0, 0);
      result = ATFWD_RET_INTERNAL_FAILED;
      goto end;
    }
    //*new_node = NULL;
  }
  else
  {
    tmp_res->type = (*new_node)->type;
    tmp_res->resource_ID = id_info->res_id;
    tmp_res->next = NULL;
    switch((*new_node)->type)
    {
      case QAPI_NET_LWM2M_TYPE_INTEGER_E:
      {
        tmp_res->value.asInteger = (*new_node)->value.asInteger;
        break;
      }
      case QAPI_NET_LWM2M_TYPE_BOOLEAN_E:
      {
        tmp_res->value.asBoolean = (*new_node)->value.asBoolean;
        break;
      }
      case QAPI_NET_LWM2M_TYPE_FLOAT_E:
      {
        tmp_res->value.asFloat = (*new_node)->value.asFloat;
        break;
      }
      case QAPI_NET_LWM2M_TYPE_STRING_E:
      case QAPI_NET_LWM2M_TYPE_OPAQUE_E:
      {
        tmp_res->value.asBuffer.length = (*new_node)->value.asBuffer.length;
        tmp_res->value.asBuffer.buffer = (*new_node)->value.asBuffer.buffer;
        (*new_node)->value.asBuffer.buffer = NULL;
        break;
      }
      case QAPI_NET_LWM2M_TYPE_OBJECT_LINK:
      {
        tmp_res->value.asObjLink = (*new_node)->value.asObjLink;
        break;
      }
    }
    athdl_free(*new_node); //buffer has been directly used by tmp_res, so only free new_node itself here
  }

  *root = (qapi_Net_LWM2M_Resource_Info_t *)util_at_list_add((util_at_list_t *)(*root), (util_at_list_t *)tmp_res);
  tmp_res = NULL;
  result = ATFWD_RET_OK;

end:
  if(new_node != NULL && *new_node != NULL)
  {
    lwm2m_util_FlatData_array_free(new_node, 1);
  }
  if(tmp_res != NULL)
  {
    if(tmp_res->type == QAPI_NET_LWM2M_TYPE_MULTIPLE_RESOURCE && tmp_res->value.asChildren.array != NULL)
    {
      //free array memory
      lwm2m_util_FlatData_array_free(&(tmp_res->value.asChildren.array), tmp_res->value.asChildren.count);
      ATFWD_LOG_MSG_LOW("free tmp_res->value.asChildren.array", 0,0,0);
    }
    if(tmp_res->type == QAPI_NET_LWM2M_TYPE_STRING_E || tmp_res->type == QAPI_NET_LWM2M_TYPE_OPAQUE_E)
    {
      athdl_free(tmp_res->value.asBuffer.buffer);
      ATFWD_LOG_MSG_LOW("free tmp_res->value.asBuffer.buffer", 0,0,0);
    }
    athdl_free(tmp_res);
  }
  ATFWD_LOG_MSG_MED("ResInfo_list_extend() out, res %d", (int)result, 0, 0);
  return result;
}
/**
 * @fn atfwd_result_code_t lwm2m_util_InstInfo_list_extend()
 * @brief extend root with one new node.
 * @param
 *     @param[in] qapi_Net_LWM2M_Instance_Info_v2_t ** root
 *     @param[in] qapi_Net_LWM2M_Obj_Info_t * id_info 
 *     @param[in|out] iqapi_Net_LWM2M_Flat_Data_t ** new_node
 * @return int32 AT Handler error
 */
atfwd_result_code_t lwm2m_util_InstInfo_list_extend(qapi_Net_LWM2M_Instance_Info_v2_t ** root, qapi_Net_LWM2M_Obj_Info_t * id_info, qapi_Net_LWM2M_Flat_Data_t ** new_node)
{
  atfwd_result_code_t result = ATFWD_RET_MEMORY_ERR;
  qapi_Net_LWM2M_Instance_Info_v2_t * tmp_inst = NULL;

  ATFWD_LOG_MSG_MED("InstInfo_list_extend() in", 0, 0, 0);

  if((root == NULL || id_info == NULL || new_node == NULL || *new_node == NULL) ||
     (!(id_info->obj_mask & QAPI_NET_LWM2M_OBJECT_ID_E) || !(id_info->obj_mask & QAPI_NET_LWM2M_INSTANCE_ID_E) || !(id_info->obj_mask & QAPI_NET_LWM2M_RESOURCE_ID_E))
    )
  {
    ATFWD_LOG_MSG_LOW("invalid input", 0, 0, 0);
    result = ATFWD_RET_PARAM_INPUT_ERR;
    goto end;
  }

  tmp_inst = athdl_malloc(sizeof(qapi_Net_LWM2M_Instance_Info_v2_t));
  if(tmp_inst == NULL)
  {
    ATFWD_LOG_MSG_LOW("malloc tmp_inst failed", 0, 0, 0);
    goto end;
  }
  tmp_inst->instance_ID = id_info->obj_inst_id;
  tmp_inst->next = NULL;
  tmp_inst->no_resources = 1;
  tmp_inst->resource_info = NULL;
  if(ATFWD_RET_OK != lwm2m_util_ResInfo_list_extend(&(tmp_inst->resource_info), id_info, new_node))
  {
    ATFWD_LOG_MSG_LOW("res extend failed", 0, 0, 0);
    result = ATFWD_RET_INTERNAL_FAILED;
    goto end;
  }
  *root = (qapi_Net_LWM2M_Instance_Info_v2_t *)util_at_list_add((util_at_list_t *)(*root), (util_at_list_t *)tmp_inst);
  tmp_inst = NULL;
  result = ATFWD_RET_OK;
end:
  if(new_node != NULL && *new_node != NULL)
  {
    lwm2m_util_FlatData_array_free(new_node, 1);
  }
  athdl_free(tmp_inst);
  ATFWD_LOG_MSG_MED("InstInfo_list_extend() out, res %d", (int)result, 0, 0);
  return result;
}
/**
 * @fn atfwd_result_code_t lwm2m_util_ObjInfo_list_extend()
 * @brief extend root with one new node.
 * @param
 *     @param[in] qapi_Net_LWM2M_Data_v2_t ** root
 *     @param[in] qapi_Net_LWM2M_Obj_Info_t * id_info 
 *     @param[in|out] iqapi_Net_LWM2M_Flat_Data_t ** new_node
 * @return int32 AT Handler error
 */
atfwd_result_code_t lwm2m_util_ObjInfo_list_extend(qapi_Net_LWM2M_Data_v2_t ** root, qapi_Net_LWM2M_Obj_Info_t * id_info, qapi_Net_LWM2M_Flat_Data_t ** new_node)
{
  atfwd_result_code_t result = ATFWD_RET_MEMORY_ERR;
  qapi_Net_LWM2M_Data_v2_t * tmp_obj = NULL;

  ATFWD_LOG_MSG_MED("ObjInfo_list_extend() in", 0, 0, 0);

  if((root == NULL || id_info == NULL || new_node == NULL || *new_node == NULL) ||
     (!(id_info->obj_mask & QAPI_NET_LWM2M_OBJECT_ID_E) || !(id_info->obj_mask & QAPI_NET_LWM2M_INSTANCE_ID_E) || !(id_info->obj_mask & QAPI_NET_LWM2M_RESOURCE_ID_E))
    )
  {
    ATFWD_LOG_MSG_LOW("invalid input", 0, 0, 0);
    result = ATFWD_RET_PARAM_INPUT_ERR;
    goto end;
  }

  tmp_obj = athdl_malloc(sizeof(qapi_Net_LWM2M_Data_v2_t));
  if(tmp_obj == NULL)
  {
    goto end;
  }
  tmp_obj->object_ID = id_info->obj_id;
  tmp_obj->next = NULL;
  tmp_obj->no_instances = 1;
  tmp_obj->instance_info = NULL;
  if(ATFWD_RET_OK != lwm2m_util_InstInfo_list_extend(&(tmp_obj->instance_info), id_info, new_node))
  {
    ATFWD_LOG_MSG_LOW("ins extend failed", 0, 0, 0);
    result = ATFWD_RET_INTERNAL_FAILED;
    goto end;
  }

  *root = (qapi_Net_LWM2M_Data_v2_t *)util_at_list_add((util_at_list_t *)(*root), (util_at_list_t *)tmp_obj);
  tmp_obj = NULL;
  result = ATFWD_RET_OK;

end:
  if(new_node != NULL && *new_node != NULL)
  {
    lwm2m_util_FlatData_array_free(new_node, 1);
  }
  athdl_free(tmp_obj);
  ATFWD_LOG_MSG_MED("ObjInfo_list_extend() out, res %d", (int)result, 0, 0);
  return result;
}
/**
 * @fn boolean lwm2m_util_ObjInfo_buffer_addlist()
 * @brief prepare buffer and add to inter response list
 * @param
 * @return boolean
 */
boolean lwm2m_util_ObjInfo_buffer_addlist(char * inter_resp_hdr, uint8_t *buffer, size_t length,
                                                        qapi_Net_LWM2M_Value_Type_t type, qapi_Net_LWM2M_ID_t level, uint16_t id,
                                                        int max_resp_len, int * inter_resp_count,
                                                        uint32_t * resp_total_len, util_at_list_t ** resp_hdr)
{
  boolean result = FALSE;
  
  char * inter_resp = NULL; //whole body of each line
  int inter_resp_hdr_len = 0;
  size_t buf_len = 0;
  uint8_t * data_buf=NULL;
  char * split_tmp_ptr = NULL; //used to handle resp split
  size_t other_len = 0;
  util_at_list_t * resp_node = NULL;


  ATFWD_LOG_MSG_MED("ObjInfo_buffer_addlist() in", 0, 0, 0);


  if(inter_resp_hdr == NULL || max_resp_len < ATHDL_RESP_LEN_MIN || inter_resp_count == NULL || resp_total_len == NULL || resp_hdr == NULL)
  {
    ATFWD_LOG_MSG_LOW("invalid input", 0, 0, 0);
    goto end;
  }

  inter_resp_hdr_len = strlen(inter_resp_hdr);
  if(level == QAPI_NET_LWM2M_RESOURCE_ID_E)
  {
    other_len = 10; //The 10 is for {", <value_type>, ""\n\0}
  }
  else if(level == QAPI_NET_LWM2M_RESOURCE_INSTANCE_ID_E)
  {
    other_len = 12; //The 12 is for {/x", <value_type>, ""\n\0}
  }
  else
  {
    ATFWD_LOG_MSG_LOW("invalid level", 0, 0, 0);
    goto end;
  }
  if(FALSE == lwm2m_util_unpack_buffer(&data_buf,&buf_len,buffer,length,type))
  {
    ATFWD_LOG_MSG_LOW("unpack buffer failed", 0, 0, 0);
    goto end;
  }
  buf_len = strlen((char*)data_buf);

  if((inter_resp_hdr_len+buf_len+other_len) > max_resp_len)
  {
    int offset = 0;
    int split_buf_len = 0;
    int count_down = 0;
    
    split_tmp_ptr = athdl_malloc(max_resp_len);
    if(split_tmp_ptr == NULL)
    {
      ATFWD_LOG_MSG_LOW("malloc split_tmp_ptr failed", 0, 0, 0);
      goto end;
    }

    while(buf_len > 0)
    {
      memset(split_tmp_ptr, 0x00, max_resp_len);
      count_down = buf_len / (max_resp_len - inter_resp_hdr_len - other_len - 3);
      if(buf_len > (max_resp_len - inter_resp_hdr_len - other_len - 3)) // 3 for {, 1}
      {
        split_buf_len = (max_resp_len - inter_resp_hdr_len - other_len - 3);
      }
      else
      {
        split_buf_len = buf_len;
      }
      strlcpy(split_tmp_ptr, (char *)(data_buf+offset), split_buf_len + 1);
      offset = offset + strlen(split_tmp_ptr);
      buf_len = buf_len - split_buf_len;
      inter_resp = athdl_malloc(max_resp_len);
      if(inter_resp == NULL)
      {
        ATFWD_LOG_MSG_LOW("malloc inter_resp failed", 0, 0, 0);
        goto end;
      }
      if(buf_len <= 0)
      {
        if(level == QAPI_NET_LWM2M_RESOURCE_ID_E)
        {
          snprintf(inter_resp, max_resp_len, "%s\", %d, \"%s\"\n", inter_resp_hdr, (int)type, split_tmp_ptr); //prepare resp line
        }
        else
        {
          snprintf(inter_resp, max_resp_len, "%s/%d\", %d, \"%s\"\n", inter_resp_hdr, (int)id, (int)type, split_tmp_ptr); //prepare resp line
        }
      }
      else
      {
        if(level == QAPI_NET_LWM2M_RESOURCE_ID_E)
        {
          snprintf(inter_resp, max_resp_len, "%s\", %d, \"%s\", %d\n", inter_resp_hdr, (int)type, split_tmp_ptr, count_down); //prepare resp line
        }
        else
        {
          snprintf(inter_resp, max_resp_len, "%s/%d\", %d, \"%s\", %d\n", inter_resp_hdr, (int)id, (int)type, split_tmp_ptr, count_down); //prepare resp line
        }
      }
      *resp_total_len = *resp_total_len + strlen(inter_resp);
      *inter_resp_count++;
      resp_node = (util_at_list_t *)athdl_malloc(sizeof(util_at_list_t));
      if(resp_node == NULL)
      {
        ATFWD_LOG_MSG_LOW("malloc resp_node failed", 0, 0, 0);
        goto end;
      }
      resp_node->id = (uint16_t)(*inter_resp_count);
      resp_node->ptr = (void *)inter_resp;
      inter_resp = NULL;
      *resp_hdr = (util_at_list_t *)util_at_list_add(*resp_hdr, resp_node); //add into resp list
      resp_node = NULL;
    }
  }
  else
  {
    *inter_resp_count++;
    inter_resp = athdl_malloc(max_resp_len);
    if(inter_resp == NULL)
    {
      ATFWD_LOG_MSG_LOW("malloc inter_resp failed", 0, 0, 0);
      goto end;
    }
    if(level == QAPI_NET_LWM2M_RESOURCE_ID_E)
    {
      snprintf(inter_resp, max_resp_len, "%s\", %d, \"%s\"\n", inter_resp_hdr, (int)type, data_buf); //prepare resp line
    }
    else
    {
      snprintf(inter_resp, max_resp_len, "%s/%d\", %d, \"%s\"\n", inter_resp_hdr, (int)id, (int)type, data_buf); //prepare resp line
    }
    *resp_total_len = *resp_total_len + strlen(inter_resp);
    resp_node = (util_at_list_t *)athdl_malloc(sizeof(util_at_list_t));
    if(resp_node == NULL)
    {
      ATFWD_LOG_MSG_LOW("malloc resp_node failed", 0, 0, 0);
      goto end;
    }
    resp_node->id = (uint16_t)(*inter_resp_count);
    resp_node->ptr = (void *)inter_resp;
    inter_resp = NULL;
    *resp_hdr = (util_at_list_t *)util_at_list_add(*resp_hdr, resp_node); //add into resp list
    resp_node = NULL;
  }
  result = TRUE;
end:
  athdl_free(data_buf);
  athdl_free(split_tmp_ptr);
  athdl_free(inter_resp);
  athdl_free(resp_node);
  ATFWD_LOG_MSG_MED("ObjInfo_buffer_addlist() out, bool res %d", (int)result, 0, 0);
  return result;
}

/**
 * @fn boolean lwm2m_util_ObjInfo_struct_display()
 * @brief send object info with URC
 * @param
 *     @param[in] atcmd_name
 *     @param[in] lwm2m_data
 * @return boolean
 */
boolean lwm2m_util_ObjInfo_struct_display(char * atcmd_name, char ** resp_str, qapi_Net_LWM2M_Data_v2_t * lwm2m_data, int max_resp_len)
{
  qapi_Net_LWM2M_Instance_Info_v2_t *inst_info = NULL;

  char * inter_resp_hdr = NULL;//resp header of each line
  char * inter_resp = NULL; //whole body of each line
  char * resp_tmp_ptr = NULL; //used to built resp_str
  char * internal_tmp_ptr = NULL; //used to handle resp split

  util_at_list_t * resp_hdr = NULL; //resp list header
  util_at_list_t * resp_node = NULL;

  int inter_resp_count = 0;
  uint32_t resp_total_len = 0;

  boolean result = FALSE;

  ATFWD_LOG_MSG_MED("ObjInfo_struct_display() in", 0, 0, 0);

  if(atcmd_name == NULL || lwm2m_data == NULL || *resp_str != NULL || max_resp_len < ATHDL_RESP_LEN_MIN)
  {
    ATFWD_LOG_MSG_LOW("invalid input", 0, 0, 0);
    goto end;
  }

  /* Object information */
  ATFWD_LOG_MSG_LOW("input parameter lwm2m_data with obj_id: %d, ins_num: %d", lwm2m_data->object_ID, lwm2m_data->no_instances, 0);
  ATFWD_LOG_MSG_LOW_SPRINT("atcmd_name: %s, max_resp_len: %d", atcmd_name, max_resp_len, 0);

  max_resp_len++;//increase 1, to give \0 place.
  inter_resp_hdr = athdl_malloc(max_resp_len);
  if(inter_resp_hdr == NULL)
  {
    ATFWD_LOG_MSG_LOW("malloc inter_resp_hdr failed", 0, 0, 0);
    goto end;
  }

  /* Object instance information */
  inst_info = lwm2m_data->instance_info;
  while (inst_info)
  {
    qapi_Net_LWM2M_Resource_Info_t *res_info = NULL;

    /* Object instance resource information */
    res_info = inst_info->resource_info;
    while (res_info)
    {
      //prepare line header
      snprintf(inter_resp_hdr, max_resp_len, "%s:\"%d/%d/%d", atcmd_name, (int)lwm2m_data->object_ID, (int)inst_info->instance_ID, (int)res_info->resource_ID);
      //prepare line body and store to resp_list
      if(res_info->type == QAPI_NET_LWM2M_TYPE_FLOAT_E ||
         res_info->type == QAPI_NET_LWM2M_TYPE_BOOLEAN_E ||
         res_info->type == QAPI_NET_LWM2M_TYPE_INTEGER_E ||
         res_info->type == QAPI_NET_LWM2M_TYPE_OBJECT_LINK)
      {
        inter_resp_count++;
        inter_resp = athdl_malloc(max_resp_len);
        if(inter_resp == NULL)
        {
          ATFWD_LOG_MSG_LOW("malloc inter_resp failed", 0, 0, 0);
          goto end;
        }

        switch (res_info->type)
        {
          case QAPI_NET_LWM2M_TYPE_FLOAT_E:
          {
            if((internal_tmp_ptr = util_at_float_to_string(res_info->value.asFloat)) == NULL)
            {
              ATFWD_LOG_MSG_LOW("float to str failed", 0, 0, 0);
              goto end;
            }
            snprintf(inter_resp, max_resp_len, "%s\", %d, %s\r\n", inter_resp_hdr, (int)res_info->type, internal_tmp_ptr);
            athdl_free(internal_tmp_ptr);
            break;
          }
          case QAPI_NET_LWM2M_TYPE_BOOLEAN_E:
          {
            snprintf(inter_resp, max_resp_len, "%s\", %d, %d\r\n", inter_resp_hdr, (int)res_info->type, (int)res_info->value.asBoolean);
            break;
          }
          case QAPI_NET_LWM2M_TYPE_INTEGER_E:
          {
            snprintf(inter_resp, max_resp_len, "%s\", %d, %lld\r\n", inter_resp_hdr, (int)res_info->type, res_info->value.asInteger);
            break;
          }
          case QAPI_NET_LWM2M_TYPE_OBJECT_LINK:
          {
            uint16_t obj_id = res_info->value.asObjLink >> 16;
            uint16_t inst_id = res_info->value.asObjLink & 0xFFFF;
            snprintf(inter_resp, max_resp_len, "%s\", %d, [%d:%d]\r\n", inter_resp_hdr, (int)res_info->type, obj_id, inst_id);
            break;
          }
        }
        resp_total_len = resp_total_len + strlen(inter_resp);
        resp_node = (util_at_list_t *)athdl_malloc(sizeof(util_at_list_t));
        if(resp_node == NULL)
        {
          ATFWD_LOG_MSG_LOW("malloc resp_node failed", 0, 0, 0);
          goto end;
        }
        resp_node->id = (uint16_t)inter_resp_count;
        resp_node->ptr = (void *)inter_resp;
        resp_hdr = (util_at_list_t *)util_at_list_add(resp_hdr, resp_node);
      }
      else if(res_info->type == QAPI_NET_LWM2M_TYPE_STRING_E ||
              res_info->type == QAPI_NET_LWM2M_TYPE_OPAQUE_E)
      {
        if(FALSE == lwm2m_util_ObjInfo_buffer_addlist(inter_resp_hdr,res_info->value.asBuffer.buffer,res_info->value.asBuffer.length,res_info->type,
                                                      QAPI_NET_LWM2M_RESOURCE_ID_E, 0, max_resp_len, &inter_resp_count, &resp_total_len, &resp_hdr))
        {
          ATFWD_LOG_MSG_LOW("buffer addlist failed", 0, 0, 0);
          goto end;
        }
      }
      else if(res_info->type == QAPI_NET_LWM2M_TYPE_MULTIPLE_RESOURCE)
      {
        uint32_t cntr = 0;
        for (cntr = 0; cntr < res_info->value.asChildren.count; cntr++)
        {
          if(QAPI_NET_LWM2M_TYPE_OBJECT_LINK == res_info->value.asChildren.array[cntr].type ||
             QAPI_NET_LWM2M_TYPE_INTEGER_E == res_info->value.asChildren.array[cntr].type ||
             QAPI_NET_LWM2M_TYPE_BOOLEAN_E == res_info->value.asChildren.array[cntr].type ||
             QAPI_NET_LWM2M_TYPE_FLOAT_E == res_info->value.asChildren.array[cntr].type)
          {
            inter_resp_count++;
            inter_resp = athdl_malloc(max_resp_len);
            if(inter_resp == NULL)
            {
              ATFWD_LOG_MSG_LOW("malloc inter_resp failed", 0, 0, 0);
              goto end;
            }

            switch(res_info->value.asChildren.array[cntr].type)
            {
              case QAPI_NET_LWM2M_TYPE_OBJECT_LINK:
              {
                uint16_t obj_id = res_info->value.asChildren.array[cntr].value.asObjLink >> 16;
                uint16_t inst_id = res_info->value.asChildren.array[cntr].value.asObjLink & 0xFFFF;
                snprintf(inter_resp, max_resp_len, "%s/%d\", %d, [%d:%d]\r\n", inter_resp_hdr, (int)res_info->value.asChildren.array[cntr].id, (int)res_info->value.asChildren.array[cntr].type, obj_id, inst_id);
                break;
              }
              case QAPI_NET_LWM2M_TYPE_INTEGER_E:
              {
                snprintf(inter_resp, max_resp_len, "%s/%d\", %d, %lld\r\n", inter_resp_hdr, (int)res_info->value.asChildren.array[cntr].id, (int)res_info->value.asChildren.array[cntr].type, res_info->value.asChildren.array[cntr].value.asInteger);
                break;
              }
              case QAPI_NET_LWM2M_TYPE_BOOLEAN_E:
              {
                snprintf(inter_resp, max_resp_len, "%s/%d\", %d, %d\r\n", inter_resp_hdr, (int)res_info->value.asChildren.array[cntr].id, (int)res_info->value.asChildren.array[cntr].type, (int)res_info->value.asChildren.array[cntr].value.asBoolean);
                break;
              }
              case QAPI_NET_LWM2M_TYPE_FLOAT_E:
              {
                if((internal_tmp_ptr = util_at_float_to_string(res_info->value.asFloat)) == NULL)
                {
                  ATFWD_LOG_MSG_LOW("float to str failed", 0, 0, 0);
                  goto end;
                }
                snprintf(inter_resp, max_resp_len, "%s/%d\", %d, %s\r\n", inter_resp_hdr, (int)res_info->value.asChildren.array[cntr].id, (int)res_info->value.asChildren.array[cntr].type, internal_tmp_ptr);
                athdl_free(internal_tmp_ptr);
                break;
              }
            }

            resp_total_len = resp_total_len + strlen(inter_resp);
            resp_node = (util_at_list_t *)athdl_malloc(sizeof(util_at_list_t));
            if(resp_node == NULL)
            {
              ATFWD_LOG_MSG_LOW("malloc resp_node failed", 0, 0, 0);
              goto end;
            }
            resp_node->id = (uint16_t)inter_resp_count;
            resp_node->ptr = (void *)inter_resp;
            resp_hdr = (util_at_list_t *)util_at_list_add(resp_hdr, resp_node);
          }
          else if(QAPI_NET_LWM2M_TYPE_STRING_E == res_info->value.asChildren.array[cntr].type ||
                  QAPI_NET_LWM2M_TYPE_OPAQUE_E == res_info->value.asChildren.array[cntr].type)
          {
            if(FALSE == lwm2m_util_ObjInfo_buffer_addlist(inter_resp_hdr,res_info->value.asChildren.array[cntr].value.asBuffer.buffer,
                                                          res_info->value.asChildren.array[cntr].value.asBuffer.length,
                                                          res_info->value.asChildren.array[cntr].type, QAPI_NET_LWM2M_RESOURCE_INSTANCE_ID_E,
                                                          res_info->value.asChildren.array[cntr].id, max_resp_len, &inter_resp_count,&resp_total_len, &resp_hdr))
            {
              ATFWD_LOG_MSG_LOW("buffer addlist failed", 0, 0, 0);
              goto end;
            }
          }
          else
          {
            ATFWD_LOG_MSG_ERROR("ResID %d multi-src type(%d) unsupport", res_info->resource_ID, res_info->value.asChildren.array[cntr].type, 0);
          }
        }
      }
      else
      {
        ATFWD_LOG_MSG_ERROR("ResID %d type(%d) unsupport", res_info->resource_ID, res_info->type, 0);
      }
      res_info = res_info->next;
    }
    inst_info = inst_info->next;
  }
  inter_resp = NULL;
  *resp_str = athdl_malloc(resp_total_len+1);
  if(*resp_str == NULL)
  {
    ATFWD_LOG_MSG_LOW("malloc resp_str failed", 0, 0, 0);
    goto end;
  }
  resp_tmp_ptr = *resp_str;
  resp_node = resp_hdr;
  while(resp_node != NULL)
  {
    snprintf(resp_tmp_ptr, resp_total_len, "%s", (char *)resp_node->ptr);
    resp_tmp_ptr = resp_tmp_ptr + strlen((char *)resp_node->ptr);
    resp_node = resp_node->next;
  }
  result = TRUE;
end:

  athdl_free(inter_resp_hdr);
  if(resp_hdr)
  {
    while(resp_hdr)
    {
      resp_node = resp_hdr;
      resp_hdr = resp_node->next;
      athdl_free(resp_node->ptr);
      athdl_free(resp_node);
    }
  }
  athdl_free(inter_resp);
  athdl_free(internal_tmp_ptr);
  if(result != TRUE)
  {
    if(resp_str)
    {
      athdl_free(*resp_str);
    }
  }
  ATFWD_LOG_MSG_MED("ObjInfo_struct_display() out, bool res %d, max_len: %d", (int)result, max_resp_len, 0);
  return result;
}

/**
 * @fn void lwm2m_util_attr_list_free()
 * @brief free the list
 * @param
 *     @param[in] qapi_Net_LWM2M_Attributes_t * head
 * @return void
 */
void lwm2m_util_attr_list_free(qapi_Net_LWM2M_Attributes_t *head)
{
  qapi_Net_LWM2M_Attributes_t * temp = NULL;
  ATFWD_LOG_MSG_MED("attr_list_free() in", 0, 0, 0);

  while(head != NULL)
  {
    temp = head;
    head = temp->next;
    athdl_free(temp);
  }
  ATFWD_LOG_MSG_MED("attr_list_free() out", 0, 0, 0);
}

/**
 * @fn void lwm2m_util_attr_list_add()
 * @brief add node into list
 * @param
 *     @param[in] data_atfwd_athdl_list_t * head
 *     @param[in] data_atfwd_athdl_list_t * node
 * @return data_atfwd_athdl_list_t * head
 */
boolean lwm2m_util_attr_list_add(qapi_Net_LWM2M_Attributes_t ** head, qapi_Net_LWM2M_Attributes_t ** new_node)
{
  qapi_Net_LWM2M_Attributes_t * temp = NULL;
  boolean result = FALSE;
  ATFWD_LOG_MSG_MED("attr_list_add() in", 0, 0, 0);

  if(NULL == new_node || NULL == *new_node)
  {
    goto end;
  }
  if(*head != NULL)
  {
    temp = *head;
    while(NULL != temp->next)
    {
      temp = temp->next;
    }
    temp->next = *new_node;
  }
  else
  {
    *head = *new_node;
  }
  *new_node = NULL;
  result = TRUE;
end:
  ATFWD_LOG_MSG_MED("attr_list_add() out, bool res %d", (int)result, 0, 0);
  return result;
} 
/**
 * @fn boolean lwm2m_util_attr_list_display()
 * @brief send attribute with URC
 * @param
 *     @param[in] atcmd_name
 *     @param[in] prefix
 *     @param[in] attr_info
 * @return atfwd_result_code_t
 */
atfwd_result_code_t lwm2m_util_attr_list_display(char * atcmd_name, char * prefix, qapi_Net_LWM2M_Attributes_t * attr_info)
{
  atfwd_result_code_t result = ATFWD_RET_PARAM_ERR;
  char * inter_resp = NULL; //whole body of each line

  char attr_buffer[LWM2M_AT_PAYLOAD_MAX_LEN+1] = {0};
  int32_t attr_len = 0;
  qapi_Net_LWM2M_Attributes_t * tmp = attr_info;
  char * temp_buffer = NULL;

  ATFWD_LOG_MSG_MED("attr_list_display() in", 0, 0, 0);

  if(atcmd_name == NULL || prefix == NULL || attr_info == NULL)
  {
    ATFWD_LOG_MSG_LOW("invalid input", 0, 0, 0);
    goto end;
  }

  inter_resp = athdl_malloc(ATHDL_URC_LEN_MAX);
  if(inter_resp == NULL)
  {
    ATFWD_LOG_MSG_LOW("malloc inter_resp failed", 0, 0, 0);
    result = ATFWD_RET_MEMORY_ERR;
    goto end;
  }

  while(tmp != NULL)
  {
    attr_len = 0;
    memset(attr_buffer, 0x0, LWM2M_AT_PAYLOAD_MAX_LEN+1);
    attr_len += snprintf(attr_buffer+attr_len, LWM2M_AT_PAYLOAD_MAX_LEN-attr_len,"\"");

    if(tmp->set_attr_mask & QAPI_NET_LWM2M_DIM_E)
    {
      attr_len += snprintf(attr_buffer+attr_len, LWM2M_AT_PAYLOAD_MAX_LEN-attr_len,"dim=%d;",tmp->dim);
    }
    else if(tmp->clr_attr_mask & QAPI_NET_LWM2M_DIM_E)
    {
      attr_len += snprintf(attr_buffer+attr_len, LWM2M_AT_PAYLOAD_MAX_LEN-attr_len,"dim=;");
    }
    else
    {
      ATFWD_LOG_MSG_LOW("dim avoided", 0, 0, 0);
    }

    if(tmp->set_attr_mask & QAPI_NET_LWM2M_MIN_PERIOD_E)
    {
      attr_len += snprintf(attr_buffer+attr_len, LWM2M_AT_PAYLOAD_MAX_LEN-attr_len,"pmin=%u;",tmp->minPeriod);
    }
    else if(tmp->clr_attr_mask & QAPI_NET_LWM2M_MIN_PERIOD_E)
    {
      attr_len += snprintf(attr_buffer+attr_len, LWM2M_AT_PAYLOAD_MAX_LEN-attr_len,"pmin=;");
    }
    else
    {
      ATFWD_LOG_MSG_LOW("pmin avoided", 0, 0, 0);
    }

    if(tmp->set_attr_mask & QAPI_NET_LWM2M_MAX_PERIOD_E)
    {
      attr_len += snprintf(attr_buffer+attr_len, LWM2M_AT_PAYLOAD_MAX_LEN-attr_len,"pmax=%u;",tmp->maxPeriod);
    }
    else if(tmp->clr_attr_mask & QAPI_NET_LWM2M_MAX_PERIOD_E)
    {
      attr_len += snprintf(attr_buffer+attr_len, LWM2M_AT_PAYLOAD_MAX_LEN-attr_len,"pmax=;");
    }
    else
    {
      ATFWD_LOG_MSG_LOW("pmax avoided", 0, 0, 0);
    }

    if(tmp->set_attr_mask & QAPI_NET_LWM2M_LESS_THAN_E)
    {
      if((temp_buffer = util_at_float_to_string(tmp->lessThan)) == NULL)
      {
        ATFWD_LOG_MSG_LOW("float_to_string fail", 0, 0, 0);
        result = ATFWD_RET_INTERNAL_FAILED;
        goto end;
      }
      attr_len += snprintf(attr_buffer+attr_len, LWM2M_AT_PAYLOAD_MAX_LEN-attr_len,"lt=%s;",temp_buffer);

      athdl_free(temp_buffer);
    }
    else if(tmp->clr_attr_mask & QAPI_NET_LWM2M_LESS_THAN_E)
    {
      attr_len += snprintf(attr_buffer+attr_len, LWM2M_AT_PAYLOAD_MAX_LEN-attr_len,"lt=;");
    }
    else
    {
      ATFWD_LOG_MSG_LOW("lt avoided", 0, 0, 0);
    }

    if(tmp->set_attr_mask & QAPI_NET_LWM2M_GREATER_THAN_E)
    {
      if((temp_buffer = util_at_float_to_string(tmp->greaterThan)) == NULL)
      {
        ATFWD_LOG_MSG_LOW("float_to_string fail", 0, 0, 0);
        result = ATFWD_RET_INTERNAL_FAILED;
        goto end;
      }
      attr_len += snprintf(attr_buffer+attr_len, LWM2M_AT_PAYLOAD_MAX_LEN-attr_len,"gt=%s;",temp_buffer);

      athdl_free(temp_buffer);
    }
    else if(tmp->clr_attr_mask & QAPI_NET_LWM2M_GREATER_THAN_E)
    {
      attr_len += snprintf(attr_buffer+attr_len, LWM2M_AT_PAYLOAD_MAX_LEN-attr_len,"gt=;");
    }
    else
    {
      ATFWD_LOG_MSG_LOW("gt avoided", 0, 0, 0);
    }

    if(tmp->set_attr_mask & QAPI_NET_LWM2M_STEP_E)
    {
      if((temp_buffer = util_at_float_to_string(tmp->step)) == NULL)
      {
        ATFWD_LOG_MSG_LOW("float_to_string fail", 0, 0, 0);
        result = ATFWD_RET_INTERNAL_FAILED;
        goto end;
      }
      attr_len += snprintf(attr_buffer+attr_len, LWM2M_AT_PAYLOAD_MAX_LEN-attr_len,"st=%s;",temp_buffer);

      athdl_free(temp_buffer);
    }
    else if(tmp->clr_attr_mask & QAPI_NET_LWM2M_STEP_E)
    {
      attr_len += snprintf(attr_buffer+attr_len, LWM2M_AT_PAYLOAD_MAX_LEN-attr_len,"st=;");
    }
    else
    {
      ATFWD_LOG_MSG_LOW("st avoided", 0, 0, 0);
    }

    attr_len += snprintf(attr_buffer+attr_len, LWM2M_AT_PAYLOAD_MAX_LEN-attr_len,"\"");

    if((result = lwm2m_idmap_ObjInfo_map(&(tmp->obj_info), LWM2M_IDMAP_MAP_TO_ORG)) != ATFWD_RET_OK)
    {
      ATFWD_LOG_MSG_LOW("ObjInfo map fail", 0, 0, 0);
      goto end;
    }

    if(tmp->obj_info.obj_mask & QAPI_NET_LWM2M_OBJECT_ID_E && tmp->obj_info.obj_mask & QAPI_NET_LWM2M_INSTANCE_ID_E && tmp->obj_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_ID_E && tmp->obj_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_INSTANCE_ID_E)
    {
      snprintf(inter_resp, ATHDL_URC_LEN_MAX, "%s, \"%d/%d/%d/%d\", %s", prefix, tmp->obj_info.obj_id, tmp->obj_info.obj_inst_id, tmp->obj_info.res_id, tmp->obj_info.res_inst_id, attr_buffer);
    }
    else if(tmp->obj_info.obj_mask & QAPI_NET_LWM2M_OBJECT_ID_E && tmp->obj_info.obj_mask & QAPI_NET_LWM2M_INSTANCE_ID_E && tmp->obj_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_ID_E)
    {
      snprintf(inter_resp, ATHDL_URC_LEN_MAX, "%s, \"%d/%d/%d\", %s", prefix, tmp->obj_info.obj_id, tmp->obj_info.obj_inst_id, tmp->obj_info.res_id, attr_buffer);
    }
    else if(tmp->obj_info.obj_mask & QAPI_NET_LWM2M_OBJECT_ID_E && tmp->obj_info.obj_mask & QAPI_NET_LWM2M_INSTANCE_ID_E)
    {
      snprintf(inter_resp, ATHDL_URC_LEN_MAX, "%s, \"%d/%d\", %s", prefix, tmp->obj_info.obj_id, tmp->obj_info.obj_inst_id, attr_buffer);
    } 
    else if(tmp->obj_info.obj_mask & QAPI_NET_LWM2M_OBJECT_ID_E)
    {
      snprintf(inter_resp, ATHDL_URC_LEN_MAX, "%s, \"%d\", %s", prefix, tmp->obj_info.obj_id, attr_buffer);
    }
    else
    {
      ATFWD_LOG_MSG_LOW("obj_mask invalid", 0, 0, 0);
      goto end;
    }

    qapi_atfwd_send_urc_resp(atcmd_name, inter_resp);
    tmp = tmp->next;
  }
  result = ATFWD_RET_OK;
end:
  athdl_free(inter_resp);
  athdl_free(temp_buffer);
  ATFWD_LOG_MSG_MED("attr_list_display() out, res %d", (int)result, 0, 0);
  return result;
}

/**
 * @fn boolean lwm2m_util_req_store_delete()
 * @brief dequeue and free node from the list
 * @param
 *     @param[in] qapi_Net_LWM2M_Obj_Info_t * target_id
 *     @param[in] qapi_Net_LWM2M_DL_Msg_t msg_type
 * @return TRUE/FALSE
 */
boolean lwm2m_util_req_store_delete(qapi_Net_LWM2M_Obj_Info_t * target_id, qapi_Net_LWM2M_DL_Msg_t msg_type)
{
  util_at_list_t * temp = lwm2m_at_req_store_hdr;
  util_at_list_t * former = lwm2m_at_req_store_hdr;
  boolean result = FALSE;
  ATFWD_LOG_MSG_MED("req_store_delete() in", 0, 0, 0);

  if(target_id == NULL || lwm2m_at_req_store_hdr == NULL)
  {
    ATFWD_LOG_MSG_LOW("invalid parameter", 0, 0, 0);
    goto end;
  }

  while(temp != NULL)
  {
    if(temp->ptr != NULL &&
       lwm2m_util_obj_info_check(1, ((lwm2m_at_req_store_t *)temp->ptr)->id_info, target_id) &&
       ((lwm2m_at_req_store_t *)temp->ptr)->msg_type == msg_type)
    {
      if(temp == lwm2m_at_req_store_hdr)
      {
        lwm2m_at_req_store_hdr = temp->next;
      }
      else
      {
        former->next = temp->next;
      }
      ATFWD_LOG_MSG_LOW("free ep with id %d", temp->id, 0, 0);



      athdl_free(((lwm2m_at_req_store_t *)temp->ptr)->id_info);
      athdl_free(temp->ptr);
      athdl_free(temp);
      result = TRUE;
      goto end;
    }
    former = temp;
    temp = temp->next;
  }
  result = FALSE;
end:
  ATFWD_LOG_MSG_MED("req_store_delete() out, bool res %d", (int)result, 0, 0);
  return result;
}
/**
 * @fn lwm2m_at_req_store_t lwm2m_util_req_store_find_by_notifyId()
 * @brief find lwm2m_at_req_store_t * by notification id from lwm2m_at_req_store_hdr
 * @param
 *     @param[in] uint16_t notification_id 
 * @return data_atfwd_LWM2M_Req_Store_t * / NULL
 */
lwm2m_at_req_store_t * lwm2m_util_req_store_find_by_notifyId(uint16_t notification_id)
{
  util_at_list_t * temp = lwm2m_at_req_store_hdr;
  lwm2m_at_req_store_t * match_ep = NULL;

  ATFWD_LOG_MSG_MED("req_store_find_by_notifyId() in", 0, 0, 0);

  while(temp != NULL)
  {
    if(((lwm2m_at_req_store_t *)(temp->ptr))->notification_id == notification_id)
    {
      match_ep = (lwm2m_at_req_store_t *)(temp->ptr);
      goto end;
    }
    temp = temp->next;
  }
end:
  ATFWD_LOG_MSG_MED("req_store_find_by_notifyId() out, match_ep %d", match_ep, 0, 0);
  return match_ep;
}
/**
 * @fn void lwm2m_util_res_list_free()
 * @brief free resource list
 * @param
 *     @param[in] qapi_Net_LWM2M_Resource_Info_t * res_ptr
 * @return N.A
 */
void lwm2m_util_res_list_free(qapi_Net_LWM2M_Resource_Info_t * res_ptr)
{
  qapi_Net_LWM2M_Resource_Info_t * resource_head = NULL;
  ATFWD_LOG_MSG_MED("util_res_list_free() in", 0,0,0);

  if(res_ptr == NULL)
  {
    return;
  }
  resource_head = res_ptr;

  while(resource_head != NULL)
  {
    qapi_Net_LWM2M_Resource_Info_t * tmp_res_ptr = resource_head;
    resource_head = tmp_res_ptr->next;
    ATFWD_LOG_MSG_LOW("util_res_list_free() freeing resource id[%d]", tmp_res_ptr->resource_ID,0,0);

    if(tmp_res_ptr->type == QAPI_NET_LWM2M_TYPE_MULTIPLE_RESOURCE && tmp_res_ptr->value.asChildren.array != NULL)
    {
      //free array memory
      lwm2m_util_FlatData_array_free(&(tmp_res_ptr->value.asChildren.array), tmp_res_ptr->value.asChildren.count);
    }
    if(tmp_res_ptr->type == QAPI_NET_LWM2M_TYPE_STRING_E)
    {
      athdl_free(tmp_res_ptr->value.asBuffer.buffer);
    }
    athdl_free(tmp_res_ptr);
  }

  ATFWD_LOG_MSG_MED("util_res_list_free() out", 0, 0, 0);
}
/**
 * @fn void lwm2m_util_inst_list_free()
 * @brief free instance list
 * @param
 *     @param[in] qapi_Net_LWM2M_Instance_Info_v2_t * inst_ptr
 * @return N.A
 */
void lwm2m_util_inst_list_free(qapi_Net_LWM2M_Instance_Info_v2_t * inst_ptr)
{
  qapi_Net_LWM2M_Instance_Info_v2_t *instance_head = NULL;
  ATFWD_LOG_MSG_MED("util_inst_list_free() in", 0,0,0);

  if(inst_ptr == NULL)
  {
    return;
  }
  instance_head = inst_ptr;

  while(instance_head != NULL)
  {
    qapi_Net_LWM2M_Instance_Info_v2_t * tmp_inst_ptr = instance_head;
    instance_head = tmp_inst_ptr->next;
    ATFWD_LOG_MSG_LOW("util_inst_list_free() freeing instance id[%d]", tmp_inst_ptr->instance_ID,0,0);
    lwm2m_util_res_list_free(tmp_inst_ptr->resource_info);
    athdl_free(tmp_inst_ptr);
  }

  ATFWD_LOG_MSG_MED("util_inst_list_free() out", 0, 0, 0);
}
/**
 * @fn void lwm2m_util_obj_list_free()
 * @brief free instance list
 * @param
 *     @param[in] qapi_Net_LWM2M_Data_v2_t * obj_ptr
 * @return N.A
 */
void lwm2m_util_obj_list_free(qapi_Net_LWM2M_Data_v2_t * obj_ptr)
{
  qapi_Net_LWM2M_Data_v2_t * obj_head = NULL;
  ATFWD_LOG_MSG_MED("util_obj_list_free() in", 0,0,0);

  if(obj_ptr == NULL)
  {
    return;
  }
  obj_head = obj_ptr;

  while(obj_head != NULL)
  {
    qapi_Net_LWM2M_Data_v2_t * tmp_obj_ptr = obj_head;
    obj_head = tmp_obj_ptr->next;
    ATFWD_LOG_MSG_LOW("util_obj_list_free() freeing object id[%d]", tmp_obj_ptr->object_ID,0,0);
    lwm2m_util_inst_list_free(tmp_obj_ptr->instance_info);
    athdl_free(tmp_obj_ptr);
  }

  ATFWD_LOG_MSG_MED("util_obj_list_free() out", 0, 0, 0);
}

/**
 * @fn void lwm2m_util_m2mset_data_free()
 * @brief free lwm2m_at_M2MSET_t data
 * @param
 *     @param[in] m2mset_data_ptr
 * @return N.A
 */
void lwm2m_util_m2mset_data_free(lwm2m_at_M2MSET_t ** m2mset_data_ptr)
{
  ATFWD_LOG_MSG_MED("m2mset_data_free() in", 0,0,0);

  if(m2mset_data_ptr == NULL || *m2mset_data_ptr == NULL)
  {
    goto end;
  }
  lwm2m_util_obj_list_free((*m2mset_data_ptr)->obj_data_ptr);
  athdl_free(*m2mset_data_ptr);
end:
  ATFWD_LOG_MSG_MED("m2mset_data_free() out", 0, 0, 0);
}
/**
 * @fn void lwm2m_util_m2msetmsgdata_free()
 * @brief free lwm2m_at_M2MSETMSGDATA_t input
 * @param
 *     @param[in] lwm2m_at_M2MSETMSGDATA_t ** input_ptr
 * @return N.A
 */
void lwm2m_util_m2msetmsgdata_free(lwm2m_at_M2MSETMSGDATA_t ** input_ptr)
{
  ATFWD_LOG_MSG_MED("m2msetmsgdata_free() in", 0,0,0);

  if(input_ptr == NULL || *input_ptr == NULL)
  {
    goto end;
  }
  lwm2m_util_FlatData_array_free(&((*input_ptr)->lwm2m_flat_data), 1);
  lwm2m_util_attr_list_free((*input_ptr)->lwm2m_attr);
  athdl_free(*input_ptr);
end:
  ATFWD_LOG_MSG_MED("m2msetmsgdata_free() out", 0, 0, 0);
}
/**
 * @fn boolean lwm2m_util_req_store_free()
 * @brief dequeue and free the req store list
 * @param
 *     @param[in] util_at_list_t ** req_store_hdr
 * @return N.A
 */
void lwm2m_util_req_store_free(util_at_list_t ** req_store_hdr)
{

  util_at_list_t * tmp = NULL;

  ATFWD_LOG_MSG_MED("req_store_free() in", 0, 0, 0);

  if(req_store_hdr == NULL || *req_store_hdr == NULL)
  {
    goto end;
  }

  while(*req_store_hdr != NULL)
  {
    tmp = *req_store_hdr;
    (*req_store_hdr) = tmp->next;
    if(tmp->ptr != NULL)
    {
      ATFWD_LOG_MSG_LOW("free ep with id %d", tmp->id, 0, 0);
      athdl_free(((lwm2m_at_req_store_t *)tmp->ptr)->id_info);
      athdl_free(tmp->ptr);
    }
    athdl_free(tmp);
  }
end:
  ATFWD_LOG_MSG_MED("req_store_free() out",0, 0, 0);
}
/**
 * @fn boolean lwm2m_util_error_code_map()
 * @brief map error to response code
 * @param
 *     @param[in] atfwd_result_code_t input_result
 * @return qapi_Net_LWM2M_Response_Code_t
 */
qapi_Net_LWM2M_Response_Code_t lwm2m_util_error_code_map(atfwd_result_code_t input_result)
{
  qapi_Net_LWM2M_Response_Code_t status_code = QAPI_NET_LWM2M_500_INTERNAL_SERVER_E;

  ATFWD_LOG_MSG_MED("error_code_map() in", 0, 0, 0);
  switch(input_result)
  {
    case ATFWD_RET_QAPI_FAILED: //payload deocde error
    case ATFWD_RET_PARAM_ERR: //Uri error
    case ATFWD_RET_UNSUPPORTIVE: //Req type not support
    {
      status_code = QAPI_NET_LWM2M_400_BAD_REQUEST_E;
      break;
    }
    case ATFWD_RET_EXIST: //ID map failure
    {
      status_code = QAPI_NET_LWM2M_404_NOT_FOUND_E;
      break;
    }
    default:
    {
      break;
    }
  }
  ATFWD_LOG_MSG_MED("error_code_map() out",0, 0, 0);
  return status_code;
}
/**
 * @fn boolean send_error_response()
 * @brief internally send error response
 * @param
 *     @param[in] qapi_Net_LWM2M_Response_Code_t status_code
 *     @param[in] uint8_t msg_id_len
 *     @param[in] uint8_t * msg_id
 *     @param[in] qapi_Net_LWM2M_Obj_Info_t * id_info
 * @return N.A
 */
void lwm2m_util_send_error_response(qapi_Net_LWM2M_App_Handler_t hdl, qapi_Net_LWM2M_Response_Code_t status_code, uint8_t msg_id_len, uint8_t * msg_id, qapi_Net_LWM2M_Obj_Info_t * id_info)
{
  qapi_Net_LWM2M_App_Ex_Obj_Data_t *lwm2m_app_data = NULL;
  qapi_Status_t lwm2m_ret = QAPI_OK;

  ATFWD_LOG_MSG_MED("send_error_response() in", 0, 0, 0);

  /*Send message*/
  //Malloc lwm2m_app_data
  if((lwm2m_app_data = athdl_malloc(sizeof(qapi_Net_LWM2M_App_Ex_Obj_Data_t))) == NULL)
  {
    ATFWD_LOG_MSG_LOW("lwm2m_app_data malloc failed", 0, 0, 0);
    goto end;
  }
  
  lwm2m_app_data->msg_type = QAPI_NET_LWM2M_RESPONSE_MSG_E;
  lwm2m_app_data->status_code = status_code;
  lwm2m_app_data->msg_id_len = msg_id_len;
  lwm2m_util_msgid_copy(msg_id, lwm2m_app_data->msg_id, msg_id_len);
  memscpy(&(lwm2m_app_data->obj_info), sizeof(qapi_Net_LWM2M_Obj_Info_t), id_info, sizeof(qapi_Net_LWM2M_Obj_Info_t));
  
  //Call QAPIqapi_Net_LWM2M_Send_Message
  lwm2m_ret = qapi_Net_LWM2M_Send_Message(hdl, lwm2m_app_data);
  if(lwm2m_ret != QAPI_OK)
  {
    ATFWD_LOG_MSG_ERROR("QAPI Send fail, err %d", (int)lwm2m_ret, 0, 0);
  }
  else
  {
    ATFWD_LOG_MSG_HIGH("err_resp: status send success ", 0, 0, 0);
  }
end:
  athdl_free(lwm2m_app_data);
  ATFWD_LOG_MSG_MED("send_error_response() out", 0, 0, 0);
}
/**
 * @fn void lwm2m_util_extreq_errhdl()
 * @brief send 404 not found response
 * @param
 *     @param[in] qapi_Net_LWM2M_App_Handler_t hdl
 *     @param[in] uint16 req_id
 *     @param[in] atfwd_result_code_t input_result
 * @return atfwd_result_code_t
 */
void lwm2m_util_extreq_errhdl(qapi_Net_LWM2M_App_Handler_t hdl, uint16 req_id, atfwd_result_code_t input_result)
{
  util_at_list_t * req_match_node = NULL;
  lwm2m_at_req_store_t * req_store_data = NULL;

  ATFWD_LOG_MSG_MED("lwm2m_util_extreq_errhdl() in", 0,0,0);

  if(hdl == NULL || input_result == ATFWD_RET_OK)
  {
    ATFWD_LOG_MSG_ERROR("errhdl:hndl mismatch", 0, 0, 0);
    goto end;
  }

  //Find match req_id record
  if((req_match_node = util_at_list_find(lwm2m_at_req_store_hdr, req_id)) == NULL)
  {
    ATFWD_LOG_MSG_ERROR("errhdl:no matched req_id", 0, 0, 0);
    goto end;
  }
  req_store_data = (lwm2m_at_req_store_t *)(req_match_node->ptr);
  if(req_store_data == NULL || req_store_data->id_info == NULL)
  {
    ATFWD_LOG_MSG_ERROR("errhdl: record invalid", 0, 0, 0);
    goto end;
  }

  lwm2m_util_send_error_response(hdl, lwm2m_util_error_code_map(input_result), req_store_data->msg_id_len, req_store_data->msg_id, req_store_data->id_info);
  lwm2m_util_req_store_delete(req_store_data->id_info, req_store_data->msg_type);

end:
  ATFWD_LOG_MSG_MED("lwm2m_util_extreq_errhdl() out",0,0,0);
}

/**
 * @fn void util_at_string_to_Double()
 * @brief this function convert string into double
 * @param 
 *     @param[in] String_Buffer pointer to beginning of string
 *     @param[out] double_Value 
 * @return TRUE on success FALSE on failure
 */
boolean util_at_string_to_Double(const uint8 *buffer, double *dataP)
{
  if (buffer == NULL || dataP == NULL) return FALSE;

  return (boolean)utils_plainTextToFloat64((uint8_t *)buffer,strlen((char *)buffer),dataP);
}
/**
 * @fn char * util_at_float_to_string()
 * @brief convert float into string
 * @param
 *     @param[in] double input
 * @return boolean
 */
char * util_at_float_to_string(double input)
{
  uint8_t * tmp = NULL;
  char * resp = NULL;
  int len = 0;

  len = utils_float64ToPlainText(input, &tmp);
  if(tmp == NULL || len == 0)
  {
    return NULL;
  }
  if((resp = athdl_malloc(len + 1)) == NULL)
  {
    return NULL;
  }
  strlcpy(resp, (char *)tmp, len + 1);
  athdl_free(tmp);
  return resp;
}

#endif /*LWM2M_AT_ENABLE*/

