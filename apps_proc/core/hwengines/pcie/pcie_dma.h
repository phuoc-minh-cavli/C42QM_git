/**
 * @file pcie_dma.h
 * @brief
 * This header files defines the public interface to the PCIe Device Driver
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

#ifndef __PCIE_DMA_H__
#define __PCIE_DMA_H__

#include "comdef.h"
#include "pcie.h"

/** PCIe DMA request result */
typedef enum
{
   PCIE_DMA_DONE_SUCCESS = 0 ,                   /** < -- DMA Done Successfully */
   PCIE_DMA_WRITE_AXI_ERROR_DETECTED = 1 ,       /** < -- Fatal Error,DMA write channel has received an error response from the AXI bus */
   PCIE_DMA_READ_AXI_ERROR_DETECTED = 2 ,        /** < -- Fatal Error,DMA Read channel has received an error response from the AXI bus */
   PCIE_DMA_READ_UR  = 3 ,                       /** < -- DMA read channel has received a PCIe UR CPL Status */
   PCIE_DMA_READ_CA = 4 ,                        /** < -- DMA read channel has received a PCIe CA CPL status */
   PCIE_DMA_READ_CPL_TIMEOUT = 5 ,               /** < -- DMA read channel has timed-out while waiting for the remote device to respond to the MRd request */
   PCIE_DMA_READ_DATA_POISONING = 6 ,            /** < -- DMA read channel has detected data poisoning in the CPL */
   PCIE_DMA_STOPPED  = 7 ,                       /** < -- DMA read channel has detected data poisoning in the CPL */
   PCIE_DMA_READ_ERROR_UNKOWN = 8                /** < -- DMA READ UNKONWN Error */
}pcie_dma_interrupt_result_type;

/** PCIe DMA read or write request return type */
typedef enum
{
   PCIE_DMA_ACCEPTED = 0 ,            /** < -- DMA request is accpeted and assigned channel to handle it */
   PCIE_DMA_CHANNEL_FULL  = 1 ,       /** < -- DMA Channel Full. Cannot accept more request */
   PCIE_DMA_STOP_SUCCESS  = 2 ,       /** < -- DMA STOP success */
   PCIE_DMA_STOP_FAILURE  = 3 ,       /** < -- DMA STOP failure */
   PCIE_DMA_ERROR = 4,                /** < -- DMA other errors */
   PCIE_DMA_NOT_READY = 5             /** < -- DMA not ready */
}pcie_dma_req_return_type;

/** PCIe DMA request parameters structure */
typedef struct
{
   uint32 host_phy_addr_low;       /** < -- Lower bits of Host Physical Address */
   uint32 host_phy_addr_hi;        /** < -- Upper bits of Host Physical Address */
   uint32 device_phy_addr_low;     /** < -- Lower bits of Device Physical Address */
   uint32 device_phy_addr_hi;      /** < -- Upper bits of Device Physical Address */
   uint32 size;                    /** < -- Size of data */
   uint32 rw_options;              /** < -- Read/Write options */
   void *user_data;                /** < -- User data */
}pcie_dma_req_type;

/* ============================================================================
 **  Function : pcie_dma_register_cb
 ** ============================================================================
 */
/**
 * Registers pcie dma call back
 *
 * @param[in,out]   read_cb  call back pointer
 *
 * @return          void
 */
void pcie_dma_register_cb(dma_req_cb read_cb);

/* ============================================================================
**  Function : pcie_dma_init
** ============================================================================
*/
/**
 * Initialize PCIe DMA HW and SW module.
 *
 * @param[in]   void
 *
 * @return      pcie_return_status
 */
enum _pcie_return_status pcie_dma_init(void);

/* ============================================================================
**  Function : pcie_bam_regs_dump_init
** ============================================================================
*/
/**
 * Initialize for dumping the BAM registers.
 *
 * @param[in]    void
 *
 * @return       pcie_return_status
 */
enum _pcie_return_status pcie_bam_regs_dump_init(void);

/* ============================================================================
**  Function : pcie_bam_regs_dump
** ============================================================================
*/
/**
 * Dumps the BAM registers.
 *
 * @param[in]     void
 *
 * @return        void
 */
void pcie_bam_regs_dump(void);

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
void pcie_dma_deinit(void);

/* ============================================================================
**  Function : pcie_dma_reset
** ============================================================================
*/
/**
 * Reset PCIe DMA HW and SW.Running DMA request will be stopped
 *
 * @param[in]     void.
 *
 * @return        void
 */
void pcie_dma_reset(void);

/* ============================================================================
**  Function : pcie_dma_read
** ============================================================================
*/
/**
 * Read data from the host memory through PCIe DMA.
 *
 * @param[in,out]     read_req   PCIe  DMA read request.
 *
 * @return            pcie_dma_req_return_type
 *    PCIE_DMA_ACCEPTED,
 *    PCIE_DMA_CHANNEL_FULL,
 *    PCIE_DMA_STOP_SUCCESS,
 *    PCIE_DMA_STOP_FAILURE,
 *    PCIE_DMA_ERROR
 */
pcie_dma_req_return_type pcie_dma_read(pcie_dma_req_type *read_req);

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
pcie_dma_req_return_type pcie_dma_stop_read(void);

/* ============================================================================
**  Function : pcie_dma_write
** ============================================================================
*/
/**
 * Write data to the host memory through PCIe DMA.
 *
 * @param[in,out]    write_req   PCIe  DMA write request.
 *
 * @return           pcie_dma_req_return_type
 *    PCIE_DMA_ACCEPTED,
 *    PCIE_DMA_CHANNEL_FULL,
 *    PCIE_DMA_STOP_SUCCESS,
 *    PCIE_DMA_STOP_FAILURE,
 *    PCIE_DMA_ERROR
 */
pcie_dma_req_return_type pcie_dma_write(pcie_dma_req_type *write_req);

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
pcie_dma_req_return_type pcie_dma_stop_write(void);

/* ============================================================================
 **  Function : pcie_dma_flush
 ** ============================================================================
 */
/**
 * Setup a transaction to flush out all descriptors with NO_SUBMIT flag
 *
 * @param[in]   void
 *
 * @return      void
 */
void pcie_dma_flush(void);

/* ============================================================================
**  Function : pcie_dma_set_state
** ============================================================================
*/
/**
 * Sets the dma state.
 *
 * @param[in]      boolean
 *
 * @return         void
 */
void pcie_dma_set_state(boolean state);

/* ============================================================================
**  Function : pcie_dma_get_state
** ============================================================================
*/
/**
 * Gets the dma state.
 *
 * @param[in]          void
 *
 * @return             boolean
 */
boolean pcie_dma_get_state(void);

/* ============================================================================
**  Function : pcie_dma_poll
** ============================================================================
*/
/**
 * Poll for dma request.
 *
 * @param[in]     void
 *
 * @return        pcie_return_status
 *                Status of the operation
 */
enum _pcie_return_status pcie_dma_poll(void);

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
pcie_dma_req_return_type pcie_dma_qmb_memcpy(void *dst, void* src, uint32 size, void* user_data);

#endif  /*__PCIE_DMA_H__*/
