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
*  &copy; Copyright 2010, 2019 Qualcomm Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.

$Header: //components/rel/core.mpss/3.10/systemdrivers/pmic/drivers/rtc/src/pm_rtc.c#5 $
when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/10/12   hs      Changed the type for internalResourceIndex from int to uint8.
11/15/11   hs      Added support for pm_hard_reset() per CR316544

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/

#include "device_info.h"
#include "pm_device.h"
#include "pm_rtc_driver.h"
#include "pm_rtc.h"
#include "pm_version.h"


/*===========================================================================

                        FUNCTION DECLARATIONS

===========================================================================*/

 static pm_err_flag_type pm_rtc_get_rtc_info(uint8 pmic_device_index, uint32 *pon_slave_id);


/*===========================================================================

                        GLOBAL DEFINITIONS

===========================================================================*/

#define RTC_TIME_SEC_SIZE  4
#define RTC_TIME_MSEC_SIZE 2

static boolean pm_rtc_initialized = FALSE;

static pm_rtc_register_info_type rtc_reg = 
{
    .base_address      = 0x6000, //0x6000  //base address
    .peripheral_offset = 0x100,            //peripheral_offset
    .time_adj          = 0x44,   //0x44    //rtc_time_adjust;
    .rtc_en_ctl        = 0x46,   //0x46    //sec_rtc_ctrl;
    .alrm_en_ctl       = 0x146,  //0x146   //Alarm_en_cntl
	.alarm_ctl         = 0x147,  //0x147   //Alarm Alarm Ctrl 
    .abort_en_ctl      = 0x46,   //0x46    //Abort enable
    .int_latched_clr   = 0x14,   //0x14    //rtc_rw_alrm_ctrl;
    .wdata0            = 0x40,   //0x40    //rtc_rw_wdata0;
    .wdata1            = 0x41,   //0x41    //rtc_rw_wdata1;
    .wdata2            = 0x42,   //0x42    //rtc_rw_wdata2;
    .wdata3            = 0x43,   //0x43    //rtc_rw_wdata3;
    .rdata0            = 0x48,   //0x48    //rtc_rw_rdata0;
    .rdata1            = 0x49,   //0x49    //rtc_rw_rdata1;
    .rdata2            = 0x4A,   //0x4A    //rtc_rw_rdata2;
    .rdata3            = 0x4B,   //0x4B    //rtc_rw_rdata3;
    .msec_rdata0       = 0x62,   //0x62    //rtc_rw_msec_rdata0;
    .msec_rdata1       = 0x63,   //0x63    //rtc_rw_msec_rdata1;
    .alrm_data0        = 0x140,  //0x140   //rtc_alrm_data0;
    .alrm_data1        = 0x141,  //0x141   //rtc_alrm_data1;
    .alrm_data2        = 0x142,  //0x142   //rtc_alrm_data2;
    .alrm_data3        = 0x143,  //0x143   //rtc_alrm_data3;
    .alrm_msec_data0   = 0x144,  //0x144   //rtc_alrm_msec_data0;
    .alrm_msec_data1   = 0x145,  //0x145   //rtc_alrm_msec_data1;
    .alrm_msec_rtc_cfg = 0x156,  //0x156   //rtc_alrm_msec_rtc_cfg;  
    .test              = 0xE2,   //0xE2    //rtc_test;
};

/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/

