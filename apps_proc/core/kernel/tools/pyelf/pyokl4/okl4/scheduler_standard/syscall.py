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
Based on kernel/scheduler/src/syscall.c
"""

from pyokl4.okl4.scheduler_standard.scheduler import scheduler_unblock
from pyokl4.autogen import Autogen

def sys_thread_start(thread):
    """Go thread!  Go!"""
    # Ensure thread has not started
    assert not thread.scheduler.started

    # Start the thread and unblock it.
    thread.scheduler.started = True
    scheduler_unblock(thread)

def sys_priority_set(thread, priority):
    """Set the priority of the given thread."""

    #
    # In OKL4, we allow thread priorities to be modified at any point in
    # time through the syscall. To make the python implementation easier,
    # we only allow the priority to be set prior to the thread being
    # started.
    #
    # This could be modified at some later point in time if the need arises.
    #
    assert thread.scheduler.suspend_count & 1 != 0

    # If None, then leave it at the autogen default value.
    if priority is None:
        return

    # Ensure the priority is valid.
    assert priority < Autogen.scheduler_autogen.constants.NUM_PRIORITIES

    # Need to update both 'priority' and, if the scheduler is using
    # inheritance, 'base_priority'.
    thread.scheduler.priority = priority
    if hasattr(thread.scheduler, 'base_priority'):
        thread.scheduler.base_priority = priority

def sys_affinity_set(thread, affinity):
    """Set the affinity of the given thread."""

    # Only for multi-queue scheduler with affinity support
    assert hasattr(thread.scheduler, "affinity")

    # If None, then leave it at the autogen default value.
    if affinity is None:
        return

    #
    # In OKL4, we allow thread affinity to be modified at any point in
    # time through the syscall. To make the python implementation easier,
    # we only allow the affinity to be set prior to the thread being
    # started.
    #
    # This could be modified at some later point in time if the need arises.
    #
    assert thread.scheduler.suspend_count & 1 != 0
    thread.scheduler.affinity = affinity

def sys_timeslice_length_set(thread, timeslice_length):
    """Set the timeslice length of the thread."""

    if timeslice_length is not None:
        thread.scheduler.timeslice_length = timeslice_length
