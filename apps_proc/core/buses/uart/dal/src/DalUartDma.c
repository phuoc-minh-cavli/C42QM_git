/*==================================================================================================

FILE: DalUartDma.c

DESCRIPTION: This module handles DMOV operations when the UART DAL is in DMA mode.  It is
             not used in legacy mode.

                     Copyright (c) 2010-2014 Qualcomm Technologies Incorporated
                                        All Rights Reserved
                                     Qualcomm Proprietary/GTDR

==================================================================================================*/
/*==================================================================================================

$Header: //components/rel/core.tx/6.0/buses/uart/dal/src/DalUartDma.c#1 $

==================================================================================================*/
/*==================================================================================================
                                            DESCRIPTION
====================================================================================================

GLOBAL FUNCTIONS:
   dma_close
   dma_deinit
   dma_init
   dma_open
   dma_receive
   dma_rx_flush
   dma_rx_intr_type
   dma_rx_intr_done
   dma_rx_purge
   dma_rx_wait_for_idle
   dma_transmit
   dma_tx_intr_done
   dma_tx_purge
   dma_tx_wait_for_idle

==================================================================================================*/
/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/

#if ( defined(_WIN32) )  // needed for logging
#include <wdm.h>
#include "debug.h"
#include "DalUartDma.tmh"
#endif

#include "DalUartDma.h"
#include "DalUartTypes.h"
#include "tal.h"
#include "string.h"

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

static boolean create_dma_objects (DMA_CONTEXT *dma_ctxt);
static void    destroy_dma_objects(DMA_CONTEXT *dma_ctxt);

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/
/*==================================================================================================

FUNCTION: create_dma_objects

DESCRIPTION:

==================================================================================================*/
static boolean create_dma_objects(DMA_CONTEXT *dma_ctxt)
{
   void *id = dma_ctxt->config.device_id;

   if ((dma_ctxt->rx_state_sync = tal_os_sync_create())  == NULL) { goto error; }
   if ((dma_ctxt->tx_state_sync = tal_os_sync_create())  == NULL) { goto error; }
   if ((dma_ctxt->rx_idle_event = tal_os_event_create()) == NULL) { goto error; }
   if ((dma_ctxt->tx_idle_event = tal_os_event_create()) == NULL) { goto error; }

   return TRUE;
error:
   tal_os_log(id, ERROR, "create_dma_objects: failed");
   return FALSE;
}

/*==================================================================================================

FUNCTION: destroy_dma_objects

DESCRIPTION:

==================================================================================================*/
static void destroy_dma_objects(DMA_CONTEXT *dma_ctxt)
{
   if (dma_ctxt->rx_state_sync) { tal_os_sync_destroy (dma_ctxt->rx_state_sync); }
   if (dma_ctxt->tx_state_sync) { tal_os_sync_destroy (dma_ctxt->tx_state_sync); }
   if (dma_ctxt->rx_idle_event) { tal_os_event_destroy(dma_ctxt->rx_idle_event); }
   if (dma_ctxt->tx_idle_event) { tal_os_event_destroy(dma_ctxt->tx_idle_event); }
}

/*==================================================================================================
                                          GLOBAL FUNCTIONS
==================================================================================================*/
/*==================================================================================================

FUNCTION: dma_close

DESCRIPTION:

==================================================================================================*/
void dma_close(DMA_CONTEXT *dma_ctxt)
{
   if (dma_ctxt->dma_handle)
   {
      tal_dma_close(dma_ctxt->dma_handle);
      dma_ctxt->dma_handle = NULL;
   }
}

/*==================================================================================================

FUNCTION: dma_deinit

DESCRIPTION:

==================================================================================================*/
void dma_deinit(DMA_CONTEXT *dma_ctxt)
{
   if (!dma_ctxt) { return; }

   destroy_dma_objects(dma_ctxt);
   tal_os_free(dma_ctxt);
}

