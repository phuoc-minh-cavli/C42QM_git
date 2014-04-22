/*============================================================================
  FILE:         uimdrv_gpio.c

  OVERVIEW:     Contains functions used by uim driver to control and initialize
                gpio/tlmm interface

  DEPENDENCIES: N/A

                Copyright (c) 2012 - 2017 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                QUALCOMM Technologies Confidential and Proprietary
============================================================================*/

/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.  Please
use ISO format for dates.

$Header: //components/rel/uim.mpss/3.3.1/uimdrv/src/hw/gpio_tlmm/uimdrv_gpio.c#1 $
$DateTime: 2020/07/22 10:53:27 $
$Author: pwbldsvc $

 when       who        what, where, why
------      ----       -----------------------------------------------------------
12/26/17    gm         Bringup changes for SDM2500
06/07/16    sam        Stub out Hotswap related functionality
06/07/16    sam        Stub out GPIO Info logging
06/06/16    sam        Stub out external LDO related functionality
03/12/15    ks         SIM tray by tieing the card detect gpios
01/08/14    ks         Compile warnings
01/05/15    ks         supoport for UIM3 external LDO on 8909
12/03/14    sam        Incorporate the newly added TLMM API to read back GPIO configs
10/31/14    nmb        Correct slot typecasting
09/12/14    ssr        Fix to initialize the Hotswap GPIO when Hotswap feature
                       is enabled
08/25/14    ll         Fix compile warnings medium
06/16/14    ll         Switch to new string based TLMM APIs
05/12/14    ak         8994 Bring up changes
05/06/14    ks         8936 bring up changes
04/21/14    rm/akv     Fix for glitch seen on the IO line while powering down
01/29/14    sam        Usage of newer style diag macros
12/27/13    ks         8916 bring up changes
11/27/13    akv        Initialization of hw enumeration based on dal chip family
10/22/13    ll         Check UIM GPIOs as inputs upon timeout w/recovery disabled
10/08/13    nmb        HW header file cleanup
10/08/13    nmb        Changed default card detect GPIO to pull up
09/11/13    na         8x62 bring up changes
08/06/13    rm/ak      Bolt RUMI code
07/12/13    nmb        Global Reorganization
07/12/13    akv        HW Enumeration - modifying target specific hardware values
                       and organizing them in value-key pairs
05/23/13    ak         Apply NV values for PULL and DRIVE STRENGTH for UIM1 dedicated
                       UIM controller pads
05/23/13    ak         Reset CLK, DATA, and RESET PULL to NO-PULL when setting to
                       UIM control
05/22/13    akv        Warm reset fixes for Triton
02/21/13    ak         Use UIM instance number for GPIO configuration logic
02/21/13    ak         UIM1 support on 9x25 v2
02/12/13    js         General F3 macro cleanup
12/05/12    js         UIM Parallel Processing changes
11/21/12    rm         Fix to set the GPIO state to LOW after configuring it them
                       to TLMM ownership
11/09/12    akv        BATT ALARM configuration for Dime
10/26/12    akv/ssr    HW Enumeration changes
============================================================================*/
#include "uimdrv_gpio.h"
#include "uimglobals.h"

/* NOTE: X will be updated based on uim_instance in runtime in updateGpioNameWithInstance() */
#define UIM_GPIO_DATA_NAME                       "uimX_data"
#define UIM_GPIO_CLK_NAME                        "uimX_clk"
#define UIM_GPIO_RESET_NAME                      "uimX_reset"
#define UIM_GPIO_PRESENT_NAME                    "uimX_present"
#define UIM_GPIO_BATT_ALARM_NAME                 "uim_batt_alarm"
#define UIM_GPIO_EXT_LDO_EN                      "uimX_eldo_en"
#define UIM_GPIO_EXT_LDO_VSEL                    "uimX_eldo_vsel"


DALGpioIdType disabled_uim_card_presence_gpio_id;

