# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from . import defines
from sectools.common.utils.c_logging import logger
from sectools.features.fuse_common.parsegen.fb_config.defines import FEC_DATA_56BIT_MASK, \
    FEC_DATA_64BIT_MASK, FEC_IN_ROW, DIFF_FEC_REG, DIFF_FEC_REG_NAZGUL, DIFF_FEC_REG_NIC, \
    DIFF_FEC_REG_TW, DIFF_FEC_REG_VT, DIFF_FEC_REG_SAI, DIFF_FEC_REG_CYP, DIFF_FEC_KONA_HURACAN_FUSION, DIFF_FEC_HASTINGS, \
    DIFF_FEC_MAKENA, FEC_DATA_32BIT_MASK, ROT_SUPPORTED, FEC_SHADOW, MRC_2_0_CHIPSETS, DIFF_FEC_OLYMPIC
from sectools.features.fuse_common.parsegen.fb_config.defines import IPQ_CHIPSET, IPQ_CHIPSET_NO_FEC_REG
from sectools.features.fuse_common.parsegen.fb_config.rules import generate_fec, \
    get_field_value, generate_hash_from_file, set_field_value, \
    get_keydata_from_file, find_region_entry_in_datamodel, update_fec_15_10, delete_field


def add_leading_zeros_in_hash(hash_value, size):
    return hash_value.zfill(size)

