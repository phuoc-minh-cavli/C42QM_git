/**
  @file mhi_sio_ser.c
  @brief
  This file contains the implementation of the MHI SIO serial device adaptation  
    

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

#include "mhi.h"
#include "mhi_sio_i.h" 
#include "mhi_ctrl.h" 
#include "mhi_osal.h" 
#include "queue.h"
#include "mhi_sio_util.h"
#ifdef MHI_SIO_USES_PORTMAPPER
#include "ds_port_mapper.h" 
#endif
#include "DDIPlatformInfo.h"
 

/************ Local constants **************************/

/* Max amount of ports we support */
#define MHI_SIO_MAX_PORTS 25 

/* DSM large item size of 1k */
#define DSM_LARGE_ITEM_SIZE 0x400

/* DSM large item size of 2k */
#define DSM_DIAG_ITEM_SIZE 0x800

/* Worker priority for SIO should be below MHI Worker thread priority */
#define MHI_SIO_WORKER_PRIORITY 199

/* Notification Flag bits */  
#define MHI_SIO_SIG_DSM_FLOW 0
#define MHI_SIO_SIG_RX_DSM_Q 1
#define MHI_SIO_SIG_DPC_Q    2

/* MAX DSM chain size we will support */
#define MHI_SIO_MAX_CHAIN_SIZE 40

/* DSM Chain size to alloate */
#define MHI_SIO_RX_CHAIN_SIZE 5

/* Threshold under which we will free a large DSM item and replace with smaller one. 
 * Note: This is what hsic uses, need to evaluate if this can be bumped up higher */
#define MHI_SIO_USE_SMALL_DSM_ITEMS_TRESHOLD 10

/* Macro to translate MINOR number to index. They are 1 to 1 */ 
#define MHI_SIO_PORT_INDEX(port) ((port)&0xFF)

/* Macro to extract minor */ 
#define MHI_SIO_MINOR(port) MHI_SIO_PORT_INDEX(port)

/* Define RESERVED minor number */
#define RESERVED 0xFF

/* Check if the port corresponds to a loopback channel */
#ifndef MHI_MULTI_LOOBPACK
#define IS_LOOPBACK_CHANNEL(port_id) \
                                (DEVICE_ID(SIO_MAJOR_MHI,20) == (port_id))
#else
#define IS_LOOPBACK_CHANNEL(port_id) \
                                ((DEVICE_ID(SIO_MAJOR_MHI,20) == (port_id)) || \
                                 (DEVICE_ID(SIO_MAJOR_MHI,21) == (port_id)) || \
                                 (DEVICE_ID(SIO_MAJOR_MHI,22) == (port_id)) || \
                                 (DEVICE_ID(SIO_MAJOR_MHI,23) == (port_id)) || \
                                 (DEVICE_ID(SIO_MAJOR_MHI,24) == (port_id)))
#endif

/* Internal pool id's for DSM items MHI SIO supports */
#define MHI_SIO_DSM_RX_POOL_DEFAULT 0
#define MHI_SIO_DSM_RX_POOL_DIAG    1

/* Number of DSM pools */
#define MHI_SIO_DSM_RX_POOL_NUM 2

/* Return Constants */
#define MHI_SIO_SUCCESS 0 
#define MHI_SIO_ERROR   1
#define MHI_SIO_TX_FULL 2
#define MHI_SIO_TX_PARTIAL 3

/* Deferred procedure call object defines a DPC which can be inserted into a queue */
typedef struct _mhi_sio_dpc
{
   q_link_type link;      /**< Link type used to queue DPCs */ 
   void (*func)(void*);   /**< DPC function to call */ 
   void*           arg;   /**< Void args to supply to func */ 
}mhi_sio_dpc_t;


/* SIO port Configuration defines PORT -> MHI Channel, rx buffer sizes etc. 
 * For now support only a pair of channels, 
 * for each port. Channel out will always be followed by channel in */  
typedef struct _mhi_sio_port_config
{
   uint32 minor;   /**< Minor PORT Number */ 
   uint32 chOut;   /**< OUT direction - host to device */  
   uint32 rx_size; /**< Size of rx buffers in bytes assigned to each pool */ 
}mhi_sio_port_config_t;


/* MHI SIO Port Information */
typedef struct _mhi_sio_port
{
   /* SIO Specific configuration for port  */
   sio_stream_id_type    stream_id;          /**< Stream ID Type. */
   sio_stream_type       stream_type;        /**< Type of stream being opened.
                                               Filled in by SIO for 
                                               internal use only.           */
   sio_stream_mode_type  stream_mode;        /**< Stream Mode Type.            */
   dsm_watermark_type   *rx_queue;           /**< Received Data Queue.         */
   dsm_watermark_type   *tx_queue;           /**< Transmit Data Queue.         */
   sio_port_id_type      port_id;            /**< Port which needs to be used. */

   sio_rx_func_ptr_type  rx_func_ptr;        /**< If non-NULL, this function 
                                               will be called for each. 
                                               packet which is received.    */
   sio_flow_ctl_type     tx_flow;            /**< TX flow control method.      */
   sio_flow_ctl_type     rx_flow;            /**< RX flow control method.      */

   dsm_mempool_id_enum_type pool_id;         /**< Pool id for RX dsm items */

   /* Implementation specific parameters  */
   mhi_osal_sync_type    sync;               /**< Sync structure to protect port context */ 
   mhi_sio_port_config_t *config;            /**< Port confiuration pointer */ 
   uint8                 open;               /**< IF port was opened successfully by client */ 
   uint8                 flow_enabled;       /**< Specifies if flow is enabled for a given port */ 
   uint32                rx_size;            /**< Specifies max size of rx buffers at given point */ 
   dsm_item_type         *rx_chain_pending;  /**< Points to the pending rx chain (buffers submitted) */
   dsm_item_type         *rx_chain_free;     /**< Points to the free rx chain, yet to be submitted */
   dsm_item_type         *rx_incomplete;     /**< Incomplete packet received from MHI Core which is waiting on EOT */
   dsm_item_type         *tx_chain_remain;   /**< TX chain remainder to be submitted for the last incomplete packet */  
   dsm_item_type         *tx_incomplete;     /**< Pointer to pending incomplete DSM item submitted to MHI Core. The tx_chain_remainder pointer points to the 
                                               remainder that has yet to be submitted  */ 

   uint32                tx_submitted;       /**< Number of succesfully submitted TX packets awaiting completion */ 

   boolean               flush_pending;      /**< Specifies if Port has a pending flush */ 
   boolean               close_pending;      /**< Specifies if Port has a pending close (cb specified) */ 
   sio_vv_func_ptr_type  flush_cb;           /**< Function to call when flush is completed */  

   sio_vv_func_ptr_type  close_cb;           /**< Close callback to be called when port is closed */  

   uint64                rx_count;           /**< RX packet counter */
   uint64                tx_count;           /**< TX packet counter */ 

   /* Line state notification */ 
   sio_ioctl_enable_dtr_event_ext_type dtr_event_cb; /**< CB type specified by client for DTR notification */ 

   mhi_ctrl_serial_state_t serial_state;     /**< Serial state */ 
   mhi_ctrl_line_state_t   line_state;       /**< Line state reflecting what the clients are seeing */ 
}mhi_sio_port_t;

/* MHI Serial Context */ 
typedef struct _mhi_ser_ctx
{
   mhi_osal_sync_type    sync;                       /**< Global sync for serial context */ 
   mhi_sio_port_t ports[MHI_SIO_MAX_PORTS];          /**< Array of port info, indexed by minor number */     
   mhi_osal_thread_t worker_thread;                  /**< Worker thread for sio */ 

   mhi_osal_event_handle hNotify;                    /**< Notification event */ 
   uint32                notifySig;                  /**< Notify signals */ 
   mhi_sio_dsm_pool_t pool[MHI_SIO_DSM_RX_POOL_NUM]; /**< DSM shared pools for rx buffers */ 

   /* DPC QUeue */ 
   q_type dpc_queue;                                 /**< DPC queue */

   /* Queue for DSM RX packets received to be forwarded to clients */
   q_type dsm_ser_rx_queue;
   mhi_status_t mhi_enumerated;
   boolean isFusion; 

   mhi_ctrl_ctx_t ctrl_ctx;                          /**< Control context */  

}mhi_ser_ctx_t;


