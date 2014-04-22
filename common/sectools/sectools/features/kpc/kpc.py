# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
"""
:Authors: sectools

Main interface to the keyprovision functionalities. Supports APIs for
command line interface, UI interface and test interface.
"""

from sectools.common.parsegen.secdat.segment.defines import SEG_TYPE_KEYPROV
from sectools.common.utils.c_logging import logger
from sectools.features.fuse_common.fcc import FuseCommonCore
from sectools.features.kpc.parsegen import fb_config


class KeyProvisionCore(FuseCommonCore):
    """Provides interface for user to set up parameters (including input files,
    output directory, etc) and perform fuse blowing related operations
    (generation, validation of sec.dat).
    """
    FEATURE_DIR         = 'keyprovision_output'
    TOOL_NAME           = 'kpc'
    SECDAT_SEGMENT_TYPE = SEG_TYPE_KEYPROV
    CONFIG_MODULE       = fb_config

    def set_config_paths(self, oem_config_path=None, qti_config_path=None,
                         ui_config_path=None, user_config_path=None, secimage_config_path=None):

        FuseCommonCore.set_config_paths(self, oem_config_path, qti_config_path, ui_config_path, user_config_path, secimage_config_path)
        # Log the path update
        log_messages = ['Config paths set to: ',
                        '    ' + 'KEYMAP: ' + str(oem_config_path),
                        '    ' + 'UI: ' + str(ui_config_path),
                        '    ' + 'USER: ' + str(user_config_path)]
        if secimage_config_path:
            log_messages.append('    ' + 'SECIMAGE: ' + str(secimage_config_path))
        log_string = '\n'.join(str(s) for s in log_messages)
        logger.info(log_string)