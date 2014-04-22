import stat
import csv 
import itertools
import struct
import os
import shutil
import hashlib
import string
import re
import sys
import getopt
from optparse import OptionParser

from os.path import join as pj
from os.path import split as ps
from os.path import exists as pe
from os.path import abspath as ap


#import elfFileClass

PAD_BYTE                  = 0               # Padding byte 0s
# ELF Definitions
ELF_HDR_SIZE              = 52          
ELF_PHDR_SIZE             = 32          
ELFINFO_MAG0_INDEX        = 0
ELFINFO_MAG1_INDEX        = 1
ELFINFO_MAG2_INDEX        = 2
ELFINFO_MAG3_INDEX        = 3
ELFINFO_MAG0              = '\x7f'
ELFINFO_MAG1              = 'E'
ELFINFO_MAG2              = 'L'
ELFINFO_MAG3              = 'F'
ELFINFO_CLASS_INDEX       = 4
ELFINFO_CLASS_32          = '\x01'
ELFINFO_VERSION_INDEX     = 6
ELFINFO_VERSION_CURRENT   = '\x01'
ELF_BLOCK_ALIGN           = 0x1000

# ELF Program Header Types
NULL_TYPE                 = 0x0
LOAD_TYPE                 = 0x1
DYNAMIC_TYPE              = 0x2
INTERP_TYPE               = 0x3
NOTE_TYPE                 = 0x4
SHLIB_TYPE                = 0x5
PHDR_TYPE                 = 0x6
TLS_TYPE                  = 0x7

PROGRAM_HEADER_LEN        = 8

print "extpl_img_builder"

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   #return env.Detect('extpl_paging_elf_builder')
   return env.Detect('extpl_paging_bin_builder')

def generate(env):
   extpl_img_generate(env)
   
def extpl_img_generate(env):
   extpl_elf_act = env.GetBuilderAction(extpl_paging_elf_builder)
   extpl_elf_bld = env.Builder(action = extpl_elf_act, suffix = '.elf')

   extpl_bin_act = env.GetBuilderAction(extpl_paging_bin_builder)
   extpl_bin_bld = env.Builder(action = extpl_bin_act, suffix = '.bin')

   env.Append(BUILDERS = {'ExtplPagingElfBuilder' : extpl_elf_bld})
   env.Append(BUILDERS = {'ExtplPagingBinBuilder' : extpl_bin_bld})
   
def extpl_paging_elf_builder(target, source, env):
   source_str = os.path.abspath(str(source[0]))
   target_str = os.path.abspath(str(target[0]))   
   build_extpl_paging_elf(target_str, source_str)

   return None

def extpl_paging_bin_builder(target, source, env):
   source_str = os.path.abspath(str(source[0]))
   target_str = os.path.abspath(str(target[0]))   
   build_extpl_paging_bin(target_str, source_str)

   return None

#----------------------------------------------------------------------------
# Header Class Notes:
# In order to properly read and write the header structures as binary data,
# the Python Struct library is used to align and package up the header objects
# All Struct objects are initialized by a special string with the following
# notation. These structure objects are then used to decode binary data in order
# to fill out the appropriate class in Python, or they are used to package up
# the Python class so that we may write the binary data out. 
#----------------------------------------------------------------------------
"""
      Format | C Type         | Python Type | Standard Size
      -----------------------------------------------------
    1) 'X's  | char *         | string      | 'X' bytes
    2) H     | unsigned short | integer     | 2 bytes
    3) I     | unsigned int   | integer     | 4 bytes
     
"""

