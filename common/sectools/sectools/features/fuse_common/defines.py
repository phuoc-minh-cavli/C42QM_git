# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

"""Contains constants related to the fbc & kpc package."""

from sectools.common.parsegen.elf.header.ident import ELFCLASS32, ELFCLASS64
from sectools.common.utils import c_path
from . import DIR_PATH

#------------------------------------------------------------------------------
# Common constants - Path
#------------------------------------------------------------------------------
# Absolute path of the package
PACKAGE_PATH                        = DIR_PATH

# Default config directory (relative path)
DEFAULT_CONFIG_DIR_REL              = '../../..'

# Default config directory (absolute path)
DEFAULT_CONFIG_DIR                  = c_path.join(PACKAGE_PATH, DEFAULT_CONFIG_DIR_REL)

#------------------------------------------------------------------------------
# Common constants - Feature
#------------------------------------------------------------------------------
COMMON_DIR                          = 'common_output'
SEC_DAT                             = 'sec.dat'
SEC_ELF_UNSIGNED                    = 'sec_unsigned.elf'
SEC_ELF_SIGNED                      = 'sec.elf'
SECDAT_TYPE_BIN                     = 1
SECDAT_TYPE_ELF                     = 2
# Name of root node for secimage config related object
SECIMAGE_ROOTNODE_NAME = 'secimage'
# prefix for secimage config file
PREFIX_SECIMAGE = 'fb_kp'
XML_NAME_END_SECIMAGE = '_' + PREFIX_SECIMAGE + '_' + SECIMAGE_ROOTNODE_NAME + '.xml'
#------------------------------------------------------------------------------
# Debug log paths
#------------------------------------------------------------------------------
DEST_DEBUG_DIR                      = 'debug'

DEST_DEBUG_FILE_DATA_MODEL_REPR     = 'data_model_repr_{0}.txt'
DEST_DEBUG_FILE_SECDAT_REPR         = 'secdat_repr.txt'


ELF_CLASS_MAP = {32: ELFCLASS32, 64: ELFCLASS64}

