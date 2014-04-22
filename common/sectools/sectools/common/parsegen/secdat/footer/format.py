# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import hashlib

from sectools.common.utils.compat import ensure_int
from sectools.common.utils.struct_base import StructBase
from six import ensure_binary


class SecdatFooter(StructBase):
    """Secdat Footer Class"""

    FOOTER_SIZE = 32

    def _unpack_data_list(self, unpacked):
        self.hash = b''
        for i in range(0, self.FOOTER_SIZE):
            self.hash += ensure_binary('{0:c}'.format(unpacked[i]), encoding='latin-1')

    def _pack_data_list(self):
        return[ensure_int(c) for c in self.hash]

    @classmethod
    def get_format(self):
        return ('B' * self.FOOTER_SIZE)


def extract_ftr(data):
    footer_data = data[len(data) - SecdatFooter.get_size():]
    footer = SecdatFooter(footer_data)
    data = data[:len(data) - footer.get_size()]
    return data, footer

def get_ftr_size(version):
    return SecdatFooter.get_size()

def create_footer(ftr, version, data):
    hasher = hashlib.sha256()
    hasher.update(data)
    footer = create_empty_footer()
    footer.hash = hasher.digest()
    return footer

def create_empty_footer():
    footer = SecdatFooter()
    return footer
