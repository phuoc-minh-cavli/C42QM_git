#############################################################################
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
Properties of the target machine.
"""

import math, os
from weaver import MergeError
from weaver.machine_xml import Machine_el
from weaver.parse_spec import XmlCollector
from pyokl4.autogen import Autogen

# Full cache permissions
FULL_CACHE_PERMS_MASK = 0xffff

# Checks if any regions in a given array overlap. Assumes that the array
# is already sorted.
def _assert_no_overlaps(mem_array):
    """Raise an exception if any of the memory regions in the map overlap."""
    highest_memory = -1L
    highest = ()

    # Check for overlaps and give a meaningful error message if there
    # is one.
    for (base, size, name) in mem_array:
        if base <= highest_memory:
            raise MergeError, \
                  'The machine memory region 0x%x--0x%x (size 0x%x) in ' \
                  '"%s" overlaps with region 0x%x--0x%x (size 0x%x) in ' \
                  '"%s".' % (base, base + size - 1, size, name,
                             highest[0], highest[0] + highest[1] -1,
                             highest[1], highest[2])
        else:
            highest_memory = base + size - 1
            highest = (base, size, name)

# Checks that none of the virtual memory regions in a machine object overlap.
def _virtmem_assert_no_overlaps(machine):
    # Extract all the virtual memory regions from the machine object.
    mem_array = sorted([(base, size, name)
                        for (name, mem) in machine.virtual_mem.items()
                        for (base, size, _) in mem])

    _assert_no_overlaps(mem_array)

# Checks that none of the physical memory regions in a machine object overlap.
def _physmem_assert_no_overlaps(machine):
    # Extract all the physical memory regions from the machine object.
    mem_array = [(base, size, name)
                        for (name, mem) in machine.physical_mem.items()
                        for (base, size, _, _, overlap) in mem
                        if overlap == False]

    # Extract all the physical memory regions from the machine's devices.
    mem_array_dev = [(base, size, name)
                        for (_, physitem) in machine.physical_device.items()
                        for (name, mem) in physitem.physical_mem.items()
                        for (base, size, _, _, overlap) in mem
                        if overlap == False]

    mem_array.extend(mem_array_dev)

    sorted_mem = sorted([(base, size, name)
                        for (base, size, name) in mem_array])

    _assert_no_overlaps(sorted_mem)

def _check_cortexa8_erratum_453369(cpu, el):
    """
    Cortex A8 has an erratum in which a Neon store to physical memory in the
    range 0x0 to 0x1000 can result in memory corruption or incorrect memory
    reads. For the given memory element, warn the user if the base address is
    0x0 and an affected CPU revision is being used.

    This affects revisions r1p1, r1p2, r1p3, and r1p7.
    """
    if el.base == 0x0:
        cpu_info = cpu.rsplit('_')
        if cpu_info[0] == "cortexa8" and (cpu_info[1] == "r1p1" or
                cpu_info[1] == "r1p2" or cpu_info[1] == "r1p3"  or
                cpu_info[1] == "r1p7"):
            import sys
            sys.stderr.write("WARNING: Using physical memory addresses in the range 0x0 to 0x1000 may trigger Cortex A8 erratum 453369\n")

class PhysicalDevice(object):
    """
    Description of a physical device.  A simple interface to the data
    in the XML file.
    """

    def __init__(self, name, machine):
        self.name = name
        self.physical_mem = {}
        self.interrupt = {}
        self.machine = machine
        self.key = None

    def add_physical_mem(self, name, mem):
        """
        Add a named list of physical memory ranges. The memory ranges must be a
        list of tuples of the form (base, size, rights, cache_attr, overlap).
        """
        self.physical_mem[name] = mem
        _physmem_assert_no_overlaps(self.machine)

    def add_interrupt(self, name, number):
        """Add a named interrupts."""
        # print "New interrupt %d added" % number
        self.interrupt[name] = number

    def get_physical_mem(self, name):
        """
        Get the named list of physical memory ranges.

        Returns None if the named memory range does not exist.
        """
        return self.physical_mem.get(name)

    def get_interrupt(self, name):
        """
        Get the named interrupt.

        Raises a MergeError if the named interrupt does not exist.
        """
        if name not in self.interrupt:
            raise MergeError, "Interrupt called %s not found." % name

        return self.interrupt[name]

#pylint: disable-msg=R0902
# We have a large number of instance attributes
class Machine(XmlCollector):
    """Description of the image's target machine."""

    element = Machine_el

    def __init__(self):
        XmlCollector.__init__(self)

        self.cpu = ""
        self.num_cpus = 1
        self.board = ""
        self.virtual_mem = {}
        self.physical_mem = {}
        self.physical_device = {}
        self.kernel_heap_align = None
        self.arch_max_spaces = None
        self.image_binary = None
        self.image_physical = None
        self.image_physical_entry = None
        self.image_remove_nobits = None
        self.image_strip = None
        self.hash_size       = 0
        self.hash_overhead_size = 0

    def _get_word_size(self):
        """ Return the word size in bits. """
        return Autogen.machine_info.WORD_SIZE
    word_size = property(_get_word_size)

    def _get_sizeof_word(self):
        """Get the word size in bytes."""
        return self.word_size / 8
    sizeof_word = property(_get_sizeof_word)

    def _sizeof_pointer(self):
        """Return the size of a pointer on the machine."""
        return self.word_size / 8
    sizeof_pointer = property(_sizeof_pointer)

    def _get_page_sizes(self):
        """ Return the machine page sizes. """
        return Autogen.machine_info.PAGE_SIZES
    page_sizes = property(_get_page_sizes)

    def _get_l1_cache_line_size(self):
        """Return the machine's L1 cache line size."""
        return Autogen.machine_info.L1_CACHE_LINE_SIZE
    l1_cache_line_size = property(_get_l1_cache_line_size)

    def _get_l2_cache_line_size(self):
        """Return the machine's L2 cache line size."""
        return Autogen.machine_info.L2_CACHE_LINE_SIZE
    l2_cache_line_size = property(_get_l2_cache_line_size)

    def check_cache_permissions(self, attr, perm_mask):
        """
        Check if the cache attributes given are allowed according to the
        permissions.
        """

        assert type(attr) == type("")
        assert type(perm_mask) == type(set())

        if "all" in perm_mask:
            return True

        attr_val = Autogen.machine_info.cache_attributes.get(attr, None)
        perm_mask_val = \
                      set([Autogen.machine_info.cache_attributes.get(i, None)
                           for i in perm_mask])

        return attr_val in perm_mask_val

    def cache_attr_to_str(self, attr):
        """
        Convert cache attribute to a string.
        """
        return attr

    def cache_perms_to_str(self, perm_mask):
        """Convert the cache permissions into a string."""

        assert type(perm_mask) == type(set())

        if "all" in perm_mask:
            return "all"
        return ', '.join(sorted(perm_mask))

    def get_cache_attr(self, attr):
        """
        Return the numeric value of a named cache attribute or raise
        an exception.
        """
        if Autogen.machine_info.cache_attributes.has_key(attr):
            return attr
        else:
            raise MergeError, "Unknown cache policy: '%s'." % attr

    def add_virtual_mem(self, name, mem):
        """
        Add a named list of virtual memory ranges. The memory ranges must be a
        list of tuples of the form (base, size, mem_type)
        """
        self.virtual_mem[name] = mem
        _virtmem_assert_no_overlaps(self)

    def add_physical_mem(self, name, mem):
        """
        Add a named list of physical memory ranges. The memory ranges must be a
        list of tuples of the form (base, size, mem_type, overlap)
        """
        self.physical_mem[name] = mem
        _physmem_assert_no_overlaps(self)

    def add_phys_device(self, name):
        """
        Create and add a named physical device.
        Returns the newly created device.
        """
        self.physical_device[name] = PhysicalDevice(name, self)
        return self.physical_device[name]

    def get_phys_device(self, name):
        """Get the details of the named physical device."""
        return self.physical_device[name]

    def get_virtual_memory(self, name):
        """Get the names list of virtual memory ranges."""
        if name not in self.virtual_mem:
            raise MergeError, "Virtual memory called %s not found." % name

        return self.virtual_mem[name]

    def get_physical_memory(self, name):
        """Get the names list of physical memory ranges."""
        # First look for the physical memory in devices...
        for dev in self.physical_device.itervalues():
            phys_mem = dev.get_physical_mem(name)

            if phys_mem is not None:
                return phys_mem

        # ... then try to look for it in the machine
        if name not in self.physical_mem:
            raise MergeError, "Physical memory called %s not found." % name

        return self.physical_mem[name]

    def find_physical_memory(self, base, size):
        """Locate the block of physical memory covering the specified
        base and size. If found, construct a region using the base and size
        provided, and the rights from the physical memory"""
        def construct_region(base, size, rights, overlap = False):
            """Build a region tuple from the inputs."""
            return [(base, size, rights, None, overlap)]

        # Note: if you don't specify a src tag, then the specified pool
        # must fit within a single <region> tag.
        for dev in self.physical_device.itervalues():
            for devmem in dev.physical_mem.itervalues():
                for (dbase, dsize, rights, _, overlap) in devmem:
                    if (dbase <= base) and (dsize - (base - dbase) >= size):
                        return construct_region(base, size, rights, overlap)

        for mem in self.physical_mem.itervalues():
            for (pbase, psize, rights, _, overlap) in mem:
                if (pbase <= base) and (psize - (base - pbase) >= size):
                    return construct_region(base, size, rights, overlap)

        raise MergeError, "Memory base %#x size %#x does not exist in a " \
              "single physical memory range." % (base, size)

    def min_page_size(self):
        """Return the smallest allowable page size."""
        return self.page_sizes[-1]

    def max_page_size(self):
        """Return the largest allowable page size."""
        return self.page_sizes[0]

    def remove_16meg_pages(self):
        """
        Remove page sizes greater that 1Meg.  This will prevent 16Meg mappings
        from being used in phys relocatable images.
        """
        while Autogen.machine_info.PAGE_SIZES[0] > 0x100000:
            del Autogen.machine_info.PAGE_SIZES[0]

    def superpage_alignment(self, size):
        """Calculate the biggest alignment supported on the current
        machine for the given size.

        Items smaller than the min page size are min page size aligned
        and items larger than the max page size are max page sise
        aligned. Items within this range are rounded down to the nearest
        page size.
        """
        # Align small objects to the page size.
        if size <= self.min_page_size():
            return self.min_page_size()

        # Align large items to the max page size.
        if size > self.max_page_size():
            return self.max_page_size()

        for page_size in self.page_sizes:
            if page_size <= size:
                return page_size

    def natural_alignment(self, size):
        """
        Return the natural alignment for the size.  Eg 64K regions are aligned
        to 64K boundaries.

        Items smaller than the min page size are min page size aligned
        and items larger than the max page size are max page sise
        aligned. Items within this range are rounded down to the nearest power
        of two.
        """

        # Align small objects to the page size.
        if size <= self.min_page_size():
            return self.min_page_size()

        # Align large items to the max page size.
        if size > self.max_page_size():
            return self.max_page_size()

        return 1 << int(math.log(size, 2))

    def map_mem_rights_str(self, string):
        """
        Map the string cache permissions mask to an internal representation.
        """
        return set([x.strip() for x in string.split(",")])

    def map_mem_rights(self, region_el):
        """Map the string memory rights to a memory rights value."""
        mem_rights = getattr(region_el, "rights", "all")

        return self.map_mem_rights_str(mem_rights)

    def get_hash_size(self):
        """Return the machine's hash size."""
        return self.hash_size

    def get_hash_overhead_size(self):
        """Return the machine's hash overhead size."""
        return self.hash_overhead_size

    def collect_xml(self, machine_el, ignore_name, **_):
        """Collect the attributes of the machine element."""

        # If the file attribute is specified, then read the full version
        # of the machine element from that file.
        if hasattr(machine_el, "file"):
            machine_el = \
                       Machine_el.parse_xml_file(os.path.join(machine_el._path,
                                                              machine_el.file))

        cpu_el = machine_el.find_child("cpu")
        self.cpu = cpu_el.name
        self.num_cpus = getattr(cpu_el, "num", 1)

        hash_size_el = machine_el.find_child("hash_size")
        self.hash_size = getattr(hash_size_el, "size", 0)

        hash_overhead_size_el = machine_el.find_child("hash_overhead_size")
        self.hash_overhead_size = getattr(hash_overhead_size_el, "size", 0)

        cache_attr_el = machine_el.find_child("cache_attribute")
        self.l1_dcache_way_size = getattr(cache_attr_el, "l1_dcache_way_size", None)

        board_el = machine_el.find_child("board")
        if board_el is not None: # FIXME: this shouldn't be optional -kswong
            self.board = board_el.name

        attrs = machine_el.find_child("kernel_heap_attrs")
        if attrs is not None:
            self.kernel_heap_align = getattr(attrs, 'align',
                                             self.kernel_heap_align)

        for p_el in machine_el.find_children("physical_memory"):
            if not ignore_name.match(p_el.name):
                # Check if this memory element contains the range affected
                # by Cortex A8 erratum 453369.
                for el in p_el.find_children("region"):
                    _check_cortexa8_erratum_453369(self.cpu, el)
                mem = [(el.base, el.size, self.map_mem_rights(el), None,
                        getattr(el, 'overlap', False))
                       for el in p_el.find_children("region")]
                self.add_physical_mem(p_el.name, mem)

        for v_el in machine_el.find_children("virtual_memory"):
            if not ignore_name.match(v_el.name):
                mem = [(el.base, el.size, self.map_mem_rights(el))
                       for el in v_el.find_children("region")]
                self.add_virtual_mem(v_el.name, mem)

        for d_el in machine_el.find_children("phys_device"):
            if not ignore_name.match(d_el.name):
                device = self.add_phys_device(d_el.name)
                for p_el in d_el.find_children("physical_memory"):
                    if not ignore_name.match(p_el.name):
                        # Check if this memory element contains the range
                        # affected by Cortex A8 erratum 453369.
                        for el in p_el.find_children("region"):
                            _check_cortexa8_erratum_453369(self.cpu, el)
                        mem = [(el.base, el.size, self.map_mem_rights(el),
                                getattr(el, 'cache_policy', "uncached"),
                                getattr(el, 'overlap', False))
                               for el in p_el.find_children("region")]
                        device.add_physical_mem(p_el.name, mem)
                for i_el in d_el.find_children("interrupt"):
                    if not ignore_name.match(i_el.name):
                        device.add_interrupt(i_el.name, i_el.number)

        image = machine_el.find_child("image_type")
        if image is not None:
            self.image_binary = getattr(image, 'binary', None)
            self.image_physical = getattr(image, 'physical', None)
            self.image_physical_entry = getattr(image, 'physical_entry', None)
            self.image_remove_nobits = getattr(image, 'remove_nobits', None)
            self.image_strip = getattr(image, 'strip', None)

    def assert_no_virtual_mem(self):
        """
        Raise an error if there are virtual memory regions present.
        This is only an error for unified images.
        """

        if self.virtual_mem:
            raise MergeError, \
                  'The XML element "virtual_memory" is not allowed ' \
                  'when building OKL4 Pico images.'

    def set_elfweaver_info(self, info):
       """
       Function to set elfweaver_info in machine.
       """
       self.elfweaver_info = info
