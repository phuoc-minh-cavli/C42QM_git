'''
=============================================================================

                  Q S H R I N K E L F . P Y

Copyright (c) 2015-2020 by QUALCOMM Technologies Inc.  All Rights Reserved.

=============================================================================
DESCRIPTION
    Elf modification script for debug string optimization.
=============================================================================

=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order
$Header: //components/rel/build.tn/4.0/myps/qshrink/QShrinkElf.py#3 $
$Date: 2020/09/09 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/09/20   rm      Added QShrink 4.0 support
=============================================================================
'''

import hashlib
import uuid
import zlib
import re
import time
import string
import struct
import csv
import os, errno
import shutil
import binascii
import os
import sys
import time
import inspect
import optparse
import elfConstants as const
import elfFileClass as elfFileClass
import elfUtils as utils

__qsrdebug__= 1

if __qsrdebug__:
  from QElfAnomalies import analyzeAnomalies
  #seems to happen whether __qsrdebug__ or not

# ======================== start globals ========================

const = {
  # Constants as defined in the ELF specification
  # Nowhere near complete but sufficient for this program.

  # ELF Sizes
  'Elf32_Addr'    : 4,
  'Elf32_Half'    : 2,
  'Elf32_Off'     : 4,
  'Elf32_Sword'   : 4,
  'Elf32_Word'    : 4,
  # ELF Type
  'ET_NONE'       : 0,
  'ET_REL'        : 1,
  'ET_EXEC'       : 2,
  'ET_DYN'        : 3,
  'ET_CORE'       : 4,
  'ET_LOPROC'     : 0xff00, # lower processor-specific range
  'ET_HIPROC'     : 0xffff, # higher processor-specific range
  # ELF Machine
  'EM_NONE'       : 0, # Error?
  'EM_M32'        : 1,
  'EM_SPARC'      : 2,
  'EM_386'        : 3,
  'EM_68K'        : 4,
  'EM_88K'        : 5,
  'EM_860'        : 7,
  'EM_MIPS'       : 8,
  'EM_MIPS_RS4_BE': 10,
  'EM_ARM'        : 40, # Only ones that this script handles
  # ELF Version
  'EV_NONE'       : 0, # Error
  'EV_CURRENT'    : 1, # Good
  # ELF Identifier Index
  'EI_MAG0'       : 0,
  'EI_MAG1'       : 1,
  'EI_MAG2'       : 2,
  'EI_MAG3'       : 3,
  'EI_CLASS'      : 4, # File Class (32/64-bit)
  'EI_DATA'       : 5, # Data Encoding (Little/Big-endian)
  'EI_VERSION'    : 6, # Error/Current
  'EI_PAD'        : 7, # Start of padding.  Should be 0 for 8 bytes.
  'EI_NIDENT'     : 16, # Size of ELF Identifier
  # Magic Numbers.  Identify ELF files
  'ELFMAG0'       : 0x7f,
  'ELFMAG1'       : ord("E"),
  'ELFMAG2'       : ord("L"),
  'ELFMAG3'       : ord("F"),
  # 32-bit or 64-bit
  'ELFCLASSNONE'  : 0, # Error
  'ELFCLASS32'    : 1, # 32-bit
  'ELFCLASS64'    : 2, # 64-bit
  # Endian-ness
  'ELFDATANONE'   : 0, # Error
  'ELFDATA2LSB'   : 1, # Little-endian
  'ELFDATA2MSB'   : 2, # Big-endian
  # Elf Header Offset
  'ELFH_PHOFF_OFFSET'     : 28, # e_phOff
  'ELFH_SHOFF_OFFSET'     : 32, # e_shOff
  'ELFH_PHNUM_OFFSET'     : 44, # e_phNum
  'ELFH_SHNUM_OFFSET'     : 48, # e_phNum
  'ELFH_SHSTRINDEX_OFFSET'     : 50, # e_phNum
  # Section Header Offset
  'SECTIONH_OFFSET'     : 16, #sh_off
  'SECTIONH_SIZE'       : 20, #sh_size
  'SECTIONH_LINK'       : 24, #sh_link
  # Section Header Entry
  'SHN_UNDEF'     : 0, # should ignore
  'SHN_LORESERVE' : 0xff00, # lower reserved range
  'SHN_HIRESERVE' : 0xffff, # higher reserved range
  'SHN_LOPROC'    : 0xff00, # lower processor specific range
  'SHN_HIPROC'    : 0xff1f, # higher processor specific range
  'SHN_ABS'       : 0xfff1, # if symbol is relative to, use fixed address
  'SHN_COMMON'    : 0xfff2, # if symbol is relative to,treat as common symbol
  # Section Header Type
  'SHT_NULL'      : 0, # no section data
  'SHT_PROGBITS'  : 1, # holds data defined by program
  'SHT_SYMTAB'    : 2, # holds symbol table (same as SHT_DYNSYM ?)
  'SHT_STRTAB'    : 3, # holds string table
  'SHT_RELA'      : 4, # holds relocation entries with explicit addends
  'SHT_HASH'      : 5, # holds symbol hash table
  'SHT_DYNAMIC'   : 6, # holds dynamic linking information
  'SHT_NOTE'      : 7, # holds information on the file (labels?)
  'SHT_NOBITS'    : 8, # does not go into file. sh_offset not real
  'SHT_REL'       : 9, # holds relocation entries without explicit addends
  'SHT_SHLIB'     : 10, # reserved
  'SHT_DYNSYM'    : 11, # holds a symbol table (same as SHT_SYMTAB ?)
  'SHT_LOPROC'    : 0x70000000, # lower processor specific semantics range
  'SHT_HIPROC'    : 0x7fffffff, # higher processor specific semantics range
  'SHT_LOUSER'    : 0x80000000, # lower application reserve range
  'SHT_HIUSER'    : 0xffffffff, # higher application reserve range
  # Section Header Flags
  'SHF_WRITE'     : 0x1, # section is writable during execution
  'SHF_ALLOC'     : 0x2, # occupies memory during process execution
  'SHF_EXECINSTR' : 0x4, # executable machine instructions
  'SHF_MASKPROC'  : 0xf0000000 # processor-specific flags (just a mask)
}

header = {
# global ELF header stat.  Set in initializeWithElfHeader 
  'endian'            : 0, # 1 = little endian  :: 2 = big endian
  'headerSize'        : 0, # size of the ELF Header. (always 52?)
  'programHeaderOff'  : 0, # start of program header table
  'programHeaderSize' : 0, # size of each program header entry
  'programHeaderNum'  : 0, # number of program header entries in table
  'sectionHeaderOff'  : 0, # start of section header table
  'sectionHeaderSize' : 0, # size of each section header entry
  'sectionHeaderNum'  : 0, # number of section header entries in table
  'entry'             : 0, # entry address into executable image
  'flags'             : 0,
  'sectionNameTable'  : 0  # Section Header String Table Index
}

class ElfShrinkStateClass:
  '''
  Used to hold most variables for ElfShrinkState.
  '''


REPLACEMENT_MODE = 1
#discovery or replacement mode?

# @TODO Eventually need to make these below dynamically grabbed, somehow.

MSG_V2_STRUCT_SIZE = 0
# size of the xx_msg_v2_const struct
MSG_V3_STRUCT_SIZE = 0
# size of the xx_msg_v3_const struct

MSG_V2_ERR_STRUCT_SIZE = 0
# size of the xx_err_msg_v2_const struct
MSG_V3_ERR_STRUCT_SIZE = 0
# size of the xx_err_msg_v3_const struct

QSR_POINTER_OFFSET = 8

MSG_V3_QSR_POINTER_OFFSET = 4
# offset of the struct's const char* const* pointer
MSG_V3_SS_MASK_OFFSET = 0
# offset of the struct's ss_mask pointer

MSG_V3_SS_INDEX_OFFSET = 0
# offset of the struct's ss_index pointer
MSG_V3_LINE_OFFSET = 2
# offset of the struct's line offset

QSR_STRING_START = 0
# Virtual address of the start of the QSR string section

QSR_STRING_SIZE = 0
# Size of the QSR string section

QSR_STRING_END = 0
# Virtual address of the end of the QSR string section

QSR_STRING_OFFSET = 0
# Elf Offset of QSR_STRING section

QSR_DESC_ENTRY_SIZE = 8
# Size of Description table entry

QSR_UAM_TABLE_ENTRY_SIZE = 2
# Size of MSG V4 UAM table entry


QSR_STRING_START_OUTPUT = 0
# Virtual address of the start of the QSR string section in the output ELF
# This can be different from QSR_STRING_START if QSR_STRING section is not contiguous to the previous section

QSR_EMPTY_SIZE = 32
# If all QSR strings are shrinked, size of QSR (empty) section


#QShrink 4.0 globals

QSR_MSG_START = 0   # Start Virtual address of QSR_4_0_MSG section
QSR_MSG_SIZE  = 0   # Size of QSR_4_0_MSG section
QSR_MSG_END   = 0   # End Virtual address of QSR_4_0_MSG section
QSR_MSG_OFFSET = 0  # Elf Offset of QSR_4_0_MSG section

# QSR_4_0_MSG_STR
QSR_MSG_STR_START  = 0  # Start Virtual address of QSR_4_0_MSG_STR section
QSR_MSG_STR_SIZE   = 0  # Size of QSR_4_0_MSG_STR section
QSR_MSG_STR_END    = 0  # End Virtual address of QSR_4_0_MSG_STR section
QSR_MSG_STR_OFFSET = 0  # Elf Offset of QSR_4_0_MSG_STR section


# ======================== end globals ========================

#=========================================================================
# Function : checkEIDENT 
#
# Arguments
#  header: (str) Bytes of ELF header
#  e_ident: (str) Bytes of the e_ident field
#
# Description
#  Checks the e_ident field for global state that needs to be set.  May exit
#  if flags show an unsupported type.
#
# Return Values:
#  None
#=========================================================================
def checkEIDENT(header, e_ident):
  # formatting print to separate runQshrink info 
  if __qsrdebug__:
    print ""
  checkEidentELF(e_ident)
  checkEidentClass(e_ident)
  getEidentData(header, e_ident)
  checkEidentVersion(e_ident)


#=========================================================================
# Function : checkEidentELF 
#
# Arguments
#  e_ident: (str) Bytes of the e_ident field
#
# Description
#  Checks to see if the ELF file is actually an ELF file.  Will exit if we are
#  not working with an ELF file.
#
# Return Values:
#  None
#=========================================================================
def checkEidentELF(e_ident):
  if e_ident[0:4] != "".join(['\x7f', "ELF"]) :
    print "NOT AN ELF FILE!"
    exit(1)

#=========================================================================
# Function : checkEidentClass 
#
# Arguments
#  e_ident: (str) Bytes of the e_ident field
#
# Description
#  Checks if ELF is 32-bit.  Exits if it is NONE, UNKNOWN, or 64-bit.
#
# Return Values:
#  None
#=========================================================================
def checkEidentClass(e_ident):
  num = ord(e_ident[const['EI_CLASS']])
  if num != const['ELFCLASS32'] :
    print "NEEDS TO BE ELF-32"
    if    num == const['ELFCLASSNONE'] : print "CURRENT: INVALID"
    elif  num == const['ELFCLASS64']   : print "CURRENT: ELF-64"
    else: print "UNKNOWN EI_CLASS:", str(num)
    exit()

#=========================================================================
# Function : getEidentData 
#
# Arguments
#  header: (str) Bytes of the ELF header
#  e_ident: (str) Bytes of the e_ident field.
#
# Description
#    Sets the global variable for endianness.  Modifies behavior of the
#   getWord(str) family of functions.
#
# Return Values:
#  None
#=========================================================================
def getEidentData(header, e_ident):
  num = ord(e_ident[const['EI_DATA']])
  if 1 <= num <= 2:
    if __qsrdebug__:
      if    num == 1 : print "LITTLE ENDIAN"
      elif  num == 2 : print "BIG ENDIAN"
    header['endian'] = num
    return
  print "INVALID EI_DATA:", str(num)
  exit()

#=========================================================================
# Function : checkEidentVersion 
#
# Arguments
#  e_ident: (str) Bytes of the e_ident field.
#
# Description
#   Checks the ELF file version.  Exits on anything other than a Current one.
#
# Return Values:
#  None
#=========================================================================
def checkEidentVersion(e_ident):
  num = ord(e_ident[const['EI_VERSION']])
  if num == 1:
    return
  elif num == 0:  print "EIDENT VERSION: INVALID"
  else :          print "EIDENT VERSION: UNKNOWN", str(num)
  exit()

#=========================================================================
# Function : getPrettyBinary 
#
# Arguments
#  string: (str) String of bytes
#
# Description
#   Only prints ASCII characters and a '.' for all non-ascii characters.  May 
#  become confusing if there are real '.'s in your strings.
#  Used only for debugging and when you don't want to use fromelf.
#
# Return Values:
#  None
#=========================================================================
def getPrettyBinary(string):
  return ''.join([(x if (32 <= ord(x) <= 126) else '.') for x in string])

#=========================================================================
# Function : getSectionFromVA 
#
# Arguments
#  virtual_Address: used to look up the appropriate section 
#
# Description
#  Looks up the section that a given VA resides in.
#
# Return Values:
#  a section header object
#=========================================================================
def getSectionFromVA(virtual_address):
  dbg_match = re.compile(
      r'(\.debug)|(\.symtab)|(\.strtab)|(\.note)|(\.comment)|(\.shstrtab)')
  for shd in ElfShrinkState.sectionHeaderDict.values():
    if not dbg_match.search(shd.name):
      if(virtual_address >= shd.data['sh_addr'] and
         virtual_address < shd.data['sh_addr'] + shd.data['sh_size']):
        return shd
  print "getSectionFromVA(", hex(virtual_address), "):", hex(virtual_address), "is not in a section" 
  return None

