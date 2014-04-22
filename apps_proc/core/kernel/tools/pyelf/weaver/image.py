##############################################################################
# Australian Public Licence B (OZPLB)
# 
# Version 1-0
# 
# Copyright (c) 2006-2011, Open Kernel Labs, Inc.
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

"""Generate the ELF image file."""

import os.path
from elf.core import UnpreparedElfFile, SectionedElfSegment
from elf.section import UnpreparedElfSection
from elf.constants import PT_PAX_FLAGS, PT_GNU_STACK, PT_PHDR, PT_LOAD, \
                          PT_MIPS_REGINFO, PT_ARM_EXIDX, PT_NOTE, PT_DYNAMIC, \
                          SHT_PROGBITS, SHT_NOBITS, SHF_WRITE, SHF_ALLOC, \
                          PF_R, PF_W, ElfMachine, EF_MIPS_ABI_O64, ET_EXEC, \
                          SHN_ABS, SHN_UNDEF, STB_WEAK, \
                          SHF_OKL4_SEGMENT_START, SHF_OKL4_SEGMENT_END
from elf.ByteArray import ByteArray
from elf.util import align_up, align_down
from weaver import MergeError
import weaver.pools
from weaver.allocator import AllocatorItem, AllocatorSimpleItem, TiedItem, \
     AllocatorException
import weaver.machine
from weaver.relocs import apply_relocs
from weaver.util import _0
from pyokl4.autogen import Autogen
from weaver.physreloc import PhysRelocTable
from weaver.util import get_symbol
import weaver.weavertypes

# By default align text segment of relocatable ELF file to a 1M
# boundary.  This will put programs in different 1M boundaries, which
# will improve FASS performance.
DEFAULT_RELOC_ALIGNMENT = 1024 * 1024

# A list of segment types that are not included in the final image.
SKIPPED_TYPES = [
    PT_PAX_FLAGS, # PAX_FLAGS.  Added by the gentoo linker.
    PT_GNU_STACK, # PT_GNU_STACK.  Indicates stack executability.
    PT_MIPS_REGINFO, #
    PT_PHDR,      # PT_PHDR. Entry for header table itself
    PT_NOTE,
    PT_DYNAMIC,
    PT_ARM_EXIDX, # FIXME: Motha issue #2968 - philipd.
    ]

# List of symbols that should not be changed when they're added to the
# image symbol table.
# Probably should be list of regexs, but that's not needed yet.
NO_PREFIX_SYMBOLS = (
    # ARM mapping symbols.
    "$a", # ARM code
    "$t", # THUMB code.
    "$d", # Data items
    "$b", # Paired-instruction (32-bit) branches in Thumb code
    "$f", # function pointer?
    )

def can_prefix_symbol(symbol):
    """
    Return whether or not a symbol should have a prefix added to it in
    the final image.
    """
    return symbol.name not in NO_PREFIX_SYMBOLS

def valid_segment(segment):
    """
    Return whether or not the segment is one that we will process.
    """
    if segment.type in SKIPPED_TYPES:
        return False

    if segment.get_memsz() == 0:
        return False

    if segment.type not in (PT_LOAD, PT_ARM_EXIDX):
        raise MergeError, "Unable to handle segments that aren't " \
              "of type LOAD (found type %#x)." % (segment.type)

    return True

def prefixed_section_name(section_prefix, section_name):
    """
    Return the prefixed form of the name of the non-kernel sections in the
    image.
    """
    # Remove any pathname components from the prefix.
    section_prefix = os.path.basename(section_prefix)

    #strip GNU leading dots in section names
    if section_name and section_name[0] == ".":
        section_name = section_name[1:]

    return "%s.%s" % (section_prefix, section_name)

class ImageAttrs(object):
    """
    Attributes of an item that appears in the ELF image.
    """
    class UnifiedDescriptor(object):
        """
        A Descriptor for attributes that must remain at their default
        values when building an image for a unified address space.

        In a unified address space, virtual addresses are meaningless
        and references to virtual address properties in the XML must
        result in an error.  Unfortunately we don't know if a unified
        image is being built until after the XML has been parsed (the
        information is found in the kernel binary, loaded by
        Kernel.collect_xml(), after XML processing).  In addition, the
        standard idiom for processing XML attributes is:

          attrs.virtpool = getattr(el, 'virtpool', attrs.virtpool)

        Wrapping each of these with a test about the unified address
        space would reduce the clarity of the code.

        The solution is to use this descriptor for the affected
        attributes.  If their value changes from the default then the
        attribute has been set and an error should be raised.

        """
        def __init__(self, attr):
            """
            Attr is the name of the attribute in the instance to store
            the value.
            """
            self.value = None
            self.attr = attr

        def __get__(self, instance, _):
            """
            Get the value of the attribute.
            """
            return getattr(instance, self.attr, None)

        def __set__(self, instance, value):
            """
            Set the value of the attribute.  If part of a unified
            image it is an error to change the attribute to anything
            other than its default value.
            """
            if hasattr(instance, self.attr) and \
                   instance.is_unified and \
                   value != getattr(instance, self.attr):
                # Reverse map from attr name to XML attribute.
                attr_map = {
                    '_virt_addr': 'virt_addr',
                    '_virtpool': 'virtpool',
                    '_direct': 'direct',
                    '_cache_attr': 'cache_policy',
                    '_attach': 'attach',
                    }

                raise MergeError, \
                      'The XML attribute "%s" is not allowed when building ' \
                      'OKL4 Pico images.' % \
                      (attr_map[self.attr])

            setattr(instance, self.attr, value)

        def __delete__(self, instance):
            pass

    class MemType (object):
        """
        Representation of the properties of an object memory type.
        """
        def __init__(self, need_virt, need_phys, need_mapping, need_attaching):
            self.needs_virt = need_virt
            self.needs_phys = need_phys
            self.needs_mapping = need_mapping
            self.needs_attaching  = need_attaching

        def need_virtual(self):
            """
            Return whether or not the objecy needs virtual memory.
            """
            return self.needs_virt

        def need_physical(self):
            """
            Return whether or not the object needs physical memory.
            """
            return self.needs_phys

        def need_mapping(self):
            """
            Return whether or not the object requires a mapping.
            """
            return self.needs_mapping

        def need_attaching(self):
            """
            Return whether or not the object requires attaching to the
            mmu
            """
            return self.needs_attaching

    # Representation of an unmapped object.  Used internally.
    unmapped = MemType(True, True, False, True)

    # Name and virt_addr used to be attributes.  Disable to catch bugs.
    name = property()
    virt_addr = property()
    _virt_addr = property()

    # Attributes that can't be set in unified images.
    virtpool = UnifiedDescriptor('_virtpool')
    direct = UnifiedDescriptor('_direct')
    cache_attr = UnifiedDescriptor('_cache_attr')
    attach = UnifiedDescriptor('_attach')

    def __init__(self, is_unified, ns_node, virtpool, physpool, pager,
                 direct, cache_policy):
        self.is_unified = is_unified
        self.ns_node = ns_node
        self.file = None
        self.data = None
        self.size = None
        self._virt_addrs = {}
        self._default_virt_addr = None
        self.phys_addr = None
        self.direct = direct
        self.virtpool = virtpool
        self.physpool = physpool
        self.align = None
        self.attach = PF_R | PF_W
        self.pager = pager
        self.elf_flags = 0
        self.scrub = False
        self.protected = False
        self.cache_attr = cache_policy if cache_policy else "default"
        self.cache_policy_mask = weaver.machine.FULL_CACHE_PERMS_MASK
        self.mem_type = None
        self.phys_reloc = False
        self.memsec_type = "contiguous"

    def register_sas(self, sas):
        """
        Register that the attributes will be used in the given sas.

        The item can have different virtual addresses in different
        SASs.
        """
        self._virt_addrs.setdefault(sas, self._default_virt_addr)

    def set_default_virt_addr(self, default_virt_addr):
        """
        Set the initial virtual address to use in a sas.

        If the user specifies the virtual address of an object in the XML,
        then that address should be used in all SASs.
        """
        if self.is_unified and default_virt_addr is not None:
            raise MergeError, \
                  'The XML attribute "virt_addr" is not allowed when ' \
                  'building OKL4 Pico images.'

        self._default_virt_addr = default_virt_addr

    def virt_addr_sas(self, sas):
        """Return the virtual address of the item in the given SAS."""
        assert sas is not None
        return self._virt_addrs.get(sas, self._default_virt_addr)

    def set_virt_addr_sas(self, sas, value):
        """Set the virtual address of the item in the given SAS."""
        self._virt_addrs[sas] = value

    def has_virt_addr_sas(self, sas):
        """
        Return whether or not the virtual address attribute has been set in a
        SAS.
        """
        return self.virt_addr_sas(sas) is not None

    def has_phys_addr(self):
        """
        Return whether or not the physical address attribute has been set.
        """
        return self.phys_addr is not None

    def has_fixed_addr(self, is_virtual, sas):
        """
        Return whether or not the given address attribute has been
        set in a SAS.
        """
        if is_virtual:
            return self.has_virt_addr_sas(sas)
        else:
            return self.has_phys_addr()

    def include_in_image(self):
        """
        Return whether or not the item has data and should be included
        in the image.
        """
        return self.data is not None or \
               self.file is not None

    def need_memory(self, is_virtual):
        """
        Return whether or not the object needs virtual or physical
        memory.
        """
        if self.mem_type is None:
            return True

        if is_virtual:
            return self.mem_type.need_virtual()
        else:
            return self.mem_type.need_physical()

    def need_segment(self):
        """
        Return whether or not the object requires a physical segment.
        """
        return self.mem_type is None or \
               self.mem_type.need_physical()

    def need_mapping(self):
        """
        Return whether or not the object requires a mapping.
        """

        return self.mem_type is None or \
               self.mem_type.need_mapping()

    def need_attaching(self):
        """
        Return whether or not the object requires attaching to the mmu.
        """

        return self.mem_type is None or \
               self.mem_type.need_attaching()

    def aligned_addrs_sas(self, machine, sas):
        """
        Return the tuple (virt_addr, phys_addr, size) being the values of
        these attributes rounded to the whole pages that it occupies.  The
        minimum page size of the machine is used.
        """

        # Attrs contains the exact byte address size and address
        # in bytes.  Now round them up to page sizes.
        map_pg_sz = machine.min_page_size()

        virt_addr = align_down(_0(self.virt_addr_sas(sas)), map_pg_sz)
        phys_addr = align_down(_0(self.phys_addr), map_pg_sz)

        # Calculate the shift cause by aligning the phys_addr
        alignment_diff = 0

        if self.has_phys_addr():
            alignment_diff = self.phys_addr - phys_addr

        size = None
        if self.size is not None:
            # In certain cases, phys alignments can leave us a
            # page short. To account for this we add alignment
            # differences to the size.
            size = align_up(self.size + alignment_diff, map_pg_sz)

        return (virt_addr, phys_addr, size)

    def abs_name(self):
        """Return the absolute name of the attribute."""
        return self.ns_node.abs_name()

    def for_sas(self, sas):
        """
        Return a single SAS delegate to this attributes.  The delegate has of
        the existing ImageAttrs interfaces, plus virt_addr and aligned_addrs
        that return values for the SAS.

        This simplifies code the only applies to one SAS, and does not
        otherwise need the SAS object passed to it.
        """
        assert sas in self._virt_addrs

        class AttrsDelegate(object):
            """
            Class to act as a delegate to ImageAttrs, but also provide
            simplier interfaces that only apply to the given SAS.
            """
            # List of attributes that might be assigned to.
            _local_attribs = ('virt_addr', 'aligned_addrs', '_attrs', '_sas')

            def __init__(self, attrs, sas):
                self._attrs = attrs
                self._sas = sas

            def aligned_addrs(self, machine):
                """
                Return the tuple (virt_addr, phys_addr, size) being the values
                of these attributes rounded to the whole pages that it
                occupies.  The minimum page size of the machine is used.
                """
                return self._attrs.aligned_addrs_sas(machine, sas)

            def _get_virt_addr(self):
                return self._attrs.virt_addr_sas(self._sas)

            def _set_virt_addr(self, value):
                self._attrs._virt_addrs[self._sas] = value

            virt_addr = property(_get_virt_addr, _set_virt_addr)

            def __getattr__(self, name):
                return getattr(self._attrs, name)

            def __setattr__(self, name, value):
                if name not in AttrsDelegate._local_attribs:
                    setattr(self._attrs, name, value)
                elif name == 'virt_addr':
                    self._set_virt_addr(value)
                else:
                    self.__dict__[name] = value

        return AttrsDelegate(self, sas)

