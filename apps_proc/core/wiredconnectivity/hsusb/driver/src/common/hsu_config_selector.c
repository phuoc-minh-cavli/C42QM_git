/*==============================================================================

High Speed USB Configuration Selector Module

GENERAL DESCRIPTION
This module is responsible for the initialization of the High Speed USB 
stack. 

EXTERNALIZED FUNCTIONS
hsu_config_selector_init
hsu_get_num_of_net_instances
hsu_conf_sel_get_cur_comp_details
hsu_conf_sel_get_rm_wakeup_delay
hsu_conf_sel_get_cur_comp_id
hsu_conf_sel_switch_dev_stack
hsu_conf_sel_stack_suspend_mode_changed
hsu_conf_sel_diag_is_on_legacy_port
hsu_conf_sel_uim_first_inst_class
hsu_conf_sel_use_pmic_otg_comparators
hsu_conf_sel_number_of_sdcc_luns
hsu_conf_sel_disable_sleep_voting
hsu_conf_sel_is_hsusb_initialized
hsu_conf_sel_update_bmp_composition
hsu_conf_sel_get_bmp_device_info
hsu_conf_sel_hs_main_core_enabled
hsu_conf_sel_hsic_core_enabled
hsu_conf_sel_is_diag_over_hsic 
hsu_conf_sel_hide_ftl

INITALIZATION AND SEQUENCING REQUIREMENTS
hsu_config_selector_init() should be called after DSM pools are initalized,
and before the creation of tasks that use USB, such as DIAG, Modem, RmNET, 
etc...

Copyright (c) 2007, 2009, 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when      who     what, where, why
--------  ---     ------------------------------------------------------------
09/12/06  ke      Initial version

==============================================================================*/

/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include "qapi_usb_internal.h"
#include "customer.h" /* for FEATURE_HSU_TEST to work */
#include "comdef.h"
#include "err.h"
#include "hsu_config_selector.h"
#include "hsu_conf_sel_types_int.h"
#include "jtypes_int.h"
#include "jos_int.h"
#include "jslave_init.h"
#include "hsu_conf_sel_i.h"
#include "hsu_conf_sel_stack_utils_int.h"
#include "hsu_conf_sel_comp_utils.h"
#include "hsu_common_int.h"
#include "hsu_al_ser.h"
#include "hsu_al_ser_i.h"
#include "hsu_conf_sel_nv.h"
#include <stringl/stringl.h>


#ifdef FEATURE_HS_USB_F3
#include "DDITlmm.h"
#include "DDIChipInfo.h"
#include "DALDeviceId.h"
#include "DDIPlatformInfo.h"
#endif /* FEATURE_HS_USB_F3 */

#ifdef FEATURE_HS_USB_MS_FD
#include "hsu_al_ms_int.h"
#endif /*FEATURE_HS_USB_MS_FD*/

#include "hsu_apis.h"
#include "hsu_hwio_dependency.h"

#if defined(FEATURE_HS_USB_OTG)
#include "hsu_host_al_ms.h"
#endif /* FEATURE_HS_USB_OTG */

#ifdef FEATURE_HS_USB_OTG
#include "hsu_host_al_hid.h"
#endif  /* FEATURE_HS_USB_OTG */

#include "nv_items.h"
#include "hsu_conf_sel_ui_int.h"

#ifdef FEATURE_HSU_ECM_APP_STUB
#include "hsu_ecm_app_stub.h"
#endif /* FEATURE_HSU_ECM_APP_STUB */

#ifdef FEATURE_HSU_ECM_HOST_APP_STUB
#include "hsu_ecm_host_app_stub.h"
#endif /* FEATURE_HSU_ECM_HOST_APP_STUB */

#if defined(FEATURE_HSU_EEM_APP_STUB)
#include "hsu_eem_app_stub.h"
#endif /* FEATURE_HSU_EEM_APP_STUB */

#include "hsu_al_ecm_int.h"

#ifdef FEATURE_HS_USB_OBEX_PERF_TESTS
#include "hsu_obex_perf_test_app.h"
#endif /*FEATURE_HS_USB_OBEX_PERF_TESTS*/

#ifdef FEATURE_HS_USB_ACM_PERF_TESTS
#include "hsu_acm_test.h"
#endif /*FEATURE_HS_USB_ACM_PERF_TESTS*/

#ifdef FEATURE_HS_USB_SAMPLE_LOOPBACK
#include "hsu_cd_sample_loopback.h"
#endif /* FEATURE_HS_USB_SAMPLE_LOOPBACK */

#include "HAL_hsusb.h"

#include "hsu_conf_sel_core_config.h"
#include "hsu_core_config_int.h"
#include "hsu_core_config.h"

#include "hsu_al_dsm_utils.h"

#if defined (FEATURE_HS_USB_OS_DETECTION)
#include "hsu_os_detection.h"
#endif /* FEATURE_HS_USB_OS_DETECTION */

#include "hsu_vregmgr.h"
#include "hsu_clkmgr.h"
#include "hsu_lpm.h"

#include "hsu_event_notifier.h"
#include "hsu_phy_utils_int.h"

#ifdef FEATURE_HS_USB_BAM
#include "hsu_al_bam_utils.h"
#endif /* FEATURE_HS_USB_BAM */

#include "hsu_os_abstract_bus.h"

#if defined(FEATURE_HS_USB_BRINGUP_POLL_INTR) || defined(FEATURE_HS_USB_MEM_TEST)
#include "hsu_al_task.h"
#endif /* FEATURE_HS_USB_BRINGUP_POLL_INTR || FEATURE_HS_USB_MEM_TEST */

#ifdef FEATURE_HS_USB_HEALTH_MONITOR
  #include "hsu_health_diag_init.h"
#endif /* FEATURE_HS_USB_HEALTH_MONITOR */

#include "hsu_log.h"
#include "hsu_platform.h"

/* For QDSS markers */
#include "tracer.h"

#ifndef USB_THREADX
#ifndef FEATURE_HS_USB_PRE_SILICON_BU
#include "tracer_event_ids.h"
#endif // FEATURE_HS_USB_PRE_SILICON_BU
#endif

#include "hsu_bam_sm.h"
//#include "cdc_gsi.h"

#include "HALhwio.h"

/*==============================================================================

DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================
Constants and Macros
==============================================================================*/
#define HS_USB_UIM_FIRST_INST_CLASS_DEFAULT 2
#ifdef FEATURE_HSU_EEM_APP_STUB
  #define HS_USB_WAIT_BEFORE_TEST_PING 10000 /* 10 sec */
#endif

#if defined(FEATURE_HS_USB_BRINGUP_POLL_INTR)
/* USB interrupt polling timer interval in milliseconds */
#ifndef HSU_CONF_SEL_BRINGUP_POLL_INTR_INTERVAL_MS
  #define HSU_CONF_SEL_BRINGUP_POLL_INTR_INTERVAL_MS (3)
#endif /* HSU_CONF_SEL_BRINGUP_POLL_INTR_INTERVAL_MS */
#endif /* FEATURE_HS_USB_BRINGUP_POLL_INTR */



/*==============================================================================
Forward declarations
==============================================================================*/
void hsu_api_init_phy_2nd_core(void);
boolean hsu_conf_sel_enable_link_pwr_mgmt(void);
static void update_is_ecm_int_ep_sharing_supported(void);
static void hsu_conf_sel_assign_iface_slot_index_from_comp(const hsu_conf_sel_comp_info_type *);
/*==============================================================================
Typedefs
==============================================================================*/

/*==============================================================================
Variables
==============================================================================*/

// Holds the value of primary usb core index 
static usb_hal_core_type hsu_core_config_usb_idx = HSU_CORE_CONFIG_USB3_IDX;

/* Holds whether hsu_config_selector_init was completed */
static boolean hsu_is_initialized = FALSE;


#if (defined FEATURE_HSU_ECM_APP_STUB) || \
  (defined FEATURE_HS_USB_ECM_PERFORMANCE_TESTS)
boolean is_ecm_app_stub_active = TRUE;
#endif /*FEATURE_HSU_ECM_APP_STUB || FEATURE_HS_USB_ECM_PERFORMANCE_TESTS*/


#ifdef FEATURE_HS_USB_BREWMP_COMPOSITION
static hsu_conf_sel_brewmp_device_data_type brewmp_device_data = {0};
#endif  /* FEATURE_HS_USB_BREWMP_COMPOSITION */

#if defined(FEATURE_HS_USB_BRINGUP_POLL_INTR)
static rex_timer_type hsu_conf_sel_bringup_poll_tmr;
#endif /* FEATURE_HS_USB_BRINGUP_POLL_INTR */

#ifdef FEATURE_HS_USB_ECM_INT_EP_SHARING    
static boolean ecm_is_ep_sharing_supported = FALSE;
#endif /* FEATURE_HS_USB_ECM_INT_EP_SHARING */

static uint8 hsu_iface_index_for_comp_array[NUM_OF_HSU_IFACE];
/*==============================================================================

FUNCTION DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================

FUNCTION          register_cust_class_driver

DESCRIPTION
This function is the recommended location where customer defined class driver 
is initialized. Refer to hsu_cd_sample_loopback_init as an example.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

==============================================================================*/
static void register_cust_class_driver(void) 
{
#ifdef FEATURE_HS_USB_SAMPLE_LOOPBACK
  hsu_cd_sample_loopback_init();
#endif /* FEATURE_HS_USB_SAMPLE_LOOPBACK */
}

/*==============================================================================

FUNCTION          is_valid_composition_id

DESCRIPTION
This function runs several tests to check if the given composition id is valid.

DEPENDENCIES
None.

RETURN VALUE
TRUE - valid, FALSE - invalid.

SIDE EFFECTS
None.

==============================================================================*/

