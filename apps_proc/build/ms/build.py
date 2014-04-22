#!/usr/bin/env python

import os    
import sys   # To process command line arguments
import re    # Advanced string search techniques using regular expressions 
import fnmatch
import time
import fileinput
import subprocess
from subprocess import Popen
from glob import glob
import shlex
from optparse import OptionParser

dict = {'MN_FLASHLESS':'ACIMAAAA',
        'MN_FLASHLESS_NO_AUDIO': 'ACIHAAAA',
        'MN_NAND':'ACINAAAA',
        'MN_FLASHLESS_EXTERNAL':'ACIMAAAZ',
        'MN_FLASHLESS_NO_AUDIO_EXTERNAL': 'ACIHAAAZ',
        'MN_NAND_EXTERNAL':'ACINAAAZ',
		'MN_DIAG_OVER_UART':'ACIRAAAU' ,
		'MN_DIAG_OVER_USB':'ACIRAAAA'}
		
		
#Counting the number of build commands 
NumofBuildCommands=0
BuildIDCheck= sys.argv[1]
#Splitting the build command from BUILD_ID "=" ACI etc 
BuildID= BuildIDCheck.split('=',1)
print("BuildID is "+str(BuildID))

# USES FLAGS CHECK
#Checking to make sure that there are USES FLAGS being used 
uses_flags_input = re.search(r'.*\s+(USES_FLAGS=[a-zA-Z_,]*)\s*(.*)', str(' '.join(sys.argv)))

if uses_flags_input is not None:
 uses_flags_args = uses_flags_input.group(1)
 remainder_cmdline = uses_flags_input.group(2)
 
else: 
 uses_flags_args = " "
 remainder_cmdline = " "
 

# SPLITTING USING , 

if "," in BuildID[1]:  
 
 BuildCommands=BuildID[1].split(',')
 NumofBuildCommands=len(BuildCommands)

 for x in range (0,NumofBuildCommands):
  BUILD_ID_Item=BuildCommands[x]
  BUILD_ID=str(BUILD_ID_Item)
  if(BUILD_ID in dict):
   BUILD_ID=dict[BUILD_ID]
  else: 
  	BUILD_ID=BUILD_ID
	
 

  os.environ["BUILD_ID"]=BUILD_ID    

  if os.name =='nt':  
   print("Running on Windows for multiple commands")
   if  BUILD_ID == ['ACFNAADZ','ACKNAADZ']:
       p=Popen ("build_llvm.cmd llvm_dam " + "BUILD_ID=" + BUILD_ID + " " + uses_flags_args + ' ' + remainder_cmdline)
   else:
       p=Popen ("build_9205.cmd all " + uses_flags_args + ' ' + remainder_cmdline) 
   stdout,stderr=p.communicate()
  elif os.name =='posix':
   if "/apps_proc/build/ms" in str(os.getcwd()):
    cmd = "./build.sh all " + "BUILD_ID="+BUILD_ID + " " +str(uses_flags_args) + ' ' + str(remainder_cmdline)
   else:
    cmd = "./apps_proc/build/ms/./build.sh all " + "BUILD_ID="+BUILD_ID + " " +str(uses_flags_args) + ' ' + str(remainder_cmdline)
    print ("cmd:",cmd)
   formatted_cmd = shlex.split("./build.sh all " + "BUILD_ID="+BUILD_ID + " " +str(uses_flags_args) + ' ' + str(remainder_cmdline))
   print formatted_cmd
   p=Popen (cmd, shell=True)
   stdout,stderr=p.communicate()

else:

 BUILD_ID=str(BuildID[1])
 if(BUILD_ID in dict):
   BUILD_ID=dict[BUILD_ID]
 else: 
   BUILD_ID=BUILD_ID

 
 os.environ["BUILD_ID"]=BUILD_ID
 
 if os.name =='nt':  
   print("Running on Windows for single commands")
   p=Popen ("build_9205.cmd all " + uses_flags_args + ' ' + remainder_cmdline) 
   stdout,stderr=p.communicate()
 elif os.name =='posix':
   print("Running on Linux for single commands")
   if "/apps_proc/build/ms" in str(os.getcwd()):
    cmd = "./build.sh all " + "BUILD_ID="+BUILD_ID + " " +str(uses_flags_args) + ' ' + str(remainder_cmdline)
   else:
    cmd = "./apps_proc/build/ms/build.sh all " + "BUILD_ID="+BUILD_ID + " " +str(uses_flags_args) + ' ' + str(remainder_cmdline)
   formatted_cmd = shlex.split("./build.sh all " + "BUILD_ID="+BUILD_ID + " " +str(uses_flags_args) + ' ' + str(remainder_cmdline))
   print formatted_cmd
   p=Popen (cmd, shell=True)
   stdout,stderr=p.communicate()
 

print ("wrapper.py: merging log files, in:", os.getcwd())
log_file_list = sorted(glob("build-log-*.txt"))
last_n_log_files = log_file_list[len(log_file_list)-(NumofBuildCommands-1) : ]
print "   Looking for:", last_n_log_files

fout=open("build-log.txt","a")
for log_file in last_n_log_files:
 fin=open(log_file,"r")
 data2=fin.read()
 fin.close()
 fout.write(data2)
fout.close()
