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
Based on arch/armv5/kernel/arm-shared-domains/src/map_window.c
"""

from pyokl4.arch.armv5.okl4.arm_shared_domains.local import mmu_update_dacr
from pyokl4.arch.armv5.okl4.mmu_dynamic.internal import SIZE_1M
from pyokl4.okl4.kernel_base.bitmap import bitmap_isset, bitmap_set
from pyokl4.okl4.kernel_base.types import IS_ALIGNED

MAP_WINDOW_CTRL_USE_CALLBACK = 0x1

# TODO: The following function needs to be implemented
def arm_l2_gen_window(shared_mmu, ctrl):
    return None
# def arm_l2_gen_window(struct mmu_context *shared_mmu, word_t ctrl):
#     entry = arm_l2_gen_invalid()
#     entry = (word_t)shared_mmu & (~ARM_L2_SHARED_CALLBACK_MASK)
#     if (ctrl & MAP_WINDOW_CTRL_USE_CALLBACK) != 0:
#         entry |= ARM_L2_SHARED_CALLBACK_MASK

#     return entry


def share_mmu(mmu, shared_mmu):
    assert(not bitmap_isset(mmu.arm_shrdomains.shared_mmus, shared_mmu))
    bitmap_set(mmu.arm_shrdomains.shared_mmus, shared_mmu)

    assert(not bitmap_isset(shared_mmu.arm_shrdomains.client_mmus, mmu))
    bitmap_set(shared_mmu.arm_shrdomains.client_mmus, mmu)

    mmu_update_dacr(mmu, shared_mmu)

# TODO: The following function needs to be implemented
def map_window_1M(mmu, domain, virt, l2_entry):
    return 1
#   entry_ptr = arm_l1_entry(mmu.mmu.pgtable, virt)
#   entry = *entry_ptr;
#   if (is_arm_l1_valid(entry)) {
#       tree = arm_l1_subtree(entry);
#   } else {
#       tree = objmanager_allocate(domain, ARM_L2_SIZE, ARM_HWL1_ALIGN);
#       if (tree == NULL) {
#           return ERROR_NO_RESOURCES;
#       }
#       *entry_ptr = arm_l1_gen_subtree((word_t)tree);
#   }
#   entry_ptr = tree + arm_l2_index(virt);
#   entry = *entry_ptr;
#   if (is_arm_l2_valid(entry)) {
#       return ERROR_EXISTING_MAPPING;
#   }
#   if (is_arm_l2_shared_entry(entry)) {
#       return ERROR_EXISTING_MAPPING;
#   }

#   *entry_ptr = l2_entry;
#   mmu_sync_cpd(mmu, l2_entry, virt);
#   return 0

def mmu_map_window(mmu, domain, shared_mmu, virt, size, ctrl):
    if not IS_ALIGNED(size, SIZE_1M):
        return False#ERROR_INVALID_ARGUMENT
    if not IS_ALIGNED(virt, SIZE_1M):
        return False#ERROR_INVALID_ARGUMENT

    if not bitmap_isset(mmu.arm_shrdomains.shared_mmus, shared_mmu):
        share_mmu(mmu, shared_mmu)

    entry = arm_l2_gen_window(shared_mmu, ctrl)
    virt_end = virt + size
    for virt in range(virt, virt_end, SIZE_1M):
        # User can handle the error by unmap_window().
        ret = map_window_1M(mmu, domain, virt, entry)
        if ret != 0:
            return False#ret
    return 0

def map_window(current_domain, mmu, shared_mmu, virt, size, ctrl):
    ret = mmu_map_window(mmu, current_domain, shared_mmu, virt, size, ctrl)
    return ret
#   if ret:
#       return False
#   return True
