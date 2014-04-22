/***************************************************************************************************
    @file
    data_util_bit_field.h

    @brief
    Facilitates bit field operations by providing bit field related utilities.

    Copyright (c) 2013, 2021 Qualcomm Technologies, Inc.
    All Rights Reserved.
    Confidential and Proprietary - Qualcomm Technologies, Inc.
***************************************************************************************************/

#ifndef DATA_UTIL_BIT_FIELD
#define DATA_UTIL_BIT_FIELD

#include "data_utils_standard.h"

#define DATA_UTIL_BIT_FIELD_MAX (64)
typedef uint64_t data_util_bit_field_type;





/***************************************************************************************************
    @function
    data_util_bit_field_set_bits

    @brief
    Sets bits in a bit field.

    @param[in]
        bits_to_be_set
            bits to be set in the bit field

    @param[out]
        bit_field
            bit field to be used for setting bits

    @retval
    none
***************************************************************************************************/
void data_util_bit_field_set_bits(data_util_bit_field_type *bit_field,
                             data_util_bit_field_type bits_to_be_set);


/***************************************************************************************************
    @function
    data_util_bit_field_is_bits_set

    @brief
    Checks If specific set of bits are set in a bit field.

    @param[in]
        bit_field
            bit field to be used for checking If a specific set of bits are set
        bits_to_be_checked
            bits to be checked in the bit field
        is_partial_match_accepted
            TRUE If a subset of set bits is good enough for a successful match, FALSE otherwise

    @param[out]
        none

    @retval
    TRUE If the match has been successful, FALSE otherwise
***************************************************************************************************/
int data_util_bit_field_is_bits_set(data_util_bit_field_type bit_field,
                               data_util_bit_field_type bits_to_be_checked,
                               int is_partial_match_accepted);




#endif
