/*!
  @file
  httpc_demo.c

  @brief
  HTTP client demo application to test HTTP client services. 
*/

/*===========================================================================

  Copyright (c) 2017-2023 Qualcomm Technologies, Inc. All Rights Reserved

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

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
25Jan2017 vpulimi  Created the module. First version of the file.
===========================================================================*/
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "qapi_status.h"
#include "bench.h"
#include "qapi_netservices.h"
#include "qapi_driver_access.h"
#include "qapi_socket.h"
#include "qapi_fs.h"
#include "qapi_version.h"

#define HTTP_APP_PERSISTENCE_PATH "/datatx/http_app_persist.html"

extern TX_BYTE_POOL *byte_pool_qcli;
extern QCLI_Group_Handle_t qcli_net_handle; /* Handle for Net Command Group. */

extern QCLI_Command_t net_cmd_list[];

#define HTTPC_PRINTF(...) QCLI_Printf(qcli_net_handle, __VA_ARGS__)
#define HTTPC_DEMO_MAX_NUM (4)
#define HTTPC_DEMO_DEFAULT_BODY_LEN 120
#define HTTPC_DEMO_DEFAULT_MAX_BODY_LEN 2048
#define HTTPC_DEMO_MAX_BODY_LEN 10240
#define HTTPC_DEMO_DEFAULT_MAX_HEADER_LEN   350

/* support DELETE method from dataservice version 1.6.0.
   DON'T change the value of below 2 Macros!!!
 */
#define DATASERVICE_VERSION_SUPPORT_DELETE_MAJOR_NUM 1
#define DATASERVICE_VERSION_SUPPORT_DELETE_MINOR_NUM 6

uint16_t httpc_demo_max_body_len[NO_OF_APPS] = {0};
uint16_t httpc_demo_max_header_len[NO_OF_APPS] = {0};

extern bool dam_thrd_running[NO_OF_APPS];

typedef struct http_client_demo_s
{
  qapi_Net_HTTPc_handle_t client;
  uint32_t num;
  uint32_t total_len;
  qapi_Net_SSL_Obj_Hdl_t sslCtx;
  qapi_Net_SSL_Config_t *sslCfg;
  qapi_Net_HTTPc_Config_t *httpc_cfg;
  uint8_t connect; /* Stored for proxy cases */
  uint8_t server_port; /* Stored for proxy cases */
  uint8_t server_url[256]; /* Stored for proxy cases */
} http_client_demo_t;

http_client_demo_t http_client_demo[NO_OF_APPS][HTTPC_DEMO_MAX_NUM];

static void* httpc_malloc(uint32_t size)
{
  void *ptr = NULL; 
  uint32_t status = 0; 

  status = tx_byte_allocate(byte_pool_qcli, &ptr, size, TX_NO_WAIT);
  if (status != TX_SUCCESS)
  {
    HTTPC_PRINTF("Failed to allocate memory with %d", status); 
    return NULL;
  }
  
  return ptr;   
}  

static void httpc_free(void *ptr)
{
  if (ptr != NULL)
  {
    tx_byte_release(ptr);
    ptr = NULL; 
  }
}  

