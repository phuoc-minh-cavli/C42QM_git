/**************************************************************************************************************
 * Copyright © 2018-2020 Qualcomm Technologies, Inc.
 * @file  utils_app_v2.c
 * @author  Qualcomm Technologies, Inc.
 * @brief File contains the utility api's for stub application
 *
 **************************************************************************************************************/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>

#include "qapi_lwm2m.h"
#include "utils_app_v2.h"
#include "log_intf_module.h"
#include "qcli.h"
#include "qcli_api.h"

#include "qapi_diag.h"

extern QCLI_Group_Handle_t qcli_m2m_stub_handle_v2;

/**
 * @fn     int32_t app_free_id_info_v2  ()
 * @brief  This API is used to Free the memory allocated for qapi lwm2m id info 
 * @param  id_infoP (IN) Pointer to memory allocated for id info to be freed 
 * @return  int32_t   0 on success
 *          -1 on failure
 */
int32_t app_free_id_info_v2 (qapi_Net_LWM2M_Id_Info_v2_t *id_infoP)
{
  qapi_Net_LWM2M_Id_Info_v2_t *idinfo_link = NULL; 
  if (id_infoP == NULL)
  {
    return -1;
  }
  while (id_infoP != NULL)
  {
    idinfo_link = id_infoP->next ;
    tx_byte_release (id_infoP);
    id_infoP = idinfo_link;
  }
  return 0;
}

/**
 * @fn     int32_t app_free_Data_v2()  
 * @brief  This API is used to Free the memory allocated for qapi lwm2m data 
 * @param  dataP (IN)  Pointer to memory allocated for qapi lwm2m data  to be freed
 * @return  int32_t   0 on success
 *          -1 on failure
 */
int32_t app_free_data_v2(qapi_Net_LWM2M_Data_v2_t *dataP)
{
  if (dataP == NULL)
    return -1;
  
  if (dataP != NULL)
  {
    app_free_instance_info_v2 (dataP->instance_info);
    tx_byte_release (dataP);
  }

  return 0;
}


/**
 * @fn     int32_t app_free_instance_info_v2()
 * @brief  This API is used to Free the memory allocated for qapi lwm2m instance 
 * @param  instanceP (IN) Pointer to memory allocated for qapi instance info to be freed 
 * @return  int32_t   0 on success
 *          -1 on failure
 */
int32_t app_free_instance_info_v2(qapi_Net_LWM2M_Instance_Info_v2_t *instanceP)
{
  if (instanceP == NULL)
   return -1;

  if (instanceP != NULL)
  {
    app_free_resource_info_v2(instanceP->resource_info);
    tx_byte_release (instanceP);
  }

  return 0;
}

int32_t app_free_flat_data_v2 (qapi_Net_LWM2M_Flat_Data_t **resourceP, size_t count)
{
  int i = 0;
  /* Validate the input parameters */
  if (!resourceP)
    return QAPI_ERR_INVALID_PARAM;

  if (!*resourceP || !count)
    return QAPI_OK;

  for (i=0; i<count; i++)
  {
    qapi_Net_LWM2M_Flat_Data_t* targetFlatData = (*resourceP) + i;

    if ((targetFlatData->type == QAPI_NET_LWM2M_TYPE_STRING_E || 
        targetFlatData->type == QAPI_NET_LWM2M_TYPE_OPAQUE_E) &&
        targetFlatData->value.asBuffer.buffer && 
        targetFlatData->value.asBuffer.length)
    {
      tx_byte_release(targetFlatData->value.asBuffer.buffer);
    }
    else if ((targetFlatData->type == QAPI_NET_LWM2M_TYPE_OBJECT || 
             targetFlatData->type == QAPI_NET_LWM2M_TYPE_OBJECT_INSTANCE || 
             targetFlatData->type == QAPI_NET_LWM2M_TYPE_MULTIPLE_RESOURCE) &&
             targetFlatData->value.asChildren.array && 
             targetFlatData->value.asChildren.count)
    {
      app_free_flat_data_v2(&(targetFlatData->value.asChildren.array), targetFlatData->value.asChildren.count);
    }
  }
  tx_byte_release(*resourceP);
 *resourceP = NULL;
  return QAPI_OK;  
}

