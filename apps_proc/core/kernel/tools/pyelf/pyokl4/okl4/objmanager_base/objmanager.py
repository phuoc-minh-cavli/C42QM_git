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

"""Common object manager methods."""

from weaver.weavertypes import SimpleElfweaverType
from weaver.range_allocator import RangeAllocator, RangeAllocatorException

class OKL4_Padding(SimpleElfweaverType):
    """
    Padding for alignment.
    """

    # Field property list.  The keys are used to detect places where a
    # renamed field has been missed.  The value describes various
    # field properties.
    _plist = {
        }

    def __init__(self, address, size):
        SimpleElfweaverType.__init__(self, address, None)

        self._type_size = size

    def to_data(self):
        """Generate a string representation of the objmanager objects"""
        return '\0' * self._type_size

class ObjmanagerBase(SimpleElfweaverType):
    """
    An implementation of objmanager with simple allocation requirements,
    suitable for object managers only need to perform memory allocation
    at weave-time.
    """

    # Field property list.  The keys are used to detect places where a
    # renamed field has been missed.  The value describes various
    # field properties.
    _plist = {
        "allocs_log" : {},
        "allocated_chunks" : {}
        }

    def __init__(self, virt_addr, phys_addr, heap_size):
        """
        Initialise the object manager, creating the full set of objects that
        are required.
        """
        SimpleElfweaverType.__init__(self, virt_addr, phys_addr = phys_addr)

        # Setup a range allocator to track our internal heap usage.
        self._allocator = RangeAllocator(virt_addr, heap_size)
        self._vaddr = virt_addr
        self._paddr = phys_addr
        self._heap_size = heap_size
        self._misc_allocs = []
        self.allocs_log = []
        self.allocated_chunks = []

    def get_base_address(self):
        """
        Return the virtual address bottom of the obj manager's memory range.
        """
        return self.get_address()

    def _virt_to_phys(self, virt):
        """Convert virt addr to phys."""
        return virt - self._vaddr + self._paddr

    def _allocate_object(self, objclass, num_objects, alignment,
            total_allocation_size_override = 0):
        """Allocate an arbitrary object."""

        # Determine how much memory we need to allocate.
        size = objclass._type_size * num_objects

        if total_allocation_size_override > 0:
            assert total_allocation_size_override >= size
            size = total_allocation_size_override

        # Construct the objects.
        objects = []

        for i in range(0, num_objects):
            obj = objclass(None)
            objects.append(obj)
            self._misc_allocs.append(obj)

        self.allocated_chunks.append((size, alignment, objclass, objects[:]))

        return objects

    def to_data(self):
        """Generate a string representation of the objmanager objects"""

        data = []

        # Get a list of all our objects.
        objects = self._misc_allocs

        # Create padding objects for everything still in the free list.
        for (base, size) in self._allocator.free_list:
            objects.append(OKL4_Padding(base, size))

        # Sort the objects by virtual address.
        objects = sorted(objects, key = lambda x: x.get_address())

        # Convert the objects to data.
        data = ''.join(x.to_data() for x in objects)

        # Ensure the data seems sane.
        assert len(data) == self._heap_size
        return data

    def free_list_size(self):
        """Return the size of the freelist."""
        # Sum up all the entries in our free list.
        return sum(size for (_, size) in self._allocator.free_list)

    def finalise_allocations(self):
        """
        Finalise any deferred allocation policies.
        """
        def obj_cmp(a, b):
            """Sort objects by alignment then size."""
            (a_size, a_alignment, a_objclass, a_objects) = a
            (b_size, b_alignment, b_objclass, b_objects) = b

            align_cmp = cmp(a_alignment, b_alignment)

            if align_cmp == 0:
                return cmp(a_size, b_size)
            else:
                return align_cmp

        for chunk in sorted(self.allocated_chunks, cmp = obj_cmp, reverse=True):
            (size, alignment, objclass, objects) = chunk

            # Skip if addresses have already been allocated.  This can happen
            # in Pico.
            if objects[0].get_address() is not None:
                continue

            # Perform the allocation.
            try:
                vaddr = self._allocator.allocate(size, alignment)[0]
            except RangeAllocatorException:
                raise Exception(
                    "%s: Kernel heap allocation failed: kernel heap size too small." %
                    self.get_elfweaver_obj().namespace.abs_path("."))

            # Log the allocation for memstats
            self.allocs_log.append((size, objclass))

            for (i, obj) in enumerate(objects):
                obj_vaddr = vaddr + i * objclass._type_size
                obj_paddr = self._virt_to_phys(obj_vaddr)
                obj.set_address(address = obj_vaddr, phys_addr = obj_paddr)

    def get_phys_relocs(self, base_offset=0):
        relocs = []

        for chunk in self.allocated_chunks:
            (size, alignment, objclass, objects) = chunk

            for obj in objects:
                obj_relocs = obj.get_phys_relocs()

                obj_offset = obj.get_address() - self._allocator.base

                for reloc in obj_relocs:
                    reloc.address += base_offset + obj_offset

                relocs.extend(obj_relocs)

        return relocs
