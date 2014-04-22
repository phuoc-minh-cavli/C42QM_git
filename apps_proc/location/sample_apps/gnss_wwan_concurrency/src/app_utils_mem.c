/*****************************************************************************
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*****************************************************************************/
#include <qapi_fs.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "app_utils_misc.h"
#include "qapi_diag.h"

#define LOCATION_BYTE_POOL_SIZE (30720*8)

static UCHAR freeMemoryLocation[LOCATION_BYTE_POOL_SIZE];
static TX_BYTE_POOL *bytePoolLocation = NULL;
static boolean isInitialised = FALSE;

int32 app_utils_byte_create_pool() {

    int32 ret = -1;

    if(TRUE == isInitialised) {
        return 0;
    }

    do {
        if(0 != (ret = txm_module_object_allocate((void**)&bytePoolLocation, sizeof(TX_BYTE_POOL)))) {
            APP_LOGE("txm_module_object_allocate failed err %d", ret);
            break;
        }

        if(0 != (tx_byte_pool_create(bytePoolLocation, "Location application pool", freeMemoryLocation, LOCATION_BYTE_POOL_SIZE))) {
            APP_LOGE("tx_byte_pool_create failed err %d", ret);
            break;
        }
    } while(0);

    if(0 != ret) {
        if(bytePoolLocation) {
            txm_module_object_deallocate(bytePoolLocation);
        }
        APP_LOGE("Module initialization failed %d", ret);
    } else {
        isInitialised = TRUE;
    }

    return ret;
}

int32 app_utils_byte_allocate(void** memory_ptr, uint32 memory_size) {

    if(!isInitialised && 0 != app_utils_byte_create_pool()) {
        return -1;
    }
    int ret = tx_byte_allocate(
            bytePoolLocation, memory_ptr, memory_size, TX_NO_WAIT);
    return (TX_SUCCESS == ret)? 0: -1;
}

int32 app_utils_byte_release(void* memory_ptr) {

    if(NULL != memory_ptr) {
        return (TX_SUCCESS == tx_byte_release(memory_ptr))? 0: -1;
    } else {
        APP_LOGE("NULL memory pointer");
    }

    return -1;
}

TX_BYTE_POOL *app_utils_get_byte_pool() {

    int ret = 0;
    if(!isInitialised && (0 != (ret = app_utils_byte_create_pool()))) {
        APP_LOGH("Memory pool err %d", ret);
        return NULL;
    }
    return bytePoolLocation;
}