/* Static variables */
mhi_sio_port_config_t portConfig[]={ 
   /* 0*/   {SIO_MINOR_MHI_DIAG     , 4, DSM_DIAG_ITEM_SIZE*MHI_SIO_RX_CHAIN_SIZE*2 },
   /* 1*/	{SIO_MINOR_MHI_SAHARA   , 2, DSM_LARGE_ITEM_SIZE*MHI_SIO_RX_CHAIN_SIZE},
   /* 2*/	{SIO_MINOR_MHI_SSR      , 6, DSM_LARGE_ITEM_SIZE*MHI_SIO_RX_CHAIN_SIZE},
   /* 3*/	{SIO_MINOR_MHI_EFS      ,10, DSM_LARGE_ITEM_SIZE*MHI_SIO_RX_CHAIN_SIZE},
   /* 4*/	{SIO_MINOR_MHI_QMI      ,14, DSM_LARGE_ITEM_SIZE*MHI_SIO_RX_CHAIN_SIZE*2},
   /* 5*/	{SIO_MINOR_MHI_MBIM     ,12, DSM_LARGE_ITEM_SIZE*MHI_SIO_RX_CHAIN_SIZE},
   /* 6*/	{SIO_MINOR_MHI_IPSW_0   ,34, DSM_LARGE_ITEM_SIZE*MHI_SIO_RX_CHAIN_SIZE},
   /* 7*/	{SIO_MINOR_MHI_IPSW_1   ,36, DSM_LARGE_ITEM_SIZE*MHI_SIO_RX_CHAIN_SIZE},
   /* 8*/	{SIO_MINOR_MHI_IPSW_2   ,38, DSM_LARGE_ITEM_SIZE*MHI_SIO_RX_CHAIN_SIZE},
   /* 9*/	{SIO_MINOR_MHI_IPSW_3   ,40, DSM_LARGE_ITEM_SIZE*MHI_SIO_RX_CHAIN_SIZE},
   /*10*/	{SIO_MINOR_MHI_QDSS     , 8, DSM_LARGE_ITEM_SIZE*MHI_SIO_RX_CHAIN_SIZE},
   /*11*/	{SIO_MINOR_MHI_SMCT     ,44, DSM_LARGE_ITEM_SIZE*MHI_SIO_RX_CHAIN_SIZE},
   /*12*/	{SIO_MINOR_MHI_CSVT     ,42, DSM_LARGE_ITEM_SIZE*MHI_SIO_RX_CHAIN_SIZE},
   /*13*/	{SIO_MINOR_MHI_DUN      ,32, DSM_LARGE_ITEM_SIZE*MHI_SIO_RX_CHAIN_SIZE},
   /*14*/	{SIO_MINOR_MHI_QMI_2    ,16, DSM_LARGE_ITEM_SIZE*MHI_SIO_RX_CHAIN_SIZE*2},
   /*15*/	{SIO_MINOR_MHI_DCI      ,20, DSM_LARGE_ITEM_SIZE*MHI_SIO_RX_CHAIN_SIZE*2},
   /*16*/	{RESERVED               , 0, 0                     },
   /*17*/	{RESERVED               , 0, 0                     },
   /*18*/	{RESERVED               , 0, 0                     },
   /*19*/	{RESERVED               , 0, 0                     },
   /*20*/	{20                     , 0, DSM_LARGE_ITEM_SIZE*MHI_SIO_RX_CHAIN_SIZE},
   /*21*/	{21                     ,60, DSM_LARGE_ITEM_SIZE*MHI_SIO_RX_CHAIN_SIZE},
   /*22*/	{22                     ,62, DSM_LARGE_ITEM_SIZE*MHI_SIO_RX_CHAIN_SIZE},
   /*23*/	{23                     ,64, DSM_LARGE_ITEM_SIZE*MHI_SIO_RX_CHAIN_SIZE},
   /*24*/	{24                     ,66, DSM_LARGE_ITEM_SIZE*MHI_SIO_RX_CHAIN_SIZE}

}; 

/* Static Context for Serial interface */
mhi_ser_ctx_t mhi_ser_ctx; 

/**=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*= 
 **************** Internal Functions *************
 *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*= */

/******************************** Prototypes****************** */ 
void mhi_sio_seri_thread_entry(void *ctx);
uint32 mhi_sio_seri_submit_tx(mhi_sio_port_t *port, dsm_item_type *tx_ptr, boolean new_packet ); 
void mhi_sio_seri_submit_rx(mhi_ser_ctx_t *ctx, mhi_sio_port_t *port);
void mhi_sio_seri_drain_tx(mhi_sio_port_t *port);
void mhi_sio_seri_close_after_flush_cb(mhi_sio_port_t *port);
void mhi_sio_seri_flush_tx(mhi_sio_port_t *port, sio_vv_func_ptr_type flush_tx_done_cb);
sio_status_type mhi_sio_seri_extract_port(sio_port_id_type port, uint32 *minor);
void mhi_sio_seri_close_dpc(void *arg);
void mhi_sio_seri_close(mhi_sio_port_t *port, sio_vv_func_ptr_type close_done_cb);

/******************************** Implementation *************************************************************/

/** 
 * @brief      mhi_sio_seri_close_dpc
 *
 *             Called to close an sio port. Cleans up any DSM items, and marks the port as closed. Also 
 *             closes the channels with MHI Core 
 *              
 * 
 * @param[in]  *arg - Void arguments 
 *
 * @dependencies
 *             Must be called as DPC in different context than the MHI Core thread context. If it is called in same 
 *             we cannot clean up a channel properly while servicing its cb's. 
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_sio_seri_close_dpc(void *arg)
{
   mhi_sio_port_t *port = (mhi_sio_port_t*)arg; 
   sio_vv_func_ptr_type close_cb;  
   uint32 index; 

   /* Validate port and extract index */
   if (NULL == port)
   {
      mhi_osal_debug(MSG_FATAL, "mhi_sio_seri_close: port is NULL");
      return;
   }
   
   mhi_osal_debug(MSG_HIGH, "mhi_sio_seri_close_dpc: portid=%d, Enter",port->port_id);

   mhi_osal_syncenter(&mhi_ser_ctx.sync);

   mhi_sio_seri_extract_port(port->port_id, &index); 

   /* Close channels */
   mhi_channel_close(port->config->chOut); 
   mhi_channel_close(port->config->chOut+1); 

   port->open=FALSE;    
   close_cb = port->close_cb;
   
   /* Free the pending buffers on the RX path */ 
   dsm_free_packet(&port->rx_chain_pending); 
   dsm_free_packet(&port->rx_chain_free); 

   /* Destroy the sync object and cleanup port */
   mhi_osal_syncdestroy(&port->sync); 
   mhi_osal_syncleave(&mhi_ser_ctx.sync); 

   port->close_cb=NULL;

   /* Issue CB if required */
   if(NULL != close_cb)
      close_cb();

   mhi_osal_debug(MSG_HIGH, "mhi_sio_seri_close_dpc: portid=%d, Exit",port->port_id);
}

/** 
 * @brief      mhi_sio_seri_close
 *
 *             #desc 
 *              
 * 
 * @param[in]  *port - Port to close
 * @param[in]  close_done_cb - CB to be called when close is completed
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
void mhi_sio_seri_close(mhi_sio_port_t *port, sio_vv_func_ptr_type close_done_cb)
{
   dsm_item_type *dsm; 

   mhi_osal_debug(MSG_HIGH, "mhi_sio_seri_close: Close called for port_id: %d", port->port_id);

   mhi_osal_syncenter(&mhi_ser_ctx.sync); 

   /* Validate port is not already closed  */
   if(FALSE == port->open)
   {
      mhi_osal_debug(MSG_ERR, "mhi_sio_ser_close: Client attempted to close a stream that was not opened");
      mhi_osal_syncleave(&mhi_ser_ctx.sync); 
      return; 
   }

   /* If no CB immediately close and clean up */
   if(NULL == close_done_cb)
   {
      mhi_channel_close(port->config->chOut); 

      /* Abort the write operations before closing channel so we can clean up 
       * the TX DSM items in the callback */
      mhi_channel_abort(port->config->chOut+1);
      mhi_channel_close(port->config->chOut+1); 
   }
   else
   {
      /* Issue a flush and supply local cb which will close the port */
      port->close_cb = close_done_cb;
      port->close_pending=TRUE;

      mhi_sio_seri_flush_tx(port, (sio_vv_func_ptr_type)mhi_sio_seri_close_after_flush_cb);
      mhi_osal_syncleave(&mhi_ser_ctx.sync); 
      return; 
   }

   port->open=FALSE;    

   /* Free the un-submitted buffers of incomplete chain if any */ 
   dsm_free_packet(&port->tx_incomplete); 

   /* Clean up anything sitting on WMQ */
   while(!dsm_is_wm_empty(port->tx_queue))
   {
      dsm=dsm_dequeue(port->tx_queue);
      dsm_free_packet(&dsm);
   }

   /* Free the pending buffers on the RX path */ 
   dsm_free_packet(&port->rx_chain_pending); 
   dsm_free_packet(&port->rx_chain_free); 

   /* Destroy the sync object and cleanup port */
   mhi_osal_syncdestroy(&port->sync); 
   mhi_osal_syncleave(&mhi_ser_ctx.sync); 

   return; 
}

/** 
 * @brief      mhi_sio_seri_close_after_flush_cb
 *
 *             Called after a tx flush operation is completed during a sio_close operation. This is called 
 *             in TX write callback context. This function will fire a DPC via SIO thread which will do the actual 
 *             closing of the sio port.  
 *              
 * 
 * @param[in]  *port - Port to close
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
void mhi_sio_seri_close_after_flush_cb(mhi_sio_port_t *port)
{
   mhi_sio_dpc_t *dpc; 

   /* Queue a DPC to SIO thread to handle a close as we cant close a channel from MHI CORE worker thread context, which 
    * this function will be running from */
   dpc=mhi_osal_malloc(sizeof(*dpc));
 
   if(NULL == dpc) 
     mhi_osal_debug(MSG_FATAL, "mhi_sio_seri_close_after_flush_cb: Failed to alloc a dpc");  

   dpc->func=mhi_sio_seri_close_dpc;
   dpc->arg=(void*)port; 

   q_link(dpc, &dpc->link);
   q_put(&mhi_ser_ctx.dpc_queue, &dpc->link);

   mhi_osal_atomic_set_bit(&mhi_ser_ctx.notifySig, MHI_SIO_SIG_DPC_Q); 

   /* Signal SIO thread */ 
   mhi_osal_eventsignal(mhi_ser_ctx.hNotify);
}

/** 
 * @brief      mhi_sio_seri_forward_rx_completion
 *
 *             Forward RX completions to SIO thread
 *              
 * 
 * @param[in]  *dsm_item
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
void mhi_sio_seri_forward_rx_completion(dsm_item_type *dsm_item)
{
   dsm_item_type *dsm_chain=dsm_item; 

   /* Optimize DSM chain 
    * Note: This can also be done in the MHI SIO thread instead of here. Since threshold is small it's 
    * fine to do here as we will free the large DSM item sooner */
   mhi_sio_dsm_optimize_dsm_chain(&dsm_chain, MHI_SIO_USE_SMALL_DSM_ITEMS_TRESHOLD);

   /* Queue RX DSM item to worker thread */
   q_link(dsm_chain, &dsm_chain->link);
   q_put(&mhi_ser_ctx.dsm_ser_rx_queue, &dsm_chain->link);

   mhi_osal_atomic_set_bit(&mhi_ser_ctx.notifySig, MHI_SIO_SIG_RX_DSM_Q); 
   mhi_osal_eventsignal(mhi_ser_ctx.hNotify); 
}

