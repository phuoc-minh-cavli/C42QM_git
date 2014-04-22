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
Based on arch/armv7/kernel/mmu-dynamic/src/mmu_context.c
"""

from pyokl4.arch.armv7.okl4.mmu_common.map_page import mmu_map_page
from pyokl4.arch.armv7.okl4.mmu_common.mmu_context import AsidAllocator
from pyokl4.autogen import Autogen
from pyokl4.arch.armv7.okl4.mmu_common.internal import \
        ARM_TOPTREE_ENTRIES, ARM_TOPTREE_SIZE, ARM_L1PageTableEntry, \
        ARM_L1KernelPageTableEntry
from weaver.weavertypes import SimpleElfweaverType

import pyokl4.events as events

ASID_BITMAP_SIZE = 32

class AsidAllocator(object):

    class AsidBitmapItem(SimpleElfweaverType):

        _type_size = 4

        # Field property list.  The keys are used to detect places where a
        # renamed field has been missed.  The value describes various field
        # properties.
        _plist = {
            'allocated': {},
            'bitmap': {},
            }

        def __init__(self, address, back_ptr = None, phys_addr = None):
            SimpleElfweaverType.__init__(self, address, back_ptr, phys_addr)

            self.allocated = 0
            self.bitmap = 0

        def is_full(self):
            """Return whether or not the bitmap is full."""
            assert(self.allocated <= ASID_BITMAP_SIZE)

            return self.allocated == ASID_BITMAP_SIZE

        def allocate(self):
            """Allocate an ASID from the bitmap"""
            assert(self.allocated < ASID_BITMAP_SIZE)

            self.bitmap = (self.bitmap | (1 << self.allocated))
            self.allocated += 1

            return (self.allocated - 1)

        def to_string(self, _ = ''):
            """Generate a human-readable form of the object."""
            return "%#x" % (self.bitmap)

        def to_data(self):
            """Convert the bitmap into binary data."""
            return self._pack(4, self.bitmap)

    def __init__(self, domain, num_asids):
        self.num_items = num_asids / ASID_BITMAP_SIZE
        assert((self.num_items * ASID_BITMAP_SIZE) == num_asids)
        self.items = Autogen.objmanager.objmanager_allocate(self.AsidBitmapItem,
                                                        domain, self.num_items, 4)
        assert(self.items is not None)

    def alloc(self):
        """Allocate a new ASID."""
        for i, item in enumerate(self.items):
            if not item.is_full():
                return (i * ASID_BITMAP_SIZE) + item.allocate()

        assert("No more ASIDS!" is None)

    def to_string(self, _ = ""):
        """Generate a human-readable form of the object."""
        return "Asid bitmap:\n" + \
               ''.join("\t%s\n" % (item.to_string()) for item in self.items)

    def patch_image(self, elf, image):
        asid_bitmap = elf.find_symbol('asid_bitmap')
        assert(asid_bitmap is not None)

        asid_bitmap_address = self.items[0].get_address()
        image.patch(asid_bitmap, asid_bitmap_address)

kernel_mappings = []
asid_allocator = None

def mmu_add_kernel_mapping(virt, phys, page_size, access, cache,
                           dont_filter_physreloc=False):
    """
    Record the kernel mapping for later inclusion into mmu contexts when they
    are created.
    """
    kernel_mappings.append((virt, phys, page_size, access, cache, None,
                            dont_filter_physreloc))

def mmu_mmu_context_init(obj, **_):
    """Initialise the mmu context page table."""
    global asid_allocator
    assert(obj is not None)

    domain = obj._domain
    assert(domain is not None)

    if asid_allocator is None:
        asid_allocator = AsidAllocator(domain, 256)

    asid = asid_allocator.alloc()
    # print "asid = %s\n%s" % (asid, asid_allocator.to_string())

    pgtable = Autogen.objmanager.objmanager_allocate(ARM_L1PageTableEntry,
                                domain, ARM_TOPTREE_ENTRIES, ARM_TOPTREE_SIZE)
    assert(pgtable is not None)

    # FIXME: don't waste space, share L2
    for mapping in kernel_mappings:
        # Note: the kernel should be set global.  This isn't just a
        # a performance issue, because the kernel relies on the ability
        # to change the ASID in a global region.  If you make the kernel
        # non-global you will break this assumption.
        (virt, phys, page_size, access, cache, _,
         dont_filter_physreloc) = mapping
        mmu_map_page(pgtable, domain, virt, phys, page_size, access, cache,
                     not_global=False, dont_filter_physreloc=dont_filter_physreloc)

    assert pgtable[0].get_address() != 0

    obj.mmu.pgtable = pgtable
    obj.mmu.pgtable_phys = pgtable[0].get_phys_address
    obj.mmu.thread_reference_count = 0
    obj.mmu.asid = asid

    return True

kernel_mmu = None

def setup_kernel_mmu(elf, image, **kwargs):
    global kernel_mmu
    assert(kernel_mmu is not None)

    kernel_mmu = kernel_mmu[0].get_address()
    _kernel_mmu = elf.find_symbol('_kernel_mmu')
    assert(_kernel_mmu is not None)
    image.patch(_kernel_mmu, kernel_mmu)


def mmu_kernel_context_create(domain):
    """Initialise the kernel MMU context."""
    global kernel_mmu
    assert(kernel_mmu is None)

    kernel_mmu = Autogen.objmanager.objmanager_allocate(
            Autogen.types.OKL4_mmu_context, domain, 1, 4)
    assert(kernel_mmu is not None)

    pgtable = Autogen.objmanager.objmanager_allocate(ARM_L1KernelPageTableEntry,
            domain, Autogen.machine_info.KERNEL_L1_ENTRIES, 1, \
            tail_alignment = ARM_TOPTREE_SIZE)
    assert(pgtable is not None)

    for mapping in kernel_mappings:
        (virt, phys, page_size, access, cache, _,
         dont_filter_physreloc) = mapping
        mmu_map_page(pgtable, domain, virt, phys, page_size, access, cache,
                     not_global=False, kernel_pgtable = True,
                     dont_filter_physreloc=dont_filter_physreloc)

    kernel_mmu[0].mmu.pgtable = pgtable
    kernel_mmu[0].mmu.pgtable_phys = pgtable[0].get_phys_address
    kernel_mmu[0]._domain = domain

    events.subscribe("kernel_image_writeout", setup_kernel_mmu)

def init():
    """Initialise this module"""
    global kernel_mappings
    global asid_allocator

    kernel_mappings = []
    asid_allocator = None

    events.subscribe("mmu_context_create", mmu_mmu_context_init)
