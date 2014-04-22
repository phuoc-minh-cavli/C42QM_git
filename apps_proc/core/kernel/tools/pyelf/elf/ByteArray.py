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
#pylint: disable-msg=R0904
# We inherit from array, so we get a bunch of public methods
#pylint: disable-msg=W0142
# We need to use *magic do dispatch to struct.pack
"""
This modules providse the ByteArray class, which is an efficient way
of storing large chunks of binary data.
"""

from array import array
import struct

class ByteArray(array):
    """
    The ByteArray class is a very simple subclass of python's
    standard array type. It simply always creates the array with a
    Byte type.
    """

    def __new__(cls, data=""):
        """
        Create a new bytearray. data can be a string as per the
        array() constructor documentation.
        """
        return array.__new__(cls, 'B', data)

    def set_data(self, offset, value, bytes, endian):
        """
        Set an integer data in the array of bytes. Offset is the
        place at which to insert the value. Value is the value of the integer.
        bytes is how many bytes the integer should be. Valid values are 1, 2, 4 or 8.
        endian is whether this is big or little endian integer. Should be either '<'
        or '>'.
        """
        bytes_to_format = { 1 : "B", 2 : "H", 4 : "L", 8 : "Q" }
        temp = struct.pack(endian + bytes_to_format[bytes], value)
        format = "B" * bytes
        self[offset:offset+bytes] = array('B', struct.unpack(format, temp))

    def get_data(self, offset, bytes, endian):
        """
        Return 'bytes' bytes, starting at 'offset'. Interpret data with the
        given endianess.
        """
        format = "B" * bytes
        vals = self[offset:offset+bytes]
        value = struct.pack(format, *vals)
        bytes_to_format = { 1 : "B", 2 : "H", 4 : "L", 8 : "Q" }
        return struct.unpack(endian + bytes_to_format[bytes], value)[0]

    def copy(self):
        """Create a copy of the ByteArray."""
        # In Python 2.4.4, 2.5.2 and 2.6.0 the array.array constructor
        # uses a generic copy, rather than memcpy(), for everything
        # but strings, so the following is a lot faster than:
        #
        # return ByteArray(self)

        cpy = ByteArray()
        cpy.fromstring(self.tostring())

        return cpy

    def to_string(self, endianess):
        """
        Return a hex dump of the string.
        """
        # XXX: Dies if len(self) % 4 != 0
        string = ''

        for i in range(0, len(self), 16):
            for j in range(0, 4):
                if i + j * 4 >= len(self):
                    break

                string += "%x " % self.get_data(i + j * 4, 4, endianess)

            string += '\n'

        return string

