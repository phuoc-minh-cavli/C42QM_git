#ifndef DLFS_H
#define DLFS_H
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

#include <stdint.h>
#include <string.h>
#include "types.h"

#define DLFS_EOK 0x000 			/*Unused*/
#define DLFS_EFAILED 0x001		/*Unused*/
#define DLFS_EEOS 0x002			/*Unused*/

#ifdef __cplusplus
 extern "C" {
#endif

int dlfs_open(const char *name, unsigned int mode);

/** \details
 * Close a file
 *  
 * @param[in] fd
 *    Integer value of the handle 
 *
 * @return 0 if close is successful, -1 otherwise
 *
 */
int dlfs_close(int fd);

/** \details
 * Get the size of the file 
 *
 * @param[in] fd
 *    Integer value of the handle 
 *
 * @return Size of the file in bytes
 *
 */
int dlfs_flen(int fd);

/** \details
 * Get the size of the file 
 *     m,. jnm jh mnh\0-
 * @param[in] fd
 *    Integer value of the handle 
 *
 * @param[in] buffer
 *    Pointer to the stat data structure.  Only the total size
 *    field is populated
 *
 * @return 0 if close is successful, -1 otherwise
 *
 */
int dlfs_fstat(int fd, void *buffer);

struct dlfs_stat {
    off_t st_size;
    mode_t st_mode;
    int st_ino;
    dev_t st_dev;
    unsigned long st_ctime;
};

typedef int ino_t;

/** \details
 * Get the current file position indicator
 *
 * @param[in] fd
 *    Integer value of the handle 
 *
 * @return Current file position indicator
 *
 */
int dlfs_ftell(int fd);

/** \details
 * Read the content of the file described by file handle 
 *
 * @param[in] handle
 *    Integer value of the handle 
 * @param[out] *buffer
 *    Pointer to the buffer that the data will be written to
 * @param[in] size
 *    Number of bytes to read from the file
 *
 * @return number of bytes read, -1 otherwise
 *
 */
int dlfs_read(int fd, char *buffer, int count);

/** \details
 * Set the current file position indicator
 * Since the file is not really opened on the server side, calling this
 * function simply set a file position variable stored on the client side
 *
 * @param[in] handle
 *    Integer value of the handle 
 *
 * @param[in] offset
 *    The new file position, from the start of the file
 *
 * @return 0 if seek is successful, -1 otherwise
 *
 */
int dlfs_seek(int fd, int offset);

/** \details
 * Maps a file handle 
 *  
 * @param[in] addr 
 *    Starting address of address range to map.  This parameter
 *    is used as a hint if non-zero.
 *  
 * @param[int] length 
 *    Length of the address range to be mapped.
 *  
 * @param[in] prot 
 *    Describes the desired memory protection of the mapping.
 *    Only PROT_READ is supported.
 *  
 * @param[in] flags 
 *    Ignored, updates to the address range are not supported
 *  
 * @param[in] fd 
 *    File descriptor of the file to map.  dlfs_mmap() only
 *    supports mapping files so this parameter is mandatory
 *  
 * @param[in] offset 
 *    Offset into the file to start mapping.  Offset must be a
 *    multiple of the page size.
 *  
 * @return Pointer to the mapped area on success, -1 otherwise. 
 *  
 */
void* dlfs_mmap(void* addr, size_t length, int prot, int flags, int fd,
                off_t offset);

/** \details
 *  Unmaps memory
 *  
 *  @param[in] addr
 *     The beginning address of the address range to unmap.  The
 *     address addr must be a multiple of the page size.
 *  
 *  @param[in] length
 *     The length of the address range to unmap.
 *  
 *  @return 0 if successful, -1 otherwise
 *  
 */
int dlfs_munmap(void *addr, size_t length);

#ifdef __cplusplus
}
#endif

#endif // DLFS_H
