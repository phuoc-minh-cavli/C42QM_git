/******************************************************************************
 * LOC DB IFACE . H
 * Interface providing DB functionality for use in Location modules to
 * persist data.
 *
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *******************************************************************************/
#ifndef __LOC_DB_IFACE_H__
#define __LOC_DB_IFACE_H__

#include <stdint.h>
#include <string>

using std::string;


/* Function: LocDBIface_addStrKeyValuePair()
 *
 * Adds the key value string pair to persistent storage.
 * dbFilePath identifies the file to use for storage.
 * tableName allows same key with different tableName
 *
 * If the key is already present, it's value gets overwritten.
 *
 * Returns 0 for success, -1 for failure.
 * */
int LocDBIface_addStrKeyValuePair(
        const string& dbFilePath,
        const string& key, const string& value);

/* Function: LocDBIface_getStrValue()
 *
 * Fetches the string value stored in DB file for specified key.
 * The value is updated in the passed in value string reference.
 *
 * Returns 0 for success, -1 for failure.
 * */
int LocDBIface_getStrValue(
        const string& dbFilePath,
        const string& key, string& value, bool bSingleLine = false);


void LocDBIface_set_max_efs_log_count(uint32_t maxLogCount);
void LocDBIface_log_efs(const string& message);

#endif //__LOC_DB_IFACE_H__
