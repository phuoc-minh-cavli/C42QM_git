############################################################################
# DESCRIPTION
# This file contains routines to calculate the last physical address and the
# size required to place the hash segment
# 
# Copyright (c) 2008 by Qualcomm Technologies Incorporated.  All Rights Reserved.
#
############################################################################

import elf.core
import sys
import weaver.machine
from elf.constants import ET_REL, PT_NULL, PT_LOAD, SHT_NOBITS, SHF_WRITE, SHF_ALLOC, PF_R, PF_W
from elf.section import UnpreparedElfSection
from elf.segment import SectionedElfSegment
from elf.core import UnpreparedElfFile, PreparedElfFile

#import weaver.bootinfo_elf
#from weaver.ezxml import EzXMLError, size_attr

def print_physinfo(image, machine, options, fout=sys.stdout):
    """Calculate the hash size and the last phys location where hash 
       segment should be placed."""
    hash_segment_size   = 0
    hash_segment_offset = 0
    last_paddr          = 0    
    last_seg_phdr       = None
    end_addr            = 0

    min_page_size      =  machine.min_page_size()
    hash_size          = machine.hash_size
    hash_overhead_size = machine.hash_overhead_size

    for segment in image.elf.segments:
        program_header = segment.get_program_header(image.elf.endianess, image.elf.wordsize)
        """ For paged segment hash_segment_size += segment_size/page_size * hash_size """
        #if program_header.p_flags & weaver.bootinfo_elf.MI_PBT_PAGED_SEGMENT:
        #    """ print >> fout,  "PAGED SEGMENT" """
        #    hash_segment_size += (program_header.p_filesz/min_page_size) * hash_size
        #else:
        #    """ For non paged segment hash_segment_size = hash_size """
        #    hash_segment_size += hash_size
        #    if last_paddr < program_header.p_paddr:
        #        last_paddr = program_header.p_paddr
        #        last_seg_phdr = program_header
        hash_segment_size += hash_size
        if last_paddr < program_header.p_paddr:
           last_paddr = program_header.p_paddr
           last_seg_phdr = program_header
        """ print >> fout, "hash segment size = %d" % hash_segment_size """
        """ print >> fout, "Last Physical address = %x" % last_paddr """
    
    hash_segment_size += hash_overhead_size
    hash_segment_size = (hash_segment_size & (~(min_page_size-1)))  + min_page_size
    hash_segment_offset = ((last_seg_phdr.p_offset + last_seg_phdr.p_filesz) & (~(last_seg_phdr.p_align-1))) + last_seg_phdr.p_align
    last_paddr = ((last_seg_phdr.p_paddr + last_seg_phdr.p_memsz - 1) & (~(min_page_size-1))) + min_page_size

    print >> fout, "hash_segment_size=%d" % hash_segment_size
    print >> fout, "hash_segment_offset=0x%x" % hash_segment_offset
    print >> fout, "last_phys_address=0x%x" % last_paddr

    if image.elf.elf_type == ET_REL:
       """Append a dummy segment at the end of the image so that it fills up the entire image. This is 
          Used by the PIL driver on the apps side to reserve physical memory before loading the physially
          relocatable modem image."""
       regions = machine.get_physical_memory('physical')
       for (base, size, mem_type, _) in regions:
          if (base + size) >= end_addr:
              end_addr = base + size

       if end_addr < last_paddr:
          raise UserError, "End address %s of default physpool is less than hash segment start address %s" % (end_addr, last_paddr)

       """ Unprepare the elf file to add the dummy segment to the elf """
       elf = UnpreparedElfFile(filename=options.output_file)

       dummy_section = UnpreparedElfSection(elf,
                                            name='Dummy.Section', 
                                            section_type=SHT_NOBITS,
                                            address =last_paddr + hash_segment_size,
                                            data = None,
                                           flags = SHF_WRITE|SHF_ALLOC)

       dummy_section.increment_size(end_addr - last_paddr - hash_segment_size)
    
       elf.add_section(dummy_section)
        
       dummy_segment = SectionedElfSegment(elf, 
                                           PT_LOAD,
                                           last_paddr + hash_segment_size,
                                           last_paddr + hash_segment_size,
                                           PF_R | PF_W,
                                           machine.min_page_size(),
                                           sections=[dummy_section])

       """ Add the newly created segment to the elf """
       elf.add_segment(dummy_segment)

       """ Prepare the new elf and write it out """
       elf = elf.prepare(elf.wordsize, elf.endianess)
       
       elf.to_filename(options.output_file)

