/******************************************************************************

  @file    connection.c

  ---------------------------------------------------------------------------

  Copyright (c) 2016-2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------

 *****************************************************************************/

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
 *    Pascal Rieux - Please refer to git log
 *    
 *******************************************************************************/

#include <stdlib.h>
#include <ctype.h>
#include "qapi_dss.h"
#include <stringl/stringl.h>
#include <time.h>
#include <time_svc.h>
#include "qapi_socket.h"
#include "qapi_coap.h"
#include "qapi_dnsc.h"
#include "iotapp_dss_util.h"
#include "app_coap.h"
#include "connection.h"
#include "coap-internal.h"
#include "qurt.h"
#include "net_utils_ext.h"
#include "qapi_net_status.h"


#define htons(s) ((((s) >> 8) & 0xff) | \
    (((s) << 8) & 0xff00))
#define ntohs(s) htons(s)

#define IFACE_NAME_MAX_LEN 20
#define COAP_MAX_HOST_LENGTH      64
#define PRV_ADDRSTRLEN  48

extern qurt_signal_t coap_sig;
const char* coap_malloc_error = "COAP : Memory allocation failed";;
const char* coap_invalid_params = "COAP: Invalid Params Passed";
const char* coap_ctx_fail = "COAP:Failed to retrieve Context from CID";
int sock_errnum;

#ifdef APPS_PACKET_LOGGING_ENABLE
extern void dump_buf_to_dpl(int handle, uint8_t * dump_buf, int buf_len, bool recvd);
#endif

extern generate_rand32(void);

/* In case of message send failure call this function for Applications to know about failure case */

void Callback_Invoke_If_Send_Failure(client_context_t* context , coap_transaction_t * transacP)
{

   client_context_t* contextP = context;
   void * umem = NULL;
   qapi_Coap_Transaction_t * clientTransaction = NULL;
   coap_transaction_t * transaction = NULL;
   int ret = QAPI_OK;

   if (contextP == NULL || transacP == NULL)
   {
     return;
   }

   umem = mem_utility_Get_MemoryPool((void *)contextP->cid, APP_COAP);
   clientTransaction = (qapi_Coap_Transaction_t *)mem_utility_Get_Memory_From_Pool(umem, sizeof(qapi_Coap_Transaction_t));

	
	if(transacP)
	{
	  /* In case of resumption failure inform applications via transaction callback */
	
	  COAP_LOCK_MUTEX(contextP->CoapClientMutex);

	   transaction = contextP->transactionList;
	   while (NULL != transaction)
	   {
         if(clientTransaction)
         {
         	ret = coap_init_client_transaction(&clientTransaction , contextP ,transaction , umem , false, NULL);
		  
            if(ret == QAPI_OK)
			 {
               ds_iot_ulog_high("Calling callbacks before flushing the packets");
			   transaction->callback((void *)contextP->cid, clientTransaction, NULL);
			 }
         }
	     else
		 {
		   ds_iot_ulog_high("COAP : Memory allocation failed");
		   COAP_UNLOCK_MUTEX(contextP->CoapClientMutex);
		   return;
		 }
	
		 transaction = transaction->next;
	   }
	   
	   if(clientTransaction)
	   {
	     mem_utility_release_memory(umem!=NULL, clientTransaction);
	   }
	   
	   COAP_UNLOCK_MUTEX(contextP->CoapClientMutex);
	}

	return;
}


/* @fn     int create_socket()
 * @brief  This function is used to create the socket 
 * @param  client_addr - struct sockaddr type
 * @return int           - socket fd  
 *
 */
