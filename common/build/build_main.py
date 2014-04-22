# @PydevCodeAnalysisIgnore
'''
=======================================================================================================
build_main.py:

Usage:
   build_main [<build>:<directory> ...] [<var>=<value> ...]

   Examples:
      build_main modem:M8960AAAAANAAM0045 apps:M8960AAAAANLGA100001 modem_bid=AAAAANAA
      build_main modem:e:\\work\\8960_e\\0045_mb_a apps:M8960AAAAANLGA100001 modem_bid=AAAAANAA

Description:
   This script is used to generate a Metabuild based on input from the command line.
   It takes a series of name/value pairs that be of either of two formats:

build location:
   build can be any of the support build types.  For 8960, these are 'apps', 'modem', 'dsps', 'boot', 'lpass', 'rpm' and 'wcnss'.

   location can be either a directory or a build ID.  If the location is a build ID,
   build_main.py will call 'findbuild' to find the location of the build.

   var=value

   var represents a variable in the contents.xml file, i.e. something of the format
   ${var}.

   value is the new value to assign to that variable.  The resulting format will be
   ${var:value}

build_main.py will perform the following tasks:

   - Validate contents template.
   
   - Update contents.xml with data from the command line.

   - Generate "goto" scripts for each of the named builds.

   - Copy binaries (pointed to by contents.xml) into a fat.bin file.

   - Process partition.xml into rawprogram.xml and patch.xml

The tools to do these last two tasks are found through pointers in the contents.xml file.

 Copyright (c) 2020-2021 by Qualcomm Technologies, Incorporated. All Rights Reserved.
 QUALCOMM Proprietary/GTDR
-------------------------------------------------------------------------------

  $Header: //components/rel/build.glue/1.0/build_main.py#25 $
  $DateTime: 2021/07/06 13:11:49 $
 
===============================================================================
'''

import sys
import os
import stat
import subprocess
import shutil
import re
from glob import glob

print "build_main.py: Executing from", os.getcwd()

import lib.meta_log as lg 
import lib.meta_lib as ml
import lib.update_meta as um
import lib.validate_contents_template as vct
from lib.fb_info_lib  import is_security_bulletin_compliant
from xml.etree import ElementTree as et

#---------------------------------------------------------#
# Find our parameters                                     #
#---------------------------------------------------------#
lg = lg.Logger('build_main')
lg.log("build_main.py:Finding Parameters")
param_file_fn = 'latest_args.txt'
params = sys.argv[1:]
#Check db entry need to create for engg build
engg_build_db_entry = '--nodbentry' not in params
if not engg_build_db_entry:
   params.remove('--nodbentry')
# Flag to check if script needs to be exited after contents template validation
exit_after_vct = False
if len(params) > 0:
   # For script help: consider if '-help' is 1st argument 
   if '-help' in params[0]:
      print __doc__
      sys.exit()
   # Clean build   	  
   if '-clean' in params[0]:
      # Removing old build_main log
      lg.log("build_main.py:Clean build")
      clean_file_list = ['build_main_01.log', '../../contents.xml', '../../build_success.txt', '../../BuildProducts.txt']
      clean_file_list.extend(glob('../../goto_*.py'))
      for file in clean_file_list:
        if os.path.exists(file):
           lg.log('Removing :' + file)
           os.remove(file)
      ml.CleanBuild()
      sys.exit()
   	  
   # Atleast 1 new argument is provided
   # Ignore latest_args.txt and overwrite
   param_file = open(param_file_fn, 'w')
   for p in params:
      param_file.write (p + '\n')
   param_file.close()
elif os.path.exists(param_file_fn):
   # No new args - Read from latest_args.txt
   param_file = open(param_file_fn, 'r')
   params = param_file.readlines()
   param_file.close()
else:
   # Set flag to exit script after validating contents template
   exit_after_vct = True

#-------------------------------------------------------#
# Validating all contents template avialable in glue
#-------------------------------------------------------#
lg.log_highlight("BUILD_MAIN.PY - START VALIDATING CONTENTS TEMPLATE")
contents_template_list = glob('../config/contents_template*.xml')
# True in case there is no default contents template
contents_validation_failed = len(contents_template_list) <= 0 
if contents_validation_failed:
   lg.log("Error: Default contents template is missing!!!")
   sys.exit()
# Found contents template
for content_template in contents_template_list:
   # Start contents validation
   content_template = os.path.abspath(content_template)
   lg.log("Validating contents Template : " + content_template)
   invalid_xml_tag = vct.validate_contents_template(content_template)
   if(len(invalid_xml_tag) > 0):
      lg.log('Error: Contents template validation failed! Please verify below unsupported XML tags\n' + '\n'.join(invalid_xml_tag))
      contents_validation_failed = True 
   else:
      # Validation passed
      lg.log("***Validation PASSED***")

