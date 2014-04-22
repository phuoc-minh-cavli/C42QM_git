/*==============================================================================

  USB and HSIC Clock Management
  
  GENERAL DESCRIPTION
    This module defines APIs for USB and HSIC clock management.

==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/src/common/9205/hsu_clkmgr.c#2 $
  $DateTime: 2020/03/31 00:58:24 $
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------
  02/20/20  cs      Enabled usb sleep clock to recieve interrupt from QGIC
  11/02/13  jc      Initial version

    
==============================================================================    
  Copyright (c) 2013 - 2020 by Qualcomm Technologies, Inc. All Rights Reserved.
==============================================================================*/

//==============================================================================
// Include Files for Module
//==============================================================================
#include "customer.h"
#include "hsu_clkmgr.h"
#include "hsu_core_config_int.h"
#include "hsu_conf_sel_nv.h"
#include "hsu_config_selector.h"
#include "hsu_conf_sel_nv.h"
#include "npa.h"
#include "hsu_os_abstract_bus.h"

#include "DDIClock.h"
#include "hsu_log.h"
#include "HAL_hsusb.h"
#include "port_sync.h"

// For VMIDT restoration // MOCK_UTMI is disable since host mode is not used
//#include "tz_syscalls.h"
//#include "syscalls.h"

#include "jusb_core.h"

#include "hsu_hwio_dependency.h"
#include "hsu_platform.h"

//==============================================================================
// Typedefs
//==============================================================================

/* HSU CLKMGR instance type */  
typedef struct {

  hsu_core_type_enum          core_type;

  hsu_clkmgr_state            clock_state;

  hsu_clkmgr_state            npa_clock_state;

  hsu_clkmgr_speed_level_type clock_speed;

  //============================================================
  // Clocks that needs to be control via NPA API call to RPM.
  //   This may incur milliseconds of delays
  //============================================================

  // PCNOC sources frequency for CI USB gcc_usb_hs_ahb_clk and SNPS USB gcc_usb_phy_cfg_ahb_clk
  npa_client_handle           bus_pcnoc_npa_client;
  
  // SNOC sources frequency for SNPS USB gcc_sys_noc_usb3_axi_clk
  npa_client_handle           bus_snoc_npa_client;

  npa_client_handle           bus_bimc_npa_client;

#if 0
  npa_client_handle           bus_apps_cpu_npa_client;
#endif

  DalDeviceHandle            *phClock; 
  
  //========================
  // CI USB Core Clocks 
  //========================
  
  // CI USB peripheral clock used for DMA
  ClockIdType                 gcc_usb_hs_phy_cfg_ahb_clk;
  
  // CI USB system clock used for link controller protocol engine.  
  // This clock is not used in case AHB2AHB Bridge Bypass is enabled
  ClockIdType                 gcc_usb_hs_system_clk;
  
  ClockIdType                 gcc_usb_hs_inactivity_timers_clk;

  ClockIdType                 gcc_pcnoc_usb_clk;
  
  ClockIdType                 gcc_usb2a_phy_sleep_clk;

  ClockPowerDomainIdType      power_domain_id;

  // Debugging purpose APPS CPU VDD vote to prevent power collapse
  npa_client_handle           cpu_vdd_handle;
  boolean                     is_cpu_vdd_voted;
} hsu_clkmgr_type;

//==============================================================================
// Macros
//==============================================================================
#define HSU_PNOC_SVS_MODE_SPEED_KHZ       (50000)
#define HSU_PNOC_NOMINAL_MODE_SPEED_KHZ   (100000)
#define HSU_APPS_CPU_NOMINAL_MODE_SPEED_KHZ (1200000)

// Note that clock driver will vote for the next closest performance level
// gcc_usb_phy_cfg_ahb_clk  cbc  gcc_pcnoc_grp  usb_30  sm_cbc  19.20  50.00  100.00  133.33
#define HSU_PCNOC_SVS2_MODE_SPEED_KHZ                  (19200)
#define HSU_PCNOC_SVS_MODE_SPEED_KHZ                   (50000)
#define HSU_PCNOC_NOMINAL_MODE_SPEED_KHZ               (100000)
#define HSU_PCNOC_TURBO_MODE_SPEED_KHZ                 (130000)

// gcc_bimc_clk  cbc gcc_bimc_ddr_grp  bimc  sm_cbc  202.10  404.20  808.40  1066.00
// The 19.2 MHz vote is for allowing USB HW access to DDR while APPS is in power collapse
#define HSU_BIMC_MIN_USB_MODE_SPEED_KHZ                (19200)
#define HSU_BIMC_SVS_MODE_SPEED_KHZ                    (120000)
#define HSU_BIMC_NOMINAL_MODE_SPEED_KHZ                (240000)
#define HSU_BIMC_TURBO_MODE_SPEED_KHZ                  (400000)

//==============================================================================
// Constants and Variables
//==============================================================================
static hsu_clkmgr_type  hsu_clkmgr;

