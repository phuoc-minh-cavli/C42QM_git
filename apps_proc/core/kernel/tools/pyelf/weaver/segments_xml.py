##############################################################################
# Australian Public Licence B (OZPLB)
# 
# Version 1-0
# 
# Copyright (c) 2006-2011, Open Kernel Labs, Inc.
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

"""Process segment XML elements."""

import re
from elf.constants import PF_X, PF_W, PF_R, ET_EXEC, ET_DYN, \
        PT_PHDR, PT_DYNAMIC, SHN_ABS, SHN_UNDEF, STB_WEAK, STT_FILE
from elf.segnames import get_segment_names
from weaver import MergeError
import weaver.image
from weaver.ezxml import Element, long_attr, bool_attr, str_attr, size_attr
from weaver.relocs import revert_relocs
from elf.ByteArray import ByteArray

# General element
Segment_el = Element("segment",
                     name = (str_attr, "required"),
                     phys_addr = (long_attr, "optional"),
                     physpool = (str_attr, "optional"),
                     virt_addr = (long_attr, "optional"),
                     virtpool = (str_attr, "optional"),
                     align = (size_attr, "optional"),
                     attach = (str_attr,  "optional"),
                     direct = (bool_attr, "optional"),
                     pager = (str_attr, "optional"),
                     protected = (bool_attr, "optional"),
                     cache_policy = (str_attr, "optional"),
                     cache_policy_mask = (long_attr, "optional"),
                     mem_type = (str_attr, "optional"),
                     memsec_type = (str_attr, "optional")
                     )

Heap_el = Element("heap",
                  size = (size_attr, "optional"),
                  virt_addr = (long_attr, "optional"),
                  phys_addr = (long_attr, "optional"),
                  physpool = (str_attr, "optional"),
                  virtpool = (str_attr, "optional"),
                  align = (size_attr, "optional"),
                  attach = (str_attr,  "optional"),
                  direct = (bool_attr, "optional"),
                  zero = (bool_attr, "optional"),
                  pager = (str_attr, "optional"),
                  cache_policy = (str_attr, "optional"),
                  cache_policy_mask = (long_attr, "optional"),
                  mem_type = (str_attr, "optional"))

Patch_el = Element("patch",
                   address = (str_attr, "required"),
                   value = (long_attr, "required"),
                   bytes = (long_attr, "optional"))

def translate_mem_types(mem_type_str):
    """
    Translate the text description of the memory type into an object
    that describes the properties.
    """
    vals = {
        # Key        : (virt?, phys?  map?,  attach to mmu?)
        'virtual'    : (True,  False, False, False),
        'physical'   : (False, True,  False, True),
        'unattached' : (False, True,  False, False),
        'unmapped'   : (True,  True,  False, True),
        'default'    : (True,  True,  True,  True),
        }

    mem_vals = vals.get(mem_type_str)

    if mem_vals is not None:
        return weaver.image.ImageAttrs.MemType(*mem_vals)
    else:
        valid = vals.keys()
        valid.sort()

        raise MergeError, \
              'Unknown memory type "%s".  Supported types are %s' % \
              (mem_type_str, valid)

def start_to_value(start, elf):
    """
    Convert a start value, which may be a number, a text value of a
    number or a symbol, to a number.  ELF is used to resolve the value
    of a symbol.
    """
    # FIXME: depending on the type of start is not so nice
    if start is not None and not isinstance(start, (int, long)):
        if re.match("\s*0[xX][0-9a-fA-F]+$", start):
            start = long(start, 0)
        else:
            sym = elf.find_symbol(start)

            if not sym:
                raise MergeError, "Symbol %s not found" % (start)

            start = sym.value

    return start

def segments_dict(segments_el):
    """Convert a list of segments elements into a dict, indexed by name."""
    return dict([(segment_el.name, segment_el) for segment_el in segments_el])


def elf_segment_names(elf):
    """Return a dict of all ELF segment names, indexed by segment number.
    The value in the dictionary is a list of all the ELF segment's names.
    Note: An ELF segment can have multiple names!
    """

    # Case 1, the ELF segments are explicitly named in a .segment_names
    # section. If the user went to this much effort, then lets
    # respect that. Each segment will have a single name in this
    # case.
    seg_names_sect = elf.find_section_named(".segment_names")
    if seg_names_sect is not None:
        names = [[x] for x in seg_names_sect.get_strings()[1:]]
        return dict(enumerate(names))

    # Case 2, no explicit naming, we fall back to matching sections
    # to segments
    if elf.sections is not None:
        segment_names = {}
        for i, segment in enumerate(elf.segments):
            segment_names[i] = ["__idx_%d" % i]
            if hasattr(segment, "sections"):
                segment_names[i] += [x.name for x in segment.sections]

        return segment_names

    # Case 3, there is no explicit naming, we fall back to legacy
    # behaviour for now. This could go in the future.
    names = get_segment_names(elf)



