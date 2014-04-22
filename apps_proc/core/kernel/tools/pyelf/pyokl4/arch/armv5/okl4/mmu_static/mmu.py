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

"""ARMV5 static mmu functions."""

from pyokl4.okl4.mmu_common.map_range import map_range
import pyokl4.arch.armv5.okl4.mmu_common.map_page as mmu_static_map_page
import pyokl4.events as events
import pyokl4.arch.armv5.okl4.mmu_common as mmu_common
from pyokl4.autogen import Autogen

# Global static page table.
_pg_table = None

# Use the common MMU module's upgrade_perms call.
upgrade_perms = mmu_common.map_page.upgrade_perms

def pgtable_create(domain):
    """Create the pagetable."""
    return mmu_static_map_page.page_table_create(domain)

#def map_page_by_phys(domain, mmu, vaddr, paddr, size, perms, cache):
def map_page_by_phys(domain, _, vaddr, paddr, size, perms, cache,
                     dont_filter_physreloc=False):
    """?"""
    #pylint: disable-msg=W0603
    global _pg_table
    if _pg_table is None:
        _pg_table = pgtable_create(domain)
    assert _pg_table is not None

    mappings = map_range(vaddr, paddr, size, Autogen.machine_info.PAGE_SIZES)

    for (virt, phys, page_size) in mappings:
        mmu_static_map_page.mmu_map_page(_pg_table,
                                         domain, virt, phys, page_size, perms,
                                         cache,
                                         dont_filter_physreloc=dont_filter_physreloc)

#def map_page(domain, mmu, vaddr, segment, offest, size, perms, cache):
def map_page(domain, mmu, vaddr, segment, _, size, perms, cache):
    """Map a page into the static pagetable."""
    map_page_by_phys(domain, mmu, vaddr, segment.attrs.phys_addr, size, perms,
                     cache)

def map_kernel_page(current_domain, vaddr, paddr, size, perms, cache,
                    dont_filter_physreloc=False):
    """Map a kernel (global) mapping into our static page table."""
    map_page_by_phys(current_domain, None, vaddr, paddr, size, perms, cache,
                     dont_filter_physreloc=dont_filter_physreloc)

def dump_pgtable():
    """Dump the static pagetable as a string."""
    assert _pg_table is not None

    return ''.join(_pg_table[i].to_string()
                    for i in range(1, mmu_static_map_page.page_table_size()))

def setup_pgtable(elf, image, **_):
    """Setup the pagetable in the given binary image."""
    assert _pg_table is not None

    _mmu_pgtable = elf.find_symbol('_mmu_pgtable')
    _mmu_pgtable_phys = elf.find_symbol('_mmu_pgtable_phys')
    assert _mmu_pgtable is not None
    assert _mmu_pgtable_phys is not None
    pgtable_virt = _pg_table[0].get_address()
    pgtable_phys = _pg_table[0].get_phys_address()
    image.patch(_mmu_pgtable, pgtable_virt)
    image.patch(_mmu_pgtable_phys, pgtable_phys)

    # Register a phys reloc record for _mmu_pgtable_phys.
    seg = elf.find_segment_by_vaddr(_mmu_pgtable_phys.value)
    phys_addr = _mmu_pgtable_phys.value - seg.vaddr + seg.paddr
    image.phys_reloc_table.add_relocation(phys_addr,
                                          Autogen.types.OKL4_phys_reloc.PHYS_RELOC_ABS,
                                          pgtable_phys)

def map_kernel_bootstrap_page(current_domain, vaddr,
                              paddr, size, perms, cache):
    """
    Map a temporary page into the kernel pagetable, used for a few cycl3es
    when the MMU is initially being turned on at system boot time.
    """
    map_kernel_page(current_domain, vaddr, paddr, size, perms, cache)

def virt_to_l1_entry(virt):
    # Not implemented on armv5
    return 0

def init():
    """Initialise this module"""
    #pylint: disable-msg=W0603
    global _pg_table
    _pg_table = None

    # Patch the kernel pagetable just before we writeout the image.
    events.subscribe("kernel_image_writeout", setup_pgtable)
