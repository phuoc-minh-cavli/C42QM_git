##############################################################################
# Australian Public Licence B (OZPLB)
# 
# Version 1-0
# 
# Copyright (c) 2006-2010, Open Kernel Labs, Inc.
# 
# All rights reserved.
# 
# Developed by: Embedded, Real-time and Operating Systems Program (ERTOS)
#               National ICT Australia
#               http://www.ertos.nicta.com.au
# 
# Permission is granted by Open Kernel Labs, Inc., free of charge, to
# any person obtaining a copy of this software and any associated
# documentation files (the "Software") to deal with the Software without
# restriction, including (without limitation) the rights to use, copy,
# modify, adapt, merge, publish, distribute, communicate to the public,
# sublicense, and/or sell, lend or rent out copies of the Software, and
# to permit persons to whom the Software is furnished to do so, subject
# to the following conditions:
# 
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimers.
# 
#     * Redistributions in binary form must reproduce the above
#       copyright notice, this list of conditions and the following
#       disclaimers in the documentation and/or other materials provided
#       with the distribution.
# 
#     * Neither the name of Open Kernel Labs, Inc., nor the names of its
#       contributors, may be used to endorse or promote products derived
#       from this Software without specific prior written permission.
# 
# EXCEPT AS EXPRESSLY STATED IN THIS LICENCE AND TO THE FULL EXTENT
# PERMITTED BY APPLICABLE LAW, THE SOFTWARE IS PROVIDED "AS-IS", AND
# NATIONAL ICT AUSTRALIA AND ITS CONTRIBUTORS MAKE NO REPRESENTATIONS,
# WARRANTIES OR CONDITIONS OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
# BUT NOT LIMITED TO ANY REPRESENTATIONS, WARRANTIES OR CONDITIONS
# REGARDING THE CONTENTS OR ACCURACY OF THE SOFTWARE, OR OF TITLE,
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, NONINFRINGEMENT,
# THE ABSENCE OF LATENT OR OTHER DEFECTS, OR THE PRESENCE OR ABSENCE OF
# ERRORS, WHETHER OR NOT DISCOVERABLE.
# 
# TO THE FULL EXTENT PERMITTED BY APPLICABLE LAW, IN NO EVENT SHALL
# NATIONAL ICT AUSTRALIA OR ITS CONTRIBUTORS BE LIABLE ON ANY LEGAL
# THEORY (INCLUDING, WITHOUT LIMITATION, IN AN ACTION OF CONTRACT,
# NEGLIGENCE OR OTHERWISE) FOR ANY CLAIM, LOSS, DAMAGES OR OTHER
# LIABILITY, INCLUDING (WITHOUT LIMITATION) LOSS OF PRODUCTION OR
# OPERATION TIME, LOSS, DAMAGE OR CORRUPTION OF DATA OR RECORDS; OR LOSS
# OF ANTICIPATED SAVINGS, OPPORTUNITY, REVENUE, PROFIT OR GOODWILL, OR
# OTHER ECONOMIC LOSS; OR ANY SPECIAL, INCIDENTAL, INDIRECT,
# CONSEQUENTIAL, PUNITIVE OR EXEMPLARY DAMAGES, ARISING OUT OF OR IN
# CONNECTION WITH THIS LICENCE, THE SOFTWARE OR THE USE OF OR OTHER
# DEALINGS WITH THE SOFTWARE, EVEN IF NATIONAL ICT AUSTRALIA OR ITS
# CONTRIBUTORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH CLAIM, LOSS,
# DAMAGES OR OTHER LIABILITY.
# 
# If applicable legislation implies representations, warranties, or
# conditions, or imposes obligations or liability on Open Kernel Labs, Inc.
# or one of its contributors in respect of the Software that
# cannot be wholly or partly excluded, restricted or modified, the
# liability of Open Kernel Labs, Inc. or the contributor is limited, to
# the full extent permitted by the applicable legislation, at its
# option, to:
# a.  in the case of goods, any one or more of the following:
# i.  the replacement of the goods or the supply of equivalent goods;
# ii.  the repair of the goods;
# iii. the payment of the cost of replacing the goods or of acquiring
#  equivalent goods;
# iv.  the payment of the cost of having the goods repaired; or
# b.  in the case of services:
# i.  the supplying of the services again; or
# ii.  the payment of the cost of having the services supplied again.
# 
# The construction, validity and performance of this licence is governed
# by the laws in force in New South Wales, Australia.

