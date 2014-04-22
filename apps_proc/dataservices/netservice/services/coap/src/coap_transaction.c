/******************************************************************************

  @file    coap_transaction.c

  ---------------------------------------------------------------------------

  Copyright (c) 2016-2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------

 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <time_svc.h>
#include "rcinit.h"

#include <stringl/stringl.h>
#include "qapi_socket.h"

#include "qurt/qurt_thread.h"
#include "qurt/qurt_mutex.h"
#include "qurt/qurt_error.h"

#include <errno.h>
#include "qurt.h"
#include "er-coap-13.h"
#include "app_coap.h"
#include "connection.h"
#include "coap-internal.h"
#include "qapi_coap.h"
#include "net_utils_ext.h"

#ifndef MOB_TX
#include "qurt_txm_qapi_fwk.h"
#else
#include "offtarget_stubs.h"
#endif

#ifdef APPS_PACKET_LOGGING_ENABLE  
extern void dump_buf_to_dpl(int handle, uint8_t * dump_buf, int buf_len, bool recvd);
#endif

bool transaction_find(void * context,void * fromSessionH, coap_packet_t * dl_pkt , coap_transaction_t ** newtransacP);

#define MAX_PACKET_SIZE 1024 
#define CoAP_MAX_HEADER_LENGTH 128
#define MIN_ACK_TIMEOUT 1           /* CoAP RFC7252 recommended Min Response timeout in seconds */

#define CoAP_MAX_MESSAGE_SIZE (MAX_PACKET_SIZE + CoAP_MAX_HEADER_LENGTH)  /* CoAP RFC7252 recommended Message Size */

qurt_signal_t coap_sig;

qurt_thread_t coap_thread_id = 0;
qurt_thread_t get_coap_thread_id(void);

extern uint64 time_get_ms_native( void );

extern uint32_t generate_rand32(void);

extern coap_status_t coap_error_code;

static uint32_t generate_coap_handle_id(void) ;

//! Mutex used to make sure context is not being used while closing a certain client
qurt_mutex_t * gCoapMutex;

client_context_t * g_client_ctxt = NULL ;

static fd_set readfds;    
static int g_check;  

#define IS_RESPONSE_MSG 1        //coap response message
#define IS_NOT_RESPONSE_MSG 0    //coap requests message


/*

   This function is used to find whether transaction for the incoming downlink packet 
   present or not in the transaction list

*/
bool transaction_find(void * context,void * fromSessionH, coap_packet_t * dl_pkt , coap_transaction_t ** newtransacP)
{
  bool found = false;
  client_context_t * contextP = context;
  coap_transaction_t * transacP = NULL;

  if(contextP == NULL || dl_pkt == NULL)
  {
    return false;
  }

  ds_iot_ulog_high("transaction_find : Entering");
  transacP = contextP->transactionList;


  while (NULL != transacP)
  {
    void * targetSessionH;

    targetSessionH = transacP->sessionH;

    if (coap_session_is_equal(fromSessionH, targetSessionH, contextP->clientData) == true)
    {
      if (!transacP->ack_received)
      {
        if ((COAP_TYPE_ACK == dl_pkt->type) || (COAP_TYPE_RST == dl_pkt->type))
        {
          if (transacP->mID == dl_pkt->mid)
          {
            found = true;
			ds_iot_ulog_high("found transaction for ACK or RST");
            *newtransacP = transacP;
			 break;
          }
        }
      }

      if (prv_checkFinished(transacP, dl_pkt) == 1)
      {
         found = true;
		 ds_iot_ulog_high("found transaction for a token match");
         *newtransacP = transacP;
	     break;
      }
    }

	transacP = transacP->next;
 }    

 return found;
}



/*
This is an Internal Utility to calculate Exhange Lifetime from
various config params
*/
uint32_t calculate_exchange_lifetime(client_context_t *client_ctxt)
{
   double   coap_max_transmit_span = 0.0;
   uint32_t coap_exchange_lifetime = COAP_EXCHANGE_LIFETIME;
   uint32_t response_timeout = 0;
   uint32_t  max_retransmit = 0;
   double    r_factor = 0.0;
   uint32_t  max_latency = 0;

   if(client_ctxt == NULL)
   return COAP_EXCHANGE_LIFETIME;

   else
   {
      response_timeout = client_ctxt->coap_transaction_timeout;
      max_retransmit = client_ctxt->coap_max_retransmits;
      r_factor = client_ctxt->coap_ack_random_factor;
      max_latency = client_ctxt->coap_max_latency;

     coap_max_transmit_span = (response_timeout * ( (1 << max_retransmit) - 1) * r_factor);
     client_ctxt->coap_max_transmit_span = (uint32_t)coap_max_transmit_span;
     client_ctxt->blockwise_max_age = client_ctxt->coap_max_transmit_span;
     if(client_ctxt->blockwise_max_age == 0)
        client_ctxt->blockwise_max_age = COAP_MAX_TRANSMIT_SPAN;

     ds_iot_ulog_high1("blockwise_max_age %lu",client_ctxt->blockwise_max_age);
     coap_exchange_lifetime =  (coap_max_transmit_span + (2 * max_latency) + response_timeout);
     return coap_exchange_lifetime;
   }

}

