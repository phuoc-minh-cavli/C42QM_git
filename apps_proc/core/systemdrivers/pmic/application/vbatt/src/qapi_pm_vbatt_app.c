/*! \file
*  
*  \brief  qapi_pm_vbatt_app.c
*  \details Implementation file for vbatt information.
*    
*  &copy; Copyright 2023 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================
                                Edit History
 
when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/11/17   kcs     Updated the file for more Battery management API's (CR -2110414).
5/29/17   kcs      created.

===========================================================================*/

/*===========================================================================

                    INCLUDE FILES 

===========================================================================*/
#include "qapi_pmapp_vbatt.h"
#include "pm_err_flags.h"
#include "pmapp_vbatt.h"
#include "qurt_txm_qapi_fwk.h"
#include "qurt_memory.h"

/*===========================================================================

                   FUNCTION IMPLEMENTATION 

===========================================================================*/

qapi_Status_t qapi_PM_Vbatt_handler(UINT id, UINT a, UINT b, UINT c, UINT d, UINT e, 
                         UINT f, UINT g, UINT h, UINT i, UINT j, UINT k, UINT l)
{
   int             is_kernel_address = 0;
   qapi_Status_t   retval            = 0;
 
  if((a == NULL)
#ifdef FEATURE_PMIC_CHARGER
  && (id != TXM_QAPI_PMIC_VBATT_GET_CHARGE_REG) && (id != TXM_QAPI_PMIC_VBATT_SET_CHARGE_REG)
#endif
  )  // pointer NULL check
  {
    return QAPI_ERR_INVALID_PARAM; 
  }

  switch(id)
  {
    case TXM_QAPI_PMIC_VBATT_GET_BATTERY_STATUS:
        retval = qapi_Pmapp_Vbatt_Get_Battery_Status((uint8*)a);
        break;
    case TXM_QAPI_PMIC_VBATT_GET_BATTERY_HEALTH:
        retval = qapi_Pmapp_Vbatt_Get_Battery_Health((qapi_PM_Battery_Health_t*)a);
        break;
    case TXM_QAPI_PMIC_VBATT_GET_BATTERY_TEMPARATURE:
        retval = qapi_Pmapp_Vbatt_Get_Battery_Temparature((qapi_PM_Battery_Temperature_t*)a);
        break;
    case TXM_QAPI_PMIC_VBATT_GET_BATTERY_TECHNOLOGY:
        retval = qapi_Pmapp_Vbatt_Get_Battery_Technology((qapi_PM_Battery_Technology_t*)a);
        break;
    case TXM_QAPI_PMIC_VBATT_GET_BATTERY_CHARGE_STATUS:
        retval = qapi_Pmapp_Vbatt_Get_Battery_Charge_Status((qapi_PM_Battery_Chg_Status_t*)a);
        break;
    case TXM_QAPI_PMIC_VBATT_GET_BATTERY_CHARGER_SOURCE:
        retval = qapi_Pmapp_Vbatt_Get_Battery_Charger_Source((qapi_PM_Battery_Chg_Src_t*)a);
        break;
#ifdef FEATURE_PMIC_CHARGER
	case TXM_QAPI_PMIC_VBATT_GET_BATTERY_VOLT_STATUS:
        retval = qapi_Pmapp_Vbatt_Get_Battery_Volt_Status((uint32*)a);
        break;
    case TXM_QAPI_PMIC_VBATT_GET_CHARGE_REG:
        retval = qapi_Pmapp_Vbatt_get_charge_reg((uint8)a,(uint8*)b);
        break;
    case TXM_QAPI_PMIC_VBATT_SET_CHARGE_REG:
        retval = qapi_Pmapp_Vbatt_set_charge_reg((uint8)a,(uint8)b);
        break;
    case TXM_QAPI_PMIC_VBATT_CONFIG_SYSOK_OPTION:
        retval = qapi_Pmapp_Vbatt_config_sysok_option((uint8)a);
#endif
    default:
      break;
	  
  }
  return (UINT)retval;
}

qapi_Status_t qapi_Pmapp_Vbatt_Get_Battery_Status(uint8 *qapi_batt_stat)
{
    qapi_Status_t qapi_err_flag  = QAPI_OK;
	pm_err_flag_type pm_err_flag = PM_ERR_FLAG__SUCCESS;

	pm_err_flag = pmapp_vbatt_get_battery_status((uint8*)qapi_batt_stat);
    if (pm_err_flag == PM_ERR_FLAG__SUCCESS)
    {
		qapi_err_flag = QAPI_OK;
    }
    else if (pm_err_flag == PM_ERR_FLAG__FEATURE_NOT_SUPPORTED)
    {
		qapi_err_flag = QAPI_ERR_NOT_SUPPORTED;
    }
    else if (pm_err_flag == PM_ERR_FLAG__PAR1_OUT_OF_RANGE)
    {
		qapi_err_flag = QAPI_ERR_INVALID_PARAM;
    }
    
    else if (pm_err_flag == PM_ERR_FLAG__BATTERY_MISSING)
    {
		qapi_err_flag = QAPI_ERR_BATT_ABSENT;
    }
    
    else 
    {
		qapi_err_flag = QAPI_ERROR;
    }
   
    return qapi_err_flag;
}