/** 
 * @brief      mhi_sio_seri_extract_port
 *
 *             Extract minor port number 
 *              
 * 
 * @param[in]  port   - SIO Port
 * @param[out] *minor - Receives the minor number
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             SIO_DONE_S if successful, 
 *             SIO_BADP_S otherwise
 * 
 * @sideeffect None 
 */
sio_status_type mhi_sio_seri_extract_port(sio_port_id_type port, uint32 *minor)
{
   /* get index for local port info based on minor number */
   if( MHI_SIO_PORT_INDEX(port)> MHI_SIO_MAX_PORTS)
      return SIO_BADP_S; 

   *minor=MHI_SIO_MINOR(port); 

   /* Verify major number */	
   if(SIO_MAJOR_MHI != port>>8)
      return SIO_BADP_S; 	

   return SIO_DONE_S;
}

/** 
 * @brief      mhi_sio_seri_forward_rx_buffer
 *
 *             Forward RX buffer to client. Called by SIO thread
 *              
 * 
 * @param[in]  *dsm_item - DSM item to forward
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
void mhi_sio_seri_forward_rx_buffer(dsm_item_type *dsm_item)
{
   mhi_sio_port_t *port = (mhi_sio_port_t*)dsm_item->app_ptr; 

   /* clear the app_ptr */
   dsm_item->app_ptr=NULL; 

   if(NULL == port) 
   {
      mhi_osal_debug(MSG_FATAL, "mhi_sio_seri_forward_rx_buffer: app_ptr is NULL"); 
   }

   /* Either enqueue the DSM item to clients tx queue or call the cb */
   if(port->open)
   {
      if(NULL != port->rx_func_ptr)
      {
         port->rx_func_ptr(&dsm_item);
      }
      else
      {
         dsm_enqueue(port->rx_queue, &dsm_item);
      }
   }
   /* If port is no longer open free the buffer */
   else
   {
      mhi_osal_debug(MSG_ERR, "mhi_sio_seri_forward_rx_buffer: Received DSM item on closed port");
      dsm_free_packet(&dsm_item); 
   }
}

/** 
 * @brief      mhi_sio_seri_write_completion_cb
 *
 *             Write completion callback. Called by MHI Core driver when write is completed.  
 *              
 * 
 * @param[in]  status - status of operation
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
void mhi_sio_seri_write_completion_cb(mhi_return_t status)
{
   mhi_sio_port_t *port;
   dsm_item_type *dsm_buff; 
   sio_vv_func_ptr_type flush_cb; 

   /* Get head DSM buffer for transaction and determine the port */
   dsm_buff=(dsm_item_type*)status.userData; 
   port=(mhi_sio_port_t*)dsm_buff->app_ptr;

   if (NULL == port)
   {
      mhi_osal_debug(MSG_FATAL, "mhi_sio_seri_write_completion_cb: port is NULL, DSM head=0x%x",dsm_buff); 
      return; 
   }

   mhi_osal_debug(PROFILING_LOG|MSG_HIGH,"mhi_sio_seri_write_completion_cb: port_id=%d, ch_id=%d, DSM head=0x%x",
                                           port->port_id,port->config->chOut+1,dsm_buff);


   /* Clear the app_ptr so that its not used by mistake later on */
   dsm_buff->app_ptr=NULL; 

   /* Write operation should succeed. If it failed log an error, and drop the packet unless it's a control packet. 
    * Note: MHI Core: if it fails in the middle of a multi buffer write, it should drop all cb's for that particular chain of writes. 
    * Currently it does not fail any writes, upon later implementation need to ensure it will
    * do only ONE success or failure callback for a multi buffer write */
   if(MHI_SUCCESS != status.status) 
   {
      mhi_osal_debug(MSG_ERR, "mhi_sio_seri_write_completion_cb: Write failure, status: %d", status.status); 
   }

   /* Free the buffer */
   dsm_free_packet(&dsm_buff);
   mhi_osal_atomic_dec(&port->tx_submitted);   

   /* If there are no submitted dsm items to MHI core, check if a tx flush is pending. Grab the sync to eliminate race between
    * a flush command and this completion. Note after a flush command there are no more writes issued by the client 
    * until a write is completed */
   if(NULL == port->tx_chain_remain && TRUE == dsm_is_wm_empty(port->tx_queue) && 
      0 == port->tx_submitted )
   {
      mhi_osal_syncenter(&port->sync);
      if( TRUE == port->flush_pending )
      {
        port->flush_pending = FALSE; 

        /* Clear the flag to avoid race where close happens as we are handling the last write completion */
        if( TRUE == port->close_pending)
        {
           port->close_pending=FALSE;
        }

        flush_cb = port->flush_cb; 
        mhi_osal_syncleave(&port->sync);
        
        if( NULL != flush_cb) 
        {
           mhi_osal_debug(MSG_LOW, "mhi_sio_seri_write_completion_cb: Invoke flush cb for port_id=%d",port->port_id);
           if ((void (*)(mhi_sio_port_t*))flush_cb != mhi_sio_seri_close_after_flush_cb)
           {
              flush_cb();

              /* Check for a pending close cb. It is possible user called close (with cb) while a flush is pending. In this case issue both CB's */
              if( NULL != port->close_cb )
              {
                 mhi_sio_seri_close_after_flush_cb(port);
              }
           }
           else
           {
              mhi_sio_seri_close_after_flush_cb(port);
           }
        }
      }
      else
      {
         mhi_osal_syncleave(&port->sync);
      }
   }

   /* Attempt to Drain Pending chain and TX WM-q if there are any items */
   if(NULL != port->tx_chain_remain || FALSE == dsm_is_wm_empty(port->tx_queue))
   {
      mhi_sio_seri_drain_tx(port); 
   }
}

/** 
 * @brief      mhi_sio_seri_drain_tx
 *
 *             Drains any TX chain remainders (not submitted) and any queued DSM items in the TX WM queue 
 *             by submitting them to MHI Core. 
 *              
 * 
 * @param[in]  *port - Port to drain
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
void mhi_sio_seri_drain_tx(mhi_sio_port_t *port)
{
   uint32 ret; 
   mhi_osal_syncenter(&port->sync);

   /* Transmit any remainder chain */
   if(NULL != port->tx_chain_remain)
   {
      if (MHI_SIO_SUCCESS != mhi_sio_seri_submit_tx(port, port->tx_chain_remain, FALSE))
      {
         mhi_osal_debug(MSG_HIGH, "mhi_sio_seri_drain_tx: Remainder chain submit_tx hit full condition, port id: %d", port->port_id);
         mhi_osal_syncleave(&port->sync);
         return; 
      }
   }

   /* Transmit items off the wm_queue. If we hit full condition stop */
   while( FALSE == dsm_is_wm_empty(port->tx_queue))
   {
      if (MHI_SIO_SUCCESS != (ret = mhi_sio_seri_submit_tx(port, mhi_sio_peek_wmq(port->tx_queue), FALSE)))
      {
         /* If no buffers were submitted break and well try later. If on other hand some buffers got submitted 
          * remove from WMQ as it is now tracked by tx_incomplete and also break. If succesfuly remove it 
          * from WMQ and continue draining */
         if( MHI_SIO_TX_FULL == ret)
         {
            mhi_osal_debug(MSG_HIGH, "mhi_sio_seri_drain_tx: WM item, Submit_tx hit full condition, port id: %d", port->port_id);
         }
         else if( MHI_SIO_TX_PARTIAL == ret)
         {
            mhi_osal_debug(MSG_HIGH, "mhi_sio_seri_drain_tx: Submit_tx partially submitted buffers , port id: %d", port->port_id);
            dsm_dequeue(port->tx_queue);
         }
         break;
      }

      dsm_dequeue(port->tx_queue);
   }

   mhi_osal_syncleave(&port->sync);
}

/** 
 * @brief      mhi_sio_seri_submit_tx
 *             Submit a new or queued packet. Called when new packet is received from upper layers or to 
 *             submit any remainder or queued TX packet 
 *              
 * 
 * @param[in]  *port  - SIO port
 * @param[in]  *tx_ptr - TX packet pointer
 * @param[in]  new_packet - TRUE if dealing with a new packet
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             MHI_SIO_SUCCESS if successful 
 *             MHI_SIO_TX_FULL if full condition was reached in MHI CORE 
 *             MHI_SIO_TX_PARTIAL if full condition was reached in MHI CORE but some buffers were accepted
 * 
 * @sideeffect None 
 */
