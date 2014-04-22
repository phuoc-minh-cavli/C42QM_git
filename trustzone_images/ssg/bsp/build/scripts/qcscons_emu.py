#Copyright (c) 2019 Qualcomm Technologies, Inc.
#All Rights Reserved.
#Confidential and Proprietary - Qualcomm Technologies, Inc.

import SCons
import SCons.Action
import SCons.Errors
import SCons.Script
import SCons.Tool
from SCons.Script import *

import errno
import glob
import inspect
import os
import re
import stat
import string
import subprocess
import sys
import tempfile

#----------------------------------------------------------------------------
# Hooks for Scons
#----------------------------------------------------------------------------
def exists(env):
  return True

def generate(env):
  env.AddMethod(require_restricted_api_emu, "RequireRestrictedApi")

#----------------------------------------------------------------------------
# copied from config.py
#----------------------------------------------------------------------------
def inc_paths_builder(env, incpaths_env, api_names, dbg_str=None, fname=None):
   paths_to_add = list()
   # get command line options
   useincfiles = env.GetOption('useincfiles')
   #debugincpaths_str = env.GetOption('debugincpaths').lower()
   debuginfo_opt = env.get('DEBUGINFO_OPT')
   debugincpaths = False
   clean = env.GetOption('clean')
   cwd = os.getcwd()

   if dbg_str == None and fname != None:
      dbg_str = "Require APIs for " + file
   elif dbg_str == None:
      dbg_str = "Require APIs"

   if fname == None:
      useincfiles = False

   # turn on debugging
   if ('incpaths' in debuginfo_opt) or ('incpaths-int' in debuginfo_opt) or ('incpaths-ext' in debuginfo_opt):
      debugincpaths = True

   if not useincfiles:
      if debugincpaths:
         env.PrintDebugInfo('any', "================================================================")
         env.PrintDebugInfo('any', "=== " + dbg_str)
         env.PrintDebugInfo('any', "=== from " + cwd)
         env.PrintDebugInfo('any', "----------------------------------------------------------------")
         env.PrintDebugInfo('any', api_names)

      # don't use external or internal inlcude files, put incpaths in comand line
      for api_name in api_names:

         #import pdb; pdb.set_trace()
         if api_name in incpaths_env:
            incpath_list = incpaths_env[api_name]
            for p in incpath_list:
               if p not in paths_to_add:
                  paths_to_add.append(p)
      ret = []
      for p in paths_to_add:
        exists = os.path.exists(p)
        if debugincpaths:
          env.PrintDebugInfo('any', " {} = {}".format('i' if exists else 'x', p))
        if exists:
          ret.append(p)
      return ret

   else: # useincfiles is True
      # create external or internal include files
      # Little hack, to get the correct location for the file
      # if the target path is the same as the script, scons will return
      # the full path from the File method, but if the target dir is not
      # in CWD it will return just the file with no path
      check_build_path = env.File(fname)

      #print os.path.realpath(fname)
      #print os.path.realpath(str(check_build_path))

      # determine file full path
      if os.path.realpath(str(check_build_path)) == os.path.realpath(fname):
         target_file = os.path.realpath(os.getcwd() + "/" + env.subst('${BUILDPATH}') + "/" + fname + ".inc")
      else:
         target_file = os.path.realpath(os.getcwd() + "/" + fname + ".inc")

      if clean:
         #Delete(target_file)
         if os.path.exists(target_file):
            os.unlink(target_file)
            print env.subst("Removed ") + target_file
      else:
         incpaths_dict = dict()
         incpaths_list = []

         if debugincpaths:
            print "================================================================"
            print "=== " + dbg_str
            print "=== from " + cwd
            print "----------------------------------------------------------------"
            print list
         for incpath in list:
            for i in incpaths_env.subst(incpath).split():
               if os.path.exists(i):
                  i = os.path.realpath(i)
                  if debugincpaths:
                     print " i = " + i
                  #env.AppendUnique(INCPATHS = "${INCPREFIX}"+ i +" ")
                  i = incpaths_env.subst("${INCPREFIX}") + i
                  if i not in incpaths_dict:
                     incpaths_dict[i] = 0
                     incpaths_list.append(i)
               else:
                  if debugincpaths:
                     print " x = " + i
         if debugincpaths:
            print "----------------------------------------------------------------"

         if not os.path.exists(os.path.split(target_file)[0]):
            os.mkdir(os.path.split(target_file)[0])

         incpaths_file = open(target_file, 'a')

         for incpath in incpaths_list:
            # write inc paths file
            incpaths_file.write (incpath + '\n')
         incpaths_file.close()

         env.Append(INCPATHS = ' --via ' + target_file)
   print 'reached the end of inc_paths_builder because ???'
   return None

def require_api(env, api_env, api_prefix, api_names, api_suffix, dbg_str, fname, append=True):
   paths = []
   # make sure api_names is a list
   if type(api_names) != list:
      api_names = [api_names]

   if len(api_names) > 0:
      #import pdb; pdb.set_trace()

      api_list = ["".join([api_prefix, api_name, api_suffix]) for api_name in api_names]
      paths = inc_paths_builder(env, api_env, api_list, dbg_str, fname)
      if append:
        env.AppendUnique(CPPPATH=paths)
   return paths


# on-target, we use require_au_restricted_api for RequireRestrictedApi()
#
# we cannot use this off-target as we're not calling LoadAreaSoftwareUnits()
# which eventually calls InitAUIncPaths()
def require_restricted_api_emu(env, api_names, append=True):
   api_prefix = "SSG_" # force to SSG_, this is CORE_ on-target
   api_env = env['SCONS_INCPATHS_ENV']
   return require_api(env, api_env, api_prefix, api_names, api_suffix="_RESTRICTED",
      dbg_str="Required Restricted APIs", fname="internal_restricted", append=append)
