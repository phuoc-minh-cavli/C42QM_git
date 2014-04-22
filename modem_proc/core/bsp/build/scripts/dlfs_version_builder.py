#===============================================================================
#Copyright (c) 2014 -2017 Qualcomm Technologies, Incorporated.  All Rights Reserved.
#QUALCOMM Proprietary.  Export of this technology or software is regulated
#by the U.S. Government, Diversion contrary to U.S. law prohibited.
#===============================================================================


#-------------------------------------------------------------------------------
#                      EDIT HISTORY FOR FILE
#
#  $Header: //components/rel/core.mpss/3.10/bsp/build/scripts/dlfs_version_builder.py#1 $
#  $DateTime: 2018/06/26 03:18:17 $
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 09/08/14   amitkulk     Selective versioning
# 02/10/15   amitkulk     new init list gen builder
#===============================================================================

import os
import string
import SCons.Scanner
import datetime
import ntpath
from SCons.Script import *
import uuid


#------------------------------------------------------------------------------
# Globals
#------------------------------------------------------------------------------
SHLIB_VERSIONED_SO_LIST = 'SHLIB_INFO_COLLECTOR'

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('dlfs_version_builder')

#-------------------------------------------------------------------------------
# Generate builder
#-------------------------------------------------------------------------------
def generate(env):
   rootenv = env.get('IMAGE_ENV')
   rootenv[SHLIB_VERSIONED_SO_LIST] = []
   version_init_c_generate(env)
   version_list_init_c_generate(env)
   version_list_init_c_from_lf_generate(env)
   
   return

def version_init_c_generate(env):
   env.AddMethod(create_ver_info_c, "AddSharedLibraryVersion")
   version_init_c_act = env.GetBuilderAction(create_ver_info_c_bldr)
   version_init_c_bld = env.Builder(action = version_init_c_act,
                            source_scanner = SCons.Scanner.C.CScanner())
   env.Append(BUILDERS = {'AddSharedLibraryVersionBldr' : version_init_c_bld})
   
   return 
     
def version_list_init_c_generate(env):
   version_list_init_c_act = env.GetBuilderAction(create_version_info_init_list_c)
   version_list_init_c_bld = env.Builder(action = version_list_init_c_act)
   env.Append(BUILDERS = {'AddSharedLibraryVersionList' : version_list_init_c_bld})
   
   return
   
def version_list_init_c_from_lf_generate(env):
   version_list_init_c_from_lf_act = env.GetBuilderAction(create_version_info_init_list_c_from_lf)
   version_list_init_c_from_lf_bld = env.Builder(action = version_list_init_c_from_lf_act)
   env.Append(BUILDERS = {'AddSharedLibraryVersionListFromLf' : version_list_init_c_from_lf_bld})
   
   return

#------------------------------------------------------------------------------
# Globals Templates(Read Only)
#------------------------------------------------------------------------------
template_version_info_type = "typedef enum { \n\
   VERSION_INFO_MAGIC, \n\
   VERSION_INFO_TIMESTAMP, \n\
   VERSION_INFO_UUID, \n\
   VERSION_INFO_HEX_TOOL_REV, \n\
   VERSION_INFO_UNDEFINED \n\
}version_info_data_type; \n\n" 

template_version_info = "typedef struct version_info{\n\
   version_info_data_type version_info_type ;\n \
   union \n\
   {\n\
      int   version_data_int ;\n\
      char  version_data_stream[50];\n\
   }version_info_data;\n \
}version_info;\n\n" \
 
template_version_info_list = "typedef struct version_info_symbol_table\n\
{\n\
   const char     *filename;\n\
   version_info   *version_symbol_name;\n\
}version_info_symbol_table;\n\n"


def create_ver_info_c(env, target, source, SHARED_LIB):
   
   rootenv = env.get('IMAGE_ENV')
   versioned_shared_lib_realpath = os.path.join(env.RealPath(env.get('BUILDPATH')), SHARED_LIB) + ".so"
   rootenv[SHLIB_VERSIONED_SO_LIST].append(versioned_shared_lib_realpath)
   version_info = env.AddSharedLibraryVersionBldr(target, source, SHARED_LIB=SHARED_LIB)
   return version_info


