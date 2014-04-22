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
Support class for a cell that contains a single okl4 program.
"""
import re

from elf.constants import PF_R, PF_W, PF_X
from elf.ByteArray import ByteArray
from elf.util import align_up, align_down

from weaver import MergeError
from weaver.cells import AbstractCell
from weaver.memobjs_xml import collect_memobj_attrs
from weaver.ezxml import Element, str_attr, bool_attr, long_attr, \
     size_attr, ParsedElement
from weaver.segments_xml import Heap_el, \
        make_pager_attr, generate_section_prefix
from weaver.okl4_cell_environment import KernelCellEnvironment
from weaver.cap import SegmentRights, PhysAddrRights, \
     TraceBufferControlRights, RightsCopyData

from pyokl4.autogen import Autogen
from weaver.cells.collection.physmem_item import  PhysMemItem_el
from weaver.cells.collection.pd import Program_el, PD_el, Extension_el, \
        ProtLib_el, collect_pd
from weaver.cells.collection.environment import Environment_el, \
        collect_environment
from weaver.kernel import Kernel

# If the vclient module is not present, treat as nops.
try:
    from weaver.cells.collection.vclient import snap_vclients
except:
    snap_vclients = lambda *args: 0

TraceBuffer_el = Element("tracebuffer",
                         size = (size_attr, "required"),
                         virt_addr = (long_attr, "optional"),
                         phys_addr = (long_attr, "optional"),
                         physpool = (str_attr, "optional"),
                         virtpool = (str_attr, "optional"),
                         align = (size_attr, "optional"),
                         attach = (str_attr,  "optional"),
                         direct = (bool_attr, "optional"),
                         zero = (bool_attr, "optional"),
                         pager = (str_attr, "optional"),
                         cache_policy = (str_attr, "optional"),
                         mem_type = (str_attr, "optional"),
                         cache_policy_mask = (long_attr, "optional"),
                         buffers = (long_attr, "optional"))

Arg_el = Element("arg",
                 value = (str_attr, "required"))
CommandLine_el = Element("commandline", Arg_el)

PlatformControl_el = Element("platform_control",
                             name = (str_attr, "required"))

ProcessorControl_el = Element("processor_control",
                              name = (str_attr, "required"))

ProfileControl_el = Element("profile_control",
                              name = (str_attr, "required"))

IntlockControl_el = Element("intlock_control",
                              name = (str_attr, "required"))

Cell_el = Element("cell", Heap_el, Environment_el, CommandLine_el,
                  PlatformControl_el, TraceBuffer_el, PD_el, Program_el,
                  Extension_el, ProtLib_el, PhysMemItem_el,
                  ProcessorControl_el, ProfileControl_el,
                  IntlockControl_el,
                  name = (str_attr, "required"),
                  kernel_heap = (size_attr, "required"),
                  physpool = (str_attr, "optional"),
                  exclusive_physpool = (bool_attr, "optional"),
                  virtpool = (str_attr, "optional"),
                  pager = (str_attr, "optional"),
                  direct = (bool_attr, "optional"),
                  priority = (long_attr, 'optional'),
                  timeslice = (long_attr, 'optional'),
                  affinity = (long_attr, 'optional'),
                  # The following values are not used at the moment
                  caps = (long_attr, "optional"),
                  mutexes = (long_attr, "optional"),
                  threads = (long_attr, "optional"),
                  domain = (long_attr, "optional"),
                  mmu = (long_attr, "optional"))

DEFAULT_HEAP_SIZE = 0x10000
# FIXME: Jira ticket OLD-1683 - awiggins.
# Replace with a default buffers value.
DEFAULT_TRACEBUFFER_BUFFERS = 2 #Autogen.types.TRACEBUFFER_NUM_BUFFERS

class Cell(AbstractCell):
    # disable: Too many arguments
    # pylint: disable-msg=R0913
    """
    Cell for OKL4 programs, pds, drivers and other matters.
    """

    element = Cell_el

    def __init__(self):
        AbstractCell.__init__(self)

        self.heap_ms = None
        self.stack_ms = None
        self.utcb_ms = None
        self.elf = None
        self.api_version = None
        self.def_virtpool = None
        self.def_physpool = None
        self.phys_addr = None
        self.virt_addr = None
        self.phys_attrs = None
        self.virt_attrs = None
        self.virt_pool = None
        self.env = None
        self.heap = None
        self.namespace = None
        self.domain = None
        self.mmu_context = None
        self.main_kdomain = None
        self.main_kmmu = None
        self.main_kthread = None
        self.attachments = None
        self.group = None
        self.env_address_space = None
        self.mappings = [] # List of (env name, segment attr) tuples.
        self.realms = []
        self.exclusive_physpool = False
        self.sharing_group = None
        self.vclients = []
        self.vservers = []

    def get_rights_copy_data(self):
        """
        Return a cap.RightsCopyData object for use when resolving caps from
        other cells.
        """
        return RightsCopyData(domain = self.domain,
                              image_group = self.sharing_group)

    def collect_xml(self, el, _, namespace, machine, pools, kernel, image):
        """Handle an OKL4 Cell Compound Object"""
        self.name = el.name
        self.namespace = namespace.add_namespace(self.name, "Cell")

        self.sas = pools.new_sas(self.namespace.abs_name())

        image.push_attrs(getattr(el, 'virtpool', None),
                         getattr(el, 'physpool', None),
                         make_pager_attr(getattr(el, 'pager', None)),
                         getattr(el, 'direct', None),
                         getattr(el, 'priority', None),
                         getattr(el, 'timeslice', None),
                         getattr(el, 'affinity', None))

        self.exclusive_physpool = getattr(el, 'exclusive_physpool', False)

        self.heap = kernel.register_heap(self.namespace, self.name,
                    el.kernel_heap)
        self.env = KernelCellEnvironment(el.name, self.namespace,
                    machine, image, self.elf, None)
        self.heap.register_environment(self.env)

        # Create a group, outside any program realms, for allocating addresses
        # for memsections from other cells that we want to share.  If the
        # memsections where to be placed in the manager program's realm, a
        # number of complex environment entries would have to be created and
        # there is no mechanism to do that.
        self.sharing_group = image.add_group(self.sas)

        if 'realms' in Autogen.features:
            self.env_address_space = self.env.add_address_space_entry(
                "MAIN_ADDRESS_SPACE")

        #
        # Later in the merge process, symbols will be emitted with this name as
        # a substring. Ensure that the name is sane.
        #
        if not re.match("^[A-Za-z0-9_]+$", self.name):
            raise MergeError("The cell name '%s' contains invalid characters."
                    % self.name)

        # Unified images only support one program and very few other
        # elements.  Check here that the document conforms to this.
        if image.is_unified:
            for tag in ('pd', 'program', 'extension', 'protected_library',
                        'tracebuffer'):
                num_els = len(el.find_children(tag))

                if tag == 'program' and num_els > 1:
                    raise MergeError, \
                          'Only one "program" element is allowed ' \
                          'when building OKL4 Pico images.'
                elif tag != 'program' and num_els > 0:
                    raise MergeError, \
                          'The XML element "%s" is not allowed ' \
                          'when building OKL4 Pico images.' % tag

        # FIXME: Jira ticket OLD-1440 - jamesp.
        # We directly register the virtmem pool with each pd type object
        # rather than using attchments.  See awiggins, jamesp, nhannah and the
        # bug for further information.
        # Group for cell shared items.
        self.group = None

        # Locate the manager program, if there is only one program tag than
        # that program is the manager regardless of manager tag.  If multiple
        # programs are defined then either take the one with the manager tag
        # set to true (must only be one of these) otherwise take the first
        # program as the maanger if none are marked.
        prog_els = el.find_children('program')
        manager_prog_el = None

        if len(prog_els) == 1:
            manager_prog_el = prog_els[0]
            setattr(manager_prog_el, 'manager', True)
        else:
            for prog_el in prog_els:
                if getattr(prog_el, 'manager', False):
                    if manager_prog_el is not None:
                        raise MergeError("Only one program tag can be "
                                         "marked as manager")
                    manager_prog_el = prog_el

            if manager_prog_el is None and prog_els:
                # If user doesn't specify we use the first one
                manager_prog_el = prog_els[0]
                setattr(manager_prog_el, 'manager', True)

        if manager_prog_el is None:
            raise MergeError("At least one program tag must be provided.")

        # Now process the manager program tag
        self.domain, self.mmu_context, threads, pd_pool_attr, pd_pool_seg, pd_pool, \
                self.attachments = \
                collect_pd(self, manager_prog_el, self.namespace, image, machine,
                           self.sas, self.heap, self.env,
                           generate_section_prefix(el.name),
                           self.env_address_space, kernel = kernel)

        # Collect all PD like tags
        for tag in ('pd', 'program', 'extension', 'protected_library'):
            # Make sure not to process the manager program twice
            for tag_el in [tag_el for tag_el in el.find_children(tag)
                           if tag_el is not manager_prog_el]:
                _, _, el_threads, _, _, _, el_attachments = \
                        collect_pd(self, tag_el, self.namespace, image, machine,
                                self.sas, self.heap, self.env,
                                generate_section_prefix(el.name),
                                self.env_address_space,
                                pd_pool_attr, pd_pool_seg, pd_pool, self.domain)
                threads.extend(el_threads)
                self.attachments.extend(el_attachments)

        (self.def_virtpool, self.def_physpool) = image.current_pools()

        collect_environment(el.find_child("environment"), self.env)

        # Add the machine info struct and populate it
        if 'kernel_smp' in Autogen.modules:
            machine_info = self.env.add_machine_info_entry("MACHINE_INFO",
                    machine.num_cpus)
        else:
            machine_info = self.env.add_machine_info_entry("MACHINE_INFO")
        if machine.l1_cache_line_size is not None:
            machine_info.l1_cache_line_size = machine.l1_cache_line_size
        if machine.l2_cache_line_size is not None:
            machine_info.l2_cache_line_size = machine.l2_cache_line_size

        # Set these up now even though we can't actually assign values
        # till later
        self.phys_attrs = image.new_attrs(self.namespace.add_namespace("default_physpool"))
        if not image.is_unified:
            self.phys_attrs.attach = PF_R | PF_W | PF_X

        self.phys_attrs.register_sas(self.attachments[0].image_realm.sas)
        kern_segment = self.domain.register_segment(self.phys_attrs.for_sas(self.attachments[0].image_realm.sas))
        self.mmu_context.register_attachment(kern_segment)
        self.env.add_physmem_segpool_entry("MAIN_PHYSMEM_SEGPOOL",
                                           kern_segment,
                                           self.mmu_context)

        if not image.is_unified:
            self.virt_attrs = image.new_attrs(self.namespace.add_namespace("default_virtpool"))
            self.virt_pool = self.env.add_virtmem_pool_entry("MAIN_VIRTMEM_POOL", self.virt_attrs)

        # Weave out any memsection information to the environment.
        # Record any Tracebuffers that are assigned to the initial program.
        if el.find_children("tracebuffer"):
            tb_el = el.find_child("tracebuffer")

            tb_attr = collect_memobj_attrs(tb_el, self.namespace,
                                         image, machine)

            # Tracebuffers are mapped in userspace read-only, and with
            # architecture-specific special cache attributes to avoid cache
            # aliasing.
            tb_attr.attach = PF_R
            tb_attr.cache_attr = "tracebuffer"

            # and needs page size alignment
            tb_attr.size = align_up(tb_attr.size, machine.min_page_size())

            # Do not allocate the physical address of the tracebuffer in the
            # normal manner.  The tracebuffer is allocated in the same chunk
            # of memory as the kernel heaps to avoid complex virt->phys
            # mappings in the kernel.
            tb_attr.phys_addr = pools.IGNORE

            # The tracebuffer will be loaded from its own elf segment.  The
            # final data will be calculated as part of generating the kernel
            # heap, but to ensure that a segment is created give it some dummy
            # data of the correct size.
            tb_attr.data = \
                         ByteArray('\0' * tb_attr.size)

            kernel.register_tracebuffer(self.domain, tb_attr,
                                        getattr(tb_el, "buffers",
                                                DEFAULT_TRACEBUFFER_BUFFERS))

            self.namespace.add("tracebuffer_control",
                    TraceBufferControlRights("tracebuffer_control"))
            self.env.add_cap_entry('TRACEBUFFER_CONTROL',
                    'tracebuffer_control', self.elf)

        # Find heap and add it
        heap_el = el.find_child('heap')
        if heap_el is None:
            heap_el = ParsedElement('heap')

        heap_attr = collect_memobj_attrs(heap_el, self.namespace,
                                         image, machine)

        if heap_attr.size is None:
            heap_attr.size = DEFAULT_HEAP_SIZE

        self.heap_ms = self.attachments[0].image_realm.add_memsection(heap_attr,
                                                                      machine)
        self.heap.heap = self.heap_ms.attrs
        sas_attrs = self.attachments[0].image_realm.sas_attrs(self.heap_ms.attrs)
        user_heap_seg = self.domain.register_segment(sas_attrs)
        self.mmu_context.register_mapping(user_heap_seg, sas_attrs)
        SegmentRights.add_standard_rights(user_heap_seg, heap_attr.ns_node)
        PhysAddrRights.add_standard_rights(user_heap_seg, heap_attr.ns_node)
        self.env.add_heap(self.heap.heap)

        # Add memsection for heap.
        self.register_realm_mapping('MEMSEC_MAIN_HEAP', user_heap_seg,
                                    self.attachments[0].env_realm,
                                    self.mmu_context)

        # Add command line arguments
        commandline_el = el.find_child("commandline")

        if commandline_el is not None:
            args = [arg_el.value
                    for arg_el in commandline_el.find_children("arg")]
        else:
            args = []

        self.env.add_arg_list(args)

        image.pop_attrs()

    def resolve_cross_cell_dependencies(self, machine, image):
        """
        Resolve any references to objects in other cells.
        """
        snap_vclients(self.vclients)

    def generate_dynamic_segments(self, machine, image):
        """
        Create environment buffers.
        """
        image.push_attrs(
            virtual = self.def_virtpool,
            physical = self.def_physpool)
        # Calculation whether machine has cache colour issues.
        ccalignment = False
        if machine.elfweaver_info.arch in ("armv6", "armv7"):
            if machine.l1_dcache_way_size is None:
                raise MergeError("l1_dcache_way_size is required on this platform.")
            else:
                if machine.l1_dcache_way_size > machine.min_page_size():
                    ccalignment = True

        # Allocate memory for the tracebuffer.
        if self.domain.tracebuffer is not None:
            tb_ms = \
                  self.attachments[0].image_realm.add_memsection(
                          self.domain.tracebuffer.attrs, machine,
                          require_cache_colour_alignment = ccalignment)
            self.domain.tracebuffer.memsection = tb_ms
            sas_attrs = self.attachments[0].image_realm.sas_attrs(tb_ms.attrs)
            tb_seg = self.domain.register_segment(sas_attrs)
            self.mmu_context.register_mapping(tb_seg, sas_attrs)
            self.register_realm_mapping('TRACEBUFFER', tb_seg,
                                        self.attachments[0].env_realm,
                                        self.mmu_context)
            SegmentRights.add_standard_rights(tb_seg,
                    self.domain.tracebuffer.attrs.ns_node)

        if 'realms' in Autogen.features:
            # Crop the address space back to just this cell's virtpool.
            self.prepare_address_space()

        # Create memsection objects in the environment.
        #self.create_env_memsections()

        # Add any mappings or environment entries attachments may require
        for attachment in self.attachments:
            attachment.resolve()

        # Search for environment objects to be created.
        self.find_weaved_objects_for_creation(image)

        # Create a segment table for the domain.
        self.env.add_segment_table("SEGMENT_TABLE", self.heap)

        # Setup a descriptor table for IRQs added either manually or by
        # 'collect_use_devices'.
        kern_irqs = []
        for domain in self.heap.domains:
            kern_irqs.extend(domain.irqs)
            kern_irqs.extend(domain.virqs)

        self.env.add_env_interrupt_list("INTERRUPT_LIST", kern_irqs)
        self.env.add_interrupt_device_map('NO_DEVICE',
                                          [kern_irq.irq for kern_irq in kern_irqs])

        # Resolve all caps and attachments in the environment
        self.env.snap(self)

        # Search for environment pointers to be weaved in.
        self.env.find_weaved_environment_requests(self.name, image)

        self.env.generate_dynamic_segments(self, image, machine, self.sas,
                                           self.attachments[0].image_realm)

        # If an exclusive physpool is used for this cell convert all segments
        # within it's range
        if self.exclusive_physpool:
            physpool = self.sas.get_physical_pool_by_name(self.def_physpool)
            physpool_attrs = image.new_attrs(self.namespace)
            self.domain.replace_segments(physpool, physpool_attrs, self.sas)

        image.pop_attrs()

    def layout_cells_post(self, image, machine):
        self.set_free_pools(image, machine)

    def generate_init(self, machine, image):
        """
        Initialize any data structures required by the cell.
        """
        self.env.generate_init(image)

    def register_realm_mapping(self, tag, kern_segment, env_realm, kern_mmu):
        # FIXME: Jira ticket OLD-1790 - jsok.
        assert kern_segment is not None
        assert isinstance(kern_segment, Kernel.Heap.Domain.Segment)

        self.env.add_static_memory_information(tag, kern_segment,
                                               env_realm,
                                               kern_mmu)

    def prepare_address_space(self):
        # Now that we know what virtual address region our cell covers, trim
        # the address space to suit.

        # Collect all of the virtual pools used by memsections and
        # segments in the cell.
        virtpools = set([self.def_virtpool])
        for pd in self.attachments:
            virtpools |= pd.image_realm.get_virtpools()

        # Get the memory ranges held by these pools.
        virt_fulllist = [r for virtpool in virtpools
                         for r in self.sas.get_virtual_pool_by_name(virtpool).get_fulllist()]

        self.env_address_space.set_address_range(virt_fulllist)

    def set_free_pools(self, image, machine):
        ######## Default physical pool ########
        phys_free = \
            self.sas.get_physical_pool_by_name(self.def_physpool).get_freelist()[:]

        if not phys_free:
            raise MergeError, "No free physical memory left."

        # Sort biggest to smallest
        phys_free.sort(key=lambda x: x.base - x.end)

        # Extract the biggest regions and remove them from the lists.
        phys_range = phys_free[0]
        del phys_free[0]

        # Futher work on the physpool is held over until the virtpool is
        # processed.

        ######## Default virtual pool ########
        if not image.is_unified:
            virt_free = \
                self.sas.get_virtual_pool_by_name(self.def_virtpool).get_freelist()[:]
            # Sort biggest to smallest
            virt_free.sort(key=lambda x: x.base - x.end)

            # Extract the biggest regions and remove them from the lists.
            virt_range = virt_free[0]
            del virt_free[0]

            if not Autogen.mmu.allows_runtime_mapping():
                # On compositions like okl4nano, which can't map memory at
                # runtime, the virtpool needs to be mapped by elfweaver.  For
                # efficient mapping, the pool's address range needs to be
                # adjusted.
                #
                # Firstly, the virt and phys pools tend to be large so they
                # should be mapped with as many 1M pages as possible.
                # Rounding both up to the next 1M boundary wastes memory, so
                # instead move the base addresses so that their values mod 1M
                # are the same.  The map_page() implementation can then use 1M
                # pages at the first opportunity.  If the adjusted address
                # doesn't fit in the pool then don't use it because the pool
                # is already so small that it can be mapped
                # efficiently.
                #
                # This allocation policy could be improved:
                # FIXME: Jira ticket OLD-1465 - malcolmp.

                max_page_size = machine.max_page_size()
                if (virt_range.base % max_page_size !=
                    phys_range.base % max_page_size):

                    largest_offset = max(virt_range.base % max_page_size,
                                         phys_range.base % max_page_size)

                    new_virt_base = \
                                  (align_down(virt_range.base, max_page_size) +
                                   largest_offset)
                    new_phys_base = \
                                  (align_down(phys_range.base, max_page_size) +
                                   largest_offset)

                    # If the new addresses fit in the existing range then use
                    # them.
                    if (new_virt_base < virt_range.end and
                        new_phys_base < phys_range.end):
                        virt_range.base = new_virt_base
                        phys_range.base = new_phys_base

                # Secondly, make sure that the virtpool is no larger than the
                # physpool, so that all of the pool is backed.

                if ((virt_range.end - virt_range.base) >
                    (phys_range.end - phys_range.base)):
                    virt_range.end = virt_range.base + (phys_range.end -
                                                        phys_range.base)

            self.virt_attrs.size = virt_range.end - virt_range.base + 1
            self.virt_attrs.set_virt_addr_sas(self.sas, virt_range.base)

            if not Autogen.mmu.allows_runtime_mapping():
                # Map the virtpool using memory from the physpool.
                self.virt_attrs.phys_addr = phys_range.base
                self.attachments[0].image_realm.add_memsection(self.virt_attrs,
                                                               machine)
                sas_attrs = self.attachments[0].image_realm.sas_attrs(self.virt_attrs)
                seg = self.domain.register_segment(sas_attrs)
                self.mmu_context.register_mapping(seg, sas_attrs)

                if self.env_address_space is not None:
                    self.env_address_space.set_static_mmu_virtpool(
                        virt_range.base, virt_range.end)

        # Now create the physpool.
        self.phys_attrs.size = phys_range.end - phys_range.base + 1
        self.phys_attrs.phys_addr = phys_range.base
        self.attachments[0].image_realm.add_memsection(self.phys_attrs, machine)

    def _weaved_env_create_mutex(self, env_name, attributes):
        # Called when a symbol tells us that we need to create a mutex.
        # attributes will be of the form:
        #      [01]{$heldbymain}
        #       ^          ^
        # is_counted      optionally denotes that the thread is held by main at
        #                 startup.
        mutex_attrs = attributes.split('$')
        assert mutex_attrs[0] in ['0', '1']
        is_counted = int(mutex_attrs[0]) == 1

        if len(mutex_attrs) > 1:
            assert len(mutex_attrs) == 2
            assert mutex_attrs[1] == 'heldbymain'

            # Find the main thread of this program. Ensure there is only one.
            holder = None

            for thread in self.mmu_context.threads:
                if thread.is_main_thread:
                    assert holder is None
                    holder = thread
        else:
            holder = None

        if 'mutex' in Autogen.features:
            mutex = self.domain.register_mutex(env_name, is_counted, holder)
            self.env.add_mutex_entry(env_name, mutex)
        elif 'futex' in Autogen.features:
            lock = 0
            if holder is not None:
                lock = 1
            self.env.add_futex_entry(env_name, owner = holder,
                    lock = lock, is_counted = is_counted, count = lock)
        else:
            raise Exception("Mutex creation requested but no mutex module exists.")

    def find_weaved_objects_for_creation(self, image):
        """Find any symbols that indicate we need to create an object
        and create them. The code indicates such objects to us by inserting
        specially named symbols into the symbol table.
        """

        # Build a regexp to match against the symbol name.
        # We are matching symbols such as:
        # okl4test-MAIN-.weaved_env_create$okl4_mutex_t$DEVICE_MUTEX$0$heldbymain
        weaved_env_create_re = re.compile(
            r'^(?P<cellname>[^-]+)-' +
            r'(?P<program>[^-]+)-' +
            r'.weaved_env_create\$' +
            r'(?P<object_type>[^$]+)\$' +
            r'(?P<env_name>[^$]+)\$' +
            r'(?P<attributes>.*)$')

        # Dictionary of object types that we support weave-time creation of.
        creation_dict = {
            'okl4_mutex_t' : self._weaved_env_create_mutex,
        }

        symbols = image.elf.get_symbol_table().get_all_symbols()
        for symbol in symbols:
            # Search for any symbols matching our regexp.
            if symbol is None or symbol.name is None:
                continue
            match = weaved_env_create_re.match(symbol.name)
            if match is None:
                continue

            # Extract the relevant text from the symbol name.
            matchdict = match.groupdict()
            cellname = matchdict['cellname']
            program = matchdict['program']
            object_type = matchdict['object_type']
            env_name = matchdict['env_name']
            attributes = matchdict['attributes']

            # Check that the symbol relates to our cell.
            if cellname != self.name:
                continue

            # Prepend program name to environment key to make it unique within
            # the cell environment.
            env_name = '%s_%s' % (program.upper(), env_name)

            if creation_dict.get(object_type) is not None:
                creation_dict[object_type](env_name, attributes)
            else:
                raise Exception("Weave-time creation of %s is not supported" % (
                        object_type))

Cell.register()
