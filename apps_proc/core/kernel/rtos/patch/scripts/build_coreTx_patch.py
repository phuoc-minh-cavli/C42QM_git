
import os
import shutil
import sys

basePath = sys.argv[1]
print ("Patch Script path ::" + basePath)
os.chdir(basePath)
from subprocess import call

#check if diff,grep and cut command exists
try:
    call("diff")
except OSError as e:
    if e.errno == os.errno.ENOENT:
		print "WARNING: 'diff' not found, skipping patch generation"
		sys.exit()
try:
    call("grep")
except OSError as e:
    if e.errno == os.errno.ENOENT:
		print "WARNING: 'grep' not found, skipping patch generation"
		sys.exit()
 
try:
    call("cut")
except OSError as e:
    if e.errno == os.errno.ENOENT:
		print "WARNING: 'cut' not found, skipping patch generation"
		sys.exit()

print "Stating patch generation ... "    

# delete temp files/directories if any
shutil.rmtree("./orig", ignore_errors=True, onerror=None)
shutil.rmtree("./modified", ignore_errors=True, onerror=None)
os.mkdir("./orig")
os.mkdir("./modified")

expLogicBase = "../../ExpressLogic/coreTx/"
qcSrcBase = "../../threadx/src/"
qcIncBase = "../../../../api/kernel/threadx/"
print "EL base : " + expLogicBase
print "qcSrcBase  : " + qcSrcBase
print "qcIncBase  : " + qcIncBase

if not os.path.exists(expLogicBase):
	print "WARNING:"+ expLogicBase +" not found.skipping patch generation"
	sys.exit()
if not os.path.exists(qcSrcBase):
	print "WARNING:"+ qcSrcBase +" not found.skipping patch generation"
	sys.exit()
if not os.path.exists(qcIncBase):
	print "WARNING:"+ qcIncBase +" not found.skipping patch generation"
	sys.exit()
# create listFiles containing all changes src/header file's name
os.system("diff -arqb --ignore-file-name-case " + expLogicBase + " " + qcSrcBase + " | grep Files | cut -d' ' -f2 | rev | cut -d'/' -f1 | rev > listFiles.txt")
os.system("diff -arqb --ignore-file-name-case " + expLogicBase + " " + qcIncBase + " | grep Files | cut -d' ' -f2 | rev | cut -d'/' -f1 | rev >> listFiles.txt")


import os.path
from shutil import copyfile

with open("listFiles.txt", "r") as files:
    for file in files:
		filename = file.rstrip('\n')
		srcFile =  expLogicBase + filename
		if os.path.exists(srcFile):
			copyfile( srcFile , "orig/" + filename )
		else:
			print "WARNING: src file not found : " + srcFile
		ext = filename[-1]
		if ext == 's':
			tmFileName = filename[:-1]+"S"
			filename = tmFileName 
		if os.path.isfile(qcSrcBase + filename):
			copyfile( qcSrcBase + filename , "modified/" + filename )
		elif os.path.isfile(qcIncBase + filename):
			copyfile( qcIncBase + filename , "modified/" + filename )
		else:
			print "WARNING:" + filename + " not found..."
			
os.system("diff -au --ignore-file-name-case orig modified > ../TxCore_Patch.patch")

# delete temp files/directories
shutil.rmtree("./orig", ignore_errors=True, onerror=None)
shutil.rmtree("./modified", ignore_errors=True, onerror=None)
os.remove("listFiles.txt")


