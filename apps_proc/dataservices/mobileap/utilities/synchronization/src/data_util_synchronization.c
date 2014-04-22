/***************************************************************************************************
    @file
    data_util_synchronization.c

    @brief
    Implements functions supported in util_synchronization.h.

    Copyright (c) 2013 - 2019, 2021 Qualcomm Technologies, Inc.
    All Rights Reserved.
    Confidential and Proprietary - Qualcomm Technologies, Inc.
***************************************************************************************************/

#include "data_util_synchronization.h"

/***************************************************************************************************
    @function
    data_util_sync_data_init

    @implementation detail
    None.
***************************************************************************************************/
void data_util_sync_data_init(data_util_sync_data_type *sync_data, data_util_bit_field_type sync_data_bit_field)
{
    if(sync_data)
    {
        sync_data->sync_data_bit_field = sync_data_bit_field;
        qurt_mutex_init(&sync_data->sync_mutex);
        sync_data->sync_data_bit_field = sync_data_bit_field;
        if(data_util_bit_field_is_bits_set(sync_data_bit_field,
                                      DATA_UTIL_SYNC_BIT_FIELD_COND_VAR_USED,
                                      FALSE))
        {
            qurt_signal_init(&sync_data->sync_cond);
        }
    }
}

/***************************************************************************************************
    @function
    data_util_sync_data_destroy

    @implementation detail
    Sync object's bit field is reset.
***************************************************************************************************/
void data_util_sync_data_destroy(data_util_sync_data_type *sync_data)
{
    if(sync_data)
    {
        qurt_mutex_destroy(&sync_data->sync_mutex);
        if(data_util_bit_field_is_bits_set(sync_data->sync_data_bit_field,
                                      DATA_UTIL_SYNC_BIT_FIELD_COND_VAR_USED,
                                      FALSE))
        {
            qurt_signal_destroy(&sync_data->sync_cond);
        }
        sync_data->sync_data_bit_field = NIL;
    }
}

/***************************************************************************************************
    @function
    data_util_sync_data_acquire_mutex

    @implementation detail
    None.
***************************************************************************************************/
int data_util_sync_data_acquire_mutex(data_util_sync_data_type *sync_data)
{
    int ret;

    ret = ESUCCESS;

    if(sync_data)
    {
        qurt_mutex_lock(&sync_data->sync_mutex);
    }

    return ret;
}

/***************************************************************************************************
    @function
    data_util_sync_data_release_mutex

    @implementation detail
    None.
***************************************************************************************************/
int data_util_sync_data_release_mutex(data_util_sync_data_type *sync_data)
{
    int ret;

    ret = ESUCCESS;

    if(sync_data)
    {
        qurt_mutex_unlock(&sync_data->sync_mutex);
    }

    return ret;
}

/***************************************************************************************************
    @function
    data_util_sync_data_wait_on_cond

    @implementation detail
    get_time_of_day is used to provide the absolute time to timed wait.
***************************************************************************************************/
int data_util_sync_data_wait_on_cond(data_util_sync_data_type *sync_data,
                                int wait_for_time_seconds)
{
    int ret;

    ret = ESUCCESS;

    if(sync_data)
    {
        if(wait_for_time_seconds)
        {
            ret = qcmap_pthread_cond_wait_timed(&sync_data->sync_cond,
                                                      &sync_data->sync_mutex,
                                                      wait_for_time_seconds);
        }
        else
        {
            ret = qurt_signal_wait(&sync_data->sync_cond,
                                                CMDQ_SIGNAL_MASK,QURT_SIGNAL_ATTR_WAIT_ALL);
        }
    }
    return ret;
}


