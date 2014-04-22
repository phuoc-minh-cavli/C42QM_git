#===============================================================================
#
# GENERAL DESCRIPTION
#    build script
#
# Copyright (c) 2018 by QUALCOMM Technologies, Incorporated.
# All Rights Reserved.
# Qualcomm Technologies Confidential and Proprietary
#
#-------------------------------------------------------------------------------
#
# $Header: //components/rel/msf.mpss/1.0/build/msf.py#1 $
# $DateTime: 2018/06/27 11:21:46 $
# $Author: pwbldsvc $
#===============================================================================

def exists(env):
   return env.Detect('MSF_cleanpack')

def generate(env):
   env.CleanPack("CLEANPACK_TARGET", env.FindFiles("*", "../pack"))
   env.CleanPack("CLEANPACK_TARGET", "msf_cleanpack.py")

