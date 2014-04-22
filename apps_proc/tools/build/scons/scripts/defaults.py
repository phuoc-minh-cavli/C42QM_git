'''
   Defaults

   GENERAL DESCRIPTION
      Defualt values added to build system environment
'''
#===============================================================================
#
# Defaults
#
# GENERAL DESCRIPTION
#    Defualt values added to build system environment
#
# Copyright (c) 2009-2012 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/scons.qc/1.1/scripts/defaults.py#25 $
#  $DateTime: 2020/05/15 14:40:20 $
#  $Author: pwbldsvc $
#  $Change: 23883302 $
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 07/28/10   wd      For QAPI 1.0.2
#                    * Automatic Cloning, sub scripts no longer need to clone
#                      the environment it is done for them before loading the
#                      script.
#                    * Added APIs for AddFileList(), AddArtifacts().
#                    * Added initial for external build system builder.
#                    * Changed config.py to use our won wrapper when loading
#                      .py files to get better debugging info.
#
# 04/24/09   wd      Create
#
#==============================================================================
import os
import atexit
import re
import json
import shutil
from SCons.Script import *
from datetime import datetime
from glob import glob

#------------------------------------------------------------------------------
# Global to keep time info
#------------------------------------------------------------------------------
global build_start_time
global default_env

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
    '''
    exists
    '''
    return env.Detect('defaults')

