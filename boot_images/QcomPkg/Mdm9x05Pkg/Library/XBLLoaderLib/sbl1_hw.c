/*=============================================================================

                         SBL1 Hardware Initialization

GENERAL DESCRIPTION
  This file does basic hardware initialization at power up.

Copyright 2011-2018, 2020 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.xf/0.2/QcomPkg/Mdm9x05Pkg/Library/XBLLoaderLib/sbl1_hw.c#11 $

when       who       what, where, why
--------   ---       ----------------------------------------------------------
05/01/20   rrawat    Configured gpio_20 to high
12/11/18   aus       Added support for interrupt handling
04/09/18   sv        Added sbl1_pm_qsee_pre_init_wrapper
08/03/17   sj        added dead battery charging logic in ramdump case also
10/18/16   ds        do pm_device_init in dload mode
07/15/16   yps       Remove Sec lib
06/16/16   qbz       Added DDI support
06/02/16   rp		 Load training data from flash directly into DDR_STRUCT instead of using an intermediate buffer
05/04/16   rp		 Rename ddr training APIs
05/03/16   rp		 Move ddr_remapper before training
02/18/16   aab       Added a call to boot_pm_system_init();
11/16/15   plc       Make PIMEM noncacheable for now
10/22/15   plc       Replaced tlb invalidate with generic EL-agnostic call
10/13/15   plc       Sync some changes from 8996 tip
10/07/15   plc       Changes to enable DDR on RUMI, and skip DDR on Virtio
08/25/15   plc       Bring up to date with 8996 tip
08/07/15   plc       Stub out Tsens temperature check for 8998 presi virtio
07/23/15   sds       Call boot_ICB_Config_PostInit() after training.
07/15/15   aab       Moved pm_sbl_chg_init() call right after pm_driver_init(). This API is used to do weak battery charging.
05/31/15   aab       Replace pm_oem_init() with pm_sbl_chg_init()
04/20/15   aks       Added support for Voltage Sensors
03/27/15   kpa       Added boot_pm_device_programmer_init
03/04/15   kpa       Reduce DDR freq to 200Mhz for Deviceprogrammer
03/04/15   kpa       Do not initialize pImem during dload mode boot, reset GCC_RESET_STATUS
02/20/15   kpa       Update boot logs for pmic driver
02/18/15   kpa       Rename sbl1_hw_pmic_init to sbl1_hw_pre_ddr_init. Add railway driver.
02/11/15   kpa       Enable Pimem.
02/03/15   wek       Skip ddr training for device programmer.
01/27/15   kpa       Added sbl1_ddr_set_default_params
01/27/15   jjo       Enable tsens init.
01/05/15   kpa       Temporarily stub out pimem and tsens init.
10/14/14   kpa       Move pm_device_init into sbl1_hw_pmic_init() for pmic
                     image loading support
05/07/14   kedara    Branch for 8996 and include HALbootHWIO.h instead of msmhwioreg.h
03/21/14   ck        Added logic to save reset_status_register to imem cookies before clearing
03/18/14   ck        Updated boot_hw_reset calls as they now take a reset type parameter
03/14/14   ck        Moved boot_busywait_init to sbl1_main_ctl as it needs to be done before boot logger init
03/03/14   ck        Removing SpmiInit and SpmiCfg from sbl1_hw_init per SPMI team as PMIC driver does this now
02/18/14   ck        Added logic to sbl1_hw_init to clear PMIC_ABNORMAL_RESIN_STATUS
10/17/13   ck        Temp zeroing of ddr training memory location until ddr team delivers
08/07/13   sr        Removed boot_ddr_debug call .
07/26/13   rj        Calling i2C init to fix eeprom init issue
07/11/13   rj        Calling boot_pre_ddr_clock_init after CDT configuration is
                     done to dynamically detect ddr and setup the clock plan
06/18/13   sl        Call boot_ddr_debug()
06/06/13   dh        Backup gcc reset status then clear it
06/14/13   dh        Only load ddr training data if sbl is not in dload mode
05/21/13   yp        Turn on vibrator when device power up in normal mode.
04/04/13   dh        Move boot_DALSYS_InitMod to early sbl1_main_ctl
04/02/13   dh        Use boot_dload_is_dload_mode_set instead of uefi dump cookie
                     to determin if device is in dload mode
04/11/13   sl        Relocate boot_ddr_test() call
04/03/13   sl        Call Clock API to get DDR speed
03/21/13   dh        Add memory barrier in boot_ddr_post_init
03/07/12   jz        Cleanup logging
12/13/12   jz        Change clock frequency back to MAX after DDR training is done
12/04/12   dh        Move boot_Tsens_Init to sbl1_hw_init
11/26/12   dh        Skip DDR training if UEFI dload cookie is set
11/12/12   sl        Enable DDR Test Framework
11/05/12   dh        Move thermal management code to common file
10/09/12   dh        Add boot_ICB_Segments_Init right after ddr init to configure
                     bimc slave address range
                     Add boot_SpmiCfg_Init before boot_SpmiBus_Init
10/09/12   dh        Only enable fast debug feature if FEATURE_BOOT_FAST_DEBUG
                     is defined
09/26/12   jz        Compile out boot_debug_mode_enter for Virtio testing
09/25/12   dh        Added pm_driver_init to allow PMIC API usage and pm_oem_init
                   to allow customer to call PMIC API in boot for
                   their desirable PMIC configurations.
08/30/12   dh        Add boot log for clock and pmic functions
08/16/12   AJC       Added boot_debug_mode_enter for FASTDEBUG feature
08/01/12   dh        Add sbl1_hw_deinit
07/23/12   dh        Add sbl1_set_ddr_training_data and sbl1_wait_for_ddr_training
07/16/12   dh        Move spmi and pmic init to sbl1_hw_init
06/18/12   dh        Switch to boot external driver api for tsensor and bus API
06/11/12   dh        Add sbl1_check_device_temp, check temp at the beginning of
                     sbl1_hw_init
06/08/12   dh        Add ICB_Config_Init in sbl1_hw_init
05/29/12   dh        Move boot_clock_init to sbl1_hw_init_secondary
                     Add boot_busywait_init in sbl1_hw_init
                     Remove gpio init from sbl1_hw_init_secondary since it must be
                     called after smem is available.
                     Rename sbl1_clk_regime_ram_dump_init to sbl1_hw_dload_init
05/08/12   dh        Add clock init api
10/18/11   dh        Initial revision
=============================================================================*/

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_comdef.h"
#include "boot_sbl_if.h"
#include "sbl1_hw.h"
#include "sbl1_mc.h"
#include "boothw_target.h"
#include "boot_dload.h"
#include "boot_logger.h"
#include "boot_config_data.h"
#include "boot_util.h"
#include "boot_thermal_management.h"
#include "boot_extern_clk_interface.h"
#include "boot_extern_ddr_interface.h"
#include "boot_extern_pmic_interface.h"
#include "boot_extern_dal_interface.h"
#include "boot_extern_tlmm_interface.h"
#include "boot_extern_bus_interface.h"
#include "boot_extern_tsensor_interface.h"
#include "boot_extern_pimem_interface.h"
#include "boot_extern_power_interface.h"
#include "boot_extern_vsense_interface.h"
#include "boot_extern_bam_interface.h"
#include "boot_shared_imem_cookie.h"
#include "string.h"
#include "boot_cache_mmu.h"
#include "HALbootHWIO.h"
#include "boot_profiler.h"
#include "boot_page_table_armv7.h"
#include "ddr_drivers.h"
#include "icb_sdi.h"
#include "boot_visual_indication.h"
#include "boot_fastcall_tz.h"
#include "boot_extern_ddi_api_interface.h"
#include "boot_extern_qusb_ldr_interface.h"
#include "InterruptBoot.h"
#include "DDITlmm.h"
/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