/**
 * @fn     int32_t  stub_app_free_resource_info_v2 ()
 * @brief  This API is used to Free the memory allocated for qapi lwm2m resource  
 * @param  resourceP (IN) Pointer to memory allocated for qapi  lwm2m resource info to be freed 
 * @return  int32_t   0 on success
 *          -1 on failure
 */
int32_t app_free_resource_info_v2 (qapi_Net_LWM2M_Resource_Info_t *resourceP)
{
  qapi_Net_LWM2M_Resource_Info_t *resource_linkP = NULL;

  if (resourceP == NULL)
    return -1;

  while (resourceP != NULL)
  {
    resource_linkP =   resourceP->next;
    if(resourceP->type == QAPI_NET_LWM2M_TYPE_STRING_E ||
       resourceP->type == QAPI_NET_LWM2M_TYPE_OPAQUE_E)
    {
    if(resourceP->value.asBuffer.buffer)
    {
      tx_byte_release (resourceP->value.asBuffer.buffer);
    }
    }
    else if(resourceP->type == QAPI_NET_LWM2M_TYPE_MULTIPLE_RESOURCE
             || resourceP->type == QAPI_NET_LWM2M_TYPE_OBJECT ||
             resourceP->type == QAPI_NET_LWM2M_TYPE_OBJECT_INSTANCE)
    {
      if(resourceP->value.asChildren.array)
       app_free_flat_data_v2(&(resourceP->value.asChildren.array), resourceP->value.asChildren.count);
    }
    tx_byte_release (resourceP);
    resourceP = resource_linkP;
  }

  return 0;
}


void app_display_obj_info_v2(qapi_Net_LWM2M_Data_v2_t *lwm2m_data)
{
  qapi_Net_LWM2M_Instance_Info_v2_t *inst_info = NULL; 

  /* Validate the input parameters */
  if (!lwm2m_data)
    return;

  /* Object information */
  QCLI_Printf (qcli_m2m_stub_handle_v2 , "############# Object Information : %d ################\n", lwm2m_data->object_ID);
  QCLI_Printf (qcli_m2m_stub_handle_v2 , "Object Instances : %d ################\n", lwm2m_data->no_instances);
  
  /* Object instance information */
  inst_info = lwm2m_data->instance_info;
  while (inst_info)
  {
    qapi_Net_LWM2M_Resource_Info_t *res_info = NULL; 

    /* Object instance resource information */
    res_info = inst_info->resource_info;
    while (res_info)
    {
      switch (res_info->type)
      {
        case QAPI_NET_LWM2M_TYPE_STRING_E:           
        {
          uint8_t data_buf[128];

          memset(data_buf, 0x00, sizeof(data_buf));
          if(res_info->value.asBuffer.length)
          {
            strlcpy((void *)data_buf, (void *)res_info->value.asBuffer.buffer, res_info->value.asBuffer.length + 1);
          }
          else
          {
            res_info->value.asBuffer.buffer = NULL;
            res_info->value.asBuffer.buffer = 0;
          }
          QCLI_Printf (qcli_m2m_stub_handle_v2 , "Resource ID : %d,  Value : %s\n", res_info->resource_ID, data_buf);
          break;
        }  

        case QAPI_NET_LWM2M_TYPE_FLOAT_E:
        {
          QCLI_Printf (qcli_m2m_stub_handle_v2 , "Resource ID : %d,  Value : %ld\n", res_info->resource_ID, res_info->value.asFloat);
          break; 
        }

        case QAPI_NET_LWM2M_TYPE_BOOLEAN_E:
        {
          QCLI_Printf (qcli_m2m_stub_handle_v2 , "Resource ID : %d,  Value : %d\n", res_info->resource_ID, res_info->value.asBoolean);
          break;
        }

        case QAPI_NET_LWM2M_TYPE_INTEGER_E: 
        {
          QCLI_Printf (qcli_m2m_stub_handle_v2 , "Resource ID : %d,  Value : %lld\n", res_info->resource_ID, res_info->value.asInteger);
          break;
        }

        case QAPI_NET_LWM2M_TYPE_MULTIPLE_RESOURCE:
        {
          uint32_t cntr = 0;

          /* Only object link support */
          for (cntr = 0; cntr < res_info->value.asChildren.count; cntr++)
          {  
            if (QAPI_NET_LWM2M_TYPE_OBJECT_LINK == res_info->value.asChildren.array[cntr].type)
            {
              uint16_t obj_id = res_info->value.asChildren.array[cntr].value.asObjLink >> 16;
              uint16_t inst_id = res_info->value.asChildren.array[cntr].value.asObjLink & 0xFFFF;

              QCLI_Printf (qcli_m2m_stub_handle_v2 , "Resource ID : %d,  Value : [%d -> %d]\n", res_info->resource_ID, obj_id, inst_id);
            }
            else 
            {
              QCLI_Printf (qcli_m2m_stub_handle_v2 , "Resource ID : %d type(%d) not supported in test application(TO BE DONE)!\n", res_info->resource_ID, res_info->type);
            }
          }

          break;
        }

        default:
        {
          QCLI_Printf (qcli_m2m_stub_handle_v2 , "Resource ID : %d type(%d) not supported in test application(TO BE DONE)!\n", res_info->resource_ID, res_info->type);
          break; 
        }
      }
     
      res_info = res_info->next;   
    }
  
    inst_info = inst_info->next;
  } 
 
  return;  
}

