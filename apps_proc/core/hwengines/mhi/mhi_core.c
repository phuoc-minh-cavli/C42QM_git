/**
  @file mhi_core.c
  @brief
  This file contains the core implementation of MHI 


*/
/*
===============================================================================

Edit History

$Header: 

Change revision history maintained in version system
===============================================================================
Copyright (c) 2013-2015 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===============================================================================
*/

#include "mhi_i.h"
#include "mhi_ring.h"
#include "mhi_mmio.h"
#include "mhi_link.h"
#include "mhi_core.h"
#include "mhi_util.h" 

/* Helper Macro used to sign extend 1 by NUM */
#define EXTEND(NUM) ((1<<((NUM)%32))-1) 

/* Macro that calculates SW interrupt mask from start to end (is exclusive) */
#define SWINTMASK(index,SWSTART, SWEND)  ( ((SWEND)/32<(index)?0x0: (SWEND)/32>(index)?0xFFFFFFFF:EXTEND(SWEND)) ^ ((SWSTART)/32<(index)?0x0: (SWSTART)/32>(index)?0xFFFFFFF:EXTEND(SWSTART)) )

/* Timeout for flushing pending DMA completions */
#define LINK_COMPLETION_TIMEOUT 1000 /* in ms */

/* Define Max Number of event rings */
#define MAX_ER 255

/** Local variables **/

/** Local Prototypes **/
void    mhi_core_pendingdb_clear(ring_t *ring);
boolean mhi_core_pendingdb_isset(ring_t *ring);

void    mhi_core_scheduler(void);
void    mhi_core_sys_error(void);

void    mhi_core_isr(mhi_ctx_t *ctx); 

void    mhi_core_handle_single_ring_completion(void);
void    mhi_core_handle_single_ring_acks(void);
void    mhi_core_process_single_ring(void);
uint32  mhi_core_process_ring_acks(ring_t *er);

uint32  mhi_core_process_ring_completion(ring_t *ring);
void    mhi_core_process_ring_cb(ring_t *ring, mhi_link_async_req_t *linkReq);
uint32  mhi_core_process_tr_element(ring_t *ring, mhi_ring_element_t *element, event_t *event);
uint32  mhi_core_process_tre(ring_t *ring , mhi_ring_element_t *el );
uint32  mhi_core_process_command(ring_t *ring, mhi_ring_element_t *el, event_t *event );
void    mhi_core_set_ringstate(ring_t *ring, ring_state_t state);


void    mhi_core_abort_linkreqs(void);
uint32  mhi_core_issue_linkreq(uint32 index, mhi_link_async_req_t *linkReq, mhi_op_t op );
boolean mhi_core_are_link_completions_pending(ring_t * ring);

uint32  mhi_core_fetch_ch_ctx(uint32 chid, uint32 num);
uint32  mhi_core_flush_ch_ctx(uint32 chid, uint32 num);
uint32  mhi_core_flush_ctx(ring_t *ring);

uint32  mhi_core_validateHostAddress(uint64 ptr, uint32 index);
void    mhi_core_thread_entry(void *ctx);

uint32  mhi_core_send_interrupt(ring_t *er, boolean blocked);
uint32  mhi_core_flush_interrupts(void);
void    mhi_core_process_idle_tasks(void);

uint32 mhi_core_log_ring_ids_on_Q(list_t *q, uint32 offset);

void    mhi_core_recover_ch(uint32 chid);

uint32 mhi_core_init_event_ring(uint32 erNum);
uint32 mhi_corei_send_event(uint32 erNum, mhi_ring_element_t *el, uint32 trigger_msi, boolean noFlush );

#ifndef MHI_BOOT
void    mhi_core_send_delayed_interrupt(int32 time_ms, mhi_timer_user_data data);
#endif

/* Externalized test functions defined in mhi_test.c */
extern void mhi_pm_test_init_sw_link_inact_timer(void);
extern void mhi_pm_state_loop_test(void);
extern void mhi_loopback_test(void);
extern void mhi_pm_test_intr_check(void);

/** 
 * @brief      mhi_core_link_rw_cb
 *
 *             Main callback function used for async link r/w request completions 
 *              
 * 
 * @param[in]  ret - MHI Link return type describing the operation and status
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
void mhi_core_link_rw_cb(mhi_link_ret_t ret)
{
   mhi_link_async_req_t *linkReq;
   mhi_addr_t addr={0};

   if (NULL == ret.userdata)
   {
      mhi_osal_debug(MSG_FATAL,"mhi_core_link_rw_cb: userdata is NULL, return from cb");
      return;
   }

   linkReq=(mhi_link_async_req_t*)ret.userdata;
   addr.va=linkReq->local_addr;

   mhi_osal_debug(PROFILING_LOG|MSG_MEDIUM,"mhi_core_link_rw_cb: ch_id=%d, local_addr=0x%x, size=%d"
                   ,linkReq->ring->id,linkReq->local_addr,linkReq->size);

   /* Invalidate the cache for read buffer */
   if (MHI_READ == linkReq->op || MHI_READM == linkReq->op)
   {
      mhi_osal_cache_ctrl(MHI_CACHE_OP_INVALIDATE,&addr,linkReq->size);
   }

   if(MHI_SUCCESS == ret.status)
      linkReq->status = SUCCESS; 
   else
      linkReq->status = ERROR;

   mhi_osal_debug(MSG_LOW,"mhi_core_link_rw_cb: ring %d, linkReq 0x%x, local_addr 0x%x",
                                 linkReq->ring->id,linkReq,linkReq->local_addr);
   
   mhi_core_process_ring_cb(linkReq->ring, linkReq); 
}

/** 
 * @brief      mhi_core_queue_pending_ring
 *
 *             Queues a ring on to the pending ring queue
 *              
 * 
 * @param[in]  *ring - Ring 
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
void mhi_core_queue_pending_ring(ring_t *ring)
{
   ring_t *current = NULL; 

   /* Find where to insert based on priority */ 
   do
   {
      mhi_q_get_next(&mhiCtx.pendingRingQ, (list_t**)&current); 

      if(NULL != current)
      {
         if( ring->priority < current->priority)
            break; 
      }
   }while( NULL != current); 

   if(NULL == current) 
      mhi_q_queue(&mhiCtx.pendingRingQ, &ring->list);  
   else
      mhi_q_insert_before(&current->list, &ring->list);  

   return; 
}

/**
 * @brief       mhi_core_signal_worker_thread
 *
 *              Signal MHI worker thread                
 *
 *
 * @dependencies
 *              Can only be called after MHI init
 * 
 * @return       
 *              None
 * 
 * @sideeffect  None
 */
void mhi_core_signal_worker_thread(void)
{
   mhi_osal_eventsignal(mhiCtx.hNotify);
}

#ifndef MHI_BOOT
/** 
 * @brief      mhi_core_send_delayed_interrupt
 *
 *             Sends a delayed interrupt for an event ring to host upon timer expiration 
 *              
 * 
 * @param[in]  time_ms - time elapsed
 * @param[in]  data - user data
 *
 * @dependencies
 *             Must not be called when mhi is suspended
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_core_send_delayed_interrupt(int32 time_ms, mhi_timer_user_data data)
{
   ring_t *er=(ring_t*)data; 

   if(NULL == er)
   {
      mhi_osal_debug(MSG_FATAL, "mhi_core_send_delayed_interrupt: Received null ring, fatal error");
      return;
   } 

   /* Clear the pending int flag and send the MSI if it is set */
   mhi_osal_syncenter(&er->sync);

   if(FALSE != mhiCtx.erCtx[er->id].pending_msi)
   {
      mhiCtx.erCtx[er->id].pending_msi=FALSE;

      if(mhi_link_triggerinterrupt(er->ctx->ev.msivec))
      {
         mhi_osal_syncleave(&er->sync);
         mhi_osal_debug(MSG_FATAL, "mhi_core_send_delayed_interrupt: Failed to generate MSI interrupt");
      }
   }

   mhi_osal_syncleave(&er->sync);
}
#endif

/** 
 * @brief      mhi_core_flush_interrupts
 *
 *             Flushes all pending moderated interrupts. Function will continue to send 
 *             interrupts even if it encounters an error in order to stop all the timers 
 *              
 *
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             SUCCESS if successful, ERROR otherwise
 * 
 * @sideeffect None 
 */
uint32 mhi_core_flush_interrupts(void)
{
   uint32 ret, i, status = SUCCESS;

   /* Stop all the timers and fire any pending event interrupts so 
    * we don't get them during low power states */
   for(i=0; i<mhiCtx.mhiConfig.nEr; i++)
   {
      ring_t *er=&mhiCtx.er[i];

      if(er->state != RING_UNINIT)
      {
         mhi_osal_syncenter(&er->sync);
         if(0 != er->ctx->ev.intmodt && FALSE != mhiCtx.erCtx[i].pending_msi)
         {
            ret = mhi_core_send_interrupt(er, FALSE);

            if(SUCCESS != ret)
            {
               mhi_osal_debug(MSG_ERR, "mhi_core_send_event: Failed to generate MSI interrupt");
               status = ERROR; 
            }
         }
         mhi_osal_syncleave(&er->sync);
      }
   }

   return status; 
}

/** 
 * @brief      mhi_core_send_interrupt
 *
 *             Send interrupt for specified event ring. In case of INTMOD != 0, If there is a pending interrupt, and the 
 *             interrupt is not blocked (BEI = 0), stop the timer and send the interrupt. If there is no pending 
 *             interrupts, start a timer and mark an interrupt pending. In case of INTMOD ==0, send the interrupt if not 
 *             blocked otherwise don't send an interrupt/
 *              
 * 
 * @param[in]  *er  - Event ring
 * @param[in]  blocked - Whether MSI is blocked or not 
 *
 * @dependencies
 *             To be only used for event ring processing. Event Ring sync section should be held.
 *              
 * 
 * @return       
 *             SUCCESS if successful otherwise ERROR
 * 
 * @sideeffect None 
 */
uint32 mhi_core_send_interrupt(ring_t *er, boolean blocked)
{
   if(!blocked)
   {
      /* If interrupt moderation is enabled and timer is running, stop the timer and generate an interrupt */
      if(0 != er->ctx->ev.intmodt && FALSE != mhiCtx.erCtx[er->id].pending_msi)
      {
#ifndef MHI_BOOT
         mhi_osal_timer_stop(&mhiCtx.erCtx[er->id].intmod_timer); 
#endif
         mhiCtx.erCtx[er->id].pending_msi=FALSE;
      }

      if(mhiCtx.mhi_link_info.mhi_link_hw_enabled && mhi_link_triggerinterrupt(er->ctx->ev.msivec))
      {
         mhi_osal_debug(MSG_ERR, "mhi_core_send_interrupt: Failed to generate MSI interrupt");
         return SYSERR; 
      }
   }
   else
   {
      /* If moderation is enabled and there are no pending interrupts, start a timer. In BOOT mode, 
       * instead of starting timer fire the interrupt if intmod is not 0 as we don't support it */
      if(0 != er->ctx->ev.intmodt &&  FALSE == mhiCtx.erCtx[er->id].pending_msi)
      {
#ifndef MHI_BOOT
         mhiCtx.erCtx[er->id].pending_msi=TRUE; 

         mhi_osal_timer_start(&mhiCtx.erCtx[er->id].intmod_timer, 
                               mhi_core_send_delayed_interrupt,
                               (mhi_timer_user_data)er, 
                               er->ctx->ev.intmodt, 0);
#else
         if(mhi_link_triggerinterrupt(er->ctx->ev.msivec))
         {
            mhi_osal_debug(MSG_ERR, "mhi_core_send_interrupt: Failed to generate MSI interrupt");
            return SYSERR; 
         }
#endif
      }
   }

   return SUCCESS;
}

/** 
 * @brief      mhi_core_init_event_ring
 *
 *             Initialize event ring context. Setup intmod timer if needed.
 *             Enable event ring doorbell interrupt.
 *             Perform any other initialization needed for the event ring. 
 *              
 * 
 * @param[in]  erNum  - Event ring number
 *
 * @dependencies
 *             mhi_core_cache_hostconfig 
 * 
 * @return       
 *             SUCCESS if successful 
 *             SYSERR if system error occurred
 * 
 * @sideeffect  None 
 */
uint32 mhi_core_init_event_ring(uint32 erNum)
{
   ring_t *er;
   uint32 ret=SUCCESS;

   er=&mhiCtx.er[erNum];  
  
   if(RING_UNINIT == er->state) 
   {
      if(SUCCESS != mhi_ring_init_ctx(er, (mhi_ring_ctx_t*)&mhiCtx.lErCtxBase[erNum]))
      {
         mhi_osal_debug(MSG_ERR, "mhi_core_init_event_ring: Failed to init ring ctx");
         ret=SYSERR;
         goto exit;
      }

#ifndef MHI_BOOT
      /* If INTMOD is set, set up a timer */ 
      if(0 != mhiCtx.lErCtxBase[erNum].intmodt)
      {
         mhi_osal_timer_init(&mhiCtx.erCtx[erNum].intmod_timer); 
         mhiCtx.erCtx[erNum].pending_msi=FALSE;
      }
#endif

      /* Enable ERDB interrupts */
      mhi_mmio_enableerdbinterrupt(erNum); 
   }
   
exit:
   return ret;  
}


/** 
 * @brief      mhi_core_send_event
 *
 *             Populate an event in event ring and notify the HOST 
 *              
 * 
 * @param[in]  erNum  - Event ring number
 * @param[in]  *el    - Event element to send
 * @param[in]  trigger_msi - Whether to trigger an msi or not
 *
 * @dependencies
 *              
 * 
 * @return       
 *              SUCCESS if successful 
 *              SYSERR if system error occurred
 * 
 * @sideeffect  None 
 */
uint32 mhi_core_send_event(uint32 erNum, mhi_ring_element_t *el, uint32 trigger_msi)
{
   return mhi_corei_send_event(erNum, el, trigger_msi, FALSE);
}

/** 
 * @brief      mhi_corei_send_event
 *
 *             Populate an event in event ring and notify the HOST 
 *              
 * 
 * @param[in]  erNum  - Event ring number
 * @param[in]  *el    - Event element to send
 * @param[in]  trigger_msi - Whether to trigger an msi or not
 * @param[in]  noFlush - Populate the event in local cached ring only, do not flush to host
 *
 * @dependencies
 *              
 * 
 * @return       
 *              SUCCESS if successful 
 *              SYSERR if system error occurred
 * 
 * @sideeffect  None 
 */
uint32 mhi_corei_send_event(uint32 erNum, mhi_ring_element_t *el, uint32 trigger_msi, boolean noFlush )
{
   ring_t *er;
   uint32 ret=SUCCESS;

   mhi_osal_debug(PROFILING_LOG|MSG_LOW,"mhi_core_send_event: send_event=start");

   er=&mhiCtx.er[erNum];

   mhi_osal_debug(MSG_HIGH,"mhi_core_send_event: Generating event on event ring: %x", er->id); 

   mhi_osal_syncenter(&er->sync);

   /* Do not generate event for standalone power management testing on Virtio */
#if !defined(MHI_PM_STATE_TEST) || !defined(VIRTIO) 

   if(RING_UNINIT == er->state) 
      mhi_osal_debug(MSG_FATAL,"mhi_corei_send_event: Event ring %d not initialized",er->id);
   
   /* Get pending DB's */  
   mhi_mmio_get_erdbinterrupts(&mhiCtx.int_erdb, NULL);

   /* Process any acks if any */
   mhi_core_process_ring_acks(er);

   /* Insert element into ring */
   ret=mhi_ring_insert_element(er,el,FALSE==er->reliable&&mhi_core_pendingdb_isset(er));
   if (SUCCESS != ret && RING_FULL != ret) 
   {
      mhi_osal_debug(MSG_ERR, "mhi_corei_send_event: Failed to insert ring element to event ring, system error");  
      ret=SYSERR;
      goto exit;
   }
   
   /* Flush the event immediately if event was inserted successfully and noFlush is FALSE */
   if (SUCCESS == ret && !noFlush)
   {
      mhi_core_flush_cached_events(erNum, trigger_msi);
   }
   
#else 
   /* Do not generate event for standalone power management testing on Virtio */
   NOT_USED(er);
   NOT_USED(ctx);
   NOT_USED(oldOffset);
#endif 

exit:
   mhi_osal_syncleave(&er->sync);

   mhi_osal_debug(PROFILING_LOG|MSG_LOW,"mhi_core_send_event: send_event=end");

   return ret; 
}

