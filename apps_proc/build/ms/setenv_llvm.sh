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
# $Header: //components/rel/build.tn/4.0/ms/setenv_llvm.sh#1 $
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

LLVM_COMPILER_PATH=/pkg/qct/software/llvm/release/arm/4.0.3
PYTHONPATH=/pkg/qct/software/python/2.7.9/bin
MAKE_PATH=/pkg/gnu/make/3.81/bin

export PATH=$MAKE_PATH:$PYTHON_PATH:$LLVMBIN:$PATH

export LLVMTOOLS=LLVM
export LLVMROOT=/pkg/qct/software/llvm/release/arm/4.0.3
export LLVMBIN=/pkg/qct/software/llvm/release/arm/4.0.3/bin
export LLVMLIB=/pkg/qct/software/llvm/release/arm/4.0.3/lib/clang/4.0.3/lib
export GNULIBC=/pkg/qct/software/llvm/release/arm/4.0.3/armv7m-none-eabi/libc  
export LLVMTOOLPATH=$LLVMROOT/tools/bin
export LLVMINCLUDE=/pkg/qct/software/llvm/release/arm/4.0.3/armv7m-none-eabi/libc/include
export LLVMINC=/pkg/qct/software/llvm/release/arm/4.0.3/aarch64-linux-gnu/libc/include
export LLVMINCLUDECXX=/pkg/qct/software/llvm/release/arm/4.0.3/armv7m-none-eabi/include/c++/v1 
export LIBC=/pkg/qct/software/llvm/release/arm/4.0.3/armv7m-none-eabi/libc/lib


# Hexagon Setup
export HEXAGON_ROOT=/pkg/qct/software/hexagon/releases/tools
fi

export BUILDSPEC=KLOCWORK
export TARGET_ROOT=../../..