# Abort packaging in case of contents template validation failed 
if contents_validation_failed:
   lg.log("Error: Abort packaging")
   sys.exit()

# User only want to validate contents template (Ex:glue image CRM build)
if exit_after_vct:
   # Create the dummy file "../../build_success.txt" and write something into it
   # this is to indicate to CRM tools that build is successful
   lg.log("Generating build_success.txt")
   file_handle = file('../../build_success.txt', 'w')
   file_handle.write('./build_success.txt')
   file_handle.close()
   # Create 'BuildProducts.txt' file , which will enlist the files/artifacts to check by the CRM. 
   lg.log("Generating BuildProducts.txt")
   file_handle = file('../../BuildProducts.txt', 'w')
   file_handle.write('./build_success.txt')
   file_handle.close()
   lg.log("build_main.py:Done")
   sys.exit()

# Contents template validation passed, procced with meta build packing
ignore_missing_files = False
gen_minimized = False
fb_nearest = False
variant = ''
wflow_filters = ''

si_build_list = []
for p in params[:]:  # [:] syntax makes a copy of the list so we can modify it in the for loop.
   if p.startswith('-imf') or p.startswith('--imf'): 
      ignore_missing_files = True
      params.remove(p)
   elif p.startswith('-minimized') or p.startswith('--minimized'):
     gen_minimized = True
     params.remove(p)
   elif p.startswith('-fb_nearest') or p.startswith('--fb_nearest'):
     fb_nearest = True
     params.remove(p)
   elif p.startswith('variant='):
     variant = p.strip().split('=')[-1]
   elif p.startswith('-params'):
     config_name = p.strip().split('=')[-1]
     param_index = params.index(p)
     root_dir = os.path.join(os.path.dirname(__file__), '../../')
     build_config_path = os.path.join(root_dir , 'build_cfg.xml')
     lg.log("build_config_path: " + build_config_path)
     if os.path.exists(build_config_path):
        # Initialize the XML tree
        lg.ReleaseXmlTree = et.ElementTree(file=build_config_path)
        ReleaseXmlRoot = lg.ReleaseXmlTree.getroot()
        for config in ReleaseXmlRoot.findall('build_params'):
            if config.get('name').upper() == config_name.upper():
                params.remove(p)
                for c in config.text.strip().split():
                    params.insert(param_index, c.strip())
                    param_index += 1
   elif (':' in p):
      tag, si_build = p.strip().split(':', 1)
      si_build_list.append(si_build.strip())

import datetime
# import socket
# lg.log('Current machine is: '   + socket.gethostname())
lg.log('Current directory is: ' + os.getcwd(), verbose=0)
lg.log('Current date/time is: ' + str(datetime.datetime.now())) 
lg.log('Variant: ' + str(variant)) 

# Log the OS environment
lg.log("OS Environment:")
for var in os.environ:
   lg.log("   " + var + ' = ' + os.environ[var])

# Add meta build ID to the XML file
meta_id = 'Unknown'
if 'CRM_BUILDID' in os.environ:
   meta_id = os.environ['CRM_BUILDID']
   lg.log("OS Environment CRM_BUILDID Found")
   engg_build_db_entry = False
elif 'JobName' in os.environ:
   meta_id = os.environ['JobName'].split('_')[0]
   engg_build_db_entry = False
else:
   on_linux = sys.platform.startswith("linux")
   findmetaid_regex = '[/\\\\]([^/\\\\]+)[/\\\\]common[/\\\\]build'
   m = re.search(findmetaid_regex, os.getcwd())
   if (m):
      meta_id = m.group(1).upper()
   engg_build_db_entry = engg_build_db_entry and True
params.append("common:{0}".format(meta_id))

#Abort build for missing bulletin CRs
ret_val = -1
if (('JobName' in os.environ) or ('CRM_BUILDID' in os.environ)) and \
   (len(re.findall("\.\d+\.\d+$", meta_id))<=0) and \
   (len(re.findall("^UB_", meta_id))<=0) and \
   (len(re.findall("^CI_", meta_id)) <= 0):
   lg.log("********************Checking Missing bulletin CRs********************************")
   try:
      if is_security_bulletin_compliant(meta_id, si_build_list):
         lg.log("Error : Meta build compilation aborted for missing security bulletin CRs")  
         lg.log("Approve exceptions : https://psitool.qualcomm.com/buildannouncement/spexception/?spbuild="+ meta_id) 
         sys.exit(ret_val)
   except Exception as x:
      lg.log("Exception : "+ str(x))
      lg.log("Skipping security bulletin compliance check for exception!!!")
   lg.log("*********************************************************************************")