/* @fn     void * coap_register_client()
 * @brief  This function is used to create coap client context  and create socket for the client 
 * @param  ip - ip address 
 *         localport - port number
 *         addressFamily - adress family type
 *         lwm2mH - handle to lwm2m context
 *         app_cb - function pointer to lwm2m call back 
 * @return void * client_ctxt pointer
 */ 

void * coap_register_client(qapi_Coap_Session_Info_t * coap_session_config)
{
  client_context_t *client_ctxt = NULL;

   if (coap_session_config == NULL ) return NULL;

  client_ctxt = (client_context_t *)malloc(sizeof(client_context_t));

   if(!client_ctxt) return NULL;
  	
   memset (client_ctxt , 0 , sizeof ( client_context_t));

    client_ctxt->client_handle_request_cb = coap_session_config->cb;
    client_ctxt->clientData = coap_session_config->clientData;


    client_ctxt->coap_max_retransmits = coap_session_config->coap_max_retransmits;
    client_ctxt->coap_transaction_timeout = coap_session_config->coap_transaction_timeout;
	client_ctxt->last_error = QAPI_OK;
	client_ctxt->mod_category = QAPI_NET_NO_MODULE_CAT;
    if(client_ctxt->coap_transaction_timeout < MIN_ACK_TIMEOUT)
    {
       ds_iot_ulog_high("CoAP : Invalid ACK TIMEOUT Passed");
       free(client_ctxt);
       return NULL;
    }

    client_ctxt->coap_ack_random_factor = coap_session_config->coap_ack_random_factor;
    client_ctxt->coap_max_latency = coap_session_config->coap_max_latency;

    client_ctxt->coap_exchange_lifetime = calculate_exchange_lifetime(client_ctxt);

    if(data_is_user_space_module() && data_get_module_info(&client_ctxt->module_uid, &client_ctxt->module_instance))
    {
       ds_iot_ulog_high("Retrieving module id or module instance failed");
       free(client_ctxt);
       return NULL;
    }
	
    client_ctxt->cid =generate_coap_handle_id();

    ds_iot_ulog_high1("COAP REGISTER CLIENT ex value %lu",client_ctxt->coap_exchange_lifetime);

 
    client_ctxt->CoapClientMutex = (qurt_mutex_t *)malloc(sizeof(qurt_mutex_t));

    if(client_ctxt->CoapClientMutex == NULL)
    {
       ds_iot_ulog_err("CoAP : Error allocating memory to CoapClientMutex.");
       free(client_ctxt);
       return NULL;
    }

    qurt_mutex_init(client_ctxt->CoapClientMutex);

    qurt_mutex_lock(gCoapMutex);
  if (NULL != g_client_ctxt)
  {
    client_ctxt->next = g_client_ctxt;
    g_client_ctxt = client_ctxt;    
  }
  else
  {
    g_client_ctxt = client_ctxt;
  }

  qurt_mutex_unlock(gCoapMutex);
  return client_ctxt;

}

void coap_init(void)
{
  qurt_signal_init(&coap_sig);
  qurt_signal_clear(&coap_sig, 0x01);
}

/* fn     void *coap_thread() 
 * brief  This thread will handle receiving of packets from servers 
 * param  void *p - NULL 
 * return void 
 */ 

