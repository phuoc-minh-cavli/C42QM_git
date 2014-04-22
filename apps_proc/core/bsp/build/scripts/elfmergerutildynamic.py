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
#  $Header: //components/rel/core.tx/6.0/bsp/build/scripts/elfmergerutildynamic.py#1 $
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

import sys
import os
import shutil
from SCons.Script import *
from elfparserutil import *

#----------------------------------------------------------------------------
# GLOBAL VARIABLES
#----------------------------------------------------------------------------
INPUT_ELF = 'input_elf'
SECTION_NAME = 'section_name'

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('elfmergerutil_builder_dynamic')

def generate(env):
   rootenv = env.get('IMAGE_ENV')
   #rootenv.AddMethod(query_section_by_name_util, "QuerySectionByName")
   
   elfmergerutildynamic_act = env.GetBuilderAction(elfmergerutil_builder_dynamic)
   elfmergerutildynamic_bld = env.Builder(action = elfmergerutildynamic_act, 
                         source_scanner = SCons.Scanner.C.CScanner(),
                         emitter = elfmergerutil_emitter_dynamic
                         )

   rootenv.Append(BUILDERS = {'ElfMergerUtilBuilderDynamic' : elfmergerutildynamic_bld})

def elfmergerutil_emitter_dynamic(target, source, env):
    return (target, source)


#------------------------------------------------------------------------------
# Copy and merge PT_NULL marked dynamic sections to PT_LOAD marked DYNAMIC sections
# .dyanmic -> DYNAMIC_DATA
# .dynsym ->  DYNAMIC_SYMBOL_TABLE
# .dynstr -> DYNAMIC_STRING_TABLE
# .hash -> DYNAMIC_HASH_TABLE
#------------------------------------------------------------------------------    

def merge_elf_util_dynamic(pri_file_name, sec_file_name, 
                          out_file_name, verbose=False):
    global VERBOSE
    VERBOSE = verbose
    
    section_name_list = [("DYNAMIC_DATA", ".dynamic"), 
                         ("DYNAMIC_STRING_TABLE", ".dynstr"), 
                         ("DYNAMIC_SYMBOL_TABLE", ".dynsym"), 
                         ("DYNAMIC_HASH_TABLE", ".hash")
                        ]
    
    sec_elf_fp = OPEN(sec_file_name, "rb")
    sec_elf_dict = preprocess_elf_file(sec_elf_fp, sec_file_name)
    
    shutil.copyfile(pri_file_name, out_file_name)
    out_elf_fp = OPEN(out_file_name, "r+b")
    out_elf_dict = preprocess_elf_file(out_elf_fp, out_file_name)

    for sh_name in section_name_list:    
        no_error = False
        try:
            merge_dynamic_sections_by_name(out_elf_dict, sec_elf_dict, pri_file_name,
                                sh_name[0], sh_name[1])
            no_error = True
        except RuntimeError as err_str:
            raise RuntimeError, err_str
    
    sec_elf_fp.close()
    out_elf_fp.close()
    if False == no_error:
        os.remove(out_file_name)

    return None

#------------------------------------------------------------------------------
#  Mapped to ElfMergerUtilBuilderDynamic
#------------------------------------------------------------------------------
def elfmergerutil_builder_dynamic(target, source, env):
    pri_elf = env.RealPath(str(source[0]))
    sec_elf = env.RealPath(str(source[1]))
    target_full = env.RealPath(str(target[0]))
    
    merge_elf_util_dynamic(pri_elf, sec_elf, target_full, True)
   

    return