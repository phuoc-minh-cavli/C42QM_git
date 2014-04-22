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

"""Tracebuffer event parsing and creation code."""

from base64 import b64decode
from struct import unpack, calcsize

from pyokl4.autogen import Autogen

class TraceEvent(object):
    """Representation of a generic tracebuffer event."""

    def __init__(self, timestamp, eventtype, major, minor, data):
        self.timestamp = timestamp
        self.eventtype = eventtype
        self.major = major
        self.minor = minor
        self.data = data

    def id(self):
        raise NotImplementedError

class ObjectManagerEvent(TraceEvent):

    def __init__(self, timestamp, eventtype, major, minor, data):
        TraceEvent.__init__(self, timestamp, eventtype, major, minor, data)

    def id(self):
        # WARNING: This only makes sense for the subset of object manager
        # events we care about and are set up the right way.
        return self.data[0]

class MutexEvent(TraceEvent):
    """Representation of one of the mutex events defined in the tracebuffer."""

    events = None

    @classmethod
    def init_constants(cls):
        # FIXME: Jira ticket OLD-1684 - peterh
        cls.events = \
            { Constants.event.TP_EVT_MTX_MUTEX_ACQUIRE : ("acquire", 3),
              Constants.event.TP_EVT_MTX_MUTEX_RELEASE : ("release", 2),
              Constants.event.TP_EVT_MTX_MUTEX_INCREMENT : ("increment", 3),
              Constants.event.TP_EVT_MTX_MUTEX_DECREMENT : ("decrement", 2),
              Constants.event.TP_EVT_MTX_MUTEX_BLOCK : ("block", 3),
              Constants.event.TP_EVT_MTX_HYBRID_MUTEX_ACQUIRE : ("acquire", 3),
              Constants.event.TP_EVT_MTX_HYBRID_MUTEX_RELEASE : ("release", 2),
              Constants.event.TP_EVT_MTX_HYBRID_MUTEX_BLOCK : ("block", 3)
              }

    def __init__(self, timestamp, eventtype, major, minor, data):
        TraceEvent.__init__(self, timestamp, eventtype, major, minor, data)

        (self.eventname, self.argc) = MutexEvent.events[self.eventtype]
        assert(len(data) == self.argc)

        # Order of data is mutexid, threadid
        (self.mutexid, self.threadid) = data[0:2]
        self.thread_object = None

    def id(self):
        return self.mutexid

class SchedulerEvent(TraceEvent):
    """
    Representation of the scheduler events defined in the tracebuffer.  Unlike
    mutex events (which act on mutex objects) the scheduler events act on
    thread objects.
    """

    events = None

    @classmethod
    def init_constants(cls):
        # FIXME: Jira ticket OLD-1684 - peterh
        cls.events = \
            {
            Constants.event.TP_EVT_SCH_SCHEDULER_BLOCK : ("block", 1),
            Constants.event.TP_EVT_SCH_SCHEDULER_UNBLOCK : ("unblock", 1),
            Constants.event.TP_EVT_SCH_CONTEXT_SWITCH : ("switch", 2),
            }

    def __init__(self, timestamp, eventtype, major, minor, data):
        TraceEvent.__init__(self, timestamp, eventtype, major, minor, data)
        (self.eventname, self.argc) = SchedulerEvent.events[self.eventtype]
        self.threadid = data[0]
        self.from_thread = self.threadid
        if eventtype == Constants.event.TP_EVT_SCH_CONTEXT_SWITCH:
            self.to_thread = data[1]

    def id(self):
        return self.threadid

class ExceptionEvent(TraceEvent):
    """
    Representation of the exception events defined in the tracebuffer.  These
    act on thread objects.
    """

    events = None

    @classmethod
    def init_constants(cls):
        # FIXME: Jira ticket OLD-1684 - peterh
        cls.events = \
            {
            Constants.event.TP_EVT_EXC_SWI_SYSCALL : ("syscall", 2),
            Constants.event.TP_EVT_EXC_PAGEFAULT : ("pagefault", 1),
            }

    def __init__(self, timestamp, eventtype, major, minor, data):
        TraceEvent.__init__(self, timestamp, eventtype, major, minor, data)
        (self.eventname, self.argc) = ExceptionEvent.events[self.eventtype]
        self.threadid = data[0]
        if self.eventtype == Constants.event.TP_EVT_EXC_SWI_SYSCALL:
            assert(len(data) > 1)
            self.syscallid = data[1]

    def id(self):
        return self.threadid

#
# These structures cannot be setup until the Autogen module has been
# initialised.
#
class Constants(object):
    """
    A collection of various constant values identifying tracepoint entries.
    This includes:
        * Event ids
        * Major numbers
        * Minor numbers
    """
    events = None
    kernel_mutex_events = None
    hybrid_mutex_events = None
    event_factory = None
    event = None
    major = None
    minor = None
    ignore = set()

    @classmethod
    def init_constants(cls):
        cls.event = Autogen.types.OKL4_tracepoint_event
        cls.major = Autogen.types.OKL4_tracepoint_major
        cls.minor = Autogen.types.OKL4_tracepoint_minor
        cls.syscalls = Autogen.types.Syscall_IDs

        MutexEvent.init_constants()
        SchedulerEvent.init_constants()
        ExceptionEvent.init_constants()

        cls.kernel_mutex_events = cls.minor.TRACEPOINT_MINOR_MUTEX_OPS
        cls.hybrid_mutex_events = cls.minor.TRACEPOINT_MINOR_HYBRID_MUTEX_OPS

        cls.event_factory = \
            { cls.major.TRACEPOINT_MAJOR_MUTEX : MutexEvent,
              cls.major.TRACEPOINT_MAJOR_OBJECT_MANAGER : ObjectManagerEvent,
              cls.major.TRACEPOINT_MAJOR_SCHEDULER : SchedulerEvent,
              cls.major.TRACEPOINT_MAJOR_EXCEPTION : ExceptionEvent,
            }

        #
        # From a reporting perspective, the various syscall versions of
        # tracepoint events should be ignored (there are equivalent
        # follow up events which have the desired data).
        #
        cls.ignore = set((
                cls.event.TP_EVT_SYS_THREAD_CREATE,
                cls.event.TP_EVT_SYS_DOMAIN_CREATE,
                cls.event.TP_EVT_SYS_MUTEX_CREATE,
                cls.event.TP_EVT_SYS_HYBRID_MUTEX_CREATE,
                cls.event.TP_EVT_SYS_MMU_CONTEXT_CREATE,
                cls.event.TP_EVT_SYS_SEGMENT_CREATE,
                cls.event.TP_EVT_SYS_MESSAGE_QUEUE_CREATE,
                cls.event.TP_EVT_SYS_SEMAPHORE_CREATE,
                cls.event.TP_EVT_SYS_CHANNEL_CREATE,
                cls.event.TP_EVT_SYS_MUTEX_LOCK,
                cls.event.TP_EVT_SYS_MUTEX_UNLOCK,
                cls.event.TP_EVT_SYS_HYBRID_MUTEX_LOCK,
                cls.event.TP_EVT_SYS_HYBRID_MUTEX_UNLOCK,
                ))
