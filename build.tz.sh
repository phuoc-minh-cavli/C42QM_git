#! /bin/bash
DIR_CHIPCODE=$PWD

source $DIR_CHIPCODE/shell_print.sh
source $DIR_CHIPCODE/setenv.sh

export SECTOOLS=$(pwd)/common/sectoolsv2/ext/Linux/sectools
export SCONS_OVERRIDE_NUM_JOBS=$(nproc)
export TARGET_PRODUCT=$(/bin/grep -oP '(?<=product_name>)[^<]+' $DIR_CHIPCODE/contents.xml)

cd trustzone_images/build/ms/
reset

if [ "$1" == "-c" ]; then
    TASK_NAME="Clean "
    LOG_INF "Clean TZ ..."
    sleep 3
    python build_all.py -b TZ.XF.5.1 CHIPSET=mdm9205 config=build_config_deploy.xml --clean
else
    TASK_NAME="Build "
    LOG_INF "Build TZ ..."
    sleep 3
    python build_all.py -b TZ.XF.5.1 CHIPSET=mdm9205 --config=build_config_deploy.xml --recompile
fi

if [ $? -eq 0 ]; then
    LOG_OK "$TASK_NAME TZ successfully"
    cd $DIR_CHIPCODE/
    exit 0
else
    LOG_ERR "$TASK_NAME TZ failed"
fi
cd $DIR_CHIPCODE/
exit 1

