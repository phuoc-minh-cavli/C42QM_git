#ifndef __PM_PBS_CLIENT_DRIVER_H__
#define __PM_PBS_CLIENT_DRIVER_H__

/*! \file pm_pbs_client_driver.h 
*  \n 
*  \brief PBS Client driver type definitions and init function prototype.   
*  \n  
*  \n &copy; Copyright 2013 QUALCOMM Technologies Incorporated, All Rights Reserved
*/
/* =======================================================================
                                Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/systemdrivers/pmic/drivers/pbs/src/pm_pbs_client_driver.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/13/13   kt      Initial version
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_err_flags.h"
#include "pm_target_information.h"


/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/
typedef struct
{
	
    pm_register_address_type base_address;
    pm_register_address_type peripheral_offset;
    pm_register_address_type trig_ctl;
    pm_register_address_type en_ctl1;
    pm_register_address_type int_rt_status;
    pm_register_address_type int_set_type;
    pm_register_address_type int_polarity_high;
    pm_register_address_type int_polarity_low;
    pm_register_address_type int_latched_clr;
    pm_register_address_type int_en_set;
    pm_register_address_type int_en_clr;
    pm_register_address_type int_latched_status;
    pm_register_address_type int_pending_status;
    pm_register_address_type trig_cfg;
    pm_register_address_type hw_trig_re_en;     //0x40
    pm_register_address_type hw_trig_fe_en;     //0x41
    pm_register_address_type trig_latched_clr;  //0x48
    pm_register_address_type scratch1;          //0x50
    pm_register_address_type scratch2;          //0x51
}pm_pbs_client_register_info_type;

typedef struct
{
    pm_comm_info_type                 *comm_ptr;
    pm_pbs_client_register_info_type   *pbs_client_reg_table;   
    uint8                              num_peripherals;
}pm_pbs_client_data_type;

/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/
void pm_pbs_client_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index);

pm_pbs_client_data_type* pm_pbs_client_get_data(uint8 pmic_index);

uint8 pm_pbs_client_get_num_peripherals(uint8 pmic_index);

#endif /* __PM_PBS_CLIENT_DRIVER_H__ */
