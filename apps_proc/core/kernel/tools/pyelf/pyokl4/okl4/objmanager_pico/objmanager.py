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
Python implementation of the pico objmanager
"""

from pyokl4.autogen import Autogen
from pyokl4.okl4.objmanager_base.objmanager import ObjmanagerBase
import pyokl4.kernel_data as kernel_data

class ObjmanagerException(Exception):
    """Exception to be raised if we have a problem in the objmanager"""

class Objmanager(ObjmanagerBase):
    """
    An implementation of the trivial objmanager that performs the basic
    set of functions required for generation of the initial kernel data
    structures.
    """

    # Objmanager type.
    name = "pico"

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

        # Create a pool of unallocated objects.
        self._pooled_objects = {}

        # Table of max sizes, used when reporting allocation
        # failures.
        self._max_objects = {}

        # Convert strings in max_objects to Autogen types.
        for (item, count) in max_objects.items():
            if count == 0:
                continue

            if not Autogen.types.kernel_objects.has_key(item):
                raise ObjmanagerException(
                        "Don't know how to create a '%s' object." % item)

            # Create the object.
            type_class = Autogen.types.kernel_objects[item].type_class
            objects = ObjmanagerBase._allocate_object(
                    self, type_class, count, type_class._type_alignment)

            # Populate the object header.
            for index, obj in enumerate(objects):
                obj.header.index = index

                # Populate the magic number for debug mode.
                if Autogen.objmanager_autogen.USE_HEADER_MAGIC:
                    obj.header.magic = \
                            Autogen.objmanager_autogen.make_header_magic(
                                type_class)

            self._pooled_objects[type_class] = objects
            self._max_objects[type_class] = count

            # Patch the kernel to point to the array if the kernel provides a
            # symbol inside struct global.
            offset = Autogen.types.OKL4_global.get_offset_of(
                'objmanager_%s_array' % type_class._type_name,
                optional=True)

            if offset is not None:
                array = kernel_data.get_kernel_data().create_object(
                        "_global", Autogen.types.OKL4_void_ptr,
                        offset = offset)
                array.value = objects[0]

        # Allocate the object tables early because in Pico the cap id is the
        # object's address and caps must be valid while the heap is being
        # built.
        self.finalise_allocations()

    def allocate_object(self, objclass, num_objects, alignment,
            total_allocation_size_override = 0):
        """Allocate an object from the objmanagers heap"""

        # Are we allocating a pooled object?
        objnum = Autogen.objmanager_autogen.get_constant_from_obj(objclass)

        if objnum >= 0:
            assert num_objects == 1
            assert total_allocation_size_override == 0

            if len(self._pooled_objects[objclass]) == 0:
                raise ObjmanagerException(
                    'Too many %s objects have been allocated; ' \
                    'only %d are allowed.' % \
                    (objclass._type_name, self._max_objects[objclass]))

            return [self._pooled_objects[objclass].pop(0)]

        # Otherwise, we are performing a miscellaneous allocation.
        return ObjmanagerBase._allocate_object(self, objclass,
                num_objects, alignment, total_allocation_size_override)

def set_mpointer(mpointer, obj, unwindid, blocked):
    """Set a managed pointer to an object."""

    mpointer.object = obj

def objmanager_allocate(objclass, domain, num_objects, alignment,
                        total_allocation_size_override = 0):
    """Perform an allocation from the domain's objmanager"""

    return domain._source_heap.allocate_object(objclass, num_objects,
            alignment, total_allocation_size_override)


# objmanager_pico needs to be closely intertwined with scheduler_pico.
# For efficiency, the scheduler requires that objmanager store threads in a
# contiguous array in order of decreasing priority.
#
# scheduler_pico.scheduler.scheduler_sort_threads transforms the thread array
# to be sorted by decreasing priority.  This should already have been called.
# Thus, create_weaved_thread should be called with threads in strictly
# decreasing order of transformed priority.

# _LAST_PRIORITY is used to store the priority of the last thread created.
# It it used only for sanity checking, to ensure that thread priorities are
# infact decreasing.
_LAST_PRIORITY = None

def create_weaved_thread(**kwargs):
    """Create a new thread."""

    # Ensure that the collapsed priorities values are contiguous and
    # decreasing (see note above).
    #pylint: disable-msg=W0603
    global _LAST_PRIORITY
    if _LAST_PRIORITY is not None:
        assert _LAST_PRIORITY > kwargs['priority']

    _LAST_PRIORITY = kwargs['priority']

    okl4_thread = Autogen.syscalls.sys_thread_create(**kwargs)
    Autogen.scheduler.syscall.sys_thread_start(okl4_thread)
    return okl4_thread

def objm_init():
    """
    Reset global data between unit test runs.
    """
    #pylint: disable-msg=W0603
    global _LAST_PRIORITY
    _LAST_PRIORITY = None

    Objmanager._instance_created = False