#=========================================================================
# Function : initializeArmElfShrinkState 
#
# Arguments
#  elfFile, outputFile, hashFile,
#  logFile, anomalyListFile
#
# Description
#  Initialize the global ElfShrinkState
#
# Return Values:
#  None
#=========================================================================
def initializeArmElfShrinkState(elfFile, outputFile, buildOutputPath, hashFile,qdbFile,
                                logFile, anomalyListFile):
  global ElfShrinkState
  ElfShrinkState = ElfShrinkStateClass()
  #used to hold variables shared between the processing methods

  ElfShrinkState.sectionNameHeader = None
  # Header of the section with the names of the other sections

  ElfShrinkState.sectionNameData = None
  # Data of the section with the names of the other sections.

  ElfShrinkState.path = elfFile
  # ELF file path

  ElfShrinkState.bytesDict = dict()
  #'name' -> bytes, for section caching

  ElfShrinkState.sectionHeaderDict = {}
  # index | name => section header

  ElfShrinkState.strAnomalyDict = {}
  # anomaly_VA_Addr => anomaly 

  ElfShrinkState.strMovedStrDict = {}
  # strPointer => moved string Address

  ElfShrinkState.stringHashDict = {}
  # msg string => big-endian hash

  ElfShrinkState.stringPtrDict = {}
  # msg string => moved pointers

  ElfShrinkState.hashStringDict = {}

  ElfShrinkState.anomalyStructCount = 0

  ElfShrinkState.path = elfFile
  ElfShrinkState.outputPath = outputFile
  ElfShrinkState.buildOutputPath = buildOutputPath
  ElfShrinkState.hashPath = hashFile
  ElfShrinkState.logPath = logFile
  
  ElfShrinkState.outputLog = open(ElfShrinkState.logPath, 'a')

  ElfShrinkState.qdbFile = qdbFile
  path,filename = os.path.split(qdbFile)  
  ElfShrinkState.qdbName = re.split('\.',filename)[0] 
    
  ElfShrinkState.matches = 0
  # summary counter for number of symbols which matched

  ElfShrinkState.image_vend_offset = 0
  ElfShrinkState.linker_endAddr = 0
  # Linker generated "end" address used by Kernel needs to be modified for 9K targets, if present

  ElfShrinkState.diagDescTblEntry = 0
  ElfShrinkState.diagDescTbl = 0

  ElfShrinkState.use_linker_end_symbol = 1
  # DIAG description table entry offsets,  msg_v2_ss_num_table_entries, msg_v2_ss_table

  ElfShrinkState.anomalyListPath = anomalyListFile

#=========================================================================
# Function : runQShrink 
#
# Arguments
#  elfFile: (str) ELF file to read in
#  outputFile: (str) ELF file to outputLog to
#  buildOutputPath: Output path for generated files
#  hashFile: (str) Master Hash file.  hash:filename:string format
#  qdhFile : 
#  logFile: (str) Holds the log of changes.  name:index:address:bytes
#  anomalyListFile: (str) Lists invalid qsr structs found.
#  replace_only_mode: 
#  1 - only replace strings already hashed
#  0 - discover new strings and replace the strings in Elf
#  2 - only discover new strings, do not update Elf file.
# Description
#    Main entry function .Runs the ELF editor.  Gets its parameters from
#   Qshrink.py which parses command-line options.
#
# Return Values:
#  None
#=========================================================================
def runQShrink(elfFile, outputFile,buildOutputPath,hashFile,qdbFile,
               logFile, anomalyListFile, replace_only_mode,
               use_linker_end_symbol):

  # keep track of total time
  start = time.clock()

  import datetime #can't be at top of file

  timeString = str(datetime.datetime.now())
  timeString = re.sub(r' ', r'_', timeString)
  timeString = re.sub(r'\..*', r'', timeString)
  timeString = re.sub(r':', r'.', timeString)

  #................................................
  # Important path related information - Start
  #................................................

  input_Path,inputFileName = os.path.split(elfFile)
  inputFileName = re.split('.elf',inputFileName)[0]
  msgHash_Path,msghashName = os.path.split(hashFile)

  if (logFile ==""):
    logFile = "qshrink_"+ inputFileName
  else:
    logFile_Path,logFilename = os.path.split(logFile)
    logFile = logFilename
    
  if (anomalyListFile ==""):
    anomalyListFile = "qshrink_an_"+ inputFileName
  
  if (buildOutputPath ==""):
    buildOutputPath=msgHash_Path
  
  if (buildOutputPath !=""):
    if not os.path.exists(buildOutputPath):
      try:
        os.makedirs(buildOutputPath)
      except OSError, e:         
         if e.errno != errno.EEXIST:
           print "Error in creating QShrink Output Directory, Check for parallel builds.."
           exit(1)
    
  logFile = logFile + ".log"
  logFile = os.path.join(buildOutputPath,logFile)
  reportFile = anomalyListFile + "_report" + ".txt"
  reportFile = os.path.join(buildOutputPath,reportFile)
  anomalyListFile = anomalyListFile + ".csv"
  anomalyListFile = os.path.join(buildOutputPath,anomalyListFile)
  
  #..............................................
  # Important path related information - End
  #..............................................

  datetime = 0

  resultStrs = [ \
    " ".join(["Starting Qshrink"]),
    " ".join(["ELF File:", elfFile]),
    " ".join(["Output File:", outputFile]),
    " ".join(["Build Output Path:", buildOutputPath]),
    " ".join(["QShrink 2,3 Hash File:", hashFile]),
    " ".join(["QDB File:", qdbFile]),
    " ".join(["Log File:", logFile])
  ]
  if replace_only_mode == 1:
    resultStrs.append("Replacement Mode")
  elif replace_only_mode == 0:
    resultStrs.append("Discover and Replace Mode")
  elif replace_only_mode == 2:
    resultStrs.append("Discover Only Mode")

  try:
    outputLog = open(logFile, 'w')
  except IOError:
    print "Error: Could not open Output log, ",logFile
    exit(1)
  else:
    for strn in resultStrs:
      print(strn)
      outputLog.write(strn)
      outputLog.write("\n")
    outputLog.close()

  if __qsrdebug__:
    print "Anomaly List File:", anomalyListFile
    print "Report File:", reportFile

  if (replace_only_mode != 2):
    # copy if output is different than input.
    # if input == output, then the original copy will be lost
    if elfFile != outputFile :
      copyStart = time.clock()
      if __qsrdebug__:
        print "Copy Start"
      shutil.copyfile(elfFile, outputFile)
      copyTime = time.clock() - copyStart
      if __qsrdebug__:
        print "Finished Copying (", copyTime, "seconds)"
      try:
        outputLog = open(logFile, 'a')
      except IOError:
        print "Error: Could not open Output log, ",logFile
        exit(1)
      else:
        outputLog.write(" ".join(["Finished Copying (", str(copyTime),
                                "seconds)\n"]))
        outputLog.close()

  #global state information
  global ElfShrinkState
  
  initializeArmElfShrinkState(elfFile, outputFile, buildOutputPath, hashFile,qdbFile,
                              logFile, anomalyListFile )

  ElfShrinkState.outputLog.write("QSHRINK4:initializeArmElfShrinkState\n")
  
  global REPLACEMENT_MODE
  REPLACEMENT_MODE = replace_only_mode

  ElfShrinkState.errCount = 0
  # summary counter for number of invalid symbol matches

  ElfShrinkState.matches = 0
  # summary counter for number of symbols which matched

  ElfShrinkState.newHashedStrings = 0
  
  ElfShrinkState.version = 0
  # QSHrink version

  #counters
  ElfShrinkState.hashified = 0
  ElfShrinkState.moved = 0
  ElfShrinkState.uniqueStructMatches = 0

  ElfShrinkState.use_linker_end_symbol = use_linker_end_symbol

  ElfShrinkState.visitedAnomalies = {}

  #start ElfShrink      
  #check QShrink version 4
  ElfShrinkState.qsr_4=0
  
  ElfShrinkState.outputLog.write("QSHRINK4: before initializeWithElfHeader\n")
  
  qsr_2_or_3 = initializeWithElfHeader()
  
  ElfShrinkState.outputLog.write("QSHRINK4: after initializeWithElfHeader\n")
  
  print "qsr_2_or_3 ",qsr_2_or_3
  print "Qshrink Version:- ",ElfShrinkState.version
  
  if (ElfShrinkState.version == 4):
    ElfShrinkState.qsr_4 = 1
    startTime = time.clock()
    qsr_2_or_3 = qsr4process()
  
  if (REPLACEMENT_MODE != 2):
      # discover_only mode
    if ElfShrinkState.qsr_4 ==1:
      #Work on output file directly
      ElfShrinkState.outputLog.write("".join(["\n","QShrink 4.0 details :","\n"]))
      
      # ELF Manipulator called directly for two section removal.
      elf = elfFileClass.elfFile(outputFile)
      elf.removeSectionByName("QSR_4_0_MSG")
      elf.removeSectionByName("QSR_4_0_MSG_STR")
      
      # Update apps end address
      elf.updateSymbolValueByName("apps_end_post_qsh",ElfShrinkState.linker_endAddr)
      print "Jsumit LOGS"
      qsh_string_section_offset=0xFFFFFFFF
      
      # Print statistics of memory saving
      for i in xrange(0, header['sectionHeaderNum']) :
        section_hdr = getSectionHeader(i);
        print section_hdr.name,"        ",section_hdr.data['sh_offset']
        if section_hdr.name=="QSR_4_0_MSG_STR":
            qsh_string_section_offset=section_hdr.data['sh_offset']
        if qsh_string_section_offset<section_hdr.data['sh_offset']:
            print "MOving up setion ",section_hdr.name
            elf.moveupSectionByName(section_hdr.name)
      elf.writeOutELF(outputFile)
      msaved= (QSR_MSG_SIZE+QSR_MSG_STR_SIZE)/1024.0;
      if (msaved >0):
        print "QShrink memory saving :",msaved/1024," MB "
      ElfShrinkState.outputLog.write("".join(["\n","Total Struct Matches:", str(ElfShrinkState.matches)]))
      ElfShrinkState.outputLog.write("".join(["\n","Qshrunk ",str(QSR_MSG_SIZE/1024)," KB ","\n"]))
  
  # print QShrink output  
  ElfShrinkState.outputLog.close()

  if ElfShrinkState.errCount > 0:
    os.remove(ElfShrinkState.outputPath)

  if ElfShrinkState.errCount > 0:
    resultStrs = [ \
      " ".join(["Total Time:", str(time.clock() - start)]),
      " ".join(["Qshrink completed with errors"]),
      " ".join(["Total Errors Found:", str(ElfShrinkState.errCount),
              "[Investigate errors in logFile]"])
      ]
  else:
    if ((QSR_STRING_SIZE== 0)  and (QSR_MSG_SIZE==0)):
      ElfShrinkState.version = 2
     
    resultStrs = [ \
      " ".join(["Qshrink Version :",
              str(ElfShrinkState.version)]),
      " ".join(["Total Time:", str(time.clock() - start)]),
      " ".join(["Qshrink completed "]),
      ]


  try:
    outputLog = open(ElfShrinkState.logPath, 'a')
  except IOError:
    print "Could not open Log File, ElfShrinkState.logPath"
    exit(1)
  else:
    for strn in resultStrs:
      print(strn)
      outputLog.write(strn)
      outputLog.write("\n")
    outputLog.close()

#=========================================================================
# Function : investigateString 
#
# Arguments
#  strPointer: VA of string
#  section header for string's section 
#
# Description
#    Retrieve the string from the string pointer and section.
#
# Return Values:
#  the string
#=========================================================================
def investigateString(strPointer, strSection):

  global ElfSrhinkState
  
  # check for a non-existant encapsulating section
  if strSection is None :
    print "investigateString(", ", ".join([hex(strPointer), str(strSection)]), "):", hex(strPointer), "is not in a section"
    return None

  if strSection.name not in ElfShrinkState.bytesDict:
    ElfShrinkState.bytesDict[strSection.name] = getSectionData(strSection.name)

  strOffset = strPointer - strSection.data['sh_addr']

  string = getString(ElfShrinkState.bytesDict[strSection.name], strOffset)

  return string