uint32 gslp_clk_enabled = 0;
//==============================================================================
// Local Function Prototypes
//==============================================================================
static void hsu_DalClock_SetClockFrequency(DalDeviceHandle * _h,  ClockIdType  nClock,  uint32  nFreq,  ClockFrequencyType  eMatch);
static void hsu_DalClock_EnableClock(DalDeviceHandle * _h,  ClockIdType  nClock);
static void hsu_DalClock_DisableClock(DalDeviceHandle * _h,  ClockIdType  nClock);
static void hsu_DalClock_GetClockId(DalDeviceHandle * _h,  const char * szClock,  ClockIdType * pnId);
static void hsu_npa_issue_required_request(npa_client_handle client, npa_resource_state state);
static void hsu_npa_complete_request(npa_client_handle client);
static npa_client_handle hsu_npa_create_sync_client(const char       *resource_name, 
                                             const char       *client_name,
                                             npa_client_type   client_type);


//==============================================================================
// Global Function Definitions
//==============================================================================

//==============================================================================
// FUNCTION hsu_clkmgr_config_clocks
//==============================================================================
void hsu_clkmgr_config_clocks(hsu_core_type_enum core_type)
{
  DalDeviceHandle    *phClock                   = hsu_clkmgr.phClock;
  hsu_clkmgr_state    prev_state;
  hsu_event_log_enum  event = EVT_CLK_ON___UNKNOWN_CONNECTED;

  HSU_ASSERT(core_type < HSU_NUM_OF_CORES_TYPES);

  prev_state = hsu_clkmgr.clock_state;

  if (prev_state == HSU_CLKMGR__CLK_ON)
  {
    /* Clock is enabled */
    return;
  }

  if (prev_state == HSU_CLKMGR__CLK_OFF)
  {
    // Event logging
    if (hsu_port_is_attached(hsu_hal_os_bus_core_id(core_type)))
    {
      switch (core_get_curr_speed(core_type))
      {
        case SPEED_SUPER: event = EVT_CLK_ON___SS_CONNECTED;  break;
        case SPEED_HIGH:  event = EVT_CLK_ON___HS_CONNECTED;  break;
        case SPEED_FULL:  event = EVT_CLK_ON___FS_CONNECTED;  break;
      }
      hsu_event_log(event, hsu_hal_os_bus_core_id(core_type));
    }
    else
    {
      hsu_event_log(EVT_CLK_ON___NOT_CONNECTED, hsu_hal_os_bus_core_id(core_type));
    }

    if (HSU_USB3_SNPS_CORE == core_type) 
    {
      //========================
      // CI USB Core Clocks 
      //========================

      // SW needs to vote for PNOC for gcc_usb_hs_ahb_clk freq (min = 50)
      // This is required in order for PNOC bus to stay above 19.2 Mhz
      if (hsu_clkmgr.clock_speed == HSU_CLKMGR_SPEED_LOW)
      {
        hsu_npa_issue_required_request(hsu_clkmgr.bus_pcnoc_npa_client, HSU_PNOC_SVS_MODE_SPEED_KHZ);
      }
      else if (hsu_clkmgr.clock_speed == HSU_CLKMGR_SPEED_MID)
      {
        hsu_npa_issue_required_request(hsu_clkmgr.bus_pcnoc_npa_client, HSU_PNOC_NOMINAL_MODE_SPEED_KHZ);
      }
      else
      {
        hsu_npa_issue_required_request(hsu_clkmgr.bus_pcnoc_npa_client, HSU_PNOC_NOMINAL_MODE_SPEED_KHZ);
      }
   
      hsu_DalClock_EnableClock(phClock, hsu_clkmgr.gcc_usb_hs_phy_cfg_ahb_clk);
      hsu_DalClock_SetClockFrequency(phClock, hsu_clkmgr.gcc_usb_hs_system_clk, 133, CLOCK_FREQUENCY_MHZ_AT_LEAST);
      hsu_DalClock_EnableClock(phClock, hsu_clkmgr.gcc_usb_hs_system_clk);
	  hsu_DalClock_EnableClock(phClock, hsu_clkmgr.gcc_usb_hs_inactivity_timers_clk);
	  hsu_DalClock_EnableClock(phClock, hsu_clkmgr.gcc_pcnoc_usb_clk);
	  if(gslp_clk_enabled == 0)
	  {
	  hsu_DalClock_EnableClock(phClock, hsu_clkmgr.gcc_usb2a_phy_sleep_clk);
	  gslp_clk_enabled = 1;
	  }
    
      //is_ahb2ahb_bypass_enabled = HAL_hsusb_GetAsyncBridgesBypass(HSU_CORE_CONFIG_USB3_IDX);

    }
    else
    {
      HSU_ERR_FATAL("unknown core type (%u)", core_type, 0, 0);
    }
  }

  /* Set state variable only after clock is configured in case other 
  * thread queries this state in the middle of configuration 
  */
  hsu_clkmgr.clock_state = HSU_CLKMGR__CLK_ON;

}

