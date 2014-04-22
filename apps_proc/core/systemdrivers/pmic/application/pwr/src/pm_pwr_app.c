/*! \file pm_pwr_app.c
*  \n
*  \brief Pwr Application APIs implementation.
*  \n  
*  \n &copy; Copyright 2013 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                            Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/systemdrivers/pmic/application/pwr/src/pm_pwr_app.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/09/14   vk      vdd_mss API change to include setting time and support ON/OFF feature
10/07/13   rh      File created
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "hw_module_type.h"
#include "pm_pbs_client.h"
#include "pmapp_pwr.h"
#include "pm_target_information.h"
#include "pm_resources_and_types.h"
#include "pm_version.h"
#include "pm_npa.h"
#include "DDITimetick.h"
#include "pm_ulog.h"
#include "pm_ldo_driver.h"
#include "pm_smps_driver.h"
#include "pm_malloc.h"
#include "CoreVerify.h"
#include "busywait.h"


static pm_pwr_resource_info_type* apps_rail = NULL;
static boolean  configure_once = FALSE;

#define PM_STATUS_REG_POLL_INTERVAL             25     /* us time slice to poll the VREG status flag after */ 
#define PM_SMPS_ENABLE_MIN_SETTLE_TIME_US      120    /* SMPS min settling time to wait after turning ON the rail */
#define PM_SMPS_VOLT_MIN_SETTLE_TIME_US         25    /* SMPS min settling time to wait after voltage change on this rail */
#define SYS_CLK_FREQ_KHZ                        19200    /* System Clock Freq of 19.2 Mhz(Could change in future targets)*/
#define STEPPER_VS_CTL_CLOCK_BASE               20    /* Min No. Clock Cycles in STEPPER_VS_CTL Register used to calculate stepper rate */              


/*
 * Structure to hold the Power rails Diag runtime variables
 */
static struct
{
  DalTimetickTime64Type     start_settling_time;                              /**< start settling time */
  DalTimetickTime64Type     end_settling_time;                                /**< end settling time */
  uint64                    actual_settling_time_usec;                        /**< actual settling time in usec */
  uint32                    apps_time_taken_to_settle;                         /**< apps time taken to settle in usec */                          
  uint32                    calculated_settling_time_usec;                    /**< calculated settling time based on formula in usec */
  uint32                    apps_settling_time_us;                             /**< settling time if changed in different conditions in usec */
  uint32                    settling_loop_count;                              /**< vreg_ok polling loop count */
  uint32                    vreg_ok_err_count;                                 /**< vreg_ok error count */
  pm_err_flag_type          err_flag_vreg_ok;                                 /**< vreg_ok error flag */
  pm_on_off_type            prev_rail_status;                                 /**< prev_ON/OFF rail_status */
  pm_on_off_type            current_rail_status;                              /**< current_ON/OFF rail_status */
  uint32                    prev_voltage;                                     /**< previous voltage */
  uint32                    current_voltage;                                  /**< current voltage request*/
  pm_err_flag_type          returned_err_flag;                                /**< current voltage request*/
  pm_pbs_client_type*       apps_pbs_level_trigger_sequence;
  pm_pbs_client_type*       apps_pbs_ocp_trigger_sequence;
  DalDeviceHandle           *timetick_handle;                                 /**< handle for getting system time stamp */  
  uint8                     model;      
  pm_pwr_data_type          *pwr_data;
  pm_comm_info_type         *comm_ptr;
  pm_smps_data_type         *smps_ptr;
  pm_ldo_data_type          *ldo_ptr;
  uint32                    apps_stepper_rate;
  boolean                   external_manage_flag;
} apps_rail_status;


/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/

static pm_err_flag_type pmapp_pwr_apps_get_stepper_rate(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8 peripheral_index, uint32 *apps_stepper_rate_uV_per_uS);
static pm_err_flag_type pmapp_pwr_apps_configure_once(void);
static void pmapp_pwr_clear_apps_status(void);



pm_err_flag_type pmapp_pwr_set_vdd_apps(uint32 voltage) 

