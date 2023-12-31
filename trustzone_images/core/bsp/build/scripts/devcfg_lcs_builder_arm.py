#===============================================================================
#
# Device Config Builder
#
# GENERAL DESCRIPTION
#    Contains builder(s) definitions
#
# Copyright (c) 2014 by QUALCOMM, Technologies Inc.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/core.tz/2.1.3/bsp/build/scripts/devcfg_lcs_builder_arm.py#1 $
#  $DateTime: 2019/05/23 05:43:45 $
#  $Author: pwbldsvc $
#  $Change: 19292165 $
#                      EDIT HISTORY FOR FILE
#                      
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#  
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 10/14/14   aa    Initial Revision
#===============================================================================

import os
import subprocess
import string
from SCons.Script import *

#------------------------------------------------------------------------------
# Globals
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return True

def generate(env):
   """
   Generate function for devcfg lcs builder. 
   Generates devcfg linker script
   """

   rootenv = env.get('IMAGE_ENV')
   # Devcfg linker script generator
   devcfg_lcs_act = env.GetBuilderAction(devcfg_lcs_builder, action_source=None)
   devcfg_lcs_target_scan = env.Scanner(devcfg_lcs_target_scan_func, name='DevCfgTargetScanner')
   devcfg_lcs_bld = env.Builder(action = devcfg_lcs_act,
                                target_scanner = devcfg_lcs_target_scan,
                                suffix = '.lcs')
   rootenv.Append(BUILDERS = {'DevcfgLCSBuilder' : devcfg_lcs_bld})

   return
   
def devcfg_lcs_target_scan_func(node, env, path):
   rootenv = env.get('IMAGE_ENV')
   env.Depends(node, env.Value(rootenv.get('DEVCFG_INFO_COLLECTOR', [])))
   return []

   
def devcfg_lcs_builder(target, source, env):
   """
   source: lcs template file name with location
   """
   rootenv = env.get('IMAGE_ENV')
   
   if rootenv.GetUsesFlag('USES_DEVCFG') is False:
      return None
     
   # The output linker script file name
   lcs_fn = env.RealPath(str(target[0]))
   
   create_devcfg_lcs_file(rootenv, lcs_fn)
   
