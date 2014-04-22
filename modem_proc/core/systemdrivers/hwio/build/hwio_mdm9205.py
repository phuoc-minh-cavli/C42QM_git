#===========================================================================
#
#  @file hwio_mdm9205.py
#  @brief HWIO config file for the HWIO generation scripts for MDM9205.
#
#  This file can be invoked by calling:
#
#    HWIOGen.py --cfg=hwio_mdm9205.py --flat=..\..\..\api\systemdrivers\hwio\mdm9205\ARM_ADDRESS_FILE.FLAT
#
#  ===========================================================================
#
#  Copyright (c) 2019 Qualcomm Technologies Incorporated.
#  All rights reserved.
#  QUALCOMM Proprietary and Confidential.
#
#  ===========================================================================
#
#  $Header: //components/rel/core.mpss/3.10/systemdrivers/hwio/build/hwio_mdm9205.py#5 $
#  $DateTime: 2019/01/30 02:50:00 $
#  $Author: pwbldsvc $
#
#  ===========================================================================

CHIPSET = 'mdm9205'

# ============================================================================
# HWIO_BASE_FILES
# ============================================================================

bases = [
  'BOOT_ROM',
  'BOOT_ROM_START_ADDRESS',
  'BOOT_ROM_END_ADDRESS',
  'RPM',
  'RPM_SS_MSG_RAM_START_ADDRESS',
  'SECURITY_CONTROL',
  'MPM2_MPM',
  'MSS_TOP',
  'PMIC_ARB',
  'QDSS_SOC_DBG',
  'CLK_CTL',
  'CORE_TOP_CSR',
  'CRYPTO0_CRYPTO_TOP',
  'TLMM',
  'BIMC',
  'ULTAUDIO_CORE',
  'PRNG_PRNG_TOP',
  'PC_NOC',
  'BLSP1_BLSP',
  'PDM_PERPH_WEB',
  'DEHR_WRAPPER_8K_R01',
  'QPIC_QPIC_XPU2',
  'XPU_CFG_PCNOC_CFG_MPU1132_4_M19L12_AHB',
  'BIMC_MPU_MPU1132_32_M31L10_AHB',
  'OCIMEM_WRAPPER_CSR',
  'PCNOC_SNOC_MPU_MPU1132_32_M31L10_AHB',
  'XPU_CFG_RPM_CFG_MPU1132_2_M21L12_AHB',
  'MPU_BOOT_IMEM_CFG_MPU1132A_4_M31L12_AHB',
  'PKA_WRAPPER',
  'QC_DCC_TWIZY_2KB2LL',
  'MPU_MSS_Q6_CFG_MPU1132A_20_M31L12_AXI',
  'VMIDMT_PCNOC_SNOC_2_CFG_VMIDMT_SMR_128_SSD7_SID7_MA128',
]

base_resize = {
  'BOOT_ROM'                     : 0x100000,
  'BOOT_ROM_START_ADDRESS'       :      0x0,
  'BOOT_ROM_END_ADDRESS'         :      0x0,
  'RPM_SS_MSG_RAM_START_ADDRESS' :   0x3000,
}

HWIO_BASE_FILES = [
  {
    'filename': '../../../api/systemdrivers/hwio/' + CHIPSET + '/msmhwiobase.h',
    'bases': bases,
    'map-type': 'qurt-opt',
    'virtual-address-start': 0xE0000000,
    'virtual-address-end': 0xF0000000,
    'resize': base_resize,
    'qurt-memsection-filename': '../../../api/systemdrivers/hwio/' + CHIPSET + '/msmhwioplat.xml',
    'qurt-memsection-attributes': {'PRNG_PRNG_TOP': 'tlb_lock="boot" glob="1"', 'CLK_CTL': 'tlb_lock="boot" glob="1"'},
    'default-cache-policy': 'device',
    'devcfg-filename': '../config/' + CHIPSET + '/HWIOBaseMap.c',
    'check-sizes': True,
    'check-for-overlaps': True,

    # Modem firmware needs MSS_TOP to have fixed virtual
    # address to set  special cache attributes. This is
    # handled in the top-level qdsp6.xml file.
    'fixed-virtual-address': { 'MSS_TOP': 0xEC000000 },
    'skip-memsection': ['MSS_TOP'],
  }
]


# ============================================================================
# HWIO_REGISTER_FILES
# ============================================================================

HWIO_REGISTER_FILES = [
  {
    'filename': '../hw/' + CHIPSET + '/msmhwioreg.h.ref',
    'bases': bases,
    'filter-exclude': ['RESERVED', 'DUMMY']
  }
]

# ============================================================================
# HWIO_T32VIEW_FILES
# ============================================================================

HWIO_T32VIEW_FILES = [
  {
    'symbol-filename': '../scripts/' + CHIPSET + '/hwio.cmm',
    'limit-array-size': [ 10, 4 ],
    'per-filename': '../scripts/' + CHIPSET + '/hwioreg',
    'filter-exclude': ['RESERVED', 'DUMMY'],
    'header': '''
#error This file is for reference only and cannot be included.  See "go/hwio" or mail corebsp.sysdrv.hwio for help.
'''
  },
]


# ============================================================================
# Main
#
# Entry point when invoking this directly.
# ============================================================================

if __name__ == "__main__":
  from subprocess import Popen
  hwiogen = Popen(["\\\\ben\\corebsp_labdata_0001\\sysdrv\\hwio\\HWIOGen.py", "--cfg=hwio_" + CHIPSET + ".py", "--flat=../../../api/systemdrivers/hwio/" + CHIPSET + "/ARM_ADDRESS_FILE.FLAT"], shell=True)
  hwiogen.wait()