{
/*Initialize variables*/
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_err_flag_type err_flag_vreg_ok = PM_ERR_FLAG__SUCCESS;
    pm_on_off_type vreg_ok_status = PM_OFF;
	pm_on_off_type on_off=PM_ON;

/*Check if external manage mode is set */
    if (apps_rail_status.external_manage_flag==TRUE)
    {
      err_flag=PM_ERR_FLAG__EXTERNAL_MANAGE_MODE;   
       PM_LOG_MSG_INFO("apps rail managed by MVC");  
       return err_flag;                             
    }

    
/*Update Rail Status and Rail Voltage variables*/
    apps_rail_status.prev_rail_status = apps_rail_status.current_rail_status;                                     
    apps_rail_status.prev_voltage = apps_rail_status.current_voltage;
    apps_rail_status.current_voltage = voltage; 
    apps_rail_status.current_rail_status=PM_INVALID;
            
/*Clear apps_rail_status variables*/
    pmapp_pwr_clear_apps_status();
    
/*Configure apps Rail Once*/
    if(configure_once == FALSE)
    {
        err_flag |= pmapp_pwr_apps_configure_once();
    } //run_once
        
/*Get the VREG enable status if previously Rail status was off as RPM Could have turned it ON*/
    if((apps_rail_status.prev_rail_status==PM_OFF) && (on_off==PM_ON))
       {
        err_flag |= pm_pwr_sw_enable_status_alg(apps_rail_status.pwr_data, apps_rail_status.comm_ptr, apps_rail->resource_index - 1, &apps_rail_status.prev_rail_status);
        if (err_flag != PM_ERR_FLAG__SUCCESS)
        {
            apps_rail_status.returned_err_flag = err_flag;
           PM_LOG_MSG_ERR_FATAL("Failed to read apps vreg_enable status"); 
        }
    }
/*Conditions to Enable, Disable and Set Voltage*/
    /*Condition 1: For turning apps rail OFF*/
    if((voltage==0) || (on_off==PM_OFF))      
        {
        /* Disable apps */
        if(apps_rail_status.prev_rail_status==PM_ON)
        {
        err_flag |= pm_pwr_sw_enable_alg(apps_rail_status.pwr_data, apps_rail_status.comm_ptr, apps_rail->resource_index - 1, PM_OFF);
        CORE_VERIFY(err_flag == PM_ERR_FLAG__SUCCESS);  
        }
        
        if((voltage>0) && (apps_rail_status.prev_voltage != apps_rail_status.current_voltage))
        {
        err_flag |= pm_pwr_volt_level_alg(apps_rail_status.pwr_data, apps_rail_status.comm_ptr, apps_rail->resource_index - 1, voltage);
        apps_rail_status.returned_err_flag = err_flag;
        CORE_VERIFY(err_flag == PM_ERR_FLAG__SUCCESS);      
        }
    }
        
    /*Condition 2: For changing Voltage level if apps Rail is already ON*/
   else if((apps_rail_status.prev_rail_status == PM_ON) && (on_off==PM_ON)) 
	     {       
        //If current and previous voltages are same, then dont set voltage level register to save timeline hit 
        if(apps_rail_status.prev_voltage != apps_rail_status.current_voltage)
        {
            err_flag |= pm_pwr_volt_level_alg(apps_rail_status.pwr_data, apps_rail_status.comm_ptr, apps_rail->resource_index - 1, voltage);
            apps_rail_status.returned_err_flag = err_flag;
            CORE_VERIFY(err_flag == PM_ERR_FLAG__SUCCESS);
        }   
    }
    
    /*Condition 3: For Enabling and Setting Voltage if apps is OFF*/
    /*Default condition (apps_rail_status.prev_rail_status=0 and (voltage != 0 or on_off==ON))*/
    else{
        //set new voltage level
        err_flag |= pm_pwr_volt_level_alg(apps_rail_status.pwr_data, apps_rail_status.comm_ptr, apps_rail->resource_index - 1, voltage);
        apps_rail_status.returned_err_flag = err_flag;
        CORE_VERIFY(err_flag == PM_ERR_FLAG__SUCCESS);
        err_flag |= pm_pwr_sw_enable_alg(apps_rail_status.pwr_data, apps_rail_status.comm_ptr, apps_rail->resource_index - 1, PM_ON);   
    }
        
/*Settling Conditions*/
     if((on_off == PM_ON) && (voltage != 0))      
          {
        /* Settling start time in ticks. */
        DalTimetick_GetTimetick64(apps_rail_status.timetick_handle, &apps_rail_status.start_settling_time);
        /*Calculate setteling time*/
        if(voltage>=apps_rail_status.prev_voltage)
            apps_rail_status.calculated_settling_time_usec  = (voltage - apps_rail_status.prev_voltage)/apps_rail_status.apps_stepper_rate;
        else
            apps_rail_status.calculated_settling_time_usec  = (apps_rail_status.prev_voltage - voltage)/apps_rail_status.apps_stepper_rate;
        
        apps_rail_status.apps_settling_time_us= (apps_rail_status.calculated_settling_time_usec < PM_SMPS_VOLT_MIN_SETTLE_TIME_US) ? PM_SMPS_VOLT_MIN_SETTLE_TIME_US : apps_rail_status.calculated_settling_time_usec;
        //If apps rail was OFF minimum settling time needed is 125uS
        if((apps_rail_status.prev_rail_status == PM_OFF) && (apps_rail_status.apps_settling_time_us < PM_SMPS_ENABLE_MIN_SETTLE_TIME_US))
        {  
            apps_rail_status.apps_settling_time_us =  PM_SMPS_ENABLE_MIN_SETTLE_TIME_US;   
        }
        /*end of calculate settling time */
        
        /* Wait for the calculated settling time */
        /*if rail was previously off wait for 125uS(min enable wait time)*/
        if((apps_rail_status.prev_rail_status==PM_OFF) || (apps_rail_status.prev_voltage != apps_rail_status.current_voltage)) 
        {
            busywait(apps_rail_status.apps_settling_time_us);
            apps_rail_status.apps_time_taken_to_settle = apps_rail_status.apps_settling_time_us;
        }
        //if prev_status is PM_ON and PrevVoltage=CurrentVoltage then no settling time needed
        else 
            apps_rail_status.apps_time_taken_to_settle =0;
                
        while(apps_rail_status.apps_time_taken_to_settle <= (apps_rail_status.apps_settling_time_us*10))
        {//end while if time taken to settle exceeds 10times of settling time
            /* Get the VREG_OK status for apps */
            err_flag_vreg_ok=pm_pwr_is_vreg_ready_alg(apps_rail_status.pwr_data, apps_rail_status.comm_ptr, apps_rail->resource_index - 1, &vreg_ok_status);   
            if(vreg_ok_status == PM_ON)
                break;
            else 
            {
                apps_rail_status.settling_loop_count++;
                busywait(PM_STATUS_REG_POLL_INTERVAL);
                apps_rail_status.apps_time_taken_to_settle += PM_STATUS_REG_POLL_INTERVAL;
                if(PM_ERR_FLAG__SUCCESS != err_flag_vreg_ok)
                {
                    apps_rail_status.vreg_ok_err_count++;
                    apps_rail_status.err_flag_vreg_ok = err_flag;
                }
            }
        }       
        apps_rail_status.current_rail_status=vreg_ok_status;
        
        if ((vreg_ok_status != PM_ON) && (apps_rail_status.apps_time_taken_to_settle > (apps_rail_status.apps_settling_time_us*10)))
        {
           /* Settling end time in ticks. */
           DalTimetick_GetTimetick64(apps_rail_status.timetick_handle, &apps_rail_status.end_settling_time);
           DalTimetick_CvtFromTimetick64(apps_rail_status.timetick_handle,
                              (apps_rail_status.end_settling_time - apps_rail_status.start_settling_time),  
                                T_USEC, &apps_rail_status.actual_settling_time_usec);
           apps_rail_status.current_rail_status = PM_OFF;
            PM_LOG_MSG_ERR_FATAL("apps Rail Settling Failure");  
        }   
    }

    /*No Settling time if rail is turning off*/ 
    if((on_off == PM_OFF) || (voltage == 0))  
        {   
        apps_rail_status.current_rail_status = PM_OFF;
    }
    
    PM_LOG_MSG_INFO(
        "prev_status=%d, current_status=%d, prev_voltage=%d, current_voltage=%d, settling time=%d",
        apps_rail_status.prev_rail_status, apps_rail_status.current_rail_status, apps_rail_status.prev_voltage, 
        apps_rail_status.current_voltage, apps_rail_status.apps_time_taken_to_settle);                          
        
    apps_rail_status.returned_err_flag = err_flag;
    
    PM_LOG_MSG_ERROR(err_flag, "error = %d", err_flag);  
    
    return err_flag;

}


