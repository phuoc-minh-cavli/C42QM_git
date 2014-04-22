#===============================================================================
#
# @file ssc_static_lib_builder.py
#
# GENERAL DESCRIPTION
#   Builder for SSC Static Libraries
#
# Copyright (c) 2017-2019 by Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# $Header: //components/rel/ssc.slpi/5.1/build/ssc_static_lib_builder.py#4 $
# $DateTime: 2019/08/23 08:12:25 $
# $Change: 20304074 $
#
#===============================================================================
import inspect
import os
import platform
import subprocess
import logging
from itertools import chain

format_str = '%(filename)s(line %(lineno)s)\t%(levelname)s -- %(message)s'
logging.basicConfig(level=logging.ERROR, format=format_str)
logger = logging.getLogger(__name__)

#logger.setLevel(logging.INFO)  #logs info and higher
#logger.setLevel(logging.DEBUG) #logs everything

#------------------------------------------------------------------------------
# Globals
#------------------------------------------------------------------------------
ME               = inspect.getfile(inspect.currentframe())
MY_ROOT          = os.path.dirname(os.path.dirname(ME))
static_sensors   = []

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
  '''
  exists
  '''
  return env.Detect('ssc_static_lib_builder')

def generate(env):
  '''
  generate
  '''
  env.AddUsesFlags('USES_SSC_STATIC_LIB_BUILDER')
  env.AddMethod(add_ssc_su, 'AddSSCSU')

  env.AddUsesFlags('USES_STATIC_SENSORS_LIST_GEN')
  env.AddMethod(generate_static_sensor_list, 'GenerateStaticSensorList')

  return None

#==============================================================================
# add_island_lib
#==============================================================================
def add_island_lib(env, tag, lib_source, sections = []):
  if 'USES_MEMSEG' in env:
    env.TagLibraryForMemorySegment(tag, 'ssc', lib_source, sections)
  else:
    env.AddIslandLibrary(tag, lib_source, sections)