/* Writing a 1 clears corresponding bit field */
#define GCC_RESET_STATUS_CLEAR_VAL 0x0

/* backed up GCC_RESET_STATUS for ramdump */
static uint32 reset_status_register = 0;

/*=============================================================================

                              FUNCTION DEFINITIONS

=============================================================================*/

/*===========================================================================

**  Function :  sbl1_ddr_init

** ==========================================================================
*/
/*!
*
* @brief
*   Initialize DDR device.
* *
* @par Dependencies
*  Following Api's need to be called first:
*    sbl1_hw_init : To have ddr clocks setup.
*    sbl1_ddr_set_params : To have DDR config parameters setup.
*
* @retval
*   None
*
* @par Side Effects
*   None
*
*/
void sbl1_ddr_init(bl_shared_data_type *bl_shared_data)
{
  ddr_info sbl1_ddr_info;
#ifndef FEATURE_RUMI_BOOT
  pm_err_flag_type pm_driver_init;
#endif
  icbcfg_error_type icb_config_post_init;
  icbcfg_remap_info_type remap = {0x0, 0x20000000, 0x60000000, ICBCFG_REMAP_INTERLEAVE_DEFAULT, FALSE};
  /* Initialize DDR */
#ifdef FEATURE_RUMI_BOOT
  boot_ddr_initialize_device(19200);
#else
  boot_ddr_initialize_device(boot_clock_get_ddr_speed());
#endif

  /* Configure DDR slave segments address range */
  sbl1_ddr_info = boot_ddr_get_info();
  boot_ICB_Segments_Init( "/dev/icbcfg/boot", &sbl1_ddr_info);
	 
 	/* Incase of dual channel + dual rank + 768 MB per each rank.
 	Base address to be aligned with size at BIMC and so remapping at buses to fit into memory map
 	Address 0x0 is being mapped to 0x6000 0000 for a block of 0.5GB
 	*/
 	if (sbl1_ddr_info.ddr_size.sdram0_cs0_addr == 0x0)
 	{
     if((sbl1_ddr_info.ddr_size.sdram0_cs0 == 768) && (sbl1_ddr_info.ddr_size.sdram0_cs1 == 768)
        && (sbl1_ddr_info.ddr_size.sdram1_cs0 == 768) &&  (sbl1_ddr_info.ddr_size.sdram1_cs1 == 768))
     {
 	    boot_ICB_Remap("/dev/icbcfg/boot", &sbl1_ddr_info, &remap);
     }
     else if ((sbl1_ddr_info.ddr_size.sdram0_cs0 == 1536) && (sbl1_ddr_info.ddr_size.sdram0_cs1 == 1536)
              && (sbl1_ddr_info.ddr_size.sdram1_cs0 == 0) &&  (sbl1_ddr_info.ddr_size.sdram1_cs1 == 0 )) {
       boot_ICB_Remap("/dev/icbcfg/boot", &sbl1_ddr_info, &remap);
     }
 	}

#ifndef FEATURE_RUMI_BOOT
  if(boot_dload_is_dload_mode_set() == FALSE)
  {
   /* perform sanity ddr test */
   boot_log_message("do ddr sanity test, Start");
   boot_log_start_timer();
  // boot_ddr_test(boot_clock_get_ddr_speed()); Need to look into
   boot_log_stop_timer("do ddr sanity test, Delta");
  }
#endif

  /* PMIC driver must be initialized after DDR init for dead/weak battery charging
     and before ddr training as its uses PMIC API's */
   /*compile out pmic and clock api for rumi */
#if (!defined(FEATURE_RUMI_BOOT))
  /* PMIC image not loaded for Device Programmer */

  if (bl_shared_data->build_type != SBL_BUILD_DEVICEPROGRAMMER_DDR) {
    /* Initialize PMIC Driver - Allowing API usage */
  
  boot_log_message("pm_driver_init, Start");
  boot_log_start_timer(); 
  
  /* Initialize PMIC Driver - Allowing API usage */
  BL_VERIFY((pm_driver_init=boot_pm_driver_init()) == PM_ERR_FLAG__SUCCESS, (uint16)pm_driver_init|BL_ERROR_GROUP_PMIC);
  
  boot_log_stop_timer("pm_driver_init, Delta"); 

  
  boot_log_message("vsense_init, Start");
  boot_log_start_timer();
  
  /* calibrate the voltage sensors */
  BL_VERIFY((boot_vsense_init() == TRUE), BL_ERROR_GROUP_PMIC);

  boot_log_stop_timer("vsense_init, Delta"); 

 
  }  
#endif /* FEATURE_RUMI_BOOT*/

//Open the TZ and DEVCFG regions before ddr_test
boot_qsee_unlock_region();

#ifndef FEATURE_RUMI_BOOT
  if (boot_ddr_params_is_training_on_rpm_required() == FALSE &&
      boot_dload_is_dload_mode_set() == FALSE && (bl_shared_data->build_type != SBL_BUILD_DEVICEPROGRAMMER_DDR)
	  && (bl_shared_data->build_type != SBL_BUILD_DDRDEBUGIMAGE))
  {
    boot_ddr_do_ddr_training();    
   boot_ddr_test(boot_clock_get_ddr_speed());
  }
#endif

  if (bl_shared_data->build_type == SBL_BUILD_DEVICEPROGRAMMER_DDR) {
  /* Add the DDR heap to DAL */
  //boot_DALSYS_HeapInit(boot_external_heap, BOOT_EXTERNAL_HEAP_SIZE, TRUE); ****Need to fix****
  }

  /*ICB post config api will configure any settings done after DDR/memmap is finalized. */
  BL_VERIFY((icb_config_post_init=boot_ICB_Config_PostInit("/dev/icbcfg/boot")) == ICBCFG_SUCCESS,(uint16)icb_config_post_init|BL_ERROR_GROUP_ICB);
  boot_ddr_post_init();
} /* sbl1_ddr_init() */

