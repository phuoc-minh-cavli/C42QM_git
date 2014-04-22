
#
# CBSP Builders build rules
#
# GENERAL DESCRIPTION
#    Contains builder(s) to build a Secure App
#
# Copyright 2015 - 2019 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#----------------------------------------------------------------------------
#
#  $Header: //components/rel/ssg.tz/1.1.3/bsp/build/scripts/secure_app_builder.py#1 $
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
from __future__ import print_function
import os
import sys
import platform
import fnmatch
import hashlib
import pickle
import SCons.Script

#----------------------------------------------------------------------------
# Global values
#----------------------------------------------------------------------------
DEFAULT_APP_SPLITBIN_COUNT = 8
THIS_DIR = os.path.dirname(os.path.realpath(__file__))

STANDARD_INCLUDE_PATHS = [
  '${BUILD_ROOT}/core/api/kernel/libstd',
  '${BUILD_ROOT}/core/api/kernel/libstd/stringl/',
  '${BUILD_ROOT}/ssg/api/securemsm/trustzone/gp/',
  '${BUILD_ROOT}/ssg/api/securemsm/trustzone/qsee/',
  '${BUILD_ROOT}/ssg/api/securemsm/util',
  '${BUILD_ROOT}/ssg/securemsm/trustzone/qsee/include/',
  '${BUILD_ROOT}/ssg/securemsm/trustzone/qsee/mink/include/',
]

#TODO: kick out most of these and go fix each TA to pull from the correct directories
SDK_INC_PATHS = [
  '${SDK_ROOT}/inc',
  '${SDK_ROOT}/inc/gp',
  '${SDK_ROOT}/inc/qsee',
  '${SDK_ROOT}/inc/idl',
  '${SDK_ROOT}/inc/idl/private',
  '${SDK_ROOT}/inc/private',
  '${SDK_ROOT}/inc/util',
]

#----------------------------------------------------------------------------
# Hooks for Scons
#----------------------------------------------------------------------------
def exists(env):
  return True

def generate(env):
  secure_app_generate(env)

#---------
#Build helpers
#--------
def get_ta_out_dir(env):
  if env.get('TA_OUT_DIR'):
    return '${TA_OUT_DIR}/${SHORT_BUILDPATH}/${PROC}/'
  else:
    return '${OUT_DIR}/${SHORT_BUILDPATH}/${PROC}/'
      
def get_lib_out_dir(env):
  if env.get('LIB_OUT_DIR'):
    return '${LIB_OUT_DIR}/'
  else:
    return '${OUT_DIR}/'

