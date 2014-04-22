#===============================================================================
#
# Dynamic Loading
#
# GENERAL DESCRIPTION
#    Contains all MBN Utilities for image generation 
#
# Copyright 2014 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/core.tx/6.0/bsp/build/scripts/pad_file_builder.py#1 $
#  $DateTime: 2020/01/30 22:49:35 $
#  $Author: pwbldsvc $
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 12/15/14   amitkulk  Created
#===============================================================================

import os
import string
import SCons.Scanner
from SCons.Script import *

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('pad_file_builder')

#-------------------------------------------------------------------------------
# Generate builder
#-------------------------------------------------------------------------------
def generate(env):
   rootenv = env.get('IMAGE_ENV')  
   pad_file_generate(env)
   return
   
def pad_file_generate(env):
   pad_file_c_act = env.GetBuilderAction(generate_pad_files)
   pad_file_c_bld = env.Builder(action = pad_file_c_act)
   env.Append(BUILDERS = {'AddDynamicPadFiles' : pad_file_c_bld})
   return

#------------------------------------------------------------------------------
# Globals Templates(Read Only)
# ToDo: This needs a clean up, should generate a header file with this template
#------------------------------------------------------------------------------
generic_header = "#define DUMMY_PAD_DATA_1K\\\n\
    0xF0F0F0F0, 0xF0F0F0F0, 0xF0F0F0F0, 0xF0F0F0F0, 0xF0F0F0F0,\\\n\
    0xF0F0F0F0, 0xF0F0F0F0, 0xF0F0F0F0, 0xF0F0F0F0, 0xF0F0F0F0,\\\n\
    0xF0F0F0F0, 0xF0F0F0F0, 0xF0F0F0F0, 0xF0F0F0F0, 0xF0F0F0F0,\\\n\
    0xF0F0F0F0, 0xF0F0F0F0, 0xF0F0F0F0, 0xF0F0F0F0, 0xF0F0F0F0,\\\n\
    0xF1F1F1F1, 0xF1F1F1F1, 0xF1F1F1F1, 0xF1F1F1F1, 0xF1F1F1F1,\\\n\
    0xF2F2F2F2, 0xF2F2F2F2, 0xF2F2F2F2, 0xF2F2F2F2, 0xF2F2F2F2,\\\n\
    0xF3F3F3F3, 0xF3F3F3F3, 0xF3F3F3F3, 0xF3F3F3F3, 0xF3F3F3F3,\\\n\
    0xF4F4F4F4, 0xF4F4F4F4, 0xF4F4F4F4, 0xF4F4F4F4, 0xF4F4F4F4,\\\n\
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,\\\n\
    0x11111111, 0x11111111, 0x11111111, 0x11111111, 0x11111111,\\\n\
    0x22222222, 0x22222222, 0x22222222, 0x22222222, 0x22222222,\\\n\
    0x33333333, 0x33333333, 0x33333333, 0x33333333, 0x33333333,\\\n\
    0x44444444, 0x44444444, 0x44444444, 0x44444444, 0x44444444,\\\n\
    0xF0F0F0F0, 0xF0F0F0F0, 0xF0F0F0F0, 0xF0F0F0F0, 0xF0F0F0F0,\\\n\
    0xF1F1F1F1, 0xF1F1F1F1, 0xF1F1F1F1, 0xF1F1F1F1, 0xF1F1F1F1,\\\n\
    0xF2F2F2F2, 0xF2F2F2F2, 0xF2F2F2F2, 0xF2F2F2F2, 0xF2F2F2F2,\\\n\
    0xF3F3F3F3, 0xF3F3F3F3, 0xF3F3F3F3, 0xF3F3F3F3, 0xF3F3F3F3,\\\n\
    0xF4F4F4F4, 0xF4F4F4F4, 0xF4F4F4F4, 0xF4F4F4F4, 0xF4F4F4F4,\\\n\
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,\\\n\
    0x11111111, 0x11111111, 0x11111111, 0x11111111, 0x11111111,\\\n\
    0x22222222, 0x22222222, 0x22222222, 0x22222222, 0x22222222,\\\n\
    0x33333333, 0x33333333, 0x33333333, 0x33333333, 0x33333333,\\\n\
    0x44444444, 0x44444444, 0x44444444, 0x44444444, 0x44444444,\\\n\
    0xF0F0F0F0, 0xF0F0F0F0, 0xF0F0F0F0, 0xF0F0F0F0, 0xF0F0F0F0,\\\n\
    0xF1F1F1F1, 0xF1F1F1F1, 0xF1F1F1F1, 0xF1F1F1F1, 0xF1F1F1F1,\\\n\
    0xF2F2F2F2, 0xF2F2F2F2, 0xF2F2F2F2, 0xF2F2F2F2, 0xF2F2F2F2,\\\n\
    0xF3F3F3F3, 0xF3F3F3F3, 0xF3F3F3F3, 0xF3F3F3F3, 0xF3F3F3F3,\\\n\
    0xF4F4F4F4, 0xF4F4F4F4, 0xF4F4F4F4, 0xF4F4F4F4, 0xF4F4F4F4,\\\n\
    0x000007ac, 0x000000f0, 0x0000042c, 0x00000430, 0x00000430,\\\n\
    0x0000001b, 0x0200008c, 0x0000044c, 0x020000bd, 0x00000498,\\\n\
    0x0200013a, 0x00000538, 0x0200013b, 0x00000548, 0x00000000,\\\n\
    0x00000558, 0x02000003, 0x00000580, 0x02000004, 0x00000584,\\\n\
    0x02000005, 0x00000588, 0x0200007f, 0x0000058c, 0x02000150,\\\n\
    0x00000590, 0x0200014d, 0x00000594, 0x0200014f, 0x000005a4,\\\n\
    0x0200014e, 0x000005e4, 0x0200006f, 0x00000600, 0x02000145,\\\n\
    0x00000670, 0x020000ab, 0x000006c8, 0x02000070, 0x00000708,\\\n\
    0x02000099, 0x0000070c, 0x0200009b, 0x0000071c, 0x0200009c,\\\n\
    0x00000738, 0x0200009e, 0x00000748, 0x02000131, 0x00000758,\\\n\
    0x0200009f, 0x00000768, 0x0200010d, 0x00000778, 0x02000142,\\\n\
    0x00000788, 0x0200014c, 0x00000798, 0x02000020, 0x000007a8,\\\n\
    0x636f7270, 0x6d616e5f, 0x69640065, 0x705f6761, 0x7374726f,\\\n\
    0x736e6900, 0x636e6174, 0x61730065, 0x656c706d, 0x7365745f,\\\n\
    0x6d007374, 0x655f6773, 0x6c62616e, 0x736d0065, 0x6f745f67,\\\n\
    0x6478715f, 0x7562006d, 0x5f736573, 0x74736574, 0x77680073,\\\n\
    0x69676e65, 0x5f73656e, 0x74736574, 0x656b0073, 0x6c656e72,\\\n\
    0x6c61645f, 0x7365745f, 0x6b007374, 0x656e7265, 0x6c625f6c,\\\n\
    0x5f747361, 0x74736574, 0x656b0073, 0x6c656e72, 0x5f346c5f,\\\n\
    0x74736574, 0x706d0073, 0x5f636f72, 0x74736574, 0x79730073,\\\n\
    0x6d657473, 0x76697264, 0x5f737265, 0x74736574, 0x6f700073,\\\n\
    0x5f726577, 0xF0F0F0F0, 0xF0F0F0F0, 0xF0F0F0F0, 0xF0F0F0F0,\\\n\
    0xF0F0F0F0, 0xF0F0F0F0, 0xF0F0F0F0, 0xF0F0F0F0, 0xF0F0F0F0,\\\n\
    0xF0F0F0F0\n\n\
    #define DUMMY_PAD_DATA_5K\\\n\
    DUMMY_PAD_DATA_1K,\\\n\
    DUMMY_PAD_DATA_1K,\\\n\
    DUMMY_PAD_DATA_1K,\\\n\
    DUMMY_PAD_DATA_1K,\\\n\
    DUMMY_PAD_DATA_1K\n\n\n\
    #define DUMMY_PAD_DATA_4K\\\n\
    DUMMY_PAD_DATA_1K,\\\n\
    DUMMY_PAD_DATA_1K,\\\n\
    DUMMY_PAD_DATA_1K,\\\n\
    DUMMY_PAD_DATA_1K,\n\n\n\
    #define DUMMY_PAD_DATA_3K\\\n\
    DUMMY_PAD_DATA_1K,\\\n\
    DUMMY_PAD_DATA_1K,\\\n\
    DUMMY_PAD_DATA_1K,\n\n\n\
    #define DUMMY_PAD_DATA_2K\\\n\
    DUMMY_PAD_DATA_1K,\\\n\
    DUMMY_PAD_DATA_1K,\n\n\n\
    "

