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
Provide functionality to model linker scripts as a series of nested commands.
"""

from elf.constants import STT_OBJECT, STB_GLOBAL, STV_DEFAULT, SHN_ABS, \
        SHF_WRITE, SHF_ALLOC, SHF_EXECINSTR, SHF_GROUP, SHF_ALLOC, PT_LOAD
from elf.user_structures import ElfSymbol
from elf.util import align_up
from elf.ByteArray import ByteArray


# Default alignment for segments.  Kernels are aligned to 0x8000.  See
# linker scripts.
DEFAULT_SEGMENT_ALIGN = 0x1000

def merge_sections(base_sect, merge_sect, merged_sects, remove_sects, verbose):
    """Merge sections together."""

    if verbose:
        print "\tMerging in section %s to section %s" % (merge_sect.name,
                                                         base_sect.name)

    size = base_sect.get_size()
    offset = align_up(size, merge_sect.addralign)

    base_sect.merge(merge_sect, offset - size)

    merged_sects[merge_sect] = (base_sect, offset)

    if remove_sects is not None:
        remove_sects.append(merge_sect)

    if verbose:
        print "\tPadded by %#x bytes" % (offset - size)
        print "\tMerged data starts at %#x" % offset

# Don't complain about too few public methods.
#pylint: disable-msg=R0903
class Command(object):
    """
    Top level class, sole purpose of existance is to hold a collection of
    variables that are shared between all the commands and drastically reduces
    the need to pass multiple arguments around.
    """

    elf = None
    sym_tab = None
    section = None
    segment = None
    virt_addr = 0
    phys_addr = 0
    merged_sects = {}
    discarded_sects = []
    sections = []
    segments = []
    verbose = False

    def __init__(self):
        pass

    def parse(self):
        """Process the contents of the linker script command."""
        raise NotImplementedError

def command_reset():
    """Reset the global command variables."""

    Command.elf = None
    Command.sym_tab = None
    Command.section = None
    Command.segment = None
    Command.virt_addr = 0
    Command.merged_sects = {}
    Command.discarded_sects = []
    Command.sections = []
    Command.segments = []
    Command.verbose = False

class CommandObj(object):
    """
    Wrapper class to allow us to use direct values of callables as parameters.
    """

    def __init__(self, value, *args):
        self.value = value

        if callable(self.value):
            # Due to using kwargs['name'] to pass in args in callers we are
            # given a list rather than the args directly
            self.value_args = args[0]

            # If no args were passed in convert to an empty sequence so that
            # we can call the function properly
            if self.value_args == None:
                self.value_args = ()

            # Make sure we have a sequence
            if type(self.value_args) != type(()):
                self.value_args = (self.value_args,)

    def __call__(self):
        if callable(self.value):
            if Command.verbose:
                print "\tCalling %s to calculate value with args" % \
                        self.value, self.value_args

            return self.value(*self.value_args)
        else:
            return self.value

class Assert(Command):
    """
    Assert that a given expression is true at link-time.
    """

    def __init__(self, assert_func, error_string, **kwargs):
        Command.__init__(self)

        self.assert_func = assert_func
        self.error_string = error_string

    def parse(self):
        """Assert that a function evalutes to True."""

        if Command.verbose:
            print "Assertion object"

        result = self.assert_func()
        if result != True:
            if type(self.error_string) == type(lambda: None):
                s = self.error_string()
            else:
                s = self.error_string
            raise Exception("Link-time assertion failed: %s" % (s))

class Memory(Command):
    """
    Simulate the memory section of a gnu linker script, sets the base point
    that we start addressing from after the command.  Instead of having a
    MEMORY {} section then referring to the entries (rom, ram, etc.) in it
    all following sections are assumed to go into the section described by
    the base and size until a new memory entry is found.
    """

    def __init__(self, virt_addr, **kwargs):
        Command.__init__(self)

        self.virt_addr = CommandObj(virt_addr, kwargs.get('virt_addr_args'))

    def parse(self):
        """Create a new segment starting at the base address."""

        if Command.verbose:
            print "Memory object"

        Command.virt_addr = self.virt_addr()

        if Command.verbose:
            print "\tSetting virt address to %#x" % Command.virt_addr

class CommandSegment(object):
    """
    Class to represent an segment object as we work upon it, cleans up the
    previous use of a tuple for this purpose.
    """

    def __init__(self, virt_addr, align, segment_type):
        self.virt_addr = virt_addr
        self.align = align
        self.segment_type = segment_type
        self.sections = []
        self.flags = None

class Segment(Command):
    """
    Declare the start of a new segment.
    """

    def __init__(self, align = DEFAULT_SEGMENT_ALIGN, segment_type = PT_LOAD,
                 **kwargs):
        Command.__init__(self)

        self.align = CommandObj(align, kwargs.get('align_args'))
        self.segment_type = segment_type

    def parse(self):
        """Create a new segment aligned to the given value."""

        if Command.verbose:
            print "Segment object"

        Command.virt_addr = align_up(Command.virt_addr, self.align())

        Command.segment = CommandSegment(Command.virt_addr, self.align(),
                                         self.segment_type)
        Command.segments.append(Command.segment)

        if Command.verbose:
            print "\tNew segment of type %s, starts at virt address %#x" % \
                    (self.segment_type, Command.virt_addr)
            print "\tAdding new segment at index %d" % \
                    Command.segments.index(Command.segment)

class Section(Command):
    """
    Simulate the section command in the gnu linker script.  Creates a section
    named name based on base_sect.  If not provided base_sect is assumed to
    be the same as name.  Can have a collection of commands within it to merge
    other sections or define symbols.
    """

    def __init__(self, name, commands, align = 0, base_sect = None, **kwargs):
        Command.__init__(self)

        self.name = name
        self.commands = commands
        self.base_sect = name

        if align != 0:
            align = CommandObj(align, kwargs.get('align_args'))
            self.align = Align(align())
        else:
            self.align = None

        if base_sect:
            self.base_sect = base_sect

    def parse(self):
        """Create the new section."""

        if Command.verbose:
            print "Section object"

        sect = Command.elf.find_section_named(self.base_sect)

        if sect is None:
            if Command.verbose:
                print "\tNot creating section, unable to find base section %s" \
                        % self.base_sect

            return
        elif Command.verbose:
            print "\tCreating section %s based on base section %s" % \
                    (self.name, self.base_sect)

        # Before assigning the section do an align
        if self.align != None:
            self.align.parse()

        Command.section = Command.elf.clone_section(sect, Command.virt_addr)
        Command.sections.append(Command.section)

        # Now set the address of the section given the current address
        Command.section.address = align_up(Command.virt_addr,
                                           Command.section.addralign)
        Command.virt_addr = Command.section.address

        # XXX: This checks works for the current rvct but if we are merging
        # sections that aren't going into a segment they should not affect
        # the address value at all.  Fix later.  (Related to merging all the
        # 8000+ .debug_foo$$$morefoo sections in RVCT)
        if Command.section.flags & SHF_ALLOC:
            if len(Command.segment.sections) == 0:
                Command.segment.virt_addr = Command.virt_addr

            Command.segment.sections.append(Command.section)

        if self.name != self.base_sect:
            Command.section.name = self.name

        if Command.verbose:
            print "\tSection virt address is %#x" % Command.section.address
            print "\tParsing commands..."

        for command in self.commands:
            command.parse()

        Command.section = None

        if Command.verbose:
            print "Finished section %s" % self.name

class Merge(Command):
    """
    Merge the list of sections into the current section being created.  Do not
    mix the use of explicit sections 'foo.bar' with wildcards 'foo.*'.
    """

    def __init__(self, sections):
        Command.__init__(self)

        self.sections = sections

    def parse(self):
        """Merge together the sections fron different files."""

        if Command.verbose:
            print "Merge object"

        remove_sects = []

        for sect in self.sections:
            if sect[-1] == '*':
                if Command.verbose:
                    print "\tMerging in all sections %s" % sect

                glob_name = sect[:-1]

                for merge_sect in Command.elf.sections:
                    if merge_sect.name.find(glob_name) == 0 and \
                            merge_sect.name != glob_name and \
                            merge_sect != Command.section:
                        merge_sections(Command.section, merge_sect,
                                       Command.merged_sects, remove_sects,
                                       Command.verbose)
            else:
                merge_sect = Command.elf.find_section_named(sect)

                if not merge_sect:
                    if Command.verbose:
                        print "\tUnable to find section %s to merge" % sect

                    continue

                merge_sections(Command.section, merge_sect,
                               Command.merged_sects, remove_sects,
                               Command.verbose)

        for sect in remove_sects:
            Command.elf.remove_section(sect)

        # Reset the current address based on the extra data added
        Command.virt_addr += Command.section.address + \
                Command.section.get_size() - Command.virt_addr

        if Command.verbose:
            print "\tSetting virt address to %#x" % Command.virt_addr

class MergeFlags(Command):
    """
    Merge all sections with the given flags (read, write, execute).
    """

    def __init__(self, flags):
        Command.__init__(self)

        self.flags = flags

    def parse(self):
        """Set the merged flags."""

        if Command.verbose:
            print "MergeFlags object"
            print "\tMerging all sections with flags %d" % self.flags
            print "\t\t(A %d, W %d, X %d, G %d)" % \
                  (self.flags & SHF_ALLOC > 0,
                   self.flags & SHF_WRITE > 0,
                   self.flags & SHF_EXECINSTR > 0,
                   self.flags & SHF_GROUP > 0)

        remove_sects = []

        for sect in Command.elf.sections:
            if sect.flags == self.flags:
                merge_sections(Command.section, sect, Command.merged_sects,
                               remove_sects, Command.verbose)

        for sect in remove_sects:
            Command.elf.remove_section(sect)

        # Reset the current address based on the extra data added
        Command.virt_addr += Command.section.address + \
                Command.section.get_size() - Command.virt_addr

        if Command.verbose:
            print "\tSetting virt address to %#x" % Command.virt_addr

class Align(Command):
    """
    Align the current address to the given value.
    """

    def __init__(self, value, **kwargs):
        Command.__init__(self)

        self.value = CommandObj(value, kwargs.get('value_args'))

    def parse(self):
        """Move the address pointer."""

        if Command.verbose:
            print "Align object"

        old_addr = Command.virt_addr
        Command.virt_addr = align_up(Command.virt_addr, self.value())

        if Command.verbose:
            print "\tSetting virt address to %#x" % Command.virt_addr

        if Command.section:
            Command.section.append_data(ByteArray('\0' * (Command.virt_addr -
                                                          old_addr)))

            if Command.verbose:
                print "\tPadding current section by %#x bytes" % \
                        (Command.virt_addr - old_addr)

class Symbol(Command):
    """
    Define a symbol at the current address.
    """

    def __init__(self, name, value = None, **kwargs):
        Command.__init__(self)

        self.name = name
        self.value = CommandObj(value, kwargs.get('value_args'))

    def parse(self):
        """Define the symbol."""

        class SymbolStruct:
            """
            Pseudo symbol structure because none of the official
            classes have quite the constructor interface that we're
            after.
            """

            # Don't complain about many arguments.
            #pylint: disable-msg=R0913
            def __init__(self, value, size, sym_type, bind, other, shndx):
                self.st_value = value
                self.st_size = size
                self.st_type = sym_type
                self.st_bind = bind
                self.st_other = other
                self.st_shndx = shndx

        if Command.verbose:
            print "Symbol object"

        sym = Command.sym_tab.get_symbol(self.name)
        # We call and assign here (and reasign value if needed below) so that
        # we only see a single verbose print if the value() is actually a
        # wrapped function.
        value = self.value()
        if sym:
            if value == None:
                sym.value = Command.virt_addr
                value = Command.virt_addr
            else:
                sym.value = value
            sym.type = STT_OBJECT
            sym.shndx = SHN_ABS
            sym.section = None
        else:
            if value == None:
                value = Command.virt_addr
            sym_st = SymbolStruct(value, 0, STT_OBJECT, STB_GLOBAL,
                                  STV_DEFAULT, SHN_ABS)
            Command.sym_tab.add_symbol(ElfSymbol(self.name, None, sym_st))

        if Command.verbose:
            print "\tCreated symbol %s at address %#x" % (self.name, value)

class Pad(Command):
    """
    Pad out memory by a given number of bytes.
    """

    def __init__(self, value, **kwargs):
        Command.__init__(self)

        self.value = CommandObj(value, kwargs.get('value_args'))

    def parse(self):
        """Pad out the memory."""

        if Command.verbose:
            print "Pad object"

        old_addr = Command.virt_addr
        Command.virt_addr += self.value()

        if Command.verbose:
            print "\tSetting virt address to %#x" % Command.virt_addr

        if Command.section:
            Command.section.append_data(ByteArray('\0' * (Command.virt_addr -
                                                          old_addr)))

            if Command.verbose:
                print "\tPadding current section by %#x bytes" % \
                        (Command.virt_addr - old_addr)

class Discard(Command):
    """
    Discard the list of sections.
    """

    def __init__(self, sections):
        Command.__init__(self)

        self.sections = sections

    def parse(self):
        """Discard the sections."""

        if Command.verbose:
            print "Discard object"

        remove_sects = []

        for sect in self.sections:
            if sect[-1] == '*':
                if Command.verbose:
                    print "\tDiscard all sections %s" % sect

                glob_name = sect[:-1]

                for discard_sect in Command.elf.sections:
                    if discard_sect.name.find(glob_name) == 0:
                        Command.discarded_sects.append(discard_sect)
                        remove_sects.append(discard_sect)

                        if Command.verbose:
                            print "\t\tDiscarding section %s" % \
                                    discard_sect.name
            else:
                discard_sect = Command.elf.find_section_named(sect)

                if not discard_sect:
                    if Command.verbose:
                        print "\tUnable to find section %s to discard" % sect

                    continue

                Command.discarded_sects.append(discard_sect)
                remove_sects.append(discard_sect)

                if Command.verbose:
                    print "\tDiscarding section %s" % sect

        for sect in remove_sects:
            Command.elf.remove_section(sect)

class DiscardType(Command):
    """
    Discard sections based on their types.
    """

    def __init__(self, types):
        Command.__init__(self)

        self.types = types

    def parse(self):
        """Discard the sections."""

        if Command.verbose:
            print "DiscardType object"
            print "\tDiscarding sections with types", \
                  [sect_type.__str__() for sect_type in self.types]

        remove_sects = []

        for sect in Command.elf.sections:
            if sect.type in self.types:
                Command.discarded_sects.append(sect)
                remove_sects.append(sect)

                if Command.verbose:
                    print "\t\tDiscarding section %s" % sect.name

        for sect in remove_sects:
            Command.elf.remove_section(sect)