static boolean is_valid_composition_id(hsu_composition_enum composition_id, boolean is_flashless_boot)
{
  boolean is_valid = TRUE;
#ifdef CI_MERGE
  boolean is_ser4_present = FALSE;
  boolean is_net2_to_net8_present = FALSE;
  uint8 i;
#endif
#ifdef FEATURE_HS_USB_F3
  hsu_platform_info_type const *platform_info;
#endif /* FEATURE_HS_USB_F3 */
  const hsu_conf_sel_comp_info_type* composition_info;

  /* It is possible that the value is a FS composition value, or just "garbage". 
  If so, we should try to write the default HSU composition to the NV item. 
  This is because FS and HS are not active at the same time on any MSM, so 
  we're not running over anything "correct". */
  if(hsu_conf_sel_comp_utils_comp_id_is_valid_for_hsu(composition_id) == FALSE)
  {
    HSU_MSG_HIGH_1(
      "is_valid_composition_id(): CURRENT_COMPOSITION value out of range (%d)",
      composition_id);
    return FALSE;
  }

  composition_info = hsu_conf_sel_comp_utils_get_comp_details(composition_id);
  HSU_ASSERT(composition_info);
#ifdef CI_MERGE
  for (i =0; i< NUM_OF_HSU_IFACE; i++)
  {
    if (composition_info->iface_type[i] == SER4_HSU_IFACE)
    {
      is_ser4_present = TRUE;
    }
    if ((composition_info->iface_type[i] >= NET2_HSU_IFACE) && (composition_info->iface_type[i] <= NET8_HSU_IFACE))
    {
      is_net2_to_net8_present = TRUE;
    }
  }

  // Check if there is more than on RMNET 
  if (is_net2_to_net8_present)
  {
    HSU_MSG_HIGH_1(
          "is_valid_composition_id(): Composition (%d) have RMNET other than NET1, Target only support NET1. Switching to default.", composition_id);
    is_valid = FALSE;
  }
  
  // For remote_efs flashless, efs sync SER4 port is require.
  if (is_flashless_boot && !is_ser4_present)  
  {
    HSU_MSG_HIGH_1(
        "is_valid_composition_id(): The hardware detect to run remote_efs flashless and require SER4 port. (Composition= (%d).) Switching to default", composition_id);
    is_valid = FALSE;
  }
#endif

#ifdef FEATURE_HS_USB_MS_FD
  /* If the current composition includes MS, check the number of luns.
  If we have 0 luns  - current composition is not valid and we need to 
  switch back to default composition. */
  if ((composition_info->num_of_fd_instances_in_comp[HSU_CFG_SEL_MS_FD] > 0) &&
    (hsu_al_ms_get_num_luns() == 0))
  {
    HSU_MSG_HIGH(
      "is_valid_composition_id(): Composition with MS was chosen but there are 0 luns. Switching to default.");
    is_valid = FALSE;
  }
#endif /*FEATURE_HS_USB_MS_FD*/

#ifdef FEATURE_HS_USB_F3
  platform_info = hsu_platform_get_info();

  switch(platform_info->hsu_platform_id)
  {
    case HSU_PLATFORM_LA_FUSION:
    {
      if (composition_info->supported_drv_by_comp != HSU_LA_FUSION_NO_ECM_CMP)
      {
        is_valid = FALSE;
      }
    }
    break;

    case HSU_PLATFORM_GOBI_FUSION:
    {
      if (composition_info->supported_drv_by_comp != HSU_GOBI_FUSION_CMP)
      {
        is_valid = FALSE;
      }
    }
    break;
  }
#endif /* FEATURE_HS_USB_F3 */

  return is_valid;

}  /* is_valid_composition_id */

/*==============================================================================

FUNCTION          hsu_conf_sel_config_get_core_idx

DESCRIPTION
This function returns value of the primary usb core index.

DEPENDENCIES
None.

RETURN VALUE
Returns value of the primary usb core index.

SIDE EFFECTS
None.

==============================================================================*/
usb_hal_core_type hsu_conf_sel_config_get_core_idx(void)
{
  if(hsu_core_config_usb_idx >= HSU_MAX_NUM_OF_CORES)
  {
    HSU_ERR_FATAL("Invalid USB core index", hsu_core_config_usb_idx, 0, 0);
    hsu_core_config_usb_idx = HSU_CORE_CONFIG_USB3_IDX; // for KW
  }
  return hsu_core_config_usb_idx;
}

#ifdef FEATURE_HSU_ECM_APP_STUB
/*==============================================================================

FUNCTION          INIT_ECM_APP_STUB_IF_NEEDED

DESCRIPTION
Called when either FEATURE_HSU_TEST or FEATURE_HSU_ECM_APP_STUB are defined.
If ECM is part of the composition, it initializes the ECM application stub 
task. 

DEPENDENCIES
cur_dev_comp_details must already be set (the function 
update_device_composition_from_nv() should have been called)

RETURN VALUE
None.

SIDE EFFECTS
None.

==============================================================================*/
static void init_ecm_app_stub_if_needed()
{
  struct hsu_device_params_type* hs_main_core_conf_sel_info = NULL;

  /* 
  * ECM application stub should be initialized when FEATURE_HSU_TEST is used, 
  * and also when FEATURE_HSU_ECM_APP_STUB is used, if the network function is 
  * part of the used composition. 
  */

  hs_main_core_conf_sel_info = hsu_conf_sel_core_config_get_usb3_core_conf_sel_info();
  if (hs_main_core_conf_sel_info == NULL)
  {
    HSU_ERR_FATAL("init_ecm_app_stub_if_needed(): hs_main_core_conf_sel_info is NULL", 0, 0, 0);
  } 
  if (hs_main_core_conf_sel_info->configs_arr[HSU_CONFIG_IDX_0].cur_dev_comp_details == NULL)
  {
    HSU_ERR_FATAL("init_ecm_app_stub_if_needed(): cur_dev_comp_details is NULL", 0, 0, 0);
  }

  if(hs_main_core_conf_sel_info->configs_arr[HSU_CONFIG_IDX_0].cur_dev_comp_details->num_of_fd_instances_in_comp[HSU_CFG_SEL_ECM_FD] > 0)  
  {
    if(hsu_ecm_app_stub_init_task() != TRUE)
    {
      HSU_ERR_FATAL("init_ecm_app_stub_if_needed(): hsu_ecm_app_stub_init_task() failed", 0, 0, 0);
    }
  }
} /* init_ecm_app_stub_if_needed */
#endif /* FEATURE_HSU_ECM_APP_STUB */

#if defined(FEATURE_HSU_EEM_APP_STUB)
/*==============================================================================

FUNCTION          INIT_EEM_APP_STUB

DESCRIPTION
Init the EEM application stub, and use it to send 1 IP PING command to the 
EEM device.

DEPENDENCIES
FEATURE_HSU_EEM_APP_STUB is defined.

RETURN VALUE
None.

SIDE EFFECTS
None.

==============================================================================*/
static void init_eem_app_stub()
{
  /* Initialize the EEM application stub */
  if(hsu_eem_app_stub_init_task() != TRUE)
  {
    HSU_ERR_FATAL("init_eem_app_stub(): hsu_eem_app_stub_init_task() failed", 0, 0, 0);
  }

  /* Open the SIO EEM data channel */
  if(hsu_eem_app_stub_sio_open() != TRUE)
  {
    HSU_ERR_FATAL("init_eem_app_stub(): hsu_eem_app_stub_sio_open() failed", 0, 0, 0,);
  }

  /* We don't want to try to send the PING before the device is enumerated */
  hsu_qurt_sleep_ms(HS_USB_WAIT_BEFORE_TEST_PING);
  hsu_eem_app_stub_test_bulk_tx_ping();
} /* init_eem_app_stub */
#endif /* #if defined(FEATURE_HSU_EEM_APP_STUB) */

/*==============================================================================

FUNCTION          CHECK_UPDATE_DEVICE_COMPOSITION

DESCRIPTION
Read EFS item for the current USB device composition, and set the static 
variable cur_dev_comp_details to point to the composition information.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
If the current value of the NV item is invalid, it may be over-written.

==============================================================================*/
static void check_update_device_composition() 
{
  hsu_composition_enum nv_composition_id;
  const hsu_conf_sel_comp_info_type * nv_composition_info = NULL;

#if defined(FEATURE_HS_USB_SNPS_CORE)
  {
  boolean should_write_value = FALSE;
  const hsu_conf_sel_comp_info_type * default_comp_info;
  uint32 fast_boot_setting =  HWIO_BOOT_CONFIG_INM(HWIO_BOOT_CONFIG_FAST_BOOT_BMSK) >> HWIO_BOOT_CONFIG_FAST_BOOT_SHFT;
  
  // 0x2 is usb (flashless boot)
  boolean is_flashless_boot = (fast_boot_setting == HWIO_BOOT_CONFIG_FAST_BOOT_USB_FVAL); 
  
  /* Update composition from NV only for HS-OTG core */
  nv_composition_id = 
    (hsu_composition_enum)hsu_conf_sel_get_device_composition();
  
  // Setting default composition for below 2 nv_composition since they are not supported in Twizy 
  if ((nv_composition_id == EFS_HSU_DIAG_SER3_SER5_SER6_MDM_NET1) || (nv_composition_id == EFS_HSU_DIAG_NMEA_MDM1_MDM2_NET))
  {
	should_write_value = TRUE;
  }
  else
  {
	// In case of invalid composition update nv item with default composition value
    should_write_value = !is_valid_composition_id(nv_composition_id, is_flashless_boot);
  }

  if(should_write_value == TRUE)
  {
    if (is_flashless_boot)  
    {
      default_comp_info = hsu_conf_sel_comp_utils_get_comp_details(EFS_HSU_DIAG_SER4_QDSS_MDM_NET1);
    }
    else
    {
      default_comp_info = hsu_conf_sel_comp_utils_get_default_comp_info();
    }

    if(default_comp_info == NULL)
    {
      HSU_ERR_FATAL("update_device_composition_from_nv(): "
        "hsu_conf_sel_comp_utils_get_default_comp_info returned NULL", 0, 0, 0);
    }    
#ifndef FEATURE_HS_USB_ACM_BAM
    nv_composition_id = default_comp_info->efs_comp_id;
#else /* FEATURE_HS_USB_ACM_BAM */
    /* A2 has only three streams that can be use.Three RmNET instances and MDM 
    can not use SPS simultaneous therefore in case that FEATURE_HS_USB_ACM_BAM 
    is defined we switch back to XXX_MDM_NET2 composition. */
    switch(nv_composition_id)
    {
    case EFS_HSU_DIAG_NMEA_MS_MDM_NET3:
      nv_composition_id = EFS_HSU_DIAG_NMEA_MS_MDM_NET2;
      break;
    case EFS_HSU_DIAG_NMEA_MDM_NET3:
      nv_composition_id = EFS_HSU_DIAG_NMEA_MDM_NET2;
      break;
    default:
	  nv_composition_id = default_comp_info->efs_comp_id;
      break;
    }
#endif /* !FEATURE_HS_USB_ACM_BAM */

    
    if(FALSE == hsu_conf_sel_nv_efs_write(HSU_EFS_DEVICE_COMPOSITION, (uint32)nv_composition_id))
    {
      HSU_ERR_FATAL("update_device_composition_from_nv(): "
        "hsu_conf_sel_nv_efs_dev_comp_write failed.", 0, 0, 0);
    }
    else
    {
      hsu_conf_sel_nv_efs_update(HSU_EFS_DEVICE_COMPOSITION);
      HSU_MSG_MED_1(
        "update_device_composition_from_nv(): Wrote %d to CURRENT_COMPOSITION",
        nv_composition_id);
    }
  }

#ifdef T_UNDP
  /* For UNDP (U2) cards, ask the card for the nv_composition_id info */
  {
    hsu_composition_enum undp_nv_comp_id;
    if (TRUE == hsu_get_undp_hsu_comp_id(&undp_nv_comp_id))
    {
      /* Update nv_composition_id value from UNDP */
      nv_composition_id = undp_nv_comp_id;
    }
  }
#endif /* T_UNDP */
  }
  nv_composition_info = hsu_conf_sel_comp_utils_get_comp_details(nv_composition_id);
#endif /* FEATURE_HS_USB_SNPS_CORE */

  if (nv_composition_info) 
  {
    hsu_conf_sel_core_config_set_cores_comp(nv_composition_info);
    hsu_conf_sel_assign_iface_slot_index_from_comp(nv_composition_info);
  }
  else
  {
    HSU_ERR_FATAL("check_update_device_composition: Unknown nv_composition_info", 
      nv_composition_info, 0, 0);
  }

} /* update_device_composition_from_nv */

