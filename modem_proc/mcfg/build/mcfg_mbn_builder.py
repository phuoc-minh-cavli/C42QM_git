#===========================================================================
#  GENERAL DESCRIPTION:
#    Contains a scons custom build for creating mcfg mbns
#
#  Copyright (c) 2012-2016 Qualcomm Technologies, Inc. (QTI).
#  All Rights Reserved.
#  Qqualcomm Technologies Confidential and Proprietary
#===========================================================================
#
#  OVERVIEW:
#   The following gives an overview for using the sleep_lpr_builder.
#
#
#===========================================================================

import os
import subprocess
import itertools
import fnmatch
import string
import time
import shutil
import re
import xml.etree.ElementTree as ET
from SCons.Script import *

#---------------------------------------------------------------------------
# Globals
#---------------------------------------------------------------------------
CONFIG_XML_LOCATION = 'CONFIG_XML_LOCATION'

#---------------------------------------------------------------------------
# Plugins for the SCons framework
#---------------------------------------------------------------------------
def exists(env):
  return True;

def generate(env):
    ''' 
    Sets up a custom scons builder for generating mcfg mbns
    '''

    env.PrintDebugInfo('mcfg_mbn_builder', 'mcfg mbn builder is getting constructed') 
    rootenv = env.get('IMAGE_ENV')
    rootenv[CONFIG_XML_LOCATION] = []

    mcfg_mbn_source_scanner = env.Scanner(function = mcfg_mbn_scanner, name="McfgMbnScanner", 
                                          skeys = ['.xml', '.XML'], 
                                          recursive=True)
    mcfg_mbn_actn = env.GetBuilderAction(mcfg_mbn_builder, action_source=None)
    mcfg_mbn_bldr = env.Builder(action=mcfg_mbn_actn, emitter = mcfg_mbn_emitter, source_scanner = mcfg_mbn_source_scanner, multi=True)
    rootenv.Append(BUILDERS={'McfgMbnBuilder' : mcfg_mbn_bldr})

    env.PrintDebugInfo('MCFG MBN Builder',' Inside generate function %s' %str(env['IMAGE_NAME']))

    return

def mcfg_mbn_scanner(node, env, path):
    '''
    This will be called once all XML/Excel files are added to the environment. Put
    dependencies on each of them
    '''
    env.PrintDebugInfo('MCFG MBN Builder','In scanner function')
    node_dependencies = []
    node_path = str(node)
    # Only parse MCFG XMLs. Currently, this means any xml starting with "mcfg_..."
    if node_path[-4:] == '.xml' and os.path.basename(node_path).find('mcfg') > -1:
        env.PrintDebugInfo('MCFG MBN Builder','mcfg_mbn_scanner: parsing "%s"' % node_path)
        #print "parse: '%s'" % node_path
        try:
            xml_tree = ET.parse(node_path)
        except:
            print "Could not parse XML file: \n\t", node_path
            raise
      
        root = xml_tree.getroot()
        #for each node in xml tree
        for child in root:
            if child.tag == 'GroupFile' or child.tag == 'NvEfsFile':
                #import pdb; pdb.set_trace()
                fpath = child.attrib.get('buildPath')
                #print "new dep: '%s'" % fpath
                abs_fpath = env.RealPath("${TARGET_ROOT}" + os.sep + fpath)
                node_dependencies.append(abs_fpath)
  
    return env.File(node_dependencies)


def mcfg_mbn_builder(target, source, env):
    mcfg_config_xml_file = str(source[0]).replace("\\","/")
    print mcfg_config_xml_file
    filename = mcfg_config_xml_file.split("/")[-1]
    print filename
    filename_split = filename.split("_")
    config = filename_split[0] + "_" + filename_split[1] + ":" + filename.split("gen_")[1].replace(".xml","")
    dirname= mcfg_config_xml_file.split("mcfg_gen/")[1].split(filename)[0]
    cmd = "perl "+ env.RealPath("${BUILD_ROOT}/mcfg/build/build_mcfgs.pl",posix =True) + " --configs=" + config + " --force-rebuild --source-dir=" + dirname + " --flavor=${SHORT_BUILDPATH} "  + " -xml"
    cmd = env.subst(cmd)
    print cmd
    retval = env.ExecCmds(cmd, dir=env.RealPath('${BUILD_ROOT}/mcfg/build',posix =True)) 
    if retval[2]:
      exit(retval[2])
    return None   
   
def mcfg_mbn_emitter(target, source, env):
   return (target, source)   
