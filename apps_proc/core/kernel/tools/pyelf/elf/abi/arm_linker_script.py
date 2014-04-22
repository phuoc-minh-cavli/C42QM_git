##############################################################################
# Australian Public Licence B (OZPLB)
# 
# Version 1-0
# 
# Copyright (c) 2006-2012, Open Kernel Labs, Inc.
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

import elf.abi
from elf.constants import PT_NOTE
from elf.abi.linker_commands import Command, Memory, Section, Merge, Align, \
        Symbol, Discard, Segment, Assert, Pad

from pyokl4.autogen import Autogen

# Kernel segments must be aligned on 0x8000.
SEGMENT_ALIGN = 0x8000

def armv4_gnu_kernel_soc_link(product, cpu_desc):
    """
    Perform a kernel + soc link for armv4 based on the linker-gnu.lds in
    arch/armv4/kernel/kernel_standard.
    """

    # Note that in this linker script, we do not care about load addresses or
    # segments, which means many of the variables from the normal linker
    # script can be omitted.

    def sym(s):
        '''Lookup the value of a symbol at link-time.'''
        if s == '.':
            return Command.virt_addr
        else:
            return Command.sym_tab.get_symbol(s).value

    PHYS_START = 0
    script = [Memory(lambda: sym('_phys_start')),
              Segment(),
              Symbol('_start_kernel'),
              Symbol('_vectors_start'),
              Section('.vectors', [Merge(['.vectors']), Merge(['.vectors.*'])],
                      base_sect='.vectors'),
              Section('.init', [Merge(['.init.head']), Merge(['.init']),
                                Merge(['.init.*'])],
                      base_sect='.init.head'),
              Symbol('_start_kernel'),
              Section('.stack', [Merge(['.stack'])]),
              Section('.global', [Symbol('_start_global'), Merge(['.global'])]),
              Assert(lambda: sym('.') - PHYS_START < 0x1000,
                  lambda:
                  "PANIC: The global offset is larger than 0x1000, this is " + \
                  "not acceptable for ARM, please try to reduce it! " + \
                  "(%#x - %#x < 0x1000)" % (sym('.'), PHYS_START)),
              Section('.text', [Merge(['.text']), Merge(['.text.*'])],
                      align=8),
              Symbol('_start_data'),
              Section('.data', [Merge(['.pgtable']),
                                Merge(['.sdata', '.scommon', '.sbss']),
                                Merge(['.data']),
                                Merge(['.rodata']),
                                Merge(['.rodata.*']),
                                Merge(['.constdata']),
                                Merge(['.glue*'])],
                      align=8),
              Section('.bss', [Merge(['.bss']), Merge(['COMMON'])],
                      align=8),
              Section('.kdb', [Merge(['.kdb.text']),
                               Merge(['.kdb.data'])],
                      align=8, base_sect='.kdb.data'),
              Section('.tracebuffer', [Merge(['.tracebuffer.text']),
                                       Merge(['.tracebuffer.data'])],
                      align=8, base_sect='.tracebuffer.data'),
              Symbol('_end'),
            ]

    script.extend([
        Align(32),
        Memory(0),
        Segment(segment_type = PT_NOTE),
        Section('.elfweaver_info', [Merge(['.elfweaver_info'])])
    ])

    script.extend([Discard(['.gnu.linkonce.wi.*'])])

    return script

