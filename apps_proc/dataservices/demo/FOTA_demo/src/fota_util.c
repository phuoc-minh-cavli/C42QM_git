/*==============================================================================

                             FOTA_UTIL.C

GENERAL DESCRIPTION
  This file deals with the utility of FOTA download through LwM2M

Copyright (c) 2021 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR FILE
when        who    what, where, why
--------    ---    -------------------------------------------------------------

==============================================================================*/

#include "fota_util.h"
#include "fota_app.h"

extern size_t memscpy(void *dst, size_t dst_size, const void *src, size_t src_size);

list_t * list_add(list_t *head,
                     list_t *node)
{
  list_t *target = NULL;

  if(NULL == head) return node;

  if(head->id > node->id)
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

void free_memory(void **var)
{
  if(var == NULL || *var == NULL)
    return;

  free(*var);
  *var = NULL;

  return;
}

void free_coap_multi_option(qapi_Multi_Option_t **out_opt)
{
  if(out_opt && *out_opt)
  {
    qapi_Multi_Option_t *n = (*out_opt)->next;
    (*out_opt)->next = NULL;
    if (((*out_opt)->is_static == 0) && ((*out_opt)->data))
    {
      free_memory((void **)&((*out_opt)->data));
    }
    free_memory((void **)out_opt);
    free_coap_multi_option(&n);
  }
}

void coap_copy_multi_option(qapi_Multi_Option_t *in_opt,
                                      qapi_Multi_Option_t **out_opt)
{
  qapi_Multi_Option_t *opt = NULL;
  qapi_Multi_Option_t *next_opt = NULL;
  if(in_opt == NULL || out_opt == NULL)
    return;

  *out_opt = NULL;

  while(in_opt)
  {
    opt = (qapi_Multi_Option_t *)malloc(sizeof(qapi_Multi_Option_t));
    if(opt == NULL)
      goto end;

    memset(opt, 0x00, sizeof(qapi_Multi_Option_t));
    opt->next = NULL;
    opt->len = in_opt->len;
    if(in_opt->is_static)
    {
      opt->data = in_opt->data;
      opt->is_static = 1;
    }
    else
    {
      opt->is_static = 0;
      opt->data = malloc(opt->len);
      if(opt->data == NULL)
        goto end;
      memset(opt->data, 0x00, opt->len);
      memscpy(opt->data, opt->len, in_opt->data, in_opt->len);
    }
    if(NULL == *out_opt)
    {
      opt->next = *out_opt;
      *out_opt = opt;
    }
    else
    {
      for(next_opt = *out_opt; (next_opt != NULL && next_opt->next != NULL) ; next_opt = next_opt->next);
      next_opt->next = opt;
    }
    in_opt = in_opt->next;
  }
  return;
end:
  free_coap_multi_option(out_opt);
  return;
}

qapi_Status_t coap_copy_pkt(qapi_Coap_Session_Hdl_t hndl,
                                  qapi_Coap_Packet_t *in_coap_pkt,
                                  qapi_Coap_Packet_t **out_coap_pkt)
{
  int retval = QAPI_ERROR;

  if(hndl == NULL ||out_coap_pkt == NULL)
    return retval;

  if(in_coap_pkt == NULL)
  {
    *out_coap_pkt= NULL;
    return QAPI_OK;
  }

  retval = qapi_Coap_Init_Message(hndl,out_coap_pkt ,in_coap_pkt->type,in_coap_pkt->code);
  if(retval != QAPI_OK)
  {
    retval = QAPI_ERR_NO_MEMORY;
    goto end;
  }
  memscpy(*out_coap_pkt, sizeof(qapi_Coap_Packet_t), in_coap_pkt, sizeof(qapi_Coap_Packet_t));

  (*out_coap_pkt)->buffer = NULL;
  (*out_coap_pkt)->proxy_uri = NULL;
  (*out_coap_pkt)->uri_host = NULL;
  (*out_coap_pkt)->uri_path = NULL;
  (*out_coap_pkt)->location_path = NULL;
  (*out_coap_pkt)->location_query = NULL;
  (*out_coap_pkt)->uri_query = NULL;
  (*out_coap_pkt)->payload = NULL;

  if(in_coap_pkt->proxy_uri && in_coap_pkt->proxy_uri_len)
  {
    (*out_coap_pkt)->proxy_uri = malloc(in_coap_pkt->proxy_uri_len);
    if((*out_coap_pkt)->proxy_uri == NULL)
    {
      retval = QAPI_ERR_NO_MEMORY;
      goto end;
    }
    memset((*out_coap_pkt)->proxy_uri, 0x00, in_coap_pkt->proxy_uri_len);
    memscpy((*out_coap_pkt)->proxy_uri, (*out_coap_pkt)->proxy_uri_len, 
            in_coap_pkt->proxy_uri, in_coap_pkt->proxy_uri_len);
  }
  if(in_coap_pkt->uri_host && in_coap_pkt->uri_host_len)
  {
    (*out_coap_pkt)->uri_host = malloc(in_coap_pkt->uri_host_len);
    if((*out_coap_pkt)->uri_host == NULL)
    {
      retval = QAPI_ERR_NO_MEMORY;
      goto end;
    }
    memset((*out_coap_pkt)->uri_host, 0x00, in_coap_pkt->uri_host_len);
    memscpy((*out_coap_pkt)->uri_host, (*out_coap_pkt)->uri_host_len, 
            in_coap_pkt->uri_host, in_coap_pkt->uri_host_len);
  }
  if(in_coap_pkt->location_path)
  {
    coap_copy_multi_option(in_coap_pkt->location_path, &((*out_coap_pkt)->location_path));
  }
  if(in_coap_pkt->location_query && in_coap_pkt->location_query_len)
  {
    (*out_coap_pkt)->location_query = malloc(in_coap_pkt->location_query_len);
    if((*out_coap_pkt)->location_query == NULL)
    {
      retval = QAPI_ERR_NO_MEMORY;
      goto end;
    }
    memset((*out_coap_pkt)->location_query, 0x00, in_coap_pkt->location_query_len);
    memscpy((*out_coap_pkt)->location_query, (*out_coap_pkt)->location_query_len, 
            in_coap_pkt->location_query, in_coap_pkt->location_query_len);
  }
  if(in_coap_pkt->uri_path)
  {
    coap_copy_multi_option(in_coap_pkt->uri_path, &((*out_coap_pkt)->uri_path));
  }
  if(in_coap_pkt->uri_query)
  {
    coap_copy_multi_option(in_coap_pkt->uri_query, &((*out_coap_pkt)->uri_query));
  }
  if(in_coap_pkt->payload && in_coap_pkt->payload_len)
  {
    (*out_coap_pkt)->payload = malloc(in_coap_pkt->payload_len);
    if((*out_coap_pkt)->payload == NULL)
    {
      retval = QAPI_ERR_NO_MEMORY;
      goto end;
    }
    memset((*out_coap_pkt)->payload, 0x00, in_coap_pkt->payload_len);
    memscpy((*out_coap_pkt)->payload, (*out_coap_pkt)->payload_len, 
            in_coap_pkt->payload, in_coap_pkt->payload_len);
  }
  retval = QAPI_OK;
end:

  if(retval != QAPI_OK)
  {
    qapi_Coap_Free_Message(hndl, *out_coap_pkt);
  }
  return retval;
}

void free_coap_client_transaction(qapi_Coap_Session_Hdl_t hndl,
                                              qapi_Coap_Transaction_t **in_clientTransaction)
{
  qapi_Coap_Transaction_t *clientTransaction = NULL;

  if(hndl == NULL || in_clientTransaction == NULL || *in_clientTransaction == NULL)
    return;

  clientTransaction = *in_clientTransaction;

  if(clientTransaction->message)
  {
    qapi_Coap_Free_Message(hndl, clientTransaction->message);
  }
  if(clientTransaction->buffer)
  {
    free_memory((void **)&clientTransaction->buffer);
  }
  free_memory((void **)&clientTransaction);
}

qapi_Status_t coap_copy_client_transaction(qapi_Coap_Session_Hdl_t hndl,qapi_Coap_Transaction_t *in_clientTransaction ,
                                                       qapi_Coap_Transaction_t **out_clientTransaction)
{
  int retval = QAPI_ERROR;

  if(hndl == NULL || in_clientTransaction == NULL || out_clientTransaction == NULL)
    return QAPI_ERROR;

  *out_clientTransaction = (qapi_Coap_Transaction_t *)malloc(sizeof(qapi_Coap_Transaction_t));
  if(*out_clientTransaction == NULL)
  {
    return QAPI_ERR_NO_MEMORY;
  }
  memset(*out_clientTransaction, 0x00, sizeof(qapi_Coap_Transaction_t));
  (*out_clientTransaction)->ack_received = in_clientTransaction->ack_received;
  if(coap_copy_pkt(hndl,in_clientTransaction->message, 
                   &((*out_clientTransaction)->message)) != QAPI_OK)
  {
    retval = QAPI_ERROR;
    goto end;
  }
  if(in_clientTransaction->buffer && in_clientTransaction->buffer_len)
  {
    (*out_clientTransaction)->buffer = malloc(in_clientTransaction->buffer_len);
    if((*out_clientTransaction)->buffer== NULL)
    {
      retval = QAPI_ERROR;
      goto end;
    }
    memset((*out_clientTransaction)->buffer, 0x00, in_clientTransaction->buffer_len);
    memscpy((*out_clientTransaction)->buffer, (*out_clientTransaction)->buffer_len,
            in_clientTransaction->buffer, in_clientTransaction->buffer_len);
  }
 
  (*out_clientTransaction)->mID = in_clientTransaction->mID;
  (*out_clientTransaction)->response_timeout = in_clientTransaction->response_timeout;
  (*out_clientTransaction)->retrans_counter = in_clientTransaction->retrans_counter;
  (*out_clientTransaction)->retrans_time = in_clientTransaction->retrans_time;
  (*out_clientTransaction)->userData = in_clientTransaction->userData;

  retval = QAPI_OK;

end:

  if(retval != QAPI_OK)
    free_coap_client_transaction(hndl, out_clientTransaction);
  return retval;
}


int32_t app_free_id_info_(qapi_Net_LWM2M_Id_Info_v2_t *id_infoP)
{
  qapi_Net_LWM2M_Id_Info_v2_t *idinfo_link = NULL; 
  if (id_infoP == NULL)
  {
    return -1;
  }
  while(id_infoP != NULL)
  {
    idinfo_link = id_infoP->next ;
    free_memory((void **)&id_infoP);
    id_infoP = idinfo_link;
  }
  return 0;
}

static qapi_Status_t free_lwm2m_flat_data
( 
  size_t lwm2m_data_size,
  qapi_Net_LWM2M_Flat_Data_t **lwm2m_data)
{
  int i = 0;
  /* Validate the input parameters */
  if (!lwm2m_data)
    return QAPI_ERR_INVALID_PARAM;

  if (!*lwm2m_data || !lwm2m_data_size)
    return QAPI_OK;

  for (i=0; i<lwm2m_data_size; i++)
  {
    qapi_Net_LWM2M_Flat_Data_t* targetFlatData = (*lwm2m_data) + i;

    if ((targetFlatData->type == QAPI_NET_LWM2M_TYPE_STRING_E || 
        targetFlatData->type == QAPI_NET_LWM2M_TYPE_OPAQUE_E) &&
        targetFlatData->value.asBuffer.buffer &&
        targetFlatData->value.asBuffer.length)
    {
      free_memory((void **)&targetFlatData->value.asBuffer.buffer);
    }
    else if ((targetFlatData->type == QAPI_NET_LWM2M_TYPE_OBJECT || 
             targetFlatData->type == QAPI_NET_LWM2M_TYPE_OBJECT_INSTANCE || 
             targetFlatData->type == QAPI_NET_LWM2M_TYPE_MULTIPLE_RESOURCE)&&
             targetFlatData->value.asChildren.array &&
             targetFlatData->value.asChildren.count)
    {
      free_lwm2m_flat_data(targetFlatData->value.asChildren.count, &(targetFlatData->value.asChildren.array));
    }
  }
  free_memory((void **)lwm2m_data);
  return QAPI_OK;  
}

static void free_lwm2m_all_resource_instances
(
  qapi_Net_LWM2M_Resource_Info_t *resource_info_head
)
{
  /* Object instance resource information */
  while (resource_info_head)
  {
    qapi_Net_LWM2M_Resource_Info_t *del_res_info_node = resource_info_head;

    /* Resource type - String */ 
    if ((QAPI_NET_LWM2M_TYPE_STRING_E == resource_info_head->type || 
         QAPI_NET_LWM2M_TYPE_OPAQUE_E == resource_info_head->type) &&
        (resource_info_head->value.asBuffer.buffer) && (resource_info_head->value.asBuffer.length))
         free_memory((void **)&resource_info_head->value.asBuffer.buffer);

    /* Resource type - Multi-resource instance */
    else if ((QAPI_NET_LWM2M_TYPE_MULTIPLE_RESOURCE == resource_info_head->type) &&
             (resource_info_head->value.asChildren.array)&& (resource_info_head->value.asChildren.count))
             free_lwm2m_flat_data(resource_info_head->value.asChildren.count, &resource_info_head->value.asChildren.array);

    /* Iterate through all the resources */           
    resource_info_head = resource_info_head->next;
    free_memory((void **)&del_res_info_node);
  }
}

qapi_Status_t free_lwm2m_copy_data_
(
  qapi_Net_LWM2M_Data_v2_t **lwm2m_data
)
{
  /* Validate the input parameters */
  if (!lwm2m_data)
    return QAPI_ERR_INVALID_PARAM;

  while(*lwm2m_data)
  {
    qapi_Net_LWM2M_Data_v2_t *del_node = *lwm2m_data;
    int i = 0;
    /* Object instance information */
    while (del_node->instance_info && i < del_node->no_instances)
    {
      qapi_Net_LWM2M_Instance_Info_v2_t *del_inst_info_node = del_node->instance_info;
  
      /* Object instance resource information */
      free_lwm2m_all_resource_instances(del_inst_info_node->resource_info);
            
      /* Iterate through all the instances */ 
      del_node->instance_info = del_node->instance_info->next;
      free_memory((void **)&del_inst_info_node);
      i++;
    }
    /* Iterate through all the objects */ 
    *lwm2m_data = (*lwm2m_data)->next;
    free_memory((void **)&del_node);
  }
  return QAPI_OK;
}

static int lwm2m_flat_data_copy
(
  size_t               in_lwm2m_data_size,
  qapi_Net_LWM2M_Flat_Data_t  *in_lwm2m_data, 
  qapi_Net_LWM2M_Flat_Data_t **out_lwm2m_data)
{
  int result = QAPI_ERROR;
  int  i = 0;
  uint32_t out_lwm2m_data_size = 0;
  
  /* Validate the input parameters */
  if ((!out_lwm2m_data) || !in_lwm2m_data_size)
  {  
    LOG_ERROR("FOTA_LOG: %s wrong param",__FUNCTION__);
    return QAPI_ERR_INVALID_PARAM;  
  }

  out_lwm2m_data_size = in_lwm2m_data_size * sizeof(qapi_Net_LWM2M_Flat_Data_t);
  /* Object allocation */
  *out_lwm2m_data = malloc(out_lwm2m_data_size);
  if(*out_lwm2m_data == NULL)
  {
    return QAPI_ERROR;
  }
  memset(*out_lwm2m_data, 0x00, out_lwm2m_data_size);
  memscpy(*out_lwm2m_data, out_lwm2m_data_size, in_lwm2m_data, in_lwm2m_data_size * sizeof(qapi_Net_LWM2M_Flat_Data_t));

  result = QAPI_OK;
  for(i=0; i<in_lwm2m_data_size; i++)
  {
    qapi_Net_LWM2M_Flat_Data_t *destTarget = (*out_lwm2m_data) + i;
    qapi_Net_LWM2M_Flat_Data_t *srcTarget  = in_lwm2m_data  + i;

    if ((srcTarget->type == QAPI_NET_LWM2M_TYPE_STRING_E || 
        srcTarget->type == QAPI_NET_LWM2M_TYPE_OPAQUE_E) &&
        srcTarget->value.asBuffer.buffer &&
        srcTarget->value.asBuffer.length)
    {
      uint8_t  *in_payload      = srcTarget->value.asBuffer.buffer;
      uint32_t  in_payload_len  = srcTarget->value.asBuffer.length;
      uint8_t **out_payload     = &destTarget->value.asBuffer.buffer;
      uint32_t *out_payload_len = &destTarget->value.asBuffer.length;

      *out_payload = NULL;
      *out_payload_len = 0;

      *out_payload = malloc(in_payload_len);
      if(*out_payload == NULL)
      {
        result = QAPI_ERR_NO_MEMORY;
        goto COPY_ERROR;
      }
      memset(*out_payload, 0x00, in_payload_len);
      *out_payload_len = in_payload_len;
      memscpy(*out_payload, *out_payload_len, in_payload, in_payload_len);
    }
    else if ((srcTarget->type == QAPI_NET_LWM2M_TYPE_MULTIPLE_RESOURCE || 
             srcTarget->type == QAPI_NET_LWM2M_TYPE_OBJECT_INSTANCE || 
             srcTarget->type == QAPI_NET_LWM2M_TYPE_OBJECT) &&
             srcTarget->value.asChildren.array &&
             srcTarget->value.asChildren.count)
    {
      destTarget->value.asChildren.array = NULL;
      result = lwm2m_flat_data_copy(srcTarget->value.asChildren.count, srcTarget->value.asChildren.array, &(destTarget->value.asChildren.array));

      if (result != QAPI_OK)
        goto COPY_ERROR;
    }
  }
COPY_ERROR:
  /* Release the allocated resources as there is a failure */
  if (result)
    free_lwm2m_flat_data(in_lwm2m_data_size, out_lwm2m_data);

  return result; 
}

static qapi_Status_t lwm2m_copy_resource_instances
(
  qapi_Net_LWM2M_Resource_Info_t* in_res_info ,
  qapi_Net_LWM2M_Resource_Info_t** out_res_info,
  int count,size_t in_res_info_size)
{
  qapi_Status_t result = QAPI_OK;
  int i = 0;

  if(!out_res_info)
    return QAPI_ERR_INVALID_PARAM;

  while (in_res_info && i++ < count)
  {
    qapi_Net_LWM2M_Resource_Info_t *out_res_node = NULL;
    uint32_t out_res_node_size = sizeof(qapi_Net_LWM2M_Resource_Info_t);

    out_res_node = (qapi_Net_LWM2M_Resource_Info_t *)malloc(sizeof(qapi_Net_LWM2M_Resource_Info_t));
    if(out_res_node == NULL)
    {
      goto COPY_ERROR;
    }
    memset(out_res_node, 0x00, (sizeof(qapi_Net_LWM2M_Resource_Info_t)));
    memscpy(out_res_node, out_res_node_size, in_res_info, sizeof(qapi_Net_LWM2M_Resource_Info_t));

    /* Store the object instance resource  information */     
    out_res_node->next = NULL;

    /* Resource type - String */
    if ((QAPI_NET_LWM2M_TYPE_STRING_E == in_res_info->type || 
         QAPI_NET_LWM2M_TYPE_OPAQUE_E == in_res_info->type) &&
        (in_res_info->value.asBuffer.buffer) &&
        (in_res_info->value.asBuffer.length))
    {
      uint8_t  *in_payload      = in_res_info->value.asBuffer.buffer;
      uint32_t  in_payload_len  = in_res_info->value.asBuffer.length;
      uint8_t **out_payload     = &out_res_node->value.asBuffer.buffer;
      uint32_t *out_payload_len = &out_res_node->value.asBuffer.length;

      *out_payload = NULL;
      *out_payload_len = 0x00;

      *out_payload = malloc(in_payload_len);
      if(*out_payload == NULL)
      {
        result = QAPI_ERR_NO_MEMORY;
        goto COPY_ERROR;
      }
      memset(*out_payload, 0x00, in_payload_len);
      *out_payload_len = in_payload_len;
      memscpy(*out_payload, *out_payload_len, in_payload, in_payload_len);
    }
    /* Resource type - Multi-resource instance. */
    else if ((QAPI_NET_LWM2M_TYPE_MULTIPLE_RESOURCE == in_res_info->type) &&
             (in_res_info->value.asChildren.array) &&
             (in_res_info->value.asChildren.count))
    {
      qapi_Net_LWM2M_Flat_Data_t  *in_array       = in_res_info->value.asChildren.array;
      uint32_t  in_array_size  = in_res_info->value.asChildren.count;
      qapi_Net_LWM2M_Flat_Data_t **out_array      = &out_res_node->value.asChildren.array;
      uint32_t *out_array_size = (uint32_t *)&out_res_node->value.asChildren.count;

      *out_array = NULL;
      *out_array_size = 0;
      if(0x00 != lwm2m_flat_data_copy((size_t)in_array_size, in_array, out_array))
      {
        result = QAPI_ERR_NO_MEMORY;
        goto COPY_ERROR;
      }
      *out_array_size = in_array_size;
    }

    /* Add the object resource instance information */
    *out_res_info = (qapi_Net_LWM2M_Resource_Info_t *)LIST_ADD(*out_res_info ,out_res_node);
    /* Iterate through the remaining resources */
    in_res_info = in_res_info->next;
  }

  return result;

COPY_ERROR:
  free_lwm2m_all_resource_instances(*out_res_info);
  return result;
}

int lwm2m_copy_data_(qapi_Net_LWM2M_Data_v2_t *in_lwm2m_data, 
                                  qapi_Net_LWM2M_Data_v2_t **out_lwm2m_data)
{
  qapi_Status_t result = QAPI_ERROR;
  qapi_Net_LWM2M_Data_v2_t *in_obj_info = in_lwm2m_data;
  qapi_Net_LWM2M_Instance_Info_v2_t *in_inst_info = NULL;
  qapi_Net_LWM2M_Resource_Info_t *in_res_info = NULL;
  uint32_t out_lwm2m_data_size = 0;
  int i = 0;

  /* Validate the input parameters */
  if ((!in_lwm2m_data) ||
      (!out_lwm2m_data))
    return QAPI_ERROR;

  *out_lwm2m_data = NULL;
  while(in_obj_info)
  {
    qapi_Net_LWM2M_Data_v2_t *out_obj_info = NULL;
    out_lwm2m_data_size = sizeof(qapi_Net_LWM2M_Data_v2_t);
  
    /* Object allocation */
    out_obj_info = (qapi_Net_LWM2M_Data_v2_t *)malloc(sizeof(qapi_Net_LWM2M_Data_v2_t));
    if(out_obj_info == NULL)
    {
      goto COPY_ERROR;
    }
    memset(out_obj_info, 0x00, (sizeof(qapi_Net_LWM2M_Data_v2_t)));
    memscpy(out_obj_info, out_lwm2m_data_size, in_obj_info, sizeof(qapi_Net_LWM2M_Data_v2_t));

    out_obj_info->instance_info = NULL;
    out_obj_info->next = NULL;

    /* Object instance allocation */
    if (!in_obj_info->instance_info || !in_obj_info->no_instances)
    {
      /* Add the object information */
      *out_lwm2m_data = (qapi_Net_LWM2M_Data_v2_t * )LIST_ADD(*out_lwm2m_data, out_obj_info);
      in_obj_info = in_obj_info->next;
      continue;
    }

    in_inst_info = in_obj_info->instance_info;
    i = 0;
    while(in_inst_info && i < in_obj_info->no_instances)
    {
      qapi_Net_LWM2M_Instance_Info_v2_t *out_inst_node = NULL; 
      uint32_t out_inst_node_size = sizeof(qapi_Net_LWM2M_Instance_Info_v2_t);
  
      out_inst_node = (qapi_Net_LWM2M_Instance_Info_v2_t *)malloc(sizeof(qapi_Net_LWM2M_Instance_Info_v2_t));
      if(out_inst_node == NULL)
      {
        goto COPY_ERROR;
      }
      memset(out_inst_node, 0x00, (sizeof(qapi_Net_LWM2M_Instance_Info_v2_t)));
      memscpy(out_inst_node, out_inst_node_size, in_inst_info, sizeof(qapi_Net_LWM2M_Instance_Info_v2_t));

      out_inst_node->next = NULL;
      out_inst_node->resource_info = NULL;
  
      /* Add the object instance information */
      out_obj_info->instance_info = (qapi_Net_LWM2M_Instance_Info_v2_t * ) LIST_ADD(out_obj_info->instance_info, out_inst_node);   
      
      /* Object instance resource allocation */
      in_res_info = in_inst_info->resource_info;
     /* Iterate through the remaining instances */
     if(lwm2m_copy_resource_instances(in_res_info,&out_inst_node->resource_info, out_inst_node->no_resources, in_inst_info->no_resources)!=QAPI_OK)
       goto COPY_ERROR;

      i++;
      in_inst_info = in_inst_info->next;
    }

    /* Add the object information */
    *out_lwm2m_data = (qapi_Net_LWM2M_Data_v2_t * )LIST_ADD(*out_lwm2m_data, out_obj_info);
    in_obj_info = in_obj_info->next;
  }
  result = QAPI_OK;

COPY_ERROR:
  /* Release the allocated resources as there is a failure */
  if (result)
    free_lwm2m_copy_data_(out_lwm2m_data);

  return result;
}


