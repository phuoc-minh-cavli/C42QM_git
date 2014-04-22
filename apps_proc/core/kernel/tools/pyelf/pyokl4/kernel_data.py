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

from weaver.range_allocator import RangeAllocator
import pyokl4.events as events

_singleton = None

class KernelData(object):

    def __init__(self, elf_file):
        self._objects = {}
        self._arrays = {}
        self._elf_file = elf_file

    def create_object(self, symbol_name, object_type, offset = 0):
        """
        Create a new object to be patched into the kernel image.

        The object will be patched in at the symbol 'symbol_name' + offset.
        """

        assert not self._objects.has_key(symbol_name)

        #
        # First of all we need to lookup the symbol to (i) make sure it exists;
        # and (ii) get the virtual address.
        #
        # We need the latter because other objects may need to create pointers
        # to us, which can only work if we have a correctly calculated address.
        #
        symbol = self._elf_file.find_symbol(symbol_name)
        if symbol == None:
            raise Exception("Symbol '%s' does not exist in the given kernel binary." % symbol_name)

        # Create an instance of the object, passing in the virtual address
        # as a creation attribute.
        new_object = object_type(symbol.value + offset)
        self._objects[(symbol_name, offset)] = new_object
        return new_object

    def has_key(self, symbol_name, offset = 0):
        """
        Determine if this KernelData object has an object in it with the given
        key.
        """

        return self._objects.has_key((symbol_name, offset))

    def get_object(self, symbol_name, offset = 0):
        """
        Get the object at 'symbol_name' and offset 'offset'.
        """

        assert self._objects.has_key((symbol_name, offset))
        return self._objects[(symbol_name, offset)]

    def create_array(self, symbol_name, object_type, size, offset = 0):
        """
        Create a new array of objects to be patched into the kernel image.

        The array will be patched in at the symbol 'symbol_name' + offset.
        """
        array = []
        for i in xrange(0, size):
            array.append(self.create_object(symbol_name, object_type,
                offset + i * object_type._type_size))
        self._arrays[(symbol_name, offset)] = array
        return array

    def get_array(self, symbol_name, size = 0, offset = 0):
        """
        Get the array at 'symbol_name' and offset 'offset'.
        """

        assert self._arrays.has_key((symbol_name, offset))
        return self._arrays[(symbol_name, offset)]

    def patch(self, image):
        """
        Patch the given image with pending kernel data.
        """

        # Use an allocator to ensure there is no overlap in the symbols we are
        # patching
        allocator = RangeAllocator(0, 0x100000000L)

        # Patch all of our symbols
        for ((symbol_name, offset), object) in sorted(self._objects.items()):
            symbol = self._elf_file.find_symbol(symbol_name)

            # Ensure there is not overlap. The allocator will raise an
            # exception if will try to patch over the same range of memory more
            # than once.
            base = symbol.value + offset
            size = object._type_size
            allocator.allocate_range(base, size)

            # Patch the file.
            object.patch(image, symbol, offset)

def get_kernel_data():
    global _singleton
    return _singleton

def _kernel_image_load(elf = None, **kwargs):
    """Setup the kernel_data module."""
    global _singleton
    _singleton = KernelData(elf)

def _kernel_image_writeout(image = None, **kwargs):
    """Writeout the kernel data to the given image."""
    get_kernel_data().patch(image)

# Setup the module
def init():
    global _singleton
    _singleton = None

    events.subscribe("kernel_image_load",
            _kernel_image_load, priority = -100)
    events.subscribe("kernel_image_writeout",
            _kernel_image_writeout, priority = 100)