#============================================================================
# Add the Secure App Builder to the environment. Also Load other builders
# that are needed for secure app compilation via LoadToolScript helper.
#============================================================================
def secure_app_generate(env):
  if 'SECURE_APP_BUILDER_ARCHIVE_OBJS' not in env:
    env.Replace(SECURE_APP_BUILDER_ARCHIVE_OBJS=True)

  env.AddMethod(secure_app_builder, "SecureAppBuilder")
  env.AddMethod(get_object_file_name, "GetObjectFileName")
  env.AddMethod(secure_app_lib_builder, "SecureAppLibBuilder")
  env.AddMethod(offtarget_client_app_builder, "OfftargetClientAppBuilder")
  env.AddMethod(offtarget_client_app_lib_builder, "OfftargetClientAppLibBuilder")



  if env.StandaloneSdk():
    if env.OfftargetTesting():
      toolchain = 'offtarget_toolchain.py'
    else:
      toolchain = 'llvm.py'
    tools = [
      os.path.join(THIS_DIR, toolchain),
      os.path.join(THIS_DIR, 'mbn_builder.py'),
      os.path.join(THIS_DIR, 'scl_builder.py'),
      os.path.join(THIS_DIR, 'get_dependency_builder.py'),
      os.path.join(THIS_DIR, 'image_environment.py'),
      os.path.join(THIS_DIR, 'secure_apps.py'),
      os.path.join(THIS_DIR, 'unittesting_apps.py'),
      '${SDK_ROOT}/scripts/sectools/sectools_builder.py',
    ]
    if 'USES_CLEAN_PACK' not in env:
      tools.append(os.path.join(THIS_DIR, 'pil_splitter_builder.py'))
  else:
    if env.OfftargetTesting():
      toolchain = '${BUILD_ROOT}/ssg/bsp/build/scripts/offtarget_toolchain.py'
    else:
      toolchain = '${BUILD_ROOT}/tools/build/scons/scripts/llvm.py'
    tools = [
      toolchain,
      '${BUILD_ROOT}/ssg/bsp/build/scripts/mbn_builder.py',
      '${BUILD_ROOT}/ssg/bsp/build/scripts/scl_builder.py',
      '${BUILD_ROOT}/sectools/sectools_builder.py',
      '${BUILD_ROOT}/ssg/bsp/build/scripts/get_dependency_builder.py',
      '${BUILD_ROOT}/ssg/bsp/build/scripts/image_environment.py',
      '${BUILD_ROOT}/ssg/bsp/build/scripts/pil_splitter_builder.py',
      '${BUILD_ROOT}/ssg/bsp/build/scripts/secure_apps.py',
      '${BUILD_ROOT}/ssg/bsp/build/scripts/unittesting_apps.py',
    ]
      
    #elfimage.py is required only for embedding internal TAs
    #Adding this script only in internal cases
    if 'USES_CLEAN_PACK' not in env:
      tools.append('${BUILD_ROOT}/core/bsp/build/scripts/elfimage.py')
    else:
      env.AddMethod(dummy_method,'AddEmbeddedImage')
      env.AddMethod(dummy_method,'RemoveEmbeddedImage')
      env.AddMethod(dummy_method,'EmbeddedImagesAsBuilderSource')
      env.AddMethod(dummy_method, 'BuilderSourceAsEmbeddedImages')
  # for double signing
  tools.append(os.path.join(THIS_DIR,'sectools_glue.py'))
  for tool in tools:
    env.LoadToolScript(tool)