#----------------------------------------------------------------------------
# ELF Header Class 
#----------------------------------------------------------------------------
class Elf32_Ehdr:
   # Structure object to align and package the ELF Header
   s = struct.Struct('16sHHIIIIIHHHHHH') 
   
   def __init__(self,  entry_address=0, data=None):
      if data != None:
         unpacked_data       = (Elf32_Ehdr.s).unpack(data)
         self.unpacked_data  = unpacked_data
         self.e_ident        = unpacked_data[0]
         self.e_type         = unpacked_data[1]
         self.e_machine      = unpacked_data[2]
         self.e_version      = unpacked_data[3]
         self.e_entry        = unpacked_data[4]
         self.e_phoff        = unpacked_data[5]
         self.e_shoff        = unpacked_data[6]
         self.e_flags        = unpacked_data[7]
         self.e_ehsize       = unpacked_data[8]
         self.e_phentsize    = unpacked_data[9]
         self.e_phnum        = unpacked_data[10]
         self.e_shentsize    = unpacked_data[11]
         self.e_shnum        = unpacked_data[12]
         self.e_shstrndx     = unpacked_data[13]
      else:
         self.e_ident = ELFINFO_MAG0
         self.e_type = 0x2
         self.e_machine = 0xA4
         self.e_version = 0x1
         self.e_entry = entry_address
         self.e_phoff = 52
         self.e_shoff = 0
         self.e_flags = 3
         self.e_ehsize = 52
         self.e_phentsize = 32
         self.e_phnum = 1
         self.e_shentsize = 0
         self.e_shnum = 0
         self.e_shstrndx = 0

   def printValues(self):
      print "ATTRIBUTE / VALUE"
      for attr, value in self.__dict__.iteritems():
         print attr, value

   def getPackedData(self):
      values = [self.e_ident,
                self.e_type,
                self.e_machine,
                self.e_version,
                self.e_entry,
                self.e_phoff,
                self.e_shoff,
                self.e_flags,
                self.e_ehsize,
                self.e_phentsize,
                self.e_phnum,
                self.e_shentsize,
                self.e_shnum,
                self.e_shstrndx
               ]
      
      return (Elf32_Ehdr.s).pack(*values)

   def writePackedData(self, target):
      values = [self.e_ident,
                self.e_type,
                self.e_machine,
                self.e_version,
                self.e_entry,
                self.e_phoff,
                self.e_shoff,
                self.e_flags,
                self.e_ehsize,
                self.e_phentsize,
                self.e_phnum,
                self.e_shentsize,
                self.e_shnum,
                self.e_shstrndx
               ]

      s = struct.Struct('16sHHIIIIIHHHHHH') 
      packed_data = s.pack(*values)
      
      fp = OPEN(target,'wb')
      fp.write(packed_data)
      fp.close()

      return s.size

#----------------------------------------------------------------------------
# ELF Program Header Class 
#----------------------------------------------------------------------------
class Elf32_Phdr:

   # Structure object to align and package the ELF Program Header 
   s = struct.Struct('I' * 8) 
   
   def __init__(self, entry_address=0, load_size=0, data=None):
      if data != None:
         unpacked_data       = (Elf32_Phdr.s).unpack(data)
         self.unpacked_data  = unpacked_data
         self.p_type         = unpacked_data[0]
         self.p_offset       = unpacked_data[1]
         self.p_vaddr        = unpacked_data[2]
         self.p_paddr        = unpacked_data[3]
         self.p_filesz       = unpacked_data[4]
         self.p_memsz        = unpacked_data[5]
         self.p_flags        = unpacked_data[6]
         self.p_align        = unpacked_data[7]
      else:  
         self.p_type         = 1
         self.p_offset       = 0x001000
         self.p_vaddr        = entry_address
         self.p_paddr        = entry_address
         self.p_filesz       = load_size
         self.p_memsz        = load_size
         self.p_flags        = 7
         self.p_align        = 0x001000

   def printValues(self):
      print "ATTRIBUTE / VALUE"
      for attr, value in self.__dict__.iteritems():
         print attr, value
            
   def getPackedData(self):
      values = [self.p_type,
                self.p_offset,
                self.p_vaddr,
                self.p_paddr,
                self.p_filesz,
                self.p_memsz,
                self.p_flags,
                self.p_align
               ]
      
      return (Elf32_Phdr.s).pack(*values)
   def getLength(self):
      return PROGRAM_HEADER_LEN
      
   def writePackedData(self, target):
      values = [self.p_type,
                self.p_offset,
                self.p_vaddr,
                self.p_paddr,
                self.p_filesz,
                self.p_memsz,
                self.p_flags,
                self.p_align
               ]
      s = struct.Struct('I' * self.getLength()) 
      packed_data = s.pack(*values)
      
      fp = OPEN(target,'ab')
      fp.write(packed_data)
      fp.close()
      return s.size

 
#----------------------------------------------------------------------------
# Verify ELF header contents from an input ELF file
#----------------------------------------------------------------------------
def verify_elf_header(elf_header):
   if (elf_header.e_ident[ELFINFO_MAG0_INDEX] != ELFINFO_MAG0) or \
      (elf_header.e_ident[ELFINFO_MAG1_INDEX] != ELFINFO_MAG1) or \
      (elf_header.e_ident[ELFINFO_MAG2_INDEX] != ELFINFO_MAG2) or \
      (elf_header.e_ident[ELFINFO_MAG3_INDEX] != ELFINFO_MAG3) or \
      (elf_header.e_ident[ELFINFO_CLASS_INDEX] != ELFINFO_CLASS_32) or \
      (elf_header.e_ident[ELFINFO_VERSION_INDEX] != ELFINFO_VERSION_CURRENT):

      return False
   else:
      return True