/*==================================================================================================

FUNCTION: dma_init

DESCRIPTION:

==================================================================================================*/
DMA_CONTEXT *dma_init(TAL_DMA_CONFIG *config)
{
   void *id = config->device_id;
   DMA_CONTEXT *dma_ctxt;

   dma_ctxt = tal_os_malloc(sizeof(DMA_CONTEXT));
   if (dma_ctxt == NULL) { goto error; }

   memset(dma_ctxt, 0, sizeof(DMA_CONTEXT));
   dma_ctxt->config = *config;

   tal_os_log(id, INFO, "dma_init: rx_dma_state = IDLE");
   dma_ctxt->rx_dma_state = IDLE;

   tal_os_log(id, INFO, "dma_init: tx_dma_state = IDLE");
   dma_ctxt->tx_dma_state = IDLE;

   if ( !create_dma_objects(dma_ctxt) ) { goto error; }

   return(dma_ctxt);
error:
   dma_deinit(dma_ctxt);
   tal_os_log(id, ERROR, "dma_init: failed");
   return(NULL);
}

/*==================================================================================================

FUNCTION: dma_open

DESCRIPTION:

==================================================================================================*/
boolean dma_open(DMA_CONTEXT *dma_ctxt)
{
   void *id = dma_ctxt->config.device_id;
   TAL_RESULT result;

   result = tal_dma_open(&dma_ctxt->dma_handle, &dma_ctxt->config);
   if (result != TAL_SUCCESS) { goto error; }

   return TRUE;
error:
   tal_os_log(id, ERROR, "dma_open: failed");
   return FALSE;
}

/*==================================================================================================

FUNCTION: dma_receive

DESCRIPTION:
   Start a DMA RX transfer (UART RXFIFO -> uncached RAM)

==================================================================================================*/
boolean dma_receive(DMA_CONTEXT *dma_ctxt, TAL_DMA_DESCRIPTOR *descriptor)
{
   void *id = dma_ctxt->config.device_id;
   TAL_RESULT result;
   boolean ret = FALSE;

   tal_os_sync_enter(dma_ctxt->rx_state_sync);

   if (dma_ctxt->rx_dma_state != IDLE)
   {
      tal_os_log(id, ERROR, "dma_receive: DMA not ready");
   }
   else
   {
      result = tal_dma_rx_transfer(dma_ctxt->dma_handle, descriptor);

      if (result != TAL_SUCCESS)
      {
         tal_os_log(id, ERROR, "dma_receive: transfer failed");
      }
      else
      {
         tal_os_log(id, INFO, "dma_receive: rx_dma_state = TRANSFER");
         dma_ctxt->rx_dma_state = TRANSFER;
         ret = TRUE;
      }
   }

   tal_os_sync_exit(dma_ctxt->rx_state_sync);
   return(ret);
}

/*==================================================================================================

FUNCTION: dma_rx_flush

DESCRIPTION:

==================================================================================================*/
void dma_rx_flush(DMA_CONTEXT *dma_ctxt)
{
   void *id = dma_ctxt->config.device_id;
   TAL_RESULT result;

   tal_os_sync_enter(dma_ctxt->rx_state_sync);

   if (dma_ctxt->rx_dma_state == TRANSFER)
   {
      tal_os_log(id, INFO, "dma_rx_flush: calling tal_dma_rx_cancel()");

      result = tal_dma_rx_cancel(dma_ctxt->dma_handle);

      if (result != TAL_SUCCESS)
      {
         tal_os_log(id, ERROR, "dma_rx_flush: failed to flush RX DMA channel");
      }

      tal_os_log(id, INFO, "dma_rx_flush: rx_dma_state = FLUSH");
      dma_ctxt->rx_dma_state = FLUSH;
   }

   tal_os_sync_exit(dma_ctxt->rx_state_sync);
}

