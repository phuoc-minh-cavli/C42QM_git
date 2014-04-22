#ifndef _SPIDEVICE_H_
#define _SPIDEVICE_H_
/*==================================================================================================

FILE: SpiDevice.h

DESCRIPTION:
   This file defines an API for the SPI common device layer.

====================================================================================================

   Edit History

When      Who  What, where, why
--------  ---  ------------------------------------------------------------
10/04/16  ska  Added enum for Mem copy Failure
06/08/16  ska  Ported for TN
03/10/16  dpk  Added PCNOC Vote/Devote in SPI Open/Close path
12/07/15  sg   Added GPIO Low power mode support in SPI close path
06/11/12  dk   Added review comments.
03/27/12  ag   Added batch transfer updates.
09/26/11  ag   Created

====================================================================================================
                     Copyright (c) 2011-2016 Qualcomm Technologies Incorporated
                                        All Rights Reserved
                                     Qualcomm Proprietary/GTDR
==================================================================================================*/

#include "SpiDeviceTypes.h"
#include "SpiDeviceError.h"
#include "spi_defs.h"

/*==================================================================================================
                                              TYPEDEFS
==================================================================================================*/

typedef enum
{
   SPIDEVICE_ERR_BASE = SPIDEVICE_RES_ERROR_CLS_DEVICE,
   SPIDEVICE_ERR_CLOCK_SETTING_FAILED,
   SPIDEVICE_ERR_INVALID_CONFIG_POINTER,
   SPIDEVICE_ERR_INVALID_DEV_PARAMETERS,
   SPIDEVICE_ERR_INVALID_BUFFER_POINTER,
   SPIDEVICE_ERR_INVALID_BUFFER_LEN,
   SPIDEVICE_ERR_INVALID_RUNSTATE,
   SPIDEVICE_ERR_INVALID_DEV_HANDLE,
   SPIDEVICE_ERR_INVALID_PTR,
   SPIDEVICE_ERR_INVALID_NUM_BATCH_TRANSFER_VECS,
   SPIDEVICE_TIMEOUT,
   SPIDEVICE_FIFO_ERROR,
   SPIDEVICE_TRANSFER_CANCELED,
   SPIDEVICE_ERR_MEM_CPY_FAILED,
} SPIDEVICE_ERROR;

typedef void* SPIDEVICE_HANDLE;

typedef enum
{
   SPIDEVICE_POWER_STATE_0,  // bus and clocks off
   SPIDEVICE_POWER_STATE_1,  // bus on clocks off
   SPIDEVICE_POWER_STATE_2,  // bus on clocks on
} SpiDevice_PowerStates;

typedef enum
{
   SPIDEVICE_GPIO_STATE_ACTIVE,  
   SPIDEVICE_GPIO_STATE_INACTIVE,  
} SpiDevice_GpioStates;

typedef enum
{
   SPIDEVICE_PNOC_VOTE_ADD,  
   SPIDEVICE_PNOC_VOTE_REMOVE,  
} SpiDevice_PnocVotes;

typedef enum
{
   CS_HIGH,  // chip select is manually forced high
   CS_LOW,   // chip select is manually forced low
   CS_AUTO,  // chip select is automatically controlled by the SPI core
} SpiCSControl;

typedef struct
{
   void    *QupHandle;       // opaque handle used to identify QUP instance
   void    *QupVirtAddr;     // QUP virtual address
   void    *QupPhysAddr;     // QUP physical address

   boolean  InterruptBased;  // TRUE to use interrupts, FALSE to use polling
   boolean  BamSupported;    // TRUE if BAM mode is supported
   boolean  UseCmdDesc;      // TRUE to use command descriptors for register configuration
   uint32   BamThreshold;    // a transfer size greater or equal to this triggers BAM mode

   void    *TcsrVirtAddr;           // set this to NULL if TCSR interrupt is not required
   uint32   TcsrInterruptBitMask;   // TCSR interrupt register bit mask
   uint32   TcsrInterruptBitShift;  // TCSR interrupt register bit shift
} SpiDevice_Config;

typedef struct
{
   SpiDataAddrType *pData;
   uint32           uLen;
} SpiBufferType;

typedef struct
{
   SpiBufferType *pReadBuffVec;
   uint32         uNumReadVecs;
   SpiBufferType *pWriteBuffVec;
   uint32         uNumWriteVecs;
} SpiLinkedTransferType;

typedef struct
{
   void   *pUser;
   uint32  result;
} SpiDeviceBamResult;

typedef struct
{
   uint32  nTransferStatus;
   void   *pArg;
} SpiDevice_AsyncResult;

typedef void (*SPI_DEV_ASYNC_CB)(SpiDevice_AsyncResult *);

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/

/*  @brief Initializes the device.
 *
 *  This Function Initializes the device and creates the
 *  necessary data structures to support other calls into the
 *  device.
 *
 *  @param[in]  pConfig  pointer to device configuration
 *  @param[out] phDev    pointer to device handle
 *  @return              error status
 */