/*==============================================================================

FUNCTION          HSU_CONF_SEL_INIT_ADAPT_LAYERS

DESCRIPTION
Initializa the HS-USB adaptation layers.

DEPENDENCIES
None, except for those noted above.

RETURN VALUE
None.

SIDE EFFECTS
None.

==============================================================================*/
static void hsu_conf_sel_init_adapt_layers(void)
{
  /**********************************/
  /* Initialize CDC ALs and related */
  /**********************************/ 

  /* Initialization of contiguous buffer pool has to be done before giving
  stack memory to Jungo, to prevent Jungo starting to allocate before
  hsu_conf_sel_stack_utils_alloc_stack_memory is called for the last time. */
  hsu_al_dsm_utils_init_contiguous_buf_pool();

  if(hsu_al_ser_init() == FALSE)
  {
    HSU_ERR_FATAL("*_init(): hsu_al_ser_init returned FALSE", 0, 0, 0);
  }

#ifdef FEATURE_HS_USB_OTG
  if(hsu_host_al_hid_init() == FALSE)
  {
    HSU_ERR_FATAL("*_init(): hsu_host_al_hid_init failed.", 0, 0, 0);
    return;
  }
#endif  /* FEATURE_HS_USB_OTG */

#if defined(FEATURE_HS_USB_OTG)
  if(hsu_host_al_ms_init() == FALSE)
  {
    HSU_ERR_FATAL("*_init(): hsu_host_al_ms_init failed.", 0, 0, 0);
    return;
  }
#endif  /* FEATURE_HS_USB_OTG */
}  /* hsu_conf_sel_init_adapt_layers */

void hsu_conf_sel_usb_rumi_bu_init(void)
{
// RUMI Phy Base Address for primary, sec and MP controllers
// Subject to change per RUMI release
// NSK : Moving this to a more chipset specific file because we are trying to share between Chiron and Carrera 
//#define RUMI_PHY_PRIM                            (USB3_PRI_USB31_PRIM_MVS_WRAPPER_BASE_PHYS + 0x00120000)  // 0x0A720000

#define HWIO_HS2_EMU_VERSION_OFFS                (0x00000000)
#define HWIO_HS2_LOCAL_RESET_REG_OFFS            (0x00000004)
#define HWIO_HS2_CLK_STATUS_OFFS                 (0x00000010)
#define HWIO_HS2_CLK_STATUS_SEL_OFFS             (0x00000014)
#define HWIO_HS2_IMEM_DELAY_CTRL_OFFS            (0x00000100)
#define HWIO_HS2_IMEM_DELAY_MIN_DELAY_OFFS       (0x00000104)
#define HWIO_HS2_USB_CLK_CTRL_OFFS               (0x00000020)
#define HWIO_HS2_USB_CLK_STATUS_OFFS             (0x00000024)
#define HWIO_HS2_QRBTC_CONTROL_OFFS              (0x00000028)
#define HWIO_HS2_QRBTC_CONTROL_OE_OFFS           (0x0000002c)
#define HWIO_HS2_USB30_CTRL_OFFS                 (0x00000034)
#define HWIO_HS2_USB30_STATUS_OFFS               (0x00000038)
#define HWIO_HS2_USB_PIPE3_CLK_CTRL_OFFS         (0x0000003c)
#define HWIO_HS2_USB30_IRQ0_DELAY_FROM_RISE_OFFS (0x00000040)
#define HWIO_HS2_USB30_IRQ1_DELAY_FROM_RISE_OFFS (0x00000044)
#define HWIO_HS2_USB30_IRQ_DELAY_CTRL_OFFS       (0x00000048)
#define HWIO_HS2_USB30_IRQ0_DELAY_FROM_FALL_OFFS (0x00000060)
#define HWIO_HS2_USB30_IRQ1_DELAY_FROM_FALL_OFFS (0x00000064)
#define HWIO_HS2_SETUP_VERSION_OFFS              (0x00000050)
#define HWIO_HS2_USB_UNIPHY_CLK_CTRL_OFFS        (0x00000054)
#define HWIO_HS2_ROM_STRING_SIZE_OFFS            (0x00010004)

#define UTMI_MMCM_SHIFT                          (0x000001A0)

#define USB_RUMI_PHY_INIT_RETRY_COUNT_MAX        (5)

  uint32        retry_count;
  uint32        utmi_khz;

  for (retry_count = 0; retry_count < USB_RUMI_PHY_INIT_RETRY_COUNT_MAX; retry_count++)
  {
    // Put everything in Reset except the PHY
    out_dword(RUMI_PHY_PRIM + HWIO_HS2_LOCAL_RESET_REG_OFFS, 0xFFF0);
    hsu_os_delay_us(100);
    out_dword(RUMI_PHY_PRIM + HWIO_HS2_LOCAL_RESET_REG_OFFS, 0xFFF3);
    hsu_os_delay_us(100); 
    out_dword(RUMI_PHY_PRIM + HWIO_HS2_LOCAL_RESET_REG_OFFS, 0xFFF0);

    // Shifting ULPI/UTMI clock MMCM
    out_dword(RUMI_PHY_PRIM + HWIO_HS2_USB_CLK_CTRL_OFFS, 0x00100000);
    hsu_os_delay_us(100);
    out_dword(RUMI_PHY_PRIM + HWIO_HS2_USB_CLK_CTRL_OFFS, 0x00000000);
    hsu_os_delay_us(100);
    out_dword(RUMI_PHY_PRIM + HWIO_HS2_USB_CLK_CTRL_OFFS, UTMI_MMCM_SHIFT);
    hsu_os_delay_us(100);

    // Shifting PIPE3 clock MMCM - Not used since only USB2 supported on RUMI
    out_dword(RUMI_PHY_PRIM + HWIO_HS2_USB_PIPE3_CLK_CTRL_OFFS, 0x00100000);
    hsu_os_delay_us(100);
    out_dword(RUMI_PHY_PRIM + HWIO_HS2_USB_PIPE3_CLK_CTRL_OFFS, 0x00000000);
    hsu_os_delay_us(100);
    out_dword(RUMI_PHY_PRIM + HWIO_HS2_USB_PIPE3_CLK_CTRL_OFFS, 0x00000004);
    hsu_os_delay_us(100);

    // Clear the other Resets */
    out_dword(RUMI_PHY_PRIM + HWIO_HS2_LOCAL_RESET_REG_OFFS, 0x0);
    hsu_os_delay_us(100);

    // Put clock selector to read UTMI clock */
    out_dword(RUMI_PHY_PRIM + HWIO_HS2_CLK_STATUS_SEL_OFFS, 0x00000009);
    hsu_os_delay_us(100);    
    utmi_khz = in_dword(RUMI_PHY_PRIM + HWIO_HS2_CLK_STATUS_OFFS);
    HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "utmi_clk_at_pad_khz, %u", utmi_khz);

    // Try until 60Mhz
    if (60 == (utmi_khz / 1000))
    {
      return;
    }
  }
  HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "rumi_phy_init_failed_after_attempt, %u", retry_count);
}


static void hsu_conf_sel_pre_sil_hw_init(void)
{
  hsu_platform_info_type const * platform_info = hsu_platform_get_info();

  /**API similar to VI code **/
  /** Reset the PHY clocks**/ 
  if (HSU_PLATFORM_RUMI != platform_info->hsu_platform_id)
  {
    //HWIO_GCC_USB3PHY_PHY_BCR_OUTM(HWIO_GCC_USB3PHY_PHY_BCR_BLK_ARES_BMSK, (0x1 << HWIO_GCC_USB3PHY_PHY_BCR_BLK_ARES_SHFT));
    HWIO_GCC_QUSB2_PHY_BCR_OUTM(HWIO_GCC_QUSB2_PHY_BCR_BLK_ARES_BMSK,(0x1 << HWIO_GCC_QUSB2_PHY_BCR_BLK_ARES_SHFT));
    hsu_os_delay_us(1000);
    HWIO_GCC_QUSB2_PHY_BCR_OUTM(HWIO_GCC_QUSB2_PHY_BCR_BLK_ARES_BMSK,(0x0 << HWIO_GCC_QUSB2_PHY_BCR_BLK_ARES_SHFT));
    hsu_os_delay_us(1000);
  }
  
  if (HSU_PLATFORM_RUMI == platform_info->hsu_platform_id)
  {
    hsu_conf_sel_usb_rumi_bu_init();
  }
  
  if (HSU_PLATFORM_RUMI != platform_info->hsu_platform_id)
  {
    /** Start the controller RESET **/ 
    HWIO_GCTL_OUTM(HWIO_GCTL_CORESOFTRESET_BMSK,(0x1 << HWIO_GCTL_CORESOFTRESET_SHFT));
    
    /** RESET THE DIGITAL INTERFACE OF PHY**/ 
    HWIO_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_OUTMI(0x0,HWIO_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_PHYSOFTRST_BMSK,
    									 ((uint32)0x1 << HWIO_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_PHYSOFTRST_SHFT));
    hsu_os_delay_us(100);
    HWIO_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_OUTMI(0x0,HWIO_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_PHYSOFTRST_BMSK,
    									 ((uint32)0x0 << HWIO_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_PHYSOFTRST_SHFT));
    hsu_os_delay_us(100);
    
    /** GUSB3PIPECTL **/
    HWIO_GUSB3PIPECTL_REGS_p_GUSB3PIPECTL_OUTMI(0, HWIO_GUSB3PIPECTL_REGS_p_GUSB3PIPECTL_DELAYP1TRANS_BMSK,
    							((uint32) 0x0 << HWIO_GUSB3PIPECTL_REGS_p_GUSB3PIPECTL_DELAYP1TRANS_SHFT));
    
    HWIO_GUSB3PIPECTL_REGS_p_GUSB3PIPECTL_OUTMI(0, HWIO_GUSB3PIPECTL_REGS_p_GUSB3PIPECTL_DISRXDETU3RXDET_BMSK,
    							((uint32) 0x0 << HWIO_GUSB3PIPECTL_REGS_p_GUSB3PIPECTL_DISRXDETU3RXDET_SHFT));
    
    /** END CONTROLLER RESET **/
    HWIO_GCTL_OUTM(HWIO_GCTL_CORESOFTRESET_BMSK,(0x0 << HWIO_GCTL_CORESOFTRESET_SHFT));
    hsu_os_delay_us(100);
  }
}

