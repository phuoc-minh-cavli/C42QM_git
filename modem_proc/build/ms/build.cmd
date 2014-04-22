@echo off
rem ==========================================================================
rem
rem  Image Top-Level Build Script
rem
rem  General Description
rem     build batch file.
rem
rem Copyright (c) 2009-2012 by QUALCOMM, Incorporated.
rem All Rights Reserved.
rem QUALCOMM Proprietary/GTDR
rem
rem --------------------------------------------------------------------------
rem
rem $Header: //components/rel/build.mpss/2.2.8.1.c5.0.1/ms/build.cmd#1 $
rem $DateTime: 2018/10/05 13:57:21 $
rem $Change: 17304820 $
rem
rem ==========================================================================

rem -- Call script to setup build environment, if it exists.
if exist setenv.cmd call setenv.cmd

rem -- Call the main build command
python build_variant.py %*
@exit /B %ERRORLEVEL%
