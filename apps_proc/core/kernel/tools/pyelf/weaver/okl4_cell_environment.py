##############################################################################
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

"""Environment descriptor for a Cell."""

import re

from elf.ByteArray import ByteArray
from elf.constants import PF_R, PF_W
from weaver.segments_xml import generate_section_prefix
from weaver.okl4_cell_environment_elf import CellEnvSection, CellEnvHeap, \
        CellEnvVirtmemPool, CellEnvArgList, CellEnvInterruptDeviceMap, \
        CellEnvKthread, CellEnvKdomain, CellEnvKmmu, CellEnvKswitch, \
        CellEnvCapEntry, CellEnvPhysmemSegpool, \
        CellEnvMachineInfo, CellEnvMemsec, CellEnvMemsecContiguous, \
        CellEnvRealm, CellEnvAddressSpace, CellEnvSegmentTable, \
        CellEnvInterruptList, CellEnvPd, CellEnvMutex, CellEnvElfFile, \
        CellEnvKCap, CellEnvVirtmemItem, CellEnvExtensionEntryToken, \
        CellEnvFutex, CellEnvWord, CellEnvPdExtToken, \
        CellEnvMessageQueue, CellEnvPOSIXmqList, \
        CellEnvValueEntry, CellEnvTZMonitor, CellEnvATagTable, \
        CellEnvSharedBuffer, CellEnvSharedBufferCap, CellEnvSharedBuffers, \
        CellEnvValueInCapEntry, CellEnvPhysmemItem, CellEnvVirq, \
        CellEnvVServerEntry, CellEnvVInterruptLines, CellEnvVClientEntry, \
        CellEnvValuePathEntry, CellEnvVInterruptLine, CellEnvMemsecPaged
from weaver import MergeError
from elf.util import align_up
from pyokl4.autogen import Autogen
from weaver.kernel import Kernel
from weaver.physreloc import PhysRelocWriter

