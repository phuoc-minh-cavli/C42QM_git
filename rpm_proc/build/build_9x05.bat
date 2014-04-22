@echo off
rem ==========================================================================
rem
rem  RPM build system launcher
rem
rem Copyright (c) 2012 by QUALCOMM, Incorporated.
rem All Rights Reserved.
rem QUALCOMM Proprietary/GTDR
rem ==========================================================================
rem $Header: //components/rel/rpm.bf/2.1.3/build/build_9x05.bat#1 $

SET BUILD_ASIC=9x05
SET MSM_ID=9x05
SET HAL_PLATFORM=9x05
SET TARGET_FAMILY=9x05
SET CHIPSET=mdm9x05
SET SECPOLICY=SEC_POLICY_MULTIPLE_DEFAULT_SIGN

python build_common.py %*


