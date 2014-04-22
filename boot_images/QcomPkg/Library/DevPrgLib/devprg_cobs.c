/**************************************************************************
 * FILE: devprg_cobs.c
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
#include "devprg_cobs.h"

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
const char* devprg_cobs_get_error(enum devprg_cobs_error_t error)
{
    switch (error)
    {
        case COBS_SUCCESS: 
            return "Success";
        case COBS_ERROR_NULL_SRC_BUFFER: 
            return "COBS_ERROR_NULL_SRC_BUFFER: Source buffer is null";
        case COBS_ERROR_NULL_DST_BUFFER: 
            return "COBS_ERROR_NULL_DST_BUFFER: Destination buffer is null";
        case COBS_ERROR_INVALID_LENGTH: 
            return "COBS_ERROR_INVALID_LENGTH: Invalid length of data in source buffer";
        case COBS_ERROR_INVALID_STUFFING: 
            return "COBS_ERROR_INVALID_STUFFING: Error while occured while unstuffing the data";
        case COBS_ERROR_DST_BUFFER_OVERFLOW: 
            return "COBS_ERROR_DST_BUFFER_OVERFLOW: Destination buffer overflow";
        default: 
            return "Unknown error";
    }
}

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
    unsigned char *dst, uint32 dst_length, enum devprg_cobs_error_t *error)
{
    uint32 stuffed_length = 1;
    unsigned char encoded_value = 0x01;
    unsigned char *encoded_value_ptr = dst;
    unsigned char *dest_value_ptr;
    uint32 index;
    *error = COBS_SUCCESS;
    if (src == NULL)
    {
        *error = COBS_ERROR_NULL_SRC_BUFFER;
        return 0;
    }
    if (dst == NULL)
    {
        *error = COBS_ERROR_NULL_DST_BUFFER;
        return 0;
    }
    if (src_length == 0)
    {
        *error = COBS_ERROR_INVALID_LENGTH;
        return 0;
    }
    if (dst_length == 0)
    {
        *error = COBS_ERROR_INVALID_LENGTH;
        return 0;
    }
    memset(dst, 0x0, dst_length);
    dest_value_ptr = dst + 1;
    index = 0;

    while (index < src_length)
    {
        if (encoded_value == 0xFF)
        {
            *encoded_value_ptr = encoded_value;
            encoded_value = 0x01;
            encoded_value_ptr = dest_value_ptr;
        }
        else
        {
            unsigned char data_value = *(src + index);
            if (data_value != 0x00)
            {
                *dest_value_ptr = data_value;
                encoded_value++;
            }else
            {
               *encoded_value_ptr = encoded_value;
                encoded_value = 0x01;
                encoded_value_ptr = dest_value_ptr;
            }
            index++;
        }
        
        dest_value_ptr++;
        stuffed_length++;

        if (index == src_length)
        {
            *encoded_value_ptr = encoded_value;
            *dest_value_ptr = 0x00;
            dest_value_ptr++;
            stuffed_length++;
        }
       
        if (stuffed_length >= dst_length && index < src_length)
        {
            *error = COBS_ERROR_DST_BUFFER_OVERFLOW;
            return 0;
        }
    }


    return stuffed_length;
}

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
    unsigned char *dst, uint32 dst_length, enum devprg_cobs_error_t *error)
{
    uint32 unstuffed_length = 0;
    unsigned char encoded_value = *src;
    unsigned char index = 1;
    *error = COBS_SUCCESS;
    if (src == NULL)
    {
        *error = COBS_ERROR_NULL_SRC_BUFFER;
        return 0;
    }
    if (dst == NULL)
    {
        *error = COBS_ERROR_NULL_DST_BUFFER;
        return 0;
    }
    if (src_length == 0)
    {
        *error = COBS_ERROR_INVALID_LENGTH;
        return 0;
    }
    if (dst_length == 0)
    {
        *error = COBS_ERROR_INVALID_LENGTH;
        return 0;
    }

    while (1)
    {
        if ((unstuffed_length >= dst_length) && (encoded_value != 0x00))
        {
            *error = COBS_ERROR_DST_BUFFER_OVERFLOW;
            return 0;
        }
        if (index != encoded_value)
        {
            unsigned char data_value = *(src + index);
            *dst = data_value;
            unstuffed_length++;
            dst++;
        }
        else
        {
            encoded_value = *(src + index);
            if (encoded_value == 0x00)
            {
                break;
            }
            if (index != 0xFF)
            {
                *dst = 0x00;
                dst++;
                unstuffed_length++;
            }
            src = src + index;
            index = 1;
            continue;
        }
        index++;

        if (index == 0)
        {
            *error = COBS_ERROR_INVALID_STUFFING;
            return 0;
        }
    }
    return unstuffed_length;
}