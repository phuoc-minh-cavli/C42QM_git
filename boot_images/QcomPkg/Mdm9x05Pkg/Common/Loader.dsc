#==============================================================================
# @file Mdm9x05Pkg_Loader.dsc
# 
#
# Copyright (c) 2015 - 2019 Qualcomm Technologies, Inc. All rights reserved.
# Portions Copyright (c) 2009 - 2010, Apple Inc. All rights reserved.
# This program and the accompanying materials
# are licensed and made available under the terms and conditions of the BSD License
# which accompanies this distribution.  The full text of the license may be found at
# http://opensource.org/licenses/bsd-license.php
#
# THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
# WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
#==============================================================================
#                              EDIT HISTORY
#
#
# when       who     what, where, why
# --------   ---     ----------------------------------------------------------
# 09/12/19   rrawat  Added support for variants
# 01/21/19   ss      HSUart Enablement on Twizy(MDM9205)
# 12/21/18   as      Enabled DevPrgN compilation for DCPU mode
# 12/11/18   aus     Added BootInterruptLib
# 12/04/18   rohik   Added support for FOTA feature
# 02/23/18   svl     added EEPROM tools support
# 02/20/18   svl     added nand tools support
# 01/30/18   vg      Updated  i2c instance/saddr for eeprom 
# 12/28/17   rohik   Disabled compilation of UFS driver libraries for Vipertooth
# 05/03/17   czq     Updated PcdPsHoldAddress
# 03/22/17   bd      Add compiler flag to disable minidump
# 01/30/17   vg      Updated the EEPROM QUP core
# 01/18/17   sb      Enable hs200 mode in loader
# 01/02/17   sb      Updated device programmer elf names for starlord
# 10/24/16   yps     Clean unused libs
# 10/27/16   PR		 ADD UartSettingsLib
# 08/17/16   yps     Remove Sec lib
# 09/22/16   vg      Added I2C lib 
# 10/05/16   arung   Add UsbSharedLib
# 08/10/16   yps     Fixed Noc issue
# 07/21/16   yps     upgrade compiler to 3.8.4
# 07/08/16   rp      Added DDI support
# 07/23/16   axu     Added DisplayLib
# 06/23/16   kpa     Added FontLib, BootDisplayLib
# 06/22/16   bc      Add ClockRamDumpLib
# 06/20/16   aab     Added PmicLoaderLib
# 06/08/16   sj      removed the mass storage cookie references
# 05/09/16   digant  LoaderAPTLib enabled
# 04/20/16   et      Conditionalized UFS rumi and target lib loading
# 03/28/16   ss      Added BAM support
# 03/28/16   kpa     remove PRE_SIL flag defines
# 03/10/16   aab     Adding PmicLib for XBLRamDumpLib
# 03/09/16   va      updating PmicCoreLib location for loader
# 02/04/16   ml      Removed display dependencies from loader
# 01/28/16   jt      Add SDCC libs
# 01/18/16   eo      Reduced sdcc code size
# 01/14/16   et      Updated device programmer elf names
# 01/12/16   kameya  Updated QUSB Libraries
# 01/08/16   et      Merge tags for buildit
# 12/22/15   plc     Enable ImageVersion library
# 12/15/15   bh      Settings relocation
# 12/02/15   jt      Rename SdccLibNull
# 11/20/15   kpa     Added SecImgAuthTargetLibNull
# 10/27/15   vk      Add stack canary
# 10/07/15   plc     Mainline as much RUMI functionality as possible
# 09/30/15   ck      Moved null libs to common directory
# 09/28/15   rli     Retire MpPark
# 09/03/15   tselvam PcieConfigLib support added
# 08/27/15   plc     Bring up to date with 8996 tip
# 08/20/15   ap      Enabled DALConfigLib, DALConfigLoaderLib and DALConfigToolsLib
# 08/07/15   plc     Update AC Timer base
# 06/08/15   sk      Add HSTI
# 06/12/15   plc     Pulling in below changes from tip
# 06/04/15   dg      Add Null ReportStatusCodeLib
# 05/18/15   bh      Remove LoaderUtilsLib
# 04/30/15   plc     Initial Revision
#
#==============================================================================
################################################################################
#
# Defines Section - statements that will be processed to create a Makefile.
#
################################################################################
[Defines]
  PLATFORM_NAME                  = Mdm9x05Pkg
  PLATFORM_GUID                  = 88862AD2-D41F-44E2-9514-513B9F97A416
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x00010005
  SUPPORTED_ARCHITECTURES        = ARM|AARCH64
  BUILD_TARGETS                  = DEBUG|RELEASE
  SKUID_IDENTIFIER               = DEFAULT
  OUTPUT_DIRECTORY               = Build/Mdm9x05$(VAR)_Loader
 
[LibraryClasses.common]
  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
  BaseMemoryLib|ArmPkg/Library/BaseMemoryLibStm/BaseMemoryLibStm.inf

  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
  ArmMmuLib|ArmPkg/Library/ArmMmuLib/ArmMmuBaseLib.inf
  PeCoffExtraActionLib|ArmPkg/Library/DebugPeCoffExtraActionLib/DebugPeCoffExtraActionLib.inf
  CacheMaintenanceLib|ArmPkg/Library/ArmCacheMaintenanceLib/ArmCacheMaintenanceLib.inf
  PrePiLib|EmbeddedPkg/Library/PrePiLib/PrePiLib.inf

  IoLib|MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf

