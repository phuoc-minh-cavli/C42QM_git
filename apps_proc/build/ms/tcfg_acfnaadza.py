#==============================================================================
# Target Build ID Config Script.
#
# Copyright (c) 2010 Qualcomm Technologies Inc.  All Rights Reserved
#==============================================================================

def exists(env):
   return env.Detect('tcfg_ACFNAADZA_data')

def generate(env):
   # Save the tcfg_bid_data in the env
   env['TCFG_BID_IMAGE'] = 'APPS_PROC'

   env.AddUsesFlags('USES_AKA', from_builds_file = True)
   env.AddUsesFlags('USES_ARM_ASM_SPINLOCK', from_builds_file = True)
   env.AddUsesFlags('USES_ATS_API_STUB', from_builds_file = True)
   env.AddUsesFlags('USES_AUDIO_BAREBONE_ACDB', from_builds_file = True)
   env.AddUsesFlags('USES_CORE_DSM', from_builds_file = True)
   env.AddUsesFlags('USES_CTA_STRIP', from_builds_file = True)
   env.AddUsesFlags('USES_DAL', from_builds_file = True)
   env.AddUsesFlags('USES_DDR_ONLY', from_builds_file = True)
   env.AddUsesFlags('USES_DEM', from_builds_file = True)
   env.AddUsesFlags('USES_DIAG_SMD_SUPPORT', from_builds_file = True)
   env.AddUsesFlags('USES_DMOV', from_builds_file = True)
   env.AddUsesFlags('USES_DOG_KEEPALIVE', from_builds_file = True)
   env.AddUsesFlags('USES_ERR_INJECT_CRASH', from_builds_file = True)
   env.AddUsesFlags('USES_EXT_BUILD_FLAVORS', from_builds_file = True)
   env.AddUsesFlags('USES_FEATURE_APSS_AUDIO_INIT', from_builds_file = True)
   env.AddUsesFlags('USES_FEATURE_AUDIO_CSD_QMI_SERVICE', from_builds_file = True)
   env.AddUsesFlags('USES_FEATURE_QAPI_ENABLED', from_builds_file = True)
   env.AddUsesFlags('USES_FLASH_DAL', from_builds_file = True)
   env.AddUsesFlags('USES_FTA', from_builds_file = True)
   env.AddUsesFlags('USES_FTM_HWTC', from_builds_file = True)
   env.AddUsesFlags('USES_HSU', from_builds_file = True)
   env.AddUsesFlags('USES_HSU_ECM', from_builds_file = True)
   env.AddUsesFlags('USES_HSU_FAST_CHARGE', from_builds_file = True)
   env.AddUsesFlags('USES_I2C_ON_APPS', from_builds_file = True)
   env.AddUsesFlags('USES_IDIAG', from_builds_file = True)
   env.AddUsesFlags('USES_IGUANA', from_builds_file = True)
   env.AddUsesFlags('USES_LLDB_ENABLE', from_builds_file = True)
   env.AddUsesFlags('USES_MBNTOOLS', from_builds_file = True)
   env.AddUsesFlags('USES_MEMHEAP2', from_builds_file = True)
   env.AddUsesFlags('USES_MEM_USAGE_HTML', from_builds_file = True)
   env.AddUsesFlags('USES_MODULE_MMU_CONTROL_ENABLE', from_builds_file = True)
   env.AddUsesFlags('USES_NO_BOOT_LOADER', from_builds_file = True)
   env.AddUsesFlags('USES_NO_EFS_SRC', from_builds_file = True)
   env.AddUsesFlags('USES_NO_RFS_SRC', from_builds_file = True)
   env.AddUsesFlags('USES_NO_TFTP_SRC', from_builds_file = True)
   env.AddUsesFlags('USES_PMEM_REMOTE', from_builds_file = True)
   env.AddUsesFlags('USES_PRODCUTS_DLOAD_ENTRY', from_builds_file = True)
   env.AddUsesFlags('USES_QSEECOM', from_builds_file = True)
   env.AddUsesFlags('USES_QSEE_LOG', from_builds_file = True)
   env.AddUsesFlags('USES_QURT', from_builds_file = True)
   env.AddUsesFlags('USES_RCINIT', from_builds_file = True)
   env.AddUsesFlags('USES_RCINIT', from_builds_file = True)
   env.AddUsesFlags('USES_SEC', from_builds_file = True)
   env.AddUsesFlags('USES_SEC_POLICY_MULTIPLE_DEFAULT_SIGN', from_builds_file = True)
   env.AddUsesFlags('USES_SFS', from_builds_file = True)
   env.AddUsesFlags('USES_SLEEPCTL_API_STUB', from_builds_file = True)
   env.AddUsesFlags('USES_STACK_PROTECTOR', from_builds_file = True)
   env.AddUsesFlags('USES_STORAGE_NAND', from_builds_file = True)
   env.AddUsesFlags('USES_THERMAL', from_builds_file = True)
   env.AddUsesFlags('USES_THREADX', from_builds_file = True)
