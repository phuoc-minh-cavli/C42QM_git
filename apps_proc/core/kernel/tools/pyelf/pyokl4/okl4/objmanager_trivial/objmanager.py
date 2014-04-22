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

# Python implementation of the trivial objmanager

"""
Based on kernel/objmanager-trivial/src/storage.c.tmpl
"""

from pyokl4.okl4.objmanager_trivial.internal import CAPID_INDEX_BITS
from pyokl4.autogen import Autogen
from pyokl4.kernel_data import get_kernel_data
from pyokl4.okl4.objmanager_base.objmanager import ObjmanagerBase
import pyokl4.events as events
from pyokl4.okl4.kernel_base.list import LIST_ENQUEUE

# Import some names from Autogen
#pylint: disable-msg=C0103
OKL4_word_t = Autogen.types.OKL4_word_t
OKL4_void_ptr = Autogen.types.OKL4_void_ptr
kernel_objects = Autogen.types.kernel_objects
kernel_types = Autogen.types.kernel_types

# System word size.
WORD_SIZE = OKL4_void_ptr._type_size

# Determine if we have kernel debugging support (and hence need to weave KDB
# objects)
_DEBUGGING_ENABLED = ("kdb_object" in kernel_types)

if _DEBUGGING_ENABLED:
    OKL4_kdb_object = Autogen.types.OKL4_kdb_object
else:
    OKL4_kdb_object = None

class ObjmanagerException(Exception):
    """Exception to be raised if we have a problem in the objmanager"""

