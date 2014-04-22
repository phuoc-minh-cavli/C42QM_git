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

"""The merge command is the main purpose of elf weaver. It provides a
way for merging many ELF files into one for the purpose of creating a
bootable image.

The merge_cmd function is the input from weaver.main. It parses command
line arguments and calls the merge() function which does most of the work.

The merge() command can also be called from another python library.
"""
import os
import gc
from optparse import OptionParser
from weaver.parse_spec import parse_spec_xml, parse_spec_xml_string, \
     XmlCollector
from weaver.ezxml import size_attr, ParsedElement
import weaver.image
import weaver.pools
import weaver.namespace
import re
from weaver import MergeError
from weaver.subcommand import register_subcommand
from weaver.machine import Machine
from weaver.kernel import Kernel
from weaver.pools import VirtualPool, PhysicalPool
from weaver.memstats import MEMSTATS_SECTION_NAME
from weaver.memstats_generator import memstats_generate, write_out_memstats
from weaver.relocs import revert_relocs, apply_relocs
from weaver.image import valid_segment

from elf.util import align_up

from elf.section import UnpreparedElfNote
from elf.core import UnpreparedElfFile
from elf.constants import PT_IA_64_UNWIND
from elf.ByteArray import ByteArray
from weaver.modify import modify
from elf.constants import ET_REL
from weaver.physinfo import print_physinfo

def collect_image_objects(parsed, ignore_name, kernel_heap_size,
                          namespace, image, cust, last_phys):
    """
    Extract the information in the XML elements and shove them into
    various data structures for later memory layout and image
    processing.
    """

    # FIXME: collect and handle extension namespaces
    machine_el = parsed.find_child("machine")
    machine = XmlCollector.COLLECTORS[machine_el.tag]()
    machine.collect_xml(machine_el, ignore_name)

    kernel_el = parsed.find_child("kernel")
    kernel = XmlCollector.COLLECTORS[kernel_el.tag]()
    # Load the kernel to fetch the autogen machine properties.
    kernel.load_kernel(kernel_el, machine, cust)

    pools = weaver.pools.Pools()

    for el in parsed.find_children("virtual_pool"):
        pool = XmlCollector.COLLECTORS[el.tag](pools, machine)
        pool.collect_xml(el, machine, pools)

    for el in parsed.find_children("physical_pool"):
        pool = XmlCollector.COLLECTORS[el.tag](pools, machine)
        pool.collect_xml(el, machine, pools)

    kernel.collect_xml(kernel_el, kernel_heap_size, namespace, image,
                       machine, pools, cust)

    if image.is_unified:
        machine.assert_no_virtual_mem()
        pools.assert_no_virtpools()

    # The tage names of the non-cell elements of an image.
    SYSTEM_TAGS = (Machine.element.name, VirtualPool.element.name,
                   PhysicalPool.element.name, Kernel.element.name)

    cell_els = []
    for cell_el in parsed.children:
        if cell_el.tag not in SYSTEM_TAGS:
            if not ignore_name.match(cell_el.name):
                cell_els.append(cell_el)
            elif last_phys:
                # if we are doing last_phys calculation, ensure
                # kernel heaps still get allocated correctly
                null_cell_el = ParsedElement('null_cell')
                null_cell_el.name = "ignored_" + cell_el.name
                null_cell_el.kernel_heap = cell_el.kernel_heap

                cell_els.append(null_cell_el)

    # All top level elements should be known.
    assert [cell_el for cell_el in cell_els
            if cell_el.tag not in XmlCollector.COLLECTORS] == []

    # Create Cell objects to handle the remains elements.
    found_cells = []

    if image.is_unified and len(cell_els) > 1:
        raise MergeError("Only one cell element is allowed when building "
                         "OKL4 Pico images.")

    # At this point the machine and pools objects are fully initialsed from
    # the contents of their XML element and will not change.
    #
    # Now the top level cell elements will be parsed at Cell objects made for
    # each one.

    for cell_el in cell_els:
        cell = XmlCollector.COLLECTORS[cell_el.tag]()
        if cell_el.name == "oklinux":
            cell_el.name = "vmlinux"
        cell.collect_xml(cell_el, ignore_name, namespace, machine, pools,
                         kernel, image)
        found_cells.append(cell)

    return machine, kernel, pools, found_cells

