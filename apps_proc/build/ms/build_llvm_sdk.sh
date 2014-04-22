#!/bin/bash
cd `dirname $0`

#===============================================================================
# Set target enviroment
#===============================================================================
BUILD_ID_CMD_LINE=$2
export BUILD_ID=${BUILD_ID_CMD_LINE:9:8}

export BUILD_ASIC=9205
export MSM_ID=9205
export HAL_PLATFORM=9205
export TARGET_FAMILY=9205
export CHIPSET=mdm9205

export BUILD_CMD="BUILD_ID=$BUILD_ID MSM_ID=$MSM_ID HAL_PLATFORM=$HAL_PLATFORM TARGET_FAMILY=$TARGET_FAMILY BUILD_ASIC=$BUILD_ASIC CHIPSET=$CHIPSET $*"
export OKL4_SDK=../../core/kernel/sdk

#
#===============================================================================
# Build Common
#===============================================================================

if [ -e setenv_llvm.sh ]; then
source setenv_llvm.sh
fi

#===============================================================================
# Call SCons
#===============================================================================

export BUILD_ROOT=../..
export SCONS_BUILD_ROOT=../..
export SCONS_TARGET_ROOT=$SCONS_BUILD_ROOT/..
export TOOLS_SCONS_ROOT=$SCONS_BUILD_ROOT/tools/build/scons


$TOOLS_SCONS_ROOT/build/rename-log.sh
$TOOLS_SCONS_ROOT/build/build.sh -f $TOOLS_SCONS_ROOT/build/start.scons $BUILD_CMD

build_result=$?
if [ "${build_result}" != "0" ] ; then
    exit ${build_result}
fi


build_result=$?
if [ "${build_result}" != "0" ] ; then
    exit ${build_result}
fi
python copyheaders.py $BUILD_ROOT
#------------------------------------------------------------------------------
# Generate BuildProducts.txt
#------------------------------------------------------------------------------
echo apps_proc/build/ms/M9205_llvm_dam.elf > ../../../BuildProducts.txt
