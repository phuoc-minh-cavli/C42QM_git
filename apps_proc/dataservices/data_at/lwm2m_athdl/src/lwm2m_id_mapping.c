/******************************************************************************

                        Lwm2m_id_mapping . C 

******************************************************************************/

/******************************************************************************

  @file    lwm2m_id_mapping.c
  @brief   lwm2m ID mapping functionsy

  DESCRIPTION
  Utility functions support the handler of QCT native lwM2M AT commands

  ---------------------------------------------------------------------------
  Copyright (c) 2017 - 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------

******************************************************************************/
#ifdef LWM2M_AT_ENABLE
/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "lwm2m_id_mapping.h"
#include "at_handler_utility.h"
#include "lwm2m_at_handler.h"
#include "fs_public.h"


/*===========================================================================

                           Global Variables

===========================================================================*/
lwm2m_idmap_ep_t * lwm2m_idmap_map_hdr = NULL;
int lwm2m_idmap_write_fd = -1;

/*===========================================================================

                           Functions

===========================================================================*/


int lwm2m_data_getline(char **lineptr, size_t *n, int stream) 
{
    char *bufptr = NULL;
    char *p = bufptr;
    size_t size;
    char c;
	int len=0;

    if (lineptr == NULL || stream < NIL || n == NULL ) 
	{
    	return -1;
    }

    bufptr = *lineptr;
    size = *n;

    len = efs_read(stream, &c, 1);
    if ( len == 0) {
    	return -1;
    }
    if (bufptr == NULL) 
	{
    	bufptr = athdl_malloc(128);
    	if (bufptr == NULL) 
		{
    		return -1;
    	}
    	size = 128;
    }
    p = bufptr;
    while(len != 0) 
	{
    	if ((p - bufptr) > (size - 1)) 
		{
    		size = size + 128;
    		bufptr = realloc(bufptr, size);
    		if (bufptr == NULL) {
    			return -1;
    		}
            p = bufptr;
    	}
		if ( c != '\r' )
		{
	    	*p++ = c;
		}
    	if (c == '\n') 
		{
    		break;
    	}
    	len = efs_read(stream, &c, 1);
    }

    *p++ = '\0';
    *lineptr = bufptr;
    *n = size;

    return p - bufptr - 1;
}

/**
 * @fn void lwm2m_idmap_ep_free()
 * @brief free endpoint list
 * @param
 *     @param[in]  lwm2m_idmap_ep_t * header
 * @return N.A
 */
void lwm2m_idmap_ep_free(lwm2m_idmap_ep_t * header)
{
  lwm2m_idmap_ep_t * tmp_ptr = NULL;

  ATFWD_LOG_MSG_MED("ep_free() in, hdr type %d", ((header == NULL)? -1 : header->id_type),0,0);

  while(header != NULL)
  {
    tmp_ptr = header;
    header = tmp_ptr->next;
    if(tmp_ptr->sub_hdr != NULL)
    {
      lwm2m_idmap_ep_free(tmp_ptr->sub_hdr);
    }
    athdl_free(tmp_ptr);
  }
  ATFWD_LOG_MSG_MED("ep_free() out",0,0,0);
}

/**
 * @fn lwm2m_idmap_ep_t * lwm2m_idmap_ep_find()
 * @brief find endpoint by id
 * @param
 *     @param[in]  lwm2m_idmap_ep_t * header
 *     @param[in]  uint16_t id
 *     @param[in]  boolean direct
 * @return pointer to match_ep
 */
lwm2m_idmap_ep_t * lwm2m_idmap_ep_find(lwm2m_idmap_ep_t * header, uint16_t id, boolean direct)
{
  lwm2m_idmap_ep_t * temp = header;
  lwm2m_idmap_ep_t * match_ep = NULL;

  ATFWD_LOG_MSG_MED("ep_find() in, id %d, dirt %d", id, (int)direct, 0);

  while(temp != NULL)
  {
    if((direct == LWM2M_IDMAP_ORG_TO_MAP && temp->org_id == id) || (direct == LWM2M_IDMAP_MAP_TO_ORG && temp->map_id == id))
    {
      match_ep = temp;
    }
    temp = temp->next;
  }

  ATFWD_LOG_MSG_MED("ep_find() out, match_ep %d",match_ep,0,0);
  return match_ep;
}
/**
 * @fn atfwd_result_code_t lwm2m_idmap_id_map()
 * @brief do id mapping
 * @param
 *     @param[in]  lwm2m_idmap_ep_t * header
 *     @param[in/out]  uint16_t * id
 *     @param[in]  boolean direct
 * @return atfwd_result_code_t
 */
