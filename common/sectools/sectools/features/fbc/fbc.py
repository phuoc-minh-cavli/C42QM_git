# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
"""
:Authors: sectools

Main interface to the fuseblower functionalities. Supports APIs for
command line interface, UI interface and test interface.
"""

from sectools.common.core.feature import FeatureBase_Abs
from sectools.common.parsegen.secdat.segment.defines import SEG_TYPE_EFUSE,\
    SEG_TYPE_ROT
from sectools.common.utils.c_logging import logger
from sectools.features.fbc.parsegen import fb_config
from sectools.features.fuse_common.fcc import FuseCommonCore


FUSEBLOWER_ID = 'fuseblower'


class FuseBlowerCore(FeatureBase_Abs, FuseCommonCore):
    """Provides interface for user to set up parameters (including input files,
    output directory, etc) and perform fuse blowing related operations
    (generation, validation of sec.dat).
    """
    FEATURE_DIR         = 'fuseblower_output'
    TOOL_NAME           = 'fbc'
    SECDAT_SEGMENT_TYPE = SEG_TYPE_EFUSE
    CONFIG_MODULE       = fb_config

    def set_config_paths(self, oem_config_path=None, qti_config_path=None,
                         ui_config_path=None, user_config_path=None, secimage_config_path=None, dummy=None):

        FuseCommonCore.set_config_paths(self, oem_config_path, qti_config_path, ui_config_path, user_config_path, secimage_config_path, dummy)
        # Log the path update
        log_messages = ['Config paths set to: ',
                        '    ' + 'OEM: ' + str(oem_config_path),
                        '    ' + 'QTI: ' + str(qti_config_path),
                        '    ' + 'UI: ' + str(ui_config_path),
                        '    ' + 'USER: ' + str(user_config_path)]
        if secimage_config_path:
            log_messages.append('    ' + 'SECIMAGE: ' + str(secimage_config_path))
        log_string = '\n'.join(str(s) for s in log_messages)
        logger.info(log_string)

    @classmethod
    def _feature_id(cls):
        return FUSEBLOWER_ID

    @classmethod
    def is_plugin(cls):
        return True

    @classmethod
    def _ui_interface(cls):
        from sectools.features.fbc.ui.intf import fuse_blower_ui_intf
        return fuse_blower_ui_intf


class RootOfTrustCore(FuseCommonCore):
    """Provides interface for user to set up parameters (including input files,
    output directory, etc) and perform fuse blowing related operations
    (generation, validation of sec.dat).
    """
    FEATURE_DIR         = 'rootoftrust_output'
    TOOL_NAME           = 'rot'
    SECDAT_SEGMENT_TYPE = SEG_TYPE_ROT
    CONFIG_MODULE       = fb_config

    def set_config_paths(self, oem_config_path=None, qti_config_path=None,
                         ui_config_path=None, user_config_path=None, secimage_config_path=None):

        FuseCommonCore.set_config_paths(self, oem_config_path, qti_config_path, ui_config_path, user_config_path, secimage_config_path)
        # Log the path update
        log_messages = ['Config paths set to: ',
                        '    ' + 'ROT_OEM: ' + str(oem_config_path),
                        '    ' + 'UI: ' + str(ui_config_path),
                        '    ' + 'ROT_USER: ' + str(user_config_path)]
        if secimage_config_path:
            log_messages.append('    ' + 'SECIMAGE: ' + str(secimage_config_path))
        log_string = '\n'.join(str(s) for s in log_messages)
        logger.info(log_string)