class _OEM_CONFIG(object):
    '''
    OEM_CONFIG rule class that takes in user config and generates a fuse region object
    based on set rules
    '''
    def __init__(self, chipset='None'):
        '''
        Constructor
        '''
        self.chipset = chipset
        self.entries = []

    def addEntry(self, entry):
        '''
        adds user entries
        '''
        self.entries.append(entry)

    def configure(self, data_model):
        '''
        configures fuse/fields based on name/value
        '''

        '''
        Fuseblower - Secboot Mode Rules :
        QFPROM_RAW_OEM_CONFIG_ROW1_LSB
        Bits     Field Name
        31: 16   OEM_HW_ID
        15: 8    ANTI_ROLLBACK_FEATURE_EN
        7: 4     SPARE0
        3: 0     ROOT_CERT_TOTAL_NUM

        QFPROM_RAW_OEM_CONFIG_ROW1_MSB
        Bits     Field Name     Field Values
        31: 16   SPARE0         READ ONLY
        15: 0    OEM_PRODUCT_ID

        QFPROM_RAW_OEM_CONFIG_ROW0_MSB
        Bit     Field Name
        31        WDOG_EN
        20        DISABLE_ROT_TRANSFER
        1         APPS_NIDEN_DISABLE
        0         APPS_DBGEN_DISABLE

        '''
        if not self.entries:
            return

        try:
            if self.chipset in IPQ_CHIPSET:
                region_id = 'QFPROM_RAW_OEM_CONFIG'
                oem_hwid_9to0 = 0
                oem_hwid_15to10 = 0
                oem_productid_3to0 = 0
                oem_productid_5to4 = 0
                oem_productid_13to6 = 0
                oem_productid_15to14 = 0

                for entry in self.entries:
                    if entry['name'] == 'oem_hwid':
                        oem_hwid = entry['value']
                        oem_hwid = int(oem_hwid, 16)
                        oem_hwid_9to0 = oem_hwid & 0x3FF
                        oem_hwid_15to10 = (oem_hwid >> 10) & 0x3F

                        set_field_value(data_model, region_id, field_id='OEM_HW_ID_9_0', value=oem_hwid_9to0)
                        set_field_value(data_model, region_id, field_id='OEM_HW_ID_15_10', value=oem_hwid_15to10)

                    elif entry['name'] == 'oem_productid':
                        oem_productid = entry['value']
                        oem_productid = int(oem_productid, 16)
                        oem_productid_3to0 = oem_productid & 0xF
                        oem_productid_5to4 = (oem_productid >> 4) & 0x3
                        oem_productid_13to6 = (oem_productid >> 6) & 0xFF
                        oem_productid_15to14 = (oem_productid >> 14) & 0x3

                        set_field_value(data_model, region_id, field_id='OEM_PRODUCT_ID_3_0', value=oem_productid_3to0)
                        set_field_value(data_model, region_id, field_id='OEM_PRODUCT_ID_5_4', value=oem_productid_5to4)
                        set_field_value(data_model, region_id, field_id='OEM_PRODUCT_ID_13_6', value=oem_productid_13to6)
                        set_field_value(data_model, region_id, field_id='OEM_PRODUCT_ID_15_14', value=oem_productid_15to14)

                    elif entry['name'] == 'watchdog_enable':
                        watchdog_enable = 0
                        if entry['value'] == 'true': watchdog_enable = 1
                        set_field_value(data_model, region_id, field_id='WDOG_EN', value=watchdog_enable)

                # Generate FEC bits
                spare0 = get_field_value(data_model, region_id, 'SPARE0')
                spare0 = int(spare0, 16)
                data0_row0 = update_fec_15_10(data_model, oem_hwid_9to0, region_id, 'FEC_VALUE0', 2)
                data1_row0 = update_fec_15_10(data_model, oem_hwid_15to10 + oem_productid_3to0, region_id, 'FEC_VALUE1', 2)
                data2_row0 = update_fec_15_10(data_model, oem_productid_5to4 + oem_productid_13to6, region_id, 'FEC_VALUE2', 2)
                data3_row0 = update_fec_15_10(data_model, oem_productid_15to14 + spare0, region_id, 'FEC_VALUE3', 2)

            else:
                # Fuseblower - Secboot Mode Rules
                for entry in self.entries:
                    if entry['name'] == 'oem_hw_id':
                        oem_hw_id = str(entry['value'])
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='OEM_HW_ID', value=oem_hw_id)

                    elif entry['name'] == 'mrc_cert_count':
                        # CR 612309 - translate mrc_cert_count in config file into MRC count index
                        if '0x' in entry['value']:
                            mrc_cert_count = int(entry['value'],16)
                        else:
                            mrc_cert_count = int(entry['value'])

                        # CR 612309 - adjust blow value of ROOT_CERT_TOTAL_NUM
                        if(mrc_cert_count > 0):
                            mrc_cert_count -= 1

                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='ROOT_CERT_TOTAL_NUM', value=mrc_cert_count)

                        if self.chipset in ROT_SUPPORTED and self.chipset not in MRC_2_0_CHIPSETS:
                            # CR 713126 - DO NOT blow write disable for CPK1 if mrc count is > 1, means ROOT_CERT_TOTAL_NUM blow value is > 0
                            cpk1_write_disable = 1
                            if(mrc_cert_count > 0):
                                cpk1_write_disable = 0
                            set_field_value(data_model, region_id='QFPROM_RAW_RD_WR_PERM', field_id='CUST_PRIV_KEY1', value=cpk1_write_disable)

                    elif entry['name'] == 'oem_product_id':
                        oem_product_id = str(entry['value'])
                        set_field_value(data_model=data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='OEM_PRODUCT_ID', value=oem_product_id)

                    elif entry['name'] == 'apps_debug_disabled':
                        apps_debug_disabled = 0
                        if entry['value'] == 'true': apps_debug_disabled = 1
                        set_field_value(data_model=data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='APPS_DBGEN_DISABLE', value=apps_debug_disabled)

                    elif entry['name'] == 'allow_bus_dcvs':
                        apps_niden_disabled = 0
                        # if enable bus dcvs is false (disabled), only then will APPS_NIDEN_DISABLED be set,
                        if entry['value'] == 'false': apps_niden_disabled = 1
                        set_field_value(data_model=data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='APPS_NIDEN_DISABLE', value=apps_niden_disabled)

                    # Implementation - FR 23467
                    elif entry['name'] == 'watchdog_enable':
                        watchdog_enable = 0
                        if entry['value'] == 'true': watchdog_enable = 1
                        set_field_value(data_model=data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='WDOG_EN', value=watchdog_enable)

                    elif entry['name'] == 'eku_enforcement_en':
                        eku_enforcement = 0
                        if entry['value'] == 'true': eku_enforcement = 1
                        set_field_value(data_model=data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='EKU_ENFORCEMENT_EN', value=eku_enforcement)

                    elif entry['name'] == 'sp_disable':
                        sp_disable = 0
                        if entry['value'] == 'true': sp_disable = 1
                        set_field_value(data_model=data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='SP_DISABLE', value=sp_disable)

                    elif entry['name'] == 'sp_fips_enable':
                        sp_fips_enable = 0
                        if entry['value'] == 'true': sp_fips_enable = 1
                        set_field_value(data_model=data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='SP_FIPS_ENABLE', value=sp_fips_enable)

                    elif entry['name'] == 'tz_sw_crypto_fips_enable':
                        tz_sw_crypto_fips_enable = 0
                        if entry['value'] == 'true': tz_sw_crypto_fips_enable = 1
                        set_field_value(data_model=data_model, region_id='QFPROM_RAW_OEM_CONFIG',
                                        field_id='TZ_SW_CRYPTO_FIPS_ENABLE',
                                        value=tz_sw_crypto_fips_enable)

                    elif entry['name'] == 'tz_sw_crypto_fips_override':
                        tz_sw_crypto_fips_override = 0
                        if entry['value'] == 'true': tz_sw_crypto_fips_override = 1
                        set_field_value(data_model=data_model, region_id='QFPROM_RAW_OEM_CONFIG',
                                        field_id='TZ_SW_CRYPTO_FIPS_OVERRIDE',
                                        value=tz_sw_crypto_fips_override)

                    elif entry['name'] == 'sp_fips_override':
                        sp_fips_override = 0
                        if entry['value'] == 'true': sp_fips_override = 1
                        set_field_value(data_model=data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='SP_FIPS_OVERRIDE', value=sp_fips_override)

                    elif entry['name'] == 'spu_iar_feature_enable':
                        spu_iar_feature_enable = 0
                        if entry['value'] == 'true': spu_iar_feature_enable = 1
                        set_field_value(data_model=data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='SPU_IAR_FEATURE_ENABLE', value=spu_iar_feature_enable)

                    elif entry['name'] == 'disable_rot_transfer':
                        disable_rot_transfer = 0
                        if entry['value'] == 'true': disable_rot_transfer = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='DISABLE_ROT_TRANSFER', value=disable_rot_transfer)

                    elif entry['name'] == 'sp_nvm_ar_config':
                        if '0x' in entry['value']:
                            sp_nvm_ar_config = int(entry['value'],16)
                        else:
                            sp_nvm_ar_config = int(entry['value'])
                        if (sp_nvm_ar_config >= 0 and sp_nvm_ar_config <= 3):
                            set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='SP_NVM_AR_CONFIG', value=sp_nvm_ar_config)
                        else:
                            raise RuntimeError('Invalid sp_nvm_ar_config value. Allowed values are 0, 1, 2, 3')

                    elif entry['name'] == 'anti_rollback_feature_enable':
                        if entry['value'] == 'false':
                            set_field_value(data_model=data_model, region_id='QFPROM_RAW_OEM_CONFIG',
                                            field_id=defines.FIELD_ID_MAP['anti_rollback_feature_enable'],
                                            value=0)

                    elif entry['name'] == 'fast_boot_feature_enable':
                        if entry['value'] == 'false':
                            set_field_value(data_model=data_model, region_id='QFPROM_RAW_OEM_CONFIG',
                                            field_id=defines.FIELD_ID_MAP['fast_boot_feature_enable'],
                                            value=0)

                    elif entry['name'] == 'oem_periph_pid':
                        oem_periph_pid = str(entry['value'])
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='PERIPH_PID', value=oem_periph_pid)

                    elif entry['name'] == 'oem_periph_vid':
                        oem_periph_vid = str(entry['value'])
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='PERIPH_VID', value=oem_periph_vid)

                    elif entry['name'] == 'pcie_subsys_dev_id':
                        pcie_subsys_dev_id = str(entry['value'])
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='PCIE_SUBSYS_DEV_ID', value=pcie_subsys_dev_id)

                    elif entry['name'] == 'subsys_vendor_id':
                        subsys_vendor_id = str(entry['value'])
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='SUBSYS_VENDOR_ID', value=subsys_vendor_id)

                    elif entry['name'] == 'wcss_dbg_disable':
                        wcss_dbg_disable = 0
                        if entry['value'] == 'true': wcss_dbg_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='WCSS_DBG_DISABLE', value=wcss_dbg_disable)

                    elif entry['name'] == 'wcss_spniden_disable':
                        wcss_spniden_disable = 0
                        if entry['value'] == 'true': wcss_spniden_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='WCSS_SPNID_DISABLE', value=wcss_spniden_disable)

                    elif entry['name'] == 'rpm_dbgen_disable':
                        rpm_dbgen_disable = 0
                        if entry['value'] == 'true': rpm_dbgen_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='RPM_DBGEN_DISABLE', value=rpm_dbgen_disable)

                    elif entry['name'] == 'rpm_wcss_nid_disable':
                        rpm_wcss_nid_disable = 0
                        if entry['value'] == 'true': rpm_wcss_nid_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='RPM_WCSS_NID_DISABLE', value=rpm_wcss_nid_disable)

                    elif entry['name'] == 'rpm_dapen_disable':
                        rpm_dapen_disable = 0
                        if entry['value'] == 'true': rpm_dapen_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='RPM_DAPEN_DISABLE', value=rpm_dapen_disable)

                    elif entry['name'] == 'wcss_spiden_disable':
                        wcss_spiden_disable = 0
                        if entry['value'] == 'true': wcss_spiden_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='WCSS_SPID_DISABLE', value=wcss_spiden_disable)

                    elif entry['name'] == 'wcss_nid_disable':
                        wcss_nid_disable = 0
                        if entry['value'] == 'true': wcss_nid_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='WCSS_NID_DISABLE',
                                        value=wcss_nid_disable)

                    elif entry['name'] == 'q6ss_dbg_disable':
                        q6ss_dbg_disable = 0
                        if entry['value'] == 'true': q6ss_dbg_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='Q6SS_DBG_DISABLE',
                                        value=q6ss_dbg_disable)

                    elif entry['name'] == 'apps_spiden_disable':
                        apps_spiden_disable = 0
                        if entry['value'] == 'true': apps_spiden_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='APPS_SPIDEN_DISABLE',
                                        value=apps_spiden_disable)

                    elif entry['name'] == 'apps_spniden_disable':
                        apps_spniden_disable = 0
                        if entry['value'] == 'true': apps_spniden_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='APPS_SPNIDEN_DISABLE',
                                        value=apps_spniden_disable)

                    elif entry['name'] == 'dap_dbgen_disable':
                        dap_dbgen_disable = 0
                        if entry['value'] == 'true': dap_dbgen_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='DAP_DBGEN_DISABLE',
                                        value=dap_dbgen_disable)

                    elif entry['name'] == 'dap_niden_disable':
                        dap_niden_disable = 0
                        if entry['value'] == 'true': dap_niden_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='DAP_NIDEN_DISABLE',
                                        value=dap_niden_disable)

                    elif entry['name'] == 'dap_spiden_disable':
                        dap_spiden_disable = 0
                        if entry['value'] == 'true': dap_spiden_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='DAP_SPIDEN_DISABLE',
                                        value=dap_spiden_disable)

                    elif entry['name'] == 'dap_spniden_disable':
                        dap_spniden_disable = 0
                        if entry['value'] == 'true': dap_spniden_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='DAP_SPNIDEN_DISABLE',
                                        value=dap_spniden_disable)

                    elif entry['name'] == 'dap_deviceen_disable':
                        dap_deviceen_disable = 0
                        if entry['value'] == 'true': dap_deviceen_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='DAP_DEVICEEN_DISABLE',
                                        value=dap_deviceen_disable)

                    elif entry['name'] == 'efuse_ubi32_dbg_disable':
                        efuse_ubi32_dbg_disable = 0
                        if entry['value'] == 'true': efuse_ubi32_dbg_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='EFUSE_UBI32_DBG_DISABLE',
                                        value=efuse_ubi32_dbg_disable)

                    elif entry['name'] == 'q6ss_etm_disable':
                        q6ss_etm_disable = 0
                        if entry['value'] == 'true': q6ss_etm_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='Q6SS_ETM_DISABLE',
                                        value=q6ss_etm_disable)

                    elif entry['name'] == 'q6ss_dfd_disable':
                        q6ss_dfd_disable = 0
                        if entry['value'] == 'true': q6ss_dfd_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='Q6SS_DFD_DISABLE',
                                        value=q6ss_dfd_disable)

                    elif entry['name'] == 'lpass_q6_etm_disable':
                        lpass_q6_etm_disable = 0
                        if entry['value'] == 'true': lpass_q6_etm_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='LPASS_Q6_ETM_DISABLE',
                                        value=lpass_q6_etm_disable)

                    elif entry['name'] == 'lpass_q6_dfd_disable':
                        lpass_q6_dfd_disable = 0
                        if entry['value'] == 'true': lpass_q6_dfd_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='LPASS_Q6_DFD_DISABLE',
                                        value=lpass_q6_dfd_disable)

                    elif entry['name'] == 'fuse_debug_bus_disable':
                        fuse_debug_bus_disable = 0
                        if entry['value'] == 'true': fuse_debug_bus_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='FUSE_DEBUG_BUS_DISABLE',
                                        value=fuse_debug_bus_disable)

                    elif entry['name'] == 'lpass_q6_dbgen_disable':
                        lpass_q6_dbgen_disable = 0
                        if entry['value'] == 'true': lpass_q6_dbgen_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='LPASS_Q6_DBGEN_DISABLE',
                                        value=lpass_q6_dbgen_disable)

                    elif entry['name'] == 'qwes_gtp_wwan_enable':
                        qwes_gtp_wwan_enable = 0
                        if entry['value'] == 'true': qwes_gtp_wwan_enable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='QWES_GTP_WWAN', value=qwes_gtp_wwan_enable)

                    elif entry['name'] == 'qwes_gnss_enable':
                        qwes_gnss_enable = 0
                        if entry['value'] == 'true': qwes_gnss_enable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='QWES_GNSS', value=qwes_gnss_enable)

                    elif entry['name'] == 'qwes_volte_enable':
                        qwes_volte_enable = 0
                        if entry['value'] == 'true': qwes_volte_enable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='QWES_VoLTE', value=qwes_volte_enable)

                    elif entry['name'] == 'shared_qsee_spiden_disable':
                        shared_qsee_spiden_disable = 0
                        if entry['value'] == 'true': shared_qsee_spiden_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='SHARED_QSEE_SPIDEN_DISABLE', value=shared_qsee_spiden_disable)

                    elif entry['name'] == 'shared_qsee_spniden_disable':
                        shared_qsee_spniden_disable = 0
                        if entry['value'] == 'true': shared_qsee_spniden_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='SHARED_QSEE_SPNIDEN_DISABLE', value=shared_qsee_spniden_disable)

                    elif entry['name'] == 'shared_mss_dbgen_disable':
                        shared_mss_dbgen_disable = 0
                        if entry['value'] == 'true': shared_mss_dbgen_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='SHARED_MSS_DBGEN_DISABLE', value=shared_mss_dbgen_disable)

                    elif entry['name'] == 'shared_mss_niden_disable':
                        shared_mss_niden_disable = 0
                        if entry['value'] == 'true': shared_mss_niden_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='SHARED_MSS_NIDEN_DISABLE', value=shared_mss_niden_disable)

                    elif entry['name'] == 'shared_cp_dbgen_disable':
                        shared_cp_dbgen_disable = 0
                        if entry['value'] == 'true': shared_cp_dbgen_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='SHARED_CP_DBGEN_DISABLE', value=shared_cp_dbgen_disable)

                    elif entry['name'] == 'shared_cp_niden_disable':
                        shared_cp_niden_disable = 0
                        if entry['value'] == 'true': shared_cp_niden_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='SHARED_CP_NIDEN_DISABLE', value=shared_cp_niden_disable)

                    elif entry['name'] == 'shared_ns_dbgen_disable':
                        shared_ns_dbgen_disable = 0
                        if entry['value'] == 'true': shared_ns_dbgen_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='SHARED_NS_DBGEN_DISABLE', value=shared_ns_dbgen_disable)

                    elif entry['name'] == 'apps_niden_disable':
                        apps_niden_disable = 0
                        if entry['value'] == 'true': apps_niden_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='APPS_NIDEN_DISABLE', value=apps_niden_disable)

                    elif entry['name'] == 'shared_ns_niden_disable':
                        shared_ns_niden_disable = 0
                        if entry['value'] == 'true': shared_ns_niden_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='SHARED_NS_NIDEN_DISABLE', value=shared_ns_niden_disable)

                    elif entry['name'] == 'shared_misc_debug_disable':
                        shared_misc_debug_disable = 0
                        if entry['value'] == 'true': shared_misc_debug_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='SHARED_MISC_DEBUG_DISABLE', value=shared_misc_debug_disable)

                    elif entry['name'] == 'qwes_qdr2':
                        qwes_qdr2 = 0
                        if entry['value'] == 'true': qwes_qdr2 = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='QWES_QDR2', value=qwes_qdr2)

                    elif entry['name'] == 'qwes_qdr3':
                        qwes_qdr3 = 0
                        if entry['value'] == 'true': qwes_qdr3 = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='QWES_QDR3', value=qwes_qdr3)

                    elif entry['name'] == 'qwes_gnss_single_frequency':
                        qwes_gnss_single_frequency = 0
                        if entry['value'] == 'true': qwes_gnss_single_frequency = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='QWES_GNSS_SINGLE_FREQUENCY', value=qwes_gnss_single_frequency)

                    elif entry['name'] == 'qwes_gnss_multi_frequency':
                        qwes_gnss_multi_frequency = 0
                        if entry['value'] == 'true': qwes_gnss_multi_frequency = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='QWES_GNSS_MULTI_FREQUENCY', value=qwes_gnss_multi_frequency)

                    elif entry['name'] == 'qwes_gnss_over_ethernet_ptp':
                        qwes_gnss_over_ethernet_ptp = 0
                        if entry['value'] == 'true': qwes_gnss_over_ethernet_ptp = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='QWES_GNSS_OVER_ETHERNET_PTP', value=qwes_gnss_over_ethernet_ptp)

                    elif entry['name'] == 'qwes_cv2x_location_basic':
                        qwes_cv2x_location_basic = 0
                        if entry['value'] == 'true': qwes_cv2x_location_basic = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='QWES_CV2X_LOCATION_BASIC', value=qwes_cv2x_location_basic)

                    elif entry['name'] == 'qwes_cv2x_location_premium':
                        qwes_cv2x_location_premium = 0
                        if entry['value'] == 'true': qwes_cv2x_location_premium = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='QWES_CV2X_LOCATION_PREMIUM', value=qwes_cv2x_location_premium)

                    elif entry['name'] == 'qwes_tcedr':
                        qwes_tcedr = 0
                        if entry['value'] == 'true': qwes_tcedr = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='QWES_TCEDR', value=qwes_tcedr)

                    elif entry['name'] == 'qwes_ppme':
                        qwes_ppme = 0
                        if entry['value'] == 'true': qwes_ppme = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='QWES_PPME', value=qwes_ppme)

                    elif entry['name'] == 'disable_qpic_quadio_xfer':
                        disable_qpic_quadio_xfer = 0
                        if entry['value'] == 'true': disable_qpic_quadio_xfer = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='DISABLE_QPIC_QUADIO_XFER', value=disable_qpic_quadio_xfer)

                    elif entry['name'] == 'msmc_mss_dbgen':
                        msmc_mss_dbgen = 0
                        if entry['value'] == 'true': msmc_mss_dbgen = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='MSMC_MSS_DBGEN', value=msmc_mss_dbgen)

                    elif entry['name'] == 'all_debug_disable':
                        all_debug_disable = 0
                        if entry['value'] == 'true': all_debug_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='ALL_DEBUG_DISABLE', value=all_debug_disable)
                    elif entry['name'] == 'e_dload_disable':
                        e_dload_disable = 0
                        if entry['value'] == 'true': e_dload_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='E_DLOAD_DISABLE',
                                        value=e_dload_disable)
                    elif entry['name'] == 'enum_timeout':
                        enum_timeout = 0
                        if entry['value'] == 'true': enum_timeout = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='ENUM_TIMEOUT',
                                        value=enum_timeout)
                    elif entry['name'] == 'force_dload_disable':
                        force_dload_disable = 0
                        if entry['value'] == 'true': force_dload_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='FORCE_DLOAD_DISABLE',
                                        value=force_dload_disable)
                    elif entry['name'] == 'sdcc_adma_disable':
                        sdcc_adma_disable = 0
                        if entry['value'] == 'true': sdcc_adma_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='SDCC_ADMA_DISABLE',
                                        value=sdcc_adma_disable)
                    if entry['name'] == 'fast_boot':
                        if '0x' in entry['value']:
                            fast_boot = int(entry['value'], 16)
                        else:
                            fast_boot = int(entry['value'])
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='FAST_BOOT',
                                        value=fast_boot)
                    elif entry['name'] == 'pbl_qspi_boot_edl_enabled':
                        pbl_qspi_boot_edl_enabled = 0
                        if entry['value'] == 'true': pbl_qspi_boot_edl_enabled = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG',
                                        field_id='PBL_QSPI_BOOT_EDL_ENABLED',value=pbl_qspi_boot_edl_enabled)
                    elif entry['name'] == 'spi_clk_boot_freq':
                        spi_clk_boot_freq = 0
                        if entry['value'] == 'true': spi_clk_boot_freq = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='SPI_CLK_BOOT_FREQ',
                                        value=spi_clk_boot_freq)
                    elif entry['name'] == 'sw_fuse_prog_disable':
                        sw_fuse_prog_disable = 0
                        if entry['value'] == 'true': sw_fuse_prog_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='SW_FUSE_PROG_DISABLE',
                                        value=sw_fuse_prog_disable)
                    elif entry['name'] == 'pbl_fdl_timeout_reset_feature_enable':
                        pbl_fdl_timeout_reset_feature_enable = 0
                        if entry['value'] == 'true': pbl_fdl_timeout_reset_feature_enable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG',
                                        field_id='PBL_FDL_TIMEOUT_RESET_FEATURE_ENABLE',
                                        value=pbl_fdl_timeout_reset_feature_enable)
                    elif entry['name'] == 'pbl_log_disable':
                        pbl_log_disable = 0
                        if entry['value'] == 'true': pbl_log_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='PBL_LOG_DISABLE',
                                        value=pbl_log_disable)
                    elif entry['name'] == 'pbl_usb_type_c_disable':
                        pbl_usb_type_c_disable = 0
                        if entry['value'] == 'true': pbl_usb_type_c_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='PBL_USB_TYPE_C_DISABLE',
                                        value=pbl_usb_type_c_disable)
                    elif entry['name'] == 'usb_ss_disable':
                        usb_ss_disable = 0
                        if entry['value'] == 'true': usb_ss_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='USB_SS_DISABLE',
                                        value=usb_ss_disable)
                    elif entry['name'] == 'debug_disable_in_rom':
                        debug_disable_in_rom = 0
                        if entry['value'] == 'true': debug_disable_in_rom = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='DEBUG_DISABLE_IN_ROM',
                                        value=debug_disable_in_rom)
                    elif entry['name'] == 'debug_policy_disable':
                        debug_policy_disable = 0
                        if entry['value'] == 'true': debug_policy_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='DEBUG_POLICY_DISABLE',
                                        value=debug_policy_disable)
                    elif entry['name'] == 'shared_misc1_debug_disable':
                        shared_misc1_debug_disable = 0
                        if entry['value'] == 'true': shared_misc1_debug_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG',
                                        field_id='SHARED_MISC1_DEBUG_DISABLE', value=shared_misc1_debug_disable)
                    elif entry['name'] == 'shared_misc2_debug_disable':
                        shared_misc2_debug_disable = 0
                        if entry['value'] == 'true': shared_misc2_debug_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG',
                                        field_id='SHARED_MISC2_DEBUG_DISABLE', value=shared_misc2_debug_disable)
                    elif entry['name'] == 'shared_misc3_debug_disable':
                        shared_misc3_debug_disable = 0
                        if entry['value'] == 'true': shared_misc3_debug_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG',
                                        field_id='SHARED_MISC3_DEBUG_DISABLE', value=shared_misc3_debug_disable)
                    elif entry['name'] == 'shared_misc4_debug_disable':
                        shared_misc4_debug_disable = 0
                        if entry['value'] == 'true': shared_misc4_debug_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG',
                                        field_id='SHARED_MISC4_DEBUG_DISABLE', value=shared_misc4_debug_disable)
                    elif entry['name'] == 'shared_misc5_debug_disable':
                        shared_misc5_debug_disable = 0
                        if entry['value'] == 'true': shared_misc5_debug_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG',
                                        field_id='SHARED_MISC5_DEBUG_DISABLE', value=shared_misc5_debug_disable)
                    elif entry['name'] == 'sp_ari_test_mode_feature_enable':
                        sp_ari_test_mode_feature_enable = 0
                        if entry['value'] == 'true': sp_ari_test_mode_feature_enable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG',
                                        field_id='SP_ARI_TEST_MODE_FEATURE_ENABLE',
                                        value=sp_ari_test_mode_feature_enable)
                    elif entry['name'] == 'sp_ari_suspension_mode_feature_enable':
                        sp_ari_suspension_mode_feature_enable = 0
                        if entry['value'] == 'true': sp_ari_suspension_mode_feature_enable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG',
                                        field_id='SP_ARI_SUSPENSION_MODE_FEATURE_ENABLE',
                                        value=sp_ari_suspension_mode_feature_enable)
                    elif entry['name'] == 'spu_iar_enabled':
                        spu_iar_enabled = 0
                        if entry['value'] == 'true': spu_iar_enabled = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='SPU_IAR_ENABLED',
                                        value=spu_iar_enabled)
                    elif entry['name'] == 'disable_rsa':
                        disable_rsa = 0
                        if entry['value'] == 'true': disable_rsa = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='DISABLE_RSA',
                                        value=disable_rsa)
                    elif entry['name'] == 'force_usb_boot_disable':
                        force_usb_boot_disable = 0
                        if entry['value'] == 'true': force_usb_boot_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG',
                                        field_id='FORCE_USB_BOOT_DISABLE', value=force_usb_boot_disable)
                    elif entry['name'] == 'pcie_early_init_en':
                        pcie_early_init_en = 0
                        if entry['value'] == 'true': pcie_early_init_en = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='PCIE_EARLY_INIT_EN',
                                        value=pcie_early_init_en)
                    elif entry['name'] == 'sw_rot_use_serial_num':
                        sw_rot_use_serial_num = 0
                        if entry['value'] == 'true': sw_rot_use_serial_num = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='SW_ROT_USE_SERIAL_NUM',
                                        value=sw_rot_use_serial_num)
                    elif entry['name'] == 'apps_hash_integrity_check_disable':
                        apps_hash_integrity_check_disable = 0
                        if entry['value'] == 'true': apps_hash_integrity_check_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG',
                                        field_id='APPS_HASH_INTEGRITY_CHECK_DISABLE',
                                        value=apps_hash_integrity_check_disable)
                    elif entry['name'] == 'nand_xfer_param':
                        nand_xfer_param = 0
                        if entry['value'] == 'true': nand_xfer_param = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='NAND_XFER_PARAM',
                                        value=nand_xfer_param)
                    elif entry['name'] == 'usb_pipo_disable':
                        usb_pipo_disable = 0
                        if entry['value'] == 'true': usb_pipo_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='USB_PIPO_DISABLE',
                                        value=usb_pipo_disable)
                    elif entry['name'] == 'spare_reg27_secure':
                        spare_reg27_secure = 0
                        if entry['value'] == 'true': spare_reg27_secure = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='SPARE_REG27_SECURE',
                                        value=spare_reg27_secure)
                    elif entry['name'] == 'spare_reg28_secure':
                        spare_reg28_secure = 0
                        if entry['value'] == 'true': spare_reg28_secure = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='SPARE_REG28_SECURE',
                                        value=spare_reg28_secure)
                    elif entry['name'] == 'spare_reg29_secure':
                        spare_reg29_secure = 0
                        if entry['value'] == 'true': spare_reg29_secure = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='SPARE_REG29_SECURE',
                                        value=spare_reg29_secure)
                    elif entry['name'] == 'spare_reg30_secure':
                        spare_reg30_secure = 0
                        if entry['value'] == 'true': spare_reg30_secure = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='SPARE_REG30_SECURE',
                                        value=spare_reg30_secure)
                    elif entry['name'] == 'sp_app_rot_transfer_feature_enabled':
                        sp_app_rot_transfer_feature_enabled = 0
                        if entry['value'] == 'true': sp_app_rot_transfer_feature_enabled = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG',
                                        field_id='SP_APP_ROT_TRANSFER_FEATURE_ENABLED',
                                        value=sp_app_rot_transfer_feature_enabled)
                    elif entry['name'] == 'sp_nvm_ar_split_config':
                        sp_nvm_ar_split_config = 0
                        if entry['value'] == 'true': sp_nvm_ar_split_config = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='SP_NVM_AR_SPLIT_CONFIG',
                                        value=sp_nvm_ar_split_config)
                    elif entry['name'] == 'ddr_bootstrap_gpio':
                        ddr_bootstrap_gpio = 0
                        if entry['value'] == 'true': ddr_bootstrap_gpio = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='DDR_BOOTSTRAP_GPIO',
                                        value=ddr_bootstrap_gpio)
                    elif entry['name'] == 'edl_indicator':
                        edl_indicator = 0
                        if entry['value'] == 'true': edl_indicator = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='EDL_INDICATOR',
                                        value=edl_indicator)
                    elif entry['name'] == 'qspi_dma_disable':
                        qspi_dma_disable = 0
                        if entry['value'] == 'true': qspi_dma_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='QSPI_DMA_DISABLE',
                                        value=qspi_dma_disable)
                    elif entry['name'] == 'segment_hash_function_select_0':
                        segment_hash_function_select_0 = 0
                        if entry['value'] == 'true': segment_hash_function_select_0 = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG',
                                        field_id='SEGMENT_HASH_FUNCTION_SELECT_0',
                                        value=segment_hash_function_select_0)
                    elif entry['name'] == 'segment_hash_function_select_1':
                        segment_hash_function_select_1 = 0
                        if entry['value'] == 'true': segment_hash_function_select_1 = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG',
                                        field_id='SEGMENT_HASH_FUNCTION_SELECT_1',
                                        value=segment_hash_function_select_1)
                    elif entry['name'] == 'segment_hash_function_select_2':
                        segment_hash_function_select_2 = 0
                        if entry['value'] == 'true': segment_hash_function_select_2 = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG',
                                        field_id='SEGMENT_HASH_FUNCTION_SELECT_2',
                                        value=segment_hash_function_select_2)
                    elif entry['name'] == 'segment_hash_function_select_3':
                        segment_hash_function_select_3 = 0
                        if entry['value'] == 'true': segment_hash_function_select_3 = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG',
                                        field_id='SEGMENT_HASH_FUNCTION_SELECT_3',
                                        value=segment_hash_function_select_3)
                    elif entry['name'] == 'sp_ari_suspension_feature_enable':
                        sp_ari_suspension_feature_enable = 0
                        if entry['value'] == 'true': sp_ari_suspension_feature_enable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG',
                                        field_id='SP_ARI_SUSPENSION_FEATURE_ENABLE',
                                        value=sp_ari_suspension_feature_enable)
                    elif entry['name'] == 'rffe_mod1p2_fuse_sel':
                        rffe_mod1p2_fuse_sel = 0
                        if entry['value'] == 'true': rffe_mod1p2_fuse_sel = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='RFFE_MOD1P2_FUSE_SEL',
                                        value=rffe_mod1p2_fuse_sel)
                    elif entry['name'] == 'llcc_dsrw_disable':
                        llcc_dsrw_disable = 0
                        if entry['value'] == 'true': llcc_dsrw_disable = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='LLCC_DSRW_DISABLE',
                                        value=llcc_dsrw_disable)
        except Exception as e:
            raise RuntimeError('Error from: %s, error=%s' % (self.__class__.__name__, str(e)))

    def update_entry(self, data_model, entries, name):
        '''
        retrieves the field value from data_model and update the relevant config entries
        '''
        field_id = defines.FIELD_ID_MAP[name]

        value = get_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id=field_id )
        print(value)

        user_entry = None
        for entry in entries:
            if entry.name == name:
                user_entry = entry
                break

        if name == 'mrc_cert_count':
            if int(value,16) > 0:
                user_entry.value = str(int(value,16)+1)
            else:
                # root_cert_num value is 0
                user_entry.value = '0'
        elif name == 'apps_debug_disabled':
            if int(value,16) == 1:
                user_entry.value = 'true'
            else:
                user_entry.value = 'false'

        elif name == 'allow_bus_dcvs':
            # enable_bus_dcvs affects APPS_NIDEN_DISABLE inversely
            # if APPS_NIDEN_DISABLE value is 1, the 'enable_bus_dcvs' user entry should be false
            if int(value,16) == 1:
                user_entry.value = 'false'
            else:
                user_entry.value = 'true'

        # Implementation - FR 23467
        elif name == 'watchdog_enable':
            if int(value,16) == 1:
                user_entry.value = 'true'
            else:
                user_entry.value = 'false'

        elif name == 'disable_rot_transfer':
            if int(value,16) == 1:
                user_entry.value = 'true'
            else:
                user_entry.value = 'false'

        else:
            user_entry.value = value


