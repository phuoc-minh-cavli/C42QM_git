/*! \file
*  
*  \brief  mpp_app.cpp ----File contains the implementation of the public APIs for RTC resource type.
*  \details Implementation file for RTC resourece type.
*  Each of the APIs checks for access and then if necessary directs
*  the call to Driver implementation or to RPC function for the master processor.
*  
*    PMIC code generation Version: 2.0.0.22
*    This file contains code for Target specific settings and modes.
*  
*  &copy; Copyright 2020 Qualcomm Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.

$Header: //components/rel/core.tx/6.0/systemdrivers/pmic/drivers/rtc/src/pm_rtc.c#2 $ 
when       who     what, where, why
--------   ---     ----------------------------------------------------------
19/02/20   ks      created

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_rtc.h"
#include "pm_rtc_driver.h"
#include "pm_comm.h"
#include "pm_ulog.h"


/*===========================================================================

                       Definitions

===========================================================================*/
#define REG_RTC(reg) ((rtc_ptr->rtc_reg->base_address) \
                      + (rtc_ptr->rtc_reg->reg))
#define SLAVE_ID_RTC (rtc_ptr->comm_ptr->slave_id)
/*===========================================================================

                        GLOBAL DEFINITIONS

===========================================================================*/

#define RTC_TIME_SEC_SIZE  4
#define RTC_TIME_MSEC_SIZE 2

/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/

