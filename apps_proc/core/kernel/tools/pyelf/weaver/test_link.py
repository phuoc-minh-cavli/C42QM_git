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
"""
Testing elfweaver command to link an ELF using elfweaver.
"""

import sys
import subprocess
import optparse

from weaver.subcommand import register_subcommand
from weaver.link import link_cmd

# FIXME: RVCT is a horrible, terrible mess.  Kill it.
# The --partial command appers to do nothing compared to the GNU linker.  We end
# up with 2000+ (!!!!) sections with many duplicates (10+ .text, .data, etc.).
# Obviously it doesn't even attempt to merge same named sections from the files
# passed on the commandline.  Need to have better section merging before we can
# cope with this.  And fun section names like 'i.L4_FpageLog2', what are we
# meant to do with them? O.o  Also we have sections of type GROUP that need to
# be dealt with.

def get_script_names(args):
    """
    Find the linker script names from the command line arguments.

    Recognises both GNU and RVCT options.
    """
    scripts = []
    next = False

    for arg in args:
        if next:
            scripts.append(arg)
            next = False
        elif arg.startswith("--script="):
            scripts.append(arg[9:])
        elif arg == "-T":
            next = True
        elif arg.startswith("-T") and len(arg) > 2:
            if not(arg.startswith("-Tbss") or arg.startswith("-Tdata") or
                   arg.startswith("-Ttext")):
                scripts.append(arg[2:])
        elif arg == "--scatter":
            next = True

    return scripts

def test_link_cmd(args):
    """
    Link an ELF using either elfweaver.
    """

    print "********"
    print "WARNING: elfweaver test_link is for internal debugging and testing purposes only."
    print "********"
    print

    parser = optparse.OptionParser("%prog [options] -- <linker> [linker_options]",
                                   add_help_option=0)
    parser.add_option("-H", "--help", action="help")
    parser.add_option("-o", "--output", dest="target", metavar="FILE",
                        help="Linker will create FILE.")
    parser.add_option("-f", "--file-segment-list", dest="file_segment_list",
                      metavar="FILE",
                      help="File containing segment names to be added to \
                      .segment_names, one per line")
    parser.add_option("-c", "--cmd-segment-list", dest="cmd_segment_list",
                        help="quoted list of comma separated segment names to \
                        be added to .segment_names,")
    parser.add_option("-s", "--create-segments", dest="create_segments",
                      action="store_true",
                      help="Set to enable gathering orphaned sections and \
                      placing each in a new segment")

    (options, args) = parser.parse_args(args)

    create_segments = options.create_segments
    file_segment_list = options.file_segment_list
    cmd_segment_list = options.cmd_segment_list
    if cmd_segment_list:
        cmd_segment_list = cmd_segment_list.split(r',')
    target = options.target

    scripts = get_script_names(args)

    # Okay, now we want elfweaver to link things
    # First, run the linker command after inserting an -r into the commands
    args.insert(1, '-r')

    if subprocess.Popen(args).wait() != 0:
        sys.exit(1)

    # Construct the link command's arguments
    target = args[args.index('-o') + 1]
    text_loc = None
    data_loc = None

    # Because apparantly one can use both -Ttext addr and -Ttext=addr as valid
    # syntax.  Blarg.
    if '-Ttext' in args:
        text_loc = args[args.index('-Ttext') + 1]
    if '-Tdata' in args:
        data_loc = args[args.index('-Tdata') + 1]
    for arg in args:
        if len(arg) == 6:
            continue
        if arg.startswith("-Ttext"):
            text_loc = arg.split('=')[1]
        if arg.startswith("-Tdata"):
            data_loc = arg.split('=')[1]

    link_args = ['-o', target, target]
    if text_loc != None:
        link_args.append('-Ttext')
        link_args.append(text_loc)
    if data_loc != None:
        link_args.append('-Tdata')
        link_args.append(data_loc)

    link_cmd(link_args)

register_subcommand('test_link', test_link_cmd,
                    usage = "Linker testing command",
                    is_hidden = True)
