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
#  $Header: //components/rel/core.mpss/3.10/securemsm/cryptodrivers/prng/chipset/build/mdm9607.SConscript#1 $
#  $DateTime: 2018/06/26 03:18:17 $
#  $Author: pwbldsvc $
#  $Change: 16502877 $
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
CHIPSETREDIRECTION = 'mdm9607'

script_sconfname = None
if os.path.exists('../' + CHIPSETREDIRECTION + '/build/SConscript') :
   script_sconfname = '../' + CHIPSETREDIRECTION + '/build/SConscript'
else :
   print "Directory: [%s] doesnot exist"%script_sconfname
   raise RuntimeError, "mdm9607  [%s] does not exist"%script_sconfname

#-------------------------------------------------------------------------------
# Load sub scripts
#-------------------------------------------------------------------------------
script_scon = None
script_scon = '../' + CHIPSETREDIRECTION + '/build'

env.SConscript(dirs=[script_scon], exports='env')
