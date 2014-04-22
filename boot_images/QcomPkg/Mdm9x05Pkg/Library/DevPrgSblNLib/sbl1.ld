/*=============================================================================
  
                     SCATTER LOADING DESCRIPTION FILE

  Copyright (c) 2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Proprietary
 
  The scatter loading description file is used to define the SBL1 memory map.
=============================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: $
  $DateTime: $
  $Author: $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2019-04-30   as    Initial Revision.

===========================================================================*/
#include "../../Include/Target_cust.h"

ENTRY(_main)

PHDRS {
  CODE_ROM PT_LOAD;
  CODE_STACK PT_LOAD;
  CODE_RAM PT_LOAD;
  CODE_RAM_ZI PT_LOAD;
  BOOT_DDR PT_LOAD;
  BOOT_SYSTEM_DEBUG PT_LOAD;
  BOOT_SYSTEM_DEBUG_DATA PT_LOAD;
  BOOT_DDR_DSF_RAM_BUFFER PT_LOAD;
  BOOT_OCIMEM_DATA PT_LOAD;
  BOOT_OCIMEM_DEVICEPROG_DATA PT_LOAD;
}

SECTIONS
{
  /*====================== Sections in OCIMEM ===============================*/

  SBL1_OCIMEM_DEVICEPROG_DATA SCL_SBL_DEVPRG_ZI_BASE: 
  {
    Image$$SBL1_DEVICEPROG_FH_ZI$$Base = . ;
    *fs_hotplug_parser.o* (.bss);
    *boot_sdcc_hotplug.o* (.bss);
    *fs_hotplug*.o* (.bss);
    *fs_blockdev*.o* (.bss);
    qusb_*.o* (COMMON);
    qusb_*.o* (.bss .bss.*);
    *(.bss.BOOT_UNCACHED_ZI_ZONE)
    *devprg_*.o* (.bss .bss.*);
    *devprg_*.o* (COMMON);
    devprgb_buffers_li*.o* (.bss .bss.*);
    devprgb_buffers_li*.o* (COMMON);

    ASSERT(SIZEOF(SBL1_OCIMEM_DEVICEPROG_DATA) <= SCL_SBL_DEVPRG_ZI_SIZE, "Invalid size of SBL1_OCIMEM_DEVICEPROG_DATA Section for DeviceProgrammerSBLN [ERROR]");
  }: BOOT_OCIMEM_DEVICEPROG_DATA
  Image$$SBL1_DEVICEPROG_FH_ZI$$ZI$$Length = SIZEOF(SBL1_OCIMEM_DEVICEPROG_DATA);


 /***********************************************************
 Log from build:
 SYSTEM_DEBUG_DATA loaded at [00226000,0022688b] bss,  data

SYSTEM_DEBUG_CO_RO loaded at [00222000,00226ef3] text, rodata
**************************************************************/

  /* This section contains the system debug image code that came from SDI.
     It is placed in RPM Code RAM memory above RPM FW. */
  SYSTEM_DEBUG_CO_RO SCL_DEVPRG_SYSTEM_DEBUG_CO_RO_BASE:
  { 

  BIMC_v1.o* (.text*);
  BIMC_DPE_v1.o* (.text*);
  BIMC_SHKE_v1.o* (.text*);
  BIMC_SCMO_v1.o* (.text*);
  DDR_PHY.o* (.text*);
  AHB2PHY.o* (.text*);

  ddr_params.o* (.text*);
  ddr_sync.o* (.text*);
  ddr_log.o* (.text*);

  ddr_rpm_target.o* (.text*);
  ddr_config.o* (.text*);
  mem_rdwr.o* (.text*);
  *boot_extern_ddr_interface*.o* (.text*);  

    /*1409 whole taken by rodata*/

  BIMC_v1.o* (.rodata*);
  BIMC_DPE_v1.o* (.rodata*);
  BIMC_SHKE_v1.o* (.rodata*);
  BIMC_SCMO_v1.o* (.rodata*);
  DDR_PHY.o* (.rodata*);
  AHB2PHY.o* (.rodata*);

  ddr_sync.o* (.rodata*);
  ddr_log.o* (.rodata*);

  ddr_rpm_target.o* (.rodata*);
  ddr_config.o* (.rodata*);
  mem_rdwr.o* (.rodata*);
  *boot_extern_ddr_interface*.o* (.rodata*);

    ASSERT(SIZEOF(SYSTEM_DEBUG_CO_RO) <= SCL_DEVPRG_SYSTEM_DEBUG_CO_RO_SIZE, "SYSTEM_DEBUG_CO_RO Region Overflow for DeviceProgrammerSBLN [ERROR]");
  } : BOOT_SYSTEM_DEBUG

  /* section contains system debug related data and placed above system debug 
     related CO+RO in OCIMEM.
   */
  SYSTEM_DEBUG_DATA SCL_DEVPRG_SYSTEM_DEBUG_DATA_BASE : ALIGN(8)
  {
  BIMC_v1.o* (.data*);
  BIMC_DPE_v1.o* (.data*);
  BIMC_SHKE_v1.o* (.data*);
  BIMC_SCMO_v1.o* (.data*);
  DDR_PHY.o* (.data*);
  AHB2PHY.o* (.data*);

  ddr_params.o* (.data*);
  ddr_sync.o* (.data*);
  ddr_log.o* (.data*);

  ddr_rpm_target.o* (.data*);
  ddr_config.o* (.data*);
  mem_rdwr.o* (.data*);
  *boot_extern_ddr_interface*.o* (.data*);
  BIMC_v1.o* (.bss*);
  BIMC_DPE_v1.o* (.bss*);
  BIMC_SHKE_v1.o* (.bss*);
  BIMC_SCMO_v1.o* (.bss*);
  DDR_PHY.o* (.bss*);
  AHB2PHY.o* (.bss*);

  ddr_params.o* (.bss*);
  ddr_sync.o* (.bss*);
  ddr_log.o* (.bss*);

  ddr_rpm_target.o* (.bss*);
  ddr_config.o* (.bss*);
  mem_rdwr.o* (.bss*);
  *boot_extern_ddr_interface*.o* (.bss*);

  *(ddr_func_ptr);

    ASSERT(SIZEOF(SYSTEM_DEBUG_DATA) <= SCL_DEVPRG_SYSTEM_DEBUG_DATA_SIZE, "SYSTEM_DEBUG_DATA Region Overflow for DeviceProgrammerSBLN [ERROR]"); 
  } : BOOT_SYSTEM_DEBUG_DATA


  /* This must be placed at the end or adjacent execution regions overlap */
  SYSTEM_DEBUG_QSEE_INFO SCL_DEVPRG_SYSTEM_DEBUG_QSEE_INFO_BASE :ALIGN(8)
  { 
    Image$$SYSTEM_DEBUG_QSEE_INFO$$Base = . ;

  } : BOOT_SYSTEM_DEBUG_DATA
  Image$$SYSTEM_DEBUG_QSEE_INFO$$ZI$$Length = SIZEOF(SYSTEM_DEBUG_QSEE_INFO);
  
  /*====================== Sections in L2 TCM ===============================*/
  /*  This section contains SBL1's code and ro data */
  SBL1_ROM SCL_DEVPRG_SBL1_CODE_BASE: 
  {
    *(BOOT_UTIL_ASM)   
    Image$$SBL1_VECTOR_TABLE$$Start = . ;  
    *(SBL1_VECTOR_TABLE)
    Image$$SBL1_VECTOR_TABLE$$End = . ;    
    *(SBL1_EXCEPTION_HANDLERS)
    *sbl1_Aarch64.o* (SBL1_ENTRY)
    *(RO)
    *(ARM_MMU)
    *(.gcc_except_table  .got .got.plt )
    /* RO DATA */
    *(.gnu.linkonce.r.*)
    ddr_test*.o*(.text*);
    ddr_phy_training*.o*(.text*);
    bimc_training_apis*.o*(.text*);
    ddr_drivers*.o*(.text*);
    ddr_target*.o*(.text*);
	/*=============WARNING=============*/
	/*  asm code works by keeping this line here
		do not change this.
	*/
	*(FLASH_UTIL_ASM)
	/*=============WARNING=============*/
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
    *(.text*)
    *(.stub )

    ASSERT((Image$$SBL1_VECTOR_TABLE$$End - Image$$SBL1_VECTOR_TABLE$$Start) <= SCL_SBL1_VECTOR_TABLE_SIZE, "SBL1_VECTOR_TABLE Region Overflow for DeviceProgrammerSBLN [ERROR]");
     ASSERT(SIZEOF(SBL1_ROM) <= SCL_DEVPRG_SBL1_CODE_SIZE, "Invalid size of SBL1_ROM Section for DeviceProgrammerSBLN [ERROR]");
  } : CODE_ROM


  SBL1_STACK SCL_DEVPRG_SBL1_STACK_BASE: 
  {
    . = . + SCL_DEVPRG_SBL1_STACK_SIZE;
    ASSERT(SIZEOF(SBL1_STACK) == SCL_DEVPRG_SBL1_STACK_SIZE, "Invalid size of SBL1_STACK Section for DeviceProgrammerSBLN [ERROR]");  
  } : CODE_STACK


  SBL1_DATA_RW SCL_DEVPRG_SBL1_DATA_BASE :
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

    ASSERT(SIZEOF(SBL1_DATA_RW) <= SCL_DEVPRG_SBL1_DATA_SIZE, "Invalid size of SBL1_DATA_RW Section for DeviceProgrammerSBLN [ERROR]");
  } : CODE_RAM
  Image$$SBL1_DATA_RW$$Length = SIZEOF(SBL1_DATA_RW);

  TBSS :{*(.tbss) }

  /*====================== Sections in DDR =========================*/

  /* This section contains all ZI that can be allocated in DDR. */  
  SBL1_DDR_ZI SCL_DEVPRG_SBL1_DDR_ZI_BASE :
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

    * (.bss.BOOT_DDR_ZI_DATA_ZONE)
    *fs_*.o* (.bss*);
    *hfat*.o* (.bss*)
    *Ce*.o* (.bss*)
    *(.bss.BOOT_EXTERNAL_HEAP)
    *(.bss.BOOT_DDR_ZI_ZONE)
    ASSERT(SIZEOF(SBL1_DDR_ZI) <= SCL_DEVPRG_SBL1_DDR_ZI_SIZE, "SBL1_DDR_ZI is too large for DeviceProgrammerSBLN [ERROR]");
  } : BOOT_DDR
  Image$$SBL1_DDR_ZI$$ZI$$Length = SIZEOF(SBL1_DDR_ZI);

  SBL1_DEVICEPROG_USB_ZI : ALIGN(8)
  {
    Image$$SBL1_DEVICEPROGRAMMER_USB_ZI$$Base = .;
    *devprgb_buffers_dd*.o* (.bss .bss.*);
    *devprgb_buffers_dd*.o* (COMMON);
  }
  Image$$SBL1_DEVICEPROGRAMMER_USB_ZI$$ZI$$Length = SIZEOF(SBL1_DEVICEPROG_USB_ZI);

  /* This section contains L1 and L2 page table that's allocated in DDR.
   SCL_DEVPRG_SBL1_DDR_PAGE_TABLE_BASE must be 16k aligned */
  SBL1_DDR_PAGE_TABLE  SCL_DEVPRG_SBL1_DDR_PAGE_TABLE_BASE :
 {
    *(.bss.BOOT_DDR_PAGE_TABLE_ZONE)
    ASSERT(SIZEOF(SBL1_DDR_PAGE_TABLE) <= SCL_DEVPRG_SBL1_DDR_PAGE_TABLE_SIZE,   "SBL1_DDR_PAGE_TABLE is too large for DeviceProgrammerSBLN [ERROR]");
  } : BOOT_DDR


  /* This section contains the uncached ZI region */
  SBL1_DDR_UNCACHED_ZI  SCL_DEVPRG_SBL1_DDR_UNCACHED_ZI_BASE :
  {
    Image$$SBL1_DDR_UNCACHED_ZI$$Base = . ;
    *(BOOT_DDR_UNCACHED_ZI)  
    *(.bss.BOOT_DDR_UNCACHED_ZI_ZONE)

    ASSERT(SIZEOF(SBL1_DDR_UNCACHED_ZI) <= SCL_DEVPRG_SBL1_DDR_UNCACHED_ZI_SIZE, "SBL1_DDR_UNCACHED_ZI is too large for DeviceProgrammerSBLN [ERROR]");

    /* Ensure that all DDR regions fit in the allocated memory space */
    ASSERT((SCL_DEVPRG_SBL1_DDR_ZI_SIZE + SCL_DEVPRG_SBL1_DDR_PAGE_TABLE_SIZE + SCL_DEVPRG_SBL1_DDR_UNCACHED_ZI_SIZE) <= SCL_DEVPRG_SBL1_DDR_DATA_SIZE, "SBL1 DDR Region Overflow for DeviceProgrammerDDR [ERROR]");
  } : BOOT_DDR
  Image$$SBL1_DDR_UNCACHED_ZI$$ZI$$Length = SIZEOF(SBL1_DDR_UNCACHED_ZI);


  /*====================== BSS Section in L2 TCM =========================*/

   /* Area of L2 TCM used to hold L2 and L3 page tables because PBL does not supply
     enough memory to expand on what it created. */
  SBL1_PAGE_TABLE  SCL_DEVPRG_SBL1_PAGE_TABLE_BASE :
  {
    *(.bss.BOOT_PAGE_TABLE_ZONE)
    ASSERT(SIZEOF(SBL1_PAGE_TABLE) <= SCL_DEVPRG_SBL1_PAGE_TABLE_SIZE,   "DeviceProgrammerSBLN: SBL1_PAGE_TABLE Region Overflow [ERROR]");
  } : CODE_RAM_ZI  


  /* This section contains non DDR ZI. */
  /* Must be placed at EOF as it contains the *(.bss.*) catch all */
  SBL1_DATA_ZI SCL_DEVPRG_SBL1_DATA_ZI_BASE: 
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
    *fs_hotplug*.o*(.bss);
    *fs_blockdev_ufs*.o*(.bss);
    *fs_blockdev*.o*(.bss);
    *ddr_test*.o*(.bss*);
    *ddr_drivers*.o*(.bss*);
    *ddr_phy_training*.o*(.bss*);
    *bimc_training_apis*.o*(.bss*);
    *ddr_target*.o*(.bss*);
    *ddr_external_api*.o*(.bss*);

    *(.bss.BOOT_INTERNAL_HEAP)
    *(CACHE_MMU_ZI)
    /* excludes .bss for DDR_ZI and DLOAD_ZI */    
    *( .bss* )
    *(.dynbss)
    *(COMMON)
    *(__libc_freeres_ptrs)
    ASSERT(SIZEOF(SBL1_DATA_ZI) <= SCL_DEVPRG_SBL1_DATA_ZI_SIZE, "Invalid size of SBL1_DATA_ZI Section for DeviceProgrammerSBLN [ERROR]");

    /* Ensure SBL1 regions fit inside of allocated region */
    ASSERT((SCL_DEVPRG_SBL1_CODE_SIZE + SCL_DEVPRG_SBL1_STACK_SIZE+ SCL_DEVPRG_SBL1_DATA_SIZE + SCL_DEVPRG_SBL1_DATA_ZI_SIZE
             + SCL_DEVPRG_SBL1_PAGE_TABLE_SIZE + SCL_DEVPRG_SBL1_SHARED_FUNCTIONS_TABLE_SIZE)
            <= SCL_SBL1_IMAGE_SIZE, "SBL1 Image Size Overflow for DeviceProgrammerSBLN [ERROR]");
  } : CODE_RAM_ZI
  Image$$SBL1_DATA_ZI$$ZI$$Length = SIZEOF(SBL1_DATA_ZI) ;

  
  SBL1_SHARED_FUNCTIONS_TABLE SCL_DEVPRG_SBL1_SHARED_FUNCTIONS_TABLE_BASE :
  {
    Image$$SBL1_SHARED_FUNCTIONS_TABLE$$Base = . ;
    . = . + SCL_DEVPRG_SBL1_SHARED_FUNCTIONS_TABLE_SIZE;

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
