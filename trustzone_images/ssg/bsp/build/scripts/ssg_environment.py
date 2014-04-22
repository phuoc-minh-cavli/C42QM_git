"""Collection of general-purpose tools/methods for use in SSG SConscripts"""

import hashlib
import os
import sys
import SCons
import sys
import os
import threading
import SCons.Defaults
import time
import datetime


THIS_DIR = os.path.abspath(os.path.dirname(__file__))


#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
    return env.Detect('ssg_environment')


def generate(env):
    elf_scripts_path = env.subst('${BUILD_ROOT}/core/bsp/build/scripts/')
    sys.path.append(os.path.abspath(elf_scripts_path))
    env.AddMethod(feature_flag, "DeclareFeature")
    env.AddMethod(get_scons_abspath, "SConsAbsPath")
    env.AddMethod(get_app_id_by_image_name, "GetAppIDByImageName")
    env.AddMethod(offtarget_testing, "OfftargetTesting")
    env.AddMethod(is_64_bit_image, "Is64BitImage")
    env.AddMethod(standalone_sdk, "StandaloneSdk")
    env.AddMethod(file_exists, "SconsFileExists")
    env.AddMethod(dir_exists, "SconsDirExists")
    env.AddMethod(get_image_config_xml, "GetImageConfigXml")
    env.AddMethod(has_apps_component,"HasAppsComponent")

    #We need to have a dummy DoUnitTest method for when we aren't building the unittest target
    #the actual unittest sconscript will load the actual tools/features
    env.AddMethod(dummy_method,"DoBareTest")
    env.AddMethod(dummy_method,"DoGoogleTest")

    #Dummy Doxygen method
    env.AddMethod(dummy_method,"AddDoxySource")
    
    #Dummy publish and require APIs
    env.AddMethod(dummy_method,"PublishProtectedApi")
    env.AddMethod(dummy_method,"PublishPrivateApi")
    env.AddMethod(dummy_method,"RequirePublicApi")
    env.AddMethod(dummy_method,"RequireRestrictedApi")
    env.AddMethod(dummy_method,"RequireProtectedApi")
    env.AddMethod(dummy_method,"RequirePrivateApi")

    # now use DeclareFeature to declare this script itself
    env.DeclareFeature("SSG_BUILD_ENVIRONMENT", uses_prefix=True)
    
    
    env.Tool('image_configuration', toolpath=[THIS_DIR])
    
    env.AddCommandlineArgument(env.BoolVariable('do_build_resource_stats','print out stats about memory and cpu load while building',0))
    default_env = SCons.Defaults._fetch_DefaultEnvironment()
    #This file is called from a few potential places during a full build; we only want to spawn the
    #stats printer thread once
    if env.get('do_build_resource_stats') and not default_env.get('CPU_LOAD_THREADING_STUFF_INITTED'):
      default_env['CPU_LOAD_THREADING_STUFF_INITTED'] = True
      init_print_cpu_load(env)


def print_cpu_load(psutil):
  p = psutil.Process()
  #as far as the terminal output is concerned, one print statement is atomic. so we put all of
  #the output into one string so that the stats aren't interleaved with other build output
  with p.oneshot():
    s = '__cpustats__ ' + str(time.time())
    s += ' getloadavg' + str(psutil.getloadavg())
    s += ' cpu_percent(' + str(psutil.cpu_percent(percpu=False))
    s += ') ' + str(psutil.virtual_memory())
  print s
  
def print_cpu_load_daemon(psutil, *args, **kwargs):
  while True:
    print_cpu_load(psutil)
    time.sleep(10)