atfwd_result_code_t lwm2m_idmap_id_map(lwm2m_idmap_ep_t * header, uint16_t * in_id, boolean direct)
{
  lwm2m_idmap_ep_t * match_ep = NULL;

  atfwd_result_code_t result = ATFWD_RET_PARAM_INPUT_ERR;

  ATFWD_LOG_MSG_MED("id_map() in",0,0,0);

  if(in_id == NULL)
  {
    goto end;
  }

  if((match_ep = lwm2m_idmap_ep_find(header, *in_id, direct)) != NULL)
  {
    ATFWD_LOG_MSG_HIGH("mapping type %d in_id %d to direct %d", header->id_type, *in_id, (int)direct);
    *in_id = (direct == LWM2M_IDMAP_ORG_TO_MAP? match_ep->map_id: match_ep->org_id);
  }
  else
  {
    if(NULL != lwm2m_idmap_ep_find(header, *in_id, !direct))
    {
      ATFWD_LOG_MSG_ERROR("map type %d id %d to direct %d conflict", header->id_type, *in_id, (int)direct);
      result = ATFWD_RET_EXIST;
      goto end;
    }
  }
  result = ATFWD_RET_OK;

end:
  ATFWD_LOG_MSG_MED("id_map() out, res %d", (int)result,0,0);
  return result;
}
/**
 * @fn atfwd_result_code_t lwm2m_idmap_objid_map()
 * @brief map object id
 * @param
 *     @param[in/out]  uint16_t * id
 *     @param[in]  boolean direct
 * @return atfwd_result_code_t
 */
atfwd_result_code_t lwm2m_idmap_ObjId_map(uint16_t * in_id, boolean direct)
{
  atfwd_result_code_t result = ATFWD_RET_BASE;

  ATFWD_LOG_MSG_MED("objid_map() in", 0, 0, 0);

  result = lwm2m_idmap_id_map(lwm2m_idmap_map_hdr, in_id, direct);

  ATFWD_LOG_MSG_MED("objid_map() out, res %d", (int)result,0,0);
  return result;
}
/**
 * @fn atfwd_result_code_t lwm2m_idmap_objinfo_map()
 * @brief map qapi_Net_LWM2M_Obj_Info_t ids
 * @param
 *     @param[in/out]  qapi_Net_LWM2M_Obj_Info_t * in_ptr
 *     @param[in]  boolean direct
 * @return atfwd_result_code_t
 */