#=========================================================================
# Function : processV3Struct 
#
# Arguments
#  struct: MSG_V3 struct to be processed
#
# Description
#    Goes through and hashifies the pointer or moves the string in the output Elf File
#
# Return Values:
#  new QSR_STRING length, new QSR_STRING
#=========================================================================
def processV3Struct(struct):
  
  global REPLACEMENT_MODE, ElfShrinkState
  QSRstringRE = re.compile('(\d+):(\d+):((.*\x0a*)*)')

  # find QSR_string_ptr, the intermediate pointer
  strPointer = convertFromElf('Elf32_Addr',
                      ElfShrinkState.bytesDict[struct['section']][
                          struct['offset'] + MSG_V3_QSR_POINTER_OFFSET :
                          struct['offset'] + MSG_V3_QSR_POINTER_OFFSET +
                          const['Elf32_Addr']])

  ssMask = convertFromElf('Elf32_Addr',
                      ElfShrinkState.bytesDict[struct['section']][
                          struct['offset'] + MSG_V3_SS_MASK_OFFSET :
                          struct['offset'] + MSG_V3_SS_MASK_OFFSET +
                          const['Elf32_Addr']])

                            
  secHead = getSectionHeader(struct['section'])

  elfOffset = (secHead.data['sh_offset'] + struct['offset'])

  # if strPointer is null, do not count as an error or anomaly, seen in Q6 build
  if (strPointer == 0):    
    anomaly = {}
    anomaly['structName'] = struct['name']
    anomaly['structSection'] = struct['section']
    anomaly['structAddress'] = struct['address']
    anomaly['structOffset'] = struct['offset']
    anomaly['stringAddress'] = strPointer
    anomaly['Anomaly'] = "string pointer is null for msg_v2_struct"
    ElfShrinkState.errors.append(anomaly)
    ElfShrinkState.errCount += 1 
    return

  
  strPointerOffset = strPointer - QSR_STRING_START

  # check if intermediate pointer is outside of QSR_STRING
  # anomaly : QSR_string_ptr outside of QSR_STRING
  if strPointerOffset < 0 or strPointerOffset > QSR_STRING_SIZE:
    strSection = getSectionFromVA(strPointer)
    QSRstring = investigateString(strPointer, strSection)
    if strPointer not in ElfShrinkState.visitedAnomalies:
      ElfShrinkState.visitedAnomalies[strPointer] = 1;
      anomaly = {}
      anomaly['structName'] = struct['name']
      anomaly['Anomaly'] = "qsr string outside QSR_STR section"
      anomaly['structSection'] = struct['section']
      anomaly['structAddress'] = struct['address']
      anomaly['structOffset'] = struct['offset']
      anomaly['stringAddress'] = strPointer
      anomaly['string'] = QSRstring
      anomaly['stringSection'] = strSection.name

      # found the string
      if QSRstring is not None :
        if ElfShrinkState.strAnomalyDict.has_key(strPointer):
          anomaly['stringSymName'] = ElfShrinkState.strAnomalyDict[strPointer]['name']
          del ElfShrinkState.strAnomalyDict[strPointer]
        ElfShrinkState.anomalyStructCount +=1
        ElfShrinkState.anomalies.append(anomaly)
      else :
        anomaly['Anomaly'] = "qsr_str_temp does not point to a valid string"
        ElfShrinkState.errors.append(anomaly)
        ElfShrinkState.errCount += 1 
        return

  # No anomalies  
  else:
    QSRstring = getString(ElfShrinkState.bytesDict['QSR_STRING'], strPointerOffset)
    ElfShrinkState.totalQSR_STRINGlen+=len(QSRstring)+5

  QSRstring = re.sub('\x0d', r'', QSRstring)
  # Double backslash, need to be replaced in string as 
  # Python does not match double backslashes, if present in string.
  QSRstring = re.sub(r"\\", r" ", QSRstring)

  match = QSRstringRE.match(QSRstring)
  if not match:
    print "\n ERROR: Expected decimal value for SSID in Debug MSG(SSID:LINE:FILE:MSG),",QSRstring
    print "Supported Subsystem ID definition : #define SSID_X decimal_value \n"
    ElfShrinkState.errCount += 1 
    return

  ssid = int(match.group(1))
  line = int(match.group(2))
  #Hash Format string
  string = match.group(3)
  string = re.sub(r'\\n', '\x0a', string)
  hash = ""
  ptrUpdated = 0

  if (string in ElfShrinkState.stringHashDict):
    hash = ElfShrinkState.stringHashDict[string]
  elif REPLACEMENT_MODE == 1:
    # move this string and adjust pointers

    #replace : with null, we do not replace with null, since the code checks for ':'
    #string = re.sub(colonFixRE, '\x00', string, 1)

    # Check whether the string was already added to ElfShrinkState.newQSR_STRINGlist
    # The strPointer Dictionary takes care of anomalies too
    if (strPointer in ElfShrinkState.strMovedStrDict):
      newVAddr = ElfShrinkState.strMovedStrDict[strPointer]
      ptrUpdated = 1
    elif (string in ElfShrinkState.stringPtrDict):
      newVAddr = ElfShrinkState.stringPtrDict[string]
      ptrUpdated = 1    
    else:
      newVAddr = QSR_STRING_START_OUTPUT + ElfShrinkState.newQSR_STRINGlen
      ElfShrinkState.strMovedStrDict[strPointer] = newVAddr
      ElfShrinkState.stringPtrDict[string] = newVAddr 

      #add string to list
      ElfShrinkState.newQSR_STRINGlist.append(string)
      ElfShrinkState.newQSR_STRINGlist.append('\x00')
      strLen = len(string) + 1 #for null terminator

      # pad to hit word boundaries
      if strLen % 4 == 3:
        ElfShrinkState.newQSR_STRINGlist.append('\x00')
        strLen = strLen + 1
      elif strLen % 4 == 2:
        ElfShrinkState.newQSR_STRINGlist.append('\x00\x00')
        strLen = strLen + 2
      elif strLen % 4 == 1:
        ElfShrinkState.newQSR_STRINGlist.append('\x00\x00\x00')
        strLen = strLen + 3
      # update new pointer to end of QSR_STRING
      ElfShrinkState.newQSR_STRINGlen += strLen

    # log it
    string = re.sub('\x0a', r'\\n', string)
    if __qsrdebug__:
      ElfShrinkState.outputLog.write(", ".join([secHead.name,
                                              str(struct['section']),
                               hex(struct['address']), struct['name'],
                               hex(elfOffset), hex(newVAddr), string,
                               "moved & updated", hex(strPointer),"\n"]))
      if (ptrUpdated == 1):
        ElfShrinkState.outputLog.write(", ".join([secHead.name,
                                              str(struct['section']),
                               hex(struct['address']), struct['name'],
                               hex(elfOffset), hex(newVAddr), string,
                               "pointer updated", hex(strPointer),"\n"]))

    qsr_str_hash = convertToElf('Elf32_Addr', newVAddr)
    ElfShrinkState.moved += 1
  else:
    # discovery mode
    hash = toHash(string)
    # replace new lines (all unix (0x0a) internally) with special chars to 
    # avoid partial strings in future Master Hash load
    string = re.sub('\x0a', r'\\n', string)
    ElfShrinkState.outputHash.write("".join([
                      str(convertFromElf('Elf32_Word', hash, endian=False)),
                      ":", string, "\n"]))
    ElfShrinkState.newHashedStrings += 1 # increment new strings counter

  if hash != "":
  # replace pointer in QSR struct with hash
    qsr_str_hash = convertToElf('Elf32_Word',
                convertFromElf('Elf32_Word', hash, endian=False),
                 endian=True) #gotta flip direction

    string = re.sub('\x0a', r'\\n', string)
    if __qsrdebug__:
      ElfShrinkState.outputLog.write(
                          ", ".join([secHead.name, str(struct['section']),
                               hex(struct['address']), struct['name'],
                               hex(elfOffset),
                               hex(convertFromElf('Elf32_Word',
                                              hash, endian=False)),
                               string, "hashified",
                               hex(strPointer),"\n"]))
    # hash is written in log and hash file in big-endian order
    ElfShrinkState.hashified += 1

  descstruct = makeQSRDescObject(elfOffset, ssid, ssMask, line, qsr_str_hash) 
  ElfShrinkState.qsrDescStructs.append(descstruct)
  
#=========================================================================
# Function : processV2Struct 
#
# Arguments
#  struct: MSG_V2 struct to be processed
#  output: the output ELF file
#
# Description
#    Goes through and hashifies the pointer or moves the string in the output Elf File
#
# Return Values:
#  new QSR_STRING length, new QSR_STRING
#=========================================================================
def processV2Struct(struct, output):
  
  global REPLACEMENT_MODE, ElfShrinkState

  # find QSR_string_ptr, the intermediate pointer
  strPointer = convertFromElf('Elf32_Addr',
                      ElfShrinkState.bytesDict[struct['section']][
                          struct['offset'] + QSR_POINTER_OFFSET :
                          struct['offset'] + QSR_POINTER_OFFSET +
                          const['Elf32_Addr']])
                            
  secHead = getSectionHeader(struct['section'])

  # check if intermediate pointer is outside of QSR_STRING
  # anomaly : QSR_string_ptr outside of QSR_STRING
  newVAddr = strPointer
  elfOffset = (secHead.data['sh_offset'] + struct['offset'] +
               QSR_POINTER_OFFSET)

  strPointerOffset = strPointer - QSR_STRING_START

  # if strPointer is null, do not count as an error or anomaly, seen in Q6 build
  if (strPointer == 0):
    return
    
  #anomaly Ptr inside QSR String QSR_STRING
  if strPointerOffset < 0 or strPointerOffset > QSR_STRING_SIZE:
    strSection = getSectionFromVA(strPointer)
    string = investigateString(strPointer, strSection)
    if strPointer not in ElfShrinkState.visitedAnomalies:
      ElfShrinkState.visitedAnomalies[strPointer] = 1;
      anomaly = {}
      anomaly['structName'] = struct['name']
      anomaly['Anomaly'] = "qsr string outside QSR_STR section"
      anomaly['structSection'] = struct['section']
      anomaly['structAddress'] = struct['address']
      anomaly['structOffset'] = struct['offset']
      anomaly['stringAddress'] = strPointer
      anomaly['lineNo'] = struct['lineNo'] 
      anomaly['string'] = string
      anomaly['stringSection'] = strSection.name

      # found the string
      if string is not None :
        if ElfShrinkState.strAnomalyDict.has_key(strPointer):
          anomaly['stringSymName'] = ElfShrinkState.strAnomalyDict[strPointer]['name']
          del ElfShrinkState.strAnomalyDict[strPointer]
        ElfShrinkState.anomalyStructCount +=1
        ElfShrinkState.anomalies.append(anomaly)
      else :
        anomaly['Anomaly'] = "qsr_str_temp does not point to a valid string"
        ElfShrinkState.errors.append(anomaly)
        ElfShrinkState.errCount += 1 
        return

  # No anomalies  
  else:
    string = getString(ElfShrinkState.bytesDict['QSR_STRING'], strPointerOffset)
    ElfShrinkState.totalQSR_STRINGlen += len(string)+5

  string = re.sub('\x0d', r'', string)
  # Double backslash, need to be replaced in string as 
  # Python does not match double backslashes, if present in string.
  string = re.sub(r"\\", r" ", string)
  hash = ""
  ptrUpdated = 0
  if (string in ElfShrinkState.stringHashDict):
    hash = ElfShrinkState.stringHashDict[string]
  elif REPLACEMENT_MODE == 1:
    # move this string and adjust pointers

    #replace : with null, we do not replace with null, since the code checks for ':'
    #string = re.sub(colonFixRE, '\x00', string, 1)

    # Check whether the string was already added to ElfShrinkState.newQSR_STRINGlist
    # The strPointer Dictionary takes care of anomalies too
    
    if (strPointer in ElfShrinkState.strMovedStrDict):
      newVAddr = ElfShrinkState.strMovedStrDict[strPointer]
      ptrUpdated = 1
    elif (string in ElfShrinkState.stringPtrDict):
      newVAddr = ElfShrinkState.stringPtrDict[string]
      ptrUpdated = 1    
    else:
      newVAddr = QSR_STRING_START_OUTPUT + ElfShrinkState.newQSR_STRINGlen
      ElfShrinkState.strMovedStrDict[strPointer] = newVAddr
      ElfShrinkState.stringPtrDict[string] = newVAddr

      #add string to list
      ElfShrinkState.newQSR_STRINGlist.append(string)
      ElfShrinkState.newQSR_STRINGlist.append('\x00')
      strLen = len(string) + 1 #for null terminator

      # pad to hit word boundaries
      if strLen % 4 == 3:
        ElfShrinkState.newQSR_STRINGlist.append('\x00')
        strLen = strLen + 1
      elif strLen % 4 == 2:
        ElfShrinkState.newQSR_STRINGlist.append('\x00\x00')
        strLen = strLen + 2
      elif strLen % 4 == 1:
        ElfShrinkState.newQSR_STRINGlist.append('\x00\x00\x00')
        strLen = strLen + 3
      # update new pointer to end of QSR_STRING
      ElfShrinkState.newQSR_STRINGlen += strLen

    # replace pointer in QSR struct with new str addr
    output.seek(elfOffset)
    output.write(convertToElf('Elf32_Addr', newVAddr))

    # log it
    string = re.sub('\x0a', r'\\n', string)
    if __qsrdebug__:
      ElfShrinkState.outputLog.write(", ".join([secHead.name,
                                              str(struct['section']),
                               hex(struct['address']), struct['name'],
                               hex(elfOffset), hex(newVAddr), string,
                               "moved & updated", hex(strPointer) , "\n"]))
      if (ptrUpdated == 1):
        ElfShrinkState.outputLog.write(", ".join([secHead.name,
                                              str(struct['section']),
                               hex(struct['address']), struct['name'],
                               hex(elfOffset), hex(newVAddr), string,
                               "pointer updated", hex(strPointer) , "\n"]))

      
    ElfShrinkState.moved += 1
  else:
    # discovery mode
    hash = toHash(string)
    # replace new lines (all unix (0x0a) internally) with special chars to 
    # avoid partial strings in future Master Hash load
    string = re.sub('\x0a', r'\\n', string)
    ElfShrinkState.outputHash.write("".join([
                      str(convertFromElf('Elf32_Word', hash, endian=False)),
                      ":", string, "\n"]))
    ElfShrinkState.newHashedStrings += 1 # increment new strings counter

  if hash != "":
  # replace pointer in QSR struct with hash
    elfOffset = (secHead.data['sh_offset'] + struct['offset'] +
                 QSR_POINTER_OFFSET)
    output.seek(elfOffset)
    output.write(convertToElf('Elf32_Word',
                convertFromElf('Elf32_Word', hash, endian=False),
                 endian=True)) #gotta flip direction

    string = re.sub('\x0a', r'\\n', string)
    if __qsrdebug__:
      ElfShrinkState.outputLog.write(
                          ", ".join([secHead.name, str(struct['section']),
                               hex(struct['address']), struct['name'],
                               hex(elfOffset),
                               hex(convertFromElf('Elf32_Word',
                                              hash, endian=False)),
                               string, "hashified",
                               hex(strPointer), "\n"]))
    # hash is written in log and hash file in big-endian order
    ElfShrinkState.hashified += 1

#=========================================================================
# Function : updateDiagDescTableAddress 
#
# Arguments
#    output: elf output file
# Description
#   Updates DIAG MSG_V3_SS_TABLE address and number of entries
#
# Return Values:
#  None
#=========================================================================
def updateDiagDescTableAddress(output):
  global ElfShrinkState

  output.seek(ElfShrinkState.diagDescTblEntry)  
  output.write(convertToElf('Elf32_Word', ElfShrinkState.qsr_desc_table_num_entries));

  TblAddr = QSR_STRING_START_OUTPUT + ElfShrinkState.newQSR_STRINGlen
  output.seek(ElfShrinkState.diagDescTbl)  
  output.write(convertToElf('Elf32_Word', TblAddr));
  output.flush()
  
  ElfShrinkState.outputLog.write(" ".join(['DIAG desc table Num entries, ', 
       str(ElfShrinkState.qsr_desc_table_num_entries),"\n"]))
  ElfShrinkState.outputLog.write(" ".join(['DIAG desc table value changed at ',
       hex(ElfShrinkState.diagDescTbl), ' to ',
       hex(TblAddr),"\n"]))
  

#=========================================================================
# Function : updateKernel_VirtEndAddr 
#
# Arguments
#    output: elf output file
# Description
#   Updates Kernel const variable image_vend const data by reducing QSTRING section
#    size from Kernel generated end symbol
#
# Return Values:
#  None
#=========================================================================
def updateKernel_VirtEndAddr(output):
  print "Calling updateKernel_VirtEndAddr"
  global ElfShrinkState,QSR_DESC_ENTRY_SIZE,QSR_EMPTY_SIZE

  align = 0;

  #get the align value from QSR Program Header
  for i in range(0, int(header['programHeaderNum'])):
    ph = ProgramHeader(i) 
    if ph.data['p_vaddr'] == QSR_STRING_START:
      align = ph.data['p_align'];

  old_qsrsize = QSR_STRING_SIZE + align
  old_qsrsize -= old_qsrsize % align

  new_qsr_size = ElfShrinkState.newQSR_STRINGlen
  if ElfShrinkState.version == 3:
    new_qsr_size += ElfShrinkState.qsr_desc_table_num_entries*QSR_DESC_ENTRY_SIZE

  if new_qsr_size == 0:
    # only for version 2 where all strings are removed
    new_qsr_size = QSR_EMPTY_SIZE
  new_q_align = new_qsr_size % align
  if new_q_align != 0: 
    new_qsr_size += align - new_q_align

  ElfShrinkState.linker_endAddr -= QSR_STRING_START - QSR_STRING_START_OUTPUT
  if(ElfShrinkState.use_linker_end_symbol == 1) and (old_qsrsize - new_qsr_size > align):
    ElfShrinkState.linker_endAddr += align
  ElfShrinkState.linker_endAddr -= old_qsrsize - new_qsr_size
     
  output.seek(ElfShrinkState.image_vend_offset)  
  
  output.write(convertToElf('Elf32_Word', ElfShrinkState.linker_endAddr));
  output.flush()  
    
