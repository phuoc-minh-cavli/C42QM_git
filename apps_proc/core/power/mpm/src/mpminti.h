#ifndef MPMINTI_H
#define MPMINTI_H

/**
@file mpminti.h
 
@brief Internal definitions for MPM interrupt management.

*/

/*-----------------------------------------------------------------------------
             Copyright © 2011-2016 QUALCOMM Technologies, Incorporated.
                 All Rights Reserved.
             QUALCOMM Confidential and Proprietary.
-----------------------------------------------------------------------------*/

/*=============================================================================

$Header: //components/rel/core.tx/6.0/power/mpm/src/mpminti.h#1 $

=============================================================================*/

#include "comdef.h"
#include "HALmpmint.h"
#include "mpmint_target.h"
#include "qurt_interrupt.h"

/*=========================================================================
 *                     INTERNAL OS SPECIFIC DEFINITIONS
 *=========================================================================*/
/* The macros defined below are usually OS depedent. New definitions must be
 * added as support for new OS comes or it will result into default value
 * that may not work as expected on all OSes. */

#ifdef USES_QNX                   /* QNX specific implementation */
  #ifndef QNP_POWERMAN_PROC       /* For QNP CORE process */
    #include "DALSys.h"
   
    extern DALSYSSyncHandle       DalSharedMsgRAMLock;

    #define VMPM_MSG_RAM_LOCK()   DALSYS_SyncEnter(DalSharedMsgRAMLock)  
    #define VMPM_MSG_RAM_UNLOCK() DALSYS_SyncLeave(DalSharedMsgRAMLock)

  #else                           /* For POWERMAN process */

    #define VMPM_MSG_RAM_LOCK()
    #define VMPM_MSG_RAM_UNLOCK()

  #endif

#else

  #define VMPM_MSG_RAM_LOCK(flag)     INTLOCK_SAVE(flag)
  #define VMPM_MSG_RAM_UNLOCK(flag)   INTFREE_RESTORE(flag)

#endif

#define MPMINT_NULL_IRQ         (uint32)(-1)
    
/*===========================================================================
 *                         INTERNAL TYPE DEFINTIONS
 *==========================================================================*/

/**
 * Mapping of external enumeration's representation of interrupts to the
 * HAL's internal enumeration values.
 */
typedef struct
{
  HAL_mpmint_IsrType hal_irq;
  uint32             local_irq;
} mpmint_irq_data_type;

/**
 * Utility Macro for populating a mpmint_irq_data_type struct.
 */
#define MPMINT_DEFINE_IRQ_MAPPING(name, local_irq) \
  {                                \
    HAL_MPMINT_##name##_ISR,       \
    local_irq,                     \
  }

/*===========================================================================
 *                       EXTERNAL VARIABLE DECLARATIONS
 *===========================================================================*/
/**
 * Number of interrupts master wants MPM to monitor during deep sleep.
 */
extern uint32 mpmint_num_mapped_irqs;

/*
 * Table mapping the external enumeration's representation of interrupts to the
 * HAL's internal enumeration values.  Also, we keep around a little bit of
 * state (number of times an interrupt has been latched, etc.) for debugging.
 */
extern mpmint_irq_data_type mpmint_isr_tbl[];

/**
 * Number of entries in mapping table.
 */
extern const uint32 mpmint_isr_tbl_size;

/*===========================================================================
 *                            FUNCTION PROTOTYPES
 *===========================================================================*/

/**
 * Gets the table index corresponding to in_id for mpmint_isr_tbl[].
 * 
 * @param in_id[in]    Interrupt for which the table index is required.
 * @param out_id[out]  Table index (only valid if return value of this function
 *                     is TRUE).
 * 
 * @return boolean TRUE if valid index was found; FALSE otherwise.
 */
boolean mpmint_get_isr_tbl_index
(
  mpmint_isr_type in_id,
  uint32          *out_id
);

/**
 * Triggers the GPIO passed as argument on the master. This function usually
 * called by mpmint_trigger_interrupts while coming out of sleep if it finds
 * particular GPIO occurred at MPM during sleep.
 *
 * Some of the parameters are only for debugging/logging purpose and it may
 * not be used on all implementations. Only first argument needs to have
 * valid value (However, this validity may or may not be checked based on
 * implementation).
 *
 * @see mpmint_trigger_interrupts
 *
 * @param gpio_num: Actual gpio number that will be triggered.
 */
void mpmint_retrigger_gpio
(
  uint32 gpio_num
);

/**
 * Trigger the interrupt passed as argument on the master. This function 
 * usually called by mpmint_trigger_interrupts while coming out of sleep if
 * it finds particular interrupt occurred at MPM during sleep.
 *
 * Some of the parameters are only for debugging/logging purpose and it may
 * not be used on all implementations. Only first argument needs to have
 * valid value (However, this validity may or may not be checked based on
 * implementation).
 *
 * @see mpmint_trigger_interrupts
 *
 * @param irq_num: IRQ number for the interrupt that will be triggered.
 */
void mpmint_retrigger_interrupt
(
  uint32 irq_num
);

/**
 * Initializes log for mpm. This function must be called before using
 * any log functionalities for mpm.
 */
void mpmint_log_init( void );

/**
 * Logs the mpm driver messages in Ulog. A valid ulog handle should be passed
 * as there will not be any checking in this function. For low memory targets
 * such as sensor, this could be a stub function.
 *
 * @Note
 * mpmint_log_init must be called before calling this function.
 *
 * @param arg_count: Number of parameters that are to be logged
 * @param format: The formate string to print logs. Similar to one used in 
 *                printf statements.
 */
void mpmint_log_printf( int arg_count, const char * format, ... );

#endif /* !MPMINTI_H */

