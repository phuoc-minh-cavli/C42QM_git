/*****************************************************************************
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*****************************************************************************/
#ifndef __APP_UTILS_DB_IFACE_H__
#define __APP_UTILS_DB_IFACE_H__

#include <stdint.h>

/* Fetch the string value stored in DB file for specified key.
 * The value is updated in the passed in value string reference.
 *
 * Returns 0 for success, -1 for failure.
 * */
char* app_DBIface_getStrValue(const char* dbFilePath, const char* key);

/* Find out the largest integer key present in a file
 * Can be used to infer the number of different steps in an exec file
 * like in local controller code.
 *
 * Returns -1 if no integer key found in the file.
 * */
int app_DBIface_getMaxIntKey(const char* dbFilePath);

/* Total number of integer keys in the db file */
uint32_t app_DBIface_getIntKeyCount(const char* dbFilePath);

/* Total number of keys in the db file. */
uint32_t app_DBIface_getKeyCount(const char* dbFilePath);

#endif //__APP_UTILS_DB_IFACE_H__