#============================================================================
# secure_app_builder:
# this builder takes in the list of source files, include file paths,
# the metadata dictionary and the desired image name to build the
# secure application. The builder takes key word arguments as optional
# parameters. The key word arguments currently processed are:
# stack_size, heap_size, sec_xml_config, user_libs
# The Node object of the final builder is returned
# back so that the caller can add it as a dependency if needed.
#============================================================================
def secure_app_builder(env, sources, includes, metadata, image, **kwargs):
  env = env.Clone()
  return_list = []

  deploy_variants = kwargs.get('deploy_variants', ['DO_NOT_SHIP'])
  try:
    del kwargs['deploy_variants']
  except:
    pass

  if 'stack_size' in kwargs:
    print('Error: Declaring stack size via SecureAppBuilder is no longer supported.')
    print('       Please declare stack size in the app metadata.')
    sys.exit(1)
  if 'heap_size' in kwargs:
    print('Error: Declaring heap size via SecureAppBuilder is no longer supported. ')
    print('       Please declare heap size in the app metadata.')
    sys.exit(1)

  env.AppendUnique(CPPPATH = includes)
  env.AppendUnique(CPPPATH=SDK_INC_PATHS)
  env.AppendUnique(CPPPATH = STANDARD_INCLUDE_PATHS)

  # Added -ffunction-sections to reduce the size of TAs by culling unused
  # sections at the function level rather than the file level when used with
  # the linker flag --gc-sections
  env.Append(CCFLAGS = ' -fPIC  -ffunction-sections ')

  if env.OfftargetTesting():
    if env.StandaloneSdk():
      env.Append(CPPPATH=['$SDK_ROOT/inc/offtarget','$SDK_ROOT/inc/offtarget/private'])
    env.Append(CCFLAGS= ' -g ')
    env.Append(LINKFLAGS = ' -shared -Bsymbolic -Bdynamic -Wl,-gc-sections -Wl,-e,CElfFile_invoke ')
    # Wrap malloc and free calls for all TAs (except TA with nowrap_malloc_free,
    # which uses its own internal implementation of malloc and free)
    if kwargs.get('nowrap_malloc_free','0') == '0':
      env.Append(LINKFLAGS = " -Wl,-wrap,malloc -Wl,-wrap,free ")

    # wrap commonlib functions
    wrap_funcs = ['close', 'creat', 'fcntl', 'fstat',
            'fsync', 'lseek', 'lstat', 'mkdir', 'open',
            'openat', 'opendir', 'read', 'readdir', 'remove',
            'rmdir', 'telldir', 'unlink', 'unlinkat',
            'write', 'closedir',
            ]
    env.Append(LINKFLAGS=''.join([' -Wl,-wrap,{} '.format(f) for f in wrap_funcs]))
    
    if env.get('OFFTARGET_WEVERYTHING'):
      env.Append(CCFLAGS=' -Wno-error ')
    
  else:
    # Wrap malloc and free calls for all TAs (except TA with nowrap_malloc_free,
    # which uses its own internal implementation of malloc and free)
    if kwargs.get('nowrap_malloc_free','0') == '0':
      env.Append(LINKFLAGS = " --wrap malloc --wrap free ")

    # Added -gc-sections to garbage collect unused sections which when used with
    # compiler flag -ffunction-sections is at the function level.
    env.Append(LINKFLAGS = " -shared -Bsymbolic -Bdynamic -gc-sections -e TZAPPENTRY ")
    

  if kwargs.get('strip'):
    env.Append(CCFLAGS=' -g0')
    env.Append(LINKFLAGS=' -s')

  #The app scons environment is expected to define the OUT_DIR path
  build_dir = '${OUT_DIR}/${SHORT_BUILDPATH}/'
  build_path = os.path.join(build_dir, image)

  metadata_c_file = os.path.join(build_dir, 'metadata.c')

  md = env.AddSecureAppMetadata(source=env.Value(pickle.dumps(metadata)),target=metadata_c_file)
  md = env.Object(source=md)

  #compile sources to objects first, so we can control where the .o files end up at
  sobjects = compile_sources(env, build_path, sources, deploy_variants, kwargs.get('deploy_objects'), **kwargs)
  sobjects.append(md)

  libs, objs = env.GetTADependencies()

  sobjects.extend(objs)

  if not env.OfftargetTesting():
    if env.StandaloneSdk():
      libcpath = '${SDK_ROOT}/libs'
    else:
      libcpath = '${BUILD_ROOT}/ssg/securemsm/trustzone/qsee/mink/libstd'

    print('!! Image is ' + image)

    legacy_libc = kwargs.get('legacy_libc',
                             image in ['secure_ui_sample',
                                       'secure_ui_sample64',
                                       'fingpd32', 'fingpr32',
                                       'fingerpd', 'fingerpr'])
    if legacy_libc:
      libcfname = '${APP_EXEC_MODE}/minklibc.a'
    else:
      libcfname = '${APP_EXEC_MODE}/libc.a'
      
    libs.append(os.path.join(libcpath, libcfname))

    # The libclang_rt.builtins-arm.a is common for both armv7 and armv8
    libs.insert(0, '${LLVMLIB}/libclang_rt.builtins-arm.a')


  libs.extend(kwargs.get('user_libs',[]))
  libs = env.Flatten(libs)
  libs = [env.File(l) for l in libs]

  elf = env.Program(build_path, source=sobjects, LIBS=libs)
  return_list.extend(elf)

  # For offtarget testing, we don't have special linker scripts, mbns,
  # pil splitting, sectools signing, or deploying, so we can just install
  # the elf as a shared object
  if env.OfftargetTesting():
    ta_install_dir = get_ta_out_dir(env)
    ta_install_name = os.path.join(ta_install_dir, image + env['SHLIBSUFFIX'])
    ta_inst = env.InstallAs(ta_install_name, elf)
    return_list.extend(ta_inst)
    return return_list

  scl = env.SclBuilder(build_path, env.GetTALinkerScript())
  env.Depends(elf, scl)
  env.Depends(elf, env.GetCommonlibMBN())

  image_map = env.SideEffect(build_path + '.map', elf)
  env.Clean(elf, image_map)

  pbn = env.InstallAs(build_path + ".pbn", elf)
  return_list.extend(pbn)

  install_root = env.RealPath('${MBN_ROOT}')
  install_unsigned_root = env.SectoolGetUnsignedInstallPath(install_base_dir = install_root)
  env.Replace(MBN_FILE = os.path.join(install_unsigned_root, image))

  mbn = env.MbnBuilder( build_path,
                        pbn, IMAGE_TYPE=image, MBN_TYPE="elf",
                        IMAGE_ID=4, FLASH_TYPE="sdcc")
  return_list.extend(mbn)

  # Sectools signing
  default_config = env.subst('${SECIMAGE_CONFIG}')
  if default_config == 'secimage.xml':
    default_config = env.SectoolConfigV1()
  elif default_config == 'secimagev2.xml':
    default_config = env.SectoolConfigV2()
  elif default_config == 'secimagev3.xml':
    default_config = env.SectoolConfigV3()
  elif default_config == 'secimage_ecc.xml':
    default_config = env.SectoolConfigECC()
  elif default_config == 'secimage_eccv3.xml':
    default_config = env.SectoolConfigECCV3()
  else:
    raise ValueError('{} is not a valid secimage*.xml file'.format(default_config))
  
  alt_sign = kwargs.get('alt_sign')
  if not alt_sign:
    soc_hw_version = env.get("SOC_HW_VERSION")
    soc_vers = env.get("SOC_VERS")
  else:
    soc_hw_version = None
    soc_vers = None

  qti_doublesign = kwargs.get('qti_doublesign')
  secimage_xml_config = kwargs.get('sec_xml_config', default_config)

  #if the user passes his/her own secimage_xml_config file, the expectation here is that the
  #sign_id match the image name.
  if default_config != secimage_xml_config:
    sign_id = image
  else:
    sign_id = 'tzapps_dbl' if qti_doublesign else 'tzapps'
    
  app_id = kwargs.get('user_app_id', env.GetAppIDByImageName(image))

  if env.StandaloneSdk():
    pil_base_dir = '${OUT_DIR}/mbn/PIL_IMAGES/SPLITBINS_${SHORT_BUILDPATH}'
  else:
    pil_base_dir = '${BUILD_ROOT}/build/ms/bin/PIL_IMAGES/SPLITBINS_${SHORT_BUILDPATH}'

  if qti_doublesign:
    # Decide signer based on sign type
    signer = 'qti_remote' if env.get('SIGNTYPE') == 'CASS' else 'local_v2'
    if env.StandaloneSdk():
      glue_config_qti = os.path.join(os.path.dirname(default_config), '${SECIMAGE_CONFIG_QTI}')
    else:
      glue_config_qti = '${BUILD_ROOT}/sectools/config/integration/${SECIMAGE_CONFIG_QTI}'
    env['USES_SEC_POLICY_STEP1_QC_SIGN'] = True
    glue_max_num_root_certs = SCons.Script.ARGUMENTS.get('MAX_NUM_ROOT_CERTS', env.get('MAX_NUM_ROOT_CERTS'))
    sectools_signed_mbn_glue_step1 = env.SectoolBuilderGlue(
      glue_config_qti = glue_config_qti,
      glue_target_base_dir = '${SHORT_BUILDPATH}/intermediate',
      glue_source=mbn,
      glue_sign_id=sign_id,
      glue_app_id=app_id,
      glue_signer = signer,
      glue_qti_sign = True,
      glue_msmid = env.subst('${MSM_ID}'),
      glue_sectools_install_base_dir = os.path.join(install_root,'intermediate'),
      glue_install_file_name = image + ".mbn",
      glue_config = env.RealPath(secimage_xml_config),
      glue_soc_hw_version = soc_hw_version,
      glue_soc_vers=soc_vers,
      glue_max_num_root_certs = glue_max_num_root_certs,
      glue_is_step1 = True)

    return_list.append(sectools_signed_mbn_glue_step1)
    env.Depends(sectools_signed_mbn_glue_step1, env.Value(signer))

    env['USES_SEC_POLICY_STEP2_OEM_SIGN'] = True
    sectools_signed_mbn = env.SectoolBuilderGlue(
      glue_target_base_dir = '${SHORT_BUILDPATH}',
      glue_source=sectools_signed_mbn_glue_step1,
      glue_sign_id=sign_id,
      glue_app_id=app_id,
      glue_signer = "local_v2",
      glue_qti_sign = False,
      glue_msmid = env.subst('${MSM_ID}'),
      glue_sectools_install_base_dir =  '${MBN_ROOT}',
      glue_install_file_name = image + ".mbn",
      glue_config = env.RealPath(secimage_xml_config),
      glue_soc_hw_version = soc_hw_version,
      glue_max_num_root_certs = glue_max_num_root_certs,
      glue_soc_vers=soc_vers)
  else:
    sectools_signed_mbn = env.SectoolBuilder(
      target_base_dir = '${SHORT_BUILDPATH}',
      source=mbn,
      sign_id=sign_id,
      app_id=app_id,
      msmid = env.subst('${MSM_ID}'),
      sectools_install_base_dir = install_root,
      install_file_name = image + ".mbn",
      config = env.RealPath(secimage_xml_config),
      soc_hw_version = soc_hw_version,
      soc_vers=soc_vers)
  
  return_list.extend(sectools_signed_mbn)
  env.Depends(sectools_signed_mbn, mbn)

  default_deployables = [sectools_signed_mbn, mbn]
  
  if env.StandaloneSdk() or 'USES_CLEAN_PACK' in env:
    #pil splitting not supported in external sdk
    pass
  else:
    # Build files for PIL driver
    split_bin_count = kwargs.get('split_bin_count',DEFAULT_APP_SPLITBIN_COUNT)
    pil_bin_list = env.PilSplit([sectools_signed_mbn, mbn],pil_base_dir, split_bin_count)
    return_list.extend(pil_bin_list)
    default_deployables.append(pil_bin_list)

  # default apps deployment (if 'deploy_sources' is not given):
  # mbn & pil images -> given deploy variants (default DO_NOT_SHIP)
  
  deployables = kwargs.get('deploy_sources',default_deployables)

  env.Deploy(deployables, deploy_variants=deploy_variants, skip_check=True)

  # elfs & maps are always saved to DO_NOT_SHIP. If they are desired in external
  # dirs, they must be passed in by the caller. sym files are not explicitly
  # created by this builder & thus must be deployed from the app's SConscript.
  env.DeployInternal([elf, image_map])

  return return_list

