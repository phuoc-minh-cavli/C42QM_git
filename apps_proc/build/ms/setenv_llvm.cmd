@echo off
rem ==========================================================================
rem
rem  Set Environment Script for Windows
rem
rem  General Description
rem     Setup Qualcomm-specific environment variables for the build system.
rem
rem Copyright (c) 2009-2012 by QUALCOMM, Incorporated.
rem All Rights Reserved.
rem QUALCOMM Proprietary/GTDR
rem
rem --------------------------------------------------------------------------
rem
rem $Header: //components/rel/build.tn/4.0/ms/setenv_llvm.cmd#2 $
rem $DateTime: 2019/02/25 02:15:43 $
rem $Change: 18416151 $
rem
rem ==========================================================================

set PYTHON_PATH=C:\CRMApps\Apps\Python275-64
set PATH=C:\CRMApps\Apps\Python275-64;%PATH%
SET BUILD_VER=1234

REM Common
SET BUILD_ASIC=9205
SET MSM_ID=9205
SET HAL_PLATFORM=9205
SET TARGET_FAMILY=9205
SET CHIPSET=mdm9205

set LLVMTOOLS=LLVM
set LLVMROOT=C:\Apps\LLVM\4.0.3
set LLVMBIN=%LLVMROOT%\bin
set LLVMLIB=%LLVMROOT%\lib\clang\4.0.3\lib
set LLVMTOOLPATH=%LLVMROOT%\tools\bin
set MUSLPATH=%LLVMROOT%\tools
set MUSL32PATH=%LLVMROOT%\tools
set LLVMINC=%LLVMROOT%\aarch64-linux-gnu\libc\include
set LLVMINCLUDE=%LLVMROOT%\armv7m-none-eabi\libc\include
set LLVMINCLUDECXX=%LLVMROOT%\armv7m-none-eabi\include\c++\v1 
set LIBC=%LLVMROOT%\armv7m-none-eabi\libc\lib
set LLVMLIBRT=%LLVMROOT%
set GNULIBC=%LLVMROOT%\armv7m-none-eabi\libc    




set path=%LLVMBIN%;%PERLPATH%;%CRMPERL%;%PYTHON_PATH%;%GNUPATH%;%ARMCC5BIN%;%UTILROOT%;%path%

call %1.cmd
set  BUILDSPEC=KLOCWORK
rem Dump environment to stdout so that calling scripts can read it.
set
