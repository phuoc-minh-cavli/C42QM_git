""" 
Copyright (c) 2019 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
"""

import SCons
import os

def exists(env):
  return True
    
def generate(env):
  env.AddMethod(legacy_syscall_is_enabled,'LegacySyscallIsEnabled')
  env.AddMethod(kmod_service_is_enabled,'KmodServiceIsEnabled')
  env.AddMethod(cclient_env_service_is_enabled,'CClientEnvServiceIsEnabled')
  env.AddMethod(feature_is_enabled,'FeatureIsEnabled')
  
  env.AddMethod(add_feature_sources,'AddFeatureSources')
  env.AddMethod(add_feature_library,'AddFeatureLibrary')
  env.AddMethod(add_kmod_service_sources,'AddKmodServiceSources')
  env.AddMethod(add_kmod_service_library,'AddKmodServiceLibrary')
  env.AddMethod(add_legacy_syscall_sources,'AddLegacySyscallSources')
  env.AddMethod(add_legacy_syscall_library,'AddLegacySyscallLibrary')
  env.AddMethod(add_cclient_env_sources,'AddCClientEnvSources')
  env.AddMethod(add_cclient_env_library,'AddCClientEnvLibrary')
  
  env.AddMethod(uid_to_class_num,'UIDToClassNum')
  env.AddMethod(add_bool_config_var,"AddBoolConfigVar")
  env.AddMethod(add_config_vars,"AddConfigVars")
  env.AddMethod(add_commandline_argument,"AddCommandlineArgument")
  
  env.AddMethod(bool_variable,'BoolVariable')
  env.AddMethod(enum_variable,'EnumVariable')
  env.AddMethod(list_variable,'ListVariable')
  env.AddMethod(package_variable,'PackageVariable')
  env.AddMethod(path_variable,'PathVariable')
  
      
      
def _is_enabled(env, feature, all_list_name, disabled_list_name):
  for l in [all_list_name,disabled_list_name]:
    if not l in env:
      print "WARNING! {} is not in the env; cannot determine feature enablement. Defaulting to feature {} is enabled (True)".format(l, feature)
      return True
  
  if feature in env[all_list_name]:
    if feature not in env[disabled_list_name]:
      return True
    return False
  print "WARNING! {} is not in env['{}']; defaulting to feature is enabled (True)".format(feature,all_list_name)
  return True
      
def legacy_syscall_is_enabled(env, legacy_syscall):
  return _is_enabled(env, legacy_syscall, 'all_legacy_syscalls', 'disabled_legacy_syscalls')
  
def cclient_env_service_is_enabled(env, cclient_env_service):
  cclient_env_service = long_uid_to_short_name(env, cclient_env_service)
  return _is_enabled(env, cclient_env_service, 'all_cclient_env_services', 'disabled_cclient_env_services')
  
def kmod_service_is_enabled(env, kmod_service):
  kmod_service = long_uid_to_short_name(env, kmod_service)
  return _is_enabled(env, kmod_service, 'all_kmod_services', 'disabled_kmod_services')
      
def feature_is_enabled(env, feature):
  return _is_enabled(env, feature, 'all_features', 'disabled_features')
  
def add_feature_sources(env, images, feature, sources, stubs):
  s = sources if env.FeatureIsEnabled(feature) else stubs
  env.AddBinaryObject(images, s)
    
def add_feature_library(env, images, libpath, feature, sources, stubs):
  s = sources if env.FeatureIsEnabled(feature) else stubs
  env.AddBinaryLibrary(images, libpath, s)

def add_kmod_service_sources(env, images, kmod_service, sources, stubs):
  s = sources if env.KmodServiceIsEnabled(kmod_service) else stubs
  env.AddBinaryObject(images, s)
    
def add_kmod_service_library(env, images, libpath, kmod_service, sources, stubs):
  s = sources if env.KmodServiceIsEnabled(kmod_service) else stubs
  env.AddBinaryLibrary(images, libpath, s)

def add_legacy_syscall_sources(env, images, legacy_syscall, sources, stubs):
  s = sources if env.LegacySyscallIsEnabled(legacy_syscall) else stubs
  env.AddBinaryObject(images, s)
    
def add_legacy_syscall_library(env, images, libpath, legacy_syscall, sources, stubs):
  s = sources if env.LegacySyscallIsEnabled(legacy_syscall) else stubs
  env.AddBinaryLibrary(images, libpath, s)

def add_cclient_env_sources(env, images, cclientenv_service, sources, stubs):
  s = sources if env.CClientEnvServiceIsEnabled(cclientenv_service) else stubs
  env.AddBinaryObject(images, s)
    
