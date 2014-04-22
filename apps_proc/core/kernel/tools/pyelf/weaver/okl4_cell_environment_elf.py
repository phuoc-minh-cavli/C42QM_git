##############################################################################
# Australian Public Licence B (OZPLB)
# 
# Version 1-0
# 
# Copyright (c) 2006-2011, Open Kernel Labs, Inc.
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

"""Binary representations of cell environment entries."""

from weaver.util import _0, log2, get_symbol
from weaver.cells.collection.pd import ExtensionAttachment
from pyokl4.autogen import Autogen
import weaver.weavertypes
from pyokl4.okl4.kernel_base.list import LIST_HEAD_INIT, LIST_ENQUEUE
import pyokl4.okl4.futex.futex as futex
from pyokl4.libokl4.realms import realms
from weaver.cap import Rights, Value
from weaver import MergeError
from weaver.binarywriter import BinaryWriter
from weaver.physreloc import PhysReloc
from elf.util import align_down, align_up

#
# CellEnvironmentEntry objects act as proxies for the underlying
# libokl4 object that will be written into the environment.  These
# objects have two purposes:
#
# 1) To calculate the size of the libokl4 object prior to addresses or
# kernel caps being calculated, and,
#
# 2) Creating the final libokl4 object.
#
# Both of these purposes are performed by the create() and
# write_entry() methods.
#
# The create() method is called to initialise the libokl4 object,
# which must be stored in the okl4_obj field.  The virtual address
# where the libokl4 object will reside is passed as a parameter to
# simplify the initialisation of pointers.  This method is called
# *twice*, once, with a random address, so that the size of the
# libokl4 object can be calculated, and then a second time with the
# correct address.  It is recommended that the libokl4 object be
# created and reinitialised each time.
#
# The write_entry() method is called to write out a binary version of
# the entry.  It is also called twice immediately after the create()
# methods of every object in the environment has been called.  This
# method is passed a BinaryWriter object into which the binary data
# must be written.  The size of the data written to this writer is
# taken to be the size of the libokl4 object.
#
# The environment is written out in two formats both of which use the
# same format for the libokl4 object.  The first format stores
# string->object pairs and every object recorded in the environment is
# written out.  The user then finds the object by passing the
# appropriate string to the libokl4 environment code.
#
# The second format uses specially formatted global variables and ELF
# sections in the client ELF file to specify the desired environment
# entries.  This format is much more compact because strings are not
# written into the environment, nor are unwanted environment entries.
#
# However, if an libokl4 object depends on another libokl4 object in
# the environment there is a danger in the new format that the second
# object will not be written out because it is not directly
# referenced.  This is solved by the code for the new environment
# calling the mark_as_used() method o each directly referenced
# CellEnvironmentEntry object.  It in turn calls
# mark_children_as_used().  Objects with dependencies are expected to
# implement this method to call mark_as_used() n all of its
# dependencies.  All used objects will be written out in the new
# format.
#

class CellEnvironmentEntry(object):
    """
    Base class for all environment entries.
    """
    child = property()

    def __init__(self):
        self.address = None
        self.okl4_obj = None
        self.used = False

        # for convenience
        self.word_size = Autogen.types.OKL4_word_t._type_size
        self.endianess = weaver.weavertypes._ELFWEAVER_TYPES_ENDIANESS

    def get_size(self):
        """Return the number of bytes in this structure."""
        if self.okl4_obj:
            return self.okl4_obj._type_size
        else:
            raise MergeError, "Not implemented for this Environment Entry"

    def set_address(self, address):
        """Sets the address at which this environment entry is written out."""
        self.address = address

    def get_address(self):
        """Gets the address at which this environment entry is written out."""
        return self.address

    def create(self, address, sas):
        """Create the data that will go in the entry."""
        raise NotImplementedError

    def mark_as_used(self):
        """Ensures that this environment entry is placed in the environment."""
        if self.used:
            # Don't recurse
            return

        self.used = True
        self.mark_children_as_used()

    def mark_children_as_used(self):
        """Ensures that any objects that this entry refers to are also placed
        in the environment. This should be overridden by sub-classes which
        write out pointers to other environment entries."""
        pass

    def is_used(self):
        return self.used

    def write_entry(self, section, sas):
        """
        Record any phys relocations in the entry and then write out the form
        of the entry.
        """
        section.record_phys_relocs(self.okl4_obj.get_phys_relocs())

        section.write_output(self.okl4_obj.to_data())

    def get_libokl4_type_name(self):
        """Get the libokl4 type name of the entry (e.g. "okl4_mutex_t")"""
        # Extract it from our okl4_obj. This only works after create() has been
        # called. If our okl4_obj is a derived type, it will already have the _t
        # appended. However at this point, there is no way to tell if the okl4_obj
        # is a derived type, other than check the suffix.
        if self.okl4_obj._type_libokl4_name.endswith("_t"):
            return "okl4_%s" % self.okl4_obj._type_libokl4_name
        else:
            return "okl4_%s_t" % self.okl4_obj._type_libokl4_name

class CellEnvKCap(CellEnvironmentEntry):
    """
    Environment entry consisting of a single cap.
    """
    def __init__(self, cap):
        CellEnvironmentEntry.__init__(self)
        self.object = cap

    def create(self, address, sas):
        """Create the data that will go in the entry."""
        self.okl4_obj = Autogen.types.OKL4_designator_t(address = address)
        self.okl4_obj.value = self.object.cap_id

class CellEnvVInterruptLine(CellEnvironmentEntry):
    """
    Environment entry consisting of the cap of the virtual interrupt line.
    """
    def __init__(self, kinterline):
        CellEnvironmentEntry.__init__(self)
        self.kinterline = kinterline

    def create(self, address, sas):
        """Create the data that will go in the entry."""
        self.okl4_obj = Autogen.types.OKL4_designator_t(address = address)

        if self.kinterline.cap_object is not None:
            self.okl4_obj.value = self.kinterline.cap_object.get_cap_value()
        else:
            self.okl4_obj.value = 0

class CellEnvVirq(CellEnvironmentEntry):
    """
    Environment entry consisting of a single cap.
    """
    def __init__(self, obj):
        CellEnvironmentEntry.__init__(self)
        self.irq_obj = obj

    def create(self, address, sas):
        """Create the data that will go in the entry."""
        self.okl4_obj = Autogen.types.OKL4_word_t(address = address)
        self.okl4_obj.value = self.irq_obj.irq

class CellEnvVServerEntry(CellEnvironmentEntry):
    """
    Environment entry representing a vserver object

    okl4_vserver_info_t {
        okl4_word_t num_clients;
        struct {
            okl4_channel_data_t *data;
            okl4_word_t max_messages;
            okl4_word_t message_size;
        } channels;
    }

    okl4_channel_data_t {
        okl4_kcap_t kcap;
        okl4_word_t not_empty_irq;
        okl4_word_t not_full_irq;
    }

    This writes out the vserver_info object at the start followed by
    num_clients channel_data objects.  The data pointer in vserver_info
    points at the starting channel object.
    """
    def __init__(self, kern_channels):
        CellEnvironmentEntry.__init__(self)

        self.kern_channels = kern_channels
        self.channel_data = []

    def get_size(self):
        return Autogen.types.OKL4_vserver_info._type_size + \
                len(self.kern_channels) * Autogen.types.OKL4_channel_data._type_size

    def create(self, address, sas):
        """Create the data that will go in the entry."""
        self.okl4_obj = Autogen.types.OKL4_vserver_info(address = address)
        self.okl4_obj.num_clients = len(self.kern_channels)
        self.okl4_obj.channels.max_messages = self.kern_channels[0].max_messages
        self.okl4_obj.channels.message_size = self.kern_channels[0].message_size

        address = address + Autogen.types.OKL4_vserver_info._type_size
        self.okl4_obj.channels.data = Autogen.types.OKL4_void_ptr(address = address)

        self.channel_data = []
        trigger = Autogen.types.OKL4_channel_transition
        in_not_empty_idx = trigger.CHANNEL_TRANSITION_NOTEMPTY_INBOUND
        out_not_full_idx = trigger.CHANNEL_TRANSITION_NOTFULL_OUTBOUND

        for (i, kern_chn) in enumerate(self.kern_channels):
            channel_data = Autogen.types.OKL4_channel_data(address = address + \
                    i * Autogen.types.OKL4_channel_data._type_size)
            channel_data.kcap = kern_chn.cap_id

            if kern_chn.virqs[in_not_empty_idx] is not None:
                channel_data.not_empty_irq = kern_chn.virqs[in_not_empty_idx].irq
            else:
                channel_data.not_empty_irq = Autogen.types.INVALID_VIRQ

            if kern_chn.virqs[out_not_full_idx] is not None:
                channel_data.not_full_irq = kern_chn.virqs[out_not_full_idx].irq
            else:
                channel_data.not_full_irq = Autogen.types.INVALID_VIRQ

            self.channel_data.append(channel_data)

    def write_entry(self, section, sas):
        CellEnvironmentEntry.write_entry(self, section, sas)

        for channel in self.channel_data:
            section.record_phys_relocs(channel.get_phys_relocs())
            section.write_output(channel.to_data())

