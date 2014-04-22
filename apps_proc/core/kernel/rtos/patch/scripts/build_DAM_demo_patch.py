
import os
import shutil
import sys

basePath = sys.argv[1]
os.chdir(basePath)
from subprocess import call
try:
    call("diff")
except OSError as e:
    if e.errno == os.errno.ENOENT:
		print "WARNING: 'diff' not found, skipping patch generation"
		sys.exit()
		
elDemoBase = "../../ExpressLogic/tx_DAM_demo_app"
qcDemoBase = "../../threadx/txm_demo_app/src"
if not os.path.exists(elDemoBase):
	print "WARNING:"+ elDemoBase +" not found.skipping patch generation"
	sys.exit()
if not os.path.exists(qcDemoBase):
	print "WARNING:"+ qcDemoBase +" not found.skipping patch generation"
	sys.exit()

os.system("diff -au --ignore-file-name-case  "+ elDemoBase +" " + qcDemoBase +" > ../TxDAM_demo_app_patch.patch")