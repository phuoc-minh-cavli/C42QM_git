/**
  @file mhi_ring.c
  @brief
  This file contains the implementation of the MHI ring processing 
    

*/
/*
===============================================================================

                             Edit History

 $Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/06/15   nk      REV E Changes
01/06/15   nk      Add ring reliability
10/01/14   am      Init ring to known pattern
05/22/14   nk      Fix to not clear interrupt when checking for throttling
03/20/14   nk      Add interruptable field to ring
02/10/14   am      Added MHI Spec Rev D changes and tweaked logging priority
10/22/13   nk      New algorithm for ring processing, improvements, fixes, DMA support
06/25/13   nk      IPA initalization integration 
01/15/13   nk      Created
===============================================================================
                   Copyright (c) 2013-2015 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/

#include "mhi_link.h"
#include "mhi_ring.h"
#include "mhi_i.h"
#include "mhi_osal.h"
#include "mhi_core.h"
#include "mhi_util.h"

/* Control channels */
#define CTRL_OUT 14
#define CTRL_IN  15

#define CTRL_OUT_2  16
#define CTRL_IN_2   17

/* EFS channels */
#define EFS_OUT 10
#define EFS_IN  11
/* SW loopback channels */
#define SW_LOOPBACK_OUT 0
#define SW_LOOPBACK_IN  1

/* Max ring size in ring elements */ 
#define MAX_RING_SIZE 0x10000


/* Local prototypes */ 
uint32 mhi_ring_cache(ring_t *ring, uint32 wrOffset);
uint32 mhi_ring_validate_ctx(ring_type_t type, mhi_ring_ctx_t *ctx);
uint32 mhi_ring_process_rel(ring_t *ring, uint32 offset );

/** 
 * @brief      mhi_ring_cache
 *
 *             Cache MHI Ring 
 *              
 * 
 * @param[in]  *ring   - Ring to cache
 * @param[in]  wrOffset - Write offset up until to cache too
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             SUCCESS if succeeded, otherwise ERROR
 * 
 * @sideeffect None 
 */
uint32 mhi_ring_cache(ring_t *ring, uint32 wrOffset) 
{
   uint32 oldOffset, cache_len, ret=0;
   mhi_addr_t addr={0};

   /* For all rings other than event ring, cache TRB's  */
   if(RING_TYPE_ER == ring->type)
   {
      return SUCCESS; 
   }   

   oldOffset=ring->wrOffset; 

   /* Cache only new TRB's  */
   if(oldOffset!=wrOffset)
   {
      /* Read to wrOFfset or end of ring */
      cache_len=(wrOffset<oldOffset?ring->rSize-oldOffset:wrOffset-oldOffset);

      /* Cache the TRB's, handle wraparound case  */
      addr.va=(uint32)&ring->lRingBase[oldOffset];
      ret = mhi_link_read(ADDR_CTRL,ring->ringBase+(oldOffset*sizeof(mhi_ring_element_t)),
            &addr,
            cache_len*sizeof(mhi_ring_element_t), NULL);

      /* Check for failure. Return code should be SUCCESS */
      if(SUCCESS != ret) 
      {
         goto err;
      }

      if(wrOffset<oldOffset)
      {
         addr.va=(uint32)&ring->lRingBase[0];
         ret = mhi_link_read(ADDR_CTRL,ring->ringBase,
               &addr,
               ring->wrOffset*sizeof(mhi_ring_element_t), NULL);

         /* Check for failure. Return code should be SUCCESS */
         if(SUCCESS != ret)
         {
            goto err; 
         }

      }

   }

   return ret;  

err:
   return ERROR; 
}

/** 
 * @brief      mhi_ring_validate_ctx 
 *
 *             Validates a rings context provided by host 
 *              
 *
 *
 * @dependencies
 *              
 * 
 * @return       
 *              SUCCESS if operation succeeded, otherwise ERROR
 * 
 * @sideeffect  None 
 */
