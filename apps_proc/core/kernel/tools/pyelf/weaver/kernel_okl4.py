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

"""Collection of items that will initialise a okl4 kernel."""

from elf.core import SectionedElfSegment
from elf.section import UnpreparedElfSection
from elf.ByteArray import ByteArray
from elf.constants import PF_R, PF_W, PT_LOAD, SHT_PROGBITS, SHT_NOBITS, \
        SHF_WRITE, SHF_ALLOC
from elf.util import align_down, align_up

from weaver.util import get_symbol
from weaver import MergeError

from pyokl4.autogen import Autogen
import pyokl4.events as events
import pyokl4.okl4.segment as segments

from pyokl4.okl4.mmu_common.access import \
        ACCESS_KRWX_URX

# Checks if any regions in a given array overlap. Assumes that the array
# is already sorted.
segment_ranges = []
def assert_no_segment_overlaps():
    """Raise an exception if any of the regions in segment_ranges overlap."""
    segment_ranges.sort()

    highest_memory = -1L
    highest = ()

    # Check for overlaps and give a meaningful error message if there
    # is one.
    for (base, size, attrs, must_not_alias) in segment_ranges:
        if base <= highest_memory and must_not_alias:
            raise MergeError, \
                  'The segment memory region 0x%x--0x%x (size 0x%x) in ' \
                  '"%s" overlaps with region 0x%x--0x%x (size 0x%x) ' \
                  'in "%s".  This is an internal error.' % \
                  (base, base + size - 1, size, attrs.abs_name(),
                   highest[0], highest[0] + highest[1] -1,
                   highest[1], highest[2].abs_name())
        else:
            highest_memory = base + size - 1
            highest = (base, size, attrs, must_not_alias)

class IRQAllocator(object):
    """
    An IRQ allocator, aware of physical devices, vIRQs, virtual interrupt
    lines and channels.
    """
    def __init__(self):
        self.allocations = {}
        self.virqs_alloced = 0

    def add_irq(self, num, owner, object):
        if num in self.allocations:
            self.allocations[num].append((owner, object))
        else:
            self.allocations[num] = [(owner, object)]

    def allocate_virq(self, owner):
        # Allocation algorithm used:
        # An vIRQ must be unique across the entire system.
        # However it may share the same IRQ as a physical device so long as the
        # domain is not using that device.

        if self.virqs_alloced > Autogen.types.MAX_VIRQS:
            raise MergeError, \
                    "More than %d vIRQ registrations in the system\n" % \
                    Autogen.types.MAX_VIRQS

        irq = 1

        while irq in self.allocations:
            irq_is_free = True

            for (irq_owner, irq_obj) in self.allocations[irq]:
                if irq_obj.is_virq or \
                        (irq_owner == owner and not irq_obj.is_virq):
                    irq_is_free = False
                    break

            if irq_is_free:
                break
            else:
                irq += 1

        self.virqs_alloced += 1

        return irq

