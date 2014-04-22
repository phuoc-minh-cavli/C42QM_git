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
Generic scheduler code that is shared between all scheduler implemntations.
"""

from pyokl4.okl4.kernel_base.list import LIST_ENQUEUE
from pyokl4.autogen import Autogen
from pyokl4.okl4.scheduler_standard import num_priorities

# These functions are replaced by scheduler specific (i.e. standard or smp) on
# a call to init()
scheduler_data_init = None
get_scheduler_queue = None

def set_runnable(thread):
    """Mark the thread as runnable"""

    thread.scheduler.suspend_count = thread.scheduler.suspend_count & ~0x1

def scheduler_unblock(thread):
    """Unblock the thread and add it to the run queue"""

    set_runnable(thread)

    scheduler_enqueue(thread)

def scheduler_enqueue(thread):
    """Add a thread to the scheduler queue."""

    scheduler_data_init()

    scheduler_queue = get_scheduler_queue(thread)

    LIST_ENQUEUE(scheduler_queue.priorities[thread.scheduler.priority],
            thread.scheduler.list_entry)

    set_bitmap(scheduler_queue, thread.scheduler.priority)

    if 'scheduler_metrics' in Autogen.modules:
        scheduler_queue.metrics.num_threads += 1

def set_bitmap(scheduler_queue, prio):
    """Set a bit in the scheduler's priority bitmap, indicating that at least
    one thread is present on the scheduler's queue at the given priority."""

    word_bits = Autogen.types.WORD_BITS
    prios = num_priorities()

    if prios > word_bits:
        lvl2 = prio / word_bits
        lvl1 = prio % word_bits

        # Set two-level bitmap.
        scheduler_queue.bitmap_l1 |= 1 << lvl2
        if scheduler_queue.bitmap_l2[lvl2] is None:
            scheduler_queue.bitmap_l2[lvl2] = 1 << lvl1
        else:
            scheduler_queue.bitmap_l2[lvl2] |= 1 << lvl1

        scheduler_queue.highest_prio = max(scheduler_queue.highest_prio, prio)
    elif prios > 1:
        # Set the one-level bitmap.
        scheduler_queue.bitmap |= 1 << prio

def init(module):
    """Initialise the scheduler"""

    global scheduler_data_init
    global get_scheduler_queue

    scheduler_data_init = module.scheduler_data_init
    get_scheduler_queue = module.get_scheduler_queue