class KernelCellEnvironment:
    """
    Environment descriptor for a Cell.
    """
    def __init__(self, name, namespace, machine, image, elf,
                 mappings):
        self.namespace = namespace
        self.name = name
        self.min_page_size = machine.min_page_size()
        self.machine = machine
        self.image = image
        self.elf = elf
        self.entries = {}
        self.path_entries = []
        self.memsect = None
        self.mappings = mappings
        self.pd_list = []
        self.programs = []
        self.env_patches = []
        self.sas = None
        mqlist = CellEnvPOSIXmqList()
        self._add_entry("posix_mq_list", mqlist)

        self._add_entry("BUFFER_AREA_SIZE", CellEnvWord(value=0))
        self._add_entry("BUFFER_AREA_MAX_BUFFER_SIZE", CellEnvWord(value=0))

    def _add_entry(self, tag, entry, no_dup = True):
        """Add an entry to the environment."""
        # Raise an error if the entry name is already used.
        if isinstance(tag, str):
            tag = tag.upper()

        # Support for anonymous objects in the environment. Their name is their
        # tag.
        if tag is None:
            tag = entry

        if no_dup and tag in self.entries:
            raise MergeError, \
                  'Key "%s" is already present in environment for %s.' % \
                  (str(tag), self.namespace.abs_name())

        self.entries[tag] = entry

    def get_entry(self, tag):
        if isinstance(tag, str):
            tag = tag.upper()
        try:
            return self.entries[tag]
        except:
            return None

    def add_program(self, symbol_prefix, kern_domain, kern_mmu):
        """Add a program to be patched."""
        self.programs.append((symbol_prefix, kern_domain, kern_mmu))

    def add_heap(self, heap):
        """Add a heap to the environmnet."""
        if hasattr(Autogen.types, "OKL4_virtmem_item"):
            self._add_entry("MAIN_HEAP", CellEnvHeap(heap))

    def add_arg_list(self, args):
        """Add the arguments structure to the environment."""
        arg_list = CellEnvArgList(len(args), args)
        self._add_entry("MAIN_ARGS", arg_list)

    def add_num_vuas(self, num_vuas):
        """ Add the number of vuas to the environment."""
        num_vuas = CellEnvValueEntry(num_vuas)
        self._add_entry("VMMU_NUM_VUAS", num_vuas)

    def add_virtmem_pool_entry(self, tag, attrs):
        """Add virtmem pool details to the environment."""
        if 'memory_pools' in Autogen.features:
            pool = CellEnvVirtmemPool(attrs)
            self._add_entry(tag, pool)

            return pool
        else:
            return None

    def add_kthread_entry(self, tag, kern_thread, env_kdomain=None,
                          env_kmmu=None):
        """ Add kthread details to the environment """

        # Weave out a cap for this entry.
        self._add_entry(tag + "_KCAP", CellEnvKCap(kern_thread))

        # Write out a okl4_kthread object for this entry.
        if hasattr(Autogen.types, 'OKL4_kthread'):
            env_kthread = CellEnvKthread(kern_thread, env_kdomain, env_kmmu)
            self._add_entry(tag, env_kthread)

            return env_kthread
        else:
            return None

    def add_kdomain_entry(self, tag, kern_domain, env_parent=None):
        """ Add kdomain details to the environment """
        # TODO: We currently don't have subdomains, therefore
        # parent will always be None. -jsok
        kdomain = CellEnvKdomain(kern_domain, env_parent)
        self._add_entry(tag, kdomain)

        return kdomain

    def add_kmmu_entry(self, tag, mmu):
        """ Add kmmu details to the environment """
        env_kmmu = CellEnvKmmu(mmu)
        self._add_entry(tag, env_kmmu)

        return env_kmmu

    def add_kswitch_entry(self, tag, kern_switch_table, env_kdomain, env_kmmu):
        """ Add kswitch details to the environment """
        kswitch = CellEnvKswitch(kern_switch_table, env_kdomain, env_kmmu)
        self._add_entry(tag, kswitch)

        return kswitch

    def add_message_queue_entry(self, tag, mmu):
        """ Add message queue details to the environment """
        # FIXME: Jira ticket OLD-1461 - jamesp.
        # This is somewhat hacky, the environment actually doesn't need to
        # know anything about the mmu but the kernel does for running the
        # message queue create call.  However caps are handled here in the
        # snap() call, additionally we don't want to add a normal cap as there
        # is no reason to put the mmu cap into the environment.  A better
        # way would be nicer but this should work.
        env_msgq = CellEnvMessageQueue()
        env_msgq.mmu_cap = CellEnvCapEntry(mmu, None)

        self._add_entry(tag, env_msgq)
        self.path_entries.append(env_msgq.mmu_cap)

        return env_msgq

    def add_channel_entry(self, tag, kern_chn, msg_size):
        """ Add channel details to the environment """
        self._add_entry(tag + "_KCAP", CellEnvKCap(kern_chn))
        self._add_entry(tag + "_MSG_SIZE", CellEnvWord(msg_size))

    def add_virq_entry(self, name, irq_obj):
        """ Add channel details to the environment """
        self._add_entry(name, CellEnvVirq(irq_obj))

    def add_vserver_entry(self, tag, kern_channels):
        """Add vserver details to the environment"""
        self._add_entry(tag + '_VSERVER', CellEnvVServerEntry(kern_channels))

    def add_vclient_entry(self, tag, obj):
        """Add vclient details to the environment"""
        self._add_entry(tag + '_CLIENT', CellEnvVClientEntry(obj))

    def add_buffer_area(self, size, max_buffer_size):
        self._add_entry("BUFFER_AREA_SIZE", CellEnvWord(value=int(size)),
                        no_dup=False)
        self._add_entry("BUFFER_AREA_MAX_BUFFER_SIZE",
                        CellEnvWord(value=int(max_buffer_size)), no_dup=False)

    def add_posix_mq_entry(self, path, right_path):
        mqlist = self.get_entry("posix_mq_list")

        mq = CellEnvCapEntry(right_path, None)
        mqlist.insert(path, mq)
        self.path_entries.append(mq)

    def add_tzmonitor_entry(self, tag, kern_tzmonitor):
        """Add tzmonitor object details to the environment """
        env_tzmonitor = CellEnvTZMonitor(kern_tzmonitor)
        self._add_entry(tag, env_tzmonitor)

    def add_vinterrupt_line_entry(self, tag, kern_vinterrupt_line):
        """Add virtual interrupt line object details to the environment"""
        self._add_entry(tag, CellEnvVInterruptLine(kern_vinterrupt_line))

    def add_vinterrupt_lines_entry(self, tag, kern_vinterrupt_lines):
        """
        Add the details of a collection of virtual interrupt lines to the
        environment.
        """
        self._add_entry(tag, CellEnvVInterruptLines(kern_vinterrupt_lines))

    def add_extension_entry_token_entry(self, tag, entry_index):
        """ Add extension entry token details to the environment """
        entry_token = CellEnvExtensionEntryToken(entry_index)
        self._add_entry(tag, entry_token)

        return entry_token

    def add_mutex_entry(self, tag, kern_mutex, is_counted=False,
            is_hybrid=False):
        """ Add mutex details to the environment. """
        env_mutex = CellEnvMutex(kern_mutex, is_counted, is_hybrid)
        self._add_entry(tag, env_mutex)

        return env_mutex

    def add_futex_entry(self, tag, **kwargs):
        """ Add futex details to the environment. """
        env_futex = CellEnvFutex(**kwargs)
        self._add_entry(tag, env_futex)

        return env_futex

    def add_env_interrupt_list(self, tag, kern_irqs):
        """Add an IRQ entry to the environment."""
        if kern_irqs is not None:
            env_table = CellEnvInterruptList(kern_irqs)
            self._add_entry(tag, env_table)

            return env_table
        else:
            return None

    def add_physmem_segpool_entry(self, tag, kern_segment, kern_mmu):
        """Add physmem segpool details to the environment."""
        if 'memory_pools' in Autogen.features:
            env_segpool = CellEnvPhysmemSegpool(self.machine, kern_segment,
                                                kern_mmu)
            self._add_entry(tag, env_segpool)

            return env_segpool
        else:
            return None

    def add_segment_table(self, tag, heap):
        if 'segment' not in Autogen.features:
            return None

        env_table = CellEnvSegmentTable(heap)
        self._add_entry(tag, env_table)
        return env_table

    def add_static_memory_information(self, tag, kern_seg, env_realm,
                                      kern_mmu):
        """
        Add information about a statically mapped segment of memory into
        the enviornment.

        If realms are available, we weave out a contiguous memsection with the
        given tag.

        Additionally, we also weave out a virtmem item with the name
        tag + "_RANGE", allowing systems without realms to also access this
        information.

        Return the memsection entry and the virtmem item in a tuple.
        """

        env_ms = None
        # If the library has memsection support, weave out a memsection.
        if env_realm and 'realms' in Autogen.features:
            if kern_seg.attrs.memsec_type == "paged":
                env_ms = CellEnvMemsecPaged(self.image, self.machine,
                        self.name, kern_seg, kern_mmu)
            else:
                env_ms = CellEnvMemsecContiguous(self.image, self.machine,
                                                 self.name, kern_seg,
                                                 kern_mmu)
            self._add_entry(tag, env_ms)
            env_realm.add_env_memsection(env_ms)

        # Additionally weave out a simple range item. Minimal systems will
        # need to use this, because they will not have memsecs in their
        # libokl4.
        range = None
        if tag is not None and hasattr(Autogen.types, "OKL4_virtmem_item"):
            range = CellEnvVirtmemItem(kern_seg)
            self._add_entry(tag + "_RANGE", range)

        return (env_ms, range)

    def add_realm_entry(self, tag, env_address_space):
        """Add a realm to the environment."""
        realm_obj = CellEnvRealm(self.machine, env_address_space)
        self._add_entry(tag, realm_obj)

        return realm_obj

    def add_address_space_entry(self, tag):
        """Add an address_space to the environment."""
        addrspace_obj = CellEnvAddressSpace()
        self._add_entry(tag, addrspace_obj)

        return addrspace_obj

    def add_pd_entry(self, tag, attachments, num_attachments, ms,
            env_kdomain, env_kmmu, kern_switch_table, env_address_space, id_):
        """ Add protection domain details to the environment """
        if 'PD' in Autogen.features:
            env_pd = CellEnvPd(attachments, num_attachments, ms, env_kdomain,
                               env_kmmu, kern_switch_table,
                               env_address_space, id_)
            self._add_entry(tag, env_pd)

            return env_pd
        else:
            return None

    def add_pd_ext_token_entry(self, tag, switch_table):
        token = CellEnvPdExtToken(switch_table)
        self._add_entry(tag, token)

        return token

    def add_elffile_entry(self, tag, elf_segment_names):
        """Add an elffile entry to the environment."""
        if 'ElfFile' in Autogen.features:
            elffile = CellEnvElfFile(elf_segment_names, self)
            self._add_entry(tag, elffile)

            return elffile
        else:
            return None

    def add_shared_buffer_entry(self, tag, attrs):
        """Add a shared buffer to the environment."""
        shared_buffer = CellEnvSharedBuffer(attrs)
        self._add_entry(tag, shared_buffer)

        return shared_buffer

    def add_shared_buffer_cap_entry(self, tag, segment):
        """Add a shared buffer to the environment."""
        shared_buffer_cap = CellEnvSharedBufferCap(segment)
        self._add_entry(tag, shared_buffer_cap)

        return shared_buffer_cap

    def add_shared_buffers_entry(self, tag, attrs, segments):
        """Add shared buffers array to the environment."""
        self._add_entry(tag, CellEnvSharedBuffers(attrs, segments))

    # FIXME: Jira ticket OLD-1449 - nelson.
    def add_kernel_info_entry(self, base, end, kernel):
        """Add kernel info details to the environment."""
        pass

    def add_elf_segment_entry(self, name, seg):
        """Add info about an elf segment to the environment."""
        pass

    def add_elf_info_entry(self, name, file_type, thread):
        """Add info about an elf file to the environment."""
        pass

    def add_cap_entry(self, key, right_path, attach):
        """Add a cap to the environment."""
        cap_entry = CellEnvCapEntry(right_path, attach)
        self._add_entry(key, cap_entry)
        self.path_entries.append(cap_entry)

    def add_no_env_cap_entry(self, path):
        """
        Add a cap that won't actually be written out in the environment but
        allows us to reuse snap() to actually add and resolve it for other
        purposes.
        """
        entry = CellEnvCapEntry(path, None)
        self.path_entries.append(entry);

        return entry

    def add_attachment(self, name):
        """
        Add an attachment, this does not add any entry to the environemnt it
        simply allows us to reuse the cap infrastrucutre.
        """
        return self.add_no_env_cap_entry(name)

    def add_interrupt_device_map(self, name, irq_nums):
        """Add a device's list of IRQs to the environment."""
        if 'interrupt' in Autogen.features:
            irq_list = CellEnvInterruptDeviceMap(len(irq_nums), irq_nums)
            name = name + "_interrupt_map"
            self._add_entry(name.upper(), irq_list)

    def add_machine_info_entry(self, key, num_cpus = 1):
        """Add a machine info struct to the environment"""
        machine_info = CellEnvMachineInfo(num_cpus)
        self._add_entry(key, machine_info)

        return machine_info

    def add_physmem_item_entry(self, attrs, kern_mmu, kern_seg):
        item = CellEnvPhysmemItem(attrs, kern_mmu, kern_seg)
        self._add_entry(attrs.ns_node.name, item)

    def add_value_entry(self, key, value):
        """Add an integer constant to the environment."""
        value_entry = CellEnvValueEntry(value)
        self._add_entry(key, value_entry)

        return value_entry

    def add_value_path_entry(self, key, path):
        """Add an integer to the environment."""
        value_path_entry = CellEnvValuePathEntry(path)
        self._add_entry(key, value_path_entry)
        self.path_entries.append(value_path_entry)

        return value_path_entry

    def add_value_in_cap_entry(self, key, value):
        """Add an integer to the environment."""
        value_entry = CellEnvValueInCapEntry(value)
        self._add_entry(key, value_entry)

        return value_entry

    def add_atag_table(self, key, atags):
        """ Add a set of atags to the environment """
        env_atags = CellEnvATagTable(atags)
        self._add_entry(key, env_atags)

        return env_atags

    def snap(self, cell):
        """
        Snap the cap pointer names in caps.
        """

        for ent in self.path_entries:
            if not ent.has_value() and ent.path is not None:
                val_obj = self.namespace.lookup(ent.path)

                if val_obj is None:
                    # Needs more context.
                    raise MergeError, 'Object "%s" not found.' % ent.path
                else:
                    ent.obtain_value(val_obj, cell, self)

    def add_int_entry(self, tag, integer):
        """
        Create an environment entry that stores an integer.
        """
        self.add_value_entry(tag, integer)

    def get_environment_vaddr(self):
        """
        Returns where the okl4_env_t structure can be found for this cell.
        OKL4 Cell returns cell environment virtual address.
        """
        return self.memsect.segment.vaddr

    def is_complete_env_required(self, image):
        """
        Determine if a complete environment needs to be written out.
        """
        # Test if any programs link against the __okl4_environ symbol.
        for symbol_prefix, _, _ in self.programs:
            sym = image.elf.find_symbol(symbol_prefix + '__okl4_environ')

            if sym is not None:
                return True

        return False

    def generate_dynamic_segments(self, cell, image, machine, sas,
                                  image_group):
        """
        Create a memsection for the environment based on the size
        of the contents.
        """
        attrs = image.new_attrs(cell.namespace.add_namespace("cell_environment",
                                                             "memsection"))
        attrs.attach = PF_R | PF_W
        attrs.pager = None
        attrs.data = ByteArray('\0')

        dummy_env_seg = Kernel.Heap.Domain.Segment(None, attrs)

        if hasattr(cell, 'attachments'):
            my_env_ms, _ = self.add_static_memory_information(None, dummy_env_seg,
                                                           cell.attachments[0].env_realm,
                                                           None)
        else:
            my_env_ms = None

        if my_env_ms is not None:
            my_env_ms.mark_as_used()

        self.sas = image_group.sas

        out = self.generate_data(image)
        attrs.size = align_up(len(out.getvalue()), self.min_page_size)
        attrs.align = self.min_page_size

        self.memsect = image_group.add_memsection(attrs, machine,
                section_prefix = generate_section_prefix(cell.name))

        (manager_kern_domain, manager_kern_mmu) = [
            (kern_domain, kern_mmu)
            for (_, kern_domain, kern_mmu) in self.programs
            if kern_domain.manager][0]

        sas_attrs = image_group.sas_attrs(attrs)
        kern_seg = manager_kern_domain.register_segment(sas_attrs)

        if my_env_ms is not None:
            my_env_ms.kern_seg = kern_seg
            my_env_ms.kern_mmu = manager_kern_mmu

        for (_, _, kern_mmu) in self.programs:
            kern_mmu.register_mapping(kern_seg, sas_attrs)

        return kern_seg

    def generate_init(self, image):
        """Create the environment section."""

        out = self.generate_data(image)

        dlen = align_up(len(out.getvalue()), self.min_page_size)

        # Allow the environment to shrink. In some cases we need to.
        # e.g. Weaving out the "address_space" object on Nano - the object
        # shrinks in size once we know the precise virtpool that we allocate
        # out of.
        assert(self.memsect.attrs.size >= dlen)

        section = ByteArray(out.getvalue())
        self.memsect.segment.sections[0]._data[:dlen] = section

        image.phys_reloc_table.add_relocations(out.to_phys_addr(self.memsect.segment.paddr))

        self.patch_environment_vars(image)

        # Patch in the symbol to the full environment, if required.
        for symbol_prefix, _, _ in self.programs:
            sym = image.elf.find_symbol(symbol_prefix + '__okl4_environ')

            if sym is not None:
                image.patch(sym, self.get_environment_vaddr())

    def register_environment_patch(self, symbol, object_type, env_name,
            env_entry):
        """Tells elfweaver that it needs to patch a user pointer to an
        environment entry."""
        self.env_patches.append((symbol, object_type, env_name, env_entry))
        env_entry.mark_as_used()

    def find_weaved_environment_requests(self, cell_name, image):
        """Find any variables that need to be patched in as pointers into the
        environment. The code indicates such objects to us by inserting
        specially named symbols into the symbol table.
        """

        # Build a regexp to match against the symbol name.
        # We are matching symbols such as:
        # okl4test-MAIN-.weaved_env$okl4_mutex_t$DEVICE_MUTEX
        #  or
        # okl4test-MAIN-.weaved_env_create$okl4_mutex_t$DEVICE_MUTEX$attributes
        weaved_env_re = re.compile(
            r'^(?P<cellname>[^-]+)-' +
            r'(?P<program>[^-]+)-' +
            r'.weaved_env(?:_create)?\$' +
            r'(?P<object_type>[^$]+)\$' +
            r'(?P<env_name>[^$]+)')

        for symbol in image.elf.get_symbol_table().get_all_symbols():
            # Search for any symbols matching our regexp.
            if symbol is None or symbol.name is None:
                continue
            match = weaved_env_re.match(symbol.name)
            if match is None:
                continue

            # Extract the relevant text from the symbol name.
            matchdict = match.groupdict()
            cellname = matchdict['cellname']
            program = matchdict['program']
            object_type = matchdict['object_type']
            env_name = matchdict['env_name']

            # Check that the symbol relates to our cell.
            if cellname != cell_name:
                continue

            orig_env_name = env_name
            env_entry = self.entries.get(env_name.upper(), None)

            if env_entry is None:
                # The environment entry may be prefixed with the program name,
                # if it was a weave-time created object.  Try looking for
                # that.
                env_name = '%s_%s' % (program.upper(), env_name)
                env_entry = self.entries.get(env_name, None)

            if env_entry is not None:
                self.register_environment_patch(
                        symbol, object_type, env_name, env_entry)
            else:
                raise Exception(
                    "Found environment tag: %s in cell: %s. Cannot find %s in environment %s for weave-time patching. Available are: %s" % (
                        orig_env_name, cell_name, orig_env_name, self.name, sorted(self.entries.keys())))

    def patch_environment_vars(self, image):
        """Perform the patching of all weave-time environment references now
        that the environment itself has been weaved out."""
        for symbol, object_type, env_name, env_entry in self.env_patches:
            # Find the address of the environment entry.
            address = env_entry.get_address()
            assert address is not None

            # Check that we're patching a pointer of the correct type.
            okl4type = env_entry.get_libokl4_type_name()
            if object_type != okl4type:
                raise Exception("Cannot patch environment pointer with key " + \
                    "%s of type %s to type %s. " % (env_name, object_type,
                        okl4type))

            # Determine how many patchings we need to perform from the size of
            # the symbol or the section that it lies within.
            size = symbol.size
            if size == 0:
                # GCC doesn't give us the size of the section in the symbol, so
                # we need to lookup the size of the section itself.
                size = symbol.section.get_size()

            word_size = Autogen.types.OKL4_word_t._type_size
            assert (size % word_size) == 0

            for offset in xrange(0, size, word_size):
                # Patch the pointer to its environment entry.
                word = Autogen.types.OKL4_word_t(0)
                word.value = address
                word.patch(image, symbol, offset)

    def generate_data(self, image):
        """
        Generate the binary form of the environment.
        """
        out = PhysRelocWriter()

        complete_env = self.is_complete_env_required(image)
        section = CellEnvSection(out, complete_env)

        # Determine the virtual address of each string and entry.
        # This method can be called before the base address of the
        # memsection is know, in which case use a base address of 0,
        # because it doesn't really matter.
        if (self.memsect != None and
            self.memsect.attrs.virt_addr_sas(self.sas) is not None):
            base = self.memsect.attrs.virt_addr_sas(self.sas)
        else:
            base = 0

        section.write_environment(base, self.entries, self.sas)

        return out