#pylint: disable-msg=R0902,R0913,W0142
# We dont care about too many arguments, we need them all (0913)
# Also we don't really care about having too many instance attributes (0902)
# We also don't mind using **magic in the one place it's used. (0142)
"""
ELF parsing library.
"""
__revision__ = 1.0

from elf.ByteArray import ByteArray
from elf.constants import PT_NULL, PT_PHDR, SHT_NOBITS
from elf.structures import InvalidArgument, ELF_PH_CLASSES
from elf.util import Span, Unprepared, Prepared

class ElfSegment(object):
    """
    ElfSegment instances represent segments in an ELF file. This differs from
    ElfProgramHeader, which just describes the header and not the data itself.
    """

    def __init__(self, elffile, program_header_type=PT_NULL, vaddr=0, paddr=0,
                 flags=0, align=0, prog_header=None):
        """Create a new ElfSegment, with a given set of initial parameters."""
        self.elffile = elffile
        if prog_header:
            program_header_type = prog_header.p_type
            vaddr = prog_header.p_vaddr
            paddr = prog_header.p_paddr
            flags = prog_header.p_flags
            align = prog_header.p_align
        self.type = program_header_type
        self.vaddr = vaddr
        self.paddr = paddr
        self.flags = flags
        self.align = align

        if self.vaddr is None:
            self.vaddr = 0
        if self.paddr is None:
            self.paddr = 0

        self.prepared = False
        self._offset = None

        self._prog_header_size = None # This is only used by HeaderElfSements

    def remove_nobits(self):
        """Remove any NOBITS sections from the segment."""
        raise NotImplementedError

    def prepare(self, offset, prog_header_size = None):
        """Prepare this segment ready for writing."""
        if self.prepared:
            raise Prepared, "This segment is already prepared"
        if not prog_header_size and self.type == PT_PHDR:
            raise InvalidArgument, "Must set the program header size on PHDR "\
                  "segments when preparing"
        if prog_header_size and self.type != PT_PHDR:
            raise InvalidArgument, "Program header size should only " \
                  "be set on phdr sections."

        self._offset = offset
        self.prepared = True
        self._prog_header_size = prog_header_size

    def get_memsz(self):
        """Return the size this segment occupies in memory."""
        raise NotImplementedError

    def get_filesz(self):
        """Return the size this segment occupies in memory."""
        raise NotImplementedError

    def get_offset(self):
        """Return the offset of this segment is the physical file."""
        if self.prepared:
            return self._offset
        else:
            raise Unprepared, "Can only get the offset once the" \
                  " segment has been prepared."
    offset = property(get_offset)

    def has_sections(self):
        """Return true if the segment has internal sections."""
        raise NotImplementedError

    def copy_into(self, elffile):
        """Copy this segment into the given elf file."""
        raise NotImplementedError

    def _do_copy_into(self, elffile, **kwargs):
        """
        Implements the guts of the copy_into process, once the kwargs for the
        particular class have been collected.
        """
        new_segment = self.__class__(elffile, program_header_type=self.type,
                                     vaddr=self.vaddr,
                                     paddr=self.paddr,
                                     flags=self.flags,
                                     align=self.align, **kwargs)
        new_segment._prog_header_size = self._prog_header_size
        new_segment._offset = self._offset
        new_segment.prepared = self.prepared
        return new_segment

    def contains_vaddr(self, vaddr):
        """Return true if this segment contains the given virtual address."""
        return vaddr in self.get_span()

    def get_span(self):
        """Return the span of memory that this segment holds."""
        return Span(self.vaddr, self.vaddr + self.get_memsz())

    def vtop(self, vaddr):
        """Convert a virtual address to a physical address"""
        if vaddr not in self.get_span():
            raise InvalidArgument, "Vaddr must be in segment's range"
        return self.paddr + (vaddr - self.vaddr)

    def get_program_header(self, endianess, wordsize):
        """
        Return a program header for this segment with a given endianess and
        wordsize.
        """
        if not self.prepared:
            raise Unprepared, "get_program_header can't be called if the " \
                  " segment is unprepared."
        try:
            prog_header = ELF_PH_CLASSES[wordsize](endianess)
        except KeyError:
            raise InvalidArgument, "wordsize %s is not valid. " \
                  "Only %s are valid" %  \
                  (wordsize, ELF_PH_CLASSES.keys())

        prog_header.p_type = self.type
        prog_header.p_vaddr = self.vaddr
        prog_header.p_paddr = self.paddr
        prog_header.p_flags = self.flags
        prog_header.p_align = self.align
        prog_header.p_offset = self.offset
        prog_header.p_memsz = self.get_memsz()
        prog_header.p_filesz = self.get_filesz()

        return prog_header

    def __str__(self):
        return "<ElfSegment %s>" % self.type