atfwd_result_code_t lwm2m_idmap_ObjInfo_map(qapi_Net_LWM2M_Obj_Info_t * in_ptr, boolean direct)
{
  lwm2m_idmap_ep_t * match_ep = NULL;

  atfwd_result_code_t result = ATFWD_RET_PARAM_INPUT_ERR;

  ATFWD_LOG_MSG_MED("objinfo_map() in, info mask %d", ((in_ptr == NULL)? -1 : in_ptr->obj_mask), 0, 0);

  if(in_ptr == NULL || !(in_ptr->obj_mask & QAPI_NET_LWM2M_OBJECT_ID_E))
  {
    goto end;
  }

  match_ep = lwm2m_idmap_ep_find(lwm2m_idmap_map_hdr, in_ptr->obj_id, direct);

  if((result = lwm2m_idmap_ObjId_map(&(in_ptr->obj_id), direct)) != ATFWD_RET_OK)
  {
    goto end;
  }

  if(in_ptr->obj_mask & QAPI_NET_LWM2M_RESOURCE_ID_E && match_ep != NULL)
  {
    if((result = lwm2m_idmap_id_map(match_ep->sub_hdr, &(in_ptr->res_id), direct)) != ATFWD_RET_OK)
    {
      goto end;
    }
  }

end:
  ATFWD_LOG_MSG_MED("objinfo_map() out, res %d", (int)result,0,0);
  return result;
}
/*
 * @fn atfwd_result_code_t lwm2m_idmap_FlatData_map()
 * @brief map qapi_Net_LWM2M_Flat_Data_t array's ids
 * @param
 *     @param[in/out]  qapi_Net_LWM2M_Flat_Data_t * in_ptr
 *     @param[in]  size_t in_len
 *     @param[in]  uint16 obj_id
 *     @param[in]  boolean direct
 * @return atfwd_result_code_t
*/
atfwd_result_code_t lwm2m_idmap_FlatData_map(qapi_Net_LWM2M_Flat_Data_t * in_ptr, size_t in_len, uint16 obj_id, boolean direct)
{
  lwm2m_idmap_ep_t * match_ep = NULL;
  uint32_t i = 0;

  atfwd_result_code_t result = ATFWD_RET_PARAM_INPUT_ERR;

  ATFWD_LOG_MSG_MED("FlatData_map() in, obj_id %d, in_len %d", obj_id, in_len, 0);

  if(in_ptr == NULL)
  {
    goto end;
  }

  if((match_ep = lwm2m_idmap_ep_find(lwm2m_idmap_map_hdr, obj_id, direct)) == NULL)
  {
    result = ATFWD_RET_OK;
    goto end;
  }

  for(i = 0; i < in_len; i++)
  {
    if(in_ptr[i].type == QAPI_NET_LWM2M_TYPE_OBJECT || in_ptr[i].type == QAPI_NET_LWM2M_TYPE_OBJECT_INSTANCE)
    {
      result = ATFWD_RET_LOCAL_UNSUPPORTIVE;
      goto end;
    }
    if((result = lwm2m_idmap_id_map(match_ep->sub_hdr, &(in_ptr[i].id), direct)) != ATFWD_RET_OK)
    {
      goto end;
    }
  }
  result = ATFWD_RET_OK;
end:
  ATFWD_LOG_MSG_MED("FlatData_map() out, res %d", (int)result,0,0);
  return result;
} 

/**
 * @fn atfwd_result_code_t lwm2m_idmap_parse_line()
 * @brief parse input string line into idmap_ep struct, string line have format as [org_objid - map_objid: org_resid1 - map_resid1, org_resid2 - map_resid2, ...], square brackets not include
 * @param
 *     @param[in]  char * in_line
 *     @param[out] lwm2m_idmap_ep_t ** out_hdr
 * @return atfwd_result_code_t
 */
