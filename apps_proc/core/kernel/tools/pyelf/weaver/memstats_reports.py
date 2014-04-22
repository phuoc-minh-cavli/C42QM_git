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
Classes for the various types of reports which can be produces from the
memstats data.
"""

import re
import sys

EMPTY_PAGE_LIMIT = 10

SECTION_DROPS = re.compile(
        r".*(weaved|debug|segment_names|comment|bootinfo).*")

class MemorySetup(object):
    """
    Utility class to prep and report on segments and sections that came
    from the ELF image originally
    """
    def __init__(self, resources):
        self.segments = []
        self.unallocated_sections = []

        for segment in resources.find_child("segment_list").find_children("segment"):
            seg_sections = set(segment.sections.split(' '))

            self.segments.append({'secs'   : seg_sections,
                                  'phys'   : segment.phys,
                                  'memsz'  : segment.memsz,
                                  'filesz' : segment.filesz})

        self.section_list = \
                          resources.find_child("section_list").find_children("section")

    def find_segment(self, sec_id):
        for seg in self.segments:
            if sec_id in seg['secs']:
                return seg

        return None

    def find_section(self, phys_id):
        return first(self.section_list, lambda sec: sec.id, phys_id)

    def process_section(self, phys_section):
        curr_seg = self.find_segment(phys_section.id)

        if curr_seg is None:
            self.unallocated_sections.append(phys_section)

        return curr_seg


def first(items, func, val):
    matches = [None] + [item for item in items if func(item) == val]
    return matches[-1]

class Report(object):
    """ Common report functionality"""
    def __init__(self, parsed_xml):
        self.revision = parsed_xml.find_child("revision")

    @classmethod
    def print_revision(cls, revision):
        print "\tRepository: %s" % revision.repository
        print "\tChangeset: %s" % revision.changeset
        print

    def compare_revisions(self, old):
        fail = False
        if self.revision is not None and old.revision is not None:
            if self.revision.repository != old.revision.repository:
                print "Images built from different repositories"
                fail = True
            elif self.revision.changeset != old.revision.changeset:
                print "Revision details differ"
                fail = True
            if fail:
                print "Old:"
                Report.print_revision(old.revision)
                print"New:"
                Report.print_revision(self.revision)

        return fail

    @classmethod
    def names_union(cls, list_a, list_b, func):
        """
        Take the supplied lists and produce a single list with the
        union of the names in each.  func() is applied to each list
        element to obtain the name.
        """
        return list(set([func(x) for x in list_a + list_b]))

    @classmethod
    def cell_check(cls, old_cells, new_cells, cname):
        def cell_in_list(clist, name):
            return first(clist, lambda cell: cell[0], name)

        new_cell = cell_in_list(new_cells, cname)
        old_cell = cell_in_list(old_cells, cname)

        if new_cell is None: # only in old
            print "Cell %s removed" % cname
        elif old_cell is None: #only in new
            print "Cell %s added" % cname
        return new_cell, old_cell

class FixedUsage(Report):
    """
    Report on the memory usage by objects produced directly from the compiled
    code.
    """
    def __init__(self, parsed_xml):
        #
        # This is still grouped by program.  Need to find programs
        # in kernel space and cell spaces, then extract full info from
        # resources section.
        #
        Report.__init__(self, parsed_xml)
        self._programs = []
        environ = parsed_xml.find_child("environment")
        resources = parsed_xml.find_child("resources")

        # Find page size
        phys_mem = resources.find_child("phys_mem")
        self.page_size = phys_mem.page_size

        self._coll = MemorySetup(resources)

        kspace = environ.find_child("kernel").find_child("space")
        self.process_space(kspace)

        for cell in environ.find_children("cell"):
            for space in cell.find_children("space"):
                self.process_space(space)

    def print_program_stats(self, prog, n_objs, verbose):
        """ For the given program, output the size values"""

        print "Statistics for program %s:\n" % prog['name']
        print "\tSections (with debug and empty sections dropped):\n"

        prog_static_size = 0

        for (name, size) in prog['sections']:
            print "\t%-40s%d Bytes" % (name, size)
            prog_static_size += size
        print

        prog_rounded_size = sum([seg['memsz'] for seg in prog['segments']])
        if prog_rounded_size % self.page_size:
            full_pages = prog_rounded_size / self.page_size
            partial_pages = full_pages + 1
            prog_rounded_size = partial_pages * self.page_size

        rounded_size = prog_rounded_size / 1024.0
        pages = prog_rounded_size / self.page_size

        print "\tTotal static size by sections (unrounded) is %.2f KB\n" \
              % (prog_static_size / 1024.0)
        print "\tSize by rounded segments (%d total) is %.2f KB (%d pages)\n" \
              % (len(prog['segments']), rounded_size, pages)

        if verbose:
            self.object_lists(prog['text_objects'],
                              prog['data_objects'],
                              n_objs)

        return rounded_size

    def object_lists(self, func_list, data_list, n_objs):
        if func_list:
            func_list.sort(key=lambda x: -x[1])
            print "\tTop %d functions (from %d) by size:\n" \
                  % (len(func_list[:n_objs]), len(func_list))

        for (name, size) in func_list[:n_objs]:
            print "\t%-40s%d Bytes" % (name, size)

        if data_list:
            data_list.sort(key=lambda x: -x[1])
            print "\n\tTop %d global data elements (from %d) by size:\n" \
                  % (len(data_list[:n_objs]), len(data_list))

        for (name, size) in data_list[:n_objs]:
            print "\t%-40s%d Bytes" % (name, size)

    def generate_diff(self, n_objs, verbose, old):
        #
        # Produce a difference report instead of a "normal" report.
        #
        print

        if verbose:
            print "Note: diff report does not process text/data objects"
            print

        differences = self.compare_revisions(old)

        prognames = Report.names_union(self._programs, old._programs,
                                       lambda x: x['name'])
        def prog_in_list(programs, name):
            return first(programs, lambda prog: prog['name'], name)

        for pname in prognames:
            in_new = prog_in_list(self._programs, pname)
            in_old = prog_in_list(old._programs, pname)

            if in_new is None: # only in old
                print "Removed Program %s" % pname
                self.print_program_stats(in_new, n_objs, verbose)
                differences = True
            elif in_old is None: # only in new
                print "Added Program %s" % pname
                self.print_program_stats(in_new, n_objs, verbose)
                differences = True
            else:
                #
                # Work our way through the contents of each program
                # and compare.
                #
                new_sections = in_new['sections']
                old_sections = in_old['sections']
                secnames = Report.names_union(new_sections, old_sections,
                                              lambda x: x[0])
                def sec_in_list(sections, name):
                    return first(sections, lambda sec: sec[0], name)

                old_size_by_section = 0
                new_size_by_section = 0

                for name in secnames:
                    sec_new = sec_in_list(new_sections, name)
                    sec_old = sec_in_list(old_sections, name)

                    if sec_new is None: # only in old
                        print "Removed Section: %s: %d Bytes" % sec_old
                        old_size_by_section += sec_old[1]
                        differences = True
                    elif sec_old is None: # only in new
                        print "Removed Section: %s: %d Bytes" % sec_new
                        new_size_by_section += sec_new[1]
                        differences = True
                    else:
                        old_size_by_section += sec_old[1]
                        new_size_by_section += sec_new[1]

                        if sec_old[1] != sec_new[1]:
                            print "Section %s varies in size " \
                                "(old %d, new %d)" % \
                                (name, sec_old[1], sec_new[1])
                            differences = True


                size_string = "Size difference by %s in %s: old %d " \
                    "Bytes, new %d Bytes"
                if old_size_by_section != new_size_by_section:
                    print size_string % ("section", pname,
                                         old_size_by_section,
                                         new_size_by_section)
                    # differences already set

                def sum_segments(seg_list):
                    return sum([seg['filesz'] for seg in seg_list])

                new_size_by_segment = sum_segments(in_new['segments'])
                old_size_by_segment = sum_segments(in_old['segments'])

                if old_size_by_segment != new_size_by_segment:
                    print size_string % ("segment", pname,
                                         old_size_by_segment,
                                         new_size_by_segment)
                    differences = True

        if not differences:
            print "No differences."

        print


    def generate(self, n_objs, verbose):
        """
        Produce the report on stdout
        """
        total_rounded_size = 0

        if self.revision is not None:
            print "Mercurial Revision:"
            self.print_revision(self.revision)

        for prog in self._programs:
            rounded_size = self.print_program_stats(prog, n_objs, verbose)
            total_rounded_size += rounded_size
        total_pages = total_rounded_size * 1024.0 / self.page_size

        print "Total static size by rounded segments is %.2f KB (%d pages)" \
                % (total_rounded_size, total_pages)


    def process_space(self, space):

        for program in space.find_children("program"):
            program_desc = {'name' : program.name,
                            'segments' : [],
                            'sections' : [],
                            'text_objects' : [],
                            'data_objects' : []}

            for psec in program.find_child("psec_list").find_children("psec"):
                if SECTION_DROPS.match(psec.name):
                    continue

                phys_section = self._coll.find_section(psec.phys)
                assert(phys_section is not None)

                seg = self._coll.process_section(phys_section)

                if seg is not None:
                    program_desc['sections'].append((psec.name, 
                                                     phys_section.size))

                    if seg not in program_desc['segments']:
                        program_desc['segments'].append(seg)

            for obj in program.find_child("object_list").find_children("object"):
                if obj.type == "text":
                    program_desc["text_objects"].append((obj.name, obj.size))
                else:
                    program_desc["data_objects"].append((obj.name, obj.size))

            self._programs.append(program_desc)

class InitialUsage(Report):
    """
    Report on the physical memory which is in use at the end of the
    OKL4 kernel initialisation.
    """
    def __init__(self, parsed_xml):
        Report.__init__(self, parsed_xml)
        self.resources = parsed_xml.find_child("resources")
        self.environ = parsed_xml.find_child("environment")

        # Find page size
        phys_mem = self.resources.find_child("phys_mem")
        self.page_size = phys_mem.page_size

        self._coll = MemorySetup(self.resources)

        for phys_section in self._coll.section_list:
            self._coll.process_section(phys_section)

        self._heaps = []
        mempools = self.resources.find_child("pools").find_children("mem_pool")

        for heap in mempools:
            for sec in self.resources.find_all_children("section"):
                if sec.id == heap.node:
                    self._heaps.append(sec)

                    for usec in self._coll.unallocated_sections:
                        if usec.id == sec.id:
                            self._coll.unallocated_sections.remove(usec)


    def generate_diff(self, _, verbose, old):
        def sec_to_psec(environ, sec_id):
            for psec in environ.find_all_children("psec"):
                if psec.phys == sec_id:
                    return psec.name
            return ""

        differences = self.compare_revisions(old)

        #
        # Keeping this fairly simple - any variation in the number
        # of segments results in a quick exit.
        #
        if len(self._coll.segments) != len(old._coll.segments):
            print "Number of segments differ: %d vs %d" % \
                (len(self._coll.segments), len(old._coll.segments))
            differences = True
        else:
            memsizes = [(old_seg['memsz'], new['memsz']) for (old_seg, new) in
                        zip(old._coll.segments, self._coll.segments)]
            for count, (old_memsz, new_memsz) in enumerate(memsizes):
                if new_memsz != old_memsz:
                    print "Segment %d has size variation; old %d Bytes, " \
                        "new %d Bytes" %  (count, old_memsz, new_memsz)
                    differences = True

        #
        # Unallocated sections
        #
        def id_in_list(sec_list, sec_id):
            return first(sec_list, lambda sec: sec.id, sec_id)

        unallocated_ids = \
            Report.names_union(self._coll.unallocated_sections,
                               old._coll.unallocated_sections,
                               lambda x: x.id)
        if verbose:
            for sec_id in unallocated_ids:
                new_sec = id_in_list(self._coll.unallocated_sections, sec_id)
                old_sec = id_in_list(old._coll.unallocated_sections, sec_id)

                if new_sec is None:
                    print "%s removed" % sec_to_psec(old.environ, sec_id)
                    differences = True
                elif old_sec is None:
                    print "%s added" % sec_to_psec(self.environ, sec_id)
                    differences = True
        else:
            for sec_id in unallocated_ids:
                # Apologies for fomatting of next statement
                if (id_in_list(self._coll.unallocated_sections, sec_id) is None
                    or
                    id_in_list(old._coll.unallocated_sections, sec_id) is None):
                    print "Init script sections differ"
                    differences = True
                    break

        if not differences:
            print "No differences."

        print

    def generate(self, _, verbose):
        """
        Produce the report on stdout
        """
        def sec_to_psec(environ, sec_id):
            for psec in environ.find_all_children("psec"):
                if psec.phys == sec_id:
                    return psec.name
            return ""

        seg_len = len(self._coll.segments)

        seg_size = sum([seg['memsz'] for seg in self._coll.segments])
        seg_size = seg_size / 1024

        print "\nMemory in Use at end of initialization"
        print "=======================================\n"
        print "Memory claimed by ELF image:"

        if verbose:
            for seg_num, segment in enumerate(self._coll.segments):
                print "ELF Segment %d: %#8x - %#8x (%4dKB)" % \
                    (seg_num + 1, segment['phys'],
                     segment['phys'] + segment['memsz'] - 1,
                     segment['memsz'] / 1024)

            print

        print "%4d Segments giving %6dKB" % (seg_len, seg_size)

        print "\nMemory set up by Elfweaver:"

        for sec in self._coll.unallocated_sections:
            if verbose:
                name = sec_to_psec(self.environ, sec.id)
                if name == "":
                    #
                    # The only _expected_ case for this is the global
                    # thread handles.
                    #
                    for pool in self.resources.find_all_children("id_pool"):
                        if pool.source == sec.id:
                            name = pool.name

                print "%-40s%#8x - %#8x (%4dKB)" % (name,
                                                     sec.address,
                                                     sec.address + sec.size - 1,
                                                     sec.size / 1024)

        sec_len = len(self._coll.unallocated_sections)
        sec_size = sum([sec.size for sec in self._coll.unallocated_sections])
        sec_size = sec_size / 1024

        print "%4d additional sections giving %6dKB" % (sec_len, sec_size)

        for cell_num, heap in enumerate(self._heaps):
            print "\nCell %d:" % (cell_num + 1)
            hsz = heap.size / 1024
            heap_str = "  Heap (found in ELF image)"
            if verbose:
                print "%-40s%#8x - %#8x (%4dKB)" % \
                      (heap_str,
                       heap.address,
                       heap.address + heap.size - 1,
                       hsz)
            else:
                print "%-40s%6dKB" % (heap_str, hsz)

        final_size = seg_size + sec_size
        phys_size = 0
        phys_mem = self.resources.find_child("phys_mem")
        poolname = phys_mem.pool

        for sec in phys_mem.find_children("section"):
            if sec.id == poolname:
                phys_size = sec.size
                break

        phys_size = phys_size / 1024

        print "\nTotal Memory claimed for use: %8dKB\n" % final_size

        if phys_size != 0:
            print "Physical memory remaining: %dKB of %dKB\n" % \
                  ((phys_size - final_size), phys_size)

class HeapUsage(Report):
    """
    Report on the number of heaps, and how much of their storage has been used,
    at the end of OKL4 kernel initialisation.
    """
    def __init__(self, parsed_xml):
        Report.__init__(self, parsed_xml)
        env = parsed_xml.find_child("environment")
        resources = parsed_xml.find_child("resources")

        # Find page size
        phys_mem = resources.find_child("phys_mem")
        self.page_size = phys_mem.page_size

        if env.type == "okl4nano":
            print >> sys.stderr, "Heap Usage report not valid for Nano Kernel"
            sys.exit(1)

        pools = resources.find_child("pools")
        sections = resources.find_child("section_list")
        heaps = {}
        self.cells = []

        for pool in pools.find_children("mem_pool"):
            for sec in sections.find_children("section"):
                if sec.id == pool.node:
                    heaps[pool.id] = (pool, sec)

        for cell in parsed_xml.find_child("environment").find_children("cell"):
            heap = None
            allocs = []
            for pool in cell.find_children("pool"):
                if pool.vmid in heaps:
                    heap = heaps[pool.vmid]
            if heap is not None:
                self.cells.append((cell.name, heap, allocs))
                for heap_alloc in cell.find_child("heap_alloc_list").find_children("heap_alloc"):
                    allocs.append((heap_alloc.name, heap_alloc.module, heap_alloc.size))

    @classmethod
    def sum_heap(cls, heap):
        return sum([group.size for group in heap.find_children("group")])

    @classmethod
    def print_heap_allocs(cls, allocs, verbose):
        sorted_allocs = {}
        total_sizes = {}

        for (name, module, size) in allocs:
            if name in sorted_allocs:
                if module in sorted_allocs[name]:
                    if size in sorted_allocs[name][module]:
                        sorted_allocs[name][module][size] += 1
                    else:
                        sorted_allocs[name][module][size] = 1
                else:
                    sorted_allocs[name][module] = {size: 1}
            else:
                sorted_allocs[name] = {module: {size: 1}}

        for name in sorted_allocs:
            total = 0
            for module in sorted_allocs[name]:
                for size in sorted_allocs[name][module]:
                    total += size * sorted_allocs[name][module][size]
            total_sizes[name] = total

        size_ordered_allocs = [tuple(reversed(t)) for t in total_sizes.items()]
        size_ordered_allocs.sort()
        size_ordered_allocs.reverse()

        print "  Allocations:"

        for total, name in size_ordered_allocs:
            print "    %-23s -> %6d Bytes" % (name, total)
            for module in sorted_allocs[name]:
                breakout = False
                if breakout:
                    break
                for size in sorted_allocs[name][module]:
                    count = sorted_allocs[name][module][size]
                    if not verbose and size == total:
                        breakout = True
                        break
                    print "      (%2d x %5d Bytes" % (count, size),
                    if verbose or len(sorted_allocs[name]) > 1:
                        print ",  by %s )" % module
                    else:
                        print ")"

        total_allocations = sum(total_sizes[name] for name in total_sizes)
        print "  TOTAL = %d Bytes\n" % total_allocations

    def generate_diff(self, _, verbose, old):
        differences = self.compare_revisions(old)

        cell_names = Report.names_union(self.cells, old.cells,
                                        lambda x: x[0])
        def group_sizes(heap):
            groups = heap.find_children("group")
            return dict((group.name, group) for group in groups)

        for cname in cell_names:
            new_cell, old_cell = Report.cell_check(old.cells, self.cells, cname)

            if None in (new_cell, old_cell):
                differences = True
            else:
                # Compare heaps
                new_heap, new_sec = new_cell[1]
                old_heap, old_sec = old_cell[1]

                if new_heap.id != old_heap.id:
                    print "Cell %s heap id changed from %s to %s" % \
                        (cname, old_heap.id, new_heap.id)
                    differences = True

                if new_sec.size != old_sec.size:
                    print "Cell %s heap size changed from %#x to %#x" % \
                        (cname, old_sec.size, new_sec.size)
                    differences = True

                old_used = HeapUsage.sum_heap(old_heap)
                new_used = HeapUsage.sum_heap(new_heap)

                if old_used != new_used:
                    print "Cell %s heap usage from %d Bytes to %d Bytes" % \
                        (cname, old_used, new_used)
                    differences = True

                    if verbose:
                        new_groups = group_sizes(new_heap)
                        old_groups = group_sizes(old_heap)

                        for group in new_groups.keys():
                            if new_groups[group] != old_groups[group]:
                                print"\t%s: %d Bytes to %d Bytes" % \
                                    (group,
                                     old_groups[group],
                                     new_groups[group])

        if not differences:
            print "No differences."

        print

    def generate(self, _, verbose):
        """
        Produce the report on stdout
        """
        print "\nHeap Usage"
        print "===========\n"

        for count, cell in enumerate(self.cells):
            cellname = cell[0]
            (heap, sec) = cell[1]
            allocs = cell[2]
            used = HeapUsage.sum_heap(heap)
            print "Cell %d - '%s':" % ((count + 1), cellname)

            size = sec.size
            print "Heap is %s - size %d Bytes with %d Bytes used" % \
                  (heap.id, size, used)

            HeapUsage.print_heap_allocs(allocs, verbose)
        print


class CapUsage(Report):
    """
    Report on the Caps available, and the number used,
    at the end of OKL4 kernel initialisation.
    """
    def __init__(self, parsed_xml):
        Report.__init__(self, parsed_xml)
        #
        # Get all the id pools, then go back through the environment
        # to work out what has been allocated, and what has been used.
        #
        self.id_pools = []
        self.cells = []

        env = parsed_xml.find_child("environment")

        if env.type == "okl4nano":
            print >> sys.stderr, "IDs Usage report not valid for Nano Kernel."
            sys.exit(1)

        resources = parsed_xml.find_child("resources")
        ids = resources.find_child("ids")

        # Find page size
        phys_mem = resources.find_child("phys_mem")
        self.page_size = phys_mem.page_size

        for pool in ids.find_children("id_pool"):
            if hasattr(pool, "parent"):
                self.assign_slot(pool.parent)

            ranges = []

            for rnge in pool.find_children("range"):
                ranges.append({'id': rnge.id, 'size': rnge.size, 'used': 0})

            self.id_pools.append((pool.name, pool.total, ranges))

        for cell in env.find_children("cell"):
            cell_ids = dict((c_rnge.name, c_rnge.node) for \
                                c_rnge in cell.find_children("id_range"))
            # Get the used values in short form.
            for c_rnge in cell.find_children("id_range"):
                for rnge in ranges:
                    if rnge['id'] == c_rnge.node:
                        rnge['used'] = c_rnge.used

            #FIXME - Need to deal with long form when verbose reenabled.
            spaces = []

            self.cells.append((cell_ids, spaces))

    def assign_thread(self, handle, clist):
        """
        A thread has been created using the handle list and capability
        list provided.  Locate them and increment their usage count.
        """
        for pool in self.id_pools:
            for rnge in pool[2]:
                if (rnge['id'] == handle) or (rnge['id'] == clist):
                    rnge['used'] += 1

    def find_slot(self, list_id):
        """
        Find the id_range resource element identified by the id.
        """
        for pool in self.id_pools:
            for rnge in pool[2]:
                if rnge['id'] == list_id:
                    return rnge

    def assign_slot(self, list_id):
        """
        A new entity has been created using an id from the list identified
        by the parameter.  Increment its usage count.
        """
        rnge = self.find_slot(list_id)
        rnge['used'] += 1


    def generate_diff(self, _, verbose, old=None):
        if verbose:
            print "(No verbose option when comparing ID reports)"

        differences = self.compare_revisions(old)

        if len(self.id_pools) != len(old.id_pools):
            print "Different number of pools in old and new"
            print "List in old: "
            print ' '.join([pool[0] for pool in old.id_pools])

            print "List in new: "
            print ' '.join([pool[0] for pool in self.id_pools])

            differences = True
        else:
            pools = max(len(self.id_pools), len(old.id_pools))

            for index in range(pools):
                (new_name, new_total, new_range) = self.id_pools[index]
                (old_name, old_total, old_range) = old.id_pools[index]

                if old_name != new_name:
                    differences = True
                    print "Name change for list %d: was %s now %s" % \
                        (index, old_name, new_name)
                else:
                    if old_total != new_total:
                        differences = True
                        print "List %s has different length: was %d " \
                            "now %d" % (old_name, old_total, new_total)

                    if old_range != new_range:
                        differences = True
                        print "List %s has a different amount used: " \
                            "was %d now %d" % (old_name, old_range, new_range)

        if not differences:
            print "No differences."
        print


    def generate(self, _, verbose):
        """
        Produce the report on stdout
        """

        def process_id_lists(ids, format_str):
            """
            ID lists can occur at several levels - logic factored out
            for reuse.
            """
            for list_type in ids.keys():
                rnge_id = ids[list_type]
                rnge = self.find_slot(rnge_id)
                # Take the range, and strip the sequence number off
                # the end of the name, to give name of the enclosing
                # id pool
                id_pool = rnge_id[:-1]
                pad2 = ' ' * (16 - len(id_pool))
                print format_str %  (rnge['size'],
                                     rnge['used'])

        print "\nCap usage"
        print "=========\n"

        if verbose:
            print >> sys.stderr, "Verbose not available on caps report"
            sys.exit(-1)

        print

        ctype_str = "  %4d available,   %4d used"

        for count, (cell_ids, spaces) in enumerate(self.cells):
            print "Cell %d:" % (count + 1)
            process_id_lists(cell_ids, ctype_str)


class EnvSummary(object):
    """
    Produce a quick summary of the environment.  Added as a debugging tool,
    but may be useful as an overview without reading the XML
    """
    def __init__(self, parsed_xml):
        def process_cell_contents(cell, rep_obj):
            for space in cell.find_children("space"):
                rep_obj['spaces'].append(space.name)

                for prog in space.find_children("program"):
                    rep_obj['programs'].append((prog.name,
                                                len(prog.find_child("psec_list").find_children("psec"))))

                for id_range in space.find_children("id_range"):
                    count = 0
                    for src_range in resources.find_all_children("range"):
                        if src_range.id == id_range.node:
                            count = src_range.size

                    rep_obj['id_ranges'].append((count, id_range.name))

                for pool in space.find_children("pool"):
                    rep_obj['pools'].append((pool.id, pool.vmid))


        environment = parsed_xml.find_child("environment")
        resources = parsed_xml.find_child("resources")
        kernel_x = environment.find_child("kernel")
        cells_x = environment.find_children("cell")

        # Find page size
        phys_mem = resources.find_child("phys_mem")
        self.page_size = phys_mem.page_size

        self.kernel = {'spaces'    : [],
                       'programs'  : [],
                       'id_ranges' : [],
                       'pools'     : []}
        process_cell_contents(kernel_x, self.kernel)

        self.cells = []
        for cell in cells_x:
            new_cell = {'spaces'   : [],
                       'programs'  : [],
                       'id_ranges' : [],
                       'pools'     : []}
            process_cell_contents(cell, new_cell)
            self.cells.append((cell.name, new_cell))

    def generate_diff(self, _, verbose, old):
        print "Environment Summary report does not provide a " \
            "difference report."

    def generate(self, _, verbose):
        """
        Produce the report on stdout
        """
        def cell_out(cell):
            for space in cell['spaces']:
                print "  Space called " + space

            print "  Id Ranges:"
            for id_rng in cell['id_ranges']:
                print "    %d for %s" % id_rng

            print "  Pools:"
            for pool in cell['pools']:
                print "    %s on %s" % pool

            print "  Programs:"
            for prog in cell['programs']:
                print "    %s with %d sections" % prog
            print

        print "\nQuick summary of environment:\n"
        print "In kernel:"
        cell_out(self.kernel)

        for cell in self.cells:
            print "In Cell %s:" % cell[0]
            cell_out(cell[1])

REPORTS_SET = {'fixed'       : FixedUsage,
               'initial'     : InitialUsage,
               'heap'        : HeapUsage,
               'caps'        : CapUsage,
               'env-summary' : EnvSummary,
               }
