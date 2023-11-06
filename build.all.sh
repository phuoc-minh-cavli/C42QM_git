#!/bin/bash

DIR_CHIPCODE=$PWD

reset

source $DIR_CHIPCODE/shell_print.sh
source $DIR_CHIPCODE/setenv.sh

if [ "$1" == "-c" ]; then
    TASK_NAME="Clean"
    LOG_INF "Clean All ..."
else
    TASK_NAME="Build"
    LOG_INF "Build All ..."
    BUILD_ALL=1
fi

sleep 3

bash build.approc.sh $1 | tee build_approc.log &
APP_PROC=$!

bash build.boot.sh $1 | tee build_boot.log &
BOOT=$!

bash build.mpss.sh $1 | tee build_mpss.log &
MPSS=$!

# bash build.rpm.sh $1 | tee build_rpm.log &
# RPM=$!

# bash build.tz.sh $1 | tee build_tz.log &
# TZ=$!

LOG_V "APP_PROC = $APP_PROC"
LOG_V "BOOT = $BOOT"
LOG_V "MPSS = $MPSS"
# LOG_V "RPM = $RPM"
# LOG_V "TZ = $TZ"

wait $APP_PROC $BOOT $MPSS

if [ $? -eq 0 ]; then
    cd $DIR_CHIPCODE
    LOG_INF "$TASK_NAME all successfully"
    if [ $BUILD_ALL ]; then
        ./get_metabuid.sh
    fi
else
    LOG_INF "$TASK_NAME failed"
fi