/*==================================================================================================

FUNCTION: dma_rx_intr_done

DESCRIPTION:

==================================================================================================*/
void dma_rx_intr_done(DMA_CONTEXT *dma_ctxt)
{
   void *id = dma_ctxt->config.device_id;

   tal_os_sync_enter(dma_ctxt->rx_state_sync);
   tal_os_log(id, INFO, "dma_rx_intr_done: rx_dma_state = IDLE");
   dma_ctxt->rx_dma_state = IDLE;
   tal_os_event_set(dma_ctxt->rx_idle_event);
   tal_os_sync_exit(dma_ctxt->rx_state_sync);
}

/*==================================================================================================

FUNCTION: dma_rx_intr_type

DESCRIPTION:

==================================================================================================*/
DMA_STATE dma_rx_intr_type(DMA_CONTEXT *dma_ctxt)
{
   DMA_STATE interrupt_type;

   tal_os_sync_enter(dma_ctxt->rx_state_sync);
   interrupt_type = dma_ctxt->rx_dma_state;
   tal_os_sync_exit(dma_ctxt->rx_state_sync);

   return(interrupt_type);
}

/*==================================================================================================

FUNCTION: dma_rx_purge

DESCRIPTION:

==================================================================================================*/
void dma_rx_purge(DMA_CONTEXT *dma_ctxt)
{
   void *id = dma_ctxt->config.device_id;
   TAL_RESULT result;

   tal_os_sync_enter(dma_ctxt->rx_state_sync);

   if (dma_ctxt->rx_dma_state == TRANSFER)
   {
      tal_os_log(id, INFO, "dma_rx_purge: calling tal_dma_rx_cancel()");

      result = tal_dma_rx_cancel(dma_ctxt->dma_handle);

      if (result != TAL_SUCCESS)
      {
         tal_os_log(id, ERROR, "dma_rx_purge: failed to flush RX DMA channel");
      }

      tal_os_log(id, INFO, "dma_rx_purge: rx_dma_state = PURGE");
      dma_ctxt->rx_dma_state = PURGE;
   }
   else if (dma_ctxt->rx_dma_state == FLUSH)
   {
      tal_os_log(id, INFO, "dma_rx_purge: rx_dma_state = PURGE");
      dma_ctxt->rx_dma_state = PURGE;
   }

   tal_os_sync_exit(dma_ctxt->rx_state_sync);
}

/*==================================================================================================

FUNCTION: dma_rx_wait_for_idle

DESCRIPTION:
   This function waits for the RX DMA channel to return to the IDLE state.

==================================================================================================*/
void dma_rx_wait_for_idle(DMA_CONTEXT *dma_ctxt)
{
   void *id = dma_ctxt->config.device_id;
   boolean rx_idle;
   TAL_RESULT result;

   tal_os_sync_enter(dma_ctxt->rx_state_sync);
   tal_os_event_reset(dma_ctxt->rx_idle_event);
   rx_idle = (dma_ctxt->rx_dma_state == IDLE);
   tal_os_sync_exit(dma_ctxt->rx_state_sync);

   if (!rx_idle)
   {
      result = tal_os_event_wait(dma_ctxt->rx_idle_event, 500);

      if ( result != TAL_SUCCESS )  // force the RX state to idle and hope for the best
      {
         tal_os_sync_enter(dma_ctxt->rx_state_sync);
         tal_os_log(id, ERROR, "dma_rx_wait_for_idle: wait failed");
         tal_os_log(id, INFO, "dma_rx_wait_for_idle: rx_dma_state = IDLE");
         dma_ctxt->rx_dma_state = IDLE;
         tal_os_sync_exit(dma_ctxt->rx_state_sync);
      }
   }
}

