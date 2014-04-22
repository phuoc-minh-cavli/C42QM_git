/*============================================================================
  FILE:         uimdrv_ldo_config_stubs.c

  OVERVIEW:     This file consists of LDO configuration realted APIs

  DEPENDENCIES: N/A

                Copyright (c) 2016-2017 QUALCOMM Technologies, Inc(QTI).
                All Rights Reserved.
                QUALCOMM Technologies Confidential and Proprietary
============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

$Header: //components/rel/uim.mpss/3.3.1/uimdrv/src/hw/pmic/uimdrv_ldo_config.c#1 $
$DateTime: 2020/07/22 10:53:27 $
$Author: pwbldsvc $

when        who        what, where, why
------      ----       -----------------------------------------------------------
12/26/17    gm         Bringup changes for SDM2500
12/05/16    bcho       Remove compilation error
06/06/16    sam        Intial revision
============================================================================*/
#include "uimdrv_pmic.h"
#include "uimdrv_uartdm.h"
#include "uimdrv_rumi.h"
#include "DDIChipInfo.h"
#include "DDIPlatformInfo.h"
#include "uimglobals.h"
#include "uim_common_efs.h"
#include "uimdrv_gpio.h"
#include "pm_version.h"

/**
*  This function gets the customized LDO settings from NV.
*
*  @param uim_instance      uim instance.
*/
void uimdrv_pmic_efs_get_user_ldo_config(uim_instance_enum_type uim_instance)
{
  uimVccEnum                        pmic_ref = INVALID_PMIC_NPA_RESOURCE;
  uim_instance_global_type*         uim_ptr = NULL;
  uim_common_efs_status_enum_type   efs_write_status = UIM_COMMON_EFS_SUCCESS;

  RETURN_IF_INSTANCE_INVALID(uim_instance);

  pmic_ref = m_HWConfig.uim_slot_config[uim_instance].uimVcc;

  uim_ptr = uim_get_instance_ptr(uim_instance);

  if (uim_ptr == NULL)
  {
    return;
  }

  /* === Read the required UIM HW LDO configuration from EFS === */
  if (UIM_COMMON_EFS_SUCCESS == uim_common_efs_read(UIM_COMMON_EFS_UIMDRV_LDO_SETTINGS,
                                                    UIM_COMMON_EFS_ITEM_FILE_TYPE,
                                                    (uim_common_efs_context_type)pmic_ref,
                                                    (uint8 *)&(uim_ptr->hardware.uim_ldo),
                                                    sizeof(uim_ptr->hardware.uim_ldo)))
  {
  }
  else
  {
    uim_ptr->hardware.uim_ldo.customize_ldo_setting                 = FALSE;
    uim_ptr->hardware.uim_ldo.desired_voltage_class                 = UIM_VOLTAGE_CLASS_INVALID;
    uim_ptr->hardware.uim_ldo.desired_voltage                       = PMIC_UIM_VOLTAGE_INVALID;
    uim_ptr->hardware.uim_ldo.pdown_allowed_upon_card_removal       = FALSE;
    uim_ptr->hardware.uim_ldo.pdown_allowed_upon_error              = FALSE;

    efs_write_status = uim_common_efs_write(UIM_COMMON_EFS_UIMDRV_LDO_SETTINGS,
                                                       UIM_COMMON_EFS_ITEM_FILE_TYPE,
                                                       (uim_common_efs_context_type)pmic_ref,
                                                       (const char*)&(uim_ptr->hardware.uim_ldo),
                                                       sizeof(uim_ptr->hardware.uim_ldo));

    UIMDRV_MSG_HIGH_1(uim_ptr->id, "writing default values to UIM_COMMON_EFS_UIMDRV_LDO_SETTINGS is success/fail-0x%x", efs_write_status);
  }
} /* uimdrv_pmic_efs_get_user_ldo_config */