def attach_to_elf_flags(attach):
    """Convert the attach 'rwx' string to ELF flags."""
    flags = 0L

    if "r" in attach:
        flags |= PF_R

    if "w" in attach:
        flags |= PF_W

    if "x" in attach:
        flags |= PF_X

    return flags

def make_pager_attr(pager):
    """Check that the pager value is valid."""
    valid_pagers = ("none", "default", "memload")

    # No pager given.
    if pager is None:
        return None

    if pager not in valid_pagers:
        raise MergeError, \
              '"%s" is not a recognised pager.  Valid values are %s.' % \
              (pager, valid_pagers)

    if pager is 'none':
        pager = None

    return pager

def collect_patches(elf, name_prefix, patch_els, filename, image):
    """Process 'patch' elements."""
    for patch_el in patch_els:
        if re.match("\s*0[Xx][0-9a-fA-F]+L?$", patch_el.address):
            # convert from a string hex representation to a number
            patch_el.address = long(patch_el.address, 0)
            # ensure we know how big it is
            if not hasattr(patch_el, "bytes"):
                raise MergeError, \
                      "Bytes attribute must be specified if patch " \
                      "address is a number."
        else:
            # look up address of symbol
            name = patch_el.address
            if name_prefix == "kernel":
                sym = elf.find_symbol(patch_el.address)
            else:
                sym = elf.find_symbol(name_prefix + patch_el.address)

            if not sym:
                if filename is None:
                    raise MergeError, \
                            "symbol %s not found in internaly linked kernel" % \
                            patch_el.address
                else:
                    raise MergeError, \
                            "symbol %s not found in %s" % (patch_el.address,
                                                           filename)

            patch_el.address = sym.value

            if not hasattr(patch_el, "bytes"):
                # set the size
                patch_el.bytes = sym.size
                if patch_el.bytes == 0:
                    raise MergeError, \
                          "Elf file does not specify size of symbol " \
                          "%s, please specify 'bytes' in this patch " \
                          "element" % name

        image.patch(sym, patch_el.value, patch_el.bytes)

def generate_section_prefix(cell_name):
    """Generate the section prefix"""
    return cell_name

def generate_symbol_prefix(section_prefix, prog_name):
    """Generate the prefix for symbols."""
    return section_prefix + '-' + prog_name + '-'

def collect_elf_segments(elf, segment_els, section_prefix,
                         symbol_prefix, drops, image_coln, namespace,
                         image, machine):
    """
    Process all of the segment elements in a program/kernel, etc.
    """
    def has_text_section(segment):
        """
        Return whether or not the segment contains a text section.
        """
        if not segment.has_sections():
            return False
        else:
            return len([sect for sect in segment.get_sections() if
                        sect.name == '.text']) != 0

    elf_seg_names = elf_segment_names(elf)
    sdict = segments_dict(segment_els)

    # Test that every segment element references a segment in the ELF
    # file.
    elf_seg_names_txt = sum(elf_seg_names.values(), [])

    for seg_name in sdict:
        if seg_name not in elf_seg_names_txt:
            raise MergeError, \
                  '%s: Cannot find segment "%s" in the ELF file. ' \
                  'Valid values are %s' % \
                  (namespace.abs_name(), seg_name, elf_seg_names_txt)

    # Test that every segment has a name.
    for i in range(len(elf.segments)):
        if i not in elf_seg_names:
            raise MergeError, \
                  '%s: Cannot find segment #%d\'s name in the ELF file. '

    collected_segments = []

    last_segment = None # The last segment processed.
    # Current list of segments that could share pages.
    subpage_group = []

    # Revert any relocations if there are any reloc sections
    relocate = revert_relocs(elf, machine)

    for i, segment in enumerate(elf.segments):
        seg_names = elf_seg_names[i]
        if len(seg_names) > 1:
            name = seg_names[1].lstrip(".")
        else:
            name = seg_names[0]

        # Sometimes we get segment name conflicts, if so append segment
        # index. This is a bit ugly...
        try:
            ns_node = namespace.add_namespace(name, "ELF segment")
        except MergeError:
            name = "%s_%d" % (name, i)
            ns_node = namespace.add_namespace(name, "ELF segment")

        attrs = image.new_attrs(ns_node, for_segment = True)

        if not image.is_unified:
            attrs.set_default_virt_addr(segment.vaddr)
            attrs.attach = segment.flags & (PF_R | PF_W | PF_X)
        attrs.elf_flags = segment.flags