def generate(env):
    '''
    generate
    '''
    global build_start_time
    global default_env

    build_start_time = datetime.now()
    default_env = env

    env['BUILD_TIME_SATRT'] = build_start_time
    # ensure minimum version sof python and SCons
    env.EnsurePythonVersion(2, 5)
    env.EnsureSConsVersion(1, 0)


    #==========================================================================
    # Changes to SCONS_QAPI
    #==========================================================================
    # 1.0.3 - Added ability to call external build system builders.
    #         Complete implementation to AddLibrary, AddBinaryLibrary.
    # 1.0.4 - Added capability to control SCONS num of jobs trough OS
    #         enviroment.
    # 1.0.5 - Added capability clean pack
    # 1.0.6 - Changed to AddBinaryLibrary, add optional exception tags
    env.Replace(SCONS_QAPI="1.0.6")

    #==========================================================================
    # Get QC SCons ext reelase info
    #==========================================================================
    ''' Find version information.
    Expected version info:
      Release label: @SCONS.QC.X

    Expected date format:
      $DateTime: 2020/05/15 14:40:20 $
    '''

    release_text = env.subst('${BUILD_ROOT}/tools/build/scons/release.txt')
    if os.path.exists(release_text):
        rfile = open(release_text, 'r')
        rfile_data = rfile.read()

        rfile_ver_re = re.search('\@SCONS\.QC\.(.*)', rfile_data, re.I)
        rfile_date_re = re.search('DateTime\: (.* .*) ', rfile_data)

        if rfile_ver_re is not None:
            scons_qc_ver = rfile_ver_re.group(1)
        else:
            scons_qc_ver = "uknown"

        if rfile_date_re is not None:
            scons_qc_date = rfile_date_re.group(1)
            scons_qc_ver_txt = "%s (%s)" % (scons_qc_ver, scons_qc_date)
        else:
            scons_qc_ver_txt = scons_qc_ver

        env.Replace(SCONS_QC_VER=scons_qc_ver_txt)

    #==========================================================================
    # Load essentials
    #==========================================================================
    if 'BUILD_SCRIPTS_ROOT' not in env:
        build_scripts_root = os.environ.get('BUILD_SCRIPTS_ROOT')
        if build_scripts_root is None:
            build_scripts_root = '${BUILD_ROOT}/tools/build/scons/scripts'
        env.Replace(BUILD_SCRIPTS_ROOT=build_scripts_root)

    if 'BUILD_SCRIPTS_PRODUCT_ROOT' not in env:
        build_scripts_product_root = \
            os.environ.get('SCONS_BUILD_SCRIPTS_PRODUCT_ROOT')
        if build_scripts_product_root is None:
            build_scripts_product_root = \
                '${BUILD_ROOT}/tools/build/scons/scripts'
        env.Replace(BUILD_SCRIPTS_PRODUCT_ROOT=build_scripts_product_root)

    if 'BUILD_SCRIPTS_OEM_ROOT' not in env:
        build_scripts_oem_root = os.environ.get('BUILD_SCRIPTS_OEM_ROOT')
        if build_scripts_oem_root is None:
            build_scripts_oem_root = '${BUILD_ROOT}/tools/build/scons/scripts'
        env.Replace(BUILD_SCRIPTS_OEM_ROOT=build_scripts_oem_root)

    # load minimum required scripts
    # to enable most of Qualcomm SCons infrastructure
    env.Tool('utils', toolpath=['${BUILD_SCRIPTS_ROOT}'])
    env.Tool('cmdline', toolpath=['${BUILD_SCRIPTS_ROOT}'])

    env.PrintDebugInfo('scripts', "Loading DEFAULT scripts...")
    env.LoadToolScript("${BUILD_SCRIPTS_ROOT}/loaded_builders.py")
    env.LoadToolScript("${BUILD_SCRIPTS_ROOT}/formatting.py")

    #==========================================================================
    # Load alternate PRODUCT options
    #==========================================================================
    defaults_product = "${BUILD_SCRIPTS_PRODUCT_ROOT}/defaults_product.py"

    if env.PathExists(defaults_product):
        env.LoadToolScript(defaults_product)

    #==========================================================================
    # Load alternate OEM options
    #==========================================================================
    defaults_oem = "${BUILD_SCRIPTS_OEM_ROOT}/defaults_oem.py"

    if env.PathExists(defaults_oem):
        env.LoadToolScript(defaults_oem)

    #==========================================================================
    # Init defaults options
    #==========================================================================
    set_db_name(env)
    set_num_jobs(env)
    reg_on_exit()

    # optimize scons, cache MD5, don't MD5 every single build
    # unless file time chnages
    env.SetOption('max_drift', 1)
    env.Decider('MD5-timestamp')
    #env.SetOption('implicit_cache', 1)

    #if GetOption('clean'):
    #   env.SetOption('implicit-deps-unchanged', 1)
    #   env.SetOption('implicit_cache', 0)

    # init dir and file name patterns
    image_root_dir_patterns = env.get('IMAGE_ROOT_DIR_PATTERNS')
    if image_root_dir_patterns is None:
        image_root_dir_patterns = ['${BUILD_ROOT}/*/bsp/*/build']
        env.Replace(IMAGE_ROOT_DIR_PATTERNS=image_root_dir_patterns)

    image_dir_patterns = env.get('IMAGE_DIR_PATTERNS')
    if image_dir_patterns is None:
        image_dir_patterns = ['*']
        env.Replace(IMAGE_DIR_PATTERNS=image_dir_patterns)

    au_root_dir_patterns = env.get('AU_ROOT_DIR_PATTERNS')
    if au_root_dir_patterns is None:
        au_root_dir_patterns = ['${BUILD_ROOT}/*/build']
        env.Replace(AU_ROOT_DIR_PATTERNS=au_root_dir_patterns)

    au_dir_patterns = env.get('AU_DIR_PATTERNS')
    if au_dir_patterns is None:
        au_dir_patterns = ['${BUILD_ROOT}/${AU_NAME}/build']
        env.Replace(AU_DIR_PATTERNS=au_dir_patterns)

    script_dir_patterns = env.get('SCRIPT_DIR_PATTERNS')
    if script_dir_patterns is None:
        script_dir_patterns = ['../*/build']
        env.Replace(SCRIPT_DIR_PATTERNS=script_dir_patterns)

    script_name_patterns = env.get('SCRIPT_NAME_PATTERNS')
    if script_name_patterns is None:
        script_name_patterns = ['SConscript', '*.scons']
        env.Replace(SCRIPT_NAME_PATTERNS=script_name_patterns)

    api_script_name_patterns = env.get('API_SCRIPT_NAME_PATTERNS')
    if api_script_name_patterns is None:
        api_script_name_patterns = ['paths.sconscript', '*.api']
        env.Replace(API_SCRIPT_NAME_PATTERNS=api_script_name_patterns)

    build_warnings_for_oem = env.get('ENABLE_BUILD_WARNINGS_FOR_OEM')
    if build_warnings_for_oem is None:
        env.Replace(ENABLE_BUILD_WARNINGS_FOR_OEM=False)

    read_component_details(env)
    autocorrect_filter_opt(env)

    # Add command text formatting and utils to enviroment
    env.LoadToolScript("${BUILD_SCRIPTS_ROOT}/generic_builders.py")
    env.LoadToolScript('${BUILD_SCRIPTS_ROOT}/scons_mod.py')
    env.LoadToolScript('${BUILD_SCRIPTS_ROOT}/config.py')

    display_config_banner(env)
    check_pywin32(env)

    return None