/**
*  This function sets the voltage regulator with the required
*  voltage in PMIC language, the value is based on the desired
*  voltage by the user that is set in NV. The voltage regulator
*  value is passed while making a npa request.
*
*  @param uim_instance      uim instance.
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

  if (uim_ptr->hardware.uim_ldo.customize_ldo_setting == FALSE ||
      uim_ptr->hardware.uim_ldo.desired_voltage == PMIC_UIM_VOLTAGE_INVALID)
  {
    UIMDRV_MSG_HIGH_0(uim_ptr->id,"No customized voltage regulator settings");
    return;
  }

  switch (uim_ptr->hardware.uim_ldo.desired_voltage)
  {
    case PMIC_UIM_ACTIVE_CLASS_B_LOW:
      uim_hw_if.pmic[uim_instance].m_classB_voltageRegulator = PMIC_NPA_MODE_ID_UIM_ACTIVE_CLASS_B_LOW;
      break;

    case PMIC_UIM_ACTIVE_CLASS_B:
      uim_hw_if.pmic[uim_instance].m_classB_voltageRegulator = PMIC_NPA_MODE_ID_UIM_ACTIVE_CLASS_B;
      break;

    case PMIC_UIM_ACTIVE_CLASS_B_HIGH:
      uim_hw_if.pmic[uim_instance].m_classB_voltageRegulator = PMIC_NPA_MODE_ID_UIM_ACTIVE_CLASS_B_HIGH;
      break;

    case PMIC_UIM_ACTIVE_CLASS_C_LOW:
      uim_hw_if.pmic[uim_instance].m_classC_voltageRegulator = PMIC_NPA_MODE_ID_UIM_ACTIVE_CLASS_C_LOW;
      break;

    case PMIC_UIM_ACTIVE_CLASS_C:
      uim_hw_if.pmic[uim_instance].m_classC_voltageRegulator = PMIC_NPA_MODE_ID_UIM_ACTIVE_CLASS_C;
      break;

    case PMIC_UIM_ACTIVE_CLASS_C_HIGH:
      uim_hw_if.pmic[uim_instance].m_classC_voltageRegulator = PMIC_NPA_MODE_ID_UIM_ACTIVE_CLASS_C_HIGH;
      break;

    default:
      ERR_FATAL("Incorrect voltage setting",0,0,0);
  }
}/* uimdrv_pmic_set_voltage_regulator_settings */


/**
*  This function is used to evaluate if the required voltage
*  request on the desired UIM LDO is allowed based on the
*  clients configuration settings of the UIM LDO.
*
*  @param  uim_instance  uim instance id.
*  @param  req           Required request.
*  @return boolean       TRUE - indicates to the calling process
*                        that the ldo bahviour can be modified.
*/
boolean uim_evaluate_req_ldo_voltage_mod(uim_instance_enum_type uim_instance,
                                         npa_resource_state     req)
{
  uim_instance_global_type*   uim_ptr = NULL;
  uim_ptr = uim_get_instance_ptr(uim_instance);

  if (uim_ptr == NULL)
  {
    return FALSE;
  }

  /* 1. If the LDO behaviour settings are NOT customized by the user,
        then we will simply return TRUE.
     2. In the case of the Recovery, all LDO modifications made by the user
        with respect to keeping the LDO active all the time is disregarded.
        The settings are honoured after recovery. Hence we return TRUE.
     If recovery is in progress and UIM state is not in error this would
     imply that recovery is still being attempted.
     When we fail recovery the state is set to UIM_ERR_S and the recovery triggered
     flag is cleared later and at point the users requests are entertained. */
  if ( uim_ptr->hardware.uim_ldo.customize_ldo_setting == FALSE ||
        (uim_ptr->flag.recovery_triggered == TRUE &&
         uim_ptr->state.status != UIM_ERR_S) )
  {
    UIMDRV_MSG_HIGH_0(uim_ptr->id,"Recovery in progress or ldo behavior not modified");
    return TRUE;
  }

  UIMDRV_MSG_HIGH_1(uim_ptr->id,"uim_evaluate_req_ldo_voltage_mod - Evaluating req - 0x%x", req);

  switch (req)
  {
    case PMIC_NPA_MODE_ID_UIM_STANDBY:
    {
      if (uim_ptr->hardware.uim_ldo.pdown_allowed_upon_card_removal == FALSE &&
          uim_ptr->hardware.uim_ldo.pdown_allowed_upon_error == FALSE)
      {
        UIMDRV_MSG_HIGH_0(uim_ptr->id,"LDO PDown allowed under all conditions");
        return TRUE;
      }

      if (uim_ptr->hardware.uim_ldo.pdown_allowed_upon_card_removal == TRUE &&
          uim_ptr->command.cmd_ptr != NULL &&
          uim_ptr->command.cmd_ptr->hdr.command == UIM_HOTSWAP_CARD_REM_F)
      {
        UIMDRV_MSG_HIGH_0(uim_ptr->id,"LDO PDown allowed only upon card removal and card is removed");
        return TRUE;
      }

      if (  uim_ptr->hardware.uim_ldo.pdown_allowed_upon_error == TRUE &&
            uim_ptr->state.status != UIM_INITIALIZED_S &&
            uim_ptr->state.status != UIM_CLK_OFF_S )
      {
        UIMDRV_MSG_HIGH_1(uim_ptr->id,"LDO Power Down only upon error. UIM state is 0x%x", uim_ptr->state.status);
        return TRUE;
      }
      break;
    }

    case PMIC_NPA_MODE_ID_UIM_ACTIVE_CLASS_C_LOW:
    case PMIC_NPA_MODE_ID_UIM_ACTIVE_CLASS_C:
    case PMIC_NPA_MODE_ID_UIM_ACTIVE_CLASS_C_HIGH:
    {
      if (uim_ptr->hardware.uim_ldo.desired_voltage_class == UIM_VOLTAGE_CLASS_INVALID ||
          uim_ptr->hardware.uim_ldo.desired_voltage_class == UIM_VOLTAGE_CLASS_C)
      {
        return TRUE;
      }
      break;
    }

    case PMIC_NPA_MODE_ID_UIM_ACTIVE_CLASS_B_LOW:
    case PMIC_NPA_MODE_ID_UIM_ACTIVE_CLASS_B:
    case PMIC_NPA_MODE_ID_UIM_ACTIVE_CLASS_B_HIGH:
    {
      if (uim_ptr->hardware.uim_ldo.desired_voltage_class == UIM_VOLTAGE_CLASS_INVALID ||
          uim_ptr->hardware.uim_ldo.desired_voltage_class == UIM_VOLTAGE_CLASS_B)
      {
        return TRUE;
      }
      break;
    }

    default:
      ERR_FATAL("UIM_%d uim_evaluate_req_ldo_voltage_mod - vote unrecognized",uim_ptr->id,0,0);
  }
  return FALSE;
} /* uim_evaluate_req_ldo_voltage_mod */


