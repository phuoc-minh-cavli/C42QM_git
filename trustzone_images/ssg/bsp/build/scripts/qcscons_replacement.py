#Copyright (c) 2018 Qualcomm Technologies, Inc.
#All Rights Reserved.
#Confidential and Proprietary - Qualcomm Technologies, Inc.

from __future__ import print_function

import SCons
import os
import SCons.Script
import sys
import platform


  
def dummy(*args,**kwargs):
  pass

def exists(env):
  return True
 
def is_key_enable(env, keys):
  keys = [keys] if not isinstance(keys,list) else keys
  return len(set(keys).intersection(set(env.get('IMAGE_BUILD_TAGS',[])))) > 0
 
def check_alias(env, alias_list):
  return len(set(alias_list).intersection(set(SCons.Script.COMMAND_LINE_TARGETS))) > 0
  
def init_image_vars(env, alias_list, proc, config='none', plat='l4',
   buildpath=None, flashtype='nand', target='${IMAGE_NAME}_${BUILD_ID}',
   build_kcfg=None, build_tags=[], deprecated_build_tags=[], tools=[]):
   
  env['PROC'] = proc
  env.Replace(IMAGE_ALIASES=alias_list)
  env.Replace(IMAGE_BUILD_TAGS=build_tags)
  env.Replace(BUILDPATH=env.subst(alias_list[0]).lower() + "/${PROC}/${BUILD_ID}")
  for t in build_tags:
    env[t] = True
    
def init_build_config(*args, **kwargs):
  pass
  
#we can't call Entry() on a path that may not exist, as scons will intern it and assume
# that it actually does exist, causing things like env.Glob to detect the non existant entry
def sconstruct_relative_to_abs(env,path):
  path = path.strip()
  path = env.subst(path)
  if path.startswith('#'):
    path = path[1:]
    if path.startswith('/'):
      path = path[1:]
    elif path.startswith('\\'):
      path = path[2:]
    path = os.path.join(env.Dir('#').get_abspath(),path)

  return path

#===============================================================================
# real_path
# we actually use os.path.abspath instead of os.path.realpath. The only
# functional difference is that realpath resolves symlinks and takes much, much
# longer. We don't care about resolving symlinks to the real path; the symlink
# works just fine for our purposes
#===============================================================================
def real_path(env, paths, posix=False):
  env_subst = lambda x: sconstruct_relative_to_abs(env,x)
  ospath_realpath = os.path.abspath

  if env.get('PLATFORM', 'none') == 'cygwin':
    posix = True

  if type(paths) is list:
    paths = [env_subst(path) for path in paths]
    paths = [ospath_realpath(path) for path in paths]
    
    if posix:
      paths = [path.replace('\\', '/') for path in paths]
  else:
    paths = env_subst(paths)
    paths = ospath_realpath(paths)
    
    if posix:
      paths = paths.replace('\\', '/')

  return paths

def path_exists(env, path):
   path = env.RealPath(path)
   return os.path.exists(path)
  
def get_builder_action (env, action, action_text = 'Generating ', action_target="TARGET", action_source="SOURCE"):
  action_target_name_str = "None"

  if action_target is not None:
    action_target_name_str = "${%s.name}" % action_target

  verbose_str = "=== %s ${BUILDPATH}/%s" % (action_text, action_target_name_str)

  act = SCons.Action.Action(action, verbose_str)
  return act
  
def load_tool_script(env, tool, toolpath=None):
  if toolpath is None:
    toolpath = []
  if not isinstance(toolpath,list):
    toolpath = [toolpath]

  if env.IsFile(tool):
    toolpath.append(os.path.dirname(tool))
    tool = os.path.splitext(os.path.basename(tool))[0]
    
  env.Tool(tool, toolpath=toolpath)
    
  return None
   
def is_dir(env, path):
  path = env.RealPath(path)
  return os.path.isdir(path)
   
def is_file(env, path):
  path = env.RealPath(path)
  return os.path.isfile(path)
   
def print_info(env, message, raw=True):
  print('** INFO: ' + env.subst(str(message), raw))
  
def print_debug_info(env, debuginfo_type, message, raw=True):
  debuginfo_type = str(debuginfo_type)
  debuginfo_opt = env.get('DEBUGINFO_OPT',[])

  if debuginfo_type in debuginfo_opt:
    print ('** DBGINF: ' + env.subst(str(message), raw))
  
def print_warning(env, message, width=80, raw=True):
   if not env.get('DISABLE_BUILD_WARNINGS', False):
      print ('!! WARNING: ' + env.subst(str(message), raw))
  