#ifndef BOOT_PRE_SILICON
#ifdef  FEATURE_BOOT_FAST_DEBUG
/*===========================================================================

**  Function :  sbl1_enter_fastdebug

** ==========================================================================
*/
/*!
*
* @brief
*   Enters debug mode if PMIC GPIO key is pressed while booting up
*
* @param[in] bl_shared_data Pointer to shared data
*
*
* @retval
*   None
*
* @par Side Effects
*   None
*
*/
static volatile boolean enter_fastdebug = FALSE;
static void  boot_debug_mode_enter (void)
{

  uint8 data;
  uint32 addr, readlen;
  SpmiBus_ResultType spmi_write;

  // On 8974, GPIO4 is used for the focus key
  // Use a pull up value of 30uA and a voltage source of VIN0

    data = 0x2;
  addr = 0xC341;
  BL_VERIFY((spmi_write=boot_SpmiBus_WriteLong(0, (SpmiBus_AccessPriorityType)0, addr, &data, 1))==SPMI_BUS_SUCCESS), (uint16)spmi_write|BL_ERROR_GROUP_SPMI);

    data = 0x0;
    addr = 0xC340;
  BL_VERIFY((spmi_write=boot_SpmiBus_WriteLong(0, (SpmiBus_AccessPriorityType)0, addr, &data, 1))==SPMI_BUS_SUCCESS), (uint16)spmi_write|BL_ERROR_GROUP_SPMI);


  // Slave & Priority  = 0
  data = 0x0;
  addr = 0xC342;
  BL_VERIFY((spmi_write=boot_SpmiBus_WriteLong(0, (SpmiBus_AccessPriorityType)0, addr, &data, 1))==SPMI_BUS_SUCCESS), (uint16)spmi_write|BL_ERROR_GROUP_SPMI);

  // Then enable the GPIO
  data = 0x80;
  addr = 0xC346;
  BL_VERIFY((spmi_write=boot_SpmiBus_WriteLong(0, (SpmiBus_AccessPriorityType)0, addr, &data, 1))==SPMI_BUS_SUCCESS), (uint16)spmi_write|BL_ERROR_GROUP_SPMI);

  // Now poll the value
  addr = 0xC308;
  if (SPMI_BUS_SUCCESS == boot_SpmiBus_ReadLong(0, (SpmiBus_AccessPriorityType)0, addr, &data, 1, &readlen))
  {
    // If GPIO is pressed, bit 0 will be 0 (active low)
    if (!(data & 0x1))
    {
      enter_fastdebug = TRUE;
      while(enter_fastdebug);
    }
  }
}
#endif
#endif


