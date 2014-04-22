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
The modify subcommand of elfweaver.

This subcommand is designed to make changes to an existing ELF file to
accommodate the limitations of various boot-loaders.
"""

import optparse
from os import chmod, path
from stat import S_IRWXU, S_IRGRP, S_IXGRP, S_IROTH, S_IXOTH
from elf.File import File
from elf.core import UnpreparedElfFile
from elf.constants import SHT_PROGBITS, SHT_NOBITS
from elf.section import UnpreparedElfNote
from elf.ByteArray import ByteArray
from weaver.subcommand import register_subcommand

class UnknownField(Exception):
    """Exception thrown when an unknown field is encountered."""
    pass

def adjust(elf, field_designator, offset, absolute):
    """Adjust the physical address of each segment."""
    if field_designator[0] == "segment":
        # We do something for every segment
        for segment in elf.segments:
            if field_designator[1] == "paddr":
                if absolute:
                    segment.paddr = offset
                else:
                    segment.paddr += offset
            else:
                print "Don't know about field_designator %s." % \
                      field_designator[1]
    else:
        raise UnknownField, "Don't know about field %s" % field_designator[0]

def change(elf, field_designator, old, new):
    """Change the physical address of each segment."""
    if field_designator[0] == "segment":
        for segment in elf.segments:
            if field_designator[1] == "paddr":
                if segment.paddr == old:
                    segment.paddr = new
    else:
        raise UnknownField, "Don't know about field %s" % field_designator[0]

def merge_sections(elf, name):
    """Merge together sections with the same name prefix."""
    print "Merging elf sections", name
    base_section = None
    for section in elf.sections[:]:
        if base_section:
            if section.name.startswith(name):
                base_section.append_data(section.get_data())
                elf.remove_section(section)
            else:
                break
        if not base_section and section.name.startswith(name):
            base_section = section
            base_section.name = name
            base_section.set_in_segment_offset(0)

def get_binary_sections(segments):
    """
    Search through a list of segments and return a list of
    sections and padding suitable for writing out a binary boot image.
    """

    valid_sections = []
    # Get all segments containing sections, sort by phys addr
    valid_segments = [seg for seg in segments if seg.has_sections()]
    valid_segments.sort(key=lambda x: x.paddr)

    # paddr will be continuous since we have sorted segments by it.
    # Set it to the lowest paddr
    # We will track it as we go.
    if len(valid_segments) >= 1:
        paddr = valid_segments[0].paddr
    else:
        return valid_sections

    # vaddr of each segment will jump around a fair bit, set it inside loop
    vaddr = 0
    for segment in valid_segments:
        vaddr = segment.vaddr

        # Make sure our phys addr is correct before we start
        # iterating the sections.
        if segment.paddr > paddr:
            padding = segment.paddr - paddr
            valid_sections.append(("segment padding", padding))
            paddr += padding

        if segment.is_scatter_load():
            # Scatter load files are strange, no copy out exactly
            # what's required.
            for section in segment.get_sections():
                if section.type == SHT_NOBITS:
                    valid_sections.append(("section padding",
                                           section.get_size()))
                else:
                    valid_sections.append(("section", section))

            vaddr += segment.get_memsz()
            paddr += segment.get_memsz()
        else:
            for section in segment.get_sections():
                # If we skipped a section, pad it before moving on.
                if section.address > vaddr:
                    padding = section.address - vaddr
                    valid_sections.append(("section padding", padding))

                    vaddr += padding
                    paddr += padding

                valid_sections.append(("section", section))
                vaddr += section.get_size()
                paddr += section.get_size()

    return valid_sections

def write_binary_file(sections, filename):
    """
    Write a list of sections to a file.
    'sections' is a list of tuples, the first element is a key:
    If the key is "section", the second element is an elf section
    If the key is "padding", the second element is amount to pad
    """

    # We write out the ByteArrays
    outfile = File(filename, "wb", 0)

    for (tag, section) in sections:
        if tag == "section":
            outfile.write(section.get_data())
        elif tag == "segment padding" or tag == "section padding":
            outfile.seek(section, 1)

    # Truncate the file to turn any trailing BSS sections into zeros.
    outfile.truncate()

    outfile.close()

    # For compatibility with objcopy, finally make the file "rwxr-xr-x".
    chmod(filename, (S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH))

def modify(options, elf):
    for (field_desc, offset) in options.adjust:
        absolute = True
        if offset.startswith("+") or offset.startswith("-"):
            absolute = False
        offset = long(offset, 0)
        field_desc = field_desc.split(".")
        adjust(elf, field_desc, offset, absolute)

    for (field_desc, mod) in options.change:
        field_desc = field_desc.split(".")
        (old, new) = [long(x, 0) for x in mod.split("=")]
        change(elf, field_desc, old, new)

    for section_name in options.merge_sections:
        merge_sections(elf, section_name)

    if options.physical_entry:
        entry = elf.entry_point
        for segment in elf.segments:
            if segment.contains_vaddr(entry):
                offset = segment.vaddr - segment.paddr
                elf.entry_point = entry - offset

    if options.physical:
        for segment in elf.segments:
            offset = segment.vaddr - segment.paddr
            segment.vaddr = segment.paddr
            for section in segment.get_sections():
                section.address -= offset

    if options.remove_section_headers:
        elf.remove_section_headers()


    if options.strip:
        elf.strip()

    if options.remove_nobits:
        elf.remove_nobits()

    if options.binary:
        valid_sections = get_binary_sections(elf.segments)
        write_binary_file(valid_sections, options.output)

        # We do not write out the file as per normal modify invocation
        return

    if options.add_note:
        note = UnpreparedElfNote(elf, ".okl_config")
        note.note_name = "config_type"
        note.note_type = 1
        note._desc_data = ByteArray(options.add_note)
        elf.add_section(note)

def modify_cmd(args):
    """Merge command call from main. This parses command line
    arguments and calls merge, which does all the main work."""
    parser = optparse.OptionParser("%prog modify file [options]",
                                   add_help_option=0)

    parser.add_option("-H", "--help", action="help")
    parser.add_option("--adjust", nargs=2, dest="adjust",
                      action="append", default=[])
    parser.add_option("--physical", dest="physical",
                      action="store_true")
    parser.add_option("--physical_entry", dest="physical_entry",
                      action="store_true")
    parser.add_option("--change", dest="change",
                      action="append", nargs=2, default=[])
    parser.add_option("--merge_sections", dest="merge_sections",
                      action="append", default=[])
    parser.add_option("--no-section-headers", dest="remove_section_headers",
		      action="store_true")
    parser.add_option("--remove_nobits", dest="remove_nobits",
                      action="store_true")
    parser.add_option("--binary", dest="binary", action="store_true",
                      help="Write out boot binary")
    parser.add_option("--add_note", dest="add_note",
                      help="Add a note to the image")
    parser.add_option("-o", "--output", dest="output", metavar="FILE",
                      help="Target output file")
    parser.add_option("--strip", dest="strip",
                      help="remove .strtab and .symtab sections",
                      action="store_true")

    (options, args) = parser.parse_args(args)

    if len(args) != 1:
        parser.error("Must supply a file to operate on.")

    filename = args[0]

    elf = UnpreparedElfFile(filename=filename)

    if options.output is None:
        if options.binary:
            # Output file is base name plus ".bin"
            output_ext = ".bin"
            (filename_root, filename_ext) = path.splitext(filename)
            (_, file_name) = path.split(filename_root)
            options.output = file_name + output_ext
        else:
            # Output back to the same file
            (_, file_name_ext) = path.split(filename)
            options.output = file_name_ext

    modify(options, elf)

    if options.binary:
        return

    elf = elf.prepare(elf.wordsize, elf.endianess)
    elf.to_filename(options.output)

register_subcommand('modify', modify_cmd,
                    usage = "Modify the properties of an ELF file",
                    is_hidden = True)
