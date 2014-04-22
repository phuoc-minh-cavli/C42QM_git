#ifndef _DAL_INTRCTRLR_CONFIG_H
#define _DAL_INTRCTRLR_CONFIG_H
/*==============================================================================

FILE:      DALInterruptControllerConfig.h

DESCRIPTION: This file implements a Interrupt Controller for QGIC hardware
             interrupt controller

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

  Copyright (c) 2019 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary/GTDR

==============================================================================*/
/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/systemdrivers/InterruptController/src/l4_qgic/DALInterruptControllerConfig.h#1 $ 

when       who     what, where, why
--------   ---     ---------------------------------------------------------
07/28/07   an     Initial version. Conversion of Tramp to DALInterruptController

===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "DALInterruptController.h"
#include "HALqgic.h"

/*=========================================================================

                           CONSTANTS

===========================================================================*/


/*
 * Starting priority of non-secure interrupts.
 */
#define INTERRUPT_START_PRIORITY 128

/*
 * Median priority of an incoming interrupt.
 */
#define INTERRUPT_MEDIAN_PRIORITY 32

/*
 * Highest priority IRQ of an incoming interrupt.
 */
#define INTERRUPT_HIGHEST_PRIORITY 8

/*
 * Highest priority FIQ of an incoming interrupt.
 */
#define INTERRUPT_FIQ_PRIORITY 0


 /*
  *  Invalid interrupt value for error checking
  */
#define INVALID_INTERRUPT                0x7FFFFFFF


#define INTERRUPT_TRUE 1
#define INTERRUPT_FALSE 0


#define INTERRUPTF_MPM           0x00000001
#define INTERRUPTF_CLEARBEFORE   0x00000002
#define INTERRUPTF_CLEARAFTER    0x00000004
#define INTERRUPTF_REGISTERED    0x00000008
#define INTERRUPTF_RUNNING       0x00000010
#define INTERRUPTF_SUSPENDED     0x00000020
#define INTERRUPTF_FAKETRIGGER   0x00000040

/*=========================================================================

                           TYPE DEFINITIONS

===========================================================================*/

typedef void (*DALISR_HandlerType) (uint32 param);

/*
 * InterruptStateType
 *
 * Data structure containing all information about an interrupt state.
 *
 *  Isr:          Interrupt service routine to invoke.
 *  nParam:       Parameter to pass the ISR.
 *  nTrigger      The interrupt trigger type.  
 *  flags:        Interrupt flags.
 *  hEvent:       Interrupt Event to trigger when the interrupt fires.
 *  nMPMID:       MAO interrupt ID if this is a wakeup source for the MPM
 *                hardware.  Only used if TRAMPF_MAOINT is set.
 *  nCount:       count of how many times the interrupt fired.  
 *  nLastFired:   The time stamp of the last time the interrupt fired.
 */
typedef struct
{
  uint64                    nLastFired;
  #ifdef DALINTERRUPT_MPM_WAKEUP
  uint32                    nMPMID;
  #endif
  uint32                    nCount;
  uint32                    nPriority;
  DALISR                    Isr;
  DALISRCtx                 nParam;
  char                     *pInterruptName;
  uint8                     nTrigger;
  uint8                     nFlags;
} InterruptStateType;

/*
 * InterruptConfigType
 */
typedef struct
{
  uint32                     nQGICNumber;
  uint8                      nPriority;
  char                      *pInterruptName;
} InterruptConfigType;


/*
 * InterruptQgicModeType
 *
 * The mode tramp should configure the QGIC for.
 *
 * TRUSTZONE - Processor supports trustzone so FIQ will be used for secure
 *             interrupts, and the non-secure mode will run tramp as well.
 * LEGACY    - Only secure mode exists and will configure both secure and
 *             non-secure portions of the interrupt controller.
 */
typedef enum
{
  QGIC_MODE_TRUSTZONE,
  QGIC_MODE_LEGACY
} QgicModeType;

/*
 * Interrupt Controller base address type.
 *
 * Structure containing qgic register base addresses these addresses are then 
 * used to get the virtual addresses in Interrupt_DeviceInit via HWIO dal
 * pQGICBaseAddr            - QGIC base address
 * nQGICDistributorOffset   - Distributor register definition base
 * nQGICCPUOffset           - CPU interface Register definition base offset
 * nHypervisorOffset        - Hypervisor base address offset
 * nVirtualizationCPUOffset - Virtualization cpu base offset
 */
typedef struct
{
  uint8  *pQGICBaseAddr;
  uint32  nQGICDistributorOffset;
  uint32  nQGICCPUOffset;
  uint32  nHypervisorOffset;
  uint32  nVirtualizationCPUOffset;
}InterruptBaseAddressType;


 /*
  * InterruptPlatformDataType
  * This structure holds the platform build time properties of the interrupt controller.
  * eMode                  - Mode of operation for this processor.
  * nMaxIRQ                - Number of supported interrupts.
  * pIRQConfigs            - Pointer to an array of interrupt definitions in the BSP.
  * psQgicAddr             - pointer Qgic Base address data type.   
  */
typedef struct
{
  QgicModeType               eMode;
  uint32                     nMaxIRQ;
  InterruptConfigType       *pIRQConfigs;
  InterruptBaseAddressType  *psQgicAddr;
} InterruptPlatformDataType;

 /*
  * InterruptDataType
  * This structure holds all the run time properties of the interrupt controller.
  * pHWIOHandle            - Handle to the HWIO driver module.
  * pTimetickHandle        - Handle to the Timetick driver module.
  * pClientCtxt            - Handle to the client context.
  * pInterruptState        - Pointer to the array keeping track of the interrupt state.
  * pPlatformConfig        - Pointer to the platform configuration.
  */
typedef struct
{
  DalDeviceHandle               *pHWIOHandle;
  DalDeviceHandle               *pTimetickHandle;
  InterruptControllerClientCtxt *pClientCtxt;
  InterruptStateType            *pInterruptState;
  InterruptPlatformDataType     *pPlatformConfig;
} InterruptDataType;

#endif //_DAL_INTRCTRLR_CONFIG_H
