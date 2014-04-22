#!/usr/bin/python
#===========================================================================

## NOTE: The syntax of this program is
##
##      python PrependHeaderForSahara.py efs1.mbn efs1.bin efs_ready_for_sahara.mbn
##
## i.e. combining efs1.mbn+efs1.bin to create the output file efs_ready_for_sahara.mbn

## NOTE: This can be done *without python* like so

## copy efs1.mbn+efs1.bin efs_ready_for_sahara.mbn
## efs1.mbn
## efs1.bin
##        1 file(s) copied.     


# REFERENCES

#  $Header: //components/rel/core.glue/1.3/tools/cmm/common/mdm9205/kickstart/PrependHeaderForSahara.py#1 $
#  $DateTime: 2019/01/17 22:25:40 $ 
#  $Author: pwbldsvc $

# when          who     what, where, why 
# --------      ---     ------------------------------------------------------- 
# 2012-05-24    ah      First release. Essentially does this copy file1+file2 file3

# Copyright (c) 2007-2012
# Qualcomm Technologies Incorporated.
# All Rights Reserved.
# Qualcomm Confidential and Proprietary
# ===========================================================================*/


import sys, shutil, os

HEADER  = ""
BODY    = ""
OUTPUT  = "combined.mbn"

if len(sys.argv) < 3:
    print "\nERROR: This program needs 2 arguments, which is the header and the raw data. It's purpose is "
    print "to prepend a header MBN file to raw file data as required for transport with the SAHARA protocol.\n"
    print "Ex 1: python PrependHeaderForSahara.py header.mbn raw.bin  <-- this will output combined.mbn\n"
    print "Ex 2: python PrependHeaderForSahara.py header.mbn raw.bin Myfile.mbn <-- this will output Myfile.mbn\n"
    sys.exit(1)
    
HEADER  = sys.argv[1]
BODY    = sys.argv[2]

if len(sys.argv) >= 4:
    OUTPUT  = sys.argv[3]

if not os.path.exists(HEADER):
    print "\nERROR: '%s' not found\n" % HEADER
    sys.exit()      # error

if not os.path.exists(BODY):
    print "\nERROR: '%s' not found\n" % BODY
    sys.exit()      # error

destination = open(OUTPUT,'wb')
shutil.copyfileobj(open(HEADER,'rb'), destination)
print "INFO: Added '%s'" % HEADER
shutil.copyfileobj(open(BODY,'rb'), destination)
print "INFO: Added '%s'" % BODY
destination.close() ## all of this same as cat file1 file2 > destination
print "INFO: Created '%s'" % OUTPUT
