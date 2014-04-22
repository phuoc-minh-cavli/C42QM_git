/*!
  @file
  coap_cmd.c

  @brief
  Implementation of coap command handler functionality.
*/

/*===========================================================================

  Copyright (c) 2018-2023 Qualcomm Technologies, Inc. All Rights Reserved

  Qualcomm Technologies Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "qcli.h"
#include "qcli_api.h"
#include "qapi_socket.h"
#include "qapi.h"
#include "qapi_netservices.h"
#include "bench.h"
#include "net_demo.h"
#include "pal.h"
#include "qapi_ns_utils.h"
#include "stringl/stringl.h"
#include "qapi_coap.h"
#include "qapi_status.h"
#include "qapi_ssl.h"
#include "qapi_fs.h"
#include "dss_netapp.h"

typedef void* qapi_Net_coap_Hndl_t;


#define COAP_SERVERS_SSL_MAX_FRAG_LEN 2048
#define MAX_CERT_NAME_LEN QAPI_NET_SSL_MAX_CERT_NAME_LEN 
#define COAP_PRINTF(...) QCLI_Printf(qcli_net_handle, __VA_ARGS__)
#define MEMSET(d, c, l)			memset((void *)(d), (int32_t)(c), (int32_t)(l))
#define COAP_MAX_BUFFER_LENGTH 2048
#define MAX_RESOURCE_NAME_LEN 63

/*TLS Certificate PEM file names*/
#define TS_CLIENT_CERT_PEM     "client-cert.pem"
#define TS_CLIENT_CERT_KEY_PEM "client-key.pem"
#define TS_CLIENT_CA_LIST_PEM  "cacert.pem"


extern TX_BYTE_POOL *byte_pool_qcli;
qapi_Net_coap_Hndl_t app_coapcli_hndle[NO_OF_APPS] = {NULL};
qapi_Coap_Packet_t * messageP[NO_OF_APPS]= {NULL};
qapi_Coap_Message_Params_t message_param[NO_OF_APPS] = {NULL};
boolean connection_status[NO_OF_APPS] = {0};
qapi_Net_Coap_Get_Extended_Config_Options_t get_coap_ext_conf[NO_OF_APPS];
extern bool dam_thrd_running[NO_OF_APPS];
extern QCLI_Group_Handle_t qcli_net_handle; /* Handle for Net Command Group. */
extern QCLI_Command_t net_cmd_list[];


static int32_t coap_handle_session_cb(qapi_Coap_Session_Hdl_t hdl,qapi_Coap_Packet_t * messageP,void * clientData);
static void coap_handle_transaction_cb(qapi_Coap_Session_Hdl_t hdl ,qapi_Coap_Transaction_t * transaction, qapi_Coap_Packet_t * message);
static qapi_Coap_Status_t coap_block1_handler(qapi_Coap_Session_Hdl_t handle , qapi_Coap_Packet_t * message , int app_id);
static qapi_Coap_Status_t coap_block2_handler(qapi_Coap_Session_Hdl_t  handle , qapi_Coap_Packet_t * message , int app_id);
static qapi_Coap_Status_t coap_downlink_block1_handler(qapi_Coap_Session_Hdl_t handle , qapi_Coap_Packet_t * message , int app_id);
static qapi_Coap_Status_t coap_downlink_block2_handler(qapi_Coap_Session_Hdl_t  handle , qapi_Coap_Packet_t * message , int app_id);
static int get_appid(qapi_Coap_Session_Hdl_t  handle);
static qapi_Coap_Status_t observe_notify_handler(qapi_Coap_Session_Hdl_t hdl , qapi_Coap_Packet_t * message , int app_id);
static void init_global_variable(int app_id);
extern int dss_get_data_call_handle_of_index(int app_id ,int ind);


char get_block2_buffer[NO_OF_APPS][COAP_MAX_BUFFER_LENGTH] ={0};
uint16_t get_block2_buffer_len[NO_OF_APPS] = {0};

char uri_path_block[NO_OF_APPS][MAX_RESOURCE_NAME_LEN] = {0};
uint16_t uri_path_block_len[NO_OF_APPS] = {0}; 

char send_block1_buffer[NO_OF_APPS][COAP_MAX_BUFFER_LENGTH] ={0};
uint16_t send_block1_buffer_len[NO_OF_APPS] = {0};
uint16_t block1_buffer_total_len[NO_OF_APPS] = {0};
uint16_t block1_size[NO_OF_APPS] = {0};

uint16_t block2_size[NO_OF_APPS]  = {0};
uint16_t last_block2_num_rcvd[NO_OF_APPS] = {0};
uint16_t last_block1_num_sent[NO_OF_APPS] = {0};

char get_block1_buffer[NO_OF_APPS][COAP_MAX_BUFFER_LENGTH] ={0};
uint16_t downlink_block1_buffer_len[NO_OF_APPS] = {0};
uint16_t downlink_block1_buffer_total_len[NO_OF_APPS] = {COAP_MAX_BUFFER_LENGTH , COAP_MAX_BUFFER_LENGTH};

uint16_t last_downlink_block2_num_sent[NO_OF_APPS] ={0};
uint16_t last_downlink_block1_num_rcvd[NO_OF_APPS] ={0};

uint16_t total_observe_count[NO_OF_APPS] = {0};
uint16_t current_observe_count[NO_OF_APPS] = {0};

int cancel_sent[NO_OF_APPS] = {0};
uint16_t observe_mid[NO_OF_APPS] = {0};

qapi_Coap_Message_Type_t observe_type[NO_OF_APPS];

#define INTEGER 1
#define STRING 2
#define STRING_SPLIT_CHAR 3

static void* coap_malloc(uint32_t size)
{
  void *ptr = NULL; 
  uint32_t status = 0; 

  status = tx_byte_allocate(byte_pool_qcli, &ptr, size, TX_NO_WAIT);
  if (status != TX_SUCCESS)
  {
    COAP_PRINTF("Failed to allocate memory with %d", status); 
    return NULL;
  }
  
  return ptr;   
}  

void coap_free(void * p)
{
  if(p) 
  {
    tx_byte_release(p);
    p = NULL;
  }
}

static void * memmove_s(
        void *dest,
        const void *src,
        size_t n
        )
{
    unsigned char *d =(unsigned char *)dest;
    unsigned char *s =(unsigned char *)src;

    if(s == d)
        return dest;

    if(s < d) { //copy from back
        s=s+n-1;
        d=d+n-1;
        while(n--)
        {
            *d-- = *s--;
        }
    }
    else { 		//copy from front
        while(n--)
            *d++=*s++;
    }
    return dest;
}

static size_t memsmove(
    void        *dst,
    size_t      dst_size,
    const void  *src,
    size_t      src_size)
{
  size_t  copy_size = (dst_size <= src_size)? dst_size : src_size;

  memmove_s(dst, src, copy_size);

  return copy_size;
}

#define SET_EXTENDED_HEADER_DATA(type, input_data, input_data_len, header_val, val_len) \
{ \
  switch(type) \
  { \
    case INTEGER: \
    { \
      char *head = NULL; \
      uint32_t data = 0; \
      COAP_PRINTF("set_header : header val %s\n" ,input_data); \
      data = atoi((char *)input_data); \
      COAP_PRINTF("set_header : header val %d\n" ,data); \
      if (255 >= data) { \
          val_len = 1; \
          head = coap_malloc(1); \
          if (NULL != head) { \
              memset(head, 0x00, sizeof(char)); \
              *head = data; \
          } \
      } else if (65535 >= data) { \
          val_len = 2; \
          head = coap_malloc(2); \
          if (NULL != head) { \
              memset(head, 0x00, 2 *sizeof(char)); \
              *head     = ((data & 0xFF00) >> 8); \
              *(head +1) = (data & 0x00FF); \
          } \
      } else { \
          val_len = 4; \
          head   = coap_malloc(4); \
          if (NULL != head) { \
              memset(head, 0x00, 4 *sizeof(char)); \
              *head     = ((data & 0xFF000000) >> 24); \
              *(head +1) = ((data & 0x00FF0000) >> 16); \
              *(head +2) = ((data & 0x0000FF00) >> 8); \
              *(head +3) = (data & 0x000000FF); \
          } \
      } \
      header_val = (const void *)head; \
    }  \
    break; \
 \
    case STRING: \
    { \
      header_val = (char *)input_data; \
      val_len    = input_data_len; \
    } \
    break; \
  } \
} \

#define GET_EXTENDED_HEADER_DATA(type, options, split_char, dst, dst_len) \
{ \
  switch(type) \
  { \
    case INTEGER: \
    { \
      uint32_t var = 0; \
      int i = 0; \
      while (i< options->len) \
      { \
        var <<= 8; \
        var |= (uint32_t)options->val[i++]; \
      } \
      COAP_PRINTF("get_header returned success for header type = %d\n " , options->option_num); \
      COAP_PRINTF("get_header : header val %d\n" ,var); \
    } \
    break; \
     \
    case STRING_SPLIT_CHAR: \
    { \
      if (dst_len > 0) \
      { \
        /* dst already contains an option: concatenate */ \
        dst[dst_len] = split_char; \
        dst_len += 1; \
        /* memmove handles 2-byte option headers */ \
        memsmove(dst+dst_len, options->len, options->val, options->len); \
        dst_len += options->len; \
      } \
      else \
      { \
        /* dst is empty: set to option */ \
        memcpy(dst, options->val, options->len); \
        dst_len = options->len; \
      } \
    } \
    break; \
 \
    case STRING: \
    { \
      COAP_PRINTF("get_header returned success for header type = %d\n " , options->option_num); \
      COAP_PRINTF("get_header : header val %s\n" ,options->val); \
    } \
    break; \
  } \
} \


static void init_global_variable(int app_id)
{

	MEMSET(get_block2_buffer[app_id] , 0 , COAP_MAX_BUFFER_LENGTH);
	MEMSET(send_block1_buffer[app_id] , 0 , COAP_MAX_BUFFER_LENGTH); 
	MEMSET(uri_path_block[app_id] , 0 , MAX_RESOURCE_NAME_LEN);
	MEMSET(get_block1_buffer[app_id] , 0 , COAP_MAX_BUFFER_LENGTH);
	
	get_block2_buffer_len[app_id] = 0 ;
	send_block1_buffer_len[app_id] = 0;
	block1_buffer_total_len[app_id] = 0;
	block1_size[app_id] = 0;
	last_block2_num_rcvd[app_id] = 0;
	last_block1_num_sent[app_id] = 0;
	uri_path_block_len[app_id] = 0;
    block2_size[app_id] = 0;
	downlink_block1_buffer_len[app_id] = 0;
	last_downlink_block1_num_rcvd[app_id] = 0;
	total_observe_count[app_id] = 0;
	current_observe_count[app_id] = 0;
	observe_mid[app_id] = 0;
	cancel_sent[app_id] = 0;

}

/*static void  process_ext_options(qapi_Coap_Session_Hdl_t hdl,qapi_Coap_Packet_t * messageP)
{
  qapi_Coap_Ext_Msg_Option_t *ext_option = NULL;
  ext_option = (qapi_Coap_Ext_Msg_Option_t *)coap_malloc(sizeof(qapi_Coap_Ext_Msg_Option_t));
  
  if(ext_option == NULL)
    return;

  memset(ext_option, 0x00, sizeof(qapi_Coap_Ext_Msg_Option_t));

  if(qapi_Coap_Get_Header_Ext(hdl, messageP , &ext_option, 0, 1) !=QAPI_OK)
  {
    COAP_PRINTF("get_header returned error for all header type ");
    coap_free(ext_option);
    return;
  }
  else
  {
    size_t dst_len=0;
    uint8_t dst[100] = {0};
    qapi_Coap_Ext_Msg_Option_t *options = ext_option;

    while(options != NULL)
    {
      if(options->option_num == 258)
      {
        GET_EXTENDED_HEADER_DATA(INTEGER, options, NULL, dst, dst_len);
      }
      else if(options->option_num == 120)
      {
        GET_EXTENDED_HEADER_DATA(STRING_SPLIT_CHAR, options, '&', dst, dst_len);
      }
      else if(options->option_num == 80)
      {
        GET_EXTENDED_HEADER_DATA(STRING, options, NULL, dst, dst_len);
      }
      options = options->next;
    }
    if(dst)
      COAP_PRINTF("get_header : header val %s\n" ,dst);

    options = ext_option;

    while(options != NULL)
    {
      ext_option = options->next;

      if(options->val)
        coap_free(options->val);

      if(options)
        coap_free(options);

      options = ext_option;
    }
  }
}
*/
static qapi_Coap_Status_t coap_block1_handler(qapi_Coap_Session_Hdl_t handle , qapi_Coap_Packet_t * message , int app_id)
{
	qapi_Coap_Status_t ret = QAPI_NO_ERROR;
	uint16_t payload_len;
	qapi_Coap_Block_Params_t params;
	qapi_Coap_Packet_t * response = NULL;	
	qapi_Coap_Message_Params_t message_param = {0};
	char * payload = NULL;
	int nextblock1ok = 0;
    uint16_t send_mid = 0;

	MEMSET(&params,0,sizeof(qapi_Coap_Block_Params_t));
	
	if(handle == NULL || message == NULL)
	{
       COAP_PRINTF("passed parameter is NULL\n");
	   return QAPI_COAP_IGNORE;
	}

	if(message->code == QAPI_CONTINUE_2_31)
	{
       if(message->block1_more)	 
       {
    	   if(qapi_Coap_Init_Message(handle, &response , QAPI_COAP_TYPE_CON , QAPI_COAP_PUT) != QAPI_OK)
           {
              COAP_PRINTF("coap_block1_handler : CON : qapi_Coap_Init_Message returned error");
    	      return QAPI_COAP_IGNORE;
           }	
           
		   if (qapi_Coap_Set_Header(handle,response ,QAPI_COAP_URI_PATH , uri_path_block[app_id] , uri_path_block_len[app_id]) != QAPI_OK)
    	   {
              COAP_PRINTF("coap_block1_handler : CON : qapi_Coap_Set_Header returned error");
              return QAPI_COAP_IGNORE;
    	   }
    	
    	   if(last_block1_num_sent[app_id] == message->block1_num)
   	       {
              nextblock1ok = 1;
			  params.num = message->block1_num + 1;
    	      last_block1_num_sent[app_id] = last_block1_num_sent[app_id] + 1 ;
    	   }
           else
    	  	  params.num = last_block1_num_sent[app_id];
    
    	   if(send_block1_buffer_len[app_id] >= block1_buffer_total_len[app_id])
              params.more = 0;
    	   else
    		  params.more = 1;
    		 
           params.size = block1_size[app_id];
    	  
    	   if (qapi_Coap_Set_Header(handle,response ,QAPI_COAP_BLOCK1,&params,sizeof(qapi_Coap_Block_Params_t)) != QAPI_OK)
    	   {
              COAP_PRINTF("coap_block1_handler : CON : qapi_Coap_Set_Header returned error");
    		  return QAPI_COAP_IGNORE;
    	   } 
    	  
    	   send_mid = message->mid;
           message_param.lastmid = &(send_mid);
		   if(response != NULL)
    	   response->mid = send_mid;
		   if(QAPI_IS_OPTION(message , QAPI_COAP_OPTION_TOKEN))
		   {
			 message_param.token = message->token;
			 message_param.token_len = message->token_len;
		   }

           message_param.msg_cb = coap_handle_transaction_cb;
			    		
           payload_len = block1_size[app_id];
    	   payload = (char *)coap_malloc((payload_len+1)*sizeof(char));
    	  
    	   if(payload != NULL)
    	   {
              MEMSET(payload , 0 , payload_len+1);
    	      strlcpy(payload , send_block1_buffer[app_id] + send_block1_buffer_len[app_id], payload_len+1);
    	   }
    	   else
    	   {
              COAP_PRINTF("coap_block1_handler : Memory allocation failure\n");
    		  return QAPI_COAP_IGNORE;
    	   }
    
           if(qapi_Coap_Set_Payload(handle,response , payload, payload_len) != QAPI_OK)
    	   {
              COAP_PRINTF("coap_block1_handler : CON : qapi_Coap_Set_Payload returned error");
    		  if(payload !=NULL)
    		  coap_free(payload);
    		  return QAPI_COAP_IGNORE;
    	      
    	   }
    
    	   if(qapi_Coap_Send_Message(handle ,response, &message_param) != QAPI_OK)
           {
    	      
    	      COAP_PRINTF("coap_block1_handler : CON : qapi_Coap_Send_Message returned error");
    		  return QAPI_COAP_IGNORE;
           }
    	   else
    	   {
              if(nextblock1ok == 1)
    	      {
                 send_block1_buffer_len[app_id] = send_block1_buffer_len[app_id] + payload_len;	   
    	      }
    
    	   }
    
    	   if(payload !=NULL)
    		  coap_free(payload);
       }
	   else
	   {
          COAP_PRINTF("coap_block1_handler : message->code == QAPI_CONTINUE_2_31 but frag more is 0\n");
		  return QAPI_COAP_IGNORE;
	   }
	   
    }  
	else if(message->code == QAPI_CHANGED_2_04)
    { 
       COAP_PRINTF("coap_block1_handler : message->code == QAPI_CHANGED_2_04\n");
	   COAP_PRINTF("coap_block1_handler : coap_block1_handler processes put request successfully\n");
	   COAP_PRINTF("coap_block1_handler : total bytes put = %d \n" , send_block1_buffer_len[app_id]);
	}
	else 
	{
       COAP_PRINTF("coap_block1_handler : message->code = %d\n" , message->code);
       ret = QAPI_COAP_IGNORE;
	}
	
	return ret;

}