/*
  @brief Add instant number to GPIO name string based on physical slot number
         This physical slot number should retreive from uimUart
*/
void updateGpioNameWithInstance
(
  uim_gpios_name_type *pGpiosName,
  uim_slot_type uim_slot
)
{
  memset(pGpiosName, 0, sizeof(uim_gpios_name_type));
  strlcpy(pGpiosName->DataGpioName, UIM_GPIO_DATA_NAME, UIM_MAX_GPIO_NAME_LEN);
  pGpiosName->DataGpioName[UIM_TLMM_GPIO_INST_INDEX] = (char)(((int)'0')+ uim_slot);
  strlcpy(pGpiosName->ClkGpioName, UIM_GPIO_CLK_NAME, UIM_MAX_GPIO_NAME_LEN);
  pGpiosName->ClkGpioName[UIM_TLMM_GPIO_INST_INDEX] = (char)(((int)'0')+ uim_slot);
  strlcpy(pGpiosName->ResetGpioName, UIM_GPIO_RESET_NAME, UIM_MAX_GPIO_NAME_LEN);
  pGpiosName->ResetGpioName[UIM_TLMM_GPIO_INST_INDEX] = (char)(((int)'0')+ uim_slot);
  strlcpy(pGpiosName->PresentGpioName, UIM_GPIO_PRESENT_NAME, UIM_MAX_GPIO_NAME_LEN);
  pGpiosName->PresentGpioName[UIM_TLMM_GPIO_INST_INDEX] = (char)(((int)'0')+ uim_slot);
  strlcpy(pGpiosName->BattAlarmGpioName, UIM_GPIO_BATT_ALARM_NAME, UIM_MAX_GPIO_NAME_LEN);

  strlcpy(pGpiosName->UimExtLdoEnGpioName, UIM_GPIO_EXT_LDO_EN, UIM_MAX_GPIO_NAME_LEN);
  pGpiosName->UimExtLdoEnGpioName[UIM_TLMM_GPIO_INST_INDEX] = (char)(((int)'0')+ uim_slot);
  strlcpy(pGpiosName->UimExtLdoVselGpioName, UIM_GPIO_EXT_LDO_VSEL, UIM_MAX_GPIO_NAME_LEN);
  pGpiosName->UimExtLdoVselGpioName[UIM_TLMM_GPIO_INST_INDEX] = (char)(((int)'0')+ uim_slot);
}/* update_gpio_name_with_instance */


