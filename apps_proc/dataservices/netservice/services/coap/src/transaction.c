/******************************************************************************

  @file    transaction.c

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
 *    Simon Bernard - Please refer to git log
 *    Toby Jaffey - Please refer to git log
 *    Pascal Rieux - Please refer to git log
 *    Bosch Software Innovations GmbH - Please refer to git log
 *
 *******************************************************************************/

/*
   Copyright (c) 2013, 2014 Intel Corporation

   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 * Neither the name of Intel Corporation nor the names of its contributors
 may be used to endorse or promote products derived from this software
 without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 THE POSSIBILITY OF SUCH DAMAGE.

 David Navarro <david.navarro@intel.com>

*/

/*
   Contains code snippets which are:

 * Copyright (c) 2013, Institute for Pervasive Computing, ETH Zurich
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.

*/

/************************************************************************
 *  Function for communications transactions.
 *
 *  Basic specification: rfc7252
 *
 *  Transaction implements processing of piggybacked and separate response communication
 *  dialogs specified in section 2.2 of the above specification.
 *  The caller registers a callback function, which is called, when either the result is
 *  received or a timeout occurs.
 *
 *  Supported dialogs:
 *  Requests (GET - DELETE):
 *  - CON with mid, without token => regular finished with corresponding ACK.MID
 *  - CON with mid, with token => regular finished with corresponding ACK.MID and response containing
 *                  the token. Supports both versions, with piggybacked ACK and separate ACK/response.
 *                  Though the ACK.MID may be lost in the separate version, a matching response may
 *                  finish the transaction even without the ACK.MID.
 *  - NON without token => no transaction, no result expected!
 *  - NON with token => regular finished with response containing the token.
 *  Responses (COAP_201_CREATED - ?):
 *  - CON with mid => regular finished with corresponding ACK.MID
 */

#include <time.h>
#include <stringl/stringl.h>
#include <time_svc.h>
#include "qapi_socket.h"

#include <time_svc.h>
#include "app_coap.h"
#include "connection.h"
#include "coap-internal.h"
#include "qapi_coap.h"
#include "net_utils_ext.h"
#include "qapi_timer.h"


extern client_context_t client_ctxt;

/*
 * Modulo mask (+1 and +0.5 for rounding) for a random number to get the tick number for the random
 * retransmission time between COAP_RESPONSE_TIMEOUT and COAP_RESPONSE_TIMEOUT*COAP_RESPONSE_RANDOM_FACTOR.
 */
#define COAP_RESPONSE_TIMEOUT_TICKS         (CLOCK_SECOND * COAP_RESPONSE_TIMEOUT)
#define COAP_RESPONSE_TIMEOUT_BACKOFF_MASK  ((CLOCK_SECOND * COAP_RESPONSE_TIMEOUT * (COAP_RESPONSE_RANDOM_FACTOR - 1)) + 1.5)


#define COAP_MAX_ID   ((uint16_t)0xFFFF)
#define COAP_URI_FLAG_OBJECT_ID    (uint8_t)0x04
#define COAP_URI_FLAG_INSTANCE_ID  (uint8_t)0x02
#define COAP_URI_FLAG_RESOURCE_ID  (uint8_t)0x01
#define COAP_STRING_ID_MAX_LEN 6
/* DTLS Resumption would kick in only minimum elapsed time between last packet from server to now */
#define DEFAULT_COAP_SESSION_RESUMPTION_TIME 60

#define COAP_URI_IS_SET_INSTANCE(uri) (((uri)->flag & COAP_URI_FLAG_INSTANCE_ID) != 0)
#define COAP_URI_IS_SET_RESOURCE(uri) (((uri)->flag & COAP_URI_FLAG_RESOURCE_ID) != 0)


/* @fn      static int prv_checkFinished()
 * @brief   This functions is used to check for  token set 
 * @param   transacP        - pointer the transaction
 *          receivedMessage - pointer to the coap packet 
 * @return  on success      - 1
 *          on error        - 0  
 */
