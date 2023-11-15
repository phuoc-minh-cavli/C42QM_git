#!/bin/bash

# Export enviroment variable

if [[ "$1" == "arm5" ]]
then
export ARM_COMPILER_PATH=/pkg/qct/software/ARM_Compiler_5/bin
export ARMROOT=/pkg/qct/software/ARM_Compiler_5
else
export ARM_COMPILER_PATH=/pkg/qct/software/ARM_Compiler_5.0.5/bin
export ARMROOT=/pkg/qct/software/ARM_Compiler_5.0.5
fi

# export ARMLMD_LICENSE_FILE=27000@18.202.207.45
export ARMTOOLS=ARMCT5
export ARMLIB=$ARMROOT/lib
export ARMINCLUDE=$ARMROOT/include
export ARMINC=$ARMINCLUDE
export ARMBIN=$ARMROOT/bin
export ARMHOME=$ARMROOT
export armlmd_license
export ARMPATH=$ARM_COMPILER_PATH
export PATH=$ARM_COMPILER_PATH:$PATH

# Set up compiler path
PYTHON_PATH=/usr/bin/python2.7
MAKE_PATH=/usr/bin/make
export LLVMTOOLS=LLVM
export LLVMVER=4.0.11
export LLVMROOT=/pkg/qct/software/llvm/release/arm/${LLVMVER}
export LLVMBIN=$LLVMROOT/bin
export LLVMLIB=$LLVMROOT/lib/clang/${LLVMVER}/lib/linux
export LLVMLIBPROP=$LLVMROOT/lib/clang/${LLVMVER}/lib/linux-propri_rt
export LLVMTOOLPATH=$LLVMROOT/tools/bin
export MUSLPATH=$LLVMROOT/aarch64-none-elf/libc
export MUSL32PATH=$LLVMROOT/armv7-none-eabi/libc
export LLVMINC=$MUSLPATH/include
export LLVM32INC=$MUSL32PATH/include
export LIBC=$MUSL32PATH
export LLVMINCLUDECXX=$LLVM32INC
export LLVMINCLUDE=$LLVM32INC
export LLVMTOOLPATH=/pkg/qct/software/llvm/release/arm/${LLVMVER}/tools/bin
export GNUROOT=/pkg/qct/software/gcc-linaro-aarch64-none-elf-4.9-2014.05_linux
export GNUARM7=/pkg/qct/software/gcc-linaro-arm-linux-gnueabihf-4.9-2014.05_linux
export GNULIBC="/pkg/qct/software/arm/linaro-toolchain/aarch64-none-elf/4.9-2014.07/aarch64-linux-gnu/libc"
export GNUTOOL="/pkg/qct/software/arm/linaro-toolchain/aarch64-none-elf/4.9-2014.07/aarch64-linux-gnu"
export HEXAGON_ROOT=/pkg/qct/software/HEXAGON_Tools/
export SCONS_OVERRIDE_NUM_JOBS=$(nproc)
export PATH=$GNUTOOL:$LLVMTOOLPATH:$PATH

export CAVLI_HW_VERSION="v0"

env