/*==============================================================================

FUNCTION          HSU_CONF_SEL_USB_HW_INIT

DESCRIPTION
Initialize the HS-USB hardware.

DEPENDENCIES
None, except for those noted above.

RETURN VALUE
None.

SIDE EFFECTS
None.

==============================================================================*/
static void hsu_conf_sel_usb_hw_init(void)
{

  /* Wrapper Function to initialize hsu clock manager and LPM for each active core */
  hsu_cores_hw_init();

  if (hsu_conf_sel_hs_main_core_enabled())
  {
#ifdef FEATURE_HS_USB_SNPS_CORE  
    if(hsu_conf_sel_is_usb_core_snps())
    {
      hsu_platform_info_type const * platform_info = hsu_platform_get_info();

      switch (platform_info->hsu_platform_id)
      {
        case HSU_PLATFORM_VIRTIO:
        case HSU_PLATFORM_RUMI:
        {
          hsu_conf_sel_pre_sil_hw_init();
        }
        break;

        default:
        break;
      }

      // Initialize SNPS core
      hsu_init_snps_usb();
    }
#endif
  }
  // else do nothing
}  /* hsu_conf_sel_usb_hw_init */


/*==============================================================================

FUNCTION          HSU_CONF_SEL_UPDATE_CORE_CONFIG

DESCRIPTION
Initialize core related information.

DEPENDENCIES
None, except for those noted above.

RETURN VALUE
None.

SIDE EFFECTS
None.

==============================================================================*/
static void hsu_conf_sel_update_core_config(void)
{
  /* Define all the structures needed for the stack init */
  struct hsu_device_params_type* core_conf_sel_info = NULL;
  const hsu_conf_sel_comp_info_type* cur_dev_comp_details = NULL;

  core_conf_sel_info =
    hsu_conf_sel_core_config_get_core_conf_sel_info(HSU_CORE_IDX_PRIMARY);

  if (core_conf_sel_info == NULL)
  {
    HSU_ERR_FATAL("_init(): core_conf_sel_info is NULL", 0, 0, 0);
  }

  cur_dev_comp_details = 
    core_conf_sel_info->configs_arr[HSU_CONFIG_IDX_0].cur_dev_comp_details;

  if (cur_dev_comp_details == NULL)
  {
    HSU_ERR_FATAL("_init(): cur_dev_comp_details is NULL", 0, 0, 0);
  }

  memscpy(core_conf_sel_info->configs_arr[HSU_CONFIG_IDX_0].num_of_fd_instances, 
    sizeof(core_conf_sel_info->configs_arr[HSU_CONFIG_IDX_0].num_of_fd_instances),
    &(cur_dev_comp_details->num_of_fd_instances_in_comp),
    sizeof(core_conf_sel_info->configs_arr[HSU_CONFIG_IDX_0].num_of_fd_instances));

  core_conf_sel_info->product_id = cur_dev_comp_details->product_id;

}  /* hsu_conf_sel_update_core_config */

#if defined(FEATURE_HS_USB_BRINGUP_POLL_INTR)
/*==============================================================================

FUNCTION          HSU_CONF_SEL_BRINGUP_POLL

DESCRIPTION
Polls USB interrupt and handle it.

DEPENDENCIES
This function should not be called anywhere except 
hsu_conf_sel_bringup_poll_enqueue()

RETURN VALUE
None.

SIDE EFFECTS
None.
==============================================================================*/
static void hsu_conf_sel_bringup_poll(void *param)
{
  /* Poll the HW for interrupts, and handle them if necessary */
  if (hsu_os_bus_poll_interrupt())
  {
      hsu_os_bus_handle_interrupt();
  }
 
  (void)rex_set_timer(
    &hsu_conf_sel_bringup_poll_tmr, 
    HSU_CONF_SEL_BRINGUP_POLL_INTR_INTERVAL_MS);
}

/*==============================================================================

FUNCTION          HSU_CONF_SEL_BRINGUP_POLL_ENQUEUE

DESCRIPTION
Enqueue USB polling/handling operation in AL TASK

DEPENDENCIES
This function should not be called anywhere except 
hsu_conf_sel_bringup_poll_init()
AL Task should already be initialized.

RETURN VALUE
None.

SIDE EFFECTS
None.
==============================================================================*/
static void hsu_conf_sel_bringup_poll_enqueue(unsigned long param)
{
  hsu_al_task_enqueue_cmd(hsu_conf_sel_bringup_poll, (void *)param);
}

/*==============================================================================

FUNCTION          HSU_CONF_SEL_BRINGUP_POLL_ENQUEUE

DESCRIPTION
Creates and starts a timer for periodical polling of USB interrupt.
The purpose of this API is to rely on timer based USB interrupt 
handling rather than tramp.  In many cases, USB tramp is not working during
bring-up.  This API can be used to test USB without tramp dependency.

DEPENDENCIES
AL Task should already be initialized.

RETURN VALUE
None.

SIDE EFFECTS
None.
==============================================================================*/
static void hsu_conf_sel_bringup_poll_init(void)
{
  rex_def_timer_ex(
    &hsu_conf_sel_bringup_poll_tmr, 
    hsu_conf_sel_bringup_poll_enqueue, 0);

  (void)rex_set_timer(
    &hsu_conf_sel_bringup_poll_tmr, 
    HSU_CONF_SEL_BRINGUP_POLL_INTR_INTERVAL_MS);
}
#endif /* FEATURE_HS_USB_BRINGUP_POLL_INTR */


/*==============================================================================

FUNCTION          HSU_ERROR_FATAL

DESCRIPTION
HSUSB Error Fatal Handler 

DEPENDENCIES
None

RETURN VALUE
None.

SIDE EFFECTS
None.
==============================================================================*/
static void hsu_err_fatal(void)
{
  HSU_ULOG(NO_MSG, BUS_LOG, "ERROR_FATAL");
#if defined(FEATURE_HS_USB_DUMP_USB_INFO_ON_ERR_FATAL) && !defined(FEATURE_HS_USB_MEM_OPTIMIZATION)
   /* adding HSUSB core dump to HSU_ERR_FATAL */
   hsu_os_bus_save_mem_dump();
#endif /* FEATURE_HS_USB_DUMP_USB_INFO_ON_ERR_FATAL && !FEATURE_HS_USB_MEM_OPTIMIZATION */

}

