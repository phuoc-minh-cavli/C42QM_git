#ifndef FLASH_FTL_UTIL_H
#define FLASH_FTL_UTIL_H

/**********************************************************************
 * flash_ftl_util.c
 *
 * Utility tools for Flash Translation layer (FTL).
 * This layer provides utilities tools for FTL core APIs
 *
 *
 * Copyright (c) 2016-2017, 2020 QUALCOMM Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *
 **********************************************************************/
/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *
 * when         who     what, where, why
 * ----------   ---     ------------------------------------------------
 * 2020-08-17   sk      Increasing simultaneous file access limit to 3.
 * 2017-03-10   svl     Increased malloc access limit for badblock backup.
 * 2016-12-02   svl     New malloc/free func to provide qurt region for flash
                        operations.
 * 2016-11-11   svl     Review comments incorporated.
 * 2016-09-14   svl     FTL utility layer support added.
 *=======================================================================*/

#include <flash_ftl_types.h>


#define SIZE_OF_1_KBYTE                             (1024)

#ifndef FLASH_FTL_ALIGNED_MALLOC_CALL_ACCESS_LIMIT
/** This macro defines the number of times
    flash_ftl_aligned_malloc can be called.
    The limit is set to 2 for now as it is required
    for :
    - flash_ftl_read_bytes() API
    - flash_ftl_core_bad_block_backup()
*/
#define FLASH_FTL_ALIGNED_MALLOC_CALL_ACCESS_LIMIT  (3)
#endif

/**
 * flash_ftl_aligned_malloc - allocate cpu aligned memory of size bytes and
 * returns a pointer to it.
 * NOTE :
 * This api can be called only "FLASH_FTL_ALIGNED_MALLOC_CALL_ACCESS_LIMIT"
 * number of times. If called more than above times then it will return NULL.
 * currently w.r.t to macro this API can only be called once.
 *
 * @param size [IN]
 *    number of bytes to be allocated
 *
 * @return FLASH_FTL_STATUS [OUT]
 *    NULL         - if size passed is 0/maximum attempts to call this api is reached
 *    FLASH_FTL_OK - if success
 */
void*
flash_ftl_aligned_malloc(uint32 size);

/**
  Frees memory allocated using flash_ftl_aligned_malloc
*/
void
flash_ftl_aligned_free(void *ptr);

/**
 * flash_ftl_malloc - allocate the size bytes and return a pointer to it
 * It can have additional checks for debugging
 *
 * @param size [IN]
 *    number of bytes to be allocated
 *
 * @return FLASH_FTL_STATUS [OUT]
 *    NULL         - if failed
 *    FLASH_FTL_OK - if success
 */
void*
flash_ftl_malloc(uint32 size);

/**
 * Frees memory allocated using flash_ftl_malloc and can have additional checks for
 * debugging
 *
 * @param ptr [IN]
 *    pointer to the memory to be freed
 *
 */
void
flash_ftl_free(void *ptr);

/**
 * flash_ftl_memscpy - Size bounded memory copy.
 * Copies the source string to the destination buffer.
 * This function ensures to copy only a maximum of destination buffer bytes.
 *
 * @param dst [IN]
 *    pointer to the dest of copy
 *
 * @param dest_size [IN]
 *    size of destination string
 *
 * @param src [IN]
 *    pointer to the source of copy
 *
 * @param src_size [IN]
 *    size of source string
 *
 * @return FLASH_FTL_STATUS [OUT]
 *    FLASH_FTL_INVALID_PARAM      - if args are invalid
 *    FLASH_FTL_FAIL               - if copy fails
 *    FLASH_FTL_OK                 - if success
 */
FLASH_FTL_STATUS
flash_ftl_memscpy(void *dst, uint32 dst_size, const void *src, uint32 src_size);

/**
 * flash_ftl_memset - initialize memory with given pattern.
 *
 * Copies the "value" to the ptr buffer.
 *
 * @param ptr [IN]
 *    pointer to the memory to be initialized
 *
 * @param value [IN]
 *    data to be set at ptr
 *
 * @param num [IN]
 *    number of bytes to set at ptr
 *
 * @return FLASH_FTL_STATUS [OUT]
 *    FLASH_FTL_INVALID_PARAM      - if ptr is NULL or if num is 0
 *    FLASH_FTL_OK                 - if success
 */
FLASH_FTL_STATUS
flash_ftl_memset( void * ptr, uint32 value, uint32 num );

/**
 * compares two strings and returns if they are equal
 *
 * @param str1 [IN]
 *    first string to be compared
 *
 * @param str2 [IN]
 *    second string to be compared
 *
 * @param cmp_result [OUT]
 *    value is < 0 if str1 is less than str2.
 *    value is > 0 if str2 is less than str1.
 *    value is = 0 if str1 is equal to str2.
 *
 * @return FLASH_FTL_STATUS [OUT]
 *    FLASH_FTL_INVALID_PARAM      - if str1/str2 are NULL
 *    FLASH_FTL_OK                 - if success
 */
FLASH_FTL_STATUS
flash_ftl_strcmp(const uint8 *str1, const uint8 *str2, uint32 *cmp_result);

/**
 * return length of the input string
 *
 * @param str [IN]
 *    input string whose length is to be calculated
 *
 * @return length of string [OUT]
 *    if str is valid string then length (positive value) of str is returned.
 *    else FLASH_FTL_INVALID_PARAM (negative value) is returned
 */
uint32
flash_ftl_strlen(const uint8 *str);

#endif /* #ifndef FLASH_FTL_UTIL_H */
