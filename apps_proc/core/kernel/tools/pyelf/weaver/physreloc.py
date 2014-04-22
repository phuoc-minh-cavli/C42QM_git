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

"""Support objects for the image phys relocating table."""

import StringIO
from pyokl4.autogen import Autogen

from weaver.binarywriter import BinaryWriter

class PhysRelocWriter(StringIO.StringIO):
    def __init__(self):
        StringIO.StringIO.__init__(self)

        self.reloc_offsets = []

    def record_phys(self, relocs):
        if not relocs:
            return

        tell = self.tell()

        for reloc in relocs:
            reloc.address += tell

        self.reloc_offsets.extend(relocs)

    def to_phys_addr(self, base):
        for reloc in self.reloc_offsets:
            reloc.address += base

        return self.reloc_offsets

class PhysReloc(object):
    """Phys Reloc"""
    __slots__ = ('address', 'reloc_type', 'contents', 'must_filter')

    def __init__(self, address, reloc_type, contents, must_filter=True):
        self.address = address
        self.reloc_type = reloc_type
        self.contents = contents
        self.must_filter = must_filter

class PhysRelocTable(object):
    """Representation of the phys reloc table."""
    def __init__(self):
        self.reloc_table = []

    def add_relocation(self, address, reloc_type, contents, can_filter=True):
         """Add a relocation to the table."""
         self.reloc_table.append(PhysReloc(address, reloc_type, contents,
                                           can_filter))

    def add_relocations(self, relocs):
        """Add a list of relocations to the table."""
        self.reloc_table.extend(relocs)

        for reloc in relocs:
            assert reloc.contents is not None

    def to_data(self, reloc_pools, word_size, endianess):
        """
        Convert the phys reloc table into binary data.  Only relocations whose
        contents refer to the reloc pools are written out.
        """
        class RelocWriter(BinaryWriter):
            def __init__(self, word_size, endianess):
                BinaryWriter.__init__(self, word_size, endianess)

                self.output = StringIO.StringIO()

            def write_output(self, value):
                self.output.write(value)

        final_relocs = []

        # Filter out relocations whose contents do not point into relocatable
        # pools.  These addresses will not change.
        for reloc in self.reloc_table:
            for (_, pool) in reloc_pools:
                if (not reloc.must_filter) or pool.contains(reloc.contents, 4):
                    final_relocs.append(reloc)

        if not final_relocs:
            return None

        writer = RelocWriter(word_size, endianess)

        writer.write_word(Autogen.types.OKL4_phys_reloc.PHYS_RELOC_VERSION)
        writer.write_word(len(final_relocs))

        for reloc in final_relocs:
            writer.write_word(reloc.address)
            writer.write_word(reloc.reloc_type)

        return writer.output.getvalue()

    def dump(self):
        """Print out the contents of the phys reloc table."""
        for reloc in self.reloc_table:
            print "%#x %#x (%s %s)" % (reloc.offset, reloc.contents,
                                       reloc.reloc_type, reloc.can_filter)
