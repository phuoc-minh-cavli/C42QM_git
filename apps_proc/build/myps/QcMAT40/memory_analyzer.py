import os, subprocess, sys
import os.path
from collections import OrderedDict


QCMAT_MEM_TOOL_PATH = "${BUILD_ROOT}/build/myps/QcMAT40/qcmat.py"
QCMAT_REPORT_TOOL_PATH = "${BUILD_ROOT}/build/myps/QcMAT40/qcmatreport.py"

TEST = True
#TEST = False
ERR_MEMORY_ANALYZER = "Memory Analyzer tool failure"
MEMORY_ANALYZER_DONE = "Done running QcMAT tool"
ERR_MEMORY_ANALYZER_REPORT = "Memory Analyzer Report failure"
MEMORY_ANALYZER_REPORT_DONE = "Done running QcMAT report"
MEMORY_ANALYZER_OUTPUT_MISSING = "qcmat csv outputfile missing"
MEMORY_EXCEEDS_BUDGET = "Actual memory exceeds budget. Aborting build."      

def exists(env):
   return env.Detect('memory_analyzer_builder')

def generate(env):
   if 'USES_MEMORY_ANALYZER' in env:
      memory_analyzer_generate(env)

def memory_analyzer_generate(env):
   env.AddMethod(memory_analyzer_builder_wrapper, "RunMemoryAnalyzer")
   memory_analyzer_act = env.GetBuilderAction(memory_analyzer_builder)
   memory_analyzer_bld = env.Builder(
      action = memory_analyzer_act,
      emitter = memory_analyzer_emitter,
   )
   env.Append(BUILDERS = {'MemoryAnalyzerInternalBuilder' : memory_analyzer_bld})

def memory_analyzer_builder_wrapper(env,target,source):
   return env.MemoryAnalyzerInternalBuilder(target, source)

def die(msg):
   if TEST:
      print msg
      sys.exit(1)
   else:
      myEnv.PrintError(msg)
      #SCons.Script.Exit(1)

def memory_budget_parser(mem_budget_path):
   rv = OrderedDict()
   mbfp = myEnv.subst(mem_budget_path)
   with open(mbfp, 'r') as f:
      lines = f.readlines()
   isProcessingPath = False 
   budget_row_index = 6
   for l in lines:
      l = l.strip()
      row = l.split(',')
      if l == '' or l.replace(",", "") == '' or 'total' in row[0].lower():
         isProcessingPath = False
         continue
      if 'module' in row[0].lower():
         path_prefix = row[budget_row_index]
         path_prefix = path_prefix[path_prefix.index('(')+1 : path_prefix.index(')')]
         isProcessingPath = True
         continue
      if isProcessingPath:
         path =  "%s/%s" % (path_prefix, row[0]) 
         #print "path:"+path
         rv[path] = row[budget_row_index]
   return rv

def memory_profile_parser(csvout):
   if TEST:
      myEnv.PrintInfo("csvout")
      myEnv.PrintInfo(csvout)
   MEMORY_ANALYSIS_PATH_INDICATOR = "Memory Analysis of :: "
   rv = OrderedDict()
   with open(csvout, 'r') as f:
      lines = f.readlines()
   isProcessingPath = False
   for l in lines:
      l = l.strip()
      if l == '' or l.replace(",", "") == '':
         isProcessingPath = False
         continue
      if MEMORY_ANALYSIS_PATH_INDICATOR in l:
         path_prefix = l.replace(MEMORY_ANALYSIS_PATH_INDICATOR, '')
         isProcessingPath = True
         continue
      if isProcessingPath:
         #if TEST: print "current line:" + l
         row = l.split(",")
         component = row[0]
         row_total = row[5]
         path = "%s/%s" % (path_prefix, component) # component here refers to individual pieces under this path
         rv[path] = row_total
   return rv

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

