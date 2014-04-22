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

"""ARMV5 dynamic mmu functions."""

#pylint: disable-msg=W0403
from pyokl4.okl4.mmu_common.map_range import map_range
import pyokl4.arch.armv5.okl4.mmu_common as mmu_common
import map_page as mmu_cpd_map_page
from .. mmu_common import map_page as mmu_static_map_page
import pyokl4.events as events
from internal import arm_l2_index
import pyokl4.kernel_data as kernel_data
from pyokl4.autogen import Autogen

# Global kernel pagetable.
kernel_pgtable = None

# Use the common MMU module's upgrade_perms call.
upgrade_perms = mmu_common.map_page.upgrade_perms

def kernel_pgtable_create(domain):
    """Create a kernel pagetable"""
    return mmu_static_map_page.page_table_create(domain)

def map_page(domain, mmu, vaddr, segment, offset, size, perms, cache):
    """Map a page into the given domain."""
    okl4_seg = segment.okl4_object.segment
    assert offset < okl4_seg.size
    assert offset + size <= okl4_seg.size
    assert (perms | okl4_seg.rwx_mask) == okl4_seg.rwx_mask
    assert (cache | okl4_seg.attribs_mask) == okl4_seg.attribs_mask

    phys_addr = okl4_seg.base + offset

    mappings = map_range(vaddr, phys_addr, size, Autogen.machine_info.PAGE_SIZES)

    for (virt, phys, page_size) in mappings:
        mmu_cpd_map_page.mmu_map_page(mmu.mmu.pgtable, domain, virt, phys,
                page_size, perms, cache)

def map_kernel_page(current_domain, vaddr, paddr, size, perms, cache,
                    dont_filter_physreloc=False):
    """Map a page into the kernel's pagetable."""
    #pylint: disable-msg=W0603
    global kernel_pgtable

    if kernel_pgtable is None:
        kernel_pgtable = kernel_pgtable_create(current_domain)

    mappings = map_range(vaddr, paddr, size, Autogen.machine_info.PAGE_SIZES)

    for (virt, phys, page_size) in mappings:
        mmu_static_map_page.mmu_map_page(kernel_pgtable,
                current_domain, virt, phys, page_size, perms, cache,
                                         dont_filter_physreloc=dont_filter_physreloc)

def setup_kernel_pgtable(elf, image, **_):
    """Setup the kernel's pagetable into the given image."""
    assert kernel_pgtable is not None

    _mmu_cpd = elf.find_symbol('_mmu_cpd')
    _mmu_cpd_phys = elf.find_symbol('_mmu_cpd_phys')
    assert(_mmu_cpd is not None)
    assert(_mmu_cpd_phys is not None)

    pgtable_virt = kernel_pgtable[0].get_address()
    pgtable_phys = kernel_pgtable[0].get_phys_address()
    image.patch(_mmu_cpd, pgtable_virt)
    image.patch(_mmu_cpd_phys, pgtable_phys)

    # Register a phys reloc record for _mmu_cpd_phys.
    seg = elf.find_segment_by_vaddr(_mmu_cpd_phys.value)
    phys_addr = _mmu_cpd_phys.value - seg.vaddr + seg.paddr
    image.phys_reloc_table.add_relocation(phys_addr,
                                          Autogen.types.OKL4_phys_reloc.PHYS_RELOC_ABS,
                                          pgtable_phys)

def create_buffer_context_table(elf, image, **_):
    """Create a table of MMU contexts that own kernel buffers"""
    data = kernel_data.get_kernel_data()

    if "copy_area_mmu_table" in data:
        return

    data.create_array(
            "copy_area_mmu_table", Autogen.types.OKL4_void_ptr,
            Autogen.types.CHANNEL_PRIMARY_MAX_KMMUS)

def setup_buffer_context_table(elf, image, kernel, **_):
    data = kernel_data.get_kernel_data()
    copy_area_mmu_table = data.get_array("copy_area_mmu_table")

    for heap in kernel.heaps.values():
        for domain in heap.domains:
            for mmu in domain.mmu_contexts:
                if not mmu.copy_area:
                    continue
                realm_pool = mmu.copy_area

                for window in realm_pool.windows:
                    index = arm_l2_index(window[0] - \
                            Autogen.machine_info.USER_VIRT_END)
                    if index < 0 or \
                            index >= Autogen.types.CHANNEL_PRIMARY_MAX_KMMUS:
                        raise Exception, \
                                "Invalid index in copy area table: %d" % \
                                index

                    if copy_area_mmu_table[index].value:
                        print copy_area_mmu_table[index]
                        raise Exception, \
                                "Reused index in copy area table: %d" % \
                                index

                    copy_area_mmu_table[index].value = mmu.okl4_object

def dump_kernel_pgtable():
    """Dump the kernel's pagetable."""
    return ''.join(kernel_pgtable[i].to_string()
                   for i in range(1, mmu_static_map_page.page_table_size()))

def map_kernel_bootstrap_page(current_domain, vaddr,
                              paddr, size, perms, cache):
    """
    Map a temporary page into the kernel pagetable, used for a few cycles when
    the MMU is initially being turned on at system boot time.
    """
    map_kernel_page(current_domain, vaddr, paddr, size, perms, cache)

def virt_to_l1_entry(virt):
    # Not implemented on armv5
    return 0

def init():
    """Initialise this module"""
    #pylint: disable-msg=W0603
    global kernel_pgtable
    kernel_pgtable = None

    # Patch the kernel pagetable just before we writeout the image.
    events.subscribe("kernel_image_writeout", setup_kernel_pgtable)

    # Create and fill in the kernel buffer context table if it is needed
    if 'channel' in Autogen.modules:
        events.subscribe("kernel_image_load", \
                create_buffer_context_table)
        events.subscribe("kernel_image_writeout", \
                setup_buffer_context_table)