/*==============================================================================

FUNCTION          HSU_CONFIG_SELECTOR_INIT

DESCRIPTION
Called from the Application's processor TMC, after DSM pools are initialized,
and before the creation of tasks that use USB, such as DIAG, Modem, RmNET, 
etc...
The function initializes the High Speed USB stack according to the default
composition.

DEPENDENCIES
None, except for those noted above.

RETURN VALUE
None.

SIDE EFFECTS
Various tasks related to the HS-USB stack are created.

==============================================================================*/
void hsu_config_selector_init(void) 
{
  boolean  err_crash_register_status;

#ifndef USB_THREADX
#ifndef FEATURE_HS_USB_PRE_SILICON_BU
  tracer_event_simple(HSU_TASK_INIT);
#endif // FEATURE_HS_USB_PRE_SILICON_BU
#endif

  hsu_platform_init();

  /* Initialize HSUSB EFS items configuration file */
  hsu_conf_sel_nv_efs_init_config_file();
  hsu_conf_sel_nv_efs_update_all_items();

#ifdef FEATURE_HS_USB_SNPS_CORE
  if(hsu_conf_sel_is_usb_core_snps()) 
  {
     hsu_core_config_usb_idx = HSU_CORE_CONFIG_USB3_IDX;
  }
#endif /* FEATURE_HS_USB_SNPS_CORE */

  // Check and update device composition
  check_update_device_composition();
  hsu_log_init(); /* has dependency on EFS items settings */
  
  //HAL_hsusb_Init();
  hsu_conf_sel_core_config_set_core_info();

  hsu_conf_sel_stack_utils_init_memory();

#ifdef FEATURE_HS_USB_HEALTH_MONITOR
  hsu_diag_qxdm_init();
#endif /* FEATURE_HS_USB_HEALTH_MONITOR */

  hsu_conf_sel_init_adapt_layers();


#ifdef FEATURE_HS_USB_ECM_INT_EP_SHARING    
  update_is_ecm_int_ep_sharing_supported();
#endif /* FEATURE_HS_USB_ECM_INT_EP_SHARING */

#ifdef FEATURE_HSU_ECM_HOST_APP_STUB
  hsu_ecm_host_app_stub_mem_allocation(); 
#endif /* FEATURE_HSU_ECM_HOST_APP_STUB */

  /* Init OS detection parameters */
#if defined (FEATURE_HS_USB_OS_DETECTION)
  hsu_os_detection_init_params();
#endif /* FEATURE_HS_USB_OS_DETECTION */


  HSU_MSG_HIGH("*_init(): setting up the stack");
  /************************/
  /* Initialize the stack */
  /************************/

  hsu_conf_sel_usb_hw_init();
  hsu_conf_sel_update_core_config();

#if defined(FEATURE_HS_USB_BAM)
  hsu_al_bam_init();
#endif /* FEATURE_HS_USB_BAM */

#ifdef FEATURE_HS_USB_BAM_RESET
  /* marker: initialize bam state machine */
  hsu_bam_sm_init();

  #ifdef FEATURE_HS_USB_BAM_RESET_TEST
  /* initialize sps reset test stub */
   hsu_bam_reset_test_stub_init();
  #endif /* FEATURE_HS_USB_BAM_RESET_TEST */

#endif /* FEATURE_HS_USB_BAM_RESET */

  /* all hsu_phy_utils_start_hsic() logic is deferred until GPIO handshake or MDM PWR CTRL = 1 */

  if (hsu_conf_sel_stack_utils_setup_stack
  (
#ifdef T_UNDP
    TRUE
#endif
  ) != HSU_JSUCCESS)
  {
    HSU_ERR_FATAL("*_init(): hsu_conf_sel_stack_utils_setup_stack failed", 0, 0, 0);
  }

  /* 
  * ECM application stub should be initialized when FEATURE_HSU_TEST is used, 
  * and also when FEATURE_HSU_ECM_APP_STUB is used, if the network function is 
  * part of the used composition. 
  */
#ifdef FEATURE_HSU_ECM_APP_STUB
  if (is_ecm_app_stub_active)
  {
    init_ecm_app_stub_if_needed();
  }
#endif

#if defined(FEATURE_HSU_EEM_APP_STUB)
  init_eem_app_stub();
#endif

#ifdef FEATURE_HS_USB_OBEX_PERF_TESTS
  if (!hsu_obex_perf_test_app_init()) 
  {
    HSU_MSG_ERROR("*_init(): hsu_ser_perf_test_app_init() failed");
  }
#endif /* FEATURE_HS_USB_OBEX_PERF_TESTS */

#ifdef FEATURE_HS_USB_ACM_PERF_TESTS
  if (!hsu_acm_test_app_init()) 
  {
    HSU_MSG_ERROR("*_init(): hsu_acm_test_app_init() failed");
  }
#endif /* FEATURE_HS_USB_ACM_PERF_TESTS */

/* register customer defined class driver */
  register_cust_class_driver();

#ifdef FEATURE_HSU_ECM_HOST_APP_STUB
  hsu_ecm_host_app_stub_init_task();
#endif /* FEATURE_HSU_ECM_HOST_APP_STUB */

#if defined(FEATURE_HS_USB_BRINGUP_POLL_INTR) 
  /* During bring-up, the tramp for USB may not work. The feature will poll 
   * and handle USB interrupt with timer to remove dependency on tramp. */
  hsu_conf_sel_bringup_poll_init();
#endif /* FEATURE_HS_USB_BRINGUP_POLL_INTR */

#ifndef FEATURE_HS_USB_MEM_TEST
  if (TRUE == hsu_conf_sel_hs_main_core_enabled())
  {
    /* Register for USB VBUS detection only after stack initialization */
    hsu_vregmgr_pmic_vbus_register();
  }
#endif // FEATURE_HS_USB_MEM_TEST
  
  hsu_is_initialized = TRUE;


  /* To register callback during error */
  err_crash_register_status = err_crash_cb_register(hsu_err_fatal);
  if(!err_crash_register_status)
    {
      HSU_ASSERT(
        "err_crash_cb_register(): Returned False " == NULL);
    } 

#ifdef FEATURE_HS_USB_MEM_TEST
  hsu_al_task_enqueue_cmd((hsu_al_task_cmd_func_type)jos_mem_test, (void*)NULL);
#endif // FEATURE_HS_USB_MEM_TEST
  hsu_event_notif_raise(HSU_EVENT_INIT_COMPLETED);

} /* hsu_config_selector_init */

/*==============================================================================

FUNCTION          HSU_CONF_SEL_GET_CUR_COMP_DETAILS

DESCRIPTION
This function returns a pointer to the information relevant for the current 
composition, for the primary core.

DEPENDENCIES
None.

RETURN VALUE
Pointer to hsu_conf_sel_comp_info_type for success, 
Currently all failures cause ASSERT.

SIDE EFFECTS
None.

==============================================================================*/
const hsu_conf_sel_comp_info_type *  hsu_conf_sel_get_cur_comp_details(void)
{
  struct hsu_device_params_type* primary_core_conf_sel_info ;

  primary_core_conf_sel_info = 
    hsu_conf_sel_core_config_get_core_conf_sel_info(HSU_CORE_IDX_PRIMARY);

  return primary_core_conf_sel_info->configs_arr[HSU_CONFIG_IDX_0].cur_dev_comp_details;
} /* hsu_conf_sel_get_cur_comp_details */


/*==============================================================================

FUNCTION          HSU_CONF_SEL_GET_CUR_COMP_ID

DESCRIPTION
This function returns the hsu_conf_sel_composition_id_enum of the current 
composition, for the primary core

DEPENDENCIES
None.

RETURN VALUE
hsu_conf_sel_composition_id_enum 

SIDE EFFECTS
None.

==============================================================================*/
hsu_conf_sel_composition_id_enum hsu_conf_sel_get_cur_comp_id(void)
{
  const hsu_conf_sel_comp_info_type* cur_dev_comp_details = NULL;


  cur_dev_comp_details = hsu_conf_sel_get_cur_comp_details();

  if (cur_dev_comp_details == NULL)
  {
    HSU_ERR_FATAL("_get_cur_comp_id(): cur_dev_comp_details is NULL", 0, 0, 0);
  }

  return cur_dev_comp_details->hsu_comp_id;
} /* hsu_conf_sel_get_cur_comp_id */

#ifndef T_UNDP /* UNDP does not support UI/Pictbridge */
/*==============================================================================

FUNCTION          HSU_CONF_SEL_SWITCH_DEV_STACK

DESCRIPTION
This function initializes and starts the device stack according to the new 
composition given as a parameter.

DEPENDENCIES
Assumes the stack is up. 

RETURN VALUE
TRUE for success, currently all failures cause an ASSERT.

SIDE EFFECTS
cur_dev_comp_details is updated with the new composition.

==============================================================================*/
boolean hsu_conf_sel_switch_dev_stack
(
 hsu_conf_sel_composition_id_enum new_hsu_comp_id,
 reconfig_complete_cb_t reconfig_cb_func_ptr,
 void *      arg_for_reconfig_cb
)
{
  struct hsu_device_params_type* primary_core_conf_sel_info = NULL;
  const hsu_conf_sel_comp_info_type * new_dev_comp = 
    hsu_conf_sel_comp_utils_get_comp_details_by_hsu_id(new_hsu_comp_id);
  HSU_ASSERT(new_dev_comp != NULL);


  /* Update hsu_core_config_table with the new composition details */
  /***************************************/
  /* Set the primary core data for configuration 0 since it depends on the chosen composition */
  primary_core_conf_sel_info = 
    hsu_conf_sel_core_config_get_core_conf_sel_info(HSU_CORE_IDX_PRIMARY);

  primary_core_conf_sel_info->configs_arr[HSU_CONFIG_IDX_0].cur_dev_comp_details = new_dev_comp;
  primary_core_conf_sel_info->product_id = new_dev_comp->product_id;
  memscpy (primary_core_conf_sel_info->configs_arr[HSU_CONFIG_IDX_0].num_of_fd_instances,
    sizeof(primary_core_conf_sel_info->configs_arr[HSU_CONFIG_IDX_0].num_of_fd_instances),
    new_dev_comp->num_of_fd_instances_in_comp,
    sizeof(primary_core_conf_sel_info->configs_arr[HSU_CONFIG_IDX_0].num_of_fd_instances));

#ifdef FEATURE_HS_USB_BREWMP_COMPOSITION
  if (new_hsu_comp_id == HSU_CONF_SEL_BREWMP_COMP)
  {
    if (!brewmp_device_data.is_initialized)
    {
      HSU_MSG_ERROR(
        "hsu_conf_sel_switch_dev_stack(): Error - trying to switch to"
        "BrewMP composition before it has been initialized.");

      return FALSE;
    }
  }
#endif  /* FEATURE_HS_USB_BREWMP_COMPOSITION */

  /* Start the stack */
  /*******************/
  HSU_MSG_LOW(
    "*_switch_dev_stack(): calling hsu_conf_sel_stack_utils_switch_device_stack");
  if(hsu_conf_sel_stack_utils_switch_device_stack(
    HSU_CORE_IDX_PRIMARY,
    reconfig_cb_func_ptr,
    arg_for_reconfig_cb) != HSU_JSUCCESS)
  {
    HSU_ERR_FATAL("*_switch_dev_stack(): "
      "hsu_conf_sel_stack_utils_switch_device_stack failed", 0, 0, 0);
  }
  return TRUE;
} /* hsu_conf_sel_switch_dev_stack */
#endif /* !T_UNDP */


/*===========================================================================

FUNCTION      HSU_GET_NUM_OF_NET_INSTANCES

DESCRIPTION
This function returns the number of the network (ECM) FD instances included in 
the current composition, for the primary core.

DEPENDENCIES
FEATURE_HS_USB must be defined. 
Must be called after hsu_config_selector_init() was called.

RETURN VALUE
The number of the network (ECM) FD instances included in the current composition. 

SIDE EFFECTS
None

===========================================================================*/
uint8 hsu_get_num_of_net_instances(void)
{
  const hsu_conf_sel_comp_info_type* cur_dev_comp_details = NULL;
  uint8 num_of_net_instances = 0;

  cur_dev_comp_details = hsu_conf_sel_get_cur_comp_details();

  if (cur_dev_comp_details == NULL)
  {
    HSU_ERR_FATAL("hsu_get_num_of_net_instances(): "
      "cur_dev_comp_details is NULL", 0, 0, 0);
  }    

#if (defined FEATURE_HS_USB_ECM_PERFORMANCE_TESTS) || (defined FEATURE_HSU_ECM_APP_STUB)
  /* Disable ECM application in order to run ECM performance test or 
  ECM application stub. The ecm_stub_app will run instead of the 
  real application   */
  if ((cur_dev_comp_details->num_of_fd_instances_in_comp[HSU_CFG_SEL_ECM_FD] > 1) &&
    (is_ecm_app_stub_active))
  {
    return 1;
  }
  else if (is_ecm_app_stub_active)
  {
    return 0;
  }
#endif /* FEATURE_HS_USB_ECM_PERFORMANCE_TESTS || FEATURE_HSU_ECM_APP_STUB */

  num_of_net_instances = cur_dev_comp_details->num_of_fd_instances_in_comp[HSU_CFG_SEL_ECM_FD];

  /*This API is called by SMD bridge to know at boot time to know how many times to 
    call SIO open for ECM .
    If we have a IPC Router or LWIP port then we want to reduce num_of_net_instances by one as
    SIO_Open open for IPCR/LWIP will not be called from SMD driver */
    
  if(HSU_IFACE_ABSENT_IN_COMP != hsu_conf_sel_get_iface_index_comp(IPCR_HSU_IFACE))
  {
    num_of_net_instances--;
  }

  
  if(HSU_IFACE_ABSENT_IN_COMP != hsu_conf_sel_get_iface_index_comp(LWIP_HSU_IFACE))
  {
    num_of_net_instances--;
  }

  // DPL is treated like a real ECM, do not subtract a net instance count
  
  return num_of_net_instances;
} /* hsu_get_num_of_net_instances */

