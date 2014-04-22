/***************************************************************************************************
    @file
    data_util_memory.h

    @brief
    Facilitates memory operations by providing memory related utilities.

    Copyright (c) 2013 - 2019 Qualcomm Technologies, Inc.
    All Rights Reserved.
    Confidential and Proprietary - Qualcomm Technologies, Inc.
***************************************************************************************************/

#ifndef DATA_UTIL_MEMORY
#define DATA_UTIL_MEMORY

#include "data_utils_standard.h"
#include "data_port.h"






/***************************************************************************************************
    @function
    data_util_memory_alloc

    @brief
    Allocates a block of memory and initializes it.

    @param[in]
        size_of_memory_to_be_allocated
            size of the memory block that needs to be allocated

    @param[out]
        none

    @retval
    pointer to the block of memory If memory allocation is successful, NULL otherwise
***************************************************************************************************/
void* data_util_memory_alloc(size_t size_of_memory_to_be_allocated);






/***************************************************************************************************
    @function
    data_util_memory_free

    @brief
    Frees a block of memory and sets the pointer to the memory to NULL.

    @param[in]
    @param[out]
        to_be_freed_memory_ptr
            pointer to the pointer to the memory to be freed, pointer to memory is set to NULL

    @retval
    none
***************************************************************************************************/
void data_util_memory_free(void** to_be_freed_memory_ptr);

#endif