void coap_thread(void *p) 
{
  int result = 0;   
  client_context_t *client_ctxt = NULL;
  int sock_err = 0;
  g_check = 0;

  rcinit_handshake_startup();

  gCoapMutex = (qurt_mutex_t *)malloc(sizeof(qurt_mutex_t));

#ifndef MOB_TX
  qurt_qapi_register(TXM_QAPI_COAP_BASE + 1, TXM_DATA_COAP_NUM_IDS , qapi_Coap_Handler);
  qurt_qapi_register(TXM_QAPI_COAP_BASE2 + 1, TXM_DATA_COAP_NUM_IDS_2 , qapi_Coap_Handler);
#endif

  qurt_mutex_init(gCoapMutex);
  
  coap_thread_id = qurt_thread_get_id();

  while (g_check == 0) 
  {
    time_t tv_currtime_ms;
    time_t tv_transaction_timeout_ms = (COAP_TIMEOUT_SEC *1000);
    int fd_count = 0;

    tv_currtime_ms = (time_t)time_get_ms_native();

    FD_ZERO(&readfds);
    COAP_LOCK_MUTEX(gCoapMutex);
    if (NULL != g_client_ctxt)
    {
      client_ctxt = g_client_ctxt;
      while ( client_ctxt)
      {
        if(-1 != client_ctxt->sock)
          fd_count++;
        client_ctxt = client_ctxt->next;
      }
    }
    COAP_UNLOCK_MUTEX(gCoapMutex);
    if(fd_count == 0)
    {
      ds_iot_ulog_high("CoAP Blocked because no connection in the list yet");
      qurt_signal_wait(&coap_sig, 0x01, QURT_SIGNAL_ATTR_CLEAR_MASK | QURT_SIGNAL_ATTR_WAIT_ANY);
      ds_iot_ulog_high("CoAP Unblocked");
    }
    COAP_LOCK_MUTEX(gCoapMutex);
    client_ctxt = g_client_ctxt;
    while ( client_ctxt) 
    {
      if((-1 != client_ctxt->sock) && (client_ctxt->sock_error == FALSE))
      {   
		COAP_LOCK_MUTEX(client_ctxt->CoapClientMutex);
        FD_SET(client_ctxt->sock, &readfds);
        transaction_step(client_ctxt, tv_currtime_ms,&(tv_transaction_timeout_ms));
        block_step(client_ctxt);
        COAP_UNLOCK_MUTEX(client_ctxt->CoapClientMutex);
      }      
      client_ctxt = client_ctxt->next;
    }
    COAP_UNLOCK_MUTEX(gCoapMutex);
    
    result = qapi_select(&readfds, NULL, NULL, 500);

    if (result < 0)
    {
      ds_iot_ulog_high1("coap: qapi_select(): %d", result);

      COAP_LOCK_MUTEX(gCoapMutex);
      client_ctxt = g_client_ctxt;
      while (client_ctxt) 
      {
        if(client_ctxt->isResumed == TRUE)
        {
          client_ctxt->isResumed = FALSE;
          ds_iot_ulog_high2("coap: isResumed:%d for cli: %u",client_ctxt->isResumed,client_ctxt);
        }
        client_ctxt= client_ctxt->next;
      }
      COAP_UNLOCK_MUTEX(gCoapMutex);

    }
    else if (result > 0)
    {
      static uint8_t buffer[CoAP_MAX_MESSAGE_SIZE];
      int numBytes;
      memset(buffer, 0x00, CoAP_MAX_MESSAGE_SIZE);
      /*                                  
       * If an event happens on the socket
       */
      COAP_LOCK_MUTEX(gCoapMutex);
      client_ctxt = g_client_ctxt;

      while (client_ctxt)
      {
        if ((client_ctxt->sock != -1) && (client_ctxt->sock_error == FALSE) && 
            (FD_ISSET(client_ctxt->sock, &readfds)))
        {
          if(client_ctxt->isResumed != TRUE)
          {
            struct sockaddr addr_str;
            uint32_t addrLen;

            addrLen = sizeof(addr_str);

            /* Check and handle Socket error */

            sock_err = qapi_errno(client_ctxt->sock);

            if(sock_err == ECONNABORTED || sock_err == ECONNREFUSED || sock_err == ECONNRESET || sock_err == EPIPE || sock_err == ENOTSOCK)
			{
               int ret = QAPI_ERROR;

               ret = Callback_Invoke_If_Coap_Event((void *)client_ctxt , NULL , QAPI_COAP_EVENT_SSL_ALERT_FATAL , client_ctxt->clientData);

	           if(ret == QAPI_OK)
	             ds_iot_ulog_high("Callback_Invoke_If_Coap_Event returned success");

               FD_CLR(client_ctxt->sock, &readfds);
               client_ctxt->sock_error = TRUE;
			   goto next;
            }

            /*                              
             * We retrieve the data received
             */   
           ds_iot_ulog_high2("coap: isResumed:%d for cli: %u",client_ctxt->isResumed,client_ctxt);
  #if DTLS_SUPPORT
            numBytes = qapi_recvfrom(client_ctxt->sock, (char *)buffer, CoAP_MAX_MESSAGE_SIZE, 
                                            MSG_PEEK, (struct sockaddr *)&addr_str, (int32_t *)&addrLen);
            ds_iot_ulog_high1("coap: numBytes recieved %d",numBytes);
  #else
            numBytes = qapi_recvfrom(client_ctxt->sock, (char *)buffer, CoAP_MAX_MESSAGE_SIZE, 
                                            0, (struct sockaddr *)&addr_str, (int32_t *)&addrLen);
  #endif
            if (0 < numBytes)
            {
              connection_t * connP;
  #ifdef DEBUG
              char s[INET6_ADDRSTRLEN];
              in_port_t port; 
              if (AF_INET == addr_str.ss_family)
              {         
                struct sockaddr_in *saddr = (struct sockaddr_in*)&addr_str;
                inet_ntop(saddr->sin_family, &saddr->sin_addr, s,
                    INET6_ADDRSTRLEN);
                port = saddr->sin_port; 
              } 
              else if (AF_INET6 == addr_str.ss_family)
              {
                struct sockaddr_in6 *saddr = (struct sockaddr_in6
                    *)&addr_str;
                inet_ntop(saddr->sin6_family, &saddr->sin6_addr, s,
                    INET6_ADDRSTRLEN);
                port = saddr->sin6_port;
              }
              ds_iot_log_strf_high("coap: %d bytes received from [%s]:%hu",
                  numBytes, s, ntohs(port));
  #endif
  
  
#ifdef APPS_PACKET_LOGGING_ENABLE  
              /*                                                          
               * Dump received packets                                 
               */                                                              
              ds_iot_ulog_high1("CoAP RX START,bytes[%d]", numBytes);
              coap_dump_packet(buffer, numBytes);
              
              dump_buf_to_dpl(client_ctxt->sock, buffer, numBytes, TRUE);
#endif
              connP = client_ctxt->conninfo;
            
              if (connP == NULL) 
                 ds_iot_ulog_err("COAP: not able to find the connection");
              if (connP != NULL)                                          
              { 
                /*                                                      
                 * Let liblwm2m respond to the query depending on the
                 * context
                 */
  #ifdef DTLS_SUPPORT
                if(connP->ssl)
                {
                  result = qapi_Net_SSL_Read(connP->ssl, buffer, numBytes);
                  ds_iot_ulog_high1("COAP:ssl read returns: %d",result);
                }
                else
                {
                  numBytes = qapi_recvfrom(client_ctxt->sock, (char *)buffer, CoAP_MAX_MESSAGE_SIZE, 
                                 0, (struct sockaddr *)&addr_str, (int32_t *)&addrLen);
                }
                /*last_conn_time is get updated whenever new downlink packet received from that connection*/
                ds_iot_ulog_high("COAP:UPdated last packet time");
                connP->last_conn_time = (time_t)time_get_secs();
  #endif          
                COAP_LOCK_MUTEX(client_ctxt->CoapClientMutex);
                coap_handle_packet(client_ctxt, buffer, (unsigned int)numBytes, connP);
                COAP_UNLOCK_MUTEX(client_ctxt->CoapClientMutex);
              } 
              else
              {
                numBytes = qapi_recvfrom(client_ctxt->sock, (char *)buffer, CoAP_MAX_MESSAGE_SIZE, 
                                 0, (struct sockaddr *)&addr_str, (int32_t *)&addrLen);
                ds_iot_ulog_high("COAP: not able to find the connection");
                break;
              }
            }  
             else if (numBytes == 0) 
            { 
              ds_iot_ulog_high("Recvd a Zero length UDP packet dropping it!!"); 
#ifdef DTLS_SUPPORT
              qapi_recvfrom(client_ctxt->sock, (char *)buffer, CoAP_MAX_MESSAGE_SIZE, 0, 
                                                    (struct sockaddr *)&addr_str, (int32_t *)&addrLen); 
#endif
            } 
            else if (0 > numBytes)
            {
              ds_iot_ulog_high1("There seems to be an error : %d on the socket; Marking it as invalid", 
                          qapi_errno(client_ctxt->sock));
              client_ctxt->sock_error = TRUE;
            }
  
          }
          else
          {
            client_ctxt->isResumed = FALSE;
            ds_iot_ulog_high2("coap: isResumed:%d for cli: %u",client_ctxt->isResumed,client_ctxt);
          }
        }
next:
        client_ctxt = client_ctxt->next; 
      }
      COAP_UNLOCK_MUTEX(gCoapMutex);

    }
    else
    {
      ds_iot_ulog_high1("coap: qapi_select(): %d", result);
    }
  }
} 