atfwd_result_code_t lwm2m_idmap_parse_line(char * in_line, lwm2m_idmap_ep_t * out_ptr)
{
  char * tmp_str = NULL;
  char * res_str = NULL;
  char * obj_str = NULL;
  char * id_str = NULL;
  int32 id_value = 0;
  lwm2m_idmap_ep_t ** tmp_hdr = NULL;
  lwm2m_idmap_ep_t * tmp_ptr = NULL;

  atfwd_result_code_t result = ATFWD_RET_PARAM_INPUT_ERR;

  ATFWD_LOG_MSG_MED("parse_line() in",0,0,0);

  if(in_line == NULL || out_ptr == NULL || out_ptr->id_type != QAPI_NET_LWM2M_OBJECT_ID_E || out_ptr->sub_hdr != NULL)
  {
    goto end;
  }

  tmp_str = in_line;
  obj_str = strtok_r(tmp_str, ":", &tmp_str);

  if(obj_str)
  {
    id_str = strtok_r(obj_str, "-", &obj_str);
    if(id_str == NULL || FALSE == util_at_string_to_Integer((uint8_t *)id_str, &id_value) || id_value < 0 || id_value > 65535 ||LWM2M_AT_IS_STANDARD_OBJECT(id_value))
    {
      ATFWD_LOG_MSG_LOW("str to int fail at obj_str", 0, 0, 0);
      result = ATFWD_RET_PARAM_OBJ_ERR;
      goto end;
    }
    out_ptr->org_id = (uint16_t)id_value;
    if(obj_str == NULL || FALSE == util_at_string_to_Integer((uint8_t *)obj_str, &id_value) || id_value < 0 || id_value > 65535||LWM2M_AT_IS_STANDARD_OBJECT(id_value))
    {
      ATFWD_LOG_MSG_LOW("str to int fail at obj_str", 0, 0, 0);
      result = ATFWD_RET_PARAM_OBJ_ERR;
      goto end;
    }
    out_ptr->map_id = (uint16_t)id_value;
    ATFWD_LOG_MSG_LOW("obj org_id %d, map_id %d", out_ptr->org_id, out_ptr->map_id, 0);
  }
  else
  {
    goto end;
  }

  tmp_hdr = &(out_ptr->sub_hdr);

  res_str = strtok_r(tmp_str, ",",&tmp_str);
  while(res_str)
  {
    tmp_ptr = (lwm2m_idmap_ep_t *)athdl_malloc(sizeof(lwm2m_idmap_ep_t));
    if(tmp_ptr == NULL)
    {
      result = ATFWD_RET_MEMORY_ERR;
      goto end;
    }
    tmp_ptr->id_type = QAPI_NET_LWM2M_RESOURCE_ID_E;

    id_str = strtok_r(res_str, "-", &res_str);
    if(id_str == NULL || FALSE == util_at_string_to_Integer((uint8_t *)id_str, &id_value) || id_value < 0 || id_value > 65535)
    {
      ATFWD_LOG_MSG_LOW("str to int fail at res_str", 0, 0, 0);
      result = ATFWD_RET_PARAM_RES_ERR;
      goto end;
    }
    tmp_ptr->org_id = (uint16_t)id_value;
    if(res_str == NULL || FALSE == util_at_string_to_Integer((uint8_t *)res_str, &id_value) || id_value < 0 || id_value > 65535)
    {
      ATFWD_LOG_MSG_LOW("str to int fail at res_str", 0, 0, 0);
      result = ATFWD_RET_PARAM_RES_ERR;
      goto end;
    }
    tmp_ptr->map_id = (uint16_t)id_value;
    ATFWD_LOG_MSG_LOW("res org_id %d, map_id %d", tmp_ptr->org_id, tmp_ptr->map_id, 0);

    *tmp_hdr = (lwm2m_idmap_ep_t *)util_at_list_add((util_at_list_t *)(*tmp_hdr), (util_at_list_t *)tmp_ptr);
    tmp_ptr = NULL;

    res_str = strtok_r(tmp_str, ",",&tmp_str);
  }
  result = ATFWD_RET_OK;

end:
  if(tmp_ptr)
  {
    lwm2m_idmap_ep_free(tmp_ptr);
    tmp_ptr = NULL;
  }
  ATFWD_LOG_MSG_MED("parse_line() out, res %d, in_line size %d",(int)result,(int)((in_line == NULL)? 0 : strlen(in_line)),0);
  return result;
}

/**
 * @fn atfwd_result_code_t lwm2m_idmap_efs_read()
 * @brief read MAP file into memory
 * @param
 * @return atfwd_result_code_t
 */
