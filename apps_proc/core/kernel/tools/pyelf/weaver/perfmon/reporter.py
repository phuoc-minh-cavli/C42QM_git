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

from weaver.perfmon.trace import BufferTrace, TraceError, EventError
from weaver.perfmon.reports.mutex_contention import mutex_report
from weaver.perfmon.reports.scheduler import thread_state_report
from weaver.perfmon.reports.thread_activity import thread_activity_report
from weaver.perfmon.events import Constants
from weaver.perfmon.lifecycle import Objmanager

import weaver.autogen_helper

from elf.core import UnpreparedElfFile

from pyokl4.autogen import Autogen
from weaver.elfweaver_info import find_elfweaver_info

def prepare_data(encoded, imagefile, tracefile):
    """
    Take the provided tracefile and process the events within it into a list
    of OKL4 objects.  This list is then returned for a specific report to
    extract the data it cares about.

    *encoded* indicates if the file is base64 encoded or not, *form* indicates
    if it is a collection of full tracebuffer dumps or individual buffers.
    *imagefile* is used to determine the original platform+build params info
    so that the correct autogen types can be loaded.
    """

    #
    # Get build info from image to extract correct autogen types
    #
    img = UnpreparedElfFile(imagefile)
    einfo = find_elfweaver_info(img)
    weaver.autogen_helper.initialise(None, einfo.product, einfo.arch,
                                     einfo.cpu_desc, einfo.cpu_features,
                                     einfo.debug, img.endianess)
    #
    # Now set up info from autogen constnats
    #
    Constants.init_constants()
    Objmanager.init_factories()

    #
    # Now go through events, and process them into a series of objects
    # with base analysis done.
    #
    try:
        for event in BufferTrace(encoded, tracefile).events():
            if event.major == \
                    Constants.major.TRACEPOINT_MAJOR_OBJECT_MANAGER:
                Objmanager.explicit_action(event)
            else:
                Objmanager.get(event).add_event(event)
    except EventError:
        print """
Warning: a tracepoint event of a type not handled by perfmon was encountered
in the trace.  This has resulted in the trace being abandoned at that point.
"""
        print "\tAttempting to continue using trace prior to error"

    except TraceError:
        print "Warning: error encountered processing trace"
        print "\tAttempting to continue using trace prior to error"

    return Objmanager.objects()




PERFMON_REPORTS = {
    "mutex_contention" : mutex_report,
    "thread_state" : thread_state_report,
    "thread_activity" : thread_activity_report,
}
