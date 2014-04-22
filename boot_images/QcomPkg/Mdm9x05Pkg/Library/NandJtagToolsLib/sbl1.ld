/*=============================================================================

                     SCATTER LOADING DESCRIPTION FILE

  Copyright 2018 by Qualcomm Technologies, Inc. 
  All Rights Reserved.

  GENERAL DESCRIPTION

  The scatter loading description file is used to define the Jtag Programmer memory map.
=============================================================================*/
/*=============================================================================

                            EDIT HISTORY FOR FILE
   This section contains comments describing changes made to the module.
   Notice that changes are listed in reverse chronological order.


  when         who     what, where, why
  ----------   ---     -------------------------------------------------------
  2019-06-11   sl      Updated for 9205 Multipage
  2018-09-09   sb      Updated for 9205
  2018-02-20   svl     Support for VT
=============================================================================*/

/*=============================================================================
     NOTE:  The linker script name *has* to be "sbl1.ld" as the build rules
     use that name to check for linker script presence
=============================================================================*/

#include "../../Include/Target_cust.h"

ENTRY(_ModuleEntryPoint)

PHDRS {
  NAND_JTAG_CODE_ROM PT_LOAD;    
  NAND_JTAG_CODE_STACK PT_LOAD;
  NAND_JTAG_APP_RAM PT_LOAD;
  NAND_JTAG_ZI_RAM PT_LOAD;
  NAND_JTAG_PARAM_ADDR PT_LOAD;
}

SECTIONS
{
  /*  This section of 128KB contains Jtag Programmer code */
  NAND_JTAG_CODE SCL_IMEM_BASE: 
  {
    *nand_start.o* (StartHere)
    *nand_entry.o* (RO)
    *nand_core.o* (RO)
    *(.text .stub .text.*)
    *(ARM_MMU)
    *(RO)
    *(.gcc_except_table  .got .got.plt )
    /* RO DATA */
    *(.constdata .rodata .rodata.* .gnu.linkonce.r.*)
    /*TODO: we are 200bytes over, optimize*/
    ASSERT(SIZEOF(NAND_JTAG_CODE) <= SIZE_128KB, "Jtag Programmer code is greater than dedicated 128KB");
  } : NAND_JTAG_CODE_ROM

  /* Stack location = IMEM Base + 128KB CODE_ROM + 32KB Stack */

  NAND_JTAG_STACK SCL_BOOTIMEM_BASE_ADDR: 
  {
    . = . + SIZE_32KB;
    Image$$NAND_JTAG_STACK$$Start = . ;
  } : NAND_JTAG_CODE_STACK

  /* The rest of the section contains Jtag Programmer Code RAM and ZI */
  NAND_JTAG_RW :
  {
    *(.data .data.* .data1)
    *(RW)
    Image$$NAND_JTAG_ZI$$Start = . ;
  } : NAND_JTAG_APP_RAM

  /*
    0x21020 = sizeof(flash_prog_param)
    Total size of BOOT IMEM: SIZE_512KB
    Keep NAND PARAM data at last of BOOT IMEM
    So to calculate starting offset of nand param we subtract
    sizeof(flash_prog_param) from BOOT IMEM size i.e.
    SIZE_512KB - 0x21020 = 0x5EFE0
                                   _______________________
    Boot Imem Start (0x08C00000)  |                       |
                                  |        Stack,         |
                                  |        RW area        |
                                  |                       |
                                  |_______________________|
    NAND PARAM Start (0x08C5EFE0) |                       |
                                  |                       |
    Boot Imem End  (0x08C80000)   |_______________________|
  */
  NAND_JTAG_ZI_PARAM_ADDR (SCL_BOOTIMEM_BASE_ADDR + 0x5EFE0):
  {
     Image$$NAND_PARAM_ADDR$$Start = . ;
    *(.bss.FLASH_PROG_PARAM)
  } : NAND_JTAG_PARAM_ADDR

  /* check what is COMMON */
  NAND_JTAG_ZI_ADDR (Image$$NAND_JTAG_ZI$$Start):
  {
    *(.bss.FLASH_TOOLS_HEAP_ZONE)
    *(COMMON)
    *(.bss*)
    *(ZI)
     Image$$NAND_JTAG_ZI$$End = . ;
  } : NAND_JTAG_ZI_RAM


/*  NAND_JTAG_IMEM_END (SCL_IMEM_BASE + SCL_IMEM_SIZE) :*/
  NAND_JTAG_IMEM_END (SCL_BOOTIMEM_BASE_ADDR + SIZE_512KB) :
  {
     Image$$NAND_JTAG_MEM$$End = . ;
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