#==============================================================================
# Function to check total command path length for Windows only.
# Windows limitation has a Unicode 256 character limitation for sake of falling
# into a trap we will set max path length to 254 chars
# POSIX OSs is no issue command line limit usually in the 128k range.
#
#==============================================================================
def check_paths(env, cmd):
    '''
    check_paths
    '''
    #check for multiple paths using space, within the cmd line
    if " " in cmd:
        list_of_paths = cmd.split(" ")
    else:
        list_of_paths = [cmd]
    maxline = int(env.subst('$MAXPATHLINELENGTH'))
    for line in list_of_paths:
        if len(line) >= maxline:
            errstr = "\nWindows Path Length Exceeded: Max = %s Current = %s\
                     \n\n" % (maxline, len(line))
            print "\n** Build errors... %s %s" % (cmd, errstr)

#------------------------------------------------------------------------------
# Register function for on exit event
#------------------------------------------------------------------------------
def print_build_failures():
    '''
    print_build_failures
    '''
    from SCons.Script import GetBuildFailures
    global default_env

    build_failures = GetBuildFailures()
    if build_failures:
        print '** Build errors...'
        for build_failure in build_failures:
            if default_env['PLATFORM'] in ['windows', 'win32']:
                default_env.Replace(MAXPATHLINELENGTH=254)
                check_paths(default_env, str(build_failure.node))
            print "%s failed: %s" % (build_failure.node, build_failure.errstr)
        print ''

def on_exit():
    '''
    on_exit
    '''
    print
    print "=================================================================" \
        "============="
    print "   SCons build summary"
    print "=================================================================" \
        "============="

    print_build_failures()

    global build_start_time
    build_end_time = datetime.now()
    elapsed_time = build_end_time - build_start_time

    # computation per python instructions for timedelta.total_seconds()
    # whihc is new in python 2.7
    td = elapsed_time
    total_microseconds = \
        td.microseconds + (td.seconds + td.days *  24 * 3600) * 10**6
    seconds, microseconds = divmod(total_microseconds, 10**6)
    minutes, seconds = divmod(seconds, 60)
    hours, minutes = divmod(minutes, 60)

    # always round up, as build start time and end time will alwasy round up,
    # this way the values alway match
    # (delta vs manually calculating from end-start).
    if microseconds > 0:
        seconds = seconds + 1

    print "** Build time..."
    print " Build start  : %s" % build_start_time.ctime()
    print " Build end    : %s" % build_end_time.ctime()
    print " Elapsed time : %d:%.2d:%.2d" % (hours, minutes, seconds)

def reg_on_exit():
    '''
    reg_on_exit
    '''

    atexit.register(on_exit)

#------------------------------------------------------------------------------
# Set detabase name to be used... BUILD_ID must be defined
#------------------------------------------------------------------------------
def set_db_name(env):
    '''
    set_db_name

    Set detabase name to be used... BUILD_ID must be defined
    '''
    # create file name
    oem_bid = env.get('OEM_BUILD_ID')

    # OS environment overrides any other settings.
    oem_bid = os.environ.get('SCONS_OEM_BUILD_ID', oem_bid)

    db_ext = ".dblite"

    bid = str(env.get('BUILD_ID'))

    if oem_bid is not None:
        oem_bid = str(oem_bid)
        env.Replace(OEM_BUILD_ID=oem_bid)
        bid = "%s_%s" % (oem_bid, bid)

    cleanpack = env.GetOption('cleanpack')
    db_name = ".sconsign_%s" % bid
    if cleanpack:
        main_build_db_name = db_name
        db_name += "_cleanpack"

    # remove db first file if force build option is enabled
    force_build = env.GetOption('force_build')

    if force_build:
        db_del_file = "%s%s" % (db_name, db_ext)

        if os.path.exists(db_del_file):
            os.unlink(db_del_file)
            print
            env.PrintInfo("FORCE BUILD, cache db deleted [%s]" % db_del_file)

    if cleanpack:
        if os.path.exists("%s.dblite" % main_build_db_name):
            shutil.copy("%s.dblite" % main_build_db_name, "%s.dblite" % db_name)
        else:
            print "Warning: <%s.dblite> file not created, since no <%s.dblite> file was present in the current directory." % (db_name, main_build_db_name)

    # set db filename
    SCons.SConsign.File(db_name)