class _MRC_2_0(object):
    '''
    MRC 2.0 rule class that takes in user config and generates a fuse region object
    based on set rules
    '''
    def __init__(self, chipset='None'):
        '''
        Constructor
        '''
        self.chipset = chipset
        self.entries = []

    def addEntry(self, entry):
        '''
        adds user entries
        '''
        self.entries.append(entry)

    def configure(self, data_model):
        '''
        configures fuse/fields based on name/value
        '''

        '''
        Fuseblower - MRC 2.0 Rules

        '''
        if not self.entries:
            return

        try:
            # below condition is for chipsets supporting MRC 2.0
            if self.chipset in MRC_2_0_CHIPSETS:
                region_id = 'QFPROM_RAW_MRC_2_0'
                for entry in self.entries:

                    if entry['name'] == 'root_cert_activate_index':
                        if '0x' in entry['value']:
                            root_cert_activate_index = int(entry['value'],16)
                        else:
                            root_cert_activate_index = int(entry['value'])
                        root_cert_activate_index = 1 << root_cert_activate_index

                        set_field_value(data_model, region_id, field_id='ROOT_CERT_ACTIVATION_LIST', value=root_cert_activate_index)

                    elif entry['name'] == 'root_cert_revoke_index':
                        if '0x' in entry['value']:
                            root_cert_revoke_index = int(entry['value'],16)
                        else:
                            root_cert_revoke_index = int(entry['value'])
                        root_cert_revoke_index = 1 << root_cert_revoke_index

                        set_field_value(data_model, region_id, field_id='ROOT_CERT_REVOCATION_LIST', value=root_cert_revoke_index)

        except Exception as e:
            raise RuntimeError('Error from: %s, error=%s' % (self.__class__.__name__, str(e)))

    def update_entry(self, data_model, entries, name):
        '''
        retrieves the field value from data_model and update the relevant config entries
        '''
        field_id = defines.FIELD_ID_MAP[name]

        value = get_field_value(data_model, region_id='QFPROM_RAW_MRC_2_0', field_id=field_id )
        print(value)

        user_entry = None
        for entry in entries:
            if entry.name == name:
                user_entry = entry
                break
        user_entry.value = value


class _OEM_SECURE(object):
    '''
    OEM_SECURE rule class that takes in user config and generates a fuse region object
    based on set rules
    '''
    def __init__(self, chipset='None'):
        '''
        Constructor
        '''
        self.chipset = chipset
        self.entries = []

    def addEntry(self, entry):
        '''
        adds user entries
        '''
        self.entries.append(entry)

    def configure(self, data_model):
        '''
        configures fuse/fields based on name/value
        '''

        '''
        Fuseblower - OEM_SECURE Rules :

        '''
        if not self.entries:
            return

        try:
            # below condition is for Napier QCA chipset
            if self.chipset == '6290' or self.chipset == '6390':
                region_id = 'QFPROM_RAW_OEM_SECURE'
                for entry in self.entries:
                    if entry['name'] == 'oem_hw_id':
                        oem_hw_id = str(entry['value'])
                        set_field_value(data_model, region_id, field_id='OEM_HW_ID', value=oem_hw_id)

                    elif entry['name'] == 'oem_product_id':
                        oem_product_id = str(entry['value'])
                        set_field_value(data_model, region_id, field_id='OEM_PRODUCT_ID', value=oem_product_id)

                    elif entry['name'] == 'watchdog_enable':
                        watchdog_enable = 0
                        if entry['value'] == 'true': watchdog_enable = 1
                        set_field_value(data_model, region_id, field_id='WDOG_EN', value=watchdog_enable)

                    elif entry['name'] == 'total_rot_num':
                        if '0x' in entry['value']:
                            total_rot_num = int(entry['value'],16)
                        else:
                            total_rot_num = int(entry['value'])

                        if(total_rot_num > 0):
                            total_rot_num -= 1

                        set_field_value(data_model, region_id, field_id='TOTAL_ROT_NUM', value=total_rot_num)

                    elif entry['name'] == 'rot_activation':
                        if '0x' in entry['value']:
                            rot_activation = int(entry['value'],16)
                        else:
                            rot_activation = int(entry['value'])
                        rot_activation = 1 << rot_activation

                        set_field_value(data_model, region_id, field_id='ROT_ACTIVATION', value=rot_activation)

                    elif entry['name'] == 'rot_revocation':
                        if '0x' in entry['value']:
                            rot_revocation = int(entry['value'],16)
                        else:
                            rot_revocation = int(entry['value'])
                        rot_revocation = 1 << rot_revocation

                        set_field_value(data_model, region_id, field_id='ROT_REVOCATION', value=rot_revocation)

                    elif entry['name'] == 'SECURE_BOOT0_use_serial_num':
                        SECURE_BOOT0_use_serial_num = 0
                        if entry['value'] == 'true': SECURE_BOOT0_use_serial_num = 1
                        set_field_value(data_model, region_id, field_id='SECURE_BOOT0_USE_SERIAL_NUM', value=SECURE_BOOT0_use_serial_num)

                    elif entry['name'] == 'SECURE_BOOT1_use_serial_num':
                        SECURE_BOOT1_use_serial_num = 0
                        if entry['value'] == 'true': SECURE_BOOT1_use_serial_num = 1
                        set_field_value(data_model, region_id, field_id='SECURE_BOOT1_USE_SERIAL_NUM', value=SECURE_BOOT1_use_serial_num)

                    elif entry['name'] == 'SECURE_BOOT2_use_serial_num':
                        SECURE_BOOT2_use_serial_num = 0
                        if entry['value'] == 'true': SECURE_BOOT2_use_serial_num = 1
                        set_field_value(data_model, region_id, field_id='SECURE_BOOT2_USE_SERIAL_NUM', value=SECURE_BOOT2_use_serial_num)

                    elif entry['name'] == 'SECURE_BOOT3_use_serial_num':
                        SECURE_BOOT3_use_serial_num = 0
                        if entry['value'] == 'true': SECURE_BOOT3_use_serial_num = 1
                        set_field_value(data_model, region_id, field_id='SECURE_BOOT3_USE_SERIAL_NUM', value=SECURE_BOOT3_use_serial_num)

                    elif entry['name'] == 'SECURE_BOOT4_use_serial_num':
                        SECURE_BOOT4_use_serial_num = 0
                        if entry['value'] == 'true': SECURE_BOOT4_use_serial_num = 1
                        set_field_value(data_model, region_id, field_id='SECURE_BOOT4_USE_SERIAL_NUM', value=SECURE_BOOT4_use_serial_num)

                    elif entry['name'] == 'SECURE_BOOT5_use_serial_num':
                        SECURE_BOOT5_use_serial_num = 0
                        if entry['value'] == 'true': SECURE_BOOT5_use_serial_num = 1
                        set_field_value(data_model, region_id, field_id='SECURE_BOOT5_USE_SERIAL_NUM', value=SECURE_BOOT5_use_serial_num)

                    elif entry['name'] == 'SECURE_BOOT6_use_serial_num':
                        SECURE_BOOT6_use_serial_num = 0
                        if entry['value'] == 'true': SECURE_BOOT6_use_serial_num = 1
                        set_field_value(data_model, region_id, field_id='SECURE_BOOT6_USE_SERIAL_NUM', value=SECURE_BOOT6_use_serial_num)

                    elif entry['name'] == 'SECURE_BOOT7_use_serial_num':
                        SECURE_BOOT7_use_serial_num = 0
                        if entry['value'] == 'true': SECURE_BOOT7_use_serial_num = 1
                        set_field_value(data_model, region_id, field_id='SECURE_BOOT7_USE_SERIAL_NUM', value=SECURE_BOOT7_use_serial_num)

                    elif entry['name'] == 'acmt_bypass_disable':
                        acmt_bypass_disable = 0
                        if entry['value'] == 'true': acmt_bypass_disable = 1
                        set_field_value(data_model, region_id, field_id='ACMT_BYPASS_DISABLE', value=acmt_bypass_disable)

                    elif entry['name'] == 'anti_rollback_feature_enable':
                        anti_rollback_feature_enable = 0
                        if entry['value'] == 'true': anti_rollback_feature_enable = 1
                        set_field_value(data_model, region_id, field_id='ANTI_ROLLBACK_FEATURE_ENABLE', value=anti_rollback_feature_enable)

                    elif entry['name'] == 'ram_dump_use_sn':
                        ram_dump_use_sn = 0
                        if entry['value'] == 'true': ram_dump_use_sn = 1
                        set_field_value(data_model, region_id, field_id='RAM_DUMP_USE_SN', value=ram_dump_use_sn)

        except Exception as e:
            raise RuntimeError('Error from: %s, error=%s' % (self.__class__.__name__, str(e)))

    def update_entry(self, data_model, entries, name):
        '''
        retrieves the field value from data_model and update the relevant config entries
        '''
        field_id = defines.FIELD_ID_MAP[name]

        value = get_field_value(data_model, region_id='QFPROM_RAW_OEM_SECURE', field_id=field_id )
        print(value)

        user_entry = None
        for entry in entries:
            if entry.name == name:
                user_entry = entry
                break

        if name == 'total_rot_num':
            if int(value,16) > 0:
                user_entry.value = str(int(value,16)+1)
            else:
                # root_cert_num value is 0
                user_entry.value = '0'
        # Implementation - FR 23467
        elif name == 'watchdog_enable':
            if int(value,16) == 1:
                user_entry.value = 'true'
            else:
                user_entry.value = 'false'
        else:
            user_entry.value = value


