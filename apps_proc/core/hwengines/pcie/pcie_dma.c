/**
 * @file pcie_dma.c
 * @brief
 * Implements the interfaces to the PCIe Core HW
 */
/*
===============================================================================

Edit History

$Header:

Change revision history maintained in version system
===============================================================================
Copyright (c) 2013-15 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===============================================================================
*/
#include "pcie_osal.h"
#include "pcie_dma.h"
#include "pcie_tgt.h"
#include "pcie.h"
#include "bam.h"

#ifndef PCIE_BOOT
#include "ipa_err.h"
#include "ipa_per_common.h"
#include "ipa_per_mhi.h"
#endif

#define IPA_DMA_BASE          0x7904000              /** IPADMA Constants */
#define PIPE_CB_PRODUCER      0x1F                   /** markers to identify the producer callback*/
#define PIPE_CB_CONSUMER      0xF1                   /** markers to identify the consumer callback*/

#define DESC_SIZE             200*8                  /** Descriptor size */
#define NUM_CHANNELS          2                      /** Number of DMA Channels */
#define PCIE_BAM_PIPE_POLL_TIMEOUT_MS  1000          /** Timeout for BAM pipe poll */
#define PCIE_DMA_QMB_MEMCPY_WAIT       1000          /** Wait time for QMB sync memcopy */
#define PCIE_DMA_QMB_NOT_READY         -9            /** QMB not ready */

/* Utility macros */
#define ALIGN8(x) (((unsigned int)(x)+7)&(~7))

/* Error codes */
#define DMA_ERROR  -1
#define DMA_FULL    1

/* Defines for BAM Pipe */
#define PCIE_DMA_CHAN0_PROD_PIPE             0
#define PCIE_DMA_CHAN0_CONS_PIPE             1
#define PCIE_DMA_CHAN1_PROD_PIPE             11
#define PCIE_DMA_CHAN1_CONS_PIPE             12
#define PCIE_BAM_NUM_PIPES                   4
#define PCIE_BAM_NUM_OF_BAMS                 1
#define PCIE_BAM_DUMP_MASK                   (0x00000000 | (1<<PCIE_DMA_CHAN0_PROD_PIPE) | (1<<PCIE_DMA_CHAN0_CONS_PIPE) | (1<<PCIE_DMA_CHAN1_PROD_PIPE) | (1<<PCIE_DMA_CHAN1_CONS_PIPE) )

/** DMA operation */
typedef enum
{
   DMA_OP_READ,  /** < -- DMA read operation */
   DMA_OP_WRITE, /** < -- DMA write operation */
}dma_op;

/** DMA device context */
typedef struct _pcie_dma_dev
{
   bam_handle bam_dev;                  /** < -- BAM device handle */
   bam_handle cons_pipe[NUM_CHANNELS];  /** < -- Handles for consumer pipe */
   bam_handle prod_pipe[NUM_CHANNELS];  /** < -- Handles for producer pipe */
   pcie_osal_meminfo desc_base;         /** < -- Pointer to descriptor base */
   dma_req_cb async_rw_req_cb;          /** < -- Generic callback for asynchronous read/write requests */
   boolean pcie_dma_state;              /** < -- State of PCIe DMA */
}pcie_dma_dev_t;

/** Pipe numbers assigned by IPA */
typedef struct _pcie_dma_ch
{
   uint32 cons_pipe;                      /** < -- Consumer pipe # */
   uint32 prod_pipe;                      /** < -- Producer pipe # */
#ifndef PCIE_BOOT
   ipa_pipe_handle_type cons_pipe_handle; /** < -- Consumer pipe handle */
   ipa_pipe_handle_type prod_pipe_handle; /** < -- Producer pipe handle */
#endif /* PCIE_BOOT */
} pcie_dma_ch_t;


