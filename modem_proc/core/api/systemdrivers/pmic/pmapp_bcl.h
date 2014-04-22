#ifndef __PMAPP_BCL_H__
#define __PMAPP_BCL_H__
/*===========================================================================


               P M A P P_ B C L_ C O M P  H E A D E R    F I L E

\details
    This file contains functions prototypes and variable/type/constant 
  declarations to support conversions between voltage values and PMIC
  register values
  
Copyright (c) 2019            by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.10/api/systemdrivers/pmic/pmapp_bcl.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/07/13   rl      File created
===========================================================================*/

#include "pm_err_flags.h"
#include "pm_resources_and_types.h"
#include "com_dtypes.h"

typedef enum
{
    PMAPP_BCL_LOW1_THR = 0,       /**< LOW1 Threshold. */
    PMAPP_BCL_HI1_THR  = 1,       /**< HI Threshold. */ 
    PMAPP_BCL_THR__INVALID
}pmapp_bcl_thr_type;

/**
Callback function type definition
*/
typedef void (*pmapp_bcl_client_cb) (pmapp_bcl_thr_type, void *, void *, void *);


/**
Sets Threshold Voltage for Comparator, enables IRQ and Registers Callback func.
 @param voltage  Voltage to be set for comparator in mV 
                 Range of voltage 2250 to 3600 mV in 
                 25 mV steps 

 @param thr_type Threshold type can be selected as low1 or low2.
                  PMAPP_BCL_LOW1_THR
                  PMAPP_BCL_HI1_THR    
                  
 @param cb_ptr   Callback function pointer
 
 @param irq_en   TRUE to enable Threshold IRQ
                 FALSE to disable Threshold IRQ
 @note Usage Example:
            pmapp_bcl_set_thr_voltage(3700, PM_BCL_COMP_LOW1_THR, 
                                      &cb_func, TRUE);
 
 @return 
 PM_ERR_FLAG__SUCCESS if successful. See #pm_err_flag_type 
*/
pm_err_flag_type pmapp_bcl_set_thr_voltage(uint32 voltage, 
                                           pmapp_bcl_thr_type threshold_type, 
                                           pmapp_bcl_client_cb cb_ptr,
                                           boolean irq_en);

/** @} */ /* end_addtogroup pmapp_pwr */

#endif /* __PMAPP_BCL_H__ */
