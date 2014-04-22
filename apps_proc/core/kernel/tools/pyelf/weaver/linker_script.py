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
Provide implementations of our linker scripts in a python format.
"""

from elf.abi.linker_commands import Command, Memory, Section, Merge, \
        command_reset, Segment

def perform_link(elf, sym_tab, command_func, product, cpu_desc, verbose):
    """
    Perform a link using the provided command function to generate the list
    of commands.
    """

    command_reset()
    Command.elf = elf
    Command.sym_tab = sym_tab
    Command.verbose = verbose

    commands = command_func(product, cpu_desc)

    for command in commands:
        command.parse()

    for sect in Command.sections:
        Command.elf.add_section(sect)

    return Command.segments, Command.merged_sects, Command.discarded_sects

def standard_link(section_vaddr, _):
    """
    Perform a standard link using a simple form of the command syntax.  Returns
    the segments to use as well as any merged or discarded sections.
    """

    text_addr = 0x08000000
    data_addr = None

    if section_vaddr:
        text_addr = section_vaddr.get(".text", text_addr)
        data_addr = section_vaddr.get(".data", data_addr)

    commands = [Memory(text_addr),
                Segment(),
                Section('.text', [Merge(['.text', '.text.*'])]),
                Section('.rodata', [Merge(['.rodata', '.rodata.*'])])]

    if data_addr:
        commands.extend([Memory(data_addr), Segment()])

    commands.extend([Section('.data', [Merge(['.data', '.data.*'])]),
                     Section('.got', [Merge(['.got'])]),
                     Section('.got.plt', [Merge(['.got.plt'])]),
                     Section('.bss', [Merge(['.bss'])])])

    return commands

def zonetests_link(_):
    """
    Perform a zonetest link as per the iguana example zonetests linker script.
    """

    return [Memory(0x80500000),
            Segment(),
            Section('.text', [Merge(['.text'])]),
            Section('.rodata', [Merge(['.rodata', '.rodata.*'])]),
            Segment(0x100000),
            Section('.data', [Merge(['.data'])]),
            Section('.got', [Merge(['.got'])]),
            Section('.got.plt', [Merge(['.got.plt'])]),
            Section('.bss', [Merge(['.bss'])]),
            Segment(0x100000),
            Section('.zone', [Merge(['.zone'])])]