#=========================================================================
# Function : processStructs 
#
# Arguments
#  structs: MSG_V2 structs
#  output : output Elf File
#
# Description
#   Goes through the list of structs found,
#    uses the 'QSR_string_ptr' to a string, hashes 
#     the string, and replaces the pointer with the hash.
#
# Return Values:
#  None
#=========================================================================
def processStructs(structs, output):

  global REPLACEMENT_MODE, ElfShrinkState

  ElfShrinkState.anomalies = []

  ElfShrinkState.qsrDescStructs = []

  if __qsrdebug__:
    print "Processing all QSR Structs..."

  #sort into sections
  structs.sort(cmp=lambda x, y: cmp(x['address'], y['address']))

  #setup for replacement mode
  ElfShrinkState.newQSR_STRINGlist = []
  ElfShrinkState.newQSR_STRINGlen = 0
  ElfShrinkState.totalQSR_STRINGlen = 0

  # keeps track of Virtual Addresses which have already been visited
  visited = {}

  #for changing strings that get moved in replacement mode
  
  if __qsrdebug__:
    #print header for csv style logging of changes
    ElfShrinkState.outputLog.write(", ".join(["Section Name",
                                            "Section Index",
                                            "Struct VA",
                                            "Symbol Name",
                                            "ELF File Offset",
                                            "Hash",
                                            "Hashed String" ,
                                            "Action Taken",
                                            "Original VA",
                                             "\n"]))

  for struct in structs:
    # loop over structs doing stuff

    # do not visit same struct twice
    if struct['address'] not in visited:
      visited[struct['address']] = 1
      ElfShrinkState.uniqueStructMatches += 1
    else:
      continue

    if struct['section'] not in ElfShrinkState.bytesDict:
      ElfShrinkState.bytesDict[struct['section']] = getSectionData(struct['section'])
      # load new section

#   process Struct even for anomalies    
  # end loop over structs
    if struct['version'] == 3:
      processV3Struct(struct)    
    elif struct['version'] == 2:
      lineNo = convertFromElf('Elf32_Half',
                        ElfShrinkState.bytesDict[struct['section']][struct['offset'] + 0 :
                          struct['offset'] + 0 + 2])

      struct['lineNo'] = lineNo
      processV2Struct(struct, output)

  
  del ElfShrinkState.qsrDescStructs

#=========================================================================
# Function : unAlignHash 
#
# Arguments
#   hash: hash to be unAlinend(not divisible by 4)
#
# Description
#   adds one if the last 2 bits are 00
#
# Return Values:
#  the hash, either untouch or unaligned
#=========================================================================
def unAlignHash(hash):
  '''
  @param hash: hash to be unAlinend
  @return: the hash, either untouch or unaligned
  adds one if the last 2 bits are 00
  '''
  if hash & 0x3 == 0:
    hash += 1
  return hash

#=========================================================================
# Function : toHash 
#
# Arguments
#   string: (str) String to hash.  Should be filename:message
#
# Description
#   Translate a string into a hash.  First checks to see if the string already
#   has a hash.  Otherwise, computes one and returns it. 
#   NOTE: Only used in discovery mode.
#  @TODO: Make hash computation faster on collision case
#  @TODO: Make case for full hash
#  @TODO: Handle int wrap-around case
#
# Return Values:
#  (str) Unique hash for the string.
#=========================================================================
def toHash(string):
  global ElfSrhinkState
  hash = hashlib.md5(string).digest()[-4:]
  # lowest 4 bytes of hash

  hash = convertToElf('Elf32_Word',
                      unAlignHash(convertFromElf('Elf32_Word', hash,
                                             endian=False)),
                      endian=False)
 
  # check if hash is unused or if string is already mapped to hash
  if ((hash not in ElfShrinkState.hashStringDict) or (ElfShrinkState.hashStringDict[hash] == string)) :
    ElfShrinkState.hashStringDict[hash] = string
    ElfShrinkState.stringHashDict[string] = hash
    return hash

  # search for an unused hash
  hash = convertToElf('Elf32_Word',
                      unAlignHash(convertFromElf('Elf32_Word', hash,
                                             endian=False) + 1),
                      endian=False) # @TODO: improve collision resolution
  new_hash = 2
  while hash in ElfShrinkState.hashStringDict :    
    if ElfShrinkState.hashStringDict[hash] == string :
      if __qsrdebug__:
        print "Hash already present but tried to hash again:", \
              hex(convertFromElf('Elf32_Word', hash, endian=False)), string
      return hash
    hash = convertToElf('Elf32_Word',
                        unAlignHash(convertFromElf('Elf32_Word', hash,
                                             endian=False) + new_hash),
                        endian=False) # @TODO: improve collision resolution
    new_hash += 1
  # cache results
  ElfShrinkState.hashStringDict[hash] = string
  ElfShrinkState.stringHashDict[string] = hash
  return hash

#=========================================================================
# Function : symbolTableEntryGenerator 
#
# Arguments
#   bytes: (str) String of bytes which contains the Symbol Table
#   strTabData: (str) String of bytes which contains the String table
#
# Description
#   Creates a generator over the given bytes (should be the Symbol Table) that 
#   yields the next 16 bytes as a Symbol Table Entry.
#
# Return Values:
#  (generator:SymbolTableEntry) Iterable which has SymbolTableEntry
#          objects according to bytes
#=========================================================================
def symbolTableEntryGenerator(bytes, strTabData):
  assert len(bytes) % 16 == 0
  for i in xrange(0, len(bytes), 16) :
    yield SymbolTableEntry(bytes[i : i + 16], strTabData)

#=========================================================================
# Function : initializeWithElfHeader 
#
# Arguments
#
# Description
#   @warning: May suddenly exit script if bad header information is found 
#   Sets global data according to the ELF header.
#
# Return Values:
#=========================================================================
def initializeWithElfHeader():
  global ElfShrinkState
  global QSR_STRING_START, QSR_STRING_SIZE, QSR_STRING_END, QSR_STRING_OFFSET
  global QSR_MSG_START, QSR_MSG_SIZE, QSR_MSG_END, QSR_MSG_OFFSET
  global QSR_MSG_STR_START, QSR_MSG_STR_SIZE, QSR_MSG_STR_END, QSR_MSG_STR_OFFSET
  global QSR_STRING_START_OUTPUT

  ElfShrinkState.outputLog.write("QSHRINK4FUNC: initializeWithElfHeader\n")

  qsr_23 =0
  QSR_STRING_START = 0
  QSR_STRING_SIZE = 0
  QSR_STRING_END = 0
  QSR_STRING_OFFSET = 0
  QSR_MSG_START = 0
  QSR_MSG_SIZE = 0
  QSR_MSG_END = 0
  QSR_MSG_OFFSET = 0
  QSR_MSG_STR_START=0
  QSR_MSG_STR_SIZE=0
  QSR_MSG_STR_END=0
  QSR_MSG_STR_OFFSET=0

  for key in ElfShrinkState.sectionHeaderDict.keys():
    ElfShrinkState.sectionHeaderDict.pop(key) #clear all

  # set global header's entries according to ELF header
  try: 
    input = open(ElfShrinkState.path, 'rb')
  except IOError:
    print "Error, Could not open input Elf File, ", ElfShrinkState.path
    exit(1)
  else:
    elf_header = input.read(52)
    processHeader(elf_header)
    input.close()

  # get all section headers and map by index and name
  ElfShrinkState.sectionNameHeader = getSectionHeader(header['sectionNameTable'])
  ElfShrinkState.sectionNameData = getSectionData(header['sectionNameTable'])
  if __qsrdebug__:
    print "Loading all Section Headers"
  sectionStart = time.clock()
  for i in xrange(0, header['sectionHeaderNum']) :
    ElfShrinkState.sectionHeaderDict[i] = getSectionHeader(i)
    ElfShrinkState.sectionHeaderDict[getSectionHeader(i).name] = ElfShrinkState.sectionHeaderDict[i]
  if __qsrdebug__:
    print "Finished Loading Section Headers (", (time.clock()
                                               - sectionStart), "seconds)"
  # print "Number of Sections: ", header['sectionHeaderNum']

  # set global data about the QSR string section
  # mainly used for shorter calculations
  if ('QSR_4_0_MSG' in ElfShrinkState.sectionHeaderDict):
    qsrStringHeader = getSectionHeader('QSR_4_0_MSG')
    QSR_MSG_START = qsrStringHeader.data['sh_addr']
    QSR_MSG_SIZE = qsrStringHeader.data['sh_size']
    QSR_MSG_END = QSR_MSG_START + QSR_MSG_SIZE
    QSR_MSG_OFFSET = qsrStringHeader.data['sh_offset']
    
    qsrStringHeader_data = getSectionHeader('QSR_4_0_MSG_STR')
    QSR_MSG_STR_START = qsrStringHeader_data.data['sh_addr']
    QSR_MSG_STR_SIZE = qsrStringHeader_data.data['sh_size']
    QSR_MSG_STR_END = QSR_MSG_STR_START + QSR_MSG_STR_SIZE
    QSR_MSG_STR_OFFSET = qsrStringHeader_data.data['sh_offset']
    
    print "QSR_MSG_START %08x QSR_MSG_SIZE %08x QSR_MSG_END %08x QSR_MSG_OFFSET %08x" %(QSR_MSG_START,QSR_MSG_SIZE,QSR_MSG_END,QSR_MSG_OFFSET)
    print "QSR_MSG_STR_START %08x QSR_MSG_STR_SIZE %08x QSR_MSG_STR_END %08x QSR_MSG_STR_OFFSET %08x" %(QSR_MSG_STR_START,QSR_MSG_STR_SIZE,QSR_MSG_STR_END,QSR_MSG_STR_OFFSET)
    
    ElfShrinkState.outputLog.write( "QSR_MSG_START %08x QSR_MSG_SIZE %08x QSR_MSG_END %08x QSR_MSG_OFFSET %08x\n" %(QSR_MSG_START,QSR_MSG_SIZE,QSR_MSG_END,QSR_MSG_OFFSET))
    ElfShrinkState.outputLog.write( "QSR_MSG_STR_START %08x QSR_MSG_STR_SIZE %08x QSR_MSG_STR_END %08x QSR_MSG_STR_OFFSET %08x\n" %(QSR_MSG_STR_START,QSR_MSG_STR_SIZE,QSR_MSG_STR_END,QSR_MSG_STR_OFFSET))

    ElfShrinkState.version = 4

  if ('QSR_4_0_MSG' in ElfShrinkState.sectionHeaderDict):
     ElfShrinkState.linker_endAddr = 0
     align = 4096
     for i in range(0, int(header['programHeaderNum'])):
        ph = ProgramHeader(i)
        if (ElfShrinkState.linker_endAddr < (ph.data['p_vaddr'] + ph.data['p_memsz'])
             and (ph.data['p_type'] == 1)
             and (ph.data['p_vaddr'] < QSR_MSG_START)):
          ElfShrinkState.linker_endAddr = ph.data['p_vaddr'] + ph.data['p_memsz'] 
          align =  ph.data['p_align'] 
     ElfShrinkState.linker_endAddr = align * (ElfShrinkState.linker_endAddr/align +1)
   
  return qsr_23  
  
#=========================================================================
# Function : getString 
#
# Arguments
#  bytes: (str) Bytes that contain a null character.
#  begin: (int) Where to start in the given string of bytes.
#  end: (int) Where to end, non-inclusive, in the given string of bytes
#
# Description
#   Gets the first null-terminated string from bytes[begin : end].  If end is
#   not given, then it will search the bytes[begin : ].  If no null is found,
#   then it will return bytes[begin : ]
#
# Return Values:
#   All characters from begin to either end or the first null char.
#=========================================================================
def getString(bytes, begin=0, end= -1):
  index = begin
  if end == -1 : end = len(bytes)
  while (index < end and bytes[index] != '\x00'):
#         and bytes[index] != '\x01'):
    index += 1
  return bytes[begin:index]
  
#=========================================================================
# Function : getStringGenerator 
#
# Arguments
#   bytes: (str) Bytes to get a string from
#   begin: (int) Start offset into bytes
#   end: (int) End offset into bytes.  Non-inclusive.
#
# Description
#   Deprecated.  Requires only a single-null byte between strings and a
#   contiguous string section.  Only yields strings in order.  Not used since
#   strings are not guaranteed to be in order.
#   Returns a generator that yields the next string in bytes. 
#
# Return Values:
#   (generator:str) Iterable which has strings from bytes
#=========================================================================
def getStringGenerator(bytes, begin=0, end= -1):
  new_index = 0
  index = begin
  if end == -1 : end = len(bytes)    
  while index < end :
    while index < end and bytes[index] != '\x00' :
      index += 1
    new_index = index
    while index < end :
      if bytes[index] == '\x00' :
        index += 1
      else:
        break
    yield begin,bytes[begin:new_index]
    begin = index

#=========================================================================
# Function : getSectionData 
#
# Arguments
#   section: ([str|int]) Index of the section whose data you want
#
# Description
#   Reads the input ELF file and grabs the section's data.  Does not cache to 
#   avoid a Memory Error when part of a build.
#
# Return Values:
#   section data
#=========================================================================
def getSectionData(section):
  secheader = getSectionHeader(section)
  offset = secheader.data['sh_offset']
  size = secheader.data['sh_size']
  try:
    input = open(ElfShrinkState.path, 'rb')
  except IOError:
    print "Error: Could not open input ELF file, ",ElfShrinkState.path
    exit(1)
  else:
    input.seek(offset)
    ret = input.read(size)
    input.close()
  return ret


#=========================================================================
# Function : processHeader 
#
# Arguments
#   elf_header: (str) Bytes of the header
#
# Description
#   Goes through the ELF header and sets entries in the global header dict
#   accordingly.
#
# Return Values:
#=========================================================================
def processHeader(elf_header):

  global header
  header_data = header # local name is faster
  e_ident = elf_header[0:16]
  checkEIDENT(header_data, e_ident)
  if __qsrdebug__:
    print ""
  checkElfHeaderType    (convertFromElf('Elf32_Half', elf_header[16:18]))
  checkElfHeaderMachine (convertFromElf('Elf32_Half', elf_header[18:20]))
  checkElfHeaderVersion (convertFromElf('Elf32_Word', elf_header[20:24]))
  header_data["entry"] = convertFromElf('Elf32_Addr', elf_header[24:28])
  header_data["programHeaderOff"] = convertFromElf(
                                       'Elf32_Off' , elf_header[28:32])
  header_data["sectionHeaderOff"] = convertFromElf(
                                       'Elf32_Off' , elf_header[32:36])
  getElfHeaderFlags(header_data, convertFromElf(
                                       'Elf32_Word', elf_header[36:40]))
  header_data["headerSize"] = convertFromElf(
                                       'Elf32_Half', elf_header[40:42])
  header_data["programHeaderSize"] = convertFromElf(
                                       'Elf32_Half', elf_header[42:44])
  header_data["programHeaderNum"] = convertFromElf(
                                       'Elf32_Half', elf_header[44:46])
  header_data["sectionHeaderSize"] = convertFromElf(
                                       'Elf32_Half', elf_header[46:48])
  header_data["sectionHeaderNum"] = convertFromElf(
                                       'Elf32_Half', elf_header[48:50])
  header_data["sectionNameTable"] = convertFromElf(
                                       'Elf32_Half', elf_header[50:52])

