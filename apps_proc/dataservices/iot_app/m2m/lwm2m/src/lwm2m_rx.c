/*==============================================================================

                             LWM2M_RX.C

GENERAL DESCRIPTION
  This file deals with the LWM2M RX Packet processing implementation.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Command Queue must me initialized

Copyright (c) 2017-2023 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
==============================================================================*/

#include "lwm2m_rx.h"
#include "qapi_coap.h"
#include "net_utils_ext.h"
#ifdef PSM_LWM2M
#include "liblwm2m.h"
#include "qapi_psm.h"
#endif
#include "lwm2m_carrier.h"

#ifdef BLOCK1_SUPPORT  
#include "coap-internal.h"
#endif
#include "ds_log.h"
#include "lwm2mclient.h"


extern bool roaming_status;
extern bool lwm2m_in_sleep_state;
extern uint32_t active_time;
extern bool lwm2m_psm_enable;  // LWM2M PSM is enabled or not.
extern bool persis_regstatus_feat_enabled;

extern const char *coap_error_message;

#ifdef BLOCK1_SUPPORT  
extern client_context_t * get_coap_context_from_CID(qapi_Coap_Session_Hdl_t hdl);
#endif

extern void handle_reset(void * contextP,
                         void * fromSessionH,
                         qapi_Coap_Packet_t * message);



static lwm2m_task_pkt_q                        pktq;

static qurt_mutex_t gLwm2mRXMutex;


static lwm2m_pktq_t                      * lwm2m_pktq = NULL;
static lwm2m_pktq_t                        lwm2m_pktqinfo;

lwm2m_pending_pkt pending_dl_pkt;


/*===========================================================================

FUNCTION LWM2M_CMDQ_INIT()

DESCRIPTION
  Initialize the command queues and set the command queue pointers in the
  lwm2m_info struct to the appropriate values.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

=========================================================================*/
void lwm2m_pktq_init (lwm2m_pktq_t * lwm2m_pktqinfo)
{
  if (lwm2m_pktqinfo == NULL)
  {
    LOG_DEBUG("LWM2M State is NULL ");
    return;
  }

  LOG_DEBUG ("lwm2m_cmdq_init ");

  lwm2m_pktq = lwm2m_pktqinfo;

  q_init(&pktq);

  lwm2m_pktq->cmd_q_ptr = &pktq;
}


/*===========================================================================

FUNCTION LWM2M_GET_CMD_BUFFER

DESCRIPTION
  This function is used to allocate a command buffer that the client can
  then enqueue into the command queue.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the command

SIDE EFFECTS
  None

=========================================================================*/
lwm2m_pkt_t* lwm2m_get_pkt_buffer()
{
  lwm2m_pkt_t*                 pkt_ptr = NULL;

  pkt_ptr = (lwm2m_pkt_t*)lwm2m_malloc(sizeof(lwm2m_pkt_t));

  if (pkt_ptr == NULL) 
  {
    ds_iot_ulog_high("LWM2M_LOG: Memory Allocation FAILED cmd buff");
  }

  return pkt_ptr;
}


/*===========================================================================

FUNCTION  LWM2M_ENQUEUE_CMD

DESCRIPTION
  This function is used to enqueue the command into the LWM2M queue

DEPENDENCIES
  None.

PARAMETERS
  Pointer to the command to enqueue

RETURN VALUE
  Void

SIDE EFFECTS
  None

=========================================================================*/
void lwm2m_enqueue_pkt(lwm2m_pkt_t* cmd_ptr)
{
#if 0
  LOG_INFO("Cmd_ptr: 0x%x with cmd_id: %d, posted to int Q," 
                             "num cmds already in Q: %d",
                             cmd_ptr, cmd_ptr->cmd_hdr.cmd_id, 
                             q_cnt(&pktq));
#endif
  q_put(&pktq, &(cmd_ptr->cmd_hdr.link));

  //LOG_INFO(" Num commands in internal Q now: %d",
                           //  q_cnt(&pktq));
  return;
}

