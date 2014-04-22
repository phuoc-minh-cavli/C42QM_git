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

from elf.util import align_up, align_down

class RangeAllocatorException(Exception):
    """Exception to be raised if we have a problem in the range allocator."""

class RangeAllocator(object):
    """Given a range, provide an interface allowing intervals in this range to
    be allocated. Intervals may have alignment requests."""

    def __init__(self, base, size):
        """Create a new RangeAllocator, able to allocate ranges starting from
        'base' up to size 'size'."""
        self.base = base
        self.size = size
        self.free_list = [(base, size)]

    def _split_node(self, node_base, node_end, alloc_base, alloc_end):

        # Split the node in two nodes: before and after
        replacements = []
        if alloc_base - node_base > 0:
            replacements.append(
                    (node_base, alloc_base - node_base))
        if node_end - alloc_end > 0:
            replacements.append(
                    (alloc_end, node_end - alloc_end))
        return replacements

    def allocate_range(self, base, size):
        """
        Allocate the given range.
        """
        end = base + size

        # Search through the free list, trying to find where the range falls
        for i, (node_start, node_length) in enumerate(self.free_list):
            node_end = node_start + node_length

            # If the allocation fits, stop searching
            if node_start <= base and node_end >= end:
                # Split the node into two nodes: the before and after.
                self.free_list[i:i+1] = self._split_node(node_start, node_end, base, end)
                break
        else:
            # Raise an exception if the allocation failed.
            raise RangeAllocatorException(
                    "Can not allocate range %08x--%08x." % (base, base + size))


    def allocate(self, size, alignment = 1, tail_alignment = 1):
        """Allocate an interval from the free range area."""

        aligned_alloc_start = None
        aligned_alloc_end = None

        # Search for a free node large enough for the allocation.
        for i, (node_start, node_length) in enumerate(self.free_list):
            node_end = node_start + node_length
            if tail_alignment == 1:
                aligned_alloc_start = align_up(node_start, alignment)
                aligned_alloc_end = aligned_alloc_start + size
            else:
                aligned_alloc_end = align_down(node_end, tail_alignment)
                aligned_alloc_start = aligned_alloc_end - size

            # If the allocation fits, stop searching.
            if (node_end >= aligned_alloc_end) and \
                    (node_start <= aligned_alloc_start):
                # Split the node into two nodes: the before and after.
                self.free_list[i:i+1] = self._split_node(node_start, node_end,
                                                         aligned_alloc_start,
                                                         aligned_alloc_end)
                break
        else:
            # Raise an exception if the allocation failed.
            raise RangeAllocatorException(
                    "Can not allocate interval of size %d." % size)


        # Return the allocation to the caller.
        return (aligned_alloc_start, size)