//==============================================================================
// FUNCTION hsu_clkmgr_config_npa_clocks
//==============================================================================
void hsu_clkmgr_config_npa_clocks(hsu_core_type_enum core_type)
{
  hsu_clkmgr_state    prev_state;

  HSU_ASSERT(core_type < HSU_NUM_OF_CORES_TYPES);

  prev_state = hsu_clkmgr.npa_clock_state;

  if (prev_state == HSU_CLKMGR__CLK_ON)
  {
    /* Clock is enabled */
    return;
  }

#ifndef FEATURE_HS_USB_NO_HS_OTG_CORE
  if (prev_state == HSU_CLKMGR__CLK_OFF)
  {
    if (hsu_clkmgr.cpu_vdd_handle && (FALSE == hsu_clkmgr.is_cpu_vdd_voted))
    {
      hsu_clkmgr.is_cpu_vdd_voted = TRUE;
      hsu_npa_issue_required_request(hsu_clkmgr.cpu_vdd_handle, 1);
    }

    if (HSU_USB3_SNPS_CORE == core_type) 
    {
      if (hsu_clkmgr.clock_speed == HSU_CLKMGR_SPEED_LOW)
      {
        hsu_npa_issue_required_request(hsu_clkmgr.bus_pcnoc_npa_client, HSU_PNOC_SVS_MODE_SPEED_KHZ);
        hsu_npa_issue_required_request(hsu_clkmgr.bus_bimc_npa_client, HSU_BIMC_MIN_USB_MODE_SPEED_KHZ);
      }
      else if (hsu_clkmgr.clock_speed == HSU_CLKMGR_SPEED_MID)
      {
        hsu_npa_issue_required_request(hsu_clkmgr.bus_pcnoc_npa_client, HSU_PNOC_NOMINAL_MODE_SPEED_KHZ);
        hsu_npa_issue_required_request(hsu_clkmgr.bus_bimc_npa_client, HSU_BIMC_NOMINAL_MODE_SPEED_KHZ);
      }
      else
      {
        hsu_npa_issue_required_request(hsu_clkmgr.bus_pcnoc_npa_client, HSU_PNOC_NOMINAL_MODE_SPEED_KHZ);
        hsu_npa_issue_required_request(hsu_clkmgr.bus_bimc_npa_client, HSU_BIMC_NOMINAL_MODE_SPEED_KHZ);
      }
    }
    else
    {
      HSU_ERR_FATAL("unknown core type (%u)", core_type, 0, 0);
    }
  }
#endif // !FEATURE_HS_USB_NO_HS_OTG_CORE

  // Set state variable only after clock is configured in case other 
  // thread queries this state in the middle of configuration.

  hsu_clkmgr.npa_clock_state = HSU_CLKMGR__CLK_ON;

  HSU_ULOG_ENUM2(NO_MSG, BUS_LOG, "",     
    hsu_clkmgr.npa_clock_state, HSU_CLKMGR__NPA_OFFSET2, 
    hsu_clkmgr.core_type, HSU_CORE_OFFSET2);
  
}

//==============================================================================
// FUNCTION hsu_clkmgr_release_clocks
//==============================================================================
void hsu_clkmgr_release_clocks(hsu_core_type_enum core_type)
{
  boolean           is_system_clock_on        = FALSE;
  DalDeviceHandle  *phClock                   = hsu_clkmgr.phClock;
  hsu_clkmgr_state  prev_state;
  
  hsu_event_log_enum event = EVT_CLK_OFF__UNKNOWN_CONNECTED;

  HSU_ASSERT(core_type < HSU_NUM_OF_CORES_TYPES);
  
  // Set state variable prior to turning off in case other task queries in middle of turning off 
  prev_state = hsu_clkmgr.clock_state;
  hsu_clkmgr.clock_state = HSU_CLKMGR__CLK_OFF;

  if (prev_state == HSU_CLKMGR__CLK_OFF)
  {
    /* Clock is already OFF */
    HSU_MSG_HIGH_1("Clock is already off: id = %u", hsu_clkmgr.core_type);
    return;
  }

  if (prev_state == HSU_CLKMGR__CLK_ON)
  {
    // Event logging
    if (hsu_port_is_attached(hsu_hal_os_bus_core_id(core_type)))
    {
      switch (core_get_curr_speed(core_type))
      {
        case SPEED_SUPER: event = EVT_CLK_OFF__SS_CONNECTED;  break;
        case SPEED_HIGH:  event = EVT_CLK_OFF__HS_CONNECTED;  break;
        case SPEED_FULL:  event = EVT_CLK_OFF__FS_CONNECTED;  break;
      }
      hsu_event_log(event, hsu_hal_os_bus_core_id(core_type));
    }
    else
    {
      hsu_event_log(EVT_CLK_OFF__NOT_CONNECTED, hsu_hal_os_bus_core_id(core_type));
    }

    if (HSU_USB3_SNPS_CORE == core_type)
    {
      //========================
      // CI USB Core Clocks 
      //========================

      // Turn off USB System clock only if it was ON since it could be OFF 
      // if AHB2AHB Bridge Bypass is enabled
      is_system_clock_on = DalClock_IsClockOn(phClock, hsu_clkmgr.gcc_usb_hs_system_clk);
      if (is_system_clock_on)
      {
        hsu_DalClock_DisableClock(phClock, hsu_clkmgr.gcc_usb_hs_system_clk);
      }
      hsu_DalClock_DisableClock(hsu_clkmgr.phClock, hsu_clkmgr.gcc_usb_hs_phy_cfg_ahb_clk);
	  hsu_DalClock_DisableClock(hsu_clkmgr.phClock, hsu_clkmgr.gcc_usb_hs_inactivity_timers_clk);
	  hsu_DalClock_DisableClock(hsu_clkmgr.phClock, hsu_clkmgr.gcc_pcnoc_usb_clk);
    }
    else
    {
      HSU_ERR_FATAL("unknown core (%u)", core_type, 0, 0);
    }
    
    if (hsu_clkmgr.cpu_vdd_handle && hsu_clkmgr.is_cpu_vdd_voted)
    {
      hsu_clkmgr.is_cpu_vdd_voted = FALSE;
      hsu_npa_issue_required_request(hsu_clkmgr.cpu_vdd_handle, 0);
    }
  }
  else
  {
    HSU_ASSERT("hsu_clkmgr_release_clocks_i: HSUSB invalid clock state" == NULL);
  }

   HSU_ULOG_ENUM2_1(NO_MSG, BUS_LOG, "is_system_clk_on %u",     
    hsu_clkmgr.clock_state, HSU_CLKMGR_OFFSET2, 
    hsu_clkmgr.core_type, HSU_CORE_OFFSET2, 
    is_system_clock_on);
}