/** 
 * @brief      mhi_core_flush_cached_events
 *
 *             Update the host's ring with events accumulated in the device's cached event ring
 * 
 * @param[in]  erNum  - Event ring number
 * @param[in]  trigger_msi - Whether to trigger an msi or not
 *
 * @dependencies
 *              
 * 
 * @return       
 *              SUCCESS if successful 
 *              SYSERR if system error occurred
 * 
 * @sideeffect  None 
 */
uint32 mhi_core_flush_cached_events(uint32 erNum, uint32 trigger_msi)
{
   ring_t *er=&mhiCtx.er[erNum];
   uint32 cachedOffset_start, cachedOffset_end;
   uint32 num_events_to_burst=0;
   uint32 ret=MHI_SUCCESS;

   mhi_osal_syncenter(&er->sync);

   if (RING_UNINIT == er->state)
      mhi_osal_debug(MSG_FATAL,"mhi_core_flush_cached_events: Event ring %d not initialized",er->id);

   /* Number of events cached is the difference between read pointer offset in 
    * the event ring context and the actual read pointer offset in the local event ring */   
   cachedOffset_start=(er->ctx->gen.rp-er->ctx->gen.rbase)/sizeof(mhi_ring_element_t);
   cachedOffset_end=er->rdOffset;
   num_events_to_burst=(cachedOffset_end-cachedOffset_start+er->rSize)%er->rSize;

   if (num_events_to_burst)
   {
      mhi_addr_t addr={0};
      uint32 size=((cachedOffset_end>cachedOffset_start)?cachedOffset_end-cachedOffset_start:er->rSize-cachedOffset_start)*sizeof(mhi_ring_element_t);
      
      /* Burst elements to host */
      addr.va=(uint32)er->lRingBase + cachedOffset_start*sizeof(mhi_ring_element_t);
      if(mhi_link_write(ADDR_CTRL,
                       er->ringBase+sizeof(mhi_ring_element_t)*cachedOffset_start,
                       &addr,
                       size,
                       NULL))
      {
         ret=SYSERR;
         goto exit;
      }
                           
      /* Handle wrap around */  
      if (0 != cachedOffset_end && cachedOffset_end < cachedOffset_start)
      {
         addr.va=(uint32)er->lRingBase;
         if(mhi_link_write(ADDR_CTRL,
                     er->ringBase,
                     &addr,
                     (cachedOffset_end)*sizeof(mhi_ring_element_t),
                     NULL))
         {
            ret=SYSERR;
            goto exit;
         }                       
      }
    
      /* Update read pointer */
      er->ctx->gen.rp=(er->rdOffset*sizeof(mhi_ring_element_t))+er->ctx->gen.rbase;
      addr.va=(uint32)&er->ctx->gen.rp;
      addr.pa=VA2PA(addr.va, mhiCtx.pool_uncached.va, mhiCtx.pool_uncached.pa);
      if(mhi_link_write(ADDR_CTRL,
                         mhiCtx.ecabap+sizeof(mhi_generic_ctx_t)*er->id+((uint32)&er->ctx->gen.rp-(uint32)er->ctx), 
                         &addr, 
                         sizeof(er->ctx->gen.rp), 
                         NULL))
      {
         ret=SYSERR;
         goto exit;
      }                         

      /* Increment counters only after bursting is complete */
      mhiCtx.evCounter[erNum]+=num_events_to_burst; 

      mhi_osal_debug(MSG_HIGH, "mhi_core_flush_cached_events: Sending events: 0x%x%x through 0x%x%x",
                              QW2DW(mhiCtx.evCounter[erNum])-num_events_to_burst,QW2DW(mhiCtx.evCounter[erNum]));
   }

   if(mhi_core_send_interrupt(er, !trigger_msi))
   {
      mhi_osal_debug(MSG_ERR, "mhi_core_send_event: Failed to generate MSI interrupt");
      ret=SYSERR;  
   }

exit:
   mhi_osal_syncleave(&er->sync);
   if (SUCCESS != ret)
      mhi_osal_debug(MSG_ERR, "mhi_core_flush_cached_events: Failed to send event");
   return ret;
}

/** 
 * @brief      mhi_core_process_ch_requestopen
 *
 *             Process any channel open requests from clients 
 *              
 *
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             SUCCESS if operation succeeded  
 * 
 * @sideeffect None 
 */
uint32 mhi_core_process_ch_requestopen(void)
{
   uint32 i; 
   mhi_request_t *req; 
   ch_t *ch; 
   mhi_return_t ret; 

   /* Scan channels and find ones that are pending open. 
    * Signal them as MHI is now in M0 state */
   for(i = 0; i< mhiCtx.mhiConfig.nCh; i++)
   {
      ch=&mhiCtx.ch[i];
      if(CH_PENDING_OPEN == ch->ch_client_state)
      {
         /* No need to grab sync as pending clients cannot submit any new commands until it gets the open indication. Also at this point there 
            can be no new pending clients as state is already changed to RUNNING and there is a mutex protection around this */
         if(mhi_q_dequeue(&ch->request_q,(list_t**)&req))
         {
            mhi_osal_debug(MSG_FATAL, "mhi_core_process_ch_requestopen: Pending Open channel has no request. Generating system error"); 
            return ERROR; 
         }

         /* Make sure first request is an open request */
         if(MHI_OPEN != req->op)
         {
            mhi_osal_debug(MSG_FATAL, "mhi_core_process_ch_requestopen: First request for a un-open channel should be open. Generating system error"); 
            return ERROR; 
         }

         ret.status=MHI_SUCCESS; 
         ret.userData=req->cb.userData; 
         ret.op=req->op; 

         ch->ch_client_state=CH_OPENED;  

         /* Put the request back on free queue */
         mhi_q_queue(&ch->request_free_q,(list_t*)req);

         /* Do the CB */
         req->cb.cbFunc(ret); 
      } 
   }

   return SUCCESS; 
}

/**
 * @brief       mhi_core_send_exec_env_change_event
 *
 *              Send execution environment change event to the host
 *
 *
 * @param[in]   excenv - Current execution environment (SBL or AMSS)
 *
 * @dependencies
 *              None
 * 
 * @return       
 *              None  
 * 
 * @sideeffect  None 
 */
void mhi_core_send_exec_env_change_event(uint32 excenv)
{
  mhi_ring_element_t event;

  event.exec_env_ChgEvent.exec_env = excenv; 
  event.exec_env_ChgEvent.type=MHI_RING_EL_TYPE_EXEC_ENV_CHG;
  if (SUCCESS != mhi_core_send_event(PRIMARY_ER,&event,1))
  {
     mhi_core_sys_error();
  }
}

/** 
 * @brief      mhi_core_abort_linkreqs
 *
 *             Abort all pending linkreqs 
 *              
 *
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
void mhi_core_abort_linkreqs(void)
{
   int i;

   mhi_link_async_req_t *req = (mhi_link_async_req_t*)mhiCtx.linkReq;

   for(i=0; i < mhiCtx.mhi_core_tgt_config->max_linkreq; i++, req++)
   {
      if(req->status == PENDING)
      {
         req->status = ABORT;
         mhi_core_process_ring_cb(req->ring, req);
      }
   }
}


/** 
 * @brief      mhi_core_reset
 *
 *             Resets the MHI Core layer. This is used to put MHI in reset state. 
 *              
 *
 *
 * @dependencies
 *             mhi_core_init must be first called
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_core_reset()
{
   uint32 i;

   mhi_osal_debug(MSG_HIGH, "mhi_core_reset: Resetting MHI Core");

   /* Abort all link requests */
   mhi_core_abort_linkreqs();

   /* Complete all ring linkreq's */
   mhi_core_flush_link_completions(NULL);

   /* Complete all ack's */
   while(!mhi_q_is_queue_empty(&mhiCtx.waitAckRingQ))
   {
      mhi_core_handle_single_ring_acks();
   }

   /* Reset the power management module */
   mhi_pm_reset();

   /* Reset the MHI state machine */
   mhi_sm_reset();

   /* Reset rest of MMIO registers */
   mhi_mmio_reset();


   /* Reset local data structures */
   mhi_osal_memset(mhiCtx.lChCtxBase,0,sizeof(mhi_ch_ctx_t)*mhiCtx.mhiConfig.nCh);
   mhi_osal_memset(mhiCtx.lErCtxBase,0,sizeof(mhi_ev_ctx_t)*mhiCtx.mhiConfig.nEr);

   /* Initialize the counter that tracks the number of pending completions */
   mhiCtx.num_completions_pending = 0;

   /* Initialize event to NULL, will be created as needed */
   mhiCtx.hNoMoreCompletions = NULL;

   /* Reset work pending flag */
   mhiCtx.is_mhi_work_pending = FALSE;

   /* Clear the reset bit  */ 
   mhi_mmio_clear_reset();

   /* Reset the link */
   mhi_link_reset();

   /* Reset the hardware accelerator */
   mhi_hw_acc_reset();

   /* Reset SYSERR pending flag */
   mhiCtx.is_syserr_pending = FALSE;

   /* Initialize Queues */
   mhi_q_init(&mhiCtx.pendingRingQ);   
   mhi_q_init(&mhiCtx.completeRingQ);
   mhi_q_init(&mhiCtx.waitErRingQ); 
   mhi_q_init(&mhiCtx.waitLinkRingQ);
   mhi_q_init(&mhiCtx.waitAckRingQ);

   /* Mark all rings as uninitialized */
   mhi_ring_reset(&mhiCtx.cr);

   for(i=0; i < mhiCtx.mhiConfig.nCh; i++)
   {
      mhi_ring_reset(&mhiCtx.tr[i]);
   }

   mhi_osal_free(mhiCtx.er);
   mhi_osal_free(mhiCtx.erCtx);

   /* Clear reliable ER */
   mhiCtx.reliableEr[0]=0;
   mhiCtx.reliableEr[1]=0;
   mhiCtx.reliableEr[2]=0;
   mhiCtx.reliableEr[3]=0;

   /* Set MHI state to ready, so host can start initializing MMIO */ 
   mhi_sm_set_mhiState(MHI_STATE_READY);

   mhi_osal_debug(MSG_HIGH, "mhi_core_reset: Reset complete"); 
   return;
}
/** 
 * @brief      mhi_core_save_ring
 *
 *             Saves the local ring to host memory. This is used for transitioning between execution 
 *             environments. 
 *              
 * 
 * @param[in]  *ring - Ring to save
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             SUCCESS if operation succeeded, ERROR otherwise  
 * 
 * @sideeffect None 
 */
uint32 mhi_core_save_ring(ring_t *ring)
{
   /* If a DB is set update the write offset */
   if(mhi_core_pendingdb_isset(ring))
   {
      if (SUCCESS != mhi_ring_update_wroffset(ring))
      {
        return ERROR;  
      }
   }

   /* Update the rings context. Note this is the MHI context not the logical ring object context */
   mhi_ring_update_ctx(ring);

   /* Flush the context to host memory */
   if(SUCCESS != mhi_core_flush_ctx(ring))
   {
      return ERROR; 
   }

   return SUCCESS;
}

/** 
 * @brief      mhi_core_deinit
 *
 *             De-initializes the MHI Core. This is primarily used during boot to clean up. 
 *             Clean up includes backing up the command ring and event rings to host memory. 
 *              
 *
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             SUCCESS if operation succeeded  
 * 
 * @sideeffect None 
 */
uint32 mhi_core_deinit(void)
{
   uint32 i; 
   ring_t *ring; 

#ifndef MHI_BOOT
   NOT_USED(i);
   NOT_USED(ring); 

   mhi_osal_debug(MSG_FATAL, "mhi_core_deinit: MHI Core Deinit not supported"); 
#else

   /* Backup the command ring */
   ring=&mhiCtx.cr;
   mhi_core_save_ring(ring);

   /* Back-up event rings */
   for(i = 0; i < mhiCtx.mhiConfig.nEr; i++)
   {
      ring=&mhiCtx.er[i];

      if(RING_UNINIT != ring->state)
      {
         mhi_core_save_ring(ring);
      }
   }
#endif

   return SUCCESS; 
}

/** 
 * @brief      mhi_core_init
 *
 *             Initialize the MHI Core
 *              
 *
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             SUCCESS if operation succeeded  
 * 
 * @sideeffect None 
 */
uint32 mhi_core_init(void)
{
   uint32 i, ret=ERROR;  
   mhi_interrupts_t chdb;
   mhi_state_t mhi_initial_state;   
   
   /* Set the EXECENV */
   mhi_mmio_set_execenv(MHI_ENV);

   /* Initialize MHI power management */
   if (SUCCESS != mhi_pm_init(&mhiCtx.mhi_pm_info))
   {
      mhi_osal_debug(MSG_ERR, "mhi_init: Failed to initialize MHI power management");
      goto err; 
   }

   /* Initialize the MHI state machine */
   mhi_initial_state=mhi_mmio_get_mhistate(); 
   mhiCtx.mhi_sm_info.mhiSync = &mhiCtx.mhiSync;
   mhi_sm_init(&mhiCtx.mhi_sm_info);

   /* Register for link notifications now that the sm is up */
   mhi_link_register_notifications_cb();

   /* Initialize MHI hardware accelerator */
   mhiCtx.mhi_hw_acc_info.p_mhictrl_pa = &mhiCtx.mhiLocalAddr.mhictrl.pa;
   mhiCtx.mhi_hw_acc_info.p_mhidata_pa = &mhiCtx.mhiLocalAddr.mhidata.pa;

   if (SUCCESS != mhi_hw_acc_init(&mhiCtx.mhi_hw_acc_info))
   {
      mhi_osal_debug(MSG_ERR, "mhi_core_init: Failed to initialize MHI hardware accelerator");
      goto err;
   }
   /* Trigger an interrupt for EXEC ENV change */
   mhi_link_triggerinterrupt(1); 

   /* Allocate local MHI channel and event contexts */
   mhi_osal_mallocmemregion(&mhiCtx.pool_uncached,
                            (sizeof(mhi_ch_ctx_t)*mhiCtx.mhiConfig.nCh)+
                            (sizeof(mhi_ev_ctx_t)*MAX_ER)+
                            sizeof(mhi_cr_ctx_t),
                            MHI_MEMORY_MAPPING);

   if( NULL == mhiCtx.pool_uncached.handle)
   {
      mhi_osal_debug(MSG_ERR, "mhi_core_init: Could not allocate uncached pool");
      goto err;
   }

   /* Set up the contexts in the pool */

   mhiCtx.lCrCtxBase=(mhi_cr_ctx_t*)mhiCtx.pool_uncached.va;
   mhiCtx.lChCtxBase=(mhi_ch_ctx_t*)(mhiCtx.lCrCtxBase+1); 
   mhiCtx.lErCtxBase=(mhi_ev_ctx_t*)(mhiCtx.lChCtxBase+mhiCtx.mhiConfig.nCh);
   
   /* Allocate logical channel context */ 
   if( NULL == (mhiCtx.ch = mhi_osal_malloc(mhiCtx.mhiConfig.nCh*sizeof(ch_t))) )
   {
      mhi_osal_debug(MSG_ERR, "mhi_core_init: Could not allocate local channel context");
      goto err;
   }
   mhi_osal_memset(mhiCtx.ch, 0 , mhiCtx.mhiConfig.nCh*sizeof(ch_t));

   /* Allocate generic ring structures. Event rings are allocated after M0 as they are host configurable */ 
   if( NULL == (mhiCtx.tr = mhi_osal_malloc(sizeof(ring_t)*mhiCtx.mhiConfig.nCh)) )
   {
      mhi_osal_debug(MSG_ERR, "mhi_core_init: Could not allocate local generic ring structure");
      goto err;
   }

   mhi_osal_memset(mhiCtx.tr, 0, sizeof(ring_t)*mhiCtx.mhiConfig.nCh);

   /* Initialize all the ring objects */
   mhi_ring_init(0, &mhiCtx.cr, RING_TYPE_CR, mhi_core_process_command);

   for(i = 0; i <  mhiCtx.mhiConfig.nCh ; i++)
   {
      mhi_ring_init(i, &mhiCtx.tr[i], RING_TYPE_TR, mhi_core_process_tr_element);
   }

   if(NULL == (mhiCtx.evCounter = mhi_osal_malloc(sizeof(*mhiCtx.evCounter)*MAX_ER)))
   {
      mhi_osal_debug(MSG_ERR, "mhi_core_init: Could not allocate event counters");
      goto err;
   }

   mhi_osal_memset(mhiCtx.evCounter, 0, sizeof(*mhiCtx.evCounter)*MAX_ER);


   /* Allocate Link requests and insert to free queue  */
   mhiCtx.linkReq=mhi_osal_malloc(sizeof(mhi_link_async_req_t)*mhiCtx.mhi_core_tgt_config->max_linkreq); 
   mhi_q_init(&mhiCtx.freeLinkReqQ); 

   for (i = 0; i < mhiCtx.mhi_core_tgt_config->max_linkreq; i++)
   {
      mhi_q_queue(&mhiCtx.freeLinkReqQ, (list_t*)(((mhi_link_async_req_t*)mhiCtx.linkReq)+i)); 
   }

   /* Initialize Queues */
   mhi_q_init(&mhiCtx.pendingRingQ);   
   mhi_q_init(&mhiCtx.completeRingQ);
   mhi_q_init(&mhiCtx.waitErRingQ); 
   mhi_q_init(&mhiCtx.waitLinkRingQ);
   mhi_q_init(&mhiCtx.waitAckRingQ);

   /* Initialize the counter that tracks the number of pending completions */
   mhiCtx.num_completions_pending = 0;

   /* Initialize event to NULL, will be created as needed */
   mhiCtx.hNoMoreCompletions = NULL;

   /* Clear the mhi work pending flag, will be set in mhi_core_ack_interrupts if applicable */
   mhiCtx.is_mhi_work_pending = FALSE;

   /* Initialize the link completion timer */
   mhi_osal_timer_init(&mhiCtx.link_completion_timer);

   /* SYSERR cannot be pending at init */
   mhiCtx.is_syserr_pending = FALSE;

   /* Get the MHI state. If already in M0, cache the host configuration */ 
   if (MHI_STATE_M0 == mhi_initial_state)
   {
      mhiCtx.boot_enabled=TRUE;
      if (SUCCESS != mhi_core_cache_hostconfig())
      {
         mhi_osal_debug(MSG_ERR, "mhi_core_init: Failed to cache host configuration");
         goto err;
      }

      /* Send exec env state change notification to host */   
      mhi_core_send_exec_env_change_event(MHI_ENV);
   }

#ifndef NOINTERRUPTS 
   /* Spawn MHI worker thread. After we start the thread, if there is any pending interrupts we will immediately start processing */
   if(MHI_SUCCESS != mhi_osal_spawn_thread(&mhiCtx.worker, "MHI CORE", mhi_core_thread_entry, (void*)&mhiCtx, mhiCtx.mhi_core_tgt_config->mhi_worker_priority) )
   {
      mhi_osal_debug(MSG_ERR, "mhi_core_init: Could not spawn worker thread");
      goto err;
   }
#endif

#ifdef MHI_TEST
   //Core loopback is below sio layer

   /* Enable SW link inactivity timer, if needed */   
   if (!mhiCtx.mhi_link_info.hw_link_inact_timer_supported)  
   {
      mhi_pm_test_init_sw_link_inact_timer();
   }
 
#ifdef MHI_CORE_LOOPBACK
   /* SPAWN thread to listen for loopback */ 
   if(MHI_SUCCESS != mhi_osal_spawn_thread(&mhiCtx.worker, "MHI TEST", mhi_loopback_test, NULL, mhiCtx.mhi_core_tgt_config->mhi_test_priority) )
   {
      mhi_osal_debug(MSG_ERR, "mhi_core_init: Could not spawn test thread");
      goto err;
   }
#endif
#endif

   /* Power Management loop test */
#ifdef MHI_PM_STATE_TEST
   /* SPAWN thread to listen for loopback */ 
   if(MHI_SUCCESS != mhi_osal_spawn_thread(&mhiCtx.worker, "MHI PM TEST", mhi_pm_state_loop_test, &mhiCtx.mhi_pm_info, mhiCtx.mhi_core_tgt_config->mhi_test_priority) )
   {
      mhi_osal_debug(MSG_ERR, "mhi_core_init: Could not spawn test thread");
      goto err;
   }
#endif

   /* Enable all SW CHDB interrupts and the MHI Control interrupt. Event ring DB interrupts will get initialized on demand */
   chdb.chdb[0]=SWINTMASK(0, SWCHSTART,SWCHEND); 
   chdb.chdb[1]=SWINTMASK(1, SWCHSTART,SWCHEND); 
   chdb.chdb[2]=SWINTMASK(2, SWCHSTART,SWCHEND); 
   chdb.chdb[3]=SWINTMASK(3, SWCHSTART,SWCHEND); 
#ifdef MHI_DEVICEWAKE_DB
   chdb.chdb[3]|=MHI_PM_DEVICE_WAKE_DB_MASK; /* Use device wake db */
#endif
   mhi_mmio_enablechdbinterrupts(&chdb);
   mhi_mmio_enablectrlinterrupts();

   ret = SUCCESS; 

err:
   return ret; 
}