def create_devcfg_lcs_file(env, output_devcfg_lcs_file):
   """
   This creates the output linker script for devcfg image
   INPUTS:
   env: Build environment
   template_file: Template filename that the linker script will be based off of
   devcfg_hdr_filename: The devcfg header file that will contain the addressed
   of the segments. These will be reserved in the primary elf
   OUTPUT:
   output_devcfg_lcs_file: Generated devcfg lcs file   
   """
   #import pdb;pdb.set_trace()
   devcfg_xml_list =  env.get('DEVCFG_INFO_COLLECTOR', [])  
   hyp_source_list = []
   tz_source_list = []
   for build_tag in env.get('BUILD_TAGS') :
      for sub_dict in  env.get('DEVCFG_INFO_COLLECTOR', []) :
         sources = sub_dict.get(build_tag+'_C')
         if(sources):
            if 'HYP'.lower() in build_tag.lower() :
               hyp_source_list.extend(sources)
            if 'QSEE'.lower() in build_tag.lower() :
               tz_source_list.extend(sources)
   
   try:
      lcsf = open(output_devcfg_lcs_file, 'w')
   except IOError:
      err_str = " Couldn't open file: " + output_devcfg_lcs_file + "for writing\n"
      devcfg_lcs_error(err_str)
   
   template = '''/*===========================================================================
#
# FILE:         devconfig_data.scl
#
# DESCRIPTION:  Linker File for deviceconfig image
#
# AUTOGENERATED, DON NOT MODIFY THIS FILE
===========================================================================*/
#include "customer.h"
\n'''

   lcsf.write(template)  
   #------------------------------------------------------------------------------
   # Linker Rule for QSEE Data
   #------------------------------------------------------------------------------
   lcsf.write('''
DEVCFG_DATA_SEG_TZ (DEVCFG_DATA_TZ_ADDR)
{
   DEVCFG_DATA_TZ_RO +0x0
   {
      devcfg_data_tz_oem.o (+RO-DATA, +FIRST)
      DALConfig_tz_oem.o (+RO-DATA)
''')
   for file in tz_source_list:
      file_path = env.RealPath(str(file))
      file_name_ext = os.path.basename(file_path)
      lcsf.write("      " + file_name_ext + " (+RO-DATA)\n")
   lcsf.write('''   }
}\n''')
   
   lcsf.write('''
DEVCFG_DATA_SEG2_TZ (+0x0) 
{
   DEVCFG_DATA_TZ_ZI +0x0 ZEROPAD
   {
''')
   lcsf.write("      devcfg_data_tz_oem.o (+ZI)\n")
   lcsf.write("      DALConfig_tz_oem.o (+ZI)\n")
   for file in tz_source_list:
      file_path = env.RealPath(str(file))
      file_name_ext = os.path.basename(file_path)
      lcsf.write("      " + file_name_ext + " (+ZI)\n")   
   lcsf.write('''    }\n
   DEVCFG_DATA_TZ_RW +0x0
   {
''')
   lcsf.write("      devcfg_data_tz_oem.o (+RW-DATA)\n")
   lcsf.write("      DALConfig_tz_oem.o (+RW-DATA)\n")
   for file in tz_source_list:
      file_path = env.RealPath(str(file))
      file_name_ext = os.path.basename(file_path)
      lcsf.write("      " + file_name_ext + " (+RW-DATA)\n")
   lcsf.write('''   }
}\n''')

   lcsf.write('''
DEVCFG_CODE_SEG3_TZ (+0x0)
{
   TZ_CODE_SEC +0x0
   {
''')
   lcsf.write("      devcfg_data_tz_oem.o (+RO-CODE)\n")
   lcsf.write("      DALConfig_tz_oem.o (+RO-CODE)\n")
   for file in tz_source_list:
      file_path = env.RealPath(str(file))
      file_name_ext = os.path.basename(file_path)
      lcsf.write("      " + file_name_ext + " (+RO-CODE)\n")
   lcsf.write('''   }
}\n''')
   
   #------------------------------------------------------------------------------
   # Linker Rule for HYP Data
   #------------------------------------------------------------------------------
   lcsf.write('''
DEVCFG_DATA_SEG_HYP (DEVCFG_DATA_HYP_ADDR)
{
   DEVCFG_DATA_HYP_RO +0x0
   {
      devcfg_data_hyp_oem.o (+RO-DATA, +FIRST)
      DALConfig_hyp_oem.o (+RO-DATA)
''')
   for file in hyp_source_list:
      file_path = env.RealPath(str(file))
      file_name_ext = os.path.basename(file_path)
      lcsf.write("      " + file_name_ext + " (+RO-DATA)\n")
   lcsf.write('''   }
}\n''')
   
   lcsf.write('''
DEVCFG_DATA_SEG2_HYP (+0x0) 
{
   DEVCFG_DATA_HYP_ZI +0x0 ZEROPAD
   {
''')
   lcsf.write("      devcfg_data_hyp_oem.o (+ZI)\n")
   lcsf.write("      DALConfig_hyp_oem.o (+ZI)\n")
   for file in hyp_source_list:
      file_path = env.RealPath(str(file))
      file_name_ext = os.path.basename(file_path)
      lcsf.write("      " + file_name_ext + " (+ZI)\n")
   lcsf.write('''    }\n
   DEVCFG_DATA_HYP_RW +0x0
   {
''')
   lcsf.write("      devcfg_data_hyp_oem.o (+RW-DATA)\n")
   lcsf.write("      DALConfig_hyp_oem.o (+RW-DATA)\n")
   for file in hyp_source_list:
      file_path = env.RealPath(str(file))
      file_name_ext = os.path.basename(file_path)
      lcsf.write("      " + file_name_ext + " (+RW-DATA)\n")
   lcsf.write('''   }
}\n''')

   lcsf.write('''
DEVCFG_CODE_SEG3_HYP (+0x0)
{
   HYP_CODE_SEC +0x0
   {
''')
   lcsf.write("      devcfg_data_hyp_oem.o (+RO-CODE)\n")
   lcsf.write("      DALConfig_hyp_oem.o (+RO-CODE)\n")
   for file in hyp_source_list:
      file_path = env.RealPath(str(file))
      file_name_ext = os.path.basename(file_path)
      lcsf.write("      " + file_name_ext + " (+RO-CODE)\n")
   lcsf.write('''   }
}\n''')

   lcsf.close()
   
   return
    
def devcfg_lcs_error(Info, Info1=None):
   """
   Error handler for devcfg framework. Info can be a string that describes the error and
   Info1 can be any data that needs to be printed along with the error message.
   """
   print Info
   raise Exception(Info)
   