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

"""Tracebuffer event parsing and creation code."""

from base64 import b64decode
from struct import unpack, calcsize
from weaver.perfmon.events import Constants

from pyokl4.autogen import Autogen

class TraceError(Exception):
    """Catch-all exception for any errors during processing of the trace."""
    pass

class EventError(Exception):
    """Indicates that there was an unexpected event in the trace """
    pass

class Buffer(object):
    """
    Representation of a single buffer within a tracebuffer.
    """
    def __init__(self, base_timestamp, data_str):
        self.base_timestamp = base_timestamp
        self.data_str = data_str

    def split_eid(self, eid):
        """Return the decoded eid field."""
        eid_event = (eid & 255)
        eid_major = (eid >> 8) & 255
        eid_minor = (eid >> 16) & 255

        return (eid_event, eid_major, eid_minor)

    def split_info(self, info):
        """Return the decoded info field."""
        args = (info >> 8) & 127
        reclen = info & 127

        return (args, reclen)

    def entries(self):
        """
        Return a generator over the trace events in the tracebuffer.

        Entry format is:
        time offset: uint32
        id: uint32 packed with event, major, minor
        info: unit32 packed with
        data: number of words to cover items.

        """

        fmt = "<III"
        hdr_size = calcsize(fmt)

        idx = 0
        str_len = len(self.data_str)

        while (idx < str_len):
            try:
                #
                # Pull the first three words
                #
                desc = self.data_str[idx:idx + hdr_size]
                (ts, eid, info) = unpack(fmt, desc)
                (eid_event, eid_major, eid_minor) = self.split_eid(eid)
                (args, reclen) = self.split_info(info)
                assert(reclen > 0)
                #
                # Now get args (if any)
                #
                if args > 0:
                    args_fmt = "I" * args
                    args_size = calcsize(args_fmt)
                    data = unpack(args_fmt, \
                            self.data_str[idx + hdr_size: \
                                    idx + hdr_size + args_size])
                else:
                    data = []

                #
                # And store, using the major number to determine the container
                #
                idx += reclen * Autogen.types.WORD_SIZE

                if eid_event not in Constants.ignore:
                    yield Constants.event_factory[eid_major](\
                            float(ts) + self.base_timestamp,
                            eid_event, eid_major, eid_minor,
                            data)
            except KeyError:
                raise EventError()
            except:
                raise TraceError()

class Trace(object):
    """Represents all the tracepoints for processing."""
    def __init__(self, b64):
        self.b64 = b64
        self.records = 0

    def events(self):
        """
        Returns a generator over all trace events in the data stream.

        Subclasses must implement the buffers() method to produce an
        iterator over the buffers in the input data.
        """
        for buff in self.buffers():
            for entry in buff.entries():
                self.records += 1

                yield entry

    def buffers(self):
        """Return a sequence of buffers from the input files."""
        raise NotImplementedError


class BufferTrace(Trace):
    """Generate a trace from a text file."""
    hdr_fmt = "<QII"
    hdr_size = calcsize(hdr_fmt)

    def __init__(self, b64, filename):
        Trace.__init__(self, b64)
        self.filename = filename

    def b64_buffers(self):
        """Return a generator over base64 encoded buffers."""
        process = False

        for line in self.f.readlines():
            if line.find("TB-START") == 0:
                process = True
                continue
            elif not process:
                continue

            try:
                yield b64decode(line)

                process = False
            except:
                raise TraceError

    def binary_buffers(self):
        """Return a generator over a binary sequence of buffers."""

        hdr_data = self.f.read(self.hdr_size)

        while hdr_data:
            try:
                (ts, wrap, head) = unpack(self.hdr_fmt, hdr_data)
                buffer_data = self.f.read(head)

                yield hdr_data + buffer_data

                hdr_data = self.f.read(self.hdr_size)
            except:
                raise TraceError


    def buffers(self):
        """Return a sequence of buffers."""
        try:
            self.f = open(self.filename)
        except:
            print "Missing filename"
            exit(-1)

        if self.b64:
            tb_iter = self.b64_buffers()
        else:
            tb_iter = self.binary_buffers()

        for tb_str in tb_iter:
            hdr_data = tb_str[0:self.hdr_size]
            (ts, wrap, head) = unpack(self.hdr_fmt, hdr_data)
            buffer_data = tb_str[self.hdr_size : self.hdr_size + head]

            yield Buffer(ts, buffer_data)

        self.f.close()
        return