#=========================================================================
# Function : getElfHeaderFlags 
#
# Arguments
#   header: (str) Global header dictionary
#   e_flags: (str) Bytes of the e_flags field
#
# Description
#   Print out known flags in the ELF file.  No real reason for this to exist
#   as of yet.
#
# Return Values:
#=========================================================================
def getElfHeaderFlags(header, e_flags):

  header['flags'] = e_flags
  #if __qsrdebug__:
    #print "Flags:", header['flags'], hex(header['flags'])
    #if header['flags'] & 0x2: print "Flag: EF_ARM_HASENTRY"
    #if header['flags'] & 0x4: print "Flag: EF_ARM_SYMSARESORTED"
    #if header['flags'] & 0x8: print "Flag: EF_ARM_DYNSYMSUSESEGIDX"
    #if header['flags'] & 0x10: print "Flag: EF_ARM_MAPSYMSFIRST"
    #if header['flags'] & 0xff000000 == 0x02000000 :
      #print "Flag: EF_ARM_EABI -> Version 2"
    #elif header['flags'] & 0xff000000 == 0 :
      #print "Flag: EABI Unknown Conformance"
    #print "Flags only relevant to ARM/Thumb Architecture"
    #print ""

#=========================================================================
# Function : checkElfHeaderVersion 
#
# Arguments
#   e_version: number read from ELF header
#
# Description
#   checks version against constants
#
# Return Values:
#=========================================================================
def checkElfHeaderVersion(e_version):
  num = e_version
  if    num == const['EV_NONE']    : print "Invalid Version"
  elif  num == const['EV_CURRENT'] :
    pass
#    print "Current Version"
  else:
    print "Unknown Version: ", num

#=========================================================================
# Function : checkElfHeaderVersion 
#
# Arguments
#   e_machine: number read from ELF header
#
# Description
#   Checks if machine type is compatable with this script.
#
# Return Values:
#=========================================================================
def checkElfHeaderMachine(e_machine):
  num = e_machine
  if    num == const['EM_NONE'] :
    pass
#    print "No Machine", "Cannot Check Flags"
  elif  num == const['EM_M32'] :
    pass
#    print "AT&T WE 32100", "Cannot Check Flags"
  elif  num == const['EM_SPARC'] :
    pass
#    print "SPARC", "Cannot Check Flags"
  elif  num == const['EM_386'] :
    pass
#    print "Intel Architecture", "Cannot Check Flags"
  elif  num == const['EM_68K'] :
    pass
#    print "Motorola 68000", "Cannot Check Flags"
  elif  num == const['EM_88K'] :
    pass
#    print "Motorola 88000", "Cannot Check Flags"
  elif  num == const['EM_860'] :
    pass
#    print "Intel 80860", "Cannot Check Flags"
  elif  num == const['EM_MIPS'] :
    pass
#    print "MIPS RS3000 Big-Endian", "Cannot Check Flags"
  elif  num == const['EM_MIPS_RS4_BE'] :
    pass
#    print "MIPS RS4000 Big-Endian", "Cannot Check Flags"
  elif  num == const['EM_ARM'] :
    pass
#    print "ARM/Thumb Architecture"
  else:
#    print "Machine: ", num
    pass

#=========================================================================
# Function : checkElfHeaderType 
#
# Arguments
#   e_type: number read from ELF header
#
# Description
#   Checks if machine type is compatable with this script.
#
# Return Values:
#=========================================================================
def checkElfHeaderType(e_type):
  num = e_type
  if num != const['ET_EXEC'] :
    print "ELF Type Must Be Executable to Modify"
    exit()
#  if    num == const['ET_NONE']  : print "No File Type"
#  elif  num == const['ET_REL']   : print "Re-Locatable File"
#  elif  num == const['ET_EXEC']  : print "Executable File"
#  elif  num == const['ET_DYN']   : print "Shared Object File"
#  elif  num == const['ET_CORE']  : print "Core File"
#  elif  const['ET_LOPROC'] <= num <= const['ET_HIPROC']: 
#    print "Processor Specific:", str(num)
#  else:
#    print "Unknown Type:", str(num)
#    exit()

#=========================================================================
# Function : convertFromElf 
#
# Arguments
#   typeOrLen: str|int - either name of Elf32 type or length in bytes
#   byteString: to be converted to an unsigned number
#   endian: should endianness be adjusted from ELF file? 
#
# Description
#   This accounts for endianness.
#
# Return Values:
#   a number based on the given format
#=========================================================================
def convertFromElf(typeOrLen, byteString, endian=True):
  endianness = ">" #big by default, like english, MSB first
  if endian == True:
    if header['endian'] == 1:
      endianness = "<" #little
    else:
      endianness = ">" #big

  size = 0
  if typeOrLen in const:
    size = const[typeOrLen]
  else:
    size = typeOrLen
  assert size == len(byteString)

  if size == 4:
    format = "L"
  elif size == 2:
    format = "H"
  elif size == 1:
    format = "B"
  elif size == 8:
    format = "Q"
  else:
    raise Exception("bad convertFromElf input: " +
                    str(typeOrLen) + " " + byteString)

  return struct.unpack(endianness + format, byteString)[0]

#=========================================================================
# Function : convertToElf 
#
# Arguments
#   typeOrLen: str|int - either name of Elf32 type or length in bytes
#   number: unsigned number to be converted
#   endian: should endianness be adjusted to match ELF file? 
#
# Description
#   This accounts for endianness.
#
# Return Values:
#   a byte string in the correct format
#=========================================================================
def convertToElf(typeOrLen, number, endian=True):

  endianness = ">" #big by default, like english, MSB first
  if endian == True:
    if header['endian'] == 1:
      endianness = "<" #little
    else:
      endianness = ">" #big

  size = 0
  if typeOrLen in const:
    size = const[typeOrLen]
  else:
    size = typeOrLen

  if size == 4:
    format = "L"
  elif size == 2:
    format = "H"
  elif size == 1:
    format = "B"
  elif size == 8:
    format = "Q"
  else:
    raise Exception("bad convertToElf input: " +
                    str(typeOrLen) + " " + str(number))

  return struct.pack(endianness + format, number)

#=========================================================================
# Function : getSectionHeader 
#
# Arguments
#   args: ([str|int]) If empty, then return an empty SectionHeaderClass.
#                           Otherwise, if first arg is a str, return a 
#                           SectionHeaderClass by name.  If first arg is an
#                           int, return a SectionHeaderClass by index.
#                           Otherwise, return an empty SectionHeaderClass.
#
# Description
#   Wrapper around SectionHeaderClass's constructor to manage the
#   ElfShrinkState.sectionHeaderDict caches.
# Return Values:
#   a byte string in the correct format
#=========================================================================
def getSectionHeader(*args):

  global ElfShrinkState
  if len(args) != 0:
    if args[0] in ElfShrinkState.sectionHeaderDict :
      return ElfShrinkState.sectionHeaderDict[args[0]]
    else:
      return SectionHeaderClass(args[0])
  return SectionHeaderClass()

#=========================================================================
# Class : SectionHeaderClass 
#
# Arguments
#
# Description
#   Initializes data dictionary and grabs header data if index or name
#   is given to the constructor.
#
# Return Values:
#   Section Header class dictionary
#=========================================================================
class SectionHeaderClass:
  def __init__(self, *args):
    '''
    @param *args: Eats up optional parameters.  If they are given, there must
                  be two of them.  The first one is used as the path and the 
                  second the index in a call to readIndex right after the 
                  creation of the data dictionary.
                  
    Initializes data dictionary and grabs header data if index or name
    is given to the constructor.
    '''
    self.name = "NO_NAME"
    self.data = {
      'sh_name'      : 0, # index in section header string table with name
      'sh_type'      : 0, # section's contents and semantics type
      'sh_flags'     : 0,
      'sh_addr'      : 0, # address of section's first byte in image
      'sh_offset'    : 0, # offset from beginning of file to section data
      'sh_size'      : 0, # size of section's data
      'sh_link'      : 0, # section header table index link. depends on type
      'sh_info'      : 0, # depends on type
      'sh_addralign' : 0, # alignment of the entire section
      'sh_entsize'   : 0  # size of entries. only for certain types  
    }
    self.index = 0
    if len(args) != 0 :
      if (type(args[0]) is int
        and 0 <= args[0] <= header['sectionHeaderNum']):
        self.readIndex(args[0])
      elif type(args[0]) is str :
        self.getByName(args[0])
      else: assert False


  def getByName(self, name):
    for i in range(header['sectionHeaderNum'] - 1, 0, -1) :
      self.readIndex(i)
      if self.name == name : return
    print "SectionHeaderClass : getByName(", name, "): Bad Name"
    assert False

  def readIndex(self, index):
    '''
    @param path: The path to the file to read through
    @param index: The index of the section header to read.  File offset is 
                  calculated based off of this.  0 is the null header.
    Gets header data by index.  Also assigns index to object.  Not all data
    is needed and so is not read to save time.
    '''
    global ElfShrinkState
    assert 0 <= index <= header['sectionHeaderNum']
    offset = header['sectionHeaderOff'] + index * header['sectionHeaderSize']
    try :
      input = open(ElfShrinkState.path, 'rb')
    except IOError:
      print "Error: Could not open Input, elf file", ElfShrinkState.path
      exit(1)
    else:
      input.seek(offset)
      bytes = input.read(header['sectionHeaderSize'])
      self.data['sh_name'] = convertFromElf('Elf32_Word', bytes[0:4])
      self.data['sh_type'] = convertFromElf( 'Elf32_Word', bytes[4:8] )
      self.data['sh_flags'] = convertFromElf( 'Elf32_Word', bytes[8:12] )
      self.data['sh_addr'] = convertFromElf('Elf32_Addr', bytes[12:16])
      self.data['sh_offset'] = convertFromElf('Elf32_Off', bytes[16:20])
      self.data['sh_size'] = convertFromElf('Elf32_Word', bytes[20:24])
      self.data['sh_link'] = convertFromElf( 'Elf32_Word', bytes[24:28] )
#      self.data['sh_info'] = convertFromElf( 'Elf32_Word', bytes[28:32] )
      self.data['sh_addralign'] = convertFromElf('Elf32_Word', bytes[32:36])
#      self.data['sh_entsize'] = getFromElf( 'Elf32_Word', bytes[36:40] )
      input.close()
    self.index = index
    # need this check in case this header is used to find sectionNameData
    if ElfShrinkState.sectionNameData is not None :
      self.name = getString(ElfShrinkState.sectionNameData, self.data['sh_name'])

  def printData(self):
    print "SECTION HEADER:", self.index, self.name
    flags = self.data['sh_flags']
    if flags & 0x1 != 0 :         print "FLAG: SHF_WRITE"
    if flags & 0x2 != 0 :         print "FLAG: SHF_ALLOC"
    if flags & 0x4 != 0 :         print "FLAG: SHF_EXECINSTR"
    if flags & 0xf0000000 != 0 :  print "FLAG: SHF_MASKPROC"

    type = self.data['sh_type']
    if   type == const['SHT_NULL']:     print "TYPE: SHT_NULL"
    elif type == const['SHT_PROGBITS']: print "TYPE: SHT_PROGBITS"
    elif type == const['SHT_SYMTAB']:   print "TYPE: SHT_SYMTAB"
    elif type == const['SHT_STRTAB']:   print "TYPE: SHT_STRTAB"
    elif type == const['SHT_RELA']:     print "TYPE: SHT_RELA"
    elif type == const['SHT_HASH']:     print "TYPE: SHT_HASH"
    elif type == const['SHT_DYNAMIC']:  print "TYPE: SHT_DYNAMIC"
    elif type == const['SHT_NOTE']:     print "TYPE: SHT_NOTE"
    elif type == const['SHT_NOBITS']:   print "TYPE: SHT_NOBITS"
    elif type == const['SHT_REL']:      print "TYPE: SHT_REL"
    elif type == const['SHT_SHLIB']:    print "TYPE: SHT_SHLIB"
    elif type == const['SHT_DYNSYM']:   print "TYPE: SHT_DYNSYM"
    elif const['SHT_LOPROC'] <= type <= const['SHT_HIPROC']:
      print "TYPE: PROCESSOR SPECIFIC"
    elif const['SHT_LOUSER'] <= type <= const['SHT_HIUSER']:
      print "TYPE: RESERVED FOR APPLICATIONS"

    for key in self.data :
      if key != "sh_flags" and key != "sh_type" :
        print key, ":", self.data[key], hex(self.data[key])
    print ""

#=========================================================================
# Class : ProgramHeader 
#
# Arguments
#
# Description: Program Header Dictionary
#
# Return Values:
#=========================================================================
class ProgramHeader:
  def __init__(self, *args):
    self.data = {
      'p_type'    : 0,
      'p_offset'  : 0,
      'p_vaddr'   : 0,
      'p_paddr'   : 0,
      'p_filesz'  : 0,
      'p_memsz'   : 0,
      'p_flags'   : 0,
      'p_align'   : 0,
    }
    self.index = 0
    if len(args) != 0:
      self.readIndex(args[0])

  def readIndex(self, index):
    global ElfShrinkState
    assert (index < header['programHeaderNum'])

    self.offset = header['programHeaderOff'] + index * header['programHeaderSize']
    try:
      input = open(ElfShrinkState.path, 'rb')
    except IOError:
      print "Error: Could not open Input, elf file", ElfShrinkState.path
      exit(1)
    else:
      input.seek(self.offset)
      bytes = input.read(header['programHeaderSize'])
      self.data['p_type'] = convertFromElf('Elf32_Word', bytes[0:4])
      self.data['p_offset'] = convertFromElf('Elf32_Off', bytes[4:8])
      self.data['p_vaddr'] = convertFromElf('Elf32_Addr', bytes[8:12])
      self.data['p_paddr'] = convertFromElf('Elf32_Addr', bytes[12:16])
      self.data['p_filesz'] = convertFromElf('Elf32_Word', bytes[16:20])
      self.data['p_memsz'] = convertFromElf('Elf32_Word', bytes[20:24])
      self.data['p_flags'] = convertFromElf('Elf32_Word', bytes[24:28])
      self.data['p_align'] = convertFromElf('Elf32_Word', bytes[28:32])
      input.close()
    self.index = index

  def printData(self):
    print "Program Header:", self.index
    for key in self.data :
      print key, ":", self.data[key], hex(self.data[key])
    print ""

#=========================================================================
# Function : makeQSRobject 
#
# Arguments
#   entry: a symbol table entry object
#   args: if available, the section header object 
#
# Description : make an object with the structure data
#
# Return Values:
#   a dictionary for the object with things from entry filled in
#=========================================================================
def makeQSRobject(entry, *args):
  ''' 
  @param entry: a symbol table entry object
  @param args: if available, the section header object 
  @return: a dictionary for the object with things from entry filled in 
  '''
  d = {
       'name' : entry.name,
       'size' : entry.size,
       'address' : entry.value,
       'section' : entry.sectionIndex
      }
  if len(args) > 0:
    d['section'] = args[0].name
    d['offset'] = entry.value - args[0].data['sh_addr']
    d['elf_offset'] = d['offset'] + args[0].data['sh_offset']
  return d

