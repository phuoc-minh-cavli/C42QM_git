#ifndef QURT_SYSTEM_H
#define QURT_SYSTEM_H

/**
  @file qurt_system.h 
  @brief  Prototypes of QuRT System level functionality API

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2015,2021  by Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================*/

#include "comdef.h" //For definitions in target.builds
#include "qurt_types.h"

/*=============================================================================
                                 CONSTANTS AND MACROS
=============================================================================*/ 
#define QURT_SYSTEM_IRQ_MODE  0x12

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#ifndef INT_MASK
#define INT_MASK 0x80
#endif
/*------------------------------------------| 0x80C00000
|			 QURT_Memory_Pool (1MB)			|	
--------------------------------------------| 0x80B00000
|		  APPS Heap Region (~ 3.5 MB)		| 
--------------------------------------------| app_heap_start
|		     APPS Image (~ 6.5 MB)			| 
--------------------------------------------| 0x80000000 */

#define QURT_MEMPOOL_SIZE              QURT_MEM_RGN_SIZE 
#define QURT_MEMPOOL_START_ADDRESS     QURT_MEM_RGN_BASE

#define QURT_TX_SHARED_MEM_START  TX_SHARED_MEM_START
#define QURT_TX_SHARED_MEM_SIZE   TX_SHARED_MEM_SIZE


#ifndef TXM_MAXIMUM_MODULES
#define TXM_MAXIMUM_MODULES 8
#endif

#ifndef QURT_MAX_MODULES
#define QURT_MAX_MODULES               TXM_MAXIMUM_MODULES
#define QURT_MAX_ASIDS                 (QURT_MAX_MODULES + 1) /* ASID 0 belongs to kernel space */
#endif
#define KERNEL_ASID	0

/*------------------------------------------| 0x1000
|     VA Range 1                            |
--------------------------------------------| 0x40000000
|     QURT_TXM_VA_RANGE (512 MB)            |
--------------------------------------------| 0x60000000
|     VA Range 2                            |
--------------------------------------------| 0xFF001000 */

#define QURT_TXM_VA_RANGE_START             0x40000000
#define QURT_TXM_VA_RANGE_SIZE              (512 * 1024 * 1024) // 512 MB
#define FULL_VA_RANGE_START                 0x1000
#define FULL_VA_RANGE_SIZE                  0xFF000000          // Whole VA range, i.e upto 4GB - 16MB

#define QURT_POWER_SHUTDOWN_TYPE_WFI       0
#define QURT_POWER_SHUTDOWN_TYPE_L2RET     1
#define QURT_POWER_SHUTDOWN_TYPE_L2NORET   2

/*=============================================================================
                                    TYPEDEFS
=============================================================================*/

/*application heap information type*/
typedef struct qurt_sysenv_app_heap {
   /** @cond */
   unsigned int heap_base; /*heap base address*/
   unsigned int heap_limit; /*heap end address*/
   /** @endcond */
} qurt_sysenv_app_heap_t ;

/*=============================================================================
                                    FUNCTIONS
=============================================================================*/
/**@ingroup func_qurt_system_is_irq_mode
  Tells if the system is in IRQ mode or not.

  @return
  TRUE  -- System is in IRQ mode \n
  FALSE -- System is NOT in IRQ mode.
               
  @dependencies
  None.
*/
int qurt_system_is_irq_mode(void);

/**@ingroup func_qurt_system_is_irq_mode
  Tells if the Interrputs are Enabled or not.

  @return
  TRUE  -- Interrupts are Disabled \n
  FALSE -- Interrupts are Enabled.
               
  @dependencies
  None.
*/
int qurt_system_ints_are_locked(void);

/**@ingroup func_qurt_power_register_idle_hook
  BSP hook that need to be set if low for mode is desired
  
  @param[in] entrypoint Pointer to BSP function .

  @return
  None.

  @dependencies
  None.
  
*/
void qurt_power_register_idle_hook(void (*entrypoint) (qurt_time_t));

/**@ingroup func_qurt_system_register_bsp_hook
  BSP hook that need to be set before qurt_init gets invoked
  
  @param[in] entrypoint Pointer to BSP initialization function .

  @return
  None.

  @dependencies
  None.
  
*/
void qurt_system_register_bsp_hook(void (*entrypoint) (void));


void qurt_power_shutdown_hook(void *pFn,int l2_retention,int args);

/**@ingroup func_qurt_power_shutdown_enter
  Triggers a power collapse. \n
  This funciton saves the current thread context, flushes cache and invokes a WFI.
  The type indicates whether the L2 cache is retained during the power collapse or not.

  @param[in]  type    Specifies the type of power collapse
  QURT_POWER_SHUTDOWN_TYPE_WFI -- No power shutdown, only wait for interrupt \n
  QURT_POWER_SHUTDOWN_TYPE_L2NORET -- L2 cache non-retention type \n
  QURT_POWER_SHUTDOWN_TYPE_L2RET   -- L2 cache retention type \n
  
  @return
  QURT_EINVALID -- Invalid type argument.
  QURT_EOK -- Operation was successfully performed. This value indicates that the processor has
              returned from Power-collapse mode. \n
  
  @dependencies
  None.
*/
int qurt_power_shutdown_enter (int type,int args);

/**@ingroup func_qurt_power_shutdown_exit
  Undoes state changes made preparing for power collapse.\n
  This function unmasks the global interrupts.\n
  This should be called only from Idle thread context. Will raise an error if not.

  @return
  None

  @dependencies
  None.
  
*/
void qurt_power_shutdown_exit (void);


/**@ingroup func_qurt_system_power_systick_restore
  Restore systick timer when coming out of deep sleep.
  
  @param[in]  tsleep     time in deep sleep
  @param[in]  syst_rvr   reload value count
  @param[in]  syst_cvr   current value before sleep                   
                        
  @return
  A 32-bit word with remaining reload value to be set.

  @dependencies
  None.
  
*/
uint32 qurt_system_power_systick_restore( uint64 tsleep, uint32 syst_rvr, 
                                     uint32 syst_cvr );


/**@ingroup func_qurt_sysenv_get_app_heap  
  Gets information on the program heap from the kernel.
   
  @datatypes
  #qurt_sysenv_app_heap_t

  @param[out] aheap  Pointer to information on the program heap.

  @return
  QURT_EOK -- Success. \n
  QURT_EVAL -- Invalid parameter.
               
  @dependencies
  None.
*/
int qurt_sysenv_get_app_heap(qurt_sysenv_app_heap_t *aheap );

#endif /* QURT_SYSTEM_H */

