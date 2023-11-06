#! /bin/bash
DIR_CHIPCODE=$PWD

source $DIR_CHIPCODE/shell_print.sh
source $DIR_CHIPCODE/setenv.sh
export TARGET_PRODUCT=$(/bin/grep -oP '(?<=product_name>)[^<]+' $DIR_CHIPCODE/contents.xml)

clear

cd apps_proc/build/ms

TARGET_BUILD_ID=ACFNAABZ
TARGET_LLVM_DAM=ACFNAADZ

if [ "$1" == "-c" ]; then
    TASK_NAME="Clean"
    LOG_INF "Clean app_proc ..."
    sleep 3
    ./build.sh all BUILD_ID=${TARGET_BUILD_ID} -c
else
    TASK_NAME="Build"
    LOG_INF "Build app_proc ..."
    sleep 3
    ./build.sh all BUILD_ID=${TARGET_BUILD_ID}
    ./build_llvm.sh llvm_dam BUILD_ID=${TARGET_LLVM_DAM}
fi

if [ $? -eq 0 ]; then
    LOG_OK "$TASK_NAME APP_PROC successfully"
    cd $DIR_CHIPCODE/
    exit 0
else
    LOG_ERR "$TASK_NAME APP_PROC failed"
fi
cd $DIR_CHIPCODE/
exit 1
