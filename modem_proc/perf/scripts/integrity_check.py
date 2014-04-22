##  @file       integrity_check.py.
#   @author     raholr.
#   @brief      This script calculates the checksums for the RO_Compressed Region.
#   @version    1.0.
#===============================================================================
# Copyright (c) 2014 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# Qualcomm Technologies Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when      who     ver     what, where, why
# --------  ------  ----    ----------------------------------------------------
# 07/26/16  raholr  2.1     Remove FW crashdump section from elf if not overlaid
# 06/15/16  raholr  2.0     Adding Support for CLADE, update symbols in
#                           crash_hook under shutdown, multi-region support
#                           for integrity check
# 06/30/15  raholr  1.0     Created the script
# 
#-------------------------------------------------------------------------------

import sys
import re
import os
import inspect

file_path = inspect.getframeinfo(inspect.currentframe()).filename
file_dir, file_name = os.path.split(file_path)

sz_bit = 14
block_size = 1 << sz_bit
max_checksum_blocks = 1 << (sz_bit -2)
max_size = max_checksum_blocks << sz_bit
checksum_size = 4

#sections to run integrity_check over
integrity_chunks = [".clade.comp", ".candidate_compress_section"]

#sections integrity check will not iterate over, but we need this for finding the start and end addresses
#of the region we will mark as RW from RX during a fatal crash
rx_tlb_candidates = ["ro_fatal", ".rw_candidate_compress_section"]

fw_overlay_sh_name = "overlay_mem_dump"
# import elfManipualtor {{{
elfManipulator_path = os.path.abspath(
  os.path.join(
    file_dir,
    'elfManipulator',
    'include'))

sys.path.insert(0, elfManipulator_path)

import elfUtils as utils
import elfConstants as const
import elfFileClass as elfFileClass
import dynRec_manip as editor

##
# @brief    calculate xor checksums for specified address range and store in specified location
# @param    elf          The elfFileClass object to read from
# @param    start      The starting point to read from
# @param    end        end point
# @pre      returns pointer to next location in checksum array
def checksum_xor(elf_handle, start, end, write_ptr):
  count = 0
  temp = editor.readDataByAddress(elf_handle, start, 4)
  print temp
  while start < end:
    step_end = start + block_size
    if step_end > end:
	  step_end = end;
    block_xor = 0
    while start < step_end:
      temp = editor.readDataByAddress(elf_handle, start, 4)
      block_xor = block_xor ^ temp
      start = start + 4
    editor.setDataByAddress(elf_handle, write_ptr, 4, block_xor)
    write_ptr = write_ptr + 4
    count = count + 1
  return write_ptr
  

##
# @brief    calculate fletcher's checksums for specified address range and store in specified location
# @param    elf          The elfFileClass object to read from
# @param    start      The starting point to read from
# @param    end        end point
# @pre      returns pointer to next location in checksum array  
def checksum_fletcher(elf_handle, start, end, write_ptr):
  count = 0
  temp = editor.readDataByAddress(elf_handle, start, 4)
  while start < end:
   step_end = start + block_size
   if step_end > end:
     step_end = end
   sa = 0
   sb = 0
   while start < step_end:
     temp = editor.readDataByAddress(elf_handle, start, 4)
     sa = sa + temp
     sb = sb + sa
     start = start + 4
   sf = (sb ^ sa) % 4294967296
   editor.setDataByAddress(elf_handle, write_ptr, 4, sf)
   write_ptr = write_ptr + 4
   count = count + 1 
  return write_ptr  
  


def main(): 
  Dict = {}
  retcode = 0
  section_list = integrity_chunks + rx_tlb_candidates
  start_va_rx = []
  end_va_rx = []
  clade_start_va_rx = []
  clade_end_va_rx = []
  elf_handle = elfFileClass.elfFile(sys.argv[1])
  checksum_array = elf_handle.getSymbolByName("perf_integritycheck_checksum_buf")
  if checksum_array == const.RC_ERROR :
    print " "*3 + "-> ! Cannot find \'perf_integritycheck_checksum_buf'\, integritycheck disabled"
    sys.exit(1)
  begin_array = elf_handle.getSymbolByName("perf_integritycheck_compress_section_begin")
  if begin_array == const.RC_ERROR:
    print " "*3 + "-> ! Cannot find \'perf_integritycheck_compress_section_begin'\, integritycheck disabled"
    sys.exit(1)

  sz_array = elf_handle.getSymbolByName("perf_integritycheck_compress_section_size")
  if sz_array == const.RC_ERROR:
    print " "*3 + "-> ! Cannot find \'perf_integritycheck_compress_section_size'\, integritycheck disabled"
    sys.exit(1)

  start_ptr = checksum_array.st_value
  begin_arr_addr = begin_array.st_value
  sz_arr_addr = sz_array.st_value
  count = 0
  total_size = 0
  already_logged = 0
  for j in section_list:
    sh = elf_handle.getSectionByName(j)
    if sh == const.RC_ERROR:
      continue
    start_va_rx.append(sh.sh_addr)
    end_va_rx.append(sh.sh_addr + sh.sh_size)
    if j in integrity_chunks:
      total_size += sh.sh_size
      clade_start_va_rx.append(sh.sh_addr)
      clade_end_va_rx.append(sh.sh_addr + sh.sh_size)
    else:
      continue
    if total_size > max_size:
      if already_logged:
        print " "*3 + "-> ! Warning: By adding %s compress size is greater than max size! Running Integrity Check for first %d section" %(sh.sh_nameStr, count)
        retcode = 2
      already_logged = 1
    else:
      start_ptr = checksum_fletcher(elf_handle, sh.sh_addr, sh.sh_addr + sh.sh_size, start_ptr)
      count += 1
      editor.setDataByAddress(elf_handle, begin_arr_addr, 4, sh.sh_addr)
      editor.setDataByAddress(elf_handle, sz_arr_addr, 4, sh.sh_size)
      begin_arr_addr += 4
      sz_arr_addr += 4
  
##
# If the FW crashdump section is not overlaid, then remove the section
# here. Workaround for qurt image builder and CLADE compatibility. This 
# removes section if it is of type PROG_BITS

  fw_overlay_sh = elf_handle.getSectionByName(fw_overlay_sh_name)
  if fw_overlay_sh != const.RC_ERROR:
    Dict["start_va_map_fw_dump_overlay"] = fw_overlay_sh.sh_addr
    Dict["end_va_map_fw_dump_overlay"] = fw_overlay_sh.sh_addr + 0xc0000
    elf_handle.removeSectionByName(fw_overlay_sh_name)
  else:
     print "Overlay mem dump section not found\n"
     elf_handle.writeOutELF(sys.argv[2])
     sys.exit(retcode)
  Dict["perf_integritycheck_num_sections"] = count
  Dict["start_va_enable_fw_dump_overlay"] = min(start_va_rx)
  Dict["end_va_enable_fw_dump_overlay"] = max(end_va_rx)
  print "jsumit start_va_map_fw_dump_overlay :   ",Dict["start_va_map_fw_dump_overlay"]
  print "jsumit end_va_map_fw_dump_overlay :   ",Dict["end_va_map_fw_dump_overlay"]
  print "jsumit start_va_enable_fw_dump_overlay :   ",Dict["start_va_enable_fw_dump_overlay"]
  print "jsumit end_va_enable_fw_dump_overlay :   ",Dict["end_va_enable_fw_dump_overlay"]
  elf_handle.updateSymbolValuesByDict(Dict)
  elf_handle.writeOutELF(sys.argv[2])
  sys.exit(retcode)
  
  
  
  
if __name__ == '__main__':
  main()

