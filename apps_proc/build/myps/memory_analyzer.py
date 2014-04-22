import os, subprocess, sys
import os.path

QCMAT_MEM_TOOL_PATH = "${BUILD_ROOT}/build/myps/QcMAT40/qcmat.py"
QCMAT_REPORT_TOOL_PATH = "${BUILD_ROOT}/build/myps/QcMAT40/qcmatreport.py"

TEST = True
ERR_MEMORY_ANALYZER = "Memory Analyzer tool failure"
MEMORY_ANALYZER_DONE = "Done running QcMAT tool"
ERR_MEMORY_ANALYZER_REPORT = "Memory Analyzer Report failure"
MEMORY_ANALYZER_REPORT_DONE = "Done running QcMAT report"
MEMORY_ANALYZER_OUTPUT_MISSING = "qcmat csv outputfile missing"
MEMORY_EXCEEDS_BUDGET = "Actual memory exceeds budget. Aborting build."      


def exists(env):
   return env.Detect('qcmat_builder')

def generate(env):
   qcmat_generate(env)
   
def  qcmat_generate(env):
   QCMAT_act = env.GetBuilderAction(qcmat_builder)
   QCMAT_bld = env.Builder(action = QCMAT_act,
                        emitter = QCMAT_emitter, 
                         suffix = '.elf')

   env.Append(BUILDERS = {'QCMAT_ANALYSIS' : QCMAT_bld})

def QCMAT_emitter(target, source, env):
   env.Depends(target, source)  
   return (target, source) 

def die(msg):
   myEnv.PrintError(msg)
   #SCons.Script.Exit(1)

def run_mem_tool(qcmat_cmd):
   if TEST: print qcmat_cmd
   cmd = subprocess.Popen(qcmat_cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
   stdout, stderr = cmd.communicate()
   if stderr:
   	  myEnv.PrintInfo(str(stderr))
   	  die(ERR_MEMORY_ANALYZER)
   if stdout:
      myEnv.PrintInfo(str(stdout))
      myEnv.PrintInfo(MEMORY_ANALYZER_DONE)

def qcmat_builder(env,source,target):
   """Assume qcmat tool exists in the same directory
   target : [ <csvout> ]
   source : [ <map>, <elf>, <budgetfile> ]
   """
   env.PrintInfo("memory analyzer builder")
   global myEnv
   myEnv = env

   mapfile = str(source[0])
   elffile = str(source[1])
   csvout = str(target[0])
   qmafout = str(target[0]).replace(".csv",".qmaf")
   
   qcmat_cmd = "python {TOOLPATH} -m \"{mapfile}\" -e \"{elffile}\" -o \"{qmafout}\" -bt {btype} -csv {csvout}"# -html {htmlout}"

   qmat_arg = {
      'TOOLPATH' : env.RealPath(QCMAT_MEM_TOOL_PATH, posix=True),
      'mapfile' : mapfile,
      'elffile' : elffile,
      'qmafout' : qmafout,
      'btype' : 'ADSP',
      'csvout' : csvout,
      #'htmlout' : htmlout,
   }
   qcmat_cmd = qcmat_cmd.format(**qmat_arg)

   run_mem_tool(qcmat_cmd)
