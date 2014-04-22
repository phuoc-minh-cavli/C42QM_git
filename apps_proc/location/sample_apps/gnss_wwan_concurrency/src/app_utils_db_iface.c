/*****************************************************************************
  Copyright (c) 2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*****************************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#include "app_utils_db_iface.h"
#include "app_utils_fs_iface.h"
#include "app_utils_misc.h"

/* Character used in db file to separate key and value*/
#define KEY_VALUE_SEPARATOR '='

/* Character used in db file to separate a key=value pair from another*/
#define KEY_VALUE_PAIR_SEPARATOR '\n'

/*Check for whitespace characters*/
#define IS_WHITESPACE(c) (c == ' ' || c == '\t' || c == '\r' || c == '\n')

/*Trim leading whitespace charecter*/
#define TRIM_WHITESPACES(ptr, maxPtr) \
    while(IS_WHITESPACE(*ptr) && ptr < maxPtr) { \
        ptr++; \
    }

/*Trim trailing whitespace charecter*/
#define TRIM_WHITESPACES_TRAILING(ptr, minPtr) \
    while(IS_WHITESPACE(*ptr) && ptr > minPtr) { \
        ptr--; \
    }

/* Return the pointer to first instance of key in the dbFileContent, or NULL if no key instance found*/
static char* _app_DBIface_searchKey(
        const char* dbFileContent, int dbFileSize, const char* key)
{
    char* keyStartPtr = (char*)dbFileContent;
    char* keyEndPtr = NULL;
    char* currentPtr = keyStartPtr;
    const char* endPtr = dbFileContent + dbFileSize;
    int keyLen = 0;

    while (keyStartPtr < endPtr) {

        TRIM_WHITESPACES(keyStartPtr, endPtr);

        /*look for key value separator*/
        currentPtr = strchr(keyStartPtr, KEY_VALUE_SEPARATOR);
        if (currentPtr == NULL) {
            APP_LOGM("Couldn't find KEY_VALUE_SEPARATOR");
            return NULL;
        }

        keyEndPtr = currentPtr - 1;
        TRIM_WHITESPACES_TRAILING(keyEndPtr, keyStartPtr);

        keyLen = keyEndPtr - keyStartPtr + 1;

        /*Now compare*/
        if (keyLen == strlen(key)
                && memcmp(key, keyStartPtr, keyLen) == 0) {
            return keyStartPtr;
        }

        /*Move to the next key*/
        currentPtr = strchr(keyStartPtr, KEY_VALUE_PAIR_SEPARATOR);
        if (currentPtr == NULL) {
            APP_LOGM("Couldn't find KEY_VALUE_PAIR_SEPARATOR, move to eof");
            currentPtr = (char*)(endPtr - 1);
        }
        keyStartPtr = currentPtr + 1;
    }

    APP_LOGM("Key not found !");
    return NULL;
}

/* Look for all integer keys in the file, and return the max */
static int app_DBIface_searchMaxIntKey(const char* dbFileContent, int dbFileSize)
{
    APP_LOGM("app_DBIface_searchMaxIntKey()");

    char* keyStartPtr = (char*)dbFileContent;
    char* keyEndPtr = NULL;
    char* currentPtr = keyStartPtr;
    const char* endPtr = dbFileContent + dbFileSize;
    int keyLen = 0;
    int keyValue = 0;
    int maxKeyValue = -1;

    while (keyStartPtr < endPtr)
    {
        TRIM_WHITESPACES(keyStartPtr, endPtr);

        /* look for key value separator*/
        currentPtr = strchr(keyStartPtr, KEY_VALUE_SEPARATOR);
        if (currentPtr == NULL) {
            APP_LOGM("Couldn't find KEY_VALUE_SEPARATOR");
            return -1;
        }

        keyEndPtr = currentPtr - 1;
        TRIM_WHITESPACES_TRAILING(keyEndPtr, keyStartPtr);

        keyLen = keyEndPtr - keyStartPtr + 1;

        /* Now compare */
        if (app_utils_is_string_uint(keyStartPtr, keyLen))
        {
            keyValue = (int)app_utils_string_to_uint64(keyStartPtr, keyLen);
            if (keyValue > maxKeyValue)
            {
                maxKeyValue = keyValue;
            }
        }

        /* Move to the next key */
        currentPtr = strchr(keyStartPtr, KEY_VALUE_PAIR_SEPARATOR);
        if (currentPtr == NULL) {
            APP_LOGM("Couldn't find KEY_VALUE_PAIR_SEPARATOR, move to eof");
            currentPtr = (char*)(endPtr - 1);
        }
        keyStartPtr = currentPtr + 1;
    }

    return maxKeyValue;
}

