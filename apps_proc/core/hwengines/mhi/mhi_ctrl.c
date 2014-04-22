/**
  @file mhi_ctrl.c
  @brief
  This file contains the implementation of the MHI Control Channel Protocol 
  for sideband signals and proprietary messages  
    

*/
/*
===============================================================================

                             Edit History

 $Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------
02/16/13   nk      Created
===============================================================================
                   Copyright (c) 2014 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/

#include "mhi.h" 
#include "mhi_osal.h" 
#include "mhi_ctrl.h"

#ifndef PACKED
#define PACKED __packed  
#endif 

#define ASCII_CTRL 0x4C525443

#define CTRL_CH_OUTBOUND  18
#define CTRL_CH_INBOUND   19

#define CTRL_LINE_STATE_SIZE sizeof(uint32)
#define CTRL_SERIAL_STATE_SIZE sizeof(uint32)

#define NUM_RX_CTRL_MSG 10
#define PAYLOAD_RX_SIZE 8

#define RX_MSG_SIZE (sizeof(mhi_ctrl_msg_hdr_t)+PAYLOAD_RX_SIZE)

/** Pointer to control context */ 
static mhi_ctrl_ctx_t *ctx=NULL; 

/** Local function declarations */ 
void mhi_ctrli_msg_read_completion(mhi_return_t status); 
void mhi_ctrli_msg_write_completion(mhi_return_t status);
void mhi_ctrli_send_msg(mhi_ctrl_msg_id_t id, uint32 destid, uint8 *payload, uint32 size);

/** 
 * @brief      mhi_ctrli_msg_read_completion
 *
 *             Control channel read completion cb invoked by MHI 
 *              
 * 
 * @param[in]  status - result of the read operation
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_ctrli_msg_read_completion(mhi_return_t status)
{
   mhi_ctrl_ctx_t *ctx = status.userData; 
   mhi_ctrl_msg_hdr_t *hdr=(mhi_ctrl_msg_hdr_t*)status.payload.rw.buff; 
   mhi_ctrl_line_state_t *msg, old_state; 
   mhi_cb_t cb; 
   
   /* Validate userdata */
   if(NULL == ctx)
   { 
      mhi_osal_debug(MSG_FATAL, "mhi_ctrli_msg_read_completion: userdata is NULL");
      return; 
   }

   if(NULL == hdr) 
   {
      mhi_osal_debug(MSG_FATAL, "mhi_ctrli_msg_read_completion: buff is NULL");
      return;
   }

   /* If request failed log it */
   if(MHI_SUCCESS != status.status)
   {
      mhi_osal_debug(MSG_FATAL, "mhi_ctrli_msg_read_completion: Control message failed");
      return; 
   }
  
   /* Validate destid and pre-amble */   
   if(hdr->destid >= NUM_ID)
   {
      mhi_osal_debug(MSG_ERR, "mhi_ctrli_msg_read_completion: Invalid destid received"); 
   }
   else if(ASCII_CTRL != hdr->preamble)
   {
      mhi_osal_debug(MSG_ERR, "mhi_ctrli_msg_read_completion: Preamble invalid"); 
   }
   else
   {
      msg=(mhi_ctrl_line_state_t*)&hdr->msg_start; 

      mhi_osal_syncenter(&ctx->sync);

      /* Save latest ctrl line state */
      old_state=ctx->line_state[hdr->destid];
      ctx->line_state[hdr->destid]=*msg;

      /* If notification is enabled and line state changed notify upper layers */
      if( (ctx->notify_enable[hdr->destid/32]&(1<<(hdr->destid%32))) &&
            ((old_state & MHI_CTRL_LINE_STATE_DTR) != (ctx->line_state[hdr->destid] & MHI_CTRL_LINE_STATE_DTR)))
      {
         ctx->notify_func(ctx->userdata[hdr->destid],*msg);
      }

      mhi_osal_syncleave(&ctx->sync);
   }

   /* Re-submit the message */
   cb.userData=(void*)ctx; 
   cb.cbFunc=mhi_ctrli_msg_read_completion;

   if(MHI_PENDING != mhi_channel_read(CTRL_CH_OUTBOUND,(void*)hdr, RX_MSG_SIZE , NULL , &cb))
   {
      mhi_osal_debug(MSG_ERR, "mhi_ctrli_msg_read_completion: Unable to re-submit buffer"); 
   }
}

