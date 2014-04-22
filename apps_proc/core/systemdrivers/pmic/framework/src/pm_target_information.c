/*! \file pm_target_information.c
*  
*  \brief This file contains target specific PMIC settings common across processors.
*  \n   
*  &copy; Copyright 2012-2014 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.

$Header: //components/rel/core.tx/6.0/systemdrivers/pmic/framework/src/pm_target_information.c#1 $ 

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_target_information.h"
#include "DALSys.h"
#include "pm_version.h"
#include "pm_comm.h"
#include "device_info.h"
#include "DDIChipInfo.h"

/*===========================================================================

                        STATIC VARIABLES 

===========================================================================*/

static DALSYS_PROPERTY_HANDLE_DECLARE(hProp_common);
static DALSYS_PROPERTY_HANDLE_DECLARE(hProp_target);
static DALSYSPropertyVar prop_common;                /* Union to retreive property values */
static DALSYSPropertyVar prop_target;                /* Union to retreive property values */

extern boolean                      bPMICStub;

void pm_target_information_init(void)
{
	/*
	* Runtime detection to support multiple PMIC configuration on any target.
	* We're only concerned with the primary PMIC here so we use index 0.
	*/
	char* pmic_props = NULL;
    const char* pmic_props_common = "/apps/pmic/common";
	uint8 pm_model = 0;
    if (bPMICStub == TRUE)
    {
        pmic_props = "/apps/pmd9665";
    }
    else
    {	
        pm_model = pm_get_pmic_model((uint8)0);
        switch(pm_model)
        {
            case PMIC_IS_PMD9655:
                if(DALCHIPINFO_FAMILY_MDM9x65 == DalChipInfo_ChipFamily())
                {
                   pmic_props = "/apps/pmd9665";
                }
                else
                {
                   pmic_props = "/apps/pmd9655";
                }
                break;
            case PMIC_IS_PMD9607:
                {
                   pmic_props = "/apps/pmd9607";
                   break;
                }
            case PMIC_IS_PME9205:
                {
                   pmic_props = "/apps/pme9205";
                   break;
                }
            default:
                break;
        }
    }
	
	if(NULL == pmic_props)
	{
		/* 
		* ERR Fatal here as we were unable to discover the PMIC.  No device ID for PMIC -
		* just use 0.
		*/
		ERR_FATAL("DALLOG Device [0x%0x] Type [%s] Log [%s]", 0,
                1, "PMIC:  Initialization failed to identify PMIC device!");
		return;
	}
	
	if(DAL_SUCCESS != DALSYS_GetDALPropertyHandleStr((const char*)pmic_props, hProp_target))
	{
		ERR_FATAL("DALLOG Device [0x%0x] Type [%s] Log [%s]", 0,
                1, "PMIC:  Initialization failed hProp_target handle retrieval!");
	}	
    if(DAL_SUCCESS != DALSYS_GetDALPropertyHandleStr(pmic_props_common, hProp_common))
	{
	
		ERR_FATAL("DALLOG Device [0x%0x] Type [%s] Log [%s]", 0,
			1, "PMIC:  Initialization failed hProp_common handle retrieval!");
	}
}

pm_err_flag_type pm_target_information_read_peripheral_rev(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

    if(!comm_ptr)
    {
        return PM_ERR_FLAG__COMM_TYPE_NOT_RECOGNIZED;
}

    err_flag = pm_get_peripheral_info(comm_ptr->slave_id, peripheral_info);


    return err_flag;
    }


void* pm_target_information_get_common_info(uint32 prop_id)
{
  void *info = NULL;

  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hProp_common, NULL, prop_id, &prop_common))
  {
      info = (void*)prop_common.Val.pStruct;
  }

  return info;
}

void* pm_target_information_get_specific_info(uint32 prop_id)
{
  void *info = NULL;

  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hProp_target, NULL, prop_id, &prop_target))
  {
      info = (void*)prop_target.Val.pStruct;
  }

  return info;
}

uint32 pm_target_information_get_periph_count_info(uint32 prop_id , uint8 pmic_index)
{
  uint32 *peripheral_count = NULL;

  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hProp_target, NULL, prop_id, &prop_target)&& 
  	 (pmic_index < PM_MAX_NUM_PMICS) && (pm_get_pmic_model(pmic_index) != PMIC_IS_UNKNOWN))
  {
      peripheral_count = (uint32*)prop_target.Val.pStruct;
      return peripheral_count[pmic_index];
  }

  return 0;
}
