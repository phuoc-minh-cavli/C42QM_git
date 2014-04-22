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
XML definitions and collection functions for handling shared buffers.
"""

from weaver.memobjs_xml import collect_memobj_attrs, Memsection_el
from weaver.cap import SegmentRights, PhysAddrRights
from weaver.ezxml import update_element
from weaver.okl4_cell_environment_atags_elf import ATagSharedBuffer
from weaver.ezxml import Element, long_attr, bool_attr, str_attr, size_attr, \
        ParsedElement

SharedBuffer_el = update_element(Memsection_el)
SharedBuffer_el.set_name("shared_buffer")

SharedBuffers_el = Element('shared_buffers',
                            buffers = (long_attr, 'required'),
                            name_base = (str_attr, 'required'),
                            file = (str_attr, 'optional'),
                            size = (size_attr, "optional"),
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
                            cache_policy_mask = (long_attr, "optional"),
                            mem_type = (str_attr, "optional"))

def collect_shared_buffers(el, image_coln, kern_mmu, manager_domain,
                           namespace, image, machine, sas, name, env,
                           atags=None):
    for shared_buf_el in el.find_children('shared_buffer'):
        shared_buf_attr = collect_memobj_attrs(shared_buf_el, namespace, image,
                                               machine,
                                               representing = "shared buffer")
        shared_buf_ns = shared_buf_attr.ns_node
        shared_buf_ms = image_coln.add_shared_buffer(shared_buf_attr, machine,
                sas, section_prefix = name)

        if shared_buf_attr.align is None:
            # force alignment to page boundary, not the default natural alignment
            shared_buf_attr.align = machine.min_page_size()

        sas_attrs = image_coln.sas_attrs(shared_buf_attr)
        shared_buf_seg = manager_domain.register_segment(sas_attrs)

        env_name = "%s_SHARED_BUFFER" % shared_buf_el.name
        shared_buf_env = env.add_shared_buffer_entry(env_name, sas_attrs)

        if atags is None:
            kern_mmu.register_mapping(shared_buf_seg, sas_attrs)
            env.add_shared_buffer_cap_entry("%s_CAP" % env_name, shared_buf_seg)
        else:
            kern_mmu.register_attachment(shared_buf_seg)
            atags.append(ATagSharedBuffer(shared_buf_el.name,
                segment = shared_buf_seg, attrs = sas_attrs))

        SegmentRights.add_standard_rights(shared_buf_seg, shared_buf_ns,
                env_entries=[(env_name, shared_buf_env)])

        # Store the returned virtmem range environment entry in the
        # segment rights to give to any other cells which have capabilities to
        # this memsection.
        PhysAddrRights.add_standard_rights(shared_buf_seg, shared_buf_ns)

def collect_shared_buffers_array(el, image_coln, kern_mmu, manager_domain,
                                namespace, image, machine, sas, name, env,
                                atags=None):
    attr = []
    segments = []

    for shared_buffers_el in el.find_children('shared_buffers'):
        for i in range(shared_buffers_el.buffers):
            shared_buf_el = ParsedElement('shared_buffer')
            shared_buf_el.name = "%s%d" % (shared_buffers_el.name_base, i)
            if hasattr(shared_buffers_el, 'file'):
                shared_buf_el.file = shared_buffers_el.file
            if hasattr(shared_buffers_el, 'size'):
                shared_buf_el.size = shared_buffers_el.size
            if hasattr(shared_buffers_el, 'virt_addr'):
                shared_buf_el.virt_addr = shared_buffers_el.virt_addr
            if hasattr(shared_buffers_el, 'phys_addr'):
                shared_buf_el.phys_addr = shared_buffers_el.phys_addr
            if hasattr(shared_buffers_el, 'physpool'):
                shared_buf_el.physpool = shared_buffers_el.physpool
            if hasattr(shared_buffers_el, 'virtpool'):
                shared_buf_el.virtpool = shared_buffers_el.virtpool
            if hasattr(shared_buffers_el, 'align'):
                shared_buf_el.align = shared_buffers_el.align
            if hasattr(shared_buffers_el, 'attach'):
                shared_buf_el.attach = shared_buffers_el.attach
            if hasattr(shared_buffers_el, 'direct'):
                shared_buf_el.direct = shared_buffers_el.direct
            if hasattr(shared_buffers_el, 'zero'):
                shared_buf_el.zero = shared_buffers_el.zero
            if hasattr(shared_buffers_el, 'pager'):
                shared_buf_el.pager = Shared_buffers_el.pager
            if hasattr(shared_buffers_el, 'cache_policy'):
                shared_buf_el.cache_policy = shared_buffers_el.cache_policy
            if hasattr(shared_buffers_el, 'cache_policy_mask'):
                shared_buf_el.cache_policy_mask = shared_buffers_el.cache_policy_mask
            if hasattr(shared_buffers_el, 'mem_type'):
                shared_buf_el.mem_type = shared_buffers_el.mem_type

            shared_buf_attr = collect_memobj_attrs(shared_buf_el, namespace, image,
                                                    machine,
                                                    representing = "shared buffer")
            shared_buf_ns = shared_buf_attr.ns_node
            shared_buf_ms = image_coln.add_shared_buffer(shared_buf_attr, machine,
                                    sas, section_prefix = name)

            if shared_buf_attr.align is None:
                # force alignment to page boundary, not the default natural alignment
                shared_buf_attr.align = machine.min_page_size()

            sas_attrs = image_coln.sas_attrs(shared_buf_attr)
            shared_buf_seg = manager_domain.register_segment(sas_attrs)

            env_name = "%s_SHARED_BUFFER" % shared_buf_el.name
            shared_buf_env = env.add_shared_buffer_entry(env_name, sas_attrs)
            attr.append(sas_attrs)

            if atags is None:
                kern_mmu.register_mapping(shared_buf_seg, sas_attrs)
                env.add_shared_buffer_cap_entry("%s_CAP" % env_name, shared_buf_seg)
                segments.append(shared_buf_seg)
            else:
                kern_mmu.register_attachment(shared_buf_seg)
                atags.append(ATagSharedBuffer(shared_buf_el.name,
                    segment = shared_buf_seg, attrs = sas_attrs))

            SegmentRights.add_standard_rights(shared_buf_seg, shared_buf_ns,
                    env_entries=[(env_name, shared_buf_env)])

            # Store the returned virtmem range environment entry in the
            # segment rights to give to any other cells which have capabilities to
            # this memsection.
            PhysAddrRights.add_standard_rights(shared_buf_seg, shared_buf_ns)

        if atags is None:
            env.add_shared_buffers_entry('SHARED_BUFFERS',
                    attr, segments)