def armv5_gnu_kernel_soc_link(product, cpu_desc):
    """
    Perform a kernel + soc link for armv5 based on the linker.lds in
    arch/armv5/kernel/kernel_standard.
    """

    # Note that in this linker script, we do not care about load addresses or
    # segments, which means many of the variables from the normal linker
    # script can be omitted.

    def sym(s):
        '''Lookup the value of a symbol at link-time.'''
        if s == '.':
            return Command.virt_addr
        else:
            return Command.sym_tab.get_symbol(s).value

    def ROUND_TO_NEXT_PAGE(x):
        return ((x + 4095) >> 12) << 12

    def NEXT_PAGE(x):
        return ((x + 4096) >> 12) << 12

    KERNEL_VADDR_BASE = Autogen.machine_info.KERNEL_VADDR_BASE
    KERNEL_STACK_OFFSET = Autogen.machine_info.KERNEL_STACK_OFFSET
    KERNEL_STACK_SIZE = Autogen.machine_info.KERNEL_STACK_SIZE
    VADDR_GLOBAL = Autogen.machine_info.VADDR_GLOBAL
    script = [Memory(KERNEL_VADDR_BASE),
              Symbol('_start_kernel'),
              Segment(),
              Section('.init', [Merge(['.init.head']), Merge(['.init']),
                                Merge(['.init.*'])],
                        base_sect='.init.head',
                        align=32),
              Align(128),
              Pad(0x10),
              Assert(lambda: sym('.') == KERNEL_VADDR_BASE + KERNEL_STACK_OFFSET,
                  lambda:
                  "The size of .init segment has increased, please change " + \
                  "STACK_OFFSET to fit the new size and make _start_global " + \
                  "to be in the middle of the cache line. (%#x != %#x + %#x)" % (
                      sym('.'), KERNEL_VADDR_BASE, KERNEL_STACK_OFFSET)),
              Section('.stack', [Merge(['.stack']), Symbol('_start_global'), Merge(['.global'])]),
              Assert(lambda: sym('.') - KERNEL_VADDR_BASE < 0x1000,
                  lambda:
                  "PANIC: The global offset is larger than 0x1000, this is " + \
                  "not acceptable for ARM, please try to reduce it! " + \
                  "(%#x - %#x < 0x1000)" % (sym('.'), KERNEL_VADDR_BASE)),
              Assert(lambda: sym('_start_global') == \
                  KERNEL_VADDR_BASE + KERNEL_STACK_OFFSET + KERNEL_STACK_SIZE,
                  lambda: "Kernel stack has changed, " + \
                  "please update KERNEL_STACK_SIZE as well. " + \
                  "(%#x != %#x + %#x + %#x)" % (sym('_start_global'),
                  KERNEL_VADDR_BASE, KERNEL_STACK_OFFSET, KERNEL_STACK_SIZE)),
              Section('.text', [Merge(['.text']), Merge(['.text.*'])], 32),
              Section('.irqtext', [Merge(['.irqtext']), Merge(['.irqtext*'])],
                    32),
              Symbol('_start_data'),
              Section('.data', [Merge(['.pgtable']),
                                Merge(['.sdata', '.scommon', '.sbss']),
                                Merge(['.data']),
                                Merge(['.rodata']),
                                Merge(['.rodata.*']),
                                Merge(['.constdata']),
                                Merge(['.glue*'])],
                      32),
              Section('.kdb', [Merge(['.kdb.text']),
                               Merge(['.kdb.data'])], 32,
                      '.kdb.data'),
              Section('.tracebuffer', [Merge(['.tracebuffer.text']),
                                       Merge(['.tracebuffer.data'])], 32,
                      '.tracebuffer.data'),
              Section('.bss', [Merge(['.bss']),
                               Merge(['COMMON'])], 32),
              Symbol('_end'),
            ]

    #define DEFINE_VECTORS_PAGE
    def DEFINE_VECTORS_PAGE():
        return [
            Memory(0xffff0000),
            Segment(),
            Symbol('_vectors_start'),
            Section('.vectors', [Merge(['.vectors']),
                                 Merge(['.vectors.*'])
                                 ]),
            Memory(0xffff0f00),
            Section('.oklinux_helpers', [Merge(['.oklinux_helpers'])]),
            ]

    #define DEFINE_USER_HELPERS_PAGE(__vaddr, __laddr, __segment)
    def DEFINE_USER_HELPERS_PAGE(vaddr, use_segment):
        # load addresses and segments are not applicable here any more
        tail = [ Symbol('_helpers_start'),
                Section('.user_helpers', [
                                Merge(['.user_helpers.1']),
                                Merge(['.user_helpers.2']),
                                Merge(['.user_helpers*']),
                        ],
                        base_sect='.user_helpers.2'),
                Memory(lambda: sym('_helpers_start') + 0x200),
                Symbol('_helpers_tls'), ]

        if use_segment:
            return [Memory(vaddr), Segment()] + tail
        else:
            return [Memory(vaddr)] + tail


    if Autogen.scheduler.has_tasklock():
        #define DEFINE_TASKLOCK_PAGE
        def DEFINE_TASKLOCK_PAGE():
            return [
                Memory(0xffffe000),
                Segment(),
                Symbol('_shared_start'),
                Section('.shared', [Merge(['.shared'])]),
                ]
    else:
        def DEFINE_TASKLOCK_PAGE():
            return []

    # if defined(OKL4_MODULE_VECTOR_PAGE_FAULTS)
    if 'vector_page_faults' in Autogen.modules:
        script.extend(DEFINE_VECTORS_PAGE())
        script.extend(DEFINE_TASKLOCK_PAGE())
        script.extend(DEFINE_USER_HELPERS_PAGE(lambda: 0xfffff000,
                use_segment=True))
    else:
        script.extend(DEFINE_VECTORS_PAGE())
        script.extend(DEFINE_USER_HELPERS_PAGE(
            lambda: sym('_vectors_start') + 0x800, use_segment=False))
        script.extend(DEFINE_TASKLOCK_PAGE())

    script.extend([
        Align(32),
        Memory(0),
        Segment(segment_type = PT_NOTE),
        Section('.elfweaver_info', [Merge(['.elfweaver_info'])])
    ])

    script.extend([Discard(['.gnu.linkonce.wi.*'])])

    return script