uint32 mhi_sio_seri_submit_tx(mhi_sio_port_t *port, dsm_item_type *tx_ptr, boolean new_packet )
{
   dsm_item_type *dsm_temp;
   mhi_cb_t cb; 
   uint32 i, nSubmitted, ret=MHI_SIO_SUCCESS; 
   uint8 remainder; 
   mhi_status_t status; 

   /* Used to describe the buffers in the chain. Chain should not be bigger than 20 buffers, if it is 
    * we should dynamically malloc the data/size arrays instead of statically defining them */
   void* data_array[MHI_SIO_MAX_CHAIN_SIZE];
   uint32 size_array[MHI_SIO_MAX_CHAIN_SIZE];
   
   if(NULL == tx_ptr)
   {
      mhi_osal_debug(MSG_FATAL, "mhi_sio_seri_submit_tx: tx_ptr is NULL");
   }

   mhi_osal_syncenter(&port->sync);

   if( tx_ptr == port->tx_chain_remain) 
      remainder=TRUE; 
   else
      remainder=FALSE;

   /* Check if there is a remainder chain pending or there are items on the WMQ and we are dealing with a new packet
    * If so queue the new packet to clients WMQ */
   if( (NULL != port->tx_chain_remain || FALSE == dsm_is_wm_empty(port->tx_queue)) && TRUE == new_packet)
   {
      dsm_enqueue(port->tx_queue, &tx_ptr); 
      mhi_osal_syncleave(&port->sync);
      return MHI_SIO_SUCCESS; 
   }

   i=0;
   dsm_temp=tx_ptr; 
   mhi_osal_debug(PROFILING_LOG|MSG_HIGH,"mhi_sio_seri_submit_tx: DSM head=0x%x, port_id=%d, ch_id=%d",tx_ptr,port->port_id,port->config->chOut+1);
   do
   {
      /* QDSS and EFS use the app ptr and app_data for ptr and size instead of the DSM items since they wish to use large 
       * buffers for power and performance reasons  */
      if(SIO_MINOR_MHI_EFS == MHI_SIO_MINOR(port->port_id) || SIO_MINOR_MHI_QDSS == MHI_SIO_MINOR(port->port_id)
         || IS_LOOPBACK_CHANNEL(port->port_id))
      {
         if(NULL != dsm_temp->app_ptr && NULL != dsm_temp->app_field)
         {
            data_array[i]=dsm_temp->app_ptr;
            size_array[i]=dsm_temp->app_field;
            i++; 
            continue; 
         }
         else if(!IS_LOOPBACK_CHANNEL(port->port_id))
         {
            mhi_osal_debug(MSG_HIGH, "mhi_sio_seri_submit_tx: app_ptr is not set for QDSS/EFS port:%d.  Ignoring, Defaulting"
                  "to DSM item data_ptr ", port->port_id); 
         }
      }

      /* If we reached this point and app_ptr is set, log it as some client might be trying to use app_ptr as buffer pointer */
      if(NULL != dsm_temp->app_ptr)
      {
         mhi_osal_debug(MSG_HIGH, "mhi_sio_seri_submit_tx: app_ptr is set for port:%d non EFS, or QDSS ports. Ignoring. Defaulting"
               "to DSM item data_ptr ", port->port_id); 
      }

      /* By default use the data_ptr and used fields of DSM item */
      data_array[i]=(void*)(dsm_temp->data_ptr);
      size_array[i]=dsm_temp->used;
      i++;
   }while( (dsm_temp=dsm_temp->pkt_ptr) != NULL && i<MHI_SIO_MAX_CHAIN_SIZE );

   /* Detect case where we run out if items in data_array and there are more chained items. This is a fatal error. See comment above about data_array size */
   if(NULL != dsm_temp)
   {
      mhi_osal_debug(MSG_FATAL, "mhi_sio_seri_submit_tx: Did not reach end of DSM chain"); 
      dsm_free_packet(&tx_ptr); 
   }

   /* Link the user data with HEAD DSM item so that we can free it easily. If we are working on remainder 
    * the HEAD DSM is pointed to by tx_incomplete pointer in port context. If we are working on a fresh 
    * DSM item then use the tx_ptr as the HEAD DSM */ 
   cb.cbFunc=mhi_sio_seri_write_completion_cb; 
   cb.userData=(void*)(remainder==TRUE?port->tx_incomplete:tx_ptr); 

   /* Use the app_ptr of head DSM item and link it to the port. This is used later when processing tx completion callbacks */
   ((dsm_item_type*)(cb.userData))->app_ptr=(void*)port; 

   status = mhi_channel_write_multiple(port->config->chOut+1, data_array, size_array, i, &nSubmitted, &cb); 

   switch(status)
   {
      case MHI_PENDING:
         /* All buffers were accepted. If we were working on the chain remainder clear the chain remainder pointer and return */
         if(remainder) 
         {
            port->tx_chain_remain = NULL; 
            port->tx_incomplete = NULL;
         }
         mhi_osal_atomic_inc(&port->tx_submitted);

         break; 
      case MHI_OP_FULL:

	     mhi_osal_debug(MSG_MEDIUM,"mhi_sio_seri_submit_tx: MHI_OP_FULL on port_id=%d, nSubmitted=%d nElements=%d",port->port_id,nSubmitted,i);
         if(0 != nSubmitted)
         {
            /** Not all buffers were accepted. Set the chain remainder pointer and the incomplete packet pointers, by traversing 
             *  submitted items to locate the remainder of chain   */ 
            for(i = 0, dsm_temp=tx_ptr; i < nSubmitted && dsm_temp != NULL; i++,dsm_temp=dsm_temp->pkt_ptr);

            if(i != nSubmitted) 
            {
               mhi_osal_debug(MSG_FATAL, "mhi_sio_seri_submit_tx: Failed to reach nSubmitted:%d in packet chain. Fatal error", nSubmitted); 
            }
            else if(NULL == dsm_temp)
            {
               mhi_osal_debug(MSG_FATAL, "mhi_sio_seri_submit_tx: Reached nSubmitted:%d in packet chain, dsm_item is NULL. Fatal error", nSubmitted); 
            }

            port->tx_chain_remain=dsm_temp; 

            /* If dealing with non-remainder set the incomplete packet pointer. */
            if(!remainder)
            {
               port->tx_incomplete=tx_ptr; 
            }

            ret = MHI_SIO_TX_PARTIAL;
         }
         else
         {
            /* if no buffers were submitted queue the packet to clients WMQ if we are dealing with a new packet */
            if(!remainder && new_packet)
            {
               dsm_enqueue(port->tx_queue, &tx_ptr);
            }
            ret = MHI_SIO_TX_FULL;
         } 
         break;
   }

   mhi_osal_syncleave(&port->sync);
   return ret; 
}

/** 
 * @brief      mhi_sio_seri_read_completion_cb
 *
 *             Handle RX completions. For EOT, MHI Core will generate an early CB. Need to split the packet and give it 
 *             to the client.  
 *              
 * 
 * @param[in]  status
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
void mhi_sio_seri_read_completion_cb(mhi_return_t status)
{
   mhi_sio_port_t *port=status.userData;
   dsm_item_type *dsm_buff, *dsm_buff_head; 
   uint32 i, bytesProcessed=0; 

   if(NULL == port)
   {
      mhi_osal_debug(MSG_FATAL, "mhi_sio_seri_read_completion_cb: Port is NULL"); 
   }

   mhi_osal_syncenter(&port->sync);

   dsm_buff=port->rx_chain_pending; 
   if(NULL == dsm_buff) 
   {
      mhi_osal_debug(MSG_FATAL, "mhi_sio_seri_read_completion_cb: Pending chain is NULL in rx completion callback "); 
      mhi_osal_syncleave(&port->sync); 
      return; 
   }

   /* Traverse packet chain to determine which packet got processed. For every packet in the chain 
    * set the used field. This will be packets size until we reach the CB buffer. For the CB buffer 
    * we check the actual bytes received in case of EOT */
   i=0; 
   dsm_buff_head=dsm_buff; 

   do
   {
      bytesProcessed+=status.payload.rw.bytesRW; 

      /* If we located DSM buffer set its size to bytes processed and return */
      if(dsm_buff->data_ptr == (uint8*)status.payload.rw.buff)
      {
         dsm_buff->used=status.payload.rw.bytesRW;
         break; 
      }
      else
      {
         dsm_buff->used=dsm_buff->size; 
      }

      dsm_buff=dsm_buff->pkt_ptr;
      i++; 
   }
   while( dsm_buff != NULL && i < MHI_SIO_MAX_CHAIN_SIZE ); 

   if(MHI_SIO_MAX_CHAIN_SIZE == i || NULL == dsm_buff)
   {
      mhi_osal_debug(MSG_FATAL, "mhi_sio_seri_read_completion_cb: could not locate last processed buffer in DSM packet"); 
      mhi_osal_syncleave(&port->sync); 
      return;
   }

   /* Split chain at packet received, and set the used data field to amount reported by MHI completion status */

   port->rx_chain_pending=dsm_buff->pkt_ptr;
   dsm_buff->pkt_ptr=NULL;

   if(status.status != MHI_SUCCESS)
   {
      mhi_osal_debug(MSG_ERR, "mhi_sio_seri_read_completion_cb: Packet was not succesfully processed, dropping packet");
      dsm_free_packet(&dsm_buff_head);
      goto end;
   }

   /* Link the DSM buffer with the port using the app_ptr field. This is how MHI SIO task will know which 
    * DSM item belongs to which port when examining the rx queue */
   dsm_buff_head->app_ptr=(void*)port;

   port->rx_count++;
   mhi_osal_debug(MSG_HIGH, "mhi_sio_seri_read_completion_cb: Port: 0x%x, RX packet #: 0x%x%x, size: 0x%x", port->port_id,
         QW2DW(port->rx_count), bytesProcessed);

   dsm_append(&port->rx_incomplete, &dsm_buff_head);

   /* If EOT is not set queue the packet to the rx_incomplete chain */
   if(!status.payload.rw.EOT)
   {
      mhi_osal_debug(MSG_HIGH, "mhi_sio_seri_read_completion_cb: Port: 0x%x, Received incomplete packet queing to rx_incomplete chain", port->port_id);
   }
   else
   {
      /* If EOT is reached forward the rx_incomplete dsm item and clear if it exists */ 
      if(port->rx_incomplete)
      {
         dsm_buff_head=port->rx_incomplete; 
         port->rx_incomplete=NULL;
      }

      /* Forward the received packet chain to user. This just enqueues the chain to the SIO thread */
      mhi_sio_seri_forward_rx_completion(dsm_buff_head); 
   }

