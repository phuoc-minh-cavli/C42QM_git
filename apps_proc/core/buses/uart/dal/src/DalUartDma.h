#ifndef __DALUARTDMA_H__
#define __DALUARTDMA_H__
/*==================================================================================================

FILE: DalUartDma.h

DESCRIPTION: This module handles DMOV operations when the UART DAL is in DMA mode.  It is
             not used in legacy mode.

                     Copyright (c) 2010-2014 Qualcomm Technologies Incorporated
                                        All Rights Reserved
                                     Qualcomm Proprietary/GTDR

==================================================================================================*/
/*==================================================================================================

$Header: //components/rel/core.tx/6.0/buses/uart/dal/src/DalUartDma.h#1 $

==================================================================================================*/
/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/

#include "DalUartTypes.h"
#include "tal.h"

/*==================================================================================================
                                              TYPEDEFS
==================================================================================================*/

typedef enum
{
   IDLE,      // no outstanding DMOV requests
   TRANSFER,  // waiting on callback for DMTransfer()
   FLUSH,     // waiting on callback for DMChanFlush() - keep data
   PURGE      // waiting on callback for DMChanFlush() - discard data
} DMA_STATE;

typedef struct
{
   TAL_DMA_CONFIG  config;         // client configuration
   TAL_DMA_HANDLE  dma_handle;     // handle to DMA device

   DMA_STATE       rx_dma_state;   // current state of RX DMA requests
   DMA_STATE       tx_dma_state;   // current state of TX DMA requests
   void *          rx_state_sync;  // protects receive state
   void *          tx_state_sync;  // protects transmit state
   void *          rx_idle_event;  // used when closing to indicate RX is idle
   void *          tx_idle_event;  // used when closing to indicate TX is idle
} DMA_CONTEXT;

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/

void          dma_close              (DMA_CONTEXT *dma_ctxt);
void          dma_deinit             (DMA_CONTEXT *dma_ctxt);
DMA_CONTEXT   *dma_init              (TAL_DMA_CONFIG *config);
boolean       dma_open               (DMA_CONTEXT *dma_ctxt);
boolean       dma_receive            (DMA_CONTEXT *dma_ctxt, TAL_DMA_DESCRIPTOR *descriptor);
void          dma_rx_flush           (DMA_CONTEXT *dma_ctxt);
void          dma_rx_intr_done       (DMA_CONTEXT *dma_ctxt);
DMA_STATE     dma_rx_intr_type       (DMA_CONTEXT *dma_ctxt);
void          dma_rx_purge           (DMA_CONTEXT *dma_ctxt);
void          dma_rx_wait_for_idle   (DMA_CONTEXT *dma_ctxt);
boolean       dma_transmit           (DMA_CONTEXT *dma_ctxt, TAL_DMA_DESCRIPTOR *descriptor);
void          dma_tx_intr_done       (DMA_CONTEXT *dma_ctxt);
void          dma_tx_purge           (DMA_CONTEXT *dma_ctxt);
void          dma_tx_wait_for_idle   (DMA_CONTEXT *dma_ctxt);

#endif  // __DALUARTDMA_H__