int prv_checkFinished(coap_transaction_t * transacP,
    coap_packet_t * receivedMessage)
{
  int len;
  const uint8_t* token;
  coap_packet_t * transactionMessage = transacP->message;

  /* check token in rcvd message to handle empty ACK without token set scenario  */
  if (!IS_OPTION(transactionMessage, COAP_OPTION_TOKEN) || !IS_OPTION(receivedMessage , COAP_OPTION_TOKEN))
  {
    // request without token
    return transacP->ack_received ? 1 : 0;
  }

  len = coap_get_header_token(receivedMessage, &token);
  if (transactionMessage->token_len == len)
  {
    if (memcmp(transactionMessage->token, token, len)==0) return 1;
  }

  return 0;
}
/* @fn     coap_transaction_t * transaction_new()
 * @brief  This function is used to prepare new coap transaction 
 * @param  transac_newP  - pointer to the new transaction param 
 * @return on error     - NULL
 *         on success   - coap new transaction pointer          
 */

coap_transaction_t * transaction_new(coap_transac_new_param_t *transac_newP)
{
  coap_transaction_t * transacP = NULL;
  client_context_t *contexP = NULL ; 
  int mID = 0;

  if ((transac_newP == NULL) || (transac_newP->coapHandle == NULL)) return NULL;
  contexP = (client_context_t * )(transac_newP->coapHandle); 

  transacP = (coap_transaction_t *)malloc(sizeof(coap_transaction_t));

  if (NULL == transacP) 
  {
	contexP->last_error = QAPI_ERR_COAP_ENOMEM;
	contexP->mod_category = QAPI_NET_COAP_CAT;
	return NULL;
  }
  memset(transacP, 0, sizeof(coap_transaction_t));

  transacP->mID = ((coap_packet_t *)transac_newP->message)->mid; 
  mID = transacP->mID ;
  transacP->message = transac_newP->message;

  if (0 < transac_newP->token_len)
  {
    if (NULL != transac_newP->token)
    {
      coap_set_header_token(transacP->message, transac_newP->token, transac_newP->token_len);
    }
    else {
      // generate a token
      uint8_t temp_token[COAP_TOKEN_LEN];
      time_t tv_sec =  time_get_secs();
      // initialize first 6 bytes, leave the last 2 random
      temp_token[0] = mID;
      temp_token[1] = mID >> 8;
      temp_token[2] = tv_sec;
      temp_token[3] = tv_sec >> 8;
      temp_token[4] = tv_sec >> 16;
      temp_token[5] = tv_sec >> 24;
      // use just the provided amount of bytes
      coap_set_header_token(transacP->message, temp_token, transac_newP->token_len);
    }

  }    

  COAP_LOCK_MUTEX(contexP->CoapClientMutex);

  contexP->transactionList=(coap_transaction_t*)COAP_LIST_ADD(contexP->transactionList,transacP);
  contexP->trans_count++;

  COAP_UNLOCK_MUTEX(contexP->CoapClientMutex);

  return transacP;
 
}
/* @fn     void transaction_free() 
 *
 * @brief  This function is used to free the transaction
 * @param  transacP - pointer to the coap transaction
 * @return void  
 */
void transaction_free(coap_transaction_t * transacP)
{
  if (transacP)
  {
    if (transacP->buffer)
    {
      free(transacP->buffer);
    }

    free(transacP);
    
  }
}

/* @fn     void transaction_remove()
 * @brief  This function is used to remove transaction from client context
 * @param  contextp - client context handle
 *         transacP - pointer to the transaction to remove
 * @return void
 */
void transaction_remove(client_context_t * contextP,
    coap_transaction_t * transacP)
{
  ds_iot_ulog_high("Entering");
  contextP->transactionList = (coap_transaction_t *) COAP_LIST_RM(contextP->transactionList, transacP->mID, NULL);
  contextP->trans_count--;
  transaction_free(transacP);
}
/* @fn     bool transaction_handleResponse( )
 * @brief  This function is used to  handlee the response to the transaction send
 * @param  contectp     - handle to coap context
 *         fromsessionH - pointer to the connection  
 *         message      - input coap packet pointer
 *         response     - response to be filled coap packet  pointer
 * @return on success   - true 
 *         on error     - false        
 *
 */        