/** 
 * @brief      mhi_ctrli_msg_write_completion
 *
 *             Control channel write completion callback invoked by MHI
 *              
 * 
 * @param[in]  status - result of the write operation
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_ctrli_msg_write_completion(mhi_return_t status)
{
   /* Validate userdata */
   if(NULL == status.userData)
   { 
      mhi_osal_debug(MSG_FATAL, "mhi_ctrli_msg_write_completion: userdata is NULL");
      return; 
   }

   /* If request failed log it */
   if(MHI_SUCCESS != status.status)
   {
      mhi_osal_debug(MSG_FATAL, "mhi_ctrli_msg_write_completion: Control message failed");
      return;
   }

   /* Free the message */
   mhi_osal_free(status.userData);
}

/** 
 * @brief      mhi_ctrli_send_msg
 *
 *             Sends a control message over the control channel
 *              
 * 
 * @param[in]  id - Message id
 * @param[in]  destid - Destination ID associated with message
 * @param[in]  *payload - Payload message data
 * @param[in]  size - Size of payload
 *
 * @dependencies
 *             mhi_ctrl_init must be called first
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_ctrli_send_msg(mhi_ctrl_msg_id_t id, uint32 destid, uint8 *payload, uint32 size)
{
   mhi_cb_t cb; 
   mhi_ctrl_msg_hdr_t *hdr; 
   uint32 msg_size; 
   mhi_status_t ret; 

   msg_size=size+sizeof(mhi_ctrl_msg_hdr_t)-sizeof(uint32);
   hdr = mhi_osal_malloc(msg_size);
   
   if(NULL == hdr) 
   {
      mhi_osal_debug(MSG_ERR, "mhi_ctrli_send_msg: Unable to alloc header"); 
      return;
   }

   /* Populate hdr */
   hdr->preamble=ASCII_CTRL; 
   hdr->msgid=id;
   hdr->destid=destid; 
   hdr->size=size; 
  
   /* Populate payload */ 
   mhi_osal_memcpy((void*)&hdr->msg_start, payload, size);
    
   cb.userData=(void*)hdr; 
   cb.cbFunc=mhi_ctrli_msg_write_completion;

   if(MHI_PENDING != (ret = mhi_channel_write(CTRL_CH_INBOUND, (uint32*)hdr,msg_size, NULL, &cb)))
   {
      if( MHI_OP_FULL == ret)
      {
         mhi_osal_debug(MSG_ERR, "mhi_ctrli_send_msg: Channel full dropping message");
      }
      else
      {
         mhi_osal_debug(MSG_ERR, "mhi_ctrli_send_msg: Unknown error code"); 
      }

      mhi_osal_free(hdr); 
   }

   return; 
}

/** 
 * @brief      mhi_ctrl_init
 *
 *             Initializes MHI control layer
 *              
 * 
 * @param[in]  *ctrl_ctx - Pointer to control context where to store info
 * @param[in]  notify_func - Notification function to be used for control state changes
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_ctrl_init(mhi_ctrl_ctx_t *ctrl_ctx, mhi_ctrl_notify_func notify_func)
{
   uint32 i; 
   mhi_cb_t cb; 

   ctx=ctrl_ctx; 

   mhi_osal_syncinit(&ctx->sync); 

   mhi_osal_syncenter(&ctx->sync); 

   ctx->notify_func=notify_func; 
   if (MHI_SUCCESS != mhi_channel_open(CTRL_CH_INBOUND, NULL))
   {
      mhi_osal_debug(MSG_ERR, "mhi_ctrl_init: Failed to open ctrl channel"); 
      goto err;
   }
   if (MHI_SUCCESS != mhi_channel_open(CTRL_CH_OUTBOUND, NULL))
   {
      mhi_osal_debug(MSG_ERR, "mhi_ctrl_init: Failed to open ctrl channel"); 
      goto err; 
   }

   /* Allocate control messages for RX path */
   ctx->ctrl_pool=mhi_osal_malloc(NUM_RX_CTRL_MSG*RX_MSG_SIZE);

   if(NULL == ctx->ctrl_pool)
   {
      mhi_osal_debug(MSG_FATAL, "mhi_ctrl_init: Failed to allocate control pool");
      goto err; 
   }

   cb.cbFunc=mhi_ctrli_msg_read_completion; 

   /* Submit messages on RX path */
   for(i=0; i < NUM_RX_CTRL_MSG; i++)
   {
      cb.userData=(void*)ctx;

      if( MHI_PENDING != mhi_channel_read(CTRL_CH_OUTBOUND,(void*)((uint32)ctx->ctrl_pool+(i*RX_MSG_SIZE)), RX_MSG_SIZE , NULL , &cb))
      {
         mhi_osal_debug(MSG_FATAL, "mhi_ctrl_init: Failed to insert RX pool");  
         goto err; 
      }
   }

   mhi_osal_syncleave(&ctx->sync);
   mhi_osal_debug(MSG_HIGH, "mhi_ctrl_init: Successfully initialized control channels "); 
   return; 