#
# Assume everything is fixed at build
#
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  UefiUsbLib|MdePkg/Library/UefiUsbLib/UefiUsbLib.inf

  # Qcom Generic Libraries
  NULL|QcomPkg/Library/StackCanaryBaseLib/StackCanaryBaseLibLoader.inf
  QcomUtilsLib|QcomPkg/Library/QcomUtilsLib/QcomUtilsLib.inf
  TimetickRunTimeLib|QcomPkg/Library/TimetickLibB/TimetickRunTimeLib.inf
  TimetickLib|QcomPkg/Library/TimetickLibB/TimetickLib.inf
  TimerLib|ArmPkg/Library/ArmArchTimerLib/ArmArchTimerLib.inf
  TimerSleepLib|QcomPkg/Library/TimerSleepLib/TimerSleepLib.inf
  ArmGenericTimerCounterLib|ArmPkg/Library/ArmGenericTimerPhyCounterLib/ArmGenericTimerPhyCounterLib.inf
  SmemLib|QcomPkg/Library/SmemLib/build/SmemLibXBLLoader.inf
  RealTimeClockLib|QcomPkg/Library/RealTimeClockLibLegacy/RealTimeClockLib.inf
  SerialPortLib|QcomPkg/Library/SerialPortLib/SerialPortShLib.inf
  SecBootSigLib|QcomPkg/Library/SecBootSigLib/SecBootSigLib.inf
  NULL|QcomPkg/Library/QcomBaseLib/QcomBaseLib.inf
  XBLSharedMemLib|QcomPkg/Library/XBLSharedMemLib/XBLSharedMemLib.inf
  TzRuntimeLib|QcomPkg/Library/TzRuntimeLib/TzRuntimeLib.inf
  TzRngLib|QcomPkg/Library/TzRngLib/TzRngLib.inf
  SdccCommonLib|QcomPkg/Library/NullLibs/SdccLibNull/SdccCommonLibNull.inf
  SdccHalLib|QcomPkg/Library/NullLibs/SdccLibNull/SdccCommonLibNull.inf

  #-----NAND driver Library-----{
  NandTargetLib|QcomPkg/Mdm9x05Pkg/Library/NandTargetLib/NandTargetLib.inf
  NandHalLoaderLib|QcomPkg/Library/FlashCommonLib/NandHalLib/NandHalLoaderLib.inf
  FlashCommonLoaderLib|QcomPkg/Library/FlashCommonLib/FlashCommonLoaderLib.inf
  #}

  #-----NandJtagTools-----{
  NandToolsLib|QcomPkg/Library/FlashCommonLib/FlashCommonNandToolsLib.inf
  NandHalToolsLib|QcomPkg/Library/FlashCommonLib/NandHalLib/NandHalToolsLib.inf
  #}

  #-----NorJtagTools-----{
  NorToolsLib|QcomPkg/Library/FlashCommonLib/FlashCommonNorToolsLib.inf
  #}

  UfsCommonLib|QcomPkg/Library/NullLibs/UfsTargetLibNull/UfsCommonLibNull.inf
  UfsLoaderLib|QcomPkg/Library/NullLibs/UfsTargetLibNull/UfsLoaderLibNull.inf
  UfsJtagLib|QcomPkg/Library/NullLibs/UfsTargetLibNull/UfsJtagLibNull.inf
  QusbLib|QcomPkg/Library/QusbLib/QusbLib.inf
  SecCfgLib|QcomPkg/Library/SecCfgLib/SecCfgLib.inf
  HotPlugLib|QcomPkg/Library/NullLibs/HotPlugLibNull/HotPlugLibNull.inf
  HotPlugEfsLib|QcomPkg/Library/NullLibs/HotPlugLibNull/HotPlugLibNull.inf
  HFATLib|QcomPkg/Library/HFATLib/HFATLib.inf
  ROMFSLib|QcomPkg/Library/EfsLib/ROMFSLib.inf
  EfsUtilsLib|QcomPkg/Library/EfsLib/EfsUtilsLib.inf
  EfsCookieBootLib|QcomPkg/Library/EfsLib/EfsCookieBootLib.inf
  ServicesUtilsLib|QcomPkg/Library/ServicesLib/ServicesUtilsLib.inf
  PlatformInfoLoaderLib|QcomPkg/Library/PlatformInfoLib/PlatformInfoLoader.inf
  ChipInfoLoaderLib|QcomPkg/Library/ChipInfoLib/ChipInfoLoader.inf
  RpmbCgLib|QcomPkg/Library/RpmbCgLib/RpmbCgLib.inf
  SecHwTestLib|QcomPkg/Library/SecHwTestLib/SecHwTestLib.inf
  SecBootLib|QcomPkg/Library/SecBootLib/SecBootLib.inf
  SecBootBaseLib|QcomPkg/Library/SecBootBaseLib/SecBootBaseLib.inf
  PeHashLib|QcomPkg/Library/PeHashLib/PeHashLib.inf
  TargetResetLib|QcomPkg/Library/NullLibs/TargetResetLibNull/TargetResetLib.inf
  TLMMTargetLib|QcomPkg/Mdm9x05Pkg/Library/TLMMTargetLib/TLMMTargetLib.inf
  ImageVersionLib|QcomPkg/Library/ImageVersionLib/ImageVersionLib.inf
  PmicLib|QcomPkg/Mdm9x05Pkg/Library/PmicLib/PmicLib.inf
  PmicLoaderLib|QcomPkg/Mdm9x05Pkg/Library/PmicLib/loader/PmicLoaderLib.inf
  SecBootAuthLib|QcomPkg/Library/NullLibs/SecBootAuthLibNull/SecBootAuthLibNull.inf
  SecImgAuthTargetLib|QcomPkg/Mdm9x05Pkg/Library/SecImgAuthTargetLib/SecImgAuthTargetLib.inf
  PrngLib|QcomPkg/Mdm9x05Pkg/Library/RNGLibLoader/RNGLibHw.inf
  HashLib|QcomPkg/Mdm9x05Pkg/Library/HashLibLoader/HashLibHw.inf
  RNGLib|QcomPkg/Mdm9x05Pkg/Library/RNGLibLoader/RNGLibHw.inf
  CryptoLib|QcomPkg/Mdm9x05Pkg/Library/CryptoLib/CryptoLib.inf
  AptLib|QcomPkg/Library/NullLibs/AptLibNull/AptLibNull.inf
  CryptoRDILib|QcomPkg/Mdm9x05Pkg/Library/CryptoInterfaceLib/CryptoRDILib.inf 
  CryptoLoaderILib|QcomPkg/Mdm9x05Pkg/Library/CryptoInterfaceLib/CryptoLoaderILib.inf  

  # Target Libraries
  XBLLoaderLib|QcomPkg/Mdm9x05Pkg/Library/XBLLoaderLib/XBLLoaderLib.inf
  XBLCommonLib|QcomPkg/Mdm9x05Pkg/Library/XBLLoaderLib/XBLCommonLib.inf
  DDITargetLib|QcomPkg/Mdm9x05Pkg/Library/DDITargetLib/DDITargetLib.inf
  DDRTargetLib|QcomPkg/Mdm9x05Pkg/Library/DDRTargetLib/DDRTargetLib.inf
  DSFTargetLib|QcomPkg/Mdm9x05Pkg/Library/DSFTargetLib/DSFTargetLib.inf
  #ClockTargetLib|QcomPkg/Library/NullLibs/ClockLibNull/ClockLibNull.inf
  ClockTargetLib|QcomPkg/Mdm9x05Pkg/Library/ClockTargetLib/ClockTargetLib.inf
  HotPlugLib|QcomPkg/Library/NullLibs/HotPlugLibNull/HotPlugLibNull.inf
  HFATLib|QcomPkg/Library/HFATLib/HFATLib.inf
  PmicApiLib|QcomPkg/Library/PmicApiLib/PmicApiLib.inf
  HotPlugLib|QcomPkg/Library/NullLibs/HotPlugLibNull/HotPlugLibNull.inf
  HFATLib|QcomPkg/Library/HFATLib/HFATLib.inf
  SdccClockLib|QcomPkg/Mdm9x05Pkg/Library/SdccClockLib/SdccCoreClockLib.inf
  #UfsTargetBootLib|QcomPkg/Mdm9x05Pkg/Library/UfsTargetLib/UfsTargetBootLib.inf
  #UfsTargetJtagLib|QcomPkg/Mdm9x05Pkg/Library/UfsTargetLib/UfsTargetJtagLib.inf
  #UfsTargetUefiLib|QcomPkg/Mdm9x05Pkg/Library/UfsTargetLib/UfsTargetUefiLib.inf
  HashLib|QcomPkg/Mdm9x05Pkg/Library/HashLibLoader/HashLibHw.inf
  IcbCfgLib|QcomPkg/Library/ICBLib/ICBLib.inf
  HALqgicLib|QcomPkg/Library/HalqgicLib/HALqgicLibLoader.inf
  InterruptGicLib|QcomPkg/Library/InterruptGicLib/InterruptGicLib.inf
  IcbTargetLib|QcomPkg/Mdm9x05Pkg/Library/ICBTargetLib/ICBTargetLib.inf
  UsbWrapperLayerLib|QcomPkg/Mdm9x05Pkg/Library/UsbWrapperLayerLib/UsbWrapperLayerLib.inf
  BusyWaitLib|QcomPkg/Library/BusyWaitLib/BusyWaitLib.inf
  TlmmLib|QcomPkg/Library/TlmmLib/TlmmLib.inf
  HALclkLib|QcomPkg/Mdm9x05Pkg/Library/HALclkLib/HALclkLibXBL.inf
  HALclkSharedLib|QcomPkg/Library/HALclkSharedLib/HALclkSharedLibUEFI.inf
  PmicShutdownLibBoot|QcomPkg/Mdm9x05Pkg/Library/PmicLib/core/shutdown/pm_shutdown.inf
  PmicShutdownRuntimeLib|QcomPkg/Mdm9x05Pkg/Library/PmicLib/core/shutdown/pm_runtime_shutdown.inf
  QusbDciCommonLib|QcomPkg/Mdm9x05Pkg/Library/QusbDciCommonLib/QusbDciCommonLib.inf
  UsbSharedLib|QcomPkg/Mdm9x05Pkg/Library/UsbSharedLib/UsbSharedLibLoader.inf
