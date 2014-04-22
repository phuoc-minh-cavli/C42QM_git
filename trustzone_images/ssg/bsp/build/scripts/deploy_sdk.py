import sys
import os
import re
from SCons.Script import ARGUMENTS, BUILD_TARGETS, COMMAND_LINE_TARGETS
import subprocess
import shlex
import shutil
import SCons

_deploy_common = None

def exists(env):
  return True

def dummy(*args, **kwargs):
  pass

def generate(env):
  env.AddMethod(remove_tree, "_RemoveTree")
  if env.StandaloneSdk() or ('deploy_sdk' not in BUILD_TARGETS and 'deploy_sdk' not in COMMAND_LINE_TARGETS):
    env.AddMethod(dummy, "DeploySdk")
    env.AddMethod(dummy, "DeploySdkLib")
    env.AddMethod(dummy, "DeploySdkHeader")
    env.AddMethod(dummy, "DeploySdkSample")
    env.AddMethod(dummy, "DeploySdkScript")
    env.AddMethod(dummy, "DeploySdkDoc")
    env.AddMethod(dummy, "DeploySdkDir")
    env.AddMethod(dummy, "VerifyDeploySdkBuilder")
    return

  sys.path.append(env.subst('${BUILD_ROOT}/build/ms'))
  from deploy_builder import _deploy_common as _d_c
  global _deploy_common
  _deploy_common = _d_c
  env.AddMethod(deploy_sdk, "DeploySdk")
  env.AddMethod(deploy_sdk_lib, "DeploySdkLib")
  env.AddMethod(deploy_sdk_header, "DeploySdkHeader")
  env.AddMethod(deploy_sdk_sample, "DeploySdkSample")
  env.AddMethod(deploy_sdk_script, "DeploySdkScript")
  env.AddMethod(deploy_sdk_doc, "DeploySdkDoc")
  env.AddMethod(deploy_sdk_dir, "DeploySdkDir")
  env.AddMethod(deploy_sdk_idl, "DeploySdkIdl")

  bld = env.Builder(action=verify_deploy_sdk_builder)
  env.Append(BUILDERS={"VerifyDeploySdkBuilder":bld})

import traceback
#Testing the internal and external variants of the SDK is a bit different since they ahve different goals
#The external variant is meant to be called externally with a given TA sconscript running the build. We
#supply the external variant just as one folder, the equivalent of qtee_tas/sdk/latest/external from the
#TZAPPS image
#The Internal SDK is quite different, as it is embedded into the TZ APPS image. we need to be able to test
#the internal sdk as part of that layout in order to actually run tests that actually test internal sdk 
#functionality correctly. So we need to layout the entire TZ APPS image within the deploy directory.
#The layout of the TZ APPS image also calls for having the external sdk located under qtee_tas/sdk/latest/external
#But OEMs get it from deploy$BUILD_ID/TZ_SDK_EXTERNAL. We can do 2 things:
#deploy it to just deploy${BUILD_ID}/TZ_SDK_EXTERNAL and force whomever updates ssg.qteesdk.1.0 to copy it to qtee_tas/sdk/latest/external
#or just deploy it there in addition to deploy${BUILD_ID}
#the 1st option sounds easier and less error prone, so let's do it here
def deploy_sdk(env, deploy_list, **kwargs):
  #Don't deploy the test variant.
  if env.get('tzbsp_with_test_svc'):
    return

  ret = []
  
  v = ARGUMENTS.get('deploy_variant')
  if isinstance(v,list):
    v = v[0]
  
  variants = []
  ARGUMENTS_backup = ARGUMENTS.get('deploy_variant')
  if kwargs.get('internal_only'):
    variants = ['TZ_APPS_IMAGE']
  elif kwargs.get('external_only'):
    variants = ['TZ_SDK_EXTERNAL']
  elif v not in ['TZ_SDK_EXTERNAL','TZ_APPS_IMAGE']:
    variants = ['TZ_SDK_EXTERNAL','TZ_APPS_IMAGE']

  kwargs['skip_check'] = False
  kwargs['deploy_target'] = 'deploy_sdk'
  dst_path = kwargs.get('dst_path')

  for v in variants:
    ARGUMENTS['deploy_variant'] = [v]
    if v == 'TZ_SDK_EXTERNAL':
      kwargs['path_map'] = {'dst' : dst_path }
      ret.append(_deploy_common(env, deploy_list, ['TZ_SDK_EXTERNAL'], **kwargs))
    elif v == 'TZ_APPS_IMAGE':
      kwargs['path_map'] = {'dst' : os.path.join('qtee_tas/sdk/latest/internal',dst_path)}
      ret.append(_deploy_common(env, deploy_list, ['TZ_APPS_IMAGE'], **kwargs))

  
  try:
    ARGUMENTS['deploy_variant'] = ARGUMENTS_backup
  except:
    pass
  return ret