bool transaction_handleResponse(void * context,
    void * fromSessionH,
    coap_packet_t * dl_pkt,
    coap_packet_t * response,
    void * umem)
{
  bool found = false;
  bool reset = false;
  client_context_t * contextP = context;
  coap_transaction_t * transacP;
  coap_block_session_info_t * block_info = NULL;
  qapi_Status_t ret_status = QAPI_OK;
  blockWise_handling_status_t status = BLOCKWISE_TRANSFER_REQUEST_FALSE;

  qapi_Coap_Transaction_t * clientTransaction = NULL;

  ds_iot_ulog_high("Entering");
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
            transacP->ack_received = true;
            reset = COAP_TYPE_RST == dl_pkt->type;
          }
        }
      }

      if (reset || prv_checkFinished(transacP, dl_pkt))
      {
        // HACK: If a message is sent from the monitor callback,
        // it will arrive before the registration ACK.
        // So we resend transaction that were denied for authentication reason.
        if (!reset)
        {
          if (COAP_TYPE_CON == dl_pkt->type && NULL != response)
          {
            coap_init_message(response, COAP_TYPE_ACK, 0, dl_pkt->mid);
            message_send(contextP, response, fromSessionH, NULL);
          }

          if ((COAP_401_UNAUTHORIZED == dl_pkt->code) && (contextP->coap_max_retransmits > transacP->retrans_counter))
          {
            transacP->ack_received = false;
            transacP->retrans_time += ((contextP->coap_transaction_timeout) *1000);
            return true;
          }
        }       

        status = get_blockWise_handling_status(contextP, dl_pkt , true);
        ds_iot_ulog_high1("Blockwise handling status[%d]", status);

        if(status == BLOCKWISE_COAP_HANDLE_REQUEST)
        {
           ret_status = process_blockwise_transfer(contextP,transacP,(qapi_Coap_Packet_t *)dl_pkt);
           if(ret_status == QAPI_COAP_IGNORE)
           {
                return true;
           }
           
        }
        else if(status == BLOCKWISE_INVALID_REQUEST)
        {
           ret_status = QAPI_BAD_OPTION_4_02;
        }
        if(ret_status == QAPI_REQ_ENTITY_INCOMPLETE_4_08 || ret_status == QAPI_REQUEST_ENTITY_TOO_LARGE_4_13 
           || ret_status == QAPI_INTERNAL_SERVER_ERROR_5_00 || ret_status == QAPI_BAD_OPTION_4_02)
        {
           if(response != NULL)
           {
               coap_init_message(response, COAP_TYPE_ACK, ret_status, dl_pkt->mid);
               message_send(contextP, response, fromSessionH, NULL);
           }
        }
        block_info = get_block_info_from_pkt(contextP , dl_pkt);
        //Empty ACK for blockwise CON response pkt 
        if(block_info && COAP_TYPE_ACK == dl_pkt->type && dl_pkt->code == COAP_EMPTY && transacP->empty_ack_received == FALSE)
        {
           if(block_info->numByteSent != block_info->complete_PaylodLen)
           {
              int l_time = 0;
              ds_iot_ulog_high("COAP:TF Got an Empty ACK , Response pending");

              l_time = contextP->coap_exchange_lifetime;
              transacP->retrans_counter = contextP->coap_max_retransmits + 2;
              transacP->empty_ack_received = TRUE;
              transacP->retrans_time += l_time * 1000;
              return true;
           }
        }
        if (transacP->callback != NULL)
        {
          int ret = QAPI_ERROR;
          ds_iot_ulog_high2("Transaction Handle Response transaction address %p and userdata %p ",transacP,transacP->userData);

          /* Umem Is  the User Space Memory Pointer
             if this is not NULL it means we are getting DL 
             Data for a DAM Application hence we need to allocate
             the memory from a TX Byte Pool instead of doing a
             malloc
          */

          ret = coap_init_client_transaction(&clientTransaction , contextP ,transacP , umem , true, block_info);

          if(ret == QAPI_OK)
          {
            if(block_info)
            {
               dl_pkt->mid  = block_info->original_pkt->mid;
            }
            if(ret_status == QAPI_REQ_ENTITY_INCOMPLETE_4_08 || ret_status == QAPI_REQUEST_ENTITY_TOO_LARGE_4_13 || ret_status == QAPI_INTERNAL_SERVER_ERROR_5_00 
                || dl_pkt->code == QAPI_REQ_ENTITY_INCOMPLETE_4_08 || dl_pkt->code == QAPI_REQUEST_ENTITY_TOO_LARGE_4_13)
            {
               transacP->callback((void *)contextP->cid, clientTransaction, NULL);
            }
            else
            {
            transacP->callback((void *)contextP->cid, clientTransaction, dl_pkt);
            }
  
            if(umem)
            {
             qapi_Timer_Sleep(300,QAPI_TIMER_UNIT_MSEC,true);
		    }
            else
             free(clientTransaction);
          }
        }
        if(block_info && status != BLOCKWISE_APPLICATION_HANDLE_REQUEST)
           clean_block_info(contextP, &block_info);
        transaction_remove(contextP, transacP);
		transacP = NULL;
        return true;
      }
      // if we found our guy, exit
      if (found)
      {
        time_t tv_msec = time_get_ms_native();
        // (tv_sec>=0) is changed to the below code to avoid the compiler warning 'unsigned int value cannot be less than zero'
        if ((tv_msec == 0) || (tv_msec > 0))
        {
          transacP->retrans_time = tv_msec;
        }
        if (transacP->response_timeout)
        {
          transacP->retrans_time += transacP->response_timeout;
        }
        else
        {
          transacP->retrans_time += (contextP->coap_transaction_timeout*1000) * (transacP->retrans_counter);
        }
        return true;
      }
    }

    transacP = transacP->next;
  }
  return false;
}