#  SpiNorCommonLib|QcomPkg/Library/SpiNorCommonLib/SpiNorCommonLib.inf
  BootConfigLib|QcomPkg/Mdm9x05Pkg/Library/BootConfigLib/BootConfigLib.inf
  LoaderCacheLib|QcomPkg/XBLLoader/LoaderCacheLib.inf
  SmemTargetLib|QcomPkg/Mdm9x05Pkg/Library/SmemTargetLib/SmemTargetLib.inf
  FontLib|QcomPkg/Library/FontLib/FontLib.inf
  BootDisplayLib|QcomPkg/Library/BootDisplayLib/BootDisplayLib.inf
  DisplayLib|QcomPkg/Library/DisplayLib/DisplayLib.inf

  BootInterruptLib|QcomPkg/Library/BootInterruptLib/BootInterruptLib.inf
  
  # DAL Libraries
  DALModEnvLib|QcomPkg/Library/DALModEnvLib/DALModEnvLib.inf
  DALCommonLoaderLib|QcomPkg/Library/DALCommonLib/DALCommonLoaderLib.inf
  DALFwkLib|QcomPkg/Library/DALFwkLib/DALFwkLib.inf

  DALGenLibLoader|QcomPkg/Mdm9x05Pkg/Settings/DALConfig/DALGenLibLoader.inf
  DALGenLibTools|QcomPkg/Mdm9x05Pkg/Settings/DALConfig/DALGenLibTools.inf

  DALConfigLib|QcomPkg/Mdm9x05Pkg/Settings/DALConfig/DALConfigCoreLib.inf
  DALConfigLoaderLib|QcomPkg/Mdm9x05Pkg/Settings/DALConfig/DALConfigLoaderLib.inf
  DALConfigToolsLib | QcomPkg/Mdm9x05Pkg/Settings/DALConfig/DALConfigToolsLib.inf

  # Test Interface Libraries

  # HAL Libraries
  # Display HAL Libraries
  HALDSILib|QcomPkg/Library/HALDSILib/HALDSILib.inf
  ExternalDisplayLib|QcomPkg/Library/ExternalDisplayLib/ExternalDisplayLib.inf
  HALDPLib|QcomPkg/Library/HALDPLib/HALDPLibStub.inf  
  HALHDMILib|QcomPkg/Library/HALHDMILib/HALHDMILibStub.inf
  HALMDSSLib|QcomPkg/Library/HALMDSSLib/HALMDSS3xx.inf
  # MDP Libraries
  MDPLibBoot|QcomPkg/Library/MDPLib/MDPLibBoot.inf
  MDPPlatformLibBoot|QcomPkg/Mdm9x05Pkg/Library/MDPPlatformLibBoot/MDPPlatformLibBoot.inf

  # BAM Lib Library
  BamLib|QcomPkg/Library/BamLib/BamLoaderLib.inf
  BamLoaderLib|QcomPkg/Library/BamLib/BamLoaderLib.inf
  BamTargetLib|QcomPkg/Mdm9x05Pkg/Library/BamTargetLib/BamTargetLib.inf
  

  # RFS Lib Library
  RFSLib|QcomPkg/Library/RFSLib/RFSLib.inf

  # ULog Libraries
  ULogApiLib|QcomPkg/Library/ULogApiLib/ULogApiLib.inf
  CoreUtilsLib|QcomPkg/Library/CoreUtilsLib/CoreUtilsLib.inf
  
  # Sleep Libraries
  SleepLib|QcomPkg/Library/SleepLib/SleepLib.inf
  
  # Railway Libraries - These are internal chipset specific clock libraries.
  #                   They should only be used by the Railway DXE driver.
  RailwayLib|QcomPkg/Library/RailwayLib/RailwayLib.inf
  RailwayTargetLib|QcomPkg/Mdm9x05Pkg/Library/RailwayTargetLib/RailwayTargetLib.inf

  # SPMI Library
  SPMILib|QcomPkg/Mdm9x05Pkg/Library/SPMILib/SPMILib.inf
  SPMIApiLib|QcomPkg/Library/SPMIApiLib/SPMIApiLib.inf

  # I2C Library
  I2CLib|QcomPkg/Library/I2CLib/I2CLib.inf
  I2CApiLib|QcomPkg/Library/I2CApiLib/I2CApiLib.inf
  SPILib|QcomPkg/Library/SPILib/SPILib.inf
  SPIApiLib|QcomPkg/Library/SPIApiLib/SPIApiLib.inf

  # ADC Libraries
  AdcLoaderLib|QcomPkg/Library/AdcLib/build/AdcLoaderLib.inf

  # TSENS Libraries
  TsensTargetLib|QcomPkg/Mdm9x05Pkg/Library/TsensTargetLib/TsensTargetLib.inf
  TsensLoaderLib|QcomPkg/Library/TsensLib/TsensLoaderLib.inf
  BootTempCheckLib|QcomPkg/Library/BootTempCheckLib/BootTempCheckLib.inf

  # MuslLib: POSIX string APIs
  MuslLib|QcomPkg/Library/MuslLib/MuslLib.inf
  NULL|QcomPkg/Library/MuslLib/MuslLib.inf
  
  # UART settings Library
  UartSettingsLib|QcomPkg/Mdm9x05Pkg/Settings/UART/UartSettingsLib.inf

