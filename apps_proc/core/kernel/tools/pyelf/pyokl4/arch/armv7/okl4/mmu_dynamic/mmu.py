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

from pyokl4.autogen import Autogen
from pyokl4.okl4.mmu_common.map_range import map_range

import mmu_context as mmu_dynamic_mmu_context
from pyokl4.arch.armv7.okl4.mmu_common.map_page import upgrade_perms
import pyokl4.arch.armv7.okl4.mmu_common.map_page as mmu_dynamic_map_page
import pyokl4.events as events

# Global initial pagetable.
initial_mmu = None

def map_page(domain, mmu, vaddr, segment, offset, size, perms, cache):
    okl4_seg = segment.okl4_object.segment

    assert offset < okl4_seg.size
    assert offset + size <= okl4_seg.size
    assert (perms | okl4_seg.rwx_mask) == okl4_seg.rwx_mask
    assert (cache | okl4_seg.attribs_mask) == okl4_seg.attribs_mask

    phys_addr = okl4_seg.base + offset

    mappings = map_range(vaddr, phys_addr, size, Autogen.machine_info.PAGE_SIZES)

    for (virt, phys, page_size) in mappings:
        mmu_dynamic_map_page.mmu_map_page(mmu.mmu.pgtable,
                domain, virt, phys, page_size, perms, cache)
        if virt < mmu.mmu.first_used_vaddr:
            mmu.mmu.first_used_vaddr = virt

def map_kernel_page(current_domain, vaddr, paddr, size, perms, cache,
                    dont_filter_physreloc=False):
    mappings = map_range(vaddr, paddr, size, Autogen.machine_info.PAGE_SIZES)

    for (virt, phys, page_size) in mappings:
        mmu_dynamic_mmu_context.mmu_add_kernel_mapping(virt, phys, page_size,
                                                       perms, cache,
                                                       dont_filter_physreloc=dont_filter_physreloc)

def kernel_context_create(domain):
    mmu_dynamic_mmu_context.mmu_kernel_context_create(domain)

def dump_kernel_pgtable():
    assert(initial_mmu is not None)
    return initial_mmu.mmu.pgtable[0].to_string()

def setup_kernel_pgtable(elf, image, **kwargs):
    assert(initial_mmu is not None)

    _mmu_initial_pgtable = elf.find_symbol('_mmu_initial_pgtable')
    _mmu_initial_pgtable_phys = elf.find_symbol('_mmu_initial_pgtable_phys')
    assert(_mmu_initial_pgtable is not None)
    assert(_mmu_initial_pgtable_phys is not None)

    pgtable_virt = initial_mmu.mmu.pgtable[0].get_address()
    pgtable_phys = initial_mmu.mmu.pgtable[0].get_phys_address()
    image.patch(_mmu_initial_pgtable, pgtable_virt)
    image.patch(_mmu_initial_pgtable_phys, pgtable_phys)

    # Register a phys reloc record for _mmu_initial_pgtable_phys.
    seg = elf.find_segment_by_vaddr(_mmu_initial_pgtable_phys.value)
    phys_addr = _mmu_initial_pgtable_phys.value - seg.vaddr + seg.paddr
    image.phys_reloc_table.add_relocation(phys_addr,
                                          Autogen.types.OKL4_phys_reloc.PHYS_RELOC_ABS,
                                          initial_mmu.mmu.pgtable[0].get_phys_address())

    mmu_dynamic_mmu_context.asid_allocator.patch_image(elf, image)

# Maps the kernel 1:1 mapping into the initial pagetable.
def map_kernel_bootstrap_page(current_domain, vaddr, paddr, size, perms, cache):
    # This must be called after the first call to map_page which sets up
    # initial_mmu
    assert(initial_mmu is not None)

    assert(vaddr == paddr)
    assert(size == 0x100000)

    mmu_dynamic_map_page.mmu_map_page(initial_mmu.mmu.pgtable,
            current_domain, vaddr, paddr, size, perms, cache, False,
            overwrite = True)

# Look up the L1 entry for a virtual address in the initial page table
def virt_to_l1_entry(virt):
    return mmu_dynamic_map_page.mmu_virt_to_l1_entry(initial_mmu.mmu.pgtable, virt)

def setup_initial_mmu(obj, **_):
    global initial_mmu
    if initial_mmu is None:
        initial_mmu = obj

    assert(initial_mmu is not None)

# Initialise this module
def init():
    global initial_mmu
    initial_mmu = None

    # Patch the kernel pagetable just before we writeout the image.
    events.subscribe("kernel_image_writeout", setup_kernel_pgtable)
    events.subscribe("mmu_context_create", setup_initial_mmu)
