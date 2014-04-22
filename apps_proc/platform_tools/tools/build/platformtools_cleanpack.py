#==============================================================================#
#
# tx_platform Cleanpack SCons Script
#
# GENERAL DESCRIPTION
#    Scons cleanpack script for tx_platform
#
# Copyright (c) 2021-22 Qualcomm Technologies Incorporated.
#
# All Rights Reserved. Qualcomm Confidential and Proprietary
# Export of this technology or software is regulated by the U.S. Government.
# Diversion contrary to U.S. law prohibited.
#
# All ideas, data and information contained in or disclosed by
# this document are confidential and proprietary information of
# QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
# By accepting this material the recipient agrees that this material
# and the information contained therein are held in confidence and in
# trust and will not be used, copied, reproduced in whole or in part,
# nor its contents revealed in any manner to others without the express
# written permission of QUALCOMM Technologies Incorporated.
#
#------------------------------------------------------------------------------#
# $Header: //components/rel/platform_tools.tx/1.0/tools/build/platformtools_cleanpack.py#2 $
#------------------------------------------------------------------------------#
#
#==============================================================================#

def exists(env):
   return env.Detect('PLATFORMTOOLS_cleanpack')

def generate(env):
   # Current All Modules completely removed except demo folder.
   # Pack exceptions: USES_CUSTOMER_GENERATE_LIBS, USES_COMPILE_L1_OPT_AC_PROTECTED_LIBS
   # - These flags enable to ship sources/non-sources to certain customers.
   # - Files which should not be shipped to certain customer(s) should not have the these pack exceptions.
   # - Any new flags need to be added to this list.
   DATA_PACK_EXCEPTION = ["USES_CUSTOMER_GENERATE_LIBS", "USES_COMPILE_L1_OPT_AC_PROTECTED_LIBS"]
   DATA_PACK_EXCEPTION_TIER1 = ["USES_CUSTOMER_GENERATE_LIBS"]
   # Note: 
   # - Below CleanPack calls will remove all no-source and header files;
   # - Scons already knows about source code based on AddLibrary/AddBinaryLibrary calls
   #   - *.c/*.cpp files will get removed through Scons step.

   # pcid - Shipped as Binary Library
#   env.CleanPack("CLEANPACK_TARGET", env.FindFiles("*", "../pcid/src"))
#   env.CleanPack("CLEANPACK_TARGET", env.FindFiles("*", "../pcid/inc"))

   # platform - Shipped as Binary Library - remove un-compiled files
#   env.CleanPack("CLEANPACK_TARGET", env.FindFiles("*", "../platform/src"))

   # xtra - Shipped as Binary Library

   # Build
   env.CleanPack("CLEANPACK_TARGET", "../build/location_cleanpack.py")

   # The following base source files are not adequately cleaned by the --cleanpack
   # flag, and must be manually removed.  The reason for this is most likely one
   # of the following:
   #
   #    1) A .scons file uses a USES flag to control whether or not this module is
   #       built, and that flag is preventing AddBinaryLibrary from being called,
   #       so QC-SCons doesn't know that these files need to be cleaned.  If all
   #       base source files (.c, .cpp, etc., but not .h) are to be removed, then
   #       this could be done automatically in the .scons file with the following
   #       line:
   #
   #             env.CleanPack('CLEANPACK_TARGET', env.FindFiles(['*.c', '*.cpp]))
   #
   #    2) Extraneous base source files are synched from Perforce that are not used
   #       in the build, and so QC-SCons doesn't know to remove them.
   #
   #       In this case, it is appropriate to simply list the files for cleaning in
   #       this script.
   #
   #    2) This list of base source files is detected at build time from the list
   #       of files in the build, and the .scons file depended on the old pack
   #       scripts to remove the necessary files.
   #
   #       In this case, it is appropriate to simply list the files for cleaning in
   #       this script.
