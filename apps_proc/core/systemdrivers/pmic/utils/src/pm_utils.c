/*! \file
*  
*  \brief  pm_utils.c ----This file contains the implementation of utils functions.
*  \details This file contains the implementation of utils functions.
*  
*  &copy; Copyright 2013 Qualcomm Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.

$Header: //components/rel/core.tx/6.0/systemdrivers/pmic/utils/src/pm_utils.c#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/20/15   vk      Created

===========================================================================*/

/*===========================================================================

INCLUDE FILES 

===========================================================================*/
#include "pm_utils.h"
#include "DALSys.h"
#include "DALDeviceId.h"
#include "DDIAdc.h"
#include "CoreVerify.h"
#include "DDIChipInfo.h"
#include "pm_version.h"

/*===========================================================================

                        GLOBAL VARIABLES DEFINITIONS

===========================================================================*/

static boolean          g_adc_init = FALSE;  
static DalDeviceHandle *g_adc_handle;


/*===========================================================================

                FUNCTION PROTOTYPES

===========================================================================*/

static pm_err_flag_type pm_utils_adc_init(void);                  /*ADC Initialization*/

/*
  @name 
      pm_utils_adc_get_value
 
  @description
     This function returns the adc results based on given channel  
  
  @param 
      chnl: pass the channels mentioned in AdcInputs.h file only 
*/
static pm_err_flag_type pm_utils_adc_get_value(char *chnl, AdcResultType *adc_result); 

/* ================================================================================================================ */


//This API is applicable for both PON GEN1 and GEN2 peripherals
uint8
pm_utils_get_upper_idx(uint32 data_value, uint32 *data_list, uint8 list_count)
{
  uint8 timer_id = list_count - 1, i;

  /* Calc timer id */
  if (data_value == 0)
  {
    timer_id = 0;
  }
  else if (data_value == data_list[list_count - 1])
  {
    timer_id = list_count - 1;
  }
  else
  {
    /* Room for improvement if the list gets too long */
    for (i = 0; i < list_count - 1; i++)
    {
      if (data_value > data_list[i] && data_value <= data_list[i+1])
      {
        timer_id = i+1;
        break;
      }
    }
  }
  return timer_id;
}

pm_err_flag_type pm_utils_adc_init()
{
  DALResult result = DAL_SUCCESS;
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;   
  
  result = DAL_AdcDeviceAttach(DALDEVICEID_ADC, &g_adc_handle);
  if (result != DAL_SUCCESS)
  {
    err_flag = PM_ERR_FLAG__ADC_INVALID_RES;   //Invalid response from the ADC
  }
  else
  {
    g_adc_init = TRUE;
  }
  return err_flag;
}

pm_err_flag_type pm_utils_get_battery_id(uint32* battery_id)
{
  AdcResultType          adc_result;
  pm_err_flag_type       err_flag  = PM_ERR_FLAG__SUCCESS;
     
  CORE_VERIFY_PTR(battery_id);             // Pointer NULL check
  
  err_flag = pm_utils_adc_get_value(ADC_INPUT_BATT_ID,&adc_result);
  if(PM_ERR_FLAG__SUCCESS != err_flag)
  {
    return err_flag;
  }
  /*Check for the result*/
  if(ADC_RESULT_VALID == adc_result.eStatus)  
  {
    *battery_id = adc_result.nPhysical;
  }
  else
  {
    err_flag = PM_ERR_FLAG__ADC_INVALID_RES; // BAT ID Read Error
  }

  return err_flag;
}

pm_err_flag_type pm_utils_get_battery_volt_level(uint32* battery_voltage)
{
  AdcResultType          adc_result;
  pm_err_flag_type       err_flag  = PM_ERR_FLAG__SUCCESS;
     
  CORE_VERIFY_PTR(battery_voltage);             // Pointer NULL check
  
  err_flag = pm_utils_adc_get_value(ADC_INPUT_VBATT,&adc_result);
  if(PM_ERR_FLAG__SUCCESS != err_flag)
  {
    return err_flag;
  }
  /*Check for the result*/
  if(ADC_RESULT_VALID == adc_result.eStatus)  
  {
    *battery_voltage = (uint32)(adc_result.nMicrovolts/1000);
  }
  else
  {
    err_flag = PM_ERR_FLAG__ADC_INVALID_RES; // Vbatt Read Error
  }
  return err_flag;
}

pm_err_flag_type pm_utils_adc_get_value(char *chnl,AdcResultType *adc_result) 
{
  DALResult              result        = DAL_SUCCESS;
  AdcInputPropertiesType adc_channel;
  uint32                 InputNameSize = 0;
  pm_err_flag_type       err_flag 	 = PM_ERR_FLAG__SUCCESS;
     
  CORE_VERIFY_PTR(adc_result);             // Pointer NULL check
  CORE_VERIFY_PTR(chnl);                  // Pointer NULL check
   
  InputNameSize = strlen(chnl) + 1;  
  if(g_adc_init == FALSE)                       // Checking the ADC Initialization
  {
  	err_flag = pm_utils_adc_init();        //Initialize the ADC
  
  	if(err_flag!= PM_ERR_FLAG__SUCCESS)
  	{
  		err_flag = PM_ERR_FLAG__VBATT_INTERROGATION_FAILED;    //ADC does not exist.
             return err_flag;
  	}
  }
  memset(&adc_channel, 0, sizeof(adc_channel));
  /*Get the channel from where the data is needed*/
  result = DalAdc_GetAdcInputProperties(g_adc_handle, chnl, InputNameSize, &adc_channel);
  if (result != DAL_SUCCESS)
  {
  	err_flag = PM_ERR_FLAG__ADC_INVALID_RES;          // Invalid response from the ADC
  }
  else
  {
    memset(adc_result, 0, sizeof(*adc_result));
    /*Read the Voltage of the Battery*/
    result = DalAdc_Read(g_adc_handle, &adc_channel, adc_result);
    if (result != DAL_SUCCESS)
    {
      err_flag = PM_ERR_FLAG__ADC_VBATT_READ_ERR;  // Vbatt Read Error
    }
  }
  return err_flag;
}

