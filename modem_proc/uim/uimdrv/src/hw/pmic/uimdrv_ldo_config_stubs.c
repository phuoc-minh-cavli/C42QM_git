/*============================================================================
  FILE:         uimdrv_ldo_config_stubs.c

  OVERVIEW:     This is the stubs file for uimdrv_ldo_cofig.c

  DEPENDENCIES: N/A

                Copyright (c) 2016 QUALCOMM Technologies, Inc(QTI).
                All Rights Reserved.
                QUALCOMM Technologies Confidential and Proprietary
============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

$Header: //components/rel/uim.mpss/3.3.1/uimdrv/src/hw/pmic/uimdrv_ldo_config_stubs.c#1 $
$DateTime: 2020/07/22 10:53:27 $
$Author: pwbldsvc $

when        who        what, where, why
------      ----       -----------------------------------------------------------
06/06/16    sam        Intial revision
============================================================================*/
#include "uimdrv_pmic.h"
#include "uimglobals.h"
#include "uimdrv_gpio.h"


/**
*  Stubs function for uimdrv_pmic_efs_get_user_ldo_config
*  Set teh default values for LDO config globals
*/
void uimdrv_pmic_efs_get_user_ldo_config(uim_instance_enum_type uim_instance)
{
  uim_instance_global_type*   uim_ptr = NULL;

  uim_ptr = uim_get_instance_ptr(uim_instance);
  if (uim_ptr == NULL)
  {
    return;
  }

  uim_ptr->hardware.uim_ldo.customize_ldo_setting                 = FALSE;
  uim_ptr->hardware.uim_ldo.desired_voltage_class                 = UIM_VOLTAGE_CLASS_INVALID;
  uim_ptr->hardware.uim_ldo.desired_voltage                       = PMIC_UIM_VOLTAGE_INVALID;
  uim_ptr->hardware.uim_ldo.pdown_allowed_upon_card_removal       = FALSE;
  uim_ptr->hardware.uim_ldo.pdown_allowed_upon_error              = FALSE;
} /* uimdrv_pmic_efs_get_user_ldo_config */


/**
*  Stubs function for uimdrv_pmic_set_voltage_regulator_settings
*/
void uimdrv_pmic_set_voltage_regulator_settings(uim_instance_enum_type  uim_instance)
{
  uim_instance_global_type*   uim_ptr = NULL;

  RETURN_IF_INSTANCE_INVALID(uim_instance);

  uim_ptr = uim_get_instance_ptr(uim_instance);

  if (uim_ptr == NULL)
  {
    return;
  }

  /* Initialize the voltage regulator to have the default settings */
  uim_hw_if.pmic[uim_instance].m_classB_voltageRegulator = PMIC_NPA_MODE_ID_UIM_ACTIVE_CLASS_B;
  uim_hw_if.pmic[uim_instance].m_classC_voltageRegulator = PMIC_NPA_MODE_ID_UIM_ACTIVE_CLASS_C;

 /*LDO customisation not supported hence return*/
  return;
}/* uimdrv_pmic_set_voltage_regulator_settings */


/**
* Stubs function for uim_evaluate_req_ldo_voltage_mod
*/
boolean uim_evaluate_req_ldo_voltage_mod(uim_instance_enum_type uim_instance,
                                         npa_resource_state     req)
{
  (void)uim_instance;
  (void)req;
  return TRUE;
} /* uim_evaluate_req_ldo_voltage_mod */

/*
  @brief  Stubs function for uim_config_external_ldo
*/
void uim_config_external_ldo
(
  uim_gpios_name_type   *pGpiosName,
  uim_instance_enum_type uim_instance
)
{
  RETURN_IF_INSTANCE_INVALID(uim_instance);

  uim_hw_if.uim_is_external_ldo[uim_instance] = FALSE;
}/*uim_config_external_ldo*/


/* Stubs function for  uimConfigureUimExtLdoToClassC*/
boolean uimConfigureUimExtLdoToClassC(uim_instance_enum_type uim_instance)
{
  (void)uim_instance;
  return FALSE;
} /* uimConfigureUimExtLdoToClassC */


/* Stubs function for  uimConfigureUimExtLdoToClassB*/
boolean uimConfigureUimExtLdoToClassB(uim_instance_enum_type uim_instance)
{
  (void)uim_instance;
  return FALSE;
} /* uimConfigureUimExtLdoToClassB */


/* Stubs function for  uimConfigureUimExtLdoToZero*/
boolean uimConfigureUimExtLdoToZero(uim_instance_enum_type uim_instance)
{
  (void)uim_instance;
  return FALSE;
} /* uimConfigureUimExtLdoToZero */