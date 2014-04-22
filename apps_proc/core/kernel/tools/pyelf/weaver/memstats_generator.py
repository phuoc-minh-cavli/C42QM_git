##############################################################################z
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
Generate the memstats report XML.
"""

import re
from xml.dom.minidom import getDOMImplementation, DocumentType

from elf.core import UnpreparedElfFile
from elf.ByteArray import ByteArray
from weaver.util import _0

#########################
# New elfweaver merge memstats implementation
#########################

# Old re from the original memstats, may be more complex than we actually
# need?
FUNC_SEC = re.compile(r'^[a-zA-Z0-9_]*\.(text).*')
DATA_SEC = re.compile(r'^[a-zA-Z0-9_]*\.(bss|data).*')
CODE = re.compile(r'Code')
DATA = re.compile(r'Data')
DOLLAR_DROPS = re.compile(r'^[^$].*')

class ElfweaverSection(object):
    """Pseudo-section for memory ranges allocated by Elfweaver"""
    def __init__(self, addr, size, name = ""):
        self.address = addr
        self.size = size
        self.name = name

    def get_size(self):
        return self.size

def get_symbols_in_section(elf, section):
    """
    Return the list of symbols in the given section.
    """
    # Because we don't update the .section element of symbols when we
    # prepare a file (they are all still UnpreparedElfSection's) the
    # core section_symbols() returns [].  So we write a quick function
    # to look up .section.prepared_to to get around this problem.

    if isinstance(elf, UnpreparedElfFile):
        return (sym for (_, sym ) in elf.section_symbols(section))
    else:
        return (x for x in elf.get_symbols() if x.section != None and
                x.section.prepared_to == section)

def gen_symbols_data(dom, parent, elf, sections):
    """
    Generate XML elements for ELF symbols.
    """
    text_num = 0
    data_num = 0

    object_list = dom.createElement("object_list")
    parent.appendChild(object_list)

    for sect, _ in sections:
        if isinstance(sect, ElfweaverSection):
            continue
        if FUNC_SEC.match(sect.name):
            for sym in get_symbols_in_section(elf, sect):
                if DOLLAR_DROPS.match(sym.name) and \
                       CODE.match(sym.get_type_string()):
                    elem = dom.createElement("object")

                    elem.setAttribute("type", "text")
                    elem.setAttribute("name", sym.name)
                    elem.setAttribute("size", str(sym.size))

                    object_list.appendChild(elem)
                    text_num += 1

        if DATA_SEC.match(sect.name):
            for sym in get_symbols_in_section(elf, sect):
                if DOLLAR_DROPS.match(sym.name) and \
                       DATA.match(sym.get_type_string()):
                    elem = dom.createElement("object")

                    elem.setAttribute("type", "data")
                    elem.setAttribute("name", sym.name)
                    elem.setAttribute("size", str(sym.size))

                    object_list.appendChild(elem)
                    data_num += 1

    object_list.setAttribute("num_text", str(text_num))
    object_list.setAttribute("num_data", str(data_num))

def memstats_generate(image, machine, kernel):
    """
    Generate an XML description of the image we are producing so that the
    memstats tool can be run to generate various reports about the system
    state.
    """

    # XXX: Figure out if we need it or how to get revision information
    #revision = ('test', 'v1.2.24')
    revision = None

    # Go through all valid sections and put them into a hash along with an
    # id number, this lets up later lookup the hash to get the id to reference
    # as 'phys<id>' or 'virt<id>' in various places.
    section_hash = {}
    sec_id = 0
    segments = [x for x in image.elf.segments if x.has_sections()]

    for seg in segments:
        for sect in [x for x in seg.get_sections() if x.get_size() != 0]:
            section_hash[sect] = sec_id
            sec_id += 1

    kernel_sections = [x for x in section_hash.iteritems()
                       if x[0].name.find('kernel') == 0]


    doc_type = DocumentType("memstats")
    doc_type.systemId = "memstats-2.0.1.dtd"
    dom = getDOMImplementation().createDocument(None, "memstats", doc_type)

    if revision is not None:
        elem = dom.createElement("revision")
        elem.setAttribute("repository", revision[0])
        elem.setAttribute("changeset",  revision[1])

        dom.documentElement.appendChild(elem)

    environment = dom.createElement("environment")
    environment.setAttribute("type", kernel.product)
    dom.documentElement.appendChild(environment)

    generate_kernel(dom, environment, kernel_sections, image.elf)

    vm_id = 1
    vmpool_id = 0
    heap_id = 0
    cell_sect_ids = {}
    mem_pools = []
    cap_lists = []
    # Per cell information, we are esentially doing the back converstion from
    # our internal representation (heap, domains, etc.) back to XML (cell, etc.)
    for name, heap in kernel.heaps.iteritems():
        # Only one domain per heap at the moment
        domain = heap.domains[0]

        cell = dom.createElement("cell")
        cell.setAttribute("name", name)
        environment.appendChild(cell)

        #
        # This function is currently called twice by the merge process.
        # For the first call the okl4_object instances referenced below will
        # NOT be set (it's used to guestimate the size of the XML string)
        # So we can't really do anything other than skip this bit.
        #
        # We only support generation of heap information on objmanager_simple.
        #
        if heap.okl4_object != None and \
                (heap.okl4_object.name == "simple" or \
                 heap.okl4_object.name == "pico"):
            # Add in the heap 
            heap_sect = ElfweaverSection(heap.phys_addr, heap.size)
            section_hash[heap_sect] = sec_id
            pool = dom.createElement("pool")
            pool.setAttribute("id", "cell_heap_%d" % heap_id)
            heap_name = "Heap%d" % heap_id
            pool.setAttribute("vmid", heap_name)
            cell.appendChild(pool)
            mem_pools.append((heap_name, sec_id, 
                              heap.size - heap.okl4_object.free_list_size()))
            sec_id += 1
            heap_id += 1

            heap_alloc_list = dom.createElement("heap_alloc_list")
            cell.appendChild(heap_alloc_list)
            for (size, objclass) in heap.okl4_object.allocs_log:
                heap_alloc = dom.createElement("heap_alloc")
                heap_alloc.setAttribute("name", objclass.__name__)
                heap_alloc.setAttribute("size", str(size))
                heap_alloc.setAttribute("module", objclass.__module__)
                heap_alloc_list.appendChild(heap_alloc)

            if hasattr(domain.okl4_object, "_domain"):
                if domain.okl4_object._domain is not None:
                    clist = domain.okl4_object._domain.objmanager.clist

                    clist_det = [len(clist), 0]
                    for cap in domain.okl4_object._domain.objmanager.clist:
                        if cap.type != 0:
                            clist_det[1] += 1
                    cap_lists.append(clist_det)
                    id_range = dom.createElement("id_range")
                    id_range.setAttribute("name", "all")
                    id_range.setAttribute("node", "all%d" % (len(cap_lists) - 1))
                    id_range.setAttribute("used", "%d" % clist_det[1])
                    cell.appendChild(id_range)

        # FIXME: We should output any heap specific information here, due to
        # changes in the structure of the kernel (no space id ranges, no
        # clist ranges, etc.) the old information we used to output is no
        # longer valid.  It should be replace here (and domain, thread, etc. as
        # needed) with entirely new flags and the reporting changed as
        # appropiate.

        # MMU_Contexts have no name, what should we be outputting?
        for mmu in domain.mmu_contexts:
            space = dom.createElement("space")
            space.setAttribute("name", name)
            cell.appendChild(space)

            pool = dom.createElement("pool")
            pool.setAttribute("id", "vm%d" % vm_id)
            pool.setAttribute("vmid", "vm_pool%d" % vmpool_id)
            space.appendChild(pool)

            # Now add in the (non-heap) mappings in the mmu_context.
            # Note that the kernel heap areas don't appear in an mmu context
            for (_, attr) in mmu.mappings:
                sname = attr.abs_name()[1:].replace('/', '.')
                new_sect = ElfweaverSection(attr.phys_addr, attr.size, sname)
                section_hash[new_sect] = sec_id
                sec_id += 1

            # Again, no named threads?
            # FIXME: thread is unused here... that doesn't seem right.
            for _ in mmu.threads:
                program = dom.createElement("program")
                program.setAttribute("name", name)
                space.appendChild(program)

                cell_sect_ids[name] = [vmpool_id, []]
                cell_sections = [x for x in section_hash.iteritems()
                                 if x[0].name.find(name) == 0]

                psec_list = dom.createElement("psec_list")
                program.appendChild(psec_list)

                for sect_id, (sect, id_) in enumerate(cell_sections):
                    psec = dom.createElement("psec")
                    psec.setAttribute("name", sect.name)
                    psec.setAttribute("phys", "phys%d" % id_)
                    psec.setAttribute("virt", "vmpool%d%d" %
                                      (vmpool_id, sect_id))
                    psec_list.appendChild(psec)

                    cell_sect_ids[name][1].append((sect, sect_id))

                a_thr = dom.createElement("thread")
                a_thr.setAttribute("pool_id", "Heap0")
                a_thr.setAttribute("handle_id", "")
                a_thr.setAttribute("clist_id", "all%d" % (len(cap_lists) - 1))
                space.appendChild(a_thr)

                gen_symbols_data(dom, program, image.elf, cell_sections)

                
        vm_id += 1
        vmpool_id += 1

    generate_resources(dom, dom.documentElement, image.elf, machine,
                       section_hash, segments, kernel_sections,
                       cell_sect_ids, mem_pools, cap_lists)

    return dom

def generate_kernel(dom, parent, kernel_sections, elf):
    """
    Generate XML elements for the kernel data.
    """
    # Generate the appropiate kernel tags, there should be a nice way to
    # genericify this as just another cell?  Otherwise we should drop the
    # irrelevant cell like tags (space, program) from the kernel entirely
    # and modify the reporting to handle it.
    kernel = dom.createElement("kernel")
    parent.appendChild(kernel)

    space = dom.createElement("space")
    space.setAttribute("name", "kspace")
    kernel.appendChild(space)

    elem = dom.createElement("pool")
    elem.setAttribute("id", "vm0")
    elem.setAttribute("vmid", "kspc")
    space.appendChild(elem)

    program = dom.createElement("program")
    program.setAttribute("name", "kernel")
    space.appendChild(program)

    psec_list = dom.createElement("psec_list")
    program.appendChild(psec_list)


    for sect, id_ in kernel_sections:
        elem = dom.createElement("psec")
        elem.setAttribute("name", sect.name)
        elem.setAttribute("phys", "phys%d" % id_)
        elem.setAttribute("virt", "kspc%d" % id_)

        psec_list.appendChild(elem)

    gen_symbols_data(dom, program, elf, kernel_sections)


def generate_resources(dom, parent, elf, machine, section_hash,
                       segments, kernel_sections, cell_sect_ids,
                       mem_pools, cap_lists):
    """
    Generate DOM elements for resources.
    """
    # Generate resource information here.
    resources = dom.createElement("resources")
    parent.appendChild(resources)

    phys_mem = dom.createElement("phys_mem")
    phys_mem.setAttribute("machine", str(elf.machine))
    phys_mem.setAttribute("name", machine.cpu)
    phys_mem.setAttribute("pool", "physical")
    phys_mem.setAttribute("page_size", "%#x" % machine.min_page_size())

    resources.appendChild(phys_mem)

    for id_, mem in machine.physical_mem.iteritems():
        elem = dom.createElement("section")
        elem.setAttribute("id", id_)
        elem.setAttribute("address", "%#x" % mem[0][0])
        elem.setAttribute("size", "%#x" % mem[0][1])
        phys_mem.appendChild(elem)

    section_list = dom.createElement("section_list")
    resources.appendChild(section_list)

    for sect, id_ in section_hash.iteritems():
        elem = dom.createElement("section")
        elem.setAttribute("id", "phys%d" % id_)
        elem.setAttribute("address", "%#x" % _0(sect.address))
        elem.setAttribute("size", "%#x" % sect.get_size())
        section_list.appendChild(elem)

    segment_list = dom.createElement("segment_list")
    resources.appendChild(segment_list)

    for seg in segments:
        section_ids = ['phys%d' % section_hash[x] for x in seg.sections if x.get_size() != 0]

        elem = dom.createElement("segment")
        elem.setAttribute("sections", ' '.join(section_ids))
        elem.setAttribute("phys", "%#x" % seg.paddr)
        elem.setAttribute("virt", "%#x" % seg.vaddr)
        elem.setAttribute("filesz", "%#x" % seg.get_filesz())
        elem.setAttribute("memsz", "%#x" % seg.get_memsz())
        segment_list.appendChild(elem)

    sl = dom.createElement("sas_list")
    resources.appendChild(sl)
    sas = dom.createElement("sas")
    sas.setAttribute("id", "sas1")
    sl.appendChild(sas)

    total_caps = 0
    for clist in cap_lists:
        total_caps += clist[0]

    start = 0
    count = 0
    ids = dom.createElement("ids")
    resources.appendChild(ids)
    id_pool = dom.createElement("id_pool")
    id_pool.setAttribute("name", "all")
    id_pool.setAttribute("source", "tbd")
    id_pool.setAttribute("total", "%d" % total_caps)
    ids.appendChild(id_pool)

    for clist in cap_lists:
        id_range = dom.createElement("range")
        id_range.setAttribute("id", "all%d" % count)
        id_range.setAttribute("start", "%d" % start)
        id_range.setAttribute("size", "%d" % clist[0])
        id_pool.appendChild(id_range)
        start += clist[0]
        count += 1

    pools = dom.createElement("pools")
    resources.appendChild(pools)

    virtmem = dom.createElement("virt_mem")
    virtmem.setAttribute("id", "kspc")
    virtmem.setAttribute("sas_id", "sas1")
    pools.appendChild(virtmem)

    for sect, id_ in kernel_sections:
        elem = dom.createElement("vm_section")
        elem.setAttribute("id", "kspc%d" % id_)
        elem.setAttribute("address", "%#x" % sect.address)
        elem.setAttribute("size", "%#x" % sect.get_size())

        virtmem.appendChild(elem)

    for pool_id, sect_info in cell_sect_ids.values():
        virtmem = dom.createElement("virt_mem")
        virtmem.setAttribute("id", "vmpool_%d" % pool_id)
        virtmem.setAttribute("sas_id", "sas1")
        pools.appendChild(virtmem)

        for sect, id_ in sect_info:
            elem = dom.createElement("vm_section")
            elem.setAttribute("id", "vmpool_%d%d" % (pool_id, id_))
            elem.setAttribute("address", "%#x" % _0(sect.address))
            elem.setAttribute("size", "%#x" % sect.get_size())

            virtmem.appendChild(elem)

    for pool in mem_pools:
        mp = dom.createElement("mem_pool")
        mp.setAttribute("id", pool[0])
        mp.setAttribute("node", "phys%d" % pool[1])
        pools.appendChild(mp)
        group = dom.createElement("group")
        group.setAttribute("name", "all")
        group.setAttribute("type", "normal")
        group.setAttribute("size", "0x%x" % pool[2])
        mp.appendChild(group)

def write_out_memstats(elf, xml_str):
    """
    Write the XML DOM to a note in the ELF image.
    """
    memstat_sec = elf.find_section_named("memstats")
    if not (len(xml_str) <= memstat_sec.get_size()):
        print "too big %d won't fit in %d" % (len(xml_str), memstat_sec.get_size())
        assert(False)
    memstat_sec._desc_data[:len(xml_str)] = ByteArray(xml_str)
