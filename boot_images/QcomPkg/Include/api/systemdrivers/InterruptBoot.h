#ifndef INTERRUPT_BOOT_H
#define INTERRUPT_BOOT_H

/** @file InterruptBoot.h
 

  Copyright (c) 2010-2011, Qualcomm Technologies Inc. All rights reserved.
  
  Portions Copyright (c) 2008 - 2009, Apple Inc. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
  
**/

/*===========================================================================
                             Edit History


when       who     what, where, why
--------   ---     --------------------------------------------------------
01/11/10   JZ      Created.

===========================================================================*/



/* -----------------------------------------------------------------------
**                           Definition
** ----------------------------------------------------------------------- */



/**
  Interrupt Source configurations
  
  All possible interrupt configurations.
  INTERRUPT_SOURCE_CONFIG_TRIGGER_LEVEL : Level trigger
  INTERRUPT_SOURCE_CONFIG_TRIGGER_LEVEL : Edge trigger
**/
#define INTERRUPT_SOURCE_CONFIG_TRIGGER_LEVEL 0
#define INTERRUPT_SOURCE_CONFIG_TRIGGER_EDGE  1


/**
  Shutdown our hardware
  
  InterruptExitBootServices will disable interrupts and turn off the timer and disable interrupts
  after all the event handlers have run.

  @param[in]  Event   The Event that is being processed
  @param[in]  Context Event Context
**/
void InterruptExitBootServices (void);


/**
  C Interrupt Handler calledin the interrupt context when Source interrupt is active.

  @param Source         Source of the interrupt. Hardware routing off a specific platform defines
                        what source means.
  @param SystemContext  Pointer to system register context. Mostly used by debuggers and will
                        update the system context after the return from the interrupt if
                        modified. Don't change these values unless you know what you are doing

**/
typedef void ( *HARDWARE_INTERRUPT_HANDLER) ( unsigned int Source , void *SystemContext);

/**
  Register Handler for the specified interrupt source.

  @param Source   Hardware source of the interrupt
  @param Handler  Callback for interrupt. NULL to unregister

  @retval RETURN_SUCCESS Source was updated to support Handler.
  @retval RETURN_DEVICE_ERROR  Hardware could not be programmed.

**/
uint32 RegisterInterruptSource (uint32 Source, HARDWARE_INTERRUPT_HANDLER Handler, void *context);


/**
  Enable interrupt source Source.

  @param Source   Hardware source of the interrupt

  @retval RETURN_SUCCESS       Source interrupt enabled.
  @retval RETURN_DEVICE_ERROR  Hardware could not be programmed.

**/
uint32 EnableInterruptSource (uint32 Source);


/**
  Disable interrupt source Source.

  @param Source   Hardware source of the interrupt

  @retval RETURN_SUCCESS       Source interrupt disabled.
  @retval RETURN_DEVICE_ERROR  Hardware could not be programmed.

**/
uint32 DisableInterruptSource( uint32 Source);


/**
  Set interrupt source Source Configuration.

  @param Source   Hardware source of the interrupt
  @param Config   Interrupt Configuration

  @retval RETURN_SUCCESS       Source interrupt configured.
  @retval RETURN_DEVICE_ERROR  Hardware could not be programmed.

**/
uint32 SetInterruptSourceConfig (uint32 Source, uint32 Config);


/**
  Return current state of interrupt source Source.

  @param Source   Hardware source of the interrupt
  @param InterruptState  TRUE: source enabled, FALSE: source disabled.

  @retval RETURN_SUCCESS       InterruptState is valid
  @retval RETURN_DEVICE_ERROR  InterruptState is not valid

**/
uint32 GetInterruptSourceState (uint32 Source, boolean *InterruptState);


/**
  IrqInterruptHandler that is called when a processor interrupt occurs.

  @param  InterruptType    Defines the type of interrupt or exception that
                           occurred on the processor.This parameter 
                           is processor architecture specific.

  @return None

**/
void IrqInterruptHandler (uint32 InterruptType);


/**
  Initialize the state information for the Hardware interrupt Protocol
  and also setup for GIC specific hardware.


  @retval RETURN_SUCCESS    Succcess

**/
uint32 InterruptInitialize(void);


/**
  Signal to the hardware that the End Of Intrrupt state 
  has been reached.

  @param Source   Hardware source of the interrupt

  @retval RETURN_SUCCESS       Source interrupt EOI'ed.
  @retval RETURN_DEVICE_ERROR  Hardware could not be programmed.

**/
uint32 EndOfInterrupt (uint32 Source);

/**
  InterruptGicEnable that is called when to enable the GIC level interrupt

  @param  None

  @return None

**/
void InterruptGicEnable( void );

/**
  InterruptGicDisable that is called when to disable the GIC level interrupt

  @param  None

  @return None

**/
void InterruptGicDisable( void );




#endif /* INTERRUPT_BOOT_H */