static qapi_Coap_Status_t coap_block2_handler(qapi_Coap_Session_Hdl_t  handle , qapi_Coap_Packet_t * message , int app_id)
{
	qapi_Coap_Status_t ret = QAPI_NO_ERROR;
	uint16_t payload_len;
	qapi_Coap_Block_Params_t params;
	qapi_Coap_Packet_t * response = NULL;	
	qapi_Coap_Message_Params_t message_param = {0};
    uint16_t send_mid = 0;

	MEMSET(&params,0,sizeof(qapi_Coap_Block_Params_t));
	
	if(handle == NULL || message == NULL)
	{
       COAP_PRINTF("passed parameter is NULL\n");
	   return QAPI_COAP_IGNORE;
	}

	if(message->code == QAPI_CONTENT_2_05)
	{

	  
	  
	  if(last_block2_num_rcvd[app_id] == message->block2_num)
	  {
		if(last_block2_num_rcvd[app_id] == 0)
			  get_block2_buffer[app_id][0]= '\0';
	  
		if(message->payload_len > 0 && message->payload)
		{
			payload_len = message->payload_len;
			get_block2_buffer_len[app_id] = get_block2_buffer_len[app_id] + payload_len;
			strlcat(get_block2_buffer[app_id] ,(char*)(message->payload) , get_block2_buffer_len[app_id]);
            if(get_block2_buffer_len[app_id] >= COAP_MAX_BUFFER_LENGTH){
                return QAPI_COAP_IGNORE;
            }
			get_block2_buffer[app_id][get_block2_buffer_len[app_id]] = '\0';
			COAP_PRINTF("coap_block2_handler : Recieved payload using block2 with payload length = %d\n" , get_block2_buffer_len[app_id]);
			COAP_PRINTF("coap_block2_handler: Blockwise2 payload = %s\n" , get_block2_buffer[app_id]);
			params.num = message->block2_num + 1;
			last_block2_num_rcvd[app_id] = last_block2_num_rcvd[app_id] + 1 ;
		}
	  }
	  else
	  params.num = last_block2_num_rcvd[app_id];

	
	  if(message->block2_more == 1)
	  {
	     if(qapi_Coap_Init_Message(handle, &response , QAPI_COAP_TYPE_CON , QAPI_COAP_GET) != QAPI_OK)
         {
              COAP_PRINTF("coap_block2_handler : CON : qapi_Coap_Init_Message returned error");
			  return QAPI_COAP_IGNORE;
         }	
		 		 
	     if (qapi_Coap_Set_Header(handle,response ,QAPI_COAP_URI_PATH , uri_path_block[app_id]  , uri_path_block_len[app_id]) != QAPI_OK)
	     {
            COAP_PRINTF("coap_block2_handler : CON : qapi_Coap_Set_Header returned error");
            return QAPI_COAP_IGNORE;
	     }

         params.more = 0;
         params.size = block2_size[app_id];
	  
	    if (qapi_Coap_Set_Header(handle,response ,QAPI_COAP_BLOCK2,&params,sizeof(qapi_Coap_Block_Params_t)) != QAPI_OK)
	    {
            COAP_PRINTF("coap_block2_handler : CON : qapi_Coap_Set_Header returned error");
		    return QAPI_COAP_IGNORE;
	    } 
	  

	    send_mid = message->mid;
        message_param.lastmid = &(send_mid);
		if(response != NULL)
		response->mid = send_mid;
		if(QAPI_IS_OPTION(message , QAPI_COAP_OPTION_TOKEN))
	    {
	       message_param.token = message->token;
		   message_param.token_len = message->token_len;
	    }

		message_param.msg_cb = coap_handle_transaction_cb;

		if(qapi_Coap_Send_Message(handle ,response, &message_param) != QAPI_OK)
        {
	      
	      COAP_PRINTF("coap_block2_handler : CON : qapi_Coap_Send_Message returned error");
		  return QAPI_COAP_IGNORE;
        }
	 }
	 else
	 {     
	       if(get_block2_buffer_len[app_id] + 1 >= COAP_MAX_BUFFER_LENGTH){
                return QAPI_COAP_IGNORE;
           }
	       COAP_PRINTF("coap_block2_handler : message->block2_more == 0\n");
           get_block2_buffer[app_id][get_block2_buffer_len[app_id]+1] = '\0';
		   COAP_PRINTF("coap_block2_handler : Recieved full payload using block2 with payload length = %d\n" , get_block2_buffer_len[app_id]);
		   COAP_PRINTF("coap_block2_handler:  Blockwise2 payload = %s\n" , get_block2_buffer[app_id]);
     }
  
	 }
	 else
     { 
       COAP_PRINTF("coap_block2_handler : message->code != QAPI_CONTENT_2_05");
       ret = QAPI_COAP_IGNORE;
	 }
	
	return ret;

}

static qapi_Coap_Status_t coap_downlink_block1_handler(qapi_Coap_Session_Hdl_t handle , qapi_Coap_Packet_t * message , int app_id)
{
	qapi_Coap_Status_t ret = QAPI_NO_ERROR;
	uint16_t payload_len;
	qapi_Coap_Block_Params_t params;
	qapi_Coap_Packet_t * response = NULL;	
	qapi_Coap_Message_Params_t message_param = {0};
	uint8_t message_code;
    uint16_t send_mid = 0;

	MEMSET(&params,0,sizeof(qapi_Coap_Block_Params_t));
	MEMSET(&message_param,0,sizeof(qapi_Coap_Message_Params_t));
	
	if(handle == NULL || message == NULL)
	{
       COAP_PRINTF("passed parameter is NULL\n");
	   ret =  QAPI_COAP_IGNORE;
	   goto end;
	}

	if(message->code == QAPI_COAP_PUT)
	{

       if(message->block1_num == 0)
	   {
				MEMSET(get_block1_buffer[app_id],0,COAP_MAX_BUFFER_LENGTH);
				downlink_block1_buffer_len[app_id] = 0;
				last_downlink_block1_num_rcvd[app_id] = 0;
				
	   }
	   
	   if(message->payload_len > 0 && message->payload)
       {
          if(last_downlink_block1_num_rcvd[app_id] == message->block1_num)
     	  {
			 params.num = message->block1_num;
			 payload_len = message->payload_len;
 		     downlink_block1_buffer_len[app_id] = downlink_block1_buffer_len[app_id] + payload_len;
			 strlcat(get_block1_buffer[app_id] ,(char*)(message->payload) , downlink_block1_buffer_len[app_id]);
 		     COAP_PRINTF("coap_downlink_block1_handler : Recieved payload using block1 with payload length = %d\n" ,downlink_block1_buffer_len[app_id]);
 		     COAP_PRINTF("coap_downlink_block1_handler: Blockwise1 payload = %s\n" , get_block1_buffer[app_id]);
     	     last_downlink_block1_num_rcvd[app_id] = last_downlink_block1_num_rcvd[app_id] + 1 ;
     	  }
          else
     	  	 params.num = last_downlink_block1_num_rcvd[app_id];
	   }

	   if(downlink_block1_buffer_len[app_id]>=COAP_MAX_BUFFER_LENGTH)
	      message_code = QAPI_REQUEST_ENTITY_TOO_LARGE_4_13;
	   else
	   {
          if(message->block1_more == 0)
		     message_code = QAPI_CHANGED_2_04;
		  else
		  	 message_code = QAPI_CONTINUE_2_31;
	   }
 
       if(qapi_Coap_Init_Message(handle, &response , QAPI_COAP_TYPE_ACK , message_code) != QAPI_OK)
       {
          COAP_PRINTF("coap_block1_handler : CON : qapi_Coap_Init_Message returned error");
          ret = QAPI_COAP_IGNORE;
          goto end;
       }	
             
       params.more = message->block1_more;
       params.size = message->block1_size;
 

       if (qapi_Coap_Set_Header(handle,response ,QAPI_COAP_BLOCK1,&params,sizeof(qapi_Coap_Block_Params_t)) != QAPI_OK)
       {
          COAP_PRINTF("coap_block1_handler : CON : qapi_Coap_Set_Header returned error");
     	  ret = QAPI_COAP_IGNORE;
          goto end;
       } 

       send_mid = message->mid;
       message_param.lastmid = &(send_mid);
	   if(response != NULL)
       response->mid = send_mid;
	   if(QAPI_IS_OPTION(message , QAPI_COAP_OPTION_TOKEN))
	   {
		   message_param.token = message->token;
		   message_param.token_len = message->token_len;
	   }

       message_param.msg_cb = NULL; 
      

       if(qapi_Coap_Send_Message(handle ,response, &message_param) != QAPI_OK)
       {
          COAP_PRINTF("coap_downlink_block1_handler : qapi_Coap_Send_Message returned error");
     	  ret = QAPI_COAP_IGNORE;
          goto end;
       }
	   else
	   {
          if(message->block1_more == 0)
          {
             COAP_PRINTF("coap_downlink_block1_handler: put request successfully processed\n");
			 COAP_PRINTF("coap_downlink_block1_handler: total payload length recieved = %d\n", downlink_block1_buffer_len[app_id]);
			 if(downlink_block1_buffer_len[app_id] + 1 >= COAP_MAX_BUFFER_LENGTH)
             {
			   COAP_PRINTF("coap_downlink_block1_handler : total payload length received exceeded max length");
               ret = QAPI_COAP_IGNORE;
               goto end;
             }
             get_block1_buffer[app_id][downlink_block1_buffer_len[app_id]+1] = '\0';
			 COAP_PRINTF("coap_downlink_block1_handler: total payload = %s\n", get_block1_buffer[app_id]);
		  }

	   }

	 }  
	 else 
	 {
       COAP_PRINTF("coap_downlink_block1_handler : expected a put request\n" , message->code);
       ret = QAPI_COAP_IGNORE;
       goto end;
	   
	 }


end : 

	 if(response != NULL)
     {
  
		  qapi_Coap_Free_Message(app_coapcli_hndle[app_id],messageP[app_id]);
		  response= NULL;
     }
	
	 return ret;

}

static qapi_Coap_Status_t coap_downlink_block2_handler(qapi_Coap_Session_Hdl_t  handle , qapi_Coap_Packet_t * message , int app_id)
{
    qapi_Coap_Status_t ret = QAPI_NO_ERROR;
	COAP_PRINTF("coap_downlink_block2_handler : downlink block2 is not supported....\n");
	return ret;

}

static int get_appid(qapi_Coap_Session_Hdl_t  handle)
{
 int i=-1;
 for(i=0 ;i < NO_OF_APPS ; i++)
 {
   if(app_coapcli_hndle[i]==handle)
   	return i;  
 }
	return -1;
}


static qapi_Coap_Status_t observe_notify_handler(qapi_Coap_Session_Hdl_t handle , qapi_Coap_Packet_t * message , int app_id)
{

		
			qapi_Coap_Packet_t * response = NULL;	
			qapi_Coap_Message_Params_t message_param = {0};
			uint16_t send_mid = 0;
			char * payload = NULL;
			qapi_Coap_Status_t ret = QAPI_NO_ERROR;
			int observe_cancel_value =1 ;
			qapi_Coap_Message_Type_t cancel_msg_type = QAPI_COAP_TYPE_CON;
			
			 COAP_PRINTF("observe_notify_handler called\n");
		
			 
			if(handle == NULL || message == NULL)
			{
			   COAP_PRINTF("passed parameter is NULL\n");
			   ret =  QAPI_COAP_IGNORE;
			   goto error;
			}
		
			if(message->type == QAPI_COAP_TYPE_ACK && message->code == QAPI_CONTENT_2_05 )
			{	  
				 
			   COAP_PRINTF("observe_notify_handler called : Recieved content in ACK for observe \n");
		
			   if((char*)message->payload != NULL && message->payload_len > 0)
			   {
				 payload = (char*)coap_malloc((message->payload_len) * sizeof(char));
				 if(payload != NULL)
				 {
				   memcpy(payload, (char*)message->payload, message->payload_len);
				 }
			   }
		
			   if(current_observe_count[app_id] == 0)
			   {
				observe_mid[app_id] = message->mid;
			   }
		
			   current_observe_count[app_id]++;
				
			}
			else if(message->type == QAPI_COAP_TYPE_CON && message->code == QAPI_CONTENT_2_05)
			{
			  
			   COAP_PRINTF("observe_notify_handler called : Recieved content in CON for observe \n");
		
			   MEMSET(&message_param,0,sizeof(message_param));
				
			   if((char*)message->payload != NULL && message->payload_len > 0)
			   {
				   payload = (char*)coap_malloc((message->payload_len) * sizeof(char));
				   
				   if(payload != NULL)
				   {
					memcpy(payload, (char*)message->payload, message->payload_len);
				   }
				   else 
				   {
					 COAP_PRINTF("Payload malloc allocation failed\n");
					 payload = NULL;
					 ret =	QAPI_COAP_IGNORE;
					 goto error;
				   }
			   }
		
			   current_observe_count[app_id]++;
		
			   if(qapi_Coap_Init_Message(handle, &response , QAPI_COAP_TYPE_ACK , QAPI_COAP_EMPTY) != QAPI_OK)
			   {
					  COAP_PRINTF("observe_notify_handler : CON : qapi_Coap_Init_Message returned error");
					  ret =  QAPI_COAP_IGNORE;
					  goto error;
			   }	
		
				send_mid = message->mid;
				message_param.lastmid = &(send_mid);
				if(response != NULL)
				response->mid = send_mid;
				
				if(QAPI_IS_OPTION(message , QAPI_COAP_OPTION_TOKEN))
				{
					  message_param.token = message->token;
					  message_param.token_len = message->token_len;
				}
				message_param.msg_cb = NULL;
		
				if(qapi_Coap_Send_Message(handle ,response, &message_param) != QAPI_OK)
				{
				  
				  COAP_PRINTF("observe_notify_handler : ACK : qapi_Coap_Send_Message returned error");
				  ret =  QAPI_COAP_IGNORE;
				  goto error;
				}
		
				if(response != NULL)
				{
		
				   qapi_Coap_Free_Message(handle,response);
				   response  = NULL;
				} 
			   
				if((current_observe_count[app_id] >= total_observe_count[app_id]) && !cancel_sent[app_id])
				{
		
				  MEMSET(&message_param,0,sizeof(message_param));
				  if(observe_type[app_id] == QAPI_COAP_TYPE_CON)
				  {
					  cancel_msg_type = QAPI_COAP_TYPE_CON;
					  message_param.msg_cb = coap_handle_transaction_cb;
				  }
				  else if(observe_type[app_id] == QAPI_COAP_TYPE_NON)
				  {
					  cancel_msg_type = QAPI_COAP_TYPE_NON;
				  }
		
				   
				  if(qapi_Coap_Init_Message(handle, &response , cancel_msg_type , QAPI_COAP_GET) != QAPI_OK)
				  {
						  COAP_PRINTF("observe_notify_handler : CON : qapi_Coap_Init_Message returned error");
						  ret =  QAPI_COAP_IGNORE;
						  goto error;
				  } 
			
				   if (qapi_Coap_Set_Header(handle,response ,QAPI_COAP_URI_PATH , uri_path_block[app_id] , uri_path_block_len[app_id]) != QAPI_OK)
				   {
						COAP_PRINTF("observe_notify_handler : CON  : qapi_Coap_Set_Header returned error");
						ret =  QAPI_COAP_IGNORE;
						goto error;
				   }
		
				   if (qapi_Coap_Set_Header(handle,response ,QAPI_COAP_OBSERVE,&observe_cancel_value,sizeof(int)) != QAPI_OK)
				   {
					COAP_PRINTF("observe_notify_handler : ACK : qapi_Coap_Set_Header returned error");
					ret =  QAPI_COAP_IGNORE;
					goto error;
				   } 
			
					send_mid = observe_mid[app_id] + 1;
					message_param.lastmid = &(send_mid);
					if(response != NULL)
					response->mid = send_mid;
					
					if(QAPI_IS_OPTION(message , QAPI_COAP_OPTION_TOKEN))
					{
						  message_param.token = message->token;
						  message_param.token_len = message->token_len;
					}
		
			
					if(qapi_Coap_Send_Message(handle ,response, &message_param) != QAPI_OK)
					{
					  
					  COAP_PRINTF("observe_notify_handler : ACK : qapi_Coap_Send_Message returned error");
								  ret =  QAPI_COAP_IGNORE;
					  goto error;
					}
					else
					{
					   COAP_PRINTF("observe_notify_handler : sent cancel observe for con .....\n");
					   cancel_sent[app_id] = 1;
					}
					
			
				}
			   
		
			}
			else if(message->type == QAPI_COAP_TYPE_NON && message->code == QAPI_CONTENT_2_05)
			{	 
				 COAP_PRINTF("observe_notify_handler called : Recieved content in NON CON for observe \n");
		
				  MEMSET(&message_param,0,sizeof(message_param));
				  
				  if((char*)message->payload != NULL && message->payload_len > 0)
				  {
					 payload = (char*)coap_malloc((message->payload_len) * sizeof(char));
					 if(payload != NULL)
					 {
					  memcpy(payload, (char*)message->payload, message->payload_len);
					 }
					 else 
					 {
						COAP_PRINTF("Payload malloc allocation failed\n");
						payload = NULL;
						ret =	QAPI_COAP_IGNORE;
						goto error;
					  }
				  }
		
				  if(current_observe_count[app_id] == 0)
				  {
					 observe_mid[app_id] = message->mid;
				  }
				  
				current_observe_count[app_id]++; 
		
				if(current_observe_count[app_id] >= total_observe_count[app_id]  && !cancel_sent[app_id])
				{
		
				  if(observe_type[app_id] == QAPI_COAP_TYPE_CON)
				  {
					  cancel_msg_type = QAPI_COAP_TYPE_CON;
					  message_param.msg_cb = coap_handle_transaction_cb;  
				  }
				  else if(observe_type[app_id] == QAPI_COAP_TYPE_NON)
				  {
					  cancel_msg_type = QAPI_COAP_TYPE_NON;
					  message_param.msg_cb = NULL;
				  }
		
				
				 if(qapi_Coap_Init_Message(handle, &response , cancel_msg_type , QAPI_COAP_GET) != QAPI_OK)
				 {
					  COAP_PRINTF("observe_notify_handler : NON_CON : qapi_Coap_Init_Message returned error");
					  ret =  QAPI_COAP_IGNORE;
					  goto error;
				 }	
				 
				 if (qapi_Coap_Set_Header(handle,response ,QAPI_COAP_URI_PATH , uri_path_block[app_id] , uri_path_block_len[app_id]) != QAPI_OK)
				 {
					COAP_PRINTF("observe_notify_handler : NON_CON : qapi_Coap_Set_Header returned error");
					ret =  QAPI_COAP_IGNORE;
					goto error;
				 }
		
				if (qapi_Coap_Set_Header(handle,response ,QAPI_COAP_OBSERVE,&observe_cancel_value,sizeof(int)) != QAPI_OK)
				{
					COAP_PRINTF("observe_notify_handler : NON_CON : qapi_Coap_Set_Header returned error");
					ret =  QAPI_COAP_IGNORE;
					goto error;
				} 
			  
		
				send_mid =observe_mid[app_id] + 1;
				message_param.lastmid = &(send_mid);
				if(response != NULL)
				response->mid = send_mid;
				if(QAPI_IS_OPTION(message , QAPI_COAP_OPTION_TOKEN))
				{
					  message_param.token = message->token;
					  message_param.token_len = message->token_len;
				}
				
				if(qapi_Coap_Send_Message(handle ,response, &message_param) != QAPI_OK)
				{
				  
				  COAP_PRINTF("observe_notify_handler : NON_CON : qapi_Coap_Send_Message returned error");
				  ret =  QAPI_COAP_IGNORE;
				  goto error;
				}
				else
				{
					COAP_PRINTF("observe_notify_handler : sent cancel observe for non con .....\n");
					cancel_sent[app_id] = 1;
				}
			
				}
		
				if(response != NULL && cancel_msg_type != QAPI_COAP_TYPE_CON)
				{
		
				   qapi_Coap_Free_Message(handle,response);
				   response  = NULL;
				}
		
		 }
		
		error :
				if(payload != NULL)
				 coap_free(payload);
		
				return ret;

}

