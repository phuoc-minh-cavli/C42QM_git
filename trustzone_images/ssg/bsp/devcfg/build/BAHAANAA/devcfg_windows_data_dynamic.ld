/*===========================================================================
#
# FILE:         devconfig_data.scl
#
# DESCRIPTION:  Linker File for deviceconfig image
#
# AUTOGENERATED, DON NOT MODIFY THIS FILE
===========================================================================*/
#include "customer.h"

PHDRS {
  DEVCFG_DATA_SEG_TZ PT_LOAD;
  }
SECTIONS {

   DEVCFG_DATA_TZ (DEVCFG_OEM_BASEADDR):
   {
      KEEP(*DevCfgSegInfo_oem_tz.o (.rodata* .data* .sdata*))
      KEEP(*DevCfg_9205_data_oem_tz.o (.rodata* .data* .sdata*))
      KEEP(*DevCfg_9205_oem_tz.o (.rodata* .data* .sdata*))
      KEEP(*core/pmic/pm/config/mdm9205_pme9205/build/devcfg_windows/scorpion/BAHAANAA/DAL_DEVCFG_OEM_QSEE_IMG/9205_xml/src/9205_devcfg_pm_reg_dump_config.o (.rodata* .data* .sdata*))
      KEEP(*core/pmic/pm/config/mdm9205_pme9205/build/devcfg_windows/scorpion/BAHAANAA/DAL_DEVCFG_OEM_QSEE_IMG/9205_xml/src/9205_devcfg_pm_config.o (.rodata* .data* .sdata*))
      KEEP(*core/settings/systemdrivers/icb/build/devcfg_windows/scorpion/BAHAANAA/DAL_DEVCFG_OEM_QSEE_IMG/9205_xml/9205/9205_devcfg_BIMC_error_OEM_data.o (.rodata* .data* .sdata*))
      KEEP(*core/settings/systemdrivers/icb/build/devcfg_windows/scorpion/BAHAANAA/DAL_DEVCFG_OEM_QSEE_IMG/9205_xml/9205/9205_devcfg_NOC_error_OEM_data.o (.rodata* .data* .sdata*))
      KEEP(*core/settings/systemdrivers/icb/build/devcfg_windows/scorpion/BAHAANAA/DAL_DEVCFG_OEM_QSEE_IMG/9205_xml/9205/9205_devcfg_ABT_OEM_data.o (.rodata* .data* .sdata*))
      KEEP(*ssg/securemsm/trustzone/qsee/mink/oem/build/devcfg_windows/scorpion/BAHAANAA/DAL_DEVCFG_OEM_QSEE_IMG/9205_xml/common/9205_devcfg_reset_reason_oem_map.o (.rodata* .data* .sdata*))
      ASSERT(SIZEOF(DEVCFG_DATA_TZ) <= DEVCFG_DATA_TZ_SZ, "Oversize");
   }:DEVCFG_DATA_SEG_TZ

  .got : { * (.got) }
  .got.plt : { * (.got.plt) }
  .debug 0 : { *(.debug) }
  .line 0 : { *(.line) }

  .debug_srcinfo 0 : { *(.debug_srcinfo) }
  .debug_sfnames 0 : { *(.debug_sfnames) }

  .debug_aranges 0 : { *(.debug_aranges) }
  .debug_pubnames 0 : { *(.debug_pubnames) }

  .debug_info 0 : { *(.debug_info .gnu.linkonce.wi.*) }
  .debug_abbrev 0 : { *(.debug_abbrev) }
  .debug_line 0 : { *(.debug_line) }
  .debug_frame 0 : { *(.debug_frame) }
  .debug_str 0 : { *(.debug_str) }
  .debug_loc 0 : { *(.debug_loc) }
  .debug_macinfo 0 : { *(.debug_macinfo) }

  .debug_weaknames 0 : { *(.debug_weaknames) }
  .debug_funcnames 0 : { *(.debug_funcnames) }
  .debug_typenames 0 : { *(.debug_typenames) }
  .debug_varnames 0 : { *(.debug_varnames) }

  .debug_pubtypes 0 : { *(.debug_pubtypes) }
  .debug_ranges 0 : { *(.debug_ranges) }
  .gnu.attributes 0 : { KEEP (*(.gnu.attributes)) }
  .note.gnu.arm.ident 0 : { KEEP (*(.note.gnu.arm.ident)) }
  /DISCARD/ : { *(.ARM.exidx*) *(.note.GNU-stack) *(.gnu_debuglink) *(.gnu.lto_*) *(.init) *(.fini)}
}