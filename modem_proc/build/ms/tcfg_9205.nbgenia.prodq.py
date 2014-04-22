#==============================================================================
# Target Build ID Config Script.
#
# Copyright (c) 2010 Qualcomm Technologies Inc.  All Rights Reserved
#==============================================================================

def exists(env):
   return env.Detect('tcfg_9205.nbgenia.prodQ_data')

def generate(env):
   # Save the tcfg_bid_data in the env
   env['TCFG_BID_IMAGE'] = 'MODEM_PROC'

   env.AddUsesFlags('USES_AKA', from_builds_file = True)
   env.AddUsesFlags('USES_AUDIO_AVS_SOURCES', from_builds_file = True)
   env.AddUsesFlags('USES_AUTOSTKV2', from_builds_file = True)
   env.AddUsesFlags('USES_BLAST', from_builds_file = True)
   env.AddUsesFlags('USES_CGPS', from_builds_file = True)
   env.AddUsesFlags('USES_CORE_DSM', from_builds_file = True)
   env.AddUsesFlags('USES_CTA_STRIP', from_builds_file = True)
   env.AddUsesFlags('USES_DAL', from_builds_file = True)
   env.AddUsesFlags('USES_DATAMODEM_DSS_PING', from_builds_file = True)
   env.AddUsesFlags('USES_DATAMODEM_SEC_SSL', from_builds_file = True)
   env.AddUsesFlags('USES_DIAG_NV_SUPPORT', from_builds_file = True)
   env.AddUsesFlags('USES_DIAG_SMD_SUPPORT', from_builds_file = True)
   env.AddUsesFlags('USES_DUMMY_SEG', from_builds_file = True)
   env.AddUsesFlags('USES_EFS_SETUP_DEVICE', from_builds_file = True)
   env.AddUsesFlags('USES_ERR_INJECT_CRASH', from_builds_file = True)
   env.AddUsesFlags('USES_EXCLUDE_C2K_FW', from_builds_file = True)
   env.AddUsesFlags('USES_EXCLUDE_LTE_FW', from_builds_file = True)
   env.AddUsesFlags('USES_EXCLUDE_TDSCDMA_FW', from_builds_file = True)
   env.AddUsesFlags('USES_EXCLUDE_WCDMA_FW', from_builds_file = True)
   env.AddUsesFlags('USES_EXPORT_LOC_API', from_builds_file = True)
   env.AddUsesFlags('USES_EXPORT_PBMLIB', from_builds_file = True)
   env.AddUsesFlags('USES_FEATURE_CATM1_DISABLE', from_builds_file = True)
   env.AddUsesFlags('USES_FEATURE_CGPS_XTRA', from_builds_file = True)
   env.AddUsesFlags('USES_FEATURE_CGPS_XTRA_T', from_builds_file = True)
   env.AddUsesFlags('USES_FEATURE_CLADE_HEAP', from_builds_file = True)
   env.AddUsesFlags('USES_FEATURE_CMI_PMB', from_builds_file = True)
   env.AddUsesFlags('USES_FEATURE_DYNAMIC_LOADING', from_builds_file = True)
   env.AddUsesFlags('USES_FEATURE_DYNAMIC_LOADING_GERAN', from_builds_file = True)
   env.AddUsesFlags('USES_FEATURE_DYNAMIC_LOADING_GLOBAL', from_builds_file = True)
   env.AddUsesFlags('USES_FEATURE_DYNAMIC_LOADING_GNSS', from_builds_file = True)
   env.AddUsesFlags('USES_FEATURE_DYNAMIC_LOADING_LTE', from_builds_file = True)
   env.AddUsesFlags('USES_FEATURE_DYNAMIC_LOADING_NB1', from_builds_file = True)
   env.AddUsesFlags('USES_FEATURE_ENABLE_GSM_VOCODER', from_builds_file = True)
   env.AddUsesFlags('USES_FEATURE_EXTERNAL_PAGING', from_builds_file = True)
   env.AddUsesFlags('USES_FEATURE_FSK_TX', from_builds_file = True)
   env.AddUsesFlags('USES_FEATURE_GNSS_DYNAMIC_HEAP_MEM_ENABLED', from_builds_file = True)
   env.AddUsesFlags('USES_FEATURE_GNSS_NAV_HW_VER5', from_builds_file = True)
   env.AddUsesFlags('USES_FEATURE_GNSS_OSYS_V668', from_builds_file = True)
   env.AddUsesFlags('USES_FEATURE_GNSS_OTDOA_50', from_builds_file = True)
   env.AddUsesFlags('USES_FEATURE_GNSS_SENSITIVITY_IMPROVMENT_FOR_9X05', from_builds_file = True)
   env.AddUsesFlags('USES_FEATURE_GNSS_TDP_20_ENABLED', from_builds_file = True)
   env.AddUsesFlags('USES_FEATURE_GPS_WWAN_PRIORITY_SEARCH', from_builds_file = True)
   env.AddUsesFlags('USES_FEATURE_JOLOKIA_MODEM', from_builds_file = True)
   env.AddUsesFlags('USES_FEATURE_LOCTECH_10HZ', from_builds_file = True)
   env.AddUsesFlags('USES_FEATURE_LOCTECH_NHZ', from_builds_file = True)
   env.AddUsesFlags('USES_FEATURE_LOCTECH_QMISLIM', from_builds_file = True)
   env.AddUsesFlags('USES_FEATURE_LOCTECH_QMISLIM', from_builds_file = True)
   env.AddUsesFlags('USES_FEATURE_LOC_GTS_SUPPORT', from_builds_file = True)
   env.AddUsesFlags('USES_FEATURE_LOC_GTS_TTR_SUPPORT', from_builds_file = True)
   env.AddUsesFlags('USES_FEATURE_LTE_IDLE_SVS_CLK', from_builds_file = True)
   env.AddUsesFlags('USES_FEATURE_MCS_TCXO_REFACTORED', from_builds_file = True)
   env.AddUsesFlags('USES_FEATURE_RF_SUPPORT_BAND_87_88', from_builds_file = True)
   env.AddUsesFlags('USES_FS_ASYNC_PUT', from_builds_file = True)
   env.AddUsesFlags('USES_FTM_HWTC', from_builds_file = True)
   env.AddUsesFlags('USES_GEOFENCE', from_builds_file = True)
   env.AddUsesFlags('USES_GNSS_BDS', from_builds_file = True)
   env.AddUsesFlags('USES_GNSS_GLO', from_builds_file = True)
   env.AddUsesFlags('USES_GPS_LM_STANDALONE', from_builds_file = True)
   env.AddUsesFlags('USES_GSM', from_builds_file = True)
   env.AddUsesFlags('USES_GSTK', from_builds_file = True)
   env.AddUsesFlags('USES_IMS', from_builds_file = True)
   env.AddUsesFlags('USES_IMS_SMS', from_builds_file = True)
   env.AddUsesFlags('USES_INTEGRITY_CHECK', from_builds_file = True)
   env.AddUsesFlags('USES_IPSEC', from_builds_file = True)
   env.AddUsesFlags('USES_JOLOKIA_MODEM', from_builds_file = True)
   env.AddUsesFlags('USES_LCS_FILE', from_builds_file = True)
   env.AddUsesFlags('USES_LTE', from_builds_file = True)
   env.AddUsesFlags('USES_LTE_CATM', from_builds_file = True)
   env.AddUsesFlags('USES_MBNTOOLS', from_builds_file = True)
   env.AddUsesFlags('USES_MCFG_MBN_BUILDER', from_builds_file = True)
   env.AddUsesFlags('USES_MCFG_MULTI_MBN', from_builds_file = True)
   env.AddUsesFlags('USES_MODEM_RCINIT', from_builds_file = True)
   env.AddUsesFlags('USES_MSGR', from_builds_file = True)
   env.AddUsesFlags('USES_MYPS_LINKER', from_builds_file = True)
   env.AddUsesFlags('USES_NB1', from_builds_file = True)
   env.AddUsesFlags('USES_NB1_PLT', from_builds_file = True)
   env.AddUsesFlags('USES_NO_AUDIO', from_builds_file = True)
   env.AddUsesFlags('USES_NO_PLATFORM', from_builds_file = True)
   env.AddUsesFlags('USES_NO_SENSORS', from_builds_file = True)
   env.AddUsesFlags('USES_PBMTASK', from_builds_file = True)
   env.AddUsesFlags('USES_PLATFORM_IMAGE_INTEGRITY_CHECK', from_builds_file = True)
   env.AddUsesFlags('USES_POSIX', from_builds_file = True)
   env.AddUsesFlags('USES_QDSP6', from_builds_file = True)
   env.AddUsesFlags('USES_QMI_SAP', from_builds_file = True)
   env.AddUsesFlags('USES_QMI_SI_LOG', from_builds_file = True)
   env.AddUsesFlags('USES_QPIC_NAND', from_builds_file = True)
   env.AddUsesFlags('USES_QURT', from_builds_file = True)
   env.AddUsesFlags('USES_RCINIT', from_builds_file = True)
   env.AddUsesFlags('USES_REMOTEFS_QMI', from_builds_file = True)
   env.AddUsesFlags('USES_REX', from_builds_file = True)
   env.AddUsesFlags('USES_RUIM', from_builds_file = True)
   env.AddUsesFlags('USES_SBI', from_builds_file = True)
   env.AddUsesFlags('USES_SEC', from_builds_file = True)
   env.AddUsesFlags('USES_SEC_CLNT', from_builds_file = True)
   env.AddUsesFlags('USES_SEC_POLICY_DEFAULT_SIGN', from_builds_file = True)
   env.AddUsesFlags('USES_SERVICES_ZLIB', from_builds_file = True)
   env.AddUsesFlags('USES_SFS', from_builds_file = True)
   env.AddUsesFlags('USES_STACK_PROTECTOR', from_builds_file = True)
   env.AddUsesFlags('USES_STORAGE_NAND', from_builds_file = True)
   env.AddUsesFlags('USES_STRIP_NO_ODM', from_builds_file = True)
   env.AddUsesFlags('USES_THERMAL', from_builds_file = True)
   env.AddUsesFlags('USES_UNIFIED_SP_BUILD', from_builds_file = True)
   env.AddUsesFlags('USES_VIRTIO', from_builds_file = True)
   env.AddUsesFlags('USES_WTR1625', from_builds_file = True)