class ImageAllocatorItem(AllocatorItem):
    """
    Allocator item for ELF image contents.

    An item may have guard pages, which are regions of virtual memory on
    either size of the item, which are not mapped.  To implement this, memory
    for the item and the guard pages is requested from the allocator.  The
    returned address points to the start of the bottom guard page, and this is
    offset before setting the item's address.  Since it is the item's address
    and size that is used for mapping, this ensures that the guard pages are
    never mapped.
    """
    class ImageDirectItem(TiedItem):
        """
        Direct Allocation item for ELF image contents.
        """

        def __init__(self, sas, attrs, is_virtual, guard_page_size):
            TiedItem.__init__(self)

            self.attrs = attrs
            self.is_virtual = is_virtual
            self.sas = sas
            self.guard_page_size = guard_page_size

        def get_addr(self):
            """
            Return the address of the tied item.
            Returns None if the address has not been set.
            """
            if self.is_virtual:
                # With guard pages, tell the allocator that the item starts at
                # the bottom guard page, not the data.
                if self.guard_page_size and self.attrs.virt_addr is not None:
                    return self.attrs.virt_addr - self.guard_page_size
                else:
                    return self.attrs.virt_addr
            else:
                return self.attrs.phys_addr

        def set_addr(self, addr):
            """Set the address of the tied item to the given value."""
            if self.is_virtual:
                # With guard pages, the allocator has supplied the address of
                # the bottom guard page, so adjust it to set the correct
                # address.
                if self.guard_page_size:
                    self.attrs.virt_addr = addr + self.guard_page_size
                else:
                    self.attrs.virt_addr = addr
            else:
                self.attrs.phys_addr = addr

        def get_allocator(self):
            """
            Return the allocator that should be used to allocate the
            tied item's address.
            """
            if self.is_virtual:
                return self.sas.get_virtual_pool_by_name(self.attrs.virtpool)
            else:
                return self.sas.get_physical_pool_by_name(self.attrs.physpool)

    def __init__(self, image_object, sas, is_virtual, guard_page_size = None):
        AllocatorItem.__init__(self)

        self.image_object = image_object
        self._is_virtual = is_virtual
        self.attrs = self.image_object.get_attrs().for_sas(sas)
        self.sas = sas
        self.guard_page_size = guard_page_size

        if self.attrs.direct:
            self.direct_item = \
                             ImageAllocatorItem.ImageDirectItem(sas,
                                                                self.attrs,
                                                                not is_virtual,
                                                                guard_page_size)
        else:
            self.direct_item = None

    def is_virtual(self):
        """Return whether or not a virtual address is being allocated."""
        return self._is_virtual

    def get_name(self):
        """Return a text description of the item."""
        return self.attrs.abs_name()

    def get_size(self):
        """Return the size of the item ."""
        if self._is_virtual and self.guard_page_size:
            # With guard pages, increase the size to include the guard
            # pages so that nothing will be allocated where the guard
            # pages will be.
            return self.attrs.size + 2 * self.guard_page_size
        else:
            return self.attrs.size

    def get_alignment(self):
        """Return the desired alignment of the item."""
        return self.attrs.align

    def get_offset(self):
        """Return the desired offset into the page of the item."""

        # With guard pages, memory for the item and the guard pages are
        # allocated.  However, the alignment applies to the item, not the
        # bottom guard page.  Setting the offset to the negation of the guard
        # page size ensures that this happens.
        guard_offset = -self.guard_page_size \
                       if self._is_virtual and self.guard_page_size else 0

        # If we have a virtual address, calculate the offset...
        if self.attrs.virt_addr is not None:
            return (self.attrs.virt_addr % self.attrs.align) + guard_offset
        # Otherwise place the item at the bottom of the page, modulo guard
        # page.
        else:
            return guard_offset

    def get_cache_attr(self):
        """Return the desired cache attributes."""
        return self.attrs.cache_attr

    def get_tied_item(self):
        """
        Return the item tied to this one, or None if these is no such
        item.
        """
        return self.direct_item

    def get_addr(self):
        """
        Return the address of the object.
        Returns None if the address has not been set.
        """
        if self._is_virtual:
            return self.attrs.virt_addr
        else:
            return self.attrs.phys_addr

    def set_addr(self, addr):
        """Set the address of the object to the given value."""
        if self._is_virtual:
            # With guard pages, the allocator has supplied the address of
            # the bottom guard page, so adjust it to set the correct
            # address.
            if self.guard_page_size:
                self.attrs.virt_addr = addr + self.guard_page_size
            else:
                self.attrs.virt_addr = addr
        else:
            self.attrs.phys_addr = addr

    #def pre_alloc(self, system_default_virt, system_default_phys,
    #              machine, sas):
    def pre_alloc(self, system_default_virt, system_default_phys,
                  machine, _):
        """
        Set attribute defaults that are not known until layout time.
        """
        # If the pools are not known use the default pools.
        if self.attrs.virtpool is None:
            self.attrs.virtpool = system_default_virt

        if self.attrs.physpool is None:
            self.attrs.physpool = system_default_phys

        # Use the natural alignment of the item if none is specified.
        # If unified is enabled, use min page size instead.
        if self.attrs.align is None:
            if self.image_object.unified:
                self.attrs.align = \
                                 machine.min_page_size()
            else:
                self.attrs.align = \
                                 machine.natural_alignment(self.attrs.size)

        assert self.attrs.align is not None

    def post_alloc(self, sas):
        """
        Perform any post processing once the address has been
        calculated.

        This function may be called mulitple times.
        """
        self.image_object.post_alloc(sas)

    def get_pool(self):
        """
        Get the pool associated with this item.
        """
        if self._is_virtual:
            return self.attrs.virtpool
        else:
            return self.attrs.physpool