/* fn     void coap_close() 
 * brief  This function is used free the transaction list and connection list
 * param  coapHandle - handle of coap context
 * return void
 */

void coap_close(void *coapHandle) 
{
  client_context_t* client_ctxt  = NULL;         
  client_context_t* contextP = g_client_ctxt;
  client_ctxt = (client_context_t*)coapHandle;

  if(client_ctxt == NULL)
  return;

  COAP_LOCK_MUTEX(gCoapMutex);
  ds_iot_ulog_high1("coap_close() %p", coapHandle);

  COAP_LOCK_MUTEX(client_ctxt->CoapClientMutex);
  Callback_Invoke_If_Send_Failure(client_ctxt, client_ctxt->transactionList);
  while (NULL != client_ctxt->transactionList)                                   
  {                                                                           
    coap_transaction_t * transaction;                                      

    transaction = client_ctxt->transactionList;                                
    client_ctxt->transactionList = client_ctxt->transactionList->next;  
    transaction->sessionH = NULL;   
    transaction_free(transaction);
    transaction = NULL;
  }      
  if ( contextP == client_ctxt) {
    g_client_ctxt = client_ctxt->next;
  } else {
    while ( contextP != NULL && contextP->next != client_ctxt ) {
      contextP = contextP-> next;
    }
    if (contextP) {
      contextP->next = client_ctxt->next;
    }
  }

  if(client_ctxt->conninfo)
  {
    connection_free(client_ctxt->conninfo);
     client_ctxt->conninfo = NULL;
  }
  if(client_ctxt->sslctx != QAPI_NET_SSL_INVALID_HANDLE)
  {
    qapi_Net_SSL_Obj_Free(client_ctxt->sslctx);
    ds_iot_ulog_high("coap_close() : Marking ssl_ctxt as invalid");
    client_ctxt->sslctx = QAPI_NET_SSL_INVALID_HANDLE;
  }

  if(client_ctxt->cfg.sni_Name != NULL)
  {
    free(client_ctxt->cfg.sni_Name);
    client_ctxt->cfg.sni_Name = NULL;
  }

  if(client_ctxt->sock != -1) 
  {
     qapi_socketclose(client_ctxt->sock);
     client_ctxt->sock = -1;
     client_ctxt->sock_error  = FALSE;
     ds_iot_ulog_high("coap_close() : Marked the socket as Invalid \n");
  }
  COAP_UNLOCK_MUTEX(client_ctxt->CoapClientMutex);
  qurt_mutex_destroy(client_ctxt->CoapClientMutex);
  free(client_ctxt->CoapClientMutex);
  free(client_ctxt);

    
    COAP_UNLOCK_MUTEX(gCoapMutex);
}  

