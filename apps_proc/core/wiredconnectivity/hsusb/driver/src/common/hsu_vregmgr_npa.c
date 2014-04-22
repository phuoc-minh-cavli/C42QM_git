/*==============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/src/common/hsu_vregmgr_npa.c#1 $
  $DateTime: 2020/01/30 22:49:35 $

  when      who     what, where, why
  --------  ---     ------------------------------------------------------------
  03/01/10  jch     Initial version

==============================================================================*/


#include <comdef.h>
#include <customer.h>

#include "hsu_common_int.h"
#include "hsu_vregmgr.h"

#include "hsu_lpm.h"
#include "hsu_config_selector.h"

#include "npa.h"
#include "hsu_log.h"

/* For External Vbus detection through PMIC */
#include "pmapp_usb.h"
#include "hsu_os_abstract_bus.h"
#include "hsu_os_abstract_bus_ext.h"
#include "hsu_al_task.h"
#include "hsu_core_config_int.h"
#include "hsu_platform.h"

#include "DDITlmm.h"
#include "DDIGPIOInt.h"
#include "DalDevice.h"
#include "DALDeviceId.h"


/*==============================================================================
                  MACRO DEFINITIONS
==============================================================================*/

/*==============================================================================
                  TYPE DECLARATIONS
==============================================================================*/
typedef struct {

  /* HS-USB */
  npa_client_handle hsusb_handle;
  boolean           is_hsusb_ldo_turned_on;

#ifdef CI_MERGE
  /* SS-USB */
  npa_client_handle ssusb_handle;
  boolean           is_ssusb_ldo_turned_on;

  /* HSIC */
  npa_client_handle hsic_handle;
  boolean           is_hsic_ldo_turned_on;

#endif 

  /* PMIC external VBUS detection for cable connection */
  boolean           is_usb_cable_attached;

  /* PMIC external VBUS detection invoked at least once 
  **  This flag is required to trigger PMIC VBUS at least once on boot-up.
  ** After that redundant status from PMIC will be filtered out because
  ** it causes faiure at the client.
  */
  boolean           is_usb_vbus_notif_invoked_prior;
} hsu_vregmgr_ctx_type;

/*==============================================================================
                   LOCAL VARIABLES
==============================================================================*/
static hsu_vregmgr_ctx_type hsu_vregmgr;

static hsu_gpio_handle_ctx_type hsu_gpio_3p3_ldo_ctx = 
 {
   /* h_tlmm         */  NULL,
   /* h_gpio_int     */  NULL,
   /* pin_config     */  0,
 };

#define HSU_3P3_GPIO  (29)

/*==============================================================================
                  LOCAL FUNCTION DECLARATIONS
==============================================================================*/
static void hsusb_vregmgr_vbus_callback_al_queued(void *param);
static void hsu_vregmgr_gpio_register(void);


