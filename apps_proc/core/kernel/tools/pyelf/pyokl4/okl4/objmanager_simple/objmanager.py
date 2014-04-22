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
Based on kernel/objmanager-simple/src/mem_allocator.c
"""

# Implement a simple heap, the C code appears to do the following

# Declare an array of HEAP_SIZE bytes and link words seperated by MIN_HEAP_SIZE
# bytes into a linked list.  When allocate is called we find a set of bytes
# large enough to hold the requested size and pull that chunk out of the linked
# list.  All returns objects are multiples of MIN_HEAP_SIZE.

# The python implementation does a similar task, setup a series of chunks in
# a list and then remove then as allocations are done, replacing them with
# single new chunks that contains a list of the object types (threads, etc.)
# that were allocated.  To write out the data strucutre we combine any
# unallocated chunks and the allowed chunks, sort by address and just call
# to_data() on all the objects.

from elf.util import align_up
from weaver.range_allocator import RangeAllocator, RangeAllocatorException
from pyokl4.autogen import Autogen
from weaver.weavertypes import SimpleElfweaverType
from pyokl4.okl4.kernel_base.list import LIST_ENQUEUE
from pyokl4.okl4.objmanager_simple.cap_allocator import lookup_cap
from pyokl4.okl4.objmanager_base.objmanager import ObjmanagerBase


# Minimum allocation size from the heap.
NODE_HEADER_SIZE = 8L
HEAP_MIN_ALLOC = NODE_HEADER_SIZE
HEAP_MIN_ALIGNMENT = NODE_HEADER_SIZE

class ObjmanagerException(Exception):
    """Exception to be raised if we have a problem in the objmanager"""


class Objmanager(ObjmanagerBase):
    """
    A simple object manager, using a simple heap as backing for all objects.
    """

    # Objmanager type.
    name = "simple"

    class AllocatedNode(SimpleElfweaverType):
        """Represent an allocated chunk of memory from the objmanager's heap."""

        # Field property list.  The keys are used to detect places where a
        # renamed field has been missed.  The value describes various field
        # properties.
        _plist = {
            'alloc_size': {},
            'objs': {},
            'padding': {},
            'objclass': {},
            'alignment': {},
            'tail_alignment': {},
            }

        def __init__(self, alloc_size, objclass, num_objects, alignment,
                tail_alignment = 1):
            SimpleElfweaverType.__init__(self, None)

            self.alloc_size = alloc_size
            self.objclass = objclass
            self.alignment = alignment
            self.tail_alignment = tail_alignment

            # Create the objects but to not assign an address
            self.objs = [objclass(None, self)
                         for _ in range(num_objects)]

            # Calculate amount of padding
            self.padding = (self.alloc_size -
                            (num_objects * self.objclass._type_size))
            assert self.padding >= 0

        def finalise_allocations(self, allocator, virt_to_phys,
                                 allocs_log,
                                 heap_path):
            try:
                (vaddr, vsize) = allocator.allocate(self.alloc_size,
                                                    alignment = self.alignment,
                                                    tail_alignment = \
                                                    self.tail_alignment)
            except RangeAllocatorException:
                raise Exception(
                    "%s: Kernel heap allocation failed: " \
                    "kernel heap size too small." % heap_path)

            # Log the allocation for memstats
            allocs_log.append((vsize, self.objclass))

            paddr = virt_to_phys(vaddr)
            self.set_address(vaddr, paddr)

            obj_size = self.objclass._type_size

            for (i, obj) in enumerate(self.objs):
                obj.set_address(address = vaddr + i * obj_size,
                                phys_addr = paddr + i * obj_size)

        def get_phys_relocs(self, base_offset=0):
            relocs = []
            obj_size = self.objclass._type_size

            for (i, obj) in enumerate(self.objs):
                relocs.extend(obj.get_phys_relocs(base_offset + i * obj_size))

            return relocs

        def to_data(self):
            """Convert the chunk to binary data."""

            return ''.join(obj.to_data() for obj in self.objs) + \
                ('\0' * self.padding)

    class FreeNode(SimpleElfweaverType):
        """
        Represent a contiguous chunk of free memory in the objmanager's heap.
        """

        # Field property list.  The keys are used to detect places where a
        # renamed field has been missed.  The value describes various field
        # properties.
        _plist = {
            'size': {},
            'initial_node': {},
            'next': {},
            }

        def __init__(self, size, address,
                phys_addr = None, initial_node = False, back_ptr = None):
            SimpleElfweaverType.__init__(self, address, back_ptr, phys_addr)

            self.size = size
            self.initial_node = initial_node
            self.next = None

        def to_data(self):
            """Convert the chunk to binary data."""

            # Next pointer
            ptr = Autogen.types.OKL4_void_ptr(self._address,
                    self, phys_addr = self.get_phys_address())
            ptr.value = self.next

            # Free bytes
            free = Autogen.types.OKL4_word_t(self._address,
                    self, phys_addr = self.get_phys_address())
            if self.initial_node:
                free.value = 0
            else:
                free.value = self.size

            header = ptr.to_data() + free.to_data()
            padding = self.size - len(header)

            return header + ('\0' * padding)

    # Field property list.  The keys are used to detect places where a
    # renamed field has been missed.  The value describes various
    # field properties.
    _plist = {
        'size': {},
        'allocated_chunks': {},
        'allocs_log': {},
        'init_node': {},
        }

    def __init__(self, virt_address, phys_address, size):
        """
        Create a new heap at the given virtual address of the given size.
        """

        ObjmanagerBase.__init__(self, virt_address, phys_address, size)

    def get_address(self):
        """Return the address of the object manager."""

        (vaddr, _) = self._allocator.free_list[0]

        return vaddr

    def get_base_address(self):
        """
        Return the virtual address bottom of the obj manager's memory range.
        """
        return self._address

    def allocate_object(self, objclass, num_objects, alignment,
            total_allocation_size_override = 0, tail_alignment = 1):
        """Allocate an object from the objmanagers heap"""

        # Get the allocation size.
        alloc_size = objclass._type_size * num_objects

        if total_allocation_size_override > 0:
            assert total_allocation_size_override > alloc_size

            alloc_size = total_allocation_size_override

        alloc_size = align_up(alloc_size, HEAP_MIN_ALLOC)

        # If the alignment request is too small, fix it.
        if alignment < HEAP_MIN_ALIGNMENT:
            alignment = HEAP_MIN_ALIGNMENT

        # Create an allocation containing the series of objects requested
        chunk = Objmanager.AllocatedNode(alloc_size, objclass, num_objects,
                alignment, tail_alignment = tail_alignment)
        self.allocated_chunks.append(chunk)

        return chunk.objs

    def to_data(self):
        """Generate a byte array representing the heap of the objmanager"""

        # Create free nodes for each free chunk of memory.
        free_nodes = []
        for (vaddr, vsize) in self._allocator.free_list:
            free_nodes.append(
                    Objmanager.FreeNode(vsize, vaddr,
                        self._virt_to_phys(vaddr), back_ptr = self))

        # Setup next pointers on the free chunks
        for i in range(len(free_nodes) - 1):
            free_nodes[i].next = free_nodes[i + 1]

        # Sort everything by virtual address
        sorted_chunks = self.allocated_chunks + free_nodes
        sorted_chunks.sort(key = lambda x: x.get_address())
        data = [chunk.to_data() for chunk in sorted_chunks]

        # Write out the data
        data = ''.join(data)
        if len(data) != self._heap_size:
            raise Exception("%s: Too much data specified for the size of the "
                            "kernel heap.  Specified: %x Required: %x" %
                            (self.get_elfweaver_obj().namespace.abs_path("."),
                             self._heap_size, len(data)))
        assert len(data) == self._heap_size

        return data

    def finalise_allocations(self):
        """
        Finalise any deferred allocation policies.
        """
        def obj_cmp(a, b):
            """Sort objects by alignment then size."""
            align_cmp = cmp(a.alignment, b.alignment)

            if align_cmp == 0:
                return cmp(a.alloc_size, b.alloc_size)
            else:
                return align_cmp

        for chunk in sorted(self.allocated_chunks, cmp = obj_cmp, reverse=True):
            chunk.finalise_allocations(self._allocator,
                                       self._virt_to_phys,
                                       self.allocs_log,
                                       self.get_elfweaver_obj().namespace.abs_path("."))

    def get_phys_relocs(self, base_offset=0):
        relocs = []

        for chunk in self.allocated_chunks:
            chunk_relocs = chunk.get_phys_relocs()

            chunk_offset = chunk.get_address() - self._allocator.base

            for reloc in chunk_relocs:
                reloc.address += base_offset + chunk_offset

            relocs.extend(chunk_relocs)

        return relocs

