#============================================================================
#
# CBSP Builders build rules
#
# GENERAL DESCRIPTION
#    Contains builder(s) to build a Secure App
#
# Copyright 2015-2017 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#----------------------------------------------------------------------------
#
#  $Header: //components/rel/ssg.tz/1.1.3/bsp/build/scripts/get_dependency_builder.py#1 $
#  $DateTime: 2019/05/23 05:32:51 $
#  $Author: pwbldsvc $
#  $Change: 19292046 $
#
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who       what, where, why
# --------   ---       ------------------------------------------------------
#
#============================================================================
import os

def exists(env):
  True

def generate(env):
  env.AddMethod(get_ta_dependency,    "GetTADependencies")
  env.AddMethod(get_commonlib_mbn,    "GetCommonlibMBN")
  env.AddMethod(get_ta_linker_script, "GetTALinkerScript")
  env.AddMethod(get_qtee_emu_libs,    "GetQteeEmuLibs")

def get_ta_dependency(env):
  if env.StandaloneSdk():
    applib = [env.File('${SDK_ROOT}/libs/${APP_EXEC_MODE}/applib.lib')]
    cmnlib = [env.File('${SDK_ROOT}/libs/${APP_EXEC_MODE}/libcmnlib.so')]
    appobj = [env.File('${SDK_ROOT}/libs/${APP_EXEC_MODE}/common_applib.o')]
  else:
    applib = [env.File('${BUILD_ROOT}/ssg/bsp/trustzone/qsapps/applib/build/${SHORT_BUILDPATH}/${PROC}/applib.lib')]
    cmnlib = [env.File('${BUILD_ROOT}/ssg/bsp/trustzone/qsapps/commonlib/build/${SHORT_BUILDPATH}/${PROC}/libcmnlib.so')]
    appobj = [env.File('${BUILD_ROOT}/ssg/bsp/trustzone/qsapps/applib/build/${SHORT_BUILDPATH}/${PROC}/common_applib.o')]

  #these files are distributed as binaries with the SDK, so we definitely don't want to
  #accidentally delete them when cleaning
  if env.StandaloneSdk():
    env.NoClean([applib,cmnlib,appobj])
    env.Precious([applib,cmnlib,appobj])

  return applib + cmnlib, appobj

def get_commonlib_mbn(env):
  # offtarget testing won't generate a cmnlib mbn
  if env['PROC'] == 'scorpion':
    cmnlib_mbn = '${BUILD_ROOT}/build/ms/bin/${SHORT_BUILDPATH}/cmnlib.mbn'
  elif env['PROC'] == 'A53_64':
    cmnlib_mbn = '${BUILD_ROOT}/build/ms/bin/${SHORT_BUILDPATH}/cmnlib64.mbn'


def get_ta_linker_script(env):
  root_path = '${SDK_ROOT}/scripts/' if env.StandaloneSdk() else '${BUILD_ROOT}/ssg/bsp/build/scripts/'
  link_script = 'secureapp.ld' if env['APP_EXEC_MODE'] == 'aarch32' else 'secureapp64.ld'

  return os.path.join(root_path, link_script)

def get_qtee_emu_libs(env, test_svc_enabled):

  if env.StandaloneSdk():
    path = '${SDK_ROOT}/libs/${APP_EXEC_MODE}/'
    cmnlib_path = path
  else:
    sdk_version = 'sdk32offtarget' if env['PROC'] in ['x86-32'] else 'sdk64offtarget'
    test_variant = 'test_enabled' if test_svc_enabled else 'test_disabled'

    path = ('${BUILD_ROOT}/ssg/bsp/sdk/QTEEEmu/build/' +
            sdk_version +
            '/TZ_SDK_INTERNAL/' +
            test_variant +
            '/${PROC}/${BUILD_ID}/')

    cmnlib_path = '${BUILD_ROOT}/ssg/bsp/trustzone/qsapps/commonlib/build/${SHORT_BUILDPATH}/${PROC}/'

  cmnlib_support = [env.File(cmnlib_path + 'libcmnlibsupport.so')]
  client_apis    = [env.File(path        + 'offtarget_client_apis.lib')]
  mink           = [env.File(path        + 'offtargetmink.lib')]
  support        = [env.File(path        + 'offtargetsupport.lib')]

  #these files are distributed as binaries with the SDK, so we definitely don't want to
  #accidentally delete them when cleaning
  if env.StandaloneSdk():
    env.NoClean([cmnlib_support,client_apis,mink,support])
    env.Precious([cmnlib_support,client_apis,mink,support])

  return cmnlib_support + client_apis + mink + support, cmnlib_path