pm_err_flag_type pm_rtc_rw_get_ms_time(uint8 pmic_chip, pm_rtc_time_type  *time_ptr)
{
    pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;   
    pm_rtc_data_type      *rtc_ptr  = pm_rtc_get_data(pmic_chip);
	
    pm_register_data_type   rtc_sec_val[RTC_TIME_SEC_SIZE];
    pm_register_data_type   rtc_msec_val[RTC_TIME_MSEC_SIZE];

    if(rtc_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if (NULL == time_ptr)
    {
        errFlag |=  PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
    }
    else
    {
        // Read the second registers
        errFlag |= pm_comm_read_byte_array(SLAVE_ID_RTC, 
                                           REG_RTC(rdata0), RTC_TIME_SEC_SIZE, 
                                           rtc_sec_val, 0);
        
        //Read the millisecond registers
        errFlag |= pm_comm_read_byte_array(SLAVE_ID_RTC, 
                                           REG_RTC(msec_rdata0), RTC_TIME_MSEC_SIZE, 
                                           rtc_msec_val, 0);
        
        //Converting the byte array into the 32-BIT SEC field
        time_ptr->sec = pm_rtc_counter_val_to_time_32_bits(PM_RTC_VAL3_SHIFT, rtc_sec_val[3]);
        time_ptr->sec += pm_rtc_counter_val_to_time_32_bits(PM_RTC_VAL2_SHIFT, rtc_sec_val[2]);
        time_ptr->sec += pm_rtc_counter_val_to_time_32_bits(PM_RTC_VAL1_SHIFT, rtc_sec_val[1]);
        time_ptr->sec += pm_rtc_counter_val_to_time_32_bits(PM_RTC_VAL0_SHIFT, rtc_sec_val[0]);
        
        //Converting the byte array into the 32-BIT SEC field
        time_ptr->msec = pm_rtc_counter_val_to_time_32_bits(PM_RTC_VAL1_SHIFT, rtc_msec_val[1]);
        time_ptr->msec += pm_rtc_counter_val_to_time_32_bits(PM_RTC_VAL0_SHIFT, rtc_msec_val[0]);
    }

    return errFlag;
}

pm_err_flag_type pm_rtc_alarm_set_ms_time(uint8 pmic_chip, pm_rtc_time_type  *time_ptr)
{
    pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;
    pm_rtc_data_type      *rtc_ptr  = pm_rtc_get_data(pmic_chip);
    pm_register_data_type   rtc_sec_val[RTC_TIME_SEC_SIZE];
    pm_register_data_type   rtc_msec_val[RTC_TIME_MSEC_SIZE];

    if(rtc_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if (NULL == time_ptr)
    {
        errFlag |=  PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
    }
    else
    {       
        rtc_sec_val[0] = pm_rtc_time_32_bits_to_rtc_counter_val(PM_RTC_VAL0_SHIFT, time_ptr->sec);
        rtc_sec_val[1] = pm_rtc_time_32_bits_to_rtc_counter_val(PM_RTC_VAL1_SHIFT, time_ptr->sec);
        rtc_sec_val[2] = pm_rtc_time_32_bits_to_rtc_counter_val(PM_RTC_VAL2_SHIFT, time_ptr->sec);
        rtc_sec_val[3] = pm_rtc_time_32_bits_to_rtc_counter_val(PM_RTC_VAL3_SHIFT, time_ptr->sec);
        
        rtc_msec_val[0] = pm_rtc_time_32_bits_to_rtc_counter_val(PM_RTC_VAL0_SHIFT, time_ptr->msec);
        rtc_msec_val[1] = pm_rtc_time_32_bits_to_rtc_counter_val(PM_RTC_VAL1_SHIFT, time_ptr->msec);
        
        // Write the second registers
        errFlag |= pm_comm_write_byte_array(SLAVE_ID_RTC, 
                                           REG_RTC(alrm_data0), RTC_TIME_SEC_SIZE, 
                                           rtc_sec_val, 0);
        
        //Write the millisecond registers
        errFlag |= pm_comm_write_byte_array(SLAVE_ID_RTC, 
                                           REG_RTC(alrm_msec_data0), RTC_TIME_MSEC_SIZE, 
                                           rtc_msec_val, 0);

    }

    return errFlag;
}

pm_err_flag_type pm_rtc_alarm_enable(uint8 pmic_chip, boolean  enable)
{
    pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;
    pm_rtc_data_type      *rtc_ptr  = pm_rtc_get_data(pmic_chip);
    pm_register_data_type rtc_enable = 0;
	pm_model_type pmic_model  = PMIC_IS_INVALID;
        
    if(rtc_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    if (TRUE == enable)
    {
        rtc_enable = 0x80;
    }
    errFlag |= pm_comm_write_byte_mask(SLAVE_ID_RTC, REG_RTC(alrm_en_ctl), 0x80, rtc_enable,0);
	
    // set alarm control register to 0, SW workaround added for yoda 3.0 ONLY
	if(errFlag == PM_ERR_FLAG__SUCCESS)
	{
		pmic_model = pm_get_pmic_model(pmic_chip);
		if(pmic_model == PMIC_IS_PME9205)
		{
			errFlag += pm_comm_write_byte(SLAVE_ID_RTC, REG_RTC(alarm_ctl), 0,0);
		}
	}
    return errFlag;
}

pm_err_flag_type pm_rtc_alarm_msec_clk_enable(uint8 pmic_chip, boolean  enable)
{
	pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;
    pm_rtc_data_type      *rtc_ptr  = pm_rtc_get_data(pmic_chip);
	pm_register_data_type    msec_clk_cntl_data = 0;
        
    if(rtc_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if (TRUE == enable)
    {
        msec_clk_cntl_data = 0x1;
    }
			
	//MSEC RTC CFG - bit:4 for ENA_MSEC_CLK; bit:0 for ENA_MSEC_RTC
	if (errFlag == PM_ERR_FLAG__SUCCESS)
	{
		errFlag |= pm_comm_write_byte_mask(SLAVE_ID_RTC, REG_RTC(alrm_msec_rtc_cfg), 0x1, msec_clk_cntl_data,0); //4-bit one
	}

    return errFlag;
}

/*===========================================================================

                        INTERNAL FUNCTION DEFINITIONS

===========================================================================*/
pm_register_data_type pm_rtc_time_32_bits_to_rtc_counter_val(pm_rtc_val_shift_type  bits_shift, uint32  sec_32_bits)
{
    pm_register_data_type data = 0;

    data = (pm_register_data_type) ( sec_32_bits >> bits_shift ) ;
    return (data);
}

uint32 pm_rtc_counter_val_to_time_32_bits(pm_rtc_val_shift_type  bits_shift, pm_register_data_type  rtc_val_8_bits)
{
    uint32 time = 0 ;
    uint32 temp32 = rtc_val_8_bits ;

    time = temp32 << bits_shift ;
    return time ;
}