[LibraryClasses.AARCH64]
  ArmLib|ArmPkg/Library/ArmLib/AArch64/AArch64Lib.inf

[LibraryClasses.ARM]
  ArmLib|ArmPkg/Library/ArmLib/ArmV7/ArmV7Lib.inf
  
[LibraryClasses.common.SEC]
  ProcLib|QcomPkg/Library/ProcLib/ProcLib.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  ReportStatusCodeLib|QcomPkg/Library/NullLibs/PeiReportStatusCodeLibFrameworkNull/PeiReportStatusCodeLibNull.inf
  UefiDecompressLib|MdePkg/Library/BaseUefiDecompressLib/BaseUefiDecompressLib.inf
  ExtractGuidedSectionLib|EmbeddedPkg/Library/PrePiExtractGuidedSectionLib/PrePiExtractGuidedSectionLib.inf
  PeCoffLib|MdePkg/Library/BasePeCoffLib/BasePeCoffLib.inf
  HobLib|EmbeddedPkg/Library/PrePiHobLib/PrePiHobLib.inf
  PrePiHobListPointerLib|EmbeddedPkg/Library/PrePiHobListPointerLib/PrePiHobListPointerLib.inf
  MemoryAllocationLib|EmbeddedPkg/Library/PrePiMemoryAllocationLib/PrePiMemoryAllocationLib.inf 
  PerformanceLib|MdeModulePkg/Library/PeiPerformanceLib/PeiPerformanceLib.inf
  ShLib|QcomPkg/Library/ShLib/ShLibMgr.inf
  SerialPortLib|QcomPkg/Library/SerialPortLib/SerialPortLib.inf
  HSUartLib|QcomPkg/Library/HSUartLib/HSUartLib.inf

  # Target SEC Libraries
  SdccClockLib|QcomPkg/Library/NullLibs/SdccLibNull/SdccCommonLibNull.inf
  SdccTargetLib|QcomPkg/Library/NullLibs/SdccLibNull/SdccCommonLibNull.inf

  DebugLib|QcomPkg/Library/DebugLib/DebugLib.inf

[LibraryClasses.ARM.SEC]
  ArmLib|ArmPkg/Library/ArmLib/ArmV7/ArmV7LibPrePi.inf

[LibraryClasses.AARCH64.SEC]
  ArmLib|ArmPkg/Library/ArmLib/AArch64/AArch64LibPrePi.inf

[LibraryClasses]
  NULL|ArmPkg/Library/CompilerIntrinsicsLib/CompilerIntrinsicsLib.inf

!include QcomPkg/Mdm9x05Pkg/$(VAR)/Variant.dsc.inc

[BuildOptions.AARCH64]
  GCC:*_*_AARCH64_ARCHCC_FLAGS  == -mllvm -inline-threshold-multicaller-size=40 -mtune=cortex-a53 -I $(WORKSPACE)/QcomPkg/Mdm9x05Pkg/Include -include $(WORKSPACE)/QcomPkg/Mdm9x05Pkg/Include/Target_cust.h -DMDEPKG_NDEBUG -DQCOM_EDK2_PATCH $(FEATURE_FLAGS) -DQCOM_XBL_MINIDUMP
  GCC:*_*_AARCH64_ARCHPP_FLAGS  == -I $(WORKSPACE)/QcomPkg/Mdm9x05Pkg/Include -include $(WORKSPACE)/QcomPkg/Mdm9x05Pkg/Include/Target_cust.h -DMDEPKG_NDEBUG -DQCOM_EDK2_PATCH $(FEATURE_FLAGS)

!if $(PRODMODE) == "PRODMODE"
  RVCT:*_*_*_CC_FLAGS = -DPRODMODE
  GCC:*_*_*_CC_FLAGS = -DPRODMODE
!endif

################################################################################
#
# Pcd Section - list of all EDK II PCD Entries defined by this Platform
#
################################################################################

[PcdsFeatureFlag.common]
  gEmbeddedTokenSpaceGuid.PcdCacheEnable|TRUE
  gEmbeddedTokenSpaceGuid.PcdPrePiProduceMemoryTypeInformationHob|TRUE
  gArmTokenSpaceGuid.PcdCpuDxeProduceDebugSupport|FALSE
  gEfiMdeModulePkgTokenSpaceGuid.PcdTurnOffUsbLegacySupport|TRUE