uint32 mhi_ring_validate_ctx(ring_type_t type, mhi_ring_ctx_t *ctx)
{
   uint32 status = SUCCESS; 
   uint64 rlen, rp , wp, rbase;

   rbase=ctx->gen.rbase;
   rp=ctx->gen.rp;
   wp=ctx->gen.wp;
   rlen=ctx->gen.rlen; 

   if(0 == rlen)
   {
      status++; 
      mhi_osal_debug(MSG_ERR, "mhi_ring_validate_ctx: Validation of ring context failed. Ring length is 0"); 
   }

   if(0 != (rlen%sizeof(mhi_ring_element_t)))
   {
      status++;    
      mhi_osal_debug(MSG_ERR, "mhi_ring_validate_ctx: Validation failed: Ring length: %x%x is invalid \
            It must be multiple of ring element size",QW2DW(rlen));  
   }

   if(wp<rbase || rp<rbase)
   {
      status++;
      mhi_osal_debug(MSG_ERR, "mhi_ring_validate_ctx: Validation failed: RP: %x%x WP: %x%x is invalid. \
            It must be greater than ring base",QW2DW(rp), QW2DW(wp));            
   }

   if((wp-rbase)>rlen || (rp-rbase)>rlen)
   {     
      mhi_osal_debug(MSG_ERR, "mhi_ring_validate_ctx: Validation failed: RP: %x%x WP: %x%x is invalid. \
            It must be less than ring size",QW2DW(rp), QW2DW(wp)); 
   }

   if(RING_TYPE_TR == type && ctx->ch.erindex >= mhiCtx.mhiConfig.nEr)
   {
      mhi_osal_debug(MSG_ERR, "mhi_ring_validate_ctx: Validation failed, erindex is: %d", ctx->ch.erindex);
   }

   if(SUCCESS != status)
      status = ERROR;  

   return status; 
}

/** 
 * @brief      mhi_ring_validate_rel
 *
 *             This function validates a ring element. If invalid it will generate an appropriate error.  
 *              
 * 
 * @param[in]  *ring  
 * @param[in]  *element
 *
 * @dependencies
 *              
 * 
 * @return       
 *              ERROR if operation failed otherwise SUCCESS 
 * 
 * @sideeffect  None 
 */
int32 mhi_ring_validate_rel(ring_t *ring, mhi_ring_element_t *element)
{
   switch(element->rel.type)
   {
      case MHI_RING_EL_TYPE_TRANSFER:
         if(RING_TYPE_TR != ring->type)
         {
            mhi_osal_debug(MSG_ERR,"mhi_ring_validate_rel:Transfer TRB received on non transfer ring: %x", ring);
            return ERROR;
         }
         break;
      case MHI_RING_EL_TYPE_NOOP:
         /* Only command ring supports noop  */
         if(RING_TYPE_CR != ring->type)
         {
            mhi_osal_debug(MSG_ERR,"mhi_ring_validate_rel:Noop element received on unexpected ring: %x", ring);
            return ERROR;
         }
         break;
      case MHI_RING_EL_TYPE_RESET_CH_CMD:
      case MHI_RING_EL_TYPE_STOP_CH_CMD:
      case MHI_RING_EL_TYPE_START_CH_CMD:
         if(element->cmdReset.chid > HWCHANNELBASEEND)
         {
            mhi_osal_debug(MSG_ERR,"mhi_ring_validate_rel:Channel command has invalid channel number: %x", ring);
            return ERROR; 
         }
         if(RING_TYPE_CR != ring->type)
         {
            mhi_osal_debug(MSG_ERR,"mhi_ring_validate_rel:Command received on non command ring: %x", ring);
            return ERROR;
         }
         break;
      case MHI_RING_EL_TYPE_START_ALL_CHS_CMD:
         break;
      default:
         mhi_osal_debug(MSG_ERR,"mhi_ring_validate_rel:Unknown ring element received on ring: %x" , ring);
         return ERROR; 
   }

   return SUCCESS;
}