qapi_Status_t qapi_Pmapp_Vbatt_Get_Battery_Health(qapi_PM_Battery_Health_t *qapi_batt_health)
{
    qapi_Status_t qapi_err_flag  = QAPI_OK;
	pm_err_flag_type pm_err_flag = PM_ERR_FLAG__SUCCESS;

	pm_err_flag = pmapp_get_battery_health((pm_battery_health* )qapi_batt_health);
    if (pm_err_flag == PM_ERR_FLAG__SUCCESS)
    {
		qapi_err_flag = QAPI_OK;
    }
    else if (pm_err_flag == PM_ERR_FLAG__FEATURE_NOT_SUPPORTED)
    {
		qapi_err_flag = QAPI_ERR_NOT_SUPPORTED;
    }
    else if (pm_err_flag == PM_ERR_FLAG__PAR1_OUT_OF_RANGE)
    {
		qapi_err_flag = QAPI_ERR_INVALID_PARAM;
    }
    else if (pm_err_flag == PM_ERR_FLAG__BATTERY_MISSING)
    {
		qapi_err_flag = QAPI_ERR_BATT_ABSENT;
    }

    else 
    {
		qapi_err_flag = QAPI_ERROR;
    }
   
    return qapi_err_flag;
}

qapi_Status_t qapi_Pmapp_Vbatt_Get_Battery_Temparature(qapi_PM_Battery_Temperature_t *qapi_batt_temp)
{
    qapi_Status_t qapi_err_flag  = QAPI_OK;
	pm_err_flag_type pm_err_flag = PM_ERR_FLAG__SUCCESS;

	pm_err_flag = pmapp_get_battery_temp((pm_battery_temperature*)qapi_batt_temp);
    if (pm_err_flag == PM_ERR_FLAG__SUCCESS)
    {
		qapi_err_flag = QAPI_OK;
    }
    else if (pm_err_flag == PM_ERR_FLAG__FEATURE_NOT_SUPPORTED)
    {
		qapi_err_flag = QAPI_ERR_NOT_SUPPORTED;
    }
    else if (pm_err_flag == PM_ERR_FLAG__PAR1_OUT_OF_RANGE)
    {
		qapi_err_flag = QAPI_ERR_INVALID_PARAM;
    }

    else if (pm_err_flag == PM_ERR_FLAG__BATTERY_MISSING)
    {
		qapi_err_flag = QAPI_ERR_BATT_ABSENT;
    }

    else 
    {
		qapi_err_flag = QAPI_ERROR;
    }
   
    return qapi_err_flag;
}

qapi_Status_t qapi_Pmapp_Vbatt_Get_Battery_Technology(qapi_PM_Battery_Technology_t *qapi_batt_tech)
{
    qapi_Status_t qapi_err_flag  = QAPI_OK;
	pm_err_flag_type pm_err_flag = PM_ERR_FLAG__SUCCESS;
	
	pm_err_flag = pmapp_get_battery_tech((pm_battery_technology*)qapi_batt_tech);
    if (pm_err_flag == PM_ERR_FLAG__SUCCESS)
    {
		qapi_err_flag = QAPI_OK;
    }
    else if (pm_err_flag == PM_ERR_FLAG__FEATURE_NOT_SUPPORTED)
    {
		qapi_err_flag = QAPI_ERR_NOT_SUPPORTED;
    }
    else if (pm_err_flag == PM_ERR_FLAG__PAR1_OUT_OF_RANGE)
    {
		qapi_err_flag = QAPI_ERR_INVALID_PARAM;
    }

    else if (pm_err_flag == PM_ERR_FLAG__BATTERY_MISSING)
    {
		qapi_err_flag = QAPI_ERR_BATT_ABSENT;
    }

    else 
    {
		qapi_err_flag = QAPI_ERROR;
    }
   
    return qapi_err_flag;
}

