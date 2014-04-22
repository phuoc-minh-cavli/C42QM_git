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
#  Copyright (c) 2018 Qualcomm Technologies Incorporated.
#  All Rights Reserved.
#  QUALCOMM Proprietary and Confidential.
#
#  ===========================================================================
#
#  $Header: //components/rel/core.tx/6.0/systemdrivers/hwio/build/hwio_mdm9205.py#1 $
#  $DateTime: 2020/01/30 22:49:35 $
#  $Author: pwbldsvc $
#
#  ===========================================================================

CHIPSET = 'mdm9205'

# ============================================================================
# HWIO_BASE_FILES
# ============================================================================

bases = [
  '.*'
]

bases_exclude = [
  'A7SS_L2_END',
  'A7SS_L2_SIZE',
  'BOOT_ROM',
  'BOOT_ROM_END_ADDRESS',
  'BOOT_ROM_SIZE',
  'BOOT_ROM_START_ADDRESS',
  'DDR',
  'DDR_END',
  'DDR_SIZE',
  'DEHR.*',
  'EBI1_PHY_CFG',
  'LPASS_LPASS_LPM',
  'LPASS_LPM',
  'LPASS_LPM_SIZE',
  'LPASS_LPM_END',
  'MSS_TCM_END',
  'MSS_TCM_SIZE',
  'MSS_TCM',
  'QDSS_STM',
  'QDSS_STM_END',
  'QDSS_STM_SIZE',
  'QDSS_WRAPPER_TOP',
  'RPM_CODE_RAM_END_ADDRESS',
  'RPM_CODE_RAM_SIZE',
  'RPM_CODE_RAM_START_ADDRESS',
  'RPM_DATA_RAM_END_ADDRESS',
  'RPM_DATA_RAM_SIZE',
  'RPM_DATA_RAM_START_ADDRESS',
  'RPM_MSG_RAM',
  'RPM_SS_MSG_RAM_END_ADDRESS',
  'RPM_SS_MSG_RAM_SIZE',
  'IMEM',
  'IMEM_END',
  'IMEM_SIZE',
  'BOOT_IMEM',
  'BOOT_IMEM_END',
  'BOOT_IMEM_SIZE',
]

base_resize = {
  'BLSP1_BLSP':                           0x39000,
  'RPM_SS_MSG_RAM_START_ADDRESS':         0x3000,
  'A7SS_L2':                              0x40000,
}


HWIO_BASE_FILES = [
  {
    'filename': '../../../api/systemdrivers/hwio/' + CHIPSET + '/msmhwiobase.h',
    'bases': bases,
    'filter-exclude': bases_exclude,
    'map-type': 'physical',
    'virtual-address-start': 0x10000000,
    'virtual-address-end': 0x60000000,
    'resize': base_resize,
    'l4-memsection-filename': '../../../api/systemdrivers/hwio/' + CHIPSET + '/msmhwioplat_memsection.xml',
    'l4-machine-filename': '../../../api/systemdrivers/hwio/' + CHIPSET + '/msmhwioplat_machine.xml',
    'default-cache-policy': 'device',
    'devcfg-filename': '../config/' + CHIPSET + '/HWIOBaseMap.c',
    'check-sizes': True,
    'check-for-overlaps': True,
  }
]


# ============================================================================
# HWIO_REGISTER_FILES
# ============================================================================

HWIO_REGISTER_FILES = [
  {
    'filename': '../hw/' + CHIPSET + '/msmhwioreg.h.ref',
    'bases': bases,
    'filter-exclude': ['RESERVED', 'DUMMY'],
    'header': '''
#error This file is for reference only and cannot be included.  See "go/hwio" or mail corebsp.sysdrv.hwio for help.
'''
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
    'filter-exclude': ['RESERVED', 'DUMMY']
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