#============================================================================
# Create the offtarget client app
# based on secure_app_builder w/o. metadata and env.GetTADependencies()
#============================================================================
def offtarget_client_app_builder(env, sources, includes, image, **kwargs):
  env = env.Clone()

  if not env.OfftargetTesting():
      raise ValueError('Error: offtarget_client_app_builder only supported for offtarget compilation')
      sys.exit(1)

  if kwargs.get('strip'):
    env.Append(CCFLAGS=' -g0')
    env.Append(LINKFLAGS=' -s')
    
  if env.get('OFFTARGET_WEVERYTHING'):
    env.Append(CCFLAGS=' -Wno-error ')

  deploy_variants = kwargs.get('deploy_variants', ['DO_NOT_SHIP'])

  
  env.AppendUnique(CPPPATH = includes)
  env.AppendUnique(CPPPATH=SDK_INC_PATHS)
  env.AppendUnique(CPPPATH='${BUILD_ROOT}/ssg/securemsm/sdk/CA/inc')
  env.AppendUnique(CPPPATH = STANDARD_INCLUDE_PATHS)
  if env.StandaloneSdk():
    env.Append(CPPPATH=['$SDK_ROOT/inc/offtarget','$SDK_ROOT/inc/offtarget/private'])
  else:
    env.AppendUnique(CPPPATH='${BUILD_ROOT}/ssg/securemsm/sdk/QTEEEmu/client_apis/inc')
  env.Append(LINKFLAGS= ' -lpthread -ldl ')

  #The app scons environment is expected to define the OUT_DIR path
  build_dir = '${OUT_DIR}/${SHORT_BUILDPATH}/'
  build_path = os.path.join(build_dir, image)

  ta_install_dir = get_ta_out_dir(env)

  if kwargs.get('no_emulation_libs'):
    libs = []
  else:
    test_svc_enabled = kwargs.get('test_svc_enabled')
    libs, cmnlib_path = env.GetQteeEmuLibs(test_svc_enabled)

    #The CA scons environment is expected to define the TA_OUT_DIR path
    env.Append(CPPDEFINES = 'TA_PATH_DEFINE=\\"' + ta_install_dir + '\\"')
    env.Append(CPPDEFINES = 'CMNLIB_PATH_DEFINE=\\"' + cmnlib_path + '\\"')

  #compile sources to objects first, so we can control where the .o files end up at
  sobjects = compile_sources(env, build_path, sources, deploy_variants, kwargs.get('deploy_objects'), **kwargs)

  libs.extend(kwargs.get('user_libs',[]))
  libs = env.Flatten(libs)
  libs = [env.File(l) for l in libs]

  elf = env.Program(build_path, source=sobjects, LIBS=libs)
  return elf