/** 
 * @brief      mhi_ring_get_priority
 *
 *             Returns a rings priority. Currently command ring is highest. Second comes the control channel,
 *             and then all other rings. In future we can have a table of properties per ring. 
 *              
 *
 *
 * @dependencies
 *              
 * 
 * @return       
 *              Ring priority 
 * 
 * @sideeffect  None 
 */
uint32 mhi_ring_get_priority(ring_t *ring)
{
   if(RING_TYPE_CR == ring->type) 
   {
      return RING_PRIORITY_MAX; 
   }
   else if(RING_TYPE_TR == ring->type)
   {
      if( CTRL_IN == ring->id || CTRL_OUT == ring->id || CTRL_IN_2 == ring->id || 
          CTRL_OUT_2 == ring->id || EFS_OUT == ring->id || EFS_IN == ring->id )
      {
         return RING_PRIORITY_CONTROL;
      }
#ifdef SW_LOOPBACK_IS_CRITICAL
      else if( SW_LOOPBACK_IN == ring->id || SW_LOOPBACK_OUT == ring->id)
      {
         return RING_PRIORITY_CONTROL;
      }
#endif
      /* All other rings return 2 */
      else
      {
         return RING_PRIORITY_REST; 
      }

   }
   /* Event rings are not processed in any queue, set a default low value */
   else
   {
      return RING_PRIORITY_MIN; 
   }
}

/** 
 * @brief      mhi_ring_update_wroffset
 *
 *             Update the rings latest write offset from its write pointer retrieved from the
 *             doorbell. Also cache any new ring elements
 *              
 * 
 * @param[in]  *ring - Ring
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             SUCCESS if successful
 *             SYSERR if system error occurred
 *             INVALID_OFFSET if offset is invalid
 *             ERROR if ring cache failed
 *
 * 
 * @sideeffect None 
 */
uint32 mhi_ring_update_wroffset(ring_t *ring)
{
   uint32 wrOffset, ret; /* WR offset in bytes */
   uint64 db=0; 

   mhi_osal_debug(MSG_MEDIUM, "mhi_ring_update_wroffset: Updating ring's WP "); 

   /* Calculate the local ring offsets */
   switch(ring->type)
   {
      case RING_TYPE_CR:
         db=mhi_mmio_get_crdb();
         break;
      case RING_TYPE_ER:
         db=mhi_mmio_get_erdb(ring->id);
         break;
      case RING_TYPE_TR:
         db=mhi_mmio_get_chdb(ring->id);
         break;
      default:
         mhi_osal_debug(MSG_ERR, "mhi_ring_update_wroffset: Invalid ring type specified. Possible corruption. Returning system error"); 
         return SYSERR; 
   }

   wrOffset=((uint32)(db-ring->ringBase))/sizeof(mhi_ring_element_t); 

   /* Sanity check wrOffset */ 
   if(wrOffset > ring->rSize)
   {
      mhi_osal_debug(MSG_ERR, "mhi_ring_update_wroffset: Invalid write pointer offset: %x received", wrOffset); 
      return INVALID_OFFSET; 
   }

   /* Cache ring */
   ret = mhi_ring_cache(ring, wrOffset); 

   ring->wrOffset=wrOffset; 

   return ret; 
}

/** 
 * @brief      mhi_ring_init
 *
 *             Initializes ring. This function should be called once. Should not be called 
 *             across MHI Resets. See mhi_ring_init_ctx 
 *              
 * 
 * @param[in]  id  
 * @param[in]  *ring  
 * @param[in]  type
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
uint32 mhi_ring_init(uint32 id, ring_t *ring, ring_type_t type, rel_process_func func)
{
   ring->id = id; 
   ring->type=type; 
   ring->rel_process=func;

   /* Set ring priority */
   ring->priority=mhi_ring_get_priority(ring); 

   mhi_osal_syncinit(&ring->sync); 

   return SUCCESS; 
}

