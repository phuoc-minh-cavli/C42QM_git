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
KDB subsystem.
"""

# Use of protected fields are part of the type system design.
#pylint: disable-msg=W0212

from pyokl4.okl4.kernel_base.list import \
        LIST_ENTRY_INIT, LIST_ENQUEUE, LIST_HEAD_INIT

from pyokl4.kernel_data import get_kernel_data
from pyokl4.autogen import Autogen
import okl4types.okl4extras.debug.arm926ej_s_r0p3_vfp.types as types
import pyokl4.events as events

def _kdb_init():
    """Initialise the KDB subsystem."""

    # Determine if we have already initialised.
    data = get_kernel_data()
    if data.has_key("kdb_barrier_list"):
        return

    # Otherwise, Setup KDB list heads.
    heads = []
    heads.append(get_kernel_data().create_object(
            "kdb_barrier_list", types.OKL4_list_head_t))
    heads.append(get_kernel_data().create_object(
            "kdb_domain_list", types.OKL4_list_head_t))
    heads.append(get_kernel_data().create_object(
            "kdb_hybrid_mutex_list", types.OKL4_list_head_t))
    heads.append(get_kernel_data().create_object(
            "kdb_interrupt_list", types.OKL4_list_head_t))
    heads.append(get_kernel_data().create_object(
            "kdb_intlock_control_list", types.OKL4_list_head_t))
    heads.append(get_kernel_data().create_object(
            "kdb_message_queue_list", types.OKL4_list_head_t))
    heads.append(get_kernel_data().create_object(
            "kdb_mmu_context_list", types.OKL4_list_head_t))
    heads.append(get_kernel_data().create_object(
            "kdb_mutex_list", types.OKL4_list_head_t))
    heads.append(get_kernel_data().create_object(
            "kdb_platform_control_list", types.OKL4_list_head_t))
    heads.append(get_kernel_data().create_object(
            "kdb_processor_control_list", types.OKL4_list_head_t))
    heads.append(get_kernel_data().create_object(
            "kdb_profile_control_list", types.OKL4_list_head_t))
    heads.append(get_kernel_data().create_object(
            "kdb_segment_list", types.OKL4_list_head_t))
    heads.append(get_kernel_data().create_object(
            "kdb_semaphore_list", types.OKL4_list_head_t))
    heads.append(get_kernel_data().create_object(
            "kdb_switch_table_list", types.OKL4_list_head_t))
    heads.append(get_kernel_data().create_object(
            "kdb_thread_list", types.OKL4_list_head_t))
    heads.append(get_kernel_data().create_object(
            "kdb_tracebuffer_control_list", types.OKL4_list_head_t))
    for head in heads:
        LIST_HEAD_INIT(head)

def kdb_barrier_init(obj, **kwargs):
    """
    Initialise the KDB barrier object.
    """

    # Setup KDB subsystem
    _kdb_init()

    # Allocate a KDB object for the kernel object
    kdb_data = Autogen.objmanager.objmanager_allocate(types.OKL4_kdb_object,
            obj._domain, 1, types.OKL4_word_t._type_size)[0]

    # Setup KDB object list
    kdb_list_head = get_kernel_data().get_object("kdb_barrier_list")
    LIST_ENTRY_INIT(kdb_data.list)
    LIST_ENQUEUE(kdb_list_head, kdb_data.list)

    # Copy in kdbname string one character at a time
    kdbname = kwargs.get("kdbname", None)
    if (kdbname):
        for i, char in enumerate(kdbname[:len(kdb_data.name)-1]):
            kdb_data.name[i] = ord(char)

    # Give kernel object and KDB object pointers to each other
    obj.kdb.info = kdb_data
    kdb_data.object = obj
    kdb_data.type = Autogen.objmanager_autogen.OBJTYPE_BARRIER

def kdb_domain_init(obj, **kwargs):
    """
    Initialise the KDB domain object.
    """

    # Setup KDB subsystem
    _kdb_init()

    # Allocate a KDB object for the kernel object
    kdb_data = Autogen.objmanager.objmanager_allocate(types.OKL4_kdb_object,
            obj._domain, 1, types.OKL4_word_t._type_size)[0]

    # Setup KDB object list
    kdb_list_head = get_kernel_data().get_object("kdb_domain_list")
    LIST_ENTRY_INIT(kdb_data.list)
    LIST_ENQUEUE(kdb_list_head, kdb_data.list)

    # Copy in kdbname string one character at a time
    kdbname = kwargs.get("kdbname", None)
    if (kdbname):
        for i, char in enumerate(kdbname[:len(kdb_data.name)-1]):
            kdb_data.name[i] = ord(char)

    # Give kernel object and KDB object pointers to each other
    obj.kdb.info = kdb_data
    kdb_data.object = obj
    kdb_data.type = Autogen.objmanager_autogen.OBJTYPE_DOMAIN

def kdb_hybrid_mutex_init(obj, **kwargs):
    """
    Initialise the KDB hybrid_mutex object.
    """

    # Setup KDB subsystem
    _kdb_init()

    # Allocate a KDB object for the kernel object
    kdb_data = Autogen.objmanager.objmanager_allocate(types.OKL4_kdb_object,
            obj._domain, 1, types.OKL4_word_t._type_size)[0]

    # Setup KDB object list
    kdb_list_head = get_kernel_data().get_object("kdb_hybrid_mutex_list")
    LIST_ENTRY_INIT(kdb_data.list)
    LIST_ENQUEUE(kdb_list_head, kdb_data.list)

    # Copy in kdbname string one character at a time
    kdbname = kwargs.get("kdbname", None)
    if (kdbname):
        for i, char in enumerate(kdbname[:len(kdb_data.name)-1]):
            kdb_data.name[i] = ord(char)

    # Give kernel object and KDB object pointers to each other
    obj.kdb.info = kdb_data
    kdb_data.object = obj
    kdb_data.type = Autogen.objmanager_autogen.OBJTYPE_HYBRID_MUTEX

def kdb_interrupt_init(obj, **kwargs):
    """
    Initialise the KDB interrupt object.
    """

    # Setup KDB subsystem
    _kdb_init()

    # Allocate a KDB object for the kernel object
    kdb_data = Autogen.objmanager.objmanager_allocate(types.OKL4_kdb_object,
            obj._domain, 1, types.OKL4_word_t._type_size)[0]

    # Setup KDB object list
    kdb_list_head = get_kernel_data().get_object("kdb_interrupt_list")
    LIST_ENTRY_INIT(kdb_data.list)
    LIST_ENQUEUE(kdb_list_head, kdb_data.list)

    # Copy in kdbname string one character at a time
    kdbname = kwargs.get("kdbname", None)
    if (kdbname):
        for i, char in enumerate(kdbname[:len(kdb_data.name)-1]):
            kdb_data.name[i] = ord(char)

    # Give kernel object and KDB object pointers to each other
    obj.kdb.info = kdb_data
    kdb_data.object = obj
    kdb_data.type = Autogen.objmanager_autogen.OBJTYPE_INTERRUPT

def kdb_intlock_control_init(obj, **kwargs):
    """
    Initialise the KDB intlock_control object.
    """

    # Setup KDB subsystem
    _kdb_init()

    # Allocate a KDB object for the kernel object
    kdb_data = Autogen.objmanager.objmanager_allocate(types.OKL4_kdb_object,
            obj._domain, 1, types.OKL4_word_t._type_size)[0]

    # Setup KDB object list
    kdb_list_head = get_kernel_data().get_object("kdb_intlock_control_list")
    LIST_ENTRY_INIT(kdb_data.list)
    LIST_ENQUEUE(kdb_list_head, kdb_data.list)

    # Copy in kdbname string one character at a time
    kdbname = kwargs.get("kdbname", None)
    if (kdbname):
        for i, char in enumerate(kdbname[:len(kdb_data.name)-1]):
            kdb_data.name[i] = ord(char)

    # Give kernel object and KDB object pointers to each other
    obj.kdb.info = kdb_data
    kdb_data.object = obj
    kdb_data.type = Autogen.objmanager_autogen.OBJTYPE_INTLOCK_CONTROL

def kdb_message_queue_init(obj, **kwargs):
    """
    Initialise the KDB message_queue object.
    """

    # Setup KDB subsystem
    _kdb_init()

    # Allocate a KDB object for the kernel object
    kdb_data = Autogen.objmanager.objmanager_allocate(types.OKL4_kdb_object,
            obj._domain, 1, types.OKL4_word_t._type_size)[0]

    # Setup KDB object list
    kdb_list_head = get_kernel_data().get_object("kdb_message_queue_list")
    LIST_ENTRY_INIT(kdb_data.list)
    LIST_ENQUEUE(kdb_list_head, kdb_data.list)

    # Copy in kdbname string one character at a time
    kdbname = kwargs.get("kdbname", None)
    if (kdbname):
        for i, char in enumerate(kdbname[:len(kdb_data.name)-1]):
            kdb_data.name[i] = ord(char)

    # Give kernel object and KDB object pointers to each other
    obj.kdb.info = kdb_data
    kdb_data.object = obj
    kdb_data.type = Autogen.objmanager_autogen.OBJTYPE_MESSAGE_QUEUE

def kdb_mmu_context_init(obj, **kwargs):
    """
    Initialise the KDB mmu_context object.
    """

    # Setup KDB subsystem
    _kdb_init()

    # Allocate a KDB object for the kernel object
    kdb_data = Autogen.objmanager.objmanager_allocate(types.OKL4_kdb_object,
            obj._domain, 1, types.OKL4_word_t._type_size)[0]

    # Setup KDB object list
    kdb_list_head = get_kernel_data().get_object("kdb_mmu_context_list")
    LIST_ENTRY_INIT(kdb_data.list)
    LIST_ENQUEUE(kdb_list_head, kdb_data.list)

    # Copy in kdbname string one character at a time
    kdbname = kwargs.get("kdbname", None)
    if (kdbname):
        for i, char in enumerate(kdbname[:len(kdb_data.name)-1]):
            kdb_data.name[i] = ord(char)

    # Give kernel object and KDB object pointers to each other
    obj.kdb.info = kdb_data
    kdb_data.object = obj
    kdb_data.type = Autogen.objmanager_autogen.OBJTYPE_MMU_CONTEXT

def kdb_mutex_init(obj, **kwargs):
    """
    Initialise the KDB mutex object.
    """

    # Setup KDB subsystem
    _kdb_init()

    # Allocate a KDB object for the kernel object
    kdb_data = Autogen.objmanager.objmanager_allocate(types.OKL4_kdb_object,
            obj._domain, 1, types.OKL4_word_t._type_size)[0]

    # Setup KDB object list
    kdb_list_head = get_kernel_data().get_object("kdb_mutex_list")
    LIST_ENTRY_INIT(kdb_data.list)
    LIST_ENQUEUE(kdb_list_head, kdb_data.list)

    # Copy in kdbname string one character at a time
    kdbname = kwargs.get("kdbname", None)
    if (kdbname):
        for i, char in enumerate(kdbname[:len(kdb_data.name)-1]):
            kdb_data.name[i] = ord(char)

    # Give kernel object and KDB object pointers to each other
    obj.kdb.info = kdb_data
    kdb_data.object = obj
    kdb_data.type = Autogen.objmanager_autogen.OBJTYPE_MUTEX

def kdb_platform_control_init(obj, **kwargs):
    """
    Initialise the KDB platform_control object.
    """

    # Setup KDB subsystem
    _kdb_init()

    # Allocate a KDB object for the kernel object
    kdb_data = Autogen.objmanager.objmanager_allocate(types.OKL4_kdb_object,
            obj._domain, 1, types.OKL4_word_t._type_size)[0]

    # Setup KDB object list
    kdb_list_head = get_kernel_data().get_object("kdb_platform_control_list")
    LIST_ENTRY_INIT(kdb_data.list)
    LIST_ENQUEUE(kdb_list_head, kdb_data.list)

    # Copy in kdbname string one character at a time
    kdbname = kwargs.get("kdbname", None)
    if (kdbname):
        for i, char in enumerate(kdbname[:len(kdb_data.name)-1]):
            kdb_data.name[i] = ord(char)

    # Give kernel object and KDB object pointers to each other
    obj.kdb.info = kdb_data
    kdb_data.object = obj
    kdb_data.type = Autogen.objmanager_autogen.OBJTYPE_PLATFORM_CONTROL

def kdb_processor_control_init(obj, **kwargs):
    """
    Initialise the KDB processor_control object.
    """

    # Setup KDB subsystem
    _kdb_init()

    # Allocate a KDB object for the kernel object
    kdb_data = Autogen.objmanager.objmanager_allocate(types.OKL4_kdb_object,
            obj._domain, 1, types.OKL4_word_t._type_size)[0]

    # Setup KDB object list
    kdb_list_head = get_kernel_data().get_object("kdb_processor_control_list")
    LIST_ENTRY_INIT(kdb_data.list)
    LIST_ENQUEUE(kdb_list_head, kdb_data.list)

    # Copy in kdbname string one character at a time
    kdbname = kwargs.get("kdbname", None)
    if (kdbname):
        for i, char in enumerate(kdbname[:len(kdb_data.name)-1]):
            kdb_data.name[i] = ord(char)

    # Give kernel object and KDB object pointers to each other
    obj.kdb.info = kdb_data
    kdb_data.object = obj
    kdb_data.type = Autogen.objmanager_autogen.OBJTYPE_PROCESSOR_CONTROL

def kdb_profile_control_init(obj, **kwargs):
    """
    Initialise the KDB profile_control object.
    """

    # Setup KDB subsystem
    _kdb_init()

    # Allocate a KDB object for the kernel object
    kdb_data = Autogen.objmanager.objmanager_allocate(types.OKL4_kdb_object,
            obj._domain, 1, types.OKL4_word_t._type_size)[0]

    # Setup KDB object list
    kdb_list_head = get_kernel_data().get_object("kdb_profile_control_list")
    LIST_ENTRY_INIT(kdb_data.list)
    LIST_ENQUEUE(kdb_list_head, kdb_data.list)

    # Copy in kdbname string one character at a time
    kdbname = kwargs.get("kdbname", None)
    if (kdbname):
        for i, char in enumerate(kdbname[:len(kdb_data.name)-1]):
            kdb_data.name[i] = ord(char)

    # Give kernel object and KDB object pointers to each other
    obj.kdb.info = kdb_data
    kdb_data.object = obj
    kdb_data.type = Autogen.objmanager_autogen.OBJTYPE_PROFILE_CONTROL

def kdb_segment_init(obj, **kwargs):
    """
    Initialise the KDB segment object.
    """

    # Setup KDB subsystem
    _kdb_init()

    # Allocate a KDB object for the kernel object
    kdb_data = Autogen.objmanager.objmanager_allocate(types.OKL4_kdb_object,
            obj._domain, 1, types.OKL4_word_t._type_size)[0]

    # Setup KDB object list
    kdb_list_head = get_kernel_data().get_object("kdb_segment_list")
    LIST_ENTRY_INIT(kdb_data.list)
    LIST_ENQUEUE(kdb_list_head, kdb_data.list)

    # Copy in kdbname string one character at a time
    kdbname = kwargs.get("kdbname", None)
    if (kdbname):
        for i, char in enumerate(kdbname[:len(kdb_data.name)-1]):
            kdb_data.name[i] = ord(char)

    # Give kernel object and KDB object pointers to each other
    obj.kdb.info = kdb_data
    kdb_data.object = obj
    kdb_data.type = Autogen.objmanager_autogen.OBJTYPE_SEGMENT

def kdb_semaphore_init(obj, **kwargs):
    """
    Initialise the KDB semaphore object.
    """

    # Setup KDB subsystem
    _kdb_init()

    # Allocate a KDB object for the kernel object
    kdb_data = Autogen.objmanager.objmanager_allocate(types.OKL4_kdb_object,
            obj._domain, 1, types.OKL4_word_t._type_size)[0]

    # Setup KDB object list
    kdb_list_head = get_kernel_data().get_object("kdb_semaphore_list")
    LIST_ENTRY_INIT(kdb_data.list)
    LIST_ENQUEUE(kdb_list_head, kdb_data.list)

    # Copy in kdbname string one character at a time
    kdbname = kwargs.get("kdbname", None)
    if (kdbname):
        for i, char in enumerate(kdbname[:len(kdb_data.name)-1]):
            kdb_data.name[i] = ord(char)

    # Give kernel object and KDB object pointers to each other
    obj.kdb.info = kdb_data
    kdb_data.object = obj
    kdb_data.type = Autogen.objmanager_autogen.OBJTYPE_SEMAPHORE

def kdb_switch_table_init(obj, **kwargs):
    """
    Initialise the KDB switch_table object.
    """

    # Setup KDB subsystem
    _kdb_init()

    # Allocate a KDB object for the kernel object
    kdb_data = Autogen.objmanager.objmanager_allocate(types.OKL4_kdb_object,
            obj._domain, 1, types.OKL4_word_t._type_size)[0]

    # Setup KDB object list
    kdb_list_head = get_kernel_data().get_object("kdb_switch_table_list")
    LIST_ENTRY_INIT(kdb_data.list)
    LIST_ENQUEUE(kdb_list_head, kdb_data.list)

    # Copy in kdbname string one character at a time
    kdbname = kwargs.get("kdbname", None)
    if (kdbname):
        for i, char in enumerate(kdbname[:len(kdb_data.name)-1]):
            kdb_data.name[i] = ord(char)

    # Give kernel object and KDB object pointers to each other
    obj.kdb.info = kdb_data
    kdb_data.object = obj
    kdb_data.type = Autogen.objmanager_autogen.OBJTYPE_SWITCH_TABLE

def kdb_thread_init(obj, **kwargs):
    """
    Initialise the KDB thread object.
    """

    # Setup KDB subsystem
    _kdb_init()

    # Allocate a KDB object for the kernel object
    kdb_data = Autogen.objmanager.objmanager_allocate(types.OKL4_kdb_object,
            obj._domain, 1, types.OKL4_word_t._type_size)[0]

    # Setup KDB object list
    kdb_list_head = get_kernel_data().get_object("kdb_thread_list")
    LIST_ENTRY_INIT(kdb_data.list)
    LIST_ENQUEUE(kdb_list_head, kdb_data.list)

    # Copy in kdbname string one character at a time
    kdbname = kwargs.get("kdbname", None)
    if (kdbname):
        for i, char in enumerate(kdbname[:len(kdb_data.name)-1]):
            kdb_data.name[i] = ord(char)

    # Give kernel object and KDB object pointers to each other
    obj.kdb.info = kdb_data
    kdb_data.object = obj
    kdb_data.type = Autogen.objmanager_autogen.OBJTYPE_THREAD

def kdb_tracebuffer_control_init(obj, **kwargs):
    """
    Initialise the KDB tracebuffer_control object.
    """

    # Setup KDB subsystem
    _kdb_init()

    # Allocate a KDB object for the kernel object
    kdb_data = Autogen.objmanager.objmanager_allocate(types.OKL4_kdb_object,
            obj._domain, 1, types.OKL4_word_t._type_size)[0]

    # Setup KDB object list
    kdb_list_head = get_kernel_data().get_object("kdb_tracebuffer_control_list")
    LIST_ENTRY_INIT(kdb_data.list)
    LIST_ENQUEUE(kdb_list_head, kdb_data.list)

    # Copy in kdbname string one character at a time
    kdbname = kwargs.get("kdbname", None)
    if (kdbname):
        for i, char in enumerate(kdbname[:len(kdb_data.name)-1]):
            kdb_data.name[i] = ord(char)

    # Give kernel object and KDB object pointers to each other
    obj.kdb.info = kdb_data
    kdb_data.object = obj
    kdb_data.type = Autogen.objmanager_autogen.OBJTYPE_TRACEBUFFER_CONTROL


# Initialise this module
def init():
    """Initialse KDB syscall event handlers."""
    # Subscribe to creation events.
    events.subscribe("barrier_create", kdb_barrier_init)
    # Subscribe to creation events.
    events.subscribe("domain_create", kdb_domain_init)
    # Subscribe to creation events.
    events.subscribe("hybrid_mutex_create", kdb_hybrid_mutex_init)
    # Subscribe to creation events.
    events.subscribe("interrupt_create", kdb_interrupt_init)
    # Subscribe to creation events.
    events.subscribe("intlock_control_create", kdb_intlock_control_init)
    # Subscribe to creation events.
    events.subscribe("message_queue_create", kdb_message_queue_init)
    # Subscribe to creation events.
    events.subscribe("mmu_context_create", kdb_mmu_context_init)
    # Subscribe to creation events.
    events.subscribe("mutex_create", kdb_mutex_init)
    # Subscribe to creation events.
    events.subscribe("platform_control_create", kdb_platform_control_init)
    # Subscribe to creation events.
    events.subscribe("processor_control_create", kdb_processor_control_init)
    # Subscribe to creation events.
    events.subscribe("profile_control_create", kdb_profile_control_init)
    # Subscribe to creation events.
    events.subscribe("segment_create", kdb_segment_init)
    # Subscribe to creation events.
    events.subscribe("semaphore_create", kdb_semaphore_init)
    # Subscribe to creation events.
    events.subscribe("switch_table_create", kdb_switch_table_init)
    # Subscribe to creation events.
    events.subscribe("thread_create", kdb_thread_init)
    # Subscribe to creation events.
    events.subscribe("tracebuffer_control_create", kdb_tracebuffer_control_init)