class ImageSegmentAllocatorItem(ImageAllocatorItem):
    """
    Allocator item for ELF segment image contents.
    """
    def __init__(self, image_object, sas, is_virtual, relocate = False,
                 force_alignment = False, force_page = False):
        ImageAllocatorItem.__init__(self, image_object, sas, is_virtual)

        self.relocate = relocate
        self.force_alignment = force_alignment
        self._force_page = force_page

        # We'll find out the minimum page size in pre_alloc().
        self.min_page_size = None

    def pre_alloc(self, system_default_virt, system_default_phys,
                  machine, sas):
        """
        Set attribute defaults that are not known until layout time.
        """
        self.min_page_size = machine.min_page_size()

        ImageAllocatorItem.pre_alloc(self, system_default_virt,
                                     system_default_phys,
                                     machine, sas)

    def get_alignment(self):
        """Return the desired alignment of the item."""
        # Unless otherwise requested, place the first segment of a
        # relocatable file on a 1M virtual address to improve FASS
        # performance. Ignore if unified is enabled.
        if self._is_virtual and self.relocate and \
               self.force_alignment and not self.image_object.unified:
            return DEFAULT_RELOC_ALIGNMENT
        else:
            return ImageAllocatorItem.get_alignment(self)

    def get_offset(self):
        """Return the desired offset into the page of the item."""

        # If we have a virtual address, calculate the offset...
        if self.relocate:
            return self.image_object.segment.vaddr % self.min_page_size
        else:
            return ImageAllocatorItem.get_offset(self)

    def force_page(self):
        """
        Return whether or not the segment should be allocated on a new
        page even if it would fit in a partial page.
        """
        return self._force_page

class ImageCollection(object):
    """
    A class representing a collection if segments and memsections that are
    related in some way.  The items in the collection are laid out according
    to an allocation policy determined by the collection.
    """

    class SubPageGroup(object):
        """
        A class representing a group of items in the collection that could be
        allocated within the same page.  Normally items are allocated on page
        boundaries and the unused part of the last page is lost.  In subpage
        groups this memory can be used by the next item if the size and
        alignment requirements allow.  This group is useful for ELF segments,
        which can sometimes share a page.

        This class pretends to be both a collection and a single ImageObject
        object.  This is because these objects are stored in a list along side
        single ImageObjects and the masquerade makes the code that uses these
        objects simpler.
        """
        def __init__(self, group):
            self.group = group
            # Alias 'attrs' to ourself to make us look like an ImageObject.
            self.attrs = self

            # Ensure nobody tries to nest SubPageGroups within themselves.
            if [i for i in group
                    if isinstance(i, ImageCollection.SubPageGroup)]:
                raise  MergeError, 'Cannot create a SubPageGroup containing ' \
                        'another SubPageGroup.'

        def __iter__(self):
            """
            Return the group iterator as our own.
            """
            return self.group.__iter__()

        def __getitem__(self, key):
            """
            Indexing this object indexes the internal list.
            """
            return self.group.__getitem__(key)

        def include_in_image(self):
            """
            Return whether or not any item in the group has data and should be
            included in the image.
            """
            return any(i.attrs.include_in_image() for i in self.group)

        def has_fixed_addr(self, is_virtual, sas):
            """
            Return whether or not any item in the list has the given address
            attribute has been set.
            """
            return any(i.attrs.has_fixed_addr(is_virtual, sas)
                       for i in self.group)

        def _get_direct(self):
            """
            Return whether or not any item in the list has the direct
            attribute set.
            """
            return any(i.attrs.direct for i in self.group)

        direct = property(_get_direct)

        def _get_virtpool(self):
            """
            Return a set of all of the virtpools used by items in this
            group.
            """
            return set(i.attrs.virtpool for i in self.group)

        virtpool = property(_get_virtpool)

        def prepare_items(self, system_default_virt, system_default_phys,
                          machine, sas, is_virtual):
            """
            Call the pre_alloc() method on each item in the group.
            Return a list of allocator items, one for each valid
            item in the group.
            """
            alloc_items = []
            for item in self.group:
                alloc_item = item.get_allocator_item(sas,
                                                    is_virtual = is_virtual)

                if alloc_item is not None:
                    alloc_item.pre_alloc(system_default_virt,
                                         system_default_phys,
                                         machine, sas)
                    alloc_items.append(alloc_item)

            return alloc_items

        def get_kernel_heaps(self):
            """
            Return kernel heaps in the group.
            """
            return [i for i in self.group if isinstance(i, ImageKernelHeap)]

        def get_kernel_items(self):
            """
            Return all items in the group that aren't kernel heaps.
            """
            return [i for i in self.group if not isinstance(i, ImageKernelHeap)]

    # For class ImageCollection.
    def __init__(self, image, sas):
        self.image = image
        # The SAS from this addresses for this group will be allocated.
        self.sas = sas
        self.items = []
        self.default_virtpool = None
        self.default_physpool = None

    def sas_attrs(self, attrs):
        """
        Return a ImageAttrs delegate that returns info that applies only to
        the SAS in this this group is allocated.

        The attrs must have been added to this group before this method is
        called.
        """
        return attrs.for_sas(self.sas)

    def get_items(self):
        """
        Return a flat list of all of the items in the group and its subpage
        groups.
        """
        items = []
        for i in self.items:
            if isinstance(i, self.SubPageGroup):
                items.extend(i.group)
            else:
                items.append(i)

        return items

    def prime(self, def_virt_pool, def_phys_pool, is_unified):
        """
        The prime function is responsible for performing any ImageCollection
        manipulation or initialisation before items within the collection are
        allocated addresses.
        """
        self.default_virtpool = def_virt_pool
        self.default_physpool = def_phys_pool

    def alloc(self, items, is_virtual, machine):
        """
        Allocate the given items from the ImageCollection.
        """
        raise NotImplementedError

    def _add_segment(self, segment_index, section_prefix, symbol_prefix,
                    segment, attrs, machine,
                    relocate = False, force_virt_addr = False,
                    force_alignment = False):
        """Create a segment for inclusion in the image."""
        if not valid_segment(segment):
            return None

        is_kernel_section = (section_prefix == "kernel")

        # Prepare the image for inclusion.
        new_segment = segment.copy_into(self.image.elf)

        # Align segments to the page boundary if is safe to do so.
        # RVCT tends to give one word alignment to segments that could
        # be paged aligned and following their alignment can cause
        # problems with some bootloaders.  To avoid this, reset the
        # alighment to a safer value.
        if new_segment.vaddr % machine.min_page_size() == 0 and \
               new_segment.align < machine.min_page_size():
            new_segment.align = machine.min_page_size()

        # Rename the sections in the segment, giving each the supplied
        # prefix
        if new_segment.has_sections():
            for section in new_segment.get_sections():
                assert section.link is None

                orig_section_name = section.name

                section.name = prefixed_section_name(section_prefix,
                                                     section.name)
                # Add the program name as a prefix to all non-kernel
                # symbols, except for those symbols that can't have a
                # prefix added.
                if is_kernel_section:
                    continue

                for _, symbol in self.image.elf.section_symbols(section):
                    if not can_prefix_symbol(symbol):
                        continue

                    # gcc and RVCT both generate a symbol for each section.
                    # Under RVCT the symbol name is the same as the section
                    # name.
                    # Under GCC, the symbol actually has no name.
                    # We map GCC onto RVCT's behaviour for consistency and
                    # so that code in elfweaver can lookup the symbol name
                    # to find the section.
                    if symbol.name == '' and symbol.value == 0:
                        symbol.name = orig_section_name

                    # Note that here we are changing the symbol's name property,
                    # however the dictionary of symbol name -> symbol is not
                    # updated until later.
                    symbol.name = symbol_prefix + symbol.name
                    self.image.elf.get_symbol_table().\
                                   link.add_string(symbol.name)

            new_segment.get_sections()[0].flags |= SHF_OKL4_SEGMENT_START
            new_segment.get_sections()[-1].flags |= SHF_OKL4_SEGMENT_END

        self.image.elf.get_symbol_table().update_symbol_dict()
        self.image.elf.add_segment(new_segment)

        iseg = ImageSegment(new_segment, segment_index,
                            attrs, self.sas, self.image.is_unified,
                            relocate = relocate,
                            force_virt_addr = force_virt_addr,
                            force_alignment = force_alignment)

        if attrs.protected:
            if self.image.protected_segment is not None:
                raise MergeError, \
                      'Only one segment can be declared protected.  ' \
                      'Found "%s" and  "%s".' % \
                      (self.image.protected_segment.get_attrs().abs_name(),
                      attrs.abs_name())

            self.image.protected_segment = iseg

        return iseg

    def add_segment(self, segment_index, section_prefix, symbol_prefix,
                    elf_segment, attrs, machine,
                    relocate = False, force_virt_addr = False,
                    force_alignment = False):
        """
        Add a new ELF segment to the collection.  If <relocate> is True then
        elfweaver will give the segment a new virtual address and the
        contents will be relocated.
        """
        attrs.register_sas(self.sas)

        segment = \
                self._add_segment(segment_index, section_prefix,
                                  symbol_prefix,
                                  elf_segment, attrs,
                                  machine, relocate,
                                  force_virt_addr,
                                  force_alignment)

        if segment is not None:
            self.items.append(segment)

        return segment

    def _add_memsection(self, attrs, machine, section_prefix = None,
                        guard_pages = False,
                        require_cache_colour_alignment = False):
        """
        Create a memsection for inclusion in the image.

        If the data or file attributes of 'attr' are non-None, then a
        ELF segment will be created, otherwise the memsection will be
        created and included in the address layout process, but it
        will not appear in the image.
        """
        new_segment = None

        if attrs.file is not None or attrs.data is not None or \
               attrs.scrub:
            if attrs.scrub:
                if attrs.size is None:
                    raise MergeError, \
                          'No size attribute given for memsection \"%s\".' % \
                          attrs.abs_name()
                data = attrs.size
                section_type = SHT_NOBITS
            else:
                if attrs.file is not None:
                    the_file = open(attrs.file, 'r')
                    data = ByteArray(the_file.read())
                    the_file.close()
                else:
                    data = attrs.data

                if attrs.size is not None and len(data) < attrs.size:
                    data.extend([0] * (attrs.size - len(data)))

                attrs.size = data.buffer_info()[1] * data.itemsize
                section_type = SHT_PROGBITS

            if section_prefix:
                section_name = "%s.%s" % (section_prefix, attrs.ns_node.name)
            else:
                section_name = attrs.ns_node.name

            sect = UnpreparedElfSection(self.image.elf,
                                        section_name, section_type,
                                        attrs.virt_addr_sas(self.sas),
                                        data = data,
                                        flags = (SHF_WRITE | SHF_ALLOC |
                                                 SHF_OKL4_SEGMENT_START |
                                                 SHF_OKL4_SEGMENT_END))
            self.image.elf.add_section(sect)
            new_segment = SectionedElfSegment(self.image.elf, PT_LOAD,
                                              attrs.virt_addr_sas(self.sas),
                                              attrs.phys_addr, PF_R | PF_W,
                                              machine.min_page_size(),
                                              sections=[sect])
            self.image.elf.add_segment(new_segment)
        elif attrs.size is None:
            raise MergeError, \
                'No size attribute given for memsection \"%s\".' % \
                attrs.abs_name()

        guard_page_size = machine.min_page_size() if guard_pages else None
        # if attrs.align is none means it uses natural alignment, init here.
        if attrs.align is None:
            attrs.align = machine.natural_alignment(attrs.size)

        if require_cache_colour_alignment:
            assert(machine.l1_dcache_way_size is not None)
            cache_colour_alignment = machine.l1_dcache_way_size
            if attrs.align < cache_colour_alignment:
                attrs.align = cache_colour_alignment

        obj = ImageMemsection(new_segment, attrs, self.sas,
                              self.image.is_unified,
                              guard_page_size = guard_page_size)

        return obj

    def add_physmem_item(self, attrs, machine, sas):
        attrs.register_sas(self.sas)
        physmem_item = ImagePhysMemItem(attrs, sas, self.image.is_unified)
        self.items.append(physmem_item)

    def add_memsection(self, attrs, machine,
                       section_prefix = None, guard_pages = False,
                       require_cache_colour_alignment = False):
        """
        Add a new memsection to the collection.
        """
        attrs.register_sas(self.sas)

        memsec = self._add_memsection(attrs, machine, section_prefix,
                                      guard_pages,
                                      require_cache_colour_alignment)
        self.items.append(memsec)

        return memsec

    def add_shared_buffer(self, attrs, machine, sas, section_prefix = None):
        """
        Add a new shared buffer to the collection.  Shared buffers act like
        memsections so we just reuse the code.
        """
        attrs.register_sas(self.sas)

        shared_buf = self._add_memsection(attrs, machine, section_prefix)
        self.items.append(shared_buf)

        return shared_buf

    def add_object(self, attrs):
        """
        """
        attrs.register_sas(self.sas)

        imgobj = ImageObject(attrs, self.image.is_unified)
        self.items.append(imgobj)

        return imgobj

    def add_subpage_group(self, item_list):
        """
        Add a collection of items that can be allocated within a page if
        necessary.

        The items in the list must have already been added to the group and
        must all be allocated from the same pool.
        """
        # XXX: Need to check allocation compatibility.
        assert len(item_list) > 0
        for item in item_list:
            self.items.remove(item)

        self.items.append(self.SubPageGroup(item_list))

    def add_absolute_symbols(self, abs_symbols, symbol_prefix):
        """
        Adds a set of absolute symbols to the collection.
        """
        for idx, symbol in abs_symbols:
            # Note that undefined weak symbols can be treated as if they were
            # absolute symbols. They should always have a value of 0.
            assert symbol.shndx == SHN_ABS or \
                (symbol.shndx == SHN_UNDEF and symbol.bind == STB_WEAK and \
                 symbol.value == 0)

            if not can_prefix_symbol(symbol):
                raise MergeError, \
                    "Cannot prefix absolute symbol %s" % symbol.name

            new_sym = symbol.copy_into(None)
            new_symdx = self.image.elf.add_symbol(new_sym)
            old_symdx = idx

            self.image.elf.reloc_merge_symbols_temp[old_symdx] = new_symdx

            new_sym.name = symbol_prefix + new_sym.name
            self.image.elf.get_symbol_table().\
                           link.add_string(new_sym.name)

        self.image.elf.get_symbol_table().update_symbol_dict()


    def get_fixed_items(self, is_virtual):
        """
        Return a list of all Collection Items with fixed addresses.
        """
        return [i for i in self.items
                if i.attrs.has_fixed_addr(is_virtual, self.sas)]

    def get_direct_items(self):
        """
        Return a list of all Collection Items which are direct mapped.
        """
        return [i for i in self.items if i.attrs.direct]

    def get_unallocated_items(self, is_virtual):
        """
        Return a list of all Collection Items which do not have fixed
        addresses or are direct mapped.
        """
        return [i for i in self.items
                if not (i.attrs.has_fixed_addr(is_virtual, self.sas) or
                        i.attrs.direct)]

    def get_items_in_image(self):
        """
        Return a list of all Collection Items which have data contents.
        """
        return [i for i in self.items if i.attrs.include_in_image()]

    def get_items_not_in_image(self):
        """
        Return a list of all Collection Items which do not have data
        contents.
        """
        return [i for i in self.items if not i.attrs.include_in_image()]

    def get_virtpools(self):
        """
        Return a set of all of the virtpools used by items in this collection.
        """
        vset = set(i.attrs.virtpool for i in self.items
                   if not isinstance(i, self.SubPageGroup))

        for item in self.items:
            if isinstance(item, self.SubPageGroup):
                vset |= item.attrs.virtpool

        return vset

