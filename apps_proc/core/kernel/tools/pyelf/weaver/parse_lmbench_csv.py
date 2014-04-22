#!/usr/bin/python
#####################################################################
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
#####################################################################

import sys
import re

from weaver.subcommand import register_subcommand

class LmbenchField:
    def __init__(self, pattern, name, multiline=False, varlength=False, \
            detect_units=False, override_units=None, multicolumn=None):
        self.pattern = pattern
        self.name = name
        self.multiline = multiline
        self.varlength = varlength
        self.detect_units = detect_units
        self.override_units = override_units
        self.multicolumn = multicolumn

lmboutfields = \
    [
    LmbenchField("\[(lmbench3.0 results for.*)\]", "title")
    ] + [
    LmbenchField("\[%s: (.*)\]" % name, name) for name in [
        "LMBENCH_VER",
        "BENCHMARK_HARDWARE",
        "BENCHMARK_OS",
        "ALL",
        "DISKS",
        "DISK_DESC",
        "ENOUGH",
        "FAST",
        "FASTMEM",
        "FILE",
        "FSDIR",
        "HALF",
        "INFO",
        "LINE_SIZE",
        "LOOP_O",
        "MB",
        "MHZ",
        "MOTHERBOARD",
        "NETWORKS",
        "PROCESSORS",
        "REMOTE",
        "SLOWFS",
        "OS",
        "SYNC_MAX",
        "LMBENCH_SCHED",
        "TIMING_O",
        "LMBENCH VERSION",
        "USER",
        "HOSTNAME",
        "NODENAME",
        "SYSNAME",
        "PROCESSOR",
        "MACHINE",
        "RELEASE",
        "VERSION",
        ]
    ] + [
    LmbenchField("\[(\w{3} \w{3} \d{1,2} .*)\]", "start_timestamp"),
    LmbenchField("\[ (\d{2}:\d{2}:\d{2} up .*)\]", "uptime"),
    LmbenchField("\[mount: (.*)\]", "mount", varlength=True),
    ] + [
    LmbenchField("%s: (.*)" % name.replace("+","\+"), name, detect_units=True) \
            for name in [
        "Simple syscall",
        "Simple read",
        "Simple write",
        "Simple stat",
        "Simple fstat",
        "Simple open/close",
        "Select on 10 fd's",
        "Select on 100 fd's",
        "Select on 250 fd's",
        "Select on 500 fd's",
        # FIXME: Jira ticket MARVIN-246 - robs.
        #"Select on 10 tcp fd's",
        #"Select on 100 tcp fd's",
        #"Select on 250 tcp fd's",
        #"Select on 500 tcp fd's",
        "Signal handler installation",
        "Signal handler overhead",
        "Protection fault",
        "Pipe latency",
        "AF_UNIX sock stream latency",
        "Process fork+exit",
        "Process fork+execve",
        "Process fork+/bin/sh -c",
        "integer bit",
        "integer add",
        "integer mul",
        "integer div",
        "integer mod",
        "int64 bit",
        "uint64 add",
        "int64 mul",
        "int64 div",
        "int64 mod",
        "float add",
        "float mul",
        "float div",
        "double add",
        "double mul",
        "double div",
        "float bogomflops",
        "double bogomflops",
        ]
    ] + [
    LmbenchField("%s: (.*)" % name, name) for name in [
        "integer bit parallelism",
        "integer add parallelism",
        "integer mul parallelism",
        "integer div parallelism",
        "integer mod parallelism",
        "int64 bit parallelism",
        "int64 add parallelism",
        "int64 mul parallelism",
        "int64 div parallelism",
        "int64 mod parallelism",
        "float add parallelism",
        "float mul parallelism",
        "float div parallelism",
        "double add parallelism",
        "double mul parallelism",
        "double div parallelism",
        ]
    ] + [
    LmbenchField("%s: (.*)" % name, name, detect_units=True) for name in [
        "File /tmp/XXX write bandwidth",
        "Pagefaults on /tmp/XXX"
        ]
    ] + [
    LmbenchField(name, name, multiline=True, override_units="microseconds") \
            for name in [
        "\"mappings"
        ]
    ] + [
    #LmbenchField(name, name, multiline=True, multicolumn=[ \
    #    "# files created", "creates/sec", "deletes/sec"]) for name in [
    #    "\"File system latency"
    #    ]
    #] + [
    # FIXME: Jira ticket MARVIN-246 - robs.
    #LmbenchField("%s: (.*)" % name, name, detect_units=True) for name in [
    #    "UDP latency using localhost",
    #    "TCP latency using localhost",
    #    "TCP/IP connection cost to localhost"
    #    ]
    #] + [
    #LmbenchField(name, name, multiline=True, detect_units=True) for name in [
    #    "Socket bandwidth using localhost"
    #    ]
    #] + [
    LmbenchField("%s: (.*)" % name, name, detect_units=True) for name in [
        "AF_UNIX sock stream bandwidth",
        "Pipe bandwidth"
        ]
    ] + [
    LmbenchField(name, name, multiline=True, override_units="MB/sec") \
            for name in [
        "\"read bandwidth",
        "\"read open2close bandwidth",
        "\"Mmap read bandwidth",
        "\"Mmap read open2close bandwidth",
        "\"libc bcopy unaligned",
        "\"libc bcopy aligned",
        "Memory bzero bandwidth",
        "\"unrolled bcopy unaligned",
        "\"unrolled partial bcopy unaligned",
        "Memory read bandwidth",
        "Memory partial read bandwidth",
        "Memory write bandwidth",
        "Memory partial write bandwidth",
        "Memory partial read/write bandwidth",
        ]
    ] + [
    LmbenchField("%s (ovr=.*)" % name, "Context switch overhead %s" % name, \
            multiline=True, override_units="microseconds") for name in [
        "\"size=0k",
        "\"size=4k",
        "\"size=8k",
        "\"size=16k",
        "\"size=32k",
        "\"size=64k",
        ]
    ] + [
    LmbenchField("tlb: (.*)", "TLB size", detect_units=True)
    ] + [
    LmbenchField(name, name, multiline=True) for name in [
        "Memory load parallelism",
        ]
    ] + [
    LmbenchField("%s: (.*)" % name, name, detect_units=True) for name in [
        "STREAM copy latency",
        "STREAM copy bandwidth",
        "STREAM scale latency",
        "STREAM scale bandwidth",
        "STREAM add latency",
        "STREAM add bandwidth",
        "STREAM triad latency",
        "STREAM triad bandwidth",
        "STREAM2 fill latency",
        "STREAM2 fill bandwidth",
        "STREAM2 copy latency",
        "STREAM2 copy bandwidth",
        "STREAM2 daxpy latency",
        "STREAM2 daxpy bandwidth",
        "STREAM2 sum latency",
        "STREAM2 sum bandwidth",
        ]
    ] + [
    LmbenchField(name, name) for name in ["Memory load latency"]
    ] + [
    LmbenchField(name, name, multiline=True, override_units="nanoseconds") \
            for name in [
        "\"stride=16",
        "\"stride=32",
        "\"stride=64",
        "\"stride=128",
        "\"stride=256",
        "\"stride=512",
        "\"stride=1024",
        ]
    ] + [
    LmbenchField(name, name) for name in ["Random load latency"]
    ] + [
    LmbenchField(name, name, multiline=True, override_units="nanoseconds") \
            for name in [
        "\"stride=16",
        ]
    ] + [
    LmbenchField("\[(\w{3} \w{3} \d{1,2} .*)\]", "end_timestamp"),
    ]

