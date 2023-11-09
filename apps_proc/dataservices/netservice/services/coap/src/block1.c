/******************************************************************************

  @file    block1.c

  ---------------------------------------------------------------------------

  Copyright (c) 2016-2018, 2020-2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------

 *****************************************************************************/

#include <stringl/stringl.h>
#include <time.h>
#include "qapi_socket.h"
#include <time_svc.h>
#include "app_coap.h"
#include "connection.h"
#include "coap-internal.h"

qapi_Coap_Status_t blockwise_rx_handler(qapi_Coap_Session_Hdl_t          hdl, coap_block_session_info_t * block_info , qapi_Coap_Packet_t * dl_pkt , qapi_Coap_Block_Params_t * dl_bl_param);


/* @fn     is_bl_session_match()
 * @brief  This function is used to match the blockwise session for coap packet
 * @param  block_info - pointer to coap blockwise session info
 *         dl_pkt -     pointer to coap packet 
 * @return boolean - true on match, false otherwise
 */

bool is_bl_session_match(coap_block_session_info_t * block_info , qapi_Coap_Packet_t * dl_pkt)
{
   bool ret = false;
   char *dl_pkt_uri_path = NULL;
   int dl_pkt_uri_path_len = 0;
   
   if(block_info == NULL || dl_pkt == NULL)
     return ret;

   if ((COAP_TYPE_ACK == dl_pkt->type) || (COAP_TYPE_RST == dl_pkt->type))
   {
      if(block_info->master_pkt && block_info->master_pkt->mid == dl_pkt->mid)
      {
         return true;
      }
      if(block_info->original_pkt && block_info->original_pkt->mid == dl_pkt->mid)
      {
          return true;
      }
   }
   if(QAPI_IS_OPTION(dl_pkt , QAPI_COAP_OPTION_TOKEN))
   {
      if (dl_pkt->token_len == block_info->token_len)
      {
          if(memcmp(block_info->token , dl_pkt->token , dl_pkt->token_len)==0)
            return true;
      }
   }

   dl_pkt_uri_path_len = coap_get_header_uri_path((void * )dl_pkt, (char **)&dl_pkt_uri_path);
   if(block_info->uri_path_str_len == (uint16_t)dl_pkt_uri_path_len)
   {
     if(memcmp(block_info->uri_path_str, dl_pkt_uri_path, dl_pkt_uri_path_len) == 0)
     {
       ret = true;
     }
   }
   if(dl_pkt_uri_path)
     free(dl_pkt_uri_path);
   return ret;
}


/* @fn     get_block_info_from_pkt()
 * @brief  This function is used to get the blockwise session info for coap packet
 * @param  contextP - pointer to coap client context
 *         dl_pkt -     pointer to coap packet 
 * @return  pointer to blockwise session info on match, Null otherwise
 */

coap_block_session_info_t* get_block_info_from_pkt(client_context_t * contextP , coap_packet_t * dl_pkt)
{
  if(contextP == NULL || dl_pkt == NULL)
    return NULL;

  if(is_bl_session_match(contextP->bl_info_param[DL_REQ] , (qapi_Coap_Packet_t *) dl_pkt))
  {
     ds_iot_ulog_high("Block: Found blockwise downlink session ");
     return contextP->bl_info_param[DL_REQ];
  }
  else if(is_bl_session_match(contextP->bl_info_param[UL_REQ] , (qapi_Coap_Packet_t *)dl_pkt))
  {
     ds_iot_ulog_high("Block: Found blockwise Uplink session ");
     return contextP->bl_info_param[UL_REQ];
  }
  ds_iot_ulog_high("Block: Not found blockwise session for the pkt ");
  return NULL;
}


/* @fn     session_handleBlockwise()
 * @brief  This function is used to handle downlink blockwise transfer when transaction not found 
 * @param  contextP - pointer to coap client context
 *         dl_pkt -     pointer to downlink coap packet 
 *         dl_block_param - pointer to block info to downlink coap packet 
 * @return  QAPI_NO_ERROR, QAPI_COAP_IGNORE in case of success otherwise failure code
 */

