#==============================================================================
# @file Mdm9x05Pkg_LD_Variant.dscinc
# Mdm9x05Pkg Loader package.
#
# Copyright (c) 2019, 2022 Qualcomm Technologies, Inc. All rights reserved.
# Portions Copyright (c) 2009 - 2010, Apple Inc. All rights reserved.
# This program and the accompanying materials
# are licensed and made available under the terms and conditions of the BSD License
# which accompanies this distribution.  The full text of the license may be found at
# http://opensource.org/licenses/bsd-license.php
#
# THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
# WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#==============================================================================

[BuildOptions.ARM]
  GCC:*_*_ARM_ARCHCC_FLAGS  == -fno-common -mtune=cortex-a53 -I$(WORKSPACE)/QcomPkg/Mdm9x05Pkg/Include -I$(WORKSPACE)/QcomPkg/Include/api -include $(WORKSPACE)/QcomPkg/Mdm9x05Pkg/Include/Target_cust.h -DFEATURE_FOTA_BOOT -DFEATURE_BOOT_NAND_BOOT -DMDEPKG_NDEBUG -DQCOM_EDK2_PATCH $(FEATURE_FLAGS) -DQCOM_XBL_MINIDUMP -DPON1_SUPPORT -DZIGGY_WAKEUP -DFOTA_SUPPORT -DQUSB_FEDL -DKEY_PRESS_DBNC -DAWARE_SMB_CONFIG
  GCC:*_*_ARM_ARCHASM_FLAGS  == -I$(WORKSPACE)/QcomPkg/Mdm9x05Pkg/Include -I$(WORKSPACE)/QcomPkg/Include/api -include $(WORKSPACE)/QcomPkg/Mdm9x05Pkg/Include/Target_cust.h -DFEATURE_BOOT_NAND_BOOT -DMDEPKG_NDEBUG -DQCOM_EDK2_PATCH $(FEATURE_FLAGS)
