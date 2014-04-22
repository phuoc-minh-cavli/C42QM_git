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
Processing of the machine XML element.
"""

from weaver.ezxml import Element, long_attr, str_attr, size_attr, bool_attr

Region_el = Element("region",
                    base = (long_attr, "required"),
                    size = (size_attr, "required"),
                    rights = (str_attr, "optional"),
                    cache_policy = (str_attr, "optional"),
                    overlap = (bool_attr, "optional"))

VirtualMemory_el = Element("virtual_memory",
                            Region_el,
                            name = (str_attr, "required"))

PhysicalMemory_el = Element("physical_memory",
                            Region_el,
                            name = (str_attr, "required"))

HashSize_el = Element("hash_size",
                        size = (size_attr, "required"))

HashOverHeadSize_el = Element("hash_overhead_size",
                        size = (size_attr, "required"))

Interrupt_el = Element("interrupt",
                            name = (str_attr, "required"),
                            number = (long_attr, "required"))

PhysicalDevice_el = Element("phys_device",
                            PhysicalMemory_el,
                            Interrupt_el,
                            name = (str_attr, "required"))

KernelHeapAttrs_el = Element("kernel_heap_attrs",
                             align = (size_attr, "optional"))

CachePolicy_el = Element("cache_policy",
                         name = (str_attr, "required"),
                         value = (long_attr, "required"))

CPU_el = Element("cpu",
                 name = (str_attr, "required"),
                 num = (long_attr, "optional"))

CacheAttr_el = Element("cache_attribute",
                 l1_dcache_way_size = (long_attr, "optional"))

ISA_el = Element("isa",
                 name = (str_attr, "required"))

Board_el = Element("board",
                   name = (str_attr, "required"))

ImageType_el = Element("image_type",
                   binary = (bool_attr, "required"),
                   physical = (bool_attr, "optional"),
                   physical_entry = (bool_attr, "optional"),
                   remove_nobits = (bool_attr, "optional"),
                   strip = (bool_attr, "optional"))

Machine_el = Element("machine",
                     ISA_el,
                     CPU_el,
                     CacheAttr_el,
                     Board_el,
                     PhysicalDevice_el,
                     VirtualMemory_el,
                     HashSize_el,
                     HashOverHeadSize_el,
                     PhysicalMemory_el,
                     KernelHeapAttrs_el,
                     CachePolicy_el,
                     ImageType_el,
                     file = (str_attr, "optional"))
