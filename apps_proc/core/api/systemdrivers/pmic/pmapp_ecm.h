#ifndef __PMAPP_ECM_H__
#define __PMAPP_ECM_H__
/*===========================================================================


               P M A P P_ E C M  H E A D E R    F I L E

\details
    This file contains functions prototypes and variable/type/constant 
  declarations for the PMIC ECM application
  
Copyright (c) 2016            by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/api/systemdrivers/pmic/pmapp_ecm.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
3/17/16    vk      Created file
===========================================================================*/
#include "comdef.h"
#include "pm_err_flags.h"



/*===========================================================================

                PMAPP ECM TYPE DEFINITIONS

===========================================================================*/
typedef struct
{
  uint8                    smps_count;                        /**< smps ecm enable count*/
  uint8                    ldo_count;                         /**< lodo ecm enable count */
  uint8                    ext_ch_count;                      /**< external channel(QPOET) ecm enable count */      
}pmapp_ecm_enable_status_type;

/*===========================================================================

                PM APP ECM FUNCTION PROTOTYPES

===========================================================================*/

/*=========================================================================== */
/*                     pmapp_ecm_enable_status                 	      */
/*=========================================================================== */
/**
  This function registers a callback that will be triggered once the CBLPWR_N
  signal is held for a pre-configured amount of time

  PARAMETERS
 * @param[in]  get_ecm_status: ecm enable rail status
  @return      pm_err_flag_type - PMIC error flag status 

*/
pm_err_flag_type pmapp_ecm_enable_status(pmapp_ecm_enable_status_type *get_ecm_status);

#endif /* __PMAPP_ECM_H__ */