#---------------------------------------------------------#
# UPDATE META
#---------------------------------------------------------#
lg.log_highlight("BUILD_MAIN.PY - CALLING UPDATE_META.PY")  
meta_contents_xml = '../../contents.xml'
meta_contents_template = '../config/contents_template.xml'
if len(variant) > 0:
   template_file = '../config/contents_template_' + variant + '.xml'
   if os.path.exists(template_file):
      meta_contents_template = template_file
   else:
      lg.log("Error : Missing variant contents template - "+ template_file)  
      sys.exit(ret_val)

lg.log("Selected Template file:  " + meta_contents_template)	  
if not os.path.exists(meta_contents_xml):
   shutil.copy(meta_contents_template, meta_contents_xml)
os.chmod(meta_contents_xml, stat.S_IREAD | stat.S_IWRITE | stat.S_IRGRP | stat.S_IWGRP | stat.S_IROTH | stat.S_IWOTH)
#Check if we are compiling meta on cloud environment
local_image = ('LOCAL_IMAGES' in str(os.environ))
ret_val = um.update_meta(lg_obj=lg, pairs=params, fb_nearest=fb_nearest, local_image=local_image)
if ret_val:
   sys.exit(ret_val)

# Create snapshot file list to track generated files list in build process.
ml.SaveFilesSnapshot()

# Initialize meta info object
mi = ml.meta_info(logger=lg)

# BUILD.PY
#---------------------------------------------------------#
lg.log_highlight("BUILD_MAIN.PY - CALLING BUILD.PY")

build_cmd = ['python', 'build.py', '--wflow_filter', wflow_filters]
if ignore_missing_files:
   build_cmd.append('--imf')
ret_val_build = lg.log_exec(build_cmd)
lg.log('return value of build.py: ' + str(ret_val_build))
if(ret_val_build != 0):
   lg.log('Failing the meta-build because of error returned from build.py!!!')
   sys.exit(ret_val_build)

#--------------------------------------------------------------------------#
# Call function to validate file paths in contents.xml (Moved to build.py) #
#--------------------------------------------------------------------------#
#if not ignore_missing_files:
#   mi = ml.meta_info(logger=lg)
#   lg.log_highlight("BUILD_MAIN.PY - VALIDATING FILE PATHS")
#   try:
#      um.validate_filepaths(mi, lg)
#   except ml.InvalidFilePathException, ex:
#      lg.log("build_main.py:InvalidFilePathException exception!!!, invalid file paths found in contents.xml ,count of invalid file paths = " + str(len(ex.invalid_files_list)))
#      for invalidfile in ex.invalid_files_list:
#        lg.log("build_main.py:Invalid file-path: " + invalidfile)
#      lg.log( "build_main.py:InvalidFilePathException occurred")  # exit the program
#      sys.exit(1)
#   except ml.InvalidFileElementTypeException,ex:
#      lg.log("build_main.py:InvalidFileElementTypeException exception!!!, please change the file type of below files to 'download_type' in contents.xml= " + str(len(ex.list_not_with_download_file_types)))      
#      for invalidfile in ex.list_not_with_download_file_types:
#        lg.log("build_main.py:Invalid file-element-type: " + invalidfile +" Change this file type to 'download_type'")
#      lg.log( "build_main.py:InvalidFileElementTypeException occurred")  # exit the program
#      sys.exit(1)        
#   lg.log("build_main.py:============== CHECK COMPLETE ===============\n")
   
#   lg.log("build_main.py:Calling gen_build_manifest")
#   lg.log_exec(['python', 'gen_build_manifest.py'])

#---------------------------------------------------------#
# Generate FFU image for WP
#---------------------------------------------------------#
if os.path.exists("build_relwp.bat"):
   if (lg.log_exec(['build_relwp.bat']) != 0):
      raise Exception, 'build_relwp.bat failed.'

#---------------------------------------------------------#
# Create notes
#---------------------------------------------------------#
for build_name in mi.get_build_list():
   is_on_linux = 0
   build_path = mi.get_build_path(build_name)
   notes_file = build_path + "*_notes.txt"
   notes_file_paths = glob(notes_file) 
   for notes_file_path in notes_file_paths:
      in_file = open(notes_file_path, 'r')
      for line in in_file.readlines():
         if line.find("Host Env") > -1:
            if line.find("Linux") > -1:
               is_on_linux = 1
      if is_on_linux == 1:
         lg.log(build_name + " is built on linux\n")
      in_file.close()