//==============================================================================
// FUNCTION hsu_clkmgr_release_npa_clocks
//==============================================================================
void hsu_clkmgr_release_npa_clocks(hsu_core_type_enum core_type)
{
  hsu_clkmgr_state  prev_state;
  
  HSU_ASSERT(core_type < HSU_NUM_OF_CORES_TYPES);
  
  // Set state variable prior to turning off in case other task queries in middle of turning off 
  prev_state = hsu_clkmgr.npa_clock_state;
  hsu_clkmgr.npa_clock_state = HSU_CLKMGR__CLK_OFF;

  if (prev_state == HSU_CLKMGR__CLK_OFF)
  {
    /* Clock is already OFF */
    HSU_MSG_HIGH_1("NPA clock is already off: id = %u", hsu_clkmgr.core_type);
    return;
  }

#ifndef FEATURE_HS_USB_NO_HS_OTG_CORE
  if (prev_state == HSU_CLKMGR__CLK_ON)
  {
    if (HSU_USB3_SNPS_CORE == core_type)
    {
      //========================
      // CI USB Core Clocks 
      //========================
      hsu_npa_complete_request(hsu_clkmgr.bus_pcnoc_npa_client);
      hsu_npa_complete_request(hsu_clkmgr.bus_bimc_npa_client);
    }
    else
    {
      HSU_ERR_FATAL("hsu_clkmgr_release_npa_clocks: HSUSB invalid clock state, (%u)",
        prev_state, 0, 0);
    }
  }
#endif /* !FEATURE_HS_USB_NO_HS_OTG_CORE */

  HSU_ULOG_ENUM2(NO_MSG, BUS_LOG, "",     
    hsu_clkmgr.npa_clock_state, HSU_CLKMGR__NPA_OFFSET2, 
    hsu_clkmgr.core_type, HSU_CORE_OFFSET2);

}

//==============================================================================
// FUNCTION hsu_clkmgr_init
//==============================================================================
void hsu_clkmgr_init(void)
{
  DALResult                 dal_result;
  DalDeviceHandle          *phClock;
  hsu_platform_info_type const * platform_info = hsu_platform_get_info();

  hsu_clkmgr.core_type    = HSU_USB3_SNPS_CORE;
  hsu_clkmgr.clock_state  = HSU_CLKMGR__CLK_OFF;
  hsu_clkmgr.npa_clock_state  = HSU_CLKMGR__CLK_OFF;
  hsu_clkmgr.clock_speed  = hsu_conf_sel_get_efs_perf_settings();
  
  dal_result = DAL_DeviceAttach(DALDEVICEID_CLOCK, &hsu_clkmgr.phClock);
  if ((dal_result != DAL_SUCCESS) || (NULL == hsu_clkmgr.phClock))
  {
    HSU_ERR_FATAL("hsu_clkmgr_init: DAL_DeviceAttach failed: %u", dal_result, 0, 0);
	return;
  }
  phClock = hsu_clkmgr.phClock; 
  

  //========================
  // CI USB Core Clocks
  //========================
  hsu_DalClock_GetClockId(phClock, "gcc_usb_hs_phy_cfg_ahb_clk", &hsu_clkmgr.gcc_usb_hs_phy_cfg_ahb_clk); 
  hsu_DalClock_GetClockId(phClock, "gcc_usb_hs_system_clk", &hsu_clkmgr.gcc_usb_hs_system_clk);
  hsu_DalClock_GetClockId(phClock, "gcc_usb_hs_inactivity_timers_clk", &hsu_clkmgr.gcc_usb_hs_inactivity_timers_clk);
  hsu_DalClock_GetClockId(phClock, "gcc_pcnoc_usb_clk", &hsu_clkmgr.gcc_pcnoc_usb_clk);
  hsu_DalClock_GetClockId(phClock, "gcc_usb2a_phy_sleep_clk", &hsu_clkmgr.gcc_usb2a_phy_sleep_clk);

  // Get USB power domain ID
  // No Power Domain for USB in Twizy
  //dal_result = DalClock_GetPowerDomainId(phClock, "usb30_gdsc", &hsu_clkmgr.power_domain_id);

  switch (platform_info->hsu_platform_id)
  {
    case HSU_PLATFORM_RUMI:
    break;

    default:
    {
      hsu_clkmgr.bus_pcnoc_npa_client = hsu_npa_create_sync_client("/clk/pcnoc", "hsusb_pcnoc_bus_client", NPA_CLIENT_REQUIRED);
      HSU_ASSERT(hsu_clkmgr.bus_pcnoc_npa_client != NULL);

#if 0      
      hsu_clkmgr.bus_apps_cpu_npa_client = hsu_npa_create_sync_client("/clk/cpu", "hsusb_apps_cpu_client", NPA_CLIENT_REQUIRED);
      HSU_ASSERT(hsu_clkmgr.bus_apps_cpu_npa_client != NULL);
#endif

      hsu_clkmgr.bus_bimc_npa_client = hsu_npa_create_sync_client("/clk/bimc", "hsusb_bimc_bus_client", NPA_CLIENT_REQUIRED);
      HSU_ASSERT(hsu_clkmgr.bus_bimc_npa_client != NULL);

      //====================================
      // For debugging low power mode 
      //====================================
      if (hsu_conf_sel_disable_sleep_voting())
      {
        hsu_clkmgr.cpu_vdd_handle = hsu_npa_create_sync_client("/core/cpu/vdd", "hsu_clkmgr_cpu_vdd", NPA_CLIENT_REQUIRED); 
        //HSU_ASSERT(hsu_clkmgr.cpu_vdd_handle != NULL);
      }
      else
      {
        hsu_clkmgr.cpu_vdd_handle = NULL;
      } 
    }
    break;
  }
}

