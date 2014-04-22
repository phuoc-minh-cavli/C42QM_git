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

#pylint: disable-msg=R0902,R0903,R0904,R0913
# We disable the warning about a class having too many public methods
# because for this class we are sure it is a good thing. We also
# don't care about the classes that have too few methods. It would be
# nice to disable these per class.
# Also we don't really care about having too many instance attributes
"""
ELF parsing library.
"""
__revision__ = 1.0

from elf.ByteArray import ByteArray
from elf.constants import \
     PT_PHDR, PT_LOAD, PT_DYNAMIC, SHT_STRTAB, SHT_NOBITS, \
     SHT_NULL, SHT_DYNSYM, SHT_REL, SHT_RELA, STT_OBJECT, STT_FUNC, \
     ELFOSABI_NONE, EM_NONE, EM_ARM, EM_MIPS, ET_NONE, ElfFormatError, \
     EF_MIPS_ABI_O64, EF_ARM_EABIMASK, EF_ARM_EABI_VER2, \
     EF_ARM_DYNSYMSUSESEGIDX, SHT_PROGBITS, SHF_WRITE, SHF_ALLOC, \
     ET_EXEC, ET_DYN, SHT_REL, SHT_RELA, SHF_OKL4_SEGMENT_START, \
     SHF_OKL4_SEGMENT_END, DT_REL, DT_RELSZ, DT_RELENT, DT_NULL
from elf.structures import InvalidArgument, ElfIdentification, \
     ELF_PH_CLASSES, ELF_SH_CLASSES, ELF_HEADER_CLASSES, \
     ELF_DYNAMIC_STRUCT_CLASSES
from elf.File import File
from elf.util import align_up, align_down
from elf.section import ELF_SECTION_TYPES, UnpreparedElfSection, \
     UnpreparedElfStringTable, UnpreparedElfSymbolTable, PreparedElfSection, \
     UnpreparedElfReloc, UnpreparedElfReloca
from elf.segment import DataElfSegment, HeaderElfSegment, SectionedElfSegment
from elf.abi import ElfAbi, ABI_REGISTRATION