uint32
SpiDevice_Init
(
   SpiDevice_Config *pConfig,
   SPIDEVICE_HANDLE *phDev
);

/*  @brief Deinitializes the device.
 *
 *  This Function Deinitializes the device and releases
 *  resources acquired during init.
 *
 *  @param[in] hDev  device handle
 *  @return          error status
 */
uint32
SpiDevice_DeInit
(
   SPIDEVICE_HANDLE hDev
);

/*  @brief Peform a bi-directional transfer
 *
 *  Read a buffer from SPI device and/or Write a buffer of data to a SPI device
 *
 *  @param[in] hDev          device handle
 *  @param[in] pQupConfig    pointer to the QUP config for data transfer
 *                           client function can set it to NULL if it
 *                           needs to use the settings used by previous
 *                           call with the same device context
 *  @param[in] pReadBuffer   pointer where incoming data will be stored
 *  @param[in] uReadBufLen   number of bytes of valid data to be
 *                           stored in pReadbuffer
 *  @param[in] pWriteBuffer  pointer where outgoing data is stored
 *  @param[in] uWriteBufLen  number of bytes of valid data stored in
 *                           pWritebuffer
 *  @return                  error status
 */
uint32
SpiDevice_WriteRead
(
   SPIDEVICE_HANDLE   hDev,
   SpiDeviceInfoType *pQupConfig,
   SpiDataAddrType   *pReadBuffer,
   uint32             uReadBufLen,
   SpiDataAddrType   *pWriteBuffer,
   uint32             uWriteBufLen
);

/*  @brief Read a buffer from spi device.
 *
 *  @param[in] hDev         device handle
 *  @param[in] pQupConfig   pointer to the QUP config for data transfer
 *  @param[in] pReadBuffer  pointer where incoming data will be stored
 *  @param[in] uReadBufLen  number of bytes of valid data to be
 *                          stored in pReadbuffer
 *  @return                 error status
 */
uint32
SpiDevice_Read
(
   SPIDEVICE_HANDLE   hDev,
   SpiDeviceInfoType *pQupConfig,
   SpiDataAddrType   *pReadBuffer,
   uint32             uReadBufLen
);

/*  @brief Write a buffer to spi device.
 *
 *  @param[in] hDev          device handle
 *  @param[in] pQupConfig    pointer to the QUP config for data transfer
 *  @param[in] pWriteBuffer  pointer where outgoing data is stored
 *  @param[in] uWriteBufLen  number of bytes of valid data stored in
 *                           pWritebuffer
 *  @return                  error status
 */
uint32
SpiDevice_Write
(
   SPIDEVICE_HANDLE   hDev,
   SpiDeviceInfoType *pQupConfig,
   SpiDataAddrType   *pWriteBuffer,
   uint32             uWriteBufLen
);

/*  @brief Peform a bi-direction transfer
 *
 *  Schedules a read/write/read-write(bi-directional) transfer.
 *  Once the transfer is complete or an error occurs
 *  the callback will be called.
 *
 *  @param[in] hDev          device handle
 *  @param[in] pQupConfig    pointer to the QUP config for data transfer
 *  @param[in] pReadBuffer   pointer where incoming data will be stored
 *  @param[in] uReadBufLen   number of bytes of valid data to be
 *                           stored in pReadbuffer
 *  @param[in] pWriteBuffer  pointer where outgoing data is stored
 *  @param[in] uWriteBufLen  number of bytes of valid data stored in
 *                           pWritebuffer
 *  @param[in] pCallbackFn   pointer to a callback function to be called
 *                           when transfer finishes or aborts
 *  @param[in] pArg          pointer to be passed to the callback function
 *  @return                  error status
 */
uint32
SpiDevice_AsyncWriteRead
(
   SPIDEVICE_HANDLE   hDev,
   SpiDeviceInfoType *pQupConfig,
   SpiDataAddrType   *pReadBuffer,
   uint32             uReadBufLen,
   SpiDataAddrType   *pWriteBuffer,
   uint32             uWriteBufLen,
   SPI_DEV_ASYNC_CB   pCallbackFn,
   void              *pArg
);

/*  @brief Schedules a buffer read from spi device.
 *
 *  Schedules a buffer read from spi device.
 *  Once the read is complete or an error occurs
 *  the callback will be called.
 *
 *  @param[in] hDev         device handle
 *  @param[in] pQupConfig   pointer to the QUP config for data transfer
 *  @param[in] pReadBuffer  pointer where incoming data will be stored
 *  @param[in] uReadBufLen  number of bytes of valid data to be
 *                          stored in pReadbuffer
 *  @param[in] pCallbackFn  pointer to a callback function to be called
 *                          when transfer finishes or aborts
 *  @param[in] pArg         pointer to be passed to the callback function
 *  @return                 error status
 */
uint32
SpiDevice_AsyncRead
(
   SPIDEVICE_HANDLE   hDev,
   SpiDeviceInfoType *pQupConfig,
   SpiDataAddrType   *pReadBuffer,
   uint32             uReadBufLen,
   SPI_DEV_ASYNC_CB   pCallbackFn,
   void              *pArg
);