/*===========================================================================

**  Function :  sbl1_hw_init

** ==========================================================================
*/
/*!
*
* @brief
*   This function performs hardware initialization.
*
*   Only common hardware that is needed for flash
*   initialization is initialized here.
*
* @param[in] bl_shared_data Pointer to shared data
*
* @par Dependencies
*   None
*
* @retval
*   None
*
* @par Side Effects
*   None
*
*/
void sbl1_hw_init()
{
  TsensResultType status;
  
 #ifndef SAHARA_OVER_UART 
  /* Configure GPIO 20 */
   DALGpioSignalType gpio_cfg = 0;
   gpio_cfg = (DALGpioSignalType)DAL_GPIO_CFG_OUT(20, 0, 
                                                 DAL_GPIO_OUTPUT, 
												 DAL_GPIO_PULL_UP, 
												 DAL_GPIO_2MA, 
												 DAL_GPIO_HIGH_VALUE);
   Tlmm_ConfigGpioGroup(DAL_TLMM_GPIO_ENABLE, &gpio_cfg, 1);
 #endif 
 
  /* Initialize BAM interface */
  boot_bam_drv_init();

#ifndef BOOT_PRE_SILICON
#ifdef FEATURE_BOOT_FAST_DEBUG
  /* Check if we are going to enter debug mode */
  boot_debug_mode_enter();
#endif
#endif

  /* Initialize temperature sensor */
  status = boot_Tsens_Init();
  BL_VERIFY(status == TSENS_SUCCESS,(uint16)status|BL_ERROR_GROUP_TSENS);

  /* Check the temperature */
//plc  boot_check_device_temp();

if(PcdGet32 (PcdBuildType) != SBL_BUILD_DDRDEBUGIMAGE)
{
  /* Put High Speed QUSB2 PHY in non-drive mode. */
  boot_qusb_ldr_utils_hs_phy_nondrive_mode_set();
}
  HWIO_OUT(MPM2_MPM_CONTROL_CNTCR, 0x1);

#if (!defined(FEATURE_RUMI_BOOT))
  /* Calling here to ensure eeprom init goes fine for CDT read */
  boot_pre_i2c_clock_init();

#endif /*FEATURE_RUMI_BOOT*/

  /* Interrupt Initialization */							 
   InterruptInitialize(); 
  
} /* sbl1_hw_init() */


