/*===========================================================================
  FILE:         mpmint.c
  
  OVERVIEW:     This file contains the necessary functions to interface
                the MPM IC hardware registers.

                Copyright (c) 2005 - 2016 QUALCOMM Technologies, Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary

  $Header: //components/rel/core.tx/6.0/power/mpm/src/mpmint.c#1 $
===========================================================================*/

#include "mpmint.h"
#include "mpminti.h"
#include "HALmpmint.h"
#include "CoreVerify.h"
#include "CoreIntrinsics.h"

/*============================================================================
 *                             DATA DECLARATION
 *===========================================================================*/

/**
 * Table mapping the external enumeration representation of interrupts to the 
 * HAL's itnernal enumeration values
 */
extern mpmint_irq_data_type mpmint_isr_tbl[];

/**
 * Number of entries in mapping table.
 */
extern const uint32 mpmint_isr_tbl_size;

/**
 * Number of interrupts master wants MPM to monitor during deep sleep.
 */
extern uint32 mpmint_num_mapped_irqs;

/*============================================================================
 *                      INTERNAL FUNCTION DECLARATIONS
 *===========================================================================*/

/**
 * Converts detection type and polarity to equivalent HAL trigger type.
 *
 * @param detect: Detection type for the interrupt or gpio (edge/level).
 * @param polarity: Polarity of the interrupt or gpio (positive/negative).
 *
 * @return HAL trigger type equivalent for input detection and polarity values.
 */
static HAL_mpmint_TriggerType mpmint_trans_int_type
(
  mpmint_detect_type   detect, 
  mpmint_polarity_type polarity
)
{
  HAL_mpmint_TriggerType hal_trigger = HAL_MPMINT_TRIGGER_INVALID;

  if ( ( detect   == MPMINT_LEVEL_DETECT ) && 
       ( polarity == MPMINT_ACTIVE_LOW ) )
  {
    hal_trigger = HAL_MPMINT_TRIGGER_LOW;
  }
  else if ( ( detect   == MPMINT_LEVEL_DETECT ) && 
            ( polarity == MPMINT_ACTIVE_HIGH ) ) 
  {
    hal_trigger = HAL_MPMINT_TRIGGER_HIGH;
  }
  else if ( ( detect   == MPMINT_EDGE_DETECT ) && 
            ( polarity == MPMINT_ACTIVE_LOW ) ) 
  {
    hal_trigger = HAL_MPMINT_TRIGGER_FALLING;
  }
  else if ( ( detect   == MPMINT_EDGE_DETECT ) && 
            ( polarity == MPMINT_ACTIVE_HIGH ) ) 
  {
    hal_trigger = HAL_MPMINT_TRIGGER_RISING;
  }
  else if ( ( detect   == MPMINT_EDGE_DETECT ) && 
            ( polarity == MPMINT_ACTIVE_DUAL ) )
  {
    hal_trigger = HAL_MPMINT_TRIGGER_DUAL;
  }
  else 
  {
    mpmint_log_printf(2, "Invalid detect and/or polarity: %d %d", 
                      (int)detect, (int)polarity);
    CORE_VERIFY(0);
  }

  return hal_trigger;

}

/*============================================================================
 *                      EXTERNAL FUNCTION DECLARATIONS
 *===========================================================================*/
/**
 * @brief A function that loops through all the latched MPM interrupts and
 *        performs an associated soft-trigger operation so that it looks like
 *        the interrupt was caught in the normal mechanism, rather than by a
 *        secondary interrupt controller during deep sleep.
 *
 */
