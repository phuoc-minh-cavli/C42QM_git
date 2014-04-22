# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

"""Contains constants related to the cfgparser package.

Constants relate to:

#. Common constants
#. Auto-generated FB XML Parser - auto_gen_xml_config
"""

import os

from sectools.common.utils import c_path

DIR_PATH = os.path.dirname(os.path.abspath(__file__))

#------------------------------------------------------------------------------
# Common constants
#------------------------------------------------------------------------------
# Absolute path of the package
PACKAGE_PATH = DIR_PATH

# Name of the root node of any config related objects
ROOTNODE_NAME = 'fuseblower'

# Name of the directory containing chip specific folders
CONFIG_DIR_BASENAME = 'config'

#------------------------------------------------------------------------------
# Auto-generated XML Parser related information
#------------------------------------------------------------------------------
# XML - Basic information
XML_PY_FILE_NAME = 'auto_gen_fb_config.py'
XML_PY_FILE_NAME_USER = 'auto_gen_fb_user_config.py'
XML_PY_PATH = c_path.join(PACKAGE_PATH, XML_PY_FILE_NAME)
XML_PY_PATH_USER = c_path.join(PACKAGE_PATH, XML_PY_FILE_NAME_USER)

XML_NAME_ENDING_OEM = '_' + ROOTNODE_NAME + '_OEM' + '.xml'
XML_NAME_ENDING_QTI = '_' + ROOTNODE_NAME + '_QTI' + '.xml'
XML_NAME_ENDING_UI = '_' + ROOTNODE_NAME + '_UI' + '.xml'
XML_NAME_ENDING_USER = '_' + ROOTNODE_NAME + '_USER' + '.xml'

# Modes of tool operation
FB_MODE_SECBOOT = 'secboot'
FB_MODE_ROT     = 'rot'

XML_NAME_END_OEM = \
{
    FB_MODE_SECBOOT : '_' + ROOTNODE_NAME + '_OEM' + '.xml',
    FB_MODE_ROT : '_' + ROOTNODE_NAME + '_ROT' + '_OEM' + '.xml',
}

XML_NAME_END_QTI = \
{
    FB_MODE_SECBOOT : '_' + ROOTNODE_NAME + '_QTI' + '.xml',
    FB_MODE_ROT : '_' + ROOTNODE_NAME + '_ROT' + '_QTI' + '.xml',
}

XML_NAME_END_UI = \
{
    FB_MODE_SECBOOT : '_' + ROOTNODE_NAME + '_UI' + '.xml',
    FB_MODE_ROT : '_' + ROOTNODE_NAME + '_ROT' + '_UI' + '.xml',
}

XML_NAME_END_USER = \
{
    FB_MODE_SECBOOT : '_' + ROOTNODE_NAME + '_USER' + '.xml',
    FB_MODE_ROT : '_' + ROOTNODE_NAME + '_ROT' + '_USER' + '.xml',
}

# XML - lines at the beginning of the file
XML_PREPEND_LINES = '<?xml version="1.0" encoding="UTF-8"?>\n'

# XML - rootnode related constants
XML_ROOTNODE_NAMESPACE = 'tns:'
XML_ROOTNODE_NAME = ROOTNODE_NAME
XML_ROOTNODE = XML_ROOTNODE_NAMESPACE + XML_ROOTNODE_NAME

# XML - namespace related constants
XML_NAMESPACE_FUSEBLOWER = 'xmlns:tns="http://www.qualcomm.com/fuseblower"'
XML_NAMESPACE_W3 = 'xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"'

XML_NAMESPACE_SCHEMA = 'xsi:schemaLocation="http://www.qualcomm.com/fuseblower ../xsd/fuseblower.xsd"'
XML_NAMESPACE_SCHEMA_USER = 'xsi:schemaLocation="http://www.qualcomm.com/fuseblower ../xsd/fuseblower_user.xsd"'

XML_NAMESPACE = XML_NAMESPACE_FUSEBLOWER + '\n\t' + XML_NAMESPACE_W3 + '\n\t' + XML_NAMESPACE_SCHEMA
XML_NAMESPACE_USER = XML_NAMESPACE_FUSEBLOWER + '\n\t' + XML_NAMESPACE_W3 + '\n\t' + XML_NAMESPACE_SCHEMA_USER

