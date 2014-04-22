/*****************************************************************************
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*****************************************************************************/
#ifndef __APP_UTILS_FS_IFACE_H__
#define __APP_UTILS_FS_IFACE_H__

/* Read data from specified file and stores in the buffer provided.
 * Takes care of fopen/fread/fclose operations internally.
 *
 * numBytesToRead == 0 ==> read entire file
 *
 * Returns number of bytes read.
 * */
size_t app_FSIface_readFromFile(
                void* buffer,
                size_t bufferSize,
                size_t numBytesToRead,
                const char* filePathPtr,
                size_t filePathSize);

/* Fetch the size of fileName passed in.
 *
 * Returns the size of file.
 * -1 if file does not exist.
 * */
int app_FSIface_getFileSize(const char* filePathPtr);

#endif //__APP_UTILS_FS_IFACE_H__