def relocate_to_offset(name, section_prefix, image, machine):
    elf = UnpreparedElfFile(name)

    revert_relocs(elf, machine)

    relocs = elf.get_reloc_sections()

    remove_sects = []
    for sect in elf.sections:
        if sect.type == PT_IA_64_UNWIND:
            remove_sects.append(sect)
            continue

        pref_sec_name = weaver.image.prefixed_section_name(section_prefix,
                                                           sect.name)
        image_sect = image.find_section_named(pref_sec_name)
        if image_sect is None:
            continue

        offset = image_sect.address - sect.address

        sect.address += offset

    for sect in remove_sects:
        elf.remove_section(sect)

    remove_segs = []
    for seg in elf.segments:
        if not valid_segment(seg):
            remove_segs.append(seg)
            continue

        offset = seg.sections[0].address - seg.vaddr
        seg.vaddr += offset

    for seg in remove_segs:
        elf.segments.remove(seg)

    apply_relocs(elf, machine)

    for sect in elf.get_reloc_sections():
        elf.remove_section(sect)

    return elf


def merge(spec_file, options):
    """Based on a given spec file, process it and merge into
    the output_file."""

    image = weaver.image.Image(options.program_header_offset)
    machine = None

    # Root of the namespace to hold cap rights to objects.
    namespace = weaver.namespace.ObjectNameSpace(None, '/')

    try:
        # parsed is an in memory tree of Element Objects
        # which is created from spec_file

        Machine.register()
        VirtualPool.register()
        PhysicalPool.register()
        Kernel.register()

        if options.spec_is_string:
            parsed = parse_spec_xml_string(spec_file)
        else:
            parsed = parse_spec_xml(spec_file)

        machine, kernel, pools, found_cells = \
            collect_image_objects(parsed, options.ignore_name,
                                  options.kernel_heap_size,
                                  namespace, image, options.cust, options.last_phys)

        # At this point the XML has been processed and will not be
        # referred to any longer.  Image, kernel and environment
        # objects for everything listed in the document should have
        # been made, however at this point addresses and cap values
        # are not known.

        # If the user hasn't specified the type of image to write,
        # then use the image type specified by the machine. If there
        # are no machine-specific defaults, we generate a virtually
        # addressed ELF image.
        if options.binary is None and machine.image_binary is not None:
            options.binary = machine.image_binary

        if options.physical is None and machine.image_physical is not None:
            options.physical = machine.image_physical

        if options.physical_entry is None and \
                machine.image_physical_entry is not None:
            options.physical_entry = machine.image_physical_entry

        if options.remove_nobits is None and \
               machine.image_remove_nobits is not None:
            options.remove_nobits = machine.image_remove_nobits
        if options.strip is None and machine.image_strip is not None:
            options.strip = machine.image_strip

        # We assume that at least one cell, other than the kernel,
        # should be defined.
        if len(found_cells) == 0:
            raise MergeError("At least one cell must be defined in the XML file.")

        # For each cell resolve any references to objects in other cells.
        for cell in found_cells:
            cell.resolve_cross_cell_dependencies(machine, image)

        # For each found cell generate any dynamic segments that need
        # processing
        for cell in found_cells:
            cell.generate_dynamic_segments(machine, image)

        # At this point every object that will appear in the image and
        # environment, or needs address space allocation must have
        # been created and their sizes fixed.

        # Kernel dynamic elements need to be performed after all the
        # cells have been completed in order for the cell's
        # environment memsections to be populated before we generate
        # the kernel's init script
        kernel.create_dynamic_segments(pools)

        # At this point every object that will appear in the kernel
        # heap must appear in the kernel object.  Also every
        # relationship between objects is also known (eg you know that
        # object A will contains pointers to these N objects, even of
        # the addresses of the N objects is yet to be calculated).
        #
        # It is now too late to add mappings, environment entries or
        # loadable ELF segments.

        image.layout(machine)

        # At this point the address of everything is either known or
        # can be calculated from a known addresses.

        # Print out where the free part of each pool starts.  Do here
        # before the free memory is turned into an libokl4 free pool.
        if options.last_phys:
            print pools.print_last_phys()

        # Kernel post procesing now that addresses (esp of the heap)
        # are known.
        kernel.layout_cells_post()

        # Per cell post address processing.  Remember that the kernel
        # heap structures are yet to be created.
        for cell in found_cells:
            cell.layout_cells_post(image, machine)

        # Write out the initialied kernel heap.  Note: This function
        # now adds sections and segments for the kernel heap.  That is
        # why we need to move image.prepare() to after this function.
        kernel.generate_init_script(image, machine)

        # At this point the kernel heap is fixed and all cap values
        # are known.
        #
        # Perform any generation code that needs to be run for each
        # found cell.  This includes creating the environment and
        # relocating ELF segments.
        for cell in found_cells:
            cell.generate_init(machine, image)

        # At this point is is still possible to change the contents of
        # the image but great care must be taken.  It's far to late to
        # be making major changes to the contents.

        # Generate the memstats xml from the internal elfweaver representation
        # of the system.
        dom = memstats_generate(image, machine, kernel)

        # We create a note section in the image in which to place the memstats
        # details. The section must be created before we prepare the file.
        memstat_sec = UnpreparedElfNote(image.elf, MEMSTATS_SECTION_NAME)
        memstat_sec.note_name = "Memstats Details"
        memstat_sec.note_type = 1
        memstat_sec._desc_data = \
                               ByteArray([0] *
                                         align_up(len(dom.toprettyxml()),
                                                          8))
        image.elf.add_section(memstat_sec)

        image.make_phys_reloc_table(machine, pools, found_cells[0].sas)

        # The ELF file must be prepared to calculate the offset value of
        # the segment info records in the environment.
        gc.collect()

        image.prepare()

        image.apply_patches()

        # At this point the entire contents of the image is fixed.

        # Output the memstats results to the note in the ELF image.
        write_out_memstats(image.elf, dom.toprettyxml())
       
        if options.physically_relocatable_image:
            image.elf.elf_type =ET_REL

        image.write_out_image(options.output_file)
        
        print_physinfo(image, machine, options)

    finally:
        # If wanted, print out the final tree.
        if options.dump_layout:
            image.dump()

        if options.dump_namespace:
            print '\n'.join(namespace.dump())

        if options.emit_relocs:
            offset = 0x1000000

            for (name, section_prefix, elf) in image.constituent_elfs:
                base = os.path.basename(name)
                dir =  os.path.dirname(options.output_file)
                reloc_name = os.path.join(dir, base) + ".reloc"

                # The kernel ELF is already prepared.
                if name == 'kernel':
                    prep = elf
                else:
                    prep = relocate_to_offset(name, section_prefix, image.elf,
                                              machine).prepare(32, "<")

                prep.to_filename(reloc_name)