class _OEM_SEC_BOOT(object):
    '''
    OEM_SEC_BOOT rule class that takes in user config and generates a fuse region object
    based on set rules
    '''
    def __init__(self, chipset='None'):
        '''
        Constructor
        '''
        self.chipset = chipset
        self.entries = []

    def addEntry(self, entry):
        '''
        adds user entries
        '''
        self.entries.append(entry)

    def configure(self, data_model):
        '''
        configures fuse/fields based on name/value
        '''

        '''
        OEM fuse settings for this secure boot configuration. This can be overridden with the Qualcomm settings if necessary.
        Bit 7: Reserved
        Bit 6: Use Serial Num for secure boot authentication (0: Use OEM ID (Default), 1: Use Serial Num)
        Bit 5: Authentication Enable (0: no auth, 1: auth required)
        Bit 4: PK Hash in Fuse (0: SHA-256 hash of root cert is ROM, 1: SHA-256 hash of root cert to use is in OEM_PK_HASH)
        Bits 3-0: ROM PK Hash Index (If PK Hash in Fuse is 0, then this index selects which of 16 keys in ROM to use)

        MSB
        Bits     Field Name
        30: 24   FEC_VALUE
        23: 16   SEC_BOOT7
        15:  8   SEC_BOOT6
         7:  0   SEC_BOOT5

        LSB
        Bits     Field Name
        31: 24   SEC_BOOT4
        23: 16   SEC_BOOT3
        15:  8   SEC_BOOT2
         7:  0   SEC_BOOT1
        '''
        if not self.entries:

            if self.chipset in IPQ_CHIPSET:
                return

            elif self.chipset in IPQ_CHIPSET_NO_FEC_REG:
                pass

            # To support same rules for fuseblower - Secboot & RoT mode
            elif find_region_entry_in_datamodel(data_model, 'QFPROM_RAW_OEM_SEC_BOOT') == 'QFPROM_RAW_OEM_SEC_BOOT':
                # Just Generate FEC bits
                logger.debug2('No USER config OEM_SEC_BOOT entries to set, just generate FEC for OEM_SEC_BOOT')
                fuse_value = generate_fec(data_model, region_id='QFPROM_RAW_OEM_SEC_BOOT', row=0)
                # Lastly, set FEC_EN bit
                # some chipsets have the FEC_EN bit in the row instead of having FEC_EN region
                if self.chipset in FEC_IN_ROW:
                    if int(fuse_value,16):
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_SEC_BOOT', field_id='OEM_SEC_BOOT_ROW0_FEC_EN', value=0x1)
                elif self.chipset in DIFF_FEC_REG_VT:
                    # set FEC_EN region bit for Region19 (OEM_SEC_BOOT region)
                    if fuse_value:
                        set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION19_FEC_EN', value=0x1)
                elif self.chipset in DIFF_FEC_REG_NAZGUL:
                    # set FEC_EN region bit for Region20 (OEM_SEC_BOOT region)
                    if fuse_value:
                        set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION20_FEC_EN', value=0x1)
                elif self.chipset in DIFF_FEC_REG_TW:
                    # set FEC_EN region bit for Region21 (OEM_SEC_BOOT region)
                    if fuse_value:
                        set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION21_FEC_EN', value=0x1)
                elif self.chipset in DIFF_FEC_REG_NIC or self.chipset in DIFF_FEC_REG_SAI:
                    # set FEC_EN region bit for Region22 (OEM_SEC_BOOT region)
                    if fuse_value:
                        set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN',field_id='REGION22_FEC_EN', value=0x1)
                elif self.chipset in DIFF_FEC_KONA_HURACAN_FUSION or self.chipset in DIFF_FEC_OLYMPIC:
                    # set FEC_EN region bit for Region23 (OEM_SEC_BOOT region)
                    if fuse_value:
                        set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION23_FEC_EN', value=0x1)
                elif self.chipset in DIFF_FEC_MAKENA:
                    # set FEC_EN region bit for Region31 (OEM_SEC_BOOT region)
                    if fuse_value:
                        set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION31_FEC_EN', value=0x1)
                else:
                    # set FEC_EN region bit for Region24 (OEM_SEC_BOOT region)
                    if fuse_value:
                        set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION24_FEC_EN', value=0x1)
                return

            else:
                return

        try:
            sec_boot1_use_serial_num = 0
            sec_boot2_use_serial_num = 0
            sec_boot3_use_serial_num = 0
            sec_boot1_rom_pk_hash_index = 0
            sec_boot2_rom_pk_hash_index = 0
            sec_boot3_rom_pk_hash_index = 0
            sec_boot1_pk_hash_in_fuse = 0
            sec_boot2_pk_hash_in_fuse = 0
            sec_boot3_pk_hash_in_fuse = 0
            sec_boot1_auth_en = 0
            sec_boot2_auth_en = 0
            sec_boot3_auth_en = 0

            for entry in self.entries:
                value = 0
                if 'use_serial_num' in entry['name']:
                    if entry['value'] == 'true': value = 1

                    if 'SEC_BOOT1' in entry['name']:
                        sec_boot1_use_serial_num = value
                    elif 'SEC_BOOT2' in entry['name']:
                        sec_boot2_use_serial_num = value
                    elif 'SEC_BOOT3' in entry['name']:
                        sec_boot3_use_serial_num = value

                elif 'rom_pk_hash_index' in entry['name']:
                    if '0x' in entry['value']:
                        value = int(entry['value'], 16)
                    else:
                        value = int(entry['value'])

                    if 'SEC_BOOT1' in entry['name']:
                        sec_boot1_rom_pk_hash_index = value
                    elif 'SEC_BOOT2' in entry['name']:
                        sec_boot2_rom_pk_hash_index = value
                    elif 'SEC_BOOT3' in entry['name']:
                        sec_boot3_rom_pk_hash_index = value

                elif 'PK_Hash_in_Fuse' in entry['name']:
                    if entry['value'] == 'true' : value = 1

                    if 'SEC_BOOT1' in entry['name']:
                        sec_boot1_pk_hash_in_fuse = value
                    elif 'SEC_BOOT2' in entry['name']:
                        sec_boot2_pk_hash_in_fuse = value
                    elif 'SEC_BOOT3' in entry['name']:
                        sec_boot3_pk_hash_in_fuse = value

                elif 'auth_en' in entry['name']:
                    if entry['value'] == 'true': value = 1
                    if 'SECURE_BOOT1' in entry['name']:
                        sec_boot1_auth_en = value
                    elif 'SECURE_BOOT2' in entry['name']:
                        sec_boot2_auth_en = value
                    elif 'SECURE_BOOT3' in entry['name']:
                        sec_boot3_auth_en = value

            # set SEC_BOOT1
            set_field_value(data_model, region_id='QFPROM_RAW_OEM_SEC_BOOT',
                            field_id='SEC_BOOT1_ROM_PK_Hash_Index',
                            value=sec_boot1_rom_pk_hash_index, ignore_missing_value=True)
            set_field_value(data_model, region_id='QFPROM_RAW_OEM_SEC_BOOT',
                            field_id='SEC_BOOT1_PK_Hash_in_Fuse',
                            value=sec_boot1_pk_hash_in_fuse)
            set_field_value(data_model, region_id='QFPROM_RAW_OEM_SEC_BOOT',
                            field_id='SEC_BOOT1_Use_Serial_Num',
                            value=sec_boot1_use_serial_num)
            set_field_value(data_model, region_id='QFPROM_RAW_OEM_SEC_BOOT', field_id='OEM_SECURE_BOOT1_AUTH_EN',
                            value=sec_boot1_auth_en, ignore_missing_value=True)

            # set SEC_BOOT2
            if sec_boot2_rom_pk_hash_index:
                set_field_value(data_model, region_id='QFPROM_RAW_OEM_SEC_BOOT',
                                field_id='SEC_BOOT2_ROM_PK_Hash_Index',
                                value=sec_boot2_rom_pk_hash_index, ignore_missing_value=True)
            set_field_value(data_model, region_id='QFPROM_RAW_OEM_SEC_BOOT',
                            field_id='SEC_BOOT2_PK_Hash_in_Fuse',
                            value=sec_boot2_pk_hash_in_fuse)
            if sec_boot2_use_serial_num:
                set_field_value(data_model, region_id='QFPROM_RAW_OEM_SEC_BOOT',
                                field_id='SEC_BOOT2_Use_Serial_Num',
                                value=sec_boot2_use_serial_num)
            set_field_value(data_model, region_id='QFPROM_RAW_OEM_SEC_BOOT', field_id='OEM_SECURE_BOOT2_AUTH_EN',
                            value=sec_boot2_auth_en, ignore_missing_value=True)

            # set SEC_BOOT3
            if sec_boot3_rom_pk_hash_index:
                set_field_value(data_model, region_id='QFPROM_RAW_OEM_SEC_BOOT',
                                field_id='SEC_BOOT3_ROM_PK_Hash_Index',
                                value=sec_boot3_rom_pk_hash_index, ignore_missing_value=True)
            set_field_value(data_model, region_id='QFPROM_RAW_OEM_SEC_BOOT',
                            field_id='SEC_BOOT3_PK_Hash_in_Fuse',
                            value=sec_boot3_pk_hash_in_fuse)
            if sec_boot3_use_serial_num:
                set_field_value(data_model, region_id='QFPROM_RAW_OEM_SEC_BOOT',
                                field_id='SEC_BOOT3_Use_Serial_Num',
                                value=sec_boot3_use_serial_num)
            set_field_value(data_model, region_id='QFPROM_RAW_OEM_SEC_BOOT', field_id='OEM_SECURE_BOOT3_AUTH_EN',
                            value=sec_boot3_auth_en, ignore_missing_value=True)

            if self.chipset in IPQ_CHIPSET or self.chipset in IPQ_CHIPSET_NO_FEC_REG:
                return

            # Generate FEC bits
            fuse_value = generate_fec(data_model, region_id='QFPROM_RAW_OEM_SEC_BOOT', row=0)

            # Lastly, set FEC_EN bit
            # some chipsets have the FEC_EN bit in the row instead of having FEC_EN region
            if self.chipset in FEC_IN_ROW:
                if int(fuse_value,16):
                    set_field_value(data_model, region_id='QFPROM_RAW_OEM_SEC_BOOT', field_id='OEM_SEC_BOOT_ROW0_FEC_EN', value=0x1)
            elif self.chipset in DIFF_FEC_REG or self.chipset in DIFF_FEC_REG_CYP:
                # set FEC_EN region bit for Region11 (OEM_SEC_BOOT region)
                set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION11_FEC_EN', value=0x1)
                # In case of MDM9x07/9x06 target, blow corresponding Shadow bit as well
                if self.chipset in FEC_SHADOW:
                    set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION11_FEC_EN_REDUNDANCY', value=0x1)
            elif self.chipset in DIFF_FEC_REG_VT:
                # set FEC_EN region bit for Region19 (OEM_SEC_BOOT region)
                set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION19_FEC_EN', value=0x1)
            elif self.chipset in DIFF_FEC_REG_NAZGUL:
                # set FEC_EN region bit for Region20 (OEM_SEC_BOOT region)
                set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION20_FEC_EN', value=0x1)
            elif self.chipset in DIFF_FEC_REG_TW:
                # set FEC_EN region bit for Region21 (OEM_SEC_BOOT region)
                set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION21_FEC_EN', value=0x1)
            elif self.chipset in DIFF_FEC_REG_NIC or self.chipset in DIFF_FEC_REG_SAI:
                # set FEC_EN region bit for Region22 (OEM_SEC_BOOT region)
                set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION22_FEC_EN', value=0x1)
            elif self.chipset in DIFF_FEC_KONA_HURACAN_FUSION or self.chipset in DIFF_FEC_OLYMPIC:
                # set FEC_EN region bit for Region23 (OEM_SEC_BOOT region)
                set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION23_FEC_EN', value=0x1)
            elif self.chipset in DIFF_FEC_MAKENA:
                # set FEC_EN region bit for Region31 (OEM_SEC_BOOT region)
                set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION31_FEC_EN', value=0x1)
            else:
                # set FEC_EN region bit for Region24 (OEM_SEC_BOOT region)
                set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION24_FEC_EN', value=0x1)

        except Exception as e:
            raise RuntimeError('Error from: %s, error=%s' % (self.__class__.__name__, str(e)))

    def update_entry(self, data_model, entries, name):
        '''
        retrieves the field value from data_model and update the relevant config entries
        '''
        field_id = defines.FIELD_ID_MAP[name]

        value = get_field_value(data_model, region_id='QFPROM_RAW_OEM_SEC_BOOT', field_id=field_id )
        print(value)

        user_entry = None
        for entry in entries:
            if entry.name == name:
                user_entry = entry
                break

        if name == 'use_serial_num':
            if int(value, 16):
                user_entry.value = 'true'
            else:
                user_entry.value = 'false'
        elif name == 'rom_pk_hash_index':
            user_entry.value = str(int(value,16))


