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
#
# $Header: //components/rel/build.tn/4.0/ms/tbc_core_devcfg.py#1 $
#
# when       who    what, where, why
# -------    ---    -------------------------------------------
# 09/03/10   dlb'   Initial Revision
#==============================================================================

def generate(env):
   #---------------------------------------------------------------------------
   # Add Device Configuration specific defines
   #---------------------------------------------------------------------------
   # Reason for doing this is because python does 
   # not like the 'x' in targets like '9x15'
   if env['MSM_ID'] in ['9x25']:
      env.Replace(DAL_CONFIG_TARGET_ID = '0x9025')   
   elif env['MSM_ID'] in ['9x35']:
      env.Replace(DAL_CONFIG_TARGET_ID = '0x9035')  
   elif env['MSM_ID'] in ['9x45']:
      env.Replace(DAL_CONFIG_TARGET_ID = '0x9045')      
   elif env['MSM_ID'] in ['9x55']:
       env.Replace(DAL_CONFIG_TARGET_ID = '0X9055')
   elif env['MSM_ID'] in ['9x06']:
       env.Replace(DAL_CONFIG_TARGET_ID = '0X9206')
   else:
      env.Replace(DAL_CONFIG_TARGET_ID = '0x${MSM_ID}')

   env.Append(CPPDEFINES = [
      "DAL_CONFIG_IMAGE_APPS",
      "DAL_CONFIG_TARGET_ID=${DAL_CONFIG_TARGET_ID}"
   ])

def exists(env):
    return env.Detect('tbc_core_devcfg')