#----------------------------------------------------------------------------
# Preprocess an ELF file and return the ELF Header Object and an 
# array of ELF Program Header Objects
#----------------------------------------------------------------------------
def read_elf_headers(elf_file_name):

   # Initialize
   elf_fp = OPEN(elf_file_name, 'rb')
   elf_header = Elf32_Ehdr(data=elf_fp.read(ELF_HDR_SIZE))
   phdr_table = []

   # Verify ELF header information
   if verify_elf_header(elf_header) is False:
      raise RuntimeError, "ELF file failed verification: " + elf_file_name

   # Get program header size
   phdr_size = elf_header.e_phentsize

   # Find the program header offset 
   file_offset = elf_header.e_phoff
   elf_fp.seek(file_offset) 
   
   # Read in the program headers
   for i in range(elf_header.e_phnum):
      phdr_table.append(Elf32_Phdr(elf_fp.read(phdr_size))) 

   elf_fp.close()
   return [elf_header, phdr_table]

#----------------------------------------------------------------------------
# Pad a file with specific number of bytes
# Note: Assumes the fp is seeked to the correct location of padding 
#----------------------------------------------------------------------------
def pad_file(fp, num_bytes, value):
    
   if num_bytes < 0:
      raise RuntimeError, "Number of bytes to pad must be greater than zero"
  
   while num_bytes > 0:
      fp.write('%c' % value)
      num_bytes -= 1

   return 

def print_elf_hdr(elf):
   # ELF Header
   print "ELF Header:"
   print " Magic:".ljust(36) + ''.join([
       str(elf.elfHeader.e_ident[CONST.elfIdentification.EI_MAG0]),
       " ",
       str(elf.elfHeader.e_ident[CONST.elfIdentification.EI_MAG1]),
       " ",
       str(elf.elfHeader.e_ident[CONST.elfIdentification.EI_MAG2]),
       " ",
       str(elf.elfHeader.e_ident[CONST.elfIdentification.EI_MAG3]),
       " ",
       str(elf.elfHeader.e_ident[CONST.elfIdentification.EI_CLASS]),
       " ",
       str(elf.elfHeader.e_ident[CONST.elfIdentification.EI_DATA]),
       " ",
       str(elf.elfHeader.e_ident[CONST.elfIdentification.EI_VERSION]),
       " 0 0 0 0 0 0 0 0 0"
        ])

   print " Class:".ljust(36) + str(elf.elfHeader.e_ident[CONST.elfIdentification.EI_CLASS])
   print " Data:".ljust(36) + str(elf.elfHeader.e_ident[CONST.elfIdentification.EI_DATA])
   print " Version:".ljust(36) + str(elf.elfHeader.e_ident[CONST.elfIdentification.EI_VERSION])
   print " OS/ABI:".ljust(36) + str(elf.elfHeader.e_ident[CONST.elfIdentification.EI_OSABI])
   print " ABI Version:".ljust(36) + str(elf.elfHeader.e_ident[CONST.elfIdentification.EI_ABIVERSION])
   print " Type:".ljust(36) + str(elf.elfHeader.e_type)
   print " Machine:".ljust(36) + str(elf.elfHeader.e_machine)
   print " Version:".ljust(36) + str(hex(elf.elfHeader.e_version))
   print " Entry point address:".ljust(36) + str(hex(elf.elfHeader.e_entry))
   print " Start of program headers:".ljust(36) + str(elf.elfHeader.e_phoff)
   print " Start of section headers:".ljust(36) + str(elf.elfHeader.e_shoff)
   print " Flags:".ljust(36) + str(hex(elf.elfHeader.e_flags))
   print " Size of this header:".ljust(36) + str(elf.elfHeader.e_ehsize)
   print " Size of program headers:".ljust(36) + str(elf.elfHeader.e_phentsize)
   print " Number of program headers:".ljust(36) + str(elf.elfHeader.e_phnum)
   print " Size of section headers:".ljust(36) + str(elf.elfHeader.e_shentsize)
   print " Number of section headers:".ljust(36) + str(elf.elfHeader.e_shnum)
   print " Section header string table index: " + str(elf.elfHeader.e_shstrndx)
   print

def print_phdr(name, phdr):
   print "Program Header: " + name 
   print " p_type:".ljust(36) + str(phdr.p_type)
   print " p_offset:".ljust(36) + str(phdr.p_offset)
   print " p_vaddr:".ljust(36) + str(hex(phdr.p_vaddr))
   print " p_paddr:".ljust(36) + str(hex(phdr.p_paddr))
   print " p_filesz:".ljust(36) + str(hex(phdr.p_filesz))
   print " p_memsz:".ljust(36) + str(hex(phdr.p_memsz))
   print " p_flags:".ljust(36) + str(phdr.p_flags)
   print " p_align:".ljust(36) + str(phdr.p_align)
   print

