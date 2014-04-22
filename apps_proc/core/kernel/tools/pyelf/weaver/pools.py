##############################################################################
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

"""Manage virtual and physical memory pools."""

from elf.util import align_down, align_up
from weaver import MergeError
from weaver.allocator import Allocator, AllocatorException, \
     AllocatorSimpleItem, SimpleTiedItem, AllocatorTracker
from weaver.parse_spec import XmlCollector
from weaver.pools_xml import VirtualPool_el, PhysicalPool_el

class Pool(Allocator):
    """
    A named collection of virtual or physical memory from which
    memsections or segments are allocated.
    """
    def __init__(self, name, min_alloc_size, machine, tracker = None,
            is_global = False):
        Allocator.__init__(self, min_alloc_size, machine, tracker)

        self.name = name
        self.direct = False
        self._is_global = is_global

    def _get_is_global(self):
        """Return whether the pool should be shared beween SASs."""
        return self._is_global
    is_global = property(_get_is_global)

    def get_name(self):
        """Return the name of the pool."""
        return self.name


    def clone(self, tracker):
        """
        Return a pool that covers the same address range as this
        pool, but has not had any allocations made from it.
        """
        my_clone = Pool(self.name, self.machine.min_page_size(), self.machine)
        my_clone.clone_memory(self)
        my_clone.tracker = tracker

        return my_clone


class VirtualPool(Pool, XmlCollector):
    """
    A pool of virtual memory regions.
    """
    element = VirtualPool_el

    def __init__(self, _, machine):
        XmlCollector.__init__(self)
        Pool.__init__(self, None, machine.min_page_size(), machine)

    def collect_xml(self, memory_pool_el, machine, pools):
        """
        Create a pool from the supplied element and add the described
        memory to the pool.
        """
        self.name = memory_pool_el.name
        pools.add_new_pool(self, pools.virtual_pools, "Virtual pool")

        for el in memory_pool_el.children:
            if el.tag == 'memory':
                src = getattr(el, 'src', None)
                base = getattr(el, 'base', None)
                size = getattr(el, 'size', None)

                pools.add_virtual_memory(self.name, machine, src=src, base=base,
                                         size=size)

class PhysicalPool(Pool, XmlCollector):
    """
    A pool of physical memory regions.
    """

    element = PhysicalPool_el

    def __init__(self, pools, machine):
        XmlCollector.__init__(self)
        Pool.__init__(self, None, machine.min_page_size(), machine,
                      tracker=pools.allocated_physical)

        self.relocatable = False

    def collect_xml(self, memory_pool_el, machine, pools):
        """
        Create a pool from the supplied element and add the described
        memory to the pool.
        """
        self.name = memory_pool_el.name
        self.relocatable = getattr(memory_pool_el, "relocatable", False)

        # Pool relocations allow images to start on any 1M boundary.  This
        # will fail if 16Meg mappings are used, so remove references to 16Meg
        # page tables to prevent these mappings from being used.
        if self.relocatable:
            machine.remove_16meg_pages()

        pools.add_new_pool(self, pools.physical_pools, "Physical pool")

        for el in memory_pool_el.children:
            if el.tag == 'memory':
                src = getattr(el, 'src', None)
                base = getattr(el, 'base', None)
                size = getattr(el, 'size', None)

                pools.add_physical_memory(self.name, machine, src=src,
                                          base=base, size=size)
        # The direct attribute said that items allocated in this pool
        # could use the direct flag, but this was never enforced and
        # in practice every pool had direct set.  Now any pool can be
        # used for direct allocation and this attribute is ignored.