/*==================================================================================================

FUNCTION: dma_transmit

DESCRIPTION:
   Start a DMA TX transfer (uncached RAM -> UART TXFIFO)

==================================================================================================*/
boolean dma_transmit(DMA_CONTEXT *dma_ctxt, TAL_DMA_DESCRIPTOR *descriptor)
{
   void *id = dma_ctxt->config.device_id;
   TAL_RESULT result;
   boolean ret = FALSE;

   tal_os_sync_enter(dma_ctxt->tx_state_sync);

   if (dma_ctxt->tx_dma_state != IDLE)
   {
      tal_os_log(id, ERROR, "dma_transmit: DMA not ready");
   }
   else
   {
      result = tal_dma_tx_transfer(dma_ctxt->dma_handle, descriptor);

      if (result != TAL_SUCCESS)
      {
         tal_os_log(id, ERROR, "dma_transmit: transfer failed");
      }
      else
      {
         tal_os_log(id, INFO, "dma_transmit: tx_dma_state = TRANSFER");
         dma_ctxt->tx_dma_state = TRANSFER;
         ret = TRUE;
      }
   }

   tal_os_sync_exit(dma_ctxt->tx_state_sync);
   return(ret);
}

/*==================================================================================================

FUNCTION: dma_tx_intr_done

DESCRIPTION:

==================================================================================================*/
void dma_tx_intr_done(DMA_CONTEXT *dma_ctxt)
{
   void *id = dma_ctxt->config.device_id;

   tal_os_sync_enter(dma_ctxt->tx_state_sync);
   tal_os_log(id, INFO, "dma_tx_intr_done: tx_dma_state = IDLE");
   dma_ctxt->tx_dma_state = IDLE;
   tal_os_event_set(dma_ctxt->tx_idle_event);
   tal_os_sync_exit(dma_ctxt->tx_state_sync);
}

/*==================================================================================================

FUNCTION: dma_tx_purge

DESCRIPTION:

==================================================================================================*/
void dma_tx_purge(DMA_CONTEXT *dma_ctxt)
{
   void *id = dma_ctxt->config.device_id;
   TAL_RESULT result;

   tal_os_sync_enter(dma_ctxt->tx_state_sync);

   if (dma_ctxt->tx_dma_state == TRANSFER)
   {
      tal_os_log(id, INFO, "dma_tx_purge: calling tal_dma_tx_cancel()");

      result = tal_dma_tx_cancel(dma_ctxt->dma_handle);

      if (result != TAL_SUCCESS)
      {
         tal_os_log(id, ERROR, "dma_tx_purge: failed to flush TX DMA channel");
      }

      tal_os_log(id, INFO, "dma_tx_purge: tx_dma_state = PURGE");
      dma_ctxt->tx_dma_state = PURGE;
   }

   tal_os_sync_exit(dma_ctxt->tx_state_sync);
}

/*==================================================================================================

FUNCTION: dma_tx_wait_for_idle

DESCRIPTION:
   This function waits for the TX DMA channel to return to the IDLE state.

==================================================================================================*/
void dma_tx_wait_for_idle(DMA_CONTEXT *dma_ctxt)
{
   void *id = dma_ctxt->config.device_id;
   boolean tx_idle;
   TAL_RESULT result;

   tal_os_sync_enter(dma_ctxt->tx_state_sync);
   tal_os_event_reset(dma_ctxt->tx_idle_event);
   tx_idle = (dma_ctxt->tx_dma_state == IDLE);
   tal_os_sync_exit(dma_ctxt->tx_state_sync);

   if (!tx_idle)
   {
      result = tal_os_event_wait(dma_ctxt->tx_idle_event, 500);

      if ( result != TAL_SUCCESS )  // force the TX state to idle and hope for the best
      {
         tal_os_sync_enter(dma_ctxt->tx_state_sync);
         tal_os_log(id, ERROR, "dma_tx_wait_for_idle: wait failed");
         tal_os_log(id, INFO, "dma_tx_wait_for_idle: tx_dma_state = IDLE");
         dma_ctxt->tx_dma_state = IDLE;
         tal_os_sync_exit(dma_ctxt->tx_state_sync);
      }
   }
}
