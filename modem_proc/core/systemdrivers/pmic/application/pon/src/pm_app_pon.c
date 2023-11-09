/*! \file pm_app_pon.c
*  
*  \brief Implementation file for PON APP level APIs.
*    
*  Copyright (c) 2019, 2023 Qualcomm Technologies, Inc. All rights reserved.
*/
/*===================================================================
EDIT HISTORY FOR MODULE
 This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Date: 2023/09/13 $ 
$Change: 49273354 $
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/25/19   abh     Added file for Tiny Modem image in Twizy
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_pon.h"
#include "pm_app_pon.h"
#include "pm_err_flags.h"
#include "device_info.h"
#include "pm_comm.h"
#include "pm_device.h"
#include "pmapp_pon.h"
#include "products_utility.h" 
/*===============================================================================
                            DEFINITIONS
/*=============================================================================== */
typedef struct pm_misc_register_info_type
{
    pm_register_address_type    boot_option;
    pm_register_address_type    psm;
}pm_misc_register_info_type;

static pm_misc_register_info_type misc_reg =
{
    .boot_option = 0x95D,  		//MDM_BOOT_OPTIONS
    .psm         = 0x960,       //PSM_ENTRY_EXIT
};

#define PM_MISC_SLAVE_ID 0
#define PM_PON_SLAVE_ID  0
extern boolean bPMICStub;
static pmapp_pon_isr_data isr_data;
/*===========================================================================
                       Function  Prototype
===========================================================================*/

//API used to get pmic specific PS HOLD configuration
pm_err_flag_type 
pm_tgt_get_pshold_reset_cfg( uint32                      pmic_index,
                             pm_app_pon_reset_cfg_type   app_pshold_cfg,
                             pm_pon_reset_cfg_type*      pshold_cfg);

//API used to do target specific PSHOLD configuration
pm_err_flag_type
pm_pon_tgt_specific_pshold_cfg(uint32 pmic_index, pm_app_pon_reset_cfg_type app_pshold_cfg);

/* DAL ISR callback */
static void* pon_isr(void* me, uint32 intrMask);

/*===========================================================================
                   API Implementation
===========================================================================*/

pm_err_flag_type
pm_app_pon_pshold_cfg(pm_app_pon_reset_cfg_type app_pshold_cfg) 
{
   pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
   pm_model_type pmic_model  = PMIC_IS_INVALID;
   uint32  pmic_index        = 0;
   pm_pon_reset_cfg_type    pshold_cfg = PM_PON_RESET_CFG_INVALID;


   if (app_pshold_cfg >= PM_APP_PON_CFG_MAX) 
   {
      return PM_ERR_FLAG__INVALID;
   }

   if ( (app_pshold_cfg == PM_APP_PON_CFG_DVDD_HARD_RESET) ||
        (app_pshold_cfg == PM_APP_PON_CFG_DVDD_SHUTDOWN) )
   {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;//SHUTDOWN configs are not supported at this time
   }

   err_flag |= pm_device_setup();
   if (err_flag != PM_ERR_FLAG__SUCCESS) {
        return PM_ERR_FLAG__INVALID;
   }

   while (pmic_index < PM_MAX_NUM_PMICS) 
   {
      pmic_model = pm_get_pmic_model(pmic_index);

      if ((pmic_model != PMIC_IS_INVALID) && (pmic_model != PMIC_IS_UNKNOWN)) 
      {
         //Get PMIC specific PSHOLD configuration
         err_flag |= pm_tgt_get_pshold_reset_cfg(pmic_index, app_pshold_cfg, &pshold_cfg); 
         if (err_flag != PM_ERR_FLAG__SUCCESS) 
         {
            break;
         }

         //PSHOLD Reset Config
         if (pshold_cfg != PON_NO_CFG_NEEDED) 
         { 
            err_flag |= pm_pon_ps_hold_cfg(pmic_index, pshold_cfg);
         }

         //Target specific reset config 
         err_flag |= pm_pon_tgt_specific_pshold_cfg(pmic_index, app_pshold_cfg);

      }
      pmic_index++;
   }

   return err_flag;
} 

pm_err_flag_type
pm_app_misc_get_cfg(pm_app_misc_cfg_type app_misc_cfg, uint8 *data)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  pm_register_address_type reg = 0;

  if(app_misc_cfg >= PM_APP_MISC_CFG_MAX || data == NULL)
  {
    return PM_ERR_FLAG__INVALID;
  }
  switch(app_misc_cfg)
  {
    case PM_APP_MISC_CFG_BOOT_OPTION:
         reg = misc_reg.boot_option;
         break;
    case PM_APP_MISC_CFG_PSM:
         reg = misc_reg.psm;
         break;
    default:
         break;
  }
 
  err_flag = pm_comm_read_byte(PM_MISC_SLAVE_ID,reg, data, 0); 
  
  return err_flag;
}