# XML - classname generator
XML_CLASSNAME_GEN = lambda x: 'complex_' + x

# Fuseblower - Secboot Mode & RoT Mode
# mapping of user configurable entries and what Fuse Field they modify
FIELD_ID_MAP = {'oem_hw_id':'OEM_HW_ID',
                'mrc_cert_count':'ROOT_CERT_TOTAL_NUM',
                'oem_product_id':'OEM_PRODUCT_ID',
                'SEC_BOOT1_PK_Hash_in_Fuse' : 'SEC_BOOT1_PK_Hash_in_Fuse',
                'SEC_BOOT1_rom_pk_hash_index':'SEC_BOOT1_ROM_PK_Hash_Index',
                'SEC_BOOT1_use_serial_num':'SEC_BOOT1_Use_Serial_Num',
                'SEC_BOOT2_PK_Hash_in_Fuse' : 'SEC_BOOT2_PK_Hash_in_Fuse',
                'SEC_BOOT3_rom_pk_hash_index':'SEC_BOOT3_ROM_PK_Hash_Index',
                'SEC_BOOT2_use_serial_num':'SEC_BOOT1_Use_Serial_Num',
                'SEC_BOOT3_PK_Hash_in_Fuse' : 'SEC_BOOT3_PK_Hash_in_Fuse',
                'SEC_BOOT2_rom_pk_hash_index':'SEC_BOOT2_ROM_PK_Hash_Index',
                'SEC_BOOT3_use_serial_num':'SEC_BOOT3_Use_Serial_Num',
                'mrc_pk_hash_index':'ROOT_CERT_PK_HASH_INDEX',
                'modem_mrc_pk_hash_index':'MODEM_ROOT_CERT_PK_HASH_INDEX',
                'root_cert_hash' : 'HASH_DATA_ROW0',
                'root_cert_file' : 'HASH_DATA_ROW0',
                'root_cert_sha384_hash_file' : 'HASH_DATA_ROW0',
                'apps_debug_disabled' : 'APPS_DBGEN_DISABLE',
                'allow_bus_dcvs' : 'APPS_NIDEN_DISABLE',
                'watchdog_enable' : 'WDOG_EN',
                'root_cert_hash0' : 'HASH0_DATA_ROW0',
                'root_cert_hash0_file' : 'HASH0_DATA_ROW0',
                'root_cert_sha384_hash0_file' : 'HASH0_DATA_ROW0',
                'pk_hash1' : 'HASH1_DATA_ROW0',
                'pk_hash1_file':'HASH1_DATA_ROW0',
                'image_encr_key1' : 'KEY_DATA_ROW0',
                'image_encr_key1_file' : 'KEY_DATA_ROW0',
                'disable_rot_transfer': 'DISABLE_ROT_TRANSFER',
                'current_sw_rot_apps': 'CURRENT_SW_ROT_APPS',
                'current_sw_rot_modem': 'CURRENT_SW_ROT_MODEM',
                'root_certhash' : 'HASH_DATA0_ROW0',
                'root_certfile' : 'HASH_DATA0_ROW0',
                'oem_hwid' : 'OEM_HW_ID_9_0',
                'oem_productid' :'OEM_PRODUCT_ID_3_0',
                'SECURE_BOOT0_use_serial_num': 'SECURE_BOOT0_USE_SERIAL_NUM',
                'SECURE_BOOT1_use_serial_num': 'SECURE_BOOT1_USE_SERIAL_NUM',
                'SECURE_BOOT2_use_serial_num': 'SECURE_BOOT2_USE_SERIAL_NUM',
                'SECURE_BOOT3_use_serial_num': 'SECURE_BOOT3_USE_SERIAL_NUM',
                'SECURE_BOOT4_use_serial_num': 'SECURE_BOOT4_USE_SERIAL_NUM',
                'SECURE_BOOT5_use_serial_num': 'SECURE_BOOT5_USE_SERIAL_NUM',
                'SECURE_BOOT6_use_serial_num': 'SECURE_BOOT6_USE_SERIAL_NUM',
                'SECURE_BOOT7_use_serial_num': 'SECURE_BOOT7_USE_SERIAL_NUM',
                'total_rot_num': 'TOTAL_ROT_NUM',
                'rot_activation': 'ROT_ACTIVATION',
                'rot_revocation': 'ROT_REVOCATION',
                'root_cert_activate_index': 'ROOT_CERT_ACTIVATION_LIST',
                'root_cert_revoke_index': 'ROOT_CERT_REVOCATION_LIST',
                'sp_nvm_ar_config': 'SP_NVM_AR_CONFIG',
                'wifi_board_id': 'WIFI_BOARD_ID',
                'eku_enforcement_en': 'EKU_ENFORCEMENT_EN',
                'sp_disable': 'SP_DISABLE',
                'sp_fips_enable': 'SP_FIPS_ENABLE',
                'tz_sw_crypto_fips_enable': 'TZ_SW_CRYPTO_FIPS_ENABLE',
                'tz_sw_crypto_fips_override': 'TZ_SW_CRYPTO_FIPS_OVERRIDE',
                'sp_fips_override': 'SP_FIPS_OVERRIDE',
                'spu_iar_feature_enable': 'SPU_IAR_FEATURE_ENABLE',
                'anti_rollback_feature_enable': 'ANTI_ROLLBACK_FEATURE_ENABLE',
                'oem_periph_pid':'PERIPH_PID',
                'oem_periph_vid': 'PERIPH_VID',
                'wcss_dbg_disable': 'WCSS_DBG_DISABLE',
                'wcss_spniden_disable': 'WCSS_SPNID_DISABLE',
                'rpm_dbgen_disable': 'RPM_DBGEN_DISABLE',
                'rpm_wcss_nid_disable': 'RPM_WCSS_NID_DISABLE',
                'rpm_dapen_disable': 'RPM_DAPEN_DISABLE',
                'wcss_spiden_disable': 'WCSS_SPID_DISABLE',
                'wcss_nid_disable': 'WCSS_NID_DISABLE',
                'q6ss_dbg_disable': 'Q6SS_DBG_DISABLE',
                'apps_spiden_disable': 'APPS_SPIDEN_DISABLE',
                'apps_spniden_disable': 'APPS_SPNIDEN_DISABLE',
                'dap_dbgen_disable': 'DAP_DBGEN_DISABLE',
                'dap_niden_disable': 'DAP_NIDEN_DISABLE',
                'dap_spiden_disable': 'DAP_SPIDEN_DISABLE',
                'dap_spniden_disable': 'DAP_SPNIDEN_DISABLE',
                'dap_deviceen_disable': 'DAP_DEVICEEN_DISABLE',
                'fast_boot_feature_enable': 'FAST_BOOT',
                'efuse_ubi32_dbg_disable': 'EFUSE_UBI32_DBG_DISABLE',
                'q6ss_etm_disable': 'Q6SS_ETM_DISABLE',
                'q6ss_dfd_disable': 'Q6SS_DFD_DISABLE',
                'lpass_q6_etm_disable': 'LPASS_Q6_ETM_DISABLE',
                'lpass_q6_dfd_disable': 'LPASS_Q6_DFD_DISABLE',
                'fuse_debug_bus_disable': 'FUSE_DEBUG_BUS_DISABLE',
                'lpass_q6_dbgen_disable': 'LPASS_Q6_DBGEN_DISABLE',
                'qwes_gtp_wwan_enable': 'QWES_GTP_WWAN',
                'qwes_gnss_enable': 'QWES_GNSS',
                'qwes_volte_enable': 'QWES_VoLTE',
                'shared_qsee_spiden_disable': 'SHARED_QSEE_SPIDEN_DISABLE',
                'shared_qsee_spniden_disable': 'SHARED_QSEE_SPNIDEN_DISABLE',
                'shared_mss_dbgen_disable': 'SHARED_MSS_DBGEN_DISABLE',
                'shared_mss_niden_disable': 'SHARED_MSS_NIDEN_DISABLE',
                'shared_cp_dbgen_disable': 'SHARED_CP_DBGEN_DISABLE',
                'shared_cp_niden_disable': 'SHARED_CP_NIDEN_DISABLE',
                'shared_ns_dbgen_disable': 'SHARED_NS_DBGEN_DISABLE',
                'shared_ns_niden_disable': 'SHARED_NS_NIDEN_DISABLE',
                'apps_niden_disable': 'APPS_NIDEN_DISABLE',
                'shared_misc_debug_disable': 'SHARED_MISC_DEBUG_DISABLE',
                'acmt_bypass_disable': 'ACMT_BYPASS_DISABLE',
                'ram_dump_use_sn': 'RAM_DUMP_USE_SN',
                'oem_spare_random1': 'OEM_SPARE_RANDOM1',
                'oem_spare_random2': 'OEM_SPARE_RANDOM2',
                'oem_spare_random3': 'OEM_SPARE_RANDOM3',
                'anti_rollback_write_disable': 'ANTI_ROLLBACK_WRITE_DISABLE',
                'qwes_qdr2': 'QWES_QDR2',
                'qwes_qdr3': 'QWES_QDR3',
                'qwes_gnss_single_frequency': 'QWES_GNSS_SINGLE_FREQUENCY',
                'qwes_gnss_multi_frequency': 'QWES_GNSS_MULTI_FREQUENCY',
                'qwes_gnss_over_ethernet_ptp': 'QWES_GNSS_OVER_ETHERNET_PTP',
                'qwes_cv2x_location_basic': 'QWES_CV2X_LOCATION_BASIC',
                'qwes_cv2x_location_premium': 'QWES_CV2X_LOCATION_PREMIUM',
                'qwes_tcedr': 'QWES_TCEDR',
                'qwes_ppme': 'QWES_PPME',
                'disable_qpic_quadio_xfer': 'DISABLE_QPIC_QUADIO_XFER',
                'msmc_mss_dbgen': 'MSMC_MSS_DBGEN',
                'all_debug_disable': 'ALL_DEBUG_DISABLE',
                'e_dload_disable': 'E_DLOAD_DISABLE',
                'enum_timeout': 'ENUM_TIMEOUT',
                'force_dload_disable': 'FORCE_DLOAD_DISABLE',
                'sdcc_adma_disable': 'SDCC_ADMA_DISABLE',
                'fast_boot': 'FAST_BOOT',
                'pbl_qspi_boot_edl_enabled': 'PBL_QSPI_BOOT_EDL_ENABLED',
                'spi_clk_boot_freq': 'SPI_CLK_BOOT_FREQ',
                'sw_fuse_prog_disable': 'SW_FUSE_PROG_DISABLE',
                'pbl_fdl_timeout_reset_feature_enable': 'PBL_FDL_TIMEOUT_RESET_FEATURE_ENABLE',
                'pbl_log_disable': 'PBL_LOG_DISABLE',
                'pbl_usb_type_c_disable': 'PBL_USB_TYPE_C_DISABLE',
                'usb_ss_disable': 'USB_SS_DISABLE',
                'debug_disable_in_rom': 'DEBUG_DISABLE_IN_ROM',
                'debug_policy_disable': 'DEBUG_POLICY_DISABLE',
                'shared_misc1_debug_disable': 'SHARED_MISC1_DEBUG_DISABLE',
                'shared_misc2_debug_disable': 'SHARED_MISC2_DEBUG_DISABLE',
                'shared_misc3_debug_disable': 'SHARED_MISC3_DEBUG_DISABLE',
                'shared_misc4_debug_disable': 'SHARED_MISC4_DEBUG_DISABLE',
                'shared_misc5_debug_disable': 'SHARED_MISC5_DEBUG_DISABLE',
                'sp_ari_test_mode_feature_enable': 'SP_ARI_TEST_MODE_FEATURE_ENABLE',
                'sp_ari_suspension_feature_enable':'SP_ARI_SUSPENSION_FEATURE_ENABLE',
                'sp_app_rot_transfer_feature_enabled': 'SP_APP_ROT_TRANSFER_FEATURE_ENABLED',
                'sp_nvm_ar_split_config': 'SP_NVM_AR_SPLIT_CONFIG',
                'sp_ari_suspension_mode_feature_enable': 'SP_ARI_SUSPENSION_MODE_FEATURE_ENABLE',
                'spu_iar_enabled': 'SPU_IAR_ENABLED',
                'disable_rsa': 'DISABLE_RSA',
                'OEM_SECURE_BOOT1_auth_en': 'OEM_SECURE_BOOT1_AUTH_EN',
                'OEM_SECURE_BOOT2_auth_en': 'OEM_SECURE_BOOT2_AUTH_EN',
                'OEM_SECURE_BOOT3_auth_en': 'OEM_SECURE_BOOT3_AUTH_EN',
                'force_usb_boot_disable': 'FORCE_USB_BOOT_DISABLE',
                'pcie_early_init_en': 'PCIE_EARLY_INIT_EN',
                'sw_rot_use_serial_num': 'SW_ROT_USE_SERIAL_NUM',
                'apps_hash_integrity_check_disable': 'APPS_HASH_INTEGRITY_CHECK_DISABLE',
                'nand_xfer_param': 'NAND_XFER_PARAM',
                'usb_pipo_disable': 'USB_PIPO_DISABLE',
                'spare_reg27_secure': 'SPARE_REG27_SECURE',
                'spare_reg28_secure': 'SPARE_REG28_SECURE',
                'spare_reg29_secure': 'SPARE_REG29_SECURE',
                'spare_reg30_secure': 'SPARE_REG30_SECURE',
                'segment_hash_function_select_0': 'SEGMENT_HASH_FUNCTION_SELECT_0',
                'segment_hash_function_select_1': 'SEGMENT_HASH_FUNCTION_SELECT_1',
                'segment_hash_function_select_2': 'SEGMENT_HASH_FUNCTION_SELECT_2',
                'segment_hash_function_select_3': 'SEGMENT_HASH_FUNCTION_SELECT_3',
                'qspi_dma_disable': 'QSPI_DMA_DISABLE',
                'ddr_bootstrap_gpio': 'DDR_BOOTSTRAP_GPIO',
                'edl_indicator': 'EDL_INDICATOR',
                'rffe_mod1p2_fuse_sel': 'RFFE_MOD1P2_FUSE_SEL',
                'llcc_dsrw_disable': 'LLCC_DSRW_DISABLE',
                'pcie_subsys_dev_id': 'PCIE_SUBSYS_DEV_ID',
                'subsys_vendor_id': 'SUBSYS_VENDOR_ID'
                }

