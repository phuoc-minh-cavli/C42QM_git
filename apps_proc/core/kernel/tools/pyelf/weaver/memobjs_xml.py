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
"""
Processing of the memory object XML tags.
"""

from weaver import MergeError
import weaver.image
from weaver.ezxml import Element, long_attr, bool_attr, str_attr, size_attr
from weaver.segments_xml import attach_to_elf_flags, make_pager_attr, \
         translate_mem_types
from weaver.util import check_file_exists


Memsection_el = Element("memsection",
                   name = (str_attr, "required"),
                   file = (str_attr, "optional"),
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

Stack_el = Element("stack",
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

def collect_memobj_attrs(el, namespace, image, machine,
                         name_is_tag = False, representing = None):
    """Collect the attributes of a memory section like element."""

    assert el is not None

    if name_is_tag or el.tag in ("stack", "heap", "thread_pool",
                                 "tracebuffer"):
        name = el.tag
    else:
        assert el.name is not None
        name = el.name

    values = image.new_attrs(namespace.add_namespace(name, representing))

    values.file = getattr(el, 'file', values.file)
    if values.file is not None:
        filename = check_file_exists(values.file, el._path)
        values.file = filename

    values.size = getattr(el, 'size', values.size)
    values.set_default_virt_addr(getattr(el, 'virt_addr', None))
    values.phys_addr = getattr(el, 'phys_addr', values.phys_addr)
    values.direct = getattr(el, 'direct', values.direct)
    values.virtpool = getattr(el, 'virtpool', values.virtpool)
    values.physpool = getattr(el, 'physpool', values.physpool)
    values.align = getattr(el, 'align', values.align)
    values.scrub = getattr(el, 'zero', values.scrub)
    values.cache_policy_mask = getattr(el, 'cache_policy_mask',
                                       values.cache_policy_mask)

    if hasattr(el, 'mem_type'):
        values.mem_type = translate_mem_types(el.mem_type)

    if hasattr(el, 'attach'):
        values.attach = attach_to_elf_flags(el.attach)

    if hasattr(el, 'pager'):
        values.pager = make_pager_attr(el.pager)

    if hasattr(el, 'cache_policy'):
        values.cache_attr = machine.get_cache_attr(el.cache_policy)

    return values