/** 
 * @brief      mhi_core_isr
 *
 *             MHI ISR routine that processes the MHI interrupt. This function will mask interrupts 
 *             and signal the MHI Worker thread to do the processing
 *              
 * 
 * @param[in]  *ctx - MHI Context
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect Disables interrupts
 */
void mhi_core_isr(mhi_ctx_t *ctx)
{
   if (mhiCtx.mhi_link_info.mhi_link_hw_enabled) 
   {  
      /* Disable Interrupts. Reason is, since its a level interrupt interrupts need to be masked
       * otherwise we will be stuck in a interrupt processing loop preventing the worker thread from 
       * executing. After the work thread is finished it will re-enable interrupts */ 
      mhi_mmio_maskinterrupts(); 
   }
   else
   {
      mhi_osal_debug(MSG_FATAL,"mhi_core_isr: Cannot mask interrupts, mhi link hw disabled");
   }

   /* Signal worker thread */
   mhi_osal_eventsignal(ctx->hNotify);
    
#ifdef MHI_PM_STATE_TEST
   mhi_pm_test_intr_check();
#endif
}

/** 
 * @brief      mhi_core_thread_entry
 *
 *             This function is the entry point for the MHI Core Worker thread
 *              
 * 
 * @param[in]  *ctx - MHI Context
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
void mhi_core_thread_entry(void *ctx)
{
   mhi_ctx_t *pMhiCtx=(mhi_ctx_t*)ctx; 

   /* Create events we will listen on */
   if(MHI_SUCCESS != mhi_osal_eventcreate(&pMhiCtx->hNotify, NULL) )
   {
      mhi_osal_debug(MSG_FATAL,"mhi_core_thread_entry: Worker thread couldn't create event. Fatal error");
      return; 
   }

   /* Register ISR */ 
   if(MHI_ISR_INVALID_VEC != mhiCtx.mhi_core_tgt_config->mhi_isr_vec)
   {
      mhi_osal_isrinstall(&mhiCtx.irqhandle,mhiCtx.mhi_core_tgt_config->mhi_isr_vec, mhi_core_isr, &mhiCtx); 
   }
   else
   {
      mhi_link_ctrl_core_interrupt(ENABLED);
   }
   
   /* Worker thread entry routine */
   while(1)
   {
      mhi_core_scheduler();
      /* Wait on Notify event triggered by interrupts or user invocations of deferred API's */
      mhi_osal_eventwait(pMhiCtx->hNotify);
   }
}

/** 
 * @brief      mhi_core_ack_interrupts
 *
 *             Acknowledge all the interrupts by reading the interrupts status registers. This will clear them 
 *             form HW and store the pending interrupts to our context. Also acknowledge any SW interrupts/triggers
 *             such as pendingChannel bit array and etc. 
 *              
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
void mhi_core_ack_interrupts(void)
{
   mhi_interrupts_t ctrl, erdb, chdb; 
   uint32 pendingChannels; 
   ring_t *ring=NULL;
   uint32 i, j;
   uint64 db;
   boolean trigger_wake=FALSE;
   
   /* If link hardware is disabled, MMIO must not be accessed. */
   if (mhiCtx.mhi_link_info.mhi_link_hw_enabled)
   {
      /* Get all the MHI interrupts and store the latest interrupt state in the context
       * and any new interrupts in the local variables ctrl,erdb,chdb */
      mhi_mmio_get_mhictrlinterrupts(&mhiCtx.int_ctrl, &ctrl); 
      mhi_mmio_get_erdbinterrupts(&mhiCtx.int_erdb, &erdb);      
      mhi_mmio_get_chdbinterrupts(&mhiCtx.int_chdb, &chdb);      

      /* Log pending control interrupts */
      ctrl.ctrl?mhi_osal_debug(MSG_LOW,"mhi_core_ack_interrupts: State of ctrl = %x",ctrl.ctrl):(void)0;

      /* Check for new control interrupts */
      if(MHI_CTRL_INT & ctrl.ctrl)
      {
         mhi_sm_set_trig_pending(MHI_SM_TRIG_CTRL_INTR_SHIFT,FALSE);
      }

      if(MHI_CRDB_INT & ctrl.ctrl) 
      {
         ring=&mhiCtx.cr;

         if(RING_IDLE == ring->state || RING_WAITREL == ring->state)
         {
            db=mhi_mmio_get_crdb();
            /* Make ring pending only if host is trying to send a new command and wrOffset actually changed */
            if (db != (ring->ringBase + ring->wrOffset*sizeof(mhi_ring_element_t)))
            {
               mhi_core_set_ringstate(ring, RING_PENDING);
            }
            else
            {
               mhiCtx.int_ctrl.ctrl^=MHI_CRDB_INT;
            }
         }
      }

#ifdef MHI_DEVICEWAKE_DB
      /* Check for device wake doorbell, i.e., MHI_PM_DEVICE_WAKE_DB_NUM if supported */
      if (chdb.chdb[3] & MHI_PM_DEVICE_WAKE_DB_MASK)
      {
         uint32 device_wake_state=mhi_pm_get_device_wake_gpio_state();

         mhi_osal_debug(MSG_LOW,"mhi_core_ack_interrupts: device_wake db is pending");
         mhiCtx.int_chdb.chdb[3]^=MHI_PM_DEVICE_WAKE_DB_MASK;
         /* Trigger device wake event for MHI SM if db value changed */
         if (device_wake_state != mhiCtx.mhi_sm_info.device_wake)
         {
            mhi_sm_set_device_wake_pending(device_wake_state);
         }
      }
#endif

      /* For each new channel DB, move ring to pending if it's able to process new DB's  */ 
      for(i=0; i < sizeof(chdb.chdb)/sizeof(chdb.chdb[0]); i++)
      {
         chdb.chdb[i]?mhi_osal_debug(MSG_LOW,"mhi_core_ack_interrupts: chdb.chdb[%d]=%x",i,chdb.chdb[i]):(void)0;
         erdb.erdb[i]?mhi_osal_debug(MSG_LOW,"mhi_core_ack_interrupts: erdb.erdb[%d]=%x",i,erdb.erdb[i]):(void)0;
         for(j=i*32; 0 != chdb.chdb[i] && j<mhiCtx.mhiConfig.nCh; chdb.chdb[i]>>=1, j++)
         {
            if(chdb.chdb[i] & 0x1)
            {  
               ring=&mhiCtx.tr[j]; 

               /* If ring state is idle, waiting for elements or uninitialized, make it pending */ 
               if(RING_IDLE == ring->state || RING_WAITREL == ring->state)
               {
                  db=mhi_mmio_get_chdb(ring->id);
                  /* Make ring pending only if host is trying to send new data */
                  if (db != (ring->ringBase + ring->wrOffset*sizeof(mhi_ring_element_t)))
                  {
                     mhi_core_set_ringstate(ring, RING_PENDING);
                  }
                  else
                  {
                     mhiCtx.int_chdb.chdb[ring->id/32]^=(1<<(ring->id%32));
                  }
               }
               else if (RING_UNINIT == ring->state)
               {
                  mhi_osal_debug(MSG_ERR, "mhi_core_ack_interrupts: Host rang DB for uninitialized ring");
               }
            }
         } 
      }

      /* For all transfer rings waiting on new event ring elements, move them to completion ring queue if event ring DB is pending */
      mhi_q_peek_head(&mhiCtx.waitErRingQ,(list_t **)&ring);
      while (ring != NULL)
      {
         list_t* nextElem=(list_t*)ring;

         /* Get next available list element */
         mhi_q_get_next(&mhiCtx.waitErRingQ, &nextElem);  

         /* Get pointer to the ring relative to list2 */
         ring=(ring_t*)((list_t*)ring-1); 
         if (mhi_core_pendingdb_isset(&mhiCtx.er[ring->erNum]))
         {
            /* Remove ring from list2 */
            mhi_q_remove((list_t*)&ring->list2); 
            mhi_q_queue(&mhiCtx.completeRingQ, &ring->list2);
            mhi_core_pendingdb_clear(&mhiCtx.er[ring->erNum]);
         }
         ring=(ring_t*)nextElem;
      }  

      /* For reliable event rings schedule processing of host acks */
      for(i=0; i<sizeof(mhiCtx.reliableEr)/sizeof(mhiCtx.reliableEr[0]); i++)
      {
         for(j=i*32; i == j/32 && (mhiCtx.reliableEr[i]>>j%32) && j < mhiCtx.mhiConfig.nEr; j++)
         {
            if(0x1 & (mhiCtx.reliableEr[i]>>j%32))
            {
               /* If event ring is pending schedule ring for ack completions */
               if(mhi_core_pendingdb_isset(&mhiCtx.er[j]))
               {
                  if(mhiCtx.er[j].list3.next == NULL)
                  {
                     mhi_q_queue(&mhiCtx.waitAckRingQ, &mhiCtx.er[j].list3);
                     mhi_core_pendingdb_clear(&mhiCtx.er[j]);
                  }
               }
            }
         }
      }
   }

   /* Check for any pending channel notifications. If any move the rings to pending state 
    * if the ring is waiting for requests and clear the flags */
   for(i=0; i < sizeof(mhiCtx.pendingChannels)/sizeof(mhiCtx.pendingChannels[0]); i++)
   {       
      mhiCtx.pendingChannels[i]?mhi_osal_debug(MSG_LOW,"mhi_core_ack_interrupts: mhiCtx.pendingChannels[i]=%x",
                                           i,mhiCtx.pendingChannels[i]):(void)0;
      for(j=i*32, pendingChannels=mhiCtx.pendingChannels[i]; pendingChannels != 0 && j<mhiCtx.mhiConfig.nCh; pendingChannels>>=1, j++)
      {
         if(pendingChannels & 1)
         {
            ring=&mhiCtx.tr[j]; 

            /* If ring is waiting on requests put the ring on pending queue */
            if(RING_WAITREQ == ring->state)
                  mhi_core_set_ringstate(&mhiCtx.tr[j], RING_PENDING); 

            mhi_core_ch_pendingclear(ring->id);
         }
      } 
   }

   /* Detect any stalled channels if there is no wakeup pending */
   if (MHI_STATE_M3 == mhiCtx.mhi_sm_info.mhiState 
         && !(mhi_sm_get_trigs_status()&(1<<MHI_SM_TRIG_CORE_WAKEUP_SHIFT)) 
         && FALSE==mhiCtx.mhi_sm_info.wakeup_pending)
   {
      /* Check for any pending work on inbound rings */
      for(i=0; i < mhiCtx.mhiConfig.nCh; i++)
      {
         if(!mhi_q_is_queue_empty(&mhiCtx.ch[i].request_q) && MHI_CH_TYPE_INBOUND == mhiCtx.lChCtxBase[i].chtype && RING_WAITREL == mhiCtx.tr[i].state )
         {
            if(mhiCtx.ch[i].wakeCount < M3_WAKE_THRESHOLD)
            {
               mhiCtx.ch[i].wakeCount++;
               mhiCtx.ch[i].stalled=TRUE;
               mhi_pm_core_wakeup();
               mhi_osal_debug(MSG_HIGH, "mhi_core_ack_interrupts: Triggering wake from M3 due to stalled channel: %d, count : %d", i, mhiCtx.ch[i].wakeCount);
               trigger_wake = TRUE;
            }
            else
            {
               mhi_osal_debug(MSG_HIGH, "mhi_core_ack_interrupts: Supressing wake from M3 due to stalled channel: %d, count : %d", i, mhiCtx.ch[i].wakeCount);
            }
         }
      }
   }

   mhiCtx.is_mhi_work_pending = !mhi_q_is_queue_empty(&mhiCtx.pendingRingQ) || !mhi_q_is_queue_empty(&mhiCtx.completeRingQ) || !mhi_q_is_queue_empty(&mhiCtx.waitAckRingQ) || trigger_wake;

}

/** 
 * @brief      mhi_core_process_command
 *
 *             Process a MHI Command 
 *              
 * 
 * @param[in]  *ring - Ring Object 
 * @param[in]  *el  -  Ring element
 * @param[out] *event - Event object to populate if generating an event
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             SUCCESS if operation succeeded, SYSERR otherwise  
 * 
 * @sideeffect None 
 */
