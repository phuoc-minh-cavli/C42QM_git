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
Functions for the segment name section.

The segment name section maps strings to segment indexes.
"""

from elf.section import PreparedElfSection, UnpreparedElfStringTable
from elf.constants import PF_R, PF_W, PF_X, PT_LOAD

def get_segment_names(elf, seglist = None, orphans = None):
    """
    Return a list of names that correspond to the segments in the ELF
    file.

    The names can come from the command line, a file, the program
    headers listed in a linker script or generated from the flag
    values of the segments themselves.
    """

    if orphans is None:
        orphans = []

    # segments specifed in a file
    if seglist is not None:
        seglist = _pad_non_loaded(elf, seglist)
    # otherwise we use the defaults
    else:
        seglist = []

    # If no segment names are found, then generate default names.
    # Loadable segments are named according to their ELF flags.  Other
    # segments are unnamed because they are unused by elfweaver.
    if len(seglist) < len(elf.segments):
        seglist_enum = []
        segcount = 0

        for seg in elf.segments[len(seglist):]:
            seg_name = ""

            if seg.has_sections():
                for sect in seg.get_sections():
                    if sect.name in orphans:
                        seg_name = sect.name

            if seg_name == "":
                if seg.flags & PF_R:
                    seg_name += 'r'

                if seg.flags & PF_W:
                    seg_name += 'w'

                if seg.flags & PF_X:
                    seg_name += 'x'

                if seg_name == "":
                    seg_name = "??"

            seglist_enum.append((segcount, seg_name))
            segcount += 1

        # Find and flag all segments which have non-unique flags
        counts = count([seg.flags for seg in elf.segments])
        duplicates = [counts[seg.flags] > 1 for seg in elf.segments]

        # For all segments which have duplicate names:
        # First try to append the name of their first section
        # Failing that, append their segment number
        # (which is actually its position in the segment lsit)
        # Finally, construct the seglist that will be returned
        for i, seg_name in seglist_enum:
            is_dup = duplicates[i]
            if is_dup == True and seg_name not in orphans:
                seg_i = elf.segments[i]
                if seg_i.has_sections():
                    first_sect_name = seg_i.get_sections()[0].name
                    if first_sect_name.startswith("."):
                        seg_name += "-" + first_sect_name.split(".")[1]
                    else:
                        seg_name += "-" + first_sect_name
                else:
                    seg_name += "-%02d" % i
            seglist.append(seg_name)

    return seglist

class SegnameElfStringTable(UnpreparedElfStringTable):
    """
    A customised string table implementation that allows nul strings
    to appear in places other than the start of the table.  This will
    mean that there will be exactly <n> string if there are <n>
    segments, even if some of the segments are unnamed.

    The calculation of the offset is very inefficient if the number of
    segments is large
    """
    prepares_to = PreparedElfSection

    def __init__(self, elffile):
        UnpreparedElfStringTable.__init__(self, elffile, ".segment_names")

    def add_string(self, string):
        """Add a new string to the table. Return the data offset."""
        offset = sum([len(s) for s in self.strings])

        self.strings.append(string + '\x00')
        self.offsets[string] = offset
        self.string_dict[offset] = string
        return offset

def add_segment_names(elf, seglist):
    """
    Add a segment name string table to an elf file, containing the given list
    of segment names
    """
    # create the string table
    segname_tab = SegnameElfStringTable(elf)

    # add the segment names
    for segname in seglist:
        segname = segname.strip()
        segname_tab.add_string("%s" % segname)

    # add the table to the file
    elf.add_section(segname_tab)

def _seg_list_rvct(elf, segments_to_sections_dict):
    return [_seg_from_script_guess(seg, segments_to_sections_dict) for seg in elf.segments]

def _seg_from_script_guess(seg, segments_to_sections_dict):
    seg_score = _score_segments(seg, segments_to_sections_dict)
    max_score = 0
    best_guess = "ERROR: NO GUESS"
    for scored_seg in seg_score:
        if seg_score[scored_seg] > max_score:
            max_score = seg_score[scored_seg]
            best_guess = scored_seg

    return best_guess

def _score_segments(seg, segments_to_sections_dict):
    """ """
    return count([_seg_from_sec_script(sec, segments_to_sections_dict) for sec in seg.get_sections()])

def _seg_from_sec_script(sec, segments_to_sections_dict):
    """Goes through the dictionary of segment to section mappings from the
    linker script and returns the segment that contains a given section
    """
    for segment, script_sections in segments_to_sections_dict.items():
        if sec.name in script_sections:
            return segment

def count(items):
    """
    Return a dictionary counting the number of times each item appears in the
    input list.
    """
    counts = {}
    for item in items:
        counts[item] = counts.get(item, 0) + 1
    return counts

def _pad_non_loaded(elf, names):
    """
    Create a list of names for all segments in an ELF file from a list
    of names for the loadable segments.  Non-loadable segments are
    named ''.
    """

    if names == []:
        return []

    names = names[:] # Make a copy to manipulate.
    seglist = []

    for seg in elf.segments:
        if seg.type == PT_LOAD:
            seglist.append(names.pop(0))
        else:
            seglist.append("")

    return seglist
