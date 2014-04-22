/******************************************************************************

  @file    coap_utils.c

  ---------------------------------------------------------------------------

  Copyright (c) 2016-2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------

 *****************************************************************************/

#include <stringl/stringl.h>
#include <stdarg.h>
#include <time.h>
#include "qapi_socket.h"

#include "app_coap.h"
#include "connection.h"
#include "coap-internal.h"
#include "qurt.h"

#define MAX_PACKET_SIZE 63
#define MAX_PACKET_BYTES_DUMP_PER_LINE 16
#define MAX_PACKET_HEX_DUMP_PER_LINE 57


int coap_mutex_lock(qurt_mutex_t *  mutex, const char* file_name, uint32_t line_number)
{
  qurt_mutex_lock((qurt_mutex_t *)mutex);
  return 0;
}

int coap_mutex_unlock(qurt_mutex_t *  mutex, const char* file_name, uint32_t line_number)
{
  qurt_mutex_unlock((qurt_mutex_t *)mutex);
  return 0;
}


/* @fn     coap_list_t * coap_list_add()
 * @brief  This function is used to add the node into coap_list  
 * @param  head - pointer to coap_list head  
 *         node - pointer to new node 
 * @return pointer to coap_list_t  
 */
coap_list_t * coap_list_add(coap_list_t * head,
    coap_list_t * node)
{
  coap_list_t * target;

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

/* @fn     coap_list_t * coap_list_remove()
 * @brief  This function is used to remove the node from coap_list
 * @param  head -  pointer to coap_list head
 *         id   -  coap integer id   
 *         node -  double pointer to coap_list 
 * @return pointer to coap_list_t        
 */


coap_list_t * coap_list_remove(coap_list_t * head,
    uint16_t id,
    coap_list_t ** nodeP)
{
  coap_list_t * target;

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
  while (NULL != target->next && target->next->id != id)
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

void coap_list_free(coap_list_t * head, void * umem)
{
  if (head != NULL)
  {
    coap_list_t * nextP;

    nextP = head->next;

    mem_utility_release_memory(umem!=NULL, head);

    coap_list_free(nextP, umem);
  }
}


/**
 * @fn coap_list_t * coap_list_find()
 * @brief This function is used to find a node in the list
 *
 * @param head  pointer to the first node in the list
 * @param id  id of the node to be searched
 *
 * @return    pointer to the node with give id on success
 *            NULL if id is not in the list
 */
coap_list_t * coap_list_find(coap_list_t * head,
    uint16_t id)
{
  while (NULL != head && head->id != id)
  {
    head = head->next;
  }

  if (NULL != head && head->id == id) return head;

  return NULL;
}

#ifdef APPS_PACKET_LOGGING_ENABLE  


/* @fn     void coap_dump_packet()
 * @brief  This function is used dump the packet 
 * @param  buffer - pointer to the packet
 *         length - length of buffer
 * @return void       
 */

void coap_dump_packet(uint8_t * buffer,
    int length)
{
  unsigned int i = 0, j = 0, k = 0,offset=0;
  static uint8_t logBuffer[MAX_PACKET_HEX_DUMP_PER_LINE];
  static uint8_t tempBuffer[MAX_PACKET_BYTES_DUMP_PER_LINE ];
  uint8_t copyLen = 0;
  unsigned int ulength = (unsigned int)length;
  if(buffer == NULL)
    return;
  while (k < ulength)
  {
    if ((k + MAX_PACKET_BYTES_DUMP_PER_LINE) < ulength)
      copyLen =  MAX_PACKET_BYTES_DUMP_PER_LINE;
    else
      copyLen = ulength - k;
    memset(logBuffer, 0, MAX_PACKET_HEX_DUMP_PER_LINE);
    memscpy(tempBuffer, MAX_PACKET_BYTES_DUMP_PER_LINE, buffer + k, copyLen);
    i = 0;
    snprintf( (char *)logBuffer + i, MAX_PACKET_HEX_DUMP_PER_LINE - i, "%07X ", (unsigned int )(offset*MAX_PACKET_BYTES_DUMP_PER_LINE));
    i += 8;
    j = 0;
    while (i < MAX_PACKET_HEX_DUMP_PER_LINE && (j < copyLen) && (j < MAX_PACKET_BYTES_DUMP_PER_LINE))
    {
      snprintf( (char *)logBuffer + i, MAX_PACKET_HEX_DUMP_PER_LINE - i, "%02X ", (unsigned int )tempBuffer[j]);
      i += 3;
      j++;
    }

    ds_iot_log_strf_high("%s", (char *)logBuffer);

    k += (MAX_PACKET_BYTES_DUMP_PER_LINE);
    offset++;
  }
}

#endif

int Callback_Invoke_If_Coap_Event(void * context , qapi_Coap_Packet_t * coap_pkt , qapi_Coap_Event_t coap_event , void * event_info)
{
  int ret = QAPI_ERROR;
  client_context_t * contextP = NULL;

  if(context == NULL)
  {
	ds_iot_ulog_high(" Callback_Invoke_If_Coap_Event : context or coap_pkt is NULL");
    return ret;
  }

  contextP = (client_context_t *)context;

  if(contextP->event_cb)
  {
    contextP->event_cb((void *)contextP->cid , coap_pkt , coap_event , event_info);
  }
  else
  {
	ds_iot_ulog_high("Callback_Invoke_If_Coap_Event : Event callback not set");
    goto end;
  }

  ret = QAPI_OK;

end:
	
  return ret;
}