uint32 mhi_core_process_command(ring_t *ring, mhi_ring_element_t *el, event_t *event )
{
   uint32 chid=el->cmdReset.chid, ret=ERROR;
   ring_t *ch_ring=&mhiCtx.tr[chid];
   ch_t *ch=NULL;

   /* Set up default event as error */
   event->trigger_msi=TRUE;
   event->event_el.cmdCompleteEvent.code=MHI_CMD_CODE_UNDEFINED_ERR;
   event->event_el.cmdCompleteEvent.type=MHI_RING_EL_TYPE_MHI_CMD_COMPLETE_EVENT;

   if(chid > mhiCtx.mhiConfig.nCh && chid < HWCHANNELBASE && chid > HWCHANNELBASEEND)
   {
      mhi_osal_debug(MSG_ERR,"mhi_core_process_command: Invalid channel specified: %d",chid);
      return SYSERR;
   }

   ch=&mhiCtx.ch[chid];

   /* If command is a channel command check if HWACC should handle it */
   if(el->rel.type == MHI_RING_EL_TYPE_RESET_CH_CMD ||
         el->rel.type == MHI_RING_EL_TYPE_STOP_CH_CMD || el->rel.type == MHI_RING_EL_TYPE_START_CH_CMD)
   {
      if(chid >= HWCHANNELBASE && chid <= HWCHANNELBASEEND)
      {
         switch(el->rel.type)
         {
            case MHI_RING_EL_TYPE_RESET_CH_CMD:
               ret = mhi_hw_acc_ch_cmd(chid , CH_CMD_RESET);
               break;
            case MHI_RING_EL_TYPE_STOP_CH_CMD:
               ret = mhi_hw_acc_ch_cmd(chid , CH_CMD_STOP);
               break;
            case MHI_RING_EL_TYPE_START_CH_CMD:
               if (FALSE == mhiCtx.mhi_hw_acc_info.hw_acc_started)
               {
                  /* Start the hardware accelerator */
                  mhi_hw_acc_start();
               }
               ret = mhi_hw_acc_ch_cmd(chid , CH_CMD_START);
               break;
         }

         /* If command was processed successfully set the code to SUCCESS */
         if (SUCCESS == ret)
            event->event_el.cmdCompleteEvent.code=MHI_CMD_CODE_SUCCESS;

         return SUCCESS; 
      }
   }

   /* Handle command based on command type */
   switch(el->rel.type)
   {
      case MHI_RING_EL_TYPE_START_ALL_CHS_CMD:
         /* Do Nothing*/
         break;

      case MHI_RING_EL_TYPE_NOOP:
         /* Do Nothing*/
         break;

      case MHI_RING_EL_TYPE_RESET_CH_CMD:
         
         /* Flush pending link completions before channel is reset */
         if (SUCCESS != mhi_core_flush_link_completions(ch_ring))
         {
            mhi_osal_debug(MSG_ERR,"mhi_core_process_command: Failed to flush completions for ch %d during ch reset",chid);
            return SYSERR;
         }
         
         /* Remove the ring from all Qs */
         if(NULL != ch_ring->list.prev)
            mhi_q_remove((list_t *)&ch_ring->list);
         if(NULL != ch_ring->list2.prev)
            mhi_q_remove((list_t *)&ch_ring->list2);
    
         mhi_core_set_ringstate(ch_ring,RING_UNINIT);
         
         ch->skip_to_td=0;
         ch->trb_bytes_processed=0;

         /* Reset the device sequence numbers */
         if(ch->reliable)
         {
            ch->seq_device=0;
            ch->ack_device=0;
         }

         /* Clear pending db interrupt state in SW and in HW */
         mhi_core_pendingdb_clear(ch_ring);
         mhi_mmio_clear_chdb_interrupt(chid);

         /* Set the channel to disabled state */
         mhiCtx.lChCtxBase[chid].chstate=MHI_CH_STATE_DISABLED;

         if(SUCCESS != mhi_core_flush_ch_ctx(chid,1))
         {
            mhi_osal_debug(MSG_ERR,"mhi_core_process_command: Failed to flush ch ctx for ch %d during ch reset",chid);
            return SYSERR;
         }
         break; 

      case MHI_RING_EL_TYPE_STOP_CH_CMD:

         /* Flush pending link completions before channel is stopped */ 
         if (SUCCESS != mhi_core_flush_link_completions(ch_ring))
         {
            mhi_osal_debug(MSG_ERR,"mhi_core_process_command: Failed to flush completions for ch %d during ch stop",chid);
            return SYSERR;
         }
                 
         /* Set the channel to stop state */
         mhiCtx.lChCtxBase[chid].chstate=MHI_CH_STATE_STOPPED;

         /* Update Context and flush it out to host */
         mhi_ring_update_ctx(ch_ring); 
         if(SUCCESS != mhi_core_flush_ch_ctx(chid,1))
         {
            mhi_osal_debug(MSG_ERR,"mhi_core_process_command: Failed to flush ch ctx for ch %d during ch stop",chid);
            return SYSERR;
         }
         break; 

      case MHI_RING_EL_TYPE_START_CH_CMD:

         /* Cache the new channel context */
         if(SUCCESS != mhi_core_fetch_ch_ctx(chid,1))
         {
            mhi_osal_debug(MSG_ERR,"mhi_core_process_command: Failed to fetch ch ctx for ch %d during ch start",chid);
            return SYSERR; 
         }

         /* Ignore duplicate channel start commands */
         if(mhiCtx.lChCtxBase[chid].chstate == MHI_CH_STATE_RUNNING)
            break;

         if(MHI_SUCCESS != mhi_ring_init_ctx(ch_ring,(mhi_ring_ctx_t*)&mhiCtx.lChCtxBase[chid]))
         {
            mhi_osal_debug(MSG_ERR,"mhi_core_process_command: Failed to init ring ctx for ch %d during ch start",chid);
            return SYSERR;
         }

         mhi_core_set_ringstate(ch_ring,RING_PENDING);
         mhiCtx.lChCtxBase[chid].chstate=MHI_CH_STATE_RUNNING;

         /* Clear pending db interrupt state and in HW */
         mhi_core_pendingdb_clear(ch_ring);
         mhi_mmio_clear_chdb_interrupt(chid);
         
         /* Initialize the event ring context */
         if(SUCCESS != mhi_core_init_event_ring(mhiCtx.lChCtxBase[chid].erindex))
         {
            mhi_osal_debug(MSG_ERR, "mhi_core_process_command: Failed to initialize Event Ring %d",mhiCtx.lChCtxBase[chid].erindex);
            return SYSERR; 
         }

         /* Save reliability flag and sequence number to logical context. Mark the event ring as reliable if the channel is reliable */ 
         /* Assume each reliable channel has its own event ring */

         ch->reliable=el->cmdStart.reliable;
         ch_ring->reliable=ch->reliable; 

         if(ch->reliable)
         {
            mhiCtx.reliableEr[ch_ring->ctx->ch.erindex/32]|=1<<(ch_ring->ctx->ch.erindex%32);
            mhiCtx.er[ch_ring->ctx->ch.erindex].reliable=TRUE;
         }
         else
         {
            mhiCtx.reliableEr[ch_ring->ctx->ch.erindex/32]&=~(1<<(ch_ring->ctx->ch.erindex%32));
            mhiCtx.er[ch_ring->ctx->ch.erindex].reliable=FALSE;
         }

         ch->seq_host=el->cmdStart.seqnum;
        
         /* Perform channel recovery if any */
         mhi_core_recover_ch(chid); 

         if(SUCCESS != mhi_core_flush_ch_ctx(chid,1))
         {
            mhi_osal_debug(MSG_ERR,"mhi_core_process_command: Failed to flush ch ctx for ch %d during ch start",chid);
            return SYSERR;
         }
         break; 
   }

   /* Set the event code to success */
   event->event_el.cmdCompleteEvent.code=MHI_CMD_CODE_SUCCESS;

   return SUCCESS;
}

/** 
 * @brief      mhi_core_fetch_ch_ctx
 *
 *             Fetch num number of channel context at specified channel id 
 *              
 * 
 * @param[in]  chid - Channel id
 * @param[in]  num  - Number of contexts to fetch 
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             SUCCESS if operation succeeded otherwise ERROR
 * 
 * @sideeffect None 
 */
uint32 mhi_core_fetch_ch_ctx(uint32 chid, uint32 num)
{
   mhi_addr_t addr;
   /* Cache contexts */

   addr.va=(uint32)&mhiCtx.lChCtxBase[chid];
   addr.pa=VA2PA(addr.va, mhiCtx.pool_uncached.va, mhiCtx.pool_uncached.pa);

   if(SUCCESS != mhi_link_read(ADDR_CTRL,mhiCtx.ccabap+sizeof(mhi_ch_ctx_t)*chid,&addr,sizeof(mhi_ch_ctx_t)*num, NULL))
   {
      mhi_osal_debug(MSG_ERR, "mhi_core_fetch_ch_ctx: Failed to fetch channel contexts from host");
      return ERROR; 
   }

   return SUCCESS; 
}

/** 
 * @brief      mhi_core_flush_ctx
 *
 *             Flush context to host memory 
 *              
 * 
 * @param[in]  *ring - Ring whose context to flush
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             SUCCESS if operation succeeded otherwise ERROR
 * 
 * @sideeffect None 
 */
uint32 mhi_core_flush_ctx(ring_t *ring)
{
   uint32* src=NULL; 
   uint64 dest=0; 
   uint32 i; 
   mhi_ring_ctx_t mask={0};
   mhi_addr_t addr;

   switch(ring->type)
   {
      case RING_TYPE_CR:
         src=(uint32*)mhiCtx.lCrCtxBase;
         dest=mhiCtx.crcbap;
         mhi_osal_memset(&mask, 0xFF, sizeof(mask));
         break;
      case RING_TYPE_ER:
         src=(uint32*)&mhiCtx.lErCtxBase[ring->id];
         dest=mhiCtx.ecabap+sizeof(mhi_ev_ctx_t)*ring->id;
         mhi_osal_memset(&mask, 0xFF, sizeof(mask));
         break;
      case RING_TYPE_TR:
         switch(mhiCtx.lChCtxBase[ring->id].chstate)
         {
            case MHI_CH_STATE_ENABLED:
               return SUCCESS;
            case MHI_CH_STATE_DISABLED:
               mask.ch.chstate=~mask.ch.chstate;
               break;
            case MHI_CH_STATE_RUNNING:
               mask.ch.chstate=~mask.ch.chstate;
               mask.ch.rp=~mask.ch.rp;
               break;
            case MHI_CH_STATE_SUSPENDED:
               mask.ch.chstate=~mask.ch.chstate;
               break;
            case MHI_CH_STATE_STOPPED:
               mask.ch.chstate=~mask.ch.chstate;
               mask.ch.rp=~mask.ch.rp;
               mask.ch.wp=~mask.ch.wp;
               break;
            case MHI_CH_STATE_ERROR:
               mask.ch.chstate=~mask.ch.chstate;
         }
         src=(uint32*)&mhiCtx.lChCtxBase[ring->id];
         dest=mhiCtx.ccabap+sizeof(mhi_ch_ctx_t)*ring->id;
         break;
   }

   /* Assume all masked fields are multiples of 4 bytes */
   for(i=0; i < sizeof(mhi_ring_ctx_t)/sizeof(uint32); i++)
   {
      if( 0 != *((uint32*)&mask+i) )
      {
         addr.va=(uint32)(src+i);
         addr.pa=VA2PA(addr.va, mhiCtx.pool_uncached.va, mhiCtx.pool_uncached.pa);
         if(mhi_link_write(ADDR_CTRL,dest+(i*sizeof(uint32)), &addr, sizeof(uint32), NULL))
         {
            mhi_osal_debug(MSG_ERR, "mhi_core_flush_ctx: Failed to flush out channel context to host");
            return ERROR;
         }
      }
   }

   return SUCCESS;
}

/** 
 * @brief      mhi_core_flush_ch_ctx
 *
 *             Flush num number of channel context starting at chId
 *              
 * 
 * @param[in]  chid - Channel ID from which to flush
 * @param[in]  num  - Number of channels to flush
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             SUCCESS if operation succeeded, ERROR otherwise 
 * 
 * @sideeffect None 
 */
uint32 mhi_core_flush_ch_ctx(uint32 chid, uint32 num)
{
   uint32 i;

   /* Flush out the contexts */
   for(i=0; i < num; i++)
   {
      if(SUCCESS != mhi_core_flush_ctx(&mhiCtx.tr[chid+i]))
      {
         return ERROR;
      }
   }
   
   return SUCCESS; 
}

/** 
 * @brief      mhi_core_validateHostAddress
 *
 *             Validate host address by checking to see if its in the correct bounds 
 *              
 * 
 * @param[in]  ptr   - Host address
 * @param[in]  index - Address index. 0 = MHICTRL and 1 = MHIDATA
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             SUCCESS if address is valid, non zero otherwise 
 * 
 * @sideeffect None 
 */
uint32 mhi_core_validateHostAddress(uint64 ptr, uint32 index)
{
   if(ADDR_CTRL == index)
      return (ptr<mhiCtx.mhiaddr.mhictrlbase || ptr>mhiCtx.mhiaddr.mhictrllimit);
   else
      return (ptr<mhiCtx.mhiaddr.mhidatabase || ptr>mhiCtx.mhiaddr.mhidatalimit);
}

/** 
 * @brief      mhi_core_process_ring_cb
 *
 *             Process ring callback function when a link request is completed 
 *              
 * 
 * @param[in]  *ring  - Ring  
 * @param[in]  *linkReq - Completed link request
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
void mhi_core_process_ring_cb(ring_t *ring, mhi_link_async_req_t *linkReq)
{
   mhi_link_async_req_t *pendingReq; 

#ifndef MHI_BOOT
   if ((mhi_osal_get_systime_ms() - linkReq->time_issued_ms) > MAX_TIME_FOR_LINKREQ_MS)
      mhi_osal_debug(MSG_FATAL,"mhi_core_process_ring_cb: Issued link request %x from ring %d exceeded max time threshold",linkReq,ring->id);
#endif   

#ifndef MHI_BOOT
   mhi_osal_syncenter(&mhiCtx.mhiSync);
   /* Decrement the count for number of outstanding completions */
   mhiCtx.num_completions_pending--;

   if (0 > mhiCtx.num_completions_pending)
      mhi_osal_debug(MSG_FATAL,"mhi_core_process_ring_cb:mhiCtx.num_completions_pending = %d",mhiCtx.num_completions_pending);
      
   /* Signal no more completions pending if the event was created */
   if (0 == mhiCtx.num_completions_pending && NULL != mhiCtx.hNoMoreCompletions)
      mhi_osal_eventsignal(mhiCtx.hNoMoreCompletions);
   mhi_osal_syncleave(&mhiCtx.mhiSync);
#endif   

   /* Attempt to process the link request queue. We must process in sequence. If completed request is not the first on queue, 
    * leave it marked complete, it will be processed later in sequence */ 
   mhi_q_peek_head(&ring->linkReqQ, (list_t**)&pendingReq);

   if(NULL == pendingReq) 
      mhi_osal_debug(MSG_FATAL, "mhi_core_process_ring_cb: ring linkReqQ is empty, asserting"); 

   /* If first request is not ready yet return */
   if (pendingReq != linkReq)
   {
      return;
   }
   else
   {
      mhi_osal_debug(MSG_MEDIUM, "mhi_core_process_ring_cb: Ring trb complete: addr: %x, size: %x", linkReq->local_addr, linkReq->size); 

      /* Put the ring on the completion queue if its not any other queue. List2 is used for completion and 
       * wait for event ring queue */  
      mhi_osal_syncenter(&mhiCtx.mhiSync); 
      if(NULL == ring->list2.next) 
      {
         mhi_osal_debug(MSG_HIGH,"mhi_core_process_ring_cb: put ring id %d on completeRingQ",ring->id);
         mhi_q_queue(&mhiCtx.completeRingQ, (list_t*)&ring->list2); 

         /* Notify core thread */
         mhi_osal_eventsignal(mhiCtx.hNotify); 
      }
      mhi_osal_syncleave(&mhiCtx.mhiSync);
   }

   return;
}

/** 
 * @brief      mhi_core_cache_hostconfig
 *
 *             Cache host configuration from host memory
 *              
 *
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             SUCCESS if operation succeeded  
 * 
 * @sideeffect None 
 */