end:
   mhi_osal_syncleave(&port->sync);

   /* If pending pointer is NULL submit new packet chain */
   if(NULL == port->rx_chain_pending)
   {
      mhi_sio_seri_submit_rx(&mhi_ser_ctx, port); 
   }
}

/** 
 * @brief      mhi_sio_seri_submit_rx
 *
 *             Submits RX buffers to MHI core 
 *              
 * 
 * @param[in]  *ctx - Pointer to serial context
 * @param[in]  *port - Pointer to sio port context
 *
 * @dependencies
 *              
 * 
 * @return       
 *              
 * 
 * @sideeffect  None 
 */
void mhi_sio_seri_submit_rx(mhi_ser_ctx_t *ctx, mhi_sio_port_t *port)
{
   dsm_item_type *dsm_buff, *dsm_new, *dsm_temp;
   mhi_status_t status; 
   uint32 i, nSubmitted; 
   mhi_cb_t cb; 

   /* Used to describe the buffers in the chain. Chain should not be bigger than 20 buffers, if it is 
    * we should dynamically malloc
    * the data/size arrays instead of statically defining them */
   void* data_array[MHI_SIO_MAX_CHAIN_SIZE];
   uint32 size_array[MHI_SIO_MAX_CHAIN_SIZE];

   mhi_osal_syncenter(&port->sync);  

   /* Check to see if buffers were already submitted */ 
   if(NULL != port->rx_chain_pending)
   {
      /* If we have pending items, submit the remainder if there is any which is pointed to by rx_chain_free */
      dsm_buff=port->rx_chain_free;
   }
   else
   {
      /* No pending buffers so allocate a new chain */
      dsm_buff = mhi_sio_dsm_generate_dsm_chain(port->pool_id,port->rx_size);
   }

   /* If could not allocate a buffer return */
   if(NULL == dsm_buff)
   {
      mhi_osal_syncleave(&port->sync); 
      mhi_osal_debug(MSG_HIGH, "mhi_sio_seri_submit_rx: Could not allocate DSM items for port: %x", port->port_id); 
      return; 
   }

   /* Iterate over chain and prepare transfer descriptor array */ 
   i=0;
   dsm_temp=dsm_buff; 
   do
   {
      data_array[i]=(void*)(dsm_temp->data_ptr);
      size_array[i]=dsm_temp->size;
      i++;
   }while( (dsm_temp=dsm_temp->pkt_ptr) != NULL && i<MHI_SIO_MAX_CHAIN_SIZE );

   /* Detect case where we run out if items in data_array and there is more chained items. This is a fatal error. See comment above about data_array size */
   if(NULL != dsm_temp)
   {
      mhi_osal_debug(MSG_FATAL, "mhi_sio_seri_submit_rx: Did not reach end of DSM chain"); 
   }

   cb.cbFunc=mhi_sio_seri_read_completion_cb; 
   cb.userData=(void*)port;


   /* Validate port */
   if (MHI_SIO_PORT_INDEX(port->port_id)>MHI_SIO_MAX_PORTS)
   {
      mhi_osal_debug(MSG_FATAL, "mhi_sio_seri_submit_rx: port is invalid"); 
   }

   status = mhi_channel_read_multiple(port->config->chOut, data_array, size_array, i, &nSubmitted, &cb); 

   switch(status)
   {
      case MHI_PENDING: 
         /* Request was successfully submitted, return, sanity check nSubmitted */
         if (i != nSubmitted)
         {
            mhi_osal_debug(MSG_FATAL, "mhi_sio_seri_submit_rx: Number of items submitted does not match requested amount");
         }
         /* Set pending and free rx chain buffer pointers  */
         port->rx_chain_free=NULL; 
         break; 
      case MHI_OP_FULL:
         /* if entire DSM chain was not processed, we will re-submit later in read completion callback. Split the chain at last processed buffer. If no buffers 
          * were processed, dsm_new will point to dsm_buff */ 
         mhi_sio_dsm_split_dsm_chain(dsm_buff, nSubmitted, &dsm_new);

         /* Set free rx chain buffer pointer  */
         port->rx_chain_free=dsm_new; 
         break;
   }

   /* If pending chain already exists, append newly submitted buffers to the pending chain */
   if(NULL != port->rx_chain_pending)
   {
      dsm_append(&port->rx_chain_pending, &dsm_buff);
   }
   else
   {
      port->rx_chain_pending=dsm_buff; 
   }
   mhi_osal_syncleave(&port->sync); 
}

/** 
 * @brief      mhi_sio_seri_handle_pool_avail
 *
 *             Handle pool dsm item available notification
 *              
 * 
 * @param[in]  *ctx - MHI Serial context
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
void mhi_sio_seri_handle_pool_avail(mhi_ser_ctx_t *ctx)
{
   uint32 i; 

   for(i=0; i < MHI_SIO_MAX_PORTS; i++)
   {
      if(TRUE == ctx->ports[i].open)
      {
         mhi_sio_seri_submit_rx(ctx, &ctx->ports[i]); 
      }
   }
}

/** 
 * @brief      mhi_sio_seri_handle_signals
 *
 *             Handle Signals set for MHI SIO Thread
 *              
 * 
 * @param[in]  *ctx - MHI Serial context
 *
 * @dependencies
 *              
 * 
 * @return       
 *              MHI_SUCCESS if operation succeeded  
 * 
 * @sideeffect  None 
 */
void mhi_sio_seri_handle_signals(mhi_ser_ctx_t *ctx)
{
   uint32 flags; 
   volatile uint32* sigs = &ctx->notifySig; 

   flags=*sigs;

   if(0 != (flags&(1<<MHI_SIO_SIG_DSM_FLOW)))
   {
      /* Clear bit */
      mhi_osal_atomic_clear_bit(&ctx->notifySig, MHI_SIO_SIG_DSM_FLOW); 

      /* IF flow is enabled submit DSM items to MHI */
      if(TRUE == ctx->pool[MHI_SIO_DSM_RX_POOL_DEFAULT].available || 
            TRUE == ctx->pool[MHI_SIO_DSM_RX_POOL_DIAG].available) 
      {
         mhi_sio_seri_handle_pool_avail(ctx);
      }
   }

   /* If we have RX DSM items to process forward them to clients */
   if(0 != (flags&(1<<MHI_SIO_SIG_RX_DSM_Q)))
   {
      /* Clear bit */
      mhi_osal_atomic_clear_bit(&ctx->notifySig, MHI_SIO_SIG_RX_DSM_Q); 

      /* Forward RX items to clients either via WMQ or cb */
      while(q_cnt(&mhi_ser_ctx.dsm_ser_rx_queue) != 0)
      {
         dsm_item_type *dsm_ptr = (dsm_item_type *)q_get(&mhi_ser_ctx.dsm_ser_rx_queue);

         if(NULL == dsm_ptr)
         {
            mhi_osal_debug(MSG_FATAL, "mhi_sio_seri_handle_signals: Non empty queue returned null item "); 
            return; 

         } 

         mhi_sio_seri_forward_rx_buffer(dsm_ptr); 
      }
   }

   /* If we have DPC's execute them */
   if(0 != (flags&(1<<MHI_SIO_SIG_DPC_Q)))
   {
      /* Clear bit */
      mhi_osal_atomic_clear_bit(&ctx->notifySig, MHI_SIO_SIG_DPC_Q); 

      /* Fire DPC's */
      while(q_cnt(&mhi_ser_ctx.dpc_queue) != 0)
      {
         mhi_sio_dpc_t *dpc = (mhi_sio_dpc_t *)q_get(&mhi_ser_ctx.dpc_queue);

         if(NULL == dpc)
         {
            mhi_osal_debug(MSG_FATAL, "mhi_sio_seri_handle_signals: Non empty dpc queue returned null item "); 
            return; 
         } 

         /* Call function */
         dpc->func(dpc->arg);

         mhi_osal_free(dpc); 
      }
   }
}

/** 
 * @brief      mhi_sio_seri_thread_entry
 *
 *             Entry point for SIO Thread
 *              
 * 
 * @param[in]  *ctx - Context
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
void mhi_sio_seri_thread_entry(void *ctx)
{

   mhi_ser_ctx_t *ser_ctx=(mhi_ser_ctx_t*)ctx; 

   /* Create events we will listen on */
   if(MHI_SUCCESS != mhi_osal_eventcreate(&ser_ctx->hNotify, NULL) )
   {
      mhi_osal_debug(MSG_FATAL,"mhi_sio_seri_thread_entry: Worker thread couldn't create event. Fatal error");
      return; 
   }

   /* Worker thread entry routine */
   while(1)
   {
      /* Wait on Notify event triggered by interrupts or user invocations of deferred API's */
      mhi_osal_eventwait(ser_ctx->hNotify);
      mhi_sio_seri_handle_signals(ser_ctx);

   }
}

/** 
 * @brief      mhi_sio_seri_pool_event_cb
 *
 *             Pool event CB. Called by pool when a FEW or MANY condition happens
 *              
 * 
 * @param[in]  pool_id - Pool id
 * @param[in]  mem_level - Memory level 
 * @param[in]  mem_op    - Memory operation
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
void mhi_sio_seri_pool_event_cb(dsm_mempool_id_type pool_id, dsm_mem_level_enum_type mem_level, dsm_mem_op_enum_type mem_op) 
{
   switch(mem_level)
   {
      /* Few is lower watermark when we stop flow. Nothing needs to be done we handle failure if we cant allocate.
       * In future we can tweak the level and stop all ports to prevent bringing items to a critically low level */
      case DSM_MEM_LEVEL_MHI_FEW:
         break;
         /* Many is upper watermark to resume flow */ 
      case DSM_MEM_LEVEL_MHI_MANY:

         if(pool_id == DSM_DS_LARGE_ITEM_POOL)
         {
            mhi_ser_ctx.pool[MHI_SIO_DSM_RX_POOL_DEFAULT].available=TRUE; 
         }
         else
         {
            mhi_ser_ctx.pool[MHI_SIO_DSM_RX_POOL_DIAG].available=TRUE; 
         }

         mhi_osal_atomic_set_bit(&mhi_ser_ctx.notifySig, MHI_SIO_SIG_DSM_FLOW); 
         /* Signal SIO thread */ 
         mhi_osal_eventsignal(mhi_ser_ctx.hNotify);
         break;
   }
}