//==============================================================================
// FUNCTION hsu_clkmgr_enable_power_domain
//==============================================================================
void hsu_clkmgr_enable_power_domain(boolean enable, hsu_core_type_enum core_type)
{
  // No power domain for USB in Twizy
#if 0
  DALResult dal_result = DAL_SUCCESS;
  //DALResult dal_result = DAL_SUCCESS;
  hsu_platform_info_type const * platform_info = hsu_platform_get_info();

  switch (core_type)
  {
#ifdef FEATURE_HS_USB_SNPS_CORE
    // The case falls on to default case when SNPS core is not in use.
    case HSU_USB3_SNPS_CORE:  
      if(hsu_conf_sel_is_usb_core_snps()) 
      {
        if (enable)
        {
          dal_result = DalClock_EnablePowerDomain(hsu_clkmgr.phClock, hsu_clkmgr.power_domain_id);
          //HSU_ULOG_NOFN_2(NO_MSG, REG_LOG, "0x%08X, 0x%08X", HWIO_GCC_USB30_GDSCR_ADDR, (HWIO_GCC_USB30_GDSCR_IN & ~HWIO_GCC_USB30_GDSCR_SW_COLLAPSE_BMSK) | (HWIO_GCC_USB30_GDSCR_SW_COLLAPSE_DISABLE_FVAL << HWIO_GCC_USB30_GDSCR_SW_COLLAPSE_SHFT));
          //HSU0_ULOG_2(NO_MSG, REG_LOG, "0x%08X, 0x%08X", HWIO_GCC_USB30_GDSCR_ADDR, (HWIO_GCC_USB30_GDSCR_IN & ~HWIO_GCC_USB30_GDSCR_SW_COLLAPSE_BMSK) | (HWIO_GCC_USB30_GDSCR_SW_COLLAPSE_DISABLE_FVAL << HWIO_GCC_USB30_GDSCR_SW_COLLAPSE_SHFT));
        }
        else
        { 
          dal_result = DalClock_DisablePowerDomain(hsu_clkmgr.phClock, hsu_clkmgr.power_domain_id);
          //HSU_ULOG_NOFN_2(NO_MSG, REG_LOG, "0x%08X, 0x%08X", HWIO_GCC_USB30_GDSCR_ADDR, (HWIO_GCC_USB30_GDSCR_IN & ~HWIO_GCC_USB30_GDSCR_SW_COLLAPSE_BMSK) | (HWIO_GCC_USB30_GDSCR_SW_COLLAPSE_ENABLE_FVAL << HWIO_GCC_USB30_GDSCR_SW_COLLAPSE_SHFT));
        }
        HSU_ASSERT(dal_result == DAL_SUCCESS);
        break;
      }
#endif /* FEATURE_HS_USB_SNPS_CORE */
    default:
    {
      HSU_ERR_FATAL("Power Domain invalid for core_type (%u), enable(%u)", core_type, enable, 0);
    }
  }
#endif
}


