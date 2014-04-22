# ===============================================================================
#
#  Copyright (c) 2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from . import defines as df
from .v4_header import DbgpHdrv4


class DbgpHdrv5(DbgpHdrv4):

    INVALID_FLAG_BITS = 0xFFFF00FFFE00

    def set_base_info(self):
        self.magic = df.DBGPMAG
        self.version = df.DBGPVERFIVE

    def validate_ranges(self, serial_num_min_count=0):
        super(DbgpHdrv5, self).validate_ranges(serial_num_min_count=serial_num_min_count)