qapi_Status_t session_handleBlockwise(client_context_t * contextP, qapi_Coap_Packet_t * dl_pkt, qapi_Coap_Block_Params_t  *dl_block_param)
{
   qapi_Status_t ret_status = QAPI_ERROR;
   bool final_pkt = false;
   coap_block_session_info_t * block_info = NULL;
   int dl_pkt_uri_path_len = 0;

   if(contextP == NULL ||dl_block_param ==NULL || dl_pkt == NULL)
   {
      ds_iot_ulog_high("session_handleBlockwise: NULL Param Passed ");
      return QAPI_INTERNAL_SERVER_ERROR_5_00;
   }
   //B2 uplink response ACK
   if(dl_pkt->code == QAPI_COAP_GET)
   {
     block_info = contextP->bl_info_param[UL_REQ];
     if(block_info == NULL || !is_bl_session_match(block_info , dl_pkt)) 
     {
       return QAPI_NO_ERROR;
     }
     ret_status = get_block_wise_packet(contextP, block_info->master_pkt, block_info , dl_block_param);
   }
   // B1 downlink request
   if(dl_pkt->code == QAPI_COAP_PUT || dl_pkt->code == QAPI_COAP_POST)
   {
     block_info = contextP->bl_info_param[DL_REQ];
     if(block_info == NULL )
     {
       block_info = (coap_block_session_info_t *)malloc(sizeof(coap_block_session_info_t));
       if(block_info == NULL)
  {
          return QAPI_INTERNAL_SERVER_ERROR_5_00;
  }
       memset(block_info, 0 , sizeof(coap_block_session_info_t));
       block_info->master_pkt = ( qapi_Coap_Packet_t *) malloc(sizeof (coap_packet_ext_t));
       if(block_info->master_pkt == NULL)
  {
          free(block_info);
          return QAPI_INTERNAL_SERVER_ERROR_5_00;
       }
       coap_init_message((void *)(block_info->master_pkt), COAP_TYPE_ACK, QAPI_CONTINUE_2_31 ,dl_pkt->mid);
       dl_pkt_uri_path_len = coap_get_header_uri_path((void * )dl_pkt, (char **)&(block_info->uri_path_str));
       if(dl_pkt_uri_path_len < 1)
         block_info->uri_path_str_len = 0;
       else
         block_info->uri_path_str_len = (uint16_t)dl_pkt_uri_path_len;
       contextP->bl_info_param[DL_REQ] = block_info;
       block_info->first_bl_time = time_get_secs() ;
       block_info->tv_expiry_time     = contextP->blockwise_max_age;
     }
     else if ((block_info && !is_bl_session_match(block_info , dl_pkt)) || (block_info == NULL && dl_block_param->num != 0))
      {
        return QAPI_REQ_ENTITY_INCOMPLETE_4_08;
      }
     if((ret_status = blockwise_rx_handler((qapi_Coap_Session_Hdl_t)contextP->cid, block_info , dl_pkt , dl_block_param)) == QAPI_NO_ERROR)
     {
       if(dl_block_param->more == 0)
       {
         ds_iot_ulog_high("Block: received final packet");
         final_pkt = true;
       }
       else
       {
         if(dl_pkt->token_len)
         {
           coap_set_header_token(block_info->master_pkt, dl_pkt->token, dl_pkt->token_len);
           block_info->token_len          = dl_pkt->token_len;
           COAP_MEMCPY(block_info->token, COAP_TOKEN_LEN, dl_pkt->token , block_info->token_len);
         }
         else
         {
           block_info->master_pkt->token_len = 0;
           memset(block_info->master_pkt->token, 0, sizeof(block_info->master_pkt->token));
           UNSET_OPTION(block_info->master_pkt, COAP_OPTION_TOKEN);
         }
         coap_set_header_block1((void *)block_info->master_pkt, dl_block_param->num, dl_block_param->more, dl_block_param->size);
         if(dl_block_param->num == 0)coap_set_option((coap_packet_ext_t *)block_info->master_pkt, COAP_OPTION_BLOCK1, NULL);
       }
     }
   }
   if(ret_status == QAPI_NO_ERROR && final_pkt == false)
   {
      ds_iot_ulog_high("Block: sending next blockwise req/resp packet ");
      block_info->master_pkt->mid = dl_pkt->mid;
      message_send(contextP, (coap_packet_t *)block_info->master_pkt,contextP->conninfo , NULL);
      block_info->last_msg_send_time = time_get_secs();
      ret_status = QAPI_COAP_IGNORE;
   }
   return ret_status;
}

/* @fn     transaction_handleBlockwise()
 * @brief  This function is used to handle downlink blockwise transfer when transaction found 
 * @param  contextP - pointer to coap client context
 *         dl_pkt -     pointer to downlink coap packet 
 *         dl_block_param - pointer to block info to downlink coap packet 
 * @return  QAPI_NO_ERROR, QAPI_COAP_IGNORE in case of success otherwise failure code
 */