/*==============================================================================
                    FUNCTION DECLARATIONS
==============================================================================*/
/*===========================================================================
FUNCTION hsu_vregmgr_turn_on

DESCRIPTION
  Turn on vreg.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_vregmgr_turn_on(hsu_vreg_id_type vreg_id)
{
  hsu_platform_info_type const * platform_info = hsu_platform_get_info();
  hsu_vregmgr_ctx_type *ctx = &hsu_vregmgr;
  DALResult                     dal_result;

  switch (platform_info->hsu_platform_id)
  {
    case HSU_PLATFORM_RUMI:
    break;

    default:
    {
      switch(vreg_id)
      {
        case HSU_VREG_ID_HSUSB:
        {
          HSU_ASSERT(ctx->hsusb_handle != NULL);
    
          if (FALSE == ctx->is_hsusb_ldo_turned_on)
          {
            npa_issue_required_request(ctx->hsusb_handle, 
            PMIC_NPA_MODE_ID_USB_HS_PERI_LPM_0);
            ctx->is_hsusb_ldo_turned_on = TRUE;
    
            HSU_ULOG_1(NO_MSG, BUS_LOG, "pmic_npa_mode_id_usb_hs_peri_lpm_0_active, vreg_id %u",
              vreg_id);
          }

          // Call the TLMM DAL API to enable the GPIO29 for turning ON 3P3 LDO
          dal_result = DalTlmm_ConfigGpio(
          hsu_gpio_3p3_ldo_ctx.h_tlmm, 
          hsu_gpio_3p3_ldo_ctx.pin_config,
                 DAL_TLMM_GPIO_ENABLE); 

          dal_result = DalTlmm_GpioOut(
           hsu_gpio_3p3_ldo_ctx.h_tlmm, 
          hsu_gpio_3p3_ldo_ctx.pin_config,
                 DAL_GPIO_HIGH_VALUE);

          HSU_ASSERT(dal_result == DAL_SUCCESS);
        }
        break;
      }
    }
    break;
  }
}

/*===========================================================================
FUNCTION hsu_vregmgr_turn_off

DESCRIPTION
  Turn off vreg.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_vregmgr_turn_off(hsu_vreg_id_type vreg_id)
{
  hsu_platform_info_type const * platform_info = hsu_platform_get_info();
  hsu_vregmgr_ctx_type *ctx = &hsu_vregmgr;
  DALResult                     dal_result;

  switch (platform_info->hsu_platform_id)
  {
    case HSU_PLATFORM_RUMI:
    break;

    default:
    {
      switch(vreg_id)
      {
        case HSU_VREG_ID_HSUSB:
        {
          HSU_ASSERT(ctx->hsusb_handle != NULL);

          if (ctx->is_hsusb_ldo_turned_on)
          {
            if (hsu_lpm_is_ldo_power_collapse_enabled())
            {
               dal_result = DalTlmm_GpioOut(
                 hsu_gpio_3p3_ldo_ctx.h_tlmm, 
                 hsu_gpio_3p3_ldo_ctx.pin_config,
                 DAL_GPIO_LOW_VALUE);

               // Call the TLMM DAL API to enable the GPIO29 for turning off 3P3 LDO
               dal_result = DalTlmm_ConfigGpio(
                            hsu_gpio_3p3_ldo_ctx.h_tlmm, 
                            hsu_gpio_3p3_ldo_ctx.pin_config,
                            DAL_TLMM_GPIO_DISABLE);

               HSU_ASSERT(dal_result == DAL_SUCCESS);

                  
              /* If LDO power collapse is enabled or if HS-USB core is disabled,
              ** vote off the USB LDOs (3.075V and 1.8V)
              */    
              npa_issue_required_request(ctx->hsusb_handle, 
              PMIC_NPA_MODE_ID_USB_HS_POWER_OFF);

              HSU_ULOG_1(NO_MSG, BUS_LOG, "pmic_npa_mode_id_usb_power_off, vreg_id %u",
                vreg_id);              
            }
            else
            {
              npa_issue_required_request(ctx->hsusb_handle, 
              PMIC_NPA_MODE_ID_USB_HS_PERI_LPM_1);

              HSU_ULOG_1(NO_MSG, BUS_LOG, "pmic_npa_mode_id_usb_hs_peri_lpm_1_standby, vreg_id %u",
                vreg_id);              
            }

            ctx->is_hsusb_ldo_turned_on = FALSE;
          }
        }    
        break;
      }
    }
    break;
  }
  
}