def armv6_gnu_kernel_soc_link(product, cpu_desc):
    """
    Perform a kernel + soc link for armv6 based on the linker.lds in
    arch/armv6/kernel/kernel_standard.
    """

    def sym(s):
        '''Lookup the value of a symbol at link-time.'''
        if s == '.':
            return Command.virt_addr
        else:
            return Command.sym_tab.get_symbol(s).value

    def ROUND_TO_NEXT_PAGE(x):
        return ((x + 4095) >> 12) << 12

    def NEXT_PAGE(x):
        return ((x + 4096) >> 12) << 12

    KERNEL_VADDR_BASE = Autogen.machine_info.KERNEL_VADDR_BASE
    KERNEL_STACK_OFFSET = Autogen.machine_info.KERNEL_STACK_OFFSET
    KERNEL_STACK_SIZE = Autogen.machine_info.KERNEL_STACK_SIZE
    VADDR_GLOBAL = Autogen.machine_info.VADDR_GLOBAL

    if 'kernel_smp' in Autogen.modules:
        INIT_PAD = 0
    else:
        INIT_PAD = 0x10

    script = [Memory(KERNEL_VADDR_BASE),
              Symbol('_start_kernel'),
              Segment(),
              Section('.init', [Merge(['.init.head']), Merge(['.init']),
                                Merge(['.init.*'])],
                        base_sect='.init.head',
                        align=32),
              Align(128),
              Pad(INIT_PAD),
              Assert(lambda: sym('.') == KERNEL_VADDR_BASE + KERNEL_STACK_OFFSET,
                  lambda:
                  "The size of .init segment has increased, please change " + \
                  "STACK_OFFSET to fit the new size and make _start_global " + \
                  "to be in the middle of the cache line. (%#x != %#x + %#x)" % (
                      sym('.'), KERNEL_VADDR_BASE, KERNEL_STACK_OFFSET)),
              Section('.stack', [Merge(['.stack']), Symbol('_start_global'), Merge(['.global'])]),
              Assert(lambda: sym('.') - KERNEL_VADDR_BASE < 0x1000,
                  lambda:
                  "PANIC: The global offset is larger than 0x1000, this is " + \
                  "not acceptable for ARM, please try to reduce it! " + \
                  "(%#x - %#x < 0x1000)" % (sym('.'), KERNEL_VADDR_BASE)),
              Assert(lambda: sym('_start_global') == \
                  KERNEL_VADDR_BASE + KERNEL_STACK_OFFSET + KERNEL_STACK_SIZE,
                  lambda: "Kernel stack has changed, " + \
                  "please update KERNEL_STACK_SIZE as well. " + \
                  "(%#x != %#x + %#x + %#x)" % (sym('_start_global'),
                  KERNEL_VADDR_BASE, KERNEL_STACK_OFFSET, KERNEL_STACK_SIZE)),
              Section('.text', [Merge(['.text']), Merge(['.text.*'])], 32),
              Section('.irqtext', [Merge(['.irqtext']), Merge(['.irqtext*'])],
                    32),
              Symbol('_start_data'),
              Section('.data', [Merge(['.pgtable']),
                                Merge(['.sdata', '.scommon', '.sbss']),
                                Merge(['.data']),
                                Merge(['.rodata']),
                                Merge(['.rodata.*']),
                                Merge(['.constdata']),
                                Merge(['.glue*'])],
                      32),
              Section('.kdb', [Merge(['.kdb.text']),
                               Merge(['.kdb.data'])], 32,
                      '.kdb.data'),
              Section('.tracebuffer', [Merge(['.tracebuffer.text']),
                                       Merge(['.tracebuffer.data'])], 32,
                      '.tracebuffer.data'),
              Section('.bss', [Merge(['.bss']),
                               Merge(['COMMON'])], 32),
              Symbol('_end'),
            ]

    #define DEFINE_VECTORS_PAGE
    def DEFINE_VECTORS_PAGE():
        return [
            Memory(0xffff0000),
            Segment(),
            Symbol('_vectors_start'),
            Section('.vectors', [Merge(['.vectors']),
                                 Merge(['.vectors.*'])
                                 ]),
            ]

    # if defined(CPU_IS_ARM1136JS) && (ARM1136_REVISION = 0)
    #define DEFINE_USER_HELPERS_PAGE(__vaddr, __laddr, __segment)
    def DEFINE_USER_HELPERS_PAGE(vaddr, use_segment):
        # load addresses and segments are not applicable here any more
        tail = [
            Symbol('_helpers_start'),
            Section('.user_helpers', [
                            Merge(['.user_helpers.1']),
                            Merge(['.user_helpers.2']),
                            Merge(['.user_helpers*']),
                    ],
                    base_sect='.user_helpers.1'),
            Memory(lambda: vaddr() + 0x200),
            Section('.user_tls', [Merge(['.user_tls'])]),
                ]
        if use_segment:
            return [Memory(vaddr), Segment()] + tail
        else:
            return [Memory(vaddr)] + tail


    if Autogen.scheduler.has_tasklock():
        #define DEFINE_TASKLOCK_PAGE
        def DEFINE_TASKLOCK_PAGE():
            return [
                Memory(0xffffe000),
                Segment(),
                Symbol('_shared_start'),
                Section('.shared', [Merge(['.shared'])]),
                ]
    else:
        def DEFINE_TASKLOCK_PAGE():
            return []

    # if defined(OKL4_MODULE_VECTOR_PAGE_FAULTS)
    if 'vector_page_faults' in Autogen.modules:
        script.extend(DEFINE_VECTORS_PAGE())
        script.extend(DEFINE_TASKLOCK_PAGE())
        script.extend(DEFINE_USER_HELPERS_PAGE(lambda: 0xfffff000,
                use_segment=True))
    else:
        script.extend(DEFINE_VECTORS_PAGE())
        script.extend(DEFINE_USER_HELPERS_PAGE(
            lambda: sym('_vectors_start') + 0x800, use_segment=False))
        script.extend(DEFINE_TASKLOCK_PAGE())

    script.extend([
        Align(32),
        Memory(0),
        Segment(segment_type = PT_NOTE),
        Section('.elfweaver_info', [Merge(['.elfweaver_info'])])
    ])

    script.extend([Discard(['.gnu.linkonce.wi.*'])])

    return script