class Realm(Pool):
    """
    An allocator that places items within a collection of
    <chunk_size> sized regions.
    """

    # By default use a one megabyte region for all windows.  This
    # works well with FASS.
    DEFAULT_CHUNK_SIZE = 1024 * 1024

    def __init__(self, name, min_alloc_size, machine,
                 tracker = None,
                 chunk_size = DEFAULT_CHUNK_SIZE):
        Pool.__init__(self, name, min_alloc_size, machine, tracker=tracker)

        self.source_pool = None
        self.windows = []
        self.machine = machine
        self.chunk_size = chunk_size

    def prime(self, pool, ranges, sas):
        """
        Priming a realm involves initialising any chunks that are
        known to be required.  The ranges parameter contains the
        addresses of the items in the realm with fixed addresses.
        Chunks are created around these items by reserving the 1M
        regions around each item.  There can be multiple items in a
        chunk and a chunk can be made up of multiple 1M regions if the
        items fall into adjacent regions.

        Chunks are taken from the pool containing their memory range,
        which later allocations for the realm are taken from the
        supplied pool.  The interface, mark_window, needs to be passed
        the list of ranges already allocated from the chunk.  If the
        allocator finds that other allocations have occured, then an
        exception is raised.
        """
        try:
            self.source_pool = sas.get_virtual_pool_by_name(pool)
        except KeyError:
            raise MergeError, \
                    "Failed to prime realm '%s' with source pool '%s'" % \
                    (self.name, pool)

        # Sort ranges to find those that are in the same window.
        ranges.sort(key=lambda x: x[0])

        holes = []
        chunk_base = None
        chunk_end = None
        num_chunks = None

        for (base, size) in ranges:
            range_base = align_down(base, self.chunk_size)

            if chunk_base is None:
                # Start a new chunk.
                assert holes == []
                chunk_base = range_base
                chunk_end = align_up(base + size - 1, self.chunk_size) - 1
                num_chunks = (chunk_end - chunk_base + 1) / self.chunk_size
                holes.append((base, size))

            elif chunk_base <= range_base and range_base < chunk_end:
                # Range begins inside our current chunk

                if (base + size - 1) > chunk_end:
                    # But the range extends past the chunk by this much:
                    extend = (base + size) - chunk_end
                    # Which translates to this many extra 1M regions:
                    extra_1m_regions = align_up(extend, self.chunk_size) / \
                                   self.chunk_size
                    num_chunks += extra_1m_regions
                    chunk_end = chunk_base + (num_chunks * self.chunk_size) - 1

                holes.append((base, size))

            else:
                # The range wholly exists outside of our current
                # chunk, so we mark the chunk and start a new one
                assert holes != []

                total_chunk_size = self.chunk_size * num_chunks
                free_mem = sas.mark_chunk(chunk_base,
                                          total_chunk_size,
                                          holes)

                assert free_mem is not None

                # Add the free parts of the window to our freelist.
                for (free_base, free_end, mem_type) in free_mem:
                    self.add_memory(free_base, free_end - free_base + 1,
                                    mem_type)

                # Add the window to the list of claimed windows.
                self.windows.append((chunk_base,
                                     chunk_base + total_chunk_size - 1))

                chunk_base = range_base
                chunk_end = align_up(base + size - 1, self.chunk_size) - 1
                holes = [(base, size)]
                num_chunks = (chunk_end - chunk_base + 1) / self.chunk_size

        # Clean up
        if chunk_base is not None:
            assert holes
            total_chunk_size = self.chunk_size * num_chunks
            free_mem = sas.mark_chunk(chunk_base,
                                      total_chunk_size,
                                      holes)

            assert free_mem is not None
            # Add the free parts of the window to our freelist.
            for (free_base, free_end, mem_type) in free_mem:
                self.add_memory(free_base, free_end - free_base + 1, mem_type)

            # Add the window to the list of claimed windows.
            self.windows.append((chunk_base,
                                 chunk_base + total_chunk_size - 1))

    def alloc(self, item):
        # At this point the realm has already been primed, so any items
        # at fixed addresses have already reserved the surrounding
        # window sized regions.
        #
        # Now we need to allocate the remaining items in and around
        # the fixed items (but honouring any other constraints
        # specified eg. alignment, direct etc.)
        #
        # We could potentionally run out of memory from our little realm
        # pool so we need to be prepared to allocate subsequent
        # windows from the source pool to accomodate the remaining
        # items.
        #
        # Note that any sub-page groups are allocated in the order
        # they were passed in.

        try:
            # First try and allocate from our own resources.
            Pool.alloc(self, item)
        except AllocatorException:
            # Out of memory!
            #
            # Determine how many more windows we need to allocate
            # Although the size of an item may be smaller than the
            # window size, it's offset (alignment requirements) may still
            # push it over a window boundary.
            size = align_up(item.get_size() + item.get_offset(),
                            self.chunk_size)

            # Item may have an alignment which greater than a window size
            alignment = max(self.chunk_size, item.get_alignment())

            # If the offset is negative then there is a guard page in front of
            # the item.  If the alignment is bigger than the chunk size, then
            # more memory is needed so that item can be placed on a multiple
            # of chunk_size boundary and still have memory to allocate the
            # guard page from.
            if (item.get_offset() < 0 and
                (item.get_alignment() >= self.chunk_size)):
                size += self.chunk_size * (alignment / self.chunk_size)

            # If the item has a tied item, then we'll need a window
            # that can satisfy that allocation.  So this by tying an
            # item to our allocation, but do not allocate the
            # equivalent physical memory.
            tied = item.get_tied_item()
            if tied is not None:
                tied = SimpleTiedItem(tied.get_allocator(),
                                      alloc_tied = False)

            # Now we request the required number of windows
            # from the realm's source pool
            win = AllocatorSimpleItem(name = self.get_name() + "/realm_window",
                                      size = size,
                                      alignment = alignment,
                                      tied = tied)

            self.tracker.am_marking_window()
            self.source_pool.alloc(win)
            self.add_memory(win.get_addr(), win.get_size(),
                            self.machine.map_mem_rights_str("all"))

            # Record the new window.
            self.windows.append((win.get_addr(),
                                 win.get_addr() + size - 1))

            Pool.alloc(self, item)

    def alloc_group(self, group):
        # We use the same logic as alloc(), but do the necessary handling
        # of subpage groups.

        try:
            # First try and allocate from our own resources.
            Pool.alloc_group(self, group)
        except AllocatorException:
            # Out of memory!

            # Determine how many more windows we need to allocate
            # Knowing the exact size of a group is tricky, currently we:
            # Round up each item to the nearest page and sum all items.
            # Also take into account the possibility of the first item
            # having an offset.
            tied = None
            size = 0
            for i in group:
                if size:
                    size = align_up(size, i.get_alignment())

                size += align_up(i.get_size() + i.get_offset(),
                                 self.machine.min_page_size())

                # If the item has a tied item, then we'll need a window
                # that can satisfy that allocation.  So this by tying an
                # item to our allocation, but do not allocate the
                # equivalent physical memory.
                #
                # XXX: This needs to take into account different
                # items being tied to different allocators.
                if i.get_tied_item() is not None:
                    tied = SimpleTiedItem(i.get_tied_item().get_allocator(),
                                          alloc_tied = False)

            size = align_up(size, self.chunk_size)

            # Item may have an alignment which greater than a window size
            alignment = max(self.chunk_size, group[0].get_alignment())

            # If the offset is negative then there is a guard page in front of
            # the item.  If the alignment is bigger than the chunk size, then
            # more memory is needed so that item can be placed on a multiple
            # of chunk_size boundary and still have memory to allocate the
            # guard page from.
            if (group[0].get_offset() < 0 and
                (group[0].get_alignment() >= self.chunk_size)):
                size += self.chunk_size * (alignment / self.chunk_size)

            # Now we request the required number of windows
            # from the realm's source pool
            win = AllocatorSimpleItem(name = self.get_name() + "/realm_window",
                                      size = size,
                                      alignment = alignment,
                                      tied = tied)

            self.tracker.am_marking_window()
            self.source_pool.alloc(win)
            self.add_memory(win.get_addr(), win.get_size(),
                            self.machine.map_mem_rights_str("all"))

            # Record the new window.
            self.windows.append((win.get_addr(),
                                 win.get_addr() + size - 1))

            Pool.alloc_group(self, group)