/*  @brief Schedules a buffer write to spi device.
 *
 *  Schedules a buffer write to spi device. Once the write is
 *  complete or an error occurs the callback will be called.
 *
 *  @param[in] hDev          device handle
 *  @param[in] pQupConfig    pointer to the QUP config for data transfer
 *  @param[in] pWriteBuffer  pointer where outgoing data is stored
 *  @param[in] uWriteBufLen  number of bytes of valid data stored in
 *                           pWritebuffer
 *  @param[in] pCallbackFn   pointer to a callback function to be called
 *                           when transfer finishes or aborts
 *  @param[in] pArg          pointer to be passed to the callback function
 *  @return                  error status
 */
uint32
SpiDevice_AsyncWrite
(
   SPIDEVICE_HANDLE   hDev,
   SpiDeviceInfoType *pQupConfig,
   SpiDataAddrType   *pWriteBuffer,
   uint32             uWriteBufLen,
   SPI_DEV_ASYNC_CB   pCallbackFn,
   void              *pArg
);

/*  @brief Does a batch of transfers in a sequence.
 *
 *  @param[in]  hDev              device handle
 *  @param[in]  pTransfer         transfer handle
 *  @param[in]  uNumTransferVecs  number of transfer vectors
 *  @param[in]  pQupConfig        pointer to the QUP config for data transfer
 *  @param[out] puNumCompleted    pointer to return number of completed transfers
 *  @return                       error status
 */
uint32
SpiDevice_BatchTransfer
(
   SPIDEVICE_HANDLE       hDev,
   SpiLinkedTransferType *pTransfer,
   uint32                 uNumTransferVecs,
   SpiDeviceInfoType     *pQupConfig,
   uint32                *puNumCompleted
);

/*  @brief Cancels ongoing transfer.
 *
 *  Cancels ongoing transfer by doing necessary operations related to hw core.
 *
 *  @param[in] hDev  device handle
 *  @return          void
 */
void
SpiDevice_CancelTransfer
(
   SPIDEVICE_HANDLE hDev
);

/*  @brief Set the power state of the device.
 *
 *  @param[in] hDev         device handle
 *  @param[in] ePowerState  desired power state
 *  @return                 error status
 */
uint32
SpiDevice_SetPowerState
(
   SPIDEVICE_HANDLE      hDev,
   SpiDevice_PowerStates ePowerState
);


/*  @brief Set the gpio state of the device.
 *
 *  @param[in] hDev         device handle
 *  @param[in] ePowerState  desired power state
 *  @return                 error status
 */
uint32
SpiDevice_SetGpioState
(
   SPIDEVICE_HANDLE      hDev,
   SpiDevice_GpioStates  eGpioState
);

/*  @brief Set the PNOC voting
 *
 *  @param[in] hDev         device handle
 *  @param[in] ePnocVote    Add/Remove Vote
 *  @return                 error status
 */
uint32 
SpiDevice_SetPNOCVote
(
   SPIDEVICE_HANDLE hDev, 
   SpiDevice_PnocVotes ePnocVote
); 

/*  @brief Gets the HW device information.
 *
 *  @param[in]  hDev     device handle
 *  @param[out] spiInfo  HW info
 *  @return              error status
 */
uint32
SpiDevice_GetHwInfo
(
   SPIDEVICE_HANDLE  hDev,
   SpiInfoType      *spiInfo
);

/*  @brief QUP ISR (interrupt service routine)
 *
 *  This function should be called in interrupt context when a QUP interrupt
 *  is signaled.  It is used for block mode only.  It is responsible for moving
 *  data to/from the HW FIFOs and signaling when the transfer completes.
 *
 *  @param[in] pdata  pointer to data
 *  @return           0
 */
uint32
SpiDevice_IsrHook
(
   void *pdata
);

/*  @brief QUP IST (interrupt service thread)
 *
 *  This function was used when interrupt servicing was split into two parts
 *  (one part to clear the interrupt and one part to read/write the FIFOs).
 *  It is currently not used.
 *
 *  @param[in] pdata  pointer to data
 *  @return           0
 */
uint32
SpiDevice_IstHook
(
   void *pdata
);

/*  @brief BAM completion callback routine
 *
 *  This callback function is used to signal completion of a BAM transfer.
 *
 *  @param[in] pResult  transfer result
 *  @return             void
 */
void
SpiDevice_BamCallback
(
   SpiDeviceBamResult *pResult
);

/*  @brief Configures the SPI chip select (manual or auto).
 *
 *  @param[in] hDev         device handle
 *  @param[in] uChipSelect  chip select line (0 - 3)
 *  @param[in] eControl     selected control method (high/low/auto)
 *  @return                 error status
 */
uint32
SpiDevice_SetCSControl
(
   SPIDEVICE_HANDLE hDev,
   uint32           uChipSelect,
   SpiCSControl     eControl
);

#endif  // _SPIDEVICE_H_
