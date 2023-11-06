#!/bin/bash

DIR_CHIPCODE=$PWD
source $DIR_CHIPCODE/setenv.sh
source $DIR_CHIPCODE/shell_print.sh

TARGET_PRODUCT=C42QM
# RELEASE_VERSION=v0.1
GIT_COMMIT=$(git rev-parse --short HEAD)
BUILD_DATE=$(date "+%Y/%m/%d-%H:%M:%S")

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
        zip -r ${TARGET_PRODUCT}-${GIT_COMMIT}.zip ${BIN_OUT}/
    else
        zip -r ${TARGET_PRODUCT}-${RELEASE_VERSION}-${GIT_COMMIT}.zip ${BIN_OUT}/
    fi
}

function get_meta()
{
    do_build
    create_md5sum
    compress_binaries
}

get_meta