/** 
 * @brief      mhi_sio_seri_flush_tx
 *
 *             Performs a flush operation by setting up a cb and marking a flush pending in the port 
 *             so that when the last write is completed the cb will be called 
 *              
 * 
 * @param[in]  *port - Port to flush on 
 * @param[in]  flush_tx_done_cb - Function to call after flush is complete
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
void mhi_sio_seri_flush_tx(mhi_sio_port_t *port, sio_vv_func_ptr_type flush_tx_done_cb)
{
   mhi_osal_syncenter(&port->sync);

   mhi_osal_debug(MSG_HIGH, "mhi_sio_seri_flush_tx: Flush called for port_id: %d", port->port_id);

   if(port->flush_pending == TRUE)
   {
      if((sio_vv_func_ptr_type)mhi_sio_seri_close_after_flush_cb == flush_tx_done_cb)
      {
         mhi_osal_debug(MSG_HIGH, "mhi_sio_seri_flush_tx: Close issued while flush is pending"); 
      }
      else
      {
         mhi_osal_debug(MSG_ERR, "mhi_sio_seri_flush_tx: Flush is already pending"); 
      }
      mhi_osal_syncleave(&port->sync);
      return; 
   }

   /* If there is nothing pending, and there are no outstanding writes issue the cb */ 
   if(NULL == port->tx_chain_remain && TRUE == dsm_is_wm_empty(port->tx_queue) && 
      0 == port->tx_submitted )
   {
      mhi_osal_syncleave(&port->sync);

      /* Issue the CB if specified */
      if( NULL != flush_tx_done_cb) 
      {
	     mhi_osal_debug(MSG_HIGH, "mhi_sio_seri_flush_tx: Invoke flush cb for port=%x",port->port_id);
         if ((void (*)(mhi_sio_port_t*))flush_tx_done_cb != mhi_sio_seri_close_after_flush_cb)
            flush_tx_done_cb();
         else if(TRUE == port->close_pending)
         {
            mhi_sio_seri_close_after_flush_cb(port);
         }
      }

      return; 
   }
   else
   {
      /* Mark a flush pending */
      port->flush_pending = TRUE; 
      port->flush_cb = flush_tx_done_cb;

      mhi_osal_syncleave(&port->sync);
      
      /* If needed in future start a timer to timeout on flush */
      return; 
   }
}

/**
 * @brief      mhi_sio_seri_ctrl_notify_cb
 * 
 *             Called by MHI Control to notify of line state changes
 *              
 * 
 * @param[in]  userdata  - userdata associated with notification
 * @param[in]  msg - Line state
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
void mhi_sio_seri_ctrl_notify_cb(void* userdata, mhi_ctrl_line_state_t msg)
{
   mhi_sio_port_t *port = userdata; 
   mhi_ctrl_line_state_t old_state; 
   /* Validate port */ 
   if(NULL == port)
   {
      mhi_osal_debug(MSG_FATAL, "mhi_sio_seri_ctrl_notify_cb: port is NULL"); 
      return; 
   }

   /* Update the line state */
   old_state=port->line_state; 
   port->line_state=msg; 

   /* If CB is set issue CB */
   if(NULL != port->dtr_event_cb.cb_func)
   {
      /* If client state seen by client is different then current line state issue the CB */
      if( (port->line_state & MHI_CTRL_LINE_STATE_DTR) != (old_state & MHI_CTRL_LINE_STATE_DTR) )
      {
         port->dtr_event_cb.cb_func(port->dtr_event_cb.cb_data); 
      }
   }
}

/** 
 * @brief      mhi_sio_seri_ioctl
 *
 *             Handle ioctl from client
 *              
 * 
 * @param[in]  port_id - port id
 * @param[in]  ioctl_cmd - ioctl command
 * @param[in]  ioctl_param_ptr* - Pointer to ioctl parameters
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
sio_status_type mhi_sio_seri_ioctl(sio_port_id_type port_id, sio_ioctl_cmd_type ioctl_cmd, sio_ioctl_param_type* ioctl_param_ptr)
{
   sio_status_type sio_status; 
   uint32 index; 
   mhi_sio_port_t  *port; 

   sio_status = mhi_sio_seri_extract_port(port_id, &index);

   if(sio_status != SIO_DONE_S)
   {
      mhi_osal_debug(MSG_ERR, "mhi_sio_ser_ioctl: Invalid port_id specified");
      return sio_status; 
   }
    
   if(portConfig[index].minor != index)
   {
      mhi_osal_debug(MSG_ERR, "mhi_sio_ser_ioctl: Unsupported port_id specified");
      return sio_status; 
   }

   port=&mhi_ser_ctx.ports[index];

   switch(ioctl_cmd)
   {
      case SIO_IOCTL_CD_ASSERT:
         /* If CD is not set send updated line state */
         if(0 == port->serial_state & MHI_CTRL_SERIAL_STATE_DCD)
         {
            port->serial_state|=MHI_CTRL_SERIAL_STATE_DCD; 
            mhi_ctrl_send_serial_state(portConfig[index].chOut+1, port->serial_state);
         }
         break; 
      case SIO_IOCTL_CD_DEASSERT:
         /* If CD is set send updated line state */
         if(1 == port->serial_state & MHI_CTRL_SERIAL_STATE_DCD)
         {
            port->serial_state&=~MHI_CTRL_SERIAL_STATE_DCD; 
            mhi_ctrl_send_serial_state(portConfig[index].chOut+1, port->serial_state);
         }
         break;
      case SIO_IOCTL_RI_ASSERT: 
         /* If RI is not set send updated line state */
         if(0 == port->serial_state & MHI_CTRL_SERIAL_STATE_RI)
         {
            port->serial_state|=MHI_CTRL_SERIAL_STATE_RI; 
            mhi_ctrl_send_serial_state(portConfig[index].chOut+1, port->serial_state);
         }

         break;
      case SIO_IOCTL_RI_DEASSERT: 
         /* If RI is set send updated line state */
         if(1 == port->serial_state & MHI_CTRL_SERIAL_STATE_RI)
         {
            port->serial_state&=~MHI_CTRL_SERIAL_STATE_RI; 
            mhi_ctrl_send_serial_state(portConfig[index].chOut+1, port->serial_state);
         }
         break;
      case SIO_IOCTL_DSR_ASSERT: 
         /* If DSR is not set send updated line state */
         if(0 == port->serial_state & MHI_CTRL_SERIAL_STATE_DSR)
         {
            port->serial_state|=MHI_CTRL_SERIAL_STATE_DSR; 
            mhi_ctrl_send_serial_state(portConfig[index].chOut+1, port->serial_state);
         }
         break;
      case SIO_IOCTL_DSR_DEASSERT: 
         /* If DSR is set send updated line state */
         if(1 == port->serial_state & MHI_CTRL_SERIAL_STATE_DSR)
         {
            port->serial_state&=~MHI_CTRL_SERIAL_STATE_DSR; 
            mhi_ctrl_send_serial_state(portConfig[index].chOut+1, port->serial_state);
         }
         break;
      case SIO_IOCTL_ENABLE_DTR_EVENT: 
      case SIO_IOCTL_ENABLE_DTR_EVENT_EXT:
         /* Set the CB */
         port->dtr_event_cb=ioctl_param_ptr->enable_dte_ready_event_ext;

         mhi_ctrl_register_notify_cb(portConfig[index].chOut, (void*)port); 
         break; 
      case SIO_IOCTL_DISABLE_DTR_EVENT: 
      case SIO_IOCTL_DISABLE_DTR_EVENT_EXT:
         /* Clear the cb */
         port->dtr_event_cb.cb_func=NULL; 
         mhi_ctrl_deregister_notify_cb(portConfig[index].chOut); 
         break;
      case SIO_IOCTL_DTE_READY_ASSERTED:
         *(((sio_ioctl_param_type *)ioctl_param_ptr)->dte_ready_asserted) = 
            port->line_state & MHI_CTRL_LINE_STATE_DTR;
         break;     
   }

   return SIO_DONE_S; 
}

/*********************   EXTERNAL functions  *****************/

/**
 * @brief      mhi_sio_ser_init
 *
 *             Initialize MHI SIO Serial device interface
 *              
 *
 * @dependencies
 *              
 * 
 * @return       
 *              MHI_SUCCESS if operation succeeded  
 * 
 * @sideeffect  None 
 */