err:
   mhi_osal_syncleave(&ctx->sync);
   return; 

}

/** 
 * @brief      mhi_ctrl_register_notify_cb
 *
 *             Register a control notification callback for a particular destination id
 *              
 * 
 * @param[in]  destid - Destination id with which to register a callback
 * @param[in]  *userdata - userdata to be supplied with callback
 *
 * @dependencies
 *             mhi_ctrl_init must be called first
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_ctrl_register_notify_cb(uint32 destid, void *userdata)
{
   mhi_ctrl_line_state_t state; 

   if(NULL == ctx)
   {
      mhi_osal_debug(MSG_MEDIUM, "mhi_ctrl_register_notify_cb: Ctrl channel is disabled returning"); 
      return; 
   }

   mhi_osal_syncenter(&ctx->sync); 
   
   if(destid > NUM_ID)
   {
      mhi_osal_debug(MSG_ERR, "mhi_ctrl_register_notify_cb: Invalid destid"); 
   }

   /* Enable notifications and send out current line state */
   ctx->notify_enable[destid/32]|=1<<(destid%32);
   ctx->userdata[destid]=userdata; 
   state=ctx->line_state[destid]; 
   mhi_osal_syncleave(&ctx->sync); 
 
   ctx->notify_func(ctx->userdata[destid],state);
}

/** 
 * @brief      mhi_ctrl_deregister_notify_cb
 *
 *             De-register a notification callback with a particular destination id
 *              
 * 
 * @param[in]  destid - Destination id 
 *
 * @dependencies
 *             mhi_ctrl_init must be called first
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_ctrl_deregister_notify_cb(uint32 destid)
{
   /* Disable notifications */
   mhi_osal_syncenter(&ctx->sync); 
   ctx->notify_enable[destid/32]&=~(1<<(destid%32));
   mhi_osal_syncleave(&ctx->sync); 
}

/** 
 * @brief      mhi_ctrl_send_line_state
 *
 *             Send line state
 *              
 * 
 * @param[in]  destid - Destination id associated with line state
 * @param[in]  msg - Line state
 *
 * @dependencies
 *             mhi_ctrl_init must be called first
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_ctrl_send_line_state(uint32 destid, mhi_ctrl_line_state_t msg)

{
   mhi_osal_debug(MSG_MEDIUM, "mhi_ctrl_send_line_state: Entry"); 
   mhi_ctrli_send_msg(MHI_CTRL_MSG_SET_LINE_STATE, destid, (uint8*)&msg, CTRL_LINE_STATE_SIZE); 

   return; 
}

/** 
 * @brief      mhi_ctrl_send_serial_state
 *
 *             Send control state
 *              
 * 
 * @param[in]  destid - Destination id associated with control state
 * @param[in]  msg - Control state
 *
 * @dependencies
 *             mhi_ctrl_init must be called first
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_ctrl_send_serial_state(uint32 destid, mhi_ctrl_serial_state_t msg)
{
   if (NULL == ctx)
   {
      mhi_osal_debug(MSG_MEDIUM, "mhi_ctrl_send_serial_state: Ctrl channel is not initialized returning"); 
      return;
   }

   mhi_osal_debug(MSG_MEDIUM, "mhi_ctrl_send_serial_state: Entry"); 
   mhi_ctrli_send_msg(MHI_CTRL_MSG_SET_SERIAL_STATE, destid, (uint8*)&msg, CTRL_SERIAL_STATE_SIZE); 

   return; 
}

/** 
 * @brief      mhi_ctrl_send_generic_cmd
 *
 *             Send a generic command
 *              
 * 
 * @param[in]  destid - Destination id 
 * @param[in]  *cmd - Pointer to command
 * @param[in]  size - Command size
 *
 * @dependencies
 *             mhi_ctrl_init must be called first
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_ctrl_send_generic_cmd(uint32 destid, uint8 *cmd, uint32 size)
{
   if (NULL == ctx)
   {
      mhi_osal_debug(MSG_MEDIUM, "mhi_ctrl_send_generic_cmd: Ctrl channel is not initialized returning"); 
      return;
   }

   mhi_osal_debug(MSG_MEDIUM, "mhi_ctrl_send_generic_cmd: Entry"); 

   mhi_ctrli_send_msg(MHI_CTRL_MSG_ENCAPSULATED_CMD, destid, (uint8*)cmd, size); 
   return; 
}

