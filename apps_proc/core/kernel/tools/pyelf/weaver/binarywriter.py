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

from struct import pack

class BinaryWriter(object):
    """Convenience functions for writing binary data to some output stream.
    Users should inherit from this class and implement 'write_output'."""

    def __init__(self, word_size, endianess):
        # word_size is bytes, word_bit is bits
        self.word_size = word_size
        self.word_bits = word_size * 8
        self.endianess = endianess

        if self.word_bits == 64:
            word_char = 'Q'
            signed_word_char = 'q'
        else:
            word_char = 'I'
            signed_word_char = 'i'

        self.format_word = endianess + word_char
        self.format_signed_word = endianess + signed_word_char
        self.format_string = endianess + '%ds'
        self.format_int = endianess + 'I'
        self.format_signed_int = endianess + 'i'

    def encode_word(self, word):
        """Encode a single word."""
        return pack(self.format_word, word)

    def encode_signed_word(self, word):
        """Encode a single signed word."""
        return pack(self.format_signed_word, word)

    def encode_int(self, integer):
        """Encode a single int (always 32-bit wide)."""
        return pack(self.format_int, integer)

    def encode_signed_int(self, integer):
        """Encode a single signed int (always 32-bits wide)."""
        return pack(self.format_signed_int, integer)

    def encode_string(self, string):
        """Encode a string.  The string will be NUL terminated."""
        string += '\0'
        return pack(self.format_string % len(string), string)

    def encode_byte(self, value):
        """Encode a byte."""
        return pack('b', value)

    def write_word(self, word):
        """Write out an encoded word."""
        if word < 0:
            self.write_output(self.encode_signed_word(word))
        else:
            self.write_output(self.encode_word(word))

    def write_int(self, integer):
        """Write out an encoded int (always 32-bits wide)."""
        if integer < 0:
            self.write_output(self.encode_signed_int(integer))
        else:
            self.write_output(self.encode_int(integer))

    def write_ptrs(self, ptr_array):
        """Write out a pointer array."""
        for (key_ptr, value_ptr) in ptr_array:
            self.write_word(key_ptr)
            self.write_word(value_ptr)

    def write_strings(self, strings):
        """
        Write out a list of strings.

        The strings will be byte aligned in memory.
        """
        for string in strings:
            self.write_output(self.encode_string(string))

    def write_bytes(self, value, count):
        """
        Write out count number of bytes with the same value.
        Useful for padding.
        """
        self.write_output(self.encode_byte(value) * count)

    def write_output(self, value):
        """Write the given string to the output stream."""
        raise NotImplementedError

