/******************************************************************************

  @file    packet.c

  ---------------------------------------------------------------------------
  Copyright (c) 2016-2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------

******************************************************************************/

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
 *    domedambrosio - Please refer to git log
 *    Fabien Fleutot - Please refer to git log
 *    Fabien Fleutot - Please refer to git log
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


#include "internals.h"
#include "lwm2mclient.h" 
#include <stdlib.h>
#include <string.h>

#include <stdio.h>
#include "lwm2m_rx.h"
#include "ds_log.h"

extern uint32_t active_time;
extern bool lwm2m_in_sleep_state;
extern bool roaming_status;

/**
 * @fn void handle_reset()
 * @brief the function used to handle RESET for observation
 * @param contextP handle to lwm2m context
 * @param fromSessionH handle to server session(connection)
 * @param message handle to message received
 */
void handle_reset(void * contextP,
    void * fromSessionH,
    qapi_Coap_Packet_t * message)
{
  lwm2m_server_data_t lwm2m_info;
  lwm2m_context_t * context = NULL;

  if (contextP == NULL || fromSessionH == NULL || message == NULL) 
  {
    LOG_DEBUG(" COAP: Handle Reset Passed  Argument is NULL");
    return;
  }

  ds_iot_ulog_high("Handling the RST message ");
  context = contextP;
#ifdef LWM2M_CLIENT_MODE
  LOG_DEBUG("Entering");
  
  /* Inform the OEM applications of the reset to cancel the observation - custom objects */
  memset(&lwm2m_info, 0x00, sizeof(lwm2m_server_data_t));
  lwm2m_info.msg_type = QAPI_NET_LWM2M_CANCEL_OBSERVE_REQ_E;
  lwm2m_info.notification_id = message->mid;
  lwm2m_client_application_notification(&lwm2m_info);     

  /* Cancel observation for standard objects */
  observe_cancel(context, message->mid, fromSessionH, NULL);
#endif
}

/**
 * @fn int32_t lwm2m_handle_request_cb()
 * @brief This function implements the callback to coap stack called upon
 *        receiving a request
 * @param context       handle to lwm2m context
 * @param fromSessionH  handle to server session(connection)
 * @param message       handle to request message
 * @param response      handle to response message to be filled
 * @return returns the coap status code to be responded with 
 */

int32_t lwm2m_handle_request_cb(qapi_Coap_Session_Hdl_t  hdl,
    qapi_Coap_Packet_t * dl_pkt,
    void * clientData)
{
   lwm2m_pkt_t * pkt = NULL;


   qapi_Coap_Status_t coap_response_code;

  if (hdl == NULL||  dl_pkt == NULL || clientData == NULL) 
  { 
     LOG_DEBUG("COAP: Passed Argument is NULL");
     coap_response_code =(qapi_Coap_Status_t) QAPI_COAP_IGNORE;
	 qapi_Coap_Free_Message(hdl, dl_pkt);
     return coap_response_code;
  }

  coap_response_code = (qapi_Coap_Status_t)QAPI_NO_ERROR;

 
  /* Enqueue this request to the lwm2m command 
   queue buffer
  */

  if(lwm2m_cnt_pkt() > LWM2M_MAX_DL_PKTS)
  {
     LOG_DEBUG("Lwm2m command queue is full, ignoring the request in call back");
     coap_response_code =(qapi_Coap_Status_t) QAPI_COAP_IGNORE;
	 qapi_Coap_Free_Message(hdl, dl_pkt);
     return coap_response_code;
  }

  pkt = lwm2m_get_pkt_buffer();

  if(pkt == NULL)
  {
    LOG_DEBUG("Allocating lwm2m command queue buffer failed");
    coap_response_code =  QAPI_MEMORY_ALLOCATION_ERROR;
	qapi_Coap_Free_Message(hdl, dl_pkt);
    return coap_response_code;
  }

  memset(&pkt->cmd_hdr, 0x00, sizeof(lwm2m_pkt_hdr_t));
    memset(&pkt->cmd_data ,0, sizeof(pkt->cmd_data));
  ds_iot_ulog_high1("COAP:LWM2M Enqueuing Packed for Processing by LWM2M RX Thread %d ", dl_pkt->mid);

  pkt->cmd_data.cHandle = (void *)hdl;
  pkt->cmd_data.message = (void *)dl_pkt;
  pkt->cmd_data.clientData = clientData;

  lwm2m_enqueue_pkt(pkt);

  lwm2m_set_signal();

  return coap_response_code;

}

void lwm2m_Coap_Event_Callback(qapi_Coap_Session_Hdl_t hdl , qapi_Coap_Packet_t * dl_pkt , qapi_Coap_Event_t lwm2m_event , void * event_info)
{

	lwm2m_pkt_t * pkt = NULL;
	lwm2m_event_msg * event_msg = NULL;
	
	if (hdl == NULL || event_info == NULL) 
	{ 
	   LOG_DEBUG("lwm2m_Coap_Event_Callback : Passed Argument is NULL");
       return;
	}

    pkt = lwm2m_get_pkt_buffer();

    if(pkt == NULL)
    {
      LOG_DEBUG("lwm2m_Coap_Event_Callback : pkt %s",mallocError);
      return;
    }

    memset(pkt, 0x00, sizeof(lwm2m_pkt_t));

    pkt->cmd_hdr.cmd_id = LWM2M_EVENT_CB;
    pkt->cmd_data.cHandle = (void *)hdl;
    pkt->cmd_data.message = (void *)dl_pkt;

    event_msg = (lwm2m_event_msg *)malloc(sizeof(lwm2m_event_msg));

	if(event_msg == NULL)
    {
      if(pkt)
      {
        lwm2m_free(pkt);
	  }
      LOG_DEBUG("lwm2m_Coap_Event_Callback : event_msg %s",mallocError);
      return;
    }

    event_msg->event_info = event_info;
    event_msg->lwm2m_event = lwm2m_event;

    pkt->cmd_data.clientData = (void *)event_msg;

    lwm2m_enqueue_pkt(pkt);
    lwm2m_set_signal();
    return;
 }



