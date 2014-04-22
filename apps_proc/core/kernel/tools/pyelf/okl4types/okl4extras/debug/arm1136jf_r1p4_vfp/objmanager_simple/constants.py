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
Object constants mapping functions.
"""

# The design below requires many return statements.
#pylint: disable-msg=R0911

from pyokl4.autogen import Autogen

# Object type constants used internally in the C kernel source.
OBJTYPE_INVALID = 0
OBJTYPE_BARRIER = 1
OBJTYPE_DOMAIN = 2
OBJTYPE_HYBRID_MUTEX = 3
OBJTYPE_INTERRUPT = 4
OBJTYPE_INTLOCK_CONTROL = 5
OBJTYPE_MESSAGE_QUEUE = 6
OBJTYPE_MMU_CONTEXT = 7
OBJTYPE_MUTEX = 8
OBJTYPE_PLATFORM_CONTROL = 9
OBJTYPE_PROCESSOR_CONTROL = 10
OBJTYPE_PROFILE_CONTROL = 11
OBJTYPE_SEGMENT = 12
OBJTYPE_SEMAPHORE = 13
OBJTYPE_SWITCH_TABLE = 14
OBJTYPE_THREAD = 15
OBJTYPE_TRACEBUFFER_CONTROL = 16

# Number of kernel object types.
NUM_OBJECT_TYPES = 17

def get_obj_from_constant(constant):
    """Mapping from constant to object type."""
    if constant == OBJTYPE_PLATFORM_CONTROL:
        return Autogen.types.OKL4_platform_control
    if constant == OBJTYPE_MMU_CONTEXT:
        return Autogen.types.OKL4_mmu_context
    if constant == OBJTYPE_SWITCH_TABLE:
        return Autogen.types.OKL4_switch_table
    if constant == OBJTYPE_SEGMENT:
        return Autogen.types.OKL4_segment
    if constant == OBJTYPE_BARRIER:
        return Autogen.types.OKL4_barrier
    if constant == OBJTYPE_INTLOCK_CONTROL:
        return Autogen.types.OKL4_intlock_control
    if constant == OBJTYPE_PROCESSOR_CONTROL:
        return Autogen.types.OKL4_processor_control
    if constant == OBJTYPE_HYBRID_MUTEX:
        return Autogen.types.OKL4_hybrid_mutex
    if constant == OBJTYPE_MUTEX:
        return Autogen.types.OKL4_mutex
    if constant == OBJTYPE_MESSAGE_QUEUE:
        return Autogen.types.OKL4_message_queue
    if constant == OBJTYPE_SEMAPHORE:
        return Autogen.types.OKL4_semaphore
    if constant == OBJTYPE_TRACEBUFFER_CONTROL:
        return Autogen.types.OKL4_tracebuffer_control
    if constant == OBJTYPE_DOMAIN:
        return Autogen.types.OKL4_domain
    if constant == OBJTYPE_INTERRUPT:
        return Autogen.types.OKL4_interrupt
    if constant == OBJTYPE_PROFILE_CONTROL:
        return Autogen.types.OKL4_profile_control
    if constant == OBJTYPE_THREAD:
        return Autogen.types.OKL4_thread
    return None

def get_constant_from_obj(object_type):
    """Mapping from object type to constant."""
    if object_type == Autogen.types.OKL4_platform_control:
        return OBJTYPE_PLATFORM_CONTROL
    if object_type == Autogen.types.OKL4_mmu_context:
        return OBJTYPE_MMU_CONTEXT
    if object_type == Autogen.types.OKL4_switch_table:
        return OBJTYPE_SWITCH_TABLE
    if object_type == Autogen.types.OKL4_segment:
        return OBJTYPE_SEGMENT
    if object_type == Autogen.types.OKL4_barrier:
        return OBJTYPE_BARRIER
    if object_type == Autogen.types.OKL4_intlock_control:
        return OBJTYPE_INTLOCK_CONTROL
    if object_type == Autogen.types.OKL4_processor_control:
        return OBJTYPE_PROCESSOR_CONTROL
    if object_type == Autogen.types.OKL4_hybrid_mutex:
        return OBJTYPE_HYBRID_MUTEX
    if object_type == Autogen.types.OKL4_mutex:
        return OBJTYPE_MUTEX
    if object_type == Autogen.types.OKL4_message_queue:
        return OBJTYPE_MESSAGE_QUEUE
    if object_type == Autogen.types.OKL4_semaphore:
        return OBJTYPE_SEMAPHORE
    if object_type == Autogen.types.OKL4_tracebuffer_control:
        return OBJTYPE_TRACEBUFFER_CONTROL
    if object_type == Autogen.types.OKL4_domain:
        return OBJTYPE_DOMAIN
    if object_type == Autogen.types.OKL4_interrupt:
        return OBJTYPE_INTERRUPT
    if object_type == Autogen.types.OKL4_profile_control:
        return OBJTYPE_PROFILE_CONTROL
    if object_type == Autogen.types.OKL4_thread:
        return OBJTYPE_THREAD
    return -1