class BaseElfFile(object):
    """
    Class for creating and manipulating ELF files.

    Elf file classes can exist in two distinct states, prepared and unprepared.
    A prepared file has all its file offsets, name offsets and such setup and
    correct. In this state it is only possible to perform changes that won't
    affect the layout of the final ELF file.

    For example, it would be possible to change the virtual address of a
    segment, but not add new data to a segment, or add new segments.

    A file in the prepared state can be converted to raw data, and/or written
    out to a file.

    When an ELF file is created using init_and_prepare_from_filename, it is set
    in the prepared state. A new elf file created with the constructor is in
    the unprepared state.

    An unprepared ELF file can be converted into a prepared ELF file by calling
    the prepare() method.
    """

    def __init__(self):
        self._ph_offset = None
        self._ph_fixed = False

        self.machine = EM_NONE
        self.elf_type = ET_NONE
        self.entry_point = 0
        self.osabi = ELFOSABI_NONE
        self.abiversion = 0
        self.flags = 0

        self.has_section_headers = True

        # Pointer to the commonly used symbol table section.
        self._symbol_table = None

        # A file doesn't necessarily have these defined at all times!
        # Be careful when using unprepared elf files in particular
        self.wordsize = NotImplemented
        self.endianess = NotImplemented

    def _get_machine(self):
        return self._machine

    def _set_machine(self, machine):
        self._machine = machine
        try:
            self._abi = ABI_REGISTRATION[machine][0](self)
        except:
            self._abi = ElfAbi(self)

    machine = property(_get_machine, _set_machine)

    def _get_abi(self):
        return self._abi
    abi = property(_get_abi)

    def _initialise_header_information(self, hdr):
        """Initialise information from the ELF header"""
        self.machine = hdr.e_machine
        self.elf_type = hdr.e_type
        self.entry_point = hdr.e_entry
        self.osabi = hdr.ident.ei_osabi
        self.abiversion = hdr.ident.ei_abiversion
        self.flags = hdr.e_flags

    def _get_headers(self, hdr, file_, classes, off, entsize, num):
        """Create a list of section or program headers."""
        # Create all the objects...
        h_class = classes[self.wordsize]
        headers = [h_class(self.endianess) for _ in range(num)]
        # Then populate them...
        data = file_.get_data(off, num*entsize)
        for idx, header in enumerate(headers):
            header.fromdata(data[entsize*idx:entsize*(idx+1)], hdr)
        return headers

    def remove_section_headers(self):
        self.has_section_headers = False

    def _get_section_headers(self, hdr, file_):
        """Return a list of section headers, given the elf header and file."""
        return self._get_headers(hdr, file_, ELF_SH_CLASSES, hdr.e_shoff, \
                                 hdr.e_shentsize, hdr.e_shnum)

    def _initialise_sections(self, hdr, file_, prepare):
        """Initialise information from section headers."""
        self._sh_offset = hdr.e_shoff
        self._sh_strndx = hdr.e_shstrndx
        self._sheaders = self._get_section_headers(hdr, file_)
        self.sections = []

        if self._sh_offset == 0:
            return

        # Now create section objects for all of them
        for i, header in enumerate(self._sheaders):
            sect_cls = ELF_SECTION_TYPES.get(header.sh_type,
                                             UnpreparedElfSection)
            if sect_cls in (UnpreparedElfSymbolTable, UnpreparedElfReloc,
                            UnpreparedElfReloca):
                section = None # We handle symbol tables and relocs later
            else:
                if header.sh_type != SHT_NOBITS:
                    data = file_.get_data(header.sh_offset, header.sh_size)
                else:
                    data = header.sh_size

                section = sect_cls(self, None, header.sh_type, header.sh_addr,
                                   data, header.sh_flags, header.sh_addralign,
                                   header.sh_info)
                if prepare:
                    section = section.prepare(header.sh_offset, i,
                                              header.sh_name)
            self.sections.append(section)

        # Now set up any links between sections and convert the info
        # field to the appropriate type.
        for section, header in zip(self.sections, self._sheaders):
            if section:
                if header.sh_link:
                    section.link = self.sections[header.sh_link]
                # Cases where info is a section (symbols, relocs) are
                # handled above.
                assert not section.info_is_section()
                section.info = header.sh_info


    def _get_strtable(self):
        """
        Return the string table for this file. Returns None if the table does
        not exist.
        """
        strtable = None

        if self._sh_strndx:
            strtable = self.sections[self._sh_strndx]

            if strtable.type != SHT_STRTAB:
                raise ElfFormatError, \
                      "The section string table is malformed. %s %s" \
                      % (strtable, self.sections)
        return strtable

    def _initialise_section_names(self):
        """
        Use this file's string table to assign to assign names to all of its
        sections.
        """
        strtable = self._get_strtable()

        if strtable:
            for section, header in zip(self.sections, self._sheaders):
                section._name = strtable.get_string_ofs(header.sh_name)

    def _init_cls_sections(self, file_, prepare, classes):
        """
        Initialise the sections which are of a given list of classes
        """


        for i, header in enumerate(self._sheaders):
            sect_cls = ELF_SECTION_TYPES.get(header.sh_type,
                                             UnpreparedElfSection)

            if sect_cls in classes:
                assert self.sections[i] is None

                data = file_.get_data(header.sh_offset, header.sh_size)
                strtable = self._get_strtable()
                name = strtable.get_string_ofs(header.sh_name)
                sect = sect_cls(self, name, header.sh_type, header.sh_addr,
                                data, header.sh_flags, header.sh_addralign,
                                header.sh_info, self.sections[header.sh_link])
                if prepare:
                    sect = sect.prepare(header.sh_offset, i, header.sh_name)
                self.sections[i] = sect

        # Now set up any links between sections and convert the info
        # field to the appropriate type.
        for section, header in zip(self.sections, self._sheaders):
            if section:
                if header.sh_link:
                    section.link = self.sections[header.sh_link]
                if section.info_is_section():
                    section.info = self.sections[header.sh_info]
                else:
                    section.info = header.sh_info

    def _initialise_symbol_tables(self, file_, prepare):
        """Initialise symbol information."""
        self._init_cls_sections(file_, prepare, [UnpreparedElfSymbolTable])

    def _initialise_relocs(self, file_, prepare):
        """Initialise reloc information."""
        self._init_cls_sections(file_, prepare, [UnpreparedElfReloc,
                                                 UnpreparedElfReloca])

    def _initialise_segments(self, hdr, file_, prepare):
        """Initialise instance from program headers."""
        # Now load all the program headers
        #
        # "The time has come," the developer said, "to talk of many
        # things: Of section to segment mapping and scatter load, of
        # cabbages and kings, and why the RVCT linker is on drugs and
        # whether pigs have wings."
        #
        # There are two ways of determining which sections belong in a
        # segment:
        #
        # 1) In most cases a section is in a segment if its virtual
        #    address range falls in the virtual address range of the
        #    segment.  These segments tend to contain multiple
        #    PROGBITS sections and may end with a NOBITS section.
        #
        # 2) In the case of segments build with RVCT's scatter load
        #    support the virtual address of the included sections can
        #    be anywhere in memory.  In this case a section is in the
        #    segment if its data file offset falls in the offset range
        #    of the segment.  These segments can contain multiple
        #    PROGBITS and NOBITS sections in any order.  The file size
        #    of the segment is the filesize of all of the PROGBITS
        #    sections.  The memory size of the segment is the sum of
        #    the size of all of the sections, not the difference
        #    between the lowest and highest virtual addresses.
        #
        # Some other things about ELF files that you need to know:
        #
        # a) Segments can overlap, and sections can appear in multiple
        #    segments.
        #
        # b) Some sections do not appear in any segments.  These tend
        #    to be the symbol table and other debugging data.
        #
        # c) The offsets of NOBITS sections tend to be the same as the
        #    offset for the next PROGBITS section, even if it is in
        #    the next segment.  However, sometimes NOBITS sections
        #    appear at the beginning of a segment.
        #
        # d) The address offset from the start of a segment and the
        #    file offset from the start of the segment are not
        #    necessarily the same.
        #
        # Therefore, to map sections to segments involves two passes:
        #
        # 1) Find all of the sections that fall in the segment virtual
        #    address range.
        #
        # 2) Find all of the remaining sections that fall in the file
        #    offset range.  If any are found then mark the segment as
        #    a scatter-load segment.
        #
        # The file size and memory size of a segment depends on
        # whether or not it is a scatter load segment.  For scatter
        # load files, the section that has the latest offset from the
        # beginning of the section is used, while for non-scatter-load
        # segments, the section with the largest virtual address is
        # used.
        #
        # File offsets cannot be used for both types of segments
        # because broken linkers can produce sections with
        # inconsistant virtual address to file offset mappings and
        # this will make elfweaver produce segments with incorrect
        # file sizes or memory sizes.
        #
        # "O Readers," said the developer, "You've had a pleasant run!
        # Shall we start the code again?" But answer came there none--
        # And this was scarcely odd, because no-one would read this far
        # for fun.

        self._ph_offset = hdr.e_phoff
        self.segments = []

        # Short circuit the return.
        if hdr.e_phoff == 0:
            return

        ph_list = self._get_headers(hdr, file_, ELF_PH_CLASSES, hdr.e_phoff,
                                    hdr.e_phentsize, hdr.e_phnum)

        # Map segments to sections, pass 1.
        sheaders = self._get_section_headers(hdr, file_)
        sect_offsets = [header.sh_offset for header in sheaders]

        def offset_overlap(section, offset, ph, last_start_section):
            """
            Return whether or not a section's offset overlaps with a program
            header's offset range.
            """
            low = ph.p_offset
            high = low + ph.p_filesz

            def bss_overlap():
                """Detect of the bss section is part of this segment."""
                # Calculating which segment a scatter load BSS section falls
                # in is very tricky.  Consider the following elf file, based
                # on a customer file:
                #
                # Section Headers:
                # [Nr] Name      Type            Addr     Off
                # [ 1] ER_ROOT   PROGBITS        90100000 000034
                # ...
                # [ 5] THREE     NOBITS          90300020 0001f8
                # [ 6] FOUR      PROGBITS        90400000 0001f8
                # ...
                # [10] TEN       PROGBITS        f0900000 0023c8
                # [11] REMAINS   NOBITS          f0900004 0023cc
                # [12] BSS1      NOBITS          a0700000 0023cc
                # [13] BSS2      NOBITS          a0800000 0023cc
                # [14] BSS3      NOBITS          a0900000 0023cc
                # ...
                # Program Headers:
                # Type  Offset   VirtAddr   PhysAddr   FileSiz MemSiz
                # LOAD  0x000034 0x90100000 0x90100000 0x02398 0x034c4
                # LOAD  0x0023cc 0xa0700000 0xa0700000 0x00000 0x03000
                #
                # Here we have five BSS sections in two segments.  One is in
                # the middle of a segment (THREE) surrounded by PROGBITS
                # sections.  The REMAINS section is at the end the first
                # segment, BSS1 is at the start of the second segment, which
                # also contains BSS2 and BSS3.  These last four sections all
                # have the same file offset.
                #
                # Sections that start a segment are detected by the first pass
                # and are not considered here.  Therefore any BSS section here
                # that has the same offset at the start of the section (the
                # variable 'low' here) generally are the last section of the
                # previous segment.
                #
                # But if a scatter load section only contains BSS sections,
                # then the offset of the sections will be the same as the end
                # of the segment and it is impossible to tell which to segment
                # the non-starting BSS sections belong (in this case sections
                # BSS2 and BSS3).
                #
                # The solution is to rely on the order of sections in the
                # section list.  If the last seen section that started a
                # segment is the start of this segment, this the BSS sections
                # belong, otherwise they belong somewhere else.
                #
                # The section list is not required to put the sections in
                # segment order, but that seems to be the case in the real
                # world.
                return ((offset == high and
                         last_start_section.address == ph.p_vaddr) or
                        (offset != low))

            return (low <= offset <= high and
                    ((section.type == SHT_NOBITS and bss_overlap())  or
                     (section.type != SHT_NOBITS and section.get_size() > 0 and
                      offset + section.get_size() <= high)))

        # List of sections that are at the start of segments
        starting_sections = []

        # Map sections to segment pass 1: Find sections that fall in the
        # segment's virtual address range.
        for ph in ph_list:
            if ph.p_type != PT_PHDR:
                low = ph.p_vaddr
                high = low + ph.p_memsz
                sects = []
                in_okl4_segment = False
                in_correct_okl4_segment = False

                for (section, offset) in zip(self.sections, sect_offsets):
                    # If the section has our custom flag, then the segment
                    # containing it may overlap virtual addresses with another
                    # segment.  In this case only place sections in the
                    # segment if the addresses and the offsets overlap.
                    if (section.flags & SHF_OKL4_SEGMENT_START) != 0:
                        assert not in_okl4_segment
                        in_okl4_segment = True

                        if ph.p_offset == offset:
                            in_correct_okl4_segment = True
                            sects.append(section)

                            starting_sections.append(section)
                    else:
                        # See if the section's virtual address range falls within
                        # the virtual address range of the segment.
                        addr = section.address

                        if (low <= addr < high and addr + section.get_size() <= high and
                            section.flags & SHF_ALLOC and
                            (not in_okl4_segment or in_correct_okl4_segment)):
                            sects.append(section)

                            if low == addr:
                                starting_sections.append(section)

                    if (section.flags & SHF_OKL4_SEGMENT_END) != 0:
                        in_okl4_segment = False
                        in_correct_okl4_segment = False

                if sects:
                    seg = SectionedElfSegment(self, prog_header=ph,
                                              sections=sects)
                else:
                    data = file_.get_data(ph.p_offset, ph.p_filesz)
                    seg = DataElfSegment(self, prog_header=ph, data=data,
                                         memsz=ph.p_memsz)
            else:
                seg = HeaderElfSegment(self, prog_header=ph)
            self.segments.append(seg)

        # Map segments to sections, pass 2: FInd sections that fall in the
        # segment's offset range.
        for ph, seg in zip(ph_list, self.segments):
            if seg.has_sections():
                last_start_section = None
                this_range_of_sections = False

                for (section, offset) in zip(self.sections, sect_offsets):
                    # In order to correctly detect BSS sections in scatter
                    # load segments, we need to know which section is at the
                    # start of the program header.  Also, since sections that
                    # appear in the same segment appear in order in the
                    # sections list we only need to consider sections that are
                    # found between the section that starts this program
                    # header and the nest section that starts a section (or
                    # end of list).
                    if section in starting_sections:
                        if offset == ph.p_offset:
                            this_range_of_sections = True
                            last_start_section = section
                        else:
                            this_range_of_sections = False
                            last_start_section = None

                    # Scatter-load sections do not appear in multiple segments,
                    # so skip sections that are already in some segment or are
                    # not in this program header.
                    if section.in_segment or not this_range_of_sections:
                        continue
                    # See if the section's file offset range falls within the
                    # file offset range of the segment.  Remember that the
                    # trailing BSS section can have the offset of the next
                    # section
                    if offset_overlap(section, offset, ph, last_start_section):
                        seg.add_section(section)
                        seg.set_scatter_load()

                # If we're a scatter-load segment we need to work out in segment
                # offsets for our sections
                if seg.is_scatter_load():
                    for section in seg.sections:
                        idx = self.sections.index(section)
                        offset = sect_offsets[idx]
                        section.set_in_segment_offset(offset - ph.p_offset)

        # If need be, we now prepare the segments
        if prepare:
            for ph, seg in zip(ph_list, self.segments):
                if ph.p_type == PT_PHDR:
                    prog_header_size = hdr.e_phentsize * hdr.e_phnum
                else:
                    prog_header_size = None
                seg.prepare(ph.p_offset, prog_header_size)

    def init_and_prepare_from_file(self, file_):
        """This method initialise an ElfFile from a given File object."""
        ident = ElfIdentification()
        #ident.fromdata(file_.get_data(0, ident.get_size()))
        try:
            ident.fromdata(file_.get_data(0, ident.get_size()))
        except EOFError:
            raise ElfFormatError
        if file_.size() < ident.get_size():
            self.wordsize = 0
            self.sections = []
            self.segments = []
            return
        self.wordsize = ident.wordsize
        self.endianess = ident.endianess

        header_data = file_.get_data(0, ELF_HEADER_CLASSES[self.wordsize].size())
        hdr = ELF_HEADER_CLASSES[self.wordsize]()
        hdr.fromdata(header_data)

        self._initialise_header_information(hdr)
        self._initialise_sections(hdr, file_)
        self._initialise_symbol_tables(file_)
        self._initialise_relocs(file_)
        self._initialise_section_names()
        self._initialise_segments(hdr, file_)

    def init_and_prepare_from_filename(self, filename):
        """
        This method returns a new Elf file object initialised from a file.
        """
        return self.init_and_prepare_from_file(File(filename, "rb"))

    def get_ph_offset(self):
        """Get the program header offset."""
        return self._ph_offset

    def has_segments(self):
        """Return true if this ELF file has segments."""
        return bool(self.segments)

    def get_paddr(self):
        """Return the physical address of the first loadable segment"""
        for seg in self.segments:
            if seg.type == PT_LOAD:
                return seg.paddr
        raise InvalidArgument, "Elf file has no segments of type PT_LOAD?"

    def add_symbol(self, symbol):
        """
        Add a symbol to this files symbol table and return the index
        of the symbol in the symbol table.
        """
        symtab = self.get_symbol_table()

        return symtab.add_symbol(symbol)

    def get_symbol_table(self):
        """
        Return the main symbol table for this file, creating it if it doesn't
        exist yet.
        """
        if self._symbol_table is None:
            symtab = self.find_section_named(".symtab")

            if symtab == None:
                # we need to create a symbol table
                string_table = UnpreparedElfStringTable(self, ".strtab")
                symtab = UnpreparedElfSymbolTable(self, ".symtab",
                                                   link=string_table)
                self.sections.append(string_table)
                self.sections.append(symtab)

            self._symbol_table = symtab

        return self._symbol_table

    def get_symbols(self):
        """ Return a list of symbols (ElfSymbol) in this file. """
        return self.get_symbol_table().symbols

    def section_symbols(self, section):
        """Find the symbols and their indexes for the given section"""
        return ((idx, sym) for (idx, sym) in enumerate(self.get_symbols())
                if sym.section == section)

    def find_symbol(self, name):
        """
        Find the named symbol in the file. Returns None if it can't be found.
        Only returns the first one found.
        """
        symbol_table = self.get_symbol_table()
        return symbol_table.get_symbol(name)

    def get_symbol_value(self, symbol_name):
        """
        Return the contents of memory at the symbol address.
        This function will fail if the objetc is greater than 8 bytes
        in size.
        """
        sym = self.find_symbol(symbol_name)
        if sym is None:
            return None

        if sym.type not in (STT_OBJECT, STT_FUNC):
            return sym.value
        elif self.elf_type in [ET_EXEC, ET_DYN]:
            # sym.value is a virtual address
            return self.get_value(sym.value, sym.size, self.endianess)
        else:
            # sym.value is an offset into the section
            return sym.section.get_data().get_data(sym.value, sym.size, self.endianess)


    def get_value(self, address, size, endianess = None):
        """get a value from an elf file."""
        if self.machine == EM_MIPS:
            if self.flags & EF_MIPS_ABI_O64:
                if address & 0x80000000:
                    address |= 0xffffffff00000000L

        for segment in self.segments:
            if not segment.has_sections():
                continue
            for section in segment.get_sections():
                if address >= section.address and \
                       address < (section.address + section.get_size()):
                    offset = address - section.address
                    if endianess is None:
                        endianess = self.endianess
                    return section.get_data().get_data(offset, size, endianess)

        return None

    def find_section_named(self, name):
        """Find a section named `name'. Return None if no section is found."""
        return dict([(sect.name, sect) for sect in self.sections
                     if sect.name == name]).get(name)

    def strip(self):
        for s in self.sections:
            if s.name == '.strtab':
                self.sections.remove(s)
            if s.name == '.symtab':
                self.sections.remove(s)


    def remove_nobits(self):
        """
        Remove any NOBITS sections from the ELF file by converting them to
        PROGBITS sections. This will ensure that segments have memsz and filesz
        equal.
        """
        if self.has_segments():
            for segment in self.segments:
                segment.remove_nobits()
        else:
            for section in self.sections:
                section.remove_nobits()

    def _addr_type_is_virt(cls, addr_type):
        """
        Parse the address space type, returning true if virtual, false if
        physical. May raise InvalidArgument.
        """
        try:
            return {"virtual": True, "physical": False}[addr_type]
        except KeyError:
            raise InvalidArgument("addr_type must be 'virtual' or 'physical'")
    _addr_type_is_virt = classmethod(_addr_type_is_virt)

    def _get_ptload_segments(self):
        """
        Return a list of those segments in the file which are of type PT_LOAD.
        """
        return [seg for seg in self.segments if seg.type == PT_LOAD]

    def get_first_addr(self, addr_type = "virtual"):
        """
        Return the first memory address used in the file. By default it uses
        the virtual memory address. This can be changed to use the physical
        memory address if physical is set to True. Getting last physical can only
        be used if the file has segments.
        """
        virtual = self._addr_type_is_virt(addr_type)

        if self.has_segments():
            segments = self._get_ptload_segments()
            if virtual:
                addrs = [segment.vaddr for segment in segments]
            else:
                addrs = [segment.paddr for segment in segments]
        else:
            # Base it on sections instead
            if not virtual:
                raise InvalidArgument, "Can't find first physical \
                address in an ElfFile without segments."
            addrs = [section.address for section in self.sections]
        if addrs:
            return min(addrs)
        else:
            return 0

    def get_last_addr(self, addr_type = "virtual"):
        """
        Return the last memory address used in the file. By default it uses the
        virtual memory address. This can be changed to use the physical memory
        address if physical is set to True. Getting last physical can only be
        used if the file has segments.
        """
        virtual = self._addr_type_is_virt(addr_type)

        if self.has_segments():
            segments = self._get_ptload_segments()
            if virtual:
                addrs = [(segment.vaddr + segment.get_memsz(), segment.align)
                         for segment in segments]
            else:
                addrs = [(segment.paddr + segment.get_memsz(), segment.align)
                         for segment in segments]
        else:
            # Base it on sections instead
            if not virtual:
                raise InvalidArgument, "Can't find last physical \
                address in an ElfFile without segments."
            addrs = [(section.address + section.get_size(), section.addralign)
                     for section in self.sections]
        if addrs:
            last_addr, last_align = max(addrs)
        else:
            last_addr, last_align = 0, 0
        return align_up(last_addr, last_align)

    def get_address_span(self, addr_type = "virtual"):
        """Return the address span of a given file."""
        return self.get_last_addr(addr_type) - self.get_first_addr(addr_type)

    def find_segment_by_vaddr(self, vaddr):
        """Given a virtual address return the segment in which it is found."""
        for segment in self.segments:
            if vaddr in segment.get_span():
                return segment

    def find_section_by_vaddr(self, vaddr):
        """Given a virtual address return the section in which it is found."""
        for section in self.sections:
            if vaddr in section.get_span():
                return section

    def _reloc_sects(self):
        return [sec for sec in self.sections if sec.type in (SHT_REL, SHT_RELA)]
    reloc_sects = property(_reloc_sects)