#=========================================================================
# Function : makeQSRDescobject 
#
# Arguments
#   entry: QSR desc bytes
#
# Description : make an object with the Desc structure
#
# Return Values:
#   a dictionary for the object with things from entry filled in
#=========================================================================
def makeQSRDescObject(elfoffset, ssid, ssmask, line, msg_hash):
  ''' 
  @return: a dictionary for the object with things from entry filled in 
  '''
  d = {
       'elfoffset' : elfoffset,
       'ss_id' : ssid,
       'ss_mask' : ssmask,
       'line' : line,
       'qsr_msg_hash' : msg_hash
      }
  return d

#=========================================================================
# Function : makeQSRDescobject 
#
# Arguments
#   entry: QSR4 desc bytes
#
# Description : make an object with the Desc structure
#
# Return Values:
#   a dictionary for the object with things from entry filled in
#=========================================================================
def makeQSR4DescObject(aam_uam_index,uam_address,ssid, ssmask):
  ''' 
  @return: a dictionary for the object with things from entry filled in 
  '''
  d = {
       'aam_uam_index' : aam_uam_index,
       'uam_address' : uam_address,       
       'ss_id' : ssid,
       'ss_mask' : ssmask
      }
  return d
    

#=========================================================================
# Class : SymbolTableEntry 
#
# Arguments
#    bytes: Entry in bytes
#   stringTable : To get the symbol names
#
# Description: Symbol Table Entry 
#
# Return Values:
#=========================================================================
class SymbolTableEntry:
  def __init__(self, bytes, stringTable):
    self.data = {
      'st_name'   : 0,
#      'st_value'  : 0,
#      'st_size'   : 0,
#      'st_info'   : 0,
      'st_other'  : 0,
#      'st_shndx'  : 0
    }
#    self.data['st_name'] = convertFromElf( 'Elf32_Word', bytes[0:4] )
#    self.data['st_value'] = convertFromElf( 'Elf32_Addr', bytes[4:8] )
    self.value = convertFromElf('Elf32_Addr', bytes[4:8])
#    self.data['st_size'] = convertFromElf( 'Elf32_Word', bytes[8:12] )
    self.size = convertFromElf('Elf32_Word', bytes[8:12])
#    self.data['st_info'] = convertFromElf( 1, bytes[12] ,False)
#    self.st_info = convertFromElf( 1, bytes[12] ,False)
#    self.data['st_other'] = convertFromElf( 1, bytes[13] ,False)
#    self.data['st_shndx'] = convertFromElf( 'Elf32_Half', bytes[14:16] )
    self.sectionIndex = convertFromElf('Elf32_Half', bytes[14:16])
    self.name = getString(stringTable, convertFromElf('Elf32_Word', bytes[0:4]))

  def printData(self):
    print "Symbol Table Entry:", self.name
    for key in self.data :
      print key, ":", self.data[key], hex(self.data[key])
    print ""

#QShrink4
#=========================================================================
# Function : qsr4process
#
# Arguments
#
# Description
#   Iterates through symbol table entries, finding MSG V2 structs.
#   Then it goes through the structs hashify-ing or moving their strings.
#
# Return Values:
#  If QSR 2.0 or QSR 3.0 structures are found in code, then return 1, else 
#  return 0
#=========================================================================
def qsr4process():

  global ElfShrinkState
  global QSR_MSG_START, QSR_MSG_SIZE, QSR_MSG_END, QSR_MSG_OFFSET
  global QSR_MSG_STR_START, QSR_MSG_STR_SIZE, QSR_MSG_STR_END, QSR_MSG_STR_OFFSET

  qsr_23 = qsr4ReadVariables()
  ElfShrinkState.bytesDict['QSR_4_0_MSG'] = getSectionData('QSR_4_0_MSG')
  ElfShrinkState.bytesDict['QSR_4_0_MSG_STR'] = getSectionData('QSR_4_0_MSG_STR')
  print "Starting processV4Structs"
  processV4Structs()
  qsr4CreateHashFile()
  
  if len(ElfShrinkState.QSR4_STRINGlist) !=0:    
    del ElfShrinkState.QSR4_STRINGlist #free memory
    processQSR4MsgDescTable()
    qsr4UpdateVariables()  
    
  for key in ElfShrinkState.bytesDict.keys():
    ElfShrinkState.bytesDict.pop(key) #clear all except QSR_MSG from mem    
  return qsr_23

#=========================================================================
# Function : qsr4ReadVariables
#
# Arguments
#
# Description
#   Iterates through symbol table entries, finding QShrink variables
#   msg_v4_ss_num_aligned_entries 
#   msg_v4_ss_num_unaligned_entries
#   msg_v4_ss_num_table_entries
#   msg_v4_ss_table
#   msg_v4_ss_address_table
#   msg_v4_dbase_guid (Database GUID)
#   msg_v4_dbase_name (Database name)
#   qsr_4_0_msg_aligned_start_address  
#   qsr_4_0_msg_unaligned_start_address  
#   qsr_4_0_msg_end_address 
#
# Return Values:
#  List of structs
#=========================================================================
def qsr4ReadVariables():
 
  global ElfShrinkState
  global QSR_MSG_START, QSR_MSG_SIZE, QSR_MSG_END, QSR_MSG_OFFSET
  global QSR_MSG_STR_START, QSR_MSG_STR_SIZE, QSR_MSG_STR_END, QSR_MSG_STR_OFFSET
  
  qsr_23 = 0
  qsr4structFound = 0
  
# get the symbol table and the string table to find symbol names
  symbolTableData = getSectionData('.symtab')
# all symbol data
  stringTableData = getSectionData('.strtab')
# all symbol names
  qsrRE = re.compile(r'xx_msg_v[234]_const')
  qsr20RE = re.compile(r'xx_msg_v2_const')
  qsr30RE = re.compile(r'xx_msg_v3_const')
  qsr40RE = re.compile(r'xx_msg_v4_const')
  errRE = re.compile(r'xx_err_msg_v[23]_const')
  err20RE = re.compile(r'xx_err_msg_v2_const')
  err30RE = re.compile(r'xx_err_msg_v3_const')
  
  QSRStrTmpRE = re.compile(r'QSR_string_ptr')
  MSG_v4_re = re.compile(r'msg_v4_')
  qsr_4_re = re.compile(r'qsr_4_0_')  
  
  QSR4AlignedEntryRE = re.compile(r'msg_v4_ss_num_aligned_entries')
  QSR4UnAlignedEntryRE = re.compile(r'msg_v4_ss_num_unaligned_entries')
  QSR4TableEntryRE = re.compile(r'msg_v4_ss_num_table_entries')
  QSR4TblRE = re.compile(r'msg_v4_ss_table')
  QSR4AAMTblRE = re.compile(r'msg_v4_ss_aam_table')
  QSR4UAMTblRE = re.compile(r'msg_v4_ss_uam_table')
  QSR4DBaseGuidEntryRE = re.compile(r'msg_v4_dbase_guid')
  QSR4DBaseNameRE = re.compile(r'msg_v4_dbase_name')  
  QSR4AlignedStartAddrRE = re.compile(r'qsr_4_0_msg_aligned_start_address')  
  QSR4UnAlignedStartAddrRE = re.compile(r'qsr_4_0_msg_unaligned_start_address')  
  QSR4MsgEndAddrRE = re.compile(r'qsr_4_0_msg_end_address')  

  ElfShrinkState.QSR4AlignedEntry = 0
  ElfShrinkState.QSR4UnAlignedEntry = 0 
  ElfShrinkState.QSR4TableEntry = 0
  ElfShrinkState.QSR4Tbl = 0
  ElfShrinkState.QSR4AAMTbl = 0
  ElfShrinkState.QSR4UAMTbl = 0
  ElfShrinkState.QSR4DBaseGuidEntry = 0
  ElfShrinkState.QSR4DBaseNameEntry = 0
  ElfShrinkState.QSR4AlignedStartAddr = 0
  ElfShrinkState.QSR4UnAlignedStartAddr = 0
  ElfShrinkState.QSR4MsgEndAddr = 0
  ElfShrinkState.QSR4MTRACEStart = 0
  ElfShrinkState.QSR4MTRACEEnd = 0 
  ElfShrinkState.QSR4QTRACEStrStart = 0
  ElfShrinkState.QSR4QTRACEStrEnd = 0
  
  
  
  structs = []
  visited_symtab_str = {}  
  ElfShrinkState.errors = []
  if __qsrdebug__:
    print "Scanning Symbol Table for Structs ..."
  
  ElfShrinkState.version = 4
  # iterate through entries in the symbol table (uses a generator) to get the 
  # structure size
  # for entry in symbolTableEntryGenerator(symbolTableData, stringTableData):
    # print "Symbol data  %08x  %08x  %s"%(entry.value,entry.size,entry.name)
  # return
  for entry in symbolTableEntryGenerator(symbolTableData, stringTableData):
       # 'name' : entry.name,
       #'size' : entry.size,
       #'address' : entry.value,
       #'section' : entry.sectionIndex  
  # check for a QSR struct symbol
    if "xx_msg_v4_const" in entry.name and "_Z" in entry.name:
        continue
    if qsrRE.search(entry.name):
      if qsr40RE.search(entry.name):
        # print "Symbol data  %08x  %08x  %s"%(entry.value,entry.size,entry.name)
        if (not (QSR_MSG_START <= entry.value <= QSR_MSG_END) and
            entry.size != 0x0 ):
          #=== anomaly - investigate
          secHead = getSectionHeader(entry.sectionIndex)
          struct = makeQSRobject(entry, secHead);
          anomaly = {}
          anomaly['structName'] = struct['name']
          anomaly['structSection'] = struct['section']
          anomaly['structAddress'] = struct['address']
          anomaly['structOffset'] = struct['offset']
          anomaly['stringAddress'] = strPointer
          ElfShrinkState.errCount += 1
          anomaly['Anomaly'] = "Struct xx_msg_v4_const out of QShrink section "
          ElfShrinkState.errors.append(anomaly)
        else:
           qsr4structFound = 1
      elif qsr20RE.search(entry.name) or qsr30RE.search(entry.name):
        qsr_23 = 1
      
    
    elif errRE.search(entry.name):
      print "xx_err_ Symbol data  %08x  %08x  %s"%(entry.value,entry.size,entry.name)
      if err20RE.search(entry.name) or err30RE.search(entry.name):
        qsr_23 = 1

    elif (ElfShrinkState.image_vend_offset  == 0) and (entry.name == "apps_end_post_qsh"):
      print "image_vend_offset Symbol data  %08x  %08x  %s"%(entry.value,entry.size,entry.name)
      secHead = getSectionHeader(entry.sectionIndex)
      new_struct = makeQSRobject(entry, secHead);
      ElfShrinkState.image_vend_offset = new_struct['elf_offset']     

    elif (ElfShrinkState.use_linker_end_symbol  == 1) and (entry.name == "end"):
      print "linker Symbol data  %08x  %08x  %s"%(entry.value,entry.size,entry.name)
      ElfShrinkState.linker_endAddr = entry.value

    elif MSG_v4_re.search(entry.name):

      if (ElfShrinkState.QSR4AlignedEntry == 0) and (QSR4AlignedEntryRE.match(entry.name)):
        print "QSR4AlignedEntry Symbol data  %08x  %08x  %s"%(entry.value,entry.size,entry.name)
        secHead = getSectionHeader(entry.sectionIndex)
        new_struct = makeQSRobject(entry, secHead);
        ElfShrinkState.QSR4AlignedEntry = new_struct['elf_offset']

      elif (ElfShrinkState.QSR4UnAlignedEntry  == 0) and (QSR4UnAlignedEntryRE.match(entry.name)):      
        print "QSR4UnAlignedEntry Symbol data  %08x  %08x  %s"%(entry.value,entry.size,entry.name)
        secHead = getSectionHeader(entry.sectionIndex)
        new_struct = makeQSRobject(entry, secHead);
        ElfShrinkState.QSR4UnAlignedEntry = new_struct['elf_offset']

      elif (ElfShrinkState.QSR4TableEntry  == 0) and (QSR4TableEntryRE.match(entry.name)):      
        print "QSR4TableEntry Symbol data  %08x  %08x  %s"%(entry.value,entry.size,entry.name)
        secHead = getSectionHeader(entry.sectionIndex)
        new_struct = makeQSRobject(entry, secHead);
        ElfShrinkState.QSR4TableEntry = new_struct['elf_offset']
      
      elif (ElfShrinkState.QSR4Tbl  == 0) and (QSR4TblRE.match(entry.name)):      
        print "QSR4Tbl Symbol data  %08x  %08x  %s"%(entry.value,entry.size,entry.name)
        secHead = getSectionHeader(entry.sectionIndex)
        new_struct = makeQSRobject(entry, secHead);
        ElfShrinkState.QSR4Tbl = new_struct['elf_offset']
        ElfShrinkState.QSR4TblMaxEntry = new_struct['size'] / QSR_DESC_ENTRY_SIZE
    
      elif (ElfShrinkState.QSR4AAMTbl == 0) and (QSR4AAMTblRE.match(entry.name)):      
        print "QSR4AAMTbl Symbol data  %08x  %08x  %s"%(entry.value,entry.size,entry.name)
        secHead = getSectionHeader(entry.sectionIndex)
        new_struct = makeQSRobject(entry, secHead);
        ElfShrinkState.QSR4AAMTbl = new_struct['elf_offset']
        ElfShrinkState.QSR4AAMTblMaxEntry = new_struct['size'] / 4

      elif (ElfShrinkState.QSR4UAMTbl == 0) and (QSR4UAMTblRE.match(entry.name)):     
        print "QSR4UAMTbl Symbol data  %08x  %08x  %s"%(entry.value,entry.size,entry.name)
        secHead = getSectionHeader(entry.sectionIndex)
        new_struct = makeQSRobject(entry, secHead);
        ElfShrinkState.QSR4UAMTbl = new_struct['elf_offset']
        ElfShrinkState.QSR4UAMTblMaxEntry = new_struct['size'] /QSR_UAM_TABLE_ENTRY_SIZE

      elif (ElfShrinkState.QSR4DBaseGuidEntry  == 0) and (QSR4DBaseGuidEntryRE.match(entry.name)):      
        print "QSR4DBaseGuidEntry Symbol data  %08x  %08x  %s"%(entry.value,entry.size,entry.name)
        secHead = getSectionHeader(entry.sectionIndex)
        new_struct = makeQSRobject(entry, secHead);
        ElfShrinkState.QSR4DBaseGuidEntry = new_struct['elf_offset']
      
      elif (ElfShrinkState.QSR4DBaseNameEntry  == 0) and (QSR4DBaseNameRE.match(entry.name)):      
        print "QSR4DBaseNameEntry Symbol data  %08x  %08x  %s"%(entry.value,entry.size,entry.name)
        secHead = getSectionHeader(entry.sectionIndex)
        new_struct = makeQSRobject(entry, secHead);
        ElfShrinkState.QSR4DBaseNameEntry = new_struct['elf_offset']



    elif qsr_4_re.search(entry.name):
      if (ElfShrinkState.QSR4AlignedStartAddr  == 0) and (QSR4AlignedStartAddrRE.match(entry.name)):      
        print "QSR4AlignedStartAddr Symbol data  %08x  %08x  %s"%(entry.value,entry.size,entry.name)
        secHead = getSectionHeader(entry.sectionIndex)
        new_struct = makeQSRobject(entry, secHead);
        ElfShrinkState.QSR4AlignedStartAddr = new_struct['elf_offset']
 
      elif (ElfShrinkState.QSR4UnAlignedStartAddr  == 0) and (QSR4UnAlignedStartAddrRE.match(entry.name)):      
        print "QSR4UnAlignedStartAddr Symbol data  %08x  %08x  %s"%(entry.value,entry.size,entry.name)
        secHead = getSectionHeader(entry.sectionIndex)
        new_struct = makeQSRobject(entry, secHead);
        ElfShrinkState.QSR4UnAlignedStartAddr = new_struct['elf_offset']
 
      elif (ElfShrinkState.QSR4MsgEndAddr == 0) and (QSR4MsgEndAddrRE.match(entry.name)):      
        print "QSR4MsgEndAddr Symbol data  %08x  %08x  %s"%(entry.value,entry.size,entry.name)
        secHead = getSectionHeader(entry.sectionIndex)
        new_struct = makeQSRobject(entry, secHead);
        ElfShrinkState.QSR4MsgEndAddr = new_struct['elf_offset']
        
        
        
    else:
      continue

  #=== end anomaly investigation
  # end for symbol table iterator

  if (ElfShrinkState.qdbFile == ""):
        print "Error : QSR4 QDB file not found"
        ElfShrinkState.outputLog.write("".join(["\n","QShrink failed ! QSR 4 QDB File not found !","\n"]))
        exit(1)

  if (qsr4structFound):
    if ((ElfShrinkState.QSR4AlignedEntry == 0) or (ElfShrinkState.QSR4UnAlignedEntry == 0)
       or (ElfShrinkState.QSR4TableEntry == 0) or (ElfShrinkState.QSR4Tbl == 0)
      or (ElfShrinkState.QSR4AAMTbl == 0) or (ElfShrinkState.QSR4UAMTbl == 0) 
      or (ElfShrinkState.QSR4DBaseGuidEntry == 0)
      or (ElfShrinkState.QSR4DBaseNameEntry == 0)
      ):
        print "Error : QSR 4 Descriptors not found"
        ElfShrinkState.outputLog.write("".join(["\n","QShrink failed ! QSR 4 Descriptors not found !","\n"]))
        exit(1)

    if ((ElfShrinkState.QSR4AlignedStartAddr == 0) or (ElfShrinkState.QSR4UnAlignedStartAddr == 0)
      or (ElfShrinkState.QSR4MsgEndAddr == 0) ):
        print "Error : QSR 4 structures not found"
        ElfShrinkState.outputLog.write("".join(["\n","QShrink failed ! QSR 4 structures not found !","\n"]))
        exit(1)

  # Get the values of addresses 
    try: 
    
    
      input = open(ElfShrinkState.path, 'rb')
    except IOError:
      print "Error, Could not open input Elf File, ", ElfShrinkState.path
      exit(1)
    else:
      
      input.seek(ElfShrinkState.QSR4AlignedStartAddr)  
      bytes = input.read(const['Elf32_Addr'])
      ElfShrinkState.QSR4AlignedStartAddr = convertFromElf('Elf32_Word',bytes)
      

      
      
      input.seek(ElfShrinkState.QSR4UnAlignedStartAddr)
      bytes = input.read(const['Elf32_Addr'])
      ElfShrinkState.QSR4UnAlignedStartAddr = convertFromElf('Elf32_Word',bytes)
      

      
      input.seek(ElfShrinkState.QSR4MsgEndAddr)  
      bytes = input.read(const['Elf32_Addr'])
      ElfShrinkState.QSR4MsgEndAddr = convertFromElf('Elf32_Word',bytes)
      

      input.close()
    
    if __qsrdebug__:
      ElfShrinkState.outputLog.write("".join(["QSR4 Aligned Start Address - ", 
                hex(ElfShrinkState.QSR4AlignedStartAddr),"\n"]))
      ElfShrinkState.outputLog.write("".join(["QSR4 UnAligned Start Address - ", 
                hex(ElfShrinkState.QSR4UnAlignedStartAddr),"\n"]))
      ElfShrinkState.outputLog.write("".join(["QSR4 MSG End Address - ", 
                hex(ElfShrinkState.QSR4MsgEndAddr),"\n"]))

  return qsr_23