/*===========================================================================

**  Function :  sbl1_hw_deinit

** ==========================================================================
*/
/*!
*
* @brief
*   This function deinit the hardware that's not needed beyond sbl1
*
* @param[in] bl_shared_data Pointer to shared data
*
*
* @retval
*   None
*
* @par Side Effects
*   None
*
*/
void sbl1_hw_deinit()
{

  /* Call clock exit boot to disable unneeded clock*/
  BL_VERIFY(boot_clock_exit_boot() == TRUE,BL_ERR_DISABLE_CLOCK_FAIL|BL_ERROR_GROUP_BOOT);
  
  /* Interrupt de-initialize */
   InterruptExitBootServices();
  
} /* sbl1_hw_deinit() */


/*===========================================================================

**  Function :  sbl1_hw_dload_init

** ==========================================================================
*/
/*!
*
* @brief
*   This function initializes the necessary clocks for dload
*   We only do so when auth is disabled
*
* @param[in] bl_shared_data Pointer to shared data
*
* @par Dependencies
*   None
*
* @retval
*   None
*
* @par Side Effects
*   None
*
*/
void sbl1_hw_dload_init(bl_shared_data_type *bl_shared_data)
{
  boot_dload_clock_init();
}


/*===========================================================================

**  Function :  sbl1_hw_init_secondary

** ==========================================================================
*/
/*!
*
* @brief
*   This function performs secondary hardware initialization.
*
*   DDR is initialized here. This needs to be done after trustzone is loaded.
*
* @param[in] bl_shared_data Pointer to shared data
*
* @par Dependencies
*   None
*
* @retval
*   None
*
* @par Side Effects
*   None
*
*/
void sbl1_hw_init_secondary( bl_shared_data_type *bl_shared_data )
{
  /*compile out pmic and clock api for rumi */
#if (!defined(FEATURE_RUMI_BOOT))

  /* First save gcc reset status to shared IMEM, then clear it */
  reset_status_register = HWIO_IN(GCC_RESET_STATUS);

  if (boot_shared_imem_cookie_ptr)
  {
    boot_shared_imem_cookie_ptr->reset_status_register = reset_status_register;
  }

 //In VT, power on RESET STATUS should be zero
  HWIO_OUT(GCC_RESET_STATUS,GCC_RESET_STATUS_CLEAR_VAL);

  /* note for future, this needs to happen post ddr set param and set default param
   otherwise we will not be able to detect if we are in 2nd pass or not
   */
 //todo HWIO_OUTF(GCC_RESET_FSM_CTRL, FIRST_PASS_COMPLETE, 0x0);

  boot_log_message("clock_init, Start");
  boot_log_start_timer();

  BL_VERIFY((boot_clock_init() == TRUE), BL_ERR_CPU_CLK_INIT_FAIL|BL_ERROR_GROUP_BOOT);

  boot_log_stop_timer("clock_init, Delta");

#endif /*FEATURE_RUMI_BOOT*/

} /* sbl1_hw_init_secondary() */

