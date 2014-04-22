# --------------------------------------------------------------------------- #
# Description   ::  Defines the SCons Builder for LTE PLT Autogen
# --------------------------------------------------------------------------- #
# $Header: //components/rel/nb1.mpss/2.2/PLT/build/nb1_plt_builder.py#1 $
# --------------------------------------------------------------------------- #
# Copyright (c) 2009 Qualcomm Technologies Incorporated.
#
# All Rights Reserved. Qualcomm Confidential and Proprietary
# Export of this technology or software is regulated by the U.S. Government.
# Diversion contrary to U.S. law prohibited.
#
# All ideas, data and information contained in or disclosed by
# this document are confidential and proprietary information of
# Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
# By accepting this material the recipient agrees that this material
# and the information contained therein are held in confidence and in
# trust and will not be used, copied, reproduced in whole or in part,
# nor its contents revealed in any manner to others without the express
# written permission of Qualcomm Technologies Incorporated.
# --------------------------------------------------------------------------- #

# --------------------------------------------------------------------------- #
#                       IMPORT MODULES & DEFINITONS                           #
# --------------------------------------------------------------------------- #
import SCons.Scanner
import os
import os.path
import sys


# --------------------------------------------------------------------------- #
#                       LOCAL VARIABLE DEFINITIONS                            #
# --------------------------------------------------------------------------- #

#------------------------------------------------------------------------------
#                      SCONS HOOKS (exist/generate)
#------------------------------------------------------------------------------
def exists(env):
    return env.Detect('nb1_plt_builder')

def generate(env):
    plt_act = env.GetBuilderAction(plt_builder_llvm)
    plt_bld = env.Builder(action = plt_act, 
                          source_scanner = SCons.Scanner.C.CScanner(),
                          emitter = plt_emitter_llvm)
    env.Append(BUILDERS = {'NB1PLT' : plt_bld})


#-------------------------------------------------------------------------------
# NB1 PLT Autogen emitter function
#
def plt_emitter_llvm(target, source, env):
    # Get the inputs and outputs into variables
    header_file = str(source[0])
    c_file      = str(target[0])

    env.PrintDebugInfo('plt', "hdr: " + header_file)
    env.PrintDebugInfo('plt', "src: " + c_file)

    # Output goes in the same location as .c file
    output_dir = os.path.dirname(c_file)
    # Output filenames are based on input .stm filename
    filebase = os.path.splitext(os.path.basename(header_file))[0]
    doti_file   = os.path.join(output_dir, filebase + '.i')
    pm_file     = os.path.join(output_dir, 'APSSubSystem68Metadata.pm')
    
    # Preprocess the SMF -> SMI
    doti = env.PreProcess(doti_file, header_file)
    env.Depends(target, doti)

    # Update the target list
    target = [env.File(c_file),
              env.File(pm_file),
              ]
    # Update the source list
    source = [source[0], doti] + source[1:]

    return (target, source)


#-------------------------------------------------------------------------------
# LTE PLT Autogen builder function
#
def plt_builder_llvm(target, source, env):
    PLT_DIR = "${NB1_ROOT}/PLT/src"

    hexagon_version = os.environ.get('HEXAGON_RTOS_RELEASE', '1.1')
    
    header_file  = str(source[0])
    doti_file    = str(source[1])
    xml_files    = [str(x) for x in source[2:]]
    
    c_file      = str(target[0])
    pm_file     = str(target[1])

    # build command
    if sys.platform.startswith("linux"):
        #py_path = "/pkg/qct/software/python/2.6.2/bin/python "
        plt_cmd = "python " + PLT_DIR + "/nb1_plt_parser.py"
    else:
        plt_cmd = PLT_DIR + "/win64/nb1_plt_parser.exe"

    plt_cmd += ' -v ' + hexagon_version
    plt_cmd += ' -p ' + pm_file
    plt_cmd += ' -o ' + c_file
    plt_cmd += ' -r ' + env['HEXAGON_ROOT']
    plt_cmd += ' -x ' + ' -x '.join(xml_files)
    plt_cmd += ' ' + doti_file

    env.PrintInfo('PLT Cmd: ' + plt_cmd)

    # run command(s)
    data, err, rv = env.RunCmds([env.File(header_file)],
                                [env.File(doti_file)],
                                plt_cmd)
