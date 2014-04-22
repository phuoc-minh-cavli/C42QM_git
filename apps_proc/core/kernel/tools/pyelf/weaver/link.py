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
Implement a command to link a relocatable ELF file, no symbol matching is done
only application of reloc sections.  This should work the same as as using ld.
"""

import os
import sys
from optparse import OptionParser

from elf.abi import KERNEL_SOC_LINKER_SCRIPT
from elf.core import UnpreparedElfFile, SectionedElfSegment
from elf.constants import PF_W, PF_R, PF_X, SHF_WRITE, SHF_EXECINSTR, \
        SHT_SYMTAB, ET_EXEC, SHF_ALLOC, PT_LOAD, PT_ARM_EXIDX
from elf.segnames import get_segment_names, add_segment_names

from elf.abi.linker_commands import merge_sections
from weaver.subcommand import register_subcommand
from weaver.linker_script import perform_link, standard_link
from weaver.elfweaver_info import find_elfweaver_info
from weaver import MergeError

import weaver.autogen_helper

def link_cmd(args):
    """
    Link a relocatable ELF
    """

    # Link command shouldn't be run by end users!
    print >> sys.stderr, "********"
    print >> sys.stderr, "WARNING: The link command is for internal debugging " \
                         "and testing purposes only."
    print >> sys.stderr, "********"
    print

    parser = OptionParser("%prog link [options] <file1.o> [<file2.o>, ...]",
                          add_help_option=0)
    parser.add_option("-H", "--help", action="help")
    parser.add_option("-o", "--output", dest="output_file", metavar="FILE",
                      help="Destination filename")
    parser.add_option("--section-start", action="callback", nargs=1,
                      type="string", dest="section_vaddr",
                      callback=parse_options, metavar="SECT_NAME=SECT_ADDR",
                      help="Specify the virtual address of a given section")
    parser.add_option("-T", action="callback", nargs=1, type="string",
                      dest="section_vaddr", callback=parse_options,
                      metavar="(bss|data|text) SECT_ADDR",
                      help="Specify the virtaul address of the bss, data or"
                           "text sections")
    parser.add_option("--kernel-soc", action="store_true", default=False,
                      dest="kernel_soc",
                      help="Specify that we are linking the kernel and soc")
    parser.add_option("--rvct", action="store_true", default=False,
                      dest="rvct", help="Use rvct specific linker script")
    parser.add_option("-v", "--verbose", action="store_true", default=False,
                      dest="verbose", help="Print out processing messages")
    parser.add_option("--verbose-merge", action="store_true", default=False,
                      dest="verbose_merge",
                      help="Print out very detailed merging information")
    parser.add_option("--verbose-script", action="store_true", default=False,
                      dest="verbose_script",
                      help="Print out linker script handling information")
    parser.add_option("--verbose-relocs", action="store_true", default=False,
                      dest="verbose_relocs", help="Print out all reloc info")

    (options, args) = parser.parse_args(args)

    if len(args) < 1:
        parser.error("Invalid number of arguments")

    if options.output_file is None:
        parser.error("Must specify output file")

    elf = link(args, options.section_vaddr, options.kernel_soc, options.rvct,
               False, options.verbose, options.verbose_merge,
               options.verbose_script, options.verbose_relocs)

    seglist = get_segment_names(elf)
    add_segment_names(elf, seglist)

    if options.verbose:
        print "Writing out file"

    out_file = elf.prepare(elf.wordsize, elf.endianess)
    out_file.to_filename(options.output_file)

    os.chmod(options.output_file, 0750)

    if options.verbose:
        print "Done"

def link(files, section_vaddr = None, kernel_soc = True, rvct = False,
         partial_link = False, verbose = False, verbose_merge = False,
         verbose_script = False, verbose_relocs = False):
    """
    Perform the actual link, split so that elfweaver merge can call this easily.
    """

    # Handle merging of multiple files.
    # Use the first provided elf file as the base file.  For each additonal file
    # merge the sections (.text -> .text) but do no other merging i.e. do not
    # merge any .text.foo into .text.  Update the symbol table and any relocs
    # to take into account the merging then merge in the symbol table and
    # relocation sections.
    base_elf = UnpreparedElfFile(files[0])

    if kernel_soc:
        elfweaver_info = find_elfweaver_info(base_elf)

        if elfweaver_info is None:
            raise MergeError("Cannot find kernel information in file %s" % \
                             files[0])

        elfweaver_info.output()

        product = elfweaver_info.product
        arch = elfweaver_info.arch
        if arch == "armv7-a":
            arch = "armv7"
        if arch == "armv5te":
            arch = "armv5"
        cpu_desc = elfweaver_info.cpu_desc
        cpu_features = elfweaver_info.cpu_features
        debug = elfweaver_info.debug

        # Setup the Autogen variable based on our current kernel.
        weaver.autogen_helper.initialise(None, product, arch, cpu_desc,
                                         cpu_features, debug, base_elf.endianess)

        # Tell the ELF ABI code what the target arch is.
        base_elf.abi.set_arch(arch)
    if verbose:
        print "Using %s as base file" % files[0]

    base_sym_tab = [sym_tab for sym_tab in base_elf.sections
                    if sym_tab.type == SHT_SYMTAB]

    # Things get really, really ugly if there is more than one symbol table,
    # fortunately sane compilers / linkers appear to only have one anyway.
    assert len(base_sym_tab) == 1

    base_sym_tab = base_sym_tab[0]

    for merge_file in files[1:]:
        merge_elf = UnpreparedElfFile(merge_file)

        if verbose:
            print "Merging in file %s" % merge_file

        del sym_tab # Stop pylint complaints about var reuse.
        sym_tab = [sym_tab for sym_tab in merge_elf.sections
                   if sym_tab.type == SHT_SYMTAB]

        # Things get really, really ugly if there is more than one symbol table,
        # fortunately sane compilers / linkers appear to only have one anyway.
        assert len(sym_tab) == 1

        sym_tab = sym_tab[0]

        merged_sects = {}

        for sect in merge_elf.sections[1:]:
            # Symbol table and relocations require more specific magic and get
            # handled later on
            if sect.type == SHT_SYMTAB:
                continue
            elif sect in merge_elf.reloc_sects:
                continue

            found_sect = base_elf.find_section_named(sect.name)
            if found_sect is None:
                # Don't need to merge this section as there is no corrosponding
                # entry in the base file, so just go ahead and add it.
                base_elf.add_section(sect)

                if verbose_merge:
                    print "\tAdding section %s" % sect.name

                continue

            merge_sections(found_sect, sect, merged_sects, None,
                           verbose_merge)

        # Update any symbols or relocations that relied on a merged section
        # to correctly point at the new section at the correct offset
        if verbose:
            print "\tUpdating relocation sections with merged data"

        sym_tab.update_merged_sections(merged_sects)
        for sect in merge_elf.reloc_sects:
            sect.update_merged_sections(merged_sects)

        # Merge the symbol tables, this is more just tricky than any deep magic
        # * For each undefined symbol in the base file try to find a match
        #   in the input file.  If we find one then replace the base file's
        #   symbol with the defined one.  Keep a list of the mappings from the
        #   input files symbols to the new base file symbol index.
        # * Merge the two symbol tables.  For each symbol in the input file's
        #   symbol table;
        #   * If it is undefined, try to find a match in the base file's symbol
        #     table.  If found record the mapping from old symbol to new index.
        #   * If it is defined or there is no match copy it over, again keeping
        #     a mapping from old symbol to new index.
        # * Update all the relocations in the input file to correctly point at
        #   the new symbol table and the correct symbol index.  And merge in
        #   the relocations sections if a section already exists or add them.
        if verbose:
            print "\tMerging symbol tables"

        merged_syms = base_sym_tab.resolve(sym_tab)
        merged_syms.update(base_sym_tab.merge_symbols(sym_tab))

        for sect in merge_elf.reloc_sects:
            sect.update_merged_symbols(base_sym_tab, merged_syms)

        for sect in merge_elf.reloc_sects:
            found_sect = base_elf.find_section_named(sect.name)

            if found_sect is None:
                base_elf.add_section(sect)

                if verbose_merge:
                    print "\tAdding relocation section %s" % sect.name
            else:
                found_sect.append_relocs(sect.get_relocs())

                if verbose_merge:
                    print "\tMerging in relocation section %s" % sect.name

    # Now before we lay everything out we need to adjust the size of any
    # sections (such as the .bss or .got) that may increase in size due
    # to allocation of symbols, etc. Relocations must be allocated before
    # the symbol table, because they may add new symbols.
    if verbose:
        print "Allocating symbol and relocation data"

    for sect in base_elf.reloc_sects:
        #pylint: disable-msg=E1103
        if verbose_relocs:
            sect.set_verbose(verbose_relocs)
        sect.allocate_relocs()

    base_sym_tab.allocate_symbols()

    # Do any linker scripty things we need to do.  For the moment we either do
    # a standard link or a kernel+soc link, the actions performed are in python
    # functions currently but may be moved into external scripts later.
    if kernel_soc:
        if verbose:
            print "Performing a kernel + soc link, rvct = ", rvct
            print "Product %s, arch %s, cpu_desc %s" % (product, arch, cpu_desc)

        kernel_link_func_types = KERNEL_SOC_LINKER_SCRIPT[arch]
        if not rvct:
            kernel_link_func = kernel_link_func_types['gnu']
        else:
            kernel_link_func = kernel_link_func_types['rvct']

        # rom_sym = base_elf.find_symbol('__phys_addr_rom')

        segments, merged_sects, discarded_sects = \
                perform_link(base_elf, base_sym_tab, kernel_link_func,
                             product, cpu_desc, verbose_script)
    else:
        if verbose:
            print "Performing standard link"

        segments, merged_sects, discarded_sects = \
                perform_link(base_elf, base_sym_tab, standard_link,
                             section_vaddr, verbose_script)

    # Remove any symbols relating to discarded sections and update for any of
    # the merged sections
    if verbose:
        print "Updating symbols for discarded and merged sections"

    discarded_syms = base_sym_tab.update_discarded_sections(discarded_sects)
    base_sym_tab.update_merged_sections(merged_sects)

    if verbose:
        print "Updating relocation sections with new symbols"

    for sect in base_elf.reloc_sects:
        sect.update_discarded_symbols(discarded_syms)
        sect.update_discarded_sections(discarded_sects)
        sect.update_merged_sections(merged_sects)

    # Segments don't have the correct flags yet, go through and update them
    # based on the types of the included sections.
    for segment in segments:
        segment.flags = PF_R

        for sect in segment.sections:
            if sect.flags & SHF_WRITE:
                segment.flags |= PF_W
            if sect.flags & SHF_EXECINSTR:
                segment.flags |= PF_X

    for segment in segments:
        if segment.sections == []:
            continue

        new_seg = SectionedElfSegment(base_elf, segment.segment_type,
                segment.virt_addr, segment.virt_addr, segment.flags,
                segment.align, sections = segment.sections)
        base_elf.add_segment(new_seg)

        if verbose:
            print "Adding segment to file"
            print "\tFlags %d (R %d, W %d, X %d)" % \
                    (segment.flags, segment.flags & PF_R > 0,
                     segment.flags & PF_W > 0, segment.flags & PF_X > 0)
            print "\tVirt address %x" % segment.virt_addr
            print "\tSections", [sect.name for sect in segment.sections]

    if partial_link:
        # Explicity break the entry point for a partial link so that files
        # don't appear to magically work in the merge when other things
        # are broken
        base_elf.entry_point = 0x0

        return base_elf

    #
    # For the final written image, only sections in a segment appear
    # in the image.  Any section that wants to occupy memory in the
    # running program, but is _not_ in a segment and has not
    # explicitly been discarded is considered a fatal error.  This
    # detects problems with the linker script.
    #

    # Get the list of all sections allocated to segments.
    seg_sects = [sect for seg in base_elf.segments
                 if seg.type in (PT_LOAD, PT_ARM_EXIDX)
                 for sect in seg.sections]

    for sect in base_elf.sections:
        if (sect not in seg_sects and
            sect not in discarded_sects and
            (sect.flags & SHF_ALLOC) != 0):
            raise MergeError, \
                  "Section '%s' implicitly dropped during merge." % \
                  sect.name

    return link_apply_relocs(base_elf, verbose)

def link_apply_relocs(elf, verbose = False):
    """
    Due to wanting to do partial links (leaving relocations unapplied) for
    elfweaver merge's the following code has been removed from the actual
    link function.
    """

    # All sections are laid out and at their final size, go through and update
    # symbol values to their final position
    if verbose:
        print "Updating symbols"

    # Link process has ensured there is only one symbol table
    sym_tab = [sect for sect in elf.sections if sect.type == SHT_SYMTAB][0]

    sym_tab.update_symbols()

    # Make sure that there are no undefined symbols.
    sym_tab.check_all_defined()

    relocated_all = True
    # Apply all the relocs we know how to handle
    relocs_remove = []

    if verbose:
        print "Applying relocations"

    for sect in elf.reloc_sects:
        if sect.apply():
            relocated_all = False
        else:
            relocs_remove.append(sect)

    if verbose:
        print "Applied all relocations", relocated_all

    for sect in relocs_remove:
        elf.remove_section(sect)

    if relocated_all:
        elf.entry_point = elf.find_symbol("_start").value

        elf.elf_type = ET_EXEC

        if verbose:
            print "Setting entry point to %x" % elf.entry_point

    if verbose:
        print "Adding default segnames"

    return elf

def parse_options(option, opt, value, parser):
    """
    Implement ld's interface for section naming
    --section-start section=addr
    -Tbss addr
    -Tdata addr
    -Ttext addr
    """

    if getattr(parser.values, option.dest) is None:
        setattr(parser.values, option.dest, {})

    if opt == "--section-start":
        args = value.split('=')
        getattr(parser.values, option.dest)[args[0]] = int(args[1], 16)
    elif opt == "-T":
        if value in ("bss", "data", "text"):
            # Trickery to reuse -T for two types of operation.  While the
            # parser only has one argument for this type nothing stops us
            # manually eating additional arguments.
            getattr(parser.values, option.dest)['.' + value] = \
                    int(parser.rargs.pop(0), 16)

register_subcommand('link', link_cmd,
                    usage = "Link a kernel together",
                    is_hidden = True)