/*===========================================================================

**  Function :  sbl1_hw_pre_ddr_init

** ==========================================================================
*/
/*!
*
* @brief
*  Initialize PMIC and railway driver. API used for hw initialization that
*  must occur before DDR init.
*
* @param[in] bl_shared_data Pointer to shared data
*
* @par Dependencies
*    This Api needs to be called :
*  Before: sbl1_ddr_init
*
*
* @retval
*   None
*
* @par Side Effects
*   None
*
*/
void sbl1_hw_pre_ddr_init( bl_shared_data_type *bl_shared_data )
{
   pm_err_flag_type pm_device_init, pm_driver_init, pm_sbl_chg_init,pm_devprog_init;

   if ( bl_shared_data->build_type == SBL_BUILD_SBL ||
        bl_shared_data->build_type == SBL_BUILD_DDRDEBUGIMAGE )
   {
    boot_log_message("pm_device_init, Start");
    boot_log_start_timer();
    if (bl_shared_data->build_type == SBL_BUILD_DDRDEBUGIMAGE)
    {
      boot_pm_set_psi_address((uint64)SCL_PMIC_CONFIG_BUF_BASE_DDI);
    }
    /* Initialize the pmic */
    BL_VERIFY((pm_device_init=boot_pm_device_init()) == PM_ERR_FLAG__SUCCESS, (uint16)pm_device_init|BL_ERROR_GROUP_PMIC);
    boot_log_stop_timer("pm_device_init, Delta");

    boot_log_message("pm_driver_init, Start");
    boot_log_start_timer();
    BL_VERIFY((pm_driver_init=boot_pm_driver_init()) == PM_ERR_FLAG__SUCCESS, (uint16)pm_driver_init|BL_ERROR_GROUP_PMIC);
    boot_log_stop_timer("pm_driver_init, Delta");

    boot_log_message("pm_sbl_chg_init, Start");
    boot_log_start_timer();
    BL_VERIFY((pm_sbl_chg_init=boot_pm_sbl_chg_init()) == PM_ERR_FLAG__SUCCESS, (uint16)pm_sbl_chg_init|BL_ERROR_GROUP_PMIC);
    boot_log_stop_timer("pm_sbl_chg_init, Delta");

    boot_log_message("vsense_init, Start");
    boot_log_start_timer();

    /* calibrate the voltage sensors */
    BL_VERIFY((boot_vsense_init() == TRUE), BL_ERR_VSENS_INIT_FAIL|BL_ERROR_GROUP_BOOT);

    boot_log_stop_timer("vsense_init, Delta");

    /*Initialize railway driver */
    boot_railway_init();

    /* Call CPR init to settle the voltages to the recommended levels*/
    boot_cpr_init();

    /*pmic post initialization*/
    boot_pm_system_init();

  }
#if 0
  else if (bl_shared_data->build_type == SBL_BUILD_DDRDEBUGIMAGE)
  {
    boot_log_message("DDI_Init, Start");

    boot_log_message("pm_device_init, Start");
    boot_log_start_timer();
    /* Initialize the pmic */
    BL_VERIFY((pm_device_init=boot_pm_device_init()) == PM_ERR_FLAG__SUCCESS, (uint16)pm_device_init|BL_ERROR_GROUP_PMIC);
    boot_log_stop_timer("pm_device_init, Delta");

    boot_log_message("pm_driver_init, Start");
    boot_log_start_timer();
    BL_VERIFY((pm_driver_init=boot_pm_driver_init()) == PM_ERR_FLAG__SUCCESS, (uint16)pm_driver_init|BL_ERROR_GROUP_PMIC);
    boot_log_stop_timer("pm_driver_init, Delta");


    boot_log_message("vsense_init, Start");
    boot_log_start_timer();

    /* calibrate the voltage sensors */
    BL_VERIFY((boot_vsense_init() == TRUE), BL_ERR_VSENS_INIT_FAIL|BL_ERROR_GROUP_BOOT);

    boot_log_stop_timer("vsense_init, Delta");

    /*Initialize railway driver */
    boot_railway_init();

    /* Call CPR init to settle the voltages to the recommended levels*/
    boot_cpr_init();
    boot_log_stop_timer("DDI_Init, Delta");

  }
#endif
  else
  {
    boot_log_message("pm_device_programmer_init, Start");
    boot_log_start_timer();

    /* Initialize the pmic */
    BL_VERIFY(((pm_devprog_init=boot_pm_device_programmer_init()) == PM_ERR_FLAG__SUCCESS), (uint16)pm_devprog_init|BL_ERROR_GROUP_PMIC);

    boot_log_stop_timer("pm_device_programmer_init, Delta");
  }

}

/*===========================================================================

**  Function :  sbl1_hw_get_reset_status

** ==========================================================================
*/
/*!
*
* @brief
*   This function returns the value of reset status register saved from this boot
*
* @par Dependencies
*   None
*
* @retval
*   None
*
* @par Side Effects
*   None
*
*/
uint32 sbl1_hw_get_reset_status( void )
{
   return reset_status_register;
}  /* sbl1_hw_get_reset_status */