class CellEnvVClientEntry(CellEnvironmentEntry):
    """
    Environment entry representing a vclient object

    """
    def __init__(self, obj):
        CellEnvironmentEntry.__init__(self)
        self.obj = obj

    def create(self, address, sas):
        """Create the data that will go in the entry."""
        self.okl4_obj = Autogen.types.OKL4_channel_data(address = address)

        cap = self.obj.dest_cap.get_cap_value()
        if cap is not None:
            self.okl4_obj.kcap = cap
        else:
            self.okl4_obj.kcap = 0

        if self.obj.virq_out is not None:
            self.okl4_obj.not_empty_irq = self.obj.virq_out.irq
        else:
            self.okl4_obj.not_empty_irq = Autogen.types.INVALID_VIRQ

        if self.obj.virq_in is not None:
            self.okl4_obj.not_full_irq = self.obj.virq_in.irq
        else:
            self.okl4_obj.not_full_irq = Autogen.types.INVALID_VIRQ

class CellEnvVInterruptLines(CellEnvironmentEntry):
    """
    Environment entry representing a collection of virtual interrupt lines.

    struct okl4_virtual_interrupt_lines {
        okl4_word_t num_lines;
        okl4_kcap_t *lines;
    };

    """

    def __init__(self, lines):
        CellEnvironmentEntry.__init__(self)
        self.lines = lines
        self.line_objs = []

    def get_size(self):
        return (Autogen.types.OKL4_virtual_interrupt_lines._type_size +
                Autogen.types.OKL4_designator_t._type_size * len(self.lines))

    def create(self, address, sas):
        """Create the data that will go in the entry."""
        self.okl4_obj = Autogen.types.OKL4_virtual_interrupt_lines(address = address)

        self.okl4_obj.num_lines = len(self.lines)

        address = address + Autogen.types.OKL4_virtual_interrupt_lines._type_size
        self.okl4_obj.lines = Autogen.types.OKL4_void_ptr(address = address)

        self.line_objs = []

        for line in self.lines:
            line_obj = Autogen.types.OKL4_designator_t(address = address)

            if line.cap_object is not None and \
                   line.cap_object.get_cap_value() is not None:
                line_obj.value = line.cap_object.get_cap_value()

            self.line_objs.append(line_obj)

    def write_entry(self, section, sas):
        CellEnvironmentEntry.write_entry(self, section, sas)

        for line_obj in self.line_objs:
            section.write_output(line_obj.to_data())

class CellEnvWord(CellEnvironmentEntry):
    """
    Environment entry consisting of a single word.

    The input word can either be a static value (passed in through 'value') or
    a inputless function, allowing the environment value to be evaluated lazily
    when the environment is written out.
    """
    def __init__(self, value = None, function = None):
        CellEnvironmentEntry.__init__(self)

        self.function = function
        self.value = value

    def create(self, address, sas):
        """Create the data that will go in the entry."""
        self.okl4_obj = Autogen.types.OKL4_word_t(address = address)

        # If we were provided with a function, evaluate it now. Otherwise, use
        # the static value passed in.
        if self.function != None:
            self.okl4_obj.value = self.function()
        else:
            assert self.value != None
            self.okl4_obj.value = self.value

class CellEnvKdomain(CellEnvironmentEntry):
    """ Environment entry for a libokl4 kdomain.

    The data structure for the entry is:

    struct okl4_kdomain {
        okl4_kcap_t kcap;
    };
    """
    def __init__(self, kern_domain, env_parent=None):
        CellEnvironmentEntry.__init__(self)

        self.kern_domain = kern_domain
        self.env_parent = env_parent

    def create(self, address, sas):
        """Create the data that will go in the entry."""
        self.okl4_obj = Autogen.types.OKL4_kdomain(address=address)

        if self.kern_domain.okl4_object is not None:
            self.okl4_obj.kcap = self.kern_domain.okl4_object._cap_id

    def write_entry(self, section, sas):
        """Write the kdomain."""
        if self.env_parent:
            self.okl4_obj.domain = self.env_parent.okl4_obj

        CellEnvironmentEntry.write_entry(self, section, sas)

class CellEnvMutex(CellEnvironmentEntry):
    """
    Environment entry for a mutex

    The data structure for the entry is:

    struct okl4_mutex_t {
        okl4_kcap_t id;
    };
    """
    def __init__(self, kern_mutex, is_counted, is_hybrid):
        CellEnvironmentEntry.__init__(self)
        self.kern_mutex = kern_mutex
        self.is_counted = is_counted
        self.is_hybrid = is_hybrid

    def create(self, address, sas):
        """Create the data that will go in the entry."""
        self.okl4_obj = Autogen.types.OKL4_okl4_mutex(address=address)

        if self.kern_mutex.okl4_object is not None:
            self.okl4_obj.id = self.kern_mutex.okl4_object._cap_id

        # If non-hybrid, is_counted and count do not need to be set up here
        # because they are tracked in the kernel instead
        if self.is_hybrid is True:
            self.okl4_obj.is_hybrid = 1
            self.okl4_obj.hybrid_user_status = -1
            if self.is_counted is True:
                self.okl4_obj.is_counted = 1
            else:
                self.okl4_obj.is_counted = 0
            self.okl4_obj.count = 0

class CellEnvMessageQueue(CellEnvironmentEntry):
    """
    Environment entry for a message_queue

    The data structure for the entry is:

    struct okl4_message_queue_t {
        okl4_kcap_t id;
    };
    """
    def __init__(self):
        CellEnvironmentEntry.__init__(self)

        self.mmu_cap = None
        self.kern_msgq = None

    def create(self, address, sas):
        """Create the data that will go in the entry."""
        assert 'message_queue' in Autogen.features
        #pylint: disable-msg=E1101

        self.okl4_obj = Autogen.types.OKL4_okl4_message_queue(address=address)

        if self.kern_msgq.okl4_object is not None:
            self.okl4_obj.kcap = self.kern_msgq.okl4_object._cap_id

class CellEnvPOSIXmqList(CellEnvironmentEntry):
    # FIXME: Jira ticket OLD-1436 - glee.
    """
    Environment entry for the list of OKL4 message_queue instances
    wrapped as POSIX mq instances.

    The data looks like this:
    struct okl4_env_mqs {
        okl4_word_t count;
        struct {
            char path[80];
            okl4_message_queue_t mq; /*(just a cap) */
            okl4_kcap_t mmu;
        } elts[1];
    };
    """

    # The CellEnvPOSIXmqList is similar to the ArgList in that
    # There is no relevant Autogen type so just write out the struct in
    # binary format.

    def __init__(self):
        CellEnvironmentEntry.__init__(self)
        self._list = []
        self._buffer_len = 80

    def insert(self, name, msgq):
        assert(len(name) < self._buffer_len)
        self._list.append((name, msgq))

    def get_size(self):
        return self.word_size + (len(self._list) *
                                 (self._buffer_len + (self.word_size * 2)))

    def create(self, address, sas):
        """Create the data that will go in the entry."""
        self.address = address

        for (_, msgq) in self._list:
            msgq.create(address, sas)

    def write_entry(self, section, sas):
        section.write_int(len(self._list))

        for (name, msgq) in self._list:
            section.write_strings([name])
            section.write_bytes(0, ((self._buffer_len - 1) - len(name)))
            msgq.write_entry(section, sas)
            # On the first pass through write_entry, there may be no
            # okl4_obj provided yet, so write null cap
            if msgq.right.object.mmu.okl4_obj is None:
                section.write_int(-1)
            else:
                section.write_output(msgq.right.object.mmu.okl4_obj.to_data())

    def get_libokl4_type_name(self):
        # We don't have a okl4_obj object, so just fake it.
        return "okl4_env_mqs_t"


class CellEnvTZMonitor(CellEnvironmentEntry):
    """
    Environment entry for a tzmonitor object

    The data structure for the entry is:
    struct okl4_tzmoniotr_t {
        okl4_kcap_t kcap;
    };
    """

    def __init__(self, kern_tzmonitor):
        CellEnvironmentEntry.__init__(self)

        self.kern_tzmonitor = kern_tzmonitor

    def create(self, address, sas):
        """Create the data that will go in the entry."""
        self.okl4_obj = Autogen.types.OKL4_okl4_tzmonitor(address=address)

        if self.kern_tzmonitor.okl4_object is not None:
            self.okl4_obj.kcap = self.kern_tzmonitor.okl4_object._cap_id