/** 
 * @brief      mhi_ring_reset
 *
 *             Reset ring data 
 *              
 * 
 * @param[in]  *ring - Ring to reset
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
void mhi_ring_reset(ring_t *ring)
{
   ring->state=RING_UNINIT;

   mhi_osal_memset(&ring->list, 0, sizeof(list_t));
   mhi_osal_memset(&ring->list2, 0, sizeof(list_t));
   mhi_osal_memset(&ring->list3, 0, sizeof(list_t));
}

/** 
 * @brief      mhi_ring_init_ctx
 *
 *             Initializes ring ctx. Done after channel reset for TR's, and after MHI reset for all rings. 
 *             Also validates cached contexts to make sure the host programmed them correctly.  
 *              
 * 
 * @param[in]  *ring - Ring 
 * @param[in]  *ctx  - Pointer to MHI Ring context such as event ring context, channel ring context, command ring context
 *
 * @dependencies
 *              
 * 
 * @return       
 *              MHI_SUCCESS if operation succeeded  
 * 
 * @sideeffect  None 
 */
uint32 mhi_ring_init_ctx(ring_t *ring, mhi_ring_ctx_t *ctx)
{
   uint32 wroffset; 
   mhi_osal_debug(MSG_HIGH, "mhi_ring_init_ctx: id: %d, type: %x, ctx: %x", ring->id, ring->type,ctx);  

   /* Validate the ring context */
   if(SUCCESS != mhi_ring_validate_ctx(ring->type,ctx))
   {
      mhi_osal_debug(MSG_ERR,"mhi_ring_init_ctx: Ring type; %d , Validation failed", ring->type);
      return MHI_ERROR; 
   }

   /* Initialize ring structure */
   ring->ctx=ctx; 
   ring->rSize=ctx->gen.rlen/sizeof(mhi_ring_element_t); 
   ring->ringBase=ctx->gen.rbase;
   ring->rdOffset=(ctx->gen.rp-ring->ringBase)/sizeof(mhi_ring_element_t);
   ring->wrOffset=(ctx->gen.rp-ring->ringBase)/sizeof(mhi_ring_element_t);  
   ring->erNum=ring->type==RING_TYPE_TR?ctx->ch.erindex:PRIMARY_ER; 

   /* Set flag if ring processing is interruptable or not. Command rings are not */
   ring->interruptable=ring->type!=RING_TYPE_CR?TRUE:FALSE;

   mhi_q_init(&ring->linkReqQ); 
   mhi_q_init(&ring->linkReqWaitAckQ); 

   /* Malloc the ring element cache. If its bigger then 1 Mb, or 65K entries then return ERROR */  
   if(ring->rSize > MAX_RING_SIZE)
   {
      mhi_osal_debug(MSG_ERR,"mhi_ring_init_ctx: Ring type; %d , ring size is too big to be cached", ring->type);
      return MHI_ERROR; 
   }

   /* Free up any existing cached ring */ 
   if(NULL != ring->lRingBase)
   {
      mhi_osal_free_aligned(ring->lRingBase); 
   } 

   if(NULL == (ring->lRingBase=mhi_osal_malloc_aligned(ring->rSize*sizeof(mhi_ring_element_t), ALIGN_OFFSET)))
   {
      /* Do a FATAL as reset will not resolve a memory issue */
      mhi_osal_debug(MSG_FATAL,"mhi_ring_init_ctx: Ring type; %d , unable to allocate ring cache", ring->type);
      return MHI_ERROR; 
   }

   /* Initialize the ring with a known pattern */
   mhi_osal_memset(ring->lRingBase,0xAA,ring->rSize*sizeof(mhi_ring_element_t));

   ring->state=RING_IDLE; 

   /* Cache up to WP in context */
   wroffset=(ctx->gen.wp-ctx->gen.rbase)/sizeof(mhi_ring_element_t);
   if(SUCCESS != mhi_ring_cache(ring, wroffset )) 
   {
      mhi_osal_debug(MSG_FATAL,"mhi_ring_init_ctx: Ring type; %d , unable to cache ring elements", ring->type);
      return MHI_ERROR;
   }
   ring->wrOffset=wroffset;

   return SUCCESS; 
}

