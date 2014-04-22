/*==========================================================================
 
  FILE:       mpmint_internal.c

  OVERVIEW:   This file implements default OS specific functions of MPM driver
              and is expected to be used within MPM driver only.

              Copyright (c) 2011-2014 QUALCOMM Technologies Incorporated
              All Rights Reserved.
              Qualcomm confidential and Proprietary
===========================================================================*/
/*==========================================================================
$Header: //components/rel/core.tx/6.0/power/mpm/src/os/default/mpmint_os.c#1 $
$DateTime: 2020/01/30 22:49:35 $
$Author: pwbldsvc $
===========================================================================*/
#include "mpmint.h"                     /* MPM related files */
#include "mpminti.h"
#include "mpmint_target.h"
#include "DDIInterruptController.h"     /* IC and GPIO related files */
#include "DDITlmm.h"
#include "DDIGPIOInt.h"
#include "DDIHWIO.h"
#include "DalDevice.h"
#include "DALDeviceId.h"
#include "DALStdErr.h"
#include "CoreVerify.h"
#include "msmhwiobase.h"
#include "mpm_src_util.h"

/*=========================================================================
 *                   INTERNAL DEFINITIONS & DECLARATIONS
 *========================================================================*/

/**
 * RPM wakeup interrupt flag (trigger type). The value used here is the
 * most common. If a particular target uses a different value, define it
 * either in build file or target specific header of that target.
 */
#ifndef MPMINT_RPM_WAKEUP_IRQ_FLAG
#define MPMINT_RPM_WAKEUP_IRQ_FLAG DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER
#endif

/**
 * Handle for DAL based Interrupt Controller. This will be used for mapping,
 * registering and triggering interrupts.
 */
static DalDeviceHandle *mpm_dal_ic_handle = NULL;

/**
 * Handle for DAL based GPIO controller. It will be used for mapping and 
 * triggering GPIOs.
 */
static DalDeviceHandle *mpm_dal_gpio_handle = NULL;

/* After driver initialization, this variable will contain the Virtual Address
 * in message ram for master's mpm configuration registers base.
 * It is this variable that is updated or accessed by the mutator & accessor 
 * functions. */ 
static uint32 mpm_cfg_base_addr_va = 0;

/*==========================================================================
 *                      STATIC FUNCTIONS
 *==========================================================================*/

/**
 *  Mutator function for static variable mpm_cfg_base_addr_va 
 */
static void mpm_set_proc_mpm_base_va( uint32 proc_mpm_base_va )
{
  mpm_cfg_base_addr_va =  proc_mpm_base_va;
}

/**
 * @brief Calculate the Processor's MPM RAM Virtual Address from the 
 *        corresponding Physical Address. 
 *
 * @param msg_ram_pa: Physical address in message ram for which we
 *                    will find the virtual address. 
 *
 * @side effect
 * Sets the variable mpm_cfg_base_addr_va with virtual address.
 */
static void mpm_find_proc_va_from_pa( uint32 msg_ram_pa )
{
  uint32 RPM_RAM_start_pa = RPM_SS_MSG_RAM_START_ADDRESS_BASE_PHYS;
  uint32 RPM_RAM_end_va = 0;
  uint32 RPM_RAM_base_size = RPM_SS_MSG_RAM_START_ADDRESS_BASE_SIZE;
  uint32 rpm_proc_mpm_pa_offset;
  uint32 proc_mpm_base_va = 0;
  DalDeviceHandle *mpm_rpm_ram_dal_handle;

  /* After initialization, this variable will contain the Virtual Address 
   * corresponding to the RPM's shared RAM Base Physical Address */
  uint32 rpm_ram_base_va = 0x0;

  /* Calculate the RPM RAM Physical Addr upper boundary */
  if ( (RPM_RAM_start_pa + RPM_RAM_base_size) < RPM_RAM_start_pa )
  {
    CORE_VERIFY(0);
  }

  /* Calculate the physical address offset of master MPM Region
   * relative to the RPM RAM start address */
  rpm_proc_mpm_pa_offset = 
               safe_uint32_subtract( msg_ram_pa,
                                     RPM_RAM_start_pa );
  /* Determine the RPM RAM Base adddress virtual address */
  CORE_DAL_VERIFY(DAL_DeviceAttach(DALDEVICEID_HWIO, &mpm_rpm_ram_dal_handle));
  CORE_DAL_VERIFY( DalHWIO_MapRegion( mpm_rpm_ram_dal_handle, 
                                      "RPM_SS_MSG_RAM_START_ADDRESS", 
                                      (uint8 **) &rpm_ram_base_va ) );

  /* Add Processor's address offset to the RPM RAM base address
   * to get the processor's MPM virtual addr (VA) region start */
  proc_mpm_base_va = safe_uint32_addition( rpm_proc_mpm_pa_offset, 
                                           rpm_ram_base_va);

  /* Calculate the RPM RAM Virtual Addr upper boundary */
  RPM_RAM_end_va = safe_uint32_addition( rpm_ram_base_va,
                                         RPM_RAM_base_size);

  /* Perform bounds check on result.  If processor's virtual address
   * is outside RPM RAM virtual address range,
   * log an error and halt processor */ 
  if ( proc_mpm_base_va <= RPM_RAM_end_va ) 
  {
    mpm_set_proc_mpm_base_va( proc_mpm_base_va );
    mpmint_log_printf( 1,
                      "MPM_SUCCESS: RAM Address within expected addr range: %x",
                      proc_mpm_base_va );
  }
  else
  {
    mpmint_log_printf( 1,
                      "MPM_FAIL: RAM Address out of range error: %x",
                      proc_mpm_base_va );
    CORE_VERIFY(0);
  }
}

