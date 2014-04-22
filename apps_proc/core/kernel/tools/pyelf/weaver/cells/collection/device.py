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
XML definitions and collection functions for handling devices.
"""

from elf.constants import PF_R, PF_W

from weaver import MergeError
from weaver.ezxml import Element, str_attr, bool_attr, long_attr
from weaver.cap import SegmentRights, PhysAddrRights

UseDevice_el = Element('use_device',
                       name = (str_attr, 'required'),
                       key = (str_attr, 'optional'),
                       direct = (bool_attr, "optional"),
                       cache_policy = (str_attr, "optional"),
                       virt_base = (long_attr, "optional"))

def collect_use_devices(cell, el, image_coln, kern_mmu, kern_domain,
                        manager_kern_domain, namespace, image, machine,
                        env):
    device_mem = []

    for device_el in el.find_children("use_device"):
        dev = machine.get_phys_device(device_el.name)

        if hasattr(device_el, "key"):
            prefix = device_el.key
        else:
            prefix = dev.name

        if hasattr(device_el, 'virt_base'):
            virt_base = device_el.virt_base

            # Find out the base of the lowest physical mem section
            first_range_base = None
            for ranges in dev.physical_mem.values():
                for base, _, _, _ in ranges:
                    if first_range_base is None or base < first_range_base:
                        first_range_base = base
            if first_range_base is None:
                raise MergeError, "Device %s must have at least one physical " \
                        "memory range defined to use with virt_base attribute" \
                        % device_el.name

        # Potentially we can have multiple named physical mem sections
        # each with multiple ranges.
        for key in dev.physical_mem:
            ranges = dev.physical_mem[key]
            index = 0

            for base, size, _, cache_attr, _ in ranges:
                # If there's only one range just use the key otherwise
                # append an index to distingiush entries
                if len(ranges) == 1:
                    name = key
                else:
                    name = key + '_' + str(index)
                    index += 1
                name = prefix + '_' + name

                device_ns = namespace.add_namespace(name, "device memory")
                attrs = image.new_attrs(device_ns)
                attrs.attach = PF_R | PF_W

                if hasattr(device_el, 'cache_policy'):
                    attrs.cache_attr = \
                            machine.get_cache_attr(device_el.cache_policy)
                elif not image.is_unified and cache_attr is not None:
                    attrs.cache_attr = \
                            machine.get_cache_attr(cache_attr)

                attrs.phys_addr = base
                attrs.size = size

                attrs.direct = getattr(device_el, 'direct', attrs.direct)

                if hasattr(device_el, 'virt_base'):
                    if attrs.direct is True:
                        raise MergeError, "Device %s can only be mapped " \
                                "direct or with a virt_base, not both" \
                                % device_el.name

                    virt_addr = device_el.virt_base + (base - first_range_base)
                    attrs.set_default_virt_addr(virt_addr)

                device_ms = image_coln.add_memsection(attrs, machine)
                device_mem.append(device_ms)

                sas_attrs = image_coln.sas_attrs(device_ms.attrs)
                dev_kern_seg = \
                             manager_kern_domain.register_segment(sas_attrs,
                                                                  use_as_heap = False)
                kern_mmu.register_mapping(dev_kern_seg, sas_attrs)
                SegmentRights.add_standard_rights(dev_kern_seg, device_ns)
                PhysAddrRights.add_standard_rights(dev_kern_seg, device_ns)

                # FIXME: Jira ticket OLD-1790 - jsok.
                name = el.name + '_' + name
                if hasattr(cell, 'register_realm_mapping'):
                    if hasattr(image_coln, "env_realm"):
                        env_realm = image_coln.env_realm
                    else:
                        env_realm = None
                    cell.register_realm_mapping(name, dev_kern_seg,
                                                env_realm, kern_mmu)


        sorted_interrupts = [dev.interrupt[irq]
                             for irq in sorted(dev.interrupt.keys())]

        for irq in sorted_interrupts:
            kern_domain.register_irq(irq)

        env.add_interrupt_device_map(prefix, sorted_interrupts)

    return device_mem
