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
rem $Header: //components/rel/build.tn/4.0/ms/setenv.cmd#1 $
rem $DateTime: 2018/06/25 01:51:12 $
rem $Change: 16491955 $
rem
rem ==========================================================================
set PYTHON_PATH=C:\CRMapps\apps\Python275-64
set PATH=C:\CRMapps\apps\Python275-64;%PATH%
SET BUILD_VER=1234

REM Common
SET BUILD_ASIC=9x06
SET MSM_ID=9x06
SET HAL_PLATFORM=9x06
SET TARGET_FAMILY=9x06
SET CHIPSET=mdm9x06

call %1.cmd
set  BUILDSPEC=KLOCWORK
rem Dump environment to stdout so that calling scripts can read it.
set