static void process_options(qapi_Coap_Packet_t * messageP)
{

 if(messageP == NULL) 
 {
    COAP_PRINTF("Got Null coap packet in process_options\n");
	return ;
 }
 if(QAPI_IS_OPTION(messageP , QAPI_COAP_OPTION_IF_MATCH))
 {
   COAP_PRINTF("QAPI_COAP_OPTION_IF_MATCH is Set\n");
 }
 if(QAPI_IS_OPTION(messageP , QAPI_COAP_OPTION_URI_HOST))
 {
    COAP_PRINTF("QAPI_COAP_OPTION_URI_HOST is Set\n");
 }
 if(QAPI_IS_OPTION(messageP , QAPI_COAP_OPTION_ETAG))
 {
    COAP_PRINTF("QAPI_COAP_OPTION_ETAG is Set\n");
 }
 if(QAPI_IS_OPTION(messageP , QAPI_COAP_OPTION_IF_NONE_MATCH))
 {
   COAP_PRINTF("QAPI_COAP_OPTION_IF_NONE_MATCH is Set\n");
 }
 if(QAPI_IS_OPTION(messageP , QAPI_COAP_OPTION_OBSERVE))
 {
    COAP_PRINTF("QAPI_COAP_OPTION_OBSERVE is Set\n");
 }
 if(QAPI_IS_OPTION(messageP , QAPI_COAP_OPTION_URI_PORT))
 {
    COAP_PRINTF("QAPI_COAP_OPTION_URI_PORT is Set\n");
 }
 if(QAPI_IS_OPTION(messageP , QAPI_COAP_OPTION_LOCATION_PATH))
 {
   COAP_PRINTF("QAPI_COAP_OPTION_LOCATION_PATH is Set\n");
 }
 if(QAPI_IS_OPTION(messageP , QAPI_COAP_OPTION_URI_PATH))
 {
    COAP_PRINTF("QAPI_COAP_OPTION_URI_PATH is Set\n");
 }
 if(QAPI_IS_OPTION(messageP , QAPI_COAP_OPTION_MAX_AGE))
 {
    COAP_PRINTF("QAPI_COAP_OPTION_MAX_AGE is Set\n");
 }
 if(QAPI_IS_OPTION(messageP , QAPI_COAP_OPTION_URI_QUERY))
 {
    COAP_PRINTF("QAPI_COAP_OPTION_URI_QUERY is Set\n");
 }
 if(QAPI_IS_OPTION(messageP , QAPI_COAP_OPTION_ACCEPT))
 {  
     COAP_PRINTF("QAPI_COAP_OPTION_ACCEPT is Set\n");
 }
 if(QAPI_IS_OPTION(messageP , QAPI_COAP_OPTION_TOKEN))
 {
    COAP_PRINTF("QAPI_COAP_OPTION_TOKEN is Set\n");
 } 

 if(QAPI_IS_OPTION(messageP , QAPI_COAP_OPTION_LOCATION_QUERY))
 {
    COAP_PRINTF("QAPI_COAP_OPTION_LOCATION_QUERY is Set\n");
 } 
  if(QAPI_IS_OPTION(messageP , QAPI_COAP_OPTION_BLOCK2))
 {
   COAP_PRINTF("QAPI_COAP_OPTION_BLOCK2 is Set\n");
 }
   if(QAPI_IS_OPTION(messageP , QAPI_COAP_OPTION_BLOCK1))
 {
   COAP_PRINTF("QAPI_COAP_OPTION_BLOCK1 is Set\n");
 }
    if(QAPI_IS_OPTION(messageP , QAPI_COAP_OPTION_SIZE))
 {
    COAP_PRINTF("QAPI_COAP_OPTION_SIZE is Set\n");
 }
	 if(QAPI_IS_OPTION(messageP , QAPI_COAP_OPTION_PROXY_URI))
 {
    COAP_PRINTF("QAPI_COAP_OPTION_PROXY_URI is Set\n");
 }
 if(QAPI_IS_OPTION(messageP , QAPI_COAP_OPTION_SIZE1))
 {
    COAP_PRINTF("QAPI_COAP_OPTION_SIZE1 is Set\n");
 }

}


static int m2m_cert_read_from_EFS_file(
    const char * name,
    void ** buffer_ptr,
    size_t * buffer_size
    )
{
  uint32 bytes_read = 0;
  int efs_fd = QAPI_ERROR;
  struct qapi_FS_Stat_Type_s stat = {0};
  uint8_t *file_buf = NULL;

  if((!name) || (!buffer_ptr) || (!buffer_size)) 
  {
    return QAPI_ERROR;
  }

  MEMSET( &stat, 0, sizeof(struct qapi_FS_Stat_Type_s )); 
  if(qapi_FS_Open(name, QAPI_FS_O_RDONLY_E, &efs_fd) < 0) 
  {
    COAP_PRINTF("COAP DAM MODE QCLI  : Opening EFS file %s failed\n", name);
    return QAPI_ERROR;
  }

  if(qapi_FS_Stat_With_Handle(efs_fd, &stat) < 0) {
    COAP_PRINTF("COAP DAM MODE QCLI : Reading EFS file %s failed\n", name);
    return QAPI_ERROR;
  }

  file_buf = coap_malloc(1 * (stat.st_size + 4)); /*Added 4 bytes extra for padding*/

  if(file_buf == NULL) {
    COAP_PRINTF("COAP DAM MODE QCLI : calloc failed \n");
    return QAPI_ERROR;
  }

  qapi_FS_Read(efs_fd, file_buf, stat.st_size, &bytes_read);
  if((bytes_read != stat.st_size) && (bytes_read == 0)) {
    coap_free(file_buf);
    COAP_PRINTF("COAP DAM MODE QCLI : Reading EFS file error\n");
    return QAPI_ERROR;
  }

  *buffer_ptr = file_buf;
  *buffer_size = stat.st_size;
  qapi_FS_Close(efs_fd);
  return QAPI_OK;
}

	

static int coap_ssl_check_and_store_certs(qapi_Net_SSL_Cert_Type_t type, const char *file_name)
{

   void* cert_buf   = NULL;
   void* key_buf    = NULL;
   void* calist_buf = NULL;
  
   size_t cert_buf_size = 0;
   size_t key_buf_size  = 0;
   size_t calist_buf_size  = 0;
   qapi_NET_SSL_CA_Info_t calist_info[QAPI_NET_SSL_MAX_CA_LIST];
   uint32_t ca_list_cnt = 0;
   qapi_Status_t ret    = QAPI_OK;

   qapi_Net_SSL_Cert_Info_t cert_info;
  
   if(file_name == NULL)
   {
      COAP_PRINTF(" coap_ssl_check_and_store_certs : returned QAPI_ERROR : file_name == NULL \n");
	  ret =  QAPI_ERROR;
	  goto Error;
   }
   
  
   MEMSET(&cert_info, 0, sizeof(qapi_Net_SSL_Cert_Info_t));

   if(type == QAPI_NET_SSL_CERTIFICATE_E)
   {

      if(m2m_cert_read_from_EFS_file("/datatx/ssl/"TS_CLIENT_CERT_PEM, &cert_buf, &cert_buf_size) < QAPI_OK)
      {
         COAP_PRINTF(" coap_ssl_check_and_store_certs : TS_CLIENT_CERT_PEM != QAPI_OK\n");
	     ret =  QAPI_ERROR;
	     goto Error;
      }
    
      if(m2m_cert_read_from_EFS_file("/datatx/ssl/"TS_CLIENT_CERT_KEY_PEM, &key_buf, &key_buf_size) < QAPI_OK)
      {
         COAP_PRINTF(" coap_ssl_check_and_store_certs : TS_CLIENT_CERT_KEY_PEM != QAPI_OK\n");
	     ret =  QAPI_ERROR;
	     goto Error;
      }
    
    if((cert_buf == NULL) || (key_buf == NULL)) {
      COAP_PRINTF("coap_ssl_check_and_store_certs : (cert_buf == NULL) || (key_buf == NULL) \n");
	  ret = QAPI_ERROR;
      goto Error;
    }

    cert_info.cert_Type = QAPI_NET_SSL_CERTIFICATE_E;
    cert_info.info.cert.cert_Buf = cert_buf;
    cert_info.info.cert.cert_Size = cert_buf_size;
    cert_info.info.cert.key_Buf = key_buf;
    cert_info.info.cert.key_Size = key_buf_size;
    cert_info.info.cert.pass_Key = NULL;
    
    ret = qapi_Net_SSL_Cert_Convert_And_Store(&cert_info,
                                              (const uint8_t *)file_name);

    if(ret != QAPI_OK)
	{
      COAP_PRINTF("coap_ssl_check_and_store_certs : Failed to store %s : %d\n", file_name, ret);
	  goto Error;
    } 
	else {
      COAP_PRINTF("coap_ssl_check_and_store_certs : Loaded Cert and Key successfully\n");
      return QAPI_OK;
    }
  	}
    else if(type == QAPI_NET_SSL_CA_LIST_E)
    {
      MEMSET(&calist_info, 0x00, sizeof(calist_info));
      cert_info.cert_Type = QAPI_NET_SSL_CA_LIST_E;
  
      if(m2m_cert_read_from_EFS_file("/datatx/ssl/"TS_CLIENT_CA_LIST_PEM, &calist_buf, &calist_buf_size) < QAPI_OK)
      {
        COAP_PRINTF(" coap_ssl_check_and_store_certs : TS_CLIENT_CA_LIST_PEM != QAPI_OK\n");
        ret =  QAPI_ERROR;
        goto Error;
      }
  
      if(calist_buf == NULL) 
      {
        COAP_PRINTF("coap_ssl_check_and_store_certs : (calist_buf == NULL) \n");
        ret = QAPI_ERROR;
        goto Error;
      }
  
      calist_info[ca_list_cnt].ca_Buf = calist_buf;
      calist_info[ca_list_cnt].ca_Size = calist_buf_size;
      cert_info.info.ca_List.ca_Info[ca_list_cnt] = &calist_info[ca_list_cnt];
      cert_info.info.ca_List.ca_Cnt = ++ca_list_cnt;
  
      ret = qapi_Net_SSL_Cert_Convert_And_Store(&cert_info,
                                                (const uint8_t *)file_name);
  
      if(ret != QAPI_OK)
      {
        COAP_PRINTF("coap_ssl_check_and_store_certs : Failed to store %s : %d\n", file_name, ret);
        goto Error;
      } 
      else {
        COAP_PRINTF("coap_ssl_check_and_store_certs : Loaded CaList successfully\n");
        return QAPI_OK;
      }
      
    }
  
    else
    {
        COAP_PRINTF("coap_ssl_check_and_store_certs : type != QAPI_NET_SSL_CERTIFICATE_E\n ");
        ret = QAPI_ERROR;
    }

    if(ret == QAPI_OK) 
   	  return QAPI_OK;
  
Error:
	  if(cert_info.info.cert.cert_Buf != NULL)
		coap_free(cert_info.info.cert.cert_Buf);
	  if(cert_info.info.cert.key_Buf != NULL)
		coap_free(cert_info.info.cert.key_Buf);

      for(ca_list_cnt = 0; ca_list_cnt < cert_info.info.ca_List.ca_Cnt; ca_list_cnt++)
      {
	    if(cert_info.info.ca_List.ca_Info[ca_list_cnt] != NULL)
	     coap_free(calist_info[ca_list_cnt].ca_Buf);
      }

return ret;

}

