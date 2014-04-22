/** @file InterruptGIC.c
   
  Qualcomm implmementation of Generic Interrupt Controller.

  Copyright (c) 2010-2011,2013 - 2015,2019 by Qualcomm Technologies, Inc. All Rights Reserved.
  Portions Copyright (c) 2008-2009 Apple Inc. All rights reserved.<BR>
  All rights reserved. This program and the accompanying materials 
  are licensed and made available under the terms and conditions of the BSD License 
  which accompanies this distribution. The full text of the license may be found at 
  http://opensource.org/licenses/bsd-license.php 
  
  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS, 
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED. 

**/


/*===========================================================================

                             Edit History


when       who     what, where, why
--------   ---     --------------------------------------------------------
11/27/18   pran      Adding the initial support.

===========================================================================*/

#include "com_dtypes.h"
#include "InterruptBoot.h"
#include <Library/HALqgic.h>



/*
*  Obtains interrupt service routines.
*/
HARDWARE_INTERRUPT_HANDLER  gRegisteredInterruptHandlers[HAL_QGIC_IRQ_MAX];
void *gRegisteredHandlerContext[HAL_QGIC_IRQ_MAX];


void *  gHardwareInterruptHandle = NULL;



/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */
/*Loop in while loop if x is 0.*/
#define ASSERT(x) do{ } while(!(x));



/**
  Shutdown our hardware
  
  InterruptExitBootServices will disable interrupts and turn off the timer and disable interrupts
  after all the event handlers have run.

**/
void InterruptExitBootServices (void )
{
  HAL_qgic_Reset();
}


/**
  Register Handler for the specified interrupt source.

  @param Source   Hardware source of the interrupt
  @param Handler  Callback for interrupt. NULL to unregister

  @retval RETURN_SUCCESS Source was updated to support Handler.
  @retval RETURN_INVALID_PARAMETER/RETURN_ALREADY_STARTED  Hardware could not be programmed.

**/
uint32 RegisterInterruptSource (uint32 Source, HARDWARE_INTERRUPT_HANDLER Handler , void *context)
{

  ASSERT(Source < HAL_QGIC_IRQ_MAX);

  if ((Handler == NULL) && (gRegisteredInterruptHandlers[Source] == NULL)) {
    return RETURN_INVALID_PARAMETER;
  }

  if ((Handler != NULL) && (gRegisteredInterruptHandlers[Source] != NULL)) {
    return RETURN_ALREADY_STARTED;
  }

  gRegisteredInterruptHandlers[Source] = Handler;
  gRegisteredHandlerContext[Source] = context;
  HAL_qgic_SetTargets(Source,1);
  HAL_qgic_Enable(Source);

  return RETURN_SUCCESS;
}


/**
  Enable interrupt source Source.

  @param Source   Hardware source of the interrupt

  @retval RETURN_SUCCESS       Source interrupt enabled.

**/
uint32 EnableInterruptSource ( uint32  Source)
{
  /*
   * Valid QGIC IRQ
   */
  ASSERT(Source < HAL_QGIC_IRQ_MAX);
  /*
   * Enable QGIC IRQ
   */
  HAL_qgic_Enable(Source);
  
  return RETURN_SUCCESS;
}


/**
  Disable interrupt source Source.

  @param Source   Hardware source of the interrupt

  @retval RETURN_SUCCESS       Source interrupt disabled.

**/
uint32 DisableInterruptSource ( uint32 Source)
{
  /*
   * Valid QGIC IRQ
   */
  ASSERT(Source < HAL_QGIC_IRQ_MAX);
  /*
   * Disable QGIC IRQ
   */
  HAL_qgic_Disable(Source);
  
  return RETURN_SUCCESS;
}


