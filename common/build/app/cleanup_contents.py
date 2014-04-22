'''
=======================================================================================================
cleanup_contents.py:

Usage:
   cleanup_contents.py [--src=<Source contents xml file> --tag=<list of tags for the corresponding files to be deleted from contents.xml>] [--dest=<file_name>] [--desc=<package_name>] [--sipath=<buildtag,chipset_sipl,chipset_role,root,spf>]

Examples:
   cleanup_contents.py --tag=["clean_for_release"]

Description:
   This scriptsreads contents.xml and generate contents_release.xml with all the file paths cleaned, removes the file elements that has the given tag as its attribute.

Arguments:
   tag:
      Tag is an iptional parameter to the script. It is the list of attributes in the file_ref element that the user specify to this script. Script searches for the 
      files with these attributes and remove them from contents.xml etree 
      
      Default value of tag is [clean_for_release]
   --ncchange:
      This is an optional parameter. If this param is provided, si_path(which will be chipset_imagerole) case will not be changed to lower and it will be kept same as it is in contents.xml. 
      by default, si_path case will be changed to lower case.
      
 Copyright (c) 2011-2017 by Qualcomm Technologies, Incorporated. All Rights Reserved.
 QUALCOMM Proprietary/GTDR
 
-----------------------------------------------------------------------------------------------
 $Header: //components/rel/build.glue/1.0/app/cleanup_contents.py#11 $
 $DateTime: 2019/03/04 15:25:26 $
===============================================================================================
'''

import sys
import os
import os.path

sys.path.append(os.path.abspath(os.path.join(__file__, '../../lib')))
import meta_log as lg 
import  meta_lib as ml

# Default param value
desc = "HY11"
fName = "contents_release.xml"  # destination xml file
srcFName = "contents.xml"  # source xml file
si_root_path = None
lower_case_si_path = True
# Reading command line arguments
params = sys.argv[1:]
cleanup_list = []
for p in params :
   if p.startswith("--tag"):
      if p.count('='):
         pkey, pvalue = p.strip().split('=', 1)
         cleanup_list = pvalue.strip().split(',', 1)
      elif p.count(':'):
         pkey, pvalue = p.strip().split(':', 1)
         cleanup_list = pvalue.strip().split(',', 1)
   elif p.startswith("--dest"):
      pkey, pvalue = p.strip().split('=', 1)
      fName = pvalue.strip()
   elif p.startswith("--desc"):
      pkey, pvalue = p.strip().split('=', 1)
      desc = pvalue.strip()
   elif p.startswith("--sipath"):
      pkey, pvalue = p.strip().split('=', 1)
      si_root_path = pvalue.strip()
   elif p.startswith("--src"):
      pkey, pvalue = p.strip().split('=', 1)
      srcFName = pvalue.strip()
   elif p.startswith("--ncchange"):
      lower_case_si_path = False
      
# Initialize logger
logfile, ext = fName.split('.', 1)
lg = lg.Logger(logfile)
lg.log("cleanup_contents:Finding Parameters...")
# Remove old file
if os.path.exists(fName):
   os.remove(fName)

lg.log("cleanup_contents.py: Platform is: " + sys.platform)
lg.log("cleanup_contents.py: Python Version is: " + sys.version)
lg.log("cleanup_contents.py: Current directory is: " + os.getcwd())
lg.log("cleanup_contents.py: Parameters: ")

# Read contents.xml and save it as contents_release.xml
mi = ml.meta_info(logger=lg, file_pfn=srcFName)   
mi.save(fName)
mi = ml.meta_info(logger=lg, file_pfn=fName)   

if cleanup_list == [] :
   cleanup_list = ["clean_for_release"] 
# removes release branch ('.r1', '.r*' , etc.) information from product name.   
mi.clean_product_name() 
# resets the build root elements to current or si_root_path directory, cleans all the bid vars.
mi.clean_file_paths(si_root_path_rule=si_root_path, lower_case_si_path=lower_case_si_path)   

# mi.remove_files_with_tag(cleanup_list,desc) # Removes the entries with given attribute.
# construct attribute expression with tags from taglist
# default value of attr will be (clean_for_release==true)||(clean_for_release==HY11)
attr_string = ''
for tag in cleanup_list: 
   if attr_string != '':
      attr_string = attr_string + '||(' + tag + '==true)||(' + tag + '==' + desc + ')'
   else:
      attr_string = attr_string + '(' + tag + '==true)||(' + tag + '==' + desc + ')'
mi.remove_elements_with_attrs (attr_string)

mi.save(fName)  # Saves file to the destination file name "contents_release.xml"