#==============================================================================
# add_ssc_su
#==============================================================================
def add_ssc_su(env, caller,
               # List of folders that contains the source files to be compiled
               flavor = ["src"],
               core_public_api = ['DEBUGTOOLS','KERNEL'], # for debug printf
               ssc_protected_api = [],
               clean_pack_list = [],
               # List of additional flags for the compiler. Will be added to
               # env['CFLAGS']
               cflags = [],
               # Use either island_sections or add_island_files to indicate
               # island content for the SU.
               # Both options at one time is unsupported.
               island_sections = [],
               add_island_files = False,
               register_func_name = "",
               # True or False; "Source", "Binary", or "Shared"
               binary_lib = True,
               # How many copies of this library to register with SEE
               registration_cnt = 1,
               # String: Diag SSID to use. One of: MSG_SSID_SNS MSG_SSID_SNS_FRAMEWORK,
               # MSG_SSID_SNS_PLATFORM, MSG_SSID_SNS_SENSOR_INT, MSG_SSID_SNS_SENSOR_EXT
               diag_ssid = '',
               # Required symbols from the SSC component
               pb_apis = [],
             ):
  global static_sensors

  SU_ROOT = os.path.dirname(os.path.dirname(caller))
  SU_NAME = os.path.splitext(os.path.split(caller)[1])[0]
  SRCPATH = [os.path.join(SU_ROOT, ingredient) for ingredient in flavor]
  INCPATH = [os.path.join(SU_ROOT, 'inc')]
  PBPATH = os.path.join(SU_ROOT, 'pb')

  logger.info("%s" % SU_NAME)
  logger.debug("root = %s" % SU_ROOT)
  logger.debug("core_public_api = %s" % core_public_api)
  logger.debug("ssc_protected_api = %s" % ssc_protected_api)
  logger.debug("clean_pack_list = %s" % clean_pack_list)
  logger.debug("island_sections = %s" % island_sections)
  logger.debug("register_func_name = %s" % register_func_name)

  env.VariantDir('${BUILDPATH}', SU_ROOT, duplicate=0)

  #-------------------------------------------------------------------------------
  # Build settings
  #-------------------------------------------------------------------------------
  env.Append(CFLAGS = cflags)

  if diag_ssid is not '':
    diag_ssid = '-DSNS_DIAG_SSID='+diag_ssid;
    env.Append(CPPDEFINES = diag_ssid)

  if 'SSC_TARGET_X86' in env['CPPDEFINES']:
    env.Append(CFLAGS = '-Wno-missing-field-initializers')

  #-------------------------------------------------------------------------------
  # External dependencies
  #-------------------------------------------------------------------------------
  if core_public_api and 'SSC_TARGET_HEXAGON' in env['CPPDEFINES']:
    env.RequirePublicApi(core_public_api, area='core')
  env.RequirePublicApi(['SNS_API_INCLUDES'], area='ssc_api')

  #-----------------------------------------------------------------------------
  # Source Path
  #-----------------------------------------------------------------------------
  for folder in SRCPATH :
    if not os.path.isdir(folder):
      if (type(binary_lib) is bool and binary_lib) or "Binary" == binary_lib:
        env.PrintInfo("Ignoring flavor %s: Path %s" %(str(flavor), folder))
      else:
        env.PrintError("For flavor %s: Path %s not found" %(str(flavor), folder))
        env.Exit()

  INCPATH += SRCPATH

  #-----------------------------------------------------------------------------
  # Internal dependencies
  #-----------------------------------------------------------------------------
  env.RequirePublicApi(['SNS_INCLUDES'], area='ssc')
  env.PublishPrivateApi(SU_NAME + '_API', INCPATH)
  if ssc_protected_api:
    env.RequireProtectedApi(ssc_protected_api)

  #-------------------------------------------------------------------------------
  # Source files
  #-------------------------------------------------------------------------------
  SRCFILES =  [env.FindFiles(['*.c','*.S'], folder) for folder in SRCPATH]
  ISLAND_SRCFILES = [env.FindFiles(['*_island.c'], folder) for folder in SRCPATH]

  #Flatten the above two list of lists
  SRCFILES = list(chain(*SRCFILES))
  ISLAND_SRCFILES = list(chain(*ISLAND_SRCFILES))

  [SRCFILES.remove(f) for f in ISLAND_SRCFILES if f in SRCFILES]

  VARIANT_SRCFILES = [f.replace(SU_ROOT,'${BUILDPATH}') for f in SRCFILES]
  VARIANT_ISLAND_FILES  = [f.replace(SU_ROOT,'${BUILDPATH}') for f in ISLAND_SRCFILES]

  # PEND: Update with a new API to retrieve path hard-coded below
  for pb_api in pb_apis:
    file = os.path.join(env.subst('${SSC_ROOT}'), "..", "ssc_api", 'build', '${BUILDPATH}', 'pb', pb_api)
    file += ".pb.o"
    VARIANT_SRCFILES.append(str(file))

  #-------------------------------------------------------------------------------
  # Protocol Buffer API files
  #-------------------------------------------------------------------------------
  VARIANT_PROTO_SRCFILES = []
  if 'USES_SSC_PBGEN' in env:
    #env.PrintInfo("SSC: PBPATH %s" %PBPATH)
    for proto_file in env.FindFiles(['*.proto'], PBPATH):
      #env.PrintInfo("SSC: proto_file %s" %proto_file)
      output = env.PBGen(str(proto_file))
      VARIANT_ISLAND_FILES += [output[1]]   # Just include the .c file
      VARIANT_PROTO_SRCFILES.append(os.path.join(SU_ROOT, 'build', str(output[1])))
  else:
    env.PrintInfo("SSC: Skipping PBGen for NPOR %s" %SU_NAME)

  #-------------------------------------------------------------------------------
  # Should library be built
  #-------------------------------------------------------------------------------
  should_build = False
  if (SU_NAME in env['SSC_INCLUDE_ALGO_LIBS'] or SU_NAME in env['SSC_INCLUDE_PLAT_LIBS'] or SU_NAME in env['SSC_INCLUDE_SENS_VEND_LIBS'] or SU_NAME in env['SSC_INCLUDE_SIM_LIBS']) and (SU_NAME not in env['SSC_EXCLUDE_LIBS']):
    should_build = True
  else:
    env.PrintInfo("SSC: Skipping %s" %SU_NAME)

  #-------------------------------------------------------------------------------
  # Add Libraries to Image
  #-------------------------------------------------------------------------------
  if should_build:
    SU_LIB = os.path.join('${BUILDPATH}', SU_NAME)
    ISLAND_SU_LIB = SU_LIB + "_island"
    SU_PROTO_LIB = SU_LIB + "_PROTO"

    if "Shared" == binary_lib:
      env.PrintInfo("Shared library: %s" %SU_NAME)
      if len(VARIANT_ISLAND_FILES) > 0:
        VARIANT_SRCFILES += VARIANT_ISLAND_FILES
      lib = env.AddSSCSharedLibrary(['SSC_SHARED_LIBS'], SU_LIB, VARIANT_SRCFILES, [])
    elif 'SSC_TARGET_X86' in env['CPPDEFINES']:
        lib = env.AddLibrary(['SSC_SIM'], SU_LIB, VARIANT_SRCFILES)
        if len(VARIANT_ISLAND_FILES) > 0:
          ulib = env.AddLibrary(['SSC_SIM'], ISLAND_SU_LIB, VARIANT_ISLAND_FILES)
    elif (type(binary_lib) is bool and binary_lib) or "Binary" == binary_lib:
      lib = env.AddBinaryLibrary(env['SSC_BUILD_TAGS'], SU_LIB, VARIANT_SRCFILES)
      ulib = env.AddBinaryLibrary(env['SSC_BUILD_TAGS'], ISLAND_SU_LIB, VARIANT_ISLAND_FILES)
    else:
      lib = env.AddLibrary(env['SSC_BUILD_TAGS'], SU_LIB, VARIANT_SRCFILES)
      ulib = env.AddLibrary(env['SSC_BUILD_TAGS'], ISLAND_SU_LIB, VARIANT_ISLAND_FILES)

    #-------------------------------------------------------------------------------
    # Island mode
    #-------------------------------------------------------------------------------
    if ('USES_MEMSEG' in env or 'USES_ISLAND' in env) and "Shared" != binary_lib:
      # Add only *_island.c files for the SU in uimg
      if add_island_files:
        if len(island_sections) > 0:
          # Unsupported combination
          logger.debug("Both island sections and island files enabled")
          raise Exception()
        else:
          # only add island files in island memory
          add_island_lib(env, env['SSC_BUILD_TAGS'], ulib)
      # Add all libs for the SU in uimg
      elif '*' in island_sections:
        add_island_lib(env, env['SSC_BUILD_TAGS'], lib)
        if len(VARIANT_ISLAND_FILES) > 0:
          add_island_lib(env, env['SSC_BUILD_TAGS'], ulib)

      # Add uimg tagged sections from all libs for the SU in uimg
      elif len(island_sections) > 0:
        add_island_lib(env, env['SSC_BUILD_TAGS'], lib, island_sections)
        if len(VARIANT_ISLAND_FILES) > 0:
          add_island_lib(env, env['SSC_BUILD_TAGS'], ulib, island_sections)
      else:
        logger.debug("No island files/sections for SU")

    #-------------------------------------------------------------------------------
    # Add register function to static sensor list
    #-------------------------------------------------------------------------------
    if 'USES_STATIC_SENSORS_LIST_GEN' in env and "Shared" != binary_lib:
      if register_func_name:
        if "sns_register_suid_sensor" == register_func_name:
          static_sensors.insert(0, (register_func_name, registration_cnt))
        else:
          static_sensors.append((register_func_name, registration_cnt))

  #-------------------------------------------------------------------------------
  # Remove source files
  #-------------------------------------------------------------------------------
  if clean_pack_list:
    TEMP = env.FindFiles(clean_pack_list, SU_ROOT)
    VARIANT_PROTO_SRCFILES = env.FindFiles(['*.pb.c'], SU_ROOT)
    REMOVE_FILES = [x for x in TEMP if x not in VARIANT_PROTO_SRCFILES]
    #env.PrintInfo("SSC: VARIANT_PROTO_SRCFILES %s" %VARIANT_PROTO_SRCFILES)
    #env.PrintInfo("REMOVE_FILES @ cleanpack =\n" + "\n".join(["  %s" % i for i in REMOVE_FILES]))
    env.CleanPack(env['SSC_BUILD_TAGS'], REMOVE_FILES)
    #logger.debug("REMOVE_FILES =\n" + "\n".join(["  %s" % i for i in REMOVE_FILES]))

  if not env.IsBuildInternal():
    non_flavor_files = []
    # Clean non-flavor files except common folders
    for x in os.listdir(SU_ROOT):
      if x not in flavor + [ 'build', 'src', 'inc', 'pb', 'config', 'hexagon_root_pd']:
        folder = os.path.join(SU_ROOT,x)
        if os.path.isdir(folder):
          non_flavor_files += env.FindFiles('*', folder)

    env.CleanPack(env['SSC_BUILD_TAGS'], non_flavor_files)

