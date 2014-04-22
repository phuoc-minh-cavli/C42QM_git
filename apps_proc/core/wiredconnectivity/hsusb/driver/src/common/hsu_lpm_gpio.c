/*==============================================================================

                 HSU_LPM_GPIO. C


GENERAL DESCRIPTION
  Handles GPIO related fuctions for GPIO based Suspend,Resume and Remotewakeup events.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  hsu_platform_init should be called before using the API defined in this file.

==============================================================================*/
/*=============================================================================
Copyright (c) 2013 - 2019 by Qualcomm Technologies, Inc.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
=============================================================================*/
/*=============================================================================

                            EDIT HISTORY FOR MODULE
  
This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.
  
$Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/src/common/hsu_lpm_gpio.c#1 $
$DateTime: 2020/01/30 22:49:35 $
    
when     who  what, where, why
-------- ---  ---------------------------------------------------------
02/08/13 ag  Initial version
=============================================================================*/
/*==============================================================================
                        INCLUDE FILES
==============================================================================*/
#include "hsu_lpm_gpio.h"
#include "hsu_log.h"
#include "hsu_common_int.h"
#include "hsu_os_abstract_bus.h"
#include "hsu_conf_sel_i.h"
#include "hsu_platform.h"
#include "hsu_timer.h"

/*=============================================================================
  Forward Declarations
=============================================================================*/
/* Remote Wakeup Timer definitions */
#ifdef FEATURE_HS_USB_F3

/*=============================================================================
  Constants
=============================================================================*/

/*=============================================================================
  Macros
=============================================================================*/
#define HSU_TLMM_LOW_VALUE  0
    
#define HSU_MDM2AP_GPIO_STATUS  DAL_GPIO_CFG(hsu_lpm_gpio_platform_info->gpio_remote_wakeup, 0, DAL_GPIO_OUTPUT, DAL_GPIO_NO_PULL, DAL_GPIO_2MA)
#define HSU_MDM2AP_GPIO_STATUS_INACTIVE DAL_GPIO_CFG_OUT(hsu_lpm_gpio_platform_info->gpio_remote_wakeup, 0, DAL_GPIO_OUTPUT, DAL_GPIO_NO_PULL, DAL_GPIO_2MA, HSU_TLMM_LOW_VALUE)

/*=============================================================================
  Typedefs
=============================================================================*/

/*==============================================================================
  Variables
==============================================================================*/
static hsu_lpm_gpio_remote_wakeup_ctx_type gpio_remote_wakeup_ctx;
static hsu_platform_info_type const *hsu_lpm_gpio_platform_info;
static boolean hsu_is_gpio_init_done = FALSE;