pm_err_flag_type pm_rtc_rw_get_ms_time(uint8 pmic_chip, pm_rtc_time_type  *time_ptr)
{
    pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;   
    uint32            slave_id = 0;
	
    errFlag |= pm_rtc_get_rtc_info(pmic_chip, &slave_id);

    if (errFlag != PM_ERR_FLAG__SUCCESS)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    pm_register_data_type   rtc_sec_val[RTC_TIME_SEC_SIZE];
    pm_register_data_type   rtc_msec_val[RTC_TIME_MSEC_SIZE];
    pm_register_address_type reg_rdata0 = rtc_reg.base_address + rtc_reg.rdata0;
    pm_register_address_type reg_msec_rdata0 = rtc_reg.base_address + rtc_reg.msec_rdata0;

    /* TODO: Find what the initial time is and calculate the maximum time we can pass in. */
    if (NULL == time_ptr)
    {
        errFlag |=  PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
    }
    else
    {
        // Read the second registers
        errFlag |= pm_comm_read_byte_array(slave_id, 
                                           reg_rdata0, RTC_TIME_SEC_SIZE, 
                                           rtc_sec_val, 0);
        
        //Read the millisecond registers
        errFlag |= pm_comm_read_byte_array(slave_id, 
                                           reg_msec_rdata0, RTC_TIME_MSEC_SIZE, 
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
    uint32            slave_id = 0;
    
    errFlag = pm_rtc_get_rtc_info(pmic_chip, &slave_id);
    if (errFlag != PM_ERR_FLAG__SUCCESS)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    pm_register_data_type   rtc_sec_val[RTC_TIME_SEC_SIZE];
    pm_register_data_type   rtc_msec_val[RTC_TIME_MSEC_SIZE];
    pm_register_address_type reg_data0 = rtc_reg.base_address + rtc_reg.alrm_data0;
    pm_register_address_type reg_msec_data0 = rtc_reg.base_address + rtc_reg.alrm_msec_data0;

                                           


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
        errFlag |= pm_comm_write_byte_array(slave_id, 
                                           reg_data0, RTC_TIME_SEC_SIZE, 
                                           rtc_sec_val, 0);
        
        //Write the millisecond registers
        errFlag |= pm_comm_write_byte_array(slave_id, 
                                           reg_msec_data0, RTC_TIME_MSEC_SIZE, 
                                           rtc_msec_val, 0);
       //backing up the rtc alarm to aon reigsters								   
        pm_comm_write_byte(slave_id ,0xa40 ,rtc_sec_val[0],0 );
        pm_comm_write_byte(slave_id ,0xa41 ,rtc_sec_val[1],0 );
        pm_comm_write_byte(slave_id ,0xa46 ,rtc_sec_val[2],0 );
        pm_comm_write_byte(slave_id ,0xa47 ,rtc_sec_val[3],0 );
 
    }

    return errFlag;
}

pm_err_flag_type pm_rtc_alarm_enable(uint8 pmic_chip, boolean  enable)
{
    pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;
    uint32            slave_id = 0;
    pm_register_data_type rtc_enable = 0;
	pm_model_type pmic_model  = PMIC_IS_INVALID;
        
    errFlag = pm_rtc_get_rtc_info(pmic_chip, &slave_id);
    if (errFlag != PM_ERR_FLAG__SUCCESS)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    pm_register_address_type reg_enable_ctl1 = rtc_reg.base_address + rtc_reg.alrm_en_ctl;  
    
    if (TRUE == enable)
    {
        rtc_enable = 0x80;
    }
    //Mask is on 7th bit - to control ALARM_EN only
    errFlag |= pm_comm_write_byte_mask(slave_id, reg_enable_ctl1, 0x80, rtc_enable,0);

	// set alarm control register to 0, SW workaround added for yoda 3.0 ONLY
	pm_register_address_type reg_alarm_ctl1 = rtc_reg.base_address + rtc_reg.alarm_ctl;
	
	if(errFlag == PM_ERR_FLAG__SUCCESS)
	{
		pmic_model = pm_get_pmic_model(pmic_chip);
		if(pmic_model == PMIC_IS_PME9205)
		{
			errFlag += pm_comm_write_byte(slave_id, reg_alarm_ctl1, 0,0);
		}
	}
    return errFlag;
}

pm_err_flag_type pm_rtc_alarm_msec_clk_enable(uint8 pmic_chip, boolean  enable)
{
	pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;
    uint32            slave_id = 0;
	pm_register_data_type    msec_clk_cntl_data = 0;
        
    errFlag |= pm_rtc_get_rtc_info(pmic_chip, &slave_id);
    if (errFlag != PM_ERR_FLAG__SUCCESS)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    pm_register_address_type reg_msec_rtc_cfg = rtc_reg.base_address + rtc_reg.alrm_msec_rtc_cfg;
    if (TRUE == enable)
    {
        msec_clk_cntl_data = 0x1;
    }
			
	//MSEC RTC CFG - bit:4 for ENA_MSEC_CLK; bit:0 for ENA_MSEC_RTC
	if (errFlag == PM_ERR_FLAG__SUCCESS)
	{
		errFlag |= pm_comm_write_byte_mask(slave_id, reg_msec_rtc_cfg, 0x1, msec_clk_cntl_data,0); //4-bit one
	}

    return errFlag;
}


/*===========================================================================

            Helper FUNCTION DEFINITIONS 

===========================================================================*/



/*internal function to return slave id*/
static pm_err_flag_type pm_rtc_get_rtc_info(uint8 pmic_device_index, 
                                            uint32 *pon_slave_id)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

  if (NULL == pon_slave_id)
  {
    return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
  }

  if (pm_rtc_initialized == FALSE)
  {    
    //Check SPMI is enabled - this is needed for standalone driver
	err_flag |= pm_device_setup();
    if (err_flag != PM_ERR_FLAG__SUCCESS)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    pm_rtc_initialized = TRUE;
  }

  err_flag |= pm_get_slave_id(pmic_device_index, 0, pon_slave_id);

  return err_flag;
}