/**
 * ISR for rpm to master mpm wakeup interrupt. This function is not doing
 * anything as of now as we trigger interrupts and gpios occured during sleep
 * while coming out of xo shutdown
 */
static void* rpm_to_master_isr( DALISRCtx isr_context )
{
  return NULL;
}

/*=========================================================================
 *                   GLOBAL DEFINITIONS & DECLARATIONS
 *========================================================================*/

/**
 * Number of interrupts master wants MPM to monitor during deep sleep.
 */
uint32 mpmint_num_mapped_irqs = 0;

/**
 * Accessor function for static variable mpm_cfg_base_addr_va 
 */
uint32 mpm_get_proc_mpm_base_va( void )
{
  return mpm_cfg_base_addr_va;
}

/*
 * mpmint_init
 */
void mpmint_init( void )
{
  uint32 mpm_num;
  uint32 gpio_num;
  uint32 mpm_region_start_pa = ( RPM_SS_MSG_RAM_START_ADDRESS_BASE_PHYS +
                                 VMPM_MSG_RAM_OFFSET );
  uint8 *msg_ram_base_addr = NULL;   /* Stores the virtual address for mpm config */

  /* Obtaining virtual address of mpm region from physical address */
  mpm_find_proc_va_from_pa( mpm_region_start_pa );

  msg_ram_base_addr = (uint8 *)mpm_get_proc_mpm_base_va();

  /* Initializing MPM logging */
  mpmint_log_init();

  /* Get the MPM interrupt controller hardware in a good state. */
  HAL_mpmint_Init(NULL, msg_ram_base_addr,FALSE);

  /* Obtaining DAL handle for Interrupt controller */
  CORE_LOG_DAL_VERIFY(DAL_DeviceAttach( DALDEVICEID_INTERRUPTCONTROLLER, &mpm_dal_ic_handle ),
                      mpmint_log_printf( 0, "ERROR: Attaching to Interrupt Controller failed" ) );

  /* Obtaining DAL handle for GPIO controller */
  CORE_LOG_DAL_VERIFY(DAL_DeviceAttach( DALDEVICEID_GPIOINT, &mpm_dal_gpio_handle ),
                      mpmint_log_printf( 0, "ERROR: Attaching to GPIO Controller failed" ) );

  /* Inform the local interrupt controller and GPIO controller of the wakeup 
   * interrupts.  Those subsystems will let us know when any of these 
   * interrupts gets enabled, disabled or reconfigured. */
  for( mpm_num = 0; mpm_num < mpmint_isr_tbl_size; mpm_num++ )
  {
    if( HAL_mpmint_IsGpio( mpmint_isr_tbl[mpm_num].hal_irq, &gpio_num ) )
    {
      /* Inform about the GPIO wakeup. */
      if( DAL_SUCCESS != GPIOInt_MapMPMInterrupt( 
                                      mpm_dal_gpio_handle, gpio_num, 
                                      mpmint_isr_tbl[mpm_num].hal_irq ) )
      {
        mpmint_log_printf( 3, "ERROR: MPM GPIO mapping failed (Handle: 0x%x) "
                           "(GPIO: %u) (MPM ID: %u)", mpm_dal_gpio_handle, 
                           gpio_num, mpmint_isr_tbl[mpm_num].hal_irq );
        CORE_VERIFY(0);
      }
    }
    else if( MPMINT_NULL_IRQ != mpmint_isr_tbl[mpm_num].local_irq )
    {
      /* Inform about the IRQ wakeup. */
      if( DAL_SUCCESS != DalInterruptController_MapWakeupInterrupt( 
                           mpm_dal_ic_handle, 
                           (DALInterruptID)mpmint_isr_tbl[mpm_num].local_irq,
                           mpmint_isr_tbl[mpm_num].hal_irq ) )
      {
        mpmint_log_printf( 3, "ERROR: MPM interrupt mapping failed "
                           "(Handle: 0x%x) (IRQ: %u) (MPM ID: %u)", 
                           mpm_dal_ic_handle, mpmint_isr_tbl[mpm_num].local_irq, 
                           mpmint_isr_tbl[mpm_num].hal_irq );
        CORE_VERIFY(0);
      }

      mpmint_num_mapped_irqs++;
    }
  }

  /* Register with TRAMP for the wakeup interrupt from the RPM. */
  CORE_LOG_DAL_VERIFY( 
    DalInterruptController_RegisterISR( mpm_dal_ic_handle, 
                                        MPMINT_RPM_WAKEUP_IRQ, 
                                        rpm_to_master_isr, 
                                        (DALISRCtx)0, 
                                        MPMINT_RPM_WAKEUP_IRQ_FLAG ),
    mpmint_log_printf( 0, "ERROR: RPM wakeup interrupt registration failed" ) );
}

/*
 * mpmint_retrigger_gpio
 */
void mpmint_retrigger_gpio( uint32 gpio_num )
{
  /* Triggering GPIO on the master */
  if( DAL_SUCCESS != GPIOInt_TriggerInterrupt( mpm_dal_gpio_handle, gpio_num ) )
  {
    mpmint_log_printf( 1, "ERROR: MPM GPIO trigger failed (GPIO: %u)", 
                       gpio_num );
    CORE_VERIFY(0);
  }

  return;
}

/*
 * mpmint_retrigger_interrupt
 */
void mpmint_retrigger_interrupt( uint32 irq_num )
{
  /* Triggering interrupt on the master */
  if( DAL_SUCCESS != DalInterruptController_InterruptTrigger(  
                         mpm_dal_ic_handle, (DALInterruptID)irq_num ) )
  {
    mpmint_log_printf( 1, "ERROR: MPM interrupt trigger failed (IRQ: %u)", 
                       irq_num );
    CORE_VERIFY(0);
  }

  return;
}