def merge_cmd(args):
    """Merge command call from main. This parses command line
    arguments and calls merge, which does all the main work."""
    parser = OptionParser("%prog merge [options] specfile", add_help_option=0)
    parser.add_option("-H", "--help", action="help")
    parser.add_option("-o", "--output", dest="output_file", metavar="FILE",
                      help="Destination filename")
    parser.add_option("-l", "--lastphys", dest="last_phys",
                      action="store_true", default=False,
                      help="After merging, print the next available " \
                      "physical address in each pool.")
    parser.add_option('-k', "--kernel-heap-size", dest="kernel_heap_size",
                      action="store", default="0x0L",
                      help="Specify the size of the kernel heap, " \
                      "overridding the value in the specfile.")
    parser.add_option('-i', "--ignore", dest="ignore_name",
                      action="store", default="^$",
                      help="A regex specifying programs to be ignored.")
    parser.add_option('-S', "--string", dest="spec_is_string",
                      action="store_true",
                      help="Treat the specfile argument as an XML string.")
    parser.add_option('-m', "--map", dest="dump_layout",
                      action="store_true",
                      help="Dump a memory map of the final image.")
    parser.add_option("-n", "--namespace", dest="dump_namespace",
                      action="store_true", default=False,
                      help="Dump the namespace after the merge.")
    parser.add_option('-p', "--program-header-offset",
                      dest="program_header_offset",
                      action="store", type="long", default=None,
                      help="Set the program header offset in the ELF file.")
    parser.add_option('-y', "--verify", dest="verify",
                      action="store_true", default=False,
                      help="Verify that the specfile conforms to the DTD.")
    parser.add_option('-c', "--custom-path", dest="custom_path",
                      action="store", default="",
                      help="The directory containing custom cell, "
                           "extension and ABI packages")
    parser.add_option("-u", "--cust", dest="cust", default=None,
                      help="Specify the customer being targeted. [required]")

    parser.add_option("-e", "--emit-relocs", dest="emit_relocs",
                      action="store_true", default=False,
                      help="After merging, emit the original ELFs relocated "\
                           "to their final position in the image.")
    # Modify options
    parser.add_option("--adjust", nargs=2, dest="adjust",
                      action="append", default=[])
    parser.add_option("--physical", dest="physical",
                      default=None, action="store_true")
    parser.add_option("--virtual", dest="physical",
                      action="store_false")
    parser.add_option("--physical_entry", dest="physical_entry",
                      default=None, action="store_true")
    parser.add_option("--virtual_entry", dest="physical_entry",
                      action="store_false")
    parser.add_option("--change", dest="change",
                      action="append", nargs=2, default=[])
    parser.add_option("--merge_sections", dest="merge_sections",
                      action="append", default=[])
    parser.add_option("--no-section-headers", dest="remove_section_headers",
		      action="store_true")
    parser.add_option("--remove_nobits", dest="remove_nobits",
                      action="store_true")
    parser.add_option("--binary", dest="binary", action="store_true",
                      default=None, help="Write out binary image")
    parser.add_option("--elf", dest="binary", action="store_false",
                      help="Write out ELF image")
    parser.add_option("--add_note", dest="add_note",
                      help="Add a note to the image")
    parser.add_option("--strip", dest="strip",
                      help="remove .strtab and .symtab sections",
                      action="store_true")
    parser.add_option("--physically_relocatable_image", dest="physically_relocatable_image",
                      help="Make image physically relocatable",
                      action="store_true")


    (options, args) = parser.parse_args(args)

    custom_path = options.custom_path
    if not custom_path:
        custom_path = os.environ.get("ELFWEAVER_CUSTOM_PATH")
    weaver.cells.import_custom_cells(custom_path)
    weaver.extensions.import_custom_extensions(custom_path)

    options.kernel_heap_size = size_attr(options.kernel_heap_size)
    options.ignore_name = re.compile(options.ignore_name)

    if len(args) != 1:
        parser.error("Expecting a spec file.")

    # During elfweaver-extensions development, verify the document.
    if options.verify and not options.spec_is_string:
        xmllint = os.system('xmllint ' \
                            '--path "tools/pyelf/dtd ../../dtd ../dtd" ' \
                            '-o /dev/null --valid %s' % args[0])
        if xmllint != 0:
            return xmllint

    spec_file = args[0]

    if options.output_file is None:
        output_ext = ".boot"
        (spec_file_root, _) = os.path.splitext(spec_file)
        (_, spec_file_name) = os.path.split(spec_file_root)
        options.output_file = spec_file_name + output_ext

    merge(spec_file, options)

    if (options.adjust or options.change or options.merge_sections or
        options.physical_entry or options.physical or
        options.remove_section_headers or options.strip or
        options.remove_nobits or options.binary or options.add_note):
        options.output = options.output_file

        elf = UnpreparedElfFile(filename=options.output)

        modify(options, elf)

        if options.binary:
            return

        elf = elf.prepare(elf.wordsize, elf.endianess)

        elf.to_filename(options.output)

    return 0

register_subcommand('merge', merge_cmd,
                    usage = "Merge a set of files into one ELF",
                    is_hidden = False)
