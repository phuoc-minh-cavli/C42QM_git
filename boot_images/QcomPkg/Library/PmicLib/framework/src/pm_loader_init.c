/*! \file pm_init.c
*   \n
*   \brief This file contains PMIC initialization function which initializes the PMIC Comm
*   \n layer, PMIC drivers and PMIC applications.
*   \n
*   \n &copy; Copyright 2010-2016 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/03/16   aab     Updated pm_driver_post_init() to clear OCP status
08/04/16   aab     Updated pm_driver_init() to support debug logging
01/25/16   aab     Updated pm_driver_init() to support RUMI targets with out PMIC
09/25/15   aab     Updated pm_device_setup(void) and Renamed pm_target_information_spmi_chnl_cfg() to pm_bus_init() 
08/08/15   aab     Added pm_device_setup()
06/23/15   aab     Updated pm_driver_init() to ensure if pm device is initialized.
                    pm_driver_init may be called directly in dload mode.
05/31/15   aab     Removed pm_oem_init()
07/16/14   akm     Comm change Updates
03/31/14   akm     Cleanup
01/15/13   aab     Fixed KW error
05/10/11   jtn     Fix RPM init bug for 8960
07/01/10   umr     Created.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_resource_manager.h"
#include "pm_sbl_boot.h"
#include "pm_device.h"
#include "pm_comm.h"
#include "device_info.h"
#include "pm_target_information.h"
#include "SpmiCfg.h"
#include "SpmiBus.h"
#include "pm_boot.h"
#include "CoreVerify.h"
#include "pm_utils.h"
#include "pm_app_vreg.h"
#include "pm_log_utils.h"
#include "pmio_pon.h"

/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/
static pm_err_flag_type pm_spmi_config_hw_interface_ports(void);

/*===========================================================================

                        GLOBALS and TYPE DEFINITIONS 

===========================================================================*/
static boolean pm_device_setup_done = FALSE;
static boolean pm_spmi_static_ch_init_done = FALSE;

/*===========================================================================

                        FUNCTION DEFINITIONS 

===========================================================================*/
pm_err_flag_type
pm_device_setup(void)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

    if(FALSE == pm_device_setup_done)
    {
       //SPMI init: Must be called before calling any SPMI R/W.
       err_flag |= pm_spmi_dynamic_chnl_init();

        err_flag |= pm_comm_channel_init_internal();
        err_flag |= pm_version_detect();

        pm_device_setup_done = TRUE;
    }
    return err_flag;
}




pm_err_flag_type pm_driver_init( void )
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_sbl_specific_data_type *config_param_ptr = NULL;
    uint32 initial_num_spmi_trans;

    if( (pm_is_target_pre_silicon() == TRUE) && (pm_is_pmic_present(PMIC_A) == FALSE) )
    {
       pm_log_message("Bootup: No PMIC on RUMI Target");
       return err_flag = PM_ERR_FLAG__SUCCESS;
    }
    
    if(!(pm_device_init_status()))
    {
      return  PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;  
    }

    config_param_ptr = (pm_sbl_specific_data_type*)pm_target_information_get_specific_info(PM_PROP_SBL_SPECIFIC_DATA);
    CORE_VERIFY_PTR(config_param_ptr);
    initial_num_spmi_trans = pm_log_num_spmi_transaction(0, FALSE);

    err_flag |= pm_driver_pre_init ();
    pm_resource_manager_init();
    err_flag |= pm_driver_post_init ();
    
    if (err_flag == PM_ERR_FLAG__SUCCESS)
    {
       err_flag = pm_comm_write_byte_mask(0, PMIO_PON_PERPH_RB_SPARE_ADDR, PON_PERPH_RB_SPARE_DRIVER_INIT_MASK, PON_PERPH_RB_SPARE_DRIVER_INIT_MASK, 0);
    }

    pm_log_num_spmi_transaction(initial_num_spmi_trans, config_param_ptr->verbose_uart_logging);
    err_flag |= pm_app_vreg_clear_ocp_status(); // To Clear OCP Status of LDO's and SMPS.
    return err_flag;
}


pm_err_flag_type
pm_qsee_pre_init(void)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

    /* if( (pm_is_target_pre_silicon() == TRUE) && (pm_is_pmic_present(PMIC_A) == FALSE) )
    {
       pm_log_message("Bootup: No PMIC on RUMI Target");
       return err_flag = PM_ERR_FLAG__SUCCESS;
    } */

    // update the spmi/pvc config table if targets recommends  
    err_flag |= pm_qsee_pre_target_init();
    //spmi config init for peripheral ownership
    err_flag |= pm_spmi_static_chnl_init();
    err_flag |= pm_spmi_config_hw_interface_ports();


    return err_flag;
}