def init_print_cpu_load(env):
  # Gathering cpu/ram usage stats requires psutil, which is not part of the standard pythonlib2
  # library. On internal linux harvesters, we can grab it from /pkg/ssg/..., but can't make that
  # guarantee in any other way really, so we try to import it, and if we fail, we just dont gather
  # build stats
  try:
    if os.path.isdir('/pkg/ssg'):
      sys.path.append('/pkg/ssg/pythonlib2.7/site-packages')
    import psutil
    print_cpu_load(psutil)
    #we use threading rather than multiprocessing because scons does black magic
    #to rename the Pickling module, which is also used by multiprocessing. Said
    #black magic kills the multiprocessing spawning code with weird errors. It does
    #not, however, affect how threading spawns threads. The print_cpu_load is very
    #nice and sleeps a long time, so it does not affect build time significantly,
    #regardless of the GIL existing
    thread = threading.Thread(target=print_cpu_load_daemon, args=(psutil,))
    # spawning the print_cpu_load thread as a daemon means it will get killed when 
    # the parent scons process is also killed
    thread.daemon = True
    thread.start()
    print "Initialized build resource gathering statistics!"
  except:
    print "Could not initialize build resource gathering statistics!"
    env['do_build_resource_stats'] = False
    if '/pkg/ssg/pythonlib2.7/site-packages' in sys.path:
      sys.path.remove('/pkg/ssg/pythonlib2.7/site-packages')


def feature_flag(env, flag, uses_prefix=False):
    """Adds a feature-definition flag to the environment in a way that is
       compatible with both QC SCons (USES_FLAGS) & Open Source SCons. Either
       implementation can be checked in SConscripts with 'if <flag> in env'.
       If 'uses_prefix' is True, also declares a USES_<flag> version of the
       flag, even if QC SCons is not available (useful for tools/features loaded
       globally before QC SCons)."""
    try:
        env.AddUsesFlags(flag)
    except AttributeError:
        env[flag] = True

    # most CoreBSP scripts expect that uses flags will always begin with
    # USES_* for consistency. If this feature flag does not already do that,
    # define an additional flag for their benefit.
    if flag.startswith("USES_"):
        return

    try:
        env.AddUsesFlags("USES_" + flag)
    except AttributeError:
        if uses_prefix:
            env["USES_" + flag] = True


def get_scons_abspath(env, item):
    """Returns the absolute path of the given string or SCons Node"""
    ret = None
    if type(item) == str:
      ret = env.File(item).get_abspath()
    elif isinstance(item, SCons.Node.Node):
      ret = item.get_abspath()
    elif isinstance(item, SCons.Node.Alias.Alias):
      ret = item.get_abspath()

    assert ret != None, "Unexpected item type '{}'".format(str(type(item)))
    return ret


def file_exists(env, filepath):
    """Returns True if 'filepath' exists, else False. Does not work for
       directories"""
    return os.path.exists(str(env.File(filepath).srcnode()))


def dir_exists(env, dirpath):
    """Returns True if 'dirpath' exists, else False. Only works for
       directories"""
    return os.path.exists(str(env.Dir(dirpath).srcnode()))


def get_image_config_xml(env, xml_file, alias_name):
    """Returns a valid, chipset-specific path for the IMAGE_CONFIG_XML
       environment variable required by devcfg"""
    img_cfg = "${BUILD_ROOT}/core/bsp/build/chipset/${CHIPSET}/" + xml_file
    if env.SconsFileExists(img_cfg):
        return env.subst(img_cfg)

    env.PrintWarning("Using stubs IMAGE_CONFIG_XML for " + alias_name)
    return env.subst("${BUILD_ROOT}/ssg/bsp/build/chipset/sdm0000/" + xml_file)


def get_app_id_by_image_name(env, image_name):
    """Return app id by calculating hash value with image name

       On MBNv6, app id has 4 byte length (8 hex characters) while previous
       version supported 8 byte, 16 hex characters. So, we limit it as 4 byte
       with exception (sdm670/sdm845) due to potential regression
       """

    if env["CHIPSET"] in ['sdm670', 'sdm845']:
        app_id_hexstr_len = 16
    else:
        app_id_hexstr_len = 8

    return '0x' + hashlib.sha256(image_name).hexdigest()[-app_id_hexstr_len:]

def offtarget_testing(env):
    return env.get('OFF_TARGET')

def is_64_bit_image(env):
    return env['PROC'] in ['A53_64', 'x86-64']
    
def standalone_sdk(env):
    return env.get('STANDALONE_SDK')
    
def has_apps_component(env):
    return env.get('HAS_APPS_COMPONENT')

def dummy_method(*args, **kwargs):
    pass