int coap_configure_ssl(int mode , qapi_Net_SSL_Config_t  * cfg, qapi_Coap_Sec_Info_t *ssl_cfg)
{
  qapi_Status_t ret = QAPI_OK;
  char ssl_cert[MAX_CERT_NAME_LEN] = {0};     //efs filename .PSK or .PEM
  char ssl_key[MAX_CERT_NAME_LEN]  = {0};    //efs filename key.pem 
  char ca_list[MAX_CERT_NAME_LEN]  = {0}; 
  char di_cert[MAX_CERT_NAME_LEN]  = {0}; 
  qapi_Net_SSL_Cert_Info_t cert_info ;
 
  char psk_fname_raw[MAX_CERT_NAME_LEN] = {0}; 
  char cert_fname_bin[MAX_CERT_NAME_LEN];
  void *ssl_cert_buf  = NULL;
  size_t ssl_cert_buf_size = 0;

  
  char cert_fname[MAX_CERT_NAME_LEN] = {0};
  
  if(cfg == NULL || ssl_cfg == NULL)
    return QAPI_ERROR;

  MEMSET(ssl_cfg , 0, sizeof( qapi_Coap_Sec_Info_t));
  MEMSET(cfg, 0, sizeof(qapi_Net_SSL_Config_t));
  MEMSET(&cert_info, 0, sizeof(qapi_Net_SSL_Cert_Info_t));
  MEMSET(ssl_cert, 0, MAX_CERT_NAME_LEN);
  MEMSET(ssl_key, 0, MAX_CERT_NAME_LEN);
  MEMSET(ca_list, 0, MAX_CERT_NAME_LEN);
  MEMSET(di_cert, 0, MAX_CERT_NAME_LEN);

  cfg->verify.domain =  0;
  cfg->verify.match_Name[0] = '\0';
  cfg->verify.time_Validity = 1;
  cfg->verify.send_Alert = 1;
  cfg->max_Frag_Len = COAP_SERVERS_SSL_MAX_FRAG_LEN;

  if ( mode == QAPI_COAP_MODE_PSK)
  {

    COAP_PRINTF(" sec_mode == QAPI_COAP_MODE_PSK ");

	MEMSET(cert_fname_bin, 0, MAX_CERT_NAME_LEN);
	MEMSET(psk_fname_raw, 0, MAX_CERT_NAME_LEN);
	
	    cfg->protocol = QAPI_NET_SSL_PROTOCOL_DTLS_1_2;
        cfg->cipher[0] = QAPI_NET_TLS_PSK_WITH_AES_128_CCM_8;
	    cfg->cipher[1] = QAPI_NET_TLS_PSK_WITH_AES_128_CBC_SHA;
        cfg->cipher[2]= QAPI_NET_TLS_PSK_WITH_AES_256_CBC_SHA;
        cfg->cipher[3] = QAPI_NET_TLS_PSK_WITH_AES_128_GCM_SHA256;
        cfg->cipher[4] = QAPI_NET_TLS_PSK_WITH_AES_256_GCM_SHA384;
        cfg->cipher[5] = QAPI_NET_TLS_PSK_WITH_AES_128_CBC_SHA256;
        cfg->cipher[6] = QAPI_NET_TLS_PSK_WITH_AES_256_CBC_SHA384;
	
    ssl_cfg->psk = coap_malloc(1 * MAX_CERT_NAME_LEN);

	if(ssl_cfg->psk == NULL)return QAPI_ERROR;

	strlcpy((char *)ssl_cfg->psk ,"1234_psk.bin" ,MAX_CERT_NAME_LEN );

    MEMSET(cert_fname_bin, 0, MAX_CERT_NAME_LEN);

	strlcpy((char *)cert_fname_bin ,"1234_psk.bin" ,MAX_CERT_NAME_LEN );
  
    MEMSET(psk_fname_raw, 0, MAX_CERT_NAME_LEN);

	strlcpy(psk_fname_raw , "/datatx/ssl/1234_server.psk" , MAX_CERT_NAME_LEN);

	/*read from default location and encrypt and store in secure location*/
	if(m2m_cert_read_from_EFS_file(psk_fname_raw, &ssl_cert_buf, &ssl_cert_buf_size ) != QAPI_OK)
	{

        if(ssl_cfg->psk != NULL) 
        {
	      coap_free((void *)ssl_cfg->psk);
	      ssl_cfg->psk = NULL;
        }

		return QAPI_ERROR;

	}
	else 
		ret = QAPI_OK;

      cert_info.cert_Type = QAPI_NET_SSL_PSK_TABLE_E;
      cert_info.info.psk_Tbl.psk_Size = ssl_cert_buf_size;
      cert_info.info.psk_Tbl.psk_Buf = ssl_cert_buf; 

	 ret = qapi_Net_SSL_Cert_Convert_And_Store(&cert_info, (uint8_t*)cert_fname_bin);

	if( ret != QAPI_OK)
	{
	  COAP_PRINTF(" coap_configure_ssl : returned QAPI_ERROR : mode %d -> qapi_Net_SSL_Cert_Convert_And_Store != QAPI_OK\n" , mode);
	  goto Error;
	}
	else 
	{
	  COAP_PRINTF("LWM2M_LOG: Client Cert or PSK stored successfully\n");
	  return QAPI_OK;
	}
	 
  } 
  else if (mode == QAPI_COAP_MODE_CERTIFICATE)
   {
	 COAP_PRINTF(" sec_mode == QAPI_COAP_MODE_CERTIFICATE ");
  
	 cfg->protocol = QAPI_NET_SSL_PROTOCOL_DTLS_1_2;
	 cfg->cipher[0] = QAPI_NET_TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8;	  /*OMA Spec defined Cipher Suite*/
	 cfg->cipher[1] = QAPI_NET_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384;
     cfg->cipher[2] = QAPI_NET_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384;
     cfg->cipher[3] = QAPI_NET_TLS_ECDH_RSA_WITH_AES_128_GCM_SHA256;
     cfg->cipher[4] = QAPI_NET_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256;
     cfg->cipher[5] = QAPI_NET_TLS_ECDH_RSA_WITH_AES_256_CBC_SHA;
     cfg->cipher[6] = QAPI_NET_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384;
     cfg->cipher[7] = QAPI_NET_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384;
  
	 MEMSET(cert_fname, 0, MAX_CERT_NAME_LEN);
  
	 ssl_cfg->cert = coap_malloc(1  * MAX_CERT_NAME_LEN);
  
	 if(ssl_cfg->cert == NULL)
	 {
		COAP_PRINTF(" coap_configure_ssl : returned QAPI_ERROR : mode %d -> ssl_cfg->psk == NULL\n" , mode);
		ret =  QAPI_ERROR;
		goto Error;
	 }

	 strlcpy((char *)ssl_cfg->cert, "client-cert.bin" ,MAX_CERT_NAME_LEN);
  
	 strlcpy((char *)cert_fname, "client-cert.bin" , MAX_CERT_NAME_LEN);
	 
	 if(cert_fname == NULL)
	 {
		 COAP_PRINTF(" coap_configure_ssl : returned QAPI_ERROR : mode %d -> cert_fname == NULL \n" , mode);
		 ret =	QAPI_ERROR;
		 goto Error ;
	 }
  
	 ret = coap_ssl_check_and_store_certs(QAPI_NET_SSL_CERTIFICATE_E , cert_fname);
  
	 if(ret  != QAPI_OK)
	 {
	   COAP_PRINTF(" coap_configure_ssl : returned QAPI_ERROR : mode %d -> coap_ssl_check_and_store_certs != QAPI_OK\n" , mode);
	   goto Error;
	 }

	COAP_PRINTF(" coap_configure_ssl : Processing ca_list\n" );
	ssl_cfg->ca_list = coap_malloc(1 * MAX_CERT_NAME_LEN);

	if(ssl_cfg->ca_list == NULL)
	{
       COAP_PRINTF(" coap_configure_ssl : returned QAPI_ERROR : mode %d -> ssl_cfg->ca_list == NULL %d\n" , mode, MAX_CERT_NAME_LEN);
       ret =  QAPI_ERROR;
	   goto Error;
	}

	strlcpy((char *)ssl_cfg->ca_list, "cacert.bin" ,MAX_CERT_NAME_LEN);
    strlcpy((char *)ca_list, "cacert.bin" , MAX_CERT_NAME_LEN);
    
	if(ca_list == NULL)
    {
    	COAP_PRINTF(" coap_configure_ssl : returned QAPI_ERROR : mode %d -> cert_fname == NULL \n" , mode);
    	ret =  QAPI_ERROR;
    	goto Error ;
	}

	ret = coap_ssl_check_and_store_certs(QAPI_NET_SSL_CA_LIST_E , ca_list);

	if(ret  != QAPI_OK)
   	{
	  COAP_PRINTF(" coap_configure_ssl : returned QAPI_ERROR : mode %d -> coap_ssl_check_and_store_certs != QAPI_OK\n" , mode);
      if(ssl_cfg->ca_list != NULL) 
      {
          coap_free((void *)ssl_cfg->ca_list);
          ssl_cfg->ca_list = NULL;
      }
      ret = QAPI_OK;
	}
	 else 
		 return QAPI_OK;
	 
  }

 if(ret == QAPI_OK) return QAPI_OK;

Error:
  if(ssl_cfg->psk != NULL) 
  {
	coap_free((void *)ssl_cfg->psk);
	ssl_cfg->psk = NULL;
  }
  if(ssl_cfg->cert != NULL) 
  {
     coap_free((void *)ssl_cfg->cert);
      ssl_cfg->cert = NULL;
  }

  if(ssl_cfg->di_cert != NULL) 
  {
      coap_free((void *)ssl_cfg->di_cert);
      ssl_cfg->di_cert = NULL;
  }

  if(ssl_cfg->ca_list != NULL) 
  {
     coap_free((void *)ssl_cfg->ca_list);
      ssl_cfg->ca_list = NULL;
  }

  if(cert_info.info.psk_Tbl.psk_Buf != NULL)
  {
      coap_free(cert_info.info.psk_Tbl.psk_Buf);
      cert_info.info.psk_Tbl.psk_Buf = NULL;
      cert_info.info.psk_Tbl.psk_Size = 0;
  }

   return ret;
}

int32_t coap_handle_session_cb(qapi_Coap_Session_Hdl_t    hdl,
    qapi_Coap_Packet_t * messageP,
    void * clientData)
{
   
   char * payload = NULL; 
   uint8_t message_code = 0;
   qapi_Coap_Message_Type_t message_type = QAPI_COAP_TYPE_CON;
   qapi_Coap_Status_t result = QAPI_NO_ERROR;
   int app_id = -1;
   qapi_Coap_Status_t coap_response_code = QAPI_NO_ERROR; 
   //int is_request_uri_ok = 0;
   char uri_string[MAX_RESOURCE_NAME_LEN] ={0};
   size_t uri_len = MAX_RESOURCE_NAME_LEN;
   char * uri_string_pointer = uri_string;

   if (hdl == NULL||  messageP == NULL ) 
   { 
     COAP_PRINTF("Passed Argument is NULL\n" );
     coap_response_code =QAPI_COAP_IGNORE;
	 qapi_Coap_Free_Message(hdl, messageP);
     return coap_response_code;
   }
   else
   	 COAP_PRINTF("handle for this connection %lu\n" , hdl);

   app_id = get_appid(hdl);
   
   if(app_id == -1)
   {
	  COAP_PRINTF("coap_handle_session_cb : get_appid returned -1\n");
	  goto end;
   }
   else
      COAP_PRINTF("coap_handle_session_cb : handle %lu exists on app_id %d .. \n" , hdl , app_id);
   
   message_type =  messageP->type;

   if(QAPI_IS_OPTION(messageP , QAPI_COAP_OPTION_URI_PATH) && messageP->code >=1 && messageP->code <=4 )
   {
     if(qapi_Coap_Get_Header(hdl,(qapi_Coap_Packet_t*)messageP , QAPI_COAP_URI_PATH , (void **)&uri_string_pointer , &uri_len) != QAPI_OK)
     {
       COAP_PRINTF("coap_handle_session_cb : qapi_Coap_Get_Header for uri returned error\n");
	 }
	 else
	 {
      if(strncmp(uri_string , "/test" , 5) || strncmp(uri_string , "test" , 4))
      {
        COAP_PRINTF("Received uri contains test string .....\n");
        //is_request_uri_ok = 1;
	  }
	  else 
	  	COAP_PRINTF("coap_handle_session_cb : got other path uri .....\n");
   }

   }
   
   
   switch(message_type)
   {
      case QAPI_COAP_TYPE_CON :
	  case QAPI_COAP_TYPE_NON :

       message_code = messageP->code;
	  
	    switch (message_code)
	    {
		   case QAPI_COAP_GET: 
		   	COAP_PRINTF("Got Get request from server\n");
			if(message_type == QAPI_COAP_TYPE_NON)
		    {
			 COAP_PRINTF("Processing options\n");
			 process_options(messageP);
			   
			}
            if(message_type == QAPI_COAP_TYPE_CON)
            {
            
			   COAP_PRINTF("Processing options\n");
			   
			   process_options(messageP);
			   
			   if(QAPI_IS_OPTION(messageP , QAPI_COAP_OPTION_BLOCK2))
			   {
                  result = coap_downlink_block2_handler(hdl , messageP , app_id);
		          if(result == QAPI_NO_ERROR)
		          {
                     COAP_PRINTF("coap_handle_session_cb : coap_block2_handler returned success with result %d\n" , result);
		          }
		          else
		          {  
		             coap_response_code =QAPI_COAP_IGNORE;
                     COAP_PRINTF("coap_handle_session_cb : coap_block2_handler returned error with result %d\n" , result);
		          }
			   }

			}
			
			break;
		   case QAPI_COAP_POST :
		   	COAP_PRINTF("Got post request from server\n");
			if(message_type == QAPI_COAP_TYPE_NON)
		    {
			 COAP_PRINTF("Processing options\n");
			 process_options(messageP);
             
 			}
			
            if(message_type == QAPI_COAP_TYPE_CON)
            {
              COAP_PRINTF("Processing options\n");
			  process_options(messageP);
			}
	       
		   	break;
			case QAPI_COAP_PUT :
		   	
		   	COAP_PRINTF("Got put request from server\n");
			if(message_type == QAPI_COAP_TYPE_NON)
		    {
			  COAP_PRINTF("Processing options\n");
			  process_options(messageP);
			}
            else if(message_type == QAPI_COAP_TYPE_CON)
            {
              COAP_PRINTF("Processing options\n");
			  process_options(messageP);

			  if(QAPI_IS_OPTION(messageP , QAPI_COAP_OPTION_BLOCK1))
			   {
                  result = coap_downlink_block1_handler(hdl , messageP , app_id);
		          if(result == QAPI_NO_ERROR)
		          {
                     COAP_PRINTF("coap_handle_session_cb : coap_block1_handler returned success with result %d\n" , result);
		          }
		          else
		          {  
		             coap_response_code =QAPI_COAP_IGNORE;
                     COAP_PRINTF("coap_handle_session_cb : coap_block1_handler returned error with result %d\n" , result);
		          }
			   }
			}
		   	break;
		    case QAPI_COAP_DELETE :
		   	COAP_PRINTF("Got delete request from server\n");
			
		    break;
		   case QAPI_CREATED_2_01 :
		   case QAPI_DELETED_2_02 :
		   case QAPI_VALID_2_03 :
		   case QAPI_CHANGED_2_04 :
		   case QAPI_CONTENT_2_05 :
		   case QAPI_CONTINUE_2_31 :
		   	
            if((message_type == QAPI_COAP_TYPE_NON || message_type == QAPI_COAP_TYPE_CON) && QAPI_IS_OPTION(messageP , QAPI_COAP_OPTION_OBSERVE))
			{
			   coap_response_code = observe_notify_handler(hdl , messageP , app_id);
			}
		    else if((char *)messageP->payload &&  messageP->payload_len > 0)
            {
              payload = (char*)coap_malloc((messageP->payload_len) * sizeof(char));
			  if(payload != NULL)
			  {
              memcpy(payload, (char *)messageP->payload, messageP->payload_len);
			  }
			  else 
			  	 COAP_PRINTF("Payload malloc allocation failed\n");
            }

			break;
			
       	    case QAPI_BAD_REQUEST_4_00 :
			case QAPI_UNAUTHORIZED_4_01 :
		    case QAPI_BAD_OPTION_4_02 :
			case QAPI_FORBIDDEN_4_03 :
			case QAPI_NOT_FOUND_4_04 :
			case QAPI_METHOD_NOT_ALLOWED_4_05 :
			case QAPI_NOT_ACCEPTABLE_4_06 :
			case QAPI_PRECONDITION_FAILED_4_12 :
			case QAPI_REQUEST_ENTITY_TOO_LARGE_4_13 :
			case QAPI_UNSUPPORTED_MEDIA_TYPE_4_15 :

		    COAP_PRINTF("Processing options\n");
			process_options(messageP);
       	    COAP_PRINTF("Got response with client error response code \n");
       	   	break;
			
       	    case QAPI_INTERNAL_SERVER_ERROR_5_00 :
			case QAPI_NOT_IMPLEMENTED_5_01 :
			case QAPI_BAD_GATEWAY_5_02 :
			case QAPI_SERVICE_UNAVAILABLE_5_03 :
			case QAPI_GATEWAY_TIMEOUT_5_04 :
			case QAPI_PROXYING_NOT_SUPPORTED_5_05 :
            case QAPI_MEMORY_ALLOCATION_ERROR :

		    COAP_PRINTF("Processing options\n");
			process_options(messageP);	
       		COAP_PRINTF("Got response with server error response code \n");
       	   	break;       		
       	    default : 
       	  	COAP_PRINTF("Got unsupported coap message code from server\n");
			coap_response_code =QAPI_COAP_IGNORE;
	    	}

	    break;
	    case QAPI_COAP_TYPE_ACK :
	  	COAP_PRINTF("message type : ACK , message code : %u\n",messageP->code);
		COAP_PRINTF("Processing options\n");
	    process_options(messageP);	
	    break;
	    case QAPI_COAP_TYPE_RST :
		COAP_PRINTF("message type : RST , message code : %u\n",messageP->code);
		break;
	    default : 
	  	COAP_PRINTF("Message code unknown\n");
		coap_response_code =QAPI_COAP_IGNORE;
   }

end :

    if(messageP != NULL && message_type != QAPI_COAP_TYPE_RST)
    {
	   if (qapi_Coap_Free_Message(hdl , messageP) != QAPI_OK)
	   {
	      COAP_PRINTF("coap_handle_session_cb : qapi_Coap_Free_Message returned error\n" );
          coap_response_code =QAPI_COAP_IGNORE;
	   }
    }

	   if(payload != NULL)
	   coap_free(payload);
	   
       return coap_response_code;
}

 void Coap_Event_Callback(qapi_Coap_Session_Hdl_t hdl , qapi_Coap_Packet_t * coap_pkt , qapi_Coap_Event_t coap_event , void * event_info)
 {
	COAP_PRINTF("Coap_Event_Callback called\n");
 }


 void 
 coap_handle_transaction_cb(qapi_Coap_Session_Hdl_t hdl ,qapi_Coap_Transaction_t * transaction,
    qapi_Coap_Packet_t * message)
{

qapi_Coap_Transaction_t * transact;
qapi_Coap_Packet_t * coapMessage = NULL;
qapi_Coap_Status_t result;
char * payload = NULL;

int app_id = -1;

  
 COAP_PRINTF("coap_handle_transaction_cb called hdl %lu " , hdl);
 
 if((hdl == NULL) || (transaction == NULL))
 {
	   COAP_PRINTF("hdl == NULL) || (transaction == NULL)\n" );
	   return ;
 }

 transact = (qapi_Coap_Transaction_t *)transaction;
 coapMessage = (qapi_Coap_Packet_t *)message;

 COAP_PRINTF("Reply recevied from Server\n");
 

   if ((NULL == coapMessage || QAPI_FORBIDDEN_4_03 == coapMessage->code || QAPI_BAD_REQUEST_4_00 == coapMessage->code )) 
   {
	 if (coapMessage == NULL)
	 {
	   COAP_PRINTF("recevied NULL response from Server\n");

	 }
	 else
	 {
       COAP_PRINTF("Received 403 forbidden reponse\n");
	 }

	 return ;
   }
   else
   { 

	 if (NULL != coapMessage && QAPI_COAP_TYPE_RST != coapMessage->type)
	 {
	   COAP_PRINTF("Received NON null and non rst reponse from server \n");
	   COAP_PRINTF("coapMessage->type %d\n" , coapMessage->type);
	   COAP_PRINTF("coapMessage->code %d\n" , coapMessage->code);
       COAP_PRINTF("Processing options\n");
	   process_options(coapMessage);
	   COAP_PRINTF("\ntransact->ack_received %d\n" , transact->ack_received);
	   COAP_PRINTF("transact->mID %lu\n" , transact->mID);
	   COAP_PRINTF("coapMessage->payload_len %d\n" , coapMessage->payload_len );

	   app_id = get_appid(hdl);
	   
	   if(app_id == -1)
	   {
		  COAP_PRINTF("coap_handle_transaction_cb : get_appid returned -1\n");
		  goto end;
	   }
	   else
		  COAP_PRINTF("coap_handle_transaction_cb : handle %lu exists on app_id %d\n" , hdl , app_id);
		
       if(transact->ack_received == 1 && QAPI_IS_OPTION(message , QAPI_COAP_OPTION_BLOCK1))
       {
		 result = coap_block1_handler(hdl , coapMessage , app_id);
		 		 if(result == QAPI_NO_ERROR)
		 {
            COAP_PRINTF("coap_handle_transaction_cb : coap_block1_handler returned success with result %d\n" , result);
		 }
		 else
		 {
            COAP_PRINTF("coap_handle_transaction_cb : coap_block1_handler returned error with result %d\n" , result);
		 }
		 
	   } 
	   else if(transact->ack_received == 1 && QAPI_IS_OPTION(message , QAPI_COAP_OPTION_BLOCK2))
       { 
         result = coap_block2_handler(hdl , coapMessage , app_id);
		 if(result == QAPI_NO_ERROR)
		 {
            COAP_PRINTF("coap_handle_transaction_cb : coap_block2_handler returned success with result %d\n" , result);
		 }
		 else
		 {
            COAP_PRINTF("coap_handle_transaction_cb : coap_block2_handler returned error with result %d\n" , result);
		 }
	   } 
       else if(transact->ack_received == 1 && QAPI_IS_OPTION(message , QAPI_COAP_OPTION_OBSERVE))
       {
		 result = observe_notify_handler(hdl , coapMessage , app_id);
		 if(result == QAPI_NO_ERROR)
		 {
            COAP_PRINTF("coap_handle_transaction_cb : observe_notify_handler returned success with result %d\n" , result);
            goto end;
		 }
		 else
		 {
            COAP_PRINTF("coap_handle_transaction_cb : observe_notify_handler returned error with result %d\n" , result);
			goto end;
		 }
	   } 
	   
	   
	   if((char*)coapMessage->payload != NULL && coapMessage->payload_len > 0)
	   {
         payload = (char*)coap_malloc((coapMessage->payload_len) * sizeof(char));
		 if(payload != NULL)
		 {
          memcpy(payload, (char*)coapMessage->payload, coapMessage->payload_len);
		 }
		 else 
		 	COAP_PRINTF("Payload malloc allocation failed\n");
		 	

	   }
	 }
	 else
	 {
       COAP_PRINTF("QAPI_COAP_TYPE_RST == coapMessage->type");
	   return;
	 }

   }


end :

    
    if(NULL != transaction->message)
    {
        qapi_Coap_Free_Message(hdl,transaction->message);
        transaction->message = NULL;
    }

    if(NULL != coapMessage)
       qapi_Coap_Free_Message(hdl,coapMessage);

	 if(payload != NULL)
	 coap_free(payload);
}