/*
  @brief Initialize GPIO: populate device data
*/
void initialize_gpio(uim_instance_enum_type uim_instance)
{
  DALResult             eRes = DAL_SUCCESS;
  uim_gpios_name_type   GpiosName;
  uim_slot_type         physical_slot;

  uim_instance_global_type *uim_ptr;
  uim_ptr = uim_get_instance_ptr(uim_instance); 
  uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr = NULL;

  if(uim_ptr == NULL)
  {
    return;
  }
  /* Attach TLMM and get TLMM handle for this uim instance */
  eRes = DAL_DeviceAttach(DALDEVICEID_TLMM, &(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr));
  if((NULL == uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr)||(eRes != DAL_SUCCESS))
  {
    UIMDRV_MSG_ERR_0(uim_instance,"Failed to register with TLMM DAL");
    return;
  }

  /* Mapping instance to string */
  physical_slot = (uim_slot_type)m_HWConfig.uim_slot_config[uim_instance].uimUart + 1;

  updateGpioNameWithInstance(&GpiosName, physical_slot);

  /* Get GpioId via string */
  DalTlmm_GetGpioId( uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                     GpiosName.DataGpioName,
                     &(uim_hw_if.gpio[uim_instance].m_DataGpioId));
  DalTlmm_GetGpioId( uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                     GpiosName.ClkGpioName,
                     &(uim_hw_if.gpio[uim_instance].m_ClkGpioId));
  DalTlmm_GetGpioId( uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                     GpiosName.ResetGpioName,
                     &(uim_hw_if.gpio[uim_instance].m_ResetGpioId));
  DalTlmm_GetGpioId( uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                     GpiosName.PresentGpioName,
                     &(uim_hw_if.gpio[uim_instance].m_PresentGpioId));
  DalTlmm_GetGpioId( uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                     GpiosName.BattAlarmGpioName,
                     &(uim_hw_if.gpio[uim_instance].m_BattAlarmGpioId));


  if(uim_nv_is_feature_enabled(UIMDRV_FEATURE_SIMTRAY_WITH_GPIOS_TIED,
                               uim_ptr) == TRUE)
  {
    if(uim_hw_slot_info.slot_status[UIM_INSTANCE_1].slot_enabled == FALSE)
    {
      DalTlmm_GetGpioId( uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
              "uim1_present",
              &(disabled_uim_card_presence_gpio_id));
    }
    else if(uim_hw_slot_info.slot_status[UIM_INSTANCE_2].slot_enabled == FALSE)
    {
      DalTlmm_GetGpioId( uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
          "uim2_present",
          &(disabled_uim_card_presence_gpio_id));
    }       
  }
  
   /*Populate device specific configuration data to gpio struct*/
  uim_hw_if.gpio[uim_instance].m_DataGpioConfigId.eDirection          = DAL_GPIO_OUTPUT;
  uim_hw_if.gpio[uim_instance].m_DataGpioConfigId.ePull               = (DALGpioPullType)m_HWConfig.uim_slot_config[uim_instance].uimDataGpioPullSetting;
  uim_hw_if.gpio[uim_instance].m_DataGpioConfigId.eDriveStrength      = (DALGpioDriveType)m_HWConfig.uim_slot_config[uim_instance].uimDataGpioDrvStrength;
  uim_hw_if.gpio[uim_instance].m_ClkGpioConfigId.eDirection           = DAL_GPIO_OUTPUT;
  uim_hw_if.gpio[uim_instance].m_ClkGpioConfigId.ePull                = (DALGpioPullType)m_HWConfig.uim_slot_config[uim_instance].uimClkGpioPullSetting;
  uim_hw_if.gpio[uim_instance].m_ClkGpioConfigId.eDriveStrength       = (DALGpioDriveType)m_HWConfig.uim_slot_config[uim_instance].uimClkGpioDrvStrength;
  uim_hw_if.gpio[uim_instance].m_ResetGpioConfigId.eDirection         = DAL_GPIO_OUTPUT;
  uim_hw_if.gpio[uim_instance].m_ResetGpioConfigId.ePull              = (DALGpioPullType)m_HWConfig.uim_slot_config[uim_instance].uimResetGpioPullSetting;
  uim_hw_if.gpio[uim_instance].m_ResetGpioConfigId.eDriveStrength     = (DALGpioDriveType)m_HWConfig.uim_slot_config[uim_instance].uimResetGpioDrvStrength;
  uim_hw_if.gpio[uim_instance].m_PresentGpioConfigId.eDirection       = DAL_GPIO_INPUT;
  uim_hw_if.gpio[uim_instance].m_PresentGpioConfigId.ePull            = (DALGpioPullType)m_HWConfig.uim_slot_config[uim_instance].uimCardDetectGpioPullSetting;
  uim_hw_if.gpio[uim_instance].m_PresentGpioConfigId.eDriveStrength   = (DALGpioDriveType)m_HWConfig.uim_slot_config[uim_instance].uimCardDetectGpioDrvStrength;
  uim_hw_if.gpio[uim_instance].m_BattAlarmGpioConfigId.eDirection     = DAL_GPIO_OUTPUT;
  uim_hw_if.gpio[uim_instance].m_BattAlarmGpioConfigId.ePull          = (DALGpioPullType)m_HWConfig.uimBattAlarmGpioPullSetting;
  uim_hw_if.gpio[uim_instance].m_BattAlarmGpioConfigId.eDriveStrength = (DALGpioDriveType)m_HWConfig.uimBattAlarmGpioDrvStrength;

  uim_config_external_ldo(&GpiosName, uim_instance);

} /* initialize_gpio */