#ifndef PCIE_BOOT
/* IPA memcpy */
/* IPA should move this typedef to their header */
#ifdef PCIE_ENABLE_HIGH_PRIORITY_MSG
typedef uint32 ipa_channel_h;
ipa_channel_h pcie_memcpy_h;
ipa_channel_h pcie_async_memcpy_h;
#endif /* PCIE_ENABLE_HIGH_PRIORITY_MSG */

#else
/** Flag for BAM register dump allocation status */
boolean pcie_bam_dump_alloc_success = TRUE;
#endif /* PCIE_BOOT */

/** Prototypes */
#ifndef PCIE_BOOT
/* Callback supplied to async memcpy driver for completion async requests. */
void pcie_dma_qmb_async_memcpy_cb(uint32 user_data);
#endif


/* ============================================================================
 **  Function : pcie_dma_register_cb
 ** ============================================================================
 */
/**
 * Registers pcie dma call back
 *
 * @param[in]   read_cb     call back pointer
 *
 * @return      void
 */
void pcie_dma_register_cb(dma_req_cb read_cb)
{
}

/* ============================================================================
 **  Function : pcie_dma_init
 ** ============================================================================
 */
/**
 * Initialize PCIe DMA HW and SW module.
 *
 * @param[in]    void
 *
 * @return       pcie_return_status
 */
pcie_return_status pcie_dma_init(void)
{
   return PCIE_SUCCESS;
}

/* ============================================================================
 **  Function : pcie_dma_deinit
 ** ============================================================================
 */
/**
 * DeInitialize PCIe DMA HW and SW module.
 *
 * @param[in]    void.
 *
 * @return       void
 */
void pcie_dma_deinit(void)
{
}

#ifndef PCIE_BOOT
/* ============================================================================
 **  Function : pcie_dma_reset
 ** ============================================================================
 */
/**
 * Reset PCIe DMA HW and SW.Running DMA request will be stopped
 *
 * @param[in]    void.
 *
 * @return       void
 */
void pcie_dma_reset(void)
{
}
#endif /* PCIE_BOOT */

/* ============================================================================
 **  Function : pcie_dma_read
 ** ============================================================================
 */
/**
 * Read data from the host memory through PCIe DMA.
 *
 * @param[in,out]     read_req   PCIe DMA read request.
 *
 * @return            pcie_dma_req_return_type
 *    PCIE_DMA_ACCEPTED,
 *    PCIE_DMA_CHANNEL_FULL,
 *    PCIE_DMA_STOP_SUCCESS,
 *    PCIE_DMA_STOP_FAILURE,
 *    PCIE_DMA_ERROR
 */
pcie_dma_req_return_type pcie_dma_read(pcie_dma_req_type *read_req)
{
   pcie_osal_memcpy((void*)read_req->device_phy_addr_low, read_req->size, (void*)read_req->host_phy_addr_low, read_req->size);

   return(PCIE_DMA_ACCEPTED);
}

/* ============================================================================
 **  Function : pcie_dma_write
 ** ============================================================================
 */
/**
 * Write data to the host memory through PCIe DMA.
 *
 * @param[in,out]     write_req   PCIe  DMA write request.
 *
 * @return            pcie_dma_req_return_type
 *    PCIE_DMA_ACCEPTED,
 *    PCIE_DMA_CHANNEL_FULL,
 *    PCIE_DMA_STOP_SUCCESS,
 *    PCIE_DMA_STOP_FAILURE,
 *    PCIE_DMA_ERROR
 */
pcie_dma_req_return_type pcie_dma_write(pcie_dma_req_type *write_req)
{
   pcie_osal_memcpy((void*)write_req->host_phy_addr_low, write_req->size, (void*)write_req->device_phy_addr_low, write_req->size);

   return(PCIE_DMA_ACCEPTED);
}

#ifndef PCIE_BOOT
/* ============================================================================
 **  Function : pcie_dma_stop_read
 ** ============================================================================
 */
