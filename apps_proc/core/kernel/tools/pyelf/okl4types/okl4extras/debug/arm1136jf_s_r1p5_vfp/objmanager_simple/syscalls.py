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

# Auto generated file - do not modify

"""
Based on kernel/objmanager-simple/src/lifecycle.c.tmpl
"""

# Long lines are hard to control in autogenerated files.
#pylint: disable-msg=C0301
# Unused arguments are hard to control in autogenerated files.
#pylint: disable-msg=W0613
# Use of protected fields are part of the type system design.
#pylint: disable-msg=W0212

from pyokl4.okl4.objmanager_simple.cap_allocator import allocate_cap, \
        init_master_cap
from pyokl4.autogen import Autogen

import okl4types.okl4extras.debug.arm1136jf_s_r1p5_vfp.types as types
import okl4types.okl4extras.debug.arm1136jf_s_r1p5_vfp.init as types_init
import pyokl4.events as events

def init():
    """
    Initialise this module
    """
    # Create events for all the objects we have.
    events.create_event("barrier_create")
    events.create_event("domain_create")
    events.create_event("hybrid_mutex_create")
    events.create_event("interrupt_create")
    events.create_event("intlock_control_create")
    events.create_event("message_queue_create")
    events.create_event("mmu_context_create")
    events.create_event("mutex_create")
    events.create_event("platform_control_create")
    events.create_event("processor_control_create")
    events.create_event("profile_control_create")
    events.create_event("segment_create")
    events.create_event("semaphore_create")
    events.create_event("switch_table_create")
    events.create_event("thread_create")
    events.create_event("tracebuffer_control_create")

# Create a new barrier object.
def sys_barrier_create(domain, count,  kdbname=None, **kwargs):
    """
    Model the create barrier system call in the given domain.
    """

    obj = Autogen.objmanager.objmanager_allocate(
            types.OKL4_barrier, domain, 1,
            Autogen.types.OKL4_barrier._type_alignment)[0]

    cap_id, cap = allocate_cap(domain)
    obj.header.master_designator = cap_id
    obj.header.master_domain     = domain

    #
    # Setup the cap / cap_id / domain information on the object.
    #
    # These don't actually reflect kernel data structures, but are provided to
    # make ElfWeaver's life easier.
    #
    obj._cap_id = cap_id
    obj._cap = cap
    obj._domain = domain

    types_init.init_barrier_object(obj)

    # Let other subsystems know about the object.
    events.trigger("barrier_create", kdbname=kdbname,
                   obj = obj, count = count)

    init_master_cap(cap, domain,
            Autogen.objmanager_autogen.OBJTYPE_BARRIER, obj.header)

    return obj

# Create a new domain object.
def sys_domain_create(domain, clist_entries, max_prio, reply_token_table_entries,  kdbname=None, **kwargs):
    """
    Model the create domain system call in the given domain.
    """

    obj = Autogen.objmanager.objmanager_allocate(
            types.OKL4_domain, domain, 1,
            Autogen.types.OKL4_domain._type_alignment)[0]

    cap_id, cap = allocate_cap(domain)
    obj.header.master_designator = cap_id
    obj.header.master_domain     = domain

    #
    # Setup the cap / cap_id / domain information on the object.
    #
    # These don't actually reflect kernel data structures, but are provided to
    # make ElfWeaver's life easier.
    #
    obj._cap_id = cap_id
    obj._cap = cap
    obj._domain = domain

    types_init.init_domain_object(obj)

    # Let other subsystems know about the object.
    events.trigger("domain_create", kdbname=kdbname,
                   obj = obj, clist_entries = clist_entries, max_prio = max_prio, reply_token_table_entries = reply_token_table_entries)

    init_master_cap(cap, domain,
            Autogen.objmanager_autogen.OBJTYPE_DOMAIN, obj.header)

    return obj

