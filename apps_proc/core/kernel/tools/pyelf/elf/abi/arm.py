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
Support for the ARM ABI.
"""

import elf.abi
from elf.abi import ElfRelocType, ElfAbi
from elf.abi.common import reloc_calc_none, reloc_calc_S_add_A, \
        reloc_calc_S_add_A_sub_P, reloc_calc_B_S_add_A_sub_P, \
        reloc_calc_GOT_S_add_A_sub_GOT_ORG, reloc_alloc_got_symbol, \
        reloc_calc_S_add_A_sub_GOT_ORG, \
        reloc_revert_none, reloc_revert_0, reloc_revert_S_add_A, \
        reloc_revert_S_add_A_sub_P, reloc_revert_B_S_add_A_sub_P, \
        reloc_revert_S_add_A_sub_GOT_ORG, \
        reloc_get_symbol_value, reloc_get_position, reloc_new, \
        reloc_revert_S_add_A_or_T
from elf.constants import EM_ARM, EF_ARM_INTERWORK, EF_ARM_EABIMASK, \
        EF_ARM_EABI_VER2, EF_ARM_DYNSYMSUSESEGIDX, STT_FUNC, STT_SECTION, \
        SHN_ABS, ELFOSABI_ARM_AEABI, ElfFormatError
from elf.user_structures import ElfSymbol

class ArmElfAbi(ElfAbi):
    def _default_abi_information(self):
        self.have_thumb2 = False
        self.have_blx = False

    def _arch_for_cpu(self, cpu):
        if ("cortex-a5" in cpu) or ("cortex-a7" in cpu) or ("cortex-a8" in cpu) or ("cortex-a9" in cpu):
            return "armv7"
        elif ("arm11" in cpu):
            return "armv6"
        elif ("arm926" in cpu):
            return "armv5"
        elif ("arm7" in cpu) or ("sa1100" in cpu) or ("arm920" in cpu):
            return "armv4"
        else:
            raise Exception, "ArmElfAbi: Unknown CPU %s" % cpu

    def set_arch(self, arch):
        self.have_thumb2 = "armv7" in arch
        self.have_blx = not ("armv4" in arch)

    def fixup_relocs(self):
        # This is an evil hack. RVCT 2.2 generates BPABI binaries with
        # scatterload code in them. This is both unnecessary and broken:
        # no relocations are generated for the scatterload region table.
        # So, we short-circuit the scatterload code in BPABI binaries.
        if self.elf.osabi == ELFOSABI_ARM_AEABI:
            self._fixup_rvct22_scatterload()

        # In an ARM EABI2 executable, the relocs may have segment
        # rather than symbol indices. In order to deal with this, we
        # have to search for the section symbols that correspond to the
        # target addresses, and point the relocs at those instead.
        if self.elf.flags & EF_ARM_EABIMASK == EF_ARM_EABI_VER2 and \
                self.elf.flags & EF_ARM_DYNSYMSUSESEGIDX:
            symdx_fixups = self._fixup_dyn_segment_syms()

    def _fixup_rvct22_scatterload(self):
            __main = self.elf.find_symbol("__main")
            __scatterload = self.elf.find_symbol("__scatterload")

            if __main is None or __scatterload is None:
                # RVCT 4 gets this right, so we don't have to fix it.
                return

            main_offset = __main.value - __main.section.address
            branch_insn = __main.section.get_word_at(main_offset)
            branch_op = branch_insn & 0xff000000
            branch_offset = branch_insn & 0x00ffffff
            expected_offset = __scatterload.value - __main.value - 8

            if (branch_op != 0xea000000) or (branch_offset != expected_offset):
                raise Exception, \
                    "Expected branch to __scatterload at start of __main"

            nop_insn = 0xe1a00000 # mov r0, r0
            __main.section.set_word_at(main_offset, nop_insn)

    def _fixup_dyn_segment_syms(self):
        reloc_sections = self.elf.get_reloc_sections()
        symtab = self.elf.get_symbol_table()
        section_sym_dict = {}

        # Build a dictionary of section names to symbol indices.
        for idx, sym in enumerate(symtab.symbols):
            if sym.type == STT_SECTION:
                section_sym_dict[sym.value] = idx

        for relocs in reloc_sections:
            text_section = relocs.info

            for reloc in relocs.get_relocs():
                # The symdx is currently a segment index, which we ignore.
                # Instead, we find the section containing the address that is
                # being relocated, and point the symdx at that section's symbol.
                reloc_addr = reloc.offset - relocs.info.address
                target_addr = text_section.get_word_at(reloc_addr)
                target_section = self.elf.find_section_by_vaddr(target_addr)

                if target_section is None:
                    raise ElfFormatError, \
                        "Relocated address 0x%x is not in a section" % \
                            target_addr
                if target_section.address not in section_sym_dict.keys():
                    raise ElfFormatError, \
                        "No STT_SECTION symbol for section '%s'" % \
                            target_section.name

                reloc.symdx = section_sym_dict[target_section.address]

            # Unlike normal dynamic relocs, these do need to be reverted.
            relocs.reverted = False

# Because symbols may be prefixed with a cell name after the alloc, we have
# to put the target symbol name at the start of the veneer's symbol; we can't
# use either the ABI-defined format or the one GNU ld uses.
t2a_glue_sym_format = "%s-veneer-t2a"
a2t_glue_sym_format = "%s-veneer-a2t"

class ArmRelocType(ElfRelocType):
    """ElfRelocType for ARM processors. """
    _show = {}

def reloc_calc_S_add_A_or_T(elf, sect, symtab, addend, offset, symdx):
    """
    Implement reloc
        (S + A) | T
    """
    symbol = symtab.symbols[symdx]
    if symbol.type == STT_FUNC:
        S = symbol.value & ~1
        T = symbol.value & 1
    else:
        S = symbol.value
        T = 0
    return (S + addend) | T


def reloc_calc_S_add_A_or_T_sub_P(elf, sect, symtab, addend, offset, symdx):
    """
    Implement reloc
        ((S + A) | T) - P
    """
    symbol = symtab.symbols[symdx]
    if symbol.type == STT_FUNC:
        S = symbol.value & ~1
        T = symbol.value & 1
    else:
        S = symbol.value
        T = 0

    pos = reloc_get_position(sect, offset)

    return ((S + addend) | T) - pos

def reloc_calc_S_add_A_or_T_sub_GOT_ORG(elf, sect, symtab, addend, offst, symdx):
    """
    Implement reloc
        ((S + A) | T) - GOT_ORG
    """
    symbol = symtab.symbols[symdx]
    if symbol.type == STT_FUNC:
        S = symbol.value & ~1
        T = symbol.value & 1
    else:
        S = symbol.value
        T = 0

    got_org = elf.get_got().address

    return ((S + addend) | T) - got_org

def reloc_calc_arm_branch(elf, sect, symtab, addend, offset, symdx):
    """
    Do much arch specific magic.

    The ARM ELF spec doesn't seem to explicitly tell us what to do for this
    relocation (one assumes due to R_ARM_PC24, R_ARM_PLT32, etc. being
    deprecated) so this implementation is dervied from the binutils
    implementation.  Despite not doing anything to do with PLT's this
    function works correctly for simple R_ARM_PLT32's.
    """
    # We need to preserve the op code for when we return the final value
    op = sect.get_word_at(offset) & 0xff000000

    # This is a 24bit operation so we and in only the bits we want
    addend &= 0xffffff
    # Sign extension of the addend
    addend = (addend ^ 0x800000) - 0x800000
    # Addend needs to be converted into bytes (*4)
    addend <<= 2

    # Now calculate the generic part
    val = reloc_calc_S_add_A_sub_P(elf, sect, symtab, addend, offset, symdx)

    # Convert back from bytes and cut back to 24 bits
    val >>= 2
    val &= 0xffffff

    return op | val

def reloc_revert_arm_branch(elf, sect, symtab, addend, offset, symdx):
    """
    Revert the branch calculation in order to restore the original addend.
    """
    # Calculate the generic part with using a 0 addend
    val = reloc_calc_S_add_A_sub_P(elf, sect, symtab, 0, offset, symdx)
    # Shift to reverse what we did in calc
    val >>= 2

    # Preserve the op code
    op = sect.get_word_at(offset) & 0xff000000

    # Remove val from the addend, this leaves addend broken due to the sign
    # extension and all the anding and oring we do in calc
    addend -= val

    # So stick the op back onto the top of the addend
    return op | (addend & 0xffffff)

def find_t2a_glue(elf, symtab, symdx):
    sym = elf.find_symbol(t2a_glue_sym_format % symtab.symbols[symdx].name)
    assert sym
    return sym.value

def make_t2a_glue(elf, sect, symtab, symdx, relocs):
    sym = symtab.symbols[symdx]
    glue_sym_name = t2a_glue_sym_format % sym.name

    # Look for existing glue for this symbol, and return it if it exists
    glue_sym = elf.find_symbol(glue_sym_name)
    if glue_sym:
        return glue_sym.value

    # The target must be a thumb-mode function.
    if sym.type != STT_FUNC:
        raise Exception, \
                "Cannot make veneer for non-function symbol '%s'" % sym.name
    if (sym.value & 1):
        raise Exception, \
                "Cannot make t2a veneer for thumb function '%s'" % sym.name

    # The glue code is:
    # .thumb
    #   bx pc
    #   nop
    # .arm
    #   b 0 (R_ARM_PC24 sym.name)
    glue_code = (0x46c04778, 0xeafffffe)

    # Create the glue symbol
    glue_offset = sect.get_size()
    glue_sym = ElfSymbol(glue_sym_name, sect)
    glue_sym.type = STT_FUNC
    glue_sym.bind = sym.bind
    glue_sym.size = 4 * len(glue_code)
    glue_sym.value = sect.address + glue_offset
    glue_sym.section = sect
    glue_sym.shndx = sym.shndx
    elf.get_symbol_table().add_symbol(glue_sym)

    # Insert the glue code.
    for word in glue_code:
        sect.append_word(word)

    # Create a reloc for the branch instruction. The initial addend is -8,
    # which is the ARM-mode PC bias. + 4 is the offset of the b instruction.
    reloc_new(relocs, R_ARM_JUMP24, -8, glue_offset + 4, symdx)

def find_a2t_glue(elf, symtab, symdx):
    sym = elf.find_symbol(a2t_glue_sym_format % symtab.symbols[symdx].name)
    assert sym
    return sym.value

def make_a2t_glue(elf, sect, symtab, symdx, relocs):
    sym = symtab.symbols[symdx]
    glue_sym_name = a2t_glue_sym_format % sym.name

    # Look for existing glue for this symbol, and return it if it exists
    glue_sym = elf.find_symbol(glue_sym_name)
    if glue_sym:
        return glue_sym.value

    # The target must be an arm-mode function.
    if sym.type != STT_FUNC:
        raise Exception, \
                "Cannot make veneer for non-function symbol '%s'" % sym.name
    if not (sym.value & 1):
        raise Exception, \
                "Cannot make a2t veneer for arm function '%s'" % sym.name

    if elf.abi.have_blx:
        # The glue code is:
        # .arm
        #   ldr pc, [pc, #-4]
        # .word 1 (R_ARM_PC24 sym.name)
        glue_code = (0xe51ff004, 0x00000001)
        reloc_offset = 4
    else:
        # The glue code is:
        # .arm
        #   ldr r12, [pc]
        #   bx r12
        # .word 1 (R_ARM_PC24 sym.name)
        glue_code = (0xe59fc000, 0xe12fff1c, 0x00000001)
        reloc_offset = 8

    # Create the glue symbol
    glue_offset = sect.get_size()
    glue_sym = ElfSymbol(glue_sym_name, sect)
    glue_sym.type = STT_FUNC
    glue_sym.bind = sym.bind
    glue_sym.size = 4 * len(glue_code)
    glue_sym.value = sect.address + glue_offset
    glue_sym.section = sect
    glue_sym.shndx = sym.shndx
    elf.get_symbol_table().add_symbol(glue_sym)

    # Insert the glue code.
    for word in glue_code:
        sect.append_word(word)

    # Create a reloc for the function pointer. The initial addend is 1, which
    # is the Thumb-target bit for bx.
    reloc_new(relocs, R_ARM_ABS32, 1, glue_offset + reloc_offset, symdx)

def reloc_revert_arm_jump24(elf, sect, symtab, addend, offset, symdx):
    # Preserve the op, and set the offset to jump to itself.
    op = addend & 0xff000000
    return op | 0xfffffe

def reloc_alloc_arm_jump24(elf, sect, symtab, offset, symdx, relocs):
    # Determine whether the target is thumb code.
    sym_val = reloc_get_symbol_value(symtab, symdx)
    to_thumb = sym_val & 1

    # If this jump switches modes, it needs a veneer.
    if to_thumb:
        make_a2t_glue(elf, sect, symtab, symdx, relocs)

def reloc_calc_arm_jump24(elf, sect, symtab, addend, offset, symdx):
    # Get the origin and target addresses.
    sym_val = reloc_get_symbol_value(symtab, symdx)
    pos = reloc_get_position(sect, offset)

    # Determine whether the target is thumb code.
    to_thumb = sym_val & 1

    # Determine the real address to jump to.
    if to_thumb:
        # Locate the interworking veneer.
        target = find_a2t_glue(elf, symtab, symdx)
    else:
        # Clear the thumb bit to get the real address.
        target = sym_val & ~1

    if (target & 0xff000000 == 0):
        target += pos & 0xff000000

    bias = -8
    branch_range = ((1 << 23) - 1) << 2

    # S + A - P
    imm = target + bias - pos

    if abs(imm) > branch_range:
        raise NotImplementedError, \
                "Branch out of range (|%08x| > %08x)" % (imm, branch_range)

    op = sect.get_word_at(offset) & 0xff000000
    imm = ((imm & 0x2) << 23) | ((imm >> 2) & 0xffffff)

    return op | imm

def reloc_revert_thumb_jump24(elf, sect, symtab, addend, offset, symdx):
    op = ((addend & 0xffff0000) >> 16) | ((addend & 0xffff) << 16)
    op = op & 0xf0009000

    return ((op & 0xffff0000) >> 16) | ((op & 0xffff) << 16)

def reloc_alloc_thumb_jump24(elf, sect, symtab, offset, symdx, relocs):
    # Determine whether the target is thumb code.
    sym_val = reloc_get_symbol_value(symtab, symdx)
    to_thumb = sym_val & 1

    # If this jump switches modes, it needs a veneer.
    if not to_thumb:
        make_t2a_glue(elf, sect, symtab, symdx, relocs)

def reloc_calc_thumb_jump24(elf, sect, symtab, addend, offset, symdx):
    # Get the origin and target addresses.
    sym_val = reloc_get_symbol_value(symtab, symdx)
    pos = reloc_get_position(sect, offset)

    # Determine whether the target is thumb code.
    to_thumb = sym_val & 1

    # Determine the real address to jump to.
    if to_thumb:
        # Clear the thumb bit to get the real address.
        target = sym_val & ~1
    else:
        # Locate the interworking veneer.
        target = find_t2a_glue(elf, symtab, symdx)

    # This is a PC-relative relocation, so compensate for PC bias.
    bias = -4

    # S + A - P
    imm = target + bias - pos
    imm = imm & 0x01fffffe

    branch_range = ((1 << 23) - 1) << 2
    if abs(imm) > branch_range:
        raise NotImplementedError, \
            "Branch out of range (|%08x| > %08x)" * (imm, branch_range)

    op = sect.get_word_at(offset)
    # Clear out the old immediate value
    op = (((op & 0xffff0000) >> 16) | ((op & 0xffff) << 16)) & 0xf0009000

    # Separate the various parts of the immediate value and calculate the
    # J1 and J2 values for the new instruction.
    imm11 = (imm & 0xffe) >> 1
    imm10 = (imm & 0x3ff000) >> 12
    I2 = (imm & 0x400000) >> 22
    I1 = (imm & 0x800000) >> 23
    S = (imm & 0x1000000) >> 24
    if I1:
        # I1 = 1 means that NOT(J1 XOR S) = 1, so (J1 XOR S) = 0
        J1 = S
    else:
        # I1 = 0 means that NOT(J1 XOR S) = 1, so (J1 XOR S) = 1
        J1 = ~S & 0x1
    if I2:
        J2 = S
    else:
        J2 = ~S & 0x1

    # Construct the new instruction and return in little-endian form.
    op = op | imm11 | (J2 << 11) | (J1 << 13) | (imm10 << 16) | (S << 26)
    op = ((op & 0xffff0000) >> 16) | ((op & 0xffff) << 16)
    return op

def reloc_revert_thumb_jump11(elf, sect, symtab, addend, offset, symdx):
    op = sect.get_word_at(offset)
    # This is a 16-bit instruction so preserve the top 16 bits.
    return (op & 0xffffe000)

def reloc_alloc_thumb_jump11(elf, sect, symtab, offset, symdx, relocs):
    # Ensure the target is thumb code.
    sym_val = reloc_get_symbol_value(symtab, symdx)
    to_thumb = sym_val & 1

    if not to_thumb:
        raise NotImplementedError, \
                "Relocation cannot be implemented without a veneer, which" \
                " is not permitted for R_ARM_THM_JUMP11 relocations"

def reloc_calc_thumb_jump11(elf, sect, symtab, addend, offset, symdx):
    # Get the origin and target addresses.
    sym_val = reloc_get_symbol_value(symtab, symdx)
    pos = reloc_get_position(sect, offset)

    # Ensure the target is thumb code.
    to_thumb = sym_val & 1
    if to_thumb:
        # Clear the thumb bit to get the real address.
        target = sym_val & ~1
    else:
        raise NotImplementedError, \
                "Relocation cannot be implemented without a veneer, which" \
                " is not permitted for R_ARM_THM_JUMP11 relocations"

    # This is a PC-relative relocation, so compensate for PC bias.
    bias = -4

    # S + A - P
    imm = target + bias - pos
    imm = (imm & 0xffe) >> 1

    # Return the relocated instruction. This is a 16-bit instruction so
    # preserve the top 16 bits.
    op = sect.get_word_at(offset)
    return ((op & 0xffffe000) | imm)

def reloc_revert_arm_call(elf, sect, symtab, addend, offset, symdx):
    # Return a bl instruction that branches to itself.
    return 0xebfffffe

def reloc_revert_thumb_call(elf, sect, symtab, addend, offset, symdx):
    # Return a bl instruction that branches to itself.
    return 0xfffef7ff

def reloc_alloc_arm_call(elf, sect, symtab, offset, symdx, relocs):
    # Determine whether the target is thumb code.
    sym_val = reloc_get_symbol_value(symtab, symdx)
    to_thumb = sym_val & 1

    # If this call switches modes and can't use BLX, it needs a veneer.
    if to_thumb and (not elf.abi.have_blx):
        make_a2t_glue(elf, sect, symtab, symdx, relocs)

def reloc_alloc_thumb_call(elf, sect, symtab, offset, symdx, relocs):
    # Determine whether the target is thumb code.
    sym_val = reloc_get_symbol_value(symtab, symdx)
    to_thumb = sym_val & 1

    # If this call switches modes and can't use BLX, it needs a veneer.
    if (not to_thumb) and (not elf.abi.have_blx):
        make_t2a_glue(elf, sect, symtab, symdx, relocs)

def reloc_calc_call(elf, thumb, sect, symtab, addend, offset, symdx):
    # Get the origin and target addresses.
    sym_val = reloc_get_symbol_value(symtab, symdx)
    pos = reloc_get_position(sect, offset)

    # Determine whether the target is thumb code.
    to_thumb = sym_val & 1

    # Determine the real address to jump to.
    if (thumb != to_thumb) and (not elf.abi.have_blx):
        # Interworking with no BLX instruction allowed.
        if thumb:
            target = find_t2a_glue(elf, symtab, symdx)
        else:
            target = find_a2t_glue(elf, symtab, symdx)
        to_thumb = thumb
    else:
        # Clear the thumb bit to get the real address.
        target = sym_val & ~1

    # The addend (which is the PC bias) and the maximum range of the branch
    # depend on the current mode.
    if thumb:
        if to_thumb:
            bias = -4
        else:
            bias = (pos & 0x2) - 4
        if elf.abi.have_thumb2:
            branch_range = ((1 << 23) - 1) << 1
        else:
            branch_range = ((1 << 21) - 1) << 1
    else:
        bias = -8
        branch_range = ((1 << 23) - 1) << 2

    if (target & 0xff000000 == 0):
        target += pos & 0xff000000

    # S + A - P
    imm = target + bias - pos

    if abs(imm) > branch_range:
        raise NotImplementedError, \
                "Branch out of range (|%08x| > %08x)" % (imm, branch_range)

    if thumb:
        if elf.abi.have_thumb2:
            if to_thumb:
                op = 0xd000f000 # Thumb-2 BL
            else:
                op = 0xc000f000 # Thumb-2 BLX
            imm >>= 1
            s = (imm >> 23) & 1
            i1 = (imm >> 22) & 1
            i2 = (imm >> 21) & 1
            j1 = (~i1 ^ s) & 1
            j2 = (~i2 ^ s) & 1
            imm = ((imm << 16) & 0x07ff0000) | ((imm >> 11) & 0x3ff) | \
                    (s << 10) | (j1 << 29) | (j2 << 27)
        else:
            if to_thumb:
                op = 0xf800f000 # Thumb BL
            else:
                op = 0xe800f000 # Thumb BLX
            imm >>= 1
            imm = ((imm << 16) & 0x07ff0000) | ((imm >> 11) & 0x7ff)
    else:
        if to_thumb:
            op = 0xfa000000 # ARM BLX
        else:
            op = 0xeb000000 # ARM BL
        imm = ((imm & 0x2) << 23) | ((imm >> 2) & 0xffffff)

    return op | imm

def reloc_calc_arm_call(elf, sect, symtab, addend, offset, symdx):
    return reloc_calc_call(elf, False, sect, symtab, addend, offset, symdx)

def reloc_calc_thumb_call(elf, sect, symtab, addend, offset, symdx):
    return reloc_calc_call(elf, True, sect, symtab, addend, offset, symdx)

def reloc_calc_movw(elf, sect, symtab, addend, offset, symdx):
    # Extract the 16-bit immediate value from the opcode.
    opcode = addend
    imm = ((opcode & 0xf0000) >> 4) | (opcode & 0xfff)

    # The addend for the relocation calculation is the immediate value
    # sign-extended to 32 bits.
    if imm & 0x8000:
        addend = imm | 0xffff0000
    else:
        addend = imm

    # Perform the operation. The resulting value is the new immediate value.
    val = reloc_calc_S_add_A_or_T(elf, sect, symtab, addend, offset, symdx)
    val = val & 0xffff

    # Construct the new opcode.
    opcode = opcode & 0xfff0f000
    opcode = opcode | ((val & 0xf000) << 4) | (val & 0xfff)
    return opcode

def reloc_calc_thumb_movw(elf, sect, symtab, addend, offset, symdx):
    # Convert to big-endian representation of opcode for easier manipulation.
    opcode = ((addend & 0xffff) << 16) | ((addend & 0xffff0000) >> 16)

    # Extract the 16-bit immediate value from the opcode.
    imm = ((opcode & 0xf0000) >> 4) | ((opcode & 0x4000000) >> 15) | \
            ((opcode & 0x7000) >> 4) | (opcode & 0xff)

    # The addend for the relocation calculation is the immediate value
    # sign-extended to 32 bits.
    if imm & 0x8000:
        addend = imm | 0xffff0000
    else:
        addend = imm

    # Perform the operation. The resulting value is the new immediate value.
    val = reloc_calc_S_add_A_or_T(elf, sect, symtab, addend, offset, symdx)
    val = val & 0xffff

    # Construct the new opcode.
    opcode = opcode & 0xfbf08f00
    opcode = opcode | ((val & 0xf000) << 4) | \
            ((val & 0x0800) << 15) | ((val & 0x0700) << 4) | \
            (val & 0x00ff)

    # Return the opcode in little-endian form.
    return (((opcode & 0xffff) << 16) | ((opcode & 0xffff0000) >> 16))

def reloc_calc_movt(elf, sect, symtab, addend, offset, symdx):
    # Extract the 16-bit immediate value from the opcode.
    opcode = addend
    imm = ((opcode & 0xf0000) >> 4) | (opcode & 0xfff)

    # The addend for the relocation calculation is the immediate value
    # sign-extended to 32 bits.
    if imm & 0x8000:
        addend = imm | 0xffff0000
    else:
        addend = imm

    # Perform the operation. The resulting value is the new immediate value.
    val = reloc_calc_S_add_A_or_T(elf, sect, symtab, addend, offset, symdx)
    val = (val >> 16) & 0xffff

    # Construct the new opcode.
    opcode = opcode & 0xfff0f000
    opcode = opcode | ((val & 0xf000) << 4) | (val & 0xfff)
    return opcode

def reloc_calc_thumb_movt(elf, sect, symtab, addend, offset, symdx):
    # Convert to big-endian representation of opcode for easier manipulation.
    opcode = ((addend & 0xffff) << 16) | ((addend & 0xffff0000) >> 16)

    # Extract the 16-bit immediate value from the opcode.
    imm = ((opcode & 0xf0000) >> 4) | ((opcode & 0x4000000) >> 15) | \
            ((opcode & 0x7000) >> 4) | (opcode & 0xff)

    # The addend for the relocation calculation is the immediate value
    # sign-extended to 32 bits.
    if imm & 0x8000:
        addend = imm | 0xffff0000
    else:
        addend = imm

    # Perform the operation. The upper 16 bits of the resulting value form
    # the new immediate value.
    val = reloc_calc_S_add_A_or_T(elf, sect, symtab, addend, offset, symdx)
    val = (val >> 16) & 0xffff

    # Construct the new opcode.
    opcode = opcode & 0xfbf08f00
    opcode = opcode | ((val & 0xf000) << 4) | \
            ((val & 0x0800) << 15) | ((val & 0x0700) << 4) | \
            (val & 0x00ff)

    # Return the opcode in little-endian form.
    return (((opcode & 0xffff) << 16) | ((opcode & 0xffff0000) >> 16))

def reloc_revert_movw(elf, sect, symtab, addend, offset, symdx):
    # Extract the 16-bit immediate value from the opcode.
    opcode = addend
    imm = ((opcode & 0xf0000) >> 4) | (opcode & 0xfff)

    # Undo the relocation calculation to obtain the original
    # sign-extended immediate value.
    val = reloc_revert_S_add_A_or_T(elf, sect, symtab, imm, offset, symdx)
    old_addend = val & 0xffff

    # Construct the new opcode.
    opcode = opcode & 0xfff0f000
    opcode = opcode | ((old_addend & 0xf000) << 4) | (old_addend & 0xfff)
    return opcode

def reloc_revert_thumb_movw(elf, sect, symtab, addend, offset, symdx):
    # Convert to big-endian representation of opcode for easier manipulation.
    opcode = ((addend & 0xffff) << 16) | ((addend & 0xffff0000) >> 16)

    # Extract the 16-bit immediate value from the opcode.
    imm = ((opcode & 0xf0000) >> 4) | ((opcode & 0x4000000) >> 15) | \
            ((opcode & 0x7000) >> 4) | (opcode & 0xff)

    # Undo the relocation calculation to obtain the original
    # sign-extended immediate value.
    val = reloc_revert_S_add_A_or_T(elf, sect, symtab, imm, offset, symdx)
    old_addend = val & 0xffff

    # Construct the new opcode.
    opcode = opcode & 0xfbf08f00
    opcode = opcode | ((old_addend & 0xf000) << 4) | \
            ((old_addend & 0x0800) << 15) | ((old_addend & 0x0700) << 4) | \
            (old_addend & 0x00ff)

    # Return the opcode in little-endian form.
    return (((opcode & 0xffff) << 16) | ((opcode & 0xffff0000) >> 16))

def find_prev_movw(elf, sect, offset, rd, cond):
    # The opcode we are trying to match. Or-in the bits for the destination
    # register to ensure we match the correct movw.
    movw_opcode = 0x03000000 | rd | cond
    while offset >= 0:
        inst = sect.get_word_at(offset)
        if (inst & 0xfff0f000) == movw_opcode:
            return inst
        # Not a mowv instruction. Keep looking.
        offset = offset - 4
    return None

def reloc_revert_movt(elf, sect, symtab, addend, offset, symdx):
    # Extract the 16-bit immediate value from the opcode.
    opcode = addend
    imm = ((opcode & 0xf0000) >> 4) | (opcode & 0xfff)
    imm = (imm << 16) & 0xffffffff

    # Search for the preceding movw instruction, to determine the lower 16 bits
    # of the addend.
    rd = addend & 0xf000
    cond = addend & 0xf0000000
    movw_opcode = find_prev_movw(elf, sect, offset-4, rd, cond)
    assert(movw_opcode is not None)

    movw_imm = ((movw_opcode & 0xf0000) >> 4) | (movw_opcode & 0xfff)
    addend = imm | (movw_imm & 0xffff)

    # Undo the relocation calculation to obtain the original
    # sign-extended immediate value.
    val = reloc_revert_S_add_A_or_T(elf, sect, symtab, addend, offset, symdx)
    old_addend = val & 0xffff

    # Construct the new opcode.
    opcode = opcode & 0xfff0f000
    opcode = opcode | ((old_addend & 0xf000) << 4) | (old_addend & 0xfff)
    return opcode

def find_prev_movw_thumb(elf, sect, offset, rd):
    # The opcode we are trying to match. Or-in the bits for the destination
    # register to ensure we match the correct movw.
    movw_opcode = 0xf2000000 | rd
    while offset >= 0:
        inst = sect.get_word_at(offset)
        # Convert to big-endian for comparison.
        inst = ((inst & 0xffff) << 16) | ((inst & 0xffff0000) >> 16)
        # Ignore the bits containing the immediate value.
        if (inst & 0xfb808f00) == movw_opcode:
            return inst
        # Not a movw instruction. Keep looking.
        offset = offset - 2
    return None

def reloc_revert_thumb_movt(elf, sect, symtab, addend, offset, symdx):
    # Convert to big-endian representation of opcode for easier manipulation.
    opcode = ((addend & 0xffff) << 16) | ((addend & 0xffff0000) >> 16)

    # Extract the 16-bit immediate value from the opcode.
    imm = ((opcode & 0xf0000) >> 4) | ((opcode & 0x4000000) >> 15) | \
            ((opcode & 0x7000) >> 4) | (opcode & 0xff)
    imm = (imm << 16) & 0xffffffff

    # Search for the preceding movw instruction, to determine the lower 16 bits
    # of the addend.
    rd = opcode & 0xf00
    movw_opcode = find_prev_movw_thumb(elf, sect, offset-4, rd)
    assert(movw_opcode is not None)

    movw_imm = ((movw_opcode & 0xf0000) >> 4) | ((movw_opcode & 0x4000000) >> 15) | \
            ((movw_opcode & 0x7000) >> 4) | (movw_opcode & 0xff)
    addend = imm | (movw_imm & 0xffff)

    # Undo the relocation calculation to obtain the original
    # sign-extended immediate value.
    val = reloc_revert_S_add_A_or_T(elf, sect, symtab, addend, offset, symdx)
    old_addend = val & 0xffff

    # Construct the new opcode.
    opcode = opcode & 0xfbf08f00
    opcode = opcode | ((old_addend & 0xf000) << 4) | \
            ((old_addend & 0x0800) << 15) | ((old_addend & 0x0700) << 4) | \
            (old_addend & 0x00ff)

    # Return the opcode in little-endian form.
    return (((opcode & 0xffff) << 16) | ((opcode & 0xffff0000) >> 16))

def no_op_reloc(*args, **kwargs):
    return None

R_ARM_NONE = ArmRelocType(0, "NONE", reloc_calc_none, reloc_revert_none)
R_ARM_PC24 = ArmRelocType(1, "PC24", reloc_calc_arm_branch, reloc_revert_arm_branch)
R_ARM_ABS32 = ArmRelocType(2, "ABS32", reloc_calc_S_add_A_or_T, reloc_revert_S_add_A)
R_ARM_REL32 = ArmRelocType(3, "REL32", reloc_calc_S_add_A_or_T_sub_P, reloc_revert_S_add_A_sub_P)
R_ARM_LDR_PC_G0 = ArmRelocType(4, "LDR_PC_G0")
R_ARM_ABS16 = ArmRelocType(5, "ABS16")
R_ARM_ABS12 = ArmRelocType(6, "ABS12")
R_ARM_THM_ABS5 = ArmRelocType(7, "THM_ABS5")
R_ARM_ABS8 = ArmRelocType(8, "ABS8")
R_ARM_SBREL32 = ArmRelocType(9, "SBREL32")
R_ARM_THM_PC22 = ArmRelocType(10, "THM_CALL", reloc_calc_thumb_call, reloc_revert_thumb_call, reloc_alloc_thumb_call)
R_ARM_THM_PC8 = ArmRelocType(11, "THM_PC8")
R_ARM_BREL_ADJ = ArmRelocType(12, "BREL_ADJ")
R_ARM_TLS_DESC = ArmRelocType(13, "TLS_DESC")
R_ARM_THM_SWI8 = ArmRelocType(14, "THM_SWI8")
R_ARM_XPC25 = ArmRelocType(15, "XPC25")
R_ARM_THM_XPC22 = ArmRelocType(16, "THM_XPC22")
R_ARM_TLS_DTPMOD32 = ArmRelocType(17, "TLS_DTPMOD32")
R_ARM_TLS_DTPOFF32 = ArmRelocType(18, "TLS_DTPOFF32")
R_ARM_TLS_TPOFF32 = ArmRelocType(19, "TLS_TPOFF32")
R_ARM_COPY = ArmRelocType(20, "COPY")
R_ARM_GLOB_DAT = ArmRelocType(21, "GLOB_DAT")
R_ARM_JUMP_SLOT = ArmRelocType(22, "JUMP_SLOT")
R_ARM_RELATIVE = ArmRelocType(23, "RELATIVE")
R_ARM_GOTOFF32 = ArmRelocType(24, "GOTOFF32", reloc_calc_S_add_A_or_T_sub_GOT_ORG, reloc_revert_S_add_A_sub_GOT_ORG, reloc_alloc_got_symbol)
R_ARM_BASE_PREL = ArmRelocType(25, "BASE_PREL", reloc_calc_B_S_add_A_sub_P, reloc_revert_B_S_add_A_sub_P)

# XXX: Setting the reversion to 0 is not correct but figuring out what the
# original value should be given we have no way to know what the entries in
# the .got are is... problematic.  This seems to work and for the moment
# the .got entries we have are minimal and all have 0 addends but this may
# break at some point.
R_ARM_GOT_BREL = ArmRelocType(26, "GOT_BREL", reloc_calc_GOT_S_add_A_sub_GOT_ORG, reloc_revert_0, reloc_alloc_got_symbol)

R_ARM_PLT32 = ArmRelocType(27, "PLT32", reloc_calc_arm_branch, reloc_revert_arm_branch)
R_ARM_CALL = ArmRelocType(28, "CALL", reloc_calc_arm_call, reloc_revert_arm_call, reloc_alloc_arm_call)
R_ARM_JUMP24 = ArmRelocType(29, "JUMP24", reloc_calc_arm_jump24, reloc_revert_arm_jump24, reloc_alloc_arm_jump24)
R_ARM_THM_JUMP24 = ArmRelocType(30, "THM_JUMP24", reloc_calc_thumb_jump24, reloc_revert_thumb_jump24, reloc_alloc_thumb_jump24)
R_ARM_BASE_ABS = ArmRelocType(31, "BASE_ABS")
R_ARM_ALU_PCREL_7_0 = ArmRelocType(32, "ALU_PCREL_7_0")
R_ARM_ALU_PCREL_15_8 = ArmRelocType(33, "ALU_PCREL_15_8")
R_ARM_ALU_PCREL_23_15 = ArmRelocType(34, "ALU_PCREL_23_15")
R_ARM_LDR_SBREL_11_0_NC = ArmRelocType(35, "LDR_SBREL_11_0_NC")
R_ARM_ALU_SBREL_19_12_NC = ArmRelocType(36, "ALU_SBREL_19_12_NC")
R_ARM_ALU_SBREL_27_20_CK = ArmRelocType(37, "ALU_SBREL_27_20_CK")
R_ARM_TARGET1 = ArmRelocType(38, "TARGET1", reloc_calc_S_add_A_or_T, reloc_revert_S_add_A)
R_ARM_SBREL31 = ArmRelocType(39, "SBREL31")
# V4BX is used for converting v4t binaries to v4. We need not do anything.
R_ARM_V4BX = ArmRelocType(40, "V4BX", reloc_calc_none, reloc_calc_none)
R_ARM_TARGET2 = ArmRelocType(41, "TARGET2")
R_ARM_PREL31 = ArmRelocType(42, "PREL31", reloc_calc_arm_branch, reloc_revert_arm_branch)
R_ARM_MOVW_ABS_NC = ArmRelocType(43, "MOVW_ABS_NC", reloc_calc_movw, reloc_revert_movw)
R_ARM_MOVT_ABS = ArmRelocType(44, "MOVT_ABS", reloc_calc_movt, reloc_revert_movt)
R_ARM_MOVW_PREL_NC = ArmRelocType(45, "MOVW_PREL_NC")
R_ARM_MOVT_PREL = ArmRelocType(46, "MOVT_PREL")
R_ARM_THM_MOVW_ABS_NC = ArmRelocType(47, "THM_MOVW_ABS_NC", reloc_calc_thumb_movw, reloc_revert_thumb_movw)
R_ARM_THM_MOVT_ABS = ArmRelocType(48, "THM_MOVT_ABS", reloc_calc_thumb_movt, reloc_revert_thumb_movt)
R_ARM_THM_MOVW_PREL_NC = ArmRelocType(49, "THM_MOVW_PREL_NC")
R_ARM_THM_MOVT_PREL = ArmRelocType(50, "THM_MOVT_PREL")
R_ARM_THM_JUMP19 = ArmRelocType(51, "THM_JUMP19")
R_ARM_THM_JUMP6 = ArmRelocType(52, "THM_JUMP6")
R_ARM_THM_ALU_PREL_11_0 = ArmRelocType(53, "THM_ALU_PREL_11_0")
R_ARM_THM_PC12 = ArmRelocType(54, "THM_PC12")
R_ARM_ABS32_NOI = ArmRelocType(55, "ABS32_NOI")
R_ARM_REL32_NOI = ArmRelocType(56, "REL32_NOI")
R_ARM_ALU_PC_G0_NC = ArmRelocType(57, "ALU_PC_G0_NC")
R_ARM_ALU_PC_G0 = ArmRelocType(58, "ALU_PC_G0")
R_ARM_ALU_PC_G1_NC = ArmRelocType(59, "ALU_PC_G1_NC")
R_ARM_ALU_PC_G1 = ArmRelocType(60, "ALU_PC_G1")
R_ARM_ALU_PC_G2 = ArmRelocType(61, "ALU_PC_G2")
R_ARM_LDR_PC_G1 = ArmRelocType(62, "LDR_PC_G1")
R_ARM_LDR_PC_G2 = ArmRelocType(63, "LDR_PC_G2")
R_ARM_LDRS_PC_G0 = ArmRelocType(64, "LDRS_PC_G0")
R_ARM_LDRS_PC_G1 = ArmRelocType(65, "LDRS_PC_G1")
R_ARM_LDRS_PC_G2 = ArmRelocType(66, "LDRS_PC_G2")
R_ARM_LDC_PC_G0 = ArmRelocType(67, "LDC_PC_G0")
R_ARM_LDC_PC_G1 = ArmRelocType(68, "LDC_PC_G1")
R_ARM_LDC_PC_G2 = ArmRelocType(69, "LDC_PC_G2")
R_ARM_ALU_SB_G0_NC = ArmRelocType(70, "ALU_SB_G0_NC")
R_ARM_ALU_SB_G0 = ArmRelocType(71, "ALU_SB_G0")
R_ARM_ALU_SB_G1_NC = ArmRelocType(72, "ALU_SB_G1_NC")
R_ARM_ALU_SB_G1 = ArmRelocType(73, "ALU_SB_G1")
R_ARM_ALU_SB_G2 = ArmRelocType(74, "ALU_SB_G2")
R_ARM_LDR_SB_G0 = ArmRelocType(75, "LDR_SB_G0")
R_ARM_LDR_SB_G1 = ArmRelocType(76, "LDR_SB_G1")
R_ARM_LDR_SB_G2 = ArmRelocType(77, "LDR_SB_G2")
R_ARM_LDRS_SB_G0 = ArmRelocType(78, "LDRS_SB_G0")
R_ARM_LDRS_SB_G1 = ArmRelocType(79, "LDRS_SB_G1")
R_ARM_LDRS_SB_G2 = ArmRelocType(80, "LDRS_SB_G2")
R_ARM_LDC_SB_G0 = ArmRelocType(81, "LDC_SB_G0")
R_ARM_LDC_SB_G1 = ArmRelocType(82, "LDC_SB_G1")
R_ARM_LDC_SB_G2 = ArmRelocType(83, "LDC_SB_G2")
R_ARM_MOVW_BREL_NC = ArmRelocType(84, "MOVW_BREL_NC")
R_ARM_MOVT_BREL = ArmRelocType(85, "MOVT_BREL")
R_ARM_MOVW_BREL = ArmRelocType(86, "MOVW_BREL")
R_ARM_THM_MOVW_BREL_NC = ArmRelocType(87, "THM_MOVW_BREL_NC")
R_ARM_THM_MOVT_BREL = ArmRelocType(88, "THM_MOVT_BREL")
R_ARM_THM_MOVW_BREL = ArmRelocType(89, "THM_MOVW_BREL")
R_ARM_TLS_GOTDESC = ArmRelocType(90, "TLS_GOTDESC")
R_ARM_TLS_CALL = ArmRelocType(91, "TLS_CALL")
R_ARM_TLS_DESCSEQ = ArmRelocType(92, "TLS_DESCSEQ")
R_ARM_THM_TLS_CALL = ArmRelocType(93, "THM_TLS_CALL")
R_ARM_PLT32_ABS = ArmRelocType(94, "PLT32_ABS")
R_ARM_GOT_ABS = ArmRelocType(95, "GOT_ABS")
R_ARM_GOT_PREL = ArmRelocType(96, "GOT_PREL")
R_ARM_GOT_BREL12 = ArmRelocType(97, "GOT_BREL12")
R_ARM_GOTOFF12 = ArmRelocType(98, "GOTOFF12")
R_ARM_GOTRELAX = ArmRelocType(99, "GOTRELAX")
R_ARM_GNU_VTENTRY = ArmRelocType(100, "GNU_VTENTRYLS_GD32")
R_ARM_THM_PC11 = ArmRelocType(102, "THM_PC11", reloc_calc_thumb_jump11, reloc_revert_thumb_jump11, reloc_alloc_thumb_jump11)
R_ARM_TLS_LDM32 = ArmRelocType(105, "TLS_LDM32")
R_ARM_TLS_LDO32 = ArmRelocType(106, "TLS_LDO32")
R_ARM_TLS_IE32 = ArmRelocType(107, "TLS_IE32")
R_ARM_TLS_LE32 = ArmRelocType(108, "TLS_LE32")
R_ARM_TLS_LDO12 = ArmRelocType(109, "TLS_LDO12")
R_ARM_TLS_LE12 = ArmRelocType(110, "TLS_LE12")
R_ARM_TLS_IE12GP = ArmRelocType(111, "TLS_IE12GP")
R_ARM_PRIVATE_0 = ArmRelocType(112, "PRIVATE_0")
R_ARM_PRIVATE_1 = ArmRelocType(113, "PRIVATE_1")
R_ARM_PRIVATE_2 = ArmRelocType(114, "PRIVATE_2")
R_ARM_PRIVATE_3 = ArmRelocType(115, "PRIVATE_3")
R_ARM_PRIVATE_4 = ArmRelocType(116, "PRIVATE_4")
R_ARM_PRIVATE_5 = ArmRelocType(117, "PRIVATE_5")
R_ARM_PRIVATE_6 = ArmRelocType(118, "PRIVATE_6")
R_ARM_PRIVATE_7 = ArmRelocType(119, "PRIVATE_7")
R_ARM_PRIVATE_8 = ArmRelocType(120, "PRIVATE_8")
R_ARM_PRIVATE_9 = ArmRelocType(121, "PRIVATE_9")
R_ARM_PRIVATE_10 = ArmRelocType(122, "PRIVATE_10")
R_ARM_PRIVATE_11 = ArmRelocType(123, "PRIVATE_11")
R_ARM_PRIVATE_12 = ArmRelocType(124, "PRIVATE_12")
R_ARM_PRIVATE_13 = ArmRelocType(125, "PRIVATE_13")
R_ARM_PRIVATE_14 = ArmRelocType(126, "PRIVATE_14")
R_ARM_PRIVATE_15 = ArmRelocType(127, "PRIVATE_15")
R_ARM_ME_TOO = ArmRelocType(128, "ME_TOO")
R_ARM_THM_TLS_DESCSEQ16 = ArmRelocType(129, "THM_TLS_DESCSEQ16")
R_ARM_THM_TLS_DESCSEQ32 = ArmRelocType(130, "THM_TLS_DESCSEQ32")

# Non-standard variation of ABS32 used by ADS armlink -reloc. r_symdx contains
# a segment index in the file, but fixup_relocs() above rewrites it to the
# symbol index of a section symbol, so we can just handle it as an ABS32.
R_ARM_ADS_RELOC_ABS32 = ArmRelocType(253, "ADS_RELOC_ABS32", reloc_calc_S_add_A, reloc_revert_S_add_A)

# Non-standard entry placed at the start of the REL table by ADS armlink -reloc,
# always with an address of 0, probably to trigger an error in non-ADS linkers.
# We ignore it.
R_ARM_ADS_RELOC_GUARD = ArmRelocType(255, "ADS_RELOC_GUARD", reloc_calc_none, reloc_revert_none)

elf.abi.ABI_REGISTRATION[EM_ARM] = (ArmElfAbi, ArmRelocType)
