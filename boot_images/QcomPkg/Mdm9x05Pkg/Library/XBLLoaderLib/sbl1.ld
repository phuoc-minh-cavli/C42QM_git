/*=============================================================================
  
                     SCATTER LOADING DESCRIPTION FILE
  
  Copyright 2014 - 2018 by Qualcomm Technologies, Inc. All Rights Reserved.
  
  GENERAL DESCRIPTION
  
  The scatter loading description file is used to define the SBL1 memory map.
=============================================================================*/
/*=============================================================================
  
                            EDIT HISTORY FOR FILE
   This section contains comments describing changes made to the module.
   Notice that changes are listed in reverse chronological order.
  
  
  when       who     what, where, why
  --------   ---     -------------------------------------------------------
  12/11/18   aus     Added support for interrupt handling
  16/05/18   ask	 DDR driver compile to RPM code ram
  03/01/17   kpa     Added check to ensure vector table size is within limits
  06/02/16   rp      Remove SCL_DDR_TRAINING_DATA_BUF_SIZE from assert for SBL1_DATA_ZI
  03/29/16   kpa     Move ufs bsp object to ocimem buffer
  03/26/16   kpa     move smc api code to sdi region for access in debug flow  
  03/25/16   kpa     revert updates to reuse bootrom stack address space  
  03/22/16   kpa     relocate boot ZI from ocimem to L2 TCM, Relocate stack to 
                     reclaim bootrom buffer, remove SBL1_STACK,
                     Added DDR driver segment
  03/18/16   kpa     Added XBL_SDI_ASM region to SDI region
  12/17/15   jt      Change sdcc_mmc_util from .bss to COMMON
  10/27/15   elt     Added regions for boot logger time markers
  10/07/15   plc     Add RUMI UFS symbols to correct region
  08/07/15   plc     Update for SDM660
  08/03/15   rp      Moved page table and shared function table to OCIMEM
  07/24/15   ck      Moved HALbimc.o to TCM region
  07/21/15   kpa     Resize XBL L2 TCM ZI buffer.
  07/15/15   bn      Moved buffers used by SDCC's clients to OCIMEM
  02/26/15   ck      Added missing busywait and rodata allocations
  02/20/15   kpa     Code cleanup. 
  02/12/15   kpa     Update Debug/SDI region.  
  02/12/15   kpa     Add assert to check RPM coderam buffer size.  
  02/12/15   kpa     Add XBL code region to rpm code ram buffer.  
  01/31/15   kpa     Updates to zero initialize OCIMEM ZI buffer.  
  01/21/15   kpa     Move pre-ddr init page tables to code ram, add ocimem buffer
  11/20/14   ck      Removed references to USB as it has moved to XBLRamDump
  10/22/14   ck      Added .bss to page table regions to ensure they are ZI
  10/01/14   kpa     Include Target_cust.h instead of boot_target_scl.h
  08/18/14   ck      Cleaned up SBL regions and added region protections
  08/08/14   ck      Renamed and resized SBL1_DLOAD_ZI as contents have moved to XBLRamDump.
  06/26/14   kedara  Update sections.
  05/15/14   kedara  Inital revision.
=============================================================================*/

#include "../../Include/Target_cust.h"

ENTRY(_main)

PHDRS {
  CODE_ROM PT_LOAD;
  CODE_STACK PT_LOAD;
  IRQ_STACK PT_LOAD;  
  CODE_RAM PT_LOAD;
  CODE_RAM_ZI PT_LOAD;
  BOOT_DDR PT_LOAD;
  BOOT_SYSTEM_DEBUG PT_LOAD;
  BOOT_SYSTEM_DEBUG_DATA PT_LOAD;
  BOOT_DDR_DSF_RAM_BUFFER PT_LOAD;
  BOOT_OCIMEM_DATA PT_LOAD;
}