class Zone(Pool):
    """
    An Allocator that places items within a continous region of
    <chunk_size> sized objects.
    """
    # By default use a one megabyte region for all windows.  This
    # works well with FASS.
    DEFAULT_CHUNK_SIZE = 1024 * 1024

    def __init__(self, name, min_alloc_size, machine,
                 tracker = None,
                 chunk_size = DEFAULT_CHUNK_SIZE):
        Pool.__init__(self, name, min_alloc_size, machine, tracker=tracker)

        self.source_pool = None
        self.windows = []
        self.machine = machine
        self.chunk_size = chunk_size

    def prime(self, pool, ranges, sas):
        """
        When priming a Zone pool we are given a list of ranges that need to be
        reserved (more specifically, marked). These ranges are usually collection
        items with fixed addresses.
        We must contiguously reserve and mark the windows that lie between ranges.
        """
        try:
            self.source_pool = sas.get_virtual_pool_by_name(pool)
        except KeyError:
            raise MergeError, \
                    "Failed to prime Zone '%s' using source pool '%s'" % \
                    (self.name, pool)

        if not ranges:
            return

        # Sort ranges to find those that are in the same window.
        ranges.sort(key=lambda x: x[0])

        (first_base, _) = ranges[0]
        (last_base, last_size) = ranges[-1]

        window_base = align_down(first_base, self.chunk_size)
        window_end = align_up(last_base + last_size - 1, self.chunk_size) - 1
        size = window_end - window_base + 1

        free_mem = self.source_pool.mark_window(window_base, size, ranges)

        for (free_base, free_end, mem_type) in free_mem:
            self.add_memory(free_base, free_end - free_base + 1, mem_type)

        # Add the window to the list of claimed windows.
        self.windows.append((window_base, window_end))

    def get_range(self):
        """
        Return the range covered by this Zone.
        """
        if not self.windows:
            return None
        else:
            windows = sorted(self.windows, key=lambda x: x[0])

            (base, _) = windows[0]
            (_, end) = windows[-1]

            return base, end

    def __alloc(self, requested_size, alignment, tied):
        """
        Zone internal function to extend the zone window by the
        requested amount with the specified alignment.
        """

        def alloc_new_window(size, alignment, tied = None):
            """
            Allocate a new window from anywhere in our source pool.

            Note: This function can optionally accept a tied item to also
            be allocated, however it will most likely leave the Zone in a
            broken state, the caller must ensure that any Zone invariants
            are restored.
            """
            # Item may have an alignment which greater than a window size
            alignment = max(self.chunk_size, alignment)

            win = AllocatorSimpleItem(name = self.get_name() + "/zone_window",
                                       size = size,
                                       alignment = alignment,
                                       tied = tied)

            self.tracker.am_marking_window()
            self.source_pool.alloc(win)
            self.add_memory(win.get_addr(), win.get_size(),
                            self.machine.map_mem_rights_str("all"))
            assert(win.get_size() == size)

            return (win.get_addr(), win.get_addr() + size - 1)

        def alloc_extend_window(base, size):
            """Allocate a window starting from the given base address."""
            end = align_up(base + size, self.chunk_size)

            try:
                free_mem = self.source_pool.mark_window(base, size, None)
            except AllocatorException:
                raise MergeError, "Cannot allocate contiguous Zone!"

            # The list of returned memory should not be fragmented!
            assert(len(free_mem) == 1)

            (free_base, free_end, mem_type) = free_mem[0]
            self.add_memory(free_base, free_end - free_base + 1, mem_type)

            return (base, end - 1)

        # We allocate direct items in Zones as follows:
        # 1) Allocate the tied item wherever we can, add the surrounding
        #    window to our window list.
        # 2) This will very likely leave our Zone in a broken state
        #    (e.g. a gap between the existing window and the tied item).
        #    So we then attempt to mark the gap as belonging to the Zone.
        #    If we are unable to mark the gap we raise an exception and
        #    make no further attempt to fix the Zone.
        if tied is not None:
            # Store what the zone looked like before we allocate the tied item
            zone_was_empty = self.get_range() is None
            if not zone_was_empty:
                (zone_start, zone_end) = self.get_range()

            tied_base, tied_end = alloc_new_window(requested_size,
                                                   alignment, tied=tied)
            self.windows.append((tied_base, tied_end))

            # Easy case, zone was empty, we've allocated a window for the
            # tied item, we can return now.
            if zone_was_empty:
                return
            # Not so easy case, we now want to try to mark the (likely) gap
            # we have introduced...
            else:
                if tied_base < zone_start:
                    requested_base = tied_end + 1
                    requested_size = zone_start - (tied_end + 1)
                elif tied_base > zone_end + 1:
                    requested_base = zone_end + 1
                    requested_size = tied_base - (zone_end + 1)
                else:
                    # We got lucky and allocated adjacently!
                    return

        # Now, we have two possibilities...

        # Our Zone currently has no memory, so we allocate a new
        # window from anywhere in our source pool.
        if not self.get_range():
            win_base, win_end = alloc_new_window(requested_size, alignment)

        # Otherwise we need to extend it with an adjacent window.
        else:
            if tied is None:
                # We aren't doing a tied item fix-up, calculate amount to
                # extend Zone as per normal
                (_, zone_end) = self.get_range()
                requested_base = zone_end + 1
            win_base, win_end = alloc_extend_window(requested_base,
                                                    requested_size)

        self.windows.append((win_base, win_end))

    def alloc(self, item):
        """
        When allocating items in a Zone we use the following method:
        1) Try to allocate it in the existing Zone window
        2) If the constraints of the item don't allow it to be allocated in
           our existing window we need to allocate a subsequent adjacent
           window. We pass in the end of our existing window as a hint to
           the allocator mark() method. If it fails we are out of options
           because the Zone will no longer be contiguous and therefore die.
        """
        # Optimistically try to allocate from our own pool
        try:
            Pool.alloc(self, item)
        except AllocatorException:
            # Out of memory!

            # Determine how many more windows we need to allocate
            # Although the size of an item may be smaller than the
            # window size, it's offset (alignment requirements) may still
            # push it over a window boundary.
            requested_size = align_up(item.get_size() + item.get_offset(),
                                      self.chunk_size)

            # If the item has a tied item, then we'll need a window
            # that can satisfy that allocation.  So this by tying an
            # item to our allocation, but do not allocate the
            # equivalent physical memory.
            tied = item.get_tied_item()
            if tied is not None:
                tied = SimpleTiedItem(tied.get_allocator(),
                                      alloc_tied = False)

            # If the offset is negative then there is a guard page in front of
            # the item.  If the alignment is bigger than the chunk size, then
            # more memory is needed so that item can be placed on a multiple
            # of chunk_size boundary and still have memory to allocate the
            # guard page from.
            if (item.get_offset() < 0 and
                (item.get_alignment() >= self.chunk_size)):
                requested_size += self.chunk_size * \
                                  (item.get_alignment() / self.chunk_size)

            self.__alloc(requested_size, item.get_alignment(), tied)

            # Now we retry the allocation
            Pool.alloc(self, item)

    def alloc_group(self, group):
        # Optimistically try to allocate from our own pool
        try:
            Pool.alloc_group(self, group)
        except AllocatorException:
            # Out of memory!

            # Determine how many more windows we need to allocate
            # Knowing the exact size of a group is tricky, currently we:
            # Round up each item to the nearest page and sum all items.
            # Also take into account the possibility of the first item
            # having an offset.
            size = 0
            tied = None
            for i in group:
                if size:
                    size = align_up(size, i.get_alignment())

                size += align_up(i.get_size() + i.get_offset(),
                                 self.machine.min_page_size())

                # If the item has a tied item, then we'll need a window
                # that can satisfy that allocation.  So this by tying an
                # item to our allocation, but do not allocate the
                # equivalent physical memory.
                #
                # XXX: This needs to take into account different
                # items being tied to different allocators.
                if i.get_tied_item() is not None:
                    tied = SimpleTiedItem(i.get_tied_item().get_allocator(),
                                          alloc_tied = False)

            requested_size = align_up(size, self.chunk_size)

            # The alignment of the group is taken to be the alignment of
            # the first item in the group.
            alignment = 0
            if group[0]:
                alignment = group[0].get_alignment()

            # If the offset is negative then there is a guard page in front of
            # the item.  If the alignment is bigger than the chunk size, then
            # more memory is needed so that item can be placed on a multiple
            # of chunk_size boundary and still have memory to allocate the
            # guard page from.
            if (group[0].get_offset() < 0 and
                (group[0].get_alignment() >= self.chunk_size)):
                requested_size += self.chunk_size * (alignment / self.chunk_size)

            self.__alloc(requested_size, alignment, tied)

            # Now we retry the allocation
            Pool.alloc_group(self, group)