/* @fn     void coap_close_connection()   
 * @brief  This function is used to close particular  connection  from connection list
 * @param  userdata - Hanlde to coap client context
 *         session -  connection_t pointer
 * @param  sessionResumption - Hanlde resumption whenever the context need to be reused
 * @return void 
 */ 
void coap_close_connection(void *sessionH ,                                     
    void * coap_handle, bool sessionResumption)                                    
{                                                                               
  client_context_t * app_data;                                               
  connection_t * targetP;                                                     
  app_data = ( client_context_t *) coap_handle;                                
  targetP = (connection_t *)sessionH;                                         
  ds_iot_ulog_high("coap_close_connection()");
  if (targetP && app_data) 
  {
#ifdef DTLS_SUPPORT
    if(targetP->ssl != QAPI_NET_SSL_INVALID_HANDLE)
    {
      qapi_Net_SSL_Shutdown(targetP->ssl);
    }
    if((sessionResumption == false) && (app_data->sslctx != QAPI_NET_SSL_INVALID_HANDLE))
    {
      qapi_Net_SSL_Obj_Free(app_data->sslctx);
      app_data->sslctx = QAPI_NET_SSL_INVALID_HANDLE;
    }
#endif    
    ds_iot_ulog_high1("coap_close_connection : session to delete %x", targetP);
    if(app_data->cfg.sni_Name != NULL)
    {
      free(app_data->cfg.sni_Name);
      app_data->cfg.sni_Name = NULL;
      app_data->cfg.sni_Name_Size = 0;
    }
  }
  if(app_data)
  {  
    ds_iot_ulog_high1("coap: Closing the socket with fd %d",app_data->sock);    
    if(app_data->sock != -1) {
      qapi_socketclose(app_data->sock);
      app_data->sock = -1;
      app_data->sock_error  = FALSE;
      ds_iot_ulog_high("coap: Marked the socket as Invalid");    
    }
  }
  else
  {
    ds_iot_ulog_err("coap: CoAP APP data NULL");   
  }
  return;
}          


/* @fn      void coap_handle_packet()   
 * @brief   This function is used to handle the coap packets 
 * @param   ctxt   - coap client context handle
 *          buffer - pointer to buffer received
 *          length - lenght of packet 
 fromSessionH - pointer to connection
 * @return  void
 */    