qapi_Status_t transaction_handleBlockwise(client_context_t * contextP , coap_transaction_t * transacP, qapi_Coap_Packet_t * dl_pkt, qapi_Coap_Block_Params_t  *dl_block_param) 
{
   qapi_Status_t ret_status = QAPI_ERROR;
   qapi_Coap_Packet_t *pkt = NULL;
   coap_block_session_info_t * block_info = NULL;
   bool final_pkt = false;

   if(contextP == NULL ||transacP ==NULL || dl_pkt == NULL)
   {
      ds_iot_ulog_high("transaction_handleBlockwise: NULL Param Passed ");
      return QAPI_INTERNAL_SERVER_ERROR_5_00;
   }

   pkt = (qapi_Coap_Packet_t *)transacP->message;
   //B1 uplink, B2 con-content uplink
   if(dl_pkt->code == QAPI_CONTINUE_2_31 || (dl_pkt->code == QAPI_CHANGED_2_04 && dl_block_param->more == 1) || dl_pkt->code == QAPI_COAP_GET)
   {
     block_info = contextP->bl_info_param[UL_REQ];
     if(block_info == NULL || !is_bl_session_match(block_info , dl_pkt)) 
     {
       return QAPI_NO_ERROR;
     }
     ret_status = get_block_wise_packet(contextP, pkt, block_info , dl_block_param);
   }
   if((dl_pkt->code == QAPI_CHANGED_2_04 || dl_pkt->code == QAPI_CREATED_2_01) && dl_block_param->more == 0)
   {
     ds_iot_ulog_high("Block: received final packet");
     final_pkt = true;
   }
   //B2 downlink response
   if(dl_pkt->code == QAPI_CONTENT_2_05)
   {
     block_info = contextP->bl_info_param[DL_REQ];
     if(block_info == NULL && dl_block_param->num == 0)
     {
        if(initialize_block_info(contextP, &block_info , transacP->message, dl_block_param->size)!= QAPI_OK)
        {
          return QAPI_INTERNAL_SERVER_ERROR_5_00;
        }
        contextP->bl_info_param[DL_REQ] = block_info;
        pkt = transacP->message = block_info->master_pkt;
        block_info->first_bl_time = time_get_secs() ;
     }
     else if ((block_info && !is_bl_session_match(block_info , dl_pkt)) || (block_info == NULL && dl_block_param->num != 0))
     {
        return QAPI_REQ_ENTITY_INCOMPLETE_4_08;
     }
     if((ret_status = blockwise_rx_handler((qapi_Coap_Session_Hdl_t)contextP->cid, block_info , dl_pkt , dl_block_param )) == QAPI_NO_ERROR)
     {
        if(dl_block_param->more == 0)
        {
           final_pkt = true;
        }
        else
	    {
           dl_block_param->num = dl_block_param->num + 1;
            pkt->mid = pkt->mid + 1;
           coap_set_header_block2((void *)pkt, dl_block_param->num, 0 , dl_block_param->size);
           if(dl_block_param->num == 1) coap_set_option((coap_packet_ext_t *)pkt, COAP_OPTION_BLOCK2, NULL);
	    }
     }
   }
   if(ret_status == QAPI_NO_ERROR && final_pkt == false )
   {
      ds_iot_ulog_high("Block: sending next blockwise req/resp packet ");
      transacP->retrans_counter = 0;
      transacP->retrans_time = 0;
      transacP->ack_received = 0;
      transacP->empty_ack_received = FALSE;
      if (transacP->buffer)
      {
        free(transacP->buffer);
      }
      transacP->mID = pkt->mid;
      transacP->buffer = NULL;
      transacP->message = pkt;
      transaction_send(contextP , transacP);
      block_info->last_msg_send_time = time_get_secs();
      ret_status = QAPI_COAP_IGNORE;
   }
   return ret_status;
}

/* @fn     process_blockwise_transfer()
 * @brief  This function is used to process blockwise transfer 
 * @param  contextP - pointer to coap client context
 *         transacP - pointer to transaction 
 *         dl_pkt -     pointer to downlink coap packet 
 * @return  QAPI_NO_ERROR, QAPI_COAP_IGNORE in case of success otherwise failure code
 */

qapi_Status_t process_blockwise_transfer(client_context_t * contextP , coap_transaction_t * transacP, qapi_Coap_Packet_t * dl_pkt) 
{
   qapi_Coap_Block_Params_t  *dl_block_param = NULL;
   qapi_Status_t ret_status = QAPI_ERROR;
   if(contextP == NULL || dl_pkt == NULL)
   {
      ds_iot_ulog_high("process_blockwise_transfer : NULL Param Passed ");
      return QAPI_INTERNAL_SERVER_ERROR_5_00;
   }
   if(QAPI_IS_OPTION(dl_pkt, QAPI_COAP_OPTION_BLOCK1))
   {
       ds_iot_ulog_high6("Block: Block1 downlink: type %u, code %u.%.2u, num[%d], more[%d], block_size[%d] ",
       dl_pkt->type,dl_pkt->code >> 5, dl_pkt->code & 0x1F, dl_pkt->block1_num, dl_pkt->block1_more, dl_pkt->block1_size);
   }
   if(QAPI_IS_OPTION(dl_pkt, QAPI_COAP_OPTION_BLOCK2))
   {
      ds_iot_ulog_high6("Block: Block2 downlink: type %u, code %u.%.2u, num[%d], more[%d], block_size[%d] ",
      dl_pkt->type, dl_pkt->code >> 5, dl_pkt->code & 0x1F, dl_pkt->block2_num, dl_pkt->block2_more, dl_pkt->block2_size);
   }
   if(dl_pkt->code == QAPI_COAP_GET && dl_pkt->block2_num == 0)
   {
     if(contextP->bl_info_param[UL_REQ])
     {
        ds_iot_ulog_err("Block: Ongoing uplink blockwise transfer,Dropping blockwise get request/retransmit");
        return QAPI_COAP_IGNORE;
  }
  else
        return QAPI_NO_ERROR;
   }
   if(((dl_pkt->code == QAPI_COAP_PUT || dl_pkt->code == QAPI_COAP_POST) && dl_pkt->block1_num == 0)|| (dl_pkt->code == QAPI_CONTENT_2_05 && dl_pkt->block2_num == 0 )) //B2 content check
   {
     if(contextP->bl_info_param[DL_REQ])
     {
        ds_iot_ulog_err("Block: Ongoing downlink blockwise transfer,Dropping blockwise request/retransmit");
        return QAPI_COAP_IGNORE;
     }
   }
   dl_block_param = (qapi_Coap_Block_Params_t*) malloc(sizeof(qapi_Coap_Block_Params_t));
   if(dl_block_param == NULL)
   {
     ds_iot_ulog_high("Block: Malloc failed for dl_block_param");
     return QAPI_INTERNAL_SERVER_ERROR_5_00;
   }
   memset(dl_block_param , 0 , sizeof(qapi_Coap_Block_Params_t));

   if(QAPI_IS_OPTION(dl_pkt, QAPI_COAP_OPTION_BLOCK1))
   {
      ret_status = qapi_Coap_Get_Header((qapi_Coap_Session_Hdl_t)contextP->cid ,dl_pkt , QAPI_COAP_BLOCK1 , (void **)&dl_block_param , 0);
   }
   else if(QAPI_IS_OPTION(dl_pkt, QAPI_COAP_OPTION_BLOCK2))
   { 
      ret_status = qapi_Coap_Get_Header((qapi_Coap_Session_Hdl_t)contextP->cid ,dl_pkt , QAPI_COAP_BLOCK2 , (void **)&dl_block_param , 0);
   }
   ds_iot_ulog_high1("Block: transaction[%d]", transacP != NULL);
   if(transacP)
   {
       ret_status = transaction_handleBlockwise(contextP , transacP, dl_pkt, dl_block_param);
   }
   else 
   {
      ret_status = session_handleBlockwise(contextP ,dl_pkt, dl_block_param);
   }
   ds_iot_ulog_high1("Block: Process blockwise return status[%d]", ret_status);
   if(dl_block_param)
     free(dl_block_param);
   return ret_status;
}