/******************************************************************************************************************************************/
/*  Get VDD apps Status API                                                                                                                */
/******************************************************************************************************************************************/


pm_err_flag_type pmapp_pwr_get_vdd_apps_status(uint32 *voltage, pm_on_off_type *on_off, pmapp_rail_status_type *get_status)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    
    /*prepare first run of apps Rail*/
    if(configure_once == FALSE)
    {
        err_flag=pmapp_pwr_apps_configure_once();
    } //run_once
    err_flag |= pm_pwr_volt_level_status_alg(apps_rail_status.pwr_data, apps_rail_status.comm_ptr, apps_rail->resource_index - 1,  voltage);
    err_flag |= pm_pwr_sw_enable_status_alg(apps_rail_status.pwr_data, apps_rail_status.comm_ptr, apps_rail->resource_index - 1, on_off);
    get_status->prev_rail_status=apps_rail_status.prev_rail_status;
    get_status->current_rail_status=apps_rail_status.current_rail_status;
    get_status->current_voltage=apps_rail_status.current_voltage;
    get_status->prev_voltage=apps_rail_status.prev_voltage;
    get_status->settling_time=apps_rail_status.apps_time_taken_to_settle;
    return err_flag;
}

/******************************************************************************************************************************************/
/*  VDD apps EXTERNAL MANAGE                                                                                                       */
/******************************************************************************************************************************************/
pm_err_flag_type pmapp_pwr_vdd_apps_external_manage (boolean flag)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pmapp_rail_status_type apps_status;
    
    apps_rail_status.external_manage_flag = flag;
    if (flag==FALSE){
        //read apps rail voltage and status and populate apps_rail_status accordingly
        err_flag=pmapp_pwr_get_vdd_apps_status(&(apps_rail_status.current_voltage), &(apps_rail_status.current_rail_status), &apps_status); 
        PM_LOG_MSG_INFO("Exiting External Manage Mode");  
    }
    if (flag==TRUE){
        //clear params in apps_rail_status structure  
        apps_rail_status.prev_rail_status=PM_OFF;
        apps_rail_status.prev_voltage=0;
        apps_rail_status.current_rail_status=PM_OFF;
        apps_rail_status.current_voltage=0;
       PM_LOG_MSG_INFO("Entering External Manage Mode");  
    }
    return err_flag;
}