/*==============================================================================

FUNCTION          HSU_CONF_SEL_HS_MAIN_CORE_ENABLED

DESCRIPTION
This function returns whether HS-USB OTG core is active (as primary or secodary 
core).

DEPENDENCIES
None.

RETURN VALUE
TRUE - HS-USB OTG core is active 
FALSE - HS-USB OTG core isn't active

SIDE EFFECTS
None.

==============================================================================*/
boolean hsu_conf_sel_hs_main_core_enabled (void)
{
  return ((HSU_CORE_IDX_PRIMARY == HSU_CORE_CONFIG_USB_IDX) || 
          (HSU_CORE_IDX_SECONDARY == HSU_CORE_CONFIG_USB_IDX));
} /* hsu_conf_sel_hs_main_core_enabled */

#ifdef FEATURE_HS_USB_ECM
/*==============================================================================

FUNCTION          HSU_CONFIG_SELECTOR_IS_STANDARD_ECM_SUPPORTED

DESCRIPTION
This function returns TRUE if the current composition is one that supports 
standard ECM mode.

DEPENDENCIES
None.

RETURN VALUE
TRUE if current composition supports Standard ECM.

SIDE EFFECTS
None.

==============================================================================*/
boolean hsu_config_selector_is_standard_ecm_supported(void)
{
#if !(defined FEATURE_HS_USB_STANDARD_ECM)
  return FALSE;
#else /*FEATURE_HS_USB_STANDARD_ECM*/
  const hsu_conf_sel_comp_info_type* cur_dev_comp_details = NULL;

  cur_dev_comp_details = hsu_conf_sel_get_cur_comp_details();

  if (cur_dev_comp_details == NULL)
  {
    HSU_ERR_FATAL("hsu_config_selector_is_standard_ecm_supported(): "
      "cur_dev_comp_details is NULL", 0, 0, 0);
  }   
  if (cur_dev_comp_details->hsu_comp_id == HSU_CONF_SEL_DIAG_STD_NET_COMP) 
  {
    return TRUE;
  }
#ifdef FEATURE_HS_USB_MS_FD
  if (cur_dev_comp_details->hsu_comp_id == HSU_CONF_SEL_DIAG_NMEA_MS_STD_MDM_STD_NET_COMP)
  {
    return TRUE;
  }
#endif /*FEATURE_HS_USB_MS_FD*/

  return FALSE;
#endif /*FEATURE_HS_USB_STANDARD_ECM*/
} /*hsu_config_selector_is_standard_ecm_supported*/
#endif /*FEATURE_HS_USB_ECM*/

/*==============================================================================

FUNCTION          HSU_CONFIG_SELECTOR_IS_STANDARD_ACM_SUPPORTED

DESCRIPTION
This function returns TRUE if the current composition is one that supports 
standard ACM mode.

DEPENDENCIES
None.

RETURN VALUE
TRUE if current composition supports Standard ACM.

SIDE EFFECTS
None.

==============================================================================*/
boolean hsu_config_selector_is_standard_acm_supported(void)
{
#if !(defined FEATURE_HS_USB_STANDARD_ECM) || !(defined FEATURE_HS_USB_MS_FD)
  return FALSE;
#else /*FEATURE_HS_USB_STANDARD_ECM & FEATURE_HS_USB_MS_FD*/
  const hsu_conf_sel_comp_info_type* cur_dev_comp_details = NULL;

  cur_dev_comp_details = hsu_conf_sel_get_cur_comp_details();

  if (cur_dev_comp_details == NULL)
  {
    HSU_ERR_FATAL("hsu_config_selector_is_standard_acm_supported(): "
      "cur_dev_comp_details is NULL", 0, 0, 0);
  }   
  return (cur_dev_comp_details->hsu_comp_id == HSU_CONF_SEL_DIAG_NMEA_MS_STD_MDM_STD_NET_COMP 
    ? TRUE : FALSE);
#endif /*FEATURE_HS_USB_STANDARD_ECM || FEATURE_HS_USB_MS_FD*/
}/*hsu_config_selector_is_standard_acm_supported*/

#ifdef FEATURE_HS_USB_MBIM
/*==============================================================================

FUNCTION          hsu_conf_sel_is_instance_mbim

DESCRIPTION
This function returns TRUE if the current composition is one that supports 
MBIM ECM mode. MBIM does not have multiple instances so this function 
returns TRUE only the first time,second time or more this will return FALSE. 

DEPENDENCIES
MBIM does not support miltiple instance so tthis function returns TRUE only 
on first call. All subsequent calls it will returns FALSE. The is usefull for 
MBIM + RMNET type compositions or MBIM + GNSS. 

The first ecm instance will always be MBIM interface. 

RETURN VALUE
TRUE if current composition supports MBIM ECM.

SIDE EFFECTS
None.

==============================================================================*/
boolean hsu_conf_sel_is_instance_mbim(uint8 instance_ecm)
{
  uint8 instance_mbim = hsu_conf_sel_get_iface_index_comp(MBIM_HSU_IFACE);
  return (instance_ecm == instance_mbim) ? TRUE : FALSE;
}

/*==============================================================================

FUNCTION          HSU_CONFIG_SELECTOR_IS_MBIM_ONLY_SUPPORTED

DESCRIPTION
This function returns TRUE if the current composition is one that supports 
MBIM only mode.

DEPENDENCIES
None.

RETURN VALUE
TRUE if current composition supports MBIM only mode.

SIDE EFFECTS
None.

==============================================================================*/
boolean hsu_conf_sel_is_diag_over_mbim(void)
{
  uint8 result = hsu_conf_sel_get_iface_index_comp(DIAG_HSU_IFACE);

  if(result == HSU_IFACE_ABSENT_IN_COMP)
  {
    result = hsu_conf_sel_get_iface_index_comp(MBIM_HSU_IFACE);
    if(result !=HSU_IFACE_ABSENT_IN_COMP)
      return TRUE;
  }

  return FALSE;
}
#endif /* FEATURE_HS_USB_MBIM */


/*==============================================================================

FUNCTION          HSU_CONFIG_SELECTOR_IS_QDSS_BAM_SUPPORTED

DESCRIPTION
This function returns TRUE if the current composition is one that supports 
QDSS in BAM mode.

DEPENDENCIES
None.

RETURN VALUE
TRUE if current composition supports QDSS in BAM mode.

SIDE EFFECTS
None.

==============================================================================*/
boolean hsu_config_selector_is_qdss_bam_supported(void)
{
#ifdef FEATURE_HS_USB_QDSS_PORT
  uint8 result = hsu_conf_sel_get_iface_index_comp(QDSS_HSU_IFACE);

  if(result != HSU_IFACE_ABSENT_IN_COMP)
  {
    /* HSUSB is supporting QDSS BAM-to-BAM mode */
    if (HSU_CORE_IDX_PRIMARY == HSU_CORE_CONFIG_USB_IDX)
      return TRUE;
  }
 
#endif /* FEATURE_HS_USB_QDSS_PORT */

  return FALSE;
}

/*==============================================================================

FUNCTION          HSU_CONF_SEL_IS_HSUSB_INITIALIZED

DESCRIPTION
This function returns whether hsu_config_selector_init was completed.

DEPENDENCIES
None.

RETURN VALUE
TRUE/FALSE

SIDE EFFECTS
None.

==============================================================================*/
boolean hsu_conf_sel_is_hsusb_initialized(void)
{
  return hsu_is_initialized;
}


#ifdef FEATURE_HS_USB_BREWMP_COMPOSITION
/*==============================================================================

FUNCTION          HSU_CONF_SEL_UPDATE_BMP_COMPOSITION

DESCRIPTION
This function is called when the BrewMP composition is active.
This function defines the details of the BrewMP device composition and starts
an enumeration as a BrewMP device.

DEPENDENCIES
None.

RETURN VALUE
True if the composition is updated successfully. Otherwise, False is
returned.

SIDE EFFECTS
None.

==============================================================================*/
boolean hsu_conf_sel_update_bmp_composition(
  dev_info_t*          device_info,
    /* Device related information */

  config_param_t*      configuration_info,
    /* Holds the composition details. */

  uint8                num_of_rdm_dev_in_comp,                  
    /* Number of RDM devices used */

  rdm_device_enum_type rdm_devices[HSU_NUM_OF_RDM_USB_PORTS]    
    /* RDM devices used */
)
{
  uint32 i;
  uint32 hsu_conf_sel_fd_type;
  uint8  num_of_fd_instances_per_fd[HSU_CFG_SEL_NUM_OF_FD_TYPES] = {0};

  
  /* Store the parameters in a static area. */
  brewmp_device_data.device_info            = *device_info;
  brewmp_device_data.configuration_info     = *configuration_info;
  brewmp_device_data.num_of_rdm_dev_in_comp = num_of_rdm_dev_in_comp;

  for (i = 0; i < HSU_NUM_OF_RDM_USB_PORTS; i++)
  {
    brewmp_device_data.rdm_devices[i] = rdm_devices[i];
  }

  for (i = 0;
       i < configuration_info->num_of_fd_instances;
       i++)
  {
    switch ((configuration_info->fd_instances[i]).type)
    {
    case DEVICE_TYPE_MSD:
      hsu_conf_sel_fd_type = HSU_CFG_SEL_MS_FD;
      break;
      
    case DEVICE_TYPE_HID:
      hsu_conf_sel_fd_type = HSU_CFG_SEL_HID_FD;
      break;

    case DEVICE_TYPE_LOOPBACK:
      hsu_conf_sel_fd_type = HSU_CFG_SEL_LOOPBACK_FD;
      break;

    case DEVICE_TYPE_CDC_ACM:
      hsu_conf_sel_fd_type = HSU_CFG_SEL_ACM_FD;
      break;

    case DEVICE_TYPE_CDC_ECM:
      hsu_conf_sel_fd_type = HSU_CFG_SEL_ECM_FD;
      break;

    case DEVICE_TYPE_CDC_OBEX:
      hsu_conf_sel_fd_type = HSU_CFG_SEL_OBEX_FD;
      break;

    case DEVICE_TYPE_SICD:
      hsu_conf_sel_fd_type = HSU_CFG_SEL_SICD_FD;
      break;

    case DEVICE_TYPE_AUDIO:
      hsu_conf_sel_fd_type = HSU_CFG_SEL_AUDIO_FD;
      break;

    case DEVICE_TYPE_VENDOR:
      hsu_conf_sel_fd_type = 
        (configuration_info->fd_instances[i]).vendor_specific_subtype;
      break;

    default:
      return FALSE;
    }

    num_of_fd_instances_per_fd[hsu_conf_sel_fd_type] = 
      (configuration_info->fd_instances[i]).count;
  }

  /* Update the BrewMP composition entry in the HS-USB composition table. */
  hsu_conf_sel_comp_utils_set_comp_info(
    HSU_CONF_SEL_BREWMP_COMP,
    device_info->product,
    num_of_fd_instances_per_fd,
    num_of_rdm_dev_in_comp,
    rdm_devices
  );

  brewmp_device_data.is_initialized = TRUE;

  if (hsu_conf_sel_switch_dev_stack(HSU_CONF_SEL_BREWMP_COMP, NULL, NULL) == FALSE)
  {
    HSU_ERR_FATAL(
      "hsu_conf_sel_comp_utils_update_composition: "
      "hsu_conf_sel_switch_dev_stack failed", 0, 0, 0);
  }

  return TRUE;
}  /* hsu_conf_sel_update_bmp_composition */
#endif  /* FEATURE_HS_USB_BREWMP_COMPOSITION */


