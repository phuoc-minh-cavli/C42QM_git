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

"""
                   Memory allocator for elfweaver.

   This module is used to keep track of the memory in physical and
                            virtual pools.



               Lasciate ogne speranza, voi ch~intrate.

             (Google it if you don't know the reference)

"""

from itertools import chain
from elf.util import align_up, align_down
import copy

class AllocatorException(Exception):
    """Exception thrown when an item or group could not be allocated."""
    pass


class TiedItem(object):
    """
    Abstract class describing an item that is tied to this
    allocator item.  The tied item and this item will be given the
    same addresses, but will allocated from different allocators.
    Either item may have its address already set.

    This is the basis for supporting direct/1:1 allocation.
    """
    def get_addr(self):
        """
        Return the address of the tied item.
        Returns None if the address has not been set.
        """
        raise NotImplementedError

    def set_addr(self, addr):
        """Set the address of the tied item to the given value."""
        raise NotImplementedError

    def get_allocator(self):
        """
        Return the allocator that should be used to allocate the
        tied item's address.
        """
        raise NotImplementedError

    def alloc_tied(self):
        """
        Return whether the address for this tied item should be
        allocated.  Some objects, such as PDs and Zones, just need
        a virtual region that could be used for tied allocation
        (so that a later tied allocation can occur within their
        own allocator), but do not need the direct allocation
        themselves.  Those objects can have this function return
        False.
        """
        return True

class AllocatorItem(object):
    #pylint: disable-msg=R0922
    """
    Abstract class for describing an item that is part of an
    allocation group.
    """

    def get_name(self):
        """Return a text description of the item."""
        raise NotImplementedError

    def get_size(self):
        """Return the size of the item ."""
        raise NotImplementedError

    def get_alignment(self):
        """Return the desired alignment of the item."""
        raise NotImplementedError

    def get_offset(self):
        #pylint: disable-msg=R0201
        """Return the desired offset into the page of the item."""
        return 0

    def get_cache_attr(self):
        #pylint: disable-msg=R0201
        """Return the desired cache attr of the object."""
        return None

    def get_addr(self):
        """
        Return the address of the object.
        Returns None if the address has not been set.
        """
        raise NotImplementedError

    def set_addr(self, addr):
        """Set the address of the object to the given value."""
        raise NotImplementedError

    def force_page(self):
        #pylint: disable-msg=R0201
        """
        Return whether or not the item should be allocated on a new
        page even if it would fit in a partial page.
        """
        return False

    def get_tied_item(self):
        #pylint: disable-msg=R0201
        """
        Return the item tied to this one, or None if these is no such
        item.
        """
        return None

class SimpleTiedItem(TiedItem):
    """
    A simple concrete version of AllocatorItem.TiedItem.
    """
    def __init__(self, allocator, addr = None, alloc_tied = True):
        TiedItem.__init__(self)

        self.allocator = allocator
        self.addr = addr
        self._alloc_tied = alloc_tied

    def get_addr(self):
        """
        Return the address of the tied item.
        Returns None if the address has not been set.
        """
        return self.addr

    def set_addr(self, addr):
        """Set the address of the tied item to the given value."""
        self.addr = addr

    def get_allocator(self):
        """
        Return the allocator that should be used to allocate the
        tied item's address.
        """
        return self.allocator

    def alloc_tied(self):
        """
        Return whether an address for this tied item should be
        allocated.  Some objects, such as PDs and Zones, just need a
        virtual region that could be used for tied allocation (so that
        later tied allocation can occur within their own allocator),
        but do not need the direct allocation themselves.  Those
        objects can have this function return false.
        """
        return self._alloc_tied

class AllocatorSimpleItem(AllocatorItem):
    """
    Simple concrete version of AllocatorItem.
    """
    def __init__(self, size, alignment, offset = 0,
                 addr = None, name = "unknown_item", cache_attr = "default",
                 force_page = False, tied = None):
        AllocatorItem.__init__(self)

        self.size = size
        self.alignment = alignment
        self.offset = offset
        self.addr = addr
        self.name = name
        self.cache_attr = cache_attr
        self._force_page = force_page
        self.tied = tied

    def get_name(self):
        """Return a text description of the item."""
        return self.name

    def get_size(self):
        """Return the size of the item ."""
        return self.size

    def get_alignment(self):
        """Return the desired alignment of the item."""
        return self.alignment

    def get_offset(self):
        """Return the desired offset into the page of the item."""
        return self.offset

    def get_cache_attr(self):
        """Return the desired cache attr of the object."""
        return self.cache_attr

    def get_addr(self):
        """
        Return the address of the object.
        Returns None if the address has not been set.
        """
        return self.addr

    def set_addr(self, addr):
        """Set the address of the object to the given value."""
        self.addr = addr

    def force_page(self):
        """
        Return whether or not the item should be allocated on a new
        page even if it would fit in a partial page.
        """
        return self._force_page

    def get_tied_item(self):
        """
        Return the item tied to this one, or None if these is no such
        item.
        """
        return self.tied

class AllocatorTracker(object):
    #pylint: disable-msg=R0921
    """
    Abstract class for keeping track of memory used by an allocator.

    Such a class could check for overlapping memory allocations.
    """

    def __init__(self):
        self.marking_direct = False

    def track_mark(self, base, end, can_overlap = False):
        """
        Record memory that has been marked in the allocator.

        This function will be called by mark().

        Window allocations are not tracked at all.

        If can_overlap is True, then it is not an error for the region
        to overlap a previously allocated range.
        """
        raise NotImplementedError

    def track_alloc(self, name, base, end):
        """
        Record memory that has been allocated.

        This method is called once for each item in an allocator
        group.

        """
        raise NotImplementedError