uint32 mhi_sio_ser_init(void)
{
   mhi_osal_memset(&mhi_ser_ctx,0, sizeof(mhi_ser_ctx_t)); 

   if (DalPlatformInfo_IsFusion())
   {
      mhi_ser_ctx.isFusion=TRUE;
   } 

   /* Init queues used by thread */ 
   q_init(&mhi_ser_ctx.dsm_ser_rx_queue);
   q_init(&mhi_ser_ctx.dpc_queue);

   /* Create worker thread  */
   mhi_osal_spawn_thread(&mhi_ser_ctx.worker_thread, "MHI SIO" , mhi_sio_seri_thread_entry, (void*)&mhi_ser_ctx, MHI_SIO_WORKER_PRIORITY); 

   /* Initialize pool. Use the DSM_LARGE_ITEM_POOL */  
   mhi_sio_dsm_init_pool(&mhi_ser_ctx.pool[MHI_SIO_DSM_RX_POOL_DEFAULT], DSM_DS_LARGE_ITEM_POOL, mhi_sio_seri_pool_event_cb); 

   /* Initialize sync section */
   mhi_osal_syncinit(&mhi_ser_ctx.sync);

   if(mhi_ser_ctx.isFusion)
   {
     /* Initialize control */
      mhi_ctrl_init(&mhi_ser_ctx.ctrl_ctx, mhi_sio_seri_ctrl_notify_cb);
   }

   mhi_ser_ctx.mhi_enumerated=mhi_is_enumerated();

#ifdef MHI_SIO_USES_PORTMAPPER
   {
      /* Only register with port mapper if MHI is enumerated*/
      if (MHI_DISABLED != mhi_ser_ctx.mhi_enumerated)
      {
         /* RMNET QMI port mapper configuration */ 
         ds_port_mapper_qmi_rmnet_info_type qmi_rmnet_params;

         mhi_osal_memset(&qmi_rmnet_params, 0, sizeof(qmi_rmnet_params));

         /* Configure port mapper */
         qmi_rmnet_params.peripheral_type = DATA_EP_TYPE_PCIE_V01;
         qmi_rmnet_params.num_ctrl_ch=2; 
         qmi_rmnet_params.num_data_ch=1; 
         qmi_rmnet_params.ctrl_ch_info[0].ctl_port=SIO_PORT_MHI_QMI;
         qmi_rmnet_params.ctrl_ch_info[0].def_per_end_pt=4; 
         qmi_rmnet_params.ctrl_ch_info[1].ctl_port=SIO_PORT_MHI_QMI_2;
         qmi_rmnet_params.ctrl_ch_info[1].def_per_end_pt=4; 

         qmi_rmnet_params.data_ch_info[0].per_end_pt=4; 
         qmi_rmnet_params.data_ch_info[0].is_hw_acc=TRUE; 
         qmi_rmnet_params.data_ch_info[0].data_port=SIO_PORT_NULL; 

         ds_port_mapper_config_rmnet_instances(&qmi_rmnet_params);
      }
   }
#endif

   return 0;  
}


/**
 * @brief     mhi_sio_ser_open 
 *
 *            Opens a serial stream
 *
 * @param[in]   *open_ptr - pointer to sio_open_type structure               
 *
 * @dependencies
 *              
 * 
 * @return       
 *              SIO_DONE_S if operation succeeded  
 * 
 * @sideeffect  None 
 */

sio_status_type mhi_sio_ser_open(sio_open_type *open_ptr)
{
   uint32 index; 
   mhi_status_t status; 
   sio_status_type sio_status; 
   mhi_sio_port_t *port; 

   /* Validate arguments */
   if(NULL == open_ptr)
      return  SIO_BADP_S;

   mhi_osal_debug(MSG_HIGH,"mhi_sio_ser_open: portid=%d, Enter",open_ptr->port_id); 

   /* Check if mhi is enabled. If not fail the open */ 
   if (MHI_DISABLED == mhi_ser_ctx.mhi_enumerated)
   {
      return SIO_UNAVAIL_S;
   }

   /* Validate and extract the minor number */
   sio_status = mhi_sio_seri_extract_port(open_ptr->port_id, &index); 

   if(SIO_DONE_S != sio_status)
      return sio_status; 

   /* There should be either rx queue or rx_func_ptr spocified . If both are , we will default to rx_func_ptr */
   if(NULL == open_ptr->rx_queue && NULL == open_ptr->rx_func_ptr)
      return SIO_BADP_S; 

   /* Validate a TX WMQ is specified */
   if(NULL == open_ptr->tx_queue)
      return SIO_BADP_S; 

   mhi_osal_syncenter(&mhi_ser_ctx.sync);

   port=&mhi_ser_ctx.ports[index]; 

   /* Validate minor matches */
   if(portConfig[index].minor != index)
   {
      mhi_osal_debug(MSG_HIGH, "mhi_sio_ser_open: Port:%d doesn't match index", index); 
      mhi_osal_syncleave(&mhi_ser_ctx.sync);
      return SIO_UNAVAIL_S; 
   }

   /* Check if port was opened previously */
   if(TRUE == port->open)
   {
      mhi_osal_debug(MSG_HIGH, "mhi_sio_ser_open: Port:%d already open", index); 
      mhi_osal_syncleave(&mhi_ser_ctx.sync);
      return SIO_UNAVAIL_S; 
   }

   /* Set the port config */
   port->config=&portConfig[index];

   /* Open the mhi channels. MHI should be initialized, so do a synchronous call */	
   status = mhi_channel_open(port->config->chOut, NULL); 
   if(MHI_SUCCESS != status) 
   {
      mhi_osal_syncleave(&mhi_ser_ctx.sync);
      return SIO_UNAVAIL_S;  
   }
   status = mhi_channel_open(port->config->chOut+1, NULL); 
   if(MHI_SUCCESS != status) 
   {
      mhi_osal_syncleave(&mhi_ser_ctx.sync);
      return SIO_UNAVAIL_S;  
   }

   /* Save sio open paramaters in port context */
   port->port_id=open_ptr->port_id;
   port->pool_id=(open_ptr->port_id == SIO_PORT_MHI_DIAG?open_ptr->open_param->dsm_pool_param.dsm_mempool_id:DSM_DS_LARGE_ITEM_POOL);
   port->rx_flow=open_ptr->rx_flow;
   port->rx_func_ptr=open_ptr->rx_func_ptr;
   port->rx_queue=open_ptr->rx_queue;
   port->tx_flow=open_ptr->tx_flow; 
   port->tx_queue=open_ptr->tx_queue;
   port->stream_id=open_ptr->stream_id;
   port->stream_mode=open_ptr->stream_mode;
   port->stream_type=open_ptr->stream_type;
   port->rx_size=portConfig[index].rx_size; 
   port->tx_chain_remain=NULL; 
   port->tx_incomplete=NULL; 
   port->rx_chain_free=NULL; 
   port->rx_chain_pending=NULL; 
   port->serial_state=MHI_CTRL_SERIAL_STATE_NULL;
   port->line_state=MHI_CTRL_LINE_STATE_NULL;

   mhi_sio_dsm_init_pool(&mhi_ser_ctx.pool[open_ptr->port_id==SIO_PORT_MHI_DIAG? MHI_SIO_DSM_RX_POOL_DIAG:MHI_SIO_DSM_RX_POOL_DEFAULT], port->pool_id, mhi_sio_seri_pool_event_cb);

   /* Create a sync section for the port */
   mhi_osal_syncinit(&port->sync);

   /* Mark the port as open */  
   port->open=TRUE;  

   mhi_osal_syncleave(&mhi_ser_ctx.sync);

   /* Submit a new read request */
   mhi_sio_seri_submit_rx(&mhi_ser_ctx,port);  

   mhi_osal_debug(MSG_HIGH,"mhi_sio_ser_open: portid=%d, Exit",open_ptr->port_id); 

   return SIO_DONE_S; 
}
/**
 * @brief     mhi_sio_ser_close 
 *
 *            Closes an opened serial stream
 *
 * @param[in]     stream_id - id of stream
 * @param[in]     port_id  -  id of port
 * @param[in]     close_done_cb  -  Pointer to callback function to be called when stream is closed
 *
 *
 *
 * @dependencies
 *              
 * 
 * @return       
 *              None 
 * 
 * @sideeffect  None 
 */
void mhi_sio_ser_close(sio_stream_id_type stream_id, sio_port_id_type port_id, sio_vv_func_ptr_type close_done_cb)
{
   uint32 index; 
   mhi_sio_port_t *port; 
   sio_status_type sio_status; 

   mhi_osal_debug(MSG_HIGH,"mhi_sio_ser_close: portid=%d, Enter",port_id);

   /* Validate and extract the minor number */
   sio_status = mhi_sio_seri_extract_port(port_id, &index); 

   if(SIO_DONE_S != sio_status)
   {
      mhi_osal_debug(MSG_ERR, "mhi_sio_ser_close: Client passed in incorrect port");
      return; 
   }
   port=&mhi_ser_ctx.ports[index]; 

   mhi_sio_seri_close(port, close_done_cb);

   mhi_osal_debug(MSG_HIGH,"mhi_sio_ser_close: portid=%d, Exit",port_id); 
}

/**
 * @brief     mhi_sio_ser_ioctl 
 *
 *            Serial stream control
 *              
 * @param[in] stream_id - id of stream
 * @param[in] port_id - id of port
 * @param[in] ioctl_cmd - ioctl command to perform
 * @param[in] *ioctl_param_ptr - pointer to ioctl parameters structure
 *
 *
 * @dependencies
 *              
 * 
 * @return       
 *              None 
 * 
 * @sideeffect  None 
 */

void mhi_sio_ser_ioctl(sio_stream_id_type stream_id, sio_port_id_type port_id, sio_ioctl_cmd_type ioctl_cmd, sio_ioctl_param_type* ioctl_param_ptr)
{
   (void)mhi_sio_seri_ioctl(port_id, ioctl_cmd, ioctl_param_ptr); 

   return;
}

/**
 * @brief     mhi_sio_ser_transmit 
 *
 *            Transmits data over the serial stream
 *
 * @param[in]   stream_id - id of stream
 * @param[in]   portid - id of port  
 * @param[in]   *tx_ptr - DSM item pointer containing data to be transmitted 
 * @param[in]   stream_id - id of stream
 *              
 *
 * @dependencies
 *              
 * 
 * @return       
 *              None
 * 
 * @sideeffect  None 
 */