int create_socket(struct sockaddr client_addr, bool isBindRequired , uint32_t dataCallHdl)
{
  int s = EFAILURE;
  char local_ip[48] = {0};
  int rc = -1;
  struct sockaddr_in *sa = NULL;
  struct sockaddr_in6 *sa6 = NULL;
  struct timeval timeout;
  uint32_t callhandle = dataCallHdl;

  timeout.tv_sec = 1;  
  timeout.tv_usec = 0;
  
  if(client_addr.sa_family == AF_INET)
  {
    sa = (struct sockaddr_in *)&client_addr;
    inet_ntop(AF_INET, &sa->sin_addr, local_ip, INET4_ADDRSTRLEN);
  }

  if(client_addr.sa_family == AF_INET6)
  {
    sa6 = (struct sockaddr_in6 *)&client_addr;
    inet_ntop(AF_INET6, &sa6->sin_addr, local_ip, INET6_ADDRSTRLEN);
  }
 

  if(client_addr.sa_family != AF_NONIP)
  {
    ds_iot_log_strf_high("COAP: Binding to IP :%s  ",local_ip);
    s = qapi_socket(client_addr.sa_family, SOCK_DGRAM, 0);
  }
  else
  {
    ds_iot_ulog_high1("COAP: Binding to NON-IP Socket with Call Handle: %d ",dataCallHdl);
    s = qapi_socket(AF_NONIP, SOCK_NONIP, 0);
  }
  if (s >= ESUCCESS)
  {
  
    if (qapi_setsockopt (s, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0) 
    {
      ds_iot_ulog_err("COAP: Set sock failed");
	  sock_errnum = qapi_errno(s);
    }

    if(client_addr.sa_family == AF_NONIP)
    {
      
       if ((qapi_setsockopt(s, SOL_SOCKET, SO_NONIP_DSSHNDL, &callhandle, sizeof(callhandle))) < 0) 
       {
           ds_iot_ulog_err("COAP: Set sock failed for NON IP DataCall Handle");
		   sock_errnum = qapi_errno(s);
		   s = EFAILURE;
       }
    }

    if(isBindRequired && (client_addr.sa_family != AF_NONIP))
    {
      if(client_addr.sa_family == AF_INET)
      {
         if (-1 == qapi_bind(s, (struct sockaddr *)&client_addr, sizeof(struct sockaddr_in)))
         {
           ds_iot_ulog_err("COAP: Bind failed in create_socket");
		   sock_errnum = qapi_errno(s);
		   qapi_socketclose(s);
           s = EFAILURE;
         }
      }
      if(client_addr.sa_family == AF_INET6)
      {
        rc = qapi_bind(s, (struct sockaddr *)&client_addr, sizeof(struct sockaddr_in6));
        if (rc == -1)
        {
          int sock_err;
          ds_iot_ulog_high("COAP: Bind failed in create_socket");
          rc = qapi_getsockopt(s, SOL_SOCKET, SO_ERROR, &sock_err, sizeof(int));
          ds_iot_ulog_high1("COAP: qapi_getsockopt(SOL_SOCKET:SO_ERROR) returned %d ", rc);
          if (rc == -1)
          {
            ds_iot_ulog_high1("COAP: errno for failed bind: %d ", sock_err);
          }
		  sock_errnum = sock_err;
          qapi_socketclose(s);
          s = EFAILURE;  
        }  
      }
   }
  } else 
  {
    ds_iot_ulog_err("COAP: Create Socket failed");
	sock_errnum = EFAILURE;
    s = EFAILURE;
  }

  return s;
}

/* @fn     connection_t * connection_new_incoming ()
 * @brief  This function is used to create new connection 
 * @param  connList - pointer to connection list 
 *         sock     - sock fd
 *         add      - pointer to sockaddr 
 *         addLen   - length of address    
 * @return connP    - pointer to connection_t  
 */

connection_t * connection_new_incoming(int sock,
    struct sockaddr * addr,
    size_t addrLen)
{
  connection_t * connP;

  connP = (connection_t *)malloc(sizeof(connection_t));
  
  if (connP != NULL)
  {
    memset(connP, 0, sizeof(connection_t));
    connP->sock = sock;
    COAP_MEMCPY(&(connP->addr), sizeof(connP->addr), addr, addrLen);
    connP->addrLen = addrLen;
    /*last_conn_time get updated whenever new connection session is created*/
    connP->last_conn_time = (time_t)time_get_secs();
    ds_iot_ulog_high("COAP : Updated last packet time");
  }

  return connP;
}

/**
 * @fn  resolve_identifier_fqdn()
 * @brief  This function is used to resolve host name into ip
 *
 * @param callHdl handle to data call handle
 * @param host Host name to be resolved
 *
 * @return  ESUCCESS on success,
 *          EFAILURE on failure
 */
int resolve_identifier_fqdn(qapi_Net_SSL_Identifier_t *identifier, uint32_t callHdl, uint16_t addr_type)
{
  
  int ret = QAPI_OK;
  struct ip46addr dnsInfo;
  char ifname[IFACE_NAME_MAX_LEN];
  char identi_name[QAPI_NET_SSL_MAX_DOMAIN_NAME_LEN];
  struct sockaddr_in addr;
  struct sockaddr_in6 addr6;
  qapi_DSS_Addr_Info_t *ip_info = NULL;
  char dns_svr_buf[PRV_ADDRSTRLEN];

  char *host = NULL;

  if (callHdl == 0 || identifier == NULL || identifier->identifier_Name == NULL) 
  {
    ds_iot_ulog_err("COAP: identifier or ifname is null"); 
    return QAPI_ERROR;
  }

  host = extractIPAddrFromUrl(identifier->identifier_Name, identi_name);
  if(host == NULL)
  	goto host_error;
  else
    ds_iot_log_strf_high("COAP : Identifier %s", host);

  memset(&dnsInfo, 0, sizeof(dnsInfo));
  dnsInfo.type = addr_type;

  if (addr_type == AF_INET) 
  {
    /* IPv4 address */
    memset(&addr, 0, sizeof(addr));
    if (!(inet_pton(AF_INET, host, &addr.sin_addr)))
    {
      ds_iot_log_strf_high("COAP: identifier is IPv4 literal %s",host);
      return QAPI_OK;
    }
    else
    {
		uint32_t len = 0;
       qapi_DSS_Addr_Info_t *addr_info = NULL;

       /* Retrieve IP address information */   
        qapi_DSS_Get_IP_Addr_Count((qapi_DSS_Hndl_t )callHdl, &len);
        if (0 != len)
        {
           addr_info = (qapi_DSS_Addr_Info_t *)malloc(sizeof(qapi_DSS_Addr_Info_t) * len);

           if(addr_info != NULL)
           {
             memset(addr_info, 0x00, sizeof(qapi_DSS_Addr_Info_t));

             if (qapi_DSS_Get_IP_Addr((qapi_DSS_Hndl_t )callHdl, addr_info, len) != 0)
             {
               if (addr_info)
               free(addr_info);
             }
          }
        }
		
		
      ip_info = (qapi_DSS_Addr_Info_t * )addr_info;;
      if(NULL != ip_info)
      {
        // Fetching interface name
        if( QAPI_OK != qapi_DSS_Get_Device_Name((qapi_DSS_Hndl_t )callHdl, ifname, IFACE_NAME_MAX_LEN ) )
        {
          ds_iot_ulog_err("LWM2M_LOG : Fetching iface name failed ");
          free(ip_info);
          return QAPI_ERROR;
        }

        ret = qapi_Net_DNSc_Command(QAPI_NET_DNS_START_E);

        if(ret != QAPI_OK) 
        {
          ds_iot_ulog_err1("COAP : DNS client start failed %d", ret);
          free(ip_info);
          return EFAILURE;
        }

        // Fetching Primary DNS address
        if(ip_info->dnsp_addr_s.valid_addr == 0x1) 
        {
          inet_ntop(AF_INET, &(ip_info->dnsp_addr_s.addr.v4), dns_svr_buf,
              PRV_ADDRSTRLEN);

          ret = qapi_Net_DNSc_Add_Server_on_iface(dns_svr_buf, QAPI_NET_DNS_ANY_SERVER_ID, ifname);

          if(ret != QAPI_OK) 
          {
            ds_iot_ulog_err1("LWM2M_LOG : Adding Primary DNS Server failed %d", ret);
          }
        }
        // Fetching Secondary DNS address
        if(ip_info->dnss_addr_s.valid_addr == 0x1) 
        {
          inet_ntop(AF_INET, &(ip_info->dnss_addr_s.addr.v4), dns_svr_buf,
              PRV_ADDRSTRLEN);
          ret = qapi_Net_DNSc_Add_Server_on_iface(dns_svr_buf, QAPI_NET_DNS_ANY_SERVER_ID, ifname);
          if(ret != QAPI_OK) 
          {
            ds_iot_ulog_err1("LWM2M_LOG : Adding Secondary DNS Server failed %d", ret);
          }
        }

      }
      else
      {
        ds_iot_ulog_err1("LWM2M_LOG : Failed to get primary and secondary DNS IP Address %d", ret);
        return EFAILURE;
      }

    }
  }
  else if(addr_type == AF_INET6) 
  {
    /* IPv6 address */
    memset(&addr6, 0, sizeof(addr6));
    if (!(inet_pton(AF_INET6, host, &addr6.sin_addr)))
    {
      ds_iot_log_strf_high("COAP: identifier is IPv4 literal %s",host);
      return QAPI_OK;
    }
    else
    {
	   uint32_t len = 0;
       qapi_DSS_Addr_Info_t *addr_info = NULL;

       /* Retrieve IP address information */   
        qapi_DSS_Get_IP_Addr_Count(*((qapi_DSS_Hndl_t *)callHdl), &len);
        if (0 != len)
        {
           addr_info = (qapi_DSS_Addr_Info_t *)malloc(sizeof(qapi_DSS_Addr_Info_t) * len);

           if(addr_info != NULL)
           {
             memset(addr_info, 0x00, sizeof(qapi_DSS_Addr_Info_t));

             if (qapi_DSS_Get_IP_Addr(*((qapi_DSS_Hndl_t *)callHdl), addr_info, len) != 0)
             {
               if (addr_info)
               free(addr_info);
             }
          }
        }
		
      ip_info = (qapi_DSS_Addr_Info_t * )addr_info;
      if(NULL != ip_info)
      {
        // Fetching interface name
        if( QAPI_OK != qapi_DSS_Get_Device_Name(*((qapi_DSS_Hndl_t *)callHdl), ifname, IFACE_NAME_MAX_LEN ) )
        {
          free(ip_info);
          ds_iot_ulog_err("LWM2M_LOG : Fetching iface name failed ");
          return QAPI_ERROR;
        }
        ds_iot_log_strf_high("ifname: %s", ifname);
        ret = qapi_Net_DNSc_Command(QAPI_NET_DNS_START_E);

        if(ret != QAPI_OK) 
        {
          ds_iot_ulog_err1("COAP : DNS client start failed %d", ret);
          free(ip_info);
          return EFAILURE;
        }        
        // Fetching Primary DNS address
        if(ip_info->dnsp_addr_s.valid_addr == 0x1) 
        {
          inet_ntop(AF_INET6, &(ip_info->dnsp_addr_s.addr.v6_addr8), dns_svr_buf,
                    PRV_ADDRSTRLEN);

          ret = qapi_Net_DNSc_Add_Server_on_iface(dns_svr_buf, QAPI_NET_DNS_ANY_SERVER_ID, ifname);

          if(ret != QAPI_OK) 
          {
            ds_iot_ulog_err1("CoAP : Adding Primary DNS Server failed %d", ret);
          }
        }
        // Fetching Secondary DNS address
        if(ip_info->dnss_addr_s.valid_addr == 0x1) 
        {
          inet_ntop(AF_INET6, &(ip_info->dnss_addr_s.addr.v6_addr8), dns_svr_buf,
                    PRV_ADDRSTRLEN);
          ret = qapi_Net_DNSc_Add_Server_on_iface(dns_svr_buf, QAPI_NET_DNS_ANY_SERVER_ID, ifname);
          if(ret != QAPI_OK) 
          {
            ds_iot_ulog_err1("CoAP : Adding Secondary DNS Server failed %d", ret);
          }
        }

      }
      else
      {
        ds_iot_ulog_high1("CoAP : Failed to get primary and secondary DNS IP Address %d", ret);
        return QAPI_ERROR;
      }
    }
  }
  else
  {
    ds_iot_ulog_high1("Incorrect IP type %d ", addr_type);
    return QAPI_ERROR;
  }

  /* Query the DNS server based on the application configuration */
  ds_iot_log_strf_high("COAP : Trying DNS resolve for %s on %s", host, ifname);
  
  ret = qapi_Net_DNSc_Reshost_on_iface(host, &dnsInfo, ifname);
  qapi_Net_DNSc_Command(QAPI_NET_DNS_STOP_E);

  if(ret != QAPI_OK) 
  {
    ret = QAPI_ERROR;
    ds_iot_ulog_err1("COAP: DNS Server not resolved %d", ret);
  }
  else
  {
    ds_iot_log_strf_high("COAP: Intentifer is resolved to %s", inet_ntop(addr_type, &dnsInfo.a, host, PRV_ADDRSTRLEN));
  }

  if(ip_info)
  {
    free(ip_info);
  }

  return ret;

host_error:
  ds_iot_ulog_err("COAP: host parsing from identifier is failed"); 
  return QAPI_ERROR;
}


/* @fn     connection_t * connection_create()
 * @brief  This function is used to create new connection
 * @param  contextP   - pointer to client_context_t 
 *         host       - pointer to ip string
 *         port       - pointer to port string 
 * @return connp      - pointer to connection_t
 *
 */

int connection_create(void *contextP, qapi_Coap_Connection_Cfg_t * conn_config)
{

    char * host;
    uint16_t port;
    char *lhost ;
    uint16_t lport;
    uint16_t family_type;
    qapi_Coap_Sec_Info_t* ssl_cfg;
    qapi_Net_SSL_Config_t *cfg;
    bool resumption_enabled;
    int session_resumption_time;
    int ssize;
    uint32_t callHdl = 0;

  client_context_t *context = NULL;
  struct sockaddr         client_addr;
  struct sockaddr_in sa ;
  struct sockaddr_in    * sl = NULL;
  struct sockaddr_in6   * sl6 = NULL;
  struct sockaddr_in6 sa6;
  connection_t * connP = NULL;
  bool isBindRequired = FALSE;
  struct ip46addr dnsInfo;
  int sock = -1;
#ifdef DTLS_SUPPORT
  int32_t res = QAPI_OK;
#endif
  coap_transaction_t * transaction = NULL; 


  if(contextP == NULL || conn_config == NULL)
  return INVALID_PARAMS;

  host = conn_config->dst_host;
  port = conn_config->dst_port;
  lhost = conn_config->src_host;
  lport = conn_config->src_port;
  family_type = conn_config->family_type;
  ssl_cfg = conn_config->sec_info;
  cfg = conn_config->ssl_cfg;
  resumption_enabled = conn_config->resumption_enabled;
  session_resumption_time = conn_config->session_resumption_timeout;
  if(conn_config->callHndl)
  callHdl = conn_config->callHndl;

  memset(&dnsInfo, 0, sizeof(dnsInfo));
  memset(&sa, 0, sizeof(struct sockaddr_in));
  memset(&sa6,0,sizeof(struct sockaddr_in6));
  memset(&client_addr, 0 , sizeof(client_addr));

  context = (client_context_t *)contextP;
  dnsInfo.type = family_type;


  if((family_type == AF_NONIP) && (callHdl == 0))
  {
	  context->last_error = QAPI_ERR_COAP_EINVALID_PARAM;
	  context->mod_category = QAPI_NET_COAP_CAT;
	  return INVALID_PARAMS;
	}
  


  if(family_type != AF_NONIP)
  ds_iot_log_strf_high("COAP : connection_create() to host %s port %d", host, port);

#if 0
  if(ssl_cfg != NULL)
  {
    context->bIsCoAPUriIPLiteral = ssl_cfg->bIsCoAPUriIPLiteral;
  }
#endif
    context = (client_context_t *)contextP;
 
    client_addr.sa_family = family_type;

    if(lhost != NULL)
    {
       isBindRequired = TRUE;
       if(family_type == AF_INET)
       {
         sl = (struct sockaddr_in   * )&client_addr ;
         sl->sin_port = htons(lport);
         if(inet_pton(AF_INET, lhost,&sl->sin_addr) != 0)
		 {

		    ds_iot_ulog_err("COAP : IP validation failed v4 src");
		 	context->last_error = QAPI_ERR_COAP_EINVALID_PARAM;
			context->mod_category = QAPI_NET_COAP_CAT;
			return INVALID_PARAMS;
		 }
       }
       if(family_type == AF_INET6)
       {
         sl6 = (struct sockaddr_in6   * )&client_addr ;
         sl6->sin_port =htons(lport); 
	     if(inet_pton(AF_INET6,lhost,&sl6->sin_addr) != 0)
	     {
		    ds_iot_ulog_err("COAP : IP validation failed v6 src");
		    context->last_error = QAPI_ERR_COAP_EINVALID_PARAM;
		    context->mod_category = QAPI_NET_COAP_CAT;
		    return INVALID_PARAMS;
	     }
       }
    }

    context->nextMID = generate_rand32(); 
    context->trans_count = 0;
    context->client_addr = client_addr;
    context->sock = -1;
    context->session_resumption_time = session_resumption_time;
    context->isResumed = resumption_enabled;
  
  if(dnsInfo.type == AF_INET)
  {
	if(inet_pton(AF_INET, host, &sa.sin_addr) != 0)
	{
		ds_iot_ulog_err("COAP : IP validation failed v4 dest");
		context->last_error = QAPI_ERR_COAP_EINVALID_PARAM;
		context->mod_category = QAPI_NET_COAP_CAT;
		return INVALID_PARAMS;
	}
  }
  if(dnsInfo.type == AF_INET6)
  {
    if(inet_pton(AF_INET6, host, &sa6.sin_addr) != 0)
	{
	    ds_iot_ulog_err("COAP : IP validation failed v6 dest");
       	context->last_error = QAPI_ERR_COAP_EINVALID_PARAM;
		context->mod_category = QAPI_NET_COAP_CAT;
		return INVALID_PARAMS;
    }
  }

  sock = create_socket(context->client_addr , isBindRequired ,callHdl);
  if(sock < 0) {
    ds_iot_ulog_err("COAP : Socket creation failed");
	context->last_error = sock_errnum;
    context->mod_category = QAPI_NET_SOCKET_CAT;
    return SOCKET_ERROR;
  }
  context->sock_error = FALSE;

  if((dnsInfo.type == AF_INET) || (dnsInfo.type == AF_NONIP))
  {
    sa.sin_family= context->client_addr.sa_family;
    sa.sin_port = htons(port);
    ssize = sizeof(struct sockaddr_in);

    ds_iot_ulog_high1("COAP: Created the socket with fd %d",sock);
    connP = connection_new_incoming(sock,(struct sockaddr *)&sa, ssize);
  }
  if(dnsInfo.type == AF_INET6)
  {
    sa6.sin_family = context->client_addr.sa_family;
    sa6.sin_port = htons(port);
    ssize = sizeof(struct sockaddr_in6);

    ds_iot_ulog_high1("COAP: Created the socket with fd %d",sock);

     connP = connection_new_incoming (sock, (struct sockaddr *)&sa6, ssize);
  }
  if (connP == NULL) 
  {                                                     
    ds_iot_ulog_err("COAP : Connection creation failed."); 
	sock_errnum = qapi_errno(sock);
	if(sock_errnum != QAPI_OK)
	{
		context->last_error = sock_errnum;
    	context->mod_category = QAPI_NET_SOCKET_CAT;
	}
	else
	{
		context->last_error = QAPI_ERR_COAP_CONNECTION_CREATION_FAILED;
    	context->mod_category = QAPI_NET_COAP_CAT;
	}
    qapi_socketclose(sock);
    return SOCKET_ERROR;
  } 
  else 
  {    
#ifdef DTLS_SUPPORT

    if(NULL != ssl_cfg && NULL != cfg )
    {      
      //if(qapi_connect( connP->sock, &connP->addr, connP->addrLen) < 0)

      /*
       *  For NON IP Sockets we don't support qapi_connect()
       */
      if(dnsInfo.type != AF_NONIP)
      {
          if(qapi_connect( sock, &connP->addr, connP->addrLen) < 0)        
          {
            ds_iot_ulog_err1("COAP: qapi_connect failed closing socket %d",sock);
            sock_errnum = qapi_errno(sock);
			context->last_error = sock_errnum;
    		context->mod_category = QAPI_NET_SOCKET_CAT;
			qapi_socketclose(sock); 
            free(connP);
            return SOCKET_ERROR;
          }
      }
      /* Check if SSL context has already a valid handle, then we dont need to
         create a new context and the same context would be used for resumption */

      if(context->sslctx == QAPI_NET_SSL_INVALID_HANDLE)
      {
        context->sslctx = qapi_Net_SSL_Obj_New(QAPI_NET_SSL_CLIENT_E);
        if(context->sslctx == QAPI_NET_SSL_INVALID_HANDLE)
        {
          ds_iot_ulog_err1("COAP:Error: Unable to create SSL contextclosing socket %d",sock);
          context->last_error = QAPI_NET_SSL_INVALID_HANDLE;
    	  context->mod_category = QAPI_NET_SSL_CAT;
		  qapi_socketclose(sock);
          free(connP);
          return SSL_ERROR;
        }

        /* Configurable resumption time get updated as part of coap handle*/
        context->session_resumption_time = session_resumption_time; 
        context->ssl_mode = conn_config->sec_Mode;
	    context->disable_close_notify = conn_config->disable_close_notify;
		
        if(context->cfg.sni_Name != NULL)
        {
          free(context->cfg.sni_Name);
          context->cfg.sni_Name = NULL;
        }

        memset(&context->cfg, 0, sizeof(qapi_Net_SSL_Config_t));
        memcpy(&context->cfg, cfg, sizeof(qapi_Net_SSL_Config_t));

        if(cfg->sni_Name_Size != 0)
        {
          size_t name_size = (size_t)(cfg->sni_Name_Size + 1);

          context->cfg.sni_Name = (char *)malloc(name_size);
          if(context->cfg.sni_Name == NULL)
          {
            ds_iot_ulog_err1("COAP: Memory allocation Failed for sni_Name %d" ,sock);
            res = QAPI_ERROR;
            goto error_ssl_conn;
          }
          memset(context->cfg.sni_Name, 0, name_size);
          strlcpy(context->cfg.sni_Name, cfg->sni_Name, name_size);
        }
        else
          context->cfg.sni_Name = NULL;
        if(conn_config->sec_Mode == COAP_PSK)
        {
          ds_iot_log_strf_high("COAP: ssl_cfg->sec_mode is : %d, PSK certificate: %s", conn_config->sec_Mode, ssl_cfg->psk);
          
          strlcpy(context->ssl_psk_name, ssl_cfg->psk, sizeof(context->ssl_psk_name));
          res = qapi_Net_SSL_Cert_Load(context->sslctx, QAPI_NET_SSL_PSK_TABLE_E, ssl_cfg->psk);
		  if (res < QAPI_OK)
		  {
		    context->last_error = QAPI_ERR_SSL_CERT;
		    context->mod_category = QAPI_NET_SSL_CAT;
		  }
        }
        else if(conn_config->sec_Mode == COAP_CERTIFICATE)
        {
          /* First load root ca or domain issue certificate
             both DI and ROOT CA are mutually exclusive
	     Once loaded check if we have other certificates 
	     associated and then load the same
	  */
          if(ssl_cfg->ca_list != NULL) 
          {
            if(qapi_Net_SSL_Cert_Load(context->sslctx, QAPI_NET_SSL_CA_LIST_E, ssl_cfg->ca_list) < QAPI_OK)
            {
              ds_iot_log_strf_err("COAP: Unable to load %s from FLASH" , ssl_cfg->ca_list);
			  context->last_error = QAPI_ERR_SSL_CERT;
			  context->mod_category = QAPI_NET_SSL_CAT;
              res = QAPI_ERROR;
            }else{
              res = QAPI_OK;
              strlcpy(context->ssl_ca_list, ssl_cfg->ca_list, sizeof(context->ssl_ca_list));
            }
          } 
          else if(ssl_cfg->di_cert != NULL) 
          {
            qapi_Net_SSL_Identifier_t ref_identifier;
            if(qapi_Net_SSL_Cert_Load_Get_Identifier(context->sslctx, &ref_identifier, ssl_cfg->di_cert) < QAPI_OK)
            {
			  context->last_error = QAPI_ERR_SSL_CERT;
			  context->mod_category = QAPI_NET_SSL_CAT;
              res = QAPI_ERROR;
              ds_iot_log_strf_err("COAP: Unable to load DI Certificate %s from FLASH and get Identifier" , ssl_cfg->di_cert);
              goto error_ssl_conn;
            }
            else
            {
              {  
                           
                /* coping the DI certificate name to client context from cfg file*/
                strlcpy(context->ssl_di_cert, ssl_cfg->di_cert , sizeof(context->ssl_di_cert));
                if(resolve_identifier_fqdn(&ref_identifier, callHdl ,dnsInfo.type) == QAPI_OK)
                {
                  cfg->verify.domain = 1;
                  strlcpy(cfg->verify.match_Name, ref_identifier.identifier_Name, strlen(ref_identifier.identifier_Name) + 1); 
                  ds_iot_log_strf_high("COAP: cfg->verify.match_Name(IP non-literal case) : %s", cfg->verify.match_Name);
                }
                else{
                   ds_iot_ulog_err1("COAP: Unable to find and resolve Ref Indentifier from DI Cert %d" ,ref_identifier.identifier_Type);
                   context->last_error = QAPI_NET_SSL_INVALID_HANDLE;
				   context->mod_category = QAPI_NET_SSL_CAT;
				   goto error_ssl_conn;
                }
              }
            }
          }
		  
          if(ssl_cfg->cert != NULL)
          {
            ds_iot_ulog_high1("COAP: ssl_cfg->sec_mode is : %d", conn_config->sec_Mode);
            strlcpy(context->ssl_cert_name, ssl_cfg->cert, sizeof(context->ssl_cert_name));
            res = qapi_Net_SSL_Cert_Load(context->sslctx, QAPI_NET_SSL_CERTIFICATE_E, ssl_cfg->cert);

            if(res < QAPI_OK)
            {
              ds_iot_ulog_err1("COAP: Unable to load from FLASH CERT,closing socket %d" ,sock);
			  context->last_error = QAPI_ERR_SSL_CERT;
			  context->mod_category = QAPI_NET_SSL_CAT;
              goto error_ssl_conn;
            }
          }
        }

      }
      connP->ssl = qapi_Net_SSL_Con_New(context->sslctx, QAPI_NET_SSL_DTLS_E);

      if(connP->ssl == QAPI_NET_SSL_INVALID_HANDLE)
      {
        ds_iot_ulog_err1("COAP:SSL configure failed (Unable to create SSL context),closing socket %d",sock);
		context->last_error = QAPI_NET_SSL_INVALID_HANDLE;
		context->mod_category = QAPI_NET_SSL_CAT;
        goto error_ssl_conn;
      }

	  if(conn_config->disable_close_notify == true)
	  {
	    uint32_t disable_close_notify = 1;
        ds_iot_ulog_high("COAP: disable Close-Notify in SSL");
	    qapi_Net_SSL_Set_Extended_Config_Option(connP->ssl, QAPI_NET_SSL_EXTENDED_CONFIG_DISABLE_CLOSE_NOTIFY, &disable_close_notify, sizeof(disable_close_notify));
	  }


      res = qapi_Net_SSL_Configure(connP->ssl, cfg);
      if(res < QAPI_OK)
      {
        ds_iot_ulog_err1("COAP: SSL configure failed (%d)", res);
		context->last_error = res;
		context->mod_category = QAPI_NET_SSL_CAT;
        qapi_Net_SSL_Shutdown(connP->ssl);
        goto error_ssl_conn;
      }

      if(context->enable_epoch_0 == 1)
      {
        uint32_t enable_epoch_0 = 1;
        ds_iot_ulog_high("COAP: enable epoch 0 case in DTLS");
        qapi_Net_SSL_Set_Extended_Config_Option(connP->ssl, QAPI_NET_SSL_EXTENDED_CONFIG_DTLS_HANDLE_ALERT_WITH_EPOCH_0, &enable_epoch_0, sizeof(enable_epoch_0));
      }

      res = qapi_Net_SSL_Fd_Set(connP->ssl, sock);      

      if(res < QAPI_OK)
      {
        ds_iot_ulog_err1("COAP: qapi_Net_SSL_Fd_Set Failedclosing socket %d",sock);
		context->last_error = res;
		context->mod_category = QAPI_NET_SSL_CAT;
        qapi_Net_SSL_Shutdown(connP->ssl);
        goto error_ssl_conn;
      }
      res = qapi_Net_SSL_Connect(connP->ssl);
      if(res < QAPI_OK)
      {
        if(res == QAPI_SSL_OK_HS)
        {
          ds_iot_ulog_high("COAP: The certificate is trusted");
        }
        else if (res == QAPI_ERR_SSL_CERT_CN)
        {
          /** The peer's SSL certificate is trusted, CN matches the host name, time is expired */
          ds_iot_ulog_err1("COAP: The certificate is expired,closing socket %d",sock);
		  context->last_error = res;
		  context->mod_category = QAPI_NET_SSL_CAT;
          qapi_Net_SSL_Shutdown(connP->ssl);
          goto error_ssl_conn;
        }
        else if (res == QAPI_ERR_SSL_CERT_TIME)
        {
          /** The peer's SSL certificate is trusted, CN does NOT match the host name, time is valid */
          ds_iot_ulog_err1("COAP: The certificate is trusted, but the host name is not valid,closing socket %d",sock);
		  context->last_error = res;
		  context->mod_category = QAPI_NET_SSL_CAT;
          qapi_Net_SSL_Shutdown(connP->ssl);
          goto error_ssl_conn;
        }
        else if (res == QAPI_ERR_SSL_CERT_NONE)
        {
          /** The SSL certificate of the peer is not trusted */
          ds_iot_ulog_err1("COAP: The SSL certificate of the peer is not trusted, closing socket %d",sock);
		  context->last_error = res;
		  context->mod_category = QAPI_NET_SSL_CAT;
          qapi_Net_SSL_Shutdown(connP->ssl);
          goto error_ssl_conn;
        }
        else
        {
          ds_iot_ulog_err2("COAP: SSL connect failed (%d),closing socket %d", res,sock);
		  context->last_error = res;
		  context->mod_category = QAPI_NET_SSL_CAT;
          qapi_Net_SSL_Shutdown(connP->ssl);
          goto error_ssl_conn;
        }
      }
    }
    else
    {
      connP->ssl = QAPI_NET_SSL_INVALID_HANDLE;
      context->ssl_mode = COAP_NO_SEC;
      context->session_resumption_time = session_resumption_time;

	  if(dnsInfo.type != AF_NONIP)
      {
          if(qapi_connect( sock, &connP->addr, connP->addrLen) < 0)        
          {
            ds_iot_ulog_err1("COAP: qapi_connect failed closing socket %d",sock);
            sock_errnum = qapi_errno(sock);
			context->last_error = sock_errnum;
		    context->mod_category = QAPI_NET_SOCKET_CAT;
			qapi_socketclose(sock); 
            free(connP);
            return SOCKET_ERROR;
          }
      }

    }
#endif
    context->sock = sock;

    if(context->conninfo)
    {
       connection_free(context->conninfo);
       ds_iot_ulog_high("Clearing older connInfo" );
       context->conninfo = NULL;
    }

    context->conninfo= connP;
    transaction = context->transactionList;   

    while (NULL != transaction)                                   
    {                                                                           
        transaction->sessionH = connP;
        transaction = transaction->next;
    }

    qurt_signal_set(&coap_sig, 0x01);
  }

  /* The below handle and Proto are used by CoAP
      for NIDD as a transport instead 
  */
  if(conn_config->callHndl)
  {
    connP->callHandle = conn_config->callHndl;
  }
  connP->last_conn_time = (time_t)time_get_secs();
  connP->proto = conn_config->proto;
  return COAP_CONNECT_NO_ERROR;

#ifdef DTLS_SUPPORT
error_ssl_conn:
  qapi_Net_SSL_Obj_Free(context->sslctx);
  qapi_socketclose(sock);
  context->sslctx = QAPI_NET_SSL_INVALID_HANDLE;
  if(context->cfg.sni_Name != NULL)
  {
    free(context->cfg.sni_Name);
    context->cfg.sni_Name = NULL;
    context->cfg.sni_Name_Size = 0;
  }
  free(connP);
  return SSL_ERROR;
#endif

}

/* @fn     void connection_free() 
 * @brief  This function is used to free the connection list
 * @param  connList - pointer to connection list
 * @return void   
 */
void connection_free(connection_t * connList)
{
#ifdef DTLS_SUPPORT
    if(connList->ssl != QAPI_NET_SSL_INVALID_HANDLE)
    {
      qapi_Net_SSL_Shutdown(connList->ssl);
    }
#endif
    //qapi_socketclose(connList->sock);
    free(connList);
}

int coap_reconnect(void *coap_handle, void* session, char  *new_local_ip, int session_resumption_time)
{
  struct sockaddr dst_addr;
  struct sockaddr local_addr;
  char host[48];
  char port[6];
  int ret = QAPI_ERROR;
  qapi_Coap_Connection_Cfg_t  conn_config;
  void * umem = NULL;
  qapi_Coap_Transaction_t * clientTransaction = NULL;

#ifdef DTLS_SUPPORT
  qapi_Coap_Sec_Info_t ssl_cfg = {0};
#endif 
  client_context_t *hndl = (client_context_t*)coap_handle;
  connection_t *conn = (connection_t *)session;

  struct sockaddr_in *sa = NULL;
  struct sockaddr_in6 *sa6 = NULL;
  struct sockaddr_in *sal = NULL;
  struct sockaddr_in6 *sal6 = NULL;

  int  connP = -1;
  connection_t * connPrevious = NULL;

  if( (coap_handle == NULL)  || (session == NULL) || (new_local_ip == NULL))
  {
  	if(coap_handle != NULL)
    {
    	hndl->last_error = QAPI_ERR_COAP_EINVALID_PARAM;
	    hndl->mod_category = QAPI_NET_COAP_CAT;
    }
    return INVALID_PARAMS;
  }
  umem = mem_utility_Get_MemoryPool((void *)hndl->cid, APP_COAP);

  hndl->isResumed = TRUE;
  memset(&local_addr,0,sizeof(local_addr));

  COAP_MEMCPY(&(dst_addr),sizeof (dst_addr) , &(conn->addr), sizeof (conn->addr));

  /* During the IP address change, resumption would be used*/
  coap_close_connection(session, coap_handle, true);

  /* After Closing the connection we need to 
     call transaction callbacks with NULL response to let the Application know that it wont be receiving the 
     reply of these messages as the previous connection is not valid.
     Once the callback is called flush the entries in the previous connection.
  */

    
	clientTransaction = (qapi_Coap_Transaction_t *)mem_utility_Get_Memory_From_Pool(umem, sizeof(qapi_Coap_Transaction_t));

    while (NULL != hndl->transactionList)
    {
      coap_transaction_t * transaction = NULL;
      transaction = hndl->transactionList;

          if(transaction)
          {
             if(clientTransaction)
             {
			   ret = coap_init_client_transaction(&clientTransaction , hndl ,transaction , umem , false, NULL);

               if(ret == QAPI_OK)
               {
                 ds_iot_ulog_high("RECONNECT Calling callbacks before flushing the packets");
                 transaction->callback((void *)hndl->cid, clientTransaction, NULL);
			   }
             }
             else
             {
               ds_iot_ulog_high("COAP : Memory allocation failed");
               hndl->last_error = QAPI_ERR_COAP_ENOMEM;
		       hndl->mod_category = QAPI_NET_COAP_CAT;
			   return INVALID_PARAMS;
             }

             hndl->transactionList = hndl->transactionList->next;  
             transaction->sessionH = NULL;   
             transaction_free(transaction);
             transaction = NULL;
          }
     }
     
     if(clientTransaction)
     {
        mem_utility_release_memory(umem!=NULL, clientTransaction);
     }


  inet_pton(dst_addr.sa_family, new_local_ip, local_addr.sa_data);
  

  if(dst_addr.sa_family == AF_INET)
  {
    sa = (struct sockaddr_in *)&dst_addr;
    inet_ntop(AF_INET, &sa->sin_addr, host, INET4_ADDRSTRLEN);

    local_addr.sa_family = AF_INET;
    sal = (struct sockaddr_in *)&local_addr;
    inet_pton(AF_INET, new_local_ip, &sal->sin_addr);
  }
   if(dst_addr.sa_family == AF_INET6)
  {
    sa6 = (struct sockaddr_in6 *)&dst_addr;
    inet_ntop(AF_INET6, &sa6->sin_addr, host, INET6_ADDRSTRLEN);

    local_addr.sa_family = AF_INET6;
    sal6 = (struct sockaddr_in6 *)&local_addr;
    inet_pton(AF_INET6, new_local_ip, &sal6->sin_addr);
  }

  COAP_MEMCPY(&hndl->client_addr , sizeof(hndl->client_addr),&local_addr,sizeof(local_addr));

  snprintf(port, sizeof(port), "%hd",  htons(dst_addr.sa_port));
#ifdef DTLS_SUPPORT
  if(hndl->ssl_mode == COAP_PSK)
  {
    ssl_cfg.psk = hndl->ssl_psk_name;
  }
  else if(hndl->ssl_mode == COAP_CERTIFICATE)
  {
    ssl_cfg.cert = hndl->ssl_cert_name;
    if(hndl->ssl_ca_list != NULL)
    {
      ssl_cfg.ca_list = hndl->ssl_ca_list;
    }

	#if 0
	ssl_cfg.bIsCoAPUriIPLiteral = hndl->bIsCoAPUriIPLiteral;
	#endif
  }
#endif
  ds_iot_log_strf_high("COAP : Reconnecting to %s : %s mode %d", host, port, hndl->ssl_mode);

  memset(&conn_config,0,sizeof(conn_config));

  connPrevious = (connection_t *)hndl->conninfo;

  conn_config.callHndl = connPrevious->callHandle;
  conn_config.dst_host = host;
  conn_config.dst_port = atoi(port);
  conn_config.family_type = dst_addr.sa_family;
  conn_config.proto = (qapi_Coap_Protocol_Type)connPrevious->proto;
  conn_config.resumption_enabled =  hndl->isResumed;
  conn_config.sec_info = &ssl_cfg;
  conn_config.sec_Mode = (qapi_Sec_Mode)hndl->ssl_mode;
  conn_config.session_resumption_timeout = session_resumption_time;
  conn_config.src_host = new_local_ip;
  conn_config.src_port = 0;
  conn_config.ssl_cfg = &hndl->cfg;

  if(hndl->ssl_mode == COAP_NO_SEC)
  {
    conn_config.ssl_cfg = NULL;
    conn_config.sec_info = NULL;
    connP = connection_create(coap_handle ,&conn_config);
  }
  else
    connP = connection_create(coap_handle, &conn_config);

  return connP;
}

#ifdef DTLS_SUPPORT
int coap_session_resumption(void *coap_handle, void* session, int session_resumption_time)
{
  struct sockaddr dst_addr;
  char host[INET6_ADDRSTRLEN] = {0};
  char port[6];
  char chost[INET6_ADDRSTRLEN] = {0};
  char cport[6];
  struct sockaddr_in *sa = NULL;
  struct sockaddr_in6 *sa6 = NULL;

  qapi_Coap_Sec_Info_t ssl_cfg = {0};
  qapi_Coap_Connection_Cfg_t  conn_config;

  client_context_t *hndl = (client_context_t*)coap_handle;
  connection_t *conn = (connection_t *)session;
  int new_session = -1;

  if( (coap_handle == NULL)  || (session == NULL)) return new_session;

  COAP_MEMCPY(&(dst_addr),sizeof (dst_addr) , &(conn->addr), sizeof (conn->addr));

  ds_iot_ulog_high2("coap_session_resumption: hndl->sslctx %d : conn->ssl%d", hndl->sslctx, conn->ssl);
  ds_iot_ulog_high1("coap_session_resumption: Session Resumption Timeout %d ", session_resumption_time);
  
  snprintf(cport, sizeof(cport), "%hd",  htons(hndl->client_addr.sa_port));

  snprintf(port, sizeof(port), "%hd",  htons(dst_addr.sa_port));

  memset(&conn_config,0,sizeof(conn_config));
  
  if(dst_addr.sa_family == AF_INET)
  {
    sa = (struct sockaddr_in *)&dst_addr;
    inet_ntop(AF_INET, &sa->sin_addr, host, INET4_ADDRSTRLEN);
  }
   if(dst_addr.sa_family == AF_INET6)
  {
    sa6 = (struct sockaddr_in6 *)&dst_addr;
    inet_ntop(AF_INET6, &sa6->sin_addr, host, INET6_ADDRSTRLEN);
  }

  ds_iot_log_strf_high("coap_session_resumption: Desitnation host:%s Port:%s", host, port);

  ds_iot_ulog_high("coap_session_resumption: Calling close connection");

  conn_config.callHndl = conn->callHandle;
  conn_config.proto = (qapi_Coap_Protocol_Type)conn->proto;

  
  coap_close_connection((void *)conn, (void *)hndl, true);
  // Memlead need to fix
  hndl->server_sessionH = NULL;

  if(dst_addr.sa_family == AF_INET)
  {
    sa = (struct sockaddr_in *)&dst_addr;
    inet_ntop(AF_INET, &sa->sin_addr, host, INET4_ADDRSTRLEN);
  }
   if(dst_addr.sa_family == AF_INET6)
  {
    sa6 = (struct sockaddr_in6 *)&dst_addr;
    inet_ntop(AF_INET6, &sa6->sin_addr, host, INET6_ADDRSTRLEN);
  }

  if(hndl->client_addr.sa_family == AF_INET)
  {
    sa = (struct sockaddr_in *)&hndl->client_addr;
    inet_ntop(AF_INET, &sa->sin_addr, chost, INET4_ADDRSTRLEN);
  }
  if(hndl->client_addr.sa_family == AF_INET6)
  {
    sa6 = (struct sockaddr_in6 *)&hndl->client_addr;
    inet_ntop(AF_INET6, &sa6->sin_addr, chost, INET6_ADDRSTRLEN);
  }

  snprintf(cport, sizeof(cport), "%hd",  htons(hndl->client_addr.sa_port));

  snprintf(port, sizeof(port), "%hd",  htons(dst_addr.sa_port));

 
  conn_config.dst_host = host;
  conn_config.dst_port = atoi(port);
  conn_config.family_type = dst_addr.sa_family;
  conn_config.resumption_enabled = hndl->isResumed;
  conn_config.sec_Mode = (qapi_Sec_Mode)hndl->ssl_mode;
  conn_config.session_resumption_timeout = session_resumption_time;
  conn_config.src_host = chost;
  conn_config.src_port = atoi(cport);
  conn_config.disable_close_notify = hndl->disable_close_notify;

  if(hndl->ssl_mode == COAP_PSK)
  {
    ssl_cfg.psk = hndl->ssl_psk_name;
  }
  else if(hndl->ssl_mode == COAP_CERTIFICATE)
  {
    ssl_cfg.cert = hndl->ssl_cert_name;
    if(hndl->ssl_ca_list != NULL)
    {
      ssl_cfg.ca_list = hndl->ssl_ca_list;
    }
#if 0
ssl_cfg.bIsCoAPUriIPLiteral = hndl->bIsCoAPUriIPLiteral;
#endif
  }
  else if(hndl->ssl_mode == COAP_NO_SEC)
  {
     conn_config.ssl_cfg = NULL;
     conn_config.sec_info = NULL;
    new_session = connection_create(coap_handle , &conn_config);
    return new_session;
  }

  conn_config.ssl_cfg = &hndl->cfg;
  conn_config.sec_info = &ssl_cfg;
  
  ds_iot_log_strf_high("coap_session_resumption: Resuming session with Desitnation host:%s Port:%s", host, port);
  new_session = connection_create(coap_handle, &conn_config);
  ds_iot_ulog_high1("coap_session_resumption: Resumption successful New Session:%d", new_session);
  return new_session;
}
#endif

/* @fn     int connection_send()
 * @brief  This function is used to send the buffer to the given connection
 * @param  connP  - pointer to connection 
 *         buffer - pointer to the buffer to send
 *         length - length of buffer 
 * @return on success - 0
 *         on error   - -1
 */
int connection_send(connection_t *connP,
    uint8_t * buffer,
    size_t length)
{
  int nbSent;
  size_t offset;

#ifdef WITH_LOGS
  char s[INET6_ADDRSTRLEN];
  in_port_t port;
//#ifdef WITH_LOGS
  s[0] = 0;

  if (AF_INET == connP->addr.sin6_family)
  {
    struct sockaddr_in *saddr = (struct sockaddr_in *)&connP->addr;
  inet_ntop(saddr->sin_family, &saddr->sin_addr, s, INET6_ADDRSTRLEN);
    port = saddr->sin_port;
  }
  else if (AF_INET6 == connP->addr.sin6_family)
  {
    struct sockaddr_in6 *saddr = (struct sockaddr_in6 *)&connP->addr;
    inet_ntop(saddr->sin6_family, &saddr->sin6_addr, s, INET6_ADDRSTRLEN);
    port = saddr->sin6_port;
  }
//#endif // End of WITH_LOGS
  ds_iot_log_strf_high("COAP: Sending %d bytes to [%s] ", length, s, ntohs(port));
#endif // End of WITH_LOGS

  offset = 0;

#ifdef APPS_PACKET_LOGGING_ENABLE  
  
  ds_iot_ulog_high1("CoAP TX START,bytes[%d]", length);
  coap_dump_packet(buffer, length);
  ds_iot_ulog_high("CoAP TX STOP");
  
            dump_buf_to_dpl(connP->sock, buffer, length, FALSE);
  
#endif

  ds_iot_ulog_high2("COAP: Sending info to port [%d], ip [%u]", ((struct sockaddr_in *)&connP->addr)->sin_port, ((struct sockaddr_in *)&connP->addr)->sin_addr.s_addr);
  
  while (offset != length)
  {
#ifdef DTLS_SUPPORT

    if (connP->ssl != QAPI_NET_SSL_INVALID_HANDLE)
    {
      nbSent = qapi_Net_SSL_Write(connP->ssl, (char *) (buffer + offset), length - offset );
      ds_iot_ulog_high1("COAP: qapi_Net_SSL_Write returns: %d", nbSent);
    }
    else
#endif
    {
      ds_iot_ulog_high("COAP: qapi_sendto()");
      nbSent = qapi_sendto(connP->sock, (char *) (buffer + offset), length - offset, 0, (struct sockaddr *)&(connP->addr), connP->addrLen);
    }
    if (nbSent < QAPI_OK)
    {
      ds_iot_ulog_err1("COAP: : Failed to send [%d]", qapi_errno(connP->sock));
      return -1;
    }
    offset += nbSent;
    ds_iot_ulog_high1("COAP: Sent %d bytes to server", nbSent);
  }
  return 0;
}
/* @fn     uint8_t coap_buffer_send()
 * @brief  This function is used to send the buffer  
 * @param  session    - pointer to connection
 *         buffer     - pointer to buffer to send
 *         length     - length of buffe
 *         userdata   - coap userdata pointer 
 *
 * @return on success - COAP_NO_ERROR
 *         on error   - COAP_500_INTERNAL_SERVER_ERROR 
 */ 

int16_t coap_buffer_send(void * context,
    uint8_t * buffer,
    size_t length,
    void * userdata,
    coap_transaction_t * transacP)
{
   client_context_t* contextP = context;
  connection_t * connP = NULL;
 
  time_t tv_sec = time_get_secs();

  if (contextP == NULL)
  {
    ds_iot_ulog_err1("COAP: failed sending %lu bytes, missing Context", length);
    return COAP_500_INTERNAL_SERVER_ERROR ;
  }

  connP = contextP->conninfo;

  if (connP == NULL)
  {
    ds_iot_ulog_err1("COAP: failed sending %lu bytes, missing connection", length);
    contextP->last_error = QAPI_INTERNAL_SERVER_ERROR_5_00;
	contextP->mod_category = QAPI_NET_COAP_CAT;
	return COAP_500_INTERNAL_SERVER_ERROR ;
  }

  ds_iot_ulog_high1("COAP: Is Resumption Enabled for Client %d ", contextP->isResumed);

   
  /* 
       DTLS resumption would trigger here whenever the time elapsed from last
       recevied packet from the server. last_conn_time stores the time when the last
       recevied packet from the server and compare with configurated session_resumption_time.
       The session_resumption_time should be >= than DEFAULT_COAP_session_resumption_time(60sec minimum)
       to trigger resumption service otherwise,
   */

  if ((connP) && (connP->last_conn_time != 0) && 
                  (contextP->session_resumption_time > 0) && 
                  ((tv_sec - connP->last_conn_time) > (contextP->session_resumption_time)))
                  
  {
  
    int new_targetSessionH = -1;
    if(transacP)
    {
      if(transacP->retrans_counter == 1)
      {
        ds_iot_ulog_high3("coap_buffer_send: calling coap_session_resumption: contextP:%x conn:%x transactionList:%x", 
             contextP, contextP->conninfo, contextP->transactionList->sessionH);

        contextP->isResumed = TRUE;
        new_targetSessionH = coap_session_resumption(contextP, (void *)connP, contextP->session_resumption_time);
      }
      else
      {
         goto connection_send;
      }
    }
    else
    {
      contextP->isResumed = TRUE;
      ds_iot_ulog_high2("coap_buffer_send: calling coap_session_resumption: contextP:%x conn:%x", 
             connP, contextP->conninfo);
      new_targetSessionH = coap_session_resumption(contextP, (void *)connP, contextP->session_resumption_time);
    }
    if(new_targetSessionH == 0)
    {
      /* Reassigning the new session to the server assiciated with Coap handle */

      if(transacP)
      {
        transacP->sessionH = contextP->conninfo;
      }
      connP = contextP->conninfo;
      ds_iot_ulog_high1("Coap_resumption successful: new_targetSessionH: %x", contextP->conninfo);
    } 
    else
    {
      /* Resumption failed due to genuine connection error from server side return error to caller*/

      ds_iot_ulog_high("coap_resumption failed !!");
      contextP->last_error = QAPI_ERR_COAP_DTLS_RESUMPTION_FAILED_IN_COAP_SEND;
	  contextP->mod_category = QAPI_NET_COAP_CAT;
	  return COAP_DTLS_RESUMPTION_FAILED;
    }

 }

connection_send:

  connP->last_conn_time = (time_t)tv_sec;
 
  if (-1 == connection_send(contextP->conninfo, buffer, length))
  {
    ds_iot_ulog_err1("COAP: failed sending %lu bytes", length);
    contextP->last_error = QAPI_INTERNAL_SERVER_ERROR_5_00;
	contextP->mod_category = QAPI_NET_COAP_CAT;
	return COAP_500_INTERNAL_SERVER_ERROR ;
  }

  return COAP_NO_ERROR;
}
/* @fn     bool coap_session_is_equal()
 * @brief  This function is used to compare the sessions
 * @param  session1  
 session2 session pointers
 *         userdata   - void 
 * @return on success - 1
 *         on faliure - 0 
 */         
bool coap_session_is_equal(void * session1,
    void * session2,
    void * userData)
{
  return (session1 == session2);
}