/* @fn     initialize_block_info()
 * @brief  This function is used to initialize block info 
 * @param  contextP - pointer to coap client context
 *         block_infoP - pointer to block info 
 *         pkt -     pointer to downlink coap packet 
 *         bsize     block size
 * @return  QAPI_OK in case of success otherwise QAPI_ERROR code
 */

qapi_Status_t initialize_block_info(client_context_t * contextP, coap_block_session_info_t ** block_infoP , qapi_Coap_Packet_t * pkt, uint16_t bsize)
{
   qapi_Status_t ret = QAPI_ERROR;
   coap_block_session_info_t *block_info = NULL;

   if(contextP == NULL || block_infoP == NULL || pkt ==NULL )
   {
      ds_iot_ulog_high("initialize_block_info: NULL Param Passed ");
	  if (contextP != NULL)
	  {
	  	 contextP->last_error = QAPI_INTERNAL_SERVER_ERROR_5_00;
	  	 contextP->mod_category = QAPI_NET_COAP_CAT;
	  }
	  return QAPI_INTERNAL_SERVER_ERROR_5_00;
   }
   block_info = (coap_block_session_info_t *)malloc(sizeof(coap_block_session_info_t));
   if(block_info == NULL)
   {
      return QAPI_ERROR;
   }
   memset(block_info, 0 , sizeof(coap_block_session_info_t));

   block_info->complete_PaylodLen = pkt->payload_len;
   block_info->content_type       = pkt->content_type;
   block_info->block_size         = bsize;
   block_info->original_pkt       = pkt;
   block_info->tv_expiry_time     = contextP->blockwise_max_age;

   if(pkt->etag)
   {
     block_info->etag_len           = pkt->etag_len;
     COAP_MEMCPY(block_info->etag, COAP_ETAG_LEN, pkt->etag , block_info->etag_len);
   }
   block_info->master_pkt = ( qapi_Coap_Packet_t *) malloc(sizeof (coap_packet_ext_t));
   if(block_info->master_pkt == NULL)
   {
   	  contextP->last_error = QAPI_ERR_COAP_ENOMEM;
	  contextP->mod_category = QAPI_NET_COAP_CAT;
      ret = QAPI_ERROR;
      goto clean;
   }
   coap_init_message((void *)(block_info->master_pkt),(coap_message_type_t)pkt->type, pkt->code,pkt->mid); 
   if(coap_copy_header((coap_packet_ext_t *)block_info->master_pkt , (coap_packet_ext_t *)pkt, NULL) < 0)
   {
	  contextP->last_error = QAPI_ERROR;
	  contextP->mod_category = QAPI_NET_COAP_CAT;
	  ret = QAPI_ERROR;
      goto clean;
   }
   if(pkt->token)
   {
     block_info->token_len          = pkt->token_len;
     COAP_MEMCPY(block_info->token, COAP_TOKEN_LEN, pkt->token , block_info->token_len);
     coap_set_header_token(block_info->master_pkt, pkt->token, pkt->token_len);
   }
   if(pkt->payload_len <= 0 || pkt->payload == NULL)
   {
      *block_infoP = block_info;
      return QAPI_OK;
   }
   block_info->complete_payload = (uint8_t *)malloc(pkt->payload_len);
   if(block_info->complete_payload == NULL)
   {
      contextP->last_error = QAPI_ERR_COAP_ENOMEM;
	  contextP->mod_category = QAPI_NET_COAP_CAT;
      ret = QAPI_ERROR;
      goto clean;
   }
   COAP_MEMCPY(block_info->complete_payload, pkt->payload_len, pkt->payload , pkt->payload_len);
   block_info->nextBlockP         = block_info->complete_payload;

   *block_infoP = block_info;
   ds_iot_ulog_high("Block: Successfully initialized block info");
   return QAPI_OK;
clean:
   if(block_info->master_pkt)
   {
     free(block_info->master_pkt);
     block_info->master_pkt = NULL;
   }
   if(block_info->complete_payload)
   {
     free(block_info->complete_payload);
     block_info->complete_payload = NULL;
   }
     
   if(block_info)
   {
     free(block_info);
     block_info = NULL;
   }
   ds_iot_ulog_high("Block: failed to initialize block info");
   return ret;
}


/* @fn     clean_block_info()
 * @brief  This function is used to clean blockwise session 
 * @param  ctxt - pointer to coap client context
 *         block_infoP - pointer to block info 
 * @return  void
 */

