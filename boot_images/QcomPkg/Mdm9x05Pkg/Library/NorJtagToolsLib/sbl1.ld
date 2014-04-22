/*=============================================================================
  
                     SCATTER LOADING DESCRIPTION FILE
  
  Copyright 2018, 2019 by Qualcomm Technologies, Inc. All rights reserved.
  GENERAL DESCRIPTION
  
  The scatter loading description file is used to define the Jtag Programmer memory map.
=============================================================================*/
/*=============================================================================
  
                            EDIT HISTORY FOR FILE
   This section contains comments describing changes made to the module.
   Notice that changes are listed in reverse chronological order.
  
  
  when         who     what, where, why
  ----------   ---     -------------------------------------------------------
  2018-11-01   sb      Updated for 9205
  2018-23-02   svl     Inital revision for VT.
=============================================================================*/

/*=============================================================================
     NOTE:  The linker script name *has* to be "sbl1.ld" as the build rules
     use that name to check for linker script presence
=============================================================================*/

#include "../../Include/Target_cust.h"

ENTRY(_ModuleEntryPoint)

PHDRS {
  NOR_JTAG_CODE_ROM PT_LOAD;    
  NOR_JTAG_CODE_STACK PT_LOAD;
  NOR_JTAG_APP_RAM PT_LOAD;
  NOR_JTAG_ZI_RAM PT_LOAD;
  NOR_JTAG_PARAM_ADDR PT_LOAD;
}

SECTIONS
{
  /*  This section of 128KB contains Jtag Programmer code */
  NOR_JTAG_CODE SCL_BOOTIMEM_BASE_ADDR: 
  {
    *nor_start.o* (StartHere)
    *nor_core.o* (RO)
    *(.text .stub .text.*)
    *(ARM_MMU)
    *(RO)
    *(.gcc_except_table  .got .got.plt )
    /* RO DATA */
    *(.constdata .rodata .rodata.* .gnu.linkonce.r.*)
    /*TODO: we are 200bytes over, optimize*/
    ASSERT(SIZEOF(NOR_JTAG_CODE) <= SIZE_128KB, "Jtag Programmer code is greater than dedicated 128KB");
  } : NOR_JTAG_CODE_ROM
  
  /* Stack location = IMEM Base + 128KB CODE_ROM + 32KB Stack */
  NOR_JTAG_STACK (SCL_BOOTIMEM_BASE_ADDR + SIZE_128KB) : 
  {
    . = . + SIZE_32KB;
    Image$$NOR_JTAG_STACK$$Start = . ;
  } : NOR_JTAG_CODE_STACK

  /* The rest of the section contains Jtag Programmer Code RAM and ZI */
  NOR_JTAG_RW :
  {
    *(.data .data.* .data1)
    *(RW)
    Image$$NOR_JTAG_ZI$$Start = . ;
  } : NOR_JTAG_APP_RAM

  /* TODO: add good comment after changing this change param.cmm as well */
  /* 0x8020 = sizeof(flash_prog_param) */
  /* Total size of BOOT IMEM: SIZE_512KB */
  /* Keep NAND PARAM data at last of BOOT IMEM */
  /* So to calculate starting offset of nand param we subtract
    sizeof(flash_prog_param) from BOOT IMEM size i.e.
    SIZE_512KB - 0x8020 = 0x77Fe0
                                   _______________________
    Boot Imem Start (0x08C00000)  |                       |
                                  |        Code,          |
                                  |        Stack,         |
                                  |        RW area        |
                                  |                       |
                                  |_______________________|
    NOR PARAM Start (0x08C77FE0)  |                       |
                                  |                       |
    Boot Imem End  (0x08C80000)   |_______________________|
  */
  NOR_JTAG_ZI_PARAM_ADDR (SCL_BOOTIMEM_BASE_ADDR + 0x77FE0):
  {
     Image$$NOR_PARAM_ADDR$$Start = . ;
    *(.bss.FLASH_PROG_PARAM)
  } : NOR_JTAG_PARAM_ADDR

  /**/
  NOR_JTAG_ZI_ADDR (Image$$NOR_JTAG_ZI$$Start):
  {
    *(.bss.FLASH_TOOLS_HEAP_ZONE)
    *(COMMON)
    *(.bss*)
    *(ZI)
     Image$$NOR_JTAG_ZI$$End = . ;
  } : NOR_JTAG_ZI_RAM

  NOR_JTAG_IMEM_END (SCL_BOOTIMEM_BASE_ADDR + SIZE_512KB) :
  {
     Image$$NOR_JTAG_MEM$$End = . ;
  }
  
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
