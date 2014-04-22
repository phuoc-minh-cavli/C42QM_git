#ifndef PM_VBUS_DRIVER__H
#define PM_VBUS_DRIVER__H

/*! \file
*  \n
*  \brief  pm_vbus_driver.h 
*  \details  This file contains functions prototypes and variable/type/constant
declarations for supporting VBUS pin services for the Qualcomm
PMIC chip set.
*  \n &copy; Copyright 2009 - 2013 Qualcomm Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/systemdrivers/pmic/drivers/vbus/src/pm_vbus_driver.h#1 $
$DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/24/15   vk      Added new vbus driver support
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_err_flags.h"
#include "pm_target_information.h"
#include "pm_irq.h"

/*===========================================================================

                     TYPE DEFINITIONS 

===========================================================================*/
typedef struct pm_vbus_register_info_type
{                  
    pm_register_address_type    base_address;  
    pm_register_address_type    peripheral_offset;   
    pm_register_address_type    status;                  //0x08
    pm_register_address_type    int_rt_sts;              //0x10
    pm_register_address_type    int_set_type;            //0x11
    pm_register_address_type    int_polarity_high;       //0x12
    pm_register_address_type    int_polarity_low;        //0x13
    pm_register_address_type    int_latched_clr;         //0x14
    pm_register_address_type    int_en_set;              //0x15
    pm_register_address_type    int_en_clr;              //0x16
    pm_register_address_type    int_latched_sts;         //0x18
    pm_register_address_type    int_pending_sts;         //0x19
    pm_register_address_type    int_mid_sel;             //0x1A
    pm_register_address_type    int_priority;            //0x1B
}pm_vbus_register_info_type;

typedef struct pm_vbus_data_type
{
    pm_comm_info_type        *comm_ptr;
    pm_vbus_register_info_type* vbus_register;
    uint8 num_of_peripherals;
}pm_vbus_data_type;

/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/
void pm_vbus_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index);

pm_vbus_data_type* pm_vbus_get_data(uint8 pmic_index);

uint8 pm_vbus_get_num_peripherals(uint8 pmic_index);

pm_err_flag_type pm_vbus_irq_set_trigger(uint8 pmic_chip, uint8 vbus_index,  pm_irq_trigger_type trigger);
pm_err_flag_type pm_vbus_irq_enable(  uint8 pmic_chip,   uint8 vbus_index,  boolean enable);
pm_err_flag_type pm_vbus_irq_clear(  uint8 pmic_chip,   uint8 vbus_index);
pm_err_flag_type pm_vbus_irq_status(  uint8 pmic_chip,   uint8 vbus_index,   pm_irq_status_type type,   boolean *status);
pm_err_flag_type pm_vbus_get_pin_status(  uint8 pmic_chip,   uint8 vbus_index,   pm_pin_status *pin_status);

#endif //PM_VBUS_DRIVER__H
