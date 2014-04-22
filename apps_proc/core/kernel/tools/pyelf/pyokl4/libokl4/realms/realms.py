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

# This is a python implementation of libs/okl4/src/realm.c

from pyokl4.autogen import Autogen
from elf.util import align_up, align_down

INVALID_ADDR = ~0
CHUNK_SIZE = 0x100000
# CHUNK_AVAILABLE is actually a Memsection object, setup later. Nobody should
# use it before it is set up, so this value should cause an error if it is.
CHUNK_AVAILABLE = ~1
class MEGACHUNK(object):
    def __init__(self, chunk):
        self.first_chunk = chunk

class RealmsException(Exception):
    pass

def CHUNK_IS_MEGA(chunk):
    return isinstance(chunk.memsection, MEGACHUNK)

def MEGACHUNK_FIRST(chunk):
    return chunk.memsection.first_chunk

def contains(a, b, x):
    return (a <= x) and (x < b)

def overlap(a, b, x, y):
    return contains(a, b, x) or contains(x, y, a)

def okl4_protection_domain_create(env_pd_obj, address_space):
    if hasattr(address_space, "pds"):
    # Simply inserts a protection domain into the address space object.
    # The rest of the population is done by the code in cells/pd.py
        id_ = env_pd_obj.id_
        assert address_space.pds[id_] is None
    # This pd object will need to be resolved later as we don't create them
    # just yet.
        address_space.pds[id_] = env_pd_obj

def okl4_address_space_create(address_space, first_chunk, num_chunks,
             first_reserved_chunk = -1, num_reserved_chunks = 0):
    address_space.first_chunk = first_chunk
    address_space.num_chunks = num_chunks

    if first_reserved_chunk == -1:
        first_reserved_chunk = first_chunk

    if (first_reserved_chunk < first_chunk) or \
        (first_reserved_chunk + num_reserved_chunks > first_chunk + num_chunks):
        raise RealmsException("Invalid arguments to address_space_create.")

    address_space.first_reserved_chunk = first_reserved_chunk - first_chunk
    address_space.num_reserved_chunks = num_reserved_chunks

    address_space.chunks = []
    for i in xrange(address_space.num_chunks):
        chunk = Autogen.types.OKL4_chunk(0)
        chunk.memsection = CHUNK_AVAILABLE
        address_space.chunks.append(chunk)

def okl4_address_space_eliminate(address_space, first_chunk, num_chunks):
    # Knock out any whole 1MB chunks.
    # This may need to change to be more fine grained.
    start_idx = first_chunk - address_space.first_chunk
    end_idx = first_chunk + num_chunks - address_space.first_chunk
    for i in xrange(start_idx, end_idx):
        address_space.chunks[i].memsection = None

def okl4_address_space_exclude_all_but(address_space, base, size):
    # Marks the entire rest of the address space as allocated, except for the
    # region between base and base + size
    end = base + size

    # Round to CHUNK_SIZE boundaries for now. See below.
    base = align_up(base, CHUNK_SIZE)
    end = align_down(end, CHUNK_SIZE)

    for i in xrange(address_space.num_chunks):
        addr = (address_space.first_chunk + i) * \
                CHUNK_SIZE

        if not overlap(base, end, addr, addr + CHUNK_SIZE):
            # Knock out everything in this chunk.
            if address_space.chunks[i].memsection == CHUNK_AVAILABLE:
                # If the chunk is available, mark it as None.
                address_space.chunks[i].memsection = None
            elif address_space.chunks[i].memsection is None:
                # Already good.
                pass
            elif CHUNK_IS_MEGA(address_space.chunks[i]):
                # Already good.
                pass
            else:
                # Otherwise, we need to clear the avail_before/after of every
                # allocation in this chunk.
                cur = address_space.chunks[i].memsection
                while cur is not None:
                    if cur.avail_before > 0 or cur.avail_after > 0:
                        cur.avail_before = 0
                        cur.avail_after = 0
                        # Remove the target from the allocation list.
                        new_target, last_target = _search_list_t(cur.realm.alloc,
                                cur)
                        assert cur is not None
                        assert new_target == cur
                        if last_target is None:
                            cur.realm.alloc = cur.alloc_next
                        else:
                            last_target.alloc_next = cur.alloc_next

                    cur = cur.lookup_next
        else:
            # We need to knock out just a part of this chunk.
            # TODO: We assume the base/size have been rounded to CHUNK_SIZE
            # boundaries for now. Anything else gets more complex.
            pass