class AllocatedMemoryTracker (AllocatorTracker):
    """Class for keeping track of memory allocation."""
    def __init__(self, title):
        AllocatorTracker.__init__(self)

        self.mem = []
        self.title = title
        self.alloc_name = "unknown"
        self.marking_window = False

    def track_mark(self, base, end, can_overlap = False):
        """
        Record memory that has been marked in the allocator.

        This function will be called by mark().

        Window allocations are not tracked at all.
        """
        self.__add(self.alloc_name, base, end, can_match_exact = True,
                   can_overlap = can_overlap)

    def track_alloc(self, name, base, end):
        """
        Record memory that has been allocated.

        This method is called once for each item in an allocator
        group.
        """
        # If a new zone window is being allocated then do nothing.
        if self.marking_window:
            self.marking_window = False
        else:
            self.__add(name, base, end)

    def set_alloc_name(self, name):
        """Set the name to use for the next region of marked memory."""
        self.alloc_name = name

    def am_marking_window(self):
        """
        Indicate that the next call to track_alloc() relates to the
        allocation of a new window for a zone and can be ignored.
        """
        self.marking_window = True

    def add_size(self, name, base, size):
        """Record memory usage [base, base + size - 1]"""
        end = base + size - 1
        self.__add(name, base, end)

    def check(self, name, base, size, can_match_exact = False):
        """
        Check if the memory region overlaps with any tracked memory.

        Raises a MergeError if there is an overlap.
        """
        if base == Allocator.IGNORE:
            return

        self.__check(name, base, base + size - 1, can_match_exact)

    def __check(self, name, base, end, can_match_exact = False):
        """
        Internal function to check if a memory region overlaps with
        any of the tracked memory.
        """
        size = end - base + 1

        # Check for overlaps and give a meaningful error message if there
        # is one.
        for (alloc_name, alloc_base, alloc_end) in self.mem:
            if base == alloc_base and end == alloc_end and can_match_exact:
                continue
            if (alloc_base <= base <= alloc_end) or \
                   (alloc_base <= end <= alloc_end):
                if self.title == "Physical Memory":
                   print 'Warning: %s: Parts of "%s" (0x%x-0x%x, size 0x%x) overlap with ' \
                         '"%s" (0x%x-0x%x, size 0x%x).' % \
                         (self.title, name, base, base + size - 1, size,
                          alloc_name, alloc_base, alloc_end,
                          alloc_end - alloc_base + 1)
                else:
                   raise MergeError, \
                         '%s: Parts of "%s" (0x%x-0x%x, size 0x%x) overlap with ' \
                         '"%s" (0x%x-0x%x, size 0x%x).' % \
                         (self.title, name, base, base + size - 1, size,
                          alloc_name, alloc_base, alloc_end,
                          alloc_end - alloc_base + 1)

    def __add(self, name, base, end, can_match_exact = False,
              can_overlap = False):
        """
        Record the memory usage and raise an exception if the range
        overlaps an existing allocation.

        If can_match_exact is True and a range overlaps an
        existing range exactly, then no error is raised.
        """

        if not can_overlap:
            self.__check(name, base, end, can_match_exact)

        self.mem.append((name, base, end))
        self.mem.sort()
        self.alloc_name = "unknown"

