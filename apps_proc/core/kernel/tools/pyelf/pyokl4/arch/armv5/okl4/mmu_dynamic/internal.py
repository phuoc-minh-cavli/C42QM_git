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
Based on arch/armv5/kernel/mmu-dynamic/src/internal.h
"""
#pylint: disable-msg=C0103

from pyokl4.okl4.kernel_base.types import IS_ALIGNED
from weaver.weavertypes import SimpleElfweaverType
from pyokl4.autogen import Autogen

SIZE_1M  = 1 << 20
SIZE_64K = 1 << 16
SIZE_4K  = 1 << 12

SIZE_WORD = 4
SIZE_64M  = (SIZE_1M * 64)

# ARM v5 MMU attribute values
ARM_HW_CACHE_ATTR_UNCACHED     = 0
ARM_HW_CACHE_ATTR_BUFFERED     = 1
ARM_HW_CACHE_ATTR_WRITEBACK    = 3
ARM_HW_CACHE_ATTR_WRITETHROUGH = 2
ARM_HW_CACHE_ATTR_MAX          = 3
ARM_HW_CACHE_ATTR_INVALID      = 0xffffffff

# ARM v5 MMU access values
ARM_ACCESS_SPECIAL    = 0
ARM_ACCESS_NONE       = 1
ARM_ACCESS_READONLY   = 2
ARM_ACCESS_READWRITE  = 3
ARM_ACCESS_MAX        = 3

# ARM Domains
ARM_DOMAIN_UNBOUND       = 0xff
ARM_DOMAIN_FIRST         = 0x1
ARM_DOMAIN_UNSET         = 0x0
ARM_DOMAIN_KERNEL        = 0x0

# Virtual memory indexes
ARM_L1_BITS           = 6
ARM_L2_BITS           = 6
ARM_L3_BITS           = 8
ARM_PAGE_BITS         = 12
ARM_VIRT_BITS         = 32
ARM_L1_OFFSET_BITS    = (ARM_L2_BITS + ARM_L3_BITS + ARM_PAGE_BITS)
ARM_L2_OFFSET_BITS    = (ARM_L3_BITS + ARM_PAGE_BITS)
ARM_PTE_BITS          = 2

ARM_L1_MASK           = 0xfc000000
ARM_L2_MASK           = 0x03f00000
ARM_L3_MASK           = 0x000ff000

ARM_64K_BITS        = 16
ARM_4K_BITS         = 12

# L1: ARM SW L1 PageTable Entries
ARM_L1_TYPE_INVALID   = 0
ARM_L1_TYPE_TABLE     = 1

# L2: ARM HW L1 PageTable Entries
ARM_L2_TYPE_INVALID   = 0
ARM_L2_TYPE_CTABLE    = 1 # Coarse table (64+4K pages)
ARM_L2_TYPE_1MPAGE    = 2 # Valid 1M page
ARM_L2_TYPE_FTABLE    = 3 # Fine table - unused

# L3: ARM HW L2 PageTable Entries
ARM_L3_TYPE_INVALID   = 0
ARM_L3_TYPE_64KPAGE   = 1 # Valid 64K page
ARM_L3_TYPE_4KPAGE    = 2 # Valid 4K page
ARM_L3_TYPE_NONE      = 3 # Reserved

ARM_ATTRIB_MAX        = 3
ARM_DIDS              = 16

ARM_L1_ENTRIES        = (1 << ARM_L1_BITS)
ARM_L2_ENTRIES        = (1 << ARM_L2_BITS)
ARM_L3_ENTRIES        = (1 << ARM_L3_BITS)

ARM_L1_SIZE           = (ARM_L1_ENTRIES << ARM_PTE_BITS)
ARM_L2_SIZE           = (ARM_L2_ENTRIES << ARM_PTE_BITS)
ARM_L3_SIZE           = (ARM_L3_ENTRIES << ARM_PTE_BITS)

ARM_HWL1_ALIGN        = 0x4000 # 16K
ARM_HWL2_ALIGN        = 0x400  # 1K

# Only used for internal py representation
ARM_LX_TYPE_INVALID   = 0

# ARM Domains
ARM_DAC_CLIENT = 0x1
ARM_DAC_MANGER = 0x3
ARM_DAC_MASK = 0x3
ARM_DAC_BITS = 2
ARM_DOMAIN_UNBOUND = 0xff

def ARM_DOMAIN2DAC_MASK(domain):
    """Return the DAC mask for the domain."""
    return ARM_DAC_MASK << ((domain) * ARM_DAC_BITS)

def ARM_DOMAIN2DAC_MANAGER(domain):
    """Return the DAC manager for the domain."""
    return ARM_DAC_MANGER << ((domain) * ARM_DAC_BITS)

def ARM_DOMAIN2DAC_CLIENT(domain):
    """Return the DAC client for the domain."""
    return ARM_DAC_CLIENT << ((domain) * ARM_DAC_BITS)


class PagetableException(Exception):
    """Exception to be raised if we have a problem in the page table"""

def ARM_L2_1MPAGE_GEN_PHYS(phys):
    """Return the phys address encoded for a L2 1M page entry."""
    return (phys)

def ARM_L2_1MPAGE_GEN_ATTRIB(attr):
    """Return the attrib encoded for a L2 1M page entry."""
    return ((attr) << 2)

def ARM_L2_1MPAGE_GEN_DOMAIN(domain):
    """Return the domain encoded for a L2 1M page entry."""
    return ((domain) << 5)

def ARM_L2_1MPAGE_GEN_ACCESS(access):
    """Return the access encoded for a L2 1M page entry."""
    return ((access) << 10)

def ARM_L2_CTABLE_GEN_BASE(base):
    """Return the base encoded  for a L2 ctable entry."""
    return (base)

def ARM_L2_CTABLE_GEN_DOMAIN(domain):
    """Return domain encoded for a L2 ctable entry."""
    return ((domain) << 5)

def ARM_L3_4KPAGE_GEN_PHYS(phys):
    """Return the phys address encoded for 1 L1 4K page entry."""
    return (phys)

def ARM_L3_4KPAGE_GEN_ATTRIB(attrib):
    """Return the attrib encoded for a L1 4K page entry."""
    return ((attrib) << 2)

def ARM_L3_4KPAGE_GEN_ACCESS(access):
    """Return the access encoded for a L1 4K page entry."""
    return ((((access) << 0) | ((access) << 2) |
             ((access) << 4) | ((access) << 6)) << 4)

def ARM_L3_64KPAGE_GEN_PHYS(phys):
    """Return the phys addr encoded for a L3 64K page entry."""
    return (phys)

def ARM_L3_64KPAGE_GEN_ATTRIB(attrib):
    """Return the attrib encoded for a L3 64K page entry."""
    return ((attrib) << 2)

def ARM_L3_64KPAGE_GEN_ACCESS(access):
    """Return the access encoded for a L3 64K page entry."""
    return ((((access) << 0) | ((access) << 2) |
             ((access) << 4) | ((access) << 6)) << 4)

class ARM_PageTableEntry(SimpleElfweaverType):
    """
    Base class for all dynamic ARMV5 page table levels.
    """
    _type_size = 4

    # Field property list.  The keys are used to detect places where a
    # renamed field has been missed.  The value describes various
    # field properties.
    _plist = {
        'type': {},
        }

    def __init__(self, address, back_ptr = None, phys_addr = None):
        SimpleElfweaverType.__init__(self, address, back_ptr, phys_addr)

        self.type = ARM_LX_TYPE_INVALID

    def is_valid(self):
        """Return whether or not this is a valid page table entry."""
        return self.type != ARM_LX_TYPE_INVALID

    def to_data(self):
        """Convert the chunk to binary data."""
        # Overridden below, and should never be called.
        assert False

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
    """ARMV5 L1 page table entry."""

    # Field property list.  The keys are used to detect places where a
    # renamed field has been missed.  The value describes various
    # field properties.
    _plist = {
        'type': {},
        'subtree': {},
        }

    def __init__(self, address, back_ptr = None, phys_addr = None):
        ARM_PageTableEntry.__init__(self, address, back_ptr, phys_addr)

        self.subtree = None # Python representation of subtree

    def set_subtree(self, phys_tree):
        """Set the subpage."""
        self.type = ARM_L1_TYPE_TABLE
        self.subtree = phys_tree

    def to_string(self, _ = ''):
        """Generate a human-readable form of the object."""
        if self.type == ARM_L1_TYPE_INVALID:
            string = "[ L1_TYPE_INVALID - %#x ]\n" % (0)
        elif self.type == ARM_L1_TYPE_TABLE:
            string = "[ L1_TYPE_TABLE - %#x ]\n" % \
                (self.subtree[0].get_address())
            for s in self.subtree:
                string += s.to_string()
        else:
            raise PagetableException('Bad entry type found in L1 page table')

        return string

    def to_data(self):
        """Convert the page table entry to binary data."""
        entry = self.type

        if entry == ARM_L1_TYPE_INVALID:
            pass
        elif entry == ARM_L1_TYPE_TABLE:
            entry += self.subtree[0].get_address()
        else:
            raise PagetableException('Bad entry type found in L1 page table')

        return self._pack(4, entry)

class ARM_SubPageTableEntry(ARM_PageTableEntry):
    """Common methods for ARMV5 L2 and L2 page table entries."""

    # Field property list.  The keys are used to detect places where a
    # renamed field has been missed.  The value describes various
    # field properties.
    _plist = {
        'type': {},
        'phys': {'phys_reloc': None, 'offset': 0},
        'access': {},
        'attribs': {},
        }

    def __init__(self, address, back_ptr = None, phys_addr = None):
        ARM_PageTableEntry.__init__(self, address, back_ptr, phys_addr)

        self.phys    = None
        self.access  = None
        self.attribs = None

        self.plist_put('phys', 'phys_reloc',
                       Autogen.types.OKL4_phys_reloc.PHYS_RELOC_UNKNOWN)

    def set_common(self, phys, access, attribs):
        """Set properties common to all sub page table levels."""
        self.phys    = phys
        self.access  = access
        self.attribs = attribs

class ARM_L2PageTableEntry(ARM_SubPageTableEntry):
    "ARMV5 L2 page table entry."""

    # Field property list.  The keys are used to detect places where a
    # renamed field has been missed.  The value describes various
    # field properties.
    _plist = {
        'type': {},
        'phys': {'phys_reloc': None, 'offset': 0},
        'access': {},
        'attribs': {},
        'domain': {},
        'subtree': {},
        }

    def __init__(self, address, back_ptr = None, phys_addr = None):
        ARM_SubPageTableEntry.__init__(self, address, back_ptr, phys_addr)

        self.domain  = None
        self.subtree = None # Python representation of subtree

        self.plist_put('phys', 'phys_reloc',
                       Autogen.types.OKL4_phys_reloc.PHYS_RELOC_UNKNOWN)

    def set_1mpage(self, phys, domain, access, attribs, dont_filter_physreloc):
        """Mark as a 1M page."""
        self.type = ARM_L2_TYPE_1MPAGE
        self.domain = domain
        self.set_common(phys, access, attribs)

        self.plist_put('phys', 'phys_reloc',
                       Autogen.types.OKL4_phys_reloc.PHYS_RELOC_ARMV5_L1_1MPAGE)
        self.plist_put('phys', 'dont_filter', dont_filter_physreloc)

    def set_subtree(self, domain, phys_tree):
        """Set the subpage."""
        self.type = ARM_L2_TYPE_CTABLE
        self.domain = domain
        self.subtree = phys_tree
        self.phys = phys_tree[0].get_phys_address

        self.plist_put('phys', 'phys_reloc',
                       Autogen.types.OKL4_phys_reloc.PHYS_RELOC_ARMV5_L1_L2TABLE)

    def to_string(self, _ = ''):
        """Generate a human-readable form of the object."""
        entry = self.type

        if entry == ARM_L2_TYPE_INVALID:
            string = "\t[ L2_TYPE_INVALID - %#x ]\n" % (0)
        elif entry == ARM_L2_TYPE_1MPAGE:
            string = "\t[ L2_TYPE_1MPAGE - phys: %#x - domain: %s -" \
                      " access: %s - attribs: %s ]\n" % (self.phys,
                                   self.domain, self.access, self.attribs)
        elif entry == ARM_L2_TYPE_CTABLE:
            string = "\t[ L2_TYPE_CTABLE - phys: %#x - domain: %s ]\n" \
                                            % (self.phys, self.domain)
            for s in self.subtree:
                string += s.to_string()
        else:
            raise PagetableException('Bad entry type found in L2 page table')

        return string

    def to_data(self):
        """Convert the page table entry to binary data."""
        entry = self.type

        if entry == ARM_L2_TYPE_INVALID:
            pass

        elif entry == ARM_L2_TYPE_1MPAGE:
            assert(self.access <= ARM_ACCESS_MAX)
            assert(self.attribs <= ARM_ATTRIB_MAX)
            assert(self.domain < ARM_DIDS)
            assert(IS_ALIGNED(self.phys, SIZE_1M))

            entry |= ARM_L2_1MPAGE_GEN_ACCESS(self.access)
            entry |= ARM_L2_1MPAGE_GEN_DOMAIN(self.domain)
            entry |= ARM_L2_1MPAGE_GEN_ATTRIB(self.attribs)
            entry |= ARM_L2_1MPAGE_GEN_PHYS(self.phys)

        elif entry == ARM_L2_TYPE_CTABLE:
            assert(IS_ALIGNED(self.phys(), ARM_L3_SIZE))
            assert(self.domain < ARM_DIDS)

            entry |= ARM_L2_CTABLE_GEN_DOMAIN(self.domain)
            entry |= ARM_L2_CTABLE_GEN_BASE(self.phys())

        else:
            raise PagetableException('Bad entry type found in L2 page table')

        return self._pack(4, entry)

