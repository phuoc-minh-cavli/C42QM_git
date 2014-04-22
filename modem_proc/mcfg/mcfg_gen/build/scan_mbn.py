#===============================================================================
#
# Scans mbns to determine whether files referenced by XML source are present
#
# GENERAL DESCRIPTION
#    aid to build script
#
# Copyright (c) 2012-2013 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/mcfg.mpss/10.1.1/mcfg_gen/build/scan_mbn.py#1 $
#  $DateTime: 2020/02/19 23:07:48 $
#  $Change: 22644654 $
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
#
#===============================================================================
import sys
import os
import getopt
import re
import xml.etree.ElementTree as ET

build_root = None
ifile = None

try:
  opts, args = getopt.getopt(sys.argv[1:], "b:i:", ["build_root", "input"])
except:
  print "scan_mbn.py -b <build_root> -i <input_xml> "
  sys.exit(2)

for opt, arg in opts:
  if opt in ('-b', "--build_root"):
    build_root = arg
  if opt in ('-i', "--input"):
    ifile = arg

# Build_root from SCons file included modem_proc, whereas
# relative paths from XMLs assume this is not the case
if os.path.isdir(build_root):
  build_root = re.sub(r'modem_proc.*', '', build_root, re.IGNORECASE)

try:
  with open (ifile) as file:
    pass
except:
  print "Cannot open input: '%s'" % ifile
  sys.exit(2)

ifile = os.path.normpath(ifile)
tree = ET.parse(ifile)
root = tree.getroot()
for child in root.findall('NvEfsFile'):
  fpath_rel = child.get('buildPath')
  fpath_abs = os.path.normpath(os.path.join(build_root, fpath_rel))
  try:
    with open(fpath_abs) as file:
      print "ref OK: %s" % fpath_abs
      pass
  except:
    print "Unable to locate and open file '%s'" % fpath_abs
    sys.exit(2)

sys.exit(0)
