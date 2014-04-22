#===============================================================================
# Copyright (c) 2017 by Qualcomm Technologies Inc.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#===================================================================================
import os

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
    return True

def generate(env):
    env.AddMethod(QurtQdiRegister, "QurtQdiRegister")
    qurtQDISrcBuilder_generate(env)
    return


###############################################################################
class qdiInfo:
    def __init__(self, target, device, opener, invocation, release, autogen_init_func_name, rcinit_dep_list, singleton, island_capable, rcinit_sequence_group):
        if target is "":
            target = "${BUILDPATH}/" + device + "_qdi_init.c"
        if autogen_init_func_name=="":
            autogen_init_func_name = device + "_init"
        if singleton is "":
            singleton = 2 #default non-signleton
        self.target = target
        self.device = device
        self.opener = opener
        self.invocation = invocation
        self.release = release
        self.autogen_init_func_name = autogen_init_func_name
        self.rcinit_dep_list = rcinit_dep_list
        self.singleton =singleton
        self.island_capable = island_capable
        self.rcinit_sequence_group = rcinit_sequence_group

#Methods
###############################################################################
#notes:
#accepting default target implies $BUILDPATH is set in the environment
def QurtQdiRegister(env, device, opener, invocation, release, target="", singleton=2, island_capable=0, autogen_init_func_name="", do_rcinit=False, rcinit_dep_list=[], rcinit_sequence_group='RCINIT_GROUP_0'):
    deviceInfo = qdiInfo(target, device, opener, invocation, release, autogen_init_func_name, rcinit_dep_list, singleton, island_capable, rcinit_sequence_group)

    #by default we do rc-initialization 
    if (do_rcinit ==True):
        #call rcinit with init function
        if 'USES_RCINIT' in env:
            RCINIT_IMG = ['CBSP_QDSP6_SW_IMAGE']
            RCINIT_INFO = { 'sequence_group' :deviceInfo.rcinit_sequence_group,
                            'init_name'      :deviceInfo.device,
                            'init_function'  :deviceInfo.autogen_init_func_name,
                            'dependencies'   :deviceInfo.rcinit_dep_list
                          }
            env.AddRCInitFunc(RCINIT_IMG, RCINIT_INFO)

    #call builder
    return env.QurtQDISrcBuilder(deviceInfo.target, [], DEVICEINFO=deviceInfo)

#Builders
###############################################################################
def qurtQDISrcBuilder_generate(env):
    env.Append(BUILDERS = {'QurtQDISrcBuilder' : env.Builder(action = qurtQDISrcBuilder)})

def qurtQDISrcBuilder(env, target, source):
    deviceInfo = env.get('DEVICEINFO')
    #get template as string
    templateStr=open('{0}/Input/qdi_init_template.c'.format(os.path.dirname(os.path.realpath(__file__)), ), 'r').read()

    #generate c file
    target = str(target[0])
    fileHandle = open(target, 'w')
    fileHandle.write(templateStr.format(deviceInfo.opener,deviceInfo.invocation,deviceInfo.release,deviceInfo.device,deviceInfo.autogen_init_func_name,deviceInfo.island_capable, deviceInfo.singleton))
    fileHandle.close()

#helper functions
###############################################################################