QCLI_Command_Status_t create_session(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{

    uint32_t i , p;
	QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
	qapi_Coap_Session_Info_t session_params;
	bool enable_block_config = 0;
	int ret;

	if(app_coapcli_hndle[app_id] != NULL)
   {
     result = QCLI_STATUS_SUCCESS_E;
	 COAP_PRINTF("create_session : Session on app id %d already exists with session_handle %lu " , app_id , app_coapcli_hndle[app_id]);
	 goto end;
   }

    if((Parameter_Count > 10))
    {
        COAP_PRINTF("Usage: create_session [-app <app_id>] [-a <ack_random_factor>] [-m <max_retransmit>] [-M <max_age>] [-ml <max_latency>] [-t <transaction_timeout>][-le <enable last error>] , COAP CLI create_session command\n ");
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }

    MEMSET(&session_params, 0 , sizeof(qapi_Coap_Session_Info_t));
	
    session_params.coap_transaction_timeout=QAPI_COAP_RESPONSE_TIMEOUT;
	session_params.coap_max_retransmits=QAPI_COAP_MAX_RETRANSMIT;
	session_params.coap_default_maxage=QAPI_COAP_DEFAULT_MAX_AGE;
	session_params.coap_ack_random_factor=QAPI_COAP_ACK_RANDOM_FACTOR;
	session_params.coap_max_latency=100;
	session_params.clientData=NULL;
	session_params.cb=coap_handle_session_cb;

    
    for (i = 0; i < Parameter_Count; i++)
    {
        if (strcmp((char*)Parameter_List[i].String_Value, "-a") == 0)
        {
            ++i;
			if((char*)Parameter_List[i].String_Value)
			{
              session_params.coap_ack_random_factor = (double)Parameter_List[i].Integer_Value;
			}
			
        }
        else if (strcmp((char*)Parameter_List[i].String_Value, "-m") == 0)
        {
            ++i;
            if(Parameter_List[i].Integer_Is_Valid)
            {
                session_params.coap_max_retransmits = Parameter_List[i].Integer_Value;
			}
        }
        else if (strcmp((char*)Parameter_List[i].String_Value, "-M") == 0)
        {
            ++i;
            if (Parameter_List[i].Integer_Is_Valid)
            {
              session_params.coap_default_maxage = Parameter_List[i].Integer_Value;
            }
        }
        else if (strcmp((char*)Parameter_List[i].String_Value, "-ml") == 0)
        {
            ++i;
            if (Parameter_List[i].Integer_Is_Valid)
            {
               session_params.coap_max_latency = Parameter_List[i].Integer_Value;
                
            }
        }
        else if (strcmp((char*)Parameter_List[i].String_Value, "-t") == 0)
        {
            ++i;
            if (Parameter_List[i].Integer_Is_Valid)
            {
                session_params.coap_transaction_timeout = Parameter_List[i].Integer_Value;
                
            }
        }
        else if (strcmp((char*)Parameter_List[i].String_Value, "-e") == 0)
        {
            ++i;
            if (Parameter_List[i].Integer_Is_Valid)
            {
                enable_block_config = Parameter_List[i].Integer_Value;
            }
        }
		else if (strcmp((char*)Parameter_List[i].String_Value, "-le") == 0)
        {
            ++i;
            if (Parameter_List[i].Integer_Is_Valid)
            {
                if(Parameter_List[i].Integer_Value == 1)
                {
                	get_coap_ext_conf[app_id] = QAPI_NET_COAP_GET_EXTENDED_CONFIG_LAST_ERR_CODE;
                }
				else
				{
					get_coap_ext_conf[app_id] = QAPI_NET_COAP_GET_EXTENDED_CONFIG_MAX;
				}
            }
			else
			{
				get_coap_ext_conf[app_id] = QAPI_NET_COAP_GET_EXTENDED_CONFIG_MAX;
			}
        }
    }



   if ((qapi_Coap_Create_Session(&app_coapcli_hndle[app_id],&session_params)) != QAPI_OK)
   {
     result = QCLI_STATUS_ERROR_E;
	 COAP_PRINTF("create_session : returned error from qapi_Coap_Create_Session");
	 goto end;
   }

   if(enable_block_config)
   {
      if(qapi_Coap_Set_Extended_Config_Option(app_coapcli_hndle[app_id] , QAPI_COAP_EXTENDED_CONFIG_BLOCKWISE_HANDLE_DL_BY_COAP , NULL , sizeof(int))!=QAPI_OK)
         COAP_PRINTF("create_session : qapi_Coap_Set_Extended_Config_Option failed ");
   }

   ret =  qapi_Coap_Pass_Pool_Ptr(app_coapcli_hndle[app_id],byte_pool_qcli);

   
   if ( ret != QAPI_OK)
   {
     result = QCLI_STATUS_ERROR_E;
	 app_coapcli_hndle[app_id] = NULL;
	 COAP_PRINTF("create_session : returned error from qapi_Coap_Pass_Pool_Ptr\n");
	 goto end;
   }
   else 
   {
     if(app_coapcli_hndle[app_id] == NULL)
     {
       result = QCLI_STATUS_ERROR_E;
	   COAP_PRINTF("create_session : Session on app id %d not created coapcli_hndle is NULL\n" , app_id);
       goto end;
     }
     else
     {
       result = QCLI_STATUS_SUCCESS_E;
	   COAP_PRINTF("create_session : Session on app id %d created successfully with session_handle %lu " , app_id , app_coapcli_hndle[app_id]);
     }

     if (qapi_Coap_Register_Event(app_coapcli_hndle[app_id], Coap_Event_Callback) != QAPI_OK)
     {
       result = QCLI_STATUS_ERROR_E;
	   COAP_PRINTF("create_connection : qapi_Coap_Register_Event returned error ");
	   goto end;
	 
     }
     else
     {
       COAP_PRINTF("create_connection : qapi_Coap_Register_Event returned success\n ");
     }
  }
	

end:
  if (Parameter_List != NULL)
  {
    for (p = 0; p<Parameter_Count; p++)
    {
      if (Parameter_List[p].String_Value != NULL)
      {
        tx_byte_release(Parameter_List[p].String_Value);
        Parameter_List[p].String_Value = NULL;
      }
    }
  }

  return result;

}

QCLI_Command_Status_t create_connection(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
    int i , p;
	struct ip46addr addr;
	char *addr_ptr;
    QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
	qapi_Coap_Connection_Cfg_t connection_params;
    int index = 1;
	int dss_nonip_hndl = 0;
	
	qapi_Net_SSL_Config_t t_cfg ;
    qapi_Coap_Sec_Info_t t_ssl_cfg;
	
    if((Parameter_List == NULL))
    {
		COAP_PRINTF("Usage: create_connection [-app <app_id>] [-m <sec mode> 0 MODE_PSK|1 MODE_CERTIFICATE|2 MODE_NON SEC]  [-psk <psk_key id> e.g 100_psk.bin(default) | 101_psk.bin , only for psk mode] [-p <protocol> |0 UDP|1 NIDD] [-i <handle_index for non ip >] [-s <source ip>] [-sp <source port>] [-d <server ip>] [-dp <server port>]"
		"[-r <resumption enable> 0|1] [-t <resumption timout>] , COAP CLI create_connection command\n ");
        result = QCLI_STATUS_SUCCESS_E;
        goto end;
    }

	if(app_coapcli_hndle[app_id] == NULL || connection_status[app_id] == TRUE)
    {
         result = QCLI_STATUS_ERROR_E;
	     COAP_PRINTF("create_connection : either coap handle is NULL or connection already exists\n");
	     goto end;
    }


	
	MEMSET(&connection_params,0,sizeof(qapi_Coap_Connection_Cfg_t));
	
	connection_params.sec_Mode =QAPI_COAP_MODE_NONE;
	connection_params.proto = QAPI_COAP_PROTOCOL_UDP;
	connection_params.resumption_enabled = FALSE;
	connection_params.session_resumption_timeout = 0;
	connection_params.src_host = NULL;
	connection_params.src_port = 0;
	connection_params.dst_host = NULL;
	connection_params.dst_port =5683;
	connection_params.family_type = AF_INET;
	connection_params.callHndl = NULL;
    
    for (i = 0; i < Parameter_Count; i++)
    {
        if (strcmp((char*)Parameter_List[i].String_Value, "-m") == 0)
        {
            ++i;
			if(Parameter_List[i].Integer_Is_Valid  && Parameter_List[i].Integer_Value >=0  && Parameter_List[i].Integer_Value < 3)
			{

               switch(Parameter_List[i].Integer_Value)
               {
                 case 0:
                    connection_params.sec_Mode = QAPI_COAP_MODE_PSK ;
				 break;
				 case 1:
                    connection_params.sec_Mode = QAPI_COAP_MODE_CERTIFICATE ;
				 break;
				 case 2:
                    connection_params.sec_Mode = QAPI_COAP_MODE_NONE ;
				 break;
				 default:
				 	connection_params.sec_Mode =QAPI_COAP_MODE_NONE;

			   }
			}
			else
			{
			  COAP_PRINTF("Not a valid security mode value\n");
              result = QCLI_STATUS_USAGE_E;
              goto end;
			}            
			
        }
        else if (strcmp((char*)Parameter_List[i].String_Value, "-p") == 0)
        {
            ++i;
			if(Parameter_List[i].Integer_Is_Valid && Parameter_List[i].Integer_Value >=0 && Parameter_List[i].Integer_Value <=1)
			{ 
			  switch(Parameter_List[i].Integer_Value)
               {
                 case 0:
                    connection_params.proto = QAPI_COAP_PROTOCOL_UDP ;
				 break;
				 case 1:
				 	
                    connection_params.proto =  QAPI_COAP_PROTOCOL_NIDD ;
					connection_params.family_type = AF_NONIP;
				    COAP_PRINTF("connection_params.proto =  QAPI_COAP_PROTOCOL_NIDD\n");
					break;
				 default:
				 	connection_params.proto =QAPI_COAP_PROTOCOL_UDP;

			   }

			}
			else
			{
			  COAP_PRINTF("Not a valid protocol value");
              result = QCLI_STATUS_USAGE_E;
              goto end;
			}       
			
        }
		else if (strcmp((char*)Parameter_List[i].String_Value, "-i") == 0)
        {
             ++i;
             if ((Parameter_List[i].Integer_Is_Valid) && connection_params.family_type == AF_NONIP)
             {
             
                 if(Parameter_List[i].Integer_Value > 0)
				    index = Parameter_List[i].Integer_Value;
               	 dss_nonip_hndl = dss_get_data_call_handle_of_index(app_id , index-1);
				 connection_params.callHndl = dss_nonip_hndl;
				 if(connection_params.callHndl == 0)
				 {
                   	COAP_PRINTF("handle at the given index %d is NULL\n",index);
                    result = QCLI_STATUS_USAGE_E;
                    goto end;     
				 }
				 else
				    COAP_PRINTF(" create_connection call handle fot nonip : %d\n" , connection_params.callHndl);
              
            }
            else
            {
                COAP_PRINTF("Invalid handle index value or protocol is UDP\n");
                result = QCLI_STATUS_USAGE_E;
                goto end;
            }
        }
        else if (strcmp((char*)Parameter_List[i].String_Value, "-s") == 0)
        {
            ++i;
			addr_ptr = (char*)Parameter_List[i].String_Value;
			if (inet_pton(AF_INET, addr_ptr, &addr.a) != 0)   
            {
                if (inet_pton(AF_INET6, addr_ptr, &addr.a) != 0)      
                {
                    /* not IPv6 address */
                    COAP_PRINTF("Not an IPv4/6 address of source\n");
                    result = QCLI_STATUS_USAGE_E;
                    goto end;
                }
                else
                {   
                    connection_params.src_host = addr_ptr ;
					
					if(connection_params.proto == QAPI_COAP_PROTOCOL_NIDD)
					   connection_params.family_type = AF_NONIP;
					else
					   connection_params.family_type = AF_INET6;
                }
            } 
            else
            {
                connection_params.src_host = addr_ptr ;
				
			    if(connection_params.proto == QAPI_COAP_PROTOCOL_NIDD)
				   connection_params.family_type = AF_NONIP;
				else
				connection_params.family_type = AF_INET;
            }       
        }
        else if (strcmp((char*)Parameter_List[i].String_Value, "-sp") == 0)
        { 
            ++i;
            if (Parameter_List[i].Integer_Is_Valid)
            {
                connection_params.src_port = Parameter_List[i].Integer_Value;
            }
 
        }
        else if (strcmp((char*)Parameter_List[i].String_Value, "-d") == 0)
        {
            ++i;
			addr_ptr = (char*)Parameter_List[i].String_Value;
			if (inet_pton(AF_INET, addr_ptr, &addr.a) != 0)   
            {
                if (inet_pton(AF_INET6, addr_ptr, &addr.a) != 0)      
                {
                    /* not IPv6 address */
                    COAP_PRINTF("Not an IPv4/6 address of source\n");
                    result = QCLI_STATUS_USAGE_E;
                    goto end;
                }
                else
                {   
                    connection_params.dst_host = addr_ptr ;
					
				    if(connection_params.proto == QAPI_COAP_PROTOCOL_NIDD)
					   connection_params.family_type = AF_NONIP;
					else
					   connection_params.family_type = AF_INET6 ;
                }
            } 
            else
            {
                connection_params.dst_host = addr_ptr;
				if(connection_params.proto == QAPI_COAP_PROTOCOL_NIDD)
				   connection_params.family_type = AF_NONIP;
				else
				   connection_params.family_type = AF_INET;
            }   
        }
        else if (strcmp((char*)Parameter_List[i].String_Value, "-dp") == 0)
        {
            ++i;
            if (Parameter_List[i].Integer_Is_Valid)
            {
                connection_params.dst_port = Parameter_List[i].Integer_Value ;
            }           
        }		
        else if (strcmp((char*)Parameter_List[i].String_Value, "-r") == 0)
        {
            ++i;
             if (Parameter_List[i].Integer_Is_Valid && Parameter_List[i].Integer_Value >= 0 && Parameter_List[i].Integer_Value <= 1 )
            {
               if(Parameter_List[i].Integer_Value)
			      connection_params.resumption_enabled = TRUE;
			   else
			   	  connection_params.resumption_enabled = FALSE;
                
            }
            else
            {
                COAP_PRINTF("Invalid Resumption enable value");
                result = QCLI_STATUS_USAGE_E;
                goto end;
            }
        }
        else if(strcmp((char*)Parameter_List[i].String_Value, "-t") == 0)
        {
            ++i;
		    if (Parameter_List[i].Integer_Is_Valid && connection_params.resumption_enabled == TRUE)
		    {
		       if(Parameter_List[i].Integer_Value)
		       {
                  connection_params.session_resumption_timeout = Parameter_List[i].Integer_Value;
			   }
			   else
			   {
				  result = QCLI_STATUS_ERROR_E;
				  COAP_PRINTF(" Invalid Resumption enable value timeout %d\n");
				  goto end;

			   }
		    }
            
        }
    }

	if(connection_params.sec_Mode == QAPI_COAP_MODE_NONE)
	{
       connection_params.sec_info = NULL;
	   connection_params.ssl_cfg = NULL;
	}
    else if(connection_params.sec_Mode ==0 || connection_params.sec_Mode == 2)
    { 

	  COAP_PRINTF(" create_connection : sec mode\n");

	  if(coap_configure_ssl( connection_params.sec_Mode , &t_cfg, &t_ssl_cfg) != QAPI_OK)
	  {
	     result = QCLI_STATUS_ERROR_E;
	     COAP_PRINTF("coap_configure_ssl returned error");
	     goto end;
	  }
	  else
	  {
		 if(( &t_cfg ) && ( &t_ssl_cfg ))
		 {
			connection_params.sec_info = &t_ssl_cfg;
			connection_params.ssl_cfg = &t_cfg;
		 }
		 else
		 {
			result = QCLI_STATUS_ERROR_E;
			COAP_PRINTF("coap_configure_ssl returned success but security config structures may be NULL\n");
			goto end;
		 
		 }

      }
       
	}
	else
	{
     result = QCLI_STATUS_ERROR_E;
	 COAP_PRINTF("sec_Mode entered is not supported ",result);
	 goto end; 
	}

	if((!connection_params.dst_host) && connection_params.family_type != AF_NONIP)
	{
       result = QCLI_STATUS_ERROR_E;
	   COAP_PRINTF(" destination host cannot be NULL\n ");
	   goto end;
	}

	if(qapi_Coap_Create_Connection(app_coapcli_hndle[app_id],&connection_params) != QAPI_OK)
	{
       result = QCLI_STATUS_ERROR_E;
	   COAP_PRINTF("create_connection : returned error from qapi_Coap_Create_Connection \n");
	}
	else
	{
       COAP_PRINTF("qapi_Coap_Create_Connection returned success \n");
	   connection_status[app_id] = TRUE;
	   result = QCLI_STATUS_SUCCESS_E;
	}


end:
	  if (Parameter_List != NULL)
	  {
		for (p = 0; p<Parameter_Count; p++)
		{
		  if (Parameter_List[p].String_Value != NULL)
		  {
			tx_byte_release(Parameter_List[p].String_Value);
			Parameter_List[p].String_Value = NULL;
		  }
		}
	  }
	  
	  return result;

}

	QCLI_Command_Status_t init_message(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
	{

        int i,p;
		QCLI_Command_Status_t result =	QCLI_STATUS_SUCCESS_E;
        qapi_Coap_Message_Type_t msg_type = QAPI_COAP_TYPE_CON;
	    qapi_Coap_Method_t method_type = QAPI_COAP_GET;	
		qapi_Status_t return_value = QAPI_ERROR;
  
	   if(app_coapcli_hndle[app_id] == NULL)
       {
         result = QCLI_STATUS_ERROR_E;
	     COAP_PRINTF("init_message : coapcli_hndle[app_id] == NULL ");
	     goto end;
       }
		
		if((Parameter_Count < 2) || (Parameter_Count > 4) || (Parameter_List == NULL))
		{
			COAP_PRINTF("Usage: init_message [-app <app_id>] [-t msg type | 0 CON | 1 NON CON  | 2 ACK | 3 RST] [-m coap_method_type | 1 GET |2 POST | 3 PUT |4 DELETE] , COAP CLI init_message command\n ");
		    result = QCLI_STATUS_ERROR_E;
			goto end;
		}


        MEMSET(&message_param[app_id],0,sizeof(message_param[app_id]));
        init_global_variable(app_id);
		
		observe_type[app_id] = QAPI_COAP_TYPE_CON;
		
		for (i = 0; i < Parameter_Count; i++)
		{

			if (strcmp((char*)Parameter_List[i].String_Value, "-t") == 0)
			{
				++i;

                if(Parameter_List[i].Integer_Value == 0)
				  {
                   msg_type = QAPI_COAP_TYPE_CON ; 
                   message_param[app_id].msg_cb = coap_handle_transaction_cb ;
				}
				else
				{
                  msg_type = (qapi_Coap_Message_Type_t)Parameter_List[i].Integer_Value;
                  message_param[app_id].msg_cb = NULL ;
				
			    }
			}
			else if (strcmp((char*)Parameter_List[i].String_Value, "-m") == 0)
			{
				++i;
				if(Parameter_List[i].Integer_Is_Valid && Parameter_List[i].Integer_Value >=0)
				{ 
				  switch(Parameter_List[i].Integer_Value)
				   {
					 case 1:
						 method_type = QAPI_COAP_GET; 
					 break;
					 case 2:
						 method_type = QAPI_COAP_POST; 
						break;
					 case 3:
						 method_type = QAPI_COAP_PUT; 
						break;
					 case 4:
						 method_type = QAPI_COAP_DELETE; 
				     default:
				         method_type = (qapi_Coap_Method_t)Parameter_List[i].Integer_Value;
				   }
				}
				else
				{
				  COAP_PRINTF("Not a valid method_type value");
				  result = QCLI_STATUS_USAGE_E;
				  goto end;
				}		
				
			}		
			
		}

		return_value = qapi_Coap_Init_Message(app_coapcli_hndle[app_id],&messageP[app_id],msg_type,method_type);
	
		if(return_value != QAPI_OK)
        {
	       result = QCLI_STATUS_ERROR_E;
		   COAP_PRINTF(" qapi_Coap_Init_Message returned error with return response = %d\n" , return_value);
	       goto end;
        }
		else
	    {
           COAP_PRINTF("Init_message executed successfully with return response = %d\n" ,return_value);
		}


		if(messageP[app_id] == NULL)
		{
           result = QCLI_STATUS_ERROR_E;
		   COAP_PRINTF("Init_message executed successfully but messageP[%d] is NULL \n" , app_id);
	       goto end;
		}
	end:
		  if (Parameter_List != NULL)
		  {
			for (p = 0; p<Parameter_Count; p++)
			{
			  if (Parameter_List[p].String_Value != NULL)
			  {
				tx_byte_release(Parameter_List[p].String_Value);
				Parameter_List[p].String_Value = NULL;
			  }
			}
		  }
		
		  return result;
	
	}

	QCLI_Command_Status_t set_header(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
	{
	
        uint32_t i , p;
		uint16_t header_type = QAPI_COAP_URI_PATH;
        int value = 0;
		const void * header_val = NULL;
		size_t val_len = 0;
		
		QCLI_Command_Status_t result =	QCLI_STATUS_SUCCESS_E;
		qapi_Coap_Block_Params_t block2_header_val= {0};
		qapi_Coap_Block_Params_t block1_header_val= {0};
		block1_size[app_id] = 0;
		block2_size[app_id] = 0;
		
		if((Parameter_Count < 4) || (Parameter_List == NULL))
		{
			COAP_PRINTF("Usage: set_header [-app <app_id>] [-t <header type >] [-v <value>] [-l <length>] , COAP CLI setheader command\n ");
			result = QCLI_STATUS_SUCCESS_E;
			goto end;
		}

        if(app_coapcli_hndle[app_id] == NULL || messageP[app_id] == NULL)
      {
         result = QCLI_STATUS_ERROR_E;
	     COAP_PRINTF("set_header : coapcli_hndle[app_id] == NULL \n");
	     goto end;
      }
		
        for (i = 0; i < Parameter_Count; i++)
        {
              
		   if (strcmp((char*)Parameter_List[i].String_Value, "-t") == 0)
           {
            ++i;
			
			if(Parameter_List[i].Integer_Is_Valid && Parameter_List[i].Integer_Value >= 0 && Parameter_List[i].Integer_Value <= 12  )
			{
 
			  switch (Parameter_List[i].Integer_Value)
              {  
                 case 0:
                    header_type = QAPI_COAP_URI_PATH ;
				    MEMSET(uri_path_block[app_id] , 0 , MAX_RESOURCE_NAME_LEN);
		            uri_path_block_len[app_id] = 0;
                 break;
				 case 1:
				 	header_type = QAPI_COAP_URI_QUERY;
				 break;
				 case 2:
				 	header_type = QAPI_COAP_CONTENT_TYPE;
				 break;
				 case 3:
				 	header_type = QAPI_COAP_TOKEN;
				 break;
				 case 4:
				 	header_type = QAPI_COAP_OBSERVE;
				 break;
				 case 5:
				 	header_type = QAPI_COAP_LOCATION_PATH;
				 break;
				 case 6:
				 	header_type = QAPI_COAP_SET_STATUS;
				 break;
				 case 7:
				 	header_type = QAPI_COAP_MULTI_OPTION;
				 break;
				 case 8:
				 	header_type = QAPI_COAP_BLOCK1;
				 break;
				 case 9:
				 	header_type = QAPI_COAP_BLOCK2;
				 break;
			     case 10:
         	 	    header_type = QAPI_COAP_SIZE1;  /**< BLOCK SIZE1. */
			      break;
			     case 11:
         	 	    header_type =   QAPI_COAP_SIZE; /**< BLOCK SIZE. */
			     break;
			  }            			
           
		   }
		   else if((Parameter_List[i].Integer_Is_Valid))
		   {
		     header_type = Parameter_List[i].Integer_Value;
		   }

		   }
           else if ((strcmp((char*)Parameter_List[i].String_Value, "-v")) == 0)
           {
            ++i;

			if(header_type == QAPI_COAP_BLOCK2)
		    {

                 MEMSET(&block2_header_val,0,sizeof(qapi_Coap_Block_Params_t));
          	     if (Parameter_List[i].Integer_Is_Valid)
          	     {
                     block2_header_val.more=0;
          	         block2_header_val.num=0;
          	         block2_header_val.size = Parameter_List[i].Integer_Value;
					 //COAP_PRINTF(" block2_header_val.size value = %d " ,block2_header_val.size);
          			 block2_header_val.offset = 0;
          	     }
          	     else
          	     {
                     COAP_PRINTF(" init_message invalid  block2_header_val.size value ");
          	         result = QCLI_STATUS_USAGE_E;
          	         goto end;
          	     }

			}
			else if(header_type == QAPI_COAP_TOKEN)
			{

			header_val = (uint8_t *)Parameter_List[i].String_Value;
			val_len = strlen((char*)Parameter_List[i].String_Value);
		
			
			}
			else if(header_type == QAPI_COAP_BLOCK1)
			{

				 MEMSET(&block1_header_val,0,sizeof(qapi_Coap_Block_Params_t));
          	     if (Parameter_List[i].Integer_Is_Valid)
          	     {
                     block1_header_val.more=0;
          	         block1_header_val.num=0;
          	         block1_header_val.size = Parameter_List[i].Integer_Value;
          			 block1_header_val.offset = 0;
          	     }
          	     else
          	     {
                     COAP_PRINTF(" init_message invalid  block1_header_val.size value ");
          	         result = QCLI_STATUS_USAGE_E;
          	         goto end;
          	     }


			}
			else if(header_type == QAPI_COAP_OBSERVE || header_type == QAPI_COAP_SIZE1 || header_type ==  QAPI_COAP_SIZE || header_type == QAPI_COAP_CONTENT_TYPE)
			{
                value = Parameter_List[i].Integer_Value;
			}
			else if(header_type > QAPI_COAP_SIZE && (header_type == 80 || header_type == 258 || header_type == 120))
			{
			  if(Parameter_List[i].Integer_Is_Valid)
			  {
			    SET_EXTENDED_HEADER_DATA(INTEGER, (const char*)Parameter_List[i].String_Value,
			                              0, header_val, val_len);
			  }
			  else
			  {
			    SET_EXTENDED_HEADER_DATA(STRING, (const char*)Parameter_List[i].String_Value,
			                             strlen((char*)Parameter_List[i].String_Value),
			                             header_val, val_len);
			  }
			}
			else
			{
            header_val = (char*)Parameter_List[i].String_Value;
			val_len = strlen((char*)Parameter_List[i].String_Value);
			}
           }
           else if ((strcmp((char*)Parameter_List[i].String_Value, "-l")) == 0)
           {
            ++i;
            if (Parameter_List[i].Integer_Is_Valid)
            {
              if(Parameter_List[i].Integer_Value > val_len)
			     val_len = Parameter_List[i].Integer_Value;

			  if(header_type == QAPI_COAP_URI_PATH)
			  uri_path_block_len[app_id] = val_len;
              else if(header_type == QAPI_COAP_BLOCK1 && val_len > 0)
			  block1_header_val.more=1;

			  if(header_type == QAPI_COAP_OBSERVE)
			  {
                total_observe_count[app_id] = Parameter_List[i].Integer_Value;
				current_observe_count[app_id] = 0;
			  }

            }
           }

        }

	    if(header_type > QAPI_COAP_SIZE && (header_type == 80 || header_type == 258 || header_type == 120))
        {
          if (qapi_Coap_Set_Header_Ext(app_coapcli_hndle[app_id], messageP[app_id] , header_type , header_val, val_len)!=QAPI_OK)
	     {
           COAP_PRINTF("qapi_Coap_Set_Header_Ext returned error\n");
		   result = QCLI_STATUS_ERROR_E;
		   goto end;
	     }
        }

		else if(header_type == QAPI_COAP_BLOCK2)
		{
		
		 if (qapi_Coap_Set_Header(app_coapcli_hndle[app_id], messageP[app_id] , (qapi_Coap_Header_type)header_type , &block2_header_val, sizeof(qapi_Coap_Block_Params_t))!=QAPI_OK)
	     {
           COAP_PRINTF("qapi_Coap_Set_Header returned error\n");
		   result = QCLI_STATUS_ERROR_E;
		   goto end;
	     }
		 else
		 {

           block2_size[app_id] = block2_header_val.size;   
	     }

		}
		else if(header_type == QAPI_COAP_OBSERVE || header_type == QAPI_COAP_SET_STATUS || header_type == QAPI_COAP_SIZE1 || header_type == QAPI_COAP_SIZE || header_type == QAPI_COAP_CONTENT_TYPE )
	    {
				 if (qapi_Coap_Set_Header(app_coapcli_hndle[app_id], messageP[app_id] , (qapi_Coap_Header_type)header_type , &value, sizeof(int))!=QAPI_OK)
				 {
				   COAP_PRINTF("qapi_Coap_Set_Header returned error\n");
				   result = QCLI_STATUS_ERROR_E;
				   goto end;
				 }

		}
		else if(header_type == QAPI_COAP_BLOCK1)
		{
           
				 if (qapi_Coap_Set_Header(app_coapcli_hndle[app_id], messageP[app_id] , (qapi_Coap_Header_type)header_type , &block1_header_val, sizeof(qapi_Coap_Block_Params_t))!=QAPI_OK)
				 {
				   COAP_PRINTF("qapi_Coap_Set_Header returned error\n");
				   result = QCLI_STATUS_ERROR_E;
				   goto end;
				 }
				 else
				 {
				    block1_size[app_id] =  block1_header_val.size;
				   
				 }

		}
        else 
        {

         COAP_PRINTF("qapi_Coap_Set_Header : header_type = %d\n" , header_type);
		
        if (qapi_Coap_Set_Header(app_coapcli_hndle[app_id], messageP[app_id] , (qapi_Coap_Header_type)header_type , header_val, val_len)!=QAPI_OK)
	    {
           COAP_PRINTF("qapi_Coap_Set_Header returned error\n");
		   result = QCLI_STATUS_ERROR_E;
	    }
        else
		{
		   if(header_type == QAPI_COAP_URI_PATH)
		   {
             if(header_val != NULL && uri_path_block_len[app_id] > 0 && uri_path_block_len[app_id] <= MAX_RESOURCE_NAME_LEN )
             { 
			   strlcpy(uri_path_block[app_id] , header_val , uri_path_block_len[app_id]+1);
			   uri_path_block[app_id][uri_path_block_len[app_id]+1] = '\0';
			 }

		   }
		   
	     }
        }
	
	end:
	  if (Parameter_List != NULL)
	  {
		for (p = 0; p<Parameter_Count; p++)
		{
		  if (Parameter_List[p].String_Value != NULL)
		  {
			tx_byte_release(Parameter_List[p].String_Value);
			Parameter_List[p].String_Value = NULL;
		  }
		}
	  }
	
	  return result;
	
}

QCLI_Command_Status_t get_block_info(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;
  uint32_t p = 0;
  qapi_Coap_Block_Transfer_Info_t block_info;
  qapi_Coap_Block_Transfer_Info_t * block_infoP = &block_info;
  
  if(qapi_Coap_Get_Block_Transfer_Info(app_coapcli_hndle[app_id] , &block_infoP) != QAPI_OK)
    result = QCLI_STATUS_ERROR_E;
  else
  {
    COAP_PRINTF("Printing block_info params\n");
    COAP_PRINTF("block_info->blockType = %d\n" , block_infoP->blockType);
    COAP_PRINTF("block_info->mID = %d\n" , block_infoP->mID);
    COAP_PRINTF("block_info->block_size = %d\n" , block_infoP->block_size);
    COAP_PRINTF("block_info->first_bl_time = %lu\n" , block_infoP->first_bl_time);
    COAP_PRINTF("block_info->numBlockSent = %d\n" , block_infoP->numBlockSent);
    COAP_PRINTF("block_info->numBlockPending = %d\n" , block_infoP->numBlockPending);
  }

  COAP_PRINTF("get_block_info returned with result = %d\n",result);
 
  if (Parameter_List != NULL)
  {
    for (p = 0; p<Parameter_Count; p++)
    {
      if (Parameter_List[p].String_Value != NULL)
      {
    	tx_byte_release(Parameter_List[p].String_Value);
    	Parameter_List[p].String_Value = NULL;
      }
    }
  }
  
  return result;
}


QCLI_Command_Status_t get_header(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
	
        uint32_t i = 0 , p = 0;
		uint16_t header_type = QAPI_COAP_URI_PATH;
		char * str_val = NULL;
		char ** str_val_dp =NULL;
		int option_int_val = 0;
		int * header_value = NULL;
		qapi_Coap_Block_Params_t block_val = {0};
		qapi_Coap_Block_Params_t * block_val1 = NULL;
		size_t val_len = 0;
		QCLI_Command_Status_t result =	QCLI_STATUS_SUCCESS_E;

		
		if((Parameter_Count < 1) || (Parameter_List == NULL))
		{
			COAP_PRINTF("Usage: get_header [-app <app_id>] [-t <header type >] , COAP CLI setheader command\n ");
			result = QCLI_STATUS_SUCCESS_E;
			goto end;
		}

        if(app_coapcli_hndle[app_id] == NULL || messageP[app_id] == NULL)
        {
         result = QCLI_STATUS_ERROR_E;
	     COAP_PRINTF("set_header : coapcli_hndle[app_id] == NULL \n");
	     goto end;
        }
		
        for (i = 0; i < Parameter_Count; i++)
        {
              
		   if (strcmp((char*)Parameter_List[i].String_Value, "-t") == 0)
           {
            ++i;
			
			if(Parameter_List[i].Integer_Is_Valid && Parameter_List[i].Integer_Value >= 0 && Parameter_List[i].Integer_Value <= 12  )
			{
	
			  switch (Parameter_List[i].Integer_Value)
              {  
				 case 0:
                    header_type = QAPI_COAP_URI_PATH;
            		 break;
            	  case 1:
            		 header_type = QAPI_COAP_URI_QUERY;
            		 break;
            	  case 3:
            		 header_type = QAPI_COAP_TOKEN;
            		 break;
            	  case 5:
            		 header_type = QAPI_COAP_LOCATION_PATH;
					break; 
            	  case 2:
            		 header_type = QAPI_COAP_CONTENT_TYPE;
            		 break;
            	  case 4:
            		 header_type = QAPI_COAP_OBSERVE;
            		 break;
            	  case 10:
            		 header_type = QAPI_COAP_SIZE1;
            		 break;
            	  case 11:
            		 header_type = QAPI_COAP_SIZE;			 
            		 break ;			  
            	  case 8:
            		 header_type = QAPI_COAP_BLOCK1;
            		 break;
            	  case 9:
            		 header_type = QAPI_COAP_BLOCK2;
            	  break;
			   }	
		     }
		     else if(Parameter_List[i].Integer_Is_Valid)
			 	{
			 	  header_type = Parameter_List[i].Integer_Value;
			 	}
		   	  }
		   }

		  if(header_type > QAPI_COAP_SIZE && (header_type == 80 || header_type == 258 || header_type == 120))
        	{
        	  qapi_Coap_Ext_Msg_Option_t *ext_option = NULL;
              uint8_t all = 0;
        	  ext_option = (qapi_Coap_Ext_Msg_Option_t *)coap_malloc(sizeof(qapi_Coap_Ext_Msg_Option_t));
        	  if(ext_option == NULL)
        	    goto end;

        	  memset(ext_option, 0x00, sizeof(qapi_Coap_Ext_Msg_Option_t));

        	   if(qapi_Coap_Get_Header_Ext(app_coapcli_hndle[app_id], messageP[app_id] , &ext_option, header_type, all) !=QAPI_OK)
    		  {
    			   COAP_PRINTF("get_header returned error for header type = %d\n " , header_type);
    			   coap_free(ext_option);
				   result = QCLI_STATUS_ERROR_E;
    			   goto end;
    		  }
    		  else
    			    { 
    			      size_t dst_len=0;
    			      uint8_t dst[100] = {0};
					  qapi_Coap_Ext_Msg_Option_t *options = ext_option;
					  
    			      while(options != NULL)
    			      {
    			        if(options->val && options->option_num == 258)
    			        {
    			          GET_EXTENDED_HEADER_DATA(INTEGER, options, NULL, dst, dst_len);
    			        }
    			        else if(options->val && options->option_num == 120)
    			        {
    			          GET_EXTENDED_HEADER_DATA(STRING_SPLIT_CHAR, options, '&', dst, dst_len);
    			        }
    			        else if(options->val && options->option_num == 80)
    			        {
    			          GET_EXTENDED_HEADER_DATA(STRING, options, NULL, dst, dst_len);
    			        }
    			        options = options->next;
    			      }
    			      if(dst)
                        COAP_PRINTF("get_header : header val %s\n" ,dst);
                      options = ext_option;
                      
                      while(options != NULL)
                      {
                        ext_option = options->next;

                        if(options->val)
                          coap_free(options->val);

                        if(options)
                          coap_free(options);

                        options = ext_option;
                      }
    				}
        	}

			  else if(header_type == QAPI_COAP_BLOCK1 || header_type == QAPI_COAP_BLOCK2 )
		  {

			      block_val1 = &block_val;
		
    			  if(qapi_Coap_Get_Header(app_coapcli_hndle[app_id], messageP[app_id] , (qapi_Coap_Header_type) header_type, (void ** )&block_val1 , &val_len) !=QAPI_OK)
				{
				   COAP_PRINTF("get_header returned error for header type = %d\n " , header_type);
				   result = QCLI_STATUS_ERROR_E;
				   goto end;
				}
				else
				{ 
				  COAP_PRINTF("get_header returned success for header type = %d\n " , header_type);
    			  COAP_PRINTF("get_header : num = %d , more = %d , size =%d\n" ,block_val1->num , block_val1->more , block_val1->size);
				  COAP_PRINTF("get_header : header length = %d\n " , val_len);
				}
		  }
		  else if(header_type == QAPI_COAP_CONTENT_TYPE || header_type == QAPI_COAP_OBSERVE || header_type == QAPI_COAP_SIZE1 ||  header_type == QAPI_COAP_SIZE)
		  {
			header_value = &option_int_val;

			if(qapi_Coap_Get_Header(app_coapcli_hndle[app_id], messageP[app_id] , (qapi_Coap_Header_type) header_type, (void ** )&header_value , &val_len) !=QAPI_OK)
			{
			   COAP_PRINTF("get_header returned error for header type = %d\n " , header_type);
			   result = QCLI_STATUS_ERROR_E;
			   goto end;
			}
			else
			{
			  COAP_PRINTF("get_header returned success for header type = %d\n " , header_type);
		      COAP_PRINTF("get_header : header value = %d , option_int_val = %d\n " , *((int*)header_value) , option_int_val);
			  COAP_PRINTF("get_header : header length = %d\n " , val_len);
			}

		}
		else if(header_type ==QAPI_COAP_URI_PATH || header_type == QAPI_COAP_URI_QUERY|| header_type == QAPI_COAP_TOKEN || header_type == QAPI_COAP_LOCATION_PATH || header_type ==  QAPI_COAP_MULTI_OPTION)
		{

		    str_val_dp = &str_val;
 
			if(qapi_Coap_Get_Header(app_coapcli_hndle[app_id], messageP[app_id] , (qapi_Coap_Header_type) header_type, (void ** )str_val_dp, &val_len) !=QAPI_OK)
			{
			   COAP_PRINTF("get_header returned error for header type = %d\n " , header_type);
			   result = QCLI_STATUS_ERROR_E;
			   goto end;
			}
			else
			{
	
			  COAP_PRINTF("get_header returned success for header type = %d\n " , header_type);
              COAP_PRINTF("get_header : header value = %s\n " , (char*)str_val);
			  COAP_PRINTF("get_header : header length = %d\n " , val_len);
			  
			}
		}
		else
		{
			COAP_PRINTF("get_header : undefined header type = %d\n " , header_type);
			result = QCLI_STATUS_ERROR_E;
			goto end;

		}

	end:

	  if((header_type ==QAPI_COAP_URI_PATH || header_type == QAPI_COAP_URI_QUERY || header_type == QAPI_COAP_LOCATION_PATH) && str_val != NULL )
	  {
        tx_byte_release(str_val);
	  }

	  if (Parameter_List != NULL)
	  {
		for (p = 0; p<Parameter_Count; p++)
		{
		  if (Parameter_List[p].String_Value != NULL)
		  {
			tx_byte_release(Parameter_List[p].String_Value);
			Parameter_List[p].String_Value = NULL;
		  }
		}
	  }
	
	  return result;
	
}

QCLI_Command_Status_t send_message_v2(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
    
    uint32_t i , p;
	char *payload =NULL ;
	size_t payload_len = 0;
    uint8_t is_payload_malloced = 0;
    QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
    qapi_Coap_Block_Wise_Options_t blocktype = QAPI_COAP_BLOCK_OPTION_NONE;
    uint16_t blocksize = 16;

    if((Parameter_List == NULL))
    {
        COAP_PRINTF("Usage: send_message_v2 [-app <app_id>] [-m <mid> max 16 bit unsigned int] [-t <token id>] [-l <token length>] [-p <payload>] [-L <payload length>]");
        result = QCLI_STATUS_SUCCESS_E;
        goto end;
    }


	if(app_coapcli_hndle[app_id] == NULL || messageP[app_id] == NULL || connection_status[app_id] == FALSE)
    {
     result = QCLI_STATUS_ERROR_E;
	 COAP_PRINTF("send_message_v2  : coap hndl == NULL or init_message not called or  connection not created\n");
	 goto end;
    }

    message_param[app_id].lastmid =0;
	message_param[app_id].token =0;
	message_param[app_id].token_len =0;
	
    for (i = 0; i < Parameter_Count; i++)
    {
       

        if (strcmp((char*)Parameter_List[i].String_Value, "-m" ) == 0)
        {
            ++i;
            if(Parameter_List[i].Integer_Is_Valid && Parameter_List[i].Integer_Value >=0  && Parameter_List[i].Integer_Value <=65535)
            {
              message_param[app_id].lastmid = (uint16_t *)&Parameter_List[i].Integer_Value;
            }
			else
			{
			   COAP_PRINTF("Invalid Message id value");
			   result = QCLI_STATUS_USAGE_E;
			   goto end;
			}
            
        }
        else if (strcmp((char*)Parameter_List[i].String_Value, "-t") == 0)
        {
            ++i;
           

               message_param[app_id].token =(uint8_t *)Parameter_List[i].String_Value;
			   message_param[app_id].token_len  = strlen((char*)Parameter_List[i].String_Value);
        }
        else if (strcmp((char*)Parameter_List[i].String_Value, "-l") == 0)
        {
            ++i;
            if(Parameter_List[i].Integer_Is_Valid)
            {

               message_param[app_id].token_len = Parameter_List[i].Integer_Value;
			
            }
			else
			{
			   COAP_PRINTF("Invalid token length value");
			   result = QCLI_STATUS_USAGE_E;
			   goto end;
			}
        }
        else if (strcmp((char*)Parameter_List[i].String_Value, "-btype") == 0)
        {
            ++i;
            if(Parameter_List[i].Integer_Is_Valid)
            {
              blocktype =  (qapi_Coap_Block_Wise_Options_t) Parameter_List[i].Integer_Value;
            }
            else
            {
               COAP_PRINTF("Invalid token length value");
               result = QCLI_STATUS_USAGE_E;
               goto end;
            }
        }
        else if (strcmp((char*)Parameter_List[i].String_Value, "-bsize") == 0)
        {
           ++i;
           if(Parameter_List[i].Integer_Is_Valid)
           {
              blocksize = Parameter_List[i].Integer_Value;
           }
           else
           {
              COAP_PRINTF("Invalid token length value");
              result = QCLI_STATUS_USAGE_E;
              goto end;
           }
        }
        else if(strcmp((char*)Parameter_List[i].String_Value, "-p") == 0)
        {
            ++i;
            if(Parameter_List[i].String_Value != NULL)
            {

			  payload = (char*)Parameter_List[i].String_Value ;
               
            }
            else
            {
              COAP_PRINTF("Invalid payload string value");
			   result = QCLI_STATUS_USAGE_E;
			   goto end;
            }
            
        }
        else if(strcmp((char*)Parameter_List[i].String_Value, "-L") == 0)
        {
             ++i;
            if(Parameter_List[i].Integer_Is_Valid)
            {

              payload_len = Parameter_List[i].Integer_Value;
			  COAP_PRINTF("payload length value payload_length %d ", payload_len);
			
            }
			else
			{
			   COAP_PRINTF("Invalid payload length value");
			   result = QCLI_STATUS_USAGE_E;
			   goto end;
			}
            
        	}   
   
    }


if(payload)
{
    if(block1_size[app_id] > 0 &&  block1_size[app_id] % 16 == 0)
    {
      strlcpy(send_block1_buffer[app_id] , payload , payload_len);
	  COAP_PRINTF("send_message : total send_block1_buffer[app_id] = %s , payload_len %d\n" ,send_block1_buffer[app_id],payload_len);
	  payload = (char *)coap_malloc(((block1_size[app_id])+1)*sizeof(char));
	  if(payload != NULL)
	  {
       MEMSET(payload , 0 , (block1_size[app_id])+1);
	   strlcpy(payload , send_block1_buffer[app_id] , block1_size[app_id]+1);
	   is_payload_malloced = 1;
	   block1_buffer_total_len[app_id] = payload_len;
	   payload_len = block1_size[app_id];
	   COAP_PRINTF("send_message :total_buffer_len_to_put = %d , new payload_len %d\n" , block1_buffer_total_len[app_id] ,payload_len);
	   COAP_PRINTF("send_message :  payload block1 or packet 1 = %s\n" , payload);
	  }
	  else
	  {
       result = QCLI_STATUS_ERROR_E;
	   COAP_PRINTF("send_message :  payload malloc allocation failure\n");
	   goto end;
	  }

    }
 
	if(qapi_Coap_Set_Payload(app_coapcli_hndle[app_id],messageP[app_id], payload, payload_len) != QAPI_OK)
	{
       result = QCLI_STATUS_ERROR_E;
	   COAP_PRINTF("qapi_Coap_Set_Payload returned error");
	   goto end;
	}
}

  if(qapi_Coap_Send_Message_v2(app_coapcli_hndle[app_id] ,messageP[app_id], &message_param[app_id] , blocktype , blocksize) != QAPI_OK)
  {
	result = QCLI_STATUS_ERROR_E;
     COAP_PRINTF("qapi_Coap_Send_Message_v2 returned error");
	goto end;
  }
  else
  {
    if(block1_size[app_id] > 0 &&  block1_size[app_id] % 16 == 0)
    send_block1_buffer_len[app_id] = block1_size[app_id];
  	COAP_PRINTF("qapi_Coap_Send_Message_v2 returned success");
   	if(is_payload_malloced == 1)
      coap_free(payload);
	else
		payload = NULL;
}

end:

  if(messageP[app_id] != NULL && messageP[app_id]->type == QAPI_COAP_TYPE_NON)
  {

        qapi_Coap_Free_Message(app_coapcli_hndle[app_id],messageP[app_id]);
        messageP[app_id] = NULL;
  }

  if (Parameter_List != NULL)
  {
    for (p = 0; p<Parameter_Count; p++)
    {
      if (Parameter_List[p].String_Value != NULL)
      {
        tx_byte_release(Parameter_List[p].String_Value);
        Parameter_List[p].String_Value = NULL;
      }
    }
  }
  return result;

}


QCLI_Command_Status_t send_message(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
    
    uint32_t i , p;
	char *payload =NULL ;
	size_t payload_len = 0;
    uint8_t is_payload_malloced = 0;
    QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
	
    if((Parameter_List == NULL))
    {
        COAP_PRINTF("Usage: send_message [-app <app_id>] [-m <mid> max 16 bit unsigned int] [-t <token id>] [-l <token length>] [-p <payload>] [-L <payload length>]");
        result = QCLI_STATUS_SUCCESS_E;
        goto end;
    }


	if(app_coapcli_hndle[app_id] == NULL || messageP[app_id] == NULL || connection_status[app_id] == FALSE)
    {
     result = QCLI_STATUS_ERROR_E;
	 COAP_PRINTF("send_message  : coap hndl == NULL or init_message not called or connection not created\n");
	 goto end;
    }

    message_param[app_id].lastmid =0;
	message_param[app_id].token =0;
	message_param[app_id].token_len =0;
	
    for (i = 0; i < Parameter_Count; i++)
    {
       

        if (strcmp((char*)Parameter_List[i].String_Value, "-m" ) == 0)
        {
            ++i;
            if(Parameter_List[i].Integer_Is_Valid && Parameter_List[i].Integer_Value >=0  && Parameter_List[i].Integer_Value <=65535)
            {
              message_param[app_id].lastmid = (uint16_t *)&Parameter_List[i].Integer_Value;
            }
			else
			{
			   COAP_PRINTF("Invalid Message id value");
			   result = QCLI_STATUS_USAGE_E;
			   goto end;
			}
            
        }
        else if (strcmp((char*)Parameter_List[i].String_Value, "-t") == 0)
        {
            ++i;
           

               message_param[app_id].token =(uint8_t *)Parameter_List[i].String_Value;
			   message_param[app_id].token_len  = strlen((char*)Parameter_List[i].String_Value);
        }
        else if (strcmp((char*)Parameter_List[i].String_Value, "-l") == 0)
        {
            ++i;
            if(Parameter_List[i].Integer_Is_Valid)
            {

               message_param[app_id].token_len = Parameter_List[i].Integer_Value;
			
            }
			else
			{
			   COAP_PRINTF("Invalid token length value");
			   result = QCLI_STATUS_USAGE_E;
			   goto end;
			}
        }
        else if(strcmp((char*)Parameter_List[i].String_Value, "-p") == 0)
        {
            ++i;
            if(Parameter_List[i].String_Value != NULL)
            {

			  payload = (char*)Parameter_List[i].String_Value ;
               
            }
            else
            {
              COAP_PRINTF("Invalid payload string value");
			   result = QCLI_STATUS_USAGE_E;
			   goto end;
            }
            
        }
        else if(strcmp((char*)Parameter_List[i].String_Value, "-L") == 0)
        {
             ++i;
            if(Parameter_List[i].Integer_Is_Valid)
            {

              payload_len = Parameter_List[i].Integer_Value;
			  COAP_PRINTF("payload length value payload_length %d ", payload_len);
			
            }
			else
			{
			   COAP_PRINTF("Invalid payload length value");
			   result = QCLI_STATUS_USAGE_E;
			   goto end;
			}
            
        	}   
   
    }


if(payload)
{
    if(block1_size[app_id] > 0 &&  block1_size[app_id] % 16 == 0)
    {
      strlcpy(send_block1_buffer[app_id] , payload , payload_len);
	  COAP_PRINTF("send_message : total send_block1_buffer[app_id] = %s , payload_len %d\n" ,send_block1_buffer[app_id],payload_len);
	  payload = (char *)coap_malloc(((block1_size[app_id])+1)*sizeof(char));
	  if(payload != NULL)
	  {
       MEMSET(payload , 0 , (block1_size[app_id])+1);
	   strlcpy(payload , send_block1_buffer[app_id] , block1_size[app_id]+1);
	   is_payload_malloced = 1;
	   block1_buffer_total_len[app_id] = payload_len;
	   payload_len = block1_size[app_id];
	   COAP_PRINTF("send_message :total_buffer_len_to_put = %d , new payload_len %d\n" , block1_buffer_total_len[app_id] ,payload_len);
	   COAP_PRINTF("send_message :  payload block1 or packet 1 = %s\n" , payload);
	  }
	  else
	  {
       result = QCLI_STATUS_ERROR_E;
	   COAP_PRINTF("send_message :  payload malloc allocation failure\n");
	   goto end;
	  }

    }
 
	if(qapi_Coap_Set_Payload(app_coapcli_hndle[app_id],messageP[app_id], payload, payload_len) != QAPI_OK)
	{
       result = QCLI_STATUS_ERROR_E;
	   COAP_PRINTF("qapi_Coap_Set_Payload returned error");
	   goto end;
	}
}

if(qapi_Coap_Send_Message(app_coapcli_hndle[app_id] ,messageP[app_id], &message_param[app_id]) != QAPI_OK)
{
	result = QCLI_STATUS_ERROR_E;
	COAP_PRINTF("qapi_Coap_Send_Message returned error");
	goto end;

}
else 
{
    if(block1_size[app_id] > 0 &&  block1_size[app_id] % 16 == 0)
    send_block1_buffer_len[app_id] = block1_size[app_id];
	
	COAP_PRINTF("qapi_Coap_Send_Message returned success");
   	if(is_payload_malloced == 1)
      coap_free(payload);
	else
		payload = NULL;
}

end:

  if(messageP[app_id] != NULL && messageP[app_id]->type == QAPI_COAP_TYPE_NON)
  {

        qapi_Coap_Free_Message(app_coapcli_hndle[app_id],messageP[app_id]);
        messageP[app_id] = NULL;
  }

  if (Parameter_List != NULL)
  {
    for (p = 0; p<Parameter_Count; p++)
    {
      if (Parameter_List[p].String_Value != NULL)
      {
        tx_byte_release(Parameter_List[p].String_Value);
        Parameter_List[p].String_Value = NULL;
      }
    }
  }
  return result;

}

QCLI_Command_Status_t disconnect(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{

    int p;
	QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;


    if(app_coapcli_hndle[app_id] != NULL && connection_status[app_id] == TRUE)
    {
       if(qapi_Coap_Close_Connection(app_coapcli_hndle[app_id])!=QAPI_OK)
       {
          
		  COAP_PRINTF("coap_disconnect :qapi_Coap_Close_Connection returned error !!");
		  result=QCLI_STATUS_ERROR_E;
		  goto end;
	   }
	   else
	   {
           COAP_PRINTF("qapi_Coap_Close_Connection returned success !!\n");
		   connection_status[app_id] = FALSE;
		   result = QCLI_STATUS_SUCCESS_E;
	   }
    }
	else
	{
       COAP_PRINTF("coap_disconnect : either coap handle on app_id %d doens'nt exists or connection isn't created !!\n" ,app_id);
	   result=QCLI_STATUS_ERROR_E;
	   goto end;
	  
	}

	init_global_variable(app_id);

end:
		  if (Parameter_List != NULL)
		  {
			for (p = 0; p<Parameter_Count; p++)
			{
			  if (Parameter_List[p].String_Value != NULL)
			  {
				tx_byte_release(Parameter_List[p].String_Value);
				Parameter_List[p].String_Value = NULL;
			  }
			}
		  }

	return result;
	
}

QCLI_Command_Status_t delete_session(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  
    int p;
	QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
 

     if(app_coapcli_hndle[app_id] != NULL)
     {    
        if(connection_status[app_id] == TRUE)
        {

			 COAP_PRINTF("closing connection before deleting session .....\n");
			 if(qapi_Coap_Close_Connection(app_coapcli_hndle[app_id])!=QAPI_OK)
			 {
				result=QCLI_STATUS_ERROR_E; 
				goto end;
			 }
			 else
			 {
				 COAP_PRINTF("qapi_Coap_Close_Connection returned success !!\n");
				 connection_status[app_id] = FALSE;
				 result = QCLI_STATUS_SUCCESS_E;
			 }

			 init_global_variable(app_id);

		}

		if(qapi_Coap_Destroy_Session(app_coapcli_hndle[app_id])!=QAPI_OK)
	      {
             result=QCLI_STATUS_ERROR_E;
			 COAP_PRINTF("qapi_Coap_Destroy_Session returned error !!\n");
		     goto end;
	      }	
	    else
	    {
          app_coapcli_hndle[app_id] = NULL;
	    }
     }
	 else
	 {
        result=QCLI_STATUS_ERROR_E;
		COAP_PRINTF("session handle on app id %d doesnot exists\n" , app_id);		
	 }
	 
 end:
	   if (Parameter_List != NULL)
	   {
		 for (p = 0; p<Parameter_Count; p++)
		 {
		   if (Parameter_List[p].String_Value != NULL)
		   {
			 tx_byte_release(Parameter_List[p].String_Value);
			 Parameter_List[p].String_Value = NULL;
		   }
		 }
	   }
	
	 return result;
	 
}

QCLI_Command_Status_t get_last_error_coap(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
	int p;
  	int ret = QAPI_ERROR;
  	QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
	qapi_Net_Coap_Get_Extended_Config_Options_t ext_config = get_coap_ext_conf[app_id];
	qapi_Net_Module_Last_Error_t *error_data;

	if ((Parameter_Count > 2) || (Parameter_List == NULL)) {
        COAP_PRINTF("get_last_error_coap: Coap_last_error : Invalid Params or More than necessary Params passed\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }
	
  	if(app_coapcli_hndle[app_id]!=NULL) {
   		tx_byte_allocate(byte_pool_qcli, (VOID **) &error_data, sizeof(qapi_Net_Module_Last_Error_t), TX_NO_WAIT);

		if (error_data != NULL)
    	{
    		memset(error_data, 0, sizeof(qapi_Net_Module_Last_Error_t));
  			ret = qapi_Net_COAP_Get_Extended_Config_Option(app_coapcli_hndle[app_id],ext_config,error_data, sizeof(qapi_Net_Module_Last_Error_t));
  		}
		else
		{
			COAP_PRINTF("get_last_error_coap : Memory allocation for Lasterror failed\n" ,app_id);
	        result = QCLI_STATUS_ERROR_E; 
			goto end;
		}
    
    	if(ret != QAPI_OK)
    	{
      		COAP_PRINTF("get_last_error_coap: MQTT Get Last error Code failed Failed\n");
    	}
		else
    	{
      		COAP_PRINTF("get_last_error_coap: The Last error category is %d Last error code is %d", error_data->err_cat, error_data->err_code);
    	}
	
		if(error_data != NULL)
  			tx_byte_release(error_data);
    	
  	}
	else
	{
       COAP_PRINTF("get_last_error_coap : either coap handle on app_id %d\n" ,app_id);
	   result = QCLI_STATUS_ERROR_E; 
	   goto end;
	}
  
    

	end:
  	if (Parameter_List != NULL)
  	{
    	for (p = 0; p<Parameter_Count; p++)
    	{
      		if (Parameter_List[p].String_Value != NULL)
      		{
        		tx_byte_release(Parameter_List[p].String_Value);
        		Parameter_List[p].String_Value = NULL;
      		}
    	}
  	}
  	return result;
}

QCLI_Command_Status_t coap_command_handler(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_net_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = COAP_COMMAND_HANDLER_EVT;
  cmd_parser_temp->Command = (QCLI_Command_t *)&net_cmd_list[0];
  cmd_parser_temp->Command_Index = 25;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}

QCLI_Command_Status_t 
proc_coap_command_handler(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
    uint32_t p; 
    QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
    char *command = NULL;

    if(Parameter_Count < 1)
    {
    
    COAP_PRINTF("[-app <app_id>] create_session [-a <ack_random_factor>] [-m <max_retransmit>] [-M <max_age>][-le <enable last error>]\\n");
	COAP_PRINTF("[-ml <max_latency>] [-t <transaction_timeout>]\n");
	COAP_PRINTF("\n");
	COAP_PRINTF("[-app <app_id>] create_connection  [-m <sec mode> 0 MODE_PSK|1 MODE_CERTIFICATE|2 MODE_NON SEC] [-p <protocol> |0 UDP|1 NIDD]\n");
	COAP_PRINTF("[-i <handle_index for non ip >] [-s <source ip>] [-sp <source port>] [-d <server ip>] [-dp <server port>]\n");
	COAP_PRINTF("[-r <resumption enable> 0|1] [-t <resumption timout>]\n"); 
	COAP_PRINTF("\n");
    COAP_PRINTF("[-app <app_id>] init_message [-t msg type | 0 CON | 1 NON CON | 2 ACK | 3 RST ]\n");
	COAP_PRINTF("[-m coap_method_type | 1 GET |2 POST | 3 PUT |4 DELETE]\n");
	COAP_PRINTF("\n");
    COAP_PRINTF("[-app <app_id>] set_header  [-t <header type >] [-v <value>] [-l <length>]\n");
	COAP_PRINTF("\n");
	COAP_PRINTF("[-app <app_id>] get_header  [-t <header type >]\n");
	COAP_PRINTF("\n");
    COAP_PRINTF("[-app <app_id>] send_message [-m <mid> max 16 bit unsigned int] [-t <token id>]\n");
    COAP_PRINTF("[-app <app_id>] send_message_v2 [-m <mid> max 16 bit unsigned int] [-t <token id>] [-l <token length>] [-btype <block type>]\n");
    COAP_PRINTF("[-bsize <block size>] [-p <payload>] [-L <payload length>]\n");
    COAP_PRINTF("[-app <app_id>] get_block_info\n");
	COAP_PRINTF("[-l <token length>] [-p <payload>] [-L <payload length>]\n");
	COAP_PRINTF("\n");
    COAP_PRINTF("[-app <app_id>] disconnect \n");
	COAP_PRINTF("\n");
    COAP_PRINTF("[-app <app_id>] delete_session \n");	
	COAP_PRINTF("[-app <app_id>] lasterror \n");
    result = QCLI_STATUS_USAGE_E;
    goto end;
    }

	command = (char*)Parameter_List[0].String_Value;

    if (0 == strcmp(command, "create_session")) {
        result = create_session(Parameter_Count - 1, &Parameter_List[1], app_id);
    }
    else if (0 == strcmp(command, "create_connection")) {
        result = create_connection(Parameter_Count - 1, &Parameter_List[1], app_id);
    }
	else if (0 == strcmp(command, "init_message")) {
        result = init_message(Parameter_Count - 1, &Parameter_List[1], app_id);
    }
    else if (0 == strcmp(command, "set_header")) {
        result = set_header(Parameter_Count - 1, &Parameter_List[1], app_id);
    }
	else if (0 == strcmp(command, "get_header")) {
        result = get_header(Parameter_Count - 1, &Parameter_List[1], app_id);
	}
    else if (0 == strcmp(command, "send_message")) {
        result = send_message(Parameter_Count-1, &Parameter_List[1], app_id);
    }
	 else if (0 == strcmp(command, "send_message_v2")) {
        result = send_message_v2(Parameter_Count-1, &Parameter_List[1], app_id);
    }
    else if (0 == strcmp(command, "get_block_info")) {
        return get_block_info(Parameter_Count - 1, &Parameter_List[1], app_id);
	}
    else if (0 == strcmp(command, "disconnect")) {
        result = disconnect(Parameter_Count - 1, &Parameter_List[1], app_id);
    }
    else if (0 == strcmp(command, "delete_session")) {
        result =  delete_session(Parameter_Count - 1, &Parameter_List[1], app_id);
    }
	else if (0 == strcmp((char*)Parameter_List[0].String_Value, "lasterror")) {
        return  get_last_error_coap(Parameter_Count - 1, &Parameter_List[1], app_id);
    }
    else
    {
      result = QCLI_STATUS_USAGE_E;
      goto end;
    }
    

end:
  if (Parameter_List != NULL && result == QCLI_STATUS_USAGE_E)
  {
    for (p = 0; p<Parameter_Count; p++)
    {
      if (Parameter_List[p].String_Value != NULL)
      {
        tx_byte_release(Parameter_List[p].String_Value);
        Parameter_List[p].String_Value = NULL;
      }
    }
  }

  if(command)
  {
    command = NULL;
  }
  
  return result;
}