#============================================================================
# Create the offtarget client app lib
# based on secure_app_lib_builder
#============================================================================
def offtarget_client_app_lib_builder(env, includes, sources, libname, gc=True, deploy_sources=None, deploy_variants=None, deploy_lib=None, **kwargs):
  if deploy_variants is None:
    deploy_variants = ['DO_NOT_SHIP']
  env = env.Clone()

  if not env.OfftargetTesting():
      raise ValueError('Error: offtarget_client_app_lib_builder only supported for offtarget compilation')
      sys.exit(1)

  if kwargs.get('strip'):
    env.Append(CCFLAGS=' -g0')
    env.Append(LINKFLAGS=' -s')
    
  
  if env.get('OFFTARGET_WEVERYTHING'):
    env.Append(CCFLAGS=' -Wno-error ')
    
  env.Append(CCFLAGS = ' -fPIC -ffunction-sections ')
  
  env.AppendUnique(CPPPATH = includes)
  env.AppendUnique(CPPPATH=SDK_INC_PATHS)
  env.AppendUnique(CPPPATH = STANDARD_INCLUDE_PATHS)
  if env.StandaloneSdk():
    env.Append(CPPPATH=['$SDK_ROOT/inc/offtarget','$SDK_ROOT/inc/offtarget/private'])
  if gc:
    env.Append(LINKFLAGS = ' -shared -Bsymbolic -Bdynamic -Wl,-gc-sections ')
  else:
    env.Append(LINKFLAGS = ' -shared -Bsymbolic -Bdynamic -Wl,-no-gc-sections ')

  install_dir = get_lib_out_dir(env)
  objlist = compile_sources(env, install_dir, sources, deploy_variants, **kwargs)

  lib = env.Program(os.path.join(install_dir, libname + env['SHLIBSUFFIX']), source=objlist)

  if deploy_sources:
    env.Deploy(deploy_sources, deploy_variants=deploy_variants, skip_check=True)
  if deploy_lib:
    env.Deploy(lib, deploy_variants=deploy_variants, skip_check=True)
  return lib

