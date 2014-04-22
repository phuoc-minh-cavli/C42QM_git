# ==========================================================================
#
#  RPM build system launcher
#
# Copyright (c) 2012 by QUALCOMM, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
# ==========================================================================
# $Header: //components/rel/rpm.bf/2.1.3/build/build_9x05.sh#1 $

export BUILD_ASIC=9x05
export MSM_ID=9x05
export HAL_PLATFORM=9x05
export TARGET_FAMILY=9x05
export CHIPSET=mdm9x05
export SECPOLICY=SEC_POLICY_MULTIPLE_DEFAULT_SIGN
cd './rpm_proc/build/'

python ./build_common.py $@