/*Helper Functions*/

/******************************************************************************************************************************************/
/*Get Stepper Rate: Calculates apps stepper rate based on STEPPER_VS_CTL Register                                                          */    
/******************************************************************************************************************************************/
static pm_err_flag_type pmapp_pwr_apps_get_stepper_rate(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8 peripheral_index, uint32 *apps_stepper_rate_uV_per_uS)
{

    pm_register_address_type reg = 0; 
    pm_register_data_type data =0;
    uint32 no_of_clock_cycles=0;
    uint8 LSB=0;
    uint32 sys_freq_khz=SYS_CLK_FREQ_KHZ;
    uint32 step_size_uV = *((uint32*)apps_rail->data3);
    uint32 configured_step_size_uV=0;
    uint32 uS_to_configured_step_size=0;
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    
    if(pwr_data->pwr_reg_table == NULL){return PM_ERR_FLAG__INVALID_POINTER;}
    
    //get register address
    reg = (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->STEPPER_VS_CTL);

    //read register 
    err_flag = pm_comm_read_byte(comm_ptr->slave_id, reg, &data, 0);

    //check error flag
    if(err_flag != PM_ERR_FLAG__SUCCESS) 
    {
        return err_flag;
    }
    
    //Calculate no. of clock cycles, multiply by 1000 for Accuracy 
    no_of_clock_cycles=STEPPER_VS_CTL_CLOCK_BASE*(1<<(data & 0x7));
    no_of_clock_cycles*=1000; //multiply by 1000 to maintain accuracy
    
    //Get LSB
    LSB=(data & 0x18)>>3; //LSB Part in STEPPER_VS_CTL_Register
    //calculate configured step size based on LSB 
    configured_step_size_uV=step_size_uV*(1<<LSB);  
    
    //uS taken to reach step size configured using LSB in VS_CTL Register
    uS_to_configured_step_size=no_of_clock_cycles/sys_freq_khz; 
    
    //uV increase per uS
    *apps_stepper_rate_uV_per_uS=configured_step_size_uV/uS_to_configured_step_size;
    
    return err_flag;
}

