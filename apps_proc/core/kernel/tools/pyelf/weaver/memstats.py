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

"""Implements a command for displaying memory usage statistics"""

import sys

import os.path
from subprocess import Popen, PIPE
from optparse import OptionParser

from elf.core import PreparedElfFile
from elf.constants import ElfFormatError

from weaver.subcommand import register_subcommand
from weaver.ezxml import EzXMLError
from weaver.memstats_xml import Memstats_el
from weaver.memstats_reports import REPORTS_SET

MEMSTATS_SECTION_NAME = "memstats"

def memstats_cmd(args):
    """Memory Usage Statistics program implementation"""
    parser = OptionParser("%prog memstats [options] file [file]",
                          add_help_option=0)
    parser.add_option("-H", "--help", action="help")
    parser.add_option("-x", "--xml", action="store", dest="output_xml",
                      help="Produces an XML file containing memory statistics" \
                           " in the same directory as the supplied ELF.")
    parser.add_option("-y", "--verify", action="store_true", dest="verify_xml",
                      help="Verify the XML file.")
    parser.add_option("-r", "--report", action="store", dest="report",
                      metavar="NAME",
                      help="Parse a memory statistics XML file and output "\
                           "the named report.  Valid reports are: '%s'." %
                      "', '".join(REPORTS_SET.keys()))
    parser.add_option("-v", "--verbose", action="store_true",
                      dest="verbose_report",
                      help="Produce the verbose form of the requested report.")
    parser.add_option("-l", "--largest-num", action="store",
                      dest="n_objs", type="int",
                      help="Print, at most, the largest N_OBJS " \
                           "text and code objects.")
    parser.add_option("-d", "--diff", action="store_true", dest="output_diff",
                      help="Used on a report, to indicate there are two "
                      "files and the differences should be reported.")


    (options, args) = parser.parse_args(args)


    if len(args) < 1 or args[0] is "":
        parser.error("Missing file argument(s)")

    xml_strings = get_xml_strings(options, args)
    if not xml_strings:
        sys.exit(1)

    if options.output_diff and not options.report:
        parser.error("Diff only makes sense on a report.")

    if options.report:
        if options.output_diff:
            if len(args) != 2:
                parser.error("Report diffs require two files.")

            input_string, diff_string = xml_strings
            gen_diff_report(options.report,
                            input_string,
                            diff_string,
                            options.n_objs,
                            options.verbose_report)
        else:
            gen_report(options.report,
                       xml_strings[0],
                       options.n_objs,
                       options.verbose_report)
    else:
        print >> sys.stderr, 'No report type specified.'
        print >> sys.stderr, 'Please specify with -r or --report.'
        print >> sys.stderr, 'Valid reports are:'

        for rep in REPORTS_SET:
            print >> sys.stderr, '  ' + rep



def xml_verify(target):
    """Pass the XML file through xmllint"""
    xmllint = Popen('xmllint --path "tools/pyelf/dtd ../../dtd ../dtd" '\
                        '-o /dev/null --valid -', shell=True,
                    stdin=PIPE)
    xmllint.stdin.write(target)
    xmllint.stdin.close()
    os.waitpid(xmllint.pid, 0)

def gen_report(reportname, xml_str, n_objs, verbose):
    """Generate the named report from a given XML string"""
    try:
        parsed = Memstats_el.parse_xml_str(xml_str)
    except EzXMLError, text:
        print >> sys.stderr, text
        sys.exit(1)

    if reportname in REPORTS_SET:
        report = REPORTS_SET[reportname](parsed)
        report.generate(n_objs, verbose)
    else:
        print >> sys.stderr, 'Error: "%s" is not a valid report name.' % \
              reportname
        print >> sys.stderr, 'Valid report names are:'

        for rep in REPORTS_SET:
            print >> sys.stderr, '  ' + rep

        sys.exit(1)

def gen_diff_report(reportname, xml_str, diff_xml_str, n_objs, verbose):
    """Generate a diff version of the named report from the given XML files"""
    try:
        parsed_main = Memstats_el.parse_xml_str(xml_str)
        parsed_old = Memstats_el.parse_xml_str(diff_xml_str)
    except EzXMLError, text:
        print >> sys.stderr, text

        sys.exit(1)

    if reportname in REPORTS_SET:
        report_main = REPORTS_SET[reportname](parsed_main)
        report_old = REPORTS_SET[reportname](parsed_old)
        report_main.generate_diff(n_objs, verbose, report_old)
    else:
        print >> sys.stderr, 'Error: "%s" is not a valid report name' % \
              reportname
        print >> sys.stderr, 'Valid report names are:'

        for rep in REPORTS_SET:
            print >> sys.stderr, '  ' + rep

        sys.exit(1)


def get_xml_strings(options, input_files):
    """
    Extract the xml from the input files and verify and/or write it out to disk
    if required.

    The input_files are the filenames of either Elf or XML files.

    Returns a list of strings, each containing the xml from the corresponding
    input file.
    """
    for input_file in input_files:
        if not os.path.exists(input_file):
            print >> sys.stderr, 'Error: File "%s" does not exist.' % input_file
            return None

    # Step 1. Read in the input files, which could be either elf or xml format.
    # We need to get a list of XML string objects.
    xml_strings = []
    for input_file in input_files:
        try:
            elf = PreparedElfFile(filename=input_file)
            xml_string = get_xml_from_elf(elf)

            if xml_string is None:
                print >> sys.stderr, \
                      'Error: ELF file "%s" does not contain memstats data.' % \
                      input_file
                sys.exit(1)

        except ElfFormatError:
            # It wasn't an elf, so we'll treat it as an XML file.
            xml_string = open(input_file).read()
        xml_strings.append(xml_string)

    # Step 2. Verify the XML if required
    if options.verify_xml:
        for xml_string in xml_strings:
            xml_verify(xml_string)

    # Step 3. Write out the XML if required
    if options.output_xml:
        output_xml = open(options.output_xml, "w")
        output_xml.write(xml_strings[0])
        output_xml.close()

    return xml_strings


def get_xml_from_elf(elf):
    """
    This retrieves the memstats XML from the elf file and returns it as a
    string.
    """
    note = elf.find_section_named(MEMSTATS_SECTION_NAME)

    if note is None:
        return None

    return "".join([chr(c) for c in note.get_desc_data() if c])

register_subcommand('memstats', memstats_cmd,
                    usage = "Report on the memory stats of an image",
                    is_hidden = True)

if __name__ == '__main__':
    memstats_cmd(sys.argv[1:])