void mpmint_trigger_interrupts( void )
{
  uint32 num_masks, status, temp, phys_num, hal_num;
  uint32 gpio_num, i;
  uint32 isr_tbl_idx;        /* Index into mpmint_isr_tbl[] */

  HAL_mpmint_GetNumberMasks(&num_masks);

  for(i = 0; i < num_masks; i++)
  {
    HAL_mpmint_GetPending(i, &status);

    temp = status;
    while(temp)
    {
      /* Get the next interrupt's physical id and remove it for the future */
      phys_num = (31 - CoreCountLeadingZeros32(temp)) + (32 * i);
      temp &= ~(1 << (31 - CoreCountLeadingZeros32(temp)));

      /* try to figure out which interrupt it is */
      if(!HAL_mpmint_GetEnumNumber(phys_num, &hal_num))
      {
        continue;
      }

      /* Checking validity of interrupt or gpio */
      if( TRUE == mpmint_get_isr_tbl_index((mpmint_isr_type)hal_num, 
                                           &isr_tbl_idx) )
      {
        if( mpmint_isr_tbl[isr_tbl_idx].hal_irq == hal_num )
        {
          /* If it's a gpio, perform soft-trigger into that driver */
          if(HAL_mpmint_IsGpio(hal_num, &gpio_num))
          {
            mpmint_log_printf( 4, "Interrupt soft trigger "
                               "(type: \"GPIO\") (num: %u) (bit: %u) "
                               "(pending mask num: %u) (pending mask: 0x%x)",
                               gpio_num, phys_num, i, status );
            mpmint_retrigger_gpio( gpio_num );
          }
          /* Otherwise try to get a standard irq id for this interrupt */
          else if( MPMINT_NULL_IRQ != 
                   mpmint_isr_tbl[isr_tbl_idx].local_irq )
          {
            mpmint_log_printf( 4, "Interrupt soft trigger "
                               "(type: \"IRQ\") (num: %u) (bit: %u) "
                               "(pending mask num: %u) (pending mask: 0x%x)",
                               mpmint_isr_tbl[isr_tbl_idx].local_irq,
                               phys_num, i, status );

            mpmint_retrigger_interrupt( mpmint_isr_tbl[isr_tbl_idx].local_irq );
          }
        }
      }  /* Validity */
    }    /* temp */
  }

  /* Clearing all interrupts for next cycle */
  HAL_mpmint_All_Clear();
}

/*
 * mpmint_get_num_mapped_interrupts
 */
uint32 mpmint_get_num_mapped_interrupts()
{
  return mpmint_num_mapped_irqs;
}

/*
 * mpmint_map_interrupts_with_master
 */
uint32 mpmint_map_interrupts_with_master
(
  mpmint_config_info_type *intrs,
  uint32 intrs_count
)
{
  uint32 mpm_num, irqs_mapped = 0;
  uint32 gpio_num;

  if( intrs_count != mpmint_num_mapped_irqs )
  {
    /* Calling function does not expect same number of mapped interrupts as 
     * present in mpmint table. Log a message and return 0 indicating that
     * no interrupts were mapped. It is up to caller to handle that. */
    mpmint_log_printf( 2, "WARNING: Interrupt mapping count mismatch "
                       "(expected: %u) (requested: %u)", 
                       mpmint_num_mapped_irqs, intrs_count );
    return 0;
  }

  for( mpm_num = 0; mpm_num < mpmint_isr_tbl_size; mpm_num++ )
  {
    if( !HAL_mpmint_IsGpio( mpmint_isr_tbl[mpm_num].hal_irq, &gpio_num ) &&
        MPMINT_NULL_IRQ  != mpmint_isr_tbl[mpm_num].local_irq )
    {
      intrs[irqs_mapped].mpm_hw_int_id = mpmint_isr_tbl[mpm_num].hal_irq;
      intrs[irqs_mapped].master_hw_int_id = mpmint_isr_tbl[mpm_num].local_irq;

      irqs_mapped++;
    }
  }

  mpmint_log_printf( 1, "Mapped mpm interrupts with master (IRQs Mapped: %u)",
                     irqs_mapped );

  return irqs_mapped;
}

/*
 * mpmint_config_int
 */