/**
  Set interrupt source Source Configuration.

  @param Source   Hardware source of the interrupt
  @param Config   Interrupt Configuration

  @retval RETURN_SUCCESS       Source interrupt configured.

**/
uint32 SetInterruptSourceConfig ( uint32 Source, uint32 Config)
{
  HAL_qgic_TriggerType trigger;

  /*
   * Valid QGIC IRQ
  */
  ASSERT(Source < HAL_QGIC_IRQ_MAX);

  if (Config == INTERRUPT_SOURCE_CONFIG_TRIGGER_LEVEL)
  {
    trigger = HAL_QGIC_TRIGGER_LEVEL;
  }
  else
  {
    trigger = HAL_QGIC_TRIGGER_EDGE;
  }

  HAL_qgic_SetTrigger(Source, trigger);

  return RETURN_SUCCESS;
}


/**
  Return current state of interrupt source Source.

  @param Source   Hardware source of the interrupt
  @param InterruptState  TRUE: source enabled, FALSE: source disabled.

  @retval RETURN_SUCCESS       InterruptState is valid
  @retval RETURN_INVALID_PARAMETER  Invalid argument

**/
uint32 GetInterruptSourceState(uint32 Source, boolean  *InterruptState)
{

  /*
   * Valid QGIC IRQ
   */
  ASSERT(Source < HAL_QGIC_IRQ_MAX);
  if(!InterruptState)
  {
    return RETURN_INVALID_PARAMETER;
  }

  *InterruptState = HAL_qgic_IsPending(Source); 
  return RETURN_SUCCESS;
}




/**
  IrqInterruptHandler that is called when a processor interrupt occurs.

  @param  InterruptType    Defines the type of interrupt or exception that
                           occurred on the processor.This parameter 
                           is processor architecture specific.

  @return None

**/
void IrqInterruptHandler (uint32 InterruptType)
{
  HAL_qgic_InterruptType intId;
  HAL_qgic_CPUIdType     CPUId;
  HARDWARE_INTERRUPT_HANDLER InterruptHandler;
  void *systemcontext;

  HAL_qgic_AckInterrupt(&intId, &CPUId);

  /*
  * we're only supporting CPU0
  */
  ASSERT(CPUId == 0)

  ASSERT(intId < HAL_QGIC_IRQ_MAX);// Bad Interrupt will need to be removed once we root cause the problem with spurious interrupts

  InterruptHandler = gRegisteredInterruptHandlers[intId];
  systemcontext = gRegisteredHandlerContext[intId];
  if (InterruptHandler != NULL) {
    /*
    * Call the registered interrupt handler.
    */
    InterruptHandler (intId,systemcontext);
  } else {
    //DEBUG 
  }
  /*
  *  Clear after running the handler
  */
  HAL_qgic_EndOfInterrupt (intId, CPUId);


}


/**
  Initialize the state information for the Hardware interrupt Protocol
  and also setup for GIC specific hardware.


  @retval RETURN_SUCCESS    Succcess

**/
uint32 InterruptInitialize(void)
{
  uint32     Status = RETURN_SUCCESS;
  HAL_qgic_BaseAddressType qgicBaseAddrs;


  /*
   * Setup HAL QGIC
   */
  qgicBaseAddrs.nGICDBaseAddress = PcdGet32(PcdInterruptBaseAddress);
  qgicBaseAddrs.nGICCBaseAddress = PcdGet32(PcdQGICCBaseAddress);
  
  HAL_qgic_Init(&qgicBaseAddrs);

  /*
  * Reset QGIC Interrupt HW
  */
  HAL_qgic_Reset();
  
  /*
  * No Preemption
  */
  HAL_qgic_SetBinaryPoint(HAL_QGIC_BINARYPOINT_NO_PREEMPTION);

  /*
  * Set lowest priority level to 254
  */
  HAL_qgic_SetPriorityMask(0xFE);

  /*
  * Enable Distributor
  */
  HAL_qgic_EnableDistributor();

  /*
  * Enable Interrupts
  */
  HAL_qgic_EnableInterrupts();

  return Status;
} /* InterruptInitialize */

/**
  InterruptGicEnable that is called when to enable the GIC level interrupt

  @param  None

  @return None

**/
void InterruptGicEnable(void)
{
   HAL_qgic_EnableInterrupts();
}
/**
  InterruptGicDisable that is called when to disable the GIC level interrupt

  @param  None

  @return None

**/
void InterruptGicDisable(void )
{
   HAL_qgic_DisableInterrupts();

}