[PcdsFixedAtBuild.common]
  gEfiMdeModulePkgTokenSpaceGuid.PcdFirmwareVendor|L"Qualcomm Technologies, Inc."
  gEfiMdeModulePkgTokenSpaceGuid.PcdFirmwareVersionString|L"4.00"

  gEmbeddedTokenSpaceGuid.PcdPrePiCpuMemorySize|48
  gEmbeddedTokenSpaceGuid.PcdPrePiCpuIoSize|0
  gEfiMdePkgTokenSpaceGuid.PcdMaximumUnicodeStringLength|1000000
  gEfiMdePkgTokenSpaceGuid.PcdMaximumAsciiStringLength|1000000
  gEfiMdePkgTokenSpaceGuid.PcdMaximumLinkedListLength|1000000
  gEfiMdePkgTokenSpaceGuid.PcdSpinLockTimeout|10000000
  gEfiMdePkgTokenSpaceGuid.PcdDebugClearMemoryValue|0xAF

  # Disable perf
  gEfiMdePkgTokenSpaceGuid.PcdPerformanceLibraryPropertyMask|0
  gEfiMdePkgTokenSpaceGuid.PcdPostCodePropertyMask|0
  gEfiMdePkgTokenSpaceGuid.PcdUefiLibMaxPrintBufferSize|320

# DEBUG_ASSERT_ENABLED       0x01
# DEBUG_PRINT_ENABLED        0x02
# DEBUG_CODE_ENABLED         0x04
# CLEAR_MEMORY_ENABLED       0x08
# ASSERT_BREAKPOINT_ENABLED  0x10
# ASSERT_DEADLOOP_ENABLED    0x20

!if $(PRODMODE) == "PRODMODE"
  gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x2b
!else
  gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x2f
!endif

#  DEBUG_INIT      0x00000001  // Initialization
#  DEBUG_WARN      0x00000002  // Warnings
#  DEBUG_LOAD      0x00000004  // Load events
#  DEBUG_FS        0x00000008  // EFI File system
#  DEBUG_POOL      0x00000010  // Alloc & Free's
#  DEBUG_PAGE      0x00000020  // Alloc & Free's
#  DEBUG_INFO      0x00000040  // Verbose
#  DEBUG_DISPATCH  0x00000080  // PEI/DXE Dispatchers
#  DEBUG_VARIABLE  0x00000100  // Variable
#  DEBUG_BM        0x00000400  // Boot Manager
#  DEBUG_BLKIO     0x00001000  // BlkIo Driver
#  DEBUG_NET       0x00004000  // SNI Driver
#  DEBUG_UNDI      0x00010000  // UNDI Driver
#  DEBUG_LOADFILE  0x00020000  // UNDI Driver
#  DEBUG_EVENT     0x00080000  // Event messages
#  DEBUG_GCD       0x00100000  // Global Coherency Database changes
#  DEBUG_CACHE     0x00200000  // Memory range cachability changes
#  DEBUG_VERBOSE   0x00400000  // Detailed debug messages that may significantly impact boot performance
#  DEBUG_ERROR     0x80000000  // Error

  # Production Mode Flag
!if $(PRODMODE) == "PRODMODE"
  # Only enable errors, warnings, and Boot Manager
  gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80000400
!else
  gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x800fee0f
!endif

  gEfiMdePkgTokenSpaceGuid.PcdReportStatusCodePropertyMask|0x07

  gEmbeddedTokenSpaceGuid.PcdEmbeddedAutomaticBootCommand|""
  gEmbeddedTokenSpaceGuid.PcdEmbeddedDefaultTextColor|0x07
  gEmbeddedTokenSpaceGuid.PcdEmbeddedMemVariableStoreSize|0x10000

  gEmbeddedTokenSpaceGuid.PcdFlashFvMainBase|0
  gEmbeddedTokenSpaceGuid.PcdFlashFvMainSize|0