class Allocator(object):
    """
    A memory allocator.

    This allocator is based on the first fit algorithm with an
    interface tuned to the rather strange requirements of elfweaver.

    The allocator keeps a sorted list of free address ranges.  Each
    range is a multiple of <min_alloc_size> bytes (aka a page), and is
    aligned on a <min_alloc_size> boundary.  Each range is stored as
    the triple <base, end, cache_rights>.

    Separate calls to the allocator will result in items being
    allocated in different pages.  However it is possible to allocate
    items on adjacent bytes, provided that that are part of the same
    allocation group.

    An individual item will be placed at the lowest address that
    satify its size, offset and alignment requirements.  Items in a
    group are allocated in increasing order of address.

    Memory at a particular address can be removed from the free list
    with the mark() method.  It is not an error to mark memory that is
    wholly or in part missing from the free list.

    A window of memory can also be marked with the mark_window()
    method.  A window is similar to regular marking, except that it is
    an error for the region of memory to contain memory outside of the
    freelist unless those regions are present in a list supplied to
    the method.  The entire memory region must have once been present
    in the allocator's freelist.  Windows are used to implement
    zones.

    """

    # Magic value to indicate that an item should be ignored.  Used in
    # cases where an address's item is calculated after allocation,
    # but for other reasons it must appear in the allocation lists.

    IGNORE = object()

    class Range(object):
        """
        Description of a range of free memory.
        """
        def __init__(self, base, end, cache_rights):
            self.base = base
            self.end = end
            self.cache_rights = cache_rights

        def __repr__(self):
            """
            Return a readable version of the free range.
            """
            return "%#x--%#x (%s)" % (self.base, self.end, self.cache_rights)

        def overlap(self, other_range):
            """
            Return whether or not the two ranges overlap.
            """
            return (self.base <= other_range.base <= self.end) or \
                   (other_range.base <= self.base <= other_range.end)

        def contains(self, other_range):
            """
            Return weather or not this range wholly contains the
            second range.
            """
            return (self.base <= other_range.base <= self.end) and \
                   (self.base <= other_range.end <= self.end)


    def __init__(self, min_alloc_size, machine, tracker = None):
        """
        __init__(min_alloc_size)

        min_alloc_size is the smallest size, in bytes, that can be
        allocated.  Alignment must be a multiple of this size.
        """

        # Alloc size must be a power of 2.
        assert (min_alloc_size & (min_alloc_size - 1)) == 0

        self.freelist = []
        self.fulllist = []
        self.min_alloc = min_alloc_size
        self.total_size = 0
        self.tracker = tracker
        self.machine = machine

    def __merge(self, the_list):
        #pylint: disable-msg=R0201
        """
        Merge adjacent regions into one.

        Merging regions simplifies the logic of the rest of the
        allocation code.
        """
        last = None
        new_list = []

        for free_range in the_list:
            if last is None:
                last = free_range
            else:
                if last.end + 1 == free_range.base:
                    last = self.Range(last.base, free_range.end,
                                      last.cache_rights)
                else:
                    new_list.append(last)
                    last = free_range

        if last is not None:
            new_list.append(last)

        return new_list

    def _aligned(self, addr):
        """
        Return whether or not the given address is aligned to the
        allocator's min allocation size.
        """
        return (addr % self.min_alloc) == 0

    def _align_up(self, addr):
        """
        Return the address rounded up to the allocator's min
        allocation size.
        """
        return align_up(addr, self.min_alloc)

    def _align_down(self, addr):
        """
        Return the address rounded down to the allocator's min
        allocation size.
        """
        return align_down(addr, self.min_alloc)

    def _align_range(self, base, end):
        """
        Return a range object with the base address rounded down and
        the end address rounded up (then minus 1) to the allocator's
        min allocation size.
        """
        return self.Range(self._align_down(base), self._align_up(end) - 1,
                          None)

    def sort(self):
        """
        Sort the free and full memory lists and merge any adjacent
        memory regions.
        """
        self.freelist.sort(key=lambda x: x.base)
        self.freelist = self.__merge(self.freelist)

        self.fulllist.sort(key=lambda x: x.base)
        self.fulllist = self.__merge(self.fulllist)


    def get_freelist(self):
        """Return the current free list."""
        return self.freelist

    def get_fulllist(self):
        """Return the current full list of memory in this pool."""
        return self.fulllist

    def contains(self, addr, size):
        """
        Returns whether or not that address range is in the allocator's
        full list.
        """
        contained_range = self.Range(addr, addr + size - 1, None)

        for full_range in self.fulllist:
            if full_range.contains(contained_range):
                return True
        else:
            return False

    def add_memory(self, base, size, cache_rights):
        """
        Add a region of free memory to the pool.

        The memory must be ia multiple of <min_alloc> bytes in
        size, and aligned on a <min_alloc> boundary.

        Memory addresses may only be added to the allocator once.
        """
        # Check that the memory is page aligned.
        if not self._aligned(base):
            raise AllocatorException, \
                "Attempting to add region at 0x%x, which is not aligned to: 0x%x" % (
                base, self.min_alloc)
        if not self._aligned(size):
            raise AllocatorException, \
                "Attempting to add region of size 0x%x, which is not aligned to: 0x%x" % (
                size, self.min_alloc)
        if cache_rights is None:
            raise AllocatorException, \
                "Attempting to add region with None cache rights"
        assert cache_rights is not None

        mem_range = self.Range(base, base + size - 1, cache_rights)

        # Check that the memory does not overlap any of the regions
        # already in use.
        for full_range in self.fulllist:
            if full_range.overlap(mem_range):
                raise AllocatorException, \
                      "Cannot add overlapping memory regions to the " \
                      "Allocator.  Address (%#x--%#x) already in " \
                      "(%#x--%#x)" % \
                      (mem_range.base, mem_range.end,
                       full_range.base, full_range.end)

        self.total_size += size
        self.freelist.append(mem_range)
        # Put a copy of the range in the full list to ensure that it
        # doesn't change.
        self.fulllist.append(copy.copy(mem_range))
        self.sort()


    def clone_memory(self, other):
        """
        Initialise the full and freelist from the fulllist of the
        supplied allocator.  This method is used to create a copy of
        the supplied allocator.
        """

        # Make sure that the allocation regions are compatible.
        assert self.min_alloc == other.min_alloc

        self.fulllist = copy.deepcopy(other.fulllist)
        self.freelist = copy.deepcopy(other.fulllist)

    def mark(self, base, size, cache_attr, can_overlap = False):
        """
        mark(base, end) -> marked

        Remove the given range from the free list.  This is used to
        record where fixed address objects are located.

        Returns whether or not the range was removed from the free list.

        It is *not* an error if the range is wholly outside the
        freelist.
        """

        assert base is not None

        if base == self.IGNORE:
            return False

        end = base + size - 1

        if end < base:
            raise AllocatorException, \
                  "Mark end address (%#x) less than mark base address (%#x)" \
                  % (end, base)

        if base == end:
            end = end + 1

        # Remove all of the pages containing the region.
        mark_range = self._align_range(base, end)

        new_freelist = []
        marked = False

        for free_range in self.freelist:
            assert self._aligned(free_range.base)
            assert self._aligned(free_range.end + 1)

            if free_range.overlap(mark_range):
                marked = True

                if (self.tracker is None or not self.tracker.marking_direct) \
                   and not self.machine.check_cache_permissions(cache_attr,
                                                                free_range.cache_rights):
                    raise AllocatorException("Mark: Cache attr \"%s\" is not"
                            " allowed by the cache rights \"%s\"" % (
                            self.machine.cache_attr_to_str(cache_attr),
                            self.machine.cache_perms_to_str(free_range.cache_rights)))

                # Report the allocation to the tracker, if there is
                # one.
                if self.tracker is not None:
                    self.tracker.track_mark(mark_range.base, mark_range.end,
                                            can_overlap)

                # Insert into the freelist any remaining parts of the
                # free memory region.
                if free_range.base < mark_range.base:
                    new_freelist.append(self.Range(free_range.base,
                                                   mark_range.base - 1,
                                                   free_range.cache_rights))

                if mark_range.end < free_range.end:
                    new_freelist.append(self.Range(mark_range.end + 1,
                                                   free_range.end,
                                                   free_range.cache_rights))
            else:
                new_freelist.append(free_range)

        self.freelist = new_freelist

        return marked


    def mark_window(self, window_base, size, holes):
        """
        Mark a window of memory.

        A window is region of memory whose addresses must be either in
        the free list or listed in the holes list.  Every address in
        the region must have been present in the freelist at some
        stage.

        The holes variable is a list of (base, size) tuples.

        Returns the parts of the freelist that were removed.

        In a similar way to mark(), it is not an error for region to
        live outside the fulllist.
        """
        assert window_base is not None and window_base is not self.IGNORE

        if holes is None:
            holes = []

        window_end = window_base + size - 1

        if window_end < window_base:
            raise AllocatorException, \
                  "alloc_window: Window end address (%#x) less " \
                  "than mark base address (%#x)" % \
                  (window_end, window_base)

        if window_base == window_end:
            window_end = window_end + 1

        window_range = self._align_range(window_base, window_end)

        # First check that the proposed window is in the memory that
        # was originally passed to add_memory().
        for full_range in self.fulllist:
            # If the window is fully contained within one record, then
            # we're sweet.
            # What if two records are adjacent?
            if full_range.contains(window_range):
                break
            # Jira ticket QCT-88 - mwarton.
            if window_range.base == 0 and full_range.base < 0x100000 and \
                   (full_range.base <= window_range.end <= full_range.end):
                holes = [(0, full_range.base)] + holes
                break
        else:
            return None

        # Transform the hole list from (base, size) to (base, end),
        # rounded to page boundaries, and sort in increasing order of
        # address.
        holes = [self._align_range(hole_base, hole_base + hole_size - 1)
                 for (hole_base, hole_size) in holes]
        holes.sort(key=lambda x: x.base)

        # Holes must be in the range of the window and can't overlap.
        for hole_range in holes:
            assert window_range.contains(hole_range)

        free_iter = iter(self.freelist)
        hole_iter = iter(holes)
        curr_addr = window_range.base
        free_done = False
        holes_done = False
        curr_free = None
        curr_hole = None
        new_freelist = []
        removed_freelist = []

        while curr_addr <= window_range.end:
            assert self._aligned(curr_addr)

            if not free_done and curr_free is None:
                try:
                    curr_free = free_iter.next()

                    # If the freelist range is outside where we are
                    # working, then loop and get another one.
                    if curr_free.end < curr_addr or \
                           curr_free.base > window_range.end:
                        new_freelist.append(curr_free)
                        curr_free = None
                        continue
                except StopIteration:
                    free_done = True
                    curr_free = None

            if not holes_done and curr_hole is None:
                try:
                    curr_hole = hole_iter.next()
                except StopIteration:
                    holes_done = True
                    curr_hole = None

            if curr_free is not None and \
                   curr_free.base <= curr_addr <= curr_free.end:
                if curr_hole is not None and \
                       curr_hole.base <= curr_free.end:
                    raise AllocatorException, \
                          "alloc_window: Hole (%#x-%#x) overlaps " \
                          "with free block (%#x-%#x)." % \
                          (curr_hole.base, curr_hole.end, curr_free.base,
                           curr_free.end)
                else:
                    # Remove the part we're interested in from the
                    # freelist.  Add the excess.
                    if curr_free.base < curr_addr:
                        new_freelist.append(self.Range(curr_free.base,
                                                       curr_addr - 1,
                                                       curr_free.cache_rights))

                    if curr_free.end > window_range.end:
                        new_freelist.append(self.Range(window_range.end + 1,
                                                       curr_free.end,
                                                       curr_free.cache_rights))
                        removed_freelist.append((curr_addr,
                                                 window_range.end,
                                                 curr_free.cache_rights))
                    else:
                        removed_freelist.append((curr_addr,
                                                 curr_free.end,
                                                 curr_free.cache_rights))

                    curr_addr = curr_free.end + 1
                    curr_free = None
            elif curr_hole is not None and \
                     curr_hole.base == curr_addr:
                if curr_free is not None and \
                       curr_free.base <= curr_hole.end:
                    raise AllocatorException, \
                          "alloc_window: Hole (%#x-%#x) overlaps " \
                          "with free block (%#x-%#x)." % \
                          (curr_hole.base, curr_hole.end, curr_free.base,
                          curr_free.end)
                else:
                    curr_addr = curr_hole.end + 1
                    curr_hole = None
            else:
                raise AllocatorException, \
                      "Address %#x should be in a zone but is neither " \
                      "free or in an already allocated block.  Is it part " \
                      "of a direct addressing pool?" % \
                      curr_addr

        # Copy any remaining free list records into the new freelist.
        new_freelist.extend(free_iter)

        self.freelist = new_freelist

        return removed_freelist

    def __simple_alloc(self, freelist, size, alignment, offset, force_page):
        """
        Allocate a single block of memory or raise an exception.

        Search for a place in the freelist to allocate the requested
        block of memory.

        Returns a tuple with the following values:

        - before_free - The freelist up to the place where the memory
          was allocated.
        - after_free  - The freelist after the place where the memory
          was allocated
        - alloc_start - The allocated addresses.

        The free lists are not necessarily page aligned.

        Returning the tuple allows the abort and retry semantics of
        group allocation to be implemented.
        """
        before_free = []
        after_free = []
        free_iter = iter(freelist)

        # Search the freelist for a suitable block.
        for free_range in free_iter:
            # Calculate the proposed address.
            if force_page:
                # Treat the freelist as if it started at the beginning
                # of the next complete page.
                fs_up = self._align_up(free_range.base)
                alloc_start = align_down(fs_up, alignment) + offset
                alloc_end = alloc_start + size - 1

                if alloc_start < fs_up:
                    alloc_start += alignment
                    alloc_end += alignment
            else:
                alloc_start = align_down(free_range.base, alignment) + offset
                alloc_end = alloc_start + size - 1

            # If alignment adjustments push the block below the
            # start of the free region, bump it up.
            if alloc_start < free_range.base:
                alloc_start += alignment
                alloc_end += alignment

            # If the range is within free memory, we've found it.
            if free_range.contains(self.Range(alloc_start, alloc_end, None)):
                # Put the remaining parts of the region back into the
                # correct freelists.
                if free_range.base < alloc_start - 1:
                    before_free.append(self.Range(free_range.base,
                                                  alloc_start - 1,
                                                  free_range.cache_rights))

                if alloc_end + 1 < free_range.end:
                    after_free.append(self.Range(alloc_end + 1,
                                                 free_range.end,
                                                 free_range.cache_rights))

                break
            else:
                # Not useful, so add to the before list.
                before_free.append(free_range)
        else:
            # Abort if nothing suitable was found.
            raise AllocatorException, "Unable to allocate memory of size: " \
                "0x%x aligned: 0x%x offset: 0x%x" % (size, alignment, offset)

        # Copy any remaining free list records into the after_freelist
        after_free.extend(free_iter)

        return (before_free, after_free, alloc_start, free_range.cache_rights)

    def __alloc_tied(self, them, freelist, tied_freelist, my_addr,
                     tied_addr, size, alignment, offset, force_page,
                     alloc_tied):
        """
        Allocate memory for an item from this allocator and the other
        supplied allocator.  Allocation will be successful if both
        allocators can supply the same address.

        This is used to implement direct allocation.

        The algorithm will run through the freelist of both allocators
        looking for the first shared address.  A normal allocation is
        then done from that point in both allocators.
        """
        def partition_freelist(freelist, addr, size):
            """
            Partition the free list into two parts, one of which is
            all parts up to the given address and the other is all
            parts after the given address.  The address itself should
            *not* be in the freelist.  The address does not have to be
            page aligned.  The size of the item at address is passed
            so that freelist can be correctly prepared for subpage
            allocation after the item.
            """
            before = []
            after = []

            # Is the item in the range of the freelist?
            if freelist and addr <= freelist[-1].end:
                # Split the freelist around the fixed address so that
                # the next item will be allocated at a higher
                # addresses.  Also check that the address is *not* in
                # the freelist, as the rest of the assumes that it is not.
                for free_range in freelist:
                    if free_range.end < addr:
                        before.append(free_range)
                    else:
                        if free_range.base <= addr:
                            raise AllocatorException, \
                                  "Allocated address %#x found in " \
                                  "freelist." % addr

                        after.append(free_range)

                # If the item doesn't occupy the full page then
                # put the remain part back to allow subpage
                # allocation of the next item.
                if (addr + size) % self.min_alloc != 0:
                    if self._align_up(addr + size) == \
                           after[0].base:
                        after[0].base = addr + size
                    else:
                        after = [Allocator.Range(addr + size,
                                                 self._align_up(addr +
                                                                size - 1),
                                                 None)] + after

            else:
                # If the item's address is outside the freelist range,
                # then continue allocation from the current point.
                after = freelist

            return (before, after)

        def partition_freelists(this_freelist, that_freelist, addr, size):
            """
            Partition the two freelists around the given address.
            Return a tuple in the same form at the other allocation
            functions below.
            """
            (this_before, this_freelist) = \
                          partition_freelist(this_freelist, addr, size)
            (that_before, that_freelist) = \
                          partition_freelist(that_freelist, addr, size)

            return (addr, addr,
                    this_before,
                    this_freelist,
                    that_before,
                    that_freelist,
                    None,
                    None)

        def alloc_to_addr(addr, this_freelist, that_freelist,
                          size, alignment, offset, force_page,
                          alloc_that):
            """
            Allocate memory from 'that' freelist that corresponds to
            the given address.  Return both freelists partitioned
            around the address.

            In this case, 'this' could be with the current allocator
            or the tied allocator ('me' or 'them') and 'that' is the
            reverse.  This handles the case where one address has
            already be allocated and we're trying to allocate the same
            address from the other allocator.
            """
            assert addr is not None

            # In keeping with group allocation, partition this
            # freelist so that further allocations will take place
            # above the address.
            (this_before, this_freelist) = \
                          partition_freelist(this_freelist, addr, size)

            # Search 'that' freelist for the record containing the
            # address.
            that_free_iter = iter(that_freelist)
            that_curr_free = that_free_iter.next()
            that_before = []

            while that_curr_free.end < addr:
                that_before.append(that_curr_free)
                that_curr_free = that_free_iter.next()

            # If the free list does not contain an entry at the
            # desired address, fail. The StopIteration exception is
            # raised because this is the same type of condition as
            # running off the end of the freelist in
            # base_alloc_tied(), which also raises this exception.  In
            # both cases it is caught and an AllocatorException is
            # raised with an appropriate error message.
            if that_curr_free.base > addr:
                raise StopIteration

            # Partition the freelist around the address point.
            if that_curr_free.base < addr:
                that_before.append(Allocator.Range(that_curr_free.base,
                                                   addr - 1,
                                                   that_curr_free.cache_rights))
                that_curr_free = Allocator.Range(addr,
                                                 that_curr_free.end,
                                                 that_curr_free.cache_rights)

            # Regenerate the after freelist for the allocation.
            that_remaining_freelist = [that_curr_free]
            that_remaining_freelist.extend(that_free_iter)

            # Now allocate from the same starting point.
            # pylint: disable-msg=W0212
            if alloc_that:
                (that_more_before, that_freelist, that_addr,
                 that_cache_rights) = \
                 self.__simple_alloc(that_remaining_freelist,
                                     size,
                                     alignment,
                                     offset,
                                     force_page)
            else:
                (that_more_before, that_freelist, that_addr,
                 that_cache_rights) = \
                 ([], that_remaining_freelist, None, None)

            return (
                addr, that_addr,
                this_before,
                this_freelist,
                that_before + that_more_before,
                that_freelist,
                None,
                that_cache_rights)

        def base_alloc_tied(self, them, freelist, tied_freelist,
                            size, alignment, offset, force_page,
                            alloc_tied):
            """
            Find the first common address in both freelists and
            try and allocate from there.  This is the most general
            form of tied allocation.
            """
            my_free_iter = iter(freelist)
            their_free_iter = iter(tied_freelist)

            my_curr_free = my_free_iter.next()
            their_curr_free = their_free_iter.next()

            my_new_freelist = []
            their_new_freelist = []

            # Loop through both freelists looking for two record that
            # overlap (and thus share a common address).  The loop
            # exits either whenever an overlap is found or when the
            # end of either freelist is reached (in that case
            # iter.next() will raise a exception, which is caught
            # below and turned into an allocator error.
            # Discarded records are kept in a list for later use then
            # rebuilding the freelist.
            while True:
                if my_curr_free.end < their_curr_free.base:
                    my_new_freelist.append(my_curr_free)
                    my_curr_free = my_free_iter.next()
                elif their_curr_free.end < my_curr_free.base:
                    their_new_freelist.append(their_curr_free)
                    their_curr_free = their_free_iter.next()
                else:
                    break

            # OK, we've found the sync point.
            # Make sure that that is the case.
            assert my_curr_free.overlap(their_curr_free)
            # If my record starts at a lower address then their,
            # partition my freelist around the base of their record so
            # that both freelists start at the same point.
            if my_curr_free.base < their_curr_free.base:
                assert my_curr_free.end > their_curr_free.base

                my_new_freelist.append(self.Range(my_curr_free.base,
                                                  their_curr_free.base - 1,
                                                  my_curr_free.cache_rights))
                my_curr_free = self.Range(their_curr_free.base,
                                          my_curr_free.end,
                                          my_curr_free.cache_rights)

            # If their record starts at a lower address then mine,
            # partition their freelist around the base of my record so
            # that both freelists start at the same point.
            if their_curr_free.base < my_curr_free.end:
                assert their_curr_free.end > my_curr_free.base

                their_new_freelist.append(
                    self.Range(their_curr_free.base,
                               my_curr_free.base - 1,
                               their_curr_free.cache_rights))

                their_curr_free = self.Range(my_curr_free.base,
                                             their_curr_free.end,
                                             their_curr_free.cache_rights)

            # Rebuild the freelists to allocate from, which is
            # currently the remaining part of <my,their>_curr_free
            # plus the contents of the respective iterator.
            my_remaining_freelist = [my_curr_free]
            my_remaining_freelist.extend(my_free_iter)

            their_remaining_freelist = [their_curr_free]
            their_remaining_freelist.extend(their_free_iter)

            # Now allocate from the same starting point.
            # pylint: disable-msg=W0212
            (my_before, my_freelist, my_addr, my_cache_rights) = \
                     self.__simple_alloc(my_remaining_freelist,
                                         size,
                                         alignment,
                                         offset,
                                         force_page)

            if alloc_tied:
                (their_before, their_freelist, their_addr,
                 their_cache_rights) = \
                 them.__simple_alloc(their_remaining_freelist,
                                     size,
                                     alignment,
                                     offset,
                                     force_page)
            else:
                (their_before, their_freelist, their_addr,
                 their_cache_rights) = \
                 ([], their_remaining_freelist, None, None)

            return (my_addr, their_addr,
                    my_new_freelist + my_before,
                    my_freelist,
                    their_new_freelist + their_before,
                    their_freelist,
                    my_cache_rights,
                    their_cache_rights)

        # Start of __alloc_tied()

        # Sanity check the inputs.
        # If both addresses are fixed, then they had better be the same!
        if my_addr is not None and tied_addr is not None and \
               my_addr != tied_addr:
            raise AllocatorException, \
                  "Tied allocation requested with different fixed " \
                  "addresses (%#x and %#x)" % (my_addr, tied_addr)

        # If allocation is needed (the address isn't fixed) then there
        # has better be a freelist.  Beyond this point subfunctions
        # can assume that the freelists contain at least one record.
        if my_addr is None and not self.freelist:
            raise AllocatorException, \
                  "Out of memory in this allocator."

        if tied_addr is None and not tied_freelist:
            raise AllocatorException, \
                  "Out of memory in that allocator."

        try:
            #
            # Handle the 4 different combinations of tied allocation:
            #
            # 1) Both already have addresses (and they are the same)
            if my_addr is not None and tied_addr is not None:
                assert my_addr == tied_addr

                (my_addr, their_addr,
                 my_before,
                 my_freelist,
                 their_before,
                 their_freelist,
                 my_cache_rights,
                 their_cache_rights) = \
                 partition_freelists(freelist, tied_freelist, my_addr, size)
            # 2) My address is known, so allocate theirs.
            elif my_addr is not None:
                (my_addr, their_addr,
                 my_before,
                 my_freelist,
                 their_before,
                 their_freelist,
                 my_cache_rights,
                 their_cache_rights) = \
                 alloc_to_addr(my_addr, freelist, tied_freelist, size,
                               alignment, offset, force_page, alloc_tied)
            # 3) Their address is known so allocate mine.
            elif tied_addr is not None:
                (their_addr, my_addr,
                 their_before,
                 their_freelist,
                 my_before,
                 my_freelist,
                 their_cache_rights,
                 my_cache_rights) = \
                 alloc_to_addr(tied_addr, tied_freelist, freelist, size,
                               alignment, offset, force_page, True)
            # 4) Neither address is known so find the lowest common
            #    point.
            else:
                (my_addr, their_addr,
                 my_before,
                 my_freelist,
                 their_before,
                 their_freelist,
                 my_cache_rights,
                 their_cache_rights) = \
                 base_alloc_tied(self, them, freelist, tied_freelist, size,
                                 alignment, offset, force_page, alloc_tied)

            # Bother!  The freelists are differently fragmented so
            # that same address can't be generated from both
            # allocators.  Too bad :-)
            if alloc_tied and my_addr != their_addr:
                raise AllocatorException, \
                      "Allocation did not produce same address from both " \
                      "allocators (%#x and %#x)" % (my_addr, their_addr)

            return (my_before,
                    my_freelist,
                    their_before,
                    their_freelist,
                    my_addr,
                    my_cache_rights,
                    their_cache_rights)

        except StopIteration:
            raise AllocatorException, \
                  "Could not find common address in pools for direct " \
                  "allocation."


    def __check_cache_permissions(self, item, cache_rights):
        """
        Convenience function to check that the item's cache policy is
        compatible with the cache rights of the allocated memory.
        """
        if not self.machine.check_cache_permissions(item.get_cache_attr(),
                                                    cache_rights):
            raise AllocatorException("Cache attr \"%s\" is not allowed by the "
                                     "cache rights \"%s\"." % \
                                     (self.machine.cache_attr_to_str(item.get_cache_attr()),
                                      self.machine.cache_perms_to_str(cache_rights)))

    def __restore_freelist(self, freelist_parts):
        """
        Update the freelist and restore the invariant by removing
        all partial pages.
        """
        self.freelist = []

        for free_range in chain(*freelist_parts):
            if not self._aligned(free_range.base):
                free_range.base = self._align_up(free_range.base)

            if not self._aligned(free_range.end + 1):
                free_range.end = self._align_down(free_range.end) - 1

            # Don't include pages that have been rounded out of
            # existence.
            if free_range.base < free_range.end:
                assert(free_range.cache_rights is not None)
                self.freelist.append(free_range)

    def alloc(self, item):
        """
        Allocate an item, giving it the lowest address possible that
        satisfies its alignment requirements.  An item may be tied, in
        which case the same address is given to this items and its
        tied item.
        """
        if item.get_tied_item() is not None:
            # Allocating tied items needs a completely different
            # approach to regular items.

            # In tied allocation, 'my', 'me' and similar refer to
            # parts of the this allocator's freelist, while
            # 'them', 'their', etc refer to the parts of the tied
            # item's freelist.

            tied = item.get_tied_item()
            them = tied.get_allocator()

            if (item.get_addr() == self.IGNORE or
                tied.get_addr() == self.IGNORE):
                raise AllocatorException, \
                      "%s: The IGNORE value is not allowed in tied " \
                      "allocation." % item.get_name()


            (my_before, my_freelist, their_before, their_freelist,
             addr, my_cache_rights, their_cache_rights) = \
                     self.__alloc_tied(them,
                                       self.freelist,
                                       them.freelist,
                                       item.get_addr(),
                                       tied.get_addr(),
                                       item.get_size(),
                                       item.get_alignment(),
                                       item.get_offset(),
                                       item.force_page(),
                                       tied.alloc_tied())

            # Check that the cache rights are compatible.  The rights
            # may be None if the address if one of the items is
            # already known.  In that case an allocation isn't
            # performed now (it should have happened earlier anyway)
            # and the rights are not known.
            if my_cache_rights is not None:
                self.__check_cache_permissions(item, my_cache_rights)
            if tied.alloc_tied() and their_cache_rights is not None:
                self.__check_cache_permissions(item, their_cache_rights)

            # Reconstruct and restore the invariant of both freelists
            # from the parts that we skipped over when looking for the
            # sync point plus what the simple allocator returned.
            # pylint: disable-msg=W0212
            self.__restore_freelist([my_before, my_freelist])
            them.__restore_freelist([their_before, their_freelist])

            # Inform both allocator's trackers.
            if self.tracker is not None and addr != item.get_addr():
                self.tracker.track_alloc(item.get_name(), addr,
                                         addr + item.get_size() - 1)

            if tied.alloc_tied() and them.tracker is not None and \
                   addr != tied.get_addr():
                them.tracker.track_alloc(item.get_name(), addr,
                                         addr + item.get_size() - 1)

            # Success at long last!  Finally update the item's address.
            item.set_addr(addr)

            if tied.alloc_tied():
                tied.set_addr(addr)
        elif item.get_addr() is None:
            (before_fl, after_fl, addr, cache_rights) = \
                        self.__simple_alloc(self.freelist, item.get_size(),
                                            item.get_alignment(),
                                            item.get_offset(),
                                            item.force_page())

            # Check that the cache rights are compatible.
            self.__check_cache_permissions(item, cache_rights)

            # Report the allocation to the tracker, if there is one.  For
            # completely accurate coverage the pages that were allocated
            # should really be tracked, but that is:
            # a) really hard.
            # b) not what the original version did.

            if self.tracker is not None:
                self.tracker.track_alloc(item.get_name(), addr,
                                     addr + item.get_size() - 1)
            # Update the freelist and restore the invariant by removing
            # all partial pages.
            self.__restore_freelist([before_fl, after_fl])

            item.set_addr(addr)

    def alloc_group(self, group):
        """
        Allocate a group of items.  The items will be allocated at
        increasing addresses and may share pages if size and alignment
        allow it.  This differs from alloc() where whole pages are
        consumed.  The group can contain a mixture of tied and untied
        items.  No guarantees are given as to how close the items will
        be placed in memory.
        """
        addrs = []
        # Indexes for the before and after parts of the freelist.
        # While working through the freelist, entries that have been
        # processed are collectively known as 'before' while entries
        # yet to be processed as known as 'after' (or 'freelist').
        BF = 0
        AF = 1

        # The tied items can from any allocator, so keep a hash table
        # of the allocate freelists currently in flight as the items
        # are processed.  Include the freelist for this allocator to
        # make make final cleanup at the end easier.
        active_alloc = {}
        my_alloc_state = active_alloc.setdefault(self,
                                                 [[], self.freelist])

        for item in group:
            # Allocating tied items needs a completely different
            # approach to regular items.
            if item.get_tied_item() is not None:
                # In tied allocation, 'my', 'me' and similar refer to
                # parts of the this allocator's freelist, while
                # 'them', 'their', etc refer to the parts of the tied
                # item's freelist.
                tied = item.get_tied_item()
                them = tied.get_allocator()

                them_alloc_state = active_alloc.setdefault(them,
                                                           [[], them.freelist])

                if (item.get_addr() == self.IGNORE or
                    tied.get_addr() == self.IGNORE):
                    raise AllocatorException, \
                          "%s: The IGNORE value is not allowed in tied " \
                          "allocation." % item.get_name()

                (my_before, my_alloc_state[AF], their_before,
                 them_alloc_state[AF], addr, my_cache_rights,
                 their_cache_rights) = \
                 self.__alloc_tied(them,
                                   my_alloc_state[AF],
                                   them_alloc_state[AF],
                                   item.get_addr(),
                                   tied.get_addr(),
                                   item.get_size(),
                                   item.get_alignment(),
                                   item.get_offset(),
                                   item.force_page(),
                                   tied.alloc_tied())

                # Check that the cache rights are compatible.  The
                # rights may be None if the address if one of the
                # items is already known.  In that case an allocation
                # isn't performed now (it should have happened earlier
                # anyway) and the rights are not known.
                if my_cache_rights is not None:
                    self.__check_cache_permissions(item,
                                                   my_cache_rights)
                if tied.alloc_tied() and their_cache_rights is not None:
                    self.__check_cache_permissions(item,
                                                   their_cache_rights)

                my_alloc_state[BF].extend(my_before)
                them_alloc_state[BF].extend(their_before)
            elif item.get_addr() is None:
                # If no address then perform a straightforward
                # allocation from the current point.
                (before_fl, my_alloc_state[AF], addr, cache_rights) = \
                         self.__simple_alloc(my_alloc_state[AF],
                                             item.get_size(),
                                             item.get_alignment(),
                                             item.get_offset(),
                                             item.force_page())

                # Check that the cache rights are compatible.
                self.__check_cache_permissions(item, cache_rights)

                my_alloc_state[BF].extend(before_fl)
            elif item.get_addr() is not self.IGNORE:
                # If an address has been provided, move the current
                # free list point to after that address.
                addr = item.get_addr()

                # If the item's address is outside the freelist range,
                # then continue allocation from the current point.
                if my_alloc_state[AF] and addr <= my_alloc_state[AF][-1].end:
                    # Split the freelist around the fixed address to that
                    # the next item will be allocated at a higher
                    # addresses.  Problems will arise fixed items
                    # are placed in the list unsorted.
                    updated_freelist = []
                    for free_range in my_alloc_state[AF]:
                        if free_range.end < addr:
                            my_alloc_state[BF].append(free_range)
                        else:
                            assert free_range.base > addr
                            updated_freelist.append(free_range)

                    # If the item doesn't occupy the full page then
                    # put the remain part back to allow subpage
                    # allocation of the next item.
                    if (addr + item.get_size()) % self.min_alloc != 0:
                        if self._align_up(addr + item.get_size()) == \
                           updated_freelist[0].base:
                            updated_freelist[0].base = addr + item.get_size()
                        else:
                            updated_freelist = [
                                Allocator.Range(addr + item.get_size(),
                                                self._align_up(addr +
                                                               item.get_size() -
                                                               1),
                                                None)] + updated_freelist
                    # Use the freelist above the fixed item for further
                    # allocations.
                    my_alloc_state[AF] = updated_freelist
            else:
                addr = item.get_addr()

            addrs.append(addr)

        # Now that all of the allocations have been done it is safe to
        # assign to each item its address.
        for (item, addr) in zip(group, addrs):
            # If appropiate, assign the address to the tied item as
            # well.
            if item.get_tied_item() is not None:
                tied = item.get_tied_item()
                them = tied.get_allocator()

                if tied.alloc_tied() and them.tracker is not None and \
                       addr != tied.get_addr():
                    them.tracker.track_alloc(item.get_name(), addr,
                                             addr + item.get_size() - 1)

                if tied.alloc_tied():
                    tied.set_addr(addr)

            # Report the allocation to the tracker, if there
            # is one.  For completely accurate coverage the
            # pages that were allocated should really be
            # tracked, but that is:
            # a) really hard.
            # b) not what the original version did.
            if self.tracker is not None and \
                   addr != item.get_addr():
                self.tracker.track_alloc(item.get_name(), addr,
                                         addr + item.get_size() - 1)
            item.set_addr(addr)

        # Update the freelist and restore the invariant by removing
        # all partial pages.
        for (alloc, state) in active_alloc.items():
            # pylint: disable-msg=W0212
            alloc.__restore_freelist(state)

    def next_avail(self):
        """
        Return the base address of the biggest block of memory in
        the free list.
        """

        if not self.freelist:
            raise AllocatorException, \
                      "next_avail(): Free list empty."

        return max([(fr.end - fr.base, fr.base) for fr in self.freelist])[1]

    def get_lowest_addr(self):
        """
        Return the lowest available address in the freelist.
        """

        if not self.freelist:
            raise AllocatorException, \
                      "get_lowest_addr: Free list empty."

        return self.freelist[0].base

    def get_total_size(self):
        """
        Return the total size of all memory contained in this allocator.
        """
        return self.total_size