#this helper function helps return 3 parameters about the object file that is
#getting compiled. It determines the name and relative path of the object file
#from the path and name of source file being compiled. It also returns the
#extension of the source file name. Some apps include '.o' files in the list
#of sources. We want to add these object files directly to list of objects
#since SCons Object builder does not take an object file. Moreover, we also
#return back the status of presence of the source file, to determine if
#the file is stripped or not.
def get_object_file_name(env, spath):
  #source files can be of the form a.c, ../x/a.c, b/a.c.
  #in each of above cases, we want objects to be stored as
  #a.o, x/a.o, b/a.o
  if isinstance(spath, str):
    sstr = spath
    sourceNodeRepr = env.File(spath)
  elif isinstance(spath, list):
    print('...Error: get_object_file_name does not process list!! %s' % (str(spath)))
    exit(1)
  else:
    sourceNodeRepr = spath

  spath_str = sourceNodeRepr.path
  split_pattern = env.subst('${BUILD_ROOT}/')
  if platform.system() == 'Windows':
    split_pattern = split_pattern.replace('/', '&').split(':')[-1]
    spath_str = spath_str.split(':')[-1].replace(os.sep, '&')
    sstr = spath_str.split(split_pattern)[-1]
    sstr = sstr.replace('&', os.sep)
  else:
    sstr = spath_str.split(split_pattern)[-1]

  #split at qsapps, otherwise path may become too long for windows to create object file
  if 'qsapps' in sstr:
    sstr = sstr.split('qsapps' + os.sep)[-1]
  elif 'trustzone_images' in sstr:
    sstr = sstr.split('trustzone_images' + os.sep)[-1]
  name, extension = os.path.splitext(sstr)

  #print 'Returning name and extension = %s %s from get_object_file_name' % (name, extension)
  #also return if the source file exists or we need to look for precompiled object file
  return (name, extension, sourceNodeRepr.exists())