/* Look for all integer keys in the file, and return the max */
static int app_DBIface_searchIntKeyCount(const char* dbFileContent, int dbFileSize)
{
    APP_LOGM("app_DBIface_searchMaxIntKey()");

    char* keyStartPtr = (char*)dbFileContent;
    char* keyEndPtr = NULL;
    char* currentPtr = keyStartPtr;
    const char* endPtr = dbFileContent + dbFileSize;
    int keyLen = 0;
    int keyValue = 0;
    int maxKeyValue = -1;
    uint32_t intKeyCount = 0;

    while (keyStartPtr < endPtr)
    {
        TRIM_WHITESPACES(keyStartPtr, endPtr);

        /* look for key value separator*/
        currentPtr = strchr(keyStartPtr, KEY_VALUE_SEPARATOR);
        if (currentPtr == NULL) {
            APP_LOGM("Couldn't find KEY_VALUE_SEPARATOR");
            return -1;
        }

        keyEndPtr = currentPtr - 1;
        TRIM_WHITESPACES_TRAILING(keyEndPtr, keyStartPtr);

        keyLen = keyEndPtr - keyStartPtr + 1;

        /* Now compare */
        if (app_utils_is_string_uint(keyStartPtr, keyLen))
        {
            intKeyCount++;
        }

        /* Move to the next key */
        currentPtr = strchr(keyStartPtr, KEY_VALUE_PAIR_SEPARATOR);
        if (currentPtr == NULL) {
            APP_LOGM("Couldn't find KEY_VALUE_PAIR_SEPARATOR, move to eof");
            currentPtr = (char*)(endPtr - 1);
        }
        keyStartPtr = currentPtr + 1;
    }

    return intKeyCount;
}

static int _app_DBIface_searchKeyCount(const char* dbFileContent, int dbFileSize)
{
    APP_LOGM("app_DBIface_searchMaxIntKey()");

    char* keyStartPtr = (char*)dbFileContent;
    char* keyEndPtr = NULL;
    char* currentPtr = keyStartPtr;
    const char* endPtr = dbFileContent + dbFileSize;
    uint32_t keyCount = 0;

    while (keyStartPtr < endPtr)
    {
        TRIM_WHITESPACES(keyStartPtr, endPtr);

        /* look for key value separator*/
        currentPtr = strchr(keyStartPtr, KEY_VALUE_SEPARATOR);
        if (currentPtr == NULL) {
            APP_LOGM("Couldn't find KEY_VALUE_SEPARATOR");
            return -1;
        }

        keyEndPtr = currentPtr - 1;
        TRIM_WHITESPACES_TRAILING(keyEndPtr, keyStartPtr);

        /* Count the keys */
        keyCount++;

        /* Move to the next key */
        currentPtr = strchr(keyStartPtr, KEY_VALUE_PAIR_SEPARATOR);
        if (currentPtr == NULL) {
            APP_LOGM("Couldn't find KEY_VALUE_PAIR_SEPARATOR, move to eof");
            currentPtr = (char*)(endPtr - 1);
        }
        keyStartPtr = currentPtr + 1;
    }

    return keyCount;
}

// 0 if db opened successfully, -1 otherwise
static int app_DBIface_openDb(
        const char* dbFilePath, char** dbFileContent, uint32_t* dbFileSize)
{
    /*Check if file is empty or doesn't exist*/
    *dbFileSize = app_FSIface_getFileSize(dbFilePath);
    if(*dbFileSize <= 0){
        APP_LOGM("DB File empty or doesn't exist");
        return -1;
    }

    /*Read the entire db file to a char array*/
    app_utils_byte_allocate((void **)dbFileContent, (*dbFileSize) + 1);
    memset(*dbFileContent, 0, (*dbFileSize) + 1);

    int ret = app_FSIface_readFromFile(
                *dbFileContent,
                *dbFileSize,
                *dbFileSize,
                dbFilePath,
                strlen(dbFilePath));

    if(ret < *dbFileSize){
        APP_LOGM("Read only %d bytes from db file out of %d", ret, *dbFileSize);
        app_utils_byte_release((void *)*dbFileContent);
        return -1;
    }

    return 0;
}