class _SPARE_REG19(object):
    '''
    _SPARE_REG19 rule class that takes in user config and generates a fuse region object
    based on set rules
    '''
    def __init__(self, chipset='None'):
        '''
        Constructor
        '''
        self.chipset = chipset
        self.entries = []

    def addEntry(self, entry):
        '''
        adds user entries
        '''
        if entry['ignore'] == False:
            self.entries.append(entry)

    def configure(self, data_model):
        '''
        configures fuse/fields based on name/value
        '''

        '''
        Bits     Field Name
        31: 24   ROOT_CERT_PK_HASH_INDEX
        23: 16   SPARE0
        15: 0    PIL_SUBSYSTEM_47_32

        31:24  ROOT_CERT_PK_HASH_INDEX values
        0:             PRODUCTION_DEVICE_NO_CERTIFICATE_SELECTED
        15(0xF):       DEVICE_FIXED_TO_CERTIFICATE_15
        30(0x1E):      DEVICE_FIXED_TO_CERTIFICATE_14
        45(0x2D):      DEVICE_FIXED_TO_CERTIFICATE_13
        60(0x3C):      DEVICE_FIXED_TO_CERTIFICATE_12
        75(0x4B):      DEVICE_FIXED_TO_CERTIFICATE_11
        90(0x5A):      DEVICE_FIXED_TO_CERTIFICATE_10
        105(0x69):     DEVICE_FIXED_TO_CERTIFICATE_9
        120(0x78):     DEVICE_FIXED_TO_CERTIFICATE_8
        135(0x87):     DEVICE_FIXED_TO_CERTIFICATE_7
        150(0x96):     DEVICE_FIXED_TO_CERTIFICATE_6
        165(0xA5):     DEVICE_FIXED_TO_CERTIFICATE_5
        180(0xB4):     DEVICE_FIXED_TO_CERTIFICATE_4
        195(0xC3):     DEVICE_FIXED_TO_CERTIFICATE_3
        210(0xD2):     DEVICE_FIXED_TO_CERTIFICATE_2
        225(0xE1):     DEVICE_FIXED_TO_CERTIFICATE_1
        240(0xF0):     PRODUCTION_DEVICE_FIXED_TO_CERTIFICATE_0

        '''
        if not self.entries:
            return

        cert = defines.ROOT_CERT_PK_HASH_INDEX['PRODUCTION_DEVICE_NO_CERTIFICATE_SELECTED']

        try:
            for entry in self.entries:
                if entry['name'] == 'mrc_pk_hash_index':
                    if '0x' in entry['value']:
                        index = str(int(entry['value'], 16))
                    else:
                        index = entry['value']
                    if int(index) == 0:
                        cert = defines.ROOT_CERT_PK_HASH_INDEX['PRODUCTION_DEVICE_NO_CERTIFICATE_SELECTED']
                    else:
                        cert = defines.ROOT_CERT_PK_HASH_INDEX['DEVICE_FIXED_TO_CERTIFICATE_'+index]

            set_field_value(data_model, region_id='QFPROM_RAW_SPARE_REG19', field_id='ROOT_CERT_PK_HASH_INDEX', value=cert)

        except Exception as e:
            raise RuntimeError('Error from: %s, error=%s' % (self.__class__.__name__, str(e)))

    def update_entry(self, data_model, entries, name):
        '''
        retrieves the field value from data_model and update the relevant config entries
        '''
        field_id = defines.FIELD_ID_MAP[name]

        value = get_field_value(data_model, region_id='QFPROM_RAW_SPARE_REG19', field_id=field_id )
        print(value)

        user_entry = None
        for entry in entries:
            if entry.name == name:
                user_entry = entry
                break

        if name == 'mrc_pk_hash_index':
            user_entry.value = str(int(value,16) & 0xF)


class _SPARE_REG18(object):
    '''
    _SPARE_REG18 rule class that takes in user config and generates a fuse region object
    based on set rules
    '''
    def __init__(self, chipset='None'):
        '''
        Constructor
        '''
        self.chipset = chipset
        self.entries = []

    def addEntry(self, entry):
        '''
        adds user entries
        '''
        if entry['ignore'] == False:
            self.entries.append(entry)

    def configure(self, data_model):
        '''
        configures fuse/fields based on name/value
        '''

        '''
        Bits     Field Name
        31: 16   WIFI COUNTRY CODE
        15: 8    WIFI BOARD ID
        7: 0     SAFE SWITCH


        '''
        if not self.entries:
            return

        try:
            for entry in self.entries:
                if entry['name'] == 'wifi_board_id':
                    if '0x' in entry['value']:
                        id = int(entry['value'], 16)
                    else:
                        id = int(entry['value'])
                    if int(id) < 0 or int(id) > 0xFF:
                        raise RuntimeError('Wifi Board ID cannot be less than 0 or greater than 0xFF')
                    else:
                        set_field_value(data_model, region_id='QFPROM_RAW_SPARE_REG18', field_id='WIFI_BOARD_ID', value=id)

        except Exception as e:
            raise RuntimeError('Error from: %s, error=%s' % (self.__class__.__name__, str(e)))

    def update_entry(self, data_model, entries, name):
        '''
        retrieves the field value from data_model and update the relevant config entries
        '''
        field_id = defines.FIELD_ID_MAP[name]

        value = get_field_value(data_model, region_id='QFPROM_RAW_SPARE_REG18', field_id=field_id )
        print(value)

        user_entry = None
        for entry in entries:
            if entry.name == name:
                user_entry = entry
                break

        if name == 'wifi_board_id':
            user_entry.value = str(int(value,16) & 0xFF)


class _ANTI_ROLLBACK_2(object):
    '''
    _ANTI_ROLLBACK_2 rule class that takes in user config and generates a fuse region object
    based on set rules
    '''
    def __init__(self, chipset='None'):
        '''
        Constructor
        '''
        self.chipset = chipset
        self.entries = []

    def addEntry(self, entry):
        '''
        adds user entries
        '''
        if entry['ignore'] == False:
            self.entries.append(entry)

    def configure(self, data_model):
        '''
        configures fuse/fields based on name/value

        Bits     Field Name
        63:56   ROOT_CERT_PK_HASH_INDEX

        63:56  ROOT_CERT_PK_HASH_INDEX values
        0:             PRODUCTION_DEVICE_NO_CERTIFICATE_SELECTED
        15(0xF):       DEVICE_FIXED_TO_CERTIFICATE_15
        30(0x1E):      DEVICE_FIXED_TO_CERTIFICATE_14
        45(0x2D):      DEVICE_FIXED_TO_CERTIFICATE_13
        60(0x3C):      DEVICE_FIXED_TO_CERTIFICATE_12
        75(0x4B):      DEVICE_FIXED_TO_CERTIFICATE_11
        90(0x5A):      DEVICE_FIXED_TO_CERTIFICATE_10
        105(0x69):     DEVICE_FIXED_TO_CERTIFICATE_9
        120(0x78):     DEVICE_FIXED_TO_CERTIFICATE_8
        135(0x87):     DEVICE_FIXED_TO_CERTIFICATE_7
        150(0x96):     DEVICE_FIXED_TO_CERTIFICATE_6
        165(0xA5):     DEVICE_FIXED_TO_CERTIFICATE_5
        180(0xB4):     DEVICE_FIXED_TO_CERTIFICATE_4
        195(0xC3):     DEVICE_FIXED_TO_CERTIFICATE_3
        210(0xD2):     DEVICE_FIXED_TO_CERTIFICATE_2
        225(0xE1):     DEVICE_FIXED_TO_CERTIFICATE_1
        240(0xF0):     PRODUCTION_DEVICE_FIXED_TO_CERTIFICATE_0

        '''
        if not self.entries:
            return

        cert = defines.ROOT_CERT_PK_HASH_INDEX['PRODUCTION_DEVICE_NO_CERTIFICATE_SELECTED']

        try:
            for entry in self.entries:
                if entry['name'] == 'mrc_pk_hash_index':
                    if '0x' in entry['value']:
                        index = str(int(entry['value'], 16))
                    else:
                        index = entry['value']
                    if int(index) == 0:
                        cert = defines.ROOT_CERT_PK_HASH_INDEX['PRODUCTION_DEVICE_NO_CERTIFICATE_SELECTED']
                    else:
                        cert = defines.ROOT_CERT_PK_HASH_INDEX['DEVICE_FIXED_TO_CERTIFICATE_'+index]

            set_field_value(data_model, region_id='QFPROM_RAW_ANTI_ROLLBACK_2', field_id='ROOT_CERT_PK_HASH_INDEX', value=cert)

        except Exception as e:
            raise RuntimeError('Error from: %s, error=%s' % (self.__class__.__name__, str(e)))

    def update_entry(self, data_model, entries, name):
        '''
        retrieves the field value from data_model and update the relevant config entries
        '''
        field_id = defines.FIELD_ID_MAP[name]

        value = get_field_value(data_model, region_id='QFPROM_RAW_ANTI_ROLLBACK_2', field_id=field_id )
        print(value)

        user_entry = None
        for entry in entries:
            if entry.name == name:
                user_entry = entry
                break

        if name == 'mrc_pk_hash_index':
            user_entry.value = str(int(value,16) & 0xF)


class _ANTI_ROLLBACK_3(object):
    '''
    _ANTI_ROLLBACK_3 rule class that takes in user config and generates a fuse region object
    based on set rules
    '''
    def __init__(self, chipset='None'):
        '''
        Constructor
        '''
        self.chipset = chipset
        self.entries = []

    def addEntry(self, entry):
        '''
        adds user entries
        '''
        if entry['ignore'] == False:
            self.entries.append(entry)

    def configure(self, data_model):
        '''
        configures fuse/fields based on name/value

        Bits     Field Name
        39:32   MODEM_ROOT_CERT_PK_HASH_INDEX

        39:32  MODEM_ROOT_CERT_PK_HASH_INDEX values
        0:             PRODUCTION_DEVICE_NO_CERTIFICATE_SELECTED
        15(0xF):       DEVICE_FIXED_TO_CERTIFICATE_15
        30(0x1E):      DEVICE_FIXED_TO_CERTIFICATE_14
        45(0x2D):      DEVICE_FIXED_TO_CERTIFICATE_13
        60(0x3C):      DEVICE_FIXED_TO_CERTIFICATE_12
        75(0x4B):      DEVICE_FIXED_TO_CERTIFICATE_11
        90(0x5A):      DEVICE_FIXED_TO_CERTIFICATE_10
        105(0x69):     DEVICE_FIXED_TO_CERTIFICATE_9
        120(0x78):     DEVICE_FIXED_TO_CERTIFICATE_8
        135(0x87):     DEVICE_FIXED_TO_CERTIFICATE_7
        150(0x96):     DEVICE_FIXED_TO_CERTIFICATE_6
        165(0xA5):     DEVICE_FIXED_TO_CERTIFICATE_5
        180(0xB4):     DEVICE_FIXED_TO_CERTIFICATE_4
        195(0xC3):     DEVICE_FIXED_TO_CERTIFICATE_3
        210(0xD2):     DEVICE_FIXED_TO_CERTIFICATE_2
        225(0xE1):     DEVICE_FIXED_TO_CERTIFICATE_1
        240(0xF0):     PRODUCTION_DEVICE_FIXED_TO_CERTIFICATE_0

        '''
        if not self.entries:
            return

        cert = defines.ROOT_CERT_PK_HASH_INDEX['PRODUCTION_DEVICE_NO_CERTIFICATE_SELECTED']

        try:
            for entry in self.entries:
                if entry['name'] == 'modem_mrc_pk_hash_index':
                    if '0x' in entry['value']:
                        index = str(int(entry['value'], 16))
                    else:
                        index = entry['value']
                    if int(index) == 0:
                        cert = defines.ROOT_CERT_PK_HASH_INDEX['PRODUCTION_DEVICE_NO_CERTIFICATE_SELECTED']
                    else:
                        cert = defines.ROOT_CERT_PK_HASH_INDEX['DEVICE_FIXED_TO_CERTIFICATE_'+index]

            set_field_value(data_model, region_id='QFPROM_RAW_ANTI_ROLLBACK_3', field_id='MODEM_ROOT_CERT_PK_HASH_INDEX', value=cert)

        except Exception as e:
            raise RuntimeError('Error from: %s, error=%s' % (self.__class__.__name__, str(e)))

    def update_entry(self, data_model, entries, name):
        '''
        retrieves the field value from data_model and update the relevant config entries
        '''
        field_id = defines.FIELD_ID_MAP[name]

        value = get_field_value(data_model, region_id='QFPROM_RAW_ANTI_ROLLBACK_3', field_id=field_id )
        print(value)

        user_entry = None
        for entry in entries:
            if entry.name == name:
                user_entry = entry
                break

        if name == 'modem_mrc_pk_hash_index':
            user_entry.value = str(int(value,16) & 0xF)