uint32 mhi_core_cache_hostconfig()
{
   mhi_addr_t addr;
   uint32 i;

   mhi_osal_debug(MSG_HIGH, "mhi_core_cache_hostconfig: Caching host MMIO configuration"); 

   /* Get the HOST memory region configurations, where all buffers/contexts will reside */
   mhi_mmio_get_mhiaddr(&mhiCtx.mhiaddr);

   if (MHI_SUCCESS != mhi_core_map_ctrl_and_base_regions(mhiCtx.mhiaddr,&mhiCtx.mhiLocalAddr))
   {
      mhi_osal_debug(MSG_FATAL, "mhi_core_cache_hostconfig: FATAL - Could not allocate local address space."); 
      return ERROR;
   }

   /* Get the channel/event base pointers  */
   mhiCtx.ccabap=mhi_mmio_get_chbase();
   mhiCtx.ecabap=mhi_mmio_get_erbase();
   mhiCtx.crcbap=mhi_mmio_get_crbase();

   /* Retreive mhi configuration */
   mhi_mmio_get_mhiconfig(&mhiCtx.mhiConfig);

   /* NHWER or NHWCH might have changed */ 
   mhi_link_setup_hwacc_db_routing(mhiCtx.mhi_core_tgt_config->mhi_hw_acc_mailbox_cdb_target,mhiCtx.mhi_core_tgt_config->mhi_hw_acc_mailbox_edb_target);

   if( NULL == (mhiCtx.erCtx = mhi_osal_malloc(mhiCtx.mhiConfig.nEr*sizeof(er_t))) )
   {
      mhi_osal_debug(MSG_ERR, "mhi_core_cache_hostconfig: Could not allocate logical event ring ctx");  
      return ERROR; 
   }

   mhi_osal_memset(mhiCtx.erCtx, 0, mhiCtx.mhiConfig.nEr*sizeof(er_t));

   if( NULL == (mhiCtx.er = mhi_osal_malloc(mhiCtx.mhiConfig.nEr*sizeof(ring_t))) )
   {
      mhi_osal_debug(MSG_ERR, "mhi_core_cache_hostconfig: Could not allocate logical event ring");  
      return ERROR; 
   }

   mhi_osal_memset(mhiCtx.er, 0x0, sizeof(ring_t)*mhiCtx.mhiConfig.nEr);

   for(i=0; i < mhiCtx.mhiConfig.nEr; i++)
   {
      mhi_ring_init(i, &mhiCtx.er[i], RING_TYPE_ER, NULL);
   }

   addr.va=(uint32)mhiCtx.lChCtxBase;
   addr.pa=VA2PA(addr.va, mhiCtx.pool_uncached.va, mhiCtx.pool_uncached.pa);

   /* cache all the contexts */
   if(mhi_link_read(ADDR_CTRL,mhiCtx.ccabap, &addr,sizeof(mhi_ch_ctx_t)*mhiCtx.mhiConfig.nCh, NULL))
   {
      mhi_osal_debug(MSG_ERR, "mhi_core_cache_hostconfig: Failed to cache channel contexts from host");
      return ERROR; 
   }

   addr.va=(uint32)mhiCtx.lErCtxBase;
   addr.pa=VA2PA(addr.va, mhiCtx.pool_uncached.va, mhiCtx.pool_uncached.pa);

   if(mhi_link_read(ADDR_CTRL,mhiCtx.ecabap, &addr,sizeof(mhi_ev_ctx_t)*mhiCtx.mhiConfig.nEr, NULL))
   {
      mhi_osal_debug(MSG_ERR, "mhi_core_cache_hostconfig: Failed to cache event ring contexts from host");
      return ERROR; 
   }

   addr.va=(uint32)mhiCtx.lCrCtxBase;
   addr.pa=VA2PA(addr.va, mhiCtx.pool_uncached.va, mhiCtx.pool_uncached.pa);
   if(mhi_link_read(ADDR_CTRL,mhiCtx.crcbap, &addr,sizeof(mhi_cr_ctx_t), NULL))
   {
      mhi_osal_debug(MSG_ERR, "mhi_core_cache_hostconfig: Failed to cache command ring contexts from host");
      return ERROR; 
   }

   /* Initialize the local ring contexts. Event rings, other than the primary event ring, 
    * will be initialized only when used, to avoid wasting resources */

   if(SUCCESS != mhi_ring_init_ctx(&mhiCtx.cr,(mhi_ring_ctx_t*)mhiCtx.lCrCtxBase))
   {
      mhi_osal_debug(MSG_ERR, "mhi_core_cache_hostconfig: Failed to initialize Command Ring");
      return ERROR; 
   }

   if(SUCCESS != mhi_core_init_event_ring(PRIMARY_ER))
   {
      mhi_osal_debug(MSG_ERR, "mhi_core_cache_hostconfig: Failed to initialize Primary Event Ring");
      return ERROR; 
   }
   
#ifndef MHI_HOST_REVD_COMPATIBLE
   for(i=0;i<mhiCtx.mhiConfig.nCh;i++)
   {
      /* If channel is not disabled initialize the ring */
      if(MHI_CH_STATE_DISABLED != mhiCtx.lChCtxBase[i].chstate)
      {
         if(SUCCESS != mhi_ring_init_ctx(&mhiCtx.tr[i],(mhi_ring_ctx_t*)&mhiCtx.lChCtxBase[i]))
         {
            mhi_osal_debug(MSG_ERR, "mhi_core_cache_hostconfig: Failed to initialize Transfer Ring");
            return ERROR; 
         }
   
         /* Set the channel to running state  */
         mhiCtx.lChCtxBase[i].chstate=MHI_CH_STATE_RUNNING;
   
         /* Flush the context */
         mhi_core_flush_ch_ctx(i,1); 
      }
   }
#endif

   return SUCCESS; 
}

/** 
 * @brief      mhi_core_ch_pendingclear
 *
 *             Clear the pending channel bits
 *              
 * 
 * @param[in]  chId - Channel id
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
void mhi_core_ch_pendingclear(uint32 chId)
{
   /* Clear the channel pending flag  */
   mhi_osal_atomic_clear_bit(&mhiCtx.pendingChannels[chId/32],chId%32);  
}

/** 
 * @brief      mhi_core_ch_pendingset
 *
 *             Set the pending channel flag bit
 *              
 * 
 * @param[in]  chId - Channel id
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
void mhi_core_ch_pendingset(uint32 chId)
{
   /* Set the channel pending flag */
   mhi_osal_atomic_set_bit(&mhiCtx.pendingChannels[chId/32],chId%32);  
}

/** 
 * @brief      mhi_core_complete_req
 *
 *             Complete mhi request by moving it off its queue and putting it back on the free queue. Issue any cb for the request. 
 *             If all requests are completed, close channel if its pending to be closed
 *              
 * 
 * @param[in]  *ch    - Channel to which request belongs
 * @param[in]  *ring  - Ring which request belongs to 
 * @param[in]  *req   - Request
 * @param[in]  status - Completion status of request
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
void mhi_core_complete_req(ch_t *ch, ring_t *ring, mhi_request_t *req, mhi_status_t status)
{
   mhi_return_t req_ret; 
   mhi_cb_t cb; 
   mhi_return_t close_ret; 
   list_t *next=NULL; 

   /* If request is completed put it back on free request queue for channel and issue the cb
    * if necessary */
   if(NULL != req && MHI_SUCCESS == req->ret.status)
   {
      /* Make a copy of cb function and return value before we release the request */
      req->ret.userData=req->cb.userData;
      req->ret.op=req->op;
      req->ret.payload.rw.buff=req->params.rw.data; 
      req_ret=req->ret; 

      /* set req status */
      req_ret.status=status;
      cb=req->cb; 

      /* Clear the op */
      req->op=MHI_INVALID;

      /* Do CB for request */ 
      if(TRUE == req->params.rw.last_op)
      {
         mhi_osal_debug(PROFILING_LOG|MSG_MEDIUM,"mhi_core_process_ring_completion: completion_callback=start, ch_id=%d, req_userdata=0x%x",ring->id,req_ret.userData);
         cb.cbFunc(req_ret);
         mhi_osal_debug(PROFILING_LOG|MSG_MEDIUM,"mhi_core_process_ring_completion: completion_callback=end, ch_id=%d, req_userdata=0x%x",ring->id,req_ret.userData);
      }

      mhi_osal_syncenter(&ch->chSync); 

      /* Remove the request from issued queue and put it on free queue */
      mhi_q_remove(&req->list);
      mhi_q_queue(&ch->request_free_q,&req->list);

      /* Check if channel is pending CLOSE. If it is and there are no more link requests 
       * Mark the channel as closed  */
      mhi_q_get_next(&ring->linkReqQ,&next);  
      if(CH_PENDING_CLOSE == ch->ch_client_state && NULL == next)
      {
         /* Do the CB */
         close_ret.userData=ch->close_cb.userData; 
         close_ret.op=MHI_CLOSE; 
         close_ret.status = MHI_SUCCESS; 

         mhi_osal_syncleave(&ch->chSync); 
         ch->close_cb.cbFunc(close_ret);
      }
      else
      {
         mhi_osal_syncleave(&ch->chSync); 
      }
   }
}

/** 
 * @brief      mhi_core_process_ring_acks
 *
 *             Process ring reliability ack's from the host
 *              
 * 
 * @param[in]  *er - Event ring for which to process ack's
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             SUCCESS
 * 
 * @sideeffect None 
 */
uint32 mhi_core_process_ring_acks(ring_t *er)
{
   mhi_link_async_req_t *linkReq; 
   uint32 oldWrOffset, i, ackNum;
   mhi_ring_element_t *el;
   ring_t *tr;
   ch_t *ch;

   mhi_osal_syncenter(&er->sync); 

   if(!er->reliable)
   {
      mhi_osal_syncleave(&er->sync); 
      return SUCCESS; 
   }

   oldWrOffset=er->wrOffset;

   if(mhi_core_pendingdb_isset(er))
      mhi_ring_update_wroffset(er);


   ackNum=((uint64)er->wrOffset-(uint64)oldWrOffset+er->rSize)%er->rSize;

   mhi_osal_debug(MSG_HIGH, "mhi_core_process_ring_acks: oldWroffset: %x, newWrOffset: %x, acknum %d", oldWrOffset, er->wrOffset, ackNum);
   for( i=0; i < ackNum; i++)
   {
      oldWrOffset=++oldWrOffset%er->rSize;

      el=&er->lRingBase[oldWrOffset];

      if(MHI_RING_EL_TYPE_TRANSFER_EVENT == el->rel.type)
      {
         tr=&mhiCtx.tr[el->tEvent.chid];
         ch=&mhiCtx.ch[el->tEvent.chid];

         /* Navigate our linkReq wat ack Q until we find a linkReq with an event. Close of any client requests 
          * as we go  */
         for(;;)
         {
            mhi_q_dequeue(&tr->linkReqWaitAckQ, (list_t**)&linkReq);

            if(NULL==linkReq)
            {
               break;
            }

            /* Ack any request if any */
            mhi_core_complete_req(ch, tr, linkReq->req, MHI_SUCCESS);

            if(MHI_RING_EL_TYPE_RESERVED == linkReq->event.event_el.tEvent.type)
            {

               mhi_osal_debug(MSG_HIGH, "mhi_core_process_ring_acks: Free linkReq from chain as it has no event, linkreq.link_addr : %x%x, wroffset: %d", QW2DW(linkReq->link_addr), oldWrOffset);
               mhi_q_queue(&mhiCtx.freeLinkReqQ, (list_t*)linkReq);
               continue;
            }
            else
            {
               /* If request matches free up the request and issue any cb to clients */
               if(linkReq->event.event_el.tEvent.ptr==el->tEvent.ptr && linkReq->event.event_el.tEvent.chid==el->tEvent.chid)
               {
                  mhi_osal_debug(MSG_HIGH, "mhi_core_process_ring_acks: Found event that matches linkReq->link_addr: %x%x linkReq->event.ptr: %x%x, wroffset: %d", QW2DW(linkReq->link_addr), QW2DW(el->tEvent.ptr), oldWrOffset);
                  ch->ack_device++;
                  mhi_q_queue(&mhiCtx.freeLinkReqQ, (list_t*)linkReq);
                  break;
               }
               else
               {
                  mhi_osal_debug(MSG_FATAL, "mhi_core_process_ring_acks: Event doesnt match pending requests");

               }
            }
         }
      }

   }

   mhi_osal_syncleave(&er->sync); 

   return SUCCESS;

}

/** 
 * @brief      mhi_core_process_ring_completion
 *
 *             Process ring completions for ring
 *              
 * 
 * @param[in]  *ring - Ring whose completions to process
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             SUCCESS if operation succeeded,
 *             SYSERR otherwise
 *
 * 
 * @sideeffect None 
 */
uint32 mhi_core_process_ring_completion(ring_t *ring)
{
   mhi_link_async_req_t *linkReq; 
   mhi_request_t *req=NULL; 
   mhi_status_t req_status=MHI_SUCCESS;
   int32 ret=SUCCESS; 
   ch_t *ch = &mhiCtx.ch[ring->id]; 
   ring_t *waitLinkRing;
   boolean sendmsi=FALSE;
   uint32 count=0;
   boolean event_caching_allowed=(ch->reliable)?FALSE:TRUE;

   /* Attempt to process the link request queue of the ring. We must process in sequence. 
    * If we encounter a request that's not yet completed return. This function will be called again 
    * at a later time when the request is finished. If we reached this function, the first request 
    * of the link request queue should be completed but it might not be if we processed it as it got signaled  
    */
   mhi_q_peek_head(&ring->linkReqQ, (list_t**)&linkReq);

   if(NULL == linkReq) 
   {
      mhi_osal_debug(MSG_HIGH, "mhi_core_process_ring_completion: ring linkReqQ is empty");
      return SUCCESS;
   }

   mhi_osal_debug(PROFILING_LOG|MSG_HIGH,"mhi_core_process_ring_completion: process_completion=start, ch_id=%d",ring->id);

   /* Loop over Link Requests for ring */
   do
   {
      if(PENDING == linkReq->status)
      {
         break;
      }
      else if(ABORT == linkReq->status)
      {
         req_status=MHI_ABORTED;
         goto close_req;
      }
      else if(SUCCESS == linkReq->status)
      {
#ifdef COMPLETIONS_THROTTLE_COUNT        
        /* Throttle completions */      
        if (++count == COMPLETIONS_THROTTLE_COUNT)
        {
           mhi_osal_debug(MSG_HIGH,"mhi_core_process_ring_completion: Reached count, throttling completions");
           /* Put ring on completionQ only if ring is not already Qed */
           mhi_osal_syncenter(&mhiCtx.mhiSync); 
           if (NULL == ring->list2.next)
              mhi_q_queue(&mhiCtx.completeRingQ, (list_t*)&ring->list2); 
           mhi_osal_syncleave(&mhiCtx.mhiSync);
           break;
        }
#else
        (void)count;
#endif
        req_status=MHI_SUCCESS;
      }
      else
      {
         mhi_osal_debug(MSG_FATAL,"mhi_core_process_ring_completion: Unknown request status");
      }
       
      mhi_osal_debug(PROFILING_LOG|MSG_MEDIUM,"mhi_core_process_ring_completion: process_completion=start, ch_id=%d, local_addr=0x%x",ring->id,linkReq->local_addr);

      /* Generate event if there is any  */
      if(MHI_RING_EL_TYPE_RESERVED != linkReq->event.event_el.tEvent.type)
      {
         /* Aggregate MSIs */
         if (linkReq->event.trigger_msi)
            sendmsi=TRUE;
         
         /* Burst events to host unless the channel is marked reliable */
         mhi_osal_debug(PROFILING_LOG|MSG_MEDIUM,"mhi_core_process_ring_completion: send_transfer_completion_event=start, ch_id=%d, local_addr=0x%x",ring->id,linkReq->local_addr);   
         if ( SUCCESS != (ret = mhi_corei_send_event(ring->erNum, &linkReq->event.event_el, linkReq->event.trigger_msi, event_caching_allowed)) )
         {
            /* If event ring is full (ring is empty and has no free TRB's, 
             * and we are dealing with the non primary event ring, store the event for later 
             * otherwise SYSERR */
            if(RING_FULL == ret)
            {
               mhi_osal_debug(MSG_MEDIUM, "mhi_core_process_ring_completion: Event ring full halting ring completion processing for local_addr=0x%x, ring_id=%d",linkReq->local_addr,ring->id);
               /* Put ring on completionQ only if ring is not already Qed */
               mhi_osal_syncenter(&mhiCtx.mhiSync);
               if (NULL == ring->list2.next)
                  mhi_q_queue(&mhiCtx.waitErRingQ, &ring->list2);
               /* Flush aggregated events */
               mhi_core_flush_cached_events(ring->erNum,sendmsi);
               mhi_osal_syncleave(&mhiCtx.mhiSync);
               
               return SUCCESS;
            }

            /* If generate event failed for any other reason, treat it as a system error */
            else
            {
               return SYSERR; 
            }
         }
         mhi_osal_debug(PROFILING_LOG|MSG_MEDIUM,"mhi_core_process_ring_completion: send_transfer_completion_event=end, ch_id=%d, local_addr=0x%x",ring->id,linkReq->local_addr);

         /* Increment device sequence number */
         if(ch->reliable)
         {
            ch->seq_device++;
         }

      }

close_req:
      /* Get request and logical channel object */
      req=linkReq->req; 

      /* Remove the link request and put back on either free link request queue or waitforackQ if 
       * the channel is reliable*/
      mhi_q_remove(&linkReq->list); 

      /* If channel is not reliable complete the req, otherwise we need to buffer the linkreq until host acks it */ 
      if(!ch->reliable)
      {
         mhi_core_complete_req(ch, ring, req, req_status);
      }
      
      mhi_osal_syncenter(&mhiCtx.mhiSync); 

      mhi_q_queue((!ch->reliable?&mhiCtx.freeLinkReqQ:&ring->linkReqWaitAckQ), &linkReq->list);

      if(!ch->reliable)
      {
         /* If there is a ring waiting for LINKREQ move it to pending queue */ 
         mhi_q_peek_head(&mhiCtx.waitLinkRingQ, (list_t**)&waitLinkRing);
         if(NULL != waitLinkRing)
         {
            mhi_q_remove(&waitLinkRing->list); 
            mhi_core_set_ringstate(waitLinkRing, RING_PENDING); 
         }
      }

      mhi_osal_debug(PROFILING_LOG|MSG_MEDIUM,"mhi_core_process_ring_completion: process_completion=end, ch_id=%d, local_addr=0x%x",ring->id,linkReq->local_addr);

      if (req != NULL && TRUE == req->params.rw.last_op)
         mhi_osal_debug(PROFILING_LOG|MSG_HIGH,"mhi_core_process_ring_completion: core_packet=end, ch_id=%d, req_userdata=0x%x",ring->id,req->ret.userData);
         
      mhi_osal_syncleave(&mhiCtx.mhiSync); 

      /* Get pointer to next linkReq */
      mhi_q_peek_head(&ring->linkReqQ, (list_t**)&linkReq); 

   }while(linkReq != NULL); 

   mhi_osal_debug(PROFILING_LOG|MSG_HIGH,"mhi_core_process_ring_completion: process_completion=end, ch_id=%d",ring->id);

   if (event_caching_allowed)
   {
      mhi_osal_debug(PROFILING_LOG|MSG_HIGH,"mhi_core_process_ring_completion: flush_cached_events=start, ch_id=%d",ring->id);
      /* Flush aggregated events */
      mhi_core_flush_cached_events(ring->erNum,sendmsi);
      mhi_osal_debug(PROFILING_LOG|MSG_HIGH,"mhi_core_process_ring_completion: flush_cached_events=end, ch_id=%d",ring->id);
   }

   return SUCCESS; 
}