void http_client_cb_demo(void *arg, int32_t state, void *http_resp)
{
  qapi_Net_HTTPc_Response_t *temp = (qapi_Net_HTTPc_Response_t *)http_resp;
  struct http_client_demo_s *hc = (struct http_client_demo_s *)arg;
  uint32_t *ptotal_len = NULL;

  if (!arg || !http_resp)
  {
    HTTPC_PRINTF("HTTP response callback invalid params...\n");
    return;
  }

  ptotal_len = &hc->total_len;  
  if (state >= 0)
  {
    int32_t resp_code = temp->resp_Code;
    
    /* HTTP response header */ 
    if (temp->rsp_hdr_len &&  temp->rsp_hdr)
    {
      int32_t fd = 0x00;
      int32_t status = 0x00;   
      uint8_t *data = NULL;
      uint32_t bytes_stored = 0x00;
    
#ifdef SKIP_QCLI_PRINT
      data = httpc_malloc(temp->rsp_hdr_len + 1);
      if (NULL == data)
      {
        HTTPC_PRINTF("HTTP Client Demo malloc error %d\n", state);
        goto HTTPc_CB_ERROR;
      }
         
      memcpy(data, temp->rsp_hdr, temp->rsp_hdr_len);
      data[temp->rsp_hdr_len] = '\0';      

      HTTPC_PRINTF("%s", data);  
#endif 
      
      /* Open the application persistence file */ 
      status = qapi_FS_Open(HTTP_APP_PERSISTENCE_PATH, QAPI_FS_O_CREAT_E | QAPI_FS_O_WRONLY_E | QAPI_FS_O_TRUNC_E, &fd);
      if (-1 == status)
      {
        HTTPC_PRINTF("Failed to open application HTTP persistence file [%s]\n", HTTP_APP_PERSISTENCE_PATH);
        
        if (data)
          httpc_free(data);
        
        return;
      }

      /* Persist the application information */
      qapi_FS_Write(fd, temp->rsp_hdr, temp->rsp_hdr_len, &bytes_stored); 
      if (temp->rsp_hdr_len != bytes_stored)
      {
        HTTPC_PRINTF("Failed to persist the application information");
        qapi_FS_Close(fd);

        if (data)
          httpc_free(data);
          
        return;
      }
          
      qapi_FS_Close(fd);

      HTTPC_PRINTF("HTTP response header length : %d\n", temp->rsp_hdr_len);
      HTTPC_PRINTF("##########################################################################\n");
      HTTPC_PRINTF("HTTP response header is : %s\n", temp->rsp_hdr);
      HTTPC_PRINTF("##########################################################################\n");

      if (data)
        httpc_free(data);

    }    

    /* HTTP response data */  
    if (temp->length && temp->data)
    {
      uint8_t *data = NULL;
      int32_t fd = 0x00;  
      uint32_t bytes_stored = 0x00;
      int32_t status = 0x00;
 
#ifdef SKIP_QCLI_PRINT
      data = httpc_malloc(temp->length + 1);
      if (NULL == data)
      {
        HTTPC_PRINTF("HTTP Client Demo malloc error %d\n", state);
        goto HTTPc_CB_ERROR;
      }

      memcpy(data, temp->data, temp->length);
      data[temp->length] = '\0'; 

      HTTPC_PRINTF("%s", data); 
#endif    
      
      /* Open the application persistence file */ 
      status = qapi_FS_Open(HTTP_APP_PERSISTENCE_PATH, QAPI_FS_O_CREAT_E | QAPI_FS_O_WRONLY_E | QAPI_FS_O_APPEND_E, &fd);
      if (-1 == status)
      {
        HTTPC_PRINTF("Failed to open application HTTP persistence file [%s]\n", HTTP_APP_PERSISTENCE_PATH);

        if (data)
          httpc_free(data);

        return;
      }

      /* Persist the application information */
      qapi_FS_Write(fd, temp->data, temp->length, &bytes_stored); 
      if (temp->length != bytes_stored)
      {
        HTTPC_PRINTF("Failed to persist the application information %d:%d", temp->length, bytes_stored);
        qapi_FS_Close(fd);

        if (data)
          httpc_free(data);
          
        return;
      }
          
      qapi_FS_Close(fd);

      if (data)
        httpc_free(data);

     *ptotal_len += temp->length;
    }

    /* HTTP CONNECT response */
    if ((state == 0) && (hc->connect))
    {
      qapi_Status_t error = QAPI_ERROR; 
      
      if (200 == resp_code)
      {
        uint8_t server_offset = 0; 

        if (strncmp((const char *)hc->server_url, "https://", 8) == 0)
          server_offset = 8;
        else if (strncmp((const char *)hc->server_url, "http://", 7) == 0)
          server_offset = 7;

        error = qapi_Net_HTTPc_Proxy_Connect(hc->client, (const char *)(hc->server_url + server_offset), hc->server_port, true); 

        hc->connect = FALSE;
        hc->server_port = 0x00; 
        memset((void *)hc->server_url, 0x00, sizeof(hc->server_url));
      }

      if (error)
      {
        qapi_Net_HTTPc_Free_sess(hc->client);
  
        if (hc->sslCtx)
          qapi_Net_SSL_Obj_Free(hc->sslCtx);
  
        if (hc->sslCfg)
          httpc_free(hc->sslCfg); 

        memset(hc, 0, sizeof(struct http_client_demo_s)); 
        HTTPC_PRINTF("HTTP client connected to server successfully via the PROXY with error[%d]\n", error); 

        goto HTTPc_CB_ERROR; 
      }
  
      HTTPC_PRINTF("HTTP client connected to server successfully via the PROXY\n");      
    }

    /* HTTP HEAD response */    
    if ((state == 0) && (temp->length) && (NULL == temp->data))
    {
      HTTPC_PRINTF("=========> http client HEAD response: total size %d, response code %d\n", temp->length, resp_code);
    }

    /* HTTP response data received completely */
    if (state == 0 && *ptotal_len)
    {
      HTTPC_PRINTF("=========> http client received response: total size %d, response code %d\n", *ptotal_len, resp_code);
     *ptotal_len = 0; 
    }    
  }
  else
  {
    if (QAPI_NET_HTTPC_RX_ERROR_SERVER_CLOSED == state)
      HTTPC_PRINTF("HTTP Client server closed on client[%d].\n", hc->num);
    else
      HTTPC_PRINTF("HTTP Client Receive error: %d\nPlease input 'httpc disconnect %d'\n", state, hc->num);

   *ptotal_len = 0;
  }

HTTPc_CB_ERROR:
  return;
}

char* httpc_malloc_body_demo(uint32_t len)
{
  char *body = NULL;
  int32_t i;

  body = httpc_malloc(len + 1);
  if (NULL != body) 
  {
    for (i=0; i < len; i++) 
    {
      *(body + i) = 'A' + i % 26;
    }

    *(body + len) = '\0';
  } 
  else 
  {
    HTTPC_PRINTF("malloc failed\n");
  }
 
  return body;
}

QCLI_Command_Status_t httpc_command_handler(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_net_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = HTTPC_COMMAND_HANDLER_EVT;
  cmd_parser_temp->Command = (QCLI_Command_t *)&net_cmd_list[0];
  cmd_parser_temp->Command_Index = 15;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}