void mpmint_config_int
(
  mpmint_isr_type      int_num,     
  mpmint_detect_type   detection,
  mpmint_polarity_type polarity
)
{
  uint32 isr_tbl_idx; /* Index into mpmint_isr_tbl[] */
  unsigned long interrupt_flag = 0;

  if ( ( TRUE == mpmint_get_isr_tbl_index(int_num, &isr_tbl_idx) ) &&
       ( MPMINT_NULL_IRQ != mpmint_isr_tbl[isr_tbl_idx].local_irq ) &&
       ( HAL_mpmint_IsSupported(mpmint_isr_tbl[isr_tbl_idx].hal_irq) ) )
  {
    /*mpmint_log_printf( 4, "Configuring interrupt (Local IRQ Id: %u) "
                       "(MPM ID: %u) (Detection: %d) (Polarity: %d)", 
                       mpmint_isr_tbl[isr_tbl_idx].local_irq, 
                       mpmint_isr_tbl[isr_tbl_idx].hal_irq, 
                       (int)detection, (int)polarity);*/
 
    VMPM_MSG_RAM_LOCK( interrupt_flag );
    HAL_mpmint_SetTrigger(mpmint_isr_tbl[isr_tbl_idx].hal_irq, 
                          mpmint_trans_int_type(detection, polarity));
    VMPM_MSG_RAM_UNLOCK( interrupt_flag );
  }
  else
  {
    /*mpmint_log_printf( 1, "WARNING: Invalid IRQ config request (IRQ id: %u)",
                       (uint32)int_num );*/
  }
}

/*
 * mpmint_config_wakeup
 */
void mpmint_config_wakeup
(
  mpmint_isr_type       int_num,
  mpmint_processor_type processor
)
{
  uint32 isr_tbl_idx; /* Index into mpmint_isr_tbl[] */
  unsigned long interrupt_flag = 0;

  if ( ( TRUE == mpmint_get_isr_tbl_index(int_num, &isr_tbl_idx) ) &&
       ( MPMINT_NULL_IRQ != mpmint_isr_tbl[isr_tbl_idx].local_irq ) &&
       ( HAL_mpmint_IsSupported(mpmint_isr_tbl[isr_tbl_idx].hal_irq) ) )
  {
    /*mpmint_log_printf( 2, "Enabling interrupt (Local IRQ Id: %u) (MPM Id: %u)",
                       mpmint_isr_tbl[isr_tbl_idx].local_irq,
                       mpmint_isr_tbl[isr_tbl_idx].hal_irq );*/

    VMPM_MSG_RAM_LOCK( interrupt_flag );
    HAL_mpmint_Enable(mpmint_isr_tbl[isr_tbl_idx].hal_irq);
    VMPM_MSG_RAM_UNLOCK( interrupt_flag );
  }
  else
  {
    /*mpmint_log_printf( 1, "WARNING: Invalid IRQ enable request (IRQ id: %u)",
                       (uint32)int_num );*/
  }
}

/*
 * mpmint_disable_wakeup
 */
void mpmint_disable_wakeup(mpmint_isr_type int_num)
{
  uint32 isr_tbl_idx; /* Index into mpmint_isr_tbl[] */
  unsigned long interrupt_flag = 0;
  
  if ( ( TRUE == mpmint_get_isr_tbl_index(int_num, &isr_tbl_idx) ) &&
       ( MPMINT_NULL_IRQ != mpmint_isr_tbl[isr_tbl_idx].local_irq ) &&
       ( HAL_mpmint_IsSupported(mpmint_isr_tbl[isr_tbl_idx].hal_irq) ) )
  {
    /*mpmint_log_printf( 2, "Disabling interrupt (Local IRQ Id: %u) (MPM Id: %u)",
                       mpmint_isr_tbl[isr_tbl_idx].local_irq,
                       mpmint_isr_tbl[isr_tbl_idx].hal_irq );*/

    VMPM_MSG_RAM_LOCK( interrupt_flag );
    HAL_mpmint_Disable(mpmint_isr_tbl[isr_tbl_idx].hal_irq);
    VMPM_MSG_RAM_UNLOCK( interrupt_flag );
  }
  else
  {
    /*mpmint_log_printf( 1, "WARNING: Invalid IRQ disable request (IRQ id: %u)",
                       (uint32)int_num );*/
  }
}

