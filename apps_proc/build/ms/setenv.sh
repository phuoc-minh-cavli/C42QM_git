#===============================================================================
#
# Target Linux Build Script
#
# General Description
#    build shell script file.
#
# Copyright (c) 2009-2011 by QUALCOMM, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
# $Header: //components/rel/build.tn/4.0/ms/setenv.sh#1 $
# $DateTime: 2018/06/25 01:51:12 $
# $Author: pwbldsvc $
# $Change: 16491955 $
#                      EDIT HISTORY FOR FILE
#
# This section contains comments describing changes made to the module.
# Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     -----------------------------------------------------------
#
#===============================================================================

unamestr=`uname`
if [ "$unamestr" = "Linux" ]; then
   
   # set up local environment
   export_armlmd_license()
   {
     # know where the host is located
     mdb $(echo `hostname`) return site > __temp.out

     # in boulder?
     grep -q "site: boulder" __temp.out
     if [ $? -eq 0 ]
     then
       echo "in boulder"
       export ARMLMD_LICENSE_FILE=8224@redcloud:8224@swiftly:7117@license-wan-arm1
     else
       # in rtp?
       grep -q "site: rtp" __temp.out
       if [ $? -eq 0 ]
       then
         echo "in rtp"
         export ARMLMD_LICENSE_FILE=8224@license-wan-rtp1
       else
         # in hyderabad?
         grep -q "site: hyderabad" __temp.out
         if [ $? -eq 0 ]
         then
           echo "in hyderabad"
           export ARMLMD_LICENSE_FILE=7117@license-hyd1:7117@license-hyd2:7117@license-hyd3
         else
           # in sandiego and others
           echo "in sandiego"
           export ARMLMD_LICENSE_FILE=7117@license-wan-arm1
         fi
       fi
     fi

     rm -f __temp.out
   }

ARM_COMPILER_PATH=/pkg/qct/software/arm/RVDS/5.05bld106/bin
PYTHONPATH=/pkg/qct/software/python/2.7.9/bin
MAKE_PATH=/pkg/gnu/make/3.81/bin
#export ARMTOOLS=RVCT462
#export ARMROOT=/pkg/qct/software/arm/RVDS/4.1BLD713
#export ARMLIB=$ARMROOT/RVCT/Data/4.1/462/lib
#export ARMINCLUDE=$ARMROOT/RVCT/Data/4.1/462/include/unix
#export ARMINC=$ARMINCLUDE
#export ARMCONF=$ARMROOT/RVCT/Programs/4.1/462/linux-pentium
#export ARMDLL=$ARMROOT/RVCT/Programs/4.1/462/linux-pentium
#export ARMBIN=$ARMROOT/RVCT/Programs/4.1/462/linux-pentium
export PATH=$MAKE_PATH:$PYTHON_PATH:$ARM_COMPILER_PATH:$PATH
export ARMHOME=$ARMROOT
export_armlmd_license

export ARMROOT=/pkg/qct/software/arm/RVDS/5.05bld106
export ARMLIB=$ARMROOT/lib
export ARMINCLUDE=$ARMROOT/include
export ARMINC=$ARMINCLUDE
export ARMCONF=$ARMROOT/bin
export ARMDLL=$ARMROOT/bin
export ARMBIN=$ARMROOT/bin


# Hexagon Setup
export HEXAGON_ROOT=/pkg/qct/software/hexagon/releases/tools
fi

export BUILDSPEC=KLOCWORK
export TARGET_ROOT=../../..

