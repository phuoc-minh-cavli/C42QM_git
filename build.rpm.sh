#! /bin/bash
DIR_CHIPCODE=$PWD

source $DIR_CHIPCODE/shell_print.sh
source $DIR_CHIPCODE/setenv.sh arm5

export TARGET_PRODUCT=$(/bin/grep -oP '(?<=product_name>)[^<]+' $DIR_CHIPCODE/contents.xml)
reset

if [ "$1" == "-c" ]; then
    TASK_NAME="Clean"
    LOG_INF "Clean RPM ..."
    sleep 3
    cd rpm_proc/build
    ./build_9x05.sh -c
else
    TASK_NAME="Build"
    LOG_INF "Build RPM ..."
    sleep 3
    cd rpm_proc/build
    ./build_9x05.sh
fi

if [ $? -eq 0 ]; then
    LOG_OK "$TASK_NAME RPM successfully"
    cd $DIR_CHIPCODE/
    exit 0
else
    LOG_ERR "$TASK_NAME RPM failed"
fi

cd $DIR_CHIPCODE/
exit 1
