#===============================================================================
# script to get Nanopb files for external builds.
#
# GENERAL DESCRIPTION
#    Script to download and extract nanopb dependency for SSC builds.
#
# Copyright (c) 2019-2021 by Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
#
#===============================================================================

import os, sys, re
import shutil
import time
import platform
import subprocess
if sys.version_info.major == 2:
    import urllib
    pbfile = urllib.URLopener()
elif sys.version_info.major == 3:
    import urllib.request, urllib.parse, urllib.error
    pbfile = urllib.request.URLopener()

linux_nanopb_link = 'https://jpa.kapsi.fi/nanopb/download/nanopb-0.3.9.5-linux-x86.tar.gz'
windows_nanopb_link ='https://jpa.kapsi.fi/nanopb/download/nanopb-0.3.9.5-windows-x86.zip'
linux_nanopb_tar_file="nanopb-0.3.9.5-linux-x86.tar.gz"
windows_nanopb_zip_file="nanopb-0.3.9.5-windows-x86.zip"
linux_nanopb_src_dir=linux_nanopb_tar_file.strip('.tar.gz')
windows_nanopb_src_dir=windows_nanopb_zip_file.strip('.zip')

def get_nanopb(nanopb_dst, pk_root):
    calling_dir = os.getcwd()
    os.chdir(nanopb_dst)
    if platform.system() == 'Linux':
        if os.path.exists(os.path.join(nanopb_dst, "nanopb", "generator", "protoc")):
            print("nanopb source is present")
        else:
            pbfile.retrieve(linux_nanopb_link, linux_nanopb_tar_file)
            time.sleep(5)
            if not os.path.exists(linux_nanopb_tar_file):
                print("nanopb source is not downloaded please check the link")
            os.chdir(pk_root)
            p2 = subprocess.Popen("python ssc_api/build/config_nanopb_dependency.py -f %s" %linux_nanopb_src_dir, shell=True)
            p2.communicate()
            time.sleep(1)
            os.chdir(nanopb_dst)
            if os.path.exists(linux_nanopb_tar_file):
                os.remove(linux_nanopb_tar_file)

            if os.path.isdir(linux_nanopb_src_dir):
                shutil.rmtree(linux_nanopb_src_dir)
        os.chdir(calling_dir)
    else:
        if os.path.exists(os.path.join(nanopb_dst, "nanopb", "generator-win", "protoc.exe")):
            print("nanopb source is present")
        else:
            pbfile.retrieve(windows_nanopb_link, windows_nanopb_zip_file)
            time.sleep(5)
            if not os.path.exists(windows_nanopb_zip_file):
                print("nanopb source is not downloaded please check the link")
            os.chdir(pk_root)
            p2 = subprocess.Popen("python ssc_api\\build\\config_nanopb_dependency.py -f %s" %windows_nanopb_src_dir, shell=True)
            p2.communicate()
            time.sleep(1)
            os.chdir(nanopb_dst)
            if os.path.exists(windows_nanopb_zip_file):
                os.remove(windows_nanopb_zip_file)

            if os.path.isdir(windows_nanopb_src_dir):
                shutil.rmtree(windows_nanopb_src_dir)
        os.chdir(calling_dir)