/******************************************************************************************************************************************/
/*pmapp_pwr_apps_confgure_once: Initialization steps for pmapp_pwr_set_vdd_apps API                                                         */
/******************************************************************************************************************************************/
static pm_err_flag_type pmapp_pwr_apps_configure_once(void)
{
    static uint8 pmic_index = 0;
    const char* timetick_dev_str = "SystemTimer";
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

    /* Get apps rail data from DAL CONFIG */
    apps_rail = (pm_pwr_resource_info_type*)pm_target_information_get_specific_info(PM_PROP_APPS);
    CORE_VERIFY_PTR(apps_rail);
    
    /*Update apps_rail_status struct variables*/
    apps_rail_status.model = pm_get_pmic_model(0);
    apps_rail_status.timetick_handle = NULL;
    apps_rail_status.apps_pbs_level_trigger_sequence = (pm_pbs_client_type*)apps_rail->data1;
    apps_rail_status.apps_pbs_ocp_trigger_sequence = (pm_pbs_client_type*)apps_rail->data2;
    
    /*Verify pbs_level_trigger and pbs_ocp_trigger pointers */
    if(apps_rail_status.model==PMIC_IS_PMD9635) 
    {       
        CORE_VERIFY_PTR(apps_rail_status.apps_pbs_level_trigger_sequence);
        CORE_VERIFY_PTR(apps_rail_status.apps_pbs_ocp_trigger_sequence);  
    }         
    
    if ( ( apps_rail->resource_type == RPM_SMPS_B_REQ) || (apps_rail->resource_type == RPM_LDO_B_REQ) )
    {
        pmic_index = 1;
    }

    // Apply supplied voltage to apps rail. Note that in target config where we get the apps rail info
    // from the regulator indices are 1-indexed while driver code expects 0-indexed resource ids.
    // Subtract resource_index by 1 to account for this.

    if((apps_rail->resource_type==RPM_SMPS_A_REQ) || (apps_rail->resource_type==RPM_SMPS_B_REQ)) 
       {
        apps_rail_status.smps_ptr = pm_smps_get_data(pmic_index);
        if(apps_rail_status.smps_ptr != NULL)
        {
            apps_rail_status.pwr_data = &((apps_rail_status.smps_ptr)->pm_pwr_data); 
            apps_rail_status.comm_ptr = ((apps_rail_status.smps_ptr)->comm_ptr);
        }
    }
    if((apps_rail->resource_type==RPM_LDO_A_REQ) || (apps_rail->resource_type==RPM_LDO_B_REQ))
    {
        apps_rail_status.ldo_ptr = pm_ldo_get_data(pmic_index);
        if(apps_rail_status.ldo_ptr != NULL)
        {
            apps_rail_status.pwr_data = &((apps_rail_status.ldo_ptr)->pm_pwr_data); 
            apps_rail_status.comm_ptr = ((apps_rail_status.ldo_ptr)->comm_ptr);
        }
    }
    CORE_VERIFY_PTR(apps_rail_status.pwr_data);
    CORE_VERIFY_PTR(apps_rail_status.comm_ptr);

    /*get apps_stepper_rate*/    
    err_flag=pmapp_pwr_apps_get_stepper_rate(apps_rail_status.pwr_data, apps_rail_status.comm_ptr, apps_rail->resource_index - 1, &(apps_rail_status.apps_stepper_rate));

    if (DAL_SUCCESS != DalTimetick_Attach(timetick_dev_str, &apps_rail_status.timetick_handle))
    {
        /* Update state to indicate DAL ERROR */
         PM_LOG_MSG_ERR_FATAL("Failed to get DAL Timetick Handle"); 
    }
    
    /*Get initial apps Rail Status*/
    err_flag |= pm_pwr_sw_enable_status_alg(apps_rail_status.pwr_data, apps_rail_status.comm_ptr, apps_rail->resource_index - 1, &apps_rail_status.prev_rail_status);
    if (err_flag != PM_ERR_FLAG__SUCCESS)
    {
         apps_rail_status.returned_err_flag = err_flag;
         PM_LOG_MSG_ERR_FATAL("Failed to read apps vreg_enable status");
    }
    /*Get initial apps Rail Voltage */
    err_flag |= pm_pwr_volt_level_status_alg(apps_rail_status.pwr_data, apps_rail_status.comm_ptr, apps_rail->resource_index - 1,  &apps_rail_status.prev_voltage);
    if (err_flag != PM_ERR_FLAG__SUCCESS)
    {
        apps_rail_status.returned_err_flag = err_flag;
         PM_LOG_MSG_ERR_FATAL("Failed to read apps volt level status"); 
    }
    configure_once = TRUE;
   return err_flag; 
}//end pmapp_pwr_apps_configure_once