def set_mpointer(mpointer, cap, unwindid, blocked):
    """Set a managed pointer to an object."""

    mpointer.object = cap.object
    mpointer.unwindid = unwindid
    mpointer.cap = cap
    mpointer.blocked = blocked
    LIST_ENQUEUE(cap.mpointers, mpointer.list_entry)

def objmanager_allocate(objclass, domain, num_objects, alignment,
                        total_allocation_size_override = 0,
                        tail_alignment = 1):
    """Perform an allocation from the domain's objmanager"""

    return domain._source_heap.allocate_object(objclass, num_objects,
            alignment, total_allocation_size_override, tail_alignment)

def create_weaved_thread(**kwargs):
    """Create a new thread."""

    okl4_thread = Autogen.syscalls.sys_thread_create(**kwargs)
    Autogen.scheduler.syscall.sys_priority_set(okl4_thread, kwargs['priority'])
    Autogen.scheduler.syscall.sys_timeslice_length_set(okl4_thread,
                                                       kwargs['timeslice'])
    affinity = kwargs.get('affinity', None)

    if affinity:
        Autogen.scheduler.syscall.sys_affinity_set(okl4_thread, affinity)

    Autogen.scheduler.syscall.sys_thread_start(okl4_thread)

    return okl4_thread

def objmanager_managed_lookup_object(type, domain, designator, rights, mpointer,
        unwindid, blocked):
    cap = lookup_cap(domain, designator)

    assert cap != None
    assert cap.object != None

    assert cap.rights & rights == rights
    assert cap.type == type

    set_mpointer(mpointer, cap, unwindid, blocked)

    return cap.object
