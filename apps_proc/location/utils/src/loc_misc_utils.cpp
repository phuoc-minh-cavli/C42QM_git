/* Copyright (c) 2014, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation, nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include <comdef.h>
#include <stdio.h>
#include <stdlib.h>
#include <platform_lib_log_util.h>
#include <loc_misc_utils.h>
#include <ctype.h>
#include <string>
#include "stringl.h"
#include <qapi_status.h>

#define LOG_NDDEBUG 0
#define LOG_TAG "LocSvc_misc_utils"
// APIs implemented in C
#ifdef __cplusplus
extern "C" {
#endif

#include <fs_public.h>

#ifdef __cplusplus
}
#endif


using namespace std;

int loc_util_split_string(char *raw_string, char **split_strings_ptr,
                          int max_num_substrings, char delimiter)
{
    int raw_string_index=0;
    int num_split_strings=0;
    unsigned char end_string=0;
    int raw_string_length=0;

    if(!raw_string || !split_strings_ptr) {
        LOC_LOGSE2("%s:%d]: NULL parameters", __func__, __LINE__);
        num_split_strings = -1;
        goto err;
    }
    LOC_LOGSL3("%s:%d]: raw string: %s\n", __func__, __LINE__, raw_string);
    raw_string_length = strlen(raw_string) + 1;
    split_strings_ptr[num_split_strings] = &raw_string[raw_string_index];
    for(raw_string_index=0; raw_string_index < raw_string_length; raw_string_index++) {
        if(raw_string[raw_string_index] == '\0')
            end_string=1;
        if((raw_string[raw_string_index] == delimiter) || end_string) {
            raw_string[raw_string_index] = '\0';
            LOC_LOGSL3("%s:%d]: split string: %s\n",
                     __func__, __LINE__, split_strings_ptr[num_split_strings]);
            num_split_strings++;
            if(((raw_string_index + 1) < raw_string_length) &&
               (num_split_strings < max_num_substrings)) {
                split_strings_ptr[num_split_strings] = &raw_string[raw_string_index+1];
            }
            else {
                break;
            }
        }
        if(end_string)
            break;
    }
err:
    LOC_LOGSL3("%s:%d]: num_split_strings: %d\n", __func__, __LINE__, num_split_strings);
    return num_split_strings;
}

bool loc_isspace(int c)
{
    switch (c)
    {
        case 0x20:
        case 0x09:
        case 0x0a:
        case 0x0b:
        case 0x0c:
        case 0x0d:
            return true;
        default:
            return false;
    }
}

bool loc_isnumeric(int c){

    return (c >= 48 && c <= 57);
}

void loc_util_trim_space(char *org_string)
{
    char *scan_ptr, *write_ptr;
    char *first_nonspace = NULL, *last_nonspace = NULL;

    if(org_string == NULL) {
        LOC_LOGSE2("%s:%d]: NULL parameter", __func__, __LINE__);
        goto err;
    }

    scan_ptr = write_ptr = org_string;
    while (*scan_ptr) {
        //Find the first non-space character
        if ( !loc_isspace(*scan_ptr) && first_nonspace == NULL) {
            first_nonspace = scan_ptr;
        }
        //Once the first non-space character is found in the
        //above check, keep shifting the characters to the left
        //to replace the spaces
        if (first_nonspace != NULL) {
            *(write_ptr++) = *scan_ptr;
            //Keep track of which was the last non-space character
            //encountered
            //last_nonspace will not be updated in the case where
            //the string ends with spaces
            if ( !loc_isspace(*scan_ptr)) {
                last_nonspace = write_ptr;
            }
        }
        scan_ptr++;
    }
    //Add NULL terminator after the last non-space character
    if (last_nonspace) { *last_nonspace = '\0'; }
err:
    return;
}

string loc_util_str_append(string str, string appendStr){

    int bufSize = str.size() + appendStr.size() + 1;
    char* buf = new char[bufSize];
    memset(buf, 0, bufSize);
    memscpy(buf, bufSize, str.c_str(), str.size());
    memscpy(buf+str.size(), bufSize, appendStr.c_str(), appendStr.size());
    string newStr(buf);
    delete[] buf;
    return newStr;
}

int32 loc_util_atoi(char* str, int bufferLen){

    if(str == NULL){
        LOC_LOGE0("loc_util_atoi: NULL str");
        return 0;
    }

    int idx = 0;
    // skip whitespace characters
    while(loc_isspace(str[idx]) && idx < bufferLen)
        idx++;

    if(idx == bufferLen){
        LOC_LOGE0("loc_util_atoi: all whitespaces");
        return 0;
    }

    bool isNegative = false;
    if(str[idx] == '-'){
        isNegative = true;
        idx++;
    }

    // skip whitespace characters
    while(loc_isspace(str[idx]) && idx < bufferLen)
        idx++;

    if(idx == bufferLen){
        LOC_LOGE0("loc_util_atoi: all whitespaces");
        return 0;
    }

    // scan through digits
    int32 value = 0;
    int numDigits = 0;
    while(loc_isnumeric(str[idx]) && idx < bufferLen){
        value = (value*10) + (str[idx]-48);
        idx++; numDigits++;
        if(numDigits > 9){
            LOC_LOGE0("loc_util_atoi: number too large");
            return 0;
        }
    }

    if(isNegative)
        value *= -1;

    return value;
}


string loc_util_uint64_to_string(uint64 value){

    if(value == 0){
        return "0";
    }

    // create temp char array to hold the string as we create
    // uint64 value in decimal can have 19 digits
    char numStr[20];
    memset(numStr, 0, sizeof(numStr));
    int idx = 18;
    int digit = 0;
    while(value > 0 && idx >= 0){
        digit = value%10;
        value = value/10;
        numStr[idx--] = digit + 48;
    }
    if(value > 0){
        LOC_LOGE0("Value too long, invalid");
        return "";
    }
    string str(numStr+idx+1);
    return str;
}

uint64 loc_util_string_to_uint64(string& str){

    if(str.empty()){
        LOC_LOGE0("Invalid empty string");
        return 0;
    }
    if(str.size() > 19){
        LOC_LOGE0("string to uint64: number too large");
        return 0;
    }

    // traverse all characters in string
    uint64 value = 0;
    char digit = '\0';
    for(int i=0; i < str.size(); i++){
        digit = str.at(i);
        if(digit < 48 || digit > 57){
            LOC_LOGE1("Invalid char in numeric str [%c]", digit);
            return 0;
        }
        value = (value*10) + (digit-48);
    }

    LOC_LOGSL3("loc_util_string_to_uint64 [%s] = 0x%x%x",
            str.c_str(), value>>32, value);
    return value;
}

#define MAX_FRAC_DIGITS 10000000
double loc_util_string_to_double(char* str){

    // Max len is 19
    int strLen = strnlen(str, 19);
    LOC_LOGL1("strLen: %d", strLen);

    if(strLen <= 0){
        LOC_LOGE0("Invalid empty string");
        return 0.0;
    }

    double value = 0.0;

    // traverse all characters in string
    char digit = '\0';
    int i = 0, negative = 0, dotSeen = 0;
    double fracMultiplier = 1;
    if(str[0] == '-'){
    	i++;
    	negative = 1;
    	if(strLen == 1){
    		LOC_LOGE0("Invalid number !");
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
            LOC_LOGE1("Invalid char in numeric str [%c]", digit);
            return 0.0;
        }
        uint8 digitValue = digit - 48;
        value = value*10 + digitValue;
    }
    value = value * fracMultiplier;
    if(negative == 1)
        value = value * -1;

    LOC_LOGSM3("loc_util_string_to_double: [%s] = %d.%d",
            str, (int)value, ((int)(value*MAX_FRAC_DIGITS))%MAX_FRAC_DIGITS);
    return value;
}
string loc_util_double_to_string(double num){

	int intPart = (int)num;
    double fracPart = num - intPart;
    if(fracPart < 0)
        fracPart *= -1;
    fracPart *= MAX_FRAC_DIGITS;
    int fracPartInt = (int)fracPart;

    LOC_LOGL3("loc_util_double_to_string num %d, int %d frac %d",
    		num, intPart, fracPart);

    return intPart + "." + fracPartInt;
}

void loc_util_log_location(Location& location){

    LOC_LOGM8("LAT: %d.%d LON: %d.%d ALT: %d.%d ACC: %d.%d",
            (int)location.latitude, ((int)(location.latitude*100000))%100000,
            (int)location.longitude, ((int)(location.longitude*100000))%100000,
            (int)location.altitude, ((int)(location.altitude*100))%100,
            (int)location.accuracy, ((int)(location.accuracy*100))%100);

    LOC_LOGM7("SPEED: %d.%d BEAR: %d.%d TIME: 0x%x%x FLAGS: %u",
            (int)location.speed, ((int)(location.speed*100))%100,
            (int)location.bearing, ((int)(location.bearing*100))%100,
            location.timestamp>>32, location.timestamp, location.flags);
}

int loc_util_ssl_store_read_from_EFS_file(
        const char * name,
        void ** buffer_ptr,
        size_t * buffer_size
        )
{
    int bytes_read;
    struct fs_stat stat;
    uint8 *file_buf = NULL;

    if ( (!name) ||(!buffer_ptr) ||(!buffer_size) )
    {
        LOC_LOGE0("SSL_cert_store: ssl_store_read_buffer_from_securefs_file failed!");
        return QAPI_ERROR;
    }

    if (efs_stat(name, &stat) < 0) {
        LOC_LOGE0("SSL_cert_store: Reading EFS file failed");
        return QAPI_ERROR;
    }

    if ((file_buf = malloc(stat.st_size)) == NULL) {
        LOC_LOGE0("SSL_cert_store: QAPI_ERR_NO_MEMORY ");
        return QAPI_ERR_NO_MEMORY;
    }

    bytes_read = efs_get(name, file_buf, stat.st_size);
    if ((bytes_read < 0) || (bytes_read != stat.st_size)) {
        free(file_buf);
        LOC_LOGE0("SSL_cert_store: Reading EFS file error ");
        return QAPI_ERROR;
    }

    *buffer_ptr = file_buf;
    *buffer_size = stat.st_size;

    return QAPI_OK;
}