#
# Target Specific PCDs
#

  #########################################################################
  #                               Memory Map
  #########################################################################

  gQcomTokenSpaceGuid.PcdMaxMemory|0xFFFFFFFF

  #################################
  ## Presilicon definitions.
  #################################
  gQcomTokenSpaceGuid.PcdMemoryBase|0x80000000    # Starting address
  gQcomTokenSpaceGuid.PcdMemorySize|0x20000000    # .5 GB

  #################################
  ## HLOS Base Offset and size.
  #################################
  gQcomTokenSpaceGuid.PcdHLOSMemoryBaseOffset|0x90000000 # remaining memory after this is HLOS

  #################################
  ## UEFI Memory Base Offset and Size.
  #################################
  gQcomTokenSpaceGuid.PcdUefiMemPoolBaseOffset|0x80D00000
  gQcomTokenSpaceGuid.PcdUefiMemPoolSize|0x03300000     # 51 MB

  # The HOB list will start here.
  # NOTE: Hob.c is still referencing PcdPrePiHobBase
  gQcomTokenSpaceGuid.PcdPrePiHobBase|0x80D00000

  # Offset to UEFI memory
  gQcomTokenSpaceGuid.PcdPreAllocatedMemorySize|0x00D00000

  #################################
  ## Stack Base Address and size.
  ## TODO: PcdPrePiStackBase is currently used by ModuleEntryPoint.asm
  ##################################
  gEmbeddedTokenSpaceGuid.PcdPrePiStackBase|0x80C00000
  gEmbeddedTokenSpaceGuid.PcdPrePiStackSize|0x00040000  # 256K stack


  #################################
  ## FD Base offset (refer to .fdf for FD size)
  #################################
  gQcomTokenSpaceGuid.PcdEmbeddedFdBaseAddress|0x80200000

  #################################
  ## Shared Memory Base offset and size.
  ## TODO: PcdSmemBaseAddress is currently used by smem library
  #################################
  gQcomTokenSpaceGuid.PcdSmemBaseAddress|0x81E52000  #64-bit smem addr
  gQcomTokenSpaceGuid.PcdSmemSize|0x0006E000         #440KB
  ## Generic SMEM information that SMEM lib might need
  ## LSB represents chip family, other bytes are unused for now.
  gQcomTokenSpaceGuid.PcdSmemInformation|0x00000005  # 5 = Nazgul

  # Maximum number of memory regions defined in the above memory map
  gQcomTokenSpaceGuid.PcdMaxMemRegions|64

  #################################
  ## IMem Base Address and size.
  #################################

  gQcomTokenSpaceGuid.PcdIMemCookiesBase|0x066BF000
  gQcomTokenSpaceGuid.PcdIMemCookiesSize|0x00001000

  # Uefi Debug Cookie Address and value
  gQcomTokenSpaceGuid.PcdUefiDebugCookieOffset|0x944
  gQcomTokenSpaceGuid.PcdUefiDebugCookie|0x55444247

  gArmTokenSpaceGuid.PcdCpuResetAddress|0x08000000    # warm reset should use this

  # Secure boot efuse register address
  gQcomTokenSpaceGuid.PcdSecbootRegAddr|0x0005E030
  gQcomTokenSpaceGuid.PcdSecbootAuthEnMask|0x20


  # Timer Debug
  gQcomTokenSpaceGuid.PcdTimerDxeDebug|FALSE

  # Timers
  gEmbeddedTokenSpaceGuid.PcdInterruptBaseAddress|0x0B000000

  # Timers

  gArmTokenSpaceGuid.PcdArmArchTimerFreqInHz|19200000

  gArmTokenSpaceGuid.PcdGicDistributorBase|0x0B000000
  gArmTokenSpaceGuid.PcdGicInterruptInterfaceBase|0x0B002000

  gQcomTokenSpaceGuid.PcdInterruptTimerIndex|3
  gQcomTokenSpaceGuid.PcdInterruptTimerPeriod|100000
  gQcomTokenSpaceGuid.PcdTimer1IRQ|0x28
  gQcomTokenSpaceGuid.PcdTimerTestClockFreq|918000000 # Pcycles runs at 918 MHz
  gQcomTokenSpaceGuid.PcdTimerTestClockFreqDiv|64 # Pcycles divided by value
  gQcomTokenSpaceGuid.PcdQGICCBaseAddress|0x0B002000
  gQcomTokenSpaceGuid.PcdQTimerAC_BasePhys|0x0B020000
  gQcomTokenSpaceGuid.PcdQTimerV1BasePhys|0x0B021000
  gQcomTokenSpaceGuid.PcdQTimerV2BasePhys|0x0B022000
  gQcomTokenSpaceGuid.PcdQTimerFrame|0
  gQcomTokenSpaceGuid.PcdQTimerEnabled|1
  gQcomTokenSpaceGuid.PcdPerfomanceCounterEnd|0xFFFFFFFF
  gQcomTokenSpaceGuid.PcdPerfomanceCounterStart|0x00000000
  gQcomTokenSpaceGuid.PcdInterruptVectorDGT|17
  gQcomTokenSpaceGuid.PcdInterruptVectorGPT0|18
  gQcomTokenSpaceGuid.PcdInterruptVectorGPT1|19
  gQcomTokenSpaceGuid.PcdPerformanceCounterIndex|3
  gQcomTokenSpaceGuid.PcdRealTimeClockResolution|1
  gQcomTokenSpaceGuid.PcdRealTimeClockAccuracy|200000000
  gQcomTokenSpaceGuid.PcdRealTimeClockSetToZero|1


  #MPM_SLEEP_TIMETICK_COUNT_VAL for time from power on
  gQcomTokenSpaceGuid.PcdTimeTickClkAddress|0x004A3000

  gQcomTokenSpaceGuid.PcdPsHoldAddress|0x10AC000

  #I2C
  gQcomTokenSpaceGuid.EEPROMI2CCore|2
  gQcomTokenSpaceGuid.EEPROMSlaveAddress|0x56

  # SPMI
  gQcomTokenSpaceGuid.SPMIBasePhysical0|0x02000000
  gQcomTokenSpaceGuid.SPMIBasePhysical1|0
  gQcomTokenSpaceGuid.SPMIMemoryMapSize|0x1A00
  gQcomTokenSpaceGuid.SPMIOwner|0
  gQcomTokenSpaceGuid.SPMIDebugChannel|1

  # SDCC
  gQcomTokenSpaceGuid.SDCCExtSlotNumber|1
  gQcomTokenSpaceGuid.SDCCEmmcSlotNumber|0
  gQcomTokenSpaceGuid.SDCCRpmbKey|{0x20, 0xAA, 0x9C, 0xF4, 0x99, 0x4D, 0xD4, 0xFE, 0xA5, 0x85, 0xBE, 0x96, 0x6, 0x1, 0xD1, 0xA9, 0xC3, 0x3, 0x4F, 0x91, 0x62, 0x7C, 0x64, 0x53, 0x38, 0xC1, 0x1F, 0xF2, 0x76, 0x4D, 0x2E, 0xC4}
  gQcomTokenSpaceGuid.SDCCEventBasedSDDetectEnabled|FALSE
  gQcomTokenSpaceGuid.SDCCDmaEnabled|TRUE
  gQcomTokenSpaceGuid.SDCCEmmcHwResetEnabled|TRUE
  gQcomTokenSpaceGuid.SDCCMaxSlotNumber|2
  ## SDCC eMMC speed mode - DDR50: 0, HS200: 1, HS400: 2 SDR: 3 ##
  gQcomTokenSpaceGuid.SDCCEmmcSpeedMode|2

  #
  # ARM Pcds
  #
  gArmTokenSpaceGuid.PcdArmUncachedMemoryMask|0x0000000040000000

  # Timer sync not required
  gQcomTokenSpaceGuid.PcdSyncTimerToMPP|FALSE

  # UART
  gQcomTokenSpaceGuid.UartPlatform|"Mdm9x05"

!if $(PRODMODE) == "PRODMODE"
  # Version info needed on LCD Display
  gQcomTokenSpaceGuid.VersionDisplay|FALSE
!else
  # Version info needed on debug port
  gQcomTokenSpaceGuid.VersionDisplay|TRUE
!endif

  # offset to 4KB shared memory in System IMEM             : 0x8603F000
  # offset to 200 bytes HLOS region within this 4KB memory : 0x658
  # offset 0  : 4 byte physical address for DebugLog Override
  gQcomTokenSpaceGuid.PcdDebugLogOverrideAddress|0x08642658

  # Magic signature value that is used to indicate DebugLog Override
  # capability in DebugLib of DXE
  gQcomTokenSpaceGuid.PcdDebugLogOverrideMagic|0x534B4950

  # Hard/Warm Reset
  gQcomTokenSpaceGuid.PcdUefiHardResetConfigure|TRUE
  gQcomTokenSpaceGuid.PcdUefiWarmResetConfigure|TRUE
  gQcomTokenSpaceGuid.PcdUefiHardResetEnabled|TRUE
  gQcomTokenSpaceGuid.PcdUefiWarmResetEnabled|TRUE

  # PON Debounce
  gQcomTokenSpaceGuid.PcdPONDebounce|0x00000001 # max value 0x07

################################################################################
#
# Components Section - list of all EDK II Modules needed by this Platform
#
################################################################################
[Components.common]

#
# SEC
#

