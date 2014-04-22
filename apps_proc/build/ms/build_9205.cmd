@echo off
rem ==========================================================================
rem
rem  CBSP Buils system
rem
rem  General Description
rem     build batch file.
rem
rem Copyright (c) 2009-2009 by QUALCOMM, Incorporated.
rem All Rights Reserved.
rem QUALCOMM Proprietary/GTDR
rem
rem --------------------------------------------------------------------------
rem
rem $Header: //components/rel/build.tn/4.0/ms/build_9205.cmd#2 $
rem $DateTime: 2019/06/17 09:40:13 $
rem $Author: pwbldsvc $
rem $Change: 19556983 $
rem                      EDIT HISTORY FOR FILE
rem
rem  This section contains comments describing changes made to the module.
rem  Notice that changes are listed in reverse chronological order.
rem
rem when       who     what, where, why
rem --------   ---     ---------------------------------------------------------
rem
rem ==========================================================================
@echo Start of Build.cmd
Call \\char\CRMTools\Smart\nt\bin\ARMCT505B106.cmd

REM if we have a set environment batch file, call it.
set PYTHON_PATH=C:\CRMapps\apps\Python275-64
set PATH=C:\CRMapps\apps\Python275-64;%PATH%
SET BUILD_VER=1234

REM Common
SET BUILD_ASIC=9205
SET MSM_ID=9205
SET HAL_PLATFORM=9205
SET TARGET_FAMILY=9205
SET CHIPSET=mdm9205

call %1.cmd
set  BUILDSPEC=KLOCWORK
rem Dump environment to stdout so that calling scripts can read it.
set


setlocal

set startdate=%date%
set starttime=%time%

SET BUILDSPEC=KLOCWORK
SET TARGET_ROOT=..\..\..
SET OKL4_SDK=..\..\core\kernel\sdk

REM Setup Paths
SET BUILD_ROOT=%~dp0..\..
SET TOOLS_SCONS_ROOT=%BUILD_ROOT%\tools\build\scons

SET BUILD_CMD=BUILD_ID=%BUILD_ID% BUILD_VER=%BUILD_VER% MSM_ID=%MSM_ID% HAL_PLATFORM=%HAL_PLATFORM% TARGET_FAMILY=%TARGET_FAMILY% BUILD_ASIC=%BUILD_ASIC% CHIPSET=%CHIPSET% %*
SET BUILD_ID=%BUILD_ID%

rem make sure that the successive build logs keep getting renamed
CALL %TOOLS_SCONS_ROOT%\build\rename-log.cmd

@echo Start Date=%startdate%, Time=%starttime% > build-log.txt
@echo #------------------------------------------------------------------------------- >> build-log.txt
@echo # ENVIRONMENT BEGIN >> build-log.txt
@echo #------------------------------------------------------------------------------- >> build-log.txt
set >> build-log.txt
@echo #------------------------------------------------------------------------------- >> build-log.txt
@echo # ENVIRONMENT END >> build-log.txt
@echo #------------------------------------------------------------------------------- >> build-log.txt

@echo #------------------------------------------------------------------------------- >> build-log.txt
@echo # BUILD BEGIN >> build-log.txt
@echo #------------------------------------------------------------------------------- >> build-log.txt

python %~dp0..\..\TOOLS\build\scons\build\log_output.py build-log.txt -- %~dp0..\..\tools\build\scons\SCons\scons.bat %*
rem Call the build.cmd and dump the STDERR and STDOUT into the build-log.txt
@echo %TOOLS_SCONS_ROOT%\SCons\scons -f target.scons %BUILD_CMD% 2>&1 | tee build-log.txt
call %TOOLS_SCONS_ROOT%\SCons\scons -f target.scons %BUILD_CMD% 2>&1 | tee build-log.txt
rem Dump the contents of the build-log into the console
type build-log.txt
rem Assign the value of SCONS_ERRORLEVEL to another local variable
@set BUILDERRORLEVEL=%SCONS_ERRORLEVEL%
@set SCONS_ERRORLEVEL1=%ERRORLEVEL%




@echo #------------------------------------------------------------------------------- >> build-log.txt
@echo # BUILD END >> build-log.txt
@echo #------------------------------------------------------------------------------- >> build-log.txt
set endtime=%time%
set enddate=%date%
@echo.
@echo Start Date=%startdate%, Time=%starttime% - End Date=%enddate%, Time=%endtime% >> build-log.txt
@echo Start Date=%startdate%, Time=%starttime% - End Date=%enddate%, Time=%endtime%
@echo Build returning %BUILDERRORLEVEL%
@echo Build returning %BUILDERRORLEVEL% >> build-log.txt
@exit /B %SCONS_ERRORLEVEL1%