# Create a new hybrid_mutex object.
def sys_hybrid_mutex_create(domain,  kdbname=None, **kwargs):
    """
    Model the create hybrid_mutex system call in the given domain.
    """

    obj = Autogen.objmanager.objmanager_allocate(
            types.OKL4_hybrid_mutex, domain, 1,
            Autogen.types.OKL4_hybrid_mutex._type_alignment)[0]

    cap_id, cap = allocate_cap(domain)
    obj.header.master_designator = cap_id
    obj.header.master_domain     = domain

    #
    # Setup the cap / cap_id / domain information on the object.
    #
    # These don't actually reflect kernel data structures, but are provided to
    # make ElfWeaver's life easier.
    #
    obj._cap_id = cap_id
    obj._cap = cap
    obj._domain = domain

    types_init.init_hybrid_mutex_object(obj)

    # Let other subsystems know about the object.
    events.trigger("hybrid_mutex_create", kdbname=kdbname,
                   obj = obj)

    init_master_cap(cap, domain,
            Autogen.objmanager_autogen.OBJTYPE_HYBRID_MUTEX, obj.header)

    return obj

# Create a new interrupt object.
def sys_interrupt_create(domain, irq,  kdbname=None, **kwargs):
    """
    Model the create interrupt system call in the given domain.
    """

    obj = Autogen.objmanager.objmanager_allocate(
            types.OKL4_interrupt, domain, 1,
            Autogen.types.OKL4_interrupt._type_alignment)[0]

    cap_id, cap = allocate_cap(domain)
    obj.header.master_designator = cap_id
    obj.header.master_domain     = domain

    #
    # Setup the cap / cap_id / domain information on the object.
    #
    # These don't actually reflect kernel data structures, but are provided to
    # make ElfWeaver's life easier.
    #
    obj._cap_id = cap_id
    obj._cap = cap
    obj._domain = domain

    types_init.init_interrupt_object(obj)

    # Let other subsystems know about the object.
    events.trigger("interrupt_create", kdbname=kdbname,
                   obj = obj, irq = irq)

    init_master_cap(cap, domain,
            Autogen.objmanager_autogen.OBJTYPE_INTERRUPT, obj.header)

    return obj

# Create a new intlock_control object.
def sys_intlock_control_create(domain,  kdbname=None, **kwargs):
    """
    Model the create intlock_control system call in the given domain.
    """

    obj = Autogen.objmanager.objmanager_allocate(
            types.OKL4_intlock_control, domain, 1,
            Autogen.types.OKL4_intlock_control._type_alignment)[0]

    cap_id, cap = allocate_cap(domain)
    obj.header.master_designator = cap_id
    obj.header.master_domain     = domain

    #
    # Setup the cap / cap_id / domain information on the object.
    #
    # These don't actually reflect kernel data structures, but are provided to
    # make ElfWeaver's life easier.
    #
    obj._cap_id = cap_id
    obj._cap = cap
    obj._domain = domain

    types_init.init_intlock_control_object(obj)

    # Let other subsystems know about the object.
    events.trigger("intlock_control_create", kdbname=kdbname,
                   obj = obj)

    init_master_cap(cap, domain,
            Autogen.objmanager_autogen.OBJTYPE_INTLOCK_CONTROL, obj.header)

    return obj

# Create a new message_queue object.
def sys_message_queue_create(domain, max_messages, mmu_id, quota,  kdbname=None, **kwargs):
    """
    Model the create message_queue system call in the given domain.
    """

    obj = Autogen.objmanager.objmanager_allocate(
            types.OKL4_message_queue, domain, 1,
            Autogen.types.OKL4_message_queue._type_alignment)[0]

    cap_id, cap = allocate_cap(domain)
    obj.header.master_designator = cap_id
    obj.header.master_domain     = domain

    #
    # Setup the cap / cap_id / domain information on the object.
    #
    # These don't actually reflect kernel data structures, but are provided to
    # make ElfWeaver's life easier.
    #
    obj._cap_id = cap_id
    obj._cap = cap
    obj._domain = domain

    types_init.init_message_queue_object(obj)

    # Let other subsystems know about the object.
    events.trigger("message_queue_create", kdbname=kdbname,
                   obj = obj, max_messages = max_messages, mmu_id = mmu_id, quota = quota)

    init_master_cap(cap, domain,
            Autogen.objmanager_autogen.OBJTYPE_MESSAGE_QUEUE, obj.header)

    return obj