#ifdef FEATURE_HS_USB_BREWMP_COMPOSITION
/*==============================================================================

FUNCTION          HSU_CONF_SEL_GET_BMP_DEVICE_INFO

DESCRIPTION
This function is retrieves the BrewMP device information as provided by the 
BrewMP layer upon the hsu_config_selector_update_composition() call.

DEPENDENCIES
None.

RETURN VALUE
A pointer to a brewmp_device_data_struct.

SIDE EFFECTS
None.

==============================================================================*/
hsu_conf_sel_brewmp_device_data_type* hsu_conf_sel_get_bmp_device_info(void)
{
  return &brewmp_device_data;
}  /* hsu_conf_sel_get_bmp_device_info */
#endif  /* FEATURE_HS_USB_BREWMP_COMPOSITION */

/*===========================================================================

FUNCTION      HSU_GET_NUM_OF_ACM_INSTANCES

Returns the number of ACM FD instances included in
the current composition for the primary core.

@dependencies
Must be called after hsu_config_selector_init().

@sideeffects
None.

@return
The number of ACM FD instances included in the current
composition.

===========================================================================*/
uint8 hsu_get_num_of_acm_instances(void)
{
  const hsu_conf_sel_comp_info_type* cur_dev_comp_details = NULL;

  cur_dev_comp_details = hsu_conf_sel_get_cur_comp_details();

  if (cur_dev_comp_details == NULL)
  {
    HSU_ERR_FATAL("hsu_get_num_of_net_instances(): "
      "cur_dev_comp_details is NULL", 0, 0, 0);
  }    
  return cur_dev_comp_details->num_of_fd_instances_in_comp[HSU_CFG_SEL_ACM_FD];
} /* hsu_get_num_of_net_instances */


#ifdef FEATURE_HS_USB_ECM_INT_EP_SHARING    
/*===========================================================================
FUNCTION      update_is_ecm_int_ep_sharing_supported
===========================================================================*/
static void update_is_ecm_int_ep_sharing_supported(void)
{
  const hsu_conf_sel_comp_info_type* cur_dev_comp_details = NULL;

  cur_dev_comp_details = hsu_conf_sel_get_cur_comp_details();

  if (cur_dev_comp_details == NULL)
  {
    HSU_ERR_FATAL("hsu_config_selector_is_ecm_int_ep_sharing_supported(): "
      "comp is NULL", 0, 0, 0);
  }   

  switch (cur_dev_comp_details->hsu_comp_id)
  {
    case HSU_CONF_SEL_DIAG_MDM_NET8_COMP:
      ecm_is_ep_sharing_supported = TRUE;
      break;
    default:
      ecm_is_ep_sharing_supported = FALSE; 
  }
}
/*==============================================================================

FUNCTION          HSU_CONFIG_SELECTOR_IS_ECM_INT_EP_SHARING_SUPPORTED

DESCRIPTION
This function returns TRUE if the current composition is one that supports 
ECM interrupt Endpoint mode.

DEPENDENCIES
None.

RETURN VALUE
TRUE if the current composition is one that supports ECM interrupt Endpoint mode.

SIDE EFFECTS
Only the first ECM instance will have a Interrupt pipe rest of the ECM instance 
will only have a BULk In adn BULK OUT and will share the Interrupt pipe from 
first intstance
==============================================================================*/
boolean hsu_config_selector_is_ecm_int_ep_sharing_supported(void)
{
  return ecm_is_ep_sharing_supported;
}

#endif /* FEATURE_HS_USB_ECM_INT_EP_SHARING */

