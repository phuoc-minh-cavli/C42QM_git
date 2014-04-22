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
This module tracks the objects reported in tracepoint information.
"""

from weaver.perfmon.events import Constants, MutexEvent, SchedulerEvent, \
        ExceptionEvent

from struct import pack, unpack

(MUTEX_KERNEL, MUTEX_HYBRID) = range(2)
(CREATION_EXPLICIT, CREATION_IMPLICIT) = range(2)

MUTEX_TYPE_STRINGS = {
    MUTEX_KERNEL: "kernel",
    MUTEX_HYBRID: "hybrid"
    }

class OKL4_object(object):
    """
    Represents an object to be analysed within a running OKL4 system.
    All subclasses are expected to implement is_valid()
    """
    def __init__(self, id, raw_id, createtime):
        """
        Create the object and note the time in the system at which it
        was created
        """
        self.id = id
        self.raw_id = raw_id
        self.create = createtime
        self.delete = None
        self.valid = True
        self.events = []
        self.name = None

    def remove(self, deletetime):
        """
        Mark the time of the objects removal from the system.
        """
        self.delete = deletetime

    def add_event(self, event):
        self.events.append(event)

    def set_name(self, name):
        self.name = name

    def explicitly_created(self):
        return self.creation == CREATION_EXPLICIT

    def is_valid(self):
        raise NotImplementedError

class Mutex(OKL4_object):

    def __init__(self, id, raw_id, event):
        OKL4_object.__init__(self, id, raw_id, event.timestamp)

        if isinstance(event, MutexEvent):
            self.creation = CREATION_IMPLICIT

            if event.minor == Constants.kernel_mutex_events:
                self.mtype = MUTEX_KERNEL
            else:
                self.mtype = MUTEX_HYBRID
        else:
            self.creation = CREATION_EXPLICIT

            if event.eventtype == Constants.event.TP_EVT_SYS_MUTEX_CREATE:
                self.mtype = MUTEX_KERNEL
            else:
                self.mtype = MUTEX_HYBRID

    def add_event(self, event):
        OKL4_object.add_event(self, event)

        # Get associated thread
        event.thread_object = Objmanager.get_by_id(event.threadid)

    def is_valid(self):
        """
        Confirm that the sequence of events is as expected.
        """
        state = "unlocked"
        ignore = set(("increment", "decrement",
                     "syscall-lock", "syscall-unlock"))
        cycles = 0
        for event in self.events:
            if event.eventname == "acquire":
                if state != "unlocked":
                    return (False,
                            "Expect mutex to be unlocked, not '%s'" % state)
                else:
                    state = "held"
            elif event.eventname == "block":
                if cycles > 0 and state != "held":
                    return (False,
                            "Block while mutex not held")
            elif event.eventname == "release":
                state = "unlocked"
                cycles += 1
            elif event.eventname in ignore:
                pass
            else:
                return (False,
                        "Unexpected action: '%s'" % event.eventname)
        return (True, "ok")


class Thread(OKL4_object):
    def __init__(self, id, raw_id, event):
        OKL4_object.__init__(self, id, raw_id, event.timestamp)
        if isinstance(event, SchedulerEvent) or \
                isinstance(event, ExceptionEvent):
            self.creation = CREATION_IMPLICIT
        else:
            self.creation = CREATION_EXPLICIT

    def add_event(self, event):
        #
        # Thread needs its own add_event as context switch events need to be
        # applied to two Thred instances: the thread being switched from
        # (which is where it is added in this call) and the thread being
        # switched to (which is the call to add_switch() made in this
        # function)
        #
        OKL4_object.add_event(self, event)
        if isinstance(event, SchedulerEvent):
            if event.eventtype == Constants.event.TP_EVT_SCH_CONTEXT_SWITCH:
                # Create dummy event to allow us to get the "to" thread
                data = (event.data[1], event.data[0])
                event_copy = SchedulerEvent(event.timestamp, event.eventtype, \
                                                event.major, event.minor, data)
                # And add the event to the "to" thread of the context switch.
                Objmanager.get(event_copy).add_switch(event)

    def add_switch(self, event):
        OKL4_object.add_event(self, event)

    def is_valid(self):
        bstate = "blocked"
        sstate = False
        if self.creation == CREATION_IMPLICIT:
            block_trip = True # Can have one tripup from implicit creation
            switch_trip = True
        else:
            block_trip = False
            switch_trip = False

        for event in filter(lambda x: isinstance(x, SchedulerEvent),
                            self.events):
            if event.eventname == "block":
                if bstate == "blocked":
                    if block_trip:
                        block_trip = False
                    else:
                        return (False,
                                "Cannot block while already blocked")
                bstate = "blocked"
            elif event.eventname == "unblock":
                if bstate != "blocked":
                    return (False,
                            "Cannot unblock unless blocked")
                bstate = "unblocked"
            elif event.eventname == "switch":
                if (sstate and event.from_thread != self.raw_id) or \
                        (not sstate and event.to_thread != self.raw_id):
                    if switch_trip:
                        switch_trip = False
                    else:
                        return (False,
                                "Trying to switch thread to the state it's " +
                                "already in")
                sstate = (event.to_thread == self.raw_id)
            else:
                return (False,
                        "Unexpected action: '%s'" % event.eventname)

        for event in filter(lambda x: isinstance(x, ExceptionEvent),
                            self.events):
            pass

        return (True, "ok")


class Objmanager(object):
    """
    Manage the lifecycle of the OKL4 objects.
    Objects are stored in lists of chains, with the primary identifier of
    the chain being the pointer address from the OKL4 kernel to the object.
    As this can be recycled (by deleting an object and allocating a new one
    at the same address) all instances which existed at that address are
    stored, with a '-{count}' appended.  At any time, only one object in a
    chain should have a value of None for object.delete (indicating it is
    currently in use).
    """
    factories = None
    implicit_factories = None
    naming = None
    destructors = set()
    _objects = {}

    @classmethod
    def init_factories(cls):
        cls.factories = {
            Constants.event.TP_EVT_OBJ_THREAD_CREATE : Thread,
            Constants.event.TP_EVT_OBJ_MUTEX_CREATE : Mutex,
            Constants.event.TP_EVT_OBJ_HYBRID_MUTEX_CREATE : Mutex,
            }
        cls.implicit_factories = {
            Constants.major.TRACEPOINT_MAJOR_MUTEX: Mutex,
            Constants.major.TRACEPOINT_MAJOR_SCHEDULER: Thread,
            Constants.major.TRACEPOINT_MAJOR_EXCEPTION: Thread,
            }

        cls.destructors = \
                set((Constants.event.TP_EVT_OBJ_THREAD_DELETE,
                     Constants.event.TP_EVT_OBJ_MUTEX_DELETE,
                     Constants.event.TP_EVT_OBJ_HYBRID_MUTEX_DELETE))

        cls.naming = Constants.event.TP_EVT_KDB_SET_OBJECT_NAME

    @classmethod
    def new_chain(cls, event, factory):
        name = "%d-%d" % (event.id(), 0)
        chain = [factory(name, event.id(), event), ]
        cls._objects[event.id()] = chain

    @classmethod
    def latest(cls, event):
        retval =  [o for o in cls._objects[event.id()] if o.delete is None]
        return retval

    @classmethod
    def explicit_action(cls, event):
        """
        The event passed in is a lifecycle event - either a create or a delete.
        Update the collection of active objects accordingly.
        """

        if event.eventtype in cls.factories:
            if event.id() not in cls._objects:
                cls.new_chain(event, cls.factories[event.eventtype])
            else:
                chain = cls._objects[event.id()]
                count = len(chain)
                name = "%d-%d" % (event.id(), count)
                chain.append(factory(name, event.id(), event))
        elif event.eventtype in cls.destructors:
            latest = cls.latest(event)

            assert(len(latest) == 1)
            latest[0].remove(event.timestamp)
        elif event.eventtype == cls.naming:
            if event.id() not in cls._objects:
                # At least for now, can't create an object off name
                # event alone, so discard
                pass
            else:
                latest = cls.latest(event)
                assert(len(latest) == 1)
                assert(len(event.data) == 4)
                (w0, w1, w2, w3) = event.data
                (name,) = unpack("11s",
                                 pack("III", w1, w2, w3)[0:11])
                latest[0].set_name(name)


    @classmethod
    def get(cls, event):
        """
        Get the OKL4 object associated with the tracepoint event passed in.
        This may implicitly create the object.
        """
        #
        # Note: You can only _ever_ implicitly create one object for a given
        # id
        #
        if event.id() not in cls._objects:
            cls.new_chain(event, cls.implicit_factories[event.major])

        latest = [o for o in cls._objects[event.id()] if o.delete is None]
        assert(len(latest) == 1)

        return latest[0]

    @classmethod
    def get_by_id(cls, eventid):
        """
        Get the OKL4 object associated with the tracepoint event passed in.
        This may implicitly create the object.
        """
        assert(eventid in cls._objects)

        latest = [o for o in cls._objects[eventid] if o.delete is None]
        assert(len(latest) == 1)

        return latest[0]

    @classmethod
    def objects(cls):
        """
        Return all objects created.
        """
        #
        # Cannot use a generator here as the list needs to be reused
        # when multiple reports are specified
        #
        retval = []
        for items in cls._objects.values():
            for item in items:
                retval.append(item)
        return retval