#this is the helper function that helps build libraries that are used by
#apps to compose and link with their images. Using the secure app lib builder,
#we easily control the parameters like library name format, destination of libs and
#lib artifacts.
#archive_objs has special behavior regarding its behavior:
#  exists in kwargs -> use that value to decide to archive or not
#  doesn't exist in kwargs -> use env['SECURE_APP_BUILDER_ARCHIVE_OBJS']
def secure_app_lib_builder(env, includes, sources, libname, deploy_sources=None, deploy_variants=None, deploy_lib=None, **kwargs):
  if deploy_variants is None:
    deploy_variants = ['DO_NOT_SHIP']
  env = env.Clone()

  if kwargs.get('strip'):
    env.Append(CCFLAGS=' -g0')
    env.Append(LINKFLAGS=' -s')
    
  if env.get('OFFTARGET_WEVERYTHING'):
    env.Append(CCFLAGS=' -Wno-error ')
    
  env.Replace(LIBNAME = libname)
  env.Append(CCFLAGS = ' -fPIC -ffunction-sections ')
  env.AppendUnique(CPPPATH = includes)
  env.AppendUnique(CPPPATH=SDK_INC_PATHS)
  env.AppendUnique(CPPPATH = STANDARD_INCLUDE_PATHS)

  if env.StandaloneSdk() and env.OfftargetTesting():
    env.Append(CPPPATH=['$SDK_ROOT/inc/offtarget','$SDK_ROOT/inc/offtarget/private'])
  
  #uses flag handling is odd and expects that env['USES_CLEAN_PACK'] = 'False' will evaluate to False,
  #and that env['USES_CLEAN_PACK'] = 'True' evaluates to True,  and also that if 'USES_CLEAN_PACK' isn't in the env, 
  #it evaluates to False, so let's figure that out here
  cleanpack = 'USES_CLEAN_PACK' in env and (env.get('USES_CLEAN_PACK') == True or env.get('USES_CLEAN_PACK') == 'True')
  
  if cleanpack and not kwargs.get('force_source_compilation'):
    lib = env.File('${LIB_OUT_DIR}/' + libname + '${LIBSUFFIX}')
    if not lib.exists():
      print('...Error: lib %s does not exist' % (lib.rstr()))
      sys.exit(1)
  else:
    install_dir = '${LIB_OUT_DIR}'
    objlist = compile_sources(env, install_dir, sources, deploy_variants, **kwargs)
    archive_objs = kwargs.get('archive_objs',env.get('SECURE_APP_BUILDER_ARCHIVE_OBJS',True))
    if archive_objs:
      lib = env.Library(os.path.join(install_dir, libname), objlist)
    else:
      lib = objlist
    if env.GetOption('cleanpack'):
      env.NoClean(lib)

  if deploy_sources:
    env.Deploy(deploy_sources, deploy_variants=deploy_variants, skip_check=True)
  if deploy_lib:
    env.Deploy(lib, deploy_variants=deploy_variants, skip_check=True)
  return lib