#------------------------------------------------------------------------------
# Figure out number of jobs to use
#------------------------------------------------------------------------------
def set_num_jobs(env):
    '''
    set_num_jobs

    Set detabase name to be used... BUILD_ID must be defined
    '''
    default_max_num_jobs = 12
    default_num_jobs_factor = 2
    default_override_num_jobs = 0
    max_num_jobs = env.get('MAX_NUM_JOBS', default_max_num_jobs)
    num_jobs_factor = env.get('NUM_JOBS_FACTOR', default_num_jobs_factor)
    override_num_jobs = env.get('OVERRIDE_NUM_JOBS', default_override_num_jobs)

    # OS enviroments overrides any other settings.
    max_num_jobs = int(os.environ.get('SCONS_MAX_NUM_JOBS', max_num_jobs))
    num_jobs_factor = \
        int(os.environ.get('SCONS_NUM_JOBS_FACTOR', num_jobs_factor))
    override_num_jobs = \
        int(os.environ.get('SCONS_OVERRIDE_NUM_JOBS', override_num_jobs))

    num_cpu = 1
    if hasattr(os, "sysconf"):
        # Linux
        if os.sysconf_names.has_key("SC_NPROCESSORS_ONLN"):
            num_cpu = os.sysconf("SC_NPROCESSORS_ONLN")
    else:
        # Windows:
        if os.environ.has_key("NUMBER_OF_PROCESSORS"):
            num_cpu = int(os.environ["NUMBER_OF_PROCESSORS"])

    # figure out how many jobs to process
    num_jobs = num_cpu * num_jobs_factor

    if num_jobs > max_num_jobs:
        num_jobs = max_num_jobs

    if override_num_jobs != 0:
        num_jobs = override_num_jobs

    env.SetOption('num_jobs', num_jobs)

    print
    #print ("*** Found %d CPUs, running with -j %d" % \
    #   (num_cpu, env.GetOption('num_jobs')))
    env.PrintInfo("Found %d CPUs, running with -j %d" % \
        (num_cpu, env.GetOption('num_jobs')))

def display_config_banner(env):
    '''
    display_config_banner

    Display a configuration banner
    '''
    print "===================================================================="
    print "PLATFORM      = %s " % env['PLATFORM']
    print "PYTHON Ver    = %s " % sys.version
    print "Python Path   = %s " % sys.executable
    print "PYTHON cmd    = %s " % env['PYTHONCMD']
    print "SCONS Ver     = %s " % SCons.__version__

    scons_qc_ver = env.get('SCONS_QC_VER')

    if scons_qc_ver is not None:
        print "SCONS QC Ver  = %s " % scons_qc_ver
    else:
        print "SCONS QAPI    = %s " % env.get('SCONS_QAPI', 'unknown')

    if env['PLATFORM'] in ['windows', 'win32', 'cygwin']:
        print "OS ENV SIZE   = %d " % len(str(os.environ))

    print "TARGET_ROOT   = %s " % env['TARGET_ROOT']
    print "BUILD_ROOT    = %s " % env['BUILD_ROOT']
    print "BUILD_MS_ROOT = %s " % env['BUILD_MS_ROOT']

    print "MBN_ROOT      = %s" % env['MBN_ROOT']
    print "BUILD_ID      = %s" % env['BUILD_ID']
    print "BUILD_VER     = %s" % env['BUILD_VER']
    print "BUILD_ASIC    = %s" % env['BUILD_ASIC']
    print "MSM_ID        = %s" % env['MSM_ID']
    print "CHIPSET       = %s" % env['CHIPSET']
    print "TARGET_FAMILY = %s" % env['TARGET_FAMILY']
    print "HAL_PLATFORM  = %s" % env['HAL_PLATFORM']
    print " "
    print "COMMAND_LINE_TARGETS are", \
        map(str, SCons.Script.COMMAND_LINE_TARGETS)
    print "BUILD_TARGETS are       ", map(str, BUILD_TARGETS)
    print "===================================================================="