void coap_handle_packet(void * ctxt,uint8_t * buffer,unsigned int length,void * fromSessionH)
{

  client_context_t * client_contextP = (client_context_t *) ctxt;
  coap_status_t coap_error_code = NO_ERROR;
  void * umem = NULL;
  uint8_t * ubuffer =NULL;
  coap_packet_ext_t *dl_pkt = NULL;
  coap_packet_ext_t * response = NULL;
  bool transaction_found = false;
  coap_packet_t * transact_pkt = NULL;
  int coap_uplink_pkt = IS_NOT_RESPONSE_MSG;
  qapi_Status_t ret_status = QAPI_ERROR;

  ds_iot_ulog_high("COAP_HANDLE_PACKET");

  if(client_contextP == NULL)
    return;

  umem = mem_utility_Get_MemoryPool((void *)client_contextP->cid, APP_COAP);

  dl_pkt = (coap_packet_ext_t *)mem_utility_Get_Memory_From_Pool(umem, sizeof(coap_packet_ext_t));

  if (dl_pkt == NULL)
  {
    ds_iot_ulog_high("COAP : Memory allocation failed USpace");
    return;
  }

  memset(dl_pkt,0,sizeof(coap_packet_ext_t));

  if(umem)
  {
    ubuffer = mem_utility_Get_Memory_From_Pool(umem , length);

	if (ubuffer == NULL)
    {
      ds_iot_ulog_high("COAP : Memory allocation failed USpace");
      mem_utility_release_memory(umem!=NULL, dl_pkt);
      return;
    }

    memset(ubuffer,0,length);
    memscpy(ubuffer,CoAP_MAX_MESSAGE_SIZE,buffer,length);
  }
  else
   ubuffer = buffer;

  if(ubuffer == NULL)
  {
    ds_iot_ulog_high("coap_handle_packet : ubuffer is NULL");
	mem_utility_release_memory(umem!=NULL, dl_pkt);
    return;
  }

  coap_error_code = coap_parse_message((coap_packet_ext_t *)dl_pkt, ubuffer, (uint16_t)length, umem);

  if (coap_error_code == NO_ERROR)
  {
    coap_transaction_t * newtransacP = NULL;
    ds_iot_ulog_high6("Parsed: ver %u, type %u, tkl %u, code %u.%.2u, mid %u",
    dl_pkt->coap_pkt.version, dl_pkt->coap_pkt.type, dl_pkt->coap_pkt.token_len,dl_pkt->coap_pkt.code >> 5, dl_pkt->coap_pkt.code & 0x1F, dl_pkt->coap_pkt.mid);
    ds_iot_log_strf_high("Contenttype: %d Payload: %.*s", dl_pkt->coap_pkt.content_type ,dl_pkt->coap_pkt.payload_len, dl_pkt->coap_pkt.payload);

    if(ubuffer && umem)
    {
      tx_byte_release(ubuffer);
	  ubuffer = NULL;
    }

    dl_pkt->coap_pkt.buffer = NULL;

    /* Find coap pakcet transaction present or not */
    transaction_found = transaction_find(client_contextP, fromSessionH, (coap_packet_t *)&dl_pkt->coap_pkt, &newtransacP);

    if(transaction_found && newtransacP)  /* if transaction found processs the packet here */
    {

	   transact_pkt = (coap_packet_t *)newtransacP->message;

	   if(transact_pkt && ( transact_pkt->code >= COAP_GET && transact_pkt->code <= COAP_DELETE ))
	   {
          coap_uplink_pkt = IS_NOT_RESPONSE_MSG;    /* check if Uplink message is a Requests packet */
	   }
	   else if(transact_pkt && ( transact_pkt->code >= CREATED_2_01 && transact_pkt->code<= PROXYING_NOT_SUPPORTED_5_05 ))
	   {
          coap_uplink_pkt =IS_RESPONSE_MSG;    /* check if Uplink message is a Response packet */
 	   }


       switch(dl_pkt->coap_pkt.type)
       {
         
          case COAP_TYPE_NON:

              if(dl_pkt->coap_pkt.code == COAP_EMPTY)
              {
                 ds_iot_ulog_high("COAP:TF Empty message with NON CON is invalid");
                 goto end;
              }
              else
              {
                 transaction_handleResponse(client_contextP, fromSessionH,(coap_packet_t *)&dl_pkt->coap_pkt, NULL,umem);
              }
           break;
             
 
           case COAP_TYPE_CON: 
                                                             //block2 UL/DL case CON-CONTENT
              if(newtransacP->empty_ack_received == TRUE || QAPI_IS_OPTION(&(dl_pkt->coap_pkt), QAPI_COAP_OPTION_BLOCK2)) 
              {
                response = (coap_packet_ext_t *)mem_utility_Get_Memory_From_Pool(umem, sizeof(coap_packet_ext_t));

                if(!response)
                {
                  goto end;
                }

                 memset(response,0,sizeof(coap_packet_ext_t));
                 transaction_handleResponse(client_contextP, fromSessionH,(coap_packet_t *)&dl_pkt->coap_pkt, &response->coap_pkt,umem);
              }
              else
              {
                 ds_iot_ulog_high1("CON Message Recieved before getting ACK with MID : %u",dl_pkt->coap_pkt.mid);
                 goto end;
              }

           break;
              
           case COAP_TYPE_ACK:

              /* Handle First empty ACK for uplink CON requests */
		  
              if(dl_pkt->coap_pkt.code == COAP_EMPTY && newtransacP->empty_ack_received == FALSE && (transact_pkt && (coap_uplink_pkt == IS_NOT_RESPONSE_MSG)))
              {
                 int l_time = 0;
                 ds_iot_ulog_high("COAP:TF Got an Empty ACK , Response pending");

                 l_time = client_contextP->coap_exchange_lifetime;
                 newtransacP->retrans_counter = client_contextP->coap_max_retransmits + 2;
                 newtransacP->empty_ack_received = TRUE;
                 newtransacP->retrans_time += l_time * 1000;
				 goto end;
              }
			  /* Handle First empty ACK for uplink CON response  or Non empty ACK for a coap requests */
			  else if(dl_pkt->coap_pkt.code == COAP_EMPTY && (transact_pkt && (coap_uplink_pkt == IS_RESPONSE_MSG)) || (newtransacP->empty_ack_received == FALSE))
                {
                 transaction_handleResponse( client_contextP, fromSessionH,(coap_packet_t *)&dl_pkt->coap_pkt, NULL,umem);
                }
			  else
			  { 
                 ds_iot_ulog_high("COAP:TF Got an Empty ACK already , Response pending");
				 goto end;
             }

        break;

        case COAP_TYPE_RST:

        ds_iot_ulog_high("COAP:RST MESSAGE RECEIVED");


              //Call transaction callback to handle RST for this transaction
              transaction_handleResponse(client_contextP, fromSessionH,(coap_packet_t *)&dl_pkt->coap_pkt, NULL,umem);
	   
        break;

         }
	
	}
	else /*Process coap packet if not found in TRANSACTION list */
	{
	  coap_block_session_info_t * block_info = get_block_info_from_pkt(client_contextP , &dl_pkt->coap_pkt);
      blockWise_handling_status_t blk_status = BLOCKWISE_TRANSFER_REQUEST_FALSE;
      switch(dl_pkt->coap_pkt.type)
      {

		 case COAP_TYPE_NON:

		    if(dl_pkt->coap_pkt.code == COAP_EMPTY)
            {
               ds_iot_ulog_high("COAP: Empty message with NON CON is invalid");
               goto end;
            }
            else
            {
               if (client_contextP->client_handle_request_cb)
               {
                  coap_error_code = (coap_status_t)client_contextP->client_handle_request_cb(
                                                (qapi_Coap_Session_Hdl_t)client_contextP->cid,
                                                (qapi_Coap_Packet_t *)dl_pkt, client_contextP->clientData);
                  ds_iot_ulog_high1("CoAP: Client DL Request Callback Returned with Status %d", coap_error_code);
               }
               else
               {
                  ds_iot_ulog_high("COAP: DL request call back is NULL");
                  goto end;
               }
            }

          break ;
          

		 case COAP_TYPE_CON:

				response = (coap_packet_ext_t *)mem_utility_Get_Memory_From_Pool(umem, sizeof(coap_packet_ext_t));

                if(!response)
                {
                  goto end;
                }
                      
                memset(response,0,sizeof(coap_packet_ext_t));

                 if (dl_pkt->coap_pkt.code == COAP_EMPTY)
                 {
                   ds_iot_ulog_high("Received CON request");  
                   coap_init_message(response, COAP_TYPE_ACK, 0,dl_pkt->coap_pkt.mid);
                   coap_error_code = message_send(client_contextP,&response->coap_pkt,fromSessionH,NULL);
                 }
                 else 
                 {
                    blk_status = get_blockWise_handling_status(client_contextP, &dl_pkt->coap_pkt , false);
                    ds_iot_ulog_high1("Block: Blockwise handling status[%d]", blk_status);
                    if(blk_status == BLOCKWISE_COAP_HANDLE_REQUEST)
                    {
                        ret_status = process_blockwise_transfer(client_contextP, NULL,(qapi_Coap_Packet_t *)&dl_pkt->coap_pkt);
                        if(ret_status == QAPI_COAP_IGNORE)
                        {
                            break;
                        }
                        else if(ret_status == QAPI_REQ_ENTITY_INCOMPLETE_4_08 || ret_status == QAPI_REQUEST_ENTITY_TOO_LARGE_4_13 || ret_status == QAPI_INTERNAL_SERVER_ERROR_5_00 )
                        {
                           coap_init_message(response, COAP_TYPE_ACK, ret_status, dl_pkt->coap_pkt.mid);
                           message_send(client_contextP, &response->coap_pkt, fromSessionH, NULL);
                           if(block_info == NULL && client_contextP != NULL )
                              block_info = client_contextP->bl_info_param[DL_REQ]; 
                           clean_block_info(client_contextP, &block_info);
                           break;
                        }
                    }

                    if (client_contextP->client_handle_request_cb)
                    {
                       coap_error_code = (coap_status_t)client_contextP->client_handle_request_cb(
                                                 (qapi_Coap_Session_Hdl_t)client_contextP->cid,
                                                 (qapi_Coap_Packet_t *)dl_pkt, client_contextP->clientData);
                      
                       ds_iot_ulog_high1("Client DL Request Callback Returned with Status %d", coap_error_code);
                    }
                    else
                    {
	               ds_iot_ulog_high("COAP: DL request call back is NULL");
                       if (dl_pkt)
                       {  
                          qapi_Coap_Free_Message((qapi_Coap_Session_Hdl_t)client_contextP->cid,
                                                 (qapi_Coap_Packet_t *)dl_pkt);
                          dl_pkt = NULL;
                       }
                    }

                    if(block_info == NULL && client_contextP != NULL )
                       block_info = client_contextP->bl_info_param[DL_REQ]; 
                   if(block_info && blk_status != BLOCKWISE_APPLICATION_HANDLE_REQUEST)
                   {
                      clean_block_info(client_contextP, &block_info);
                   }
                 }

        break;

        case COAP_TYPE_ACK:
             ds_iot_ulog_high("COAP: Dropping this ACK as TRANSACTION not found");
             goto end;
              
        case COAP_TYPE_RST:

             ds_iot_ulog_high("COAP: Received RST message type");

            if (client_contextP->client_handle_request_cb)
            {
                coap_error_code = (coap_status_t)client_contextP->client_handle_request_cb(
                                          (qapi_Coap_Session_Hdl_t)client_contextP->cid,
                                          (qapi_Coap_Packet_t *)dl_pkt, client_contextP->clientData);
            }
            else
            {
                ds_iot_ulog_high("COAP: DL request call back is NULL");
                if (dl_pkt)
                {
                   qapi_Coap_Free_Message((qapi_Coap_Session_Hdl_t)client_contextP->cid,
                                          (qapi_Coap_Packet_t *)dl_pkt);
                   dl_pkt = NULL;
                }
            }

            if(block_info)
            {
               clean_block_info(client_contextP, &block_info);
            }
        break;
      }

    } /* Request or Response */
	
  } /* if (parsed correctly) */
  else
  {
    ds_iot_ulog_high2("COAP: Message parsing failed %u.%2u", coap_error_code >> 5,
                         coap_error_code & 0x1F);
	
    if (coap_error_code != NO_ERROR && coap_error_code != COAP_IGNORE)
    {
      coap_packet_ext_t pkt = {0};
     /* Set to sendable error code. */
	
       if (coap_error_code >= 192)
       {
          coap_error_code = (coap_status_t)COAP_500_INTERNAL_SERVER_ERROR;
       }

   /* Reuse input buffer for error message. */
	   
        coap_init_message(&pkt, COAP_TYPE_ACK, coap_error_code,dl_pkt->coap_pkt.mid);
        coap_set_payload(&pkt, coap_error_message,strlen(coap_error_message));
        coap_error_code = message_send(client_contextP,(coap_packet_t *)&pkt,fromSessionH ,NULL);

    }

     /* free dl_pkt */
	 goto end;

  }

  /* free response */
  if(response)
  {
    mem_utility_release_memory(umem!=NULL, response);
    response = NULL;
  }

  /* returning as dl_pkt is freed in any one of the callback */
  return; 

end:
  if(dl_pkt)
  {
    qapi_Coap_Free_Message((qapi_Coap_Session_Hdl_t)client_contextP->cid,
                            (qapi_Coap_Packet_t *)dl_pkt);
    dl_pkt = NULL;
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------

/* @fn     coap_status_t message_send()
 * @brief  This function is used to serialise message and send that to server
 * @param  context    - hanlde to coap client context
 *         message    - pointer to coap packet 
 *         sessionH   - pointer to connection
 *         lastmid    - message id pointer
 * @return on error   - COAP_500_INTERNAL_SERVER_ERROR
 *         on success - COAP_NO_ERROR   
 */
coap_status_t message_send(void * context,coap_packet_t * pkt,void *sessionH , uint16_t *lastMid)
{
  client_context_t * contextP = NULL;
  coap_status_t result = (coap_status_t) COAP_500_INTERNAL_SERVER_ERROR;
  uint8_t * pktBuffer;
  size_t pktBufferLen = 0;
  size_t allocLen;
  void * umem = NULL;

  if (context && pkt && sessionH) 
  {
    contextP = (client_context_t *) context;
    if ( lastMid)
    {
      *lastMid = pkt->mid ;
    }

    allocLen = coap_serialize_get_size(pkt);
	ds_iot_ulog_high1("Size to allocate: %d", allocLen);
	if (allocLen == 0)
    {
      return result; 
    }
   
	umem = mem_utility_Get_MemoryPool((void *)contextP->cid, APP_COAP);
	pktBuffer = (uint8_t *)mem_utility_Get_Memory_From_Pool(umem, allocLen);

    if (pktBuffer != NULL)
    {
      pktBufferLen = coap_serialize_message(pkt, pktBuffer);
      ds_iot_ulog_high1("coap_serialize_message() returned %d", pktBufferLen);
      if (0 != pktBufferLen)
      { 
        result = (coap_status_t) coap_buffer_send(contextP, pktBuffer, pktBufferLen, contextP->clientData,NULL);
      }

      mem_utility_release_memory(umem!=NULL, pktBuffer);
      pkt->buffer = NULL;
    }
  } 
  return result;
}      

/* @fn     uint16_t coap_get_message_id()
 * @brief  This function is used to get message if from coap client context
 * @param  coap_Handle  - hanlde to coap cliext context  
 * @return message id  
 */
uint16_t coap_get_message_id( void *coap_Handle)
{
  client_context_t *client_ctxt = NULL;
  client_ctxt =  (client_context_t *)coap_Handle;
  return client_ctxt->nextMID++;  
}

qurt_thread_t get_coap_thread_id(void)
{
    return coap_thread_id;
}


static uint32_t generate_coap_handle_id(void) 
{ 
  uint32_t handle_id = 0;
  client_context_t * client_ctxt = NULL;

  while (true) 
  { 
    boolean handle_id_exists = false;

    /* Generate a random Device Info handle ID */ 
      handle_id = generate_rand32(); 
 
      COAP_LOCK_MUTEX(gCoapMutex);
      client_ctxt = g_client_ctxt;
      while (client_ctxt) 
      {
        if(client_ctxt->cid == handle_id)
        {
           handle_id_exists = TRUE;
        }
        client_ctxt= client_ctxt->next;
      }
      COAP_UNLOCK_MUTEX(gCoapMutex);

    if (false == handle_id_exists) 
      break;  
  } 

  return handle_id; 
}


