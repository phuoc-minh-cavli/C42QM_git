#===============================================================================
#
# Dynamic Loading
#
# GENERAL DESCRIPTION
#    Contains all MBN Utilities for image generation 
#
# Copyright 2010-2014 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/core.tx/6.0/bsp/build/scripts/elfdynamicutils.py#1 $
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
from modify_dynamic_section import *

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('elfdynamicutil_builder')

def generate(env):
   rootenv = env.get('IMAGE_ENV')
   
   elfdynamicutil_act = env.GetBuilderAction(elfdynamicutil_builder)
   elfdynamicutil_bld = env.Builder(action = elfdynamicutil_act, 
                         source_scanner = SCons.Scanner.C.CScanner(),
                         emitter = elfdynamicutil_emitter
                         )

   rootenv.Append(BUILDERS = {'ElfDynamicUtilBuilder' : elfdynamicutil_bld})

def elfdynamicutil_emitter(target, source, env):
    return (target, source)

def elfdynamicutil_builder(target, source, env):
	elf_file_name = env.RealPath(str(source[0]))
	elf_out_file_name = env.RealPath(str(target[0]))
	modify_dynamic_section(elf_file_name, elf_out_file_name)