class _OEM_PK_HASH(object):
    '''
    _OEM_PK_HASH rule class that takes in user config and generates a fuse region object
    based on set rules
    '''
    def __init__(self, chipset='None'):
        '''
        Constructor
        '''
        self.chipset = chipset
        self.entries = []

    def addEntry(self, entry):
        '''
        adds user entries
        '''
        if len(self.entries):
            raise RuntimeError('Cannot add '+entry['name']+', '+self.entries[0]['name']+' already exists!')
        self.entries.append(entry)

    def configure(self, data_model):
        '''
        configures fuse/fields based on name/value
        '''

        # This supports both sha256 and sha384

        if not self.entries:
            return

        root_cert_hash = None

        try:
            for entry in self.entries:
                if entry['name'] == 'root_cert_hash':
                    root_cert_hash = entry['value']
                elif entry['name'] == 'root_cert_file' or entry['name'] == 'root_cert_sha384_hash_file':
                    root_cert_file = entry['value']
                    if entry['name'] == 'root_cert_sha384_hash_file':
                        root_cert_hash = generate_hash_from_file(root_cert_file, hash_algo='sha384')
                        is_sha384 = True
                    else:
                        root_cert_hash = generate_hash_from_file(root_cert_file)
                        is_sha384 = False
                    root_cert_hash_int = 0
                    index = 0
                    for value in root_cert_hash:
                        if index == 0:
                            root_cert_hash_int = ord(value)
                        else:
                            root_cert_hash_int = (root_cert_hash_int << 8) | ord(value)
                        index+=1

                    root_cert_hash = hex(root_cert_hash_int)[2:].rstrip('L')
                    if is_sha384 and len(root_cert_hash) < 96:
                        root_cert_hash = add_leading_zeros_in_hash(root_cert_hash, 96)
                    elif not is_sha384 and len(root_cert_hash) < 64:
                        root_cert_hash = add_leading_zeros_in_hash(root_cert_hash, 64)

            if len(root_cert_hash) != 96 and len(root_cert_hash) != 64:
                raise RuntimeError('root_cert_hash is not a valid sha256[ 64 chars] or sha384[ 96 chars], len = '+str(len(root_cert_hash)))

            root_cert_hash_length = len(root_cert_hash)
            root_cert_hash = int(root_cert_hash, 16)

            if root_cert_hash == 0:
                logger.debug2('root_cert_hash is all 0, skipping')
                return

            root_cert_hash_old = hex(root_cert_hash)[2:].rstrip('L').rjust(root_cert_hash_length,'0')
            root_cert_hash = ''
            for idx in range(int(len(root_cert_hash_old)/2)):
                root_cert_hash += root_cert_hash_old[-2:]
                root_cert_hash_old = root_cert_hash_old[:-2]
            root_cert_hash = int(root_cert_hash, 16)

            if self.chipset == '6290' or self.chipset in IPQ_CHIPSET_NO_FEC_REG:
                hash_row0 = root_cert_hash & FEC_DATA_64BIT_MASK
                hash_row1 = (root_cert_hash >> 64) & FEC_DATA_64BIT_MASK
                hash_row2 = (root_cert_hash >> 128) & FEC_DATA_64BIT_MASK
                hash_row3 = (root_cert_hash >> 192) & FEC_DATA_64BIT_MASK
                set_field_value(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', field_id='HASH_DATA_ROW0', value=hash_row0)
                set_field_value(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', field_id='HASH_DATA_ROW1', value=hash_row1)
                set_field_value(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', field_id='HASH_DATA_ROW2', value=hash_row2)
                set_field_value(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', field_id='HASH_DATA_ROW3', value=hash_row3)

            else:
                hash_row0 = root_cert_hash & FEC_DATA_56BIT_MASK
                hash_row1 = (root_cert_hash >> 56) & FEC_DATA_56BIT_MASK
                hash_row2 = (root_cert_hash >> 112) & FEC_DATA_56BIT_MASK
                hash_row3 = (root_cert_hash >> 168) & FEC_DATA_56BIT_MASK
                if root_cert_hash_length != 96:
                    hash_row4 = (root_cert_hash >> 224) & FEC_DATA_32BIT_MASK  # 32 bit mask for ROW4 for 256 bit hash
                else:
                    hash_row4 = (root_cert_hash >> 224) & FEC_DATA_56BIT_MASK
                    hash_row5 = (root_cert_hash >> 280) & FEC_DATA_56BIT_MASK
                    hash_row6 = (root_cert_hash >> 336) & FEC_DATA_56BIT_MASK

                # merge into data model
                set_field_value(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', field_id='HASH_DATA_ROW0', value=hash_row0)
                set_field_value(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', field_id='HASH_DATA_ROW1', value=hash_row1)
                set_field_value(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', field_id='HASH_DATA_ROW2', value=hash_row2)
                set_field_value(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', field_id='HASH_DATA_ROW3', value=hash_row3)
                set_field_value(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', field_id='HASH_DATA_ROW4', value=hash_row4)
                if root_cert_hash_length > 64:
                    set_field_value(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', field_id='HASH_DATA_ROW5', value=hash_row5)
                    set_field_value(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', field_id='HASH_DATA_ROW6', value=hash_row6)

                # Generate FEC bits
                row0 = generate_fec(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', row=0)
                row1 = generate_fec(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', row=1)
                row2 = generate_fec(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', row=2)
                row3 = generate_fec(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', row=3)
                row4 = generate_fec(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', row=4)
                if root_cert_hash_length > 64:
                    row5 = generate_fec(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', row=5)
                    row6 = generate_fec(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', row=6)

                # Lastly, set FEC_EN bit
                # some chipsets have the FEC_EN bit in the row instead of having FEC_EN region
                if self.chipset in FEC_IN_ROW:
                    if int(row0, 16):
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', field_id='PK_HASH_ROW_FEC_EN0', value=0x1)
                    if int(row1, 16):
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', field_id='PK_HASH_ROW_FEC_EN1', value=0x1)
                    if int(row2, 16):
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', field_id='PK_HASH_ROW_FEC_EN2', value=0x1)
                    if int(row3, 16):
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', field_id='PK_HASH_ROW_FEC_EN3', value=0x1)
                    if int(row4, 16):
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', field_id='PK_HASH_ROW_FEC_EN4', value=0x1)
                    if root_cert_hash_length > 64:
                        if int(row5, 16):
                            set_field_value(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', field_id='PK_HASH_ROW_FEC_EN5', value=0x1)
                        if int(row6, 16):
                            set_field_value(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', field_id='PK_HASH_ROW_FEC_EN6', value=0x1)

                elif self.chipset in DIFF_FEC_REG or self.chipset in DIFF_FEC_REG_CYP:
                    # set FEC_EN region bit for Region13 (OEM_PK_HASH region)
                    set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION13_FEC_EN', value=0x1)
                    # In case of MDM9x07/9206 target, blow corresponding Shadow bit as well
                    if self.chipset in FEC_SHADOW:
                        set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION13_FEC_EN_REDUNDANCY', value=0x1)
                elif self.chipset in DIFF_FEC_REG_VT:
                    # set FEC_EN region bit for Region12 (OEM_PK_HASH region)
                    set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION12_FEC_EN', value=0x1)
                elif self.chipset in DIFF_FEC_REG_TW or self.chipset in DIFF_FEC_REG_NIC:
                    # set FEC_EN region bit for Region15 (OEM_PK_HASH region)
                    set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION15_FEC_EN', value=0x1)
                elif self.chipset in DIFF_FEC_HASTINGS:
                    # set FEC_EN region bit for Region5 (OEM_PK_HASH region)
                    set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION5_FEC_EN', value=0x1)
                else:
                    # set FEC_EN region bit for Region23 (OEM_PK_HASH region)
                    set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION23_FEC_EN', value=0x1)

        except Exception as e:
            raise RuntimeError('Error from configure(): %s, error=%s' + self.__class__.__name__, str(e))

    def update_entry(self, data_model, entries, name):
        '''
        retrieves the field value from data_model and update the relevant config entries
        '''
        user_entry = None
        for entry in entries:
            if entry.name == name:
                user_entry = entry
                break

        try:
            if name == 'root_cert_hash':
                row0 = get_field_value(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', field_id='HASH_DATA_ROW0' )
                row1 = get_field_value(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', field_id='HASH_DATA_ROW1' )
                row2 = get_field_value(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', field_id='HASH_DATA_ROW2' )
                row3 = get_field_value(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', field_id='HASH_DATA_ROW3' )
                row4 = get_field_value(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', field_id='HASH_DATA_ROW4' )

                root_cert_hash_temp = (int(row4, 16) << 224) +\
                                      (int(row3, 16) << 168) +\
                                      (int(row2, 16) << 112) +\
                                      (int(row1, 16) <<  56) +\
                                      int(row0, 16)

                logger.debug2('root_cert_hash from data model:\n%s' %hex(root_cert_hash_temp).rstrip('L'))
                # reverse the byte order
                root_cert_hash_old = hex(root_cert_hash_temp)[2:].rstrip('L')
                root_cert_hash = ''

                for idx in range(int(len(root_cert_hash_old)/2)):
                    root_cert_hash += root_cert_hash_old[-2:]
                    root_cert_hash_old = root_cert_hash_old[:-2]

                root_cert_hash = root_cert_hash.rjust(64,'0')

                logger.debug2('update_entry(): root_cert_hash updated in root config:\n%s' %root_cert_hash.rstrip('L'))
                user_entry.value = root_cert_hash.rstrip('L')

        except Exception as e:
            raise RuntimeError('Error from update_entry(): %s, error=%s' + self.__class__.__name__, str(e))


class _PK_HASH0(object):
    '''
    _PK_HASH0 rule class that takes in user config and generates a fuse region object
    based on set rules
    '''
    def __init__(self, chipset='None'):
        '''
        Constructor
        '''
        self.chipset = chipset
        self.entries = []

    def addEntry(self, entry):
        '''
        adds user entries
        '''
        if len(self.entries):
            raise RuntimeError('Cannot add '+entry['name']+', '+self.entries[0]['name']+' already exists!')
        self.entries.append(entry)

    def configure(self, data_model):
        '''
        configures fuse/fields based on name/value
        '''

        # This supports both sha256 and sha384

        if not self.entries:
            return

        pk_hash0 = None

        try:
            for entry in self.entries:
                if entry['name'] == 'root_cert_hash0':
                    pk_hash0 = entry['value']
                elif entry['name'] == 'root_cert_hash0_file' or entry['name'] == 'root_cert_sha384_hash0_file':
                    pk_hash0_file = entry['value']
                    if entry['name'] == 'root_cert_sha384_hash0_file':
                        pk_hash0 = generate_hash_from_file(pk_hash0_file, hash_algo='sha384')
                        is_sha384 = True
                    else:
                        pk_hash0 = generate_hash_from_file(pk_hash0_file)
                        is_sha384 = False
                    pk_hash0_int = 0
                    index = 0
                    for value in pk_hash0:
                        if index == 0:
                            pk_hash0_int = ord(value)
                        else:
                            pk_hash0_int = (pk_hash0_int << 8) | ord(value)
                        index+=1

                    pk_hash0 = hex(pk_hash0_int)[2:].rstrip('L')
                    if is_sha384 and len(pk_hash0) < 96:
                        pk_hash0 = add_leading_zeros_in_hash(pk_hash0, 96)
                    elif not is_sha384 and len(pk_hash0) < 64:
                        pk_hash0 = add_leading_zeros_in_hash(pk_hash0, 64)

            if len(pk_hash0) != 96 and len(pk_hash0) != 64:
                raise RuntimeError('pk_hash0 is not a valid sha256[ 64 chars] or sha384[ 96 chars], len = '+str(len(pk_hash0)))

            pk_hash0_length = len(pk_hash0)
            pk_hash0 = int(pk_hash0, 16)

            if pk_hash0 == 0:
                logger.debug2('root_cert_hash is all 0, skipping')
                return

            pk_hash0_old = hex(pk_hash0)[2:].rstrip('L').rjust(pk_hash0_length,'0')
            pk_hash0 = ''
            for idx in range(int(len(pk_hash0_old)/2)):
                pk_hash0 += pk_hash0_old[-2:]
                pk_hash0_old = pk_hash0_old[:-2]
            pk_hash0 = int(pk_hash0, 16)

            pkhash0_row0 = pk_hash0 & FEC_DATA_64BIT_MASK
            pkhash0_row1 = (pk_hash0 >> 64) & FEC_DATA_64BIT_MASK
            pkhash0_row2 = (pk_hash0 >> 128) & FEC_DATA_64BIT_MASK
            pkhash0_row3 = (pk_hash0 >> 192) & FEC_DATA_64BIT_MASK
            if pk_hash0_length > 64:
                pkhash0_row4 = (pk_hash0 >> 256) & FEC_DATA_64BIT_MASK
                pkhash0_row5 = (pk_hash0 >> 320) & FEC_DATA_64BIT_MASK

            # merge into data model
            set_field_value(data_model, region_id='QFPROM_RAW_PK_HASH0', field_id='HASH0_DATA_ROW0', value=pkhash0_row0)
            set_field_value(data_model, region_id='QFPROM_RAW_PK_HASH0', field_id='HASH0_DATA_ROW1', value=pkhash0_row1)
            set_field_value(data_model, region_id='QFPROM_RAW_PK_HASH0', field_id='HASH0_DATA_ROW2', value=pkhash0_row2)
            set_field_value(data_model, region_id='QFPROM_RAW_PK_HASH0', field_id='HASH0_DATA_ROW3', value=pkhash0_row3)
            if pk_hash0_length > 64:
                set_field_value(data_model, region_id='QFPROM_RAW_PK_HASH0', field_id='HASH0_DATA_ROW4', value=pkhash0_row4)
                set_field_value(data_model, region_id='QFPROM_RAW_PK_HASH0', field_id='HASH0_DATA_ROW5', value=pkhash0_row5)

        except Exception as e:
            raise RuntimeError('Error from configure(): %s, error=%s' + self.__class__.__name__, str(e))

    def update_entry(self, data_model, entries, name):
        '''
        retrieves the field value from data_model and update the relevant config entries
        '''
        # Need to be Implemented
        return


class _PK_HASH1(object):
    '''
    _PK_HASH1 rule class that takes in user config and generates a fuse region object
    based on set rules
    '''
    def __init__(self, chipset='None'):
        '''
        Constructor
        '''
        self.chipset = chipset
        self.entries = []

    def addEntry(self, entry):
        '''
        adds user entries
        '''
        self.entries.append(entry)

    def configure(self, data_model):
        '''
        configures fuse/fields based on name/value
        '''

        # This only supports sha256

        if not self.entries:
            return

        pk_hash1 = None

        try:
            for entry in self.entries:
                if entry['name'] == 'current_sw_rot_apps':
                    current_sw_rot_apps = int(entry['value'])
                    set_field_value(data_model, region_id='QFPROM_RAW_PK_HASH1', field_id='CURRENT_SW_ROT_APPS', value=current_sw_rot_apps)
                elif entry['name'] == 'current_sw_rot_modem':
                    current_sw_rot_modem = int(entry['value'])
                    set_field_value(data_model, region_id='QFPROM_RAW_PK_HASH1', field_id='CURRENT_SW_ROT_MODEM', value=current_sw_rot_modem)
                elif entry['name'] == 'pk_hash1':
                    pk_hash1 = entry['value']
                elif entry['name'] == 'pk_hash1_file':
                    pk_hash1_file = entry['value']
                    pk_hash1 = generate_hash_from_file(pk_hash1_file)
                    pk_hash1_int = 0
                    index = 0
                    for value in pk_hash1:
                        if index == 0:
                            pk_hash1_int = ord(value)
                        else:
                            pk_hash1_int = (pk_hash1_int << 8) | ord(value)
                        index+=1

                    pk_hash1 = hex(pk_hash1_int)[2:].rstrip('L')
                    if len(pk_hash1) < 64:
                        pk_hash1 = add_leading_zeros_in_hash(pk_hash1, 64)

            if len(pk_hash1) != 64:
                raise RuntimeError('pk_hash1 is not a valid sha256[ 64 chars], len = '+str(len(pk_hash1)))

            pk_hash1 = int(pk_hash1, 16)

            if pk_hash1 == 0:
                logger.debug2('root_cert_hash is all 0, skipping')
                row4 = generate_fec(data_model, region_id='QFPROM_RAW_PK_HASH1', row=4)
                if self.chipset in FEC_IN_ROW:
                    if int(row4, 16):
                        set_field_value(data_model, region_id='QFPROM_RAW_PK_HASH1', field_id='PKHASH1_ROW_FEC_EN4', value=0x1)

                elif self.chipset in DIFF_FEC_REG_NAZGUL:
                    # set FEC_EN region bit for Region25 (PK_HASH1 region)
                    set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION25_FEC_EN', value=0x1)

                else:
                    # set FEC_EN region bit for Region30 (PK_HASH1 region)
                    set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION30_FEC_EN', value=0x1)
                return

            pk_hash1_old = hex(pk_hash1)[2:].rstrip('L').rjust(64,'0')
            pk_hash1= ''
            for idx in range(int(len(pk_hash1_old)/2)):
                pk_hash1 += pk_hash1_old[-2:]
                pk_hash1_old = pk_hash1_old[:-2]
            pk_hash1 = int(pk_hash1, 16)

            pkhash1_row0 = pk_hash1 & FEC_DATA_56BIT_MASK
            pkhash1_row1 = (pk_hash1 >> 56) & FEC_DATA_56BIT_MASK
            pkhash1_row2 = (pk_hash1 >> 112) & FEC_DATA_56BIT_MASK
            pkhash1_row3 = (pk_hash1 >> 168) & FEC_DATA_56BIT_MASK
            pkhash1_row4 = (pk_hash1 >> 224) & FEC_DATA_32BIT_MASK  # 32 bit mask for ROW4

            # merge into data model
            set_field_value(data_model, region_id='QFPROM_RAW_PK_HASH1', field_id='HASH1_DATA_ROW0', value=pkhash1_row0)
            set_field_value(data_model, region_id='QFPROM_RAW_PK_HASH1', field_id='HASH1_DATA_ROW1', value=pkhash1_row1)
            set_field_value(data_model, region_id='QFPROM_RAW_PK_HASH1', field_id='HASH1_DATA_ROW2', value=pkhash1_row2)
            set_field_value(data_model, region_id='QFPROM_RAW_PK_HASH1', field_id='HASH1_DATA_ROW3', value=pkhash1_row3)
            set_field_value(data_model, region_id='QFPROM_RAW_PK_HASH1', field_id='HASH1_DATA_ROW4', value=pkhash1_row4)

            # Generate FEC bits
            row0 = generate_fec(data_model, region_id='QFPROM_RAW_PK_HASH1', row=0)
            row1 = generate_fec(data_model, region_id='QFPROM_RAW_PK_HASH1', row=1)
            row2 = generate_fec(data_model, region_id='QFPROM_RAW_PK_HASH1', row=2)
            row3 = generate_fec(data_model, region_id='QFPROM_RAW_PK_HASH1', row=3)
            row4 = generate_fec(data_model, region_id='QFPROM_RAW_PK_HASH1', row=4)

            # Lastly, set FEC_EN bit
            # some chipsets have the FEC_EN bit in the row instead of having FEC_EN region
            if self.chipset in FEC_IN_ROW:
                if int(row0,16):
                    set_field_value(data_model, region_id='QFPROM_RAW_PK_HASH1', field_id='PKHASH1_ROW_FEC_EN0', value=0x1)
                if int(row1,16):
                    set_field_value(data_model, region_id='QFPROM_RAW_PK_HASH1', field_id='PKHASH1_ROW_FEC_EN1', value=0x1)
                if int(row2,16):
                    set_field_value(data_model, region_id='QFPROM_RAW_PK_HASH1', field_id='PKHASH1_ROW_FEC_EN2', value=0x1)
                if int(row3,16):
                    set_field_value(data_model, region_id='QFPROM_RAW_PK_HASH1', field_id='PKHASH1_ROW_FEC_EN3', value=0x1)
                if int(row4,16):
                    set_field_value(data_model, region_id='QFPROM_RAW_PK_HASH1', field_id='PKHASH1_ROW_FEC_EN4', value=0x1)

            elif self.chipset in DIFF_FEC_REG_NAZGUL:
                # set FEC_EN region bit for Region25 (PK_HASH1 region)
                set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION25_FEC_EN', value=0x1)

            else:
                # set FEC_EN region bit for Region30 (PK_HASH1 region)
                set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION30_FEC_EN', value=0x1)

        except Exception as e:
            raise RuntimeError('Error from configure(): %s, error=%s' + self.__class__.__name__, str(e))

    def update_entry(self, data_model, entries, name):
        '''
        retrieves the field value from data_model and update the relevant config entries
        '''
        # Need to be Implemented
        return


class _IMAGE_ENCR_KEY1(object):
    '''
    _IMAGE_ENCR_KEY1 rule class that takes in user config and generates a fuse region object
    based on set rules
    '''
    def __init__(self, chipset='None'):
        '''
        Constructor
        '''
        self.chipset = chipset
        self.entries = []

    def addEntry(self, entry):
        '''
        adds user entries
        '''
        if len(self.entries):
            raise RuntimeError('Cannot add '+entry['name']+', '+self.entries[0]['name']+' already exists!')
        self.entries.append(entry)

    def configure(self, data_model):
        '''
        configures fuse/fields based on name/value
        '''
        if not self.entries:
            return

        image_encr_key1 = None

        try:
            for entry in self.entries:
                if entry['name'] == 'image_encr_key1':
                    image_encr_key1 = entry['value']
                elif entry['name'] == 'image_encr_key1_file':
                    image_encr_key1_file = entry['value']
                    image_encr_key1 = get_keydata_from_file(image_encr_key1_file)

            if len(image_encr_key1) > 32:
                raise RuntimeError('image_encr_key1 is more than 128 bits [ 32 chars], len = '+str(len(image_encr_key1)))

            image_encr_key1 = int(image_encr_key1, 16)

            if image_encr_key1 == 0:
                logger.debug2('image_encr_key1 is all 0, skipping')
                return

            image_encr_key1_old = hex(image_encr_key1)[2:].rstrip('L').rjust(32,'0')
            image_encr_key1 = ''
            for idx in range(int(len(image_encr_key1_old)/2)):
                image_encr_key1 += image_encr_key1_old[-2:]
                image_encr_key1_old = image_encr_key1_old[:-2]
            image_encr_key1 = int(image_encr_key1, 16)

            imageencrkey1_row0 = image_encr_key1 & FEC_DATA_56BIT_MASK
            imageencrkey1_row1 = (image_encr_key1 >> 56) & FEC_DATA_56BIT_MASK
            imageencrkey1_row2 = (image_encr_key1 >> 112) & 0xFFFF

            # merge into data model
            set_field_value(data_model, region_id='QFPROM_RAW_IMAGE_ENCR_KEY1', field_id='KEY_DATA_ROW0', value=imageencrkey1_row0)
            set_field_value(data_model, region_id='QFPROM_RAW_IMAGE_ENCR_KEY1', field_id='KEY_DATA_ROW1', value=imageencrkey1_row1)
            set_field_value(data_model, region_id='QFPROM_RAW_IMAGE_ENCR_KEY1', field_id='KEY_DATA_ROW2', value=imageencrkey1_row2)

            # Generate FEC bits
            row0 = generate_fec(data_model, region_id='QFPROM_RAW_IMAGE_ENCR_KEY1', row=0)
            row1 = generate_fec(data_model, region_id='QFPROM_RAW_IMAGE_ENCR_KEY1', row=1)
            row2 = generate_fec(data_model, region_id='QFPROM_RAW_IMAGE_ENCR_KEY1', row=2)

            # Lastly, set FEC_EN bit
            # some chipsets have the FEC_EN bit in the row instead of having FEC_EN region
            if self.chipset in FEC_IN_ROW:
                if int(row0,16):
                    set_field_value(data_model, region_id='QFPROM_RAW_IMAGE_ENCR_KEY1', field_id='IMAGEENCRKEY1_ROW0_FEC_EN0', value=0x1)
                if int(row1,16):
                    set_field_value(data_model, region_id='QFPROM_RAW_IMAGE_ENCR_KEY1', field_id='IMAGEENCRKEY1_ROW1_FEC_EN1', value=0x1)
                if int(row2,16):
                    set_field_value(data_model, region_id='QFPROM_RAW_IMAGE_ENCR_KEY1', field_id='IMAGEENCRKEY1_ROW2_FEC_EN2', value=0x1)

            elif self.chipset in DIFF_FEC_REG_NAZGUL:
                # set FEC_EN region bit for Region24 (IMAGE_ENCR_KEY1 region)
                set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION24_FEC_EN', value=0x1)
            else:
                # set FEC_EN region bit for Region29 (IMAGE_ENCR_KEY1 region)
                set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION29_FEC_EN', value=0x1)

        except Exception as e:
            raise RuntimeError('Error from configure(): %s, error=%s' + self.__class__.__name__, str(e))

    def update_entry(self, data_model, entries, name):
        '''
        retrieves the field value from data_model and update the relevant config entries
        '''
        # Need to be Implemented
        return


class _OEM_PKHASH(object):
    '''
    _OEM_PKHASH rule class that takes in user config and generates a fuse region object
    based on set rules
    '''
    def __init__(self, chipset='None'):
        '''
        Constructor
        '''
        self.chipset = chipset
        self.entries = []

    def addEntry(self, entry):
        '''
        adds user entries
        '''
        if len(self.entries):
            raise RuntimeError('Cannot add '+entry['name']+', '+self.entries[0]['name']+' already exists!')
        self.entries.append(entry)

    def configure(self, data_model):
        '''
        configures fuse/fields based on name/value
        '''

        # This only supports sha256

        if not self.entries:
            return

        root_certhash = None

        try:
            for entry in self.entries:
                if entry['name'] == 'root_certhash':
                    root_certhash = entry['value']
                elif entry['name'] == 'root_certfile':
                    root_certfile = entry['value']
                    root_certhash = generate_hash_from_file(root_certfile)
                    root_certhash_int = 0
                    index = 0
                    for value in root_certhash:
                        if index == 0:
                            root_certhash_int = ord(value)
                        else:
                            root_certhash_int = (root_certhash_int << 8) | ord(value)
                        index+=1

                    root_certhash = hex(root_certhash_int)[2:].rstrip('L')
                    if len(root_certhash) < 64:
                        root_certhash = add_leading_zeros_in_hash(root_certhash, 64)

            if len(root_certhash) != 64:
                raise RuntimeError('root_certhash is not a valid sha256[ 64 chars], len = '+str(len(root_certhash)))

            root_certhash = int(root_certhash, 16)

            if root_certhash == 0:
                logger.debug2('root_certhash is all 0, skipping')
                return

            root_certhash_old = hex(root_certhash)[2:].rstrip('L').rjust(64,'0')
            root_certhash = ''
            for idx in range(int(len(root_certhash_old)/2)):
                root_certhash += root_certhash_old[-2:]
                root_certhash_old = root_certhash_old[:-2]
            root_certhash = int(root_certhash, 16)

            hash_data0_row0 = root_certhash & 0x3FF
            hash_data1_row0 = (root_certhash >> 10) & 0x3FF
            hash_data2_row0 = (root_certhash >> 20) & 0x3
            hash_data3_row0 = (root_certhash >> 22) & 0xFF
            hash_data4_row0 = (root_certhash >> 30) & 0x3FF

            hash_data0_row1 = (root_certhash >> 40) & 0x3FF
            hash_data1_row1 = (root_certhash >> 50) & 0x3FF
            hash_data2_row1 = (root_certhash >> 60) & 0x3
            hash_data3_row1 = (root_certhash >> 62) & 0xFF
            hash_data4_row1 = (root_certhash >> 70) & 0x3FF

            hash_data0_row2 = (root_certhash >> 80) & 0x3FF
            hash_data1_row2 = (root_certhash >> 90) & 0x3FF
            hash_data2_row2 = (root_certhash >> 100) & 0x3
            hash_data3_row2 = (root_certhash >> 102) & 0xFF
            hash_data4_row2 = (root_certhash >> 110) & 0x3FF

            hash_data0_row3 = (root_certhash >> 120) & 0x3FF
            hash_data1_row3 = (root_certhash >> 130) & 0x3FF
            hash_data2_row3 = (root_certhash >> 140) & 0x3
            hash_data3_row3 = (root_certhash >> 142) & 0xFF
            hash_data4_row3 = (root_certhash >> 150) & 0x3FF

            hash_data0_row4 = (root_certhash >> 160) & 0x3FF
            hash_data1_row4 = (root_certhash >> 170) & 0x3FF
            hash_data2_row4 = (root_certhash >> 180) & 0x3
            hash_data3_row4 = (root_certhash >> 182) & 0xFF
            hash_data4_row4 = (root_certhash >> 190) & 0x3FF

            hash_data0_row5 = (root_certhash >> 200) & 0x3FF
            hash_data1_row5 = (root_certhash >> 210) & 0x3FF
            hash_data2_row5 = (root_certhash >> 220) & 0x3
            hash_data3_row5 = (root_certhash >> 222) & 0xFF
            hash_data4_row5 = (root_certhash >> 230) & 0x3FF

            hash_data0_row6 = (root_certhash >> 240) & 0x3FF
            hash_data1_row6 = (root_certhash >> 250) & 0x3F

            # merge into data model
            region_id = 'QFPROM_RAW_OEM_PKHASH'
            set_field_value(data_model, region_id, field_id='HASH_DATA0_ROW0', value=hash_data0_row0)
            set_field_value(data_model, region_id, field_id='HASH_DATA1_ROW0', value=hash_data1_row0)
            set_field_value(data_model, region_id, field_id='HASH_DATA2_ROW0', value=hash_data2_row0)
            set_field_value(data_model, region_id, field_id='HASH_DATA3_ROW0', value=hash_data3_row0)
            set_field_value(data_model, region_id, field_id='HASH_DATA4_ROW0', value=hash_data4_row0)

            set_field_value(data_model, region_id, field_id='HASH_DATA0_ROW1', value=hash_data0_row1)
            set_field_value(data_model, region_id, field_id='HASH_DATA1_ROW1', value=hash_data1_row1)
            set_field_value(data_model, region_id, field_id='HASH_DATA2_ROW1', value=hash_data2_row1)
            set_field_value(data_model, region_id, field_id='HASH_DATA3_ROW1', value=hash_data3_row1)
            set_field_value(data_model, region_id, field_id='HASH_DATA4_ROW1', value=hash_data4_row1)

            set_field_value(data_model, region_id, field_id='HASH_DATA0_ROW2', value=hash_data0_row2)
            set_field_value(data_model, region_id, field_id='HASH_DATA1_ROW2', value=hash_data1_row2)
            set_field_value(data_model, region_id, field_id='HASH_DATA2_ROW2', value=hash_data2_row2)
            set_field_value(data_model, region_id, field_id='HASH_DATA3_ROW2', value=hash_data3_row2)
            set_field_value(data_model, region_id, field_id='HASH_DATA4_ROW2', value=hash_data4_row2)

            set_field_value(data_model, region_id, field_id='HASH_DATA0_ROW3', value=hash_data0_row3)
            set_field_value(data_model, region_id, field_id='HASH_DATA1_ROW3', value=hash_data1_row3)
            set_field_value(data_model, region_id, field_id='HASH_DATA2_ROW3', value=hash_data2_row3)
            set_field_value(data_model, region_id, field_id='HASH_DATA3_ROW3', value=hash_data3_row3)
            set_field_value(data_model, region_id, field_id='HASH_DATA4_ROW3', value=hash_data4_row3)

            set_field_value(data_model, region_id, field_id='HASH_DATA0_ROW4', value=hash_data0_row4)
            set_field_value(data_model, region_id, field_id='HASH_DATA1_ROW4', value=hash_data1_row4)
            set_field_value(data_model, region_id, field_id='HASH_DATA2_ROW4', value=hash_data2_row4)
            set_field_value(data_model, region_id, field_id='HASH_DATA3_ROW4', value=hash_data3_row4)
            set_field_value(data_model, region_id, field_id='HASH_DATA4_ROW4', value=hash_data4_row4)

            set_field_value(data_model, region_id, field_id='HASH_DATA0_ROW5', value=hash_data0_row5)
            set_field_value(data_model, region_id, field_id='HASH_DATA1_ROW5', value=hash_data1_row5)
            set_field_value(data_model, region_id, field_id='HASH_DATA2_ROW5', value=hash_data2_row5)
            set_field_value(data_model, region_id, field_id='HASH_DATA3_ROW5', value=hash_data3_row5)
            set_field_value(data_model, region_id, field_id='HASH_DATA4_ROW5', value=hash_data4_row5)

            set_field_value(data_model, region_id, field_id='HASH_DATA0_ROW6', value=hash_data0_row6)
            set_field_value(data_model, region_id, field_id='HASH_DATA1_ROW6', value=hash_data1_row6)

            # Generate FEC bits
            data0_row0 = update_fec_15_10(data_model, hash_data0_row0, region_id, 'FEC_VALUE0_ROW0', 0)
            data1_row0 = update_fec_15_10(data_model, hash_data1_row0, region_id, 'FEC_VALUE1_ROW0', 0)
            data2_row0 = update_fec_15_10(data_model, ((0x3FF & hash_data3_row0) << 2)| hash_data2_row0, region_id, 'FEC_VALUE2_ROW0', 0)
            data3_row0 = update_fec_15_10(data_model, hash_data4_row0, region_id, 'FEC_VALUE3_ROW0', 0)

            data0_row1 = update_fec_15_10(data_model, hash_data0_row1, region_id, 'FEC_VALUE0_ROW1', 1)
            data1_row1 = update_fec_15_10(data_model, hash_data1_row1, region_id, 'FEC_VALUE1_ROW1', 1)
            data2_row1 = update_fec_15_10(data_model, ((0x3FF & hash_data3_row1) <<2) | hash_data2_row1, region_id, 'FEC_VALUE2_ROW1', 1)
            data3_row1 = update_fec_15_10(data_model, hash_data4_row1, region_id, 'FEC_VALUE3_ROW1', 1)

            data0_row2 = update_fec_15_10(data_model, hash_data0_row2, region_id,'FEC_VALUE0_ROW2', 2)
            data1_row2 = update_fec_15_10(data_model, hash_data1_row2, region_id,'FEC_VALUE1_ROW2', 2)
            data2_row2 = update_fec_15_10(data_model, ((0x3FF & hash_data3_row2) <<2) | hash_data2_row2, region_id,'FEC_VALUE2_ROW2', 2)
            data3_row2 = update_fec_15_10(data_model, hash_data4_row2, region_id,'FEC_VALUE3_ROW2', 2)

            data0_row3 = update_fec_15_10(data_model, hash_data0_row3, region_id,'FEC_VALUE0_ROW3', 3)
            data1_row3 = update_fec_15_10(data_model, hash_data1_row3, region_id,'FEC_VALUE1_ROW3', 3)
            data2_row3 = update_fec_15_10(data_model, ((0x3FF & hash_data3_row3) <<2) | hash_data2_row3, region_id,'FEC_VALUE2_ROW3', 3)
            data3_row3 = update_fec_15_10(data_model, hash_data4_row3, region_id,'FEC_VALUE3_ROW3', 3)

            data0_row4 = update_fec_15_10(data_model, hash_data0_row4, region_id,'FEC_VALUE0_ROW4', 4)
            data1_row4 = update_fec_15_10(data_model, hash_data1_row4, region_id,'FEC_VALUE1_ROW4', 4)
            data2_row4 = update_fec_15_10(data_model, ((0x3FF & hash_data3_row4) <<2) | hash_data2_row4, region_id,'FEC_VALUE2_ROW4', 4)
            data3_row4 = update_fec_15_10(data_model, hash_data4_row4, region_id,'FEC_VALUE3_ROW4', 4)

            data0_row5 = update_fec_15_10(data_model, hash_data0_row5, region_id,'FEC_VALUE0_ROW5', 5)
            data1_row5 = update_fec_15_10(data_model, hash_data1_row5, region_id,'FEC_VALUE1_ROW5', 5)
            data2_row5 = update_fec_15_10(data_model, ((0x3FF & hash_data3_row5) <<2) | hash_data2_row5, region_id,'FEC_VALUE2_ROW5', 5)
            data3_row5 = update_fec_15_10(data_model, hash_data4_row5, region_id,'FEC_VALUE3_ROW5', 5)

            data0_row6 = update_fec_15_10(data_model, hash_data0_row6, region_id,'FEC_VALUE0_ROW6', 6)
            data1_row6 = update_fec_15_10(data_model, hash_data1_row6, region_id,'FEC_VALUE1_ROW6', 6)

            # Lastly, set FEC_EN bit
            # some chipsets have the FEC_EN bit in the row instead of having FEC_EN region
            if self.chipset in IPQ_CHIPSET:
                if int(data0_row0,16) or int(data1_row0,16) or int(data2_row0,16) or int(data3_row0,16):
                    set_field_value(data_model, region_id, field_id='FEC_EN_ROW0', value=0x7)
                if int(data0_row1,16) or int(data1_row1,16) or int(data2_row1,16) or int(data3_row1,16):
                    set_field_value(data_model, region_id, field_id='FEC_EN_ROW1', value=0x7)
                if int(data0_row2,16) or int(data1_row2,16) or int(data2_row2,16) or int(data3_row2,16):
                    set_field_value(data_model, region_id, field_id='FEC_EN_ROW2', value=0x7)
                if int(data0_row3,16) or int(data1_row3,16) or int(data2_row3,16) or int(data3_row3,16):
                    set_field_value(data_model, region_id, field_id='FEC_EN_ROW3', value=0x7)
                if int(data0_row4,16) or int(data1_row4,16) or int(data2_row4,16) or int(data3_row4,16):
                    set_field_value(data_model, region_id, field_id='FEC_EN_ROW4', value=0x7)
                if int(data0_row5,16) or int(data1_row5,16) or int(data2_row5,16) or int(data3_row5,16):
                    set_field_value(data_model, region_id, field_id='FEC_EN_ROW5', value=0x7)
                if int(data0_row6,16) or int(data1_row6,16):
                    set_field_value(data_model, region_id, field_id='FEC_EN_ROW6', value=0x7)

        except Exception as e:
            raise RuntimeError('Error from configure(): %s, error=%s' + self.__class__.__name__, str(e))

    def update_entry(self, data_model, entries, name):
        '''
        retrieves the field value from data_model and update the relevant config entries
        '''
        # Need to be Implemented
        return


class _OEM_SPARE(object):
    '''
    OEM Spare Region rule class that takes in user config and generates a fuse region object
    based on set rules
    '''
    def __init__(self, chipset='None'):
        '''
        Constructor
        '''
        self.chipset = chipset
        self.entries = []

    def addEntry(self, entry):
        '''
        adds user entries
        '''
        self.entries.append(entry)

    def configure(self, data_model):
        '''
        configures fuse/fields based on name/value
        '''
        if not self.entries:
            return

        for entry in self.entries:
            if entry['value'] == 'true':
                set_field_value(data_model, region_id='QFPROM_RAW_OEM_SPARE', field_id=entry['name'].upper(), value=0)
            else:
                delete_field(data_model, region_id='QFPROM_RAW_OEM_SPARE', field_id=entry['name'].upper())


class _WR_PERM(object):
    '''
    Read Write Permission Region Region rule class that takes in user config and generates a fuse region object
    based on set rules
    '''
    def __init__(self, chipset='None'):
        '''
        Constructor
        '''
        self.chipset = chipset
        self.entries = []

    def addEntry(self, entry):
        '''
        adds user entries
        '''
        self.entries.append(entry)

    def configure(self, data_model):
        '''
        configures fuse/fields based on name/value
        '''
        if not self.entries:
            return

        for entry in self.entries:
            if entry['name'] == 'anti_rollback_write_disable':
                value = 0
                if entry['value'] == 'true':
                    value = 0x1f
                set_field_value(data_model, region_id='QFPROM_RAW_WR_PERM', field_id=entry['name'].upper(), value=value)


class _RULES(object):
    """
    takes in the user entries and routes them to their proper class for handling
    """
    def __init__(self, chipset='None'):
        self.chipset = chipset
        self.regions = {}
        self.regions['QFPROM_RAW_MRC_2_0'] = _MRC_2_0(chipset)
        self.regions['QFPROM_RAW_OEM_SECURE'] = _OEM_SECURE(chipset)
        self.regions['QFPROM_RAW_OEM_CONFIG'] = _OEM_CONFIG(chipset)
        self.regions['QFPROM_RAW_SPARE_REG19'] = _SPARE_REG19(chipset)
        self.regions['QFPROM_RAW_OEM_PK_HASH'] = _OEM_PK_HASH(chipset)
        self.regions['QFPROM_RAW_OEM_SEC_BOOT'] = _OEM_SEC_BOOT(chipset)
        self.regions['QFPROM_RAW_ANTI_ROLLBACK_2'] = _ANTI_ROLLBACK_2(chipset)
        self.regions['QFPROM_RAW_ANTI_ROLLBACK_3'] = _ANTI_ROLLBACK_3(chipset)
        self.regions['QFPROM_RAW_PK_HASH0'] = _PK_HASH0(chipset)
        self.regions['QFPROM_RAW_PK_HASH1'] = _PK_HASH1(chipset)
        self.regions['QFPROM_RAW_IMAGE_ENCR_KEY1'] = _IMAGE_ENCR_KEY1(chipset)
        self.regions['QFPROM_RAW_OEM_PKHASH'] = _OEM_PKHASH(chipset)
        self.regions['QFPROM_RAW_SPARE_REG18'] = _SPARE_REG18(chipset)
        self.regions['QFPROM_RAW_OEM_SPARE'] = _OEM_SPARE(chipset)
        self.regions['QFPROM_RAW_WR_PERM'] = _WR_PERM(chipset)

    def process(self, entries, data_model):
        """ handles the entries according to their fuse region class """
        try:
            # push the entries to their corresponding regions
            for entry in entries:
                if entry['ignore'] is False:
                    # find the region that the entry belongs to and add that entry
                    region_id = self.find_region_from_entry(data_model, entry['name'])

                    if region_id is not None:
                        self.regions[region_id].addEntry(entry)
                    else:
                        raise RuntimeError('UNSUPPORTED user entry: ' + entry['name'] + ', please check USER.xml file')

            # configure regions based on entries
            for region in self.regions:
                self.regions[region].configure(data_model)
        except Exception as e:
            raise RuntimeError('Error from: %s, error=%s' % (self.__class__.__name__, str(e)))

    def update_from_data_model(self, data_model, entries):

        try:
            for entry in entries:
                if entry.ignore == False:
                    name = entry.name
                    region_id = self.find_region_from_entry(data_model, name)

                    if region_id is not None:
                        self.regions[region_id].update_entry(data_model, entries, name)
                    else:
                        raise RuntimeError('UNSUPPORTED user entry: ' + name + ', please check USER.xml file')

        except Exception as e:
            raise RuntimeError('Error from: %s, error=%s' % (self.__class__.__name__, str(e)))

    def find_region_from_entry(self, data_model, name):
        field_id = defines.FIELD_ID_MAP[name]
        for region in data_model.module[0].fuse_region:
            for fuse in region.fuse:
                for field in fuse.field:
                    if field.id == field_id:
                        return region.id

        # got to this point, not found
        raise RuntimeError("USER config entry field not found in data_model: "+field_id)
