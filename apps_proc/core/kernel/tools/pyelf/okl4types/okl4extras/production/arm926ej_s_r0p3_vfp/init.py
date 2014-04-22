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
Initialisation functions for all kernel objects.
"""

# Unused arguments are hard to control in autogenerated files.
#pylint: disable-msg=W0613

from pyokl4.okl4.kernel_base.list import LIST_HEAD_INIT

def init_barrier_object(barrier):
    """Initialise the barrier object."""
    LIST_HEAD_INIT(barrier.barrier.syncpoint.donators)

def init_domain_object(domain):
    """Initialise the domain object."""
    LIST_HEAD_INIT(domain.objmanager.attachments)
    domain.scheduler.max_prio = 255L

def init_hybrid_mutex_object(hybrid_mutex):
    """Initialise the hybrid_mutex object."""
    LIST_HEAD_INIT(hybrid_mutex.condvar.wait_queue.donators)
    LIST_HEAD_INIT(hybrid_mutex.hybrid_mutex.syncpoint.donators)

def init_interrupt_object(interrupt):
    """Initialise the interrupt object."""
    pass

def init_intlock_control_object(intlock_control):
    """Initialise the intlock_control object."""
    pass

def init_message_queue_object(message_queue):
    """Initialise the message_queue object."""
    LIST_HEAD_INIT(message_queue.msgq.syncpoint.donators)
    LIST_HEAD_INIT(message_queue.notify_buffer_set.notifiees)
    LIST_HEAD_INIT(message_queue.notify_nonempty.notifiees)
    LIST_HEAD_INIT(message_queue.notify_nonfull.notifiees)

def init_mmu_context_object(mmu_context):
    """Initialise the mmu_context object."""
    LIST_HEAD_INIT(mmu_context.buff_area.syncpoint.donators)
    mmu_context.mmu.first_used_vaddr = 4294967295L
    mmu_context.mmu.pgtable_phys = lambda: 0

def init_mutex_object(mutex):
    """Initialise the mutex object."""
    LIST_HEAD_INIT(mutex.mutex.condvar_wait_queue.donators)
    LIST_HEAD_INIT(mutex.mutex.syncpoint.donators)

def init_platform_control_object(platform_control):
    """Initialise the platform_control object."""
    pass

def init_processor_control_object(processor_control):
    """Initialise the processor_control object."""
    pass

def init_profile_control_object(profile_control):
    """Initialise the profile_control object."""
    pass

def init_segment_object(segment):
    """Initialise the segment object."""
    pass

def init_semaphore_object(semaphore):
    """Initialise the semaphore object."""
    LIST_HEAD_INIT(semaphore.semaphore.syncpoint.donators)

def init_switch_table_object(switch_table):
    """Initialise the switch_table object."""
    pass

def init_thread_object(thread):
    """Initialise the thread object."""
    thread.hal.reg.cpsr = 16L
    thread.callback.mode = 0L
    LIST_HEAD_INIT(thread.critsect.syncpoint.donators)
    LIST_HEAD_INIT(thread.hybrid_mutex.head)
    LIST_HEAD_INIT(thread.ipc.defunct_reply_tokens)
    LIST_HEAD_INIT(thread.ipc.rcv_syncpoint.donators)
    LIST_HEAD_INIT(thread.ipc.snd_syncpoint.donators)
    thread.ipc.token_count = 1L
    LIST_HEAD_INIT(thread.mutex.head)
    thread.scheduler.current_timeslice = 10000L
    LIST_HEAD_INIT(thread.scheduler.held_syncpoints)
    thread.scheduler.suspend_count = 1L
    thread.scheduler.timeslice_length = 10000L
    thread.switch_table.caller_mode = 0L

def init_tracebuffer_control_object(tracebuffer_control):
    """Initialise the tracebuffer_control object."""
    pass