/*===========================================================================

FUNCTION LWM2M_DEQUEUE_CMD

DESCRIPTION
  This function de-queues and returns a command from the LWM2M task
  command queues.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the command if present in the queue. NULL otherwise

SIDE EFFECTS
  None

==============================================================================*/
static lwm2m_pkt_t* lwm2m_dequeue_pkt( void )
{
  lwm2m_pkt_t *pkt_ptr = NULL;

  //LOG_INFO(" lwm2m_dequeue_cmd called",0,0,0);

  pkt_ptr = (lwm2m_pkt_t*)q_get( &pktq );

  //LOG_INFO(" Cmd ptr: 0x%x, num rem cmds: LWM2M Q count: %d",
    //                       pkt_ptr, q_cnt(&pktq));

  return pkt_ptr;
}


/*===========================================================================

FUNCTION LWM2M_CNT_CMD

DESCRIPTION
  This function returns the number of items currently queued on LWM2M task
  command queues.

DEPENDENCIES
  None
    
RETURN VALUE
  The number of items currently queued on the LWM2M task
  command queue.

SIDE EFFECTS
  None.

==============================================================================*/
int lwm2m_cnt_pkt( void )
{
   int pkt_cnt = -1;

  //LOG_INFO("lwm2m_cnt_pkt: m2m_cnt_pkt called");

  pkt_cnt = q_cnt( &pktq );

  //LOG_INFO("lwm2m_cnt_pkt: packet count = %d", pkt_cnt);

  return pkt_cnt;
}