/** 
 * @brief      mhi_ring_update_ctx
 *
 *             Update rings MHI context with the rings metadata
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
void mhi_ring_update_ctx(ring_t *ring)
{
   ring->ctx->gen.rp=(ring->rdOffset*sizeof(mhi_ring_element_t))+ring->ringBase;
   ring->ctx->gen.wp=(ring->wrOffset*sizeof(mhi_ring_element_t))+ring->ringBase;
}

/** 
 * @brief      mhi_ring_process_rel
 *
 *             Process ring element by invoking the ring's ring element processing function
 *              
 * 
 * @param[in]  *ring - Ring
 * @param[in]  offset - offset into ring at which to process
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             SUCCESS
 *             SYSERR
 *             other return codes returned by ring element processor
 * 
 * @sideeffect None 
 */
uint32 mhi_ring_process_rel(ring_t *ring, uint32 offset )
{
   uint32 ret = SUCCESS;
   mhi_ring_element_t *element; 
   event_t event; 

   mhi_osal_debug(MSG_MEDIUM,"mhi_ring_process_rel: Processing ring element for ring type: %x id: %d offset:%d", ring->type, ring->id, offset); 

   element = &ring->lRingBase[offset];

   /* Set type to reserved as default un-init state */
   event.event_el.tEvent.type=MHI_RING_EL_TYPE_RESERVED;

   /*  Validate ring element. For non transfer rings do a SYSERR. For transfer rings let the TRE routine 
    *  handle it so it can generate an event */
   if( SUCCESS != mhi_ring_validate_rel(ring, element) ) 
   {
      if(RING_TYPE_TR != ring->type)
      {
         return SYSERR; 
      }
   }

   /* Call Ring's Ring element processor */ 
   ret = ring->rel_process(ring, element, &event);

   /* If event was generated by a ring element processor, generate it. */
   if(MHI_RING_EL_TYPE_RESERVED != event.event_el.tEvent.type)
   {
      /* Point the event to host address of ring element. The only events which don't have a pointer field are state 
       * change events and these are not generated as part of ring processing */
      event.event_el.tEvent.ptr=ring->ringBase+offset*sizeof(mhi_ring_element_t);

      if ( SUCCESS != mhi_core_send_event(ring->erNum, &event.event_el, event.trigger_msi) )
      {
         /* If we could not generate an event return SYSERR */
         return SYSERR; 
      }
   }

   return ret;    
}


/* Generic ring processor. Returns 0 if all work on a ring is complete */
/* db is set to indicate db has changed */  
/* cacheOnly indicates to only cache ring */ 


/** 
 * @brief      mhi_ring_process
 *
 *             Generic ring processor. Return SUCCESS if all work is done on a ring. 
 *             It can also update the rings write pointer if db flag is set. The cacheonly flag
 *             is used to only update the ring pointers and not do any processing. At this time 
 *             it is not used 
 *              
 * 
 * @param[in]  *ring      - Ring 
 * @param[in]  db         - Flag indicating if db should be fetched
 * @param[in]  cacheOnly  - Flag indicating if we should only cache the ring and not do processing
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             Return codes returned by underlying processing as well ass 
 *             SYSERR if system error occurred
 *             
 *
 * 
 * @sideeffect None 
 */