//==============================================================================
// FUNCTION hsu_clkmgr_assert_clock
//==============================================================================
void hsu_clkmgr_assert_reset(boolean set_assert, hsu_clkmgr_reset_clock_type clock_type)
{
  DALResult       dal_result = DAL_SUCCESS;
  uint32 val;

  if (set_assert)
  {
    val = 0x1;
  }
  else
  {
    val = 0x0;
  }

  switch (clock_type)
  {
    case HSU_CLKMGR__RESET_GCC_USB2_M:
      HWIO_GCC_USB_HS_BCR_OUTM(HWIO_GCC_USB_HS_BCR_BLK_ARES_BMSK,(val << HWIO_GCC_USB_HS_BCR_BLK_ARES_SHFT));
      break;

    case HSU_CLKMGR__RESET_GCC_USB2_PHY_RESET_M:
      HWIO_GCC_QUSB2_PHY_BCR_OUTM(HWIO_GCC_QUSB2_PHY_BCR_BLK_ARES_BMSK,(val << HWIO_GCC_QUSB2_PHY_BCR_BLK_ARES_SHFT));
      break;

    default:
    {
      HSU_ERR_FATAL("Clock Reset Type invalid (%u), set_assert(%u)", 0, set_assert, 0);      
    }
  } // switch(clock_type)

  HSU_ASSERT(dal_result == DAL_SUCCESS);

}


//==============================================================================
// FUNCTION hsu_clkmgr_is_clock_on
//==============================================================================
boolean hsu_clkmgr_is_clock_on(hsu_core_type_enum core_type)
{
  boolean is_clock_on = FALSE;
  
  switch (core_type)
  {
    case HSU_USB3_SNPS_CORE:
      is_clock_on = (hsu_clkmgr.clock_state == HSU_CLKMGR__CLK_ON);
      break;

    default:
      HSU_ERR_FATAL("unknown core_type (%u)", core_type, 0, 0);
  }
  
  return is_clock_on;
}


//==============================================================================
// FUNCTION hsu_clkmgr_is_peripheral_clock_on
//==============================================================================
boolean hsu_clkmgr_is_peripheral_clock_on(hsu_core_type_enum core_type)
{
  boolean is_periph_clock_on = FALSE;
  
  switch (core_type)
  {
    /* Below logic is implementation specific to this file.
    ** In HSU_CLKMGR__CLK_SUSPEND usb_p clk and hsic_p clocks to access
    ** Link HW register is enabled.
    */
    case HSU_USB3_SNPS_CORE:
      is_periph_clock_on = (hsu_clkmgr.clock_state != HSU_CLKMGR__CLK_OFF);
      break;

    default:
      HSU_ASSERT(FALSE);
  }
  
  return is_periph_clock_on;
}

//==============================================================================
// Local Function Definitions
//==============================================================================

//==============================================================================
// FUNCTION hsu_clkmgr_enable_ahb2ahb_bridge_bypass_mode
//==============================================================================
void hsu_clkmgr_enable_ahb2ahb_bridge_bypass_mode(hsu_core_type_enum core_type)
{
#if !defined(FEATURE_HS_USB_PRE_SILICON_BU)
  boolean         is_system_clock_on;
  DALResult       dal_result;
  DalDeviceHandle *phClock;

  /* HSUSB */
  ClockIdType gcc_usb_hs_system_clk = NULL;

#ifdef CI_MERGE
  /* HSIC */
  ClockIdType gcc_usb_hsic_system_clk = NULL;
#endif

  dal_result = DAL_DeviceAttach(DALDEVICEID_CLOCK, &phClock);

  if ((dal_result != DAL_SUCCESS) || (NULL == phClock))
  {
    HSU_ERR_FATAL("DAL_DeviceAttach failed: %u", dal_result, 0, 0);
	return;
  }

  switch (core_type)
  {
    case HSU_USB3_SNPS_CORE:
    {
      if (hsu_clkmgr.clock_state == HSU_CLKMGR__CLK_OFF)
      {
        HSU_ERR_FATAL("USB clocks are OFF trying to bypass AHB brdige: state (%u)",
          hsu_clkmgr.clock_state, 0, 0);
      }
      
      if (HAL_hsusb_GetAsyncBridgesBypass(HSU_CORE_CONFIG_USB3_IDX))
      {
        /* Bridge has already been bypassed */
        HSU_ULOG_ENUM(NO_MSG, BUS_LOG, "ENABLE_ahb2ahb_bridge_bypass_mode, redund",
          HSU_CORE_OFFSET2, core_type);
      }
      else
      {
        HAL_hsusb_SetAsyncBridgesBypass(HSU_CORE_CONFIG_USB3_IDX, 1);

        /* Turn off system clock if not already off */     
        DalClock_GetClockId(phClock, "gcc_usb_hs_system_clk", &gcc_usb_hs_system_clk);
        HSU_ASSERT(gcc_usb_hs_system_clk != NULL);        

        is_system_clock_on = DalClock_IsClockOn(phClock, gcc_usb_hs_system_clk);

        HSU_ULOG_ENUM_1(NO_MSG, BUS_LOG, "ENABLE_ahb2ahb_bridge_bypass_mode, is_sys_clk %u",
          HSU_CORE_OFFSET2, core_type, is_system_clock_on);

#ifdef FEATURE_HS_USB_TURN_OFF_SYSCLK_FOR_AHB2AHB_BYPASS
        if (is_system_clock_on)
        {
          dal_result = DalClock_DisableClock(phClock, gcc_usb_hs_system_clk);
          HSU_ASSERT(dal_result == DAL_SUCCESS);
        }
#endif /* FEATURE_HS_USB_TURN_OFF_SYSCLK_FOR_AHB2AHB_BYPASS */
      }
    }
    break;
    default:
      HSU_ERR_FATAL("Unknown core_type (%u)", core_type, 0, 0);
  } 
#endif /* FEATURE_HS_USB_PRE_SILICON_BU */
}

