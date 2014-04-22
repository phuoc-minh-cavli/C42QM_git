#ifndef QURT_INTERRUPT_H
#define QURT_INTERRUPT_H

/**
  @file qurt_interrupt.h 
  @brief  Prototypes of QuRT Interrupt handler API 

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2015  by Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================*/

#include "qurt_types.h"
#include "tx_port.h"
#include "tx_api.h"
#include "tx_trace.h"

/*=============================================================================
                                 CONSTANTS AND MACROS
=============================================================================*/ 
#ifdef __thumb 

#define INTLOCK_SAVE(flags) \
do { \
   TX_INTERRUPT_SAVE_AREA \
   TX_DISABLE \
   flags = interrupt_save_disabled; \
   TX_TRACE_IN_LINE_INSERT(TX_TRACE_INTERRUPT_CONTROL, TX_ULONG_TO_POINTER_CONVERT(interrupt_save_disabled), 0, 0x1, 0, TX_TRACE_INTERRUPT_CONTROL_EVENT);	\
}while(0);

#define INTFREE_RESTORE(flags) \
do { \
   TX_INTERRUPT_SAVE_AREA \
   interrupt_save_disabled = (unsigned long)flags; \
   TX_TRACE_IN_LINE_INSERT(TX_TRACE_INTERRUPT_CONTROL, TX_ULONG_TO_POINTER_CONVERT(interrupt_save_disabled), 0, 0x2, 0, TX_TRACE_INTERRUPT_CONTROL_EVENT);	\
   TX_RESTORE \
}while(0);

#else

#define INTLOCK_SAVE(flags) \
do { \
   TX_INTERRUPT_SAVE_AREA \
   TX_DISABLE \
   flags = interrupt_save; \
   TX_TRACE_IN_LINE_INSERT(TX_TRACE_INTERRUPT_CONTROL, TX_ULONG_TO_POINTER_CONVERT(interrupt_save), 0, 0x1, 0, TX_TRACE_INTERRUPT_CONTROL_EVENT);	\
}while(0);

#define INTFREE_RESTORE(flags) \
do { \
  TX_INTERRUPT_SAVE_AREA \
   interrupt_save = (unsigned long)flags; \
   TX_TRACE_IN_LINE_INSERT(TX_TRACE_INTERRUPT_CONTROL, TX_ULONG_TO_POINTER_CONVERT(interrupt_save), 0, 0x2, 0, TX_TRACE_INTERRUPT_CONTROL_EVENT);	\
   TX_RESTORE \
}while(0);
#endif
 
/*=============================================================================
                                    TYPEDEFS
=============================================================================*/
typedef void (*qurt_interrupt_handler_func_ptr_t) ( uint32 n_irq );

/*=============================================================================
                                    FUNCTIONS
=============================================================================*/
/**@ingroup func_qurt_interrupt_irq_handler
  Initial handler when interrupt is invoked. 
  
  @param
  None

  @return
  None.

  @dependencies
  None.
  
*/
// void qurt_interrupt_irq_handler ( void );

/**@ingroup func_qurt_interrupt_register_default_irq
  Common IRQ handler that is invoked when each interrupt comes in.
  
  @param[in] n_irq - Interrupt Vector number to be handled. 

  @return
  None.

  @dependencies
  None.
  
*/

void qurt_interrupt_register_default_irq( qurt_interrupt_handler_func_ptr_t 
                             qurt_interrupt_default_irq_handler_ptr );

#endif /* QURT_INIT_H */