#==============================================================================
# Generates sns_static_sensors.c
#==============================================================================
def generate_static_sensor_list(env, tags):
  global static_sensors
  if env.IsKeyEnable(tags) is True:
    logger.info("generate_static_sensor_list() called with %d sensors" % len(static_sensors))

    if len(static_sensors) == 0:
      logger.error("There are no static sensors?!!!")
      return None

    static_sensors_file = os.path.join(env.subst('${SSC_ROOT}'),
                                       'framework', 'src', env.subst('${BUILDPATH}'), 'sns_static_sensors.c')
    dirname = os.path.dirname(static_sensors_file)
    if not os.path.exists(dirname):
      os.makedirs(dirname)
    fo = open(static_sensors_file, "w")
    fo.write("/* Autogenerated file.  Manual modification is pointless. */\n\n")
    fo.write("#include \"sns_rc.h\"\n")
    fo.write("#include \"sns_register.h\"\n")
    fo.write("#include \"sns_types.h\"\n")
    fo.write("\n")
    for reg_func,reg_cnt in static_sensors:
      fo.write("sns_rc %s(sns_register_cb const *register_api);\n" % reg_func)
    fo.write("\nconst sns_register_entry sns_register_sensor_list[] =\n{\n")
    for reg_func,reg_cnt in static_sensors:
      fo.write(" { %s, %i},\n" % (reg_func, reg_cnt))
    fo.write("};\n\n")
    fo.write("const uint32_t sns_register_sensor_list_len = ARR_SIZE(sns_register_sensor_list);\n\n")
    fo.close()

