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
Based on kernel/scheduler_pico/src/scheduler.c
"""

from pyokl4.autogen import Autogen
import pyokl4.kernel_data as kernel_data
import pyokl4.events as events

#pylint: disable-msg=W0603
PATCHED_SCHEDULER_DATA = None

def _get_scheduler_data():
    """
    Return a kernel data object pointing to our scheduler_data structure.
    """

    # Setup scheduler data if required.
    global PATCHED_SCHEDULER_DATA

    if PATCHED_SCHEDULER_DATA is None:
        data = kernel_data.get_kernel_data()
        PATCHED_SCHEDULER_DATA = data.create_object(
                "_global",
                Autogen.types.OKL4_scheduler_data,
                offset = Autogen.types.OKL4_global.get_offset_of("scheduler_data"))

    return PATCHED_SCHEDULER_DATA

def scheduler_start_thread(thread):
    """Mark the thread as runnable."""
    scheduler_data = _get_scheduler_data()
    tid = Autogen.objmanager_autogen.objmanager_get_thread_index(thread)
    scheduler_data.potentially_runnable |= (1 << tid)
    scheduler_data.not_donating_priority |= (1 << tid)

def scheduler_sort_threads(**kwargs):
    """Sort the threads into descending order by priority, as expected by the
    pico scheduler."""
    # Extract the list of threads and where they live.
    # Ensure that we only have one kernel heap, domain and mmu_context.
    kernel = kwargs['kernel']
    threads = None

    for heap in kernel.heaps.values():
        for domain in heap.domains:
            for mmu in domain.mmu_contexts:
                if threads is not None:
                    raise Exception(
                        "Only one mmu_context is allowed with this scheduler!")
                threads = mmu.threads

    assert threads is not None

    # Input: the threads array. This is an array of threads with
    # XML-specified priorities.

    # Output: the threads array (modified in place so that existing
    # references pick up the updated array).
    # The new array has the threads in descending order of their priority.
    # The scheduler expects the highest priority thread to be the first in
    # the array.

    # Construct a mapping of old priorities to threads, popping off the old
    # list.
    priority_to_thread = {}

    while len(threads) > 0:
        thread = threads.pop()

        if priority_to_thread.has_key(thread.priority):
            raise Exception(
                "More than one thread exists at priority %d" % thread.priority)

        priority_to_thread[thread.priority] = thread

    # Reconstruct the same list from our sorted version.
    for prio in sorted(priority_to_thread.keys(), reverse=True):
        threads.append(priority_to_thread[prio])

def sched_init():
    """Initialise scheduler syscall event handlers."""
    events.subscribe("kernel_objects_collected", scheduler_sort_threads)

    global PATCHED_SCHEDULER_DATA
    PATCHED_SCHEDULER_DATA = None