/*==============================================================================

FUNCTION          HSU_CONF_SEL_IS_EP0_AGGRESSIVE_ERR_CHK_ENABLED

DESCRIPTION
This function returns TRUE when HSU_EFS_AGGRESSIVE_EP0_ERROR_CAPTURING_ITEM_FILE
bit field 0 is set.  This may increase the frequency of assertion errors and
should only be used to debug specific EP0 related issues.

DEPENDENCIES
None.

RETURN VALUE
TRUE if the aggressive error checking is enabled.  FALSE when aggressive error
checking is not enabled.

SIDE EFFECTS
Increased frequency of assertion errors may be seen.  This feature
should only be enabled to debug specific EP0 related issues
==============================================================================*/
boolean hsu_conf_sel_is_ep0_aggressive_err_chk_enabled(void)
{
  if (HSU_EP0_AGGRESSIVE_ERR_CHK_MASK & 
      hsu_conf_sel_aggressive_ep0_error_capturing_enabled())
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*==============================================================================

FUNCTION          HSU_CONF_SEL_IS_EP0_TIMER_ON_RX_TX_ZLP_ERR_CHK_ENABLED

DESCRIPTION
This function returns TRUE when HSU_EFS_AGGRESSIVE_EP0_ERROR_CAPTURING_ITEM_FILE
bit field 1 is set.  This may increase the frequency of assertion errors and
should only be used to debug specific EP0 related issues.

DEPENDENCIES
None.

RETURN VALUE
TRUE if the timer based aggressive error checking is enabled.  FALSE when aggressive error
checking is not enabled.

SIDE EFFECTS
Increased frequency of assertion errors may be seen.  This feature
should only be enabled to debug specific EP0 related issues
==============================================================================*/
boolean hsu_conf_sel_is_ep0_timer_on_rx_tx_zlp_err_chk_enabled(void)
{
  if (HSU_EP0_TIMER_ON_RX_TX_ZLP_ERR_CHK_MASK & 
      hsu_conf_sel_aggressive_ep0_error_capturing_enabled())
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*===========================================================================
FUNCTION          HSU_CONF_SEL_IS_DTD_MEM_LOGGING_ENABLED

DESCRIPTION
This function returns TRUE when HSU_EFS_AGGRESSIVE_EP0_ERROR_CAPTURING_ITEM_FILE
bit field 2 is set.  Enable this to debug TD active bit related issues.

DEPENDENCIES
None.

RETURN VALUE
TRUE means DTD and MEM logging will be enabled.  FALSE otherwise.

SIDE EFFECTS
Transfer speed may be reduced as logging the TD structure requires CPU time
==============================================================================*/
boolean hsu_conf_sel_is_dtd_mem_logging_enabled(void)
{
  if (HSU_DTD_MEM_LOGGING_ENABLE_MASK &
      hsu_conf_sel_aggressive_ep0_error_capturing_enabled())
  {
    return FALSE;
  }
  else
  {
    /* for now, make default EFS item inactive to enable logging */
    return TRUE;
  }
}


#ifdef FEATURE_HS_USB_INTR_ONLY_ECM_PIPE
/*==============================================================================

FUNCTION          hsu_conf_sel_is_curr_instance_intr_in_only_ecm

DESCRIPTION
This function returns TRUE if the current composition is one that supports 
INTR IN only ECM mode i.e. RMNET instance - BUNL IN & BULK OUT

DEPENDENCIES
This does not support miltiple instanceand the first instance will always be MBIM/RMNET
so this function returns TRUE only on second call. All other calls it will returns FALSE. 

This will always be the second ECM instance

RETURN VALUE
TRUE if current composition supports INTR In ONLY ECM interface.

SIDE EFFECTS
This does not support miltiple instanceand the first instance will always be MBIM/RMNET
so this function returns TRUE only on second call. All other calls it will returns FALSE. 

This will always be the second ECM instance

==============================================================================*/
boolean hsu_conf_sel_is_curr_instance_intr_in_only_ecm(uint8 instance_ecm)
{
  if(instance_ecm == hsu_conf_sel_get_iface_index_comp(GNSS_HSU_IFACE))
    return TRUE;
  else if(instance_ecm == hsu_conf_sel_get_iface_index_comp(IPCR_HSU_IFACE))
    return TRUE;
  else
    return FALSE;
}
#endif /* FEATURE_HS_USB_INTR_ONLY_ECM_PIPE */

/*==============================================================================

FUNCTION          hsu_conf_sel_is_curr_instance_dpl

DESCRIPTION
This function returns TRUE if the current composition is one that supports 
DPL which only has a BULK IN interface

DEPENDENCIES


RETURN VALUE
TRUE if current composition supports BULK IN only.

SIDE EFFECTS

==============================================================================*/
boolean hsu_conf_sel_is_curr_instance_dpl(uint8 instance_ecm)
{
  if(instance_ecm == hsu_conf_sel_get_iface_index_comp(DPL_HSU_IFACE))
    return TRUE;
  else
    return FALSE;
}

/*==============================================================================

FUNCTION          hsu_conf_sel_assign_iface_slot_index_from_comp

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

==============================================================================*/
static void hsu_conf_sel_assign_iface_slot_index_from_comp(hsu_conf_sel_comp_info_type const *nv_composition_info)
{
  uint32 i;
  uint32 obex,acm,ecm;
  #ifdef FEATURE_HS_USB_DIGITAL_AUDIO_DEVICE
  uint32 audio;
  audio=0;
  #endif

  HSU_ASSERT(nv_composition_info != NULL);
  
  obex=acm=ecm=0;

  memset(hsu_iface_index_for_comp_array,HSU_IFACE_ABSENT_IN_COMP,sizeof(hsu_iface_index_for_comp_array));
    
  for(i = 0; i < NUM_OF_HSU_IFACE; i++)
  { 
    hsu_iface_index_enum_type iface_type;

    iface_type = nv_composition_info->iface_type[i];

    switch(iface_type)
    {
      case DIAG_HSU_IFACE:
      case NMEA_HSU_IFACE:
      case SER3_HSU_IFACE:
      case SER4_HSU_IFACE:
      case SER5_HSU_IFACE:
      case SER6_HSU_IFACE:
      case DCI_HSU_IFACE:
      case QDSS_HSU_IFACE:
        hsu_iface_index_for_comp_array[iface_type]=obex++;
        break;
        
      case MDM_HSU_IFACE:
      case MDM2_HSU_IFACE:
        hsu_iface_index_for_comp_array[iface_type]=acm++;
        break;
        
      case NET1_HSU_IFACE:
      case NET2_HSU_IFACE:
      case NET3_HSU_IFACE:
      case NET4_HSU_IFACE:
      case NET5_HSU_IFACE:
      case NET6_HSU_IFACE:
      case NET7_HSU_IFACE:
      case NET8_HSU_IFACE:
      case MBIM_HSU_IFACE:
      case GNSS_HSU_IFACE:
      case IPCR_HSU_IFACE:
      case LWIP_HSU_IFACE:
      case DPL_HSU_IFACE:
        hsu_iface_index_for_comp_array[iface_type]=ecm++;
        break;
#ifdef CI_MERGE        
      case AUDIO_HSU_IFACE:
        hsu_iface_index_for_comp_array[iface_type]=audio++;
        break;
#endif        
      default:
        break;
    }
  }

  HSU_ASSERT(obex == nv_composition_info->num_of_fd_instances_in_comp[HSU_CFG_SEL_OBEX_FD]);
  HSU_ASSERT(acm == nv_composition_info->num_of_fd_instances_in_comp[HSU_CFG_SEL_ACM_FD]);
  HSU_ASSERT(ecm == nv_composition_info->num_of_fd_instances_in_comp[HSU_CFG_SEL_ECM_FD]);
}


/*==============================================================================

FUNCTION          hsu_conf_sel_get_iface_type_from_port_id

==============================================================================*/
hsu_iface_index_enum_type hsu_conf_sel_get_iface_type_from_port_id
(
  sio_port_id_type port_id
)
{
  hsu_iface_index_enum_type iface_index = NO_HSU_IFACE;
  
  switch(port_id)
  {  
    case AL_SER_PORT_DIAG:
      iface_index = DIAG_HSU_IFACE;
      break;
    
#ifdef FEATURE_USB_DIAG_NMEA
    case AL_SER_PORT_NMEA:
      iface_index = NMEA_HSU_IFACE;
      break;
#endif /* FEATURE_USB_DIAG_NMEA */

#ifdef FEATURE_HS_USB_SER3_PORT
    case AL_SER_PORT_USB_SER3:
      iface_index = SER3_HSU_IFACE;
      break;
#endif /* FEATURE_HS_USB_SER3_PORT */

#ifdef FEATURE_HS_USB_SER4_PORT
    case AL_SER_PORT_USB_SER4:
      iface_index = SER4_HSU_IFACE;
      break;
#endif /* FEATURE_HS_USB_SER4_PORT */

#ifdef FEATURE_HS_USB_SER5_PORT
    case AL_SER_PORT_USB_SER5:
      iface_index = SER5_HSU_IFACE;
      break;
#endif /* FEATURE_HS_USB_SER5_PORT */ 

#ifdef FEATURE_HS_USB_SER6_PORT
    case AL_SER_PORT_USB_SER6:
      iface_index = SER6_HSU_IFACE;
      break;
#endif /* FEATURE_HS_USB_SER6_PORT */

#ifdef FEATURE_HS_USB_DCI_PORT
    case AL_SER_PORT_USB_DCI:
      iface_index = DCI_HSU_IFACE;
      break;
#endif /* FEATURE_HS_USB_DCI_PORT */

#ifdef FEATURE_HS_USB_QDSS_PORT
    case AL_SER_PORT_USB_QDSS:
      iface_index = QDSS_HSU_IFACE;
      break;
#endif /* FEATURE_HS_USB_QDSS_PORT */

    case AL_SER_PORT_MDM:
      iface_index = MDM_HSU_IFACE;
      break;

#ifdef FEATURE_HS_USB_CSVT_PORT
    case AL_SER_PORT_MDM2:
      iface_index = MDM2_HSU_IFACE;
      break;
#endif /* FEATURE_HS_USB_CSVT_PORT */
      
    case SIO_PORT_USB_NET_WWAN:
      iface_index = NET1_HSU_IFACE;
      /* This could also be MBIM_HSU_IFACE */
      break;
      
    case SIO_PORT_USB_RMNET2:
      iface_index = NET2_HSU_IFACE;
      /* This could also be GNSS_HSU_IFACE */
      break;

    case SIO_PORT_USB_RMNET3:
      iface_index = NET3_HSU_IFACE;
      break;

    case SIO_PORT_USB_RMNET4:
      iface_index = NET4_HSU_IFACE;
      break;

    case SIO_PORT_USB_RMNET5:
      iface_index = NET5_HSU_IFACE;
      break;

    case SIO_PORT_USB_RMNET6:
      iface_index = NET6_HSU_IFACE;
      break;

    case SIO_PORT_USB_RMNET7:
      iface_index = NET7_HSU_IFACE;
      break;

    case SIO_PORT_USB_RMNET8:
      iface_index = NET8_HSU_IFACE;
      break;

    case SIO_PORT_USB_IPCR:
      iface_index = IPCR_HSU_IFACE;
      break;

    case SIO_PORT_USB_LWIP:
      iface_index = LWIP_HSU_IFACE;
      break;

    case SIO_PORT_USB_DPL:
      iface_index = DPL_HSU_IFACE;
      break;

    default:
      break;
  }
  
  return iface_index;
}


/*==============================================================================

FUNCTION          hsu_conf_sel_get_iface_index_comp

DESCRIPTION       Returns the slot index for paricular interface

DEPENDENCIES      none  

RETURN VALUE      If the interface in present in the current composition,it will return the
                  slot index for that particular FD type else return HSU_IFACE_ABSENT_IN_COMP.
SIDE EFFECTS

==============================================================================*/
uint8 hsu_conf_sel_get_iface_index_comp(hsu_iface_index_enum_type iface_type)
{
  uint8 index;
  
  HSU_ASSERT(iface_type != NO_HSU_IFACE);
  HSU_ASSERT(iface_type < NUM_OF_HSU_IFACE);

  index = hsu_iface_index_for_comp_array[iface_type];

  if(index >= NUM_OF_HSU_IFACE)
  {
    HSU_ASSERT(index == HSU_IFACE_ABSENT_IN_COMP);
  }

  return index;
}


/*==============================================================================

FUNCTION          hsu_conf_sel_get_iface_type_from_instance_number

DESCRIPTION       Returns the inface type for paricular instance number

DEPENDENCIES      none  

RETURN VALUE      If the interface in present in the current composition, 
                  it will return the particular FD type else return NO_HSU_IFACE.
SIDE EFFECTS

==============================================================================*/
hsu_iface_index_enum_type hsu_conf_sel_get_iface_type_from_instance_number(
  uint32 iface_instance_number, hsu_iface_index_enum_type start_type)
{
  hsu_iface_index_enum_type type, end_type;

  switch (start_type)
  {
    case OBEX_HSU_IFACE_START:    end_type = OBEX_HSU_IFACE_END;
      break;
    case ACM_HSU_IFACE_START:     end_type = ACM_HSU_IFACE_END;
      break;
    case ECM_HSU_IFACE_START:     end_type = ECM_HSU_IFACE_END;
      break;
    default:
      return NO_HSU_IFACE;
  }
  
  for (type = start_type; type < end_type; type++)
  {
    if (iface_instance_number == hsu_iface_index_for_comp_array[type])
    {
      return type;
    }
  }
  return NO_HSU_IFACE;
}

/*==============================================================================

FUNCTION          HSU_CONF_SEL_CHANGE_PID_COMP

==============================================================================*/
boolean hsu_conf_sel_change_pid_comp(uint16 pid)
{
  hsu_conf_sel_comp_info_type const *comp_details; 

  HSU_ASSERT(pid);

  comp_details = hsu_conf_sel_comp_utils_get_comp_details_by_pid(pid);

  if(comp_details)
  {
    hsu_conf_sel_nv_efs_write(
      HSU_EFS_DEVICE_COMPOSITION, 
      (uint32)comp_details->efs_comp_id);
    return TRUE;
  }
  else
  {
    HSU_ERR_FATAL("hsu_conf_sel_get_iface_index_comp: PID invalid", 0, 0, 0);
    return FALSE;
  }
}

/*==============================================================================

FUNCTION          HSU_CONF_SEL_GET_CUR_PID

==============================================================================*/
uint16 hsu_conf_sel_get_cur_pid(void)
{
  /* Use the remote API */
  hsu_conf_sel_comp_info_type const *cur_comp;
  cur_comp = hsu_conf_sel_get_cur_comp_details();

  if(cur_comp)
  {
    return cur_comp->product_id;
  }
  else
  {
    HSU_ERR_FATAL("hsu_conf_sel_get_cur_prod_id: cur_comp NULL", 0, 0, 0);
    return 0;
  }
  
} /* hsu_conf_sel_get_cur_prod_id */

/*==============================================================================

FUNCTION          HSU_CONF_SEL_IS_CURRENT_COMP_UNDP

==============================================================================*/
boolean hsu_conf_sel_is_current_comp_undp(void)
{
  boolean is_valid = FALSE;
  hsu_conf_sel_comp_info_type const *cur_dev_comp_details = NULL;

  cur_dev_comp_details = hsu_conf_sel_get_cur_comp_details();

  if(!cur_dev_comp_details)
  {
     HSU_ERR_FATAL("hsu_conf_sel_get_cur_prod_id: "
      "cur_dev_comp_details NULL", 0, 0, 0);
  }

  if(cur_dev_comp_details->supported_drv_by_comp == HSU_UNDP_CMP_ONLY)
  {
    is_valid = TRUE;
  }
  else
  {
    is_valid = FALSE;
  }

  return is_valid;
}  /* hsu_conf_sel_is_current_comp_undp */

/*==============================================================================

FUNCTION          HSU_CONF_SEL_IS_RMNET_OVER_SECONDARY_CORE

==============================================================================*/
boolean hsu_conf_sel_is_rmnet_over_secondary_core(void)
{
  return FALSE;
}

