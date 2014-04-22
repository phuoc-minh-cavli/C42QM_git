/******************************************************************************
   Copyright (c) 2020 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
 *******************************************************************************/
#include "app_types.h"
#include "app_utils_misc.h"

// Pass in len as 0 for it to be auto computed
uint64_t app_utils_string_to_uint64(char* str, uint32 strLen)
{
    int i = 0;
    uint64_t value = 0;
    char digit = '\0';

    if(NULL == str){
        return 0;
    }
    if (strLen == 0) {
        strLen = strlen(str);
    }
    if(strLen > 19 || strLen <= 0){
        return 0;
    }

    // traverse all characters in string
    for(i=0; i < strLen; i++){
        digit = str[i];
        if(digit < 48 || digit > 57){
            return 0;
        }
        value = (value*10) + (digit-48);
    }

    return value;
}

double qapi_loc_util_string_to_double(char* str, uint32 strLen) {

    uint8_t digitValue = 0;
    double value = 0.0;
    char digit = '\0';
    int i = 0, negative = 0, dotSeen = 0;
    double fracMultiplier = 1;

    if (strLen == 0) {
        strLen = strlen(str);
    }

    // traverse all characters in string
    if(str[0] == '-'){
        i++;
        negative = 1;
        if(strLen == 1){
            return 0.0;
        }
    }
    for(; i < strLen; i++){
        digit = str[i];
        if(dotSeen == 0 && digit == '.'){
            dotSeen = 1;
            continue;
        }
        if(dotSeen == 1){
            fracMultiplier *= 0.1;
        }
        // ascii 48 = '0', 57 = '9'
        if(digit < 48 || digit > 57){
            return 0.0;
        }
        digitValue = digit - 48;
        value = value*10 + digitValue;
    }
    value = value * fracMultiplier;
    if(negative == 1)
        value = value * -1;

    return value;
}

// returns TRUE if string is unsigned int, or else FALSE
int app_utils_is_string_uint(char* str, uint32 strLen)
{
    uint32 i = 0;
    char digit = '\0';

    if (NULL == str)
    {
        return FALSE;
    }
    if (strLen == 0)
    {
        strLen = strlen(str);
    }
    if(strLen > 19 || strLen == 0)
    {
        return FALSE;
    }

    // traverse all characters in string
    for(i = 0; i < strLen; i++)
    {
        digit = str[i];
        if(digit < 48 || digit > 57){
            return FALSE;
        }
    }

    return TRUE;
}