class CellEnvFutex(CellEnvironmentEntry):
    """
    Environment entry for a futex

    The data structure for the entry is:

struct okl4_mutex {
    okl4_word_t count;
    okl4_bool_t is_counted;
    okl4_atomic_word_t lock;
    okl4_kcap_t owner;
};
    """
    def __init__(self, **kwargs):
        CellEnvironmentEntry.__init__(self)

        self.count = kwargs.get('count', None)
        self.is_counted = kwargs.get('is_counted', None)
        self.lock = kwargs.get('lock', None)
        self.kern_owner = kwargs.get('owner', None)

    def create(self, address, sas):
        """Create the data that will go in the entry."""
        self.okl4_obj = Autogen.types.OKL4_okl4_mutex(address=address)

        if self.count is not None:
            self.okl4_obj.count = self.count

        if self.is_counted is not None:
            self.okl4_obj.is_counted = self.is_counted

        if self.lock is not None:
            self.okl4_obj.lock._set_value(self.lock)

        if self.kern_owner is None:
            self.okl4_obj.owner = Autogen.types.KCAP_INVALID

    def write_entry(self, section, sas):
        """Write the mutex."""

        #
        # If we are weaving out a counted pre-held mutex, setup the 'owner'
        # field of the futex. This allows threads to determine if they already
        # hold the futex, and hence can retake it without blocking.
        #
        # We need to calculate this owner tag late, as it requires the TCB of
        # the owner to have an allocated virtual address.
        #
        if self.kern_owner != None and self.is_counted:
            self.okl4_obj.owner = futex.get_thread_tag(self.kern_owner.okl4_object)

        CellEnvironmentEntry.write_entry(self, section, sas)

class CellEnvKmmu(CellEnvironmentEntry):
    """ Environment entry for a libokl4 kmmu.

    The data structure for the entry is:

    struct okl4_kmmu {
        okl4_kcap_t kcap;
    };
    """
    exception_handler = property()

    def __init__(self, kern_mmu):
        CellEnvironmentEntry.__init__(self)

        self.env_exception_handler = None
        self.kern_mmu = kern_mmu

    def create(self, address, sas):
        """Create the data that will go in the entry."""
        self.okl4_obj = Autogen.types.OKL4_kmmu(address=address)

        if self.kern_mmu.okl4_object:
            self.okl4_obj.kcap = self.kern_mmu.okl4_object._cap_id

    def mark_children_as_used(self):
        if hasattr(self.okl4_obj, 'exception_handler') and \
                self.env_exception_handler is not None:
            self.env_exception_handler.mark_as_used()

    def write_entry(self, section, sas):
        """Write the kmmu."""
        if hasattr(self.okl4_obj, 'exception_handler') and \
               self.env_exception_handler is not None:
            self.okl4_obj.exception_handler = self.env_exception_handler.okl4_obj

        CellEnvironmentEntry.write_entry(self, section, sas)

class CellEnvKswitch(CellEnvironmentEntry):
    """ Environment entry for a libokl4 kswitch.

    The data structure for the entry is:

    struct okl4_kswitch {
        struct okl4_kdomain *dest_domain;
        struct okl4_kmmu *dest_mmu;
        okl4_kcap_t kcap;
    };
    """
    def __init__(self, kern_switch_table, env_kdomain, env_kmmu):
        CellEnvironmentEntry.__init__(self)

        self.kern_switch_table = kern_switch_table
        self.env_dest_domain = env_kdomain
        self.env_dest_mmu = env_kmmu

    def create(self, address, sas):
        """Create the data that will go in the entry."""
        assert 'switch_table' in Autogen.features
        #pylint: disable-msg=E1101

        self.okl4_obj = Autogen.types.OKL4_kswitch(address=address)

        if self.switch_table.okl4_object:
            self.okl4_obj.kcap = self.switch_table.okl4_object._cap_id

    def write_entry(self, section, sas):
        """Write the kswitch."""
        if self.env_dest_domain is not None:
            self.okl4_obj.dest_domain = self.env_dest_domain.okl4_obj

        if self.env_dest_domain is not None:
            self.okl4_obj.dest_mmu = self.env_dest_mmu.okl4_obj

        CellEnvironmentEntry.write_entry(self, section, sas)

class CellEnvExtensionEntryToken(CellEnvironmentEntry):
    """ Environment entry for a libokl4 extension entry token.

    The data structure for the entry is:

    struct okl4_extension_entry_token {
            okl4_word_t entry_index;
    };
    """
    def __init__(self, entry_index):
        CellEnvironmentEntry.__init__(self)

        self.entry_index = entry_index

    def create(self, address, sas):
        """Create the data that will go in the entry."""
        assert 'extension' in Autogen.features
        #pylint: disable-msg=E1101

        self.okl4_obj = Autogen.types.OKL4_extension_entry_token(address=address)

    def write_entry(self, section, sas):
        """Write the entry token."""
        if self.entry_index is not None:
            self.okl4_obj.entry_index = self.entry_index

        CellEnvironmentEntry.write_entry(self, section, sas)


class CellEnvInterruptList(CellEnvironmentEntry):
    """
    Implement a python class that represents the C type;

    struct okl4_env_interrupt_list {
        okl4_word_t num_entries;
        struct okl4_env_interrupt_handle entries[];
    };

    struct okl4_env_interrupt_handle {
        okl4_interrupt_soc_descriptor_t descriptor;
        struct okl4_interrupt interrupt;
    };

    """
    def __init__(self, kern_irqs):
        CellEnvironmentEntry.__init__(self)

        self.kern_irqs = kern_irqs

    def create(self, address, sas):
        """Create the data that will go in the entry."""
        self.okl4_obj = Autogen.types.OKL4_env_interrupt_list(address)
        self.okl4_obj.num_entries = len(self.kern_irqs)

        # Fill in the IRQ entries
        self.okl4_obj.entries = []

        for kern_irq in self.kern_irqs:
            int_handle = Autogen.types.OKL4_env_interrupt_handle(0)
            int_handle.descriptor = kern_irq.irq

            if kern_irq.okl4_object:
                int_handle.interrupt.kcap = kern_irq.okl4_object._cap_id

            self.okl4_obj.entries.append(int_handle)

class CellEnvKthread(CellEnvironmentEntry):
    """ Environment entry for a libokl4 kthread.

    The data structure for the entry is:

    struct okl4_kthread {
        okl4_word_t badge;
        struct okl4_kdomain *domain;
        struct okl4_kthread *exception_handler;
        okl4_kcap_t kcap;
        struct okl4_kmmu *mmucontext;
        struct okl4_bitmap_item stack_item;
        struct okl4_bitmap_allocator *stack_pool;
        struct okl4_virtmem_item tls_item;
        struct okl4_virtmem_pool *tls_pool;
        okl4_word_t tls_tag;
    };
    """
    def __init__(self, kern_thread, env_kdomain=None, env_kmmu=None):
        CellEnvironmentEntry.__init__(self)

        self.kern_thread = kern_thread
        self.env_kdomain = env_kdomain
        self.env_kmmu = env_kmmu
        self.env_exception_handler = None

    def create(self, address, sas):
        """Create the data that will go in the entry."""
        self.okl4_obj = Autogen.types.OKL4_kthread(address=address)
        self.okl4_obj.kcap = self.kern_thread.cap_id

    def mark_children_as_used(self):
        if self.env_kdomain is not None:
            self.env_kdomain.mark_as_used()

        if self.env_kmmu is not None:
            self.env_kmmu.mark_as_used()

        if hasattr(self.okl4_obj, 'exception_handler') and \
               self.env_exception_handler is not None:
            self.env_exception_handler.mark_as_used()

    def write_entry(self, section, sas):
        """Write the kthread."""
        if self.env_kdomain is not None:
            self.okl4_obj.domain = self.env_kdomain.okl4_obj

        if self.env_kmmu is not None:
            self.okl4_obj.mmucontext = self.env_kmmu.okl4_obj

        if hasattr(self.okl4_obj, 'exception_handler') and \
               self.env_exception_handler is not None:
            self.okl4_obj.exception_handler = self.env_exception_handler.okl4_obj

        CellEnvironmentEntry.write_entry(self, section, sas)

class CellEnvHeap(CellEnvironmentEntry):
    """
    Heap environment entry.

    The data structure for the entry is:

    okl4_virtmem_item_t heap;
    """

    def __init__(self, heap):
        CellEnvironmentEntry.__init__(self)

        self.heap = heap

    def create(self, address, sas):
        """Create the data that will go in the entry."""
        self.okl4_obj = Autogen.types.OKL4_virtmem_item(address)

    def write_entry(self, section, sas):
        # If we do not have a heap, we write out a virtmem item with
        # base of 0 and no size. Heap bounds checking should pick this up.
        if self.heap:
            self.okl4_obj.range.base = self.heap.virt_addr_sas(sas)
            self.okl4_obj.range.size = self.heap.size
        else:
            self.okl4_obj.range.base = 0
            self.okl4_obj.range.size = 0

        CellEnvironmentEntry.write_entry(self, section, sas)

