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
"""
This module contains classes which can be used an manipulated by the user.
These are analogues of the various underlying classes which represent the
structures in their raw file format.
"""

import sys

from elf.constants import STT_NOTYPE, STT_OBJECT, STT_FUNC, STT_SECTION, \
     ElfSymbolVisibility, SHN_UNDEF, SHN_COMMON, SHN_LOPROC, SHN_HIPROC
from elf.util import align_up
from elf.abi import ABI_REGISTRATION

class ElfSymbol(object):
    """Represents an ELF symbol"""

    # There can be an enormous number of these objects in an ELF file, so use
    # slots to save space.
    __slots__ = (
        "name",
        "section",
        "value",
        "size",
        "type",
        "bind",
        "other",
        "shndx",
        "got_offset",
        )

    type_strings = {STT_NOTYPE : "Unspecified",
                    STT_OBJECT : "Data",
                    STT_FUNC : "Code",
                    STT_SECTION : "Section",
                    }

    def __init__(self, name, section, sym_struct=None, old_sym=None):
        """
        Create a new symbol with a given name associated with the given
        section. Either sym_struct or old_sym can be passed in to initialise
        the symbol values, otherwise they are all set to zero.
        """
        self.name = name
        # The section this symbol points to (as opposed to the symbol table,
        # which it lives in)
        self.section = section

        if sym_struct:
            self.value = sym_struct.st_value
            self.size = sym_struct.st_size
            self.type = sym_struct.st_type
            self.bind = sym_struct.st_bind
            self.other = sym_struct.st_other
            self.shndx = sym_struct.st_shndx
            self.got_offset = None
        elif old_sym:
            self.value = old_sym.value
            self.size = old_sym.size
            self.type = old_sym.type
            self.bind = old_sym.bind
            self.other = old_sym.other
            self.shndx = old_sym.shndx
            self.got_offset = old_sym.got_offset
        else:
            self.value = 0
            self.size = 0
            self.type = 0
            self.bind = 0
            self.other = 0
            self.shndx = 0
            self.got_offset = None

    def copy_into(self, section):
        """
        Copy this symbol in a new section.

        Returns a new ElfSymbol which belongs to the given section.
        """
        return ElfSymbol(self.name, section, old_sym=self)

    def get_type_string(self):
        """
        Return a string representing the symbol's type. Returns
        unknown if we don't know what the type value means.
        """
        return ElfSymbol.type_strings.get(self.type, "Unknown")

    def get_bind(self):
        """
        Return the binding of the symbol.
        """
        return self.bind

    def get_type(self):
        """
        Return the type field of the symbol.
        """
        return self.type

    def get_visibility(self):
        """
        Return visibility field of the symbol.
        """
        return ElfSymbolVisibility(self.other % 0x3)

    def __repr__(self):
        """Return a representation of the ElfSymbol instance."""
        rep =  "<ElfSymbol (%s) %s @ 0x%x Size: %d " % \
              (self.get_type_string(), str(self.name), self.value, self.size)
        if self.section and self.section.name:
            rep += self.section.name
        return rep + ">"

    def output(self, idx, file_=sys.stdout, wide=False):
        """
        Pretty print the symbol to a given file, which defaults to
        stdout.
        """
        print >> file_, "%6d:" % idx,
        print >> file_, "%8.8x" % self.value,
        print >> file_, "%5d" % self.size,
        print >> file_, "%-7s" % self.type,
        print >> file_, "%-6s" % self.bind,
        print >> file_, "%3s" % ElfSymbolVisibility(self.other % 0x3),
        print >> file_, "%3.3s" % self.shndx,
        print >> file_, {True: "%s", False: "%-.25s"}[wide] % self.name

    def _allocate_sym_from_section(self, elf, sect_name):
        """
        Allocate space in the requested section for this symbol.
        """
        sect = elf.allocate_section(sect_name)

        # We must byte align to the symbols value field
        sect_end_addr = sect.address + sect.get_size()
        sect_aligned_addr = align_up(sect_end_addr, self.value)
        padding = sect_aligned_addr - sect_end_addr

        sect.increment_size(padding + self.size)

        # Now update the symbol's info to point to the section
        self.section = sect
        self.value = sect_aligned_addr - sect.address
        self.shndx = None # Clear index to avoid double counting.

    def _set_sym_from_section(self, sect):
        """
        Point the symbol at the given section.
        """
        self.section = sect
        self.value = 0
        self.shndx = None # Clear index to avoid double counting.

    def allocate(self, elf):
        """
        Allocate any data that this symbol needs e.g. space in the .bss section.
        """
        if self.type == STT_OBJECT and self.shndx == SHN_COMMON:
            # SHN_COMMON symbols need to have space put for them in the
            # .bss section
            self._allocate_sym_from_section(elf, '.bss')
        elif self.shndx == SHN_UNDEF:
            if self.name == '_GLOBAL_OFFSET_TABLE_':
                # Point the symbol at the .got section
                self._set_sym_from_section(elf.allocate_section('.got'))
            elif self.name == '_SDA_BASE_':
                # Point the symbol at the .sdata section
                self._set_sym_from_section(elf.allocate_section('.sdata'))
        elif self.shndx >= SHN_LOPROC and self.shndx <= SHN_HIPROC:
            self._allocate_sym_from_section(elf, '.sbss')
        elif self.type == STT_NOTYPE and self.shndx == SHN_COMMON:
            self._allocate_sym_from_section(elf, '.sbss')

    def update(self, elf):
        """
        Update the value field of this symbol with the address of the section
        that the symbol is linked to.
        """
        if self.section is not None:
            self.value += self.section.address

    def update_merged_sections(self, merged_sects):
        """
        If this symbol points to any of the merged sections then update it to
        point at the new section and modify the value by the offset.
        """
        if self.section in merged_sects:
            new_sect, offset = merged_sects[self.section]
            self.section = new_sect
            self.value += offset