void clean_block_info(client_context_t * ctxt, coap_block_session_info_t **block_infoP)
{
   coap_block_session_info_t *block_info = NULL;
   
   if(ctxt == NULL || block_infoP == NULL || *block_infoP == NULL)
   {
     return;
   }
   
   block_info = *block_infoP;
   if(block_info->master_pkt)
   {
     coap_free_header(block_info->master_pkt,NULL);
     free(block_info->master_pkt);
     block_info->master_pkt = NULL;
   }
   if(block_info->complete_payload)
   {
     free(block_info->complete_payload);
     block_info->complete_payload = NULL;
   }
   if(block_info->uri_path_str)
   {
     free(block_info->uri_path_str);
     block_info->uri_path_str = NULL;
     block_info->uri_path_str_len = 0;
   }

   if(ctxt->bl_info_param[DL_REQ] == block_info)
   {
      ctxt->bl_info_param[DL_REQ] = NULL;
      ds_iot_ulog_high("Block: Removed blockwise downlink session ");
   }
   else if(ctxt->bl_info_param[UL_REQ] == block_info)
   {
      ctxt->bl_info_param[UL_REQ] = NULL;
      ds_iot_ulog_high("Block: Removed blockwise uplink session ");
   }

   if(block_info)
   {
      free(block_info);
      block_info = NULL;
	  *block_infoP = NULL;
   }

   return;
}


/* @fn     get_block_wise_packet()
 * @brief  This function is used to prepare blockwise packet 
 * @param  contextP - pointer to coap client context
 *         pkt -     pointer to  coap packet 
 *         block_infoP - pointer to block info 
 *         dl_block_param - pointer to block info to downlink coap packet 
 * @return  QAPI_NO_ERROR, QAPI_COAP_IGNORE in case of success otherwise failure code
 */

qapi_Status_t get_block_wise_packet(client_context_t * contextP, qapi_Coap_Packet_t * pkt, coap_block_session_info_t * block_info , qapi_Coap_Block_Params_t * dl_block_param )
{
   qapi_Coap_Block_Params_t bl_param = {0};
   qapi_Status_t ret = QAPI_ERROR;
   uint32_t skipBlock = 0;
   size_t  remainingSize = 0;
   uint8_t * block_payload = NULL;
   size_t block_payloadLen = 0;
   uint32_t block2_flag = 0;
   

   if(block_info == NULL)
   {
     ds_iot_ulog_high("get_block_wise_packet: NULL Param Passed ");
	 contextP->last_error = QAPI_INTERNAL_SERVER_ERROR_5_00;
	 contextP->mod_category = QAPI_NET_COAP_CAT;
	 return QAPI_INTERNAL_SERVER_ERROR_5_00;
   }

   memset(&bl_param , 0 , sizeof(qapi_Coap_Block_Params_t));
   if(QAPI_IS_OPTION(pkt, QAPI_COAP_OPTION_BLOCK2))
   {
      block2_flag = 1;
   }

   ds_iot_ulog_high3("Block: Get next blockwise packets bl_numSent[%d], bl_lastNum[%d], bl_size[%d]",
                   block_info->numBlockSent, block_info->lastBlockNum, block_info->block_size);

   /* Process First Block */
   if(dl_block_param == NULL  &&  block_info->numBlockSent == 0)
   {
      bl_param.num =0;
      remainingSize = block_info->complete_PaylodLen;
      block_info->lastBlockNum = 0;
      ret = QAPI_NO_ERROR;
      if(block_info->block_type == QAPI_COAP_BLOCK1_OPTION)
      {
         coap_set_header_size1((void *)pkt, block_info->complete_PaylodLen);
         coap_set_option((coap_packet_ext_t *)pkt, COAP_OPTION_SIZE1, NULL);
      }
      else if(block_info->block_type == QAPI_COAP_BLOCK2_OPTION)
      {
          coap_set_header_size((void *)pkt, block_info->complete_PaylodLen);
          coap_set_option((coap_packet_ext_t *)pkt, COAP_OPTION_SIZE, NULL);
      }
   }
   else if(dl_block_param)
   {
     //handle negotiation
     if(block_info->block_size > dl_block_param->size && block_info->lastBlockNum == 0)
     {
         skipBlock = block_info->block_size / dl_block_param->size;
         block_info->lastBlockNum = block_info->lastBlockNum + skipBlock - 1;
         block_info->block_size = dl_block_param->size;
         block_info->nextBlockP = block_info->nextBlockP + (block_info->block_size * block_info->lastBlockNum);
         block_info->numByteSent = block_info->numByteSent + (block_info->block_size * block_info->lastBlockNum);
     }
     if(dl_block_param->num ==  block_info->lastBlockNum + block2_flag) 
     {
       block_info->nextBlockP = block_info->nextBlockP + block_info->block_size;
       block_info->numByteSent = block_info->numByteSent + block_info->block_size;
       remainingSize = block_info->complete_PaylodLen - block_info->numByteSent;
       block_info->lastBlockNum = block_info->lastBlockNum + 1;
       bl_param.num = block_info->lastBlockNum;

       ret = QAPI_NO_ERROR;
     }
     else if(block2_flag && dl_block_param->num == block_info->lastBlockNum)
     {
       return QAPI_NO_ERROR;
     }
     else if(dl_block_param->num < block_info->lastBlockNum)
     {
       ret = QAPI_COAP_IGNORE;
     }
     else if( dl_block_param->num > block_info->lastBlockNum)
     {
       contextP->last_error = QAPI_REQ_ENTITY_INCOMPLETE_4_08;
	   contextP->mod_category = QAPI_NET_COAP_CAT;
       ret = QAPI_REQ_ENTITY_INCOMPLETE_4_08;
     }
   }
   if(ret == QAPI_NO_ERROR)
  {
      bl_param.more     = remainingSize > (block_info->block_size) ? 1 : 0;
      bl_param.size     = block_info->block_size;
      block_payload     = block_info->nextBlockP;
      block_payloadLen  = remainingSize < (block_info->block_size) ? remainingSize : (block_info->block_size);
 
       /* Set Block Header and Payload */
      if(block_info->block_type == QAPI_COAP_BLOCK1_OPTION)
      {
         coap_set_header_block1((void *)pkt, bl_param.num, bl_param.more, bl_param.size);
         coap_set_option((coap_packet_ext_t *)pkt, COAP_OPTION_BLOCK1, NULL);
      }
      else if(block_info->block_type == QAPI_COAP_BLOCK2_OPTION)
      {
          coap_set_header_block2((void *)pkt, bl_param.num, bl_param.more, bl_param.size);
          coap_set_option((coap_packet_ext_t *)pkt, COAP_OPTION_BLOCK2, NULL);
      }
      if(block_payload)
         coap_set_payload(pkt , (void *)block_payload, block_payloadLen);
      if(block_info->lastBlockNum != 0)
         pkt->mid = pkt->mid + 1;
    }

// store uplink blockwise transfer info, so user can get current snapshot using query API
   if(block_info && contextP->bl_info_param[UL_REQ] == block_info)
   {
       memset(&(contextP->block_transfer_info), 0 , sizeof(qapi_Coap_Block_Transfer_Info_t));
       contextP->block_transfer_info.valid_blockwise_session = true;
       contextP->block_transfer_info.mID = block_info->master_pkt->mid;
       contextP->block_transfer_info.block_size = block_info->block_size;
       contextP->block_transfer_info.first_bl_time = block_info->first_bl_time;
       contextP->block_transfer_info.numBlockSent = block_info->numBlockSent = block_info->lastBlockNum + 1;
       contextP->block_transfer_info.numBlockPending =  ((block_info->complete_PaylodLen + block_info->block_size -1 )/ block_info->block_size) - block_info->numBlockSent;
       if(block_info->token)
       {
          contextP->block_transfer_info.tokenLen = block_info->token_len;
          COAP_MEMCPY(contextP->block_transfer_info.token, COAP_TOKEN_LEN, block_info->token , block_info->token_len);
       }
   }

   ds_iot_ulog_high1("Block: returned with response_code = %d " , ret);
   return ret;
}