def deploy_sdk_lib(env, deploy_list, **kwargs):
  dst_path = env.subst(os.path.join('libs', kwargs.get('subdir',''), '${APP_EXEC_MODE}'))
  return env.DeploySdk(deploy_list, dst_path=dst_path, **kwargs)

def deploy_sdk_header(env, deploy_list, **kwargs):
  dst_path = env.subst(os.path.join('inc', kwargs.get('subdir','')))
  return env.DeploySdk(deploy_list, dst_path=dst_path, **kwargs)

def deploy_sdk_sample(env, deploy_list, **kwargs):
  dst_path = env.subst(os.path.join('samples', kwargs.get('subdir','')))
  return env.DeploySdk(deploy_list, dst_path=dst_path, **kwargs)

def deploy_sdk_script(env, deploy_list, **kwargs):
  dst_path = env.subst(os.path.join('scripts', kwargs.get('subdir','')))
  return env.DeploySdk(deploy_list, dst_path=dst_path, **kwargs)

def deploy_sdk_doc(env, deploy_list, **kwargs):
  dst_path = env.subst(os.path.join('docs', kwargs.get('subdir','')))
  return env.DeploySdk(deploy_list, dst_path=dst_path, **kwargs)

def deploy_sdk_idl(env, deploy_list, **kwargs):
  dst_path = env.subst(os.path.join('inc/idl', kwargs.get('subdir','')))
  return env.DeploySdk(deploy_list, dst_path=dst_path, **kwargs)

  
def deploy_sdk_dir(env, path, **kwargs):
  path = env.subst(path)
  dst_path = kwargs.get('dst_path','')
  del kwargs['dst_path']
  if 'source_only' in kwargs:
    check_should_deploy = lambda x: x.endswith(('.c','.h','.py','.scons','SConscript','SConstruct','.idl'))
  elif 'header_only' in kwargs:
    check_should_deploy = lambda x: x.endswith('.h')
  else:
    check_should_deploy = lambda x: all(['.git' not in x, '.pyc' not in x, env.subst('A53_64/${BUILD_ID}') not in x, env.subst('scorpion/${BUILD_ID}') not in x])
  for root, dirnames, filenames in os.walk(path):
    subdir = root.replace(path,'').strip('/').strip('\\')
    for filename in filenames:
      fpath = os.path.join(root,filename)
      if check_should_deploy(fpath):
        env.DeploySdk(fpath,dst_path=os.path.join(dst_path,subdir),**kwargs)


def remove_tree(tree_root):
  if sys.platform.startswith('win'):
    os.chmod(tree_root, stat.S_IWRITE)
    for (path, dname_list, fname_list) in os.walk(tree_root, followlinks=True):
      for fname in fname_list:
        os.chmod(os.path.join(path, fname), stat.S_IWRITE)
      for dname in dname_list:
        os.chmod(os.path.join(path, dname), stat.S_IWRITE)
  shutil.rmtree(tree_root)