# Create a new mmu_context object.
def sys_mmu_context_create(domain, num_segments, intlock_cap,  kdbname=None, **kwargs):
    """
    Model the create mmu_context system call in the given domain.
    """

    obj = Autogen.objmanager.objmanager_allocate(
            types.OKL4_mmu_context, domain, 1,
            Autogen.types.OKL4_mmu_context._type_alignment)[0]

    cap_id, cap = allocate_cap(domain)
    obj.header.master_designator = cap_id
    obj.header.master_domain     = domain

    #
    # Setup the cap / cap_id / domain information on the object.
    #
    # These don't actually reflect kernel data structures, but are provided to
    # make ElfWeaver's life easier.
    #
    obj._cap_id = cap_id
    obj._cap = cap
    obj._domain = domain

    types_init.init_mmu_context_object(obj)

    # Let other subsystems know about the object.
    events.trigger("mmu_context_create", kdbname=kdbname,
                   obj = obj, num_segments = num_segments, intlock_cap = intlock_cap)

    init_master_cap(cap, domain,
            Autogen.objmanager_autogen.OBJTYPE_MMU_CONTEXT, obj.header)

    return obj

# Create a new mutex object.
def sys_mutex_create(domain, is_counted, inheritance_type,  kdbname=None, **kwargs):
    """
    Model the create mutex system call in the given domain.
    """

    obj = Autogen.objmanager.objmanager_allocate(
            types.OKL4_mutex, domain, 1,
            Autogen.types.OKL4_mutex._type_alignment)[0]

    cap_id, cap = allocate_cap(domain)
    obj.header.master_designator = cap_id
    obj.header.master_domain     = domain

    #
    # Setup the cap / cap_id / domain information on the object.
    #
    # These don't actually reflect kernel data structures, but are provided to
    # make ElfWeaver's life easier.
    #
    obj._cap_id = cap_id
    obj._cap = cap
    obj._domain = domain

    types_init.init_mutex_object(obj)

    # Let other subsystems know about the object.
    events.trigger("mutex_create", kdbname=kdbname,
                   obj = obj, is_counted = is_counted, inheritance_type = inheritance_type)

    init_master_cap(cap, domain,
            Autogen.objmanager_autogen.OBJTYPE_MUTEX, obj.header)

    return obj

# Create a new platform_control object.
def sys_platform_control_create(domain,  kdbname=None, **kwargs):
    """
    Model the create platform_control system call in the given domain.
    """

    obj = Autogen.objmanager.objmanager_allocate(
            types.OKL4_platform_control, domain, 1,
            Autogen.types.OKL4_platform_control._type_alignment)[0]

    cap_id, cap = allocate_cap(domain)
    obj.header.master_designator = cap_id
    obj.header.master_domain     = domain

    #
    # Setup the cap / cap_id / domain information on the object.
    #
    # These don't actually reflect kernel data structures, but are provided to
    # make ElfWeaver's life easier.
    #
    obj._cap_id = cap_id
    obj._cap = cap
    obj._domain = domain

    types_init.init_platform_control_object(obj)

    # Let other subsystems know about the object.
    events.trigger("platform_control_create", kdbname=kdbname,
                   obj = obj)

    init_master_cap(cap, domain,
            Autogen.objmanager_autogen.OBJTYPE_PLATFORM_CONTROL, obj.header)

    return obj

# Create a new processor_control object.
def sys_processor_control_create(domain,  kdbname=None, **kwargs):
    """
    Model the create processor_control system call in the given domain.
    """

    obj = Autogen.objmanager.objmanager_allocate(
            types.OKL4_processor_control, domain, 1,
            Autogen.types.OKL4_processor_control._type_alignment)[0]

    cap_id, cap = allocate_cap(domain)
    obj.header.master_designator = cap_id
    obj.header.master_domain     = domain

    #
    # Setup the cap / cap_id / domain information on the object.
    #
    # These don't actually reflect kernel data structures, but are provided to
    # make ElfWeaver's life easier.
    #
    obj._cap_id = cap_id
    obj._cap = cap
    obj._domain = domain

    types_init.init_processor_control_object(obj)

    # Let other subsystems know about the object.
    events.trigger("processor_control_create", kdbname=kdbname,
                   obj = obj)

    init_master_cap(cap, domain,
            Autogen.objmanager_autogen.OBJTYPE_PROCESSOR_CONTROL, obj.header)

    return obj