/*
 * mpmint_config_gpio_wakeup
 */
void mpmint_config_gpio_wakeup
(
  uint32                which_gpio,
  mpmint_detect_type    detection,
  mpmint_polarity_type  polarity,
  mpmint_processor_type processor
)
{
  uint32 int_num;         
  unsigned long interrupt_flag = 0;  

  if(HAL_mpmint_IsGpioSupported(which_gpio, &int_num))
  {
    mpmint_log_printf( 4, "Enabling GPIO (Local id: %u) (MPM HAL id: %u) "
                       "(Detection: %d) (Polarity: %d)", which_gpio, int_num,
                       (int)detection, (int)polarity);

    VMPM_MSG_RAM_LOCK( interrupt_flag );
    HAL_mpmint_SetTrigger(int_num, mpmint_trans_int_type(detection, polarity));
    HAL_mpmint_Enable(int_num);
    VMPM_MSG_RAM_UNLOCK( interrupt_flag );
  }
  else
  {
    mpmint_log_printf( 1, "WARNING: Invalid GPIO config request (GPIO id: %u)",
                       which_gpio );
  }
}

/*
 * mpmint_disable_gpio_wakeup
 */
void mpmint_disable_gpio_wakeup
(
  uint32 which_gpio
)
{
  uint32 int_num;         
  unsigned long interrupt_flag = 0;  

  if(HAL_mpmint_IsGpioSupported(which_gpio, &int_num))
  {
    mpmint_log_printf( 2, "Disabling GPIO (Local id: %u) (MPM HAL id: %u)",
                       which_gpio, int_num);

    VMPM_MSG_RAM_LOCK( interrupt_flag );
    HAL_mpmint_Disable(int_num);
    VMPM_MSG_RAM_UNLOCK( interrupt_flag );
  }
  else
  {
    mpmint_log_printf( 1, "WARNING: Invalid GPIO disable request (GPIO id: %u)",
                       which_gpio );
  }
}

/*
 * mpmint_setup_interrupts
 */
void mpmint_setup_interrupts
(
  mpmint_config_info_type *intrs,
  uint32 intrs_count
)
{
  uint32 i;
  unsigned long interrupt_flag = 0;

  if( (intrs_count != mpmint_num_mapped_irqs) )
  {
    /* Input array does not contain same number of interrupts than are 
     * actually mapped for this master */
    mpmint_log_printf( 2, "ERROR: Interrupt setup count mismatch "
                       "(expected: %u) (requested: %u)",
                       mpmint_num_mapped_irqs, intrs_count);
    CORE_VERIFY(0);
  }

  if( 0 == mpmint_num_mapped_irqs )
  {
    /* Only GPIOs are mapped to mpm for this target. No need to
     * configure anything. Simply return */
    mpmint_log_printf( 0, "WARNING: No interrupt mapped to MPM" );
    return ;
  }

  /* Checking the validity of input */
  for( i = 0; i < intrs_count; i++ )
  {
    uint32 isr_tbl_idx;

    if ((FALSE == mpmint_get_isr_tbl_index(
        (mpmint_isr_type)intrs[i].mpm_hw_int_id, &isr_tbl_idx)) ||
        (!HAL_mpmint_IsSupported(intrs[i].mpm_hw_int_id)) )
    {
      mpmint_log_printf(2, "ERROR: Unsupported MPM interrupt (id: %u)",
                        intrs[i].mpm_hw_int_id);
      CORE_VERIFY(0);
    }
  }

  VMPM_MSG_RAM_LOCK( interrupt_flag );
  HAL_mpmint_setup_interrupts((HAL_mpmint_ConfigInfoType *)intrs, intrs_count);
  VMPM_MSG_RAM_UNLOCK( interrupt_flag );

}

