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

#pylint: disable-msg=R0903
"""
A set of useful stuff
"""

import types

class IntString(long):
    """
    A sub-type of integer that allows you to associate
    a string with a given integer
    """
    _show = {}
    _default_string = None
    def __new__(cls, arg, string=None):
        if string:
            cls._show[arg] = string
        return long.__new__(cls, arg)

    def __str__(self):
        if long(self) in self._show:
            return self._show[long(self)]
        elif self._default_string:
            return self._default_string % long(self)
        else:
            return long.__str__(self)

def align_up(val, alignment):
    """Round val up to a given alignment."""
    if not alignment:
        return val
    ovr = val % alignment
    if (ovr):
        val = val + alignment - ovr
    return val

def align_down(val, alignment):
    """Round val down to a given alignment"""
    if alignment:
        val -= (val % alignment)
    return val

def is_integer(val):
    """Return true if the val is an integer or long type."""
    return isinstance(val, types.IntType) or isinstance(val, types.LongType)

class Span:
    """
    A simple little helper class that lets you easily test if
    a value is in between a given span of numbers. E.g:

    >>> 3 in Span(1, 5)
    True
    >>> 0 in Span(1, 5)
    False
    >>> 1 in Span(1, 5)
    True
    >>> 5 in Span(1, 5)
    False
    """
    def __init__(self, base, end):
        """Initialise a span starting from base and going to end."""
        self.base = base
        self.end = end

    def __contains__(self, val):
        """Return true if a given value is in the span."""
        return val >= self.base and val < self.end

class Prepared(Exception):
    """
    This exception is raised when a method is called and the
    object should be in the unprepared state, but is actually in
    the prepared state.
    """

class Unprepared(Exception):
    """
    This exception is raised when a method is called and the
    object should be in the prepared state, but is actually in the
    unprepared state.
    """

def int_to_binary(val):
    """
    Converts an interger value into a string of 0's and 1's for various
    debugging purposes.  Pad to 32 bits.
    """
    bin = ''

    while val:
        if val & 0x1:
            bin += '1'
        else:
            bin += '0'

        val >>= 1

    return bin[::-1].zfill(32)