class Objmanager(ObjmanagerBase):
    """
    An implementation of the trivial objmanager that performs the basic
    set of functions required for generation of the initial kernel data
    structures.
    """

    # Objmanager type.
    name = "trivial"

    #
    # We should only have one instance of this object manager. The main
    # limitation is that we need to patch kernel variables, and the two
    # object managers would conflict with each other over the values in
    # these patched variables.
    #
    _instance_created = False

    def __init__(self, virt_address, phys_addr, heap_size, max_objects):
        """
        Initialise the object manager, creating the full set of objects that
        are required.
        """
        ObjmanagerBase.__init__(self, virt_address, phys_addr, heap_size)

        # Ensure we only have one instance
        assert not Objmanager._instance_created
        Objmanager._instance_created = True

        # Subscribe to the kernel writeout event, so we can patch our
        # variables to the kernel image just before it is written out.
        events.subscribe("kernel_image_writeout",
                lambda **kwargs: self._setup_kernel_data(**kwargs))

        # Number of different objects.
        num_object_types = Autogen.objmanager_autogen.NUM_OBJECT_TYPES

        # Convert strings in max_objects to Autogen types.
        max_okl4_objects = {}

        for (item, val) in max_objects.items():
            if val == 0:
                continue
            if not kernel_objects.has_key(item):
                raise ObjmanagerException(
                        "Don't know how to create a '%s' object." % item)
            max_okl4_objects[kernel_objects[item].type_class] = val

        # Allocate object arrays
        self._object_array = [[]] * num_object_types

        for j in range(0, num_object_types):

            # Does the user want us to create any of these types of objects?
            type_ = Autogen.objmanager_autogen.get_obj_from_constant(j)
            if type_ not in max_okl4_objects:
                continue

            # Allocate an array of these objects.
            num_objects = max_okl4_objects[type_]
            self._object_array[j] = ObjmanagerBase._allocate_object(
                    self, type_, num_objects, type_._type_alignment)

            # Setup cap values on all these objects
            for i in xrange(0, num_objects):
                # Setup header information.
                self._object_array[j][i].header.valid = False
                self._object_array[j][i].header.id = i | (j << CAPID_INDEX_BITS)

            # Setup linked list of free heads for all the objects.
            for i in xrange(0, num_objects - 1):
                self._object_array[j][i].header.next = \
                        self._object_array[j][i + 1].header

        # Allocate free heads.
        self._free_heads = ObjmanagerBase._allocate_object(
                self, OKL4_void_ptr, num_object_types, WORD_SIZE)
        for i in range(0, num_object_types):
            if self._object_array[i] != []:
                self._free_heads[i].value = self._object_array[i][0].header
            else:
                self._free_heads[i].value = None

        # Setup storage for KDB objects if we support them.
        if _DEBUGGING_ENABLED:
            num_kdb_objects = sum(max_okl4_objects.values())
            self._kdb_objects = ObjmanagerBase._allocate_object(
                    self, OKL4_kdb_object, num_kdb_objects, WORD_SIZE)
        else:
            self._kdb_objects = []

    def allocate_object(self, objclass, num_objects, alignment,
            total_allocation_size_override = 0):
        """Allocate an object from the objmanagers heap"""

        # Are we allocating a pooled object?
        objnum = Autogen.objmanager_autogen.get_constant_from_obj(objclass)
        if objnum >= 0:
            assert num_objects == 1
            assert total_allocation_size_override == 0
            return [self._allocate_pooled_object(objclass)]

        # Are we allocating a KDB object?
        if _DEBUGGING_ENABLED and objclass == Autogen.types.OKL4_kdb_object:
            assert num_objects == 1
            assert total_allocation_size_override == 0
            return [self._allocate_kdb_object()]

        # Otherwise, we are performing a miscellaneous allocation.
        return ObjmanagerBase._allocate_object(self, objclass,
                num_objects, alignment, total_allocation_size_override)

    def _allocate_pooled_object(self, objclass):
        """Allocate an object of the given class."""
        # Convert the object class into a object number.
        objtype = Autogen.objmanager_autogen.get_constant_from_obj(objclass)
        assert objtype >= 0

        # Check that we have free objects of the requested type.
        object_head = self._free_heads[objtype].value
        if object_head == None:
            raise ObjmanagerException(
                    'Too many ''%s'' objects allocated.' % objclass._type_name)

        # Allocate the object.
        self._free_heads[objtype].value = object_head.next
        object_ = object_head.get_back_ptr()

        # Initialise the structure elements
        object_head.valid = True
        object_head.next = None
        object_._cap_id = object_head.id

        # Return the newly allocated object.
        return object_

    def _allocate_kdb_object(self):
        """Allocate a KDB object from the KDB pool"""

        for obj in self._kdb_objects:
            if obj.object == None:
                return obj
        else:
            raise ObjmanagerException('No free KDB objects to allocate.')

    def _setup_kernel_data(self, **_):
        """Patches the appropiate variables in the kernel's data section"""

        kernel_data = get_kernel_data()

        # Patch free heads list.
        free_heads = kernel_data.create_object("free_heads", OKL4_void_ptr)
        free_heads.value = self._free_heads[0]

        # Patch object data.
        num_object_types = Autogen.objmanager_autogen.NUM_OBJECT_TYPES

        for i in range(1, num_object_types):
            type_ = Autogen.objmanager_autogen.get_obj_from_constant(i)

            # Patch in 'max_objects' symbol
            num_objs = len(self._object_array[i])
            max_objs = kernel_data.create_object("max_num_" + type_._type_name,
                    OKL4_word_t)
            max_objs.value = len(self._object_array[i])

            # Patch in the data array symbol
            if num_objs > 0:
                array = kernel_data.create_object("%s_array" % type_._type_name,
                        OKL4_void_ptr)
                array.value = self._object_array[i][0]

        # Patch in KDB objects information
        if _DEBUGGING_ENABLED:
            num_objs = kernel_data.create_object('max_num_kdb_object',
                    OKL4_word_t)
            num_objs.value = len(self._kdb_objects)

            array = kernel_data.create_object("kdb_objs", OKL4_void_ptr)
            array.value = self._kdb_objects[0]

def set_mpointer(mpointer, obj, unwindid, blocked):
    """Set a managed pointer to an object."""

    mpointer.object = obj
    mpointer.unwindid = unwindid
    LIST_ENQUEUE(obj.mpointers, mpointer.list_entry)

def objmanager_allocate(objclass, domain, num_objects, alignment,
                        total_allocation_size_override = 0):
    """Perform an allocation from the domain's objmanager"""

    return domain._source_heap.allocate_object(objclass, num_objects,
            alignment, total_allocation_size_override)

def create_weaved_thread(**kwargs):
    """Create a new thread."""

    okl4_thread = Autogen.syscalls.sys_thread_create(**kwargs)
    Autogen.scheduler.syscall.sys_priority_set(okl4_thread, kwargs['priority'])
    Autogen.scheduler.syscall.sys_timeslice_length_set(okl4_thread,
                                                       kwargs['timeslice'])
    Autogen.scheduler.syscall.sys_thread_start(okl4_thread)
    return okl4_thread

def init():
    """Initialise this module."""

    Objmanager._instance_created = False
