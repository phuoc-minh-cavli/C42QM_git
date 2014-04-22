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
Main class and helpers for extracting DWARF debug information from an
ELF, and displaying or otherwise manipulating it.
"""

from elf.dwarf.dwarf_data import DwarfData
from elf.dwarf.dwarf_constants import DwarfAttribute, DwarfForm, DwarfTag, \
        DwarfChildren

from elf.dwarf.dwarf_constants import FORM_REF4, FORM_DATA4, FORM_DATA2, \
        FORM_DATA1, FORM_SDATA, FORM_FLAG, FORM_ADDR, FORM_STRING, FORM_STRP, \
        FORM_REF4, FORM_BLOCK1, FORM_REF8, FORM_REF2, FORM_REF1, \
        AT_type, AT_name, \
        DW_CHILDREN_yes, \
        TAG_enumeration_type, TAG_enumeration_type, TAG_typedef

def form_write( data, form, value, offset):
    """ Writes 'value' to the data block 'data' at the
        specified offset and of the specified form """
    if (form == FORM_REF4):
        data.put_word(offset, value)

        return

    raise Exception("DON'T know how to write form ", form)

def display_form(form, data, str_data, origin):
    """ Display 'data' in a manner appropriate to it's form """
    if (form == FORM_DATA4):
        ret = "0x%x\t" % data
    elif (form == FORM_DATA2):
        ret = "%d\t" % data
    elif (form == FORM_DATA1):
        ret = "%d\t" % data
    elif (form == FORM_SDATA):
        ret = "%d\t" % data
    elif (form == FORM_FLAG):
        ret = "%d\t" % data
    elif (form == FORM_ADDR):
        ret = "0x%x\t" % data
    elif (form == FORM_STRING):
        ret = data
    elif (form == FORM_STRP):
        ret = "%s" % str_data.peek_string(data)
    elif (form == FORM_REF4):
        ret = "<0x%x>" % (data+origin)
    elif (form == FORM_BLOCK1):
        ret = "%d byte block: " % len(data)
    else:
        ret = form

    return ret

def get_form(form, data, ptr_sz):
    """ Given a form type, consume the appropriate amount of data and
        return its value """
    if (form == FORM_REF8):
        val = data.consume_doubleword()
    elif (form == FORM_REF4):
        val = data.consume_word()
    elif (form == FORM_REF2):
        val = data.consume_short()
    elif (form == FORM_REF1):
        val = data.consume_byte()
    elif (form == FORM_DATA4):
        val = data.consume_word()
    elif (form == FORM_DATA2):
        val = data.consume_short()
    elif (form == FORM_DATA1):
        val = data.consume_byte()
    elif (form == FORM_ADDR):
        val = data.consume_addr(ptr_sz)
    elif (form == FORM_STRING):
        val = data.consume_string()
    elif (form == FORM_SDATA):
        val = data.consume_sleb128()
    elif (form == FORM_BLOCK1):
        length = data.consume_byte()
        val = data.consume_bytes(length)
        val = val
    elif (form == FORM_STRP):
        offset = data.consume_word()
        val = offset
    elif (form == FORM_FLAG):
        val = data.consume_byte()
        val = val
    else:
        val = ("UNKNOWN FORM "+ str(form))

    return val



class DIE(object):
    """ Base type to represent a Dwarf Information Element (DIE).
        Subclasses of this object are dynamically created based
        on the abbreviation table (CompilationUnit.get_abbrevs())
        and the attributes table populated accordingly. """

    def __init__(self, offset, depth, cu):
        self.depth = depth
        self.offset = offset
        self.attr_value = {}
        self.attr_pos = {}
        self.cu = cu

        # These members populate by self.get_values()
        self.ptr_sz = None
        self.name = None
        self.type = None
        self.str_data = None
        self.data = None

    def __str__(self):
        string = " <%d><%x>: Abbrev Number: %d (%s)" % \
                 (self.depth,self.offset,self.number, self.tag.__str__().strip())
        for (attr, form) in self.attributes:
            string += "\n    <%2x>   %s: " % (self.attr_pos[attr], attr)
            string += display_form(form, self.attr_value[attr],
                                   self.str_data, self.cu.offset)

        return string

    def get_values(self, data, ptr_sz, str_data):
        """ Populate attr_pos and attr_value for each attribute from
            their offset and value within the debug.info data section """
        self.ptr_sz = ptr_sz
        self.name = "<<noname>>"
        self.type = None
        self.str_data = str_data
        self.data = data

        for (attr, form) in self.attributes:
            self.attr_pos[attr] = data.get_consumed()
            value =  get_form(form, data, ptr_sz)
            self.attr_value[attr] = value

            if attr == AT_name:
                self.name = display_form(form, value, str_data, self.offset)
            if attr == AT_type:
                self.type = value

    def set_type(self, type_die):
        """ Changes the AT_type attribute of the DIE to refer to the passed
            in 'type' by calculating the required offset from the base of the
            compilation unit, and modifying the binary data accordingly. """

        new_type = type_die.offset - self.cu.offset

        form = None
        for (attr, _form) in self.attributes:
            if attr == AT_type:
                form = _form

        if form is None:
            raise Exception("Why have no type form?")

        offset = self.attr_pos[long(AT_type)]

        self.type = new_type
        self.attr_value[AT_name] = new_type

        form_write(self.data, form, new_type, offset)

    def has_children(self):
        """ True if the DIE has children """
        return self.children == DW_CHILDREN_yes