class CellEnvVirtmemItem(CellEnvironmentEntry):
    """
    Virtual memory item cell environment entry.
    """

    def __init__(self, segment):
        CellEnvironmentEntry.__init__(self)

        self.segment = segment

    def create(self, address, sas):
        """Create the data that will go in the entry."""
        self.okl4_obj = Autogen.types.OKL4_virtmem_item(address)

    def write_entry(self, section, sas):
        self.okl4_obj.range.base = self.segment.attrs.virt_addr_sas(sas)
        self.okl4_obj.range.size = self.segment.attrs.size

        CellEnvironmentEntry.write_entry(self, section, sas)

class CellEnvVirtmemPool(CellEnvironmentEntry):
    """
    Environment entry for a libokl4 virtmem pool.

    The data structure for the entry is:

    struct okl4_virtmem_pool {
        struct okl4_range_allocator allocator;
        struct okl4_virtmem_pool *parent;
        struct okl4_virtmem_item virt;
    };
    """
    def __init__(self, attrs):
        CellEnvironmentEntry.__init__(self)

        self.attrs = attrs

    def create(self, address, sas):
        """Create the data that will go in the entry."""
        self.okl4_obj = Autogen.types.OKL4_virtmem_pool(address=address)

        ## Setup the allocator head and base.
        self.okl4_obj.allocator.head.base = _0(self.attrs.virt_addr_sas(sas))
        self.okl4_obj.allocator.head.total_size = _0(self.attrs.size)
        LIST_HEAD_INIT(self.okl4_obj.allocator.free_list)
        LIST_ENQUEUE(self.okl4_obj.allocator.free_list,
                self.okl4_obj.allocator.head.free_list_entry)
        self.okl4_obj.virt.range.base = _0(self.attrs.virt_addr_sas(sas))
        self.okl4_obj.virt.range.size = _0(self.attrs.size)
        self.okl4_obj.virt.range.total_size = _0(self.attrs.size)

class CellEnvPhysmemItem(CellEnvironmentEntry):
    """
    Environment entry for a libokl4 physmem item.

    The data structure for an entry is:

    struct okl4_physmem_item {
        okl4_word_t paddr;
        okl4_word_t segment;
        struct okl4_range_item range;
    };
    """
    def __init__(self, attrs, kern_mmu, kern_segment):
        CellEnvironmentEntry.__init__(self)

        self.attrs = attrs
        self.kern_segment = kern_segment
        self.kern_mmu = kern_mmu

    def create(self, address, sas):
        """Create the data that will go in the entry."""
        self.okl4_obj = Autogen.types.OKL4_physmem_item(address=address)
        self.okl4_obj.paddr = _0(self.attrs.phys_addr)

        self.okl4_obj.range.base = 0
        self.okl4_obj.range.next = self.okl4_obj
        self.okl4_obj.range.size = self.attrs.size
        self.okl4_obj.range.total_size = self.attrs.size
        self.okl4_obj.segment = (
            self.kern_mmu.segment_table.index(self.kern_segment))

class CellEnvPhysmemSegpool(CellEnvironmentEntry):
    """
    Environment entry for a libokl4 physmem segpool.

    The data structure for the entry is:

    struct okl4_physmem_segpool {
        struct okl4_range_allocator allocator;
        okl4_word_t pagebits;
        struct okl4_range_allocator *parent;
        struct okl4_physmem_item phys;
    };
    """
    def __init__(self, machine, kern_segment, kern_mmu):
        CellEnvironmentEntry.__init__(self)

        self.kern_segment = kern_segment
        self.kern_mmu = kern_mmu
        self.min_page_size = machine.min_page_size()
        self.machine = machine

    def create(self, address, sas):
        """Create the data that will go in the entry."""
        attrs = self.kern_segment.attrs

        self.okl4_obj = Autogen.types.OKL4_physmem_segpool(address=address)
        self.okl4_obj.allocator.head.base = 0
        self.okl4_obj.allocator.head.next = None
        self.okl4_obj.allocator.head.size = 0
        self.okl4_obj.allocator.head.total_size = attrs.size
        LIST_HEAD_INIT(self.okl4_obj.allocator.free_list)
        LIST_ENQUEUE(self.okl4_obj.allocator.free_list,
                self.okl4_obj.allocator.head.free_list_entry)
        self.okl4_obj.pagebits = log2(self.min_page_size)
        self.okl4_obj.parent = None
        self.okl4_obj.phys.paddr = attrs.phys_addr

        if self.kern_segment.okl4_object:
            self.okl4_obj.phys.segment = (
                self.kern_mmu.segment_table.index(self.kern_segment))

        self.okl4_obj.phys.range.base = 0
        self.okl4_obj.phys.range.next = None
        self.okl4_obj.phys.range.size = attrs.size
        self.okl4_obj.phys.range.total_size = attrs.size
        self.okl4_obj.phys.alignment = _0(self.okl4_obj.phys.paddr) \
                                    % self.machine.max_page_size()

class CellEnvPhysmemPagepool(CellEnvironmentEntry):
    """
    Environment entry for a libokl4 physmem pagepool.

    The data structure for the entry is:

    struct okl4_physmem_pagepool {
        struct okl4_physmem_item phys;
        okl4_word_t pagebits;
        struct okl4_range_allocator *parent;
        struct okl4_bitmap_allocator allocator;
    };
    """
    def __init__(self, machine, kern_segment, kern_mmu, page_size):
        CellEnvironmentEntry.__init__(self)

        self.kern_segment = kern_segment
        self.kern_mmu = kern_mmu
        self.page_size = page_size
        self.machine = machine

    def create(self, address, sas):
        """Create the data that will go in the entry."""
        attrs = self.kern_segment.attrs

        self.okl4_obj = Autogen.types.OKL4_physmem_pagepool(address=address)

        # Initialise the bitmap allocator.
        self.okl4_obj.allocator.base = 0
        self.okl4_obj.allocator.pos_guess = 0
        self.okl4_obj.allocator.size = attrs.size / self.page_size
        if attrs.size:
            bitmap_words = (((attrs.size / self.page_size) - 1) / 32) + 1
            self.okl4_obj.allocator.data.extend([0] * bitmap_words)
        else:
            self.okl4_obj.allocator.data.append(0)

        self.okl4_obj.pagebits = log2(self.page_size)
        self.okl4_obj.parent = None
        self.okl4_obj.phys.paddr = attrs.phys_addr

        if self.kern_segment.okl4_object:
            self.okl4_obj.phys.segment = (
                    self.kern_mmu.segment_table.index(self.kern_segment))

        self.okl4_obj.phys.range.base = 0
        self.okl4_obj.phys.range.next = None
        self.okl4_obj.phys.range.size = attrs.size
        self.okl4_obj.phys.range.total_size = attrs.size
        self.okl4_obj.phys.alignment = _0(self.okl4_obj.phys.paddr) % self.page_size

class CellEnvSegmentTable(CellEnvironmentEntry):
    """
    Environment entry which provides a table of segment descriptor objects.

    The data structure for the entry is:

    struct okl4_env_segments {
        okl4_word_t num_segments;
        okl4_env_segment segments[];
    }

    struct okl4_env_segment {
        okl4_kcap_t seg_id;
        okl4_word_t base;
        okl4_word_t size;
        okl4_word_t rwx;
        okl4_list_t ksegment_list;
    };
    """

    def __init__(self, heap):
        CellEnvironmentEntry.__init__(self)
        self._heap = heap

    def create(self, address, sas):
        """Create the data that will go in the entry."""
        valid_segments = [s.okl4_object if s is not None else None
                          for s in self._heap.segment_table]

        self.okl4_obj = Autogen.types.OKL4_env_segment_table(address=address)
        self.okl4_obj.num_segments = len(valid_segments)

        child_segments = []

        for i, seg in enumerate(valid_segments):
            entry_address = address + \
                Autogen.types.OKL4_env_segment_table.get_offset_of("segments") + \
                i * Autogen.types.OKL4_env_segment._type_size
            child_seg = Autogen.types.OKL4_env_segment(address=entry_address)

            if seg is not None:
                child_seg.cap_id = seg._cap_id
                child_seg.base = seg.segment.base
                child_seg.size = seg.segment.size
                child_seg.rwx = seg.segment.rwx_mask

                if hasattr(child_seg, "ksegment_list"):
                    LIST_HEAD_INIT(child_seg.ksegment_list)
            else:
                child_seg.cap_id = Autogen.types.KCAP_INVALID
                child_seg.base = 0
                child_seg.size = 0
                child_seg.rwx  = 0

                if hasattr(child_seg, "ksegment_list"):
                    LIST_HEAD_INIT(child_seg.ksegment_list)

            child_segments.append(child_seg)

        # Append empty entries up to the hard segment limit. This serves two
        # purposes: to allow the user to add segments they create, and to
        # prevent the segment table changing size when the environment segment
        # is added.
        for i in range(len(valid_segments), Autogen.types.KMMU_MAX_SEGMENTS):
            entry_address = address + \
                Autogen.types.OKL4_env_segment_table.get_offset_of("segments") + \
                i * Autogen.types.OKL4_env_segment._type_size
            child_seg = Autogen.types.OKL4_env_segment(address=entry_address)
            child_seg.cap_id = Autogen.types.KCAP_INVALID
            child_seg.base = 0
            child_seg.size = 0

            if hasattr(child_seg, "ksegment_list"):
                LIST_HEAD_INIT(child_seg.ksegment_list)

            child_segments.append(child_seg)

        self.okl4_obj.segments = child_segments