#---------------------------------------------------------------------------
# POOLS
#---------------------------------------------------------------------------

class Pools:
    """
    A collection of physical and virtual memory pools.

    A single collection of pools (e.g. one Pools object) is used across all
    objects incuding the kernel and the cell applications when merging.

    The kernel is the first object to get access to the pools, and will define
    a default virtual and physical pools which other objects can use if they
    don't have specfic pools which they want to access.
    """

    # Alias of the allocator ignore object.
    IGNORE = Allocator.IGNORE

    def __init__(self):
        """
        When first created, there are no virtual or physical pools. However
        memory trackers are in place to keep track of the pools as they are
        created and have memory allocated form them
        """
        # Name -> pool mappings for virtual and physical pools
        self.virtual_pools = {}
        self.physical_pools = {}

        # List of virtual pools that are shared between all SAS objects
        self.global_virtual_pools = []

        # Ensure no overlaps in memory pools or memory allocations
        # across pools.
        self.virtual_pool_mem = AllocatedMemoryTracker("Virtual Pools")
        self.physical_pool_mem = AllocatedMemoryTracker("Physical Pools")
        self.allocated_virtual = AllocatedMemoryTracker("Virtual Memory")
        self.allocated_physical = AllocatedMemoryTracker("Physical Memory")
        self.default_virtual = None
        self.default_physical = None

    def set_default_pools(self, virtual, physical):
        """
        Set the global default virtual and physical pools.
        """
        self.default_virtual = virtual
        self.default_physical = physical

    def get_default_virtual_pool(self):
        """Return the global default virtual pool."""
        return self.default_virtual

    def get_default_physical_pool(self):
        """Return the global default physical pool."""
        return self.default_physical

    def new_virtual_pool(self, name, machine, is_global=False):
        """Create a new virtual pool."""
        return self._new_alloc_object(name, machine,
                                      self.virtual_pools, Pool,
                                      self.allocated_virtual,
                                      "Virtual pool", is_global=is_global)

    def new_physical_pool(self, name, machine):
        """Create a new physical pool."""
        return self._new_alloc_object(name, machine,
                                      self.physical_pools, Pool,
                                      self.allocated_physical,
                                      "Physical pool")

    def _new_alloc_object(self, name, machine, pools, cls, allocated,
                          alloc_type, **kwargs):
        """
        Internal function to allocate a new physical or virtual pool.
        """
        alloc = cls(name, machine.min_page_size(), machine, tracker=allocated, \
                **kwargs)
        alloc.name = name
        self.add_new_pool(alloc, pools, alloc_type)

        return alloc

    @staticmethod
    def add_new_pool(pool, pools, alloc_type):
        """
        Register a new pool with the system.
        """
        if pool.name in pools:
            raise MergeError, \
                  '%s "%s" already exists.' % (alloc_type, pool.name)
        pools[pool.name] = pool

    def add_virtual_memory(self, virtpool, machine, src=None, base=None,
                           size=None):
        """Add memory to the freelist of a pool."""
        # Convert the free list into (base, end) pairs.
        if src is not None:
            regions = machine.get_virtual_memory(src)
        else:
            assert(base is not None and size is not None)

            regions = [(base, size, machine.map_mem_rights_str("all"))]

        for (base, size, mem_type) in regions:
            self.virtual_pool_mem.add_size(virtpool, base, size)
            self.virtual_pools[virtpool].add_memory(base, size, mem_type)

    def add_physical_memory(self, physpool, machine, src=None, base=None,
                            size=None):
        """Add memory to the freelist of a pool."""
        # Convert the free list into (base, end) pairs.
        if src is not None:
            regions = machine.get_physical_memory(src)
        else:
            assert(base is not None and size is not None)

            regions = machine.find_physical_memory(base, size)

        for (base, size, mem_type, _, _) in regions:
            try:
                self.physical_pool_mem.add_size(physpool, base, size)
                self.physical_pools[physpool].add_memory(base, size, mem_type)
            except AllocatorException, ex:
                raise MergeError, \
                    "An error occured trying to add phys pool %s. " \
                    "Details: \n\t%s" % (physpool, ex)

    def new_sas(self, name):
        """Return a new Single Address Space."""

        return SAS(name, self)

    def print_last_phys(self):
        """
        Print next address to be allocated from each physical pool.
        The pools are sorted to give a consistent output.
        """
        return '\n'.join("%s: %#x" % (name, alloc.next_avail())
                          for (name, alloc) in sorted(self.physical_pools.items()))

    def assert_no_virtpools(self):
        """
        Raise an error if there are virtpools present.  This is only
        an error for unified images.
        """

        if len(self.virtual_pools):
            raise MergeError, \
                  'The XML element "virtual_pools" is not allowed ' \
                  'when building OKL4 Pico images.'

    def relocatable_pools(self):
        """Return a list of all relocatable pools in the image."""
        return [(name, pool)
                for (name, pool) in sorted(self.physical_pools.items())
                if pool.relocatable]