/*
  @ brief Configures the DATA and CLOCK lines
*/
boolean enableUartInterface(uim_instance_enum_type uim_instance)
{
  if( NULL == uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr ||
      0 == uim_hw_if.gpio[uim_instance].m_DataGpioId     ||
      0 == uim_hw_if.gpio[uim_instance].m_ClkGpioId )
  {
    return FALSE;
  }

  DalTlmm_SelectGpioIdMode(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                           uim_hw_if.gpio[uim_instance].m_DataGpioId,
                           UIM_TLMM_IO_MODE,
                           &(uim_hw_if.gpio[uim_instance].m_DataGpioConfigId));
  DalTlmm_SelectGpioIdMode(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                           uim_hw_if.gpio[uim_instance].m_ClkGpioId,
                           UIM_TLMM_IO_MODE,
                           &(uim_hw_if.gpio[uim_instance].m_ClkGpioConfigId));

  return TRUE;
} /* enableUartInterface */


/*
  @brief  Configures RESET GPIO.
*/
boolean setResetAsOutput(uim_instance_enum_type uim_instance)
{
  if( NULL == uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr ||
      0 == uim_hw_if.gpio[uim_instance].m_ResetGpioId )
  {
    return FALSE;
  }
  DalTlmm_SelectGpioIdMode(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                           uim_hw_if.gpio[uim_instance].m_ResetGpioId,
                           UIM_TLMM_IO_MODE,
                           &(uim_hw_if.gpio[uim_instance].m_ResetGpioConfigId));

  return TRUE;
} /* setResetAsOutput */


/*
  @brief  Brings the RESET line LOW.
*/
boolean assertReset(uim_instance_enum_type uim_instance)
{
  if( NULL == uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr ||
      0 == uim_hw_if.gpio[uim_instance].m_ResetGpioId )
  {
    return FALSE;
  }

  DalTlmm_GpioIdOut(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                    uim_hw_if.gpio[uim_instance].m_ResetGpioId,
                    DAL_GPIO_LOW_VALUE);
  return TRUE;
}/* assertReset */


/*
  @brief Brings the RESET line HIGH.
*/
boolean deassertReset(uim_instance_enum_type uim_instance)
{
  if( NULL == uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr ||
      0 == uim_hw_if.gpio[uim_instance].m_ResetGpioId )
  {
    return FALSE;
  }
  /* Switch the user of the RESET line to UIM controller */
  DalTlmm_SelectGpioIdMode(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                           uim_hw_if.gpio[uim_instance].m_ResetGpioId,
                           UIM_TLMM_UIMCTRL_MODE,
                           &(uim_hw_if.gpio[uim_instance].m_ResetGpioConfigId));
  return TRUE;
}/* deassertReset */


/* Configure Reset for GPIO/TLMM */
boolean uimConfigureResetDefaultGpio(uim_instance_enum_type uim_instance)
{
  if( NULL == uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr ||
      0  == uim_hw_if.gpio[uim_instance].m_ResetGpioId )
  {
    return FALSE;
  }

  DalTlmm_SelectGpioIdMode(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                           uim_hw_if.gpio[uim_instance].m_ResetGpioId,
                           UIM_TLMM_IO_MODE,
                           &(uim_hw_if.gpio[uim_instance].m_ResetGpioConfigId));
  (void)DalTlmm_GpioIdOut(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                          uim_hw_if.gpio[uim_instance].m_ResetGpioId,
                          DAL_GPIO_LOW_VALUE);
  return TRUE;
} /* uimConfigureResetDefaultGpio */


/* Configure data for GPIO/TLMM */
boolean uimConfigureDataDefaultGpio(uim_instance_enum_type uim_instance)
{
  if( NULL ==uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr ||
      0 == uim_hw_if.gpio[uim_instance].m_DataGpioId )
  {
    return FALSE;
  }

  DalTlmm_SelectGpioIdMode(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                           uim_hw_if.gpio[uim_instance].m_DataGpioId,
                           UIM_TLMM_IO_MODE,
                           &(uim_hw_if.gpio[uim_instance].m_DataGpioConfigId));
  DalTlmm_GpioIdOut(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                     uim_hw_if.gpio[uim_instance].m_DataGpioId,
                     DAL_GPIO_LOW_VALUE);
  return TRUE;
} /* uimConfigureDataDefaultGpio */