/*============================================================================
  hsu_clkmgr_disable_ahb2ahb_bridge_bypass_mode
  ============================================================================*/
void hsu_clkmgr_disable_ahb2ahb_bridge_bypass_mode(hsu_core_type_enum core_type)
{
  boolean         is_system_clock_on;
  DALResult       dal_result;
  DalDeviceHandle *phClock;

  /* CIUSB */
  ClockIdType gcc_usb_hs_system_clk = NULL;

  dal_result = DAL_DeviceAttach(DALDEVICEID_CLOCK, &phClock);

  if ((dal_result != DAL_SUCCESS) || (NULL == phClock))
  {
    HSU_ERR_FATAL("DAL_DeviceAttach failed: %u", dal_result, 0, 0);
	return;
  }

  switch (core_type)
  {
    case HSU_USB3_SNPS_CORE:
    {
      if (hsu_clkmgr.clock_state == HSU_CLKMGR__CLK_OFF)
      {
        HSU_ERR_FATAL("USB clocks are OFF trying to disable AHB brdige bypass: state (%u)",
          hsu_clkmgr.clock_state, 0, 0);
      }
      
      if (FALSE == HAL_hsusb_GetAsyncBridgesBypass(HSU_CORE_CONFIG_USB3_IDX))
      {
        /* Bridge has already been bypassed */
        HSU_ULOG_ENUM(NO_MSG, BUS_LOG, "DISABLE_ahb2ahb_bridge_bypass_mode, redund",
          HSU_CORE_OFFSET2, core_type);
      }
      else
      {
        /* Turn off system clock if not already off */     
        DalClock_GetClockId(phClock, "gcc_usb_hs_system_clk", &gcc_usb_hs_system_clk);
        HSU_ASSERT(gcc_usb_hs_system_clk != NULL);        

        is_system_clock_on = DalClock_IsClockOn(phClock, gcc_usb_hs_system_clk);

        if (FALSE == is_system_clock_on)
        {
          dal_result = DalClock_EnableClock(phClock, gcc_usb_hs_system_clk);
          HSU_ASSERT(dal_result == DAL_SUCCESS);
        }

        HAL_hsusb_SetAsyncBridgesBypass(HSU_CORE_CONFIG_USB3_IDX, 0);

        /* Wait 100us for AHB bridge to settle */
        hsu_os_delay_us(100);

        HSU_ULOG_ENUM_1(NO_MSG, BUS_LOG, "DISABLE_ahb2ahb_bridge_bypass_mode, is_sys_clk %u",
          HSU_CORE_OFFSET2, core_type, is_system_clock_on);
      }
    }
    break;
    
    default:
      HSU_ASSERT(FALSE);
  }
}


//==============================================================================
// FUNCTION hsu_DalClock_SetClockFrequency
//==============================================================================
static void hsu_DalClock_SetClockFrequency(DalDeviceHandle * _h,  ClockIdType  nClock,  uint32  nFreq,  ClockFrequencyType  eMatch)
{
  DALResult dal_result;
  uint32    resultFreq;
  hsu_platform_info_type const * platform_info = hsu_platform_get_info();

  switch (platform_info->hsu_platform_id)
  {
    case HSU_PLATFORM_RUMI:
    break;

    default:
    {
      dal_result = DalClock_SetClockFrequency(_h, nClock, nFreq, eMatch, &resultFreq);
      if (dal_result != DAL_SUCCESS)
      {
        HSU_ERR_FATAL("DalClock_SetClockFrequency failed: result(0x%x), handle(0x%x), clock(0x%x)", dal_result, _h, nClock);
      }

      if (resultFreq < nFreq)
      {
        // Print warning that clock frequency is lower than requested.
        HSU_ULOG_1(NO_MSG, BUS_LOG, "error, resultFreq %u", resultFreq);
      }
    }
    break;
  }
}

//==============================================================================
// FUNCTION hsu_DalClock_EnableClock
//==============================================================================
static void hsu_DalClock_EnableClock(DalDeviceHandle * _h,  ClockIdType  nClock)
{
  DALResult dal_result;
  hsu_platform_info_type const * platform_info = hsu_platform_get_info();

  switch (platform_info->hsu_platform_id)
  {
    case HSU_PLATFORM_RUMI:
    break;

    default:
    {
      dal_result = DalClock_EnableClock(_h, nClock);
      if (dal_result != DAL_SUCCESS)
      {
        HSU_ERR_FATAL("DalClock_EnableClock failed: result(0x%x), handle(0x%x), clock(0x%x)", dal_result, _h, nClock);
      }
    }
    break;
  }
}

//==============================================================================
// FUNCTION hsu_DalClock_DisableClock
//==============================================================================
static void hsu_DalClock_DisableClock(DalDeviceHandle * _h,  ClockIdType  nClock)
{
  DALResult dal_result;
  hsu_platform_info_type const * platform_info = hsu_platform_get_info();

  switch (platform_info->hsu_platform_id)
  {
    case HSU_PLATFORM_RUMI:
    break;

    default:
    {
      dal_result = DalClock_DisableClock(_h, nClock);
      if (dal_result != DAL_SUCCESS)
      {
        HSU_ERR_FATAL("DalClock_DisableClock failed: result(0x%x), handle(0x%x), clock(0x%x)", dal_result, _h, nClock);
      }
    }
    break;
  }
}

