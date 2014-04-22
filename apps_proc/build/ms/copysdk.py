# Copyright (c) 2011-2015.  QCT, Inc.
#All rights reserved.

#QCT Confidential and Proprietary.
#This script will copy libs to a top level include folder.
# Usage- python copyheaders.py <ROOT_DIR>

#!/usr/bin/python
import sys
import getopt
import re
import os, fnmatch
import shutil
import stat
import time

print("started copying for final SDK folder")


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

if os.path.exists('data_iot_sdk/CMCC/'):
    shutil.rmtree('data_iot_sdk/CMCC/', onerror=del_rw)
    time.sleep(1)

ensure_dir('data_iot_sdk/CMCC/Sample_Application')
copytree('data_iot_sdk/CMCC_SDK/CMCC/test_app', 'data_iot_sdk/CMCC/Sample_Application', symlinks=False, ignore=None)
find('onenet_at.bin','data_iot_sdk/CMCC_SDK/CMCC_build/bin','data_iot_sdk/CMCC')
find('onenet_sdk.lib','data_iot_sdk/CMCC_SDK/CMCC_build/libs','data_iot_sdk/CMCC')
find('build_cmcc_onenet_app_llvm.sh','data_iot_sdk/CMCC_SDK/CMCC_build','data_iot_sdk/CMCC/Sample_Application')
find('txm_module_app_preamble_llvm.S','data_iot_sdk/CMCC_SDK/CMCC','data_iot_sdk/CMCC/Sample_Application')
find('txm_module_app_preamble_llvm.ld','data_iot_sdk/CMCC_SDK/CMCC_build','data_iot_sdk/CMCC/Sample_Application')

if os.path.exists('data_iot_sdk/CMCC/Sample_Application/qcli'):
    shutil.rmtree('data_iot_sdk/CMCC/Sample_Application/qcli', onerror=del_rw)
    time.sleep(1)

os.remove('data_iot_sdk/CMCC/Sample_Application/inc/iot_app_cli.h')
os.remove('data_iot_sdk/CMCC/Sample_Application/src/iot_app_cli.c')
if os.path.exists('data_iot_sdk/CMCC/Sample_Application/build'):
    shutil.rmtree('data_iot_sdk/CMCC/Sample_Application/build', onerror=del_rw)
    time.sleep(1)


print "done copying"
if os.path.exists('data_iot_sdk/CMCC/Sample_Application/test_app/qcli'):
    print("copying not done")

os.chdir('build/ms')