#this is the helper function that compiles the sources into object files.
#If the source files exist, they are always used for compilation.
#if the build is stripped of sources, the pre-built object files are
#used instead. To make the code and process easy, the list of prebuilt
#object files is gathered. If sources are present, then the the
#corresponding object file is removed from the list of prebuilt objects
#gathered earlier. The cummulative list of sources and objects is
#presented to the builder to use for compilation
def compile_sources(env, dest_dir, sources, deploy_variants, deploy_obj_list=None, **kwargs):
  sobjects = []

  #prepare the list of sources whose objects need to be deployed
  deploy_objects = get_deploy_object_list_sname(env, deploy_obj_list)

  for source in env.Flatten(sources):
    #print("trying to compile",env.RealPath(source))
    if isinstance(source,str):
      source = env.RealPath(source)
    objname, extension, exists = get_object_file_name(env,source)
    if exists:
      if extension.lower() == '.o':
        obj = env.RealPath(source)
      else:
        obj = env.Object(os.path.join(dest_dir,objname), source, **kwargs)
      sobjects.append(obj)
      if env.GetOption('cleanpack'):
        env.NoClean(obj)
      if deploy_objects:
        deploy_object(env, deploy_objects, source, obj, deploy_variants)
    else:
      print("compile_sources can't find file {}".format(env.RealPath(str(source))))
      #raise #TODO: what do we do now?
  return sobjects

#helper function to check if the source name exists in the list
#of sources whose objects need to be deployed.
def deploy_object(env, deploy_object_list, src_name, deploy_obj, deploy_variants):
  if isinstance(src_name, SCons.Node.Node):
    sname = src_name.get_abspath()
  else:
    sname = src_name
  if sname in deploy_object_list:
    env.Deploy(deploy_obj, deploy_variants=deploy_variants, skip_check=True)

#helper function to create an ascii named list of file names of
#sources whose objects need to be deploy
def get_deploy_object_list_sname(env, *deploy_obj_list):
  #create a list of file names whose objects need to be deployed
  #the list can be of type None, 'filename' i.e string,
  #['file1', 'file2'] list of file name strings.
  deploy_objects = []
  for dsrc in env.Flatten(deploy_obj_list):
    #the list may be empty, then kwargs call into this function
    #may just pass None..so this needs to be processed
    if dsrc is None:
      break
    dsrc = dsrc if isinstance(dsrc, list) else [dsrc]
    for fnode in dsrc:
      if isinstance(fnode, SCons.Node.Node):
        deploy_objects.append(fnode.get_abspath())
      else:
        deploy_objects.append(fnode)
  return deploy_objects

def dummy_method(env, path, name, arch, image_list):
  pass
