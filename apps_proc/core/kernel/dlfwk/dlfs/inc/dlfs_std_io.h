/*==============================================================================

Dynamic Loading Support Libs

Copyright (c) 2014 by QUALCOMM Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR

--------------------------------------------------------------------------------
 EDIT HISTORY FOR FILE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

 when               who         what, where, why
 --------           ---         -----------------------------------------------
 1/22/15          amitkulk      created 
==============================================================================*/
#ifndef DLFS_STD_IO_H
#define DLFS_STD_IO_H

#include <string.h>
#include <sys/types.h>

typedef enum
{
   DLFS_STD_IO_SEEK_SET = 0 ,      /* Seek from beginning of file.  */
   DLFS_STD_IO_SEEK_CUR = 1 ,      /* Seek from current position.  */
   DLFS_STD_IO_SEEK_END = 2 ,      /* Seek from end of file.  */
}dlfs_std_IO_SEEK;

typedef enum
{
   DLFS_STD_IO_EFS = 0 ,      /*  EFS Service for IO  */
   DLFS_STD_IO_RFS = 1 ,     /*  RFSA Service for IO */
}dlfs_std_IO;

typedef struct dlfs_std_FILE
{
  int fd;
  int filesystem;
}dlfs_std_FILE;

/** \details
 * Definition for file open mode
 */

#define O_RDONLY    0x0     /* open for read only */
#define O_WRONLY    0x1     /* open for write only */
#define O_RDWR      0x2     /* open for read/write */

#ifdef __cplusplus
 extern "C" {
#endif

int _fopen(const char *filename, unsigned int  fileMode, dlfs_std_FILE* psout);

int _flen(dlfs_std_FILE *psin);

int _fread(dlfs_std_FILE *psin, char *buf, int bufLen);

int _fseek(dlfs_std_FILE *psin, int offset, dlfs_std_IO_SEEK whence);

int _ftell(dlfs_std_FILE *psin);

int _fclose(dlfs_std_FILE *psin);

#ifdef __cplusplus
}
#endif

#endif // DLFS_H