def check_pywin32(env):
    '''
    check_pywin32

    Check for pywin32 in the currently running python version
    '''
    if env['PLATFORM'] in ['windows', 'win32', 'cygwin']:
        try:
            import win32api
        except ImportError:
            print "\n\n========================================================"
            print "********************************************************"
            print "ERROR!!!"
            print "pywin32 has not been installed for python %s" % sys.version
            print "ERROR!!!"
            print "********************************************************"
            print "========================================================\n\n"
            sys.exit(1)

def get_parsed_component_json_data(env):
    '''
    returns parsed component.json in dictionary representaiton if it
    exists else returns a Pyhton None
    '''
    Crm_dir = os.path.join(env['BUILD_ROOT'], '..' ,'Crm')
    component_json_file_path = os.path.join(Crm_dir, "*Component.json")
    component_json_file_path = glob(component_json_file_path)
    json_data = None
    if len(component_json_file_path) > 0 and os.path.exists(component_json_file_path[0]):
        component_json_file_path = component_json_file_path[0]
        env.PrintInfo("Reading " + component_json_file_path)
        with open(component_json_file_path, 'r') as file_handle:
            file_content = file_handle.read()
            # Current Component.json file is not a valid one hence converting it into valid json
            # by doing some str replace operations and create a new temp.json out of it.
        file_content = file_content.replace("'", r'"')
        file_content = file_content.replace('True', r'"True"')
        file_content = file_content.replace('False', r'"False"')
        try:
            json_data = json.loads(file_content)
        except:
            env.PrintInfo(component_json_file_path + " is invalid.  Can't process red components.")
            json_data = None

    else:
        env.PrintInfo("Component.json is missing under " + Crm_dir + "Can't process red components.")

    return json_data


def create_component_details_json(data,env):
    all_components_details = {}
    env.PrintInfo("Creating Component_details.json ....")
    for component_index in xrange(len(data["component_info"][0]["wcf_info"]["Components"])):
        access_type = data["component_info"][0]["wcf_info"]["Components"][component_index]["AccessType"]
        access_type = ("yellow" if access_type == "standard"
                       else
                       "yellow" if access_type == "restricted"
                       else
                       "red")
        base_name = ".".join(data["component_info"][0]["wcf_info"]["Components"][component_index]["ComponentName"]
                             .split('.')[:2])
        artifact_directory = ''
        if data["component_info"][0]["wcf_info"]["Components"][component_index]["ArtifactWorkspacePath"]:
            artifact_directory = re.search(r'(.*)%/(.*)', data["component_info"][0]["wcf_info"]["Components"]
            [component_index]["ArtifactWorkspacePath"]).group(2)
            artifact_directory = artifact_directory.split('...')[0]
        sync_directory = re.search(r'(.*)%/(.*)', data["component_info"][0]["wcf_info"]["Components"]
        [component_index]["Labels"][0]["LabelPathPairs"][0]["ClientPath"]).group(2)
        sync_directory = sync_directory.split('...')[0]
        all_components_details[base_name] = \
            {'base_name': base_name,
             'access_type': access_type,
             'artifact_directory': artifact_directory,
             'sync_directory': sync_directory}
    component_details_json = os.path.join(env['BUILD_ROOT'], 'build', 'component_details.json')
    with open(component_details_json, 'w') as fp:
        json.dump(all_components_details, fp, sort_keys=True, indent=4,
              separators=(',', ': '))