#-------------------------------------------------------------------------------
# Builder that generates versoion_info.c file for every SO file that is created
# Target: version_info.c, Source: SO_name(for dependency scanner)
#-------------------------------------------------------------------------------
def create_ver_info_c_bldr(target, source, env):
  
   global template_version_info
   #rootenv = env.get('IMAGE_ENV')
   
   shdlib_version_info_file = env.RealPath(str(target[0]))
   mainimg_version_info_file = env.RealPath(str(target[1]))
   try:
      so_name = env['SHARED_LIB']
   except KeyError:
      dlfs_version_script_error(env, 'SharedLibrary name not passed to builder')   

   magic_number = 0xDEED;
   UUID = "\"" + str(uuid.uuid4()).upper() + "\""
   hex_tools = "\"" +str(env.get('HEXAGON_RTOS_RELEASE')) + "\""
   curr_datetime =  datetime.datetime.now()
   dd = curr_datetime.day
   mm = curr_datetime.month
   yy = curr_datetime.year
   hh = curr_datetime.hour
   minutes = curr_datetime.minute 
  
   try:
      version_info_file_handle = open(shdlib_version_info_file, "w")
      mainimg_version_info_file_handle = open(mainimg_version_info_file, "w")
   except IOError as e:
      errStr = "I/O error({0}): {1}".format(e.errno, e.strerror)
      dlfs_version_script_error(env, errStr)
   
   #Write banner
   banner = env.CreateFileBanner(os.path.split(shdlib_version_info_file)[1], style='C')
   version_info_file_handle.write(banner)
   banner = env.CreateFileBanner(os.path.split(mainimg_version_info_file)[1], style='C')
   mainimg_version_info_file_handle.write(banner)
   
   #Independent version_info symbol name for every so, ver_info_ + so_name
   version_symbol_def_sec_attr = "version_info  ver_info_"+ so_name + "[]\n__attribute__ ((section(\".version_section\"))) = "
   version_symbol_def = "version_info  ver_info_"+ so_name + "[] = "
   
   # Modify as new version control information is added    
   init_string = "{ \n" + \
   "{ VERSION_INFO_MAGIC, { version_data_int : "  + str(hex(magic_number)) + "}} , /* Keep it first member*/ \n" + \
   "{ VERSION_INFO_TIMESTAMP, { version_data_stream : " + "\"" + str(dd) + "-" + str(mm) + "-" + str(yy) + " " + str(hh) + ":" + str(minutes) + "\" }} ,  \n" +  \
   "{ VERSION_INFO_HEX_TOOL_REV, { version_data_stream : " + hex_tools + "}} , \n" + \
   "{ VERSION_INFO_UUID, { version_data_stream : " + UUID + "}} , /* Keep it last member*/  \n" + \
   "};\n"
   
   #version_info_file_handle.write(template_UUID_macro)
   version_info_file_handle.write(template_version_info_type)
   version_info_file_handle.write(template_version_info)
   # version_info_file_handle.write(version_info_size_string)
   version_info_file_handle.write(version_symbol_def_sec_attr + init_string)
   
   #mainimg_version_info_file_handle.write(template_UUID_macro)
   mainimg_version_info_file_handle.write(template_version_info_type)
   mainimg_version_info_file_handle.write(template_version_info)
   mainimg_version_info_file_handle.write(version_symbol_def + init_string)
   
   #rootenv[SHLIB_VERSIONED_SO_LIST].append(so_name)

   version_info_file_handle.close()
   mainimg_version_info_file_handle.close()
   return

#-------------------------------------------------------------------------------
# Builder for creating the list of version information symbols to query at run time
#-------------------------------------------------------------------------------
def create_version_info_init_list_c(target, source, env):
   
   global template_version_info
   global template_version_info_list
   global template_version_info_type
   rootenv = env.get('IMAGE_ENV')
   
   
   target_file_name = str(target[0])
   # create c file to generate version info list for all shared libraries.
   try:
      target_file_handle = open(target_file_name, "w")
   except IOError as e:
      errStr = "I/O error({0}): {1}".format(e.errno, e.strerror)
      dlfs_version_script_error(env, errStr)

   #Write banner
   banner = env.CreateFileBanner(os.path.split(target_file_name)[1], style='C')
   target_file_handle.write(banner)
   
   #SHLIB_VERSIONED_SO_LIST keeps track of all DSO names that are using versioning
   list_of_so_files = []
   for item in rootenv[SHLIB_VERSIONED_SO_LIST]: 
      list_of_so_files.append(item + ".so.mbn")


   #Number of DSO version symbols present in the main image
   symbol_number_string = "\nint num_ver_info_symbols = " + str(len(list_of_so_files)) + ";\n"

   #extern the version symbols, so that they will be linked to ones present in image
   extern_symbol_list =""
   for item in list_of_so_files:
      extern_symbol_list += ("extern version_info ver_info_" + item.split(".")[0] + "[];\n")

   #Table of symbol-names and symbol addresses to access at run time.
   version_info_list = "\nversion_info_symbol_table version_info_symtab[] =\n{\n"

   #Init list of Table of symbol-names and symbol addresses to access at run time.
   for i in range (0, len(list_of_so_files)):
      version_info_list += ("   {\""+ "/" +list_of_so_files[i] + "\", " + "ver_info_"+ list_of_so_files[i].split(".")[0]+ ", }")
      if i != len(list_of_so_files)-1:
         version_info_list += ",\n"
      
   version_info_list += "\n};"

   #Finally write to the file and close
   target_file_handle.write(template_version_info_type)
   target_file_handle.write(template_version_info)
   target_file_handle.write(template_version_info_list)
   target_file_handle.write(symbol_number_string)
   target_file_handle.write(extern_symbol_list)
   target_file_handle.write(version_info_list)  
   target_file_handle.close()

   return
   