QCLI_Command_Status_t get_last_error_httpc(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, qapi_Net_HTTPc_handle_t handle)
{
  	int ret = QAPI_ERROR;
  	QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
	qapi_Net_HTTPc_Get_Extended_Config_Options_t ext_config = QAPI_NET_HTTP_GET_EXTENDED_CONFIG_LAST_ERR_CODE;
	qapi_Net_Module_Last_Error_t error_data;

	
  	if(NULL == handle) {
   		HTTPC_PRINTF("No available http client session\r\n");
    	result = QCLI_STATUS_ERROR_E;
    	goto end;
  	}

	if ((Parameter_Count >= 2) || (Parameter_List == NULL)) {
        HTTPC_PRINTF("Httpc_last_error : Invalid Params or More than necessary Params passed\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }
  
    memset(&error_data, 0, sizeof(qapi_Net_Module_Last_Error_t));
    ret = qapi_Net_HTTPc_Get_Extended_Config_Options(handle, ext_config, (void *)&error_data, sizeof(qapi_Net_Module_Last_Error_t));
    
    if(ret != QAPI_OK)
      HTTPC_PRINTF("HTTPC Get Last error Code Failed ret = %d\n", ret);
    else
    {
      HTTPC_PRINTF("The Last error category is %d, Last error code is %d\n", error_data.err_cat, error_data.err_code);
    }
	
end:
  	return result;
}

QCLI_Command_Status_t 
proc_httpc_command_handler(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  int error = QAPI_OK;
  struct http_client_demo_s *arg = NULL;
  char *command = NULL;
  uint32_t num = 0;
  static uint32_t init = false;
  qapi_Net_HTTPc_Method_e req_cmd = (qapi_Net_HTTPc_Method_e)0;
  int i;
  QCLI_Command_Status_t status = QCLI_STATUS_SUCCESS_E;

  if (Parameter_Count < 1)
  {
usage:
    HTTPC_PRINTF("\nhttpc start\n");
    HTTPC_PRINTF("httpc stop\n");
    HTTPC_PRINTF("httpc connect <server> <port> <timeout> [-dns_if <dns_interface>] [-bind_addr <bind_address>]\n");
    HTTPC_PRINTF("httpc proxy_connect <server> <port> <proxy_server> <proxy_port> <timeout> [-dns_if <dns_interface>] [-bind_addr <bind_address>]\n");
    HTTPC_PRINTF("httpc disconnect <client_num>\n");
    HTTPC_PRINTF("httpc get <client_num> <url>\n");
    HTTPC_PRINTF("httpc put <client_num> <url>\n");
    HTTPC_PRINTF("httpc post <client_num> <url>\n");
    HTTPC_PRINTF("httpc patch <client_num> <url>\n");
    HTTPC_PRINTF("httpc head <client_num> <url>\n");
    HTTPC_PRINTF("httpc delete <client_num> <url>\n");
    HTTPC_PRINTF("httpc setbody <client_num> <len>\n");
    HTTPC_PRINTF("httpc addheaderfield <client_num> <hdr_name> <hdr_value>\n");
    HTTPC_PRINTF("httpc clearheader <client_num>\n");
    HTTPC_PRINTF("httpc setparam <client_num> <key> <value>\n");
    HTTPC_PRINTF("httpc config <client_num> <addr_type> <socket option> <socket option value> <send_chunk> <chunk_delay> <chunk_retries> <conn_cnt> <conn_interval>\n");
    HTTPC_PRINTF("httpc lasterror <client_num>\n");
#ifdef INCLUDE_SSLDEMO
   sslconfig_help("httpc sslconfig <client_num>\n");
#endif
    status = QCLI_STATUS_USAGE_E;
    goto end;
  }

  if (!init)
  {
    init = true;
    memset(http_client_demo, 0x00, sizeof(http_client_demo));  
  }

  command = (char *)Parameter_List[0].String_Value;

  if (strcmp(command, "start") == 0)
  {
    error = qapi_Net_HTTPc_Start();
    if (error)
    {
      HTTPC_PRINTF("HTTP Client start failed %d\r\n",error);
      status = QCLI_STATUS_ERROR_E;
      goto end;
    }

    status = QCLI_STATUS_SUCCESS_E;
    goto end;
  }
    
  if (strcmp(command, "stop") == 0)
  {
    uint32_t i;
    
    error = qapi_Net_HTTPc_Stop();
    for (i = 0; i < HTTPC_DEMO_MAX_NUM; i++)
    {
      if (http_client_demo[app_id][i].client)
      {
        arg = &http_client_demo[app_id][i];
        arg->num = 0; 
        arg->total_len = 0;
        arg->client = NULL;
        arg->sslCtx = 0;
        break;
      }
    }
        
    if (error)
    {
      HTTPC_PRINTF("HTTP Client stop failed %d\r\n",error);
      status = QCLI_STATUS_ERROR_E;
      goto end;
    }

    status = QCLI_STATUS_SUCCESS_E;
    goto end;
  }

#ifdef HTTPc_DEBUG
  if (strcmp(command, "config") == 0)
  {
    if (Parameter_Count < 3)
      goto usage;
        
    if (Parameter_List[1].Integer_Value % 4 != 0)
      goto usage;

    httpc_demo_max_body_len[app_id] = (Parameter_List[1].Integer_Value > HTTPC_DEMO_MAX_BODY_LEN) ? HTTPC_DEMO_MAX_BODY_LEN : Parameter_List[1].Integer_Value;
    httpc_demo_max_header_len[app_id] = (Parameter_List[2].Integer_Value > HTTPC_DEMO_DEFAULT_MAX_HEADER_LEN) ? HTTPC_DEMO_DEFAULT_MAX_HEADER_LEN : Parameter_List[2].Integer_Value;
    HTTPC_PRINTF("Max body len:%d Max header len:%d\r\n", httpc_demo_max_body_len[app_id], httpc_demo_max_header_len[app_id]);

    status = QCLI_STATUS_SUCCESS_E;
    goto end;
  }
#endif

  if (strcmp(command, "connect") == 0)
  {
    uint32_t port = 0;
    uint32_t server_offset = 0;
    uint32_t timeout = 0;
    uint32_t i;
    uint16_t httpc_max_body_length = 0;
    uint16_t httpc_max_header_length = 0;
 
    if (Parameter_Count < 4)
      goto usage;

    if (strlen((char*)Parameter_List[1].String_Value) >= 64)
    {
      HTTPC_PRINTF("Maximum 64 bytes supported as Connect URL\n");
      status = QCLI_STATUS_ERROR_E;
      goto end;
    }

    for (i = 0; i < HTTPC_DEMO_MAX_NUM; i++)
    {
      if (http_client_demo[app_id][i].client == NULL)
      {
        arg = &http_client_demo[app_id][i];
        arg->num = i + 1; 
        arg->total_len = 0;
        break;
      }
    }

    if (!arg)
    {
      HTTPC_PRINTF("No More avalible HTTP CLIENT\n");
      status = QCLI_STATUS_ERROR_E;
      goto end;
    }  

    port = Parameter_List[2].Integer_Value;

    if (port == 0)
      port = 80;
    
    timeout = Parameter_List[3].Integer_Value;

    if (strncmp((char *)Parameter_List[1].String_Value, "https://", 8) == 0)
    {
      server_offset = 8;
      arg->sslCtx = qapi_Net_SSL_Obj_New(QAPI_NET_SSL_CLIENT_E);
      if (arg->sslCtx == QAPI_NET_SSL_INVALID_HANDLE)
      {
        if (arg->sslCfg)
          httpc_free(arg->sslCfg);

        memset(arg, 0, sizeof(struct http_client_demo_s));
        HTTPC_PRINTF("ERROR: Unable to create SSL context\n");
        status = QCLI_STATUS_ERROR_E;
        goto end;
      }

      if (qapi_Net_SSL_Cert_Load(arg->sslCtx, QAPI_NET_SSL_CERTIFICATE_E, "ca.crt.bin") < 0)
      {
        if (arg->sslCtx)
          qapi_Net_SSL_Obj_Free(arg->sslCtx);

        if (arg->sslCfg)
          httpc_free(arg->sslCfg); 

        memset(arg, 0, sizeof(struct http_client_demo_s));
        HTTPC_PRINTF("ERROR: Unable to load the SSL certificate\n");
        status = QCLI_STATUS_ERROR_E;
        goto end;
      }

      HTTPC_PRINTF("%s loaded from FLASH\r\n", "ca_crt.bin");

      if (qapi_Net_SSL_Cert_Load(arg->sslCtx, QAPI_NET_SSL_CA_LIST_E, "ca_list.bin") < 0)
      {
        if (arg->sslCtx)
          qapi_Net_SSL_Obj_Free(arg->sslCtx);

        if (arg->sslCfg)
          httpc_free(arg->sslCfg); 

        memset(arg, 0, sizeof(struct http_client_demo_s));
        HTTPC_PRINTF("ERROR: Unable to load the SSL CA list\n");
        status = QCLI_STATUS_ERROR_E;
        goto end;
      }

      HTTPC_PRINTF("%s loaded from FLASH\r\n", "ca_list.bin");
    }
    else if (strncmp((char *)Parameter_List[1].String_Value, "http://", 7) == 0)
    {
      server_offset = 7;
    }
      
    httpc_max_body_length = (httpc_demo_max_body_len[app_id]) ? httpc_demo_max_body_len[app_id] : HTTPC_DEMO_DEFAULT_MAX_BODY_LEN;
    httpc_max_header_length = (httpc_demo_max_header_len[app_id]) ? httpc_demo_max_header_len[app_id] : HTTPC_DEMO_DEFAULT_MAX_HEADER_LEN;

    arg->client = qapi_Net_HTTPc_New_sess(timeout, arg->sslCtx, http_client_cb_demo, (void *)arg, httpc_max_body_length, httpc_max_header_length);
    if (arg->client == NULL)
    {
      if (arg->sslCtx)
        qapi_Net_SSL_Obj_Free(arg->sslCtx);

      if (arg->sslCfg)
        httpc_free(arg->sslCfg); 

      memset(arg, 0, sizeof(struct http_client_demo_s));  
      HTTPC_PRINTF("No available http client session\r\n");
      status = QCLI_STATUS_ERROR_E;
      goto end;
    } 

    if (qapi_Net_HTTPc_Pass_Pool_Ptr(arg->client, byte_pool_qcli)!=QAPI_OK)
    {
      HTTPC_PRINTF("Error is setting User space pointer\n");
      status = QCLI_STATUS_ERROR_E;
      goto end;
    }

    for(i = 4; i < Parameter_Count; ++i)
    {
      if(strncmp((char *)Parameter_List[i].String_Value, "-dns_if", 7) == 0)
      {
        ++i;
        if((char *)Parameter_List[i].String_Value != NULL)
        {
          error = qapi_Net_HTTPc_Extended_Config_Options(arg->client,
                                                          QAPI_NET_HTTPC_EXTENDED_CONFIG_DNS_RESOLVE_INTERFACE,
                                                          (char *)Parameter_List[i].String_Value,
                                                          strlen((char *)Parameter_List[i].String_Value));
          if(error != QAPI_OK)
            HTTPC_PRINTF("fail to set dns interface: %s\n", (char *)Parameter_List[i].String_Value);
          else
            HTTPC_PRINTF("succeed to set dns interface: %s\n", (char *)Parameter_List[i].String_Value);
        }
        else
          goto usage;
      }
      else if(strncmp((char *)Parameter_List[i].String_Value, "-bind_addr", 10) == 0)
      {
        ++i;
        if((char *)Parameter_List[i].String_Value != NULL)
        {
          error = qapi_Net_HTTPc_Extended_Config_Options(arg->client,
                                                          QAPI_NET_HTTPC_EXTENDED_CONFIG_BIND_ADDRESS,
                                                          (char *)Parameter_List[i].String_Value,
                                                          strlen((char *)Parameter_List[i].String_Value));
          if(error != QAPI_OK)
            HTTPC_PRINTF("fail to set bind address: %s\n", (char *)Parameter_List[i].String_Value);
          else
            HTTPC_PRINTF("succeed to set bind address: %s\n", (char *)Parameter_List[i].String_Value);
        }
        else
          goto usage;
      }
    }

    if (arg->httpc_cfg)
    {
      error = qapi_Net_HTTPc_Configure(arg->client, arg->httpc_cfg);
      if (error)
      {
        if (arg->sslCtx)
          qapi_Net_SSL_Obj_Free(arg->sslCtx);
    
        if (arg->sslCfg)
          httpc_free(arg->sslCfg); 
    
        if (arg->httpc_cfg->sock_options)
          httpc_free(arg->httpc_cfg->sock_options);
    
        httpc_free(arg->httpc_cfg);
      
        memset(arg, 0, sizeof(struct http_client_demo_s));
        HTTPC_PRINTF("http client configuration failed on error: %d\n", error);
        status = QCLI_STATUS_ERROR_E;
        goto end;
      }    

      HTTPC_PRINTF("http client configuration successful\n");
    }

    /* SSL object is created and paramters are parsed */
    error = QAPI_OK; 
    if ((arg->sslCtx != 0 && arg->sslCtx != QAPI_NET_SSL_INVALID_HANDLE && arg->sslCfg != NULL)) 
      error = qapi_Net_HTTPc_Configure_SSL(arg->client, arg->sslCfg);
   
    if (QAPI_OK == error)
      error = qapi_Net_HTTPc_Connect(arg->client, (const char *)((char *)Parameter_List[1].String_Value + server_offset), port);

    if (error) 
    {

      int ret = 0;
      qapi_Net_Module_Last_Error_t  error_info;
      error_info.err_cat = QAPI_NET_NO_MODULE_CAT;
      error_info.err_code = 0;

      ret = qapi_Net_HTTPc_Get_Extended_Config_Options(arg->client , QAPI_NET_HTTP_GET_EXTENDED_CONFIG_LAST_ERR_CODE ,  &error_info , sizeof(qapi_Net_Module_Last_Error_t));
      
      if(ret == QAPI_OK)
      {
        HTTPC_PRINTF("TX_log : get extended config success last_category = %d , last_error = %d\n" , error_info.err_cat , error_info.err_code);
      }
      else
      {
        HTTPC_PRINTF("TX_log : get extended config fail ret = %d" , ret);
      }

      qapi_Net_HTTPc_Free_sess(arg->client);

      if (arg->sslCtx)
        qapi_Net_SSL_Obj_Free(arg->sslCtx);

      if (arg->sslCfg)
        httpc_free(arg->sslCfg); 

      memset(arg, 0, sizeof(struct http_client_demo_s)); 
      HTTPC_PRINTF("http client connect failed %d\n",error);      
      status = QCLI_STATUS_ERROR_E;
      goto end;
    }
    
    HTTPC_PRINTF("http client connect success <client num> = %d\n", arg->num);
    status = QCLI_STATUS_SUCCESS_E;
    goto end;
  }

  if (strcmp(command, "proxy_connect") == 0)
  {
    uint32_t server_port = 0;
    uint32_t proxy_port = 0;
    uint32_t server_offset = 0;
    uint32_t proxy_offset = 0;
    uint32_t timeout = 0;
    uint32_t i;
    uint16_t httpc_max_body_length = 0;
    uint16_t httpc_max_header_length = 0;
 
    if (Parameter_Count < 6)
      goto usage;

    if ((strlen((char*)Parameter_List[1].String_Value) >= 64) ||
        (strlen((char*)Parameter_List[3].String_Value) >= 64))
    {
      HTTPC_PRINTF("Maximum 64 bytes supported as Connect URL\n");
      status = QCLI_STATUS_ERROR_E;
      goto end;
    }

    for (i = 0; i < HTTPC_DEMO_MAX_NUM; i++)
    {
      if (http_client_demo[app_id][i].client == NULL)
      {
        arg = &http_client_demo[app_id][i];
        arg->num = i + 1; 
        arg->total_len = 0;
        break;
      }
    }

    if (!arg)
    {
      HTTPC_PRINTF("No More avalible HTTP CLIENT\n");
      status = QCLI_STATUS_ERROR_E;
      goto end;
    }  

    server_port = Parameter_List[2].Integer_Value;
    if (server_port == 0)
      server_port = 80;

    proxy_port = Parameter_List[4].Integer_Value;
    if (proxy_port == 0)
      proxy_port = 80;
    
    timeout = Parameter_List[5].Integer_Value;
        
    if (strncmp((char *)Parameter_List[1].String_Value, "https://", 8) == 0)
    {
    #ifdef INCLUDE_SSLDEMO
      server_offset = 8;
      arg->sslCtx = qapi_Net_SSL_Obj_New(QAPI_NET_SSL_CLIENT_E);
      if (arg->sslCtx == QAPI_NET_SSL_INVALID_HANDLE)
      {
        if (arg->sslCfg)
          httpc_free(arg->sslCfg);

        memset(arg, 0, sizeof(struct http_client_demo_s));
        HTTPC_PRINTF("ERROR: Unable to create SSL context\n");
        status = QCLI_STATUS_ERROR_E;
        goto end;
      }

      if (qapi_Net_SSL_Cert_Load(arg->sslCtx, QAPI_NET_SSL_CERTIFICATE_E, "ca.crt.bin") < 0)
      {
        if (arg->sslCtx)
          qapi_Net_SSL_Obj_Free(arg->sslCtx);

        if (arg->sslCfg)
          httpc_free(arg->sslCfg); 

        memset(arg, 0, sizeof(struct http_client_demo_s));
        HTTPC_PRINTF("ERROR: Unable to load the SSL certificate\n");
        status = QCLI_STATUS_ERROR_E;
        goto end;
      }

      HTTPC_PRINTF("%s loaded from FLASH\r\n", "ca_crt.bin");

      if (qapi_Net_SSL_Cert_Load(arg->sslCtx, QAPI_NET_SSL_CA_LIST_E, "ca_list.bin") < 0)
      {
        if (arg->sslCtx)
          qapi_Net_SSL_Obj_Free(arg->sslCtx);

        if (arg->sslCfg)
          httpc_free(arg->sslCfg); 

        memset(arg, 0, sizeof(struct http_client_demo_s));
        HTTPC_PRINTF("ERROR: Unable to load the SSL CA list\n");
        status = QCLI_STATUS_ERROR_E;
        goto end;
      }

      HTTPC_PRINTF("%s loaded from FLASH\r\n", "ca_list.bin");
	 #endif
    }
    else if (strncmp((char *)Parameter_List[1].String_Value, "http://", 7) == 0)
      server_offset = 7;
    
    if (strncmp((char *)Parameter_List[3].String_Value, "http://", 7) == 0)
      proxy_offset = 7;
    else if (strncmp((char *)Parameter_List[3].String_Value, "https://", 8) == 0)
      proxy_offset = 8;
            
    httpc_max_body_length = (httpc_demo_max_body_len[app_id]) ? httpc_demo_max_body_len[app_id] : HTTPC_DEMO_DEFAULT_MAX_BODY_LEN;
    httpc_max_header_length = (httpc_demo_max_header_len[app_id]) ? httpc_demo_max_header_len[app_id] : HTTPC_DEMO_DEFAULT_MAX_HEADER_LEN;

    arg->client = qapi_Net_HTTPc_New_sess(timeout, arg->sslCtx, http_client_cb_demo, (void *)arg, httpc_max_body_length, httpc_max_header_length);
    if (arg->client == NULL)
    {
      if (arg->sslCtx)
        qapi_Net_SSL_Obj_Free(arg->sslCtx);

      if (arg->sslCfg)
        httpc_free(arg->sslCfg); 

      memset(arg, 0, sizeof(struct http_client_demo_s));  
      HTTPC_PRINTF("No available http client session\r\n");
      status = QCLI_STATUS_ERROR_E;
      goto end;
    }

    for(i = 6; i < Parameter_Count; ++i)
    {
      if(strncmp((char *)Parameter_List[i].String_Value, "-dns_if", 7) == 0)
      {
        ++i;
        if((char *)Parameter_List[i].String_Value != NULL)
        {
          error = qapi_Net_HTTPc_Extended_Config_Options(arg->client,
                                                          QAPI_NET_HTTPC_EXTENDED_CONFIG_DNS_RESOLVE_INTERFACE,
                                                          (char *)Parameter_List[i].String_Value,
                                                          strlen((char *)Parameter_List[i].String_Value));
          if(error != QAPI_OK)
            HTTPC_PRINTF("fail to set dns interface: %s\n", (char *)Parameter_List[i].String_Value);
          else
            HTTPC_PRINTF("succeed to set dns interface: %s\n", (char *)Parameter_List[i].String_Value);
        }
        else
          goto usage;
      }
      else if(strncmp((char *)Parameter_List[i].String_Value, "-bind_addr", 10) == 0)
      {
        ++i;
        if((char *)Parameter_List[i].String_Value != NULL)
        {
          error = qapi_Net_HTTPc_Extended_Config_Options(arg->client,
                                                          QAPI_NET_HTTPC_EXTENDED_CONFIG_BIND_ADDRESS,
                                                          (char *)Parameter_List[i].String_Value,
                                                          strlen((char *)Parameter_List[i].String_Value));
          if(error != QAPI_OK)
            HTTPC_PRINTF("fail to set bind address: %s\n", (char *)Parameter_List[i].String_Value);
          else
            HTTPC_PRINTF("succeed to set bind address: %s\n", (char *)Parameter_List[i].String_Value);
        }
        else
          goto usage;
      }
    }

    if (qapi_Net_HTTPc_Pass_Pool_Ptr(arg->client, byte_pool_qcli)!=QAPI_OK)
    {
      HTTPC_PRINTF("Error is setting User space pointer\n");
      status = QCLI_STATUS_ERROR_E;
      goto end;
    }
    
    if (arg->httpc_cfg)
    {
      error = qapi_Net_HTTPc_Configure(arg->client, arg->httpc_cfg);
      if (error)
      {
        if (arg->sslCtx)
          qapi_Net_SSL_Obj_Free(arg->sslCtx);
    
        if (arg->sslCfg)
          httpc_free(arg->sslCfg); 
    
        if (arg->httpc_cfg->sock_options)
          httpc_free(arg->httpc_cfg->sock_options);
     
        httpc_free(arg->httpc_cfg);
        
        memset(arg, 0, sizeof(struct http_client_demo_s));
        HTTPC_PRINTF("http client configuration failed on error: %d\n", error);
        status = QCLI_STATUS_ERROR_E;
        goto end;
      }    
    
      HTTPC_PRINTF("http client configuration successful\n");
    }

    /* SSL object is created and paramters are parsed */
    error = QAPI_OK; 
    if ((arg->sslCtx != 0 && arg->sslCtx != QAPI_NET_SSL_INVALID_HANDLE)) 
    {
      uint8_t server_url[256]; 

      if (arg->sslCfg != NULL)
        error = qapi_Net_HTTPc_Configure_SSL(arg->client, arg->sslCfg);  
 
      if (QAPI_OK == error)
         error = qapi_Net_HTTPc_Proxy_Connect(arg->client, (const char *)((char *)Parameter_List[3].String_Value + proxy_offset), proxy_port, true);
  
      if (error) 
      {
        qapi_Net_HTTPc_Free_sess(arg->client);
  
        if (arg->sslCtx)
          qapi_Net_SSL_Obj_Free(arg->sslCtx);
  
        if (arg->sslCfg)
          httpc_free(arg->sslCfg); 
  
        memset(arg, 0, sizeof(struct http_client_demo_s)); 
        HTTPC_PRINTF("http client proxy connect failed connecting to proxy server[%s] with error[%d]\n", Parameter_List[3].String_Value, error);      
        status = QCLI_STATUS_ERROR_E;
        goto end;
      }

      /* Perform HTTP CONNECT request to connect to the web server */
      snprintf((char *)server_url, sizeof(server_url), "[%s]:%d", (const char *)Parameter_List[1].String_Value + server_offset, server_port);
      error = qapi_Net_HTTPc_Request(arg->client, QAPI_NET_HTTP_CLIENT_CONNECT_E, (const char *)server_url);
      if (error) 
      {
        qapi_Net_HTTPc_Free_sess(arg->client);
  
        if (arg->sslCtx)
          qapi_Net_SSL_Obj_Free(arg->sslCtx);
  
        if (arg->sslCfg)
          httpc_free(arg->sslCfg); 
  
        memset(arg, 0, sizeof(struct http_client_demo_s)); 
        HTTPC_PRINTF("http client connect request failed for server[%s] with error[%d]\n", Parameter_List[1].String_Value, error);      
        status = QCLI_STATUS_ERROR_E;
        goto end;
      }     
 
      /* Wait for the callback response */
      arg->connect = TRUE;
      arg->server_port = server_port; 
      memcpy(arg->server_url, Parameter_List[1].String_Value, sizeof(arg->server_url));
    }
    else 
    {
      error = qapi_Net_HTTPc_Proxy_Connect(arg->client, (const char *)((char *)Parameter_List[3].String_Value + proxy_offset), proxy_port, false);
      if (error) 
      {
        qapi_Net_HTTPc_Free_sess(arg->client);
  
        memset(arg, 0, sizeof(struct http_client_demo_s)); 
        HTTPC_PRINTF("http client proxy connect failed connecting to proxy server[%s] with error[%d]\n", Parameter_List[3].String_Value, error);      
        status = QCLI_STATUS_ERROR_E;
        goto end;
      }

      error = qapi_Net_HTTPc_Proxy_Connect(arg->client, (const char *)((char *)Parameter_List[3].String_Value + server_offset), server_port, false);
      if (error) 
      {
        qapi_Net_HTTPc_Free_sess(arg->client);
  
        memset(arg, 0, sizeof(struct http_client_demo_s)); 
        HTTPC_PRINTF("http client proxy connect failed connecting to proxy server[%s] with error[%d]\n", Parameter_List[3].String_Value, error);      
        status = QCLI_STATUS_ERROR_E;
        goto end;
      }
    }
 
    HTTPC_PRINTF("http client connect success <client num> = %d\n", arg->num);
    status = QCLI_STATUS_SUCCESS_E;
    goto end;
  }
  
  /* For commands which require <client_num> */
  if (Parameter_Count < 2)
  {
    HTTPC_PRINTF("Client session ID missing\n");
    status = QCLI_STATUS_SUCCESS_E;
    goto end;
  }

  num = Parameter_List[1].Integer_Value;  
#ifdef INCLUDE_SSLDEMO
  if (strcmp(command, "sslconfig") == 0)
  {

    if (Parameter_Count < 3)
    {
      HTTPC_PRINTF("Invalid SSL configuration parameters\n");
      goto usage;
    }

    if (num > 0 && num <= HTTPC_DEMO_MAX_NUM)
    {
      arg = &http_client_demo[app_id][num - 1];

      if (arg->sslCfg == NULL)
      {
        arg->sslCfg = httpc_malloc(sizeof(qapi_Net_SSL_Config_t));
        if (arg->sslCfg == NULL)
        {
          HTTPC_PRINTF("SSL configuration allocation failure\n");
          status = QCLI_STATUS_ERROR_E;
          goto end;
        }
 
        memset(arg->sslCfg, 0, sizeof(qapi_Net_SSL_Config_t));
      }

      /* Parse SSL config parameters from command line */
      status = ssl_parse_config_parameters(Parameter_Count-2, &Parameter_List[2], arg->sslCfg, false);
      if (status == QCLI_STATUS_USAGE_E)
        goto usage;   
    }
    else
    {
      status = QCLI_STATUS_ERROR_E;
      HTTPC_PRINTF("<client num> error\n");
    } 
    goto end;
  }
#endif
  if (strcmp(command, "config") == 0)
  {
    typedef struct _linger {
      int l_onoff;
      int l_linger;
    } linger;

    qapi_Net_HTTPc_Config_t *httpc_cfg = NULL; 
    linger so_linger; 

    if (Parameter_Count < 10) 
    {
      HTTPC_PRINTF("Missing configuration options\n");
      status = QCLI_STATUS_ERROR_E;
      goto end;
    } 

    if (num > 0 && num <= HTTPC_DEMO_MAX_NUM)
    {
      arg = &http_client_demo[app_id][num - 1];

      if ((httpc_cfg = httpc_malloc(sizeof(qapi_Net_HTTPc_Config_t))) == NULL)
      {
        status = QCLI_STATUS_ERROR_E;
        goto end;
      }
      memset(httpc_cfg, 0x00, sizeof(qapi_Net_HTTPc_Config_t));
      memset(&so_linger, 0x00, sizeof(linger));
  
      httpc_cfg->addr_type = Parameter_List[2].Integer_Value;
  
      if (SO_LINGER == Parameter_List[3].Integer_Value)
      {
        httpc_cfg->sock_options_cnt = 0x01;
  
        if ((httpc_cfg->sock_options = httpc_malloc(sizeof(qapi_Net_HTTPc_Sock_Opts_t))) == NULL)
        {
          status = QCLI_STATUS_ERROR_E;
          goto end;
        }
    
        so_linger.l_onoff = 1;
        so_linger.l_linger = Parameter_List[4].Integer_Value; 
   
        httpc_cfg->sock_options->level = SOL_SOCKET;
        httpc_cfg->sock_options->opt_name = SO_LINGER; 
        httpc_cfg->sock_options->opt_len = sizeof(linger);
        httpc_cfg->sock_options->opt_value = &so_linger; 
      }
  
      httpc_cfg->max_send_chunk = Parameter_List[5].Integer_Value;
      httpc_cfg->max_send_chunk_delay_ms = Parameter_List[6].Integer_Value;
      httpc_cfg->max_send_chunk_retries = Parameter_List[7].Integer_Value;
      httpc_cfg->max_conn_poll_cnt = Parameter_List[8].Integer_Value;
      httpc_cfg->max_conn_poll_interval_ms = Parameter_List[9].Integer_Value;
  
      if (arg->httpc_cfg)
      {
        if (arg->httpc_cfg->sock_options)
          httpc_free(arg->httpc_cfg->sock_options);
   
        httpc_free(arg->httpc_cfg);
        arg->httpc_cfg = NULL;
      }
    
      arg->httpc_cfg = httpc_cfg; 

      status = QCLI_STATUS_SUCCESS_E;
      goto end;
    }
    else
    {
      HTTPC_PRINTF("<client num> error\n");
      status = QCLI_STATUS_ERROR_E;
      goto end;
    } 
  }
  
  if (num > 0 && num <= HTTPC_DEMO_MAX_NUM)
  {
    arg = &http_client_demo[app_id][num - 1];
  
    if (arg->client == NULL || arg->num != num)
      arg = NULL; 
  }
  
  HTTPC_PRINTF("<client num> %d\n",num);
  
  if (strcmp(command, "disconnect") == 0)
  {
    if (!arg)
    {
      HTTPC_PRINTF("<client num> error\n");
      status = QCLI_STATUS_ERROR_E;
      goto end;
    }
    
    qapi_Net_HTTPc_Free_sess(arg->client);

    if (arg->sslCtx)
      qapi_Net_SSL_Obj_Free(arg->sslCtx);

    if (arg->sslCfg)
      httpc_free(arg->sslCfg); 

    if (arg->httpc_cfg)
    {
      if (arg->httpc_cfg->sock_options)
        httpc_free(arg->httpc_cfg->sock_options);
 
      httpc_free(arg->httpc_cfg);
    }

    memset(arg, 0, sizeof(struct http_client_demo_s)); 
    status = QCLI_STATUS_SUCCESS_E;
    goto end;
  }

  if (strcmp(command, "get") == 0)
    req_cmd = QAPI_NET_HTTP_CLIENT_GET_E;
  else if (strcmp(command, "put") == 0)
    req_cmd = QAPI_NET_HTTP_CLIENT_PUT_E;
  else if (strcmp(command, "post") == 0)
    req_cmd = QAPI_NET_HTTP_CLIENT_POST_E;
  else if (strcmp(command, "patch") == 0)
    req_cmd = QAPI_NET_HTTP_CLIENT_PATCH_E;
  else if (strcmp(command, "head") == 0)
    req_cmd = QAPI_NET_HTTP_CLIENT_HEAD_E;
  else if (strcmp(command, "delete") == 0)
  {
    qapi_Version_Object_t app_ver, kernel_ver;

    memset(&app_ver, 0, sizeof(qapi_Version_Object_t));
    memset(&kernel_ver, 0, sizeof(qapi_Version_Object_t));

    qapi_Version_Get_App_Version(QAPI_VERSION_CATEGORY_DATA_SERVICE, &app_ver);
    HTTPC_PRINTF("DAM_APP: dataservice QAPI version for app is : %s\n", app_ver.ver_str);

    qapi_Version_Get_Kernel_Version(QAPI_VERSION_CATEGORY_DATA_SERVICE, &kernel_ver);
    HTTPC_PRINTF("DAM_APP: dataservice QAPI version for kernel is : %s\n", kernel_ver.ver_str);

    /* support DELETE method from dataservice version 1.5.0 */
    if((app_ver.major_num > DATASERVICE_VERSION_SUPPORT_DELETE_MAJOR_NUM ||
         (app_ver.major_num == DATASERVICE_VERSION_SUPPORT_DELETE_MAJOR_NUM &&
          app_ver.minor_num >= DATASERVICE_VERSION_SUPPORT_DELETE_MINOR_NUM)) &&
        (kernel_ver.major_num > DATASERVICE_VERSION_SUPPORT_DELETE_MAJOR_NUM ||
          (kernel_ver.major_num == DATASERVICE_VERSION_SUPPORT_DELETE_MAJOR_NUM &&
           kernel_ver.minor_num >= DATASERVICE_VERSION_SUPPORT_DELETE_MINOR_NUM)))
    {
      req_cmd = QAPI_NET_HTTP_CLIENT_DELETE_E;
    }
    else
    {
      if(app_ver.major_num < DATASERVICE_VERSION_SUPPORT_DELETE_MAJOR_NUM ||
         (app_ver.major_num == DATASERVICE_VERSION_SUPPORT_DELETE_MAJOR_NUM &&
          app_ver.minor_num < DATASERVICE_VERSION_SUPPORT_DELETE_MINOR_NUM))
      {
        HTTPC_PRINTF("DAM_APP: please update DAM QCLI with dataservice QAPI version greater than %d.%.d.0 \n",
                    DATASERVICE_VERSION_SUPPORT_DELETE_MAJOR_NUM,
                    DATASERVICE_VERSION_SUPPORT_DELETE_MINOR_NUM);
      }
      if(kernel_ver.major_num < DATASERVICE_VERSION_SUPPORT_DELETE_MAJOR_NUM ||
         (kernel_ver.major_num == DATASERVICE_VERSION_SUPPORT_DELETE_MAJOR_NUM &&
          kernel_ver.minor_num < DATASERVICE_VERSION_SUPPORT_DELETE_MINOR_NUM))
      {
        HTTPC_PRINTF("DAM_APP: please update Kernel with dataservice QAPI version greater than %d.%.d.0 \n",
                    DATASERVICE_VERSION_SUPPORT_DELETE_MAJOR_NUM,
                    DATASERVICE_VERSION_SUPPORT_DELETE_MINOR_NUM);
      }
      status = QCLI_STATUS_ERROR_E;
      goto end; 
    }
  }


  if (req_cmd)
  {
    int cnt = 0; 
    char url[256]; 

    HTTPC_PRINTF("req_cmd %d\n",req_cmd);
    if (Parameter_Count < 3) 
    {
      HTTPC_PRINTF("Missing <url>\n");
      status = QCLI_STATUS_ERROR_E;
      goto end;
    }

    if (!arg)
    {
      HTTPC_PRINTF("<client num> error\n");
      status = QCLI_STATUS_ERROR_E;
      goto end;
    }

    memset(url, 0x00, sizeof(url));
    cnt = snprintf(url, sizeof(url), "%s", Parameter_List[2].String_Value);
  
    if (Parameter_Count >= 4 && Parameter_List[3].String_Value)
      snprintf(&url[cnt], sizeof(url) - cnt, " %s", Parameter_List[3].String_Value);
          
    error = qapi_Net_HTTPc_Request(arg->client, req_cmd, (const char *)url);
  } 
  else if (strcmp(command, "setbody") == 0)
  {
    char *body = NULL;
    uint32_t len = HTTPC_DEMO_DEFAULT_BODY_LEN;

    if (!arg)
    {
      HTTPC_PRINTF("<client num> error\n");
      status = QCLI_STATUS_ERROR_E;
      goto end;
    }

    if (Parameter_Count > 2)
      len = Parameter_List[2].Integer_Value;

    if (len > HTTPC_DEMO_MAX_BODY_LEN)
      len = HTTPC_DEMO_MAX_BODY_LEN;

    body = httpc_malloc_body_demo(len);
    if (!body)
    {
      status = QCLI_STATUS_ERROR_E;
      goto end;
    }

    HTTPC_PRINTF("len = %d : %d\n%s\n", len, strlen(body), body);
      
    error = qapi_Net_HTTPc_Set_Body(arg->client, (const char*)body, strlen(body));
    if (body)
      httpc_free(body);
  }
  else if (strcmp(command, "addheaderfield") == 0)
  {
    if (!arg)
    {
      HTTPC_PRINTF("<client num> error\n");
      status = QCLI_STATUS_ERROR_E;
      goto end;
    }

    if (Parameter_Count < 4)
    {
      HTTPC_PRINTF("Missing parameters\n");
      status = QCLI_STATUS_ERROR_E;
      goto end;
    }
  
    error = qapi_Net_HTTPc_Add_Header_Field(arg->client, (const char *)Parameter_List[2].String_Value, (const char *)Parameter_List[3].String_Value);
  }
  else if (strcmp(command, "clearheader") == 0)
  {
    if (!arg)
    {
      HTTPC_PRINTF("<client num> error\n");
      status = QCLI_STATUS_ERROR_E;
      goto end;
    }

    error = qapi_Net_HTTPc_Clear_Header(arg->client);
  }
  else if(strcmp(command, "setparam") == 0)
  {
    if (!arg)
    {
      HTTPC_PRINTF("<client num> error\n");
      status = QCLI_STATUS_ERROR_E;
      goto end;
    }

    if (Parameter_Count < 4)
    {
      HTTPC_PRINTF("Missing parameters\n");
      status = QCLI_STATUS_ERROR_E;
      goto end;
    }
  
    error = qapi_Net_HTTPc_Set_Param(arg->client, (const char *)Parameter_List[2].String_Value, (const char *)Parameter_List[3].String_Value);
  }
  else if(strcmp(command, "lasterror") == 0)
  {
    if (!arg)
    {
      HTTPC_PRINTF("<client num> error\n");
      status = QCLI_STATUS_ERROR_E;
      goto end;
    }
    error = get_last_error_httpc(Parameter_Count - 1, &Parameter_List[1], arg->client);
  }
  else
  {
    HTTPC_PRINTF("Unknown http client command.\n");
    status = QCLI_STATUS_ERROR_E;
    goto end;
  }

  if (error)
  {
    HTTPC_PRINTF("http client %s failed on error: %d\n", command, error);
    status = QCLI_STATUS_ERROR_E;
    goto end;
  }

end:
  if (Parameter_List != NULL)
  {
    for (i = 0; i<Parameter_Count; i++)
    {
      if (Parameter_List[i].String_Value != NULL)
      {
        httpc_free(Parameter_List[i].String_Value);
        Parameter_List[i].String_Value = NULL;
      }
    }
  }

  return status;
}

