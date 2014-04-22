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
Based on arch/armv7/kernel/mmu-dynamic/src/map_page.c
"""

from pyokl4.okl4.mmu_common.access import \
        ACCESS_RWX, ACCESS_RW, ACCESS_R, ACCESS_RX, ACCESS_NONE, \
        ACCESS_KDEVICE, ACCESS_KRWX_URX, ACCESS_KRX_UNONE, \
        ACCESS_KRW_UNONE, ACCESS_KRW_UR, ACCESS_KRW_URW, ACCESS_KRX_URX, \
        ACCESS_KR_UNONE
from pyokl4.okl4.mmu_common.mmu_attrib import mmu_get_cpu_attrib
from pyokl4.arch.armv7.okl4.mmu_common.internal import \
        ARM_ACCESS_KRW_URW, ARM_ACCESS_KRW_UR, \
        ARM_ACCESS_KRW_UNONE, ARM_ACCESS_KRX_UNONE, \
        ARM_ACCESS_KRWX_URWX, ARM_ACCESS_KRW_URW, \
        ARM_ACCESS_KRX_URX, \
        ARM_ACCESS_KRWX_URX, \
        ARM_ACCESS_KRX_URX, \
        ARM_ACCESS_KR_UR, \
        ARM_ACCESS_KRWX_UNONE, \
        ARM_ACCESS_KRW_UNONE,\
        ARM_ACCESS_KR_UNONE, \
        SIZE_16M, SIZE_1M, SIZE_64K, SIZE_4K, \
        ARM_16M_BITS, ARM_1M_BITS, ARM_64K_BITS, ARM_4K_BITS, \
        ARM_SUBTREE_ENTRIES, ARM_SUBTREE_SIZE, ARM_L1_TYPE_CTABLE, \
        ARM_L2PageTableEntry, arm_l1_index, arm_l2_index, \
        ARM_TOPTREE_ENTRIES
from pyokl4.autogen import Autogen
import pyokl4.arch.armv7.okl4.mmu_common.internal as armv7

# This is for keeping shared subtrees for kernel.
l2_subtrees_kernel = {}

# This is for keeping the page table which does kernel l2 mapping.
l2_subtrees_pgtable = {}


class MapPageException(Exception):
    """Exception to be raised if we have a problem when mapping a page"""

def upgrade_perms(rwx):
    return rwx

def mmu_get_arch_rwx(rwx):
    """Convert generic access rights to ARM specific ones."""
    rwx_map = {
        ACCESS_RWX:     ARM_ACCESS_KRWX_URWX,
        ACCESS_RW:      ARM_ACCESS_KRW_URW,
        ACCESS_R:       ARM_ACCESS_KR_UR,
        ACCESS_RX:      ARM_ACCESS_KRX_URX,
        ACCESS_NONE:    ARM_ACCESS_KRWX_UNONE,
        ACCESS_KDEVICE: ARM_ACCESS_KRW_UNONE,
        ACCESS_KRWX_URX: ARM_ACCESS_KRWX_URX,
        ACCESS_KRX_UNONE: ARM_ACCESS_KRX_UNONE,
        ACCESS_KRW_UNONE: ARM_ACCESS_KRW_UNONE,
        ACCESS_KRW_UR:    ARM_ACCESS_KRW_UR,
        ACCESS_KRW_URW:   ARM_ACCESS_KRW_URW,
        ACCESS_KRX_URX: ARM_ACCESS_KRX_URX,
        ACCESS_KR_UNONE: ARM_ACCESS_KR_UNONE
        }

    return rwx_map.get(rwx, 0xFFFFFFFFL)

def mmu_virt_to_l1_entry(pgtable, virt):
    entry_index = arm_l1_index(virt)
    entry = pgtable[entry_index]

    if entry.is_valid():
        return entry.to_word()
    else:
        return 0

def mmu_map_page(pgtable, domain, virt, phys, size, rwx, generic_attr,
                 not_global = True, overwrite=False, kernel_pgtable = False,
                 dont_filter_physreloc=False):
    """Do the work to map a page."""

    def raise_error(virt, phys):
        """Raise an error if a mapping can't be made."""
        if overwrite is False:
            raise MapPageException('Cannot map virt %#8.8x to phys %#8.8x,'
                                   ' existing mapping found' % (virt, phys))

    access = mmu_get_arch_rwx(rwx)

    attributes = mmu_get_cpu_attrib(generic_attr)

    if kernel_pgtable:
        entry_index = arm_l1_index(virt) - \
                (ARM_TOPTREE_ENTRIES - Autogen.machine_info.KERNEL_L1_ENTRIES)
    else:
        entry_index = arm_l1_index(virt)
    # print "entry_index = %s, %lx" % (entry_index, virt)

    # Map a supersection
    if size == SIZE_16M:
        # Handle duplicate entries for 16M
        for i in range(0, 1 << (ARM_16M_BITS - ARM_1M_BITS)):
            entry = pgtable[entry_index + i]
            if entry.is_valid():
                raise_error(virt, phys)

            # print "set 16m page"
            entry.set_16mpage(phys, access, attributes, not_global,
                              dont_filter_physreloc)

    # Map a section
    elif size == SIZE_1M:
        entry = pgtable[entry_index]

        if entry.is_valid():
            raise_error(virt, phys)

        entry.set_1mpage(phys, 0, access, attributes, not_global,
                         dont_filter_physreloc)
    else:
        entry = pgtable[entry_index]

        if not entry.is_valid(): # subtree does not exist, create it
            # If it is in kernel space, l2 subtrees can be shared.
            if virt >= Autogen.machine_info.KERNEL_VADDR_BASE and \
                       entry_index in l2_subtrees_kernel:
                subtree_l2 = l2_subtrees_kernel[entry_index]
                entry.set_subtree(0, subtree_l2)
                return
            else:
                subtree_l2 = Autogen.objmanager.objmanager_allocate(
                        ARM_L2PageTableEntry,
                                                           domain,
                                                           ARM_SUBTREE_ENTRIES,
                        ARM_SUBTREE_SIZE)

                entry.set_subtree(0, subtree_l2)

                # If it is kernel space, store the created subtree.
                if virt >= Autogen.machine_info.KERNEL_VADDR_BASE:
                    l2_subtrees_kernel[entry_index] = subtree_l2

            # If it is kernel space, remember the page table which does
            # this subtree's mapping.
            if virt >= Autogen.machine_info.KERNEL_VADDR_BASE:
                l2_subtrees_pgtable[entry_index] = pgtable


        if entry.type != ARM_L1_TYPE_CTABLE: # is not a subtree
            raise_error(virt, phys)

        # If it is kernel space and it has been mapped by a different
        # page table, there is no need to do it again since kernel mapping
        # is shared.
        if virt >= Autogen.machine_info.KERNEL_VADDR_BASE and \
                   entry_index in l2_subtrees_pgtable and \
                   l2_subtrees_pgtable[entry_index] != pgtable:
            return

        subtree = entry.subtree
        entry_index = arm_l2_index(virt)

        # Map a large page
        if size == SIZE_64K:
            # Handle duplicate entries for 64K
            for i in range(0, 1 << (ARM_64K_BITS - ARM_4K_BITS)):
                entry = subtree[entry_index + i]

                if entry.is_valid():
                    raise_error(virt, phys)

                entry.set_64kpage(phys, access, attributes, not_global)

        # Map a page
        elif size == SIZE_4K:
            entry = subtree[entry_index]

            if entry.is_valid():
                raise_error(virt, phys)

            entry.set_4kpage(phys, access, attributes, not_global)

        else:
            raise MapPageException('Cannot map virt %#8.8x to phys %#8.8x, '
                                   'invalid pagesize %d requested' %
                                   (virt, phys, size))

def page_table_size():
    """Return the size of the top level page table entry."""
    return ARM_TOPTREE_ENTRIES
