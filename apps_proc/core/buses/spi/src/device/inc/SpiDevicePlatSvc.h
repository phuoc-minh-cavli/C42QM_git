#ifndef _SPIDEVICEPLATSVC_H_
#define _SPIDEVICEPLATSVC_H_
/*==================================================================================================

FILE: SpiDevicePlatSvc.h

DESCRIPTION:
   This file defines an API for providing platform services to the SPI common device layer.

====================================================================================================

   Edit History

When      Who  What, where, why
--------  ---  ------------------------------------------------------------
09/20/16  dpk  Ported for ThreadX
03/10/16  dpk  Added PCNOC Vote/Devote in SPI Open/Close path
12/07/15  sg   Added GPIO Low power mode support in SPI close path
10/03/13  vmk  BAM CMD descriptor usage is determined from properties
07/14/12  ag   Fixed EA specific warnings.
05/30/12  ddk  Added APIs to set/get core clock.
04/19/12  ddk  Added APIs to get core physical and virtual address.
03/28/12  ddk  Added QUP physical address info for BAM cmd desc creation.
03/27/12  ag   Added SPI BAM mode updates.
09/26/11  ag   Created

====================================================================================================
                     Copyright (c) 2011-2016 Qualcomm Technologies Incorporated
                                        All Rights Reserved
                                     Qualcomm Proprietary/GTDR
==================================================================================================*/

#include "SpiDeviceTypes.h"
#include "SpiDeviceError.h"
#include "bam.h"

/*==================================================================================================
                                              CONSTANTS
==================================================================================================*/

#define PLAT_BAM_DESC_FLAG_INT        BAM_IOVEC_FLAG_INT  /* Generate interrupt */
#define PLAT_BAM_DESC_FLAG_EOT        BAM_IOVEC_FLAG_EOT  /* Generate end-of-transfer indication */
#define PLAT_BAM_DESC_FLAG_EOB        BAM_IOVEC_FLAG_EOB  /* Generate end-of-block indication */
#define PLAT_BAM_DESC_FLAG_NWD        BAM_IOVEC_FLAG_NWD  /* Generate Notify when done indication */
#define PLAT_BAM_DESC_FLAG_CMD        BAM_IOVEC_FLAG_CMD  /* Mark as command descriptors */
#define PLAT_BAM_DESC_FLAG_NO_SUBMIT  BAM_IOVEC_FLAG_NO_SUBMIT

/*==================================================================================================
                                              TYPEDEFS
==================================================================================================*/

typedef enum
{
   IBUSPLAT_ERROR_BASE = SPIDEVICE_RES_ERROR_CLS_PLATFORM,
   IBUSPLAT_ERROR_INIT,
   IBUSPLAT_ERROR_DEINIT,
   IBUSPLAT_ERROR_MEM_ALLOCATE,
   IBUSPLAT_ERROR_NULLPTR,
   IBUSPLAT_ERROR_RESET,
   IBUSPLAT_ERROR_TRANSFER_REQUEST,
   IBUSPLAT_ERROR_BAM_TRANSACTION,
   IBUSPLAT_ERROR_CMD_ELEMENT_CREATION_FAILED,
} IBUSPLAT_ERROR;

typedef void* IBUSPLAT_HANDLE;

typedef struct
{
   void   *pUser;
   uint32  result;
} SPIDEVICE_BAM_RESULT;

typedef void (*SPIDEVICE_BAM_CALLBACK)(SPIDEVICE_BAM_RESULT *);

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/

/*  @brief Initializes data structures necessary to support the target calls.
 *
 *  @param[in]  QupHandle  QUP handle
 *  @param[out] phPlat     pointer to platform device handle
 *  @return                error status
 */
uint32 IBUSPLAT_InitTarget(void *QupHandle,
                           IBUSPLAT_HANDLE *phPlat);

/*  @brief Releases data structures necessary to support the target calls.
 *
 *  @param[in] hPlat  platform device handle
 *  @return           error status
 */
uint32 IBUSPLAT_DeInitTarget(IBUSPLAT_HANDLE hPlat);

/*  @brief Sets the app clock frequency in Hz.
 *
 *  @param[in]  hPlat              platform device handle
 *  @param[in]  reqFrequencyHz     frequency requested
 *  @param[out] pFinalFrequencyHz  frequency after the request
 *  @return                        error status
 */
uint32 IBUSPLAT_SetAppClkHz(IBUSPLAT_HANDLE hPlat,
                            uint32 reqFrequencyHz,
                            uint32 *pFinalFrequencyHz);

/*  @brief Sets the power state of the device.
 *
 *  @param[in] hPlat  platform device handle
 *  @param[in] state  power state
 *  @return           error status
 */
uint32 IBUSPLAT_SetPowerState(IBUSPLAT_HANDLE hPlat,
                              uint32 state);

/*  @brief Sets the gpio state of the device.
 *
 *  @param[in] hPlat  platform device handle
 *  @param[in] state  gpio state
 *  @return           error status
 */
							  
uint32 IBUSPLAT_SetGpioState(IBUSPLAT_HANDLE hPlat, 
							 uint32 state);
/*  @brief Add/Remove PNOC Voting
 *
 *  @param[in] hPlat  platform device handle
 *  @param[in] state  ADD/REMOVE PNOC Vote
 *  @return           error status
 */
							  
uint32 IBUSPLAT_SetPNOCVote(IBUSPLAT_HANDLE hPlat, 
                            uint32 state);
							 
/*  @brief Reset the BAM hardware.
 *
 *  @param[in] hPlat  platform device handle
 *  @return           error status
 */
uint32 IBUSPLAT_ResetBam(IBUSPLAT_HANDLE hPlat);

/*  @brief Schedule a single BAM transfer.
 *
 *  @param[in] hPlat        platform device handle
 *  @param[in] bWrite       TRUE for write transfers, FALSE for read transfers
 *  @param[in] pAddr        physical start address for the transfer
 *  @param[in] uSize        size of the transfer in bytes
 *  @param[in] pUser        user-defined pointer associated with the transfer
 *  @param[in] uFlags       flags for the transfer
 *  @param[in] pCallBackFn  callback function for transfer completion
 *  @return                 error status
 */
uint32 IBUSPLAT_BamTransfer(IBUSPLAT_HANDLE         hPlat,
                            boolean                 bWrite,
                            void                   *pAddr,
                            uint32                  uSize,
                            void                   *pUser,
                            uint32                  uFlags,
                            SPIDEVICE_BAM_CALLBACK  pCallBackFn);

#endif  // _SPIDEVICEPLATSVC_H_
