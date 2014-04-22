#==========================================================================
#
#  QC SCons overrides file for hexagon builders:
#
#  This file is used to temporarily override Hexagon compiler options 
#  during the transition to LLVM for mpss.dpm.2.0.
#
#  Name this file hexagon_oem.py and place it in config/default.  The
#  environment variable BUILD_SCRIPTS_OEM_ROOT must point to config/default.
#
#==========================================================================

import os

#--------------------------------------------------------------------------
# Hooks for SCons
#--------------------------------------------------------------------------

def exists(env):
   return env.Detect('hexagon_product')

def generate(env):

   ENDFLAGS = ' '
   ENDCXXFLAGS = ' -std=c++03 '

   if 'USES_AUTOSTKV2' in env:
      ENDFLAGS += ' -fdisable-sp-updates '
      ENDCXXFLAGS += ' -fdisable-sp-updates '

   env.PrependCCAction(env.Append(CCFLAGS = ENDFLAGS))
   env.PrependCXXAction(env.Append(CXXFLAGS = ENDCXXFLAGS))