def okl4_address_space_chunk_alloc_fixed(address_space, base_chunk, num_chunks,
        alloc_reserved):
    assert base_chunk < address_space.num_chunks
    assert base_chunk + num_chunks <= address_space.num_chunks
    assert address_space.chunks[base_chunk].memsection == CHUNK_AVAILABLE

    reserved_start_chunk = address_space.first_reserved_chunk
    reserved_end_chunk = reserved_start_chunk + \
            address_space.num_reserved_chunks

    # Don't allocate from the reserved area when we're not asked to.
    if not alloc_reserved and address_space.num_reserved_chunks > 0 and \
        overlap(base_chunk, base_chunk + num_chunks,
                reserved_start_chunk, reserved_end_chunk):
        return None

    for i in xrange(base_chunk + 1, base_chunk + num_chunks):
        if address_space.chunks[i].memsection != CHUNK_AVAILABLE:
            return None

    # Mark them all as allocated now
    address_space.chunks[base_chunk].memsection = None
    for i in xrange(base_chunk + 1, base_chunk + num_chunks):
        address_space.chunks[i].memsection = \
                MEGACHUNK(address_space.chunks[base_chunk])

    return address_space.chunks[base_chunk]

def okl4_address_space_find_first_chunk(chunk):
    while CHUNK_IS_MEGA(chunk):
        chunk = MEGACHUNK_FIRST(chunk)
    return chunk

def okl4_address_space_chunk_base(address_space, chunk):
    chunk_index = address_space.chunks.index(chunk)
    assert chunk_index >= 0 and chunk_index < address_space.num_chunks
    assert not CHUNK_IS_MEGA(chunk)
    return (address_space.first_chunk + chunk_index) * CHUNK_SIZE

# Chunky stuff here.

def _okl4_chunk_memsection_insert(chunk, memsection):
    assert chunk.memsection != CHUNK_AVAILABLE

    # We simply insert at the head if it is empty.
    if chunk.memsection is None:
        chunk.memsection = memsection
        return

    last = None
    # Find the memsection with address past ours, remember the previous along
    # the way so we can do the insert.
    target = chunk.memsection
    while target is not None:
        if target.base > memsection.base:
            break
        last = target
        target = target.lookup_next

    if last is None:
        chunk.memsection = memsection
    else:
        last.lookup_next = memsection

    memsection.lookup_next = target

# Realm stuff be here.

def okl4_realm_create(realm, address_space):
    '''Initialises a OKL4_realm object.'''
    if address_space is None:
        raise RealmsException("No address space specified for realm creation.")
    realm.address_space = address_space
    realm.next_realm = realm.address_space.realms
    realm.address_space.realms = realm
    if hasattr (realm, "attached_pds_R"):
        for i in xrange(0, len(realm.attached_pds_R)):
            realm.attached_pds_R[i] = 0
            realm.attached_pds_W[i] = 0
            realm.attached_pds_X[i] = 0

# Search the list for a specific target memsection.
def _search_list_t(head, needle):
    '''Search the list for a specific target memsection.'''
    prev = None
    target = head
    while target is not None:
        if target == needle:
            break
        prev = target
        target = target.alloc_next

    return (target, prev)

def _taint_memsection(memsection):
    # We don't support tainting yet.
    # We need to support this at some point in order to have things like ELF
    # file segments exist in several realms.
    raise NotImplementedError

def print_chunk(chunk):
    cur = chunk.memsection
    print 'Chunk', chunk
    while cur != None:
        tmp_base = cur.base
        tmp_end = cur.base + cur.size
        tmp_abase = tmp_base - cur.avail_before
        tmp_aend = tmp_end + cur.avail_after
        print "%#x %#x %#x %#x" % (tmp_abase, tmp_base, tmp_end, tmp_aend)
        cur = cur.lookup_next

def remove_from_alloc(memsection):
    if memsection.is_tainted:
        list_head = memsection.realm.address_space.taint_list
    else:
        list_head = memsection.realm.alloc

    found, prev = _search_list_t(list_head, memsection)
    assert found == memsection

    if prev is None:
        if memsection.is_tainted:
            memsection.realm.address_space.taint_list = memsection.alloc_next
        else:
            memsection.realm.alloc = memsection.alloc_next
    else:
        prev.alloc_next = memsection.alloc_next