/*===========================================================================

**  Function :  sbl1_ddr_set_params

** ==========================================================================
*/
/*!
*
* @brief
*   Set DDR configuration parameters
*
* @param[in] bl_shared_data Pointer to shared data
*
* @par Dependencies
*    This Api needs to be called :
*  Before: sbl1_ddr_init
*  After: boot_config_data_table_init
*
* @retval
*   None
*
* @par Side Effects
*   None
*
*/
void sbl1_ddr_set_params(  bl_shared_data_type *bl_shared_data  )
{
  uint32 ddr_cdt_params_size = 0;
  struct cdt_info *config_data_table_info;
  uint8 *ddr_cdt_params_ptr;
  ddr_info ddr_type_info;
  icbcfg_error_type icb_config_Init;

  /*Initialize param only during cold boot, for download mode preserve the content */
  if(sbl1_get_presi_platform_type() == presi_rumi)
  {
    /* If a DDR CDB was found in flashed CDT, overwrite the previously shared
       default compiled DDR CDT configurations with DDR driver*/
    if (boot_cdt_contains_ddr_cdb())
    {
      boot_log_message("sbl1_ddr_set_params, Start");
      boot_log_start_timer();

      /* Extract ddr data block from configuration data table(CDT) */
      config_data_table_info =
                        bl_shared_data->sbl_shared_data->config_data_table_info;

      ddr_cdt_params_ptr =
                        boot_get_config_data_block(config_data_table_info->cdt_ptr,
                                                 CONFIG_DATA_BLOCK_INDEX_V1_DDR,
                                                 &ddr_cdt_params_size);

      /* Make sure cdt param size is valid */
      BL_VERIFY((ddr_cdt_params_size <= CONFIG_DATA_TABLE_MAX_SIZE) &&
            (ddr_cdt_params_ptr != NULL),  BL_ERR_CDT_PARAMS_SIZE_INVALID|BL_ERROR_GROUP_BOOT );

      /* Copy CDT DDR paramters to shared IMEM */
      qmemcpy((void*)SHARED_IMEM_BOOT_CDT_BASE,
              ddr_cdt_params_ptr,
              ddr_cdt_params_size);


      /* Pass cdt parameter to DDR driver */
      BL_VERIFY(boot_ddr_set_params((void*)SHARED_IMEM_BOOT_CDT_BASE, ddr_cdt_params_size) == TRUE, BL_ERR_DDR_SET_PARAMS_FAIL|BL_ERROR_GROUP_DDR);
    }

  }

  /*Initialize param only during cold boot, for download mode preserve the content */
  if(sbl1_get_presi_platform_type() == presi_rumi)
  {
    /* Get the ddr type by reading the cdt */
    ddr_type_info = boot_ddr_get_info();

    boot_log_message("Pre_DDR_clock_init, Start");
    boot_log_start_timer();

  /* Initialize the pre ddr clock */
  BL_VERIFY((boot_pre_ddr_clock_init(ddr_type_info.ddr_type) == TRUE), BOOT_PRE_DDR_CLK_INIT_FAIL|BL_ERROR_GROUP_BOOT);

    boot_log_stop_timer("Pre_DDR_clock_init, Delta");

  /*ICB config api will configure address range for NOCs */
  BL_VERIFY((icb_config_Init=boot_ICB_Config_Init("/dev/icbcfg/boot")) == ICBCFG_SUCCESS, (uint16)icb_config_Init|BL_ERROR_GROUP_ICB);

    boot_log_stop_timer("sbl1_ddr_set_params, Delta");
  }
} /* sbl1_ddr_set_params() */

/*===========================================================================

**  Function :  sbl1_ddr_set_default_params

** ==========================================================================
*/
/*!
*
* @brief
*   Set DDR configuration parameters
*
* @param[in] bl_shared_data Pointer to shared data
*
* @par Dependencies
*    This Api needs to be called :
*  Before: sbl1_ddr_init, boot_config_data_table_init
*  After:
*
* @retval
*   None
*
* @par Side Effects
*   None
*
*/
void sbl1_ddr_set_default_params(  bl_shared_data_type *bl_shared_data  )
{
  uint32 ddr_cdt_params_size = 0;
  struct cdt_info *config_data_table_info;
  uint8 *ddr_cdt_params_ptr;

  boot_log_message("sbl1_ddr_set_default_params, Start");
  boot_log_start_timer();

  if(sbl1_get_presi_platform_type() == presi_rumi)
  {
    /* Extract ddr data block from configuration data table(CDT) */
    config_data_table_info =
                      bl_shared_data->sbl_shared_data->config_data_table_info;

    ddr_cdt_params_ptr =
                      boot_get_config_data_block(config_data_table_info->cdt_ptr,
                                                 CONFIG_DATA_BLOCK_INDEX_V1_DDR,
                                                 &ddr_cdt_params_size);

    /* Make sure cdt param size is valid */
    BL_VERIFY((ddr_cdt_params_size <= CONFIG_DATA_TABLE_MAX_SIZE) &&
            (ddr_cdt_params_ptr != NULL),  BL_ERR_CDT_PARAMS_SIZE_INVALID|BL_ERROR_GROUP_BOOT );

      /* Copy CDT DDR parameters to shared IMEM */
    qmemscpy( (void*)SHARED_IMEM_BOOT_CDT_BASE,
        SHARED_IMEM_BOOT_CDT_SIZE,
        ddr_cdt_params_ptr,
        ddr_cdt_params_size );

    /* Pass cdt parameter to DDR driver */
    BL_VERIFY(boot_ddr_set_params((void*)SHARED_IMEM_BOOT_CDT_BASE, ddr_cdt_params_size) == TRUE, BL_ERR_DDR_SET_PARAMS_FAIL|BL_ERROR_GROUP_DDR);
  }
  boot_log_stop_timer("sbl1_ddr_set_default_params, Delta");

} /* sbl1_ddr_set_default_params() */