void mhi_sio_ser_transmit(sio_stream_id_type stream_id, sio_port_id_type port_id, dsm_item_type *tx_ptr)
{
   sio_status_type sio_status;
   mhi_sio_port_t *port; 
   uint32 index; 	

   /* Validate and extract the minor number */
   sio_status = mhi_sio_seri_extract_port(port_id, &index);
   if(SIO_DONE_S != sio_status)
      return ; 

   port=&mhi_ser_ctx.ports[index];

   /* Validate TX buffer */
   if (NULL  == tx_ptr || NULL == tx_ptr->data_ptr)
   {
      mhi_osal_debug(MSG_ERR, "mhi_sio_ser_transmit: Both tx_ptr and data_ptr are NULL");
      return; 
   }

   mhi_osal_debug(MSG_HIGH, "mhi_sio_ser_transmit: Port: %x TX Packet #:0x%x%x received", port_id, QW2DW(++port->tx_count));

   /* Submit the new packet */ 
   mhi_sio_seri_submit_tx(port, tx_ptr, TRUE); 
   return; 
}

/**
 * @brief     mhi_sio_ser_flush_tx 
 *
 *            Flushes any queued data to the device. The callback supplied is called when operation is finished.
 *              
 * @param[in]   stream_id - id of stream
 * @param[in]   portid - id of port  
 * @param[in]   flush_tx_done_cb - pointer to callback function 
 *
 *
 * @dependencies
 *              
 * 
 * @return       
 *              None
 * 
 * @sideeffect  None 
 */

void mhi_sio_ser_flush_tx( sio_stream_id_type stream_id, sio_port_id_type port_id, sio_vv_func_ptr_type flush_tx_done_cb)
{
   mhi_sio_port_t *port; 
   sio_status_type sio_status; 
   uint32 index; 

   /* Validate and extract the minor number */
   sio_status = mhi_sio_seri_extract_port(port_id, &index);
   if(SIO_DONE_S != sio_status)
   {
      mhi_osal_debug(MSG_HIGH, "mhi_sio_ser_flush_tx: Bad port_id: %d passed in", port_id); 
      return ; 
   }

   port=&mhi_ser_ctx.ports[index];
  
   mhi_osal_debug(MSG_MEDIUM, "mhi_sio_ser_flush_tx: Flush port_id=%x",port->port_id);
  
   mhi_sio_seri_flush_tx(port, flush_tx_done_cb);
}


/**
 * @brief     mhi_sio_ser_ctrl_open 
 * 
 .            Opens a serial control stream
 * 
 * @param[in]   *open_ptr - pointer to sio_open_type structure               
 *
 * @dependencies
 *              
 * 
 * @return       
 *              SIO_DONE_S if operation succeeded  
 * 
 * @sideeffect  None 
 */

sio_status_type mhi_sio_ser_ctrl_open(sio_open_type *open_ptr)
{
   uint32 index; 
   mhi_status_t status; 
   sio_status_type sio_status; 
   mhi_sio_port_t *port; 

   /* Validate arguments */
   if(NULL == open_ptr)
      return  SIO_BADP_S;

   /* Validate and extract the minor number */
   sio_status = mhi_sio_seri_extract_port(open_ptr->port_id, &index); 

   /* Only allow QMI traffic for now as we don't support multiplexing */
   if (SIO_MINOR_MHI_QMI != index && SIO_MINOR_MHI_QMI_2 != index)
      return SIO_BADP_S; 

   /* If MHI never got enumerated fail */
   if (MHI_DISABLED == mhi_ser_ctx.mhi_enumerated)
   {
      return SIO_UNAVAIL_S;
   }

   if(SIO_DONE_S != sio_status)
      return sio_status; 

   /* there should be either rx queue or rx_func_ptr spocified . If both are , we will default to rx_func_ptr */
   if(NULL == open_ptr->rx_queue && NULL == open_ptr->rx_func_ptr)
      return SIO_BADP_S; 
   
   if(NULL == open_ptr->tx_queue)
      return SIO_BADP_S; 

   mhi_osal_syncenter(&mhi_ser_ctx.sync);
   
   port=&mhi_ser_ctx.ports[index]; 

   /* Validate minor matches */
   if(portConfig[index].minor != index)
   {
      mhi_osal_debug(MSG_HIGH, "mhi_sio_ser_ctrl_open: Port:%d doesn't match index", index); 
      mhi_osal_syncleave(&mhi_ser_ctx.sync);
      return SIO_UNAVAIL_S; 
   }

   /* Check if port was opened previously */ 
   if(TRUE == port->open)
   {
      mhi_osal_debug(MSG_HIGH, "mhi_sio_ser_ctrl_open: Port:%d already open", index); 
      mhi_osal_syncleave(&mhi_ser_ctx.sync);
      return SIO_UNAVAIL_S; 
   }

   /* Set the port config */
   port->config=&portConfig[index];

   /* Open the MHI channels. MHI should be initialized, so do a synchronous call */	
   status = mhi_channel_open(port->config->chOut, NULL); 
   if(MHI_SUCCESS != status) 
   {
      mhi_osal_syncleave(&mhi_ser_ctx.sync);
      return SIO_UNAVAIL_S;  
   }
   status = mhi_channel_open(port->config->chOut+1, NULL); 
   if(MHI_SUCCESS != status) 
   {
      mhi_osal_syncleave(&mhi_ser_ctx.sync);
      return SIO_UNAVAIL_S;  
   }

   /* Save sio open parameters in port context */
   port->port_id=open_ptr->port_id;
   port->pool_id=DSM_DS_LARGE_ITEM_POOL;
   port->rx_flow=open_ptr->rx_flow;
   port->rx_func_ptr=open_ptr->rx_func_ptr;
   port->rx_queue=open_ptr->rx_queue;
   port->tx_flow=open_ptr->tx_flow; 
   port->tx_queue=open_ptr->tx_queue;
   port->stream_id=open_ptr->stream_id;
   port->stream_mode=open_ptr->stream_mode;
   port->stream_type=open_ptr->stream_type;
   port->rx_size=portConfig[index].rx_size; 
   port->tx_chain_remain=NULL; 
   port->tx_incomplete=NULL; 
   port->rx_chain_free=NULL; 
   port->rx_chain_pending=NULL; 
   port->serial_state=MHI_CTRL_SERIAL_STATE_NULL;
   port->line_state=MHI_CTRL_LINE_STATE_NULL;

   /* Create a sync section for the port */
   mhi_osal_syncinit(&port->sync);

   /* Mark the port as open */  
   port->open=TRUE;  

   mhi_osal_syncleave(&mhi_ser_ctx.sync);

   /* Submit a new read request */
   mhi_sio_seri_submit_rx(&mhi_ser_ctx,port);  

   return SIO_DONE_S; 
}

/**
 * @brief     mhi_sio_ser_ctrl_close 
 *
 *            Closes an opened serial stream
 *              
 *
 * @param[in]     stream_id - id of stream
 * @param[in]     port_id  -  id of port
 * @param[in]     close_done_cb  -  Pointer to callback function to be called when stream is closed
 *
 *
 *
 * @dependencies
 *              
 * 
 * @return       
 *              None 
 * 
 * @sideeffect  None 
 */
void mhi_sio_ser_ctrl_close(sio_stream_id_type stream_id, sio_port_id_type port_id, sio_vv_func_ptr_type close_done_cb)
{
   uint32 index; 
   mhi_sio_port_t *port; 
   sio_status_type sio_status; 

   /* Validate and extract the minor number */
   sio_status = mhi_sio_seri_extract_port(port_id, &index); 

   if(SIO_DONE_S != sio_status)
   {
      mhi_osal_debug(MSG_ERR, "mhi_sio_ser_ctrl_close: Client passed in incorrect port");
      return; 
   }
   port=&mhi_ser_ctx.ports[index]; 

   mhi_sio_seri_close(port, close_done_cb); 
}
/**
 * @brief     mhi_sio_ser_ctrl_ioctl 
 *
 *            Serial control stream control
 *              
 * @param[in] stream_id - id of stream
 * @param[in] port_id - id of port
 * @param[in] ioctl_cmd - ioctl command to perform
 * @param[in] *ioctl_param_ptr - pointer to ioctl parameters structure
 *
 *
 * @dependencies
 *              
 * 
 * @return       
 *              None 
 * 
 * @sideeffect  None 
 */

void mhi_sio_ser_ctrl_ioctl(sio_stream_id_type stream_id, sio_port_id_type port_id, sio_ioctl_cmd_type ioctl_cmd, sio_ioctl_param_type *ioctl_param_ptr)
{
   (void)mhi_sio_seri_ioctl(port_id, ioctl_cmd, ioctl_param_ptr); 

   return;
}

/**
 * @brief     mhi_sio_ser_ctrl_transmit 
 *
 *            Transmits data over the serial control stream
 *
 * @param[in]   stream_id - id of stream
 * @param[in]   portid - id of port  
 * @param[in]   *tx_ptr - DSM item pointer containing data to be transmitted 
 * @param[in]   stream_id - id of stream
 *              
 *
 * @dependencies
 *              
 * 
 * @return       
 *              None
 * 
 * @sideeffect  None 
 */

void mhi_sio_ser_ctrl_transmit(sio_stream_id_type stream_id, sio_port_id_type port_id, dsm_item_type *tx_ptr)
{
   sio_status_type sio_status;
   mhi_sio_port_t *port; 
   uint32 index; 	

   /* Validate and extract the minor number */
   sio_status = mhi_sio_seri_extract_port(port_id, &index);
   if(SIO_DONE_S != sio_status)
      return ; 

   port=&mhi_ser_ctx.ports[index];

   /* Validate TX buffer */
   if (NULL == tx_ptr || NULL == tx_ptr->data_ptr)
      return; 

   /* Submit the new packet */ 
   port->tx_count++;

   mhi_osal_debug(MSG_HIGH, "mhi_sio_ser_ctrl_transmit: Port: %x TX Packet #:0x%x%x received", port_id, QW2DW(port->tx_count));

   mhi_sio_seri_submit_tx(port, tx_ptr, TRUE); 
   return; 
}




