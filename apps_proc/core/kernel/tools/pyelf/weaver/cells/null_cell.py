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
Support class for a null cell that has no programs, but only a user specifiable kernel heap
"""

import re

from weaver.ezxml import ParsedElement, Element, str_attr, size_attr
from weaver import MergeError
from weaver.cells import AbstractCell
from weaver.segments_xml import make_pager_attr
from weaver.memobjs_xml import collect_memobj_attrs


NullCell_el = Element("null_cell",
                       name = (str_attr, "required"),
                       kernel_heap = (size_attr, "required"))

class NullCell(AbstractCell):
    """
    Cell for the null kernel.
    """

    element = NullCell_el

    def __init__(self):
        AbstractCell.__init__(self)

        self.domain = None
        self.heap = None

    def collect_xml(self, el, ignore_name, namespace, machine,
                    pools, kernel, image):
        """
        Parse XML elements and create an intermediate representation of
        the system.
        """
        self.name = el.name
        self.namespace = namespace.add_namespace(self.name)

        self.sas = pools.new_sas(self.namespace.abs_name())

        image.push_attrs(getattr(el, 'virtpool', None),
                         getattr(el, 'physpool', None),
                         make_pager_attr(getattr(el, 'pager', None)),
                         getattr(el, 'direct', None),
                         getattr(el, 'priority', None),
                         getattr(el, 'timeslice', None),
                         getattr(el, 'affinity', None))

        (self.def_virtpool, self.def_physpool) = image.current_pools()

        self.heap = kernel.register_heap(self.namespace, self.name, el.kernel_heap)
        #
        # Later in the merge process, symbols will be emitted with this name as
        # a substring. Ensure that the name is sane.
        #
        if not re.match("^[A-Za-z0-9_]+$", self.name):
            raise MergeError("The cell name '%s' contains invalid characters."
                    % self.name)

        self.image_group = image.add_group(self.sas)
        # All tags imply a new kdomain and kmmu.
        self.domain = self.heap.register_domain(self.namespace,
                getattr(el, 'max_caps', None), None, None, None,
                True, None, map_in_external_objects=False)

        pd_kern_mmu = self.domain.register_mmu_context(
            self.namespace, el.name,
            getattr(el, 'max_priority', None),
            getattr(el, 'l2_cache_size', None),
            getattr(el, 'num_vuas', None))

        image.pop_attrs()

    def generate_dynamic_segments(self, machine, image):
        """
        Create any segments, such as bootinfo buffers, whose size depends
        on the parsed XML.
        """
        pass

    def layout_cells_post(self, image, machine):
        pass

    def generate_init(self, machine, image):
        """
        Generate the init script for the cell, placing it into a segment.
        """
        pass

NullCell.register()