/* @fn     int transaction_send()
 * @brief  This function is used to send  transaction and to remove transaction based on the conditions set 
 * @param  context  - hanlde ot coap context 
 *         transacP - pointer to the transaction 
 * @return on success - 0
 *         on error   - COAP_500_INTERNAL_SERVER_ERROR or -1      
 */ 
int transaction_send(void * context,coap_transaction_t * transacP)
{
  client_context_t* contextP = NULL;
  bool maxRetriesReached = false;
  qapi_Coap_Transaction_t * clientTransaction = NULL;
  void * umem = NULL;
  int ret = QAPI_ERROR;
  coap_block_session_info_t * block_info = NULL;

  contextP = ( client_context_t *)context ;

  if(contextP == NULL || transacP == NULL)
  return COAP_500_INTERNAL_SERVER_ERROR;

  block_info = get_block_info_from_pkt(contextP , transacP->message);

  umem = mem_utility_Get_MemoryPool((void *)contextP->cid, APP_COAP);

  ds_iot_ulog_high("transaction_send");
  if (transacP->buffer == NULL)
  {
    transacP->buffer_len = coap_serialize_get_size(transacP->message);
    if (transacP->buffer_len == 0)
	{
       if(block_info)
        clean_block_info(contextP, &block_info);
      contextP->last_error = QAPI_INTERNAL_SERVER_ERROR_5_00;
	  contextP->mod_category = QAPI_NET_COAP_CAT;
	  transaction_remove(contextP , transacP);
      return COAP_500_INTERNAL_SERVER_ERROR;
    }

    transacP->buffer = (uint8_t*)malloc(transacP->buffer_len);
    
    if (transacP->buffer == NULL)
    {
      if(block_info)
        clean_block_info(contextP, &block_info);
	  contextP->last_error = QAPI_INTERNAL_SERVER_ERROR_5_00;
	  contextP->mod_category = QAPI_NET_COAP_CAT;
      transaction_remove(contextP , transacP);
	  transacP = NULL;
      return COAP_500_INTERNAL_SERVER_ERROR;
    }
    transacP->buffer_len = coap_serialize_message(transacP->message, transacP->buffer);
    if (transacP->buffer_len == 0)
    {
      if(block_info)
        clean_block_info(contextP, &block_info);
      transaction_remove(contextP, transacP);
	  transacP = NULL;
      return COAP_500_INTERNAL_SERVER_ERROR;
    }
  }

  if (!transacP->ack_received)
  {
    long unsigned timeout = 0x00;

    if (0 == transacP->retrans_counter)
    {
      time_t tv_sec = time_get_secs();
      //Warning Fix: pointless comparison of unsigned integer with zero
      // (tv_sec>=0) is changed to the below code to avoid the compiler warning 'unsigned int value cannot be less than zero'
      if (0 < tv_sec || 0 == tv_sec)
      {
        transacP->retrans_counter = 1;
        timeout = (contextP->coap_transaction_timeout *1000);
      }
      else
      {
        maxRetriesReached = true;
      }
    }
    else
    {
      timeout =( contextP->coap_transaction_timeout << (transacP->retrans_counter - 1))*1000;
    }

    if ((contextP->coap_max_retransmits + 1) >= transacP->retrans_counter)
    {
       time_t tv_msec = 0;
       /* This will be used for non resumption time*/
       ret = coap_buffer_send((void *)contextP, transacP->buffer, transacP->buffer_len, NULL,transacP);

      /* Update the next retransmit time after sending the COAP packet */ 
	   
       tv_msec = (time_t)time_get_ms_native();

       if(transacP->retrans_counter == 1)
       {
          transacP->retrans_time = tv_msec + timeout;
       }
       else
       {
          transacP->retrans_time += timeout;
       }
       
       transacP->retrans_counter += 1;
       if(block_info)
          block_info->last_msg_send_time = time_get_secs();

       if(ret != COAP_NO_ERROR)
       {

          /* if error occured due to resumption failure , remove transactions to avoid retransmissions */

          if(ret == COAP_DTLS_RESUMPTION_FAILED || (contextP && ret == COAP_500_INTERNAL_SERVER_ERROR))
          {
             if(transacP)
             {
                COAP_LOCK_MUTEX(contextP->CoapClientMutex);
		   
	            while (NULL != contextP->transactionList)
                {
                   coap_transaction_t * transaction = NULL;
                   transaction = contextP->transactionList;

	               if(transaction)
                   {
                      contextP->transactionList = contextP->transactionList->next;
                      transaction->sessionH = NULL;   
                      transaction_free(transaction);
                      transaction = NULL;
		           }

		        }

	            COAP_UNLOCK_MUTEX(contextP->CoapClientMutex);

			 }
			 
 			 contextP->last_error = QAPI_ERR_COAP_DTLS_RESUMPTION_FAILED_IN_COAP_SEND;
			 contextP->mod_category = QAPI_NET_COAP_CAT;
             ret = __QAPI_ERROR(QAPI_MOD_NETWORKING, QAPI_NET_COAP_DTLS_RESUMPTION_FAILED_IN_COAP_SEND);
             return ret;
          }
          else
          {
            contextP->last_error = QAPI_INTERNAL_SERVER_ERROR_5_00;
			contextP->mod_category = QAPI_NET_COAP_CAT;
            return COAP_500_INTERNAL_SERVER_ERROR;
          }
       }

    }
    else
    {
      maxRetriesReached = true;
    }
  }

  if (transacP->ack_received || maxRetriesReached)
  {
    if (transacP->callback)
    {
      int ret = QAPI_ERROR;
      ds_iot_ulog_high2("Transaction Handle Response transaction address %p and userdata %p ",transacP,transacP->userData);

      /* Umem Is  the User Space Memory Pointer
         if this is not NULL it means we are getting DL
         \]a for a DAM Application hence we need to allocate
         the memory from a TX Byte Pool instead of doing a
         malloc
      */

	  ret = coap_init_client_transaction(&clientTransaction , contextP ,transacP , umem , true, block_info);

      if(ret == QAPI_OK)
      {
        if (transacP->callback)
          transacP->callback((void *)contextP->cid, clientTransaction, NULL);

		if(clientTransaction)
		{
		  if(umem == NULL)
		    mem_utility_release_memory(umem!=NULL, clientTransaction);
		}
      }
    }
    if(block_info)
       clean_block_info(contextP, &block_info);
    transaction_remove(contextP, transacP);
	transacP = NULL;
    return -1;
  }

  return 0;
}
/* @fn     void transaction_step()
 * @brief  This function is used to handle retransmission of transaction
 * @param  contextp    - handle to coap client context
 *         currentTime - current time 
 *         TimeoutP    - pointer to struct timeval
 * @return void           
 */
void transaction_step(client_context_t * contextP,
    time_t currentTime,
    time_t * timeoutP)
{
  coap_transaction_t * transacP;

  transacP = contextP->transactionList;
  if(transacP == NULL)
    *timeoutP = 2000;
  while (transacP != NULL)
  {
    // transaction_send() may remove transaction from the linked list
    coap_transaction_t * nextP = transacP->next;
    int removed = 0;

    if (transacP->retrans_time <= currentTime)
    {
      removed = transaction_send(contextP, transacP);
    }

    if (0 == removed)
    {
      time_t interval;

      if (transacP->retrans_time > currentTime)
      {
        interval = transacP->retrans_time - currentTime;
      }
      else
      {
        interval = 1000;
      }

      if (*timeoutP > interval)
      {
        *timeoutP = interval;
      }
    }
    else
    {
      *timeoutP = 1000;
    }

    transacP = nextP;
  }
}