class CellEnvRealm(CellEnvironmentEntry):
    """
    Environment entry for a libokl4 realm.

    The okl4_realm field represents the underlying libokl4 object
    object and should be manipulated via the methods in
    pyokl4.libokl4.realms.
    """
    def __init__(self, machine, env_address_space):
        CellEnvironmentEntry.__init__(self)

        self.machine = machine
        self.env_address_space = env_address_space
        self.env_memsections = []
        self.env_physmem_items = []
        self.env_pds = []
        self.okl4_realm = None

    def add_env_memsection(self, env_memsection):
        """Adds a CellEnvMemsec object to the list of our memsections."""
        self.env_memsections.append(env_memsection)
        env_memsection.set_realm(self)

    def add_env_pd(self, env_pd):
        """Adds a CellEnvMemsec object to the list of our memsections."""
        self.env_pds.append(env_pd)

    def add_env_physmem_item(self, env_physmem):
        self.env_physmem_items.append(env_physmem)

    def create(self, address, sas):
        """
        Create the environment entry.  The okl4_obj field already points
        to the okl4_realm.
        """
        self.okl4_realm = Autogen.types.OKL4_realm(address)
        self.okl4_obj = self.okl4_realm
        realms.okl4_realm_create(self.okl4_realm,
                                 self.env_address_space.okl4_address_space)

        # Ensure that the user hasn't changed the address space on us.
        assert self.okl4_obj.address_space == self.env_address_space.okl4_obj

        for env_pd in self.env_pds:
            realms.okl4_realm_attach_pd(self.okl4_realm,
                                        env_pd.id_)

    def mark_children_as_used(self):
        if self.env_address_space is not None:
            self.env_address_space.mark_as_used()

        for ms in self.env_memsections:
            ms.mark_as_used()

class CellEnvAddressSpace(CellEnvironmentEntry):
    """
    Environment entry for a libokl4 address_space.
    This type also encompasses all chunks owned by the address space.
    """
    def __init__(self):
        CellEnvironmentEntry.__init__(self)

        self.okl4_address_space = None
        self.env_pds = []
        self.fulllist = None
        self.static_mmu_virtpool_range = None

    def create(self, address, sas):
        """Create the data that will go in the entry."""
        self.okl4_address_space = Autogen.types.OKL4_address_space(address)
        self.okl4_obj = self.okl4_address_space

        self.prepare_address_space()

        for env_pd in self.env_pds:
            realms.okl4_protection_domain_create(env_pd,
                                                 self.okl4_address_space)

        # This is a little ugly. We need to re-set the address of each element
        # of the chunks array, so we purge the elements and re-add them to the
        # array, one by one.
        chunks = self.okl4_obj.chunks
        self.okl4_obj.chunks = []
        for chunk in chunks:
            chunk._address = address + self.okl4_obj._type_size
            self.okl4_obj.chunks.append(chunk)

        # okl4_protection_domain_create puts CellEnvPd objects into env_pds[],
        # because we don't yet create PD objects early enough. Transform them
        # now.
        if hasattr(self.okl4_obj, "pds"):
            for i in xrange(len(self.okl4_obj.pds)):
                if self.okl4_obj.pds[i] is None:
                    continue
                assert isinstance(self.okl4_obj.pds[i], CellEnvPd)
                self.okl4_obj.pds[i] = self.okl4_obj.pds[i].okl4_obj
                assert isinstance(self.okl4_obj.pds[i], Autogen.types.OKL4_pd)

    def write_entry(self, section, sas):
        """Write the binary form of the entry."""
        for chunk in self.okl4_obj.chunks:
            # Megachunks are a bit tricky - their memsection element is a
            # pointer to the first chunk, but with bit zero set. Thus at this
            # stage, we can simply reset the address of the memsection element.
            if isinstance(chunk.memsection, realms.MEGACHUNK):
                chunk.memsection = Autogen.types.OKL4_memsection(
                    address = chunk.memsection.first_chunk._address | 1)

        # On systems with a static MMU, we pre-map all memory that might be
        # allocated from a realm. Thus the address space object must only allow
        # new allocations from the pre-mapped region. This function fudges the
        # address space to do so.
        if self.static_mmu_virtpool_range is not None:
            base, end = self.static_mmu_virtpool_range
            realms.okl4_address_space_exclude_all_but(
                self.okl4_address_space,
                base, end - base + 1)

        CellEnvironmentEntry.write_entry(self, section, sas)

    def attach_pd(self, env_pd):
        # Attach a CellEnvPd object to this address space so that it gets
        # weaved out.
        self.env_pds.append(env_pd)

    def prepare_address_space(self):
        sorted_list = sorted(self.fulllist, key=lambda x: x.base)

        min_addr = align_down(sorted_list[0].base, realms.CHUNK_SIZE)
        max_addr = align_up(sorted_list[-1].end, realms.CHUNK_SIZE) 
        new_first_chunk = min_addr / realms.CHUNK_SIZE
        new_num_chunks = (max_addr - min_addr + 1) / realms.CHUNK_SIZE
        realms.okl4_address_space_create(self.okl4_address_space,
                                         new_first_chunk, new_num_chunks)

        # Also exclude any holes in the virt pool.
        prev = None

        for i in sorted_list:
            if prev is not None:
                # Ensure items don't overlap.
                assert i.base > prev.end
                # Knock out just these portions of the address space.
                assert (i.base % realms.CHUNK_SIZE) == 0
                assert ((prev.end+1) % realms.CHUNK_SIZE) == 0
                first_chunk = (prev.end + 1) / realms.CHUNK_SIZE
                num_chunks = (i.base - (prev.end + 1)) / realms.CHUNK_SIZE
                realms.okl4_address_space_eliminate(self.okl4_address_space,
                    first_chunk, num_chunks)
            prev = i

    def set_address_range(self, fulllist):
        self.fulllist = fulllist

    def set_static_mmu_virtpool(self, base, end):
        self.static_mmu_virtpool_range = (base, end)

    def mark_children_as_used(self):
        CellEnvironmentEntry.mark_as_used(self)

        for pd in self.env_pds:
            pd.mark_as_used()

class CellEnvMemsec(CellEnvironmentEntry):
    """
    Environment entry for a libokl4 memsection.
    A memsection struct is abstract, so we should never be instantiated
    ourselves.
    """
    def __init__(self, image, cellname):
        CellEnvironmentEntry.__init__(self)

        self.image = image
        self.cellname = cellname

    def create_memsec(self, address, ms):
        # Child class should have created this before calling us.
        assert isinstance(ms, Autogen.types.OKL4_memsection)

        # This relies upon the fact that the memsec struct is the first element
        # of the child class.
        ms._address = address

        for attr in ('delete_handler', 'fault_handler', 'iterate_handler',
                'lookup_handler'):
            func_str = getattr(ms, attr)
            if func_str is None:
                continue
            # Interpret the function as a string. Lookup the address of the
            # this function and store it back into the object.
            assert isinstance(func_str, str)
            handler = get_symbol(self.image.elf,
                 "%s-MAIN-%s" % (self.cellname, func_str),
                 may_not_exist=True)
            if handler is not None:
                # Get the type we need for this function pointer.
                func_type = getattr(Autogen.types, "OKL4_" + attr)
                func = func_type(address = handler.value)
                setattr(ms, attr, func)
            else:
                # Callback function is not in the image. Lets hope the user
                # didn't actually need it.
                setattr(ms, attr, None)

