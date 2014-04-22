/*! \file pm_pon_app.c
*  \n
*  \brief PON Application APIs implementation.
*  \n  
*  \n &copy; Copyright 2013 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                            Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/systemdrivers/pmic/application/ecm/src/pm_ecm_app.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
2/26/14    rh      Created file
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#include "pmapp_ecm.h"
#include "pm_err_flags.h"
#include "pm_ulog.h"
#include "pm_version.h"
#include "pm_target_information.h"
#include "pm_ldo.h"
#include "pm_smps.h"
#include "pm_ecm.h"

/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/

extern boolean  bPMICStub;


/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/

/* Retrun rails for which ecm is enabled.             
*/

pm_err_flag_type pmapp_ecm_enable_status(pmapp_ecm_enable_status_type *get_ecm_status)
{

    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint32 num_of_peripherals = 0;
    uint32 count = 0 ;
    uint8 pmic_index = 0; 
    uint8 perph_index = 0;
    uint8 ch = 0;
    pm_on_off_type on_off = PM_OFF;
    pm_model_type pmic_model = PMIC_IS_UNKNOWN;
    pmic_model = pm_get_pmic_model(pmic_index);

    if (bPMICStub == TRUE)
    {
        return err_flag;
    }	
	
    if(pmic_model != PMIC_IS_PMD9655)
    {
       err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    
    if(err_flag == PM_ERR_FLAG__SUCCESS)
    {
       //Get SMPS ECM enable count
        num_of_peripherals = pm_target_information_get_periph_count_info(PM_PROP_SMPS_NUM, pmic_index);
        if(num_of_peripherals > 0)
        {

             PM_LOG_MSG_INFO("Get SMPS ECM count");  

             for(perph_index = 0; perph_index < num_of_peripherals; perph_index++)
             {

                   err_flag = pm_smps_ecm_local_enable_status(pmic_index, perph_index, &on_off);
                   if ((err_flag == PM_ERR_FLAG__SUCCESS)&& (on_off == PM_ON))
                   {
                       count ++;
                    }
                   PM_LOG_MSG_DEBUG( "SMPS rail=%d, ON_OFF=%d, SMPS ECM enable count=%d", perph_index, on_off, count); 
                   on_off = PM_OFF;
              }
              get_ecm_status->smps_count  = count;
              count = 0;

         }

       //Get LDO ECM enable count
        num_of_peripherals = pm_target_information_get_periph_count_info(PM_PROP_LDO_NUM, pmic_index);
        if(num_of_peripherals > 0)
        {
             PM_LOG_MSG_INFO("Get LDO ECM count");  
             for(perph_index = 0; perph_index < num_of_peripherals; perph_index++)
             {
                   err_flag = pm_ldo_ecm_local_enable_status(pmic_index, perph_index, &on_off);
                   if ((err_flag == PM_ERR_FLAG__SUCCESS)&& (on_off == PM_ON))
                   {

                       count ++;
                    }
                   PM_LOG_MSG_DEBUG( "LDO rail=%d, ON_OFF=%d, LDO ECM enable count=%d", perph_index, on_off, count);  
                   on_off = PM_OFF;
              }
              get_ecm_status->ldo_count  = count;
              count = 0;

         }

       PM_LOG_MSG_INFO("Get QPOET ECM count"); 
       for(ch = 0; ch < PM_MISC_ECM__INVALID; ch++)
       {
             err_flag = pm_ecm_ext_channel_enable_status( (pm_ecm_ext_channel_type)ch , &on_off);
              if ((err_flag == PM_ERR_FLAG__SUCCESS) && (on_off == PM_ON))
             {

                 count ++;
             }
             PM_LOG_MSG_DEBUG( "QPOET rail=%d, ON_OFF=%d, Ext channel ECM enable count=%d", ch, on_off, count); 
             on_off = PM_OFF;
       }
       get_ecm_status->ext_ch_count  = count;

    }

    PM_LOG_MSG_INFO( "SMPS count=%d, LDO count=%d, QPOET count=%d", get_ecm_status->smps_count, get_ecm_status->ldo_count, get_ecm_status->ext_ch_count);  
    return err_flag;
}