#----------------------------------------------------------------------------
# Helper functions to open a file and return a valid file object 
#----------------------------------------------------------------------------
def OPEN(file_name, mode):
    try:
       fp = open(file_name, mode)
    except IOError:
       raise RuntimeError, "The file could not be opened: " + file_name
    
    # File open has succeeded with the given mode, return the file object
    return fp

def import_elf_scripts():     
  anchor_str = 'modem_proc'
  path = ap(".")
  split_dir = path.split(os.sep)
  
  anchor_dir = None
  for search_dir in (split_dir,):
    if anchor_str in search_dir:
      anchor_dir = os.sep.join(search_dir[0:search_dir.index(anchor_str) + 1])
      break

  if not anchor_dir:
    raise Exception('The specified path does not contain modem_proc')

  print "anchor_dir = ", anchor_dir
  sys.path.insert( 0, ap(pj(anchor_dir, 'perf', 'scripts', 'elfManipulator',)))

  try:
    from include import elfFileClass
    from include import elfConstants
    globals().update(ELF=elfFileClass,
                     CONST=elfConstants)

  except ImportError:
    sys.stderr.write(' '.join([
      'Could not import ELF manipulator. Please place this script somewhere',
      'along <build_root>/modem_proc/build/ms.\n']))
    raise

extpl_sections = [ 
    (
      ".candidate_extpl_section_nb1", 
      "__swapped_segments_extpl_nb1_rwdata_end__", 
      "__swapped_segments_extpl_nb1_text_start__"
    ),

    (
      ".candidate_extpl_section_m1",
      "__swapped_segments_extpl_m1_rwdata_end__", 
      "__swapped_segments_extpl_m1_rodata_start__"
    ),
    (
      ".candidate_extpl_section_geran", 
      "__swapped_segments_extpl_geran_rwdata_end__", 
      "__swapped_segments_extpl_geran_text_start__"
    ),

    (
      ".candidate_extpl_section_gps", 
      "__swapped_segments_extpl_gps_rwdata_end__", 
      "__swapped_segments_extpl_gps_text_start__"
    ),
    
    (
      ".candidate_extpl_section_ondemand", 
      "__swapped_segments_extpl_ondemand_rodata_end__", 
      "__swapped_segments_extpl_ondemand_text_start__"
    ),
]

def get_extpl_segements(elf):
   shdr = []
   for section in extpl_sections:
      offset = 0
      sh=elf.getSectionByName(section[0]);
      if sh != CONST.RC_ERROR:
        ph=elf.getSegmentByAddress(sh.sh_addr)
        offset = elf.getSymbolByName(section[2]).st_value - sh.sh_addr
        size=elf.getSymbolByName(section[1]).st_value - elf.getSymbolByName(section[2]).st_value
        sh.sh_size=size
        shdr.append((sh, offset))
      else:
        print "paging section %36s was not found" %(section[0])
        
   return shdr

def build_extpl_paging_bin(target_str, source_str):
   import_elf_scripts()
   elf = ELF.elfFile(source_str)
   #print_elf_hdr(elf)

   fh = OPEN(target_str, "wb")

   offset = 0;
   shdr_tbl = get_extpl_segements(elf)
   for sh_info in shdr_tbl:  
      sh = sh_info[0]
      start = sh_info[1]
      # write the content of each segment
      print "section %36s paging offset %d size %d" % (sh.sh_nameStr, offset, sh.sh_size)
      fh.write(sh.contents[start:start+sh.sh_size])
      offset = offset + sh.sh_size

   fh.close()

   return None

#------------------------------------------------------------------------------
# Parse command line arguments
#------------------------------------------------------------------------------
def parse_args():
   usage = "usage: %prog [options]"
   version = "%prog 1.0"
   
   parser = OptionParser(usage=usage, version=version)
   parser.add_option("-f", "--infile", dest="input_filename",
                  help="Read preprocess data from FILE", metavar="FILE")
   parser.add_option("-o", "--outfile", dest="output_filename",
                  help="Write output to FILE", metavar="FILE")
   parser.add_option("-i", "--image_entry", dest="image_entry",
                  help="Image entry address", metavar="FILE")       
   (options, args) = parser.parse_args()
   
   if options.input_filename is None:
      parser.error("Usage: -f input_file -o output_file -i image_entry")
      sys.exit(2)
   if options.output_filename is None:
      parser.error("Usage: -f input_file -o output_file -i image_entry")
      sys.exit(2)               
   if options.image_entry is None:
      options.image_entry=0

   return (options, args)

#------------------------------------------------------------------------------
# main
#------------------------------------------------------------------------------
if __name__ == "__main__":  
   # get args from cmd line
   (options, args) = parse_args()
   
   # init and fix variebles
   source_str = options.input_filename
   target_str = options.output_filename
   build_extpl_paging_elf(target_str, source_str)
