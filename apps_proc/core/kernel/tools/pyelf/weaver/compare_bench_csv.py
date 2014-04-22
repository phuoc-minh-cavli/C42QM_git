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
from optparse import OptionParser

from weaver.subcommand import register_subcommand

def compare_bench_csv_cmd(args):
    parser = OptionParser("""compare_bench_csv FILE1 FILE2 [OPTIONS]

Compare results in FILE1 with those in FILE2.

This script assumes that inputs are well-formed result sets in .csv format
produced by the same bench output parser.

Only the first result set in each file will be considered.""")
    outtypes = ['values', 'formulas']

    parser.add_option('-o', '--outtype', dest='outtype', default='values',
            help='Calculate comparison statistics as [values] or generate ' \
                    'spreadsheet [formulas]')
    parser.add_option('-c', '--cpuspeed', dest='cpu_speed',
            help='Manually specify CPU speed in MHz (required)')

    options, args = parser.parse_args()
    if args[0] == 'compare_bench_csv':
        args = args[1:]

    if len(args) < 2:
        parser.print_help(sys.stderr)
        sys.exit(1)

    if options.outtype not in outtypes:
        print >> sys.stderr, "valid outtypes: [%s]" % ", ".join(outtypes)
        sys.exit(1)

    if options.cpu_speed is None:
        print >> sys.stderr, "please specify the CPU speed in MHz"
        sys.exit(1)
    elif not options.cpu_speed.isdigit():
        print >> sys.stderr, "please specify the CPU speed as an integer value"
        sys.exit(1)

    f = open(args[0], 'r')
    g = open(args[1], 'r')

    line_num = 0
    for f_line in f:
        f_cols = [x.strip() for x in f_line.split(',')]
        try:
            g_line = g.next()
        except StopIteration:
            print >> sys.stderr, "end of file reached prematurely, exiting"
            sys.exit(1)
        g_cols = [x.strip() for x in g_line.split(',')]

        line_num += 1

        if f_cols[0] != g_cols[0]:
            print >> sys.stderr, "field names do not match, exiting"
            sys.exit(1)

        if len(f_cols) < 3:
            print f_cols[0]
            continue

        if f_cols[1] != g_cols[1]:
            print >> sys.stderr, "field units do not match, exiting"
            sys.exit(1)

        new_line = [f_cols[i].strip() for i in range(0, 4)] + \
                [g_cols[i].strip() for i in range(2, 4)]

        if line_num is 1:
            new_line.extend(['raw diff', '% diff', 'cycles diff'])
            print ','.join(new_line)
            continue

        try:
            f_val = float(f_cols[2])
            g_val = float(g_cols[2])

            if options.outtype == "values":
                overhead = g_val - f_val
                perc_ovh = (overhead / f_val) * 100
                if f_cols[1] == 'microseconds':
                    cycl_ovh = overhead * float(options.cpu_speed)
                elif f_cols[1] == 'nanoseconds':
                    cycl_ovh = overhead * float(options.cpu_speed) / 1000
                else:
                    cycl_ovh = ''
                new_line.extend([str(overhead), str(perc_ovh), str(cycl_ovh)])
            elif options.outtype == "formulas":
                overhead_formula = "=E%d-C%d" % (line_num, line_num)
                perc_ovh_formula = "=100*(G%d/C%d)" % (line_num, line_num)
                if f_cols[1] == 'microseconds':
                    cycl_ovh_formula = "=G%d*%s" % (line_num, options.cpu_speed)
                elif f_cols[1] == 'nanoseconds':
                    cycl_ovh_formula = "=G%d*%s/1000" % \
                            (line_num, options.cpu_speed)
                else:
                    cycl_ovh_formula = ''
                new_line.extend([overhead_formula, perc_ovh_formula, \
                        cycl_ovh_formula])
        except:
            new_line.extend(['', '', ''])

        print ','.join(new_line)

register_subcommand('compare_bench_csv', compare_bench_csv_cmd,
                    usage = "Compare benchmark results.",
                    is_hidden = True)

if __name__ == '__main__':
    compare_bench_csv_cmd(sys.argv[1:])
