#===============================================================================
#
# Sensors Protocol Buffer generation
#
# Copyright (c) 2018-2021 by Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
#
#-------------------------------------------------------------------------------
#
#  $Id: //components/rel/ssc.api/2.0/build/ssc_pb_builder.py#11 $
#  $DateTime: 2021/02/05 12:49:42 $
#  $Change: 28711114 $
#
#===============================================================================
import os, inspect, sys
import platform
import subprocess
import SCons.Builder
import logging

#------------------------------------------------------------------------------
# Globals
#------------------------------------------------------------------------------

ME               = inspect.getfile(inspect.currentframe())
MY_ROOT          = os.path.dirname(os.path.dirname(ME))
PB_PUBLIC_PATH   = ""
protoc_command   = []

if "Windows" in platform.system() or "CYGWIN" in platform.system():
  protoc_path = os.path.join(MY_ROOT, "nanopb", "generator-win")
  protoc = os.path.join(protoc_path, "protoc.exe")
  protoc_nano = os.path.join(protoc_path, "protoc-gen-nanopb.exe")
else:
  protoc_path = os.path.join(MY_ROOT, "nanopb", "generator")
  protoc = os.path.join(protoc_path, "protoc")
  protoc_nano = os.path.join(protoc_path, "protoc-gen-nanopb")

  subprocess.call(["chmod", "755", protoc])
  protoc_bin = os.path.join(protoc_path, "protoc.bin")
  subprocess.call(["chmod", "755", protoc_bin])
  subprocess.call(["chmod", "755", protoc_nano])

format_str = '%(filename)s(line %(lineno)s)\t%(levelname)s -- %(message)s'
logging.basicConfig(level=logging.ERROR, format=format_str)
logger = logging.getLogger(__name__)

#------------------------------------------------------------------------------
# Protocol Buffer Compilation - OLD - To be removed
#------------------------------------------------------------------------------

# Determine resulting .pb.c/.pb.h files given source .proto files
def pb_emitter_old(target, source, env):
  target = []
  global protoc_command

  #if 'SSC_SLPI_USER' in env['BUILD_TAGS'] or 'SSC_SIM' in env['BUILD_TAGS']:
  for file in source:
    file_path, file_name = os.path.split(str(file))
    filename, file_extension = os.path.splitext(file_name)

    target.append(os.path.join(PB_PUBLIC_PATH, filename) + '.pb.h')
    target.append(os.path.join(file_path, filename) + '.pb.c')

    proto_path = "--proto_path=" + file_path
    if proto_path not in protoc_command:
      protoc_command.append(proto_path);

  return target, source

# Compile all source .proto files
def pb_build_action_old(env, target, source):
  for file in source:
    file_path, file_name = os.path.split(str(file))
    filename, file_extension = os.path.splitext(file_name)

    command = protoc_command + ["--nanopb_out=" + file_path] + [str(file)]
    cmd_str = ' '.join(command)
    print("[PROTOC] " + cmd_str)
    subprocess.check_output(command)

    # moves generated pb.h files to top level inc folder
    pbh = env.FindFiles(filename + ".pb.h", file_path)
    if 1 != len(pbh):
      raise Exception()
    pbh_path, pbh_name = os.path.split(str(pbh[0]))
    os.rename(pbh[0], pbh[0].replace(pbh_path, PB_PUBLIC_PATH))

    pbc = env.FindFiles(filename + ".pb.c", file_path)
    if 1 != len(pbc):
      raise Exception()
    os.rename(pbc[0], str(file).replace(".proto", ".pb.c"))

  return None

#------------------------------------------------------------------------------
# Protocol Buffer Compilation
#------------------------------------------------------------------------------

# Determine resulting .pb.c/.pb.h files given source .proto files
def pb_emitter(target, source, env):
  target = []
  global protoc_command

  #if 'SSC_SLPI_USER' in env['BUILD_TAGS'] or 'SSC_SIM' in env['BUILD_TAGS']:
  for file in source:
    file_path, file_name = os.path.split(str(file))
    filename, file_extension = os.path.splitext(file_name)

    target.append(os.path.join(PB_PUBLIC_PATH, filename) + '.pb.h')
    target.append(os.path.join(env.subst('${BUILDPATH}'), "pb", filename) + '.pb.c')

    proto_path = "--proto_path=" + file_path
    if proto_path not in protoc_command:
      protoc_command.append(proto_path);

  return target, source

