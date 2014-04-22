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
Process the kernel XML plus provide proxy objects for the contents
of the kernel heap.
"""

import os.path
import glob
import subprocess
import tempfile

import weaver.autogen_helper

from pyokl4.autogen import Autogen


from elf.segnames import add_segment_names, get_segment_names
from weaver import MergeError

from elf.core import UnpreparedElfFile
from elf.constants import ET_EXEC, PF_R, PF_W, PF_X
from elf.util import align_up, align_down
from weaver.segments_xml import collect_elf_segments, collect_patches, Patch_el
from weaver.kernel_xml import Kernel_el
from weaver.parse_spec import XmlCollector
from weaver.link import link
from weaver.util import get_symbol, check_file_exists
from weaver.elfweaver_info import find_elfweaver_info, copy_elfweaver_info
import weaver.kernel_okl4
from weaver.pools import Pools

# The default number of caps in a clist.
DEFAULT_MAX_CAPS = 256

KERNEL_TAGS = set(["file", "configuration", "platform", "linker_script",
                   "linker_wrapper", "kernel", "soc", "libs"])
KERNEL_PRELINK_TAGS = set(["file"])
KERNEL_SDKLINK_TAGS = set(["configuration", "platform"])
KERNEL_LINK_TAGS = set(["linker_script", "linker_wrapper", "kernel", "soc",
                        "libs"])
KERNEL_NEW_SDKLINK_TAGS = set(["debug"])



#--------------------------------------------------------------------------
# Representation of the kernel.

# Here is the current strucure of XML -> Internal Representation
#
# XML
# Cells -> PDs -> Threads
#                 -> IRQs
#                 -> Mutexes
#                 -> Segments/Mappings
#
# Converts to
# Heap -> Domain -> MMU Contexts -> Threads
#                                -> Mappings
#                -> IRQs
#                -> Mutexes
#                -> Segments
#                -> Tracebuffer
#
# The internal classes of Kernel are proxies for the objects that will
# appear in the kernel heap.  They are used in places that need to
# know about kernel objects before the heap is initialised.
#
# The kernel_okl4 module manipulates the final objects.


class KernelObject(object):
    """Base class for first class objects in the kernel."""

    def __init__(self):
        self.okl4_object = None

class Kernel(XmlCollector):
    """
    Kernel initialisation parameters built from the XML file.
    These parameters are then merged into the kconfig section in the
    ELF file.
    """

    one_MB = 0x100000
    DEFAULT_MAX_CAPS = DEFAULT_MAX_CAPS
    element = Kernel_el

    # What product, arch and cpu we are building this kernel for
    product = None
    arch = None
    cpu = None

    class Heap(KernelObject):
        """Kernel information pertaining to a cell."""

        class Domain(KernelObject):
            """A domain within the system."""

            class MMU_Context(KernelObject):
                """An mmu context within a domain."""

                class Thread(KernelObject):
                    """A thread within an MMU context."""

                    def __init__(self, domain, name, start, entry, init_func,
                                 stack_attrs, priority, timeslice, affinity,
                                 is_main_thread, friendly_name):
                        KernelObject.__init__(self)

                        self.domain = domain
                        self.name = name
                        self.cap_id = -1
                        self.start = start
                        self.entry = entry
                        self.init_func = init_func
                        self.stack_attrs = stack_attrs
                        self.priority = priority
                        self.timeslice = timeslice
                        self.affinity = affinity
                        self.is_main_thread = is_main_thread
                        self.held_mutexes = []
                        self.friendly_name = friendly_name

                    def get_sp(self):
                        """
                        Return the stack pointer for the thread.
                        """
                        # If the thread doesn't need a stack (the
                        # thread makes its own), then return a dummy
                        # value.
                        if self.stack_attrs is None:
                            return 0

                        # If the stack hasn't been allocated yet, then
                        # return a dummy value.
                        virt_addr = self.stack_attrs.virt_addr
                        if virt_addr is None:
                            virt_addr = 0

                        size = self.stack_attrs.size
                        if size is None:
                            size = 0

                        return virt_addr + size



                # for class MMU_Context.
                def __init__(self, domain, ns_node, env_prefix, max_priority,
                             l2_pgtable_buffer_nb, num_vuas, segment_table, intlock_cap):
                    KernelObject.__init__(self)

                    self.domain = domain
                    self.ns_node = ns_node
                    self.env_prefix = env_prefix
                    self.max_priority = max_priority
                    self.threads = []
                    self.mappings = []
                    self.mutexes = []
                    self.l2_pgtable_buffer_nb = l2_pgtable_buffer_nb
                    self.num_vuas = num_vuas
                    self.segment_table = segment_table
                    self.seg_attachments = []
                    self.copy_area = None
                    self.intlock_allowed = intlock_cap

                def register_thread(self, name, start, entry, init_func,
                                    stack_ms, priority, timeslice, affinity,
                                    is_main_thread, friendly_name):
                    if self.max_priority is not None and \
                            priority > self.max_priority:
                        raise MergeError(
                                "Tried to create thread with priority %s," \
                                " but space (%s) max priority is %s." % \
                                (priority, self.ns_node.abs_name(),
                                 self.max_priority))

                    thread = self.Thread(self, name, start, entry, init_func,
                                         stack_ms, priority, timeslice,
                                         affinity, is_main_thread,
                                         friendly_name)
                    self.threads.append(thread)

                    return thread

                def register_attachment(self, segment):
                    """
                    Register a segment to be attached to the MMU.

                    The attachment index is allocated by the segment table.
                    """
                    self.segment_table.register(segment)

                    self.seg_attachments.append(segment)

                def register_mapping(self, segment, attr):
                    """
                    Register a mapping of the segment into the mmu with the
                    given attributes.

                    The segment is attached to the mmu in the process.
                    """
                    mapping = (segment, attr)
                    self.mappings.append(mapping)

                    self.register_attachment(segment)

                    return mapping

                def clone_mappings(self, mmu_context):
                    """
                    Copy all mappings from the provided mmu into this mmu.
                    """
                    for mapping in mmu_context.mappings:
                        if mapping not in self.mappings:
                            self.mappings.append(mapping)

                def get_copy_area(self, kernel, sas, machine, namespace):
                    """
                    Return the name of the virtual memory pool for user-kernel
                    shared mappings in this MMU context. If the pool does not
                    exist, attempt to allocate one.

                    This function will return False if shared mappings can use
                    normal user addresses on the target architecture.
                    """
                    if self.copy_area is None:
                        self.copy_area = kernel.kernel.alloc_copy_area( \
                                sas, machine, namespace)
                    if self.copy_area is not False:
                        return self.copy_area.get_name()
                    return False

            class IRQ(KernelObject):
                """The ability to assign an IRQ within a domain."""

                def __init__(self, irq):
                    KernelObject.__init__(self)

                    self.irq = irq

                @property
                def is_virq(self):
                    return False

                @property
                def source(self):
                    return None

            class VIRQ(KernelObject):
                """The ability to assign a VIRQ within a domain."""

                def __init__(self, parent_namespace, source, domain):
                    KernelObject.__init__(self)

                    self.namespace = parent_namespace
                    self.source = source
                    self.domain = domain
                    self.unsourced = True
                    # IRQs are allocated at a later stage.
                    self.irq = None

                @property
                def is_virq(self):
                    return True


            class VirqLine(KernelObject):
                """The ability to assign a VIRQ line within a domain """

                def __init__(self, name):
                    KernelObject.__init__(self)
                    self.name = name
                    self.cap_object = None

            class PlatformControl(KernelObject):
                """The ability to add a Platform Control to a domain."""

                def __init__(self, name, domain):
                    KernelObject.__init__(self)
                    self.name = name
                    self.domain = domain

            class ProcessorControl(KernelObject):
                """The ability to add processor control to a domain."""

                def __init__(self, name, domain):
                    KernelObject.__init__(self)
                    self.name = name
                    self.domain = domain

            class ProfileControl(KernelObject):
                """The ability to add profile control to a domain."""

                def __init__(self, name, domain):
                    KernelObject.__init__(self)
                    self.name = name
                    self.domain = domain

            class IntlockControl(KernelObject):
                """The ability to add intlock control to a domain."""

                def __init__(self, name, domain):
                    KernelObject.__init__(self)
                    self.name = name
                    self.domain = domain


            class TraceBuffer(KernelObject):
                """A tracebuffer within a domain"""

                def __init__(self, domain, attrs, buffers):
                    KernelObject.__init__(self)
                    self.domain = domain
                    self.attrs = attrs
                    self.buffers = buffers
                    self.kernel_virt_addr = None

            class Mutex(KernelObject):
                """A mutex within a domain."""

                def __init__(self, domain, name, is_counted, holder, is_hybrid):
                    KernelObject.__init__(self)

                    self.domain = domain
                    self.name = name
                    self.is_counted = is_counted
                    self.holder = holder
                    self.is_hybrid = is_hybrid

            class Segment(KernelObject):
                """A segment with a domain."""

                def __init__(self, domain, attrs, use_as_heap=True):
                    KernelObject.__init__(self)

                    self.domain = domain
                    self.attrs = attrs
                    self.use_as_heap = use_as_heap

            class SharedSegment(Segment):
                """
                A Segment-like object representing a reference to a segment in
                another domain but this domain has a cap to (via cap_copy()).
                To the caller it has all of the properties of the original
                segment except for the cap, which is particular to this
                domain.
                """

                class OKL4_segment_delegate(object):
                    """
                    Read-only OKL4_segment delegate class.  This passes all
                    attribute references off to the original OKL4_Segment
                    object except for '_cap_id', which is replaced by the
                    value in the local cap.  For code that accesses
                    OKL4_segment objects, this will make a segment coming from
                    a different domain look like it it present in this domain.
                    """
                    def __init__(self, okl4_segment, local_cap):
                        self._okl4_segment = okl4_segment
                        self._local_cap = local_cap

                    def __getattr__(self, name):
                        """
                        Return the local cap for _cap_id, and pass everything
                        else off to the original object.
                        """
                        if name == '_cap_id':
                            return self._local_cap.get_cap_value()
                        else:
                            return getattr(self._okl4_segment, name)

                    def __setattr__(self, name, value):
                        """
                        Only allow write access to internal fields.
                        Everything else is read-only.
                        """
                        if name in ('_okl4_segment', '_local_cap'):
                            self.__dict__[name] = value
                        else:
                            assert False, \
                                   "OKL4_segment_delegate: Field '%s' is " \
                                   "read-only." % name

                # for class SharedSegment
                def __init__(self, domain, src_segment, local_cap):
                    domain.Segment.__init__(self, domain,
                                     src_segment.attrs,
                                     src_segment.use_as_heap)

                    self.src_segment = src_segment
                    self.local_cap = local_cap
                    self._okl4_object_delegate = None

                def _get_okl4_object(self):
                    """
                    Access wrapper to okl4_object.  If this field in the
                    source segment is None, then return None, else returned a
                    cached delegate for the source object
                    """
                    if self.src_segment.okl4_object is None:
                        return None
                    elif self._okl4_object_delegate is not None:
                        return self._okl4_object_delegate
                    else:
                        self._okl4_object_delegate = \
                        self.OKL4_segment_delegate(self.src_segment.okl4_object,
                                              self.local_cap)
                        return self._okl4_object_delegate

                def _set_okl4_object(self, value):
                    """Allow okl4_object to be initalised, but nothing else."""
                    assert value is None

                okl4_object = property(_get_okl4_object, _set_okl4_object)

            class Cap(KernelObject):
                """A cap copied into the domain."""

                def __init__(self, source_object, rights_mask):
                    KernelObject.__init__(self)

                    self.source_object = source_object
                    self.rights_mask = rights_mask
                    self.okl4_capid = None

                def get_cap_value(self):
                    return self.okl4_capid

            class SwitchTable(KernelObject):
                """A switch table associated with a domain and mmu."""

                def __init__(self, domain, mmu, entry_symbols,
                        entry_mode, return_stub):
                    KernelObject.__init__(self)

                    self.domain = domain
                    self.mmu = mmu
                    self.cap_id = -1
                    self.entry_symbols = entry_symbols
                    self.entry_mode = entry_mode
                    self.return_stub = return_stub

            class MessageQueue(KernelObject):
                """A message queue within a domain."""

                def __init__(self, domain, name, max_messages, mmu, quota):
                    KernelObject.__init__(self)

                    self.domain = domain
                    self.name = name
                    self.max_messages = max_messages
                    self.cap_id = -1
                    self.mmu = mmu
                    self.quota = quota

            class Channel(KernelObject):
                """A channel within a domain."""

                def __init__(self, domain, name, max_messages, message_size,
                        ms_attrs, ms, buffer_mapped):
                    KernelObject.__init__(self)

                    self.domain = domain
                    self.name = name
                    self.max_messages = max_messages
                    self.message_size = message_size
                    self.ms_attrs = ms_attrs
                    self.memsection = ms
                    self.kernel_buffer_vaddr = None # later set in 'layout_cells_post'
                    self.cap_id = -1
                    self.buffer_mapped = buffer_mapped
                    self.virqs = [None] * 4

                def _get_size(self):
                    """Return the size of the channel buffer."""
                    return self.ms_attrs.size

                size = property(_get_size)

            class TZMonitor(KernelObject):
                """
                The ability to assign a TrustZone monitor service within a
                domain.
                """

                def __init__(self, svc_num):
                    KernelObject.__init__(self)

                    self.svc_num = svc_num


            # For class Domain
            def __init__(self, max_caps, max_threads, max_prio, max_reply_caps,
                        manager, owner_domain, segment_table,
                        map_in_external_objects=True):
                KernelObject.__init__(self)

                if max_caps is None:
                    max_caps = DEFAULT_MAX_CAPS
                if max_threads is None:
                    max_threads = 1

                self.max_caps = max_caps
                self.max_threads = max_threads
                self.max_prio = max_prio
                self.max_reply_caps = max_reply_caps
                self.plat_control = None
                self.processor_control = None
                self.profile_control = None
                self.intlock_control = None
                self.manager = manager
                self.owner_domain = owner_domain
                self.mmu_contexts = []
                self.irqs = []
                self.virqs = []
                self.virq_lines = []
                self.mutexes = []
                self.segments = []
                self.domains = []
                self.caps = []
                self.tracebuffer = None
                self.okl4_tracebuffer_control_object = None
                self.switch_tables = []
                self.message_queues = []
                self.channels = []
                self.tzmonitor_svcs = []
                self.segment_table = segment_table
                self.map_in_external_objects = map_in_external_objects
                self.namespace = None

            def register_mmu_context(self, ns_node, env_prefix, max_priority,
                                     l2_pgtable_buffer_nb, num_vuas, intlock_cap=None):
                mmu = self.MMU_Context(self, ns_node, env_prefix,
                                       max_priority, l2_pgtable_buffer_nb,
                                       num_vuas, self.segment_table, intlock_cap)
                self.mmu_contexts.append(mmu)

                return mmu

            def register_irq(self, irq):
                irq_obj = self.IRQ(irq)
                self.irqs.append(irq_obj)

                return irq_obj

            def register_virq(self, parent_namespace, source):
                virq_obj = self.VIRQ(parent_namespace, source, self)
                self.virqs.append(virq_obj)

                return virq_obj

            def register_virq_line(self, name):
                virq_line_obj = self.VirqLine(name)
                self.virq_lines.append(virq_line_obj)

                return virq_line_obj

            def snap_virqs(self):
                for virq in self.virqs:
                    source = virq.namespace.lookup(virq.source)

                    if source is None:
                        raise MergeError, 'Virq source "%s" cannot be found.' % \
                              virq.source
                    else:
                        source.connect_source(virq)
                        virq.unsourced = False

            def register_platform_control(self, name):
                if self.plat_control is not None:
                    raise MergeError, \
                        "Multiple platform control items specified.  " \
                        "Only one may be allocated to a cell"

                self.plat_control = self.PlatformControl(name, self)

                return self.plat_control

            def register_processor_control(self, name):
                if self.processor_control is not None:
                    raise MergeError, \
                        "Multiple processor control items specified. " \
                        "Only one may be allocated to a cell."

                self.processor_control = self.ProcessorControl(name, self)

                return self.processor_control

            def register_profile_control(self, name):
                if self.profile_control is not None:
                    raise MergeError, \
                        "Multiple profile control items specified. " \
                        "Only one may be allocated to a cell."

                self.profile_control = self.ProfileControl(name, self)

                return self.profile_control

            def register_intlock_control(self, name):
                if self.intlock_control is not None:
                    raise MergeError, \
                        "Multiple intlock control items specified. " \
                        "Only one may be allocated to a cell."

                self.intlock_control = self.IntlockControl(name, self)

                return self.intlock_control

            def register_mutex(self, name, is_counted, holder = None,
                    is_hybrid = False):
                mutex = self.Mutex(self, name, is_counted, holder, is_hybrid)
                self.mutexes.append(mutex)
                if holder is not None:
                    assert is_hybrid is False # See cells/collection/mutex.py
                    holder.held_mutexes.append(mutex)

                return mutex

            def register_segment(self, attrs, use_as_heap = True):
                """
                Register a segment with the domain.  The segment
                occupies the physical address range given in attrs.
                If use_as_heap is True, then is is possible to use the
                segment as a kernel heap (provided that it's address
                and cap rights allow)
                """
                assert hasattr(attrs, 'virt_addr')
                segment = self.Segment(self, attrs, use_as_heap)
                self.segments.append(segment)

                return segment

            def register_shared_segment(self, src_segment, local_cap):
                assert local_cap in self.caps

                shared_segment = self.SharedSegment(self, src_segment, local_cap)

                return shared_segment

            # replace a number of small segments with a
            # single large segment
            def replace_segments(self, physpool, attrs, sas):

                def fixup_mappings(self, old, new):
                    for mmu in self.mmu_contexts:
                        mappings = mmu.mappings[:]

                        for m in mappings:
                            if m[0] == old:
                                mmu.mappings.remove(m)
                                mmu.mappings.append((new, m[1]))


                #Fixme: this doesn't work with physpools with more than
                # one range
                assert(len(physpool.fulllist)==1)
                range = physpool.fulllist[0]
                attrs.phys_addr = range.base
                attrs.size = range.end - range.base
                attrs.attach = PF_R | PF_W | PF_X
                segments = self.segments[:]

                for segment in segments:
                    if segment.attrs.physpool == physpool.name and \
                            segment.attrs.phys_addr == None:
                        self.segment_table.deregister(segment)
                        for mmu in self.mmu_contexts:
                            del mmu.seg_attachments[mmu.seg_attachments.index(segment)]

                attrs.register_sas(sas)
                new = self.register_segment(attrs.for_sas(sas))
                for mmu in self.mmu_contexts:
                    mmu.register_attachment(new)

                for segment in segments:
                    if segment.attrs.physpool == physpool.name and \
                            segment.attrs.phys_addr == None:
                        self.segments.remove(segment)
                        fixup_mappings(self, segment, new)

                return new

            def register_cap(self, source_object, rights_mask):
                cap = self.Cap(source_object, rights_mask)
                self.caps.append(cap)

                return cap

            def register_tracebuffer(self, attrs, buffers):
                """Register the tracebuffer with this domain."""
                self.tracebuffer = self.TraceBuffer(self, attrs, buffers)
                return self.tracebuffer

            def register_switch_table(self, domain, mmu, entry_symbols,
                    entry_mode, return_stub):
                switch_table = self.SwitchTable(domain, mmu, entry_symbols,
                        entry_mode, return_stub)
                self.switch_tables.append(switch_table)

                return switch_table

            def register_message_queue(self, name, max_messages, mmu, quota):
                message_queue = self.MessageQueue(self, name, max_messages, mmu,
                        quota)
                self.message_queues.append(message_queue)

                return message_queue

            def register_channel(self, name, max_messages, message_size,
                    ms_attrs, ms, buffer_mapped):
                channel = self.Channel(self, name, max_messages, message_size,
                        ms_attrs, ms, buffer_mapped)
                self.channels.append(channel)

                return channel

            def register_tzmonitor_service(self, svc_num):
                if 'trustzone' not in Autogen.features:
                    raise MergeError("TrustZone not supported by this kernel.")

                tzmonitor = self.TZMonitor(svc_num);
                self.tzmonitor_svcs.append(tzmonitor)

                return tzmonitor

        class SegmentTable(object):
            """
            An ordered set of segments

            A pseudo collection to allocate mmu attachment indexes to
            segments.  To simplify libokl4, a segment will have the same index
            for all mmus in a cell.

            """
            def __init__(self):
                self.table = []

            def __len__(self):
                """Return the number of segments in the table."""
                return len(self.table)

            def __iter__(self):
                """Iterate over the segments in the table."""
                return self.table.__iter__()

            def index(self, x):
                """Return the index of the given segment."""
                return self.table.index(x)

            def register(self, segment):
                """
                Allocate an index for segment if it is not there already.
                """
                if segment not in self.table:
                    self.table.append(segment)

            def deregister(self, segment):
                """Deregister a segment, does not reassign indices."""
                if segment in self.table:
                    id = self.table.index(segment)
                    del self.table[id]

        # For class Heap
        def __init__(self, size):
            KernelObject.__init__(self)

            self.name = None
            self.size = size
            self.heap = None
            self.domains = []
            self.virt_addr = None
            self.phys_addr = None
            self.env = None
            self.segment_table = self.SegmentTable()
            self.namespace = None

        def register_domain(self, parent_namespace, max_caps, max_threads,
                            max_prio, max_reply_caps, manager,
                            owner_domain, map_in_external_objects=True):

            domain = self.Domain(max_caps, max_threads, max_prio,
                    max_reply_caps, manager, owner_domain,
                    self.segment_table,
                    map_in_external_objects=map_in_external_objects)
            domain.namespace = parent_namespace
            self.domains.append(domain)

            return domain

        def register_environment(self, env):
            self.env = env

        def get_preallocated_objects(self):
            """
            Return a dictionary of objects that should be preallocated on the
            heap.

            Non-dynamic object managers (such as objmanager_trivial) will use
            these values to determine how many of each type of object should be
            created.

            Dynamic heaps (such as objmanager_simple) may choose to ignore
            these values, as no objects need to be preallocated.
            """

            threads = 0
            mutexes = 0
            tracebuffers = 0
            irqs = 0
            virqs = 0
            virq_lines = 0
            plat_controls = 0
            processor_controls = 0
            profile_controls = 0
            intlock_controls = 0
            tzmonitor_svcs = 0

            for domain in self.domains:
                threads += domain.max_threads
                mutexes += len(domain.mutexes)
                irqs += len(domain.irqs)
                virqs += len(domain.virqs)
                virq_lines += len(domain.virq_lines)
                if 'tracebuffer' in Autogen.modules:
                    tracebuffers += domain.tracebuffer != None
                plat_controls += domain.plat_control != None
                processor_controls += domain.processor_control != None
                profile_controls += domain.profile_control != None
                intlock_controls += domain.intlock_control != None
                tzmonitor_svcs += len(domain.tzmonitor_svcs)

            res = {
                    'thread' : threads,
                    'mutex' : mutexes,
                    'interrupt' : irqs,
                    'virtual_interrupt' : virqs,
                    'virtual_interrupt_lines' :virq_lines,
                    'platform_control' : plat_controls,
                    'processor_control' : processor_controls,
                    'profile_control' : profile_controls,
                    'intlock_control' : intlock_controls,
                    'tzmonitor' : tzmonitor_svcs,
                    }

            if 'tracebuffer' in Autogen.modules:
                res.update({'tracebuffer_control' : tracebuffers})

            return res

    # For class Kernel
    def __init__(self):
        XmlCollector.__init__(self)
        self.kernel = None
        self.elf = None
        self.configs = []
        self.base_segment = None
        self.heaps = {}
        self.heap_attrs = None
        self.dynamic_heap_size = False
        self.total_dynamic_heap_size = 0
        self.total_misc_heap_size = 0 # for tracebuffer and channels
        self.kernel_only_mappings = []
        self.kernel_shared_mappings = []
        self.tracebuffer = None
        self.product = None
        self.group = None
        self.sas = None

    def register_kernel_mapping(self, attrs):
        self.kernel_only_mappings.append(attrs)

    # XXX: Used to be register_heap
    def register_kernel_heap(self, attrs):
        self.heap_attrs = attrs

        if attrs.size is None:
            self.dynamic_heap_size = True
            attrs.size = 0
        else:
            self.dynamic_heap_size = False

    def register_tracebuffer(self, domain, attrs, buffers):
        """
        Register the tracebuffer with a domain.

        Only one tracebuffer is allowed and it may only be registered
        with one domain.
        """
        if self.tracebuffer is not None:
            raise MergeError, \
                    "Multiple trace buffers specified.  " \
                    "Only one cell may be allocated a tracebuffer."

        # Add the tracebuffer to the memory to be allocated as part of
        # the kernel heap.  Doing do means that the the kernel can
        # access the tracebuffer using it's simplified virt->phys
        # mappings.
        self.total_dynamic_heap_size += attrs.size
        self.total_misc_heap_size += attrs.size
        self.heap_attrs.size += attrs.size

        self.tracebuffer = domain.register_tracebuffer(attrs, buffers)
        return self.tracebuffer

    def register_channel_for_heap(self, kern_chn):
        """
        Record the size of the channel for allocating the kernel heap.
        """

        self.total_misc_heap_size += kern_chn.size
        self.heap_attrs.size += kern_chn.size

    def register_heap(self, parent_namespace, heap_tag, heap_size):
        """Register a heap with the kernel."""

        heap = self.Heap(heap_size)
        self.heaps[heap_tag] = heap
        heap.name = heap_tag
        heap.namespace = parent_namespace.add_namespace("kernel_heap",
                                                        "cell kernel heap")

        self.total_dynamic_heap_size += heap_size

        if self.dynamic_heap_size:
            self.heap_attrs.size += heap_size
        else:
            if self.total_dynamic_heap_size > self.heap_attrs.size:
                raise MergeError, \
                      "Cells are requesting more total kernel heap " \
                      "(%#x bytes) then is allocated (%#x bytes)." % \
                      (self.total_dynamic_heap_size, self.heap_attrs.size)

        return heap

    def get_heap(self, heap_tag):
        """Return the details of the named cell."""

        return self.heaps.get(heap_tag)

    def add_config(self, key, value):
        """Record a configuration property."""
        pass

    def layout_cells_post(self):
        """Post-layout processing of kernel structures."""
        # This must be called after image.layout() and before the cell
        # init scripts are written out.
        #
        # Do any required layout here

        next_phys_base = self.heap_attrs.phys_addr
        next_virt_base = self.heap_attrs.virt_addr_sas(self.sas)

        heap_phys_end = self.heap_attrs.phys_addr + self.heap_attrs.size
        heap_virt_end = self.heap_attrs.virt_addr_sas(self.sas) + \
                        self.heap_attrs.size

        # Give addresses to the individual heaps.
        for heap in self.heaps.values():
            heap.virt_addr = next_virt_base
            next_virt_base += heap.size
            assert next_virt_base <= heap_virt_end

            heap.phys_addr = next_phys_base
            next_phys_base += heap.size
            assert next_phys_base <= heap_phys_end

        #
        # layout for misc elements on heap
        #
        next_virt_base = heap_virt_end - self.total_misc_heap_size
        next_phys_base = heap_phys_end - self.total_misc_heap_size

        # Calcuate the address of the tracebuffer.  attrs.virt_addr
        # represents that address of the tracebuffer in userland and
        # should not be changed because it is used to map in the
        # tracebuffer.  The kernel uses a different address, stored
        # here in kernel_virt_addr, which is used when creating the
        # kernel tracebuffer object.  This latter address is patched
        # into the kernel.

        if self.tracebuffer is not None:
            ms_size = self.tracebuffer.attrs.size
            # Put the traecbuffer at the end of the allocated memory.
            tb_align = self.tracebuffer.attrs.align
            next_virt_base = align_up(next_virt_base, tb_align)
            next_phys_base = align_up(next_phys_base, tb_align)
            # There must be room for the tracebuffer.
            assert next_phys_base + ms_size <= heap_phys_end
            assert next_virt_base + ms_size <= heap_virt_end

            self.tracebuffer.kernel_virt_addr = next_virt_base
            next_virt_base += ms_size
            self.tracebuffer.attrs.phys_addr = next_phys_base
            self.tracebuffer.memsection.segment.paddr = next_phys_base
            next_phys_base += ms_size


#        import pdb; pdb.set_trace()

        for heap in self.heaps.values():
            for domain in heap.domains:
                for chn in domain.channels:
                    if chn.ms_attrs.phys_addr == Pools.IGNORE:
                        # The channel must be allocated in the kernel heap.
                        ms_size = chn.ms_attrs.size
                        assert next_phys_base + ms_size <= heap_phys_end
                        assert next_virt_base + ms_size <= heap_virt_end

                        chn.kernel_buffer_vaddr = next_virt_base
                        next_virt_base += ms_size

                        chn.ms_attrs.phys_addr = next_phys_base
                        chn.memsection.segment.paddr = next_phys_base
                        next_phys_base += ms_size
                    else:
                        # The channel is in the MMU's copy area, and the
                        # kernel can access it directly with the user address.
                        chn.kernel_buffer_vaddr = chn.ms_attrs.virt_addr

        # Trim the heap allocation by the size of the tracebuffer and channels
        # so that it won't appear in the memory map that the heap(s) and the
        # tracebuffer overlap.
        self.heap_attrs.size -= self.total_misc_heap_size

    def create_dynamic_segments(self, pools):
        """ Do final thread array and heap calculations. """

        # Mark the addresses, if they are fixed.
        self.sas.mark_physical(self.heap_attrs.abs_name(),
                               self.heap_attrs.phys_addr,
                               self.heap_attrs.size,
                               self.heap_attrs.cache_attr)
        self.sas.mark_virtual(self.heap_attrs.abs_name(),
                              self.heap_attrs.virt_addr_sas(self.sas),
                              self.heap_attrs.size,
                              self.heap_attrs.cache_attr)

        domains = sum([heap.domains for heap in self.heaps.values()], [])

        for domain in domains:
            domain.snap_virqs()

    def generate_init_script(self, image, machine):
        """
        Generate kernel data structures.
        """
        self.kernel.generate_kernel_structures(self, image.elf, image,
                                               machine)

    def collect_use_devices(self, el, image, machine, pools):
        """Collect information about devices used by the kernel."""

        # Iterate through devices used by the kernel, store their memory ranges
        for device_el in el.find_children("use_device"):
            dev = machine.get_phys_device(device_el.name)
            # print "driver %s has keys: %s" % (dev.name, dev.physical_mem.keys())
            self.kernel.add_device_mem(self, dev, image, machine, pools)

    def load_kernel(self,  kernel_el, machine, cust):
        """
        Load (and if needed link) the kernel and then initialise the
        autogen subsystem to point to the correct types and machine
        properties.
        """
        self.elf = get_kernel_file(kernel_el, machine)
        if self.elf.elf_type != ET_EXEC:
            raise MergeError, \
                  "All the merged ELF files must be of EXEC type. (%s)" % \
                  str(self.elf)

        # We setup our kernel type depending on the api version number
        check_api_versions(self.elf)

        elfweaver_info = find_elfweaver_info(self.elf)
        self.product = elfweaver_info.product
        self.arch = elfweaver_info.arch
        self.cpu = elfweaver_info.cpu_desc
        machine.set_elfweaver_info(elfweaver_info)

        # Setup the Autogen variable based on the loaded kernel.
        weaver.autogen_helper.initialise(cust, elfweaver_info.product,
                                         elfweaver_info.arch,
                                         elfweaver_info.cpu_desc,
                                         elfweaver_info.cpu_features,
                                         elfweaver_info.debug,
                                         self.elf.endianess)


    def collect_xml(self, kernel_el, kernel_heap_size, namespace, image,
                    machine, pools, cust):
        """Collect the attributes of the kernel element."""

        def get_kernel_filename(kernel_el):
            """Return the kernel filename."""
            return getattr(kernel_el, 'file', None)

        # New namespace for objects living in the kernel.
        kernel_namespace = namespace.add_namespace('kernel', "Kernel")

        # Find out if we have an MMU or using 1:1 mappings.
        image.is_unified = Autogen.mmu.has_unified_address_space()

        # Setup the machine in the autogen type
        Autogen.machine = machine

        # Create the OKL4 kernel object.
        self.kernel = weaver.kernel_okl4.OKL4Kernel(self.elf, kernel_namespace)

        # Create any special pools for the kernel.
        self.kernel.add_kernel_pools(machine, pools)

        # Create a group to store segments and memsections.
        self.sas = pools.new_sas(kernel_namespace.abs_name())
        self.group = image.add_kernel_group(self.sas)

        self.kernel.reserve_kernel_regions(self.sas)

        # Record the default pools for cells to use if they don't have
        # defaults set.
        virtpool = getattr(kernel_el, "virtpool", None)
        physpool = getattr(kernel_el, "physpool", None)

        image.set_attrs_stack(def_virt = virtpool,
                              def_phys = physpool,
                              def_priority = getattr(kernel_el, "priority", None),
                              def_timeslice = getattr(kernel_el, "timeslice", None),
                              def_affinity = getattr(kernel_el, "affinity", None))
        pools.set_default_pools(virtpool, physpool)

        segment_els = kernel_el.find_children("segment")
        patch_els = kernel_el.find_children("patch")

        image.set_kernel(self.elf)

        # Load up the kernel segments from the binary.
        segs = collect_elf_segments(self.elf, segment_els,
                                    'kernel', 'kernel', [],
                                    self.group, kernel_namespace,
                                    image, machine)

        # Remember the kernel segments so that they can be mapped into the
        # kernel's page table.
        for seg in segs:
            self.register_kernel_mapping(self.group.sas_attrs(seg.attrs))

        # Set base segment
        self.base_segment = self.kernel.get_base_segment(self.elf, segs, image)

        if self.base_segment.attrs.phys_addr is not None and \
            (self.base_segment.attrs.phys_addr & ((self.one_MB) - 1)) != 0:
            raise MergeError("Physical address of %s must be 1MB aligned" %
                                    self.base_segment.attrs.abs_name())

        if self.base_segment.attrs.align is None:
            self.base_segment.attrs.align = self.one_MB
        elif (self.base_segment.attrs.align & ((self.one_MB) - 1)) != 0:
            raise MergeError("Alignment of %s must a multiple of 1MB." %
                                    self.base_segment.attrs.abs_name())

        # Copy the elfweaver_info note into the image to that it can be
        # accessed by other elfweaver commands that require the autogen
        # types.
        copy_elfweaver_info(self.elf, image.elf)

        self.elf = self.elf.prepare(self.elf.wordsize, self.elf.endianess)

        # Register the kernel ELF with the --emit-relocs option.
        image.constituent_elfs.append(('kernel', 'kernel', self.elf))

        # The extra_patches attr may be added by a plugin.
        for patch in getattr(Kernel_el, "extra_patches", []):
            sym = get_symbol(image.elf, patch[0], True)
            if sym is not None:
                addr = int(sym.value) + int(patch[1])
                new_patch = Patch_el(address=hex(addr), bytes=patch[2],
                                     value=patch[3]) # FIXME:... value and bytes are backwards?
                patch_els.append(new_patch)

        # We use image.elf, NOT self.elf, since image.elf is the resulting
        # elf file. self.elf is merely the input elf file, so patching that is
        # pointless.
        collect_patches(image.elf, 'kernel', patch_els,
                         get_kernel_filename(kernel_el), image)

        # Collect the config elements, some of these relate to
        # heap and thread array, so do it first
        config_el = kernel_el.find_child("config")
        if config_el is not None:
            for option in config_el.find_children("option"):
                self.add_config(option.key, option.value)

        heap_attrs = \
                   image.new_attrs(kernel_namespace.add_namespace("heap",
                                                                  "total kernel heap"))
        heap_attrs.align = machine.kernel_heap_align
        if heap_attrs.align is None and \
                not Autogen.mmu.has_unified_address_space():
            # If we have an MMU, default to aligning the heap to 16K unless
            # the user has specified otherwise.  16K is used because the L1
            # page table is 16K aligned and tests have shown that matching the
            # kernel heap alignment to 16K, and not 4K, produces a
            # consistently smaller heap.
            heap_attrs.align = 16 * 1024
        heap_el = kernel_el.find_child("heap")
        if heap_el is not None:
            heap_attrs.phys_addr = getattr(heap_el, 'phys_addr',
                                           heap_attrs.phys_addr)
            heap_attrs.size = getattr(heap_el, 'size', heap_attrs.size)
            heap_attrs.align = getattr(heap_el, 'align', heap_attrs.align)

        # Override the size with the command line value, if present.
        if kernel_heap_size != 0:
            heap_attrs.size = kernel_heap_size

        heap = image.set_kernel_heap(heap_attrs, machine,
                                     self.base_segment.segment)

        self.group.add_kernel_heap(heap, segs[0])

        self.register_kernel_heap(heap_attrs)

        self.collect_use_devices(kernel_el, image, machine, self.sas)

        # Patch the kernel with the base address of its physical pool
        # NOTE:  The first kernel segment *MUST* be placed at this
        # address.
        ppool = self.sas.get_physical_pool_by_name(physpool)
        physical_memory = ppool.get_lowest_addr()
        sym = get_symbol(image.elf, "_start_kernel_phys", True)
        if sym is not None:
            image.patch(sym, physical_memory)

def check_api_versions(kernel):
    """ Return the kernel API version number.  """
    kern_sym = get_symbol(kernel, "kernel_api_version", True)
    if kern_sym:
        addr = kern_sym.value
        size = kern_sym.size
    else:
        return None
    kern_ver = kernel.get_value(addr, size, kernel.endianess)

    soc_sym = get_symbol(kernel, "soc_api_version")
    addr = soc_sym.value
    size = soc_sym.size
    soc_ver = kernel.get_value(addr, size, kernel.endianess)
    if soc_ver == None:
        raise MergeError("Unable to locate soc api symbol")

    if kern_ver != soc_ver:
        raise MergeError("Kernel api version %d doesn't match soc api version %d" % (kern_ver, soc_ver))

# Returns a fully linked kernel.o this may either be;
#
# file=<kernel.o> - we just return the specified file
# OR
# configuration=<kernel-config> AND platform=<platform-file>
#  - We need to both construct the full paths for the kernel and platform and
#    shell out to the linker.sh or linker.bat to built the kernel then return
#    that file
# OR
# linker=<linker-wrapper> AND kernel=<partial kernel.o> AND soc=<soc-file> AND
# libs=[<libs>] - Shell out to the linker but no need for directory expansion
def get_kernel_file(kernel_el, machine):
    def relative(path):
        """
        Return a path taking relativity to XML include directories into account
        """
        return os.path.join(kernel_el._path, path)

    sdk_root = os.environ.get("OKL4_SDK")
    if has_kernel_attr(kernel_el, KERNEL_PRELINK_TAGS):
        elf = UnpreparedElfFile(filename = relative(kernel_el.file))
        return elf
    elif has_kernel_attr(kernel_el, KERNEL_SDKLINK_TAGS):
        if sdk_root is None:
            print "Please set OKL4_SDK environment variable to point to the root of SDK"
            sys.exit(1)
        cpu = machine.cpu.replace(':', '_')
        kernel_dir = os.path.join(relative(sdk_root), "kernel", cpu)
        object_dir = os.path.join(kernel_dir, kernel_el.configuration, "object")
        lib_dir = os.path.join(kernel_dir, kernel_el.configuration, "libs")

        linker_script = os.path.abspath(os.path.join(object_dir, "linker.lds"))
        linker_wrapper = os.path.abspath(os.path.join(object_dir, "linker.sh"))
        kernel = os.path.abspath(os.path.join(object_dir, cpu + ".o"))
        soc = os.path.abspath(os.path.join(object_dir, kernel_el.platform +
                                           ".o"))
        libs = [os.path.abspath(lib) for lib in glob.glob(os.path.join(lib_dir,
                                                          "*.a"))]
        if not os.path.exists(kernel):
            product, debug = kernel_el.configuration.split("-")
            debug_str = ""
            if debug == "debug":
                debug_str = "_debug"

            # Re-writing the debug value here since the configuration
            # attribute is there only for backwards compatibility.
            if has_kernel_attr(kernel_el, KERNEL_NEW_SDKLINK_TAGS):
                debug = kernel_el.debug.lower()
                debug_str = ""
                if debug == "true":
                    debug_str = "_debug"

            kernel_dir = os.path.join(relative(sdk_root), "cpus", cpu)

            linker_script = os.path.abspath(os.path.join(kernel_dir, "linker.lds"))
            linker_wrapper = os.path.abspath(os.path.join(kernel_dir, "linker.sh"))
            kernel = os.path.abspath(os.path.join(kernel_dir, "kernel%s_%s.o" % (debug_str, cpu)))
            platform = kernel_el.platform
            if platform == "imx31":
                board = platform + "_kzm_arm11"
            else:
                board = "beagleboard-c3"
            soc = os.path.abspath(os.path.join(relative(sdk_root), "boards", board, "board%s_%s.o" % (debug_str, board)))

        return link_kernel(kernel, soc, libs, machine)
    elif has_kernel_attr(kernel_el, KERNEL_LINK_TAGS):
        libs = [relative(lib.strip()) for lib in kernel_el.libs.split(",")]
        return link_kernel(relative(kernel_el.kernel),
                           relative(kernel_el.soc),
                           libs, machine)
    else:

        if hasattr(kernel_el, "debug") and kernel_el.debug.lower() == "true":
            debug_str = "_debug"
        else:
            debug_str = ""

        if has_kernel_attr(kernel_el, set(["configuration"])):
            product, debug = kernel_el.configuration.split("-")
            product = product.split("okl4", 1)[1]
            if debug == "true":
                debug_str = "_debug"
        else:
            product = "microvisor"

        kernel_dir = os.path.join(kernel_el._path, "cpus", machine.cpu)
        kernel_name = "%s-%s%s.o" % (machine.cpu, product, debug_str)
        kernel = check_file_exists(kernel_name, kernel_dir)

        soc_dir = os.path.join(kernel_el._path, "boards", machine.board)
        soc_name = "board%s_%s.o" % (debug_str, machine.board)
        soc = check_file_exists(soc_name, soc_dir)

        libs = []

        return link_kernel(kernel, soc, libs, machine)

    raise MergeError("Invalid kernel tags; a prelinked kernel, SDK information or linking information must be provided")

# Given a set of tags check if the kernel element contains all the tags and
# that it does not contain any other tags
def has_kernel_attr(kernel_el, test_tags):
    for tag in test_tags:
        if not hasattr(kernel_el, tag):
            break
    else:
        for tag in KERNEL_TAGS.difference(test_tags):
            if hasattr(kernel_el, tag):
                raise MergeError("Found unexpected kernel tag %s when %s has already been specificed" % (tag, test_tags))

        return True

    return False

# Given a linker, kernel, platform and set of libraries shell out to the
# linker and return the generated fully linked kernel.o location
def link_kernel( kernel, soc, libraries, machine):
    elf = link([kernel, soc] + libraries, kernel_soc = True)

    # Kernel is linked, now add the segment names
    add_segment_names(elf, get_segment_names(elf))

    return elf

def get_linker_type(linker_script, machine):
    """
    Determine the type of linker from the linker script.
    """
    return "gnu"
    ls_buffer = open(linker_script, "r").read()

    if "NOCOMPRESS" in ls_buffer:
        return "rvct"
    elif "SECTIONS" in ls_buffer:
        if ("vectors PT_LOAD" not in ls_buffer or
            machine.cpu in ["cortex-a8-r1p2"]):  # armv7
            return "thumb"
        else:
            return "gnu"
    else:
        raise MergeError("Unknown linker.lds type.")