#=========================================================================
# Function : processV4Structs
#
# Arguments
#
# Description
#   Iterates through symbol table entries, finding MSG V2 structs.
#   Then it goes through the structs hashify-ing or moving their strings.
#
# Return Values:
#  List of structs
#=========================================================================
def processV4Structs():

  global ElfShrinkState
  global QSR_MSG_START, QSR_MSG_SIZE, QSR_MSG_END, QSR_MSG_OFFSET
  global QSR_MSG_STR_START, QSR_MSG_STR_SIZE, QSR_MSG_STR_END, QSR_MSG_STR_OFFSET
  print "QSR_MSG_STR_START ",QSR_MSG_STR_START

  ElfShrinkState.QSR4_STRINGlist = []      
  ElfShrinkState.Qsr4DescStructs = []

  ElfShrinkState.qsr4_desc_table_aligned_entries = 0
  ElfShrinkState.qsr4_desc_table_unaligned_entries = 0
      
#Read QSR_4_0_MSG section and create hash table and DIAG run time filtering tables
  msg_offset = 0 
  msg_end_offset = ElfShrinkState.QSR4MsgEndAddr - QSR_MSG_START
  print "QSR_MSG_START %08x"%(QSR_MSG_START)
  print "ElfShrinkState.QSR4MsgEndAddr %08x "%(ElfShrinkState.QSR4MsgEndAddr)
  print "msg_end_offset %08x"%(msg_end_offset)
  msg_unaligned_offset = ElfShrinkState.QSR4UnAlignedStartAddr - QSR_MSG_START
  print "msg_unaligned_offset %08x"%(msg_unaligned_offset)
  cur_id = 0
  cur_mask = 0
  cur_cnt = 0
  error_mapping = 0
  if __qsrdebug__:
    ElfShrinkState.outputLog.write("".join(["\n","QSR4 SSID-Mask combinations with more than 2k entries","\n",
                                   "SSID : SS_MASK : COUNT : AAM Table Entry Count ","\n"
                                  ]))  
  while msg_offset < msg_end_offset:
    hash = msg_offset
    # print "hash %08x"%(hash)
    hashval = hash>>3
    msg_mask = convertFromElf('Elf32_Word',
                      ElfShrinkState.bytesDict['QSR_4_0_MSG'][
                          msg_offset : msg_offset +
                          const['Elf32_Word']])
    msg_offset += const['Elf32_Word']
    msg_qsr_str = convertFromElf('Elf32_Word',
                      ElfShrinkState.bytesDict['QSR_4_0_MSG'][
                          msg_offset : msg_offset +
                          const['Elf32_Word']]) 
    # print "msg_mask %08x  msg_offset %08x  msg_qsr_str %08x "%(msg_mask,msg_offset,msg_qsr_str)
    msg_offset += const['Elf32_Word']
    # Filler data - 0xaaaaaaaa
    if ((msg_mask != 0xaaaaaaaa) or (msg_qsr_str != 0xaaaaaaaa)):
    # valid struct      
      strPointerOffset = msg_qsr_str - QSR_MSG_STR_START
      # print "strPointerOffset %08x" %(strPointerOffset)
      string = getString(ElfShrinkState.bytesDict['QSR_4_0_MSG_STR'], strPointerOffset)
      string = re.sub('\x0d', r'', string)
      string = re.sub(r"\\", r" ", string)      
      # replace new lines (all unix (0x0a) internally) with special chars to 
      # avoid partial strings in future Master Hash load
      string = re.sub('\x0a', r'\\n', string)
      hashstring = "".join([
                      str(hashval),
                      ":",
                      str(msg_mask),
                      ":",
                      string, "\n"])
      # print "hashstring :- ",hashstring
      ElfShrinkState.QSR4_STRINGlist.append(hashstring)
      ElfShrinkState.matches += 1

      ssid = int(re.split(':',string)[0])      
      if __qsrdebug__:
        if (cur_id != ssid) or (cur_mask != msg_mask):          
          if (cur_cnt > 1024):
            ElfShrinkState.outputLog.write(" ".join([str(cur_id),":", 
                      str(cur_mask),":",
                      str(cur_cnt),":",
                      str(cur_cnt/1024+1),
                                "\n"]))  
          cur_cnt = 1
        else:
          cur_cnt += 1
      
      # Make run-time filtering table for addresses - SSID table, Address table
      if hash < msg_unaligned_offset:
        # if hash % 8192 == 0:
          ssid = int(re.split(':',string)[0])
          descstruct = makeQSR4DescObject(ElfShrinkState.qsr4_desc_table_aligned_entries,0,ssid,msg_mask)
          ElfShrinkState.Qsr4DescStructs.append(descstruct)
          ElfShrinkState.qsr4_desc_table_aligned_entries +=1
          if (cur_id != ssid) or (cur_mask != msg_mask):          
            cur_id = ssid
            cur_mask = msg_mask
      else:
        # Add to address table and SSID table
        descstruct = makeQSR4DescObject(ElfShrinkState.qsr4_desc_table_aligned_entries+ElfShrinkState.qsr4_desc_table_unaligned_entries,
                           QSR_MSG_START + hash,ssid,msg_mask)
        ElfShrinkState.Qsr4DescStructs.append(descstruct)
        ElfShrinkState.qsr4_desc_table_unaligned_entries += 1
        cur_id = ssid
        cur_mask = msg_mask

  if (error_mapping == 1):
      print "QShrink failed !QSR4 Messages not mapped correctly in section Check definition !"
      ElfShrinkState.outputLog.write("".join(["\n","QShrink failed !QSR4 Messages not mapped correctly in section. Please check logs!", "\n"]))
      exit(1)                     

 
 #got all the strings
 #Get Database GUID
  m = hashlib.sha1()
  m.update("".join(ElfShrinkState.QSR4_STRINGlist))
  x = m.digest()[-16:]
  ElfShrinkState.dBaseGUID = uuid.UUID(bytes=x)

  ElfShrinkState.bytesDict.clear() #release memory  
  
#=========================================================================
# Function : processQSR4MsgDescTable 
#
# Arguments
#
# Description
# Create the Msg Description Indexed Table with ssHash, SSID, SS_MASK
# sorted on SSID
#
# Return Values:
#=========================================================================
def processQSR4MsgDescTable():

  global REPLACEMENT_MODE, ElfShrinkState

  ElfShrinkState.Qsr4DescStructs.sort(cmp=lambda x, y: cmp(x['ss_id'], y['ss_id']))

  # all symbol names
  ssidIndexDict = dict()

  ElfShrinkState.qsr4_desc_table = []
  ElfShrinkState.qsr4_desc_table_total_entries = 0
  
  ElfShrinkState.qsr4_aam_table = []
  ElfShrinkState.qsr4_uam_table = []

  ElfShrinkState.outputLog.write("processQSR4MsgDescTable: qsr4_desc_table_aligned_entries %d\n" %(ElfShrinkState.qsr4_desc_table_aligned_entries))

  if (ElfShrinkState.qsr4_desc_table_aligned_entries > ElfShrinkState.QSR4AAMTblMaxEntry):
    print "Error : QSR4 MSG_V4_SS_AAM_Table Overflow , Build has more entries than MSG_V4_SS_AAM_Table"
    ElfShrinkState.outputLog.write("\n".join(["QShrink failed ! QSR4 MSG_V4_SS_AAM_Table Overflow !"
                                                                     ,str(ElfShrinkState.qsr4_desc_table_aligned_entries),
                                                                     " : MSG_V4_SS_AAM_Table Entries:",str(ElfShrinkState.QSR4AAMTblMaxEntry),
                                                                     "Please contact QShrink team",
                                                                      "\n"]))
    exit(1)
  
  ElfShrinkState.outputLog.write("processQSR4MsgDescTable: qsr4_desc_table_unaligned_entries %d\n" %(ElfShrinkState.qsr4_desc_table_unaligned_entries))
  
  if (ElfShrinkState.qsr4_desc_table_unaligned_entries > ElfShrinkState.QSR4UAMTblMaxEntry):
    print "Error : QSR4 MSG_V4_SS_UAM_Table Overflow , Build has more entries than what was mapped in QShrink Linker script. Please check qshrink logs for instructions to proceed with the build."
    ElfShrinkState.outputLog.write("".join(["\n","QShrink failed ! QSR4 MSG_V4_SS_UAM_Table Overflow  ! Please contact QShrink team",
    "\n","Entries not mapped in linker script for the build :",
    str(ElfShrinkState.qsr4_desc_table_unaligned_entries),"\n","Total MSG_V4_SS_UAM_Table Entries allowed : ",str(ElfShrinkState.QSR4UAMTblMaxEntry),
    "\n","Please use instructions below to proceed with build.","\n\n"]))

    exit(1)
  

  indexcount = 0
  if __qsrdebug__:
    ElfShrinkState.outputLog.write("".join(["\n","\n","QSR MSG V4 TABLE entries", "\n",
                                          "Table Index - SS_ID : MASK","\n"]))
  
  for descstruct in ElfShrinkState.Qsr4DescStructs:
  # loop over structs to make unique qsr4_desc_table entries
    ssidstring = "".join([
                 str(descstruct['ss_id']),
                 ":",
                 str(descstruct['ss_mask'])])   
    if ssidIndexDict.has_key(ssidstring):
      index = ssidIndexDict[ssidstring]
    else:   
      #rt_mask
      ElfShrinkState.qsr4_desc_table.append(convertToElf('Elf32_Half', 0))
      ElfShrinkState.qsr4_desc_table.append(convertToElf('Elf32_Half', descstruct['ss_id']))
      ElfShrinkState.qsr4_desc_table.append(convertToElf('Elf32_Word', descstruct['ss_mask']))
      ssidIndexDict[ssidstring] = ElfShrinkState.qsr4_desc_table_total_entries
      if __qsrdebug__:
        ElfShrinkState.outputLog.write("".join([str(ElfShrinkState.qsr4_desc_table_total_entries)," - ",
                                            str(descstruct['ss_id'])," : ",
                                            hex(descstruct['ss_mask']),"\n"]))
      ElfShrinkState.qsr4_desc_table_total_entries+=1


  ElfShrinkState.Qsr4DescStructs.sort(cmp=lambda x, y: cmp(x['aam_uam_index'], y['aam_uam_index']))

  if __qsrdebug__:
    ElfShrinkState.outputLog.write("".join(["\n", "\n", "QSR4 AAM Table Entries", "\n",
                                         "AAM TableIndex - MSG V4 Table Index (Content) - SS_ID:MASK","\n"]))

  aam_index = 0
  uam_start = 0
  for descstruct in ElfShrinkState.Qsr4DescStructs:  
    ssidstring = "".join([
                 str(descstruct['ss_id']),
                 ":",
                 str(descstruct['ss_mask'])])   
    index = ssidIndexDict[ssidstring]
    if aam_index < ElfShrinkState.qsr4_desc_table_aligned_entries:
      #AAM entry
      ElfShrinkState.qsr4_aam_table.append(convertToElf('Elf32_Word', index))
      ElfShrinkState.outputLog.write("".join([str(aam_index)," - index ",str(index)," - ss_id ",str(descstruct['ss_id'])," : ss_mask",
                                           hex(descstruct['ss_mask']),"\n"]))
      #print "#AAM entry "+"".join([str(aam_index)," - index ",str(index)," - ss_id ",str(descstruct['ss_id'])," : ss_mask ",hex(descstruct['ss_mask']),"\n"])
      aam_index += 1
      
    else:
      if uam_start ==0:
        ElfShrinkState.outputLog.write("".join(["\n", "\n", "QSR4 UAM Table Entries", "\n",
                                         "UAM Table Start Address - MSG V4 Table Index (Content) - SS_ID:MASK","\n"]))
        uam_start = 1
    
      #UAM entry

      ElfShrinkState.qsr4_uam_table.append(convertToElf('Elf32_Half', index))
      ElfShrinkState.outputLog.write( "#UAM entry "+"".join(["index ",str(index)," - ss_id ",str(descstruct['ss_id'])," : ss_mask ",hex(descstruct['ss_mask']),"\n"]))

  if (ElfShrinkState.qsr4_desc_table_total_entries > ElfShrinkState.QSR4TblMaxEntry):
    print "Error : QSR4 MSG_V4_SS_Table Overflow , Build has more entries than MSG_V4_SS_Table"
    ElfShrinkState.outputLog.write("".join(["\n","QShrink failed ! QSR4 MSG_V4_SS_Table Overflow  !","\n"]))
    exit(1)

  
  ElfShrinkState.outputLog.write("".join(["\n","QSR4 MSG_V4_SS_Table - Total Count-",
                                 str(ElfShrinkState.qsr4_desc_table_total_entries),"\n",
                                 "QSR4 MSG_V4_SS_Table - Max Count ",
                                 str(ElfShrinkState.QSR4TblMaxEntry),
                                 "\n"]))
  ElfShrinkState.outputLog.write("".join(["QSR4 MSG_V4_SS_AAM_Table - Total Count-",
                                 str(ElfShrinkState.qsr4_desc_table_aligned_entries),"\n",
                                 "QSR4 MSG_V4_SS_AAM_Table - Max Count ",
                                 str(ElfShrinkState.QSR4AAMTblMaxEntry),"\n",
                                 "QSR4 MSG_V4_SS_UAM_Table - Total Count-",
                                 str(ElfShrinkState.qsr4_desc_table_unaligned_entries),"\n",
                                 "QSR4 MSG_V4_SS_UAM_Table - Max Count ",
                                 str(ElfShrinkState.QSR4UAMTblMaxEntry),"\n",
                                 "\n"]))
      
  del ssidIndexDict
  del ElfShrinkState.Qsr4DescStructs