class SectionedElfSegment(ElfSegment):
    """
    A SectionElfSegment is a segment which contains sections, but no explicit
    data.
    """

    def __init__(self, elffile, program_header_type=PT_NULL, vaddr=0, paddr=0,
                 flags=0, align=0, prog_header=None, sections=None):
        """
        Create a new SectionedElfSegment, with a given set of initial
        parameters.
        """
        ElfSegment.__init__(self, elffile, program_header_type, vaddr, paddr,
                            flags, align, prog_header)

        self.sections = []
        if sections:
            for section in sections:
                self.add_section(section)

        self._is_scatter_load = False

    def set_scatter_load(self):
        """Mark the segment as a scatter-load segment."""
        self._is_scatter_load = True

    def is_scatter_load(self):
        """Return whether or not the segment is a scatter-load one."""
        return self._is_scatter_load

    def copy_into(self, elffile):
        """Copy this segment into the given elf file."""

        sections = [sect.copy_into(elffile) for sect in self.sections if
                    sect.link is None]

        elffile.sections.extend(sections)

        new_segment = self._do_copy_into(elffile, sections=sections)
        new_segment._is_scatter_load = self._is_scatter_load
        return new_segment

    def has_sections(self):
        """Return true if the segment has internal sections."""
        return True

    def get_sections(self):
        """Return a list of sections associated with this segment."""
        return self.sections

    def remove_section(self, section):
        """
        Remove a section from the segment.

        Raises InvalidArgument if the section is not valid.
        """
        if section not in self.sections:
            raise InvalidArgument, "Section must be in segment to remove it."
        self.sections.remove(section)
        section.in_segment = False
        # FIXME: needs to undo what got done in add_section

    def replace_section(self, old_section, new_section):
        """
        Replace old_section with new_section in this segments list of sections.

        Raises InvalidArgument if the old_section is not valid.
        """
        if old_section not in self.sections:
            raise InvalidArgument, "Section must be in segment to replace it."
        self.remove_section(old_section)
        self.add_section(new_section)

    def add_section(self, section):
        """Add a new section to a segment."""
        # Mark the section as added to the segment.
        # HACK:  Should detect if adding a section causes a segment to
        # turn into a scatter load segment.
        section.in_segment = True
        self.sections.append(section)
        self.sections.sort(key=lambda x: x.address)

    def get_filesz(self):
        """Return the size this segment occupies in memory."""
        return self._calc_section_size()

    def _calc_section_size(self):
        """Calculate the size of the segment when it has sections."""
        # NOBITS sections are not included in the calculations.
        sects = [sect for sect in self.sections if sect.type != SHT_NOBITS]
        return self._calc_size(sects)

    def get_memsz(self):
        """Return the size this segment occupies in memory."""
        memsz = self._calc_size(self.sections)

        # Scatter load segments can have BSS sections in the middle of the
        # segment, which isn't accounted for in the _calc_size() call.
        # Calculate the extra memory size here.
        if self._is_scatter_load:
            extra = sum(sect.get_size() for sect in self.sections[:-1]
                        if sect.type == SHT_NOBITS)
            memsz += extra

        return memsz

    def _calc_size(self, sects):
        """
        Return the size of memory occupied by the given list of sections.
        """
        if not sects:
            return 0

        off, size = max((sect.calc_in_segment_offset(self), sect.get_size())
                         for sect in sects)
        return off + size

    def remove_nobits(self):
        """Remove any NOBITS sections from the segment."""
        # With scatter load segments, NOBITS sections cannot be replaced
        # because they may be followed by other PROGBITS sections, and the
        # offset from the start of the segment of these PROGBITS sections
        # must be unchanged.
        if self._is_scatter_load:
            return

        for section in self.sections:
            section.remove_nobits()

    def prepare_sections(self, all_sections, sh_string_table):
        """
        This prepares all sections within this segment, given the context of
        all_sections, which are all the sections in the elf file and
        sh_string_table, which is the string table containing the section names.
        """
        for i, section in enumerate(self.sections):
            if section not in all_sections:
                section = section.prepared_to
            else:
                name_offset = sh_string_table.add_string(section.name)
                sh_index = all_sections.index(section)
                offset = self.offset + section.calc_in_segment_offset(self)
                section = section.prepare(offset, sh_index, name_offset)
                all_sections[sh_index] = section

            self.sections[i] = section