/* Fetch for specified key, value would be updated. returns 0 is value found, -1 if not. */
// RETURNED VALUE MUST BE DEALLOCATED BY THE CALLER WITH A CALL TO app_utils_byte_release
char* app_DBIface_getStrValue(const char* dbFilePath, const char* key)
{
    char* dbFileContent = NULL;
    uint32_t dbFileSize = 0;
    char* value;

    /* key string should not have = or \n characters*/
    if(strchr(key, KEY_VALUE_SEPARATOR) != NULL ||
       strchr(key, KEY_VALUE_PAIR_SEPARATOR) != NULL){

        APP_LOGM("key can't have = or \\n char");
        return NULL;
    }

    if (app_DBIface_openDb(dbFilePath, &dbFileContent, &dbFileSize) != 0)
    {
        APP_LOGE("Failed to open db file");
        return NULL;
    }

    const char* endPtr = dbFileContent + dbFileSize;

    /*Find the key in dbFileContent*/
    char* keyPtr = _app_DBIface_searchKey(dbFileContent, dbFileSize, key);
    if(keyPtr == NULL){
        APP_LOGM("Key not found in db file");
        app_utils_byte_release((void *)dbFileContent);
        return NULL;
    }

    /*Key found, now look for value*/
    char* valueStartPtr = strchr(keyPtr, KEY_VALUE_SEPARATOR);
    if(valueStartPtr == NULL){
        APP_LOGM("Couldn't find KEY_VALUE_SEPARATOR");
        app_utils_byte_release((void *)dbFileContent);
        return NULL;
    }

    valueStartPtr++;
    TRIM_WHITESPACES(valueStartPtr, endPtr);

    /*Look for value end*/
    char* valueEndPtr = strchr(valueStartPtr, KEY_VALUE_PAIR_SEPARATOR);
    if(valueEndPtr == NULL){
        APP_LOGM("Couldn't find KEY_VALUE_PAIR_SEPARATOR, move to eof");
        valueEndPtr = (char*)endPtr;
    }

    valueEndPtr--;
    TRIM_WHITESPACES_TRAILING(valueEndPtr, valueStartPtr);

    int valueLen = valueEndPtr - valueStartPtr + 1;

    /* THIS MUST BE DEALLOCATED BY THE CALLER */
    app_utils_byte_allocate((void **)&value, valueLen+1);
    memcpy(value,valueStartPtr,valueLen);
    value[valueLen] = '\0';

    app_utils_byte_release((void *)dbFileContent);
    APP_LOGM("Key %s value %s len %d", key, value,valueLen);
    return value;
}

int app_DBIface_getMaxIntKey(const char* dbFilePath)
{
    char* dbFileContent = NULL;
    uint32_t dbFileSize = 0;
    int maxIntKey = 0;

    APP_LOGM("app_DBIface_getMaxIntKey()");

    if (app_DBIface_openDb(dbFilePath, &dbFileContent, &dbFileSize) != 0)
    {
        APP_LOGE("Failed to open db file");
        return -1;
    }

    /*Find the key in dbFileContent*/
    maxIntKey = app_DBIface_searchMaxIntKey(dbFileContent, dbFileSize);
    if(maxIntKey < 0){
        APP_LOGE("No Integer Key found in the db file.");
    }

    app_utils_byte_release((void *)dbFileContent);
    return maxIntKey;
}

/* Total number of integer keys in the db file */
uint32_t app_DBIface_getIntKeyCount(const char* dbFilePath)
{
    char* dbFileContent = NULL;
    uint32_t dbFileSize = 0;
    int intKeyCount = 0;

    APP_LOGM("app_DBIface_getIntKeyCount()");

    if (app_DBIface_openDb(dbFilePath, &dbFileContent, &dbFileSize) != 0)
    {
        APP_LOGE("Failed to open db file");
        return -1;
    }

    /*Find the key in dbFileContent*/
    intKeyCount = app_DBIface_searchIntKeyCount(dbFileContent, dbFileSize);
    if(intKeyCount < 0){
        APP_LOGE("Failed to parse the db file.");
    }

    app_utils_byte_release((void *)dbFileContent);
    return (uint32_t)intKeyCount;
}

/* Total number of keys in the db file. */
uint32_t app_DBIface_getKeyCount(const char* dbFilePath)
{
    char* dbFileContent = NULL;
    uint32_t dbFileSize = 0;
    int keyCount = 0;

    APP_LOGM("app_DBIface_getKeyCount()");

    if (app_DBIface_openDb(dbFilePath, &dbFileContent, &dbFileSize) != 0)
    {
        APP_LOGE("Failed to open db file");
        return -1;
    }

    /*Find the key in dbFileContent*/
    keyCount = _app_DBIface_searchKeyCount(dbFileContent, dbFileSize);
    if(keyCount < 0){
        APP_LOGE("Failed to parse the db file.");
    }

    app_utils_byte_release((void *)dbFileContent);
    return (uint32_t)keyCount;
}

