/** @file XBLRamDump.c
  Top-Level Logic for XBLRamDump.c
  
  Copyright (c) 2014-2017, 2018-19, Qualcomm Technologies, Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 06/14/19   aus     Added support for sahara image load for all interfaces
 05/11/17   bd      PMIC WDG support in DLOAD mode for minidump case
 04/04/17   bd      Disable display in DLOAD mode
 03/08/17   sj      Added the logic to enable display if battery threshold match
 02/17/17   din     Updated DDR dload cookie
 10/14/16   digant  set kpd pwr and pshold to hardreset
 10/12/16   digant  boot_set_pwrkey_to_hardreset
 08/08/16   digant  disable display support to fix tip/crm
 05/19/16   kpa     Added support for Display
 09/10/15   kpa     Use dcache_flush_region instead of mmu_flush_cache.
 08/24/15   ck      Added logic to enter PBL EDL if ramdump is not allowed
 09/30/14   ck      efs_boot_initialize must be called before any storage functions
 08/08/14   ck      Added boot_shared_functions_consumer.h
 07/28/14   ck      Initial revision

=============================================================================*/

#include "boot_target.h"
#include "boothw_target.h"
#include "boot_dload_debug.h"
#include "boot_raw_partition_ramdump.h"
#include "boot_sd_ramdump.h"
#include "boot_sahara.h"
#include "boot_extern_efs_interface.h"
#include "boot_shared_functions_consumer.h"
#include "pm_fg_batt_info.h"
#include "boot_loader.h"
#include "boot_sbl_shared.h"
#include "boot_cache.h"

  /* ELF image info - filled in by sbl_sahara_entry */
static mi_boot_image_header_type dload_header_ptr SECTION(".bss.BOOT_UNCACHED_DDR_ZI_ZONE");
static whitelst_tbl_entry_type hostdl_table[] = {DEFAULT_WHITELIST_REGION_TABLE};

/*===========================================================================
**  Function :  XBLRamDumpMain
** ==========================================================================
*/
/*!
* 
* @brief
*   This function is entry point for XBL Ramdump image
*
* @param[in] 
*   None
*
* @par Dependencies
*   None 
* 
* @retval
*   None
* 
*/ 
VOID XBLRamDumpMain( VOID )
{
  struct boot_sahara_interface* sbl_sahara_interface = NULL;
  void  (* jump_point)(void) = NULL ;  /* Hostdl entry point */
  boot_boolean result;

  struct mem_block hostdl_mem_block = 
  {
    /* Base address updated later. Size assumed to be max 2MB */
    0, 0, 0x1200000>>10,
    MMU_L1_NS_SECTION_MAPPING, MMU_L1_PAGETABLE_MEM_READ_WRITE, 
    MMU_L1_PAGETABLE_DEVICE_SHARED, MMU_L1_PAGETABLE_EXECUTABLE_REGION,
    MMU_PAGETABLE_DEF_DOMAIN
  };

  /* Inform the shared functions library where the shared functions table is
     and verify that it was initialized by the producer. */
  boot_shared_functions_init();

  /* configure power key to do a hardreset */
  boot_set_pwrkey_reset_type(BOOT_HARD_RESET_TYPE);
  
  /*configure pshold to hardreset */
  boot_set_pshold_reset_type(BOOT_HARD_RESET_TYPE);
  
  /* If download mode is not available then no need to continue and spin.
     Enter PBL EDL in this scenario. */
  if(!dload_mem_debug_supported())
  { 
    boot_log_message("dload_mem_debug_supported returned error");
    boot_dload_transition_pbl_forced_dload();
  }

  /* Disable PMIC for QPST case, since QPST attache is manual and may result in WDOG expiry */
  boot_pm_pon_wdog_enable(PMIC_DEVICE_INDEX, PM_OFF);  
  /* Enter Sahara */

  /* Get Sahara interface */
  sbl_sahara_interface = sbl_sahara_get_interface();
  BL_VERIFY(sbl_sahara_interface != NULL, BL_ERROR_GROUP_BOOT|BL_ERR_NULL_PTR_PASSED);
  
  /* Flush the cache before calling into sahara so that all data is flushed to memory */  
  dcache_flush_region((void *)SCL_SBL1_DDR_ZI_BASE, SCL_SBL1_DDR_ZI_SIZE);
  dcache_flush_region((void *)SCL_SBL1_OCIMEM_DATA_BASE, SCL_SBL1_OCIMEM_DATA_SIZE);
  
    /* Prepare for receiving hostdl image */
    sbl_sahara_interface->sahara_shared_data->image_rx_complete = FALSE;
    
    /* Enter sahara to take ram dumps. Load Hostdl image if sent by host*/
    sbl_sahara_entry(&dload_header_ptr, 
                     (uint32) HOSTDL_IMG, 
                     SAHARA_IMAGE_TYPE_ELF,
                     (uint32)SECBOOT_FLASH_PRG_SW_TYPE,
                     TRUE,  /* Auth Flag */
                     TRUE, /* Jump Flag */
					 hostdl_table
                     );  
    
    if (sbl_sahara_interface->sahara_shared_data->image_rx_complete == TRUE)
    {
      /* Mark hostdl region as executable */
      hostdl_mem_block.p_base = boot_sahara_get_elf_entry_address();

      /*Align on 1 MB boundary */
      hostdl_mem_block.p_base = ((hostdl_mem_block.p_base >> 20)<<20);
      hostdl_mem_block.v_base = hostdl_mem_block.p_base ;
	  
	  dcache_inval_region((void *)SCL_SBL1_DDR_PAGE_TABLE_BASE, SCL_SBL1_DDR_PAGE_TABLE_SIZE); 

      result = boot_mmu_page_table_map_single_mem_block_L1(&hostdl_mem_block); 
  
      BL_VERIFY(result, BL_ERR_SBL);      
      
      jump_point = (void (*)(void))(boot_sahara_get_elf_entry_address());

      /*Jump to Hostdl image */
      jump_point();
    }

}

