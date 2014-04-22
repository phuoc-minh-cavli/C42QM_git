# Copyright (c) 2011-2015.  QCT, Inc.
#All rights reserved.

#QCT Confidential and Proprietary.
#This script will copy libs to a top level AZURE folder.
# Usage- python copyazuresdk.py <ROOT_DIR>

#!/usr/bin/python
import sys
import getopt
import re
import os, fnmatch
import shutil
import stat
import time

print("started copying for final Azure SDK folder")


def del_rw(action, name, exc):
    os.chmod(name, os.stat(name).st_mode | stat.S_IWUSR)
    if os.path.isdir(name):
        # Give a slight pause just in case the directory isn't emptied yet
        time.sleep(1)
        os.rmdir(name)
    else:
        os.remove(name)
		
def copy_file(path, file, dest):
    srcfile=os.path.join(path, file)
    destfile=os.path.join(dest, file)
    print('Copying {0} to {1}'.format(file, dest))
    shutil.copy(srcfile, dest)
    os.chmod(destfile, os.stat(destfile).st_mode | stat.S_IWUSR)

def find(pattern, path, dest):
    result = []
    no_op = 0
    for root, dirs, files in os.walk(path):
        for name in files:
            if fnmatch.fnmatch(name, pattern):
                copy_file(root, name, dest)


def copytree(src, dst, symlinks=False, ignore=None):
    for item in os.listdir(src):
        s = os.path.join(src, item)
        d = os.path.join(dst, item)
        if os.path.isdir(s):
            shutil.copytree(s, d, symlinks, ignore)
        else:
            shutil.copy2(s, d)
def ensure_dir(f):
    if not os.path.exists(f):
        os.makedirs(f)


#changing directory
		
os.chdir('../..')

if os.path.exists('data_iot_sdk/AZURE/'):
    shutil.rmtree('data_iot_sdk/AZURE/', onerror=del_rw)
    time.sleep(1)

ensure_dir('data_iot_sdk/AZURE/')
#copytree('data_iot_sdk/AZURE_SDK/build/libs/', 'data_iot_sdk/CMCC/Sample_Application', symlinks=False, ignore=None)
find('azure_sdk_port.lib','data_iot_sdk/AZURE_SDK/build/libs','data_iot_sdk/AZURE')
find('build_azure_sdk_app_llvm.sh','data_iot_sdk/AZURE_SDK/build','data_iot_sdk/AZURE')
find('build_azure_sdk_lib_llvm.sh','data_iot_sdk/AZURE_SDK/build','data_iot_sdk/AZURE')
#find('txm_module_preamble_llvm.S','data_iot_sdk/AZURE_SDK/test-app/src','data_iot_sdk/AZURE')
#find('txm_module_app_preamble_llvm.ld','data_iot_sdk/CMCC_SDK/CMCC_build','data_iot_sdk/CMCC/Sample_Application')

if os.path.exists('data_iot_sdk/AZURE/azure_sdk_port.lib'):
    print "done copying the files to AZURE folder"
else:
	print("Copying of Azure SDK files is not done")

os.chdir('build/ms')