class Group(ImageCollection):
    """
    An image collection with no particular allocation policy for its
    contents.
    """

    def alloc(self, items, is_virtual, machine):
        """
        Allocate addresses for items in the collection.
        """
        pool_name = "Unknown"

        for item in items:
            if isinstance(item, self.SubPageGroup):
                # Collect alloc items from the group and pre_alloc() them
                alloc_items = item.prepare_items(self.default_virtpool,
                                                 self.default_physpool,
                                                 machine, self.sas, is_virtual)

                the_pool, pool_name = item[0].get_pool(self.sas, is_virtual)
                try:
                    the_pool.alloc_group(alloc_items)
                except AllocatorException, text:
                    raise MergeError, \
                          '%s pool "%s": %s: %s' % \
                          ({True: "Virtual",
                            False: "Physical"}[is_virtual],
                           pool_name, alloc_items[0].get_name(), text)

                for alloc_item in alloc_items:
                    alloc_item.post_alloc(self.sas)
            else:
                # Don't try to allocate an item not in the collection!
                alloc_item = \
                           item.get_allocator_item(self.sas,
                                                   is_virtual = is_virtual)
                # Check that the item is actually expecting a virt/phys
                # address
                if alloc_item is not None:
                    alloc_item.pre_alloc(self.default_virtpool,
                                         self.default_physpool,
                                         machine, self.sas)

                    the_pool, pool_name = item.get_pool(self.sas, is_virtual)
                    try:
                        the_pool.alloc(alloc_item)
                    except AllocatorException, text:
                        raise MergeError, \
                              '%s pool "%s": %s: %s' % \
                              ({True: "Virtual",
                                False: "Physical"}[is_virtual],
                               pool_name, alloc_item.get_name(), text)

                    alloc_item.post_alloc(self.sas)


class KernelGroup(Group):
    """
    An extension of the regular ImageCollection Group class specifically
    for containing kernel objects.
    """

    def add_kernel_heap(self, kernel_heap, text_segment):
        """
        Register a kernel heap with the group.
        """
        kernel_heap.get_attrs().register_sas(self.sas)

        self.items.append(self.SubPageGroup([text_segment, kernel_heap]))

    def get_kernel_heaps(self):
        """
        Return kernel heaps in the group.
        """
        heaps = []
        for i in self.items:
            if isinstance(i, ImageKernelHeap):
                heaps.append(i)
            elif isinstance(i, self.SubPageGroup):
                heaps.extend(i.get_kernel_heaps())
        return heaps

    def get_kernel_items(self):
        """
        Return all items in the group that aren't kernel heaps.
        """
        items = []
        for i in self.items:
            if isinstance(i, self.SubPageGroup):
                items.extend(i.get_kernel_items())
            elif not isinstance(i, ImageKernelHeap):
                items.append(i)
        return items