class ARM_L3PageTableEntry(ARM_SubPageTableEntry):
    """ARMV5 L3 page table entry."""

    # Field property list.  The keys are used to detect places where a
    # renamed field has been missed.  The value describes various
    # field properties.
    _plist = {
        'type': {},
        'phys': {'phys_reloc': None, 'offset': 0},
        'access': {},
        'attribs': {},
        'domain': {},
        'subtree': {},
        }

    def __init__(self, address, back_ptr = None, phys_addr = None):
        ARM_SubPageTableEntry.__init__(self, address, back_ptr, phys_addr)

        self.plist_put('phys', 'phys_reloc',
                       Autogen.types.OKL4_phys_reloc.PHYS_RELOC_UNKNOWN)

    def set_64kpage(self, phys, access, attribs):
        """Mark as a 64K page."""
        self.type = ARM_L3_TYPE_64KPAGE
        self.set_common(phys, access, attribs)

        self.plist_put('phys', 'phys_reloc',
                       Autogen.types.OKL4_phys_reloc.PHYS_RELOC_ARMV5_L2_64KPAGE)

    def set_4kpage(self, phys, access, attribs):
        """Mark as a 4K page."""
        self.type = ARM_L3_TYPE_4KPAGE
        self.set_common(phys, access, attribs)

        self.plist_put('phys', 'phys_reloc',
                       Autogen.types.OKL4_phys_reloc.PHYS_RELOC_ARMV5_L2_4KPAGE)

    def to_string(self, _ = ''):
        """Generate a human-readable form of the object."""
        entry = self.type

        if entry == ARM_L3_TYPE_INVALID:
            string = "\t\t[ L3_TYPE_INVALID - %#x ]\n" % (0)
        elif entry == ARM_L3_TYPE_64KPAGE:
            string = "\t\t[ L3_TYPE_64KPAGE - phys: %#x - access: %s - " \
                                "attribs: %s ]\n" % (self.phys, self.access,
                                                        self.attribs)
        elif entry == ARM_L3_TYPE_4KPAGE:
            string = "\t\t[ L3_TYPE_4KPAGE - phys: %#x - access: %s - " \
                                "attribs: %s ]\n" % (self.phys, self.access,
                                                        self.attribs)
            #string += "\t\tDATA: %s" % (self.to_data().to_string(""))
        else:
            raise PagetableException('Bad entry type found in L3 page table')

        return string

    def to_data(self):
        """Convert the page table entry to binary data."""
        entry = self.type

        if entry == ARM_L3_TYPE_INVALID:
            pass

        elif entry == ARM_L3_TYPE_64KPAGE:
            assert(self.access <= ARM_ACCESS_MAX)
            assert(self.attribs <= ARM_ATTRIB_MAX)
            assert(IS_ALIGNED(self.phys, SIZE_64K))

            entry |= ARM_L3_64KPAGE_GEN_ACCESS(self.access)
            entry |= ARM_L3_64KPAGE_GEN_ATTRIB(self.attribs)
            entry |= ARM_L3_64KPAGE_GEN_PHYS(self.phys)

        elif entry == ARM_L3_TYPE_4KPAGE:
            assert(self.access <= ARM_ACCESS_MAX)
            assert(self.attribs <= ARM_ATTRIB_MAX)
            assert(IS_ALIGNED(self.phys, SIZE_4K))

            entry |= ARM_L3_4KPAGE_GEN_ACCESS(self.access)
            entry |= ARM_L3_4KPAGE_GEN_ATTRIB(self.attribs)
            entry |= ARM_L3_4KPAGE_GEN_PHYS(self.phys)

        else:
            raise PagetableException('Bad entry type found in L3 page table')

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

def arm_l3_index(addr):
    """Return the L3 index for the address."""
    addr = (addr & ARM_L3_MASK)
    addr = (addr >> ARM_PAGE_BITS)

    return addr

def arm_l1_entry(pgtable, addr):
    """Return the L1 entry for the address."""
    return pgtable.subtree[arm_l1_index(addr)]
