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
Based on arch/armv5/kernel/mmu-dynamic/src/map_page.c
"""

from pyokl4.okl4.mmu_dynamic.access import \
        ACCESS_NONE, ACCESS_R, ACCESS_RW, ACCESS_RX, ACCESS_RWX, ACCESS_KDEVICE
from pyokl4.okl4.mmu_common.mmu_attrib import mmu_get_cpu_attrib
from pyokl4.arch.armv5.okl4.mmu_dynamic.internal import \
        SIZE_1M, SIZE_64K, SIZE_4K, SIZE_WORD, \
        ARM_64K_BITS, ARM_4K_BITS, \
        ARM_L2_ENTRIES, ARM_L3_ENTRIES, \
        ARM_HWL1_ALIGN, ARM_HWL2_ALIGN, ARM_DOMAIN_UNSET, \
        ARM_ACCESS_NONE, ARM_ACCESS_READONLY, ARM_ACCESS_READWRITE, \
        ARM_L2PageTableEntry, \
        ARM_L3PageTableEntry, ARM_L2_TYPE_CTABLE, \
        arm_l1_index, arm_l2_index, arm_l3_index
from pyokl4.autogen import Autogen

class MapPageException(Exception):
    """Exception to be raised if we have a problem when mapping a page"""

def mmu_get_arch_rwx(rwx):
    """Map rwx to arch specific values."""
    if rwx == ACCESS_R:
        access = ARM_ACCESS_READONLY
    elif rwx == ACCESS_RX:
        access = ARM_ACCESS_READONLY
    elif rwx == ACCESS_RW:
        access = ARM_ACCESS_READWRITE
    elif rwx == ACCESS_RWX:
        access = ARM_ACCESS_READWRITE
    elif rwx == ACCESS_NONE:
        access = ARM_ACCESS_NONE
    elif rwx == ACCESS_KDEVICE:
        access = ARM_ACCESS_NONE
    else:
        raise MapPageException('Invalid page permissions!')
    return access

def mmu_map_page(pgtable, domain, virt, phys, size, rwx, generic_attr,
                 dont_filter_physreloc=False):
    """Do the work to map a page."""

    def raise_error():
        """Raise an error if a mapping can't be made."""
        raise MapPageException('Cannot map virt %#8.8x to phys %#8.8x,'
                               ' existing mapping found' % (virt, phys))

    access = mmu_get_arch_rwx(rwx)
    attributes = mmu_get_cpu_attrib(generic_attr)

    # L1
    entry_index = arm_l1_index(virt)
    entry = pgtable[entry_index]

    if not entry.is_valid(): # subtree does not exist, create it
        entry.set_subtree(Autogen.objmanager.objmanager_allocate(
                ARM_L2PageTableEntry, domain, ARM_L2_ENTRIES, SIZE_WORD))

    tree = entry.subtree

    # L2
    entry_index = arm_l2_index(virt)
    entry = tree[entry_index]

    # Map a section
    if size == SIZE_1M:
        if entry.is_valid():
            raise_error(virt, phys)

        entry.set_1mpage(phys, ARM_DOMAIN_UNSET, access, attributes,
                         dont_filter_physreloc)
    else:
        if not entry.is_valid():
            # subtree does not exist, create it
            entry.set_subtree(ARM_DOMAIN_UNSET,
                    Autogen.objmanager.objmanager_allocate(ARM_L3PageTableEntry,
                        domain, ARM_L3_ENTRIES, ARM_HWL2_ALIGN))

        if entry.type != ARM_L2_TYPE_CTABLE: # is not a subtree
            raise_error(virt, phys)

        tree = entry.subtree

        # L3
        entry_index = arm_l3_index(virt)

        # Map a large page
        if size == SIZE_64K:
            # Handle duplicate entries for 64K
            for i in range (0, 1 << (ARM_64K_BITS - ARM_4K_BITS)):
                entry = tree[entry_index + i]

                if entry.is_valid():
                    raise_error(virt, phys)

                entry.set_64kpage(phys, access, attributes)
        # Map a page
        elif size == SIZE_4K:
            entry = tree[entry_index]

            if entry.is_valid():
                raise_error(virt, phys)

            entry.set_4kpage(phys, access, attributes)

        else:
            raise MapPageException('Cannot map virt %#8.8x to phys %#8.8x, ' \
                                   'invalid pagesize %d requested' % \
                                       (virt, phys, size))
