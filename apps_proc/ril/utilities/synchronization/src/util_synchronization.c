/***************************************************************************************************
    @file
    util_synchronization.c

    @brief
    Implements functions supported in util_synchronization.h.

    Copyright (c) 2013 Qualcomm Technologies, Inc.
    All Rights Reserved.
    Confidential and Proprietary - Qualcomm Technologies, Inc.
***************************************************************************************************/

#include "util_synchronization.h"

/***************************************************************************************************
    @function
    util_sync_data_init

    @implementation detail
    None.
***************************************************************************************************/
void util_sync_data_init(util_sync_data_type *sync_data, util_bit_field_type sync_data_bit_field)
{
    if(sync_data)
    {
        sync_data->sync_data_bit_field = sync_data_bit_field;
        ril_platform_thread_mutex_init(&sync_data->sync_mutex, NULL);
        sync_data->sync_data_bit_field = sync_data_bit_field;
        if(util_bit_field_is_bits_set(sync_data_bit_field,
                                      UTIL_SYNC_BIT_FIELD_COND_VAR_USED,
                                      FALSE))
        {
            ril_platform_thread_cond_init(&sync_data->sync_cond,
			                              NULL);
        }
    }
}

/***************************************************************************************************
    @function
    util_sync_data_destroy

    @implementation detail
    Sync object's bit field is reset.
***************************************************************************************************/
void util_sync_data_destroy(util_sync_data_type *sync_data)
{
    if(sync_data)
    {
        ril_platform_thread_mutex_destroy(&sync_data->sync_mutex);
        if(util_bit_field_is_bits_set(sync_data->sync_data_bit_field,
                                      UTIL_SYNC_BIT_FIELD_COND_VAR_USED,
                                      FALSE))
        {
            ril_platform_thread_cond_destroy(&sync_data->sync_cond);
        }
        sync_data->sync_data_bit_field = NIL;
    }
}

/***************************************************************************************************
    @function
    util_sync_data_acquire_mutex

    @implementation detail
    None.
***************************************************************************************************/
int util_sync_data_acquire_mutex(util_sync_data_type *sync_data)
{
    int ret;

    ret = ESUCCESS;

    if(sync_data)
    {
        ret = ril_platform_thread_mutex_lock(&sync_data->sync_mutex);
    }

    return ret;
}

/***************************************************************************************************
    @function
    util_sync_data_release_mutex

    @implementation detail
    None.
***************************************************************************************************/
int util_sync_data_release_mutex(util_sync_data_type *sync_data)
{
    int ret;

    ret = ESUCCESS;

    if(sync_data)
    {
        ret = ril_platform_thread_mutex_unlock(&sync_data->sync_mutex);
    }

    return ret;
}

/***************************************************************************************************
    @function
    util_sync_data_wait_on_cond

    @implementation detail
    get_time_of_day is used to provide the absolute time to timed wait.
***************************************************************************************************/
int util_sync_data_wait_on_cond(util_sync_data_type *sync_data,
                                int wait_for_time_seconds)
{
    int ret;

    ret = ESUCCESS;

    if(sync_data)
    {
        if(wait_for_time_seconds)
        {
            ret = ril_platform_thread_cond_wait_timed(&sync_data->sync_cond,
                                                      &sync_data->sync_mutex,
                                                      wait_for_time_seconds);
        }
        else
        {
            ret = ril_platform_thread_cond_wait(&sync_data->sync_cond,
                                                &sync_data->sync_mutex);
        }
    }
    return ret;
}

/***************************************************************************************************
    @function
    util_sync_data_signal_on_cond

    @implementation detail
    None.
***************************************************************************************************/
/*int util_sync_data_signal_on_cond(util_sync_data_type *sync_data)
{
    int ret;

    ret = ESUCCESS;

    if(sync_data)
    {
        ret = ril_platform_thread_cond_signal(&sync_data->sync_cond);
    }

    return ret;
}*/

