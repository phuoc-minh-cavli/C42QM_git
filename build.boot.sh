#! /bin/bash
DIR_CHIPCODE=$PWD

source $DIR_CHIPCODE/shell_print.sh
source $DIR_CHIPCODE/setenv.sh
export TARGET_PRODUCT=$(/bin/grep -oP '(?<=product_name>)[^<]+' $DIR_CHIPCODE/contents.xml)

reset
# cd boot_images/build/ms/

if [ "$1" == "-c" ]; then
    TASK_NAME="Clean"
    LOG_INF "Clean boot_image ..."
    sleep 3
    python -u boot_images/QcomPkg/buildit.py --variant LA -r RELEASE -t Mdm9x05Pkg,QcomToolsPkg --build_flags=cleanall
else
    TASK_NAME="Build"
    LOG_INF "Build boot_image ..."
    sleep 3
    python -u boot_images/QcomPkg/buildit.py --variant LA -r RELEASE -t Mdm9x05Pkg,QcomToolsPkg
fi

if [ $? -eq 0 ]; then
    LOG_OK "$TASK_NAME BOOT successfully"
    cd $DIR_CHIPCODE/
    exit 0
else
    LOG_ERR "$TASK_NAME BOOT failed"
fi
cd $DIR_CHIPCODE/
exit 1