/**
 * Stop running DMA read request.
 *
 * @param[in]     void
 *
 * @return        pcie_dma_req_return_type
 *    PCIE_DMA_ACCEPTED,
 *    PCIE_DMA_CHANNEL_FULL,
 *    PCIE_DMA_STOP_SUCCESS,
 *    PCIE_DMA_STOP_FAILURE,
 *    PCIE_DMA_ERROR
 */
pcie_dma_req_return_type pcie_dma_stop_read(void)
{
   return(PCIE_DMA_STOP_SUCCESS);
}

/* ============================================================================
 **  Function : pcie_dma_stop_write
 ** ============================================================================
 */
/**
 * Stop running DMA write request.
 *
 * @param[in]     void
 *
 * @return        pcie_dma_req_return_type
 *    PCIE_DMA_ACCEPTED,
 *    PCIE_DMA_CHANNEL_FULL,
 *    PCIE_DMA_STOP_SUCCESS,
 *    PCIE_DMA_STOP_FAILURE,
 *    PCIE_DMA_ERROR
 */
pcie_dma_req_return_type pcie_dma_stop_write(void)
{
   return(PCIE_DMA_STOP_SUCCESS);
}

/* ============================================================================
 **  Function : pcie_dma_flush
 ** ============================================================================
 */
/**
 * Setup a transaction to flush out all descriptors with NO_SUBMIT flag
 *
 * @param[in]     void
 *
 * @return        void
 */
void pcie_dma_flush(void)
{
}
#else
/* ============================================================================
**  Function : pcie_dma_poll
** ============================================================================
*/
/**
 * Poll for dma request
 *
 * @param[in]     Void
 *
 * @return        pcie_return_status
 *                Status of the operation
 */
pcie_return_status pcie_dma_poll(void)
{
   return(PCIE_SUCCESS);
}
#endif /* PCIE_BOOT */

/* ============================================================================
**  Function : pcie_dma_set_state
** ============================================================================
*/
/**
 * Sets the dma state.
 *
 * @param[in]     boolean
 *
 * @return        void
 */
void pcie_dma_set_state(boolean state)
{
}

/* ============================================================================
**  Function : pcie_dma_get_state
** ============================================================================
*/
/**
 * Gets the dma state.
 *
 * @param[in]      void
 *
 * @return         Boolean
 */

boolean pcie_dma_get_state(void)
{
   return(ENABLED);
}

/* ============================================================================
**  Function : pcie_bam_regs_dump_init
** ============================================================================
*/
/**
 * Initialize for dumping the BAM registers.
 *
 * @param[in]      void
 *
 * @return         pcie_return_status
 */
pcie_return_status pcie_bam_regs_dump_init(void)
{
   return PCIE_SUCCESS;
}

/* ============================================================================
**  Function : pcie_bam_regs_dump
** ============================================================================
*/
/**
 * Dumps the BAM registers.
 *
 * @param[in]      void
 *
 * @return         void
 */
void pcie_bam_regs_dump(void)
{
}

/* ============================================================================
**  Function :pcie_dma_qmb_async_memcpy_cb
** ============================================================================
*/
/**
 * Callback supplied to async memcpy driver for completion async requests.
 *
 * @param[in]      user_data      User data
 *
 * @return         void
 */
void pcie_dma_qmb_async_memcpy_cb(uint32 user_data)
{
}

/* ============================================================================
** Function: pcie_dma_qmb_memcpy
** ============================================================================
*/
/**
 * Performs either sync/asyn QMB memcopy based on user data
 *
 * @param[in]      dst       Pointer to desitnation buffer
 * @param[in]      src       Pointer to source buffer
 * @param[in]      size      Size of transfer
 * @param[in]      user_data User data to callback with
 *
 * @return         pcie_dma_req_return_type
 */
pcie_dma_req_return_type pcie_dma_qmb_memcpy(void *dst, void* src, uint32 size, void* user_data)
{
   return(PCIE_DMA_ACCEPTED);
}
