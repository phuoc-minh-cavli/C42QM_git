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

def del_rw(action, name, exc):
    os.chmod(name, os.stat(name).st_mode | stat.S_IWUSR)
    if os.path.isdir(name):
        # Give a slight pause just in case the directory isn't emptied yet
        time.sleep(1)
        os.rmdir(name)
    else:
        os.remove(name)

def ensure_dir(f):
    if not os.path.exists(f):
        os.makedirs(f)

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


#Move to Top level directory
print("Initiating IOT HEADER Copy")


if len(sys.argv) < 2:
    print("ERROR: Missing BUILD ROOT")
    sys.exit(1)

print(sys.argv[1])
os.chdir(sys.argv[1])

if len(sys.argv) == 3:
    variant = sys.argv[2]
    
#Remove old dir if exists
if os.path.exists('include/'):
    shutil.rmtree('include/', onerror=del_rw)
    time.sleep(1)

if os.path.exists('data_iot_sdk/common/include/'):
    shutil.rmtree('data_iot_sdk/common/include/', onerror=del_rw)
    time.sleep(1)

if os.path.exists('data_iot_sdk/common/libs'):
    shutil.rmtree('data_iot_sdk/common/libs', onerror=del_rw)
    time.sleep(1)
	
if os.path.exists('data_iot_sdk/common/src'):
    shutil.rmtree('data_iot_sdk/common/src', onerror=del_rw)
    time.sleep(1)

#Create destination directory
ensure_dir('include/qapi')
ensure_dir('include/threadx_api')
ensure_dir('include/libs')
ensure_dir('include/src')
ensure_dir('data_iot_sdk/sample_apps/location')



#Find and copy qapi headers.
find('qapi*.h', 'dataservices/api','include/qapi')
find('version_dataservice_def.h', 'dataservices/api','include/qapi')
find('qapi*.h', 'ril/api','include/qapi')
find('version_modem_feature_def.h', 'ril/api','include/qapi')
find('qapi*.h', 'location/api','include/qapi')
find('version_location_def.h', 'location/api','include/qapi')
find('qapi*.h', 'core/api','include/qapi')
find('qapi*.h', 'core/buses/api','include/qapi')
find('qapi*.h', 'core/qapi/common','include/qapi')
find('version_core_def.h', 'core/qapi/common','include/qapi')
find('qapi_IOpener.h', 'core/securemsm/ship/qapi/QTEEInvoke/inc','include/qapi')
find('qapi_QTEEInvoke.h', 'core/securemsm/ship/qapi/QTEEInvoke/inc','include/qapi')
find('qapi_QSEECom.h', 'core/securemsm/ship/qapi/qseecom/inc','include/qapi')
find('object.h', 'core/securemsm/ship/mink/inc/interface','include/qapi')
find('qseecom.h', 'core/securemsm/qseecom/driver/src','include/qapi')
find('msgcfg.h', 'core/api/services','include/qapi')
find('msgtgt.h' , 'core/api/services','include/qapi')
find('msg_mask.h' ,'core/api/services','include/qapi')
find('com_dtypes.h' ,'core/api/services','include/qapi')
find('qflog_ext_api.h','dataservices/api/qflog_ext','include/qapi')
find('qflog_utils.h','dataservices/api/qflog_ext','include/qapi')
find('stringl.h','core/api','include/qapi')
find('*.h','core/api/kernel/threadx','include/threadx_api')
find('qurt_timetick.h','core/api/kernel/qurt','include/threadx_api')
find('at_fwd_ext_api.h','dataservices/api/atfwd_ext','include/qapi')
find('qapi_usb_user.c','core/wiredconnectivity/hsusb/driver/src/adapt_layers','include/src')
find('qapi_diag_user.c','core/services/diag/LSM/qapi/src','include/src')
find('qapi_tsens_user.c','core/hwengines/tsens/qapi/src','include/src')
find('qapi_adc_user.c','core/hwengines/adc/qapi/src','include/src')
find('qapi_gpioint_user.c','core/systemdrivers/GPIOInt/src','include/src')
find('qapi_uart_user.c','core/buses/uart/mdd/src','include/src')
find('qapi_i2c_user.c','core/buses/i2c/src/drv','include/src')
find('qapi_spi_user.c','core/buses/spi/src/driver','include/src')


#copying libs
find('txm_lib.lib','core/kernel/rtos/threadx/txm_module_lib/build/LLVM','include/libs')
find('timer_dam_lib.lib','core/services/time_qapi/build/LLVM','include/libs')
find('diag_dam_lib.lib','core/services/diag/build/LLVM','include/libs')
find('qapi_psm_lib.lib','ril/psm/qapi_lib/build/LLVM','include/libs')
find('qapi_dam.lib','core/buses/uart/build/LLVM','include/libs')

#creating directory in common
ensure_dir('data_iot_sdk/common/libs')
ensure_dir('data_iot_sdk/common/include/qapi')
ensure_dir('data_iot_sdk/common/include/threadx_api')
ensure_dir('data_iot_sdk/common/include/src')
#Add new libs/paths here

copytree('include/qapi', 'data_iot_sdk/common/include/qapi', symlinks=False, ignore=None)
copytree('include/src', 'data_iot_sdk/common/include/src', symlinks=False, ignore=None)
copytree('include/threadx_api', 'data_iot_sdk/common/include/threadx_api', symlinks=False, ignore=None)
copytree('include/libs', 'data_iot_sdk/common/libs', symlinks=False, ignore=None)
copytree('location/sample_apps/location', 'data_iot_sdk/sample_apps/location', symlinks=False, ignore=None)
copytree('ril/psm/sample_apps/psm', 'data_iot_sdk/sample_apps/psm', symlinks=False, ignore=None)
os.chdir('build/ms')