/*
  @brief  Check if Exteranl LDO is present.
          if Externa lLDO is present, Configure External LDO related GPIOs.
         
*/
void uim_config_external_ldo
(
  uim_gpios_name_type   *pGpiosName,
  uim_instance_enum_type uim_instance
)
{
  uim_slot_type         physical_slot;
  uint8                 pm_model       = 0;  

  RETURN_IF_INSTANCE_INVALID(uim_instance);

  physical_slot = (uim_slot_type)m_HWConfig.uim_slot_config[uim_instance].uimUart + 1;

  if(( UIM_SLOT_3 == physical_slot ) && 
      (DALCHIPINFO_FAMILY_MSM8909 == uimdrv_hw_nv_config_info.dalChipFamily) )
  {
    uim_hw_if.uim_is_external_ldo[uim_instance] = TRUE;
  }
  else
  {
    uim_hw_if.uim_is_external_ldo[uim_instance] = FALSE;
  }

  pm_model = pm_get_pmic_model((uint8)0);

  if(( UIM_SLOT_1 == physical_slot ) &&
     ( DALCHIPINFO_FAMILY_MSM8909 == uimdrv_hw_nv_config_info.dalChipFamily ) &&
     ( pm_model == PMIC_IS_PM660 ))
  {
    uim_hw_if.uim_is_external_ldo[uim_instance] = TRUE;
  }
  else
  {
    uim_hw_if.uim_is_external_ldo[uim_instance] = FALSE;
  }
  
  if(uim_hw_if.uim_is_external_ldo[uim_instance])
  {
    DalTlmm_GetGpioId( uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                     pGpiosName->UimExtLdoEnGpioName,
                     &(uim_hw_if.gpio[uim_instance].m_UimExtLdoEnGpioId));

    DalTlmm_GetGpioId( uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                     pGpiosName->UimExtLdoVselGpioName,
                     &(uim_hw_if.gpio[uim_instance].m_UimExtLdoVselGpioId));

    uim_hw_if.gpio[uim_instance].m_UimExtLdoEnGpioConfigId.eDirection     = DAL_GPIO_OUTPUT;
    uim_hw_if.gpio[uim_instance].m_UimExtLdoEnGpioConfigId.ePull            = (DALGpioPullType)NO_PULL;
    uim_hw_if.gpio[uim_instance].m_UimExtLdoEnGpioConfigId.eDriveStrength   = (DALGpioDriveType)DS_2MA;

    uim_hw_if.gpio[uim_instance].m_UimExtLdoVselGpioConfigId.eDirection     = DAL_GPIO_OUTPUT;
    uim_hw_if.gpio[uim_instance].m_UimExtLdoVselGpioConfigId.ePull          = (DALGpioPullType)NO_PULL;
    uim_hw_if.gpio[uim_instance].m_UimExtLdoVselGpioConfigId.eDriveStrength = (DALGpioDriveType)DS_2MA;
  }
}/*uim_config_external_ldo*/