class CompilationUnit(object):
    """ Class representing a Compilation Unit (cu) with an ELF """
    def __str__(self):
        string =  "  Compilation Unit @ offset %#x:\n" % (self.offset)
        string += "   Length:        %#x (32-bit)\n" % self.length
        string += "   Version:       %d\n" % self.version
        string += "   Abbrev Offset: %d\n" % self.abbrev_offset
        string += "   Pointer Size:  %d" % self.ptr_sz

        return string

    def __init__(self, data, abbrev_data, str_data):
        self.str_data = str_data
        depth = 0

        self.offset = data.get_consumed()
        self.length = data.consume_word()
        end_offset = data.get_consumed() + self.length
        self.version = data.consume_short()
        self.abbrev_offset = data.consume_word()
        self.ptr_sz = data.consume_byte()

        abbrevs = self.get_abbrevs(abbrev_data, self.abbrev_offset)

        self.root = None
        self.dies = {}
        self.enums = {}
        self.typedefs = {}

        while data.get_consumed() < end_offset:
            offset = data.get_consumed()
            abbrev = data.consume_uleb128()

            if abbrev == 0:
                depth -= 1
                continue

            die = abbrevs[abbrev](offset, depth, self)
            die.get_values(data, self.ptr_sz, self.str_data)
            if die.has_children():
                depth += 1
            self.dies[offset] = die

            if die.tag == TAG_enumeration_type:
                self.enums.setdefault(die.name, []).append(die)

            if die.tag == TAG_typedef:
                self.typedefs.setdefault(die.name, []).append(die)

            if self.root is None:
                self.root = die

        return


    def get_abbrevs(self, abbrev_data, offset):
        """ Get the abbreviations for the compilation unit.
            These are like templates that define what a DIE will look like
            as we come across them in the flattened tree in the debug info
            section """
        data = abbrev_data.reset(offset)
        abbrevs = {}
        done = False

        while not done:
            number = data.consume_uleb128()
            tag = data.consume_uleb128()
            children = data.consume_uleb128()

            attr_list = []

            while True:
                attr = data.consume_uleb128()
                form = data.consume_uleb128()

                if (attr == 0) and (form == 0):
                    next_value = data.peek_uleb128()

                    if next_value == 0:
                        done = True
                    break

                attr_list.append((DwarfAttribute(attr), DwarfForm(form)))

            obj_attrs = {
                         'attributes': attr_list,
                         'tag' : DwarfTag(tag),
                         'children' : DwarfChildren(children),
                         'number' : number
                         }

            abbrevs[number] = type('ABBREV_%d' % number, (DIE,), obj_attrs)

        return abbrevs


class Dwarf(object):
    """ Represents the general structure for all debugging DWARF information
        within an ELF """
    def __init__(self, elf):
        self.debug_str = elf.find_section_named(".debug_str")
        self.debug_info = elf.find_section_named(".debug_info")
        self.debug_abbrev = elf.find_section_named(".debug_abbrev")

        if (self.debug_str is None or self.debug_info is None or
            self.debug_abbrev is None):
            self.has_dwarf = False

            self.str_data = None
            self.info_data = None
            self.abbrev_data = None
        else:
            self.has_dwarf = True

            self.str_data =    DwarfData(self.debug_str.get_data())
            self.info_data =   DwarfData(self.debug_info.get_data())
            self.abbrev_data = DwarfData(self.debug_abbrev.get_data())

            self.cu = []
            self._from_data()

    def _from_data(self):
        """ populates the pythonic list of DIE objects from the binary
            data in the debug sections """
        data = self.info_data.reset(0)

        while data.have_data():
            cu = CompilationUnit(data, self.abbrev_data, self.str_data)
            if cu is None:
                break
            self.cu.append(cu)

    def find_enums(self):
        """ Finds all enumerated type declatations. Used internally for
            debugging """
        all_enums = []
        for cu in self.cu:
            all_enums += cu.enums.keys()
        return all_enums

    def fix_enums(self):
        """
        Here we 'fix' the enums:
        We collect all the enumerated types across all compilation units
        in the ELF.
        For each of these, if there is a typedef of the same name we modify
        the 'type' element each DIE (debug info element) retargetting the
        typedef from a word_t, to the correspondingly names C-language enum.

        NB: there may be many more than just one of each typedef to fixup,
        even when compiling with fwhole-program there seems to be debug
        data emittied for each .c file which uses the type!


        The end result is that the DWARF debug information which was compiled
        to look like (to comply with OKL coding standards)

            enum sometype_t {
                SOMETYPE_FOO = 1,
                SOMETYPE_BAR,
                SOMETYPE_BAZ,
            };

            typedef sometype_t word_t;

            #define SOMETYPE_FOO ((sometype_t)1UL)
            #define SOMETYPE_BAR ((sometype_t)2UL)
            #define SOMETYPE_BAZ ((sometype_t)3UL)

        will be modified so that it actually represents as if the code was
        written:

            enum sometype_t {
                SOMETYPE_FOO = 1,
                SOMETYPE_BAR,
                SOMETYPE_BAZ,
            };

            typedef sometype_t enum sometype_t;

        This 'fixing' occurs for any enum which shares the same name as a
        typedef.
        """


        if not self.has_dwarf:
            return

        all_enums = {}
        all_typedefs = {}

        for cu in self.cu:
            all_enums.update(cu.enums)
            all_typedefs.update(cu.typedefs)

        for offset, die in all_enums.iteritems():
            typedefs = all_typedefs.get(offset)

            for t in typedefs or []:
                t.set_type(die[0])


    def dump(self):
        for cu in self.cu:
            print cu

            for k in sorted(cu.dies.keys()):
                print cu.dies[k]