#        attrs.align = segment.align

        # Put the text segment of relocatable files on a 1M virtual
        # boundary, otherwise align to what the segment requests.
        # This can be overridden by setting the 'align' attribute.
        force_alignment = (relocate and has_text_section(segment))

        if drops and any(lambda x: x in drops for x in seg_names):
            continue

        force_virt_addr = False
        # Find if there is a segment element matches this segment
        found_segment_el = None
        for seg_el_name, element in sdict.iteritems():
            if seg_el_name in seg_names:
                found_segment_el = element
                break
        if found_segment_el:
            segment_el = found_segment_el

            attrs.phys_addr = getattr(segment_el, 'phys_addr', attrs.phys_addr)
            attrs.physpool = getattr(segment_el, 'physpool', attrs.physpool)

            force_virt_addr = hasattr(segment_el, 'virt_addr')

            if relocate:
                if force_virt_addr:
                    relocate = False

                attrs.set_default_virt_addr(getattr(segment_el,
                                                    'virt_addr', None))
            elif force_virt_addr:
                raise MergeError("Cannot pass a virt_addr tag for a "
                                 "segment on a non reloc ELF")

            attrs.virtpool = getattr(segment_el, 'virtpool', attrs.virtpool)

            attrs.align = getattr(segment_el, 'align', attrs.align)
            attrs.pager = getattr(segment_el, 'pager', attrs.pager)
            attrs.direct = getattr(segment_el, 'direct', attrs.direct)
            attrs.protected = getattr(segment_el, 'protected', attrs.protected)

            if hasattr(segment_el, 'align'):
                attrs.align = segment_el.align
                force_alignment = False

            if hasattr(segment_el, 'attach'):
                attrs.attach = attach_to_elf_flags(segment_el.attach)

            if hasattr(segment_el, 'pager'):
                attrs.pager = make_pager_attr(segment_el.pager)

            if hasattr(segment_el, 'cache_policy'):
                attrs.cache_attr = \
                                   machine.get_cache_attr(segment_el.cache_policy)

            if hasattr(segment_el, 'mem_type'):
                attrs.mem_type = translate_mem_types(segment_el.mem_type)

            if hasattr(segment_el, 'memsec_type'):
                if segment_el.mem_type != "unmapped" and \
                        segment_el.memsec_type == "paged":
                    raise MergeError("Only unmapped segments can be created "\
                            "as paged memsections")
                attrs.memsec_type = segment_el.memsec_type

        iseg = image_coln.add_segment(segment_index = i,
                                      section_prefix = section_prefix,
                                      elf_segment = segment,
                                      symbol_prefix = symbol_prefix,
                                      attrs = attrs,
                                      machine = machine,
                                      relocate = relocate,
                                      force_virt_addr = force_virt_addr,
                                      force_alignment = force_alignment)

        if iseg is not None:
            collected_segments.append(iseg)

            # It is possible for different segments to occupy the same
            # page of memory (yes!  really!).  To accommodate this
            # place segments that have the same flags (RO, RW, etc)
            # into a subpage group and tell the image collection
            # about it.

            if last_segment is not None and \
                   last_segment.flags != segment.flags:
                image_coln.add_subpage_group(subpage_group)
                subpage_group = []
                iseg.set_force_page(True)

            subpage_group.append(iseg)

            last_segment = segment

    image_coln.add_subpage_group(subpage_group)


    # Find all the elf's absolute and unresolved weak symbols, and copy them
    # over. Drop the file symbols, because they're useless if the symbol
    # table is reordered.
    absolute_symbols = [(idx, sym) for idx, sym in
                            enumerate(elf.get_symbol_table().symbols)
                        if (sym.shndx == SHN_ABS and sym.type != STT_FILE) or
                           (sym.shndx == SHN_UNDEF and sym.bind == STB_WEAK)]
    image_coln.add_absolute_symbols(absolute_symbols, symbol_prefix)

    # Find the .got section, using the section prefix.
    copied_got_table = image.elf.find_section_named(section_prefix + '.got')

    # Now copy over the temporary reloc sections into the main merged_sections,
    # and store the .got section we should be using for those sections.
    image.elf.reloc_merge_sections.append((copied_got_table,
                                           image.elf.reloc_merge_sections_temp,
                                           image.elf.reloc_merge_symbols_temp))
    image.elf.reloc_merge_sections_temp = []
    image.elf.reloc_merge_symbols_temp = {}

    return collected_segments