pm_err_flag_type
pm_app_misc_set_cfg(pm_app_misc_cfg_type app_misc_cfg, uint8 mask, uint8 value)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  pm_register_address_type reg = 0;
  uint8 data = 0;
  
  if(app_misc_cfg >= PM_APP_MISC_CFG_MAX )
  {
    return PM_ERR_FLAG__INVALID;
  }
  switch(app_misc_cfg)
  {
    case PM_APP_MISC_CFG_BOOT_OPTION:
         reg = misc_reg.boot_option;
         break;
    case PM_APP_MISC_CFG_PSM:
         reg = misc_reg.psm;
         break;
    default:
         break;
  }

  err_flag = pm_comm_read_byte(PM_MISC_SLAVE_ID,reg, &data, 0);         //read the MISC_MISC_PSM register
  data &= (~mask);                                                      // clear the bits which you are interested 
  data |= ( mask & value);                                              // set the cleared bits as you want
  err_flag |= pm_comm_write_byte(PM_MISC_SLAVE_ID,reg, data, 0);        // write back to MISC_MISC_PSM register
  
  return err_flag;
}

pm_err_flag_type pmapp_pon_kypd_register_cb(pmapp_pon_fn_cb cb)
{
    static uint8 run_once = 0;
    static DalDeviceHandle *dalIntHandle = NULL;
    boolean status = FALSE;
    
    DALResult result = DAL_SUCCESS;
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

    if (bPMICStub == TRUE)
    {
        return err_flag;
    }
    if( NULL == cb )
    {
      return  PM_ERR_FLAG__INVALID_POINTER;
    }  
  
    if( MODEM_BOOT_TINY_NO_APSS != get_modem_boot_mode())
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED; 
    }
    
    if(run_once == 0)
    {
        //Configure PON interrupt for falling/rising edges
        err_flag = pm_pon_irq_set_trigger(PM_MISC_SLAVE_ID, PM_PON_IRQ_KPDPWR_ON, PM_IRQ_TRIGGER_DUAL_EDGE);

        //Clear PON interrupt
        if(err_flag == PM_ERR_FLAG__SUCCESS)
            err_flag = pm_pon_irq_clear(PM_MISC_SLAVE_ID, PM_PON_IRQ_KPDPWR_ON);

        //Enable PON interrupt
        if(err_flag == PM_ERR_FLAG__SUCCESS)
            err_flag = pm_pon_irq_enable(PM_MISC_SLAVE_ID, PM_PON_IRQ_KPDPWR_ON, TRUE);

        //Get DAL handle
        if(err_flag == PM_ERR_FLAG__SUCCESS)
            result = DAL_StringDeviceAttach("DALDEVICEID_SPMI_DEVICE", &dalIntHandle);

        if( (result == DAL_SUCCESS ) && (NULL != dalIntHandle))
        {
            result = DalDevice_Open(dalIntHandle, DAL_OPEN_SHARED);
            //Configure ISR for PON module
            if(result == DAL_SUCCESS)
            {
             isr_data.cb = cb;
             result = DalSpmi_RegisterIsr(dalIntHandle, 0x08, 0xFF, pon_isr, &isr_data);
            }
        }
        if(result != DAL_SUCCESS)
            err_flag = PM_ERR_FLAG__DAL_SERVICE_REGISTRATION_FAILED;
			
        err_flag |= pm_pon_irq_status( PM_MISC_SLAVE_ID, PM_PON_IRQ_KPDPWR_ON, PM_IRQ_STATUS_RT, &status);
        if((err_flag == PM_ERR_FLAG__SUCCESS) && (TRUE == status))
        {
          cb(status);
        }
        run_once = 1;
    }

    return err_flag;
}

static void* pon_isr(void* me, uint32 intrMask)
{
  pmapp_pon_isr_data* isr_data = (pmapp_pon_isr_data*)me;
  boolean status = FALSE;

  if( NULL == isr_data )
  {
    return NULL; // don't call CB
  }
  pm_pon_irq_clear(PM_MISC_SLAVE_ID, PM_PON_IRQ_KPDPWR_ON);
  pm_pon_irq_status( PM_MISC_SLAVE_ID, PM_PON_IRQ_KPDPWR_ON, PM_IRQ_STATUS_RT, &status);
  
  isr_data->cb(status);

  return NULL;
}


pm_err_flag_type pmapp_pon_reason_debug(pm_pon_debug_type *debug_data)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  if (debug_data == NULL)
  {
    return PM_ERR_FLAG__INVALID_POINTER;
  }
  err_flag = pm_pon_reason_debug(PM_PON_SLAVE_ID,debug_data);
  return err_flag;
}