class PreparedElfFile(BaseElfFile):
    """
    Prepared elf files have a set layout, and cannot be added to/removed from.
    They have all the details of their layout fixed and well defined, ready to
    be written to disk.
    """

    def __init__(self, wordsize=None, endianess=None, sections=None,
                 segments=None, filename=None):
        """Create a new ELF file."""
        BaseElfFile.__init__(self)

        if filename is not None:
            self.init_and_prepare_from_filename(filename)
        else:
            if sections is None:
                sections = [PreparedElfSection(self)]
            if segments is None:
                segments = []
            self.wordsize = wordsize
            self.endianess = endianess
            self.sections = sections
            self.segments = segments

    def get_elf_header(self):
        """Return the ELF header for this ElfFile."""

        elfheader = ELF_HEADER_CLASSES[self.wordsize](self.endianess)

        elfheader.e_machine = self.machine
        elfheader.e_type = self.elf_type
        elfheader.e_entry = self.entry_point
        elfheader.ident.ei_osabi = self.osabi
        elfheader.ident.ei_abiversion = self.abiversion
        elfheader.e_flags = self.flags

        # Create any section headers
        if self.sections and self.has_section_headers:
            elfheader.e_shentsize = ELF_SH_CLASSES[self.wordsize].size()
            elfheader.e_shoff = self._sh_offset
            elfheader.e_shnum = len(self.sections)
            elfheader.e_shstrndx = self._sh_strndx

        if self.segments:
            elfheader.e_phentsize = ELF_PH_CLASSES[self.wordsize].size()
            elfheader.e_phoff = self._ph_offset
            elfheader.e_phnum = len(self.segments)

        return elfheader

    def get_section_headers(self):
        """Return a list of section headers"""
        if self.has_section_headers:
            return [sect.get_section_header() for sect in self.sections]
        else:
            return []

    def todata(self):
        """Return a list of (array, offset) tuples"""
        data = [(0, self.get_elf_header().todata())]

        # Create any section headers
        if self.sections[1:]:
            sect_data = ByteArray()
            for header in self.get_section_headers():
                sect_data.extend(header.todata())
            data.append( (self._sh_offset, sect_data) )

        # Create any section headers
        if self.segments:
            ph_data = ByteArray()
            for segment in self.segments:
                ph_data.extend(
                    segment.get_program_header(self.endianess,
                                               self.wordsize).todata())
            data.append( (self._ph_offset, ph_data) )

        if self.sections[1:]:
            data += [(sec.offset, sec.get_file_data()) for sec in self.sections]
        elif self.segments:
            data += [(seg.offset, seg.get_file_data()) for seg in self.segments]

        return data

    def to_file(self, file_):
        """Write the ELF out to a real file. file_ must be a File() instance."""
        data = self.todata()
        for offset, _dat in data:
            file_.seek(offset)
            _dat.tofile(file_)
        file_.close()

    def to_filename(self, filename):
        """This method outputs the ELf file to a given filename."""
        self.to_file(File(filename, "wb"))

    # We override the methods below to call the base method, passing
    # in prepare=True, so indicate that we want to get prepared objects
    # out of them.

    def _initialise_sections(self, hdr, file_):
        BaseElfFile._initialise_sections(self, hdr, file_, True)

    def _initialise_segments(self, hdr, file_):
        BaseElfFile._initialise_segments(self, hdr, file_, True)

    def _initialise_symbol_tables(self, file_):
        BaseElfFile._initialise_symbol_tables(self, file_, True)

    def _initialise_relocs(self, file_):
        BaseElfFile._initialise_relocs(self, file_, True)