class CellEnvMemsecContiguous(CellEnvMemsec):
    """
    Environment entry for a libokl4 contiguous memsection.
    """
    def __init__(self, image, machine, cellname, kern_seg, kern_mmu):
        CellEnvMemsec.__init__(self, image, cellname)

        self.machine = machine
        self.kern_seg = kern_seg
        self.kern_mmu = kern_mmu
        self.env_realm = None

    def create(self, address, sas):
        """Create the data that will go in the entry."""
        # Create the OKL4 object.
        self.okl4_obj = Autogen.types.OKL4_memsection_contiguous(address)

        if self.kern_seg.attrs.virt_addr_sas(sas) is not None:
            vaddr, paddr, size = \
                   self.kern_seg.attrs.aligned_addrs_sas(self.machine, sas)

            realms.okl4_memsection_contiguous_create(self.okl4_obj,
                                                     self.env_realm.okl4_realm,
                                                     vaddr, paddr, size, False)

        CellEnvMemsec.create_memsec(self, address, self.okl4_obj.parent)

        if self.kern_seg.okl4_object is not None:
            self.okl4_obj.mapping.phys.segment = (
                self.kern_mmu.segment_table.index(self.kern_seg))

        # TODO: Find this a home.
        self.okl4_obj.mapping.attributes.cache_attributes = \
                Autogen.machine_info.cache_attributes[
                        self.kern_seg.attrs.cache_attr]
        self.okl4_obj.mapping.attributes.permissions = \
                Autogen.mmu_arch.mmu.upgrade_perms(self.kern_seg.attrs.attach)

    def set_realm(self, env_realm):
        self.env_realm = env_realm

    def mark_children_as_used(self):
        self.env_realm.mark_as_used()

class CellEnvMemsecPaged(CellEnvMemsec):
    """
    Environment entry for a libokl4 paged memsection.
    """
    def __init__(self, image, machine, cellname, kern_seg, kern_mmu):
        CellEnvMemsec.__init__(self, image, cellname)

        self.machine = machine
        self.kern_seg = kern_seg
        self.kern_mmu = kern_mmu
        self.env_realm = None

    def create(self, address, sas):
        """Create the data that will go in the entry."""
        self.okl4_obj = Autogen.types.OKL4_memsection_paged(address)

        if self.kern_seg.attrs.virt_addr_sas(sas) is not None:
            vaddr, paddr, size = \
                    self.kern_seg.attrs.aligned_addrs_sas(self.machine, sas)

            realms.okl4_memsection_paged_create(self.okl4_obj,
                                                self.env_realm.okl4_realm,
                                                vaddr, -1, size, 4096)

        CellEnvMemsec.create_memsec(self, address, self.okl4_obj.parent)

    def set_realm(self, env_realm):
        self.env_realm = env_realm;

    def mark_children_as_used(self):
        self.env_realm.mark_as_used()

class CellEnvPd(CellEnvironmentEntry):
    """
    Environment entry for a libokl4 PD.

    Implement a python class that represents the C type;

    struct okl4_pd {
      struct _okl4_address_space *address_space;
      struct okl4_kdomain domain;
      okl4_word_t domain_type;
      okl4_word_t id;
      struct okl4_kmmu mmu;
      struct okl4_realm *private_realm;
      _okl4_list_head_t threads;
    };
    """

    def __init__(self, attachments, num_attachments, pool,
            env_kdomain, env_kmmu, kern_switch_table, env_address_space, id_):
        CellEnvironmentEntry.__init__(self)

        self.attachments = attachments
        self.pool = pool
        self.env_kdomain = env_kdomain
        self.env_kmmu = env_kmmu
        self.env_private_realm = None
        self.kern_switch_table = kern_switch_table
        self.num_attachments = num_attachments
        self.id_ = id_
        self.env_address_space = env_address_space
        self.attach_data = None

    def get_size(self):
        size = self.okl4_obj._type_size

        if 'protected_library' in Autogen.features and \
                self.kern_switch_table is None:
            #pylint: disable-msg=E1101
            size += self.num_attachments * \
                    Autogen.types.OKL4_pd_attach_item._type_size
        return size

    def set_default_realm(self, env_realm):
        self.env_private_realm = env_realm
        env_realm.add_env_pd(self)

    def create(self, address, sas):
        """Create the data that will go in the entry."""
        self.okl4_obj = Autogen.types.OKL4_pd(address=address)

        if hasattr(self.okl4_obj, 'domain_type'):
            self.okl4_obj.domain_type = 1

        if hasattr(self.okl4_obj, 'mmu_type'):
            self.okl4_obj.mmu_type = 1

        LIST_HEAD_INIT(self.okl4_obj.threads)

        if 'protected_library' in Autogen.features:
            #pylint: disable-msg=E1101
            LIST_HEAD_INIT(self.okl4_obj.ext_list)
            LIST_HEAD_INIT(self.okl4_obj.pd_list)

        attach_objs = [attach.dest_cap.get_cap_value()
                       for attach in self.attachments]

        ext_objs = [obj for obj in attach_objs
                     if isinstance(obj, ExtensionAttachment)]
        mem_objs = [obj for obj in attach_objs
                    if not isinstance(obj, ExtensionAttachment)]

        # If kswitch is none this is a PD and we need to setup the attachment
        # list here
        if 'protected_library' in Autogen.features and \
                self.kern_switch_table is None:
            #pylint: disable-msg=E1101
            self.okl4_obj.is_extension = True

            if self.num_attachments is None:
                self.num_attachments = len(ext_objs)
            else:
                assert self.num_attachments >= len(ext_objs)

            self.okl4_obj.attach_array_size = self.num_attachments
            self.attach_data = [Autogen.types.OKL4_pd_attach_item(address = \
                    self.okl4_obj._address + Autogen.types.OKL4_pd._type_size + \
                    i * Autogen.types.OKL4_pd_attach_item._type_size)
                    for i in range(0, self.num_attachments)]

            for attach in self.attach_data:
                attach.pd = Autogen.types.OKL4_void_ptr(address = \
                        self.okl4_obj._address)
                attach.token.kswitch_kcap = Autogen.types.KCAP_INVALID

            if self.num_attachments > 0:
                self.okl4_obj.pd_attach_array = self.attach_data[0]

            for index, ext_obj in enumerate(ext_objs):
                LIST_ENQUEUE(ext_obj.get_list_entry(),
                             self.attach_data[index].pd_list_entry)
                LIST_ENQUEUE(self.okl4_obj.ext_list,
                             self.attach_data[index].ext_list_entry)
                self.attach_data[index].in_use = True
                ext_obj.set_env_attr(self.attach_data[index])

        # Sort the attached memsections by virt addr to be inline with the
        # library
        mem_objs = sorted(mem_objs, key = lambda x: x.get_sort_key())

        for mem_obj in mem_objs:
            mem_obj.set_env_attr(self.okl4_obj)

    def mark_children_as_used(self):
        if self.env_kdomain is not None:
            self.env_kdomain.mark_as_used()

        if self.env_kmmu is not None:
            self.env_kmmu.mark_as_used()

        if self.pool is not None:
            self.pool.mark_as_used()

        for i in self.attachments:
            i.mark_as_used()

    def write_entry(self, section, sas):
        """Write the PD."""
        assert self.pool is not None

        if self.env_kdomain is not None:
            self.okl4_obj.domain = self.env_kdomain.okl4_obj

        if self.env_kmmu is not None:
            self.okl4_obj.mmu = self.env_kmmu.okl4_obj

        if self.kern_switch_table is not None and \
                self.kern_switch_table.okl4_object is not None:
            #pylint: disable-msg=E1101
            self.okl4_obj.kswitch.kcap = self.kern_switch_table.okl4_object._cap_id

        if self.env_private_realm is not None:
            self.okl4_obj.private_realm = self.env_private_realm.okl4_obj

        self.okl4_obj.address_space = self.env_address_space.okl4_obj
        self.okl4_obj.id = self.id_

        CellEnvironmentEntry.write_entry(self, section, sas)

        if self.attach_data:
            for attach in self.attach_data:
                section.record_phys_relocs(attach.get_phys_relocs())
                section.write_output(attach.to_data())

class CellEnvPdExtToken(CellEnvironmentEntry):
    """
    Environment entry for a PD->Ext token.

    The data structure for the entry is:

    okl4_kcap_t kswitch_kcap;
    """
    def __init__(self, switch_table):
        CellEnvironmentEntry.__init__(self)

        self.switch_table = switch_table

    def create(self, address, sas):
        """Create the data that will go in the entry."""
        assert 'switch_table' in Autogen.features
        #pylint: disable-msg=E1101

        self.okl4_obj = Autogen.types.OKL4_extension_token(address = address)

        if self.switch_table.okl4_object is not None:
            self.okl4_obj.kswitch_kcap = self.switch_table.okl4_object._cap_id

class CellEnvZone(CellEnvironmentEntry):
    pass

class CellEnvValueEntry(CellEnvironmentEntry):
    """
    Environment entry for a simple word.

    The data structure for the entry is:

    word_t value;
    """
    def __init__(self, value):
        CellEnvironmentEntry.__init__(self)

        self.value = value

    def get_size(self):
        """Return the number of bytes in this structure."""
        return self.word_size

    def create(self, address, sas):
        """Create the data that will go in the entry."""
        self.okl4_obj = Autogen.types.OKL4_word_t(address = address)
        self.okl4_obj.value = self.value

