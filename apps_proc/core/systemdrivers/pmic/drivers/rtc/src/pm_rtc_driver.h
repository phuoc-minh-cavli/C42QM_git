#ifndef PM_RTC_DRIVER__H
#define PM_RTC_DRIVER__H

/*! \file
*  \n
*  \brief  pm_rtc_driver.h
*  \details  This file contains functions prototypes and variable/type/constant
*  declarations for supporting RTC pin services for the Qualcomm
*  PMIC chip set.
*  \n &copy; Copyright 2010-2013 Qualcomm Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/systemdrivers/pmic/drivers/rtc/src/pm_rtc_driver.h#2 $

when         who      what, where, why
--------     ---      ----------------------------------------------------------
04/10/12     hs       Changed the type for internalResourceIndex from int to uint8.
08/18/11     wra      mega clocks module V6 Initial Creation
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_err_flags.h"
#include "pm_target_information.h"
#include "pm_comm.h"

/*===========================================================================

                     TYPE DEFINITIONS 

===========================================================================*/
typedef struct pm_rtc_register_info_type
{
    pm_register_address_type    base_address;
    pm_register_address_type    peripheral_offset;
    pm_register_address_type    rtc_en_ctl;         //0x46    //sec_rtc_ctrl;
    pm_register_address_type    alrm_en_ctl;        //0x146   //Alarm_en_cntl
	pm_register_address_type    alarm_ctl;          //0x147   //Alarm Alarm Ctrl 
    pm_register_address_type    abort_en_ctl;       //0x46     //Abort enable
    pm_register_address_type    rdata0;             //0x48    //rtc_rw_rdata0;
    pm_register_address_type    rdata1;             //0x49    //rtc_rw_rdata1;
    pm_register_address_type    rdata2;             //0x4A    //rtc_rw_rdata2;
    pm_register_address_type    rdata3;             //0x4B    //rtc_rw_rdata3;
    pm_register_address_type    msec_rdata0;        //0x62    //rtc_rw_msec_rdata0;
    pm_register_address_type    msec_rdata1;        //0x63    //rtc_rw_msec_rdata1;
    pm_register_address_type    alrm_data0;         //0x140   //rtc_alrm_data0;
    pm_register_address_type    alrm_data1;         //0x141   //rtc_alrm_data1;
    pm_register_address_type    alrm_data2;         //0x142   //rtc_alrm_data2;
    pm_register_address_type    alrm_data3;         //0x143   //rtc_alrm_data3;
    pm_register_address_type    alrm_msec_data0;    //0x144   //rtc_alrm_msec_data0;
    pm_register_address_type    alrm_msec_data1;    //0x145   //rtc_alrm_msec_data1;
    pm_register_address_type    alrm_msec_rtc_cfg;  //0x156   //rtc_alrm_msec_rtc_cfg;	
}pm_rtc_register_info_type;

typedef enum pm_rtc_val_shift_type
{
    PM_RTC_VAL0_SHIFT = 0,
    PM_RTC_VAL1_SHIFT = 8,
    PM_RTC_VAL2_SHIFT = 16,
    PM_RTC_VAL3_SHIFT = 24,
    PM_HAL_RTC_INTERNAL_VAL_FROM_TIME__INVALID
} pm_rtc_val_shift_type;

typedef struct pm_rtc_data_type
{
    pm_comm_info_type *comm_ptr;
    pm_rtc_register_info_type* rtc_reg;
    uint8 num_of_peripherals;
}pm_rtc_data_type;

/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/
void pm_rtc_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index);

pm_rtc_data_type* pm_rtc_get_data(uint8 pmic_index);

pm_register_data_type pm_rtc_time_32_bits_to_rtc_counter_val(pm_rtc_val_shift_type  bits_shift, uint32  sec_32_bits);
uint32 pm_rtc_counter_val_to_time_32_bits(pm_rtc_val_shift_type  bits_shift, pm_register_data_type  rtc_val_8_bits);

pm_err_flag_type pm_rtc_alarm_msec_clk_enable(uint8 pmic_chip, boolean  enable);

#endif //PM_RTC_DRIVER__H