/* =========================================================================
**  Function :  pm_target_information_spmi_chnl_cfg
** =========================================================================*/
/**
  Description: The function retrieves PMIC property handle.
               On success it queries the SPMI configurations.
               It configures SPMI channel and interrupt ownership for peripherals.
  @param [in] pmic_prop: property name
  @return
  PM_ERR_FLAG__SUCCESS on success otherwise PM_ERR_FLAG__SPMI_OPT_ERR.
  @dependencies
  SPMI APIs.
  @sa None
*/
pm_err_flag_type
pm_spmi_dynamic_chnl_init(void)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

  /* Initialize SPMI and enable SPMI global interrupt. */
  if (SpmiCfg_Init(TRUE) == SPMI_SUCCESS)
  {
    /* Auto configure SPMI channel and peripheral interrupt ownership. */
    if (SpmiCfg_SetDynamicChannelMode(TRUE) != SPMI_SUCCESS)
    {
      err_flag = PM_ERR_FLAG__SPMI_OPT_ERR;
    }
  }
  else 
  {
     err_flag  = PM_ERR_FLAG__SPMI_OPT_ERR;
  }

  return err_flag;
}


/**
 * @brief This function configures SPMI channel.
 * 
 * @details
 *    The function retrieves PMIC property handle for the DEVCFG SDM845 target.
 *    On success it calls SPMI API to configure SPMI channels.
 *   
 * @param[in] void No input needed.
 *
 * @return pm_err_flag_type PM_ERR_FLAG__INVALID = ERROR. 
 *         PM_ERR_FLAG__SUCCESS = SUCCESS.
 *
 */
pm_err_flag_type pm_spmi_static_chnl_init(void)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

   SpmiCfg_ChannelCfg*  spmi_cfg_ptr      = NULL;
   uint32* spmi_cfg_size                  = NULL;
   uint32 rgCount = 18;
   SpmiCfg_RGConfig rgCfg [18] = {{0}};


   if(pm_spmi_static_ch_init_done == TRUE)
   {
      return PM_ERR_FLAG__SUCCESS;
   }

   /* Initialize SPMI driver. */
   if (SpmiCfg_Init(FALSE) == SPMI_SUCCESS)
   {
     /* Auto configure SPMI channel and peripheral interrupt ownership. */
     if (SpmiCfg_SetDynamicChannelMode(FALSE) != SPMI_SUCCESS)
     {
       return err_flag = PM_ERR_FLAG__SPMI_OPT_ERR;
     }
   }
   else 
   {
      return err_flag = PM_ERR_FLAG__SPMI_OPT_ERR;
   }
  
   spmi_cfg_ptr = (SpmiCfg_ChannelCfg*)pm_target_information_get_specific_info(PM_PROP_SPMI_CHANNEL_CFG);
   CORE_VERIFY_PTR(spmi_cfg_ptr);

   spmi_cfg_size = (uint32*)pm_target_information_get_specific_info(PM_PROP_SPMI_CHANNEL_CFG_SIZE);
   CORE_VERIFY_PTR(spmi_cfg_size);
  
   /* Configure SPMI channels  */
   if ((NULL != spmi_cfg_size) && ( SpmiCfg_ConfigurePeripherals(spmi_cfg_ptr, spmi_cfg_size[0],rgCfg,&rgCount) != SPMI_SUCCESS))
   {
      return err_flag = PM_ERR_FLAG__SPMI_OPT_ERR;
   }
  
   // initialize the SPMI BUS
   if( SpmiBus_Init() != SPMI_SUCCESS )
   {
       err_flag |= PM_ERR_FLAG__SPMI_OPT_ERR;
   }

  if (err_flag == PM_ERR_FLAG__SUCCESS)
  {
     pm_spmi_static_ch_init_done = TRUE;
  }

  return err_flag;
}



/**
 * @brief This function configures and enables HW interface 
 *        ports.
 * 
 * @details
 *    The function retrieves PMIC property handle for the DEVCFG SDM845 target.
 *    On success it calls SPMI API to configure PVC Ports, 
 *    Ports.
 *   
 * @param[in] void No input needed.
 *
 * @return pm_err_flag_type PM_ERR_FLAG__INVALID = ERROR. 
 *         PM_ERR_FLAG__SUCCESS = SUCCESS.
 *
 */
static pm_err_flag_type pm_spmi_config_hw_interface_ports(void)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
   uint32 index = 0;

   SpmiCfg_PvcPortCfg** pvc_port_cfg_ptr  = NULL;
   uint32* pvc_port_cfg_size              = NULL;


   /* Configure PVC Port */
   pvc_port_cfg_ptr = (SpmiCfg_PvcPortCfg**)pm_target_information_get_specific_info(PM_PROP_PVC_PORT_CFG);
   CORE_VERIFY_PTR(pvc_port_cfg_ptr);

   pvc_port_cfg_size = (uint32*)pm_target_information_get_specific_info(PM_PROP_PVC_PORT_CFG_SIZE);
   CORE_VERIFY_PTR(pvc_port_cfg_size);

   for(index = 0; index < pvc_port_cfg_size[0]; index++)
   {
     CORE_VERIFY_PTR(pvc_port_cfg_ptr[index]);

     if (SpmiCfg_ConfigurePvcPort(pvc_port_cfg_ptr[index]) != SPMI_SUCCESS)
     {
       return err_flag = PM_ERR_FLAG__SPMI_OPT_ERR;
     }
   }

   /* Enable PVC Ports */
   if (SpmiCfg_SetPVCPortsEnabled(TRUE) != SPMI_SUCCESS)
   {
     return err_flag = PM_ERR_FLAG__SPMI_OPT_ERR;
   }

   return err_flag;
}