class CellEnvValueInCapEntry(CellEnvironmentEntry):
    """
    Environment entry for a word stuffed into a kcap.

    The data structure for the entry is:

    designator_t value;
    """
    def __init__(self, value):
        CellEnvironmentEntry.__init__(self)

        self.value = value

    def get_size(self):
        """Return the number of bytes in this structure."""
        return self.word_size

    def create(self, address, sas):
        """Create the data that will go in the entry."""
        self.okl4_obj = Autogen.types.OKL4_designator_t(address = address)
        self.okl4_obj.value = self.value

class CellEnvPathEntry(CellEnvironmentEntry):
    """
    Base class for environment entries that obtain their value from an object
    in the namespace.
    """
    def __init__(self, path):
        CellEnvironmentEntry.__init__(self)

        self.path = path

    def has_value(self):
        """Return whether or not the entry has obtained this value."""
        raise NotImplementedError

    def obtain_value(self, namespace_obj, cell, env):
        """Obtain the value from the given namespace object."""
        raise NotImplementedError

class CellEnvCapEntry(CellEnvPathEntry):
    """
    Environment entry a cap.  A cap is a single word, but is
    value is calculated when laying out the kernel init script.

    The data structure for the entry is:

    word_t value;
    """
    def __init__(self, right_path, attach):
        CellEnvPathEntry.__init__(self, right_path)

        self.attach = attach
        self.right = None
        self.dest_cap = None

    def has_value(self):
        """Return whether or not the entry has obtained this value."""
        return self.dest_cap is not None

    def obtain_value(self, namespace_obj, cell, env):
        """Obtain the cap value from the Rights object in the namespace."""
        if not isinstance(namespace_obj, Rights):
            raise MergeError, "Cap object expected."

        self.right = namespace_obj
        self.dest_cap = self.right.copy_to(cell.get_rights_copy_data())

        # Sharing certain rights implicitly create subsequent
        # environment entries which make them useful or provide
        # extra information. E.g. an environment entry for a
        # segment cap with rights path = '/cell/segment_foo/attach'
        # will also result in the creation of a virtmem item named
        # "SEGMENT_FOO_RANGE"" containing the address (and size) at
        # which it is mapped.
        for name, entry in self.right.env_entries.items():
            env._add_entry(name.upper(), entry)


    def create(self, address, sas):
        """Create the data that will go in the entry."""
        self.okl4_obj = Autogen.types.OKL4_designator_t(address = address)
        self.okl4_obj.value = 0

    def write_entry(self, section, sas):
        """Write the binary form of the struct."""
        assert self.dest_cap is not None

        if self.dest_cap.get_cap_value() is not None:
            self.okl4_obj.value = self.dest_cap.get_cap_value()

        if getattr(self.dest_cap, "really_phys_addr", False):
            section.record_phys_relocs([PhysReloc(0,
                                         Autogen.types.OKL4_phys_reloc.PHYS_RELOC_ABS,
                                         self.okl4_obj.value)])

        section.write_output(self.okl4_obj.to_data())


class CellEnvValuePathEntry(CellEnvPathEntry):
    """
    Environment entry for a simple word.

    The data structure for the entry is:

    word_t value;
    """
    def __init__(self, path):
        CellEnvPathEntry.__init__(self, path)

        self.ns_obj = None

    def has_value(self):
        """Return whether or not the entry has obtained this value."""
        return self.ns_obj is not None

    def obtain_value(self, namespace_obj, cell, env):
        """Obtain the entry value from the Value object in the namespace."""
        if not isinstance(namespace_obj, Value):
            raise MergeError, "Value object expected."

        self.ns_obj = namespace_obj

    def get_size(self):
        """Return the number of bytes in this structure."""
        return self.word_size

    def create(self, address, sas):
        """Create the data that will go in the entry."""
        self.okl4_obj = Autogen.types.OKL4_word_t(address = address)
        self.okl4_obj.value = _0(self.ns_obj.value)

class CellEnvElfInfoEntry(CellEnvironmentEntry):
    pass

class CellEnvMachineInfo(CellEnvironmentEntry):
    """
    Environment entry containing machine-specific constants.
    """

    def __init__(self, num_cpus = 1):
        CellEnvironmentEntry.__init__(self)

        self.l1_cache_line_size = 0
        self.l2_cache_line_size = 0
        self.num_cpus = num_cpus

    def create(self, address, sas):
        """Create the data that will go in the entry."""
        self.okl4_obj = Autogen.types.OKL4_machine_info(address=address)

        # On pico systems the cache fields aren't present.
        if hasattr(self.okl4_obj, 'l1_cache_line_size'):
            self.okl4_obj.l1_cache_line_size = self.l1_cache_line_size
            self.okl4_obj.l2_cache_line_size = self.l2_cache_line_size

        if 'kernel_smp' in Autogen.modules:
            self.okl4_obj.num_cpus = self.num_cpus

class CellEnvInterruptDeviceMap(CellEnvironmentEntry):
    """
    Environment entry to describe the IRQs belonging
    to a device.

    struct okl4_env_interrupt_device_map {
        word_t num_entries;
        word_t entries[];
    }
    """

    def __init__(self, num_irqs, irqs):
        CellEnvironmentEntry.__init__(self)

        self.num_entries = num_irqs
        self.entries = irqs

    def create(self, address, sas):
        """Create the data that will go in the entry."""
        self.okl4_obj = Autogen.types.OKL4_env_interrupt_device_map(address=address)
        self.okl4_obj.num_entries = self.num_entries
        self.okl4_obj.entries = self.entries

class CellEnvArgList(CellEnvironmentEntry):
    """
    Environment entry to describe the command line argument list.

    struct okl4_env_args {
        int   argc;
        /* There be padding here on 64-bit machines. */
        char *argv;
    }

    +-----------------+
    | argc            |  sizeof(argc) = 1 int
    |(padding on 64-b)|  1 int on 64-bit, 0 on 32-bit
    +-----------------+
    | argv[0..n]      |  sizeof(argv) = argc + 1 word
    |   ...           |
    |   NULL          |
    +-----------------+
    | args[0..n]      |  sizeof(args) =
    |  - "arg0\0"     |    sum of [sizeof(args[i]) + 1 byte] for i = 0..n
    |  - "arg1\0"     |
    |   ...           |
    +-----------------+
    """

    # Note: The ArgList is a little special and does not have an
    # Autogen type which is capable of writing out the struct in
    # binary format. Therefore, we need to write the create, get_size
    # and write_entry methods ourselves. -jsok

    def __init__(self, num_args, args):
        CellEnvironmentEntry.__init__(self)

        self.num_args = num_args
        self.args = args

    def create(self, address, sas):
        """Create the data that will go in the entry."""
        self.address = address

    def get_size(self):
        # argc + argv pointers + array null termination
        size = (1 + self.num_args + 1) * self.word_size
        for arg in self.args:
            size += len(arg + "\0") # size of arg + null
        # padding
        while (size % self.word_size != 0):
            size += 1
        return size

    def write_entry(self, section, sas):
        """Write the binary form of the struct."""
        # First, write argc.
        section.write_int(self.num_args)

        # And some padding if we need it (on 64-bit machines).
        if section.word_bits == 64:
            section.write_int(0)

        # Second, write argv pointers.
        base = self.address
        offset = (self.num_args + 2) * self.word_size

        for arg in self.args:
            section.write_word(base + offset)
            offset += len(arg) + 1

        # NULL terminate the array.
        section.write_word(0)

        # Third, write the argv strings.
        section.write_strings(self.args)

        # Finally, write any padding if needed.
        while (offset % self.word_size != 0):
            section.write_bytes(0, 1)
            offset += 1

    def get_libokl4_type_name(self):
        # We don't have a okl4_obj object, so just fake it.
        return "okl4_env_args_t"

# fake an array of an OKL4_type for purposes of ENV code only!
class OKL4_Array(object):
    def __init__(self, type, length, address=None):
        self.type = type
        self.length = length
        self.address = address
        self._type_size = type._type_size * self.length
        self._type_libokl4_name = self.type._type_libokl4_name

class CellEnvATagTable(CellEnvironmentEntry):
    """
    Environment entry containing an atags table.
    Contains a series of tags of the format:

    struct tag {
        word_t size;
        word_t tag;
        word_t data[];
    }
    """

    def __init__(self, atags):
        CellEnvironmentEntry.__init__(self)
        self.atags = atags

    def create(self, address, sas):
        """Create the data that will go in the entry."""
        self.address = address
        word_size = Autogen.types.OKL4_word_t._type_size
        words = self.get_size() / word_size
        self.okl4_obj = OKL4_Array(Autogen.types.OKL4_word_t, words, address=address)

    def get_size(self):
        return sum(atag.get_size() for atag in self.atags)

    def write_entry(self, section, sas):
        """Write the binary form of the struct."""
        for atag in self.atags:
            atag.write_tag(section)


