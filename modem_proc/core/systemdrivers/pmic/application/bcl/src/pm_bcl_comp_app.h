#ifndef __PM_BCL_COMP_APP_H__
#define __PM_BCL_COMP_APP_H__
/*===========================================================================


               P M _ B C L_ C O M P_ APP  H E A D E R    F I L E

\details
    This file contains functions prototypes and variable/type/constant 
  declarations to support conversions between voltage values and PMIC
  register values
  
Copyright (c) 2017, 2019            by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.10/systemdrivers/pmic/application/bcl/src/pm_bcl_comp_app.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/07/17   rl      File created
===========================================================================*/
                                           
/**
Initialize BCL App Layer
 @note Usage Example:
            pmapp_bcl_comp_init();
*/                                         
                                           
void pmapp_bcl_comp_init(void);                                        
                                           
/** @} */ /* end_addtogroup pmapp_pwr */

#endif /* __PM_BCL_COMP_APP_H__ */