class RegionedGroup(Group):
    """
    This is a base class for Realms and Zones to derive from.
    At the moment, they are sufficiently similar at the ImageCollection
    class level to share priming and allocation methods.
    """
    def __init__(self, image, sas, region_pool, type_):
        Group.__init__(self, image, sas)

        self.source_pool = None
        self.region_pool = region_pool
        self.type = type_
        self.default_virtpool = None
        self.default_physpool = None

    def prime(self, system_default_virt, system_default_phys,
              is_unified):
        """
        Remove already allocated items from the zone/realm allocation pool.
        """
        self.default_virtpool = system_default_virt
        self.default_physpool = system_default_phys

        # Priming only makes sense if we will be doing virtual allocations
        if is_unified:
            return

        # Determine what our source virtpool should be...
        # All items in the RegionedGroup should have the same virtpool,
        # in the worst case we simply use the default pool.
        if self.items:
            first_item = self.items[0]
            if isinstance(first_item, self.SubPageGroup):
                self.source_pool = first_item[0].attrs.virtpool
            else:
                self.source_pool = first_item.attrs.virtpool
        else:
            self.source_pool = self.default_virtpool

        ranges = []
        for item in self.items:
            if isinstance(item, self.SubPageGroup):
                for i in item:
                    realm_range = i.get_prime_range(self.sas)

                    if realm_range is not None:
                        ranges.append(realm_range)
            else:
                realm_range = item.get_prime_range(self.sas)

                if realm_range is not None:
                    ranges.append(realm_range)

        self.region_pool.prime(self.source_pool, ranges, self.sas)

    def alloc(self, items, is_virtual, machine):
        """
        Allocate addresses for items in the collection.  The allocation policy
        for the object is stored in the associated pool subclass.
        """

        # There are no constraints on the physical allocations of items
        # in a Realm/Zone, let the regular Group allocator do its thing.
        if not is_virtual:
            Group.alloc(self, items, is_virtual, machine)
            return

        # Subpage groups get allocated first, in the order they were added!
        for group in [i for i in items if isinstance(i, self.SubPageGroup)]:
            alloc_items = group.prepare_items(self.default_virtpool,
                                              self.default_physpool,
                                              machine, self.sas, is_virtual)
            try:
                self.region_pool.alloc_group(alloc_items)
            except AllocatorException, text:
                raise MergeError, 'Virtual Pool: "%s": %s: %s' % \
                      (self.region_pool.source_pool.name,
                       alloc_items[0].get_name(), text)

            for ent in alloc_items:
                ent.post_alloc(self.sas)

        # Now we allocate the remained of the items as usual
        alloc_items = []
        for item in [i for i in items if not isinstance(i, self.SubPageGroup)]:
            alloc_item = item.get_allocator_item(self.sas, is_virtual = True)
            if alloc_item is not None:
                alloc_items.append(alloc_item)
                alloc_item.pre_alloc(self.default_virtpool,
                                     self.default_physpool,
                                     machine, self.sas)

        for ent in alloc_items:
            try:
                self.region_pool.alloc(ent)
            except AllocatorException, text:
                raise MergeError, 'Virtual Pool: "%s": %s: %s' % \
                      (self.region_pool.source_pool.name,
                       ent.get_name(), text)

            ent.post_alloc(self.sas)

class Realm(RegionedGroup):
    """
    A collection in which everything is stored in a collection of 1M regions.
    """
    def __init__(self, image, sas, region_pool, env_realm):
        RegionedGroup.__init__(self, image, sas, region_pool, "Realm")

        self.env_realm = env_realm

class Zone(RegionedGroup):
    """
    A collection in which everything is stored in a continuous collection of
    1M regions.
    """
    def __init__(self, image, sas, region_pool):
        RegionedGroup.__init__(self, image, sas, region_pool, "Zone")

class ImageObject(object):
    """Description of objects that end up in the image."""

    def __init__(self, attrs, unified):
        self.attrs = attrs
        self.unified = unified

    def get_prime_range(self, sas):
        """
        Return the virtual address range of the object needed for realm
        priming.
        """
        virt_addr = self.attrs.virt_addr_sas(sas)

        if virt_addr is not None:
            return (virt_addr, self.attrs.size)
        else:
            return None

    def get_allocator_item(self, sas, is_virtual):
        """
        Return an object used to allocate a virtual or physical
        address for the item, or None if such allocation is not
        meaningful.

        is_virtual:  Indicate whether or nor an allocator item for the
        virtual or physical address is being requested.
        """
        return ImageAllocatorItem(self, sas, is_virtual)

    def get_attrs(self):
        """Return the image attributes of the object."""
        return self.attrs

    def post_alloc(self, sas):
        """
        Called after the memory has been allocated for the item.

        This method may be called multiple times are the virtual and
        physical addresses are allocated.
        """
        pass

    def get_pool(self, sas, is_virtual):
        """
        Find the pool backing this object and return both the
        pool and its name.
        """
        if is_virtual:
            pool_name = self.attrs.virtpool
            pool = sas.get_virtual_pool_by_name(pool_name)
        else:
            pool_name = self.attrs.physpool
            pool = sas.get_physical_pool_by_name(pool_name)

        return pool, pool_name

class ImageSegment(ImageObject):
    """ELF Segment for the image."""

    def __init__(self, segment, segment_index,
                 attrs, sas, unified, relocate = False,
                 force_virt_addr = False, force_alignment = False):
        ImageObject.__init__(self, attrs, unified)

        self.segment = segment
        self.segment_index = segment_index
        self.attrs.size = segment.get_memsz()
        self.relocate = relocate
        self.force_virt_addr = force_virt_addr
        self.force_alignment = force_alignment
        self.force_page = False
        self.attrs.file = True
        self.attrs.memsec_type = attrs.memsec_type

        # Declare the segment's physical memory range in use.
        marked = sas.mark_physical(self.attrs.abs_name(),
                                   self.attrs.phys_addr,
                                   self.attrs.size,
                                   self.attrs.cache_attr)

        if self.attrs.phys_addr is not None and not marked:
            raise MergeError, \
                  'Segment "%s": Cannot reserve physical addresses ' \
                  '%#x--%#x.' % \
                  (self.attrs.abs_name(), self.attrs.phys_addr,
                   self.attrs.phys_addr + self.attrs.size - 1)

        if self.force_virt_addr:
            vaddr = self.attrs.virt_addr_sas(sas)
        else:
            vaddr = segment.vaddr

        # If it's a protected segment, reserve everything in the same SECTION
        if self.attrs.protected:
            base = align_down(vaddr, 1024 * 1024)
            size = align_up(self.attrs.size + base - vaddr, 1024 * 1024)
        else:
            base = vaddr
            size = self.attrs.size

        # Relocatable segments need to discard any virtual addresses
        # that are found in the segment as they will be allocated a
        # new virtual address later.
        # Otherwise we can mark the virtual address since it's not
        # going to change.
        if not self.relocate:
            sas.mark_virtual(self.attrs.abs_name(), base, size,
                             self.attrs.cache_attr)
            if unified:
                sas.mark_physical(self.attrs.abs_name(), base, size,
                                self.attrs.cache_attr)
                self.attrs.phys_addr = base
        else:
            self.attrs.set_virt_addr_sas(sas, None)

    def set_force_page(self, force_page):
        """
        Set whether or not the segment should be allocated on a new
        page even if it would fit in a partial page.
        """
        self.force_page = force_page

    def get_allocator_item(self, sas, is_virtual):
        """Return allocator items for the segment."""
        if is_virtual and not self.unified:
            return ImageSegmentAllocatorItem(self, sas, is_virtual = True,
                                             relocate = self.relocate,
                                             force_alignment = self.force_alignment,
                                             force_page = self.force_page)
        else:
            return ImageSegmentAllocatorItem(self, sas, is_virtual = False,
                                             relocate = self.relocate,
                                             force_alignment = self.force_alignment,
                                             force_page = self.force_page)

    def post_alloc(self, sas):
        """
        Set the segment's physical address once allocation is
        complete.
        """
        if self.attrs.phys_addr is not None:
            self.segment.paddr = self.attrs.phys_addr

        # If the image is unified, we simply copy all physical addresses
        if self.unified:
            for sect in self.segment.sections:
                # Update the sections vaddrs
                sect.address = (sect.address - self.segment.vaddr) + \
                               self.segment.paddr

            # Finally update the segment vaddr
            self.segment.vaddr = self.segment.paddr
            # Set the internal form of virt_addr because a) too much
            # kernel init code requires a virt addr even when unified,
            # and b) it is safe because it is no longer a user error
            # to set this value.
            self.attrs.set_virt_addr_sas(sas, self.attrs.phys_addr)

        # Now we want to set the virutal address, this may be different to the
        # addresses already assigned to the sections in the segment.  Figure
        # out the difference from the current virt_addr and apply that to
        # all sections contained in the segment.
        elif self.attrs.virt_addr_sas(sas) is not None:
            virt_diff = self.segment.vaddr - self.attrs.virt_addr_sas(sas)

            self.segment.vaddr = self.attrs.virt_addr_sas(sas)

            for sect in self.segment.sections:
                sect.address -= virt_diff

class ImagePhysMemItem(ImageObject):
    """Physmem item for the image"""
    def __init__(self, attrs, sas, unified):
        ImageObject.__init__(self, attrs, unified)
        self.segment = None
        self.unified = unified

        sas.mark_physical(attrs.abs_name(),
                          self.attrs.phys_addr,
                          self.attrs.size, self.attrs.cache_attr)

