/*============================================================================
  FILE:         uimdrv_gpio_logging.c

  OVERVIEW:     Contains functions used by uim driver to log gpio info

  DEPENDENCIES: N/A

                Copyright (c) 2016 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                QUALCOMM Technologies Confidential and Proprietary
============================================================================*/

/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.  Please
use ISO format for dates.

$Header: //components/rel/uim.mpss/3.3.1/uimdrv/src/hw/gpio_tlmm/uimdrv_gpio_logging.c#1 $
$DateTime: 2020/07/22 10:53:27 $
$Author: pwbldsvc $

 when       who        what, where, why
------      ----       -----------------------------------------------------------
06/06/16    akv/ssr    Intial revision
============================================================================*/
#include "uimdrv_gpio.h"
#include "uimglobals.h"

static void UIM_CONFIGURE_ALL_UIM_AS_INPUT_PULLUP_GPIOS(uim_instance_enum_type uim_instance);
static void UIM_RESTORE_ALL_UIM_GPIOS(uim_instance_enum_type uim_instance);
static boolean uim_populate_hw_gpio_info(uim_instance_global_type *uim_ptr, uim_gpio_info_struct* uim_gpio_info_ptr);

/**
 * Save GPIO register state to globals upon command response
 * timeout(Saves the 10 most recent occurances of command
 * response timeout on an internal or external command).
 *
 * @param uim_ptr Pointer to the global data common.
 * @param is_internal_command Internal or External.
 */
void uim_save_gpio_info(
  uim_instance_global_type *uim_ptr,
  boolean                  is_internal_command
)
{
  uint32 i;
  static uint32 num_times_called = 0;
  boolean uim_populate_hw_gpio_info_status = FALSE;

  if( NULL == uim_ptr )
  {
    UIMDRV_MSG_ERR_0(0, "uim_save_gpio_info: uim_ptr is NULL");
    return;
  }
  RETURN_IF_INSTANCE_INVALID(uim_ptr->id);

  UIMDRV_MSG_HIGH_0(uim_ptr->id, "uim_save_gpio_info in globals");
  /* index for circular buffer */
  i = num_times_called % UIM_NUM_TIMEOUT_INFO;
  if(i >= UIM_NUM_TIMEOUT_INFO)
  {
    UIMDRV_MSG_ERR_1(uim_ptr->id, "uim_save_gpio_info index out of range %d",
                     i);
    i = 0;
  }

  uim_ptr->debug.uim_timeout_gpio_info[i].isInternalCmd  = is_internal_command;
  uim_ptr->debug.uim_timeout_gpio_info[i].num_timeouts   = num_times_called;

  uim_populate_hw_gpio_info_status = uim_populate_hw_gpio_info(uim_ptr, &uim_ptr->debug.uim_timeout_gpio_info[i]);

  if (uim_populate_hw_gpio_info_status)
  {
    UIMDRV_MSG_ERR_3( uim_ptr->id, "Timeout Print UIM GPIO input RST Value:0x%x CLK Value:0x%x DATA Value:0x%x",
                      uim_ptr->debug.uim_timeout_gpio_info[i].gpio_input.reset,
                      uim_ptr->debug.uim_timeout_gpio_info[i].gpio_input.clk,
                      uim_ptr->debug.uim_timeout_gpio_info[i].gpio_input.data);
    UIMDRV_MSG_ERR_1( uim_ptr->id, "Detect Value: 0x%x",
                      uim_ptr->debug.uim_timeout_gpio_info[i].gpio_input.detect);
    UIMDRV_MSG_ERR_3( uim_ptr->id, "Data_CFG:Dir: 0x%x Pull: 0x%x Driv: 0x%x",
                      uim_ptr->debug.uim_timeout_gpio_info[i].data_config.eDirection,
                      uim_ptr->debug.uim_timeout_gpio_info[i].data_config.ePull,
                      uim_ptr->debug.uim_timeout_gpio_info[i].data_config.eDriveStrength);
    UIMDRV_MSG_ERR_3( uim_ptr->id, "CLK_CFG:Dir: 0x%x Pull: 0x%x Driv: 0x%x",
                      uim_ptr->debug.uim_timeout_gpio_info[i].clk_config.eDirection,
                      uim_ptr->debug.uim_timeout_gpio_info[i].clk_config.ePull,
                      uim_ptr->debug.uim_timeout_gpio_info[i].clk_config.eDriveStrength);
    UIMDRV_MSG_ERR_3( uim_ptr->id, "Reset_CFG:Dir: 0x%x Pull: 0x%x Driv: 0x%x",
                      uim_ptr->debug.uim_timeout_gpio_info[i].reset_config.eDirection,
                      uim_ptr->debug.uim_timeout_gpio_info[i].reset_config.ePull,
                      uim_ptr->debug.uim_timeout_gpio_info[i].reset_config.eDriveStrength);

    if(uim_ptr->flag.runtime_disable_recovery)
    {
      UIMDRV_MSG_ERR_3(uim_ptr->id, "Set the UIM lines to pull-up input GPIOs values:RST:0x%x CLK:0x%x DATA:0x%x",
                       uim_ptr->debug.uim_timeout_gpio_info[i].gpio_input_pull_up.reset,
                       uim_ptr->debug.uim_timeout_gpio_info[i].gpio_input_pull_up.clk,
                       uim_ptr->debug.uim_timeout_gpio_info[i].gpio_input_pull_up.data);
      UIMDRV_MSG_ERR_1(uim_ptr->id, "Detect value: 0x%x",
                       uim_ptr->debug.uim_timeout_gpio_info[i].gpio_input_pull_up.detect);
    }
  }
  num_times_called++;
} /* uim_save_gpio_info */