/*===========================================================================

            Depricated FUNCTION DEFINITIONS - need to clear - TODO

===========================================================================*/


pm_err_flag_type pm_dev_hal_rtc_start(uint8 pmic_chip, const pm_hal_rtc_time_type* start_time_ptr)
{
    pm_err_flag_type  errFlag  = PM_ERR_FLAG__SUCCESS;
    pm_rtc_data_type  *rtc_ptr = pm_rtc_get_data(pmic_chip);

    if(rtc_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    pm_register_address_type reg_rtc_en_ctl = (rtc_ptr->megartc_register->base_address) + rtc_ptr->megartc_register->rtc_en_ctl;
    pm_register_address_type reg_wdata0 = (rtc_ptr->megartc_register->base_address) + rtc_ptr->megartc_register->wdata0;
    pm_register_address_type reg_wdata1 = (rtc_ptr->megartc_register->base_address) + rtc_ptr->megartc_register->wdata1;
    pm_register_address_type reg_wdata2 = (rtc_ptr->megartc_register->base_address) + rtc_ptr->megartc_register->wdata2;
    pm_register_address_type reg_wdata3 = (rtc_ptr->megartc_register->base_address) + rtc_ptr->megartc_register->wdata3;

    if (NULL == start_time_ptr)
    {
        errFlag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
    }
    else
    {
        //INTLOCK();
        /* clear bit 7 of the SEC_RTC_CNTRL register to disable RTC */
        /* This means taht every time a HalRtcStart API is called. It will STOP RTC first before it START RTC */

        errFlag = pm_comm_write_byte_mask(rtc_ptr->comm_ptr->slave_id, reg_rtc_en_ctl , 0x80, 0, 0);

        //TransactionManager::StartTransaction(this, false) TBD;
        errFlag = pm_comm_write_byte(rtc_ptr->comm_ptr->slave_id, reg_wdata3, pm_rtc_time_32_bits_to_rtc_counter_val(PM_RTC_VAL3_SHIFT, start_time_ptr->sec), 0);
        errFlag = pm_comm_write_byte(rtc_ptr->comm_ptr->slave_id, reg_wdata2, pm_rtc_time_32_bits_to_rtc_counter_val(PM_RTC_VAL2_SHIFT, start_time_ptr->sec), 0);
        errFlag = pm_comm_write_byte(rtc_ptr->comm_ptr->slave_id, reg_wdata1, pm_rtc_time_32_bits_to_rtc_counter_val(PM_RTC_VAL1_SHIFT, start_time_ptr->sec), 0);
        errFlag = pm_comm_write_byte(rtc_ptr->comm_ptr->slave_id, reg_wdata0, pm_rtc_time_32_bits_to_rtc_counter_val(PM_RTC_VAL0_SHIFT, start_time_ptr->sec), 0);
       //TransactionManager::EndTransaction(this, false) TBD;
        /* set bit 7 of the SEC_RTC_CNTRL register to enable RTC */
        errFlag = pm_comm_write_byte_mask(rtc_ptr->comm_ptr->slave_id, reg_rtc_en_ctl , 0x80, 0x80, 0);

        //INTFREE();
    }

    return errFlag;
}

pm_err_flag_type pm_dev_hal_rtc_stop(uint8 pmic_chip)
{
    pm_err_flag_type      errFlag   = PM_ERR_FLAG__SUCCESS;
    pm_rtc_data_type *rtc_ptr = pm_rtc_get_data(pmic_chip);

    if(rtc_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    pm_register_address_type reg_rtc_en_ctl = (rtc_ptr->megartc_register->base_address) + rtc_ptr->megartc_register->rtc_en_ctl;
    errFlag = pm_comm_write_byte_mask(rtc_ptr->comm_ptr->slave_id, reg_rtc_en_ctl, 0x80, 0, 0);

    return errFlag;
}

pm_err_flag_type pm_dev_hal_rtc_get_time(uint8 pmic_chip, pm_hal_rtc_time_type  *time_ptr)
{
    pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;
    pm_rtc_data_type  *rtc_ptr = pm_rtc_get_data(pmic_chip);

    if (rtc_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    pm_register_data_type   rtcONOFF = 0, rtc_val0, rtc_val1, rtc_val2, rtc_val3;
    pm_register_address_type reg_rtc_en_ctl = (rtc_ptr->megartc_register->base_address) + rtc_ptr->megartc_register->rtc_en_ctl;
    pm_register_address_type reg_rdata0 = (rtc_ptr->megartc_register->base_address) + rtc_ptr->megartc_register->rdata0;
    pm_register_address_type reg_rdata1 = (rtc_ptr->megartc_register->base_address) + rtc_ptr->megartc_register->rdata1;
    pm_register_address_type reg_rdata2 = (rtc_ptr->megartc_register->base_address) + rtc_ptr->megartc_register->rdata2;
    pm_register_address_type reg_rdata3 = (rtc_ptr->megartc_register->base_address) + rtc_ptr->megartc_register->rdata3;

    /* TODO: Find what the initial time is and calculate the maximum time we can pass in. */
    if (NULL == time_ptr)
    {
        errFlag =  PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
    }
    else
    {
        // check if RTC HW is currently running (enabled) by looking at RTC_EN bit of SEC_RTC_CNTRL register.

        errFlag = pm_comm_read_byte_mask(rtc_ptr->comm_ptr->slave_id, reg_rtc_en_ctl, 0x80 , &rtcONOFF,0);

        /* If RTC is enabled (ON), rtcONOFF should return "1". And "0" if it is disabled (OFF). */
        if (!(rtcONOFF & 0x80))
        {
            errFlag =  PM_ERR_FLAG__RTC_HALTED;
        }
        else
        {
            //INTLOCK();

            //TransactionManager::StartTransaction(this, false) TBD;
            errFlag = pm_comm_read_byte(rtc_ptr->comm_ptr->slave_id, reg_rdata3, &rtc_val3, 0);
            errFlag = pm_comm_read_byte(rtc_ptr->comm_ptr->slave_id, reg_rdata2, &rtc_val2, 0);
            errFlag = pm_comm_read_byte(rtc_ptr->comm_ptr->slave_id, reg_rdata1, &rtc_val1, 0);
            errFlag = pm_comm_read_byte(rtc_ptr->comm_ptr->slave_id, reg_rdata0, &rtc_val0, 0);
           //TransactionManager::EndTransaction(this, false) TBD;

            //INTFREE();
            /* calculations are performed outside of intlock to shorten time spent in inlock. */
            time_ptr->sec = pm_rtc_counter_val_to_time_32_bits(PM_RTC_VAL3_SHIFT, rtc_val3);
            time_ptr->sec += pm_rtc_counter_val_to_time_32_bits(PM_RTC_VAL2_SHIFT, rtc_val2);
            time_ptr->sec += pm_rtc_counter_val_to_time_32_bits(PM_RTC_VAL1_SHIFT, rtc_val1);
            time_ptr->sec += pm_rtc_counter_val_to_time_32_bits(PM_RTC_VAL0_SHIFT, rtc_val0);
        }
    }

    return errFlag;
}

pm_err_flag_type pm_dev_hal_rtc_enable_alarm(uint8 pmic_chip, pm_hal_rtc_alarm  what_alarm, const  pm_hal_rtc_time_type* trigger_time_ptr)
{
    pm_err_flag_type  errFlag  = PM_ERR_FLAG__SUCCESS;
    pm_rtc_data_type  *rtc_ptr = pm_rtc_get_data(pmic_chip);

    if (rtc_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    pm_register_address_type reg_alrm_data0 = (rtc_ptr->megartc_register->base_address) + rtc_ptr->megartc_register->alrm_data0;
    pm_register_address_type reg_alrm_data1 = (rtc_ptr->megartc_register->base_address) + rtc_ptr->megartc_register->alrm_data1;
    pm_register_address_type reg_alrm_data2 = (rtc_ptr->megartc_register->base_address) + rtc_ptr->megartc_register->alrm_data2;
    pm_register_address_type reg_alrm_data3 = (rtc_ptr->megartc_register->base_address) + rtc_ptr->megartc_register->alrm_data3;
    pm_register_address_type reg_alrm_en_ctl = (rtc_ptr->megartc_register->base_address) + rtc_ptr->megartc_register->alrm_en_ctl;

    if (what_alarm >= PM_HAL_RTC_ALARM_INVALID)
    {
        return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
    }
    if (NULL == trigger_time_ptr)
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }

    //INTLOCK();

    // set the alarm time way in the future to prevent the alarm from going off before all 4 registers have been written

    /* Why not disable. TBD  */
    errFlag = pm_comm_write_byte(rtc_ptr->comm_ptr->slave_id, reg_alrm_data3, 0xFF, 0);

    //TransactionManager::StartTransaction(this, false) TBD;
    errFlag = pm_comm_write_byte(rtc_ptr->comm_ptr->slave_id, reg_alrm_data0, pm_rtc_time_32_bits_to_rtc_counter_val(PM_RTC_VAL0_SHIFT, trigger_time_ptr->sec), 0 );
    errFlag = pm_comm_write_byte(rtc_ptr->comm_ptr->slave_id, reg_alrm_data1, pm_rtc_time_32_bits_to_rtc_counter_val(PM_RTC_VAL1_SHIFT, trigger_time_ptr->sec), 0 );
    errFlag = pm_comm_write_byte(rtc_ptr->comm_ptr->slave_id, reg_alrm_data2, pm_rtc_time_32_bits_to_rtc_counter_val(PM_RTC_VAL2_SHIFT, trigger_time_ptr->sec), 0 );
    errFlag = pm_comm_write_byte(rtc_ptr->comm_ptr->slave_id, reg_alrm_data3, pm_rtc_time_32_bits_to_rtc_counter_val(PM_RTC_VAL3_SHIFT, trigger_time_ptr->sec), 0 );
    //TransactionManager::EndTransaction(this, false) TBD;

    /* Set bit 1 of the SEC_RTC_CNTRL register, bit<1> to "1" to enable alarm */

    errFlag = pm_comm_write_byte_mask(rtc_ptr->comm_ptr->slave_id, reg_alrm_en_ctl, 0x80, 0x80, 0 );
    //INTFREE();

    return errFlag;
}

pm_err_flag_type pm_dev_hal_rtc_disable_alarm(uint8 pmic_chip, pm_hal_rtc_alarm  what_alarm)
{
    pm_err_flag_type  errFlag  = PM_ERR_FLAG__SUCCESS;
    pm_rtc_data_type  *rtc_ptr = pm_rtc_get_data(pmic_chip);

    if (rtc_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        pm_register_address_type reg_alrm_en_ctl = (rtc_ptr->megartc_register->base_address) + rtc_ptr->megartc_register->alrm_en_ctl;
        pm_register_address_type reg_alrm_data3 = (rtc_ptr->megartc_register->base_address) + rtc_ptr->megartc_register->alrm_data3;

        // do sanity checking on params ...
        if (what_alarm >= PM_HAL_RTC_ALARM_INVALID)
        {
            return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
        }

        //INTLOCK();

        errFlag = pm_comm_write_byte_mask(rtc_ptr->comm_ptr->slave_id, reg_alrm_en_ctl, 0x80, 0, 0);

        // implemented to avoid RTC Alarm setting off early if and when
        // we disable and then re enable alrm, a previous value set may be older than the current time
        errFlag = pm_comm_write_byte(rtc_ptr->comm_ptr->slave_id, reg_alrm_data3, 0xFF , 0);

        //INTFREE();
    }
    return errFlag;
}

pm_err_flag_type pm_dev_hal_rtc_get_alarm_time(uint8 pmic_chip, pm_hal_rtc_alarm  what_alarm, pm_hal_rtc_time_type  *rtcAlrm)
{
    pm_err_flag_type        errFlag   = PM_ERR_FLAG__SUCCESS;
    pm_rtc_data_type *rtc_ptr = pm_rtc_get_data(pmic_chip);

    if (rtc_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    pm_register_data_type   alrm_val0, alrm_val1, alrm_val2, alrm_val3;
    pm_register_address_type reg_alrm_data0 = (rtc_ptr->megartc_register->base_address) + rtc_ptr->megartc_register->alrm_data0;
    pm_register_address_type reg_alrm_data1 = (rtc_ptr->megartc_register->base_address) + rtc_ptr->megartc_register->alrm_data1;
    pm_register_address_type reg_alrm_data2 = (rtc_ptr->megartc_register->base_address) + rtc_ptr->megartc_register->alrm_data2;
    pm_register_address_type reg_alrm_data3 = (rtc_ptr->megartc_register->base_address) + rtc_ptr->megartc_register->alrm_data3;

    // do sanity checking on params ...
    if (what_alarm >= PM_HAL_RTC_ALARM_INVALID)
    {
        return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
    }
    if (NULL == rtcAlrm)
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }

    //INTLOCK();

    //TransactionManager::StartTransaction(this, false) TBD;
    errFlag = pm_comm_read_byte(rtc_ptr->comm_ptr->slave_id, reg_alrm_data0, &alrm_val0, 0);
    errFlag = pm_comm_read_byte(rtc_ptr->comm_ptr->slave_id, reg_alrm_data1, &alrm_val1, 0);
    errFlag = pm_comm_read_byte(rtc_ptr->comm_ptr->slave_id, reg_alrm_data2, &alrm_val2, 0);
    errFlag = pm_comm_read_byte(rtc_ptr->comm_ptr->slave_id, reg_alrm_data3, &alrm_val3, 0);
    //TransactionManager::EndTransaction(this, false) TBD;
    //INTFREE();

    rtcAlrm->sec = pm_rtc_counter_val_to_time_32_bits(PM_RTC_VAL3_SHIFT, alrm_val3);
    rtcAlrm->sec += pm_rtc_counter_val_to_time_32_bits(PM_RTC_VAL2_SHIFT, alrm_val2);
    rtcAlrm->sec += pm_rtc_counter_val_to_time_32_bits(PM_RTC_VAL1_SHIFT, alrm_val1);
    rtcAlrm->sec += pm_rtc_counter_val_to_time_32_bits(PM_RTC_VAL0_SHIFT, alrm_val0);

    return errFlag;
}

pm_err_flag_type pm_dev_hal_rtc_get_alarm_status(uint8 pmic_chip, uint8  *alarm_status)
{
    pm_err_flag_type    errFlag             = PM_ERR_FLAG__SUCCESS;
    uint8               status_onoff_raw    = 0;
    pm_rtc_data_type *rtc_ptr = pm_rtc_get_data(pmic_chip);

    if (rtc_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    pm_register_address_type reg_alrm_en_ctl = (rtc_ptr->megartc_register->base_address) + rtc_ptr->megartc_register->alrm_en_ctl;

    // do sanity checking on params ...
    if (NULL == alarm_status)
    {
        return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
    }

    //INTLOCK();
    // Status_onoff indicates on (enabled) if equal to "1" and indicates off (disabled) if equal to "0"

    errFlag = pm_comm_read_byte_mask(rtc_ptr->comm_ptr->slave_id, reg_alrm_en_ctl, 0x80, &status_onoff_raw,0);
    //INTFREE();

    *alarm_status = (status_onoff_raw == 0x02) ? 0x01 : 0x00;

    return errFlag;
}

pm_err_flag_type pm_dev_hal_rtc_set_time_adjust(uint8 pmic_chip, uint8  time_adjust)
{
    pm_err_flag_type      errFlag   = PM_ERR_FLAG__SUCCESS;
    pm_rtc_data_type *rtc_ptr = pm_rtc_get_data(pmic_chip);

    if (rtc_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    pm_register_address_type reg_time_adj = (rtc_ptr->megartc_register->base_address) + rtc_ptr->megartc_register->time_adj;

    // do sanity checking on params ...
    /* TODO: Revist this API to fix input parameter "time adjust". This will affect CMI. */
    /* time_adjust should get changed from raw data to doc values. */
    /* Otherwise 0x7F needs to be defined in strucuture for cleaner code */
    /* Unless.. is there a reason why we return the data raw? without using the given equation? */
    if (time_adjust > 0x7F)
    {
        return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
    }

    //INTLOCK();

    errFlag = pm_comm_write_byte_mask(rtc_ptr->comm_ptr->slave_id,  reg_time_adj, 0x7F, time_adjust, 0 );
    //INTFREE();

    return errFlag;
}

pm_err_flag_type pm_dev_hal_rtc_get_time_adjust(uint8 pmic_chip, uint8  *time_adjust_ptr)
{
    pm_err_flag_type      errFlag   = PM_ERR_FLAG__SUCCESS;
    pm_rtc_data_type *rtc_ptr = pm_rtc_get_data(pmic_chip);

    if (rtc_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    pm_register_address_type reg_time_adj = (rtc_ptr->megartc_register->base_address) + rtc_ptr->megartc_register->time_adj;

    // do sanity checking on params ...
    if (NULL == time_adjust_ptr)
    {
        return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
    }

    //INTLOCK();
    // do sanity checking on params ...
    /* TODO: Revist this API to fix input parameter "time adjust". This will affect CMI. */
    /* time_adjust should get changed from raw data to doc values. */
    /* Otherwise 0x7F needs to be defined in strucuture for cleaner code */
    /* Unless.. is there a reason why we return the data raw? without using the given equation? */
    /* TODO: Also bit<7> is reserved and not used for time adjust, but from 7x30, it is included. Is there a reason why? */
    errFlag = pm_comm_read_byte_mask(rtc_ptr->comm_ptr->slave_id, reg_time_adj, 0x7F,  time_adjust_ptr ,0);
    //INTFREE();

    return errFlag;
}

pm_err_flag_type pm_dev_hal_rtc_prescaler_reset(uint8 pmic_chip)
{
   return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
}

pm_err_flag_type pm_dev_hard_reset(uint8 pmic_chip)
{
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
}

/*===========================================================================

                        INTERNAL FUNCTION DEFINITIONS

===========================================================================*/
pm_register_data_type pm_rtc_time_32_bits_to_rtc_counter_val(pm_rtc_val_shift_type  bits_shift, uint32  sec_32_bits)
{
    pm_register_data_type data = 0;

    /* */
    data = (pm_register_data_type) ( sec_32_bits >> bits_shift ) ;
    return (data);
}

uint32 pm_rtc_counter_val_to_time_32_bits(pm_rtc_val_shift_type  bits_shift, pm_register_data_type  rtc_val_8_bits)
{
    uint32 time = 0 ;
    uint32 temp32 = rtc_val_8_bits ;

    /* */
    time = temp32 << bits_shift ;
    return time ;
}


/*===========================================================================

                        DEPRECATED FUNCTIONS

===========================================================================*/
pm_err_flag_type pm_hal_rtc_start(const pm_hal_rtc_time_type *start_time_ptr)
{
    return pm_dev_hal_rtc_start(0, start_time_ptr);
}

pm_err_flag_type pm_hal_rtc_stop()
{
    return pm_dev_hal_rtc_stop(0);
}

pm_err_flag_type pm_hal_rtc_get_time(pm_hal_rtc_time_type *time_ptr)
{
    return pm_dev_hal_rtc_get_time(0, time_ptr);
}

pm_err_flag_type pm_hal_rtc_enable_alarm(pm_hal_rtc_alarm what_alarm, const pm_hal_rtc_time_type  *trigger_time_ptr)
{
    return pm_dev_hal_rtc_enable_alarm(0, what_alarm, trigger_time_ptr);
}

pm_err_flag_type pm_hal_rtc_disable_alarm(pm_hal_rtc_alarm  what_alarm)
{
    return pm_dev_hal_rtc_disable_alarm(0, what_alarm);
}

pm_err_flag_type pm_hal_rtc_get_alarm_time(pm_hal_rtc_alarm what_alarm, pm_hal_rtc_time_type  *alarm_time_ptr)
{
    return pm_dev_hal_rtc_get_alarm_time(0, what_alarm, alarm_time_ptr);
}

pm_err_flag_type pm_hal_rtc_get_alarm_status(uint8 *status_ptr)
{
    return pm_dev_hal_rtc_get_alarm_status(0, status_ptr);
}

pm_err_flag_type pm_hal_rtc_set_time_adjust(uint8 time_adjust)
{
    return pm_dev_hal_rtc_set_time_adjust(0, time_adjust);
}

pm_err_flag_type pm_hal_rtc_get_time_adjust(uint8 *time_adjust_ptr)
{
    return pm_dev_hal_rtc_get_time_adjust(0, time_adjust_ptr);
}

pm_err_flag_type pm_hal_rtc_prescaler_reset()
{
    return pm_dev_hal_rtc_prescaler_reset(0);
}