# DeviceProgrammerLib to be stubbed out for XBL image. To be enabled only in actual
# device programmer image
  QcomPkg/XBLLoader/XBLLoader.inf {
    # BUILD.MERGE sbl1_no_preamble.mbn
    # BUILD.SIGN sbl1
    <LibraryClasses>
      DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
      DevPrgLLib|QcomPkg/Library/NullLibs/DevPrgLLibNull/DevPrgLLibNull.inf
      DevPrgDLib|QcomPkg/Library/NullLibs/DevPrgDLibNull/DevPrgDLibNull.inf
      DDITargetLib|QcomPkg/Library/NullLibs/DDILibNull/DDILibNull.inf
#     BootDdrTrainingLib|QcomPkg/XBLLoader/BootDdrTrainingLibNull.inf
      BootDebugLib|QcomPkg/XBLLoader/BootDebugLib.inf
      QusbLdrLib|QcomPkg/Mdm9x05Pkg/Library/QusbLdrLib/QusbLdrLib.inf
#     SPILib|QcomPkg/Library/SPILib/SPILibLoader.inf
      I2CLib|QcomPkg/Library/I2CLib/I2CLibLoader.inf
      HashLib|QcomPkg/Mdm9x05Pkg/Library/HashLibLoader/HashLibHw.inf
      VSenseTargetLib|QcomPkg/Library/NullLibs/VSenseTargetLibNull/VSenseTargetLibNull.inf
#      PmicLoaderLib|QcomPkg/Mdm9x05Pkg/Library/PmicLib/loader/PmicLoaderLib.inf
       LoaderAptLib|QcomPkg/Mdm9x05Pkg/Library/LoaderAptLib/LoaderAptLib.inf
#      LoaderAptLib|QcomPkg/Library/NullLibs/LoaderAptLibNull/LoaderAptLibNull.inf
      SdccCommonLib|QcomPkg/Library/NullLibs/SdccLibNull/SdccCommonLibNull.inf

      # Only required for XBL loader
      CPRTargetLib|QcomPkg/Mdm9x05Pkg/Library/CPRTargetLib/CPRTargetLib.inf

      #This PCD values should match with the ones defined in boot_sbl_shared.h for sbl_build_type.
      <PcdsFixedAtBuild>
        gQcomTokenSpaceGuid.PcdBuildType|0x0
        gQcomTokenSpaceGuid.SDCCDmaEnabled|TRUE
        gQcomTokenSpaceGuid.SDCCEmmcSpeedMode|2
  }
  
  QcomPkg/Mdm9x05Pkg/Library/XBLRamDumpLib/XBLRamDumpLib.inf {
    # BUILD.MERGE sbl1_no_preamble.mbn
    # BUILD.SIGN sbl1
    # BUILD.XBL_SEC QcomPkg/Mdm9x05Pkg/Library/XBL_SEC/xbl_sec.mbn
    <LibraryClasses>
      DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
      QusbTargetLib|QcomPkg/Mdm9x05Pkg/Library/QusbTargetLib/QusbTargetLib.inf
      PmicLoaderLib|QcomPkg/Mdm9x05Pkg/Library/PmicLib/loader/PmicLoaderLib.inf
      PmicLib|QcomPkg/Mdm9x05Pkg/Library/PmicLib/PmicLib.inf
      ClockRamDumpLib|QcomPkg/Mdm9x05Pkg/Library/ClockTargetLib/ClockRamDumpLib.inf
      #ClockRamDumpLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
      SdccCommonLib|QcomPkg/Library/NullLibs/SdccLibNull/SdccCommonLibNull.inf
      <PcdsFixedAtBuild>
        gQcomTokenSpaceGuid.SDCCDmaEnabled|TRUE
        gQcomTokenSpaceGuid.SDCCEmmcSpeedMode|2
  }


  QcomPkg/XBLLoader/DevPrgN.inf {
    # BUILD.MERGE prog_firehose_nand.elf
    # BUILD.SIGN ENPRG
    # BUILD.XBL_SEC QcomPkg/Mdm9x05Pkg/Library/XBL_SEC/xbl_sec.mbn
     <LibraryClasses>
      NandHalDevPrgLib|QcomPkg/Library/FlashCommonLib/NandHalLib/NandHalDevPrgLib.inf
      FlashCommonLoaderLib|QcomPkg/Library/FlashCommonLib/FlashCommonDevPrgLib.inf
      XBLLoaderLib|QcomPkg/Mdm9x05Pkg/Library/XBLLoaderLib/XBLLoaderDevProgLib.inf
      DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
      SmemLib|QcomPkg/Library/NullLibs/SmemLibNull/SmemLibNull.inf
      DevPrgNLib|QcomPkg/Mdm9x05Pkg/Library/DevPrgNLib/DevPrgNLib.inf
      DevPrgLLib|QcomPkg/Library/NullLibs/DevPrgLLibNull/DevPrgLLibNull.inf
      DevPrgDLib|QcomPkg/Library/NullLibs/DevPrgDLibNull/DevPrgDLibNull.inf
      DDITargetLib|QcomPkg/Library/NullLibs/DDILibNull/DDILibNull.inf
      BootDdrTrainingLib|QcomPkg/XBLLoader/BootDdrTrainingLibNull.inf
      BootDebugLib|QcomPkg/XBLLoader/BootDebugLibNull.inf
      HashLib|QcomPkg/Mdm9x05Pkg/Library/HashLibLoader/HashLibHw.inf
      VSenseTargetLib|QcomPkg/Library/NullLibs/VSenseTargetLibNull/VSenseTargetLibNull.inf
      HotPlugLib|QcomPkg/Library/NullLibs/HotPlugLibNull/HotPlugLibNull.inf
      LoaderAptLib|QcomPkg/Library/NullLibs/LoaderAptLibNull/LoaderAptLibNull.inf
      QusbTargetLib|QcomPkg/Mdm9x05Pkg/Library/QusbTargetLib/QusbTargetLib.inf
      QusbLdrLib|QcomPkg/Library/NullLibs/QusbLdrLibNull/QusbLdrLibNull.inf
      CPRTargetLib|QcomPkg/Mdm9x05Pkg/Library/CPRTargetLib/CPRTargetLib.inf
      Celib|QcomPkg/Mdm9x05Pkg/Library/HashLibLoader/HashLibHw.inf

      <PcdsFixedAtBuild>
        gQcomTokenSpaceGuid.PcdBuildType|0x1
   }

 QcomPkg/XBLLoader/DevPrgSblN.inf {
    # BUILD.MERGE prog_firehose_SBL_nand.elf
    # BUILD.SIGN ENPRG
     <LibraryClasses>
      NandHalDevPrgLib|QcomPkg/Library/FlashCommonLib/NandHalLib/NandHalSblDevPrgLib.inf
      FlashCommonLoaderLib|QcomPkg/Library/FlashCommonLib/FlashCommonSblDevPrgLib.inf
      XBLLoaderLib|QcomPkg/Mdm9x05Pkg/Library/XBLLoaderLib/XBLLoaderDevProgSblLib.inf
      DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
      SmemLib|QcomPkg/Library/NullLibs/SmemLibNull/SmemLibNull.inf
      DevPrgSblNLib|QcomPkg/Mdm9x05Pkg/Library/DevPrgSblNLib/DevPrgSblNLib.inf
      DevPrgLLib|QcomPkg/Library/NullLibs/DevPrgLLibNull/DevPrgLLibNull.inf
      DevPrgDLib|QcomPkg/Library/NullLibs/DevPrgDLibNull/DevPrgDLibNull.inf
      DDITargetLib|QcomPkg/Library/NullLibs/DDILibNull/DDILibNull.inf
      BootDdrTrainingLib|QcomPkg/XBLLoader/BootDdrTrainingLibNull.inf
      BootDebugLib|QcomPkg/XBLLoader/BootDebugLibNull.inf
      HashLib|QcomPkg/Mdm9x05Pkg/Library/HashLibLoader/HashLibHw.inf
      VSenseTargetLib|QcomPkg/Library/NullLibs/VSenseTargetLibNull/VSenseTargetLibNull.inf
      HotPlugLib|QcomPkg/Library/NullLibs/HotPlugLibNull/HotPlugLibNull.inf
      LoaderAptLib|QcomPkg/Library/NullLibs/LoaderAptLibNull/LoaderAptLibNull.inf
      QusbTargetLib|QcomPkg/Mdm9x05Pkg/Library/QusbTargetLib/QusbTargetLib.inf
      QusbLdrLib|QcomPkg/Library/NullLibs/QusbLdrLibNull/QusbLdrLibNull.inf
      CPRTargetLib|QcomPkg/Library/NullLibs/PowerLibNull/CprLibNull.inf
      Celib|QcomPkg/Mdm9x05Pkg/Library/HashLibLoader/HashLibHw.inf
      #Below value is 1 for Lite memory and 2 for DDR memory usage in device programmer
      <PcdsFixedAtBuild>
        gQcomTokenSpaceGuid.PcdBuildType|0x2
   }