/*==============================================================================
FUNCTION LWM2M_PROCESS_COMMANDS()

DESCRIPTION
  This function dequeues all outstanding commands from and dispatches the
  processor functions.

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES 
  None 

SIDE EFFECTS
  None
==============================================================================*/
void lwm2m_process_pkt(void) 
{

  lwm2m_pkt_t *pkt_ptr      = NULL;
  lwm2m_context_t * contextP =NULL; 
  lwm2m_uri_t * uriP = NULL;
  lwm2m_media_type_t format;
  qapi_Coap_Packet_t * dl_pkt = NULL;
  qapi_Coap_Packet_t * response = NULL;	
  uint8_t result = (lwm2m_status_t) LWM2M_IGNORE;
  lwm2m_server_t * serverP = NULL; 
  void * fromSessionH = NULL;
  qapi_Coap_Status_t coap_response_code = QAPI_NO_ERROR;
  qapi_Coap_Session_Hdl_t hdl = NULL;
  qapi_Coap_Message_Params_t message_conf;
  qapi_Coap_Block_Wise_Options_t blocktype = QAPI_COAP_BLOCK_OPTION_NONE;
  uint16_t blocksize = 0;
  qapi_Status_t ret_status = QAPI_ERROR;
  transaction_cb_info_t * transacP =  NULL;
  lwm2m_event_msg * event_msg = NULL;
  qapi_Device_Info_t* dev_info = NULL;

#ifdef BLOCK1_SUPPORT  
  client_context_t * client_contextP = NULL;
  coap_block_session_info_t * block_info =  NULL;
  qapi_Coap_Block_Params_t  *dl_block_param = NULL;
#endif

#ifdef PSM_LWM2M
  psm_status_msg_type *psm_status = NULL;
#endif
  while ( (pkt_ptr = lwm2m_dequeue_pkt()) != NULL ) 
  {
    serverP = NULL; 
    uriP = NULL;
    memset(&message_conf, 0, sizeof(qapi_Coap_Message_Params_t));
	
	if(!lwm2m_is_AppRegister_Allowed())
    {
      dl_pkt = (qapi_Coap_Packet_t *)pkt_ptr->cmd_data.message;
	  hdl = pkt_ptr->cmd_data.cHandle;
      ds_iot_ulog_err("LwM2M clean up in progress.");
      if(pkt_ptr)
      {
         lwm2m_free(pkt_ptr);
         pkt_ptr = NULL;
      }
	  if(dl_pkt )
      {
        qapi_Coap_Free_Message(hdl, dl_pkt);
        dl_pkt = NULL;
      }
      continue;
    }
 
#ifdef PSM_LWM2M
    if(lwm2m_psm_enable == TRUE)
    {
      if(pkt_ptr->cmd_hdr.cmd_id == LWM2M_PSM_CB)
      {
        psm_status = (psm_status_msg_type *)pkt_ptr->cmd_data.clientData;
        lwm2m_psm_event_handle(psm_status);
        if(pkt_ptr)
        {
          lwm2m_free(pkt_ptr);
          pkt_ptr = NULL;
        }
        continue;
      }
    }
#endif

#ifdef ENABLE_APN_CONN_OBJ
    if(pkt_ptr->cmd_hdr.cmd_id == LWM2M_PROFILE_PROCESSING)
    {
      lwm2m_profile_action(pkt_ptr->cmd_data.clientData);
      if(pkt_ptr)
      {
        lwm2m_free(pkt_ptr);
        pkt_ptr = NULL;
      }
      continue;
    }
#endif

   if(pkt_ptr->cmd_hdr.cmd_id == LWM2M_EVENT_CB)
    {
       event_msg = (lwm2m_event_msg *)pkt_ptr->cmd_data.clientData;

       if(event_msg == NULL)
       {
         ds_iot_ulog_high("LWM2M: LWM2M Process Packet Received Corrupt event_msg");

		 if(pkt_ptr)
         {
           lwm2m_free(pkt_ptr);
           pkt_ptr = NULL;
         }
         continue;
       }

       contextP = (lwm2m_context_t *)event_msg->event_info;

       fromSessionH = pkt_ptr->cmd_data.cHandle;
       serverP = utils_findServer(contextP, fromSessionH);
 
       if(serverP)
       {
	      switch(event_msg->lwm2m_event)
          {
            case QAPI_COAP_EVENT_SSL_ALERT_FATAL: 
              serverP->status = STATE_DEREGISTERED;
		    break;
            default:
		       ds_iot_ulog_high("Got undefined coap event ");
	      }
       }

	   if(event_msg)
         free(event_msg);

       if(pkt_ptr)
       {
         lwm2m_free(pkt_ptr);
         pkt_ptr = NULL;
       }

       serverP = NULL;
       continue;
     }

   
    if(LWM2M_IS_TRANSACTION_CB (pkt_ptr->cmd_hdr.cmd_id))
    {
       hdl = pkt_ptr->cmd_data.cHandle;
       dl_pkt = (qapi_Coap_Packet_t *)pkt_ptr->cmd_data.message;
       transacP = (transaction_cb_info_t *)pkt_ptr->cmd_data.clientData;

       switch(pkt_ptr->cmd_hdr.cmd_id)
       {
         case LWM2M_BOOTSTRAP_CB:
            prv_handleBootstrapReply(hdl, transacP, dl_pkt);
            break;
         case LWM2M_REGISTRATION_CB:
            prv_handleRegistrationReply(hdl, transacP, dl_pkt);
            break;
            
         case LWM2M_REGISTRATION_UPDATE_CB:
            prv_handleRegistrationUpdateReply(hdl, transacP, dl_pkt);
            break;
            
         case LWM2M_DEREGISTRATION_CB:
            prv_handleDeregistrationReply(hdl, transacP, dl_pkt);
            break;
         case LWM2M_EXT_APPLICATION_CB:
            lwm2m_application_response_handler(hdl, transacP, dl_pkt);
            break;
         default:
            break;
       }
       if(pkt_ptr)
       {
          lwm2m_free(pkt_ptr);
          pkt_ptr = NULL;
       }
       continue;
    }
	 if(pkt_ptr->cmd_hdr.cmd_id == LWM2M_DEVICE_INFO_CB)
	 {
 	     dev_info = (qapi_Device_Info_t *)pkt_ptr->cmd_data.clientData;
         proc_lwm2m_nas_status(dev_info);
         if(pkt_ptr)
         {
           lwm2m_free(pkt_ptr);
           pkt_ptr = NULL;
         }
         continue;
	  }
    coap_response_code = QAPI_NO_ERROR;
    hdl = pkt_ptr->cmd_data.cHandle;

    if(hdl == NULL)
    {
      ds_iot_ulog_high("Handle is NULL");
      if(pkt_ptr)
      {
          lwm2m_free(pkt_ptr);
          pkt_ptr = NULL;
      }
	  continue;
    }
    dl_pkt = (qapi_Coap_Packet_t *)pkt_ptr->cmd_data.message;
    contextP = (lwm2m_context_t *)pkt_ptr->cmd_data.clientData;

    fromSessionH = pkt_ptr->cmd_data.cHandle;

    serverP = utils_findServer(contextP, fromSessionH);
    if(serverP !=NULL)
    {
      ds_iot_ulog_high1("Received pkt from SSID[%d]", serverP->shortID);
      serverP = NULL;
    }

    if(dl_pkt == NULL ||  
                        fromSessionH == NULL || contextP == NULL)
    {
      ds_iot_ulog_high("COAP:LWM2M: LWM2M Process Packet Received Corrupt Packet");
      continue; 
    }
    if(LWM2M_IS_INVALIDE_PACKETS(dl_pkt))
    {
       ds_iot_ulog_err("Invalid packet received");
       goto process_error;
   }

    ds_iot_ulog_high3("COAP:LWM2M Message Type %d Message Code %d and Message ID %d",dl_pkt->type,dl_pkt->code ,dl_pkt->mid);

    if (dl_pkt->type == QAPI_COAP_TYPE_CON)
    {
          /* Reliable CON requests are answered with an ACK. */           
      qapi_Coap_Init_Message(hdl, &response , QAPI_COAP_TYPE_ACK , QAPI_CONTENT_2_05);
      if(response == NULL)
      {
        goto process_error;
      }
      response->mid = dl_pkt->mid;
    }
    else
    {
          /* Unreliable NON requests are answered with a NON as well. */  
       qapi_Coap_Init_Message(hdl, &response , QAPI_COAP_TYPE_NON , QAPI_CONTENT_2_05);
       if(response == NULL)
       {
         goto process_error;
       }
    }

     /* mirror token */                                                  
    if (dl_pkt->token_len)
    {
       qapi_Coap_Set_Header(hdl,response ,QAPI_COAP_TOKEN,dl_pkt->token,dl_pkt->token_len);
    }
 #ifdef LWM2M_CLIENT_MODE
    uriP = uri_decode(contextP->altPath, (qapi_Multi_Option_t *)dl_pkt->uri_path);
 #else
    uriP = uri_decode(NULL, dl_pkt->uri_path);
 #endif
    if (uriP == NULL)
    {
      ds_iot_ulog_high("COAP:ERROR! LWM2M NULL URI Received");
      result = (uint8_t) LWM2M_400_BAD_REQUEST;
      qapi_Coap_Set_Header(hdl, response, QAPI_COAP_SET_STATUS, &result, 0);
      goto process_response;
    }
#ifdef BLOCK1_SUPPORT

    if(IS_OPTION(dl_pkt, COAP_OPTION_BLOCK1) && uriP->objectId == LWM2M_FIRMWARE_UPDATE_OBJECT_ID)
    {
      dl_block_param = (qapi_Coap_Block_Params_t*) malloc(sizeof(qapi_Coap_Block_Params_t));
      if(dl_block_param == NULL)
      {
        ds_iot_ulog_high("Block: Malloc failed for dl_block_param");
        result = LWM2M_500_INTERNAL_SERVER_ERROR;
        goto process_response;
      }
      memset(dl_block_param , 0 , sizeof(qapi_Coap_Block_Params_t));
      if(qapi_Coap_Get_Header(hdl,dl_pkt,QAPI_COAP_BLOCK1,(void **)&dl_block_param , 0) == QAPI_OK)
      {
        ds_iot_ulog_high4("COAP:Blockwise: block1 request NUM %u(SZX %u/ SZX Max%u) MORE %u", dl_block_param->num, 
                 dl_block_param->size, REST_MAX_CHUNK_SIZE, dl_block_param->more);
      }
      dl_block_param->size = MIN(dl_block_param->size, REST_MAX_CHUNK_SIZE);
      qapi_Coap_Set_Header(hdl, response, QAPI_COAP_BLOCK1, dl_block_param, sizeof(qapi_Coap_Block_Params_t));
    }
    if(IS_OPTION(dl_pkt, COAP_OPTION_BLOCK1) && uriP->objectId != LWM2M_FIRMWARE_UPDATE_OBJECT_ID)
    {
      client_contextP = (client_context_t *)get_coap_context_from_CID(hdl);
      block_info = get_block_info_from_pkt(client_contextP , (coap_packet_t *)dl_pkt);
      ds_iot_ulog_high1("Block: blockwise request for object[%d]", uriP->objectId);

      ret_status = process_blockwise_transfer(client_contextP, NULL,dl_pkt);
      if(ret_status == QAPI_COAP_IGNORE)
      {
        result = LWM2M_IGNORE;
        goto process_response;
      }
      else if(ret_status == QAPI_REQ_ENTITY_INCOMPLETE_4_08 || ret_status == QAPI_REQUEST_ENTITY_TOO_LARGE_4_13 || ret_status == QAPI_INTERNAL_SERVER_ERROR_5_00 )
      {
        result = ret_status;
        coap_set_status_code(response, result);
        if(block_info == NULL && client_contextP != NULL )
           block_info = client_contextP->bl_info_param[DL_REQ]; 
        clean_block_info(client_contextP, &block_info);
        goto process_response;
      }
      if(block_info == NULL && client_contextP != NULL )
         block_info = client_contextP->bl_info_param[DL_REQ]; 
      clean_block_info(client_contextP, &block_info);
    }

#endif
    {
      // Check if device is in roaming state
      if(!roaming_status)
      {
        {
         // Check if the Device is in Sleep state
          if(!lwm2m_in_sleep_state)
          {
            if (dl_pkt->type == QAPI_COAP_TYPE_RST)
            {
              handle_reset(contextP, fromSessionH, dl_pkt);
              result = (uint8_t)LWM2M_IGNORE;
              goto process_response;
            } 
            switch(uriP->flag & LWM2M_URI_MASK_TYPE)
            {
 #ifdef LWM2M_CLIENT_MODE
              case LWM2M_URI_FLAG_DM:
              {
                if(pending_dl_pkt.dl_pkt != NULL)
                {
                  free(pending_dl_pkt.dl_pkt);
                  pending_dl_pkt.dl_pkt = NULL;
                }
                pending_dl_pkt.dl_pkt = malloc(sizeof(qapi_Coap_Packet_t));
                if(pending_dl_pkt.dl_pkt != NULL)
                {
                  memset(pending_dl_pkt.dl_pkt, 0x00, sizeof(qapi_Coap_Packet_t));
                  memcpy(pending_dl_pkt.dl_pkt, dl_pkt, sizeof(qapi_Coap_Packet_t));
                }
                LOG_DEBUG("COAP:LWM2M URI Received with DM Flag Set ");
                serverP = utils_findServer(contextP, fromSessionH);
                
                if (serverP != NULL)
                {
                   /* Standard objects processing */ 
                  if (lwm2m_is_standard_object(uriP->objectId))
                  {
                    ds_iot_ulog_high("COAP:LWM2M URI Received for Standard Objects");
                    result = dm_handleRequest(contextP, uriP, serverP, dl_pkt, response);
                  }
                  else 
                  {
                    ds_iot_ulog_high("COAP:LWM2M URI Received for Extensible Objects");
                    result = dm_handleRequest_ExObj(contextP, uriP, serverP, dl_pkt, response);
                  }
                }
 #ifdef LWM2M_BOOTSTRAP
                else
                {
                  serverP = utils_findBootstrapServer(contextP, fromSessionH);
                  if (serverP != NULL)
                  {
                    ds_iot_ulog_high("COAP:LWM2M Msg for BS server, Handle Command");
                    result = bootstrap_handleCommand(contextP, uriP, serverP, dl_pkt, response);
                  }
                  else
                  {
                    ds_iot_ulog_high("COAP:LWM2M Could not find BS SERVER");
                    result = (uint8_t) LWM2M_400_BAD_REQUEST;
                    qapi_Coap_Set_Header(hdl, response, QAPI_COAP_SET_STATUS, &result, 0);
                    goto process_response;
                  }
                }
 #endif
              }
              break;

 #ifdef LWM2M_BOOTSTRAP

              case LWM2M_URI_FLAG_ALL_OBJECTS:
              if (QAPI_COAP_DELETE == dl_pkt->code)
              {
                  ds_iot_ulog_high("COAP:LWM2M Bootstrap Handle Delete ALL");
                  result = bootstrap_handleDeleteAll(contextP, fromSessionH);
              }
              else if(QAPI_COAP_GET == dl_pkt->code)
              {
                uint8_t *buffer = NULL;
                size_t buffer_length = 0;
                ds_iot_ulog_high("COAP:LWM2M Bootstrap GET Message");
                serverP = utils_findServer(contextP,fromSessionH);
                if (serverP != NULL)
                {
                   result = (uint8_t) LWM2M_400_BAD_REQUEST;
                   LOG_DEBUG("COAP:LWM2M Error- GET ALL for DM");
                   qapi_Coap_Set_Header(hdl, response, QAPI_COAP_SET_STATUS, &result, 0);
                   goto process_response;
                }
                else
                {
                  serverP = utils_findBootstrapServer(contextP, fromSessionH);
                  if (serverP != NULL)
                  {
                      if (QAPI_IS_OPTION(dl_pkt, QAPI_COAP_OPTION_ACCEPT)
                      && dl_pkt->accept_num == 1
                      && dl_pkt->accept[0] == QAPI_APPLICATION_LINK_FORMAT)
                     {
                         ds_iot_ulog_high("BS Discover for");
                         format = (lwm2m_media_type_t) QAPI_APPLICATION_LINK_FORMAT;
                         result = object_bs_discover(contextP, INVALID_OBJECT_ID, &buffer, &buffer_length);
                         serverP->last_BS_Operation_Time = lwm2m_gettime();
                         if (persis_regstatus_feat_enabled == LWM2M_TRUE) 
                           lwm2m_carrier_update_regstatus(serverP);
                     }
                  }
                  else
                  result = (uint8_t) LWM2M_400_BAD_REQUEST;
                  ds_iot_ulog_high("COAP:LWM2M Could not find Bootstrap Server");
                }
                if (LWM2M_205_CONTENT == result)
                {
                  qapi_Coap_Set_Header(hdl,response ,QAPI_COAP_CONTENT_TYPE,&format,0);
                  qapi_Coap_Set_Payload(hdl,response, buffer, buffer_length);
                }

                if(buffer)
                  lwm2m_free(buffer);

              }
              else
              {
                ds_iot_ulog_high("COAP:LWM2M Error -Unsupported Code with ALL flag");
                result = (uint8_t) LWM2M_400_BAD_REQUEST;
              }
              break;

              case LWM2M_URI_FLAG_BOOTSTRAP:
              if (dl_pkt->code == QAPI_COAP_POST)
              {
                  result = bootstrap_handleFinish(contextP, fromSessionH);
              }
              break;
#endif
#endif

              default:
              result = (uint8_t) LWM2M_IGNORE;
              ds_iot_ulog_high1("COAP:LWM2M Default with flag %d", uriP->flag & LWM2M_URI_MASK_TYPE);
              break;
            }

            ds_iot_ulog_high1("COAP:LWM2M Setting the status of Response Message as %d",result);
            qapi_Coap_Set_Header(hdl,response , QAPI_COAP_SET_STATUS , &result , 0);
#ifdef BLOCK1_SUPPORT
            if(IS_OPTION(dl_pkt, COAP_OPTION_BLOCK1 ) && (uriP->objectId == LWM2M_FIRMWARE_UPDATE_OBJECT_ID) && (dl_block_param) && (dl_block_param->more))
            {
              if(result == LWM2M_NO_ERROR)
              {
                result = COAP_231_CONTINUE;
                ds_iot_ulog_high1("COAP:LWM2M BLOCK1 OBJECT Setting the status of Response Message as %d",result);
                qapi_Coap_Set_Header(hdl,response , QAPI_COAP_SET_STATUS , &result , 0);
              }
              else 
              {
                ds_iot_ulog_high1("COAP:LWM2M OBJECTBLOCK1 Setting the status of Response Message as %d",result);
                qapi_Coap_Set_Header(hdl,response , QAPI_COAP_SET_STATUS , &result , 0);
              }
              lwm2m_free(dl_block_param);
            }
#endif

           // Update the active time for LWM2M client
            active_time = lwm2m_gettime();
            #ifdef PSM_LWM2M
			lwm2m_psm_operation(contextP, 0, NULL, PSM_NODE_TYPE_MIN, PSM_NODE_UPDATE_ALL, PSM_MODIFY_NODE);

            #endif
            ds_iot_ulog_high2("Sending response to Server %u.%2u", (result & 0xFF) >> 5, (result & 0x1F));
          } // End of lwm2m_sleep_state
          else
          {
            ds_iot_ulog_high ("Client in SLEEP STATE ignoring the request in call back ");
          }
        }
      }

 process_response:  
      if(result != LWM2M_IGNORE)
      {
        if(response->type == QAPI_COAP_TYPE_ACK && response->code != QAPI_COAP_EMPTY)
        {
           serverP = utils_findServer(contextP, hdl);
           if(serverP == NULL)
              serverP = utils_findBootstrapServer(contextP, hdl);
           blocksize = get_blockSize(serverP);
           if(blocksize >= 16 && response->payload_len > blocksize)
           { 
              blocktype  = QAPI_COAP_BLOCK2_OPTION;
           }
           else
           {
             blocksize = 0;
             blocktype  = QAPI_COAP_BLOCK_OPTION_NONE;
           }
        }
        if(qapi_Coap_Send_Message_v2(hdl , response, &message_conf, blocktype, blocksize) != QAPI_OK)
        {
           ds_iot_ulog_high("COAP:LWM2M_RX qapi_Coap_Send_Message returned error");
        }
        ds_iot_ulog_high3("COAP:LWM2M Application Result %u.%2u,CoAP Response, %d ",(result & 0xFF) >> 5, (result & 0x1F), coap_response_code);
      }
    }
    lwm2m_free(uriP);

process_error:

    if(response)
    {
      qapi_Coap_Free_Message(hdl, response);
      response = NULL;
    }

    if(dl_pkt)
    {
      qapi_Coap_Free_Message(hdl, dl_pkt);
      dl_pkt = NULL;
    }

    if(pkt_ptr)
    {
      lwm2m_free(pkt_ptr);
      pkt_ptr = NULL;
    }

  }

  return;
} /* lwm2m_process_commands */ 

