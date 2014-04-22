/*! \file
 *  \n
 *  \brief  pm_rtc_driver.c
 *  \details  
 *  \n &copy; Copyright 2010-2013 Qualcomm Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/systemdrivers/pmic/drivers/rtc/src/pm_rtc_driver.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/18/11   wra     mega clocks module V6 Initial Creation
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_rtc_driver.h"
#include "CoreVerify.h"
#include "device_info.h"
#include "pm_utils.h"
#include "pm_malloc.h"
/*===========================================================================

                        STATIC VARIABLES 

===========================================================================*/

/* Static global variable to store the RTC data */
static pm_rtc_data_type *pm_rtc_data_arr[PM_MAX_NUM_PMICS];

static pm_rtc_register_info_type rtc_reg = 
{
     0x6000, //0x6000  //base address;
     0x100,            //peripheral_offset;
     0x46,   //0x46    //sec_rtc_ctrl;
     0x146,  //0x146   //Alarm_en_cntl
	 0x147,  //0x147   //Alarm Alarm Ctrl 
     0x46,   //0x46    //Abort enable
     0x48,   //0x48    //rtc_rw_rdata0;
     0x49,   //0x49    //rtc_rw_rdata1;
     0x4A,   //0x4A    //rtc_rw_rdata2;
     0x4B,   //0x4B    //rtc_rw_rdata3;
     0x62,   //0x62    //rtc_rw_msec_rdata0;
     0x63,   //0x63    //rtc_rw_msec_rdata1;
     0x140,  //0x140   //rtc_alrm_data0;
     0x141,  //0x141   //rtc_alrm_data1;
     0x142,  //0x142   //rtc_alrm_data2;
     0x143,  //0x143   //rtc_alrm_data3;
     0x144,  //0x144   //rtc_alrm_msec_data0;
     0x145,  //0x145   //rtc_alrm_msec_data1;
     0x156,  //0x156   //rtc_alrm_msec_rtc_cfg;  
};

/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/

void pm_rtc_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index)
{
    pm_rtc_data_type *rtc_ptr = NULL;
    
   rtc_ptr =  pm_rtc_data_arr[pmic_index];
   if(rtc_ptr == NULL)
   {
     pm_malloc(sizeof(pm_rtc_data_type),(void**)&rtc_ptr);
     pm_rtc_data_arr[pmic_index] = rtc_ptr;
   }
        
    if (rtc_ptr != NULL)
    {
                                                    
        /* Assign Comm ptr */
        rtc_ptr->comm_ptr = comm_ptr;
        
        /* RTC Register Info - Obtaining Data through dal config */
        rtc_ptr->rtc_reg = &rtc_reg;
        
        CORE_VERIFY_PTR(rtc_ptr->rtc_reg);
        
        rtc_ptr->num_of_peripherals = pm_target_information_get_periph_count_info(PM_PROP_RTC_NUM, pmic_index);

        /* Num of peripherals cannot be 0 if this driver init gets called */
        CORE_VERIFY(rtc_ptr->num_of_peripherals != 0);
    }
}

pm_rtc_data_type* pm_rtc_get_data(uint8 pmic_index)
{
    if(pmic_index < PM_MAX_NUM_PMICS)
    {
        return pm_rtc_data_arr[pmic_index];
    }

    return NULL;
}


uint8 pm_rtc_get_num_peripherals(uint8 pmic_index)
{
  if((pmic_index < PM_MAX_NUM_PMICS) && 
      (pm_rtc_data_arr[pmic_index] != NULL))
  {
      return pm_rtc_data_arr[pmic_index]->num_of_peripherals;
  }

  return NULL;
}

