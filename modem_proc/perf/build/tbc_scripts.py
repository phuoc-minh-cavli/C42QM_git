#==============================================================================
# SCONS Tool for Target-Specific Build Parameters.
#
# Copyright (c) 2009-2010 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#==============================================================================
#==============================================================================
#
#                        EDIT HISTORY FOR MODULE
# 3/31/2017 slarin - initial revision
#
#==============================================================================

import sys
import os
import glob
import re
import SCons.Script
import inspect

#=====================================================================
# Global data
#=====================================================================

image_env = None

#=====================================================================
# exists()
#=====================================================================
def exists(env):
    return env.Detect('tbc_build')
# End of exists()

#=====================================================================
# generate()
#=====================================================================
def generate(env):
  # See if tools support LTO
  tool_ver = os.environ.get('HEXAGON_RTOS_RELEASE', '1.1')
  # See if we have lto_config_file
  global lto_config_file_name
  global target_pt_opts_file
  global have_lto_support
  global config_lines
  have_lto_support = False
  
  # If either of these MOB flags are enabled, do not enable LLVM opts
  if ('USES_MOB' in env) and ('USES_LINUX' in env or 'USES_MSVC' in env):
    return

  env.AddMethod(intercept_add_library_calls, "InterceptAddLibraryCalls")

  # Both LTO and target opts supported only for 8.x.xx and on
  if version_compare(tool_ver, '8.0') < 0:
    return

  t_cflags = SCons.Script.ARGUMENTS.get('T_CFLAGS', '')
  all_args = t_cflags.split()

  # Check for target opts support first
  target_pt_opts_file = env.FindConfigFiles('target_pt_opts.txt')
  if len(target_pt_opts_file) > 0:
    target_pt_opts_file = target_pt_opts_file[0]
    env.PrintInfo("Have target opts support with config file %s" % target_pt_opts_file)
    env.Append(CFLAGS="-fprofile-sample-use=%s -fenable-pt-opts -fno-exceptions -fpartition-cold -fsize-opts" % target_pt_opts_file)

  # Now check for LTO support
  lto_config_file_name = env.FindConfigFiles('lto_config_file.txt')
  if len(lto_config_file_name) > 0:
    lto_config_file_name = lto_config_file_name[0]
  else:
    return

  if env.PathExists(lto_config_file_name):
    have_lto_support = True
    env.PrintInfo("Have LTO support with config file %s" % lto_config_file_name)
    lto_config_file = open(lto_config_file_name, 'r')
    config_lines = lto_config_file.readlines()
    lto_config_file.close()
# End of generate()

#negative id v1 < v2
#zero if v1 = v2
#positive if v1 > v2
def version_compare(v1, v2):
  def normalize(v):
    return [int(x) for x in re.sub("[a-zA-Z_-]", "", v).split(".") if x != ""]
  return cmp(normalize(v1), normalize(v2))

def is_key_enable(env, keys):
  '''
  is_key_enable
  '''
  tgt_enable = False

  if type(keys) is not list:
      keys = [keys]

  # check that at least one of the targets will be build
  intersection = set(keys) & set(env.Dictionary())
  if len(intersection) > 0:
      tgt_enable = True
  return tgt_enable
    
def intercept_add_library_calls(env):
  if (have_lto_support == False):
    return

  global OriginalAddLibrary
  global OriginalAddBinaryLibrary
  '''
  We want to find method itself (a function) and search by name
  seems to be the easiest way to get it.
  '''
  for  i in env.added_methods:
    print i.name
    if i.name == 'AddLibrary':
      OriginalAddLibrary = i.method
    if i.name == 'AddBinaryLibrary':
      OriginalAddBinaryLibrary = i.method

  # Overwrite the original definitions
  env.AddMethod(add_lto_library, "AddLibrary")
  env.AddMethod(add_bin_lto_library, "AddBinaryLibrary")

def has_asm(sources):
  '''
  Does this source list contains files to ignore for LTO?
  '''
  for s in sources:
    # SCons.Node.FS.File instances are not strings
    if type(s) is not str:
      s = str(s)
    ext = os.path.splitext(s)[1]
    if ext == ".s" or ext == ".S" or ext == ".o" or ext == ".reloc":
      return True
  return False

def is_in_list(filename):
  if sys.platform.startswith('win'):
    filename = filename.replace('\\','/')
  for lto_lib_name in config_lines:
    lto_lib_name = lto_lib_name.strip()  # Remove newline character.
    if lto_lib_name in filename:
      return True
  return False

def add_lto_library(env, targets, library, sources, *args, **kwargs):
  '''
  add_lto_library
  '''
  frames = inspect.stack()
  filename = frames[2][1]
  if filename.find("Environment.py") != -1:
    filename = frames[3][1]
  if is_key_enable(env, targets):
    if type(sources) is not list:
        sources = [sources]
    sources = env.Flatten(sources)
    if (have_lto_support == True):
      # For now we skip libraries that contain asm
      contains_asm = has_asm(sources)
      do_lto = is_in_list(filename)

      if do_lto and not contains_asm:
        env.Append(CFLAGS="-flto ")

  return OriginalAddLibrary(env, targets, library, sources, *args, **kwargs)

def add_bin_lto_library(env, targets, library, sources, *args, **kwargs):
  '''
  add_bin_lto_library
  '''
  frames = inspect.stack()
  filename = frames[2][1]
  if filename.find("Environment.py") != -1:
    filename = frames[3][1]
  if is_key_enable(env, targets):
    if type(sources) is not list:
        sources = [sources]

    sources = env.Flatten(sources)
    if (have_lto_support == True):
      # For now we skip libraries that contain asm
      contains_asm = has_asm(sources)
      do_lto = is_in_list(filename)

      if do_lto and not contains_asm:
        env.Append(CFLAGS="-flto ")

  return OriginalAddBinaryLibrary(env, targets, library, sources, *args, **kwargs)

