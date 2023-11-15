#!/bin/bash

DIR_CHIPCODE=$PWD
source $DIR_CHIPCODE/setenv.sh
source $DIR_CHIPCODE/shell_print.sh
PRODUCT_NAME=$(/bin/grep -oP '(?<=product_name>)[^<]+' $DIR_CHIPCODE/contents.xml)

TARGET_PRODUCT=C42QM_${PRODUCT_NAME}
# RELEASE_VERSION=v0.1
GIT_COMMIT=$(git rev-parse --short HEAD)
BUILD_DATE=$(date "+%Y/%m/%d-%H:%M:%S")
BUILD_ID=$(date "+%y%m%d_%H%M%S")

BIN_OUT=binaries

function do_build()
{
    LOG_INF "Get Flat build Images"
    rm -rf $DIR_CHIPCODE/${BIN_OUT}
    cd common/build/
    ./build.py
    cp $DIR_CHIPCODE/about.html $DIR_CHIPCODE/${BIN_OUT}/
    ls -lh $DIR_CHIPCODE/${BIN_OUT}
    cd $DIR_CHIPCODE/
}

function create_md5sum()
{
    cd $DIR_CHIPCODE/
    echo "Metabuild date: ${BUILD_DATE}" > $DIR_CHIPCODE/${BIN_OUT}/checksum.md5
    md5sum $DIR_CHIPCODE/${BIN_OUT}/* >> $DIR_CHIPCODE/${BIN_OUT}/checksum.md5
}

function compress_binaries()
{
    cd $DIR_CHIPCODE/
    if [[ -z ${RELEASE_VERSION} ]]
    then
        zip -r ${TARGET_PRODUCT}-${BUILD_ID}.zip ${BIN_OUT}/
    else
        zip -r ${TARGET_PRODUCT}-${RELEASE_VERSION}-${GIT_COMMIT}.zip ${BIN_OUT}/
    fi
}

function check_mbn()
{
    cp $DIR_CHIPCODE
    for file in $(ls binaries/ | grep .mbn)
    do
        echo $file 
        ./common/sectools/sectools.py secimage -i binaries/$file -p 9205 -a | tee -a binaries/secimage_output
    done
}

function get_meta()
{
    do_build
    create_md5sum
    check_mbn
    compress_binaries
}

get_meta | tee get_meta_${BUILD_ID}.log