class ElfReloc(object):
    """Represents an ELF relocation"""

    __slots__ = (
        "verbose",
        "offset",
        "type",
        "symdx",
        "val",
        )

    def __init__(self, reloc_struct = None):
        if reloc_struct:
            self.offset = reloc_struct.r_offset
            self.type = reloc_struct.r_type
            self.symdx = reloc_struct.r_symdx
        else:
            self.offset = 0
            self.type = 0
            self.symdx = 0
        self.verbose = False
        self.val = None

    def _output(self, file_):
        """
        Pretty print the relocation to a given file.
        """
        print >> file_, "%8.8x" % self.offset,
        print >> file_, "%8.8x" % self.type,
        print >> file_, "%s" % self.symdx

    # def output(self, file_=sys.stdout, wide=False):
    def output(self, file_=sys.stdout, _=False):
        """
        Pretty print the relocation to a given file, which defaults to
        stdout.
        """
        self._output(file_)
        print >> file_

    def to_struct(self, struct_cls, endianess, machine):
        """
        Create and populate a struct of the given class and endianess
        using the values from this user structure.
        """
        type_class = ABI_REGISTRATION[machine][1]
        relocstruct = struct_cls(endianess, "\0"*12 , 0, type_class)
        relocstruct.r_offset = self.offset
        relocstruct.r_symdx = self.symdx
        relocstruct.r_type = self.type
        return relocstruct

    def apply(self, elf, sect, symtab):
        """
        Apply this relocation to the specified section using the provided
        symbol table.
        """

        # If the data for this reloc doesn't actually exist, just skip it
        if (self.offset >= sect.get_size()):
            return True

        offset = self.offset
        addend = sect.get_word_at(offset)

        if self.verbose:
            print "\tApplying relocation (REL)", self.type
            print "\t\tSection", sect.name
            print "\t\tOffset %x" % offset
            print "\t\tAddress %x" % (sect.address + offset)
            print "\t\tAddend %x" % addend
            print "\t\tSymdx", self.symdx
            print "\t\tSymbol (deref)", symtab.symbols[self.symdx]

        #pylint: disable-msg=E1103
        val = self.type.calculate(elf, sect, symtab, addend, offset,
                                  self.symdx)

        if self.verbose:
            if val == None or val == True:
                print "\tCalculated value is", val
            else:
                print "\tCalculated value is %x" % val

        # Catch both not knowing how to handle this reloc as well as the
        # reloc_none (which returns True)
        if val != None and type(val) != bool:
            sect.set_word_at(offset, val)

        return val is not None

    def revert(self, elf, sect, symtab):
        """
        Revert the data pointed at by this relocation to its original state.
        """

        # If the data for this reloc doesn't actually exist, just skip it
        if (self.offset >= sect.get_size()):
            return

        addend = sect.get_word_at(self.offset)

        if self.verbose:
            print "\tReverting relocation (REL)", self.type
            print "\t\tSection", sect.name
            print "\t\tOffset %x" % self.offset
            print "\t\tAddress %x" % (sect.address + self.offset)
            print "\t\tAddend %x" % addend
            print "\t\tSymdx", self.symdx
            print "\t\tSymbol (deref)", symtab.symbols[self.symdx]

        #pylint: disable-msg=E1103
        self.val = self.type.revert(elf, sect, symtab, addend, self.offset,
                               self.symdx)

        if self.verbose:
            if self.val == None or self.val == True:
                print "\tReverted value is", self.val
            else:
                print "\tReverted value is %x" % self.val

    def write(self, elf, sect, symtab):
        # Catch both not knowing how to handle this reloc as well as the
        # reloc_none (which returns True)
        if self.val != None and type(self.val) != bool:
            sect.set_word_at(self.offset, self.val)

    def allocate(self, elf, sect, symtab, relocs):
        """
        Allocate any data that this relocation requires e.g. adding .got
        entries.
        """
        if self.verbose:
            print "\tAllocating relocation (REL)", self.type
            print "\t\tSection", sect.name
            print "\t\tOffset %x" % self.offset
            print "\t\tAddress %x" % (sect.address + self.offset)
            print "\t\tSymdx", self.symdx
            print "\t\tSymbol (deref)", symtab.symbols[self.symdx]

        #pylint: disable-msg=E1103
        self.type.allocate(elf, sect, symtab, self.offset, self.symdx, relocs)

    def update_merged_sections(self, offset):
        """
        These relocations point at a merged section, modify thier offsets by
        the given offset so they point at the correct data to modify.
        """
        self.offset += offset

    def update_discarded_symbols(self, discarded_syms):
        """
        Update the symbol index of this relocation to match what it should be
        after dropping some symbols.  Discarded_syms holds a list of the symbol
        index's that have been dropped, pull everything above the index's down.
        """
        decrement = 0
        symdx = self.symdx

        for discarded_sym_idx in discarded_syms:
            if symdx > discarded_sym_idx:
                decrement += 1
            else:
                break

        self.symdx -= decrement

    def update_merged_symbols(self, merged_syms):
        """
        Update this relocation to point at the correct new symbol index using
        the mappings provided.
        """
        # If we refer to the null symbol (for whatever reason) this will always
        # be entry 0 in the symbol table, so we don't need to update
        if self.symdx == 0:
            return

        if self.symdx in merged_syms:
            self.symdx = merged_syms[self.symdx]
            return

        raise Exception, "Unable to find new symbol matching when updating " \
              "relocation, old index %d" % self.symdx

    def set_verbose(self, verbose):
        """
        Set the verbose debugging flag.  If set then relocation
        actions will print state information as they run.
        """
        self.verbose = verbose
        self.type.verbose = verbose