ROOT_CERT_PK_HASH_INDEX={'PRODUCTION_DEVICE_NO_CERTIFICATE_SELECTED':0x00,
                        #Production device, no certificate selected (Default to certificate 0)
                        'PRODUCTION_DEVICE_FIXED_TO_CERTIFICATE_0' :0xF0,
                        #Production device fixed to certificate 0
                        'DEVICE_FIXED_TO_CERTIFICATE_1'            :0xE1,
                        #Device fixed to certificate 1
                        'DEVICE_FIXED_TO_CERTIFICATE_2'            :0xD2,
                        #Device fixed to certificate 2
                        'DEVICE_FIXED_TO_CERTIFICATE_3'            :0xC3,
                        #Device fixed to certificate 3
                        'DEVICE_FIXED_TO_CERTIFICATE_4'            :0xB4,
                        #Device fixed to certificate 4
                        'DEVICE_FIXED_TO_CERTIFICATE_5'            :0xA5,
                        #Device fixed to certificate 5
                        'DEVICE_FIXED_TO_CERTIFICATE_6'            :0x96,
                        #Device fixed to certificate 6
                        'DEVICE_FIXED_TO_CERTIFICATE_7'            :0x87,
                        #Device fixed to certificate 7
                        'DEVICE_FIXED_TO_CERTIFICATE_8'            :0x78,
                        #Device fixed to certificate 8
                        'DEVICE_FIXED_TO_CERTIFICATE_9'            :0x69,
                        #Device fixed to certificate 9
                        'DEVICE_FIXED_TO_CERTIFICATE_10'           :0x5A,
                        #Device fixed to certificate 10
                        'DEVICE_FIXED_TO_CERTIFICATE_11'           :0x4B,
                        #Device fixed to certificate 11
                        'DEVICE_FIXED_TO_CERTIFICATE_12'           :0x3C,
                        #Device fixed to certificate 12
                        'DEVICE_FIXED_TO_CERTIFICATE_13'           :0x2D,
                        #Device fixed to certificate 13
                        'DEVICE_FIXED_TO_CERTIFICATE_14'           :0x1E,
                        #Device fixed to certificate 14
                        'DEVICE_FIXED_TO_CERTIFICATE_15'           :0x0F,
                        #Device fixed to certificate 15
                        'DEVICE_FIXED_TO_CERTIFICATE_INVALID'      :0xFF,
                        #Invalid certificate index
                        }