#---------------------------------------------------------#
# Generate minimized build if required
#---------------------------------------------------------#
if gen_minimized:
   lg.log_highlight("BUILD_MAIN.PY - GENERATING MINIMIZED BUILD")
   destn_folder_name = meta_id + '_minibuild'
   try:
      os.makedirs(os.path.join('./../../', destn_folder_name))
      destn_file_path = os.path.join('./../../', destn_folder_name)
      lg.log_exec(['python', 'gen_minimized_build.py', destn_file_path]) 
   except WindowsError, e:
      lg.log("minimized build is already exist, to proceed please delete the already existing folder")

#-----------------------------------------------------------------------------------------------------#
# MetaBuild POST-Verification Tools                                                                   #
# This section includes execution of scripts from other teams to run verification on the build        #
#-----------------------------------------------------------------------------------------------------#

#-----parseContentsXML.py------#
# Purpose: To verify the existence of binaries required for download   
# LABEL: VU_CORE_STORAGE_ZENO_UTILITIES.XX  
# Owner:  CoreBSP Tools 
if (os.path.exists('parseContentsXML.py')):
   lg.log_highlight("BUILD_MAIN.PY - CALLING PARSEContentsXML.PY")
   ret_val_parseContentsXML = lg.log_exec(['python', 'parseContentsXML.py', '--contentsxml=' + os.path.abspath('../../contents.xml')])
   lg.log('\nbuild_main.py: Return value of parseContentsXML.py: ' + str(ret_val_parseContentsXML) + "\n")


#-----------------------------------------------------------------------------#
# Meta compile on cloud environment,upgrading contents XML with network path
#-----------------------------------------------------------------------------#
if local_image:
   ret_val = um.update_meta(lg_obj=lg, pairs=params, fb_nearest=fb_nearest)
   if ret_val:
      sys.exit(ret_val)

# as build is now done, create the dummy file "../../build_success.txt" and write something into it
# this is to indicate to CRM tools that build is successful
lg.log("Generating build_success.txt")
file_handle = file('../../build_success.txt', 'w')
file_handle.write('./build_success.txt')
file_handle.close()

# Create 'BuildProducts.txt' file , which will enlist the files/artifacts to check by the CRM. 
lg.log("Generating BuildProducts.txt")
file_handle = file('../../BuildProducts.txt', 'w')
file_handle.write('./build_success.txt')
file_handle.close()

#----------------------------------------------------------------------#
# Create Database entry for engineering build
#----------------------------------------------------------------------#
if engg_build_db_entry:
   lg.log("Engineering build")
   workspace_root = os.path.abspath(os.path.join(os.getcwd(),'../../'))
   try:
      if sys.platform.startswith("linux"):
         record_eng_meta_command = "./RecordEnggMeta" + " -p " + mi.get_product_name() + " -l " + workspace_root
         lg.log (record_eng_meta_command)
         subproc = subprocess.Popen(['/bin/sh', '-c', record_eng_meta_command], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
      else:
         record_eng_meta_command = 'RecordEnggMeta.exe' +" -p "+ mi.get_product_name() + " -l " + workspace_root
         lg.log (record_eng_meta_command)
      subproc = subprocess.Popen(record_eng_meta_command,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
      stdout, stderr = subproc.communicate()
      if not stderr:
         lg.log(stdout)
      else:
         lg.log(stderr)
         raise Exception(stderr)		 
   except:
      try:
         lg.log("Could not use RecordEnggMeta from environment. Checking network path")
         if sys.platform.startswith("linux"):
            record_eng_meta_command = '/prj/qct/targetsw/targetenggmeta/metascript/bin/RecordEnggMeta' +" -p "+ mi.get_product_name() + " -l " + workspace_root
            lg.log (record_eng_meta_command)
            subproc = subprocess.Popen(['/bin/sh', '-c', record_eng_meta_command], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
         else:
            record_eng_meta_command = "\\\\jerry\\targetenggmeta\\metascript\\bin\\RecordEnggMeta.exe" + " -p "+ mi.get_product_name() + " -l " + workspace_root
            lg.log (record_eng_meta_command)
            subproc = subprocess.Popen(record_eng_meta_command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
         stdout, stderr = subproc.communicate()
         if not stderr:
            lg.log(stdout)
         else:
            lg.log(stderr)
      except:
         lg.log("RecordEnggMeta could not be found or unable to access. Please copy RecordEnggMeta to your environment!")	
		 
lg.log("build_main.py:Done")