/** 
 * @brief      mhi_core_handle_single_ring_completion
 *
 *             Process a single ring's completions
 *              
 *
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             See return codes of mhi_core_process_ring_completion 
 *             
 * 
 * @sideeffect None 
 */
void mhi_core_handle_single_ring_completion(void)
{
   int32 ret = SUCCESS; 
   ring_t *ring = NULL; 

   /* Get a ring from the completion ring queue */
   mhi_osal_syncenter(&mhiCtx.mhiSync); 
   mhi_q_dequeue(&mhiCtx.completeRingQ, (list_t**)&ring);  
   mhi_osal_syncleave(&mhiCtx.mhiSync); 

   if(NULL != ring)
   {
      ring=(ring_t*)((list_t*)ring-1); 
      mhi_osal_debug(MSG_MEDIUM,"mhi_core_handle_single_ring_completion: Process ring completions type: %d, id: %d", ring->type, ring->id); 

      mhi_osal_syncenter(&ring->sync); 
      ret = mhi_core_process_ring_completion(ring); 
      mhi_osal_syncleave(&ring->sync); 
   }

   if (SYSERR == ret)
      mhi_core_sys_error();

   return; 
}

/** 
 * @brief      mhi_core_handle_single_ring_acks
 *
 *             Process any host acks for reliable event rings
 *              
 *
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
void mhi_core_handle_single_ring_acks(void)
{
   int32 ret = SUCCESS; 
   ring_t *ring = NULL; 

   /* Get a ring from the completion ring queue */
   mhi_osal_syncenter(&mhiCtx.mhiSync); 
   mhi_q_dequeue(&mhiCtx.waitAckRingQ, (list_t**)&ring);  
   mhi_osal_syncleave(&mhiCtx.mhiSync); 

   if(NULL != ring)
   {
      ring=(ring_t*)((list_t*)ring-2); 
      mhi_osal_debug(MSG_MEDIUM,"mhi_core_handle_single_ring_acks: Process ring acks from host type: %d, id: %d", ring->type, ring->id); 

      mhi_osal_syncenter(&ring->sync); 
      ret = mhi_core_process_ring_acks(ring); 
      mhi_osal_syncleave(&ring->sync); 
   }

   if (SYSERR == ret)
      mhi_core_sys_error();

   return; 
}

/** 
 * @brief      mhi_core_process_single_ring
 *
 *             Process a single pending ring 
 *              
 *
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 *
 *
 * @sideeffect None 
 */
void mhi_core_process_single_ring(void)
{
   int32 ret = SUCCESS; 
   ring_t *ring = NULL; 

   mhi_osal_syncenter(&mhiCtx.mhiSync); 
   mhi_q_dequeue(&mhiCtx.pendingRingQ, (list_t**)&ring);  
   mhi_osal_syncleave(&mhiCtx.mhiSync); 

   if(NULL != ring) 
   {
      /* Check ring state, if it's on pending queue it should be pending */
      if(RING_PENDING != ring->state)
      {
         mhi_osal_debug(MSG_ERR,"mhi_core_process_single_ring: Attempting to process non pending RING type: %d, id %d", ring->type, ring->id); 
         mhi_core_sys_error(); 
      }
      mhi_osal_syncenter(&ring->sync); 

      mhi_osal_debug(MSG_MEDIUM,"mhi_core_process_single_ring: Process ring type: %d, id: %d", ring->type, ring->id); 

      /* Process ring */
      ret = mhi_ring_process(ring, mhi_core_pendingdb_isset(ring), 0);

      /* Clear any pending DB's for ring. Note it is possible we tried to process a ring but could not 
       * in case of transfer rings. In this case we still clear the pending DB to indicate we acknowledged 
       * and attempted to process it */
      mhi_core_pendingdb_clear(ring);

      /* Determine ring state depending on return of process ring */
      if(SUCCESS == ret)
      {
         mhi_core_set_ringstate(ring, RING_WAITREL); 
      }
      else if(LINK_BUSY == ret) 
      {
         mhi_core_set_ringstate(ring, RING_WAITLINK); 
      }
      else if(REQUESTQ_EMPTY == ret)
      {
         mhi_core_set_ringstate(ring, RING_WAITREQ); 
      }
      else if(PENDING_EVENT == ret)
      {
         mhi_core_set_ringstate(ring, RING_WAITER); 
      }
      else if(BADTRB == ret)
      {
         mhi_core_set_ringstate(ring, RING_ERROR); 
      }
      else if(SYSERR == ret)
      {
         mhi_osal_debug(MSG_ERR,"mhi_core_process_single_ring: SYSTEM ERROR occurred during ring processing");
         mhi_core_sys_error();
      }
      else if(RING_THROTTLED == ret) 
      {
         mhi_core_set_ringstate(ring, RING_PENDING);
      }

      /* If there are pending DMA requests, flush them */
      if (mhiCtx.mhi_link_info.is_DMA_flush_needed)
      {
         mhi_link_dma_flush();
         mhiCtx.mhi_link_info.is_DMA_flush_needed=FALSE;
      }
      
      mhi_osal_syncleave(&ring->sync); 
   }

   return; 
}


/*  MHI task scheduler. Checks for state changes, dorbells, etc.. and runs the appropriate handlers */

/** 
 * @brief      mhi_core_scheduler
 *
 *             This is the heart of MHI Core. This function is called by the MHI Worker thread whenever it is awakened. 
 *             This function will check for any new interrupts or SW triggers and run the appropriate processing 
 *             routines.  
 *              
 *
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
void mhi_core_scheduler(void)
{
   mhi_osal_debug((PROFILING_LOG|GENERAL_LOG|MSG_HIGH),"mhi_core_scheduler: Enter"); 
   
   /* If MHI core is suspended (or in low power state) and work is pending, attempt resume */
   if ((MHI_STATE_M1 == mhiCtx.mhi_sm_info.mhiState ||
        MHI_STATE_M2 == mhiCtx.mhi_sm_info.mhiState ||
        MHI_STATE_M3 == mhiCtx.mhi_sm_info.mhiState)&&
        FALSE == mhiCtx.mhi_sm_info.wakeup_pending  &&
        mhi_core_is_work_pending())
   { 
      mhi_pm_core_wakeup();
   }

   /* Check for new interrupts */
   mhi_core_ack_interrupts(); 

   /* Loop while there is pending work. 
    * Pending work is due to 
    a) MHICTRL interrupts (state changes) 
    b) Pending Ring processing or completions AND MHISTATE is M0. 
    */
   while( (mhiCtx.int_ctrl.ctrl&MHI_CTRL_INT) || mhi_sm_get_trigs_status() || (mhiCtx.is_mhi_work_pending && mhiCtx.mhi_sm_info.mhiState==MHI_STATE_M0)) 
   {
      /* Check for state changes  */
      if(mhi_sm_get_trigs_status())
      {
         mhi_sm_run_state_machine(); 

         /* Clear the pending interrupt in pending status, so we know we processed it */
         if(mhiCtx.int_ctrl.ctrl & MHI_CTRL_INT)
            mhiCtx.int_ctrl.ctrl^=MHI_CTRL_INT; 
      }

      /* If MHI is running attempt to process rings and their completions */ 
      if(MHI_STATE_M0 == mhiCtx.mhi_sm_info.mhiState)
      {
         /* Handle a single ring async completion */
         mhi_core_handle_single_ring_completion();

         /* Handle ring acks */
         mhi_core_handle_single_ring_acks();

         /* Process a single ring */
         mhi_core_process_single_ring();
      }

      /* Check for new interrupts */
      mhi_core_ack_interrupts(); 
   }/* while */

   /* Process idle tasks since all pending mhi work is complete */
   mhi_core_process_idle_tasks();

   /* Re-enable interrupts. Interrupts are disabled in the ISR, so when the worker thread is finished we 
    * need to re-enable them to facilitate handling of any new interrupts */
   if (mhiCtx.mhi_link_info.mhi_link_hw_enabled)
   { 
      mhi_mmio_unmaskinterrupts();  
   }

   mhi_osal_debug((PROFILING_LOG|GENERAL_LOG|MSG_HIGH),"mhi_core_scheduler: Exit");

   return; 
}

/** 
 * @brief      mhi_core_pendingdb_isset
 *
 *             Checks if there is a pending doorbell for the given ring 
 *              
 * 
 * @param[in]  *ring - Ring
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             TRUE if doorbell is set, FALSE otherwise
 * 
 * @sideeffect None 
 */
boolean mhi_core_pendingdb_isset(ring_t *ring)
{
   switch(ring->type)
   {
      case RING_TYPE_TR:
         return 0 != (mhiCtx.int_chdb.chdb[ring->id/32] & (1<<(ring->id%32)));
      case RING_TYPE_ER:
         return 0 != (mhiCtx.int_erdb.erdb[ring->id/32] & (1<<(ring->id%32)));
      case RING_TYPE_CR:
         return 0 != (mhiCtx.int_ctrl.ctrl & MHI_CRDB_INT); 
      default:
         mhi_osal_debug(MSG_FATAL, "mhi_core_pendingdb_isset: Invalid ring type passed");
   }

   return TRUE; 
}

/** 
 * @brief      mhi_core_pendingdb_clear
 *
 *             Clears pending doorbells in mhiCtx
 *              
 * 
 * @param[in]  *ring
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
void mhi_core_pendingdb_clear(ring_t *ring)
{
   switch(ring->type)
   {
      case RING_TYPE_TR:
         mhiCtx.int_chdb.chdb[ring->id/32] &=~(1<<(ring->id%32));
         break; 
      case RING_TYPE_ER:
         mhiCtx.int_erdb.erdb[ring->id/32] &=~(1<<(ring->id%32));
         break; 
      case RING_TYPE_CR:
         mhiCtx.int_ctrl.ctrl^=MHI_CRDB_INT; 
         break; 
      default:
         mhi_osal_debug(MSG_FATAL, "mhi_core_pendingdb_clear: Invalid ring type passed");

   }

   return; 
}

/** 
 * @brief      mhi_core_issue_linkreq
 *
 *             Issue a link request to the MHI link layer 
 *              
 * 
 * @param[in]  index - Address index
 * @param[in]  *linkReq - Link request object
 * @param[in]  op - Operation read or write
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             SUCCESS if operation succeeded  
 *             LINK_BUSY if operation could not be submitted
 *             LINK_ERROR if an error occurred 
 * 
 * @sideeffect None 
 */
uint32 mhi_core_issue_linkreq(uint32 index, mhi_link_async_req_t *linkReq, mhi_op_t op )
{
   mhi_status_t status; 
   mhi_addr_t addr={0};
   uint8 ret=SUCCESS;

   mhi_osal_debug(MSG_MEDIUM, "mhi_core_issue_linkreq: Issuing linkreq, local_addr: 0x%x link_addr: %x%x size: %d", linkReq->local_addr, QW2DW(linkReq->link_addr), linkReq->size); 

/* Must increment counter here otherwise it may race with the callback, which may decrement the counter
 * for a valid completion even before the counter had a chance to get incremented */
   mhi_osal_syncenter(&mhiCtx.mhiSync); 
   /* Increment the counter that tracks the number of outstanding completions */
   mhiCtx.num_completions_pending++;
   mhi_osal_syncleave(&mhiCtx.mhiSync); 

   linkReq->status = PENDING; 
   linkReq->op=(MHI_READM==op||MHI_READ==op)?MHI_READ:MHI_WRITE;
   linkReq->time_issued_ms = mhi_osal_get_systime_ms();
   
   addr.va=(uint32)linkReq->local_addr;

   if(MHI_READ == op || MHI_READM == op) 
   {
      status = mhi_link_read(index,linkReq->link_addr,
            &addr,
            linkReq->size, (void*)linkReq);
   }
   else if(MHI_READ_NULL == op)
   {
      /* No read to read any data, it is a null read */
      status = MHI_SUCCESS;
   }
   else
   {      
      status = mhi_link_write(index,linkReq->link_addr,
            &addr,
            linkReq->size, (void*)linkReq);
   }

   /* Check status if request is completed, if it is invoke the cb function */
   if(MHI_SUCCESS == status || MHI_PENDING == status)
   {
      if (MHI_SUCCESS == status)
      {
         linkReq->status = SUCCESS; 
         mhi_core_process_ring_cb(linkReq->ring, linkReq);
      }
      linkReq->ring->issued_tres_count++;
   }
   else if(MHI_OP_FULL == status)
   {
      ret = LINK_BUSY; 
      goto err;
   }
   else
   {
      ret = LINK_ERROR;
      goto err;
   }

   return ret;

err:
   /* Decrememnt counter since request was not accepted by link */
   mhi_osal_syncenter(&mhiCtx.mhiSync); 
   mhiCtx.num_completions_pending--;
   mhi_osal_syncleave(&mhiCtx.mhiSync); 
   return ret;
}

/** 
 * @brief      mhi_core_set_channel_state
 *
 *             Set the channel state 
 *              
 * 
 * @param[in]  chId - Channel ID 
 * @param[in]  state - Channel state to set
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             SUCCESS if operation succeeded, ERROR otherwise
 * 
 * @sideeffect None 
 */
uint32 mhi_core_set_channel_state(uint32 chId, mhi_ch_state_t state )
{
   mhiCtx.lChCtxBase[chId].chstate=state;  

   if(SUCCESS != mhi_core_flush_ch_ctx(chId, 1))
      return ERROR;

   return SUCCESS; 
}

/** 
 * @brief      mhi_core_process_tr_element
 *
 *             Process ring element. Called by ring processor. This function will call mhi_core_process_tre,
 *             where the core of TRE processing is done 
 *              
 * 
 * @param[in]  *ring   - Ring 
 * @param[in]  *element - Transfer ring element  
 * @param[out] *event - Event object to populate
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             MHI_SUCCESS if operation succeeded  
 * 
 * @sideeffect None 
 */
uint32 mhi_core_process_tr_element(ring_t *ring, mhi_ring_element_t *element, event_t *event)
{
   return mhi_core_process_tre(ring, element);
}

