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

# Auto generated file - do not modify

"""
Information about the machine.
"""

cache_attribute_map = {
        "writeback" : (0, 6),
        "uncached" : (1, 8),
        "writethrough" : (2, 4),
        "strong" : (3, 0),
        "device_shared" : (4, 2),
        "writethrough_shared" : (5, 5),
        "writeback_shared" : (6, 7),
        "uncached_shared" : (7, 9),
        "writeback_alloc" : (8, 14),
        "writeback_alloc_shared" : (9, 15),
        "device" : (10, 16),
        "outer_only" : (11, 56),
        "outer_only_shared" : (12, 57),
        }



cache_attributes = {
        "outer_only" : cache_attribute_map["outer_only"][0],
        "writeback" : cache_attribute_map["writeback"][0],
        "uncached_shared" : cache_attribute_map["uncached_shared"][0],
        "uncached" : cache_attribute_map["uncached"][0],
        "device_shared" : cache_attribute_map["device_shared"][0],
        "writeback_alloc_shared" : cache_attribute_map["writeback_alloc_shared"][0],
        "outer_only_shared" : cache_attribute_map["outer_only_shared"][0],
        "writethrough" : cache_attribute_map["writethrough"][0],
        "writethrough_shared" : cache_attribute_map["writethrough_shared"][0],
        "device" : cache_attribute_map["device"][0],
        "strong" : cache_attribute_map["strong"][0],
        "writeback_shared" : cache_attribute_map["writeback_shared"][0],
        "writeback_alloc" : cache_attribute_map["writeback_alloc"][0],
        "writecombine" : cache_attribute_map["uncached"][0],
        "default" : cache_attribute_map["writeback_alloc"][0],
        "uncached" : cache_attribute_map["uncached"][0],
        "tracebuffer" : cache_attribute_map["writeback_alloc"][0],
        "writethrough" : cache_attribute_map["uncached"][0],
        "device" : cache_attribute_map["device"][0],
        "strong" : cache_attribute_map["strong"][0],
        }

NUM_CPUS = 1
KERNEL_VADDR_BASE = 4278190080L
KERNEL_HEAP_VADDR_END = 4293918720L
KERNEL_DEVICE_VADDR_BASE = 4293918720L
KERNEL_STACK_OFFSET = 272
KERNEL_STACK_SIZE = 1024
KERNEL_L1_ENTRIES = ((4294967295 - KERNEL_VADDR_BASE + 1) / 1048576)
VADDR_GLOBAL = (KERNEL_VADDR_BASE + KERNEL_STACK_OFFSET +
                (KERNEL_STACK_SIZE * NUM_CPUS))
VADDR_SPACE_BASE = 0L
VADDR_SPACE_SIZE = 4294967296L
USER_VIRT_END = 4278190080L

# Page sizes are sored in descending order to make elfweaver's min_page_size()
# and superpage_alignment's lives easier.
PAGE_SIZES = [16777216, 1048576, 65536, 4096]
WORD_SIZE = 32
L1_CACHE_LINE_SIZE = 32
L2_CACHE_LINE_SIZE = 32
