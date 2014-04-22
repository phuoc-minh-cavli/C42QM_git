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
Common relocation functions

Relocation functions should just return the calculated value (not write it
back into the data) if successful otherwise they can return None to indicate
that they were unable to process the relocation (this is the default behaviour
if no function is provided for relocation)
"""

from elf.constants import STT_FUNC
from elf.user_structures import ElfReloca

def reloc_get_symbol_value(symtab, symdx):
    """
    Return the value of a symbol used in a relocation.
    """
    return symtab.symbols[symdx].value

def reloc_get_addressing_origin_of_symbol(symtab, symdx):
    """
    Return B(S), the addressing origin of the section pointed to by the symbol.
    """
    return symtab.symbols[symdx].section.address

def reloc_update_got_entry(elf, symtab, symdx):
    """
    Update the value stored in this symbol's got entry.
    """
    sym = symtab.symbols[symdx]

    assert sym.got_offset is not None

    elf.get_got().set_word_at(sym.got_offset, sym.value)

def reloc_get_got_address_of_symbol(elf, symtab, symdx):
    """
    Return the address of this symbols entry in the got table (absolute).
    """
    sym = symtab.symbols[symdx]
    got_org = elf.get_got().address

    return got_org + sym.got_offset

def reloc_get_position(sect, offset):
    """
    Return the address of the instruction being relocated.
    """
    return sect.address + offset

def reloc_info(elf, sect, symtab, addend, offset, symdx):
    """
    Debugger function that simply prints out info about the relocation.
    """
    print "Applying reloc to section", sect
    print "at offset %x" % offset
    print "using symbol", symtab.symbols[symdx]
    print "and addend %x" % addend

    return None

def reloc_alloc_got_symbol(elf, sect, symtab, offset, symdx, relocs):
    """
    If this symbol has no entry in the .got table add one
    """
    sym = symtab.symbols[symdx]

    elf.add_got_entry(sym)

def reloc_calc_none(elf, sect, symtab, addend, offset, symdx):
    """
    This lets a reloc that does not perform any actions be processed without
    raising an unimplemented error.
    """
    return True

def reloc_calc_S_add_A(elf, sect, symtab, addend, offset, symdx):
    """
    Implement reloc
        S + A
    """
    return symtab.symbols[symdx].value + addend

def reloc_calc_S_add_A_sub_P(elf, sect, symtab, addend, offset, symdx):
    """
    Implement reloc
        S + A - P
    """
    sym_val = reloc_get_symbol_value(symtab, symdx)

    pos = reloc_get_position(sect, offset)

    return sym_val + addend - pos

def reloc_calc_B_S_add_A_sub_P(elf, sect, symtab, addend, offset, symdx):
    """
    Immplement reloc
        B(S) + A - P
    """
    ao_sym = reloc_get_addressing_origin_of_symbol(symtab, symdx)

    pos = reloc_get_position(sect, offset)

    return ao_sym + addend - pos

def reloc_calc_GOT_S_add_A_sub_GOT_ORG(elf, sect, symtab, addend, offset, symdx):
    """
    Implement reloc
        GOT(S) + A - GOT_ORG
    """
    ga_sym = reloc_get_got_address_of_symbol(elf, symtab, symdx)

    got_org = elf.get_got().address

    return ga_sym + addend - got_org

def reloc_calc_S_add_A_sub_GOT_ORG(elf, sect, symtab, addend, offst, symdx):
    """
    Implement reloc
        S + A - GOT_ORG
    """
    sym_val = reloc_get_symbol_value(symtab, symdx)

    got_org = elf.get_got().address

    return sym_val + addend - got_org

def reloc_revert_none(elf, sect, symtab, addend, offset, symdx):
    """
    As per the calc version, do nothing
    """
    return True

def reloc_revert_0(elf, sect, symtab, addend, offset, symdx):
    """
    Revert an entry to 0x0
    """
    return 0x0

def reloc_revert_S_add_A(elf, sect, symtab, addend, offset, symdx):
    """
    Revert reloc
        S + A
    """
    sym_val = reloc_get_symbol_value(symtab, symdx)

    return addend - sym_val

def reloc_revert_S_add_A_or_T(elf, sect, symtab, addend, offset, symdx):
    """
    Revert reloc
        (S + A) | T
    """
    sym_val = reloc_get_symbol_value(symtab, symdx)

    return addend - sym_val

def reloc_revert_S_add_A_sub_P(elf, sect, symtab, addend, offset, symdx):
    """
    Revert reloc
        S + A - P
    """
    sym_val = reloc_get_symbol_value(symtab, symdx)

    pos = reloc_get_position(sect, offset)

    return addend - sym_val + pos

def reloc_revert_B_S_add_A_sub_P(elf, sect, symtab, addend, offset, symdx):
    """
    Revert reloc
        B(S) + A - P
    """
    ao_sym = reloc_get_addressing_origin_of_symbol(symtab, symdx)

    pos = reloc_get_position(sect, offset)

    return addend - ao_sym + pos

def reloc_revert_S_add_A_sub_GOT_ORG(elf, sect, symtab, addend, offst, symdx):
    """
    Revert reloc
        S + A - GOT_ORG
    """
    sym_val = reloc_get_symbol_value(symtab, symdx)

    got_org = elf.get_got().address

    return addend - sym_val + got_org

def reloc_new(relocs, type_class, addend, offset, symdx):
    rel = ElfReloca()
    rel.type = type_class
    rel.addend = addend
    rel.offset = offset
    rel.symdx = symdx
    relocs.add_reloc(rel)