#=========================================================================
# Function : qsr4EmptyCreateHashFile
#
# Arguments
#
# Description
#   Only for temporary intermediate output
#
# Return Values:
#  List of structs
#=========================================================================
def qsr4EmptyCreateHashFile():
   global ElfShrinkState
    
   ElfShrinkState.QSR4_STRINGlist = []
   
   hashstring = "".join([
                str(1),
                ":",
                str(1),
                ":",
                ElfShrinkState.qdbName, "\n"])
   ElfShrinkState.QSR4_STRINGlist.append(hashstring)
   m = hashlib.sha1()
   m.update("".join(ElfShrinkState.QSR4_STRINGlist))
   x = m.digest()[-16:]
   ElfShrinkState.dBaseGUID = uuid.UUID(bytes=x)
   
   qsr4CreateHashFile() 

#=========================================================================
# Function : qsr4CreateHashFile
#
# Arguments
#
# Description
#   Iterates through symbol table entries, finding MSG V2 structs.
#   Then it goes through the structs hashify-ing or moving their strings.
#
# Return Values:
#  List of structs
#=========================================================================
def qsr4CreateHashFile():

  global ElfShrinkState
    
 #write Hash File
  Hashfile_path = ElfShrinkState.buildOutputPath  
  Hashfile_temp = "".join(["msg_hash_",                   
                           str(ElfShrinkState.dBaseGUID),
                           ".qsr4"])  
  input_Path,inputFileName = os.path.split(ElfShrinkState.path)
  inputFileName = re.split('.elf',inputFileName)[0] 
  qdb_meta_name = "".join([
                           ElfShrinkState.qdbName,   
                           "_",
                           inputFileName,
                           ".txt"])  
  qdb_meta = os.path.join(Hashfile_path,qdb_meta_name)
  if os.path.exists(qdb_meta) :
     # get the msg_hash file name to clean it.
     qdb_meta_file = open(qdb_meta, 'rb')
     hashpath= qdb_meta_file.read()
     qdb_meta_file.close()
     os.remove(qdb_meta)
     for olddbase_f in os.listdir(Hashfile_path):
        if olddbase_f.endswith(hashpath):
          os.remove(os.path.join(Hashfile_path,olddbase_f))

    
  Hashfile_name = os.path.join(Hashfile_path,Hashfile_temp)  
  Hashfile_zip_Path,Hashfile_zipname = os.path.split(ElfShrinkState.qdbFile)
  Hashfile_zip = ElfShrinkState.qdbFile

  contentStr = [ \
    " ".join(["##################################################################","\n"]),
    " ".join(["# Qshrink 4.0 Hash Database File ","\n"]),
    " ".join(["##################################################################","\n"]),
    " ".join(["# GENERAL DESCRIPTION            ","\n"]),
    " ".join(["#                                ","\n"]),
    " ".join(["# Copyright (c) 2015 - 2020 by Qualcomm Technologies Inc. All Rights Reserved.","\n"]),
    " ".join(["# Export of this technology or software is regulated by the U.S. Government. ","\n"]),  
    " ".join(["# Diversion contrary to U.S. law prohibited. ","\n"]),  
    " ".join(["#                                ","\n"]),
    " ".join(["# Hash File:", Hashfile_name,"\n"]),
    " ".join(["# Hash File Format : <hash>:<ss_mask>:<ssid>:<line>:<file>:<string>    ","\n"]),
    " ".join(["# Qtrace Format : <hash>:<line>:<level>:<client>:<file>:<tag>:<string>    ","\n"]),
    " ".join(["#                                ","\n"]),
    " ".join(["##################################################################","\n"]),
    " ".join(["<GUID>",str(ElfShrinkState.dBaseGUID),"<\GUID>","\n"]),
    " ".join(["<Version>","00.10.00","<\Version>","\n"]),
    " ".join(["<Content>","\n"]),
    "".join(ElfShrinkState.QSR4_STRINGlist),
    " ".join(["<\Content>","\n"])    
  ]

  try:
    Hashfile = open(Hashfile_name, 'w')
  except IOError:
    print "Error: Could not open Hashfile, ",Hashfile_name
    exit(1)
  else:
    for strn in contentStr:
      Hashfile.write(strn)
    Hashfile.close()
    qdb_meta_file = open(qdb_meta, 'w')
    qdb_meta_file.write(Hashfile_temp)
    qdb_meta_file.close()
  
  try:
    Hashzip = open(Hashfile_zip, 'wb')
    Hashfile = open(Hashfile_name, 'rb')
  except IOError:
    print "Error: Could not open Hashfile, ",Hashfile_zip
    exit(1)
  else:
    # First 64 bytes File Header 
    FileHdrStr = [ \
      "".join(['\x7f', "QDB"]),
      "".join(ElfShrinkState.dBaseGUID.bytes),   
      "".join(['\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00']),
      "".join(['\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00']),
      "".join(['\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00']),
      "".join(['\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00'])
    ]
    for strn in FileHdrStr:
      Hashzip.write(strn)
    str1 = Hashfile.read()
    str2 = zlib.compress(str1, 9)
    Hashzip.write(str2)
    Hashzip.close()

# test decompress
#  f = open(Hashfile_zip, 'rb') # BINARY mode
#  f.seek(64)
#  s1=f.read()  
#  # (2) reverse the compression
#  s2 = zlib.decompress(s1)
#  s2 = s2.replace('\r\n', '\n')
#  # (3) reverse the text mode read
#  f = open("uncompressed_file.txt"+ '-fixed', 'wb') # TEXT mode
#  f.write(s2)
#  f.close()

#=========================================================================
# Function : qsr4UpdateVariables
#
# Arguments
#
# Description
#   Updates QSR4 Variables in Elf
#
# Return Values:
#  List of structs
#=========================================================================
def qsr4UpdateVariables():

  global ElfShrinkState
  global QSR_MSG_START, QSR_MSG_SIZE, QSR_MSG_END, QSR_MSG_OFFSET
  
  
  output = open(ElfShrinkState.outputPath, 'r+b')
  
  #QSR4AlignedEntry
  output.seek(ElfShrinkState.QSR4AlignedEntry)
  output.write(convertToElf('Elf32_Word',ElfShrinkState.qsr4_desc_table_aligned_entries))
  ElfShrinkState.outputLog.write(" ".join(['QSR4AlignedEntry changed in elf_offset ', 
              hex(ElfShrinkState.QSR4AlignedEntry), "to ", str(ElfShrinkState.qsr4_desc_table_aligned_entries),"\n"]))
 

  #QSR4UnAlignedEntry
  output.seek(ElfShrinkState.QSR4UnAlignedEntry)
  output.write(convertToElf('Elf32_Word',ElfShrinkState.qsr4_desc_table_unaligned_entries))
  ElfShrinkState.outputLog.write(" ".join(['QSR4UnAlignedEntry changed in elf_offset ', 
              hex(ElfShrinkState.QSR4UnAlignedEntry), "to ", str(ElfShrinkState.qsr4_desc_table_unaligned_entries),"\n"]))
 
  #QSR4TableEntry  
  output.seek(ElfShrinkState.QSR4TableEntry)
  output.write(convertToElf('Elf32_Word',ElfShrinkState.qsr4_desc_table_total_entries))
  ElfShrinkState.outputLog.write(" ".join(['QSR4TableEntry changed in elf_offset ', 
              hex(ElfShrinkState.QSR4TableEntry), "to ", str(ElfShrinkState.qsr4_desc_table_total_entries),"\n"]))
 

  #QSR4Tbl    
  output.seek(ElfShrinkState.QSR4Tbl)
  output.write("".join(ElfShrinkState.qsr4_desc_table))
  ElfShrinkState.outputLog.write(" ".join(['QSR4Tbl changed in elf_offset ', 
              hex(ElfShrinkState.QSR4Tbl),"\n"]))
 
  
  #QSR4AAMTbl    
  output.seek(ElfShrinkState.QSR4AAMTbl)
  output.write("".join(ElfShrinkState.qsr4_aam_table))
  ElfShrinkState.outputLog.write(" ".join(['QSR4AAMTbl changed in elf_offset ', 
              hex(ElfShrinkState.QSR4AAMTbl),"\n"]))
 

  #QSR4UAMTbl    
  output.seek(ElfShrinkState.QSR4UAMTbl)
  output.write("".join(ElfShrinkState.qsr4_uam_table))
  ElfShrinkState.outputLog.write(" ".join(['QSR4UAMTbl changed in elf_offset ', 
              hex(ElfShrinkState.QSR4UAMTbl),"\n"]))
 
  #QSR4DBaseNameEntry 
  output.seek(ElfShrinkState.QSR4DBaseNameEntry)
  if not (re.search("sdx50m",ElfShrinkState.path,re.IGNORECASE)):
     output.write("".join(["/readonly/firmware/image/",ElfShrinkState.qdbName,".qdb","\0"]))
     ElfShrinkState.outputLog.write("".join(['QSR4DBaseNameEntry changed in elf_offset ', 
                hex(ElfShrinkState.QSR4DBaseNameEntry), 
                " to ","/readonly/firmware/image/",ElfShrinkState.qdbName,".qdb","\n"]))
  else:
     output.write("".join(["/readonly/firmware/image/sdx50m/",ElfShrinkState.qdbName,".qdb","\0"]))
     ElfShrinkState.outputLog.write("".join(['QSR4DBaseNameEntry changed in elf_offset ', 
                hex(ElfShrinkState.QSR4DBaseNameEntry), 
                " to ","/readonly/firmware/image/sdx50m/",ElfShrinkState.qdbName,".qdb","\n"]))

  #QSR4DBaseGuidEntry
  output.seek(ElfShrinkState.QSR4DBaseGuidEntry)
  output.write("".join(ElfShrinkState.dBaseGUID.bytes_le))
  ElfShrinkState.outputLog.write(" ".join(['QSR4DBaseGuidEntry changed in elf_offset ', 
               hex(ElfShrinkState.QSR4DBaseGuidEntry), "to "]))
  output.seek(ElfShrinkState.QSR4DBaseGuidEntry)
  i = 0
  while (i < 8):
    byte = output.read(2)
    ElfShrinkState.outputLog.write("".join([
                  hex(convertFromElf('Elf32_Half', byte, endian=False)),
                  ","]))
    i += 1

  if ElfShrinkState.image_vend_offset != 0:
    #Image End
    output.seek(ElfShrinkState.image_vend_offset)  
    output.write(convertToElf('Elf32_Word', ElfShrinkState.linker_endAddr));  
    ElfShrinkState.outputLog.write(" ".join(["\n",'Linker generated IMAGE END Addr changed in elf_offset ', 
            hex(ElfShrinkState.image_vend_offset),'to',
             hex(ElfShrinkState.linker_endAddr), "\n"]))

    
  
  output.flush()
  ElfShrinkState.outputLog.write("".join(["\n","Update variables complete","\n"]))
  output.close()

if __name__ == '__main__':
  # debugging run
  # typical use should use Qshrink.py
  ElfShrinkState.anomalyCount = 0

  ElfShrinkState.outputPath = "SCTUUS.elf.temp"
  ElfShrinkState.hashPath = "hash.txt"
  ElfShrinkState.logPath = "log.log"
  ElfShrinkState.anomalyListPath = "anom.log"

  initializeWithElfHeader()
  # get the symbol table and the string table to find symbol names
  symbolTableData = getSectionData('.symtab')
  # all symbol data
  stringTableData = getSectionData('.strtab')
  print "Finished"
  
  