SECTIONS
{ 
/*====================== Sections in OCIMEM ===============================*/
 

 /***********************************************************
 Log from build:
 SYSTEM_DEBUG_DATA loaded at [00226000,0022688b] bss,  data

SYSTEM_DEBUG_CO_RO loaded at [00222000,00226ef3] text, rodata
**************************************************************/


  /* This section contains the system debug image code that came from SDI.
     It is placed in RPM Code RAM memory above RPM FW. */
  SYSTEM_DEBUG_CO_RO SCL_SYSTEM_DEBUG_CO_RO_BASE:
  {
  boot_sdi_util_asm.o* (.text*);
  ClockSDI.o* (.text*);
  boot_sdi_entry.o* (.text*);
  *boot_extern_debug_interface*.o* (.text*);  
  BIMC_v1.o* (.text*);
  BIMC_DPE_v1.o* (.text*);
  BIMC_SHKE_v1.o* (.text*);
  BIMC_SCMO_v1.o* (.text*);
  DDR_PHY.o* (.text*);
  AHB2PHY.o* (.text*);

  ddr_sdi_wrapper.o* (.text*);
  ddr_params.o* (.text*);
  ddr_sync.o* (.text*);
  ddr_log.o* (.text*);
   
  ddr_rpm_target.o* (.text*);
  ddr_config.o* (.text*);
  mem_rdwr.o* (DDRSS_MEM_RDWR);

 *boot_extern_ddr_interface*.o* (.text*);  
    
  /*1409 whole taken by rodata*/
  boot_sdi_util_asm.o* (.rodata*);
  boot_sdi_entry.o* (.rodata*);
  ClockSDI.o* (.rodata*);
 *boot_extern_debug_interface*.o* (.rodata*); 
  BIMC_v1.o* (.rodata*);
  BIMC_DPE_v1.o* (.rodata*);
  BIMC_SHKE_v1.o* (.rodata*);
  BIMC_SCMO_v1.o* (.rodata*);
  DDR_PHY.o* (.rodata*);
  AHB2PHY.o* (.rodata*);
  
  ddr_sdi_wrapper.o* (.rodata*);
  ddr_params.o* (.rodata*);
  ddr_sync.o* (.rodata*);
  ddr_log.o* (.rodata*);
   
  ddr_rpm_target.o* (.rodata*);
  ddr_config.o* (.rodata*);
  mem_rdwr.o* (.rodata*);
*boot_extern_ddr_interface*.o* (.rodata*);

    ASSERT(SIZEOF(SYSTEM_DEBUG_CO_RO) <= SCL_SYSTEM_DEBUG_CO_RO_SIZE, "XBL Loader: SYSTEM_DEBUG_CO_RO Region Overflow");
  } : BOOT_SYSTEM_DEBUG
  
  /* section contains system debug related data and placed above system debug 
     related CO+RO in OCIMEM.
   */
  SYSTEM_DEBUG_DATA SCL_SYSTEM_DEBUG_DATA_BASE : ALIGN(8)
  {
  boot_sdi_util_asm.o* (.data*);
  ClockSDI.o* (.data*);
  boot_sdi_entry.o* (.data*);
  *boot_extern_debug_interface*.o* (.data*);  
  BIMC_v1.o* (.data*);
  BIMC_DPE_v1.o* (.data*);
  BIMC_SHKE_v1.o* (.data*);
  BIMC_SCMO_v1.o* (.data*);
  DDR_PHY.o* (.data*);
  AHB2PHY.o* (.data*);
  

  ddr_sdi_wrapper.o* (.data*);
  ddr_params.o* (.data*);
  ddr_sync.o* (.data*);
  ddr_log.o* (.data*);

  ddr_rpm_target.o* (.data*);
  ddr_config.o* (.data*);
  mem_rdwr.o* (.data*);
  
  boot_sdi_util_asm.o* (.bss*);
  ClockSDI.o* (.bss*);
  *boot_extern_ddr_interface*.o* (.data*);
  boot_sdi_entry.o* (.bss*);
  *boot_extern_debug_interface*.o* (.bss*); 
  BIMC_v1.o* (.bss*);
  BIMC_DPE_v1.o* (.bss*);
  BIMC_SHKE_v1.o* (.bss*);
  BIMC_SCMO_v1.o* (.bss*);
  DDR_PHY.o* (.bss*);
  AHB2PHY.o* (.bss*);

  ddr_sdi_wrapper.o* (.bss*);
  ddr_params.o* (.bss*);
  ddr_sync.o* (.bss*);
  ddr_log.o* (.bss*);

  ddr_rpm_target.o* (.bss*);
  ddr_config.o* (.bss*);
  mem_rdwr.o* (.bss*);
 
    *boot_extern_ddr_interface*.o* (.bss*);

    *(ddr_func_ptr);

    ASSERT(SIZEOF(SYSTEM_DEBUG_DATA) <= SCL_SYSTEM_DEBUG_DATA_SIZE, "XBL Loader: SYSTEM_DEBUG_DATA Region Overflow"); 
  } : BOOT_SYSTEM_DEBUG_DATA
  
  
  /* This must be placed at the end or adjacent execution regions overlap */
  SYSTEM_DEBUG_QSEE_INFO SCL_SYSTEM_DEBUG_QSEE_INFO_BASE :ALIGN(8)
  { 
    Image$$SYSTEM_DEBUG_QSEE_INFO$$Base = . ;
    
  } : BOOT_SYSTEM_DEBUG_DATA
  Image$$SYSTEM_DEBUG_QSEE_INFO$$ZI$$Length = SIZEOF(SYSTEM_DEBUG_QSEE_INFO);

  /*====================== Sections in L2 TCM ===============================*/
  /*  This section contains SBL1's code and ro data */
  SBL1_ROM SCL_SBL1_CODE_BASE: 
  {
    
    Image$$SBL1_VECTOR_TABLE$$Start = . ;  
    *(SBL1_VECTOR_TABLE)
    Image$$SBL1_VECTOR_TABLE$$End = . ;    
    *(SBL1_EXCEPTION_HANDLERS)
    *sbl1.o* (SBL1_ENTRY)
    *(RO)
    *(ARM_MMU)
    *(BOOT_UTIL_ASM)   
    *(FLASH_UTIL_ASM) 
    *(.gcc_except_table  .got .got.plt )
    /* RO DATA */
    *(.gnu.linkonce.r.*)
 ddr_test*.o*(.text*);
  ddr_phy_training*.o*(.text*);
  bimc_training_apis*.o*(.text*);
  ddr_drivers*.o*(.text*);
  ddr_target*.o*(.text*);
  ddr_external_api*.o*(.text*);
  boot_extern_ddr_interface*.o* (ddr_boot_funcs);
 ddr_test*.o*(.rodata* );
  ddr_phy_training*.o*(.rodata* );
  bimc_training_apis*.o*(.rodata* );
  ddr_drivers*.o*(.rodata* );
  ddr_target*.o*(.rodata* );
  ddr_external_api*.o*(.rodata* );
  boot_extern_ddr_interface*.o* (ddr_boot_funcs)  ;
    *(.rodata* )
    *(.constdata* )
    *(.text* )
    *(.stub )

    ASSERT((Image$$SBL1_VECTOR_TABLE$$End - Image$$SBL1_VECTOR_TABLE$$Start) <= SCL_SBL1_VECTOR_TABLE_SIZE, "XBL Loader: SBL1_VECTOR_TABLE Region Overflow");
    ASSERT(SIZEOF(SBL1_ROM) <= SCL_SBL1_CODE_SIZE, "XBL Loader: SBL1_ROM Region Overflow");
  } : CODE_ROM


  SBL1_CODE_STACK SCL_SBL1_CODE_STACK_BASE: 
  {
    . = . + SCL_SBL1_CODE_STACK_SIZE;
    ASSERT(SIZEOF(SBL1_CODE_STACK) == SCL_SBL1_CODE_STACK_SIZE, "Invalid size of SBL1_CODE_STACK Section");  
  } : CODE_STACK

  SBL1_IRQ_STACK SCL_SBL1_IRQ_STACK_BASE: 
  {
    . = . + SCL_SBL1_IRQ_STACK_SIZE;
    ASSERT(SIZEOF(SBL1_IRQ_STACK) == SCL_SBL1_IRQ_STACK_SIZE, "Invalid size of SBL1_IRQ_STACK Section");  
  } : IRQ_STACK

  SBL1_DATA_RW SCL_SBL1_DATA_BASE :
  {
    Image$$SBL1_DATA_RW$$Base = . ;
    *ddr_test*.o*(.data*);
  *ddr_phy_training*.o*(.data*);
  *bimc_training_apis*.o*(.data*);
  *ddr_drivers*.o*(.data*);
  *ddr_target*.o*(.data*);
  *ddr_external_api*.o*(.data*);
    *(ERR_DATA_PTR ERR_DATA  .tdata)
    
    *(.data* )
 
    ASSERT(SIZEOF(SBL1_DATA_RW) <= SCL_SBL1_DATA_SIZE, "XBL Loader: SBL1_DATA_RW Region Overflow");
  } : CODE_RAM
  Image$$SBL1_DATA_RW$$Length = SIZEOF(SBL1_DATA_RW);

  TBSS :{*(.tbss) }

  /*====================== Sections in DDR =========================*/

  /* This section contains all ZI that can be allocated in DDR. */  
  SBL1_DDR_ZI SCL_SBL1_DDR_ZI_BASE :
  {
    Image$$SBL1_DDR_ZI$$Base = . ;

    /* Placeholder to reserve space for the boot logger's meta info data. */
    Image$$SBL1_DDR_LOG_META_INFO_SECTION$$Base = .;
    . = . + SCL_SBL1_BOOT_LOG_META_INFO_SIZE;

    /* Placeholder to reserve buffer space for the boot logger's log messages */
    Image$$SBL1_DDR_LOG_BUF_SECTION$$Base = .;  
    . = . + SCL_SBL1_DDR_BOOT_LOG_BUF_SIZE;

    /* Placeholder to reserve buffer space for the boot logger's time markers */
    Image$$SBL1_DDR_TIME_MARKER_BUF_SECTION$$Base = .;
    . = . + SCL_SBL1_DDR_BOOT_TIME_MARKER_BUF_SIZE;    
    
    *(.bss.BOOT_DDR_ZI_DATA_ZONE)
    *(.bss.BOOT_EXTERNAL_HEAP)
    ASSERT(SIZEOF(SBL1_DDR_ZI) <= SCL_SBL1_DDR_ZI_SIZE, "XBL Loader: SBL1_DDR_ZI Region Overflow");
  } : BOOT_DDR
  Image$$SBL1_DDR_ZI$$ZI$$Length = SIZEOF(SBL1_DDR_ZI);


  /* This section contains L1 and L2 page table that's allocated in DDR.
   SCL_SBL1_DDR_PAGE_TABLE_BASE must be 16k aligned */
  SBL1_DDR_PAGE_TABLE  SCL_SBL1_DDR_PAGE_TABLE_BASE :
 {
    *(.bss.BOOT_DDR_PAGE_TABLE_ZONE)
    ASSERT(SIZEOF(SBL1_DDR_PAGE_TABLE) <= SCL_SBL1_DDR_PAGE_TABLE_SIZE,   "SBL1_DDR_PAGE_TABLE is too large");
  } : BOOT_DDR


  /* This section contains the uncached ZI region */
  SBL1_DDR_UNCACHED_ZI  SCL_SBL1_DDR_UNCACHED_ZI_BASE :
  {
    Image$$SBL1_DDR_UNCACHED_ZI$$Base = . ;
    *(BOOT_DDR_UNCACHED_ZI)  
    *(.bss.BOOT_DDR_UNCACHED_ZI_ZONE)

    ASSERT(SIZEOF(SBL1_DDR_UNCACHED_ZI) <= SCL_SBL1_DDR_UNCACHED_ZI_SIZE, "XBL Loader: SBL1_DDR_UNCACHED_ZI Region Overflow");

    /* Ensure that all DDR regions fit in the allocated memory space */
    ASSERT((SCL_SBL1_DDR_ZI_SIZE + SCL_SBL1_DDR_PAGE_TABLE_SIZE + SCL_SBL1_DDR_UNCACHED_ZI_SIZE) 
            <= SCL_SBL1_DDR_DATA_SIZE, "XBL Loader: SBL1 DDR Region Overflow");
            
  } : BOOT_DDR
  Image$$SBL1_DDR_UNCACHED_ZI$$ZI$$Length = SIZEOF(SBL1_DDR_UNCACHED_ZI);
 

  /*====================== BSS Section in L2 TCM =========================*/
 
   /* Area of L2 TCM used to hold L2 and L3 page tables because PBL does not supply
     enough memory to expand on what it created. */
 
  /* This section contains non DDR ZI. */
  /* Must be placed at EOF as it contains the *(.bss.*) catch all */
  SBL1_DATA_ZI SCL_SBL1_DATA_ZI_BASE: 
  {
    Image$$SBL1_DATA_ZI$$Base = . ;

    /* Placeholder to reserve space for the boot logger's meta info data. */
     Image$$SBL1_LOG_META_INFO_SECTION$$Base = .;
    . = . + SCL_SBL1_BOOT_LOG_META_INFO_SIZE;
    
    /* Placeholder to reserve buffer space for the boot logger's log messages */
    Image$$SBL1_LOG_BUF_SECTION$$Base = .;
    . = . +  SCL_SBL1_BOOT_LOG_BUF_SIZE;

    /* Placeholder to reserve buffer space for the boot logger's time markers */
    Image$$SBL1_TIME_MARKER_BUF_SECTION$$Base = .;
    . = . +  SCL_SBL1_BOOT_TIME_MARKER_BUF_SIZE;
  
    *fs_*.o*(.bss*);        
    *fs_hotplug*.o(.bss);
    *fs_blockdev_ufs*.o(.bss);
    *fs_blockdev*.o(.bss);
  *ddr_test*.o*(.bss*);
  *ddr_drivers*.o*(.bss*);
  *ddr_phy_training*.o*(.bss*);
  *bimc_training_apis*.o*(.bss*);
  *ddr_target*.o*(.bss*);
  *ddr_external_api*.o*(.bss*);
    *(CACHE_MMU_ZI)
    *(.bss.BOOT_INTERNAL_HEAP)
    
    *(.bss* )
        
    *(.dynbss)
    *(COMMON)
    *(__libc_freeres_ptrs)
    ASSERT(SIZEOF(SBL1_DATA_ZI) <= SCL_SBL1_DATA_ZI_SIZE, "XBL Loader: SBL1_DATA_ZI Region Overflow");

    /* Ensure SBL1 regions fit inside of allocated region */
    ASSERT((SCL_SBL1_CODE_SIZE + SCL_SBL1_STACK_SIZE + SCL_SBL1_DATA_SIZE + SCL_SBL1_DATA_ZI_SIZE 
             + SCL_SBL1_PAGE_TABLE_SIZE + SCL_SBL1_SHARED_FUNCTIONS_TABLE_SIZE )
             <= SCL_SBL1_IMAGE_SIZE, "XBLLoader: SBL1 Image Size Overflow");
  } : CODE_RAM_ZI
  Image$$SBL1_DATA_ZI$$ZI$$Length = SIZEOF(SBL1_DATA_ZI) ;

  
  SBL1_SHARED_FUNCTIONS_TABLE SCL_SBL1_SHARED_FUNCTIONS_TABLE_BASE :
  {
    Image$$SBL1_SHARED_FUNCTIONS_TABLE$$Base = . ;
    . = . + SCL_SBL1_SHARED_FUNCTIONS_TABLE_SIZE;

  } : CODE_RAM_ZI
  Image$$SBL1_SHARED_FUNCTIONS_TABLE$$Length = SIZEOF(SBL1_SHARED_FUNCTIONS_TABLE);  

 
   /*======================================================================*/     
  
  .debug          0 : { *(.debug) }
  .line           0 : { *(.line) }
  /* GNU DWARF 1 extensions */
  .debug_srcinfo  0 : { *(.debug_srcinfo) }
  .debug_sfnames  0 : { *(.debug_sfnames) }
  /* DWARF 1.1 and DWARF 2 */
  .debug_aranges  0 : { *(.debug_aranges) }
  .debug_pubnames 0 : { *(.debug_pubnames) }
  /* DWARF 2 */
  .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
  .debug_abbrev   0 : { *(.debug_abbrev) }
  .debug_line     0 : { *(.debug_line) }
  .debug_frame    0 : { *(.debug_frame) }
  .debug_str      0 : { *(.debug_str) }
  .debug_loc      0 : { *(.debug_loc) }
  .debug_macinfo  0 : { *(.debug_macinfo) }
  /* SGI/MIPS DWARF 2 extensions */
  .debug_weaknames 0 : { *(.debug_weaknames) }
  .debug_funcnames 0 : { *(.debug_funcnames) }
  .debug_typenames 0 : { *(.debug_typenames) }
  .debug_varnames  0 : { *(.debug_varnames) }
  /* DWARF 3 */
  .debug_pubtypes 0 : { *(.debug_pubtypes) }
  .debug_ranges   0 : { *(.debug_ranges) }
  .gnu.attributes 0 : { KEEP (*(.gnu.attributes)) }
  .note.gnu.arm.ident 0 : { KEEP (*(.note.gnu.arm.ident)) }
  /DISCARD/ : { *(.ARM.exidx*) *(.note.GNU-stack) *(.gnu_debuglink) *(.gnu.lto_*) *(.init) *(.fini) }  
      
}