/*==============================================================================
                          FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/
static void hsu_lpm_gpio_timeout_tmr_cb(timer_cb_data_type);

/*===========================================================================
FUNCTION HSU_LPM_GPIO_REMOTE_WAKEUP_GPIO_INIT

DESCRIPTION
  This Initializes the GPIO as OUTPUT for Remote wakeup signaling
  
DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_lpm_gpio_remote_wakeup_gpio_init(void)
{
  DALResult dRes;

  if(!hsu_is_gpio_init_done)
  {
    hsu_lpm_gpio_platform_info = hsu_platform_get_info();
    
    memset(&gpio_remote_wakeup_ctx,0,sizeof(gpio_remote_wakeup_ctx));
    hsu_is_gpio_init_done = TRUE;
    
    dRes = DAL_DeviceAttach(DALDEVICEID_TLMM, &gpio_remote_wakeup_ctx.hsu_gpio_remote_wakeup_ctx.h_tlmm);
    HSU_ASSERT (dRes == DAL_SUCCESS);
    
    dRes = DalDevice_Open(gpio_remote_wakeup_ctx.hsu_gpio_remote_wakeup_ctx.h_tlmm, DAL_OPEN_SHARED);
    HSU_ASSERT (dRes == DAL_SUCCESS);
    
    dRes = DalTlmm_SetInactiveConfig(gpio_remote_wakeup_ctx.hsu_gpio_remote_wakeup_ctx.h_tlmm, 
      (uint32)hsu_lpm_gpio_platform_info->gpio_remote_wakeup,HSU_MDM2AP_GPIO_STATUS_INACTIVE);
    HSU_ASSERT (dRes == DAL_SUCCESS);
    
    gpio_remote_wakeup_ctx.hsu_gpio_remote_wakeup_ctx.pin_config = (DALGpioSignalType)HSU_MDM2AP_GPIO_STATUS;
    
    dRes = DalTlmm_ConfigGpio(gpio_remote_wakeup_ctx.hsu_gpio_remote_wakeup_ctx.h_tlmm, 
      gpio_remote_wakeup_ctx.hsu_gpio_remote_wakeup_ctx.pin_config, DAL_TLMM_GPIO_ENABLE);
    HSU_ASSERT (dRes == DAL_SUCCESS);
    
    gpio_remote_wakeup_ctx.remote_wakeup_timeout_tmr_value = 
      hsu_conf_sel_get_remote_wakeup_timeout();
    gpio_remote_wakeup_ctx.is_remote_wakeup_timeout_tmr_used = 
      gpio_remote_wakeup_ctx.remote_wakeup_timeout_tmr_value ? TRUE : FALSE;
    
    if(gpio_remote_wakeup_ctx.is_remote_wakeup_timeout_tmr_used)
    {
      gpio_remote_wakeup_ctx.remote_wakeup_debug_timer = hsu_timer_init(
        HSU_TIMER_CID__GPIO_REMOTE_WAKEUP_DEBUG,
        HSU_TIMER_CB_IN_HSU_AL_TASK_WITH_GIANT_MUTEX,
        hsu_lpm_gpio_timeout_tmr_cb,
        (timer_cb_data_type)NULL);
    }
    HSU_ULOG_FUNC(DATA_LOG);
  }
}

/*===========================================================================
FUNCTION HSU_LPM_GPIO_REMOTE_WAKEUP_GPIO_HIGH

DESCRIPTION
  The function raises the HSIC_REMOTE_WAKEUP_GPIO to signal Remote Wakeup to AP
DEPENDENCIES
  GPIO should be properly initialized before use.
RETURN VALUE
  None.
SIDE EFFECTS
  None.
===========================================================================*/
void hsu_lpm_gpio_remote_wakeup_gpio_high(void)
{
  DALResult dRes;

  if(hsu_is_gpio_init_done)
  {
    dRes = DalTlmm_GpioOut(gpio_remote_wakeup_ctx.hsu_gpio_remote_wakeup_ctx.h_tlmm, 
      gpio_remote_wakeup_ctx.hsu_gpio_remote_wakeup_ctx.pin_config, DAL_GPIO_HIGH_VALUE);
    HSU_ASSERT(dRes == DAL_SUCCESS);
    
    gpio_remote_wakeup_ctx.is_remote_wakeup_gpio_used = TRUE;

    HSU_ULOG_1(NO_MSG, BUS_LOG, "hsu_gpio_high, remote_wakeup_counter %u", gpio_remote_wakeup_ctx.hsu_remote_wakeup_counter);
    gpio_remote_wakeup_ctx.hsu_remote_wakeup_counter++;

    if (gpio_remote_wakeup_ctx.is_remote_wakeup_timeout_tmr_used)
    {
      /*Start request timeout timer */
      hsu_timer_set(
        &(gpio_remote_wakeup_ctx.remote_wakeup_debug_timer),
        gpio_remote_wakeup_ctx.remote_wakeup_timeout_tmr_value,
        T_MSEC);
    }
  }
}

/*===========================================================================
FUNCTION HSU_LPM_GPIO_REMOTE_WAKEUP_GPIO_LOW

DESCRIPTION
  The function lowers the HSIC_REMOTE_WAKEUP_GPIO after we get a Resume due to Remote Wake up from AP

  DEPENDENCIES
  GPIO should be properly initialized before use.
RETURN VALUE
  None.
SIDE EFFECTS
  None.
===========================================================================*/
void hsu_lpm_gpio_remote_wakeup_gpio_low(void)
{
  DALResult dRes;

  if(hsu_is_gpio_init_done && gpio_remote_wakeup_ctx.is_remote_wakeup_gpio_used)
  {
    dRes = DalTlmm_GpioOut(gpio_remote_wakeup_ctx.hsu_gpio_remote_wakeup_ctx.h_tlmm,
      gpio_remote_wakeup_ctx.hsu_gpio_remote_wakeup_ctx.pin_config, DAL_GPIO_LOW_VALUE);
    HSU_ASSERT(dRes == DAL_SUCCESS);

    HSU_ULOG_FUNC(BUS_LOG);
  }
}