uint32 mhi_ring_process(ring_t *ring, uint32 db , uint32 cacheOnly )
{
   int32 ret=SUCCESS; 

   mhi_osal_debug(MSG_MEDIUM,"mhi_ring_process: Processing ring type: %x id: %d", ring->type,ring->id); 

   /* Cache the transfer ring from previous write offset to the doorbell */ 
   if(RING_TYPE_ER != ring->type)
   {
      /* Update the ring write offset */
      if(db)
      {
         mhi_osal_debug(PROFILING_LOG|MSG_HIGH,"mhi_ring_process: ring_cache=start, ring_type=%d, ring_id=%d",ring->type,ring->id);
         ret = mhi_ring_update_wroffset(ring);
         mhi_osal_debug(PROFILING_LOG|MSG_HIGH,"mhi_ring_process: ring_cache=end, ring_type=%d, ring_id=%d",ring->type,ring->id);
   
         /* For anything else than a successful update of ring write offset generate system error */
         if(SUCCESS != ret)
         {
            return SYSERR; 
         }
      }

      if(cacheOnly) 
      {
         /* Return error code used specifically for transfer rings when external client requests 
          * are required to proceed, indicating that there is no work to be done on the ring */
         return REQUESTQ_EMPTY; 
      }

      mhi_osal_debug(PROFILING_LOG|MSG_HIGH,"mhi_ring_process: process_ring=start, ring_type=%d, ring_id=%d",ring->type,ring->id);

      /* Process each unprocessed ring element, until we reach the 
       * write pointer unless there was an error code returned. Reasons could be: 
       * a) ERROR 
       * b) SYSERROR
       * b) For transfer rings, no user buffers in queues 
       * c) Time slicing (not used, possibly in future) 
       */
      while(ring->rdOffset != ring->wrOffset)
      {
         /*  Check for pending control interrupts. If there are any, we should stop ring processing 
          *  immediately as they take precedence, unless the ring is interruptable */ 
         if(TRUE == ring->interruptable)
         {
            if (mhi_mmio_is_mhictrlint_pending())
            {
               mhi_osal_debug(MSG_MEDIUM, "mhi_ring_process: Throttling ring, ctrl interrupts pending");
               ret=RING_THROTTLED;
               break;
            }
         }

         if(SUCCESS != (ret = mhi_ring_process_rel(ring, ring->rdOffset)))
         {
            break; /* if we could not process a ring element break. A doorbell, or client trigger will 
                      cause us to process again */
         }

         /* Update local RP */
         ring->rdOffset++;

         /* Handle wraparound */
         if(ring->rdOffset>=ring->rSize)
            ring->rdOffset=0;
      }
   }

   mhi_osal_debug(PROFILING_LOG|MSG_HIGH,"mhi_ring_process: process_ring=end, ring_type=%d, ring_id=%d",ring->type,ring->id);

   if(ring->rdOffset == ring->wrOffset)
      return SUCCESS; 
   else
      return ret; 

}

/** 
 * @brief      mhi_ring_insert_element
 *
 *             Insert element into ring. Technically this implies we are producing elements 
 *             however, host is the producer of all ring elements, this function 
 *             just populates those free elements).
 *              
 * 
 * @param[in]  *ring  - Ring to insert into
 * @param[in]  *el    - Element to insert
 * @param[in]  update_offset - Whether to update the ring write offset or not
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             SYSERR - system error 
 *             RING_FULL - Ring is full from producer perspective ( semantically its empty)  
 * 
 * @sideeffect None 
 */
uint32 mhi_ring_insert_element(ring_t *ring, mhi_ring_element_t *el, boolean update_offset)
{
   /* If update_offset is true update the offset and check for errors */
   if (TRUE == update_offset && SUCCESS != mhi_ring_update_wroffset(ring))
   {
      return SYSERR; 
   }

   /* If there is no room on ring bail out */ 
   if(ring->rdOffset == ring->wrOffset)
   {
      return RING_FULL;  
   }

   /* Generate element */ 
   *(ring->lRingBase+ring->rdOffset)=*el;

   /* Increment read offset */
   ring->rdOffset=++ring->rdOffset%ring->rSize; 

   return SUCCESS;
}