def read_component_details(env):
    '''
    locates and parses component details file. If not found we return empty dict
    to maintain backward compatibility.
    Populates these environment variables:
        COMPONENT_DETAILS
        RED_COMPONENT_DETAILS
        SCONS_SCRIPTS_TO_IGNORE
    '''
    component_details_dict = {}
    build_component_details_json = os.path.join(env['BUILD_ROOT'], "build", "component_details.json")
    scons_component_details_path = os.path.join(env['BUILD_ROOT'], "tools/build/scons/data")
    scons_component_details_json = os.path.join(scons_component_details_path, "component_details.json")
    mutex_name = "copy_and_read_component_details_json"
    env.AcquireMPMutex(mutex_name)
    if not os.path.exists(build_component_details_json):
        data = get_parsed_component_json_data(env)
        if data:
            create_component_details_json(data, env)
    if os.path.exists(build_component_details_json):
        if not os.path.exists(scons_component_details_path):
            os.makedirs(scons_component_details_path)
        env.PrintInfo("Making local copy of " + build_component_details_json + " as " + scons_component_details_json)
        shutil.copyfile(build_component_details_json, scons_component_details_json)
    if os.path.exists(scons_component_details_json):
        env.PrintInfo("loading component details file: {0}".format(scons_component_details_json))
        try:
            with open(scons_component_details_json) as fh_scons_component_details_json:
                component_details_dict = json.load(fh_scons_component_details_json)
                env.PrintInfo("Successfully loaded component details file.")
        except Exception as e:
            env.PrintError(e.message)
            raise RuntimeError("Component details file found, but the JSON file failed to be parsed!")
    else:
        env.PrintInfo("No component details available.")
    env.ReleaseMPMutex(mutex_name)
    env['COMPONENT_DETAILS'] = component_details_dict
    env['RED_COMPONENT_DETAILS'] = {component : details
                                    for component, details in component_details_dict.items()
                                    if details['access_type'] == 'red'}


    build_root = env.subst('${BUILD_ROOT}').replace('\\', '/')
    build_root_parts = build_root.split('/')
    scons_scripts_to_ignore = {}

    # now populate the fully qualified artifacts and sync dirs
    # and detect if artifacts scripts should be ignored.
    for component, details in env['RED_COMPONENT_DETAILS'].items():
        sync_dir = details['sync_directory']
        qualified_sync_dir = ''
        sync_dir_root_scripts = []
        if sync_dir:
            sync_dir_parts = sync_dir.replace('\\', '/').split('/')
            sync_dir_parts.remove("")
            qualified_sync_dir = "/".join(build_root_parts + [part for part in sync_dir_parts if part not in build_root_parts])

            # locate the component root .scons scripts.
            patterns = ["".join([qualified_sync_dir, "/build/", file_item])
                            for file_item in env.get('SCRIPT_NAME_PATTERNS')]
            # locate the component root .api scripts.
            patterns += ["".join([qualified_sync_dir, "/build/", file_item])
                            for file_item in env.get('API_SCRIPT_NAME_PATTERNS')]

            sync_dir_root_scripts = env.GlobFiles(patterns, posix=True)
        details['qualified_sync_dir'] = qualified_sync_dir
        details['sync_dir_root_scripts'] = sync_dir_root_scripts

        artifact_dir = details['artifact_directory']
        qualified_artifacts_dir = ''
        artifacts_dir_root_scripts = []
        if artifact_dir:
            artifact_dir_parts = artifact_dir.replace('\\', '/').split('/')
            artifact_dir_parts.remove("")
            qualified_artifacts_dir = "/".join(
            build_root_parts + [part for part in artifact_dir_parts if part not in build_root_parts])

            patterns = ["".join([qualified_artifacts_dir, "/build/", file_item])
                        for file_item in env.get('SCRIPT_NAME_PATTERNS')]
            patterns += ["".join([qualified_artifacts_dir, "/build/", file_item])
                        for file_item in env.get('API_SCRIPT_NAME_PATTERNS')]

            artifacts_dir_root_scripts = env.GlobFiles(patterns, posix=True)
        details['qualified_artifacts_dir'] = qualified_artifacts_dir
        details['artifacts_dir_root_scripts'] = artifacts_dir_root_scripts

        # if any of the sync dir scrips exists, we will mark corresponding artifacts scripts
        # to be ignored.
        def build_from_artifacts(env, base, path):
            if 'all' in env.get('BUILD_FROM_ARTIFACTS'):
                return True
            for artifact_path in env.get('BUILD_FROM_ARTIFACTS'):
                if path.startswith('/'.join([base, artifact_path])):
                    return True
            return False

        def get_match(matchee, contenders):
            matchee_fn = '/' + os.path.basename(matchee)
            for contender in contenders:
                if contender.endswith(matchee_fn):
                    return contender
            return None

        rel_sync_dir_root_scripts = [os.path.relpath(x, build_root) for x in sync_dir_root_scripts]
        rel_artifacts_dir_root_scripts = [os.path.relpath(x, build_root) for x in artifacts_dir_root_scripts]

        # The two if/else blocks do complimentary tasks. They create a dictionary whose keys are the
        # scripts that we're ignoring, and whose values are the corresponding scripts that are being
        # processed.
        # If we're building from the artifact directory, we map sync directory to artifact
        # directory, and if we're building from the sync directory, we map artifact directory to sync
        # directory.

        # default to using artifacts
        use_artifacts_dir = True

        if sync_dir_root_scripts:
            # if artifacts dir exists, and we have to build from it, use artifacts!
            if artifacts_dir_root_scripts and build_from_artifacts(env, build_root, artifacts_dir_root_scripts[0]):
                env.PrintInfo("Ignoring the following to build from the artifacts folder: " + str(rel_sync_dir_root_scripts))
                for ignore_script in sync_dir_root_scripts:
                    matching_artifact = get_match(ignore_script, artifacts_dir_root_scripts)
                    scons_scripts_to_ignore[ignore_script] = matching_artifact
                use_artifacts_dir = True

            # if artifacts doesn't exist or if we don't have requirement to build from it, use src!
            else:
                env.PrintInfo("Ignoring the following to build from the red-ip sync folder: " + str(rel_artifacts_dir_root_scripts))
                for ignore_script in artifacts_dir_root_scripts:
                    matching_sync_dir = get_match(ignore_script, sync_dir_root_scripts)
                    scons_scripts_to_ignore[ignore_script] = matching_sync_dir
                use_artifacts_dir = False
        details['ignore_artifacts_dir'] = not use_artifacts_dir

    env['SCONS_SCRIPTS_TO_IGNORE'] = scons_scripts_to_ignore

