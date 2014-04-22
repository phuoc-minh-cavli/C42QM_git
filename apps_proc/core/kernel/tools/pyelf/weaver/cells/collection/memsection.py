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
XML definitions and collection functions for handling memsections.
"""

from weaver.memobjs_xml import collect_memobj_attrs
from weaver.cap import SegmentRights, PhysAddrRights
from weaver.cells.collection.attachment import AttachmentContainer
from pyokl4.autogen import Autogen

class MemsectionAttachment(AttachmentContainer):
    """Container for memsection object attachments"""

    def __init__(self, env_object, seg, attrs):
        """Initialise the object."""
        AttachmentContainer.__init__(self, env_object)
        self.seg = seg
        self.attrs = attrs

    def resolve(self, **kwargs):
        """Register a memsection with a mmu."""
        mmu_context = kwargs['mmu_context']
        mmu_context.register_mapping(self.seg, self.attrs)

    def set_env_attr(self, object_):
        """Set any required attributes on the environment entry."""
        assert not self.env_object.okl4_obj.memsec.attachment.in_use

        self.env_object.child.memsec.owner = \
                    Autogen.types.OKL4_void_ptr(object_._address)
        self.env_object.obj4_obj.memsec.attachment.in_use = True

    def get_list_entry(self):
        """Return the list entry object from the environment entry."""
        return self.env_object.okl4_obj.memsec.attachment.list_entry

    def get_sort_key(self):
        """Return a value to sort this attachment on."""
        return self.env_object.okl4_obj.super.base

def collect_memsections(el, image_coln, kern_mmu, manager_domain, namespace,
                        image, machine, name, env, env_realm):
    pseudo_attach_els = []

    for memsection_el in el.find_children('memsection'):
        memsection_attrs = collect_memobj_attrs(memsection_el, namespace,
                                                image, machine,
                                                representing = "memsection")
        memsection_ns = memsection_attrs.ns_node
        memsection_ms = image_coln.add_memsection(memsection_attrs, machine,
                                                  section_prefix = name)

        sas_attrs = image_coln.sas_attrs(memsection_attrs)
        ms_kern_seg = manager_domain.register_segment(sas_attrs)

        _, range_item = env.add_static_memory_information(el.name + '_' + \
                memsection_el.name, ms_kern_seg, env_realm, kern_mmu)

        if range_item:
            env_entries = [('%s_RANGE' % memsection_el.name, range_item)]
        else:
            env_entries = []

        # Store the returned virtmem range environment entry in the
        # segment rights to give to any other cells which have capabilities to
        # this memsection.
        SegmentRights.add_standard_rights(ms_kern_seg, memsection_ns,
                                          env_entries=env_entries)
        PhysAddrRights.add_standard_rights(ms_kern_seg, memsection_ns)
        kern_mmu.register_mapping(ms_kern_seg, sas_attrs)

#        Attachment.add(MemsectionAttachment(env_ms, ms_kern_seg,
#                                            memsection_ms.attrs), memsection_ns)

#         pseudo_attach_el = ParsedElement('attachment')
#         pseudo_attach_el.name = memsection_ns.abs_name()
#         pseudo_attach_els.append(pseudo_attach_el)

    return pseudo_attach_els