class DataElfSegment(ElfSegment):
    """
    A DataElfSegment is a segment which contains data, but no sections.
    """

    def __init__(self, elffile, data, program_header_type=PT_NULL, vaddr=0,
                 paddr=0, flags=0, align=0, prog_header=None, memsz=None):
        """
        Create a new DataElfSegment, with a given set of initial parameters.
        """
        ElfSegment.__init__(self, elffile, program_header_type, vaddr, paddr,
                            flags, align, prog_header)
        self._data = data
        if memsz is not None:
            self._memsz = memsz
        else:
            self._memsz = len(data)

    def copy_into(self, elffile):
        """Copy this segment into the given elf file."""
        # Make a copy of the data
        data = self._data.copy()
        return self._do_copy_into(elffile, data=data, memsz=self._memsz)

    def has_sections(self):
        """Return true if the segment has internal sections."""
        return False

    def get_file_data(self):
        """Return the data that will go into the file."""
        return self._data

    def get_memsz(self):
        """Return the size this segment occupies in memory."""
        return self._memsz

    def get_filesz(self):
        """Return the size this segment occupies in memory."""
        return len(self._data)

    def remove_nobits(self):
        """Remove any NOBITS sections from the segment."""
        zeros = ByteArray('\0' * (self.get_memsz() - self.get_filesz()))
        self._data.extend(zeros)


class HeaderElfSegment(ElfSegment):
    """
    This segment represents the program header itself.
    """

    def __init__(self, elffile, vaddr=0, paddr=0, flags=0, align=0,
                 prog_header=None, program_header_type=PT_PHDR):
        """
        Create a new HeaderElfSegment, with a given set of initial parameters.
        """
        assert program_header_type == PT_PHDR
        ElfSegment.__init__(self, elffile, PT_PHDR, vaddr, paddr, flags,
                            align, prog_header)

    def copy_into(self, elffile):
        """Copy this segment into the given elf file."""
        return self._do_copy_into(elffile)

    def get_memsz(self):
        """Return the size this segment occupies in memory."""
        if not self.prepared:
            raise Unprepared, "Phdr segments must be prepared before " \
                  "it is possible to get their size."
        return self._prog_header_size

    def get_filesz(self):
        """Return the size this segment occupies in memory."""
        if not self.prepared:
            raise Unprepared, "Phdr segments must be prepared before " \
                  "it is possible to get their size."
        return self._prog_header_size

    def has_sections(self):
        """Return true if the segment has internal sections."""
        return False

    def remove_nobits(self):
        """Remove any NOBITS sections from the segment."""
        pass
