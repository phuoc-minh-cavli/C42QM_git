# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
from . import defines as df
from sectools.common.parsegen.elf.format import ParseGenElf
from sectools.common.parsegen.elf.header.ident import ELFCLASS64
from sectools.common.parsegen.elf.segment.type import PT_LOAD
from sectools.common.parsegen.secdat import ParseGenSecdat
from sectools.common.parsegen.secdat.secdatelf.defines import SECDAT_SEGMENT_INDEX


class ParseGenSecdatElf(object):

    def __init__(self, data=None,
                 segment_data=None,
                 debug_dir=None,
                 debug_prefix=None,
                 debug_suffix=None,
                 elf_class=ELFCLASS64,
                 dummy=False):

        self.elfparser = ParseGenElf(data, _class=elf_class)
        self.segment_data = segment_data

        if data is None:
            phdr = self.elfparser.get_new_phdr_entry()()
            self.secdatparser = ParseGenSecdat(segment_data, debug_dir, debug_prefix, debug_suffix, dummy)
            self.elfparser.add_segment(phdr, segment_data)
        else:
            phdr = self.elfparser.phdrs[SECDAT_SEGMENT_INDEX]
            data = self.elfparser.get_segment_data(phdr)
            self.secdatparser = ParseGenSecdat(data, debug_dir, debug_prefix, debug_suffix)

    def get_data(self):
        self.elfparser.phdrs[SECDAT_SEGMENT_INDEX].p_offset = df.OFFSET
        self.elfparser.phdrs[SECDAT_SEGMENT_INDEX].p_type = PT_LOAD
        self.elfparser.update_segment(self.elfparser.phdrs[SECDAT_SEGMENT_INDEX], self.segment_data)
        return self.elfparser.get_data()

    @property
    def phys_addr(self):
        return self.elfparser.phdrs[SECDAT_SEGMENT_INDEX].p_paddr

    @phys_addr.setter
    def phys_addr(self, address):
        self.elfparser.phdrs[SECDAT_SEGMENT_INDEX].p_paddr = address
        self.elfparser.phdrs[SECDAT_SEGMENT_INDEX].p_vaddr = address
        self.elfparser.ehdr.e_entry = address

    def __repr__(self):
        return repr(self.elfparser) + '\n' + repr(self.secdatparser)