//==============================================================================
// FUNCTION hsu_DalClock_GetClockId
//==============================================================================
static void hsu_DalClock_GetClockId(DalDeviceHandle * _h,  const char * szClock,  ClockIdType * pnId)
{
  DALResult dal_result;

  dal_result = DalClock_GetClockId(_h, szClock, pnId);
  if (dal_result != DAL_SUCCESS)
  {
    HSU_ERR_FATAL("DalClock_GetClockId failed: result(0x%x), handle(0x%x), clock_sz(0x%x)", dal_result, _h, szClock);
  }
} 

//==============================================================================
// FUNCTION hsu_npa_issue_required_request
//==============================================================================
static void hsu_npa_issue_required_request(npa_client_handle client, npa_resource_state state)
{
  hsu_platform_info_type const * platform_info = hsu_platform_get_info();

  switch (platform_info->hsu_platform_id)
  {
    case HSU_PLATFORM_RUMI:
    break;

    default:
    {
      HSU_ULOG_1(NO_MSG, BUS_LOG, "start, client 0x%08X", (uint32)client);
      npa_issue_required_request(client, state);
      HSU_ULOG_1(NO_MSG, BUS_LOG, "end, state %u", state);
    }
    break;
  }
}

//==============================================================================
// FUNCTION hsu_npa_issue_complete_request
//==============================================================================
static void hsu_npa_complete_request(npa_client_handle client)
{
  hsu_platform_info_type const * platform_info = hsu_platform_get_info();

  switch (platform_info->hsu_platform_id)
  {
    case HSU_PLATFORM_RUMI:
    break;

    default:
    {
      HSU_ULOG_1(NO_MSG, BUS_LOG, "start, client 0x%08X", (uint32)client);
      npa_complete_request(client);
      HSU_ULOG(NO_MSG, BUS_LOG, "end");
    }
    break;
  }
}

//==============================================================================
// FUNCTION hsu_npa_create_sync_client
//==============================================================================
static npa_client_handle hsu_npa_create_sync_client( const char       *resource_name, 
                                             const char       *client_name,
                                             npa_client_type   client_type)
{
  npa_client_handle ret = NULL;
  hsu_platform_info_type const * platform_info = hsu_platform_get_info();

  switch (platform_info->hsu_platform_id)
  {
    case HSU_PLATFORM_RUMI:
    break;

    default:
    {
      ret = npa_create_sync_client(resource_name, client_name, client_type);
    }
    break;
  }
  return ret;
}

//==============================================================================
// FUNCTION hsu_clkmgr_select_utmi_clk
//==============================================================================
void hsu_clkmgr_select_utmi_clk(void)
{

  HSU_ULOG_FUNC(BUS_LOG);
  // If operating without SS PHY, follow this sequence to disable 
  // pipe clock requirement
  HWIO_GENERAL_CFG_OUTM(HWIO_GENERAL_CFG_PIPE_UTMI_CLK_DIS_BMSK,
    0x1 << HWIO_GENERAL_CFG_PIPE_UTMI_CLK_DIS_SHFT);

  hsu_os_delay_us(100);
  
  HWIO_GENERAL_CFG_OUTM(HWIO_GENERAL_CFG_PIPE_UTMI_CLK_SEL_BMSK,
    0x1 << HWIO_GENERAL_CFG_PIPE_UTMI_CLK_SEL_SHFT);
  
  hsu_os_delay_us(100);
  
  HWIO_GENERAL_CFG_OUTM(HWIO_GENERAL_CFG_PIPE3_PHYSTATUS_SW_BMSK,
    0x1 << HWIO_GENERAL_CFG_PIPE3_PHYSTATUS_SW_SHFT);

  hsu_os_delay_us(100);  
  
  HWIO_GENERAL_CFG_OUTM(HWIO_GENERAL_CFG_PIPE_UTMI_CLK_DIS_BMSK,
    0x0 << HWIO_GENERAL_CFG_PIPE_UTMI_CLK_DIS_SHFT);

#if 0 // No SS PHY clocks in Twizy
  if (FALSE == hsu_clkmgr.is_utmi_path_only)
  {
    hsu_clkmgr.is_utmi_path_only = TRUE;

    // Disable Super Speed Clocks from this point since we are running 
    // in UTMI HS ONLY path until system reset
    hsu_DalClock_DisableClock(hsu_clkmgr.phClock, hsu_clkmgr.gcc_usb3_phy_aux_clk);
    hsu_DalClock_DisableClock(hsu_clkmgr.phClock, hsu_clkmgr.gcc_usb30_sleep_clk);
    hsu_DalClock_DisableClock(hsu_clkmgr.phClock, hsu_clkmgr.gcc_usb3_phy_pipe_clk);
  }
#endif
}


//==============================================================================
// FUNCTION hsu_clkmgr_is_hs_only_mode
//==============================================================================
boolean hsu_clkmgr_is_hs_only_mode(void)
{
  return TRUE;
}

