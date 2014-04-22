#===============================================================================
#
# SECUREMSM Libs
#
# GENERAL DESCRIPTION
#    build script
#
# Copyright (c) 2015 Qualcomm Technologies Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/core.mpss/3.10/securemsm/cryptodrivers/prng/chipset/build/mdm9205.SConscript#1 $
#  $DateTime: 2018/09/07 03:22:57 $
#  $Author: pwbldsvc $
#  $Change: 17066400 $
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 09/26/11   nk      Intial checkin
#===============================================================================
import os
Import('env')
env = env.Clone()

# 9607 interface redirection
CHIPSETREDIRECTION = 'mdm9205'

script_sconfname = None
if os.path.exists('../' + CHIPSETREDIRECTION + '/build/SConscript') :
   script_sconfname = '../' + CHIPSETREDIRECTION + '/build/SConscript'
else :
   print "Directory: [%s] doesnot exist"%script_sconfname
   raise RuntimeError, "mdm9205  [%s] does not exist"%script_sconfname

#-------------------------------------------------------------------------------
# Load sub scripts
#-------------------------------------------------------------------------------
script_scon = None
script_scon = '../' + CHIPSETREDIRECTION + '/build'

env.SConscript(dirs=[script_scon], exports='env')