/* Configure External LDO of UIM to 1.8v */
boolean uimConfigureUimExtLdoToClassC(uim_instance_enum_type uim_instance)
{
  if( NULL == uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr ||
      0  == uim_hw_if.gpio[uim_instance].m_UimExtLdoEnGpioId ||
      0  == uim_hw_if.gpio[uim_instance].m_UimExtLdoVselGpioId )
  {
    return FALSE;
  }

  DalTlmm_SelectGpioIdMode(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                           uim_hw_if.gpio[uim_instance].m_UimExtLdoVselGpioId,
                           UIM_TLMM_IO_MODE,
                           &(uim_hw_if.gpio[uim_instance].m_UimExtLdoVselGpioConfigId));
  
  (void)DalTlmm_GpioIdOut(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                          uim_hw_if.gpio[uim_instance].m_UimExtLdoVselGpioId,
                          DAL_GPIO_LOW_VALUE);


  DalTlmm_SelectGpioIdMode(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                           uim_hw_if.gpio[uim_instance].m_UimExtLdoEnGpioId,
                           UIM_TLMM_IO_MODE,
                           &(uim_hw_if.gpio[uim_instance].m_UimExtLdoEnGpioConfigId));
  
  (void)DalTlmm_GpioIdOut(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                          uim_hw_if.gpio[uim_instance].m_UimExtLdoEnGpioId,
                          DAL_GPIO_HIGH_VALUE);

  return TRUE;
} /* uimConfigureUimExtLdoToClassC */


/* Configure External LDO of UIM to 3v */
boolean uimConfigureUimExtLdoToClassB(uim_instance_enum_type uim_instance)
{
  if( NULL == uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr ||
      0  == uim_hw_if.gpio[uim_instance].m_UimExtLdoEnGpioId ||
      0  == uim_hw_if.gpio[uim_instance].m_UimExtLdoVselGpioId )
  {
    return FALSE;
  }

  DalTlmm_SelectGpioIdMode(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                           uim_hw_if.gpio[uim_instance].m_UimExtLdoVselGpioId,
                           UIM_TLMM_IO_MODE,
                           &(uim_hw_if.gpio[uim_instance].m_UimExtLdoVselGpioConfigId));
  
  (void)DalTlmm_GpioIdOut(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                          uim_hw_if.gpio[uim_instance].m_UimExtLdoVselGpioId,
                          DAL_GPIO_HIGH_VALUE);

  DalTlmm_SelectGpioIdMode(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                           uim_hw_if.gpio[uim_instance].m_UimExtLdoEnGpioId,
                           UIM_TLMM_IO_MODE,
                           &(uim_hw_if.gpio[uim_instance].m_UimExtLdoEnGpioConfigId));
  
  (void)DalTlmm_GpioIdOut(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                          uim_hw_if.gpio[uim_instance].m_UimExtLdoEnGpioId,
                          DAL_GPIO_HIGH_VALUE);


  return TRUE;
} /* uimConfigureUimExtLdoToClassB */


/* Configure External LDO of UIM to 0v */
boolean uimConfigureUimExtLdoToZero(uim_instance_enum_type uim_instance)
{
  if( NULL == uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr ||
      0  == uim_hw_if.gpio[uim_instance].m_UimExtLdoEnGpioId ||
      0  == uim_hw_if.gpio[uim_instance].m_UimExtLdoVselGpioId )
  {
    return FALSE;
  }

  DalTlmm_SelectGpioIdMode(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                           uim_hw_if.gpio[uim_instance].m_UimExtLdoEnGpioId,
                           UIM_TLMM_IO_MODE,
                           &(uim_hw_if.gpio[uim_instance].m_UimExtLdoEnGpioConfigId));
  
  (void)DalTlmm_GpioIdOut(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                          uim_hw_if.gpio[uim_instance].m_UimExtLdoEnGpioId,
                          DAL_GPIO_LOW_VALUE);

  DalTlmm_SelectGpioIdMode(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                           uim_hw_if.gpio[uim_instance].m_UimExtLdoVselGpioId,
                           UIM_TLMM_IO_MODE,
                           &(uim_hw_if.gpio[uim_instance].m_UimExtLdoVselGpioConfigId));
  
  (void)DalTlmm_GpioIdOut(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                          uim_hw_if.gpio[uim_instance].m_UimExtLdoVselGpioId,
                          DAL_GPIO_LOW_VALUE);


  return TRUE;
} /* uimConfigureUimExtLdoToZero */