def generate(env):
  env.AddMethod(real_path,"RealPath")
  env.AddMethod(is_file,'IsFile')
  env.AddMethod(is_dir,'IsDir')
  if env.StandaloneSdk():
    chipset= SCons.Script.ARGUMENTS['CHIPSET']
    os_name = 'lnx' if platform.system().startswith('Linux') else 'win'
    cfg_files = ['$SDK_ROOT/config/sdk_config_{}.cfg'.format(os_name),'$SDK_ROOT/config/sdk_config_' + chipset + '.cfg']
    #For things like sm8150 /sdm855, we need to unmangle the correct CHIPSET from the RAW_CHIPSET. the config fine has these
    #correct, but scons arguments override that. so delete the CHIPSET argument just for now, and readd it in case we need it later
    del SCons.Script.ARGUMENTS['CHIPSET']
    BAD_VALUE = 'BAD_VALUE_REPLACE_ME_IN_CONFIG_FILE'
    BAD_PATH = '/BAD/PATH/REPLACE/ME/IN/CONFIG/FILE'
    env.AddConfigVars(cfg_files,
    [
      ('CLANG_VERSION',     'clang version',         BAD_VALUE),
      ('GNUARM7',           'gnu arm 7',             BAD_PATH),
      ('GNULIBC',           'gnu libc path',         BAD_PATH),
      ('GNUPATH',           'gnu path',              BAD_PATH),
      ('GNUROOT',           'gnu root',              BAD_PATH),
      ('GNUTOOL',           'gnu tool path',         BAD_PATH),
      ('LLVM32INC',         'llvm32inc path',        BAD_PATH),
      ('LLVMBIN',           'llvm bin path',         BAD_PATH),
      ('LLVMCLANG',         'clang path',            BAD_PATH),
      ('LLVMINC',           'llvm inc path',         BAD_PATH),
      ('LLVMLIB',           'llvm lib path',         BAD_PATH),
      ('LLVMLIBPROP',       'linux-propri_rt path',  BAD_PATH),
      ('LLVMROOT',          'llvm root path',        BAD_PATH),
      ('LLVMTOOLPATH',      'llvm tools path',       BAD_PATH),
      ('LLVM_VERSION',      'llvm version',          BAD_VALUE),
      ('MAKE_PATH',         'make path',             BAD_PATH),
      ('MUSL32PATH',        'musl32 path',           BAD_PATH),
      ('MUSLPATH',          'musl path',             BAD_PATH),
      ('PYTHON_PATH',       'python binary path',    BAD_PATH),
      ('BUILD_ASIC',        'build asic',            BAD_VALUE),
      ('BUILD_ID',          'build id',              BAD_VALUE),
      ('BUILD_VER',         'build version',         BAD_VALUE),
      ('CHIPSET',           'chipset',               BAD_VALUE),
      ('HAL_PLATFORM',      'hal platform',          BAD_VALUE),
      ('MAX_NUM_ROOT_CERTS','max num root certs',    BAD_VALUE),
      ('MSM_ID',            'msm id',                BAD_VALUE),
      ('RAW_CHIPSET',       'raw chipset',           BAD_VALUE),
      ('SECIMAGE_CONFIG',   'secimage config',       BAD_VALUE),
      ('SOC_HW_VERSION',    'soc hw version',        BAD_VALUE),
      ('SOC_VERS',          'soc vers',              BAD_VALUE),
      ('SOFTWARE_IMAGE',    'software image',        BAD_VALUE),
      ('TARGET_FAMILY',     'target family',         BAD_VALUE),
      ('TZ_EXEC_MODE',      'tz exec mode',          BAD_VALUE),
      ('SECIMAGE_CONFIG_QTI','SECIMAGE_CONFIG_QTI',  '${SECIMAGE_CONFIG}'),
    ])
    SCons.Script.ARGUMENTS['CHIPSET'] = chipset
  else:
    for k,v in SCons.Script.ARGUMENTS.items():
      env[k] = v
    env.Replace(SECIMAGE_CONFIG_QTI=SCons.Script.ARGUMENTS.get("SECIMAGE_CONFIG_QTI", env["SECIMAGE_CONFIG"])) 
    
  env['SHORT_BUILDPATH'] = env['BUILD_ID']
  env['BUILD_ID'] = env['BUILD_ID'] + 'A'
  env['CUST_H'] = env.subst('cust${BUILD_ID}.h').lower()
  env['USES_PLATFORM_IMAGE_INTEGRITY_CHECK'] = True
  
  env['MBN_ROOT'] = '${BUILD_ROOT}/build/ms/bin/${SHORT_BUILDPATH}/'

  env.Append(CPPPATH=['$BUILD_ROOT/build/cust','$BUILD_ROOT/build/ms'])
  
  #NOTE: None of these options actually do anything for apps builds. They're
  #here only for qcscons compatibility and are just stubs
  try:
    SCons.Script.AddOption('--callgraph',
      dest='callgraph',
      action="store_true",
      default=False,
      help='Create call graphs on supported linkers only.')

    SCons.Script.AddOption('--verbose',
      dest='verbose',
      type='int',
      nargs=1,
      default='0',
      help='Verbose level 3=raw (no formatting), 2=detailed, 1=limited, 0=off')

    SCons.Script.AddOption('--preprocess',
      dest='preprocess',
      action="store_true",
      default=False,
      help='Generate C-preprocess file *.o.pp for any compiled file')

    SCons.Script.AddOption('--mixasm',
      dest='mixasm',
      action="store_true",
      default=False,
      help='Generate mixed assembly/source file *.o.mix for any compiled file')

    SCons.Script.AddOption('--hdeptree',
      dest='hdeptree',
      action="store_true",
      default=False,
      help='Generate header dependency tree file *.dep for any compiled file')

    SCons.Script.AddOption('--cleanpack',
      dest='cleanpack',
      action="store_true",
      default=False,
      help='Perform special clean processing for Pack build')
  except:
    pass
  env.Replace(USES_MBNTOOLS = True)
  env.Replace(GLOBAL_DICT = {1:1})
  env.Replace(USES_SEC_POLICY_MULTIPLE_DEFAULT_SIGN=True)
  env.Replace(USES_SEC_POLICY_STEP1_QC_SIGN=True)
  env.Replace(USES_SEC_POLICY_STEP2_OEM_SIGN=True)

  env.Replace(INC_PATH=env['BUILD_ROOT'])
  env.Replace(INC_ROOT=env['BUILD_ROOT'])
  if 'ROOT_GLOBAL_ENV' not in env:
    env['ROOT_GLOBAL_ENV'] = env
  env['INSTALL_LIBPATH'] = ''
  
  
  if env.StandaloneSdk():
    env['BUILD_SCRIPTS_ROOT'] = '${SDK_ROOT}/scripts/'
    env.Tool('formatting',toolpath=['${SDK_ROOT}/scripts'])
  else:
    env['BUILD_SCRIPTS_ROOT'] = '${BUILD_ROOT}/tools/build/scons/scripts/'
    env.Tool('formatting',toolpath=['${BUILD_ROOT}/ssg/bsp/build/scripts'])
  
  env['PYTHONCMD'] = 'python'  
  env['PYTHON_PATH'] = env.get('PYTHON_PATH',os.environ.get('PYTHON_PATH'))
  
  env.AddMethod(load_tool_script,'LoadToolScript')
  
  env.AddMethod(print_info,"PrintInfo")
  env.AddMethod(print_warning,"PrintWarning")
  env.AddMethod(print_debug_info,"PrintDebugInfo")
  env.AddMethod(path_exists,"PathExists")
  
  env.AddMethod(real_path,"SubstRealPath")
  env.AddMethod(get_builder_action,"GetBuilderAction")
  env.AddMethod(dummy,'CleanPack')
  env.AddMethod(dummy, 'RequirePublicApi')
  env.AddMethod(dummy, 'RequirePrivateApi')
  env.AddMethod(dummy, 'RequireProtectedApi')
  env.AddMethod(dummy, 'RequireRestrictedApi')
  env.AddMethod(dummy, 'PublishPrivateApi')
  env.AddMethod(dummy, 'AddBinaryLibrary')
  env.AddMethod(dummy, 'AddBinaryObject')
  env.AddMethod(dummy, 'LoadSoftwareUnits')
  
  
  env.AddMethod(is_key_enable, 'IsKeyEnable')
  
  env.AddMethod(init_image_vars, 'InitImageVars')
  env.AddMethod(init_build_config, 'InitBuildConfig')
  env.AddMethod(check_alias,'CheckAlias')
  
  if sys.platform.startswith('win'):
    class ourSpawn:
      def ourspawn(self, sh, escape, cmd, args, env):
        import subprocess
        args = ' '.join(args[1:])
        cmdline = ' '.join([cmd, args])
        startupinfo = subprocess.STARTUPINFO()
        startupinfo.dwFlags |= subprocess.STARTF_USESHOWWINDOW
        proc = subprocess.Popen(cmdline, stdin=subprocess.PIPE, stdout=subprocess.PIPE,
            stderr=subprocess.PIPE, startupinfo=startupinfo, shell = False, env = env)
        data, err = proc.communicate()
        rv = proc.wait()
        if rv:
          print(err)
        return rv

    buf = ourSpawn()
    buf.ourenv = env
    env['SPAWN'] = buf.ourspawn
  