# Create a new profile_control object.
def sys_profile_control_create(domain,  kdbname=None, **kwargs):
    """
    Model the create profile_control system call in the given domain.
    """

    obj = Autogen.objmanager.objmanager_allocate(
            types.OKL4_profile_control, domain, 1,
            Autogen.types.OKL4_profile_control._type_alignment)[0]

    cap_id, cap = allocate_cap(domain)
    obj.header.master_designator = cap_id
    obj.header.master_domain     = domain

    #
    # Setup the cap / cap_id / domain information on the object.
    #
    # These don't actually reflect kernel data structures, but are provided to
    # make ElfWeaver's life easier.
    #
    obj._cap_id = cap_id
    obj._cap = cap
    obj._domain = domain

    types_init.init_profile_control_object(obj)

    # Let other subsystems know about the object.
    events.trigger("profile_control_create", kdbname=kdbname,
                   obj = obj)

    init_master_cap(cap, domain,
            Autogen.objmanager_autogen.OBJTYPE_PROFILE_CONTROL, obj.header)

    return obj

# Create a new segment object.
def sys_segment_create(domain, base_segment, offset,  kdbname=None, **kwargs):
    """
    Model the create segment system call in the given domain.
    """

    obj = Autogen.objmanager.objmanager_allocate(
            types.OKL4_segment, domain, 1,
            Autogen.types.OKL4_segment._type_alignment)[0]

    cap_id, cap = allocate_cap(domain)
    obj.header.master_designator = cap_id
    obj.header.master_domain     = domain

    #
    # Setup the cap / cap_id / domain information on the object.
    #
    # These don't actually reflect kernel data structures, but are provided to
    # make ElfWeaver's life easier.
    #
    obj._cap_id = cap_id
    obj._cap = cap
    obj._domain = domain

    types_init.init_segment_object(obj)

    # Let other subsystems know about the object.
    events.trigger("segment_create", kdbname=kdbname,
                   obj = obj, base_segment = base_segment, offset = offset)

    init_master_cap(cap, domain,
            Autogen.objmanager_autogen.OBJTYPE_SEGMENT, obj.header)

    return obj

# Create a new semaphore object.
def sys_semaphore_create(domain, value,  kdbname=None, **kwargs):
    """
    Model the create semaphore system call in the given domain.
    """

    obj = Autogen.objmanager.objmanager_allocate(
            types.OKL4_semaphore, domain, 1,
            Autogen.types.OKL4_semaphore._type_alignment)[0]

    cap_id, cap = allocate_cap(domain)
    obj.header.master_designator = cap_id
    obj.header.master_domain     = domain

    #
    # Setup the cap / cap_id / domain information on the object.
    #
    # These don't actually reflect kernel data structures, but are provided to
    # make ElfWeaver's life easier.
    #
    obj._cap_id = cap_id
    obj._cap = cap
    obj._domain = domain

    types_init.init_semaphore_object(obj)

    # Let other subsystems know about the object.
    events.trigger("semaphore_create", kdbname=kdbname,
                   obj = obj, value = value)

    init_master_cap(cap, domain,
            Autogen.objmanager_autogen.OBJTYPE_SEMAPHORE, obj.header)

    return obj

# Create a new switch_table object.
def sys_switch_table_create(domain, domain_id, mmu_context_id, num_entries, return_stub_ip, intlock,  kdbname=None, **kwargs):
    """
    Model the create switch_table system call in the given domain.
    """

    obj = Autogen.objmanager.objmanager_allocate(
            types.OKL4_switch_table, domain, 1,
            Autogen.types.OKL4_switch_table._type_alignment)[0]

    cap_id, cap = allocate_cap(domain)
    obj.header.master_designator = cap_id
    obj.header.master_domain     = domain

    #
    # Setup the cap / cap_id / domain information on the object.
    #
    # These don't actually reflect kernel data structures, but are provided to
    # make ElfWeaver's life easier.
    #
    obj._cap_id = cap_id
    obj._cap = cap
    obj._domain = domain

    types_init.init_switch_table_object(obj)

    # Let other subsystems know about the object.
    events.trigger("switch_table_create", kdbname=kdbname,
                   obj = obj, domain_id = domain_id, mmu_context_id = mmu_context_id, num_entries = num_entries, return_stub_ip = return_stub_ip, intlock = intlock)

    init_master_cap(cap, domain,
            Autogen.objmanager_autogen.OBJTYPE_SWITCH_TABLE, obj.header)

    return obj