qapi_Status_t qapi_Pmapp_Vbatt_Get_Battery_Charger_Source(qapi_PM_Battery_Chg_Src_t *qapi_charger_source)
{
    qapi_Status_t qapi_err_flag  = QAPI_OK;
	pm_err_flag_type pm_err_flag = PM_ERR_FLAG__SUCCESS;

	pm_err_flag = pmapp_get_charger_source((pm_battery_chg_src*)qapi_charger_source);  
	
    if (pm_err_flag == PM_ERR_FLAG__SUCCESS)
    {
		qapi_err_flag = QAPI_OK;
    }
    else if (pm_err_flag == PM_ERR_FLAG__FEATURE_NOT_SUPPORTED)
    {
		qapi_err_flag = QAPI_ERR_NOT_SUPPORTED;
    }
    else if (pm_err_flag == PM_ERR_FLAG__PAR1_OUT_OF_RANGE)
    {
		qapi_err_flag = QAPI_ERR_INVALID_PARAM;
    }

    else if (pm_err_flag == PM_ERR_FLAG__BATTERY_MISSING)
    {
		qapi_err_flag = QAPI_ERR_BATT_ABSENT;
    }

    else 
    {
		qapi_err_flag = QAPI_ERROR;
    }
   
    return qapi_err_flag;
} 

qapi_Status_t qapi_Pmapp_Vbatt_Get_Battery_Charge_Status(qapi_PM_Battery_Chg_Status_t *qapi_charge_status)
{
    qapi_Status_t qapi_err_flag  = QAPI_OK;
	pm_err_flag_type pm_err_flag = PM_ERR_FLAG__SUCCESS;

	pm_err_flag = pmapp_get_charging_status((pm_battery_chg_status*)qapi_charge_status);
    if (pm_err_flag == PM_ERR_FLAG__SUCCESS)
    {
		qapi_err_flag = QAPI_OK;
    }
    else if (pm_err_flag == PM_ERR_FLAG__FEATURE_NOT_SUPPORTED)
    {
		qapi_err_flag = QAPI_ERR_NOT_SUPPORTED;
    }
    else if (pm_err_flag == PM_ERR_FLAG__PAR1_OUT_OF_RANGE)
    {
		qapi_err_flag = QAPI_ERR_INVALID_PARAM;
    }

    else if (pm_err_flag == PM_ERR_FLAG__BATTERY_MISSING)
    {
		qapi_err_flag = QAPI_ERR_BATT_ABSENT;
    }

    else 
    {
		qapi_err_flag = QAPI_ERROR;
    }
   
    return qapi_err_flag;
}

#ifdef FEATURE_PMIC_CHARGER
qapi_Status_t qapi_Pmapp_Vbatt_get_charge_reg(uint8 offset, uint8 *data)
{
  pm_err_flag_type  err_flag	= PM_ERR_FLAG__SUCCESS;
  qapi_Status_t qapi_err_flag  = QAPI_OK;
  
  if(data == NULL)
      return QAPI_ERROR;
  
  err_flag = pmapp_get_charge_reg(offset, data);

  if(err_flag != PM_ERR_FLAG__SUCCESS) qapi_err_flag = QAPI_ERROR;

  return qapi_err_flag;
}

qapi_Status_t qapi_Pmapp_Vbatt_set_charge_reg(uint8 offset, uint8 data)
{
  pm_err_flag_type  err_flag	= PM_ERR_FLAG__SUCCESS;
  qapi_Status_t qapi_err_flag  = QAPI_OK;

  err_flag = pmapp_set_charge_reg(offset, data);

  if(err_flag != PM_ERR_FLAG__SUCCESS) qapi_err_flag = QAPI_ERROR;

  return qapi_err_flag;
}

qapi_Status_t qapi_Pmapp_Vbatt_Get_Battery_Volt_Status(uint32 *qapi_batt_stat)
{
    qapi_Status_t qapi_err_flag  = QAPI_OK;
	pm_err_flag_type pm_err_flag = PM_ERR_FLAG__SUCCESS;

	pm_err_flag = pmapp_vbatt_get_battery_volt_status((uint32*)qapi_batt_stat);
    if (pm_err_flag == PM_ERR_FLAG__SUCCESS)
    {
		qapi_err_flag = QAPI_OK;
    }
    else if (pm_err_flag == PM_ERR_FLAG__FEATURE_NOT_SUPPORTED)
    {
		qapi_err_flag = QAPI_ERR_NOT_SUPPORTED;
    }
    else if (pm_err_flag == PM_ERR_FLAG__PAR1_OUT_OF_RANGE)
    {
		qapi_err_flag = QAPI_ERR_INVALID_PARAM;
    }
    
    else if (pm_err_flag == PM_ERR_FLAG__BATTERY_MISSING)
    {
		qapi_err_flag = QAPI_ERR_BATT_ABSENT;
    }
    
    else 
    {
		qapi_err_flag = QAPI_ERROR;
    }
   
    return qapi_err_flag;
}

qapi_Status_t qapi_Pmapp_Vbatt_config_sysok_option(uint8 status)
{
  pm_err_flag_type  err_flag	= PM_ERR_FLAG__SUCCESS;
  qapi_Status_t qapi_err_flag  = QAPI_OK;

  err_flag = pmapp_config_sysok_option(status);

  if(err_flag != PM_ERR_FLAG__SUCCESS) qapi_err_flag = QAPI_ERROR;

  return qapi_err_flag;
}

#endif