/* @fn     blockwise_rx_handler()
 * @brief  This function is used to store upcoming blockwise packet 
 * @param  contextP - pointer to coap client context
 *         pkt -     pointer to  coap packet 
 *         block_infoP - pointer to block info 
 *         dl_block_param - pointer to block info to downlink coap packet 
 * @return  QAPI_NO_ERROR, QAPI_COAP_IGNORE in case of success otherwise failure code
 */

qapi_Coap_Status_t blockwise_rx_handler(qapi_Coap_Session_Hdl_t          hdl, coap_block_session_info_t * block_info , qapi_Coap_Packet_t * dl_pkt , qapi_Coap_Block_Params_t * dl_bl_param )
{
   uint8_t * oldBuffer = NULL;
   size_t oldSize;
   qapi_Coap_Status_t ret = QAPI_NO_ERROR;
   void* memory_pool = NULL;

   if(block_info == NULL || dl_pkt == NULL || dl_bl_param == NULL)
   {
      ds_iot_ulog_high("blockwise_rx_handler: Null Params Recieved");
      ret = QAPI_INTERNAL_SERVER_ERROR_5_00;
      goto end;
   }
   ds_iot_ulog_high4("Block: store next blockwise packets bl_lastNum[%d], dl_num[%d], blcompleteLen[%d], dl_size[%d]",
                    block_info->lastBlockNum, dl_bl_param->num, block_info->complete_PaylodLen, dl_bl_param->size);
   
   if(dl_bl_param->num == 0)
   {
      block_info->complete_payload = (uint8_t *)malloc(dl_pkt->payload_len);
      if(block_info->complete_payload == NULL)
      {
        ret = QAPI_INTERNAL_SERVER_ERROR_5_00;
        goto end;
      }
      COAP_MEMCPY(block_info->complete_payload, dl_pkt->payload_len, dl_pkt->payload , dl_pkt->payload_len);
      block_info->complete_PaylodLen = dl_pkt->payload_len;
      if(QAPI_IS_OPTION(dl_pkt, QAPI_COAP_OPTION_CONTENT_TYPE))
          block_info->content_type = dl_pkt->content_type;
   }
   else if (block_info->lastBlockNum < dl_bl_param->num )
   {
      if((QAPI_IS_OPTION(dl_pkt, QAPI_COAP_OPTION_CONTENT_TYPE)) && (block_info->content_type != dl_pkt->content_type))
      {
         ds_iot_ulog_high("Block: content type mismatch");
         ret = QAPI_REQ_ENTITY_INCOMPLETE_4_08;
         goto end;
      }
      if(block_info->complete_PaylodLen != dl_bl_param->size * dl_bl_param->num)
      {
         ds_iot_ulog_high("Block: Invalid downlink block num");
         ret = QAPI_REQ_ENTITY_INCOMPLETE_4_08;
         goto end;
      }
      
      oldBuffer = block_info->complete_payload;
      oldSize =  block_info->complete_PaylodLen;
      block_info->complete_PaylodLen = oldSize + dl_pkt->payload_len;
      block_info->complete_payload = (uint8_t *)malloc (block_info->complete_PaylodLen);
      if(block_info->complete_payload == NULL)
      {
          ret = QAPI_INTERNAL_SERVER_ERROR_5_00;
          goto end;
      }
      COAP_MEMCPY(block_info->complete_payload, block_info->complete_PaylodLen, oldBuffer, oldSize);
      free(oldBuffer);
      COAP_MEMCPY(block_info->complete_payload + oldSize, dl_pkt->payload_len, dl_pkt->payload , dl_pkt->payload_len);
      block_info->lastBlockNum = dl_bl_param->num;
   }
   else
   {
      ret = QAPI_COAP_IGNORE;
      goto end;
   }
   if(dl_bl_param->more == 0)
   {
      memory_pool = mem_utility_Get_MemoryPool((void *)hdl, APP_COAP);
      if(dl_pkt->payload)
      {
         mem_utility_release_memory(memory_pool!=NULL, dl_pkt->payload);
         dl_pkt->payload = NULL;
      }
      dl_pkt->payload = (uint8_t *)mem_utility_Get_Memory_From_Pool(memory_pool, block_info->complete_PaylodLen);
      if(dl_pkt->payload == NULL)
      {
         ret = QAPI_INTERNAL_SERVER_ERROR_5_00;
         goto end;
      }
      COAP_MEMCPY(dl_pkt->payload , block_info->complete_PaylodLen , block_info->complete_payload , block_info->complete_PaylodLen);
      dl_pkt->payload_len =  block_info->complete_PaylodLen;
   }

end:
    ds_iot_ulog_high1("Block: returned with response_code = %d " , ret);
    return ret;
}