# Create a new thread object.
def sys_thread_create(domain, domain_id, mmu_context_id, ip, sp, mode,  kdbname=None, **kwargs):
    """
    Model the create thread system call in the given domain.
    """

    obj = Autogen.objmanager.objmanager_allocate(
            types.OKL4_thread, domain, 1,
            Autogen.types.OKL4_thread._type_alignment)[0]

    cap_id, cap = allocate_cap(domain)
    obj.header.master_designator = cap_id
    obj.header.master_domain     = domain

    #
    # Setup the cap / cap_id / domain information on the object.
    #
    # These don't actually reflect kernel data structures, but are provided to
    # make ElfWeaver's life easier.
    #
    obj._cap_id = cap_id
    obj._cap = cap
    obj._domain = domain

    types_init.init_thread_object(obj)

    # Let other subsystems know about the object.
    events.trigger("thread_create", kdbname=kdbname,
                   obj = obj, domain_id = domain_id, mmu_context_id = mmu_context_id, ip = ip, sp = sp, mode = mode)

    init_master_cap(cap, domain,
            Autogen.objmanager_autogen.OBJTYPE_THREAD, obj.header)

    return obj

# Create a new tracebuffer_control object.
def sys_tracebuffer_control_create(domain,  kdbname=None, **kwargs):
    """
    Model the create tracebuffer_control system call in the given domain.
    """

    obj = Autogen.objmanager.objmanager_allocate(
            types.OKL4_tracebuffer_control, domain, 1,
            Autogen.types.OKL4_tracebuffer_control._type_alignment)[0]

    cap_id, cap = allocate_cap(domain)
    obj.header.master_designator = cap_id
    obj.header.master_domain     = domain

    #
    # Setup the cap / cap_id / domain information on the object.
    #
    # These don't actually reflect kernel data structures, but are provided to
    # make ElfWeaver's life easier.
    #
    obj._cap_id = cap_id
    obj._cap = cap
    obj._domain = domain

    types_init.init_tracebuffer_control_object(obj)

    # Let other subsystems know about the object.
    events.trigger("tracebuffer_control_create", kdbname=kdbname,
                   obj = obj)

    init_master_cap(cap, domain,
            Autogen.objmanager_autogen.OBJTYPE_TRACEBUFFER_CONTROL, obj.header)

    return obj


# Python code representing the sys_initial_domain_create system call

def sys_heap_create(virt_address, phys_address, heap_size, _):
    """Create a heap."""
    return Autogen.objmanager.Objmanager(virt_address, phys_address, heap_size)

def sys_initial_domain_create(heap, clist_entries, max_prio,
        reply_token_table_entries):
    """Create the initial domain object"""

    # Need to have a cap to this domain inside the domain.
    assert clist_entries >= 1

    # Create the domain
    domain = heap.allocate_object(types.OKL4_domain, 1, 4)[0]

    # Initialise the domain
    types_init.init_domain_object(domain)

    #
    # During creation, other modules may need to perform allocations (such as
    # the clist) for this domain. Do determine where memory should be
    # allocated, they use this object's domain's heap. We facilitate this by
    # setting the domain's domain to itself.
    #
    domain._domain = domain
    domain.objmanager.heap.next = heap
    domain.objmanager.heap.size = 0
    domain._cap_id = None
    domain._cap = None
    domain.header.master_domain = domain
    domain._source_heap = heap

    # Let other subsystems know about the object.
    events.trigger("domain_create", obj = domain,
            clist_entries = clist_entries, max_prio = max_prio,
            reply_token_table_entries = reply_token_table_entries)

    # Now bootstrap a cap to this domain from the domain itself
    cap_id, cap = allocate_cap(domain)
    init_master_cap(cap, domain, Autogen.objmanager_autogen.OBJTYPE_DOMAIN,
            domain.header)
    domain.header.master_designator = cap_id

    # Return the domain object.
    return domain
