#!/bin/bash

DIR_CHIPCODE=$PWD
TARGET=9205.ltenbge.prod
source $DIR_CHIPCODE/shell_print.sh
source $DIR_CHIPCODE/setenv.sh
export TARGET_PRODUCT=$(/bin/grep -oP '(?<=product_name>)[^<]+' $DIR_CHIPCODE/contents.xml)

cd modem_proc/build/ms/
reset

if [ "$1" == "-c" ]; then
    TASK_NAME="Clean"
    LOG_INF "Clean MPSS ..."
    sleep 3
    ./build.sh 9205.ltenbge.prod -c
else
    TASK_NAME="Build"
    LOG_INF "Build MPSS ..."
    sleep 3
    ./build.sh 9205.ltenbge.prod -k
fi

if [ $? -eq 0 ]; then
    LOG_OK "$TASK_NAME MPSS successfully"
    cd $DIR_CHIPCODE/
    exit 0
else
    LOG_ERR "$TASK_NAME MPSS failed"
fi
cd $DIR_CHIPCODE/
exit 1