/* configure clock for GPIO/TLMM */
boolean uimConfigureClkDefaultGpio(uim_instance_enum_type uim_instance)
{
  if( NULL == uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr ||
      0 == uim_hw_if.gpio[uim_instance].m_ClkGpioId )
  {
    return FALSE;
  }

  DalTlmm_SelectGpioIdMode(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                           uim_hw_if.gpio[uim_instance].m_ClkGpioId,
                           UIM_TLMM_IO_MODE,
                           &(uim_hw_if.gpio[uim_instance].m_ClkGpioConfigId));
  DalTlmm_GpioIdOut(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                    uim_hw_if.gpio[uim_instance].m_ClkGpioId,
                    DAL_GPIO_LOW_VALUE);
  return TRUE;
} /* uimConfigureClkDefaultGpio */


/* configure data for UIM controller */
boolean uimConfigureDataUimController(uim_instance_enum_type uim_instance)
{
  if( NULL == uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr ||
      0 == uim_hw_if.gpio[uim_instance].m_DataGpioId )
  {
    return FALSE;
  }
  DalTlmm_SelectGpioIdMode(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                           uim_hw_if.gpio[uim_instance].m_DataGpioId,
                           UIM_TLMM_UIMCTRL_MODE,
                           &(uim_hw_if.gpio[uim_instance].m_DataGpioConfigId));
  return TRUE;
} /* uimConfigureDataUimController */


/* configure clock for UIM controller */
boolean uimConfigureClkUimController(uim_instance_enum_type uim_instance)
{
  if( NULL == uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr ||
      0 == uim_hw_if.gpio[uim_instance].m_ClkGpioId )
  {
    return FALSE;
  }
  DalTlmm_SelectGpioIdMode(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                           uim_hw_if.gpio[uim_instance].m_ClkGpioId,
                           UIM_TLMM_UIMCTRL_MODE,
                           &(uim_hw_if.gpio[uim_instance].m_ClkGpioConfigId));
  return TRUE;
} /* uimConfigureClkUimController */


/* configure bat alarm GPIO */
boolean uimConfigureBattAlarmUimController(uim_instance_enum_type uim_instance)
{
  /* Only configure once by the first task that get BattAlarm GpioId */
  if( NULL == uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr ||
      0 == uim_hw_if.gpio[uim_instance].m_BattAlarmGpioId )
  {
    return FALSE;
  }
  DalTlmm_SelectGpioIdMode(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                           uim_hw_if.gpio[uim_instance].m_BattAlarmGpioId,
                           UIM_TLMM_UIMCTRL_MODE,
                           &(uim_hw_if.gpio[uim_instance].m_BattAlarmGpioConfigId));
  return TRUE;
} /* uimConfigureBattAlarmUimController */


/*===========================================================================
FUNCTION UIM_SET_RESET_AS_OUTPUT_SLOT

DESCRIPTION
  UIM_SET_RESET_AS_OUTPUT configures the GPIO connected to the RESET line.
  UIM1 RESET: GPIO1, FUNC_SEL = 0
  UIM2 RESET: GPIO5, FUNC_SEL = 0
===========================================================================*/
void UIM_SET_RESET_AS_OUTPUT_SLOT(uim_instance_global_type *uim_ptr)
{
  setResetAsOutput(uim_ptr->id);
  return;
} /* UIM_SET_RESET_AS_OUTPUT_SLOT */