class ImageMemsection(ImageObject):
    """
    Memsection for the image.

    Memsections may have guard pages, which are unmapped pages at both ends of
    the memsection.  This is primarily used to detect stack and heap
    overruns.  See ImageAllocatorItem for the implementation of guard pages.
    """
    def __init__(self, segment, attrs, sas, unified, guard_page_size):
        ImageObject.__init__(self, attrs, unified)

        self.segment = segment
        self.guard_page_size = guard_page_size

        # If the memsection has a fixed address and guard pages, then mark the
        # guard pages before the memsection itself is marked.
        if guard_page_size and self.attrs.virt_addr_sas(sas) is not None:
            name = attrs.abs_name() + '_lower_guard_page'
            virt_addr = self.attrs.virt_addr_sas(sas) - guard_page_size

            sas.mark_virtual(attrs.abs_name(),
                             virt_addr,
                             guard_page_size,
                             self.attrs.cache_attr)

            name = attrs.abs_name() + '_upper_guard_page'
            virt_addr = self.attrs.virt_addr_sas(sas) + \
                        self.attrs.size

            sas.mark_virtual(attrs.abs_name(),
                             virt_addr,
                             guard_page_size,
                             self.attrs.cache_attr)

        sas.mark_virtual(attrs.abs_name(),
                         self.attrs.virt_addr_sas(sas),
                         self.attrs.size, self.attrs.cache_attr)
        sas.mark_physical(attrs.abs_name(),
                          self.attrs.phys_addr,
                          self.attrs.size, self.attrs.cache_attr)

    def get_prime_range(self, sas):
        """
        Return the virtual address range of the object needed for realm
        priming.
        """

        virt_addr = self.attrs.virt_addr_sas(sas)
        # Because this range is passed back to the allocator for comparison
        # with its freelist, the true allocation values need to be returned.
        if virt_addr and self.guard_page_size:
            return (virt_addr - self.guard_page_size,
                    self.attrs.size + 2 * self.guard_page_size)
        else:
            return ImageObject.get_prime_range(self, sas)

    def get_allocator_item(self, sas, is_virtual):
        """
        Return the allocator items for the memsection.
        """
        if self.attrs.need_memory(is_virtual):
            return ImageAllocatorItem(self, sas, is_virtual = is_virtual,
                                      guard_page_size = self.guard_page_size)
        else:
            return None

    def post_alloc(self, sas):
        """
        Update the segment details (if necessary) after the
        memory allocation.
        """
        if self.unified:
            # Set the internal form of virt_addr because a) too much
            # kernel init code requires a virt addr even when unified,
            # and b) it is safe because it is no longer a user error
            # to set this value.
            self.attrs.set_virt_addr_sas(sas, self.attrs.phys_addr)

        if self.segment:
            self.segment.paddr = self.attrs.phys_addr
            self.segment.vaddr = \
                               self.attrs.virt_addr_sas(sas) if not \
                               self.unified else self.attrs.phys_addr
            self.segment.sections[0].address = self.attrs.virt_addr_sas(sas)

class ImageKernelHeap(ImageObject):
    """ Kernel Heap backed by segment for the image. """

    def __init__(self, attrs, machine, unified, kernel_seg):
        ImageObject.__init__(self, attrs, unified)

        self.segment = None
        self.kernel_segment = kernel_seg
        self.made_segment = False
        self.machine = machine

    def get_allocator_item(self, sas, is_virtual):
        """
        Return an allocator for physical memory only.  The virtual
        address of the heap is set up in post_alloc().
        """
        # Return an allocator item only for physical addresses.
        if is_virtual:
            return None
        else:
            return ImageAllocatorItem(self, sas, is_virtual = False)

    def post_alloc(self, sas):
        """
        Update the kernel heap values after the allocation.
        """
        # Set the virtual address of the heap.  The virtual address must be
        # above the text region and the difference between the virt and phys
        # addresses *must* be the same as the difference for the kernel's
        # first segment.
        #
        # XXX: This requires the kernel to be linked at 0xff00000, and outside
        # the virtual pool range.
        # XXX: These constraints should be managed by the allocator.
        if self.unified:
            # Set the internal form of virt_addr because a) too much
            # kernel init code requires a virt addr even when unified,
            # and b) it is safe because it is no longer a user error
            # to set this value.
            self.attrs.set_virt_addr_sas(sas, self.attrs.phys_addr)

            if self.segment:
                self.segment.paddr = self.attrs.phys_addr
                self.segment.vaddr = self.attrs.phys_addr
                self.segment.sections[0].address = self.attrs.phys_addr
        else:

            # Adjust the size in include the last whole of the 1M page
            # containing the heap.  The kernel text and heap are
            # mapped with 1M pages for efficiency and we don't want
            # other kernel objects allocated in the regions.
            size = (align_up(self.attrs.phys_addr + self.attrs.size,
                             0x100000) -
                    self.attrs.phys_addr)

            # Calculate the correct virtual address.
            self.attrs.set_virt_addr_sas(sas, self.kernel_segment.vaddr + \
                                         (self.attrs.phys_addr -
                                          self.kernel_segment.paddr))

            if not sas.mark_virtual(self.attrs.abs_name(),
                                    self.attrs.virt_addr_sas(sas),
                                    size,
                                    self.attrs.cache_attr):
                raise MergeError, \
                      "Cannot allocate kernel heap at virtual address " \
                      "%#x--%#x.\n" \
                      "This is an internal error, but as a work around " \
                      "please try and reduce the size of your image." % \
                      (self.attrs.virt_addr_sas(sas),
                       self.attrs.virt_addr_sas(sas) + size - 1)

            # Mark off the 1M page from the bottom of the kernel to
            # prevent devices being allocated there and producing
            # clashing mappings.  Turn off overlap checks in this
            # case.
            sas.mark_virtual("Kernel Text 1M Mapping",
                             Autogen.machine_info.KERNEL_VADDR_BASE,
                             0x100000,
                             self.attrs.cache_attr,
                             can_overlap = True)

            if self.segment:
                self.segment.paddr = self.attrs.phys_addr
                self.segment.vaddr = self.attrs.virt_addr_sas(sas)
                self.segment.sections[0].address = self.attrs.virt_addr_sas(sas)