def armv7_gnu_kernel_soc_link(product, cpu_desc):
    """
    Perform a kernel + soc link for armv7 based on the linker.lds in
    arch/armv7/kernel/kernel_standard.
    """

    def sym(s):
        '''Lookup the value of a symbol at link-time.'''
        if s == '.':
            return Command.virt_addr
        else:
            return Command.sym_tab.get_symbol(s).value

    def ROUND_TO_NEXT_PAGE(x):
        return ((x + 4095) >> 12) << 12

    def NEXT_PAGE(x):
        return ((x + 4096) >> 12) << 12

    if ('kernel_smp' in Autogen.modules) or ('cortex-a9mp' in cpu_desc):
        INIT_PAD = 0
    else:
        INIT_PAD = 0x10

    KERNEL_VADDR_BASE = Autogen.machine_info.KERNEL_VADDR_BASE
    KERNEL_STACK_OFFSET = Autogen.machine_info.KERNEL_STACK_OFFSET
    KERNEL_STACK_SIZE = Autogen.machine_info.KERNEL_STACK_SIZE
    VADDR_GLOBAL = Autogen.machine_info.VADDR_GLOBAL
    MAX_NUM_CPUS = Autogen.machine_info.NUM_CPUS

    if 'kernel_smp' not in Autogen.modules:
        GLOBAL_OFFSET_ASSERT = Assert(lambda: sym('.') - KERNEL_VADDR_BASE < 0x1000,
            lambda:
            "PANIC: The global offset is larger than 0x1000, this is " + \
            "not acceptable for ARM, please try to reduce it! " + \
            "(%#x - %#x < 0x1000)" % (sym('.'), KERNEL_VADDR_BASE))
        # FIXME: Jira ticket OLD-1336 - sangeeta.
        MAX_NUM_CPUS = 1
    else:
        GLOBAL_OFFSET_ASSERT = Assert(lambda: 1 == 1,
            lambda:
            "")

    script = [Memory(KERNEL_VADDR_BASE),
              Symbol('_start_kernel'),
              Segment(),
              Section('.init', [Merge(['.init.head']), Merge(['.init']),
                                Merge(['.init.*'])],
                        base_sect='.init.head',
                        align=32),
              Align(0x100),
              Pad(INIT_PAD),
              Assert(lambda: sym('.') == KERNEL_VADDR_BASE + KERNEL_STACK_OFFSET,
                  lambda:
                  "The size of .init segment has increased, please change " + \
                  "STACK_OFFSET to fit the new size and make _start_global " + \
                  "to be in the middle of the cache line. (%#x != %#x + %#x)" % (
                      sym('.'), KERNEL_VADDR_BASE, KERNEL_STACK_OFFSET)),
              Section('.stack', [Merge(['.stack']), Symbol('_start_global'), Merge(['.global'])]),
              GLOBAL_OFFSET_ASSERT,
              Assert(lambda: sym('_start_global') == \
                  KERNEL_VADDR_BASE + KERNEL_STACK_OFFSET + (KERNEL_STACK_SIZE * MAX_NUM_CPUS), \
                  lambda: "Kernel stack has changed, " + \
                  "please update KERNEL_STACK_SIZE as well. " + \
                  "(%#x != %#x + %#x + (%#x * %#x))" % (sym('_start_global'),
                  KERNEL_VADDR_BASE, KERNEL_STACK_OFFSET, KERNEL_STACK_SIZE, MAX_NUM_CPUS)),
              Section('.text', [Merge(['.vectors']), Merge(['.vectors.*']),
                                Merge(['.text']), Merge(['.text.*']), Merge(['.glue*']),
                                Merge(['.kdb.text'])], 0x1000),
              Section('.irqtext', [Merge(['.irqtext']), Merge(['.irqtext*'])],
                    32),
              Symbol('_start_data'),
              Section('.rodata', [Merge(['.rodata']),
                                  Merge(['.rodata.*']),
                                  Merge(['.constdata'])],
                      0x1000),
              Section('.data', [Merge(['.pgtable']),
                                Merge(['.sdata', '.scommon', '.sbss']),
                                Merge(['.data']),
                                Merge(['.data.soc']),
                                Merge(['.kdb.data'])],
                      0x1000),
              Section('.bss', [Merge(['.bss']),
                               Merge(['COMMON'])], 32),
              Symbol('_end'),
            ]

    #define DEFINE_USER_HELPERS_PAGE(__segment)
    def DEFINE_USER_HELPERS_PAGE(vaddr, use_segment):
        # load addresses and segments are not applicable here any more
        tail = [
            Memory(vaddr()),
            Symbol('_helpers_start'),
            Section('.user_helpers', [
                            Merge(['.user_helpers.1']),
                            Merge(['.user_helpers.2']),
                            Merge(['.user_helpers*']),
                    ],
                    base_sect='.user_helpers.2'),
            Memory(lambda: vaddr() + 0xf00),
            Section('.oklinux_helpers', [Merge(['.oklinux_helpers'])]),
                ]
        if use_segment:
            return [Memory(vaddr), Segment()] + tail
        else:
            return [Memory(vaddr)] + tail


    if Autogen.scheduler.has_tasklock():
        #define DEFINE_TASKLOCK_PAGE
        def DEFINE_TASKLOCK_PAGE():
            return [
                Memory(0xffffe000),
                Segment(),
                Symbol('_shared_start'),
                Section('.shared', [Merge(['.shared'])]),
                Memory(0xffffe200),
                Section('.user_tls', [Merge(['.user_tls'])]),
                ]
    else:
        def DEFINE_TASKLOCK_PAGE():
            return [
                Memory(0xffffe000),
                Segment(),
                Symbol('_shared_start'),
                Memory(0xffffe200),
                Section('.user_tls', [Merge(['.user_tls'])]),
            ]

    if 'vcpu' in Autogen.modules:
        script.extend(DEFINE_USER_HELPERS_PAGE(lambda: 0xffff0000,
                use_segment=True))
    else:
        script.extend(DEFINE_USER_HELPERS_PAGE(lambda: 0xfffff000,
                use_segment=True))
    script.extend(DEFINE_TASKLOCK_PAGE())

    script.extend([
        Align(32),
        Memory(0),
        Segment(segment_type = PT_NOTE),
        Section('.elfweaver_info', [Merge(['.elfweaver_info'])])
    ])

    script.extend([Discard(['.gnu.linkonce.wi.*'])])

    return script


elf.abi.KERNEL_SOC_LINKER_SCRIPT['armv4'] = {'gnu' : armv4_gnu_kernel_soc_link}
elf.abi.KERNEL_SOC_LINKER_SCRIPT['armv5'] = {'gnu' : armv5_gnu_kernel_soc_link}
elf.abi.KERNEL_SOC_LINKER_SCRIPT['armv6'] = {'gnu' : armv6_gnu_kernel_soc_link}
elf.abi.KERNEL_SOC_LINKER_SCRIPT['armv7'] = {'gnu' : armv7_gnu_kernel_soc_link}