def verify_deploy_sdk_builder(env, target, source):
  deploy_root = env.GetDeployRoot()
  deploy_verify = os.path.basename(deploy_root) + '_verify_tz_sdk'
  deploy_verify_root = os.path.join(os.path.dirname(deploy_root), deploy_verify)
  print 'deploy_verify_root',deploy_verify_root

  print('Copying the Deploy Tree for Verification from {} to {}'.format(deploy_root, deploy_verify_root))
  #Create a copy of the deploy tree to run verification checks in that tree
  #this way we dont pollute the deploy tree
  #source MUST be a temp file in the root of the sdk deployment

  #Internal sdk verification, only when specified
  print "verifying internal sdk"
  tzapps_image_root = os.path.join(deploy_root,'TZ_APPS_IMAGE')
  external_sdk_root = os.path.join(deploy_root,'TZ_SDK_EXTERNAL')
  
  tzapps_image_verify_root = os.path.join(deploy_verify_root,'TZ_APPS_IMAGE')
  external_sdk_verify_root = os.path.join(deploy_verify_root,'TZ_SDK_EXTERNAL')

  #scons is dumb about dependencies, so we do a file deploy trick which creates temp.temp at the sdk root. let's delete that here
  temp_files = [
    os.path.join(external_sdk_root,'temp.temp'),
    os.path.join(tzapps_image_root,'qtee_tas/sdk/latest/internal/temp.temp'),
  ]
  for temp in temp_files:
    temp = env.RealPath(temp)
    if os.path.isfile(temp):
      os.remove(temp)
      
  #we need to strip all symbols from external builds as well. apparently adding -s to your linkflags doesn't delete _everything_
  artifacts = env.Glob(os.path.join(external_sdk_root,'libs/*/*.o'))
  artifacts.extend(env.Glob(os.path.join(external_sdk_root,'libs/*/*.so')))
  artifacts.extend(env.Glob(os.path.join(external_sdk_root,'libs/*/*.lib')))
  for p in env.Flatten(artifacts):
    p = env.RealPath(str(p))
    print 'stripping',p
    strip = 'strip' if 'x86' in p else '/pkg/qct/software/llvm/release/arm/8.0/bin/llvm-strip'
      
    cmd = strip + ' --strip-debug --strip-unneeded {}'.format(p)
    build_args = shlex.split(cmd)
    process = subprocess.Popen(build_args, stdout=sys.stdout, stderr=sys.stderr)
    stdout,stderr = process.communicate()
    ret = process.returncode
    if ret:
      print 'stripping {} during sdk verify step failed!'.format(p)
      sys.exit(1)
     
      
  #XXX: we are not shipping every public IDL with the external SDK. we could add some filters elsewhere to make sure we dont
  #deploy them, but doing that, then probably removing/not using it after some time is a lot more work than just 
  #deleting them here  
  extra_delete_files = [
    os.path.join(external_sdk_root,'inc/idl/IValidate.idl'),
    os.path.join(external_sdk_root,'inc/idl/CCertValidate.idl'),
  ]
  for p in extra_delete_files:
    p = env.RealPath(p)
    print "deleting",p,"from external sdk"
    os.remove(p)
    

  for root,verify_root in [[tzapps_image_root,tzapps_image_verify_root],[external_sdk_root,external_sdk_verify_root]]:
    if os.path.exists(verify_root):
      print('Deleting the Deploy sdk Tree for Verification {}'.format(verify_root))
      remove_tree(verify_root)

    shutil.copytree(root, verify_root)

  #List of {command, working directory} to run to verify the SDK
  
  verify_target = ARGUMENTS.get('deploy_sdk_target','skeleton')
  
  #The qtee sdk is one folder down within TZ_SDK_INTERNAL folder, but we need to copy/delete that whole tree so get sectools and apps copied correctly
  #so let's add the qteesdk part of the name back here
  tzapps_cwd = os.path.join(tzapps_image_verify_root,'qtee_tas')
  external_sdk_cwd = external_sdk_verify_root
  cmds = [
    # Build on-target skeleton  from internal build
    {
      'cmd' : 'python /pkg/ssg/scons/script/scons -f sdk/in_tree.scons -j8 CHIPSET={} {} '.format(env['RAW_CHIPSET'],verify_target),
      'cwd' : tzapps_cwd
    },]
    
  tzapps_cwd = os.path.join(tzapps_cwd,'sdk/latest/internal')
  for vfy_root in [tzapps_cwd,external_sdk_cwd]:
    
    cmds.extend([
    #Build the OFFTARGET skeleton
    {
      'cmd' : 'python /pkg/ssg/scons/script/scons CHIPSET={1} OFF_TARGET=1 QTEE_SDK={0} skeleton_ca -j8 '.format(vfy_root, env['RAW_CHIPSET']),
      'cwd' : os.path.join(vfy_root, 'samples', 'skeleton')
    },

    #Run the skeleton CA (both 32 and 64 bit)
    {
      'cmd' : 'find -name skeleton_ca.elf -exec {} \;',
      'cwd' : os.path.join(vfy_root, 'samples', 'skeleton')
    },

    #Build the OFFTARGET example_app
    {
      'cmd' : 'python /pkg/ssg/scons/script/scons CHIPSET={1} OFF_TARGET=1 QTEE_SDK={0} example_app_ca -j8 '.format(vfy_root, env['RAW_CHIPSET']),
      'cwd' : os.path.join(vfy_root, 'samples', 'example_app')
    },

    #Run the example_app CA (both 32 and 64 bit)
    {
      'cmd' : 'find -name example_app_ca.elf -exec {} \;',
      'cwd' : os.path.join(vfy_root, 'samples', 'example_app')
    },])
  

  for cmd in cmds:
    print cmd['cwd']
    print cmd['cmd']
    build_args = shlex.split(cmd['cmd'])
    process = subprocess.Popen(build_args,cwd=cmd['cwd'], stdout=sys.stdout, stderr=sys.stderr)
    stdout,stderr = process.communicate()
    ret = process.returncode
    if ret:
      print 'verify sdk building failed!'
      sys.exit(1)

