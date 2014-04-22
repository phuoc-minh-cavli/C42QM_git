/***************************************************************************************************
    @file
    data_util_bit_field.c

    @brief
    Implements functions supported in data_util_bit_field.h.

    Copyright (c) 2013 Qualcomm Technologies, Inc.
    All Rights Reserved.
    Confidential and Proprietary - Qualcomm Technologies, Inc.
***************************************************************************************************/

#include "data_util_bit_field.h"
#include "data_utils_common.h"






/***************************************************************************************************
    @function
    data_util_bit_field_set_bits

    @implementation detail
    None.
***************************************************************************************************/
void data_util_bit_field_set_bits(data_util_bit_field_type *bit_field,
                             data_util_bit_field_type bits_to_be_set)
{
    if(bit_field)
    {
        *bit_field |= bits_to_be_set;
    }
}


/***************************************************************************************************
    @function
    data_util_bit_field_is_bits_set

    @implementation detail
    Uses XOR operation.
***************************************************************************************************/
int data_util_bit_field_is_bits_set(data_util_bit_field_type bit_field,
                               data_util_bit_field_type bits_to_be_checked,
                               int is_partial_match_accepted)
{
    int is_set;
    data_util_bit_field_type temp_bit_field;

    is_set = FALSE;
    temp_bit_field = NIL;

    temp_bit_field = bit_field & bits_to_be_checked;
    if(is_partial_match_accepted)
    {
        is_set = temp_bit_field ? TRUE : FALSE;
    }
    else
    {
        is_set = (temp_bit_field ^ bits_to_be_checked) ? FALSE : TRUE;
    }

    return is_set;
}