/*===========================================================================

**  Function :  sbl1_wait_for_ddr_training

** ==========================================================================
*/
/*!
*
* @brief
*   Wait for RPM to execute DDR training sequence.
*   If ddr training parameter data is updated, save it to storage device.
*
* @param[in]
*   bl_shared_data Pointer to shared data
*
* @par Dependencies
*   Must be called after sbl1_ddr_init
*
* @retval
*   None
*
* @par Side Effects
*   None
*
*/
void sbl1_wait_for_ddr_training( bl_shared_data_type *bl_shared_data )
{
  uint32 ddr_params_training_data_size = 0;
  void* ddr_training_data_ptr = NULL;

  /* Check if DDR parameter training is required */
  if (boot_ddr_params_is_training_required() == FALSE)
  {
    /* Only perform DDR training if SBL is not in dload mode */
    if(!boot_dload_is_dload_mode_set())
    {
      boot_log_message("sbl1_wait_for_ddr_training, Start");
      boot_log_start_timer();

      memory_barrier();

      /* If boot_ddr_post_init returns TRUE then ddr training data is updated,
        we need to save it to storage device */
      if(boot_ddr_post_init() == TRUE)
      {
        memory_barrier();

        ddr_training_data_ptr =
            boot_ddr_params_get_training_data(&ddr_params_training_data_size);

        BL_VERIFY((ddr_training_data_ptr != NULL) &&
                  (ddr_params_training_data_size != 0),
                  BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT);

        /* After training data is saved, instruct Qsee to perform a reset */
        bl_shared_data->sbl_qsee_interface.reset_required =
                                                (uint32) RESET_DEVICE_BY_QSEE;
      }

      /* Speed back up to max frequency */
      boot_clock_set_cpu_perf_level(CLOCK_BOOT_PERF_TURBO);
      boot_clock_set_L2_perf_level(CLOCK_BOOT_PERF_TURBO);

      boot_ddr_test(0);

      boot_log_stop_timer("sbl1_wait_for_ddr_training, Delta");
    }
  }
}

/*===========================================================================

**  Function :  sbl1_hw_play_vibr

** ==========================================================================
*/
/*!
*
* @brief
*   This function calls PMIC API to play power on vibration
*
* @param[in] bl_shared_data Pointer to shared data
*
* @par Dependencies
*   None
*
* @retval
*   None
*
* @par Side Effects
*   None
*
*/
void sbl1_hw_play_vibr(bl_shared_data_type *bl_shared_data)
{
  
  pm_err_flag_type pm_vib_on;

  if(!boot_dload_is_dload_mode_set())
  {
  	/*turn on vibrator*/
 	BL_VERIFY((pm_vib_on=boot_pm_vib_on())== PM_ERR_FLAG__SUCCESS,(uint16)pm_vib_on|BL_ERROR_GROUP_PMIC);
  }
  
}

/*===========================================================================

**  Function :  sbl1_pm_rpm_pre_init_wrapper

** ==========================================================================
*/
/*!
*
* @brief
*   Do pre init procedure for QSEE.
*
* @param[in]
*   bl_shared_data Pointer to shared data
*
* @par Dependencies
*   Must be called before executing RPM/QSEE
*
* @retval
*   None
*
* @par Side Effects
*   None
*
*/
void sbl1_pm_qsee_pre_init_wrapper( bl_shared_data_type *bl_shared_data )
{
  BL_VERIFY((boot_pm_qsee_pre_init()==PM_ERR_FLAG__SUCCESS),BL_ERROR_GROUP_PMIC);
}
