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

class DwarfData(object):
    """ Class to abstract the functionality of extracting various data
        types from a DWARF data section """

    def __init__(self, data):
        self.d = data
        self.size = len(data)
        self.c = 0

    def reset(self, offset = 0):
        """ Resest the consumed count, optionally to some offset """
        self.c = offset
        return self

    def get_consumed(self):
        """ Return how many bytes consumed since the last reset """
        return self.c

    def have_data(self):
        """ Return if there is still data to be consumed """
        return self.c < self.size


    def consume_uleb128(self):
        value = 0
        shift = 0
        while True:
            try:
                d = self.d[self.c]
            except Exception, e:
                d = 0
            self.c += 1
            value |= (d & 0x7F) << shift
            if not (d & 0x80):
                break
            shift += 7
        return value

    def consume_sleb128(self):
        value = 0
        shift = 0
        size = 32
        while True:
            d = self.d[self.c]
            self.c += 1
            value |= (d & 0x7F) << shift
            shift += 7
            if not (d & 0x80):
                break
        if (shift < size) and ( d & 0x40):
            result |= -(1<<shift)

        return value

    def consume_bytes(self, num):
        ret = self.d[self.c:self.c+num]
        self.c += num
        return ret

    def peek_uleb128(self):
        value = 0
        shift = 0
        count = 0
        while True:
            d = self.d[self.c + count]
            count += 1
            value |= (d & 0x7F) << shift
            if not (d & 0x80):
                break
            shift += 7
        return value

    def consume_byte(self):
        data = self.d
        i = self.c
        value = (data[i]<<0)
        self.c += 1
        return value

    def consume_short(self):
        data = self.d
        i = self.c
        value = (data[i+0]<<0 ) + (data[i+1]<<8 )
        self.c += 2
        return value

    def consume_word(self):
        data = self.d
        i = self.c
        value = (data[i+0]<<0 ) + (data[i+1]<<8 ) + \
                (data[i+2]<<16) + (data[i+3]<<24)
        self.c += 4
        return value

    def consume_doubleword(self):
        data = self.d
        i = self.c
        value = (data[i+0]<<0 ) + (data[i+1]<<8 ) + \
                (data[i+2]<<16) + (data[i+3]<<24)+ \
                (data[i+4]<<32) + (data[i+5]<<40) + \
                (data[i+6]<<48) + (data[i+7]<<56)
        self.c += 8
        return value

    def consume_string(self):
        data = self.d
        str = ""
        while True:
            d = data[self.c]
            self.c += 1
            if d == 0:
                break
            str += chr(d)

        return str

    def consume_addr(self, ptr_sz):
        if (ptr_sz==4): return self.consume_word()
        elif (ptr_sz==8): return self.consume_doubleword()
        else:
            raise Exception("Unknown PTR siz: ", ptr_sze)

    def peek_string(self, offset=0):
        if self.c != 0:
            print "WARN, already offset by", self.c,
        data = self.d[offset:]
        str = ""
        for d in data:
            if d == 0:
                break
            str += chr(d)
        return str

    def put_word(self, offset, value):
        data = self.d

        data[offset + 0] = (value >> 0) & 0xFF
        data[offset + 1] = (value >> 8) & 0xFF
        data[offset + 2] = (value >> 16) & 0xFF
        data[offset + 3] = (value >> 24) & 0xFF