/*===========================================================================
FUNCTION ENABLE_UART_INTERFACE_SLOT

DESCRIPTION
  ENABLE_UART_INTERFACE configures the GPIOs connected to the DATA and
  CLOCK lines to utilize the UART (func_sel=1).

  UIM1 DATA: GPIO3, FUNC_SEL = 1
  UIM1 CLOCK: GPIO2, FUNC_SEL = 1
  UIM2 DATA: GPIO7, FUNC_SEL = 1
  UIM2 CLOCK: GPIO6, FUNC_SEL = 1
===========================================================================*/
void ENABLE_UART_INTERFACE_SLOT(uim_instance_global_type *uim_ptr)
{
  enableUartInterface(uim_ptr->id);
  return;
} /* ENABLE_UART_INTERFACE_SLOT */


/*===========================================================================
FUNCTION UIM_CONFIGURE_RESET_DEFAULT_GPIO

DESCRIPTION

===========================================================================*/
void UIM_CONFIGURE_RESET_DEFAULT_GPIO(uim_instance_global_type *uim_ptr)
{
  uimConfigureResetDefaultGpio(uim_ptr->id);
  return;
} /* UIM_CONFIGURE_RESET_DEFAULT_GPIO */


/*===========================================================================
FUNCTION UIM_CONFIGURE_DATA_DEFAULT_GPIO

DESCRIPTION

===========================================================================*/
void UIM_CONFIGURE_DATA_DEFAULT_GPIO(uim_instance_global_type *uim_ptr)
{
  uimConfigureDataDefaultGpio(uim_ptr->id);
  return;
} /* UIM_CONFIGURE_DATA_DEFAULT_GPIO */


/*===========================================================================
FUNCTION UIM_CONFIGURE_CLK_DEFAULT_GPIO

DESCRIPTION

===========================================================================*/
void UIM_CONFIGURE_CLK_DEFAULT_GPIO(uim_instance_global_type *uim_ptr)
{
  uimConfigureClkDefaultGpio(uim_ptr->id);
  return;
} /* UIM_CONFIGURE_CLK_DEFAULT_GPIO */


/*===========================================================================
FUNCTION UIM_CONFIGURE_DATA_FOR_UIM_CONTROLLER

DESCRIPTION

===========================================================================*/
void UIM_CONFIGURE_DATA_FOR_UIM_CONTROLLER(uim_instance_global_type *uim_ptr)
{
  uimConfigureDataUimController(uim_ptr->id);
  return;
} /* UIM_CONFIGURE_DATA_FOR_UIM_CONTROLLER */


/*===========================================================================
FUNCTION UIM_CONFIGURE_CLK_FOR_UIM_CONTROLLER

DESCRIPTION

===========================================================================*/
void UIM_CONFIGURE_CLK_FOR_UIM_CONTROLLER(uim_instance_global_type *uim_ptr)
{
  uimConfigureClkUimController(uim_ptr->id);
  return;
} /* UIM_CONFIGURE_CLK_FOR_UIM_CONTROLLER */


/*===========================================================================
FUNCTION UIM_CONFIGURE_BATT_ALARM_FOR_UICC_SHUTDOWN

DESCRIPTION

===========================================================================*/
void UIM_CONFIGURE_BATT_ALARM_FOR_UICC_SHUTDOWN(uim_instance_global_type *uim_ptr)
{
  uimConfigureBattAlarmUimController(uim_ptr->id);
  return;
} /* UIM_CONFIGURE_BATT_ALARM_FOR_UICC_SHUTDOWN */


/*===========================================================================
FUNCTION UIM_ASSERT_RESET_SLOT

DESCRIPTION
  Brings the RESET line LOW.
===========================================================================*/
void UIM_ASSERT_RESET_SLOT(uim_instance_global_type *uim_ptr)
{
  assertReset(uim_ptr->id);
  return;
} /* UIM_ASSERT_RESET_SLOT */


/*===========================================================================
FUNCTION UIM_DEASSERT_RESET_SLOT

DESCRIPTION
  Brings the RESET line LOW.
===========================================================================*/
void UIM_DEASSERT_RESET_SLOT(uim_instance_global_type *uim_ptr)
{
  deassertReset(uim_ptr->id);
  return;
} /* UIM_DEASSERT_RESET_SLOT */