/*===========================================================================

FUNCTION UIM_POPULATE_HW_GPIO_INFO

DESCRIPTION
  Retrieve uim gpio values and configurations

===========================================================================*/
static boolean uim_populate_hw_gpio_info
(
  uim_instance_global_type *uim_ptr,
  uim_gpio_info_struct     *uim_gpio_info_ptr
)
{
  DALGpioValueType             eValue;
  DalTlmm_GpioIdSettingsType   eSetting;
  uim_gpios_name_type          GpiosName;
  uim_instance_enum_type       uim_instance = uim_ptr->id;

  /* Mapping instance to string */
  updateGpioNameWithInstance(&GpiosName, ((uim_instance_enum_type)m_HWConfig.uim_slot_config[uim_instance].uimUart + 1));

  //get GPIO values
  DalTlmm_GpioIdIn(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr, uim_hw_if.gpio[uim_instance].m_DataGpioId, &eValue);
  uim_gpio_info_ptr->gpio_input.data = eValue & 0x1;
  DalTlmm_GpioIdIn(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr, uim_hw_if.gpio[uim_instance].m_ClkGpioId, &eValue);
  uim_gpio_info_ptr->gpio_input.clk  = eValue & 0x1;
  DalTlmm_GpioIdIn(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr, uim_hw_if.gpio[uim_instance].m_ResetGpioId, &eValue);
  uim_gpio_info_ptr->gpio_input.reset = eValue & 0x1;
  DalTlmm_GpioIdIn(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr, uim_hw_if.gpio[uim_instance].m_PresentGpioId, &eValue);
  uim_gpio_info_ptr->gpio_input.detect = eValue & 0x1;

  //get GPIO configs and protocal setting
  DalTlmm_GetGpioIdSettings(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr, uim_hw_if.gpio[uim_instance].m_DataGpioId, &eSetting);
  uim_gpio_info_ptr->data_config.eDirection     =  eSetting.Settings.eDirection;
  uim_gpio_info_ptr->data_config.ePull          =  eSetting.Settings.ePull;
  uim_gpio_info_ptr->data_config.eDriveStrength =  eSetting.Settings.eDriveStrength;

  DalTlmm_GetGpioIdSettings(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr, uim_hw_if.gpio[uim_instance].m_ClkGpioId, &eSetting);
  uim_gpio_info_ptr->clk_config.eDirection      =  eSetting.Settings.eDirection;
  uim_gpio_info_ptr->clk_config.ePull           =  eSetting.Settings.ePull;
  uim_gpio_info_ptr->clk_config.eDriveStrength  =  eSetting.Settings.eDriveStrength;

  DalTlmm_GetGpioIdSettings(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr, uim_hw_if.gpio[uim_instance].m_ResetGpioId, &eSetting);
  uim_gpio_info_ptr->reset_config.eDirection    =  eSetting.Settings.eDirection;
  uim_gpio_info_ptr->reset_config.ePull         =  eSetting.Settings.ePull;
  uim_gpio_info_ptr->reset_config.eDriveStrength=  eSetting.Settings.eDriveStrength;

  if(uim_ptr->flag.runtime_disable_recovery)
  {
    UIM_CONFIGURE_ALL_UIM_AS_INPUT_PULLUP_GPIOS(uim_ptr->id);
    uim_clk_busy_wait (100);

    //get resetted GPIO values
    DalTlmm_GpioIdIn(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr, uim_hw_if.gpio[uim_instance].m_DataGpioId, &eValue);
    uim_gpio_info_ptr->gpio_input_pull_up.data = eValue & 0x1;
    DalTlmm_GpioIdIn(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr, uim_hw_if.gpio[uim_instance].m_ClkGpioId, &eValue);
    uim_gpio_info_ptr->gpio_input_pull_up.clk  = eValue & 0x1;
    DalTlmm_GpioIdIn(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr, uim_hw_if.gpio[uim_instance].m_ResetGpioId, &eValue);
    uim_gpio_info_ptr->gpio_input_pull_up.reset = eValue & 0x1;
    DalTlmm_GpioIdIn(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr, uim_hw_if.gpio[uim_instance].m_PresentGpioId, &eValue);
    uim_gpio_info_ptr->gpio_input_pull_up.detect = eValue & 0x1;

    UIM_RESTORE_ALL_UIM_GPIOS(uim_ptr->id);
  }
  return TRUE;
} /* uim_populate_hw_gpio_info */




