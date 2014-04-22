/******************************************************************************
 * LOC FS IFACE . H
 * Interface to the file system for use in Location code.
 *
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *******************************************************************************/
#ifndef __LOC_FS_IFACE_H__
#define __LOC_FS_IFACE_H__

#include <comdef.h>
#include <stdlib.h>

/* Function: LocFSIface_createFolderHierarchy
 *
 * specified folder hierarchy is created
 * modeValue can be passed in an octal format like 0660
 *
 * Returns 0 for success, and -1 for failure.
 * */
int LocFSIface_createFolderHierarchy(
        char* folderPathPtr, size_t folderPathSize,
        uint16 modeValue);

/* Function: LocFSIface_readFromFile()
 *
 * Reads data from specified file and stores in the buffer provided.
 * Takes care of fopen/fread/fclose operations internally.
 *
 * numBytesToRead == 0 ==> read entire file
 *
 * Returns number of bytes read.
 * */
size_t LocFSIface_readFromFile(
        void* buffer, size_t bufferSize,
        size_t numBytesToRead,
        char* filePathPtr, size_t filePathSize);

/* Function: LocFSIface_writeToFile
 *
 * Writes data to specified file, fetches from the buffer provided.
 * Takes care of fopen/fwrite/fclose operations internally.
 *
 * appendFlag : whether to overwrite or append ?
 * 0 = overwrite
 * 1 = append
 *
 * Returns number of bytes written.
 * */
size_t LocFSIface_writeToFile(
        void* buffer, size_t bufferSize,
        size_t numBytesToWrite,
        char* filePathPtr, size_t filePathSize,
        uint8 appendFlag);

/* Function: LocFSIface_deleteFile
 *
 * Deletes the specified file.
 *
 * Returns 0 for success, and -1 for failure.
 * */
int LocFSIface_deleteFile(
        char* filePathPtr, size_t filePathSize);

/* Function: LocFSIface_getFileSize
 *
 * Fetches the size of fileName passed in.
 *
 * Returns the size of file.
 * -1 if file does not exist.
 * */
int LocFSIface_getFileSize(
        char* filePathPtr, size_t filePathSize);

/* Function: LocFSIface_readFromRandomDevice
 *
 * Fetches random data from random generator HW on target
 *
 * Returns number of bytes read.
 * */
size_t LocFSIface_readFromRandomDevice(
        void* buffer, size_t bufferSize,
        size_t numBytesToRead);

/* Function: LocFSIface_chmodFile
 *
 * chmod operation on the specified file
 * modeValue can be passed in an octal format like 0660
 *
 * Returns 0 for success, and -1 for failure.
 * */
int LocFSIface_chmodFile(
        char* filePathPtr, size_t filePathSize,
        uint16 modeValue);

/* Function: LocFSIface_chmodFile
 *
 * chown operation on the specified file
 * Sets the specified userId and groupId.
 * To ignore either of userId or groupId pass in as -1.
 *
 * Returns 0 for success, and -1 for failure.
 * */
int LocFSIface_chownFile(
        char* filePathPtr, size_t filePathSize,
        int userId, int groupId);


#endif //__LOC_FS_IFACE_H__