atfwd_result_code_t lwm2m_idmap_efs_read()
{
  char * line = NULL;
  size_t line_len = LWM2M_IDMAP_MAX_LINE + 1;
  int fd = -1;
  int read_len = 0;
  lwm2m_idmap_ep_t * tmp_ptr = NULL;

  atfwd_result_code_t result = ATFWD_RET_MEMORY_ERR;

  ATFWD_LOG_MSG_MED("efs_read() in", 0,0,0);

  if(lwm2m_idmap_map_hdr != NULL)
  {
    result= ATFWD_RET_GLOBAL_INVALID;
    goto end;
  }

  line = athdl_malloc(line_len);
  if(line == NULL)
  {
    goto end;
  }

  fd = efs_open(LWM2M_IDMAP_MAP_FILE, O_RDONLY);
  if(fd > -1)
  {
    while((read_len = lwm2m_data_getline(&line, &line_len, fd)) != -1)
    {
      if(*line == '\0' || *line == '\n')
      {
        ATFWD_LOG_MSG_LOW("data_getline() return empty line", 0,0,0);
        continue;
      }
      if((int)(*(line + read_len - 1)) == '\n')
      {
        *(line + read_len - 1) = '\0';
      }
      tmp_ptr = (lwm2m_idmap_ep_t *)athdl_malloc(sizeof(lwm2m_idmap_ep_t));
      if(tmp_ptr == NULL)
      {
        goto end;
      }
      tmp_ptr->id_type = QAPI_NET_LWM2M_OBJECT_ID_E;
      if((result = lwm2m_idmap_parse_line(line, tmp_ptr)) != ATFWD_RET_OK)
      {
        goto end;
      }
      lwm2m_idmap_map_hdr = (lwm2m_idmap_ep_t *)util_at_list_add((util_at_list_t *)(lwm2m_idmap_map_hdr), (util_at_list_t *)tmp_ptr);
      tmp_ptr = NULL;
    }
  }
  else
  {
    ATFWD_LOG_MSG_HIGH("Warn: no map file", 0,0,0);
  }
  result = ATFWD_RET_OK;
end:
  if(result != ATFWD_RET_OK && lwm2m_idmap_map_hdr != NULL)
  {
    lwm2m_idmap_ep_free(lwm2m_idmap_map_hdr);
    lwm2m_idmap_map_hdr = NULL;
  }
  if(tmp_ptr)
  {
    lwm2m_idmap_ep_free(tmp_ptr);
    tmp_ptr = NULL;
  }
  athdl_free(line);

  ATFWD_LOG_MSG_MED("efs_read() out, res %d",(int)result,0,0);
  return result;
}
/**
 * @fn atfwd_result_code_t lwm2m_idmap_efs_writeline()
 * @brief wirte one line into MAP file
 * @param
 *     @param[in] char * buffer
 *     @param[in] boolean is_end
 * @return atfwd_result_code_t
 */
atfwd_result_code_t lwm2m_idmap_efs_writeline(uint8 * buffer, size_t len, boolean is_end)
{
  uint8 * tmp_buffer = NULL;
  int buf_len = 0;
  atfwd_result_code_t result = ATFWD_RET_BASE;

  ATFWD_LOG_MSG_MED("efs_writeline() in, fd %d, len %d, is_end %d", lwm2m_idmap_write_fd, (int)len, (int)is_end);

  if(buffer == NULL || len > LWM2M_IDMAP_MAX_LINE)
  {
    result = ATFWD_RET_PARAM_INPUT_ERR;
    goto end;
  }

  if(lwm2m_idmap_write_fd == -1)
  {
    lwm2m_idmap_write_fd = efs_open(LWM2M_IDMAP_MAP_FILE, O_WRONLY|O_CREAT|O_TRUNC);
  }

  buf_len = len + 1;
  tmp_buffer = athdl_malloc(buf_len);
  if(tmp_buffer == NULL)
  {
    result = ATFWD_RET_MEMORY_ERR;
    goto end;
  }
  snprintf((char *)tmp_buffer, buf_len, "%s", (char *)buffer);
  tmp_buffer[len] = '\n';

  if(lwm2m_idmap_write_fd <= -1 || buf_len != efs_write(lwm2m_idmap_write_fd, tmp_buffer, buf_len))
  {
    result = ATFWD_RET_FAILED;
    goto end;
  }
  result = ATFWD_RET_OK;

end:
  if(is_end || result != ATFWD_RET_OK)
  {
    efs_close(lwm2m_idmap_write_fd);
    lwm2m_idmap_write_fd = -1;
  }
  athdl_free(tmp_buffer);
  ATFWD_LOG_MSG_MED("efs_writeline() out, res %d",(int)result,0,0);
  return result;
}

#endif /*LWM2M_AT_ENABLE*/
