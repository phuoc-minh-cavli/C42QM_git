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
Processing of the memstats XML element.
"""

from weaver.ezxml import Element, str_attr, size_attr

#############################################################################
#
# Declare the XML elements
#
#############################################################################
RevisionStats_el = Element("revision",
                           repository = (str_attr, "optional"),
                           changeset = (str_attr, "optional"))

#
# Construction of elements leading to environment
#
ID_Range_el = Element("id_range",
                      name = (str_attr, "required"),
                      node = (str_attr, "required"),
                      used = (size_attr, "optional"))

Pool_el = Element("pool",
                  id = (str_attr, "required"),
                  vmid = (str_attr, "required"))

Thread_el = Element("thread",
                    pool_id = (str_attr, "required"),
                    handle_id = (str_attr, "required"),
                    clist_id = (str_attr, "required"))

Mutex_el = Element("mutex",
                   pool_id = (str_attr, "required"),
                   clist_id = (str_attr, "required"))

PSec_el = Element("psec",
                  name = (str_attr, "required"),
                  phys = (str_attr, "required"),
                  virt = (str_attr, "required"))

PSecList_el = Element("psec_list",
                      PSec_el)

HeapAlloc_el = Element("heap_alloc",
                        name   = (str_attr, "required"),
                        size   = (size_attr, "required"),
                        module = (str_attr, "required"))

HeapAllocList_el = Element("heap_alloc_list",
                            HeapAlloc_el)

Object_el = Element("object",
                     type = (str_attr, "required"),
                     name = (str_attr, "required"),
                     size = (size_attr, "required"))

ObjectList_el = Element("object_list",
                        Object_el,
                        num_text = (size_attr, "required"),
                        num_data = (size_attr, "required"))

Program_el = Element("program",
                      PSecList_el,
                      ObjectList_el,
                      name = (str_attr, "required"))

Space_el = Element("space",
                   ID_Range_el,
                   Pool_el,
                   Program_el,
                   Thread_el,
                   Mutex_el,
                   name = (str_attr, "required"))

Cell_el = Element("cell",
                  ID_Range_el,
                  Pool_el,
                  Space_el,
                  HeapAllocList_el,
                  name = (str_attr, "required"))

Kernel_el = Element("kernel",
                  ID_Range_el,
                  Pool_el,
                  Space_el)

Environment_el = Element("environment",
                         Kernel_el,
                         Cell_el,
                         type = (str_attr, "optional"))

#
# Construction of elements leading to resources
#
Group_el = Element("group",
                   name = (str_attr, "required"),
                   type = (str_attr, "optional"),
                   size = (size_attr, "required"))

MemPool_el = Element("mem_pool",
                     Group_el,
                     id = (str_attr, "required"),
                     node = (str_attr, "required"),
                     block_size = (str_attr, "optional"))

VM_Section_el = Element("vm_section",
                        id = (str_attr, "required"),
                        address = (size_attr, "required"),
                        size = (size_attr, "required"))

VirtMem_el = Element("virt_mem",
                     VM_Section_el,
                     id = (str_attr, "required"),
                     sas_id = (str_attr, "required"))

Pools_el = Element("pools",
                   VirtMem_el,
                   MemPool_el)

Range_el = Element("range",
                   id = (str_attr, "required"),
                   start = (size_attr, "required"),
                   size = (size_attr, "required"))

ID_Pool_el = Element("id_pool",
                    Range_el,
                    name = (str_attr, "required"),
                    parent = (str_attr, "optional"),
                    source = (str_attr, "required"),
                    total = (size_attr, "required"))

IDs_el = Element("ids",
                 ID_Pool_el)

SAS_el = Element("sas",
                 id = (str_attr, "required"))

SAS_List_el = Element("sas_list",
                      SAS_el)

Segment_el = Element("segment",
                     sections = (str_attr, "required"),
                     phys = (size_attr, "required"),
                     virt = (size_attr, "required"),
                     filesz = (size_attr, "required"),
                     memsz = (size_attr, "required"))

SegmentList_el = Element("segment_list",
                         Segment_el)

Section_el = Element("section",
                     id = (str_attr, "required"),
                     type = (str_attr, "optional"),
                     address = (size_attr, "required"),
                     size = (size_attr, "required"))

SectionList_el = Element("section_list",
                          Section_el)

PhysMem_el = Element("phys_mem",
                     Section_el,
                     machine = (str_attr, "required"),
                     name = (str_attr, "required"),
                     pool = (str_attr, "required"),
                     page_size = (size_attr, "required"))

Resources_el = Element("resources",
                       PhysMem_el,
                       SectionList_el,
                       SegmentList_el,
                       SAS_List_el,
                       IDs_el,
                       Pools_el)

#
# And now put it altogether into memstats
#
Memstats_el = Element("memstats",
                       RevisionStats_el,
                       Environment_el,
                       Resources_el)