class CellEnvElfFile(CellEnvironmentEntry):
    """
    Environment entry a for a libokl4 ElfFile object.

    The data structure for the entry is:

    struct okl4_elffile {
        okl4_word_t num_segments;
        okl4_elfsegment_t segments[];
    }

    struct okl4_elfsegment {
        okl4_word_t flags;
        okl4_memsection_t *memsection;
    }
    """
    def __init__(self, elf_segment_names, env):
        CellEnvironmentEntry.__init__(self)

        # Urgh. We store the environment object as the environment objects we
        # care about are created later. So we look them up by name later on.
        self.env = env
        self.elf_segment_names = elf_segment_names

    def create(self, address, sas):
        """Create the data that will go in the entry."""
        self.okl4_obj = Autogen.types.OKL4_elffile(address=address)
        self.okl4_obj.num_segments = len(self.elf_segment_names)

        for ms_name in self.elf_segment_names:
            entry = self.env.get_entry(ms_name)
            assert (isinstance(entry, CellEnvMemsecContiguous) or \
                    isinstance(entry, CellEnvMemsecPaged))
            memsegment = Autogen.types.OKL4_elfsegment(address=None)
            memsegment.memsection = entry
            self.okl4_obj.segments.append(memsegment)

    def mark_children_as_used(self):
        for ms_name in self.elf_segment_names:
            self.env.get_entry(ms_name).mark_as_used()

class CellEnvSharedBuffer(CellEnvironmentEntry):
    """
    Environment entry a for a shared buffer object.

    The data structure for the entry is a virtmem_item_t
    """
    def __init__(self, attrs):
        CellEnvironmentEntry.__init__(self)

        self.attrs = attrs

    def create(self, address, sas):
        """Create the data that will go in the entry."""
        self.okl4_obj = Autogen.types.OKL4_virtmem_item(address=address)

        self.okl4_obj.range.base = self.attrs.virt_addr_sas(sas)
        self.okl4_obj.range.size = self.attrs.size

class CellEnvSharedBufferCap(CellEnvironmentEntry):
    """
    Environment entry a for the shared buffer segment cap object.

    The data structure for the entry is:

    okl4_kcap_t cap;
    """
    def __init__(self, segment):
        CellEnvironmentEntry.__init__(self)

        self.segment = segment

    def create(self, address, sas):
        """Create the data that will go in the entry."""
        self.okl4_obj = Autogen.types.OKL4_designator_t(address=address)

        if self.segment.okl4_object is not None:
            self.okl4_obj.value = self.segment.okl4_object._cap_id

class CellEnvSharedBuffers(CellEnvironmentEntry):
    """
    Environment entry for shared buffer array object

    The data structure for the entry is:

    struct okl4_shared_buffers_array {
        okl4_word_t num_buffers;
        okl4_shared_buffer *buffers;
    };

    struct okl4_shared_buffer {
        okl4_virtmem_item_t virtmem_item;
        okl4_kcap_t cap;
        okl4_word_t physical_base;
    };
    """

    def __init__(self, attrs, segments):
        CellEnvironmentEntry.__init__(self)

        self.attrs = attrs
        self.segments = segments
        self.shared_buffers = []

    def get_size(self):
        return (Autogen.types.OKL4_shared_buffers_array._type_size +
                Autogen.types.OKL4_shared_buffer._type_size * len(self.attrs))

    def create(self, address, sas):
        """Create the data that will go in the entry."""
        self.okl4_obj = Autogen.types.OKL4_shared_buffers_array(address=address)
        self.okl4_obj.num_buffers = len(self.attrs)

        address = address + Autogen.types.OKL4_shared_buffers_array._type_size
        self.okl4_obj.buffers = Autogen.types.OKL4_void_ptr(address = address)

        self.shared_buffers = []

        for attr, segment in zip(self.attrs, self.segments):
            shared_buffer = Autogen.types.OKL4_shared_buffer(address = address)

            shared_buffer.virtmem_item.range.base = _0(attr.virt_addr_sas(sas))
            shared_buffer.virtmem_item.range.size = attr.size

            if segment.okl4_object is not None:
                shared_buffer.cap = segment.okl4_object._cap_id

            shared_buffer.physical_base = _0(attr.phys_addr)

            self.shared_buffers.append(shared_buffer)
            address = address + Autogen.types.OKL4_shared_buffer._type_size

    def write_entry(self, section, sas):
        CellEnvironmentEntry.write_entry(self, section, sas)

        for shared_buffer in self.shared_buffers:
            section.write_output(shared_buffer.to_data())

ENV_HEADER_MAGIC = 0xb2a4

# Class for writing out the environment to the image
class CellEnvSection(BinaryWriter):
    """
    Class for the binary representation of the environment.

    Data structure for the cell environment:

    +-----------------+
    | Env Header      |
    |  - ID           | (One word)
    |  - Num objects  |
    +-----------------+
    | ptrs            |
    |  - string_ptr   | (<num_entries> 2 word structs)
    |  - entry_ptr    |
    | ...             |
    +-----------------+
    | Entry structs   | (Header with version for each?)
    | ...             |
    +-----------------+
    | Null terminated |
    | strings         |
    | ...             |
    +-----------------+
    """
    def __init__(self, output, complete_env=True):
        BinaryWriter.__init__(self, Autogen.types.OKL4_word_t._type_size,
                weaver.weavertypes._ELFWEAVER_TYPES_ENDIANESS)

        self.output = output
        self.complete_env = complete_env

    def write_output(self, value):
        self.output.write(value)

    def record_phys_relocs(self, phys_relocs):
        self.output.record_phys(phys_relocs)

    def encode_hdr(self, magic, count):
        """Encode the environment header."""
        if self.word_bits == 64:
            return self.encode_word(magic << 32 | count)
        else:
            assert self.word_bits == 32
            return self.encode_word(magic << 16 | count)

    def write_env_header(self, num_items):
        """Write out the environment header."""
        self.write_output(self.encode_hdr(ENV_HEADER_MAGIC, num_items))

    def write_environment(self, base, entries, sas):
        """
        Generate the binary form of the environment.

        The environment structure contains pointers to internal
        objects.  These addresses are calculated relative to the base
        address of the environment's memsection.
        """


        # Calculate the size of the header and the pointer arrays.
        if self.complete_env:
            header_size = self.word_size
            entries_with_strings = [ent for ent in entries if isinstance(ent, str)]
            ptrs_size = self.word_size * len(entries_with_strings) * 2
        else:
            header_size = 0
            ptrs_size = 0

        entry_addr = base + header_size + ptrs_size
        entry_array = []

        #
        # Entries in the cell environment are sorted by the following list of
        # functions: items are sorted by the first function they match in the
        # following list.
        #
        sorted_order = [
                lambda x: isinstance(x, CellEnvPd) and x.kern_switch_table != None,
                lambda x: isinstance(x, CellEnvPd),
                lambda x: isinstance(x, CellEnvAddressSpace),
                lambda x: isinstance(x, CellEnvRealm),
                lambda x: isinstance(x, CellEnvMemsec),
                lambda x: isinstance(x, CellEnvZone),
                lambda x: isinstance(x, CellEnvElfFile),
                lambda x: True,
                ]
        def get_order(x):
            for (i, y) in enumerate(sorted_order):
                if y(x):
                    return i

        sorted_hash = sorted(entries.items(), key=(lambda (_, x): get_order(x)))

        if not self.complete_env:
            # Strip unused entries from the environment.
            sorted_hash = [e for e in sorted_hash if e[1].is_used()]

        # Create entry object and set its address
        for (key, entry) in sorted_hash:
            entry.set_address(entry_addr)
            entry.create(entry_addr, sas)

            if isinstance(key, str):
                entry_array.append(entry_addr)

            entry_addr += entry.get_size()

        # Calculate the total size of the entries.
        struct_size = sum([entry.get_size() for _, entry in sorted_hash])

        string_addr = base + header_size + ptrs_size + struct_size
        string_array = []

        if self.complete_env:
            for (key, entry) in sorted_hash:
                if not isinstance(key, str):
                    continue
                string_array.append(string_addr)
                string_addr += len(key) + 1 # with NULL terminator

        # At last we can write out the contents.
        # Start with the header.
        if self.complete_env:
            self.write_env_header(len(entry_array))

        # and then the pointer table.
        if self.complete_env:
            assert len(string_array) == len(entry_array)
            self.write_ptrs(zip(string_array, entry_array))

        for key, entry in sorted_hash:
            # Write out the struct.
            initial_size = self.output.tell()
            entry.write_entry(self, sas)
            end_size = self.output.tell()

            # Ensure that the sizes reported by the structure were correct
            assert end_size - initial_size == entry.get_size()

            # Ensure that every entry is word aligned.
            assert self.output.tell() % self.word_size == 0,      \
                    "Binary output is not word aligned after writing this"  \
                    " struct."

        # Finally the strings, which are stored straight after the
        # entries.
        if self.complete_env:
            strings = [key for key, _ in sorted_hash if isinstance(key, str)]
            self.write_strings(strings)