/* @fn     params_validity_check()
 * @brief  This function is used to check validity of blockwise packet
 * @param  ctxt - pointer to coap client context
 *         pkt -     pointer to  coap packet 
 *         btype - block1/block2 option
 *         bsize - blocksize 
 * @return  QAPI_OK in case of success otherwise failure code
 */

qapi_Status_t params_validity_check(client_context_t * ctxt, qapi_Coap_Packet_t * pkt , qapi_Coap_Block_Wise_Options_t btype , uint16_t bsize)
{
   qapi_Status_t ret = QAPI_OK;

  if(ctxt == NULL || pkt == NULL || COAP_IS_INVALIDE_PACKETS(pkt))
  {
    ds_iot_ulog_high("params_validity_check: Null Params Recieved");
	if(ctxt != NULL)
	{
		ctxt->last_error = QAPI_ERR_COAP_EINVALID_PARAM;
    	ctxt->mod_category = QAPI_NET_COAP_CAT;
	}
	ret = __QAPI_ERROR(QAPI_MOD_NETWORKING, QAPI_NET_COAP_EINVALID_PARAM);
    return ret;
  }

  if(QAPI_IS_OPTION(pkt, QAPI_COAP_OPTION_BLOCK2) && pkt->code == QAPI_COAP_GET)
  {
    if(ctxt->bl_info_param[DL_REQ])
    {
      ctxt->last_error = QAPI_ERR_COAP_BLOCK_SESSION_IN_PROGRESS;
      ctxt->mod_category = QAPI_NET_COAP_CAT;
      ret = __QAPI_ERROR(QAPI_MOD_NETWORKING, QAPI_NET_COAP_BLOCK_SESSION_IN_PROGRESS);
    }
    return ret;
  }

  if(btype == QAPI_COAP_BLOCK1_OPTION || btype == QAPI_COAP_BLOCK2_OPTION)
  {
    if(pkt->type == QAPI_COAP_TYPE_NON)
    {
       ctxt->last_error = QAPI_ERR_COAP_EINVALID_PARAM;
       ctxt->mod_category = QAPI_NET_COAP_CAT;
       ret = __QAPI_ERROR(QAPI_MOD_NETWORKING, QAPI_NET_COAP_EINVALID_PARAM);
       return ret;
    }
    if(IS_INVALID_BLOCKWISE_REQ(pkt , btype, bsize))
    {
       ctxt->last_error = QAPI_ERR_COAP_EINVALID_PARAM;
       ctxt->mod_category = QAPI_NET_COAP_CAT;
       ret = __QAPI_ERROR(QAPI_MOD_NETWORKING, QAPI_NET_COAP_EINVALID_PARAM);
       return ret;
    }
    if(ctxt->bl_info_param[UL_REQ])
    {
       ctxt->last_error = QAPI_ERR_COAP_BLOCK_SESSION_IN_PROGRESS;
       ctxt->mod_category = QAPI_NET_COAP_CAT;
       ret = __QAPI_ERROR(QAPI_MOD_NETWORKING, QAPI_NET_COAP_BLOCK_SESSION_IN_PROGRESS);
        return ret;
    }
    if(pkt->payload == NULL)
    {
	   ctxt->last_error = QAPI_ERR_COAP_EINVALID_PARAM;
	   ctxt->mod_category = QAPI_NET_COAP_CAT;
       ret = __QAPI_ERROR(QAPI_MOD_NETWORKING, QAPI_NET_COAP_EINVALID_PARAM);
    }
  }
   return ret;
}


