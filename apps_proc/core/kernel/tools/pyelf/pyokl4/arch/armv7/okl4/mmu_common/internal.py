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
Based on arch/armv7/kernel/mmu-dynamic/include/internal.h
"""

#pylint: disable-msg=C0103

from pyokl4.autogen import Autogen
from weaver.weavertypes import SimpleElfweaverType
from elf.util import align_down

SIZE_16M = 1 << 24
SIZE_1M  = 1 << 20
SIZE_64K = 1 << 16
SIZE_4K  = 1 << 12

# ARM Cache attributes
ARM_HW_CACHE_ATTR_STRONG                  = 0
ARM_HW_CACHE_ATTR_DEVICE_SHARED           = 2
ARM_HW_CACHE_ATTR_WRITETHROUGH            = 4
ARM_HW_CACHE_ATTR_WRITETHROUGH_SHARED     = 5
ARM_HW_CACHE_ATTR_WRITEBACK               = 6
ARM_HW_CACHE_ATTR_WRITEBACK_SHARED        = 7
ARM_HW_CACHE_ATTR_SHARED                  = 7
ARM_HW_CACHE_ATTR_UNCACHED                = 8
ARM_HW_CACHE_ATTR_UNCACHED_SHARED         = 9
ARM_HW_CACHE_ATTR_WRITEBACK_ALLOC         = 14
ARM_HW_CACHE_ATTR_WRITEBACK_ALLOC_SHARED  = 15
ARM_HW_CACHE_ATTR_DEVICE                  = 16
ARM_HW_CACHE_ATTR_CUSTOM                  = 31
ARM_HW_CACHE_ATTR_MAX                     = 0x3f
ARM_HW_CACHE_ATTR_OUTER_ONLY              = 0x38
ARM_HW_CACHE_ATTR_INVALID                 = 0xffffffff
"""
ARM_HW_CACHE_ATTR_STRONG                  = 0
ARM_HW_CACHE_ATTR_DEVICE_SHARED           = 3
ARM_HW_CACHE_ATTR_WRITETHROUGH            = 4
ARM_HW_CACHE_ATTR_WRITETHROUGH_SHARED     = 5
ARM_HW_CACHE_ATTR_WRITEBACK               = 6
ARM_HW_CACHE_ATTR_WRITEBACK_SHARED        = 7
ARM_HW_CACHE_ATTR_SHARED                  = 7
ARM_HW_CACHE_ATTR_UNCACHED                = 8
ARM_HW_CACHE_ATTR_UNCACHED_SHARED         = 9
ARM_HW_CACHE_ATTR_WRITEBACK_ALLOC         = 14
ARM_HW_CACHE_ATTR_WRITEBACK_ALLOC_SHARED  = 15
ARM_HW_CACHE_ATTR_DEVICE                  = 2
ARM_HW_CACHE_ATTR_CUSTOM                  = 7
ARM_HW_CACHE_ATTR_MAX                     = 0xf
ARM_HW_CACHE_ATTR_OUTER_ONLY              = 0xA
ARM_HW_CACHE_ATTR_INVALID                 = 0xffffffff
"""

# ARM v7 MMU access values
# (APX << 3) | (AP << 1) | XN
ARM_ACCESS_KNONE      = 0
ARM_ACCESS_KRW_UNONE  = 3
ARM_ACCESS_KRWX_UNONE = 2
ARM_ACCESS_KRW_UR     = 5
ARM_ACCESS_KRWX_URX   = 4
ARM_ACCESS_KRW_URW    = 7
ARM_ACCESS_KRWX_URWX  = 6
ARM_ACCESS_KR_UNONE   = 0xb
ARM_ACCESS_KRX_UNONE  = 0xa
ARM_ACCESS_KR_UR      = 0xf
ARM_ACCESS_KRX_URX    = 0xe

ARM_ACCESS_MAX  = 0xf

# Virtual memory indexes
ARM_VIRT_BITS       = 32
ARM_16M_BITS        = 24
ARM_1M_BITS         = 20
ARM_64K_BITS        = 16
ARM_4K_BITS         = 12
ARM_ENTRY_BITS      = 2

def find_pgtable_num_bits():
    vaddr_base = Autogen.machine_info.VADDR_SPACE_BASE
    max_valid_address = Autogen.machine_info.VADDR_SPACE_SIZE + vaddr_base - 1
    num_bits = 0
    while (max_valid_address > 0):
        max_valid_address >>= 1
        num_bits += 1
    else:
        assert(num_bits >= ARM_1M_BITS)
        return num_bits

ARM_TOPTREE_ENTRIES = (1 << (find_pgtable_num_bits() - ARM_1M_BITS))
ARM_SUBTREE_ENTRIES = (1 << (ARM_1M_BITS - ARM_4K_BITS))

ARM_TOPTREE_SIZE    = (ARM_TOPTREE_ENTRIES << ARM_ENTRY_BITS)
ARM_SUBTREE_SIZE    = (ARM_SUBTREE_ENTRIES << ARM_ENTRY_BITS)

# ARM L1 PageTable Entries
ARM_L1_TYPE_INVALID = 0
ARM_L1_TYPE_CTABLE  = 1               # Coarse table (64+4K pages)
ARM_L1_TYPE_SECTION = 2               # Valid 1M or 16M page
ARM_L1_TYPE_NONE    = 3               # Reserved
ARM_L1_SUBTYPE_16MPAGE  = 0x40000     # Valid 16M page
ARM_L1_SUBTYPE_1MPAGE   = 0           # Valid 1M page

ARM_L1_OFFSET_BITS    = (ARM_1M_BITS)
ARM_L2_OFFSET_BITS    = (ARM_4K_BITS)
ARM_L2_MASK           = 0x000ff000

# ARM L2 PageTable Entries
ARM_L2_TYPE_INVALID = 0
ARM_L2_TYPE_64KPAGE = 1  # Valid 64K page
ARM_L2_TYPE_4KPAGE  = 2  # Valid 4K page

ARM_ATTRIB_MAX  = 0x5f
ARM_DOMAIN_MAX  = 15

# These 3 are only used for internal py representation
ARM_LX_TYPE_INVALID = 0
ARM_L1_TYPE_16MPAGE = 2
ARM_L1_TYPE_1MPAGE  = 3

class PagetableException(Exception):
    """Exception to be raised if we have a problem in the page table"""

def ARM_L1_16MPAGE_GEN_PHYS(x):
    """Return the attribs encoded for a L1 16M page entry."""
    return (x)

def ARM_L1_16MPAGE_GEN_ATTRIBS(x):
    """Return the attribs encoded for a L1 16M page entry."""
    return (((x & 6) << 1) | ((x & 0x38) << 9) | ((x & 1) << 16))

def ARM_L1_16MPAGE_GEN_ACCESS(x):
    """Return the access encoded for a L1 16M page entry."""
    return (((x & 6) << 9) | ((x & 1) << 4) | ((x & 8) << 12))

def ARM_L1_16MPAGE_GEN_NOTGLOBAL(x):
    """Return the not global flag encoded for a L1 16M page entry."""
    return ((x & 1) << 17)


def ARM_L1_1MPAGE_GEN_PHYS(x):
    """Return the phys address encoded for a L1 1M page entry."""
    return (x)

def ARM_L1_1MPAGE_GEN_ATTRIBS(x):
    """Return the attribs encoded for a L1 1M page entry."""
    return (((x & 6) << 1) | ((x & 0x38) << 9) | ((x & 1) << 16))

def ARM_L1_1MPAGE_GEN_DOMAIN(x):
    """Return the domain encoded for a L1 1M page entry."""
    return ((x) << 5)

def ARM_L1_1MPAGE_GEN_ACCESS(x):
    """Return the access encoded for a L1 1M page entry."""
    return (((x & 6) << 9) | ((x & 1) << 4) | ((x & 8) << 12))

def ARM_L1_1MPAGE_GEN_NOTGLOBAL(x):
    """Return the not global flag encoded for a L1 1M page entry."""
    return ((x & 1) << 17)


def ARM_L1_CTABLE_GEN_DOMAIN(x):
    """Return the domain encoded for a L1 ctable page entry."""
    return x << 5

def ARM_L1_CTABLE_GEN_BASE(x):
    """Return the base encoded for a L1 ctable page entry."""
    return x


def ARM_L2_4KPAGE_GEN_PHYS(x):
    """Return the phys address encoded for a L2 4K page entry."""
    return (x)

def ARM_L2_4KPAGE_GEN_ATTRIBS(x):
    """Return the attribs encoded for a L2 4K page entry."""
    return (((x & 6) << 1) | ((x & 0x38) << 3) | ((x & 1) << 10))

def ARM_L2_4KPAGE_GEN_ACCESS(x):
    """Return the access encoded for a L2 4K page entry."""
    return (((x & 6) << 3) | (x & 1) | ((x & 8) << 6))

def ARM_L2_4KPAGE_GEN_NOTGLOBAL(x):
    """Return the not global flag encoded for a L2 4K page entry."""
    return ((x & 1) << 11)


def ARM_L2_64KPAGE_GEN_PHYS(x):
    """Return the phys address encoded for a L2 64K page entry."""
    return (x)

def ARM_L2_64KPAGE_GEN_ATTRIBS(x):
    """Return the attribs encoded for a L2 64K page entry."""
    return (((x & 6) << 1) | ((x & 0x38) << 9) | ((x & 1) << 10))

def ARM_L2_64KPAGE_GEN_ACCESS(x):
    """Return the access encoded for a L2 64K page entry."""
    return (((x & 6) << 3) | ((x & 1) << 15) | ((x & 8) << 6))

def ARM_L2_64KPAGE_GEN_NOTGLOBAL(x):
    """Return the not global flag encoded for a L2 64K page entry."""
    return ((x & 1) << 11)


class ARM_PageTableEntry(SimpleElfweaverType):
    """
    Base class for all ARMV7 page table levels.
    """

    _type_size = 4

    # Field property list.  The keys are used to detect places where a
    # renamed field has been missed.  The value describes various
    # field properties.
    _plist = {
        'type': {},
        'phys': {'phys_reloc': None, 'offset': 0},
        'rights': {},
        'attribs': {},
        'not_global': {},
        }

    def __init__(self, address, back_ptr = None, phys_addr = None):
        SimpleElfweaverType.__init__(self, address, back_ptr, phys_addr)

        self.type       = ARM_LX_TYPE_INVALID
        self.phys       = None
        self.rights     = None
        self.attribs    = None
        self.not_global = None

        self.make_instance_plist()
        self.plist_put('phys', 'phys_reloc',
                       Autogen.types.OKL4_phys_reloc.PHYS_RELOC_UNKNOWN)

    def set_common(self, phys, rights, attribs, not_global):
        """Set properties common to all page table levels."""
        self.phys       = phys
        self.rights     = rights
        self.attribs    = attribs
        self.not_global = not_global

    def is_valid(self):
        """Return whether or not this is a valid page table entry."""
        return self.type != ARM_LX_TYPE_INVALID

    def get_phys_relocs(self, base_offset=0):
        """
        Return a physical relocation record for the page table entry it is is
        valid.
        """
        if self.is_valid():
            phys_relocs = self._get_local_phys_reloc(base_offset)

            if self.plist_get('phys', 'dont_filter', False):
                for reloc in phys_relocs:
                    reloc.must_filter = False

            return phys_relocs
        else:
            return []

class ARM_L1PageTableEntry(ARM_PageTableEntry):
    """ARMV7 L1 page table entry."""

    # Field property list.  The keys are used to detect places where a
    # renamed field has been missed.  The value describes various
    # field properties.
    _plist = {
        'type': {},
        'phys': {'phys_reloc': None, 'offset': 0},
        'rights': {},
        'attribs': {},
        'not_global': {},
        'subtree': {},
        'domain': {},
        }

    def __init__(self, address, back_ptr = None, phys_addr = None):
        ARM_PageTableEntry.__init__(self, address, back_ptr, phys_addr)

        self.subtree = None # Python representation of subtree
        self.domain  = None

    def set_16mpage(self, phys, rights, attribs, not_global,
                    dont_filter_physreloc):
        """Mark as a 16M page"""
        self.type = ARM_L1_TYPE_16MPAGE
        self.set_common(phys, rights, attribs, not_global)

        self.plist_put('phys', 'phys_reloc',
                       Autogen.types.OKL4_phys_reloc.PHYS_RELOC_ARMV7_L1_16MPAGE)
        self.plist_put('phys', 'dont_filter', dont_filter_physreloc)

    def set_1mpage(self, phys, domain, rights, attribs, not_global,
                   dont_filter_physreloc):
        """Mark as a 1M page."""
        self.type = ARM_L1_TYPE_1MPAGE
        self.domain = domain
        self.set_common(phys, rights, attribs, not_global)

        self.plist_put('phys', 'phys_reloc',
                       Autogen.types.OKL4_phys_reloc.PHYS_RELOC_ARMV7_L1_1MPAGE)
        self.plist_put('phys', 'dont_filter', dont_filter_physreloc)

    def set_subtree(self, domain, phys_tree):
        """Set the subpage."""
        self.type = ARM_L1_TYPE_CTABLE
        self.domain = domain
        self.subtree = phys_tree
        self.phys = phys_tree[0].get_phys_address

        self.plist_put('phys', 'phys_reloc',
                       Autogen.types.OKL4_phys_reloc.PHYS_RELOC_ARMV7_L1_L2TABLE)

    def to_string(self, _ = ""):
        """Generate a human-readable form of the object."""
        entry = self.type

        if entry == ARM_L1_TYPE_INVALID:
            string = "[ L1_TYPE_INVALID - %#x ]\n" % (0)
        elif entry == ARM_L1_TYPE_1MPAGE:
            string = "[ L1_TYPE_1MPAGE - phys: %#x - domain: %s -" \
                  " access: %s - attribs: %s ]\n" % \
                  (self.phys, self.domain, self.rights, self.attribs)
        elif entry == ARM_L1_TYPE_16MPAGE:
            string = "[ L1_TYPE_16MPAGE - phys: %#x - domain: %s -" \
                  " access: %s - attribs: %s ]\n" % \
                  (self.phys, self.domain, self.rights, self.attribs)
        elif entry == ARM_L1_TYPE_CTABLE:
            string = "[ L1_TYPE_CTABLE - phys: %#x - domain: %s ]\n" % \
                  (self.phys(), self.domain)
            string += ''.join(s.to_string() for s in self.subtree)
        else:
            raise PagetableException('Bad entry type found in L1 page table')

        return string

    def to_word(self):
        """Convert the page table entry to a word."""
        entry = self.type

        if entry == ARM_L1_TYPE_INVALID:
            pass

        elif entry == ARM_L1_TYPE_16MPAGE:
            entry = ARM_L1_TYPE_SECTION | ARM_L1_SUBTYPE_16MPAGE
            assert(self.rights <= ARM_ACCESS_MAX)
            assert(self.attribs <= ARM_ATTRIB_MAX)
            assert((self.phys & (SIZE_16M - 1)) == 0)

            entry |= ARM_L1_16MPAGE_GEN_ACCESS(self.rights)
            entry |= ARM_L1_16MPAGE_GEN_ATTRIBS(self.attribs)
            entry |= ARM_L1_16MPAGE_GEN_PHYS(self.phys)
            entry |= ARM_L1_16MPAGE_GEN_NOTGLOBAL(self.not_global)

        elif entry == ARM_L1_TYPE_1MPAGE:
            entry = ARM_L1_TYPE_SECTION | ARM_L1_SUBTYPE_1MPAGE
            assert(self.rights <= ARM_ACCESS_MAX)
            assert(self.domain <= ARM_DOMAIN_MAX)
            assert((self.phys & (SIZE_1M - 1)) == 0)

            entry |= ARM_L1_1MPAGE_GEN_ACCESS(self.rights)
            entry |= ARM_L1_1MPAGE_GEN_DOMAIN(self.domain)
            entry |= ARM_L1_1MPAGE_GEN_ATTRIBS(self.attribs)
            entry |= ARM_L1_1MPAGE_GEN_PHYS(self.phys)
            entry |= ARM_L1_1MPAGE_GEN_NOTGLOBAL(self.not_global)

        elif entry == ARM_L1_TYPE_CTABLE:
            assert((self.phys() & (ARM_SUBTREE_SIZE - 1)) == 0)
            assert(self.domain <= ARM_DOMAIN_MAX)

            entry |= ARM_L1_CTABLE_GEN_DOMAIN(self.domain)
            entry |= ARM_L1_CTABLE_GEN_BASE(self.phys())

        else:
            raise PagetableException('Bad entry type found in L1 page table')

        return entry

    def to_data(self):
        """Convert the page table entry to binary data."""
        return self._pack(4, self.to_word())

class ARM_L1KernelPageTableEntry(ARM_L1PageTableEntry):
    """
    ARMV7 L1 page table entry for kernel pages, which need custom address
    functions.
    """
    def __init__(self, address, back_ptr = None, phys_addr = None):
        ARM_L1PageTableEntry.__init__(self, address, back_ptr, phys_addr)

    def get_address(self):
        """Return the virtual address of the page table."""
        return align_down(self._address, ARM_TOPTREE_SIZE)

    def get_phys_address(self):
        """Return the physical address of the page table."""
        return align_down(self._phys_addr, ARM_TOPTREE_SIZE)

class ARM_L2PageTableEntry(ARM_PageTableEntry):
    """ARMV7 L2 page table entry."""

    # Field property list.  The keys are used to detect places where a
    # renamed field has been missed.  The value describes various
    # field properties.
    _plist = {
        'type': {},
        'phys': {'phys_reloc': None, 'offset': 0},
        'rights': {},
        'attribs': {},
        'not_global': {},
        }

    def __init__(self, address, back_ptr = None, phys_addr = None):
        ARM_PageTableEntry.__init__(self, address, back_ptr, phys_addr)

    def set_64kpage(self, phys, rights, attribs, not_global):
        """Mark as a 64K page."""
        self.type = ARM_L2_TYPE_64KPAGE
        self.set_common(phys, rights, attribs, not_global)

        self.plist_put('phys', 'phys_reloc',
                       Autogen.types.OKL4_phys_reloc.PHYS_RELOC_ARMV7_L2_64KPAGE)

    def set_4kpage(self, phys, rights, attribs, not_global):
        """Mark as a 4K page."""
        self.type = ARM_L2_TYPE_4KPAGE
        self.set_common(phys, rights, attribs, not_global)

        self.plist_put('phys', 'phys_reloc',
                       Autogen.types.OKL4_phys_reloc.PHYS_RELOC_ARMV7_L2_4KPAGE)

    def to_string(self, _ = ""):
        """Generate a human-readable form of the object."""
        entry = self.type

        if entry == ARM_L2_TYPE_INVALID:
            string = "\t[ L2_TYPE_INVALID - %#x ]\n" % (0)
        elif entry == ARM_L2_TYPE_64KPAGE:
            string = "\t[ L2_TYPE_64KPAGE - phys: %#x -" \
                      " access: %s - attribs: %s ]\n" % \
                      (self.phys, self.rights, self.attribs)
        elif entry == ARM_L2_TYPE_4KPAGE:
            string = "\t[ L2_TYPE_4KPAGE - phys: %#x -" \
                      " access: %s - attribs: %s ]\n" % \
                      (self.phys, self.rights, self.attribs)
        else:
            raise PagetableException('Bad entry type found in L2 page table')

        return string

    def to_data(self):
        """Convert the page table entry to binary data."""
        entry = self.type

        if entry == ARM_L2_TYPE_INVALID:
            pass

        elif entry == ARM_L2_TYPE_64KPAGE:
            assert(self.rights <= ARM_ACCESS_MAX)
            assert(self.attribs <= ARM_ATTRIB_MAX)
            assert((self.phys & (SIZE_4K - 1)) == 0)

            entry |= ARM_L2_64KPAGE_GEN_ACCESS(self.rights)
            entry |= ARM_L2_64KPAGE_GEN_ATTRIBS(self.attribs)
            entry |= ARM_L2_64KPAGE_GEN_PHYS(self.phys)
            entry |= ARM_L2_64KPAGE_GEN_NOTGLOBAL(self.not_global)

        elif entry == ARM_L2_TYPE_4KPAGE:
            assert(self.rights <= ARM_ACCESS_MAX)
            assert(self.attribs <= ARM_ATTRIB_MAX)
            assert((self.phys & (SIZE_4K - 1)) == 0)

            entry |= ARM_L2_4KPAGE_GEN_ACCESS(self.rights)
            entry |= ARM_L2_4KPAGE_GEN_ATTRIBS(self.attribs)
            entry |= ARM_L2_4KPAGE_GEN_PHYS(self.phys)
            entry |= ARM_L2_4KPAGE_GEN_NOTGLOBAL(self.not_global)

        else:
            raise PagetableException('Bad entry type found in L2 page table')

        return self._pack(4, entry)


# Page table indexing functions

def arm_l1_index(addr):
    """Return the L1 index for the address."""
    return (addr >> ARM_L1_OFFSET_BITS)

def arm_l2_index(addr):
    """Return the L2 index for the address."""
    addr = (addr & ARM_L2_MASK)
    addr = (addr >> ARM_L2_OFFSET_BITS)
    return addr