def okl4_realm_memsection_allocate_fixed(realm, memsection):
    address_space = realm.address_space
    ms_base = memsection.base
    ms_end = ms_base + memsection.size

    first_chunk_index = (memsection.base - address_space.first_chunk *
                        CHUNK_SIZE) / CHUNK_SIZE
    last_chunk_index = \
        (memsection.base - (address_space.first_chunk *
            CHUNK_SIZE) + memsection.size - 1) / CHUNK_SIZE

    # Ensure that the allocation will actually fit inside our address space.
    if (first_chunk_index < 0) or (last_chunk_index < 0) or \
        (last_chunk_index >= address_space.num_chunks) or \
        (first_chunk_index > last_chunk_index):
        raise RealmsException("Allocation outside address space.")

    # Grab the chunks and find the chunk indexes again from the array index (it
    # may have been a megachunk).
    first_chunk = address_space.chunks[first_chunk_index]
    first_chunk = okl4_address_space_find_first_chunk(first_chunk)
    first_chunk_index = address_space.chunks.index(first_chunk)

    last_chunk = address_space.chunks[last_chunk_index]
    last_chunk = okl4_address_space_find_first_chunk(last_chunk)
    last_chunk_index = address_space.chunks.index(last_chunk)

    # Step 1: Verify that there is indeed space for the allocation.
    # The allocation may span chunks, some of which may be partly allocated.
    # e.g.
    # |  chunk    | free chunk | free chunk | free chunk |   chunk   |
    # | ms | free |                                      | free | ms |
    #      |<-------------------- allocation -----------------.|
    for i in xrange(first_chunk_index, last_chunk_index + 1):
        chunk = address_space.chunks[i]
        if not CHUNK_IS_MEGA(chunk) and (chunk.memsection != CHUNK_AVAILABLE):
            if chunk.memsection is None:
                # No allocations free here. Not a chance.
                raise RealmsException("Cannot allocate memsection.")
            # If any memsections within this chunk overlap with what we want
            # to allocate, we are sunk. This handles the overlap cases at both
            # ends, as well as the common case where the allocation is
            # entirely within one chunk.
            cur = chunk.memsection
            while cur is not None:
                if overlap(cur.base, cur.base + cur.size, ms_base, ms_end):
                    raise RealmsException("Cannot allocate memsection: " + \
                        "%#x-%#x overlaps with %#x-%#x" % (
                        ms_base, ms_end, cur.base, cur.base + cur.size))
                cur = cur.lookup_next

    # We are now certain that it is possible to make the allocation. We cannot
    # fail after this point.
    #
    # However, we may need to merge some chunks in order to achieve it.

    # Step 2: If the memsection falls entirely within available chunks, then
    # our job is really easy. We handle this separately as the merging logic
    # in Step 3 requires at least one memsection.
    all_free = True
    for i in xrange(first_chunk_index, last_chunk_index + 1):
        chunk = address_space.chunks[i]
        if chunk.memsection != CHUNK_AVAILABLE:
            all_free = False
            break

    if all_free:
        # In this case, the allocation looks something like:
        # | free chunk | free chunk | free chunk | free chunk | free chunk |
        #      |<-------------------- allocation ------------------->|
        chunks_required = (last_chunk_index - first_chunk_index) + 1

        alloc_chunk = okl4_address_space_chunk_alloc_fixed(address_space,
             first_chunk_index, chunks_required, 1)
        assert alloc_chunk == first_chunk
        chunk_base = okl4_address_space_chunk_base(address_space, alloc_chunk)
        chunk_end = chunk_base + (CHUNK_SIZE * chunks_required)

        memsection.avail_before = ms_base - chunk_base
        memsection.avail_after = chunk_end - ms_end

        # Insert memsection into allocation list
        if memsection.avail_before > 0 or memsection.avail_after > 0:
            memsection.alloc_next = realm.alloc
            realm.alloc = memsection

        memsection.chunk = alloc_chunk
        _okl4_chunk_memsection_insert(alloc_chunk, memsection)
        return

    # Step 3: If the new memsection spans chunk boundaries and the first chunk
    # is available, merge all available chunks into one.  We know that we have
    # at least one memsection in the chunks we care about (otherwise we would
    # have exited at step 2.
    #
    # After this step, the target memsection that we will inject ourselves
    # into will be in the first chunk. (However, there may be subsequent
    # chunks to worry about).
    if first_chunk.memsection == CHUNK_AVAILABLE:
        # In this case, the allocation looks something like:
        # | free chunk | free chunk | free chunk | free chunk |   chunk   |
        # |                                                   | free | ms |
        #      |<-------------------- allocation ------------------->|
        assert first_chunk != last_chunk
        first_last_ms = last_chunk.memsection

        # Ensure there is an allocation at the start of the first memsection
        # in the last chunk.
        assert first_last_ms.avail_before > 0

        # We need to merge all subsequent chunks into this one.
        for i in xrange(first_chunk_index + 1, last_chunk_index):
            chunk = address_space.chunks[i]
            assert chunk.memsection == CHUNK_AVAILABLE
            first_last_ms.avail_before += CHUNK_SIZE
            chunk.memsection = MEGACHUNK(first_chunk)

        # Hook up the last chunk's memsections into our own.
        # For each memsection in the chunk we are adopting, set their
        # chunk pointer to their new leader.
        cur = first_last_ms
        while cur is not None:
            cur.chunk = first_chunk
            cur = cur.lookup_next

        # Add the first chunk to the avail_before of this memsection.
        first_last_ms.avail_before += CHUNK_SIZE

        # Take it as our first memsection.
        first_chunk.memsection = first_last_ms
        last_chunk.memsection = MEGACHUNK(first_chunk)

    # Step 4: Find the memsection that contains (in avail_before, or
    # avail_after) the memory for our memsection.
    #
    # We now know that this must exist inside a memsection in the first chunk,
    # so we cannot fail.

    target_abase = 0
    target_aend = 0
    target_end = 0
    target_base = 0
    target = first_chunk.memsection
    while target is not None:
        # Check if the target might contain our request
        target_base = target.base
        target_end = target.base + target.size
        target_abase = target_base - target.avail_before
        target_aend = target_end + target.avail_after
        if target_abase <= ms_base and ms_base < target_aend:
            assert target is not None
            break
        target = target.lookup_next

    assert target is not None
    assert target.chunk == first_chunk

    # Step 5: If the memsection still extends over several chunks, we may need
    # to merge these chunks together into one megachunk. We then just fall
    # through to the usual allocation routine.
    if ms_end > target_aend:
        # In this case, the allocation looks something like:
        # |  chunk                                           |   chunk   |
        # | ms | free                                        | free | ms |
        #      |<-------------------- allocation ------------------>|
        assert target.chunk != last_chunk
        assert target.lookup_next is None

        last_available_chunk_index = last_chunk_index
        if last_chunk.memsection != CHUNK_AVAILABLE:
            first_last_ms = last_chunk.memsection
            assert first_last_ms != target
            # Last chunk is partly allocated, so we need to be careful about
            # putting this one in our alloc pool. We merge this chunk with the
            # first chunk.

            # Taint the memsections if need be.
            if target.realm != first_last_ms.realm or \
                    first_last_ms.is_tainted or target.is_tainted:
                if first_last_ms.is_tainted == 0:
                    _taint_memsection(first_last_ms)

                if target.is_tainted == 0:
                    _taint_memsection(target)

            # For each memsection in the chunk we are adopting, set their
            # chunk pointer to their new leader.
            cur = first_last_ms
            while cur is not None:
                cur.chunk = first_chunk
                cur = cur.lookup_next

            # Now just merge this chunk's memsections so it extends
            # the first chunk's memsections.
            target.avail_after += first_last_ms.avail_before

            on_alloc = (first_last_ms.avail_before > 0) or \
                        (first_last_ms.avail_after > 0)
            first_last_ms.avail_before = 0
            if on_alloc and first_last_ms.avail_after == 0:
                remove_from_alloc(first_last_ms)

            assert target.lookup_next is None
            target.lookup_next = first_last_ms
            last_chunk.memsection = MEGACHUNK(first_chunk)

            last_available_chunk_index -= 1

        # Make all the available chunks additions to the first chunk.
        for i in xrange(first_chunk_index + 1, last_available_chunk_index + 1):
            chunk = address_space.chunks[i]
            if CHUNK_IS_MEGA(chunk):
                # Already accounted for.
                pass
            else:
                assert chunk.memsection == CHUNK_AVAILABLE
                target.avail_after += CHUNK_SIZE
            chunk.memsection = MEGACHUNK(first_chunk)

        # Update this value again now that it has changed.
        target_aend = target_end + target.avail_after

    assert ms_end <= target_aend

    # Check that the memsection doesn't overlap with the target.
    assert not overlap(target_base, target_end, ms_base, ms_end)

    # Check that the allocation will fit inside.
    assert not contains(ms_base, ms_end, target_base)
    assert not contains(ms_base, ms_end, target_end-1)

    # We have it, and it doesn't overlap, now find out if it is before or after
    # then fix up the available before and available after
    if ms_base < target_base:
        # We are pulling it out of available before
        memsection.avail_before = ms_base - target_abase
        target.avail_before = target_base - ms_end
    else:
        # We are pulling it out of available after
        memsection.avail_after = target_aend - ms_end
        memsection.avail_before = ms_base - target_end
        target.avail_after = 0

    # Maybe remove the target from the allocation list.
    if target.avail_before == 0 and target.avail_after == 0:
        remove_from_alloc(target)

    # Do we need to taint the allocation?
    if target.is_tainted == 0 and target.realm != memsection.realm:
        # Lets taint the thing!
        _taint_memsection(target)

    memsection.is_tainted = target.is_tainted

    # Maybe add the memsection to the allocation list.
    if memsection.avail_before > 0 or memsection.avail_after > 0:
        # Add to list
        if memsection.is_tainted:
            memsection.alloc_next = address_space.taint_list
            address_space.taint_list = memsection
        else:
            memsection.alloc_next = realm.alloc
            realm.alloc = memsection

    memsection.chunk = target.chunk
    _okl4_chunk_memsection_insert(memsection.chunk, memsection)

