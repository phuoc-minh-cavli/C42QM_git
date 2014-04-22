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

"""List of cap rights."""


# Master rights common to all objects
RIGHTS_MASTER = 1L
RIGHTS_NULL = 0L
RIGHTS_FULL = ~0L
RIGHTS_COPY = ~RIGHTS_MASTER

# Per-object rights
RIGHTS_DOMAIN_MASTER = 1L
RIGHTS_DOMAIN_CAPMGMT = 2L
RIGHTS_SEGMENT_MASTER = 1L
RIGHTS_SEGMENT_ATTACH = 2L
RIGHTS_SEGMENT_LOOKUP = 4L
RIGHTS_SEGMENT_USEASHEAP = 8L
RIGHTS_MUTEX_MASTER = 1L
RIGHTS_MUTEX_ACCESS = 2L
RIGHTS_HYBRID_MUTEX_MASTER = 1L
RIGHTS_HYBRID_MUTEX_ACCESS = 2L
RIGHTS_BARRIER_MASTER = 1L
RIGHTS_BARRIER_ACCESS = 2L
RIGHTS_PROFILE_CONTROL_MASTER = 1L
RIGHTS_TRACEBUFFER_CONTROL_MASTER = 1L
RIGHTS_PLATFORM_CONTROL_MASTER = 1L
RIGHTS_INTERRUPT_MASTER = 1L
RIGHTS_INTERRUPT_REGISTER = 2L
RIGHTS_SWITCH_TABLE_MASTER = 1L
RIGHTS_SWITCH_TABLE_CALL = 2L
RIGHTS_SWITCH_TABLE_CALL_LEGACY = 4L
RIGHTS_MESSAGE_QUEUE_MASTER = 1L
RIGHTS_MESSAGE_QUEUE_SEND = 2L
RIGHTS_MESSAGE_QUEUE_RECEIVE = 4L
RIGHTS_MESSAGE_QUEUE_BUFFER = 8L
RIGHTS_SEMAPHORE_MASTER = 1L
RIGHTS_SEMAPHORE_ACCESS = 2L
RIGHTS_PROCESSOR_CONTROL_MASTER = 1L
RIGHTS_THREAD_MASTER = 1L
RIGHTS_THREAD_NOTIFY = 2L
RIGHTS_THREAD_IPC = 4L
RIGHTS_THREAD_EXREGS = 8L
RIGHTS_THREAD_INTERRUPT = 16L
RIGHTS_THREAD_ABORT = 32L
RIGHTS_THREAD_VFP = 64L
RIGHTS_THREAD_REGISTERS = 128L
RIGHTS_MMU_CONTEXT_MASTER = 1L
RIGHTS_MMU_CONTEXT_ATTACH = 2L
RIGHTS_MMU_CONTEXT_SHARE = 4L
RIGHTS_MMU_CONTEXT_MANAGE = 8L
RIGHTS_MMU_CONTEXT_FLUSH = 16L
RIGHTS_MMU_CONTEXT_SWITCH = 32L
RIGHTS_INTLOCK_CONTROL_MASTER = 1L