#-------------------------------------------------------------------------------
# Based on the guessed size create the correct macro that adds the random date 
# to the pad file of the correct size
#-------------------------------------------------------------------------------
def calculate_pad_file_size(pad_file_guessed_size):
    
    num_pages = pad_file_guessed_size / (1024)
    spill = pad_file_guessed_size % (1024)

    if(spill != 0):
        padding_num_pages = num_pages + 1 
    else:
        padding_num_pages = num_pages
    
    if (padding_num_pages > 15 or padding_num_pages < 1 ):
        return "err"
    
    return "DUMMY_PAD_DATA_" + str(padding_num_pages) + "K"
 
#-------------------------------------------------------------------------------
# parses the linker script file and provides a guessed size of dynamic sections
#-------------------------------------------------------------------------------
def parse_steer_file(steer_file_name):
   exported_symbols =[]
   profiler_info = []
   number_of_exported_syms = 0
   length_of_exported_syms = 0
   guessed_symtab_size  = 0
   guessed_strtrab_size = 0
   guessed_hashtab_size = 0

   with open(steer_file_name) as f:
      content = f.readlines()
  
   for i in range(len(content)):
      if("EXPORT" in (content[i])):
         exported_symbols.append(content[i].split(" ")[1].strip("\n"))
         length_of_exported_syms += len(content[i].split(" ")[1].strip("\n"))

   #these equations are created based on profiling information
   number_of_exported_syms = len(exported_symbols)
   guessed_symtab_size     = 16 * number_of_exported_syms
   guessed_strtrab_size    = length_of_exported_syms
   guessed_hashtab_size    = 4* number_of_exported_syms

   profiler_info.append(number_of_exported_syms)
   profiler_info.append(length_of_exported_syms)
   profiler_info.append(guessed_symtab_size)
   profiler_info.append(guessed_strtrab_size)
   profiler_info.append(guessed_hashtab_size)

   return profiler_info

