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
Kernel types base class and support methods.
"""

from struct import pack
import traceback
import sys
import copy
from weaver.util import _0
from weaver.physreloc import PhysReloc

_ELFWEAVER_TYPES_ENDIANESS = None

def set_elfweaver_types_endianess(endianess):
    """
    Record the endianness of the data structures.
    """
    #pylint: disable-msg=W0603
    # Accessing globals are not a problem.

    global _ELFWEAVER_TYPES_ENDIANESS
    _ELFWEAVER_TYPES_ENDIANESS = endianess

class ElfweaverTypeException(Exception):
    """Exception to be raised if we have an issue in the types system"""

class ElfweaverType(object):
    """
    Base class for all kernel types.
    """

    _bytes_to_format = {1 : 'B', 2 : 'H', 4 : 'L', 8 : 'Q'}

    # Hash of attribute properties, indexed by attr name.  Modelled in lisp's
    # symbol property lists.
    _plist = None

    # FIXME: Jira ticket OLD-1723 - malcolmp.
    def __init__(self, address, back_ptr = None, phys_addr = None):
        self._address = address
        self._back_ptr = back_ptr
        self._phys_addr = phys_addr
        self._dirty = False
        self._instance_plist = None

        # References to a kernel object's domain, cap and cap_id.  Not
        # every type has these but for those object that to, having
        # direct references here is mush easier that searching for the
        # value in other tables.
        self._cap_id = None
        self._cap = None
        self._domain = None
        self._source_heap = None

        self._elfweaver_obj = None

    def __setattr__(self, attr, val):
        """
        Trap missed field name changes in autogen types.

        Fields starting with '_' are not generated by autogen and are
        always allowed.
        """
        if attr[0] != '_' and \
               attr not in self._plist:
            print "Class %s: Unknown field: %s" % (self.__class__, attr)
            traceback.print_stack()
            sys.exit(1)

        object.__setattr__(self, attr, val)

    @staticmethod
    def _pack(bytes_, value):
        """Encode a value to binary form."""

        if value is None:
            value = 0
        elif value == -1:
            # Permit -1 to be used (aka. ~0). Any other negative values will
            # throw warnings as they probably indicate bugs.
            # FIXME: This should 32 be WORD_SIZE.
            value += (1 << 32)

        return pack(_ELFWEAVER_TYPES_ENDIANESS + \
                ElfweaverType._bytes_to_format[bytes_], value)

    def _pointer_to_data(self, data, size, pointer):
        """
        Helper function for the to_data() implementation of pointers.
        """
        if pointer is None:
            data.append(self._pack(size, 0))
        elif isinstance(pointer, list):
            if pointer:
                data.append(self._pack(size, pointer[0].get_address()))
            else:
                data.append(self._pack(size, 0))
        else:
            data.append(self._pack(size, pointer.get_address()))

    @staticmethod
    def _pointer_patch(image, symbol, size, addr, pointer):
        """
        Helper function for the patch() implementation of pointers.
        """
        if pointer is None:
            image.patch(symbol, 0, size, addr)
        elif isinstance(pointer, list):
            image.patch(symbol, pointer[0].get_address(), size, addr)
        else:
            image.patch(symbol, pointer.get_address(), size, addr)

    @staticmethod
    def _array_dirty_test(array):
        """
        Helper function for determining whether or not any item in an array
        type is dirty.
        """
        for item in array:
            if item is not None:
                return True

        return False

    @staticmethod
    def _field_to_string(field, name, pointer):
        """Convert a field into human readble format."""
        if type(field) in (int, long):
            return '%s : %#x\n' % (name, field)
        elif isinstance(field, list):
            string = ['%s - array[%d]\n[ ' % (name, len(field))]

            for obj in field:
                if obj is None:
                    string.append("%s " % (obj))
                elif type(obj) in (int, long):
                    string.append("%#x " % (obj))
                else:
                    string.append(obj.to_string(name))

            string.append("]\n")

            return ''.join(string)
        elif isinstance(field, ElfweaverType):
            if pointer:
                return '%s : ' % name + str(field) + '\n'
            else:
                return field.to_string(name)
        else:
            return '%s : %s\n' % (name, field)

    def get_address(self):
        """Return the virtual address of the object."""
        return self._address

    def set_address(self, address, phys_addr = None):
        """
        Set the virtual and physical address of the object.
        """
        self._address = address

        if phys_addr is not None:
            self._phys_addr = phys_addr

    def get_phys_address(self):
        """Return the physical address of the object."""
        # If this is called, then the physical address *really* should have
        # been set.
        assert self._phys_addr is not None
        return self._phys_addr

    def get_back_ptr(self):
        """Return the object's back pointer."""
        return self._back_ptr

    def patch(self, image, symbol, addr = 0):
        """Patch the data in this type at symbol + addr."""
        raise NotImplementedError

    def to_string(self, name = ''):
        """Generate a human-readable form of the object."""
        raise NotImplementedError

    def to_data(self):
        """Convert the object into binary form."""
        raise NotImplementedError

    def set_elfweaver_obj(self, obj):
        """Set the elfweaver object associated with this object."""
        self._elfweaver_obj = obj

    def get_elfweaver_obj(self):
        """Get the elfweaver object associated with this object."""
        return self._elfweaver_obj

    @classmethod
    def get_offset_of(cls, element_str, optional=False):
        """
        Return the offset in bytes of the element from the start of the
        structure.
        """
        element_plist = cls._plist.get(element_str, None)

        if element_plist is None:
            if optional:
                return None
            else:
                raise ElfweaverTypeException("Unknown element: %s" % element_str)

        return element_plist['offset']

    def get_phys_relocs(self, base_offset=0):
        """Return physical relocation data for this object."""
        return self._get_local_phys_reloc(base_offset)

    def make_instance_plist(self):
        """
        Make an instance copy of the plist.  This will be used in preference
        to the class plist.
        """
        self._instance_plist = copy.deepcopy(self._plist)

    def plist_put(self, field, plist_property, value):
        """
        Assign the value to the property of the field.  It it assigned to the
        instance copy of the plist if is exists, otherwise the class plist.
        """
        plist = self._instance_plist \
                if self._instance_plist is not None else self._plist

        plist[field][plist_property] = value

    def plist_get(self, field, plist_property, default):
        """
        Assign the value to the property of the field.  It it assigned to the
        instance copy of the plist if is exists, otherwise the class plist.
        """
        plist = self._instance_plist \
                if self._instance_plist is not None else self._plist

        if field not in plist:
            return default

        return plist[field].get(plist_property, default)

    def _get_local_phys_reloc(self, base_offset):
        """
        Helper function to return the physical relocation details for the
        fields in this type.  It does not calculate get the phys relocs of sub
        fields.
        """
        offsets = []

        # If there is an instance property list, use that in preference to the
        # class property list.
        plist = self._instance_plist \
                if self._instance_plist is not None else self._plist

        for (field, field_props) in plist.items():
            if 'phys_reloc' in field_props:
                contents = _0(getattr(self, field))
                if callable(contents):
                    contents = contents()

                offsets.append(PhysReloc(base_offset + field_props['offset'],
                                         field_props['phys_reloc'], contents))
        return offsets

class SimpleElfweaverType(ElfweaverType):
    """
    Base class for elfweaver types that are not auto-generated, implementing
    some common methods.
    """
    def __init__(self, address, back_ptr = None, phys_addr = None):
        ElfweaverType.__init__(self, address, back_ptr, phys_addr)

    def to_string(self, name=''):
        """Generate a human-readable form of the object."""

        string = name + ' - ' + str(self) + '\n'
        return string

    def patch(self, image, symbol, addr = 0):
        """Patch the data in this type at symbol + addr."""

        raise Exception, "patch() method not implemented."

    def to_data(self):
        """Convert the object into binary form."""
        raise NotImplementedError
