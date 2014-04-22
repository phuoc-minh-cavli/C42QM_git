/*****************************************************************************
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*****************************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#include "app_utils_fs_iface.h"
#include "app_utils_misc.h"
#include "qapi_fs.h"

size_t app_FSIface_readFromFile(
                        void* buffer,
                        size_t bufferSize,
                        size_t numBytesToRead,
                        const char* fileNamePtr,
                        size_t fileNameSize)
{
    int fd = -1;
    int bytesRead = 0;

    if(numBytesToRead > bufferSize){
        APP_LOGM("Insufficient buffer size !");
        return 0;
    }

    APP_LOGM("Open file with O_RDONLY (%s)", fileNamePtr);
    if (qapi_FS_Open(fileNamePtr, QAPI_FS_O_RDONLY_E, &fd) != QAPI_OK || fd < 0) {
        APP_LOGM("Failed to open file");
        return 0;
    }

     if(qapi_FS_Read(fd, buffer, numBytesToRead, &bytesRead) != QAPI_OK) {
        APP_LOGM("Failed to read file");
        return 0;
    }

    if(qapi_FS_Close(fd) != QAPI_OK){
        APP_LOGM("Failed closing file");
    }

    APP_LOGM("File %s Bytes read %d", fileNamePtr, bytesRead);
    return bytesRead;
}

int app_FSIface_getFileSize(const char* fileNamePtr)
{
    struct qapi_FS_Stat_Type_s SBuf = {0};

    if(qapi_FS_Stat(fileNamePtr, &SBuf) != QAPI_OK) {
        APP_LOGM("Error encountered while accesing file size");
        return -1;
    }

    APP_LOGM("file %s size %d", fileNamePtr, SBuf.st_size);
    return SBuf.st_size;
}