/*=============================================================================

FUNCTION LWM2M_SET_SIGNAL

DESCRIPTION
  Set the lwm2m signal using lock protection

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
void lwm2m_set_signal(void)
{
  qurt_mutex_lock(&lwm2m_pktqinfo.lwm2m_mutex);

  LWM2M_TASK_SET_SIG(lwm2m_pktqinfo.lwm2m_signal,LWM2M_SIGNAL_MASK);

  qurt_mutex_unlock(&lwm2m_pktqinfo.lwm2m_mutex);

  return;
}

/*=============================================================================

FUNCTION LWM2M_MUTEX_INIT

DESCRIPTION
  lwm2m mutex Initialization

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
static void lwm2m_mutex_init(void)
{
  qurt_mutex_init(&lwm2m_pktqinfo.lwm2m_mutex);

  qurt_mutex_init(&gLwm2mRXMutex);
  return;
}


/*=============================================================================

FUNCTION LWM2M_SIGNAL_INIT

DESCRIPTION
  LWM2M Signal Initialization
  
DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
static void lwm2m_signal_init(void)
{
  qurt_signal_init(&lwm2m_pktqinfo.lwm2m_signal);
  return;
}


/*=============================================================================

FUNCTION LWM2M_SIGNAL_WAIT

DESCRIPTION
  This API will wait on the LWM2M signal.
  The LWM2M signal will be set during various operations.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
static void lwm2m_signal_wait(void)
{
  uint32_t received_sigs = 0;

  ds_iot_ulog_high("lwm2m_signal_wait ");

  while (1)
  {
/*============================================================================
  Wait for the LWM2M Signal to be set
=============================================================================*/
    received_sigs = qurt_signal_wait(&lwm2m_pktqinfo.lwm2m_signal, LWM2M_SIGNAL_MASK,
                                     QURT_SIGNAL_ATTR_WAIT_ANY);
      //LOG_INFO(" lwm2m_signal_wait came out\n");

    if (LWM2M_TASK_IS_SIG_SET (received_sigs, LWM2M_SIGNAL_MASK))
    {
      // LWM2M Signal is set Dequeue the LWM2M Command queue
      lwm2m_process_pkt();
    }

