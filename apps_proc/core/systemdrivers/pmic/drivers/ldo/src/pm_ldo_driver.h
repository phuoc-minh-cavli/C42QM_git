#ifndef PM_LDO_DRIVER__H
#define PM_LDO_DRIVER__H

/*! \file pm_ldo_driver.h
 *  \n
 *  \brief This file contains LDO peripheral driver related function prototypes,
 *         enums and driver data structure type.   
 *  \n  
 *  \n &copy; Copyright 2012-2014 QUALCOMM Technologies Incorporated, All Rights Reserved
 */
/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/systemdrivers/pmic/drivers/ldo/src/pm_ldo_driver.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/14   aks     Adding support for multiple PMICS > 2  
12/06/12   hw      Rearchitecturing module driver to peripheral driver
========================================================================== */

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_err_flags.h"
#include "pm_pwr_alg.h"
#include "pm_comm.h"

/*===========================================================================

                     STRUCTURE TYPE AND ENUM

===========================================================================*/



/*===========================================================================

                     LDO TYPES AND STRUCTURES 

===========================================================================*/
typedef struct
{
    pm_comm_info_type  *comm_ptr;
    pm_pwr_data_type  pm_pwr_data;
    uint8              *in_bypass;
    uint8              *periph_subtype;
} pm_ldo_data_type;

/*===========================================================================

                     FUNCTION DECLARATION 

===========================================================================*/
void pm_ldo_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index);

pm_ldo_data_type* pm_ldo_get_data(uint8 pmic_index);

uint8 pm_ldo_get_num_peripherals(uint8 pmic_index);


#endif /* PM_LDO_DRIVER__H */