/* @fn     get_blockWise_handling_status()
 * @brief  This function is used to identify blockwise packet needs to handle at application or coap level
 * @param  contextP - pointer to coap client context
 *         dl_pkt -     pointer to downlink coap packet 
 *         transaction transaction found or not for the packets
 * @return  handling status
 */

blockWise_handling_status_t get_blockWise_handling_status(client_context_t * contextP, coap_packet_t * dl_pkt, bool transaction)
{
    blockWise_handling_status_t status = BLOCKWISE_TRANSFER_REQUEST_FALSE;

    if(contextP == NULL || dl_pkt == NULL)
    {
        ds_iot_ulog_high("get_blockWise_handling_status: Null Params Recieved");
       return BLOCKWISE_INVALID_REQUEST;
    }

    if(QAPI_IS_OPTION(dl_pkt, QAPI_COAP_OPTION_BLOCK1) || QAPI_IS_OPTION(dl_pkt, QAPI_COAP_OPTION_BLOCK2))
    {
       status = BLOCKWISE_COAP_HANDLE_REQUEST;
    }
    if(status != BLOCKWISE_COAP_HANDLE_REQUEST || dl_pkt->type == COAP_TYPE_NON)
    {
        return BLOCKWISE_TRANSFER_REQUEST_FALSE;
    }
    if((dl_pkt->code == QAPI_COAP_PUT || dl_pkt->code == QAPI_COAP_POST || dl_pkt->code == QAPI_COAP_DELETE) && (dl_pkt->payload_len <= 0 || dl_pkt->payload == NULL))
    {
       return BLOCKWISE_TRANSFER_REQUEST_FALSE;
    }
    if(transaction == true)
    {
        if(QAPI_IS_OPTION(dl_pkt, QAPI_COAP_OPTION_BLOCK1) &&  
          (dl_pkt->type == COAP_TYPE_ACK && (dl_pkt->code == QAPI_CONTINUE_2_31 || dl_pkt->code == QAPI_CHANGED_2_04  || dl_pkt->code == QAPI_CREATED_2_01)))
        {
           status = BLOCKWISE_COAP_HANDLE_REQUEST;
        }
        else if(QAPI_IS_OPTION(dl_pkt, QAPI_COAP_OPTION_BLOCK2) &&
               ((dl_pkt->type == COAP_TYPE_CON && (dl_pkt->code == QAPI_COAP_GET || dl_pkt->code == QAPI_CONTENT_2_05))
                || (dl_pkt->type == COAP_TYPE_ACK  && dl_pkt->code == QAPI_CONTENT_2_05)))
        {
           status = BLOCKWISE_COAP_HANDLE_REQUEST;
  }
        else 
          return BLOCKWISE_INVALID_REQUEST;

        if(QAPI_IS_OPTION(dl_pkt, QAPI_COAP_OPTION_BLOCK2) && dl_pkt->code == QAPI_CONTENT_2_05 && !contextP->handle_downlink_blockwise)
  {
           status = BLOCKWISE_APPLICATION_HANDLE_REQUEST;
        }

        return status;
  }
  else
  {
       if(QAPI_IS_OPTION(dl_pkt, QAPI_COAP_OPTION_BLOCK1) &&  
          (dl_pkt->type == COAP_TYPE_CON && (dl_pkt->code == QAPI_COAP_PUT || dl_pkt->code == QAPI_COAP_POST)))
        {
           status = BLOCKWISE_COAP_HANDLE_REQUEST;
        }
        else if(QAPI_IS_OPTION(dl_pkt, QAPI_COAP_OPTION_BLOCK2) &&
               (dl_pkt->type == COAP_TYPE_CON && dl_pkt->code == QAPI_COAP_GET))
        {
            status = BLOCKWISE_COAP_HANDLE_REQUEST;
        }
        else 
          return BLOCKWISE_INVALID_REQUEST;

        if(QAPI_IS_OPTION(dl_pkt, QAPI_COAP_OPTION_BLOCK1) &&  !contextP->handle_downlink_blockwise)
        {
           status = BLOCKWISE_APPLICATION_HANDLE_REQUEST;
        }
  }
   return status;
}



/* @fn     block_step()
 * @brief  This function is used to step the ongoing blockwise transfer and clear on expiry time
 * @param  contextP - pointer to coap client context
 * @return void
 */

void block_step(client_context_t * contextP)
{
   coap_block_session_info_t * block_info = NULL;
   uint64_t current_time = 0;

   if(contextP == NULL)
     return;

   current_time = time_get_secs();
   if(contextP->bl_info_param[UL_REQ])
  {
      block_info =contextP->bl_info_param[UL_REQ];
      if(current_time - block_info->last_msg_send_time >= block_info->tv_expiry_time)
  {
          clean_block_info(contextP , &block_info);
          contextP->bl_info_param[UL_REQ] = NULL;
      }
   }
   if(contextP->bl_info_param[DL_REQ])
   {
      block_info =contextP->bl_info_param[DL_REQ];
   
      if(block_info->last_msg_send_time)
    {
         if(current_time - block_info->last_msg_send_time >= block_info->tv_expiry_time)
         {
            clean_block_info(contextP , &block_info);
            contextP->bl_info_param[DL_REQ] = NULL;
         }
    }
  }
}

