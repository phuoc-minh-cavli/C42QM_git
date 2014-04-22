/***************************************************************************************************
    @file
    data_util_synchronization.h

    @brief
    Facilitates synchronization mechanisms by providing synchronization related utilities.

    Copyright (c) 2013 - 2019 Qualcomm Technologies, Inc.
    All Rights Reserved.
    Confidential and Proprietary - Qualcomm Technologies, Inc.
***************************************************************************************************/

#ifndef DATA_UTIL_SYNCHRONIZATION
#define DATA_UTIL_SYNCHRONIZATION

#include "data_utils_standard.h"
#include "data_util_bit_field.h"
#include "data_port.h"

#define DATA_UTIL_SYNC_BIT_FIELD_NONE                ((uint64_t) 0)
#define DATA_UTIL_SYNC_BIT_FIELD_COND_VAR_USED       (((uint64_t) 1) << 0)

typedef struct data_util_sync_data_type
{
    qcmap_pthread_mutex_t sync_mutex;
    qcmap_pthread_cond_t  sync_cond;
    data_util_bit_field_type sync_data_bit_field;
}data_util_sync_data_type;






/***************************************************************************************************
    @function
    data_util_sync_data_init

    @brief
    Creates a synchronization object.

    @param[in]
        sync_data_bit_field
            bit field to indicate the specifications of the to be created sync object
            conditional variable is created if bit field contains UTIL_SYNC_BIT_FIELD_COND_VAR_USED

    @param[out]
        sync_data
            pointer to synchronization object which will
            hold the created mutex (and condition variable, If created)

    @retval
    none
***************************************************************************************************/
void data_util_sync_data_init(data_util_sync_data_type *sync_data,
                         data_util_bit_field_type sync_data_bit_field);




/***************************************************************************************************
    @function
    data_util_sync_data_destroy

    @brief
    Destroys a synchronization object.

    @param[in]
    @param[out]
        sync_data
            pointer to synchronization object which is to be destroyed

    @retval
    none
***************************************************************************************************/
void data_util_sync_data_destroy(data_util_sync_data_type *sync_data);





/***************************************************************************************************
    @function
    data_util_sync_data_acquire_mutex

    @brief
    Acquires the mutex contained by synchronization object.

    @param[in]
         sync_data
            pointer to synchronization object whose mutex needs to be acquired

    @param[out]
        none

    @retval
    ESUCCESS If mutex has been successfully acquired, FALSE otherwise
***************************************************************************************************/
int data_util_sync_data_acquire_mutex(data_util_sync_data_type *sync_data);





/***************************************************************************************************
    @function
    data_util_sync_data_release_mutex

    @brief
    Releases the mutex contained by synchronization object.

    @param[in]
         sync_data
            pointer to synchronization object whose mutex needs to be released

    @param[out]
        none

    @retval
    ESUCCESS If mutex has been successfully released, FALSE otherwise
***************************************************************************************************/
int data_util_sync_data_release_mutex(data_util_sync_data_type *sync_data);





/***************************************************************************************************
    @function
    data_util_sync_data_wait_on_cond

    @brief
    Waits on the conditional variable contained by synchronization object.

    @param[in]
         sync_data
            pointer to synchronization object whose conditional variable needs to be used for
            waiting
         wait_for_time_seconds
            time (in seconds) to wait on the conditional variable, NIL If wait needs
            to be indefinite

    @param[out]
        none

    @retval
    ESUCCESS If wait been performed on the conditional variable and the thread has been wokenup,
    appropriate error code otherwise
***************************************************************************************************/
int data_util_sync_data_wait_on_cond(data_util_sync_data_type *sync_data,
                                int wait_for_time_seconds);

#endif
