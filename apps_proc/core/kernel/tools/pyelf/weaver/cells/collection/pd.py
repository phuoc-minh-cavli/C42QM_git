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
XML definitions and collection functions for handling PD's, Programs,
Extensions and Protected Libraries.
"""

import os

from elf.core import UnpreparedElfFile
from elf.constants import STT_FUNC, STB_GLOBAL, STV_DEFAULT

from weaver import MergeError
from weaver.memobjs_xml import Stack_el, Memsection_el, Stack_el
from weaver.memobjs_xml import collect_memobj_attrs
from weaver.ezxml import ParsedElement, Element, str_attr, bool_attr, \
        long_attr, size_attr, update_element
from weaver.segments_xml import Segment_el, Patch_el, make_pager_attr, \
        collect_elf_segments, collect_patches, generate_symbol_prefix
from weaver.cap import SegmentRights, PhysAddrRights, PlatformControlRights, \
        Attachment, MMUContextRights, ProcessorControlRights, \
        ProfileControlRights, IntlockControlRights
from weaver.cells.collection.thread import Thread_el, collect_thread, \
     DEFAULT_STACK_SIZE
from weaver.cells.collection.irq import IRQ_el, collect_irqs
from weaver.cells.collection.mutex import Mutex_el, collect_mutexes
from weaver.cells.collection.memsection import collect_memsections
from weaver.cells.collection.device import UseDevice_el, collect_use_devices
from weaver.cells.collection.physmem_item import PhysMemItem_el, collect_physmem_items
from weaver.cells.collection.attachment import Attachment_el, \
        collect_attachments, AttachmentContainer
from weaver.cells.collection.message_queue import MessageQueue_el, \
        collect_message_queues
from weaver.cells.collection.tzmonitor import  TZMonitor_el, \
        collect_tzmonitor_services
from weaver.cells.buffer_area import BufferArea_el, collect_buffer_area
from weaver.cells.collection.shared_buffer import SharedBuffer_el, SharedBuffers_el, \
        collect_shared_buffers, collect_shared_buffers_array

from weaver.util import check_file_exists

from pyokl4.autogen import Autogen

# If the virq module is not present, treat as nops.
try:
    from weaver.cells.collection.virq import VIRQ_el, collect_virqs, \
         VInterruptLine_el, VInterruptLines_el, \
         collect_virtual_interrupt_lines, \
         collect_virtual_interrupt_lines_array
except:
    VIRQ_el = Element('virq')
    VInterruptLine_el = Element('virtual_interrupt_line')
    VInterruptLines_el = Element('virtual_interrupt_lines')
    collect_virtual_interrupt_lines = lambda *args: 0
    collect_virtual_interrupt_lines_array = lambda *args: 0
    collect_virqs = lambda *args: 0

# If the vserver module is not present, treat as nops.
try:
    from weaver.cells.collection.vserver import VServer_el, collect_vservers
except:
    VServer_el = Element('vserver')
    collect_vservers = lambda *args: []

# If the channel module is not present, treat as nops.
try:
    from weaver.cells.collection.channel import Channel_el, \
         collect_channels
except:
    Channel_el = Element('channel')
    collect_channels = lambda *args: 0

# Setting the default mode to ARM. "entry_mode_symbol" is an optional
# EntryPoint attribute and if not specified we use the default as
# ARM mode

DEFAULT_MODE = 0x0

Zone_el = Element('zone', Segment_el, Memsection_el,
                  name = (str_attr, 'required'),
                  physpool = (str_attr, 'optional'),
                  virtpool = (str_attr, 'optional'),
                  num_attached = (long_attr, 'optional'),
                  base = (long_attr, 'optional'),
                  size = (size_attr, 'optional'))

PD_el = Element('pd', Segment_el, Patch_el, Thread_el, Attachment_el,
                Memsection_el, Mutex_el, IRQ_el, VIRQ_el, VInterruptLine_el,
                UseDevice_el, TZMonitor_el, VInterruptLines_el,
                MessageQueue_el, BufferArea_el, PhysMemItem_el,
                name = (str_attr, 'required'),
                file = (str_attr, 'optional'),
                physpool = (str_attr, 'optional'),
                virtpool = (str_attr, 'optional'),
                direct = (bool_attr, 'optional'),
                priority = (long_attr, 'optional'),
                timeslice = (long_attr, 'optional'),
                affinity = (long_attr, 'optional'),
                pager = (str_attr, 'optional'),
                caps = (long_attr, 'optional'),
                threads = (long_attr, 'optional'),
                platform_control = (bool_attr, 'optional'),
                num_attachments = (long_attr, 'optional'))

# A program is a PD but with a required file element and implicit Thread tags
# and attributes
Program_el = update_element(PD_el,
                            Stack_el, Channel_el, SharedBuffer_el,
                            SharedBuffers_el, VServer_el,
                            file = (str_attr, 'required'),
                            start = (str_attr, 'optional'),
                            init = (str_attr, 'optional'),
                            manager = (bool_attr, 'optional'))
Program_el.set_name('program')

# It would be nice to bracket all this code in 'if 'protected_library in
# Autogen.features' however none of the autogen gets populated until
# import_autogen_modules() is run (in the kernel collect_xml, since we need
# to know the product/arch/cpu to be able to set it).  So we cannot determine
# when loading all the .py files what is included and since all the XML code
# needs to be setup before we hit collect to parse the XML (obviously) we
# cannot selectively add it.

EntryPoint_el = Element("entry_point",
                        name = (str_attr, "required"),
                        entry_symbol = (str_attr, "required"),
                        entry_mode_symbol = (long_attr, 'optional')
                        )

# An extension is a PD but without Threads
Extension_el = update_element(PD_el, EntryPoint_el)
Extension_el.set_name('extension')
Extension_el.remove_element(Thread_el)
Extension_el.remove_attribute('threads')


# A protected library is an Extension with a required file attribute
ProtLib_el = update_element(Extension_el,
                            file = (str_attr, 'required'))
ProtLib_el.set_name('protected_library')

DEFAULT_PD_THREADS = 16

_NEXT_PD_ID = 0

def get_pd_id():
    """
    Return the next free global PD identifier.
    """
    global _NEXT_PD_ID

    pd_id = _NEXT_PD_ID
    _NEXT_PD_ID += 1

    return pd_id

class PD(object):
    """Simple container class to handle attachment objects."""

    def __init__(self, name, domain, mmu_context, attachments,
                 env_realm, image_realm):
        self.name = name
        self.domain = domain
        self.mmu_context = mmu_context
        self.attachments = attachments
        self.env_object = None
        self.env_realm = env_realm
        self.image_realm = image_realm

    def resolve(self):
        def sort_extensions(x, y):
            if isinstance(x, ExtensionAttachment) and \
                    isinstance(y, ExtensionAttachment):
                return 0
            if isinstance(x, ExtensionAttachment):
                return 1
            if isinstance(y, ExtensionAttachment):
                return -1
            return 0

        self.attachments = sorted(self.attachments, sort_extensions,
                key = lambda x: x.dest_cap.get_cap_value())

        for attachment in self.attachments:
            attach_container = attachment.dest_cap.get_cap_value()
            attach_container.resolve(domain = self.domain,
                    mmu_context = self.mmu_context, name = self.name)

class ExtensionAttachment(AttachmentContainer):
    """Container for extension object attachments"""

    switch_table = property()
    def __init__(self, env_object, name, env, mmu_context, kern_switch_table):
        """Initialise the object."""
        AttachmentContainer.__init__(self, env_object)
        self.name = name
        self.env = env
        self.mmu_context = mmu_context
        self.kern_switch_table = kern_switch_table

    def resolve(self, **kwargs):
        """
        Clone any mappings in the pd to this extension, copy the switch table
        cap into the domain and register an evnironment entry for the token.
        """
        pd_mmu_context = kwargs['mmu_context']
        self.mmu_context.clone_mappings(pd_mmu_context)

        pd_domain = kwargs['domain']
        pd_domain.register_cap(self.kern_switch_table, Autogen.rights.RIGHTS_MASTER)

        pd_name = kwargs['name']
        self.env.add_pd_ext_token_entry(pd_name + '_' + self.name + '_TOKEN',
                self.kern_switch_table)

    def set_env_attr(self, object):
        """Set any required attributes on the environment entry."""
        if self.kern_switch_table.okl4_object is not None:
            object.token.kswitch_kcap = self.kern_switch_table.okl4_object.cap_id
        object.ext = Autogen.types.OKL4_void_ptr(address = \
                self.env_object.child._address)

    def get_list_entry(self):
        """Return the list object from the environment entry."""
        return self.env_object.child.pd_list

def collect_pd(cell, pd_el, namespace, image, machine, sas, heap, env,
               section_prefix, env_address_space,
               pd_pool_attr = None, pd_pool_seg = None, pd_pool = None,
               manager_domain = None, kernel = None):
    """
    Collection for PDs, programs, protected libraries, and extensions.
    """
    manager = getattr(pd_el, 'manager', False)

    if manager:
        old_pd_name = pd_el.name
        pd_el.name = 'MAIN'
        pd_ns = namespace
    else:
        pd_ns = namespace.add_namespace(pd_el.name)

    symbol_prefix = generate_symbol_prefix(section_prefix, pd_el.name)

    image.push_attrs(getattr(pd_el, 'virtpool', None),
                     getattr(pd_el, 'physpool', None),
                     make_pager_attr(getattr(pd_el, 'pager', None)),
                     getattr(pd_el, 'direct', None),
                     getattr(pd_el, 'priority', None),
                     getattr(pd_el, 'timeslice', None),
                     getattr(pd_el, 'affinity', None))

    intlock_ctrl_available = pd_el.find_children('intlock_control')

    if intlock_ctrl_available != []:
        intlock_cap = True
    else:
        intlock_cap = False

    # All tags imply a new kdomain and kmmu.
    pd_kern_domain = heap.register_domain(namespace,
                                          getattr(pd_el, 'caps', None),
                                          getattr(pd_el, 'threads', None),
                                          getattr(pd_el, 'max_priority', None),
                                          getattr(pd_el, 'max_reply_caps', None),
                                          manager, manager_domain)

    if manager:
        manager_domain = pd_kern_domain

    pd_kern_mmu = manager_domain.register_mmu_context(
        pd_ns, pd_el.name,
        getattr(pd_el, 'max_priority', None), None, None, intlock_cap)

    MMUContextRights.add_standard_rights(pd_kern_mmu,
            pd_ns.add_namespace('mmu'))

    if pd_el.tag == 'program':
        env.add_program(symbol_prefix, pd_kern_domain, pd_kern_mmu)

    if env_address_space is not None:
        env_realm = env.add_realm_entry(pd_el.name + "_REALM",
                                        env_address_space)
    else:
        env_realm = None

    image_realm = image.add_realm(sas,
                                  sas.new_realm(section_prefix + "_" + pd_el.name,
                                                machine),
                                  env_realm)
    # FIXME: Jira ticket OLD-1440 - jamesp.
    # We directly register the virtmem pool with each pd type object
    # rather than using attchments.  See awiggins, jamesp, nhannah and the
    # bug for further information.
    if manager:
        # Generate the main pd_pool to be shared by all PDs and extensions (for
        # the moment)

        pd_pool_el = ParsedElement('pd_pool')
        pd_pool_el.name = 'pd_pool'
        pd_pool_attr = collect_memobj_attrs(pd_pool_el, pd_ns, image,
                                            machine)
        pd_pool_attr.size = DEFAULT_PD_THREADS
        pd_pool_attr.size *= DEFAULT_STACK_SIZE
        pd_pool_attr.align = machine.min_page_size()

        pd_pool_ms = image_realm.add_memsection(pd_pool_attr, machine)



        pd_pool_seg = pd_kern_domain.register_segment(image_realm.sas_attrs(pd_pool_ms.attrs))
        pd_pool = env.add_virtmem_pool_entry('MAIN_PD_VIRTPOOL',
                                             pd_pool_seg.attrs)
        cell.register_realm_mapping('MAIN_PD_VIRTPOOL_MEMSECTION', pd_pool_seg,
                                    env_realm, pd_kern_mmu)
    pd_kern_mmu.register_mapping(pd_pool_seg, image_realm.sas_attrs(pd_pool_attr))

    env_kdomain = None
    env_kmmu_context = None

    if Autogen.mmu.has_multiple_kmmu_support():
        env_kdomain = env.add_kdomain_entry(pd_el.name + '_KDOMAIN',
                                            pd_kern_domain)
        env_kmmu_context = env.add_kmmu_entry(pd_el.name + '_KMMU',
                                              pd_kern_mmu)

    # If a file attribute has been given read it in, setup segments and so
    # forth.
    file = getattr(pd_el, 'file', None)

    segs = []
    if file is not None:
        filename = check_file_exists(file, pd_el._path)
        elf = UnpreparedElfFile(filename = filename)

        segment_els = pd_el.find_children('segment')
        segs = collect_elf_segments(elf, segment_els,
                                    section_prefix, symbol_prefix, [],
                                    image_realm, pd_ns, image, machine)

        elffile_segs = []

        for seg in segs:
            seg_ns = seg.attrs.ns_node
            sas_attrs = image_realm.sas_attrs(seg.attrs)
            seg_kern_seg = pd_kern_domain.register_segment(sas_attrs)
            pd_kern_mmu.register_mapping(seg_kern_seg, sas_attrs)
            SegmentRights.add_standard_rights(seg_kern_seg, seg_ns)
            PhysAddrRights.add_standard_rights(seg_kern_seg, seg_ns)

            if manager:
                name = 'MEMSEC_' + \
                    (seg.attrs.abs_name().replace('/', '_').lstrip('_'))
            else:
                name = pd_el.name + '_MEMSEC_' + \
                    (seg.attrs.abs_name().replace('/', '_').lstrip('_'))

            cell.register_realm_mapping(name, seg_kern_seg, env_realm,
                                        pd_kern_mmu)

            elffile_segs.append(name)

        env.add_elffile_entry(pd_el.name + "_ELFFILE", elffile_segs)

        collect_patches(image.elf, symbol_prefix, pd_el.find_children('patch'),
                        filename, image)

        image.constituent_elfs.append((filename, section_prefix, None))


    kern_switch_table = None
    pd_env_entry_name = 'PD'

    # Setup extensions or protected libraries
    if pd_el.tag in ('extension', 'protected_library'):
        if not ('protected_library' in Autogen.features):
            raise MergeError("Found extension XML tag but kernel does not support extensions!")

        # In the case of protected libraries we grab entry symbols from the elf
        # file.
        if pd_el.tag == 'protected_library':
            pseudo_entry_points = []

            for sym in elf.get_symbol_table().symbols:
                if sym.get_type() == STT_FUNC and \
                        sym.get_bind() == STB_GLOBAL and \
                        sym.get_visibility() == STV_DEFAULT:
                    # Create a pseudo entry point element for the symbol.
                    entry_point = ParsedElement('entry_point')
                    entry_point.name = sym.name
                    entry_point.entry_symbol = sym.name
                    pseudo_entry_points.append(entry_point)

            pd_el.add_children(pseudo_entry_points)

        # Get entry points for the extension, there must be at least one.
        entry_points = pd_el.find_children('entry_point')
        if entry_points == []:
            raise MergeError, \
                'Extension/Protected library %s has no entry points.' % \
                    pd_el.name
        entry_symbols = \
            [symbol_prefix + entry_point.entry_symbol
                    for entry_point in entry_points]

        for entry_point in entry_points:
            val = getattr(EntryPoint_el, 'entry_symbol', None)
            if val is None:
                # If not specified, set default as ARM MODE
                entry_point.entry_mode_symbol = DEFAULT_MODE

        entry_mode_symbols = \
                [symbol_prefix + entry_point.entry_mode_symbol
                        for entry_point in entry_points]

        # Setup kernel switch table object.
        kern_switch_table = manager_domain.register_switch_table(pd_kern_domain,
                                                                 pd_kern_mmu, entry_symbols,
                                                                 entry_mode_symbols,
                                                                 symbol_prefix + "kswitch_return_stub")
        # Add entry tokens to the environment.
        for entry_index, entry_point in enumerate(entry_points):
            env.add_extension_entry_token_entry(pd_el.name + \
                 "_" + entry_point.name + '_ENTRY_TOKEN', entry_index)

        pd_env_entry_name = 'EXTENSION'

    plat_ctls = pd_el.find_children('platform_control')

    for p in plat_ctls:
        pc = pd_kern_domain.register_platform_control(p.name)
        ns_platctrl = namespace.add_namespace(p.name, "platform control")

        if manager:
            env_name = ''
            cap_name = ''
        else:
            env_name = pd_el.name + '_'
            cap_name = pd_el.name + '/'

        env.add_cap_entry(env_name + p.name.upper(),
                          cap_name + p.name, None)
        PlatformControlRights.add_standard_rights(pc, ns_platctrl)

    proc_ctls = pd_el.find_children('processor_control')

    for p in proc_ctls:
        pc = pd_kern_domain.register_processor_control(p.name)
        ns_procctrl = namespace.add_namespace(p.name, "processor control")

        if manager:
            env_name = ''
            cap_name = ''
        else:
            env_name = pd_el.name + '_'
            cap_name = pd_el.name + '/'

        env.add_cap_entry(env_name + p.name.upper(),
                          cap_name + p.name, None)
        ProcessorControlRights.add_standard_rights(pc, ns_procctrl)

    prof_ctls = pd_el.find_children('profile_control')
    for p in prof_ctls:
        pc = pd_kern_domain.register_profile_control(p.name)
        ns_profctrl = namespace.add_namespace(p.name)
        if manager:
            env_name = ''
            cap_name = ''
        else:
            env_name = pd_el.name + '_'
            cap_name = pd_el.name + '/'

        env.add_cap_entry(env_name + p.name.upper(),
                          cap_name + p.name, None)
        ProfileControlRights.add_standard_rights(pc, ns_profctrl)

    intlock_ctls = pd_el.find_children('intlock_control')
    for p in intlock_ctls:
        pc = pd_kern_domain.register_intlock_control(p.name)
        ns_intlockctrl = namespace.add_namespace(p.name)
        if manager:
            env_name = ''
            cap_name = ''
        else:
            env_name = pd_el.name + '_'
            cap_name = pd_el.name + '/'

        env.add_cap_entry(env_name + p.name.upper(),
                          cap_name + p.name, None)
        IntlockControlRights.add_standard_rights(pc, ns_intlockctrl)

    # Process the implicit thread (if any) and any thread tags
    kern_threads = []
    if pd_el.tag == 'program':
        start = getattr(pd_el, 'start', 'main')
        main_thread, main_kthread = \
                     collect_thread(cell, elf, pd_el, image_realm, pd_ns,
                                    image, machine,
                                    pd_kern_mmu,
                                    manager_domain, env, segs,
                                    env_kdomain, env_kmmu_context,
                                    pd_el.name, manager, start, True)
        kern_threads.append(main_thread)

        # Linux does its own thing so we don't weave any locks and set our
        # start point back at the specified file entry point
        if getattr(pd_el, 'linux', False):
            main_thread.start = elf.entry_point

        if env_kmmu_context:
            env_kmmu_context.env_exception_handler = main_kthread

    for thread_el in pd_el.find_children('thread'):
        kern_thread, _ = collect_thread(cell, elf, thread_el, image_realm,
                                        pd_ns, image, machine,
                                        pd_kern_mmu,
                                        manager_domain, env, segs,
                                        env_kdomain, env_kmmu_context,
                                        pd_el.name, manager)
        kern_threads.append(kern_thread)

    collect_use_devices(cell, pd_el, image_realm,
                        pd_kern_mmu, pd_kern_domain,
                        manager_domain, pd_ns, image,
                        machine, env)

    # Handle other domain specific tags
    collect_mutexes(pd_el, pd_ns, pd_kern_domain, kern_threads, env)
    collect_irqs(pd_el, pd_kern_domain)
    collect_virqs(pd_el, pd_ns, pd_kern_domain, env)
    collect_virtual_interrupt_lines(pd_el, pd_ns, pd_kern_domain, env)
    collect_virtual_interrupt_lines_array(pd_el, pd_ns, pd_kern_domain, env)
    collect_message_queues(pd_el, pd_kern_domain, env, pd_ns, env_kmmu_context)

    if pd_el.tag is 'program':
        collect_channels(pd_el, image_realm, pd_kern_mmu, manager_domain,
                pd_ns, image, machine, sas, pd_el.name, env, kernel)
        cell.vservers = collect_vservers(pd_el, image_realm, pd_kern_mmu,
                                         manager_domain, pd_ns, image,
                                         machine, sas, env, kernel)

    collect_buffer_area(pd_el, pd_kern_domain, env)
    collect_tzmonitor_services(pd_el, pd_ns, pd_kern_domain, machine, env)
    collect_physmem_items( pd_el, image, pd_ns, manager_domain, machine, env_realm, pd_kern_mmu, env, image_realm, sas);

    if pd_el.tag is 'program':
        collect_shared_buffers(pd_el, image_realm, pd_kern_mmu, manager_domain,
                pd_ns, image, machine, sas, pd_el.name, env)
        collect_shared_buffers_array(pd_el, image_realm, pd_kern_mmu, manager_domain,
                pd_ns, image, machine, sas, pd_el.name, env)


    attach_els = []

    # Collect any memsection objects within the pd.
    memsec_attach_els = \
        collect_memsections(pd_el, image_realm,
            pd_kern_mmu, manager_domain, pd_ns, image, machine,
            pd_el.name, env, env_realm)
    attach_els.extend(memsec_attach_els)

    pd_el.add_children(attach_els)
    attachments = collect_attachments(pd_el, env)
    pd_container = PD(pd_el.name, pd_kern_domain, pd_kern_mmu,
                      attachments, env_realm, image_realm)

    env_pd = env.add_pd_entry(pd_el.name + '_' + pd_env_entry_name,
                              attachments,
                              getattr(pd_el,   'num_attachments', None),
                              pd_pool, env_kdomain, env_kmmu_context,
                              kern_switch_table, env_address_space,
                              get_pd_id())

    pd_container.env_object = env_pd

    # If realms are active then tie together the releam environment
    # data structures.
    if env_address_space is not None:
        env_address_space.attach_pd(env_pd)
        env_pd.set_default_realm(env_realm)

    if 'protected_library' in Autogen.features \
            and pd_el.tag in ('extension', 'protected_library'):
        Attachment.add(ExtensionAttachment(env_pd, pd_el.name, env,
                                           pd_kern_mmu, kern_switch_table),
                       pd_ns)

    image.pop_attrs()

    if manager:
        # If we are the manager then we need to return extra information for
        # the cell collection to use
        pd_el.name = old_pd_name
        return (pd_kern_domain, pd_kern_mmu, kern_threads,
                pd_pool_attr, pd_pool_seg, pd_pool, [pd_container])
    else:
        return (None, None, kern_threads, None, None, None, [pd_container])