QcomPkg/XBLLoader/DDITarget.inf {
    # BUILD.MERGE ddi.elf
    # BUILD.SIGN ENPRG
    # BUILD.XBL_SEC QcomPkg/Mdm9x05Pkg/Library/XBL_SEC/xbl_sec.mbn
    <LibraryClasses>
      XBLLoaderLib|QcomPkg/Mdm9x05Pkg/Library/XBLLoaderLib/XBLLoaderDDILib.inf
      DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
      SmemLib|QcomPkg/Library/NullLibs/SmemLibNull/SmemLibNull.inf
      DevPrgLLib|QcomPkg/Library/NullLibs/DevPrgLLibNull/DevPrgLLibNull.inf
      DevPrgDLib|QcomPkg/Library/NullLibs/DevPrgDLibNull/DevPrgDLibNull.inf
      DDITargetLib|QcomPkg/Mdm9x05Pkg/Library/DDITargetLib/DDITargetLib.inf
      BootDdrTrainingLib|QcomPkg/XBLLoader/BootDdrTrainingLib.inf
      BootDebugLib|QcomPkg/XBLLoader/BootDebugLib.inf
      #BootDebugLib|QcomPkg/XBLLoader/BootDebugLibNull.inf
      HashLib|QcomPkg/Mdm9x05Pkg/Library/HashLibLoader/HashLibHw.inf
      LoaderAptLib|QcomPkg/Library/NullLibs/LoaderAptLibNull/LoaderAptLibNull.inf
      QusbTargetLib|QcomPkg/Mdm9x05Pkg/Library/QusbTargetLib/QusbTargetLib.inf
      QusbLdrLib|QcomPkg/Library/NullLibs/QusbLdrLibNull/QusbLdrLibNull.inf
      CPRTargetLib|QcomPkg/Library/NullLibs/PowerLibNull/CprLibNull.inf
      VSenseTargetLib|QcomPkg/Library/NullLibs/VSenseTargetLibNull/VSenseTargetLibNull.inf

      
      
    #  #This PCD values should match with the ones defined in boot_sbl_shared.h for sbl_build_type.
      <PcdsFixedAtBuild>
        gQcomTokenSpaceGuid.PcdBuildType|0x5
        
  }
  QcomPkg/Mdm9x05Pkg/Library/PmicLib/loader/Pmic.inf {
    # BUILD.MERGE sbl1_no_preamble.mbn
    # BUILD.SIGN sbl1
    # BUILD.XBL_SEC QcomPkg/Mdm9x05Pkg/Library/XBL_SEC/xbl_sec.mbn
      <LibraryClasses>
        DDITargetLib|QcomPkg/Library/NullLibs/DDILibNull/DDILibNull.inf
      <PcdsFixedAtBuild>
        gQcomTokenSpaceGuid.PcdBuildType|0x3
   }

   QcomPkg/Library/NandJtagTools/NandJtagTools.inf {
      # BUILD.MERGE NandJtagTools.elf
#enable below for DCPU mode
#      <LibraryClasses>
#        NandToolsLib|QcomPkg/Library/FlashCommonLib/FlashCommonNandToolsLib_dcpu.inf
#        NandHalToolsLib|QcomPkg/Library/FlashCommonLib/NandHalLib/NandHalToolsLib_dcpu.inf
      <PcdsFixedAtBuild>
        gQcomTokenSpaceGuid.PcdBuildType|0x6
   }

   QcomPkg/Library/NorJtagTools/NorJtagTools.inf {
      # BUILD.MERGE NorJtagTools.elf
      <PcdsFixedAtBuild>
        gQcomTokenSpaceGuid.PcdBuildType|0x7
   }

################################################################################
#
# UserExtensions.Buildit.BuildInfo - Extra info for buildit.py
#
################################################################################
[UserExtensions.Buildit.BuildInfo]
  COMPILER_ARCH = ARM  
  COMPILER_WIN  = CLANG40WIN
  COMPILER_LINUX = CLANG40LINUX