/*===========================================================================
FUNCTION HSU_LPM_LOG_CLEAR_FEATURE_FOR_REMOTE_WAKEUP

DESCRIPTION
  Increment the Clear_Feature counter and cancel the gpio_remote_wakeup timeout timer if armed.

DEPENDENCIES
  Timer should be initialized.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_lpm_gpio_log_clear_feature_for_remote_wakeup(void)
{
  if(gpio_remote_wakeup_ctx.is_remote_wakeup_gpio_used)
  {
    gpio_remote_wakeup_ctx.is_remote_wakeup_gpio_used = FALSE;
  
    if (gpio_remote_wakeup_ctx.is_remote_wakeup_timeout_tmr_used)
    {
      /* Cancel request timeout timer */
      hsu_timer_cancel(&(gpio_remote_wakeup_ctx.remote_wakeup_debug_timer));
    }
    HSU_ULOG_1(NO_MSG, BUS_LOG, "clear_feature, clear_feature_counter %u", gpio_remote_wakeup_ctx.hsu_clear_feature_counter);
    gpio_remote_wakeup_ctx.hsu_clear_feature_counter++;
  }
}

/*===========================================================================
FUNCTION HSU_LPM_IS_GPIO_REMOTE_WAKEUP_REQUIRED

DESCRIPTION
  Due to hardware bug on AP remote wakeup from usb bus does not work if AP is in VDDmin.
  so we need to use GPIO based Remote wakeup. This is the strategy we use for LA

  For GOBI Fusion AP cannot register for GPIO interrupt until the complete USB is in suspend.
  Thus if AP does an selective suspend on MDM and is still awake and MDM does a GPIO based 
  remote wakeup AP would fail to capture it. So for GOBI we check if AP is in VDDmin.
  IF AP in VDDmin use GPIO to Remote Wakeup
  IF AP not in VDDmin use regular usb Remote Wakeup
  
DEPENDENCIES
   GPIO has to be configured at init.

RETURN VALUE
  TRUE: If GPIO based remote wakeup is required, else FALSE.

SIDE EFFECTS

===========================================================================*/
boolean hsu_lpm_gpio_is_gpio_remote_wakeup_required(void)
{
  /* Check whether platform info was initialized */
  if (hsu_lpm_gpio_platform_info)
  { 
    if(((hsu_lpm_gpio_platform_info->hsu_platform_id == HSU_PLATFORM_GOBI_FUSION)
#ifdef FEATURE_HS_USB_VDDMIN_GPIO_FOR_REMOTE_WAKEUP
      &&(hsu_os_abstract_bus_is_apq_in_vddmin_state())
#endif /* FEATURE_HS_USB_VDDMIN_GPIO_FOR_REMOTE_WAKEUP */
        )
        || (hsu_lpm_gpio_platform_info->hsu_platform_id == HSU_PLATFORM_LA_FUSION))
    {
      return TRUE;
    }
  }

  return FALSE;
}

/*===========================================================================
FUNCTION HSU_LPM_HSIC_TIMEOUT_TMR_CB

DESCRIPTION
  This is the callback function that will trigger the HSU_ERR_FATAL if we do not  
  recieve a Clear_Feature from AP withing 1.5 sec from BP signaling a Remote 
  Wakeup
DEPENDENCIES
  Timer should be initialized.
RETURN VALUE
  None
SIDE EFFECTS
  None.
===========================================================================*/
static void hsu_lpm_gpio_timeout_tmr_cb(timer_cb_data_type data)
{
  /* Trigger error fatal */
  HSU_MSG_ERROR("HSU LPM HSIC timed out.");
  if (gpio_remote_wakeup_ctx.is_remote_wakeup_timeout_tmr_used)
  {
    HSU_ERR_FATAL("hsu_lpm_hsic_timeout_tmr_cb()", 0, 0, 0);
  }
}

#endif /* FEATURE_HS_USB_F3 */