class ElfReloca(ElfReloc):
    """Represents an ELF RELA relocation"""

    __slots__ = (
        "addend",
        )

    def __init__(self, reloc_struct=None):
        ElfReloc.__init__(self, reloc_struct)

        if reloc_struct:
            self.addend = reloc_struct.r_addend
        else:
            self.addend = 0

    # def output(self, file_=sys.stdout, wide=False):
    def output(self, file_=sys.stdout, _=False):
        """
        Pretty print the relocation to a given file, which defaults to
        stdout.
        """
        self._output(file_)
        print >> file_, "%8.8x" % self.addend

    def to_struct(self, struct_cls, endianess, machine):
        """
        Create and populate a struct of the given class and endianess
        using the values from this user structure.
        """
        type_class = ABI_REGISTRATION[machine][1]
        relocstruct = struct_cls(endianess, "\0"*24 , 0, type_class)
        relocstruct.r_offset = self.offset
        relocstruct.r_symdx = self.symdx
        relocstruct.r_type = self.type
        relocstruct.r_addend = self.addend
        return relocstruct

    def apply(self, elf, sect, symtab):
        """
        Apply this relocation to the specified section using the provided
        symbol table.
        """

        if self.verbose:
            print "\tApplying relocation (RELA)", self.type
            print "\t\tSection", sect.name
            print "\t\tOffset %x" % self.offset
            print "\t\tAddress %x" % (sect.address + self.offset)
            print "\t\tAddend %x" % self.addend
            print "\t\tSymdx", self.symdx
            print "\t\tSymbol (deref)", symtab.symbols[self.symdx]

        #pylint: disable-msg=E1103
        val = self.type.calculate(elf, sect, symtab, self.addend, self.offset,
                                  self.symdx)

        if self.verbose:
            if val == None or val == True:
                print "\tCalculated value is", val
            else:
                print "\tCalculated value is %x" % val

        # Catch both not knowing how to handle this reloc as well as the
        # reloc_none (which returns True)
        if val != None and type(val) != bool:
            sect.set_word_at(self.offset, val)

        return val is not None

    def revert(self, elf, sect, symtab):
        """
        ElfReloca.revert() does nothing, because the original addend is
        provided in the RELA structure.
        """
        return True
