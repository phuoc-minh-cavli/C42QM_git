/**************************************************************************************************************
 * Copyright © 2017-2018 Qualcomm Technologies, Inc.
 * @file  ext_object_test_app.c
 * @author  Qualcomm Technologies, Inc.
 * @brief File contains the implementation for the extensible object stub application
 *
 **************************************************************************************************************/

#include "ext_object_test_app_module.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "log_intf_module.h"
#include "fs_public.h"
#include <math.h>
#include <float.h>
#include <stringl/stringl.h>
#include "lwm2m_main_module.h"
#include "qurt_txm_qapi_fwk.h"
#include "qapi_socket.h"

#include "multi_app_test_module.h"

qapi_Net_LWM2M_Event_t ext_obj_event;
static bool lwm2m_client_in_sleep_state = false;

#define MAX_EXT_INSTANCE 10

extern TX_BYTE_POOL *byte_pool_qcli;

qapi_Net_LWM2M_App_Handler_t ext_object_handler_app1 = NULL;
qapi_Net_LWM2M_App_Handler_t ext_object_handler_app2 = NULL;

bool dam_app1_registerd = false;

bool dam_app2_registerd = false;

#define MAX_URI_LEN 256
#define MAX_APN_LEN 64
#define INVALID_RES_INST_ID 0xFFF9
qapi_Net_LWM2M_Config_Data_t* ext_object_clientconfig_data = NULL;

static qapi_TIMER_handle_t obs_notify_Timer1;
static qapi_TIMER_handle_t obs_notify_Timer2;

#define LWM2M_MAX_ID   ((uint16_t)0xFFFF)
#define PROCESSING_DELAY 2
#define PRV_LINK_BUFFER_SIZE  1024

extern bool dam_thrd_running[NO_OF_APPS];

#define PRV_CONCAT_STR(buf, len, index, str, str_len)    \
{                                                    \
  if ((len)-(index) < (str_len)) return -1;        \
  memscpy((buf)+(index), ((len) - (index)), (str), (str_len));         \
  (index) += (str_len);                            \
}


#define TIMEOUT_VAL_WITH_PERIOD(observe_info, timeout, current_time, period)                        \
{\
  // Minimum Period did not elapse yet, update timeoutP to interval, so that            \
  // process signal will not wait for longer timeoutP value */                          \
  if ( (timeout > PROCESSING_DELAY) && (timeout >= ( observe_info->last_time + period - current_time )) )       \
  {                                                                                     \
    timeout = (observe_info->last_time + period - current_time)-PROCESSING_DELAY;  \
    timeout = ((int)timeout < 0) ? 0 : timeout;         \
  LOG_INFO("LWM2M_LOG: timeout > interval Updating timeout to : %d\n", timeout);      \
  }\
}

qapi_Net_LWM2M_Data_t* lwm2m_object_add(qapi_Net_LWM2M_Data_t* head,
                                        qapi_Net_LWM2M_Data_t* node)
{
  qapi_Net_LWM2M_Data_t* target = NULL;

  if (NULL == head) return node;

  if(head->object_ID > node->object_ID)
  {
    node->next = head;
    return node;
  }

  target = head;
  while (NULL != target->next && target->next->object_ID < node->object_ID)
  {
    target = target->next;
  }

  node->next = target->next;
  target->next = node;

  return head;
}


qapi_Net_LWM2M_Instance_Info_t* lwm2m_instance_add(qapi_Net_LWM2M_Instance_Info_t* head,
                                                   qapi_Net_LWM2M_Instance_Info_t* node)
{
  qapi_Net_LWM2M_Instance_Info_t* target = NULL;

  if (NULL == head) return node;

  if(head->instance_ID > node->instance_ID)
  {
    node->next = head;
    return node;
  }

  target = head;
  while (NULL != target->next && target->next->instance_ID < node->instance_ID)
  {
    target = target->next;
  }

  node->next = target->next;
  target->next = node;

  return head;
}

qapi_Net_LWM2M_Resource_Info_t* lwm2m_resource_add(qapi_Net_LWM2M_Resource_Info_t* head,
                                                   qapi_Net_LWM2M_Resource_Info_t* node)
{
  qapi_Net_LWM2M_Resource_Info_t* target = NULL;

  if (NULL == head) return node;

  if(head->resource_ID > node->resource_ID)
  {
    node->next = head;
    return node;
  }

  target = head;
  while (NULL != target->next && target->next->resource_ID < node->resource_ID)
  {
    target = target->next;
  }

  node->next = target->next;
  target->next = node;

  return head;
}

ext_resource_instance_int_list_t* lwm2m_resource_inst_int_add(ext_resource_instance_int_list_t* head,
                                                              ext_resource_instance_int_list_t* node)
{
  ext_resource_instance_int_list_t* target = NULL;

  if (NULL == head) return node;

  if(head->resInstId > node->resInstId)
  {
    node->next = head;
    return node;
  }

  target = head;
  while (NULL != target->next && target->next->resInstId < node->resInstId)
  {
    target = target->next;
  }

  node->next = target->next;
  target->next = node;

  return head;
}

ext_resource_instance_string_list_t* lwm2m_resource_inst_string_add(ext_resource_instance_string_list_t* head,
                                                                    ext_resource_instance_string_list_t* node)
{
  ext_resource_instance_string_list_t* target = NULL;

  if (NULL == head) return node;

  if(head->resInstId > node->resInstId)
  {
    node->next = head;
    return node;
  }

  target = head;
  while (NULL != target->next && target->next->resInstId < node->resInstId)
  {
    target = target->next;
  }

  node->next = target->next;
  target->next = node;

  return head;
}

ext_resource_instance_int_list_t* lwm2m_resource_inst_int_remove(ext_resource_instance_int_list_t* head,
                                                                 ext_resource_instance_int_list_t** node,
                                                                 uint16_t id)
{
  ext_resource_instance_int_list_t* target = NULL;

  if (head == NULL)
  {
    if (node) *node = NULL;
    return NULL;
  }

  if (head->resInstId == id)
  {
    if (node) *node = head;
    return head->next;
  }

  target = head;
  while (NULL != target->next && target->next->resInstId < id)
  {
    target = target->next;
  }

  if (NULL != target->next && target->next->resInstId == id)
  {
    if (node) *node = target->next;
    target->next = target->next->next;
  }
  else
  {
    if (node) *node = NULL;
  }

  return head;
}

ext_resource_instance_string_list_t* lwm2m_resource_inst_string_remove(ext_resource_instance_string_list_t* head,
                                                                       ext_resource_instance_string_list_t** node,
                                                                       uint16_t id)
{
  ext_resource_instance_string_list_t* target = NULL;

  if (head == NULL)
  {
    if (node) *node = NULL;
    return NULL;
  }

  if (head->resInstId == id)
  {
    if (node) *node = head;
    return head->next;
  }

  target = head;
  while (NULL != target->next && target->next->resInstId < id)
  {
    target = target->next;
  }

  if (NULL != target->next && target->next->resInstId == id)
  {
    if (node) *node = target->next;
    target->next = target->next->next;
  }
  else
  {
    if (node) *node = NULL;
  }

  return head;
}

void lwm2m_resource_inst_int_free(ext_resource_instance_int_list_t* head)
{
  if(head != NULL)
  {
    ext_resource_instance_int_list_t *next = NULL;
    next = head->next;
    tx_byte_release(head);
    head = NULL;
    lwm2m_resource_inst_int_free(next);
  }
}

void lwm2m_resource_inst_string_free(ext_resource_instance_string_list_t* head)
{
  if(head != NULL)
  {
    ext_resource_instance_string_list_t *next = NULL;
    next = head->next;
    tx_byte_release(head);
    head = NULL;
    lwm2m_resource_inst_string_free(next);
  }
}

gen_multi_res_t* lwm2m_gen_inst_add(gen_multi_res_t* head,
                                    gen_multi_res_t* node)
{
  gen_multi_res_t* target = NULL;

  if (NULL == head) return node;

  if(head->instanceId > node->instanceId)
  {
    node->next = head;
    return node;
  }

  target = head;
  while (NULL != target->next && target->next->instanceId < node->instanceId)
  {
    target = target->next;
  }

  node->next = target->next;
  target->next = node;

  return head;
}

gen_multi_res_t* lwm2m_gen_inst_remove(gen_multi_res_t* head,
                                       gen_multi_res_t** node,
                                       uint16_t id)
{
  gen_multi_res_t* target = NULL;

  if (head == NULL)
  {
    if (node) *node = NULL;
    return NULL;
  }

  if (head->instanceId == id)
  {
    if (node) *node = head;
    return head->next;
  }

  target = head;
  while (NULL != target->next && target->next->instanceId < id)
  {
    target = target->next;
  }

  if (NULL != target->next && target->next->instanceId == id)
  {
    if (node) *node = target->next;
    target->next = target->next->next;
  }
  else
  {
    if (node) *node = NULL;
  }

  return head;
}

pre_multi_res_t* lwm2m_pre_inst_add(pre_multi_res_t* head,
                                    pre_multi_res_t* node)
{
  pre_multi_res_t* target = NULL;

  if (NULL == head) return node;

  if(head->instanceId > node->instanceId)
  {
    node->next = head;
    return node;
  }

  target = head;
  while (NULL != target->next && target->next->instanceId < node->instanceId)
  {
    target = target->next;
  }

  node->next = target->next;
  target->next = node;

  return head;
}

pre_multi_res_t* lwm2m_pre_inst_remove(pre_multi_res_t* head,
                                       pre_multi_res_t** node,
                                       uint16_t id)
{
  pre_multi_res_t* target = NULL;

  if (head == NULL)
  {
    if (node) *node = NULL;
    return NULL;
  }

  if (head->instanceId == id)
  {
    if (node) *node = head;
    return head->next;
  }

  target = head;
  while (NULL != target->next && target->next->instanceId < id)
  {
    target = target->next;
  }

  if (NULL != target->next && target->next->instanceId == id)
  {
    if (node) *node = target->next;
    target->next = target->next->next;
  }
  else
  {
    if (node) *node = NULL;
  }

  return head;
}

qapi_Net_LWM2M_Data_t* lwm2m_object_remove(qapi_Net_LWM2M_Data_t *head,
                                           qapi_Net_LWM2M_Data_t ** nodeP,
                                           uint8_t id)
{
  qapi_Net_LWM2M_Data_t * target = NULL;

  if (head == NULL)
  {
    if (nodeP) *nodeP = NULL;
    return NULL;
  }

  if (head->object_ID == id)
  {
    if (nodeP) *nodeP = head;
    return head->next;
  }

  target = head;
  while (NULL != target->next && target->next->object_ID < id)
  {
    target = target->next;
  }

  if (NULL != target->next && target->next->object_ID == id)
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


qapi_Net_LWM2M_Instance_Info_t* lwm2m_instance_remove(qapi_Net_LWM2M_Instance_Info_t *head,
                                                      qapi_Net_LWM2M_Instance_Info_t ** nodeP,
                                                      uint8_t id)
{
  qapi_Net_LWM2M_Instance_Info_t * target;

  if (head == NULL)
  {
    if (nodeP) *nodeP = NULL;
    return NULL;
  }

  if (head->instance_ID== id)
  {
    if (nodeP) *nodeP = head;
    return head->next;
  }

  target = head;
  while (NULL != target->next && target->next->instance_ID< id)
  {
    target = target->next;
  }

  if (NULL != target->next && target->next->instance_ID== id)
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

qapi_Net_LWM2M_Data_t* lwm2m_object_find(qapi_Net_LWM2M_Data_t* head,
                                         uint16_t id)
{
  while (NULL != head && head->object_ID < id)
  {
    head = head->next;
  }

  if (NULL != head && head->object_ID == id)
  return head;

  return NULL;
}

qapi_Net_LWM2M_Instance_Info_t* lwm2m_instance_find(qapi_Net_LWM2M_Instance_Info_t* head,
                                                    uint16_t id)
{
  while (NULL != head && head->instance_ID < id)
  {
    head = head->next;
  }

  if (NULL != head && head->instance_ID == id)
  return head;

  return NULL;
}

qapi_Net_LWM2M_Resource_Info_t* lwm2m_resource_find(qapi_Net_LWM2M_Resource_Info_t* head,
                                                    uint16_t id)
{
  while (NULL != head && head->resource_ID< id)
  {
    head = head->next;
  }

  if (NULL != head && head->resource_ID == id)
  return head;

  return NULL;
}

gen_multi_res_t* lwm2m_gen_inst_find(gen_multi_res_t* head,
                                           uint16_t id)
{
  while (NULL != head && head->instanceId < id)
  {
    head = head->next;
  }

  if (NULL != head && head->instanceId == id)
  return head;

  return NULL;
}

pre_multi_res_t* lwm2m_pre_inst_find(pre_multi_res_t* head,
                                           uint16_t id)
{
  while (NULL != head && head->instanceId < id)
  {
    head = head->next;
  }

  if (NULL != head && head->instanceId == id)
  return head;

  return NULL;
}

ext_resource_instance_int_list_t * lwm2m_find_resource_inst(ext_resource_instance_int_list_t * head,
                                                           uint16_t id)
{
  while (NULL != head && head->resInstId < id)
  {
    head = head->next;
  }

  if (NULL != head && head->resInstId == id) return head;

  return NULL;
}

ext_resource_instance_string_list_t * lwm2m_find_string_resource_inst(ext_resource_instance_string_list_t * head,
                                                                      uint16_t id)
{
  while (NULL != head && head->resInstId < id)
  {
    head = head->next;
  }

  if (NULL != head && head->resInstId == id) return head;

  return NULL;
}

static void lwm2m_data_free(int size,
                            qapi_Net_LWM2M_Flat_Data_t* dataP)
{
  int i = 0;

  LOG_INFO("size: %d", size);
  if (size == 0 || dataP == NULL) return;

  for (i = 0; i < size; i++)
  {
    switch (dataP[i].type)
    {
      case QAPI_NET_LWM2M_TYPE_MULTIPLE_RESOURCE:
      case QAPI_NET_LWM2M_TYPE_OBJECT_INSTANCE:
      case QAPI_NET_LWM2M_TYPE_OBJECT:
        lwm2m_data_free(dataP[i].value.asChildren.count, dataP[i].value.asChildren.array);
        break;

      case QAPI_NET_LWM2M_TYPE_STRING_E:
      case QAPI_NET_LWM2M_TYPE_OPAQUE_E:
        if (dataP[i].value.asBuffer.buffer != NULL)
        {
          tx_byte_release(dataP[i].value.asBuffer.buffer);
        }

      default:
        // do nothing
        break;
    }
  }
  tx_byte_release(dataP);
}

static int utils_plainTextToInt64(uint8_t * buffer,
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

static int utils_plainTextToFloat64(uint8_t * buffer,
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

static int setBuffer(qapi_Net_LWM2M_Flat_Data_t *dataP,
                     uint8_t * buffer,
                     size_t bufferLen)
{
  if( dataP == NULL || buffer == NULL || bufferLen == 0 ) 
  { 
    LOG_INFO(" Passed  Argument is NULL");
    return 0;
  }

  tx_byte_allocate(byte_pool_qcli, (VOID **)&(dataP->value.asBuffer.buffer), bufferLen, TX_NO_WAIT);
  if (dataP->value.asBuffer.buffer == NULL)
  {
    lwm2m_data_free(1, dataP);
    return 0;
  }
  dataP->value.asBuffer.length = bufferLen;
  memcpy(dataP->value.asBuffer.buffer, buffer, bufferLen);

  return 1;
}

static qapi_Net_LWM2M_Flat_Data_t * lwm2m_data_new(int size)
{
  qapi_Net_LWM2M_Flat_Data_t * dataP = NULL;

  if (size <= 0) return NULL;

  tx_byte_allocate(byte_pool_qcli, (VOID **)&dataP, size * sizeof(qapi_Net_LWM2M_Flat_Data_t), TX_NO_WAIT);

  if (dataP != NULL)
  {
    memset(dataP, 0, size * sizeof(qapi_Net_LWM2M_Flat_Data_t));
  }

  return dataP;
}

static void lwm2m_data_encode_string(const char * string,
                              qapi_Net_LWM2M_Flat_Data_t * dataP)
{
  size_t len = 0;
  int res = 0;

  if(dataP == NULL)
  {
    LOG_INFO("Passed NULL Arguments\n");
    return;
  }

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
    res = setBuffer(dataP, (uint8_t *)string, len);
  }

  if (res == 1)
  {
    dataP->type = QAPI_NET_LWM2M_TYPE_STRING_E;
  }
  else
  {
    dataP->type = QAPI_NET_LWM2M_TYPE_UNDEFINED;
  }
}

static void lwm2m_data_encode_int(int64_t value,
                           qapi_Net_LWM2M_Flat_Data_t * dataP)
{

  if( dataP == NULL ) 
  { 
    LOG_INFO(" Passed  Argument is NULL");
    return ;
  }

  dataP->type = QAPI_NET_LWM2M_TYPE_INTEGER_E;
  dataP->value.asInteger = value;
}

static void lwm2m_data_encode_float(double value,
                             qapi_Net_LWM2M_Flat_Data_t * dataP)
{
  LOG_INFO("value: %d", value);
  if( dataP == NULL ) 
  { 
    LOG_INFO("Passed  Argument is NULL");
    return ;
  }

  dataP->type = QAPI_NET_LWM2M_TYPE_FLOAT_E;
  dataP->value.asFloat = value;
}

static void lwm2m_data_encode_bool(bool value,
                            qapi_Net_LWM2M_Flat_Data_t * dataP)
{

  if( dataP == NULL ) 
  { 
    LOG_INFO(" Passed  Argument is NULL");
    return ;
  }

  dataP->type = QAPI_NET_LWM2M_TYPE_BOOLEAN_E;
  dataP->value.asBoolean = value;
}

static int lwm2m_data_decode_int(qapi_Net_LWM2M_Flat_Data_t * dataP,
                                 int64_t * valueP)
{
  int result;
  if ( dataP == NULL || valueP == NULL ) 
  {
    LOG_INFO("Passed  Argument is NULL");
    return 0;
  }

  switch (dataP->type)
  {
    case QAPI_NET_LWM2M_TYPE_INTEGER_E:
      *valueP = dataP->value.asInteger;
      result = 1;
      break;

    case QAPI_NET_LWM2M_TYPE_STRING_E:
    case QAPI_NET_LWM2M_TYPE_OPAQUE_E:
      result = utils_plainTextToInt64(dataP->value.asBuffer.buffer, dataP->value.asBuffer.length, valueP);
      break;

    default:
      return 0;
  }

  return result;
}

static void lwm2m_data_include(qapi_Net_LWM2M_Flat_Data_t * subData,
                               size_t count,
                               qapi_Net_LWM2M_Flat_Data_t * data)
{

  if (subData == NULL || count == 0 || data  == NULL ) return;

  switch (subData[0].type)
  {
    case QAPI_NET_LWM2M_TYPE_STRING_E:
    case QAPI_NET_LWM2M_TYPE_OPAQUE_E:
    case QAPI_NET_LWM2M_TYPE_INTEGER_E:
    case QAPI_NET_LWM2M_TYPE_FLOAT_E:
    case QAPI_NET_LWM2M_TYPE_BOOLEAN_E:
    case QAPI_NET_LWM2M_TYPE_OBJECT_LINK:
    case QAPI_NET_LWM2M_TYPE_MULTIPLE_RESOURCE:
      data->type = QAPI_NET_LWM2M_TYPE_OBJECT_INSTANCE;
      break;
    case QAPI_NET_LWM2M_TYPE_OBJECT_INSTANCE:
      data->type = QAPI_NET_LWM2M_TYPE_OBJECT;
      break;
    default:
      return;
  }
  data->value.asChildren.count = count;
  data->value.asChildren.array = subData;
}

static void lwm2m_data_encode_instances(qapi_Net_LWM2M_Flat_Data_t * subData,
                                        size_t count,
                                        qapi_Net_LWM2M_Flat_Data_t * data)
{

  if(data == NULL) 
  { 
    LOG_INFO(" Passed  Argument is NULL");
    return ;
  }

  lwm2m_data_include(subData, count, data);
  data->type = QAPI_NET_LWM2M_TYPE_MULTIPLE_RESOURCE;
}


/**
 * @fn utils_copyValue() 
 * @brief This function is used to copy value input string into output string
 * @param dst destination string
 * @param src source string
 * @param len length to be copied
 * @return  void
 */
static void utils_copyValue(void * dst,
    const void * src,
    size_t len)
{ 
#ifdef LWM2M_LITTLE_ENDIAN
  size_t i = 0;
#endif  
  if (dst == NULL || src == NULL)
  {
    LOG_INFO("LWM2M_LOG:Passed NULL Arguments\n");
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
 * @fn utils_opaqueToFloat()
 * @brief This function is used to convert opaque to Float
 * @param buffer input opaque buffer
 * @param buffer_len buffer length
 * @param dataP output float param
 * @return 0 on failure
 *         buffer_len on success
 */
static int utils_opaqueToFloat(const uint8_t * buffer,
    size_t buffer_len,
    double * dataP)
{ 
  if( buffer == NULL || dataP == NULL ) 
  { 
    LOG_INFO("LWM2M_LOG: Passed  Argument is NULL");
    return 0;
  }

  switch (buffer_len)
  {
    case 4:
      {
        float temp = 0.0f;

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

static int lwm2m_data_decode_float(qapi_Net_LWM2M_Flat_Data_t * dataP,
                                   double * valueP)
{
  int result = 0;
  if( dataP == NULL || valueP == NULL ) 
  {
    LOG_INFO(" Passed  Argument is NULL");
    return 0; 
  }

  switch (dataP->type)
  {
    case QAPI_NET_LWM2M_TYPE_FLOAT_E:
      *valueP = dataP->value.asFloat;
      result = 1;
      break;

    case QAPI_NET_LWM2M_TYPE_INTEGER_E:
      *valueP = (double)dataP->value.asInteger;
      result = 1;
      break;

    case QAPI_NET_LWM2M_TYPE_STRING_E:
      result = utils_plainTextToFloat64(dataP->value.asBuffer.buffer, dataP->value.asBuffer.length, valueP);
      break;
    case QAPI_NET_LWM2M_TYPE_OPAQUE_E:
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

  return result;
}

static int lwm2m_data_decode_bool(qapi_Net_LWM2M_Flat_Data_t * dataP,
                                  bool * valueP)
{
  int result; 
  if ( dataP == NULL || valueP == NULL ) 
  {
    LOG_INFO(" Passed  Argument is NULL");
    return 0;
  }

  switch (dataP->type)
  {
    case QAPI_NET_LWM2M_TYPE_BOOLEAN_E:
      *valueP = dataP->value.asBoolean;
      result = 1;
      break;

    case QAPI_NET_LWM2M_TYPE_STRING_E:
    case QAPI_NET_LWM2M_TYPE_OPAQUE_E:
      if (dataP->value.asBuffer.length != 1) return 0;

      switch (dataP->value.asBuffer.buffer[0])
      {
        case '0':
          *valueP = false;
          result = 1;
          break;
        case '1':
          *valueP = true;
          result = 1;
          break;
        default:
          result = 0;
          break;
      }
      break;

    default:
      result = 0;
      break;
  }

  return result;
}


int obj_inst_index_generic;
int obj_inst_created_generic[MAX_EXT_INSTANCE] = {0};
int obj_inst_index_presence;
int obj_inst_created_presence[MAX_EXT_INSTANCE] = {0};


#define SEN_UNIT "Celcius"
#define SEN_TYPE "PIR"

const QCLI_Command_t lwm2m_ext_cmd_list[] =
{
    /* Command_Function, Start_Thread, Command_String, Usage_String, Description, Abort_function */
    {init_ext_obj, false, "initapp", " [-app <app id: 0/1>] Provide application id", "Register the application. ",NULL},
    {config_client_ext_obj, false, "configclient", " [-app <app id: 0/1>] <bootstrap-uri> <apn-name> <security-mode> Provide application id and bootstrap-uri/apn/security-mode", "Config LwM2M client for the application. ",NULL},
    {create_ext_obj, false, "create_obj", " [-app <app id: 0/1>] Provide application id", "Create an extensible object. ",NULL},
    {create_ext_obj_inst, false, "create_obj_inst", " [-app <app id: 0/1>] <object id> Provide application id, object id for instance to be created", "Create an extensible object instance ",NULL},
    {delete_ext_obj_inst, false, "delete_obj_inst", " [-app <app id: 0/1>] <object id> <instance id> Provide application id, object id and instance id of object instance to be deleted", "Delete an extensible object instance ",NULL},
    {ext_read, false, "read", " [-app <app id: 0/1>] <object id> <instance id>(optional) <resource id>(optional) Provide application id, object id, instance id & resource id to read value.", "Read on extensible object,object instance or resource level ",NULL},
    {ext_write, false, "write", " [-app <app id: 0/1>] <object id> <instance id> <resource id> <value> Provide application id, object id, instance id & resource id & value to write on resource.", "Write on extensible object resource level ",NULL},
    {ext_exec, false, "execute", " [-app <app id: 0/1>] <object id> <instance id> <resource id> Provide application id, object id instance id & resource id to execute on resource.", "Execute on extensible object resource level ",NULL},
    {display_template, false, "display_template", "", "Display extensible object template ",NULL},
    {deinit_ext_obj, false, "deinitapp", " [-app <app id: 0/1>] Provide application id", "De-register the application. ",NULL},
    {set_obj_link, false, "set_obj_link", " [-app <app id: 0/1>] <object id> <instance id> Provide application id, object id and instance id of the object link.", "Set an object link", NULL},
    {get_obj_link, false, "get_obj_link", " [-app <app id: 0/1>] Provide application id, object id and instance id of the object link.", "Retrieve object links", NULL},
    {set_server_lifetime, false, "set_srv_lifetime", " [-app <app id: 0/1>] <url> <lifetime> Provide application id, URL info, lifetime information of server", "Set server lifetime", NULL},
    {get_server_lifetime, false, "get_srv_lifetime", " [-app <app id: 0/1>] <url> Provide application id, URL info information of server", "Retrieve server lifetime", NULL},
    {res_inst_create, false, "res_inst_create", " [-app <app id: 0/1>] <object id> <instance id>  <resource id> <resource instance id> <value> Provide application id, object id instance id resource id and" 
     "resource instance id and value to create resource instance.", "resource instance creation.", NULL},
    {res_inst_delete, false, "res_inst_delete", " [-app <app id: 0/1>] <object id> <instance id>  <resource id> <resource instance id> Provide application id, object id instance id resource id and "
     "resource instance id to delete resource instance.", "resource instance deletion.", NULL}
};

qapi_Net_LWM2M_Data_t *object_generic = NULL;
qapi_Net_LWM2M_Data_t *object_presence = NULL;
qapi_Net_LWM2M_Observed_t *ext_observed_list = NULL;
qapi_Net_LWM2M_Pending_Observed_t *ext_pending_observed_list = NULL;
qapi_Net_LWM2M_Observed_t *ext_multi_observed_list = NULL;
qapi_Net_LWM2M_Pending_Observed_t *ext_multi_pending_observed_list = NULL;

gen_multi_res_t *gen_multi_res = NULL;
pre_multi_res_t *pre_multi_res = NULL;

const QCLI_Command_Group_t lwm2m_ext_cmd_group =
{
    "LWM2M_EXT",              /* Group_String: will display cmd prompt as "LWM2M_EXT> " */
    sizeof(lwm2m_ext_cmd_list)/sizeof(lwm2m_ext_cmd_list[0]),   /* Command_Count */
    lwm2m_ext_cmd_list        /* Command_List */
};

QCLI_Group_Handle_t lwm2m_ext_handle;     /* Handle for Lwm2m extensible Command Group. */

/********************************************************************************
 * This function is used to register the LWM2M Extensible Command Group with QCLI.
 ********************************************************************************/
void Initialize_LWM2M_Ext_Demo(void)
{
    /* Attempt to reqister the Command Groups with the qcli framework.*/
    lwm2m_ext_handle = QCLI_Register_Command_Group(NULL, &lwm2m_ext_cmd_group);
    if (lwm2m_ext_handle)
    {
      QCLI_Printf(lwm2m_ext_handle, "LWM2M_EXT Registered\n");
    }

    return;
}

void notify_signal_cb_1(uint32 userData)
{
  int id = 0;
  multi_app_cmd_t* cmd_ptr = NULL;

  qapi_Net_LWM2M_Ext_t *ext_param = NULL;

  tx_byte_allocate(byte_pool_qcli, (VOID **)&ext_param, sizeof(qapi_Net_LWM2M_Ext_t), TX_NO_WAIT);

  if(ext_param == NULL)
  {
    LOG_INFO("Cannot assign memory for extensible object parameter");
    return;
  }

  memset(ext_param, 0x00, sizeof(qapi_Net_LWM2M_Ext_Obj_t));

  ext_param->app_data.ul_op.id = id;
  ext_param->app_data.ul_op.msg_type = QAPI_NET_LWM2M_EXT_NOTIFY_E;

  LOG_INFO("Switching from Timer to application callback context");

  if(id == 0)
  {
    if((cmd_ptr = multi_app_get_cmd_buffer()) == NULL)
    {
      LOG_INFO("Cannot obtain command buffer");
      return;
    }
    else
    {
      cmd_ptr->cmd_data.cmd_id              = EXT_OBJ_APP_ORIGINATED_EVT;
      cmd_ptr->cmd_data.data               = (void *)ext_param;
  
      if(dam_app1_registerd == false)
      {
        QCLI_Printf(lwm2m_ext_handle,"Application %d not registerd ",id+1);
        tx_byte_release(ext_param);
        return;
      }
      LOG_INFO("Enqueue the command into the command queue. ");
      multi_app_enqueue_cmd_t1(cmd_ptr);
    }
  
    multi_app_set_signal_dam_t1();
  }

  return;
}

void notify_signal_cb_2(uint32 userData)
{
  int id = 1;
  multi_app_cmd_t* cmd_ptr = NULL;

  qapi_Net_LWM2M_Ext_t *ext_param = NULL;

  tx_byte_allocate(byte_pool_qcli, (VOID **)&ext_param, sizeof(qapi_Net_LWM2M_Ext_t), TX_NO_WAIT);

  if(ext_param == NULL)
  {
    LOG_INFO("Cannot assign memory for extensible object parameter");
    return;
  }

  memset(ext_param, 0x00, sizeof(qapi_Net_LWM2M_Ext_Obj_t));

  ext_param->app_data.ul_op.id = id;
  ext_param->app_data.ul_op.msg_type = QAPI_NET_LWM2M_EXT_NOTIFY_E;

  LOG_INFO("Switching from Timer to application callback context");

  if(id == 1)
  {
    if((cmd_ptr = multi_app_get_cmd_buffer()) == NULL)
    {
      LOG_INFO("Cannot obtain command buffer");
      return;
    }
    else
    {
      cmd_ptr->cmd_data.cmd_id              = EXT_OBJ_APP_ORIGINATED_EVT;
      cmd_ptr->cmd_data.data               = (void *)ext_param;
  
      if(dam_app2_registerd == false)
      {
        QCLI_Printf(lwm2m_ext_handle,"Application %d not registerd ",id+1);
        tx_byte_release(ext_param);
        return;
      }
      LOG_INFO("Enqueue the command into the command queue. ");
      multi_app_enqueue_cmd_t2(cmd_ptr);
    }
  
    multi_app_set_signal_dam_t2();
  }

  return;
}

bool check_readable(uint16_t obj_id, uint8_t id)
{
  bool res = false;

  //if(lwm2m_object_find(object_generic, obj_id))
  {
    if(obj_id == LWM2M_GENERIC_SENSOR_OBJECT_ID)
    {
      switch(id)
      {
        case RES_M_SENSOR_VALUE:
        case RES_O_SENSOR_UNITS:
        case RES_O_MIN_MEASURED_VAL:
        case RES_O_MAX_MEASURED_VAL:
        case RES_M_MIN_RANGE_VAL:
        case RES_M_MAX_RANGE_VAL:
        case RES_M_SENSOR_STATE:
        case RES_O_MEAN_VAL:
        case RES_O_SNSR_VALUES:
             res = true;
        break;
        default:
             res = false;
        break;
      }
    }
    else if(obj_id == LWM2M_PRESENCE_SENSOR_OBJECT_ID)
    {
      switch(id)
      {
        case RES_M_DIG_INPUT_STATE:
        case RES_O_DIG_INPUT_COUNTER:
        case RES_O_SENSOR_TYPE:
        case RES_O_BUSY_TO_CLEAR_DELAY:
        case RES_O_CLEAR_TO_BUSY_DELAY:
        case RES_O_SUP_SENSOR_TYPE:
             res = true;
        break;
        default:
             res = false;
        break;
      }
    }
  }
  return res;
}

bool check_writable(uint16_t obj_id, uint8_t id)
{
  bool res = false;

  //if(lwm2m_object_find(object_generic, obj_id))
  {
    if(obj_id == LWM2M_GENERIC_SENSOR_OBJECT_ID)
    {
      switch(id)
      {
        case RES_M_MIN_RANGE_VAL:
        case RES_M_MAX_RANGE_VAL:
        case RES_M_SENSOR_STATE:
        case RES_O_SNSR_VALUES:
             res = true;
        break;
        default:
             res = false;
        break;
      }
    }
    else if(obj_id == LWM2M_PRESENCE_SENSOR_OBJECT_ID)
    {
      switch(id)
      {
        case RES_O_BUSY_TO_CLEAR_DELAY:
        case RES_O_CLEAR_TO_BUSY_DELAY:
        case RES_O_SUP_SENSOR_TYPE:
             res = true;
        break;
        default:
             res = false;
        break;
      }
    }
  }

  return res;
}

bool check_executable(uint16_t obj_id, uint8_t id)
{
  bool res = false;

  //if(lwm2m_object_find(object_generic, obj_id))
  {
    if(obj_id == LWM2M_GENERIC_SENSOR_OBJECT_ID)
    {
      switch(id)
      {
        case RES_O_RESET_MEASURED_VAL:
             res = true;
        break;
        default:
             res = false;
        break;
      }
    }
    else if(obj_id == LWM2M_PRESENCE_SENSOR_OBJECT_ID)
    {
      switch(id)
      {
        case RES_O_RESET_DIG_INPUT_COUNTER:
             res = true;
        break;
        default:
             res = false;
        break;
      }
    }
  }

  return res;
}


static qapi_Status_t qapi_Net_LWM2M_Free_Resource_Info(qapi_Net_LWM2M_Resource_Info_t *resourceP)
{
  qapi_Net_LWM2M_Resource_Info_t *resource_linkP = NULL;
  if (resourceP == NULL)
  {
    return QAPI_ERROR;
  }
  while (resourceP != NULL)
  {
    resource_linkP =   resourceP->next;
    if((resourceP->type == QAPI_NET_LWM2M_TYPE_STRING_E) && resourceP->value.asBuffer.buffer)
    {
      tx_byte_release(resourceP->value.asBuffer.buffer);
    }
    tx_byte_release(resourceP);
    resourceP = resource_linkP;
  }
   return QAPI_OK;
}

static qapi_Status_t qapi_Net_LWM2M_Free_Single_Instance_Info(qapi_Net_LWM2M_Instance_Info_t *instanceP)
{
  qapi_Net_LWM2M_Instance_Info_t *instance_linkP = NULL;
  if (instanceP == NULL)
  {
    return QAPI_ERROR;
  }
  if(instanceP != NULL)
  {
    instance_linkP = instanceP->next;
    qapi_Net_LWM2M_Free_Resource_Info(instanceP->resource_info);
    tx_byte_release(instanceP);
    instanceP = instance_linkP;
  }
  return QAPI_OK;
}

uint8_t object_check_readable(qapi_Net_LWM2M_Data_t *object,
                              qapi_Net_LWM2M_Uri_t *uri)
{
  uint8_t result = QAPI_NET_LWM2M_IGNORE_E;
  qapi_Net_LWM2M_Data_t *target = NULL;
  qapi_Net_LWM2M_Flat_Data_t *data = NULL;
  int size;

  if(object == NULL || uri == NULL ) 
  {
    LOG_INFO(" fun [%s] file [%s]  Passed  Argument is NULL\n",__func__,__FILE__);
    return QAPI_NET_LWM2M_400_BAD_REQUEST_E;
  }

  target = lwm2m_object_find(object, uri->objectId);

  if(target == NULL)
  {
    return QAPI_NET_LWM2M_404_NOT_FOUND_E;
  }
  
  if(!(uri->flag & QAPI_NET_LWM2M_INSTANCE_ID_E)) 
  {
    return QAPI_NET_LWM2M_205_CONTENT_E;
  }
  
  if(lwm2m_instance_find(target->instance_info, uri->instanceId) == NULL)
  {
    return QAPI_NET_LWM2M_404_NOT_FOUND_E;
  }
  
  if(!(uri->flag & QAPI_NET_LWM2M_RESOURCE_ID_E)) 
  {
    return QAPI_NET_LWM2M_205_CONTENT_E;
  }

  size = 1;
  data = lwm2m_data_new(1);

  if(data == NULL)
  {
    return QAPI_NET_LWM2M_500_INTERNAL_SERVER_E;
  }
  
  data->id = uri->resourceId;

  result = read_data_ext(uri->instanceId, &size, &data, object, INVALID_RES_INST_ID);

  lwm2m_data_free(1, data);

  if(result == QAPI_OK)
  {
    result = QAPI_NET_LWM2M_205_CONTENT_E;
  }
  else if(result == (uint8_t)QAPI_ERR_NO_ENTRY)
  {
    result = QAPI_NET_LWM2M_404_NOT_FOUND_E;
  }
  else 
  {
    result = QAPI_NET_LWM2M_400_BAD_REQUEST_E;
  }

  return result;
}

uint8_t object_check_numeric(qapi_Net_LWM2M_Data_t *object,
                             qapi_Net_LWM2M_Uri_t *uri)
{
  uint8_t result = QAPI_NET_LWM2M_IGNORE_E;
  qapi_Net_LWM2M_Data_t *target = NULL;
  qapi_Net_LWM2M_Flat_Data_t *data = NULL;
  int size;

  if (object == NULL || uri == NULL ) 
  {
    LOG_INFO("fun [%s] file [%s]  Passed  Argument is NULL\n",__func__,__FILE__);
    return QAPI_NET_LWM2M_400_BAD_REQUEST_E;
  }

  if(!(uri->flag & QAPI_NET_LWM2M_RESOURCE_ID_E)) 
  {
    return QAPI_NET_LWM2M_405_METHOD_NOT_ALLOWED_E;
  }

  target = lwm2m_object_find(object, uri->objectId);

  if (target == NULL)
  {
    return QAPI_NET_LWM2M_404_NOT_FOUND_E;
  }

  size = 1;
  data = lwm2m_data_new(1);

  if(data == NULL)
  {
    return QAPI_NET_LWM2M_500_INTERNAL_SERVER_E;
  }
  
  data->id = uri->resourceId;

  result = read_data_ext(uri->instanceId, &size, &data, object, INVALID_RES_INST_ID);

  if (result == QAPI_OK)
  {
    switch (data->type)
    {
      case QAPI_NET_LWM2M_TYPE_INTEGER_E:
      case QAPI_NET_LWM2M_TYPE_FLOAT_E:
        result = (uint8_t)QAPI_NET_LWM2M_205_CONTENT_E;
        break;
      default:
        result = (uint8_t) QAPI_NET_LWM2M_405_METHOD_NOT_ALLOWED_E;
    }
  }
  else if(result == (uint8_t)QAPI_ERR_NO_ENTRY)
  {
    result = QAPI_NET_LWM2M_404_NOT_FOUND_E;
  }
  else 
  {
    result = QAPI_NET_LWM2M_400_BAD_REQUEST_E;
  }

  lwm2m_data_free(1, data);

  return result;
}

qapi_Status_t ext_object_callback_app1(qapi_Net_LWM2M_App_Handler_t handler, 
                                       qapi_Net_LWM2M_Server_Data_t *lwm2m_srv_data,
                                       void                         *user_data)
{
  multi_app_cmd_t*        cmd_ptr = NULL;
  qapi_Net_LWM2M_Ext_t *ext_param = NULL;

  tx_byte_allocate(byte_pool_qcli, (VOID **)&ext_param, sizeof(qapi_Net_LWM2M_Ext_t), TX_NO_WAIT);

  if(ext_param == NULL)
  {
    LOG_INFO("Cannot assign memory for buffer");
    return QAPI_ERR_NO_MEMORY;
  }

  memset(ext_param, 0x00, sizeof(qapi_Net_LWM2M_Ext_t));

  tx_byte_allocate(byte_pool_qcli, (VOID **)&(ext_param->app_data.dl_op.lwm2m_srv_data), 
                   sizeof(qapi_Net_LWM2M_Server_Data_t), TX_NO_WAIT);

  if(ext_param->app_data.dl_op.lwm2m_srv_data == NULL)
  {
    LOG_INFO("Cannot assign memory for buffer");
    return QAPI_ERR_NO_MEMORY;
  }
  memset(ext_param->app_data.dl_op.lwm2m_srv_data, 0x00, sizeof(qapi_Net_LWM2M_Server_Data_t));
  memcpy(ext_param->app_data.dl_op.lwm2m_srv_data, lwm2m_srv_data, sizeof(qapi_Net_LWM2M_Server_Data_t));
  ext_param->app_data.dl_op.handler = handler;
  if(lwm2m_srv_data->lwm2m_attr != NULL)
  {
    tx_byte_allocate(byte_pool_qcli, (VOID **)&(ext_param->app_data.dl_op.lwm2m_srv_data->lwm2m_attr), 
                     sizeof(qapi_Net_LWM2M_Attributes_t), TX_NO_WAIT);

    if(ext_param->app_data.dl_op.lwm2m_srv_data->lwm2m_attr == NULL)
    {
      LOG_INFO("Cannot assign memory for buffer");
      return QAPI_ERR_NO_MEMORY;
    }
    memset(ext_param->app_data.dl_op.lwm2m_srv_data->lwm2m_attr, 0x00, sizeof(qapi_Net_LWM2M_Attributes_t));
    memcpy(ext_param->app_data.dl_op.lwm2m_srv_data->lwm2m_attr, lwm2m_srv_data->lwm2m_attr, sizeof(qapi_Net_LWM2M_Attributes_t));
  }
  if(lwm2m_srv_data->payload != NULL && lwm2m_srv_data->payload_len != 0)
  {
    tx_byte_allocate(byte_pool_qcli, (VOID **)&(ext_param->app_data.dl_op.lwm2m_srv_data->payload), 
                     ext_param->app_data.dl_op.lwm2m_srv_data->payload_len, TX_NO_WAIT);

    if(ext_param->app_data.dl_op.lwm2m_srv_data->payload == NULL)
    {
      LOG_INFO("Cannot assign memory for buffer");
      return QAPI_ERR_NO_MEMORY;
    }
    memset(ext_param->app_data.dl_op.lwm2m_srv_data->payload, 0x00, ext_param->app_data.dl_op.lwm2m_srv_data->payload_len);
    memcpy(ext_param->app_data.dl_op.lwm2m_srv_data->payload, lwm2m_srv_data->payload, ext_param->app_data.dl_op.lwm2m_srv_data->payload_len);
  }
  ext_param->app_data.dl_op.data = user_data;

  LOG_INFO("Switching from QCLI to application callback context");

  if((cmd_ptr = multi_app_get_cmd_buffer()) == NULL)
  {
    LOG_INFO("Cannot obtain command buffer");
    return QAPI_ERR_NO_ENTRY;
  }
  else
  {
    cmd_ptr->cmd_data.cmd_id              = EXT_OBJ_CALL_BACK_EVT;
    cmd_ptr->cmd_data.data               = (void *)ext_param;

    if(dam_app1_registerd == false)
    {
      QCLI_Printf(lwm2m_ext_handle,"Application not registerd ");
      if(ext_param->app_data.dl_op.lwm2m_srv_data->lwm2m_attr != NULL)
      tx_byte_release(ext_param->app_data.dl_op.lwm2m_srv_data->lwm2m_attr);
      tx_byte_release(ext_param->app_data.dl_op.lwm2m_srv_data);
      tx_byte_release(ext_param);
      return QAPI_ERROR;
    }
    LOG_INFO("Enqueue the command into the command queue. ");
    multi_app_enqueue_cmd_t1(cmd_ptr);
  }

  multi_app_set_signal_dam_t1();

  return QAPI_OK;

}

qapi_Status_t ext_object_callback_app2(qapi_Net_LWM2M_App_Handler_t handler, 
                                       qapi_Net_LWM2M_Server_Data_t *lwm2m_srv_data,
                                       void                         *user_data)
{
  multi_app_cmd_t*        cmd_ptr = NULL;
  qapi_Net_LWM2M_Ext_t *ext_param = NULL;

  tx_byte_allocate(byte_pool_qcli, (VOID **)&ext_param, sizeof(qapi_Net_LWM2M_Ext_t), TX_NO_WAIT);

  if(ext_param == NULL)
  {
    LOG_INFO("Cannot assign memory for buffer");
    return QAPI_ERR_NO_MEMORY;
  }

  memset(ext_param, 0x00, sizeof(qapi_Net_LWM2M_Ext_t));
   
  tx_byte_allocate(byte_pool_qcli, (VOID **)&(ext_param->app_data.dl_op.lwm2m_srv_data), 
                   sizeof(qapi_Net_LWM2M_Server_Data_t), TX_NO_WAIT);
  
  if(ext_param->app_data.dl_op.lwm2m_srv_data == NULL)
  {
    LOG_INFO("Cannot assign memory for buffer");
    return QAPI_ERR_NO_MEMORY;
  }
  memset(ext_param->app_data.dl_op.lwm2m_srv_data, 0x00, sizeof(qapi_Net_LWM2M_Server_Data_t));
  memcpy(ext_param->app_data.dl_op.lwm2m_srv_data, lwm2m_srv_data, sizeof(qapi_Net_LWM2M_Server_Data_t));
  ext_param->app_data.dl_op.handler = handler;
  if(lwm2m_srv_data->lwm2m_attr != NULL)
  {
    tx_byte_allocate(byte_pool_qcli, (VOID **)&(ext_param->app_data.dl_op.lwm2m_srv_data->lwm2m_attr), 
                     sizeof(qapi_Net_LWM2M_Attributes_t), TX_NO_WAIT);
  
    if(ext_param->app_data.dl_op.lwm2m_srv_data->lwm2m_attr == NULL)
    {
      LOG_INFO("Cannot assign memory for buffer");
      return QAPI_ERR_NO_MEMORY;
    }
    memset(ext_param->app_data.dl_op.lwm2m_srv_data->lwm2m_attr, 0x00, sizeof(qapi_Net_LWM2M_Attributes_t));
    memcpy(ext_param->app_data.dl_op.lwm2m_srv_data->lwm2m_attr, lwm2m_srv_data->lwm2m_attr, sizeof(qapi_Net_LWM2M_Attributes_t));
  }
  if(lwm2m_srv_data->payload != NULL && lwm2m_srv_data->payload_len != 0)
  {
    tx_byte_allocate(byte_pool_qcli, (VOID **)&(ext_param->app_data.dl_op.lwm2m_srv_data->payload), 
                     ext_param->app_data.dl_op.lwm2m_srv_data->payload_len, TX_NO_WAIT);

    if(ext_param->app_data.dl_op.lwm2m_srv_data->payload == NULL)
    {
      LOG_INFO("Cannot assign memory for buffer");
      return QAPI_ERR_NO_MEMORY;
    }
    memset(ext_param->app_data.dl_op.lwm2m_srv_data->payload, 0x00, ext_param->app_data.dl_op.lwm2m_srv_data->payload_len);
    memcpy(ext_param->app_data.dl_op.lwm2m_srv_data->payload, lwm2m_srv_data->payload, ext_param->app_data.dl_op.lwm2m_srv_data->payload_len);
  }
  ext_param->app_data.dl_op.data = user_data;

  LOG_INFO("Switching from QCLI to application callback context");

  if((cmd_ptr = multi_app_get_cmd_buffer()) == NULL)
  {
    LOG_INFO("Cannot obtain command buffer");
    return QAPI_ERR_NO_ENTRY;
  }
  else
  {
    cmd_ptr->cmd_data.cmd_id              = EXT_OBJ_CALL_BACK_EVT;
    cmd_ptr->cmd_data.data               = (void *)ext_param;

    if(dam_app2_registerd == false)
    {
      QCLI_Printf(lwm2m_ext_handle,"Application not registerd ");
      if(ext_param->app_data.dl_op.lwm2m_srv_data->lwm2m_attr != NULL)
      tx_byte_release(ext_param->app_data.dl_op.lwm2m_srv_data->lwm2m_attr);
      tx_byte_release(ext_param->app_data.dl_op.lwm2m_srv_data);
      tx_byte_release(ext_param);
      return QAPI_ERROR;
    }
    LOG_INFO("Enqueue the command into the command queue. ");
    multi_app_enqueue_cmd_t2(cmd_ptr);
  }

  multi_app_set_signal_dam_t2();

  return QAPI_OK;

}

qapi_Status_t ext_obj_delete_all(qapi_Net_LWM2M_App_Handler_t handler, 
                                 qapi_Net_LWM2M_Server_Data_t op_data)
{
  int i = 0;
  qapi_Net_LWM2M_Instance_Info_t *instance = NULL;
  qapi_Net_LWM2M_Data_t *object = NULL;
  qapi_Net_LWM2M_Object_Info_t instance_info = {0};
  qapi_Net_LWM2M_Uri_t uri;
  gen_multi_res_t *mult_inst1 = NULL;
  pre_multi_res_t *mult_inst2 = NULL;

  if(handler == ext_object_handler_app1)
  {
    object = lwm2m_object_find(object_generic, LWM2M_GENERIC_SENSOR_OBJECT_ID);
    if(object != NULL)
    {
      for(i =0; i< MAX_EXT_INSTANCE; i++)
      {
        if(obj_inst_created_generic[i] == 1)
        {
          instance = lwm2m_instance_find(object->instance_info, i);
          if(instance != NULL)
          {
            memset(&uri, 0x00, sizeof(qapi_Net_LWM2M_Uri_t));
            uri.flag =  (QAPI_NET_LWM2M_OBJECT_ID_E | QAPI_NET_LWM2M_INSTANCE_ID_E);
            uri.objectId = LWM2M_GENERIC_SENSOR_OBJECT_ID;
            uri.instanceId = i;

            observe_delete_ext_handle_req(handler, &uri);

            gen_multi_res = lwm2m_gen_inst_remove(gen_multi_res, &mult_inst1, i);
            if(mult_inst1 != NULL)
            {
              lwm2m_resource_inst_int_free(mult_inst1->sensors_val);
              mult_inst1->sensors_val= NULL;
              tx_byte_release(mult_inst1);
              mult_inst1 = NULL;
            }

            object->instance_info = lwm2m_instance_remove(object->instance_info, &instance, i);
            if(!qapi_Net_LWM2M_Free_Single_Instance_Info(instance))
            {
              LOG_INFO("Instance id %d cleared ", i);
              object->no_instances --;
              obj_inst_created_generic[i] = 0;
              instance = NULL;

              instance_info.no_object_info = 1;
              tx_byte_allocate(byte_pool_qcli, (VOID **)&instance_info.id_info, sizeof(qapi_Net_LWM2M_Id_Info_t), TX_NO_WAIT);
              if(instance_info.id_info == NULL)
              {
                return QAPI_ERR_NO_MEMORY;
              }
              memset(instance_info.id_info, 0x00, sizeof(qapi_Net_LWM2M_Id_Info_t));
              instance_info.id_info->id_set |= QAPI_NET_LWM2M_OBJECT_ID_E;
              instance_info.id_info->id_set |= QAPI_NET_LWM2M_INSTANCE_ID_E;
              instance_info.id_info->object_ID = LWM2M_GENERIC_SENSOR_OBJECT_ID;
              instance_info.id_info->instance_ID = i;

              qapi_Net_LWM2M_Delete_Object_Instance(handler, &instance_info);

              if(instance_info.id_info)
              {
                tx_byte_release(instance_info.id_info);
                instance_info.id_info = NULL;
              }
            }
          }
        }
      }
      LOG_INFO("LWM2M_LOG: Deleted all instances.\n");
      return QAPI_OK;
    }
  }

  else if(handler == ext_object_handler_app2)
  {
    object = lwm2m_object_find(object_presence, LWM2M_PRESENCE_SENSOR_OBJECT_ID);
    if(object != NULL)
    {
      for(i =0; i< MAX_EXT_INSTANCE; i++)
      {
        if(obj_inst_created_presence[i] == 1)
        {
          instance = lwm2m_instance_find(object->instance_info, i);
          if(instance != NULL)
          {
            memset(&uri, 0x00, sizeof(qapi_Net_LWM2M_Uri_t));
            uri.flag =  (QAPI_NET_LWM2M_OBJECT_ID_E | QAPI_NET_LWM2M_INSTANCE_ID_E);
            uri.objectId = LWM2M_PRESENCE_SENSOR_OBJECT_ID;
            uri.instanceId = i;

            observe_delete_ext_handle_req(handler, &uri);
 
            pre_multi_res = lwm2m_pre_inst_remove(pre_multi_res, &mult_inst2, i);
            if(mult_inst2 != NULL)
            {
              lwm2m_resource_inst_string_free(mult_inst2->sensor_types);
              mult_inst2->sensor_types = NULL;
              tx_byte_release(mult_inst2);
              mult_inst2 = NULL;
            }

            object->instance_info = lwm2m_instance_remove(object->instance_info, &instance, i);
            if(!qapi_Net_LWM2M_Free_Single_Instance_Info(instance))
            {
              LOG_INFO("Instance id %d cleared ", i);
              object->no_instances --;
              obj_inst_created_presence[i] = 0;
              instance = NULL;

              instance_info.no_object_info = 1;
              tx_byte_allocate(byte_pool_qcli, (VOID **)&instance_info.id_info, sizeof(qapi_Net_LWM2M_Id_Info_t), TX_NO_WAIT);
              if(instance_info.id_info == NULL)
              {
                return QAPI_ERR_NO_MEMORY;
              }
              memset(instance_info.id_info, 0x00, sizeof(qapi_Net_LWM2M_Id_Info_t));
              instance_info.id_info->id_set |= QAPI_NET_LWM2M_OBJECT_ID_E;
              instance_info.id_info->id_set |= QAPI_NET_LWM2M_INSTANCE_ID_E;
              instance_info.id_info->object_ID = LWM2M_PRESENCE_SENSOR_OBJECT_ID;
              instance_info.id_info->instance_ID = i;

              qapi_Net_LWM2M_Delete_Object_Instance(handler, &instance_info);

              if(instance_info.id_info)
              {
                tx_byte_release(instance_info.id_info);
                instance_info.id_info = NULL;
              }
            }
          }
        }
      }
      LOG_INFO("LWM2M_LOG: Deleted all instances.\n");
      return QAPI_OK;
    }
  }
  return QAPI_ERROR;
}

qapi_Status_t ext_obj_handle_event(qapi_Net_LWM2M_App_Handler_t handler, 
                                   qapi_Net_LWM2M_Server_Data_t op_data)
{
  int i = 0;
  qapi_Net_LWM2M_Instance_Info_t *instance = NULL;
  qapi_Net_LWM2M_Data_t *object = NULL;
  gen_multi_res_t *mult_inst1 = NULL;
  pre_multi_res_t *mult_inst2 = NULL;

  if(handler == NULL)
  {
    LOG_INFO("Application handle is not valid for current application");
    return QAPI_ERR_INVALID_PARAM;
  }

  if(op_data.event == QAPI_NET_LWM2M_SLEEP_E)
  {
    lwm2m_client_in_sleep_state = true;
  }

  if(op_data.event == QAPI_NET_LWM2M_WAKEUP_E)
  {
    lwm2m_client_in_sleep_state = false;
  }

  if(op_data.event == QAPI_NET_LWM2M_REGISTERTION_COMPELTED_E)
  {
    lwm2m_client_in_sleep_state = false;
  }

  if(op_data.event == QAPI_NET_LWM2M_DEVICE_FACTORY_RESET_E)
  {
    if(handler == ext_object_handler_app1)
    {
      stop_ext_obs_notify_timer(handler);
      ext_object_handler_app1 = NULL;

      object = lwm2m_object_find(object_generic, LWM2M_GENERIC_SENSOR_OBJECT_ID);
      if(object != NULL)
      {
        for(i =0; i< MAX_EXT_INSTANCE; i++)
        {
          if(obj_inst_created_generic[i] == 1)
          {
            instance = lwm2m_instance_find(object->instance_info, i);
            if(instance != NULL)
            {
              gen_multi_res = lwm2m_gen_inst_remove(gen_multi_res, &mult_inst1, i);
              if(mult_inst1 != NULL)
              {
                lwm2m_resource_inst_int_free(mult_inst1->sensors_val);
                mult_inst1->sensors_val= NULL;
                tx_byte_release(mult_inst1);
                mult_inst1 = NULL;
              }
              object->instance_info = lwm2m_instance_remove(object->instance_info, &instance, i);
              if(!qapi_Net_LWM2M_Free_Single_Instance_Info(instance))
              {
                LOG_INFO("Instance id %d cleared ", i);
                object->no_instances --;
                obj_inst_created_generic[i] = 0;
                instance = NULL;
              }
            }
          }
        }
      }
    }

    else if(handler == ext_object_handler_app2)
    {
      stop_ext_obs_notify_timer(handler);
      ext_object_handler_app2 = NULL;

      object = lwm2m_object_find(object_presence, LWM2M_PRESENCE_SENSOR_OBJECT_ID);
      if(object != NULL)
      {
        for(i =0; i< MAX_EXT_INSTANCE; i++)
        {
          if(obj_inst_created_presence[i] == 1)
          {
            instance = lwm2m_instance_find(object->instance_info, i);
            if(instance != NULL)
            {
              pre_multi_res = lwm2m_pre_inst_remove(pre_multi_res, &mult_inst2, i);
              if(mult_inst2 != NULL)
              {
                lwm2m_resource_inst_string_free(mult_inst2->sensor_types);
                mult_inst2->sensor_types = NULL;
                tx_byte_release(mult_inst2);
                mult_inst2 = NULL;
              }
              object->instance_info = lwm2m_instance_remove(object->instance_info, &instance, i);
              if(!qapi_Net_LWM2M_Free_Single_Instance_Info(instance))
              {
                LOG_INFO("Instance id %d cleared ", i);
                object->no_instances --;
                obj_inst_created_presence[i] = 0;
                instance = NULL;
              }
            }
          }
        }
      }
    }
    LOG_INFO("LWM2M_LOG: Extensible object application de-registered successfully.\n");

  }

  if(op_data.event)
  {
    ext_obj_event = op_data.event;
    return QAPI_OK;
  }

  return QAPI_ERROR;
}

/**
 * @fn     ext_object_register()
 *
 * @brief  This fucntion is used to register for Extensible object application.
 *
 * @param  int id      Identifier for different extensible application 
 * @return QAPI_OK     Success case
 *         QAPI_ERROR  Failure case
 */
qapi_Status_t ext_object_register(int id)
{
  qapi_Status_t result = QAPI_ERROR;
  void *user_data = NULL;

  if(id == 0)
  {
    if(ext_object_handler_app1 != NULL)
    {
      LOG_INFO("LWM2M_LOG: Extensible object application 1 already registered.\n");
      QCLI_Printf(lwm2m_ext_handle, "Application 1 already registered.\n");
      return result;
    }
  
    result = qapi_Net_LWM2M_Register_App_Extended(&ext_object_handler_app1, 
                                                  user_data,
                                                  ext_object_callback_app1);
    if(result != QAPI_OK)
    {
      ext_object_handler_app1 = NULL;
      user_data = NULL;	  
    }

	LOG_INFO("ext_object_register : User space memory pool, %x", byte_pool_qcli);
	

    qapi_Net_LWM2M_Pass_Pool_Ptr(ext_object_handler_app1, byte_pool_qcli);
  }

  else if(id == 1)
  {
    if(ext_object_handler_app2 != NULL)
    {
      LOG_INFO("LWM2M_LOG: Extensible object application 2 already registered.\n");
      QCLI_Printf(lwm2m_ext_handle, "Application 2 already registered.\n");
      return result;
    }
  
    result = qapi_Net_LWM2M_Register_App_Extended(&ext_object_handler_app2, 
                                                  user_data,
                                                  ext_object_callback_app2);
    if(result != QAPI_OK)
    {
      ext_object_handler_app2 = NULL;
      user_data = NULL;	  
    }

    qapi_Net_LWM2M_Pass_Pool_Ptr(ext_object_handler_app2, byte_pool_qcli);
  }


  if(result == QAPI_OK)
  {
    if (id == 0)
    {
      dam_app1_registerd = true;
    }
    else if (id == 1)
    {
      dam_app2_registerd = true;
    }

    QCLI_Printf(lwm2m_ext_handle, "Application %d registered successfully.\n", id+1);
    LOG_INFO("LWM2M_LOG: Extensible object application %d registered successfully.\n", id+1);
  }

  else
  {
    QCLI_Printf(lwm2m_ext_handle, "Error in application %d registeration.\n", id+1);
    LOG_INFO("LWM2M_LOG: Error in Extensible object application %d registration.\n", id+1);
  }

  return result;
}

qapi_Status_t ext_object_deregister(int id)
{
  qapi_Status_t result = QAPI_ERROR;
  int i = 0;
  qapi_Net_LWM2M_Instance_Info_t *instance = NULL;
  qapi_Net_LWM2M_Data_t *object = NULL;
  gen_multi_res_t *mult_inst1 = NULL;
  pre_multi_res_t *mult_inst2 = NULL;

  if(id == 0)
  {
    if(ext_object_handler_app1 == NULL)
    {
      LOG_INFO("LWM2M_LOG: Extensible object application 1 not registered.\n");
      QCLI_Printf(lwm2m_ext_handle, "Application 1 not registered.\n");
      return result;
    }

    result = qapi_Net_LWM2M_DeRegister_App(ext_object_handler_app1);
  }

  else if(id == 1 )
  {
    if(ext_object_handler_app2 == NULL)
    {
      LOG_INFO("LWM2M_LOG: Extensible object application 2 not registered.\n");
      QCLI_Printf(lwm2m_ext_handle, "Application 2 not registered.\n");
      return result;
    }

    result = qapi_Net_LWM2M_DeRegister_App(ext_object_handler_app2);
  }

  if(result == QAPI_OK)
  {
    if(id == 0)
    {
    
      dam_app1_registerd = false;
      ext_object_handler_app1 = NULL;

      object = lwm2m_object_find(object_generic, LWM2M_GENERIC_SENSOR_OBJECT_ID);
      if(object != NULL)
      {
        for(i =0; i< MAX_EXT_INSTANCE; i++)
        {
          if(obj_inst_created_generic[i] == 1)
          {
            instance = lwm2m_instance_find(object->instance_info, i);
            if(instance != NULL)
            {
              gen_multi_res = lwm2m_gen_inst_remove(gen_multi_res, &mult_inst1, i);
              if(mult_inst1 != NULL)
              {
                lwm2m_resource_inst_int_free(mult_inst1->sensors_val);
                mult_inst1->sensors_val= NULL;
                tx_byte_release(mult_inst1);
                mult_inst1 = NULL;
              }
              object->instance_info = lwm2m_instance_remove(object->instance_info, &instance, i);
              if(!qapi_Net_LWM2M_Free_Single_Instance_Info(instance))
              {
                LOG_INFO("Instance id %d cleared ", i);
                object->no_instances --;
                obj_inst_created_generic[i] = 0;
                instance = NULL;
              }
            }
          }
        }
      }
    }

    else if(id == 1)
    {
      dam_app2_registerd = false;
      ext_object_handler_app2 = NULL;

      object = lwm2m_object_find(object_presence, LWM2M_PRESENCE_SENSOR_OBJECT_ID);
      if(object != NULL)
      {
        for(i =0; i< MAX_EXT_INSTANCE; i++)
        {
          if(obj_inst_created_presence[i] == 1)
          {
            instance = lwm2m_instance_find(object->instance_info, i);
            if(instance != NULL)
            {
              pre_multi_res = lwm2m_pre_inst_remove(pre_multi_res, &mult_inst2, i);
              if(mult_inst2 != NULL)
              {
                lwm2m_resource_inst_string_free(mult_inst2->sensor_types);
                mult_inst2->sensor_types = NULL;
                tx_byte_release(mult_inst2);
                mult_inst2 = NULL;
              }
              object->instance_info = lwm2m_instance_remove(object->instance_info, &instance, i);
              if(!qapi_Net_LWM2M_Free_Single_Instance_Info(instance))
              {
                LOG_INFO("Instance id %d cleared ", i);
                object->no_instances --;
                obj_inst_created_presence[i] = 0;
                instance = NULL;
              }
            }
          }
        }
      }
    }
    QCLI_Printf(lwm2m_ext_handle, "Application %d de-registered. successfully\n", id+1);
    LOG_INFO("LWM2M_LOG: Extensible object application de-registered successfully.\n");
  }

  else
  {
    QCLI_Printf(lwm2m_ext_handle, "Error in Application de-registration\n", id+1);
    LOG_INFO("LWM2M_LOG: Error in Extensible object application de-registration.\n");
  }

  return result;
}

/**
 * @fn     ext_object_configclient()
 *
 * @brief  This fucntion is used to config lwm2m client for Extensible object application.
 *
 * @param  int id      Identifier for different extensible application 
 * @return QAPI_OK     Success case
 *         QAPI_ERROR  Failure case
 */
qapi_Status_t ext_object_configclient(int id)
{
  qapi_Status_t result = QAPI_ERROR;

  if(id == 0)
  {
    if(ext_object_handler_app1 == NULL)
    {
      LOG_INFO("LWM2M_LOG: Extensible object application 1 not registered.\n");
      QCLI_Printf(lwm2m_ext_handle, "Application 1 not registered.\n");
      return QAPI_ERROR;
    }
  
    result = qapi_Net_LWM2M_ConfigClient(ext_object_handler_app1, ext_object_clientconfig_data);
  }

  else if(id == 1)
  {
    if(ext_object_handler_app2 == NULL)
    {
      LOG_INFO("LWM2M_LOG: Extensible object application 2 not registered.\n");
      QCLI_Printf(lwm2m_ext_handle, "Application 2 not registered.\n");
      return QAPI_ERROR;
    }
  
    result = qapi_Net_LWM2M_ConfigClient(ext_object_handler_app2, ext_object_clientconfig_data);
  }


  if(result == QAPI_OK)
  {
    QCLI_Printf(lwm2m_ext_handle, "Application %d config client successfully.\n", id+1);
    LOG_INFO("LWM2M_LOG: Extensible object application %d config client successfully.\n", id+1);
  }

  else
  {
    QCLI_Printf(lwm2m_ext_handle, "Error in application %d config client.\n", id+1);
    LOG_INFO("LWM2M_LOG: Error in Extensible object application %d config client.\n", id+1);
  }

  return result;
}

/**
 * @fn     create_object()
 *
 * @brief  This fucntion is used to create object for Extensible object application.
 *
 * @param  int id      Identifier for different extensible application 
 * @return QAPI_OK     Success case
 *         QAPI_ERROR  Failure case
 */
qapi_Status_t create_object(int id)
{
  qapi_Net_LWM2M_Data_t *object = NULL;

  if(id == 0)
  {
    if(object_generic == NULL)
    {
      tx_byte_allocate(byte_pool_qcli, (VOID **)&object, sizeof(qapi_Net_LWM2M_Data_t), TX_NO_WAIT);
      if(object == NULL)
      {
        LOG_INFO("Memory allocation failure. \n");
        QCLI_Printf(lwm2m_ext_handle, "Object not created.\n");
        return QAPI_ERR_NO_MEMORY;
      }
  
      memset(object, 0x00, sizeof(qapi_Net_LWM2M_Data_t));
  
      object->object_ID = LWM2M_GENERIC_SENSOR_OBJECT_ID;
      object->next = NULL;
      object->no_instances = 0;
      object->instance_info = NULL;
  
      object_generic = lwm2m_object_add(object_generic,
                                        object);
  
      if(object_generic!= NULL)
      {
        LOG_INFO("Extensible object 1001 created. \n");
        QCLI_Printf(lwm2m_ext_handle, "Object 1001 created.\n");
        return QAPI_OK;
      }
      else
      {
        LOG_INFO("Error in creation of Extensible object 1001. \n");
        QCLI_Printf(lwm2m_ext_handle, "Error in creation of object 1001. \n");
        return QAPI_ERROR;
      }
    }
  }

  else if(id == 1)
  {
    if(object_presence == NULL)
    {
      tx_byte_allocate(byte_pool_qcli, (VOID **)&object, sizeof(qapi_Net_LWM2M_Data_t), TX_NO_WAIT);
      if(object == NULL)
      {
        LOG_INFO("Memory allocation failure. \n");
        QCLI_Printf(lwm2m_ext_handle, "Object not created.\n");
        return QAPI_ERR_NO_MEMORY;
      }
  
      memset(object, 0x00, sizeof(qapi_Net_LWM2M_Data_t));
  
      object->object_ID = LWM2M_PRESENCE_SENSOR_OBJECT_ID;
      object->next = NULL;
      object->no_instances = 0;
      object->instance_info = NULL;
  
      object_presence = lwm2m_object_add(object_presence,
                                         object);
  
      if(object_presence!= NULL)
      {
        LOG_INFO("Extensible object 1002 created. \n");
        QCLI_Printf(lwm2m_ext_handle, "Object 1002 created.\n");
        return QAPI_OK;
      }
      else
      {
        LOG_INFO("Error in creation of Extensible object 1002. \n");
        QCLI_Printf(lwm2m_ext_handle, "Error in creation of object 1002. \n");
        return QAPI_ERROR;
      }
    }
  }

  else
  {
    LOG_INFO("Extensible object 1001 & 1002 already created.\n");
    QCLI_Printf(lwm2m_ext_handle, "Object 1001 & 1002 already created. \n");
    return QAPI_ERROR;
  }

  return QAPI_ERROR;
}

bool ext_write_resource_inst_val(ext_resource_instance_int_list_t** rsList,
                                 uint16_t resId, uint16_t resValue)
{
  bool ret = false;
  ext_resource_instance_int_list_t *resInst = NULL, *temp = *rsList;
  while(temp)
  {
    if(temp->resInstId == resId)
    {
      temp->InstValue = resValue;
      return true;
    }
    temp = temp->next;
  }

  tx_byte_allocate(byte_pool_qcli, (VOID **)&resInst, 
                   sizeof(ext_resource_instance_int_list_t), TX_NO_WAIT);
  if (resInst==NULL)
  {
    return ret;
  }
  else
  {
    memset(resInst, 0, sizeof(ext_resource_instance_int_list_t));
    resInst->resInstId   = resId;
    resInst->InstValue   = resValue;

    *rsList = (ext_resource_instance_int_list_t*)lwm2m_resource_inst_int_add(*rsList, resInst);
    ret = true;
  }

  return ret;
}

bool ext_write_string_resource_inst_val(ext_resource_instance_string_list_t** rsList,
                                        uint16_t resId, char* resValue)
{
  bool ret = false;
  ext_resource_instance_string_list_t *resInst = NULL, *temp = *rsList;
  while(temp)
  {
    if(temp->resInstId == resId)
    {
      strlcpy(temp->InstValue,resValue, MAX_STRING_LEN);
      return true;
    }
    temp = temp->next;
  }

  tx_byte_allocate(byte_pool_qcli, (VOID **)&resInst, 
                   sizeof(ext_resource_instance_string_list_t), TX_NO_WAIT);
  if (resInst==NULL)
  {
    return ret;
  }
  else
  {
    memset(resInst, 0, sizeof(ext_resource_instance_string_list_t));
    resInst->resInstId   = resId;
    strlcpy(resInst->InstValue,resValue, MAX_STRING_LEN);
    *rsList = (ext_resource_instance_string_list_t*)lwm2m_resource_inst_string_add(*rsList, resInst);
    ret = true;
  }

  return ret;
}

bool ext_delete_resource_inst_val(ext_resource_instance_int_list_t** rsList,
                                  uint16_t resId)
{
  bool ret = false;
  ext_resource_instance_int_list_t *resInst = NULL;

  if(*rsList == NULL)
  {
    LOG_INFO("fun [%s] file [%s]  Passed  Argument is NULL\n",__func__,__FILE__);
    return ret;
  }

  *rsList = lwm2m_resource_inst_int_remove(*rsList, &resInst, resId);

  if (resInst==NULL)
  {
    return ret;
  }
  else
  {
    tx_byte_release(resInst);
    resInst = NULL;
    ret = true;
  }

  return ret;
}

bool ext_delete_string_resource_inst_val(ext_resource_instance_string_list_t** rsList,
                                        uint16_t resId)
{
  bool ret = false;
  ext_resource_instance_string_list_t *resInst = NULL;

  if(*rsList == NULL)
  {
    LOG_INFO("fun [%s] file [%s]  Passed  Argument is NULL\n",__func__,__FILE__);
    return ret;
  }

  *rsList = lwm2m_resource_inst_string_remove(*rsList, &resInst, resId);

  if (resInst==NULL)
  {
    return ret;
  }
  else
  {
    tx_byte_release(resInst);
    resInst = NULL;
    ret = true;
  }

  return ret;
}

uint8_t ext_generic_write_resources(uint16_t instanceId, qapi_Net_LWM2M_Flat_Data_t *data,
                                    qapi_Net_LWM2M_Data_t *object, uint16_t resInstId, uint8_t write_method)
{
  int i = 0;
  uint8_t result = 0;
  int64_t value = 0 ;
  gen_multi_res_t* mult_inst = NULL;
  if(data == NULL || object == NULL ) 
  {
    LOG_INFO("fun [%s] file [%s]  Passed  Argument is NULL\n",__func__,__FILE__);
    return QAPI_NET_LWM2M_500_INTERNAL_SERVER_E;
  }


  mult_inst = (gen_multi_res_t *) lwm2m_gen_inst_find(gen_multi_res, instanceId);
  if (NULL == mult_inst)
    return QAPI_NET_LWM2M_404_NOT_FOUND_E;

  switch (data[i].id)
  {
    case RES_O_SNSR_VALUES:
      {
        if(resInstId != INVALID_RES_INST_ID)
        {
          int64_t value = 0 ;
          if (1 != lwm2m_data_decode_int(&data[i], &value))
          {
            result = QAPI_NET_LWM2M_400_BAD_REQUEST_E;
          }
          else if (value < 0 || value > 0xFFFF )
          {
            result = QAPI_NET_LWM2M_400_BAD_REQUEST_E;
          }
          else
          {
            if (!ext_write_resource_inst_val((ext_resource_instance_int_list_t **)&(mult_inst->sensors_val),resInstId, value))
            {
              result = QAPI_NET_LWM2M_500_INTERNAL_SERVER_E;
            }
            else
            {
              result = QAPI_NET_LWM2M_204_CHANGED_E;
            }
          }
        }
        else
        {
          if (data[i].type!= QAPI_NET_LWM2M_TYPE_MULTIPLE_RESOURCE)
          {
            result = QAPI_NET_LWM2M_400_BAD_REQUEST_E;
          }
          else
          {
            int ri = 0;
            qapi_Net_LWM2M_Flat_Data_t* subTlvArray = NULL;

            ext_resource_instance_int_list_t* ValListSave = NULL;
            ext_resource_instance_int_list_t* sensor_values = NULL;
            for (sensor_values = mult_inst->sensors_val;
                 sensor_values!=NULL;
                 sensor_values = sensor_values->next)
            {
              if (!ext_write_resource_inst_val((ext_resource_instance_int_list_t **)&(ValListSave),
                                                sensor_values->resInstId,(uint16_t)sensor_values->InstValue))
              {return QAPI_NET_LWM2M_500_INTERNAL_SERVER_E;}
            }
            
            if(write_method == QAPI_COAP_PUT)
            {
              lwm2m_resource_inst_int_free(mult_inst->sensors_val);
              mult_inst->sensors_val = NULL;
            }

            subTlvArray = data[i].value.asChildren.array;

            if (data[i].value.asChildren.count == 0)
            {
              result = QAPI_NET_LWM2M_204_CHANGED_E;
            }
            else if (subTlvArray==NULL)
            {
              result = QAPI_NET_LWM2M_400_BAD_REQUEST_E;
            }
            else
            {
              for (ri=0; ri < data[i].value.asChildren.count; ri++)
                {
                  if (1 != lwm2m_data_decode_int(&subTlvArray[ri], &value))
                  {
                    result = QAPI_NET_LWM2M_400_BAD_REQUEST_E;
                    break;
                  }
                  else if (value < 0 || value > 0xFF)
                  {
                    result = QAPI_NET_LWM2M_400_BAD_REQUEST_E;
                    break;
                  }
                  else if (!ext_write_resource_inst_val((ext_resource_instance_int_list_t **)&(mult_inst->sensors_val),
                                                        subTlvArray[ri].id,(uint16_t)value))
                  {
                    result = QAPI_NET_LWM2M_500_INTERNAL_SERVER_E;
                    break;
                  }
                  else
                  {
                    result = QAPI_NET_LWM2M_204_CHANGED_E;
                  }
                }
            }

            if (result != QAPI_NET_LWM2M_204_CHANGED_E)
            {
              // free  partial created new ones
              lwm2m_resource_inst_int_free(mult_inst->sensors_val);
              // restore old values:
              mult_inst->sensors_val = ValListSave;
            }
            else
            {
                lwm2m_resource_inst_int_free(ValListSave);
            }
          }
        }
      }
    break;
    default:
      {
        return QAPI_NET_LWM2M_404_NOT_FOUND_E;
      }
  }

  return result;
}

uint8_t ext_presence_write_resources(uint16_t instanceId, qapi_Net_LWM2M_Flat_Data_t *data,
                                     qapi_Net_LWM2M_Data_t *object, uint16_t resInstId, uint8_t write_method)

{
  int i = 0;
  uint8_t result = 0;
  pre_multi_res_t* mult_inst = NULL;
  if( data == NULL || object == NULL ) 
  {
    LOG_INFO("fun [%s] file [%s]  Passed  Argument is NULL\n",__func__,__FILE__);
    return QAPI_NET_LWM2M_500_INTERNAL_SERVER_E;
  }

  i = 0;

  mult_inst = (pre_multi_res_t *) lwm2m_pre_inst_find(pre_multi_res, instanceId);
  if (NULL == mult_inst)
    return QAPI_NET_LWM2M_404_NOT_FOUND_E;

  switch (data[i].id)
  {
    case RES_O_SUP_SENSOR_TYPE:
      {
        int ri = 0;
        qapi_Net_LWM2M_Flat_Data_t* subTlvArray = NULL;
        char sensor_type[MAX_STRING_LEN];
        if(resInstId != INVALID_RES_INST_ID)
        {
          LOG_INFO("Write on  %d Resource Instance ID\n", resInstId);
          strlcpy(sensor_type, (char*)data[i].value.asBuffer.buffer, data[i].value.asBuffer.length + 1);
          if (!ext_write_string_resource_inst_val((ext_resource_instance_string_list_t **)&(mult_inst->sensor_types),resInstId, sensor_type))
          {
            result = QAPI_NET_LWM2M_500_INTERNAL_SERVER_E;
          }
          else
          {
            result = QAPI_NET_LWM2M_204_CHANGED_E;
          }
        }
        else
        {
          ext_resource_instance_string_list_t* sensorSave = NULL;
          ext_resource_instance_string_list_t* sensor_types = NULL;
          for (sensor_types = mult_inst->sensor_types;
               sensor_types!=NULL;
               sensor_types = sensor_types->next)
          {
            if (!ext_write_string_resource_inst_val((ext_resource_instance_string_list_t **)&(sensorSave),
                                              sensor_types->resInstId,sensor_types->InstValue))
            {return QAPI_NET_LWM2M_500_INTERNAL_SERVER_E;}
          }

          if(data[i].type != QAPI_NET_LWM2M_TYPE_MULTIPLE_RESOURCE)
            return QAPI_NET_LWM2M_400_BAD_REQUEST_E;

          if(write_method == QAPI_COAP_PUT)
          {
            lwm2m_resource_inst_string_free(mult_inst->sensor_types);
            mult_inst->sensor_types = NULL;
          }

          subTlvArray = data[i].value.asChildren.array;

          if (data[i].value.asChildren.count == 0)
          {
            result = QAPI_NET_LWM2M_400_BAD_REQUEST_E;
          }
          else if (subTlvArray==NULL)
          {
            result = QAPI_NET_LWM2M_400_BAD_REQUEST_E;
          }
          else
          {
            for (ri=0; ri < data[i].value.asChildren.count; ri++)
            {
              strlcpy(sensor_type, (char*)subTlvArray[ri].value.asBuffer.buffer, subTlvArray[ri].value.asBuffer.length + 1);
              if (!ext_write_string_resource_inst_val((ext_resource_instance_string_list_t **)&(mult_inst->sensor_types),subTlvArray[ri].id, sensor_type))
              {
                result = QAPI_NET_LWM2M_500_INTERNAL_SERVER_E;
              }
              else
              {
                result = QAPI_NET_LWM2M_204_CHANGED_E;
              }
            }
          }

          if (result != QAPI_NET_LWM2M_204_CHANGED_E)
          {
            // free pot. partial created new ones
              lwm2m_resource_inst_string_free(mult_inst->sensor_types);
            // restore old values:
            mult_inst->sensor_types = sensorSave;
          }
          else
          {
            // final free saved value list
              lwm2m_resource_inst_string_free(sensorSave);
          }
        }

      }
      break;

    default:
        result = QAPI_NET_LWM2M_404_NOT_FOUND_E;
  }

  return result;  
}

qapi_Net_LWM2M_Instance_Info_t* obj_create_generic_sensor(qapi_Net_LWM2M_Data_t *lwm2m_data)
{
  int i =0;
  qapi_Net_LWM2M_Data_t *object = NULL;
  qapi_Net_LWM2M_Instance_Info_t *instance = NULL;
  qapi_Net_LWM2M_Resource_Info_t *res1 = NULL;
  qapi_Net_LWM2M_Resource_Info_t *res2 = NULL;
  qapi_Net_LWM2M_Resource_Info_t *res3 = NULL;
  qapi_Net_LWM2M_Resource_Info_t *res4 = NULL;
  qapi_Net_LWM2M_Resource_Info_t *res5 = NULL;
  qapi_Net_LWM2M_Resource_Info_t *res6 = NULL;
  qapi_Net_LWM2M_Resource_Info_t *res7 = NULL;
  qapi_Net_LWM2M_Resource_Info_t *res8 = NULL;
  qapi_Net_LWM2M_Resource_Info_t *res9 = NULL;
  gen_multi_res_t *mult_inst = NULL;
  
  object = lwm2m_object_find(object_generic, LWM2M_GENERIC_SENSOR_OBJECT_ID);

  if(object == NULL)
  {
    LOG_INFO("Object not found.");
    return NULL;
  }

  for(i = 0;i < MAX_EXT_INSTANCE; i++)
  {
    if(obj_inst_created_generic[i] == 0)
    {
      obj_inst_index_generic = i;
      break;
    }
  }

  if(i == MAX_EXT_INSTANCE)
  {
    LOG_INFO("Maximum instances reached for object.");
    return NULL;
  }

  tx_byte_allocate(byte_pool_qcli, (VOID **)&instance, sizeof(qapi_Net_LWM2M_Instance_Info_t), TX_NO_WAIT);

  if(instance == NULL)
  {
    LOG_INFO("Memory allocation failure.\n");
    return NULL;
  }

  memset(instance, 0x00, sizeof(qapi_Net_LWM2M_Instance_Info_t));

  instance->instance_ID = obj_inst_index_generic;
  instance->next = NULL;

  instance->no_resources++;
  tx_byte_allocate(byte_pool_qcli, (VOID **)&instance->resource_info, sizeof(qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);

  if(instance->resource_info == NULL)
  {
    LOG_INFO("Memory allocation failure.\n");
    return NULL;
  }
  memset(instance->resource_info, 0x00, sizeof(qapi_Net_LWM2M_Resource_Info_t));

  instance->resource_info->resource_ID   = RES_M_SENSOR_VALUE;
  instance->resource_info->type          = QAPI_NET_LWM2M_TYPE_INTEGER_E;
  instance->resource_info->value.asInteger = 30;
  instance->resource_info->next          = NULL;

  instance->no_resources++;
  tx_byte_allocate(byte_pool_qcli, (VOID **)&res1, sizeof(qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);

  if(res1 == NULL)
  {
    LOG_INFO("Memory allocation failure.\n");
    return NULL;
  }
  memset(res1, 0x00, sizeof(qapi_Net_LWM2M_Resource_Info_t));

  res1->resource_ID             = RES_O_SENSOR_UNITS;
  res1->type                    = QAPI_NET_LWM2M_TYPE_STRING_E;
  res1->value.asBuffer.length   = strlen(SEN_UNIT)+1;
  tx_byte_allocate(byte_pool_qcli, (VOID **)&res1->value.asBuffer.buffer, res1->value.asBuffer.length, TX_NO_WAIT);
  if(res1->value.asBuffer.buffer == NULL)
  {
    LOG_INFO("Memory allocation failure.\n");
    return NULL;
  }
  memcpy(res1->value.asBuffer.buffer, SEN_UNIT, res1->value.asBuffer.length);
  res1->next                    = NULL;
  instance->resource_info->next = res1;

  instance->no_resources++;
  tx_byte_allocate(byte_pool_qcli, (VOID **)&res2, sizeof(qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);

  if(res2 == NULL)
  {
    LOG_INFO("Memory allocation failure.\n");
    return NULL;
  }
  memset(res2, 0x00, sizeof(qapi_Net_LWM2M_Resource_Info_t));

  res2->resource_ID             = RES_O_MIN_MEASURED_VAL;
  res2->type                    = QAPI_NET_LWM2M_TYPE_FLOAT_E;
  res2->value.asFloat           = 10.0;
  res2->next                    = NULL;
  res1->next                    = res2;

  instance->no_resources++;
  tx_byte_allocate(byte_pool_qcli, (VOID **)&res3, sizeof(qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);

  if(res3 == NULL)
  {
    LOG_INFO("Memory allocation failure.\n");
    return NULL;
  }
  memset(res3, 0x00, sizeof(qapi_Net_LWM2M_Resource_Info_t));

  res3->resource_ID             = RES_O_MAX_MEASURED_VAL;
  res3->type                    = QAPI_NET_LWM2M_TYPE_FLOAT_E;
  res3->value.asFloat           = 40.0;
  res3->next                    = NULL;
  res2->next                    = res3;

  instance->no_resources++;
  tx_byte_allocate(byte_pool_qcli, (VOID **)&res4, sizeof(qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);

  if(res4 == NULL)
  {
    LOG_INFO("Memory allocation failure.\n");
    return NULL;
  }
  memset(res4, 0x00, sizeof(qapi_Net_LWM2M_Resource_Info_t));

  res4->resource_ID             = RES_M_MIN_RANGE_VAL;
  res4->type                    = QAPI_NET_LWM2M_TYPE_FLOAT_E;
  res4->value.asFloat           = 0.0;
  res4->next                    = NULL;
  res3->next                    = res4;

  instance->no_resources++;
  tx_byte_allocate(byte_pool_qcli, (VOID **)&res5, sizeof(qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);

  if(res5 == NULL)
  {
    LOG_INFO("Memory allocation failure.\n");
    return NULL;
  }
  memset(res5, 0x00, sizeof(qapi_Net_LWM2M_Resource_Info_t));

  res5->resource_ID             = RES_M_MAX_RANGE_VAL;
  res5->type                    = QAPI_NET_LWM2M_TYPE_FLOAT_E;
  res5->value.asFloat           = 50.0;
  res5->next                    = NULL;
  res4->next                    = res5;

  instance->no_resources++;
  tx_byte_allocate(byte_pool_qcli, (VOID **)&res6, sizeof(qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);

  if(res6 == NULL)
  {
    LOG_INFO("Memory allocation failure.\n");
    return NULL;
  }
  memset(res6, 0x00, sizeof(qapi_Net_LWM2M_Resource_Info_t));

  res6->resource_ID             = RES_O_RESET_MEASURED_VAL;
  res6->type                    = QAPI_NET_LWM2M_TYPE_OPAQUE_E;
  res6->next                    = NULL;
  res5->next                    = res6;

  instance->no_resources++;
  tx_byte_allocate(byte_pool_qcli, (VOID **)&res7, sizeof(qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);

  if(res7 == NULL)
  {
    LOG_INFO("Memory allocation failure.\n");
    return NULL;
  }
  memset(res7, 0x00, sizeof(qapi_Net_LWM2M_Resource_Info_t));

  res7->resource_ID             = RES_M_SENSOR_STATE;
  res7->type                    = QAPI_NET_LWM2M_TYPE_BOOLEAN_E;
  res7->value.asBoolean         = true;
  res7->next                    = NULL;
  res6->next                    = res7;

  instance->no_resources++;
  tx_byte_allocate(byte_pool_qcli, (VOID **)&res8, sizeof(qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);

  if(res8 == NULL)
  {
    LOG_INFO("Memory allocation failure.\n");
    return NULL;
  }
  memset(res8, 0x00, sizeof(qapi_Net_LWM2M_Resource_Info_t));

  res8->resource_ID             = RES_O_MEAN_VAL;
  res8->type                    = QAPI_NET_LWM2M_TYPE_INTEGER_E;
  res8->value.asInteger         = 25;
  res8->next                    = NULL;
  res7->next                    = res8;

  instance->no_resources++;
  tx_byte_allocate(byte_pool_qcli, (VOID **)&res9, 
                   sizeof(qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);

  if(res9 == NULL)
  {
    LOG_INFO("Memory allocation failure.\n");
    return NULL;
  }
  memset(res9, 0x00, sizeof(qapi_Net_LWM2M_Resource_Info_t));

  res9->resource_ID             = RES_O_SNSR_VALUES;
  res9->type                    = QAPI_NET_LWM2M_TYPE_MULTIPLE_RESOURCE;
  res9->value.asChildren.count  = 0;
  res9->value.asChildren.array  = NULL;
  res9->next                    = NULL;
  res8->next                    = res9;

  tx_byte_allocate(byte_pool_qcli, (VOID **)&mult_inst, 
                   sizeof(gen_multi_res_t), TX_NO_WAIT);
  if(mult_inst == NULL)
  {
    LOG_INFO("Memory allocation failure.\n");
    return NULL;
  }

  memset(mult_inst, 0x00, sizeof(gen_multi_res_t));
  mult_inst->instanceId = instance->instance_ID;
  mult_inst->sensors_val = NULL;

  gen_multi_res = lwm2m_gen_inst_add(gen_multi_res, mult_inst);

  ext_write_resource_inst_val((ext_resource_instance_int_list_t **)&(mult_inst->sensors_val), 0x01, 5);

  object->no_instances++;

  lwm2m_data->object_ID = LWM2M_GENERIC_SENSOR_OBJECT_ID;
  lwm2m_data->no_instances = 1;

  return instance;
}

qapi_Net_LWM2M_Instance_Info_t* obj_create_presence_sensor(qapi_Net_LWM2M_Data_t *lwm2m_data)
{
  int i =0;
  qapi_Net_LWM2M_Data_t *object = NULL;
  qapi_Net_LWM2M_Instance_Info_t *instance = NULL;
  qapi_Net_LWM2M_Resource_Info_t *res1 = NULL;
  qapi_Net_LWM2M_Resource_Info_t *res2 = NULL;
  qapi_Net_LWM2M_Resource_Info_t *res3 = NULL;
  qapi_Net_LWM2M_Resource_Info_t *res4 = NULL;
  qapi_Net_LWM2M_Resource_Info_t *res5 = NULL;
  qapi_Net_LWM2M_Resource_Info_t *res6 = NULL;
  pre_multi_res_t *mult_inst = NULL;

  object = lwm2m_object_find(object_presence, LWM2M_PRESENCE_SENSOR_OBJECT_ID);

  if(object == NULL)
  {
    LOG_INFO("Object not found.");
    return NULL;
  }

  for(i = 0;i < MAX_EXT_INSTANCE; i++)
  {
    if(obj_inst_created_presence[i] == 0)
    {
      obj_inst_index_presence= i;
      break;
    }
  }

  if(i == MAX_EXT_INSTANCE)
  {
    LOG_INFO("Maximum instances reached for object.");
    return NULL;
  }

  tx_byte_allocate(byte_pool_qcli, (VOID **)&instance, sizeof(qapi_Net_LWM2M_Instance_Info_t), TX_NO_WAIT);

  if(instance == NULL)
  {
    LOG_INFO("Memory allocation failure.\n");
    return NULL;
  }

  memset(instance, 0x00, sizeof(qapi_Net_LWM2M_Instance_Info_t));

  instance->instance_ID = obj_inst_index_presence;
  instance->next = NULL;

  instance->no_resources++;
  tx_byte_allocate(byte_pool_qcli, (VOID **)&instance->resource_info, sizeof(qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);

  if(instance->resource_info == NULL)
  {
    LOG_INFO("Memory allocation failure.\n");
    return NULL;
  }
  memset(instance->resource_info, 0x00, sizeof(qapi_Net_LWM2M_Resource_Info_t));

  instance->resource_info->resource_ID     = RES_M_DIG_INPUT_STATE;
  instance->resource_info->type            = QAPI_NET_LWM2M_TYPE_BOOLEAN_E;
  instance->resource_info->value.asBoolean = true;
  instance->resource_info->next            = NULL;

  instance->no_resources++;
  tx_byte_allocate(byte_pool_qcli, (VOID **)&res1, sizeof(qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);

  if(res1 == NULL)
  {
    LOG_INFO("Memory allocation failure.\n");
    return NULL;
  }
  memset(res1, 0x00, sizeof(qapi_Net_LWM2M_Resource_Info_t));

  res1->resource_ID             = RES_O_DIG_INPUT_COUNTER;
  res1->type                    = QAPI_NET_LWM2M_TYPE_INTEGER_E;
  res1->value.asInteger         = 5;
  res1->next                    = NULL;
  instance->resource_info->next = res1;

  instance->no_resources++;
  tx_byte_allocate(byte_pool_qcli, (VOID **)&res2, sizeof(qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);

  if(res2 == NULL)
  {
    LOG_INFO("Memory allocation failure.\n");
    return NULL;
  }
  memset(res2, 0x00, sizeof(qapi_Net_LWM2M_Resource_Info_t));

  res2->resource_ID             = RES_O_RESET_DIG_INPUT_COUNTER;
  res2->type                    = QAPI_NET_LWM2M_TYPE_OPAQUE_E;
  res2->next                    = NULL;
  res1->next                    = res2;

  instance->no_resources++;
  tx_byte_allocate(byte_pool_qcli, (VOID **)&res3, sizeof(qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);

  if(res3 == NULL)
  {
    LOG_INFO("Memory allocation failure.\n");
    return NULL;
  }
  memset(res3, 0x00, sizeof(qapi_Net_LWM2M_Resource_Info_t));

  res3->resource_ID             = RES_O_SENSOR_TYPE;
  res3->type                    = QAPI_NET_LWM2M_TYPE_STRING_E;
  res3->value.asBuffer.length   = strlen(SEN_TYPE)+1;
  tx_byte_allocate(byte_pool_qcli, (VOID **)&res3->value.asBuffer.buffer, res3->value.asBuffer.length, TX_NO_WAIT);
  if(res3->value.asBuffer.buffer == NULL)
  {
    LOG_INFO("Memory allocation failure.\n");
    return NULL;
  }
  memcpy(res3->value.asBuffer.buffer, SEN_TYPE, res3->value.asBuffer.length);
  res3->next                    = NULL;
  res2->next                    = res3;

  instance->no_resources++;
  tx_byte_allocate(byte_pool_qcli, (VOID **)&res4, sizeof(qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);

  if(res4 == NULL)
  {
    LOG_INFO("Memory allocation failure.\n");
    return NULL;
  }
  memset(res4, 0x00, sizeof(qapi_Net_LWM2M_Resource_Info_t));

  res4->resource_ID               = RES_O_BUSY_TO_CLEAR_DELAY;
  res4->type                      = QAPI_NET_LWM2M_TYPE_INTEGER_E;
  res4->value.asInteger           = 10;
  res4->next                      = NULL;
  res3->next                      = res4;

  instance->no_resources++;
  tx_byte_allocate(byte_pool_qcli, (VOID **)&res5, sizeof(qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);

  if(res5 == NULL)
  {
    LOG_INFO("Memory allocation failure.\n");
    return NULL;
  }
  memset(res5, 0x00, sizeof(qapi_Net_LWM2M_Resource_Info_t));

  res5->resource_ID               = RES_O_CLEAR_TO_BUSY_DELAY;
  res5->type                      = QAPI_NET_LWM2M_TYPE_INTEGER_E;
  res5->value.asInteger           = 15;
  res5->next                      = NULL;
  res4->next                      = res5;

  instance->no_resources++;
  tx_byte_allocate(byte_pool_qcli, (VOID **)&res6, 
                   sizeof(qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);
  if(res6 == NULL)
  {
    LOG_INFO("Memory allocation failure.\n");
    return NULL;
  }
  memset(res6, 0x00, sizeof(qapi_Net_LWM2M_Resource_Info_t));

  res6->resource_ID             = RES_O_SUP_SENSOR_TYPE;
  res6->type                    = QAPI_NET_LWM2M_TYPE_MULTIPLE_RESOURCE;
  res6->value.asChildren.count  = 0;
  res6->value.asChildren.array  = NULL;
  res6->next                    = NULL;
  res5->next                    = res6;

  tx_byte_allocate(byte_pool_qcli, (VOID **)&mult_inst, 
                   sizeof(pre_multi_res_t), TX_NO_WAIT);
  if(mult_inst == NULL)
  {
    LOG_INFO("Memory allocation failure.\n");
    return NULL;
  }

  memset(mult_inst, 0x00, sizeof(pre_multi_res_t));
  mult_inst->instanceId = instance->instance_ID;
  mult_inst->sensor_types = NULL;

  pre_multi_res = lwm2m_pre_inst_add(pre_multi_res, mult_inst);

  ext_write_string_resource_inst_val((ext_resource_instance_string_list_t **)&(mult_inst->sensor_types), 0x01, "PIR");

  object->no_instances++;

  lwm2m_data->object_ID = LWM2M_PRESENCE_SENSOR_OBJECT_ID;
  lwm2m_data->no_instances = 1;

  return instance;
}

qapi_Status_t create_object_instance_app(qapi_Net_LWM2M_Ext_t *obj_buffer)
{
  qapi_Status_t result = QAPI_ERROR;
  qapi_Net_LWM2M_Data_t *object = NULL;
  qapi_Net_LWM2M_Instance_Info_t *instance = NULL;
  qapi_Net_LWM2M_Data_t lwm2m_data;
  qapi_Net_LWM2M_Instance_Info_t lwm2m_inst;
  qapi_Net_LWM2M_Resource_Info_t lwm2m_res;
  uint32_t obj_id;

  memset(&lwm2m_inst, 0x00, sizeof(qapi_Net_LWM2M_Instance_Info_t));
  memset(&lwm2m_res, 0x00, sizeof(qapi_Net_LWM2M_Resource_Info_t));

  if(obj_buffer->app_data.ul_op.id == 0)
  {
    if(ext_object_handler_app1 == NULL)
    {
      LOG_INFO("Application 1 not registerd Please register application ");
      QCLI_Printf(lwm2m_ext_handle, "Application 1 not registerd Please register application \n");
      return QAPI_ERROR;
    }
    if(object_generic == NULL)
    {
      LOG_INFO("Object is not valid.");
      QCLI_Printf(lwm2m_ext_handle, "Object is not valid. \n");
      return QAPI_ERROR;
    }

    if(obj_buffer->app_data.ul_op.obj_info.obj_mask & QAPI_NET_LWM2M_OBJECT_ID_E)
    {
      obj_id = obj_buffer->app_data.ul_op.obj_info.obj_id;
    }
    else
    {
      LOG_INFO("Object id is not valid.");
      QCLI_Printf(lwm2m_ext_handle, "Object id is not valid. \n");
      return QAPI_ERR_INVALID_PARAM;
    }

    object = lwm2m_object_find(object_generic, obj_id);

    if(object == NULL)
    {
      LOG_INFO("Object not found.");
      QCLI_Printf(lwm2m_ext_handle, "Object not found. \n");
      return QAPI_ERR_NO_ENTRY;
    }
  
    memset(&lwm2m_data, 0x00, sizeof(qapi_Net_LWM2M_Data_t));
  
    if(object->object_ID == LWM2M_GENERIC_SENSOR_OBJECT_ID)
    {
      instance = obj_create_generic_sensor(&lwm2m_data);
    }
  
    if(lwm2m_data.object_ID == LWM2M_GENERIC_SENSOR_OBJECT_ID && instance != NULL)
    {
      lwm2m_data.instance_info = &lwm2m_inst;
      lwm2m_data.instance_info->instance_ID = obj_inst_index_generic;
      lwm2m_data.instance_info->no_resources = instance->no_resources;
      lwm2m_data.instance_info->resource_info = &lwm2m_res;
    }

    if((instance != NULL) && ext_object_handler_app1 != NULL)
    {
      result = qapi_Net_LWM2M_Create_Object_Instance(ext_object_handler_app1, &lwm2m_data);
    }
  
    else
    {
      LOG_INFO("Cannot create object instance");
      QCLI_Printf(lwm2m_ext_handle, "Cannot create object instance error is %d\n", result);
      return QAPI_ERR_NO_MEMORY;
    }
  
    if(result == QAPI_OK)
    {
      LOG_INFO("Object instance created successfully.\n");
      QCLI_Printf(lwm2m_ext_handle, "Object instance %d created successfully for object 1001\n", obj_inst_index_generic);
      object->instance_info = lwm2m_instance_add(object->instance_info, instance);
      if(object->instance_info && object->object_ID == LWM2M_GENERIC_SENSOR_OBJECT_ID)
      {
        obj_inst_created_generic[obj_inst_index_generic] = 1;
      }
    }
  
    else
    {
      qapi_Net_LWM2M_Free_Single_Instance_Info(instance);
      QCLI_Printf(lwm2m_ext_handle, "Object instance not created.Error is %d\n", result);
      LOG_INFO("Object instance not created.Error is %d\n", result);
    }
  
    return result;
  }

  else if(obj_buffer->app_data.ul_op.id == 1)
  {
    if(ext_object_handler_app2 == NULL)
    {
      LOG_INFO("Application 2 not registerd Please register application ");
      QCLI_Printf(lwm2m_ext_handle, "Application 2 not registerd Please register application \n");
      return QAPI_ERROR;
    }

    if(object_presence == NULL)
    {
      LOG_INFO("Object is not valid.");
      QCLI_Printf(lwm2m_ext_handle, "Object is not valid. \n");
      return QAPI_ERROR;
    }

    if(obj_buffer->app_data.ul_op.obj_info.obj_mask & QAPI_NET_LWM2M_OBJECT_ID_E)
    {
      obj_id = obj_buffer->app_data.ul_op.obj_info.obj_id;
    }
    else
    {
      LOG_INFO("Object id is not valid.");
      QCLI_Printf(lwm2m_ext_handle, "Object id is not valid. \n");
      return QAPI_ERR_INVALID_PARAM;
    }

    object = lwm2m_object_find(object_presence, obj_id);

    if(object == NULL)
    {
      LOG_INFO("Object not found.");
      QCLI_Printf(lwm2m_ext_handle, "Object not found. \n");
      return QAPI_ERR_NO_ENTRY;
    }
  
    memset(&lwm2m_data, 0x00, sizeof(qapi_Net_LWM2M_Data_t));
  
    if(object->object_ID == LWM2M_PRESENCE_SENSOR_OBJECT_ID)
    {
      instance = obj_create_presence_sensor(&lwm2m_data);
    }
  
    if(lwm2m_data.object_ID == LWM2M_PRESENCE_SENSOR_OBJECT_ID && instance != NULL)
    {
      lwm2m_data.instance_info = &lwm2m_inst;
      lwm2m_data.instance_info->instance_ID = obj_inst_index_presence;
      lwm2m_data.instance_info->no_resources = instance->no_resources;
      lwm2m_data.instance_info->resource_info = &lwm2m_res;
    }

    if((instance != NULL) && ext_object_handler_app2 != NULL)
    {
      result = qapi_Net_LWM2M_Create_Object_Instance(ext_object_handler_app2, &lwm2m_data);
    }
  
    else
    {
      LOG_INFO("Cannot create object instance");
      QCLI_Printf(lwm2m_ext_handle, "Cannot create object instance error is %d\n", result);
      return QAPI_ERR_NO_MEMORY;
    }
  
    if(result == QAPI_OK)
    {
      LOG_INFO("Object instance created successfully.\n");
      QCLI_Printf(lwm2m_ext_handle, "Object instance %d created successfully for object 1002\n", obj_inst_index_presence);
      object->instance_info = lwm2m_instance_add(object->instance_info, instance);
      if(object->instance_info && object->object_ID == LWM2M_PRESENCE_SENSOR_OBJECT_ID)
      {
        obj_inst_created_presence[obj_inst_index_presence] = 1;
      }
    }
  
    else
    {
      qapi_Net_LWM2M_Free_Single_Instance_Info(instance);
      QCLI_Printf(lwm2m_ext_handle, "Object instance not created.Error is %d\n", result);
      LOG_INFO("Object instance not created.Error is %d\n", result);
    }
  
    return result;
  }

  return result;
}

qapi_Status_t create_object_instance(qapi_Net_LWM2M_App_Handler_t handler, 
                                     qapi_Net_LWM2M_Server_Data_t op_data)
{
  qapi_Status_t result = QAPI_ERROR;
  qapi_Net_LWM2M_Data_t *object = NULL;
  qapi_Net_LWM2M_Instance_Info_t *instance = NULL;
  qapi_Net_LWM2M_Data_t lwm2m_data;
  qapi_Net_LWM2M_Instance_Info_t lwm2m_inst;
  qapi_Net_LWM2M_Resource_Info_t lwm2m_res;
  uint32_t obj_id;
  qapi_Net_LWM2M_App_Ex_Obj_Data_t app_data;

  obj_id = op_data.obj_info.obj_id;

  memset(&lwm2m_inst, 0x00, sizeof(qapi_Net_LWM2M_Instance_Info_t));
  memset(&lwm2m_res, 0x00, sizeof(qapi_Net_LWM2M_Resource_Info_t));
  memset(&lwm2m_data, 0x00, sizeof(qapi_Net_LWM2M_Data_t));

  if(handler == NULL)
  {
    LOG_INFO("Application handle is not valid for current application");
    return QAPI_ERR_INVALID_PARAM;
  }

  if(handler == ext_object_handler_app1)
  {
    object = lwm2m_object_find(object_generic, obj_id);
    if(object == NULL)
    {
      LOG_INFO("Object not found.");
      return QAPI_ERR_NO_ENTRY;
    }

    if(object->object_ID == LWM2M_GENERIC_SENSOR_OBJECT_ID)
    {
      instance = obj_create_generic_sensor(&lwm2m_data);
    }

    if(lwm2m_data.object_ID == LWM2M_GENERIC_SENSOR_OBJECT_ID && instance != NULL)
    {
      lwm2m_data.instance_info = &lwm2m_inst;
      lwm2m_data.instance_info->instance_ID = obj_inst_index_generic;
      lwm2m_data.instance_info->no_resources = instance->no_resources;
      lwm2m_data.instance_info->resource_info = &lwm2m_res;
    }
  }

  else if(handler == ext_object_handler_app2)
  {
    object = lwm2m_object_find(object_presence, obj_id);
    if(object == NULL)
    {
      LOG_INFO("Object not found.");
      return QAPI_ERR_NO_ENTRY;
    }

    if(object->object_ID == LWM2M_PRESENCE_SENSOR_OBJECT_ID)
    {
      instance = obj_create_presence_sensor(&lwm2m_data);
    }

    if(lwm2m_data.object_ID == LWM2M_PRESENCE_SENSOR_OBJECT_ID && instance != NULL)
    {
      lwm2m_data.instance_info = &lwm2m_inst;
      lwm2m_data.instance_info->instance_ID = obj_inst_index_presence;
      lwm2m_data.instance_info->no_resources = instance->no_resources;
      lwm2m_data.instance_info->resource_info = &lwm2m_res;
    }
  }
  else
  {
    LOG_INFO("Object not found.");
    return QAPI_ERR_NO_ENTRY;
  }

  if(instance != NULL)
  {
    LOG_INFO("Object instance created successfully.\n");
  }
  else
  {
    LOG_INFO("Cannot create object instance");
    return QAPI_ERR_NO_MEMORY;
  }
  object->instance_info = lwm2m_instance_add(object->instance_info, instance);
  if(object->instance_info)
  {

    memset(&app_data, 0x00, sizeof(qapi_Net_LWM2M_App_Ex_Obj_Data_t));
    app_data.obj_info.obj_mask |= QAPI_NET_LWM2M_INSTANCE_ID_E;

    if(object->object_ID == LWM2M_GENERIC_SENSOR_OBJECT_ID)
    {
      obj_inst_created_generic[obj_inst_index_generic] = 1;
      app_data.obj_info.obj_inst_id = obj_inst_index_generic;
    }
    else if(object->object_ID == LWM2M_PRESENCE_SENSOR_OBJECT_ID)
    {
      obj_inst_created_presence[obj_inst_index_presence] = 1;
      app_data.obj_info.obj_inst_id = obj_inst_index_presence;
    }

    app_data.msg_type = QAPI_NET_LWM2M_CREATE_RESPONSE_MSG_E;
    app_data.obj_info.obj_id = obj_id;
    app_data.obj_info.obj_mask |= QAPI_NET_LWM2M_OBJECT_ID_E;
    app_data.status_code = QAPI_NET_LWM2M_201_CREATED_E;
    app_data.conf_msg = 0;
    app_data.msg_id_len = op_data.msg_id_len;
    memcpy(app_data.msg_id, op_data.msg_id, op_data.msg_id_len);

    qapi_Net_LWM2M_Send_Message(handler, &app_data);

  }

  if((instance != NULL))
  {
    result = qapi_Net_LWM2M_Create_Object_Instance(handler, &lwm2m_data);
  }
  else
  {
    LOG_INFO("Cannot create object instance");
    return QAPI_ERR_NO_MEMORY;
  }


  return result;
}

qapi_Status_t delete_object_instance_app(qapi_Net_LWM2M_Ext_t *obj_buffer)
{
  qapi_Status_t result = QAPI_ERROR;
  qapi_Net_LWM2M_Obj_Info_t  obj_info;
  qapi_Net_LWM2M_Data_t         *object = NULL;
  qapi_Net_LWM2M_Instance_Info_t *instance = NULL;
  qapi_Net_LWM2M_Object_Info_t instance_info;
  qapi_Net_LWM2M_Uri_t uri;
  gen_multi_res_t *mult_inst1 = NULL;
  pre_multi_res_t *mult_inst2 = NULL;

  memset(&uri, 0x00, sizeof(qapi_Net_LWM2M_Uri_t));

  if(obj_buffer->app_data.ul_op.id == 0)
  {
    if(ext_object_handler_app1 == NULL)
    {
      LOG_INFO("Application 1 not registerd Please register application ");
      QCLI_Printf(lwm2m_ext_handle, "Application 1 not registerd Please register application \n");
      return QAPI_ERROR;
    }

    if(object_generic == NULL)
    {
      LOG_INFO("Object is not valid.");
      QCLI_Printf(lwm2m_ext_handle, "Object is not valid \n");
      return QAPI_ERROR;
    }

    obj_info = obj_buffer->app_data.ul_op.obj_info;
    memset(&instance_info, 0x00, sizeof(qapi_Net_LWM2M_Object_Info_t));

    if((obj_info.obj_mask & QAPI_NET_LWM2M_OBJECT_ID_E))
    {
      if((obj_info.obj_mask & QAPI_NET_LWM2M_INSTANCE_ID_E))
      {
        if((obj_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_ID_E))
        {
          LOG_INFO("Deletion on resource level not allowed \n ");
          QCLI_Printf(lwm2m_ext_handle, "Deletion on resource level not allowed \n");
          return QAPI_ERROR;
        }
        else
        {
          object = lwm2m_object_find(object_generic, obj_info.obj_id);
          if(object != NULL)
          {
            instance = lwm2m_instance_find(object->instance_info, obj_info.obj_inst_id);
            if(instance != NULL)
            {
              instance_info.no_object_info = 1;
              tx_byte_allocate(byte_pool_qcli, (VOID **)&instance_info.id_info, sizeof(qapi_Net_LWM2M_Id_Info_t), TX_NO_WAIT);
              if(instance_info.id_info == NULL)
              {
                return QAPI_ERR_NO_MEMORY;
              }
              memset(instance_info.id_info, 0x00, sizeof(qapi_Net_LWM2M_Id_Info_t));
              instance_info.id_info->id_set |= QAPI_NET_LWM2M_OBJECT_ID_E;
              instance_info.id_info->id_set |= QAPI_NET_LWM2M_INSTANCE_ID_E;
              instance_info.id_info->object_ID = obj_info.obj_id;
              instance_info.id_info->instance_ID = obj_info.obj_inst_id;
    
              result = qapi_Net_LWM2M_Delete_Object_Instance(ext_object_handler_app1, &instance_info);
    
              if(instance_info.id_info)
              {
                tx_byte_release(instance_info.id_info);
                instance_info.id_info = NULL;
              }
              if(result == QAPI_OK)
              {
                uri.flag =  obj_info.obj_mask;
                uri.objectId = obj_info.obj_id;
                uri.instanceId = obj_info.obj_inst_id;
                uri.resourceId = obj_info.res_id;
                uri.resourceInstId = obj_info.res_inst_id;

                observe_delete_ext_handle_req(ext_object_handler_app1, &uri);

                gen_multi_res = lwm2m_gen_inst_remove(gen_multi_res, &mult_inst1, obj_info.obj_inst_id);
                if(mult_inst1 != NULL)
                {
                  lwm2m_resource_inst_int_free(mult_inst1->sensors_val);
                  mult_inst1->sensors_val= NULL;
                  tx_byte_release(mult_inst1);
                  mult_inst1 = NULL;
                }

                object->instance_info = lwm2m_instance_remove(object->instance_info, &instance, obj_info.obj_inst_id);
                if(!qapi_Net_LWM2M_Free_Single_Instance_Info(instance))
                {
                  LOG_INFO("Instance id %d cleared ", obj_info.obj_inst_id);
                  object->no_instances --;
                  if(object->object_ID == LWM2M_GENERIC_SENSOR_OBJECT_ID)
                  {
                    obj_inst_created_generic[obj_info.obj_inst_id] = 0;
                  }
                  QCLI_Printf(lwm2m_ext_handle, "Instance %d deleted for object 1001 \n", obj_info.obj_inst_id);
                  return QAPI_OK;
                }
              }
            }
            else
            {
              QCLI_Printf(lwm2m_ext_handle, "Instance id %d not valid for object 1001 \n", obj_info.obj_inst_id);
              LOG_INFO("Instance id %d not valid", obj_info.obj_inst_id);
            }
          }
          else
          {
            QCLI_Printf(lwm2m_ext_handle, "Object id %d not found \n", obj_info.obj_id);
            LOG_INFO("Object id %d not found", obj_info.obj_id);
            return QAPI_ERR_NO_ENTRY;
          }
        }
      }
      else
      {
        QCLI_Printf(lwm2m_ext_handle, "Instance id is required \n");
        LOG_INFO("Instance id is required.\n ");
        return QAPI_ERROR;
      }
    }
    LOG_INFO("Instance id not deleted");
    return result;
  }

  else if(obj_buffer->app_data.ul_op.id == 1)
  {
    if(ext_object_handler_app2 == NULL)
    {
      LOG_INFO("Application 2 not registerd Please register application ");
      QCLI_Printf(lwm2m_ext_handle, "Application 2 not registerd Please register application \n");
      return QAPI_ERROR;
    }

    if(object_presence == NULL)
    {
      LOG_INFO("Object is not valid.");
      QCLI_Printf(lwm2m_ext_handle, "Object is not valid \n");
      return QAPI_ERROR;
    }

    obj_info = obj_buffer->app_data.ul_op.obj_info;
    memset(&instance_info, 0x00, sizeof(qapi_Net_LWM2M_Object_Info_t));

    if((obj_info.obj_mask & QAPI_NET_LWM2M_OBJECT_ID_E))
    {
      if((obj_info.obj_mask & QAPI_NET_LWM2M_INSTANCE_ID_E))
      {
        if((obj_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_ID_E))
        {
          LOG_INFO("Deletion on resource level not allowed \n ");
          QCLI_Printf(lwm2m_ext_handle, "Deletion on resource level not allowed \n");
          return QAPI_ERROR;
        }
        else
        {
          object = lwm2m_object_find(object_presence, obj_info.obj_id);
          if(object != NULL)
          {
            instance = lwm2m_instance_find(object->instance_info, obj_info.obj_inst_id);
            if(instance != NULL)
            {
              instance_info.no_object_info = 1;
              tx_byte_allocate(byte_pool_qcli, (VOID **)&instance_info.id_info, sizeof(qapi_Net_LWM2M_Id_Info_t), TX_NO_WAIT);
              if(instance_info.id_info == NULL)
              {
                return QAPI_ERR_NO_MEMORY;
              }
              memset(instance_info.id_info, 0x00, sizeof(qapi_Net_LWM2M_Id_Info_t));
              instance_info.id_info->id_set |= QAPI_NET_LWM2M_OBJECT_ID_E;
              instance_info.id_info->id_set |= QAPI_NET_LWM2M_INSTANCE_ID_E;
              instance_info.id_info->object_ID = obj_info.obj_id;
              instance_info.id_info->instance_ID = obj_info.obj_inst_id;
    
              result = qapi_Net_LWM2M_Delete_Object_Instance(ext_object_handler_app2, &instance_info);
    
              if(instance_info.id_info)
              {
                tx_byte_release(instance_info.id_info);
                instance_info.id_info = NULL;
              }
              if(result == QAPI_OK)
              {
                uri.flag =  obj_info.obj_mask;
                uri.objectId = obj_info.obj_id;
                uri.instanceId = obj_info.obj_inst_id;
                uri.resourceId = obj_info.res_id;
                uri.resourceInstId = obj_info.res_inst_id;

                observe_delete_ext_handle_req(ext_object_handler_app2, &uri);

                pre_multi_res = lwm2m_pre_inst_remove(pre_multi_res, &mult_inst2, obj_info.obj_inst_id);
                if(mult_inst2 != NULL)
                {
                  lwm2m_resource_inst_string_free(mult_inst2->sensor_types);
                  mult_inst2->sensor_types = NULL;
                  tx_byte_release(mult_inst2);
                  mult_inst2 = NULL;
                }

                object->instance_info = lwm2m_instance_remove(object->instance_info, &instance, obj_info.obj_inst_id);
                if(!qapi_Net_LWM2M_Free_Single_Instance_Info(instance))
                {
                  LOG_INFO("Instance id %d cleared ", obj_info.obj_inst_id);
                  object->no_instances --;
                  if(object->object_ID == LWM2M_PRESENCE_SENSOR_OBJECT_ID)
                  {
                    obj_inst_created_presence[obj_info.obj_inst_id] = 0;
                  }
                  QCLI_Printf(lwm2m_ext_handle, "Instance %d deleted for object 1002 \n", obj_info.obj_inst_id);
                  return QAPI_OK;
                }
              }
            }
            else
            {
              QCLI_Printf(lwm2m_ext_handle, "Instance id %d not valid for object 1002 \n", obj_info.obj_inst_id);
              LOG_INFO("Instance id %d not valid", obj_info.obj_inst_id);
            }
          }
          else
          {
            QCLI_Printf(lwm2m_ext_handle, "Object id %d not found \n", obj_info.obj_id);
            LOG_INFO("Object id %d not found", obj_info.obj_id);
            return QAPI_ERR_NO_ENTRY;
          }
        }
      }
      else
      {
        QCLI_Printf(lwm2m_ext_handle, "Instance id is required \n");
        LOG_INFO("Instance id is required.\n ");
        return QAPI_ERROR;
      }
    }
    LOG_INFO("Instance id not deleted");
    return result;
  }

  return result;
}

qapi_Status_t delete_object_instance(qapi_Net_LWM2M_App_Handler_t handler, 
                                     qapi_Net_LWM2M_Server_Data_t op_data)
{
  qapi_Status_t result = QAPI_ERROR;
  qapi_Net_LWM2M_Obj_Info_t  obj_info;
  qapi_Net_LWM2M_Data_t         *object = NULL;
  qapi_Net_LWM2M_Instance_Info_t *instance = NULL;
  qapi_Net_LWM2M_Object_Info_t instance_info;
  qapi_Net_LWM2M_Uri_t uri;
  gen_multi_res_t *mult_inst1 = NULL;
  pre_multi_res_t *mult_inst2 = NULL;
  
  memset(&uri, 0x00, sizeof(qapi_Net_LWM2M_Uri_t));

  if(handler == NULL)
  {
    LOG_INFO("Application handle is not valid for current application");
    return QAPI_ERR_INVALID_PARAM;
  }

  obj_info = op_data.obj_info;
  memset(&instance_info, 0x00, sizeof(qapi_Net_LWM2M_Object_Info_t));

  if((obj_info.obj_mask & QAPI_NET_LWM2M_OBJECT_ID_E))
  {
    if((obj_info.obj_mask & QAPI_NET_LWM2M_INSTANCE_ID_E))
    {
      if((obj_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_ID_E))
      {
        LOG_INFO("Deletion on resource level not allowed \n ");
        send_response_message(handler, op_data, QAPI_NET_LWM2M_405_METHOD_NOT_ALLOWED_E);
        return QAPI_ERROR;
      }
      else
      {
        if(handler == ext_object_handler_app1)
        {
          object = lwm2m_object_find(object_generic, obj_info.obj_id);
        }
        else if(handler == ext_object_handler_app2)
        {
          object = lwm2m_object_find(object_presence, obj_info.obj_id);
        }

        if(object != NULL)
        {
          instance = lwm2m_instance_find(object->instance_info, obj_info.obj_inst_id);
          if(instance != NULL)
          {
            instance_info.no_object_info = 1;
            tx_byte_allocate(byte_pool_qcli, (VOID **)&instance_info.id_info, sizeof(qapi_Net_LWM2M_Id_Info_t), TX_NO_WAIT);
            if(instance_info.id_info == NULL)
            {
              send_response_message(handler, op_data, QAPI_NET_LWM2M_500_INTERNAL_SERVER_E);
              return QAPI_ERR_NO_MEMORY;
            }
            memset(instance_info.id_info, 0x00, sizeof(qapi_Net_LWM2M_Id_Info_t));
            instance_info.id_info->id_set |= QAPI_NET_LWM2M_OBJECT_ID_E;
            instance_info.id_info->id_set |= QAPI_NET_LWM2M_INSTANCE_ID_E;
            instance_info.id_info->object_ID = obj_info.obj_id;
            instance_info.id_info->instance_ID = obj_info.obj_inst_id;

            uri.flag =  obj_info.obj_mask;
            uri.objectId = obj_info.obj_id;
            uri.instanceId = obj_info.obj_inst_id;
            uri.resourceId = obj_info.res_id;
            uri.resourceInstId = obj_info.res_inst_id;

            observe_delete_ext_handle_req(handler, &uri);

            if(object->object_ID == LWM2M_GENERIC_SENSOR_OBJECT_ID)
            {
              gen_multi_res = lwm2m_gen_inst_remove(gen_multi_res, &mult_inst1, obj_info.obj_inst_id);
              if(mult_inst1 != NULL)
              {
                lwm2m_resource_inst_int_free(mult_inst1->sensors_val);
                mult_inst1->sensors_val= NULL;
                tx_byte_release(mult_inst1);
                mult_inst1 = NULL;
              }
            }
            else if(object->object_ID == LWM2M_PRESENCE_SENSOR_OBJECT_ID)
            {
              pre_multi_res = lwm2m_pre_inst_remove(pre_multi_res, &mult_inst2, obj_info.obj_inst_id);
              if(mult_inst2 != NULL)
              {
                lwm2m_resource_inst_string_free(mult_inst2->sensor_types);
                mult_inst2->sensor_types = NULL;
                tx_byte_release(mult_inst2);
                mult_inst2 = NULL;
              }
            }

            object->instance_info = lwm2m_instance_remove(object->instance_info, &instance, obj_info.obj_inst_id);
            if(!qapi_Net_LWM2M_Free_Single_Instance_Info(instance))
            {
              LOG_INFO("Instance id %d cleared ", obj_info.obj_inst_id);
              object->no_instances --;
              if(object->object_ID == LWM2M_GENERIC_SENSOR_OBJECT_ID)
              {
                obj_inst_created_generic[obj_info.obj_inst_id] = 0;
              }
              else if(object->object_ID == LWM2M_PRESENCE_SENSOR_OBJECT_ID)
              {
                obj_inst_created_presence[obj_info.obj_inst_id] = 0;
              }
              send_response_message(handler, op_data, QAPI_NET_LWM2M_202_DELETED_E);
              result = QAPI_OK;
            }
            else
            {
              result = QAPI_ERROR;
            }

            if(result == QAPI_OK)
            {
              result = qapi_Net_LWM2M_Delete_Object_Instance(handler, &instance_info);
            }

            if(instance_info.id_info)
            {
              tx_byte_release(instance_info.id_info);
              instance_info.id_info = NULL;
            }
            return result;
          }
          else
          {
            LOG_INFO("Instance id %d not valid", obj_info.obj_inst_id);
          }
        }
        else
        {
          LOG_INFO("Object id %d not found", obj_info.obj_id);
          send_response_message(handler, op_data, QAPI_NET_LWM2M_404_NOT_FOUND_E);
          return QAPI_ERR_NO_ENTRY;
        }
      }
    }
    else
    {
     if(ext_obj_delete_all(handler,op_data) == QAPI_OK)
      {
        send_response_message(handler, op_data, QAPI_NET_LWM2M_202_DELETED_E);
        return QAPI_OK;
      }
    }
  }
  LOG_INFO("Instance id not deleted");
  send_response_message(handler, op_data, QAPI_NET_LWM2M_400_BAD_REQUEST_E);
  return result;
}


qapi_Status_t set_obj_link_app(qapi_Net_LWM2M_Ext_t *obj_buffer)
{
  uint32_t status = 0; 
  qapi_Status_t result = QAPI_ERROR;
  qapi_Net_LWM2M_Data_t objlink_set;
  uint16_t obj_id = obj_buffer->app_data.ul_op.obj_info.obj_id;
  uint16_t inst_id = obj_buffer->app_data.ul_op.obj_info.obj_inst_id;

  memset(&objlink_set, 0x00, sizeof(qapi_Net_LWM2M_Data_t));

  /* Object information */
  objlink_set.object_ID = 0x03; 
  objlink_set.no_instances = 0x01;

  /* Instance information */ 
  status = tx_byte_allocate(byte_pool_qcli, (void **)&objlink_set.instance_info, sizeof(qapi_Net_LWM2M_Instance_Info_t), TX_NO_WAIT);
  if (status != TX_SUCCESS)
  {
    LOG_INFO("Failed to allocate memory for object instance information\n");
    QCLI_Printf(lwm2m_ext_handle, "Failed to allocate memory for object instance information\n"); 

    return QAPI_ERR_NO_MEMORY;
  }

  if (!objlink_set.instance_info)
    return QAPI_ERR_NO_MEMORY;
  
  memset(objlink_set.instance_info, 0x00, sizeof(qapi_Net_LWM2M_Instance_Info_t));
  objlink_set.instance_info->instance_ID = 0x00;
  objlink_set.instance_info->no_resources = 0x01; 
  objlink_set.instance_info->next = NULL; 

  /* Resource information */
  status = tx_byte_allocate(byte_pool_qcli, (void **)&objlink_set.instance_info->resource_info, sizeof(qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);
  if (status != TX_SUCCESS)
  {
    tx_byte_release(objlink_set.instance_info);   

    LOG_INFO("Failed to allocate memory for resource information\n");
    QCLI_Printf(lwm2m_ext_handle, "Failed to allocate memory for resource information\n"); 

    return QAPI_ERR_NO_MEMORY;
  }

  if (!objlink_set.instance_info->resource_info)
    return QAPI_ERR_NO_MEMORY;

  objlink_set.instance_info->resource_info->type = QAPI_NET_LWM2M_TYPE_OBJECT_LINK;
  objlink_set.instance_info->resource_info->resource_ID = 22;
  objlink_set.instance_info->resource_info->value.asObjLink = (obj_id << 16 | inst_id);
  objlink_set.instance_info->resource_info->next = NULL;

  if (obj_buffer->app_data.ul_op.id == 0)
  {
    if (ext_object_handler_app1 == NULL)
    {
      LOG_INFO("Application 1 not registered please register application\n");
      QCLI_Printf(lwm2m_ext_handle, "Application 1 not registerd please register application\n");

      return QAPI_ERROR;
    }

    /* Set object link */ 
    result = qapi_Net_LWM2M_Set(ext_object_handler_app1, &objlink_set);
    if (result)
    {
      LOG_INFO("Application 1 failed to set the object link[%d->%d] with result[0x%x]\n", obj_id, inst_id, result);
      QCLI_Printf(lwm2m_ext_handle, "Application 1 failed to set the object link[%d->%d] with result[0x%x]\n", obj_id, inst_id, result);
    }
    else 
    {
      LOG_INFO("Application 1 successfully set the object link[%d->%d] with result[0x%x]\n", obj_id, inst_id, result);
      QCLI_Printf(lwm2m_ext_handle, "Application 1 successfully set the object link[%d->%d] with result[0x%x]\n", obj_id, inst_id, result);
    }
  }
  else if (obj_buffer->app_data.ul_op.id == 1)
  {
    if (ext_object_handler_app2 == NULL)
    {
      LOG_INFO("Application 2 not registered please register application\n");
      QCLI_Printf(lwm2m_ext_handle, "Application 2 not registered please register application\n");

      return QAPI_ERROR;
    }

    /* Set object link */ 
    result = qapi_Net_LWM2M_Set(ext_object_handler_app2, &objlink_set);
    if (result)
    {
      LOG_INFO("Application 2 failed to set the object link[%d->%d] with result[0x%x]\n", obj_id, inst_id, result);
      QCLI_Printf(lwm2m_ext_handle, "Application 2 failed to set the object link[%d->%d] with result[0x%x]\n", obj_id, inst_id, result);
    }
    else 
    {
      LOG_INFO("Application 2 successfully set the object link[%d->%d] with result[0x%x]\n", obj_id, inst_id, result);
      QCLI_Printf(lwm2m_ext_handle, "Application 2 successfully set the object link[%d->%d] with result[0x%x]\n", obj_id, inst_id, result);
    }
  }

  if (objlink_set.instance_info)
  {
    if (objlink_set.instance_info->resource_info)
      tx_byte_release(objlink_set.instance_info->resource_info); 

    tx_byte_release(objlink_set.instance_info); 
  }
  
  return result;
}


qapi_Status_t get_obj_link_app(qapi_Net_LWM2M_Ext_t *obj_buffer)
{
  uint32_t status = 0;
  qapi_Status_t result = QAPI_ERROR;
  qapi_Net_LWM2M_Data_t *objlink_get = NULL;
  qapi_Net_LWM2M_Object_Info_t lwm2m_info_req;
  
  /* Object link information */
  memset(&lwm2m_info_req, 0x00, sizeof(qapi_Net_LWM2M_Object_Info_t));
  lwm2m_info_req.no_object_info = 0x01;

  status = tx_byte_allocate(byte_pool_qcli, (void **)&lwm2m_info_req.id_info, sizeof(qapi_Net_LWM2M_Id_Info_t), TX_NO_WAIT);
  if (status != TX_SUCCESS)
  {
    LOG_INFO("Failed to allocate memory for request ID information\n");
    QCLI_Printf(lwm2m_ext_handle, "Failed to allocate memory for request ID information\n"); 

    return QAPI_ERR_NO_MEMORY;
  }

  if (!lwm2m_info_req.id_info)
    return QAPI_ERR_NO_MEMORY;
  
  lwm2m_info_req.id_info->id_set = QAPI_NET_LWM2M_OBJECT_ID_E | QAPI_NET_LWM2M_INSTANCE_ID_E | QAPI_NET_LWM2M_RESOURCE_ID_E;
  lwm2m_info_req.id_info->object_ID   = 03;
  lwm2m_info_req.id_info->instance_ID = 00;
  lwm2m_info_req.id_info->resource_ID = 22;
  

  if (obj_buffer->app_data.ul_op.id == 0)
  {
    if (ext_object_handler_app1 == NULL)
    {
      LOG_INFO("Application 1 not registered please register application\n");
      QCLI_Printf(lwm2m_ext_handle, "Application 1 not registerd Please register application\n");

      return QAPI_ERROR;
    }

    /* Get object link */ 
    result = qapi_Net_LWM2M_Get(ext_object_handler_app1, &lwm2m_info_req, &objlink_get);
    if (result)
    {
      LOG_INFO("Application 1 failed to get the object link information with result[0x%x]\n", result);
      QCLI_Printf(lwm2m_ext_handle, "Application 1 failed to get the object link information with result[0x%x]\n", result);
    }
    else 
    {
      if ((objlink_get) && (objlink_get->instance_info))
      {
        /* Display the object links */
        qapi_Net_LWM2M_Resource_Info_t *res_info = objlink_get->instance_info->resource_info;
  
        LOG_INFO("Application 1 successfully retrieved the object link information with result[0x%x]\n", result);
        QCLI_Printf(lwm2m_ext_handle, "Application 1 successfully retrieved the object link information with result[0x%x]\n", result);
  
        if (res_info)
        {
          uint32_t cnt = 0; 
  
          for (cnt = 0; cnt < res_info->value.asChildren.count; cnt++)
          {
            uint16_t obj_id = res_info->value.asChildren.array[cnt].value.asObjLink >> 16; 
            uint16_t inst_id = res_info->value.asChildren.array[cnt].value.asObjLink & 0x0000FFFF; 
  
            LOG_INFO("Application 1 : Object link[%d->%d]\n", obj_id, inst_id);
            QCLI_Printf(lwm2m_ext_handle, "Application 1 : Object link[%d->%d]\n", obj_id, inst_id);
          }
        }
      }
    }   
  }
  else if (obj_buffer->app_data.ul_op.id == 1)
  {
    if (ext_object_handler_app2 == NULL)
    {
      LOG_INFO("Application 2 not registered please register application\n");
      QCLI_Printf(lwm2m_ext_handle, "Application 2 not registered please register application\n");

      return QAPI_ERROR;
    }

    /* Get object link */ 
    result = qapi_Net_LWM2M_Get(ext_object_handler_app2, &lwm2m_info_req, &objlink_get);
    if (result)
    {
      LOG_INFO("Application 2 failed to get the object link information with result[0x%x]\n", result);
      QCLI_Printf(lwm2m_ext_handle, "Application 2 failed to get the object link information with result[0x%x]\n", result);
    }
    else 
    {
      if ((objlink_get) && (objlink_get->instance_info))
      {
        /* Display the object links */
        qapi_Net_LWM2M_Resource_Info_t *res_info = objlink_get->instance_info->resource_info;
  
        LOG_INFO("Application 2 successfully retrieved the object link information with result[0x%x]\n", result);
        QCLI_Printf(lwm2m_ext_handle, "Application 2 successfully retrieved the object link information with result[0x%x]\n", result);
  
        if (res_info)
        {
          uint32_t cnt = 0; 
  
          for (cnt = 0; cnt < res_info->value.asChildren.count; cnt++)
          {
            uint16_t obj_id = res_info->value.asChildren.array[cnt].value.asObjLink >> 16; 
            uint16_t inst_id = res_info->value.asChildren.array[cnt].value.asObjLink & 0x0000FFFF; 
  
            LOG_INFO("Application 2 : Object link[%d->%d]\n", obj_id, inst_id);
            QCLI_Printf(lwm2m_ext_handle, "Application 2 : Object link[%d->%d]\n", obj_id, inst_id);
          }
        }
      }
    }
  }

  if (lwm2m_info_req.id_info)
    tx_byte_release(lwm2m_info_req.id_info); 

  return result;
}


qapi_Status_t set_server_lifetime_app(qapi_Net_LWM2M_Ext_t *obj_buffer)
{  
  qapi_Status_t result = QAPI_ERROR;
  uint32_t lifetime = obj_buffer->app_data.ul_op.lifetime; 
  uint8_t *url = obj_buffer->app_data.ul_op.url_info;

  if (obj_buffer->app_data.ul_op.id == 0)
  {
    if (ext_object_handler_app1 == NULL)
    {
      LOG_INFO("Application 1 not registerd Please register application\n");
      QCLI_Printf(lwm2m_ext_handle, "Application 1 not registerd please register application\n");

      return QAPI_ERROR;
    }

    /* Set the server lifetime */ 
    result = qapi_Net_LWM2M_Set_ServerLifeTime(ext_object_handler_app1, url, lifetime);
    if (result)
    {
      LOG_INFO("Application 1 failed to set the server lifetime[%d] for URL[%s] with result[0x%x]\n", lifetime, url, result);
      QCLI_Printf(lwm2m_ext_handle, "Application 1 failed to set the server lifetime[%d] for URL[%s] with result[0x%x]\n", lifetime, url, result);
    }
    else 
    {
      LOG_INFO("Application 1 successfully set the server lifetime[%d] for URL[%s] with result[0x%x]\n", lifetime, url, result);
      QCLI_Printf(lwm2m_ext_handle, "Application 1 successfully set the server lifetime[%d] for URL[%s] with result[0x%x]\n", lifetime, url, result);
    }
  }
  else if (obj_buffer->app_data.ul_op.id == 1)
  {
    if (ext_object_handler_app2 == NULL)
    {
      LOG_INFO("Application 2 not registerd Please register application\n");
      QCLI_Printf(lwm2m_ext_handle, "Application 2 not registered please register application\n");

      return QAPI_ERROR;
    }

    /* Set the server lifetime */  
    result = qapi_Net_LWM2M_Set_ServerLifeTime(ext_object_handler_app2, url, lifetime);
    if (result)
    {
      LOG_INFO("Application 2 failed to set the server lifetime[%d] for URL[%s] with result[0x%x]\n", lifetime, url, result);
      QCLI_Printf(lwm2m_ext_handle, "Application 2 failed to set the server lifetime[%d] for URL[%s] with result[0x%x]\n", lifetime, url, result);
    }
    else 
    {
      LOG_INFO("Application 2 successfully set the server lifetime[%d] for URL[%s] with result[0x%x]\n", lifetime, url, result);
      QCLI_Printf(lwm2m_ext_handle, "Application 2 successfully set the server lifetime[%d] for URL[%s] with result[0x%x]\n", lifetime, url, result);
    }
  }
 
  return result;
}


qapi_Status_t get_server_lifetime_app(qapi_Net_LWM2M_Ext_t *obj_buffer)
{  
  qapi_Status_t result = QAPI_ERROR;
  uint32_t lifetime = 0x00;  
  uint8_t *url = obj_buffer->app_data.ul_op.url_info;

  if (obj_buffer->app_data.ul_op.id == 0)
  {
    if (ext_object_handler_app1 == NULL)
    {
      LOG_INFO("Application 1 not registerd Please register application\n");
      QCLI_Printf(lwm2m_ext_handle, "Application 1 not registerd please register application\n");

      return QAPI_ERROR;
    }

    /* Get the server lifetime */ 
    result = qapi_Net_LWM2M_Get_ServerLifeTime(ext_object_handler_app1, url, &lifetime);
    if (result)
    {
      LOG_INFO("Application 1 failed to get the server lifetime for URL[%s] with result[0x%x]\n", url, result);
      QCLI_Printf(lwm2m_ext_handle, "Application 1 failed to get the server lifetime for URL[%s] with result[0x%x]\n", url, result);
    }
    else 
    {
      LOG_INFO("Application 1 successfully retrieved the server lifetime[%d] for URL[%s] with result[0x%x]\n", lifetime, url, result);
      QCLI_Printf(lwm2m_ext_handle, "Application 1 successfully retrieved the server lifetime[%d] for URL[%s] with result[0x%x]\n", lifetime, url, result);
    }
  }
  else if (obj_buffer->app_data.ul_op.id == 1)
  {
    if (ext_object_handler_app2 == NULL)
    {
      LOG_INFO("Application 2 not registerd Please register application\n");
      QCLI_Printf(lwm2m_ext_handle, "Application 2 not registered please register application\n");

      return QAPI_ERROR;
    }

    /* Get the server lifetime */  
    result = qapi_Net_LWM2M_Get_ServerLifeTime(ext_object_handler_app2, url, &lifetime);
    if (result)
    {
      LOG_INFO("Application 2 failed to get the server lifetime for URL[%s] with result[0x%x]\n", url, result);
      QCLI_Printf(lwm2m_ext_handle, "Application 2 failed to get the server lifetime for URL[%s] with result[0x%x]\n", url, result);
    }
    else 
    {
      LOG_INFO("Application 2 successfully retrieved  the server lifetime[%d] for URL[%s] with result[0x%x]\n", lifetime, url, result);
      QCLI_Printf(lwm2m_ext_handle, "Application 2 successfully retrieved the server lifetime[%d] for URL[%s] with result[0x%x]\n", lifetime, url, result);
    }
  }
 
  return result;
}


uint8_t payload_set_data_generic(qapi_Net_LWM2M_Flat_Data_t* data,
                                       qapi_Net_LWM2M_Instance_Info_t* instance,
                                       uint16_t res_inst_id)
{
  qapi_Net_LWM2M_Resource_Info_t *resource = NULL;

  if( data == NULL || instance == NULL) 
  {
    LOG_INFO("LWM2M_LOG: fun [%s] file [%s]  Passed  Argument is NULL\n",__func__,__FILE__);
    return QAPI_NET_LWM2M_500_INTERNAL_SERVER_E;
  } 

  switch (data->id)
  {
    case RES_M_SENSOR_VALUE:
      resource = lwm2m_resource_find(instance->resource_info, data->id);
      if(resource != NULL)
      lwm2m_data_encode_int(resource->value.asInteger, data);
      return QAPI_NET_LWM2M_205_CONTENT_E;

    case RES_O_SENSOR_UNITS:
      resource = lwm2m_resource_find(instance->resource_info, data->id);
      if(resource != NULL)
      lwm2m_data_encode_string((const char *)resource->value.asBuffer.buffer, data);
      return QAPI_NET_LWM2M_205_CONTENT_E;

    case RES_O_MIN_MEASURED_VAL:
      resource = lwm2m_resource_find(instance->resource_info, data->id);
      if(resource != NULL)
      lwm2m_data_encode_float(resource->value.asFloat, data);
      return QAPI_NET_LWM2M_205_CONTENT_E;

    case RES_O_MAX_MEASURED_VAL:
      resource = lwm2m_resource_find(instance->resource_info, data->id);
      if(resource != NULL)
      lwm2m_data_encode_float(resource->value.asFloat, data);
      return QAPI_NET_LWM2M_205_CONTENT_E;

    case RES_M_MIN_RANGE_VAL:
      resource = lwm2m_resource_find(instance->resource_info, data->id);
      if(resource != NULL)
      lwm2m_data_encode_float(resource->value.asFloat, data);
      return QAPI_NET_LWM2M_205_CONTENT_E;

    case RES_M_MAX_RANGE_VAL:
      resource = lwm2m_resource_find(instance->resource_info, data->id);
      if(resource != NULL)
      lwm2m_data_encode_float(resource->value.asFloat, data);
      return QAPI_NET_LWM2M_205_CONTENT_E;

    case RES_M_SENSOR_STATE:
      resource = lwm2m_resource_find(instance->resource_info, data->id);
      if(resource != NULL)
      lwm2m_data_encode_bool(resource->value.asBoolean, data);
      return QAPI_NET_LWM2M_205_CONTENT_E;

    case RES_O_MEAN_VAL:
      resource = lwm2m_resource_find(instance->resource_info, data->id);
      if(resource != NULL)
      lwm2m_data_encode_int(resource->value.asInteger, data);
      return QAPI_NET_LWM2M_205_CONTENT_E;

    case RES_O_SNSR_VALUES:
    {
      ext_resource_instance_int_list_t *sensor_values = NULL;
      gen_multi_res_t *mult_inst = NULL;

      mult_inst = lwm2m_gen_inst_find(gen_multi_res, instance->instance_ID);
      if(mult_inst == NULL)
      {
        return QAPI_NET_LWM2M_404_NOT_FOUND_E;
      }
      if(res_inst_id != INVALID_RES_INST_ID)
      {
        ext_resource_instance_int_list_t *resInst;
        resInst = lwm2m_find_resource_inst(mult_inst->sensors_val, res_inst_id);
        if(resInst != NULL)
        {
          lwm2m_data_encode_int(resInst->InstValue, data);
          return QAPI_NET_LWM2M_205_CONTENT_E;
        }
        else
        {
          return QAPI_NET_LWM2M_404_NOT_FOUND_E;
        }
      }
      else
      {
        uint8_t ri = 0;
        if(mult_inst->sensors_val)
        {
          ri++;
          for(sensor_values = mult_inst->sensors_val; sensor_values->next != NULL; sensor_values = sensor_values->next, ri++);
        }

        if(ri == 0)
        {
          return QAPI_NET_LWM2M_400_BAD_REQUEST_E;
        }
        else
        {
          qapi_Net_LWM2M_Flat_Data_t *subTlvP = lwm2m_data_new(ri);
          if(NULL == subTlvP)
            return QAPI_NET_LWM2M_500_INTERNAL_SERVER_E;

          for(sensor_values = mult_inst->sensors_val, ri = 0; sensor_values != NULL;
              sensor_values = sensor_values->next, ri++)
          {
            subTlvP[ri].id = (uint16_t)sensor_values->resInstId;
            lwm2m_data_encode_int(sensor_values->InstValue, &subTlvP[ri]);
          }
          lwm2m_data_encode_instances(subTlvP, ri, data);
          return QAPI_NET_LWM2M_205_CONTENT_E;
        }
      }
      
    }
    default:
       return QAPI_NET_LWM2M_404_NOT_FOUND_E;
  }
}

uint8_t payload_set_data_presence(qapi_Net_LWM2M_Flat_Data_t* data,
                               qapi_Net_LWM2M_Instance_Info_t* instance,
                               uint16_t res_inst_id)
{
  qapi_Net_LWM2M_Resource_Info_t *resource = NULL;
  if( data == NULL || instance == NULL) 
  {
    LOG_INFO("LWM2M_LOG: fun [%s] file [%s]  Passed  Argument is NULL\n",__func__,__FILE__);
    return QAPI_NET_LWM2M_500_INTERNAL_SERVER_E;
  } 

  switch (data->id)
  {
    case RES_M_DIG_INPUT_STATE:
      resource = lwm2m_resource_find(instance->resource_info, data->id);
      if(resource != NULL)
      lwm2m_data_encode_bool(resource->value.asBoolean, data);
      return QAPI_NET_LWM2M_205_CONTENT_E;

    case RES_O_DIG_INPUT_COUNTER:
      resource = lwm2m_resource_find(instance->resource_info, data->id);
      if(resource != NULL)
      lwm2m_data_encode_int(resource->value.asInteger, data);
      return QAPI_NET_LWM2M_205_CONTENT_E;

    case RES_O_SENSOR_TYPE:
      resource = lwm2m_resource_find(instance->resource_info, data->id);
      if(resource != NULL)
      lwm2m_data_encode_string((const char *)resource->value.asBuffer.buffer, data);
      return QAPI_NET_LWM2M_205_CONTENT_E;

    case RES_O_BUSY_TO_CLEAR_DELAY:
      resource = lwm2m_resource_find(instance->resource_info, data->id);
      if(resource != NULL)
      lwm2m_data_encode_int(resource->value.asInteger, data);
      return QAPI_NET_LWM2M_205_CONTENT_E;

    case RES_O_CLEAR_TO_BUSY_DELAY:
      resource = lwm2m_resource_find(instance->resource_info, data->id);
      if(resource != NULL)
      lwm2m_data_encode_int(resource->value.asInteger, data);
      return QAPI_NET_LWM2M_205_CONTENT_E;

    case RES_O_SUP_SENSOR_TYPE:
    {
      ext_resource_instance_string_list_t *sensor_types = NULL;
      pre_multi_res_t *mult_inst = NULL;

      mult_inst = lwm2m_pre_inst_find(pre_multi_res, instance->instance_ID);
      if(mult_inst == NULL)
      {
        return QAPI_NET_LWM2M_404_NOT_FOUND_E;
      }
      if(res_inst_id != INVALID_RES_INST_ID)
      {
        ext_resource_instance_string_list_t *resInst;
        resInst = lwm2m_find_string_resource_inst(mult_inst->sensor_types, res_inst_id);
        if(resInst != NULL)
        {
          lwm2m_data_encode_string(resInst->InstValue, data);
          return QAPI_NET_LWM2M_205_CONTENT_E;
        }
        else
        {
          return QAPI_NET_LWM2M_404_NOT_FOUND_E;
        }
      }
      else
      {
        uint8_t ri = 0;
        if(mult_inst->sensor_types)
        {
          ri++;
          for(sensor_types = mult_inst->sensor_types; sensor_types->next != NULL; sensor_types = sensor_types->next, ri++);
        }

        if(ri == 0)
        {
          return QAPI_NET_LWM2M_400_BAD_REQUEST_E;
        }
        else
        {
          qapi_Net_LWM2M_Flat_Data_t *subTlvP = lwm2m_data_new(ri);
          if(NULL == subTlvP)
            return QAPI_NET_LWM2M_500_INTERNAL_SERVER_E;

          for(sensor_types = mult_inst->sensor_types, ri = 0; sensor_types != NULL;
              sensor_types = sensor_types->next, ri++)
          {
            subTlvP[ri].id = (uint16_t)sensor_types->resInstId;
            lwm2m_data_encode_string(sensor_types->InstValue, &subTlvP[ri]);
          }
          lwm2m_data_encode_instances(subTlvP, ri, data);
          return QAPI_NET_LWM2M_205_CONTENT_E;
        }
      }
    }
    default:
      return QAPI_NET_LWM2M_404_NOT_FOUND_E;
  }

}

qapi_Status_t ext_object_read(qapi_Net_LWM2M_App_Handler_t handler,
                              qapi_Net_LWM2M_Uri_t uri, qapi_Net_LWM2M_Content_Type_t format, 
                              uint8_t **buffer, uint32_t *length)
{
  qapi_Status_t result = QAPI_ERROR;
  qapi_Net_LWM2M_Obj_Info_t  obj_info;
  qapi_Net_LWM2M_Data_t *object = NULL;
  qapi_Net_LWM2M_Instance_Info_t *instance = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource = NULL;
  qapi_Net_LWM2M_Flat_Data_t *data = NULL;
  int i = 0;
  int size = 0;
  //qapi_Net_LWM2M_Content_Type_t cont_type;

  if(handler == NULL || buffer == NULL || length == NULL)
  {
    LOG_INFO("Application handle is not valid for current application");
    //send_response_message(op_data, QAPI_NET_LWM2M_400_BAD_REQUEST_E);
    return QAPI_ERR_INVALID_PARAM;
  }

  obj_info.obj_mask = (qapi_Net_LWM2M_ID_t)uri.flag;
  obj_info.obj_id= uri.objectId;
  obj_info.obj_inst_id= uri.instanceId;
  obj_info.res_id= uri.resourceId;
  obj_info.res_inst_id= uri.resourceInstId;

  if(obj_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_ID_E)
  {
    if(handler == ext_object_handler_app1)
    {
      if(lwm2m_object_find(object_generic, obj_info.obj_id))
      {
        if(check_readable(obj_info.obj_id, obj_info.res_id) == false)
        {
          LOG_INFO("Read on resource %d not allowed.", obj_info.res_id);
          return QAPI_ERROR;
        }
      }
    }
    else if(handler == ext_object_handler_app2)
    {
      if(lwm2m_object_find(object_presence, obj_info.obj_id))
      {
        if(check_readable(obj_info.obj_id, obj_info.res_id) == false)
        {
          LOG_INFO("Read on resource %d not allowed.", obj_info.res_id);
          return QAPI_ERROR;
        }
      }
    }
  }

  if((obj_info.obj_mask & QAPI_NET_LWM2M_OBJECT_ID_E))
  {
    if(handler == ext_object_handler_app1)
    {
      object = lwm2m_object_find(object_generic, obj_info.obj_id);
    }
    else if(handler == ext_object_handler_app2)
    {
      object = lwm2m_object_find(object_presence, obj_info.obj_id);
    }

    if(object != NULL)
    {
      if((obj_info.obj_mask & QAPI_NET_LWM2M_INSTANCE_ID_E))
      {
        instance = lwm2m_instance_find(object->instance_info, obj_info.obj_inst_id);
        if(instance == NULL)
        {
          LOG_INFO("Instance id not found.");
          result = QAPI_ERR_NO_ENTRY;
          goto end;
        }
        else
        {
          if((obj_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_ID_E))
          {
            resource = lwm2m_resource_find(instance->resource_info, obj_info.res_id);
            if(resource == NULL)
            {
              LOG_INFO("Resource id not found.");
              result = QAPI_ERR_NO_ENTRY;
              goto end;
            }
            size = 1;
            data = lwm2m_data_new(size);
            if(data == NULL)
            {
              result = QAPI_ERR_NO_MEMORY;
              goto end;
            }
            (data)->id = resource->resource_ID;
          }
          if((obj_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_INSTANCE_ID_E))
          {
            result = read_data_ext(instance->instance_ID, &size, &data, object, obj_info.res_inst_id);
          }
          else
          {
            result = read_data_ext(instance->instance_ID, &size, &data, object, INVALID_RES_INST_ID);
          }
        }
      }
      else
      {
        for(instance = object->instance_info; instance != NULL; instance = instance->next)
        {
          size++;
        }
        data = lwm2m_data_new(size);
        if(data == NULL)
        {
          result = QAPI_ERR_NO_MEMORY;
          goto end;
        }
        instance = object->instance_info;
        i =0;
        result = QAPI_OK;
        while(instance != NULL && result == QAPI_OK)
        {
          result = read_data_ext(instance->instance_ID, (int *)&((data)[i].value.asChildren.count),
                                 &((data)[i].value.asChildren.array), object, INVALID_RES_INST_ID);
          (data)[i].id = instance->instance_ID;
          (data)[i].type = QAPI_NET_LWM2M_TYPE_OBJECT_INSTANCE;
          i++;
          instance = instance->next;
        }
      }
    }
    else
    {
      LOG_INFO("object id not found.");
      result = QAPI_ERR_NO_ENTRY;
      goto end;
    }
  }
  else
  {
    LOG_INFO("Not valid request.");
    result = QAPI_ERR_INVALID_PARAM;
    goto end;
  }

  /*if((obj_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_ID_E))
  {
    cont_type = QAPI_NET_LWM2M_TEXT_PLAIN;
  }
  else
  {
    cont_type = QAPI_NET_LWM2M_M2M_TLV;
  }*/

  if(result == QAPI_OK)
  {
    result = qapi_Net_LWM2M_Encode_Data(handler,
                                       (qapi_Net_LWM2M_Obj_Info_t *)&uri,
                                       data,
                                       (size_t)size,
                                       NULL,
                                       format,
                                       buffer,
                                       length);
     if (result != QAPI_OK || length == 0)
     {
       if(format == QAPI_NET_LWM2M_APPLICATION_OCTET_STREAM)
       {
         result = QAPI_ERR_BAD_PAYLOAD;
         goto end;
       }
       else if (format != QAPI_NET_LWM2M_TEXT_PLAIN || size != 1
           || (data)->type != QAPI_NET_LWM2M_TYPE_STRING_E || (data)->value.asBuffer.length != 0)
       {
         result = QAPI_ERR_BAD_PAYLOAD;
         goto end;
       }
     }
  }

  if((format == QAPI_NET_LWM2M_TEXT_PLAIN || format == QAPI_NET_LWM2M_APPLICATION_OCTET_STREAM))
  {
    if((size != 1
       || !(uri.flag & QAPI_NET_LWM2M_RESOURCE_ID_E)
       || data->type == QAPI_NET_LWM2M_TYPE_OBJECT
       || data->type == QAPI_NET_LWM2M_TYPE_OBJECT_INSTANCE
       || data->type == QAPI_NET_LWM2M_TYPE_MULTIPLE_RESOURCE))
    {
      result = QAPI_ERR_BAD_PAYLOAD;
    }
  }
end:
  if(data)
  {
    lwm2m_data_free(size, data);
  }
  return result;

}

qapi_Status_t read_data_ext(uint16_t instanceid, int *size, qapi_Net_LWM2M_Flat_Data_t **data,
                            qapi_Net_LWM2M_Data_t *object, uint16_t res_inst_id)
{
  qapi_Net_LWM2M_Instance_Info_t *target = NULL;
  int i = 0, ri = 0;
  uint8_t result = 0;
  uint16_t reslist_ob1[] = {
                             RES_M_SENSOR_VALUE,
                             RES_O_SENSOR_UNITS,
                             RES_O_MIN_MEASURED_VAL,
                             RES_O_MAX_MEASURED_VAL,
                             RES_M_MIN_RANGE_VAL,
                             RES_M_MAX_RANGE_VAL,
                             RES_M_SENSOR_STATE,
                             RES_O_MEAN_VAL
                           };

  uint16_t reslist_ob2[] = {
                             RES_M_DIG_INPUT_STATE,
                             RES_O_DIG_INPUT_COUNTER,
                             RES_O_SENSOR_TYPE,
                             RES_O_BUSY_TO_CLEAR_DELAY,
                             RES_O_CLEAR_TO_BUSY_DELAY
                           };

  int nbres;
  int temp;

  target = lwm2m_instance_find(object->instance_info, instanceid);
  if(target == NULL)
  {
    return QAPI_ERR_NO_ENTRY;
  }

  if(*size == 0)
  {
    if(object->object_ID == LWM2M_GENERIC_SENSOR_OBJECT_ID)
    {
      nbres = sizeof(reslist_ob1)/sizeof(uint16_t);
    }
    else
    {
      nbres = sizeof(reslist_ob2)/sizeof(uint16_t);
    }
    temp = nbres;
     
    if(object->object_ID == LWM2M_GENERIC_SENSOR_OBJECT_ID)
    {
      ext_resource_instance_int_list_t *sensor_values = NULL;
      gen_multi_res_t *mult_inst = NULL;

      mult_inst = lwm2m_gen_inst_find(gen_multi_res, instanceid);
      if(mult_inst == NULL)
      {
        return QAPI_ERR_NO_ENTRY;
	  }
      for (sensor_values = mult_inst->sensors_val, ri=0;
          sensor_values!=NULL;
          sensor_values = sensor_values->next, ri++);
      if(ri!=0)
        nbres++;
    }
    else if(object->object_ID == LWM2M_PRESENCE_SENSOR_OBJECT_ID)
    {
      ext_resource_instance_string_list_t *sensor_types = NULL;
      pre_multi_res_t *mult_inst = NULL;

      mult_inst = lwm2m_pre_inst_find(pre_multi_res, instanceid);
      if(mult_inst == NULL)
      {
        return QAPI_ERR_NO_ENTRY;
	  }
      for (sensor_types = mult_inst->sensor_types, ri=0;
          sensor_types!=NULL;
          sensor_types = sensor_types->next, ri++);
      if(ri!=0)
        nbres++;
    }
    *data = lwm2m_data_new(nbres);
    if(*data == NULL)
    {
      return QAPI_ERR_NO_MEMORY;
    }
    *size = nbres;

    if(object->object_ID == LWM2M_GENERIC_SENSOR_OBJECT_ID)
    {
      for(i = 0; i<temp; i++)
      {
        (*data)[i].id = reslist_ob1[i];
      }
      if(ri != 0)
      {
        (*data)[temp].id = RES_O_SNSR_VALUES;
      }
    }
    else
    {
      for(i = 0; i<temp; i++)
      {
        (*data)[i].id = reslist_ob2[i];
      }
      if(ri != 0)
      {
        (*data)[temp].id = RES_O_SUP_SENSOR_TYPE;
      }
    }
  }

  i = 0;
  do
  {
    if(object->object_ID == LWM2M_GENERIC_SENSOR_OBJECT_ID)
    result = payload_set_data_generic((*data)+i, target, res_inst_id);
    else
    result = payload_set_data_presence((*data)+i, target, res_inst_id);
    i++;
  }while(i< *size && result == QAPI_NET_LWM2M_205_CONTENT_E);

  if(result == QAPI_NET_LWM2M_205_CONTENT_E)
  {
  return QAPI_OK;
  }
  else if(result == QAPI_NET_LWM2M_404_NOT_FOUND_E)
  {
    return QAPI_ERR_NO_ENTRY;
  }
  else
  {
    return QAPI_ERROR;
  }
}

qapi_Status_t read_ext_obj(qapi_Net_LWM2M_App_Handler_t handler, 
                           qapi_Net_LWM2M_Server_Data_t op_data)
{
  qapi_Status_t result = QAPI_ERROR;
  qapi_Net_LWM2M_Obj_Info_t  obj_info;
  qapi_Net_LWM2M_Data_t *object = NULL;
  qapi_Net_LWM2M_Instance_Info_t *instance = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource = NULL;
  qapi_Net_LWM2M_App_Ex_Obj_Data_t *read_data = NULL;
  qapi_Net_LWM2M_Flat_Data_t *data = NULL;
  uint8_t *payload = NULL;
  uint32_t payload_length = 0;
  int i = 0;
  int size = 0;
  qapi_Net_LWM2M_Content_Type_t cont_type;

  /*if(object_generic == NULL)
  {
    LOG_INFO("Object is not valid.");
    send_response_message(op_data, QAPI_NET_LWM2M_500_INTERNAL_SERVER_E);
    return QAPI_ERROR;
  }*/

  if(handler == NULL)
  {
    LOG_INFO("Application handle is not valid for current application");
    //send_response_message(op_data, QAPI_NET_LWM2M_400_BAD_REQUEST_E);
    return QAPI_ERR_INVALID_PARAM;
  }

  obj_info = op_data.obj_info;

  if(obj_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_ID_E)
  {
    if(handler == ext_object_handler_app1)
    {
      if(lwm2m_object_find(object_generic, obj_info.obj_id))
      {
        if(check_readable(obj_info.obj_id, obj_info.res_id) == false)
        {
          LOG_INFO("Read on resource %d not allowed.", obj_info.res_id);
          send_response_message(handler, op_data, QAPI_NET_LWM2M_405_METHOD_NOT_ALLOWED_E);
          return QAPI_ERROR;
        }
      }
    }
    else if(handler == ext_object_handler_app2)
    {
      if(lwm2m_object_find(object_presence, obj_info.obj_id))
      {
        if(check_readable(obj_info.obj_id, obj_info.res_id) == false)
        {
          LOG_INFO("Read on resource %d not allowed.", obj_info.res_id);
          send_response_message(handler, op_data, QAPI_NET_LWM2M_405_METHOD_NOT_ALLOWED_E);
          return QAPI_ERROR;
        }
      }
    }
  }

  tx_byte_allocate(byte_pool_qcli, (VOID **)&read_data, sizeof(qapi_Net_LWM2M_App_Ex_Obj_Data_t), TX_NO_WAIT);
  if(read_data == NULL)
  {
    LOG_INFO("Memory allocation failure.\n");
    send_response_message(handler, op_data, QAPI_NET_LWM2M_500_INTERNAL_SERVER_E);
    return QAPI_ERR_NO_MEMORY;
  }
  memset(read_data, 0x00, sizeof(qapi_Net_LWM2M_App_Ex_Obj_Data_t));

  if((obj_info.obj_mask & QAPI_NET_LWM2M_OBJECT_ID_E))
  {
    if(handler == ext_object_handler_app1)
    {
      object = lwm2m_object_find(object_generic, obj_info.obj_id);
    }
    else if(handler == ext_object_handler_app2)
    {
      object = lwm2m_object_find(object_presence, obj_info.obj_id);
    }

    if(object != NULL)
    {
      if((obj_info.obj_mask & QAPI_NET_LWM2M_INSTANCE_ID_E))
      {
        instance = lwm2m_instance_find(object->instance_info, obj_info.obj_inst_id);
        if(instance == NULL)
        {
          LOG_INFO("Instance id not found.");
          result = QAPI_ERR_NO_ENTRY;
          send_response_message(handler, op_data, QAPI_NET_LWM2M_404_NOT_FOUND_E);
          goto end;
        }
        else
        {
          if((obj_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_ID_E))
          {
            resource = lwm2m_resource_find(instance->resource_info, obj_info.res_id);
            if(resource == NULL)
            {
              LOG_INFO("Resource id not found.");
              result = QAPI_ERR_NO_ENTRY;
              send_response_message(handler, op_data, QAPI_NET_LWM2M_404_NOT_FOUND_E);
              goto end;
            }
            size = 1;
            data = lwm2m_data_new(size);
            if(data == NULL)
            {
              result = QAPI_ERR_NO_MEMORY;
              send_response_message(handler, op_data, QAPI_NET_LWM2M_500_INTERNAL_SERVER_E);
              goto end;
            }
            (data)->id = resource->resource_ID;
          }
          if((obj_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_INSTANCE_ID_E))
          {
            result = read_data_ext(instance->instance_ID, &size, &data, object, obj_info.res_inst_id);
          }
          else
          {
            result = read_data_ext(instance->instance_ID, &size, &data, object, INVALID_RES_INST_ID);
          }
        }
      }
      else
      {
        for(instance = object->instance_info; instance != NULL; instance = instance->next)
        {
          size++;
        }
        data = lwm2m_data_new(size);
        if(data == NULL)
        {
          result = QAPI_ERR_NO_MEMORY;
          send_response_message(handler, op_data, QAPI_NET_LWM2M_500_INTERNAL_SERVER_E);
          goto end;
        }
        instance = object->instance_info;
        i =0;
        result = QAPI_OK;
        while(instance != NULL && result == QAPI_OK)
        {
          result = read_data_ext(instance->instance_ID, (int *)&((data)[i].value.asChildren.count),
                                 &((data)[i].value.asChildren.array), object, INVALID_RES_INST_ID);
          (data)[i].id = instance->instance_ID;
          (data)[i].type = QAPI_NET_LWM2M_TYPE_OBJECT_INSTANCE;
          i++;
          instance = instance->next;
        }
      }
    }
    else
    {
      LOG_INFO("object id not found.");
      result = QAPI_ERR_NO_ENTRY;
      send_response_message(handler, op_data, QAPI_NET_LWM2M_404_NOT_FOUND_E);
      goto end;
    }
  }
  else
  {
    LOG_INFO("Not valid request.");
    result = QAPI_ERR_INVALID_PARAM;
    send_response_message(handler, op_data, QAPI_NET_LWM2M_400_BAD_REQUEST_E);
    goto end;
  }

/*  if((obj_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_ID_E) && 
     !(obj_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_INSTANCE_ID_E))
  {
    cont_type = QAPI_NET_LWM2M_TEXT_PLAIN;
  }
  else
  {
    cont_type = QAPI_NET_LWM2M_M2M_TLV;
  }
*/
  if(op_data.accept_is_valid)
  {
    cont_type = op_data.accept;
  }
  else
  {
    cont_type = op_data.content_type;
  }

  if(result == QAPI_OK)
  {
    result = qapi_Net_LWM2M_Encode_Data(handler,
                                        &op_data.obj_info,
                                               data,
                                        (size_t)size,
                                        NULL,
                                        cont_type,
                                        &payload,
                                        &payload_length);
     if (result != QAPI_OK || payload_length == 0)
     {
       if(cont_type == QAPI_NET_LWM2M_APPLICATION_OCTET_STREAM)
       {
         result = QAPI_ERR_BAD_PAYLOAD;
         send_response_message(handler, op_data, QAPI_NET_LWM2M_406_NOT_ACCEPTABLE_E);
         goto end;
       }
       else if (op_data.content_type != QAPI_NET_LWM2M_TEXT_PLAIN || size != 1
           || (data)->type != QAPI_NET_LWM2M_TYPE_STRING_E || (data)->value.asBuffer.length != 0)
       {
         result = QAPI_ERR_BAD_PAYLOAD;
         send_response_message(handler, op_data, QAPI_NET_LWM2M_400_BAD_REQUEST_E);
         goto end;
       }
     }
  }
  else if(result == QAPI_ERR_NO_ENTRY)
  {
    send_response_message(handler, op_data, QAPI_NET_LWM2M_404_NOT_FOUND_E);
    goto end;
  }
  else 
  {
    send_response_message(handler, op_data, QAPI_NET_LWM2M_400_BAD_REQUEST_E);
    goto end;
  }

  if(op_data.accept_is_valid && (op_data.accept == QAPI_NET_LWM2M_TEXT_PLAIN || op_data.accept == QAPI_NET_LWM2M_APPLICATION_OCTET_STREAM))
  {
    if((size != 1
       || !(op_data.obj_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_ID_E)
       || data->type == QAPI_NET_LWM2M_TYPE_OBJECT
       || data->type == QAPI_NET_LWM2M_TYPE_OBJECT_INSTANCE
       || data->type == QAPI_NET_LWM2M_TYPE_MULTIPLE_RESOURCE))
  {
      result = QAPI_ERR_BAD_PAYLOAD;
      send_response_message(handler, op_data, QAPI_NET_LWM2M_406_NOT_ACCEPTABLE_E);
      goto end;
    }
  }

  if (payload_length == 0)
  {
    LOG_INFO("Payload Length is zero");
    result = QAPI_ERROR;
    goto end;
  }
  else
  {
    read_data->payload_len = payload_length;
    tx_byte_allocate(byte_pool_qcli, (VOID **)&(read_data->payload), 
                     payload_length, TX_NO_WAIT);

    if(read_data->payload == NULL)
    {
      LOG_INFO("Error in memory allocation");
      send_response_message(handler, op_data, QAPI_NET_LWM2M_500_INTERNAL_SERVER_E);
      result = QAPI_ERR_NO_MEMORY;
      goto end;
    }
    if (payload == NULL)
    {
      LOG_INFO("Payload is NULL");
      result = QAPI_ERROR;
      goto end;
    }
    else
    {
      if((cont_type== QAPI_NET_LWM2M_TEXT_PLAIN || cont_type == QAPI_NET_LWM2M_APPLICATION_OCTET_STREAM))
      {
        if((size != 1
           || !(op_data.obj_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_ID_E)
           || data->type == QAPI_NET_LWM2M_TYPE_OBJECT
           || data->type == QAPI_NET_LWM2M_TYPE_OBJECT_INSTANCE
           || data->type == QAPI_NET_LWM2M_TYPE_MULTIPLE_RESOURCE))
        {
          cont_type = QAPI_NET_LWM2M_M2M_TLV;
        }
      }
      memcpy(read_data->payload, payload, payload_length);
      read_data->msg_type    = QAPI_NET_LWM2M_RESPONSE_MSG_E;
      read_data->obj_info    = obj_info;
      read_data->status_code = QAPI_NET_LWM2M_205_CONTENT_E;
      read_data->conf_msg    = 0;
      read_data->msg_id_len  = op_data.msg_id_len;
      memcpy(read_data->msg_id, op_data.msg_id, op_data.msg_id_len);
      read_data->content_type = cont_type;
    
      result = qapi_Net_LWM2M_Send_Message(handler, read_data);
    }
  }

end:
  if(data)
  {
    lwm2m_data_free(size, data);
  }
  if(read_data)
  {
    if(read_data->payload)
    {
      tx_byte_release(read_data->payload);
      read_data->payload = NULL;
    }
    tx_byte_release(read_data);
    read_data = NULL;
  }
  if(payload)
  {
    tx_byte_release(payload);
    payload = NULL;
  }
  return result;
}

qapi_Status_t write_data_ext(uint16_t instanceid, int size, qapi_Net_LWM2M_Flat_Data_t *data,
                             qapi_Net_LWM2M_Data_t *object, uint16_t res_inst_id, uint8_t msg_code)
{
  qapi_Net_LWM2M_Instance_Info_t *target = NULL;
  int i = 0;
  double float_value = 0;
  bool bool_val = false;
  int64_t int_val = 0;
  qapi_Net_LWM2M_Resource_Info_t *resource = NULL;
  qapi_Status_t result = QAPI_ERROR;

  target = lwm2m_instance_find(object->instance_info, instanceid);
  if(target == NULL)
  {
    return QAPI_ERR_NO_ENTRY;
  }

  if(object->object_ID == LWM2M_GENERIC_SENSOR_OBJECT_ID)
  {
    i = 0;
    do
    {
      switch(data[i].id)
      {
        case RES_M_SENSOR_VALUE:
        case RES_O_SENSOR_UNITS:
        case RES_O_MIN_MEASURED_VAL:
        case RES_O_MAX_MEASURED_VAL:
        case RES_O_RESET_MEASURED_VAL:
        case RES_O_MEAN_VAL:
            result = QAPI_ERR_NOT_SUPPORTED;
            break;
        case RES_M_MIN_RANGE_VAL:
        case RES_M_MAX_RANGE_VAL:
        case RES_M_SENSOR_STATE:
        case RES_O_SNSR_VALUES:
            result = QAPI_OK;
            break;
        default:
            result = QAPI_ERROR;
            break;
      }
      i++;
    }while(i < size && result == QAPI_OK);
  }

  else if(object->object_ID == LWM2M_PRESENCE_SENSOR_OBJECT_ID)
  {
    i = 0;
    do
    {
      switch(data[i].id)
      {
        case RES_M_DIG_INPUT_STATE:
        case RES_O_DIG_INPUT_COUNTER:
        case RES_O_RESET_DIG_INPUT_COUNTER:
        case RES_O_SENSOR_TYPE:
            result = QAPI_ERR_NOT_SUPPORTED;
            break;
        case RES_O_BUSY_TO_CLEAR_DELAY:
        case RES_O_CLEAR_TO_BUSY_DELAY:
        case RES_O_SUP_SENSOR_TYPE:
            result = QAPI_OK;
            break;
        default:
            result = QAPI_ERROR;
            break;
      }
      i++;
    }while(i < size && result == QAPI_OK);
  }

  if(result == QAPI_OK)
  {
    if(object->object_ID == LWM2M_GENERIC_SENSOR_OBJECT_ID)
    {
      i = 0;
      do
      {
        switch(data[i].id)
        {
          case RES_M_MIN_RANGE_VAL:
          {
            if(lwm2m_data_decode_float(data+i,&(float_value)) == 1)
            {
              resource = lwm2m_resource_find(target->resource_info, data[i].id);
              if(resource != NULL)
              {
                resource->value.asFloat = float_value;
                result = QAPI_OK;
              }
              else
              {
                result = QAPI_ERR_NO_ENTRY;
              }
            }
            else
            {
              result = QAPI_ERR_INVALID_PARAM;
            }
          }
          break;
        
          case RES_M_MAX_RANGE_VAL:
          {
            if(lwm2m_data_decode_float(data+i,&(float_value)) == 1)
            {
              resource = lwm2m_resource_find(target->resource_info, data[i].id);
              if(resource != NULL)
              {
                resource->value.asFloat = float_value;
                result = QAPI_OK;
              }
              else
              {
                result = QAPI_ERR_NO_ENTRY;
              }
            }
            else
            {
              result = QAPI_ERR_INVALID_PARAM;
            }
          }
          break;
    
          case RES_M_SENSOR_STATE:
          {
            if(lwm2m_data_decode_bool(data+i,&(bool_val)) == 1)
            {
              resource = lwm2m_resource_find(target->resource_info, data[i].id);
              if(resource != NULL)
              {
                resource->value.asBoolean = bool_val;
                result = QAPI_OK;
              }
              else
              {
                result = QAPI_ERR_NO_ENTRY;
              }
            }
            else
            {
              result = QAPI_ERR_INVALID_PARAM;
            }
          }
          break;

          case RES_O_SNSR_VALUES:
          {
            result = ext_generic_write_resources(instanceid, data+i, object, res_inst_id, msg_code);
            if(result == QAPI_NET_LWM2M_204_CHANGED_E)
              result = QAPI_OK;
            else if(result == QAPI_NET_LWM2M_404_NOT_FOUND_E)
              result = QAPI_ERR_NO_ENTRY;
            else if(result == QAPI_NET_LWM2M_500_INTERNAL_SERVER_E)
              result = QAPI_ERR_INVALID_PARAM;
            else 
              result = QAPI_ERROR;
          }
          break;

          default:
          if(size > 1)
          result = QAPI_OK;
          else
          result = QAPI_ERR_NO_ENTRY;
        }
        i++;
      }while(i < size && result == QAPI_OK);
    }
    
    else
    {
      i = 0;
      do
      {
        switch(data[i].id)
        {
          case RES_O_BUSY_TO_CLEAR_DELAY:
          {
            if(lwm2m_data_decode_int(data+i,&(int_val)) == 1)
            {
              resource = lwm2m_resource_find(target->resource_info, data[i].id);
              if(resource != NULL)
              {
                resource->value.asInteger = int_val;
                result = QAPI_OK;
              }
              else
              {
                result = QAPI_ERR_NO_ENTRY;
              }
            }
            else
            {
              result = QAPI_ERR_INVALID_PARAM;
            }
          }
          break;
        
          case RES_O_CLEAR_TO_BUSY_DELAY:
          {
            if(lwm2m_data_decode_int(data+i,&(int_val)) == 1)
            {
              resource = lwm2m_resource_find(target->resource_info, data[i].id);
              if(resource != NULL)
              {
                resource->value.asInteger = int_val;
                result = QAPI_OK;
              }
              else
              {
                result = QAPI_ERR_NO_ENTRY;
              }
            }
            else
            {
              result = QAPI_ERR_INVALID_PARAM;
            }
          }
          break;
    
          case RES_O_SUP_SENSOR_TYPE:
          {
            result = ext_presence_write_resources(instanceid, data+i, object, res_inst_id, msg_code);
            if(result == QAPI_NET_LWM2M_204_CHANGED_E)
              result = QAPI_OK;
            else if(result == QAPI_NET_LWM2M_404_NOT_FOUND_E)
              result = QAPI_ERR_NO_ENTRY;
            else if(result == QAPI_NET_LWM2M_500_INTERNAL_SERVER_E)
              result = QAPI_ERR_INVALID_PARAM;
            else 
              result = QAPI_ERROR;
          }
          break;
    
          default:
          if(size > 1)
          result = QAPI_OK;
          else
          result = QAPI_ERR_NO_ENTRY;
        }
        i++;
      }while(i < size && result == QAPI_OK);
    }
  }
  return result;
}

qapi_Status_t write_ext_obj(qapi_Net_LWM2M_App_Handler_t handler, 
                            qapi_Net_LWM2M_Server_Data_t op_data,
                            uint8_t msg_code)
{
  qapi_Status_t result = QAPI_ERROR;
  qapi_Net_LWM2M_Obj_Info_t  obj_info;
  qapi_Net_LWM2M_Data_t *object = NULL;
  qapi_Net_LWM2M_Instance_Info_t *instance = NULL;
  qapi_Net_LWM2M_App_Ex_Obj_Data_t *write_data = NULL;
  qapi_Net_LWM2M_Flat_Data_t *data = NULL;
  size_t data_len;

  /*if(object_generic == NULL)
  {
    LOG_INFO("Object is not valid.");
    send_response_message(op_data, QAPI_NET_LWM2M_500_INTERNAL_SERVER_E);
    return QAPI_ERROR;
  }*/

  if(handler == NULL)
  {
    LOG_INFO("Application handle is not valid for current application");
    //send_response_message(op_data, QAPI_NET_LWM2M_400_BAD_REQUEST_E);
    return QAPI_ERR_INVALID_PARAM;
  }
  
  obj_info = op_data.obj_info;

  if(obj_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_ID_E)
  {
    if(handler == ext_object_handler_app1)
    {
      if(lwm2m_object_find(object_generic, obj_info.obj_id))
      {
        if(check_writable(obj_info.obj_id, obj_info.res_id) == false)
        {
          LOG_INFO("Write on resource %d not allowed.", obj_info.res_id);
          send_response_message(handler, op_data, QAPI_NET_LWM2M_405_METHOD_NOT_ALLOWED_E);
          return QAPI_ERROR;
        }
      }
    }
    else if(handler == ext_object_handler_app2)
    {
      if(lwm2m_object_find(object_presence, obj_info.obj_id))
      {
        if(check_writable(obj_info.obj_id, obj_info.res_id) == false)
        {
          LOG_INFO("Write on resource %d not allowed.", obj_info.res_id);
          send_response_message(handler, op_data, QAPI_NET_LWM2M_405_METHOD_NOT_ALLOWED_E);
          return QAPI_ERROR;
        }
      }
    }
  }

  tx_byte_allocate(byte_pool_qcli, (VOID **)&write_data, sizeof(qapi_Net_LWM2M_App_Ex_Obj_Data_t), TX_NO_WAIT);
  if(write_data == NULL)
  {
    LOG_INFO("Memory allocation failure.\n");
    send_response_message(handler, op_data, QAPI_NET_LWM2M_500_INTERNAL_SERVER_E);
    return QAPI_ERR_NO_MEMORY;
  }
  memset(write_data, 0x00, sizeof(qapi_Net_LWM2M_App_Ex_Obj_Data_t));

  if((obj_info.obj_mask & QAPI_NET_LWM2M_OBJECT_ID_E ))
  {
    if(handler == ext_object_handler_app1)
    {
      object = lwm2m_object_find(object_generic, obj_info.obj_id);
    }
    else if(handler == ext_object_handler_app2)
    {
      object = lwm2m_object_find(object_presence, obj_info.obj_id);
    }

    if(object != NULL)
    {
      if((obj_info.obj_mask & QAPI_NET_LWM2M_INSTANCE_ID_E))
      {
        instance = lwm2m_instance_find(object->instance_info, obj_info.obj_inst_id);
        if(instance != NULL)
        {
          result = qapi_Net_LWM2M_Decode_Data(handler,
                                                     &op_data.obj_info,
                                              op_data.payload,
	                                          op_data.payload_len,
                                              op_data.content_type,
                                              &data,
                                              &data_len);
          if(result != QAPI_OK || data_len == 0)
          {
            LOG_INFO("Error in payload decoding.");
            result = QAPI_ERR_BAD_PAYLOAD;
            send_response_message(handler, op_data, QAPI_NET_LWM2M_406_NOT_ACCEPTABLE_E);
            goto end;
          }
        }
        else
        {
          LOG_INFO("Instance id not found.");
          result = QAPI_ERR_NO_ENTRY;
          send_response_message(handler, op_data, QAPI_NET_LWM2M_404_NOT_FOUND_E);
          goto end;
        }
      }
      else
      {
        LOG_INFO("Object level write not allowed.");
        result = QAPI_ERR_NOT_SUPPORTED;
        send_response_message(handler, op_data, QAPI_NET_LWM2M_405_METHOD_NOT_ALLOWED_E);
        goto end;
      }
    }
    else
    {
      LOG_INFO("Object id not found.");
      result = QAPI_ERR_NO_ENTRY;
      send_response_message(handler, op_data, QAPI_NET_LWM2M_404_NOT_FOUND_E);
      goto end;
    }
  }
  else
  {
    LOG_INFO("Not valid request.");
    result = QAPI_ERR_INVALID_PARAM;
    send_response_message(handler, op_data, QAPI_NET_LWM2M_400_BAD_REQUEST_E);
    goto end;
  }

  if(data_len == 1 && 
     (data[0].type == QAPI_NET_LWM2M_TYPE_OBJECT_INSTANCE))
  {
    LOG_INFO("count:%d, type:%d", data[0].value.asChildren.count, data[0].type);
 
    result = write_data_ext(instance->instance_ID, data[0].value.asChildren.count, data[0].value.asChildren.array, object, INVALID_RES_INST_ID, msg_code);
  }
  else if((obj_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_INSTANCE_ID_E))
  {
    result = write_data_ext(instance->instance_ID, data_len, data, object, obj_info.res_inst_id, msg_code);
  }
  else
  {
    result = write_data_ext(instance->instance_ID, data_len, data, object, INVALID_RES_INST_ID, msg_code);
  }
  

  if(data)
  {
    lwm2m_data_free(data_len, data);
  }

  if(result == QAPI_OK)
  {
    write_data->msg_type = QAPI_NET_LWM2M_RESPONSE_MSG_E;
    write_data->obj_info = op_data.obj_info;
    write_data->status_code = QAPI_NET_LWM2M_204_CHANGED_E;
    write_data->conf_msg = 0;
    write_data->msg_id_len = op_data.msg_id_len;
    memcpy(write_data->msg_id, op_data.msg_id, op_data.msg_id_len);

    result = qapi_Net_LWM2M_Send_Message(handler, write_data);
  }

  else if(result == QAPI_ERR_NO_ENTRY)
  {
    LOG_INFO("Not valid request");
    result = QAPI_ERR_NO_ENTRY;
    send_response_message(handler, op_data, QAPI_NET_LWM2M_404_NOT_FOUND_E);
    goto end;
  }

  else if(result == QAPI_ERR_INVALID_PARAM)
  {
    LOG_INFO("Error in proccesing request");
    result = QAPI_ERR_INVALID_PARAM;
    send_response_message(handler, op_data, QAPI_NET_LWM2M_500_INTERNAL_SERVER_E);
    goto end;
  }
  
  else if(result == QAPI_ERR_NOT_SUPPORTED)
  {
    LOG_INFO("Write not allowed on readable resources");
    result = QAPI_ERR_NOT_SUPPORTED;
    send_response_message(handler, op_data, QAPI_NET_LWM2M_405_METHOD_NOT_ALLOWED_E);
    goto end;
  }

  else if(result == QAPI_ERROR)
  {
    LOG_INFO("Resource not present");
    result = QAPI_ERROR;
    send_response_message(handler, op_data, QAPI_NET_LWM2M_400_BAD_REQUEST_E);
    goto end;
  }

end:
  if(write_data)
  {
    /*if(write_data->payload)
    {
      tx_byte_release(write_data->payload);
      //free(write_data->payload);
      write_data->payload = NULL;
    }*/
    tx_byte_release(write_data);
    //free(write_data);
    write_data = NULL;
  }

  return result;
}

qapi_Status_t exec_data_ext(uint16_t instanceid, qapi_Net_LWM2M_Data_t *object)
{
  qapi_Net_LWM2M_Instance_Info_t *target = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource1 = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource2 = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource3 = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource4 = NULL;

  target = lwm2m_instance_find(object->instance_info, instanceid);
  if(target == NULL)
  {
    return QAPI_ERR_NO_ENTRY;
  }

  if(object->object_ID == LWM2M_GENERIC_SENSOR_OBJECT_ID)
  {
    resource1 = lwm2m_resource_find(target->resource_info, RES_O_MIN_MEASURED_VAL);
    resource2 = lwm2m_resource_find(target->resource_info, RES_M_MIN_RANGE_VAL);
    resource3 = lwm2m_resource_find(target->resource_info, RES_O_MAX_MEASURED_VAL);
    resource4 = lwm2m_resource_find(target->resource_info, RES_M_MAX_RANGE_VAL);
    if(resource1 != NULL && resource2 != NULL)
    {
      resource1->value.asFloat = resource2->value.asFloat;
    }
    else
    {
      return QAPI_ERR_INVALID_PARAM;
    }
    if(resource3 != NULL && resource4 != NULL)
    {
      resource3->value.asFloat = resource4->value.asFloat;
    }
    else
    {
      return QAPI_ERR_INVALID_PARAM;
    }
  }
  else
  {
    resource1 = lwm2m_resource_find(target->resource_info, RES_O_DIG_INPUT_COUNTER);
    if(resource1 != NULL)
    {
      resource1->value.asInteger = 0;
    }
    else
    {
      return QAPI_ERR_INVALID_PARAM;
    }
  }

  return QAPI_OK;
}

qapi_Status_t exec_ext_obj(qapi_Net_LWM2M_App_Handler_t handler, 
                           qapi_Net_LWM2M_Server_Data_t op_data)
{
  qapi_Status_t result = QAPI_ERROR;
  qapi_Net_LWM2M_Data_t *object = NULL;
  qapi_Net_LWM2M_Obj_Info_t  obj_info;
  qapi_Net_LWM2M_Instance_Info_t *instance = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource = NULL;
  qapi_Net_LWM2M_App_Ex_Obj_Data_t *exec_data = NULL;

  /*if(object_generic == NULL)
  {
    LOG_INFO("Object is not valid.");
    send_response_message(op_data, QAPI_NET_LWM2M_500_INTERNAL_SERVER_E);
    return QAPI_ERROR;
  }*/

  if(handler == NULL )
  {
    LOG_INFO("Application handle is not valid for current application");
    //send_response_message(op_data, QAPI_NET_LWM2M_400_BAD_REQUEST_E);
    return QAPI_ERR_INVALID_PARAM;
  }

  
  obj_info = op_data.obj_info;

  if(obj_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_ID_E)
  {
    if(handler == ext_object_handler_app1)
    {
      if(lwm2m_object_find(object_generic, obj_info.obj_id))
      {
        if(check_executable(obj_info.obj_id, obj_info.res_id) == false)
        {
          LOG_INFO("Execute on resource %d not allowed.", obj_info.res_id);
          send_response_message(handler, op_data, QAPI_NET_LWM2M_405_METHOD_NOT_ALLOWED_E);
          return QAPI_ERROR;
        }
      }
    }
    else if(handler == ext_object_handler_app2)
    {
      if(lwm2m_object_find(object_presence, obj_info.obj_id))
      {
        if(check_executable(obj_info.obj_id, obj_info.res_id) == false)
        {
          LOG_INFO("Execute on resource %d not allowed.", obj_info.res_id);
          send_response_message(handler, op_data, QAPI_NET_LWM2M_405_METHOD_NOT_ALLOWED_E);
          return QAPI_ERROR;
        }
      }
    }
  }

  if((obj_info.obj_mask & QAPI_NET_LWM2M_OBJECT_ID_E))
  {
    if(handler == ext_object_handler_app1)
    {
      object = lwm2m_object_find(object_generic, obj_info.obj_id);
    }
    else if(handler == ext_object_handler_app2)
    {
      object = lwm2m_object_find(object_presence, obj_info.obj_id);
    }

    if(object == NULL)
    {
      LOG_INFO("Object id not found.");
      send_response_message(handler, op_data, QAPI_NET_LWM2M_404_NOT_FOUND_E);
      return QAPI_ERR_NO_ENTRY;
    }
    else
    {
      if((obj_info.obj_mask & QAPI_NET_LWM2M_INSTANCE_ID_E ))
      {
        instance = lwm2m_instance_find(object->instance_info, obj_info.obj_inst_id);
        if(instance == NULL)
        {
          LOG_INFO("Instance id not found.");
          send_response_message(handler, op_data, QAPI_NET_LWM2M_404_NOT_FOUND_E);
          return QAPI_ERR_NO_ENTRY;
        }
        else
        {
          if((obj_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_ID_E))
          {
            resource = lwm2m_resource_find(instance->resource_info, obj_info.res_id);
            if(resource == NULL)
            {
              LOG_INFO("Resource id not found.");
              send_response_message(handler, op_data, QAPI_NET_LWM2M_404_NOT_FOUND_E);
              return QAPI_ERR_NO_ENTRY;
            }
            else
            {
              result = exec_data_ext(instance->instance_ID, object);

              if(result != QAPI_OK)
              {
                LOG_INFO("Execute Operation unsuccessful.");
                send_response_message(handler, op_data, QAPI_NET_LWM2M_500_INTERNAL_SERVER_E);
                return QAPI_ERR_NO_ENTRY;
              }

              tx_byte_allocate(byte_pool_qcli, (VOID **)&exec_data, sizeof(qapi_Net_LWM2M_App_Ex_Obj_Data_t), TX_NO_WAIT);
              if(exec_data == NULL)
              {
                LOG_INFO("Error in memory allocation");
                send_response_message(handler, op_data, QAPI_NET_LWM2M_500_INTERNAL_SERVER_E);
                return QAPI_ERR_NO_MEMORY;
              }
              memset(exec_data, 0x00, sizeof(qapi_Net_LWM2M_App_Ex_Obj_Data_t));

              exec_data->msg_type = QAPI_NET_LWM2M_RESPONSE_MSG_E;
              exec_data->obj_info = op_data.obj_info;
              exec_data->status_code = QAPI_NET_LWM2M_204_CHANGED_E;
              exec_data->conf_msg = 0;
              exec_data->msg_id_len = op_data.msg_id_len;
              memcpy(exec_data->msg_id, op_data.msg_id, op_data.msg_id_len);

              result = qapi_Net_LWM2M_Send_Message(handler, exec_data);
  
               if(exec_data)
               {
                 /*if(exec_data->payload)
                 {
                   tx_byte_release(exec_data->payload);
                   //free(exec_data->payload);
                   exec_data->payload = NULL;
                 }*/
                 tx_byte_release(exec_data);
                 //free(exec_data);
                 exec_data = NULL;
               }
            }
          }
          else
          {
            LOG_INFO("Execute on instance level not allowed.");
            send_response_message(handler, op_data, QAPI_NET_LWM2M_405_METHOD_NOT_ALLOWED_E);
            return QAPI_ERR_INVALID_PARAM;
          }
        }
      }
      else
      {
        LOG_INFO("Execute on object level not allowed.");
        send_response_message(handler, op_data, QAPI_NET_LWM2M_405_METHOD_NOT_ALLOWED_E);
        return QAPI_ERR_INVALID_PARAM;
      }
    }
  }

  return result;
}

qapi_Status_t discover_data_ext(uint16_t instanceid, int *size, qapi_Net_LWM2M_Flat_Data_t **data,
                                qapi_Net_LWM2M_Data_t *object)
{
  qapi_Net_LWM2M_Instance_Info_t *target = NULL;
  int i = 0;
  int ri = 0;
  gen_multi_res_t *mult_inst1 = NULL;
  pre_multi_res_t *mult_inst2 = NULL;
  ext_resource_instance_int_list_t *sensor_values = NULL;
  ext_resource_instance_string_list_t *sensor_types = NULL;

  uint16_t reslist_ob1[] = {
                             RES_M_SENSOR_VALUE,
                             RES_O_SENSOR_UNITS,
                             RES_O_MIN_MEASURED_VAL,
                             RES_O_MAX_MEASURED_VAL,
                             RES_M_MIN_RANGE_VAL,
                             RES_M_MAX_RANGE_VAL,
                             RES_O_RESET_MEASURED_VAL,
                             RES_M_SENSOR_STATE,
                             RES_O_MEAN_VAL,
                             RES_O_SNSR_VALUES
                           };

  uint16_t reslist_ob2[] = {
                             RES_M_DIG_INPUT_STATE,
                             RES_O_DIG_INPUT_COUNTER,
                             RES_O_RESET_DIG_INPUT_COUNTER,
                             RES_O_SENSOR_TYPE,
                             RES_O_BUSY_TO_CLEAR_DELAY,
                             RES_O_CLEAR_TO_BUSY_DELAY,
                             RES_O_SUP_SENSOR_TYPE
                           };

  int nbres;
  int temp;

  target = lwm2m_instance_find(object->instance_info, instanceid);
  if(target == NULL)
  {
    return QAPI_ERR_NO_ENTRY;
  }

  if(object->object_ID == LWM2M_GENERIC_SENSOR_OBJECT_ID)
  {
    mult_inst1 = lwm2m_gen_inst_find(gen_multi_res, instanceid);
    if(mult_inst1 != NULL)
    {
      for(sensor_values = mult_inst1->sensors_val; sensor_values != NULL; sensor_values = sensor_values->next, ri++);
    }
  }
  else
  {
    mult_inst2 = lwm2m_pre_inst_find(pre_multi_res, instanceid);
    if(mult_inst2 != NULL)
    {
      for(sensor_types= mult_inst2->sensor_types; sensor_types != NULL; sensor_types = sensor_types->next, ri++);
    }
  }
  if(*size == 0)
  {
    if(object->object_ID == LWM2M_GENERIC_SENSOR_OBJECT_ID)
    {
      nbres = sizeof(reslist_ob1)/sizeof(uint16_t);
    }
    else
    {
      nbres = sizeof(reslist_ob2)/sizeof(uint16_t);
    }
    temp = nbres;
     
    *data = lwm2m_data_new(nbres);
    if(*data == NULL)
    {
      return QAPI_ERR_NO_MEMORY;
    }
    *size = nbres;

    if(object->object_ID == LWM2M_GENERIC_SENSOR_OBJECT_ID)
    {
      for(i = 0; i<temp; i++)
      {
        (*data)[i].id = reslist_ob1[i];
        if((*data)[i].id == RES_O_SNSR_VALUES)
        {
          (*data)[i].type = QAPI_NET_LWM2M_TYPE_MULTIPLE_RESOURCE;
          (*data)[i].value.asChildren.count = ri;
        }
      }
    }
    else
    {
      for(i = 0; i<temp; i++)
      {
        (*data)[i].id = reslist_ob2[i];
        if((*data)[i].id == RES_O_SUP_SENSOR_TYPE)
        {
          (*data)[i].type = QAPI_NET_LWM2M_TYPE_MULTIPLE_RESOURCE;
          (*data)[i].value.asChildren.count = ri;
        }
      }
    }
  }

  else
  {
    if(object->object_ID == LWM2M_GENERIC_SENSOR_OBJECT_ID)
    {
      for(i =0; i < *size ; i++)
      {
        switch((*data)[i].id)
        {
          case RES_M_SENSOR_VALUE:
          case RES_O_SENSOR_UNITS:
          case RES_O_MIN_MEASURED_VAL:
          case RES_O_MAX_MEASURED_VAL:
          case RES_M_MIN_RANGE_VAL:
          case RES_M_MAX_RANGE_VAL:
          case RES_O_RESET_MEASURED_VAL:
          case RES_M_SENSOR_STATE:
          case RES_O_MEAN_VAL:
               break;
          case RES_O_SNSR_VALUES:
               (*data)[i].type = QAPI_NET_LWM2M_TYPE_MULTIPLE_RESOURCE;
               (*data)[i].value.asChildren.count = ri;
               break;
          default:
              return QAPI_ERROR;
        }
      }
    }
    else
    {
      for(i =0; i < *size ; i++)
      {
        switch((*data)[i].id)
        {
          case RES_M_DIG_INPUT_STATE:
          case RES_O_DIG_INPUT_COUNTER:
          case RES_O_RESET_DIG_INPUT_COUNTER:
          case RES_O_SENSOR_TYPE:
          case RES_O_BUSY_TO_CLEAR_DELAY:
          case RES_O_CLEAR_TO_BUSY_DELAY:
               break;
          case RES_O_SUP_SENSOR_TYPE:
               (*data)[i].type = QAPI_NET_LWM2M_TYPE_MULTIPLE_RESOURCE;
               (*data)[i].value.asChildren.count = ri;
               break;
          default:
              return QAPI_ERROR;
        }
      }
    }
  }

  return QAPI_OK;
}

qapi_Status_t disc_ext_obj(qapi_Net_LWM2M_App_Handler_t handler, 
                           qapi_Net_LWM2M_Server_Data_t op_data)
{
  qapi_Status_t result = QAPI_ERROR;
  qapi_Net_LWM2M_Obj_Info_t  obj_info;
  qapi_Net_LWM2M_Data_t *object = NULL;
  qapi_Net_LWM2M_Instance_Info_t *instance = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource = NULL;
  qapi_Net_LWM2M_App_Ex_Obj_Data_t *disc_data = NULL;
  qapi_Net_LWM2M_Flat_Data_t *data = NULL;
  uint8_t *payload = NULL;
  uint32_t payload_length = 0;
  int i = 0;
  int size = 0;

  if(handler == NULL)
  {
    LOG_INFO("Application handle is not valid for current application");
    return QAPI_ERR_INVALID_PARAM;
  }

  obj_info = op_data.obj_info;

  if((obj_info.obj_mask & QAPI_NET_LWM2M_OBJECT_ID_E))
  {
    if(handler == ext_object_handler_app1)
    {
      object = lwm2m_object_find(object_generic, obj_info.obj_id);
    }
    else if(handler == ext_object_handler_app2)
    {
      object = lwm2m_object_find(object_presence, obj_info.obj_id);
    }

    if(object != NULL)
    {
      if(object->instance_info == NULL && !(obj_info.obj_mask & QAPI_NET_LWM2M_INSTANCE_ID_E))
      {
        LOG_INFO("Instance list is empty.");
        result = QAPI_OK;
        goto obj_disc;
      }

      if((obj_info.obj_mask & QAPI_NET_LWM2M_INSTANCE_ID_E))
      {
        instance = lwm2m_instance_find(object->instance_info, obj_info.obj_inst_id);
        if(instance == NULL)
        {
          LOG_INFO("Instance id not found.");
          result = QAPI_ERR_NO_ENTRY;
          send_response_message(handler, op_data, QAPI_NET_LWM2M_404_NOT_FOUND_E);
          goto end;
        }
        else
        {
          if((obj_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_ID_E))
          {
            resource = lwm2m_resource_find(instance->resource_info, obj_info.res_id);
            if(resource == NULL)
            {
              LOG_INFO("Resource id not found.");
              result = QAPI_ERR_NO_ENTRY;
              send_response_message(handler, op_data, QAPI_NET_LWM2M_404_NOT_FOUND_E);
              goto end;
            }
            size = 1;
            data = lwm2m_data_new(size);
            if(data == NULL)
            {
              result = QAPI_ERR_NO_MEMORY;
              send_response_message(handler, op_data, QAPI_NET_LWM2M_500_INTERNAL_SERVER_E);
              goto end;
            }
            (data)->id = resource->resource_ID;
          }
          result = discover_data_ext(instance->instance_ID, &size, &data, object);
        }
      }
      else
      {
        for(instance = object->instance_info; instance != NULL; instance = instance->next)
        {
          size++;
        }
        data = lwm2m_data_new(size);
        if(data == NULL)
        {
          result = QAPI_ERR_NO_MEMORY;
          send_response_message(handler, op_data, QAPI_NET_LWM2M_500_INTERNAL_SERVER_E);
          goto end;
        }
        instance = object->instance_info;
        i =0;
        result = QAPI_OK;
        while(instance != NULL && result == QAPI_OK)
        {
          result = discover_data_ext(instance->instance_ID, (int *)&((data)[i].value.asChildren.count),
                                     &((data)[i].value.asChildren.array), object);
          (data)[i].id = instance->instance_ID;
          (data)[i].type = QAPI_NET_LWM2M_TYPE_OBJECT_INSTANCE;
          i++;
          instance = instance->next;
        }
      }
    }
    else
    {
      LOG_INFO("object id not found.");
      result = QAPI_ERR_NO_ENTRY;
      send_response_message(handler, op_data, QAPI_NET_LWM2M_404_NOT_FOUND_E);
      goto end;
    }
  }
  else
  {
    LOG_INFO("Not valid request.");
    result = QAPI_ERR_INVALID_PARAM;
    send_response_message(handler, op_data, QAPI_NET_LWM2M_400_BAD_REQUEST_E);
    goto end;
  }

obj_disc:
  if(result == QAPI_OK)
  {
    int len = 0;
    qapi_Net_LWM2M_Uri_t uri;
    memset(&uri, 0x00, sizeof(qapi_Net_LWM2M_Uri_t));
    uri.flag = obj_info.obj_mask;
    uri.objectId = obj_info.obj_id;
    uri.instanceId = obj_info.obj_inst_id;
    uri.resourceId = obj_info.res_id;
    uri.resourceInstId = obj_info.res_inst_id;
    
    len = ext_obj_discover_serialize(handler, op_data, &uri, size, data, &payload);
  
    if(len <= 0)
    {
      send_response_message(handler, op_data, QAPI_NET_LWM2M_500_INTERNAL_SERVER_E);
      result = QAPI_ERR_NO_MEMORY;
      goto end;
    }
    else
    {
      payload_length = len;
    }
  }

  if(data)
  {
    lwm2m_data_free(size, data);
  }

  if(payload == NULL || payload_length == 0)
  {
    send_response_message(handler, op_data, QAPI_NET_LWM2M_500_INTERNAL_SERVER_E);
    result = QAPI_ERR_NO_MEMORY;
    goto end;
  }

  tx_byte_allocate(byte_pool_qcli, (VOID **)&disc_data, 
                   sizeof(qapi_Net_LWM2M_App_Ex_Obj_Data_t), TX_NO_WAIT);
  if(disc_data == NULL)
  {
    LOG_INFO("Memory allocation failure.\n");
    send_response_message(handler, op_data, QAPI_NET_LWM2M_500_INTERNAL_SERVER_E);
    return QAPI_ERR_NO_MEMORY;
  }
  memset(disc_data, 0x00, sizeof(qapi_Net_LWM2M_App_Ex_Obj_Data_t));

  disc_data->payload_len = payload_length;
  tx_byte_allocate(byte_pool_qcli, (VOID **)&(disc_data->payload), 
                   payload_length, TX_NO_WAIT);
  if(disc_data->payload == NULL)
  {
    LOG_INFO("Error in memory allocation");
    send_response_message(handler, op_data, QAPI_NET_LWM2M_500_INTERNAL_SERVER_E);
    result = QAPI_ERR_NO_MEMORY;
    goto end;
  }
  memcpy(disc_data->payload, payload, payload_length);
  disc_data->msg_type    = QAPI_NET_LWM2M_RESPONSE_MSG_E;
  disc_data->obj_info    = obj_info;
  disc_data->status_code = QAPI_NET_LWM2M_205_CONTENT_E;
  disc_data->conf_msg    = 0;
  disc_data->msg_id_len  = op_data.msg_id_len;
  memcpy(disc_data->msg_id, op_data.msg_id, op_data.msg_id_len);
  disc_data->content_type = QAPI_NET_LWM2M_APPLICATION_LINK_FORMAT;

  result = qapi_Net_LWM2M_Send_Message(handler, disc_data);

end:
  if(disc_data)
  {
    if(disc_data->payload)
    {
      tx_byte_release(disc_data->payload);
      disc_data->payload = NULL;
    }
    tx_byte_release(disc_data);
    disc_data = NULL;
  }
  if(payload)
  {
    tx_byte_release(payload);
    payload = NULL;
  }
  return result;
}

void start_ext_obs_notify_timer()
{
  qapi_Status_t result = QAPI_ERROR;
  if(ext_observed_list == NULL)
  {
    qapi_TIMER_define_attr_t time_attr;
    time_attr.deferrable = FALSE;
    time_attr.cb_type = QAPI_TIMER_FUNC1_CB_TYPE;
    time_attr.sigs_func_ptr = notify_signal_cb_1;
    time_attr.sigs_mask_data = NULL;

    result = qapi_Timer_Def(&obs_notify_Timer1, &time_attr);
    if(result == QAPI_OK)
    {
      LOG_INFO("Timer 1 defined successfully.\n");
    }
    else
    {
      LOG_INFO("Timer 1 defined failed.\n");
    }
  }

  if(ext_multi_observed_list == NULL)
  {
    qapi_TIMER_define_attr_t time_attr;
    time_attr.deferrable = FALSE;
    time_attr.cb_type = QAPI_TIMER_FUNC1_CB_TYPE;
    time_attr.sigs_func_ptr = notify_signal_cb_2;
    time_attr.sigs_mask_data = NULL;

    result = qapi_Timer_Def(&obs_notify_Timer2, &time_attr);
    if(result == QAPI_OK)
    {
      LOG_INFO("Timer 2 defined successfully.\n");
    }
    else
    {
      LOG_INFO("Timer 2 defined failed.\n");
    }
  }
}

void stop_ext_obs_notify_timer(qapi_Net_LWM2M_App_Handler_t handler)
{
  if(handler == ext_object_handler_app1)
  {
    qapi_Timer_Stop(obs_notify_Timer1);
    qapi_Timer_Undef(obs_notify_Timer1);
  }

  else if(handler == ext_object_handler_app2)
  {
    qapi_Timer_Stop(obs_notify_Timer2);
    qapi_Timer_Undef(obs_notify_Timer2);
  }
}

static qapi_Net_LWM2M_Observed_t * lwm2m_find_observed(qapi_Net_LWM2M_App_Handler_t handler,
                                                qapi_Net_LWM2M_Uri_t *uri)
{
  qapi_Net_LWM2M_Observed_t *target = NULL;

  if(uri == NULL || handler == NULL)
  {
    return NULL;
  }

  if(handler == ext_object_handler_app1)
  {
    target = ext_observed_list;
  }

  else if(handler == ext_object_handler_app2)
  {
    target = ext_multi_observed_list;
  }

  while(target != NULL && (target->uri.objectId != uri->objectId
        || target->uri.flag != uri->flag
        || ((uri->flag & QAPI_NET_LWM2M_INSTANCE_ID_E) && target->uri.instanceId != uri->instanceId)
        || ((uri->flag & QAPI_NET_LWM2M_RESOURCE_ID_E) && target->uri.resourceId != uri->resourceId)))
  {
    target = target->next;
  }

  return target;
}

static void lwm2m_unlink_observed(qapi_Net_LWM2M_App_Handler_t handler,
                           qapi_Net_LWM2M_Observed_t *observed)
{
  if(handler == NULL || observed == NULL ) 
  {
    LOG_INFO("fun [%s] file [%s]  Passed  Argument is NULL\n",__func__,__FILE__);
    return;
  }

  if(handler == ext_object_handler_app1 && ext_observed_list == NULL)
  {
    LOG_INFO("fun [%s] file [%s]  Passed  Argument is NULL\n",__func__,__FILE__);
    return;
  }

  else if(handler == ext_object_handler_app2 && ext_multi_observed_list == NULL)
  {
    LOG_INFO("fun [%s] file [%s]  Passed  Argument is NULL\n",__func__,__FILE__);
    return;
  }

  if(handler == ext_object_handler_app1)
  {
    if (ext_observed_list == observed)
    {
      ext_observed_list = ext_observed_list->next;
    }
    else
    {
      qapi_Net_LWM2M_Observed_t *list = NULL;
  
      list = ext_observed_list;
      while (list->next != NULL && list->next != observed)
      {
        list = list->next;
      }
  
      if(list->next != NULL)
      {
        list->next = list->next->next;
      }
    }
  }

  else if(handler == ext_object_handler_app2)
  {
    if (ext_multi_observed_list == observed)
    {
      ext_multi_observed_list = ext_multi_observed_list->next;
    }
    else
    {
      qapi_Net_LWM2M_Observed_t *list = NULL;
  
      list = ext_multi_observed_list;
      while (list->next != NULL && list->next != observed)
      {
        list = list->next;
      }
  
      if(list->next != NULL)
      {
        list->next = list->next->next;
      }
    }
  }
  return;
}

void lwm2m_unlink_observe_info(qapi_Net_LWM2M_App_Handler_t handler,
                               qapi_Net_LWM2M_Observed_t *observed,
                               qapi_Net_LWM2M_Observe_info_t *observe_info)
{
  if(handler == NULL || observed == NULL || observe_info == NULL) 
  {
    LOG_INFO("fun [%s] file [%s]  Passed  Argument is NULL\n",__func__,__FILE__);
    return;
  }

  if(handler == ext_object_handler_app1 && ext_observed_list == NULL)
  {
    LOG_INFO("fun [%s] file [%s]  Passed  Argument is NULL\n",__func__,__FILE__);
    return;
  }

  else if(handler == ext_object_handler_app2 && ext_multi_observed_list == NULL)
  {
    LOG_INFO("fun [%s] file [%s]  Passed  Argument is NULL\n",__func__,__FILE__);
    return;
  }

  if (observed->observe_info == observe_info)
  {
    observed->observe_info = observed->observe_info->next;
  }
  else
  {
    qapi_Net_LWM2M_Observe_info_t *list = NULL;

    list = observed->observe_info;
    while (list->next != NULL && list->next != observe_info)
    {
      list = list->next;
    }

    if(list->next != NULL)
    {
      list->next = list->next->next;
    }
  }
  return;
}

static qapi_Net_LWM2M_Observe_info_t *lwm2m_find_observe_info(qapi_Net_LWM2M_Observed_t *observed,
                                                       uint8_t *msg_id,
                                                       uint8_t msg_id_len)
{
  qapi_Net_LWM2M_Observe_info_t *target = NULL;

  if(observed == NULL || msg_id == NULL ) 
  {
    LOG_INFO("fun [%s] file [%s]  Passed  Argument is NULL\n",__func__,__FILE__);
    return NULL;
  }
  
  target = observed->observe_info;
  while(target != NULL && (memcmp((target->msg_id + target->msg_id_len - 2), (msg_id +msg_id_len -2), 2) != 0))
  {
    target = target->next;
  }

  return target;
}

static qapi_Net_LWM2M_Observe_info_t *lwm2m_get_observe_info(qapi_Net_LWM2M_App_Handler_t handler,
                                                      qapi_Net_LWM2M_Uri_t *uri,
                                                      uint8_t *msg_id,
                                                      uint8_t msg_id_len)
{
  qapi_Net_LWM2M_Observed_t *observed = NULL;
  qapi_Net_LWM2M_Observe_info_t *observe_info = NULL;
  bool alloc = false;

  if(uri == NULL || msg_id == NULL || handler == NULL)
  {
    LOG_INFO("fun [%s] file [%s]  Passed  Argument is NULL\n",__func__,__FILE__);
    return NULL;
  }

  if(handler == ext_object_handler_app1)
  {
    if(ext_observed_list == NULL)
    {
      start_ext_obs_notify_timer();
    }
  }

  else if(handler == ext_object_handler_app2)
  {
    if(ext_multi_observed_list == NULL)
    {
      start_ext_obs_notify_timer();
    }
  }

  observed = lwm2m_find_observed(handler, uri);

  if(observed == NULL)
  {
    tx_byte_allocate(byte_pool_qcli, (VOID **)&observed, 
                   sizeof(qapi_Net_LWM2M_Observed_t), TX_NO_WAIT);
    if(observed == NULL)
    {
      return NULL;
    }
    alloc = true;
    memset(observed, 0x00, sizeof(qapi_Net_LWM2M_Observed_t));
    memcpy(&(observed->uri), uri, sizeof(qapi_Net_LWM2M_Uri_t));
    if(handler == ext_object_handler_app1)
    {
      observed->next = ext_observed_list;
      ext_observed_list = observed;
    }
    else if(handler == ext_object_handler_app2)
    {
      observed->next = ext_multi_observed_list;
      ext_multi_observed_list = observed;
    }
  }

  observe_info = lwm2m_find_observe_info(observed, msg_id, msg_id_len);
  if(observe_info == NULL)
  {
    tx_byte_allocate(byte_pool_qcli, (VOID **)&observe_info, 
                   sizeof(qapi_Net_LWM2M_Observe_info_t), TX_NO_WAIT);
    if(observe_info == NULL)
    {
      if(alloc == true)
      {
        tx_byte_release(observed);
        observed = NULL;
      }
      return NULL;
    }
    memset(observe_info, 0x00, sizeof(qapi_Net_LWM2M_Observe_info_t));
    observe_info->active = false;
    observe_info->msg_id_len = msg_id_len;
    memcpy(observe_info->msg_id, msg_id, msg_id_len);
    observe_info->next = observed->observe_info;
    observed->observe_info = observe_info;
  }

  return observe_info;
}

qapi_Net_LWM2M_Observe_info_t* ext_findInheritedWatcher(qapi_Net_LWM2M_App_Handler_t handler,
                                                        qapi_Net_LWM2M_Uri_t *uri,
                                                        uint8_t *msg_id,
                                                        uint8_t msg_id_len,
                                                        uint8_t flag)
{
  qapi_Net_LWM2M_Observe_info_t *observe_info = NULL;
  qapi_Net_LWM2M_Observed_t *observed = NULL;

  if(uri == NULL || msg_id == NULL || handler == NULL)
  {
    LOG_INFO("fun [%s] file [%s]  Passed  Argument is NULL\n",__func__,__FILE__);
    return NULL;
  }

  if((uri->flag & QAPI_NET_LWM2M_RESOURCE_ID_E))
  {
    uri->flag &= ~QAPI_NET_LWM2M_RESOURCE_ID_E;
    observed = lwm2m_find_observed(handler, uri);
    if(observed == NULL)
    {
      uri->flag &= ~QAPI_NET_LWM2M_INSTANCE_ID_E;
      observed = lwm2m_find_observed(handler, uri);
      if(observed == NULL)
      {
        //Do nothing
      }
      else
      {
        observe_info = lwm2m_find_observe_info(observed, msg_id, msg_id_len);
      }
      uri->flag |= QAPI_NET_LWM2M_INSTANCE_ID_E;
    }
    else
    {
      observe_info = lwm2m_find_observe_info(observed, msg_id, msg_id_len);
      if(observe_info != NULL && observe_info->attributes!= NULL)
      {
        if((observe_info->attributes->set_attr_mask & flag) == 0)
        {
          uri->flag &= ~QAPI_NET_LWM2M_INSTANCE_ID_E;
          observed = lwm2m_find_observed(handler, uri);
          if(observed == NULL)
          {
            observe_info = NULL;
            LOG_INFO("Observed list not found for URI \n");
          }
          else
          {
            observe_info = lwm2m_find_observe_info(observed, msg_id, msg_id_len);
          }
          uri->flag |= QAPI_NET_LWM2M_INSTANCE_ID_E;
        }
      }
      // If inherited watcher has no attributes set, move to object level
      else if(observe_info != NULL && observe_info->attributes == NULL)
      {
        uri->flag &= ~QAPI_NET_LWM2M_INSTANCE_ID_E;
        observed = lwm2m_find_observed(handler, uri);
        if(observed == NULL)
        {
          observe_info = NULL;
          LOG_INFO("Observed list not found for URI \n");
        }
        else
        {
          observe_info = lwm2m_find_observe_info(observed, msg_id, msg_id_len);
        }
        uri->flag |= QAPI_NET_LWM2M_INSTANCE_ID_E;
      }
    }
    uri->flag |= QAPI_NET_LWM2M_RESOURCE_ID_E;
  }
  else if((uri->flag & QAPI_NET_LWM2M_INSTANCE_ID_E))
  {
    uri->flag &= ~QAPI_NET_LWM2M_INSTANCE_ID_E;
    observed = lwm2m_find_observed(handler, uri);
    if(observed == NULL)
    {
      //Do nothing
    }
    else
    {
      observe_info = lwm2m_find_observe_info(observed, msg_id, msg_id_len);
    }
    uri->flag |= QAPI_NET_LWM2M_INSTANCE_ID_E;
  }

  return observe_info;
}

qapi_Status_t observe_ext_handle_req(qapi_Net_LWM2M_App_Handler_t handler,
                                     qapi_Net_LWM2M_Data_t *object,
                                     qapi_Net_LWM2M_Server_Data_t op_data,
                                     int size, qapi_Net_LWM2M_Flat_Data_t *data)
{
  qapi_Net_LWM2M_Observe_info_t *observe_info = NULL;
  qapi_Net_LWM2M_Uri_t uri;
  uint8_t msg_id[QAPI_MAX_LWM2M_MSG_ID_LENGTH];
  uint8_t msg_id_len;
  qapi_time_get_t curr_time;
  qapi_Status_t result = QAPI_ERROR;

  if(handler == NULL || object == NULL || data == NULL)
  {
    LOG_INFO("fun [%s] file [%s]  Passed  Argument is NULL\n",__func__,__FILE__);
    return QAPI_ERROR;
  }

  memset(&uri, 0x00, sizeof(qapi_Net_LWM2M_Uri_t));

  uri.flag = op_data.obj_info.obj_mask;
  uri.objectId = op_data.obj_info.obj_id;
  uri.instanceId = op_data.obj_info.obj_inst_id;
  uri.resourceId = op_data.obj_info.res_id;
  uri.resourceInstId = op_data.obj_info.res_inst_id;

  msg_id_len = op_data.msg_id_len;
  memcpy(msg_id, op_data.msg_id, op_data.msg_id_len);

  observe_info = lwm2m_get_observe_info(handler, &uri, msg_id, msg_id_len);

  if(observe_info == NULL)
  {
    LOG_INFO("Observe info is NULL\n");
    return QAPI_ERROR;
  }

  if(handler == ext_object_handler_app1)
  {
    qapi_TIMER_set_attr_t time_attr;
    time_attr.unit = QAPI_TIMER_UNIT_SEC;
    time_attr.reload = TRUE;
    time_attr.time = 1;
    result = qapi_Timer_Set(obs_notify_Timer1, &time_attr);
    if(result == QAPI_OK)
    {
      LOG_INFO("Timer 1 set successfully.\n");
    }
    else
    {
      LOG_INFO("Timer 1 set failed.\n");
    }
  }

  else if(handler == ext_object_handler_app2)
  {
    qapi_TIMER_set_attr_t time_attr;
    time_attr.unit = QAPI_TIMER_UNIT_SEC;
    time_attr.reload = TRUE;
    time_attr.time = 1;
    result = qapi_Timer_Set(obs_notify_Timer2, &time_attr);
    if(result == QAPI_OK)
    {
      LOG_INFO("Timer 2 set successfully.\n");
    }
    else
    {
      LOG_INFO("Timer 2 set failed.\n");
    }
  }

  observe_info->active = true;
  qapi_time_get(QAPI_TIME_SECS, &curr_time);

  observe_info->last_time = curr_time.time_secs;
  //update msg_id:
  //when write-attribute, observe info is created with write-attr msgid, 
  //then when observe execute, need update msgid with observe msgid)
  memset(observe_info->msg_id, 0, QAPI_MAX_LWM2M_MSG_ID_LENGTH);
  memcpy(observe_info->msg_id, msg_id, msg_id_len);    
  observe_info->msg_id_len = msg_id_len;
  if(op_data.accept_is_valid)
  {
    observe_info->format = op_data.accept;
  }
  else if ((op_data.content_type == QAPI_NET_LWM2M_TEXT_PLAIN || op_data.content_type == QAPI_NET_LWM2M_APPLICATION_OCTET_STREAM))
  {
    if((size != 1
       || !(op_data.obj_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_ID_E)
       || data->type == QAPI_NET_LWM2M_TYPE_OBJECT
       || data->type == QAPI_NET_LWM2M_TYPE_OBJECT_INSTANCE
       || data->type == QAPI_NET_LWM2M_TYPE_MULTIPLE_RESOURCE))
    {
      observe_info->format = QAPI_NET_LWM2M_M2M_TLV;
    }
    else
    {
      observe_info->format = op_data.content_type;
    }
  }
  else
  {
    observe_info->format = op_data.content_type;
  }

  if(uri.flag & QAPI_NET_LWM2M_RESOURCE_ID_E)
  {
    switch(data->type)
    {
      case QAPI_NET_LWM2M_TYPE_INTEGER_E:
           if (1 != lwm2m_data_decode_int(data, &(observe_info->lastvalue.asInteger))) 
           return QAPI_ERROR;
           break;
      case QAPI_NET_LWM2M_TYPE_FLOAT_E:
            if (1 != lwm2m_data_decode_float(data, &(observe_info->lastvalue.asFloat))) 
            return QAPI_ERROR;
            break;
      default:
            break;
    }
  }

  return QAPI_OK;
}

qapi_Status_t observe_cancel_ext_handle_req(qapi_Net_LWM2M_App_Handler_t handler,
                                            qapi_Net_LWM2M_Server_Data_t op_data)
{
  qapi_Status_t result = QAPI_ERROR;
  qapi_Net_LWM2M_Observed_t *obs = NULL;
  qapi_Net_LWM2M_Observed_t *observed = NULL;
  qapi_Net_LWM2M_Observe_info_t *target = NULL;
  qapi_Net_LWM2M_Uri_t uri;

  if(handler == NULL)
  {
    LOG_INFO("fun [%s] file [%s]  Passed  Argument is NULL\n",__func__,__FILE__);
    return QAPI_ERROR;
  }

  if(handler == ext_object_handler_app1)
  {
    obs = ext_observed_list;
  }

  else if(handler == ext_object_handler_app2)
  {
    obs = ext_multi_observed_list;
  }

  memset(&uri, 0x00, sizeof(qapi_Net_LWM2M_Uri_t));

  // Handling cancel observe with OBSERVE flag set to 1
  if((op_data.obj_info.obj_mask & QAPI_NET_LWM2M_OBJECT_ID_E) != 0)
    {
	  uri.flag           = op_data.obj_info.obj_mask;
	  uri.objectId       = op_data.obj_info.obj_id;
	  uri.instanceId     = op_data.obj_info.obj_inst_id;
	  uri.resourceId     = op_data.obj_info.res_id;
	  uri.resourceInstId = op_data.obj_info.res_inst_id;

	  observed = lwm2m_find_observed(handler, &uri);
    if(observed != NULL)
    {
      qapi_Net_LWM2M_Observe_info_t * parent = NULL;
      parent = observed->observe_info;
      while (parent != NULL && (strncmp((char *)(parent->msg_id + parent->msg_id_len - 2),
                                       (char *)(op_data.msg_id + op_data.msg_id_len -2), 2) != 0))
	  {
        parent = parent->next;
	  }

      if(parent != NULL)
      {
        target = parent;
        lwm2m_unlink_observe_info(handler, observed, parent);
      }
      if(target)
      {
        tx_byte_release(target);
        target = NULL;
        if (observed->observe_info == NULL)
        {
          LOG_INFO("LWM2M_LOG: Removing the Observe request  from the watcher list ,\n");
          lwm2m_unlink_observed(handler, observed);
          LOG_INFO("LWM2M_LOG: object id %d\n",observed->uri.objectId)
  
          if(observed->uri.flag & QAPI_NET_LWM2M_INSTANCE_ID_E)
          {
            LOG_INFO("LWM2M_LOG: instance id %d\n",observed->uri.instanceId) 
            if(observed->uri.flag & QAPI_NET_LWM2M_RESOURCE_ID_E)
            {
              LOG_INFO("LWM2M_LOG: resource id %d\n",observed->uri.resourceId);    
              if(observed->uri.flag & QAPI_NET_LWM2M_RESOURCE_INSTANCE_ID_E)
              {
                LOG_INFO("LWM2M_LOG: resource instance id %d\n",observed->uri.resourceInstId);
              }
            }
          }
          tx_byte_release(observed);
          observed = NULL;
        }
        return read_ext_obj(handler, op_data);
      }
      }
    }

  // Handling cancel observe came through RESET message
  else if((op_data.obj_info.obj_mask & QAPI_NET_LWM2M_OBJECT_ID_E) == 0)
  {
    for(observed = obs; observed != NULL; observed = observed->next)
    {
      qapi_Net_LWM2M_Observe_info_t * parent = NULL;
      parent = observed->observe_info;
      while (parent != NULL && (parent->not_id != op_data.notification_id ))
      {
        parent = parent->next;
      }
      if(parent != NULL)
      {
        target = parent;
        lwm2m_unlink_observe_info(handler, observed, parent);
      }
    if(target != NULL)
    {
      tx_byte_release(target);
      target = NULL;
      if (observed->observe_info == NULL)
      {
        LOG_INFO("LWM2M_LOG: Removing the Observe request  from the watcher list ,\n");
        lwm2m_unlink_observed(handler, observed);
        LOG_INFO("LWM2M_LOG: object id %d\n",observed->uri.objectId)
    
        if(observed->uri.flag & QAPI_NET_LWM2M_INSTANCE_ID_E)
        {
          LOG_INFO("LWM2M_LOG: instance id %d\n",observed->uri.instanceId) 
          if(observed->uri.flag & QAPI_NET_LWM2M_RESOURCE_ID_E)
          {
            LOG_INFO("LWM2M_LOG: resource id %d\n",observed->uri.resourceId);    
            if(observed->uri.flag & QAPI_NET_LWM2M_RESOURCE_INSTANCE_ID_E)
            {
              LOG_INFO("LWM2M_LOG: resource instance id %d\n",observed->uri.resourceInstId);
            }
          }
        } 
        tx_byte_release(observed);
        observed = NULL;
      }
        return QAPI_OK;
	  }
    }
  }
 

  return result;
}

void observe_delete_ext_handle_req(qapi_Net_LWM2M_App_Handler_t handler,
                                   qapi_Net_LWM2M_Uri_t *uri)
{
  qapi_Net_LWM2M_Observed_t *obs = NULL;
  qapi_Net_LWM2M_Observed_t *observed = NULL;

  if(handler == NULL || uri == NULL)
  {
    LOG_INFO("fun [%s] file [%s]  Passed  Argument is NULL\n",__func__,__FILE__);
    return;
  }

  if(handler == ext_object_handler_app1)
  {
    obs = ext_observed_list;
  }

  else if(handler == ext_object_handler_app2)
  {
    obs = ext_multi_observed_list;
  }

  observed = obs;

  while(observed != NULL)
  {
    if(observed->uri.objectId == uri->objectId
       &&(((uri->flag & QAPI_NET_LWM2M_INSTANCE_ID_E) == 0)
       || observed->uri.instanceId == uri->instanceId))
    {
      qapi_Net_LWM2M_Observed_t *next;
      qapi_Net_LWM2M_Observe_info_t *target;

      next = observed->next;

      for(target = observed->observe_info ; target != NULL; target = target->next)
      {
        if(target->attributes != NULL)
        {
          tx_byte_release(target->attributes);
          target->attributes= NULL;
        }
      }
      while(observed->observe_info != NULL)
      {
        qapi_Net_LWM2M_Observe_info_t *next;

        next = observed->observe_info->next;
        tx_byte_release(observed->observe_info);
        observed->observe_info = next;
      }

      lwm2m_unlink_observed(handler, observed);
      LOG_INFO("LWM2M_LOG: Removed observedP  %p from list\n",observed);
      tx_byte_release(observed);
      observed = NULL;

      observed = next;
    }
    else
    {
      observed = observed->next;
    }
  }
}

qapi_Status_t observe_cancel_ext_obj(qapi_Net_LWM2M_App_Handler_t handler, 
                                     qapi_Net_LWM2M_Server_Data_t op_data)
{
  qapi_Status_t result = QAPI_ERROR;

  if(handler == NULL)
  {
    LOG_INFO("Application handle is not valid for current application");
    //send_response_message(handler, op_data, QAPI_NET_LWM2M_400_BAD_REQUEST_E);
    return QAPI_ERR_INVALID_PARAM;
  }

  result = observe_cancel_ext_handle_req(handler, op_data);

  return result;
}

qapi_Status_t observe_ext_obj(qapi_Net_LWM2M_App_Handler_t handler, 
                              qapi_Net_LWM2M_Server_Data_t op_data)
{
  qapi_Status_t result = QAPI_ERROR;
  qapi_Net_LWM2M_Obj_Info_t  obj_info;
  qapi_Net_LWM2M_Data_t *object = NULL;
  qapi_Net_LWM2M_Instance_Info_t *instance = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource = NULL;
  qapi_Net_LWM2M_App_Ex_Obj_Data_t *obs_data = NULL;
  qapi_Net_LWM2M_Flat_Data_t *data = NULL;
  uint8_t *payload = NULL;
  uint32_t payload_length = 0;
  int i = 0;
  int size = 0;
  qapi_Net_LWM2M_Content_Type_t cont_type;

  if(handler == NULL)
  {
    LOG_INFO("Application handle is not valid for current application");
    //send_response_message(op_data, QAPI_NET_LWM2M_400_BAD_REQUEST_E);
    return QAPI_ERR_INVALID_PARAM;
  }

  obj_info = op_data.obj_info;

  if((obj_info.obj_mask & QAPI_NET_LWM2M_OBJECT_ID_E))
  {
    if(handler == ext_object_handler_app1)
    {
      object = lwm2m_object_find(object_generic, obj_info.obj_id);
    }
    else if(handler == ext_object_handler_app2)
    {
      object = lwm2m_object_find(object_presence, obj_info.obj_id);
    }

    if(object != NULL)
    {
      if((obj_info.obj_mask & QAPI_NET_LWM2M_INSTANCE_ID_E))
      {
        instance = lwm2m_instance_find(object->instance_info, obj_info.obj_inst_id);
        if(instance == NULL)
        {
          LOG_INFO("Instance id not found.");
          result = QAPI_ERR_NO_ENTRY;
          send_response_message(handler, op_data, QAPI_NET_LWM2M_404_NOT_FOUND_E);
          goto end;
        }
        else
        {
          if((obj_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_ID_E))
          {
            resource = lwm2m_resource_find(instance->resource_info, obj_info.res_id);
            if(resource == NULL)
            {
              LOG_INFO("Resource id not found.");
              result = QAPI_ERR_NO_ENTRY;
              send_response_message(handler, op_data, QAPI_NET_LWM2M_404_NOT_FOUND_E);
              goto end;
            }
            size = 1;
            data = lwm2m_data_new(size);
            if(data == NULL)
            {
              result = QAPI_ERR_NO_MEMORY;
              send_response_message(handler, op_data, QAPI_NET_LWM2M_500_INTERNAL_SERVER_E);
              goto end;
            }
            (data)->id = resource->resource_ID;
          }
          if((obj_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_INSTANCE_ID_E))
          {
            result = read_data_ext(instance->instance_ID, &size, &data, object, obj_info.res_inst_id);
          }
          else
          {
            result = read_data_ext(instance->instance_ID, &size, &data, object, INVALID_RES_INST_ID);
          }
        }
      }
      else
      {
        for(instance = object->instance_info; instance != NULL; instance = instance->next)
        {
          size++;
        }
        data = lwm2m_data_new(size);
        if(data == NULL)
        {
          result = QAPI_ERR_NO_MEMORY;
          send_response_message(handler, op_data, QAPI_NET_LWM2M_500_INTERNAL_SERVER_E);
          goto end;
        }
        instance = object->instance_info;
        i =0;
        result = QAPI_OK;
        while(instance != NULL && result == QAPI_OK)
        {
          result = read_data_ext(instance->instance_ID, (int *)&((data)[i].value.asChildren.count),
                                 &((data)[i].value.asChildren.array), object, INVALID_RES_INST_ID);
          (data)[i].id = instance->instance_ID;
          (data)[i].type = QAPI_NET_LWM2M_TYPE_OBJECT_INSTANCE;
          i++;
          instance = instance->next;
        }
      }
    }
    else
    {
      LOG_INFO("object id not found.");
      result = QAPI_ERR_NO_ENTRY;
      send_response_message(handler, op_data, QAPI_NET_LWM2M_404_NOT_FOUND_E);
      goto end;
    }
  }
  else
  {
    LOG_INFO("Not valid request.");
    result = QAPI_ERR_INVALID_PARAM;
    send_response_message(handler, op_data, QAPI_NET_LWM2M_400_BAD_REQUEST_E);
    goto end;
  }

  if(result == QAPI_OK)
  {
    result = observe_ext_handle_req(handler, object, op_data, size, data);
  }
  else if(result == QAPI_ERR_NO_ENTRY)
  {
    send_response_message(handler, op_data, QAPI_NET_LWM2M_404_NOT_FOUND_E);
    goto end;
  }
  else 
  {
    send_response_message(handler, op_data, QAPI_NET_LWM2M_400_BAD_REQUEST_E);
    goto end;
  }

/*  if((obj_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_ID_E))
  {
    cont_type = QAPI_NET_LWM2M_TEXT_PLAIN;
  }
  else
  {
    cont_type = QAPI_NET_LWM2M_M2M_TLV;
  }
*/
  if(op_data.accept_is_valid)
  {
    cont_type = op_data.accept;
  }
  else
  {
    cont_type = op_data.content_type;
  }

  if(result == QAPI_OK)
  {
    result = qapi_Net_LWM2M_Encode_Data(handler,
                                        &op_data.obj_info,
                                               data,
                                        (size_t)size,
                                        NULL,
                                        cont_type,
                                        &payload,
                                        &payload_length);
     if (result != QAPI_OK || payload_length == 0)
     {
       if(cont_type == QAPI_NET_LWM2M_APPLICATION_OCTET_STREAM)
       {
         result = QAPI_ERR_BAD_PAYLOAD;
         send_response_message(handler, op_data, QAPI_NET_LWM2M_406_NOT_ACCEPTABLE_E);
         goto end;
       }
       else if (op_data.content_type != QAPI_NET_LWM2M_TEXT_PLAIN || size != 1
           || (data)->type != QAPI_NET_LWM2M_TYPE_STRING_E || (data)->value.asBuffer.length != 0)
       {
         result = QAPI_ERR_BAD_PAYLOAD;
         send_response_message(handler, op_data, QAPI_NET_LWM2M_400_BAD_REQUEST_E);
         goto end;
       }
     }
  }

  if(op_data.accept_is_valid && (op_data.accept == QAPI_NET_LWM2M_TEXT_PLAIN || op_data.accept == QAPI_NET_LWM2M_APPLICATION_OCTET_STREAM))
  {
    if((size != 1
       || !(op_data.obj_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_ID_E)
       || data->type == QAPI_NET_LWM2M_TYPE_OBJECT
       || data->type == QAPI_NET_LWM2M_TYPE_OBJECT_INSTANCE
       || data->type == QAPI_NET_LWM2M_TYPE_MULTIPLE_RESOURCE))
  {
      result = QAPI_ERR_BAD_PAYLOAD;
      send_response_message(handler, op_data, QAPI_NET_LWM2M_406_NOT_ACCEPTABLE_E);
      goto end;
    }
  }

  if(payload == NULL || payload_length == 0)
  {
    send_response_message(handler, op_data, QAPI_NET_LWM2M_404_NOT_FOUND_E);
    return QAPI_ERR_BAD_PAYLOAD;
  }
  tx_byte_allocate(byte_pool_qcli, (VOID **)&obs_data, 
                   sizeof(qapi_Net_LWM2M_App_Ex_Obj_Data_t), TX_NO_WAIT);
  if(obs_data == NULL)
  {
    LOG_INFO("Memory allocation failure.\n");
    send_response_message(handler, op_data, QAPI_NET_LWM2M_500_INTERNAL_SERVER_E);
    return QAPI_ERR_NO_MEMORY;
  }
  memset(obs_data, 0x00, sizeof(qapi_Net_LWM2M_App_Ex_Obj_Data_t));

  obs_data->payload_len = payload_length;
  tx_byte_allocate(byte_pool_qcli, (VOID **)&(obs_data->payload), 
                   payload_length, TX_NO_WAIT);
  if(obs_data->payload == NULL)
  {
    LOG_INFO("Error in memory allocation");
    send_response_message(handler, op_data, QAPI_NET_LWM2M_500_INTERNAL_SERVER_E);
    result = QAPI_ERR_NO_MEMORY;
    goto end;
  }
  if((cont_type== QAPI_NET_LWM2M_TEXT_PLAIN || cont_type == QAPI_NET_LWM2M_APPLICATION_OCTET_STREAM))
  {
    if((size != 1
       || !(op_data.obj_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_ID_E)
       || data->type == QAPI_NET_LWM2M_TYPE_OBJECT
       || data->type == QAPI_NET_LWM2M_TYPE_OBJECT_INSTANCE
       || data->type == QAPI_NET_LWM2M_TYPE_MULTIPLE_RESOURCE))
    {
      cont_type = QAPI_NET_LWM2M_M2M_TLV;
    }
  }

  memcpy(obs_data->payload, payload, payload_length);
  obs_data->msg_type    = QAPI_NET_LWM2M_NOTIFY_MSG_E;
  obs_data->obj_info    = obj_info;
  obs_data->status_code = QAPI_NET_LWM2M_205_CONTENT_E;
  obs_data->conf_msg    = 0;
  obs_data->msg_id_len  = op_data.msg_id_len;
  memcpy(obs_data->msg_id, op_data.msg_id, op_data.msg_id_len);
  obs_data->content_type = cont_type;
  obs_data->observation_seq_num = 0;
  
  result = qapi_Net_LWM2M_Send_Message(handler, obs_data);

  if(obs_data)
  {
    if(obs_data->payload)
    {
      tx_byte_release(obs_data->payload);
      obs_data->payload = NULL;
    }
    tx_byte_release(obs_data);
    obs_data = NULL;
  }

end:
  if(data)
  {
    lwm2m_data_free(size, data);
  }

  if(payload)
  {
    tx_byte_release(payload);
    payload = NULL;
  }
  return result;

}

qapi_Status_t ext_object_write_attr(qapi_Net_LWM2M_App_Handler_t handler, 
                                    qapi_Net_LWM2M_Server_Data_t op_data)
{
  uint8_t result = QAPI_NET_LWM2M_IGNORE_E;

  if(handler == NULL )
  {
    LOG_INFO("Application handle is not valid for current application");
    //send_response_message(op_data, QAPI_NET_LWM2M_400_BAD_REQUEST_E);
    return QAPI_ERR_INVALID_PARAM;
  }

  result = ext_object_set_observe_param(handler, op_data);

  if(result == QAPI_NET_LWM2M_204_CHANGED_E)
  {
    return QAPI_OK;
  }
  else
  {
    return QAPI_ERROR;
  }

}

uint8_t ext_object_set_observe_param(qapi_Net_LWM2M_App_Handler_t handler,
                                     qapi_Net_LWM2M_Server_Data_t op_data)
{
  uint8_t result = QAPI_NET_LWM2M_IGNORE_E;
  qapi_Net_LWM2M_Observe_info_t *observe_info = NULL;
  qapi_Net_LWM2M_Uri_t uri;
  uint8_t msg_id[QAPI_MAX_LWM2M_MSG_ID_LENGTH];
  uint8_t msg_id_len;
  qapi_Net_LWM2M_Attributes_t attributes;

  memset(&uri, 0x00, sizeof(qapi_Net_LWM2M_Uri_t));
  memset(&attributes, 0x00, sizeof(qapi_Net_LWM2M_Attributes_t));

  if(op_data.lwm2m_attr == NULL) 
  {
    LOG_INFO("fun [%s] file [%s]  Passed  Argument is NULL\n",__func__,__FILE__);
    send_response_message(handler, op_data, QAPI_NET_LWM2M_400_BAD_REQUEST_E);
    return QAPI_NET_LWM2M_400_BAD_REQUEST_E;
  }

  memcpy(&attributes, op_data.lwm2m_attr, sizeof(qapi_Net_LWM2M_Attributes_t));
  uri.flag = op_data.obj_info.obj_mask;
  uri.objectId = op_data.obj_info.obj_id;
  uri.instanceId = op_data.obj_info.obj_inst_id;
  uri.resourceId = op_data.obj_info.res_id;
  uri.resourceInstId = op_data.obj_info.res_inst_id;

  msg_id_len = op_data.msg_id_len;
  memcpy(msg_id, op_data.msg_id, msg_id_len);

  if(!(uri.flag & QAPI_NET_LWM2M_INSTANCE_ID_E) && (uri.flag & QAPI_NET_LWM2M_RESOURCE_ID_E))
  {
    send_response_message(handler, op_data, QAPI_NET_LWM2M_400_BAD_REQUEST_E);
    return QAPI_NET_LWM2M_400_BAD_REQUEST_E;
  }

  if(handler == ext_object_handler_app1)
  {
    result = object_check_readable(object_generic, &uri);
  }

  else if(handler == ext_object_handler_app2)
  {
    result = object_check_readable(object_presence, &uri);
  }

  if (result != QAPI_NET_LWM2M_205_CONTENT_E)
  {
    send_response_message(handler, op_data, (qapi_Net_LWM2M_Response_Code_t)result);
    return (uint8_t)result;
  }

  if((attributes.set_attr_mask & LWM2M_ATTR_FLAG_NUMERIC) != 0)
  {
    if(handler == ext_object_handler_app1)
    {
      result = object_check_numeric(object_generic, &uri);
    }
    else if(handler == ext_object_handler_app2)
    {
      result = object_check_numeric(object_presence, &uri);
    }

    if (result != QAPI_NET_LWM2M_205_CONTENT_E)
    {
      send_response_message(handler, op_data, (qapi_Net_LWM2M_Response_Code_t)result);
      return(uint8_t) result;
    }
  }

  observe_info = lwm2m_get_observe_info(handler, &uri, msg_id, msg_id_len);
  
  if(observe_info == NULL)
  {
    send_response_message(handler, op_data, QAPI_NET_LWM2M_500_INTERNAL_SERVER_E);
    return (uint8_t) QAPI_NET_LWM2M_500_INTERNAL_SERVER_E; 
  }
  
  // Check rule lt value + 2*step� value < gt value
  if ((((attributes.set_attr_mask | (observe_info->attributes?observe_info->attributes->set_attr_mask:0))
          & ~(attributes.clr_attr_mask)) & LWM2M_ATTR_FLAG_NUMERIC) == LWM2M_ATTR_FLAG_NUMERIC)
  {
    float gt = 0.0;
    float lt = 0.0;
    float stp = 0.0;

    if ((attributes.set_attr_mask & QAPI_NET_LWM2M_GREATER_THAN_E) != 0)
    {
      gt = attributes.greaterThan;
    }
    else if (observe_info->attributes != NULL)
    {
      gt = observe_info->attributes->greaterThan;
    }
    else
    {
      //Do nothing
    }

    if ((attributes.set_attr_mask & QAPI_NET_LWM2M_LESS_THAN_E) != 0)
    {
      lt = attributes.lessThan;
    }
    else if (observe_info->attributes != NULL)
    {
      lt = observe_info->attributes->lessThan;
    }
    else
    {
      //Do nothing
    }

     if ((attributes.set_attr_mask & QAPI_NET_LWM2M_STEP_E) != 0)
    {
      stp = attributes.step;
    }
    else if (observe_info->attributes != NULL)
    {
      stp = observe_info->attributes->step;
    }
    else
    {
      //Do nothing
    }

    if (lt + (2 * stp) >= gt) 
    {
      send_response_message(handler, op_data, QAPI_NET_LWM2M_400_BAD_REQUEST_E);
      return (uint8_t)QAPI_NET_LWM2M_400_BAD_REQUEST_E;
    }
  }

  if(observe_info->attributes == NULL)
  {
    if(attributes.set_attr_mask != 0)
    {
      tx_byte_allocate(byte_pool_qcli, (VOID **)&(observe_info->attributes), 
                   sizeof(qapi_Net_LWM2M_Attributes_t), TX_NO_WAIT);

      if(observe_info->attributes == NULL)
      {
        send_response_message(handler, op_data, QAPI_NET_LWM2M_500_INTERNAL_SERVER_E);
        return (uint8_t)QAPI_NET_LWM2M_500_INTERNAL_SERVER_E;
      }
      memset(observe_info->attributes, 0x00, sizeof(qapi_Net_LWM2M_Attributes_t));
      memcpy(observe_info->attributes, &attributes, sizeof(qapi_Net_LWM2M_Attributes_t));
    }
  }
  else
  {
    observe_info->attributes->set_attr_mask &= ~(attributes.clr_attr_mask);
    observe_info->attributes->set_attr_mask |= attributes.set_attr_mask;
    
    if(attributes.set_attr_mask & QAPI_NET_LWM2M_MIN_PERIOD_E)
    {
      observe_info->attributes->minPeriod = attributes.minPeriod;
    }
    if(attributes.set_attr_mask & QAPI_NET_LWM2M_MAX_PERIOD_E)
    {
      observe_info->attributes->maxPeriod = attributes.maxPeriod;
    }
    if(attributes.set_attr_mask & QAPI_NET_LWM2M_LESS_THAN_E)
    {
      observe_info->attributes->lessThan = attributes.lessThan;
    }
    if(attributes.set_attr_mask & QAPI_NET_LWM2M_GREATER_THAN_E)
    {
      observe_info->attributes->greaterThan = attributes.greaterThan;
    }
    if(attributes.set_attr_mask & QAPI_NET_LWM2M_STEP_E)
    {
      observe_info->attributes->step = attributes.step;
    }
    if(attributes.clr_attr_mask & QAPI_NET_LWM2M_MIN_PERIOD_E)
    {
      observe_info->attributes->minPeriod = 0;
    }
    if(attributes.clr_attr_mask & QAPI_NET_LWM2M_MAX_PERIOD_E)
    {
      observe_info->attributes->maxPeriod = 0;
    }
    if(attributes.clr_attr_mask & QAPI_NET_LWM2M_LESS_THAN_E)
    {
      observe_info->attributes->lessThan = 0;
    }
    if(attributes.clr_attr_mask & QAPI_NET_LWM2M_GREATER_THAN_E)
    {
      observe_info->attributes->greaterThan = 0;
    }
    if(attributes.clr_attr_mask & QAPI_NET_LWM2M_STEP_E)
    {
      observe_info->attributes->step = 0;
    }
  }

  send_response_message(handler, op_data, QAPI_NET_LWM2M_204_CHANGED_E);
  return QAPI_NET_LWM2M_204_CHANGED_E;
}

qapi_Net_LWM2M_Observed_t* observe_find_by_uri(qapi_Net_LWM2M_App_Handler_t handler,
                                               qapi_Net_LWM2M_Uri_t *uri)
{
  qapi_Net_LWM2M_Observed_t *target = NULL;

  if(handler == NULL || uri == NULL) 
  { 
    LOG_INFO("fun [%s] file [%s]  Passed  Argument is NULL\n",__func__,__FILE__); 
    return NULL; 
  }

  if(handler == ext_object_handler_app1 && ext_observed_list != NULL)
  {
    target = ext_observed_list;
  }
  else if(handler == ext_object_handler_app2 && ext_multi_observed_list != NULL)
  {
    target = ext_multi_observed_list;
  }

  while(target != NULL)
  {
    if(target->uri.objectId == uri->objectId)
    {
      if((!(uri->flag & QAPI_NET_LWM2M_INSTANCE_ID_E) && !(target->uri.flag & QAPI_NET_LWM2M_INSTANCE_ID_E))
          ||((uri->flag & QAPI_NET_LWM2M_INSTANCE_ID_E) && (target->uri.flag & QAPI_NET_LWM2M_INSTANCE_ID_E) 
          && (uri->instanceId == target->uri.instanceId)))
      {
        if((!(uri->flag & QAPI_NET_LWM2M_RESOURCE_ID_E) && !(target->uri.flag & QAPI_NET_LWM2M_RESOURCE_ID_E))
          ||((uri->flag & QAPI_NET_LWM2M_RESOURCE_ID_E) && (target->uri.flag & QAPI_NET_LWM2M_RESOURCE_ID_E) 
          && (uri->resourceId == target->uri.resourceId)))
        {
          if(target->observe_info)
          {
            LOG_INFO("Found one with observers.");
          }
		  else
		  {
            LOG_INFO("Found one with no observers ");
		  }
          return target;
        }
      }
    }
    target = target->next;
  }

  LOG_INFO("Found nothing");
  return NULL;
}

void ext_obj_resource_value_changed(qapi_Net_LWM2M_App_Handler_t handler,
                                    qapi_Net_LWM2M_Uri_t *uri)
{
  qapi_Net_LWM2M_Observed_t * target = NULL;

  if(handler == NULL || uri == NULL) 
  { 
    LOG_INFO("fun [%s] file [%s]  Passed  Argument is NULL\n",__func__,__FILE__); 
    return;
  }

  if(handler == ext_object_handler_app1 && ext_observed_list != NULL)
  {
    target = ext_observed_list;
  }
  else if(handler == ext_object_handler_app2 && ext_multi_observed_list != NULL)
  {
    target = ext_multi_observed_list;
  }

  while(target != NULL)
  {
    if (target->uri.objectId == uri->objectId)
    {
      if(!(uri->flag & QAPI_NET_LWM2M_INSTANCE_ID_E)
          ||(target->uri.flag & QAPI_NET_LWM2M_INSTANCE_ID_E) == 0
          || uri->instanceId == target->uri.instanceId)
      {
        if(!(uri->flag & QAPI_NET_LWM2M_RESOURCE_ID_E)
          ||(target->uri.flag & QAPI_NET_LWM2M_RESOURCE_ID_E) == 0
          || uri->resourceId == target->uri.resourceId)
        {
          qapi_Net_LWM2M_Observe_info_t *observe_info;

          LOG_INFO("Found an observation");

          for (observe_info = target->observe_info; observe_info != NULL ; observe_info = observe_info->next)
          {
            if (observe_info->active == true)
            {
              LOG_INFO("Tagging a watcher");
              observe_info->update = true;
            }
          }
        }
      }
    }
    target = target->next;
  }
}

qapi_Status_t ext_observer_add_notify_to_pending_list(qapi_Net_LWM2M_App_Handler_t handler,
                                                      qapi_Net_LWM2M_Observe_info_t *observe_info,
                                                      qapi_Net_LWM2M_Uri_t uri,
                                                      qapi_Net_LWM2M_Content_Type_t format,
                                                      uint8_t *buffer, size_t length)
{
  qapi_Net_LWM2M_Pending_Observed_t *pending_node = NULL;
  qapi_Net_LWM2M_Pending_Observed_t *notify = NULL;
  qapi_Net_LWM2M_App_Ex_Obj_Data_t *notify_data = NULL;
  qapi_Status_t result = QAPI_ERROR;

  LOG_INFO("Entering ext_observer_add_notify_to_pending_list");
  if(observe_info == NULL || buffer == NULL || handler == NULL)
  {
    LOG_INFO("NULL parameter check failed\n");
    return QAPI_ERROR;
  }

  tx_byte_allocate(byte_pool_qcli, (VOID **)&notify_data, 
                   sizeof(qapi_Net_LWM2M_App_Ex_Obj_Data_t), TX_NO_WAIT);
  if(notify_data == NULL)
  {
    LOG_INFO("Memory allocation failure.\n");
    return QAPI_ERR_NO_MEMORY;
  }
  memset(notify_data, 0x00, sizeof(qapi_Net_LWM2M_App_Ex_Obj_Data_t));

  tx_byte_allocate(byte_pool_qcli, (VOID **)&notify, 
                   sizeof(qapi_Net_LWM2M_Pending_Observed_t), TX_NO_WAIT);
  if(notify == NULL)
  {
    LOG_INFO("LWM2M_LOG: Malloc failed\n");
    result = QAPI_ERR_NO_MEMORY;
    goto end;
  }
  memset(notify, 0x00, sizeof(qapi_Net_LWM2M_Pending_Observed_t));

  notify->observe_info = observe_info;

  notify_data->payload_len = length;
  tx_byte_allocate(byte_pool_qcli, (VOID **)&(notify_data->payload), 
                   length, TX_NO_WAIT);
  if(notify_data->payload == NULL)
  {
    LOG_INFO("Error in memory allocation");
    result = QAPI_ERR_NO_MEMORY;
    goto end;
  }
  memcpy(notify_data->payload, buffer, length);
  if(buffer)
  {
    tx_byte_release(buffer);
  }
  notify_data->msg_type    = QAPI_NET_LWM2M_NOTIFY_MSG_E;
  notify_data->obj_info.obj_mask    = (qapi_Net_LWM2M_ID_t)uri.flag;
  notify_data->obj_info.obj_id      = uri.objectId;
  notify_data->obj_info.obj_inst_id = uri.instanceId;
  notify_data->obj_info.res_id      = uri.resourceId;
  notify_data->obj_info.res_inst_id = uri.resourceInstId;
  notify_data->status_code = QAPI_NET_LWM2M_205_CONTENT_E;
  notify_data->conf_msg    = 0;
  notify_data->msg_id_len  = observe_info->msg_id_len;
  memcpy(notify_data->msg_id, observe_info->msg_id, observe_info->msg_id_len);
  notify_data->content_type = format;
  notify_data->observation_seq_num = observe_info->seq_num;

  notify->message = (void *)notify_data;

  if(handler == ext_object_handler_app1)
  {
    if(ext_pending_observed_list == NULL)
    { 
      notify->next = ext_pending_observed_list;
      ext_pending_observed_list = notify;
    } 
    else 
    {
      for(pending_node = ext_pending_observed_list; (pending_node != NULL && pending_node->next != NULL) ; pending_node = pending_node->next);
      pending_node->next = notify;
    }
  }

  else if(handler == ext_object_handler_app2)
  {
    if(ext_multi_pending_observed_list == NULL)
    { 
      notify->next = ext_multi_pending_observed_list;
      ext_multi_pending_observed_list = notify;
    } 
    else 
    {
      for(pending_node = ext_multi_pending_observed_list; (pending_node != NULL && pending_node->next != NULL) ; pending_node = pending_node->next);
      pending_node->next = notify;
    }
  }

end:

  return result;
}

void ext_object_notify(int id,
                       time_t current_time,
                       time_t *timeout)
{
  qapi_Net_LWM2M_Observed_t *observed = NULL;
  bool notify_store = false;
  qapi_Net_LWM2M_Content_Type_t cont_type;
  qapi_Status_t result = QAPI_OK;
  qapi_Net_LWM2M_Observed_t *observed_list = NULL;
  qapi_Net_LWM2M_App_Handler_t handler = NULL;

  if(id == 0)
  {
    handler = ext_object_handler_app1;
  }
  else if(id == 1)
  {
    handler = ext_object_handler_app2;
  }

  if(handler == NULL || timeout == NULL)
  {
    return;
  }

  if(handler == ext_object_handler_app1 && ext_observed_list == NULL)
  {
    stop_ext_obs_notify_timer(handler);
    return;
  }
  else if(handler == ext_object_handler_app2 && ext_multi_observed_list == NULL)
  {
    stop_ext_obs_notify_timer(handler);
    return;
  }

  if(handler == ext_object_handler_app1)
  {
    observed_list = ext_observed_list;
  }
  else if(handler == ext_object_handler_app2)
  {
    observed_list = ext_multi_observed_list;
  }

  //Send Pending Notifications
  if(!lwm2m_client_in_sleep_state && 
    (((handler == ext_object_handler_app1) && (ext_pending_observed_list != NULL))
    || ((handler == ext_object_handler_app2) && (ext_multi_pending_observed_list != NULL))))
  {
    qapi_Net_LWM2M_Pending_Observed_t *node = NULL;
    qapi_Net_LWM2M_App_Ex_Obj_Data_t *notify_data = NULL;

    if(handler == ext_object_handler_app1)
    {
      node = ext_pending_observed_list;
    }
    else if(handler == ext_object_handler_app2)
    {
      node = ext_multi_pending_observed_list;
    }

    while(node && (node->observe_info))
    {  
      tx_byte_allocate(byte_pool_qcli, (VOID **)&notify_data, 
                   sizeof(qapi_Net_LWM2M_App_Ex_Obj_Data_t), TX_NO_WAIT);
      if(notify_data == NULL)
      {
        LOG_INFO("Memory allocation failure.\n");
        //send_response_message(op_data, QAPI_NET_LWM2M_500_INTERNAL_SERVER_E);
        return;
      }
      memset(notify_data, 0x00, sizeof(qapi_Net_LWM2M_App_Ex_Obj_Data_t));
      //Sending the pedning notifies stored either when in sleep or after disbale timeout elapses  
      LOG_INFO("LWM2M_LOG: Sending pending notify \n");

      if(handler == ext_object_handler_app1)
      {
        ext_pending_observed_list = node->next;
      }
      else if(handler == ext_object_handler_app2)
      {
        ext_multi_pending_observed_list = node->next;
      }

      if(!node->observe_info || !node->message)
      {
        if(node->message) 
        {
          if(((qapi_Net_LWM2M_App_Ex_Obj_Data_t *)node->message)->payload)
          {
            tx_byte_release(((qapi_Net_LWM2M_App_Ex_Obj_Data_t *)node->message)->payload);
                            ((qapi_Net_LWM2M_App_Ex_Obj_Data_t *)node->message)->payload = NULL;
          }
          tx_byte_release(node->message);
          node->message = NULL;
        }
        tx_byte_release(node);
        node = NULL;
        if(handler == ext_object_handler_app1)
        {
          node = ext_pending_observed_list;
        }
        else if(handler == ext_object_handler_app2)
        {
          node = ext_multi_pending_observed_list;
        }
        continue;
      }

      node->observe_info->last_time = current_time;

      notify_data->payload_len = ((qapi_Net_LWM2M_App_Ex_Obj_Data_t *)node->message)->payload_len;
      tx_byte_allocate(byte_pool_qcli, (VOID **)&(notify_data->payload), 
                       notify_data->payload_len, TX_NO_WAIT);
      if(notify_data->payload == NULL)
      {
        LOG_INFO("Error in memory allocation");
        //send_response_message(op_data, QAPI_NET_LWM2M_500_INTERNAL_SERVER_E);
        result = QAPI_ERR_NO_MEMORY;
        goto end;
      }
      memcpy(notify_data->payload, ((qapi_Net_LWM2M_App_Ex_Obj_Data_t *)node->message)->payload, notify_data->payload_len);
      notify_data->msg_type    = QAPI_NET_LWM2M_NOTIFY_MSG_E;
      notify_data->obj_info.obj_mask    = ((qapi_Net_LWM2M_App_Ex_Obj_Data_t *)node->message)->obj_info.obj_mask;
      notify_data->obj_info.obj_id      = ((qapi_Net_LWM2M_App_Ex_Obj_Data_t *)node->message)->obj_info.obj_id;
      notify_data->obj_info.obj_inst_id = ((qapi_Net_LWM2M_App_Ex_Obj_Data_t *)node->message)->obj_info.obj_inst_id;
      notify_data->obj_info.res_id      = ((qapi_Net_LWM2M_App_Ex_Obj_Data_t *)node->message)->obj_info.res_id;
      notify_data->obj_info.res_inst_id = ((qapi_Net_LWM2M_App_Ex_Obj_Data_t *)node->message)->obj_info.res_inst_id;
      notify_data->status_code = QAPI_NET_LWM2M_205_CONTENT_E;
      notify_data->conf_msg    = 0;
      notify_data->msg_id_len  = node->observe_info->msg_id_len;
      memcpy(notify_data->msg_id, node->observe_info->msg_id,  node->observe_info->msg_id_len);
      notify_data->content_type = ((qapi_Net_LWM2M_App_Ex_Obj_Data_t *)node->message)->content_type;
      notify_data->observation_seq_num = node->observe_info->seq_num;

      result = qapi_Net_LWM2M_Send_Message(handler, notify_data);
      node->observe_info->not_id = notify_data->notification_id;
      node->observe_info->update = false;
end:
     if(notify_data)
     {
       if(notify_data->payload)
       {
         tx_byte_release(notify_data->payload);
         notify_data->payload = NULL;
       }
       tx_byte_release(notify_data);
       notify_data = NULL;
     }
     if(((qapi_Net_LWM2M_App_Ex_Obj_Data_t *)node->message)->payload)
     {
       tx_byte_release(((qapi_Net_LWM2M_App_Ex_Obj_Data_t *)node->message)->payload);
                      (((qapi_Net_LWM2M_App_Ex_Obj_Data_t *)node->message)->payload) = NULL;
     }
	 if((qapi_Net_LWM2M_App_Ex_Obj_Data_t *)node->message)
     {
       tx_byte_release(node->message);
       node->message = NULL;
     }
     if(node)
     {
       tx_byte_release(node);
       node = NULL;
     }
     if(handler == ext_object_handler_app1)
     {
       node = ext_pending_observed_list;
     }
     else if(handler == ext_object_handler_app2)
     {
       node = ext_multi_pending_observed_list;
     }

    }
  }

  for(observed = observed_list; observed != NULL; observed = observed->next)
  {
    qapi_Net_LWM2M_Observe_info_t *observe_info = NULL;
    uint8_t * buffer = NULL;
    size_t length = 0;
    qapi_Net_LWM2M_Flat_Data_t * data = NULL;
    int size = 0;
    double float_value = 0;
    int64_t integer_value = 0;
    bool store_value = false;
    qapi_Net_LWM2M_App_Ex_Obj_Data_t *notify_data = NULL;
    int result = 0;
    time_t min_period = 0, max_period = 0;
    qapi_Net_LWM2M_Data_t *object = NULL;
    qapi_Net_LWM2M_Observe_info_t *target = NULL;
    qapi_Net_LWM2M_Uri_t uri;

    memset(&uri, 0x00, sizeof(qapi_Net_LWM2M_Uri_t));

    if(observed->uri.flag & QAPI_NET_LWM2M_RESOURCE_ID_E)
    {
      if(handler == ext_object_handler_app1)
      {
        object = lwm2m_object_find(object_generic, observed->uri.objectId);
      }
      else if(handler == ext_object_handler_app2)
      {
        object = lwm2m_object_find(object_presence, observed->uri.objectId);
      }

      if(object == NULL)
      {
        return;
      }
      // Read resource to value to check value change
      size = 1;
      data = lwm2m_data_new(size);
      if(data == NULL)
      {
        return;
      }
      (data)->id = observed->uri.resourceId;

      if((observed->uri.flag & QAPI_NET_LWM2M_RESOURCE_INSTANCE_ID_E))
      {
        result = read_data_ext(observed->uri.instanceId, &size, &data, object,observed->uri.resourceInstId);
      }
      else
      {
        result = read_data_ext(observed->uri.instanceId, &size, &data, object, INVALID_RES_INST_ID);
      }

      if(result != QAPI_OK)
      {
        if(data != NULL)
        {
          lwm2m_data_free(size, data);
        }
        continue;
      }
      if(data == NULL)
      continue;

      switch (data->type)
      {
        case QAPI_NET_LWM2M_TYPE_INTEGER_E:
          if (lwm2m_data_decode_int(data, &integer_value) != 1) continue;
          store_value = true;
          break;
        case QAPI_NET_LWM2M_TYPE_FLOAT_E:
          if (lwm2m_data_decode_float(data, &float_value) != 1) continue;
          store_value = true;
          break;
        default:
          break;
      }
    }

    for(observe_info = observed->observe_info; observe_info != NULL; observe_info = observe_info->next)
    {
      uint16_t ssid = 0;
	  if(observe_info->msg_id_len >= 2)
	  {
        QAPI_LWM2M_SERVER_ID_INFO(observe_info->msg_id, observe_info->msg_id_len, ssid);
      }
      qapi_Net_LWM2M_Default_Attribute_Info(handler, (uint32_t)ssid, &min_period, &max_period);

      if (observe_info->active == true)
      {
        bool notify = false; //Flag that decides if notify needs to be sent

        // check if the resource value has changed and set watcher->update flag to true to trigger notify
        if(store_value)
        {
          LOG_INFO("store value is true\n");
          switch (data->type)
          {
            case QAPI_NET_LWM2M_TYPE_INTEGER_E:
              if(integer_value != observe_info->lastvalue.asInteger)
              {
                LOG_INFO("Integer value changed\n");
                observe_info->update = true;
              }
              else
              {
                LOG_INFO("Integer value did not change\n");
              }
              break;

            case QAPI_NET_LWM2M_TYPE_FLOAT_E:
              if(float_value != observe_info->lastvalue.asFloat)
              {
                LOG_INFO("Float value changed\n");
                observe_info->update = true;
              }
              else
              {
                LOG_INFO("Float value did not change\n");
              }
              break;
          }
        } //End of value change check

        // Get Pmin and Pmax period and if pmin > pmax, notify on Pmax period
        memset(&uri, 0x00, sizeof(qapi_Net_LWM2M_Uri_t));
        memcpy(&uri, &observed->uri, sizeof(qapi_Net_LWM2M_Uri_t));

        // Get the Maximum period 
        if(observe_info->attributes == NULL || (observe_info->attributes != NULL 
           && (observe_info->attributes->set_attr_mask & QAPI_NET_LWM2M_MAX_PERIOD_E) == 0))
        {
          target = ext_findInheritedWatcher(handler, &uri, observe_info->msg_id, observe_info->msg_id_len, QAPI_NET_LWM2M_MAX_PERIOD_E);
          if(target != NULL)
          {
            if(target->attributes != NULL && (target->attributes->set_attr_mask & QAPI_NET_LWM2M_MAX_PERIOD_E) != 0)
            {
              max_period = target->attributes->maxPeriod;
            }
          }
          else
          {
            if(observe_info->attributes != NULL && (observe_info->attributes->set_attr_mask & QAPI_NET_LWM2M_MAX_PERIOD_E) != 0)
            {
              max_period = observe_info->attributes->maxPeriod;
            }
          }
        }
        else
        {
          if(observe_info->attributes != NULL && (observe_info->attributes->set_attr_mask & QAPI_NET_LWM2M_MAX_PERIOD_E) != 0)
          {
            max_period = observe_info->attributes->maxPeriod;
          }
        }

        // Get the Minimum period 
        if(observe_info->attributes == NULL || (observe_info->attributes != NULL 
           && (observe_info->attributes->set_attr_mask & QAPI_NET_LWM2M_MIN_PERIOD_E) == 0))
        {
          target = ext_findInheritedWatcher(handler, &uri, observe_info->msg_id, observe_info->msg_id_len, QAPI_NET_LWM2M_MIN_PERIOD_E);
          if(target != NULL)
          {
            if(target->attributes != NULL && (target->attributes->set_attr_mask & QAPI_NET_LWM2M_MIN_PERIOD_E) != 0)
            {
              min_period = target->attributes->minPeriod;
            }
          }
          else
          {
            if(observe_info->attributes != NULL && (observe_info->attributes->set_attr_mask & QAPI_NET_LWM2M_MIN_PERIOD_E) != 0)
            {
              min_period = observe_info->attributes->minPeriod;
            }
          }
        }
        else
        {
          if(observe_info->attributes != NULL && (observe_info->attributes->set_attr_mask & QAPI_NET_LWM2M_MIN_PERIOD_E) != 0)
          {
            min_period = observe_info->attributes->minPeriod;
          }
        }

        if(min_period <= max_period)
        {
        // If notify needs to be sent due to resource value change
        if (observe_info->update == true) 
        {
          // value changed, should we notify the server ?
          // If no parameters are set, wait for pMin time
          if (observe_info->attributes == NULL || observe_info->attributes->set_attr_mask == 0)
          {
            // no conditions
            notify = true;
            LOG_INFO("Notify with no conditions");
          }

          // If lt, gt and st parameters are set
          if (notify == false && observe_info->attributes != NULL
              && (observe_info->attributes->set_attr_mask & LWM2M_ATTR_FLAG_NUMERIC) != 0
              && data != NULL)
          {
            // check for less than parameter
            if ((observe_info->attributes->set_attr_mask & QAPI_NET_LWM2M_LESS_THAN_E) != 0)
            {
              LOG_INFO("Checking lower threshold");
              // Did we cross the lower treshold ?
              switch (data->type)
              {
                case QAPI_NET_LWM2M_TYPE_INTEGER_E:
                  if ((integer_value < observe_info->attributes->lessThan
                        && observe_info->lastvalue.asInteger > observe_info->attributes->lessThan)
                      || (integer_value > observe_info->attributes->lessThan
                        && observe_info->lastvalue.asInteger < observe_info->attributes->lessThan))
                  {
                    LOG_INFO("Notify on lower threshold crossing");
                    notify = true;
                  }
                  break;
                case QAPI_NET_LWM2M_TYPE_FLOAT_E:
                  if ((float_value < observe_info->attributes->lessThan
                        && observe_info->lastvalue.asFloat > observe_info->attributes->lessThan)
                      || (float_value > observe_info->attributes->lessThan
                        && observe_info->lastvalue.asFloat < observe_info->attributes->lessThan))
                  {
                    LOG_INFO("Notify on lower threshold crossing");
                    notify = true;
                  }
                  break;
                default:
                  break;
              }
            }
            // check for greater than parameter
            if ((observe_info->attributes->set_attr_mask & QAPI_NET_LWM2M_GREATER_THAN_E) != 0)
            {
              LOG_INFO("Checking upper threshold");
              // Did we cross the upper treshold ?
              switch (data->type)
              {
                case QAPI_NET_LWM2M_TYPE_INTEGER_E:
                  if ((integer_value < observe_info->attributes->greaterThan
                        && observe_info->lastvalue.asInteger > observe_info->attributes->greaterThan)
                      || (integer_value > observe_info->attributes->greaterThan
                        && observe_info->lastvalue.asInteger < observe_info->attributes->greaterThan))
                  {
                    LOG_INFO("Notify on upper threshold crossing");
                    notify = true;
                  }
                  break;
                case QAPI_NET_LWM2M_TYPE_FLOAT_E:
                  if ((float_value < observe_info->attributes->greaterThan
                        && observe_info->lastvalue.asFloat > observe_info->attributes->greaterThan)
                      || (float_value > observe_info->attributes->greaterThan
                        && observe_info->lastvalue.asFloat < observe_info->attributes->greaterThan))
                  {
                    LOG_INFO("Notify on upper threshold crossing");
                    notify = true;
                  }
                  break;
                default:
                  break;
              }
            }
            // check for step parameter
            if ((observe_info->attributes->set_attr_mask & QAPI_NET_LWM2M_STEP_E) != 0)
            {
              LOG_INFO("Checking step");

              switch (data->type)
              {
                case QAPI_NET_LWM2M_TYPE_INTEGER_E:
                  {
                    int64_t diff;

                    diff = integer_value - observe_info->lastvalue.asInteger;
                    if ((diff < 0 && (0 - diff) >= observe_info->attributes->step)
                        || (diff >= 0 && diff >= observe_info->attributes->step))
                    {
                      LOG_INFO("Notify on step condition");
                      notify = true;
                    }
                  }
                  break;
                case QAPI_NET_LWM2M_TYPE_FLOAT_E:
                  {
                    double diff;

                    diff = float_value - observe_info->lastvalue.asFloat;
                    if ((diff < 0 && (0 - diff) >= observe_info->attributes->step)
                        || (diff >= 0 && diff >= observe_info->attributes->step))
                    {
                      LOG_INFO("Notify on step condition");
                      notify = true;
                    }
                  }
                  break;
                default:
                  break;
              }
            }
          }

          // If pmin or pmax parameters are set and vaue attribute is not set
          if (notify == false && observe_info->attributes != NULL
              && ((observe_info->attributes->set_attr_mask & QAPI_NET_LWM2M_MIN_PERIOD_E)
              || (observe_info->attributes->set_attr_mask & QAPI_NET_LWM2M_MAX_PERIOD_E)))
          {
            if((observe_info->attributes->set_attr_mask & LWM2M_ATTR_FLAG_NUMERIC)== 0)
            {
              notify = true;
            }
          }

          memcpy(&uri, &observed->uri, sizeof(qapi_Net_LWM2M_Uri_t));

          // Get the Minimum period 
          if(observe_info->attributes == NULL || (observe_info->attributes != NULL 
              && (observe_info->attributes->set_attr_mask & QAPI_NET_LWM2M_MIN_PERIOD_E) == 0))
          {
            target = ext_findInheritedWatcher(handler, &uri, observe_info->msg_id, observe_info->msg_id_len,
                                                QAPI_NET_LWM2M_MIN_PERIOD_E);
            if(target != NULL)
            {
              if(target->attributes != NULL && (target->attributes->set_attr_mask & QAPI_NET_LWM2M_MIN_PERIOD_E) != 0)
              {
                min_period = target->attributes->minPeriod;
              }
            }
            else
            {
              if(observe_info->attributes != NULL && (observe_info->attributes->set_attr_mask & QAPI_NET_LWM2M_MIN_PERIOD_E) != 0)
              {
                min_period = observe_info->attributes->minPeriod;
              }
            }
          }
          else
          {
            if(observe_info->attributes != NULL && (observe_info->attributes->set_attr_mask & QAPI_NET_LWM2M_MIN_PERIOD_E) != 0)
            {
              min_period = observe_info->attributes->minPeriod;
            }
          }

          if(notify == true)
          {
            if (observe_info->last_time + min_period > current_time)
            {
              // Update the timeout to be aligned to next Pmin time 
              //TIMEOUT_VAL_WITH_PERIOD(observe_info, *timeout, current_time, min_period);
              notify = false;
            }
            else
            {
              LOG_INFO("Notify on minimal period");
              notify = true;
            }
          }
        } // watcher->update = true
        }
        // Is the Maximum Period reached ?
        if(notify == false)
        {
          if(observe_info->last_time + max_period <= current_time)
          {
            LOG_INFO("Notify on maximal period");
            notify = true;
          }
          else
          {
            //TIMEOUT_VAL_WITH_PERIOD(observe_info, *timeout, current_time, max_period);
          } 
        }

        cont_type = observe_info->format;

        if (notify == true)
        {
          tx_byte_allocate(byte_pool_qcli, (VOID **)&notify_data, 
                   sizeof(qapi_Net_LWM2M_App_Ex_Obj_Data_t), TX_NO_WAIT);
          if(notify_data == NULL)
          {
           LOG_INFO("Memory allocation failure.\n");
           //send_response_message(op_data, QAPI_NET_LWM2M_500_INTERNAL_SERVER_E);
           return;
          }
          memset(notify_data, 0x00, sizeof(qapi_Net_LWM2M_App_Ex_Obj_Data_t));
          if (buffer == NULL)
          {
            if (data != NULL)
            {
              result = qapi_Net_LWM2M_Encode_Data(handler,
                                                 (qapi_Net_LWM2M_Obj_Info_t *)&observed->uri,
                                                 data,
                                                 size,
                                                 NULL,
                                                 cont_type,
                                                 &buffer,
                                                 &length);

              if(length == 0)
              {
                observe_info->update = false;
                observe_info->last_time = current_time;
              break;
            }

              if((cont_type == QAPI_NET_LWM2M_TEXT_PLAIN || cont_type == QAPI_NET_LWM2M_APPLICATION_OCTET_STREAM))
              {
                if((size != 1
                   || !(observed->uri.flag & QAPI_NET_LWM2M_RESOURCE_ID_E)
                   || data->type == QAPI_NET_LWM2M_TYPE_OBJECT
                   || data->type == QAPI_NET_LWM2M_TYPE_OBJECT_INSTANCE
                   || data->type == QAPI_NET_LWM2M_TYPE_MULTIPLE_RESOURCE))
                {
                  observe_info->update = false;
                  observe_info->last_time = current_time;
              break;
                }
              }
            }
            else
            {
              if(ext_object_read(handler, observed->uri, cont_type, &buffer, &length) != QAPI_OK)
              {
                buffer = NULL;
                observe_info->update = false;
                observe_info->last_time = current_time;
                break;
              }
            }

            notify_data->payload_len = length;
            tx_byte_allocate(byte_pool_qcli, (VOID **)&(notify_data->payload), 
                             length, TX_NO_WAIT);
            if(notify_data->payload == NULL)
            {
              LOG_INFO("Error in memory allocation");
              result = QAPI_ERR_NO_MEMORY;
              tx_byte_release(notify_data);
              notify_data = NULL;
              return;
            }
            memcpy(notify_data->payload, buffer, length);
            notify_data->msg_type    = QAPI_NET_LWM2M_NOTIFY_MSG_E;
            notify_data->obj_info.obj_mask    = (qapi_Net_LWM2M_ID_t)observed->uri.flag;
            notify_data->obj_info.obj_id      = observed->uri.objectId;
            notify_data->obj_info.obj_inst_id = observed->uri.instanceId;
            notify_data->obj_info.res_id      = observed->uri.resourceId;
            notify_data->obj_info.res_inst_id = observed->uri.resourceInstId;
            notify_data->status_code = QAPI_NET_LWM2M_205_CONTENT_E;
            notify_data->conf_msg    = 0;
            notify_data->msg_id_len  = observe_info->msg_id_len;
            memcpy(notify_data->msg_id, observe_info->msg_id, observe_info->msg_id_len);
            notify_data->content_type = cont_type;
            notify_data->observation_seq_num = ++observe_info->seq_num;
          }


          // check if the device is in sleep state
          if(lwm2m_client_in_sleep_state)
          {
              // If the observation request is for resource level update the last value.
              if(store_value)
              {
                LOG_INFO("store value is true\n");
                switch (data->type)
                {
                  case QAPI_NET_LWM2M_TYPE_INTEGER_E:
                    if(integer_value != observe_info->lastvalue.asInteger)
                    {
                      LOG_INFO("Integer value changed\n");
                      observe_info->lastvalue.asInteger = integer_value;
                    }
                    else
                    {
                      LOG_INFO("Integer value did not change\n");
                    }
                    break;
      
                  case QAPI_NET_LWM2M_TYPE_FLOAT_E:
                    if(float_value != observe_info->lastvalue.asFloat)
                    {
                      LOG_INFO("Float value changed\n");
                      observe_info->lastvalue.asFloat = float_value;
                    }
                    else
                    {
                      LOG_INFO("Float value did not change\n");
                    }
                    break;
                }
              }
              //Add into notify pending list
              LOG_INFO("Adding notifications into pending list for sleep state\n");
              ext_observer_add_notify_to_pending_list(handler, observe_info, observed->uri, cont_type, buffer, length);
              qapi_Net_LWM2M_Wakeup(handler ,observe_info->msg_id,observe_info->msg_id_len);

              observe_info->last_time = current_time; //To avoid observe_step from being called immediately

              if(notify_data)
              {
                if(notify_data->payload)
                {
                  tx_byte_release(notify_data->payload);
                  notify_data->payload = NULL;
                }
                tx_byte_release(notify_data);
                notify_data = NULL;
              }
              notify_store = true;
          } 
          else
          {
            // Send notifications

              observe_info->last_time = current_time;

              result = qapi_Net_LWM2M_Send_Message(handler, notify_data);
              observe_info->not_id = notify_data->notification_id;

              observe_info->update = false;

              if ((notify == true) && (store_value == true))
              {
                switch (data->type)
                {
                  case QAPI_NET_LWM2M_TYPE_INTEGER_E:
                    observe_info->lastvalue.asInteger = integer_value;
                    break;
                  case QAPI_NET_LWM2M_TYPE_FLOAT_E:
                    observe_info->lastvalue.asFloat = float_value;
                    break;
                  default:
                    break;
                }
              }
          }
          if(notify_data)
          {
            if(notify_data->payload)
            {
              tx_byte_release(notify_data->payload);
              notify_data->payload = NULL;
            }
            tx_byte_release(notify_data);
            notify_data = NULL;
          }
          if(buffer != NULL && !lwm2m_client_in_sleep_state && !notify_store) 
          {
            tx_byte_release(buffer);
            buffer = NULL;
          }
        } // notify == true

      }
    }
    if(data != NULL)
    {
      lwm2m_data_free(size, data);
    }
  }
  LOG_INFO("result is %d",result);
}

void send_response_message(qapi_Net_LWM2M_App_Handler_t handler,
                           qapi_Net_LWM2M_Server_Data_t op_data,
                           qapi_Net_LWM2M_Response_Code_t status)
{
  qapi_Net_LWM2M_App_Ex_Obj_Data_t app_data;

  if (!op_data.msg_id_len)
    return;

  memset(&app_data, 0x00, sizeof(qapi_Net_LWM2M_App_Ex_Obj_Data_t));

  app_data.msg_type = QAPI_NET_LWM2M_RESPONSE_MSG_E;
  app_data.obj_info = op_data.obj_info;
  app_data.status_code = status;
  app_data.conf_msg = 0;
  app_data.msg_id_len = op_data.msg_id_len;
  memcpy(app_data.msg_id, op_data.msg_id, op_data.msg_id_len);

  qapi_Net_LWM2M_Send_Message(handler, &app_data);
}

void display_object_template()
{
  QCLI_Printf(lwm2m_ext_handle,"Object id is %d \n", LWM2M_GENERIC_SENSOR_OBJECT_ID);
  QCLI_Printf(lwm2m_ext_handle,"Object is multi-instance \n");
  QCLI_Printf(lwm2m_ext_handle,"Res_id   Res_name                   Res_instances  Mandatory");
  QCLI_Printf(lwm2m_ext_handle,"  Type    Operation Description\n");
  QCLI_Printf(lwm2m_ext_handle,"0        Sensor value               Single         Yes      ");
  QCLI_Printf(lwm2m_ext_handle,"  Integer R         Last or current measured sensor value\n");
  QCLI_Printf(lwm2m_ext_handle,"1        Sensor units               Single         No       ");
  QCLI_Printf(lwm2m_ext_handle,"  String  R         Measurement unit definition e.g. Celcius for tempwerature.\n");
  QCLI_Printf(lwm2m_ext_handle,"2        Min measured value         Single         No       ");
  QCLI_Printf(lwm2m_ext_handle,"  Float   R         Minimum value measured since power on or reset \n");
  QCLI_Printf(lwm2m_ext_handle,"3        Max measured value         Single         No       ");
  QCLI_Printf(lwm2m_ext_handle,"  Float   R         Maximum value measured since power on or reset \n");
  QCLI_Printf(lwm2m_ext_handle,"4        Min range value            Single         Yes      ");
  QCLI_Printf(lwm2m_ext_handle,"  Float   R/W       Minimum threshold value. \n");
  QCLI_Printf(lwm2m_ext_handle,"5        Max range value            Single         Yes      ");
  QCLI_Printf(lwm2m_ext_handle,"  Float   R/W       Maximum threshold value. \n");
  QCLI_Printf(lwm2m_ext_handle,"6  Reset min and max mesaured value Single         No       ");
  QCLI_Printf(lwm2m_ext_handle,"  Opaque  E         Reset minimum and maximum measured value. \n");
  QCLI_Printf(lwm2m_ext_handle,"7        Sensor state               Single         Yes      ");
  QCLI_Printf(lwm2m_ext_handle,"  Boolean R/W       Sensor state on/off.. \n");
  QCLI_Printf(lwm2m_ext_handle,"8        Mean value                 Single         No       ");
  QCLI_Printf(lwm2m_ext_handle, "  Integer R         Mean measured value. \n");
  QCLI_Printf(lwm2m_ext_handle,"9        Sensor values              Multiple       No       ");
  QCLI_Printf(lwm2m_ext_handle, "  Integer R/W       Sensor values. \n");

  QCLI_Printf(lwm2m_ext_handle,"\n\nObject id is %d \n", LWM2M_PRESENCE_SENSOR_OBJECT_ID);
  QCLI_Printf(lwm2m_ext_handle,"Object is multi-instance \n");
  QCLI_Printf(lwm2m_ext_handle,"Res_id   Res_name                   Res_instances  Mandatory");
  QCLI_Printf(lwm2m_ext_handle,"  Type    Operation Description\n");
  QCLI_Printf(lwm2m_ext_handle,"0        Digital Input state         Single         Yes      ");
  QCLI_Printf(lwm2m_ext_handle,"  Boolean R         Current state of presence sensor\n");
  QCLI_Printf(lwm2m_ext_handle,"1        Digital Input counter       Single         No       ");
  QCLI_Printf(lwm2m_ext_handle,"  Integer R         Cummulative value of active state detected.\n");
  QCLI_Printf(lwm2m_ext_handle,"2    Digital Input counter reset     Single         No       ");
  QCLI_Printf(lwm2m_ext_handle,"  Opaque  E        Reset the counter value \n");
  QCLI_Printf(lwm2m_ext_handle,"3        Sensor type                 Single         No       ");
  QCLI_Printf(lwm2m_ext_handle,"  String  R         Type of the sensor \n");
  QCLI_Printf(lwm2m_ext_handle,"4        Busy to clear delay         Single         No       ");
  QCLI_Printf(lwm2m_ext_handle,"  Integer R/W       Delay from detection to clear state. \n");
  QCLI_Printf(lwm2m_ext_handle,"5        Clear to busy delay         Single         No       ");
  QCLI_Printf(lwm2m_ext_handle,"  Integer R/W       Delay from clear to busy state. \n");
  QCLI_Printf(lwm2m_ext_handle,"6        Supported sensor types      Multiple       No       ");
  QCLI_Printf(lwm2m_ext_handle, "  String R/W       Supported sensor types. \n");
}

QCLI_Command_Status_t init_ext_obj(uint32_t Parameter_Count,
                                   QCLI_Parameter_t *Parameter_List)
{
  int id = -1;
  int i;
  qapi_Net_LWM2M_Ext_t *ext_param1 = NULL;
  qapi_Net_LWM2M_Ext_t *ext_param2 = NULL;

  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;

  multi_app_cmd_t* cmd_ptr1 = NULL;
  multi_app_cmd_t* cmd_ptr2 = NULL;

  multi_app_cmd_parser_t *cmd_parser_temp = NULL;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, true);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(lwm2m_ext_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  if (cmd_parser_temp->app_data1.app_id != 0)
  {

    if(cmd_parser_temp->app_data1.Parameter_Count < 1 || cmd_parser_temp->app_data1.Parameter_List == NULL)
    {
      result = QCLI_STATUS_USAGE_E;
      goto end;
    }

    id = cmd_parser_temp->app_data1.app_id - 1;

    tx_byte_allocate(byte_pool_qcli, (VOID **)&ext_param1, sizeof(qapi_Net_LWM2M_Ext_t), TX_NO_WAIT);

    if(ext_param1 == NULL)
    {
      LOG_INFO("Cannot assign memory for extensible object parameter");
      result = QCLI_STATUS_ERROR_E;
      goto end;
    }

    memset(ext_param1, 0x00, sizeof(qapi_Net_LWM2M_Ext_Obj_t));

    if(id == 0 || id == 1)
    {
      ext_param1->app_data.ul_op.id = id;
      ext_param1->app_data.ul_op.msg_type = QAPI_NET_LWM2M_EXT_REGISTER_APP_E;
    }
    else
    {
      LOG_INFO("Not valid application id.");
      tx_byte_release(ext_param1);
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
    }

    LOG_INFO("Switching from QCLI to application callback context");

    if(id == 0)
    {
      if((cmd_ptr1 = multi_app_get_cmd_buffer()) == NULL)
      {
        LOG_INFO("Cannot obtain command buffer");
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }
      else
      {
        cmd_ptr1->cmd_data.cmd_id              = EXT_OBJ_APP_ORIGINATED_EVT;
        cmd_ptr1->cmd_data.data               = (void *)ext_param1;

        if(dam_thrd_running[0] == true)
        {
          dam_app1_registerd = true;
        }

        LOG_INFO("Enqueue the command into the command queue. ");
        multi_app_enqueue_cmd_t1(cmd_ptr1);
      }
  
      multi_app_set_signal_dam_t1();
  }

    else if(id == 1)
  {
      if((cmd_ptr1 = multi_app_get_cmd_buffer()) == NULL)
      {
        LOG_INFO("Cannot obtain command buffer");
        result = QCLI_STATUS_ERROR_E;
        goto end;
  }
      else
      {
        cmd_ptr1->cmd_data.cmd_id              = EXT_OBJ_APP_ORIGINATED_EVT;
        cmd_ptr1->cmd_data.data               = (void *)ext_param1;

        if(dam_thrd_running[1] == true)
  {
          dam_app2_registerd = true;
        }

        LOG_INFO("Enqueue the command into the command queue. ");
        multi_app_enqueue_cmd_t2(cmd_ptr1);
      }
  
      multi_app_set_signal_dam_t2();
  }

  else
  {
      LOG_INFO("Application id not valid.");
    }
  }

  if (cmd_parser_temp->app_data2.app_id != 0)
  {

    if(cmd_parser_temp->app_data2.Parameter_Count < 1 || cmd_parser_temp->app_data2.Parameter_List == NULL)
    {
      result = QCLI_STATUS_USAGE_E;
      goto end;
    }

    id = cmd_parser_temp->app_data2.app_id - 1;

    tx_byte_allocate(byte_pool_qcli, (VOID **)&ext_param2, sizeof(qapi_Net_LWM2M_Ext_t), TX_NO_WAIT);

    if(ext_param2 == NULL)
  {
    LOG_INFO("Cannot assign memory for extensible object parameter");
      result = QCLI_STATUS_ERROR_E;
      goto end;
  }

    memset(ext_param2, 0x00, sizeof(qapi_Net_LWM2M_Ext_Obj_t));

    if(id == 0 || id == 1)
  {
      ext_param2->app_data.ul_op.id = id;
      ext_param2->app_data.ul_op.msg_type = QAPI_NET_LWM2M_EXT_REGISTER_APP_E;
  }

  else
  {
    LOG_INFO("Not valid application id.");
      tx_byte_release(ext_param2);
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
  }

  LOG_INFO("Switching from QCLI to application callback context");

    if (id == 0)
  {
      if((cmd_ptr2 = multi_app_get_cmd_buffer()) == NULL)
    {
      LOG_INFO("Cannot obtain command buffer");
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }
    else
    {
        cmd_ptr2->cmd_data.cmd_id              = EXT_OBJ_APP_ORIGINATED_EVT;
        cmd_ptr2->cmd_data.data               = (void *)ext_param2;

        if(dam_thrd_running[0] == true)
      {
          dam_app1_registerd = true;
      }

      LOG_INFO("Enqueue the command into the command queue. ");
        multi_app_enqueue_cmd_t1(cmd_ptr2);
    }

      multi_app_set_signal_dam_t1();
  }

  else if(id == 1)
  {
      if((cmd_ptr2 = multi_app_get_cmd_buffer()) == NULL)
    {
      LOG_INFO("Cannot obtain command buffer");
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }
    else
    {
        cmd_ptr2->cmd_data.cmd_id              = EXT_OBJ_APP_ORIGINATED_EVT;
        cmd_ptr2->cmd_data.data               = (void *)ext_param2;

        if(dam_thrd_running[1] == true)
      {
          dam_app2_registerd = true;
      }

      LOG_INFO("Enqueue the command into the command queue. ");
        multi_app_enqueue_cmd_t2(cmd_ptr2);
    }

      multi_app_set_signal_dam_t2();
  }

  else
  {
    LOG_INFO("Application id not valid.");
  }
  }

end:
  if (cmd_parser_temp->app_data1.app_id != 0)
  {
    if (cmd_parser_temp->app_data1.Parameter_List != NULL)
    {
      for (i = 0; i< cmd_parser_temp->app_data1.Parameter_Count; i++)
      {
        if (cmd_parser_temp->app_data1.Parameter_List[i].String_Value != NULL)
        {
          tx_byte_release(cmd_parser_temp->app_data1.Parameter_List[i].String_Value);
          cmd_parser_temp->app_data1.Parameter_List[i].String_Value = NULL;
        }
      }
    }
  }

  if (cmd_parser_temp->app_data2.app_id != 0)
  {
    if (cmd_parser_temp->app_data2.Parameter_List != NULL)
    {
      for (i = 0; i< cmd_parser_temp->app_data2.Parameter_Count; i++)
      {
        if (cmd_parser_temp->app_data2.Parameter_List[i].String_Value != NULL)
        {
          tx_byte_release(cmd_parser_temp->app_data2.Parameter_List[i].String_Value);
          cmd_parser_temp->app_data2.Parameter_List[i].String_Value = NULL;
        }
      }
    }
  }

  tx_byte_release(cmd_parser_temp);

  return result;
}

QCLI_Command_Status_t construct_client_bootstrap_config_extobj(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List, qapi_Net_LWM2M_Config_Data_t** config_data)
{
  int   securtity_mode = 3;
  char *bootstrap_uri = NULL;
  char *apn_name     = NULL;

  int uri_len = -1;
  int apn_len = -1;

  qapi_Net_LWM2M_Config_Data_t* config_bootstrap     = NULL;
  qapi_Net_LWM2M_Config_Data_t* config_apn           = NULL;
  qapi_Net_LWM2M_Config_Data_t* config_security_mode = NULL;

  uri_len = MAX_URI_LEN + 1;
  tx_byte_allocate(byte_pool_qcli, (VOID **)&bootstrap_uri, uri_len, TX_NO_WAIT);
  if(bootstrap_uri == NULL)
  {
    return QCLI_STATUS_ERROR_E;
  }
  memset(bootstrap_uri, 0, uri_len);
  apn_len = MAX_APN_LEN + 1;
  tx_byte_allocate(byte_pool_qcli, (VOID **)&apn_name, apn_len, TX_NO_WAIT);
  if(apn_name == NULL)
  {
    return QCLI_STATUS_ERROR_E;
  }

  if(Parameter_Count == 4 && Parameter_List != NULL)
  {
	if(Parameter_List[1].String_Value == NULL || Parameter_List[2].String_Value == NULL)
	{
	  QCLI_Printf(lwm2m_ext_handle, "uri/apn value passed is NULL");
	  tx_byte_release(apn_name);
	  tx_byte_release(bootstrap_uri);	  
	  return QCLI_STATUS_USAGE_E;
	}
	strlcpy(bootstrap_uri, (char*)Parameter_List[1].String_Value, uri_len);
	strlcpy(apn_name,      (char*)Parameter_List[2].String_Value, apn_len);
	if(Parameter_List[3].Integer_Is_Valid)
	{
	  securtity_mode = Parameter_List[3].Integer_Value;
	}  
  }
  else
  {
    QCLI_Printf(lwm2m_ext_handle, "uri/apn value passed is NULL");
	tx_byte_release(apn_name);
    tx_byte_release(bootstrap_uri);	  
    return QCLI_STATUS_USAGE_E;
  }

  QCLI_Printf(lwm2m_ext_handle, "Setting bootstrap URI:%s \n", bootstrap_uri);
  QCLI_Printf(lwm2m_ext_handle, "Setting APN :%s \n",          apn_name);  
  QCLI_Printf(lwm2m_ext_handle, "Security Mode :%d \n",        securtity_mode);  

  tx_byte_allocate(byte_pool_qcli, (VOID **)&config_bootstrap, sizeof(qapi_Net_LWM2M_Config_Data_t), TX_NO_WAIT);
  tx_byte_allocate(byte_pool_qcli, (VOID **)&config_apn, sizeof(qapi_Net_LWM2M_Config_Data_t), TX_NO_WAIT);
  tx_byte_allocate(byte_pool_qcli, (VOID **)&config_security_mode, sizeof(qapi_Net_LWM2M_Config_Data_t), TX_NO_WAIT);

  if(config_bootstrap == NULL || config_apn == NULL ||config_security_mode == NULL)
  {
    tx_byte_release(apn_name);
	tx_byte_release(bootstrap_uri);
    return QCLI_STATUS_ERROR_E;
  }

  config_bootstrap->config_type = QAPI_NET_LWM2M_CONFIG_BOOTSTRAP_URL;
  tx_byte_allocate(byte_pool_qcli, (VOID **)&config_bootstrap->value.asBuffer.buffer, strlen(bootstrap_uri)+1, TX_NO_WAIT);
  config_bootstrap->value.asBuffer.length = strlen(bootstrap_uri);
  strlcpy((char*)config_bootstrap->value.asBuffer.buffer, bootstrap_uri, strlen(bootstrap_uri)+1);
  config_bootstrap->next = config_apn;

  config_apn->config_type = QAPI_NET_LWM2M_CONFIG_APN_NAME;
  tx_byte_allocate(byte_pool_qcli, (VOID **)&config_apn->value.asBuffer.buffer, strlen(apn_name)+1, TX_NO_WAIT);
  config_apn->value.asBuffer.length = strlen(apn_name);  
  strlcpy((char*)config_apn->value.asBuffer.buffer, apn_name, strlen(apn_name)+1);
  config_apn->next = config_security_mode;

  config_security_mode->config_type = QAPI_NET_LWM2M_CONFIG_SECURITY_MODE;
  config_security_mode->value.asInteger = securtity_mode;
  config_security_mode->next = NULL;

  if(bootstrap_uri)
  {
    tx_byte_release(bootstrap_uri);
    bootstrap_uri = NULL;
  }
  if(apn_name)
  {
    tx_byte_release(apn_name);
    apn_name = NULL;
  }

  *config_data = config_bootstrap;
  return QCLI_STATUS_SUCCESS_E;
}

QCLI_Command_Status_t config_client_ext_obj(uint32_t Parameter_Count,
                                   QCLI_Parameter_t *Parameter_List)
{
  int id = -1;
  QCLI_Command_Status_t qcli_result = QCLI_STATUS_SUCCESS_E;

  int i;

  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;

  qapi_Net_LWM2M_Ext_t *ext_param1 = NULL;
  qapi_Net_LWM2M_Ext_t *ext_param2 = NULL;

  multi_app_cmd_t* cmd_ptr1 = NULL;
  multi_app_cmd_t* cmd_ptr2 = NULL;

  multi_app_cmd_parser_t *cmd_parser_temp = NULL;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, true);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(lwm2m_ext_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  if (cmd_parser_temp->app_data1.app_id != 0)
  {
    if(cmd_parser_temp->app_data1.Parameter_Count < 1 || cmd_parser_temp->app_data1.Parameter_List == NULL)
    {
      result = QCLI_STATUS_USAGE_E;
      goto end;
    }

    id = cmd_parser_temp->app_data1.app_id - 1;

    qcli_result = construct_client_bootstrap_config_extobj(cmd_parser_temp->app_data1.Parameter_Count, cmd_parser_temp->app_data1.Parameter_List, &ext_object_clientconfig_data);
    if(qcli_result != QCLI_STATUS_SUCCESS_E)
    {
      result = qcli_result;
      goto end;
  }

    tx_byte_allocate(byte_pool_qcli, (VOID **)&ext_param1, sizeof(qapi_Net_LWM2M_Ext_t), TX_NO_WAIT);

    if(ext_param1 == NULL)
    {
      LOG_INFO("Cannot assign memory for extensible object parameter");
      result = QCLI_STATUS_ERROR_E;
      goto end;
    }

    memset(ext_param1, 0x00, sizeof(qapi_Net_LWM2M_Ext_Obj_t));

    if(id == 0 || id == 1)
    {
      ext_param1->app_data.ul_op.id = id;
      ext_param1->app_data.ul_op.msg_type = QAPI_NET_LWM2M_EXT_CONFIG_CLIENT_E;
    }

    else
    {
      LOG_INFO("Not valid application id.");
      tx_byte_release(ext_param1);
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
    }

    LOG_INFO("Switching from QCLI to application callback context");

    if (id == 0)
    {
      if((cmd_ptr1 = multi_app_get_cmd_buffer()) == NULL)
  {
        LOG_INFO("Cannot obtain command buffer");
        result = QCLI_STATUS_ERROR_E;
        goto end;
      }
      else
      {
        cmd_ptr1->cmd_data.cmd_id              = EXT_OBJ_APP_ORIGINATED_EVT;
        cmd_ptr1->cmd_data.data               = (void *)ext_param1;

        if(dam_app1_registerd == false)
        {
          QCLI_Printf(lwm2m_ext_handle,"Application %d not registerd ",id+1);
          tx_byte_release(ext_param1);
          result = QCLI_STATUS_SUCCESS_E;
          goto end;
        }
        LOG_INFO("Enqueue the command into the command queue. ");
        multi_app_enqueue_cmd_t1(cmd_ptr1);
      }

      multi_app_set_signal_dam_t1();
    }

    else if(id == 1)
    {
      if((cmd_ptr1 = multi_app_get_cmd_buffer()) == NULL)
      {
        LOG_INFO("Cannot obtain command buffer");
        result = QCLI_STATUS_ERROR_E;
        goto end;
      }
      else
      {
        cmd_ptr1->cmd_data.cmd_id              = EXT_OBJ_APP_ORIGINATED_EVT;
        cmd_ptr1->cmd_data.data               = (void *)ext_param1;

        if(dam_app2_registerd == false)
        {
          QCLI_Printf(lwm2m_ext_handle,"Application %d not registerd ",id+1);
          tx_byte_release(ext_param1);
          result = QCLI_STATUS_SUCCESS_E;
          goto end;
    }
        LOG_INFO("Enqueue the command into the command queue. ");
        multi_app_enqueue_cmd_t2(cmd_ptr1);
      }

      multi_app_set_signal_dam_t2();
    }

    else
    {
      LOG_INFO("Application id not valid.");
    }
  }

  if (cmd_parser_temp->app_data2.app_id != 0)
 {

     if(cmd_parser_temp->app_data2.Parameter_Count < 1 || cmd_parser_temp->app_data2.Parameter_List == NULL)
    {
      result = QCLI_STATUS_USAGE_E;
      goto end;
    }

    id = cmd_parser_temp->app_data2.app_id - 1;

    qcli_result = construct_client_bootstrap_config_extobj(cmd_parser_temp->app_data2.Parameter_Count, cmd_parser_temp->app_data2.Parameter_List, &ext_object_clientconfig_data);
  if(qcli_result != QCLI_STATUS_SUCCESS_E)
  {
      result = qcli_result;
      goto end;
  }

    tx_byte_allocate(byte_pool_qcli, (VOID **)&ext_param2, sizeof(qapi_Net_LWM2M_Ext_t), TX_NO_WAIT);

    if(ext_param2 == NULL)
  {
    LOG_INFO("Cannot assign memory for extensible object parameter");
      result = QCLI_STATUS_ERROR_E;
      goto end;
  }

    memset(ext_param2, 0x00, sizeof(qapi_Net_LWM2M_Ext_Obj_t));

  if(id == 0 || id == 1)
  {
      ext_param2->app_data.ul_op.id = id;
      ext_param2->app_data.ul_op.msg_type = QAPI_NET_LWM2M_EXT_CONFIG_CLIENT_E;
  }
  else
  {
    LOG_INFO("Not valid application id.");
      tx_byte_release(ext_param2);
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
  }

  LOG_INFO("Switching from QCLI to application callback context");

    if (id == 0)
  {
      if((cmd_ptr2 = multi_app_get_cmd_buffer()) == NULL)
    {
      LOG_INFO("Cannot obtain command buffer");
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }
    else
    {
        cmd_ptr2->cmd_data.cmd_id              = EXT_OBJ_APP_ORIGINATED_EVT;
        cmd_ptr2->cmd_data.data               = (void *)ext_param2;

        if(dam_app1_registerd == false)
      {
        QCLI_Printf(lwm2m_ext_handle,"Application %d not registerd ",id+1);
          tx_byte_release(ext_param2);
          result = QCLI_STATUS_SUCCESS_E;
          goto end;
      }
      LOG_INFO("Enqueue the command into the command queue. ");
        multi_app_enqueue_cmd_t1(cmd_ptr2);
    }

      multi_app_set_signal_dam_t1();
  }

  else if(id == 1)
  {
      if((cmd_ptr2 = multi_app_get_cmd_buffer()) == NULL)
    {
      LOG_INFO("Cannot obtain command buffer");
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }
    else
    {
        cmd_ptr2->cmd_data.cmd_id              = EXT_OBJ_APP_ORIGINATED_EVT;
        cmd_ptr2->cmd_data.data               = (void *)ext_param2;

        if(dam_app2_registerd == false)
      {
        QCLI_Printf(lwm2m_ext_handle,"Application %d not registerd ",id+1);
          tx_byte_release(ext_param2);
          result = QCLI_STATUS_SUCCESS_E;
          goto end;
      }
      LOG_INFO("Enqueue the command into the command queue. ");
        multi_app_enqueue_cmd_t2(cmd_ptr2);
    }

      multi_app_set_signal_dam_t2();
  }

  else
  {
    LOG_INFO("Application id not valid.");
  }
  }

 end:
  if (cmd_parser_temp->app_data1.app_id != 0)
  {
    if (cmd_parser_temp->app_data1.Parameter_List != NULL)
    {
      for (i = 0; i< cmd_parser_temp->app_data1.Parameter_Count; i++)
      {
        if (cmd_parser_temp->app_data1.Parameter_List[i].String_Value != NULL)
        {
          tx_byte_release(cmd_parser_temp->app_data1.Parameter_List[i].String_Value);
          cmd_parser_temp->app_data1.Parameter_List[i].String_Value = NULL;
        }
      }
    }
  }

  if (cmd_parser_temp->app_data2.app_id != 0)
  {
    if (cmd_parser_temp->app_data2.Parameter_List != NULL)
    {
      for (i = 0; i< cmd_parser_temp->app_data2.Parameter_Count; i++)
      {
        if (cmd_parser_temp->app_data2.Parameter_List[i].String_Value != NULL)
        {
          tx_byte_release(cmd_parser_temp->app_data2.Parameter_List[i].String_Value);
          cmd_parser_temp->app_data2.Parameter_List[i].String_Value = NULL;
        }
      }
    }
  }

  tx_byte_release(cmd_parser_temp);

  return result;
}

QCLI_Command_Status_t create_ext_obj(uint32_t Parameter_Count,
                                     QCLI_Parameter_t *Parameter_List)
{
  int id = -1;
  int i;

  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;

  qapi_Net_LWM2M_Ext_t *ext_param1 = NULL;
  qapi_Net_LWM2M_Ext_t *ext_param2 = NULL;

  multi_app_cmd_t* cmd_ptr1 = NULL;
  multi_app_cmd_t* cmd_ptr2 = NULL;

  multi_app_cmd_parser_t *cmd_parser_temp = NULL;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, true);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(lwm2m_ext_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  if (cmd_parser_temp->app_data1.app_id != 0)
  {
    if(cmd_parser_temp->app_data1.Parameter_Count < 1 || cmd_parser_temp->app_data1.Parameter_List == NULL)
    {
      result = QCLI_STATUS_USAGE_E;
      goto end;
    }

    id = cmd_parser_temp->app_data1.app_id - 1;

    tx_byte_allocate(byte_pool_qcli, (VOID **)&ext_param1, sizeof(qapi_Net_LWM2M_Ext_t), TX_NO_WAIT);

    if(ext_param1 == NULL)
    {
      LOG_INFO("Cannot assign memory for extensible object parameter");
      result = QCLI_STATUS_ERROR_E;
      goto end;
    }

    memset(ext_param1, 0x00, sizeof(qapi_Net_LWM2M_Ext_Obj_t));

    if(id == 0 || id == 1)
    {
      ext_param1->app_data.ul_op.id = id;
      ext_param1->app_data.ul_op.msg_type = QAPI_NET_LWM2M_EXT_CREATE_OBJ_E;
    }
    else
  {
      LOG_INFO("Not valid application id.");
      tx_byte_release(ext_param1);
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
  }

    LOG_INFO("Switching from QCLI to application callback context");

    if (id == 0)
    {
      if((cmd_ptr1 = multi_app_get_cmd_buffer()) == NULL)
      {
        LOG_INFO("Cannot obtain command buffer");
        result = QCLI_STATUS_ERROR_E;
        goto end;
      }
      else
      {
        cmd_ptr1->cmd_data.cmd_id              = EXT_OBJ_APP_ORIGINATED_EVT;
        cmd_ptr1->cmd_data.data               = (void *)ext_param1;

        if(dam_app1_registerd == false)
        {
          QCLI_Printf(lwm2m_ext_handle,"Application %d not registerd ",id+1);
          tx_byte_release(ext_param1);
          result = QCLI_STATUS_SUCCESS_E;
          goto end;
        }
        LOG_INFO("Enqueue the command into the command queue. ");
        multi_app_enqueue_cmd_t1(cmd_ptr1);
      }

      multi_app_set_signal_dam_t1();
    }

    else if(id == 1)
    {
      if((cmd_ptr1 = multi_app_get_cmd_buffer()) == NULL)
      {
        LOG_INFO("Cannot obtain command buffer");
        result = QCLI_STATUS_ERROR_E;
        goto end;
      }
      else
      {
        cmd_ptr1->cmd_data.cmd_id              = EXT_OBJ_APP_ORIGINATED_EVT;
        cmd_ptr1->cmd_data.data               = (void *)ext_param1;

        if(dam_app2_registerd == false)
        {
          QCLI_Printf(lwm2m_ext_handle,"Application %d not registerd ",id+1);
          tx_byte_release(ext_param1);
          result = QCLI_STATUS_SUCCESS_E;
          goto end;
        }
        LOG_INFO("Enqueue the command into the command queue. ");
        multi_app_enqueue_cmd_t2(cmd_ptr1);
      }

      multi_app_set_signal_dam_t2();
    }

    else
    {
      LOG_INFO("Application id not valid.");
    }
  }

  if (cmd_parser_temp->app_data2.app_id != 0)
  {
    if(cmd_parser_temp->app_data2.Parameter_Count < 1 || cmd_parser_temp->app_data2.Parameter_List == NULL)
    {
      result = QCLI_STATUS_USAGE_E;
      goto end;
    }

    id = cmd_parser_temp->app_data2.app_id - 1;

    tx_byte_allocate(byte_pool_qcli, (VOID **)&ext_param2, sizeof(qapi_Net_LWM2M_Ext_t), TX_NO_WAIT);

    if(ext_param2 == NULL)
  {
    LOG_INFO("Cannot assign memory for extensible object parameter");
      result = QCLI_STATUS_ERROR_E;
      goto end;
  }

    memset(ext_param2, 0x00, sizeof(qapi_Net_LWM2M_Ext_Obj_t));

  if(id == 0 || id == 1)
  {
      ext_param2->app_data.ul_op.id = id;
      ext_param2->app_data.ul_op.msg_type = QAPI_NET_LWM2M_EXT_CREATE_OBJ_E;
  }

  else
  {
    LOG_INFO("Not valid application id.");
      tx_byte_release(ext_param2);
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
  }

  LOG_INFO("Switching from QCLI to application callback context");

  if(id == 0)
  {
      if((cmd_ptr2 = multi_app_get_cmd_buffer()) == NULL)
    {
      LOG_INFO("Cannot obtain command buffer");
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }
    else
    {
        cmd_ptr2->cmd_data.cmd_id              = EXT_OBJ_APP_ORIGINATED_EVT;
        cmd_ptr2->cmd_data.data               = (void *)ext_param2;

        if(dam_app1_registerd == false)
      {
        QCLI_Printf(lwm2m_ext_handle,"Application %d not registerd ",id+1);
          tx_byte_release(ext_param2);
          result = QCLI_STATUS_SUCCESS_E;
          goto end;
      }
      LOG_INFO("Enqueue the command into the command queue. ");
        multi_app_enqueue_cmd_t1(cmd_ptr2);
    }

      multi_app_set_signal_dam_t1();
  }

  else if(id == 1)
  {
      if((cmd_ptr2 = multi_app_get_cmd_buffer()) == NULL)
    {
      LOG_INFO("Cannot obtain command buffer");
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }
    else
    {
        cmd_ptr2->cmd_data.cmd_id              = EXT_OBJ_APP_ORIGINATED_EVT;
        cmd_ptr2->cmd_data.data               = (void *)ext_param2;

        if(dam_app2_registerd == false)
      {
        QCLI_Printf(lwm2m_ext_handle,"Application %d not registerd ",id+1);
          tx_byte_release(ext_param2);
          result = QCLI_STATUS_SUCCESS_E;
          goto end;
      }
      LOG_INFO("Enqueue the command into the command queue. ");
        multi_app_enqueue_cmd_t2(cmd_ptr2);
    }
  
      multi_app_set_signal_dam_t2();
  }

  else
  {
    LOG_INFO("Application id not valid.");
  }
  }

end:
  if (cmd_parser_temp->app_data1.app_id != 0)
  {
    if (cmd_parser_temp->app_data1.Parameter_List != NULL)
    {
      for (i = 0; i< cmd_parser_temp->app_data1.Parameter_Count; i++)
      {
        if (cmd_parser_temp->app_data1.Parameter_List[i].String_Value != NULL)
        {
          tx_byte_release(cmd_parser_temp->app_data1.Parameter_List[i].String_Value);
          cmd_parser_temp->app_data1.Parameter_List[i].String_Value = NULL;
        }
      }
    }
  }

  if (cmd_parser_temp->app_data2.app_id != 0)
  {
    if (cmd_parser_temp->app_data2.Parameter_List != NULL)
    {
      for (i = 0; i< cmd_parser_temp->app_data2.Parameter_Count; i++)
      {
        if (cmd_parser_temp->app_data2.Parameter_List[i].String_Value != NULL)
        {
          tx_byte_release(cmd_parser_temp->app_data2.Parameter_List[i].String_Value);
          cmd_parser_temp->app_data2.Parameter_List[i].String_Value = NULL;
        }
      }
    }
  }

  tx_byte_release(cmd_parser_temp);

  return result;
}

QCLI_Command_Status_t display_template(uint32_t Parameter_Count,
                                       QCLI_Parameter_t *Parameter_List)
{
  display_object_template();
  return QCLI_STATUS_SUCCESS_E;
}

QCLI_Command_Status_t create_ext_obj_inst(uint32_t Parameter_Count,
                                          QCLI_Parameter_t *Parameter_List)
{
  int id = -1;
  int obj_id = -1;
  int i;

  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;

  qapi_Net_LWM2M_Ext_t *ext_param1 = NULL;
  qapi_Net_LWM2M_Ext_t *ext_param2 = NULL;

  multi_app_cmd_t* cmd_ptr1 = NULL;
  multi_app_cmd_t* cmd_ptr2 = NULL;

  multi_app_cmd_parser_t *cmd_parser_temp = NULL;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, true);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(lwm2m_ext_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  if (cmd_parser_temp->app_data1.app_id != 0)
  {
    id = cmd_parser_temp->app_data1.app_id - 1;

    if(cmd_parser_temp->app_data1.Parameter_Count < 2 || cmd_parser_temp->app_data1.Parameter_List == NULL)
  {
      result = QCLI_STATUS_USAGE_E;
      goto end;
  }

    if (cmd_parser_temp->app_data1.Parameter_List[1].Integer_Is_Valid)
  {
      obj_id = cmd_parser_temp->app_data1.Parameter_List[1].Integer_Value;
  }

    tx_byte_allocate(byte_pool_qcli, (VOID **)&ext_param1, sizeof(qapi_Net_LWM2M_Ext_t), TX_NO_WAIT);

    if(ext_param1 == NULL)
  {
      LOG_INFO("Cannot assign memory for extensible object parameter");
      result = QCLI_STATUS_ERROR_E;
      goto end;
  }

    memset(ext_param1, 0x00, sizeof(qapi_Net_LWM2M_Ext_Obj_t));

    if(id == 0 || id == 1)
    {
      ext_param1->app_data.ul_op.id = id;
      ext_param1->app_data.ul_op.msg_type = QAPI_NET_LWM2M_EXT_CREATE_OBJ_INSTANCE_E;
      ext_param1->app_data.ul_op.obj_info.obj_mask |= QAPI_NET_LWM2M_OBJECT_ID_E;
      if(obj_id != 0)
      {
        LOG_INFO("Invalid object id provided.");
        tx_byte_release(ext_param1);
        result = QCLI_STATUS_SUCCESS_E;
        goto end;
      }
      if(id == 0)
      {
        obj_id = LWM2M_GENERIC_SENSOR_OBJECT_ID;
      }
      else if (id == 1)
      {
        obj_id = LWM2M_PRESENCE_SENSOR_OBJECT_ID;
      }
      ext_param1->app_data.ul_op.obj_info.obj_id = obj_id;
    }

    else
    {
      LOG_INFO("Not valid application id.");
      tx_byte_release(ext_param1);
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
    }

    LOG_INFO("Switching from QCLI to application callback context");

    if (id == 0)
    {
      if((cmd_ptr1 = multi_app_get_cmd_buffer()) == NULL)
      {
        LOG_INFO("Cannot obtain command buffer");
        result = QCLI_STATUS_ERROR_E;
        goto end;
      }
      else
      {
        cmd_ptr1->cmd_data.cmd_id              = EXT_OBJ_APP_ORIGINATED_EVT;
        cmd_ptr1->cmd_data.data               = (void *)ext_param1;

        if(dam_app1_registerd == false)
        {
          QCLI_Printf(lwm2m_ext_handle,"Application %d not registerd ",id+1);
          tx_byte_release(ext_param1);
          result = QCLI_STATUS_SUCCESS_E;
          goto end;
        }
        LOG_INFO("Enqueue the command into the command queue. ");
        multi_app_enqueue_cmd_t1(cmd_ptr1);
      }

      multi_app_set_signal_dam_t1();
    }

    else if(id == 1)
    {
      if((cmd_ptr1 = multi_app_get_cmd_buffer()) == NULL)
      {
        LOG_INFO("Cannot obtain command buffer");
        result = QCLI_STATUS_ERROR_E;
        goto end;
      }
      else
      {
        cmd_ptr1->cmd_data.cmd_id              = EXT_OBJ_APP_ORIGINATED_EVT;
        cmd_ptr1->cmd_data.data               = (void *)ext_param1;

        if(dam_app2_registerd == false)
        {
          QCLI_Printf(lwm2m_ext_handle,"Application %d not registerd ",id+1);
          tx_byte_release(ext_param1);
          result = QCLI_STATUS_SUCCESS_E;
          goto end;
    }
        LOG_INFO("Enqueue the command into the command queue. ");
        multi_app_enqueue_cmd_t2(cmd_ptr1);
      }

      multi_app_set_signal_dam_t2();
    }

    else
    {
      LOG_INFO("Application id not valid.");
    }
  }

  if (cmd_parser_temp->app_data2.app_id != 0)
  {
    id = cmd_parser_temp->app_data2.app_id - 1;

    if(cmd_parser_temp->app_data2.Parameter_Count < 2 || cmd_parser_temp->app_data2.Parameter_List == NULL)
    {
      result = QCLI_STATUS_USAGE_E;
      goto end;
    }

    if (cmd_parser_temp->app_data2.Parameter_List[1].Integer_Is_Valid)
    {
      obj_id = cmd_parser_temp->app_data2.Parameter_List[1].Integer_Value;
    }

    tx_byte_allocate(byte_pool_qcli, (VOID **)&ext_param2, sizeof(qapi_Net_LWM2M_Ext_t), TX_NO_WAIT);

    if(ext_param2 == NULL)
  {
    LOG_INFO("Cannot assign memory for extensible object parameter");
      result = QCLI_STATUS_ERROR_E;
      goto end;
  }

    memset(ext_param2, 0x00, sizeof(qapi_Net_LWM2M_Ext_Obj_t));

  if(id == 0 || id == 1)
  {
      ext_param2->app_data.ul_op.id = id;
      ext_param2->app_data.ul_op.msg_type = QAPI_NET_LWM2M_EXT_CREATE_OBJ_INSTANCE_E;
      ext_param2->app_data.ul_op.obj_info.obj_mask |= QAPI_NET_LWM2M_OBJECT_ID_E;
    if(obj_id != 0)
    {
      LOG_INFO("Invalid object id provided.");
        tx_byte_release(ext_param2);
        result = QCLI_STATUS_SUCCESS_E;
        goto end;
    }
    if(id == 0)
    {
      obj_id = LWM2M_GENERIC_SENSOR_OBJECT_ID;
    }
    else if (id == 1)
    {
       obj_id = LWM2M_PRESENCE_SENSOR_OBJECT_ID;
    }
      ext_param2->app_data.ul_op.obj_info.obj_id = obj_id;
  }

  else
  {
    LOG_INFO("Not valid application id.");
      tx_byte_release(ext_param2);
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
  }

  LOG_INFO("Switching from QCLI to application callback context");

  if(id == 0)
  {
      if((cmd_ptr2 = multi_app_get_cmd_buffer()) == NULL)
    {
      LOG_INFO("Cannot obtain command buffer");
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }
    else
    {
        cmd_ptr2->cmd_data.cmd_id              = EXT_OBJ_APP_ORIGINATED_EVT;
        cmd_ptr2->cmd_data.data               = (void *)ext_param2;

        if(dam_app1_registerd == false)
      {
        QCLI_Printf(lwm2m_ext_handle,"Application %d not registerd ",id+1);
          tx_byte_release(ext_param2);
          result = QCLI_STATUS_SUCCESS_E;
          goto end;
      }
      LOG_INFO("Enqueue the command into the command queue. ");
        multi_app_enqueue_cmd_t1(cmd_ptr2);
    }
  
      multi_app_set_signal_dam_t1();
  }

  else if(id == 1)
  {
      if((cmd_ptr2 = multi_app_get_cmd_buffer()) == NULL)
    {
      LOG_INFO("Cannot obtain command buffer");
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }
    else
    {
        cmd_ptr2->cmd_data.cmd_id              = EXT_OBJ_APP_ORIGINATED_EVT;
        cmd_ptr2->cmd_data.data               = (void *)ext_param2;

        if(dam_app2_registerd == false)
      {
        QCLI_Printf(lwm2m_ext_handle,"Application %d not registerd ",id+1);
          tx_byte_release(ext_param2);
          result = QCLI_STATUS_SUCCESS_E;
          goto end;
      }
      LOG_INFO("Enqueue the command into the command queue. ");
        multi_app_enqueue_cmd_t2(cmd_ptr2);
    }
  
      multi_app_set_signal_dam_t2();
  }

  else
  {
    LOG_INFO("Application id not valid.");
  }
  }

end:
  if (cmd_parser_temp->app_data1.app_id != 0)
  {
    if (cmd_parser_temp->app_data1.Parameter_List != NULL)
    {
      for (i = 0; i< cmd_parser_temp->app_data1.Parameter_Count; i++)
      {
        if (cmd_parser_temp->app_data1.Parameter_List[i].String_Value != NULL)
        {
          tx_byte_release(cmd_parser_temp->app_data1.Parameter_List[i].String_Value);
          cmd_parser_temp->app_data1.Parameter_List[i].String_Value = NULL;
        }
      }
    }
  }

  if (cmd_parser_temp->app_data2.app_id != 0)
  {
    if (cmd_parser_temp->app_data2.Parameter_List != NULL)
    {
      for (i = 0; i< cmd_parser_temp->app_data2.Parameter_Count; i++)
      {
        if (cmd_parser_temp->app_data2.Parameter_List[i].String_Value != NULL)
        {
          tx_byte_release(cmd_parser_temp->app_data2.Parameter_List[i].String_Value);
          cmd_parser_temp->app_data2.Parameter_List[i].String_Value = NULL;
        }
      }
    }
  }

  tx_byte_release(cmd_parser_temp);

  return result;
}

QCLI_Command_Status_t delete_ext_obj_inst(uint32_t Parameter_Count,
                                          QCLI_Parameter_t *Parameter_List)
{
  int id = -1;
  int inst_id = -1;
  int obj_id = -1;
  int i;

  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;

  qapi_Net_LWM2M_Ext_t *ext_param1 = NULL;
  qapi_Net_LWM2M_Ext_t *ext_param2 = NULL;

  multi_app_cmd_t* cmd_ptr1 = NULL;
  multi_app_cmd_t* cmd_ptr2 = NULL;

  multi_app_cmd_parser_t *cmd_parser_temp = NULL;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, true);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(lwm2m_ext_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  if (cmd_parser_temp->app_data1.app_id != 0)
  {
    id = cmd_parser_temp->app_data1.app_id - 1;

    if(cmd_parser_temp->app_data1.Parameter_Count < 3 || cmd_parser_temp->app_data1.Parameter_List == NULL)
    {
      result = QCLI_STATUS_USAGE_E;
      goto end;
    }

    if (cmd_parser_temp->app_data1.Parameter_List[1].Integer_Is_Valid)
    {
      obj_id = cmd_parser_temp->app_data1.Parameter_List[1].Integer_Value;
    }

    if (cmd_parser_temp->app_data1.Parameter_List[2].Integer_Is_Valid)
    {
      inst_id = cmd_parser_temp->app_data1.Parameter_List[2].Integer_Value;
    }

    tx_byte_allocate(byte_pool_qcli, (VOID **)&ext_param1, sizeof(qapi_Net_LWM2M_Ext_t), TX_NO_WAIT);

    if(ext_param1 == NULL)
  {
      LOG_INFO("Cannot assign memory for extensible object parameter");
      result = QCLI_STATUS_ERROR_E;
      goto end;
  }

    memset(ext_param1, 0x00, sizeof(qapi_Net_LWM2M_Ext_Obj_t));

    if(id == 0 || id == 1)
    {
      ext_param1->app_data.ul_op.id = id;
      ext_param1->app_data.ul_op.msg_type = QAPI_NET_LWM2M_EXT_DELETE_OBJ_INSTANCE_E;
      if(obj_id != 0)
      {
        LOG_INFO("Invalid object id provided.");
        tx_byte_release(ext_param1);
        result = QCLI_STATUS_SUCCESS_E;
        goto end;
      }
      if(inst_id == -1)
      {
        LOG_INFO("Invalid instance id provided.");
        tx_byte_release(ext_param1);
        result = QCLI_STATUS_SUCCESS_E;
        goto end;
      }
      ext_param1->app_data.ul_op.obj_info.obj_mask |= QAPI_NET_LWM2M_OBJECT_ID_E;
      ext_param1->app_data.ul_op.obj_info.obj_mask |= QAPI_NET_LWM2M_INSTANCE_ID_E;
      if(id == 0)
      {
        obj_id = LWM2M_GENERIC_SENSOR_OBJECT_ID;
      }
      else if (id == 1)
      {
        obj_id = LWM2M_PRESENCE_SENSOR_OBJECT_ID;
      }
      ext_param1->app_data.ul_op.obj_info.obj_id = obj_id;
      ext_param1->app_data.ul_op.obj_info.obj_inst_id = inst_id;
    }

    else
    {
      LOG_INFO("Not valid application id.");
      tx_byte_release(ext_param1);
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
    }

    LOG_INFO("Switching from QCLI to application callback context");

    if (id == 0)
    {
      if((cmd_ptr1 = multi_app_get_cmd_buffer()) == NULL)
      {
        LOG_INFO("Cannot obtain command buffer");
        result = QCLI_STATUS_ERROR_E;
        goto end;
      }
      else
      {
        cmd_ptr1->cmd_data.cmd_id              = EXT_OBJ_APP_ORIGINATED_EVT;
        cmd_ptr1->cmd_data.data               = (void *)ext_param1;

        if(dam_app1_registerd == false)
        {
          QCLI_Printf(lwm2m_ext_handle,"Application %d not registerd ",id+1);
          tx_byte_release(ext_param1);
          result = QCLI_STATUS_SUCCESS_E;
          goto end;
        }
        LOG_INFO("Enqueue the command into the command queue. ");
        multi_app_enqueue_cmd_t1(cmd_ptr1);
      }

      multi_app_set_signal_dam_t1();
    }

    else if(id == 1)
    {
      if((cmd_ptr1 = multi_app_get_cmd_buffer()) == NULL)
      {
        LOG_INFO("Cannot obtain command buffer");
        result = QCLI_STATUS_ERROR_E;
        goto end;
      }
      else
      {
        cmd_ptr1->cmd_data.cmd_id              = EXT_OBJ_APP_ORIGINATED_EVT;
        cmd_ptr1->cmd_data.data               = (void *)ext_param1;

        if(dam_app2_registerd == false)
        {
          QCLI_Printf(lwm2m_ext_handle,"Application %d not registerd ",id+1);
          tx_byte_release(ext_param1);
          result = QCLI_STATUS_SUCCESS_E;
          goto end;
        }
        LOG_INFO("Enqueue the command into the command queue. ");
        multi_app_enqueue_cmd_t2(cmd_ptr1);
      }

      multi_app_set_signal_dam_t2();
    }

    else
    {
      LOG_INFO("Application id not valid.");
    }
  }

  if (cmd_parser_temp->app_data2.app_id != 0)
  {
    id = cmd_parser_temp->app_data2.app_id - 1;

    if(cmd_parser_temp->app_data2.Parameter_Count < 3 || cmd_parser_temp->app_data2.Parameter_List == NULL)
    {
      result = QCLI_STATUS_USAGE_E;
      goto end;
    }

    if (cmd_parser_temp->app_data2.Parameter_List[1].Integer_Is_Valid)
    {
      obj_id = cmd_parser_temp->app_data2.Parameter_List[1].Integer_Value;
    }

    if (cmd_parser_temp->app_data2.Parameter_List[2].Integer_Is_Valid)
    {
      inst_id = cmd_parser_temp->app_data2.Parameter_List[2].Integer_Value;
    }

    tx_byte_allocate(byte_pool_qcli, (VOID **)&ext_param2, sizeof(qapi_Net_LWM2M_Ext_t), TX_NO_WAIT);

    if(ext_param2 == NULL)
  {
    LOG_INFO("Cannot assign memory for extensible object parameter");
      result = QCLI_STATUS_ERROR_E;
      goto end;
  }

    memset(ext_param2, 0x00, sizeof(qapi_Net_LWM2M_Ext_Obj_t));

  if(id == 0 || id == 1)
  {
      ext_param2->app_data.ul_op.id = id;
      ext_param2->app_data.ul_op.msg_type = QAPI_NET_LWM2M_EXT_DELETE_OBJ_INSTANCE_E;
    if(obj_id != 0)
    {
      LOG_INFO("Invalid object id provided.");
        tx_byte_release(ext_param2);
        result = QCLI_STATUS_SUCCESS_E;
        goto end;
    }
    if(inst_id == -1)
    {
      LOG_INFO("Invalid instance id provided.");
        tx_byte_release(ext_param1);
        result = QCLI_STATUS_SUCCESS_E;
        goto end;
    }
      ext_param2->app_data.ul_op.obj_info.obj_mask |= QAPI_NET_LWM2M_OBJECT_ID_E;
      ext_param2->app_data.ul_op.obj_info.obj_mask |= QAPI_NET_LWM2M_INSTANCE_ID_E;
    if(id == 0)
    {
      obj_id = LWM2M_GENERIC_SENSOR_OBJECT_ID;
    }
    else if (id == 1)
    {
      obj_id = LWM2M_PRESENCE_SENSOR_OBJECT_ID;
    }
      ext_param2->app_data.ul_op.obj_info.obj_id = obj_id;
      ext_param2->app_data.ul_op.obj_info.obj_inst_id = inst_id;
  }
 
  else
  {
    LOG_INFO("Not valid application id.");
      tx_byte_release(ext_param2);
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
  }

  LOG_INFO("Switching from QCLI to application callback context");

  if(id == 0)
  {
      if((cmd_ptr2 = multi_app_get_cmd_buffer()) == NULL)
    {
      LOG_INFO("Cannot obtain command buffer");
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }
    else
    {
        cmd_ptr2->cmd_data.cmd_id              = EXT_OBJ_APP_ORIGINATED_EVT;
        cmd_ptr2->cmd_data.data               = (void *)ext_param2;

        if(dam_app1_registerd == false)
      {
        QCLI_Printf(lwm2m_ext_handle,"Application %d not registerd ",id+1);
          tx_byte_release(ext_param2);
          result = QCLI_STATUS_SUCCESS_E;
          goto end;
      }
      LOG_INFO("Enqueue the command into the command queue. ");
        multi_app_enqueue_cmd_t1(cmd_ptr2);
    }
  
      multi_app_set_signal_dam_t1();
  }

  else if(id == 1)
  {
      if((cmd_ptr2 = multi_app_get_cmd_buffer()) == NULL)
    {
      LOG_INFO("Cannot obtain command buffer");
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }
    else
    {
        cmd_ptr2->cmd_data.cmd_id              = EXT_OBJ_APP_ORIGINATED_EVT;
        cmd_ptr2->cmd_data.data               = (void *)ext_param2;

        if(dam_app2_registerd == false)
      {
        QCLI_Printf(lwm2m_ext_handle,"Application %d not registerd ",id+1);
          tx_byte_release(ext_param2);
          result = QCLI_STATUS_SUCCESS_E;
          goto end;
      }
      LOG_INFO("Enqueue the command into the command queue. ");
        multi_app_enqueue_cmd_t2(cmd_ptr2);
    }
  
      multi_app_set_signal_dam_t2();
  }

  else
  {
    LOG_INFO("Application id not valid.");
  }
  }

 end:
  if (cmd_parser_temp->app_data1.app_id != 0)
  {
    if (cmd_parser_temp->app_data1.Parameter_List != NULL)
    {
      for (i = 0; i< cmd_parser_temp->app_data1.Parameter_Count; i++)
      {
        if (cmd_parser_temp->app_data1.Parameter_List[i].String_Value != NULL)
        {
          tx_byte_release(cmd_parser_temp->app_data1.Parameter_List[i].String_Value);
          cmd_parser_temp->app_data1.Parameter_List[i].String_Value = NULL;
        }
      }
    }
  }

  if (cmd_parser_temp->app_data2.app_id != 0)
  {
    if (cmd_parser_temp->app_data2.Parameter_List != NULL)
    {
      for (i = 0; i< cmd_parser_temp->app_data2.Parameter_Count; i++)
      {
        if (cmd_parser_temp->app_data2.Parameter_List[i].String_Value != NULL)
        {
          tx_byte_release(cmd_parser_temp->app_data2.Parameter_List[i].String_Value);
          cmd_parser_temp->app_data2.Parameter_List[i].String_Value = NULL;
        }
      }
    }
  }

  tx_byte_release(cmd_parser_temp);

  return result;
}

QCLI_Command_Status_t set_obj_link (uint32_t Parameter_Count,
                                         QCLI_Parameter_t *Parameter_List)
{
  int id = -1;
  uint16_t obj_id = 0xFFFF;
  uint16_t inst_id = 0xFFFF;

  int i;

  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;

  qapi_Net_LWM2M_Ext_t *ext_param1 = NULL;
  qapi_Net_LWM2M_Ext_t *ext_param2 = NULL;

  multi_app_cmd_t* cmd_ptr1 = NULL;
  multi_app_cmd_t* cmd_ptr2 = NULL;

  multi_app_cmd_parser_t *cmd_parser_temp = NULL;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, true);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(lwm2m_ext_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  if (cmd_parser_temp->app_data1.app_id != 0)
  {
    id = cmd_parser_temp->app_data1.app_id - 1;

    if(cmd_parser_temp->app_data1.Parameter_Count < 3 || cmd_parser_temp->app_data1.Parameter_List == NULL)
  {
      result = QCLI_STATUS_USAGE_E;
      goto end;
  }

    if (cmd_parser_temp->app_data1.Parameter_List[1].Integer_Is_Valid)
  {
      obj_id = cmd_parser_temp->app_data1.Parameter_List[1].Integer_Value;
  }

    if (cmd_parser_temp->app_data1.Parameter_List[2].Integer_Is_Valid)
  {
      inst_id = cmd_parser_temp->app_data1.Parameter_List[2].Integer_Value;
  }

    tx_byte_allocate(byte_pool_qcli, (VOID **)&ext_param1, sizeof(qapi_Net_LWM2M_Ext_t), TX_NO_WAIT);

    if(ext_param1 == NULL)
  {
      LOG_INFO("Cannot assign memory for extensible object parameter");
      result = QCLI_STATUS_ERROR_E;
      goto end;
  }

    memset(ext_param1, 0x00, sizeof(qapi_Net_LWM2M_Ext_Obj_t));

    if(id == 0 || id == 1)
    {
      ext_param1->app_data.ul_op.id = id;
      ext_param1->app_data.ul_op.msg_type = QAPI_NET_LWM2M_EXT_SET_OBJ_LINK_E;
      ext_param1->app_data.ul_op.obj_info.obj_mask = (qapi_Net_LWM2M_ID_t)0x03;
  
      ext_param1->app_data.ul_op.obj_info.obj_id = obj_id;
      ext_param1->app_data.ul_op.obj_info.obj_inst_id = inst_id;
    }

    else
  {
      LOG_INFO("Not valid application id.");
      tx_byte_release(ext_param1);
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
    }

    LOG_INFO("Switching from QCLI to application callback context");

    if (id == 0)
    {
      if((cmd_ptr1 = multi_app_get_cmd_buffer()) == NULL)
      {
        LOG_INFO("Cannot obtain command buffer");
        result = QCLI_STATUS_ERROR_E;
        goto end;
  }
      else
      {
        cmd_ptr1->cmd_data.cmd_id              = EXT_OBJ_APP_ORIGINATED_EVT;
        cmd_ptr1->cmd_data.data               = (void *)ext_param1;

        if(dam_app1_registerd == false)
        {
          QCLI_Printf(lwm2m_ext_handle,"Application %d not registerd ",id+1);
          tx_byte_release(ext_param1);
          result = QCLI_STATUS_SUCCESS_E;
          goto end;
        }
        LOG_INFO("Enqueue the command into the command queue. ");
        multi_app_enqueue_cmd_t1(cmd_ptr1);
      }

      multi_app_set_signal_dam_t1();
    }

    else if(id == 1)
    {
      if((cmd_ptr1 = multi_app_get_cmd_buffer()) == NULL)
      {
        LOG_INFO("Cannot obtain command buffer");
        result = QCLI_STATUS_ERROR_E;
        goto end;
      }
      else
      {
        cmd_ptr1->cmd_data.cmd_id              = EXT_OBJ_APP_ORIGINATED_EVT;
        cmd_ptr1->cmd_data.data               = (void *)ext_param1;

        if(dam_app2_registerd == false)
        {
          QCLI_Printf(lwm2m_ext_handle,"Application %d not registerd ",id+1);
          tx_byte_release(ext_param1);
          result = QCLI_STATUS_SUCCESS_E;
          goto end;
        }
        LOG_INFO("Enqueue the command into the command queue. ");
        multi_app_enqueue_cmd_t2(cmd_ptr1);
      }

      multi_app_set_signal_dam_t2();
    }

    else
    {
      LOG_INFO("Application id not valid.");
    }
  }

    
  if (cmd_parser_temp->app_data2.app_id != 0)
  {
    id = cmd_parser_temp->app_data2.app_id - 1;

    if(cmd_parser_temp->app_data2.Parameter_Count < 3 || cmd_parser_temp->app_data2.Parameter_List == NULL)
    {
      result = QCLI_STATUS_USAGE_E;
      goto end;
    }

    if (cmd_parser_temp->app_data2.Parameter_List[1].Integer_Is_Valid)
    {
      obj_id = cmd_parser_temp->app_data2.Parameter_List[1].Integer_Value;
    }

    if (cmd_parser_temp->app_data2.Parameter_List[2].Integer_Is_Valid)
    {
      inst_id = cmd_parser_temp->app_data2.Parameter_List[2].Integer_Value;
    }


    tx_byte_allocate(byte_pool_qcli, (VOID **)&ext_param2, sizeof(qapi_Net_LWM2M_Ext_t), TX_NO_WAIT);

    if(ext_param2 == NULL)
    {
      LOG_INFO("Cannot assign memory for extensible object parameter");
      result = QCLI_STATUS_ERROR_E;
      goto end;
    }

    memset(ext_param2, 0x00, sizeof(qapi_Net_LWM2M_Ext_Obj_t));

    if(id == 0 || id == 2)
    {
      ext_param2->app_data.ul_op.id = id;
      ext_param2->app_data.ul_op.msg_type = QAPI_NET_LWM2M_EXT_SET_OBJ_LINK_E;
      ext_param2->app_data.ul_op.obj_info.obj_mask = (qapi_Net_LWM2M_ID_t)0x03;
  
      ext_param2->app_data.ul_op.obj_info.obj_id = obj_id;
      ext_param2->app_data.ul_op.obj_info.obj_inst_id = inst_id;
  }
  else
  {
    LOG_INFO("Not valid application id.");
      tx_byte_release(ext_param2);
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
  }
    
  LOG_INFO("Switching from QCLI to application callback context");

  if (id == 0)
  {
      if((cmd_ptr2 = multi_app_get_cmd_buffer()) == NULL)
    {
      LOG_INFO("Cannot obtain command buffer");
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }
    else
    {
        cmd_ptr2->cmd_data.cmd_id              = EXT_OBJ_APP_ORIGINATED_EVT;
        cmd_ptr2->cmd_data.data               = (void *)ext_param2;

        if (dam_app1_registerd == false)
      {
        QCLI_Printf(lwm2m_ext_handle,"Application %d not registerd ",id+1);
          tx_byte_release(ext_param2);
          result = QCLI_STATUS_SUCCESS_E;
          goto end;
      }

      LOG_INFO("Enqueue the command into the command queue. ");
        multi_app_enqueue_cmd_t1(cmd_ptr2);
    }
  
      multi_app_set_signal_dam_t1();
  }
  else if (id == 1)
  {
      if((cmd_ptr2 = multi_app_get_cmd_buffer()) == NULL)
    {
      LOG_INFO("Cannot obtain command buffer");
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }
    else
    {
        cmd_ptr2->cmd_data.cmd_id              = EXT_OBJ_APP_ORIGINATED_EVT;
        cmd_ptr2->cmd_data.data               = (void *)ext_param2;

        if (dam_app2_registerd == false)
      {
        QCLI_Printf(lwm2m_ext_handle,"Application %d not registerd ",id+1);
          tx_byte_release(ext_param2);
          result = QCLI_STATUS_SUCCESS_E;
          goto end;
      }
      LOG_INFO("Enqueue the command into the command queue. ");
        multi_app_enqueue_cmd_t2(cmd_ptr2);
    }
  
      multi_app_set_signal_dam_t2();
  }

  else
  {
    LOG_INFO("Application id not valid.");
  }
  }

 end:
  if (cmd_parser_temp->app_data1.app_id != 0)
  {
    if (cmd_parser_temp->app_data1.Parameter_List != NULL)
    {
      for (i = 0; i< cmd_parser_temp->app_data1.Parameter_Count; i++)
      {
        if (cmd_parser_temp->app_data1.Parameter_List[i].String_Value != NULL)
        {
          tx_byte_release(cmd_parser_temp->app_data1.Parameter_List[i].String_Value);
          cmd_parser_temp->app_data1.Parameter_List[i].String_Value = NULL;
        }
      }
    }
  }

  if (cmd_parser_temp->app_data2.app_id != 0)
  {
    if (cmd_parser_temp->app_data2.Parameter_List != NULL)
    {
      for (i = 0; i< cmd_parser_temp->app_data2.Parameter_Count; i++)
      {
        if (cmd_parser_temp->app_data2.Parameter_List[i].String_Value != NULL)
        {
          tx_byte_release(cmd_parser_temp->app_data2.Parameter_List[i].String_Value);
          cmd_parser_temp->app_data2.Parameter_List[i].String_Value = NULL;
        }
      }
    }
  }

  tx_byte_release(cmd_parser_temp);

  return result;
}


QCLI_Command_Status_t get_obj_link (uint32_t Parameter_Count,
                                         QCLI_Parameter_t *Parameter_List)
{
  int id = -1;

  int i;

  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;
  qapi_Net_LWM2M_Ext_t *ext_param1 = NULL;
  qapi_Net_LWM2M_Ext_t *ext_param2 = NULL;

  multi_app_cmd_t* cmd_ptr1 = NULL;
  multi_app_cmd_t* cmd_ptr2 = NULL;

  multi_app_cmd_parser_t *cmd_parser_temp = NULL;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, true);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(lwm2m_ext_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  if (cmd_parser_temp->app_data1.app_id != 0)
  {
    if(cmd_parser_temp->app_data1.Parameter_Count < 1 || cmd_parser_temp->app_data1.Parameter_List == NULL)
    {
      result = QCLI_STATUS_USAGE_E;
      goto end;
    }

    id = cmd_parser_temp->app_data1.app_id - 1;

    tx_byte_allocate(byte_pool_qcli, (VOID **)&ext_param1, sizeof(qapi_Net_LWM2M_Ext_t), TX_NO_WAIT);

    if(ext_param1 == NULL)
    {
      LOG_INFO("Cannot assign memory for extensible object parameter");
      result = QCLI_STATUS_ERROR_E;
      goto end;
  }

    memset(ext_param1, 0x00, sizeof(qapi_Net_LWM2M_Ext_t));

    if(id == 0 || id == 1)
  {
      ext_param1->app_data.ul_op.id = id;
      ext_param1->app_data.ul_op.msg_type = QAPI_NET_LWM2M_EXT_GET_OBJ_LINK_E;
  }
    else
  {
      LOG_INFO("Not valid application id.");
      tx_byte_release(ext_param1);
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
    }

    LOG_INFO("Switching from QCLI to application callback context");

    if (id == 0)
    {
      if((cmd_ptr1 = multi_app_get_cmd_buffer()) == NULL)
      {
        LOG_INFO("Cannot obtain command buffer");
        result = QCLI_STATUS_ERROR_E;
        goto end;
  }
      else
      {
        cmd_ptr1->cmd_data.cmd_id              = EXT_OBJ_APP_ORIGINATED_EVT;
        cmd_ptr1->cmd_data.data               = (void *)ext_param1;

        if(dam_app1_registerd == false)
        {
          QCLI_Printf(lwm2m_ext_handle,"Application %d not registerd ",id+1);
          tx_byte_release(ext_param1);
          result =  QCLI_STATUS_SUCCESS_E;
          goto end;
        }
        LOG_INFO("Enqueue the command into the command queue. ");
        multi_app_enqueue_cmd_t1(cmd_ptr1);
      }

      multi_app_set_signal_dam_t1();
    }

    else if(id == 1)
    {
      if((cmd_ptr1 = multi_app_get_cmd_buffer()) == NULL)
      {
        LOG_INFO("Cannot obtain command buffer");
        result = QCLI_STATUS_ERROR_E;
        goto end;
      }
      else
  {
        cmd_ptr1->cmd_data.cmd_id              = EXT_OBJ_APP_ORIGINATED_EVT;
        cmd_ptr1->cmd_data.data               = (void *)ext_param1;

        if(dam_app2_registerd == false)
        {
          QCLI_Printf(lwm2m_ext_handle,"Application %d not registerd ",id+1);
          tx_byte_release(ext_param1);
          result = QCLI_STATUS_SUCCESS_E;
          goto end;
        }
        LOG_INFO("Enqueue the command into the command queue. ");
        multi_app_enqueue_cmd_t2(cmd_ptr1);
  }

      multi_app_set_signal_dam_t2();
    }

  else
  {
      LOG_INFO("Application id not valid.");
    }
  }

  if (cmd_parser_temp->app_data2.app_id != 0)
  {

    if(cmd_parser_temp->app_data2.Parameter_Count < 1 || cmd_parser_temp->app_data2.Parameter_List == NULL)
    {
      result = QCLI_STATUS_USAGE_E;
      goto end;
    }

    id = cmd_parser_temp->app_data2.app_id - 1;

    tx_byte_allocate(byte_pool_qcli, (VOID **)&ext_param2, sizeof(qapi_Net_LWM2M_Ext_t), TX_NO_WAIT);

    if(ext_param2 == NULL)
    {
      LOG_INFO("Cannot assign memory for extensible object parameter");
      result = QCLI_STATUS_ERROR_E;
      goto end;
    }

    memset(ext_param2, 0x00, sizeof(qapi_Net_LWM2M_Ext_t));

    if(cmd_parser_temp->app_data2.app_id == 1 || cmd_parser_temp->app_data2.app_id == 2)
    {
      ext_param2->app_data.ul_op.id = id;
      ext_param2->app_data.ul_op.msg_type = QAPI_NET_LWM2M_EXT_GET_OBJ_LINK_E;
    }

    else
    {
      LOG_INFO("Not valid application id.");
      tx_byte_release(ext_param2);
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
  }
    
  LOG_INFO("Switching from QCLI to application callback context");

  if (id == 0)
  {
      if((cmd_ptr2 = multi_app_get_cmd_buffer()) == NULL)
    {
      LOG_INFO("Cannot obtain command buffer");
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }
    else
    {
        cmd_ptr2->cmd_data.cmd_id              = EXT_OBJ_APP_ORIGINATED_EVT;
        cmd_ptr2->cmd_data.data               = (void *)ext_param2;

        if (dam_app1_registerd == false)
      {
        QCLI_Printf(lwm2m_ext_handle,"Application %d not registerd ", id+1);
          tx_byte_release(ext_param2);
          result =  QCLI_STATUS_SUCCESS_E;
          goto end;
      }
      LOG_INFO("Enqueue the command into the command queue. ");
        multi_app_enqueue_cmd_t1(cmd_ptr2);
    }
  
      multi_app_set_signal_dam_t1();
  }
  else if (id == 1)
  {
      if((cmd_ptr2 = multi_app_get_cmd_buffer()) == NULL)
    {
      LOG_INFO("Cannot obtain command buffer");
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }
    else
    {
        cmd_ptr2->cmd_data.cmd_id              = EXT_OBJ_APP_ORIGINATED_EVT;
        cmd_ptr2->cmd_data.data               = (void *)ext_param2;

        if(dam_app2_registerd == false)
      {
        QCLI_Printf(lwm2m_ext_handle,"Application %d not registerd ",id+1);
          tx_byte_release(ext_param2);
          result = QCLI_STATUS_SUCCESS_E;
          goto end;
      }
      LOG_INFO("Enqueue the command into the command queue. ");
        multi_app_enqueue_cmd_t2(cmd_ptr2);
    }
  
      multi_app_set_signal_dam_t2();
  }
  else
  {
    LOG_INFO("Application id not valid.");
  }
  }

 end:
  if (cmd_parser_temp->app_data1.app_id != 0)
  {
    if (cmd_parser_temp->app_data1.Parameter_List != NULL)
    {
      for (i = 0; i< cmd_parser_temp->app_data1.Parameter_Count; i++)
      {
        if (cmd_parser_temp->app_data1.Parameter_List[i].String_Value != NULL)
        {
          tx_byte_release(cmd_parser_temp->app_data1.Parameter_List[i].String_Value);
          cmd_parser_temp->app_data1.Parameter_List[i].String_Value = NULL;
        }
      }
    }
  }

  if (cmd_parser_temp->app_data2.app_id != 0)
  {
    if (cmd_parser_temp->app_data2.Parameter_List != NULL)
    {
      for (i = 0; i< cmd_parser_temp->app_data2.Parameter_Count; i++)
      {
        if (cmd_parser_temp->app_data2.Parameter_List[i].String_Value != NULL)
        {
          tx_byte_release(cmd_parser_temp->app_data2.Parameter_List[i].String_Value);
          cmd_parser_temp->app_data2.Parameter_List[i].String_Value = NULL;
        }
      }
    }
  }

  tx_byte_release(cmd_parser_temp);

  return result;
}


QCLI_Command_Status_t set_server_lifetime (uint32_t Parameter_Count,
                                                   QCLI_Parameter_t *Parameter_List)
{  
  int id = -1;

  int i;

  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;
  uint8_t *url = NULL;
  uint32_t lifetime = 0x00;
  qapi_Net_LWM2M_Ext_t *ext_param1 = NULL;
  qapi_Net_LWM2M_Ext_t *ext_param2 = NULL;

  multi_app_cmd_t* cmd_ptr1 = NULL;
  multi_app_cmd_t* cmd_ptr2 = NULL;

  multi_app_cmd_parser_t *cmd_parser_temp = NULL;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, true);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(lwm2m_ext_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  if (cmd_parser_temp->app_data1.app_id != 0)
  {
    id = cmd_parser_temp->app_data1.app_id - 1;

    if(cmd_parser_temp->app_data1.Parameter_Count < 3 || cmd_parser_temp->app_data1.Parameter_List == NULL)
    {
      result = QCLI_STATUS_USAGE_E;
      goto end;
  }

    url = cmd_parser_temp->app_data1.Parameter_List[1].String_Value;
 
    if (cmd_parser_temp->app_data1.Parameter_List[2].Integer_Is_Valid)
  {
      lifetime = cmd_parser_temp->app_data1.Parameter_List[2].Integer_Value;
  }

    tx_byte_allocate(byte_pool_qcli, (VOID **)&ext_param1, sizeof(qapi_Net_LWM2M_Ext_t), TX_NO_WAIT);

    if(ext_param1 == NULL)
  {
    LOG_INFO("Cannot assign memory for extensible object parameter");
      result = QCLI_STATUS_ERROR_E;
      goto end;
  }

    memset(ext_param1, 0x00, sizeof(qapi_Net_LWM2M_Ext_Obj_t));

    if(id == 0 || id == 1)
    {
      ext_param1->app_data.ul_op.id = id;
      ext_param1->app_data.ul_op.msg_type = QAPI_NET_LWM2M_EXT_SET_SRV_LIFETIME_E;

      ext_param1->app_data.ul_op.lifetime = lifetime; 
      strlcpy((void *)ext_param1->app_data.ul_op.url_info, (void *)url, URL_INFO_BUFFER_SIZE);
    }

    else
    {
      LOG_INFO("Not valid application id.");
      tx_byte_release(ext_param1);
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
    }

    LOG_INFO("Switching from QCLI to application callback context");

    if (id == 0)
    {
      if((cmd_ptr1 = multi_app_get_cmd_buffer()) == NULL)
      {
        LOG_INFO("Cannot obtain command buffer");
        result = QCLI_STATUS_ERROR_E;
        goto end;
      }
      else
  {
        cmd_ptr1->cmd_data.cmd_id              = EXT_OBJ_APP_ORIGINATED_EVT;
        cmd_ptr1->cmd_data.data               = (void *)ext_param1;

        if(dam_app1_registerd == false)
        {
          QCLI_Printf(lwm2m_ext_handle,"Application %d not registerd ",id+1);
          tx_byte_release(ext_param1);
          result = QCLI_STATUS_SUCCESS_E;
          goto end;
        }
        LOG_INFO("Enqueue the command into the command queue. ");
        multi_app_enqueue_cmd_t1(cmd_ptr1);
      }

      multi_app_set_signal_dam_t1();
    }

    else if(id == 1)
    {
      if((cmd_ptr1 = multi_app_get_cmd_buffer()) == NULL)
      {
        LOG_INFO("Cannot obtain command buffer");
        result = QCLI_STATUS_ERROR_E;
        goto end;
      }
      else
      {
        cmd_ptr1->cmd_data.cmd_id              = EXT_OBJ_APP_ORIGINATED_EVT;
        cmd_ptr1->cmd_data.data               = (void *)ext_param1;

        if(dam_app2_registerd == false)
        {
          QCLI_Printf(lwm2m_ext_handle,"Application %d not registerd ",id+1);
          tx_byte_release(ext_param1);
          result = QCLI_STATUS_SUCCESS_E;
          goto end;
        }
        LOG_INFO("Enqueue the command into the command queue. ");
        multi_app_enqueue_cmd_t2(cmd_ptr1);
    }

      multi_app_set_signal_dam_t2();
    }

    else
    {
      LOG_INFO("Application id not valid.");
    }
  }

  if (cmd_parser_temp->app_data2.app_id != 0)
  {
    id = cmd_parser_temp->app_data2.app_id - 1;

    if(cmd_parser_temp->app_data2.Parameter_Count < 3 || cmd_parser_temp->app_data2.Parameter_List == NULL)
    {
      result = QCLI_STATUS_USAGE_E;
      goto end;
    }

    url = cmd_parser_temp->app_data2.Parameter_List[1].String_Value;

    if (cmd_parser_temp->app_data2.Parameter_List[2].Integer_Is_Valid)
    {
      lifetime = cmd_parser_temp->app_data2.Parameter_List[2].Integer_Value;
    }

    tx_byte_allocate(byte_pool_qcli, (VOID **)&ext_param1, sizeof(qapi_Net_LWM2M_Ext_t), TX_NO_WAIT);

    if(ext_param2 == NULL)
    {
      LOG_INFO("Cannot assign memory for extensible object parameter");
      result = QCLI_STATUS_ERROR_E;
      goto end;
    }

    memset(ext_param2, 0x00, sizeof(qapi_Net_LWM2M_Ext_Obj_t));

    if(id == 0 || id == 2)
    {
      ext_param2->app_data.ul_op.id = id;
      ext_param2->app_data.ul_op.msg_type = QAPI_NET_LWM2M_EXT_SET_SRV_LIFETIME_E;

      ext_param2->app_data.ul_op.lifetime = lifetime; 
      strlcpy((void *)ext_param2->app_data.ul_op.url_info, (void *)url, URL_INFO_BUFFER_SIZE);
  }
  else
  {
    LOG_INFO("Not valid application id.");
      tx_byte_release(ext_param2);
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
  }
    
  LOG_INFO("Switching from QCLI to application callback context");

  if (id == 0)
  {
      if((cmd_ptr2 = multi_app_get_cmd_buffer()) == NULL)
    {
      LOG_INFO("Cannot obtain command buffer");
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }
    else
    {
        cmd_ptr2->cmd_data.cmd_id              = EXT_OBJ_APP_ORIGINATED_EVT;
        cmd_ptr2->cmd_data.data               = (void *)ext_param2;

        if (dam_app1_registerd == false)
      {
        QCLI_Printf(lwm2m_ext_handle,"Application %d not registerd ",id+1);
          tx_byte_release(ext_param2);
          result = QCLI_STATUS_SUCCESS_E;
          goto end;
      }
      LOG_INFO("Enqueue the command into the command queue. ");
        multi_app_enqueue_cmd_t1(cmd_ptr2);
    }
  
      multi_app_set_signal_dam_t1();
  }
  else if (id == 1)
  {
      if((cmd_ptr2 = multi_app_get_cmd_buffer()) == NULL)
    {
      LOG_INFO("Cannot obtain command buffer");
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }
    else
    {
        cmd_ptr2->cmd_data.cmd_id              = EXT_OBJ_APP_ORIGINATED_EVT;
        cmd_ptr2->cmd_data.data               = (void *)ext_param2;

        if (dam_app2_registerd == false)
      {
        QCLI_Printf(lwm2m_ext_handle,"Application %d not registerd ",id+1);
          tx_byte_release(ext_param2);
          result = QCLI_STATUS_SUCCESS_E;
          goto end;
      }
      LOG_INFO("Enqueue the command into the command queue. ");
        multi_app_enqueue_cmd_t2(cmd_ptr2);
    }
  
      multi_app_set_signal_dam_t2();
  }

  else
  {
    LOG_INFO("Application id not valid.");
  }
  }

 end:
  if (cmd_parser_temp->app_data1.app_id != 0)
  {
    if (cmd_parser_temp->app_data1.Parameter_List != NULL)
    {
      for (i = 0; i< cmd_parser_temp->app_data1.Parameter_Count; i++)
      {
        if (cmd_parser_temp->app_data1.Parameter_List[i].String_Value != NULL)
        {
          tx_byte_release(cmd_parser_temp->app_data1.Parameter_List[i].String_Value);
          cmd_parser_temp->app_data1.Parameter_List[i].String_Value = NULL;
        }
      }
    }
  }

  if (cmd_parser_temp->app_data2.app_id != 0)
  {
    if (cmd_parser_temp->app_data2.Parameter_List != NULL)
    {
      for (i = 0; i< cmd_parser_temp->app_data2.Parameter_Count; i++)
      {
        if (cmd_parser_temp->app_data2.Parameter_List[i].String_Value != NULL)
        {
          tx_byte_release(cmd_parser_temp->app_data2.Parameter_List[i].String_Value);
          cmd_parser_temp->app_data2.Parameter_List[i].String_Value = NULL;
        }
      }
    }
  }

  tx_byte_release(cmd_parser_temp);

  return result;
}

QCLI_Command_Status_t get_server_lifetime (uint32_t Parameter_Count,
                                                   QCLI_Parameter_t *Parameter_List)
{  
  int id = -1;
  uint8_t *url = NULL;

  int i;

  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;
  qapi_Net_LWM2M_Ext_t *ext_param1 = NULL;
  qapi_Net_LWM2M_Ext_t *ext_param2 = NULL;

  multi_app_cmd_t* cmd_ptr1 = NULL;
  multi_app_cmd_t* cmd_ptr2 = NULL;

  multi_app_cmd_parser_t *cmd_parser_temp = NULL;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, true);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(lwm2m_ext_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  if (cmd_parser_temp->app_data1.app_id != 0)
  {
    id = cmd_parser_temp->app_data1.app_id - 1;

    if(cmd_parser_temp->app_data1.Parameter_Count < 2 || cmd_parser_temp->app_data1.Parameter_List == NULL)
  {
      result = QCLI_STATUS_USAGE_E;
      goto end;
  }

    url = cmd_parser_temp->app_data1.Parameter_List[1].String_Value;

    tx_byte_allocate(byte_pool_qcli, (VOID **)&ext_param1, sizeof(qapi_Net_LWM2M_Ext_t), TX_NO_WAIT);

    if(ext_param1 == NULL)
  {
      LOG_INFO("Cannot assign memory for extensible object parameter");
      result = QCLI_STATUS_ERROR_E;
      goto end;
  }

    memset(ext_param1, 0x00, sizeof(qapi_Net_LWM2M_Ext_Obj_t));

    if(id == 0 || id == 1)
    {
      ext_param1->app_data.ul_op.id = id;
      ext_param1->app_data.ul_op.msg_type = QAPI_NET_LWM2M_EXT_GET_SRV_LIFETIME_E;
    
      strlcpy((void *)ext_param1->app_data.ul_op.url_info, (void *)url, URL_INFO_BUFFER_SIZE);
    }

    else
  {
      LOG_INFO("Not valid application id.");
      tx_byte_release(ext_param1);
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
    }

    LOG_INFO("Switching from QCLI to application callback context");

    if (id == 0)
    {
      if((cmd_ptr1 = multi_app_get_cmd_buffer()) == NULL)
      {
        LOG_INFO("Cannot obtain command buffer");
        result = QCLI_STATUS_ERROR_E;
        goto end;
  }
      else
      {
        cmd_ptr1->cmd_data.cmd_id              = EXT_OBJ_APP_ORIGINATED_EVT;
        cmd_ptr1->cmd_data.data               = (void *)ext_param1;

        if(dam_app1_registerd == false)
        {
          QCLI_Printf(lwm2m_ext_handle,"Application %d not registerd ",id+1);
          tx_byte_release(ext_param1);
          result = QCLI_STATUS_SUCCESS_E;
          goto end;
        }
        LOG_INFO("Enqueue the command into the command queue. ");
        multi_app_enqueue_cmd_t1(cmd_ptr1);
      }

      multi_app_set_signal_dam_t1();
    }

    else if(id == 1)
    {
      if((cmd_ptr1 = multi_app_get_cmd_buffer()) == NULL)
      {
        LOG_INFO("Cannot obtain command buffer");
        result = QCLI_STATUS_ERROR_E;
        goto end;
      }
      else
  {
        cmd_ptr1->cmd_data.cmd_id              = EXT_OBJ_APP_ORIGINATED_EVT;
        cmd_ptr1->cmd_data.data               = (void *)ext_param1;

        if(dam_app2_registerd == false)
        {
          QCLI_Printf(lwm2m_ext_handle,"Application %d not registerd ",id+1);
          tx_byte_release(ext_param1);
          result = QCLI_STATUS_SUCCESS_E;
          goto end;
        }
        LOG_INFO("Enqueue the command into the command queue. ");
        multi_app_enqueue_cmd_t2(cmd_ptr1);
      }

      multi_app_set_signal_dam_t2();
    }

    else
    {
      LOG_INFO("Application id not valid.");
    }
  }

  if (cmd_parser_temp->app_data2.app_id != 0)
  {
    id = cmd_parser_temp->app_data2.app_id - 1;

    if(cmd_parser_temp->app_data2.Parameter_Count < 2 || cmd_parser_temp->app_data2.Parameter_List == NULL)
    {
      result = QCLI_STATUS_USAGE_E;
      goto end;
    }

    url = cmd_parser_temp->app_data2.Parameter_List[1].String_Value;

    tx_byte_allocate(byte_pool_qcli, (VOID **)&ext_param2, sizeof(qapi_Net_LWM2M_Ext_t), TX_NO_WAIT);

    if(ext_param2 == NULL)
    {
      LOG_INFO("Cannot assign memory for extensible object parameter");
      result = QCLI_STATUS_ERROR_E;
      goto end;
    }

    memset(ext_param2, 0x00, sizeof(qapi_Net_LWM2M_Ext_Obj_t));

    if(id == 0 || id == 2)
    {
      ext_param2->app_data.ul_op.id = id;
      ext_param2->app_data.ul_op.msg_type = QAPI_NET_LWM2M_EXT_GET_SRV_LIFETIME_E;
  
      strlcpy((void *)ext_param2->app_data.ul_op.url_info, (void *)url, URL_INFO_BUFFER_SIZE);
  }
  else
  {
    LOG_INFO("Not valid application id.");
      tx_byte_release(ext_param2);
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
  }
    
  LOG_INFO("Switching from QCLI to application callback context");

  if (id == 0)
  {
      if((cmd_ptr2 = multi_app_get_cmd_buffer()) == NULL)
    {
      LOG_INFO("Cannot obtain command buffer");
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }
    else
    {
        cmd_ptr2->cmd_data.cmd_id              = EXT_OBJ_APP_ORIGINATED_EVT;
        cmd_ptr2->cmd_data.data               = (void *)ext_param2;

        if (dam_app1_registerd == false)
      {
        QCLI_Printf(lwm2m_ext_handle,"Application %d not registerd ", id+1);
          tx_byte_release(ext_param2);
          result = QCLI_STATUS_SUCCESS_E;
          goto end;
      }
      LOG_INFO("Enqueue the command into the command queue. ");
        multi_app_enqueue_cmd_t1(cmd_ptr2);
    }
  
      multi_app_set_signal_dam_t1();
  }
  else if (id == 1)
  {
      if((cmd_ptr2 = multi_app_get_cmd_buffer()) == NULL)
    {
      LOG_INFO("Cannot obtain command buffer");
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }
    else
    {
        cmd_ptr2->cmd_data.cmd_id              = EXT_OBJ_APP_ORIGINATED_EVT;
        cmd_ptr2->cmd_data.data               = (void *)ext_param2;

        if(dam_app2_registerd == false)
      {
        QCLI_Printf(lwm2m_ext_handle,"Application %d not registerd ",id+1);
          tx_byte_release(ext_param2);
          result = QCLI_STATUS_SUCCESS_E;
          goto end;
      }
      LOG_INFO("Enqueue the command into the command queue. ");
        multi_app_enqueue_cmd_t2(cmd_ptr2);
    }
  
      multi_app_set_signal_dam_t2();
  }

  else
  {
    LOG_INFO("Application id not valid.");
  }
  }

 end:
  if (cmd_parser_temp->app_data1.app_id != 0)
  {
    if (cmd_parser_temp->app_data1.Parameter_List != NULL)
    {
      for (i = 0; i< cmd_parser_temp->app_data1.Parameter_Count; i++)
      {
        if (cmd_parser_temp->app_data1.Parameter_List[i].String_Value != NULL)
        {
          tx_byte_release(cmd_parser_temp->app_data1.Parameter_List[i].String_Value);
          cmd_parser_temp->app_data1.Parameter_List[i].String_Value = NULL;
        }
      }
    }
  }

  if (cmd_parser_temp->app_data2.app_id != 0)
  {
    if (cmd_parser_temp->app_data2.Parameter_List != NULL)
    {
      for (i = 0; i< cmd_parser_temp->app_data2.Parameter_Count; i++)
      {
        if (cmd_parser_temp->app_data2.Parameter_List[i].String_Value != NULL)
        {
          tx_byte_release(cmd_parser_temp->app_data2.Parameter_List[i].String_Value);
          cmd_parser_temp->app_data2.Parameter_List[i].String_Value = NULL;
        }
      }
    }
  }

  tx_byte_release(cmd_parser_temp);

  return result;
}

QCLI_Command_Status_t ext_read(uint32_t Parameter_Count,
                               QCLI_Parameter_t *Parameter_List)
{
  int id = -1;
  int inst_id = -1;
  int res_id  = -1;
  int obj_id = -1;

  int i;

  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;

  qapi_Net_LWM2M_Data_t *object1 = NULL;
  qapi_Net_LWM2M_Instance_Info_t *instance1 = NULL;
  qapi_Net_LWM2M_Instance_Info_t *instance_start1 = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource1 = NULL;

  qapi_Net_LWM2M_Data_t *object2 = NULL;
  qapi_Net_LWM2M_Instance_Info_t *instance2 = NULL;
  qapi_Net_LWM2M_Instance_Info_t *instance_start2 = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource2 = NULL;

  uint16_t resList1[] = {
                        RES_M_SENSOR_VALUE,
                        RES_O_SENSOR_UNITS,
                        RES_O_MIN_MEASURED_VAL,
                        RES_O_MAX_MEASURED_VAL,
                        RES_M_MIN_RANGE_VAL,
                        RES_M_MAX_RANGE_VAL,
                        RES_M_SENSOR_STATE,
                        RES_O_MEAN_VAL
                        };
  uint16_t resList2[] = {
                        RES_M_DIG_INPUT_STATE,
                        RES_O_DIG_INPUT_COUNTER,
                        RES_O_SENSOR_TYPE,
                        RES_O_BUSY_TO_CLEAR_DELAY,
                        RES_O_CLEAR_TO_BUSY_DELAY,
                        };
  int nRes;

  multi_app_cmd_parser_t *cmd_parser_temp = NULL;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, true);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(lwm2m_ext_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  if (cmd_parser_temp->app_data1.app_id != 0)
  {
    id = cmd_parser_temp->app_data1.app_id - 1;

    if(cmd_parser_temp->app_data1.Parameter_Count < 2 || cmd_parser_temp->app_data1.Parameter_List == NULL)
  {
      result = QCLI_STATUS_USAGE_E;
      goto end;
  }

    if (cmd_parser_temp->app_data1.Parameter_List[1].Integer_Is_Valid)
    {
      obj_id = cmd_parser_temp->app_data1.Parameter_List[1].Integer_Value;
    }

    if ((cmd_parser_temp->app_data1.Parameter_Count > 1) && cmd_parser_temp->app_data1.Parameter_List[1].Integer_Is_Valid)
      {
      inst_id = cmd_parser_temp->app_data1.Parameter_List[1].Integer_Value;
      }

    if ((cmd_parser_temp->app_data1.Parameter_Count > 2) && cmd_parser_temp->app_data1.Parameter_List[2].Integer_Is_Valid)
    {
      res_id = cmd_parser_temp->app_data1.Parameter_List[2].Integer_Value;
    }

    if(id == 0)
    {
      if(ext_object_handler_app1 == NULL)
      {
        QCLI_Printf(lwm2m_ext_handle, "Application not registered.\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
      }

      if(object_generic == NULL)
  {
        QCLI_Printf(lwm2m_ext_handle, "Object not created for application.\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
      }

      obj_id = LWM2M_GENERIC_SENSOR_OBJECT_ID;

      object1 = lwm2m_object_find(object_generic, obj_id);
    }

    else if(id == 1)
    {
      if(ext_object_handler_app2 == NULL)
      {
        QCLI_Printf(lwm2m_ext_handle, "Application not registered.\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
      }

      if(object_presence== NULL)
    {
        QCLI_Printf(lwm2m_ext_handle, "Object not created for application.\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
  }

      obj_id = LWM2M_PRESENCE_SENSOR_OBJECT_ID;

      object1 = lwm2m_object_find(object_presence, obj_id);
    }

    else
  {
      QCLI_Printf(lwm2m_ext_handle, "Not valid application id");
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
  }

    if(object1 == NULL)
    {
      QCLI_Printf(lwm2m_ext_handle,"Not a valid object id.\n");
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
    }

    if (inst_id != -1)
    {
      instance1 = lwm2m_instance_find(object1->instance_info, inst_id);
      if(instance1 == NULL)
      {
        QCLI_Printf(lwm2m_ext_handle,"Not a valid instance id.\n");
        result = QCLI_STATUS_SUCCESS_E;
        goto end;
      }
  
      if(res_id != -1)
      {
        resource1 = lwm2m_resource_find(instance1->resource_info, res_id);
        if(resource1 == NULL)
        {
          QCLI_Printf(lwm2m_ext_handle,"Not a valid resource id.\n");
          result = QCLI_STATUS_SUCCESS_E;
          goto end;
        }
  
        if(check_readable(obj_id, res_id) == false)
        {
          QCLI_Printf(lwm2m_ext_handle, "Read on resource %d not allowed", res_id);
          result = QCLI_STATUS_SUCCESS_E;
          goto end;
        }
  
        QCLI_Printf(lwm2m_ext_handle, "Read on / %d / %d / %d", obj_id,
                    inst_id, res_id);
        switch(resource1->type)
        {
          case QAPI_NET_LWM2M_TYPE_STRING_E:
             LOG_INFO("Resource %d : Value %s \n",resource1->resource_ID, resource1->value.asBuffer.buffer);
             QCLI_Printf(lwm2m_ext_handle, "Resource %d : Value %s \n",resource1->resource_ID, resource1->value.asBuffer.buffer);
          break;
          case QAPI_NET_LWM2M_TYPE_INTEGER_E:
             LOG_INFO("Resource %d : Value %d \n",resource1->resource_ID, (int)resource1->value.asInteger);
             QCLI_Printf(lwm2m_ext_handle, "Resource %d : Value %d \n",resource1->resource_ID, (int)resource1->value.asInteger);
          break;
          case QAPI_NET_LWM2M_TYPE_FLOAT_E:
             LOG_INFO("Resource %d : Value %d \n",resource1->resource_ID, (int)resource1->value.asFloat);
             QCLI_Printf(lwm2m_ext_handle, "Resource %d : Value %d \n",resource1->resource_ID, (int)resource1->value.asFloat);
          break;
          case QAPI_NET_LWM2M_TYPE_BOOLEAN_E:
             LOG_INFO("Resource %d : Value %d \n",resource1->resource_ID, resource1->value.asBoolean);
             QCLI_Printf(lwm2m_ext_handle, "Resource %d : Value %d \n",resource1->resource_ID, (int)resource1->value.asBoolean);
          break;
        }
  
        result = QCLI_STATUS_SUCCESS_E;
        goto end;
      }
  
      else
      {
        QCLI_Printf(lwm2m_ext_handle, "Read on / %d / %d ", obj_id,
                    inst_id);

        if(obj_id == LWM2M_GENERIC_SENSOR_OBJECT_ID)
  {
          nRes = sizeof(resList1)/sizeof(uint16_t);

          for(i = 0; i<nRes; i++)
          {
            resource1 = lwm2m_resource_find(instance1->resource_info, resList1[i]);
            if(resource1 == NULL)
            {
              QCLI_Printf(lwm2m_ext_handle,"Not a valid resource id.\n");
              result = QCLI_STATUS_SUCCESS_E;
              goto end;
            }

            switch(resource1->type)
            {
              case QAPI_NET_LWM2M_TYPE_STRING_E:
                 LOG_INFO("Resource %d : Value %s \n",resource1->resource_ID, resource1->value.asBuffer.buffer);
                 QCLI_Printf(lwm2m_ext_handle, "Resource %d : Value %s \n",resource1->resource_ID, resource1->value.asBuffer.buffer);
              break;
              case QAPI_NET_LWM2M_TYPE_INTEGER_E:
                 LOG_INFO("Resource %d : Value %d \n",resource1->resource_ID, (int)resource1->value.asInteger);
                 QCLI_Printf(lwm2m_ext_handle, "Resource %d : Value %d \n",resource1->resource_ID, (int)resource1->value.asInteger);
              break;
              case QAPI_NET_LWM2M_TYPE_FLOAT_E:
                 LOG_INFO("Resource %d : Value %d \n",resource1->resource_ID, (int)resource1->value.asFloat);
                 QCLI_Printf(lwm2m_ext_handle, "Resource %d : Value %d \n",resource1->resource_ID, (int)resource1->value.asFloat);
              break;
              case QAPI_NET_LWM2M_TYPE_BOOLEAN_E:
                 LOG_INFO("Resource %d : Value %d \n",resource1->resource_ID, resource1->value.asBoolean);
                 QCLI_Printf(lwm2m_ext_handle, "Resource %d : Value %d \n",resource1->resource_ID, (int)resource1->value.asBoolean);
              break;
            }
          }
          result = QCLI_STATUS_SUCCESS_E;
          goto end;
        }
        else
        {
          nRes = sizeof(resList2)/sizeof(uint16_t);
  
          for(i = 0; i<nRes; i++)
          {
            resource1 = lwm2m_resource_find(instance1->resource_info, resList2[i]);
            if(resource1 == NULL)
            {
              QCLI_Printf(lwm2m_ext_handle,"Not a valid resource id.\n");
              result = QCLI_STATUS_SUCCESS_E;
              goto end;
            }

            switch(resource1->type)
            {
              case QAPI_NET_LWM2M_TYPE_STRING_E:
                 LOG_INFO("Resource %d : Value %s \n",resource1->resource_ID, resource1->value.asBuffer.buffer);
                 QCLI_Printf(lwm2m_ext_handle, "Resource %d : Value %s \n",resource1->resource_ID, resource1->value.asBuffer.buffer);
              break;
              case QAPI_NET_LWM2M_TYPE_INTEGER_E:
                 LOG_INFO("Resource %d : Value %d \n",resource1->resource_ID, (int)resource1->value.asInteger);
                 QCLI_Printf(lwm2m_ext_handle, "Resource %d : Value %d \n",resource1->resource_ID, (int)resource1->value.asInteger);
              break;
              case QAPI_NET_LWM2M_TYPE_FLOAT_E:
                 LOG_INFO("Resource %d : Value %d \n",resource1->resource_ID, (int)resource1->value.asFloat);
                 QCLI_Printf(lwm2m_ext_handle, "Resource %d : Value %d \n",resource1->resource_ID, (int)resource1->value.asFloat);
              break;
              case QAPI_NET_LWM2M_TYPE_BOOLEAN_E:
                 LOG_INFO("Resource %d : Value %d \n",resource1->resource_ID, resource1->value.asBoolean);
                 QCLI_Printf(lwm2m_ext_handle, "Resource %d : Value %d \n",resource1->resource_ID, (int)resource1->value.asBoolean);
              break;
            }
          }
          result = QCLI_STATUS_SUCCESS_E;
          goto end;
        }
      }
    }
    else
    {
      instance_start1 = object1->instance_info;
      while(object1->instance_info != NULL)
      {
        instance1 = lwm2m_instance_find(object1->instance_info, object1->instance_info->instance_ID);
        if(instance1 == NULL)
        {
          QCLI_Printf(lwm2m_ext_handle,"Not a valid instance id.\n");
          result = QCLI_STATUS_SUCCESS_E;
          goto end;
        }

        QCLI_Printf(lwm2m_ext_handle, "Read on / %d / %d ", obj_id,
                    inst_id);

        if(obj_id == LWM2M_GENERIC_SENSOR_OBJECT_ID)
        {
          nRes = sizeof(resList1)/sizeof(uint16_t);
  
          for(i = 0; i<nRes; i++)
          {
            resource1 = lwm2m_resource_find(instance1->resource_info, resList1[i]);
            if(resource1 == NULL)
            {
              QCLI_Printf(lwm2m_ext_handle,"Not a valid resource id.\n");
              result = QCLI_STATUS_SUCCESS_E;
              goto end;
            }
    
            switch(resource1->type)
            {
              case QAPI_NET_LWM2M_TYPE_STRING_E:
                 LOG_INFO("Resource %d : Value %s \n",resource1->resource_ID, resource1->value.asBuffer.buffer);
                 QCLI_Printf(lwm2m_ext_handle, "Resource %d : Value %s \n",resource1->resource_ID, resource1->value.asBuffer.buffer);
              break;
              case QAPI_NET_LWM2M_TYPE_INTEGER_E:
                 LOG_INFO("Resource %d : Value %d \n",resource1->resource_ID, (int)resource1->value.asInteger);
                 QCLI_Printf(lwm2m_ext_handle, "Resource %d : Value %d \n",resource1->resource_ID, (int)resource1->value.asInteger);
              break;
              case QAPI_NET_LWM2M_TYPE_FLOAT_E:
                 LOG_INFO("Resource %d : Value %d \n",resource1->resource_ID, (int)resource1->value.asFloat);
                 QCLI_Printf(lwm2m_ext_handle, "Resource %d : Value %d \n",resource1->resource_ID, (int)resource1->value.asFloat);
              break;
              case QAPI_NET_LWM2M_TYPE_BOOLEAN_E:
                 LOG_INFO("Resource %d : Value %d \n",resource1->resource_ID, resource1->value.asBoolean);
                 QCLI_Printf(lwm2m_ext_handle, "Resource %d : Value %d \n",resource1->resource_ID, (int)resource1->value.asBoolean);
              break;
            }
          }
        }
        else
        {
          nRes = sizeof(resList2)/sizeof(uint16_t);
  
          for(i = 0; i<nRes; i++)
          {
            resource1 = lwm2m_resource_find(instance1->resource_info, resList2[i]);
            if(resource1 == NULL)
            {
              QCLI_Printf(lwm2m_ext_handle,"Not a valid resource id.\n");
              result = QCLI_STATUS_SUCCESS_E;
              goto end;
            }
    
            switch(resource1->type)
            {
              case QAPI_NET_LWM2M_TYPE_STRING_E:
                 LOG_INFO("Resource %d : Value %s \n",resource1->resource_ID, resource1->value.asBuffer.buffer);
                 QCLI_Printf(lwm2m_ext_handle, "Resource %d : Value %s \n",resource1->resource_ID, resource1->value.asBuffer.buffer);
              break;
              case QAPI_NET_LWM2M_TYPE_INTEGER_E:
                 LOG_INFO("Resource %d : Value %d \n",resource1->resource_ID, (int)resource1->value.asInteger);
                 QCLI_Printf(lwm2m_ext_handle, "Resource %d : Value %d \n",resource1->resource_ID, (int)resource1->value.asInteger);
              break;
              case QAPI_NET_LWM2M_TYPE_FLOAT_E:
                 LOG_INFO("Resource %d : Value %d \n",resource1->resource_ID, (int)resource1->value.asFloat);
                 QCLI_Printf(lwm2m_ext_handle, "Resource %d : Value %d \n",resource1->resource_ID, (int)resource1->value.asFloat);
              break;
              case QAPI_NET_LWM2M_TYPE_BOOLEAN_E:
                 LOG_INFO("Resource %d : Value %d \n",resource1->resource_ID, resource1->value.asBoolean);
                 QCLI_Printf(lwm2m_ext_handle, "Resource %d : Value %d \n",resource1->resource_ID, (int)resource1->value.asBoolean);
              break;
            }
          }
        }
        object1->instance_info = object1->instance_info->next;
      }
      object1->instance_info = instance_start1;
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
    }
  }

  if (cmd_parser_temp->app_data2.app_id != 0)
  {
    id = cmd_parser_temp->app_data2.app_id - 1;

    if(cmd_parser_temp->app_data2.Parameter_Count < 2 || cmd_parser_temp->app_data2.Parameter_List == NULL)
    {
      result = QCLI_STATUS_USAGE_E;
      goto end;
  }

    if (cmd_parser_temp->app_data2.Parameter_List[1].Integer_Is_Valid)
    {
      obj_id = cmd_parser_temp->app_data2.Parameter_List[1].Integer_Value;
    }

    if ((cmd_parser_temp->app_data2.Parameter_Count > 1) && cmd_parser_temp->app_data2.Parameter_List[1].Integer_Is_Valid)
    {
      inst_id = cmd_parser_temp->app_data2.Parameter_List[1].Integer_Value;
    }

    if ((cmd_parser_temp->app_data2.Parameter_Count > 2) && cmd_parser_temp->app_data2.Parameter_List[2].Integer_Is_Valid)
  {
      res_id = cmd_parser_temp->app_data2.Parameter_List[2].Integer_Value;
  }

  if(id == 0)
  {
    if(ext_object_handler_app1 == NULL)
    {
      QCLI_Printf(lwm2m_ext_handle, "Application not registered.\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }

    if(object_generic == NULL)
    {
      QCLI_Printf(lwm2m_ext_handle, "Object not created for application.\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }

    obj_id = LWM2M_GENERIC_SENSOR_OBJECT_ID;

      object2 = lwm2m_object_find(object_generic, obj_id);
  }

  else if(id == 1)
  {
    if(ext_object_handler_app2 == NULL)
    {
      QCLI_Printf(lwm2m_ext_handle, "Application not registered.\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }

    if(object_presence== NULL)
    {
      QCLI_Printf(lwm2m_ext_handle, "Object not created for application.\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }

    obj_id = LWM2M_PRESENCE_SENSOR_OBJECT_ID;

      object2 = lwm2m_object_find(object_presence, obj_id);
  }

  else
  {
    QCLI_Printf(lwm2m_ext_handle, "Not valid application id");
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
  }

    if(object2 == NULL)
  {
    QCLI_Printf(lwm2m_ext_handle,"Not a valid object id.\n");
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
  }

    if (inst_id != -1)
  {
      instance2 = lwm2m_instance_find(object2->instance_info, inst_id);
      if(instance2 == NULL)
    {
      QCLI_Printf(lwm2m_ext_handle,"Not a valid instance id.\n");
        result = QCLI_STATUS_SUCCESS_E;
        goto end;
    }
  
    if(res_id != -1)
    {
        resource2 = lwm2m_resource_find(instance2->resource_info, res_id);
        if(resource2 == NULL)
      {
        QCLI_Printf(lwm2m_ext_handle,"Not a valid resource id.\n");
          result = QCLI_STATUS_SUCCESS_E;
          goto end;
      }

      if(check_readable(obj_id, res_id) == false)
      {
        QCLI_Printf(lwm2m_ext_handle, "Read on resource %d not allowed", res_id);
          result = QCLI_STATUS_SUCCESS_E;
          goto end;
      }
  
      QCLI_Printf(lwm2m_ext_handle, "Read on / %d / %d / %d", obj_id,
                  inst_id, res_id);
        switch(resource2->type)
      {
        case QAPI_NET_LWM2M_TYPE_STRING_E:
             LOG_INFO("Resource %d : Value %s \n",resource2->resource_ID, resource2->value.asBuffer.buffer);
             QCLI_Printf(lwm2m_ext_handle, "Resource %d : Value %s \n",resource2->resource_ID, resource2->value.asBuffer.buffer);
        break;
        case QAPI_NET_LWM2M_TYPE_INTEGER_E:
             LOG_INFO("Resource %d : Value %d \n",resource2->resource_ID, (int)resource2->value.asInteger);
             QCLI_Printf(lwm2m_ext_handle, "Resource %d : Value %d \n",resource2->resource_ID, (int)resource2->value.asInteger);
        break;
        case QAPI_NET_LWM2M_TYPE_FLOAT_E:
             LOG_INFO("Resource %d : Value %d \n",resource2->resource_ID, (int)resource2->value.asFloat);
             QCLI_Printf(lwm2m_ext_handle, "Resource %d : Value %d \n",resource2->resource_ID, (int)resource2->value.asFloat);
        break;
        case QAPI_NET_LWM2M_TYPE_BOOLEAN_E:
             LOG_INFO("Resource %d : Value %d \n",resource2->resource_ID, resource2->value.asBoolean);
             QCLI_Printf(lwm2m_ext_handle, "Resource %d : Value %d \n",resource2->resource_ID, (int)resource2->value.asBoolean);
        break;
      }
  
        result = QCLI_STATUS_SUCCESS_E;
        goto end;
    }
  
    else
    {
      QCLI_Printf(lwm2m_ext_handle, "Read on / %d / %d ", obj_id,
                  inst_id);

      if(obj_id == LWM2M_GENERIC_SENSOR_OBJECT_ID)
      {
        nRes = sizeof(resList1)/sizeof(uint16_t);
  
        for(i = 0; i<nRes; i++)
        {
            resource2 = lwm2m_resource_find(instance2->resource_info, resList1[i]);
            if(resource2 == NULL)
          {
            QCLI_Printf(lwm2m_ext_handle,"Not a valid resource id.\n");
              result = QCLI_STATUS_SUCCESS_E;
              goto end;
          }

            switch(resource2->type)
          {
            case QAPI_NET_LWM2M_TYPE_STRING_E:
                 LOG_INFO("Resource %d : Value %s \n",resource2->resource_ID, resource2->value.asBuffer.buffer);
                 QCLI_Printf(lwm2m_ext_handle, "Resource %d : Value %s \n",resource2->resource_ID, resource2->value.asBuffer.buffer);
            break;
            case QAPI_NET_LWM2M_TYPE_INTEGER_E:
                 LOG_INFO("Resource %d : Value %d \n",resource2->resource_ID, (int)resource2->value.asInteger);
                 QCLI_Printf(lwm2m_ext_handle, "Resource %d : Value %d \n",resource2->resource_ID, (int)resource2->value.asInteger);
            break;
            case QAPI_NET_LWM2M_TYPE_FLOAT_E:
                 LOG_INFO("Resource %d : Value %d \n",resource2->resource_ID, (int)resource2->value.asFloat);
                 QCLI_Printf(lwm2m_ext_handle, "Resource %d : Value %d \n",resource2->resource_ID, (int)resource2->value.asFloat);
            break;
            case QAPI_NET_LWM2M_TYPE_BOOLEAN_E:
                 LOG_INFO("Resource %d : Value %d \n",resource2->resource_ID, resource2->value.asBoolean);
                 QCLI_Printf(lwm2m_ext_handle, "Resource %d : Value %d \n",resource2->resource_ID, (int)resource2->value.asBoolean);
            break;
          }
        }
          result = QCLI_STATUS_SUCCESS_E;
          goto end;
      }
      else
      {
        nRes = sizeof(resList2)/sizeof(uint16_t);

        for(i = 0; i<nRes; i++)
        {
            resource2 = lwm2m_resource_find(instance2->resource_info, resList2[i]);
            if(resource2 == NULL)
          {
            QCLI_Printf(lwm2m_ext_handle,"Not a valid resource id.\n");
              result = QCLI_STATUS_SUCCESS_E;
              goto end;
          }
    
            switch(resource2->type)
          {
            case QAPI_NET_LWM2M_TYPE_STRING_E:
                 LOG_INFO("Resource %d : Value %s \n",resource2->resource_ID, resource2->value.asBuffer.buffer);
                 QCLI_Printf(lwm2m_ext_handle, "Resource %d : Value %s \n",resource2->resource_ID, resource2->value.asBuffer.buffer);
            break;
            case QAPI_NET_LWM2M_TYPE_INTEGER_E:
                 LOG_INFO("Resource %d : Value %d \n",resource2->resource_ID, (int)resource2->value.asInteger);
                 QCLI_Printf(lwm2m_ext_handle, "Resource %d : Value %d \n",resource2->resource_ID, (int)resource2->value.asInteger);
            break;
            case QAPI_NET_LWM2M_TYPE_FLOAT_E:
                 LOG_INFO("Resource %d : Value %d \n",resource2->resource_ID, (int)resource2->value.asFloat);
                 QCLI_Printf(lwm2m_ext_handle, "Resource %d : Value %d \n",resource2->resource_ID, (int)resource2->value.asFloat);
            break;
            case QAPI_NET_LWM2M_TYPE_BOOLEAN_E:
                 LOG_INFO("Resource %d : Value %d \n",resource2->resource_ID, resource2->value.asBoolean);
                 QCLI_Printf(lwm2m_ext_handle, "Resource %d : Value %d \n",resource2->resource_ID, (int)resource2->value.asBoolean);
            break;
          }
        }
          result = QCLI_STATUS_SUCCESS_E;
          goto end;
      }
    }
  }
  else
  {
      instance_start2 = object2->instance_info;
      while(object2->instance_info != NULL)
    {
        instance2 = lwm2m_instance_find(object2->instance_info, object2->instance_info->instance_ID);
        if(instance2 == NULL)
      {
        QCLI_Printf(lwm2m_ext_handle,"Not a valid instance id.\n");
          result = QCLI_STATUS_SUCCESS_E;
          goto end;
      }

      QCLI_Printf(lwm2m_ext_handle, "Read on / %d / %d ", obj_id,
                  inst_id);

      if(obj_id == LWM2M_GENERIC_SENSOR_OBJECT_ID)
      {
        nRes = sizeof(resList1)/sizeof(uint16_t);
  
        for(i = 0; i<nRes; i++)
        {
            resource2 = lwm2m_resource_find(instance2->resource_info, resList1[i]);
            if(resource2 == NULL)
          {
            QCLI_Printf(lwm2m_ext_handle,"Not a valid resource id.\n");
              result = QCLI_STATUS_SUCCESS_E;
              goto end;
          }

            switch(resource2->type)
          {
            case QAPI_NET_LWM2M_TYPE_STRING_E:
                 LOG_INFO("Resource %d : Value %s \n",resource2->resource_ID, resource2->value.asBuffer.buffer);
                 QCLI_Printf(lwm2m_ext_handle, "Resource %d : Value %s \n",resource2->resource_ID, resource2->value.asBuffer.buffer);
            break;
            case QAPI_NET_LWM2M_TYPE_INTEGER_E:
                 LOG_INFO("Resource %d : Value %d \n",resource2->resource_ID, (int)resource2->value.asInteger);
                 QCLI_Printf(lwm2m_ext_handle, "Resource %d : Value %d \n",resource2->resource_ID, (int)resource2->value.asInteger);
            break;
            case QAPI_NET_LWM2M_TYPE_FLOAT_E:
                 LOG_INFO("Resource %d : Value %d \n",resource2->resource_ID, (int)resource2->value.asFloat);
                 QCLI_Printf(lwm2m_ext_handle, "Resource %d : Value %d \n",resource2->resource_ID, (int)resource2->value.asFloat);
            break;
            case QAPI_NET_LWM2M_TYPE_BOOLEAN_E:
                 LOG_INFO("Resource %d : Value %d \n",resource2->resource_ID, resource2->value.asBoolean);
                 QCLI_Printf(lwm2m_ext_handle, "Resource %d : Value %d \n",resource2->resource_ID, (int)resource2->value.asBoolean);
            break;
          }
        }
      }
      else
      {
        nRes = sizeof(resList2)/sizeof(uint16_t);

        for(i = 0; i<nRes; i++)
        {
            resource2 = lwm2m_resource_find(instance2->resource_info, resList2[i]);
            if(resource2 == NULL)
          {
            QCLI_Printf(lwm2m_ext_handle,"Not a valid resource id.\n");
              result = QCLI_STATUS_SUCCESS_E;
              goto end;
          }

            switch(resource2->type)
          {
            case QAPI_NET_LWM2M_TYPE_STRING_E:
                 LOG_INFO("Resource %d : Value %s \n",resource2->resource_ID, resource2->value.asBuffer.buffer);
                 QCLI_Printf(lwm2m_ext_handle, "Resource %d : Value %s \n",resource2->resource_ID, resource2->value.asBuffer.buffer);
            break;
            case QAPI_NET_LWM2M_TYPE_INTEGER_E:
                 LOG_INFO("Resource %d : Value %d \n",resource2->resource_ID, (int)resource2->value.asInteger);
                 QCLI_Printf(lwm2m_ext_handle, "Resource %d : Value %d \n",resource2->resource_ID, (int)resource2->value.asInteger);
            break;
            case QAPI_NET_LWM2M_TYPE_FLOAT_E:
                 LOG_INFO("Resource %d : Value %d \n",resource2->resource_ID, (int)resource2->value.asFloat);
                 QCLI_Printf(lwm2m_ext_handle, "Resource %d : Value %d \n",resource2->resource_ID, (int)resource2->value.asFloat);
            break;
            case QAPI_NET_LWM2M_TYPE_BOOLEAN_E:
                 LOG_INFO("Resource %d : Value %d \n",resource2->resource_ID, resource2->value.asBoolean);
                 QCLI_Printf(lwm2m_ext_handle, "Resource %d : Value %d \n",resource2->resource_ID, (int)resource2->value.asBoolean);
            break;
          }
        }
      }
        object2->instance_info = object2->instance_info->next;
    }
      object2->instance_info = instance_start2;
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
  }
  }


 end:
  if (cmd_parser_temp->app_data1.app_id != 0)
  {
    if (cmd_parser_temp->app_data1.Parameter_List != NULL)
    {
      for (i = 0; i< cmd_parser_temp->app_data1.Parameter_Count; i++)
      {
        if (cmd_parser_temp->app_data1.Parameter_List[i].String_Value != NULL)
        {
          tx_byte_release(cmd_parser_temp->app_data1.Parameter_List[i].String_Value);
          cmd_parser_temp->app_data1.Parameter_List[i].String_Value = NULL;
        }
      }
    }
  }

  if (cmd_parser_temp->app_data2.app_id != 0)
  {
    if (cmd_parser_temp->app_data2.Parameter_List != NULL)
    {
      for (i = 0; i< cmd_parser_temp->app_data2.Parameter_Count; i++)
      {
        if (cmd_parser_temp->app_data2.Parameter_List[i].String_Value != NULL)
        {
          tx_byte_release(cmd_parser_temp->app_data2.Parameter_List[i].String_Value);
          cmd_parser_temp->app_data2.Parameter_List[i].String_Value = NULL;
        }
      }
    }
  }

  tx_byte_release(cmd_parser_temp);

  return result;
}

QCLI_Command_Status_t ext_write(uint32_t Parameter_Count,
                                QCLI_Parameter_t *Parameter_List)
{
  int id = -1;
  int inst_id = -1;
  int res_id  = -1;
  int obj_id = -1;

  int i;

  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;
  qapi_Net_LWM2M_Data_t *object1 = NULL;
  qapi_Net_LWM2M_Instance_Info_t *instance1 = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource1 = NULL;
  char value1[255];

  qapi_Net_LWM2M_Data_t *object2 = NULL;
  qapi_Net_LWM2M_Instance_Info_t *instance2 = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource2 = NULL;
  char value2[255];

  multi_app_cmd_parser_t *cmd_parser_temp = NULL;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, true);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(lwm2m_ext_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  if (cmd_parser_temp->app_data1.app_id != 0)
  {
    id = cmd_parser_temp->app_data1.app_id - 1;

    if(cmd_parser_temp->app_data1.Parameter_List == NULL || cmd_parser_temp->app_data1.Parameter_Count < 5)
  {
      result = QCLI_STATUS_USAGE_E;
      goto end;
  }

    if (cmd_parser_temp->app_data1.Parameter_List[1].Integer_Is_Valid)
    {
      obj_id = cmd_parser_temp->app_data1.Parameter_List[1].Integer_Value;
    }

    if (cmd_parser_temp->app_data1.Parameter_List[2].Integer_Is_Valid)
      {
      inst_id = cmd_parser_temp->app_data1.Parameter_List[2].Integer_Value;
      }

    if (cmd_parser_temp->app_data1.Parameter_List[3].Integer_Is_Valid)
    {
      res_id = cmd_parser_temp->app_data1.Parameter_List[3].Integer_Value;
    }

    if(id == 0)
    {
      if(ext_object_handler_app1 == NULL)
      {
        QCLI_Printf(lwm2m_ext_handle, "Application not registered.\n");
        result =QCLI_STATUS_ERROR_E;
        goto end;
      }

      if(object_generic == NULL)
      {
        QCLI_Printf(lwm2m_ext_handle, "Object not created for application.\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
      }

      obj_id = LWM2M_GENERIC_SENSOR_OBJECT_ID;

      object1 = lwm2m_object_find(object_generic, obj_id);
    }

    else if(id == 1)
    {
      if(ext_object_handler_app2 == NULL)
  {
        QCLI_Printf(lwm2m_ext_handle, "Application not registered.\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
      }
  
      if(object_presence== NULL)
      {
        QCLI_Printf(lwm2m_ext_handle, "Object not created for application.\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
      }
  
      obj_id = LWM2M_PRESENCE_SENSOR_OBJECT_ID;
  
      object1 = lwm2m_object_find(object_presence, obj_id);
    }

    else
    {
      QCLI_Printf(lwm2m_ext_handle, "Not valid application id");
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
    }

    if(cmd_parser_temp->app_data1.Parameter_List[4].String_Value)
    {
      strlcpy(value1,(const char *)cmd_parser_temp->app_data1.Parameter_List[4].String_Value, (sizeof(cmd_parser_temp->app_data1.Parameter_List[4].String_Value) + 1));
    }
    else
    {
      QCLI_Printf(lwm2m_ext_handle, "Not valid value.\n");
      result = QCLI_STATUS_ERROR_E;
      goto end;
  }

    if(object1 == NULL)
    {
      QCLI_Printf(lwm2m_ext_handle,"Object not found.\n");
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
    }
    instance1 = lwm2m_instance_find(object1->instance_info, inst_id);
    if(instance1 == NULL)
    {
      QCLI_Printf(lwm2m_ext_handle,"Not a valid instance id.\n");
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
    }

    resource1 = lwm2m_resource_find(instance1->resource_info, res_id);
    if(resource1 == NULL)
    {
      QCLI_Printf(lwm2m_ext_handle,"Not a valid resource id.\n");
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
    }

    if(check_writable(obj_id, res_id) == false)
    {
      QCLI_Printf(lwm2m_ext_handle, "Write on resource %d not allowed", res_id);
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
    }

    QCLI_Printf(lwm2m_ext_handle, "Write on / %d / %d / %d", obj_id, inst_id, res_id);
    switch(resource1->type)
    {
      case QAPI_NET_LWM2M_TYPE_STRING_E:
           memcpy(resource1->value.asBuffer.buffer, value1, sizeof(value1));
           LOG_INFO("Resource %d : Value %s \n",resource1->resource_ID, resource1->value.asBuffer.buffer);
           QCLI_Printf(lwm2m_ext_handle, "Resource %d : Value %s \n",resource1->resource_ID, resource1->value.asBuffer.buffer);
      break;
      case QAPI_NET_LWM2M_TYPE_INTEGER_E:
           resource1->value.asInteger = cmd_parser_temp->app_data1.Parameter_List[4].Integer_Value;
           LOG_INFO("Resource %d : Value %d \n",resource1->resource_ID, (int)resource1->value.asInteger);
           QCLI_Printf(lwm2m_ext_handle, "Resource %d : Value %d \n",resource1->resource_ID, (int)resource1->value.asInteger);
      break;
      case QAPI_NET_LWM2M_TYPE_FLOAT_E:
           resource1->value.asFloat = cmd_parser_temp->app_data1.Parameter_List[4].Integer_Value;
           LOG_INFO("Resource %d : Value %d \n",resource1->resource_ID,(int) resource1->value.asFloat);
           QCLI_Printf(lwm2m_ext_handle, "Resource %d : Value %d \n",resource1->resource_ID, (int)resource1->value.asFloat);
      break;
      case QAPI_NET_LWM2M_TYPE_BOOLEAN_E:
           resource1->value.asBoolean = cmd_parser_temp->app_data1.Parameter_List[4].Integer_Value;
           LOG_INFO("Resource %d : Value %d \n",resource1->resource_ID, resource1->value.asBoolean);
           QCLI_Printf(lwm2m_ext_handle, "Resource %d : Value %d \n",resource1->resource_ID, (int)resource1->value.asBoolean);
      break;
    }
  }

  if (cmd_parser_temp->app_data2.app_id != 0)
  {
    id = cmd_parser_temp->app_data2.app_id - 1;

    if(cmd_parser_temp->app_data2.Parameter_List == NULL || cmd_parser_temp->app_data2.Parameter_Count < 5)
    {
      result = QCLI_STATUS_USAGE_E;
      goto end;
    }

    if (cmd_parser_temp->app_data2.Parameter_List[1].Integer_Is_Valid)
  {
      obj_id = cmd_parser_temp->app_data2.Parameter_List[1].Integer_Value;
  }

    if (cmd_parser_temp->app_data2.Parameter_List[2].Integer_Is_Valid)
  {
      inst_id = cmd_parser_temp->app_data2.Parameter_List[2].Integer_Value;
  }

    if (cmd_parser_temp->app_data2.Parameter_List[3].Integer_Is_Valid)
  {
      res_id = cmd_parser_temp->app_data2.Parameter_List[3].Integer_Value;
  }

  if(id == 0)
  {
    if(ext_object_handler_app1 == NULL)
    {
      QCLI_Printf(lwm2m_ext_handle, "Application not registered.\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }

    if(object_generic == NULL)
    {
      QCLI_Printf(lwm2m_ext_handle, "Object not created for application.\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }

    obj_id = LWM2M_GENERIC_SENSOR_OBJECT_ID;

      object2 = lwm2m_object_find(object_generic, obj_id);
  }

  else if(id == 1)
  {
    if(ext_object_handler_app2 == NULL)
    {
      QCLI_Printf(lwm2m_ext_handle, "Application not registered.\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }
  
    if(object_presence== NULL)
    {
      QCLI_Printf(lwm2m_ext_handle, "Object not created for application.\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }
  
    obj_id = LWM2M_PRESENCE_SENSOR_OBJECT_ID;
  
      object2 = lwm2m_object_find(object_presence, obj_id);
  }

  else
  {
    QCLI_Printf(lwm2m_ext_handle, "Not valid application id");
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
  }

    if(cmd_parser_temp->app_data2.Parameter_List[4].String_Value)
  {
      strlcpy(value2,(const char *)cmd_parser_temp->app_data2.Parameter_List[4].String_Value, (sizeof(cmd_parser_temp->app_data2.Parameter_List[4].String_Value) + 1));
  }
  else
  {
    QCLI_Printf(lwm2m_ext_handle, "Not valid value.\n");
      result = QCLI_STATUS_ERROR_E;
      goto end;
  }

    if(object2 == NULL)
  {
    QCLI_Printf(lwm2m_ext_handle,"Object not found.\n");
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
  }
    instance2 = lwm2m_instance_find(object2->instance_info, inst_id);
    if(instance2 == NULL)
  {
    QCLI_Printf(lwm2m_ext_handle,"Not a valid instance id.\n");
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
  }

    resource2 = lwm2m_resource_find(instance2->resource_info, res_id);
    if(resource2 == NULL)
  {
    QCLI_Printf(lwm2m_ext_handle,"Not a valid resource id.\n");
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
  }

  if(check_writable(obj_id, res_id) == false)
  {
    QCLI_Printf(lwm2m_ext_handle, "Write on resource %d not allowed", res_id);
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
  }

  QCLI_Printf(lwm2m_ext_handle, "Write on / %d / %d / %d", obj_id, inst_id, res_id);
    switch(resource2->type)
  {
    case QAPI_NET_LWM2M_TYPE_STRING_E:
           memcpy(resource2->value.asBuffer.buffer, value2, sizeof(value2));
           LOG_INFO("Resource %d : Value %s \n",resource2->resource_ID, resource2->value.asBuffer.buffer);
           QCLI_Printf(lwm2m_ext_handle, "Resource %d : Value %s \n",resource2->resource_ID, resource2->value.asBuffer.buffer);
    break;
    case QAPI_NET_LWM2M_TYPE_INTEGER_E:
           resource2->value.asInteger = cmd_parser_temp->app_data2.Parameter_List[4].Integer_Value;
           LOG_INFO("Resource %d : Value %d \n",resource2->resource_ID, (int)resource2->value.asInteger);
           QCLI_Printf(lwm2m_ext_handle, "Resource %d : Value %d \n",resource2->resource_ID, (int)resource2->value.asInteger);
    break;
    case QAPI_NET_LWM2M_TYPE_FLOAT_E:
           resource2->value.asFloat = cmd_parser_temp->app_data2.Parameter_List[4].Integer_Value;
           LOG_INFO("Resource %d : Value %d \n",resource2->resource_ID,(int) resource2->value.asFloat);
           QCLI_Printf(lwm2m_ext_handle, "Resource %d : Value %d \n",resource2->resource_ID, (int)resource2->value.asFloat);
    break;
    case QAPI_NET_LWM2M_TYPE_BOOLEAN_E:
           resource2->value.asBoolean = cmd_parser_temp->app_data2.Parameter_List[4].Integer_Value;
           LOG_INFO("Resource %d : Value %d \n",resource2->resource_ID, resource2->value.asBoolean);
           QCLI_Printf(lwm2m_ext_handle, "Resource %d : Value %d \n",resource2->resource_ID, (int)resource2->value.asBoolean);
    break;
  }
  }

 end:
  if (cmd_parser_temp->app_data1.app_id != 0)
  {
    if (cmd_parser_temp->app_data1.Parameter_List != NULL)
    {
      for (i = 0; i< cmd_parser_temp->app_data1.Parameter_Count; i++)
      {
        if (cmd_parser_temp->app_data1.Parameter_List[i].String_Value != NULL)
        {
          tx_byte_release(cmd_parser_temp->app_data1.Parameter_List[i].String_Value);
          cmd_parser_temp->app_data1.Parameter_List[i].String_Value = NULL;
        }
      }
    }
  }

  if (cmd_parser_temp->app_data2.app_id != 0)
  {
    if (cmd_parser_temp->app_data2.Parameter_List != NULL)
    {
      for (i = 0; i< cmd_parser_temp->app_data2.Parameter_Count; i++)
      {
        if (cmd_parser_temp->app_data2.Parameter_List[i].String_Value != NULL)
        {
          tx_byte_release(cmd_parser_temp->app_data2.Parameter_List[i].String_Value);
          cmd_parser_temp->app_data2.Parameter_List[i].String_Value = NULL;
        }
      }
    }
  }

  tx_byte_release(cmd_parser_temp);

  return result;
}

QCLI_Command_Status_t ext_exec(uint32_t Parameter_Count,
                                      QCLI_Parameter_t *Parameter_List)
{
  int id = -1;
  int inst_id = -1;
  int res_id  = -1;
  int obj_id = -1;

  int i;

  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;
  qapi_Net_LWM2M_Data_t *object1 = NULL;
  qapi_Net_LWM2M_Instance_Info_t *instance1 = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource1 = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource1_1 = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource1_2 = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource1_3 = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource1_4 = NULL;

  qapi_Net_LWM2M_Data_t *object2 = NULL;
  qapi_Net_LWM2M_Instance_Info_t *instance2 = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource2 = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource2_1 = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource2_2 = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource2_3 = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource2_4 = NULL;

  multi_app_cmd_parser_t *cmd_parser_temp = NULL;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, true);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(lwm2m_ext_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  if (cmd_parser_temp->app_data1.app_id != 0)
  {
    id = cmd_parser_temp->app_data1.app_id - 1;

    if(cmd_parser_temp->app_data1.Parameter_List == NULL || cmd_parser_temp->app_data1.Parameter_Count < 4)
  {
      result = QCLI_STATUS_USAGE_E;
      goto end;
  }

    if (cmd_parser_temp->app_data1.Parameter_List[1].Integer_Is_Valid)
    {
      obj_id = cmd_parser_temp->app_data1.Parameter_List[1].Integer_Value;
    }

    if (cmd_parser_temp->app_data1.Parameter_List[2].Integer_Is_Valid)
      {
      inst_id = cmd_parser_temp->app_data1.Parameter_List[2].Integer_Value;
      }

    if (cmd_parser_temp->app_data1.Parameter_List[3].Integer_Is_Valid)
      {
      res_id = cmd_parser_temp->app_data1.Parameter_List[3].Integer_Value;
    }

    if(id == 0)
    {
      if(ext_object_handler_app1 == NULL)
      {
        QCLI_Printf(lwm2m_ext_handle, "Application not registered.\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
      }
  
      if(object_generic == NULL)
      {
        QCLI_Printf(lwm2m_ext_handle, "Object not created for application.\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }
  
      obj_id = LWM2M_GENERIC_SENSOR_OBJECT_ID;
  
      object1 = lwm2m_object_find(object_generic, obj_id);
    }
 
    else if(id == 1)
  {
      if(ext_object_handler_app2 == NULL)
      {
        QCLI_Printf(lwm2m_ext_handle, "Application not registered.\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
      }
  
      if(object_presence== NULL)
      {
        QCLI_Printf(lwm2m_ext_handle, "Object not created for application.\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
  }
  
      obj_id = LWM2M_PRESENCE_SENSOR_OBJECT_ID;
  
      object1 = lwm2m_object_find(object_presence, obj_id);
    }

    else
  {
      QCLI_Printf(lwm2m_ext_handle, "Not valid application id");
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
    }

  if(object1 == NULL)
  {
    QCLI_Printf(lwm2m_ext_handle,"Not a valid object id.\n");
    result = QCLI_STATUS_SUCCESS_E;
    goto end;
  }

  instance1 = lwm2m_instance_find(object1->instance_info, inst_id);
  if(instance1 == NULL)
  {
    QCLI_Printf(lwm2m_ext_handle,"Not a valid instance id.\n");
    result = QCLI_STATUS_SUCCESS_E;
    goto end;
  }

    resource1 = lwm2m_resource_find(instance1->resource_info, res_id);
    if(resource1 == NULL)
    {
      QCLI_Printf(lwm2m_ext_handle,"Not a valid resource id.\n");
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
    }

    if(check_executable(obj_id,res_id) == false)
    {
      QCLI_Printf(lwm2m_ext_handle, "Execute on resource %d not allowed", res_id);
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
  }

    QCLI_Printf(lwm2m_ext_handle, "Execute on / %d / %d / %d", obj_id,
              inst_id, res_id);

    if(obj_id == LWM2M_GENERIC_SENSOR_OBJECT_ID)
    {
      resource1_1 = lwm2m_resource_find(instance1->resource_info, RES_O_MIN_MEASURED_VAL);
      resource1_2 = lwm2m_resource_find(instance1->resource_info, RES_M_MIN_RANGE_VAL);
      resource1_3 = lwm2m_resource_find(instance1->resource_info, RES_O_MAX_MEASURED_VAL);
      resource1_4 = lwm2m_resource_find(instance1->resource_info, RES_M_MAX_RANGE_VAL);
      if(resource1_1 != NULL && resource1_2 != NULL)
      {
        resource1_1->value.asFloat = resource1_2->value.asFloat;
      }
      if(resource1_3 != NULL && resource1_4 != NULL)
      {
        resource1_3->value.asFloat = resource1_4->value.asFloat;
      }
    }
    else
    {
      resource1_1 = lwm2m_resource_find(instance1->resource_info, RES_O_DIG_INPUT_COUNTER);
      if(resource1_1 != NULL)
      {
        resource1_1->value.asInteger = 0;
      }
    }
  }

  if (cmd_parser_temp->app_data2.app_id != 0)
  {
    id = cmd_parser_temp->app_data2.app_id - 1;

    if(cmd_parser_temp->app_data2.Parameter_List == NULL || cmd_parser_temp->app_data2.Parameter_Count < 4)
  {
      result = QCLI_STATUS_USAGE_E;
      goto end;
    }

    if (cmd_parser_temp->app_data2.Parameter_List[1].Integer_Is_Valid)
    {
      obj_id = cmd_parser_temp->app_data2.Parameter_List[1].Integer_Value;
    }

    if (cmd_parser_temp->app_data2.Parameter_List[2].Integer_Is_Valid)
    {
      inst_id = cmd_parser_temp->app_data2.Parameter_List[2].Integer_Value;
  }

    if (cmd_parser_temp->app_data2.Parameter_List[3].Integer_Is_Valid)
  {
      res_id = cmd_parser_temp->app_data2.Parameter_List[3].Integer_Value;
  }

  if(id == 0)
  {
    if(ext_object_handler_app1 == NULL)
    {
      QCLI_Printf(lwm2m_ext_handle, "Application not registered.\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }
  
    if(object_generic == NULL)
    {
      QCLI_Printf(lwm2m_ext_handle, "Object not created for application.\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }
  
    obj_id = LWM2M_GENERIC_SENSOR_OBJECT_ID;
  
      object2 = lwm2m_object_find(object_generic, obj_id);
  }
 
  else if(id == 1)
  {
    if(ext_object_handler_app2 == NULL)
    {
      QCLI_Printf(lwm2m_ext_handle, "Application not registered.\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }
  
    if(object_presence== NULL)
    {
      QCLI_Printf(lwm2m_ext_handle, "Object not created for application.\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }
  
    obj_id = LWM2M_PRESENCE_SENSOR_OBJECT_ID;
  
      object2 = lwm2m_object_find(object_presence, obj_id);
  }

  else
  {
    QCLI_Printf(lwm2m_ext_handle, "Not valid application id");
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
  }

  if(object2 == NULL)
  {
    QCLI_Printf(lwm2m_ext_handle,"Not a valid object id.\n");
    result = QCLI_STATUS_SUCCESS_E;
    goto end;
  }

  instance2 = lwm2m_instance_find(object2->instance_info, inst_id);
  if(instance2 == NULL)
  {
    QCLI_Printf(lwm2m_ext_handle,"Not a valid instance id.\n");
    result = QCLI_STATUS_SUCCESS_E;
    goto end;
  }

    resource2 = lwm2m_resource_find(instance2->resource_info, res_id);
    if(resource2 == NULL)
  {
    QCLI_Printf(lwm2m_ext_handle,"Not a valid resource id.\n");
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
  }

  if(check_executable(obj_id,res_id) == false)
  {
    QCLI_Printf(lwm2m_ext_handle, "Execute on resource %d not allowed", res_id);
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
  }

  QCLI_Printf(lwm2m_ext_handle, "Execute on / %d / %d / %d", obj_id,
              inst_id, res_id);

  if(obj_id == LWM2M_GENERIC_SENSOR_OBJECT_ID)
  {
      resource2_1 = lwm2m_resource_find(instance2->resource_info, RES_O_MIN_MEASURED_VAL);
      resource2_2 = lwm2m_resource_find(instance2->resource_info, RES_M_MIN_RANGE_VAL);
      resource2_3 = lwm2m_resource_find(instance2->resource_info, RES_O_MAX_MEASURED_VAL);
      resource2_4 = lwm2m_resource_find(instance2->resource_info, RES_M_MAX_RANGE_VAL);
      if(resource2_1 != NULL && resource2_2 != NULL)
    {
        resource2_1->value.asFloat = resource2_2->value.asFloat;
    }
      if(resource2_3 != NULL && resource2_4 != NULL)
    {
        resource2_3->value.asFloat = resource2_4->value.asFloat;
    }
  }
  else
  {
      resource2_1 = lwm2m_resource_find(instance2->resource_info, RES_O_DIG_INPUT_COUNTER);
      if(resource2_1 != NULL)
    {
        resource2_1->value.asInteger = 0;
      }
    }
  }

 end:
  if (cmd_parser_temp->app_data1.app_id != 0)
  {
    if (cmd_parser_temp->app_data1.Parameter_List != NULL)
    {
      for (i = 0; i< cmd_parser_temp->app_data1.Parameter_Count; i++)
      {
        if (cmd_parser_temp->app_data1.Parameter_List[i].String_Value != NULL)
        {
          tx_byte_release(cmd_parser_temp->app_data1.Parameter_List[i].String_Value);
          cmd_parser_temp->app_data1.Parameter_List[i].String_Value = NULL;
        }
      }
    }
  }

  if (cmd_parser_temp->app_data2.app_id != 0)
  {
    if (cmd_parser_temp->app_data2.Parameter_List != NULL)
    {
      for (i = 0; i< cmd_parser_temp->app_data2.Parameter_Count; i++)
      {
        if (cmd_parser_temp->app_data2.Parameter_List[i].String_Value != NULL)
        {
          tx_byte_release(cmd_parser_temp->app_data2.Parameter_List[i].String_Value);
          cmd_parser_temp->app_data2.Parameter_List[i].String_Value = NULL;
        }
      }
    }
  }

  tx_byte_release(cmd_parser_temp);

  return result;
}

QCLI_Command_Status_t deinit_ext_obj(uint32_t Parameter_Count,
                                     QCLI_Parameter_t *Parameter_List)
{
  int id = -1;

  int i;

  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;
  qapi_Net_LWM2M_Ext_t *ext_param1 = NULL;
  qapi_Net_LWM2M_Ext_t *ext_param2 = NULL;

  multi_app_cmd_t* cmd_ptr1 = NULL;
  multi_app_cmd_t* cmd_ptr2 = NULL;

  multi_app_cmd_parser_t *cmd_parser_temp = NULL;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, true);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(lwm2m_ext_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  if (cmd_parser_temp->app_data1.app_id != 0)
  {
    if(cmd_parser_temp->app_data1.Parameter_Count < 1 || cmd_parser_temp->app_data1.Parameter_List == NULL)
    {
      result = QCLI_STATUS_USAGE_E;
      goto end;
    }

    id = cmd_parser_temp->app_data1.app_id - 1;

    tx_byte_allocate(byte_pool_qcli, (VOID **)&ext_param1, sizeof(qapi_Net_LWM2M_Ext_t), TX_NO_WAIT);

    if(ext_param1 == NULL)
  {
    LOG_INFO("Cannot assign memory for extensible object parameter");
      result = QCLI_STATUS_ERROR_E;
      goto end;
  }

    memset(ext_param1, 0x00, sizeof(qapi_Net_LWM2M_Ext_Obj_t));

  if(id == 0 || id == 1)
  {
      ext_param1->app_data.ul_op.id = id;
      ext_param1->app_data.ul_op.msg_type = QAPI_NET_LWM2M_EXT_DEREGISTER_APP_E;
  }
  else
  {
    LOG_INFO("Not valid application id.");
      tx_byte_release(ext_param1);
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
  }

  LOG_INFO("Switching from QCLI to application callback context");

    if (id == 0)
  {
      if((cmd_ptr1 = multi_app_get_cmd_buffer()) == NULL)
    {
      LOG_INFO("Cannot obtain command buffer");
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }
    else
    {
        cmd_ptr1->cmd_data.cmd_id              = EXT_OBJ_APP_ORIGINATED_EVT;
        cmd_ptr1->cmd_data.data               = (void *)ext_param1;

        if(dam_app1_registerd == false)
      {
        QCLI_Printf(lwm2m_ext_handle,"Application %d not registerd ",id+1);
          tx_byte_release(ext_param1);
          result = QCLI_STATUS_SUCCESS_E;
          goto end;
      }
      LOG_INFO("Enqueue the command into the command queue. ");
        multi_app_enqueue_cmd_t1(cmd_ptr1);
    }

      multi_app_set_signal_dam_t1();
  }

  else if(id == 1)
  {
      if((cmd_ptr1 = multi_app_get_cmd_buffer()) == NULL)
    {
      LOG_INFO("Cannot obtain command buffer");
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }
    else
    {
        cmd_ptr1->cmd_data.cmd_id              = EXT_OBJ_APP_ORIGINATED_EVT;
        cmd_ptr1->cmd_data.data               = (void *)ext_param1;

        if(dam_app2_registerd == false)
      {
        QCLI_Printf(lwm2m_ext_handle,"Application %d not registerd ",id+1);
          tx_byte_release(ext_param1);
          result = QCLI_STATUS_SUCCESS_E;
          goto end;
      }
      LOG_INFO("Enqueue the command into the command queue. ");
        multi_app_enqueue_cmd_t2(cmd_ptr1);
    }

      multi_app_set_signal_dam_t2();
  }

  else
  {
    LOG_INFO("Application id not valid.");
  }
  }

  if (cmd_parser_temp->app_data2.app_id != 0)
  {
     if(cmd_parser_temp->app_data2.Parameter_Count < 1 || cmd_parser_temp->app_data2.Parameter_List == NULL)
    {
      result = QCLI_STATUS_USAGE_E;
      goto end;
    }

    id = cmd_parser_temp->app_data2.app_id - 1;

    tx_byte_allocate(byte_pool_qcli, (VOID **)&ext_param2, sizeof(qapi_Net_LWM2M_Ext_t), TX_NO_WAIT);

    if(ext_param2 == NULL)
    {
      LOG_INFO("Cannot assign memory for extensible object parameter");
      result = QCLI_STATUS_ERROR_E;
      goto end;
    }

    memset(ext_param2, 0x00, sizeof(qapi_Net_LWM2M_Ext_Obj_t));

    if(id == 0 || id == 1)
      {
      ext_param2->app_data.ul_op.id = id;
      ext_param2->app_data.ul_op.msg_type = QAPI_NET_LWM2M_EXT_DEREGISTER_APP_E;
    }

    else
    {
      LOG_INFO("Not valid application id.");
      tx_byte_release(ext_param2);
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
  }

    LOG_INFO("Switching from QCLI to application callback context");

    if(id == 0)
  {
      if((cmd_ptr2 = multi_app_get_cmd_buffer()) == NULL)
    {
        LOG_INFO("Cannot obtain command buffer");
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }
      else
    {
        cmd_ptr2->cmd_data.cmd_id              = EXT_OBJ_APP_ORIGINATED_EVT;
        cmd_ptr2->cmd_data.data               = (void *)ext_param2;

        if(dam_app1_registerd == false)
      {
          QCLI_Printf(lwm2m_ext_handle,"Application %d not registerd ",id+1);
          tx_byte_release(ext_param2);
          result = QCLI_STATUS_SUCCESS_E;
          goto end;
      }
        LOG_INFO("Enqueue the command into the command queue. ");
        multi_app_enqueue_cmd_t1(cmd_ptr2);
    }
   
      multi_app_set_signal_dam_t1();
  }

    else if(id == 1)
  {
      if((cmd_ptr2 = multi_app_get_cmd_buffer()) == NULL)
      {
        LOG_INFO("Cannot obtain command buffer");
        result = QCLI_STATUS_ERROR_E;
        goto end;
  }
      else
      {
        cmd_ptr2->cmd_data.cmd_id              = EXT_OBJ_APP_ORIGINATED_EVT;
        cmd_ptr2->cmd_data.data               = (void *)ext_param2;

      if(dam_app2_registerd == false)
  {
        QCLI_Printf(lwm2m_ext_handle,"Application %d not registerd ",id+1);
        tx_byte_release(ext_param2);
          result = QCLI_STATUS_SUCCESS_E;
          goto end;
      }
        LOG_INFO("Enqueue the command into the command queue. ");
        multi_app_enqueue_cmd_t2(cmd_ptr2);
      }
  
      multi_app_set_signal_dam_t2();
  }

  else
    {
      LOG_INFO("Application id not valid.");
    }
  }

 end:
  if (cmd_parser_temp->app_data1.app_id != 0)
  {
    if (cmd_parser_temp->app_data1.Parameter_List != NULL)
    {
      for (i = 0; i< cmd_parser_temp->app_data1.Parameter_Count; i++)
      {
        if (cmd_parser_temp->app_data1.Parameter_List[i].String_Value != NULL)
        {
          tx_byte_release(cmd_parser_temp->app_data1.Parameter_List[i].String_Value);
          cmd_parser_temp->app_data1.Parameter_List[i].String_Value = NULL;
        }
      }
    }
  }

  if (cmd_parser_temp->app_data2.app_id != 0)
  {
    if (cmd_parser_temp->app_data2.Parameter_List != NULL)
    {
      for (i = 0; i< cmd_parser_temp->app_data2.Parameter_Count; i++)
      {
        if (cmd_parser_temp->app_data2.Parameter_List[i].String_Value != NULL)
        {
          tx_byte_release(cmd_parser_temp->app_data2.Parameter_List[i].String_Value);
          cmd_parser_temp->app_data2.Parameter_List[i].String_Value = NULL;
        }
      }
    }
  }

  tx_byte_release(cmd_parser_temp);

  return result;
}

qapi_Net_LWM2M_Attributes_t *lwm2m_find_attributes(qapi_Net_LWM2M_App_Handler_t handler,
                                                   qapi_Net_LWM2M_Server_Data_t op_data,
                                                   qapi_Net_LWM2M_Uri_t *uri)
{
  qapi_Net_LWM2M_Observed_t *observed;
  qapi_Net_LWM2M_Observe_info_t *observe_info;
  qapi_Net_LWM2M_Attributes_t *param = NULL;

  if(uri == NULL || handler == NULL)
  {
    return NULL;
  }

  observed = observe_find_by_uri(handler, uri);

  if(observed != NULL && observed->observe_info != NULL)
  {
    for(observe_info = observed->observe_info; observe_info != NULL; observe_info = observe_info->next)
    {
      if(memcmp((observe_info->msg_id + observe_info->msg_id_len - 2), (op_data.msg_id + op_data.msg_id_len -2), 2) == 0)
      {
        param = observe_info->attributes;
      }
    }
  }

  return param;
}

int ext_obj_discover_serialize(qapi_Net_LWM2M_App_Handler_t handler,
                               qapi_Net_LWM2M_Server_Data_t op_data,
                               qapi_Net_LWM2M_Uri_t *uri,
                               int size,
                               qapi_Net_LWM2M_Flat_Data_t *data,
                               uint8_t **buffer)
{
  uint32_t buffer_size = 0;
  int i = 0;
  
  qapi_Net_LWM2M_Uri_t temp_uri;
  qapi_Net_LWM2M_Attributes_t *res_attr_list_tail  = NULL;
  qapi_Net_LWM2M_Attributes_t *res_attr_list_head  = NULL;
  qapi_Net_LWM2M_Attributes_t *obj_attr  = NULL;
  qapi_Net_LWM2M_Attributes_t *inst_attr = NULL;
  qapi_Net_LWM2M_Attributes_t *attr_list_head = NULL;
  

  if(handler == NULL || uri == NULL || buffer == NULL ) 
  {
    LOG_INFO("Passed  Argument is NULL");
    return -1;
  }
  LOG_INFO("ext_obj_discover_serialize/size: %d,flag[%d][/%d/%d/%d]", size, uri->flag, uri->objectId,uri->instanceId, uri->instanceId);

  //find the object level attributes
  if(uri->flag & QAPI_NET_LWM2M_OBJECT_ID_E)
  {
    memset(&temp_uri, 0, sizeof(qapi_Net_LWM2M_Uri_t));
    temp_uri.objectId = uri->objectId;
    temp_uri.flag = QAPI_NET_LWM2M_OBJECT_ID_E;

    obj_attr = lwm2m_find_attributes(handler, op_data, &temp_uri);  
  }
  
  if(uri->flag & QAPI_NET_LWM2M_INSTANCE_ID_E)
  {
	memset(&temp_uri, 0, sizeof(qapi_Net_LWM2M_Uri_t));  
	// get object instance level attributes
	temp_uri.objectId   = uri->objectId;
	temp_uri.instanceId = uri->instanceId;
	temp_uri.flag = QAPI_NET_LWM2M_OBJECT_ID_E | QAPI_NET_LWM2M_INSTANCE_ID_E;
	inst_attr = lwm2m_find_attributes(handler, op_data, &temp_uri);  
	
	/* get resource level attrbiutes list, this need to be used when discover on instance or resource levele*/
	memcpy(&temp_uri, uri, sizeof(temp_uri));
	temp_uri.flag     = QAPI_NET_LWM2M_OBJECT_ID_E | QAPI_NET_LWM2M_INSTANCE_ID_E | QAPI_NET_LWM2M_RESOURCE_ID_E;

    if(data != NULL)
    {
      for(i=0; i<size; i++)
      {
        qapi_Net_LWM2M_Attributes_t *res_attr = NULL;
        temp_uri.resourceId = (data+i)->id;
        res_attr = lwm2m_find_attributes(handler, op_data, &temp_uri);
        if(res_attr != NULL)
        {  
          res_attr->next = NULL;
          if(res_attr_list_head == NULL)
          {
            res_attr_list_head = res_attr;
          }
          else
          {
            res_attr_list_tail->next = res_attr;
          }
          res_attr_list_tail = res_attr;
        }
      }
    } // data != NULL
  }

  //converge obj/instance/resource level attribute into a list
  if(obj_attr!= NULL)
  {
    attr_list_head = obj_attr; 
	if(inst_attr != NULL)
	{
	  attr_list_head->next = inst_attr;
      
	  if(res_attr_list_head != NULL)
	  {
	    inst_attr->next = res_attr_list_head;
	  }	   
	}
	else if(res_attr_list_head != NULL)
	{
	  obj_attr->next = res_attr_list_head;
	}
  }
  else if(inst_attr != NULL)
  {
    attr_list_head = inst_attr;
    inst_attr->next = res_attr_list_head;
  }
  else if(res_attr_list_head != NULL)
  {
    attr_list_head = res_attr_list_head;
  }

  if(data != NULL && size != 0)
  {
    qapi_Net_LWM2M_Encode_Data(handler, &op_data.obj_info, data, size, attr_list_head, QAPI_NET_LWM2M_APPLICATION_LINK_FORMAT, buffer, &buffer_size);
  }
  else
  {
    qapi_Net_LWM2M_Encode_Data(handler, &op_data.obj_info, NULL, 0, attr_list_head, QAPI_NET_LWM2M_APPLICATION_LINK_FORMAT, buffer, &buffer_size);
  }

  return buffer_size;
}

QCLI_Command_Status_t res_inst_create(uint32_t Parameter_Count,
                                      QCLI_Parameter_t *Parameter_List)
{
  int id = -1;
  int res_inst_id = -1;
  int inst_id = -1;
  int res_id  = -1;
  int obj_id = -1;
  char value1[255];
  char value2[255];
  int i;
  gen_multi_res_t* mult_inst1 = NULL;
  pre_multi_res_t* mult_inst2 = NULL;

  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;
  qapi_Net_LWM2M_Data_t *object1 = NULL;
  qapi_Net_LWM2M_Instance_Info_t *instance1 = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource1 = NULL;
  

  qapi_Net_LWM2M_Data_t *object2 = NULL;
  qapi_Net_LWM2M_Instance_Info_t *instance2 = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource2 = NULL;

  multi_app_cmd_parser_t *cmd_parser_temp = NULL;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, true);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(lwm2m_ext_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  if (cmd_parser_temp->app_data1.app_id != 0)
  {
    id = cmd_parser_temp->app_data1.app_id - 1;

    if(cmd_parser_temp->app_data1.Parameter_List == NULL || cmd_parser_temp->app_data1.Parameter_Count < 4)
    {
      result = QCLI_STATUS_USAGE_E;
      goto end;
    }

    if (cmd_parser_temp->app_data1.Parameter_List[1].Integer_Is_Valid)
    {
      obj_id = cmd_parser_temp->app_data1.Parameter_List[1].Integer_Value;
    }

    if (cmd_parser_temp->app_data1.Parameter_List[2].Integer_Is_Valid)
    {
      inst_id = cmd_parser_temp->app_data1.Parameter_List[2].Integer_Value;
    }

    if (cmd_parser_temp->app_data1.Parameter_List[3].Integer_Is_Valid)
    {
      res_id = cmd_parser_temp->app_data1.Parameter_List[3].Integer_Value;
    }

    if (cmd_parser_temp->app_data1.Parameter_List[4].Integer_Is_Valid)
    {
      res_inst_id = cmd_parser_temp->app_data1.Parameter_List[4].Integer_Value;
    }

    if(id == 0)
    {
      if(ext_object_handler_app1 == NULL)
      {
        QCLI_Printf(lwm2m_ext_handle, "Application not registered.\n");
        result =QCLI_STATUS_ERROR_E;
        goto end;
      }

      if(object_generic == NULL)
      {
        QCLI_Printf(lwm2m_ext_handle, "Object not created for application.\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
      }

      obj_id = LWM2M_GENERIC_SENSOR_OBJECT_ID;

      object1 = lwm2m_object_find(object_generic, obj_id);
    }

    else if(id == 1)
    {
      if(ext_object_handler_app2 == NULL)
      {
        QCLI_Printf(lwm2m_ext_handle, "Application not registered.\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
      }
  
      if(object_presence== NULL)
      {
        QCLI_Printf(lwm2m_ext_handle, "Object not created for application.\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
      }
  
      obj_id = LWM2M_PRESENCE_SENSOR_OBJECT_ID;
  
      object1 = lwm2m_object_find(object_presence, obj_id);
    }

    else
    {
      QCLI_Printf(lwm2m_ext_handle, "Not valid application id");
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
    }

    if(cmd_parser_temp->app_data1.Parameter_List[5].String_Value)
    {
      strlcpy(value1,(const char *)cmd_parser_temp->app_data1.Parameter_List[5].String_Value, sizeof(cmd_parser_temp->app_data1.Parameter_List[5].String_Value)+1);
    }
    else
    {
      QCLI_Printf(lwm2m_ext_handle, "Not valid value.\n");
      result = QCLI_STATUS_ERROR_E;
      goto end;
    }

    if(object1 == NULL)
    {
      QCLI_Printf(lwm2m_ext_handle,"Object not found.\n");
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
    }
    instance1 = lwm2m_instance_find(object1->instance_info, inst_id);
    if(instance1 == NULL)
    {
      QCLI_Printf(lwm2m_ext_handle,"Not a valid instance id.\n");
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
    }

    resource1 = lwm2m_resource_find(instance1->resource_info, res_id);
    if(resource1 == NULL)
    {
      QCLI_Printf(lwm2m_ext_handle,"Not a valid resource id.\n");
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
    }

    if(check_writable(obj_id, res_id) == false)
    {
      QCLI_Printf(lwm2m_ext_handle, "Creation on resource %d not allowed", res_id);
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
    }

    QCLI_Printf(lwm2m_ext_handle, "Creating on / %d / %d / %d / %d ", obj_id, inst_id, res_id, res_inst_id);

    if(id == 0)
    {
      mult_inst1 = (gen_multi_res_t *) lwm2m_gen_inst_find(gen_multi_res, inst_id);
      if (NULL == mult_inst1)
      {
        LOG_INFO("Instance id is not found");
        QCLI_Printf(lwm2m_ext_handle, "Instance id is not found");
        result =  QCLI_STATUS_ERROR_E;
        goto end;
      }
  
      if (!ext_write_resource_inst_val((ext_resource_instance_int_list_t **)&(mult_inst1->sensors_val),res_inst_id, 
                                        cmd_parser_temp->app_data1.Parameter_List[5].Integer_Value))
      {
        result = QCLI_STATUS_ERROR_E;
        QCLI_Printf(lwm2m_ext_handle, "Failed to create resource instance");
        goto end;
      }
      else
      {
        result = QCLI_STATUS_SUCCESS_E;
        QCLI_Printf(lwm2m_ext_handle, "Created resource instance successfully.");
      }
    }
    else if(id == 1)
    {
      mult_inst2 = (pre_multi_res_t *) lwm2m_pre_inst_find(pre_multi_res, inst_id);
      if (NULL == mult_inst2)
      {
        LOG_INFO("Instance id is not found");
        QCLI_Printf(lwm2m_ext_handle, "Instance id is not found");
        result =  QCLI_STATUS_ERROR_E;
        goto end;
      }
  
      if (!ext_write_string_resource_inst_val((ext_resource_instance_string_list_t **)&(mult_inst2->sensor_types),res_inst_id, value1))
      {
        result = QCLI_STATUS_ERROR_E;
        QCLI_Printf(lwm2m_ext_handle, "Failed to create resource instance");
        goto end;
      }
      else
      {
        result = QCLI_STATUS_SUCCESS_E;
        QCLI_Printf(lwm2m_ext_handle, "Created resource instance successfully.");
      }
    }
  }

  if (cmd_parser_temp->app_data2.app_id != 0)
  {
    id = cmd_parser_temp->app_data2.app_id - 1;

    if(cmd_parser_temp->app_data2.Parameter_List == NULL || cmd_parser_temp->app_data2.Parameter_Count < 5)
    {
      result = QCLI_STATUS_USAGE_E;
      goto end;
    }

    if (cmd_parser_temp->app_data2.Parameter_List[1].Integer_Is_Valid)
    {
      obj_id = cmd_parser_temp->app_data2.Parameter_List[1].Integer_Value;
    }

    if (cmd_parser_temp->app_data2.Parameter_List[2].Integer_Is_Valid)
    {
      inst_id = cmd_parser_temp->app_data2.Parameter_List[2].Integer_Value;
    }

    if (cmd_parser_temp->app_data2.Parameter_List[3].Integer_Is_Valid)
    {
      res_id = cmd_parser_temp->app_data2.Parameter_List[3].Integer_Value;
    }

    if (cmd_parser_temp->app_data2.Parameter_List[4].Integer_Is_Valid)
    {
      res_inst_id = cmd_parser_temp->app_data2.Parameter_List[4].Integer_Value;
    }

    if(id == 0)
    {
      if(ext_object_handler_app1 == NULL)
      {
        QCLI_Printf(lwm2m_ext_handle, "Application not registered.\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
      }

      if(object_generic == NULL)
      {
        QCLI_Printf(lwm2m_ext_handle, "Object not created for application.\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
      }

      obj_id = LWM2M_GENERIC_SENSOR_OBJECT_ID;

      object2 = lwm2m_object_find(object_generic, obj_id);
    }

    else if(id == 1)
    {
      if(ext_object_handler_app2 == NULL)
      {
        QCLI_Printf(lwm2m_ext_handle, "Application not registered.\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
      }
  
      if(object_presence== NULL)
      {
        QCLI_Printf(lwm2m_ext_handle, "Object not created for application.\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
      }
  
      obj_id = LWM2M_PRESENCE_SENSOR_OBJECT_ID;
  
      object2 = lwm2m_object_find(object_presence, obj_id);
    }

    else
    {
      QCLI_Printf(lwm2m_ext_handle, "Not valid application id");
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
    }

    if(cmd_parser_temp->app_data2.Parameter_List[5].String_Value)
    {
      strlcpy(value2,(const char *)cmd_parser_temp->app_data2.Parameter_List[5].String_Value, sizeof(cmd_parser_temp->app_data2.Parameter_List[5].String_Value)+1);
    }
    else
    {
      QCLI_Printf(lwm2m_ext_handle, "Not valid value.\n");
      result = QCLI_STATUS_ERROR_E;
      goto end;
    }

    if(object2 == NULL)
    {
      QCLI_Printf(lwm2m_ext_handle,"Object not found.\n");
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
    }
    instance2 = lwm2m_instance_find(object2->instance_info, inst_id);
    if(instance2 == NULL)
    {
      QCLI_Printf(lwm2m_ext_handle,"Not a valid instance id.\n");
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
    }

    resource2 = lwm2m_resource_find(instance2->resource_info, res_id);
    if(resource2 == NULL)
    {
      QCLI_Printf(lwm2m_ext_handle,"Not a valid resource id.\n");
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
    }

    if(check_writable(obj_id, res_id) == false)
    {
      QCLI_Printf(lwm2m_ext_handle, "Creation on resource %d not allowed", res_id);
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
    }

    QCLI_Printf(lwm2m_ext_handle, "Creating on / %d / %d / %d / %d", obj_id, inst_id, res_id, res_inst_id);

    if(id == 0)
    {
      mult_inst1 = (gen_multi_res_t *) lwm2m_gen_inst_find(gen_multi_res, inst_id);
      if (NULL == mult_inst1)
      {
        LOG_INFO("Instance id is not found");
        QCLI_Printf(lwm2m_ext_handle, "Instance id is not found");
        result =  QCLI_STATUS_ERROR_E;
        goto end;
      }
  
      if (!ext_write_resource_inst_val((ext_resource_instance_int_list_t **)&(mult_inst1->sensors_val),res_inst_id, 
                                       cmd_parser_temp->app_data2.Parameter_List[5].Integer_Value))
      {
        result = QCLI_STATUS_ERROR_E;
        QCLI_Printf(lwm2m_ext_handle, "Failed to create resource instance");
        goto end;
      }
      else
      {
        result = QCLI_STATUS_SUCCESS_E;
        QCLI_Printf(lwm2m_ext_handle, "Created resource instance successfully.");
      }
    }
    else if(id == 1)
    {
      mult_inst2 = (pre_multi_res_t *) lwm2m_pre_inst_find(pre_multi_res, inst_id);
      if (NULL == mult_inst2)
      {
        LOG_INFO("Instance id is not found");
        QCLI_Printf(lwm2m_ext_handle, "Instance id is not found");
        result =  QCLI_STATUS_ERROR_E;
        goto end;
      }
  
      if (!ext_write_string_resource_inst_val((ext_resource_instance_string_list_t **)&(mult_inst2->sensor_types),res_inst_id, value2))
      {
        result = QCLI_STATUS_ERROR_E;
        QCLI_Printf(lwm2m_ext_handle, "Failed to create resource instance");
        goto end;
      }
      else
      {
        result = QCLI_STATUS_SUCCESS_E;
        QCLI_Printf(lwm2m_ext_handle, "Created resource instance successfully.");
      }
    }
  }

 end:
  if (cmd_parser_temp->app_data1.app_id != 0)
  {
    if (cmd_parser_temp->app_data1.Parameter_List != NULL)
    {
      for (i = 0; i< cmd_parser_temp->app_data1.Parameter_Count; i++)
      {
        if (cmd_parser_temp->app_data1.Parameter_List[i].String_Value != NULL)
        {
          tx_byte_release(cmd_parser_temp->app_data1.Parameter_List[i].String_Value);
          cmd_parser_temp->app_data1.Parameter_List[i].String_Value = NULL;
        }
      }
    }
  }

  if (cmd_parser_temp->app_data2.app_id != 0)
  {
    if (cmd_parser_temp->app_data2.Parameter_List != NULL)
    {
      for (i = 0; i< cmd_parser_temp->app_data2.Parameter_Count; i++)
      {
        if (cmd_parser_temp->app_data2.Parameter_List[i].String_Value != NULL)
        {
          tx_byte_release(cmd_parser_temp->app_data2.Parameter_List[i].String_Value);
          cmd_parser_temp->app_data2.Parameter_List[i].String_Value = NULL;
        }
      }
    }
  }
  tx_byte_release(cmd_parser_temp);
  return result;
}

QCLI_Command_Status_t res_inst_delete(uint32_t Parameter_Count,
                                      QCLI_Parameter_t *Parameter_List)
{
  int id = -1;
  int res_inst_id = -1;
  int inst_id = -1;
  int res_id  = -1;
  int obj_id = -1;
  int i;
  gen_multi_res_t* mult_inst1 = NULL;
  pre_multi_res_t* mult_inst2 = NULL;

  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;
  qapi_Net_LWM2M_Data_t *object1 = NULL;
  qapi_Net_LWM2M_Instance_Info_t *instance1 = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource1 = NULL;
  

  qapi_Net_LWM2M_Data_t *object2 = NULL;
  qapi_Net_LWM2M_Instance_Info_t *instance2 = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource2 = NULL;

  multi_app_cmd_parser_t *cmd_parser_temp = NULL;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, true);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(lwm2m_ext_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  if (cmd_parser_temp->app_data1.app_id != 0)
  {
    id = cmd_parser_temp->app_data1.app_id - 1;

    if(cmd_parser_temp->app_data1.Parameter_List == NULL || cmd_parser_temp->app_data1.Parameter_Count < 4)
    {
      result = QCLI_STATUS_USAGE_E;
      goto end;
    }

    if (cmd_parser_temp->app_data1.Parameter_List[1].Integer_Is_Valid)
    {
      obj_id = cmd_parser_temp->app_data1.Parameter_List[1].Integer_Value;
    }

    if (cmd_parser_temp->app_data1.Parameter_List[2].Integer_Is_Valid)
    {
      inst_id = cmd_parser_temp->app_data1.Parameter_List[2].Integer_Value;
    }

    if (cmd_parser_temp->app_data1.Parameter_List[3].Integer_Is_Valid)
    {
      res_id = cmd_parser_temp->app_data1.Parameter_List[3].Integer_Value;
    }

    if (cmd_parser_temp->app_data1.Parameter_List[4].Integer_Is_Valid)
    {
      res_inst_id = cmd_parser_temp->app_data1.Parameter_List[4].Integer_Value;
    }

    if(id == 0)
    {
      if(ext_object_handler_app1 == NULL)
      {
        QCLI_Printf(lwm2m_ext_handle, "Application not registered.\n");
        result =QCLI_STATUS_ERROR_E;
        goto end;
      }

      if(object_generic == NULL)
      {
        QCLI_Printf(lwm2m_ext_handle, "Object not created for application.\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
      }

      obj_id = LWM2M_GENERIC_SENSOR_OBJECT_ID;

      object1 = lwm2m_object_find(object_generic, obj_id);
    }

    else if(id == 1)
    {
      if(ext_object_handler_app2 == NULL)
      {
        QCLI_Printf(lwm2m_ext_handle, "Application not registered.\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
      }
  
      if(object_presence== NULL)
      {
        QCLI_Printf(lwm2m_ext_handle, "Object not created for application.\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
      }
  
      obj_id = LWM2M_PRESENCE_SENSOR_OBJECT_ID;
  
      object1 = lwm2m_object_find(object_presence, obj_id);
    }

    else
    {
      QCLI_Printf(lwm2m_ext_handle, "Not valid application id");
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
    }

    if(object1 == NULL)
    {
      QCLI_Printf(lwm2m_ext_handle,"Object not found.\n");
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
    }
    instance1 = lwm2m_instance_find(object1->instance_info, inst_id);
    if(instance1 == NULL)
    {
      QCLI_Printf(lwm2m_ext_handle,"Not a valid instance id.\n");
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
    }

    resource1 = lwm2m_resource_find(instance1->resource_info, res_id);
    if(resource1 == NULL)
    {
      QCLI_Printf(lwm2m_ext_handle,"Not a valid resource id.\n");
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
    }

    if(check_writable(obj_id, res_id) == false)
    {
      QCLI_Printf(lwm2m_ext_handle, "Deletion on resource %d not allowed", res_id);
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
    }

    QCLI_Printf(lwm2m_ext_handle, "Deleting  / %d / %d / %d / %d ", obj_id, inst_id, res_id, res_inst_id);

    if(id == 0)
    {
      mult_inst1 = (gen_multi_res_t *) lwm2m_gen_inst_find(gen_multi_res, inst_id);
      if (NULL == mult_inst1)
      {
        LOG_INFO("Instance id is not found");
        QCLI_Printf(lwm2m_ext_handle, "Instance id is not found");
        result =  QCLI_STATUS_ERROR_E;
        goto end;
      }
  
      if (!ext_delete_resource_inst_val((ext_resource_instance_int_list_t **)&(mult_inst1->sensors_val),res_inst_id))
      {
        result = QCLI_STATUS_ERROR_E;
        QCLI_Printf(lwm2m_ext_handle, "Failed to delete resource instance");
        goto end;
      }
      else
      {
        result = QCLI_STATUS_SUCCESS_E;
        QCLI_Printf(lwm2m_ext_handle, "Removed resource instance successfully.");
      }
    }
    else if(id == 1)
    {
      mult_inst2 = (pre_multi_res_t *) lwm2m_pre_inst_find(pre_multi_res, inst_id);
      if (NULL == mult_inst2)
      {
        LOG_INFO("Instance id is not found");
        QCLI_Printf(lwm2m_ext_handle, "Instance id is not found");
        result =  QCLI_STATUS_ERROR_E;
        goto end;
      }
  
      if (!ext_delete_string_resource_inst_val((ext_resource_instance_string_list_t **)&(mult_inst2->sensor_types),res_inst_id))
      {
        result = QCLI_STATUS_ERROR_E;
        QCLI_Printf(lwm2m_ext_handle, "Failed to delete resource instance");
        goto end;
      }
      else
      {
        result = QCLI_STATUS_SUCCESS_E;
        QCLI_Printf(lwm2m_ext_handle, "Removed resource instance successfully.");
      }
    }
  }

  if (cmd_parser_temp->app_data2.app_id != 0)
  {
    id = cmd_parser_temp->app_data2.app_id - 1;

    if(cmd_parser_temp->app_data2.Parameter_List == NULL || cmd_parser_temp->app_data2.Parameter_Count < 5)
    {
      result = QCLI_STATUS_USAGE_E;
      goto end;
    }

    if (cmd_parser_temp->app_data2.Parameter_List[1].Integer_Is_Valid)
    {
      obj_id = cmd_parser_temp->app_data2.Parameter_List[1].Integer_Value;
    }

    if (cmd_parser_temp->app_data2.Parameter_List[2].Integer_Is_Valid)
    {
      inst_id = cmd_parser_temp->app_data2.Parameter_List[2].Integer_Value;
    }

    if (cmd_parser_temp->app_data2.Parameter_List[3].Integer_Is_Valid)
    {
      res_id = cmd_parser_temp->app_data2.Parameter_List[3].Integer_Value;
    }

    if (cmd_parser_temp->app_data2.Parameter_List[4].Integer_Is_Valid)
    {
      res_inst_id = cmd_parser_temp->app_data2.Parameter_List[4].Integer_Value;
    }

    if(id == 0)
    {
      if(ext_object_handler_app1 == NULL)
      {
        QCLI_Printf(lwm2m_ext_handle, "Application not registered.\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
      }

      if(object_generic == NULL)
      {
        QCLI_Printf(lwm2m_ext_handle, "Object not created for application.\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
      }

      obj_id = LWM2M_GENERIC_SENSOR_OBJECT_ID;

      object2 = lwm2m_object_find(object_generic, obj_id);
    }

    else if(id == 1)
    {
      if(ext_object_handler_app2 == NULL)
      {
        QCLI_Printf(lwm2m_ext_handle, "Application not registered.\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
      }
  
      if(object_presence== NULL)
      {
        QCLI_Printf(lwm2m_ext_handle, "Object not created for application.\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
      }
  
      obj_id = LWM2M_PRESENCE_SENSOR_OBJECT_ID;
  
      object2 = lwm2m_object_find(object_presence, obj_id);
    }

    else
    {
      QCLI_Printf(lwm2m_ext_handle, "Not valid application id");
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
    }

    if(object2 == NULL)
    {
      QCLI_Printf(lwm2m_ext_handle,"Object not found.\n");
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
    }
    instance2 = lwm2m_instance_find(object2->instance_info, inst_id);
    if(instance2 == NULL)
    {
      QCLI_Printf(lwm2m_ext_handle,"Not a valid instance id.\n");
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
    }

    resource2 = lwm2m_resource_find(instance2->resource_info, res_id);
    if(resource2 == NULL)
    {
      QCLI_Printf(lwm2m_ext_handle,"Not a valid resource id.\n");
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
    }

    if(check_writable(obj_id, res_id) == false)
    {
      QCLI_Printf(lwm2m_ext_handle, "Deletion on resource %d not allowed", res_id);
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
    }

    QCLI_Printf(lwm2m_ext_handle, "Deleting / %d / %d / %d / %d", obj_id, inst_id, res_id, res_inst_id);

    if(id == 0)
    {
      mult_inst1 = (gen_multi_res_t *) lwm2m_gen_inst_find(gen_multi_res, inst_id);
      if (NULL == mult_inst1)
      {
        LOG_INFO("Instance id is not found");
        QCLI_Printf(lwm2m_ext_handle, "Instance id is not found");
        result =  QCLI_STATUS_ERROR_E;
        goto end;
      }
  
      if (!ext_delete_resource_inst_val((ext_resource_instance_int_list_t **)&(mult_inst1->sensors_val),res_inst_id))
      {
        result = QCLI_STATUS_ERROR_E;
        QCLI_Printf(lwm2m_ext_handle, "Failed to delete resource instance");
        goto end;
      }
      else
      {
        result = QCLI_STATUS_SUCCESS_E;
        QCLI_Printf(lwm2m_ext_handle, "Removed resource instance successfully.");
      }
    }
    else if(id == 1)
    {
      mult_inst2 = (pre_multi_res_t *) lwm2m_pre_inst_find(pre_multi_res, inst_id);
      if (NULL == mult_inst2)
      {
        LOG_INFO("Instance id is not found");
        QCLI_Printf(lwm2m_ext_handle, "Instance id is not found");
        result =  QCLI_STATUS_ERROR_E;
        goto end;
      }
  
      if (!ext_delete_string_resource_inst_val((ext_resource_instance_string_list_t **)&(mult_inst2->sensor_types),res_inst_id))
      {
        result = QCLI_STATUS_ERROR_E;
        QCLI_Printf(lwm2m_ext_handle, "Failed to delete resource instance");
        goto end;
      }
      else
      {
        result = QCLI_STATUS_SUCCESS_E;
        QCLI_Printf(lwm2m_ext_handle, "Removed resource instance successfully.");
      }
    }
  }

 end:
  if (cmd_parser_temp->app_data1.app_id != 0)
  {
    if (cmd_parser_temp->app_data1.Parameter_List != NULL)
    {
      for (i = 0; i< cmd_parser_temp->app_data1.Parameter_Count; i++)
      {
        if (cmd_parser_temp->app_data1.Parameter_List[i].String_Value != NULL)
        {
          tx_byte_release(cmd_parser_temp->app_data1.Parameter_List[i].String_Value);
          cmd_parser_temp->app_data1.Parameter_List[i].String_Value = NULL;
        }
      }
    }
  }

  if (cmd_parser_temp->app_data2.app_id != 0)
  {
    if (cmd_parser_temp->app_data2.Parameter_List != NULL)
    {
      for (i = 0; i< cmd_parser_temp->app_data2.Parameter_Count; i++)
      {
        if (cmd_parser_temp->app_data2.Parameter_List[i].String_Value != NULL)
        {
          tx_byte_release(cmd_parser_temp->app_data2.Parameter_List[i].String_Value);
          cmd_parser_temp->app_data2.Parameter_List[i].String_Value = NULL;
        }
      }
    }
  }
  tx_byte_release(cmd_parser_temp);
  return result;
}