#-------------------------------------------------------------------------------
# Builder for creating the pad files
#-------------------------------------------------------------------------------
def generate_pad_files(target, source, env):
        
    #parse steer file
    steer_file_name = str(source[0])
    profiler_info = parse_steer_file(steer_file_name)
        
    dyn_pad_file_name = str(target[0])
    dyn_symtab_pad_file_name = str(target[1])
    dyn_strtab_file_name = str(target[2])
    dyn_hashtab_file_name = str(target[3])
    
    #generate files
    try:
        dyn_pad_file_handle = open(dyn_pad_file_name, "w")
        dyn_symtab_file_handle = open(dyn_symtab_pad_file_name, "w")
        dyn_strtab_file_handle = open(dyn_strtab_file_name, "w")
        dyn_hashtab_file_handle = open(dyn_hashtab_file_name, "w")
    except IOError as e:
        errStr = "I/O error({0}): {1}".format(e.errno, e.strerror)
        dlfs_pad_gen_script_error(env, errStr)
    
    #ToDo: Create a header file to include constant macro template
    #generic_header = "#include <stdio.h>\n#include \"padding.h\"\n\n"
        
    #generate_dynamic_file for .dynamic section
    dyn_pad_file_handle.write(generic_header)
    dyn_pad_file_handle.write("unsigned int _DYNAMIC[] = {\n")
    dyn_pad_file_handle.write("\t" + str("DUMMY_PAD_DATA_1K") +"\n};\n")
    dyn_pad_file_handle.close()

    #generate_dynamic_symtab_pad_file for .dynsym section
    dyn_symtab_size_string = calculate_pad_file_size(profiler_info[0])
    if(dyn_symtab_size_string == "err"):
        errStr = "I/O error({0}): {1}".format(e.errno, e.strerror)
        dlfs_pad_gen_script_error(env, errStr)
        
    dyn_symtab_file_handle.write(generic_header)
    dyn_symtab_file_handle.write("unsigned int __dynamic_init_symdata_loc__[] = {\n")
    dyn_symtab_file_handle.write("\t" + dyn_symtab_size_string +"\n};\n")    
    dyn_symtab_file_handle.close()
    
    #generate_dynamic_strtab_pad_file for .dynstr section
    dyn_strtab_size_string = calculate_pad_file_size(profiler_info[1])
    if(dyn_strtab_size_string == "err"):
        errStr = "I/O error({0}): {1}".format(e.errno, e.strerror)
        dlfs_pad_gen_script_error(env, errStr)
        
    dyn_strtab_file_handle.write(generic_header)
    dyn_strtab_file_handle.write("unsigned int __dynamic_init_stringdata_loc__[] = {\n")
    dyn_strtab_file_handle.write("\t" + dyn_strtab_size_string +"\n};\n")   
    dyn_strtab_file_handle.close()
    
    #generate_dynamic_hastab_pad_file for .dynhash section
    dyn_hashtab_size_string = calculate_pad_file_size(profiler_info[2])
    if(dyn_hashtab_size_string == "err"):
        errStr = "I/O error({0}): {1}".format(e.errno, e.strerror)
        dlfs_pad_gen_script_error(env, errStr)
        
    dyn_hashtab_file_handle.write(generic_header)
    dyn_hashtab_file_handle.write("unsigned int __dynamic_init_hashdata_loc__[] = {\n")
    dyn_hashtab_file_handle.write("\t" + dyn_hashtab_size_string +"\n};\n")    
    dyn_hashtab_file_handle.close()
        
    return

#-------------------------------------------------------------------------------
# Helper to generate_pad_files()
#-------------------------------------------------------------------------------
def dlfs_pad_gen_script_error(env, Info):
    """
    Error handler for devcfg framework. Info can be a string that describes the error and
    Info1 can be any data that needs to be printed along with the error message.
    """
    env.PrintError("DLFS pad file generator Script error found: " + Info)
    raise Exception(Info)   