/** 
 * @brief      mhi_core_process_tre
 *
 *             Perform the initial part of transfer ring element processing. 
 *             If there are available client requests and link requests, create a link request 
 *             tying the client buffer and host buffer (trb) and issue it to MHI Link. After completion 
 *             the function mhi_core_process_ring_completion will be invoked which will then generate an 
 *             event, if any, to host 
 *              
 * 
 * @param[in]  *ring - Ring 
 * @param[in]  *el - Ring element
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             SUCCESS if operation succeeded  
 *             SYSERR if system error occurred 
 *             LINK_BUSY if there are no link request objects or the link layer cannot accept any more
 *             REQUESTQ_EMPTY if there are no client requests 
 *             BADTRB - If the transfer ring buffer address is bad 
 * 
 * @sideeffect None 
 */
uint32 mhi_core_process_tre(ring_t *ring , mhi_ring_element_t *el )
{
   ch_t *ch = &mhiCtx.ch[ring->id];
   uint32 bytes_processed=0, bytes_remaining, trb_bytes_remaining, bytes_torw; 
   mhi_request_t *req = NULL;
   int32 ret=SUCCESS; 
   boolean last_op, eot=FALSE; 
   mhi_op_t op; 
   mhi_link_async_req_t *linkReq = NULL; 
   event_t newEvent;  

   /* If channel is not running we should not be processing the channel, return */
   if(MHI_CH_STATE_RUNNING != mhiCtx.lChCtxBase[ring->id].chstate)
      goto exit;  

   mhi_osal_debug(PROFILING_LOG|MSG_MEDIUM,"mhi_core_process_tre: process_tre=start, ch_id=%d",ring->id);
 
   /* Populate common transfer event info in case its used */
   newEvent.event_el.tEvent.chid=ring->id;
   newEvent.event_el.tEvent.code=MHI_CMD_CODE_INVALID;
   newEvent.event_el.tEvent.type=MHI_RING_EL_TYPE_RESERVED;
   newEvent.event_el.tEvent.ptr=ring->ringBase+(ring->rdOffset*sizeof(mhi_ring_element_t));
   newEvent.event_el.tEvent.len=0;

   /* BEI controls whether we generate an interrupt or not */
   newEvent.trigger_msi=!el->tr.bei;


   /* Validate TRB */ 
   if(SUCCESS != mhi_core_validateHostAddress(el->tr.ptr, ADDR_DATA)|| el->tr.len==0 || el->tr.type != MHI_RING_EL_TYPE_TRANSFER)
   {
      mhi_osal_syncenter(&mhiCtx.mhiSync); 
      /*  Grab a free link req. */
      mhi_q_dequeue(&mhiCtx.freeLinkReqQ, (list_t**)&linkReq);
      mhi_osal_syncleave(&mhiCtx.mhiSync); 

      /* If TRB is invalid make sure we have a linkreq. If not return LINK_BUSY any will process again later */
      if(NULL == linkReq)
      {
         ret = LINK_BUSY; 
         goto exit;
      }

      /* Set the channel to error state */
      if(SUCCESS != mhi_core_set_channel_state(ring->id, MHI_CH_STATE_ERROR))
      {
         ret = SYSERR;
         goto exit;
      }

      /* Generate transfer completion event, with error code */
      newEvent.event_el.tEvent.type=MHI_RING_EL_TYPE_TRANSFER_EVENT;
      newEvent.event_el.tEvent.code=MHI_CMD_CODE_RING_EL_ERROR;

      /* Override BEI control for RING_EL_ERROR */
      newEvent.trigger_msi=1;

      linkReq->local_addr=0; 
      linkReq->link_addr=0; 
      linkReq->status=SUCCESS; 
      linkReq->ring=ring; 
      linkReq->event=newEvent; 
      linkReq->req=NULL;
      linkReq->time_issued_ms = mhi_osal_get_systime_ms();

      mhi_q_queue(&ring->linkReqQ, (list_t*)linkReq); 

#ifndef MHI_BOOT
      /* Increment counter since mhi_core_process_ring_cb will decrement it */
      mhi_osal_syncenter(&mhiCtx.mhiSync); 
      mhiCtx.num_completions_pending++;
      mhi_osal_syncleave(&mhiCtx.mhiSync); 
#endif

      /* Trigger CB so we can generate the error event */ 
      mhi_core_process_ring_cb(ring, linkReq);   

      ret = BADTRB; 
      goto exit; 
   }

   /* Check if we need to advance to next TD boundary. If so return succes */
   if(0 != ch->skip_to_td)
   {
      /* If chain bit is not set we have reached end of TD.  
       * clear the skip_to_TD bit */
      if(0 == el->tr.chain)
      {
         ch->skip_to_td=0;
         ring->interruptable=TRUE;
      }

      ret = SUCCESS;
      goto exit;
   }

   /* Loop over client requests and advance the processing of the current TRE as long as work can be done. 
    * Break conditions include: 
    * a) Completely processing TRB or EOT condition
    * b) Running out of client requests 
    * c) Running out of link requests 
    * */
   for(;;)
   {
      /* Check client state, Don't grab mutex because channel might be in process of getting destroyed */ 
      if(CH_OPENED != ch->ch_client_state)
      {
         mhi_osal_debug(MSG_HIGH, "mhi_core_process_tre: Channel is not opened, pending close ch: %d ",ring->id);

         /* No client requests, cant process TRE , return */
         ret = REQUESTQ_EMPTY; 
         goto exit; 
      }

      mhi_osal_syncenter(&ch->chSync);

      /* Check client state again. Channel could be in pending CLOSE.  
       * If not opened return REQUESTQ_EMPTY so that we do not attempt to process again until more new requests are available */
      if(CH_OPENED != ch->ch_client_state)
      {
         mhi_osal_syncleave(&ch->chSync);

         mhi_osal_debug(MSG_HIGH, "mhi_core_process_tre: Channel is not opened, pending close ch: %d ",ring->id);
         /* No client requests, cant process TRB , return */
         ret = REQUESTQ_EMPTY; 
         goto exit; 
      }

      /* Get pointer to channels pending request */
      mhi_q_peek_head(&ch->request_q, (list_t**)&req);

      /* If no more requests in queue, return error */
      if(NULL == req)
      {
         mhi_osal_syncleave(&ch->chSync);
         mhi_osal_debug(MSG_MEDIUM, "mhi_core_process_tre: Request Q empty. ch: %d ",ring->id);

         ret = REQUESTQ_EMPTY; 
         goto exit; 
      }

      /* If channel was stalled in M3 clear the global M3 wake counter */
      if (ch->stalled)
      {
         ch->stalled=FALSE;
         ch->wakeCount=0;
         mhi_osal_debug(MSG_HIGH, "mhi_core_process_tre: Clearing stalled channel flag due to data flow ch:%d, and clearing ch wake counter", ring->id);
      }

      /*  Grab a new link request. If there is none return LINK_BUSY */
      mhi_osal_syncenter(&mhiCtx.mhiSync); 
      mhi_q_dequeue(&mhiCtx.freeLinkReqQ, (list_t**)&linkReq);
      mhi_osal_syncleave(&mhiCtx.mhiSync); 

      if(NULL == linkReq)
      {
         mhi_osal_syncleave(&ch->chSync);
         mhi_osal_debug(MSG_HIGH, "mhi_core_process_tre: No more link requests ring id: %d ",ring->id);
         ret = LINK_BUSY; 
         goto exit; 
      }

      op=req->op; 
      bytes_processed=req->ret.payload.rw.bytesRW;

      /* Calculate bytes remaining for clients request and for current TRB */
      bytes_remaining=req->params.rw.size-bytes_processed; 
      trb_bytes_remaining=el->tr.len - ch->trb_bytes_processed;

      /* Determine if last operation or not based on request */ 
      last_op = req->params.rw.last_op;

      mhi_osal_debug(MSG_MEDIUM, "mhi_core_process_tre: Processing TRB, ch: %d, bytes remaining: %x, bytes processed %x",ring->id, bytes_remaining, bytes_processed );
      if(MHI_WRITE == op || MHI_WRITEM == op) 
      {

         /* Determine size to write which will be request size up to max TRB size */
         bytes_torw= bytes_remaining>trb_bytes_remaining? trb_bytes_remaining:bytes_remaining;

         /* Calculate remainder and determine if any event is necessary */
         req->ret.payload.rw.bytesRW+=bytes_torw; 
         bytes_remaining-=bytes_torw; 

         if(0 == bytes_remaining)
         {  
            /* ack the write */ 
            req->ret.status=MHI_SUCCESS; 

            /* If we finished the request and its the last OP, EOT condition has happened 
             */ 
            if(1 == last_op)
            {

               /* Generate transfer completion event if iEOT bit is set in TRE */
               if(1 == el->tr.ieot)
               {
                  newEvent.event_el.tEvent.type=MHI_RING_EL_TYPE_TRANSFER_EVENT;
                  newEvent.event_el.tEvent.code=MHI_CMD_CODE_EOT;
                  newEvent.event_el.tEvent.len=ch->trb_bytes_processed+bytes_torw; 
               }

               /* Set flag in channel to indicate we need to move to next TD boundary if we are not already at end */ 
               if(0 != el->tr.chain)
               {
                  ch->skip_to_td=1;
                  
                  /* Mark the ring as non interruptable so that we will process down to end of TD or wroffset without 
                   * fetching and updating the write pointer */
                  ring->interruptable=FALSE;
               }

               eot=TRUE; 
            }
         }
      }
      else if (MHI_READ == op || MHI_READM == op || MHI_READ_NULL == op)
      {
         /* Calculate bytes to read. This will be min of request or TRB bytes remaining */
         bytes_torw= bytes_remaining>trb_bytes_remaining? trb_bytes_remaining:bytes_remaining;

         /* Update request processed bytes. */
         req->ret.payload.rw.bytesRW+=bytes_torw; 
         bytes_remaining-=bytes_torw; 
 
         /* Check for request completion  
            A request is completed if we read all the bytes. 
            or we reached end of a TD (EOT on outbound direction)  */
         if(0 == bytes_remaining || (bytes_torw == trb_bytes_remaining && 0 == el->tr.chain))
         {
            /* ack the read */ 
            req->ret.status=MHI_SUCCESS; 

            if(bytes_torw == trb_bytes_remaining && 0 == el->tr.chain) 
            {
               /* IF EOT happened override last_op so we can do a cb to client  */
               req->params.rw.last_op=TRUE; 
               req->ret.payload.rw.EOT=TRUE;

               if(1 == el->tr.ieot)
               {
                  /* Generate transfer completion event only if we reached end of TD */
                  newEvent.event_el.tEvent.type=MHI_RING_EL_TYPE_TRANSFER_EVENT;
                  newEvent.event_el.tEvent.code=MHI_CMD_CODE_EOT;
                  newEvent.event_el.tEvent.len=ch->trb_bytes_processed+bytes_torw; 
               }
            }
         }
      }
      else
      {
         mhi_osal_syncleave(&ch->chSync);
         mhi_osal_debug(MSG_FATAL, "mhi_core_process_tre: Unknown request in queue, possible corruption");
         mhi_core_sys_error();
         ret=SYSERR; 
         goto exit; 
      }

      /* Check if TRB is completely processed. 
       * If it is and there wasn't an EOT event, generate IEOB (EOT overrides EOB) only if the 
       * client request is completed as well (inbound only). Otherwise it is an overflow event if we reached end of a TD */ 
      if(bytes_torw == trb_bytes_remaining && MHI_CMD_CODE_EOT != newEvent.event_el.tEvent.code)
      {
         if(FALSE==eot && 0 == el->tr.chain && 0 != (ring->id%2))
         {
            /* Generate overflow completion event */
            newEvent.event_el.tEvent.type=MHI_RING_EL_TYPE_TRANSFER_EVENT;
            newEvent.event_el.tEvent.code=MHI_CMD_CODE_OVERFLOW;
            newEvent.event_el.tEvent.len=el->tr.len; 
         }
         else
         {
            if(1 == el->tr.ieob)
            {
               /* Generate transfer completion event */
               newEvent.event_el.tEvent.type=MHI_RING_EL_TYPE_TRANSFER_EVENT;
               newEvent.event_el.tEvent.code=MHI_CMD_CODE_EOB;
               newEvent.event_el.tEvent.len=el->tr.len; 
            }
         }
      }

      /* Issue link request to write/read data to TRB */
      linkReq->local_addr=(uint32)(req->params.rw.data+bytes_processed); 
      linkReq->link_addr=el->tr.ptr+ch->trb_bytes_processed; 
      linkReq->size=bytes_torw; 
      linkReq->ring=ring; 
      linkReq->req = (req->ret.status == MHI_SUCCESS?req:NULL); 
      linkReq->status=PENDING; 
      linkReq->event=newEvent; 
      linkReq->priority=(ring->priority<=RING_PRIORITY_CONTROL)?HIGH:LOW;

      /* Queue the link request to ring */
      mhi_q_queue(&linkReq->ring->linkReqQ, (list_t*)linkReq);

      mhi_osal_debug(PROFILING_LOG|MSG_MEDIUM, "mhi_core_process_tre: Issuing linkreq, local_addr=0x%x, size=%d, ch_id=%d, req_userdata=0x%x", linkReq->local_addr, linkReq->size, ring->id, req->cb.userData); 

      /* Issue link request to link */
      ret = mhi_core_issue_linkreq(ADDR_DATA, linkReq, op); 

      if (LINK_BUSY == ret)
      { 
         mhi_osal_debug(MSG_HIGH, "mhi_core_process_tre: mhi_core_issue_linkreq reported LINK_BUSY");
         /* Revert req bytesRW */
         req->ret.payload.rw.bytesRW-=bytes_torw;
         /* Put the linkReq back on the free Q */
         mhi_q_remove((list_t*)linkReq);
         mhi_q_queue(&mhiCtx.freeLinkReqQ, (list_t*)linkReq);
         mhi_osal_syncleave(&ch->chSync);
         goto exit;
      }
      else if(SUCCESS != ret)
      {
         mhi_osal_debug(MSG_ERR, "mhi_core_process_tre: Failed to issue r/w link req wih ret=%d.  Returning SYSTEM error",ret);
         ret=SYSERR;
         mhi_osal_syncleave(&ch->chSync);
         goto exit;
      }

      /* if request is completed remove from request queue */
      if(MHI_SUCCESS == req->ret.status)
      {
         /* Remove request from channel queue and put it on the issued queue. This is not needed since its also tied to linkReq, but it will aid in debug 
          * to have outstanding issued request queue */  
         mhi_q_remove((list_t*)req);
         mhi_q_queue(&ch->request_q_issued ,(list_t*)req);
      }

      /* Update client packet size */
      ch->total_payload_size+=bytes_torw;
      if (MHI_CMD_CODE_EOT == newEvent.event_el.tEvent.code)
      {
         mhi_osal_debug(PROFILING_LOG|MSG_HIGH, "mhi_core_process_tre: ch_id=%d, processed_total_payload_size=%d, req_userdata=0x%x", ring->id, ch->total_payload_size, req->cb.userData);
         ch->total_payload_size=0;
      }

      mhi_osal_syncleave(&ch->chSync);

      /* Check for completion of TRB. If we are done reset the meta data and return. Otherwise continue in processing 
       * loop which will fetch the next client request */
      if(bytes_torw < trb_bytes_remaining && 0 == eot)
      {
         ch->trb_bytes_processed+=bytes_torw;
      } 
      else
      {
         ch->trb_bytes_processed=0;
         break;
      }
   } /* for(;;) TRB processing loop */

exit:
  /* Do not log exit for profiling if no request is in flight */
  if (req != NULL)
    mhi_osal_debug(PROFILING_LOG|MSG_MEDIUM,"mhi_core_process_tre: process_tre=end, ch_id=%d, req_userdata=0x%x",ring->id,req->cb.userData);

   return ret; 
}

/** 
 * @brief      mhi_core_set_ringstate
 *
 *             Set the state of a ring. Along with settings state it puts the ring on the respective
 *             ring queue for that state.
 *              
 * 
 * @param[in]  *ring - Ring 
 * @param[in]  state - State
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
void mhi_core_set_ringstate(ring_t *ring, ring_state_t state)
{
   if ( NULL != ring->list.next || NULL != ring->list.prev)
      mhi_osal_debug(MSG_FATAL, "mhi_core_set_ring_state: Ring still on queue" ); 

   switch(state)
   {
      case RING_PENDING:
         ring->state = RING_PENDING;
         mhi_core_queue_pending_ring(ring);  
         break; 
      case RING_WAITREL: 
         ring->state = RING_WAITREL; 
         break;
      case RING_WAITREQ: 
         ring->state = RING_WAITREQ;
         break;
      case RING_WAITLINK: 
         mhi_q_queue(&mhiCtx.waitLinkRingQ, &ring->list); 
         ring->state = RING_WAITLINK;
         break;
      case RING_UNINIT:
         ring->state = RING_UNINIT;
         break;
      default:
         /* Constant value is passed in so this not possible */
         ;
   }

   ring->ring_state_log[ring->state].count++;

   return; 
}
 