class OKL4Kernel(object):
    """
    A class representing the compiled kernel.  This class contains all
    of the particular knowledge of the kernel composition as it
    appears in the kernel binary.

    If a composition is sufficiently different that a different class
    would be worthwhile, then that new class must be a subclass of
    this one.

    This class is reponsible for creating all objects in the kernel
    heap and allocating cap id for use in userland.
    """

    def __init__(self, elf, namespace):
        self.namespace = namespace
        self.device_list = []
        self.elf = elf
        self.irq_allocator = IRQAllocator()

        # Reset segment ranges list to avoid unit test problems.
        global segment_ranges
        segment_ranges = []

        # Create two events: one for just after we load the kernel,
        # and one just before we write out the kernel.
        events.create_event("kernel_image_load")
        events.create_event("kernel_image_writeout")
        events.create_event("kernel_objects_collected")

    def add_kernel_pools(self, machine, pools):
        """
        Create any internal pools for the kernel.
        """

        # The kernel uses memory above KERNEL_VADDR_BASE (typically
        # 0xff000000), with the kernel text starting at
        # KERNEL_VADDR_BASE, and devices between KERNEL_DEVICE_VADDR_BASE
        # and the vector table. Kernel-user copy areas are placed
        # immediately above USER_VIRT_END for configurations that
        # need them.

        if not Autogen.mmu.has_unified_address_space():
            ktext_vaddr = Autogen.machine_info.KERNEL_VADDR_BASE
            kdev_vaddr = Autogen.machine_info.KERNEL_DEVICE_VADDR_BASE
            kshared_vaddr = Autogen.machine_info.USER_VIRT_END

            # Pool for the kernel text and heap.
            pools.new_virtual_pool("kernel", machine)
            pools.add_virtual_memory("kernel", machine,
                                     base=ktext_vaddr,
                                     size=kdev_vaddr - ktext_vaddr)

            # Pool for buffers accessible by both user contexts and the kernel,
            # if necessary.
            if 'MMU_HAS_COPY_AREA' in Autogen.types.__dict__:
                kshared_size = 0

                if 'channel' in Autogen.features:
                    kshared_size += Autogen.types.CHANNEL_PRIMARY_MAX_KMMUS * \
                            0x100000

                assert kshared_size <= ktext_vaddr - kshared_vaddr

                if kshared_size > 0:
                    pools.new_virtual_pool("kernel_copy_areas", machine,
                                           is_global=True)
                    pools.add_virtual_memory("kernel_copy_areas", machine,
                                             base=kshared_vaddr,
                                             size=kshared_size)

            # Pool for kernel devices.
            pools.new_virtual_pool("kernel_devices", machine)
            pools.add_virtual_memory("kernel_devices", machine,
                                     base=kdev_vaddr,
                                     size=0x100000)

    def reserve_kernel_regions(self, sas):
        if not Autogen.mmu.has_unified_address_space():
            # Mark any regions reserved for temporary mappings. This is used
            # for the cache colour flush area on ARMv6.
            reserved = Autogen.kernel_arch.linker.get_reserved_virt()

            for name, base, size in reserved:
                sas.mark_virtual(
                    self.namespace.add_namespace(name,
                                                 "reserved memory").abs_name(),
                    base, size, "default")

    def alloc_copy_area(self, sas, machine, namespace):
        """Allocate an area for kernel-user shared mappings."""
        # Allocate a 1MB virtual region for buffers shared between the kernel
        # and a user context. This is used for channel buffers on ARMv5.
        realm_pool = sas.new_realm("mmu_copy_area", machine)

        # If priming the realm pool fails, there is no copy area pool.
        try:
            realm_pool.prime("kernel_copy_areas", [], sas)
        except MergeError:
            return False

        return realm_pool

    def patch_device_symbols(self, kernel, image):
        """Patch the addresses of devices into the SoC."""
        for (dev_name, attrs) in self.device_list:
            image.patch(get_symbol(image.elf, dev_name), long(attrs.virt_addr))

    def patch_global_struct(self, elf, image, machine):
        """Patch the magic kernel 'global' struct"""
        if 'kernel_smp' in Autogen.modules:
            # Location of the global struct we want to actually patch
            global_sym = get_symbol(elf, '_global')
            global_data = Autogen.types.OKL4_global(global_sym.value)

            # We want to patch pointers to the actual lock queue items, so
            # look up the symbol for the queue array in the .data section.
            # From there we just do manual calculations to figure out where
            # each new struct starts.
            queue_items_sym = get_symbol(elf, 'lock_queue_items')

            # Lookup the array of cpu local data
            cpu_local_items_sym = get_symbol(elf, 'cpu_local_items')

            # Number of CPUs variable
            num_cpus_sym = get_symbol(elf, 'num_cpus')

            # The cpu_local_items array was generated as [MAX_NUM_CPUS]
            # so we can derive the compiled in max number of CPUs to use
            # from it.
            max_num_cpus = cpu_local_items_sym.size / \
                    Autogen.types.OKL4_cpu_local._type_size

            if max_num_cpus < machine.num_cpus:
                raise MergeError("This kernel was compiled to support up to "
                                 "%d CPUs but %d CPUs were asked for in the "
                                 "XML" % (max_num_cpus, machine.num_cpus))

            queue_items_data = \
                    [Autogen.types.OKL4_clh_lock_queue_item(
                        queue_items_sym.value +
                        i * Autogen.types.OKL4_clh_lock_queue_item._type_size)
                        for i in range(0, max_num_cpus + 1)]

            cpu_local_data = \
                    [Autogen.types.OKL4_cpu_local(
                        cpu_local_items_sym.value +
                        i * Autogen.types.OKL4_cpu_local._type_size)
                        for i in range(0, max_num_cpus + 1)]

            if Autogen.scheduler.has_affinity():
                scheduler_queue_array_sym = get_symbol(elf, 'scheduler_queue_array')
                scheduler_queue_array = \
                    [Autogen.types.OKL4_scheduler_queue(
                        scheduler_queue_array_sym.value +
                        i * Autogen.types.OKL4_scheduler_queue._type_size)
                     for i in range(max_num_cpus)]

                for i in range(max_num_cpus):
                    cpu_local_data[i].scheduler_queue = scheduler_queue_array[i]

            # Link the queue head to the first queue item
            global_data.lock_queue_head = queue_items_data[0]

            # Link the cpu local data's pointers to each subsequent queue item
            for i in range(0, max_num_cpus):
                cpu_local_data[i].lock_queue_item = \
                        queue_items_data[i + 1]
                cpu_local_data[i].effective_priority = \
                        Autogen.scheduler.num_priorities() + 1

            global_data.patch(image, global_sym)

            for i in range(0, max_num_cpus):
                cpu_local_data[i].patch(image, cpu_local_items_sym,
                        i * Autogen.types.OKL4_cpu_local._type_size)

            image.patch(num_cpus_sym, machine.num_cpus)

    def add_device_mem(self, kernel, dev, image, machine, pools):
        # Naming format: <name>_dev
        name = dev.name.split('_')[0]

        # Ensure keys are sorted
        for i, key in enumerate(sorted(dev.physical_mem.keys())):
            # XXX: Assume there is only one region in each key
            assert (len(dev.get_physical_mem(key)) == 1)
            (base, size, _, cache_attr, _) = dev.get_physical_mem(key)[0]

            attrs = \
                  image.new_attrs(self.namespace.add_namespace("%s_mem%d" %
                                                               (name, i),
                                                               "device memory"))
            attrs.attach = PF_R | PF_W
            attrs.phys_addr = base
            attrs.virtpool = "kernel_devices"
            attrs.size = size

            assert type(cache_attr) == type("")
            if cache_attr is not None:
                attrs.cache_attr = machine.get_cache_attr(cache_attr)

            memsec = kernel.group.add_memsection(attrs, machine, pools)
            sas_attrs = kernel.group.sas_attrs(attrs)

            kernel.register_kernel_mapping(sas_attrs)

            device_mem_name = "%s_mem%d" % (name, i)
            self.device_list.append((device_mem_name, sas_attrs))


    def install_kernel_mappings_arm(self, kernel, domain, machine):
        one_MB = 0x100000
        four_KB = 0x1000

        kernel_sections = {".init" : [0, 0], ".text" : [0, 0],
            ".irqtext" : [0, 0], ".rodata" : [0, 0], ".data" : [0, 0],
            ".bss" : [0, 0]}

        for sh in kernel.elf.get_section_headers():
            if sh._name in kernel_sections:
                kernel_sections[sh._name][0] = sh.sh_addr
                kernel_sections[sh._name][1] = sh.sh_size

        def map_kernel_page (vaddr, paddr, size, perms, cache,
                             dont_filter_physreloc=False):
            """Simple wrapper for autogen map_kernel_page()."""
            Autogen.mmu_arch.mmu.map_kernel_page(domain, vaddr, paddr, size,
                                                 perms, cache,
                                                 dont_filter_physreloc)

        def map_kernel_unified(phys_addr):
            """
            Map into the kernel the entire text and heap address space
            (currently some 15M).

            The entire region is mapped, even in the page does not
            contain the text or the heap, in preparation for the user
            to install additional segments into a domain's heap.
            These segments must be within 15M of the physical address
            of the kernel text.
            """
            assert (phys_addr & (one_MB - 1)) == 0

            perms = Autogen.types.OKL4_page_perms.PAGE_PERMS_NONE
            cache = Autogen.types.OKL4_cache_attr.CACHE_ATTR_DEFAULT

            for virt_addr in range(Autogen.machine_info.KERNEL_VADDR_BASE,
                                   Autogen.machine_info.KERNEL_DEVICE_VADDR_BASE,
                                   one_MB):
                map_kernel_page(virt_addr, phys_addr, one_MB, perms,
                                cache, dont_filter_physreloc=True)

                phys_addr += one_MB

        def map_kernel_fine_grained(phys_addr):
            """
            Map into the kernel .init, .stack, .text and heap address space
            (currently some 15M).

            The entire region is mapped, even in the page does not
            contain the text or the heap, in preparation for the user
            to install additional segments into a domain's heap.
            These segments must be within 15M of the physical address
            of the kernel text.
            """
            assert (phys_addr & (one_MB - 1)) == 0

            cache = Autogen.types.OKL4_cache_attr.CACHE_ATTR_DEFAULT

            # Map .init and . stack in KRW_UNONE permission
            # It is allowable to put .init in not executalbe page since mmu
            # hasn't been enabled.
            assert (kernel_sections[".init"][0] & (four_KB - 1)) == 0
            virt_addr = kernel_sections[".init"][0]
            size = kernel_sections[".text"][0] - kernel_sections[".init"][0]
            map_kernel_page(kernel_sections[".init"][0], phys_addr, size,
                    Autogen.types.OKL4_page_perms.PAGE_PERMS_KRW_UNONE, cache)
            phys_addr += size
            virt_addr += size

            # Map .text, .irqtext in KRX_UNONE permission
            assert (kernel_sections[".text"][0] & (four_KB - 1)) == 0
            size = kernel_sections[".rodata"][0] - kernel_sections[".text"][0]
            map_kernel_page(kernel_sections[".text"][0], phys_addr, size,
                    Autogen.types.OKL4_page_perms.PAGE_PERMS_KRX_UNONE, cache)
            phys_addr += size
            virt_addr += size

            # Map .rodata in KR_UNONE permission
            assert (kernel_sections[".rodata"][0] & (four_KB - 1)) == 0
            size = align_up(kernel_sections[".rodata"][1], four_KB)
            map_kernel_page(kernel_sections[".rodata"][0], phys_addr, size,
                    Autogen.types.OKL4_page_perms.PAGE_PERMS_KR_UNONE, cache)
            phys_addr += size
            virt_addr += size

            # Map .data, .bss in KRW_UNONE permission
            assert (kernel_sections[".data"][0] & (four_KB - 1)) == 0
            size = align_up(kernel_sections[".bss"][0] +
                    kernel_sections[".bss"][1] - kernel_sections[".data"][0],
                    four_KB)
            map_kernel_page(kernel_sections[".data"][0], phys_addr, size,
                    Autogen.types.OKL4_page_perms.PAGE_PERMS_KRW_UNONE, cache)
            phys_addr += size
            virt_addr += size

            # Map all the remaining pages as KRW_UNONE permission
            perms = Autogen.types.OKL4_page_perms.PAGE_PERMS_KRW_UNONE
            map_kernel_page(virt_addr, phys_addr,
                    Autogen.machine_info.KERNEL_HEAP_VADDR_END - virt_addr,
                    perms, cache, dont_filter_physreloc=True)


        #XXX: We assume mapping[0] is the kernel
        if hasattr(Autogen.types, "FINE_GRAINED_PAGES"):
            # Map kernel sections .init, .stack, .text, .data, .tracebuffer
            # in separate access permission
            map_kernel_fine_grained(kernel.kernel_only_mappings[0].phys_addr)
        else:
            # Map kernel .init, .stack, .text, .data, .tracebuffer, .bss
            # in the same access permission
            map_kernel_unified(kernel.kernel_only_mappings[0].phys_addr)

        # Override generic mappings for those that need to be accessible by the
        # user.
        special_mappings = {}
        # Userspace helpers page provided by the kernel.
        # Page access permission for armv7 is KRX_URX
        if machine.elfweaver_info.arch in ("armv7"):
            perm = Autogen.types.OKL4_page_perms.PAGE_PERMS_KRX_URX
        else:
            perm = Autogen.types.OKL4_page_perms.PAGE_PERMS_KRWX_URX
        special_mappings[ \
            align_down(Autogen.kernel_arch.linker.get_helpers_base(), four_KB)] = (
            perm,
            Autogen.types.OKL4_cache_attr.CACHE_ATTR_DEFAULT)

        # We want this for ARMv5 microvisor only - check for cpd_bitmap
        if 'cpd_bitmap' in Autogen.modules and 'vcpu' in Autogen.modules:
            # User helpers are in the vectors page.
            # Need to map the vectors page user readable/executable as that is
            # where our user helpers lie.
            special_mappings[Autogen.kernel_arch.linker.get_vectors_base()] = (
                ACCESS_KRWX_URX,
                Autogen.types.OKL4_cache_attr.CACHE_ATTR_DEFAULT)

        if machine.elfweaver_info.arch in ("armv7") or Autogen.scheduler.has_tasklock():
            # Map .share page.
            special_mappings[Autogen.kernel_arch.linker.get_tasklock_base()] = (
                Autogen.types.OKL4_page_perms.PAGE_PERMS_KRW_URW,
                Autogen.types.OKL4_cache_attr.CACHE_ATTR_DEFAULT)

        # And now finally map the kernel mappings.
        attr = kernel.kernel_only_mappings[1]
        if attr.virt_addr in special_mappings:
            (perms, cache) = special_mappings[attr.virt_addr]
        else:
            perms = Autogen.types.OKL4_page_perms.PAGE_PERMS_NONE
            if attr.cache_attr != None:
                cache = Autogen.machine_info.cache_attributes[attr.cache_attr]
            else:
                cache = Autogen.types.OKL4_cache_attr.CACHE_ATTR_DEFAULT
        map_kernel_page(attr.virt_addr, attr.phys_addr, attr.size, perms, cache)

        # Map all the device mappings
        for attr in kernel.kernel_only_mappings[2:]:
            if attr.virt_addr in special_mappings:
                (perms, cache) = special_mappings[attr.virt_addr]
            else:
                perms = Autogen.types.OKL4_page_perms.PAGE_PERMS_KDEVICE
                if attr.cache_attr != None:
                    cache = Autogen.machine_info.cache_attributes[attr.cache_attr]
                else:
                    cache = Autogen.types.OKL4_cache_attr.CACHE_ATTR_DEFAULT

            map_kernel_page(attr.virt_addr, attr.phys_addr, attr.size, perms,
                            cache)

    def get_kernel_mapping_function(self, cpu, product, which='default'):
        nop = lambda *args, **kwargs: None
        if product == 'okl4pico':
            funcs = {'default' : nop}
        else:
            funcs = {'default': self.install_kernel_mappings_arm}

        return funcs[which]

    def install_kernel_mappings(self, kernel, domain, machine):
        assert domain is not None
        func = self.get_kernel_mapping_function(machine.cpu,
                machine.elfweaver_info.product)
        func(kernel, domain, machine)


    def generate_mmu_context(self, machine, okl4_domain, mmu, num_segments, sas):
        """Create a new address space."""

        # Create a new MMU context.

        if mmu.num_vuas is not None:
            okl4_mmu_context = Autogen.mmu.create_context(okl4_domain,
                                                          num_segments = num_segments,
                                                          l2_pgtable_buffer_nb = mmu.l2_pgtable_buffer_nb,
                                                          vuas = mmu.num_vuas)
        else:
            okl4_mmu_context = Autogen.mmu.create_context(okl4_domain, num_segments, mmu.intlock_allowed)

        mmu.okl4_object = okl4_mmu_context

        # Populate any threads in the space.
        for thread in mmu.threads:
            # Create a new thread.
            assert thread.affinity is None or thread.affinity < machine.num_cpus

            okl4_thread = Autogen.objmanager.create_weaved_thread(
                    domain = okl4_domain,
                    domain_id = okl4_domain,
                    mmu_context_id = okl4_mmu_context,
                    ip = thread.start,
                    sp = thread.get_sp(),
                    mode = 0,
                    priority = thread.priority,
                    timeslice = thread.timeslice,
                    affinity = thread.affinity,
                    kdbname = thread.friendly_name)
            thread.okl4_object = okl4_thread
            thread.cap_id = okl4_thread._cap_id

            # Setup the initial registers
            okl4_thread.hal.reg.r0 = thread.init_func
            okl4_thread.hal.reg.r1 = thread.entry

            # Lock any mutexes that are held by us.
            for mutex in thread.held_mutexes:
                assert(mutex.holder == thread)
                Autogen.mutex.mutex.mutex_lock(mutex.okl4_object, okl4_thread)

        # Map the user-specified mappings into this space if we have an mmu.
        if not Autogen.mmu.has_unified_address_space():
            # If the MMU supports segments, attach the segment (or increment
            # its reference count).
            if Autogen.mmu.has_segment_support():
                for segment in mmu.seg_attachments:
                    if segment.attrs.need_attaching():
                        index = mmu.segment_table.index(segment)
                        Autogen.mmu.attach_segment(okl4_mmu_context,
                                                   segment.okl4_object,
                                                   index)

            for (segment, attrs) in mmu.mappings:
                if not attrs.need_mapping():
                    continue

                (virt_addr, phys_addr, size) = attrs.aligned_addrs(machine)
                assert(virt_addr % machine.min_page_size() == 0)
                assert(phys_addr % machine.min_page_size() == 0)

                (_, seg_phys, _) = segment.attrs.aligned_addrs(machine)
                Autogen.mmu_arch.mmu.map_page(okl4_domain, okl4_mmu_context,
                        virt_addr, segment, phys_addr - seg_phys,
                        size, attrs.attach,
                        Autogen.machine_info.cache_attributes[attrs.cache_attr])

        # Check that the user-specified mappings don't map the same
        # physical memory more than once.
        if (not 'cache_control' in Autogen.features) or \
                not Autogen.cache_control.has_virtual_alias_support():
            for (_, attr1) in mmu.mappings:
                for (_, attr2) in mmu.mappings:
                    if attr1 is attr2:
                        continue

                    (_, phys1, size1) = attr1.aligned_addrs_sas(machine, sas)
                    (_, phys2, size2) = attr2.aligned_addrs_sas(machine, sas)

                    if phys1 + size1 <= phys2:
                        continue

                    if phys2 + size2 <= phys1:
                        continue

                    # If we get here, it means the two physical ranges overlap
                    raise MergeError, \
                        'Physical address of "%s" and "%s" overlap.' \
                                % (attr1.abs_name(), attr2.abs_name())

        return okl4_mmu_context

    def check_unsourced_virqs(self, domain):
        for virq in [v for v in  domain.virqs if v.unsourced]:
            raise MergeError, "virq %s has invalid source: %s\n" % \
                    (virq.namespace.abs_path("."), virq.source)

    def generate_domain_irqs(self, domain, heap_name):

        # Assign any irqs to the domain, this includes any 'use_device' elements.

        for irq in domain.irqs + domain.virqs:
            # To enable the use of an IRQ, we create an IRQ object. C syscall
            # bindings do not actually exist for the creation of this object
            # as it is expected that all of these objects will be created
            # by the time the system boots.
            if 'interrupt_virq' in Autogen.modules:
                if irq.is_virq:
                    irq.irq = self.irq_allocator.allocate_virq(heap_name)

                okl4_irq = Autogen.syscalls.sys_interrupt_create(
                        domain.okl4_object, irq.irq, irq.is_virq)
            else:
                okl4_irq = Autogen.syscalls.sys_interrupt_create(
                        domain.okl4_object, irq.irq)
            irq.okl4_object = okl4_irq
            self.irq_allocator.add_irq(irq.irq, heap_name, irq)

    def generate_domain(self, machine, okl4_heap, domain):
        """Create the given domain."""

        # Create the domain. If we have a parent domain, we create our new
        # domain out of it. Otherwise, we have to create an initial domain.
        if domain.max_prio is None:
            domain.max_prio = Autogen.scheduler.num_priorities() - 1

        # If a number of reply caps was not specified, use the same number
        # as we have clist entries.
        if domain.max_reply_caps == None:
            domain.max_reply_caps = domain.max_caps

        if domain.manager:
            # Create a new initial domain.
            okl4_domain = Autogen.syscalls.sys_initial_domain_create(okl4_heap,
                    domain.max_caps, domain.max_prio, domain.max_reply_caps)
            domain.okl4_object = okl4_domain
            okl4_domain.set_elfweaver_obj(domain)
        else:
            okl4_domain = \
                    Autogen.syscalls.sys_domain_create(
                            domain.owner_domain.okl4_object, domain.max_caps,
                            domain.max_prio, domain.max_reply_caps)
            domain.okl4_object = okl4_domain
            okl4_domain.set_elfweaver_obj(domain)

            okl4_domain._source_heap = \
                    domain.owner_domain.okl4_object._source_heap
            okl4_domain.objmanager.heap.next = \
                    domain.owner_domain.okl4_object.objmanager.heap
            okl4_domain.objmanager.heap.size = 0

        # Grant the domain access to the platform control system call.
        if domain.plat_control:
            domain.plat_control.okl4_object = \
                    Autogen.syscalls.sys_platform_control_create(okl4_domain)

        # Grant the domain access to the processor control system call.
        if domain.processor_control:
            domain.processor_control.okl4_object = \
                    Autogen.syscalls.sys_processor_control_create(okl4_domain)

        # Grant the domain access to the profile control system call.
        if domain.profile_control:
            domain.profile_control.okl4_object = \
                    Autogen.syscalls.sys_profile_control_create(okl4_domain)

        # Grant the domain access to the intlock control system call.
        if domain.intlock_control:
            domain.intlock_control.okl4_object = \
                    Autogen.syscalls.sys_intlock_control_create(okl4_domain)

        # Create any mutexes for this domain.
        for mutex in domain.mutexes:
            if mutex.is_hybrid:
                okl4_mutex = Autogen.syscalls.sys_hybrid_mutex_create(
                        okl4_domain)
            # FIXME: Jira ticket #QCT-95 - junggyu.
            # The use of inheritance_type in elfweaver should be
            # determined more cleanly
            elif hasattr(Autogen.types, "OKL4_inheritance_type"):
                okl4_mutex = Autogen.syscalls.sys_mutex_create(okl4_domain,
                        mutex.is_counted, Autogen.types.OKL4_inheritance_type.INHERITANCE_TYPE_INHERITANCE)
            else:
                okl4_mutex = Autogen.syscalls.sys_mutex_create(okl4_domain,
                        mutex.is_counted)
            mutex.okl4_object = okl4_mutex

        # Setup any futex data structures for this domain.
        if 'futex' in Autogen.features:
            Autogen.futex.futex_init(okl4_domain, domain.max_threads)

        if 'tracebuffer' in Autogen.modules and \
                'tracebuffer_null' not in Autogen.modules:
            # Create any tracebuffers for this domain
            if domain.tracebuffer:
                tracebuffer = Autogen.types.OKL4_tracebuffer_header(
                        domain.tracebuffer.kernel_virt_addr)

                # Initialise the header and set trace_buffer pointer
                Autogen.tracebuffer.tracebuffer_init(tracebuffer,
                        domain.tracebuffer.attrs.size,
                        domain.tracebuffer.buffers)

                # Put the initialised header in the memsection
                data = tracebuffer.to_data()
                ms = domain.tracebuffer.memsection
                ms.segment.sections[0]._data[:len(data)] = ByteArray(data)

                # Create the control object (used for access control)
                domain.okl4_tracebuffer_control_object = \
                        Autogen.syscalls.sys_tracebuffer_control_create(
                                okl4_domain)

        # Create any segments for this domain
        if Autogen.mmu.has_segment_support():
            for segment in domain.segments:
                if not segment.attrs.need_segment():
                    continue

                # TODO : Somehow track segments so that mappings below
                # can use them.
                (_, phys_addr, size) = segment.attrs.aligned_addrs(machine)

                # Record segment properties in a global list for later
                # checking that no overlaps exist.
                segment_ranges.append((phys_addr, size, segment.attrs,
                                       segment.use_as_heap))

                # The segment create syscall is actually a segment split and
                # is incapable of actually creating a new segment.
                # For this reason we initialise the segment using the syscall
                # and manually fill in the remaining parameters.

                okl4_segment = Autogen.syscalls.sys_segment_create(okl4_domain,
                        None, # No base segment
                        phys_addr)
                segments.segment_initial_segment_create(
                    okl4_segment,
                    phys_addr, size,
                    Autogen.mmu_arch.mmu.upgrade_perms(segment.attrs.attach),
                    segment.attrs.cache_policy_mask,
                    segment.use_as_heap)
                segment.okl4_object = okl4_segment

        # Assign any tzmonitor services to the domain
        if 'trustzone' in Autogen.features:
            for tzmonitor_svc in domain.tzmonitor_svcs:
                # To register an ARM TrustZone secure monitor service, we
                # create a tzmonitor object. The actual C syscall binding
                # does not exist for creation of this object; all of these
                # objects will be created by the time system boots.
                okl4_tzmonitor = Autogen.syscalls.sys_tzmonitor_create(
                        okl4_domain, tzmonitor_svc.svc_num)
                tzmonitor_svc.okl4_object = okl4_tzmonitor

        # The creation of spaces for this domain is deferred until we have
        # created the kernel mappings that need to go into each space.

    def generate_domain_mmu_contexts(self, machine, domain, sas):
        # Create any spaces for this domain.
        # This must be performed after we know all our kernel mappings.
        for mmu in domain.mmu_contexts:
            # Create the space.
            mmu.okl4_object = self.generate_mmu_context(machine,
                                                       domain.okl4_object, mmu,
                                                       len(mmu.segment_table), sas)

    def generate_domain_message_queues(self, machine, domain):
        # Create any message_queues for this domain.
        if 'message_queue' in Autogen.features:
            for msgq in domain.message_queues:
                #pylint: disable-msg=E1101

                # FIXME: Jira ticket OLD-1461 - jamesp.
                # Resolve the mmu cap if it has been given.  Syscall needs to
                # be modified to call buffer area init if a mmu is present
                # (as is done in C) and implement those functions + mpointer
                # generation.
                mmu = None

                okl4_msgq = Autogen.syscalls.sys_message_queue_create(
                        domain.okl4_object, msgq.max_messages,
                        msgq.mmu.kern_mmu.okl4_object._cap_id, msgq.quota)
                msgq.okl4_object = okl4_msgq

    def generate_domain_channels(self, machine, domain):
        # Create any channels for this domain.
        if 'channel' in Autogen.features:
            for chn in domain.channels:
                if not chn.buffer_mapped:
                    chn.ms_attrs.virt_addr = ~0

                okl4_chn = Autogen.syscalls.sys_channel_create(
                    domain.okl4_object,
                    chn.max_messages,
                    chn.message_size,
                    chn.ms_attrs.virt_addr,
                    chn.kernel_buffer_vaddr)
                chn.okl4_object = okl4_chn
                chn.cap_id = okl4_chn._cap_id


    @staticmethod
    def hookup_channels(domain):
        if 'channel' in Autogen.features:
            for chn in domain.channels:
                for (i, virq) in enumerate(chn.virqs):
                    if virq is not None:
                        chn.okl4_object.irq.virqs[i] = chn.virqs[i].okl4_object

    def generate_switch_tables(self, domain, image):

        # Generate any switch tables in this domain.
        for switch_table in domain.switch_tables:

            # Get the return stub ip
            return_stub_ip = image.elf.find_symbol(switch_table.return_stub)
            if return_stub_ip is None:
                raise MergeError, \
                    "Symbol %s not found" % switch_table.return_stub

            #Get the mode
            mode = image.elf.find_symbol(switch_table.entry_mode)
            if mode is None:
                raise MergeError, \
                        "Symbol %s not found" % switch_table.entry_mode

            #pylint: disable-msg=E1101
            okl4_switch_table = Autogen.syscalls.sys_switch_table_create(
                    domain = domain.okl4_object,
                    domain_id = switch_table.domain.okl4_object,
                    mmu_context_id = switch_table.mmu.okl4_object,
                    num_entries = len(switch_table.entry_symbols),
                    return_stub_ip = return_stub_ip.value)

            # Get switch table ips and populate switch table.
            for entry_index, entry_symbol in \
                    enumerate(switch_table.entry_symbols):

                entry_ip = image.elf.find_symbol(entry_symbol)

                if entry_ip is None:
                    raise MergeError, "Symbol %s not found" % entry_symbol

                Autogen.switch_table.switch_table_set_entry(okl4_switch_table,
                                entry_index, entry_ip.value, mode.value)

            # Setup reference to kernel object.
            switch_table.okl4_object = okl4_switch_table
            switch_table.cap_id = okl4_switch_table.cap_id


    def make_kernel_heap_segment(self, heap_attrs, elf, name, base, data, word_bits):
        """
        The kernel heap contains multiple (occupied, empty) pairs.  Usually,
        the empty part is very large compared to the occupied part - this is
        due to each cell having a small initial state but large maximum heap
        size.

        By placing each pair into an ELF segment, we can take advantage of the
        large empty section, make it into a NOBITS section, and reduce the
        overall ELF image size."""

        heap_vbase = heap_attrs.virt_addr
        heap_pbase = heap_attrs.phys_addr
        heap_align = heap_attrs.align

        # We pad out the occupied section of the heap to the next word
        # boundary.
        size = len(data)
        data = data.rstrip('\0')
        padding = -(len(data) % (-word_bits / 8))
        data += '\0' * padding

        # Work out location of the ELF segment for the heap.
        offset = base - heap_vbase
        assert not offset < 0
        vbase = heap_vbase + offset
        pbase = heap_pbase + offset

        # Work out the empty segment of the heap.
        z_base = vbase + len(data)
        z_size = size - len(data)

        # Create the section for the heap.
        heap_sect = UnpreparedElfSection(elf, "%s.heap" % name, SHT_PROGBITS,
                vbase, data = ByteArray(data), flags = SHF_WRITE | SHF_ALLOC)
        elf.add_section(heap_sect)

        # Create the section for the empty kheap.
        heap_bss_sect = UnpreparedElfSection(elf, "%s.heap_bss" % name,
                SHT_NOBITS, z_base, data = z_size, flags = SHF_WRITE |
                SHF_ALLOC)
        elf.add_section(heap_bss_sect)

        # Create the segment for these two sections.
        segment = SectionedElfSegment(elf, PT_LOAD, vbase, pbase,
                PF_R | PF_W, heap_align, sections = [heap_sect, heap_bss_sect])
        elf.add_segment(segment)

    def generate_kernel_structures(self, kernel, elf, image, machine):
        """This is how is should be."""

        # Let modules know that all details have been collected and that any
        # changes that need to be made before we start generating kernel
        # structures should occur now.
        events.trigger("kernel_objects_collected",
                kernel = kernel, elf = elf, image = image, machine = machine)

        # Let modules know that we are starting to perform generation.
        events.trigger("kernel_image_load",
                kernel = kernel, elf = elf, image = image, machine = machine)

        self.patch_device_symbols(kernel, image)

        self.patch_global_struct(elf, image, machine)

        # FIXME / HACK: This is a hack the pulls the symbol "exception_return"
        # out of the ELF file and into a global variable, accessable by
        # 'get_exception_return' so that we don't have to pass the ELF file
        # around.
        Autogen.kernel.switch_to.set_exception_return(elf)

        #
        # The kernel consists of several heaps, each of which may contain
        # one or more objects in it that are allocated out of it. Domains
        # created inside a heap also use that heap.
        #
        for heap in kernel.heaps.values():
            heap.okl4_object = Autogen.syscalls.sys_heap_create(
                heap.virt_addr, heap.phys_addr, heap.size,
                heap.get_preallocated_objects())
            heap.okl4_object.set_elfweaver_obj(heap)

        # Fill out the heaps.
        for heap in kernel.heaps.values():
            # Generate the domains inside this cell
            for domain in heap.domains:
                self.generate_domain(machine, heap.okl4_object, domain)

        domains = sum([heap.domains for heap in kernel.heaps.values()], [])
        # Create global kernel mappings.
        kernel_mappings_domain = kernel.heaps.values()[0].domains[0].okl4_object
        self.install_kernel_mappings(kernel, kernel_mappings_domain, machine)

        if 'kernel_smp' in Autogen.modules:
            # Create kernel MMU context.
            Autogen.mmu_arch.mmu.kernel_context_create(kernel_mappings_domain)

        # Create spaces associated with each heap.
        for heap_name, heap in kernel.heaps.items():
            # Generate the domains inside this cell
            for domain in heap.domains:
                self.generate_domain_mmu_contexts(machine, domain, kernel.sas)
                self.generate_domain_message_queues(machine, domain)
                self.generate_domain_irqs(domain, heap_name)
                self.generate_domain_channels(machine, domain)

        # Hook up interrupt sources to virqs.
        cell_list = kernel.heaps.keys()
        if 'interrupt_virq' in Autogen.modules:
            # Ensure every virq has a valid source
            for domain in domains:
                self.hookup_channels(domain)
                self.check_unsourced_virqs(domain)

        # Now we iterate over the domains again and create any switch_table
        # objects associated with them. This must be done after all domains and
        # mmus have been created because the switch_table may contain a
        # reference to them.

        for domain in domains:
            self.generate_switch_tables(domain, image)


        # Setup caps in all the domains.
        for domain in domains:
            for cap in domain.caps:
                source_domain = cap.source_object.domain.okl4_object
                source_object = cap.source_object.okl4_object

                cap.okl4_object, cap.okl4_capid = \
                             Autogen.objmanager.syscalls.sys_cap_copy(
                                          current_domain = domain,
                                          src_domain = source_domain,
                                          src_cap = source_object._cap,
                                          dest_domain = domain.okl4_object,
                                          rights_mask = cap.rights_mask)

        # The simple objectmanager defers allocation until all of the heap
        # contents are known to optimise memory usage.  Inform the heaps that
        # it is time to finalise the allocations.
        for heap in kernel.heaps.values():
            heap.okl4_object.finalise_allocations()

        # Let modules know that we have finished setting everything up,
        # and are just about to write out the image.
        events.trigger("kernel_image_writeout",
                kernel = kernel, elf = elf, image = image, machine = machine)

        # Make sure that no overlapping segments have been created.
        # QC_MODIFIED FIXME assert_no_segment_overlaps()

        # Create the kernel heap segment.

        for heap_name, heap in kernel.heaps.items():
            base = heap.okl4_object.get_base_address()
            data = heap.okl4_object.to_data()
            phys_relocs = heap.okl4_object.get_phys_relocs()
            heap_attrs = kernel.group.sas_attrs(image.kernel_heap.get_attrs())

            heap_phys = heap_attrs.phys_addr + (base - heap_attrs.virt_addr)

            for reloc in phys_relocs:
                reloc.address += heap_phys

            image.phys_reloc_table.add_relocations(phys_relocs)

            self.make_kernel_heap_segment(heap_attrs, elf, heap_name, base,
                                          data, machine._get_word_size())

    def get_base_segment(self, elf, segs, image):
        """Return the base segment, check for XIP and patch accordingly."""

        if get_symbol(elf, '__phys_addr_rom', True) is not None:
            phys_ram_sym = get_symbol(elf, '__phys_addr_ram')
            base_segment = segs[1]
            image.patch(phys_ram_sym, base_segment)

            phys_rom_sym = get_symbol(elf, '__phys_addr_rom')
            image.patch(phys_rom_sym, segs[0])

        else:
            phys_ram_sym = get_symbol(elf, '__phys_addr_ram', True)
            base_segment = segs[0]

            if phys_ram_sym is not None:
                image.patch(phys_ram_sym, base_segment)

        if get_symbol(elf, '__phys_addr_stack', True) is not None:
            stack_sym = elf.find_symbol('__stack')

            if stack_sym is not None:
                phys_stack_sym = get_symbol(elf, '__phys_addr_stack')
                image.patch(phys_stack_sym, base_segment, value_offset = \
                                image.Patch.DiffSymSeg(stack_sym,
                                                       base_segment.segment))

        return base_segment
