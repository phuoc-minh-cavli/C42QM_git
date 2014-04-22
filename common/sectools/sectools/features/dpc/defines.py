# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

"""Contains constants related to the dpc package."""


from sectools.common.utils import c_path
from . import DIR_PATH as PACKAGE_PATH


#------------------------------------------------------------------------------
# Common constants
#------------------------------------------------------------------------------

# Default config directory (relative path)
DEFAULT_CONFIG_DIR_REL = '../../..'

# Default config directory (absolute path)
DEFAULT_CONFIG_DIR = c_path.join(PACKAGE_PATH, DEFAULT_CONFIG_DIR_REL)


#------------------------------------------------------------------------------
# Debug log paths
#------------------------------------------------------------------------------
DEST_DEBUG_DIR = 'debug'