class SAS(object):
    """
    Representation of a single address space.

    A single address space is a collection of pools from which
    memory can be allocated.  Objects allocated from a SAS are
    guaranteed to have non-overlapping addresses.

    The physical pools within a SAS are shared with other SASs. The
    virtual pools are typically private copies, but some virtual pools
    contain global mappings used by the kernel and are shared with
    other SASs. The pool names and initial memory range are shared
    between all SASs.
    """

    def __init__(self, name, pools):
        self.name = name

        # Name -> pool mappings for virtual and physical pools
        self.virtual_pools = {}
        self.physical_pools = pools.physical_pools

        # Ensure no overlaps in memory pools or memory allocations
        # across pools.
        self.allocated_virtual = AllocatedMemoryTracker("Virtual Memory")
        self.allocated_physical = pools.allocated_physical
        self.default_virtual = None
        self.default_physical = None

        # Make copies of the virtual pools.
        for (name, pool) in pools.virtual_pools.iteritems():
            if pool.is_global:
                self.virtual_pools[name] = pool
            else:
                self.virtual_pools[name] = pool.clone(self.allocated_virtual)

    def get_virtual_pool_by_name(self, name):
        """Return the virtual pool with the given name."""
        return self.virtual_pools[name]

    def get_physical_pool_by_name(self, name):
        """Return the physical pool with the given name."""
        return self.physical_pools[name]

    def get_physical_pools(self):
        """Return all physical pools."""
        return self.physical_pools.values()

    def mark_virtual(self, range_name, base, size, cache_policy,
                     can_overlap = False):
        """
        Mark a region of virtual memory as used.
        """
        marked = False

        if base is not None:
            # Unlike for physical addresses, it is not an error for
            # the range to be outside every pool.  This is chiefly
            # because the kernel virtual addresses will always be
            # outside every pool.
            self.allocated_virtual.set_alloc_name(range_name)

            for (name, alloc) in self.virtual_pools.items():
                try:
                    if alloc.mark(base, size, cache_policy, can_overlap):
                        marked = True
                except AllocatorException, text:
                    raise MergeError, 'Virtual pool "%s": %s' % (name, text)

            if not marked and not can_overlap:
                self.allocated_virtual.check(range_name, base, size,
                                             can_match_exact=True)

        return marked

    def mark_physical(self, range_name, base, size, cache_policy):
        """
        Mark a region of physical memory as used.  The region must
        exist in a pool.
        """
        marked = False
        if base is not None:
            self.allocated_physical.set_alloc_name(range_name)

            for (name, alloc) in self.physical_pools.items():
                try:
                    if alloc.mark(base, size, cache_policy):
                        marked = True
                except AllocatorException, text:
                    raise MergeError, 'Physical pool "%s": %s' % (name, text)

            if not marked:
                self.allocated_physical.check(range_name, base, size,
                                              can_match_exact=True)

        return marked

    def mark_chunk(self, base, size, holes):
        """
        Mark a chunk as being used, returning the free portions of the
        memory claimed.  Search for virtual pool list because it is
        unknown which pool the chunk live in.
        """
        for (name, alloc) in self.virtual_pools.items():
            try:
                allocations = alloc.mark_window(base, size, holes)

                if allocations is not None:
                    return allocations
            except AllocatorException, text:
                raise MergeError, 'Virtual pool "%s": %s' % (name, text)

        return None

    def _new_alloc_object(self, name, machine, pools, cls, allocated,
                          alloc_type):
        """
        Internal function to allocate a new realm or zone.
        """
        alloc = cls(name, machine.min_page_size(), machine, tracker=allocated)
        alloc.name = name
        self.add_new_pool(alloc, pools, alloc_type)

        return alloc

    @staticmethod
    def add_new_pool(pool, pools, alloc_type):
        """
        Register a new pool with the system.
        """
        if pool.name in pools:
            raise MergeError, \
                  '%s "%s" already exists.' % (alloc_type, pool.name)
        pools[pool.name] = pool

    def new_realm(self, name, machine):
        """Create a new realm."""
        return self._new_alloc_object(name, machine,
                                      self.virtual_pools, Realm,
                                      self.allocated_virtual, "Realm")

    def new_zone(self, name, machine):
        """Create a new zone."""
        # Zones and virtual pools share the same address space, so they share
        # the same namespace.
        return self._new_alloc_object(name, machine,
                                      self.virtual_pools, Zone,
                                      self.allocated_virtual, "Zone")