# End of read_component_details()

def autocorrect_filter_opt(env):
    ''' 
        This automatically fix the filter command by over writting the env['FILTER_OPT'] variable 
        according to the logic based on details['ignore_artifacts_dir']
    '''

    def strip_image_dir(image_path):
        # replacing os specific seperator as '/'
        image_path = image_path.replace('\\', '/')
        
        # slicing the first string which could be modem_proc/qswat_proc
        first_sep = len(image_path.split('/')[0]) + 1
        return image_path[first_sep:]

    def get_match_filter(filter_opt_param, sync_dir, arti_dir):
        match_term = None
        if filter_opt_param.startswith(sync_dir):
            match_term = sync_dir
        if filter_opt_param.startswith(arti_dir):
            match_term = arti_dir
        return match_term

    
    if env['FILTER_OPT'] is not None:
        final_list = []
        for filter_opt_param in env['FILTER_OPT']:
            # replacing os specific seperator as '/'
            filter_opt_param = filter_opt_param.replace('\\', '/')

            # check if os.sep is present or not else append os.sep
            if not (filter_opt_param[-1] == '/'):
                filter_opt_param += '/'

            match = False
            for component, details in env['RED_COMPONENT_DETAILS'].items():
                replace_value = None
                match_term = None
                sync_dir = strip_image_dir(details['sync_directory'])
                arti_dir = strip_image_dir(details['artifact_directory'])
                
                match_filter = get_match_filter(filter_opt_param, sync_dir, arti_dir)

                if match_filter:
                    if details['ignore_artifacts_dir']:
                        replace_value = sync_dir
                    else:
                        replace_value = arti_dir
                
                if replace_value:
                    # replacing and removing extra sep added previously 
                    filter_opt_param = filter_opt_param.replace(match_filter, replace_value)[:-1]
                    final_list.append(filter_opt_param)
                    match = True
                    break

            if not match:
                final_list.append(filter_opt_param[:-1])

        if final_list:
            print 'Replacing filter params as: ' + str(final_list)
            env.Replace(FILTER_OPT=final_list)