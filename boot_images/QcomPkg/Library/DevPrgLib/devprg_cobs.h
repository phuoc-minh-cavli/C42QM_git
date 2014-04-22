/**************************************************************************
 * FILE: devprg_cobs.h
 *
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Proprietary
 *
 *************************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header:
  $DateTime:
  $Author:

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2019-07-18   as    Create

===========================================================================*/

#ifndef __DEVPRG_COBS_H__
#define __DEVPRG_COBS_H__

#include "LoaderUtils.h" //needed for uint32

enum devprg_cobs_error_t {
    COBS_SUCCESS,
    COBS_ERROR_NULL_SRC_BUFFER,
    COBS_ERROR_NULL_DST_BUFFER,
    COBS_ERROR_INVALID_LENGTH,
    COBS_ERROR_INVALID_STUFFING,
    COBS_ERROR_DST_BUFFER_OVERFLOW
};

/******************************************************************************
* Name: devprg_cobs_get_error
*
* Description:
* This function returns the description of error
*
* Arguments:
*   error        [IN]: Error code

Returns:
* Returns the description of error
*
******************************************************************************/
const char* devprg_cobs_get_error(enum devprg_cobs_error_t error);

/******************************************************************************
* Name: devprg_cobs_stuff_bytes
*
* Description:
* This function perform consistent overhead byte stuffing on the source buffer
* and stores the result in destination buffer
*
* Arguments:
*   *src        [IN]: Pointer to source buffer
*   src_length  [IN]: Length of data in source buffer
*   *dst        [IN]: Pointer to destination buffer
*   dst_length  [IN]: Length of destination buffer
*   *error      [OUT]: Error code

Returns:
* Returns the length of data on the destination buffer provided the error code
* is SUCCESS.
*
******************************************************************************/
uint32 devprg_cobs_stuff_bytes(unsigned char *src, uint32 src_length,
    unsigned char *dst, uint32 dst_length, enum devprg_cobs_error_t *error);

/******************************************************************************
* Name: devprg_cobs_unstuff_bytes
*
* Description:
* This function performs unstuffing over consistent overhead byte stuffed data
* in the source buffer and stores the result in destination buffer
*
* Arguments:
*   *src    [IN]: Pointer to source buffer
*   src_length  [IN]: Length of data in source buffer
*   *dst        [IN]: Pointer to destination buffer
*   dst_length  [IN]: Length of destination buffer
*   *error  [OUT]: Error code

Returns:
* Returns the length of data on the destination buffer provided the error code
* is SUCCESS.
*
******************************************************************************/
uint32 devprg_cobs_unstuff_bytes(unsigned char *src, uint32 src_length,
    unsigned char *dst, uint32 dst_length, enum devprg_cobs_error_t *error);

#endif