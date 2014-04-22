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
XML definitions and collection functions for handling channel.
"""
from elf.util import align_up
from elf.ByteArray import ByteArray
from weaver.memobjs_xml import collect_memobj_attrs, Memsection_el
from weaver.cap import ChannelRights, SegmentRights, PhysAddrRights, ChannelVIRQSource
from weaver.ezxml import long_attr
from weaver.ezxml import update_element
from weaver.pools import Pools
from weaver.okl4_cell_environment_atags_elf import ATagChannel

Channel_el = update_element(Memsection_el,
        max_messages = (long_attr, 'required'),
        message_size = (long_attr, 'required'))
Channel_el.set_name("channel")

def collect_channel(chn_el, image_coln, kern_mmu, manager_domain, namespace,
                    image, machine, sas, name, env, kernel, atags=None,
                    vserver=False):
    """Process a single channel element."""
    num_buffer_elems = chn_el.message_size * 2
    buffer_size = num_buffer_elems * chn_el.max_messages
    chn_el.size = align_up(buffer_size, machine.min_page_size())
    chn_ms_attrs = collect_memobj_attrs(chn_el, namespace, image, machine,
                                        representing = "channel")

    chn_ms_attrs.data = ByteArray('\0' * chn_ms_attrs.size)

    mmu_copy_area = kern_mmu.get_copy_area(kernel, sas, \
                                           machine, namespace)
    if mmu_copy_area:
        chn_ms_attrs.virtpool = mmu_copy_area
    else:
        # See tracebuffer doc in collect_xml():cell.py
        chn_ms_attrs.phys_addr = Pools.IGNORE

    chn_ns = chn_ms_attrs.ns_node
    chn_ms = image_coln.add_memsection(chn_ms_attrs, machine,
                                       section_prefix = name)

    sas_attrs = image_coln.sas_attrs(chn_ms.attrs)
    chn_seg = manager_domain.register_segment(sas_attrs)
    kern_chn = manager_domain.register_channel(chn_el.name,
                                               chn_el.max_messages,
                                               chn_el.message_size, sas_attrs,
                                               chn_ms, atags is None)

    if atags is None:
        kern_mmu.register_mapping(chn_seg, sas_attrs)

        if not vserver:
            env.add_channel_entry("%s_CHANNEL" % chn_el.name, kern_chn,
                                  chn_el.message_size)
    else:
        assert not vserver

        if mmu_copy_area:
            kern_mmu.register_mapping(chn_seg, sas_attrs)
        else:
            kern_mmu.register_attachment(chn_seg)

        atags.append(ATagChannel(chn_el.name, segment = chn_seg,
                                 attrs = sas_attrs, channel = kern_chn))

    # Put the sharing rights to the channels' buffer into the namespace.
    ChannelRights.add_standard_rights(kern_chn, chn_ns)
    # Put the channel's interrupt sources into the namespace to be found
    # later by virq objects.
    ChannelVIRQSource.add_standard_sources(manager_domain,
                                           kern_chn, chn_ns)

    # Add environment entries and rights to access the primary-side buffer
    chn_buf_ns = chn_ns.add_namespace('buffer')
    chn_buf_env_name = "%s_CHANNEL_BUFFER" % chn_el.name
    chn_buf_env = env.add_shared_buffer_entry(chn_buf_env_name, sas_attrs)

    SegmentRights.add_standard_rights(chn_seg, chn_buf_ns,
            env_entries=[(chn_buf_env_name, chn_buf_env)])
    PhysAddrRights.add_standard_rights(chn_seg, chn_buf_ns)

    # Add the channel to the memory to be allocated as part of the kernel
    # heap.
    kernel.register_channel_for_heap(kern_chn)

    return kern_chn

def collect_channels(el, image_coln, kern_mmu, manager_domain, namespace, image,
        machine, sas, name, env, kernel, atags=None, vserver=False):
    """Process the channel elements."""
    for chn_el in el.find_children('channel'):
        collect_channel(chn_el, image_coln, kern_mmu, manager_domain,
                        namespace, image, machine, sas, name, env, kernel,
                                                atags, vserver)
