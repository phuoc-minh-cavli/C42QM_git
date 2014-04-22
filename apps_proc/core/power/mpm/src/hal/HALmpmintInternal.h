#ifndef HAL_MPMINT_INTERNAL_H
#define HAL_MPMINT_INTERNAL_H

/*
===========================================================================

FILE:         HALmpmintInternal.h

DESCRIPTION:  
  This is the hardware abstraction layer internal interface for the MPM
  interrupt controller block.

===========================================================================

                             Edit History

$Header: //components/rel/core.tx/6.0/power/mpm/src/hal/HALmpmintInternal.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
08/18/11   nbg     General code clean up based on earlier code review.
08/06/09   ajf     Rename mao --> mpm.
06/30/09   taw     Added a field for the TLMM pad enable register.
01/09/09   taw     Added HAL_maoint_PlatformIntType, which contains the
                   properties for each interrupt.
03/20/08   kr      Created.
05/27/08   kr      Modifications based on code review

===========================================================================
             Copyright © 2008-2015 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include "HALmpmint.h"

/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */

/*
 * HAL_mpmint_PlatformType
 *
 * Platform (asic/processor) specific information filled in by the platform
 * code in HAL_pic_PlatformEarlyInit.
 *
 *  WakeupReg     : The base of the interrupt enable register array.
 *  nEnableReg    : The base of the interrupt enable register array.
 *  nDetectReg    : The base of the interrupt detect ctl register array.
 *  nDetect3Reg   : The base of the interrupt detect ctl register array.
 *  nPolarityReg  : The base of the interrupt polarity register array.
 *  nStatusReg    : The base of the interrupt status register array.
 *  nEnableRegPad : The base of TLMM's wakeup interrupt enable register array.
 *  nClearReg     : The base of the interrupt clear register array.
 */
typedef struct
{
  uint32 WakeupReg;
  uint32 nEnableReg;
  uint32 nDetectReg;
  uint32 nDetect3Reg;
  uint32 nPolarityReg;
  uint32 nStatusReg;
  uint32 nEnableRegPad;
  uint32 nClearReg;
} HAL_mpmint_PlatformType;

/*
 * HAL_mpmint_PlatformIntType
 *
 * The description of an mpm interrupt.
 *
 *  eTrigger:      trigger type (edge, level, positive, negative)
 *  gpio:          which GPIO this interrupt is associated with
 *  nPadMask:      mask to use for this interrupt in the TLMM pad enables
 */
typedef struct
{
  HAL_mpmint_TriggerType   eTrigger;
  uint8                    gpio;
  uint8                    padBitPos;
} HAL_mpmint_PlatformIntType;


/* -----------------------------------------------------------------------
**                         DECLARATIONS
** ----------------------------------------------------------------------- */

extern HAL_mpmint_PlatformType    registerLayout;
extern HAL_mpmint_PlatformIntType aInterruptTable[];

/**
 * Intializes the register layour for writing to shared memory. Its 
 * implementation will vary based on OS. For Apps (QNX or EA) we will
 * need virtual address for the base of shared memory address for mpm.
 *
 * @param master_msg_ram_base_ptr: Address of the memory region for this
 *                                 master reserved in shared messsage ram.
 *                                 It is usually provided by RPM team. The
 *                                 address can be physical or virtual 
 *                                 depending on target.
 */
extern void HAL_mpmint_InitializeRegisterLayout
(
  uint8 *master_msg_ram_base_ptr
);

/**
 * Maps the APSS base physical address to virtual address so that 
 * HWIO macros can be used.
 *
 * @Note:
 * This function must be called before any HWIO operation. It can
 * be during early init stage for vmpm.
 */
extern void HAL_mpmint_hwio_mmap(void);

/**
 * Prepares mpm driver to perform IPC with RPM afterwards. This is just
 * a setup function and it does not perform any actual IPC. For example,
 * in case where we use DAL IPC, it would be attaching to DAL IPC device.
 * On some target this function will map the IPC registers for the process
 * they are running on.
 */
void HAL_mpmint_SetupIPC(void);

/**
 * Sends an IPC interrupt to RPM whenver vMPM configuration for this
 * master has been changed. This happens when any interrupt/gpio 
 * configuration (detection, polarity) is changed.
 *
 * @Note:
 * This function may have different implementation on different targets.
 * The default implementation is using DAL IPC interrupt interface.
 */
extern void vmpm_send_interrupt(void);

#endif /* HAL_MPMINT_INTERNAL_H */