/* Configure all UIM lines as pull-up input GPIOs to log their values for debugging */
static void UIM_CONFIGURE_ALL_UIM_AS_INPUT_PULLUP_GPIOS(uim_instance_enum_type uim_instance)
{
  DalTlmm_GpioConfigIdType Settings;

  if( NULL == uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr ||
      0 == uim_hw_if.gpio[uim_instance].m_ResetGpioId    ||
      0 == uim_hw_if.gpio[uim_instance].m_ClkGpioId      ||
      0 == uim_hw_if.gpio[uim_instance].m_DataGpioId     ||
      0 == uim_hw_if.gpio[uim_instance].m_PresentGpioId )
  {
    return;
  }

  uim_memscpy(&Settings,
              sizeof(DalTlmm_GpioConfigIdType),
              &(uim_hw_if.gpio[uim_instance].m_ResetGpioConfigId),
              sizeof(DalTlmm_GpioConfigIdType));

  Settings.ePull           = DAL_GPIO_PULL_UP;

  DalTlmm_SelectGpioIdMode(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                           uim_hw_if.gpio[uim_instance].m_ResetGpioId,
                           UIM_TLMM_IO_MODE,
                           &Settings);

  uim_memscpy(&Settings,
              sizeof(DalTlmm_GpioConfigIdType),
              &(uim_hw_if.gpio[uim_instance].m_ClkGpioConfigId),
              sizeof(DalTlmm_GpioConfigIdType));

  Settings.ePull           = DAL_GPIO_PULL_UP;

  DalTlmm_SelectGpioIdMode(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                           uim_hw_if.gpio[uim_instance].m_ClkGpioId,
                           UIM_TLMM_IO_MODE,
                           &Settings);

  uim_memscpy(&Settings,
              sizeof(DalTlmm_GpioConfigIdType),
              &(uim_hw_if.gpio[uim_instance].m_DataGpioConfigId),
              sizeof(DalTlmm_GpioConfigIdType));

  Settings.ePull           = DAL_GPIO_PULL_UP;

  DalTlmm_SelectGpioIdMode(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                           uim_hw_if.gpio[uim_instance].m_DataGpioId,
                           UIM_TLMM_IO_MODE,
                           &Settings);

  if(TRUE == uim_hw_if.intctrl[uim_instance].m_hs_enable)
  {

    uim_memscpy(&Settings,
                sizeof(DalTlmm_GpioConfigIdType),
                &(uim_hw_if.gpio[uim_instance].m_PresentGpioConfigId),
                sizeof(DalTlmm_GpioConfigIdType));

    Settings.ePull           = DAL_GPIO_PULL_UP;

    DalTlmm_SelectGpioIdMode(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                             uim_hw_if.gpio[uim_instance].m_PresentGpioId,
                             UIM_TLMM_IO_MODE,
                             &Settings);
  }
}/*UIM_CONFIGURE_ALL_UIM_AS_INPUT_PULLUP_GPIOS*/




static void UIM_RESTORE_ALL_UIM_GPIOS(uim_instance_enum_type uim_instance)
{
  if( NULL == uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr ||
      0 == uim_hw_if.gpio[uim_instance].m_ResetGpioId    ||
      0 == uim_hw_if.gpio[uim_instance].m_ClkGpioId      ||
      0 == uim_hw_if.gpio[uim_instance].m_DataGpioId     ||
      0 == uim_hw_if.gpio[uim_instance].m_PresentGpioId )
  {
    return;
  }

  DalTlmm_SelectGpioIdMode(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                           uim_hw_if.gpio[uim_instance].m_ResetGpioId,
                           UIM_TLMM_IO_MODE,
                           &(uim_hw_if.gpio[uim_instance].m_ResetGpioConfigId));
  DalTlmm_SelectGpioIdMode(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                           uim_hw_if.gpio[uim_instance].m_ClkGpioId,
                           UIM_TLMM_IO_MODE,
                           &(uim_hw_if.gpio[uim_instance].m_ClkGpioConfigId));
  DalTlmm_SelectGpioIdMode(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                           uim_hw_if.gpio[uim_instance].m_DataGpioId,
                           UIM_TLMM_IO_MODE,
                           &(uim_hw_if.gpio[uim_instance].m_DataGpioConfigId));

  if(TRUE == uim_hw_if.intctrl[uim_instance].m_hs_enable)
  {
    DalTlmm_SelectGpioIdMode(uim_hw_if.gpio[uim_instance].m_TlmmHandle_ptr,
                             uim_hw_if.gpio[uim_instance].m_PresentGpioId,
                             UIM_TLMM_IO_MODE,
                             &(uim_hw_if.gpio[uim_instance].m_PresentGpioConfigId));
  }
}/*UIM_RESTORE_ALL_UIM_GPIOS*/
