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

"""Misc relocation functions to streamline code paths"""

from elf.constants import ET_EXEC, ET_DYN
from elf.ByteArray import ByteArray

def revert_relocs(elf, machine):
    reloc_sections = elf.get_reloc_sections()

    if len(reloc_sections) == 0:
        return False

    # Tell the relocation code what sort of machine we're targeting.
    elf.abi.set_cpu(machine.cpu)

    # Do any ABI-specific fixups necessary to make relocation possible.
    elf.abi.fixup_relocs()

    # We need to revert all relocations before we copy anything. This
    # allows us to treat the code as if it was never linked at a specific
    # address, so we can move it around and re-apply the relocations later.
    for reloc_sect in reloc_sections:
        # The reloc.offset isn't an offset in dynamic shared objects and
        # executables, but an actual address; reset it back to being a
        # useful offset for the relocation code.
        if elf.elf_type in (ET_EXEC, ET_DYN):
            for reloc in reloc_sect.relocs:
                reloc.offset -= reloc_sect.info.address

        if reloc_sect.reverted:
            continue

        reloc_sect.revert()

    # And now for a bit of evil; the actual contents of the .got (what
    # the actual entries represent) can't be known without using the
    # original symbol values and trying to figure out from the relocs
    # and symbols what went where.  Soo, lets just empty the .got of
    # all data and rerun the allocation function for the relocs; this
    # should repopulate the entries correctly.
    if elf.get_got():
        elf.get_got()._data = ByteArray()

    # Now, allocate space for all the relocations. This includes allocating
    # .got entries, creating interworking veneers, etc.
    for sect in reloc_sections:
        sect.allocate_relocs()

    # Finally, move any sections that now overlap because allocate_relocs
    # resized things.
    elf.resolve_section_overlaps()

    return True

def apply_relocs(elf, machine):
    # Update all relocation sections to use the new symbol table and to
    # update their symbol indexes to the new correct values
    new_symtab = elf.get_symbol_table()

    for got, sects, syms in elf.reloc_merge_sections:
        for sect in sects:
            sect.update_merged_symbols(new_symtab, syms)

    # Now update all the symbols to the correct position, the copy_into
    # process has converted all the symbols back into offsets rather than
    # fixed addresses so this will work.
    new_symtab.update_symbols()

    # Now apply the relocations
    elf.abi.set_cpu(machine.cpu)
    for got, sects, syms in elf.reloc_merge_sections:
        if len(sects) == 0:
            continue

        elf.got = got

        for sect in sects:
            sect.apply()

