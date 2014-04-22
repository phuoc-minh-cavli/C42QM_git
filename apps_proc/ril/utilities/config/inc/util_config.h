/***************************************************************************************************
    @file
    util_config.h

    @brief
    Facilitates reading configuration file.

    Copyright (c) 2015 Qualcomm Technologies, Inc.
    All Rights Reserved.
    Confidential and Proprietary - Qualcomm Technologies, Inc.

***************************************************************************************************/

#ifndef UTIL_CONFIG
#define UTIL_CONFIG

#define INTEGER 0
#define STRING 1





/***************************************************************************************************
    @function
    util_config_load

    @brief
    Read configuration file.

    @param[in]

    @param[out]
        none

    @retval
    Pointer to config object if loading config file is successful, NULL otherwise.
***************************************************************************************************/
void* util_config_load(const char *);






/***************************************************************************************************
    @function
    util_memory_free

    @brief
    Frees a block of memory and sets the pointer to the memory to NULL.

    @param[in]
    @param[out]
        to_be_freed_memory_ptr
            pointer to the pointer to the memory to be freed, pointer to memory is set to NULL

    @retval
    none
***************************************************************************************************/
void util_config_unload(void*);





/***************************************************************************************************
    @function
    util_config_get_value

    @implementation detail
    None.
***************************************************************************************************/
int util_config_get_value(void *config_list,
                          char *key,
                          int   type,
                          void *value);

#endif

