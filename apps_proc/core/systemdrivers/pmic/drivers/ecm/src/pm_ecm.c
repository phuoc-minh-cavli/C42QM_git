/*! \file pm_ecm_oem.c
*  \n
*  \brief This file contains ecm peripheral APIs
*  \n
*  \n &copy; Copyright 2016 QUALCOMM Technologies Incorporated, All Rights Reserved
*/
/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/09/16   akm     Creation
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_ecm.h"
#include "pm_version.h"
#include "pm_resource_manager.h"
#include "pm_resources_and_types.h"


/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/

#define PM_MISC_ECM_EN_CTL   0x980
#define PM_MISC_ECM_STATUS4  0x90B

typedef struct
{
  pm_register_address_type status4;		   //0x00B
  pm_register_address_type ecm_en_ctl;             //0x080
  
} pm_misc_register_info_type;


static pm_misc_register_info_type 
pm_misc_reg_table =
{
 PM_MISC_ECM_STATUS4,
 PM_MISC_ECM_EN_CTL, 
};

pm_err_flag_type pm_ecm_ext_channel_enable_status( pm_ecm_ext_channel_type Ch , pm_on_off_type *on_off)
{
	uint8 data = 0;
	uint8 pmic_index = 0; 
	pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
	pm_model_type pmic_model = PMIC_IS_UNKNOWN;
	pmic_model = pm_get_pmic_model(pmic_index);
	
	
	if((pmic_model != PMIC_IS_PMD9655) || (Ch >= PM_MISC_ECM__INVALID) || (on_off == NULL))
	{
	    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
	}
	
        //Read ECM present bit 
        err_flag = pm_comm_read_byte_mask(0, pm_misc_reg_table.status4 ,0x10 ,&data,0);

        if ((err_flag != PM_ERR_FLAG__SUCCESS) || (data != 0x10))
        {
            return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
        }
          
       err_flag = pm_comm_read_byte(0, pm_misc_reg_table.ecm_en_ctl, &data, 0);
       if(err_flag != PM_ERR_FLAG__SUCCESS) return err_flag ;

       *on_off = (data & 0x80) ? PM_ON : PM_OFF;

       return err_flag;
}