/** 
 * @brief      mhi_core_sys_error
 *
 *             Set MHI to system error state
 *              
 *
 *
 * @dependencies
 *             Should be called from MHI core thread which handles the state machine
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_core_sys_error(void)
{
   mhi_ring_element_t event; 

   /* If we are already in system error state and is_syserr_pending is false, return */
   if (mhi_sm_get_mhistate() == MHI_STATE_SYSERR && !mhiCtx.is_syserr_pending) 
      return; 

   if (!mhiCtx.mhi_link_info.mhi_link_hw_enabled)
   {
      mhi_osal_debug(MSG_FATAL, "mhi_core_sys_error: SYSERR attempted while link hw disabled");
      return;
   }
   
   /* Set the MHI state to SYSERR */
   mhi_sm_set_syserr_mhistate();

   /* Trigger event */
   event.stateChgEvent.mhistate = MHI_STATE_SYSERR;
   event.stateChgEvent.type=MHI_RING_EL_TYPE_MHI_STATE_CHG;

   /* Flush any pending interrupts */ 
   mhi_core_flush_interrupts();

   /* MHI system error. Used for usually non transfer ring related errors like event/command ring */
#ifdef MHI_FATAL_ON_SYSERR
   mhi_osal_debug(MSG_FATAL, "--------------MHI system error------------");
#else
   mhi_osal_debug(MSG_ERR, "--------------MHI system error------------");
#endif
   
   if (mhi_pm_get_d_state() != MHI_LINK_D0)
   {
      mhi_osal_debug(MSG_ERR, "mhi_core_sys_error: Device not in D0, unable to send notification to host");
      return;
   }
   
   if(MHI_SUCCESS != mhi_core_send_event(PRIMARY_ER,&event, 1))
   {
      mhi_osal_debug(MSG_HIGH, "mhi_core_sys_error: Could not generate an event, triggering interrupt only instead"); 

      if(mhi_link_triggerinterrupt(mhiCtx.lErCtxBase[PRIMARY_ER].msivec))
      {
         mhi_osal_debug(MSG_ERR, "mhi_core_sys_error: Failed to generate MSI interrupt");
      }
   }

   mhiCtx.is_syserr_pending = FALSE;
}

#ifndef MHI_BOOT
/* Callback invoked on expiry of the link request completion timer */
void mhi_core_link_completion_timer_cb(int32 time_ms, mhi_timer_user_data data)
{
  ring_t *ring;

  ring=(ring_t *)data;

  mhi_osal_debug(MSG_HIGH,"mhi_core_link_completion_timer_cb: Enter");

  /* If ring pointer is supplied check for completions of that ring only. If ring pointer
   * i.e. data is NULL, completions for all rings should have been received */
  if (mhi_core_are_link_completions_pending(ring))
  {
    /* Error if completions are not received within timeout */
     mhi_osal_debug(MSG_ERR,"mhi_core_link_completion_timer_cb: Timeout");
     mhi_osal_eventsignal(mhiCtx.hNoMoreCompletions);
  }
}

/** 
 * @brief      mhi_core_suspend
 *
 *             Change the state of valid channels to suspended
 *              
 *
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             SUCCESS/ERROR
 * 
 * @sideeffect None 
 */
uint32 mhi_core_suspend(void)
{
  int i;

  mhi_osal_debug(MSG_HIGH,"mhi_core_suspend: Enter");

  /* Flush all pending link completions before suspending channels */
  if (SUCCESS != mhi_core_flush_link_completions(NULL))
  {
     mhi_osal_debug(MSG_ERR, "mhi_core_suspend: Failed to flush link completions");
     return ERROR;
  }
   
  for(i=0;i<mhiCtx.mhiConfig.nCh;i++)
  {
    ring_t *ring = &mhiCtx.tr[i];

    /* If channel is in running state, suspend */
    if(MHI_CH_STATE_RUNNING == mhiCtx.lChCtxBase[i].chstate)
    {
       /* Save read pointers to channel context */
       mhiCtx.lChCtxBase[i].rp =  ring->ringBase + (ring->rdOffset*sizeof(mhi_ring_element_t));

       /* Set the channel to suspend state  */
       mhi_core_set_channel_state(i, MHI_CH_STATE_SUSPENDED);
    }
  }

  if (SUCCESS != mhi_core_flush_interrupts())
  {
     mhi_osal_debug(MSG_ERR, "mhi_core_suspend: Failed to flush interrupts");
     return ERROR;
  }

  return SUCCESS;
}
  
/** 
 * @brief      mhi_core_resume
 *
 *             Change the state of suspended channels to running
 *              
 *
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             SUCCESS/ERROR
 * 
 * @sideeffect None 
 */
uint32 mhi_core_resume(void)
{
  int i;
  
  mhi_osal_debug(MSG_HIGH,"mhi_core_resume: Enter");

  for(i=0;i<mhiCtx.mhiConfig.nCh;i++)
  {
    /* If channel is not disabled and is suspended then resume */
    if(MHI_CH_STATE_SUSPENDED == mhiCtx.lChCtxBase[i].chstate)
    {
       /* Set the channel to running state  */
       if (SUCCESS != mhi_core_set_channel_state(i, MHI_CH_STATE_RUNNING))
       {
          mhi_osal_debug(MSG_ERR, "mhi_core_resume: Failed to set channel state for channel %d",i); 
          return ERROR;
       }
    }
  }

  return SUCCESS;
}    
#endif /* #ifndef MHI_BOOT */

/*
* @brief      mhi_core_map_ctrl_and_base_regions
*
*             Map the host control and data regions to the local address space.
*             
* 
* @paran[in]  hostAddr - MHI host address configuration
* @paran[in]  *localAddr - Pointer to local MHI address configuration
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
mhi_status_t mhi_core_map_ctrl_and_base_regions(mhi_host_addr_t hostAddr, mhi_local_addr_t *localAddr) 
{
   uint64 base[2]; 
   uint64 limit[2];
   mhi_addr_t *local[2]; 

   /* Map host address spaces */
   base[0]=hostAddr.mhictrlbase;
   limit[0]=hostAddr.mhictrllimit;
   local[0]=&localAddr->mhictrl;

   base[1]=hostAddr.mhidatabase;
   limit[1]=hostAddr.mhidatalimit;
   local[1]=&localAddr->mhidata;

   return mhi_link_maphostaddrspace(base, limit, local,  2);
}

/*
* @brief      mhi_core_process_idle_tasks
*
*             Process tasks that need to be done after MHI is done doing useful
*             work, such as: processing rings or power state transitions. 
*             
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
void mhi_core_process_idle_tasks(void)
{
   /* If there are pending DMA requests, flush them */
   if (mhiCtx.mhi_link_info.is_DMA_flush_needed)
   {
      mhi_link_dma_flush();
      mhiCtx.mhi_link_info.is_DMA_flush_needed=FALSE;
   }
  
#ifndef MHI_BOOT
   /* Rearm link inactivity timer if conditions apply */
   if (MHI_STATE_M0 == mhiCtx.mhi_sm_info.mhiState && LOW == mhiCtx.mhi_sm_info.device_wake)
      mhi_link_enable_inactivity_intr(MAX_INACTIVITY);
#endif   
}

/*
* @brief      mhi_core_is_work_pending
*
*             Check whether MHI has pending work 
*             
*
* @dependencies
*             None
*              
* 
* @return       
*             TRUE/FALSE
* 
* @sideeffect None 
*/
boolean mhi_core_is_work_pending(void)
{
   mhi_core_ack_interrupts();

   /* Log which rings are in the pendingRingQ and completeRingQ */
   if (!mhi_q_is_queue_empty(&mhiCtx.pendingRingQ))
   {
      mhi_osal_debug(MSG_HIGH,"mhi_core_ack_interrupts: pendingRingQ is not empty");
      mhi_core_log_ring_ids_on_Q(&mhiCtx.pendingRingQ, 0);  
   }

   mhi_osal_syncenter(&mhiCtx.mhiSync);  
   if (!mhi_q_is_queue_empty(&mhiCtx.completeRingQ))
   {
      mhi_osal_debug(MSG_HIGH,"mhi_core_ack_interrupts: completeRingQ is not empty");
      mhi_core_log_ring_ids_on_Q(&mhiCtx.completeRingQ, 1);  
   }

   if (!mhi_q_is_queue_empty(&mhiCtx.waitAckRingQ))
   {
      mhi_osal_debug(MSG_HIGH,"mhi_core_ack_interrupts: waitAckRingQ is not empty");
      mhi_core_log_ring_ids_on_Q(&mhiCtx.waitAckRingQ, 2);  
   }

   mhi_osal_syncleave(&mhiCtx.mhiSync); 
   
   return mhiCtx.is_mhi_work_pending;
}
  

/*
* @brief      mhi_core_log_ring_ids_on_Q
*
*             Walk the queue supplied and log all ring ids that are enqueued on it 
*             
* @params[in] q - Pointer to the queue
* @params[in] offset - Offset of ring pointer relative to the base of the element
*
* @dependencies
*             Address of list must coincide with that of the ring
*              
* 
* @return       
*             SUCCESS/ERROR
* 
* @sideeffect None 
*/
uint32 mhi_core_log_ring_ids_on_Q(list_t *q, uint32 offset)
{
   uint32 rings_onQ[4]={0};
   ring_t *ring=NULL;
   
   if (NULL == q)
      return ERROR;

   while(SUCCESS == mhi_q_get_next(q,(list_t **)&ring) && NULL != ring)
   {
      ring=(ring_t*)((list_t*)ring-offset); 
      rings_onQ[ring->id/32]|=1<<(ring->id%32);
      /* Restore the pointer to queued element before attempting to get next */
      ring=(ring_t*)((list_t*)ring+offset);
   }
   mhi_osal_debug(MSG_HIGH,"mhi_core_ack_interrupts: Rings on Q: %x %x %x %x",
                           rings_onQ[0],rings_onQ[1],rings_onQ[2],rings_onQ[3]);
   return SUCCESS;
} 

/*
* @brief      mhi_core_flush_link_completions
*
*             Flush all completions from pending link requests 
*             
* 
* @param[in]  ring - Ring for which completions need to be flushed. If ring is NULL, 
*                    completions for all channels will be flushed.
*
* @dependencies
*             None
*              
* 
* @return       
*             SUCCESS - Flush succeeded 
*             SYSERR - Flush failed
* 
* @sideeffect 
*             None 
*/
uint32 mhi_core_flush_link_completions(ring_t *ring)
{
  mhi_osal_debug(MSG_MEDIUM,"mhi_core_flush_link_completions: Enter");
 
  /* Nothing to do if ring is uninitialized */
  if (NULL != ring && RING_UNINIT == ring->state)
  {
     return SUCCESS;
  }

  /* Check if there are any link requests pending completions */ 
  mhi_osal_syncenter(&mhiCtx.mhiSync);
  if (mhi_core_are_link_completions_pending(ring))
  {      
      /* Create event for circumstance when there are no more completions pending */   
      if(MHI_SUCCESS != mhi_osal_eventcreate(&mhiCtx.hNoMoreCompletions, NULL) )
      {
        mhi_osal_debug(MSG_FATAL,"mhi_core_suspend: Couldn't create event. Fatal error");
        return SYSERR; 
      }
  }
  mhi_osal_syncleave(&mhiCtx.mhiSync);

  /* If the event was created to track the occurance of the last pending completion, start the 
   * link completion timer and wait for the event */  
  if (NULL != mhiCtx.hNoMoreCompletions)
  {
      mhi_osal_debug(MSG_HIGH,"mhi_sm_run_state_machine: Starting completion timer, completions pending = %d",
                                                                                             mhiCtx.num_completions_pending);
#ifndef MHI_BOOT
      /* Start timer for checking number of outstanding completions from the link */
      mhi_osal_timer_start(&mhiCtx.link_completion_timer,mhi_core_link_completion_timer_cb,NULL,mhiCtx.mhi_core_tgt_config->link_completion_timeout,NULL);
#endif
      /* Process received completions if possible, while waiting for pending completions */
      while (mhi_core_are_link_completions_pending(ring) && FALSE == mhi_q_is_queue_empty(&mhiCtx.completeRingQ))
      {
         mhi_core_handle_single_ring_completion();
      }

      /* Wait for event if link completions are still pending */
      if (mhi_core_are_link_completions_pending(ring))
      {
         mhi_osal_eventwait(mhiCtx.hNoMoreCompletions);
      }

      /* If we reach this point, all pending completions have been received */
      mhi_osal_eventdestroy(&mhiCtx.hNoMoreCompletions);
      mhi_osal_debug(MSG_MEDIUM,"mhi_core_flush_link_completions: No more pending link request completions");
#ifndef MHI_BOOT
      mhi_osal_timer_stop(&mhiCtx.link_completion_timer);
#endif

      /* Flush pending completions if any */
      if (NULL != ring && FALSE == mhi_q_is_queue_empty(&ring->linkReqQ))
      {
         if (SUCCESS != mhi_core_process_ring_completion(ring))
         {
            mhi_osal_debug(MSG_ERR,"mhi_core_flush_link_completions: Failed to process completions for ring %d",
                                                                      ring->id);
            return SYSERR;
         }
      }
      else if (NULL == ring)
      {
         while (!mhi_q_is_queue_empty(&mhiCtx.completeRingQ))
         {
            mhi_core_handle_single_ring_completion();
         }
      }

      /* If there are completions still pending, that means event signaled due to timeout */
      if (mhi_core_are_link_completions_pending(ring))
      {
         mhi_osal_debug(MSG_ERR,"mhi_core_flush_link_completions: Failed to flush completions, %d still pending",
                                                mhiCtx.num_completions_pending);
         return SYSERR;
      }
  }

  return SUCCESS;
}

/*
* @brief      mhi_core_are_link_completions_pending
*
*             Check if link competions are pending for single ring, or all rings if ch_ring is NULL 
*             
* 
* @param[in]  ring - Ring for which to check completions
*
* @dependencies
*             None
* 
* @return       
*             TRUE/FALSE
* 
* @sideeffect 
*             None 
*/
boolean mhi_core_are_link_completions_pending(ring_t * ring)
{
   return ((NULL != ring && FALSE == mhi_q_is_queue_empty(&ring->linkReqQ)) || \
           (NULL == ring && mhiCtx.num_completions_pending != 0));
}

/** 
 * @brief      mhi_core_recover_ch
 *
 *             Recover a channel using sequence number based reliability mechanism
 *              
 * 
 * @param[in]  *ch - Channel to recover
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
void mhi_core_recover_ch(uint32 chid)
{
   mhi_link_async_req_t *linkReq;
   list_t *requestQHead; 
   boolean recoveryComplete=FALSE;
   ring_t *ring;
   ch_t *ch;

   ch=&mhiCtx.ch[chid];

   ring=&mhiCtx.tr[chid];

   /* If channel is not reliable or the channel is not opened nothing to recover */
   if(!ch->reliable || ch->ch_client_state != CH_OPENED)
   {
      return; 
   }
   /* Mark the head of request queue */
   requestQHead=ch->request_q.next;

   while(ch->ack_device < ch->seq_device )
   {

      if(ch->ack_device==ch->seq_host)
         recoveryComplete=TRUE; 

      mhi_q_dequeue(&ring->linkReqWaitAckQ, (list_t**)&linkReq);

      if(NULL == linkReq)
      {
         mhi_osal_debug(MSG_ERR, ": Host send bogus sequence number in start command");
         break;
      }

      if(MHI_CMD_CODE_INVALID != linkReq->event.event_el.tEvent.code)
      {
         ch->ack_device++; 
      }

      if(NULL != linkReq->req)
      {  
         mhi_q_remove(&linkReq->req->list);

         if(!recoveryComplete)
         {
            mhi_core_complete_req(ch, ring, linkReq->req, MHI_SUCCESS);
            mhi_q_queue(&ch->request_free_q, &linkReq->req->list);
         }
         else
         {
            mhi_q_insert_before(requestQHead, &linkReq->req->list);
         }
      }

      /* Relinquish the linkReq */
      mhi_q_remove(&linkReq->list);
      mhi_q_queue(&mhiCtx.freeLinkReqQ, &linkReq->list);
   }

   /* Mark these as equal if there was an error during recovery */
   ch->ack_device=ch->seq_device;
}