/*===========================================================================
FUNCTION hsu_vregmgr_init

DESCRIPTION
  initialize VREGMGR for HS-USB.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_vregmgr_init(void)
{
  hsu_vregmgr_ctx_type *ctx = &hsu_vregmgr;
  hsu_platform_info_type const * platform_info = hsu_platform_get_info();

  switch (platform_info->hsu_platform_id)
  {
    case HSU_PLATFORM_RUMI:
    {
      ctx->is_hsusb_ldo_turned_on = TRUE;
    }
    break;

    default:
    {
      //-----------------------------------------------------------------------
      // HS-USB LDO
      //-----------------------------------------------------------------------  
      ctx->hsusb_handle = npa_create_sync_client(
        PMIC_NPA_GROUP_ID_USB_HS1, 
        "hsusb", 
        NPA_CLIENT_REQUIRED);
    
      if (NULL == ctx->hsusb_handle)
      {
        HSU_ERR_FATAL("HS-USB: PMIC NPA registration failed", 0, 0, 0);
      }

      hsu_vregmgr_gpio_register();
    
      // Assume that VREG is turned-off by default
      ctx->is_hsusb_ldo_turned_on = FALSE; 
    }
    break;
  }
}

/*===========================================================================
FUNCTION hsusb_vregmgr_vbus_callback_al_queued

DESCRIPTION
  Callback for VBUS status from PMIC handlder executed in AL TASK to exit 
  Low Power mode.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
static void hsusb_vregmgr_vbus_callback_al_queued(void *param)
{
  uint32 is_high            = (uint32)param;
  hsu_vregmgr_ctx_type *ctx = &hsu_vregmgr;

  uint32 core_index = hsu_hal_os_bus_core_id(HSU_CORE_CONFIG_USB_IDX);

  //hsu_os_bus_core_exit_lpm(core_index);
  
  // Log XO shutdown and VDD MIN count
  hsu_platform_log_xosd_and_vddmin_info();
  
  if (is_high)
  {
    if (hsu_vregmgr.is_usb_vbus_notif_invoked_prior && ctx->is_usb_cable_attached)
    {      
      HSU_ULOG_1(ERROR_MSG, BUS_LOG, "high_pmic_usb_vbus, dupe_filtered, core_index %u", core_index);
      hsu_event_log(EVT_PMIC_VBUS_HIGH_DUPLICATE, core_index);
    }
    else
    {
      hsu_os_bus_core_exit_lpm(core_index);
        
      hsu_vregmgr.is_usb_vbus_notif_invoked_prior = TRUE;

      HSU_ULOG_1(NO_MSG, BUS_LOG, "high_pmic_usb_vbus, core_index %u", core_index);
      ctx->is_usb_cable_attached = TRUE;
      hsu_os_bus_send_event_to_core(core_index, HSU_OS_BUS_CONNECT_EVT);
    }
  }
  else
  {
    if (hsu_vregmgr.is_usb_vbus_notif_invoked_prior && (FALSE == ctx->is_usb_cable_attached))
    {
      HSU_ULOG_1(ERROR_MSG, BUS_LOG, "low_pmic_usb_vbus, dupe_filtered, core_index %u", core_index);
      hsu_event_log(EVT_PMIC_VBUS_LOW_DUPLICATE, core_index);     
    }
    else
    {
      hsu_os_bus_core_exit_lpm(core_index);  
        
      hsu_vregmgr.is_usb_vbus_notif_invoked_prior = TRUE;

      HSU_ULOG_1(NO_MSG, BUS_LOG, "low_pmic_usb_vbus, core_index %u", core_index);
      ctx->is_usb_cable_attached = FALSE;
      hsu_os_bus_send_event_to_core(core_index, HSU_OS_BUS_DISCONNECT_EVT);
    }
  }
}

/*===========================================================================
FUNCTION hsusb_vregmgr_vbus_callback

DESCRIPTION
  Callback for VBUS status from PMIC.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void hsusb_vregmgr_vbus_callback(boolean is_high)
{  
  HSU_ULOG_1(NO_MSG, BUS_LOG, "pmic_usb_vbus_isr, is_high %u", is_high);

  hsu_al_task_enqueue_cmd(
    hsusb_vregmgr_vbus_callback_al_queued, (void*)is_high);
}


/*===========================================================================
FUNCTION hsu_bootup_delayed_connect_cb

DESCRIPTION
  Delay simulated bootup cable connect

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_bootup_delayed_connect_cb(timer_cb_data_type data)
{
  hsusb_vregmgr_vbus_callback(TRUE);
}


/*===========================================================================
FUNCTION hsu_vregmgr_pmic_vbus_register

DESCRIPTION
  Register for PMIC GPIO for USB VBUS status

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_vregmgr_pmic_vbus_register(void)
{
  pm_err_flag_type err_flag;
  hsu_platform_info_type const * platform_info = hsu_platform_get_info();

  hsu_vregmgr.is_usb_vbus_notif_invoked_prior = FALSE;

  switch (platform_info->hsu_platform_id)
  {
    case HSU_PLATFORM_VIRTIO:
    case HSU_PLATFORM_RUMI:
    {
      static hsu_timer_handle_type bootup_delay_connect_tmr_hnd = NULL;

      HSU_ULOG(NO_MSG, BUS_LOG, "entry");

      hsu_vregmgr.is_usb_cable_attached = FALSE;
      bootup_delay_connect_tmr_hnd = hsu_timer_init(HSU_TIMER_CID_BOOTUP_DELAY_CONNECT, HSU_TIMER_CB_IN_HSU_AL_TASK_WITH_GIANT_MUTEX, hsu_bootup_delayed_connect_cb, NULL);
      hsu_timer_set(&bootup_delay_connect_tmr_hnd, 500, T_MSEC);
      

      err_flag = PM_ERR_FLAG__SUCCESS;
    }
    break;

    default:
    {
      hsu_vregmgr.is_usb_cable_attached = FALSE;
	  err_flag = pmapp_usb_vbus_status_callback(hsusb_vregmgr_vbus_callback);
    }
    break;
  }

  HSU_ASSERT(err_flag == PM_ERR_FLAG__SUCCESS);

  
}


/*===========================================================================
FUNCTION hsu_vregmgr_is_port_attached
===========================================================================*/
boolean hsu_vregmgr_is_usb_cable_attached(void)
{
  return hsu_vregmgr.is_usb_cable_attached;
}

/*===========================================================================
FUNCTION hsu_vregmgr_gpio_register
===========================================================================*/
void hsu_vregmgr_gpio_register()
{
  /* Declare a handle to the TLMM object */
  DALResult           dal_result; 

  /* Attach to a DAL */
  dal_result = DAL_DeviceAttach(
    DALDEVICEID_TLMM, 
    &hsu_gpio_3p3_ldo_ctx.h_tlmm);
  
  HSU_ASSERT((dal_result == DAL_SUCCESS) && hsu_gpio_3p3_ldo_ctx.h_tlmm);
 
  /* Create a GPIO configuration  */
  hsu_gpio_3p3_ldo_ctx.pin_config = DAL_GPIO_CFG(
    HSU_3P3_GPIO,
    0,
    DAL_GPIO_OUTPUT,
    DAL_GPIO_PULL_UP,
    DAL_GPIO_8MA); 


  return;
}