#-------------------------------------------------------------------------------
# Builder for creating the list of version information symbols to query at run time
# This is slightly different from create_version_info_init_list_c since this builder 
# takes list of list files as an argument as opposed to list of DSO names.
#-------------------------------------------------------------------------------
def create_version_info_init_list_c_from_lf(target, source, env):
   
   global template_version_info
   global template_version_info_list
   global template_version_info_type
   rootenv = env.get('IMAGE_ENV')

   target_file_name = str(target[0])
   
   # create c file to generate version info list for all shared libraries.
   try:
      target_file_handle = open(target_file_name, "w")
   except IOError as e:
      errStr = "I/O error({0}): {1}".format(e.errno, e.strerror)
      dlfs_version_script_error(env, errStr)
      
   #List to hold all DSO paths
   shlib_versioned_so_list = []
   
   for file in source:
      source_file_name = str(file)
    
      # open lf file to generate list for all shared libraries.
      try:
         source_file_handle = open(source_file_name, "rb")
      except IOError as e:
         errStr = "I/O error({0}): {1}".format(e.errno, e.strerror)
         dlfs_version_script_error(env, errStr)

      #import pdb;pdb.set_trace()
      for line in source_file_handle.readlines():
         dso_name = line.strip('\r\n').split('/')[-1]
         shlib_versioned_so_list.append(str(dso_name))
  
   #Write banner
   banner = env.CreateFileBanner(os.path.split(target_file_name)[1], style='C')
   target_file_handle.write(banner)
   
   #SHLIB_VERSIONED_SO_LIST keeps track of all DSO names that are using versioning
   list_of_so_files = []
   for item in shlib_versioned_so_list: 
      list_of_so_files.append(item + ".mbn")


   #Number of DSO version symbols present in the main image
   symbol_number_string = "\nint num_ver_info_symbols = " + str(len(list_of_so_files)) + ";\n"

   #extern the version symbols, so that they will be linked to ones present in image
   extern_symbol_list =""
   for item in list_of_so_files:
      extern_symbol_list += ("extern version_info ver_info_" + item.split(".")[0] + "[];\n")

   #Table of symbol-names and symbol addresses to access at run time.
   version_info_list = "\nversion_info_symbol_table version_info_symtab[] =\n{\n"

   #Init list of Table of symbol-names and symbol addresses to access at run time.
   for i in range (0, len(list_of_so_files)):
      version_info_list += ("   {\""+ "/" +list_of_so_files[i] + "\", " + "ver_info_"+ list_of_so_files[i].split(".")[0]+ ", }")
      if i != len(list_of_so_files)-1:
         version_info_list += ",\n"
      
   version_info_list += "\n};"

   #Finally write to the file and close
   target_file_handle.write(template_version_info_type)
   target_file_handle.write(template_version_info)
   target_file_handle.write(template_version_info_list)
   target_file_handle.write(symbol_number_string)
   target_file_handle.write(extern_symbol_list)
   target_file_handle.write(version_info_list)  
   target_file_handle.close()

   return

   
def dlfs_version_script_error(env, Info):
    """
    Error handler for devcfg framework. Info can be a string that describes the error and
    Info1 can be any data that needs to be printed along with the error message.
    """
    env.PrintError("DLFS Version Script error found: " + Info)
    raise Exception(Info)   