class UnpreparedElfFile(BaseElfFile):
    """
    Unprepared elf files can have their contents modified however properties
    such as section header offsets, etc are not necessarily defined.
    """

    def __init__(self, filename=None):
        BaseElfFile.__init__(self)
        if filename is not None:
            self.init_and_prepare_from_filename(filename)
        else:
            self.sections = [UnpreparedElfSection(self)]
            self.segments = []
            self._sh_offset = None
            self._sh_strndx = 0

        self.got = None

        # Relocation sections in this elf for help with handling merging of
        # files of with relocation sections
        self.reloc_sections = None
        self.reloc_merge_sections = []
        self.reloc_merge_sections_temp = []
        self.reloc_merge_symbols_temp = {}

    def set_ph_offset(self, offset, fixed=False):
        """Set the program header offset."""

        if offset is not None:
            min_offset = min([header_class.size() for header_class in \
                              ELF_HEADER_CLASSES.values()])
            if offset < min_offset:
                raise InvalidArgument, \
                      "Program header offset must be at least %d (%d)." % \
                      (min_offset, offset)

        if not self._ph_fixed:
            self._ph_offset = offset
            self._ph_fixed = fixed

    def add_segment(self, segment):
        """Add a new segment to the ELF file."""
        self.segments.append(segment)

    def add_section(self, section):
        """Adding a new section to the ELF file."""
        self.sections.append(section)
        section.elffile = self

    def clone_section(self, section, address):
        """
        Returns a new section based on the provided section.  This avoids a
        circular dependency in the elfweaver linker's linker script code.
        """
        return UnpreparedElfSection(self, section.name, section.type,
                                    address, None, section.flags,
                                    section.addralign, section.info,
                                    section.link, section.entsize)

    def allocate_section(self, sect_name):
        """
        Returns the section requested, creating a new one if required.
        """
        sect = self.find_section_named(sect_name)
        if sect is None:
            if "bss" in sect_name:
                sect = UnpreparedElfSection(self, sect_name, SHT_NOBITS, 0, 0,
                                            SHF_WRITE | SHF_ALLOC, 8)
            elif sect_name == '.got':
                sect = UnpreparedElfSection(self, sect_name, SHT_PROGBITS, 0,
                                            ByteArray(), SHF_WRITE | SHF_ALLOC,
                                            4, entsize=4)
                got_plt = UnpreparedElfSection(self, '.got.plt', SHT_PROGBITS,
                                               0, ByteArray('\0' * 4 * 3),
                                               SHF_WRITE | SHF_ALLOC, 4,
                                               entsize=4)
                self.add_section(got_plt)
            else:
                sect = UnpreparedElfSection(self, sect_name, SHT_PROGBITS, 0,
                                            ByteArray(), SHF_WRITE | SHF_ALLOC,
                                            4, entsize=4)

            self.add_section(sect)

        return sect

    def add_got_entry(self, sym):
        """Add sym to the got table."""
        if sym.got_offset == None:
            # Allocate this symbol at the end of the current .got
            got = self.get_got()
            offset = got.get_size()
            got.append_word(sym.value)

            # Now set the got value on the symbol so that it is able to update
            # the .got section when we know the actual address it refers to
            sym.got_offset = offset

    def get_got(self):
        """Return the .got creating it if needed."""
        if not self.got:
            self.got = self.allocate_section('.got')

        return self.got

    def _fixup_dyn_symbols(self, dyn_symtab):
        """Map symbols in the dynamic symbol table to the main symbol table."""
        assert dyn_symtab.type == SHT_DYNSYM

        symtab = self.get_symbol_table()
        sym_dict = {}
        fixups = {}

        symbol_key = lambda sym: (sym.value, sym.size, sym.type)

        for idx, sym in enumerate(symtab.symbols):
            sym_dict[symbol_key(sym)] = idx

        for idx, sym in enumerate(dyn_symtab.symbols):
            if symbol_key(sym) in sym_dict.keys():
                fixups[idx] = sym_dict[symbol_key(sym)]
            else:
                raise ElfFormatError, \
                    "%s (%s) is in .dynsym but not in .symtab" % \
                        (sym.name, symbol_key(sym))

        return fixups

    def _fixup_ads_dyn_segment(self, dyn_segment):
        """Generate sections for a broken ADS-generated dynamic segment."""

        assert self.reloc_sections is not None
        assert len(self.reloc_sections) == 0

        data = dyn_segment.get_file_data()

        # Create a temporary dynamic table that covers the entire segment.
        # The real table is shorter but we have no reliable way to know how
        # long it is; we just keep going until we see a DT_NULL.
        dyn_entry_class = ELF_DYNAMIC_STRUCT_CLASSES[self.wordsize]
        dyn_entry_size = ELF_DYNAMIC_STRUCT_CLASSES[self.wordsize].size()
        dyn_table = (dyn_entry_class(self.endianess).fromdata(data, i)
                     for i in xrange(0, len(data), dyn_entry_size))

        for dyn_entry in dyn_table:
            if dyn_entry.d_tag == DT_REL:
                rel_offset = dyn_entry.d_addr - dyn_segment.vaddr
            elif dyn_entry.d_tag == DT_RELSZ:
                rel_size = dyn_entry.d_val
            elif dyn_entry.d_tag == DT_RELENT:
                rel_entsize = dyn_entry.d_val
            elif dyn_entry.d_tag == DT_NULL:
                break

        if rel_offset is None:
            return

        if rel_size is None:
            raise ElfFormatError, \
                "Dynamic section contains DT_REL but no DT_RELSZ"
        if rel_entsize is None:
            raise ElfFormatError, \
                "Dynamic section contains DT_REL but no DT_RELENT"

        # Skip the first entry, which contains a guard reloc with r_offset = 0
        rel_data = ByteArray()
        rel_data.extend(data[rel_offset+rel_entsize:rel_offset+rel_size])
        rel_section = UnpreparedElfReloc(self, name = '.rel.ads',
                                         entsize = rel_entsize,
                                         info = self.sections[0],
                                         link = self.sections[0],
                                         data = rel_data)
        self.reloc_sections.append(rel_section)

    def _fixup_dyn_relocs(self):
        """Split a dynamic reloc section into static reloc sections."""
        assert self.reloc_sections is not None

        # If there are no relocations there's nothing to do.
        if not self.reloc_sections:
            return

        # The relocation tables haven't been merged if there's more than one.
        if len(self.reloc_sections) > 1:
            return

        dyn_relocs = self.reloc_sections[0]
        assert dyn_relocs.type in (SHT_REL, SHT_RELA)

        # If the info field points to a non-null section, this isn't a merged
        # relocation table.
        if dyn_relocs.info.type != SHT_NULL:
            return

        # If the link field points to a .dynsym section, we need to convert
        # the symbol indicies to use the .symtab section.
        if dyn_relocs.link.type == SHT_DYNSYM:
            symdx_fixups = self._fixup_dyn_symbols(dyn_relocs.link)
        else:
            symdx_fixups = None

        if dyn_relocs.type == SHT_RELA:
            name_prefix = ".rela"
        else:
            name_prefix = ".rel"

        split_relocs = {}

        for reloc in dyn_relocs.relocs:
            section = self.find_section_by_vaddr(reloc.offset)

            if section is None:
                raise ElfFormatError, \
                    "Could not find section for reloc with address %#8.8x" % \
                    reloc.offset

            name = section.name

            if section.name not in split_relocs:
                split_relocs[name] = \
                        dyn_relocs.__class__(self, name=name_prefix + name,
                                             link=self.get_symbol_table(),
                                             info=section,
                                             entsize=dyn_relocs.entsize)
                # Dynamic relocs don't need to be reverted.
                split_relocs[name].reverted = True

            if symdx_fixups is not None:
                reloc.symdx = symdx_fixups[reloc.symdx]

            split_relocs[name].add_reloc(reloc)

        self.reloc_sections = split_relocs.values()

    def get_reloc_sections(self):
        """Returns a list of any relocation sections in this ELF"""
        if self.reloc_sections is None:
            self.reloc_sections = [sect for sect in self.sections \
                                   if sect.type in (SHT_REL, SHT_RELA)]

            # In a binary linked with armlink -reloc, there is a broken
            # PT_DYNAMIC segment that does not contain any sections. If such a
            # segment exists, we must parse it to locate the dynamic rel table.
            if self.reloc_sections == []:
                for seg in self.segments:
                    if seg.type == PT_DYNAMIC and not seg.has_sections():
					    #Disabled to enable dynamic loading to enable dynamic loading on TN/APPs
                        #self._fixup_ads_dyn_segment(seg)
                        break

            # In an executable or dynamic shared object, the relocations may
            # have been merged into a single table. We split them up again,
            # so we can handle relocations for different sections separately.
            # Generally, ADS and RVCT need this, and GNU ld doesn't.
			#Disabled to enable dynamic loading on TN/APPs 
            #if self.elf_type in [ET_EXEC, ET_DYN]:
                #self._fixup_dyn_relocs()

        return self.reloc_sections

    def remove_section(self, section):
        """Remove section"""
        if section not in self.sections:
            # reloc sections can have extra segments that are not in the elf
            # file.  See get_reloc_sections().
            if self.reloc_sections is not None and section in self.reloc_sections:
                return
            else:
                raise InvalidArgument, "Can't remove a section that doesn't exist"

        for segment in self.segments:
            if segment.has_sections() and section in segment.sections:
                segment.remove_section(section)

        self.sections.remove(section)

    def replace_section(self, old_section, new_section):
        """Replace section"""
        if old_section not in self.sections:
            raise InvalidArgument, "Can't remove a section that doesn't exist"
        for segment in self.segments:
            if segment.has_sections() and old_section in segment.sections:
                segment.replace_section(old_section, new_section)
        self.sections[self.sections.index(old_section)] = new_section

    def create_and_replace(self, cls, name):
        """
        This method creates a new section of a given class with a given name.
        If another section of the same name exists, it is replaced by the new
        section, otherwise, the new section is appended to the file.
        """
        old_sect = self.find_section_named(name)
        new_sect = cls(self, name)
        if old_sect:
            self.replace_section(old_sect, new_sect)
        else:
            self.add_section(new_sect)
        return new_sect

    def resolve_section_overlaps(self):
        """
        This method checks the file's sections for overlaps, bumps section
        addresses upwards where necessary, and updates symbols that point to
        the moved sections.
        """
        moved_sections = {}

        # Iterate through pairs of consecutive loadable sections, sorted by
        # address
        loadable_sections = filter(lambda x: x.flags & SHF_ALLOC, self.sections)
        sections = sorted(loadable_sections, key=lambda x: x.address)
        for sect_a, sect_b in zip(sections, sections[1:]):
            sect_a_top = sect_a.address + sect_a.get_size()

            if sect_a_top > sect_b.address:
                new_address = align_up(sect_a_top, sect_b.addralign)
                offset = new_address - sect_b.address
                sect_b.address = new_address
                moved_sections[sect_b] = (sect_b, offset)

        # Note that we only update symbols here, and not relocations. That is
        # because the relocs should have offsets into their section at this
        # point, not absolute addresses.
        self.get_symbol_table().update_merged_sections(moved_sections)

    def prepare(self, wordsize, endianess):
        """Prepare file for writing"""
        self.wordsize = wordsize
        self.endianess = endianess
        # First we prepare the symbols, as this might in fact add
        # new sections.
        sh_string_table = None

        # Sort the sections so they appear something like you'd expect from
        # ld, but keep the loadable sections in the original order to handle
        # ELF files with overlapping virtual addresses.
        old_sections = self.sections[1:]
        if len(self.sections) > 1:
            self.sections = [self.sections[0]]
        else:
            self.sections = []
        for sect in old_sections:
            if sect.address > 0:
                self.sections.append(sect)

        old_sections = [sect for sect in old_sections if sect.address == 0]
        old_sections.sort(key=lambda x: x.name)
        self.sections.extend(old_sections)

        if len(self.sections) > 1:
            # Create section header string table
            sh_string_table = self.create_and_replace(UnpreparedElfStringTable,
                                                      ".shstrtab")

        # We need to make sure the symbol table is the last thing in the file,
        symtab = self.find_section_named(".symtab")
        if symtab:
            i = self.sections.index(symtab)
            # swap symtab with the last section
            self.sections[-1], self.sections[i] = symtab, self.sections[-1]

        # First find where our segments should go.
        if self._ph_offset is None or self._ph_offset == 0:
            self.set_ph_offset(ELF_HEADER_CLASSES[wordsize].size())

        ofs = self._ph_offset
        ###########
        sections = self.sections[:]
        if self.segments:
            ofs = self._prepare_segments(ofs, sh_string_table, sections)

        ofs, sections = self._prepare_sections(ofs, sh_string_table, sections)

        new_file = PreparedElfFile(wordsize, endianess, sections, self.segments)
        new_file._sh_offset = align_up(ofs, new_file.wordsize / 8)
        new_file._sh_strndx = self._sh_strndx
        new_file._ph_offset = self._ph_offset

        new_file.machine = self.machine
        new_file.elf_type = self.elf_type
        new_file.entry_point = self.entry_point
        new_file.osabi = self.osabi
        new_file.abiversion = self.abiversion
        new_file.flags = self.flags

        new_file.has_section_headers = self.has_section_headers

        for section in new_file.sections:
            section.elffile = new_file

        return new_file

    def _prepare_sections(self, ofs, sh_string_table, sections):
        """
        Prepare sections in the file. Passed the current ofs value, and returns
        any updated value.
        """
        # We now add all the section that are not part of the segment.
        if sh_string_table:
            self._sh_strndx = self.sections.index(sh_string_table)

            # we need to add all the section names to the sh_string_table before
            # we go prepring things, since once sh_string_table is prepared, we
            # can't add to it any more.
            for section in sections:
                if isinstance(section, UnpreparedElfSection):
                    sh_string_table.add_string(section.name)

        new_sections = []
        for idx, section in enumerate(sections):
            if isinstance(section, UnpreparedElfSection) and section.name:
                if sh_string_table:
                    name_offset = sh_string_table.offset_of(section.name)
                else:
                    name_offset = None
                offset = align_up(ofs, section.addralign)
                # Note: must prepare before getting size
                section = section.prepare(offset, idx, name_offset)
                ofs = offset + section.get_size()
            elif isinstance(section, UnpreparedElfSection):
                # NULL sections
                section = section.prepare(0, 0, 0)

            new_sections.append(section)
        # Check that all sections have a valid section index
        for section in new_sections[1:]:
            assert section.index < len(sections)

        return ofs, new_sections

    def _prepare_segments(self, ofs, sh_string_table, all_sections):
        """Prepare the segments in the file for writing."""

        ph_class = ELF_PH_CLASSES[self.wordsize]
        ph_header_size = ph_class.size() * len(self.segments)
        ofs += ph_header_size
        for segment in self.segments:
            #round down
            new_ofs = align_down(ofs, segment.align)
            if segment.align != 0:
                new_ofs += segment.vaddr % segment.align
            # add extra
            if new_ofs <= ofs:
                new_ofs += segment.align

            assert new_ofs >= ofs, "New offset must be greater " \
                   "than the old offset"
            # check + add pagesize
            ofs = new_ofs
            if segment.align != 0:
                assert ofs % segment.align == segment.vaddr % segment.align, \
                       "Must be congruent"

            if segment.type == PT_PHDR:
                segment.prepare(ofs, ph_header_size)
            else:
                segment.prepare(ofs)
                # Now can prepare the sections inside it if it has any
                if segment.has_sections():
                    segment.prepare_sections(all_sections, sh_string_table)
            ofs += segment.get_filesz()

        return ofs

    def _initialise_sections(self, hdr, file_):
        BaseElfFile._initialise_sections(self, hdr, file_, False)

    def _initialise_segments(self, hdr, file_):
        BaseElfFile._initialise_segments(self, hdr, file_, False)

    def _initialise_symbol_tables(self, file_):
        BaseElfFile._initialise_symbol_tables(self, file_, False)

    def _initialise_relocs(self, file_):
        BaseElfFile._initialise_relocs(self, file_, False)

    def init_and_prepare_from_file(self, file_):
        """This method initialise an ElfFile from a given File object."""
        BaseElfFile.init_and_prepare_from_file(self, file_)

    def init_and_prepare_from_filename(self, filename):
        """
        This method returns a new Elf file object initialised from a file.
        """
        return self.init_and_prepare_from_file(File(filename, "rb"))