/*============================================================================
  Clear the LWM2M signal 
=============================================================================*/

    qurt_mutex_lock(&lwm2m_pktqinfo.lwm2m_mutex);

    LWM2M_TASK_CLEAR_SIG(lwm2m_pktqinfo.lwm2m_signal, LWM2M_SIGNAL_MASK);

    qurt_mutex_unlock(&lwm2m_pktqinfo.lwm2m_mutex);
  }
}

/*================================================================

FUNCTION LWM2M_CMDQ_TASK_INIT

DESCRIPTION
  LWM2M CMDQ Task initialization
  
DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==================================================================*/
void lwm2m_pkt_task_init(void)
{

  LOG_DEBUG("lwm2m_pkt_task_init ");
/*============================================================================
  Reseting the Global LWM2M CMDQ Structure
=============================================================================*/

  memset (&lwm2m_pktqinfo, 0x00, sizeof (lwm2m_pktq_t));

/*----------------------------------------------------------------------------
  Initialize the command queues and set the command queue pointers in the
   lwm2m_info struct to the appropriate values.
----------------------------------------------------------------------------*/
  lwm2m_pktq_init(&lwm2m_pktqinfo);

}


/*=============================================================================

FUNCTION LWM2M_RX_TASK_ENTRY

DESCRIPTION
  This is the entry point for LWM2M RX task
  Handshake with RCInit
  Reset the Global LWM2M CMDQ structure
  Initalize the LWM2M Qurt signal

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
void lwm2m_cmdq_task_entry(void *thread_ctxt)
{

  LOG_DEBUG("lwm2m_rx_task_entry ");
/*============================================================================
  Handshake with RCInit
    Indicate to the Task start framework that we have completed the task start
  procedures.
 
  If any synchronization is needed with other tasks during the task start time
  we should send an event indicating what we wish to communicate. 
=============================================================================*/

  rcinit_handshake_startup();

/*=============================================================================
Initialize Mutex Object
==============================================================================*/

  lwm2m_mutex_init();

/*=============================================================================
Initialize Signal for LWM2M RX THREAD
==============================================================================*/

  lwm2m_signal_init ();

/*=============================================================================
Wait on the CoAP Signal
==============================================================================*/
  lwm2m_signal_wait();

}