def add_cclient_env_library(env, images, libpath, cclientenv_service, sources, stubs):
  s = sources if env.CClientEnvServiceIsEnabled(cclientenv_service) else stubs
  env.AddBinaryLibrary(images, libpath, s)



def uid_to_class_num(env,uid_str):

  uid_str = long_uid_to_short_name(env,uid_str)

  if 'IDL_CLASS_MAP' not in env['ROOT_GLOBAL_ENV']:
    sys.path.insert(0, env.subst('${BUILD_ROOT}/ssg/bsp/build/scripts'))
    from SecureAppMetadata import IDLScanner
    idl_class_ids = IDLScanner().create_class_id_map(env)
    env['ROOT_GLOBAL_ENV']['IDL_CLASS_MAP'] = idl_class_ids
    
  idl_class_ids = env['ROOT_GLOBAL_ENV']['IDL_CLASS_MAP']
  return idl_class_ids[uid_str]
  
#this can receive either long form (e.g. 'CApp_UID') uid strings or short for
# (e.g. just 'app')
def long_uid_to_short_name(env,uid_str):
  if uid_str.startswith('C'):
    uid_str = uid_str[1:]
  return uid_str.lower().replace('_uid','')
  
  
class ConfigVar:
  def __init__(self, var, enable_map, disable_map, enable_if):
    self.var = var
    self.enable_map = enable_map
    self.disable_map = disable_map
    self.enable_if = enable_if
    
  def unpack(self):
    return self.var, self.enable_map, self.disable_map, self.enable_if


def add_bool_config_var(env,
                        config_name,
                        help=None,
                        default_value=None,
                        enable_map=None,
                        disable_map=None,
                        enable_if=None):
  if enable_map is None:
    enable_map = {}
  if disable_map is None:
    disable_map = {}
  if help is None:
    help = ""
  var = env.BoolVariable(config_name, help, default_value)
  return ConfigVar(var, enable_map, disable_map, enable_if)
                        
#files can contain either raw strings of paths to config files, or tuples of (str,bool)
#where tuple[0] is the path to the config file and tuple[1] is a bool designating whether
#or not the file is required. If the file is required and doesnt exist, we error out. If 
#the file is not required and doesnt exist, we just print a warning
def add_config_vars(env, files, args):
  files_processed = []
  for f in files:
    required = True
    if isinstance(f,tuple):
      path, required = f
    else:
      path = f
    path = env.RealPath(path)
    if not env.SconsFileExists(path):
      if required:
        raise ValueError('ERROR: cannot open config file {}'.format(path))
      else:
        print 'WARNING: cannot open config file {}'.format(path)
    files_processed.append(path)
    
  files_processed = [env.RealPath(f) for f in env.Flatten(files_processed)]
  variables = SCons.Script.Variables(files_processed, args=SCons.Script.ARGUMENTS)
  for arg in args:
    if isinstance(arg,ConfigVar):
      var = arg.var
    else:
      var = arg
    variables.Add(var)
  variables.Update(env)
  for arg in args:
    #at this point, there isn't anything to do for vanilla scons options. they've already been 
    #loaded into the env and that's all that they can do
    if isinstance(arg, ConfigVar):
      var, enable_map, disable_map, enable_if = arg.unpack()
      config_name = var[0]
      if callable(enable_if):
        if not enable_if(env):
          env[config_name] = False
      if env.get(config_name):
        env.Append(**enable_map)
      else:
        env.Append(**disable_map)
    
def add_commandline_argument(env, variable):
  vars = SCons.Script.Variables(args=SCons.Script.ARGUMENTS)
  vars.Add(variable)
  vars.Update(env)
  #if the default value for variable is None and the variable isnt specified on the commandline
  #scons just wont add it to the env. That doesn't make any sense, so let's fix that here
  default_val = variable[2]
  key = variable[0]
  if default_val is None and key not in env:
    env[key] = None
  
def bool_variable(env, key, help, default):
  return SCons.Variables.BoolVariable(key, help, default)
  
def enum_variable(env, key, help, default, allowed_values, map={}, ignorecase=0):
  return SCons.Variables.EnumVariable(key, help, default, allowed_values, map, ignorecase)
  
def list_variable(env, key, help, default, names, map={}):
  return SCons.Variables.ListVariable(key, help, default, names, map)
  
def package_variable(env, key, help, default, searchfunc=None):
  return SCons.Variables.PackageVariable(key, help, default, searchfunc)
  
def path_variable(env, key, help, default, validator=None):
  return SCons.Variables.PathVariable(key, help, default, validator)
  
  