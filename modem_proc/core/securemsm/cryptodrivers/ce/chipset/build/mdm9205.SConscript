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
#  $Header: //components/rel/core.mpss/3.10/securemsm/cryptodrivers/ce/chipset/build/mdm9205.SConscript#2 $
#  $DateTime: 2019/02/20 04:46:10 $
#  $Author: pwbldsvc $
#  $Change: 18376863 $
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

# mdm9205 interface redirection
CHIPSETREDIRECTION = 'mdm9205'

script_sconfname = None
if os.path.exists('../' + CHIPSETREDIRECTION + '/build/SConscript') :
   script_sconfname = '../' + CHIPSETREDIRECTION + '/build/SConscript'
else :
   print "Directory: [%s] doesnot exist"%script_sconfname
   raise RuntimeError, "mdm9205  [%s] does not exist"%script_sconfname

DEL_FILES = [
    '${BUILD_ROOT}/core/securemsm/cryptodrivers/ce/chipset/build/mdm9607.SConscript',
    '${BUILD_ROOT}/core/securemsm/cryptodrivers/ce/chipset/mdm9607/build/SConscript',
]

env.CleanPack(['SINGLE_IMAGE', 'CBSP_SINGLE_IMAGE',
      'MODEM_IMAGE',  'CBSP_MODEM_IMAGE',
      'APPS_IMAGE',   'CBSP_APPS_IMAGE',
      'QDSP6_SW_IMAGE', 'CBSP_QDSP6_SW_IMAGE',
      'MBA_CORE_SW'
     ], DEL_FILES)

#-------------------------------------------------------------------------------
# Load sub scripts
#-------------------------------------------------------------------------------
script_scon = None
script_scon = '../' + CHIPSETREDIRECTION + '/build'

env.SConscript(dirs=[script_scon], exports='env')
