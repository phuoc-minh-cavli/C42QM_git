# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from . import defines as df
from sectools.common.parsegen.dbgp.header.v3_header import DbgpHdrv3, ROOTCERTHASH_MAX_COUNT

ROOTCERTHASH_MAX_CHAR_COUNT = 96


class DbgpHdrv4(DbgpHdrv3):

    @classmethod
    def get_format_rootcerthash_array(cls):
        return '<' + ('48s' * ROOTCERTHASH_MAX_COUNT)

    @classmethod
    def get_format_rootcerthash_qti_array(cls):
        return '<' + ('48s' * ROOTCERTHASH_MAX_COUNT)

    @property
    def rootcerthash_array(self):
        return self._rootcerthash_array

    @rootcerthash_array.setter
    def rootcerthash_array(self, value):
        for i in range(len(value)):
            if len(value[i]) != ROOTCERTHASH_MAX_CHAR_COUNT:
                raise RuntimeError('rootcerthash is not equal to 384 bits, rxd input rootcerthash bit length is '+str(len(value[i])*4))

        self._rootcerthash_count = len(value)
        self._rootcerthash_array = value

    @property
    def rootcerthash_qti_array(self):
        return self._rootcerthash_qti_array

    @rootcerthash_qti_array.setter
    def rootcerthash_qti_array(self, value):
        for i in range(len(value)):
            if len(value[i]) != ROOTCERTHASH_MAX_CHAR_COUNT:
                raise RuntimeError('rootcerthash_qti is not equal to 384 bits, rxd input rootcerthash_qti bit length is '+str(len(value[i])*4))

        self._rootcerthash_qti_count = len(value)
        self._rootcerthash_qti_array = value

    def set_base_info(self):
        self.magic = df.DBGPMAG
        self.version = df.DBGPVERFOUR
