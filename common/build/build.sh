#!/bin/sh
cd `dirname $0`
python build_main.py $*
build_result=$?
if [ "${build_result}" != "0" ] ; then
    exit ${build_result}
fi
