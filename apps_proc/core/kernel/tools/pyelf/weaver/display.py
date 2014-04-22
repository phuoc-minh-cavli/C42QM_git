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
Implements a command for displaying an ELF file in a format very
similar to readelf. It also is able to print out l4 data structures.
"""

import sys
from optparse import OptionParser
from elf.core import PreparedElfFile
from weaver.elfweaver_info import find_elfweaver_info
from weaver.subcommand import register_subcommand

def print_pheaders(elf, summary=True, fout=sys.stdout):
    """Print out the program headers of a given elf file. If summary is
    true, then provide a summary of the Elf file."""
    segments = elf.segments
    if summary:
        if not elf.has_segments():
            print >> fout
            print >> fout,  "There are no program headers in this file."
        else:
            print >> fout
            print >> fout,  "Elf file type is %s" % elf.elf_type
            print >> fout,  "Entry point 0x%x" % elf.entry_point
            print >> fout,  "There are %d program headers, starting at offset %s" % \
                  (len(segments), elf._ph_offset)
    if not elf.has_segments():
        return
    print >> fout
    print >> fout,  "Program Headers:"
    if elf.wordsize == 64:
        print >> fout,  "  Type           Offset             VirtAddr           PhysAddr\n" +  \
        "                 FileSiz            MemSiz              Flags  Align"
    else:
        print >> fout,  "  Type           Offset   VirtAddr   PhysAddr   " \
              "FileSiz MemSiz  Flg Align"

    for segment in segments:
        segment.get_program_header(elf.endianess, elf.wordsize).output(fout)
    if elf.sections:
        print >> fout
        print >> fout,  " Section to Segment mapping:"
        print >> fout,  "  Segment Sections..."
        for i, segment in enumerate(segments):
            print >> fout,  "   %02d    " % i,
            if segment.has_sections():
                for sect in segment.sections:
                    if sect.get_size():
                        print >> fout,  sect.name,
            print >> fout,  ""


def print_sheaders(elf, summary=True, fout=sys.stdout):
    """Print out the section headers of a given elf file. If summary is
    true, then provide a summary of the Elf file."""
    sections = elf.sections
    if summary:
        if sections is None or sections == []:
            print >> fout
            print >> fout, "There are no sections in this file."
        else:
            print >> fout, "There are %d section headers, starting at offset 0x%x:" % \
                  (len(sections), elf._sh_offset)
    if sections is None or sections == []:
        return
    hdrs = elf.get_section_headers()
    print >> fout
    print >> fout, "Section Headers:"
    if elf.wordsize == 64:
        print >> fout, "  [Nr] Name              Type             Address           Offset\n" \
                      "       Size              EntSize          Flags  Link  Info  Align"
    else:
        print >> fout, "  [Nr] Name              Type            Addr" \
              "     Off    Size   ES Flg Lk Inf Al"
    for sect in hdrs:
        sect.output(fout)

    print >> fout, "Key to Flags:"
    print >> fout, "  W (write), A (alloc), X (execute), M (merge), S (strings)"
    print >> fout, "  I (info), L (link order), G (group), x (unknown)"
    print >> fout, "  O (extra OS processing required) o (OS specific), "\
          "p (processor specific)"

def print_symbol_table(elf, fout=sys.stdout, wide=False):
    print >> fout, "   Num:    Value  Size Type    Bind   Vis      Ndx Name"

    for (idx, sym) in enumerate(elf.get_symbol_table().symbols):
        sym.output(idx, fout, wide)

def print_relocations(elf, fout=sys.stdout, wide=False):
    found = False
    for sec in elf.sections:
        if hasattr(sec, "relocs"):
            for reloc in sec.relocs:
                reloc.output(fout, wide)
                found = True
    if not found:
        print >> fout
        print >> fout, "There are no relocations in this file."

def print_cmd(args):
    """Display an ELF file. args contains command line arguments passed
    from sys.argv. These are parsed using option parser."""

    parser = OptionParser("%prog print [options] file", add_help_option=0)
    parser.add_option("-H", "--help", action="help")
    parser.add_option("-a", "--all", action="store_true", dest="all",
                      help="Print all information")
    parser.add_option("-h", "--header", action="store_true", dest="header",
                      help="Print ELF header")
    parser.add_option("-l", "--pheaders", action="store_true", dest="pheaders",
                       help="Print ELF sections headers")
    parser.add_option("-S", "--sheaders", action="store_true", dest="sheaders",
                      help="Print ELF sections headers")
#     parser.add_option("-k", "--kconfig", action="store_true", dest="kconfig",
#                       help="Print L4 kernel config data structure (Default)")
    parser.add_option("-m", "--segnames", action="store_true", dest="segnames",
                      help="Print segment names")
    parser.add_option("-s", "--syms", action="store_true", dest="symbols",
                      help="Print the symbol table")
    parser.add_option("-r", "--relocs", action="store_true", dest="relocs",
                      help="Display the relocations (if present)")
    parser.add_option("-W", "--wide", action="store_true",
                      dest="wide", default=False,
                      help="Allow output width to exceed 80 characters")
    parser.add_option("-e", "--elfweaver-info", action="store_true",
                      dest="elfweaverinfo", help="Print elfweaver info section")

    (options, args) = parser.parse_args(args)

    if len(args) != 1:
        parser.error("incorrect number of arguments")

    if options.all:
        options.header = options.pheaders = options.sheaders = \
                             options.elfweaverinfo = options.segnames = True

    elf = PreparedElfFile(filename=args[0])

    if options.header:
        elf.get_elf_header().output(sys.stdout)

    if options.sheaders:
        print_sheaders(elf, not options.header, sys.stdout)

    if options.pheaders:
        print_pheaders(elf, not options.header, sys.stdout)

    if options.symbols:
        print_symbol_table(elf, sys.stdout, options.wide)

    if options.relocs:
        print_relocations(elf, sys.stdout, options.wide)

    if options.elfweaverinfo:
        print
        elfweaverinfo = find_elfweaver_info(elf)
        if elfweaverinfo:
            elfweaverinfo.output(sys.stdout)
        else:
            print "There is no elfweaver info section in this file."

    if options.segnames:
        segcount = 0
        print
        segnames = get_segnames(elf)

# Segment name support has been removed.
#        if segnames:
#             print "Segment    Name"

#             for segname in segnames.strings[1:]:
#                 idx = segname.index('\x00')
#                 segname = segname[:idx]
#                 segname = segname.strip()

#                 if segname != "":
#                     print "  %02d       %s" % (segcount, segname)

#                 segcount += 1
#        else:
            # TODO - use the first section in each segment
            # as the segment name and print that.
        print "There is no .segnames section in this file"

    return 0


def get_segnames(elf):
    """Find the segnames data structructure in an ELF. Return none if it
    can't be found."""
    return elf.find_section_named(".segment_names")

register_subcommand('print', print_cmd,
                    usage = "Print display an ELF",
                    is_hidden = False)