# Compile all source .proto files
def pb_build_action(env, target, source):
  for file in source:
    file_path, file_name = os.path.split(str(file))
    filename, file_extension = os.path.splitext(file_name)
    target_path, target_name = os.path.split(str(target[1]))
    nanopb_path = os.path.join('ssc_api', 'nanopb', 'build')

    command = protoc_command + ["--nanopb_out=" + target_path] + [str(file)]
    cmd_str = ' '.join(command)
    print("[PROTOC] " + cmd_str)
    try:
      subprocess.check_output(command, stderr = subprocess.STDOUT)
    except subprocess.CalledProcessError as err:
      print(str(err.output))
      if env.IsBuildInternal() and 'GLIBCXX_3.4.21' in str(err.output):
          env.PrintError("Missing GLIBCXX dependencies for protoc compiler: To resolve, please visit go/nanopre for more information.");
      sys.exit(1)

    # moves generated pb.h files to top level inc folder
    pbh = env.FindFiles(filename + ".pb.h", target_path)
    if 1 != len(pbh):
      raise Exception()
    pbh_path, pbh_name = os.path.split(str(pbh[0]))
    if nanopb_path not in pbh_path:
      os.rename(pbh[0], pbh[0].replace(pbh_path, env.subst('${SSC_PB_INC}')))

  return None

# Determine resulting .pb.c/.pb.h files given source .proto files
def native_pb_emitter(target, source, env):
  target = []
  global protoc_command

  for file in source:
    file_path, file_name = os.path.split(str(file))
    base_path, base_dirname = os.path.split(os.path.dirname(str(file)))
    filename, file_extension = os.path.splitext(file_name)

    target.append(os.path.join(file_path, '..', 'build', env.subst('${BUILDPATH}'), base_dirname, filename + '.pb.h'))
    target.append(os.path.join(env.subst('${BUILDPATH}'), base_dirname, filename) + '.pb.c')

    proto_path = "--proto_path=" + file_path
    if proto_path not in protoc_command:
      protoc_command.append(proto_path);

  return target, source

# Compile native source .proto files
def native_pb_build_action(env, target, source):
  for file in source:
    file_path, file_name = os.path.split(str(file))
    filename, file_extension = os.path.splitext(file_name)
    target_path, target_name = os.path.split(str(target[1]))

    command = protoc_command + ["--nanopb_out=" + target_path] + [str(file)]
    cmd_str = ' '.join(command)
    print("[PROTOC] " + cmd_str)
    try:
      subprocess.check_output(command, stderr = subprocess.STDOUT)
    except subprocess.CalledProcessError as err:
      print(str(err.output))
      if env.IsBuildInternal() and 'GLIBCXX_3.4.21' in str(err.output):
          env.PrintError("Missing GLIBCXX dependencies for protoc compiler: To resolve, please visit go/nanopre for more information.");
      sys.exit(1)

  return None

#------------------------------------------------------------------------------
# Prepare PROTOC command options
#------------------------------------------------------------------------------
def proto_buf_prep(env):
  global PB_PUBLIC_PATH
  global protoc
  global protoc_command

  pb_paths = []
  PB_PUBLIC_PATH = env.subst('${SSC_PB_INC}')
  if not os.path.exists(PB_PUBLIC_PATH):
    os.makedirs(PB_PUBLIC_PATH)

  protoc_command = [protoc, "--plugin=" + protoc_nano]
  logger.debug("protoc_command =\n" + "\n".join(["  %s" % i for i in protoc_command]))

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
  '''
  exists
  '''
  return env.Detect('ssc_pb')

def generate(env):
  '''
  generate
  '''
  env.AddUsesFlags('USES_SSC_PBCompile')
  pb_bld_old = SCons.Builder.Builder(action = pb_build_action_old, src_suffix = '.proto', emitter = pb_emitter_old)
  env.Append(BUILDERS = {'PBCompile' : pb_bld_old})

  env.AddUsesFlags('USES_SSC_PBGEN')
  pb_bld = SCons.Builder.Builder(action = pb_build_action, src_suffix = '.proto', emitter = pb_emitter)
  env.Append(BUILDERS = {'PBGen' : pb_bld})

  native_pb_bld = SCons.Builder.Builder(action = native_pb_build_action, src_suffix = '.proto', emitter = native_pb_emitter)
  env.Append(BUILDERS = {'PBGenNative' : native_pb_bld})

  # Create an ${SSC_PB_INC} variable.
  ssc_pb_inc = os.path.join("${BUILD_ROOT}", "ssc_api", "build", "${BUILDPATH}", "pb")
  print("ssc_pb_inc: " + ssc_pb_inc)
  env.Replace(SSC_PB_INC=ssc_pb_inc)

  proto_buf_prep(env)
  env.Append(CFLAGS = ' -DPB_FIELD_16BIT')

  return None
