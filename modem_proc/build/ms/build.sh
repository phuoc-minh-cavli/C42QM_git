#!/bin/sh
#===============================================================================
#
# Image Top-Level Build Script
#
# General Description
#    Image Top-Level Build Script
#
# Copyright (c) 2009-2012 by QUALCOMM, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
# $Header: //components/rel/build.mpss/2.2.8.1.c5.0.1/ms/build.sh#1 $
# $DateTime: 2018/10/05 13:57:21 $
# $Change: 17304820 $
#
#===============================================================================

cd `dirname $0`

# Call script to setup build environment, if it exists.
if [ -e setenv.sh ]; then
source ./setenv.sh
fi

# Call the main build command
python build_variant.py $*
build_result=$?
if [ "${build_result}" != "0" ] ; then
    exit ${build_result}
fi