def process_qcmaf_out(qcmat_report_cmd):
   cmd = subprocess.Popen(qcmat_report_cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
   stdout, stderr = cmd.communicate()
   if stderr:
      die(ERR_MEMORY_ANALYZER_REPORT)
   if stdout:
      print stdout
      print MEMORY_ANALYZER_REPORT_DONE

def compare_profile_to_budget(mem_budget_path, mapfile, qcmat_csvout, final_csvout):
   if not os.path.exists(qcmat_csvout):
      die(MEMORY_ANALYZER_OUTPUT_MISSING)
   # ordered dict { <path> : <memory profile/budget> } 
   # ordered by granularity. core/ first then core/<subsys> then core/<subsys>/<area>
   budget_dict = memory_budget_parser(mem_budget_path)
   profile_dict = memory_profile_parser(qcmat_csvout)
   if TEST:
      print "budget_dict"
      print budget_dict
      #print "profile_dict"
      #print profile_dict
   exceeds_budget = {}
   budget_delta = {}
   for k in budget_dict:
      if not (k in profile_dict):
         myEnv.PrintInfo("found in budget but not in memory profile")
         myEnv.PrintInfo(str(k))
         continue
      overflow = int(profile_dict[k]) - int(budget_dict[k])
      budget_delta[k] = [profile_dict[k], budget_dict[k], overflow] 
      if overflow > 0:
         exceeds_budget[k] = [profile_dict[k], budget_dict[k], overflow] # actual, budget, overflow

   header = ["path,actual,budget,overflow"]
   sorted_budget_paths = budget_delta.keys()
   sorted_budget_paths.sort()
   data = ["\n%s,%s,%s,%d" % tuple([k]+budget_delta[k]) for k in sorted_budget_paths]
   with open(final_csvout, 'w') as f:
      f.writelines(header+data)

   if exceeds_budget:
      myEnv.PrintInfo("Detected Memory Budget Violation on following modules")
      insert_ws = lambda s, col : str(s) + ' ' * (col - len(str(s)))
      headings = header[0].split(',')
      ph = (
      	insert_ws(headings[0], 60), 
      	insert_ws(headings[1], 8), 
      	insert_ws(headings[2], 8),
      	insert_ws(headings[3], 8),
      	)
      print "%s%s\t%s\t%s" % ph
      for i in exceeds_budget:
      	 ebp = (
      	 	insert_ws(i, 60), 
      	 	insert_ws(exceeds_budget[i][0], 8), 
      	 	insert_ws(exceeds_budget[i][1], 8),
      	 	insert_ws(exceeds_budget[i][2], 8),
      	 	)
      	 print "%s%s\t%s\t%s" % ebp
      if 'USES_MEMOERY_ANALYSIS_BUILD_BLOCKER' in myEnv:
         die(MEMORY_EXCEEDS_BUDGET)
   myEnv.PrintInfo("Memory analysis finished.")

def memory_analyzer_builder(target, source, env):
   """Assume qcmat tool exists in the same directory
   target : [ <csvout> ]
   source : [ <map>, <elf>, <budgetfile> ]
   """
   env.PrintInfo("memory analyzer builder")
   global myEnv
   myEnv = env
   diff_mode = False

   mapfile = str(source[0])
   elffile = str(source[1])
   mem_budget_path = env.get("mem_budget_path")
   final_csvout = str(target[0])
   qmafout = str(target[1])
   qcmat_csvout = str(target[2])
   #htmlout = str(target[3])

   index = qmafout.find('output.qmaf') 
   qmafout2 = qmafout[:index] + "prevqmaf/" + qmafout[index:]
   if os.path.isfile(qmafout2):
       diff_mode = True
       #print "DInfo: diff path available\n"

   qcmat_cmd = "python {TOOLPATH} -m \"{mapfile}\" -e \"{elffile}\" -o \"{qmafout}\" -bt {btype} -csv {csvout}"# -html {htmlout}"

   qmat_arg = {
      'TOOLPATH' : env.RealPath(QCMAT_MEM_TOOL_PATH, posix=True),
      'mapfile' : mapfile,
      'elffile' : elffile,
      'qmafout' : qmafout,
      'btype' : 'WCNSS',
      'csvout' : qcmat_csvout,
      #'htmlout' : htmlout,
   }
   qcmat_cmd = qcmat_cmd.format(**qmat_arg)


   qcmat_report_cmd = "python {TOOLPATH} -q1 {qmaf} -csv {csv}" #" -html {html}"
   qcmat_report_arg = {
      'TOOLPATH' : env.RealPath(QCMAT_REPORT_TOOL_PATH, posix=True),
      'qmaf': qmafout,
      'csv' : qcmat_csvout,
      #'html' : htmlout,
   }
   qcmat_report_cmd = qcmat_report_cmd.format(**qcmat_report_arg)

   if diff_mode:
       qcmat_report_cmd = qcmat_report_cmd + " -q2 " + qmafout2
       #print "\nDInfo qcmat_cmd: " + qcmat_report_cmd + "\n"

   if TEST:
      env.PrintInfo("qcmat cmd: " + qcmat_cmd)
      env.PrintInfo("qcmat report cmd: " + qcmat_report_cmd)
      env.PrintInfo("current working directory: " + os.getcwd())
   run_mem_tool(qcmat_cmd)
   process_qcmaf_out(qcmat_report_cmd)
   if os.path.exists(mem_budget_path):
      compare_profile_to_budget(mem_budget_path, mapfile, qcmat_csvout, final_csvout)
   else:
      env.PrintInfo("No budget file found at " + mem_budget_path)
      env.PrintInfo("Skipping memory comparison...")

def memory_analyzer_emitter(target, source, env):
   final_csvout = str(target[0])
   outdir = os.path.dirname(final_csvout)
   if not os.path.exists(outdir):
      os.mkdir(outdir)
   target += [
      outdir + "/output.qmaf",
      outdir + "/memory_breakdown.csv",
      #outdir + "/memory_breakdown_html",
   ]
   env.Replace(mem_budget_path=str(source[2]))
   return (target, source[:2])