def parse_lmbench_csv_cmd(args):
    usage = "Usage: parse_lmbench_csv FILE RECORDNAME"
    if len(args) < 1:
        print >> sys.stderr, usage
        print >> sys.stderr, "please provide an input file"
        sys.exit(1)
    elif len(args) < 2:
        print >> sys.stderr, usage
        print >> sys.stderr, "please choose a name for the result set"
        sys.exit(1)

    f = open(args[0], "r")

    print ', , %s,' % args[1]

    i = 0
    j = 0
    line = f.next()
    multiline_current = None
    multiline_current_extra = None
    varlength_current = None

    while True:
        if i >= len(lmboutfields):
            break

        line = line.rstrip("\r\n")

        k = 0
        match = None
        for field in lmboutfields[i:]:
            match = re.match(field.pattern, line)
            if match:
                if varlength_current:
                    print ','
                    varlength_current = None
                for x in range(i, i + k):
                    print lmboutfields[x].name, ", , ,"
                i += k + 1
                if field.multiline:
                    multiline_current = field
                    if match.groups():
                        multiline_current_extra = \
                                match.group(1).replace(',', ';')
                else:
                    print field.name, ",",
                    if match.groups():
                        if field.override_units:
                            print field.override_units, ",",
                            print match.group(1).replace(',', ';'),
                        elif field.detect_units:
                            vals = match.group(1).replace(',', ';').split()
                            print vals[len(vals) - 1], ",",
                            print ' '.join(vals[0:len(vals) - 1]),
                        else:
                            print ",",
                            print match.group(1).replace(',', ';'),
                    else:
                        print ",",
                    if field.varlength:
                        print ";",
                        varlength_current = field
                    else:
                        print ","
                break
            else:
                k += 1

        if match is None and varlength_current:
            match = re.match(varlength_current.pattern, line)
            if match:
                print match.group(1).replace(',', ';') + ";",
            else:
                print
                varlength_current = None
                i += 1

        elif match is None and multiline_current:
            if line == "":
                multiline_current = None
                multiline_current_extra = None
            else:
                match = re.match("(\S*)\s*(.*)", line)
                if multiline_current.multicolumn:
                    vals = match.group(2).split('\t')
                    for x in range(0, len(vals)):
                        print multiline_current.name,
                        print match.group(1).replace(',', ';'),
                        print ",",
                        print multiline_current.multicolumn[x],
                        print ",",
                        print vals[x],
                        print ","
                else:
                    print multiline_current.name,
                    print match.group(1).replace(',', ';'),
                    print ",",
                    if multiline_current.override_units:
                        print multiline_current.override_units, ",",
                        print match.group(2).replace(',', ';'),
                        print ",",
                    elif multiline_current.detect_units:
                        vals = match.group(2).replace(',', ';').split()
                        print vals[len(vals) - 1],
                        print ",",
                        print ' '.join(vals[0:len(vals) - 1]),
                        print ",",
                    else:
                        print ",",
                        print match.group(2).replace(',', ';'),
                        print ",",
                    if multiline_current_extra:
                        print multiline_current_extra
                    else:
                        print

        try:
            line = f.next()
        except StopIteration:
            break

    f.close()

register_subcommand('parse_lmbench_csv', parse_lmbench_csv_cmd,
                    usage = "Parse lmbench CSV output",
                    is_hidden = True)

if __name__ == '__main__':
    parse_lmbench_csv_cmd(sys.argv[1:])