/******************************************************************************************************************************************/
/*pmapp_pwr_clear_apps_status: Clears some variables of apps_rail_status struct                                                             */
/******************************************************************************************************************************************/
static void pmapp_pwr_clear_apps_status(void)
{
    apps_rail_status.start_settling_time = 0;
    apps_rail_status.end_settling_time = 0;
    apps_rail_status.actual_settling_time_usec = 0;
    apps_rail_status.calculated_settling_time_usec = 0;                       
    apps_rail_status.apps_time_taken_to_settle = 0;
    apps_rail_status.apps_settling_time_us = 0;
    apps_rail_status.settling_loop_count = 0;
    apps_rail_status.vreg_ok_err_count = 0;    
    apps_rail_status.returned_err_flag = PM_ERR_FLAG__SUCCESS;                             
    apps_rail_status.err_flag_vreg_ok = PM_ERR_FLAG__SUCCESS; 
}

/******************************************************************************************************************************************/
/*pmapp_pwr_apps_volt_level_info: Passes volt level to be set in volt_ctl2 register for apps rail and the pm_apps_config_info for MVC        */
/******************************************************************************************************************************************/

pm_err_flag_type pmapp_pwr_apps_volt_level_info(uint32 voltage, pm_rail_configs_type *pm_apps_rail_configs){
    pm_err_flag_type err_flag=PM_ERR_FLAG__SUCCESS;
    static pm_rail_cfg_info_type *pm_apps_rail_info_temp;
    static pm_rail_cfg_info_type *pm_apps_rail_info_alloc_mem;
    uint32 vset[2] = {0}; //register values for the voltage to be set
    static boolean pm_external_data_init = FALSE;
    uint8 peripheral_index = (apps_rail->resource_index - 1);
    uint8 i =0;
    
    if(pm_apps_rail_configs == NULL){
        return PM_ERR_FLAG__INVALID_POINTER;
    }
    
    if (pm_external_data_init==FALSE){      
        pm_apps_rail_info_temp = (pm_rail_cfg_info_type*)pm_target_information_get_specific_info(PM_PROP_APPS_CFG_INFO);
        CORE_VERIFY_PTR(pm_apps_rail_info_temp);
        pm_malloc((PM_APPS_CFG_COUNT * sizeof(pm_rail_cfg_info_type)), ((void**)&(pm_apps_rail_info_alloc_mem))); 
        CORE_VERIFY_PTR(pm_apps_rail_info_alloc_mem);
        pm_external_data_init=TRUE;
    }
    
    if(configure_once==FALSE)
    {
        pmapp_pwr_apps_configure_once();
    }
    
    for(i=0; i<PM_APPS_CFG_COUNT;i++){
        pm_apps_rail_info_alloc_mem[i].valid = pm_apps_rail_info_temp[i].valid;
        pm_apps_rail_info_alloc_mem[i].data = pm_apps_rail_info_temp[i].data;
    }
    
    
    err_flag = pm_pwr_volt_calculate_vset_alg(apps_rail_status.pwr_data, apps_rail_status.comm_ptr, peripheral_index, voltage, vset);
    
    pm_apps_rail_configs->nNumConfigs = PM_APPS_CFG_COUNT;
    
    pm_apps_rail_info_alloc_mem[0].data = vset[0];
    
    if(apps_rail_status.pwr_data->pwr_specific_info[peripheral_index].is_periph_unified_reg ==  TRUE)
    {
        pm_apps_rail_info_alloc_mem[1].data = vset[1];
    }
    
    pm_apps_rail_configs->pm_rail_cfg_info   =   pm_apps_rail_info_alloc_mem;
    
    PM_LOG_MSG_INFO(
        "Voltage=%d, RegValue=%d",
        voltage, pm_apps_rail_info_alloc_mem[PM_APPS_VOLT_LEVEL].data);    
    
    return err_flag;
}