class Image(object):
    """Representation of the contents of the final image."""

    class Patch(object):
        """A Class for describing patches to segments."""

        class DiffSymSeg(object):
            """Offset using the difference between a symbol and segment"""
            def __init__(self, sym, seg):
                self.sym = sym
                self.seg = seg

            def get_diff(self):
                """
                Return the difference between the symbol value and
                segment base address.
                """
                return self.sym.value - self.seg.vaddr

        def __init__(self, sym, value, elf, size = None, addr_offset = 0,
                     value_offset = 0):
            assert sym is not None
            assert value is not None

            self.sym = sym
            self.value = value
            self.elf = elf
            self.size = size
            self.addr_offset = addr_offset
            self.value_offset = value_offset

        def _get_section(self):
            """Return the section containing the patch's symbol."""
            return self.sym.section

        section = property(_get_section)

        def get_addr(self):
            """Return the address to be patched."""
            addr = self.sym.value

            if self.elf.machine == ElfMachine(8):
                if self.elf.flags & EF_MIPS_ABI_O64:
                    if addr & 0x80000000:
                        addr |= 0xffffffff00000000L

            return addr + self.addr_offset

        def get_size(self):
            """Return the size of the object being patched."""
            if self.size != None:
                return self.size

            if self.sym.size == 0:
                return self.elf.wordsize / 8

            return self.sym.size

        def get_value_offset(self):
            """
            Return the difference between the value and the segment it
            was originally in.
            """
            if isinstance(self.value_offset, self.DiffSymSeg):
                #pylint: disable-msg=E1103
                return self.value_offset.get_diff()
            else:
                return self.value_offset

    class AttrStack:
        """
        Class for holding a stack of attribute values.  Virtpool,
        physpool and pagers operate in that way.

        These stacks differ from regular stacks in that the top of the
        stack is defined to be the last non-None entry in the list.
        """
        def __init__(self):
            self.stack = []
            self.top = None

        def __getitem__(self, index):
            return self.stack[index]

        def set(self, value):
            """
            Set the stack to only contain the given value.
            """
            assert value is not None

            self.stack = [value]
            self.top = 0

            assert self.top is None or self.top < len(self.stack)

        def push(self, value):
            """
            Push an item onto the stack.  If the item is not None,
            then then will become the top of the stack.
            """
            self.stack.append(value)

            if value is not None:
                self.top = len(self.stack) - 1

                if self.top < 0:
                    self.top = None

            assert self.top is None or \
                   (self.top < len(self.stack) and \
                    self.stack[self.top] is not None)

        def pop(self):
            """
            Pop on item off the stack.  If the item is non-None,
            then the top of the stack is moved to the last non-None
            value in the list.
            """
            value = self.stack.pop()

            if value is not None:
                # Recalculate the top of the stack.
                self.top = None
                for i, item in enumerate(self.stack):
                    if item is not None:
                        self.top = i

                assert self.top is None or \
                       self.stack[self.top] is not None

            assert self.top is None or self.top < len(self.stack)

        def tos(self):
            """
            Return the item at the top of the stack, or None if there
            is no such item.
            """
            if self.top is None:
                return None
            else:
                return self.stack[self.top]

        def bot(self):
            """
            Return the item at the bottom of the stack, or None if the
            stack is empty.
            """
            if self.stack:
                return self.stack[0]
            else:
                return None

    objects = property()

    def __init__(self, ph_offset):
        self.ph_offset = ph_offset
        self.kernel_heap = None
        self.zones = []
        self.elf = None
        self.constituent_elfs = []
        self.endianess = None
        self.wordsize = None
        self.patches = []
        # The following are the attribute stack
        self.virt_pool_stack = Image.AttrStack()
        self.phys_pool_stack = Image.AttrStack()
        self.pager_stack = Image.AttrStack()
        self.direct_stack = Image.AttrStack()
        self.cache_policy_stack = Image.AttrStack()
        # The following are the scheduler parameters stack
        self.priority_stack = Image.AttrStack()
        self.timeslice_stack = Image.AttrStack()
        self.affinity_stack = Image.AttrStack()

        self.protected_segment = None
        self.collections = []
        # The following flag controls whether the image should be
        # built for a unified address space.  This should be set
        # correctly before any items are added to the image.  Unified
        # address spaces are required for systems without a MMU.
        self.is_unified = False

        self.reloc_threads_start = []
        self.reloc_threads_entry = []
        self.reloc_threads_init_func = []

        self.phys_reloc_table = PhysRelocTable()

    def get_symbol(self, prefix, sym_name):
        """
        Return the symbol from the image with given prefix and name.

        This is used to find symbols in non-kernel parts of the image,
        which have had their name prefixed to avoid clashes with
        kernel symbols.
        """
        return self.elf.find_symbol(prefix + '-' + sym_name)

    def current_pools(self):
        """Return the current virtual and physical pools."""
        return (self.virt_pool_stack.tos(),
                self.phys_pool_stack.tos())

    def new_attrs(self, ns_node, for_segment = False):
        """
        Create a new attribute object.

        The attributes are initialised with the current values from
        the attribute stack and the supplied namespace.
        """
        def_direct = False

        if for_segment:
            def_direct = self.direct_stack.tos()

        return ImageAttrs(is_unified = self.is_unified,
                          ns_node = ns_node,
                          virtpool = self.virt_pool_stack.tos(),
                          physpool = self.phys_pool_stack.tos(),
                          pager = self.pager_stack.tos(),
                          direct = def_direct,
                          cache_policy = self.cache_policy_stack.tos())

    def set_attrs_stack(self, def_virt = None, def_phys = None,
                        def_pager = None, def_direct = None,
                        def_priority = None, def_timeslice = None,
                        def_affinity = None, def_cache_policy = None):
        """
        Prime the attribute stack with initial values.
        """
        if def_virt is not None:
            if self.is_unified:
                raise MergeError, \
                      'The XML attribute "virtpool" is not allowed ' \
                      'when building OKL4 Pico images.'

            self.virt_pool_stack.set(def_virt)

        if def_phys is not None:
            self.phys_pool_stack.set(def_phys)

        if def_pager is not None:
            self.pager_stack.set(def_pager)

        if def_direct is not None:
            if self.is_unified:
                raise MergeError, \
                      'The XML attribute "direct" is not allowed ' \
                      'when building OKL4 Pico images.'

            self.direct_stack.set(def_direct)

        if def_priority is not None:
            self.priority_stack.set(def_priority)

        if def_timeslice is not None:
            self.timeslice_stack.set(def_timeslice)

        if def_affinity is not None:
            self.affinity_stack.set(def_affinity)

        if def_cache_policy is not None:
            self.cache_policy_stack.set(def_cache_policy)

    def push_attrs(self, virtual = None, physical = None,
                   pager = None, direct = None,
                   priority = None, timeslice = None,
                   affinity = None, cache_policy = None):
        """Push values onto the attribute and scheduler params stacks."""
        if virtual is not None and self.is_unified:
            raise MergeError, \
                  'The XML attribute "virtpool" is not allowed ' \
                  'when building OKL4 Pico images.'

        if direct is not None and self.is_unified:
            raise MergeError, \
                  'The XML attribute "direct" is not allowed ' \
                  'when building OKL4 Pico images.'

        self.virt_pool_stack.push(virtual)
        self.phys_pool_stack.push(physical)
        self.pager_stack.push(pager)
        self.direct_stack.push(direct)
        self.priority_stack.push(priority)
        self.timeslice_stack.push(timeslice)
        self.affinity_stack.push(affinity)
        self.cache_policy_stack.push(cache_policy)

    def pop_attrs(self):
        """Pop values from the attribute and scheduler params stacks."""
        self.virt_pool_stack.pop()
        self.phys_pool_stack.pop()
        self.pager_stack.pop()
        self.direct_stack.pop()
        self.priority_stack.pop()
        self.timeslice_stack.pop()
        self.cache_policy_stack.pop()

    def get_default_sched_params(self):
        """Return the current values from the scheduler parameters stack."""
        return (self.priority_stack.tos(),
                self.timeslice_stack.tos(),
                self.affinity_stack.tos(),
                )

    def prepare(self):
        """Prepare the ELF file for writing to disk."""
        self.elf = self.elf.prepare(self.wordsize, self.endianess)

    def write_out_image(self, output_file):
        """Write out the final ELF file."""
        self.elf.to_filename(output_file)

    def allocate_ungrouped_items(self, items, is_virtual, machine):
        """
        During layout we are required to allocate items which come from
        disparate collections, this method allows us to safely allocate
        these items.
        """

        for (sas, i) in items:
            if isinstance(i, ImageCollection.SubPageGroup):
                raise MergeError, "SubPageGroups that don't exist in the " \
                        "image are not yet supported!"

        alloc_items = [(sas, i.get_allocator_item(sas, is_virtual = is_virtual))
                       for (sas, i) in items]
        alloc_items = [(sas, i) for (sas, i) in alloc_items if i is not None]

        # Sort items by decreasing order of alignment size
        alloc_items.sort(key=lambda (sas, i): i.get_alignment(), reverse=True)

        for sas, ent in alloc_items:
            ent.pre_alloc(self.virt_pool_stack.bot(),
                          self.phys_pool_stack.bot(),
                          machine, sas)

            if is_virtual:
                the_pool = sas.get_virtual_pool_by_name(ent.get_pool())
            else:
                the_pool = sas.get_physical_pool_by_name(ent.get_pool())
            try:
                the_pool.alloc(ent)
            except AllocatorException, ex:
                raise MergeError, ("Error allocating memory item %s " \
                    "in pool %s.\n\tDetails: %s") % \
                    (ent.get_name(), ent.get_pool(), ex)

            ent.post_alloc(sas)

    def layout(self, machine):
        """Layout the image in memory."""
        #======================================
        # Proper allocation algorithm...
        #======================================

        #======================================
        # Preparations

        # 1) Prime any Realms or Zones
        for obj in self.collections:
            obj.prime(self.virt_pool_stack.bot(),
                      self.phys_pool_stack.bot(),
                      self.is_unified)

        # 2) Separate kernel groups and regular collections
        # since we allocate kernel groups last
        kernel_groups = []
        collections = []
        for colln in self.collections:
            if isinstance(colln, KernelGroup):
                kernel_groups.append(colln)
            else:
                collections.append(colln)

        # 3) Allocate the physical addresses for kernel items except
        #    the kernel heap.  The kernel text needs to be at the
        #    bottom of its physical pool, so give it first choice.
        #    The other items are device mappings, so allocating them
        #    now won't change the image layout.
        for group in kernel_groups:
            kernel_items = group.get_kernel_items()
            group.alloc(kernel_items, False, machine)

        if not self.is_unified:
            #======================================
            # Virtual allocations:

            # 4) Allocate items with fixed addresses
            for colln in collections:
                fixed_items = colln.get_fixed_items(is_virtual=True)
                colln.alloc(fixed_items, True, machine)

            # 5) Allocate any direct addressed items
            for colln in collections:
                fixed_items = colln.get_direct_items()
                colln.alloc(fixed_items, True, machine)

            # 6) Allocate remaining Zone items
            zones = [c for c in collections if isinstance(c, Zone)]
            for zone in zones:
                zone.alloc(zone.get_unallocated_items(True), True,
                           machine)

            # 7) Allocate remaining Realm items
            realms = [c for c in collections if isinstance(c, Realm)]
            for realm in realms:
                realm.alloc(realm.get_unallocated_items(True), True,
                            machine)

            # 8) Allocate remaining Group items
            groups = [c for c in collections if isinstance(c, Group)]
            for group in groups:
                group.alloc(group.get_unallocated_items(True), True,
                            machine)


        #======================================
        # Physical allocations:

        # 1) Allocate fixed items (including direct)
        for colln in collections:
            fixed_items = colln.get_fixed_items(is_virtual=False)
            colln.alloc(fixed_items, False, machine)

        # 2) Allocate kernel heap/s after all other items which will end up
        #    in the final image have been allocated.  This is done
        #    first to ensure that the heap is within 16M of the kernel
        #    text.
        if not self.is_unified:
            for group in kernel_groups:
                heaps = group.get_kernel_heaps()
                # We only explicitly allocate a physical address for kernel
                # heaps since the post_alloc routine sets the virtual address
                group.alloc(heaps, False, machine)

        # 3) Allocate items that have data and will end up in the final image
        for colln in collections:
            in_image = colln.get_items_in_image()
            colln.alloc(in_image, False, machine)

        # 3a) For the unified case, allocate the kernel heap AFTER the
        #     rest of the image.  The Pico SDK relies on this
        #     behaviour so that the user has a constant address to
        #     link their programs at.
        if self.is_unified:
            for group in kernel_groups:
                heaps = group.get_kernel_heaps()
                # We only explicitly allocate a physical address for kernel
                # heaps since the post_alloc routine sets the virtual address
                group.alloc(heaps, False, machine)

        if not self.is_unified:
            # 4) Now allocate the virtual addresses of any remaining
            #    kernel items.  The kernel has a fixed virtual
            #    address, and is not relocatable, but we still need to
            #    allocate kernel devices (and any other possible
            #    objects).
            for group in kernel_groups:
                kernel_items = group.get_kernel_items()
                group.alloc(kernel_items, True, machine)

        # Now allocate all remaining items that have physical addresses but
        # don't end up in the final image...

        # 5) Allocate remaining Zone, Realms and Group items
        unalloced = []
        for colln in collections:
            for una in colln.get_unallocated_items(False):
                unalloced.append((colln.sas, una))

        self.allocate_ungrouped_items(unalloced, False, machine)

        # Update and apply any relocations
        apply_relocs(self.elf, machine)

        # And finally we need to update any threads so their entry point is at
        # the correct position for the movod section
        for thread, sect, old_addr in self.reloc_threads_start:
            thread.start -= old_addr - sect.address
        for thread, sect, old_addr in self.reloc_threads_entry:
            thread.entry -= old_addr - sect.address
        for thread, sect, old_addr in self.reloc_threads_init_func:
            thread.init_func -= old_addr - sect.address

    def add_reloc_thread(self, thread, segs):
        """
        Add a thread object that needs to be updated by a potential move of
        the sections in which the thread's entry point is.
        """
        # First we need to find the section that the thread's entry point
        # resides in, if that section moves then we need to move the thread's
        # entry point correspondingly.
        found_start = False
        found_enty = False
        found_init_func = False

        for seg in segs:
            for sect in seg.segment.sections:
                if 0 <= thread.start - sect.address < sect.get_size():
                    self.reloc_threads_start.append((thread, sect,
                                                     sect.address))
                    found_start = True
                if 0 <= thread.entry - sect.address < sect.get_size():
                    self.reloc_threads_entry.append((thread, sect,
                                                     sect.address))
                    found_enty = True
                if 0 <= thread.init_func - sect.address < sect.get_size():
                    self.reloc_threads_init_func.append((thread, sect,
                                                         sect.address))
                    found_init_func = True

        if not (found_start or found_enty or found_init_func):
            raise MergeError("Unable to find section that thread resides "
                             "in, should never get here!")

    def make_phys_reloc_table(self, machine, pools, sas):
        reloc_pools = pools.relocatable_pools()
        sym = get_symbol(self.elf, "__physical_relocs", may_not_exist=True)
        reloc_data = self.phys_reloc_table.to_data(reloc_pools,
                                                   Autogen.types.OKL4_word_t._type_size,
                                                   weaver.weavertypes._ELFWEAVER_TYPES_ENDIANESS)

        if sym is None:
            return

        if reloc_data:
            # Where to place the phys reloc table?
            #
            # The phys reloc table is used and discarded before the kernel
            # starts, so it would be a waste to allocate memory just for it.
            # Therefore elfweaver will steal some already allocated memory.
            # Firstly it will look for some memory that that has been
            # allocated to a memsection that is not in the image (such as a
            # stack or a heap).  If nothing suitable is found then it will
            # allocate some memory from the default pools.  Because the
            # freelists for these pools have already been written out, this
            # memory is not lost to the cell.  Once the addresses have been
            # found, a section and segment are created and the phys address
            # patched into the kernel.  The virtual address is only there to
            # make a valid ELF file and is not used.
            #
            # Because the memory is stolen, the reloc code will zero out the
            # table before the kernel starts to prevent phys addresses leaking
            # into userland.

            section = ByteArray(reloc_data)
            section_size = len(section)

            non_image_memsections = []

            # Make a list of every memsection that is not in the image.
            for colln in self.collections:
                if not isinstance(colln, KernelGroup):
                    non_image_memsections.extend(colln.get_items_not_in_image())

            for memsect in non_image_memsections:
                # Search for a memsection big enough to hold the reloc table.
                if memsect.attrs.size >= section_size:
                    sect_phys = memsect.attrs.phys_addr
                    # We need a valid virtual address, to avoid overwriting
                    # something else if the boot loader gets picky, but it
                    # doesn't matter what address it is if the memsection
                    # appears in multiple SASs.
                    sect_virt = memsect.attrs._virt_addrs.values()[0]

                    break
            else:
                # If no memsection can be found, allocate some free memory
                # from the default pools.
                physpool = sas.get_physical_pool_by_name(pools.get_default_physical_pool())
                virtpool = sas.get_virtual_pool_by_name(pools.get_default_virtual_pool())

                phys_item = AllocatorSimpleItem(len(section),
                                                machine.min_page_size())
                virt_item = AllocatorSimpleItem(len(section),
                                                machine.min_page_size())

                try:
                    physpool.alloc(phys_item)
                    virtpool.alloc(virt_item)
                except AllocationError:
                    pass

                sect_phys = phys_item.get_addr()
                sect_virt = virt_item.get_addr()

            sect = UnpreparedElfSection(self.elf,
                                        "kernel.phys_relocs", SHT_PROGBITS,
                                        sect_virt,
                                        data = section,
                                        flags = (SHF_WRITE | SHF_ALLOC |
                                                 SHF_OKL4_SEGMENT_START |
                                                 SHF_OKL4_SEGMENT_END))
            self.elf.add_section(sect)
            new_segment = SectionedElfSegment(self.elf, PT_LOAD,
                                              sect_virt,
                                              sect_phys, PF_R | PF_W,
                                              machine.min_page_size(),
                                              sections=[sect])
            self.elf.add_segment(new_segment)

            self.patch(sym, sect_phys)
        else:
            self.patch(sym, 0)


    def apply_patches(self):
        """Apply registered patches."""
        for segment in self.elf.segments:
            for section in segment.sections:
                for patch in self.patches:
                    if patch.sym.section.prepared_to is section:
                        assert (0 <= patch.get_addr() - section.address <
                                section.get_size())
                        offset = patch.get_addr() - section.address

                        if isinstance(patch.value, weaver.image.ImageObject):
                            value = patch.value.attrs.phys_addr + \
                                    patch.get_value_offset()
                        else:
                            value = patch.value + patch.get_value_offset()

                        if value == -1:
                            # Permit -1 to be used.  Other negative values
                            # will still fire a warning as they probably
                            # aren't actually intended.
                            value += (1 << Autogen.types.WORD_BITS)

                        section.get_data().set_data(offset, value,
                                                    patch.get_size(),
                                                    self.endianess)

    def set_kernel(self, kernel):
        """ Record the kernel."""
        self.elf = UnpreparedElfFile()
        self.endianess = kernel.endianess
        self.wordsize = kernel.wordsize

        self.elf.elf_type = ET_EXEC
        self.elf.endianess = kernel.endianess
        self.elf.wordsize = kernel.wordsize
        self.elf.machine = kernel.machine
        self.elf.osabi = kernel.osabi
        self.elf.abiversion = kernel.abiversion
        self.elf.flags = kernel.flags
        self.elf.entry_point = kernel.entry_point

        if self.ph_offset is not None:
            self.elf.set_ph_offset(self.ph_offset, fixed=True)

    def patch(self, sym, value, size = None, addr_offset = 0, value_offset = 0):
        """Record the details of a patch to a segment."""
        self.patches.append(self.Patch(sym, value, self.elf, size, addr_offset,
                                       value_offset))

    def set_kernel_heap(self, attrs, machine, kern_seg):
        """
        Record the details of the kernel heap.
        """
        self.kernel_heap = ImageKernelHeap(attrs, machine,
                                           self.is_unified, kern_seg)

        return self.kernel_heap

    def add_group(self, sas):
        """
        Add a new collection group to the image and return the group.
        """
        group = Group(self, sas)
        self.collections.append(group)
        return group

    def add_kernel_group(self, sas):
        """
        Add a new kernel collection group to the image and return the
        group.  This group knows about the special allocation
        requirements of the kernel.
        """
        group = KernelGroup(self, sas)
        self.collections.append(group)

        return group

    def add_realm(self, sas, realm_pool, env_realm):
        """
        Add a new Realm collection to the image and return the
        collection.  The realm_pool specifies the pool from which the
        Realm will obtain memory.
        """
        assert(isinstance(realm_pool, weaver.pools.Realm))
        realm = Realm(self, sas, realm_pool, env_realm)
        self.collections.append(realm)

        return realm

    def add_zone(self, sas, zone_pool):
        """
        Add a new Zone collection to the image and return the
        collection.  The zone_pool specifies the pool from which the
        zone will obtain memory.
        """
        assert(isinstance(zone_pool, weaver.pools.Zone))
        zone = Zone(self, sas, zone_pool)
        self.collections.append(zone)

        return zone

    def dump(self):
        """
        Print out a virtual and physical memory map of the final
        image.
        """
        if self.collections is None:
            return

        # Collate all objects in collections into a single list
        objects = []
        for colln in self.collections:
            objects.extend(set(colln.get_items()))

        virtual_objects = {}
        physical_objects = {}

        for obj in objects:
            for (sas, virt_addr) in obj.attrs._virt_addrs.items():
                virt_obj = virtual_objects.setdefault(sas, {})

                if virt_addr is not None:
                    vbase = virt_addr
                    vend = vbase + obj.attrs.size - 1
                    virt_obj[vbase, vend] = obj.attrs.abs_name()

            if (obj.attrs.phys_addr is not None and
                obj.attrs.phys_addr is not weaver.pools.Pools.IGNORE) :
                pbase = obj.attrs.phys_addr
                pend = pbase + obj.attrs.size - 1

                if (pbase, pend) in physical_objects:
                    physical_objects[pbase, pend].append(obj.attrs.abs_name())
                else:
                    physical_objects[pbase, pend] = [obj.attrs.abs_name()]

        if not self.is_unified:
            print "VIRTUAL:"

            for (sas, virt_obj) in sorted(virtual_objects.items(),
                                    cmp = lambda x,y: cmp(x[0].name,
                                                          y[0].name)):
                print "  %s" % sas.name

                for (base, end), name in sorted(virt_obj.items()):
                    print "    <%08x:%08x> %s" % (base, end, name)

        print "PHYSICAL:"
        for (base, end), names in sorted(physical_objects.items()):
            for name in names:
                print "    <%08x:%08x> %s" % (base, end, name)