def okl4_realm_attach_pd(realm, pd_id):
    if hasattr (realm, "attached_pds_R"):
        idx = pd_id / 32
        offset = pd_id % 32

    #always attach with full perms for now
        realm.attached_pds_R[idx] |= (1 << offset)
        realm.attached_pds_W[idx] |= (1 << offset)
        realm.attached_pds_X[idx] |= (1 << offset)

    # Maybe some day we'll make the PDs perform the mapping here
    # too. For now, we just assume that Elfweaver has performed
    # this elsewhere.

def okl4_memsection_create(memsection, realm, vaddr, paddr, size):
    if realm is None:
        raise RealmsException("No realm specified for memsection creation.")
    memsection.base = vaddr
    memsection.realm = realm
    memsection.size = size
    memsection.super = memsection
    if vaddr == INVALID_ADDR:
        raise RealmsException("Realms allocation must have a known address.")
    else:
        okl4_realm_memsection_allocate_fixed(realm, memsection)

    memsection.own_next = realm.memsections
    realm.memsections = memsection

def okl4_memsection_contiguous_create(memsection, realm, vaddr, paddr, size,
        one_to_one):
    # Allocate physical memory
    if paddr == INVALID_ADDR:
        # Would normally allocate from a physpool.
        # We expect elfweaver to always specify this.
        raise RealmsException("Mappings must have their physical address already specified.")

    if one_to_one:
        if vaddr != INVALID_ADDR:
            # Can't ask for 1:1 and specify
            raise RealmsException("Cannot specify 1:1 mapping and virt address.")
        vaddr = paddr
    okl4_memsection_create(memsection.parent, realm, vaddr, paddr, size)
    memsection.super = memsection.parent

    memsection.mapping.phys = Autogen.types.OKL4_physmem_item(0)
    memsection.mapping.phys.paddr = paddr
    memsection.mapping.phys.range.size = memsection.super.size
    memsection.mapping.virt = memsection.super.base
    memsection.mapping.size = memsection.super.size
    memsection.super.delete_handler = '_memsection_contiguous_delete'
    memsection.super.fault_handler = '_memsection_contiguous_fault'
    memsection.super.iterate_handler = '_memsection_contiguous_iterator'
    memsection.super.lookup_handler = '_memsection_contiguous_lookup'

    # okl4_memsection_map(&memsection->super, &memsection->mapping)

def okl4_memsection_paged_create(memsection, realm, vaddr, paddr, size,
        page_size):
    # Only allow unmapped paged memsections.
    if paddr != INVALID_ADDR:
        raise RealmsException("Elfweaver can only create unmapped paged memsections")

    okl4_memsection_create(memsection.parent, realm, vaddr, paddr, size)
    memsection.super = memsection.parent

    memsection.num_pages = size / page_size
    memsection.page_size = page_size
    memsection.physpool = None

    memsection.super.delete_handler = '_memsection_paged_delete'
    memsection.super.fault_handler = '_memsection_paged_fault'
    memsection.super.iterate_handler = '_memsection_paged_iterator'
    memsection.super.lookup_handler = '_memsection_paged_lookup'

def init():
    global CHUNK_AVAILABLE
    CHUNK_AVAILABLE = Autogen.types.OKL4_memsection(address = 0xFFFFFFFE)