/**
 * @fn std_obj_list_t * std_obj_list_add()
 * @brief This function is used to add node to the list
 *
 * @param head  pointer to the first node in the list
 * @param node  pointer to the new node to be added
 *
 * @return    pointer to the head node
 *
 */
std_obj_list_t * std_obj_list_add(std_obj_list_t * head,
    std_obj_list_t * node)
{
  std_obj_list_t * target = NULL;

  if (NULL == head) return node;

  if (head->id > node->id)
  {
    node->next = head;
    return node;
  }

  target = head;
  while (NULL != target->next && target->next->id <= node->id)
  {
    target = target->next;
  }

  node->next = target->next;
  target->next = node;

  return head;
}

/**
 * @fn std_obj_list_t * std_obj_list_find()
 * @brief This function is used to find a node in the list
 *
 * @param head  pointer to the first node in the list
 * @param id  id of the node to be searched
 *
 * @return    pointer to the node with give id on success
 *            NULL if id is not in the list
 */
std_obj_list_t * std_obj_list_find(std_obj_list_t * head,
    uint16_t id)
{
  while (NULL != head && head->id < id)
  {
    head = head->next;
  }

  if (NULL != head && head->id == id) return head;

  return NULL;
}

/**
 * @fn std_obj_list_t * std_obj_list_remove()
 * @brief This function is used to remove node from list
 *
 * @param head  pointer to the first node in the list
 * @param id  id of the node to be removed
 * @param nodeP double pointer to the list
 *
 * @return    pointer to the head node of the list on success
 *            NULL if id is not in the list
 */
std_obj_list_t * std_obj_list_remove(std_obj_list_t * head,
    uint16_t id,
    std_obj_list_t ** nodeP)
{
  std_obj_list_t * target;

  if (head == NULL)
  {
    if (nodeP) *nodeP = NULL;
    return NULL;
  }

  if (head->id == id)
  {
    if (nodeP) *nodeP = head;
    return head->next;
  }

  target = head;
  while (NULL != target->next && target->next->id < id)
  {
    target = target->next;
  }

  if (NULL != target->next && target->next->id == id)
  {
    if (nodeP) *nodeP = target->next;
    target->next = target->next->next;
  }
  else
  {
    if (nodeP) *nodeP = NULL;
  }

  return head;
}

/**
 * @fn std_obj_utils_copyValue() 
 * @brief This function is used to copy value input string into output string
 * @param dst destination string
 * @param src source string
 * @param len length to be copied
 * @return  void
 */
void std_obj_utils_copyValue(void * dst,
    const void * src,
    size_t len)
{ 
#ifdef LWM2M_LITTLE_ENDIAN
  size_t i = 0;
#endif  
  if (dst == NULL || src == NULL)
  {
    LOG_ERROR("LWM2M_LOG:Passed NULL Arguments\n");
    return;
  }
#ifdef LWM2M_BIG_ENDIAN
  memscpy(dst, src, len);
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
 * @fn std_obj_utils_plainTextToInt64()
 * @brief This function is used to convert plain text to int64 format 
 *
 * @param buffer Input plain text
 * @param length length of the buffer
 * @param dataP  output param after conversion
 *
 * @return 0 on failure
 *         1 on success
 */

int std_obj_utils_plainTextToInt64(uint8_t * buffer,
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


